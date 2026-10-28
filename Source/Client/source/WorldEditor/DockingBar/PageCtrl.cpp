// PageCtrl.cpp : implementation file
//
// 게임개발자용 맵에디터
// 저작권자 (c) 2001 신용우 (grace77@hanimail.com)
//
// 이 프로그램의 모든 권리는 저작권자에게 있습니다.
// 저작권자의 동의없이 프로그램을 설치/사용/수정/배포할 수 없습니다.

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "PageCtrl.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageCtrl dialog


CPageCtrl::CPageCtrl (UINT nIDTemplate, CWnd* pParent /*=NULL*/)
	: CDialog (nIDTemplate, pParent)
{
	//{{AFX_DATA_INIT(CPageCtrl)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nScrollPos = 0;
	m_nTemporaryHeight = 0;
}

void CPageCtrl::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CPageCtrl)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CPageCtrl, CDialog)
	//{{AFX_MSG_MAP(CPageCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageCtrl message handlers

void CPageCtrl::CalcLayout()
{
	CRect drc, prc;
	GetClientRect (&drc);
	GetParent()->GetClientRect (&prc);

	if (prc.Width() >= drc.Width())
	{
		int gap = (prc.Width() - drc.Width()) / 2;
		CRect rc = drc;
		rc.left += gap;
		rc.right += gap;
		MoveWindow (&rc);
	}

	// recalc scroll window
	int dy = 0;

	if (m_nScrollPos + dy < 0)
	{
		int max_pos = 0;
		if (drc.Height() > prc.Height() - m_nTemporaryHeight)
		{
			max_pos = drc.Height() - (prc.Height() - m_nTemporaryHeight);
		}

		if (m_nScrollPos + dy > -max_pos)
		{
			ScrollWindow (0, dy);
			m_nScrollPos += dy;
		}
		else
		{
			ScrollWindow (0, -max_pos - m_nScrollPos);
			m_nScrollPos = -max_pos;
		}
	}
	else
	{
		ScrollWindow (0, -m_nScrollPos);
		m_nScrollPos = 0;
	}
}

void CPageCtrl::OnLButtonDown (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();

	SetCapture();
	m_pt = point;

	CDialog::OnLButtonDown (nFlags, point);
}

void CPageCtrl::OnMouseMove (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (nFlags == MK_LBUTTON)
	{
		int dy = point.y - m_pt.y;

		if (m_nScrollPos + dy < 0)
		{
			CRect drc, prc;
			GetClientRect (&drc);
			GetParent()->GetClientRect (&prc);

			int max_pos = 0;
			if (drc.Height() > prc.Height() - m_nTemporaryHeight)
			{
				max_pos = drc.Height() - prc.Height() + m_nTemporaryHeight;
			}

			if (m_nScrollPos + dy > -max_pos)
			{
				ScrollWindow (0, dy);
				m_nScrollPos += dy;
			}
			else
			{
				ScrollWindow (0, -max_pos - m_nScrollPos);
				m_nScrollPos = -max_pos;
			}
		}
		else
		{
			ScrollWindow (0, -m_nScrollPos);
			m_nScrollPos = 0;
		}
	}

	m_pt = point;

	CDialog::OnMouseMove (nFlags, point);
}

void CPageCtrl::OnLButtonUp (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();

	CDialog::OnLButtonUp (nFlags, point);
}


void CPageCtrl::OnOK()
{
}

void CPageCtrl::OnCancel()
{
}

BOOL CPageCtrl::EnableWindow (BOOL bEnable)
{
	CWnd *pWnd = GetWindow (GW_CHILD);
	while (pWnd != NULL)
	{
		pWnd->EnableWindow (bEnable);
		pWnd = pWnd->GetNextWindow();
	}

	return TRUE;
}

BOOL CPageCtrl::OnMouseWheel (UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	int dy = zDelta / 6;

	if (m_nScrollPos + dy < 0)
	{
		CRect drc, prc;
		GetClientRect (&drc);
		GetParent()->GetClientRect (&prc);

		int max_pos = 0;
		if (drc.Height() > prc.Height())
		{
			max_pos = drc.Height() - prc.Height();
		}

		if (m_nScrollPos + dy > -max_pos)
		{
			ScrollWindow (0, dy);
			m_nScrollPos += dy;
		}
		else
		{
			ScrollWindow (0, -max_pos - m_nScrollPos);
			m_nScrollPos = -max_pos;
		}
	}
	else
	{
		ScrollWindow (0, -m_nScrollPos);
		m_nScrollPos = 0;
	}

	return CDialog::OnMouseWheel (nFlags, zDelta, pt);
}


BOOL CPageCtrl::OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	CRect drc, prc;
	GetClientRect (&drc);
	GetParent()->GetClientRect (&prc);

	if (nHitTest == HTCLIENT)
		if (drc.Height() > prc.Height() - m_nTemporaryHeight)
		{
			SetCursor (AfxGetApp()->LoadCursor (IDC_VIEWMOVE));
			return FALSE;
		}

	return CDialog::OnSetCursor (pWnd, nHitTest, message);
}

void CPageCtrl::UpdateUI()
{
}

void CPageCtrl::SetTemporaryHeight (int nTemporaryHeight)
{
	m_nTemporaryHeight = nTemporaryHeight;
}
