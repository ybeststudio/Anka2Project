#include "stdafx.h"

#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "affect.h"
#include "packet.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "battle.h"
#include "guild.h"
#include "utils.h"
#include "locale_service.h"
#include "lua_incl.h"
#include "arena.h"
#include "horsename_manager.h"
#include "item.h"
#include "DragonSoul.h"
#include "../../common/service.h"

#define IS_NO_SAVE_AFFECT(type) \
	((type) == AFFECT_WAR_FLAG || \
	 (type) == AFFECT_REVIVE_INVISIBLE || \
	 ((type) >= AFFECT_PREMIUM_START && (type) <= AFFECT_PREMIUM_END) || \
	 (type) == AFFECT_AUTO_HUNT_AFFECT \
	 || (type) == AFFECT_PASSIVE_RELIC_STONE_DEF \
	 || (type) == AFFECT_PASSIVE_RELIC_DISMOUNT_SPEED \
	)

#define IS_NO_CLEAR_ON_DEATH_AFFECT(type) \
	((type) == AFFECT_BLOCK_CHAT || \
	 ((type) >= 500 && (type) < 600) || \
	 ((type) >= AFFECT_COLLECTIONS0 && (type) <= AFFECT_COLLECT_KOLEKCJONER_EXTRA) || \
	 (type) == AFFECT_AUTO_HUNT || \
	 (type) == AFFECT_DBONE_BRONZE || \
	 (type) == AFFECT_DBONE_SILVER || \
	 (type) == AFFECT_DBONE_GOLD || \
	 (type) == AFFECT_DBONE_DIAMOND || \
	 (type) == AFFECT_DBONE_PLATIN || \
	 (type) == AFFECT_DBONE_EMERALD)

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
bool IsRealTimeAffect(DWORD affectIndex)
{
	switch(affectIndex)
	{
		case AFFECT_AUTO_PICK_UP:
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		case AFFECT_DECORATION:
#endif
			return true;
			break;
	}
	return false;
}
#endif

void SendAffectRemovePacket(LPDESC d, DWORD pid, DWORD type, BYTE point)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	TPacketGCAffectRemove ptoc;
	ptoc.bHeader = HEADER_GC_AFFECT_REMOVE;
	ptoc.dwType = type;
	ptoc.bApplyOn = point;
	d->Packet(&ptoc, sizeof(TPacketGCAffectRemove));

	TPacketGDRemoveAffect ptod;
	ptod.dwPID		= pid;
	ptod.dwType		= type;
	ptod.bApplyOn	= point;

	db_clientdesc->DBPacket(HEADER_GD_REMOVE_AFFECT, 0, &ptod, sizeof(ptod));
}

void SendAffectAddPacket(LPDESC d, CAffect * pkAff)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	TPacketGCAffectAdd ptoc;
	ptoc.bHeader = HEADER_GC_AFFECT_ADD;
	ptoc.elem.dwType = pkAff->dwType;
	ptoc.elem.bApplyOn = pkAff->bApplyOn;
	ptoc.elem.lApplyValue = pkAff->lApplyValue;
	ptoc.elem.dwFlag = pkAff->dwFlag;
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	ptoc.elem.lDuration = IsRealTimeAffect(ptoc.elem.dwType) ? pkAff->lDuration - time(0): pkAff->lDuration;
#else
	ptoc.elem.lDuration = pkAff->lDuration;
#endif
	ptoc.elem.lSPCost = pkAff->lSPCost;
	d->Packet(&ptoc, sizeof(TPacketGCAffectAdd));
}

// Affect
CAffect * CHARACTER::FindAffect(DWORD dwType, BYTE bApply) const
{
	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAffect = *it++;

		if (pkAffect->dwType == dwType && (bApply == APPLY_NONE || bApply == pkAffect->bApplyOn))
			return pkAffect;
	}

	return NULL;
}

EVENTFUNC(affect_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "affect_event> <Factor> NULL pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL)
	{
		return 0;
	}

	if (!ch->UpdateAffect())
		return 0;
	else
		return passes_per_sec;
}

bool CHARACTER::UpdateAffect()
{
#ifdef ENABLE_MAGIC_WEAPON_BUG_FIX
	if (IsAffectFlag(AFF_GWIGUM) && !GetWear(WEAR_WEAPON))
		RemoveAffect(SKILL_GWIGEOM);

	if (IsAffectFlag(AFF_GEOMGYEONG) && !GetWear(WEAR_WEAPON))
		RemoveAffect(SKILL_GEOMKYUNG);
#endif

	if (GetPoint(POINT_HP_RECOVERY) > 0)
	{
		if (GetMaxHP() <= GetHP())
		{
			PointChange(POINT_HP_RECOVERY, -GetPoint(POINT_HP_RECOVERY));
		}
		else
		{
			int iVal = MIN(GetPoint(POINT_HP_RECOVERY), GetMaxHP() * 7 / 100);

			PointChange(POINT_HP, iVal);
			PointChange(POINT_HP_RECOVERY, -iVal);
		}
	}

	if (GetPoint(POINT_SP_RECOVERY) > 0)
	{
		if (GetMaxSP() <= GetSP())
			PointChange(POINT_SP_RECOVERY, -GetPoint(POINT_SP_RECOVERY));
		else
		{
			int iVal = MIN(GetPoint(POINT_SP_RECOVERY), GetMaxSP() * 7 / 100);

			PointChange(POINT_SP, iVal);
			PointChange(POINT_SP_RECOVERY, -iVal);
		}
	}

	if (GetPoint(POINT_HP_RECOVER_CONTINUE) > 0)
	{
		PointChange(POINT_HP, GetPoint(POINT_HP_RECOVER_CONTINUE));
	}

	if (GetPoint(POINT_SP_RECOVER_CONTINUE) > 0)
	{
		PointChange(POINT_SP, GetPoint(POINT_SP_RECOVER_CONTINUE));
	}

	AutoRecoveryItemProcess( AFFECT_AUTO_HP_RECOVERY );
	AutoRecoveryItemProcess( AFFECT_AUTO_SP_RECOVERY );

#ifdef __LEADERSHIP__BONUS__
	if (GetParty())
	{
		RemoveAffect(AFF_LEADERSHIP);
	}
#endif

	if (GetMaxStamina() > GetStamina())
	{
		int iSec = (get_dword_time() - GetStopTime()) / 3000;
		if (iSec)
			PointChange(POINT_STAMINA, GetMaxStamina()/1);
	}

	if (ProcessAffect())
		if (GetPoint(POINT_HP_RECOVERY) == 0 && GetPoint(POINT_SP_RECOVERY) == 0 && GetStamina() == GetMaxStamina())
		{
			m_pkAffectEvent = NULL;
			return false;
		}

	return true;
}

void CHARACTER::StartAffectEvent()
{
	if (m_pkAffectEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();
	info->ch = this;
	m_pkAffectEvent = event_create(affect_event, info, passes_per_sec);
	sys_log(1, "StartAffectEvent %s %p %p", GetName(), this, get_pointer(m_pkAffectEvent));
}

void CHARACTER::ClearAffect(bool bSave, bool bSomeAffect)
{
	TAffectFlag afOld = m_afAffectFlag;
	WORD wMovSpd = GetPoint(POINT_MOV_SPEED);
	WORD wAttSpd = GetPoint(POINT_ATT_SPEED);

	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAff = *it;

		if (bSave)
		{

			if ( IS_NO_CLEAR_ON_DEATH_AFFECT(pkAff->dwType) || IS_NO_SAVE_AFFECT(pkAff->dwType) )
			{
				++it;
				continue;
			}

#ifdef ENABLE_MULTI_FARM_BLOCK
			if(pkAff->dwType == AFFECT_MULTI_FARM_PREMIUM)
			{
				++it;
				continue;
			}
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
			if (IsRealTimeAffect(pkAff->dwType))
			{
				++it;
				continue;
			}
#endif

#ifdef __LEADERSHIP__BONUS__
			if (pkAff->dwType == AFF_LEADERSHIP)
			{
				++it;
				continue;
			}
#endif

#ifdef ENABLE_BATTLE_PASS
			if (pkAff->dwType == AFFECT_BATTLE_BONUS1 || pkAff->dwType == AFFECT_BATTLE_BONUS2 || pkAff->dwType == AFFECT_BATTLE_BONUS3)
			{
				++it;
				continue;
			}
#endif

#ifdef ENABLE_EQUIPMENT_HAND_EFFECT
			if (pkAff->dwType == AFFECT_DBONE_BRONZE || pkAff->dwType == AFFECT_DBONE_SILVER || pkAff->dwType == AFFECT_DBONE_GOLD || pkAff->dwType == AFFECT_DBONE_DIAMOND || pkAff->dwType == AFFECT_DBONE_PLATIN || pkAff->dwType == AFFECT_DBONE_EMERALD)
			{
				++it;
				continue;
			}
#endif

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
			if (pkAff->dwType == AFFECT_COLLECTIONS0 || pkAff->dwType == AFFECT_COLLECTIONS1 || pkAff->dwType == AFFECT_COLLECTIONS2 || pkAff->dwType == AFFECT_COLLECTIONS3 || pkAff->dwType == AFFECT_COLLECTIONS4 || pkAff->dwType == AFFECT_COLLECTIONS5 || pkAff->dwType == AFFECT_COLLECTIONS6 || pkAff->dwType == AFFECT_COLLECT_POLOWANIE)
			{
				++it;
				continue;
			}
#endif

#ifdef ENABLE_NINETH_SKILL
			if (pkAff->dwType == SKILL_CHEONUN)
			{
				++it;
				continue;
			}
#endif

			if (bSomeAffect)
			{
				switch (pkAff->dwType)
				{
					case (SKILL_JEONGWI):
					case (SKILL_GEOMKYUNG):
					case (SKILL_CHUNKEON):
					case (SKILL_GWIGEOM):
					case (SKILL_TERROR):
					case (SKILL_JUMAGAP):
					case (SKILL_HOSIN):
					case (SKILL_REFLECT):
					case (SKILL_GICHEON):
					case (SKILL_KWAESOK):
					case (SKILL_JEUNGRYEOK):
					//case (SKILL_JEOKRANG):
					//case (SKILL_CHEONGRANG):
					{
						++it;
						continue;
					}
				}
			}

			if (IsPC())
			{
				SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
			}
		}

		ComputeAffect(pkAff, false);

		it = m_list_pkAffect.erase(it);
		CAffect::Release(pkAff);
	}

	if (afOld != m_afAffectFlag ||
		wMovSpd != GetPoint(POINT_MOV_SPEED) ||
		wAttSpd != GetPoint(POINT_ATT_SPEED))
		UpdatePacket();

	CheckMaximumPoints();

	if (m_list_pkAffect.empty())
		event_cancel(&m_pkAffectEvent);
}

int CHARACTER::ProcessAffect()
{
	bool	bDiff	= false;
	CAffect	*pkAff	= NULL;

	for (int i = 0; i <= PREMIUM_MAX_NUM; ++i)
	{
		int aff_idx = i + AFFECT_PREMIUM_START;

		pkAff = FindAffect(aff_idx);

		if (!pkAff)
			continue;

		int remain = GetPremiumRemainSeconds(i);

		if (remain < 0)
		{
			RemoveAffect(aff_idx);
			bDiff = true;
		}
		else
			pkAff->lDuration = remain + 1;
	}

	////////// HAIR_AFFECT
	pkAff = FindAffect(AFFECT_HAIR);
	if (pkAff)
	{
		// IF HAIR_LIMIT_TIME() < CURRENT_TIME()
		if ( this->GetQuestFlag("hair.limit_time") < get_global_time())
		{
			// SET HAIR NORMAL
			this->SetPart(PART_HAIR, 0);
			// REMOVE HAIR AFFECT
			RemoveAffect(AFFECT_HAIR);
		}
		else
		{
			// INCREASE AFFECT DURATION
			++(pkAff->lDuration);
		}
	}
	////////// HAIR_AFFECT

	CHorseNameManager::instance().Validate(this);

	TAffectFlag afOld = m_afAffectFlag;
	long lMovSpd = GetPoint(POINT_MOV_SPEED);
	long lAttSpd = GetPoint(POINT_ATT_SPEED);

	itertype(m_list_pkAffect) it;

	it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		pkAff = *it;

		bool bEnd = false;

		if (pkAff->dwType >= GUILD_SKILL_START && pkAff->dwType <= GUILD_SKILL_END)
		{
			if (!GetGuild() || !GetGuild()->UnderAnyWar())
				bEnd = true;
		}

		if (pkAff->lSPCost > 0)
		{
			if (GetSP() < pkAff->lSPCost)
				bEnd = true;
			else
				PointChange(POINT_SP, -pkAff->lSPCost);
		}

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
		if(IsRealTimeAffect(pkAff->dwType))
		{
			if(time(0) > pkAff->lDuration)
				bEnd = true;
		}
		else
		{
#endif
		if ( --pkAff->lDuration <= 0 )
		{
			bEnd = true;
		}
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
		}
#endif

		if (bEnd)
		{
			it = m_list_pkAffect.erase(it);
			ComputeAffect(pkAff, false);
			bDiff = true;
			if (IsPC())
			{
				SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
			}

			CAffect::Release(pkAff);

			continue;
		}

		++it;
	}

	if (bDiff)
	{
		if (afOld != m_afAffectFlag ||
			lMovSpd != GetPoint(POINT_MOV_SPEED) ||
			lAttSpd != GetPoint(POINT_ATT_SPEED))
		{
			UpdatePacket();
		}

		CheckMaximumPoints();
	}

	if (m_list_pkAffect.empty())
		return true;

	return false;
}

void CHARACTER::SaveAffect()
{
	TPacketGDAddAffect p;

	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAff = *it++;

		if (IS_NO_SAVE_AFFECT(pkAff->dwType))
			continue;

#ifdef ENABLE_NINETH_SKILL
		if (pkAff->dwType == SKILL_CHEONUN)
			continue;
#endif

		// sys_log(1, "AFFECT_SAVE: %u %u %d %d", pkAff->dwType, pkAff->bApplyOn, pkAff->lApplyValue, pkAff->lDuration);

		p.dwPID				= GetPlayerID();
		p.elem.dwType		= pkAff->dwType;
		p.elem.bApplyOn		= pkAff->bApplyOn;
		p.elem.lApplyValue	= pkAff->lApplyValue;
		p.elem.dwFlag		= pkAff->dwFlag;
		p.elem.lDuration	= pkAff->lDuration;
		p.elem.lSPCost		= pkAff->lSPCost;

		db_clientdesc->DBPacket(HEADER_GD_ADD_AFFECT, 0, &p, sizeof(p));
	}
}

EVENTINFO(load_affect_login_event_info)
{
	DWORD pid;
	DWORD count;
	char* data;

	load_affect_login_event_info()
	: pid( 0 )
	, count( 0 )
	, data( 0 )
	{
	}
};

EVENTFUNC(load_affect_login_event)
{
	load_affect_login_event_info* info = dynamic_cast<load_affect_login_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "load_affect_login_event_info> <Factor> NULL pointer" );
		return 0;
	}

	DWORD dwPID = info->pid;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(dwPID);

	if (!ch)
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}

	LPDESC d = ch->GetDesc();

	if (!d)
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}

	if (d->IsPhase(PHASE_HANDSHAKE) ||
		d->IsPhase(PHASE_LOGIN) ||
		d->IsPhase(PHASE_SELECT) ||
		d->IsPhase(PHASE_DEAD) ||
		d->IsPhase(PHASE_LOADING))
	{
		return PASSES_PER_SEC(1);
	}
	else if (d->IsPhase(PHASE_CLOSE))
	{
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
	else if (d->IsPhase(PHASE_GAME))
	{
		// Fix
		if (ch->IsLoadedAffect())
		{
			M2_DELETE_ARRAY(info->data);
			return 0;
		}

		sys_log(1, "Affect Load by Event");
		ch->LoadAffect(info->count, (TPacketAffectElement*)info->data);
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
	else
	{
		sys_err("input_db.cpp:quest_login_event INVALID PHASE pid %d", ch->GetPlayerID());
		M2_DELETE_ARRAY(info->data);
		return 0;
	}
}

void CHARACTER::LoadAffect(DWORD dwCount, TPacketAffectElement * pElements)
{
	m_bIsLoadedAffect = false;

	if (!GetDesc()->IsPhase(PHASE_GAME))
	{
		if (test_server)
			sys_log(0, "LOAD_AFFECT: Creating Event", GetName(), dwCount);

		load_affect_login_event_info* info = AllocEventInfo<load_affect_login_event_info>();

		info->pid = GetPlayerID();
		info->count = dwCount;
		info->data = M2_NEW char[sizeof(TPacketAffectElement) * dwCount];
		thecore_memcpy(info->data, pElements, sizeof(TPacketAffectElement) * dwCount);

		event_create(load_affect_login_event, info, PASSES_PER_SEC(1));

		return;
	}

	ClearAffect(true);

	if (test_server)
		sys_log(0, "LOAD_AFFECT: %s count %d", GetName(), dwCount);

	TAffectFlag afOld = m_afAffectFlag;

	long lMovSpd = GetPoint(POINT_MOV_SPEED);
	long lAttSpd = GetPoint(POINT_ATT_SPEED);

	for (DWORD i = 0; i < dwCount; ++i, ++pElements)
	{
		if (pElements->dwType == SKILL_MUYEONG)
			continue;

#ifdef ENABLE_NINETH_SKILL
		if (pElements->dwType == SKILL_CHEONUN)
			continue;
#endif

		if (AFFECT_AUTO_HP_RECOVERY == pElements->dwType || AFFECT_AUTO_SP_RECOVERY == pElements->dwType)
		{
			LPITEM item = FindItemByID(pElements->dwFlag);
			if (!item)
			{
				continue;
			}

			item->Lock(true);
		}

		if (pElements->bApplyOn >= POINT_MAX_NUM)
		{
			sys_err("invalid affect data %s ApplyOn %u ApplyValue %d",
				GetName(), pElements->bApplyOn, pElements->lApplyValue);
			continue;
		}


		CAffect* pkAff = CAffect::Acquire();
		m_list_pkAffect.push_back(pkAff);

		pkAff->dwType		= pElements->dwType;
		pkAff->bApplyOn		= pElements->bApplyOn;
		pkAff->lApplyValue	= pElements->lApplyValue;
		pkAff->dwFlag		= pElements->dwFlag;
		pkAff->lDuration	= pElements->lDuration;
		pkAff->lSPCost		= pElements->lSPCost;

		SendAffectAddPacket(GetDesc(), pkAff);

		ComputeAffect(pkAff, true);
	}

#ifdef ENABLE_MOUNT_SYSTEM
	RemoveAffect(AFFECT_MOUNT);
	RemoveAffect(AFFECT_MOUNT_BONUS);
#endif

	if ( CArenaManager::instance().IsArenaMap(GetMapIndex()) == true )
	{
		RemoveGoodAffect();
	}

#ifdef __DOJANG_SRC_FUNCTIONS__
	if (GetMapIndex() == DOJANG_MAPINDEX)
	{
		RemoveAffect(SKILL_JEONGWI);
		RemoveAffect(SKILL_GEOMKYUNG);
		RemoveAffect(SKILL_CHUNKEON);
		RemoveAffect(SKILL_EUNHYUNG);
		RemoveAffect(SKILL_GYEONGGONG);
		RemoveAffect(SKILL_GWIGEOM);
		RemoveAffect(SKILL_TERROR);
		RemoveAffect(SKILL_JUMAGAP);
		RemoveAffect(SKILL_MANASHILED);
		RemoveAffect(SKILL_HOSIN);
		RemoveAffect(SKILL_REFLECT);
		RemoveAffect(SKILL_KWAESOK);
		RemoveAffect(SKILL_JEUNGRYEOK);
		RemoveAffect(SKILL_GICHEON);
	}
#endif

	if (afOld != m_afAffectFlag || lMovSpd != GetPoint(POINT_MOV_SPEED) || lAttSpd != GetPoint(POINT_ATT_SPEED))
	{
		UpdatePacket();
	}

	StartAffectEvent();

	m_bIsLoadedAffect = true;

	ComputePoints(); // @fixme156

	// Loading and resetting Dragon Soul Stone settings
	DragonSoul_Initialize();

	// @fixme118 (regain affect hp/mp)
	if (!IsDead())
	{
		PointChange(POINT_HP, GetMaxHP() - GetHP());
		PointChange(POINT_SP, GetMaxSP() - GetSP());
	}
}

bool CHARACTER::AddAffect(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube
#ifdef ENABLE_NINETH_SKILL
	, uint8_t bShieldDuration
#endif
)	//@fixme532
{
	// CHAT_BLOCK
	if (dwType == AFFECT_BLOCK_CHAT && lDuration > 1)
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your chat has been temporary blocked for spam."));
	// END_OF_CHAT_BLOCK

	if (IsDead())	//@fixme000
		return false;

	if (lDuration == 0)
	{
		sys_err("Character::AddAffect lDuration == 0 type %d", lDuration, dwType);
		lDuration = 1;
	}

	switch (dwType)	//@fixme406
	{
		case SKILL_HOSIN:		// Segen
		case SKILL_REFLECT:		// Reflektieren
		case SKILL_GICHEON:		// Hilfe des Drachen
		case SKILL_JEONGEOP:	// Kurieren
		case SKILL_KWAESOK:		// Schnelligkeit
		case SKILL_JEUNGRYEOK:	// Angriff+
#ifdef ENABLE_WOLFMAN_CHARACTER
		case SKILL_CHEONGRANG:	// Indigowolfseele
#endif
#ifdef ENABLE_NINETH_SKILL
		case SKILL_CHEONUN:		// Ätherschild
#endif
		{
			const CAffect * pkAffect = FindAffect(dwType, bApplyOn);	// same bonus only: Swiftness also adds CASTING_SPEED under the same type
			if (!pkAffect)
				break;

			if (lApplyValue < pkAffect->lApplyValue)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A stronger %s effect is already active (%ld < %ld)."), CSkillManager::instance().Get(dwType)->szName, lApplyValue, pkAffect->lApplyValue);
				return false;
			}
		}
		break;

		default:
			break;
	}	//@END_fixme406

#ifdef ENABLE_IGNORE_LOW_POWER_BUFF
	switch (dwType)
	{
	case SKILL_HOSIN:
	case SKILL_REFLECT:
	case SKILL_GICHEON:
	case SKILL_JEONGEOP:
	case SKILL_KWAESOK:
	case SKILL_JEUNGRYEOK:
	{
		const CAffect* pkAffect = FindAffect(dwType, bApplyOn);	// same bonus only: Swiftness also adds CASTING_SPEED under the same type
		if (!pkAffect)
			break;

		if (lApplyValue < pkAffect->lApplyValue)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("(%s) Becerisi engellendi. (%ld%%) deðeri kullanmýþ olduðunuz (%ld%%) deðerden daha düþük."), CSkillManager::instance().Get(dwType)->szName, lApplyValue, pkAffect->lApplyValue);
			return false;
		}
	}
	break;

	default:
		break;
	}
#endif

	CAffect* pkAff = NULL;

	if (IsCube)
	{
		pkAff = FindAffect(dwType, bApplyOn);
	}
	else
	{
		pkAff = FindAffect(dwType);
	}

	if (dwFlag == AFF_STUN)
	{
		if (m_posDest.x != GetX() || m_posDest.y != GetY())
		{
			m_posDest.x = m_posStart.x = GetX();
			m_posDest.y = m_posStart.y = GetY();
			battle_end(this);

			SyncPacket();
		}
	}

	// Overwriting of effects that already exist
	if (pkAff && bOverride)
	{
		ComputeAffect(pkAff, false); // Once the effect is removed

		if (GetDesc())
			SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
	}
	else
	{
		// Add new effect
		// NOTE: Therefore, multiple effects can be attached to the same type.
		pkAff = CAffect::Acquire();
		m_list_pkAffect.push_back(pkAff);
	}

	sys_log(1, "AddAffect %s type %d apply %d %d flag %u duration %d", GetName(), dwType, bApplyOn, lApplyValue, dwFlag, lDuration);
	sys_log(0, "AddAffect %s type %d apply %d %d flag %u duration %d", GetName(), dwType, bApplyOn, lApplyValue, dwFlag, lDuration);

	pkAff->dwType		= dwType;
	pkAff->bApplyOn		= bApplyOn;
	pkAff->lApplyValue	= lApplyValue;
	pkAff->dwFlag		= dwFlag;
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	pkAff->lDuration = IsRealTimeAffect(dwType) ? lDuration + time(0) : lDuration;
#else
	pkAff->lDuration	= lDuration;
#endif
	pkAff->lSPCost		= lSPCost;
#ifdef ENABLE_NINETH_SKILL
	pkAff->bShieldDuration = bShieldDuration;
#endif

	WORD wMovSpd = GetPoint(POINT_MOV_SPEED);
	WORD wAttSpd = GetPoint(POINT_ATT_SPEED);

	ComputeAffect(pkAff, true);

	if (pkAff->dwFlag || wMovSpd != GetPoint(POINT_MOV_SPEED) || wAttSpd != GetPoint(POINT_ATT_SPEED))
	{
		UpdatePacket();
	}

	StartAffectEvent();

	if (IsPC())
	{
		SendAffectAddPacket(GetDesc(), pkAff);

		if (IS_NO_SAVE_AFFECT(pkAff->dwType))
			return true;

#ifdef ENABLE_NINETH_SKILL
		if (pkAff->dwType == SKILL_CHEONUN)
			return true;
#endif

		TPacketGDAddAffect p;
		p.dwPID			= GetPlayerID();
		p.elem.dwType		= pkAff->dwType;
		p.elem.bApplyOn		= pkAff->bApplyOn;
		p.elem.lApplyValue	= pkAff->lApplyValue;
		p.elem.dwFlag		= pkAff->dwFlag;
		p.elem.lDuration	= pkAff->lDuration;
		p.elem.lSPCost		= pkAff->lSPCost;

		db_clientdesc->DBPacket(HEADER_GD_ADD_AFFECT, 0, &p, sizeof(p));
	}

	return true;
}

void CHARACTER::RefreshAffect()
{
	itertype(m_list_pkAffect) it = m_list_pkAffect.begin();

	while (it != m_list_pkAffect.end())
	{
		CAffect * pkAff = *it++;
		ComputeAffect(pkAff, true);
	}
}

void CHARACTER::ComputeAffect(CAffect * pkAff, bool bAdd, bool bTemp)
{
	if (bAdd && pkAff->dwType >= GUILD_SKILL_START && pkAff->dwType <= GUILD_SKILL_END)	
	{
		if (!GetGuild())
			return;

		if (!GetGuild()->UnderAnyWar())
			return;

		if (!GetWarMap()) // @fixme231
			return;
	}

	if (pkAff->dwFlag)
	{
		if (!bAdd)
		{
			m_afAffectFlag.Reset(pkAff->dwFlag);
		}
		else
		{
			m_afAffectFlag.Set(pkAff->dwFlag);
		}
	}

#ifdef __LEADERSHIP__BONUS__
	if (pkAff->dwType == AFF_LEADERSHIP)
	{
		if (!bAdd)
		{
			PointChange(POINT_PARTY_ATTACKER_BONUS, -GetPoint(POINT_PARTY_ATTACKER_BONUS));
			PointChange(POINT_PARTY_TANKER_BONUS, -GetPoint(POINT_PARTY_TANKER_BONUS));
			PointChange(POINT_PARTY_BUFFER_BONUS, -GetPoint(POINT_PARTY_BUFFER_BONUS));
			PointChange(POINT_PARTY_SKILL_MASTER_BONUS, -GetPoint(POINT_PARTY_SKILL_MASTER_BONUS));
			PointChange(POINT_PARTY_DEFENDER_BONUS, -GetPoint(POINT_PARTY_DEFENDER_BONUS));
			PointChange(POINT_PARTY_HASTE_BONUS, -GetPoint(POINT_PARTY_HASTE_BONUS));

			ComputeBattlePoints();
			
			SetPoint(POINT_PARTY_ATTACKER_BONUS, 0);
			SetPoint(POINT_PARTY_TANKER_BONUS, 0);
			SetPoint(POINT_PARTY_BUFFER_BONUS, 0);
			SetPoint(POINT_PARTY_SKILL_MASTER_BONUS, 0);
			SetPoint(POINT_PARTY_DEFENDER_BONUS, 0);
			SetPoint(POINT_PARTY_HASTE_BONUS, 0);
		
			ComputePoints();
		}
		else
		{
			if (GetPoint(pkAff->bApplyOn) <= 0)
				PointChange(pkAff->bApplyOn, pkAff->lApplyValue);
		}
		
		return;
	}
#endif

	if (bAdd)
		PointChange(pkAff->bApplyOn, pkAff->lApplyValue);
	else
		PointChange(pkAff->bApplyOn, -pkAff->lApplyValue);

	if (pkAff->dwType == SKILL_MUYEONG)
	{
		if (bAdd)
			StartMuyeongEvent();
		else
			StopMuyeongEvent();
	}

#ifdef ENABLE_PVP_BALANCE
	if (pkAff->dwType == SKILL_GYEONGGONG)
	{
		if (bAdd)
			StartGyeongGongEvent();
		else
			StopGyeongGongEvent();
	}
#endif

#ifdef ENABLE_NINETH_SKILL
	if (pkAff->dwType == SKILL_CHEONUN)
	{
		if (bAdd)
			StartCheonunEvent(static_cast<uint8_t>(pkAff->lApplyValue), pkAff->bShieldDuration);
		else
			StopCheonunEvent();
	}
#endif
}

bool CHARACTER::RemoveAffect(CAffect* pkAff, bool single)	//@fixme433
{
	if (!pkAff)
	{
		return false;
	}

	// AFFECT_BUF_FIX
	m_list_pkAffect.remove(pkAff);
	// END_OF_AFFECT_BUF_FIX

	ComputeAffect(pkAff, false);

	/*
	* Fix white flag bug.
	* The white flag bug occurs when attacking immediately after casting buff skill -> transforming -> using white flag (AFFECT_REVIVE_INVISIBLE).
	* The reason is that at the time of casting the disguise, only the disguise effect is applied, ignoring the buff skill effect,
	* If you attack immediately after using the white flag, RemoveAffect is called, and it becomes a disguise effect + buff skill effect while doing ComputePoints.
	* If you are disguised in ComputePoints, you can make the buff skill effect not work,
	* ComputePoints are widely used, so I'm reluctant to make big changes (it's hard to know what side effects will happen).
	* Therefore, it is modified only when AFFECT_REVIVE_INVISIBLE is deleted with RemoveAffect.
	* If the time runs out and the white flag is released, no bug occurs, so do the same.
	* (If you look at ProcessAffect, if the time runs out and the Affect is deleted, ComputePoints is not called.)
	*/
	if (single) {	//@fixme433
		if (AFFECT_REVIVE_INVISIBLE != pkAff->dwType)
			ComputePoints();
		else  // @fixme110
			UpdatePacket();
	}

	CheckMaximumPoints();

	if (test_server)
	{
		sys_log(0, "AFFECT_REMOVE: %s (flag %u apply: %u)", GetName(), pkAff->dwFlag, pkAff->bApplyOn);
	}

	if (IsPC())
	{
		SendAffectRemovePacket(GetDesc(), GetPlayerID(), pkAff->dwType, pkAff->bApplyOn);
	}

	CAffect::Release(pkAff);
	return true;
}

bool CHARACTER::RemoveAffect(DWORD dwType)
{
	// CHAT_BLOCK
	if (dwType == AFFECT_BLOCK_CHAT)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your chat block has been lifted."));
	}
	// END_OF_CHAT_BLOCK

	bool flag = false;

	CAffect * pkAff;

	while ((pkAff = FindAffect(dwType)))
	{
		RemoveAffect(pkAff);
		flag = true;
	}

	return flag;
}

bool CHARACTER::IsAffectFlag(DWORD dwAff) const
{
	return m_afAffectFlag.IsSet(dwAff);
}

void CHARACTER::RemoveGoodAffect()
{
	RemoveAffect(AFFECT_MOV_SPEED);
	RemoveAffect(AFFECT_ATT_SPEED);
	RemoveAffect(AFFECT_STR);
	RemoveAffect(AFFECT_DEX);
	RemoveAffect(AFFECT_INT);
	RemoveAffect(AFFECT_CON);
	RemoveAffect(AFFECT_CHINA_FIREWORK);

	RemoveAffect(SKILL_JEONGWI);
	RemoveAffect(SKILL_GEOMKYUNG);
	RemoveAffect(SKILL_CHUNKEON);
	RemoveAffect(SKILL_EUNHYUNG);
	RemoveAffect(SKILL_GYEONGGONG);
	RemoveAffect(SKILL_GWIGEOM);
	RemoveAffect(SKILL_TERROR);
	RemoveAffect(SKILL_JUMAGAP);
	RemoveAffect(SKILL_MANASHILED);
	RemoveAffect(SKILL_HOSIN);
	RemoveAffect(SKILL_REFLECT);
	RemoveAffect(SKILL_KWAESOK);
	RemoveAffect(SKILL_JEUNGRYEOK);
	RemoveAffect(SKILL_GICHEON);
#ifdef ENABLE_NINETH_SKILL
	RemoveAffect(SKILL_CHEONUN);
#endif
}

bool CHARACTER::IsGoodAffect(BYTE bAffectType) const
{
	switch (bAffectType)
	{
		case (AFFECT_MOV_SPEED):
		case (AFFECT_ATT_SPEED):
		case (AFFECT_STR):
		case (AFFECT_DEX):
		case (AFFECT_INT):
		case (AFFECT_CON):
		case (AFFECT_CHINA_FIREWORK):

		case (SKILL_JEONGWI):
		case (SKILL_GEOMKYUNG):
		case (SKILL_CHUNKEON):
		case (SKILL_EUNHYUNG):
		case (SKILL_GYEONGGONG):
		case (SKILL_GWIGEOM):
		case (SKILL_TERROR):
		case (SKILL_JUMAGAP):
		case (SKILL_MANASHILED):
		case (SKILL_HOSIN):
		case (SKILL_REFLECT):
		case (SKILL_KWAESOK):
		case (SKILL_JEUNGRYEOK):
		case (SKILL_GICHEON):
#ifdef ENABLE_WOLFMAN_CHARACTER
		case (SKILL_JEOKRANG):
		case (SKILL_CHEONGRANG):
#endif
#ifdef ENABLE_NINETH_SKILL
		case (SKILL_CHEONUN):
#endif
			return true;
	}
	return false;
}

void CHARACTER::RemoveBadAffect()
{
	sys_log(0, "RemoveBadAffect %s", GetName());
	RemovePoison();
#ifdef ENABLE_WOLFMAN_CHARACTER
	RemoveBleeding();
#endif
	RemoveFire();

	RemoveAffect(AFFECT_STUN);
	RemoveAffect(AFFECT_SLOW);
	RemoveAffect(SKILL_TUSOK);
	//RemoveAffect(SKILL_CURSE);
	//RemoveAffect(SKILL_PABUP);
	//RemoveAffect(AFFECT_FAINT);
	//RemoveAffect(AFFECT_WEB);
	//RemoveAffect(AFFECT_SLEEP);
	//RemoveAffect(AFFECT_CURSE);
	//RemoveAffect(AFFECT_PARALYZE);
	//RemoveAffect(SKILL_BUDONG);
}
