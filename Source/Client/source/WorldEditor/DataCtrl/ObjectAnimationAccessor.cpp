#include "StdAfx.h"
#include "ObjectAnimationAccessor.h"
#include "../WorldEditor.h"

void CRaceMotionDataAccessor::UpdateActorInstanceAccessorMotion()
{
	CSceneObject * pso = ((CWorldEditorApp*)AfxGetApp())->GetSceneObject();
	pso->UpdateActorInstanceMotion();
}

float CRaceMotionDataAccessor::GetMotionDuration()
{
	return m_fMotionDuration;
}

void CRaceMotionDataAccessor::SetMotionDuration (float fDuration)
{
	m_fMotionDuration = fDuration;
}

DWORD CRaceMotionDataAccessor::MakeEvent (int iEventType)
{
	switch (iEventType)
	{
		case MOTION_EVENT_TYPE_FLY:
			return MakeEventFly();
			break;
		case MOTION_EVENT_TYPE_EFFECT:
			return MakeEventEffect();
			break;
		case MOTION_EVENT_TYPE_SCREEN_WAVING:
			return MakeEventScreenWaving();
			break;
		case MOTION_EVENT_TYPE_SCREEN_FLASHING:
			return MakeEventScreenFlashing();
			break;
		case MOTION_EVENT_TYPE_SPECIAL_ATTACKING:
			return MakeEventAttacking();
			break;
		case MOTION_EVENT_TYPE_SOUND:
			return MakeEventSound();
			break;
		case MOTION_EVENT_TYPE_CHARACTER_SHOW:
			return MakeEventCharacterShow();
			break;
		case MOTION_EVENT_TYPE_CHARACTER_HIDE:
			return MakeEventCharacterHide();
			break;
		case MOTION_EVENT_TYPE_WARP:
			return MakeEventWarp();
			break;
		case MOTION_EVENT_TYPE_EFFECT_TO_TARGET:
			return MakeEventEffectToTarget();
			break;
		default:
			assert (!" CRaceMotionDataAccessor::MakeEvent - Strange Event Index!");
			Tracef (" CRaceMotionDataAccessor::MakeEvent - Strange Event Index!\n");
			break;
	}

	return DWORD (-1);
}
DWORD CRaceMotionDataAccessor::MakeEventScreenWaving()
{
	TScreenWavingEventData * pData = new TScreenWavingEventData;

	pData->iType = MOTION_EVENT_TYPE_SCREEN_WAVING;
	pData->fDurationTime = 0.0f;
	pData->iPower = 0;

	DWORD dwIndex = m_MotionEventDataVector.size();
	m_MotionEventDataVector.push_back (pData);

	return dwIndex;
}
DWORD CRaceMotionDataAccessor::MakeEventScreenFlashing()
{
	TScreenFlashingEventData * pData = new TScreenFlashingEventData;

	pData->iType = MOTION_EVENT_TYPE_SCREEN_FLASHING;
	pData->FlashingColor = D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f);

	DWORD dwIndex = m_MotionEventDataVector.size();
	m_MotionEventDataVector.push_back (pData);

	return dwIndex;
}

DWORD CRaceMotionDataAccessor::MakeEventFly()
{
	TMotionFlyEventData * pData = new TMotionFlyEventData;

	pData->iType = MOTION_EVENT_TYPE_FLY;
	pData->isAttaching = FALSE;
	pData->strAttachingBoneName = "";
	pData->dwFlyIndex = 0;
	pData->strFlyFileName = "";
	pData->v3FlyPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);

	DWORD dwIndex = m_MotionEventDataVector.size();
	m_MotionEventDataVector.push_back (pData);

	return dwIndex;
}

DWORD CRaceMotionDataAccessor::MakeEventEffect()
{
	TMotionEffectEventData * pData = new TMotionEffectEventData;

	pData->iType = MOTION_EVENT_TYPE_EFFECT;
	pData->isIndependent = FALSE;
	pData->isAttaching = FALSE;
	pData->isFollowing = FALSE;
	pData->strAttachingBoneName = "";
	pData->dwEffectIndex = 0;
	pData->strEffectFileName = "";
	pData->v3EffectPosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);

	DWORD dwIndex = m_MotionEventDataVector.size();
	m_MotionEventDataVector.push_back (pData);

	return dwIndex;
}
DWORD CRaceMotionDataAccessor::MakeEventAttacking()
{
	TMotionAttackingEventData * pAttackingData = new TMotionAttackingEventData;

	pAttackingData->iType = MOTION_EVENT_TYPE_SPECIAL_ATTACKING;

	DWORD dwIndex = m_MotionEventDataVector.size();
	m_MotionEventDataVector.push_back (pAttackingData);

	return dwIndex;
}
DWORD CRaceMotionDataAccessor::MakeEventSound()
{
	TMotionSoundEventData * pData = new TMotionSoundEventData;

	pData->iType = MOTION_EVENT_TYPE_SOUND;
	pData->strSoundFileName = "";

	DWORD dwIndex = m_MotionEventDataVector.size();
	m_MotionEventDataVector.push_back (pData);

	return dwIndex;
}

DWORD CRaceMotionDataAccessor::MakeEventCharacterShow()
{
	TMotionCharacterShowEventData * pData = new TMotionCharacterShowEventData;

	pData->iType = MOTION_EVENT_TYPE_CHARACTER_SHOW;

	DWORD dwIndex = m_MotionEventDataVector.size();
	m_MotionEventDataVector.push_back (pData);

	return dwIndex;
}

DWORD CRaceMotionDataAccessor::MakeEventCharacterHide()
{
	TMotionCharacterHideEventData * pData = new TMotionCharacterHideEventData;

	pData->iType = MOTION_EVENT_TYPE_CHARACTER_HIDE;

	DWORD dwIndex = m_MotionEventDataVector.size();
	m_MotionEventDataVector.push_back (pData);

	return dwIndex;
}

DWORD CRaceMotionDataAccessor::MakeEventWarp()
{
	TMotionWarpEventData * pData = new TMotionWarpEventData;

	pData->iType = MOTION_EVENT_TYPE_WARP;

	DWORD dwIndex = m_MotionEventDataVector.size();
	m_MotionEventDataVector.push_back (pData);

	return dwIndex;
}

DWORD CRaceMotionDataAccessor::MakeEventEffectToTarget()
{
	TMotionEffectToTargetEventData * pData = new TMotionEffectToTargetEventData;

	pData->iType = MOTION_EVENT_TYPE_EFFECT_TO_TARGET;

	DWORD dwIndex = m_MotionEventDataVector.size();
	m_MotionEventDataVector.push_back (pData);

	return dwIndex;
}

BOOL CRaceMotionDataAccessor::GetMotionEventDataPointer (DWORD dwIndex, TMotionEventData** ppData)
{
	if (dwIndex >= m_MotionEventDataVector.size())
	{
		return FALSE;
	}

	*ppData = m_MotionEventDataVector[dwIndex];

	return TRUE;
}

void CRaceMotionDataAccessor::MovePosition (DWORD dwIndex, float fTime)
{
	if (dwIndex >= m_MotionEventDataVector.size())
	{
		return;
	}

	m_MotionEventDataVector[dwIndex]->fStartingTime = fTime;
	const float c_fFrameTime = 1.0f / g_fGameFPS;
	m_MotionEventDataVector[dwIndex]->dwFrame = fTime / c_fFrameTime;
}

void CRaceMotionDataAccessor::DeleteEvent (DWORD dwIndex)
{
	if (dwIndex >= m_MotionEventDataVector.size())
	{
		return;
	}

	TMotionEventData * pData = m_MotionEventDataVector[dwIndex];
	delete pData;

	// Delete Event in the vector
	TMotionEventDataVector::iterator itor = m_MotionEventDataVector.begin();
	for (DWORD i = 0; i < dwIndex; ++i, ++itor);
	m_MotionEventDataVector.erase (itor);
}

void CRaceMotionDataAccessor::SetComboInputStartTime (float fTime)
{
	m_ComboInputData.fInputStartTime = fTime;
}

void CRaceMotionDataAccessor::SetNextComboTime (float fTime)
{
	m_ComboInputData.fNextComboTime = fTime;
}

void CRaceMotionDataAccessor::SetComboInputEndTime (float fTime)
{
	m_ComboInputData.fInputEndTime = fTime;
}

void CRaceMotionDataAccessor::SetComboMotionEnable (BOOL bFlag)
{
	m_isComboMotion = bFlag;
}
void CRaceMotionDataAccessor::SetAttackingMotionEnable (BOOL bFlag)
{
	m_isAttackingMotion = bFlag;
}

void CRaceMotionDataAccessor::SetMotionType (int iType)
{
	m_MotionAttackData.iMotionType = iType;
}

void CRaceMotionDataAccessor::SetAttackType (int iType)
{
	m_MotionAttackData.iAttackType = iType;
}

void CRaceMotionDataAccessor::SetHittingType (int iType)
{
	m_MotionAttackData.iHittingType = iType;
}

void CRaceMotionDataAccessor::SetStiffenTime (float fTime)
{
	m_MotionAttackData.fStiffenTime = fTime;
}
void CRaceMotionDataAccessor::SetInvisibleTime (float fTime)
{
	m_MotionAttackData.fInvisibleTime = fTime;
}
void CRaceMotionDataAccessor::SetExternalForceTime (float fForce)
{
	m_MotionAttackData.fExternalForce = fForce;
}

void CRaceMotionDataAccessor::SetAttackStartTime (DWORD dwIndex, float fTime)
{
	if (dwIndex >= m_MotionAttackData.HitDataContainer.size())
	{
		return;
	}

	NRaceData::THitData & rHitData = m_MotionAttackData.HitDataContainer[dwIndex];
	rHitData.fAttackStartTime = fTime;
	RefreshTimeHitPosition();
}

void CRaceMotionDataAccessor::SetAttackEndTime (DWORD dwIndex, float fTime)
{
	if (dwIndex >= m_MotionAttackData.HitDataContainer.size())
	{
		return;
	}

	NRaceData::THitData & rHitData = m_MotionAttackData.HitDataContainer[dwIndex];
	rHitData.fAttackEndTime = fTime;
	RefreshTimeHitPosition();
}

void CRaceMotionDataAccessor::SetAttackingBoneName (DWORD dwIndex, const std::string & strBoneName)
{
	if (dwIndex >= m_MotionAttackData.HitDataContainer.size())
	{
		return;
	}

	NRaceData::THitData & rHitData = m_MotionAttackData.HitDataContainer[dwIndex];
	rHitData.strBoneName = strBoneName;
	RefreshTimeHitPosition();
}

void CRaceMotionDataAccessor::SetAttackingWeaponLength (DWORD dwIndex, float fWeaponLength)
{
	if (dwIndex >= m_MotionAttackData.HitDataContainer.size())
	{
		return;
	}

	NRaceData::THitData & rHitDataContainer = m_MotionAttackData.HitDataContainer[dwIndex];
	rHitDataContainer.fWeaponLength = fWeaponLength;
	RefreshTimeHitPosition();
}

void CRaceMotionDataAccessor::RefreshTimeHitPosition()
{
	CWorldEditorApp * pApp = (CWorldEditorApp*) AfxGetApp();
	CSceneObject * pObject = pApp->GetSceneObject();

	pObject->BuildTimeHitPosition (&m_MotionAttackData);
}

DWORD CRaceMotionDataAccessor::GetHitDataCount() const
{
	const NRaceData::TMotionAttackData * pMotionAttackData = GetMotionAttackDataPointer();
	return pMotionAttackData->HitDataContainer.size();
}

const NRaceData::THitData* CRaceMotionDataAccessor::GetHitDataPtr (DWORD dwIndex) const
{
	const NRaceData::TMotionAttackData * pMotionAttackData = GetMotionAttackDataPointer();
	if (dwIndex >= pMotionAttackData->HitDataContainer.size())
	{
		return NULL;
	}
	return &pMotionAttackData->HitDataContainer[dwIndex];
}

void CRaceMotionDataAccessor::InsertHitData()
{
	m_MotionAttackData.HitDataContainer.push_back (NRaceData::SHitData());
	NRaceData::THitData & rHitData = * (m_MotionAttackData.HitDataContainer.rbegin());
	rHitData.fAttackStartTime = 0.0f;
	rHitData.fAttackEndTime = 0.0f;
	rHitData.fWeaponLength = 0.0f;
	rHitData.strBoneName = "Bip01";
	rHitData.mapHitPosition.clear();
	RefreshTimeHitPosition();
}

void CRaceMotionDataAccessor::DeleteHitData (DWORD dwIndex)
{
	if (dwIndex >= m_MotionAttackData.HitDataContainer.size())
	{
		return;
	}

	DeleteVectorItem<NRaceData::THitData> (&m_MotionAttackData.HitDataContainer, dwIndex);
	RefreshTimeHitPosition();
}

void CRaceMotionDataAccessor::SetHitLimitCount (int iCount)
{
	m_MotionAttackData.iHitLimitCount = iCount;
}

void CRaceMotionDataAccessor::SetMotionFileName (const char* c_szFileName)
{
	m_strMotionFileName = c_szFileName;
}

void CRaceMotionDataAccessor::SetLoopCount (int iLoopCount)
{
	m_iLoopCount = iLoopCount;
}

void CRaceMotionDataAccessor::SetLoopMotionEnable (BOOL bFlag)
{
	m_isLoopMotion = bFlag;
}

void CRaceMotionDataAccessor::SetLoopStartTime (float fTime)
{
	m_fLoopStartTime = fTime;
}

void CRaceMotionDataAccessor::SetLoopEndTime (float fTime)
{
	m_fLoopEndTime = fTime;
}

void CRaceMotionDataAccessor::SetCancelEnable (BOOL bEnable)
{
	m_bCancelEnableSkill = bEnable;
}

void CRaceMotionDataAccessor::ClearAccessor()
{
	m_iLoopCount = 0;
	m_fMotionDuration = 0.0f;

	m_isComboMotion = FALSE;
	m_ComboInputData.fInputStartTime = 0.3f;
	m_ComboInputData.fNextComboTime = 0.4f;
	m_ComboInputData.fInputEndTime = 0.5f;

	m_isAttackingMotion = FALSE;
	m_MotionAttackData.fExternalForce = 0.0f;
	m_MotionAttackData.fInvisibleTime = 0.0f;
	m_MotionAttackData.fStiffenTime = 0.0f;
	m_MotionAttackData.iAttackType = 0;
	m_MotionAttackData.iMotionType = 0;
	m_MotionAttackData.iHittingType = 0;
	m_MotionAttackData.iHitLimitCount = 0;
	m_MotionAttackData.HitDataContainer.clear();

	m_isLoopMotion = FALSE;
	m_fLoopStartTime = 0.0f;

	m_bCancelEnableSkill = FALSE;

	m_SoundInstanceVector.clear();
	stl_wipe (m_MotionEventDataVector);
}

CRaceMotionDataAccessor::CRaceMotionDataAccessor()
{
	ClearAccessor();
}
CRaceMotionDataAccessor::~CRaceMotionDataAccessor()
{
}
