#include "stdafx.h"
#include "config.h"
#include "utils.h"
#include "vector.h"
#include "char.h"
#include "battle.h"
#include "char_manager.h"
#include "packet.h"
#include "motion.h"
#include "party.h"
#include "affect.h"
#include "buffer_manager.h"
#include "questmanager.h"
#include "p2p.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "exchange.h"
#include "sectree_manager.h"
#include "xmas_event.h"
#include "guild_manager.h"
#include "war_map.h"
#include "locale_service.h"
#include "BlueDragon.h"
#include "../../common/service.h"
#include "../../common/VnumHelper.h"
#ifdef ENABLE_ZODIAC_MISSION
	#include "dungeon.h"
#endif

BOOL g_test_server;
extern LPCHARACTER FindVictim(LPCHARACTER pkChr, int iMaxDistance);
#ifdef ENABLE_DEFENSAWE_SHIP
extern LPCHARACTER FindVictimHydra(LPCHARACTER pkChr, int iMaxDistance);
#endif

namespace
{
	class FuncFindChrForFlag
	{
		public:
			FuncFindChrForFlag(LPCHARACTER pkChr) :
				m_pkChr(pkChr), m_pkChrFind(NULL), m_iMinDistance(INT_MAX)
				{
				}

			void operator () (LPENTITY ent)
			{
				if (!ent->IsType(ENTITY_CHARACTER))
					return;

				if (ent->IsObserverMode())
					return;

				LPCHARACTER pkChr = (LPCHARACTER) ent;

				if (!pkChr->IsPC())
					return;

				if (!pkChr->GetGuild())
					return;

				if (pkChr->IsDead())
					return;

				int iDist = DISTANCE_APPROX(pkChr->GetX()-m_pkChr->GetX(), pkChr->GetY()-m_pkChr->GetY());

				if (iDist <= 500 && m_iMinDistance > iDist &&
						!pkChr->IsAffectFlag(AFF_WAR_FLAG1) &&
						!pkChr->IsAffectFlag(AFF_WAR_FLAG2) &&
						!pkChr->IsAffectFlag(AFF_WAR_FLAG3))
				{
					// If it's our flag
					if ((DWORD) m_pkChr->GetPoint(POINT_STAT) == pkChr->GetGuild()->GetID())
					{
						CWarMap * pMap = pkChr->GetWarMap();
						BYTE idx;

						if (!pMap || !pMap->GetTeamIndex(pkChr->GetGuild()->GetID(), idx))
							return;

						// Draw a flag only when there is no flag in our base. Otherwise, the flag on the base
						// I want to leave it alone, but I can be pulled out...
						if (!pMap->IsFlagOnBase(idx))
						{
							m_pkChrFind = pkChr;
							m_iMinDistance = iDist;
						}
					}
					else
					{
						// If it is the opponent's flag, draw it unconditionally.
						m_pkChrFind = pkChr;
						m_iMinDistance = iDist;
					}
				}
			}

			LPCHARACTER m_pkChr;
			LPCHARACTER m_pkChrFind;
			int m_iMinDistance;
	};

	class FuncFindChrForFlagBase
	{
		public:
			FuncFindChrForFlagBase(LPCHARACTER pkChr) : m_pkChr(pkChr)
			{
			}

			void operator () (LPENTITY ent)
			{
				if (!ent->IsType(ENTITY_CHARACTER))
					return;

				if (ent->IsObserverMode())
					return;

				LPCHARACTER pkChr = (LPCHARACTER) ent;

				if (!pkChr->IsPC())
					return;

				CGuild * pkGuild = pkChr->GetGuild();

				if (!pkGuild)
					return;

				int iDist = DISTANCE_APPROX(pkChr->GetX()-m_pkChr->GetX(), pkChr->GetY()-m_pkChr->GetY());

				if (iDist <= 500 &&
						(pkChr->IsAffectFlag(AFF_WAR_FLAG1) ||
						 pkChr->IsAffectFlag(AFF_WAR_FLAG2) ||
						 pkChr->IsAffectFlag(AFF_WAR_FLAG3)))
				{
					CAffect * pkAff = pkChr->FindAffect(AFFECT_WAR_FLAG);

					sys_log(0, "FlagBase %s dist %d aff %p flag gid %d chr gid %u",
							pkChr->GetName(), iDist, pkAff, m_pkChr->GetPoint(POINT_STAT),
							pkChr->GetGuild()->GetID());

					if (pkAff)
					{
						if ((DWORD) m_pkChr->GetPoint(POINT_STAT) == pkGuild->GetID() &&
								m_pkChr->GetPoint(POINT_STAT) != pkAff->lApplyValue)
						{
							CWarMap * pMap = pkChr->GetWarMap();
							BYTE idx;

							if (!pMap || !pMap->GetTeamIndex(pkGuild->GetID(), idx))
								return;

							//if (pMap->IsFlagOnBase(idx))
							{
								BYTE idx_opp = idx == 0 ? 1 : 0;

								SendGuildWarScore(m_pkChr->GetPoint(POINT_STAT), pkAff->lApplyValue, 1);

								pMap->ResetFlag();

								pMap->Notice(CHAT_TYPE_NOTICE, 568, "%s#%s", pMap->GetGuild(idx)->GetName(), pMap->GetGuild(idx_opp)->GetName());
							}
						}
					}
				}
			}

			LPCHARACTER m_pkChr;
	};

	class FuncFindGuardVictim
	{
		public:
			FuncFindGuardVictim(LPCHARACTER pkChr, int iMaxDistance) :
				m_pkChr(pkChr),
			m_iMinDistance(INT_MAX),
			m_iMaxDistance(iMaxDistance),
			m_lx(pkChr->GetX()),
			m_ly(pkChr->GetY()),
			m_pkChrVictim(NULL)
			{
			};

			void operator () (LPENTITY ent)
			{
				if (!ent->IsType(ENTITY_CHARACTER))
					return;

				LPCHARACTER pkChr = (LPCHARACTER) ent;

				// Don't attack the PC first
				if (pkChr->IsPC())
					return;

#ifdef ENABLE_BOT_PLAYER
				if (pkChr->IsBotCharacter())
					return;
#endif

				if (pkChr->IsNPC() && !pkChr->IsMonster())
					return;

				if (pkChr->IsDead())
					return;

				if (pkChr->IsAffectFlag(AFF_EUNHYUNG) ||
						pkChr->IsAffectFlag(AFF_INVISIBILITY) ||
						pkChr->IsAffectFlag(AFF_REVIVE_INVISIBLE))
					return;

				// dwarf pass
				if (pkChr->GetRaceNum() == 5001)
					return;

				int iDistance = DISTANCE_APPROX(m_lx - pkChr->GetX(), m_ly - pkChr->GetY());

				if (iDistance < m_iMinDistance && iDistance <= m_iMaxDistance)
				{
					m_pkChrVictim = pkChr;
					m_iMinDistance = iDistance;
				}
			}

			LPCHARACTER GetVictim()
			{
				return (m_pkChrVictim);
			}

		private:
			LPCHARACTER	m_pkChr;

			int		m_iMinDistance;
			int		m_iMaxDistance;
			long	m_lx;
			long	m_ly;

			LPCHARACTER	m_pkChrVictim;
	};
}

bool CHARACTER::IsAggressive() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_AGGRESSIVE);
}

void CHARACTER::SetAggressive()
{
	SET_BIT(m_pointsInstant.dwAIFlag, AIFLAG_AGGRESSIVE);
}

bool CHARACTER::IsCoward() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_COWARD);
}

void CHARACTER::SetCoward()
{
	SET_BIT(m_pointsInstant.dwAIFlag, AIFLAG_COWARD);
}

bool CHARACTER::IsBerserker() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_BERSERK);
}

bool CHARACTER::IsStoneSkinner() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_STONESKIN);
}

bool CHARACTER::IsGodSpeeder() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_GODSPEED);
}

bool CHARACTER::IsDeathBlower() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_DEATHBLOW);
}

bool CHARACTER::IsReviver() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_REVIVE);
}

void CHARACTER::CowardEscape()
{
	int iDist[4] = {500, 1000, 3000, 5000};

	for (int iDistIdx = 2; iDistIdx >= 0; --iDistIdx)
		for (int iTryCount = 0; iTryCount < 8; ++iTryCount)
		{
			SetRotation(number(0, 359));

			float fx, fy;
			float fDist = number(iDist[iDistIdx], iDist[iDistIdx+1]);

			GetDeltaByDegree(GetRotation(), fDist, &fx, &fy);

			bool bIsWayBlocked = false;
			for (int j = 1; j <= 100; ++j)
			{
				if (!SECTREE_MANAGER::instance().IsMovablePosition(GetMapIndex(), GetX() + (int) fx*j/100, GetY() + (int) fy*j/100))
				{
					bIsWayBlocked = true;
					break;
				}
			}

			if (bIsWayBlocked)
				continue;

			m_dwStateDuration = PASSES_PER_SEC(1);

			int iDestX = GetX() + (int) fx;
			int iDestY = GetY() + (int) fy;

			if (Goto(iDestX, iDestY))
				SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

			sys_log(0, "WAEGU move to %d %d (far)", iDestX, iDestY);
			return;
		}
}

void  CHARACTER::SetNoAttackShinsu()
{
	SET_BIT(m_pointsInstant.dwAIFlag, AIFLAG_NOATTACKSHINSU);
}
bool CHARACTER::IsNoAttackShinsu() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOATTACKSHINSU);
}

void CHARACTER::SetNoAttackChunjo()
{
	SET_BIT(m_pointsInstant.dwAIFlag, AIFLAG_NOATTACKCHUNJO);
}

bool CHARACTER::IsNoAttackChunjo() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOATTACKCHUNJO);
}

void CHARACTER::SetNoAttackJinno()
{
	SET_BIT(m_pointsInstant.dwAIFlag, AIFLAG_NOATTACKJINNO);
}

bool CHARACTER::IsNoAttackJinno() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOATTACKJINNO);
}

void CHARACTER::SetAttackMob()
{
	SET_BIT(m_pointsInstant.dwAIFlag, AIFLAG_ATTACKMOB);
}

bool CHARACTER::IsAttackMob() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_ATTACKMOB);
}

#ifdef ENABLE_SUNG_MAHI_TOWER
bool CHARACTER::IsNomove() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE);
}

void CHARACTER::SetNomove()
{
	SET_BIT(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE);
}

void CHARACTER::RemoveNomove()
{
	REMOVE_BIT(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE);
}

bool CHARACTER::IsNoattack() const
{
	return IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOATTACK);
}

void CHARACTER::SetNoattack()
{
	SET_BIT(m_pointsInstant.dwAIFlag, AIFLAG_NOATTACK);
}

void CHARACTER::RemoveNoattack()
{
	REMOVE_BIT(m_pointsInstant.dwAIFlag, AIFLAG_NOATTACK);
}
#endif


// STATE_IDLE_REFACTORING
void CHARACTER::StateIdle()
{
	if (IsStone())
	{
		__StateIdle_Stone();
		return;
	}
	else if (IsWarp() || IsGoto())
	{
		// Warps are treated as events
		m_dwStateDuration = 60 * passes_per_sec;
		return;
	}

	if (IsPC()
#ifndef ENABLE_BOT_PLAYER
		|| IsBotCharacter()
#endif
	)
		return;

	// NPC handling
	if (!IsMonster())
	{
		__StateIdle_NPC();
		return;
	}

	__StateIdle_Monster();
}

void CHARACTER::__StateIdle_Stone()
{
	m_dwStateDuration = PASSES_PER_SEC(1);

	int iPercent = 0; // @fixme136
	if (GetMaxHP() >= 0)
		iPercent = (GetHP() * 100) / GetMaxHP();
	DWORD dwVnum = number(MIN(GetMobTable().sAttackSpeed, GetMobTable().sMovingSpeed ), MAX(GetMobTable().sAttackSpeed, GetMobTable().sMovingSpeed));

	if (iPercent <= 10 && GetMaxSP() < 10)
	{
		SetMaxSP(10);
		SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0);

		CHARACTER_MANAGER::instance().SelectStone(this);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 500, GetY() - 500, GetX() + 500, GetY() + 500);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1500, GetY() - 1500, GetX() + 1500, GetY() + 1500);
		CHARACTER_MANAGER::instance().SelectStone(NULL);
	}
	else if (iPercent <= 20 && GetMaxSP() < 9)
	{
		SetMaxSP(9);
		SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0);

		CHARACTER_MANAGER::instance().SelectStone(this);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 500, GetY() - 500, GetX() + 500, GetY() + 500);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1500, GetY() - 1500, GetX() + 1500, GetY() + 1500);
		CHARACTER_MANAGER::instance().SelectStone(NULL);
	}
	else if (iPercent <= 30 && GetMaxSP() < 8)
	{
		SetMaxSP(8);
		SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0);

		CHARACTER_MANAGER::instance().SelectStone(this);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 500, GetY() - 500, GetX() + 500, GetY() + 500);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SelectStone(NULL);
	}
	else if (iPercent <= 40 && GetMaxSP() < 7)
	{
		SetMaxSP(7);
		SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0);

		CHARACTER_MANAGER::instance().SelectStone(this);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SelectStone(NULL);
	}
	else if (iPercent <= 50 && GetMaxSP() < 6)
	{
		SetMaxSP(6);
		SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0);

		CHARACTER_MANAGER::instance().SelectStone(this);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SelectStone(NULL);
	}
	else if (iPercent <= 60 && GetMaxSP() < 5)
	{
		SetMaxSP(5);
		SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0);

		CHARACTER_MANAGER::instance().SelectStone(this);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 500, GetY() - 500, GetX() + 500, GetY() + 500);
		CHARACTER_MANAGER::instance().SelectStone(NULL);
	}
	else if (iPercent <= 70 && GetMaxSP() < 4)
	{
		SetMaxSP(4);
		SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0);

		CHARACTER_MANAGER::instance().SelectStone(this);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 500, GetY() - 500, GetX() + 500, GetY() + 500);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SelectStone(NULL);
	}
	else if (iPercent <= 80 && GetMaxSP() < 3)
	{
		SetMaxSP(3);
		SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0);

		CHARACTER_MANAGER::instance().SelectStone(this);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SelectStone(NULL);

#ifdef ENABLE_ZODIAC_MISSION
		if(GetMapIndex() >= 3570000 && GetMapIndex() <= 3579999 && GetDungeon() != NULL)
		{
			int PortalFlag = GetDungeon()->GetFlag("Portal");
			if(PortalFlag >= 1 && PortalFlag <= 12)
			{
				DWORD vnum[] = {0, 35740, 35730,35726,35736,35744,35734,35746,35732,35742,35748,35728,35738};
				SetMaxSP(3);
				SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0);
				CHARACTER_MANAGER::instance().SelectStone(this);
				CHARACTER_MANAGER::instance().SpawnGroup(vnum[PortalFlag], GetMapIndex(), GetX() - 500, GetY() - 500, GetX() + 500, GetY() + 500);
				CHARACTER_MANAGER::instance().SelectStone(NULL);
			}
		}
#endif
	}
	else if (iPercent <= 90 && GetMaxSP() < 2)
	{
		SetMaxSP(2);
		SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0);

		CHARACTER_MANAGER::instance().SelectStone(this);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 500, GetY() - 500, GetX() + 500, GetY() + 500);
		CHARACTER_MANAGER::instance().SelectStone(NULL);
	}
	else if (iPercent <= 99 && GetMaxSP() < 1)
	{
		SetMaxSP(1);
		SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0);

		CHARACTER_MANAGER::instance().SelectStone(this);
		CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, GetMapIndex(), GetX() - 1000, GetY() - 1000, GetX() + 1000, GetY() + 1000);
		CHARACTER_MANAGER::instance().SelectStone(NULL);
	}
	else
		return;

	UpdatePacket();
	return;
}

void CHARACTER::__StateIdle_NPC()
{
	MonsterChat(MONSTER_CHAT_WAIT);
	m_dwStateDuration = PASSES_PER_SEC(5);

	// Idle processing of the pet system is handled by CPetActor::Update, not the state machine shared by almost all types of characters.
#ifdef ENABLE_MOUNT_SYSTEM
	if (IsMount())
		return;
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (IsGrowthPet())
		return;
#endif

	if (IsPet())
		return;

	else if (IsGuardNPC())
	{
		if (!quest::CQuestManager::instance().GetEventFlag("noguard"))
		{
			FuncFindGuardVictim f(this, 50000);

			if (GetSectree())
				GetSectree()->ForEachAround(f);

			LPCHARACTER victim = f.GetVictim();

			if (victim)
			{
				m_dwStateDuration = passes_per_sec/2;

				if (CanBeginFight())
					BeginFight(victim);
			}
		}
	}
	else
	{
		if (GetRaceNum() == xmas::MOB_SANTA_VNUM) // Santa
		{
			if (get_dword_time() > m_dwPlayStartTime)
			{
				int	next_warp_time = 2 * 1000; // 2 seconds

				m_dwPlayStartTime = get_dword_time() + next_warp_time;

				// Time has passed, so let's warp.
				/*
				* For santa
				const int WARP_MAP_INDEX_NUM = 4;
				static const long c_lWarpMapIndexs[WARP_MAP_INDEX_NUM] = {61, 62, 63, 64};
				*/
				// Shinseonja No Haeyong
				const int WARP_MAP_INDEX_NUM = 7;
				static const long c_lWarpMapIndexs[WARP_MAP_INDEX_NUM] = { 61, 62, 63, 64, 3, 23, 43 };
				long lNextMapIndex;
				lNextMapIndex = c_lWarpMapIndexs[number(1, WARP_MAP_INDEX_NUM) - 1];

				if (map_allow_find(lNextMapIndex))
				{
					// It is here.
					M2_DESTROY_CHARACTER(this);
					int iNextSpawnDelay = 50 * 60;

					xmas::SpawnSanta(lNextMapIndex, iNextSpawnDelay);
				}
				else
				{
					TPacketGGXmasWarpSanta p;
					p.bHeader   = HEADER_GG_XMAS_WARP_SANTA;
					p.bChannel  = g_bChannel;
					p.lMapIndex = lNextMapIndex;
					P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGXmasWarpSanta));
				}
				return;
			}
		}

		if (!IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
		{
			//
			// It moves from place to place.
			//
			LPCHARACTER pkChrProtege = GetProtege();

			if (pkChrProtege)
			{
				if (DISTANCE_APPROX(GetX() - pkChrProtege->GetX(), GetY() - pkChrProtege->GetY()) > 500)
				{
					if (Follow(pkChrProtege, number(100, 300)))
						return;
				}
			}

			if (!number(0, 6))
			{
				SetRotation(number(0, 359));

				float fx, fy;
				float fDist = number(200, 400);

				GetDeltaByDegree(GetRotation(), fDist, &fx, &fy);

				// Loose nailing property check; If you can't go between the final and intermediate positions, don't go.
				if (!(SECTREE_MANAGER::instance().IsMovablePosition(GetMapIndex(), GetX() + (int) fx, GetY() + (int) fy)
					&& SECTREE_MANAGER::instance().IsMovablePosition(GetMapIndex(), GetX() + (int) fx / 2, GetY() + (int) fy / 2)))
					return;

				SetNowWalking(true);

				if (Goto(GetX() + (int) fx, GetY() + (int) fy))
					SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

				return;
			}
		}
	}
}

void CHARACTER::__StateIdle_Monster()
{
#ifdef ENABLE_MELEY_LAIR_DUNGEON
	if(IsMeley())
	{
		if(!IsStone() && time(0) > GetProtectTime("MeleyTime"))
		{
			if (HasMobSkill())
			{
				for (unsigned int iSkillIdx = 0; iSkillIdx < MOB_SKILL_MAX_NUM; ++iSkillIdx)
				{
					if (CanUseMobSkill(iSkillIdx))
					{
						if (UseMobSkill(iSkillIdx))
						{
							SendMovePacket(FUNC_MOB_SKILL, iSkillIdx, GetX(), GetY(), 0, get_dword_time());
							float fDuration = CMotionManager::instance().GetMotionDuration(GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, MOTION_SPECIAL_1 + iSkillIdx));
							m_dwStateDuration = (DWORD) (fDuration == 0.0f ? PASSES_PER_SEC(2) : PASSES_PER_SEC(fDuration));
							return;
						}
					}
				}
			}
			SetProtectTime("MeleyTime",time(0)+5);
		}
		return;
	}
#endif
	
	if (IsStun())
		return;

	if (!CanMove())
		return;

	if (IsCoward())
	{
		// Cowardly monsters only run away.
		if (!IsDead())
			CowardEscape();

		return;
	}

	if (IsBerserker())
		if (IsBerserk())
			SetBerserk(false);

	if (IsGodSpeeder())
		if (IsGodSpeed())
			SetGodSpeed(false);

	LPCHARACTER victim = GetVictim();

	if (!victim || victim->IsDead())
	{
		SetVictim(NULL);
		victim = NULL;
		m_dwStateDuration = PASSES_PER_SEC(1);
	}

#ifdef FIX_BLOCK_MOB_SAFEZONE
	if (victim && victim->GetSectree() && victim->GetSectree()->IsAttr(victim->GetX(), victim->GetY(), ATTR_BANPK)) 
	{
		SetVictim(NULL);
		victim = NULL;
		m_dwStateDuration = PASSES_PER_SEC(1);
	}
#endif

	if (!victim || victim->IsBuilding())
	{
		// stone protection treatment
		if (m_pkChrStone)
		{
			victim = m_pkChrStone->GetNearestVictim(m_pkChrStone);
		}
		// Preemptive monster treatment
		else if (!no_wander && IsAggressive())
		{
			if (GetMapIndex() == 61 && quest::CQuestManager::instance().GetEventFlag("xmas_tree"))
			{

			}
			else
			{
#ifdef ENABLE_DEFENSAWE_SHIP
				if (IsHydraMob() || IsHydra())
				{
					victim = FindVictimHydra(this, 40000);
				}
				else
				{
					victim = FindVictim(this, m_pkMobData->m_table.wAggressiveSight);
				}
#else
				victim = FindVictim(this, m_pkMobData->m_table.wAggressiveSight);
#endif
			}
		}
	}

	if (victim && !victim->IsDead())
	{
		SECTREE *tree = victim->GetSectree();
		if (tree && tree->IsAttr(victim->GetX(), victim->GetY(), ATTR_BANPK))
			return;
      
		if (CanBeginFight())
			BeginFight(victim);

		return;
	}

	if (IsAggressive() && !victim)
		m_dwStateDuration = PASSES_PER_SEC(number(1, 3));
	else
		m_dwStateDuration = PASSES_PER_SEC(number(3, 5));

	LPCHARACTER pkChrProtege = GetProtege();

	// If it is far from the thing to protect (stone, party leader), follow it.
	if (pkChrProtege)
	{
		if (DISTANCE_APPROX(GetX() - pkChrProtege->GetX(), GetY() - pkChrProtege->GetY()) > 1000)
		{
			if (Follow(pkChrProtege, number(150, 400)))
			{
				MonsterLog("[IDLE] 리더로부터 너무 멀리 떨어졌다! 복귀한다.");
				return;
			}
		}
	}

	//
	// just go back and forth
	//
	if (!no_wander && !IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
	{
		if (!number(0, 6))
		{
			SetRotation(number(0, 359));

			float fx, fy;
			float fDist = number(300, 700);

			GetDeltaByDegree(GetRotation(), fDist, &fx, &fy);

			// Loose nail property check; If the final and intermediate positions cannot be reached, do not go.
			if (!(SECTREE_MANAGER::instance().IsMovablePosition(GetMapIndex(), GetX() + (int) fx, GetY() + (int) fy)
						&& SECTREE_MANAGER::instance().IsMovablePosition(GetMapIndex(), GetX() + (int) fx/2, GetY() + (int) fy/2)))
				return;

			// NOTE: When a monster wanders around in IDLE state, it is currently run unconditionally. (Never walk)
			// The graphics team wants to see the monsters walking, so they temporarily walk or run with a certain probability. (It only works once in test mode, because the overall feel of the game is wrong)
			if (g_test_server)
			{
				if (number(0, 100) < 60)
					SetNowWalking(false);
				else
					SetNowWalking(true);
			}

			if (Goto(GetX() + (int) fx, GetY() + (int) fy))
				SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

			return;
		}
	}

	MonsterChat(MONSTER_CHAT_WAIT);
}
// END_OF_STATE_IDLE_REFACTORING

bool __CHARACTER_GotoNearTarget(LPCHARACTER self, LPCHARACTER victim)
{
	if (IS_SET(self->GetAIFlag(), AIFLAG_NOMOVE))
		return false;

	switch (self->GetMobBattleType())
	{
		case BATTLE_TYPE_RANGE:
		case BATTLE_TYPE_MAGIC:
			// The wizard or archer goes up to 80% of the attack distance and starts attacking.
			if (self->Follow(victim, self->GetMobAttackRange() * 8 / 10))
				return true;
			break;

		default:
			// 90% of the rest?
			if (self->Follow(victim, self->GetMobAttackRange() * 9 / 10))
				return true;
	}

	return false;
}

void CHARACTER::StateMove()
{
	DWORD dwElapsedTime = get_dword_time() - m_dwMoveStartTime;
	float fRate = (float) dwElapsedTime / (float) m_dwMoveDuration;

	if (fRate > 1.0f)
		fRate = 1.0f;

	int x = (int) ((float) (m_posDest.x - m_posStart.x) * fRate + m_posStart.x);
	int y = (int) ((float) (m_posDest.y - m_posStart.y) * fRate + m_posStart.y);

	Move(x, y);

	if (IsPC() && (thecore_pulse() & 15) == 0)
	{
		UpdateSectree();

		if (GetExchange())
		{
			LPCHARACTER victim = GetExchange()->GetCompany()->GetOwner();
			int iDist = DISTANCE_APPROX(GetX() - victim->GetX(), GetY() - victim->GetY());

			// distance check
			if (iDist >= EXCHANGE_MAX_DISTANCE)
			{
				GetExchange()->Cancel();
			}
		}
	}

	if (IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| IsBotCharacter()
#endif
	)
	{
#ifdef ENABLE_BOT_PLAYER
		if (!IsBotCharacter())
#endif
		if (IsWalking() && GetStamina() < GetMaxStamina())
		{
			// Stamina increases after 5 seconds
			if (get_dword_time() - GetWalkStartTime() > 5000)
				PointChange(POINT_STAMINA, GetMaxStamina() / 1);
		}

		// If you're running while you're in combat (botlar icin ST tuketilmez - yurume moduna gecmezler)
		if (!IsWalking() && !IsRiding()){
			if ((get_dword_time() - GetLastAttackTime()) < 20000)
			{
				StartAffectEvent();

				if (IsStaminaHalfConsume())
				{
					if (thecore_pulse()&1)
						PointChange(POINT_STAMINA, -STAMINA_PER_STEP);
				}
				else
					PointChange(POINT_STAMINA, -STAMINA_PER_STEP);

				StartStaminaConsume();

				if (GetStamina() <= 0)
				{
					// I don't have enough stamina so I have to walk
					SetStamina(0);
					SetNowWalking(true);
					StopStaminaConsume();
				}
			}
			else if (IsStaminaConsume())
			{
				StopStaminaConsume();
			}
		}
	}
#ifdef ENABLE_BOT_PLAYER
	else if (IsBotCharacter());
#endif
	else
	{
		// XXX AGGRO
		if (IsMonster() && GetVictim())
		{
			LPCHARACTER victim = GetVictim();
			UpdateAggrPoint(victim, DAMAGE_TYPE_NORMAL, -(victim->GetLevel() / 3 + 1));

			if (g_test_server)
			{
				// If the monster is chasing the enemy, it will run unconditionally.
				SetNowWalking(false);
			}
		}

		if (IsMonster() && GetMobRank() >= MOB_RANK_BOSS && GetVictim())
		{
			LPCHARACTER victim = GetVictim();

			// giant tortoise
			if (GetRaceNum() == 2191 && number(1, 20) == 1 && get_dword_time() - m_pkMobInst->m_dwLastWarpTime > 1000)
			{
				// warp test
				float fx, fy;
				GetDeltaByDegree(victim->GetRotation(), 400, &fx, &fy);
				long new_x = victim->GetX() + (long)fx;
				long new_y = victim->GetY() + (long)fy;
				SetRotation(GetDegreeFromPositionXY(new_x, new_y, victim->GetX(), victim->GetY()));
				Show(victim->GetMapIndex(), new_x, new_y, 0, true);
				GotoState(m_stateBattle);
				m_dwStateDuration = 1;
				ResetMobSkillCooltime();
				m_pkMobInst->m_dwLastWarpTime = get_dword_time();
				return;
			}

			// TODO Change direction and be less stupid!
			if (number(0, 3) == 0)
			{
				if (__CHARACTER_GotoNearTarget(this, victim))
					return;
			}
		}
	}

	if (1.0f == fRate)
	{
		if (IsPC())
		{
			sys_log(1, "도착 %s %d %d", GetName(), x, y);
			GotoState(m_stateIdle);
			StopStaminaConsume();
		}
#ifdef ENABLE_BOT_PLAYER
		else if (IsBotCharacter());
#endif
		else
		{
			if (GetVictim() && !IsCoward())
			{
				if (!IsState(m_stateBattle))
					MonsterLog("[BATTLE] 근처에 왔으니 공�?시작 %s", GetVictim()->GetName());

				GotoState(m_stateBattle);
				m_dwStateDuration = 1;
			}
			else
			{
				if (!IsState(m_stateIdle))
					MonsterLog("[IDLE] 대상이 없으니 쉬자");

				GotoState(m_stateIdle);

				//LPCHARACTER rider = GetRider();

				m_dwStateDuration = PASSES_PER_SEC(number(1, 3));
			}
		}
	}
}

void CHARACTER::StateBattle()
{
	if (IsStone())
	{
		sys_err("Stone must not use battle state (name %s)", GetName());
		return;
	}

	if (IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| IsBotCharacter()
#endif
	)
		return;

	if (!CanMove())
		return;

	if (IsStun())
		return;

	LPCHARACTER victim = GetVictim();

	if (IsCoward())
	{
		if (IsDead())
			return;

		SetVictim(NULL);

		if (number(1, 50) != 1)
		{
			GotoState(m_stateIdle);
			m_dwStateDuration = 1;
		}
		else
			CowardEscape();

		return;
	}

	if (!victim || (victim->IsStun() && IsGuardNPC()) || victim->IsDead()
#ifdef FIX_BLOCK_MOB_SAFEZONE
		|| (victim->GetSectree() && victim->GetSectree()->IsAttr(victim->GetX(), victim->GetY(), ATTR_BANPK))
#endif
	)
	{
		if ((victim && victim->IsDead() && !no_wander && IsAggressive() && (!GetParty() || GetParty()->GetLeader() == this))
#ifdef FIX_BLOCK_MOB_SAFEZONE
			|| (victim && victim->GetSectree() && victim->GetSectree()->IsAttr(victim->GetX(), victim->GetY(), ATTR_BANPK) && !no_wander && IsAggressive() && (!GetParty() || GetParty()->GetLeader() == this))
#endif
		)
		{
			LPCHARACTER new_victim = NULL;
#ifdef ENABLE_DEFENSAWE_SHIP
			if(IsHydraMob() || IsHydra())
				new_victim = FindVictimHydra(this, 40000);
			else
				new_victim = FindVictim(this, m_pkMobData->m_table.wAggressiveSight);
#else
			LPCHARACTER new_victim = FindVictim(this, m_pkMobData->m_table.wAggressiveSight);
#endif
			SetVictim(new_victim);
			m_dwStateDuration = PASSES_PER_SEC(1);

			if (!new_victim)
			{
				switch (GetMobBattleType())
				{
					case BATTLE_TYPE_MELEE:
					case BATTLE_TYPE_SUPER_POWER:
					case BATTLE_TYPE_SUPER_TANKER:
					case BATTLE_TYPE_POWER:
					case BATTLE_TYPE_TANKER:
						{
							float fx, fy;
							float fDist = number(400, 1500);

							GetDeltaByDegree(number(0, 359), fDist, &fx, &fy);

							if (SECTREE_MANAGER::instance().IsMovablePosition(victim->GetMapIndex(),
										victim->GetX() + (int) fx,
										victim->GetY() + (int) fy) &&
									SECTREE_MANAGER::instance().IsMovablePosition(victim->GetMapIndex(),
										victim->GetX() + (int) fx/2,
										victim->GetY() + (int) fy/2))
							{
								float dx = victim->GetX() + fx;
								float dy = victim->GetY() + fy;

								SetRotation(GetDegreeFromPosition(dx, dy));

								if (Goto((long) dx, (long) dy))
								{
									sys_log(0, "KILL_AND_GO: %s distance %.1f", GetName(), fDist);
									SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
								}
							}
						}
				}
			}
			return;
		}

		SetVictim(NULL);

		if (IsGuardNPC())
			Return();

		m_dwStateDuration = PASSES_PER_SEC(1);
		return;
	}

	if (IsSummonMonster() && !IsDead() && !IsStun())
	{
		if (!GetParty())
		{
			// Summon and make a party to fill.
			CPartyManager::instance().CreateParty(this);
		}

		LPPARTY pParty = GetParty();
		bool bPct = !number(0, 3);

		if (bPct && pParty->CountMemberByVnum(GetSummonVnum()) < SUMMON_MONSTER_COUNT)
		{
			MonsterLog("부하 몬스터 소환!");

			int sx = GetX() - 300;
			int sy = GetY() - 300;
			int ex = GetX() + 300;
			int ey = GetY() + 300;

			LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRange(GetSummonVnum(), GetMapIndex(), sx, sy, ex, ey, true, true);

			if (tch)
			{
				pParty->Join(tch->GetVID());
				pParty->Link(tch);
			}
		}
	}

	LPCHARACTER pkChrProtege = GetProtege();

	float fDist = DISTANCE_APPROX(GetX() - victim->GetX(), GetY() - victim->GetY());
	float fDistCheck = 15000.0f;
	
#ifdef ENABLE_SUNG_MAHI_TOWER
	if (GetMapIndex() >= (354 * 10000) && GetMapIndex() < ((354 + 1) * 10000))
		fDistCheck = 16000.0f;
#endif

	if (fDist >= 4000.0f)
	{
		MonsterLog("타겟이 멀어서 포기");
		SetVictim(NULL);

		if (pkChrProtege)
			if (DISTANCE_APPROX(GetX() - pkChrProtege->GetX(), GetY() - pkChrProtege->GetY()) > 1000)
				Follow(pkChrProtege, number(150, 400));

		return;
	}

	if (fDist >= GetMobAttackRange() * 1.15)
	{
		__CHARACTER_GotoNearTarget(this, victim);
		return;
	}

	if (m_pkParty)
		m_pkParty->SendMessage(this, PM_ATTACKED_BY, 0, 0);

	if (2493 == m_pkMobData->m_table.dwVnum)
	{
		// Water Dragon (2493) Special Treatment
		m_dwStateDuration = BlueDragon_StateBattle(this);
		return;
	}

	DWORD dwCurTime = get_dword_time();
	DWORD dwDuration = CalculateDuration(GetLimitPoint(POINT_ATT_SPEED), 2000);

	if ((dwCurTime - m_dwLastAttackTime) < dwDuration) // Water Dragon (2493) Special Treatment
	{
		m_dwStateDuration = MAX(1, (passes_per_sec * (dwDuration - (dwCurTime - m_dwLastAttackTime)) / 1000));
		return;
	}

	if (IsBerserker() == true)
		if (GetHPPct() < m_pkMobData->m_table.bBerserkPoint)
			if (IsBerserk() != true)
				SetBerserk(true);

	if (IsGodSpeeder() == true)
		if (GetHPPct() < m_pkMobData->m_table.bGodSpeedPoint)
			if (IsGodSpeed() != true)
				SetGodSpeed(true);

	//
	// mob skill handling
	//
	if (HasMobSkill())
	{
		for (unsigned int iSkillIdx = 0; iSkillIdx < MOB_SKILL_MAX_NUM; ++iSkillIdx)
		{
			if (CanUseMobSkill(iSkillIdx))
			{
				SetRotationToXY(victim->GetX(), victim->GetY());

				if (UseMobSkill(iSkillIdx))
				{
					SendMovePacket(FUNC_MOB_SKILL, iSkillIdx, GetX(), GetY(), 0, dwCurTime);

					float fDuration = CMotionManager::instance().GetMotionDuration(GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, MOTION_SPECIAL_1 + iSkillIdx));
					m_dwStateDuration = (DWORD) (fDuration == 0.0f ? PASSES_PER_SEC(2) : PASSES_PER_SEC(fDuration));

					if (test_server)
						sys_log(0, "USE_MOB_SKILL: %s idx %u motion %u duration %.0f", GetName(), iSkillIdx, MOTION_SPECIAL_1 + iSkillIdx, fDuration);

					return;
				}
			}
		}
	}

	if (!Attack(victim)) // What if the attack fails? Why did it fail? TODO
		m_dwStateDuration = passes_per_sec / 2;
	else
	{
		// Makes the enemy look
		SetRotationToXY(victim->GetX(), victim->GetY());

		SendMovePacket(FUNC_ATTACK, 0, GetX(), GetY(), 0, dwCurTime);

		float fDuration = CMotionManager::instance().GetMotionDuration(GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, MOTION_NORMAL_ATTACK));
		m_dwStateDuration = (DWORD) (fDuration == 0.0f ? PASSES_PER_SEC(2) : PASSES_PER_SEC(fDuration));
	}
}

void CHARACTER::StateFlag()
{
	m_dwStateDuration = (DWORD) PASSES_PER_SEC(0.5);

	CWarMap * pMap = GetWarMap();

	if (!pMap)
		return;

	FuncFindChrForFlag f(this);
	GetSectree()->ForEachAround(f);

	if (!f.m_pkChrFind)
		return;

	if (NULL == f.m_pkChrFind->GetGuild())
		return;

	char buf[256];
	BYTE idx;

	if (!pMap->GetTeamIndex(GetPoint(POINT_STAT), idx))
		return;

	f.m_pkChrFind->AddAffect(AFFECT_WAR_FLAG, POINT_NONE, GetPoint(POINT_STAT), idx == 0 ? AFF_WAR_FLAG1 : AFF_WAR_FLAG2, INFINITE_AFFECT_DURATION, 0, false);
	f.m_pkChrFind->AddAffect(AFFECT_WAR_FLAG, POINT_MOV_SPEED, 50 - f.m_pkChrFind->GetPoint(POINT_MOV_SPEED), 0, INFINITE_AFFECT_DURATION, 0, false);

	pMap->RemoveFlag(idx);

	pMap->Notice(CHAT_TYPE_NOTICE, 568, "%s#%s", pMap->GetGuild(idx)->GetName(), f.m_pkChrFind->GetName());
}

void CHARACTER::StateFlagBase()
{
	m_dwStateDuration = (DWORD) PASSES_PER_SEC(0.5);

	FuncFindChrForFlagBase f(this);
	GetSectree()->ForEachAround(f);
}

void CHARACTER::StateHorse()
{
#ifdef ENABLE_BOT_PLAYER
	if (IsBotCharacter())
		return;
#endif
	float	START_FOLLOW_DISTANCE = 400.0f; // Start chasing if you fall more than this distance
	float	START_RUN_DISTANCE = 700.0f; // If you fall more than this distance, run to chase.
	int		MIN_APPROACH = 150; // min approach distance
	int		MAX_APPROACH = 300; // max approach distance

	DWORD	STATE_DURATION = (DWORD)PASSES_PER_SEC(0.5); // state duration

	bool bDoMoveAlone = true; // Whether to move around alone when close to the character -_-;
	bool bRun = true; // Should I run?

	if (IsDead())
		return;

	m_dwStateDuration = STATE_DURATION;

	LPCHARACTER victim = GetRider();

	// ! no // if there is no target, the summoner will clear me directly
	if (!victim)
	{
		M2_DESTROY_CHARACTER(this);
		return;
	}

	// m_pkMobInst null olabilir (karakter destroy edilirken)
	if (m_pkMobInst == NULL)
		return;

	m_pkMobInst->m_posLastAttacked = GetXYZ();

	float fDist = DISTANCE_APPROX(GetX() - victim->GetX(), GetY() - victim->GetY());

	if (fDist >= START_FOLLOW_DISTANCE)
	{
		if (fDist > START_RUN_DISTANCE)
			SetNowWalking(!bRun); // NOTE: I thought it would stop when I saw the function name, but when I use SetNowWalking(false) it starts running.. -_-;

		Follow(victim, number(MIN_APPROACH, MAX_APPROACH));

		m_dwStateDuration = STATE_DURATION;
	}
	else if (bDoMoveAlone && (get_dword_time() > m_dwLastAttackTime))
	{
		// wondering-.-
		m_dwLastAttackTime = get_dword_time() + number(5000, 12000);

		SetRotation(number(0, 359)); // direction is set randomly

		float fx, fy;
		float fDist = number(200, 400);

		GetDeltaByDegree(GetRotation(), fDist, &fx, &fy);

		// Check for loose nail properties; If the final and intermediate positions cannot be reached, do not go.
		if (!(SECTREE_MANAGER::instance().IsMovablePosition(GetMapIndex(), GetX() + (int) fx, GetY() + (int) fy)
					&& SECTREE_MANAGER::instance().IsMovablePosition(GetMapIndex(), GetX() + (int) fx/2, GetY() + (int) fy/2)))
			return;

		SetNowWalking(true);

		if (Goto(GetX() + (int) fx, GetY() + (int) fy))
			SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	}
}
