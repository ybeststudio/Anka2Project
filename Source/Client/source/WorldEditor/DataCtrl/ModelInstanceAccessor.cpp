#include "StdAfx.h"

void CModelInstanceAccessor::UpdateLocalTime (float fLocalTime)
{
	m_fLocalTime = fLocalTime;
}
float CModelInstanceAccessor::GetLocalTime()
{
	return m_fLocalTime;
}
float CModelInstanceAccessor::GetDuration()
{
	if (!m_pcurMotionThing)
	{
		return 0.0f;
	}
	if (m_pcurMotionThing->GetMotionCount() == 0)
	{
		return 0.0f;
	}

	return m_pcurMotionThing->GetMotionPointer (0)->GetDuration();
}

BOOL CModelInstanceAccessor::SetAccessorModel (CGraphicThing * pThing)
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
		LogBox ("Not a model file.", "Error");
		pThing->Release();
		return FALSE;
	}

	CGrannyModel * pModel = pThing->GetModelPointer (0);

	if (!pModel)
	{
		LogBox ("Not a valid model file.", "Error");
		pThing->Release();
		return FALSE;
	}


	CGraphicVertexBuffer m_kSharedDeformableVertexBuffer;
	m_kSharedDeformableVertexBuffer.Destroy();
	m_kSharedDeformableVertexBuffer.Create (
		GetDeformableVertexCount(),
		D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1,
		D3DUSAGE_WRITEONLY,
		D3DPOOL_DEFAULT);
	SetMainModelPointer (pModel, &m_kSharedDeformableVertexBuffer);
	//SetModelPointer(pModel);

	m_pModelThing = pThing;

	return TRUE;
}

BOOL CModelInstanceAccessor::SetAccessorMotion (CGraphicThing * pThing)
{
	if (!pThing)
	{
		return FALSE;
	}

	ReleaseArtificialMotion();
	ClearMotion();

	if (pThing->IsEmpty())
	{
		pThing->AddReference();
	}

	if (0 == pThing->GetMotionCount())
	{
		LogBox ("[ERROR] Not a motion file!");
		return FALSE;
	}

	CGrannyMotion * pMotion = pThing->GetMotionPointer (0);

	if (!pMotion)
	{
		LogBox ("[ERROR] Failed to load!");
		return FALSE;
	}

	SetArtificialMotion (pMotion);

	m_pcurMotionThing = pThing;

	return TRUE;
}

void CModelInstanceAccessor::SetArtificialMotion (const CGrannyMotion * pMotion)
{
	float localTime = GetLocalTime();

	if (m_pgrnCtrl)
	{
		// Delete animation at once
		//GrannySetControlEaseOutCurve(m_pgrnCtrl, localTime, localTime, 1.0f, 1.0f, 0.0f, 0.0f);
		//GrannySetControlEaseIn(m_pgrnCtrl, false);
		//GrannySetControlEaseOut(m_pgrnCtrl, true);
		GrannyFreeControl (m_pgrnCtrl);
	}

	granny_animation * pgrnAnimation = pMotion->GetGrannyAnimationPointer();
	granny_model_instance * pgrnModelInstance = m_pgrnModelInstance;

	if (NULL == pgrnModelInstance)
	{
		Tracef ("pgrnModelInstance is NULL\n");
		return;
	}

	m_pgrnCtrl = GrannyPlayControlledAnimation (localTime, pgrnAnimation, pgrnModelInstance);

	if (NULL == m_pgrnCtrl)
	{
		Tracef ("m_pgrnCtrl is NULL\n");
		return;
	}

	GrannySetControlLoopCount (m_pgrnCtrl, 0);
}

void CModelInstanceAccessor::ReleaseArtificialMotion()
{
	if (m_pgrnCtrl)
	{
		GrannyCompleteControlAt (m_pgrnCtrl, 0.0f);
		GrannySetModelClock (m_pgrnModelInstance, 0.0f);

		GrannyControlIsComplete (m_pgrnCtrl);
		GrannyFreeControlIfComplete (m_pgrnCtrl);

		m_pgrnCtrl = NULL;
	}
}

// Interceptor Functions
void CModelInstanceAccessor::Deform (const D3DXMATRIX* c_pWorldMatrix)
{
	if (!m_pModel)
	{
		return;
	}

	GrannySetModelClock (m_pgrnModelInstance, GetLocalTime());

	//m_pgrnWorldPose = m_pgrnDefaultWorldPose;
	UpdateWorldPose (c_pWorldMatrix);
	UpdateWorldMatrices (c_pWorldMatrix);

	if (m_pModel->CanDeformPNTVertices())
	{
		// WORK
		CGraphicVertexBuffer& rkDeformableVertexBuffer = __GetDeformableVertexBufferRef();
		TPNTVertex* pntVertices;
		if (rkDeformableVertexBuffer.Lock ((void**)&pntVertices))
		{
			DeformPNTVertices (pntVertices);
			rkDeformableVertexBuffer.Unlock();
		}
		else
		{
			TraceError ("GRANNY DEFORM DYNAMIC BUFFER LOCK ERROR");
		}
		// END_OF_WORK
	}
}

void CModelInstanceAccessor::UpdateWorldPose (const D3DXMATRIX* c_pWorldMatrix)
{
	granny_skeleton* pgrnSkeleton = GrannyGetSourceSkeleton (m_pgrnModelInstance);

	/////

	if (m_pgrnLocalPose)
	{
		if (m_boneCount < pgrnSkeleton->BoneCount)
		{
			GrannyFreeLocalPose (m_pgrnLocalPose);

			m_boneCount = pgrnSkeleton->BoneCount;
			m_pgrnLocalPose = GrannyNewLocalPose (m_boneCount);
		}
	}
	else
	{
		m_boneCount = pgrnSkeleton->BoneCount;
		m_pgrnLocalPose = GrannyNewLocalPose (m_boneCount);
	}

	/////

	float localTime = GetLocalTime();
	const D3DXMATRIX* pAttachBoneMatrix = (mc_pParentInstance)
										  ? (const D3DXMATRIX*) mc_pParentInstance->GetBoneMatrixPointer (m_iParentBoneIndex)
										  : NULL;

	GrannySetModelClock (m_pgrnModelInstance, localTime);
	GrannySampleModelAnimations (m_pgrnModelInstance, 0, pgrnSkeleton->BoneCount, m_pgrnLocalPose);
	GrannyBuildWorldPose (pgrnSkeleton,
						  0,
						  pgrnSkeleton->BoneCount,
						  m_pgrnLocalPose,
						  (granny_real32 const*)pAttachBoneMatrix,
						  m_pgrnWorldPoseReal);

	GrannyFreeCompletedModelControls (m_pgrnModelInstance);
}

BOOL CModelInstanceAccessor::IsModelThing()
{
	return NULL != m_pModelThing;
}

BOOL CModelInstanceAccessor::IsMotionThing()
{
	return NULL != m_pcurMotionThing;
}

DWORD CModelInstanceAccessor::GetBoneCount()
{
	CGrannyModel * pModel = m_pModelThing->GetModelPointer (0);
	granny_model * pgrnModel = pModel->GetGrannyModelPointer();
	return pgrnModel->Skeleton->BoneCount;
}

BOOL CModelInstanceAccessor::GetBoneName (DWORD dwIndex, std::string * pstrBoneName)
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

void CModelInstanceAccessor::ClearModel()
{
	if (m_pModelThing)
	{
		m_pModelThing->Release();
		m_pModelThing = NULL;
	}
}

void CModelInstanceAccessor::ClearMotion()
{
	if (m_pcurMotionThing)
	{
		m_pMotionBackupList.push_back (m_pcurMotionThing);
		m_pcurMotionThing = NULL;
	}
}

CModelInstanceAccessor::CModelInstanceAccessor()
{
	m_boneCount = 0;
	m_pgrnLocalPose = NULL;
	m_pgrnCtrl = NULL;

	m_pModelThing = NULL;
	m_pcurMotionThing = NULL;
}

CModelInstanceAccessor::~CModelInstanceAccessor()
{
	std::list<CGraphicThing*>::iterator itor = m_pMotionBackupList.begin();
	for (; itor != m_pMotionBackupList.end(); ++itor)
	{
		CGraphicThing * pThing = *itor;
		pThing->Release();
	}

	ReleaseArtificialMotion();
}
