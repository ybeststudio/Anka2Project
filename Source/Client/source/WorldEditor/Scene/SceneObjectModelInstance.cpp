#include "StdAfx.h"
#include "SceneObject.h"
#include "../../EterLib/Camera.h"
#include "../../EffectLib/EffectManager.h"
#include "../../GameLib/itemdata.h"

void CSceneObject::Refresh()
{
	CEffectManager::Instance().DeleteAllInstances();

	// NOTE : LocalTime 초기화. 해주지 않을 시에 다음 동작의 시작 시간이 바뀌어 버릴 때가 있다.
	//        함수를 따로 만들어야겠다. - [levites]
	SetLocalTime (0.0f);
	m_fMotionStartTime = 0.0f;
	m_fDuration = m_ActorInstanceAccessor.GetMotionDuration();
	SendLocalTimeToObserver (m_fDuration, m_fLocalTime);
	m_ActorInstanceAccessor.SetLocalTime (m_fLocalTime);

	/////

	CGraphicThing * pModel;
	CGraphicThing * pMotion;
	if (!m_pObjectData->GetModelThing (&pModel))
	{
		return;
	}
	if (!m_pObjectData->GetMotionThing (&pMotion))
	{
		return;
	}

	m_ActorInstanceAccessor.Clear();
	if (SetModelThing (pModel) == false)
	{
		return;
	}
	if (SetMotionThing (pMotion) == false)
	{
		return;
	}

	BuildAttachingData();
	m_ActorInstanceAccessor.SetMotionData (m_pObjectData->GetMotionDataPointer());
}

void CSceneObject::BuildAttachingData()
{
	/*
	m_pObjectData->CreateAttachingEffectData();
	for (DWORD i = 0; i < m_pObjectData->GetAttachingDataCount(); ++i)
	{
		NRaceData::TAttachingData * pAttachingData;
		if (m_pObjectData->GetAttachingDataPointer(i, &pAttachingData))
		{
			switch (pAttachingData->dwType)
			{
				case NRaceData::ATTACHING_DATA_TYPE_COLLISION_DATA:
					break;
				case NRaceData::ATTACHING_DATA_TYPE_EFFECT:
					AddAttachingEffect(pAttachingData);
					break;
				case NRaceData::ATTACHING_DATA_TYPE_OBJECT:
					AddAttachingObject(pAttachingData);
					break;
			}
		}
	}
	*/
	RefreshAttachingData();
}

void CSceneObject::AddAttachingEffect (const NRaceData::TAttachingData * c_pAttachingData)
{
}

void CSceneObject::AddAttachingObject (const NRaceData::TAttachingData * c_pAttachingData)
{
}

void CSceneObject::RefreshAttachingData()
{
	m_ActorInstanceAccessor.ClearAttachingEffect();
	m_ActorInstanceAccessor.ClearAttachingObject();
	for (DWORD i = 0; i < m_pObjectData->GetAttachingDataCount(); ++i)
	{
		NRaceData::TAttachingData *pData;
		if (!m_pObjectData->GetAttachingDataPointer (i, &pData))
		{
			continue;
		}

		switch (pData->dwType)
		{
			case NRaceData::ATTACHING_DATA_TYPE_EFFECT:
			{
				if (pData->isAttaching)
				{
					m_ActorInstanceAccessor.AttachEffectByName (0, pData->strAttachingBoneName.c_str(), pData->pEffectData->strFileName.c_str());
				}
				else
				{
					m_ActorInstanceAccessor.AttachEffectByName (0, 0, pData->pEffectData->strFileName.c_str());
				}
				break;
			}

			case NRaceData::ATTACHING_DATA_TYPE_OBJECT:
			{
				NRaceData::TAttachingObjectData * pObjectData = pData->pObjectData;
				m_ActorInstanceAccessor.AttachObject (pObjectData->strFileName.c_str(), pData->strAttachingBoneName.c_str());
				break;
			}
		}
	}
}

void CSceneObject::FitCamera()
{
	D3DXVECTOR3 v3Min, v3Max;
	m_ActorInstanceAccessor.SetLocalTime (0.1f);
	m_ActorInstanceAccessor.Deform();
	m_ActorInstanceAccessor.GetBoundBox (&v3Min, &v3Max);

	float fHeight = fMAX (v3Max.z - v3Min.z, 100.0f);

	CCamera * pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();
	pCurrentCamera->SetDistance (fHeight * 5.0f);
}

BOOL CSceneObject::SetModelThing (CGraphicThing * pThing)
{
	if (!m_ActorInstanceAccessor.SetAccessorModel (pThing))
	{
		ClearModelThing();
		ClearMotionThing();
		return false;
	}
	return true;
}

BOOL CSceneObject::SetMotionThing (CGraphicThing * pThing)
{
	if (!m_ActorInstanceAccessor.SetAccessorMotion (pThing))
	{
		ClearMotionThing();
		return false;
	}

	m_fDuration = m_ActorInstanceAccessor.GetMotionDuration();

	return true;
}

void CSceneObject::ClearModelThing()
{
	m_ActorInstanceAccessor.ClearModel();
}

void CSceneObject::ClearMotionThing()
{
	m_ActorInstanceAccessor.ClearMotion();
}
