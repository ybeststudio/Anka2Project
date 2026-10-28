#include "stdafx.h"
#include "growth_pet.h"
#include "utils.h"
#include "vector.h"
#include "char.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "packet.h"
#include "item_manager.h"
#include "item.h"
#include "banword.h"
#include "desc.h"
#include "desc_client.h"
#include "typedef.h"
#include "constants.h"
#include "db.h"

#ifdef ENABLE_GROWTH_PET_SYSTEM

extern int (*check_name) (const char* str);
extern bool	battle_is_attackable(LPCHARACTER ch, LPCHARACTER victim);
extern int passes_per_sec;

EVENTINFO(growthpet_update_event_info)
{
	LPGROWTH_PET pet;
};

EVENTFUNC(growthpet_update_event)
{
	growthpet_update_event_info* info = dynamic_cast<growthpet_update_event_info*>(event->info);
	if (!info)
	{
		sys_err("growthpet_update_event> <Factor> Null pointer");
		return 0;
	}

	LPGROWTH_PET pPet = info->pet;

	if (!pPet)
		return 0;

	pPet->Update();

	return PASSES_PER_SEC(1) / 2;
}

struct FuncGyeonggongBoom
{
	WORD m_wDamage;
	DWORD m_dwX;
	DWORD m_dwY;
	LPCHARACTER m_pAttacker;

	FuncGyeonggongBoom(WORD wDamage, DWORD dwX, DWORD dwY, LPCHARACTER pAttacker)
		:
		m_wDamage(wDamage), m_dwX(dwX), m_dwY(dwY), m_pAttacker(pAttacker)
	{
	}

	void operator () (LPENTITY ent)
	{
		if (!ent->IsType(ENTITY_CHARACTER))
			return;

		LPCHARACTER pVictim = (LPCHARACTER)ent;

		if (pVictim->IsPC() || pVictim->IsMonster() || pVictim->IsStone())
		{
			if (DISTANCE_APPROX(m_dwX - pVictim->GetX(), m_dwY - pVictim->GetY()) > 200)
				return;

			if (m_pAttacker)
			{
				if (pVictim == m_pAttacker)
					return;

				if (battle_is_attackable(m_pAttacker, pVictim))
					return;

				pVictim->Damage(m_pAttacker, m_wDamage, DAMAGE_TYPE_SPECIAL);
			}
		}
	}
};

EVENTINFO(gyeonggong_boom_event_info)
{
	LPGROWTH_PET pPet;
	DWORD dwX;
	DWORD dwY;
	WORD wDamage;
};

EVENTFUNC(gyeonggong_boom_event)
{
	gyeonggong_boom_event_info* info = dynamic_cast<gyeonggong_boom_event_info*>(event->info);
	if (!info)
	{
		sys_err("gyeonggong_boom_event_info> <Factor> Null pointer");
		return 0;
	}

	if (!info->pPet || !info->pPet->GetGrowthPet())
		return 0;

	FuncGyeonggongBoom f(info->wDamage, info->dwX, info->dwY, info->pPet->GetOwner());

	if (info->pPet->GetGrowthPet()->GetSectree())
		info->pPet->GetGrowthPet()->GetSectree()->ForEachAround(f);

	return 0;
}

EVENTINFO(feather_skill_event_info)
{
	LPGROWTH_PET pPet;
	CPoly* pSkillPoly;
	BYTE bSkillLevel;
	DWORD dwDuration;
};

EVENTFUNC(feather_skill_event)
{
	feather_skill_event_info* info = dynamic_cast<feather_skill_event_info*>(event->info);
	if (!info)
	{
		sys_err("feather_skill_event_info> <Factor> Null pointer");
		return 0;
	}

	if (!info->pPet)
		return 0;

	if (info->dwDuration < time(0))
	{
		info->pPet->CancelFeatherWalk();
		return 0;
	}

	if (info->pPet->GetGrowthPet())
	{
		info->pPet->GetGrowthPet()->EffectPacket(SE_GYEONGGONG_BOOM);

		const DWORD dwPetLevel = info->pPet->GetPetPoint(POINT_UPBRINGING_PET_LEVEL) - 81;
		info->pSkillPoly->SetVar("lv", dwPetLevel);
		info->pSkillPoly->SetVar("k", info->bSkillLevel);
		WORD wSkillValue = (WORD)info->pSkillPoly->Eval();
		wSkillValue = MIN(number(wSkillValue, wSkillValue * 2), 1400);

		gyeonggong_boom_event_info* info2 = AllocEventInfo<gyeonggong_boom_event_info>();
		info2->pPet = info->pPet;
		info2->dwX = info->pPet->GetGrowthPet()->GetX();
		info2->dwY = info->pPet->GetGrowthPet()->GetY();
		info2->wDamage = wSkillValue;

		event_create(gyeonggong_boom_event, info2, PASSES_PER_SEC(1) * 0.65);
	}

	return PASSES_PER_SEC(1);
} 

EVENTINFO(invincible_skill_end_event_info)
{
	LPCHARACTER ch;
};

EVENTFUNC(invincible_skill_end_event)
{
	invincible_skill_end_event_info* info = dynamic_cast<invincible_skill_end_event_info*>(event->info);
	if (!info)
	{
		sys_err("invincible_skill_end_event> <Factor> Null pointer");
		return 0;
	}

	if (!info->ch)
		return 0;

	info->ch->SetInvincible(false);

	return 0;
}

CGrowthPet::CGrowthPet(LPCHARACTER pOwner)
{
	m_pGrowthPet = NULL;
	m_pOwner = pOwner;

	m_dwID = 0;
	m_dwVnum = 0;
	m_bState = STATE_NONE;
	m_strName = "";
	m_bSize = 0;
	m_bType = 0;
	m_bLevelStep = 0;
	memset(m_dwPoints, 0, sizeof(m_dwPoints));
	memset(m_aSkill, 0, sizeof(m_aSkill));
	m_dwHPApply = 0;
	m_dwSPApply = 0;
	m_dwDefApply = 0;
	m_dwAgeApply = 0;
	m_bAutoSkillFlag = 0;

	m_pUpdateEvent = NULL;
	m_pFeatherSkillEvent = NULL;
	m_pSummonItem = NULL;

	m_bMobExpMsg = false;
	m_bItemExpMsg = false;
}

CGrowthPet::~CGrowthPet()
{
	if (m_pGrowthPet)
		M2_DESTROY_CHARACTER(m_pGrowthPet);

	m_pGrowthPet = 0;
	m_pOwner = 0;

	event_cancel(&m_pUpdateEvent);
	event_cancel(&m_pFeatherSkillEvent);
}

LPGROWTH_PET CGrowthPet::Summon(LPITEM pSummonItem, bool bIsResummon)
{
	if (GetPetPoint(POINT_UPBRINGING_DURATION) < time(0))
	{
		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 743, "%s", m_strName.c_str());
		return NULL;
	}

	if (GetPetPoint(POINT_UPBRINGING_PET_LEVEL) > m_pOwner->GetLevel())
	{
		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 744, "");
		return NULL;
	}

	if (m_pOwner->GetExchange() || m_pOwner->IsOpenSafebox() || m_pOwner->GetMyShop() || m_pOwner->GetShopOwner() || m_pOwner->IsCubeOpen())
	{
		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 745, "");
		return NULL;
	}

	if (m_pOwner->m_pkFishingEvent)
		return NULL;

	if (m_pGrowthPet)
	{
		M2_DESTROY_CHARACTER(m_pGrowthPet);
		event_cancel(&m_pUpdateEvent);
	}

	long x = m_pOwner->GetX();
	long y = m_pOwner->GetY();
	long z = m_pOwner->GetZ();

	x += number(-100, 100);
	y += number(-100, 100);

	const DWORD dwPetEvol = GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL);
	const DWORD dwPetVnum = dwPetEvol < PET_EVOLUTION_4 ? pSummonItem->GetValue(0) : pSummonItem->GetValue(3);

	m_pGrowthPet = CHARACTER_MANAGER::Instance().SpawnMob(
		dwPetVnum,
		m_pOwner->GetMapIndex(),
		x, y, z,
		false, (int)(m_pOwner->GetRotation() + 180), false);

	if (!m_pGrowthPet)
	{
		sys_err("Failed to summon growth pet vnum %d", dwPetVnum);
		return NULL;
	}

	m_pGrowthPet->SetEmpire(m_pOwner->GetEmpire());

	m_pGrowthPet->SetName(m_strName);
	m_pGrowthPet->SetLevel(GetPetPoint(POINT_UPBRINGING_PET_LEVEL));
	m_pGrowthPet->SetCharacterSize(m_bSize);

	SetSummonItem(pSummonItem);
	pSummonItem->Lock(true);

	RefreshAffect();
	m_pGrowthPet->Show(m_pOwner->GetMapIndex(), x, y, z);

	if (!bIsResummon && m_pOwner->GetDesc())
	{
		TPacketGCPetSummon packet;

		packet.header = HEADER_GC_PET_SUMMON;
		packet.dwID = m_dwID;
		m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));

		if (dwPetEvol == PET_GROWTH_SKILL_OPEN_EVOL_LEVEL)
			UpdateSkillPacket();
	}

	if (!bIsResummon)
	{
		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 746, "%s", m_strName.c_str());

		m_bMobExpMsg = false;
		m_bItemExpMsg = false;
	}

	m_pOwner->ComputePoints();

	growthpet_update_event_info* info = AllocEventInfo<growthpet_update_event_info>();
	info->pet = this;

	m_pUpdateEvent = event_create(growthpet_update_event, info, PASSES_PER_SEC(1));

	return this;
}

void CGrowthPet::Unsummon()
{
	if (IsSummoned())
	{
		if (m_pGrowthPet)
			M2_DESTROY_CHARACTER(m_pGrowthPet);

		m_pGrowthPet = NULL;

		if (m_pOwner && m_pOwner->GetDesc())
		{
			TPacketGCPet packet;
			packet.header = HEADER_GC_PET;
			packet.subheader = SUBHEADER_PET_UNSUMMON;

			m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));

			m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 747, "%s", m_strName.c_str());
		}
	}

	if (m_pOwner && m_pOwner->GetActiveGrowthPet())
		m_pOwner->SetActiveGrowthPet(NULL);

	if(m_pSummonItem)
		m_pSummonItem->Lock(false);

	ClearAffect();

	event_cancel(&m_pUpdateEvent);
	event_cancel(&m_pFeatherSkillEvent);
}

bool CGrowthPet::Update()
{
	bool bResult = true;
	bool bUnsummon = false;

	if (IsSummoned() && m_pGrowthPet->IsDead())
		bUnsummon = true;
	else if (!m_pSummonItem)
		bUnsummon = true;
	else if (m_pSummonItem->GetOwner() != this->GetOwner())
		bUnsummon = true;

	if (bUnsummon)
	{
		Unsummon();
		return true;
	}

	if (IS_SET(m_bAutoSkillFlag, PET_SKILL_FLAG_HEAL))
	{
		if (m_pOwner->GetHP() < (m_pOwner->GetMaxHP() / 100) * 20)
			UseSkill(PET_SKILL_AFFECT_HEAL);
	}

	if (IS_SET(m_bAutoSkillFlag, PET_SKILL_FLAG_INVINCIBILITY))
	{
		if (m_pOwner->GetHP() < (m_pOwner->GetMaxHP() / 100) * 10)
			UseSkill(PET_SKILL_AFFECT_INVINCIBILITY);
	}

	if (IS_SET(m_bAutoSkillFlag, PET_SKILL_FLAG_REMOVAL))
	{
		if (m_pOwner->IsAffectFlag(AFF_SLOW)
			|| m_pOwner->IsAffectFlag(AFF_POISON)
			|| m_pOwner->IsAffectFlag(AFF_FIRE)
			|| m_pOwner->IsAffectFlag(AFF_STUN)
			/*|| m_pOwner->IsAffectFlag(AFF_BLEEDING)*/)
			UseSkill(PET_SKILL_AFFECT_REMOVAL);
	}

	if (IS_SET(m_bAutoSkillFlag, PET_SKILL_FLAG_FEATHER))
	{
		if (m_pOwner->GetVictim())
			UseSkill(PET_SKILL_FEATHER);
	}

	DWORD dwPetAge = time(0) - GetPetPoint(POINT_UPBRINGING_BIRTHDAY);
	dwPetAge = dwPetAge / (3600 * 24);
	DWORD dwAgeApply = dwPetAge / 60;

	if (m_dwAgeApply != dwAgeApply)
		m_dwAgeApply = dwAgeApply;

	if (IsSummoned())
		bResult = bResult && UpdateFollowAI();

	return bResult;
}

bool CGrowthPet::UpdateFollowAI()
{
	if (!m_pGrowthPet->m_pkMobData || !m_pOwner)
		return false;

	LPCHARACTER pTarget = m_pFeatherSkillEvent ? m_pOwner->GetVictim() : m_pOwner;

	if (!pTarget)
		pTarget = m_pOwner;

	float START_FOLLOW_DISTANCE = 300.0f;
	float START_RUN_DISTANCE = 900.0f;

	float RESPAWN_DISTANCE = 4500.f;
	int APPROACH = m_pFeatherSkillEvent ? 50 : 200;

	bool bRun = m_pFeatherSkillEvent && pTarget != m_pOwner ? true : false;

	DWORD currentTime = time(0);

	long ownerX = pTarget->GetX();
	long ownerY = pTarget->GetY();

	long charX = m_pGrowthPet->GetX();
	long charY = m_pGrowthPet->GetY();

	float fDist = DISTANCE_APPROX(charX - ownerX, charY - ownerY);

	if (fDist >= RESPAWN_DISTANCE)
	{
		float fOwnerRot = pTarget->GetRotation() * 3.141592f / 180.f;
		float fx = -APPROACH * cos(fOwnerRot);
		float fy = -APPROACH * sin(fOwnerRot);
		if (m_pGrowthPet->Show(pTarget->GetMapIndex(), ownerX + fx, ownerY + fy))
		{
			return true;
		}
	}


	if (fDist >= START_FOLLOW_DISTANCE || m_pFeatherSkillEvent)
	{
		if (fDist >= START_RUN_DISTANCE)
		{
			bRun = true;
		}

		m_pGrowthPet->SetNowWalking(!bRun);

		FollowCharacter(APPROACH);

		m_pGrowthPet->SetLastAttacked(currentTime);
	}
	else
		m_pGrowthPet->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	return true;
}

bool CGrowthPet::FollowCharacter(float fMinDistance)
{
	if (!m_pOwner || !m_pGrowthPet)
		return false;

	LPCHARACTER pTarget = m_pFeatherSkillEvent ? m_pOwner->GetVictim() : m_pOwner;

	if (!pTarget)
		pTarget = m_pOwner;

	float fOwnerX = pTarget->GetX();
	float fOwnerY = pTarget->GetY();

	float fPetX = m_pGrowthPet->GetX();
	float fPetY = m_pGrowthPet->GetY();

	float fDist = DISTANCE_SQRT(fOwnerX - fPetX, fOwnerY - fPetY);
	if (fDist <= fMinDistance)
		return false;

	m_pGrowthPet->SetRotationToXY(fOwnerX, fOwnerY);

	float fx, fy;

	float fDistToGo = fDist - fMinDistance;
	GetDeltaByDegree(m_pGrowthPet->GetRotation(), fDistToGo, &fx, &fy);

	if (!m_pGrowthPet->Goto((int)(fPetX + fx + 0.5f), (int)(fPetY + fy + 0.5f)))
		return false;

	m_pGrowthPet->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0, 0);

	return true;
}

void CGrowthPet::Save()
{
	TGrowthPet petTable;
	CreateGrowthPetProto(&petTable);

	db_clientdesc->DBPacket(HEADER_GD_GROWTH_PET_SAVE, 0, &petTable, sizeof(TGrowthPet));
}

void CGrowthPet::SetGrowthPetProto(TGrowthPet* pGrowthPetTable)
{
	m_dwID = pGrowthPetTable->dwID;
	m_dwVnum = pGrowthPetTable->dwVnum;
	m_bState = pGrowthPetTable->bState;
	m_strName = pGrowthPetTable->szName;
	m_bSize = pGrowthPetTable->bSize;
	m_bType = pGrowthPetTable->bType;
	m_bLevelStep = pGrowthPetTable->bLevelStep;
	thecore_memcpy(m_aSkill, pGrowthPetTable->aSkill, sizeof(m_aSkill));
	m_dwHPApply = pGrowthPetTable->dwHPApply;
	m_dwSPApply = pGrowthPetTable->dwSPApply;
	m_dwDefApply = pGrowthPetTable->dwDefApply;
	m_dwAgeApply = pGrowthPetTable->dwAgeApply;

	SetPetPoint(POINT_UPBRINGING_PET_LEVEL, pGrowthPetTable->dwLevel);
	SetPetPoint(POINT_UPBRINGING_PET_EXP, pGrowthPetTable->lExp);
	SetPetPoint(POINT_UPBRINGING_PET_ITEM_EXP, pGrowthPetTable->lItemExp);
	SetPetPoint(POINT_UPBRINGING_PET_NEXT_EXP, GetRequiredTotalExp(pGrowthPetTable->dwLevel));
	SetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL, pGrowthPetTable->bEvolution);
	SetPetPoint(POINT_UPBRINGING_PET_HP, pGrowthPetTable->dwHP);
	SetPetPoint(POINT_UPBRINGING_PET_DEF_GRADE, pGrowthPetTable->dwDef);
	SetPetPoint(POINT_UPBRINGING_PET_SP, pGrowthPetTable->dwSP);
	SetPetPoint(POINT_UPBRINGING_DURATION, pGrowthPetTable->lEndTime);
	SetPetPoint(POINT_UPBRINGING_MAX_DURATION, pGrowthPetTable->lMaxTime);
	SetPetPoint(POINT_UPBRINGING_BIRTHDAY, pGrowthPetTable->lBirthday);

	RefreshAutoSkillFlag();

	m_pSummonItem = ITEM_MANAGER::Instance().FindByVID(m_dwID);
}

void CGrowthPet::CreateGrowthPetProto(TGrowthPet* pGrowthPetTable)
{
	pGrowthPetTable->dwID = m_dwID;
	pGrowthPetTable->dwVnum = m_dwVnum;
	pGrowthPetTable->bState = m_bState;
	pGrowthPetTable->dwOwner = m_bState == STATE_SAFEBOX ? m_pOwner->GetDesc()->GetAccountTable().id : m_pOwner->GetPlayerID();
	strlcpy(pGrowthPetTable->szName, m_strName.c_str(), sizeof(pGrowthPetTable->szName));
	pGrowthPetTable->bSize = m_bSize;
	pGrowthPetTable->bLevelStep = m_bLevelStep;
	pGrowthPetTable->dwLevel = GetPetPoint(POINT_UPBRINGING_PET_LEVEL);
	pGrowthPetTable->bEvolution = GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL);
	pGrowthPetTable->bType = m_bType;

	pGrowthPetTable->dwHP = GetPetPoint(POINT_UPBRINGING_PET_HP);
	pGrowthPetTable->dwDef = GetPetPoint(POINT_UPBRINGING_PET_DEF_GRADE);
	pGrowthPetTable->dwSP = GetPetPoint(POINT_UPBRINGING_PET_SP);

	pGrowthPetTable->dwHPApply = m_dwHPApply;
	pGrowthPetTable->dwSPApply = m_dwSPApply;
	pGrowthPetTable->dwDefApply = m_dwDefApply;
	pGrowthPetTable->dwAgeApply = m_dwAgeApply;

	thecore_memcpy(pGrowthPetTable->aSkill, m_aSkill, sizeof(pGrowthPetTable->aSkill));

	pGrowthPetTable->lExp = GetPetPoint(POINT_UPBRINGING_PET_EXP);
	pGrowthPetTable->lItemExp = GetPetPoint(POINT_UPBRINGING_PET_ITEM_EXP);

	pGrowthPetTable->lBirthday = GetPetPoint(POINT_UPBRINGING_BIRTHDAY);
	pGrowthPetTable->lEndTime = GetPetPoint(POINT_UPBRINGING_DURATION);
	pGrowthPetTable->lMaxTime = GetPetPoint(POINT_UPBRINGING_MAX_DURATION);
}

void CGrowthPet::SetPetPoint(BYTE bIdx, DWORD dwVal)
{
	m_dwPoints[bIdx] = dwVal;
}

void CGrowthPet::ChangePetPoint(BYTE bIdx, int iVal, bool bAmount, bool bPacket)
{
	switch (bIdx)
	{
		case POINT_UPBRINGING_PET_LEVEL:
		{
			if (!bAmount)
				iVal += GetPetPoint(bIdx);

			if (iVal > PET_MAX_LEVEL)
				return;

			iVal = MAX(0, iVal);
			SetPetPoint(bIdx, iVal);

			m_pGrowthPet->SetLevel(iVal);
			SendLevelUpEffect();

			DWORD dwNewLevel = GetPetPoint(POINT_UPBRINGING_PET_LEVEL);

			if(m_pOwner)
				m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 748, "%s#%d", m_strName.c_str(), dwNewLevel);

			ChangePetPoint(POINT_UPBRINGING_PET_NEXT_EXP, GetRequiredTotalExp(dwNewLevel), true);

			m_bItemExpMsg = false;
			m_bMobExpMsg = false;
		} break;

		case POINT_UPBRINGING_PET_EXP:
		case POINT_UPBRINGING_PET_ITEM_EXP:
		case POINT_UPBRINGING_PET_NEXT_EXP:
		case POINT_UPBRINGING_PET_EVOL_LEVEL:
		case POINT_UPBRINGING_PET_HP:
		case POINT_UPBRINGING_PET_DEF_GRADE:
		case POINT_UPBRINGING_PET_SP:
		case POINT_UPBRINGING_DURATION:
		case POINT_UPBRINGING_MAX_DURATION:
		case POINT_UPBRINGING_BIRTHDAY:
		{
			if (!bAmount)
				iVal += GetPetPoint(bIdx);

			iVal = MAX(0, iVal);
			SetPetPoint(bIdx, iVal);
		}
		break;
	}

	if (bPacket && m_pOwner && m_pOwner->GetDesc())
	{
		TPacketGCPetPointUpdate packet;
		packet.header = HEADER_GC_PET_POINT_CHANGE;
		packet.dwID = GetPetID();
		packet.bPoint = bIdx;
		packet.dwValue = GetPetPoint(bIdx);

		m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));
	}
}

DWORD CGrowthPet::GetPetPoint(BYTE bIdx) const
{
	return m_dwPoints[bIdx];
}

void CGrowthPet::PetSetExchangePacket(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
		return;

	TPacketGCPetSet packet;
	packet.header = HEADER_GC_PET_SET_EXCHANGE;
	packet.dwID = GetPetID();
	packet.dwSummonItemVnum = GetSummonItemVnum();

	strlcpy(packet.szName, GetPetName().c_str(), sizeof(packet.szName));
	thecore_memcpy(packet.aSkill, GetPetSkill(), sizeof(packet.aSkill));

	for (int i = 0; i < POINT_UPBRINGING_MAX_NUM; i++)
		packet.dwPoints[i] = GetPetPoint(i);

	ch->GetDesc()->Packet(&packet, sizeof(packet));
}

void CGrowthPet::SendLevelUpEffect()
{
	if (!m_pOwner)
		return;

	struct packet_point_change pack;
	pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
	pack.dwVID = m_pGrowthPet->GetVID();
	pack.type = POINT_LEVEL;
	pack.value = GetPetPoint(POINT_UPBRINGING_PET_LEVEL);
	pack.amount = 0;

	GetOwner()->PacketAround(&pack, sizeof(pack));
}

void CGrowthPet::SendLevelStepEffect()
{
	if (!m_pOwner)
		return;

	struct packet_point_change pack;

	pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
	pack.dwVID = m_pGrowthPet->GetVID();
	pack.type = POINT_LEVEL_STEP;
	pack.value = 0;
	pack.amount = 0;

	DWORD dwCurrentEXP = GetPetPoint(POINT_UPBRINGING_PET_EXP);
	DWORD dwNextEXP = GetRequiredMobExp(GetPetPoint(POINT_UPBRINGING_PET_LEVEL));
	DWORD q = DWORD(dwNextEXP / 4.0f);

	if (dwCurrentEXP >= q * 4 && m_bLevelStep < 4)
	{
		for (int i = 0; i < 4 - m_bLevelStep; ++i)
		{
			++m_bLevelStep;
			m_pOwner->PacketAround(&pack, sizeof(pack));
		}
	}
	else if (dwCurrentEXP >= q * 3 && m_bLevelStep < 3)
	{
		for (int i = 0; i < 3 - m_bLevelStep; ++i)
		{
			++m_bLevelStep;
			m_pOwner->PacketAround(&pack, sizeof(pack));
		}
	}
	else if (dwCurrentEXP >= q * 2 && m_bLevelStep < 2)
	{
		for (int i = 0; i < 2 - m_bLevelStep; ++i)
		{
			++m_bLevelStep;
			m_pOwner->PacketAround(&pack, sizeof(pack));
		}
	}
	else if (dwCurrentEXP >= q && m_bLevelStep < 1)
	{
		++m_bLevelStep;
		m_pOwner->PacketAround(&pack, sizeof(pack));

	}
}

void CGrowthPet::UpdateSkillPacket()
{
	if (!m_pOwner || !m_pOwner->GetDesc() || !m_pSummonItem)
		return;

	const DWORD dwPetVnum = m_pSummonItem->GetVnum();
	const DWORD dwPetLevel = GetPetPoint(POINT_UPBRINGING_PET_LEVEL) - 81;

	TPacketGCPetSkillUpdate packet;
	packet.header = HEADER_GC_PET_SKILL_UPDATE;
	packet.dwID = m_dwID;

	for (int i = 0; i < PET_SKILL_COUNT_MAX; ++i)
	{
		packet.aSkill[i].bLocked = m_aSkill[i].bLocked;
		packet.aSkill[i].bSkill = m_aSkill[i].bSkill;
		packet.aSkill[i].bLevel = m_aSkill[i].bLevel;
		packet.aSkill[i].dwCooltime = m_aSkill[i].dwCooltime;

		memset(packet.aSkill[i].bSkillFormula1, 0, sizeof(packet.aSkill[i].bSkillFormula1));
		memset(packet.aSkill[i].wSkillFormula2, 0, sizeof(packet.aSkill[i].wSkillFormula2));
		memset(packet.aSkill[i].bSkillFormula3, 0, sizeof(packet.aSkill[i].bSkillFormula3));

		if (m_aSkill[i].bSkill)
		{
			CPetSkillProto* pSkillProto = CGrowthPetManager::Instance().GetGrowthPetSkill(dwPetVnum, m_aSkill[i].bSkill);
			if (!pSkillProto)
				continue;

			CPoly* pSkillPoly = pSkillProto->GetPolyByType(m_bType);
			if (!pSkillPoly)
				continue;

			for (int j = 1; j <= PET_GROWTH_SKILL_LEVEL_MAX; ++j)
			{
				if (pSkillProto->GetSkillType() == PET_SKILL_USE_TYPE_AUTO)
				{
					BYTE bSkillFormula1 = 0;
					if (m_aSkill[i].bSkill == PET_SKILL_AFFECT_REMOVAL && (m_dwVnum == PET_NEMERE || m_dwVnum == PET_RED_DRAGON || m_dwVnum == PET_NESSIE))
					{
						switch (m_bType)
						{
							case 1:
							case 2:
								bSkillFormula1 = 47;
								break;
							case 3:
							case 4:
								bSkillFormula1 = 57;
								break;
							case 5:
							case 6:
							case 7:
							case 8:
								bSkillFormula1 = 67;
								break;
						}
					}
					else
					{
						pSkillProto->kActivatePctPoly.SetVar("lv", dwPetLevel);
						pSkillProto->kActivatePctPoly.SetVar("k", j);
						bSkillFormula1 = (BYTE)pSkillProto->kActivatePctPoly.Eval();
					}

					packet.aSkill[i].bSkillFormula1[j - 1] = bSkillFormula1;

					pSkillProto->kDurationPoly.SetVar("lv", dwPetLevel);
					pSkillProto->kDurationPoly.SetVar("k", j);
					const BYTE bSkillFormula3 = (BYTE)pSkillProto->kDurationPoly.Eval();

					packet.aSkill[i].bSkillFormula3[j - 1] = bSkillFormula3;
				}

				pSkillPoly->SetVar("lv", dwPetLevel);
				pSkillPoly->SetVar("k", j);

				const WORD wSkillFormula2 = (WORD)pSkillPoly->Eval();

				packet.aSkill[i].wSkillFormula2[j - 1] = wSkillFormula2;
			}
		}
	}

	m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));
}

void CGrowthPet::NameChange(const char* c_szName, LPITEM pChangeNameItem, LPITEM pUpBringingItem)
{
	if (!m_pOwner)
		return;

	TPacketGCPetNameChangeResult packet;
	packet.header = HEADER_GC_PET_NAME_CHANGE_RESULT;
	packet.subheader = SUBHEADER_PET_NAME_CHANGE_SUCCESS;
	packet.dwID = GetPetID();
	memset(packet.szName, 0, sizeof(packet.szName));

	if (strlen(c_szName) < PET_NAME_MIN_SIZE)
	{
		packet.subheader = SUBHEADER_PET_NAME_CHANGE_FAILED;
		m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));

		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 749, "");
		return;
	}

	if (CBanwordManager::Instance().CheckString(c_szName, strlen(c_szName)))
	{
		packet.subheader = SUBHEADER_PET_NAME_CHANGE_FAILED;
		m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));

		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 750, "");
		return;
	}

	if (!check_name(c_szName))
	{
		packet.subheader = SUBHEADER_PET_NAME_CHANGE_FAILED;
		m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));

		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 750, "");
		return;
	}

	char szNameText[64 + 1];
	DBManager::instance().EscapeString(szNameText, sizeof(szNameText), c_szName, strlen(c_szName));

	int iPrice = PET_HATCHING_MONEY;
	if (m_pOwner->GetGold() < iPrice)
	{
		packet.subheader = SUBHEADER_PET_NAME_CHANGE_FAILED;
		m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));

		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 70, "");
		return;
	}

	m_pOwner->PointChange(POINT_GOLD, -iPrice);
	m_strName = szNameText;
	strlcpy(packet.szName, szNameText, sizeof(packet.szName));

	if (IsSummoned())
		Summon(pUpBringingItem, true);

	ITEM_MANAGER::Instance().RemoveItem(pChangeNameItem);

	m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));
}

void CGrowthPet::Feed(const TPacketCGPetFeed* feedPacket)
{
	if (!m_pOwner)
		return;

	if (feedPacket->index == FEED_LIFE_TIME_EVENT && m_pOwner->IsUnderRefine())
	{
		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 74, "");
		return;
	}

	TPacketGCPet packet;
	packet.header = HEADER_GC_PET;
	packet.subheader = SUBHEADER_PET_FEED_SUCCESS;

	switch (feedPacket->index)
	{
		case FEED_LIFE_TIME_EVENT:
		{
			for (DWORD dwPos : feedPacket->pos)
			{
				LPITEM item = m_pOwner->GetInventoryItem(dwPos);
				if (!item)
				{
					packet.subheader = SUBHEADER_PET_FEED_FAILED;
					break;
				}

				bool bIsFoodItem = true;

				if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_PETFEED))
					bIsFoodItem = false;

				if (item->GetType() != ITEM_GROWTH_PET)
					bIsFoodItem = false;

				if (item->GetSubType() != PET_UPBRINGING && item->GetSubType() != PET_EGG)
					bIsFoodItem = false;

				if (!bIsFoodItem)
				{
					packet.subheader = SUBHEADER_PET_FEED_FAILED;
					break;
				}

				if (!RewardFood(item, PET_FEED_TYPE_PCT))
					break;
			}
		} break;

		case FEED_EXP_EVENT:
		{
			DWORD dwTotalEXP = 0;
			for (int i = 0; i < PET_FEED_SLOT_MAX; ++i)
			{
				if (!feedPacket->count[i])
					break;

				LPITEM item = m_pOwner->GetInventoryItem(feedPacket->pos[i]);
				if (!item)
				{
					packet.subheader = SUBHEADER_PET_FEED_FAILED;
					break;
				}

				DWORD dwItemEXP = 0;
				switch (item->GetType())
				{
					case ITEM_WEAPON:
					case ITEM_ARMOR:
						dwItemEXP = (item->GetCount() * item->GetShopBuyPrice()) / 2;
						break;

					case ITEM_GROWTH_PET:
					{
						switch (item->GetSubType())
						{
							case PET_EXPFOOD:
								dwItemEXP = (item->GetCount() * item->GetShopBuyPrice()) / 2;
								break;

							case PET_EXPFOOD_PER:
								dwItemEXP = GetRequiredItemExp(GetPetPoint(POINT_UPBRINGING_PET_LEVEL)) * 0.05f;
								break;

						}
					} break;
				}

				if (RewardEXP(EXP_TYPE_ITEM, dwItemEXP))
				{
					ITEM_MANAGER::Instance().RemoveItem(item);
					dwTotalEXP += dwItemEXP;
				}
				else
					break;
			}

			m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 751, "%d", dwTotalEXP);
		} break;

		case FEED_EVOL_EVENT:
		{
			if (!Evolve(feedPacket))
				packet.subheader = SUBHEADER_PET_FEED_FAILED;
		} break;
	}

	m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));
}

bool CGrowthPet::CanReceiveEXP()
{
	DWORD dwLevel = GetPetPoint(POINT_UPBRINGING_PET_LEVEL);
	DWORD bEvolution = GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL);

	if (dwLevel >= PET_MAX_LEVEL)
		return false;

	if (dwLevel > m_pOwner->GetLevel())
		return false;

	if (dwLevel <= 40 && bEvolution == PET_EVOLUTION_1)
		return true;

	if (dwLevel <= 80 && bEvolution == PET_EVOLUTION_2)
		return true;

	if (dwLevel >= 81 && (bEvolution == PET_EVOLUTION_3 || bEvolution == PET_EVOLUTION_4))
		return true;

	return false;
}

BYTE CGrowthPet::CanLevelUp()
{
	DWORD dwLevel = GetPetPoint(POINT_UPBRINGING_PET_LEVEL);
	DWORD bEvolution = GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL);

	if (dwLevel >= PET_MAX_LEVEL)
		return LEVELUP_ERROR_MAX_LEVEL;

	if (dwLevel == m_pOwner->GetLevel())
		return LEVELUP_ERROR_OWNER_LEVEL;

	if (dwLevel == 40 && bEvolution == PET_EVOLUTION_1)
		return LEVELUP_ERROR_EVOLUTION;

	if (dwLevel == 80 && bEvolution == PET_EVOLUTION_2)
		return LEVELUP_ERROR_EVOLUTION;

	return LEVELUP_ERROR_NONE;
}

DWORD CGrowthPet::GetRequiredTotalExp(DWORD dwLevel)
{
	if (dwLevel >= PET_MAX_LEVEL)
		return 0;

	DWORD dwEXP = exp_table[dwLevel] / 2;

	return dwEXP;
}

DWORD CGrowthPet::GetRequiredMobExp(DWORD dwLevel)
{
	if (dwLevel >= PET_MAX_LEVEL)
		return 0;

	DWORD dwTotalEXP = GetRequiredTotalExp(dwLevel);
	DWORD dwItemEXP = dwTotalEXP / 10;

	return dwTotalEXP - dwItemEXP;
}


DWORD CGrowthPet::GetRequiredItemExp(DWORD dwLevel)
{
	if (dwLevel >= PET_MAX_LEVEL)
		return 0;

	DWORD dwEXP = GetRequiredTotalExp(dwLevel) / 10;

	return dwEXP;
}

bool CGrowthPet::RewardEXP(BYTE bType, DWORD dwEXP)
{
	if (!m_pOwner)
		return false;

	DWORD dwPetLevel = GetPetPoint(POINT_UPBRINGING_PET_LEVEL);
	if (dwPetLevel > m_pOwner->GetLevel())
		return false;

	if (!CanReceiveEXP())
		return false;

	DWORD dwCurrentMobEXP = GetPetPoint(POINT_UPBRINGING_PET_EXP);
	DWORD dwCurrentItemEXP = GetPetPoint(POINT_UPBRINGING_PET_ITEM_EXP);

	switch (bType)
	{
		case EXP_TYPE_MOB:
		{
			BYTE bError = CanLevelUp();
			DWORD dwRequiredMobEXP = GetRequiredMobExp(dwPetLevel);

			if (dwCurrentMobEXP >= dwRequiredMobEXP)
			{
				if (bError != LEVELUP_ERROR_NONE)
				{
					if (!m_bItemExpMsg && (dwCurrentItemEXP >= GetRequiredItemExp(dwPetLevel)))
					{
						m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 752, "");
						m_bItemExpMsg = true;
					}

					return false;
				}
			}

			DWORD dwRewardEXP = (dwEXP + dwCurrentMobEXP) > dwRequiredMobEXP ? (dwRequiredMobEXP - dwCurrentMobEXP) : dwEXP;
			if(dwRewardEXP)
				ChangePetPoint(POINT_UPBRINGING_PET_EXP, dwRewardEXP);

			SendLevelStepEffect();

			if (dwCurrentItemEXP >= GetRequiredItemExp(dwPetLevel))
			{
				if (dwCurrentMobEXP + dwEXP >= dwRequiredMobEXP)
				{
					SendEvolveMessage();

					if (bError != LEVELUP_ERROR_NONE)
					{
						if (!m_bMobExpMsg && (bError == LEVELUP_ERROR_OWNER_LEVEL))
						{
							m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 753, "");
							m_bMobExpMsg = true;
						}

						return true;
					}

					ChangePetPoint(POINT_UPBRINGING_PET_EXP, 0, true);
					ChangePetPoint(POINT_UPBRINGING_PET_ITEM_EXP, 0, true);

					ChangePetPoint(POINT_UPBRINGING_PET_LEVEL, 1);
					IncreaseBattlePoints(dwPetLevel + 1);

					m_bLevelStep = 0;
				}
			}
			else if (GetPetPoint(POINT_UPBRINGING_PET_EXP) >= dwRequiredMobEXP)
			{
				if (!m_bItemExpMsg)
				{
					m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 752, "");
					m_bItemExpMsg = true;
				}
			}

		} break;

		case EXP_TYPE_ITEM:
		{
			BYTE bError = CanLevelUp();
			DWORD dwRequiredItemEXP = GetRequiredItemExp(dwPetLevel);

			if (dwCurrentItemEXP >= dwRequiredItemEXP)
			{
				if (bError == LEVELUP_ERROR_OWNER_LEVEL && (dwCurrentMobEXP >= GetRequiredMobExp(dwPetLevel)))
					m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 753, "");

				m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 754, "%s", m_strName.c_str());

				return false;
			}

			DWORD dwRewardEXP = (dwEXP + dwCurrentItemEXP) > dwRequiredItemEXP ? (dwRequiredItemEXP - dwCurrentItemEXP) : dwEXP;
			if (dwRewardEXP)
				ChangePetPoint(POINT_UPBRINGING_PET_ITEM_EXP, dwRewardEXP);

			if (dwCurrentMobEXP >= GetRequiredMobExp(dwPetLevel))
			{
				if (dwCurrentItemEXP + dwEXP >= dwRequiredItemEXP)
				{
					SendEvolveMessage();

					if (bError != LEVELUP_ERROR_NONE)
					{
						if (bError == LEVELUP_ERROR_OWNER_LEVEL)
							m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 753, "");

						return true;
					}

					DWORD dwNewRequiredItemEXP = GetRequiredItemExp(dwPetLevel + 1);
					DWORD dwNextLevelItemEXP = dwCurrentItemEXP + dwEXP - dwRequiredItemEXP;

					ChangePetPoint(POINT_UPBRINGING_PET_EXP, 0, true);
					ChangePetPoint(POINT_UPBRINGING_PET_ITEM_EXP, 0, true);

					ChangePetPoint(POINT_UPBRINGING_PET_LEVEL, 1);
					IncreaseBattlePoints(dwPetLevel + 1);

					m_bLevelStep = 0;

					if (CanReceiveEXP() && CanLevelUp() == LEVELUP_ERROR_NONE && dwNextLevelItemEXP && GetPetPoint(POINT_UPBRINGING_PET_LEVEL) < PET_MAX_LEVEL)
					{
						dwNextLevelItemEXP = dwNextLevelItemEXP > dwNewRequiredItemEXP ? dwNewRequiredItemEXP : dwNextLevelItemEXP;
						ChangePetPoint(POINT_UPBRINGING_PET_ITEM_EXP, dwNextLevelItemEXP);
					}
				}
			}

		} break;
	}

	return true;
}

bool CGrowthPet::LevelUp()
{
	DWORD dwPetLevel = GetPetPoint(POINT_UPBRINGING_PET_LEVEL);
	DWORD dwCurrentMobEXP = GetPetPoint(POINT_UPBRINGING_PET_EXP);
	DWORD dwCurrentItemEXP = GetPetPoint(POINT_UPBRINGING_PET_ITEM_EXP);

	if (!CanReceiveEXP())
		return false;

	if (CanLevelUp() != LEVELUP_ERROR_NONE)
		return false;

	if (dwCurrentMobEXP >= GetRequiredMobExp(dwPetLevel) && dwCurrentItemEXP >= GetRequiredItemExp(dwPetLevel))
	{
		ChangePetPoint(POINT_UPBRINGING_PET_EXP, 0, true);
		ChangePetPoint(POINT_UPBRINGING_PET_ITEM_EXP, 0, true);

		ChangePetPoint(POINT_UPBRINGING_PET_LEVEL, 1);
		IncreaseBattlePoints(dwPetLevel + 1);

		m_bLevelStep = 0;
		return true;
	}

	return false;
}

bool CGrowthPet::RewardFood(LPITEM pItem, BYTE bType)
{
	if (!m_pSummonItem)
		return false;

	if (m_pSummonItem->GetID() == pItem->GetID())
		return false;

	DWORD dwDuration = GetPetPoint(POINT_UPBRINGING_DURATION) - time(0);
	DWORD dwMaxDuration = GetPetPoint(POINT_UPBRINGING_MAX_DURATION);

	if (dwDuration >= dwMaxDuration)
		return false;

	DWORD dwLifeReward = (dwMaxDuration / 100.0f) * fnumber(3.0f, 5.0f);

	if ((dwLifeReward + dwDuration > dwMaxDuration) ||
		(bType == PET_FEED_TYPE_FULL))
		dwLifeReward = dwMaxDuration - dwDuration;

	ChangePetPoint(POINT_UPBRINGING_DURATION, dwLifeReward);

	m_pSummonItem->SetSocket(0, m_pSummonItem->GetSocket(0) + dwLifeReward);

	if (pItem->GetSubType() == PET_UPBRINGING)
		CGrowthPetManager::Instance().DeleteGrowthPet(pItem->GetSocket(2), true);

	ITEM_MANAGER::Instance().RemoveItem(pItem);

	return true;
}

void CGrowthPet::PremiumFeed(const TPacketCGPetRevive* revivePacket, LPITEM pSummonItem)
{
	if (!m_pOwner || !m_pOwner->GetDesc())
		return;

	TPacketGCPet packet;
	packet.header = HEADER_GC_PET;
	packet.subheader = SUBHEADER_PET_REVIVE_SUCCESS;

	time_t petAge = pSummonItem->GetSocket(0) - GetPetPoint(POINT_UPBRINGING_BIRTHDAY);
	DWORD requiredMaterialCount = petAge / (3600 * 24 * 10);
	if (!requiredMaterialCount)
		requiredMaterialCount = 1;

	DWORD materialCount = 0;

	for (int i = 0; i < PET_REVIVE_MATERIAL_SLOT_MAX; ++i)
	{
		if (revivePacket->count[i] == 0)
			break;

		LPITEM pItem = m_pOwner->GetInventoryItem(revivePacket->pos[i]);
		if (pItem && pItem->GetType() == ITEM_GROWTH_PET && pItem->GetSubType() == PET_PRIMIUM_FEEDSTUFF)
			materialCount += revivePacket->count[i];
		else
		{
			packet.subheader = SUBHEADER_PET_REVIVE_FAILED;
			m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));
			return;
		}
	}

	if (materialCount < requiredMaterialCount)
	{
		packet.subheader = SUBHEADER_PET_REVIVE_FAILED;
		m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));
		return;
	}

	for (int i = 0; i < PET_REVIVE_MATERIAL_SLOT_MAX; ++i)
	{
		LPITEM item = m_pOwner->GetInventoryItem(revivePacket->pos[i]);
		DWORD dwItemCount = item->GetCount();

		if (dwItemCount < requiredMaterialCount)
		{
			requiredMaterialCount -= dwItemCount;
			ITEM_MANAGER::Instance().RemoveItem(item);
			continue;
		}
		else
		{
			item->SetCount(item->GetCount() - requiredMaterialCount);
			requiredMaterialCount = 0;
			break;
		}
	}

	if(!requiredMaterialCount)
		Revive(pSummonItem, PET_REVIVE_TYPE_PREMIUM);

	m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));
}

void CGrowthPet::Revive(LPITEM pSummonItem, BYTE bType)
{
	if (!m_pOwner)
		return;

	time_t petAge = pSummonItem->GetSocket(0) - GetPetPoint(POINT_UPBRINGING_BIRTHDAY);
	DWORD dwMaxDuration = GetPetPoint(POINT_UPBRINGING_MAX_DURATION);

	float fDurationFactor = 0.5f;
	if (bType == PET_REVIVE_TYPE_PREMIUM)
		fDurationFactor = 1.0f;
	dwMaxDuration *= fDurationFactor;


	ChangePetPoint(POINT_UPBRINGING_DURATION, time(0) + dwMaxDuration, true);
	pSummonItem->SetSocket(0, time(0) + dwMaxDuration);
	pSummonItem->StartRealTimeExpireEvent();

	float fAgeFactor = 0.5f;
	if (bType == PET_REVIVE_TYPE_PREMIUM)
		fAgeFactor = 0.8f;

	if (bType == PET_REVIVE_TYPE_PREMIUM)
		ChangePetPoint(POINT_UPBRINGING_BIRTHDAY, time(0) - (petAge * fAgeFactor), true);
	else
	{
		ChangePetPoint(POINT_UPBRINGING_BIRTHDAY, time(0), true);
		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 755, "");
	}

	m_dwAgeApply = 0;
}

void CGrowthPet::SendEvolveMessage()
{
	if (!m_pOwner)
		return;

	for (DWORD i = 1; i <= PET_EVOLUTION_4; ++i)
	{
		if (i == GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL))
		{
			DWORD dwLevel = GetPetPoint(POINT_UPBRINGING_PET_LEVEL);
			if (dwLevel < arPetEvolutionTable[i - 1].dwLevel)
				return;

			if (i < PET_EVOLUTION_3)
			{
				if (GetPetPoint(POINT_UPBRINGING_PET_EXP) + GetPetPoint(POINT_UPBRINGING_PET_ITEM_EXP) < GetRequiredTotalExp(dwLevel))
					return;
			}
			else if (i == PET_EVOLUTION_3)
			{
				long dwAge = GetPetPoint(POINT_UPBRINGING_BIRTHDAY) - time(0);
				DWORD dwDayCount = dwAge / (3600 * 24);

				if (dwDayCount < PET_LAST_EVOL_MIN_DAY_COUNT)
					return;
			}

			if (i < PET_EVOLUTION_3)
				m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 756, "");
			else if (i == PET_EVOLUTION_3)
				m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 757, "");
		}
	}
}

bool CGrowthPet::Evolve(const TPacketCGPetFeed* feedPacket)
{
	if (!m_pOwner || !m_pSummonItem)
		return false;

	DWORD dwEvolution = GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL);

	if (dwEvolution == PET_EVOLUTION_4)
	{
		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 758, "");
		return true;
	}

	DWORD idx = dwEvolution - 1;
	DWORD dwLevel = GetPetPoint(POINT_UPBRINGING_PET_LEVEL);

	if (dwLevel >= m_pOwner->GetLevel() && dwEvolution < PET_EVOLUTION_3)
	{
		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 753, "");
		return true;
	}

	if (dwLevel < arPetEvolutionTable[idx].dwLevel)
	{
		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 759, "%d", arPetEvolutionTable[idx].dwLevel);
		return true;
	}

	if (dwEvolution < PET_EVOLUTION_3)
	{
		if ((GetPetPoint(POINT_UPBRINGING_PET_EXP) + GetPetPoint(POINT_UPBRINGING_PET_ITEM_EXP)) < GetPetPoint(POINT_UPBRINGING_PET_NEXT_EXP))
		{
			m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 760, "");
			return true;
		}
	}
	else if (dwEvolution == PET_EVOLUTION_3)
	{
		long dwAge = GetPetPoint(POINT_UPBRINGING_BIRTHDAY) - time(0);
		DWORD dwDayCount = dwAge / (3600 * 24);

		if (dwDayCount < PET_LAST_EVOL_MIN_DAY_COUNT)
		{
			m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 761, "%d", PET_LAST_EVOL_MIN_DAY_COUNT);
			return true;
		}
	}

	std::vector<std::pair<WORD, WORD>> itemVec;
	for (int i = 0; i < PET_EVOL_MAX_ITEM_COUNT; ++i)
	{
		std::pair<DWORD, WORD> requiredItem = arPetEvolutionTable[idx].dwItems[i];
		bool bIsFound = false;

		for (int j = 0; j < PET_FEED_SLOT_MAX; ++j)
		{
			if (!feedPacket->count[j])
				break;

			LPITEM item = m_pOwner->GetInventoryItem(feedPacket->pos[j]);
			if (!item)
				return false;

			if (item->GetVnum() == requiredItem.first && item->GetCount() >= requiredItem.second)
			{
				bIsFound = true;
				itemVec.push_back(std::make_pair(feedPacket->pos[j], requiredItem.second));
			}
		}

		if (!bIsFound)
		{
			TItemTable* lItem = ITEM_MANAGER::Instance().GetTable(requiredItem.first);
			if (lItem)
				m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 762, "%s#%d", lItem->szLocaleName, requiredItem.second);
		}
	}

	if (itemVec.size() == PET_EVOL_MAX_ITEM_COUNT)
	{
		for (const auto& pair : itemVec)
		{
			LPITEM item = m_pOwner->GetInventoryItem(pair.first);
			item->SetCount(item->GetCount() - pair.second);
		}

		ChangePetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL, 1);

		if (GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL) == PET_EVOLUTION_4)
			Summon(m_pSummonItem, true);

		if (GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL) == PET_EVOLUTION_2 || GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL) == PET_EVOLUTION_3)
		{
			ChangePetPoint(POINT_UPBRINGING_PET_EXP, 0, true);
			ChangePetPoint(POINT_UPBRINGING_PET_ITEM_EXP, 0, true);

			ChangePetPoint(POINT_UPBRINGING_PET_LEVEL, 1);
		}

		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 763, "%s#%s", m_strName.c_str(), arPetEvolutionTable[idx].szEvolutionName);
	}

	return true;
}

void CGrowthPet::Evolve(BYTE dwEvolution)
{
	if (!dwEvolution || !m_pSummonItem || !m_pOwner)
		return;

	if (dwEvolution > PET_EVOLUTION_4)
		dwEvolution = PET_EVOLUTION_4;

	ChangePetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL, dwEvolution, true);
	Summon(m_pSummonItem, true);

	if(GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL) > PET_EVOLUTION_1)
		m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 763, "%s#%s", m_strName.c_str(), arPetEvolutionTable[dwEvolution - 2].szEvolutionName);
}

void CGrowthPet::RefreshAffect()
{
	if (!m_pOwner || !m_pGrowthPet)
		return;

	DWORD dwPointType[3] = { POINT_MAX_HP , POINT_MAX_SP, POINT_DEF_GRADE };
	DWORD dwPetPointType[3] = { POINT_UPBRINGING_PET_HP, POINT_UPBRINGING_PET_SP, POINT_UPBRINGING_PET_DEF_GRADE };

	for (int i = 0; i < 3; ++i)
	{
		DWORD dwPlayerPoint = 0;
		switch (dwPointType[i])
		{
			case POINT_MAX_HP:
			case POINT_MAX_SP:
				dwPlayerPoint = m_pOwner->GetRealPoint(dwPointType[i]);
				break;

			default:
				dwPlayerPoint = m_pOwner->GetPoint(dwPointType[i]);
				break;
		}

		float fFactor = float(GetPetPoint(dwPetPointType[i]));
		fFactor /= 10;

		dwPlayerPoint = (dwPlayerPoint / 100.0f) * fFactor;

		m_pOwner->AddAffect(AFFECT_GROWTH_PET, dwPointType[i], dwPlayerPoint, AFF_NONE, INFINITE_AFFECT_DURATION, 0, true, true);
	}

	for (int i = 0; i < PET_SKILL_COUNT_MAX; ++i)
	{
		CPetSkillProto* pSkillProto = CGrowthPetManager::Instance().GetGrowthPetSkill(m_pSummonItem->GetVnum(), GetSkillBySlot(i));
		if (!pSkillProto)
			continue;

		if (pSkillProto->GetSkillType() == PET_SKILL_USE_TYPE_PASSIVE)
			UseSkill(pSkillProto->dwAffectFlag);
	}
}

void CGrowthPet::ClearAffect()
{
	if (m_pOwner)
		m_pOwner->RemoveAffect(AFFECT_GROWTH_PET);
}

void CGrowthPet::IncreaseBattlePoints(DWORD dwLevel, bool bAttrChange)
{
	if (!m_pSummonItem || !m_pOwner)
		return;

	DWORD dwHP = 0, dwSP = 0, dwDEF = 0;

	switch (m_bType)
	{
		case 2:
		case 4:
		case 6:
		case 8:
		{
			if (dwLevel % EVEN_TYPE_HP_LEVEL == 0)
			{
				DWORD dwBonusIncrease = m_dwHPApply + m_dwAgeApply;
				if ((m_pSummonItem->GetValue(0) == PET_RED_DRAGON)
					|| (m_pSummonItem->GetValue(0) == PET_NESSIE))
					dwBonusIncrease += 1;

				dwHP += dwBonusIncrease;
			}

			if (dwLevel % EVEN_TYPE_SP_LEVEL == 0)
			{
				DWORD dwBonusIncrease = m_dwSPApply + m_dwAgeApply;
				if ((m_pSummonItem->GetValue(0) == PET_RED_DRAGON)
					|| (m_pSummonItem->GetValue(0) == PET_NESSIE))
					dwBonusIncrease += 1;

				dwSP += dwBonusIncrease;
			}

			if (dwLevel % EVEN_TYPE_DEF_GRADE_LEVEL == 0)
			{
				DWORD dwBonusIncrease = m_dwDefApply + m_dwAgeApply;
				if ((m_pSummonItem->GetValue(0) == PET_RED_DRAGON)
					|| (m_pSummonItem->GetValue(0) == PET_NESSIE))
					dwBonusIncrease += 1;

				dwDEF += dwBonusIncrease;
			}
		} break;

		case 1:
		case 3:
		case 5:
		case 7:
		{
			if (dwLevel % ODD_TYPE_HP_LEVEL == 0)
			{
				DWORD dwBonusIncrease = number(arPetHPBonusTable[m_bType - 1].bMinValue, arPetHPBonusTable[m_bType - 1].bMaxValue) + m_dwAgeApply;;
				if ((m_pSummonItem->GetValue(0) == PET_RED_DRAGON)
					|| (m_pSummonItem->GetValue(0) == PET_NESSIE))
					dwBonusIncrease += 1;

				dwHP += dwBonusIncrease;
			}

			if (dwLevel % ODD_TYPE_SP_LEVEL == 0)
			{
				DWORD dwBonusIncrease = number(arPetSPBonusTable[m_bType - 1].bMinValue, arPetSPBonusTable[m_bType - 1].bMaxValue) + m_dwAgeApply;;
				if ((m_pSummonItem->GetValue(0) == PET_RED_DRAGON)
					|| (m_pSummonItem->GetValue(0) == PET_NESSIE))
					dwBonusIncrease += 1;

				dwSP += dwBonusIncrease;
			}

			if (dwLevel % ODD_TYPE_DEF_GRADE_LEVEL == 0)
			{
				DWORD dwBonusIncrease = number(arPetDefBonusTable[m_bType - 1].bMinValue, arPetDefBonusTable[m_bType - 1].bMaxValue) + m_dwAgeApply;;
				if ((m_pSummonItem->GetValue(0) == PET_RED_DRAGON)
					|| (m_pSummonItem->GetValue(0) == PET_NESSIE))
					dwBonusIncrease += 1;

				dwDEF += dwBonusIncrease;
			}
		} break;
	}

	bool bUpdateClient = false;
	if (!bAttrChange)
		bUpdateClient = true;

	if (dwHP)
		ChangePetPoint(POINT_UPBRINGING_PET_HP, dwHP, false, bUpdateClient);

	if (dwSP)
		ChangePetPoint(POINT_UPBRINGING_PET_SP, dwSP, false, bUpdateClient);

	if (dwDEF)
		ChangePetPoint(POINT_UPBRINGING_PET_DEF_GRADE, dwDEF, false, bUpdateClient);

	if(!bAttrChange)
		RefreshAffect();
}

void CGrowthPet::AttrDetermine(LPITEM pDetermineItem)
{
	if (m_pOwner && m_pOwner->GetDesc())
	{
		TPacketGCPetDetermineResult packet;
		packet.header = HEADER_GC_PET_DETERMINE_RESULT;
		packet.type = m_bType;

		m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));
	}

	pDetermineItem->SetCount(pDetermineItem->GetCount() - 1);
}

void CGrowthPet::AttrChange(LPITEM pSummonItem, LPITEM pAttrChangeItem)
{
	if (!m_pOwner)
		return;

	SetSummonItem(pSummonItem);

	BYTE bMaxType = (pSummonItem->GetVnum() == PET_MONKEY)
		|| (pSummonItem->GetVnum() == PET_SPIDER) 
		|| (pSummonItem->GetVnum() == PET_AZRAEL) ? 6 : 8;
	m_bType = number(1, bMaxType);

	if (m_bType % 2 == 0)
	{
		m_dwHPApply = number(arPetHPBonusTable[m_bType - 1].bMinValue, arPetHPBonusTable[m_bType - 1].bMaxValue);
		m_dwSPApply = number(arPetSPBonusTable[m_bType - 1].bMinValue, arPetSPBonusTable[m_bType - 1].bMaxValue);
		m_dwDefApply = number(arPetDefBonusTable[m_bType - 1].bMinValue, arPetDefBonusTable[m_bType - 1].bMaxValue);
	}

	SetPetPoint(POINT_UPBRINGING_PET_HP, number(1, 23));
	SetPetPoint(POINT_UPBRINGING_PET_SP, number(1, 19));
	SetPetPoint(POINT_UPBRINGING_PET_DEF_GRADE, number(1, 21));

	for (int i = 1; i <= GetPetPoint(POINT_UPBRINGING_PET_LEVEL); ++i)
		IncreaseBattlePoints(i, true);

	ChangePetPoint(POINT_UPBRINGING_PET_HP, 0);
	ChangePetPoint(POINT_UPBRINGING_PET_SP, 0);
	ChangePetPoint(POINT_UPBRINGING_PET_DEF_GRADE, 0);

	TPetHatch hatchTable = arPetHatchTable[m_dwVnum - PET_MONKEY];
	DWORD petDuration = number(hatchTable.bMinDuration, hatchTable.bMaxDuration) * (3600 * 24);

	ChangePetPoint(POINT_UPBRINGING_MAX_DURATION, petDuration, true);
	ChangePetPoint(POINT_UPBRINGING_DURATION, time(0) + petDuration, true);
	pSummonItem->SetSocket(0, time(0) + petDuration);
	pSummonItem->SetSocket(1, petDuration);

	bool bChangeSkill = true;
	for (const auto &skill : m_aSkill)
	{
		if (skill.bSkill)
		{
			bChangeSkill = false;
			break;
		}
	}

	if (bChangeSkill)
	{
		BYTE bSkillCount = number(1, hatchTable.bMaxSKillCount);
		if (pSummonItem->GetVnum() == PET_EXEDYAR)
			bSkillCount = 3;

		for (int i = 0; i < PET_SKILL_COUNT_MAX; ++i)
		{
			if(i < bSkillCount)
				m_aSkill[i].bLocked = false;
			else
				m_aSkill[i].bLocked = true;
		}
	}

	m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 764, "");

	if (m_pOwner->GetDesc())
	{
		TPacketGCPetAttrChangeResult packet;
		packet.header = HEADER_GC_PET_ATTR_CHANGE_RESULT;
		packet.type = m_bType;
		packet.pos.cell = pSummonItem->GetCell();

		m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));

		if(bChangeSkill)
			UpdateSkillPacket();
	}

	pAttrChangeItem->SetCount(pAttrChangeItem->GetCount() - 1);
}

void CGrowthPet::LearnSkill(BYTE bSlot, LPITEM pSkillBookItem)
{
	if (!m_pOwner)
		return;

	if (GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL) < PET_GROWTH_SKILL_OPEN_EVOL_LEVEL)
		return;

	if (m_aSkill[bSlot].bSkill != 0)
		return;

	BYTE bSkillValue = pSkillBookItem->GetValue(0);

	for (const auto& skill : m_aSkill)
	{
		if (skill.bSkill == bSkillValue)
		{
			m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 765, "");
			return;
		}
	}

	m_aSkill[bSlot].bLevel = 1;
	m_aSkill[bSlot].bSkill = bSkillValue;

	m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 766, "");

	ITEM_MANAGER::Instance().RemoveItem(pSkillBookItem);

	RefreshAutoSkillFlag();
	UpdateSkillPacket();

	RefreshAffect();
}

void CGrowthPet::SkillUpgrade(BYTE bSlot)
{
	if (!m_pOwner)
		return;

	if (GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL) < PET_GROWTH_SKILL_OPEN_EVOL_LEVEL)
		return;

	if (m_aSkill[bSlot].bSkill == 0)
		return;

	if (m_aSkill[bSlot].bLevel >= PET_GROWTH_SKILL_LEVEL_MAX)
		return;

	int iPrice = PET_SKILL_UPGRADE_PRICE;
	if (m_pOwner->GetGold() < iPrice)
		return;

	m_pOwner->PointChange(POINT_GOLD, -iPrice);
	m_aSkill[bSlot].bLevel += 1;

	m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 766, "");

	UpdateSkillPacket();

	RefreshAffect();
}

void CGrowthPet::DeleteSkill(BYTE bSlot, LPITEM pSkillDeleteItem)
{
	if (GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL) < PET_GROWTH_SKILL_OPEN_EVOL_LEVEL)
		return;

	if (m_aSkill[bSlot].bSkill == 0)
		return;

	m_aSkill[bSlot].bSkill = 0;
	m_aSkill[bSlot].bLevel = 0;
	m_aSkill[bSlot].dwCooltime = 0;
	ITEM_MANAGER::Instance().RemoveItem(pSkillDeleteItem);

	RefreshAutoSkillFlag();
	UpdateSkillPacket();

	ClearAffect();
	RefreshAffect();
}

void CGrowthPet::DeleteAllSkill(LPITEM pDeleteAllSkillItem)
{
	if (GetPetPoint(POINT_UPBRINGING_PET_EVOL_LEVEL) < PET_GROWTH_SKILL_OPEN_EVOL_LEVEL)
		return;

	for (int i = 0; i < PET_SKILL_COUNT_MAX; ++i)
	{
		m_aSkill[i].bSkill = 0;
		m_aSkill[i].bLevel = 0;
		m_aSkill[i].dwCooltime = 0;
	}

	ITEM_MANAGER::Instance().RemoveItem(pDeleteAllSkillItem);

	RefreshAutoSkillFlag();
	UpdateSkillPacket();

	ClearAffect();
}

bool CGrowthPet::UseSkill(BYTE bSkillAffect)
{
	if (!m_pOwner || !m_pSummonItem)
		return false;

	int iSkillSlot = GetSlotBySkill(bSkillAffect);
	if (iSkillSlot == -1)
		return false;

	if (bSkillAffect == PET_SKILL_AFFECT_HEAL
		|| bSkillAffect == PET_SKILL_AFFECT_INVINCIBILITY
		|| bSkillAffect == PET_SKILL_AFFECT_REMOVAL
		|| bSkillAffect == PET_SKILL_FEATHER)
	{
		if (GetSkillCooltimeBySlot(iSkillSlot) > get_global_time())
			return true;
	}

	CPetSkillProto* pSkillProto = CGrowthPetManager::Instance().GetGrowthPetSkill(m_pSummonItem->GetVnum(), bSkillAffect);
	if (!pSkillProto)
		return false;

	CPoly* pSkillPoly = pSkillProto->GetPolyByType(m_bType);
	if (!pSkillPoly)
		return false;

	const BYTE bSkillLevel = GetSkillLevelBySlot(iSkillSlot);
	const DWORD dwPetLevel = GetPetPoint(POINT_UPBRINGING_PET_LEVEL) - 81;
	pSkillPoly->SetVar("lv", dwPetLevel);
	pSkillPoly->SetVar("k", bSkillLevel);
	const WORD wSkillValue = (WORD)pSkillPoly->Eval();

	if (pSkillProto->GetSkillType() == PET_SKILL_USE_TYPE_PASSIVE)
	{
		if (!pSkillProto->dwPointOn)
			return false;

		if (m_pOwner->AddAffect(AFFECT_GROWTH_PET, pSkillProto->dwPointOn, wSkillValue, AFF_NONE, INFINITE_AFFECT_DURATION, 0, true, true))
			return true;
	}
	else if (pSkillProto->GetSkillType() == PET_SKILL_USE_TYPE_AUTO)
	{
		BYTE bSkillActivatePct = 0; 

		if (bSkillAffect == PET_SKILL_AFFECT_REMOVAL && (m_dwVnum == PET_NEMERE || m_dwVnum == PET_RED_DRAGON || m_dwVnum == PET_NESSIE))
		{
			switch (m_bType)
			{
				case 1:
				case 2:
					bSkillActivatePct = 47;
					break;
				case 3:
				case 4:
					bSkillActivatePct = 57;
					break;
				case 5:
				case 6:
				case 7:
				case 8:
					bSkillActivatePct = 67;
					break;
			}
		}
		else
		{
			pSkillProto->kActivatePctPoly.SetVar("lv", dwPetLevel);
			pSkillProto->kActivatePctPoly.SetVar("k", bSkillLevel);
			bSkillActivatePct = (BYTE)pSkillProto->kActivatePctPoly.Eval();
		}

		SetSkillCooltime(iSkillSlot, time(0) + pSkillProto->dwCooldown);

		if (number(1, 100) > bSkillActivatePct)
			return true;

		switch (bSkillAffect)
		{
			case PET_SKILL_AFFECT_HEAL:
			{
				m_pOwner->PointChange(POINT_HP, wSkillValue);
				m_pOwner->EffectPacket(SE_HPUP_RED);
				m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 767, "%s#%d", pSkillProto->szName, wSkillValue);
			} break;

			case PET_SKILL_AFFECT_INVINCIBILITY:
			{
				float fInvincibleTime = float(wSkillValue) / 10.0f;
				m_pOwner->SetInvincible(true);

				invincible_skill_end_event_info* info = AllocEventInfo<invincible_skill_end_event_info>();
				info->ch = m_pOwner;

				event_create(invincible_skill_end_event, info, PASSES_PER_SEC(fInvincibleTime));
				m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 768, "%s", pSkillProto->szName);
			} break;

			case PET_SKILL_AFFECT_REMOVAL:
			{
				m_pOwner->RemoveBadAffect();
				m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 769, "%s", pSkillProto->szName);
			} break;

			case PET_SKILL_FEATHER:
			{
				pSkillProto->kDurationPoly.SetVar("lv", dwPetLevel);
				pSkillProto->kDurationPoly.SetVar("k", bSkillLevel);
				const BYTE dwSkillDuration = (BYTE)pSkillProto->kDurationPoly.Eval() / 10;

				feather_skill_event_info* info = AllocEventInfo<feather_skill_event_info>();
				info->pPet = this;
				info->pSkillPoly = pSkillPoly;
				info->bSkillLevel = bSkillLevel;
				info->dwDuration = dwSkillDuration + time(0);

				m_pFeatherSkillEvent = event_create(feather_skill_event, info, PASSES_PER_SEC(1));

				m_pOwner->LocaleChatPacket(CHAT_TYPE_INFO, 770, "%s#%s", m_pOwner->GetName() , m_strName.c_str());
			} break;
		}
	}

	return false;
}

int CGrowthPet::GetSlotBySkill(BYTE bSkill)
{
	for (DWORD i = 0; i < PET_SKILL_COUNT_MAX; ++i)
	{
		if (m_aSkill[i].bSkill == bSkill)
			return i;
	}

	return -1;
}
BYTE CGrowthPet::GetSkillLevelBySlot(BYTE bSlot)
{
	if (bSlot >= PET_SKILL_COUNT_MAX)
		return 0;

	return m_aSkill[bSlot].bLevel;
}

BYTE CGrowthPet::GetSkillBySlot(BYTE bSlot)
{
	if (bSlot >= PET_SKILL_COUNT_MAX)
		return 0;

	return m_aSkill[bSlot].bSkill;
}

DWORD CGrowthPet::GetSkillCooltimeBySlot(BYTE bSlot)
{
	if (bSlot >= PET_SKILL_COUNT_MAX)
		return 0;

	return m_aSkill[bSlot].dwCooltime;
}

void CGrowthPet::SetSkillCooltime(BYTE bSlot, DWORD dwCooltime)
{
	if (bSlot >= PET_SKILL_COUNT_MAX)
		return;

	m_aSkill[bSlot].dwCooltime = dwCooltime;

	TPacketGCPetSkillCooltime packet;
	packet.header = HEADER_GC_PET_SKILL_COOLTIME;
	packet.dwID = m_dwID;
	packet.bSlot = bSlot;
	packet.dwCooltime = dwCooltime;

	m_pOwner->GetDesc()->Packet(&packet, sizeof(packet));
}

void CGrowthPet::RefreshAutoSkillFlag()
{
	m_bAutoSkillFlag = 0;

	for (DWORD i = 0; i < PET_SKILL_COUNT_MAX; ++i)
	{
		if (m_aSkill[i].bSkill == PET_SKILL_AFFECT_HEAL)
			SET_BIT(m_bAutoSkillFlag, PET_SKILL_FLAG_HEAL);

		else if (m_aSkill[i].bSkill == PET_SKILL_AFFECT_INVINCIBILITY)
			SET_BIT(m_bAutoSkillFlag, PET_SKILL_FLAG_INVINCIBILITY);

		else if (m_aSkill[i].bSkill == PET_SKILL_AFFECT_REMOVAL)
			SET_BIT(m_bAutoSkillFlag, PET_SKILL_FLAG_REMOVAL);

		else if (m_aSkill[i].bSkill == PET_SKILL_FEATHER)
			SET_BIT(m_bAutoSkillFlag, PET_SKILL_FLAG_FEATHER);
	}
}

CGrowthPetManager::CGrowthPetManager()
{
	m_growthPetGlobalMap.clear();
	m_growthPetSkillMap.clear();
}

CGrowthPetManager::~CGrowthPetManager()
{
	if (m_growthPetSkillMap.size())
	{
		auto it = m_growthPetSkillMap.begin();

		while (it != m_growthPetSkillMap.end())
		{
			for (const auto& kv : it->second)
				M2_DELETE(kv.second);

			++it;
		}
	}
}

LPGROWTH_PET CGrowthPetManager::CreateGrowthPet(LPCHARACTER ch, DWORD dwID)
{
	auto it = m_growthPetGlobalMap.find(dwID);

	if (it != m_growthPetGlobalMap.end())
	{
		sys_err("Duplicated growth pet found id %d", dwID);
		return NULL;
	}

	LPGROWTH_PET pPet = M2_NEW CGrowthPet(ch);

	m_growthPetGlobalMap.emplace(dwID, pPet);

	return pPet;
}

void CGrowthPetManager::DeleteGrowthPet(DWORD dwID, bool bDestroy)
{
	auto it = m_growthPetGlobalMap.find(dwID);

	if (it == m_growthPetGlobalMap.end())
	{
		sys_err("Could not find growth pet id %d", dwID);
		return;
	}

	LPGROWTH_PET pPet = it->second;

	if (!pPet->GetOwner())
	{
		sys_err("Could not find growth pet owner id %d", dwID);
		return;
	}

	DWORD dwPlayerID = pPet->GetOwner()->GetPlayerID();

	pPet->GetOwner()->DeleteGrowthPet(dwID);

	if (bDestroy)
	{
		db_clientdesc->DBPacketHeader(HEADER_GD_GROWTH_PET_DELETE, 0, sizeof(DWORD) + sizeof(DWORD));
		db_clientdesc->Packet(&dwID, sizeof(DWORD));
		db_clientdesc->Packet(&dwPlayerID, sizeof(DWORD));
	}

	M2_DELETE(pPet);

	m_growthPetGlobalMap.erase(it);
}

LPGROWTH_PET CGrowthPetManager::FindGrowthPet(DWORD dwID)
{
	auto it = m_growthPetGlobalMap.find(dwID);

	if (it == m_growthPetGlobalMap.end())
	{
		sys_err("Could not find growth pet id %d", dwID);
		return NULL;
	}

	return it->second;
}

void CGrowthPetManager::GenerateGrowthPetProto(LPCHARACTER ch, TGrowthPet* pGrowthPetTable, LPITEM pEggItem, const char* c_szName)
{
	int iHatchPrice = pEggItem->GetValue(3);
	DWORD dwUpBringingVnum = pEggItem->GetVnum() + EGG_TO_UPBRINGING_DELTA;

	// Abort before charging gold / destroying the egg if the upbringing item has no
	// item_proto entry -- otherwise AutoGiveItem() below returns NULL and the
	// unchecked SetSocket() deref crashes the whole channel (remote DoS), after the
	// player already lost the egg and the hatch fee.
	if (!ITEM_MANAGER::instance().GetTable(dwUpBringingVnum))
	{
		sys_err("GrowthPet hatch: no item_proto for upbringing vnum %u (egg vnum %u)", dwUpBringingVnum, pEggItem->GetVnum());
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 409, "");
		return;
	}

	TPetHatch hatchTable = arPetHatchTable[dwUpBringingVnum - PET_MONKEY];

	DWORD petDuration = number(hatchTable.bMinDuration, hatchTable.bMaxDuration) * (3600 * 24);

	TPetSkill aSkill[PET_SKILL_COUNT_MAX];

	BYTE bSkillCount = number(1, hatchTable.bMaxSKillCount);
	if (dwUpBringingVnum == PET_EXEDYAR)
		bSkillCount = 3;

	for (int i = 0; i < bSkillCount; ++i)
		aSkill[i].bLocked = false;

	BYTE bPetType = number(1, 6);
	DWORD dwHP = 0, dwSP = 0, dwDef = 0;
	DWORD dwHPApply = 0, dwSPApply = 0, dwDefApply = 0;

	if (bPetType % 2 == 0)
	{
		dwHPApply = number(arPetHPBonusTable[bPetType - 1].bMinValue, arPetHPBonusTable[bPetType - 1].bMaxValue);
		dwSPApply = number(arPetSPBonusTable[bPetType - 1].bMinValue, arPetSPBonusTable[bPetType - 1].bMaxValue);
		dwDefApply = number(arPetDefBonusTable[bPetType - 1].bMinValue, arPetDefBonusTable[bPetType - 1].bMaxValue);
	}

	dwHP = number(1, 23);
	dwSP = number(1, 19);
	dwDef = number(1, 21);

	ch->PointChange(POINT_GOLD, -iHatchPrice);
	ITEM_MANAGER::Instance().RemoveItem(pEggItem, "REMOVE (EGG HATCH)");

	LPITEM pUpBringingItem = ch->AutoGiveItem(dwUpBringingVnum, 1);

	// Proto existence is guaranteed above, but AutoGiveItem can still return NULL if
	// the player went dead/stunned at this instant; never dereference it blindly.
	if (!pUpBringingItem)
	{
		sys_err("GrowthPet hatch: AutoGiveItem returned NULL for vnum %u (player %s)", dwUpBringingVnum, ch->GetName());
		return;
	}

	pUpBringingItem->SetSocket(0, time(0) + petDuration);
	pUpBringingItem->SetSocket(1, petDuration);
	pUpBringingItem->SetSocket(2, pUpBringingItem->GetID());

	pGrowthPetTable->dwID = pUpBringingItem->GetID();
	pGrowthPetTable->dwOwner = ch->GetPlayerID();
	pGrowthPetTable->dwVnum = pUpBringingItem->GetVnum();
	pGrowthPetTable->bState = STATE_UPBRINGING;
	strlcpy(pGrowthPetTable->szName, c_szName, sizeof(pGrowthPetTable->szName));
	pGrowthPetTable->bSize = number(1, 3);
	pGrowthPetTable->dwLevel = 1;
	pGrowthPetTable->bLevelStep = 0;
	pGrowthPetTable->bEvolution = 1;
	pGrowthPetTable->bType = bPetType;
	thecore_memcpy(pGrowthPetTable->aSkill, aSkill, sizeof(aSkill));
	pGrowthPetTable->dwHP = dwHP;
	pGrowthPetTable->dwSP = dwSP;
	pGrowthPetTable->dwDef = dwDef;
	pGrowthPetTable->dwHPApply = dwHPApply;
	pGrowthPetTable->dwSPApply = dwSPApply;
	pGrowthPetTable->dwDefApply = dwDefApply;
	pGrowthPetTable->dwAgeApply = 0;
	pGrowthPetTable->lExp = 0;
	pGrowthPetTable->lItemExp = 0;
	pGrowthPetTable->lBirthday = time(0);
	pGrowthPetTable->lEndTime = time(0) + petDuration;
	pGrowthPetTable->lMaxTime = petDuration;
}

void CGrowthPetManager::EggHatch(LPCHARACTER ch, const char* c_szName, TItemPos Cell)
{
	TPacketGCPet packet;
	packet.header = HEADER_GC_PET;

	LPITEM pEggItem = ch->GetItem(Cell);

	if (!pEggItem)
	{
		packet.subheader = SUBHEADER_PET_EGG_USE_FAILED_TIMEOVER;
		ch->GetDesc()->Packet(&packet, sizeof(packet));
		return;
	}

	if (!(pEggItem->GetType() == ITEM_GROWTH_PET && pEggItem->GetSubType() == PET_EGG))
	{
		packet.subheader = SUBHEADER_PET_EGG_USE_FAILED_TIMEOVER;
		ch->GetDesc()->Packet(&packet, sizeof(packet));
		return;
	}

	if (ch->GetGold() < pEggItem->GetValue(3))
	{
		packet.subheader = SUBHEADER_PET_EGG_USE_FAILED_TIMEOVER;
		ch->GetDesc()->Packet(&packet, sizeof(packet));

		ch->LocaleChatPacket(CHAT_TYPE_INFO, 70, "");
		return;
	}

	if (strlen(c_szName) < PET_NAME_MIN_SIZE)
	{
		packet.subheader = SUBHEADER_PET_EGG_USE_FAILED_BECAUSE_NAME;
		ch->GetDesc()->Packet(&packet, sizeof(packet));

		ch->LocaleChatPacket(CHAT_TYPE_INFO, 749, "");
		return;
	}

	if (CBanwordManager::Instance().CheckString(c_szName, strlen(c_szName)))
	{
		packet.subheader = SUBHEADER_PET_EGG_USE_FAILED_BECAUSE_NAME;
		ch->GetDesc()->Packet(&packet, sizeof(packet));

		ch->LocaleChatPacket(CHAT_TYPE_INFO, 750, "");
		return;
	}

	if (!check_name(c_szName))
	{
		packet.subheader = SUBHEADER_PET_EGG_USE_FAILED_BECAUSE_NAME;
		ch->GetDesc()->Packet(&packet, sizeof(packet));

		ch->LocaleChatPacket(CHAT_TYPE_INFO, 750, "");
		return;
	}

	char szNameText[64 + 1];
	DBManager::instance().EscapeString(szNameText, sizeof(szNameText), c_szName, strlen(c_szName));

	TGrowthPet petTable;
	GenerateGrowthPetProto(ch, &petTable, pEggItem, szNameText);
	if (!petTable.dwID)
	{
		ch->GetDesc()->Packet(&packet, sizeof(packet));
		return;
	}

	LPGROWTH_PET pPet = CreateGrowthPet(ch, petTable.dwID);
	if (!pPet)
	{
		ch->GetDesc()->Packet(&packet, sizeof(packet));
		return;
	}

	pPet->SetGrowthPetProto(&petTable);
	pPet->Save();

	ch->SetGrowthPet(pPet);

	packet.subheader = SUBHEADER_PET_EGG_USE_SUCCESS;
	ch->GetDesc()->Packet(&packet, sizeof(packet));

	TItemTable* lItem = ITEM_MANAGER::Instance().GetTable(petTable.dwVnum);
	if (lItem)
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 771, "%s", lItem->szLocaleName);
}

struct SPetPointOnType
{
	const char* c_pszName;
	int iPointOn;
}

kPetPointOnTypes[] = {
	{ "RESIST_WARRIOR", POINT_RESIST_WARRIOR },
	{ "RESIST_SURA", POINT_RESIST_SURA },
	{ "RESIST_ASSASSIN", POINT_RESIST_ASSASSIN },
	{ "RESIST_SHAMAN", POINT_RESIST_SHAMAN },
	{ "MELEE_MAGIC_ATT_BONUS_PER", POINT_MELEE_MAGIC_ATT_BONUS_PER },
	{ "ATTBONUS_MONSTER", POINT_ATTBONUS_MONSTER },
	{ "PENETRATE_PCT", POINT_PENETRATE_PCT },
	{ "CASTING_SPEED", POINT_CASTING_SPEED },
	{ "BOW_DISTANCE", POINT_BOW_DISTANCE },
	{ "STEAL_SP", POINT_STEAL_SP },
	{ "STEAL_HP", POINT_STEAL_HP },
	{ "KILL_HP_RECOVERY", POINT_KILL_HP_RECOVERY },
	{ "BLOCK", POINT_BLOCK },
	{ "REFLECT_MELEE", POINT_REFLECT_MELEE },
	{ "GOLD_DOUBLE_BONUS", POINT_MALL_ITEMBONUS },
	{ "EXP_DOUBLE_BONUS", POINT_MALL_EXPBONUS },
	{ "POTION_BONUS", POINT_POTION_BONUS },
	{ "\n", POINT_NONE },
};

int FindPetPointType(const char* c_sz)
{
	for (int i = 0; *kPetPointOnTypes[i].c_pszName != '\n'; ++i)
	{
		if (!strcasecmp(c_sz, kPetPointOnTypes[i].c_pszName))
			return kPetPointOnTypes[i].iPointOn;
	}
	return POINT_NONE;
}

CPoly* CPetSkillProto::GetPolyByType(BYTE bType)
{
	switch (bType)
	{
		case 1:
			return &kPointPoly1;
		case 2:
			return &kPointPoly2;
		case 3:
			return &kPointPoly3;
		case 4:
			return &kPointPoly4;
		case 5:
			return &kPointPoly5;
		case 6:
			return &kPointPoly6;
		case 7:
			return &kPointPoly7;
		case 8:
			return &kPointPoly8;
	}

	return NULL;
}

bool CGrowthPetManager::InitializeSkill(TGrowthPetSkillTable* pTab, int iSize)
{
	bool bError = false;
	std::vector<CPetSkillProto*> skillVec;

	for (int i = 0; i < iSize; ++i, ++pTab)
	{
		CPetSkillProto* pProto = M2_NEW CPetSkillProto;

		pProto->dwPetVnum = pTab->dwPetVnum;
		pProto->dwSkillVnum = pTab->dwSkillVnum;
		strlcpy(pProto->szName, pTab->szName, sizeof(pProto->szName));
		pProto->bType = pTab->bType;
		pProto->dwCooldown = pTab->dwCooldown;
		pProto->dwAffectFlag = pTab->dwAffectFlag;
		pProto->dwPointOn = FindPetPointType(pTab->szPointOn);

		if (!pProto->kPointPoly1.Analyze(pTab->szPointPoly1))
		{
			sys_err("syntax error skill: %s szPointPoly1: %s", pTab->szName, pTab->szPointPoly1);
			bError = true;
			M2_DELETE(pProto);
			continue;
		}

		if (!pProto->kPointPoly2.Analyze(pTab->szPointPoly2))
		{
			sys_err("syntax error skill: %s szPointPoly2: %s", pTab->szName, pTab->szPointPoly2);
			bError = true;
			M2_DELETE(pProto);
			continue;
		}

		if (!pProto->kPointPoly3.Analyze(pTab->szPointPoly3))
		{
			sys_err("syntax error skill: %s szPointPoly3: %s", pTab->szName, pTab->szPointPoly3);
			bError = true;
			M2_DELETE(pProto);
			continue;
		}

		if (!pProto->kPointPoly4.Analyze(pTab->szPointPoly4))
		{
			sys_err("syntax error skill: %s szPointPoly4: %s", pTab->szName, pTab->szPointPoly4);
			bError = true;
			M2_DELETE(pProto);
			continue;
		}

		if (!pProto->kPointPoly5.Analyze(pTab->szPointPoly5))
		{
			sys_err("syntax error skill: %s szPointPoly5: %s", pTab->szName, pTab->szPointPoly5);
			bError = true;
			M2_DELETE(pProto);
			continue;
		}

		if (!pProto->kPointPoly6.Analyze(pTab->szPointPoly6))
		{
			sys_err("syntax error skill: %s szPointPoly6: %s", pTab->szName, pTab->szPointPoly6);
			bError = true;
			M2_DELETE(pProto);
			continue;
		}

		if (!pProto->kPointPoly7.Analyze(pTab->szPointPoly7))
		{
			sys_err("syntax error skill: %s szPointPoly7: %s", pTab->szName, pTab->szPointPoly7);
			bError = true;
			M2_DELETE(pProto);
			continue;
		}

		if (!pProto->kPointPoly8.Analyze(pTab->szPointPoly8))
		{
			sys_err("syntax error skill: %s szPointPoly8: %s", pTab->szName, pTab->szPointPoly8);
			bError = true;
			M2_DELETE(pProto);
			continue;
		}

		if (!pProto->kActivatePctPoly.Analyze(pTab->szActivatePctPoly))
		{
			sys_err("syntax error skill: %s szActivatePctPoly: %s", pTab->szName, pTab->szActivatePctPoly);
			bError = true;
			M2_DELETE(pProto);
			continue;
		}

		if (!pProto->kActivatePctPoly.Analyze(pTab->szActivatePctPoly))
		{
			sys_err("syntax error skill: %s szActivatePctPoly: %s", pTab->szName, pTab->szActivatePctPoly);
			bError = true;
			M2_DELETE(pProto);
			continue;
		}

		if (!pProto->kDurationPoly.Analyze(pTab->szDurationPoly))
		{
			sys_err("syntax error skill: %s szDurationPoly: %s", pTab->szName, pTab->szDurationPoly);
			bError = true;
			M2_DELETE(pProto);
			continue;
		}

		skillVec.push_back(pProto);
	}

	if (!bError)
	{
		auto it = m_growthPetSkillMap.begin();

		while (it != m_growthPetSkillMap.end())
		{
			for (const auto& kv : it->second)
				M2_DELETE(kv.second);

			++it;
		}

		for (const auto& pSkill : skillVec)
			m_growthPetSkillMap[pSkill->dwPetVnum][pSkill->dwSkillVnum] = pSkill;
	}

	return !bError;
}

CPetSkillProto* CGrowthPetManager::GetGrowthPetSkill(DWORD dwPetVnum, DWORD dwSkillVnum)
{
	auto petIterator = m_growthPetSkillMap.find(dwPetVnum);

	if (petIterator == m_growthPetSkillMap.end())
		return NULL;

	auto skillIterator = petIterator->second.find(dwSkillVnum);
	if (skillIterator == petIterator->second.end())
		return NULL;

	return skillIterator->second;
}

#endif // ENABLE_GROWTH_PET_SYSTEM
