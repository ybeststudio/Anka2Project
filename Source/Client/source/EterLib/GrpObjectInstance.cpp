#include "StdAfx.h"
#include "GrpObjectInstance.h"
#include "../eterBase/Timer.h"

void CGraphicObjectInstance::OnInitialize()
{
	ZeroMemory(m_abyPortalID, sizeof(m_abyPortalID));
}

void CGraphicObjectInstance::Clear()
{
	if (m_CullingHandle)
	{
		CCullingManager::Instance().Unregister(m_CullingHandle);
		m_CullingHandle = nullptr;
	}

	ClearHeightInstance();

#ifdef __ENABLE_STEALTH_FIX__
	m_isAlwaysHidden = false;
#endif

	m_isVisible = TRUE;

	m_v3Position.x = m_v3Position.y = m_v3Position.z = 0.0f;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	m_v3Scale.x = m_v3Scale.y = m_v3Scale.z = 1.0f;
#else
	m_v3Scale.x = m_v3Scale.y = m_v3Scale.z = 0.0f;
#endif

	m_fYaw = m_fPitch = m_fRoll = 0.0f;
	D3DXMatrixIdentity(&m_worldMatrix);

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	m_v3ScaleAcce.x = m_v3ScaleAcce.y = m_v3ScaleAcce.z = 0.0f;
	m_bAttachedAcceRace = 0;
	D3DXMatrixIdentity(&m_matAbsoluteTrans);
	D3DXMatrixIdentity(&m_matScale);
	D3DXMatrixIdentity(&m_matScaleWorld);
#endif

#ifdef ENABLE_OBJ_SCALLING
	m_v3ScalePosition.x = m_v3ScalePosition.y = m_v3ScalePosition.z = 0.0f;
	D3DXMatrixIdentity(&m_ScaleMatrix);
	D3DXMatrixIdentity(&m_PositionMatrix);
	D3DXMatrixIdentity(&m_TransformMatrix);
#endif

	ZeroMemory(m_abyPortalID, sizeof(m_abyPortalID));

	OnClear();
}

bool CGraphicObjectInstance::Render()
{
	if (!isShow())
		return false;

	OnRender();
	return true;
}

void CGraphicObjectInstance::BlendRender()
{
	if (!isShow())
		return;

	OnBlendRender();
}

void CGraphicObjectInstance::RenderToShadowMap()
{
	if (!isShow())
		return;

	OnRenderToShadowMap();
}

void CGraphicObjectInstance::RenderShadow()
{
	if (!isShow())
		return;

	OnRenderShadow();
}

void CGraphicObjectInstance::RenderPCBlocker()
{
	if (!isShow())
		return;

	OnRenderPCBlocker();
}

void CGraphicObjectInstance::Update()
{
	OnUpdate();

	UpdateBoundingSphere();
}

void CGraphicObjectInstance::Deform()
{
	if (!isShow())
		return;

	OnDeform();
}

void CGraphicObjectInstance::Transform()
{
#ifdef ENABLE_OBJ_SCALLING
	D3DXMATRIX tmp1;
	D3DXMatrixMultiply(&tmp1, &m_PositionMatrix, &m_mRotation);

	m_worldMatrix = tmp1;
	m_worldMatrix._41 += m_v3Position.x;
	m_worldMatrix._42 += m_v3Position.y;
	m_worldMatrix._43 += m_v3Position.z;
	D3DXMatrixMultiply(&tmp1, &m_PositionMatrix, &m_ScaleMatrix);

	D3DXMATRIX tmp2;
	D3DXMatrixMultiply(&tmp2, &tmp1, &m_mRotation);

	m_TransformMatrix = tmp2;
	m_TransformMatrix._41 = m_v3ScalePosition.x + m_v3Position.x + m_TransformMatrix._41;
	m_TransformMatrix._42 = m_v3ScalePosition.y + m_v3Position.y + m_TransformMatrix._42;
	m_TransformMatrix._43 = m_v3ScalePosition.z + m_v3Position.z + m_TransformMatrix._43;
#else
	m_worldMatrix = m_mRotation;
	m_worldMatrix._41 += m_v3Position.x;
	m_worldMatrix._42 += m_v3Position.y;
	m_worldMatrix._43 += m_v3Position.z;
#endif
}

const D3DXVECTOR3 & CGraphicObjectInstance::GetPosition() const
{
	return m_v3Position;
}

const D3DXVECTOR3 & CGraphicObjectInstance::GetScale() const
{
	return m_v3Scale;
}

float CGraphicObjectInstance::GetRotation()
{
	return GetRoll();
}

float CGraphicObjectInstance::GetYaw()
{
	return m_fYaw;
}

float CGraphicObjectInstance::GetPitch()
{
	return m_fPitch;
}

float CGraphicObjectInstance::GetRoll()
{
	return m_fRoll;
}

D3DXMATRIX & CGraphicObjectInstance::GetTransform()
{
	return m_worldMatrix;
}

void CGraphicObjectInstance::SetRotationQuaternion(const D3DXQUATERNION &q)
{
	D3DXMatrixRotationQuaternion(&m_mRotation, &q);
}

void CGraphicObjectInstance::SetRotationMatrix(const D3DXMATRIX & m)
{
	m_mRotation = m;
}

void CGraphicObjectInstance::SetRotation(float fRotation)
{
	m_fYaw = 0;
	m_fPitch = 0;
	m_fRoll = fRotation;

	D3DXMatrixRotationZ(&m_mRotation, D3DXToRadian(fRotation));
}

void CGraphicObjectInstance::SetRotation(float fYaw, float fPitch, float fRoll)
{
	//m_fRotation = fRotation;
	m_fYaw = fYaw;
	m_fPitch = fPitch;
	m_fRoll = fRoll;

	D3DXMatrixRotationYawPitchRoll(&m_mRotation, D3DXToRadian(fYaw), D3DXToRadian(fPitch), D3DXToRadian(fRoll));
}

void CGraphicObjectInstance::SetPosition(float x, float y, float z)
{
	m_v3Position.x = x;
	m_v3Position.y = y;
	m_v3Position.z = z;	
}

void CGraphicObjectInstance::SetPosition(const D3DXVECTOR3 & newposition)
{
	m_v3Position = newposition;
}

#ifdef ENABLE_OBJ_SCALLING
void CGraphicObjectInstance::SetScalePosition(float x, float y, float z)
{
	m_v3ScalePosition.x = x;
	m_v3ScalePosition.y = y;
	m_v3ScalePosition.z = z;
}

void CGraphicObjectInstance::SetScale(float x, float y, float z, bool bScaleNow)
#else
void CGraphicObjectInstance::SetScale(float x, float y, float z)
#endif
{
	m_v3Scale.x = x;
	m_v3Scale.y = y;
	m_v3Scale.z = z;

#ifdef ENABLE_OBJ_SCALLING
	if (bScaleNow)
		D3DXMatrixScaling(&m_ScaleMatrix, m_v3Scale.x, m_v3Scale.y, m_v3Scale.z);
#endif
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void CGraphicObjectInstance::SetScaleWorld(float x, float y, float z)
{
	m_v3Scale.x = x;
	m_v3Scale.y = y;
	m_v3Scale.z = z;
	D3DXMatrixScaling(&m_matScaleWorld, x, y, z);
}

void CGraphicObjectInstance::SetAcceScale(float x, float y, float z)
{
	m_v3ScaleAcce.x = x;
	m_v3ScaleAcce.y = y;
	m_v3ScaleAcce.z = z;
}

void CGraphicObjectInstance::SetAcceScale(const D3DXVECTOR3& rv3Scale, BYTE bRace)
{
	m_v3ScaleAcce = rv3Scale;
	m_bAttachedAcceRace = bRace;
}
#endif

void CGraphicObjectInstance::Show()
{
	m_isVisible = true;
}

void CGraphicObjectInstance::Hide()
{
	m_isVisible = false;
}

bool CGraphicObjectInstance::isShow()
{
#ifdef __ENABLE_STEALTH_FIX__
	return m_isVisible && !m_isAlwaysHidden;
#else
	return m_isVisible;
#endif
}

#ifdef __ENABLE_STEALTH_FIX__
void CGraphicObjectInstance::ApplyAlwaysHidden()
{
	m_isAlwaysHidden = true;
}

void CGraphicObjectInstance::ReleaseAlwaysHidden()
{
	m_isAlwaysHidden = false;
}
#endif

//

//////////////////////////////////////////////////////////////////////////

D3DXVECTOR4 & CGraphicObjectInstance::GetWTBBoxVertex(const unsigned char & c_rucNumTBBoxVertex)
{
	return m_v4TBBox[c_rucNumTBBoxVertex];
}

bool CGraphicObjectInstance::isIntersect(const CRay & c_rRay, float * pu, float * pv, float * pt)
{
	D3DXVECTOR3 v3Start, v3Dir;
	float fRayRange;
	c_rRay.GetStartPoint(&v3Start);
	c_rRay.GetDirection(&v3Dir, &fRayRange);

	TPosition posVertices[8];

	posVertices[0] = TPosition(m_v3TBBoxMin.x, m_v3TBBoxMin.y, m_v3TBBoxMin.z);
	posVertices[1] = TPosition(m_v3TBBoxMax.x, m_v3TBBoxMin.y, m_v3TBBoxMin.z);
	posVertices[2] = TPosition(m_v3TBBoxMin.x, m_v3TBBoxMax.y, m_v3TBBoxMin.z);
	posVertices[3] = TPosition(m_v3TBBoxMax.x, m_v3TBBoxMax.y, m_v3TBBoxMin.z);
	posVertices[4] = TPosition(m_v3TBBoxMin.x, m_v3TBBoxMin.y, m_v3TBBoxMax.z);
	posVertices[5] = TPosition(m_v3TBBoxMax.x, m_v3TBBoxMin.y, m_v3TBBoxMax.z);
	posVertices[6] = TPosition(m_v3TBBoxMin.x, m_v3TBBoxMax.y, m_v3TBBoxMax.z);
	posVertices[7] = TPosition(m_v3TBBoxMax.x, m_v3TBBoxMax.y, m_v3TBBoxMax.z);

	TIndex Indices[36] = {0, 1, 2, 1, 3, 2,
						  2, 0, 6, 0, 4, 6,
						  0, 1, 4, 1, 5, 4,
						  1, 3, 5, 3, 7, 5,
						  3, 2, 7, 2, 6, 7,
						  4, 5, 6, 5, 7, 6};

	int triCount = 12;
	WORD* pcurIdx = (WORD*)Indices;

	while (triCount--)
	{
		if (IntersectTriangle(v3Start, v3Dir,
			posVertices[pcurIdx[0]],
			posVertices[pcurIdx[1]],
			posVertices[pcurIdx[2]],
			pu, pv, pt))
		{
			return true;
		}

		pcurIdx += 3;
	}

	return false;
}

CGraphicObjectInstance::CGraphicObjectInstance()
{
	m_CullingHandle = 0;
	Initialize();
}

void CGraphicObjectInstance::Initialize()
{
	if (m_CullingHandle)
		CCullingManager::Instance().Unregister(m_CullingHandle);
	m_CullingHandle = 0;

	m_pHeightAttributeInstance = nullptr;
	
	m_isVisible = TRUE;	

	m_BlockCamera = false;

	m_isAlwaysHidden = false;

	m_v3Position.x = m_v3Position.y = m_v3Position.z = 0.0f;

#ifdef ENABLE_OBJ_SCALLING
	m_v3Scale.x = m_v3Scale.y = m_v3Scale.z = 1.0f;
#else
	m_v3Scale.x = m_v3Scale.y = m_v3Scale.z = 0.0f;
#endif
	m_fYaw = m_fPitch = m_fRoll = 0.0f;
#ifdef __ENABLE_STEALTH_FIX__
	m_isAlwaysHidden = false;
#endif
	D3DXMatrixIdentity(&m_worldMatrix);
	D3DXMatrixIdentity(&m_mRotation);

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	m_v3ScaleAcce.x = m_v3ScaleAcce.y = m_v3ScaleAcce.z = 0.0f;
	D3DXMatrixIdentity(&m_matAbsoluteTrans);
	D3DXMatrixIdentity(&m_matScale);
	D3DXMatrixIdentity(&m_matScaleWorld);
#endif

#ifdef ENABLE_OBJ_SCALLING
	m_v3ScalePosition.x = m_v3ScalePosition.y = m_v3ScalePosition.z = 0.0f;

	D3DXMatrixIdentity(&m_ScaleMatrix);
	D3DXMatrixIdentity(&m_PositionMatrix);
	D3DXMatrixIdentity(&m_TransformMatrix);
#endif

	OnInitialize();
}

CGraphicObjectInstance::~CGraphicObjectInstance()
{
	Initialize();
}

void CGraphicObjectInstance::UpdateBoundingSphere()
{
	if (m_CullingHandle)
	{
		Vector3d center;
		float radius;
		GetBoundingSphere(center,radius);
		if (radius != m_CullingHandle->GetRadius())
			m_CullingHandle->NewPosRadius(center,radius);
		else
			m_CullingHandle->NewPos(center);
	}
}

void CGraphicObjectInstance::RegisterBoundingSphere()
{
	if (m_CullingHandle)
		CCullingManager::Instance().Unregister(m_CullingHandle);

	m_CullingHandle = CCullingManager::Instance().Register(this);
}

void CGraphicObjectInstance::AddCollision(const CStaticCollisionData * pscd, const D3DXMATRIX* pMat)
{
	m_StaticCollisionInstanceVector.push_back(CBaseCollisionInstance::BuildCollisionInstance(pscd, pMat));
}

void CGraphicObjectInstance::ClearCollision()
{
	CCollisionInstanceVector::iterator it;
	for(it = m_StaticCollisionInstanceVector.begin();it!=m_StaticCollisionInstanceVector.end();++it)
	{
		(*it)->Destroy();
	}
	m_StaticCollisionInstanceVector.clear();
}

bool CGraphicObjectInstance::CollisionDynamicSphere(const CDynamicSphereInstance & s) const
{
	CCollisionInstanceVector::const_iterator it;
	for(it = m_StaticCollisionInstanceVector.begin();it!=m_StaticCollisionInstanceVector.end();++it)
	{
		if ((*it)->CollisionDynamicSphere(s))
			return true;
	}
	return false;
}

bool CGraphicObjectInstance::MovementCollisionDynamicSphere(const CDynamicSphereInstance & s) const
{
	CCollisionInstanceVector::const_iterator it;
	for(it = m_StaticCollisionInstanceVector.begin();it!=m_StaticCollisionInstanceVector.end();++it)
	{
		if ((*it)->MovementCollisionDynamicSphere(s))
			return true;
	}
	return false;
}

D3DXVECTOR3 CGraphicObjectInstance::GetCollisionMovementAdjust(const CDynamicSphereInstance & s) const
{
	CCollisionInstanceVector::const_iterator it;
	for(it = m_StaticCollisionInstanceVector.begin();it!=m_StaticCollisionInstanceVector.end();++it)
	{
		if ((*it)->MovementCollisionDynamicSphere(s))
			return (*it)->GetCollisionMovementAdjust(s);
	}

	return D3DXVECTOR3(0.0f,0.0f,0.0f);
}

void CGraphicObjectInstance::UpdateCollisionData(const CStaticCollisionDataVector * pscdVector)
{
	ClearCollision();
	OnUpdateCollisionData(pscdVector);
}

DWORD CGraphicObjectInstance::GetCollisionInstanceCount()
{
	return m_StaticCollisionInstanceVector.size();
}

CBaseCollisionInstance * CGraphicObjectInstance::GetCollisionInstanceData(DWORD dwIndex)
{
	if (dwIndex>m_StaticCollisionInstanceVector.size())
	{
		return 0;
	}
	return m_StaticCollisionInstanceVector[dwIndex];
}

//////////////////////////////////////////////////////////////////////////
// Height

void CGraphicObjectInstance::SetHeightInstance(CAttributeInstance * pAttributeInstance)
{
	m_pHeightAttributeInstance = pAttributeInstance;
}

void CGraphicObjectInstance::ClearHeightInstance()
{
	m_pHeightAttributeInstance = nullptr;
}

void CGraphicObjectInstance::UpdateHeightInstance(CAttributeInstance * pAttributeInstance)
{
	ClearHeightInstance();
	OnUpdateHeighInstance(pAttributeInstance);
}

bool CGraphicObjectInstance::IsObjectHeight()
{
	if (m_pHeightAttributeInstance)
		return true;

	return false;
}

bool CGraphicObjectInstance::GetObjectHeight(float fX, float fY, float * pfHeight)
{
	if (!m_pHeightAttributeInstance)
		return false;

	return OnGetObjectHeight(fX, fY, pfHeight);
}

void CGraphicObjectInstance::SetPortal(DWORD dwIndex, int iID)
{
	if (dwIndex >= PORTAL_ID_MAX_NUM)
	{
		assert(dwIndex < PORTAL_ID_MAX_NUM);
		return;
	}

	m_abyPortalID[dwIndex] = iID;
}

int CGraphicObjectInstance::GetPortal(DWORD dwIndex)
{
	if (dwIndex >= PORTAL_ID_MAX_NUM)
	{
		assert(dwIndex < PORTAL_ID_MAX_NUM);
		return 0;
	}

	return m_abyPortalID[dwIndex];
}
