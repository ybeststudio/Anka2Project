#include "StdAfx.h"

#include "../WorldEditor.h"
#include "../../EterLib/Camera.h"
#include "../MainFrm.h"

void CSceneObject::UpdateActorInstanceMotion()
{
	m_ActorInstanceAccessor.SetMotionData (m_pObjectData->GetMotionDataPointer());
	m_ActorInstanceAccessor.SetMotion();
}

void CSceneObject::OnUpdate()
{
	static float bit_more = 0;
	static int s_iLastTime = timeGetTime();
	int iCurrentTime = timeGetTime();
	float fElapsedTime = (iCurrentTime - s_iLastTime) / 1000.0f;

	fElapsedTime *= m_fMotionSpeed;
	s_iLastTime = iCurrentTime;

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity (&matIdentity);

	m_ActorInstanceAccessor.SetLocalTime (m_fLocalTime - fmod (m_fLocalTime, float (1 / 60.0)) + 0.001f);
	m_ActorInstanceAccessor.Update();
	m_ActorInstanceAccessor.Deform();
	m_ActorInstanceAccessor.UpdateAttachingInstances();
	m_ActorInstanceAccessor.UpdateAttachingObject();

	if (m_pObjectData->isMotionThing())
		if (m_isPlay)
		{
			fElapsedTime += bit_more;
			while (fElapsedTime >= 1.0f / 60)
			{
				SendLocalTimeToObserver (m_fDuration, m_fLocalTime);
				m_ActorInstanceAccessor.SetLocalTime (m_fLocalTime);
				m_ActorInstanceAccessor.Deform();
				m_ActorInstanceAccessor.MotionEventProcess();
				m_ActorInstanceAccessor.UpdateAttachingInstances();
				m_fLocalTime += 1.0f / 60;
				fElapsedTime -= 1.0f / 60;
			}
			bit_more = fElapsedTime;

			CRaceMotionDataAccessor * pMotionData = m_pObjectData->GetMotionDataPointer();
			if (pMotionData->IsLoopMotion())
			{
				if (m_fLocalTime > pMotionData->GetLoopEndTime())
				{
					if (m_icurLoopCount > 1 || m_icurLoopCount == -1)
					{
						if (m_icurLoopCount > 1)
						{
							--m_icurLoopCount;
						}
						SetLocalTime (pMotionData->GetLoopStartTime());
					}
				}
			}

			if (m_fLocalTime > m_fDuration)
			{
				m_icurLoopCount = m_pObjectData->GetMotionDataPointer()->GetLoopCount();
				SetLocalTime (0.0f);
				if (!m_isLoop)
				{
					m_isPlay = false;
					SendStopToObserver();
				}
			}
		}

	CFlyingManager::Instance().Update();
	CEffectManager::Instance().Update();
}

void CSceneObject::SelectData (NRaceData::TAttachingData * pAttachingData)
{
	m_pSelectedAttachingData = pAttachingData;
}

int GetOperation (int iOperation)
{
	return iOperation + 1;
}

void CSceneObject::OnRenderUI (float fx, float fy)
{
}

void CSceneObject::OnRender (BOOL bClear)
{
	if (bClear)
	{
		CScreen::SetClearColor (m_ClearColor.r, m_ClearColor.g, m_ClearColor.b);
		CScreen::Clear();
	}

	if (!isModelData())
	{
		return;
	}

	D3DMATERIAL9 mat;
	mat.Ambient = D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f);
	mat.Diffuse = D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f);
	mat.Emissive = D3DXCOLOR (0.5f, 0.5f, 0.5f, 0.0f);
	mat.Specular = D3DXCOLOR (0.0f, 0.0f, 0.0f, 0.0f);
	STATEMANAGER.SetMaterial (&mat);

	CLightManager::Instance().FlushLight();

	STATEMANAGER.SetTextureStageState (0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState (0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState (0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState (0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState (0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState (0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG2);

	m_ActorInstanceAccessor.Render();
	m_ActorInstanceAccessor.RenderAttachingObject();

	Identity();
	CLightManager::Instance().RestoreLight();

	if (m_isShowingMainCharacter)
	{
		RenderBackGroundCharacter (200.0f, -200.0f, 0.0f);
	}

	if (m_isShowingCollisionData)
	{
		RenderCollisionData();
	}

	// Rendering Hit Position
	__RenderHitData();

	// Render Splash
	__RenderSplash();

	///////////////////////////////////////////////////////////////////////////////////////////////
	CEffectManager::Instance().Render();

	///////////////////////////////////////////////////////////////////////////////////////////////
	RenderEffectPosition();
	RenderGrid();

	// fly
	CPickingArrows pa;
	pa.SetCenterPosition (m_v3Target);
	pa.Render();

	CFlyingManager::Instance().Render();
}

void CSceneObject::__RenderHitData()
{
	CRaceMotionDataAccessor * pMotionData = m_pObjectData->GetMotionDataPointer();
	STATEMANAGER.SetRenderState (D3DRS_TEXTUREFACTOR, 0xffff0000);
	SetDiffuseColor (1.0f, 0.0f, 0.0f);

	if (pMotionData->isAttackingMotion())
	{
		const NRaceData::TMotionAttackData & c_rMotionAttackData = pMotionData->GetMotionAttackDataReference();

		if (m_dwcurHitDataIndex >= c_rMotionAttackData.HitDataContainer.size())
		{
			return;
		}

		const NRaceData::THitData & c_rHitData = c_rMotionAttackData.HitDataContainer[m_dwcurHitDataIndex];
		if (!c_rHitData.mapHitPosition.empty())
		{
			NRaceData::THitTimePositionMap::const_iterator it;
			for (it = c_rHitData.mapHitPosition.begin(); it != c_rHitData.mapHitPosition.end(); ++it)
			{
				RenderSphere (0,
							  it->second.v3LastPosition.x,
							  it->second.v3LastPosition.y,
							  it->second.v3LastPosition.z,
							  15.0f,
							  D3DFILL_WIREFRAME);
				RenderSphere (0,
							  it->second.v3Position.x,
							  it->second.v3Position.y,
							  it->second.v3Position.z,
							  15.0f,
							  D3DFILL_WIREFRAME);
				RenderLine3d (it->second.v3LastPosition.x,
							  it->second.v3LastPosition.y,
							  it->second.v3LastPosition.z,
							  it->second.v3Position.x,
							  it->second.v3Position.y,
							  it->second.v3Position.z);
			}
		}
	}
}

void CSceneObject::__RenderSplash()
{
	CRaceMotionDataAccessor * pMotionData = m_pObjectData->GetMotionDataPointer();

	if (!pMotionData)
	{
		return;
	}

	DWORD dwMotionEventCount = pMotionData->GetMotionEventDataCount();
	for (DWORD i = 0; i < dwMotionEventCount; ++i)
	{
		CRaceMotionDataAccessor::TMotionEventData * pMotionEventData;
		if (!pMotionData->GetMotionEventDataPointer (i, &pMotionEventData))
		{
			continue;
		}

		if (CRaceMotionDataAccessor::MOTION_EVENT_TYPE_SPECIAL_ATTACKING != pMotionEventData->iType)
		{
			continue;
		}

		CRaceMotionDataAccessor::TMotionAttackingEventData * pAttackData = (CRaceMotionDataAccessor::TMotionAttackingEventData*)pMotionEventData;
		if (pAttackData->CollisionData.SphereDataVector.empty())
		{
			continue;
		}

		CSphereCollisionInstance & rSphereInstance = pAttackData->CollisionData.SphereDataVector[0];
		rSphereInstance.Render (D3DFILL_WIREFRAME);
	}
}

void CSceneObject::RenderCollisionData()
{
	for (DWORD i = 0; i < m_pObjectData->GetAttachingDataCount(); ++i)
	{
		NRaceData::TAttachingData * pAttachingData;
		if (!m_pObjectData->GetCollisionDataPointer (i, &pAttachingData))
		{
			continue;
		}

		RenderCollisionData (&m_ActorInstanceAccessor, pAttachingData);
	}
}

void CSceneObject::RenderCollisionData (CActorInstanceAccessor * pInstance, const NRaceData::TAttachingData * c_pAttachingData)
{
	D3DXVECTOR3 v3Center (0.0f, 0.0f, 0.0f);

	D3DXMATRIX * c_pmatBone = NULL;
	D3DXMATRIX * pmatCompositeBone = NULL;
	D3DXMATRIX matCompositeBone;
	D3DXMatrixIdentity (&matCompositeBone);

	if (c_pAttachingData->isAttaching)
	{
		int iBoneIndex = 0;
		if (pInstance->GetBoneIndexByName (c_pAttachingData->strAttachingBoneName.c_str(), &iBoneIndex))
		{
			pInstance->GetBoneMatrix (iBoneIndex, &c_pmatBone);
			pInstance->GetCompositeBoneMatrix (0, iBoneIndex, &pmatCompositeBone);

			if (pmatCompositeBone)
			{
				matCompositeBone = *pmatCompositeBone;
			}

			if (c_pmatBone)
			{
				v3Center.x = c_pmatBone->_41;
				v3Center.y = c_pmatBone->_42;
				v3Center.z = c_pmatBone->_43;
				matCompositeBone._41 = c_pmatBone->_41;
				matCompositeBone._42 = c_pmatBone->_42;
				matCompositeBone._43 = c_pmatBone->_43;
			}
		}
	}

	NRaceData::TCollisionData * pCollisionData = c_pAttachingData->pCollisionData;

	switch (pCollisionData->iCollisionType)
	{
		case NRaceData::COLLISION_TYPE_ATTACKING:
			STATEMANAGER.SetRenderState (D3DRS_TEXTUREFACTOR, D3DXCOLOR (1.0f, 0.0f, 0.0f, 1.0f));
			break;

		case NRaceData::COLLISION_TYPE_DEFENDING:
			STATEMANAGER.SetRenderState (D3DRS_TEXTUREFACTOR, D3DXCOLOR (0.0f, 0.0f, 1.0f, 1.0f));
			break;

		case NRaceData::COLLISION_TYPE_BODY:
			STATEMANAGER.SetRenderState (D3DRS_TEXTUREFACTOR, D3DXCOLOR (0.0f, 1.0f, 0.0f, 1.0f));
			break;
	}

	for (DWORD j = 0; j < pCollisionData->SphereDataVector.size(); ++j)
	{
		TSphereData & rSphereData = pCollisionData->SphereDataVector[j].GetAttribute();

		RenderSphere (&matCompositeBone, rSphereData.v3Position.x, rSphereData.v3Position.y, rSphereData.v3Position.z, rSphereData.fRadius, D3DFILL_WIREFRAME);

		if (c_pmatBone)
		{
			STATEMANAGER.SaveRenderState (D3DRS_ZENABLE, FALSE);
			SetDiffuseColor (1.0f, 1.0f, 1.0f);
			RenderCube (v3Center.x - 2.5f, v3Center.y - 2.5f, v3Center.z - 2.5f,
						v3Center.x + 2.5f, v3Center.y + 2.5f, v3Center.z + 2.5f);
			STATEMANAGER.RestoreRenderState (D3DRS_ZENABLE);
		}
	}
}

void CSceneObject::RenderEffectPosition()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	if (pObjectData->GetModelTypeReference() == CObjectData::MODEL_TYPE_OBJECT && m_pSelectedAttachingData &&
	m_pSelectedAttachingData->dwType == NRaceData::ATTACHING_DATA_TYPE_EFFECT)
	{
		STATEMANAGER.SaveRenderState (D3DRS_ZENABLE, FALSE);
		STATEMANAGER.SaveRenderState (D3DRS_TEXTUREFACTOR, 0xffffffff);
		RenderSphere (0,
					  m_pSelectedAttachingData->pEffectData->v3Position.x,
					  m_pSelectedAttachingData->pEffectData->v3Position.y,
					  m_pSelectedAttachingData->pEffectData->v3Position.z,
					  15, D3DFILL_WIREFRAME
					 );

		STATEMANAGER.RestoreRenderState (D3DRS_TEXTUREFACTOR);
		STATEMANAGER.RestoreRenderState (D3DRS_ZENABLE);
	}
}

void CSceneObject::SetMotionSpeed (float fSpeed)
{
	m_fMotionSpeed = fSpeed;
}

BOOL CSceneObject::isModelData()
{
	return m_ActorInstanceAccessor.IsModelThing();
}

BOOL CSceneObject::isMotionData()
{
	return m_ActorInstanceAccessor.IsMotionThing();
}

float CSceneObject::GetMotionDuration()
{
	return m_ActorInstanceAccessor.GetMotionDuration();
}

DWORD CSceneObject::GetBoneCount()
{
	return m_ActorInstanceAccessor.GetBoneCount();
}

BOOL CSceneObject::GetBoneName (DWORD dwIndex, std::string * pstrBoneName)
{
	return m_ActorInstanceAccessor.GetBoneName (dwIndex, pstrBoneName);
}

BOOL CSceneObject::GetBoneIndex (const char* c_szBoneName, DWORD * pdwIndex)
{
	for (DWORD i = 0; i < GetBoneCount(); ++i)
	{
		std::string strBoneName;
		if (GetBoneName (i, &strBoneName))
		{
			if (0 == strBoneName.compare (c_szBoneName))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CSceneObject::SetCurrentHitDataIndex (DWORD dwIndex)
{
	m_dwcurHitDataIndex = dwIndex;
}

void CSceneObject::SetObjectData (CObjectData * pObjectData)
{
	m_pObjectData = pObjectData;
}

void CSceneObject::SetLocalTime (float fTime)
{
	m_fLocalTime = fMAX (0.001f, fTime);
}

void CSceneObject::SetLocalPercentTime (float fPercentage)
{
	m_fLocalTime = fMAX (0.001f, m_fDuration * fPercentage);
}

float CSceneObject::GetLocalTime()
{
	return m_fLocalTime;
}

bool CSceneObject::isPlay()
{
	return m_isPlay;
}

void CSceneObject::Play (bool isLoop)
{
	m_icurLoopCount = m_pObjectData->GetMotionDataPointer()->GetLoopCount();

	m_isPlay = true;
	m_isLoop = isLoop;
	m_fMotionStartTime = timeGetTime() / 1000.0f;
	SendPlayToObserver();
}

void CSceneObject::Stop()
{
	m_isPlay = false;
	SendStopToObserver();
}

void CSceneObject::OnKeyDown (int iChar)
{
	if (32 == iChar) // Space
	{
		if (!isPlay())
		{
			Play (false);
		}
		else
		{
			SetLocalTime (0.0f);
			Play (false);
		}
	}
}

void CSceneObject::OnKeyUp (int iChar)
{
}

void CSceneObject::OnMouseMove (LONG x, LONG y)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	if (pObjectData->GetModelTypeReference() == CObjectData::MODEL_TYPE_OBJECT)
		if (m_pSelectedAttachingData)
			if (m_pSelectedAttachingData->dwType == NRaceData::ATTACHING_DATA_TYPE_EFFECT)
			{
				if (m_bMoveSelectedData)
				{
					POINT p;
					GetCursorPos (&p);
					m_pSelectedAttachingData->pEffectData->v3Position.x += (p.x - m_poMoveSelectedData.x);
					m_pSelectedAttachingData->pEffectData->v3Position.y -= (p.y - m_poMoveSelectedData.y);
					m_poMoveSelectedData = p;
				}
				if (m_bMoveZSelectedData)
				{
					POINT p;
					GetCursorPos (&p);
					m_pSelectedAttachingData->pEffectData->v3Position.z -= (p.y - m_poMoveZSelectedData.y);
					m_poMoveZSelectedData = p;
				}
			}

	if (m_iGrippedDirection != -1)
	{
		GetCursorPosition (&ms_vecMousePosition.x, &ms_vecMousePosition.y, &ms_vecMousePosition.z);

		switch (m_iGrippedDirection)
		{
			case CPickingArrows::DIRECTION_X:
				m_v3Target.x = m_vecGrippedValue.x + (ms_vecMousePosition.x - m_vecGrippedPosition.x);
				break;
			case CPickingArrows::DIRECTION_Y:
				m_v3Target.y = m_vecGrippedValue.y + (ms_vecMousePosition.y - m_vecGrippedPosition.y);
				break;
			case CPickingArrows::DIRECTION_Z:
				m_v3Target.z = m_vecGrippedValue.z + (ms_vecMousePosition.z - m_vecGrippedPosition.z);
				break;
			case CPickingArrows::DIRECTION_XY:
				m_v3Target.x = m_vecGrippedValue.x + (ms_vecMousePosition.x - m_vecGrippedPosition.x);
				m_v3Target.y = m_vecGrippedValue.y + (ms_vecMousePosition.y - m_vecGrippedPosition.y);
				break;
			case CPickingArrows::DIRECTION_YZ:
				m_v3Target.y = m_vecGrippedValue.y + (ms_vecMousePosition.y - m_vecGrippedPosition.y);
				m_v3Target.z = m_vecGrippedValue.z + (ms_vecMousePosition.z - m_vecGrippedPosition.z);
				break;
			case CPickingArrows::DIRECTION_ZX:
				m_v3Target.x = m_vecGrippedValue.x + (ms_vecMousePosition.x - m_vecGrippedPosition.x);
				m_v3Target.z = m_vecGrippedValue.z + (ms_vecMousePosition.z - m_vecGrippedPosition.z);
				break;
		}

		m_ActorInstanceAccessor.SetFlyTarget (m_v3Target);
		m_ActorInstanceAccessor.SetFishingPosition (m_v3Target);
	}
}

void CSceneObject::OnLButtonDown (UINT nFlags, CPoint point)
{
	if (nFlags & MK_SHIFT)
	{
		GetCursorPos (&m_poMoveZSelectedData);
		SetCapture (AfxGetApp()->GetMainWnd()->GetSafeHwnd());
		m_bMoveZSelectedData = true;
	}
	else
	{
		GetCursorPos (&m_poMoveSelectedData);
		SetCapture (AfxGetApp()->GetMainWnd()->GetSafeHwnd());
		m_bMoveSelectedData = true;
	}

	// fly
	CPickingArrows pa;
	pa.SetCenterPosition (m_v3Target);
	int iPickingDirection = pa.Picking();
	if (iPickingDirection != -1)
	{
		m_iGrippedDirection = iPickingDirection;
		m_vecGrippedPosition = ms_vecMousePosition;
		m_vecGrippedValue = m_v3Target;
	}
}

void CSceneObject::OnLButtonUp()
{
	m_bMoveSelectedData = false;
	m_bMoveZSelectedData = false;
	ReleaseCapture();

	// fly
	m_iGrippedDirection = -1;
}

void CSceneObject::OnRButtonDown()
{
	SetCapture (AfxGetApp()->GetMainWnd()->GetSafeHwnd());
}

void CSceneObject::OnRButtonUp()
{
	ReleaseCapture();
}

BOOL CSceneObject::OnMouseWheel (short zDelat)
{
	return FALSE;
}

void CSceneObject::OnMovePosition (float fx, float fy)
{
	D3DXVECTOR3 pv3Position = ms_Camera->GetTarget();

	if (pv3Position.x + fx <= -1000.0f)
	{
		fx = -pv3Position.x - 1000.0f;
	}
	else if (pv3Position.x + fx >= 1000.0f)
	{
		fx = -pv3Position.x + 1000.0f;
	}
	if (pv3Position.y + fy <= -1000.0f)
	{
		fy = -pv3Position.y - 1000.0f;
	}
	else if (pv3Position.y + fy >= 1000.0f)
	{
		fy = -pv3Position.y + 1000.0f;
	}

	CCameraManager::Instance().GetCurrentCamera()->Move (D3DXVECTOR3 (fx, fy, 0.0f));
}

void CSceneObject::BuildTimeHitPosition (NRaceData::TMotionAttackData * pMotionAttackData)
{
	NRaceData::THitDataContainer::iterator itor = pMotionAttackData->HitDataContainer.begin();

	for (; itor != pMotionAttackData->HitDataContainer.end(); ++itor)
	{
		NRaceData::THitData & rHitData = *itor;

		rHitData.mapHitPosition.clear();

		CGraphicThing * pThing;
		pThing = (CGraphicThing*)CResourceManager::Instance().GetResourcePointer ("D:\\Ymir Work\\item/weapon/00010.gr2");
		m_ActorInstanceAccessor.RegisterModelThing (1, pThing);
		m_ActorInstanceAccessor.SetModelInstance (1, 1, 0);
		m_ActorInstanceAccessor.AttachModelInstance (0, rHitData.strBoneName.c_str(), 1);

		for (float fTime = rHitData.fAttackStartTime; fTime < rHitData.fAttackEndTime; fTime += 1.0f / 120)
		{
			m_ActorInstanceAccessor.SetLocalTime (fTime);
			m_ActorInstanceAccessor.Update();
			m_ActorInstanceAccessor.Deform();

			D3DXMATRIX * pBoneMat, * pCompositeBoneMat;
			m_ActorInstanceAccessor.CGraphicThingInstance::GetBoneMatrix (1, 0, &pBoneMat);
			m_ActorInstanceAccessor.CGraphicThingInstance::GetCompositeBoneMatrix (1, 0, &pCompositeBoneMat);

			D3DXMATRIX matBone;

			CDynamicSphereInstance s;

			s.fRadius = 15.0f;

			matBone = *pCompositeBoneMat;
			s.v3LastPosition.x = matBone._41 = pBoneMat->_41;
			s.v3LastPosition.y = matBone._42 = pBoneMat->_42;
			s.v3LastPosition.z = matBone._43 = pBoneMat->_43;

			D3DXVECTOR3 v3Top (0.0f, 0.0f, rHitData.fWeaponLength);
			D3DXVec3TransformCoord (&v3Top, &v3Top, &matBone);
			s.v3Position = v3Top;

			rHitData.mapHitPosition[fTime] = s;
		}

		m_ActorInstanceAccessor.RegisterModelThing (1, NULL);
		m_ActorInstanceAccessor.SetModelInstance (1, 1, 0);
	}
}

void CSceneObject::RegisterObserver (ISceneObserver * pObserver)
{
	m_SceneObserverList.push_back (pObserver);
}

void CSceneObject::SendLocalTimeToObserver (float fDuration, float fLocalTime)
{
	for (TSceneObserverIterator itor = m_SceneObserverList.begin(); itor != m_SceneObserverList.end(); ++itor)
	{
		ISceneObserver * pObserver = *itor;
		pObserver->SetLocalTime (fDuration, fLocalTime);
	}
}

void CSceneObject::SendPlayToObserver()
{
	for (TSceneObserverIterator itor = m_SceneObserverList.begin(); itor != m_SceneObserverList.end(); ++itor)
	{
		ISceneObserver * pObserver = *itor;
		pObserver->Play (m_isLoop);
	}
}

void CSceneObject::SendStopToObserver()
{
	for (TSceneObserverIterator itor = m_SceneObserverList.begin(); itor != m_SceneObserverList.end(); ++itor)
	{
		ISceneObserver * pObserver = *itor;
		pObserver->Stop();
	}
}

void CSceneObject::ShowMainCharacter (BOOL bFlag)
{
	m_isShowingMainCharacter = bFlag;
}

void CSceneObject::ShowCollisionData (BOOL bFlag)
{
	m_isShowingCollisionData = bFlag;
}

BOOL CSceneObject::SaveAttrFile (const char* c_szFileName)
{
	FILE * File;
	File = fopen (c_szFileName, "wt");

	D3DXVECTOR3 v3Min;
	D3DXVECTOR3 v3Max;
	m_ActorInstanceAccessor.GetBoundBox (&v3Min, &v3Max);
	fprintf (File, "%.2f\t%.2f\t%.2f\t%.2f\n", v3Min.x, v3Min.y, v3Max.x, v3Max.y);
	fclose (File);
	return TRUE;
}

void CSceneObject::Initialize()
{
	Refresh();
}

CSceneObject::CSceneObject()
{
	m_fLocalTime = 0.001f;
	m_fMotionStartTime = 0.0f;
	m_fMotionSpeed = 1.0f;

	m_isPlay = false;
	m_isLoop = false;

	m_icurLoopCount = 0;

	m_dwcurHitDataIndex = 0;

	m_pObjectData = NULL;

	m_ClearColor = D3DXCOLOR (0.4882f, 0.4882f, 0.4882f, 1.0f);

	m_fDuration = 0.0f;

	m_isShowingMainCharacter = FALSE;
	m_isShowingCollisionData = FALSE;

	m_pSelectedAttachingData = NULL;

	m_bMoveSelectedData = false;
	m_bMoveZSelectedData = false;

	m_iGrippedDirection = -1;

	// fly
	m_v3Target = D3DXVECTOR3 (0.0f, -300.0f, 100.0f);
	m_ActorInstanceAccessor.SetFlyTarget (m_v3Target);
}

CSceneObject::~CSceneObject()
{
}
