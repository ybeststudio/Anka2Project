// GTColorWellWnd.cpp : implementation file
//

#include "StdAfx.h"
#include "GTColorDialogDlg.h"
#include "../resource.h"

#include "GTColorWellWnd.h"
#include "GTDrawHelper.h"
#include "GTColorSelectorWnd.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// GTColorWellWnd

GTColorWellWnd::GTColorWellWnd()
{
	pNotifyWnd = NULL;
	m_crColor = RGB (128, 255, 0);
	m_pParent = NULL;
}

GTColorWellWnd::~GTColorWellWnd()
{
}


BEGIN_MESSAGE_MAP (GTColorWellWnd, CWnd)
	//{{AFX_MSG_MAP(GTColorWellWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CHAR()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
	ON_MESSAGE (WM_FO_SELECTBULLETTYPEOK, OnSelectBulletOK)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// GTColorWellWnd message handlers
BOOL GTColorWellWnd::Create (DWORD dwStyle, CRect rcPos, CWnd* pParent, UINT nID, int nBulletType, BOOL bPopup)
{
	nBulletType;
	bPopup;
	LPVOID lp = (LPVOID)NULL;
	if (!CreateEx (0,
				   AfxRegisterWndClass (CS_DBLCLKS, ::LoadCursor (NULL, IDC_ARROW)),
				   _T ("GTColorWellWnd"),
				   dwStyle,
				   rcPos.left,
				   rcPos.top,
				   rcPos.Width(),
				   rcPos.Height(),
				   pParent->GetSafeHwnd(),
				   (HMENU) nID,
				   lp))

	{
		// Not a lot we can do.
		TRACE0 ("Failed to create GTColorWellWnd\n");
		return FALSE;
	}
	pNotifyWnd = pParent;

	UpdateWindow();
	return TRUE;
}

BOOL GTColorWellWnd::OnEraseBkgnd (CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	pDC;
	return TRUE;
}

void GTColorWellWnd::OnLButtonDblClk (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnLButtonDblClk (nFlags, point);
}

void GTColorWellWnd::OnLButtonDown (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetCapture();
	::SetFocus (m_hWnd);

	OnMouseMove (nFlags, point);
}

void GTColorWellWnd::OnLButtonUp (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetCapture() == this)
	{
		::ReleaseCapture();
	}
	CWnd::OnLButtonUp (nFlags, point);
}

void GTColorWellWnd::OnMButtonDblClk (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonDblClk (nFlags, point);
}

void GTColorWellWnd::OnMButtonDown (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonDown (nFlags, point);
}

void GTColorWellWnd::OnMButtonUp (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonUp (nFlags, point);
}

void GTColorWellWnd::OnMouseMove (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (GetCapture() != this)
	{
		return;
	}
	CWnd::OnMouseMove (nFlags, point);
}

void GTColorWellWnd::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnChar (nChar, nRepCnt, nFlags);
}

void GTColorWellWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here

}

void GTColorWellWnd::OnPaint()
{
	CPaintDC dcPaint (this); // device context for painting
	CRect frame;
	GetClientRect (frame);

	CDC dc;
	dc.CreateCompatibleDC (&dcPaint);
	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap (&dcPaint, frame.Width(), frame.Height());
	CBitmap* pBmpOld = dc.SelectObject (&bmpMem);

	HDC		hdc = dc.m_hDC;
	HBRUSH	fill_brush, old_brush;
	COLORREF color;
	color = m_crColor;
	fill_brush = CreateSolidBrush (color);
	old_brush = (HBRUSH) SelectObject (hdc, fill_brush);
	SelectObject (hdc, GetStockObject (NULL_PEN));
	Rectangle (hdc, frame.left + 2, frame.top + 2, frame.right, frame.bottom);

	SelectObject (hdc, old_brush);
	DeleteObject (fill_brush);

	CRect rcTemp = frame;
	rcTemp.DeflateRect (0, 0, 1, 1);
	GTDrawHelper m_Panel;
	m_Panel.DrawPanel (hdc, &rcTemp, GTDrawHelper::sunken | GTDrawHelper::blackbox);

	dcPaint.BitBlt (frame.left, frame.top,	frame.Width(), frame.Height(), &dc, 0, 0, SRCCOPY);

	dc.SelectObject (pBmpOld);
}

void GTColorWellWnd::OnRButtonDown (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnRButtonDown (nFlags, point);
}

void GTColorWellWnd::OnRButtonUp (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnRButtonUp (nFlags, point);
}

void GTColorWellWnd::OnCancelMode()
{
	CWnd::OnCancelMode();
}

void GTColorWellWnd::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnKeyDown (nChar, nRepCnt, nFlags);
}

BOOL GTColorWellWnd::PreCreateWindow (CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::PreCreateWindow (cs);
}

void GTColorWellWnd::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	CWnd::PreSubclassWindow();
}

void GTColorWellWnd::OnSize (UINT nType, int cx, int cy)
{
	CWnd::OnSize (nType, cx, cy);
	CRect rc;
	GetClientRect (&rc);

}

LRESULT GTColorWellWnd::OnSelectBulletOK (WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;
	// We receive this on a color change from one of the well buttons
	// or one of the embedded color wells.
	GTColorSelectorWnd *picker = (GTColorSelectorWnd*)pNotifyWnd;

	m_crColor = picker->m_crSaveColor;
	m_pParent->ColorChanged();
	Invalidate();
	return 0L;
}

void GTColorWellWnd::OnSysColorChange()
{
	//	gfxData.OnSysColorChange();
	Invalidate();
}

UINT GTColorWellWnd::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default
	ASSERT_VALID (this);

	UINT nDlgCode = DLGC_WANTARROWS | DLGC_WANTCHARS;
	nDlgCode |= DLGC_WANTTAB;

	return nDlgCode;
}
