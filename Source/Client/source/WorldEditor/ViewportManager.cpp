#include "StdAfx.h"
#include "ViewportManager.h"
#include "../EterLib/Camera.h"

#include "MainFrm.h"
#include "WorldEditorDoc.h"

void CViewportManager::ModeButtonDown()
{
	//m_Mode = RotationMode;
}

void CViewportManager::ModeButtonUp()
{
	m_Mode = TranslateMode;
}

void CViewportManager::ControlKeyDown()
{
	ModeButtonDown();
	m_ControlKey = true;
}

void CViewportManager::ControlKeyUp()
{
	ModeButtonUp();
	m_ControlKey = false;
}

void CViewportManager::LeftButtonDown (CPoint Mouse)
{
	m_ClickedLeftButton = true;
	m_ClickedLeftButtonPoint = Mouse;
}
void CViewportManager::LeftButtonUp (CPoint Mouse)
{
	m_ClickedLeftButton = false;
	m_ClickedLeftButtonPoint = CPoint (-1, -1);
}

void CViewportManager::MiddleButtonDown (CPoint Mouse)
{
	m_ClickedMiddleButton = true;
	m_ClickedMiddleButtonPoint = Mouse;
}

void CViewportManager::MiddleButtonUp (CPoint Mouse)
{
	m_ClickedMiddleButton = false;
	m_ClickedMiddleButtonPoint = CPoint (-1, -1);
}

void CViewportManager::RightButtonDown (CPoint Mouse)
{
	m_ClickedRightButton = true;
	m_ClickedRightButtonPoint = Mouse;
}

void CViewportManager::RightButtonUp (CPoint Mouse)
{
	m_ClickedRightButton = false;
	m_ClickedRightButtonPoint = CPoint (-1, -1);
}

void CViewportManager::MouseWheel (float Wheel)
{
	float fcurDistance = CCameraManager::Instance().GetCurrentCamera()->GetDistance();
	if (fcurDistance > 40000.0f)
	{
		Wheel *= 10.0f;
	}
	else if (fcurDistance > 10000.0f)
	{
		Wheel *= 5.0f;
	}
	else if (fcurDistance > 7000.0f)
	{
		Wheel *= 3.0f;
	}
	else if (fcurDistance > 3000.0f)
	{
		Wheel *= 2.0f;
	}

	float fDistance = CCameraManager::Instance().GetCurrentCamera()->GetDistance();
	fDistance = min (max (fDistance - Wheel * 2.0f, 200.0f), 80000.0f);
	CCameraManager::Instance().GetCurrentCamera()->SetDistance (fDistance);
}

void CViewportManager::TranslateStart (float x, float y)
{
	m_xOldPosition = x;
	m_yOldPosition = y;
}

void CViewportManager::MouseMove (CPoint Mouse)
{
	CCamera * pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();

	CMainFrame* mainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CWorldEditorDoc* doc = (CWorldEditorDoc*)mainFrame->GetActiveView()->GetDocument();

	if (m_ClickedLeftButton)
	{
		bool bLockRotation = false;

		// 맵의 속성이나 Light 방향 세팅중이라면 일단은 카메라 회전을 막되, 왼쪽 Control키를 누르고 있다면 예전처럼 작동하도록 한다.
		if (doc->IsEditingMapAttribute() || doc->IsEditingDirectionalLight())
		{
			bLockRotation = true;

			if (GetAsyncKeyState (VK_LCONTROL) & 0x8000)
			{
				bLockRotation = false;
			}
		}

		//		if (RotationMode == m_Mode)
		if (false == bLockRotation)
		{
			float fRoll, fPitch;
			fRoll =  - (m_ClickedLeftButtonPoint.x - Mouse.x) * 0.3f;
			fPitch = max (- pCurrentCamera->GetPitch() - 89.9f, min (89.9f - pCurrentCamera->GetPitch(), - (m_ClickedLeftButtonPoint.y - Mouse.y) * 0.3f));

			pCurrentCamera->RotateEyeAroundTarget (fPitch, fRoll);

			m_ClickedLeftButtonPoint = Mouse;
		}
	}
}

float CViewportManager::GetFrustumFarDistance()
{
	return m_fFrustumFar;
}

void CViewportManager::SetScreenStatus (int iWidth, int iHeight)
{
	m_iWidth = iWidth;
	m_iHeight = iHeight;
}

void CViewportManager::SetCursorPosition (int ix, int iy)
{
	m_ixCursor = ix;
	m_iyCursor = iy;
}

bool CViewportManager::isTranslationMode()
{
	return (TranslateMode == m_Mode && m_ClickedMiddleButton);
}

bool CViewportManager::CheckControlKey()
{
	return m_ControlKey;
}

bool CViewportManager::isLeftButton()
{
	return m_ClickedLeftButton;
}
bool CViewportManager::isMiddleButton()
{
	return m_ClickedMiddleButton;
}
bool CViewportManager::isRightButton()
{
	return m_ClickedRightButton;
}


void CViewportManager::MoveMousePosition (float x, float y, float& xMove, float& yMove)
{
	xMove = m_xOldPosition - x;
	yMove = m_yOldPosition - y;

	m_xOldPosition = x;
	m_yOldPosition = y;
}

void CViewportManager::Initialize()
{
	m_fFrustumNear = 100.0f;
	m_fFrustumFar = 120000.0f;

	m_ClickedLeftButtonPoint = CPoint (-1, -1);
	m_ClickedMiddleButtonPoint = CPoint (-1, -1);
	m_ClickedRightButtonPoint = CPoint (-1, -1);

	m_Mode = TranslateMode;
	m_ControlKey = false;
	m_ClickedLeftButton = false;
	m_ClickedMiddleButton = false;
	m_ClickedRightButton = false;

	m_xOldPosition = 0;
	m_yOldPosition = 0;
}

CViewportManager::CViewportManager()
{
	Initialize();
}

CViewportManager::~CViewportManager()
{
}


//////////////////////////////////////////////////////////////////////////
// CCAmeraManager 함수들...
//////////////////////////////////////////////////////////////////////////

void CCamera::ProcessTerrainCollision()
{
}

void CCamera::ProcessBuildingCollision()
{
}

void CCamera::Update()
{
}