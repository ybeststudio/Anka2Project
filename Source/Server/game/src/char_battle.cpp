#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "desc.h"
#include "desc_manager.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "battle.h"
#include "pvp.h"
#include "skill.h"
#include "start_position.h"
#include "profiler.h"
#include "cmd.h"
#include "dungeon.h"
#include "log.h"
#include "unique_item.h"
#include "priv_manager.h"
#include "db.h"
#include "vector.h"
#include "marriage.h"
#include "arena.h"
#include "regen.h"
#include "exchange.h"
#include "shop_manager.h"
#include "ani.h"
#include "BattleArena.h"
#include "packet.h"
#include "party.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "questmanager.h"
#include "questlua.h"
#include "BlueDragon.h"
#include "DragonLair.h"
#include <random>
#include <algorithm>
#include "../../common/service.h"
#ifdef ENABLE_BUFFI_SYSTEM
#include "buffi_system.h"
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	#include "offlineshop_manager.h"
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	#include "growth_pet.h"
#endif

#ifdef ENABLE_ULTIMATE_REGEN
	#include "new_mob_timer.h"
#endif

#ifdef ENABLE_DUNGEON_INFO
	#include "dungeon_info.h"
#endif

#ifdef ENABLE_WHITE_DRAGON
	#include "WhiteDragon.h"
#endif

#ifdef ENABLE_QUEEN_NETHIS
	#include "SnakeLair.h"
#endif

#ifdef ENABLE_OCHAO_TEMPLE_SYSTEM
	#include "TempleOchao.h"
#endif

#ifdef ENABLE_RESP_SYSTEM
	#include "resp_manager.h"
#endif

#ifdef ENABLE_STONE_EVENT_SYSTEM
	#include "stone_event.h"
#endif

DWORD AdjustExpByLevel(const LPCHARACTER ch, const DWORD exp)
{
	if (PLAYER_EXP_TABLE_MAX < ch->GetLevel())
	{
		double ret = 0.95;
		double factor = 0.1;

		for (ssize_t i = 0; i < ch->GetLevel() - 100; ++i)
		{
			if ((i % 10) == 0)
				factor /= 2.0;

			ret *= 1.0 - factor;
		}

		ret = ret * static_cast<double>(exp);

		if (ret < 1.0)
			return 1;

		return static_cast<DWORD>(ret);
	}

	return exp;
}

bool CHARACTER::CanBeginFight() const
{
	if (!CanMove())
		return false;

	return m_pointsInstant.position == POS_STANDING && !IsDead() && !IsStun();
}

void CHARACTER::BeginFight(LPCHARACTER pkVictim)
{
	if (!pkVictim)
		return;

	SetVictim(pkVictim);
	SetPosition(POS_FIGHTING);
	SetNextStatePulse(1);
}

bool CHARACTER::CanFight() const
{
	return m_pointsInstant.position >= POS_FIGHTING ? true : false;
}

void CHARACTER::CreateFly(BYTE bType, LPCHARACTER pkVictim)
{
	if (!pkVictim)
		return;

	TPacketGCCreateFly packFly;

	packFly.bHeader         = HEADER_GC_CREATE_FLY;
	packFly.bType           = bType;
	packFly.dwStartVID      = GetVID();
	packFly.dwEndVID        = pkVictim->GetVID();

	PacketAround(&packFly, sizeof(TPacketGCCreateFly));
}

void CHARACTER::DistributeSP(LPCHARACTER pkKiller, int iMethod)
{
	if (!pkKiller)
		return;

	if (pkKiller->GetSP() >= pkKiller->GetMaxSP())
		return;

	bool bAttacking = (get_dword_time() - GetLastAttackTime()) < 3000;
	bool bMoving = (get_dword_time() - GetLastMoveTime()) < 3000;

	if (iMethod == 1)
	{
		int num = number(0, 3);

		if (!num)
		{
			int iLvDelta = GetLevel() - pkKiller->GetLevel();
			int iAmount = 0;

			if (iLvDelta >= 5)
				iAmount = 10;
			else if (iLvDelta >= 0)
				iAmount = 6;
			else if (iLvDelta >= -3)
				iAmount = 2;

			if (iAmount != 0)
			{
				iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;

				if (iAmount >= 11)
					CreateFly(FLY_SP_BIG, pkKiller);
				else if (iAmount >= 7)
					CreateFly(FLY_SP_MEDIUM, pkKiller);
				else
					CreateFly(FLY_SP_SMALL, pkKiller);

				pkKiller->PointChange(POINT_SP, iAmount);
			}
		}
	}
	else
	{
		if (pkKiller->GetJob() == JOB_SHAMAN || (pkKiller->GetJob() == JOB_SURA && pkKiller->GetSkillGroup() == 2))
		{
			int iAmount;

			if (bAttacking)
				iAmount = 2 + GetMaxSP() / 100;
			else if (bMoving)
				iAmount = 3 + GetMaxSP() * 2 / 100;
			else
				iAmount = 10 + GetMaxSP() * 3 / 100; // normally

			iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;
			pkKiller->PointChange(POINT_SP, iAmount);
		}
		else
		{
			int iAmount;

			if (bAttacking)
				iAmount = 2 + pkKiller->GetMaxSP() / 200;
			else if (bMoving)
				iAmount = 2 + pkKiller->GetMaxSP() / 100;
			else
			{
				// normally
				if (pkKiller->GetHP() < pkKiller->GetMaxHP())
					iAmount = 2 + (pkKiller->GetMaxSP() / 100); // When there is no blood
				else
					iAmount = 9 + (pkKiller->GetMaxSP() / 100); // basic
			}

			iAmount += (iAmount * pkKiller->GetPoint(POINT_SP_REGEN)) / 100;
			pkKiller->PointChange(POINT_SP, iAmount);
		}
	}
}

bool CHARACTER::Attack(LPCHARACTER pkVictim, BYTE bType)
{
	if (test_server)
		sys_log(0, "[TEST_SERVER] Attack : %s type %d, MobBattleType %d", GetName(), bType, !GetMobBattleType() ? 0 : GetMobAttackRange());
	//PROF_UNIT puAttack("Attack");
	if (!CanMove() || IsObserverMode())	//@fixme453
		return false;

	// @fixme131
	if (!battle_is_attackable(this, pkVictim))
		return false;

	DWORD dwCurrentTime = get_dword_time();

	if (IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| IsBotCharacter()
#endif
		)
	{
		if (IS_SPEED_HACK(this, pkVictim, dwCurrentTime))
			return false;

		if (bType == 0 && dwCurrentTime < GetSkipComboAttackByTime())
			return false;
	}
	else
	{
		MonsterChat(MONSTER_CHAT_ATTACK);
	}

#ifdef __AUTO_HUNT__
	// Metin/slot rezervasyon - SADECE AUTO_HUNT aktif olan oyuncular için
	// Not: Rezervasyon kontrolü saldýrý sýrasýnda yapýlmaz, sadece hedef seçiminde kullanýlýr
	// Böylece birden fazla oyuncu ayný hedefe damage verebilir
	if (IsPC() && m_bAutoHuntStatus && pkVictim && (pkVictim->IsStone() || pkVictim->IsNPC()))
	{
		DWORD dwTargetVID = pkVictim->GetVID();
		DWORD dwPlayerPID = GetPlayerID();
		
		// Hedefi rezerve et (hedef seçiminde farklý hedeflere yönlendirmek için)
		// Ama saldýrý sýrasýnda rezervasyon kontrolü yapmýyoruz, damage iþlemesi için
		CHARACTER_MANAGER::instance().ReserveTarget(dwTargetVID, dwPlayerPID);
	}
#endif

	pkVictim->SetSyncOwner(this);

	if (pkVictim->CanBeginFight())
		pkVictim->BeginFight(this);

	int iRet;

#ifdef ENABLE_OCHAO_TEMPLE_SYSTEM
	if ((pkVictim->IsMonster()) && (pkVictim->GetMobTable().dwVnum == TEMPLE_OCHAO_GUARDIAN) && (pkVictim->GetMapIndex() == TEMPLE_OCHAO_MAP_INDEX))
	{
		TempleOchao::CMgr::instance().GuardianAttacked();
	}
#endif

	if (bType == 0)
	{
		switch (GetMobBattleType())
		{
			case BATTLE_TYPE_MELEE:
			case BATTLE_TYPE_POWER:
			case BATTLE_TYPE_TANKER:
			case BATTLE_TYPE_SUPER_POWER:
			case BATTLE_TYPE_SUPER_TANKER:
				iRet = battle_melee_attack(this, pkVictim);
				break;

			case BATTLE_TYPE_RANGE:
				FlyTarget(pkVictim->GetVID(), pkVictim->GetX(), pkVictim->GetY(), HEADER_CG_FLY_TARGETING);
				iRet = Shoot(0) ? BATTLE_DAMAGE : BATTLE_NONE;
				break;

			case BATTLE_TYPE_MAGIC:
				FlyTarget(pkVictim->GetVID(), pkVictim->GetX(), pkVictim->GetY(), HEADER_CG_FLY_TARGETING);
				iRet = Shoot(1) ? BATTLE_DAMAGE : BATTLE_NONE;
				break;

			default:
				sys_err("Unhandled battle type %d", GetMobBattleType());
				iRet = BATTLE_NONE;
				break;
		}
	}
	else
	{
		if (IsPC() == true
#ifdef ENABLE_BOT_PLAYER
			|| IsBotCharacter()
#endif
			)
		{
			if (dwCurrentTime - m_dwLastSkillTime > 1500)
			{
				sys_log(1, "HACK: Too long skill using term. Name(%s) PID(%u) delta(%u)",
					GetName(), GetPlayerID(), (dwCurrentTime - m_dwLastSkillTime));
				return false;
			}
		}

		sys_log(1, "Attack call ComputeSkill %d %s", bType, pkVictim ? pkVictim->GetName() : "");
		iRet = ComputeSkill(bType, pkVictim);
	}

	//if (test_server && IsPC())
	//	sys_log(0, "%s Attack %s type %u ret %d", GetName(), pkVictim->GetName(), bType, iRet);

	if (iRet != BATTLE_NONE) {	//@fixme455
		pkVictim->SetSyncOwner(this);

		if (pkVictim->CanBeginFight()) {
			pkVictim->BeginFight(this);
		}
	}

#ifdef ENABLE_MELEY_LAIR_DUNGEON
	if(IsPC() && (pkVictim->IsMonster() || pkVictim->IsStone()))
	{
		SetQuestNPCIDAttack(pkVictim->GetVID());
		quest::CQuestManager::instance().Attack(GetPlayerID(), pkVictim->GetRaceNum());
	}
#endif

	if (iRet == BATTLE_DAMAGE || iRet == BATTLE_DEAD)
	{
		OnMove(true);
		pkVictim->OnMove();

		// only pc sets victim null. For npc, state machine will reset this.
		if (BATTLE_DEAD == iRet && IsPC())
			SetVictim(NULL);

		return true;
	}

	return false;
}

void CHARACTER::DeathPenalty(BYTE bTown)
{
	sys_log(1, "DEATH_PERNALY_CHECK(%s) town(%d)", GetName(), bTown);

	Cube_close(this);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CloseAcce();
#endif

	if (GetLevel() < 10)
	{
		sys_log(0, "NO_DEATH_PENALTY_LESS_LV10(%s)", GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You did not lose any Experience because of the Blessing of the Dragon God."));
		return;
	}

	if (number(0, 2))
	{
		sys_log(0, "NO_DEATH_PENALTY_LUCK(%s)", GetName());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You did not lose any Experience because of the Blessing of the Dragon God."));
		return;
	}

	if (IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY))
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);

		// NO_DEATH_PENALTY_BUG_FIX
		if (!bTown)
		{
			if (FindAffect(AFFECT_NO_DEATH_PENALTY))
			{
				sys_log(0, "NO_DEATH_PENALTY_AFFECT(%s)", GetName());
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You did not lose any Experience because of the Blessing of the Dragon God."));
				RemoveAffect(AFFECT_NO_DEATH_PENALTY);
				return;
			}
		}
		// END_OF_NO_DEATH_PENALTY_BUG_FIX

		int iLoss = ((GetNextExp() * aiExpLossPercents[MINMAX(1, GetLevel(), PLAYER_EXP_TABLE_MAX)]) / 100);

		iLoss = MIN(800000, iLoss);

		if (bTown)
			iLoss = 0;

		if (IsEquipUniqueItem(UNIQUE_ITEM_TEARDROP_OF_GODNESS))
			iLoss /= 2;

		sys_log(0, "DEATH_PENALTY(%s) EXP_LOSS: %d percent %d%%", GetName(), iLoss, aiExpLossPercents[MIN(gPlayerMaxLevel, GetLevel())]);

		PointChange(POINT_EXP, -iLoss, true);
	}
}

bool CHARACTER::IsStun() const
{
	if (IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN))
		return true;

	return false;
}

EVENTFUNC(StunEvent)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "StunEvent> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL)
	{
		return 0;
	}

	ch->m_pkStunEvent = NULL;
	ch->Dead();
	return 0;
}

void CHARACTER::Stun()
{
	if (IsStun())
		return;

	if (IsDead())
		return;

	if (!IsPC() && m_pkParty)
	{
		m_pkParty->SendMessage(this, PM_ATTACKED_BY, 0, 0);
	}

	sys_log(1, "%s: Stun %p", GetName(), this);

	PointChange(POINT_HP_RECOVERY, -GetPoint(POINT_HP_RECOVERY));
	PointChange(POINT_SP_RECOVERY, -GetPoint(POINT_SP_RECOVERY));

	CloseMyShop();

	event_cancel(&m_pkRecoveryEvent);

	TPacketGCStun pack;
	pack.header	= HEADER_GC_STUN;
	pack.vid	= m_vid;
	PacketAround(&pack, sizeof(pack));

	SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

	if (m_pkStunEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkStunEvent = event_create(StunEvent, info, PASSES_PER_SEC(0));
}

#ifdef ENABLE_BOT_PLAYER
EVENTINFO(SBotCharacterDeadEventInfo)
{
	DynamicCharacterPtr ch;
};

EVENTFUNC(bot_character_dead_event)
{
	auto info = dynamic_cast<SBotCharacterDeadEventInfo*>(event->info);
	if (info == nullptr)
	{
		sys_err("<bot_character_dead_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (!ch)
	{
		sys_err("BOT_DEAD_EVENT: cannot find bot pointer.");
		return 0;
	}
	ch->m_pkBotCharacterDeadEvent = nullptr;

	if (ch->IsBotCharacter())
	{
		ch->SetPosition(POS_STANDING);
		ch->RestartAtSamePos();
		ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
		ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
		ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
		ch->ReviveInvisible(5);
	}
	else
	{
		M2_DESTROY_CHARACTER(ch);
	}

	return 0;
}
#endif

EVENTINFO(SCharDeadEventInfo)
{
	bool isPC;
	uint32_t dwID;

	SCharDeadEventInfo()
		: isPC(0)
		, dwID(0)
	{
	}
};

EVENTFUNC(dead_event)
{
	const SCharDeadEventInfo* info = dynamic_cast<SCharDeadEventInfo*>(event->info);

	if ( info == NULL )
	{
		sys_err( "dead_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = NULL;

	if (true == info->isPC)
	{
		ch = CHARACTER_MANAGER::instance().FindByPID( info->dwID );
	}
	else
	{
		ch = CHARACTER_MANAGER::instance().Find( info->dwID );
	}

	if (NULL == ch)
	{
		sys_err("DEAD_EVENT: cannot find char pointer with %s id(%d)", info->isPC ? "PC" : "MOB", info->dwID );
		return 0;
	}

	ch->m_pkDeadEvent = NULL;

	if (ch->GetDesc())
	{
		ch->GetDesc()->SetPhase(PHASE_GAME);

		ch->SetPosition(POS_STANDING);

		PIXEL_POSITION pos;

		if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
			ch->WarpSet(pos.x, pos.y);
		else
		{
			sys_err("cannot find spawn position (name %s)", ch->GetName());
			ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		}

		ch->PointChange(POINT_HP, (ch->GetMaxHP() / 2) - ch->GetHP(), true);

		ch->DeathPenalty(0);

		ch->StartRecoveryEvent();

		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
	}
	else
	{
		if (ch->IsMonster() == true)
		{
			if (ch->IsRevive() == false && ch->HasReviverInParty() == true)
			{
				ch->SetPosition(POS_STANDING);
				ch->SetHP(ch->GetMaxHP());

				ch->ViewReencode();

				ch->SetAggressive();
				ch->SetRevive(true);

				return 0;
			}
		}

		M2_DESTROY_CHARACTER(ch);
	}

	return 0;
}

bool CHARACTER::IsDead() const
{
	if (m_pointsInstant.position == POS_DEAD)
		return true;

	return false;
}

#define GetGoldMultipler() (distribution_test_server ? 3 : 1)

void CHARACTER::RewardGold(LPCHARACTER pkAttacker)
{
	// ADD_PREMIUM
	const bool isAutoLoot =
		(pkAttacker->GetPremiumRemainSeconds(PREMIUM_AUTOLOOT) > 0 ||
			pkAttacker->IsEquipUniqueGroup(UNIQUE_GROUP_AUTOLOOT))
		? true : false; // third hand
		// END_OF_ADD_PREMIUM

	PIXEL_POSITION pos;

	if (!isAutoLoot)
	{
		if (!SECTREE_MANAGER::instance().GetMovablePosition(GetMapIndex(), GetX(), GetY(), pos))
			return;
	}

	int iTotalGold = 0;
	//
	// --------- Calculate Money Drop Probability ----------
	//
	int iGoldPercent = MobRankStats[GetMobRank()].iGoldPercent;

	if (pkAttacker->IsPC())
		iGoldPercent = iGoldPercent * (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD_DROP)) / 100;

	if (pkAttacker->GetPoint(POINT_MALL_GOLDBONUS))
		iGoldPercent += (iGoldPercent * pkAttacker->GetPoint(POINT_MALL_GOLDBONUS) / 100);

	iGoldPercent = iGoldPercent * CHARACTER_MANAGER::instance().GetMobGoldDropRate(pkAttacker) / 100;

	// ADD_PREMIUM
	if (pkAttacker->GetPremiumRemainSeconds(PREMIUM_GOLD) > 0 ||
			pkAttacker->IsEquipUniqueGroup(UNIQUE_GROUP_LUCKY_GOLD))
		iGoldPercent += iGoldPercent;
	// END_OF_ADD_PREMIUM

	if (iGoldPercent > 100)
		iGoldPercent = 100;

	int iPercent = 0;

	if (GetMobRank() >= MOB_RANK_BOSS)
		iPercent = ((iGoldPercent * PERCENT_LVDELTA_BOSS(pkAttacker->GetLevel(), GetLevel())) / 100);
	else
		iPercent = ((iGoldPercent * PERCENT_LVDELTA(pkAttacker->GetLevel(), GetLevel())) / 100);
	//int iPercent = CALCULATE_VALUE_LVDELTA(pkAttacker->GetLevel(), GetLevel(), iGoldPercent);

	if (number(1, 100) > iPercent)
		return;

	int iGoldMultipler = GetGoldMultipler();

#ifdef ENABLE_GOLD_MULTIPLIER
	if (1 == number(1, 50000)) // 1/50000 chance of 10x money
		iGoldMultipler *= 10;
	else if (1 == number(1, 10000)) // 1/10000 chance of 5x money
		iGoldMultipler *= 5;
#endif

	// personal application
	if (pkAttacker->GetPoint(POINT_GOLD_DOUBLE_BONUS))
	{
		if (number(1, 100) <= pkAttacker->GetPoint(POINT_GOLD_DOUBLE_BONUS))
			iGoldMultipler *= 2;
	}

#ifdef ENABLE_GOLD_MULTIPLIER
	//
	// --------- money drop multiple decision ----------
	//
	if (test_server)
		pkAttacker->ChatPacket(CHAT_TYPE_PARTY, "gold_mul %d rate %d", iGoldMultipler, CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker));
#endif

	//
	// --------- actual drop handling -------------
	//
	LPITEM item;

#ifdef ENABLE_GOLD10_PCT
	int iGold10DropPct = 100;
	iGold10DropPct = (iGold10DropPct * 100) / (100 + CPrivManager::instance().GetPriv(pkAttacker, PRIV_GOLD10_DROP));
#endif

	// If MOB_RANK is higher than BOSS, it is an unconditional money bomb.
	if (GetMobRank() >= MOB_RANK_BOSS && !IsStone() && GetMobTable().dwGoldMax != 0)
	{
#ifdef ENABLE_GOLD10_PCT
		if (1 == number(1, iGold10DropPct))
			iGoldMultipler *= 10; // 10x money with 1% chance
#endif

		const int iSplitCount = number(25, 35);

		for (int i = 0; i < iSplitCount; ++i)
		{
			int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax) / iSplitCount;
			if (test_server)
				sys_log(0, "iGold %d", iGold);
			iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker) / 100;
			iGold *= iGoldMultipler;

			if (iGold == 0)
			{
				continue;
			}

			if (test_server)
			{
				sys_log(0, "Drop Moeny MobGoldAmountRate %d %d", CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker), iGoldMultipler);
				sys_log(0, "Drop Money gold %d GoldMin %d GoldMax %d", iGold, GetMobTable().dwGoldMax, GetMobTable().dwGoldMax);
			}

			// NOTE: Money bombs do not deal with third hand
			if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold)))
			{
				pos.x = GetX() + ((number(-14, 14) + number(-14, 14)) * 23);
				pos.y = GetY() + ((number(-14, 14) + number(-14, 14)) * 23);

				item->AddToGround(GetMapIndex(), pos);
				item->StartDestroyEvent();

				iTotalGold += iGold; // Total gold
			}
		}
	}

	// 1% chance to drop 10 money. (10x drop)
#ifdef ENABLE_GOLD10_PCT
	else if (1 == number(1, iGold10DropPct))
	{
		//
		// money bomb drop
		//
		for (int i = 0; i < 10; ++i)
		{
			int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax);
			iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker) / 100;
			iGold *= iGoldMultipler;

			if (iGold == 0)
			{
				continue;
			}

			// NOTE: Money bombs do not deal with third hand
			if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold)))
			{
				pos.x = GetX() + (number(-7, 7) * 20);
				pos.y = GetY() + (number(-7, 7) * 20);

				item->AddToGround(GetMapIndex(), pos);
				item->StartDestroyEvent();

				iTotalGold += iGold; // Total gold
			}
		}
	}
#endif
	else
	{
		//
		// money drop in the usual way
		//
		int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax);
		iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(pkAttacker) / 100;
		iGold *= iGoldMultipler;

		int iSplitCount = 0;

		if (iGold >= 3)
			iSplitCount = number(1, 3);

		else if (GetMobRank() >= MOB_RANK_BOSS)
		{
			iSplitCount = number(3, 10);

			if ((iGold / iSplitCount) == 0)
				iSplitCount = 1;
		}
		else
			iSplitCount = 1;

		if (iGold != 0)
		{
			iTotalGold += iGold; // Total gold

			for (int i = 0; i < iSplitCount; ++i)
			{
				if (isAutoLoot)
				{
					pkAttacker->GiveGold(iGold / iSplitCount);
				}
				else if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold / iSplitCount)))
				{
					pos.x = GetX() + (number(-7, 7) * 20);
					pos.y = GetY() + (number(-7, 7) * 20);

					item->AddToGround(GetMapIndex(), pos);
					item->StartDestroyEvent();
				}
			}
		}
	}

	DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER, GetRaceNum(), iTotalGold);
}

#ifdef ENABLE_ATTENDANCE_EVENT
void CHARACTER::RewardAttendance()
{
	for (TDamageMap::iterator it = m_map_kDamage.begin(); it != m_map_kDamage.end(); ++it)
	{
		int iDamage = it->second.iTotalDamage;
		if (iDamage > 0)
		{
			LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->first);

			if (ch)
			{
				DWORD dwCount = 0;
				
				std::string flagNames[5] = {
					"attendance_event.red_dragon_hit_points",
					"attendance_event.jotun_thrym_hit_points",
					"attendance_event.razador_hit_points",
					"attendance_event.nemere_hit_points",
					"attendance_event.beran_hit_points",
				};
				
				if(!ch->m_hitCount.empty())
				{
					for (DWORD i = 0; i < ch->m_hitCount.size(); i++)
					{
						if(ch->m_hitCount[i].dwVid == GetVID())
						{
							dwCount = ch->m_hitCount[i].dwCount;
							break;
						}
					}
				}
				
				if(dwCount)
				{
					if(ch->GetLevel() < 30)
					{
						continue;
					}
					
					if (!ch->FindAffect(AFFECT_EXP_BONUS_EVENT))
					{
						ch->AddAffect(AFFECT_EXP_BONUS_EVENT, POINT_EXP, 20, 0, 1800, 0, false);
					}
					
					if (ch->FindAffect(AFFECT_ATT_SPEED_SLOW))
					{
						ch->RemoveAffect(AFFECT_ATT_SPEED_SLOW);
					}

					ch->SetQuestFlag(flagNames[6504 - GetRaceNum()], ch->GetQuestFlag(flagNames[6504 - GetRaceNum()]) + dwCount);
	
					time_t iTime; time(&iTime); tm* pTimeInfo = localtime(&iTime);
					char szFlagname[32 + 1];
					snprintf(szFlagname, sizeof(szFlagname), "attendance.clear_day_%d", pTimeInfo->tm_yday);
					
					if(ch->GetQuestFlag(szFlagname) == 0)
					{
						ch->SetQuestFlag(szFlagname, 1);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Sistem> Günlük hediyenizi kazand?n?z."));
						
						TPacketGCAttendanceEvent packEvent;
						packEvent.bHeader = HEADER_GC_ATTENDANCE_EVENT;
						packEvent.bType = 2;
						packEvent.bValue = 1;
						ch->GetDesc()->Packet(&packEvent, sizeof(TPacketGCAttendanceEvent));
					}
				}
			}
		}
	}
}
#endif

void CHARACTER::Reward(bool bItemDrop)
{
	if (GetRaceNum() == 5001) // Waegoo drop money unconditionally
	{
		PIXEL_POSITION pos;

		if (!SECTREE_MANAGER::instance().GetMovablePosition(GetMapIndex(), GetX(), GetY(), pos))
			return;

		LPITEM item;
		int iGold = number(GetMobTable().dwGoldMin, GetMobTable().dwGoldMax);
		iGold = iGold * CHARACTER_MANAGER::instance().GetMobGoldAmountRate(NULL) / 100;
		iGold *= GetGoldMultipler();
		const int iSplitCount = number(25, 35);

		sys_log(0, "WAEGU Dead gold %d split %d", iGold, iSplitCount);

		for (int i = 1; i <= iSplitCount; ++i)
		{
			if ((item = ITEM_MANAGER::instance().CreateItem(1, iGold / iSplitCount)))
			{
				if (i != 0)
				{
					pos.x = number(-7, 7) * 20;
					pos.y = number(-7, 7) * 20;

					pos.x += GetX();
					pos.y += GetY();
				}

				item->AddToGround(GetMapIndex(), pos);
				item->StartDestroyEvent();
			}
		}
		return;
	}

	//PROF_UNIT puReward("Reward");
	LPCHARACTER pkAttacker = DistributeExp();

	if (!pkAttacker
#ifdef ENABLE_KILL_EVENT_FIX
		&& !(pkAttacker = GetMostAttacked())
#endif
		)
	{
		return;
	}

	//PROF_UNIT pu1("r1");
	if (pkAttacker->IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| pkAttacker->IsBotCharacter()
#endif
		)
	{
		if ((GetLevel() - pkAttacker->GetLevel()) >= -10)
		{
			if (pkAttacker->GetRealAlignment() < 0)
			{
				if (pkAttacker->IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_KILL))
					pkAttacker->UpdateAlignment(14);
				else
					pkAttacker->UpdateAlignment(7);
			}
			else
				pkAttacker->UpdateAlignment(2);
		}

#ifdef ENABLE_ATTENDANCE_EVENT
		if(GetRaceNum() >= 6500 && GetRaceNum() <= 6504)
		{
			RewardAttendance();
		}
#endif

		pkAttacker->SetQuestNPCID(GetVID());
		quest::CQuestManager::instance().Kill(pkAttacker->GetPlayerID(), GetRaceNum());
		CHARACTER_MANAGER::instance().KillLog(GetRaceNum());

#ifdef ENABLE_HUNTING_SYSTEM
		pkAttacker->UpdateHuntingMission(GetRaceNum());
#endif

#ifdef ENABLE_BOSS_KILL_NOTICE
		std::vector<int> canavarlistesi { 191 , 192 , 193 , 194 , 491 , 492 , 493 , 494 , 531 , 532 , 533 , 534 , 591 , 691 , 791 , 792 , 1091 , 1092 , 1093 , 1095 , 1191 , 1192 , 1304 , 1901 , 2091 , 2206 , 2307 , 2306 , 2491 , 2492 , 2494 , 2495 , 2597 , 2598 , 3190 , 3191 , 3290 , 3291 , 3390 , 3391 , 3490 , 3491 , 3590 , 3591 , 3690 , 3691 , 3790 , 3791 , 3890 , 3891 , 6390 , 6391 , 3090 , 3091 , 2191 , 6421 , 4204 , 4209 , 4210 , 3596 , 23689};

		for (auto &i: canavarlistesi)
		{
			if (GetRaceNum() == i)
			{
				auto pkMob = CMobManager::instance().Get(i);
				if (pkMob) {
					char szKillNotice[QUERY_MAX_LEN];
					snprintf(szKillNotice, sizeof(szKillNotice), "[CH%d] : %s, %s tarafýndan katledildi.", g_bChannel, pkMob->m_table.szLocaleName, pkAttacker->GetName());
					BroadcastNotice(szKillNotice);
				}
			}
		}
#endif

		if (!number(0, 9))
		{
			if (pkAttacker->GetPoint(POINT_KILL_HP_RECOVERY))
			{
				int iHP = pkAttacker->GetMaxHP() * pkAttacker->GetPoint(POINT_KILL_HP_RECOVERY) / 100;
				pkAttacker->PointChange(POINT_HP, iHP);
				CreateFly(FLY_HP_SMALL, pkAttacker);
			}

			if (pkAttacker->GetPoint(POINT_KILL_SP_RECOVER))
			{
				int iSP = pkAttacker->GetMaxSP() * pkAttacker->GetPoint(POINT_KILL_SP_RECOVER) / 100;
				pkAttacker->PointChange(POINT_SP, iSP);
				CreateFly(FLY_SP_SMALL, pkAttacker);
			}
		}
	}
	//pu1.Pop();

	if (!bItemDrop)
		return;

#ifdef ENABLE_MULTI_FARM_BLOCK
	if (!pkAttacker->GetRewardStatus())
		return;
#endif

	PIXEL_POSITION pos = GetXYZ();

	if (!SECTREE_MANAGER::Instance().GetMovablePosition(GetMapIndex(), pos.x, pos.y, pos))
		return;

	//
	// money drop
	//
	//PROF_UNIT pu2("r2");
	if (test_server)
		sys_log(0, "Drop money : Attacker %s", pkAttacker->GetName());
	RewardGold(pkAttacker);
	//pu2.Pop();

	//
	// drop an item
	//
	//PROF_UNIT pu3("r3");
	LPITEM item;

	static std::vector<LPITEM> s_vec_item;
	s_vec_item.clear();

	if (ITEM_MANAGER::instance().CreateDropItem(this, pkAttacker, s_vec_item))
	{
		if (s_vec_item.size() == 0);
		else if (s_vec_item.size() == 1)
		{
			item = s_vec_item[0];
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
			if(pkAttacker->FindAffect(AFFECT_AUTO_PICK_UP) && IS_SET(pkAttacker->GetPickUPMode(), AUTOMATIC_PICK_UP_ACTIVATE) && pkAttacker->CheckItemCanGet(item))
			{
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
				pkAttacker->SendPickupItemPacket(item->GetVnum());
#else
				pkAttacker->LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item->GetName());
#endif
				pkAttacker->AutoGiveItem(item);
			}
			else
			{
#endif
			item->AddToGround(GetMapIndex(), pos);

			if (CBattleArena::instance().IsBattleArenaMap(pkAttacker->GetMapIndex()) == false)
			{
#ifdef ENABLE_DICE_SYSTEM
				if (pkAttacker->GetParty())
				{
					FPartyDropDiceRoll f(item, pkAttacker);
					f.Process(this);
				}
				else
					item->SetOwnership(pkAttacker);
#else
				item->SetOwnership(pkAttacker);
#endif
			}

			item->StartDestroyEvent();

			pos.x = number(-7, 7) * 20;
			pos.y = number(-7, 7) * 20;
			pos.x += GetX();
			pos.y += GetY();

			sys_log(1, "DROP_ITEM: %s %d %d from %s", item->GetName(), pos.x, pos.y, GetName());
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
			}
#endif
		}
		else
		{
			int iItemIdx = s_vec_item.size() - 1;

			std::priority_queue<std::pair<int, LPCHARACTER> > pq;

			int total_dam = 0;

			for (TDamageMap::iterator it = m_map_kDamage.begin(); it != m_map_kDamage.end(); ++it)
			{
				int iDamage = it->second.iTotalDamage;
				if (iDamage > 0)
				{
					LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->first);

					if (ch)
					{
						pq.push(std::make_pair(iDamage, ch));
						total_dam += iDamage;
					}
				}
			}

			std::vector<LPCHARACTER> v;

			while (!pq.empty() && pq.top().first * 10 >= total_dam)
			{
				v.push_back(pq.top().second);
				pq.pop();
			}

			if (v.empty())
			{
				// No one has done any special damage, so no ownership
				while (iItemIdx >= 0)
				{
					item = s_vec_item[iItemIdx--];

					if (!item)
					{
						sys_err("item null in vector idx %d", iItemIdx + 1);
						continue;
					}

					item->AddToGround(GetMapIndex(), pos);
					// Those who inflict less than 10% damage have no ownership
					//item->SetOwnership(pkAttacker);
					item->StartDestroyEvent();

					pos.x = number(-7, 7) * 20;
					pos.y = number(-7, 7) * 20;
					pos.x += GetX();
					pos.y += GetY();

					sys_log(1, "DROP_ITEM: %s %d %d by %s", item->GetName(), pos.x, pos.y, GetName());
				}
			}
			else
			{
				// Ownership is divided only between those who have dealt a lot of damage.
				std::vector<LPCHARACTER>::iterator it = v.begin();

				while (iItemIdx >= 0)
				{
					item = s_vec_item[iItemIdx--];

					if (!item)
					{
						sys_err("item null in vector idx %d", iItemIdx + 1);
						continue;
					}

					LPCHARACTER ch = *it;

					if (ch->GetParty())
						ch = ch->GetParty()->GetNextOwnership(ch, GetX(), GetY());

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
					if(ch->FindAffect(AFFECT_AUTO_PICK_UP) && IS_SET(ch->GetPickUPMode(), AUTOMATIC_PICK_UP_ACTIVATE) && ch->CheckItemCanGet(item))
					{
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
						ch->SendPickupItemPacket(item->GetVnum());
#else
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item->GetName());
#endif
						ch->AutoGiveItem(item);

						++it;

						if (it == v.end())
							it = v.begin();
						continue;
					}
#endif

					item->AddToGround(GetMapIndex(), pos);

					++it;

					if (it == v.end())
						it = v.begin();

					if (CBattleArena::instance().IsBattleArenaMap(ch->GetMapIndex()) == false)
					{
#ifdef ENABLE_DICE_SYSTEM
						if (ch->GetParty())
						{
							FPartyDropDiceRoll f(item, ch);
							f.Process(this);
						}
						else
							item->SetOwnership(ch);
#else
						item->SetOwnership(ch);
#endif
					}

					item->StartDestroyEvent();

					pos.x = number(-7, 7) * 20;
					pos.y = number(-7, 7) * 20;
					pos.x += GetX();
					pos.y += GetY();

					sys_log(1, "DROP_ITEM: %s %d %d by %s", item->GetName(), pos.x, pos.y, GetName());
				}
			}
		}
	}

	m_map_kDamage.clear();
}

struct TItemDropPenalty
{
	int iInventoryPct;		// Range: 1 ~ 1000
	int iInventoryQty;		// Range: --
	int iEquipmentPct;		// Range: 1 ~ 100
	int iEquipmentQty;		// Range: --
};

TItemDropPenalty aItemDropPenalty_kor[9] =
{
	{   0,   0,  0,  0 },
	{   0,   0,  0,  0 },
	{   0,   0,  0,  0 },
	{   0,   0,  0,  0 },
	{   0,   0,  0,  0 },
	{  25,   1,  5,  1 },
	{  50,   2, 10,  1 },
	{  75,   4, 15,  1 },
	{ 100,   8, 20,  1 },
};

void CHARACTER::ItemDropPenalty(LPCHARACTER pkKiller)
{
	// Items are not dropped while the private store is open.
	if (GetMyShop())
		return;

	if (IsGM())
		return;

	if (GetLevel() < 50)
		return;

	//@fixme469
	if (CArenaManager::Instance().IsArenaMap(GetMapIndex()))
		return;

	if (CBattleArena::Instance().IsBattleArenaMap(GetMapIndex()))
		return;

	//@fixme320
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true || IsWarping() || m_pkTimedEvent)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¢¯e¨öAAC ¨¬¢¬E¡Ì¡¤I ¨ú¨¡AIAUAI ¢Ò©ø¨úiAoAo ¨úE¨úO¨öA¢¥I¢¥U."));
		return;
	}
	//@fixme320

	switch (GetMapIndex())
	{
		case 92: // PvP Arena
		case 113: // OX Event
			return;
		default:
			break;
	}

	struct TItemDropPenalty * table = &aItemDropPenalty_kor[0];

	if (GetLevel() < 10)
		return;

	int iAlignIndex;

	if (GetRealAlignment() >= 120000)
		iAlignIndex = 0;
	else if (GetRealAlignment() >= 80000)
		iAlignIndex = 1;
	else if (GetRealAlignment() >= 40000)
		iAlignIndex = 2;
	else if (GetRealAlignment() >= 10000)
		iAlignIndex = 3;
	else if (GetRealAlignment() >= 0)
		iAlignIndex = 4;
	else if (GetRealAlignment() > -40000)
		iAlignIndex = 5;
	else if (GetRealAlignment() > -80000)
		iAlignIndex = 6;
	else if (GetRealAlignment() > -120000)
		iAlignIndex = 7;
	else
		iAlignIndex = 8;

	std::vector<std::pair<LPITEM, int> > vec_item;
	LPITEM pkItem;
	int	i;
	bool isDropAllEquipments = false;

	TItemDropPenalty & r = table[iAlignIndex];
	sys_log(0, "%s align %d inven_pct %d equip_pct %d", GetName(), iAlignIndex, r.iInventoryPct, r.iEquipmentPct);

	bool bDropInventory = r.iInventoryPct >= number(1, 1000);
	bool bDropEquipment = r.iEquipmentPct >= number(1, 100);
	bool bDropAntiDropUniqueItem = false;

	if ((bDropInventory || bDropEquipment) && IsEquipUniqueItem(UNIQUE_ITEM_SKIP_ITEM_DROP_PENALTY))
	{
		bDropInventory = false;
		bDropEquipment = false;
		bDropAntiDropUniqueItem = true;
	}

	// @fixme198 BEGIN
	if (bDropInventory || bDropEquipment)
	{
		if (pkKiller)
			pkKiller->SetExchangeTime();
		SetExchangeTime();
	}
	// @fixme198 END

	if (bDropInventory) // Drop Inventory
	{
		std::vector<BYTE> vec_bSlots;
		const auto isQuestRunning = quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning();

		for (i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			auto pkItem = GetInventoryItem(i);
			if (!pkItem || (isQuestRunning && pkItem->GetType() == ITEM_QUEST)) // @fixme198 (item_quest items can be used without being consumed)
				continue;
			vec_bSlots.push_back(i);
		}

		if (!vec_bSlots.empty())
		{
			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(vec_bSlots.begin(), vec_bSlots.end(), g);

			int iQty = MIN(vec_bSlots.size(), r.iInventoryQty);

			if (iQty)
				iQty = number(1, iQty);

			for (i = 0; i < iQty; ++i)
			{
				pkItem = GetInventoryItem(vec_bSlots[i]);

				if (!pkItem) { continue; }

				if (IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_PKDROP))
					continue;

				SyncQuickslot(QUICKSLOT_TYPE_ITEM, vec_bSlots[i], 255);
				vec_item.push_back(std::make_pair(pkItem->RemoveFromCharacter(), INVENTORY));
			}
		}
		else if (iAlignIndex == 8)
			isDropAllEquipments = true;
	}

	if (bDropEquipment) // Drop Equipment
	{
		std::vector<BYTE> vec_bSlots;

		for (i = 0; i < WEAR_MAX_NUM; ++i)
			if (GetWear(i))
				vec_bSlots.push_back(i);

		if (!vec_bSlots.empty())
		{
			std::random_device rd;
			std::mt19937 g(rd());
			std::shuffle(vec_bSlots.begin(), vec_bSlots.end(), g);
			int iQty;

			if (isDropAllEquipments)
				iQty = vec_bSlots.size();
			else
				iQty = MIN(vec_bSlots.size(), number(1, r.iEquipmentQty));

			if (iQty)
				iQty = number(1, iQty);

			for (i = 0; i < iQty; ++i)
			{
				pkItem = GetWear(vec_bSlots[i]);
				if (!pkItem) { continue; }

				if (IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_PKDROP))
					continue;

				SyncQuickslot(QUICKSLOT_TYPE_ITEM, vec_bSlots[i], 255);
				vec_item.push_back(std::make_pair(pkItem->RemoveFromCharacter(), EQUIPMENT));
			}
		}
	}

	if (bDropAntiDropUniqueItem)
	{
		LPITEM pkItem;

		pkItem = GetWear(WEAR_UNIQUE1);

		if (pkItem && pkItem->GetVnum() == UNIQUE_ITEM_SKIP_ITEM_DROP_PENALTY)
		{
			SyncQuickslot(QUICKSLOT_TYPE_ITEM, WEAR_UNIQUE1, 255);
			vec_item.push_back(std::make_pair(pkItem->RemoveFromCharacter(), EQUIPMENT));
		}

		pkItem = GetWear(WEAR_UNIQUE2);

		if (pkItem && pkItem->GetVnum() == UNIQUE_ITEM_SKIP_ITEM_DROP_PENALTY)
		{
			SyncQuickslot(QUICKSLOT_TYPE_ITEM, WEAR_UNIQUE2, 255);
			vec_item.push_back(std::make_pair(pkItem->RemoveFromCharacter(), EQUIPMENT));
		}

#ifdef ENABLE_MOUNT_SYSTEM
		pkItem = GetWear(WEAR_MOUNT);

		if (pkItem && pkItem->GetVnum() == UNIQUE_ITEM_SKIP_ITEM_DROP_PENALTY)
		{
			SyncQuickslot(QUICKSLOT_TYPE_ITEM, WEAR_MOUNT, 255);
			vec_item.push_back(std::make_pair(pkItem->RemoveFromCharacter(), EQUIPMENT));
		}
#endif
	}

	{
		PIXEL_POSITION pos;
		pos.x = GetX();
		pos.y = GetY();

		unsigned int i;

		for (i = 0; i < vec_item.size(); ++i)
		{
			LPITEM item = vec_item[i].first;
			int window = vec_item[i].second;

			item->AddToGround(GetMapIndex(), pos);
			item->StartDestroyEvent();

			sys_log(0, "DROP_ITEM_PK: %s %d %d from %s", item->GetName(), pos.x, pos.y, GetName());
			LogManager::instance().ItemLog(this, item, "DEAD_DROP", (window == INVENTORY) ? "INVENTORY" : ((window == EQUIPMENT) ? "EQUIPMENT" : ""));

			pos.x = GetX() + number(-7, 7) * 20;
			pos.y = GetY() + number(-7, 7) * 20;
		}
	}
}

class FPartyAlignmentCompute
{
	public:
		FPartyAlignmentCompute(int iAmount, int x, int y)
		{
			m_iAmount = iAmount;
			m_iCount = 0;
			m_iStep = 0;
			m_iKillerX = x;
			m_iKillerY = y;
		}

		void operator () (LPCHARACTER pkChr)
		{
			if (DISTANCE_APPROX(pkChr->GetX() - m_iKillerX, pkChr->GetY() - m_iKillerY) < PARTY_DEFAULT_RANGE)
			{
				if (m_iStep == 0)
				{
					++m_iCount;
				}
				else
				{
					pkChr->UpdateAlignment(m_iAmount / m_iCount);
				}
			}
		}

		int m_iAmount;
		int m_iCount;
		int m_iStep;

		int m_iKillerX;
		int m_iKillerY;
};

void CHARACTER::Dead(LPCHARACTER pkKiller, bool bImmediateDead)
{
	if (IsDead())
		return;

#ifdef DISABLE_STOP_RIDING_WHEN_DIE
	{
		if (IsHorseRiding())
		{
			StopRiding();
		}
		else if (GetMountVnum())
		{
#ifdef ENABLE_MOUNT_SYSTEM
			RemoveAffect(AFFECT_MOUNT);
			RemoveAffect(AFFECT_MOUNT_BONUS);
			LPITEM item = GetWear(WEAR_UNIQUE1);
			LPITEM item2 = GetWear(WEAR_UNIQUE2);
			LPITEM item3 = GetWear(WEAR_MOUNT);

			if (item && item->IsRideItem())
				UnequipItem(item);

			if (item2 && item2->IsRideItem())
				UnequipItem(item2);

			if (item3 && item3->IsRideItem())
				UnequipItem(item3);
#endif
			m_dwMountVnum = 0;
			UnEquipSpecialRideUniqueItem();

			UpdatePacket();
		}
	}
#endif

	if (!pkKiller && m_dwKillerPID)
		pkKiller = CHARACTER_MANAGER::instance().FindByPID(m_dwKillerPID);

	m_dwKillerPID = 0; // Must reset --> DO NOT DELETE THIS LINE UNLESS YOU ARE 1000000% SURE

#ifdef ENABLE_ULTIMATE_REGEN
	CNewMobTimer::Instance().Dead(this, pkKiller);
#endif

	if (IsPC())
		ResetSkillCoolTimes();
	//@fixme297
	if (IsPC())
		RemoveBadAffect();
	//@fixme297
	bool isAgreedPVP = false;
	bool isUnderGuildWar = false;
	bool isDuel = false;
	bool isForked = false;

	if (pkKiller && pkKiller->IsPC())
	{
		if (pkKiller->m_pkChrTarget == this)
			pkKiller->SetTarget(NULL);

#ifdef __AUTO_HUNT__
		// Ölen karakter bir metin/slot ise rezervasyonu kaldýr
		if (IsStone() || (IsNPC() && !IsPC()))
		{
			CHARACTER_MANAGER::instance().ReleaseTarget(GetVID());
		}

		// Ölen oyuncu ise tüm rezervasyonlarýný kaldýr
		if (IsPC())
		{
			CHARACTER_MANAGER::instance().ReleaseAllTargetsByPlayer(GetPlayerID());
		}
#endif

		if (!IsPC() && pkKiller->GetDungeon())
			pkKiller->GetDungeon()->IncKillCount(pkKiller, this);

		isAgreedPVP = CPVPManager::instance().Dead(this, pkKiller->GetPlayerID());
		isDuel = CArenaManager::instance().OnDead(pkKiller, this);

		if (IsPC())
		{
			CGuild * g1 = GetGuild();
			CGuild * g2 = pkKiller->GetGuild();

			if (g1 && g2)
				if (g1->UnderWar(g2->GetID()))
					isUnderGuildWar = true;

			pkKiller->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().Kill(pkKiller->GetPlayerID(), quest::QUEST_NO_NPC);
			CGuildManager::instance().Kill(pkKiller, this);
		}
	}

#ifdef ENABLE_RANKING
	if ((IsPC()))
	{
		if (((isAgreedPVP) || (isDuel)) && (pkKiller))
		{
			SetRankPoints(1, pkKiller->GetRankPoints(1) + 1);
			pkKiller->SetRankPoints(0, pkKiller->GetRankPoints(0) + 1);
		}
		else if (isUnderGuildWar)
		{
			pkKiller->SetRankPoints(2, pkKiller->GetRankPoints(2) + 1);
		}
	}

	if (pkKiller)
	{
		if (pkKiller->IsPC())
		{
			if (IsStone())
			{
				if (pkKiller)
					pkKiller->SetRankPoints(5, pkKiller->GetRankPoints(5) + 1);
			}
			else if (IsMonster())
			{
				if (GetMobRank() >= MOB_RANK_BOSS)
					pkKiller->SetRankPoints(7, pkKiller->GetRankPoints(7) + 1);
				else
					pkKiller->SetRankPoints(6, pkKiller->GetRankPoints(6) + 1);
			}
		}
	}
#endif

#ifdef ENABLE_STONE_EVENT_SYSTEM
	if (!IsPC() && pkKiller && pkKiller->IsPC())
	{
		if (IsStone())
		{
			if (pkKiller->GetQuestFlag("new_inventory_info.status") == 0)
			{
				pkKiller->SetQuestFlag("new_inventory_info.status", 1);
			}
			
			if (CStoneEvent::instance().IsStoneEvent() == true)
			{
				if (pkKiller->GetLevel() > GetMobTable().bLevel+20 || pkKiller->GetLevel() < GetMobTable().bLevel-20)
				{
					pkKiller->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Metin ta?? etkinli?i> Bu metinden puan alamazs?n kendi seviyene yak?n metinleri kesmen gerekiyor."));
				}
				else
				{
					CStoneEvent::instance().SetStoneKill(pkKiller->GetPlayerID());
					CStoneEvent::instance().StoneUpdateP2PPacket(pkKiller->GetPlayerID());
					CStoneEvent::instance().StoneInformation(pkKiller);
				}
			}
		}
	}
#endif

	if (pkKiller && !isAgreedPVP && !isUnderGuildWar && IsPC() && !isDuel)
	{
		if (GetGMLevel() == GM_PLAYER || test_server)
		{
			//ItemDropPenalty(pkKiller);
		}
	}

	SetPosition(POS_DEAD);
	ClearAffect(true);

#ifdef __AUTO_HUNT__
	// Ölen karakter bir metin/slot ise rezervasyonu kaldýr
	if ((IsStone() || (IsNPC() && !IsPC())) && GetVID())
	{
		CHARACTER_MANAGER::instance().ReleaseTarget(GetVID());
	}

	// Ölen oyuncu ise tüm rezervasyonlarýný kaldýr
	if (IsPC() && GetPlayerID())
	{
		CHARACTER_MANAGER::instance().ReleaseAllTargetsByPlayer(GetPlayerID());
	}
#endif

#ifdef ENABLE_MOB_FOLLOW_SYSTEM
	if (IsPC())
		ClearEnemyNPCs();
#endif

	if (pkKiller && IsPC())
	{
		if (!pkKiller->IsPC())
		{
			if (!isForked)
			{
				sys_log(1, "DEAD: %s %p WITH PENALTY", GetName(), this);
				SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);
				LogManager::instance().CharLog(this, pkKiller->GetRaceNum(), "DEAD_BY_NPC", pkKiller->GetName());
			}
		}
		else
		{
			sys_log(1, "DEAD_BY_PC: %s %p KILLER %s %p", GetName(), this, pkKiller->GetName(), get_pointer(pkKiller));
			REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);

			if (GetEmpire() != pkKiller->GetEmpire())
			{
				int iEP = MIN(GetPoint(POINT_EMPIRE_POINT), pkKiller->GetPoint(POINT_EMPIRE_POINT));

				PointChange(POINT_EMPIRE_POINT, -(iEP / 10));
				pkKiller->PointChange(POINT_EMPIRE_POINT, iEP / 5);

				if (GetPoint(POINT_EMPIRE_POINT) < 10)
				{
				}

				char buf[256];
				snprintf(buf, sizeof(buf),
						"%d %d %d %s %d %d %d %s",
						GetEmpire(), GetAlignment(), GetPKMode(), GetName(),
						pkKiller->GetEmpire(), pkKiller->GetAlignment(), pkKiller->GetPKMode(), pkKiller->GetName());

				LogManager::instance().CharLog(this, pkKiller->GetPlayerID(), "DEAD_BY_PC", buf);
			}
			else
			{
				if (!isAgreedPVP && !isUnderGuildWar && !IsKillerMode() && GetAlignment() >= 0 && !isDuel && !isForked)
				{
					int iNoPenaltyProb = 0;

					if (pkKiller->GetAlignment() >= 0) // 1/3 percent down
						iNoPenaltyProb = 33;
					else // 4/5 percent down
						iNoPenaltyProb = 20;

					if (number(1, 100) < iNoPenaltyProb)
						pkKiller->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You did not drop any Item(s) as you are protected by the Dragon God."));
					else
					{
						if (pkKiller->GetParty())
						{
							FPartyAlignmentCompute f(-20000, pkKiller->GetX(), pkKiller->GetY());
							pkKiller->GetParty()->ForEachOnMapMember(f, pkKiller->GetMapIndex());	//@fixme522

							if (f.m_iCount == 0)
								pkKiller->UpdateAlignment(-20000);
							else
							{
								sys_log(0, "ALIGNMENT PARTY count %d amount %d", f.m_iCount, f.m_iAmount);

								f.m_iStep = 1;
								pkKiller->GetParty()->ForEachOnMapMember(f, pkKiller->GetMapIndex());	//@fixme522
							}
						}
						else
							pkKiller->UpdateAlignment(-20000);
					}
				}

				char buf[256];
				snprintf(buf, sizeof(buf),
						"%d %d %d %s %d %d %d %s",
						GetEmpire(), GetAlignment(), GetPKMode(), GetName(),
						pkKiller->GetEmpire(), pkKiller->GetAlignment(), pkKiller->GetPKMode(), pkKiller->GetName());

				LogManager::instance().CharLog(this, pkKiller->GetPlayerID(), "DEAD_BY_PC", buf);
			}
		}
	}
	else
	{
		sys_log(1, "DEAD: %s %p", GetName(), this);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_DEATH_PENALTY);
	}

	ClearSync();

	//sys_log(1, "stun cancel %s[%d]", GetName(), (uint32_t)GetVID());
	event_cancel(&m_pkStunEvent); // The stun event kills.

	if (IsPC())
	{
		m_dwLastDeadTime = get_dword_time();
		SetKillerMode(false);
		GetDesc()->SetPhase(PHASE_DEAD);
	}
#ifdef ENABLE_BOT_PLAYER
	else if (IsBotCharacter())
	{
		m_dwLastDeadTime = get_dword_time();
		SetKillerMode(false);
	}
#endif
	else
	{
		//@fixme504
		if (m_pkRegen) {
			if (m_pkDungeon) {
				// Dungeon regen may not be valid at this point
				if (m_pkDungeon->IsValidRegen(m_pkRegen, regen_id_)) {
					--m_pkRegen->count;
				}
			}
			else {
				// Is this really safe?
				--m_pkRegen->count;
			}
#ifdef ENABLE_RESP_SYSTEM
			CRespManager::instance().KillMob(this);
#endif
			m_pkRegen = nullptr;
		}

		if (m_pkDungeon) {
			SetDungeon(nullptr);
		}
		//@end_fixme504
		if (!IS_SET(m_pointsInstant.instant_flag, INSTANT_FLAG_NO_REWARD))
		{
			if (!(pkKiller && pkKiller->IsPC() && pkKiller->GetGuild() && pkKiller->GetGuild()->UnderAnyWar(GUILD_WAR_TYPE_FIELD)))
			{
				// Resurrected monsters do not give rewards.
				if (GetMobTable().dwResurrectionVnum)
				{
					// DUNGEON_MONSTER_REBIRTH_BUG_FIX
					LPCHARACTER chResurrect = CHARACTER_MANAGER::instance().SpawnMob(GetMobTable().dwResurrectionVnum, GetMapIndex(), GetX(), GetY(), GetZ(), true, (int) GetRotation());
					if (GetDungeon() && chResurrect)
					{
						chResurrect->SetDungeon(GetDungeon());
					}
					// END_OF_DUNGEON_MONSTER_REBIRTH_BUG_FIX

					Reward(false);
				}
				else if (IsRevive() == true)
				{
					Reward(false);
				}
				else
				{
					Reward(true); // Drops gold, item, etc..
				}
			}
			else
			{
				if (pkKiller->m_dwUnderGuildWarInfoMessageTime < get_dword_time())
				{
					pkKiller->m_dwUnderGuildWarInfoMessageTime = get_dword_time() + 60000;
					pkKiller->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] There are no experience points for hunting during a guild war."));
				}
			}
		}
	}

	// BOSS_KILL_LOG
	if (GetMobRank() >= MOB_RANK_BOSS && pkKiller && pkKiller->IsPC())
	{
		char buf[51];
		snprintf(buf, sizeof(buf), "%d %ld", g_bChannel, pkKiller->GetMapIndex());
		if (IsStone())
			LogManager::instance().CharLog(pkKiller, GetRaceNum(), "STONE_KILL", buf);
		else
			LogManager::instance().CharLog(pkKiller, GetRaceNum(), "BOSS_KILL", buf);
	}
	// END_OF_BOSS_KILL_LOG

#ifndef ENABLE_RENEWAL_DEAD_PACKET
	TPacketGCDead pack;
	pack.header	= HEADER_GC_DEAD;
	pack.vid	= m_vid;
	PacketAround(&pack, sizeof(pack));
#endif

	REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

	if (GetDesc() != NULL) {

		itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

		while (it != m_list_pkAffect.end())
			SendAffectAddPacket(GetDesc(), *it++);
	}

#ifdef ENABLE_KILL_STATISTICS
	if (pkKiller && pkKiller->IsPC())
	{
		if (IsPC())
		{
			if (isDuel || isAgreedPVP)
			{
				pkKiller->AddToDuelsWon();
				AddToDuelsLost();
			}
			else
			{
				pkKiller->AddToTotalKills();

				switch (GetEmpire())
				{
				case 1:
					pkKiller->AddToShinsooKills();
					break;
				case 2:
					pkKiller->AddToChunjoKills();
					break;
				case 3:
					pkKiller->AddToJinnoKills();
					break;
				}
			}
		}
		else
		{
			if (GetMobRank() >= MOB_RANK_BOSS && IsMonster())
				pkKiller->AddToBossesKills();
			if (IsStone())
				pkKiller->AddToStonesKills();
			if (GetMobRank() > MOB_RANK_PAWN && GetMobRank() <= MOB_RANK_S_KNIGHT && IsMonster())
				pkKiller->AddToMobsKills();
		}

		if (IsPC())
			SendKillStatisticsPacket();
		pkKiller->SendKillStatisticsPacket();
	}
#endif

	if (isDuel == false)
	{
		if (m_pkDeadEvent)
		{
			sys_log(1, "DEAD_EVENT_CANCEL: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
			event_cancel(&m_pkDeadEvent);
		}

#ifdef ENABLE_BOT_PLAYER
		if (m_pkBotCharacterDeadEvent)
		{
			sys_log(1, "BOT_DEAD_EVENT_CANCEL: %s %p %p", GetName(), this, get_pointer(m_pkBotCharacterDeadEvent));
			event_cancel(&m_pkBotCharacterDeadEvent);
		}
#endif

		if (IsStone())
		{
#ifdef ENABLE_FIX_EXP_DROP_STONES
			ClearStone(pkKiller);
#else
			ClearStone();
#endif
		}

		if (GetDungeon())
		{
			GetDungeon()->DeadCharacter(this);
		}

		SCharDeadEventInfo* pEventInfo = AllocEventInfo<SCharDeadEventInfo>();
#ifdef ENABLE_BOT_PLAYER
		SBotCharacterDeadEventInfo* pBotDeadEventInfo = AllocEventInfo<SBotCharacterDeadEventInfo>();
#endif

		if (IsPC())
		{
			pEventInfo->isPC = true;
			pEventInfo->dwID = this->GetPlayerID();

			m_pkDeadEvent = event_create(dead_event, pEventInfo, PASSES_PER_SEC(180));
		}
#ifdef ENABLE_BOT_PLAYER
		else if (IsBotCharacter())
		{
			pBotDeadEventInfo->ch = this;
			m_pkBotCharacterDeadEvent = event_create(bot_character_dead_event, pBotDeadEventInfo, PASSES_PER_SEC(number(7, 15)));
		}
#endif
		else
		{
			pEventInfo->isPC = false;
			pEventInfo->dwID = this->GetVID();
#ifdef ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX
			m_pkDeadEvent = event_create(dead_event, pEventInfo, bImmediateDead ? 1 : PASSES_PER_SEC(1));
#else
			if (IsRevive() == false && HasReviverInParty() == true)
			{
				m_pkDeadEvent = event_create(dead_event, pEventInfo, bImmediateDead ? 1 : PASSES_PER_SEC(3));
			}
#ifdef ENABLE_QUEEN_NETHIS
			else if (GetRaceNum() == SnakeLair::eSnakeConfig::PILAR_STEP_4)
			{
				m_pkDeadEvent = event_create(dead_event, pEventInfo, PASSES_PER_SEC(1));
			}
#endif
			else
			{
				m_pkDeadEvent = event_create(dead_event, pEventInfo, bImmediateDead ? 1 : PASSES_PER_SEC(1));
			}
#endif
		}

		sys_log(1, "DEAD_EVENT_CREATE: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
	}

	if (m_pkExchange != NULL)
	{
		m_pkExchange->Cancel();
	}

#ifdef ENABLE_RENEWAL_DEAD_PACKET
	TPacketGCDead pack;
	pack.header = HEADER_GC_DEAD;
	pack.vid = m_vid;

	for (BYTE i = REVIVE_TYPE_HERE; i < REVIVE_TYPE_MAX; i++)
		pack.t_d[i] = CalculateDeadTime(i);

	PacketAround(&pack, sizeof(pack));
#endif

	if (IsCubeOpen() == true)
	{
		Cube_close(this);
	}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (IsPC())
		CloseAcce();
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	if (IsAuraRefineWindowOpen())
		AuraRefineWindowClose();
#endif

	if (GetExchange())
		GetExchange()->Cancel();

	CShopManager::instance().StopShopping(this);
	CloseMyShop();
	CloseSafebox();
	CloseMall();

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	if(GetOfflineShop())
		COfflineShopManager::Instance().StopShopping(this);
#endif

#ifdef ENABLE_OCHAO_TEMPLE_SYSTEM
	if (IsMonster())
	{
		if ((GetMobTable().dwVnum == TEMPLE_OCHAO_GUARDIAN) && (pkKiller && pkKiller->GetMapIndex() == TEMPLE_OCHAO_MAP_INDEX))
			TempleOchao::CMgr::instance().OnGuardianKilled(pkKiller->GetX(), pkKiller->GetY(), pkKiller->GetZ());
	}
#endif

#ifdef ENABLE_WHITE_DRAGON
	if (IsMonster())
	{
		if (pkKiller && pkKiller->IsPC())
		{
			if (WhiteDragon::CWhDr::instance().IsWhiteMap(pkKiller->GetMapIndex()))
				WhiteDragon::CWhDr::instance().OnKill(this, pkKiller);
		}
	}
#endif

#ifdef ENABLE_QUEEN_NETHIS
	if ((IsStone()) || (IsMonster()))
	{
		if (pkKiller && pkKiller->IsPC())
		{
			if (SnakeLair::CSnk::instance().IsSnakeMap(pkKiller->GetMapIndex()))
				SnakeLair::CSnk::instance().OnKill(this, pkKiller);
		}
	}
#endif

#ifdef ENABLE_DUNGEON_INFO
	if ((pkKiller) && (IsMonster()) && (pkKiller->IsPC())){
		if (pkKiller->GetMapIndex() >= 10000){
			if (CDungeonInfoExtern::instance().CheckBossKillMap(pkKiller->GetMapIndex()) == true)
			{
				if(true == IsMonster())
				{
					if(CDungeonInfoExtern::instance().GetIdBoss(pkKiller->GetMapIndex()) == GetMobTable().dwVnum)
					{
						CDungeonInfoExtern::instance().KillBossDungeon(pkKiller);
					}
				}
			}
		}
	}

	if ((pkKiller) && (IsMonster()) && (pkKiller->IsPC()))
	{
		if(CDungeonInfoExtern::instance().GetVnumMobMision(GetMobTable().dwVnum)){
			if(CDungeonInfoExtern::instance().GetStatusMision(pkKiller,GetMobTable().dwVnum) != 0){
				if (CDungeonInfoExtern::instance().GetCountMision(pkKiller,GetMobTable().dwVnum) < CDungeonInfoExtern::instance().GetCountMobMision(GetMobTable().dwVnum)){
					CDungeonInfoExtern::instance().SetCountMision(pkKiller,GetMobTable().dwVnum);
				}
			}
		}
	}
#endif

	if (true == IsMonster() && 2493 == GetMobTable().dwVnum)
	{
		if (NULL != pkKiller && NULL != pkKiller->GetGuild())
		{
			CDragonLairManager::instance().OnDragonDead( this, pkKiller->GetGuild()->GetID() );
		}
		else
		{
			sys_err("DragonLair: Dragon killed by nobody");
		}
	}
#ifdef ENABLE_BATTLE_PASS
	if (pkKiller && pkKiller->IsPC())
	{
		if (!pkKiller->v_counts.empty())
		{
			for (int i = 0; i < pkKiller->missions_bp.size(); ++i)
			{
				if (IsMonster() && GetMobRank() >= MOB_RANK_BOSS && pkKiller->missions_bp[i].type == 4)
				{
					pkKiller->DoMission(i, 1);
				}
				if (IsStone() && pkKiller->missions_bp[i].type == 5)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30140 && pkKiller->missions_bp[i].type == 9)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30141 && pkKiller->missions_bp[i].type == 10)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30142 && pkKiller->missions_bp[i].type == 11)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30143 && pkKiller->missions_bp[i].type == 12)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30144 && pkKiller->missions_bp[i].type == 13)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30145 && pkKiller->missions_bp[i].type == 14)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30146 && pkKiller->missions_bp[i].type == 15)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30147 && pkKiller->missions_bp[i].type == 16)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30148 && pkKiller->missions_bp[i].type == 17)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30149 && pkKiller->missions_bp[i].type == 18)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30197 && pkKiller->missions_bp[i].type == 19)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30202 && pkKiller->missions_bp[i].type == 20)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 30207 && pkKiller->missions_bp[i].type == 21)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 4023 && pkKiller->missions_bp[i].type == 22)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 4024 && pkKiller->missions_bp[i].type == 23)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 691 && pkKiller->missions_bp[i].type == 24)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 1304 && pkKiller->missions_bp[i].type == 25)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 1901 && pkKiller->missions_bp[i].type == 26)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 2191 && pkKiller->missions_bp[i].type == 27)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 2091 && pkKiller->missions_bp[i].type == 28)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 2206 && pkKiller->missions_bp[i].type == 29)
				{
					pkKiller->DoMission(i, 1);
				}
				if (GetRaceNum() == 6409 && pkKiller->missions_bp[i].type == 30)
				{
					pkKiller->DoMission(i, 1);
				}
			}
		}
	}

	if (pkKiller && pkKiller->IsPC())
	{
		if (!pkKiller->v_counts_premium.empty())
		{
			for (int i = 0; i < pkKiller->missions_bp_premium.size(); ++i)
			{
				if (IsMonster() && GetMobRank() >= MOB_RANK_BOSS && pkKiller->missions_bp_premium[i].type == 4)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (IsStone() && pkKiller->missions_bp_premium[i].type == 5)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30140 && pkKiller->missions_bp_premium[i].type == 9)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30141 && pkKiller->missions_bp_premium[i].type == 10)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30142 && pkKiller->missions_bp_premium[i].type == 11)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30143 && pkKiller->missions_bp_premium[i].type == 12)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30144 && pkKiller->missions_bp_premium[i].type == 13)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30145 && pkKiller->missions_bp_premium[i].type == 14)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30146 && pkKiller->missions_bp_premium[i].type == 15)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30147 && pkKiller->missions_bp_premium[i].type == 16)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30148 && pkKiller->missions_bp_premium[i].type == 17)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30149 && pkKiller->missions_bp_premium[i].type == 18)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30197 && pkKiller->missions_bp_premium[i].type == 19)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30202 && pkKiller->missions_bp_premium[i].type == 20)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 30207 && pkKiller->missions_bp_premium[i].type == 21)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 4023 && pkKiller->missions_bp_premium[i].type == 22)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 4024 && pkKiller->missions_bp_premium[i].type == 23)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 691 && pkKiller->missions_bp_premium[i].type == 24)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 1304 && pkKiller->missions_bp_premium[i].type == 25)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 1901 && pkKiller->missions_bp_premium[i].type == 26)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 2191 && pkKiller->missions_bp_premium[i].type == 27)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 2091 && pkKiller->missions_bp_premium[i].type == 28)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 2206 && pkKiller->missions_bp_premium[i].type == 29)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
				if (GetRaceNum() == 6409 && pkKiller->missions_bp_premium[i].type == 30)
				{
					pkKiller->DoMissionPremium(i, 1);
				}
			}
		}
	}
#endif
}

// DevFix 29
void CHARACTER::RewardlessDead()
{
	if (IsPC())  // This function only for mobs or stones, so if IsPC() has triggered this, redirect it regular Dead() function. - [MT2Dev Note] - 18/10/2024
	{
		sys_log(0, "<CHARACTER::RewardlessDead> Wrong Usage! It was a PC, redirect it to regular Dead()");
		Dead();  // Just to prevent, not needed if you use it properly. - [Mitachi] - 18/10/2024
		return;
	}

	if (IsDead())
	{
		return;
	}

	if (IsMonster() || IsStone())  // Dead is only possible when victim is mob or stone. - [MT2Dev Note] - 10/10/2024
	{
		m_dwKillerPID = 0;  // Remove the killer to avoid quest triggers may in dead. - [Mitachi] - 18/10/2024
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_NO_REWARD);  // Set no reward. - [Mitachi] - 18/10/2024
		SetPosition(POS_DEAD);
		ClearAffect(true);
		ClearSync();

		if (m_pkStunEvent)
		{
			event_cancel(&m_pkStunEvent);
		}

		TPacketGCDead pack;
		pack.header = HEADER_GC_DEAD;
		pack.vid = m_vid;
		PacketAround(&pack, sizeof(pack));
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);
		sys_log(0, "Rewardless_DEAD: %s %p", GetName(), this);

		// Create Dead event.. In the Dead event, for monsters, make them destroy after a few seconds. - [Ymir Dev Note]
		if (m_pkDeadEvent)
		{
			event_cancel(&m_pkDeadEvent);
		}

		if (IsStone())
		{
			ClearStone();
		}

		if (GetDungeon())
		{
			GetDungeon()->DeadCharacter(this);
		}

		SCharDeadEventInfo* pEventInfo = AllocEventInfo<SCharDeadEventInfo>();
		pEventInfo->isPC = false;
		pEventInfo->dwID = this->GetVID();
		m_pkDeadEvent = event_create(dead_event, pEventInfo, PASSES_PER_SEC(0));
		sys_log(0, "Rewardless_DEAD_EVENT_CREATE: %s %p %p", GetName(), this, get_pointer(m_pkDeadEvent));
	}
}

struct FuncSetLastAttacked
{
	FuncSetLastAttacked(DWORD dwTime) : m_dwTime(dwTime)
	{
	}

	void operator () (LPCHARACTER ch)
	{
		if (ch)
			ch->SetLastAttacked(m_dwTime);
	}

	DWORD m_dwTime;
};

void CHARACTER::SetLastAttacked(DWORD dwTime)
{
	assert(m_pkMobInst != NULL);

	m_pkMobInst->m_dwLastAttackedTime = dwTime;
	m_pkMobInst->m_posLastAttacked = GetXYZ();
}

void CHARACTER::SendDamagePacket(LPCHARACTER pAttacker, int Damage, BYTE DamageFlag)
{
#ifdef ENABLE_BOT_PLAYER
	if (pAttacker && pAttacker->IsBotCharacter())
	{
		TPacketGCDamageInfo damageInfo;
		memset(&damageInfo, 0, sizeof(TPacketGCDamageInfo));
		damageInfo.header = HEADER_GC_DAMAGE_INFO;
		damageInfo.dwVictimVID = (DWORD)GetVID();
		damageInfo.dwAttackerVID = (DWORD)pAttacker->GetVID();
		damageInfo.flag = DamageFlag;
		damageInfo.damage = Damage;
		if (GetDesc() != NULL)
			GetDesc()->Packet(&damageInfo, sizeof(TPacketGCDamageInfo));
	}
#endif

#ifdef MULTIPLE_DAMAGE_DISPLAY_SYSTEM
	// Çoklu damage gösterimi: Sadece PC sald?rd???nda (kendi damage'lerini görmek için)
	// GetTarget() kontrolü kald?r?ld? - böylece PC birden fazla moba sald?rd???nda her mob için damage gösterilir
	if (pAttacker && pAttacker->IsPC())
	{
		TPacketGCDamageInfo damageInfo;
		memset(&damageInfo, 0, sizeof(TPacketGCDamageInfo));

		damageInfo.header = HEADER_GC_DAMAGE_INFO;
#ifdef ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX
		damageInfo.dwVictimVID = (DWORD)GetVID();
		damageInfo.dwAttackerVID = (DWORD)pAttacker->GetVID();
#else
		damageInfo.dwVID = (DWORD)GetVID();
#endif
		damageInfo.flag = DamageFlag;
		damageInfo.damage = Damage;

		// Kurban?n desc'i varsa (PC ise) ona gönder
		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&damageInfo, sizeof(TPacketGCDamageInfo));
		}

		// Sald?ran?n desc'ine gönder (PC kendi damage'lerini görsün)
		if (pAttacker->GetDesc() != NULL)
		{
			pAttacker->GetDesc()->Packet(&damageInfo, sizeof(TPacketGCDamageInfo));
		}
	}
	// Kurban PC ise (ba?kas?ndan damage ald???nda)
	else if (IsPC())
	{
		TPacketGCDamageInfo damageInfo;
		memset(&damageInfo, 0, sizeof(TPacketGCDamageInfo));

		damageInfo.header = HEADER_GC_DAMAGE_INFO;
#ifdef ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX
		damageInfo.dwVictimVID = (DWORD)GetVID();
		damageInfo.dwAttackerVID = pAttacker ? (DWORD)pAttacker->GetVID() : 0;
#else
		damageInfo.dwVID = (DWORD)GetVID();
#endif
		damageInfo.flag = DamageFlag;
		damageInfo.damage = Damage;

		// Kurban PC'ye gönder (kendi ald??? damage'i görsün)
		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&damageInfo, sizeof(TPacketGCDamageInfo));
		}
	}
#else
	if (IsPC() == true || (pAttacker->IsPC() == true && pAttacker->GetTarget() == this))
	{
		TPacketGCDamageInfo damageInfo;
		memset(&damageInfo, 0, sizeof(TPacketGCDamageInfo));

		damageInfo.header = HEADER_GC_DAMAGE_INFO;
#ifdef ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX
		damageInfo.dwVictimVID = (DWORD)GetVID();
		damageInfo.dwAttackerVID = (DWORD)pAttacker->GetVID();
#else
		damageInfo.dwVID = (DWORD)GetVID();
#endif
		damageInfo.flag = DamageFlag;
		damageInfo.damage = Damage;

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&damageInfo, sizeof(TPacketGCDamageInfo));
		}

		if (pAttacker->GetDesc() != NULL)
		{
			pAttacker->GetDesc()->Packet(&damageInfo, sizeof(TPacketGCDamageInfo));
		}
	}
#endif
}

/*
* The CHARACTER::Damage method causes this to take damage.
*
* Arguments
* pAttacker : the attacker
* dam : damage
* EDamageType: What type of attack is it?
*
* Return value
* true : dead
* false : not dead yet
*/
bool CHARACTER::Damage(LPCHARACTER pAttacker, int dam, EDamageType type) // returns true if dead
{
#ifdef ENABLE_CONQUEROR_LEVEL	
	if(IsAffectFlag(AFF_CHUNWOON_MOOJUK))
	{
		SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
		return false;
	}

	if(pAttacker && pAttacker->IsPC())
	{
		if ((pAttacker->IsConquerorMap(pAttacker->GetMapIndex())))
		{
			if(!IsPC())
			{
				int value = pAttacker->GetPoint(POINT_SUNGMA_STR); 
				int aValue = SECTREE_MANAGER::instance().GetSungmaValueAffectByRegion(pAttacker->GetMapIndex(), AFFECT_SUNGMA_STR);
				if(value < aValue)
				{
					dam /= 2;
				}

				if (pAttacker->GetConquerorLevel() == 0)
					dam = 0;
			}
		}
	}
#endif

	if (!pAttacker)
		return false;

#ifdef ENABLE_BOT_PLAYER
    // Bot oyuncular için MaxHP'nin asla 0 olmamasýný garanti et (divide by zero korumasý)
    if (IsBotCharacter() && GetMaxHP() <= 0)
    {
        SetMaxHP(30000);
        SetHP(30000);
    }
#endif

	if (IsPC() && IsObserverMode() && IsDead()) // @fixme204
		return false;

	if (!GetSectree() || GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		return false;

#ifdef ENABLE_SUNG_MAHI_TOWER
	if (pAttacker && pAttacker->IsPC())
	{
		LPDUNGEON dungeonInstance = pAttacker->GetDungeon();
		if (dungeonInstance && dungeonInstance->GetFlag("chessWrongMonster") < 1)
		{
			if (this->GetUniqueMaster())
			{
				pAttacker->AggregateMonsterByMaster();
				this->SetUniqueMaster(false);
				dungeonInstance->SetFlag("chessWrongMonster", 1);
			}
		}
	}
#endif

	//@fixme423
	if (pAttacker && this) {
		if (pAttacker->IsAffectFlag(AFF_GWIGUM) && !pAttacker->GetWear(WEAR_WEAPON)) {
			pAttacker->RemoveAffect(SKILL_GWIGEOM);		// Büyülü Silah
			return false;
		}

		if (pAttacker->IsAffectFlag(AFF_GEOMGYEONG) && !pAttacker->GetWear(WEAR_WEAPON)) {
			pAttacker->RemoveAffect(SKILL_GEOMKYUNG);	// Hava Kýlýcý
			return false;
		}
	}
	//@END_fixme423

	/*
#ifdef ENABLE_DRAGON_LAIR
	if (GetRaceNum() >= 8031 && GetRaceNum() <= 8034)
	{
		SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
		return false;
	}
#endif
	*/
#ifdef ENABLE_MELEY_LAIR_DUNGEON
	if (GetProtectTime("IsMeleyBlock") == 1)
	{
		SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
		return false;
	}
#endif

#ifdef ENABLE_NINETH_SKILL
	if (IsAffectFlag(AFF_CHUNWOON_MOOJUK))
	{
		SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
		return false;
	}
#endif

	if (pAttacker && pAttacker->GetMapIndex() >= 660000 && pAttacker->GetMapIndex() < 670000)  
	{
		if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		{
			return false;
		}
	}

	if (DAMAGE_TYPE_MAGIC == type)
	{
		dam = (int)((float)dam * (100 + (pAttacker->GetPoint(POINT_MAGIC_ATT_BONUS_PER) + pAttacker->GetPoint(POINT_MELEE_MAGIC_ATT_BONUS_PER))) / 100.f + 0.5f);
	}

	if (GetRaceNum() == 5001)
	{
		bool bDropMoney = false;

		int iPercent = 0; // @fixme136
		if (GetMaxHP() >= 0)
			iPercent = (GetHP() * 100) / GetMaxHP();

		if (iPercent <= 10 && GetMaxSP() < 5)
		{
			SetMaxSP(5);
			bDropMoney = true;
		}
		else if (iPercent <= 20 && GetMaxSP() < 4)
		{
			SetMaxSP(4);
			bDropMoney = true;
		}
		else if (iPercent <= 40 && GetMaxSP() < 3)
		{
			SetMaxSP(3);
			bDropMoney = true;
		}
		else if (iPercent <= 60 && GetMaxSP() < 2)
		{
			SetMaxSP(2);
			bDropMoney = true;
		}
		else if (iPercent <= 80 && GetMaxSP() < 1)
		{
			SetMaxSP(1);
			bDropMoney = true;
		}

		if (bDropMoney)
		{
			DWORD dwGold = 1000;
			int iSplitCount = number(10, 13);

			sys_log(0, "WAEGU DropGoldOnHit %d times", GetMaxSP());

			for (int i = 1; i <= iSplitCount; ++i)
			{
				PIXEL_POSITION pos;
				LPITEM item;

				if ((item = ITEM_MANAGER::instance().CreateItem(1, dwGold / iSplitCount)))
				{
					if (i != 0)
					{
						pos.x = (number(-14, 14) + number(-14, 14)) * 20;
						pos.y = (number(-14, 14) + number(-14, 14)) * 20;

						pos.x += GetX();
						pos.y += GetY();
					}

					item->AddToGround(GetMapIndex(), pos);
					item->StartDestroyEvent();
				}
			}
		}
	}

	// Handling fear when not hitting
	if (type != DAMAGE_TYPE_NORMAL && type != DAMAGE_TYPE_NORMAL_RANGE)
	{
		if (IsAffectFlag(AFF_TERROR))
		{
			int pct = GetSkillPower(SKILL_TERROR) / 400;

			if (number(1, 100) <= pct)
				return false;
		}
	}

/*	int iCurHP = GetHP();
	int iCurSP = GetSP();*/

	bool IsCritical = false;
	bool IsPenetrate = false;
	bool IsDeathBlow = false;

	enum DamageFlag
	{
		DAMAGE_NORMAL	= (1 << 0),
		DAMAGE_POISON	= (1 << 1),
		DAMAGE_DODGE	= (1 << 2),
		DAMAGE_BLOCK	= (1 << 3),
		DAMAGE_PENETRATE= (1 << 4),
		DAMAGE_CRITICAL = (1 << 5),
	};

	/*
	* Magic-type skills and range-type skills (the archerist) calculate critical and penetration attacks.
	* I shouldn't do it, but I can't do a Nerf (downbalance) patch, so
	* Do not use the original value of the penetration attack, and apply it more than /2.
	*
	* There are many samurai stories, so melee skill is also added
	*
	* 20091109: As a result, it was concluded that the samurai became extremely strong, and the proportion of samurai in Germany was close to 70%
	*/
	if (type == DAMAGE_TYPE_MELEE || type == DAMAGE_TYPE_RANGE || type == DAMAGE_TYPE_MAGIC)
	{
		if (pAttacker)
		{
			// critical
			int iCriticalPct = pAttacker->GetPoint(POINT_CRITICAL_PCT);

			if (!IsPC()
#ifdef ENABLE_BOT_PLAYER
				&& !IsBotCharacter()
#endif
				)
				iCriticalPct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_CRITICAL_BONUS);

			if (iCriticalPct)
			{
				if (iCriticalPct >= 10) // If greater than 10, 5% + (increases by 1% every 4), so if the number is 50, 20%
					iCriticalPct = 5 + (iCriticalPct - 10) / 4;
				else // If less than 10, simply cut in half, 10 = 5%
					iCriticalPct /= 2;

				//Apply critical resistance value.
				iCriticalPct -= GetPoint(POINT_RESIST_CRITICAL);

				if (number(1, 100) <= iCriticalPct)
				{
					IsCritical = true;
					dam *= 2;
					EffectPacket(SE_CRITICAL);

					if (IsAffectFlag(AFF_MANASHIELD))
					{
						RemoveAffect(AFF_MANASHIELD);
					}
				}
			}

			// Penetration attack
			int iPenetratePct = pAttacker->GetPoint(POINT_PENETRATE_PCT);

			if (!IsPC()
#ifdef ENABLE_BOT_PLAYER
				&& !IsBotCharacter()
#endif
				)
				iPenetratePct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_PENETRATE_BONUS);

			if (iPenetratePct)
			{
				{
					CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_RESIST_PENETRATE);

					if (NULL != pkSk)
					{
						pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_RESIST_PENETRATE) / 100.0f);

						iPenetratePct -= static_cast<int>(pkSk->kPointPoly.Eval());
					}
				}

				if (iPenetratePct >= 10)
				{
					// If greater than 10, 5% + (increases by 1% every 4), so if the number is 50, 20%
					iPenetratePct = 5 + (iPenetratePct - 10) / 4;
				}
				else
				{
					// Less than 10 simply cut in half, 10 = 5%
					iPenetratePct /= 2;
				}

				//Apply penetration resistance value.
				iPenetratePct -= GetPoint(POINT_RESIST_PENETRATE);

				if (number(1, 100) <= iPenetratePct)
				{
					IsPenetrate = true;

					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, "Additional Stabbing Weapon Damage %d", GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100);

					dam += GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100;
					EffectPacket(SE_PENETRATE);

					if (IsAffectFlag(AFF_MANASHIELD))
					{
						RemoveAffect(AFF_MANASHIELD);
					}
				}
			}
		}
	}
	//
	// Attribute values are calculated only for combo attacks, bow attacks, that is, flat hits.
	//
	else if (type == DAMAGE_TYPE_NORMAL || type == DAMAGE_TYPE_NORMAL_RANGE)
	{
		if (type == DAMAGE_TYPE_NORMAL)
		{
			// Can be blocked if it is close to flat
			if (GetPoint(POINT_BLOCK) && number(1, 100) <= GetPoint(POINT_BLOCK))
			{
				if (test_server)
				{
					pAttacker->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ºí·°! (%d%%)"), GetName(), GetPoint(POINT_BLOCK));
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s ºí·°! (%d%%)"), GetName(), GetPoint(POINT_BLOCK));
				}

				SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
				return false;
			}
		}
		else if (type == DAMAGE_TYPE_NORMAL_RANGE)
		{
			// Can be avoided in the case of long range hitting
			if (GetPoint(POINT_DODGE) && number(1, 100) <= GetPoint(POINT_DODGE))
			{
				if (test_server)
				{
					pAttacker->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s È¸ÇÇ! (%d%%)"), GetName(), GetPoint(POINT_DODGE));
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s È¸ÇÇ! (%d%%)"), GetName(), GetPoint(POINT_DODGE));
				}

				SendDamagePacket(pAttacker, 0, DAMAGE_DODGE);
				return false;
			}
		}

		if (IsAffectFlag(AFF_JEONGWIHON))
			dam = (int) (dam * (100 + GetSkillPower(SKILL_JEONGWI) * 25 / 100) / 100);

		if (IsAffectFlag(AFF_TERROR))
			dam = (int) (dam * (95 - GetSkillPower(SKILL_TERROR) / 5) / 100);

		if (IsAffectFlag(AFF_HOSIN))
			dam = dam * (100 - GetPoint(POINT_RESIST_NORMAL_DAMAGE)) / 100;

		//
		// Apply attacker attribute
		//
		if (pAttacker)
		{
			if (type == DAMAGE_TYPE_NORMAL)
			{
				if (GetPoint(POINT_REFLECT_MELEE))
				{
					int reflectDamage = dam * GetPoint(POINT_REFLECT_MELEE) / 100;

					/* NOTE: If the attacker has the IMMUNE_REFLECT attribute, it is better not to reflect
					* No, the plan requested that it be fixed at 1/3 damage.*/
					if (pAttacker->IsImmune(IMMUNE_REFLECT))
						reflectDamage = int(reflectDamage / 3.0f + 0.5f);

					pAttacker->Damage(this, reflectDamage, DAMAGE_TYPE_SPECIAL);
				}
			}

			// critical
			int iCriticalPct = pAttacker->GetPoint(POINT_CRITICAL_PCT);

			if (!IsPC()
#ifdef ENABLE_BOT_PLAYER
				&& !IsBotCharacter()
#endif
				)
				iCriticalPct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_CRITICAL_BONUS);

			if (iCriticalPct)
			{
				//Apply critical resistance value.
				iCriticalPct -= GetPoint(POINT_RESIST_CRITICAL);

				if (number(1, 100) <= iCriticalPct)
				{
					IsCritical = true;
					dam *= 2;
					EffectPacket(SE_CRITICAL);
				}
			}

			// Penetration attack
			int iPenetratePct = pAttacker->GetPoint(POINT_PENETRATE_PCT);

			if (!IsPC()
#ifdef ENABLE_BOT_PLAYER
				&& !IsBotCharacter()
#endif
				)
				iPenetratePct += pAttacker->GetMarriageBonus(UNIQUE_ITEM_MARRIAGE_PENETRATE_BONUS);

			{
				CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_RESIST_PENETRATE);

				if (NULL != pkSk)
				{
					pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_RESIST_PENETRATE) / 100.0f);

					iPenetratePct -= static_cast<int>(pkSk->kPointPoly.Eval());
				}
			}

			if (iPenetratePct)
			{
				//Apply penetration resistance value.
				iPenetratePct -= GetPoint(POINT_RESIST_PENETRATE);

				if (number(1, 100) <= iPenetratePct)
				{
					IsPenetrate = true;

					if (test_server)
						ChatPacket(CHAT_TYPE_INFO, "Additional Stabbing Weapon Damage %d", GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100);
					dam += GetPoint(POINT_DEF_GRADE) * (100 + GetPoint(POINT_DEF_BONUS)) / 100;
					EffectPacket(SE_PENETRATE);
				}
			}

#ifdef ENABLE_HP_SP_STEAL_FIX
			int iStealHP_ptr = pAttacker->GetPoint(POINT_STEAL_HP);
			if (iStealHP_ptr)
			{
				if (number(1, 100) <= iStealHP_ptr)
				{
					int iHP = MIN(dam, MAX(0, GetHP())) * pAttacker->GetPoint(POINT_STEAL_HP) / 100;
					if ((pAttacker->GetHP() > 0) && (pAttacker->GetHP() + iHP < pAttacker->GetMaxHP()) && (GetHP() > 0) && (iHP > 0))
					{
						CreateFly(FLY_HP_MEDIUM, pAttacker);
						pAttacker->PointChange(POINT_HP, iHP);
						PointChange(POINT_HP, -iHP);
					}
				}
			}

			int iStealSP_ptr = pAttacker->GetPoint(POINT_STEAL_SP);
			if (iStealSP_ptr)
			{
				if (IsPC() && pAttacker->IsPC())
				{
					if (number(1, 100) <= iStealSP_ptr)
					{
						int iSP = MIN(dam, MAX(0, GetSP())) * pAttacker->GetPoint(POINT_STEAL_SP) / 100;

						if ((pAttacker->GetSP() > 0) && (pAttacker->GetSP() + iSP < pAttacker->GetMaxSP()) && (GetSP() > 0) && (iSP > 0))
						{
							CreateFly(FLY_SP_MEDIUM, pAttacker);
							pAttacker->PointChange(POINT_SP, iSP);
							PointChange(POINT_SP, -iSP);
						}
					}
				}
			}
#else
			if (pAttacker->GetPoint(POINT_STEAL_HP))
			{
				int pct = 1;

				if (number(1, 10) <= pct)
				{
#ifdef ENABLE_HP_LIMIT_RENEWAL
					long long iHP = MIN(dam, MAX(0, iCurHP)) * pAttacker->GetPoint(POINT_STEAL_HP) / 100;
#else
					int iHP = MIN(dam, MAX(0, iCurHP)) * pAttacker->GetPoint(POINT_STEAL_HP) / 100;
#endif

					if (iHP > 0 && GetHP() >= iHP)
					{
						CreateFly(FLY_HP_SMALL, pAttacker);
						pAttacker->PointChange(POINT_HP, iHP);
						PointChange(POINT_HP, -iHP);
					}
				}
			}

			// SP Steal
			if (pAttacker->GetPoint(POINT_STEAL_SP))
			{
				int pct = 1;

				if (number(1, 10) <= pct)
				{
					int iCur;

					if (IsPC())
						iCur = iCurSP;
					else
						iCur = iCurHP;

					int iSP = MIN(dam, MAX(0, iCur)) * pAttacker->GetPoint(POINT_STEAL_SP) / 100;

					if (iSP > 0 && iCur >= iSP)
					{
						CreateFly(FLY_SP_SMALL, pAttacker);
						pAttacker->PointChange(POINT_SP, iSP);

						if (IsPC())
							PointChange(POINT_SP, -iSP);
					}
				}
			}
#endif
			// Money steal
			if (pAttacker->GetPoint(POINT_STEAL_GOLD))
			{
				if (number(1, 100) <= pAttacker->GetPoint(POINT_STEAL_GOLD))
				{
					int iAmount = number(1, GetLevel());
					pAttacker->PointChange(POINT_GOLD, iAmount);
					DBManager::instance().SendMoneyLog(MONEY_LOG_MISC, 1, iAmount);
				}
			}

#ifdef ENABLE_HP_SP_ABSORB_ORB_FIX
			int iAbsoHP_ptr = pAttacker->GetPoint(POINT_HIT_HP_RECOVERY);
			if (iAbsoHP_ptr > 0)
			{
				if (number(1, 100) <= iAbsoHP_ptr)
				{
					int iHPAbso = MIN(dam, GetHP()) * pAttacker->GetPoint(POINT_HIT_HP_RECOVERY) / 100;
					if ((pAttacker->GetHP() > 0) && (pAttacker->GetHP() + iHPAbso < pAttacker->GetMaxHP()) && (GetHP() > 0) && (iHPAbso > 0))
					{
						CreateFly(FLY_HP_SMALL, pAttacker);
						pAttacker->PointChange(POINT_HP, iHPAbso);
					}
				}
			}

			int iAbsoSP_ptr = pAttacker->GetPoint(POINT_HIT_SP_RECOVERY);
			if (iAbsoSP_ptr > 0)
			{
				if (number(1, 100) <= iAbsoSP_ptr)
				{
					int iSPAbso = MIN(dam, GetSP()) * pAttacker->GetPoint(POINT_HIT_SP_RECOVERY) / 100;

					if ((pAttacker->GetSP() > 0) && (pAttacker->GetSP() + iSPAbso < pAttacker->GetMaxSP()) && (GetSP() > 0) && (iSPAbso > 0))
					{
						CreateFly(FLY_SP_SMALL, pAttacker);
						pAttacker->PointChange(POINT_SP, iSPAbso);
					}
				}
			}
#else
			if (pAttacker->GetPoint(POINT_HIT_HP_RECOVERY) && number(0, 4) > 0)
			{
				int i = ((iCurHP >= 0) ? MIN(dam, iCurHP) : dam) * pAttacker->GetPoint(POINT_HIT_HP_RECOVERY) / 100; //@fixme107

				if (i)
				{
					CreateFly(FLY_HP_SMALL, pAttacker);
					pAttacker->PointChange(POINT_HP, i);
				}
			}

			// Her vuruþta SP geri kazanýmý
			if (pAttacker->GetPoint(POINT_HIT_SP_RECOVERY) && number(0, 4) > 0)
			{
				int i = ((iCurHP >= 0) ? MIN(dam, iCurHP) : dam) * pAttacker->GetPoint(POINT_HIT_SP_RECOVERY) / 100; //@fixme107

				if (i)
				{
					CreateFly(FLY_SP_SMALL, pAttacker);
					pAttacker->PointChange(POINT_SP, i);
				}
			}
#endif
			// Rakibin manasýný ortadan kaldýrýr.
			if (pAttacker->GetPoint(POINT_MANA_BURN_PCT))
			{
				if (number(1, 100) <= pAttacker->GetPoint(POINT_MANA_BURN_PCT))
					PointChange(POINT_SP, -50);
			}
		}
	}

	//
	// Düz vuruþ veya beceriden bonus hasar/savunma hesapla
	//
	switch (type)
	{
		case DAMAGE_TYPE_NORMAL:
		case DAMAGE_TYPE_NORMAL_RANGE:
			if (pAttacker)
				if (pAttacker->GetPoint(POINT_NORMAL_HIT_DAMAGE_BONUS))
					dam = dam * (100 + pAttacker->GetPoint(POINT_NORMAL_HIT_DAMAGE_BONUS)) / 100;
#ifdef ENABLE_AVG_PVM
				if (IsNPC())
					dam = dam * (100 + pAttacker->GetPoint(POINT_ATTBONUS_MEDI_PVM)) / 100;
#endif

			dam = dam * (100 - MIN(99, GetPoint(POINT_NORMAL_HIT_DEFEND_BONUS))) / 100;
			break;

		case DAMAGE_TYPE_MELEE:
		case DAMAGE_TYPE_RANGE:
		case DAMAGE_TYPE_FIRE:
		case DAMAGE_TYPE_ICE:
		case DAMAGE_TYPE_ELEC:
		case DAMAGE_TYPE_MAGIC:
			if (pAttacker)
				if (pAttacker->GetPoint(POINT_SKILL_DAMAGE_BONUS))
					dam = dam * (100 + pAttacker->GetPoint(POINT_SKILL_DAMAGE_BONUS)) / 100;

			dam = dam * (100 - MIN(99, GetPoint(POINT_SKILL_DEFEND_BONUS))) / 100;
			break;

		default:
			break;
	}

	//
	// Mana Shield (Black God Guardian)
	//
	if (IsAffectFlag(AFF_MANASHIELD))
	{
		// The smaller the POINT_MANASHIELD is, the better
		int iDamageSPPart = dam / 3;
		int iDamageToSP = iDamageSPPart * GetPoint(POINT_MANASHIELD) / 100;
		int iSP = GetSP();

		// With SP, damage is unconditionally reduced by half
		if (iDamageToSP <= iSP)
		{
			PointChange(POINT_SP, -iDamageToSP);
			dam -= iDamageSPPart;
		}
		else
		{
			// My mental power is insufficient, so I have to cut my blood more.
			PointChange(POINT_SP, -GetSP());
			dam -= iSP * 100 / MAX(GetPoint(POINT_MANASHIELD), 1);
		}
	}

	//
	// Increased overall defense (mole item)
	//
	if (GetPoint(POINT_MALL_DEFBONUS) > 0)
	{
		int dec_dam = MIN(200, dam * GetPoint(POINT_MALL_DEFBONUS) / 100);
		dam -= dec_dam;
	}

	if (pAttacker)
	{
		//
		// Increase overall attack power (mole item)
		//
		if (pAttacker->GetPoint(POINT_MALL_ATTBONUS) > 0)
		{
			int add_dam = MIN(300, dam * pAttacker->GetLimitPoint(POINT_MALL_ATTBONUS) / 100);
			dam += add_dam;
		}

		if (pAttacker->IsPC()
#ifdef ENABLE_BOT_PLAYER
			|| pAttacker->IsBotCharacter()
#endif
			)
		{
#ifndef ENABLE_DISABLE_EMPIRE_DAMAGE_BONUS
			int iEmpire = pAttacker->GetEmpire();
			long lMapIndex = pAttacker->GetMapIndex();
			int iMapEmpire = SECTREE_MANAGER::instance().GetEmpireFromMapIndex (lMapIndex);

			// If you are from another empire, damage is reduced by 10%.
			if (iEmpire && iMapEmpire && iEmpire != iMapEmpire)
			{
				dam = dam * 9 / 10;
			}
#endif

			if (!IsPC()
#ifdef ENABLE_BOT_PLAYER
				&& !IsBotCharacter()
#endif
				&& GetMonsterDrainSPPoint())
			{
				int iDrain = GetMonsterDrainSPPoint();

				if (iDrain <= pAttacker->GetSP())
				{
					pAttacker->PointChange(POINT_SP, -iDrain);
				}
				else
				{
					int iSP = pAttacker->GetSP();
					pAttacker->PointChange(POINT_SP, -iSP);
				}
			}
		}
		else if (pAttacker->IsGuardNPC())
		{
			SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_NO_REWARD);
			Stun();
			return true;
		}
	}

	if (!GetSectree() || GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		return false;

#ifdef ENABLE_DRAGON_LAIR_RENEWAL_IMMUNE
    // Dragon Lair new immunity system (v17.3+)
    int iIgnoreNewDragonImmunityFlag = (quest::CQuestManager::instance().GetEventFlag("ignore_dragonlair_new_immune") > 0);
    if (GetRaceNum() == 2493 && !iIgnoreNewDragonImmunityFlag)
    {
        LPDUNGEON pDungeon = pAttacker->GetDungeon();
        if (pDungeon)
        {
            long lMapIndex = pDungeon->GetMapIndex();
            if (lMapIndex >= 208 * 10000 && lMapIndex < (208 + 1) * 10000)
            {
                short sStones[4] = {8031, 8032, 8033, 8034};
                for (auto stone : sStones)
                {
                    if (SECTREE_MANAGER::instance().GetMonsterCountInMap(lMapIndex, stone) > 0)
                    {
                        SendDamagePacket(pAttacker, 0, DAMAGE_BLOCK);
                        return false;
                    }
                }
            }
        }
    }
#endif

	if (!IsPC()
#ifdef ENABLE_BOT_PLAYER
		&& !IsBotCharacter()
#endif
		)
	{
		if (m_pkParty && m_pkParty->GetLeader())
			m_pkParty->GetLeader()->SetLastAttacked(get_dword_time());
		else
			SetLastAttacked(get_dword_time());

		// Monster dialogue: when correct
		MonsterChat(MONSTER_CHAT_ATTACKED);
	}

	if (IsStun())
	{
		Dead(pAttacker);
		return true;
	}

	if (IsDead())
		return true;

	// Prevents death from poison attacks
	if (type == DAMAGE_TYPE_POISON)
	{
		if (GetHP() - dam <= 0)
		{
			dam = GetHP() - 1;
		}
	}

	// ------------------------
	// Turkey premium mode
	// -----------------------
	if (pAttacker && (pAttacker->IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| pAttacker->IsBotCharacter()
#endif
		))
	{
		int iDmgPct = CHARACTER_MANAGER::instance().GetUserDamageRate(pAttacker);
		dam = dam * iDmgPct / 100;
	}

	// STONE SKIN: Damage reduced by half
	if (IsMonster() && IsStoneSkinner())
	{
		if (GetHPPct() < GetMobTable().bStoneSkinPoint)
			dam /= 2;
	}

	//PROF_UNIT puRest1("Rest1");
	if (pAttacker)
	{
		// DEATH BLOW: 4x damage with probability (!? Only use monsters for the current event or siege)
		if (pAttacker->IsMonster() && pAttacker->IsDeathBlower())
		{
			if (pAttacker->IsDeathBlow())
			{
				if (number(JOB_WARRIOR, JOB_MAX_NUM - 1) == GetJob()) // @fixme192 (1, 4)
				{
					IsDeathBlow = true;
					dam = dam * 4;
				}
			}
		}

		dam = BlueDragon_Damage(this, pAttacker, dam);

		BYTE damageFlag = 0;

		if (type == DAMAGE_TYPE_POISON)
			damageFlag = DAMAGE_POISON;
		else
			damageFlag = DAMAGE_NORMAL;

		if (IsCritical == true)
			damageFlag |= DAMAGE_CRITICAL;

		if (IsPenetrate == true)
			damageFlag |= DAMAGE_PENETRATE;

		//Final damage correction
		float damMul = this->GetDamMul();
		float tempDam = dam;
#ifdef ENABLE_ATTENDANCE_EVENT
		dam = (GetRaceNum() >= 6500 && GetRaceNum() <= 6504) ? 1 : tempDam * damMul + 0.5f;
#else
		dam = tempDam * damMul + 0.5f;
#endif
#ifdef ENABLE_BUFFI_SYSTEM
		CBuffiManager::instance().OnOwnerAttack(pAttacker, this);
		// Buffi is not allowed in PvP: while it is summoned the owner's damage to players is capped at 1.
		if (dam > 1 && IsPC() && pAttacker->IsPC() && pAttacker != this && CBuffiManager::instance().IsSummoned(pAttacker))
			dam = 1;
#endif
#ifdef ENABLE_KILL_STATISTICS
		if (pAttacker->GetTopDamage() < dam)
		{
			pAttacker->SetTopDamage(dam);
			pAttacker->SendKillStatisticsPacket();
		}
#endif

		if (pAttacker)
		{
#ifdef ENABLE_ZODIAC_MISSION
			if (GetRaceNum() >= 9884 && GetRaceNum() <= 9895)
				SendDamagePacket(pAttacker, 1, damageFlag);
			else
#endif
			{
#ifdef __DUNGEON_INFO__
				if (pAttacker->IsPC() && !IsPC())
				{
					if (m_mapDungeonList.find(GetRaceNum()) != m_mapDungeonList.end())
					{
						char szBuff[254];
						snprintf(szBuff, sizeof(szBuff), "dungeon.%u_damage", GetRaceNum());
						if (pAttacker->GetQuestFlag(szBuff) < dam)
						{
							pAttacker->SetQuestFlag(szBuff, dam);
							pAttacker->SendDungeonCooldown(GetRaceNum());
						}
					}
				}
#endif

				SendDamagePacket(pAttacker, dam, damageFlag);
			}
		}

		if (test_server)
		{
			int iTmpPercent = 0; // @fixme136
			if (GetMaxHP() >= 0)
				iTmpPercent = (GetHP() * 100) / GetMaxHP();

			if (pAttacker)
			{
				pAttacker->ChatPacket(CHAT_TYPE_INFO, "-> %s, DAM %d HP %d(%d%%) %s%s",
					GetName(),
					dam,
					GetHP(),
					iTmpPercent,
					IsCritical ? "crit " : "",
					IsPenetrate ? "pene " : "",
					IsDeathBlow ? "deathblow " : "");
			}

			ChatPacket(CHAT_TYPE_PARTY, "<- %s, DAM %d HP %d(%d%%) %s%s",
				pAttacker ? pAttacker->GetName() : 0,
				dam,
				GetHP(),
				iTmpPercent,
				IsCritical ? "crit " : "",
				IsPenetrate ? "pene " : "",
				IsDeathBlow ? "deathblow " : "");
		}

		if (m_bDetailLog)
		{
			ChatPacket(CHAT_TYPE_INFO, "%s[%d]s Attack Position: %d %d", pAttacker->GetName(), (DWORD) pAttacker->GetVID(), pAttacker->GetX(), pAttacker->GetY());
		}
#ifdef ENABLE_RANKING
		if (pAttacker->IsPC())
		{
			if (IsPC())
			{
				switch (type)
				{
					case DAMAGE_TYPE_NORMAL:
					case DAMAGE_TYPE_NORMAL_RANGE:
					{
						if (dam > pAttacker->GetRankPoints(3))
							pAttacker->SetRankPoints(3, dam);
					}
					break;
					case DAMAGE_TYPE_MELEE:
					case DAMAGE_TYPE_RANGE:
					case DAMAGE_TYPE_FIRE:
					case DAMAGE_TYPE_ICE:
					case DAMAGE_TYPE_ELEC:
					case DAMAGE_TYPE_MAGIC:
					{
						if (dam > pAttacker->GetRankPoints(4))
							pAttacker->SetRankPoints(4, dam);
					}
					break;
				default:
					break;
				}
			}
			else if (IsMonster())
			{
				if (GetMobRank() >= MOB_RANK_BOSS)
				{
					switch (type)
					{
						case DAMAGE_TYPE_NORMAL:
						case DAMAGE_TYPE_NORMAL_RANGE:
						{
							if (dam > pAttacker->GetRankPoints(8))
								pAttacker->SetRankPoints(8, dam);
						}
						break;
						case DAMAGE_TYPE_MELEE:
						case DAMAGE_TYPE_RANGE:
						case DAMAGE_TYPE_FIRE:
						case DAMAGE_TYPE_ICE:
						case DAMAGE_TYPE_ELEC:
						case DAMAGE_TYPE_MAGIC:
						{
							if (dam > pAttacker->GetRankPoints(9))
								pAttacker->SetRankPoints(9, dam);
						}
						break;
					default:
						break;
					}
				}
			}
			else if (IsStone())
			{
				switch (type)
				{
					case DAMAGE_TYPE_NORMAL:
					case DAMAGE_TYPE_NORMAL_RANGE:
					{
						if (dam > pAttacker->GetRankPoints(16))
							pAttacker->SetRankPoints(16, dam);
					}
					break;
				default:
					break;
				}
			}
		}
#endif
	}

	//
	// !!!!!!!!! The actual HP reduction part !!!!!!!!!
	//
	if (!cannot_dead)
	{
		if (GetHP() - dam <= 0) // @fixme137
			dam = GetHP();
#ifdef ENABLE_ZODIAC_MISSION
		if (GetRaceNum() >= 9884 && GetRaceNum() <= 9895)
			PointChange(POINT_HP, -1, false);
		else
#endif
			PointChange(POINT_HP, -dam, false);

#ifdef ENABLE_DUNGEON_INFO
		if (GetMapIndex() >= 10000){
			if (CDungeonInfoExtern::instance().CheckBossKillMap(GetMapIndex()) == true)
			{
				if (pAttacker){
					if(pAttacker->IsMonster()){
						if(CDungeonInfoExtern::instance().GetIdBoss(GetMapIndex()) == pAttacker->GetMobTable().dwVnum){
							if (dam > GetDamageReceivedDungeonInfo()){
								SetDamageReceivedDungeonInfo(dam);
							}
						}
					}
					if(IsMonster())
					{
						if(CDungeonInfoExtern::instance().GetIdBoss(GetMapIndex()) == GetMobTable().dwVnum){
							if (dam > pAttacker->GetDamageDoneDungeonInfo()){
								pAttacker->SetDamageDoneDungeonInfo(dam);
							}
						}
					}
					
				}
			}
		}
#endif
	}

	if (pAttacker && dam > 0 && IsNPC())
	{
		TDamageMap::iterator it = m_map_kDamage.find(pAttacker->GetVID());

		if (it == m_map_kDamage.end())
		{
			m_map_kDamage.insert(TDamageMap::value_type(pAttacker->GetVID(), TBattleInfo(dam, 0)));
			it = m_map_kDamage.find(pAttacker->GetVID());
		}
		else
		{
			it->second.iTotalDamage += dam;
		}

		StartRecoveryEvent();
		UpdateAggrPointEx(pAttacker, type, dam, it->second);
	}

	if (GetHP() <= 0)
	{
		Stun();

		if (pAttacker && !pAttacker->IsNPC())
			m_dwKillerPID = pAttacker->GetPlayerID();
		else
			m_dwKillerPID = 0;
	}

	return false;
}

void CHARACTER::DistributeHP(LPCHARACTER pkKiller)
{
	if (pkKiller && pkKiller->GetDungeon()) // There are no dumplings in the dungeon.
		return;
}

static void GiveExp(LPCHARACTER from, LPCHARACTER to, int iExp)
{
	iExp = CALCULATE_VALUE_LVDELTA(to->GetLevel(), from->GetLevel(), iExp);
#ifdef ENABLE_BUFFI_SYSTEM
	CBuffiManager::instance().OnOwnerGainExp(to, iExp);
#endif

	if (distribution_test_server)
		iExp *= 3;

	int iBaseExp = iExp;

	iExp = iExp * (100 + CPrivManager::instance().GetPriv(to, PRIV_EXP_PCT)) / 100;
	{
		if (to->IsEquipUniqueItem(UNIQUE_ITEM_LARBOR_MEDAL))
			iExp += iExp * 20 / 100;

		if (to->GetMapIndex() >= 660000 && to->GetMapIndex() < 670000)
			iExp += iExp * 20 / 100;

		if (to->GetPoint(POINT_EXP_DOUBLE_BONUS))
			if (number(1, 100) <= to->GetPoint(POINT_EXP_DOUBLE_BONUS))
				iExp += iExp * 30 / 100;

		if (to->IsEquipUniqueItem(UNIQUE_ITEM_DOUBLE_EXP))
			iExp += iExp * 50 / 100;

		switch (to->GetMountVnum())
		{
			case 20110:
			case 20111:
			case 20112:
			case 20113:
				if (to->IsEquipUniqueItem(71115) || to->IsEquipUniqueItem(71117) || to->IsEquipUniqueItem(71119) ||
						to->IsEquipUniqueItem(71121) )
				{
					iExp += iExp * 10 / 100;
				}
				break;

			case 20114:
			case 20120:
			case 20121:
			case 20122:
			case 20123:
			case 20124:
			case 20125:

				iExp += iExp * 30 / 100;
				break;
		}
	}

	{
		if (to->GetPremiumRemainSeconds (PREMIUM_EXP) > 0)
		{
			iExp += (iExp * 50 / 100);
		}

		if (to->IsEquipUniqueGroup (UNIQUE_GROUP_RING_OF_EXP) == true)
		{
			iExp += (iExp * 50 / 100);
		}
		iExp += iExp * to->GetMarriageBonus (UNIQUE_ITEM_MARRIAGE_EXP_BONUS) / 100;
	}

	iExp += (iExp * to->GetPoint(POINT_RAMADAN_CANDY_BONUS_EXP)/100);
	iExp += (iExp * to->GetPoint(POINT_MALL_EXPBONUS)/100);
	iExp += (iExp * to->GetPoint(POINT_EXP)/100);

	if (test_server)
	{
		sys_log(0, "Bonus Exp : Ramadan Candy: %d MallExp: %d PointExp: %d",
			to->GetPoint(POINT_RAMADAN_CANDY_BONUS_EXP),
			to->GetPoint(POINT_MALL_EXPBONUS),
			to->GetPoint(POINT_EXP)
		);
	}

	iExp = iExp * CHARACTER_MANAGER::instance().GetMobExpRate(to) / 100;
	if (iExp < 0)
	{
	iExp = 1 * (to->GetNextExp() / 100);
	}

	iExp = MIN(to->GetNextExp() / 10, iExp);

	if (test_server)
	{
		if (quest::CQuestManager::instance().GetEventFlag("exp_bonus_log") && iBaseExp > 0)
			to->ChatPacket(CHAT_TYPE_INFO, "exp bonus %d%%", (iExp - iBaseExp) * 100 / iBaseExp);
	}

	iExp = AdjustExpByLevel(to, iExp);

#ifdef ENABLE_ANTI_EXP
	if (to->GetAntiExp())
		return;
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
	if (to->IsConquerorMap(to->GetMapIndex()))
	{
		to->PointChange(POINT_CONQUEROR_EXP, iExp, true);
		from->CreateFly(FLY_CONQUEROR_EXP, to);
	}
	else
	{
		to->PointChange(POINT_EXP, iExp, true);
		from->CreateFly(FLY_EXP, to);
	}
#else
	to->PointChange(POINT_EXP, iExp, true);
	from->CreateFly(FLY_EXP, to);
#endif

	{
		LPCHARACTER you = to->GetMarryPartner();

		if (you)
		{
			// sometimes, this overflows
			DWORD dwUpdatePoint = 2000 * iExp / to->GetLevel() / to->GetLevel() / 3;

			if (to->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0 ||
					you->GetPremiumRemainSeconds(PREMIUM_MARRIAGE_FAST) > 0)
				dwUpdatePoint = (DWORD)(dwUpdatePoint * 3);

			marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(to->GetPlayerID());

			// DIVORCE_NULL_BUG_FIX
			if (pMarriage && pMarriage->IsNear())
				pMarriage->Update(dwUpdatePoint);
			// END_OF_DIVORCE_NULL_BUG_FIX
		}
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (to->GetActiveGrowthPet())
	{
		to->GetActiveGrowthPet()->RewardEXP(EXP_TYPE_MOB, iBaseExp);
		from->CreateFly(FLY_EXP, to->GetActiveGrowthPet()->GetGrowthPet());
	}
#endif
}

namespace NPartyExpDistribute
{
	struct FPartyTotaler
	{
#ifdef ENABLE_LEVEL_INT
		long	total;
#else
		int		total;
#endif
		int		member_count;
		int		x, y;

		FPartyTotaler(LPCHARACTER center)
			: total(0), member_count(0), x(center->GetX()), y(center->GetY())
		{};

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
			{
				total += ch->GetLevel();

				++member_count;
			}
		}
	};

	struct FPartyDistributor
	{
#ifdef ENABLE_LEVEL_INT
		long	total;
#else
		int		total;
#endif
		LPCHARACTER	c;
		int		x, y;
		DWORD		_iExp;
		int		m_iMode;
		int		m_iMemberCount;

#ifdef ENABLE_LEVEL_INT
		FPartyDistributor(LPCHARACTER center, int member_count, long total, DWORD iExp, int iMode)
#else
		FPartyDistributor(LPCHARACTER center, int member_count, int total, DWORD iExp, int iMode)
#endif
			: total(total), c(center), x(center->GetX()), y(center->GetY()), _iExp(iExp), m_iMode(iMode), m_iMemberCount(member_count)
			{
				if (m_iMemberCount == 0)
					m_iMemberCount = 1;
			};

		void operator () (LPCHARACTER ch)
		{
			if (!ch)
				return;

			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
			{
				DWORD iExp2 = 0;

				switch (m_iMode)
				{
					case PARTY_EXP_DISTRIBUTION_NON_PARITY:
						iExp2 = (DWORD) ((_iExp * ch->GetLevel()) / total);
						break;

					case PARTY_EXP_DISTRIBUTION_PARITY:
						iExp2 = _iExp / m_iMemberCount;
						break;

					default:
						sys_err("Unknown party exp distribution mode %d", m_iMode);
						return;
				}

				GiveExp(c, ch, iExp2);
			}
		}
	};
}

typedef struct SDamageInfo
{
	int iDam;
	LPCHARACTER pAttacker;
	LPPARTY pParty;

	void Clear()
	{
		pAttacker = NULL;
		pParty = NULL;
	}

	inline void Distribute(LPCHARACTER ch, int iExp)
	{
		if (pAttacker)
			GiveExp(ch, pAttacker, iExp);
		else if (pParty)
		{
			NPartyExpDistribute::FPartyTotaler f(ch);
			pParty->ForEachOnMapMember(f, ch->GetMapIndex());	//@fixme522

			if (pParty->IsPositionNearLeader(ch))
				iExp = iExp * (100 + pParty->GetExpBonusPercent()) / 100;

			// Drive experience (the party's earned experience is subtracted by 5% and given first)
			if (pParty->GetExpCentralizeCharacter())
			{
				LPCHARACTER tch = pParty->GetExpCentralizeCharacter();

				if (DISTANCE_APPROX(ch->GetX() - tch->GetX(), ch->GetY() - tch->GetY()) <= PARTY_DEFAULT_RANGE)
				{
					int iExpCenteralize = (int) (iExp * 0.05f);
					iExp -= iExpCenteralize;

					GiveExp(ch, pParty->GetExpCentralizeCharacter(), iExpCenteralize);
				}
			}

			NPartyExpDistribute::FPartyDistributor fDist(ch, f.member_count, f.total, iExp, pParty->GetExpDistributionMode());
			pParty->ForEachOnMapMember(fDist, ch->GetMapIndex());	//@fixme522
		}
	}
} TDamageInfo;

#ifdef ENABLE_KILL_EVENT_FIX
LPCHARACTER CHARACTER::GetMostAttacked()
{
	int iMostDam = -1;
	LPCHARACTER pkChrMostAttacked = NULL;
	auto it = m_map_kDamage.begin();

	while (it != m_map_kDamage.end())
	{
		// getting information from the iterator
		const VID& c_VID = it->first;
		const int iDam = it->second.iTotalDamage;

		// increasing the iterator
		++it;

		// finding the character from his vid
		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);

		// if the attacked is now offline
		if (!pAttacker)
			continue;

		// if the attacker is not a player
		if (pAttacker->IsNPC())
			continue;

		// if the player is too far
		if (DISTANCE_APPROX(GetX() - pAttacker->GetX(), GetY() - pAttacker->GetY()) > 5000)
			continue;

		if (iDam > iMostDam)
		{
			pkChrMostAttacked = pAttacker;
			iMostDam = iDam;
		}
	}

	return pkChrMostAttacked;
}
#endif

LPCHARACTER CHARACTER::DistributeExp()
{
	int iExpToDistribute = GetExp();

	if (iExpToDistribute <= 0)
		return NULL;

	int	iTotalDam = 0;
	LPCHARACTER pkChrMostAttacked = NULL;
	int iMostDam = 0;

	typedef std::vector<TDamageInfo> TDamageInfoTable;
	TDamageInfoTable damage_info_table;
	std::map<LPPARTY, TDamageInfo> map_party_damage;

	damage_info_table.reserve(m_map_kDamage.size());

	TDamageMap::iterator it = m_map_kDamage.begin();

	// First, filter out people who are not around. (50m)
	while (it != m_map_kDamage.end())
	{
		const VID & c_VID = it->first;
		int iDam = it->second.iTotalDamage;

		++it;

#ifdef ENABLE_PARTY_EXP_FIX
		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);

		if (!pAttacker || !pAttacker->IsPC() || pAttacker->IsNPC())// @fixme175
			continue;

		int dist = DISTANCE_APPROX(GetX() - pAttacker->GetX(), GetY() - pAttacker->GetY());
		if (dist > 10000 || (dist > 5000 && !pAttacker->GetParty()))
			continue;
#else
		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);
		if (!pAttacker || !pAttacker->IsPC())// @fixme205
			continue;
		if (!pAttacker || pAttacker->IsNPC() || DISTANCE_APPROX(GetX() - pAttacker->GetX(), GetY() - pAttacker->GetY()) > 5000)// @fixme175
			continue;
#endif

		iTotalDam += iDam;
		if (!pkChrMostAttacked || iDam > iMostDam)
		{
			pkChrMostAttacked = pAttacker;
			iMostDam = iDam;
		}

		if (pAttacker->GetParty())
		{
			std::map<LPPARTY, TDamageInfo>::iterator it = map_party_damage.find(pAttacker->GetParty());
			if (it == map_party_damage.end())
			{
				TDamageInfo di;
				di.iDam = iDam;
				di.pAttacker = NULL;
				di.pParty = pAttacker->GetParty();
				map_party_damage.insert(std::make_pair(di.pParty, di));
			}
			else
			{
				it->second.iDam += iDam;
			}
		}
		else
		{
			TDamageInfo di;

			di.iDam = iDam;
			di.pAttacker = pAttacker;
			di.pParty = NULL;

			//sys_log(0, "__ pq_damage %s %d", pAttacker->GetName(), iDam);
			//pq_damage.push(di);
			damage_info_table.push_back(di);
		}
	}

	for (std::map<LPPARTY, TDamageInfo>::iterator it = map_party_damage.begin(); it != map_party_damage.end(); ++it)
	{
		damage_info_table.push_back(it->second);
	}

	SetExp(0);

	if (iTotalDam == 0) // Returns if damage dealt is 0
		return NULL;

	if (m_pkChrStone) // If there is a stone, half of the experience is passed to the stone.
	{
		//sys_log(0, "__ Give half to Stone : %d", iExpToDistribute>>1);
		int iExp = iExpToDistribute >> 1;
		m_pkChrStone->SetExp(m_pkChrStone->GetExp() + iExp);
		iExpToDistribute -= iExp;
	}

	sys_log(1, "%s total exp: %d, damage_info_table.size() == %d, TotalDam %d",
			GetName(), iExpToDistribute, damage_info_table.size(), iTotalDam);
	//sys_log(1, "%s total exp: %d, pq_damage.size() == %d, TotalDam %d",
	//GetName(), iExpToDistribute, pq_damage.size(), iTotalDam);

	if (damage_info_table.empty())
		return NULL;

	// The person who inflicts the most damage recovers HP.
	DistributeHP(pkChrMostAttacked); // dumpling system

	{
		// The person or party that inflicts the most damage eats 20% of the total experience + the amount of experience it hits.
		TDamageInfoTable::iterator di = damage_info_table.begin();
		{
			TDamageInfoTable::iterator it;

			for (it = damage_info_table.begin(); it != damage_info_table.end();++it)
			{
				if (it->iDam > di->iDam)
					di = it;
			}
		}

		int	iExp = iExpToDistribute / 5;
		iExpToDistribute -= iExp;

		float fPercent = (float) di->iDam / iTotalDam;

		if (fPercent > 1.0f)
		{
			sys_err("DistributeExp percent over 1.0 (fPercent %f name %s)", fPercent, di->pAttacker->GetName());
			fPercent = 1.0f;
		}

		iExp += (int) (iExpToDistribute * fPercent);

		//sys_log(0, "%s given exp percent %.1f + 20 dam %d", GetName(), fPercent * 100.0f, di.iDam);

		di->Distribute(this, iExp);

		// If you eat 100%, return it.
		if (fPercent == 1.0f)
			return pkChrMostAttacked;

		di->Clear();
	}

	{
		// The remaining 80% of the experience is distributed.
		TDamageInfoTable::iterator it;

		for (it = damage_info_table.begin(); it != damage_info_table.end(); ++it)
		{
			TDamageInfo & di = *it;

			float fPercent = (float) di.iDam / iTotalDam;

			if (fPercent > 1.0f)
			{
				sys_err("DistributeExp percent over 1.0 (fPercent %f name %s)", fPercent, di.pAttacker->GetName());
				fPercent = 1.0f;
			}

			//sys_log(0, "%s given exp percent %.1f dam %d", GetName(), fPercent * 100.0f, di.iDam);
			di.Distribute(this, (int) (iExpToDistribute * fPercent));
		}
	}

	return pkChrMostAttacked;
}

int CHARACTER::GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, int iArrowCount/* = 1 */)
{
	LPITEM pkBow;

	if (!(pkBow = GetWear(WEAR_WEAPON)) || pkBow->GetProto()->bSubType != WEAPON_BOW)
	{
		return 0;
	}

	LPITEM pkArrow;

	if (!(pkArrow = GetWear(WEAR_ARROW)) || pkArrow->GetType() != ITEM_WEAPON ||
			pkArrow->GetProto()->bSubType != WEAPON_ARROW)
	{
		return 0;
	}

	iArrowCount = MIN(iArrowCount, pkArrow->GetCount());

	*ppkBow = pkBow;
	*ppkArrow = pkArrow;

	return iArrowCount;
}

void CHARACTER::UseArrow(LPITEM pkArrow, DWORD dwArrowCount)
{
#ifdef ENABLE_CRASH_CORE_ARROW_FIX
	if (!pkArrow)
		return;
#endif
	int iCount = pkArrow->GetCount();
	DWORD dwVnum = pkArrow->GetVnum();
#ifdef DEFAULT_FINITE_ITEMS
	iCount = iCount - MIN(iCount, dwArrowCount);
#endif
	pkArrow->SetCount(iCount);

	if (iCount == 0)
	{
		LPITEM pkNewArrow = FindSpecifyItem(dwVnum);

		sys_log(0, "UseArrow : FindSpecifyItem %u %p", dwVnum, get_pointer(pkNewArrow));

		if (pkNewArrow)
			EquipItem(pkNewArrow);
	}
}

class CFuncShoot
{
	public:
		LPCHARACTER	m_me;
		BYTE		m_bType;
		bool		m_bSucceed;

		CFuncShoot(LPCHARACTER ch, BYTE bType) : m_me(ch), m_bType(bType), m_bSucceed(FALSE)
		{
		}

		void operator () (DWORD dwTargetVID)
		{
			if (m_bType > 1)
			{
				if (g_bSkillDisable)
					return;

				m_me->m_SkillUseInfo[m_bType].SetMainTargetVID(dwTargetVID);
				/*if (m_bType == SKILL_BIPABU || m_bType == SKILL_KWANKYEOK)
					m_me->m_SkillUseInfo[m_bType].ResetHitCount();*/
			}

			LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(dwTargetVID);

			if (!pkVictim)
				return;

			// cannot attack
			if (!battle_is_attackable(m_me, pkVictim))
				return;

			if (m_me->IsNPC())
			{
				if (DISTANCE_APPROX(m_me->GetX() - pkVictim->GetX(), m_me->GetY() - pkVictim->GetY()) > 5000)
					return;
			}

			LPITEM pkBow, pkArrow;

			switch (m_bType)
			{
				case 0: // normal bow
				{
					int iDam = 0;

					if (m_me->IsPC()
#ifdef ENABLE_BOT_PLAYER
						|| m_me->IsBotCharacter()
#endif
						)
					{
						if (m_me->GetJob() != JOB_ASSASSIN)
							return;

						if (0 == m_me->GetArrowAndBow(&pkBow, &pkArrow))
							return;

#ifdef ENABLE_CRASH_CORE_ARROW_FIX
						if (!pkBow)
							break;
#endif

						if (m_me->GetSkillGroup() != 0)
						{
							if (!m_me->IsNPC() && m_me->GetSkillGroup() != 2)
							{
								if (m_me->GetSP() < 5)
									return;

								m_me->PointChange(POINT_SP, -5);
							}
						}

						iDam = CalcArrowDamage(m_me, pkVictim, pkBow, pkArrow);
						m_me->UseArrow(pkArrow, 1);

						// check speed hack
						DWORD dwCurrentTime = get_dword_time();
						if (IS_SPEED_HACK(m_me, pkVictim, dwCurrentTime))
							iDam = 0;
					}
					else
						iDam = CalcMeleeDamage(m_me, pkVictim);

					NormalAttackAffect(m_me, pkVictim);

					// Damage calculation
					iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_BOW)) / 100;

					//sys_log(0, "%s arrow %s dam %d", m_me->GetName(), pkVictim->GetName(), iDam);

					m_me->OnMove(true);
					pkVictim->OnMove();

					if (pkVictim->CanBeginFight())
						pkVictim->BeginFight(m_me);

					pkVictim->Damage(m_me, iDam, DAMAGE_TYPE_NORMAL_RANGE);
					// End of hit calculation section
				}
				break;

				case 1:	// common magic
				{
					int iDam;

					if (m_me->IsPC()
#ifdef ENABLE_BOT_PLAYER
						|| m_me->IsBotCharacter()
#endif
						)
						return;

					iDam = CalcMagicDamage(m_me, pkVictim);

					NormalAttackAffect(m_me, pkVictim);

					// Damage calculation
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
					const int resist_magic = MINMAX(0, pkVictim->GetPoint(POINT_RESIST_MAGIC), 100);
					const int resist_magic_reduction = MINMAX(0, (m_me->GetJob() == JOB_SURA) ? m_me->GetPoint(POINT_RESIST_MAGIC_REDUCTION) / 2 : m_me->GetPoint(POINT_RESIST_MAGIC_REDUCTION), 50);
					const int total_res_magic = MINMAX(0, resist_magic - resist_magic_reduction, 100);
					iDam = iDam * (100 - total_res_magic) / 100;
#else
					iDam = iDam * (100 - pkVictim->GetPoint(POINT_RESIST_MAGIC)) / 100;
#endif

					//sys_log(0, "%s arrow %s dam %d", m_me->GetName(), pkVictim->GetName(), iDam);

					m_me->OnMove(true);
					pkVictim->OnMove();

					if (pkVictim->CanBeginFight())
						pkVictim->BeginFight(m_me);

					pkVictim->Damage(m_me, iDam, DAMAGE_TYPE_MAGIC);
					// End of hit calculation section
				}
				break;

				case SKILL_YEONSA: // Repetitive Shot
				{
					int iUseArrow = 1;

					// When calculating only the total
					{
						if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
						{
#ifdef ENABLE_CRASH_CORE_ARROW_FIX
							if (!pkBow)
								break;
#endif
							m_me->OnMove(true);
							pkVictim->OnMove();

							if (pkVictim->CanBeginFight())
								pkVictim->BeginFight(m_me);

							m_me->ComputeSkill(m_bType, pkVictim);
							m_me->UseArrow(pkArrow, iUseArrow);

							if (pkVictim->IsDead())
								break;

						}
						else
							break;
					}
				}
				break;

				case SKILL_KWANKYEOK:
				{
					int iUseArrow = 1;
					if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
					{
#ifdef ENABLE_CRASH_CORE_ARROW_FIX
					if (!pkBow)
						break;
#endif
						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						sys_log(0, "%s kwankeyok %s", m_me->GetName(), pkVictim->GetName());
						m_me->ComputeSkill(m_bType, pkVictim);
						m_me->UseArrow(pkArrow, iUseArrow);
					}
				}
				break;

#ifdef ENABLE_NINETH_SKILL
				case SKILL_PUNGLOEPO:
				{
					int iUseArrow = 1;

					if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
					{
#ifdef ENABLE_CRASH_CORE_ARROW_FIX
						if (!pkBow)
							break;
#endif
						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						sys_log(0, "%s 9th skill ninja archer %s", m_me->GetName(), pkVictim->GetName());
						m_me->ComputeSkill(m_bType, pkVictim);
						m_me->UseArrow(pkArrow, iUseArrow);
					}
				}
				break;
#endif

				case SKILL_GIGUNG:
				{
					int iUseArrow = 1;
					if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
					{
#ifdef ENABLE_CRASH_CORE_ARROW_FIX
						if (!pkBow)
							break;
#endif
						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						sys_log(0, "%s gigung %s", m_me->GetName(), pkVictim->GetName());
						m_me->ComputeSkill(m_bType, pkVictim);
						m_me->UseArrow(pkArrow, iUseArrow);
					}
				}
				break;

				case SKILL_HWAJO:
				{
					int iUseArrow = 1;
					if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
					{
#ifdef ENABLE_CRASH_CORE_ARROW_FIX
						if (!pkBow)
							break;
#endif
						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						sys_log(0, "%s hwajo %s", m_me->GetName(), pkVictim->GetName());
						m_me->ComputeSkill(m_bType, pkVictim);
						m_me->UseArrow(pkArrow, iUseArrow);
					}
				}
				break;

				case SKILL_HORSE_WILDATTACK_RANGE:
				{
					int iUseArrow = 1;
					if (iUseArrow == m_me->GetArrowAndBow(&pkBow, &pkArrow, iUseArrow))
					{
#ifdef ENABLE_CRASH_CORE_ARROW_FIX
						if (!pkBow)
							break;
#endif
						m_me->OnMove(true);
						pkVictim->OnMove();

						if (pkVictim->CanBeginFight())
							pkVictim->BeginFight(m_me);

						sys_log(0, "%s horse_wildattack %s", m_me->GetName(), pkVictim->GetName());
						m_me->ComputeSkill(m_bType, pkVictim);
						m_me->UseArrow(pkArrow, iUseArrow);
					}
				}
				break;

				case SKILL_MARYUNG:
				//case SKILL_GUMHWAN:
				case SKILL_TUSOK:
				case SKILL_BIPABU:
#ifdef ENABLE_PVP_BALANCE
				case SKILL_PAERYONG: // Shaman skill
#endif
				case SKILL_YONGBI:		//@fixme474
				case SKILL_NOEJEON:
				case SKILL_GEOMPUNG:
				case SKILL_SANGONG:
				case SKILL_MAHWAN:
				case SKILL_PABEOB:
#ifdef ENABLE_NINETH_SKILL
				case SKILL_ILGWANGPYO: // 177 ninja job 1
				case SKILL_MABEOBAGGWI: // 180 sura job 2
				case SKILL_METEO: // 181 shaman job 1
#endif
				//case SKILL_CURSE:
				{
					m_me->OnMove(true);
					pkVictim->OnMove();

					if (pkVictim->CanBeginFight())
						pkVictim->BeginFight(m_me);

					sys_log(0, "%s - Skill %d -> %s", m_me->GetName(), m_bType, pkVictim->GetName());
					m_me->ComputeSkill(m_bType, pkVictim);
				}
				break;

				case SKILL_CHAIN:
				{
					m_me->OnMove(true);
					pkVictim->OnMove();

					if (pkVictim->CanBeginFight())
						pkVictim->BeginFight(m_me);

					sys_log(0, "%s - Skill %d -> %s", m_me->GetName(), m_bType, pkVictim->GetName());
					m_me->ComputeSkill(m_bType, pkVictim);
					// TODO shudder with multiple people
				}
				break;

				//@fixme474
				/*case SKILL_YONGBI:
				{
					m_me->OnMove(true);
				}
				break;*/

				/*case SKILL_BUDONG:
				{
					m_me->OnMove(true);
					pkVictim->OnMove();

					uint32_t * pdw;
					uint32_t dwEI = AllocEventInfo(sizeof(uint32_t) * 2, &pdw);
					pdw[0] = m_me->GetVID();
					pdw[1] = pkVictim->GetVID();

					event_create(budong_event_func, dwEI, PASSES_PER_SEC(1));
				}
				break;*/

				default:
					sys_log(0, "CFuncShoot: I don't know this type [%d] of range attack.", (int)m_bType);
					break;
			}

			m_bSucceed = TRUE;
		}
};

bool CHARACTER::Shoot(BYTE bType)
{
	sys_log(1, "Shoot %s type %u flyTargets.size %zu", GetName(), bType, m_vec_dwFlyTargets.size());

	if (!CanMove())
	{
		return false;
	}

	CFuncShoot f(this, bType);

	if (m_dwFlyTargetID != 0)
	{
		f(m_dwFlyTargetID);
		m_dwFlyTargetID = 0;
	}

	f = std::for_each(m_vec_dwFlyTargets.begin(), m_vec_dwFlyTargets.end(), f);
	m_vec_dwFlyTargets.clear();

	return f.m_bSucceed;
}

void CHARACTER::FlyTarget(DWORD dwTargetVID, long x, long y, BYTE bHeader)
{
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(dwTargetVID);
	TPacketGCFlyTargeting pack;

	pack.bHeader = (bHeader == HEADER_CG_FLY_TARGETING) ? HEADER_GC_FLY_TARGETING : HEADER_GC_ADD_FLY_TARGETING;
	pack.dwShooterVID = GetVID();

	if (pkVictim)
	{
		pack.dwTargetVID = pkVictim->GetVID();
		pack.x = pkVictim->GetX();
		pack.y = pkVictim->GetY();

		if (bHeader == HEADER_CG_FLY_TARGETING)
			m_dwFlyTargetID = dwTargetVID;
		else
			m_vec_dwFlyTargets.push_back(dwTargetVID);
	}
	else
	{
		pack.dwTargetVID = 0;
		pack.x = x;
		pack.y = y;
	}

	sys_log(1, "FlyTarget %s vid %d x %d y %d", GetName(), pack.dwTargetVID, pack.x, pack.y);
	PacketAround(&pack, sizeof(pack), this);
}

LPCHARACTER CHARACTER::GetNearestVictim(LPCHARACTER pkChr)
{
	if (NULL == pkChr)
		pkChr = this;

	float fMinDist = 99999.0f;
	LPCHARACTER pkVictim = NULL;

	TDamageMap::iterator it = m_map_kDamage.begin();

	// First, filter out people who are not around.
	while (it != m_map_kDamage.end())
	{
		const VID & c_VID = it->first;
		++it;

		LPCHARACTER pAttacker = CHARACTER_MANAGER::instance().Find(c_VID);

		if (!pAttacker)
			continue;

		if (pAttacker->IsAffectFlag(AFF_EUNHYUNG) ||
			pAttacker->IsAffectFlag(AFF_INVISIBILITY) ||
			pAttacker->IsAffectFlag(AFF_REVIVE_INVISIBLE))
			continue;

		float fDist = DISTANCE_APPROX(pAttacker->GetX() - pkChr->GetX(), pAttacker->GetY() - pkChr->GetY());

		if (fDist < fMinDist)
		{
			pkVictim = pAttacker;
			fMinDist = fDist;
		}
	}

	return pkVictim;
}

void CHARACTER::SetVictim(LPCHARACTER pkVictim)
{
#ifdef ENABLE_MOB_FOLLOW_SYSTEM
	if (pkVictim == this)
	{
		sys_err("SetVictim: pkVictim == this");
		return;
	}
#endif

	if (!pkVictim)
	{
#ifdef ENABLE_MOB_FOLLOW_SYSTEM
		LPCHARACTER pkOldVictim = GetVictim();
		if (IsNPC() && pkOldVictim && pkOldVictim->IsPC())
			pkOldVictim->RemoveEnemyNPC((DWORD)GetVID());
#endif

		if (0 != (DWORD)m_kVIDVictim)
			MonsterLog("°ø°? ´ë»óÀ» ÇØÁ¦");

		m_kVIDVictim.Reset();
		battle_end(this);
	}
	else
	{
#ifdef ENABLE_MOB_FOLLOW_SYSTEM
		if (IsNPC())
		{
			LPCHARACTER pkOldVictim = GetVictim();
			if (pkOldVictim && pkOldVictim->IsPC())
				pkOldVictim->RemoveEnemyNPC((DWORD)GetVID());
			if (pkVictim->IsPC())
				pkVictim->AddEnemyNPC((DWORD)GetVID());
		}
#endif

		if (m_kVIDVictim != pkVictim->GetVID())
			MonsterLog("Set attack target: %s", pkVictim->GetName());

		m_kVIDVictim = pkVictim->GetVID();
		m_dwLastVictimSetTime = get_dword_time();
	}
}

LPCHARACTER CHARACTER::GetVictim() const
{
	return CHARACTER_MANAGER::instance().Find(m_kVIDVictim);
}

LPCHARACTER CHARACTER::GetProtege() const // Returns the object to be protected
{
	if (m_pkChrStone)
		return m_pkChrStone;

	if (m_pkParty)
		return m_pkParty->GetLeader();

	return NULL;
}

int CHARACTER::GetAlignment() const
{
	return m_iAlignment;
}

int CHARACTER::GetRealAlignment() const
{
	return m_iRealAlignment;
}

void CHARACTER::ShowAlignment(bool bShow)
{
	if (bShow)
	{
		if (m_iAlignment != m_iRealAlignment)
		{
			m_iAlignment = m_iRealAlignment;
			UpdatePacket();
		}
	}
	else
	{
		if (m_iAlignment != 0)
		{
			m_iAlignment = 0;
			UpdatePacket();
		}
	}
}

void CHARACTER::UpdateAlignment(int iAmount)
{
	bool bShow = false;

#ifdef ENABLE_QUEEN_NETHIS
	if (SnakeLair::CSnk::instance().IsSnakeMap(GetMapIndex()) && iAmount < 0)
		return;
#endif

	if (m_iAlignment == m_iRealAlignment)
		bShow = true;

	int i = m_iAlignment / 10;

	m_iRealAlignment = MINMAX(-200000, m_iRealAlignment + iAmount, 200000);

	if (bShow)
	{
		m_iAlignment = m_iRealAlignment;

		if (i != m_iAlignment / 10)
			UpdatePacket();
	}
}

void CHARACTER::SetKillerMode(bool isOn)
{
	if ((isOn ? ADD_CHARACTER_STATE_KILLER : 0) == IS_SET(m_bAddChrState, ADD_CHARACTER_STATE_KILLER))
		return;

	if (isOn)
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);
	else
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);

	m_iKillerModePulse = thecore_pulse();
	UpdatePacket();
	sys_log(0, "SetKillerMode Update %s[%d]", GetName(), GetPlayerID());
}

bool CHARACTER::IsKillerMode() const
{
	return IS_SET(m_bAddChrState, ADD_CHARACTER_STATE_KILLER);
}

void CHARACTER::UpdateKillerMode()
{
	if (!IsKillerMode())
		return;

	if (thecore_pulse() - m_iKillerModePulse >= PASSES_PER_SEC(30))
		SetKillerMode(false);
}

void CHARACTER::SetPKMode(BYTE bPKMode)
{
	if (bPKMode >= PK_MODE_MAX_NUM)
		return;

	if (m_bPKMode == bPKMode)
		return;

	if (bPKMode == PK_MODE_GUILD && !GetGuild())
		bPKMode = PK_MODE_FREE;

	m_bPKMode = bPKMode;
	UpdatePacket();

	sys_log(0, "PK_MODE: %s %d", GetName(), m_bPKMode);
}

BYTE CHARACTER::GetPKMode() const
{
	return m_bPKMode;
}

struct FuncForgetMyAttacker
{
	LPCHARACTER m_ch;
	FuncForgetMyAttacker(LPCHARACTER ch)
	{
		m_ch = ch;
	}
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER)ent;
			if (ch->IsPC()
#ifdef ENABLE_BOT_PLAYER
				|| ch->IsBotCharacter()
#endif
			)
				return;

			if (ch->m_kVIDVictim == m_ch->GetVID())
				ch->SetVictim(NULL);
		}
	}
};

struct FuncAggregateMonster
{
	LPCHARACTER m_ch;
	FuncAggregateMonster(LPCHARACTER ch)
	{
		m_ch = ch;
	}
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			const LPCHARACTER ch = (LPCHARACTER)ent;
			if (!ch || ch->IsPC()
#ifdef ENABLE_BOT_PLAYER
				|| ch->IsBotCharacter()
#endif
				|| !ch->IsMonster() || ch->GetVictim()
			)
				return;

			if (number(1, 100) <= 50) // Temporarily attracts enemies with a 50% chance
			{
				if (DISTANCE_APPROX(ch->GetX() - m_ch->GetX(), ch->GetY() - m_ch->GetY()) < 5000)
				{
					if (ch->CanBeginFight())
						ch->BeginFight(m_ch);
				}
			}
		}
	}
};

struct FuncAttractRanger
{
	LPCHARACTER m_ch;
	FuncAttractRanger(LPCHARACTER ch)
	{
		m_ch = ch;
	}

	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC()
#ifdef ENABLE_BOT_PLAYER
				|| ch->IsBotCharacter()
#endif
			)
				return;
			if (!ch->IsMonster())
				return;
			if (ch->GetVictim() && ch->GetVictim() != m_ch)
				return;
			if (ch->GetMobAttackRange() > 150)
			{
				int iNewRange = 150; //(int)(ch->GetMobAttackRange() * 0.2);
				if (iNewRange < 150)
					iNewRange = 150;

				ch->AddAffect(AFFECT_BOW_DISTANCE, POINT_BOW_DISTANCE, iNewRange - ch->GetMobAttackRange(), AFF_NONE, 3 * 60, 0, false);
			}
		}
	}
};

#ifdef ENABLE_SUNG_MAHI_TOWER
struct FuncAggregateByMaster
{
	LPCHARACTER m_ch;
	FuncAggregateByMaster(LPCHARACTER ch)
	{
		m_ch = ch;
	}

	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER)ent;
			if (ch->IsPC())
				return;
			if (!ch->IsMonster())
				return;

			ch->SetVictim(NULL);
			ch->RemoveNomove();
			ch->RemoveNoattack();

			if (ch->CanBeginFight())
				ch->BeginFight(m_ch);
		}
	}
};
#endif

struct FuncPullMonster
{
	LPCHARACTER m_ch;
	int m_iLength;
	FuncPullMonster(LPCHARACTER ch, int iLength = 300)
	{
		m_ch = ch;
		m_iLength = iLength;
	}

	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch->IsPC()
#ifdef ENABLE_BOT_PLAYER
				|| ch->IsBotCharacter()
#endif
			)
				return;
			if (!ch->IsMonster())
				return;
			/*
			if (ch->GetVictim() && ch->GetVictim() != m_ch)
				return;
			*/
			float fDist = DISTANCE_APPROX(m_ch->GetX() - ch->GetX(), m_ch->GetY() - ch->GetY());
			if (fDist > 3000 || fDist < 100)
				return;

			float fNewDist = fDist - m_iLength;
			if (fNewDist < 100)
				fNewDist = 100;

			float degree = GetDegreeFromPositionXY(ch->GetX(), ch->GetY(), m_ch->GetX(), m_ch->GetY());
			float fx;
			float fy;

			GetDeltaByDegree(degree, fDist - fNewDist, &fx, &fy);
			long tx = (long)(ch->GetX() + fx);
			long ty = (long)(ch->GetY() + fy);

			ch->Sync(tx, ty);
			ch->Goto(tx, ty);
			ch->CalculateMoveDuration();

			ch->SyncPacket();
		}
	}
};

void CHARACTER::ForgetMyAttacker()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncForgetMyAttacker f(this);
		pSec->ForEachAround(f);
	}
	ReviveInvisible(5);
}

void CHARACTER::AggregateMonster()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncAggregateMonster f(this);
		pSec->ForEachAround(f);
	}
}

void CHARACTER::AttractRanger()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncAttractRanger f(this);
		pSec->ForEachAround(f);
	}
}

#ifdef ENABLE_SUNG_MAHI_TOWER
void CHARACTER::AggregateMonsterByMaster()
{
	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());
	if (pMap)
	{
		FuncAggregateByMaster f(this);
		pMap->for_each(f);
	}
}
#endif

void CHARACTER::PullMonster()
{
	LPSECTREE pSec = GetSectree();
	if (pSec)
	{
		FuncPullMonster f(this);
		pSec->ForEachAround(f);
	}
}

void CHARACTER::UpdateAggrPointEx(LPCHARACTER pAttacker, EDamageType type, int dam, CHARACTER::TBattleInfo & info)
{
	if (!pAttacker)
		return;

	// It goes up higher depending on the specific attack type.
	switch (type)
	{
		case DAMAGE_TYPE_NORMAL_RANGE:
			dam = (int) (dam*1.2f);
			break;

		case DAMAGE_TYPE_RANGE:
			dam = (int) (dam*1.5f);
			break;

		case DAMAGE_TYPE_MAGIC:
			dam = (int) (dam*1.2f);
			break;

		default:
			break;
	}

	// Gives a bonus if the attacker is the current target.
	if (pAttacker == GetVictim())
		dam = (int) (dam * 1.2f);

	info.iAggro += dam;

	if (info.iAggro < 0)
		info.iAggro = 0;

	//sys_log(0, "UpdateAggrPointEx for %s by %s dam %d total %d", GetName(), pAttacker->GetName(), dam, total);
	if (GetParty() && dam > 0 && type != DAMAGE_TYPE_SPECIAL)
	{
		LPPARTY pParty = GetParty();
		if (!pParty)
			return;

		// If you are a leader, you have more influence.
		int iPartyAggroDist = dam;

		if (pParty->GetLeaderPID() == GetVID())
			iPartyAggroDist /= 2;
		else
			iPartyAggroDist /= 3;

		pParty->SendMessage(this, PM_AGGRO_INCREASE, iPartyAggroDist, pAttacker->GetVID());
	}

		ChangeVictimByAggro(info.iAggro, pAttacker);
}

void CHARACTER::UpdateAggrPoint(LPCHARACTER pAttacker, EDamageType type, int dam)
{
	if (!pAttacker)
		return;

	if (IsDead() || IsStun())
		return;

	TDamageMap::iterator it = m_map_kDamage.find(pAttacker->GetVID());

	if (it == m_map_kDamage.end())
	{
		m_map_kDamage.insert(TDamageMap::value_type(pAttacker->GetVID(), TBattleInfo(0, dam)));
		it = m_map_kDamage.find(pAttacker->GetVID());
	}

	UpdateAggrPointEx(pAttacker, type, dam, it->second);
}

void CHARACTER::ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim)
{
	if (get_dword_time() - m_dwLastVictimSetTime < 3000) // you have to wait 3 seconds
		return;

	if (pNewVictim == GetVictim())
	{
		if (m_iMaxAggro < iNewAggro)
		{
			m_iMaxAggro = iNewAggro;
			return;
		}

		// When Aggro is reduced
		TDamageMap::iterator it;
		TDamageMap::iterator itFind = m_map_kDamage.end();

		for (it = m_map_kDamage.begin(); it != m_map_kDamage.end(); ++it)
		{
			if (it->second.iAggro > iNewAggro)
			{
				LPCHARACTER ch = CHARACTER_MANAGER::instance().Find(it->first);

				if (ch && !ch->IsDead() && DISTANCE_APPROX(ch->GetX() - GetX(), ch->GetY() - GetY()) < 5000)
				{
					itFind = it;
					iNewAggro = it->second.iAggro;
				}
			}
		}

		if (itFind != m_map_kDamage.end())
		{
			m_iMaxAggro = iNewAggro;
#ifdef ENABLE_DEFENSAWE_SHIP
			if(!IsHydraMob())
#endif
			{
				SetVictim(CHARACTER_MANAGER::instance().Find(itFind->first));
			}
			m_dwStateDuration = 1;
		}
	}
	else
	{
		if (m_iMaxAggro < iNewAggro)
		{
			m_iMaxAggro = iNewAggro;
#ifdef ENABLE_DEFENSAWE_SHIP
			if(!IsHydraMob())
#endif
			{
				SetVictim(pNewVictim);
			}
			m_dwStateDuration = 1;
		}
	}
}
