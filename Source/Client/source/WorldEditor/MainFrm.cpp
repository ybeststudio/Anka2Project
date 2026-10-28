// MainFrm.cpp : implementation of the CMainFrame class
//

#include "StdAfx.h"
#include "WorldEditor.h"
#include "WorldEditorDoc.h"
#include "WorldEditorView.h"
#include "MainFrm.h"
#include "DataCtrl/MapAccessorOutdoor.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

int iStartUpPage = ID_VIEW_MAP;
//int iStartUpPage = ID_VIEW_OBJECT;
//int iStartUpPage = ID_VIEW_EFFECT;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE (CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP (CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND (ID_EDIT_UNDO, OnUndo)
	ON_COMMAND (ID_EDIT_REDO, OnRedo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT auiStatusIndicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_OVR,
	ID_INDICATOR_OVR,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	timeBeginPeriod (1);
	ilInit();
}

CMainFrame::~CMainFrame()
{
	ilShutDown();
	DestroyBackGroundData();
	timeEndPeriod (1);
}

int CMainFrame::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate (lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!CreateStatusBar())
	{
		TRACE0 ("Failed to create toolbar\n");
		return -1;
	}

	if (!CreateToolBar())
	{
		TRACE0 ("Failed to create toolbar\n");
		return -1;
	}

	if (!CreateDockingBar())
	{
		TRACE0 ("Failed to create docking bar.\n");
		return -1;
	}

	return 0;
}

void CMainFrame::PlayEffect()
{
	m_wndEffectCtrlBar.m_pdlgTimeBar->Play();
}

BOOL CMainFrame::PreCreateWindow (CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow (cs))
	{
		return FALSE;
	}
	/*
	cs.x = 0;
	cs.y = 0;
	cs.cx = 1032;
	cs.cy = 877;
	*/
	cs.x = 0;
	cs.y = 0;
	cs.cx = 1248;
	cs.cy = 877;
	// viewport 1024x768 가 되려면 위의 설정
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump (CDumpContext& dc) const
{
	CFrameWnd::Dump (dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame normal functions

BOOL CMainFrame::CreateStatusBar()
{
	if (!m_wndStatusBar.Create (this) ||
		!m_wndStatusBar.SetIndicators (auiStatusIndicators, sizeof (auiStatusIndicators) / sizeof (UINT)))
	{
		return FALSE;
	}

	UINT uiID;
	UINT uiStyle;
	int iWidth;

	m_wndStatusBar.GetPaneInfo (1, uiID, uiStyle, iWidth);
	m_wndStatusBar.SetPaneInfo (1, uiID, uiStyle, 150);

	m_wndStatusBar.GetPaneInfo (2, uiID, uiStyle, iWidth);
	m_wndStatusBar.SetPaneInfo (2, uiID, uiStyle, 100);
	return TRUE;
}

BOOL CMainFrame::CreateToolBar()
{
	if (!m_wndFileToolBar.CreateEx (this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
									| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC))
	{
		return FALSE;
	}
	if (!m_wndFileToolBar.LoadToolBar (IDR_MAINFRAME))
	{
		return FALSE;
	}

	m_wndFileToolBar.ShowButtonText (FALSE);
	m_wndFileToolBar.EnableDocking (CBRS_ALIGN_ANY);
	EnableDocking (CBRS_ALIGN_ANY);
	DockControlBar (&m_wndFileToolBar);

	return TRUE;
}

BOOL CMainFrame::CreateDockingBar()
{
	if (!m_wndMapCtrlBar.Create (this))
	{
		return FALSE;
	}
	if (!m_wndObjectCtrlBar.Create (this))
	{
		return FALSE;
	}
	if (!m_wndEffectCtrlBar.Create (this))
	{
		return FALSE;
	}
	if (!m_wndFlyCtrlBar.Create (this))
	{
		return FALSE;
	}

	m_wndMapCtrlBar.EnableDocking (CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	m_wndObjectCtrlBar.EnableDocking (CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	m_wndEffectCtrlBar.EnableDocking (CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	m_wndFlyCtrlBar.EnableDocking (CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);

	DockControlBar (&m_wndMapCtrlBar, AFX_IDW_DOCKBAR_LEFT);
	RecalcLayout();
	DockControlBar (&m_wndObjectCtrlBar, AFX_IDW_DOCKBAR_RIGHT);
	RecalcLayout();
	DockControlBar (&m_wndEffectCtrlBar, AFX_IDW_DOCKBAR_RIGHT);
	RecalcLayout();
	DockControlBar (&m_wndFlyCtrlBar, AFX_IDW_DOCKBAR_RIGHT);
	RecalcLayout();

	m_wndMapCtrlBar.SetFloatFixedWidth (TRUE);
	m_wndMapCtrlBar.SetMinSize (CSize (220, 10));
	m_wndMapCtrlBar.SetDockingFixedWidth (TRUE);
	ShowControlBar (&m_wndMapCtrlBar, TRUE, FALSE);

	m_wndObjectCtrlBar.SetFloatFixedWidth (TRUE);
	m_wndObjectCtrlBar.SetMinSize (CSize (220, 10));
	m_wndObjectCtrlBar.SetDockingFixedWidth (TRUE);
	ShowControlBar (&m_wndObjectCtrlBar, FALSE, FALSE);

	m_wndEffectCtrlBar.SetFloatFixedWidth (TRUE);
	m_wndEffectCtrlBar.SetMinSize (CSize (220, 50));
	m_wndEffectCtrlBar.SetDockingFixedWidth (TRUE);
	ShowControlBar (&m_wndEffectCtrlBar, FALSE, FALSE);

	m_wndFlyCtrlBar.SetFloatFixedWidth (TRUE);
	m_wndFlyCtrlBar.SetMinSize (CSize (220, 50));
	m_wndFlyCtrlBar.SetDockingFixedWidth (TRUE);
	ShowControlBar (&m_wndFlyCtrlBar, FALSE, FALSE);

	return TRUE;
}

void CMainFrame::Initialize()
{
	// NOTE : If you use next line, it can affect negative side to effect usage which is used on playing
	//        with animation. - [levites]
	//CResource::SetDeleteImmediately(true);

	// NOTE : This initialize proccessing's order is very important!
	//        If you change this order, program maybe not run correctly. - [ levites ]
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*) GetActiveDocument();

	CMapManagerAccessor * pMapManagerAccessor = pDocument->GetMapManagerAccessor();
	pMapManagerAccessor->Create();

	CSceneMap * pSceneMap = pDocument->GetSceneMap();
	pSceneMap->Initialize();
	pSceneMap->SetMapManagerAccessor (pMapManagerAccessor);

	CObjectData * pObjectData = pDocument->GetObjectData();
	pObjectData->Initialize();

	CSceneEffect * pSceneEffect = pDocument->GetSceneEffect();
	pSceneEffect->Initialize();

	CSceneObject * pSceneObject = pDocument->GetSceneObject();
	pSceneObject->Initialize();

	CSceneFly * pSceneFly = pDocument->GetSceneFly();
	pSceneFly->Initialize();

	m_wndObjectCtrlBar.Initialize();
	m_wndEffectCtrlBar.Initialize();
	m_wndMapCtrlBar.Initialize();
	m_wndFlyCtrlBar.Initialize();

	ShowControlBar (&m_wndMapCtrlBar, FALSE, FALSE);
	ShowControlBar (&m_wndObjectCtrlBar, FALSE, FALSE);
	ShowControlBar (&m_wndEffectCtrlBar, FALSE, FALSE);
	ShowControlBar (&m_wndFlyCtrlBar, FALSE, FALSE);

	switch (iStartUpPage)
	{
		case ID_VIEW_MAP:
			pDocument->SetActiveMode (ID_VIEW_MAP);
			ShowControlBar (&m_wndMapCtrlBar, TRUE, FALSE);
			break;

		case ID_VIEW_OBJECT:
			pDocument->SetActiveMode (ID_VIEW_OBJECT);
			ShowControlBar (&m_wndObjectCtrlBar, TRUE, FALSE);
			break;

		case ID_VIEW_EFFECT:
			pDocument->SetActiveMode (ID_VIEW_EFFECT);
			ShowControlBar (&m_wndEffectCtrlBar, TRUE, FALSE);
			break;

		case ID_VIEW_FLY:
			pDocument->SetActiveMode (ID_VIEW_FLY);
			ShowControlBar (&m_wndFlyCtrlBar, TRUE, FALSE);
			break;
	}

	CWorldEditorView * pView = (CWorldEditorView*) GetActiveView();
	pView->Initialize();

	pSceneObject->FitCamera();
}

void CMainFrame::UpdateStatusBar (float fx, float fy, float fz)
{
	CStatusBar * pStatusBar = &m_wndStatusBar;

	CString strPixelPosition;
	CString strAreaPosition;

	strPixelPosition.Format ("%.2f / %.2f / %.2f", fx / 100.0f, fy / 100.0f, fz / 100.0f);

	CWorldEditorDoc * pDocument = (CWorldEditorDoc*) GetActiveDocument();
	CMapManagerAccessor * pMapManagerAccessor = pDocument->GetMapManagerAccessor();

	if (pMapManagerAccessor && pMapManagerAccessor->IsMapOutdoor())
	{
		CMapOutdoor & rMapOutdoor = pMapManagerAccessor->GetMapOutdoorRef();
		const TOutdoorMapCoordinate c_OutdoorMapCoordinate = rMapOutdoor.GetCurCoordinate();
		strAreaPosition.Format ("%u / %u", c_OutdoorMapCoordinate.m_sTerrainCoordX, c_OutdoorMapCoordinate.m_sTerrainCoordY);
	}

	pStatusBar->SetPaneText (1, strPixelPosition, TRUE);
	pStatusBar->SetPaneText (2, strAreaPosition, TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

BOOL CMainFrame::OnCmdMsg (UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (pHandlerInfo == NULL)
	{
		CCmdUI *pCmdUI = (CCmdUI*)pExtra;

		switch (nCode)
		{
			case CN_COMMAND:
				if (CN_COMMAND_HANDLER (nID, pCmdUI) == TRUE)
				{
					return TRUE;
				}
				break;
			case CN_UPDATE_COMMAND_UI:
				if (CN_UPDATE_HANDLER (nID, pCmdUI) == TRUE)
				{
					return TRUE;
				}
				break;
		}
	}

	return CFrameWnd::OnCmdMsg (nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::UpdateObjectControlBar()
{
	m_wndObjectCtrlBar.UpdatePage();
}

void CMainFrame::UpdateAnimationPage()
{
	m_wndObjectCtrlBar.UpdateAnimationPage();
}

void CMainFrame::UpdateEffectControlBar()
{
	m_wndEffectCtrlBar.UpdatePage();
}

void CMainFrame::UpdateFlyControlBar()
{
	m_wndFlyCtrlBar.UpdatePage();
}

void CMainFrame::RunLoadMapEvent()
{
	m_wndMapCtrlBar.RunLoadMapEvent();
}

void CMainFrame::UpdateMapControlBar()
{
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)GetActiveDocument();

	if (!pDocument)
	{
		return;
	}

	int iMode = m_wndMapCtrlBar.UpdatePage();
	pDocument->m_SceneMap.SetEditingMode (iMode);
}

void CMainFrame::OnUndo()
{
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)GetActiveDocument();
	CUndoBuffer * pUndoBuffer = pDocument->GetUndoBuffer();

	pUndoBuffer->Undo();
}

void CMainFrame::OnRedo()
{
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)GetActiveDocument();
	CUndoBuffer * pUndoBuffer = pDocument->GetUndoBuffer();

	pUndoBuffer->Redo();
}

BOOL CMainFrame::CN_COMMAND_HANDLER (UINT nID, CCmdUI *pCmdUI)
{
	if (m_wndFileToolBar.CN_COMMAND_HANDLER (nID, pCmdUI))
	{
		return TRUE;
	}

	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)GetActiveDocument();
	CWorldEditorView * pView = (CWorldEditorView*)GetActiveView();
	pDocument->SetActiveMode (nID);

	switch (nID)
	{
		case ID_VIEW_MAP:
			ShowControlBar (&m_wndMapCtrlBar, !m_wndMapCtrlBar.IsWindowVisible(), FALSE);
			ShowControlBar (&m_wndObjectCtrlBar, FALSE, FALSE);
			ShowControlBar (&m_wndEffectCtrlBar, FALSE, FALSE);
			ShowControlBar (&m_wndFlyCtrlBar, FALSE, FALSE);
			return TRUE;
			break;

		case ID_VIEW_OBJECT:
			ShowControlBar (&m_wndObjectCtrlBar, !m_wndObjectCtrlBar.IsWindowVisible(), FALSE);
			ShowControlBar (&m_wndMapCtrlBar, FALSE, FALSE);
			ShowControlBar (&m_wndEffectCtrlBar, FALSE, FALSE);
			ShowControlBar (&m_wndFlyCtrlBar, FALSE, FALSE);
			return TRUE;
			break;

		case ID_VIEW_EFFECT:
			ShowControlBar (&m_wndEffectCtrlBar, !m_wndEffectCtrlBar.IsWindowVisible(), FALSE);
			ShowControlBar (&m_wndMapCtrlBar, FALSE, FALSE);
			ShowControlBar (&m_wndObjectCtrlBar, FALSE, FALSE);
			ShowControlBar (&m_wndFlyCtrlBar, FALSE, FALSE);
			return TRUE;
			break;

		case ID_VIEW_FLY:
			ShowControlBar (&m_wndFlyCtrlBar, !m_wndFlyCtrlBar.IsWindowVisible(), FALSE);
			ShowControlBar (&m_wndEffectCtrlBar, FALSE, FALSE);
			ShowControlBar (&m_wndMapCtrlBar, FALSE, FALSE);
			ShowControlBar (&m_wndObjectCtrlBar, FALSE, FALSE);
			return TRUE;
			break;

		case ID_VIEW_INFO_BOX:
		{
			CWorldEditorView * pView = (CWorldEditorView*)GetActiveView();
			pView->ToggleInfoBoxVisibleState();
		}
		return TRUE;
		break;

		case ID_VIEW_RENDERING_MODE_SOLID:
			CSceneBase::SetRenderingMode (CSceneBase::RENDERING_MODE_SOLID);
			return TRUE;
			break;

		case ID_VIEW_RENDERING_MODE_WIRE_FRAME:
			CSceneBase::SetRenderingMode (CSceneBase::RENDERING_MODE_WIRE_FRAME);
			return TRUE;
			break;

		case ID_VIEW_DLG_PERSPECTIVE:
		{
			CWorldEditorView * pView = (CWorldEditorView*)GetActiveView();
			pView->TogglePerspectiveDialog();
		}
		return TRUE;
		break;
	}

	return FALSE;
}

BOOL CMainFrame::CN_UPDATE_HANDLER (UINT nID, CCmdUI *pCmdUI)
{
	if (m_wndFileToolBar.CN_UPDATE_HANDLER (nID, pCmdUI))
	{
		return TRUE;
	}

	switch (nID)
	{
		case ID_VIEW_MAP:
			pCmdUI->Enable();
			pCmdUI->SetCheck (m_wndMapCtrlBar.IsWindowVisible());
			return TRUE;
			break;

		case ID_VIEW_OBJECT:
			pCmdUI->Enable();
			pCmdUI->SetCheck (m_wndObjectCtrlBar.IsWindowVisible());
			return TRUE;
			break;

		case ID_VIEW_EFFECT:
			pCmdUI->Enable();
			pCmdUI->SetCheck (m_wndEffectCtrlBar.IsWindowVisible());
			return TRUE;
			break;

		case ID_VIEW_FLY:
			pCmdUI->Enable();
			pCmdUI->SetCheck (m_wndFlyCtrlBar.IsWindowVisible());
			return TRUE;
			break;

		case ID_VIEW_INFO_BOX:
		{
			pCmdUI->Enable();
			CWorldEditorView * pView = (CWorldEditorView*)GetActiveView();
			if (pView->IsInfoBoxVisible())
			{
				pCmdUI->SetCheck (TRUE);
			}
			else
			{
				pCmdUI->SetCheck (FALSE);
			}
			return TRUE;
			break;
		}

		case ID_VIEW_RENDERING_MODE_SOLID:
			pCmdUI->Enable();
			if (CSceneBase::RENDERING_MODE_SOLID == CSceneBase::GetRenderingMode())
			{
				pCmdUI->SetCheck (TRUE);
			}
			else
			{
				pCmdUI->SetCheck (FALSE);
			}
			return TRUE;
			break;

		case ID_VIEW_RENDERING_MODE_WIRE_FRAME:
			pCmdUI->Enable();
			if (CSceneBase::RENDERING_MODE_WIRE_FRAME == CSceneBase::GetRenderingMode())
			{
				pCmdUI->SetCheck (TRUE);
			}
			else
			{
				pCmdUI->SetCheck (FALSE);
			}
			return TRUE;
			break;

		case ID_VIEW_DLG_PERSPECTIVE:
		{
			CWorldEditorView * pView = (CWorldEditorView*)GetActiveView();
			pCmdUI->Enable();
			if (pView->isShowingPerspectiveDialog())
			{
				pCmdUI->SetCheck (TRUE);
			}
			else
			{
				pCmdUI->SetCheck (FALSE);
			}
		}
		return TRUE;
		break;
	}

	return FALSE;
}
