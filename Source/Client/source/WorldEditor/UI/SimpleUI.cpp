// SimpleUI.cpp : implementation file
//
// 게임개발자용 맵에디터
// 저작권자 (c) 2001 신용우 (grace77@hanimail.com)
//
// 이 프로그램의 모든 권리는 저작권자에게 있습니다.
// 저작권자의 동의없이 프로그램을 설치/사용/수정/배포할 수 없습니다.

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "../MainFrm.h"

#include "SimpleUI.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorBox

CColorBox::CColorBox() : m_dlgLight (NULL)
{
}

CColorBox::~CColorBox()
{
	if (m_dlgLight)
	{
		delete m_dlgLight;
		m_dlgLight = NULL;
	}
}

BEGIN_MESSAGE_MAP (CColorBox, CStatic)
	//{{AFX_MSG_MAP(CColorBox)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorBox message handlers

BOOL CColorBox::Create (CWnd * pParent)
{
	pfnCallBack = NULL;
	m_Observer.SetOwner (this);

	m_dlgLight = new CGTColorDialogDlg (pParent);
	m_dlgLight->SetObserver (&m_Observer);
	m_dlgLight->Create (IDD_GTCOLORDIALOG_DIALOG, pParent);
	m_dlgLight->SetColor (RGB (250, 250, 250));
	m_dlgLight->ShowWindow (FALSE);
	m_dlgLight->SetWindowText ("Light");
	m_dlgLight->SetWindowPos (NULL, 0, 0, 0, 0, true);

	return TRUE;
}

void CColorBox::OnPaint()
{
	CPaintDC dc (this); // device context for painting

	CRect rc;
	GetClientRect (&rc);

	CBrush black (RGB (0, 0, 0)), clr (m_Color);
	dc.FillRect (&rc, &black);

	rc.DeflateRect (1, 1);
	dc.FillRect (&rc, &clr);
}

void CColorBox::OnLButtonDown (UINT nHitTest, CPoint point)
{
	if (pfnCallBack)
	{
		(*pfnCallBack)();
	}

	m_dlgLight->ShowWindow (TRUE);
	CStatic::OnLButtonDown (nHitTest, point);
}

void CColorBox::SetColor (const COLORREF & rColor)
{
	m_Color = rColor;
}

void CColorBox::GetColor (COLORREF * pColor)
{
	*pColor = m_Color;
}

void CColorBox::Update()
{
	m_dlgLight->SetColor (m_Color);
	Invalidate();
}

void CColorBox::RecvColor (COLORREF & rColor)
{
	m_Color = rColor;

	Invalidate();

	if (pfnCallBack)
	{
		(*pfnCallBack)();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CStatusProgress

BEGIN_MESSAGE_MAP (CStatusProgress, CProgressCtrl)
	//{{AFX_MSG_MAP(CStatusProgress)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatusProgress message handlers
static CProgressCtrl* pProg = NULL;

void CStatusProgress::Create()
{
	m_pWait = new CWaitCursor;

	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	//	CStatusBar *pStatus = &pFrame->m_wndStatusBar;

	CRect item_rc, rc;
	for (int i = 0; i < 4; i++)
	{
		//		pStatus->GetItemRect(i + 1, &item_rc);

		if (i == 0)
		{
			rc = item_rc;
		}
		else
		{
			rc.UnionRect (&rc, &item_rc);
		}
	}

	//	CProgressCtrl::Create(WS_CHILD | WS_VISIBLE,
	//		rc, pStatus, 3000);

	pProg = this;
}

static int nUpdateDelay = 0, nCurDelay = 0;

void CStatusProgress::Destroy()
{
	nUpdateDelay = nCurDelay = 0;

	DestroyWindow();
	m_hWnd = NULL;
}

void CStatusProgress::OnDestroy()
{
	CProgressCtrl::OnDestroy();

	// TODO: Add your message handler code here
	m_pWait->Restore();
	delete m_pWait;
}


void CStatusProgress::SetCallbackPos (int nPos)
{
	if (nCurDelay >= nUpdateDelay)
	{
		nCurDelay = 0;

		ASSERT (pProg);
		pProg->SetPos (nPos);
	}

	nCurDelay++;
}

void CStatusProgress::SetCallbackDelay (int nDelay)
{
	nCurDelay = 0;
	nUpdateDelay = nDelay;
}

void CStatusProgress::SetText (CString s)
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	//	CStatusBar *p = &pFrame->m_wndStatusBar;

	//	p->SetWindowText(s);
}
