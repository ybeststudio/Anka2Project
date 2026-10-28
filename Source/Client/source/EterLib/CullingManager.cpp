#include "StdAfx.h"
#include "CullingManager.h"
#include "GrpObjectInstance.h"

void CCullingManager::RayTraceCallback(const Vector3d &/*p1*/,          // source pos of ray
							  const Vector3d &/*dir*/,          // dest pos of ray
							  float distance,
							  const Vector3d &/*sect*/,
							  SpherePack *sphere)
{
	if (m_RayFarDistance<=0.0f || m_RayFarDistance>=distance)
	{
#ifdef SPHERELIB_STRICT
		if (sphere->IS_SPHERE)
			puts("CCullingManager::RayTraceCallback");
#endif
		m_list.push_back((CGraphicObjectInstance *)sphere->GetUserData());
	}
}


void CCullingManager::VisibilityCallback(const Frustum &/*f*/,SpherePack *sphere,ViewState state)
{
#ifdef SPHERELIB_STRICT
		if (sphere->IS_SPHERE)
			puts("CCullingManager::VisibilityCallback");
#endif

	CGraphicObjectInstance * pInstance = (CGraphicObjectInstance*)sphere->GetUserData();
	if (state == VS_OUTSIDE)
	{
		pInstance->Hide();
	}
	else
	{
		pInstance->Show();
	}
}

void CCullingManager::RangeTestCallback(const Vector3d &/*p*/,float /*distance*/,SpherePack *sphere,ViewState state)
{
#ifdef SPHERELIB_STRICT
		if (sphere->IS_SPHERE)
			puts("CCullingManager::RangeTestCallback");
#endif
	if (state!=VS_OUTSIDE)
	{
		m_list.push_back((CGraphicObjectInstance *)sphere->GetUserData());
	}
}

void CCullingManager::Reset()
{
	m_Factory->Reset();
}

void CCullingManager::Update()
{
	m_Factory->Process();
}

void CCullingManager::Process()
{
	UpdateViewMatrix();
	UpdateProjMatrix();
	BuildViewFrustum();
	m_Factory->FrustumTest(GetFrustum(), this);
}

CCullingManager::CullingHandle CCullingManager::Register(CGraphicObjectInstance * obj)
{
	assert(obj);
	Vector3d center;
	float radius;
	obj->GetBoundingSphere(center,radius);
	return m_Factory->AddSphere_(center,radius,obj, false);
}

void CCullingManager::Unregister(CullingHandle h)
{
	m_Factory->Remove(h);
}

CCullingManager::CCullingManager()
{
	m_Factory = new SpherePackFactory(
		10000,	// maximum count
		6400,	// root radius
		1600,	// leaf radius
		400		// extra radius
		);
}

CCullingManager::~CCullingManager()
{
	delete m_Factory;
}

void CCullingManager::FindRange(const Vector3d &p, float radius)
{
	m_list.clear();
	m_Factory->RangeTest(p, radius, this);
}

void CCullingManager::FindRay(const Vector3d &p1, const Vector3d &dir)
{
	m_RayFarDistance = -1;
	m_list.clear();
	m_Factory->RayTrace(p1,dir,this);
}

void CCullingManager::FindRayDistance(const Vector3d &p1, const Vector3d &dir, float distance)
{
	m_RayFarDistance = distance;
	m_list.clear();
	m_Factory->RayTrace(p1,dir,this);
}