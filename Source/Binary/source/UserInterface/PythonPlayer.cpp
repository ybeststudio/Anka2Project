#include "StdAfx.h"
#include "PythonPlayerEventHandler.h"
#include "PythonApplication.h"
#include "PythonItem.h"
#include "../eterbase/Timer.h"

#include "AbstractPlayer.h"

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	#include "../EterPythonLib/PythonSlotWindow.h"
#endif

std::map<int, CGraphicImage*> m_kMap_iEmotionIndex_pkIconImage;

const DWORD c_iSkillIndex_Tongsol = 121;
const DWORD c_iSkillIndex_Combo = 122;
const DWORD c_iSkillIndex_Fishing = 123;
const DWORD c_iSkillIndex_Mining = 124;
const DWORD c_iSkillIndex_Making = 125;
const DWORD c_iSkillIndex_Language1 = 126;
const DWORD c_iSkillIndex_Language2 = 127;
const DWORD c_iSkillIndex_Language3 = 128;
const DWORD c_iSkillIndex_Polymorph = 129;
const DWORD c_iSkillIndex_Riding = 130;
const DWORD c_iSkillIndex_Summon = 131;

#ifdef ENABLE_GOLD_LIMIT
const long long POINT_MAGIC_NUMBER = 0xe73ac1da;
#else
const DWORD POINT_MAGIC_NUMBER = 0xe73ac1da;
#endif

#ifdef ENABLE_GOLD_LIMIT
void CPythonPlayer::SPlayerStatus::SetPoint(UINT ePoint, long long lPoint)
#else
void CPythonPlayer::SPlayerStatus::SetPoint(UINT ePoint, long lPoint)
#endif
{
	m_alPoint[ePoint]=lPoint ^ POINT_MAGIC_NUMBER;
}

#ifdef ENABLE_GOLD_LIMIT
long long CPythonPlayer::SPlayerStatus::GetPoint(UINT ePoint)
#else
long CPythonPlayer::SPlayerStatus::GetPoint(UINT ePoint)
#endif
{
	return m_alPoint[ePoint] ^ POINT_MAGIC_NUMBER;
}

bool CPythonPlayer::AffectIndexToSkillIndex(DWORD dwAffectIndex, DWORD * pdwSkillIndex)
{
	if (m_kMap_dwAffectIndexToSkillIndex.end() == m_kMap_dwAffectIndexToSkillIndex.find(dwAffectIndex))
		return false;

	*pdwSkillIndex = m_kMap_dwAffectIndexToSkillIndex[dwAffectIndex];
	return true;
}

bool CPythonPlayer::AffectIndexToSkillSlotIndex(UINT uAffect, DWORD* pdwSkillSlotIndex)
{
	DWORD dwSkillIndex=m_kMap_dwAffectIndexToSkillIndex[uAffect];

	return GetSkillSlotIndex(dwSkillIndex, pdwSkillSlotIndex);
}

bool CPythonPlayer::__GetPickedActorPtr(CInstanceBase** ppkInstPicked)
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	CInstanceBase* pkInstPicked=rkChrMgr.OLD_GetPickedInstancePtr();
	if (!pkInstPicked)
		return false;

	*ppkInstPicked=pkInstPicked;
	return true;
}

bool CPythonPlayer::__GetPickedActorID(DWORD* pdwActorID)
{
	CPythonCharacterManager& rkChrMgr=CPythonCharacterManager::Instance();
	return rkChrMgr.OLD_GetPickedInstanceVID(pdwActorID);
}

bool CPythonPlayer::__GetPickedItemID(DWORD* pdwItemID)
{
	CPythonItem& rkItemMgr=CPythonItem::Instance();
	return rkItemMgr.GetPickedItemID(pdwItemID);
}

bool CPythonPlayer::__GetPickedGroundPos(TPixelPosition* pkPPosPicked)
{
	CPythonBackground& rkBG=CPythonBackground::Instance();

	TPixelPosition kPPosPicked;
	if (rkBG.GetPickingPoint(pkPPosPicked))
	{
		pkPPosPicked->y=-pkPPosPicked->y;
		return true;
	}

	return false;
}

void CPythonPlayer::NEW_GetMainActorPosition(TPixelPosition* pkPPosActor)
{
	TPixelPosition kPPosMainActor;

	IAbstractPlayer& rkPlayer=IAbstractPlayer::GetSingleton();
	CInstanceBase * pInstance = rkPlayer.NEW_GetMainActorPtr();
	if (pInstance)
	{
		pInstance->NEW_GetPixelPosition(pkPPosActor);
	}
	else
	{
		CPythonApplication::Instance().GetCenterPosition(pkPPosActor);
	}
}



bool CPythonPlayer::RegisterEffect(DWORD dwEID, const char *c_szFileName, bool isCache)
{
	if (dwEID>=EFFECT_NUM)
		return false;

	CEffectManager& rkEftMgr=CEffectManager::Instance();
	rkEftMgr.RegisterEffect2(c_szFileName, &m_adwEffect[dwEID], isCache);
	return true;
}

void CPythonPlayer::NEW_ShowEffect(int dwEID, TPixelPosition kPPosDst)
{
	if (dwEID>=EFFECT_NUM)
		return;

	D3DXVECTOR3 kD3DVt3Pos(kPPosDst.x, -kPPosDst.y, kPPosDst.z);
	D3DXVECTOR3 kD3DVt3Dir(0.0f, 0.0f, 1.0f);

	CEffectManager& rkEftMgr=CEffectManager::Instance();
	rkEftMgr.CreateEffect(m_adwEffect[dwEID], kD3DVt3Pos, kD3DVt3Dir);
}

CInstanceBase* CPythonPlayer::NEW_FindActorPtr(DWORD dwVID)
{
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	return rkChrMgr.GetInstancePtr(dwVID);
}

CInstanceBase* CPythonPlayer::NEW_GetMainActorPtr()
{
	return NEW_FindActorPtr(m_dwMainCharacterIndex);
}

///////////////////////////////////////////////////////////////////////////////////////////

void CPythonPlayer::Update()
{
#ifdef __AUTO_HUNT__
	AutoHuntLoop();
#endif

	NEW_RefreshMouseWalkingDirection();

	CPythonPlayerEventHandler& rkPlayerEventHandler=CPythonPlayerEventHandler::GetSingleton();
	rkPlayerEventHandler.FlushVictimList();

	if (m_isDestPosition)
	{
		CInstanceBase * pInstance = NEW_GetMainActorPtr();
		if (pInstance)
		{
			TPixelPosition PixelPosition;
			pInstance->NEW_GetPixelPosition(&PixelPosition);

			if (abs(int(PixelPosition.x) - m_ixDestPos) + abs(int(PixelPosition.y) - m_iyDestPos) < 10000)
			{
				m_isDestPosition = FALSE;
			}
			else
			{
				if (CTimer::Instance().GetCurrentMillisecond() - m_iLastAlarmTime > 20000)
				{
					AlarmHaveToGo();
				}
			}
		}
	}

	if (m_isConsumingStamina)
	{
		float fElapsedTime = CTimer::Instance().GetElapsedSecond();
		m_fCurrentStamina -= (fElapsedTime * m_fConsumeStaminaPerSec);

		SetStatus(POINT_STAMINA, DWORD(m_fCurrentStamina));

		PyCallClassMemberFunc(m_ppyGameWindow, "RefreshStamina", Py_BuildValue("()"));
	}

	__Update_AutoAttack();
	__Update_NotifyGuildAreaEvent();
}

bool CPythonPlayer::__IsUsingChargeSkill()
{
	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return false;

	if (__CheckDashAffect(*pkInstMain))
		return true;

	if (MODE_USE_SKILL != m_eReservedMode)
		return false;

	if (m_dwSkillSlotIndexReserved >= SKILL_MAX_NUM)
		return false;

	TSkillInstance & rkSkillInst = m_playerStatus.aSkill[m_dwSkillSlotIndexReserved];

	CPythonSkill::TSkillData * pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(rkSkillInst.dwIndex, &pSkillData))
		return false;

	return pSkillData->IsChargeSkill() ? true : false;
}

void CPythonPlayer::__Update_AutoAttack()
{
	if (0 == m_dwAutoAttackTargetVID)
		return;

	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	// ÅºÈ¯°? ¾²°í ´?·Á°¡´Â µµÁß¿¡´Â ½ºÅµ
	if (__IsUsingChargeSkill())
		return;

	CInstanceBase* pkInstVictim=NEW_FindActorPtr(m_dwAutoAttackTargetVID);
	if (!pkInstVictim)
	{
		__ClearAutoAttackTargetActorID();
	}
	else
	{
		if (pkInstVictim->IsDead())
		{
			__ClearAutoAttackTargetActorID();
		}
		else if (pkInstMain->IsMountingHorse() && !pkInstMain->CanAttackHorseLevel())
		{
			__ClearAutoAttackTargetActorID();
		}
		else if (pkInstMain->IsAttackableInstance(*pkInstVictim))
		{
			if (pkInstMain->IsSleep())
			{
				//TraceError("SKIP_AUTO_ATTACK_IN_SLEEPING");
			}
			else
			{
				__ReserveClickActor(m_dwAutoAttackTargetVID);
			}
		}
	}
}

void CPythonPlayer::__Update_NotifyGuildAreaEvent()
{
	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (pkInstMain)
	{
		TPixelPosition kPixelPosition;
		pkInstMain->NEW_GetPixelPosition(&kPixelPosition);

		DWORD dwAreaID = CPythonMiniMap::Instance().GetGuildAreaID(
			ULONG(kPixelPosition.x), ULONG(kPixelPosition.y));

		if (dwAreaID != m_inGuildAreaID)
		{
			if (0xffffffff != dwAreaID)
			{
				PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_Guild_EnterGuildArea", Py_BuildValue("(i)", dwAreaID));
			}
			else
			{
				PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_Guild_ExitGuildArea", Py_BuildValue("(i)", dwAreaID));
			}

			m_inGuildAreaID = dwAreaID;
		}
	}
}

void CPythonPlayer::SetMainCharacterIndex(int iIndex)
{
	m_dwMainCharacterIndex = iIndex;

	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (pkInstMain)
	{
		CPythonPlayerEventHandler& rkPlayerEventHandler=CPythonPlayerEventHandler::GetSingleton();
		pkInstMain->SetEventHandler(&rkPlayerEventHandler);
	}
}

DWORD CPythonPlayer::GetMainCharacterIndex()
{
	return m_dwMainCharacterIndex;
}

bool CPythonPlayer::IsMainCharacterIndex(DWORD dwIndex)
{
	return (m_dwMainCharacterIndex == dwIndex);
}

DWORD CPythonPlayer::GetGuildID()
{
	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (!pkInstMain)
		return 0xffffffff;

	return pkInstMain->GetGuildID();
}

void CPythonPlayer::SetWeaponPower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower, DWORD dwAddPower)
{
	m_dwWeaponMinPower=dwMinPower;
	m_dwWeaponMaxPower=dwMaxPower;
	m_dwWeaponMinMagicPower=dwMinMagicPower;
	m_dwWeaponMaxMagicPower=dwMaxMagicPower;
	m_dwWeaponAddPower=dwAddPower;

	__UpdateBattleStatus();	
}

void CPythonPlayer::SetRace(DWORD dwRace)
{
	m_dwRace=dwRace;
}

DWORD CPythonPlayer::GetRace()
{
	return m_dwRace;
}

DWORD CPythonPlayer::__GetRaceStat()
{
	switch (GetRace())
	{
		case NPlayerData::MAIN_RACE_WARRIOR_M:
		case NPlayerData::MAIN_RACE_WARRIOR_W:
			return GetStatus(POINT_ST);
			break;
		case NPlayerData::MAIN_RACE_ASSASSIN_M:
		case NPlayerData::MAIN_RACE_ASSASSIN_W:
			return GetStatus(POINT_DX);
			break;
		case NPlayerData::MAIN_RACE_SURA_M:
		case NPlayerData::MAIN_RACE_SURA_W:
			return GetStatus(POINT_ST);
			break;
		case NPlayerData::MAIN_RACE_SHAMAN_M:
		case NPlayerData::MAIN_RACE_SHAMAN_W:
			return GetStatus(POINT_IQ);
			break;
	}
	return GetStatus(POINT_ST);
}

DWORD CPythonPlayer::__GetLevelAtk()
{
	return 2*GetStatus(POINT_LEVEL);
}

DWORD CPythonPlayer::__GetStatAtk()
{
	return (4*GetStatus(POINT_ST)+2*__GetRaceStat())/3;
}

DWORD CPythonPlayer::__GetWeaponAtk(DWORD dwWeaponPower)
{
	return 2*dwWeaponPower;
}

DWORD CPythonPlayer::__GetTotalAtk(DWORD dwWeaponPower, DWORD dwRefineBonus)
{
	DWORD dwLvAtk=__GetLevelAtk();
	DWORD dwStAtk=__GetStatAtk();

	DWORD dwWepAtk;
	DWORD dwTotalAtk;

	int hr = __GetHitRate();
	dwWepAtk = __GetWeaponAtk(dwWeaponPower+dwRefineBonus);
	dwTotalAtk = dwLvAtk+(dwStAtk+dwWepAtk)*hr/100;

	return dwTotalAtk;
}

DWORD CPythonPlayer::__GetHitRate()
{
	int src = 0;
	src = (GetStatus (POINT_DX) * 4 + GetStatus (POINT_LEVEL) * 2) / 6;
	return 100*(min(90, src)+210)/300;
}

DWORD CPythonPlayer::__GetEvadeRate()
{
	return 30*(2*GetStatus(POINT_DX)+5)/(GetStatus(POINT_DX)+95);
} 

void CPythonPlayer::__UpdateBattleStatus()
{
	m_playerStatus.SetPoint(POINT_NONE, 0);
	m_playerStatus.SetPoint(POINT_EVADE_RATE, __GetEvadeRate());
	m_playerStatus.SetPoint(POINT_HIT_RATE, __GetHitRate());
	m_playerStatus.SetPoint(POINT_MIN_WEP, m_dwWeaponMinPower+m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MAX_WEP, m_dwWeaponMaxPower+m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MIN_MAGIC_WEP, m_dwWeaponMinMagicPower+m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MAX_MAGIC_WEP, m_dwWeaponMaxMagicPower+m_dwWeaponAddPower);
	m_playerStatus.SetPoint(POINT_MIN_ATK, __GetTotalAtk(m_dwWeaponMinPower, m_dwWeaponAddPower));
	m_playerStatus.SetPoint(POINT_MAX_ATK, __GetTotalAtk(m_dwWeaponMaxPower, m_dwWeaponAddPower));	
}

#ifdef ENABLE_GOLD_LIMIT
void CPythonPlayer::SetStatus(DWORD dwType, long long lValue)
#else
void CPythonPlayer::SetStatus(DWORD dwType, long lValue)
#endif
{
	if (dwType >= POINT_MAX_NUM)
	{
		assert(!" CPythonPlayer::SetStatus - Strange Status Type!");
		Tracef("CPythonPlayer::SetStatus - Set Status Type Error\n");
		return;
	}

	if (dwType == POINT_LEVEL)
	{
		CInstanceBase* pkPlayer = NEW_GetMainActorPtr();

		if (pkPlayer)
		{
			pkPlayer->UpdateTextTailLevel(lValue);
			pkPlayer->SetLevel(lValue);
		}

	}

	switch (dwType)
	{
		case POINT_MIN_WEP:
		case POINT_MAX_WEP:
		case POINT_MIN_ATK:
		case POINT_MAX_ATK:
		case POINT_HIT_RATE:
		case POINT_EVADE_RATE:
		case POINT_LEVEL:
		case POINT_ST:
		case POINT_DX:
		case POINT_IQ:
			m_playerStatus.SetPoint(dwType, lValue);
			__UpdateBattleStatus();
			break;
		default:
			m_playerStatus.SetPoint(dwType, lValue);
			break;
	}		
}

#ifdef ENABLE_GOLD_LIMIT
long long CPythonPlayer::GetStatus(DWORD dwType)
#else
int CPythonPlayer::GetStatus(DWORD dwType)
#endif
{
	if (dwType >= POINT_MAX_NUM)
	{
		assert(!" CPythonPlayer::GetStatus - Strange Status Type!");
		Tracef("CPythonPlayer::GetStatus - Get Status Type Error\n");
		return 0;
	}

	return m_playerStatus.GetPoint(dwType);
}

const char *CPythonPlayer::GetName()
{
	return m_stName.c_str();
}

void CPythonPlayer::SetName(const char *name)
{
	m_stName = name;
}

void CPythonPlayer::NotifyDeletingCharacterInstance(DWORD dwVID)
{
	if (m_dwMainCharacterIndex == dwVID)
		m_dwMainCharacterIndex = 0;
}

void CPythonPlayer::NotifyCharacterDead(DWORD dwVID)
{
	if (__IsSameTargetVID(dwVID))
	{
		SetTarget(0);
	}
}

void CPythonPlayer::NotifyCharacterUpdate(DWORD dwVID)
{
	if (__IsSameTargetVID(dwVID))
	{
		CInstanceBase * pMainInstance = NEW_GetMainActorPtr();
		CInstanceBase * pTargetInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwVID);
		if (pMainInstance && pTargetInstance)
		{
			if (!pMainInstance->IsTargetableInstance(*pTargetInstance))
			{
				SetTarget(0);
				PyCallClassMemberFunc(m_ppyGameWindow, "CloseTargetBoard", Py_BuildValue("()"));
			}
			else
			{
				PyCallClassMemberFunc(m_ppyGameWindow, "RefreshTargetBoardByVID", Py_BuildValue("(i)", dwVID));
			}
		}
	}
}

void CPythonPlayer::NotifyDeadMainCharacter()
{
	__ClearAutoAttackTargetActorID();
	ClearSkillToggle(0);
}

void CPythonPlayer::NotifyChangePKMode()
{
	PyCallClassMemberFunc(m_ppyGameWindow, "OnChangePKMode", Py_BuildValue("()"));
}


void CPythonPlayer::MoveItemData(TItemPos SrcCell, TItemPos DstCell)
{
	if (!SrcCell.IsValidCell() || !DstCell.IsValidCell())
		return;

	TItemData src_item(*GetItemData(SrcCell));
	TItemData dst_item(*GetItemData(DstCell));
	SetItemData(DstCell, src_item);
	SetItemData(SrcCell, dst_item);
}

const TItemData * CPythonPlayer::GetItemData(TItemPos Cell) const
{
	if (!Cell.IsValidCell())
		return NULL;

	switch (Cell.window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		return &m_playerStatus.aItem[Cell.cell];

	case DRAGON_SOUL_INVENTORY:
		return &m_playerStatus.aDSItem[Cell.cell];

#ifdef ENABLE_RENEWAL_SWITCHBOT
	case SWITCHBOT:
		return &m_playerStatus.aSwitchbotItem[Cell.cell];
#endif

	default:
		return NULL;
	}
}

void CPythonPlayer::SetItemData(TItemPos Cell, const TItemData & c_rkItemInst)
{
	if (!Cell.IsValidCell())
		return;

	if (c_rkItemInst.vnum != 0)
	{
		CItemData * pItemData;
		if (!CItemManager::Instance().GetItemDataPointer(c_rkItemInst.vnum, &pItemData))
		{
			TraceError("CPythonPlayer::SetItemData(window_type : %d, dwSlotIndex=%d, itemIndex=%d) - Failed to item data\n", Cell.window_type, Cell.cell, c_rkItemInst.vnum);
			return;
		}
	}

	switch (Cell.window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		m_playerStatus.aItem[Cell.cell] = c_rkItemInst;
		break;

	case DRAGON_SOUL_INVENTORY:
		m_playerStatus.aDSItem[Cell.cell] = c_rkItemInst;
		break;

#ifdef ENABLE_RENEWAL_SWITCHBOT
	case SWITCHBOT:
		m_playerStatus.aSwitchbotItem[Cell.cell] = c_rkItemInst;
		break;
#endif

	}
}

DWORD CPythonPlayer::GetItemIndex(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	return GetItemData(Cell)->vnum;
}

int CPythonPlayer::GetItemSlotIndex(DWORD dwVnum)
{
	for (int i = 0; i < c_Inventory_Count; ++i)
	{
		const TItemData& c_rItemData = m_playerStatus.aItem[i];
		if (c_rItemData.vnum == dwVnum)
			return i;
	}

	return -1;
}

DWORD CPythonPlayer::GetItemFlags(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;
	const TItemData * pItem = GetItemData(Cell);
	assert (pItem != nullptr);
	return pItem->flags;
}

DWORD CPythonPlayer::GetItemAntiFlags(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	const TItemData* pItem = GetItemData(Cell);
	assert(pItem != nullptr);
	return pItem->anti_flags;
}

BYTE CPythonPlayer::GetItemTypeBySlot(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	CItemData * pItemDataPtr = nullptr;
	if (CItemManager::Instance().GetItemDataPointer(GetItemIndex(Cell), &pItemDataPtr))
		return pItemDataPtr->GetType();
	else
	{
		TraceError("FAILED\t\tCPythonPlayer::GetItemTypeBySlot()\t\tCell(%d, %d) item is null", Cell.window_type, Cell.cell);
		return 0;
	}
}

BYTE CPythonPlayer::GetItemSubTypeBySlot(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	CItemData* pItemDataPtr = nullptr;
	if (CItemManager::Instance().GetItemDataPointer(GetItemIndex(Cell), &pItemDataPtr))
		return pItemDataPtr->GetSubType();
	else
	{
		TraceError("FAILED\t\tCPythonPlayer::GetItemSubTypeBySlot()\t\tCell(%d, %d) item is null", Cell.window_type, Cell.cell);
		return 0;
	}
}

DWORD CPythonPlayer::GetItemCount(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;
	const TItemData * pItem = GetItemData(Cell);
	if (pItem == nullptr)
		return 0;
	else
		return pItem->count;
}

DWORD CPythonPlayer::GetItemCountByVnum(DWORD dwVnum)
{
	DWORD dwCount = 0;

	for (int i = 0; i < c_Inventory_Count; ++i)
	{
		const TItemData & c_rItemData = m_playerStatus.aItem[i];
		if (c_rItemData.vnum == dwVnum)
		{
			dwCount += c_rItemData.count;
		}
	}

	return dwCount;
}

DWORD CPythonPlayer::GetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex)
{
	if (!Cell.IsValidCell())
		return 0;

	if (dwMetinSocketIndex >= ITEM_SOCKET_SLOT_MAX_NUM)
		return 0;

	return GetItemData(Cell)->alSockets[dwMetinSocketIndex];
}

void CPythonPlayer::GetItemAttribute(TItemPos Cell, DWORD dwAttrSlotIndex, BYTE * pbyType, short * psValue)
{
	*pbyType = 0;
	*psValue = 0;

	if (!Cell.IsValidCell())
		return;

	if (dwAttrSlotIndex >= ITEM_ATTRIBUTE_SLOT_MAX_NUM)
		return;

	*pbyType = GetItemData(Cell)->aAttr[dwAttrSlotIndex].bType;
	*psValue = GetItemData(Cell)->aAttr[dwAttrSlotIndex].sValue;
}

#ifdef ENABLE_STACK_LIMIT
void CPythonPlayer::SetItemCount(TItemPos Cell, WORD byCount)
#else
void CPythonPlayer::SetItemCount(TItemPos Cell, BYTE byCount)
#endif
{
	if (!Cell.IsValidCell())
		return;

	(const_cast <TItemData *>(GetItemData(Cell)))->count = byCount;
	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

void CPythonPlayer::SetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex, DWORD dwMetinNumber)
{
	if (!Cell.IsValidCell())
		return;
	if (dwMetinSocketIndex >= ITEM_SOCKET_SLOT_MAX_NUM)
		return;

	(const_cast <TItemData *>(GetItemData(Cell)))->alSockets[dwMetinSocketIndex] = dwMetinNumber;
}

void CPythonPlayer::SetItemAttribute(TItemPos Cell, DWORD dwAttrIndex, BYTE byType, short sValue)
{
	if (!Cell.IsValidCell())
		return;
	if (dwAttrIndex >= ITEM_ATTRIBUTE_SLOT_MAX_NUM)
		return;

	(const_cast <TItemData *>(GetItemData(Cell)))->aAttr[dwAttrIndex].bType = byType;
	(const_cast <TItemData *>(GetItemData(Cell)))->aAttr[dwAttrIndex].sValue = sValue;
}

int CPythonPlayer::GetQuickPage()
{
	return m_playerStatus.lQuickPageIndex;
}

void CPythonPlayer::SetQuickPage(int nQuickPageIndex)
{
	if (nQuickPageIndex<0)
		m_playerStatus.lQuickPageIndex=QUICKSLOT_MAX_LINE+nQuickPageIndex;	
	else if (nQuickPageIndex>=QUICKSLOT_MAX_LINE)
		m_playerStatus.lQuickPageIndex=nQuickPageIndex%QUICKSLOT_MAX_LINE;	
	else
		m_playerStatus.lQuickPageIndex=nQuickPageIndex;	

	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

DWORD	CPythonPlayer::LocalQuickSlotIndexToGlobalQuickSlotIndex(DWORD dwLocalSlotIndex)
{
	return m_playerStatus.lQuickPageIndex*QUICKSLOT_MAX_COUNT_PER_LINE+dwLocalSlotIndex;	
}

void	CPythonPlayer::GetGlobalQuickSlotData(DWORD dwGlobalSlotIndex, DWORD* pdwWndType, DWORD* pdwWndItemPos)
{
	TQuickSlot& rkQuickSlot=__RefGlobalQuickSlot(dwGlobalSlotIndex);
	*pdwWndType=rkQuickSlot.Type;
	*pdwWndItemPos=rkQuickSlot.Position;
}

void	CPythonPlayer::GetLocalQuickSlotData(DWORD dwSlotPos, DWORD* pdwWndType, DWORD* pdwWndItemPos)
{
	TQuickSlot& rkQuickSlot=__RefLocalQuickSlot(dwSlotPos);
	*pdwWndType=rkQuickSlot.Type;
	*pdwWndItemPos=rkQuickSlot.Position;
}

TQuickSlot & CPythonPlayer::__RefLocalQuickSlot(int SlotIndex)
{
	return __RefGlobalQuickSlot(LocalQuickSlotIndexToGlobalQuickSlotIndex(SlotIndex));
}

TQuickSlot & CPythonPlayer::__RefGlobalQuickSlot(int SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= QUICKSLOT_MAX_NUM)
	{
		static TQuickSlot s_kQuickSlot;
		s_kQuickSlot.Type = 0;
		s_kQuickSlot.Position = 0;
		return s_kQuickSlot;
	}

	return m_playerStatus.aQuickSlot[SlotIndex];
}

void CPythonPlayer::RemoveQuickSlotByValue(int iType, int iPosition)
{
	for (BYTE i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		if (iType == m_playerStatus.aQuickSlot[i].Type)
			if (iPosition == m_playerStatus.aQuickSlot[i].Position)
				CPythonNetworkStream::Instance().SendQuickSlotDelPacket(i);
	}
}

char CPythonPlayer::IsItem(TItemPos Cell)
{
	if (!Cell.IsValidCell())
		return 0;

	return 0 != GetItemData(Cell)->vnum;
}

void CPythonPlayer::RequestMoveGlobalQuickSlotToLocalQuickSlot(DWORD dwGlobalSrcSlotIndex, DWORD dwLocalDstSlotIndex)
{
	//DWORD dwGlobalSrcSlotIndex=LocalQuickSlotIndexToGlobalQuickSlotIndex(dwLocalSrcSlotIndex);
	DWORD dwGlobalDstSlotIndex=LocalQuickSlotIndexToGlobalQuickSlotIndex(dwLocalDstSlotIndex);

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendQuickSlotMovePacket((BYTE) dwGlobalSrcSlotIndex, (BYTE)dwGlobalDstSlotIndex);
}

void CPythonPlayer::RequestAddLocalQuickSlot(DWORD dwLocalSlotIndex, DWORD dwWndType, DWORD dwWndItemPos)
{
	if (dwLocalSlotIndex>=QUICKSLOT_MAX_COUNT_PER_LINE)
		return;

	DWORD dwGlobalSlotIndex=LocalQuickSlotIndexToGlobalQuickSlotIndex(dwLocalSlotIndex);

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendQuickSlotAddPacket((BYTE)dwGlobalSlotIndex, (BYTE)dwWndType, (BYTE)dwWndItemPos);
}

void CPythonPlayer::RequestAddToEmptyLocalQuickSlot(DWORD dwWndType, DWORD dwWndItemPos)
{
    for (int i = 0; i < QUICKSLOT_MAX_COUNT_PER_LINE; ++i)
    {
        TQuickSlot& rkQuickSlot=__RefLocalQuickSlot(i);

        if (0 == rkQuickSlot.Type)
        {
            DWORD dwGlobalQuickSlotIndex=LocalQuickSlotIndexToGlobalQuickSlotIndex(i);
            CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
            rkNetStream.SendQuickSlotAddPacket((BYTE)dwGlobalQuickSlotIndex, (BYTE)dwWndType, (BYTE)dwWndItemPos);
            return;
        }
    }

}

void CPythonPlayer::RequestDeleteGlobalQuickSlot(DWORD dwGlobalSlotIndex)
{
	if (dwGlobalSlotIndex>=QUICKSLOT_MAX_COUNT)
		return;

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendQuickSlotDelPacket((BYTE)dwGlobalSlotIndex);
}

void CPythonPlayer::RequestUseLocalQuickSlot(DWORD dwLocalSlotIndex)
{
	if (dwLocalSlotIndex>=QUICKSLOT_MAX_COUNT_PER_LINE)
		return;

	DWORD dwRegisteredType;
	DWORD dwRegisteredItemPos;
	GetLocalQuickSlotData(dwLocalSlotIndex, &dwRegisteredType, &dwRegisteredItemPos);

	switch (dwRegisteredType)
	{
		case SLOT_TYPE_INVENTORY:
#ifdef ENABLE_SPECIAL_INVENTORY
		case SLOT_TYPE_SKILL_BOOK_INVENTORY:
		case SLOT_TYPE_UPGRADE_ITEMS_INVENTORY:
		case SLOT_TYPE_STONE_INVENTORY:
		case SLOT_TYPE_GIFT_BOX_INVENTORY:
		case SLOT_TYPE_CHANGERS_INVENTORY:
#endif
		{
			CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
			rkNetStream.SendItemUsePacket(TItemPos(INVENTORY, (WORD)dwRegisteredItemPos));
			break;
		}
		case SLOT_TYPE_SKILL:
		{
			ClickSkillSlot(dwRegisteredItemPos);
			break;
		}
		case SLOT_TYPE_EMOTION:
		{
			PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_ActEmotion", Py_BuildValue("(i)", dwRegisteredItemPos));
			break;
		}
	}
}

void CPythonPlayer::AddQuickSlot(int QuickSlotIndex, char IconType, char IconPosition)
{
	if (QuickSlotIndex < 0 || QuickSlotIndex >= QUICKSLOT_MAX_NUM)
		return;

	m_playerStatus.aQuickSlot[QuickSlotIndex].Type = IconType;
	m_playerStatus.aQuickSlot[QuickSlotIndex].Position = IconPosition;
}

void CPythonPlayer::DeleteQuickSlot(int QuickSlotIndex)
{
	if (QuickSlotIndex < 0 || QuickSlotIndex >= QUICKSLOT_MAX_NUM)
		return;

	m_playerStatus.aQuickSlot[QuickSlotIndex].Type = 0;
	m_playerStatus.aQuickSlot[QuickSlotIndex].Position = 0;
}

void CPythonPlayer::MoveQuickSlot(int Source, int Target)
{
	if (Source < 0 || Source >= QUICKSLOT_MAX_NUM)
		return;

	if (Target < 0 || Target >= QUICKSLOT_MAX_NUM)
		return;

	TQuickSlot& rkSrcSlot=__RefGlobalQuickSlot(Source);
	TQuickSlot& rkDstSlot=__RefGlobalQuickSlot(Target);

	std::swap(rkSrcSlot, rkDstSlot);
}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
bool CPythonPlayer::IsBeltInventorySlot(TItemPos Cell)
{
	return Cell.IsBeltInventoryCell();
}
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
bool CPythonPlayer::IsSkillBookInventorySlot(TItemPos Cell)
{
	return Cell.IsSkillBookInventoryCell();
}

bool CPythonPlayer::IsUpgradeItemsInventorySlot(TItemPos Cell)
{
	return Cell.IsUpgradeItemsInventoryCell();
}

bool CPythonPlayer::IsStoneInventorySlot(TItemPos Cell)
{
	return Cell.IsStoneInventoryCell();
}

bool CPythonPlayer::IsGiftBoxInventorySlot(TItemPos Cell)
{
	return Cell.IsGiftBoxInventoryCell();
}

bool CPythonPlayer::IsChangersInventorySlot(TItemPos Cell)
{
	return Cell.IsChangersInventoryCell();
}
#endif

bool CPythonPlayer::IsInventorySlot(TItemPos Cell)
{
	return !Cell.IsEquipCell() && Cell.IsValidCell();
}

bool CPythonPlayer::IsEquipmentSlot(TItemPos Cell)
{
	return Cell.IsEquipCell();
}

bool CPythonPlayer::IsEquipItemInSlot(TItemPos Cell)
{
	if (!Cell.IsEquipCell())
	{
		return false;
	}

	const TItemData * pData = GetItemData(Cell);
	
	if (NULL == pData)
	{
		return false;
	}

	DWORD dwItemIndex = pData->vnum;

	CItemManager::Instance().SelectItemData(dwItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
	{
		TraceError("Failed to find ItemData - CPythonPlayer::IsEquipItem(window_type=%d, iSlotindex=%d)\n", Cell.window_type, Cell.cell);
		return false;
	}

	return pItemData->IsEquipment() ? true : false;
}


void CPythonPlayer::SetSkill(DWORD dwSlotIndex, DWORD dwSkillIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return;

	m_playerStatus.aSkill[dwSlotIndex].dwIndex = dwSkillIndex;
	m_skillSlotDict[dwSkillIndex] = dwSlotIndex;
}

int CPythonPlayer::GetSkillIndex(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].dwIndex;
}

bool CPythonPlayer::GetSkillSlotIndex(DWORD dwSkillIndex, DWORD* pdwSlotIndex)
{
	std::map<DWORD, DWORD>::iterator f=m_skillSlotDict.find(dwSkillIndex);
	if (m_skillSlotDict.end()==f)
	{
		return false;
	}

	*pdwSlotIndex=f->second;

	return true;
}

int CPythonPlayer::GetSkillGrade(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].iGrade;
}

int CPythonPlayer::GetSkillLevel(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].iLevel;
}

float CPythonPlayer::GetSkillCurrentEfficientPercentage(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].fcurEfficientPercentage;
}

float CPythonPlayer::GetSkillNextEfficientPercentage(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0;

	return m_playerStatus.aSkill[dwSlotIndex].fnextEfficientPercentage;
}

void CPythonPlayer::SetSkillLevel(DWORD dwSlotIndex, DWORD dwSkillLevel)
{
	assert(!"CPythonPlayer::SetSkillLevel - »ç¿ëÇÏÁö ¾Ê´Â ÇÔ¼ö");
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return;

	m_playerStatus.aSkill[dwSlotIndex].iGrade = -1;
	m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel;
}

void CPythonPlayer::SetSkillLevel_(DWORD dwSkillIndex, DWORD dwSkillGrade, DWORD dwSkillLevel)
{
	DWORD dwSlotIndex;
	if (!GetSkillSlotIndex(dwSkillIndex, &dwSlotIndex))
		return;

	if (dwSlotIndex >= SKILL_MAX_NUM)
		return;

	switch (dwSkillGrade)
	{
		case 0:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel;
			break;
		case 1:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel-20+1;
			break;
		case 2:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel-30+1;
			break;
		case 3:
			m_playerStatus.aSkill[dwSlotIndex].iGrade = dwSkillGrade;
			m_playerStatus.aSkill[dwSlotIndex].iLevel = dwSkillLevel-40+1;
			break;
	}

	const DWORD SKILL_MAX_LEVEL = 40;





	if (dwSkillLevel>SKILL_MAX_LEVEL)
	{
		m_playerStatus.aSkill[dwSlotIndex].fcurEfficientPercentage = 0.0f;
		m_playerStatus.aSkill[dwSlotIndex].fnextEfficientPercentage = 0.0f;

		TraceError("CPythonPlayer::SetSkillLevel(SlotIndex=%d, SkillLevel=%d)", dwSlotIndex, dwSkillLevel);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].fcurEfficientPercentage	= GetSkillPower(dwSkillLevel)/100.0f;
	m_playerStatus.aSkill[dwSlotIndex].fnextEfficientPercentage = GetSkillPower(dwSkillLevel+1)/100.0f;

}

void CPythonPlayer::SetSkillCoolTime(DWORD dwSkillIndex)
{
	DWORD dwSlotIndex;
	if (!GetSkillSlotIndex(dwSkillIndex, &dwSlotIndex))
	{
		Tracenf("CPythonPlayer::SetSkillCoolTime(dwSkillIndex=%d) - FIND SLOT ERROR", dwSkillIndex);
		return;
	}

	if (dwSlotIndex>=SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::SetSkillCoolTime(dwSkillIndex=%d) - dwSlotIndex=%d/%d OUT OF RANGE", dwSkillIndex, dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].isCoolTime=true;
}

void CPythonPlayer::EndSkillCoolTime(DWORD dwSkillIndex)
{
	DWORD dwSlotIndex;
	if (!GetSkillSlotIndex(dwSkillIndex, &dwSlotIndex))
	{
		Tracenf("CPythonPlayer::EndSkillCoolTime(dwSkillIndex=%d) - FIND SLOT ERROR", dwSkillIndex);
		return;
	}

	if (dwSlotIndex>=SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::EndSkillCoolTime(dwSkillIndex=%d) - dwSlotIndex=%d/%d OUT OF RANGE", dwSkillIndex, dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].isCoolTime=false;
}

float CPythonPlayer::GetSkillCoolTime(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0.0f;

	return m_playerStatus.aSkill[dwSlotIndex].fCoolTime;
}

float CPythonPlayer::GetSkillElapsedCoolTime(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return 0.0f;

	return CTimer::Instance().GetCurrentSecond() - m_playerStatus.aSkill[dwSlotIndex].fLastUsedTime;
}

void CPythonPlayer::__ActivateSkillSlot(DWORD dwSlotIndex)
{
	if (dwSlotIndex>=SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::ActivavteSkill(dwSlotIndex=%d/%d) - OUT OF RANGE", dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].bActive = TRUE;
	PyCallClassMemberFunc(m_ppyGameWindow, "ActivateSkillSlot", Py_BuildValue("(i)", dwSlotIndex));
}

void CPythonPlayer::__DeactivateSkillSlot(DWORD dwSlotIndex)
{
	if (dwSlotIndex>=SKILL_MAX_NUM)
	{
		Tracenf("CPythonPlayer::DeactivavteSkill(dwSlotIndex=%d/%d) - OUT OF RANGE", dwSlotIndex, SKILL_MAX_NUM);
		return;
	}

	m_playerStatus.aSkill[dwSlotIndex].bActive = FALSE;
	PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSkillSlot", Py_BuildValue("(i)", dwSlotIndex));
}

BOOL CPythonPlayer::IsSkillCoolTime(DWORD dwSlotIndex)
{
	if (!__CheckRestSkillCoolTime(dwSlotIndex))
		return FALSE;

	return TRUE;
}

BOOL CPythonPlayer::IsSkillActive(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return FALSE;

	return m_playerStatus.aSkill[dwSlotIndex].bActive;
}

BOOL CPythonPlayer::IsToggleSkill(DWORD dwSlotIndex)
{
	if (dwSlotIndex >= SKILL_MAX_NUM)
		return FALSE;

	DWORD dwSkillIndex = m_playerStatus.aSkill[dwSlotIndex].dwIndex;

	CPythonSkill::TSkillData * pSkillData;
	if (!CPythonSkill::Instance().GetSkillData(dwSkillIndex, &pSkillData))
		return FALSE;

	return pSkillData->IsToggleSkill();
}

void CPythonPlayer::SetPlayTime(DWORD dwPlayTime)
{
	m_dwPlayTime = dwPlayTime;
}

DWORD CPythonPlayer::GetPlayTime()
{
	return m_dwPlayTime;
}

void CPythonPlayer::SendClickItemPacket(DWORD dwIID)
{
	if (IsObserverMode())
		return;

#ifndef ENABLE_INSTANT_PICKUP
	static DWORD s_dwNextTCPTime = 0;

	DWORD dwCurTime=ELTimer_GetMSec();

	if (dwCurTime >= s_dwNextTCPTime)
	{
		s_dwNextTCPTime=dwCurTime + 500;
#endif

		const char *c_szOwnerName;
		if (!CPythonItem::Instance().GetOwnership(dwIID, &c_szOwnerName))
			return;

		if (strlen(c_szOwnerName) > 0)
		if (0 != strcmp(c_szOwnerName, GetName()))
		{
			CItemData * pItemData;
			if (!CItemManager::Instance().GetItemDataPointer(CPythonItem::Instance().GetVirtualNumberOfGroundItem(dwIID), &pItemData))
			{
				Tracenf("CPythonPlayer::SendClickItemPacket(dwIID=%d) : Non-exist item.", dwIID);
				return;
			}
			if (!IsPartyMemberByName(c_szOwnerName) || pItemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_DROP | CItemData::ITEM_ANTIFLAG_GIVE))
			{
				PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotPickItem", Py_BuildValue("()"));
				return;
			}
		}

		CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
		rkNetStream.SendItemPickUpPacket(dwIID);

#ifndef ENABLE_INSTANT_PICKUP
	}
#endif
}

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
void CPythonPlayer::SendPickUPItemPacket(std::vector<DWORD> itemVidList)
{
	if (IsObserverMode())
		return;

	static uint32_t s_dwNextTCPTime = 0;
	uint32_t dwCurTime = ELTimer_GetMSec();
	std::vector<DWORD> itemVector;

	if (dwCurTime >= s_dwNextTCPTime)
	{
		s_dwNextTCPTime = dwCurTime + 500;
		for (size_t i = 0; i < itemVidList.size(); i++)
		{
			DWORD dwIID = itemVidList[i];
			const char * c_szOwnerName;
			if (!CPythonItem::Instance().GetOwnership(dwIID, &c_szOwnerName))
				continue;

			if (strlen(c_szOwnerName) > 0)
			{
				if (0 != strcmp(c_szOwnerName, GetName()))
				{
					CItemData * pItemData;
					if (!CItemManager::Instance().GetItemDataPointer(CPythonItem::Instance().GetVirtualNumberOfGroundItem(dwIID), &pItemData))
						continue;

					if (!IsPartyMemberByName(c_szOwnerName) || pItemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_DROP | CItemData::ITEM_ANTIFLAG_GIVE))
						continue;
				}
			}
			itemVector.push_back(dwIID);
		}
		if (itemVector.empty())
			return;

		CPythonNetworkStream & rkNetStream = CPythonNetworkStream::Instance();
		rkNetStream.SendItemsPickUpPacket(itemVector);
	}
}
#endif

void CPythonPlayer::__SendClickActorPacket(CInstanceBase& rkInstVictim)
{
	// ¸»À» Å¸°í ±¤»êÀ» Ä³´Â °Í¿¡ ´ëÇÑ ¿¹¿Ü Ã³¸®
	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (pkInstMain)
	if (pkInstMain->IsHoldingPickAxe())
	if (pkInstMain->IsMountingHorse())
	if (rkInstVictim.IsResource())
	{
		PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotMining", Py_BuildValue("()"));
		return;
	}

	static DWORD s_dwNextTCPTime = 0;

	DWORD dwCurTime=ELTimer_GetMSec();

	if (dwCurTime >= s_dwNextTCPTime)
	{
		s_dwNextTCPTime=dwCurTime+1000;

		CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();

		DWORD dwVictimVID=rkInstVictim.GetVirtualID();
		rkNetStream.SendOnClickPacket(dwVictimVID);
	}
}

void CPythonPlayer::ActEmotion(DWORD dwEmotionID)
{
	CInstanceBase * pkInstTarget = __GetAliveTargetInstancePtr();
	if (!pkInstTarget)
	{
		PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotShotError", Py_BuildValue("(is)", GetMainCharacterIndex(), "NEED_TARGET"));
		return;
	}

	CPythonNetworkStream::Instance().SendChatPacket(_getf("/kiss %s", pkInstTarget->GetNameString()));
}

void CPythonPlayer::StartEmotionProcess()
{
	__ClearReservedAction();
	__ClearAutoAttackTargetActorID();

	m_bisProcessingEmotion = TRUE;
}

void CPythonPlayer::EndEmotionProcess()
{
	m_bisProcessingEmotion = FALSE;
}

BOOL CPythonPlayer::__IsProcessingEmotion()
{
	return m_bisProcessingEmotion;
}

// Dungeon
void CPythonPlayer::SetDungeonDestinationPosition(int ix, int iy)
{
	m_isDestPosition = TRUE;
	m_ixDestPos = ix;
	m_iyDestPos = iy;

	AlarmHaveToGo();
}

void CPythonPlayer::AlarmHaveToGo()
{
	m_iLastAlarmTime = CTimer::Instance().GetCurrentMillisecond();

	/////

	CInstanceBase * pInstance = NEW_GetMainActorPtr();
	if (!pInstance)
		return;

	TPixelPosition PixelPosition;
	pInstance->NEW_GetPixelPosition(&PixelPosition);

	float fAngle = GetDegreeFromPosition2(PixelPosition.x, PixelPosition.y, float(m_ixDestPos), float(m_iyDestPos));
	fAngle = fmod(540.0f - fAngle, 360.0f);
	D3DXVECTOR3 v3Rotation(0.0f, 0.0f, fAngle);

	PixelPosition.y *= -1.0f;

	CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_middle.mse");
	CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_middle.mse", PixelPosition, v3Rotation);
}

// Party
void CPythonPlayer::ExitParty()
{
	m_PartyMemberMap.clear();

	CPythonCharacterManager::Instance().RefreshAllPCTextTail();
}

void CPythonPlayer::AppendPartyMember(DWORD dwPID, const char *c_szName)
{
	m_PartyMemberMap.insert(std::make_pair(dwPID, TPartyMemberInfo(dwPID, c_szName)));

#ifdef ENABLE_PARTY_POSITION
	if (strlen(c_szName) == 0)
		CPythonMiniMap::Instance().RemovePartyPositionInfo(dwPID);
#endif
}

void CPythonPlayer::LinkPartyMember(DWORD dwPID, DWORD dwVID)
{
	TPartyMemberInfo * pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::LinkPartyMember(dwPID=%d, dwVID=%d) - Failed to find party member", dwPID, dwVID);
		return;
	}

	pPartyMemberInfo->dwVID = dwVID;

	CInstanceBase * pInstance = NEW_FindActorPtr(dwVID);
	if (pInstance)
		pInstance->RefreshTextTail();
}

void CPythonPlayer::UnlinkPartyMember(DWORD dwPID)
{
	TPartyMemberInfo * pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::UnlinkPartyMember(dwPID=%d) - Failed to find party member", dwPID);
		return;
	}

	pPartyMemberInfo->dwVID = 0;
}

void CPythonPlayer::UpdatePartyMemberInfo(DWORD dwPID, BYTE byState, BYTE byHPPercentage)
{
	TPartyMemberInfo * pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::UpdatePartyMemberInfo(dwPID=%d, byState=%d, byHPPercentage=%d) - Failed to find character", dwPID, byState, byHPPercentage);
		return;
	}

	pPartyMemberInfo->byState = byState;
	pPartyMemberInfo->byHPPercentage = byHPPercentage;
}

void CPythonPlayer::UpdatePartyMemberAffect(DWORD dwPID, BYTE byAffectSlotIndex, short sAffectNumber)
{
	if (byAffectSlotIndex >= PARTY_AFFECT_SLOT_MAX_NUM)
	{
		TraceError(" CPythonPlayer::UpdatePartyMemberAffect(dwPID=%d, byAffectSlotIndex=%d, sAffectNumber=%d) - Strange affect slot index", dwPID, byAffectSlotIndex, sAffectNumber);
		return;
	}

	TPartyMemberInfo * pPartyMemberInfo;
	if (!GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		TraceError(" CPythonPlayer::UpdatePartyMemberAffect(dwPID=%d, byAffectSlotIndex=%d, sAffectNumber=%d) - Failed to find character", dwPID, byAffectSlotIndex, sAffectNumber);
		return;
	}

	pPartyMemberInfo->sAffects[byAffectSlotIndex] = sAffectNumber;
}

void CPythonPlayer::RemovePartyMember(DWORD dwPID)
{
	DWORD dwVID = 0;
	TPartyMemberInfo * pPartyMemberInfo;
	if (GetPartyMemberPtr(dwPID, &pPartyMemberInfo))
	{
		dwVID = pPartyMemberInfo->dwVID;
	}

	m_PartyMemberMap.erase(dwPID);

	if (dwVID > 0)
	{
		CInstanceBase * pInstance = NEW_FindActorPtr(dwVID);
		if (pInstance)
			pInstance->RefreshTextTail();
	}
}

bool CPythonPlayer::IsPartyMemberByVID(DWORD dwVID)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.begin();
	for (; itor != m_PartyMemberMap.end(); ++itor)
	{
		TPartyMemberInfo & rPartyMemberInfo = itor->second;
		if (dwVID == rPartyMemberInfo.dwVID)
			return true;
	}

	return false;
}

bool CPythonPlayer::IsPartyMemberByName(const char *c_szName)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.begin();
	for (; itor != m_PartyMemberMap.end(); ++itor)
	{
		TPartyMemberInfo & rPartyMemberInfo = itor->second;
		if (0 == rPartyMemberInfo.strName.compare(c_szName))
			return true;
	}

	return false;
}

bool CPythonPlayer::GetPartyMemberPtr(DWORD dwPID, TPartyMemberInfo ** ppPartyMemberInfo)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.find(dwPID);

	if (m_PartyMemberMap.end() == itor)
		return false;

	*ppPartyMemberInfo = &(itor->second);

	return true;
}

bool CPythonPlayer::PartyMemberPIDToVID(DWORD dwPID, DWORD * pdwVID)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.find(dwPID);

	if (m_PartyMemberMap.end() == itor)
		return false;

	const TPartyMemberInfo & c_rPartyMemberInfo = itor->second;
	*pdwVID = c_rPartyMemberInfo.dwVID;

	return true;
}

bool CPythonPlayer::PartyMemberVIDToPID(DWORD dwVID, DWORD * pdwPID)
{
	std::map<DWORD, TPartyMemberInfo>::iterator itor = m_PartyMemberMap.begin();
	for (; itor != m_PartyMemberMap.end(); ++itor)
	{
		TPartyMemberInfo & rPartyMemberInfo = itor->second;
		if (dwVID == rPartyMemberInfo.dwVID)
		{
			*pdwPID = rPartyMemberInfo.dwPID;
			return true;
		}
	}

	return false;
}

bool CPythonPlayer::IsSamePartyMember(DWORD dwVID1, DWORD dwVID2)
{
	return (IsPartyMemberByVID(dwVID1) && IsPartyMemberByVID(dwVID2));
}

// PVP
void CPythonPlayer::RememberChallengeInstance(DWORD dwVID)
{
	m_RevengeInstanceSet.erase(dwVID);
	m_ChallengeInstanceSet.insert(dwVID);
}
void CPythonPlayer::RememberRevengeInstance(DWORD dwVID)
{
	m_ChallengeInstanceSet.erase(dwVID);
	m_RevengeInstanceSet.insert(dwVID);
}
void CPythonPlayer::RememberCantFightInstance(DWORD dwVID)
{
	m_CantFightInstanceSet.insert(dwVID);
}
void CPythonPlayer::ForgetInstance(DWORD dwVID)
{
	m_ChallengeInstanceSet.erase(dwVID);
	m_RevengeInstanceSet.erase(dwVID);
	m_CantFightInstanceSet.erase(dwVID);
}

bool CPythonPlayer::IsChallengeInstance(DWORD dwVID)
{
	return m_ChallengeInstanceSet.end() != m_ChallengeInstanceSet.find(dwVID);
}
bool CPythonPlayer::IsRevengeInstance(DWORD dwVID)
{
	return m_RevengeInstanceSet.end() != m_RevengeInstanceSet.find(dwVID);
}
bool CPythonPlayer::IsCantFightInstance(DWORD dwVID)
{
	return m_CantFightInstanceSet.end() != m_CantFightInstanceSet.find(dwVID);
}

void CPythonPlayer::OpenPrivateShop()
{
	m_isOpenPrivateShop = TRUE;
}
void CPythonPlayer::ClosePrivateShop()
{
	m_isOpenPrivateShop = FALSE;
}

bool CPythonPlayer::IsOpenPrivateShop()
{
	return m_isOpenPrivateShop;
}

void CPythonPlayer::SetObserverMode(bool isEnable)
{
	m_isObserverMode=isEnable;
}

bool CPythonPlayer::IsObserverMode()
{
	return m_isObserverMode;
}


BOOL CPythonPlayer::__ToggleCoolTime()
{
	m_sysIsCoolTime = 1 - m_sysIsCoolTime;
	return m_sysIsCoolTime;
}

BOOL CPythonPlayer::__ToggleLevelLimit()
{
	m_sysIsLevelLimit = 1 - m_sysIsLevelLimit;
	return m_sysIsLevelLimit;
}

void CPythonPlayer::StartStaminaConsume(DWORD dwConsumePerSec, DWORD dwCurrentStamina)
{
	m_isConsumingStamina = TRUE;
	m_fConsumeStaminaPerSec = float(dwConsumePerSec);
	m_fCurrentStamina = float(dwCurrentStamina);

	SetStatus(POINT_STAMINA, dwCurrentStamina);
}

void CPythonPlayer::StopStaminaConsume(DWORD dwCurrentStamina)
{
	m_isConsumingStamina = FALSE;
	m_fConsumeStaminaPerSec = 0.0f;
	m_fCurrentStamina = float(dwCurrentStamina);

	SetStatus(POINT_STAMINA, dwCurrentStamina);
}

DWORD CPythonPlayer::GetPKMode()
{
	CInstanceBase * pInstance = NEW_GetMainActorPtr();
	if (!pInstance)
		return 0;

	return pInstance->GetPKMode();
}

void CPythonPlayer::SetGameWindow(PyObject *ppyObject)
{
	m_ppyGameWindow = ppyObject;
}

void CPythonPlayer::NEW_ClearSkillData(bool bAll)
{
#ifdef __AUTO_HUNT__
	m_iLastAutoHuntDieCheck = 0;
	m_iAutoHuntSleepTime = 0;
	m_bAutoHuntMob = false;
	m_bAutoHuntStatus = false;
	m_bAutoHuntStone = false;
#endif

	std::map<DWORD, DWORD>::iterator it;

	for (it = m_skillSlotDict.begin(); it != m_skillSlotDict.end();)
	{
		if (bAll || __GetSkillType(it->first) == CPythonSkill::SKILL_TYPE_ACTIVE)
			it = m_skillSlotDict.erase(it);
		else
			++it;
	}

	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		ZeroMemory(&m_playerStatus.aSkill[i], sizeof(TSkillInstance));
	}

	for (int j = 0; j < SKILL_MAX_NUM; ++j)
	{
		m_playerStatus.aSkill[j].iGrade = 0;
		m_playerStatus.aSkill[j].fcurEfficientPercentage=0.0f;
		m_playerStatus.aSkill[j].fnextEfficientPercentage=0.05f;
	}

	if (m_ppyGameWindow)
		PyCallClassMemberFunc(m_ppyGameWindow, "BINARY_CheckGameButton", Py_BuildNone());
}

void CPythonPlayer::ClearSkillDict()
{
	m_skillSlotDict.clear();

	m_isOpenPrivateShop = false;
	m_isObserverMode = false;

	m_isConsumingStamina = FALSE;
	m_fConsumeStaminaPerSec = 0.0f;
	m_fCurrentStamina = 0.0f;

	__ClearAutoAttackTargetActorID();
}

void CPythonPlayer::ClearSkillToggle(DWORD dwSlotIndex)
{
	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	if (!pkInstMain->IsDead())
		return;

	if (dwSlotIndex >= SKILL_MAX_NUM)
		return;

	for (dwSlotIndex; dwSlotIndex <= SKILL_MAX_NUM; ++dwSlotIndex)
	{
		if (IsSkillActive(dwSlotIndex))
		{
			m_playerStatus.aSkill[dwSlotIndex].bActive = FALSE;
			PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSkillSlot", Py_BuildValue("(i)", dwSlotIndex));
		}
	}
}

void CPythonPlayer::ResetSkillCoolTimes()
{
	if (!CPythonNetworkStream::Instance().GetMainActorSkillGroup())
		return;

	for (int i = 0; i < SKILL_MAX_NUM; ++i) {
		TSkillInstance& rkSkillInst = m_playerStatus.aSkill[i];
		if (!rkSkillInst.fLastUsedTime && !rkSkillInst.fCoolTime)
			continue;

		rkSkillInst.fLastUsedTime = rkSkillInst.fCoolTime = 0.0f;
		PyCallClassMemberFunc(m_ppyGameWindow, "SkillClearCoolTime", Py_BuildValue("(i)", i));
	}
}

void CPythonPlayer::Clear()
{
	memset(&m_playerStatus, 0, sizeof(m_playerStatus));
	NEW_ClearSkillData(true);

	m_bisProcessingEmotion = FALSE;

	m_dwSendingTargetVID = 0;
	m_fTargetUpdateTime = 0.0f;

	// Test Code for Status Interface
	m_stName = "";
	m_dwMainCharacterIndex = 0;
	m_dwRace = 0;
	m_dwWeaponMinPower = 0;
	m_dwWeaponMaxPower = 0;
	m_dwWeaponMinMagicPower = 0;
	m_dwWeaponMaxMagicPower = 0;
	m_dwWeaponAddPower = 0;

	/////
	m_MovingCursorPosition = TPixelPosition(0, 0, 0);
	m_fMovingCursorSettingTime = 0.0f;

	m_eReservedMode = MODE_NONE;
	m_fReservedDelayTime = 0.0f;
	m_kPPosReserved = TPixelPosition(0, 0, 0);
	m_dwVIDReserved = 0;
	m_dwIIDReserved = 0;
	m_dwSkillSlotIndexReserved = 0;
	m_dwSkillRangeReserved = 0;

	m_isUp = false;
	m_isDown = false;
	m_isLeft = false;
	m_isRight = false;
	m_isSmtMov = false;
	m_isDirMov = false;
	m_isDirKey = false;
	m_isAtkKey = false;

	m_isCmrRot = true;
	m_fCmrRotSpd = 20.0f;

	m_iComboOld = 0;

	m_dwVIDPicked = 0;
	m_dwIIDPicked = 0;

	m_dwcurSkillSlotIndex = DWORD(-1);

	m_dwTargetVID = 0;
	m_dwTargetEndTime = 0;

	m_PartyMemberMap.clear();

	m_ChallengeInstanceSet.clear();
	m_RevengeInstanceSet.clear();

	m_isOpenPrivateShop = false;
	m_isObserverMode = false;

	m_isConsumingStamina = FALSE;
	m_fConsumeStaminaPerSec = 0.0f;
	m_fCurrentStamina = 0.0f;

	m_inGuildAreaID = 0xffffffff;

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	m_AuraItemInstanceVector.clear();
	m_AuraItemInstanceVector.resize(AURA_SLOT_MAX);
	for (size_t i = 0; i < m_AuraItemInstanceVector.size(); ++i)
	{
		TItemData& rkAuraItemInstance = m_AuraItemInstanceVector[i];
		ZeroMemory(&rkAuraItemInstance, sizeof(TItemData));
	}

	m_bAuraWindowOpen = false;
	m_bOpenedAuraWindowType = AURA_WINDOW_TYPE_MAX;

	for (BYTE j = 0; j < AURA_SLOT_MAX; ++j)
		m_AuraRefineActivatedCell[j] = NPOS;

	ZeroMemory(&m_bAuraRefineInfo, sizeof(TAuraRefineInfo));
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	m_isOpenPetHatchingWindow = false;
	m_isOpenPetNameChangeWindow = false;
	m_isOpenPetFeedWindow = false;
	m_isOpenSafebox = false;
	m_isOpenMall = false;

	m_dwActivePetID = 0;

	m_PetDataMap.clear();
#endif

	__ClearAutoAttackTargetActorID();
}

#ifdef ENABLE_AURA_COSTUME_SYSTEM
void CPythonPlayer::__ClearAuraRefineWindow()
{
	for (BYTE i = 0; i < AURA_SLOT_MAX; ++i)
	{
		if (!m_AuraRefineActivatedCell[i].IsNPOS())
		{
			m_AuraRefineActivatedCell[i] = NPOS;
		}
	}
}

void CPythonPlayer::SetAuraRefineWindowOpen(BYTE wndType)
{
	m_bOpenedAuraWindowType = wndType;
	m_bAuraWindowOpen = AURA_WINDOW_TYPE_MAX != wndType;

	if (!m_bAuraWindowOpen)
		__ClearAuraRefineWindow();
}

bool CPythonPlayer::IsAuraRefineWindowEmpty()
{
	for (size_t i = 0; i < m_AuraItemInstanceVector.size(); ++i)
	{
		if (m_AuraItemInstanceVector[i].vnum)
			return false;
	}

	return true;
}

void CPythonPlayer::SetAuraRefineInfo(BYTE bAuraRefineInfoSlot, BYTE bAuraRefineInfoLevel, BYTE bAuraRefineInfoExpPercent)
{
	if (bAuraRefineInfoSlot >= AURA_REFINE_INFO_SLOT_MAX)
		return;

	m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoLevel = bAuraRefineInfoLevel;
	m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoExpPercent = bAuraRefineInfoExpPercent;
}

BYTE CPythonPlayer::GetAuraRefineInfoLevel(BYTE bAuraRefineInfoSlot)
{
	if (bAuraRefineInfoSlot >= AURA_REFINE_INFO_SLOT_MAX)
		return 0;

	return m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoLevel;
}

BYTE CPythonPlayer::GetAuraRefineInfoExpPct(BYTE bAuraRefineInfoSlot)
{
	if (bAuraRefineInfoSlot >= AURA_REFINE_INFO_SLOT_MAX)
		return 0;

	return m_bAuraRefineInfo[bAuraRefineInfoSlot].bAuraRefineInfoExpPercent;
}

void CPythonPlayer::SetAuraItemData(BYTE bySlotIndex, const TItemData& rItemInstance)
{
	if (bySlotIndex >= m_AuraItemInstanceVector.size())
	{
		TraceError("CPythonPlayer::SetAuraItemData(bySlotIndex=%u) - Strange slot index", bySlotIndex);
		return;
	}

	m_AuraItemInstanceVector[bySlotIndex] = rItemInstance;
}

void CPythonPlayer::DelAuraItemData(BYTE bySlotIndex)
{
	if (bySlotIndex >= AURA_SLOT_MAX || bySlotIndex >= (BYTE)m_AuraItemInstanceVector.size())
	{
		TraceError("CPythonPlayer::DelAuraItemData(bySlotIndex=%u) - Strange slot index", bySlotIndex);
		return;
	}

	TItemData& rInstance = m_AuraItemInstanceVector[bySlotIndex];
	CItemData* pItemData;
	if (CItemManager::instance().GetItemDataPointer(rInstance.vnum, &pItemData))
	{
		if (bySlotIndex == AURA_SLOT_MAIN || bySlotIndex == AURA_SLOT_SUB)
			DelAuraItemData(AURA_SLOT_RESULT);

		if (bySlotIndex != AURA_SLOT_RESULT)
		{
			m_AuraRefineActivatedCell[bySlotIndex] = NPOS;
		}

		ZeroMemory(&rInstance, sizeof(TItemData));
	}
}

BYTE CPythonPlayer::FineMoveAuraItemSlot()
{
	for (size_t i = 0; i < m_AuraItemInstanceVector.size(); ++i)
	{
		if (!m_AuraItemInstanceVector[i].vnum)
			return (BYTE)i;
	}

	return AURA_SLOT_MAX;
}

BYTE CPythonPlayer::GetAuraCurrentItemSlotCount()
{
	BYTE bCurCount = 0;
	for (BYTE i = 0; i < AURA_SLOT_MAX; ++i)
	{
		if (m_AuraItemInstanceVector[i].vnum)
			++bCurCount;
	}

	return bCurCount;
}

BOOL CPythonPlayer::GetAuraItemDataPtr(BYTE bySlotIndex, TItemData** ppInstance)
{
	if (bySlotIndex >= m_AuraItemInstanceVector.size())
	{
		TraceError("CPythonPlayer::GetAuraItemDataPtr(bySlotIndex=%u) - Strange slot index", bySlotIndex);
		return FALSE;
	}

	*ppInstance = &m_AuraItemInstanceVector[bySlotIndex];

	return TRUE;
}

void CPythonPlayer::SetActivatedAuraSlot(BYTE bySlotIndex, TItemPos ItemCell)
{
	if (bySlotIndex >= AURA_SLOT_MAX)
		return;

	m_AuraRefineActivatedCell[bySlotIndex] = ItemCell;
}

BYTE CPythonPlayer::FindActivatedAuraSlot(TItemPos ItemCell)
{
	for (BYTE i = AURA_SLOT_MAIN; i < AURA_SLOT_MAX; ++i)
	{
		if (m_AuraRefineActivatedCell[i] == ItemCell)
			return BYTE(i);
	}

	return AURA_SLOT_MAX;
}

TItemPos CPythonPlayer::FindUsingAuraSlot(BYTE bySlotIndex)
{
	if (bySlotIndex >= AURA_SLOT_MAX)
		return NPOS;

	return m_AuraRefineActivatedCell[bySlotIndex];
}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
void CPythonPlayer::SetPetData(TPetData* pPetData)
{
	auto it = m_PetDataMap.find(pPetData->dwID);
	if (it != m_PetDataMap.end())
		m_PetDataMap.erase(it);

	m_PetDataMap.insert(TPetDataMap::value_type(pPetData->dwID, *pPetData));
}

bool CPythonPlayer::GetPetInfo(DWORD dwID, TPetData** ppTPetData)
{
	auto it = m_PetDataMap.find(dwID);
	if (it == m_PetDataMap.end())
		return false;

	*ppTPetData = &(it->second);

	return true;
}

void CPythonPlayer::DeletePetData(DWORD dwID) 
{
	auto it = m_PetDataMap.find(dwID);
	if (it == m_PetDataMap.end())
		return;

	if (m_dwActivePetID == dwID)
		m_dwActivePetID = 0;

	m_PetDataMap.erase(it);
}

void CPythonPlayer::ClearPetExchangeData()
{
	auto it = m_PetDataMap.begin();

	while (it != m_PetDataMap.end())
	{
		TPetData pet = it->second;
		if (pet.bIsExchange)
			m_PetDataMap.erase(it++);
		else
			++it;
	}
}

void CPythonPlayer::SetPetSkillData(DWORD dwID, TPetSkillUpdatePacket* pPetSkill)
{
	auto it = m_PetDataMap.find(dwID);
	if (it == m_PetDataMap.end())
		return;

	BYTE bSkillCount = 0;

	for (int i = 0; i < PET_SKILL_COUNT_MAX; ++i, ++pPetSkill)
	{
		it->second.skillData.aSkillInfo[i].bSkill = pPetSkill->bSkill;
		it->second.skillData.aSkillInfo[i].bLevel = pPetSkill->bLevel;
		it->second.skillData.aSkillInfo[i].dwCooltime = pPetSkill->dwCooltime;

		for (int j = 0; j < PET_GROWTH_SKILL_LEVEL_MAX; j++)
		{
			it->second.skillData.aSkillInfo[i].dwFormula1[j] = pPetSkill->bSkillFormula1[j];
			it->second.skillData.aSkillInfo[i].dwFormula2[j] = pPetSkill->wSkillFormula2[j];
			it->second.skillData.aSkillInfo[i].dwFormula3[j] = pPetSkill->bSkillFormula3[j];
		}

		if (!pPetSkill->bLocked)
			bSkillCount++;
	}

	it->second.skillData.bSkillCount = bSkillCount;
}

bool CPythonPlayer::GetPetSkill(DWORD dwID, TPetSkillData** ppPetSkillData)
{
	auto it = m_PetDataMap.find(dwID);
	if (it == m_PetDataMap.end())
		return false;

	*ppPetSkillData = &(it->second.skillData);

	return true;
}

void CPythonPlayer::SetPetSkillCooltime(DWORD dwID, BYTE bSlot, DWORD dwCooltime)
{
	auto it = m_PetDataMap.find(dwID);
	if (it == m_PetDataMap.end())
		return;

	it->second.skillData.aSkillInfo[bSlot].dwCooltime = dwCooltime;
}

void CPythonPlayer::SetPetStatus(DWORD dwID, BYTE byPoint, DWORD dwValue) 
{
	auto it = m_PetDataMap.find(dwID);
	if (it == m_PetDataMap.end())
		return;

	it->second.dwPoints[byPoint] = dwValue;
}
long CPythonPlayer::GetPetStatus(DWORD dwID, BYTE byPoint) 
{ 
	auto it = m_PetDataMap.find(dwID);
	if (it == m_PetDataMap.end())
		return 0;

	return it->second.dwPoints[byPoint];
}

void CPythonPlayer::SetActivePet(DWORD dwID)
{
	m_dwActivePetID = dwID;
	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

DWORD CPythonPlayer::GetActivePet()
{
	return m_dwActivePetID;
}

DWORD CPythonPlayer::CanUseGrowthPetQuickSlot(DWORD dwLocalSlotIndex)
{ 
	if (dwLocalSlotIndex >= QUICKSLOT_MAX_COUNT_PER_LINE)
		return QUICK_SLOT_POS_ERROR;

	DWORD dwRegisteredType;
	DWORD dwRegisteredItemPos;
	GetLocalQuickSlotData(dwLocalSlotIndex, &dwRegisteredType, &dwRegisteredItemPos);

	switch (dwRegisteredType)
	{
		case SLOT_TYPE_INVENTORY:
		{
			DWORD dwItemIndex = CPythonPlayer::Instance().GetItemIndex(TItemPos(INVENTORY, dwRegisteredItemPos));
			CItemManager::Instance().SelectItemData(dwItemIndex);
			CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
			if (!pItemData)
				return QUICK_SLOT_IS_NOT_ITEM;

			if (CItemData::ITEM_TYPE_USE == pItemData->GetType() || CItemData::ITEM_TYPE_QUEST == pItemData->GetType())
				return QUICK_SLOT_ITEM_USE_SUCCESS;

			if (CItemData::ITEM_TYPE_GROWTH_PET == pItemData->GetType() && CItemData::PET_UPBRINGING == pItemData->GetSubType())
			{
				DWORD dwPetID = GetItemMetinSocket(TItemPos(INVENTORY, dwRegisteredItemPos), 2);

				CPythonPlayer::TPetData* pPet = nullptr;
				CPythonPlayer::Instance().GetPetInfo(dwPetID, &pPet);

				if (!pPet)
					return QUICK_SLOT_CAN_NOT_USE_PET_ITEM;

				if (pPet->GetDuration() < CPythonApplication::Instance().GetServerTimeStamp())
					return QUICK_SLOT_PET_ITEM_USE_FAILED;

				return QUICK_SLOT_PET_ITEM_USE_SUCCESS;
			}
		}

		case SLOT_TYPE_SKILL:
		case SLOT_TYPE_EMOTION:
		{
			return QUICK_SLOT_IS_NOT_ITEM;
		}
	}

	return QUICK_SLOT_POS_ERROR;
}
#endif

DWORD CPythonPlayer::GetHorseSkillSlotIndex()
{
	constexpr auto HorseSkillIndex = 130;
	static DWORD RequireSkillSlotIndex = 0;

	if (!RequireSkillSlotIndex)
		CPythonPlayer::Instance().FindSkillSlotIndexBySkillIndex(HorseSkillIndex, &RequireSkillSlotIndex);
	return RequireSkillSlotIndex;
}

CPythonPlayer::CPythonPlayer(void)
{
	SetMovableGroundDistance(40.0f);

	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_JEONGWI), 3));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GEOMGYEONG), 4));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_CHEONGEUN), 19));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GYEONGGONG), 49));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_EUNHYEONG), 34));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GONGPO), 64));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_JUMAGAP), 65));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_HOSIN), 94));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_BOHO), 95));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_KWAESOK), 110));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GICHEON), 96));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_JEUNGRYEOK), 111));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_PABEOP), 66));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_FALLEN_CHEONGEUN), 19));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_GWIGEOM), 63));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_MUYEONG), 78));
	m_kMap_dwAffectIndexToSkillIndex.insert(std::make_pair(int(CInstanceBase::AFFECT_HEUKSIN), 79));

	m_ppyGameWindow = nullptr;

	m_sysIsCoolTime = TRUE;
	m_sysIsLevelLimit = TRUE;
	m_dwPlayTime = 0;

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	ClearChangeLook();
	bIsChangeLookWindowOpen = false;
	bChangeLookWindowType = static_cast<decltype(bChangeLookWindowType)>(ETRANSMUTATIONTYPE::TRANSMUTATION_TYPE_ITEM);
#endif

	m_aeMBFButton[MBT_LEFT]=CPythonPlayer::MBF_SMART;
	m_aeMBFButton[MBT_RIGHT]=CPythonPlayer::MBF_CAMERA;
	m_aeMBFButton[MBT_MIDDLE]=CPythonPlayer::MBF_CAMERA;

	memset(m_adwEffect, 0, sizeof(m_adwEffect));

	m_isDestPosition = FALSE;
	m_ixDestPos = 0;
	m_iyDestPos = 0;
	m_iLastAlarmTime = 0;

	Clear();
}

CPythonPlayer::~CPythonPlayer(void) {}

#ifdef ENABLE_RENEWAL_REGEN
#include "../eterPack/EterPackManager.h"

void localToGlobal(long& X, long& Y)
{
	CPythonBackground::Instance().LocalPositionToGlobalPosition(X, Y);
	X *= 100;
	Y *= 100;
}

void positionToGlobal(const float& eX, const float& eY, long& X, long& Y)
{
	PR_FLOAT_TO_INT(eX, X);
	PR_FLOAT_TO_INT(eY, Y);
	X /= 100;
	Y /= 100;
	localToGlobal(X, Y);
}

int DISTANCE_APPROX_SERVER(int dx, int dy)
{
	int min, max;
	if (dx < 0)
		dx = -dx;

	if (dy < 0)
		dy = -dy;

	if (dx < dy)
	{
		min = dx;
		max = dy;
	}
	else
	{
		min = dy;
		max = dx;
	}
	return (((max << 8) + (max << 3) - (max << 4) - (max << 1) +(min << 7) - (min << 5) + (min << 3) - (min << 1)) >> 8);
}

bool CPythonPlayer::CheckBossSafeRange()
{
	if (m_eventData.size())
	{
		auto mainInstance = NEW_GetMainActorPtr();
		if (mainInstance)
		{
			TPixelPosition kPPosCur;
			mainInstance->NEW_GetPixelPosition(&kPPosCur);
			long mainX, mainY;
			positionToGlobal(kPPosCur.x, kPPosCur.y, mainX, mainY);

			const time_t cur_Time = time(nullptr);
			const struct tm vKey = *localtime(&cur_Time);
			int myear = vKey.tm_year;
			int mmon = vKey.tm_mon;
			int mday = vKey.tm_mday;
			int yday = vKey.tm_yday;
			int day = vKey.tm_wday;
			int hour = vKey.tm_hour;
			int minute = vKey.tm_min;
			int second = vKey.tm_sec;

			for (auto it = m_eventData.begin(); it != m_eventData.end(); ++it)
			{
				TNewRegen& newRegen = *it;
				if (newRegen.day != 0)
				{
					const int day = vKey.tm_wday + 1;
					if (day != newRegen.day)
						continue;
				}

				long posGlobalX = newRegen.x, posGlobalY = newRegen.y;
				localToGlobal(posGlobalX, posGlobalY);
				int fDist = DISTANCE_APPROX_SERVER(mainX - posGlobalX, mainY - posGlobalY);
				if (fDist <= newRegen.safeRange)
				{
					std::vector<CInstanceBase*> m_Data;
					CPythonCharacterManager::Instance().GetMobWithVnum(newRegen.mob_vnum, m_Data);
					if (m_Data.size())
					{
						for (auto itBoss = m_Data.begin(); itBoss != m_Data.end(); ++itBoss)
						{
							CInstanceBase* bossInstance = *itBoss;
							if (bossInstance)
							{
								TPixelPosition kPPosBoss;
								bossInstance->NEW_GetPixelPosition(&kPPosBoss);
								long bossX, bossY;
								positionToGlobal(kPPosBoss.x, kPPosBoss.y, bossX, bossY);
								fDist = DISTANCE_APPROX_SERVER(mainX - bossX, mainY - bossY);
								if (fDist <= newRegen.safeRange)
									return true;
							}
						}
					}
					else
					{
						const int nowTime = time(0);
						if (newRegen.leftTime < 0)
						{
							bool isSucces = false;
							const int now = time(0) + 2;
							const BYTE regenHour = newRegen.hour;
							const BYTE regenMinute = newRegen.minute;
							const BYTE regenSecond = newRegen.second;
							while (true)
							{
								++second;
								if (second == 60)
								{
									minute += 1;
									second = 0;
									if (minute == 60)
									{
										minute = 0;
										hour += 1;
										if (hour == 24)
										{
											hour = 0;
											day += 1;
											if (day == 7)
												day = 0;
											mday += 1;
											yday += 1;
											if (mday == 32)
											{
												mday = 1;
												mmon += 1;
												if (mmon == 12)
												{
													mmon = 0;
													myear += 1;
													yday = 0;
												}
											}
										}
									}
								}
								if (second == 0)
								{
									if (newRegen.hours_range)
									{
										if ((hour % newRegen.hours_range) == 0 && regenMinute == minute)
										{
											isSucces = true;
											break;
										}
									}
									if (newRegen.minute_range)
									{
										if ((minute % newRegen.minute_range) == 0)
										{
											isSucces = true;
											break;
										}
									}
								}
								if (regenHour == hour && regenMinute == minute && regenSecond == second)
								{
									if (newRegen.day)
									{
										if (newRegen.day - 1 == day)
										{
											isSucces = true;
											break;
										}
									}
									else
									{
										isSucces = true;
										break;
									}
								}
								if (time(0) > now)
									break;
							}
							if (isSucces)
							{
								std::tm tm{};
								tm.tm_year = myear;
								tm.tm_mon = mmon;
								tm.tm_mday = mday;
								tm.tm_isdst = 0;
								tm.tm_wday = day;
								tm.tm_hour = hour;
								tm.tm_min = minute;
								tm.tm_sec = second;
								tm.tm_yday = yday;
								const time_t t = mktime(&tm);
								newRegen.leftTime = t;

								if (newRegen.leftTime-nowTime >= 0 && newRegen.leftTime-nowTime <= 30)
									return true;
							}
							else
								return false;
						}
						else
						{
							if (newRegen.leftTime-nowTime >= 0 && newRegen.leftTime-nowTime <= 30)
								return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool CPythonPlayer::LoadNewRegen()
{
	m_eventData.clear();
	CMappedFile File;
	LPCVOID pData;

	if (!CEterPackManager::Instance().Get(File, "locale/common/newregen.txt", &pData))
		return false;

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);
	const std::string mapName = CPythonBackground::Instance().GetWarpMapName();
	CTokenVector TokenVector;
	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
			continue;
		else if (TokenVector.size() != 16)
			continue;
		else if (mapName != TokenVector[2])
			continue;

		TNewRegen data;
		data.x = atoi(TokenVector[3].c_str());
		data.y = atoi(TokenVector[4].c_str());
		data.day = atoi(TokenVector[7].c_str());
		data.hour = atoi(TokenVector[8].c_str());
		data.minute = atoi(TokenVector[9].c_str());
		data.second = atoi(TokenVector[10].c_str());
		data.safeRange = atoi(TokenVector[12].c_str());
		if (data.safeRange == 0)
			continue;
		data.hours_range = atoi(TokenVector[13].c_str());
		data.minute_range = atoi(TokenVector[14].c_str());
		data.mob_vnum = atoi(TokenVector[15].c_str());
		data.leftTime = -1;
		m_eventData.emplace_back(data);
	}
	return true;
}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
void CPythonPlayer::SetItemTransmutationVnum(TItemPos& Cell, const DWORD blVnum)
{
	if (!Cell.IsValidCell())
		return;

	const TItemData* item = GetItemData(Cell);
	if (item == nullptr)
		return;

	const_cast<TItemData*>(item)->dwTransmutationVnum = blVnum;
	PyCallClassMemberFunc(m_ppyGameWindow, "RefreshInventory", Py_BuildValue("()"));
}

DWORD CPythonPlayer::GetItemChangeLookVnum(TItemPos& Cell) const
{
	if (!Cell.IsValidCell())
		return 0;

	const TItemData* pItem = GetItemData(Cell);
	return pItem ? pItem->dwTransmutationVnum : 0;
}

void CPythonPlayer::ClearChangeLook(const bool bClearEffect)
{
	for (BYTE i = 0; i < static_cast<BYTE>(ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_MAX); i++)
		DelChangeLookItemData(i, bClearEffect);

	DelChangeLookFreeItemData(bClearEffect);
}

void CPythonPlayer::SetChangeLookWindowOpen(const bool blOpen)
{
	bIsChangeLookWindowOpen = blOpen;
	CPythonPlayer::Instance().ClearChangeLook(true);
}

bool CPythonPlayer::GetChangeLookWindowOpen() const
{
	return bIsChangeLookWindowOpen;
}

void CPythonPlayer::SetChangeLookWindowType(const bool blType)
{
	bChangeLookWindowType = blType;
}

bool CPythonPlayer::GetChangeLookWindowType() const
{
	return bChangeLookWindowType;
}

void CPythonPlayer::SetChangeLookItemData(TPacketGCTransmutationItemSet& data)
{
	if (data.bSlotType >= static_cast<BYTE>(ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_MAX))
		return;

	m_ChangeLookSlot[data.bSlotType] = data;
	PyCallClassMemberFunc(m_ppyGameWindow, "ActivateSlot", Py_BuildValue("(ii)", data.wCell, UI::ESlotHilight::HILIGHTSLOT_CHANGE_LOOK));
}

TPacketGCTransmutationItemSet* CPythonPlayer::GetChangeLookItemData(const BYTE bSlotType)
{
	if (bSlotType >= static_cast<BYTE>(ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_MAX))
		return nullptr;

	return &m_ChangeLookSlot[bSlotType];
}

void CPythonPlayer::DelChangeLookItemData(const BYTE bSlotType, const bool bClearEffect)
{
	if (bSlotType >= static_cast<BYTE>(ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_MAX))
		return;

	TPacketGCTransmutationItemSet& rInstance = m_ChangeLookSlot[bSlotType];

	if (bClearEffect && rInstance.wCell < c_Inventory_Count)
		PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSlot", Py_BuildValue("(ii)", rInstance.wCell, UI::ESlotHilight::HILIGHTSLOT_CHANGE_LOOK));

	ZeroMemory(&rInstance, sizeof(rInstance));
	rInstance.wCell = c_Inventory_Count;
}

void CPythonPlayer::SetChangeLookFreeItemData(TPacketGCTransmutationItemSet& data)
{
	m_ChangeLookFreeYangItemSlot = data;
	PyCallClassMemberFunc(m_ppyGameWindow, "ActivateSlot", Py_BuildValue("(ii)", data.wCell, UI::ESlotHilight::HILIGHTSLOT_CHANGE_LOOK));
}

TPacketGCTransmutationItemSet* CPythonPlayer::GetChangeLookFreeItemData()
{
	return &m_ChangeLookFreeYangItemSlot;
}

void CPythonPlayer::DelChangeLookFreeItemData(const bool bClearEffect)
{
	if (bClearEffect && m_ChangeLookFreeYangItemSlot.wCell < c_Inventory_Count)
		PyCallClassMemberFunc(m_ppyGameWindow, "DeactivateSlot", Py_BuildValue("(ii)", m_ChangeLookFreeYangItemSlot.wCell, UI::ESlotHilight::HILIGHTSLOT_CHANGE_LOOK));

	ZeroMemory(&m_ChangeLookFreeYangItemSlot, sizeof(m_ChangeLookFreeYangItemSlot));
	m_ChangeLookFreeYangItemSlot.wCell = c_Inventory_Count;
}
#endif

#ifdef __AUTO_HUNT__
#include "PythonMiniMap.h"
void CPythonPlayer::AutoHuntStop()
{
	NEW_Stop();
	__ClearAutoAttackTargetActorID();
	__ClearTarget();
	__ClearReservedAction();
}
void CPythonPlayer::ClearAutoHunt()
{
	m_bAutoHuntStatus = false;
	CPythonMiniMap::Instance().SetAutoHuntRange(false);
	m_vecAutoSkills.clear();
	m_vecAutoHuntItems.clear();
}

void CPythonPlayer::AutoHuntSettings(const char* szArgument)
{
	AutoHuntStop();

	std::vector<std::string> vecArgs;
	split_argument(szArgument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "end")
	{
		ClearAutoHunt();
	}
	else if (vecArgs[1] == "start")
	{
		ClearAutoHunt();
		if (vecArgs.size() < 7) { return; }
		if (vecArgs[2] != "empty")
		{
			std::vector<std::string> vecArgsEx;
			split_argument(vecArgs[2].c_str(), vecArgsEx, "?");
			for (BYTE j = 0; j < vecArgsEx.size(); ++j)
			{
				if (vecArgsEx[j] != "1")
					continue;
				m_vecAutoHuntItems.emplace_back(j, 0);
			}
		}
		if (vecArgs[3] != "empty")
		{
			std::vector<std::string> vecArgsEx;
			split_argument(vecArgs[3].c_str(), vecArgsEx, "?");
			int i;
			for (BYTE j = 0; j < vecArgsEx.size(); ++j)
			{
				if (j > 5)
					break;
				i = atoi(vecArgsEx[j].c_str());
				m_vecAutoSkills.emplace_back(i);
			}
		}

		m_bAutoHuntMob = vecArgs[5] == "1" ? true : false;
		m_bAutoHuntStone = vecArgs[6] == "1" ? true : false;

		m_bAutoHuntStatus = true;

		CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
		if(pkInstMain)
			pkInstMain->NEW_GetPixelPosition(&m_bAutoHuntStartPoint);
	
		CPythonMiniMap::Instance().SetAutoHuntRange(true);
	}
}

void CPythonPlayer::SetAutoHuntSleep(const int iSec)
{
	m_iAutoHuntSleepTime = !iSec ? 0 : CPythonApplication::Instance().GetServerTimeStamp() + iSec;
}

void CPythonPlayer::SetAutoHuntDieCheck(const int iSec)
{
	m_iLastAutoHuntDieCheck = !iSec ? 0 : CPythonApplication::Instance().GetServerTimeStamp() + iSec;
}

void CPythonPlayer::AutoHuntLoop()
{
	if (!m_bAutoHuntStatus)
		return;

	if (m_iLastAutoHuntDieCheck != 0)
	{
		if (m_iLastAutoHuntDieCheck > CPythonApplication::Instance().GetServerTimeStamp())
			return;
		SetAutoHuntDieCheck(0);
		SetAutoHuntSleep(5); // sleep time
		AutoHuntStop();
		CPythonNetworkStream::Instance().SendChatPacket("/restart_here", CHAT_TYPE_TALKING);
		return;
	}
	if (m_iAutoHuntSleepTime != 0)
	{
		if (m_iAutoHuntSleepTime > CPythonApplication::Instance().GetServerTimeStamp())
			return;
		SetAutoHuntSleep(0);

	}

	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (pkInstMain)
	{
		CInstanceBase* pkInstVictim = __GetTargetActorPtr();

		//This part in char.cpp -> CHARACTER::UpdateCharacter(DWORD dwPulse)
		const int now = time(0);
		const std::map<BYTE, std::pair<DWORD, int>> m_vecItemData = {
			{
				0,
				{
					70038,//itemIdx
					5//itemcooldown
				}
			},
			{
				1,
				{
					72049,//itemIdx
					60 * 5//itemcooldown
				}
			},
			{
				2,
				{
					71016,//itemIdx
					60 * 5//itemcooldown
				}
			}
		};
		for (auto it = m_vecAutoHuntItems.begin(); it != m_vecAutoHuntItems.end(); ++it)
		{
			if (it->second > now)
				continue;
			const auto itData = m_vecItemData.find(it->first);
			if (itData != m_vecItemData.end())
			{
				const DWORD itemVnum = itData->second.first;
		
				for (int i = 0; i < c_Inventory_Count; ++i)
				{
					if (m_playerStatus.aItem[i].vnum == itemVnum)
					{
						CPythonNetworkStream::Instance().SendItemUsePacket(TItemPos(INVENTORY, i));
						break;
					}
				}
				it->second = now + itData->second.second;
			}
		}

		for (const auto skillSlotNumber : m_vecAutoSkills)
		{
			CPythonSkill::TSkillData* pSkillData = NULL;
			if (CPythonSkill::Instance().GetSkillData(GetSkillIndex(skillSlotNumber), &pSkillData))
			{
				const BYTE skillIdx = GetSkillIndex(skillSlotNumber);
				if (pSkillData->IsStandingSkill() && skillIdx != 62 && skillIdx != 76 && skillIdx != 93 && skillIdx != 17)
				{
					if (!IsSkillActive(skillSlotNumber) && !__CheckRestSkillCoolTime(skillSlotNumber) && __CheckSkillUsable(skillSlotNumber))
					{
						__UseSkill(skillSlotNumber);
						return;
					}
					continue;
				}
				if (!pkInstVictim || __CheckRestSkillCoolTime(skillSlotNumber) || !__CheckSkillUsable(skillSlotNumber))
					continue;
				else if (__ProcessEnemySkillTargetRange(*pkInstMain, *pkInstVictim, *pSkillData, skillSlotNumber))
					if (__UseSkill(skillSlotNumber))
						return;
			}
		}
		if (pkInstMain->IsWalking())
		{
			if (pkInstVictim && CPythonCharacterManager::Instance().CanAttackToTarget(pkInstMain, pkInstVictim))
				AutoHuntStop();
			return;
		}
		if (!pkInstVictim || pkInstVictim->IsDead())
		{
			// AUTO_HUNT menzil artýrýldý: +40.0 -> +150.0, çarpan: 4000.0 -> 8000.0 (2x menzil)
			pkInstVictim = CPythonCharacterManager::Instance().FindVictim(pkInstMain, (CPythonMiniMap::Instance().GetAutoHuntRadius() + 150.0) * (8000.0 / 100.0), &m_bAutoHuntStartPoint, m_bAutoHuntMob, m_bAutoHuntStone);
			if (!pkInstVictim)
			{
				TPixelPosition srcPos;
				pkInstMain->NEW_GetPixelPosition(&srcPos);
				const float fDistance = GetDistanceNew(srcPos, m_bAutoHuntStartPoint);
				if(fDistance > 200.0)
					pkInstMain->NEW_MoveToDestPixelPositionDirection(m_bAutoHuntStartPoint);
				return;
			}
			SetTarget(pkInstVictim->GetVirtualID(), true, true);
			m_dwVIDReserved = pkInstVictim->GetVirtualID();
			m_dwTargetVID = pkInstVictim->GetVirtualID();
			__OnPressActor(*pkInstMain, m_dwTargetVID, true);
			return;
		}
		__ReserveProcess_ClickActor();
	}
}
#endif
