// WorldEditorView.cpp : implementation of the CWorldEditorView class
//

#include "StdAfx.h"
#include "WorldEditor.h"
#include "MainFrm.h"
#include "WorldEditorDoc.h"
#include "WorldEditorView.h"
#include "Dialog/DlgGoto.h"
#include "DataCtrl/ShadowRenderHelper.h"
#include "../EterLib/Camera.h"
#include "RenderEngine.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWorldEditorView

IMPLEMENT_DYNCREATE (CWorldEditorView, CView)

BEGIN_MESSAGE_MAP (CWorldEditorView, CView)
	//{{AFX_MSG_MAP(CWorldEditorView)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_CREATE()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_SIZE()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND (ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND (ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND (ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

#define EDITOR_LOCAL_CAMERA		10
/////////////////////////////////////////////////////////////////////////////
// CWorldEditorView construction/destruction

CWorldEditorView::CWorldEditorView() : m_bInitialized (false), m_bRenderUI (false)
{
	CCameraManager::Instance().AddCamera (EDITOR_LOCAL_CAMERA);
	m_pDlgPerspective = NULL;

	Unlock();
}

CWorldEditorView::~CWorldEditorView()
{
	if (m_pDlgPerspective)
	{
		delete m_pDlgPerspective;
		m_pDlgPerspective = NULL;
	}
}

int CWorldEditorView::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate (lpCreateStruct) == -1)
	{
		return -1;
	}

	m_pDlgPerspective = new CDlgPerspective;
	m_pDlgPerspective->Create (IDD_DIALOG_PERSPECTIVE, this);
	m_pDlgPerspective->SetWindowPos (NULL, 0, 0, 0, 0, TRUE);
	m_pDlgPerspective->SetFOV (40.0f);
	m_pDlgPerspective->SetNearDistance (100.0f);
	m_pDlgPerspective->SetFarDistance (30000.0f);
	return 0;
}

BOOL CWorldEditorView::PreCreateWindow (CREATESTRUCT& cs)
{
	return CView::PreCreateWindow (cs);
}

void CWorldEditorView::Initialize()
{
	CResource * pResource = CResourceManager::Instance().GetTypeResourcePointer ("굴림체:15.fnt");
	CGraphicText * pText = static_cast<CGraphicText*> (pResource);

	CSceneBase::CreateUI();

	m_Screen.SetPositionCamera (0.0f, 0.0f, 0.0f, 5000.0f, 30.0f, 0.0f);
	D3DXVECTOR3 v3Target = CCameraManager::Instance().GetCurrentCamera()->GetTarget();
	v3Target.z += 100.0f;
	CCameraManager::Instance().GetCurrentCamera()->SetTarget (v3Target);

	UpdateFrustum();

	m_bInitialized = true;

	CWorldEditorDoc * pDoc = GetDocument();
	CSceneMap * pSceneMap = pDoc->GetSceneMap();
	pSceneMap->CreateEnvironment();
}

void CWorldEditorView::Lock()
{
	m_isLocked = true;
}

void CWorldEditorView::Unlock()
{
	m_isLocked = false;
}

bool CWorldEditorView::IsLocked()
{
	return m_isLocked;
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditorView drawing

void CWorldEditorView::OnDraw (CDC* pDC)
{
	CWorldEditorDoc* pDoc = GetDocument();
	ASSERT_VALID (pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditorView printing

BOOL CWorldEditorView::OnPreparePrinting (CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting (pInfo);
}

void CWorldEditorView::OnBeginPrinting (CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CWorldEditorView::OnEndPrinting (CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditorView diagnostics

#ifdef _DEBUG
void CWorldEditorView::AssertValid()const
{
	CView::AssertValid();
}

void CWorldEditorView::Dump (CDumpContext& dc)const
{
	CView::Dump (dc);
}

CWorldEditorDoc* CWorldEditorView::GetDocument()// non-debug version is inline
{
	ASSERT (m_pDocument->IsKindOf (RUNTIME_CLASS (CWorldEditorDoc)));
	return (CWorldEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWorldEditorView normal functions

void CWorldEditorView::TogglePerspectiveDialog()
{
	m_pDlgPerspective->ShowWindow (!m_pDlgPerspective->IsWindowVisible());
}
BOOL CWorldEditorView::isShowingPerspectiveDialog()
{
	return m_pDlgPerspective->IsWindowVisible();
}

void CWorldEditorView::GetMouseLocalPosition (POINT point, float* px, float* py)
{
	CCamera * pOriginalCamera = CCameraManager::Instance().GetCurrentCamera();
	CCameraManager::Instance().SetCurrentCamera (EDITOR_LOCAL_CAMERA);

	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity (&matIdentity);
	STATEMANAGER.SaveTransform (D3DTS_PROJECTION, &matIdentity);
	STATEMANAGER.SaveTransform (D3DTS_VIEW, &matIdentity);

	CRect Rect;
	GetWindowRect (&Rect);
	m_Screen.SetPerspective (m_pDlgPerspective->GetFOV(), float (Rect.Width()) / float (Rect.Height()), 100.0f, m_ViewportManager.GetFrustumFarDistance());
	m_Screen.SetAroundCamera (pOriginalCamera->GetDistance(), pOriginalCamera->GetPitch(), pOriginalCamera->GetRoll(), 100.0f);

	m_Screen.SetCursorPosition (point.x, point.y, Rect.Width(), Rect.Height());
	m_Screen.GetCursorXYPosition (px, py);

	STATEMANAGER.RestoreTransform (D3DTS_VIEW);
	STATEMANAGER.RestoreTransform (D3DTS_PROJECTION);

	CCameraManager::Instance().ResetToPreviousCamera();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Camera Start
void CWorldEditorView::UpdateCursorPosition (int ix, int iy)
{
	CRect Rect;
	GetWindowRect (&Rect);
	m_Screen.SetCursorPosition (ix, iy, Rect.Width(), Rect.Height());
}

void CWorldEditorView::UpdateFrustum()
{
	m_pDlgPerspective->SetNearDistance (m_ViewportManager.GetFrustumNearDistance());
	m_pDlgPerspective->SetFarDistance (m_ViewportManager.GetFrustumFarDistance());

	CRect Rect;
	GetWindowRect (&Rect);

	float fFOV = m_pDlgPerspective->GetFOV();
	float fAspect = float (Rect.Width()) / float (Rect.Height());
	float fNearDistance = m_ViewportManager.GetFrustumNearDistance();
	float fFarDistance = m_ViewportManager.GetFrustumFarDistance();

	m_Screen.SetPerspective (fFOV, fAspect, fNearDistance, fFarDistance);
	m_Screen.UpdateViewMatrix();
}

void CWorldEditorView::UpdateTargetPosition (float fx, float fy)
{
	CWorldEditorDoc* pDoc = GetDocument();
	CSceneBase * pScene = pDoc->GetActiveScene();
	pScene->MovePosition (fx, fy);
	UpdateFrustum();
}
// Camera End
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// ETC
void CWorldEditorView::ToggleInfoBoxVisibleState()
{
	m_bRenderUI = m_bRenderUI ? false : true;
}
BOOL CWorldEditorView::IsInfoBoxVisible()
{
	return m_bRenderUI;
}
// ETC
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////
// Input Process Start
/*
	CWorldEditorDoc* pDoc = GetDocument();
 	CWorldEditorDoc::TSceneIterator itor;
	if (pDoc->CreateSceneIterator(itor))
		do
		{
			(*itor)->KeyUp(iChar);
		}
		while(pDoc->NextSceneIterator(itor));
*/
void CWorldEditorView::RunKeyDown (int iChar)
{
	CWorldEditorDoc * pDoc = GetDocument();
	CSceneBase * pScene = pDoc->GetActiveScene();
	pScene->KeyDown (iChar);
}
void CWorldEditorView::RunKeyUp (int iChar)
{
	CWorldEditorDoc * pDoc = GetDocument();
	CSceneBase * pScene = pDoc->GetActiveScene();
	pScene->KeyUp (iChar);
}
void CWorldEditorView::RunMouseMove (LONG x, LONG y)
{
	UpdateFrustum();

	CWorldEditorDoc * pDoc = GetDocument();
	CSceneBase * pScene = pDoc->GetActiveScene();
	pScene->MouseMove (x, y);
}
void CWorldEditorView::RunLButtonDown (UINT nFlags, CPoint point)
{
	UpdateFrustum();

	CWorldEditorDoc * pDoc = GetDocument();
	CSceneBase * pScene = pDoc->GetActiveScene();
	pScene->LButtonDown (nFlags, point);
}
void CWorldEditorView::RunLButtonUp()
{
	UpdateFrustum();

	CWorldEditorDoc * pDoc = GetDocument();
	CSceneBase * pScene = pDoc->GetActiveScene();
	pScene->LButtonUp();
}
void CWorldEditorView::RunRButtonDown()
{
	UpdateFrustum();

	CWorldEditorDoc * pDoc = GetDocument();
	CSceneBase * pScene = pDoc->GetActiveScene();
	pScene->RButtonDown();
}
void CWorldEditorView::RunRButtonUp()
{
	UpdateFrustum();

	CWorldEditorDoc * pDoc = GetDocument();
	CSceneBase * pScene = pDoc->GetActiveScene();
	pScene->RButtonUp();
}
BOOL CWorldEditorView::RunMouseWheel (short zDelta)
{
	UpdateFrustum();

	CWorldEditorDoc * pDoc = GetDocument();
	CSceneBase * pScene = pDoc->GetActiveScene();
	return pScene->MouseWheel (zDelta);
}
// Input Process End
/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CWorldEditorView message handlers

void CWorldEditorView::OnSize (UINT nType, int cx, int cy)
{
	if (!m_bInitialized)
	{
		CView::OnSize (nType, cx, cy);
		return;
	}

	if (SIZE_MAXIMIZED == nType || SIZE_RESTORED == nType)
	{
		m_Screen.SetViewport (0, 0, cx, cy, 0.0f, 1.0f);
		//////////////////////////////////////////////////////////////////////////
		CWorldEditorApp * pApplication = (CWorldEditorApp*) AfxGetApp();
		CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
		CGraphicDevice & rDevice = pApplication->GetGraphicDevice();

		pMapManagerAccessor->DestroyShadowTexture();
		rDevice.ResizeBackBuffer (cx, cy);
		pMapManagerAccessor->RecreateShadowTexture();
		RenderCore::CRenderEngine::Instance().Resize(cx, cy);
		//////////////////////////////////////////////////////////////////////////
	}

	UpdateFrustum();
	CView::OnSize (nType, cx, cy);
}

// Keyboard
void CWorldEditorView::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case 0x47: // G
		{
			CDlgGoto DlgGoto;
			DlgGoto.DoModal();
		}
		break;

		case VK_CONTROL:
			m_ViewportManager.ControlKeyDown();
			m_isControlKey = TRUE;
			break;

		case VK_F11: // F12로 하면 이상한 에러가 나더군요. 그래서 F11로 수정 - 동현
		{
			m_bRenderUI = !m_bRenderUI;
		}
		break;

		case VK_F4:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*) AfxGetApp();
			CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
			pMapManagerAccessor->ToggleWireframe();
		}
		break;

		case 0xDB: // '['
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*) AfxGetApp();
			CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

			if (pMapManagerAccessor->GetBrushSize() != 0)
			{
				pMapManagerAccessor->SetBrushSize (pMapManagerAccessor->GetBrushSize() - 1);
			}

			CMainFrame * pFrame = (CMainFrame*) AfxGetMainWnd();
			pFrame->UpdateMapControlBar();
		}
		break;

		case 0xDD: // ']'
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*) AfxGetApp();
			CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
			pMapManagerAccessor->SetBrushSize (pMapManagerAccessor->GetBrushSize() + 1);

			CMainFrame * pFrame = (CMainFrame*) AfxGetMainWnd();
			pFrame->UpdateMapControlBar();
		}
		break;
		case 0xBA: // ';'
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*) AfxGetApp();
			CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

			if (pMapManagerAccessor->GetBrushSizeY() != 0)
			{
				pMapManagerAccessor->SetBrushSizeY (pMapManagerAccessor->GetBrushSizeY() - 1);
			}

			CMainFrame * pFrame = (CMainFrame*) AfxGetMainWnd();
			pFrame->UpdateMapControlBar();
		}
		break;

		case 0xDE: // '''
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*) AfxGetApp();
			CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
			pMapManagerAccessor->SetBrushSizeY (pMapManagerAccessor->GetBrushSizeY() + 1);

			CMainFrame * pFrame = (CMainFrame*) AfxGetMainWnd();
			pFrame->UpdateMapControlBar();
		}
		break;
	}

	RunKeyDown (nChar);
	CView::OnKeyDown (nChar, nRepCnt, nFlags);
	Invalidate (false);
}
void CWorldEditorView::OnKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case VK_CONTROL:
			m_ViewportManager.ControlKeyUp();
			m_isControlKey = FALSE;
			break;
	}

	RunKeyUp (nChar);
	CView::OnKeyUp (nChar, nRepCnt, nFlags);
	Invalidate (false);
}

void CWorldEditorView::OnSysKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (18 == nChar)	// ALT 키
	{
		//m_ViewportManager.ModeButtonDown();
	}

	CView::OnSysKeyDown (nChar, nRepCnt, nFlags);
}

void CWorldEditorView::OnSysKeyUp (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (18 == nChar)
	{
		m_ViewportManager.ModeButtonUp();
	}

	CView::OnSysKeyUp (nChar, nRepCnt, nFlags);
}

// Movement
void CWorldEditorView::OnMouseMove (UINT nFlags, CPoint point)
{
	m_ViewportManager.MouseMove (point);

	RunMouseMove (point.x, point.y);
	UpdateCursorPosition (point.x, point.y);

	if (m_ViewportManager.isTranslationMode())
	{
		float fx, fy;
		GetMouseLocalPosition (point, &fx, &fy);

		float xMove, yMove;
		m_ViewportManager.MoveMousePosition (fx, fy, xMove, yMove);

		if (GetAsyncKeyState (VK_LSHIFT) & 0x8000)
		{
			xMove *= 5.0f;
			yMove *= 5.0f;
		}

		xMove = fMAX (xMove, -5000.0f);
		yMove = fMAX (yMove, -5000.0f);
		xMove = fMIN (xMove, +5000.0f);
		yMove = fMIN (yMove, +5000.0f);

		UpdateTargetPosition (xMove, yMove);
	}
	CView::OnMouseMove (nFlags, point);
	Invalidate (false);
}

// Left Button
void CWorldEditorView::OnLButtonDown (UINT nFlags, CPoint point)
{
	RunLButtonDown (nFlags, point);
	m_ViewportManager.LeftButtonDown (point);
	CView::OnLButtonDown (nFlags, point);
	SetCapture();
	Invalidate (false);
}
void CWorldEditorView::OnLButtonUp (UINT nFlags, CPoint point)
{
	RunLButtonUp();
	m_ViewportManager.LeftButtonUp (point);
	CView::OnLButtonUp (nFlags, point);
	ReleaseCapture();
	Invalidate (false);
}

// Middle Button
void CWorldEditorView::OnMButtonDown (UINT nFlags, CPoint point)
{
	m_ViewportManager.MiddleButtonDown (point);

	if (m_ViewportManager.isTranslationMode())
	{
		float fx, fy;
		GetMouseLocalPosition (point, &fx, &fy);

		m_ViewportManager.TranslateStart (fx, fy);
	}

	CView::OnMButtonDown (nFlags, point);
	SetCapture();
	Invalidate (false);
}
void CWorldEditorView::OnMButtonUp (UINT nFlags, CPoint point)
{
	m_ViewportManager.MiddleButtonUp (point);
	CView::OnMButtonUp (nFlags, point);
	ReleaseCapture();
	Invalidate (false);
}
BOOL CWorldEditorView::OnMouseWheel (UINT nFlags, short zDelta, CPoint pt)
{
	if (!RunMouseWheel (zDelta))
	{
		m_ViewportManager.MouseWheel (float (zDelta));
	}

	Invalidate (false);
	return CView::OnMouseWheel (nFlags, zDelta, pt);
}

// Right Button
void CWorldEditorView::OnRButtonDown (UINT nFlags, CPoint point)
{
	RunRButtonDown();
	m_ViewportManager.RightButtonDown (point);
	CView::OnRButtonDown (nFlags, point);
	SetCapture();
	Invalidate (false);
}
void CWorldEditorView::OnRButtonUp (UINT nFlags, CPoint point)
{
	RunRButtonUp();
	m_ViewportManager.RightButtonUp (point);
	CView::OnRButtonUp (nFlags, point);
	ReleaseCapture();
	Invalidate (false);

}

BOOL CWorldEditorView::OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message)
{
	//	if (GetFocus() != pWnd)
	//		pWnd->SetFocus();

	return CView::OnSetCursor (pWnd, nHitTest, message);
}

void CWorldEditorView::Process()
{
	if (IsLocked())
	{
		return;
	}

	CTimer::Instance().Advance();
	//////////////////////////////////////////////////////////////////////////////

	CWorldEditorDoc * pDoc = GetDocument();
	ASSERT_VALID (pDoc);

	CResourceManager::Instance().Update();

	UpdateFrustum();

	CSceneBase * pScene = pDoc->GetActiveScene();
	pScene->Update();

	m_Screen.Begin();

	if (RenderCore::CRenderEngine::Instance().IsInitialized())
	{
		CCamera* pCamera = CCameraManager::Instance().GetCurrentCamera();
		if (pCamera)
		{
			const D3DXVECTOR3& eye = pCamera->GetEye();
			const D3DXVECTOR3& target = pCamera->GetTarget();
			const D3DXVECTOR3& up = pCamera->GetUp();
			CRect Rect;
			GetClientRect(&Rect);
			float fAspect = (Rect.Height() > 0) ? (float)Rect.Width() / (float)Rect.Height() : 1.3333f;
			RenderCore::CRenderEngine::Instance().SetCamera(
				DirectX::XMFLOAT3(eye.x, eye.y, eye.z),
				DirectX::XMFLOAT3(target.x, target.y, target.z),
				DirectX::XMFLOAT3(up.x, up.y, up.z),
				DirectX::XMConvertToRadians(30.0f),
				fAspect,
				10.0f
			);
		}
		RenderCore::CRenderEngine::Instance().BeginFrame(0.016f, 12.0f);
		RenderCore::CRenderEngine::Instance().RenderFrame();
		RenderCore::CRenderEngine::Instance().EndFrame();
	}

	pScene->Render (true);

	if (m_bRenderUI)
	{
		///////////////////////////////////////////////////////////////////////////////////////////////
		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity (&matIdentity);
		STATEMANAGER.SaveTransform (D3DTS_PROJECTION, &matIdentity);
		STATEMANAGER.SaveTransform (D3DTS_VIEW, &matIdentity);
		STATEMANAGER.SetTransform (D3DTS_WORLD, &matIdentity);
		STATEMANAGER.SaveRenderState (D3DRS_ALPHABLENDENABLE, TRUE);
		STATEMANAGER.SetRenderState (D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		STATEMANAGER.SetRenderState (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		CRect Rect;
		GetClientRect (&Rect);
		m_Screen.SetOrtho2D (Rect.Width(), Rect.Height(), 400.0f);
		m_Screen.SetDiffuseColor (1.0f, 1.0f, 1.0f, 1.0f);

		pScene->RenderUI();

		STATEMANAGER.RestoreRenderState (D3DRS_ALPHABLENDENABLE);
		STATEMANAGER.RestoreTransform (D3DTS_VIEW);
		STATEMANAGER.RestoreTransform (D3DTS_PROJECTION);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	m_Screen.End();
	m_Screen.Show (NULL, GetSafeHwnd());

	m_Screen.ResetFaceCount();

	//////////////////////////////////////////////////////////////////////////////
	static int lastTime = timeGetTime();
	long lElapsedTime = timeGetTime() - lastTime;
	if (lElapsedTime < 16)
	{
		Sleep (16 - lElapsedTime);
	}
	lastTime = timeGetTime();
}
