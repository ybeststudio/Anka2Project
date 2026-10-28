#include "StdAfx.h"
#include "../MainFrm.h"
#include "../../EterLib/Camera.h"

D3DXCOLOR CSceneBase::ms_GridLarge;
D3DXCOLOR CSceneBase::ms_GridSmall;

D3DXVECTOR3 CSceneBase::ms_vecMousePosition;

CSceneBase::ERenderingMode CSceneBase::ms_RenderingMode;

CCamera* CSceneBase::ms_Camera = NULL;

CGraphicTextInstance CSceneBase::ms_TextInstanceFaceCount;
CGraphicTextInstance CSceneBase::ms_TextInstanceCameraDistance;
CGraphicTextInstance CSceneBase::ms_TextInstancePureRenderingTime;

int CSceneBase::ms_iPureRenderingTime;

void CSceneBase::Update()
{
	OnUpdate();
}

void CSceneBase::Render (BOOL bClear)
{
	int iStartTime = timeGetTime();

	if (D3DFILL_WIREFRAME == ms_RenderingMode)
	{
		STATEMANAGER.SaveRenderState (D3DRS_FILLMODE, ms_RenderingMode);
	}

	OnRender (bClear);

	if (D3DFILL_WIREFRAME == ms_RenderingMode)
	{
		STATEMANAGER.RestoreRenderState (D3DRS_FILLMODE);
	}

	ms_iPureRenderingTime = timeGetTime() - iStartTime;
}

void CSceneBase::RenderUI()
{
	char szMsg[128 + 1];

	CScreen::SetDiffuseColor (0.0f, 0.0f, 0.0f, 0.3f);
	CScreen::RenderBar2d (10.0f, 10.0f, 10.0f+300.0f, 10.0f+80.0f);

	_snprintf (szMsg, 128, "FaceCount : %d", CScreen::GetFaceCount());
	ms_TextInstanceFaceCount.SetValue (szMsg, strlen (szMsg));
	ms_TextInstanceFaceCount.Update();
	ms_TextInstanceFaceCount.Render();

	D3DXVECTOR3 v3Target, v3Eye;
	v3Target = CCameraManager::Instance().GetCurrentCamera()->GetTarget();
	v3Eye = CCameraManager::Instance().GetCurrentCamera()->GetEye();

	D3DXVECTOR3 diff = v3Target - v3Eye;
	float fLenView = D3DXVec3Length (&diff) / 100.0f;

	_snprintf (szMsg, 128, "CameraDistance : %f meter", fLenView);
	ms_TextInstanceCameraDistance.SetValue (szMsg, strlen (szMsg));
	ms_TextInstanceCameraDistance.Update();
	ms_TextInstanceCameraDistance.Render();

	_snprintf (szMsg, 128, "Pure Rendering Time : %d", ms_iPureRenderingTime);
	ms_TextInstancePureRenderingTime.SetValue (szMsg, strlen (szMsg));
	ms_TextInstancePureRenderingTime.Update();
	ms_TextInstancePureRenderingTime.Render();

	OnRenderUI (10.0f, 100.0f);
}

void CSceneBase::RenderGrid()
{
	STATEMANAGER.SetTransform (D3DTS_WORLD, &GetIdentityMatrix());

	SetDiffuseColor (ms_GridSmall.r, ms_GridSmall.g, ms_GridSmall.b);
	for (float x = -600.0f; x <= 600.0f; x += 30.0f)
	{
		RenderLine2d (x, 600.0f, x, -600.0f);
	}
	for (float y = -600.0f; y <= 600.0f; y += 30.0f)
	{
		RenderLine2d (-600.0f, y, 600.0f, y);
	}

	SetDiffuseColor (ms_GridLarge.r, ms_GridLarge.g, ms_GridLarge.b);
	RenderLine2d (-600.0f, 0.0f, 600.0f, 0.0f);
	RenderLine2d (0.0f, 600.0f, 0.0f, -600.0f);
}

void CSceneBase::KeyDown (int iChar)
{
	OnKeyDown (iChar);
}

void CSceneBase::KeyUp (int iChar)
{
	OnKeyUp (iChar);
}

void CSceneBase::MouseMove (LONG x, LONG y)
{
	OnMouseMove (x, y);
}

void CSceneBase::LButtonDown (UINT nFlags, CPoint point)
{
	OnLButtonDown (nFlags, point);
}

void CSceneBase::LButtonUp()
{
	OnLButtonUp();
}

void CSceneBase::RButtonDown()
{
	OnRButtonDown();
}

void CSceneBase::RButtonUp()
{
	OnRButtonUp();
}

BOOL CSceneBase::MouseWheel (short zDelta)
{
	return OnMouseWheel (zDelta);
}

void CSceneBase::MovePosition (float fx, float fy)
{
	OnMovePosition (fx, fy);
	UpdateStatusBar();
}

void CSceneBase::UpdateStatusBar()
{
	D3DXVECTOR3 pv3Position = ms_Camera->GetTarget();
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	pFrame->UpdateStatusBar (pv3Position.x, -pv3Position.y, pv3Position.z - 100.0f - 65536.0f * 0.25f);
}

int CSceneBase::GetRenderingMode()
{
	return ms_RenderingMode;
}

void CSceneBase::SetRenderingMode (ERenderingMode RenderingMode)
{
	ms_RenderingMode = RenderingMode;
}

void CSceneBase::CreateUI()
{
	CResource * pResource = CResourceManager::Instance().GetTypeResourcePointer ("±¼¸²Ã¼:15.fnt");
	CGraphicText * pText = static_cast<CGraphicText*> (pResource);

	ms_TextInstanceFaceCount.SetTextPointer (pText);
	ms_TextInstanceFaceCount.SetColor (0.8f, 0.8f, 0.8f);
	ms_TextInstanceFaceCount.SetPosition (20.0f, 20.0f);

	ms_TextInstanceCameraDistance.SetTextPointer (pText);
	ms_TextInstanceCameraDistance.SetColor (0.8f, 0.8f, 0.8f);
	ms_TextInstanceCameraDistance.SetPosition (20.0f, 40.0f);

	ms_TextInstancePureRenderingTime.SetTextPointer (pText);
	ms_TextInstancePureRenderingTime.SetColor (0.8f, 0.8f, 0.8f);
	ms_TextInstancePureRenderingTime.SetPosition (20.0f, 60.0f);
}

CSceneBase::CSceneBase()
{
	ms_GridLarge = D3DXCOLOR (0.0f, 0.0f, 0.0f, 1.0f);
	ms_GridSmall = D3DXCOLOR (0.2929f, 0.2929f, 0.2929f, 0.5f);

	ms_vecMousePosition = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);

	ms_RenderingMode = RENDERING_MODE_SOLID;

	ms_Camera = CCameraManager::Instance().GetCurrentCamera();
}

CSceneBase::~CSceneBase()
{
}
