// ObjectCtrlBar.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectCtrlBar.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


const int c_iTemporaryWidth = 0;
const int c_iTemporaryHeight = 27;


/////////////////////////////////////////////////////////////////////////////
// CObjectCtrlBar

CObjectCtrlBar::CObjectCtrlBar()
{
}

CObjectCtrlBar::~CObjectCtrlBar()
{
}


BEGIN_MESSAGE_MAP (CObjectCtrlBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CObjectCtrlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectCtrlBar normal functions

BOOL CObjectCtrlBar::Create (CWnd *pParent)
{
	if (!CSizingControlBar::Create ("Object", pParent, CSize (220, 480), TRUE, AFX_IDW_TOOLBAR + 10))
	{
		return FALSE;
	}

	SetBarStyle (GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	return TRUE;
}

void CObjectCtrlBar::Initialize()
{
	m_pageModel.Initialize();
	m_pageModel.UpdateUI();
	m_pageAnimation.Initialize();
	m_pageAnimation.RegisterObserver();
}

void CObjectCtrlBar::UpdatePage()
{
	switch (m_pageTab.GetCurrentTab())
	{
		case CObjectTabPage::PAGE_TYPE_MODEL:
			m_pageModel.UpdateUI();
			m_pageModel.ShowWindow (SW_SHOW);
			m_pageAnimation.ShowWindow (SW_HIDE);
			break;

		case CObjectTabPage::PAGE_TYPE_ANIMATION:
			m_pageAnimation.UpdateUI();
			m_pageModel.ShowWindow (SW_HIDE);
			m_pageAnimation.ShowWindow (SW_SHOW);
			break;
	}
}

void CObjectCtrlBar::UpdateAnimationPage()
{
	m_pageAnimation.UpdateLoopCount();
}

/////////////////////////////////////////////////////////////////////////////
// CObjectCtrlBar message handlers

int CObjectCtrlBar::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBar::OnCreate (lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!m_pageTab.Create (this))
	{
		return -1;
	}

	if (!m_pageModel.Create (this))
	{
		return -1;
	}

	if (!m_pageAnimation.Create (this))
	{
		return -1;
	}

	m_pageTab.SetTemporaryHeight (0);
	m_pageModel.SetTemporaryHeight (c_iTemporaryHeight);
	m_pageAnimation.SetTemporaryHeight (c_iTemporaryHeight);

	m_pageTab.ShowWindow (SW_SHOW);
	m_pageModel.ShowWindow (SW_SHOW);
	m_pageAnimation.ShowWindow (SW_HIDE);

	return 0;
}

void CObjectCtrlBar::OnSize (UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize (nType, cx, cy);

	CPageCtrl *pWnd = (CPageCtrl*)GetWindow (GW_CHILD);
	while (pWnd != NULL)
	{
		pWnd->CalcLayout();
		pWnd = (CPageCtrl*)pWnd->GetNextWindow();
	}

	RepositioningWindow (m_pageModel, c_iTemporaryWidth, c_iTemporaryHeight);
	RepositioningWindow (m_pageAnimation, c_iTemporaryWidth, c_iTemporaryHeight);
}
