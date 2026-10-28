#include "StdAfx.h"
#include "../MainFrm.h"
#include "FlyCtrlBar.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


BEGIN_MESSAGE_MAP (CFlyCtrlBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CFlyCtrlBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFlyCtrlBar

CFlyCtrlBar::CFlyCtrlBar()
{
}

CFlyCtrlBar::~CFlyCtrlBar()
{
}


BOOL CFlyCtrlBar::Create (CWnd * pParent)
{
	if (!CSizingControlBar::Create ("Fly", pParent, CSize (220, 480), TRUE, AFX_IDW_TOOLBAR + 14))
	{
		return FALSE;
	}

	SetBarStyle (GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	return TRUE;
}

void CFlyCtrlBar::Initialize()
{
	m_pageTab.Initialize();
	m_pageTab.UpdateUI();
}

void CFlyCtrlBar::UpdatePage()
{
}
/////////////////////////////////////////////////////////////////////////////
// CFlyCtrlBar message handlers

/*BOOL CFlyCtrlBar::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}*/

int CFlyCtrlBar::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBar::OnCreate (lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!m_pageTab.Create (this))
	{
		return -1;
	}

	m_pageTab.SetTemporaryHeight (0);
	m_pageTab.ShowWindow (SW_SHOW);

	return 0;
}
