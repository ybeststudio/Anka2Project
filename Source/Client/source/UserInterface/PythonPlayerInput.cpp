#include "StdAfx.h"
#include "PythonPlayer.h"
#include "PythonPlayerEventHandler.h"
#include "PythonApplication.h"
#include "../eterlib/Camera.h"
#include "../eterbase/Timer.h"

const int c_iFastestSendingCount = 3;
const int c_iSlowestSendingCount = 3;
const float c_fFastestSendingDelay = 1.0f / float(c_iFastestSendingCount);
const float c_fSlowestSendingDelay = 1.0f / float(c_iSlowestSendingCount);
const float c_fRotatingStepTime = 0.5f;

const float c_fComboDistance = 250.0f;
const float c_fClickDistance = 300.0f;

DWORD CPythonPlayer::__GetPickableDistance()
{
	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (pkInstMain)
		if (pkInstMain->IsMountingHorse())
			return 500;

	return 300;
}

void CPythonPlayer::PickCloseMoney()
{
	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	TPixelPosition kPPosMain;
	pkInstMain->NEW_GetPixelPosition(&kPPosMain);

	DWORD dwItemID;
	CPythonItem& rkItem=CPythonItem::Instance();
	if (!rkItem.GetCloseMoney(kPPosMain, &dwItemID, __GetPickableDistance()))
		return;

	SendClickItemPacket(dwItemID);
}

void CPythonPlayer::PickCloseItem()
{
	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	TPixelPosition kPPosMain;
	pkInstMain->NEW_GetPixelPosition(&kPPosMain);

	DWORD dwItemID;
	CPythonItem& rkItem=CPythonItem::Instance();
	if (!rkItem.GetCloseItem(kPPosMain, &dwItemID, __GetPickableDistance()))
		return;

	SendClickItemPacket(dwItemID);
}

#ifdef ENABLE_INSTANT_PICKUP
void CPythonPlayer::PickCloseItemVector()
{
	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	TPixelPosition kPPosMain;
	pkInstMain->NEW_GetPixelPosition(&kPPosMain);

	std::vector<DWORD> itemVidList;
	CPythonItem& rkItem = CPythonItem::Instance();
	if (!rkItem.GetCloseItemVector(pkInstMain->GetNameString(), kPPosMain, itemVidList))
		return;

	if(itemVidList.empty())
		return;

	for(int i = 0; i < itemVidList.size(); i++)
		SendClickItemPacket(itemVidList[i]);
}
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
void CPythonPlayer::PickCloseItemAutoVector()
{
	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	TPixelPosition kPPosMain;
	pkInstMain->NEW_GetPixelPosition(&kPPosMain);

	std::vector<DWORD> itemVidList;
	CPythonItem& rkItem = CPythonItem::Instance();
	if (!rkItem.GetCloseItemVector(pkInstMain->GetNameString(), kPPosMain, itemVidList))
		return;

	if(itemVidList.empty())
		return;

	SendPickUPItemPacket(itemVidList);
}
#endif

bool CPythonPlayer::__IsTarget()
{
	return 0 != __GetTargetVID();
}

bool CPythonPlayer::__IsSameTargetVID(DWORD dwVID)
{
	return dwVID == __GetTargetVID();
}

DWORD CPythonPlayer::__GetTargetVID()
{
	return m_dwTargetVID;
}

DWORD CPythonPlayer::GetTargetVID()
{
	return __GetTargetVID();
}


CInstanceBase* CPythonPlayer::__GetTargetActorPtr()
{
	return NEW_FindActorPtr(__GetTargetVID());
}

void CPythonPlayer::__SetTargetVID(DWORD dwVID)
{
	m_dwTargetVID=dwVID;
}

void CPythonPlayer::__ClearTarget()
{
	if (!__IsTarget())
		return;

	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	pkInstMain->ClearFlyTargetInstance();

	CInstanceBase * pTargetedInstance = __GetTargetActorPtr();
	if (pTargetedInstance)
		pTargetedInstance->OnUntargeted();

	__SetTargetVID(0);

	CPythonNetworkStream::Instance().SendTargetPacket(0);
}

#ifdef __AUTO_HUNT__
void CPythonPlayer::SetTarget(DWORD dwVID, BOOL bForceChange, bool isAutoAttack)
#else
void CPythonPlayer::SetTarget(DWORD dwVID, BOOL bForceChange)
#endif
{
	CInstanceBase * pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	if (!pkInstMain->CanChangeTarget())
	{
		return;
	}

	DWORD dwCurrentTime = CTimer::Instance().GetCurrentMillisecond();

	if (__IsSameTargetVID(dwVID))
	{
		if (dwVID==pkInstMain->GetVirtualID())
		{
			__SetTargetVID(0);

			pkInstMain->OnUntargeted();
			pkInstMain->ClearFlyTargetInstance();
			CPythonNetworkStream::Instance().SendTargetPacket(0);
			return;
		}
		m_dwTargetEndTime = dwCurrentTime + 1000;
		return;
	}

	if (bForceChange)
	{
		m_dwTargetEndTime = dwCurrentTime + 2000;
	}
	else
	{
		if (m_dwTargetEndTime > dwCurrentTime)
		{
			return;
		}

		m_dwTargetEndTime = dwCurrentTime + 1000;
	}

	if (__IsTarget())
	{
		CInstanceBase * pTargetedInstance = __GetTargetActorPtr();
		if (pTargetedInstance)
			pTargetedInstance->OnUntargeted();
	}


	CInstanceBase * pkInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(dwVID);
	if (pkInstTarget)
	{
#ifdef __AUTO_HUNT__
		if (pkInstMain->IsTargetableInstance(*pkInstTarget) || isAutoAttack)
#else
		if (pkInstMain->IsTargetableInstance(*pkInstTarget))
#endif
		{
			__SetTargetVID(dwVID);

			pkInstTarget->OnTargeted();
			pkInstMain->SetFlyTargetInstance(*pkInstTarget);
			pkInstMain->GetGraphicThingInstanceRef().SetFlyEventHandler(CPythonPlayerEventHandler::GetSingleton().GetNormalBowAttackFlyEventHandler(pkInstMain, pkInstTarget));
			CPythonNetworkStream::Instance().SendTargetPacket(dwVID);

			return;
		}
	}

	__SetTargetVID(0);

	pkInstMain->ClearFlyTargetInstance();
	CPythonNetworkStream::Instance().SendTargetPacket(0);

}

bool CPythonPlayer::__ChangeTargetToPickedInstance()
{
	DWORD dwVID;
	if (!CPythonCharacterManager::Instance().OLD_GetPickedInstanceVID(&dwVID))
		return false;

	SetTarget(dwVID);
	return true;
}

CInstanceBase *	CPythonPlayer::__GetSkillTargetInstancePtr(CPythonSkill::TSkillData& rkSkillData)
{
	if (rkSkillData.IsNeedCorpse())
		return __GetDeadTargetInstancePtr();

	return __GetAliveTargetInstancePtr();
}

CInstanceBase *	CPythonPlayer::__GetDeadTargetInstancePtr()
{
	if (!__IsTarget())
		return NULL;

	CInstanceBase * pkInstTarget = __GetTargetActorPtr();

	if (!pkInstTarget)
		return NULL;

	if (!pkInstTarget->IsDead())
		return NULL;

	return pkInstTarget;
}

CInstanceBase * CPythonPlayer::__GetAliveTargetInstancePtr()
{
	if (!__IsTarget())
		return NULL;

	CInstanceBase * pkInstTarget = __GetTargetActorPtr();

	if (!pkInstTarget)
		return NULL;

	if (pkInstTarget->IsDead())
		return NULL;

	return pkInstTarget;
}


void CPythonPlayer::OpenCharacterMenu(DWORD dwVictimActorID)
{
	CInstanceBase * pkInstMain = CPythonPlayer::Instance().NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	CInstanceBase * pkInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(dwVictimActorID);
	if (!pkInstTarget)
		return;

	if (!pkInstTarget->IsPC() && !pkInstTarget->IsBuilding())
		return;

//	if (pkInstMain == pkInstTarget)
//		return;

	PyCallClassMemberFunc(m_ppyGameWindow, "SetPCTargetBoard", Py_BuildValue("(is)", pkInstTarget->GetVirtualID(), pkInstTarget->GetNameString()));
}

void CPythonPlayer::__OnClickItem(CInstanceBase& rkInstMain, DWORD dwItemID)
{
}

void CPythonPlayer::__OnClickActor(CInstanceBase& rkInstMain, DWORD dwPickedActorID, bool isAuto)
{
	if (MODE_USE_SKILL == m_eReservedMode)
	{
		if (__GetTargetVID() == dwPickedActorID)
			return;

		// 2005.03.25.levites
		if (__CheckDashAffect(rkInstMain))
		{
			m_dwVIDReserved = dwPickedActorID;
			return;
		}
	}

	__ClearReservedAction();

	CInstanceBase* pkInstVictim=NEW_FindActorPtr(dwPickedActorID);
	CInstanceBase& rkInstVictim=*pkInstVictim;
	if (!pkInstVictim)
		return;

	if (rkInstMain.IsAttackableInstance(*pkInstVictim))
		if (!__CanAttack())
			return;

	if (!rkInstMain.NEW_IsClickableDistanceDestInstance(rkInstVictim))
	{
		__ReserveClickActor(dwPickedActorID);
		return;
	}

	if (rkInstVictim.IsNPC())
	{
		__SendClickActorPacket(rkInstVictim);
	}

	rkInstMain.NEW_Stop();
	return;
}

void CPythonPlayer::__OnPressActor(CInstanceBase& rkInstMain, DWORD dwPickedActorID, bool isAuto)
{
	if (MODE_USE_SKILL == m_eReservedMode)
	{
		if (__GetTargetVID() == dwPickedActorID)
			return;

		// 2005.03.25.levites
		if (__CheckDashAffect(rkInstMain))
		{
			m_dwVIDReserved = dwPickedActorID;
			return;
		}
	}

	__ChangeTargetToPickedInstance();
	__ClearReservedAction();

	if (!__CanAttack())
		return;

	CInstanceBase* pkInstVictim=NEW_FindActorPtr(dwPickedActorID);
	if (!pkInstVictim)
		return;

	CInstanceBase& rkInstVictim=*pkInstVictim;

	if (isAuto)
	{
		if (rkInstMain.IsAttackableInstance(rkInstVictim))
			__SetAutoAttackTargetActorID(rkInstVictim.GetVirtualID());
	}

#ifdef ENABLE_AUTO_ATTACK_ARCHER_NINJA_FIX
	if (rkInstMain.IsBowMode())
	{
		if (rkInstMain.IsAttackableInstance(rkInstVictim))
		{
			if (!__CanShot(rkInstMain, rkInstVictim))
			{
				m_dwVIDReserved = 0;
				__ClearAutoAttackTargetActorID();
				return;
			}
		}
	}
#else
	if (rkInstMain.IsBowMode())
	{
		if (rkInstMain.IsAttackableInstance(rkInstVictim))
			if (!__CanShot(rkInstMain, rkInstVictim))
				return;
	}
#endif

	if (!rkInstMain.NEW_IsClickableDistanceDestInstance(rkInstVictim))
	{
		__ReserveClickActor(dwPickedActorID);
		return;
	}

	if (!rkInstMain.IsAttackableInstance(rkInstVictim))
	{
		return;
	}

	CPythonPlayerEventHandler& rkPlayerEventHandler=CPythonPlayerEventHandler::GetSingleton();
	rkInstMain.NEW_AttackToDestInstanceDirection(rkInstVictim, rkPlayerEventHandler.GetNormalBowAttackFlyEventHandler(&rkInstMain, &rkInstVictim));	
}

void CPythonPlayer::__OnPressItem(CInstanceBase& rkInstMain, DWORD dwPickedItemID)
{
	static DWORD s_dwLastPickItemID=0;

	if (s_dwLastPickItemID==dwPickedItemID)
	{
		Logn(1, "CPythonPlayer::__OnPressItem - ALREADY PICKED ITEM");
		return;
	}

	__ClearReservedAction();
	__ClearAutoAttackTargetActorID();

	CPythonItem& rkItem=CPythonItem::Instance();

	TPixelPosition kPPosPickedItem;
	if (!rkItem.GetGroundItemPosition(dwPickedItemID, &kPPosPickedItem))
		return;

	if (!rkInstMain.NEW_IsClickableDistanceDestPixelPosition(kPPosPickedItem))
	{
		__ReserveClickItem(dwPickedItemID);
		return;
	}

	rkInstMain.NEW_Stop();
	SendClickItemPacket(dwPickedItemID);
}

void CPythonPlayer::__OnClickGround(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround)
{
	if (!__IsMovableGroundDistance(rkInstMain, c_rkPPosPickedGround))
		return;

	if (rkInstMain.NEW_MoveToDestPixelPositionDirection(c_rkPPosPickedGround))
		__ShowPickedEffect(c_rkPPosPickedGround);
}

void CPythonPlayer::SetMovableGroundDistance(float fDistance)
{
	MOVABLE_GROUND_DISTANCE=fDistance;
}

bool CPythonPlayer::__IsMovableGroundDistance(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround)
{
	float fDistance=rkInstMain.NEW_GetDistanceFromDestPixelPosition(c_rkPPosPickedGround);

	if (fDistance<MOVABLE_GROUND_DISTANCE)
		return false;

	//Tracef("IsMovableGroundDistance(%f)\n", fDistance);
	return true;
}

void CPythonPlayer::__OnPressGround(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround)
{
	__ClearReservedAction();
	__ClearAutoAttackTargetActorID();

	if (NEW_CancelFishing())
		return;

	if (!__IsMovableGroundDistance(rkInstMain, c_rkPPosPickedGround))
		return;

	if (!rkInstMain.NEW_MoveToDestPixelPositionDirection(c_rkPPosPickedGround))
	{
		__ReserveClickGround(c_rkPPosPickedGround);
		return;
	}
}

void CPythonPlayer::__OnPressScreen(CInstanceBase& rkInstMain)
{
	__ClearReservedAction();

	NEW_MoveToMouseScreenDirection();
}


bool CPythonPlayer::NEW_MoveToDirection(float fDirRot)
{
	// PrivateShop
	if (IsOpenPrivateShop())
		return true;

	__ClearReservedAction();

	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (!pkInstMain)
		return false;

	if (pkInstMain->isLock())
		if (!pkInstMain->IsUsingMovingSkill())
			return true;

	CCamera* pkCmrCur=CCameraManager::Instance().GetCurrentCamera();
	if (pkCmrCur)
	{
		float fCmrCurRot=CameraRotationToCharacterRotation(pkCmrCur->GetRoll());

		if (m_isCmrRot)
		{
			float fSigDirRot=fDirRot;
			if (fSigDirRot>180.0f)
				fSigDirRot=fSigDirRot-360.0f;

			float fRotRat=fSigDirRot;
			if (fRotRat>90.0f)
				fRotRat=(180.0f-fRotRat);
			else if (fRotRat<-90.0f)
				fRotRat=(-180.0f-fRotRat);

			float fElapsedTime = CPythonApplication::Instance().GetGlobalElapsedTime();

			float fRotDeg = -m_fCmrRotSpd * fElapsedTime * fRotRat / 90.0f;
			pkCmrCur->Roll(fRotDeg);
		}

		fDirRot=fmod(360.0f + fCmrCurRot + fDirRot, 360.0f);
	}

	pkInstMain->NEW_MoveToDirection(fDirRot);

	return true;
}

void CPythonPlayer::NEW_Stop()
{
	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	pkInstMain->NEW_Stop();
	m_isLeft = FALSE;
	m_isRight = FALSE;
	m_isUp = FALSE;
	m_isDown = FALSE;
}

bool CPythonPlayer::NEW_CancelFishing()
{
	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return false;

	if (pkInstMain->IsFishing())
	{
		static DWORD s_dwLastCancelTime = 0;
		if (CTimer::Instance().GetCurrentMillisecond() < s_dwLastCancelTime + 500)
			return false;

		CPythonNetworkStream::Instance().SendFishingPacket(0);
		s_dwLastCancelTime = CTimer::Instance().GetCurrentMillisecond();
		return true;
	}

	return false;
}

void CPythonPlayer::NEW_Fishing()
{
	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	if (pkInstMain->IsFishing())
	{
		CPythonNetworkStream::Instance().SendFishingPacket(0);
	}
	else
	{
		if (pkInstMain->IsMountingHorse())
		{
			PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotFishingRideMount", Py_BuildValue("()"));
			return;
		}
#ifdef ENABLE_MSG_FISHING_WHILE_POLY
		if (pkInstMain->IsPoly())
		{
			PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotFishingWhilePoly", Py_BuildValue("()"));
			return;
		}
#endif
		if (pkInstMain->CanFishing())
		{
			int irot;
			if (pkInstMain->GetFishingRot(&irot))
				CPythonNetworkStream::Instance().SendFishingPacket(irot);
			else
			{
				PyCallClassMemberFunc(m_ppyGameWindow, "OnFishingWrongPlace", Py_BuildValue("()"));
			}
		}
	}
}

void CPythonPlayer::NEW_Attack()
{
	// PrivateShop
	if (IsOpenPrivateShop())
		return;

	if (!__CanAttack())
		return;

	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	if (!pkInstMain)
		return;

	if (pkInstMain->IsBowMode())
	{
		//CPythonPlayerEventHandler& rkPlayerEventHandler=CPythonPlayerEventHandler::GetSingleton();

		CInstanceBase * pkInstTarget = __GetAliveTargetInstancePtr();
		if (!pkInstTarget)
		{
			__ChangeTargetToPickedInstance();
			pkInstTarget = __GetAliveTargetInstancePtr();
		}

		if (pkInstTarget)
		{
			if (!__CanShot(*pkInstMain, *pkInstTarget))
				return;

			if (!pkInstMain->NEW_IsClickableDistanceDestInstance(*pkInstTarget))
			{
				__ReserveClickActor(pkInstTarget->GetVirtualID());
				return;
			}

			if (pkInstMain->IsAttackableInstance(*pkInstTarget))
			{
				pkInstMain->NEW_LookAtDestInstance(*pkInstTarget);
				pkInstMain->NEW_AttackToDestInstanceDirection(*pkInstTarget);
			}
		}
		else
		{
			PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotUseSkill", Py_BuildValue("(is)", GetMainCharacterIndex(), "NEED_TARGET"));
			return;
		}
	}
	else if (m_isDirKey)
	{
		float fDirRot=0.0f;
		NEW_GetMultiKeyDirRotation(m_isLeft, m_isRight, m_isUp, m_isDown, &fDirRot);

		CCamera* pkCmrCur=CCameraManager::Instance().GetCurrentCamera();
		if (pkCmrCur)
		{
			float fCmrCurRot=CameraRotationToCharacterRotation(pkCmrCur->GetRoll());

			fDirRot=fmod(360.0f + fCmrCurRot + fDirRot, 360.0f);
		}

		pkInstMain->NEW_Attack(fDirRot);
	}
	else
	{
		if (pkInstMain->IsMountingHorse())
		{
			if (pkInstMain->IsHandMode())
				return;
		}

		pkInstMain->NEW_Attack();
	}
}

bool CPythonPlayer::NEW_IsEmptyReservedDelayTime(float fElapsedTime)
{
	m_fReservedDelayTime -= fElapsedTime;

	if (m_fReservedDelayTime <= 0.0f)
	{
		m_fReservedDelayTime = 0.0f;
		return true;
	}

	return false;
}


void CPythonPlayer::NEW_SetAutoCameraRotationSpeed(float fRotSpd)
{
	m_fCmrRotSpd=fRotSpd;
}

void CPythonPlayer::NEW_ResetCameraRotation()
{
	CCamera* pkCmrCur=CCameraManager::Instance().GetCurrentCamera();
	CPythonApplication & rkApp = CPythonApplication::Instance();

	pkCmrCur->EndDrag();

	rkApp.SetCursorNum(CPythonApplication::NORMAL);
	if ( CPythonApplication::CURSOR_MODE_HARDWARE == rkApp.GetCursorMode())
		rkApp.SetCursorVisible(TRUE);
}


bool CPythonPlayer::__CanShot(CInstanceBase& rkInstMain, CInstanceBase& rkInstTarget)
{
	if (!__HasEnoughArrow())
	{
		PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotShot", Py_BuildValue("(is)", GetMainCharacterIndex(), "EMPTY_ARROW"));
		return false;
	}

	if (rkInstMain.IsInSafe())
	{
		PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotShot", Py_BuildValue("(is)", GetMainCharacterIndex(), "IN_SAFE"));
		return false;
	}

	if (rkInstTarget.IsInSafe())
	{
		PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotShot", Py_BuildValue("(is)", GetMainCharacterIndex(), "DEST_IN_SAFE"));
		return false;
	}

	return true;
}

bool CPythonPlayer::__CanChangeTarget()
{
	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (!pkInstMain)
		return false;

	return pkInstMain->CanChangeTarget();
}

bool CPythonPlayer::__CanMove()
{
	if (__IsProcessingEmotion())
	{
		return false;
	}

	CInstanceBase* pkInstMain=NEW_GetMainActorPtr();
	if (!pkInstMain)
		return false;

	if (!pkInstMain->CanMove())
	{
		if (!pkInstMain->IsUsingMovingSkill())
			return false;
	}

	return true;
}

bool CPythonPlayer::__CanAttack()
{
	if (__IsProcessingEmotion())
	{
		return false;
	}

	if (IsOpenPrivateShop())
		return false;

	if (IsObserverMode())
		return false;

	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();

	if (!pkInstMain)
		return false;

	// Fix me
#ifdef ENABLE_CONQUEROR_LEVEL
	if (pkInstMain->IsMountingHorse() && pkInstMain->IsNewMount() && (GetSkillGrade(107) < 1 && GetSkillLevel(107) < 11))
#else
	if (pkInstMain->IsMountingHorse() && (GetSkillGrade(109) < 1 && GetSkillLevel(109) < 20))
#endif
	{
		return false;
	}

	return pkInstMain->CanAttack();
}

void CPythonPlayer::NEW_GetMultiKeyDirRotation(bool isLeft, bool isRight, bool isUp, bool isDown, float *pfDirRot)
{
	float fScrX=0.0f;
	float fScrY=0.0f;

	if (isLeft)
		fScrX=0.0f;
	else if (isRight)
		fScrX=1.0f;
	else
		fScrX=0.5f;

	if (isUp)
		fScrY=0.0f;
	else if (isDown)
		fScrY=1.0f;
	else
		fScrY=0.5f;

	NEW_GetMouseDirRotation(fScrX, fScrY, pfDirRot);
}

void CPythonPlayer::NEW_GetMouseDirRotation(float fScrX, float fScrY, float *pfDirRot)
{
	long lWidth = UI::CWindowManager::Instance().GetScreenWidth();
	long lHeight = UI::CWindowManager::Instance().GetScreenHeight();
	int nScrPosX=lWidth*fScrX;
	int nScrPosY=lHeight*fScrY;
	int nScrWidth=lWidth;
	int nScrHeight=lHeight;
	int nScrCenterX=nScrWidth/2;
	int nScrCenterY=nScrHeight/2;

	float finputRotation = GetDegreeFromPosition(nScrPosX, nScrPosY, nScrCenterX, nScrCenterY);
	*pfDirRot=finputRotation;
}


float CPythonPlayer::GetDegreeFromPosition(int ix, int iy, int iHalfWidth, int iHalfHeight)
{
	D3DXVECTOR3 vtDir(float(ix - iHalfWidth), float(iy - iHalfHeight), 0.0f);
	D3DXVec3Normalize(&vtDir, &vtDir);

	D3DXVECTOR3 vtStan(0, -1, 0);
	float ret = D3DXToDegree(acosf(D3DXVec3Dot(&vtDir, &vtStan)));

	if (vtDir.x < 0.0f)
		ret = 360.0f - ret;

	return 360.0f - ret;
}

void CPythonPlayer::__ClearReservedAction()
{
	m_eReservedMode=MODE_NONE;
}

void CPythonPlayer::__ReserveClickItem(DWORD dwItemID)
{
	m_eReservedMode=MODE_CLICK_ITEM;
	m_dwIIDReserved=dwItemID;
}

void CPythonPlayer::__ReserveClickActor(DWORD dwActorID)
{
	m_eReservedMode=MODE_CLICK_ACTOR;
	m_dwVIDReserved=dwActorID;
}

void CPythonPlayer::__ReserveClickGround(const TPixelPosition& c_rkPPosPickedGround)
{
	m_eReservedMode=MODE_CLICK_POSITION;
	m_kPPosReserved=c_rkPPosPickedGround;
	m_fReservedDelayTime=0.1f;
}

bool CPythonPlayer::__IsReservedUseSkill(DWORD dwSkillSlotIndex)
{
	if (MODE_USE_SKILL!=m_eReservedMode)
		return false;

	if (m_dwSkillSlotIndexReserved!=dwSkillSlotIndex)
		return false;

	return true;
}

void CPythonPlayer::__ReserveUseSkill(DWORD dwActorID, DWORD dwSkillSlotIndex, DWORD dwRange)
{
	m_eReservedMode=MODE_USE_SKILL;
	m_dwVIDReserved=dwActorID;
	m_dwSkillSlotIndexReserved=dwSkillSlotIndex;
	m_dwSkillRangeReserved=dwRange;

	if (m_dwSkillRangeReserved > 100)
		m_dwSkillRangeReserved -= 10;
}

void CPythonPlayer::__ClearAutoAttackTargetActorID()
{
	__SetAutoAttackTargetActorID(0);
}

void CPythonPlayer::__SetAutoAttackTargetActorID(DWORD dwVID)
{
	CInstanceBase * pkInstVictim = NEW_FindActorPtr(dwVID);
	if (pkInstVictim && pkInstVictim->IsPC())
	return;

 	m_dwAutoAttackTargetVID = dwVID;
}


void CPythonPlayer::__ReserveProcess_ClickActor()
{
	CInstanceBase* pkInstMain = NEW_GetMainActorPtr();
	CInstanceBase* pkInstReserved = NEW_FindActorPtr(m_dwVIDReserved);
	if (!pkInstMain || !pkInstReserved)
	{
		__ClearReservedAction();
		return;
	}

	if (!pkInstMain->NEW_IsClickableDistanceDestInstance(*pkInstReserved))
	{
		pkInstMain->NEW_MoveToDestInstanceDirection(*pkInstReserved);
		return;
	}

	if (!pkInstMain->IsAttackableInstance(*pkInstReserved))
	{
		pkInstMain->NEW_Stop();
		__SendClickActorPacket(*pkInstReserved);
		__ClearReservedAction();
		return;
	}

	if (pkInstReserved->IsDead())
	{
		__ClearReservedAction();
		return;
	}

	if (pkInstMain->IsInSafe())
	{
		PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotAttack", Py_BuildValue("(is)", GetMainCharacterIndex(), "IN_SAFE"));
		pkInstMain->NEW_Stop();
		__ClearReservedAction();
		return;
	}

	if (pkInstReserved->IsInSafe())
	{
		PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotAttack", Py_BuildValue("(is)", GetMainCharacterIndex(), "DEST_IN_SAFE"));
		pkInstMain->NEW_Stop();
		__ClearReservedAction();
		return;
	}

	if (__CheckDashAffect(*pkInstMain))
	{
		return;
	}

	if (pkInstMain->IsBowMode())
	{
		if (!__HasEnoughArrow())
		{
			PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotShot", Py_BuildValue("(is)", GetMainCharacterIndex(), "EMPTY_ARROW"));
			pkInstMain->NEW_Stop();
			__ClearReservedAction();
			return;
		}
	}

	if (pkInstReserved->GetVirtualID() != GetTargetVID())
	{
		SetTarget(pkInstReserved->GetVirtualID());
	}

	pkInstMain->NEW_AttackToDestInstanceDirection(*pkInstReserved);
	__ClearReservedAction();
}
/*
CInstanceBase* pkInstReserved=NEW_FindActorPtr(m_dwVIDReserved);
if (pkInstReserved)
{
	if (pkInstMain->NEW_IsClickableDistanceDestInstance(*pkInstReserved))
	{
		if (pkInstMain->IsAttackableInstance(*pkInstReserved) )
		{
			if (!pkInstReserved->IsDead())
			{
				if (pkInstMain->IsInSafe())
				{
					PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotAttack", Py_BuildValue("(is)", GetMainCharacterIndex(), "IN_SAFE"));
					pkInstMain->NEW_Stop();
				}
				else if (pkInstReserved->IsInSafe())
				{
					PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotAttack", Py_BuildValue("(is)", GetMainCharacterIndex(), "DEST_IN_SAFE"));
					pkInstMain->NEW_Stop();
				}
				else
				{
					if (pkInstMain->IsBowMode())
					{
						if (!__HasEnoughArrow())
						{
							PyCallClassMemberFunc(m_ppyGameWindow, "OnCannotShot", Py_BuildValue("(is)", GetMainCharacterIndex(), "EMPTY_ARROW"));
							pkInstMain->NEW_Stop();
							__ClearReservedAction();
							break;
						}
					}

					if (pkInstReserved->GetVirtualID() != GetTargetVID())
					{
						SetTarget(pkInstReserved->GetVirtualID());
					}

					pkInstMain->NEW_AttackToDestInstanceDirection(*pkInstReserved);
				}
			}
		}
		else
		{
			__SendClickActorPacket(*pkInstReserved);

			pkInstMain->NEW_Stop();
		}

		__ClearReservedAction();
	}
	else
	{
		//Tracen("ReservedMode: MOVE");
		pkInstMain->NEW_MoveToDestInstanceDirection(*pkInstReserved);
	}
}
else
{
	__ClearReservedAction();
}
*/
