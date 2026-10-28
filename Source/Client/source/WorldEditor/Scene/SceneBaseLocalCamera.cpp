#include "StdAfx.h"
//#include "SceneBase.h"
//
//void CLocalCamera::SetOwner(CSceneBase * pOwner)
//{
//	m_pOwner = pOwner;
//}
//
//void CLocalCamera::GetPositionPointer(D3DXVECTOR3 ** ppv3Position)
//{
//	*ppv3Position = &m_vecPosition;
//}
//
////int CLocalCamera::GetWidth()
////{
////	return m_iWidth;
////}
////int CLocalCamera::GetHeight()
////{
////	return m_iHeight;
////}
//
////float CLocalCamera::GetDistance()
////{
////	return m_fDistance;
////}
////float CLocalCamera::GetPitch()
////{
////	return m_fPitch;
////}
////float CLocalCamera::GetRotation()
////{
////	return m_fRotation;
////}
////
//void CLocalCamera::TurnOn()
//{
////	CCamera::Instance().PushParams();
//	SetPositionCamera(m_vecPosition.x, m_vecPosition.y, m_vecPosition.z,
//						m_fDistance, m_fPitch, m_fRotation);
//}
//void CLocalCamera::TurnOff()
//{
////	CCamera::Instance().PopParams();
//}
//
///*
//void CLocalCamera::RenderGrid()
//{
//	SetColorRenderState();
//	SetDiffuseColor(ms_GridSmall.r, ms_GridSmall.g, ms_GridSmall.b);
//	for (float x = -600.0f; x <= 600.0f; x += 30.0f)
//		RenderLine2d(m_vecPosition.x + x, m_vecPosition.y + 600.0f,
//					 m_vecPosition.x + x, m_vecPosition.y + -600.0f, m_vecPosition.z);
//	for (float y = -600.0f; y <= 600.0f; y += 30.0f)
//		RenderLine2d(m_vecPosition.x + -600.0f, m_vecPosition.y + y,
//					 m_vecPosition.x + 600.0f, m_vecPosition.y + y, m_vecPosition.z);
//
//	SetDiffuseColor(ms_GridLarge.r, ms_GridLarge.g, ms_GridLarge.b);
//	RenderLine2d(m_vecPosition.x + -600.0f, m_vecPosition.y + 0.0f,
//				 m_vecPosition.x + 600.0f, m_vecPosition.y + 0.0f, m_vecPosition.z);
//	RenderLine2d(m_vecPosition.x + 0.0f, m_vecPosition.y + 600.0f,
//				 m_vecPosition.x + 0.0f, m_vecPosition.y + -600.0f, m_vecPosition.z);
//}
//*/
//
//void CLocalCamera::SetCameraPosition(float fDistance, float fPitch, float fRotation)
//{
//	m_fDistance = fDistance;
//	m_fPitch = fPitch;
//	m_fRotation = fRotation;
//}
////void CLocalCamera::SetFrustum(float fNear, float fFar)
////{
////	m_fFrustumNear = fNear;
////	m_fFrustumFar = fFar;
////}
//void CLocalCamera::MovePosition(float fx, float fy)
//{
//	assert(m_pOwner);
//	m_vecPosition.x -= fx;
//	m_vecPosition.y -= fy;
//
//	m_pOwner->OnMovePosition();
//}
//
//CLocalCamera::CLocalCamera()
//{
//	m_ixCursor = 0;
//	m_iyCursor = 0;
//
//	m_iWidth = 1;
//	m_iHeight = 1;
//
//	m_fDistance = 500.0f;
//	m_fPitch = 30.0f;
//	m_fRotation = 0.0f;
//	m_vecPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
//
//	m_pOwner = NULL;
//}
//CLocalCamera::~CLocalCamera()
//{
//}
