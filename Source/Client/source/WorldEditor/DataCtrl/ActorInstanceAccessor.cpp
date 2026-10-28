#include "StdAfx.h"
#include "ActorInstanceAccessor.h"
#include "../../EffectLib/EffectManager.h"
#include "../../GameLib/FlyingObjectManager.h"

void CActorInstanceAccessor::ClearAttachingObject()
{
	for (auto itor = m_AttachingObjectList.begin(); itor != m_AttachingObjectList.end(); ++itor)
	{
		SAttachingModelInstance & rInstance = *itor;
		rInstance.pThing->Release();
		delete rInstance.pModelInstance;
		rInstance.pModelInstance = nullptr;
	}
	m_AttachingObjectList.clear();
}

void CActorInstanceAccessor::AttachObject (const char* c_szFileName, const char* c_szBoneName)
{
	if (0 == strlen (c_szFileName))
	{
		return;
	}

	SAttachingModelInstance ModelInstance;

	ModelInstance.pThing = (CGraphicThing*)CResourceManager::Instance().GetResourcePointer (c_szFileName);
	ModelInstance.pThing->AddReference();
	if (1 != ModelInstance.pThing->GetModelCount())
	{
		return;
	}

	ModelInstance.strBoneName = c_szBoneName;
	ModelInstance.pModelInstance = new CModelInstanceAccessor;

	CGraphicVertexBuffer m_kSharedDeformableVertexBuffer;
	m_kSharedDeformableVertexBuffer.Destroy();
	m_kSharedDeformableVertexBuffer.Create (
		ModelInstance.pModelInstance->GetDeformableVertexCount(),
		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
		D3DUSAGE_WRITEONLY,
		D3DPOOL_DEFAULT);
	ModelInstance.pModelInstance->SetMainModelPointer (ModelInstance.pThing->GetModelPointer (0), &m_kSharedDeformableVertexBuffer);
	//ModelInstance.pModelInstance->SetModelPointer(ModelInstance.pThing->GetModelPointer(0));

	m_AttachingObjectList.push_back (ModelInstance);
}

void CActorInstanceAccessor::UpdateAttachingObject()
{
	CGrannyLODController * pLODController = CGraphicThingInstance::GetLODControllerPointer (0);
	if (!pLODController)
	{
		return;
	}
	CGrannyModelInstance * pModelInstance = pLODController->GetModelInstance();
	if (!pModelInstance)
	{
		return;
	}

	for (auto itor = m_AttachingObjectList.begin(); itor != m_AttachingObjectList.end(); ++itor)
	{
		SAttachingModelInstance & rInstance = *itor;
		rInstance.pModelInstance->Update (CGrannyModelInstance::ANIFPS_MIN, CGrannyModelInstance::ANIFPS_MAX);
		rInstance.pModelInstance->Deform (&GetIdentityMatrix());
		rInstance.pModelInstance->SetParentModelInstance (pModelInstance, rInstance.strBoneName.c_str());
	}
}

void CActorInstanceAccessor::RenderAttachingObject()
{
	for (auto itor = m_AttachingObjectList.begin(); itor != m_AttachingObjectList.end(); ++itor)
	{
		SAttachingModelInstance & rInstance = *itor;
		rInstance.pModelInstance->RenderWithOneTexture();
	}
}

void CActorInstanceAccessor::SetMotion()
{
	m_kCurMotNode.iMotionType = MOTION_TYPE_LOOP;
	m_kCurMotNode.fStartTime = GetLocalTime();
	m_kCurMotNode.dwMotionKey = 0;
	m_kCurMotNode.fEndTime = 0.0f;
	m_kCurMotNode.fSpeedRatio = 1.0f;
	m_kCurMotNode.dwcurFrame = 0;
	m_kCurMotNode.dwFrameCount = CActorInstance::GetMotionDuration (0) / (1.0f / g_fGameFPS);
	CGraphicThingInstance::SetMotion (0, 0.0f, 1);
}

void CActorInstanceAccessor::SetLocalTime (float fLocalTime)
{
	CGraphicThingInstance::__SetLocalTime (fLocalTime);
	m_kCurMotNode.dwcurFrame = fLocalTime * g_fGameFPS;
}

void CActorInstanceAccessor::SetMotionData (CRaceMotionData * pMotionData)
{
	m_pkCurRaceMotionData = pMotionData;

	// RegisterEffect
	for (DWORD i = 0; i < pMotionData->GetMotionEventDataCount(); ++i)
	{
		const CRaceMotionData::TMotionEventData * c_pData;
		if (!pMotionData->GetMotionEventDataPointer (i, &c_pData))
		{
			continue;
		}

		if (CRaceMotionData::MOTION_EVENT_TYPE_EFFECT == c_pData->iType)
		{
			const CRaceMotionData::TMotionEffectEventData * c_pEffectData = (const CRaceMotionData::TMotionEffectEventData*)c_pData;
			CEffectManager::Instance().RegisterEffect (c_pEffectData->strEffectFileName.c_str());
		}
		else if (CRaceMotionData::MOTION_EVENT_TYPE_FLY == c_pData->iType)
		{
			const CRaceMotionData::TMotionFlyEventData * c_pFlyData = (const CRaceMotionData::TMotionFlyEventData*)c_pData;
			CFlyingManager::Instance().RegisterFlyingData (c_pFlyData->strFlyFileName.c_str());
		}
	}
}

float CActorInstanceAccessor::GetMotionDuration()
{
	return CActorInstance::GetMotionDuration (0);
}

DWORD CActorInstanceAccessor::GetBoneCount()
{
	CGrannyModel * pModel = m_pModelThing->GetModelPointer (0);
	granny_model * pgrnModel = pModel->GetGrannyModelPointer();
	return pgrnModel->Skeleton->BoneCount;
}

BOOL CActorInstanceAccessor::GetBoneName (DWORD dwIndex, std::string * pstrBoneName)
{
	CGrannyModel * pModel = m_pModelThing->GetModelPointer (0);
	granny_model * pgrnModel = pModel->GetGrannyModelPointer();

	if (dwIndex >= DWORD (pgrnModel->Skeleton->BoneCount))
	{
		return FALSE;
	}

	*pstrBoneName = pgrnModel->Skeleton->Bones[dwIndex].Name;
	return TRUE;
}

BOOL CActorInstanceAccessor::GetBoneMatrix (DWORD dwBoneIndex, D3DXMATRIX** ppMatrix)
{
	return CGraphicThingInstance::GetBoneMatrix (0, dwBoneIndex, ppMatrix);
}

BOOL CActorInstanceAccessor::GetBoneIndexByName (const char* c_szBoneName, int* pBoneIndex) const
{
	if (m_LODControllerVector.empty())
	{
		return FALSE;
	}

	const CGrannyModelInstance * c_pModelInstance = m_LODControllerVector[0]->GetModelInstance();
	c_pModelInstance->GetBoneIndexByName (c_szBoneName, pBoneIndex);

	return TRUE;
}

BOOL CActorInstanceAccessor::SetAccessorModel (CGraphicThing * pThing)
{
	assert (pThing);
	if (!pThing)
	{
		return FALSE;
	}

	ClearModel();

	pThing->AddReference();

	if (0 == pThing->GetModelCount())
	{
		LogBox ("[ERROR] SetAccessorModel: Not a model file.", "Error");
		pThing->Release();
		return FALSE;
	}

	if (CGrannyModel * pModel = pThing->GetModelPointer (0); !pModel)
	{
		LogBox ("[ERROR] SetAccessorModel: Not a valid model file.", "Error");
		pThing->Release();
		return FALSE;
	}

	CGraphicThingInstance::ReserveModelThing (2);
	CGraphicThingInstance::ReserveModelInstance (2);
	CGraphicThingInstance::RegisterModelThing (0, pThing);
	CGraphicThingInstance::SetModelInstance (0, 0, 0);

	m_pModelThing = pThing;

	return TRUE;
}

BOOL CActorInstanceAccessor::SetAccessorMotion (CGraphicThing * pThing)
{
	if (!pThing)
	{
		return FALSE;
	}

	ClearMotion();

	if (pThing->IsEmpty())
	{
		pThing->AddReference();
	}

	if (0 == pThing->GetMotionCount())
	{
		LogBox ("[ERROR] SetAccessorMotion: Not a motion file.", "Error");
		pThing->Release();
		return FALSE;
	}

	if (CGrannyMotion * pMotion = pThing->GetMotionPointer (0); !pMotion)
	{
		LogBox ("[ERROR] SetAccessorMotion: Not a valid motion file.", "Error");
		pThing->Release();
		return FALSE;
	}

	CGraphicThingInstance::RegisterMotionThing (0, pThing);
	SetMotion();

	m_pMotionThing = pThing;

	return TRUE;
}

BOOL CActorInstanceAccessor::IsModelThing()
{
	return nullptr != m_pModelThing;
}

BOOL CActorInstanceAccessor::IsMotionThing()
{
	return nullptr != m_pMotionThing;
}

void CActorInstanceAccessor::ClearModel()
{
	if (m_pModelThing)
	{
		m_pModelThing->Release();
	}

	m_pModelThing = nullptr;
}

void CActorInstanceAccessor::ClearMotion()
{
	if (m_pMotionThing)
	{
		m_pMotionThing->Release();
	}

	m_pMotionThing = nullptr;
}

void CActorInstanceAccessor::ClearAttachingEffect()
{
	__ClearAttachingEffect();
}

CActorInstanceAccessor::CActorInstanceAccessor()
{
	m_pModelThing = nullptr;
	m_pMotionThing = nullptr;

	m_kCurMotNode.fStartTime = 0.0f;
	m_kCurMotNode.fEndTime = 0.0f;
}
CActorInstanceAccessor::~CActorInstanceAccessor()
{
}
