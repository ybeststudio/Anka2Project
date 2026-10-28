// ColorAnimationDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ColorAnimationDlg.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

CColorAnimationDlg* CColorAnimationDlg::ms_pThis = NULL;


/////////////////////////////////////////////////////////////////////////////
// CColorAnimationDlg dialog
CColorAnimationDlg::CColorAnimationDlg (CWnd* pParent /*=NULL*/)
	: CDialog (CColorAnimationDlg::IDD, pParent)
{
	m_dwSelectedAlpha = 0xffffffff;
	m_dwSelectedColor = 0xffffffff;
	ms_pThis = this;
	//{{AFX_DATA_INIT(CColorAnimationDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CColorAnimationDlg::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CColorAnimationDlg)
	DDX_Control (pDX, IDC_COLOR_ANIMATION_ALPHA, m_ctrlAlpha);
	DDX_Control (pDX, IDC_COLOR_ANIMATION_COLOR, m_ctrlColor);
	DDX_Control (pDX, IDC_RGB_BAR, m_ctrlRGBBar);
	DDX_Control (pDX, IDC_ALPHA_BAR, m_ctrlAlphaBar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CColorAnimationDlg, CDialog)
	//{{AFX_MSG_MAP(CColorAnimationDlg)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_HSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorAnimationDlg message handlers

/////////////////////////////////////////////////////////////////////////////
// CColorAnimationDlg normal functions

void CColorAnimationDlg::CallBack()
{
	if (ms_pThis->m_dwSelectedColor != 0xffffffff)
	{
		COLORREF Color;
		ms_pThis->m_ctrlColor.GetColor (&Color);

		ms_pThis->m_pRedAccessor  ->SetValue (ms_pThis->m_dwSelectedColor, GetRValue (Color) / 255.0f);
		ms_pThis->m_pGreenAccessor->SetValue (ms_pThis->m_dwSelectedColor, GetGValue (Color) / 255.0f);
		ms_pThis->m_pBlueAccessor ->SetValue (ms_pThis->m_dwSelectedColor, GetBValue (Color) / 255.0f);

		ms_pThis->Invalidate (FALSE);
	}
}

void CColorAnimationDlg::SetRGBAAccessor (
	CTimeEventFloatAccessor * pRedAccessor,
	CTimeEventFloatAccessor * pGreenAccessor,
	CTimeEventFloatAccessor * pBlueAccessor,
	CTimeEventFloatAccessor * pAlphaAccessor)
{
	m_pRedAccessor   = pRedAccessor;
	m_pGreenAccessor = pGreenAccessor;
	m_pBlueAccessor  = pBlueAccessor;
	m_pAlphaAccessor = pAlphaAccessor;

	AlignRGB();

	ms_pThis = this;
}

void CColorAnimationDlg::OnPaint()
{
	CWindowDC dc (this); // device context for painting

	//CDialog::OnPaint();
	CRect rAll;
	GetWindowRect (&rAll);

	// draw RGB
	{
		CRect r;
		m_ctrlRGBBar.GetWindowRect (&r);
		r.left -= rAll.left;
		r.right -= rAll.left;
		r.top -= rAll.top;
		r.bottom -= rAll.top;
		{
			int i;
			for (i = 0; i < r.Width(); i++)
			{
				float rv, gv, bv;
				m_pRedAccessor->GetTimeValue (i * 1.0f / r.Width(), &rv);
				m_pGreenAccessor->GetTimeValue (i * 1.0f / r.Width(), &gv);
				m_pBlueAccessor->GetTimeValue (i * 1.0f / r.Width(), &bv);
				CPen pen (PS_SOLID, 1, RGB (rv * 255, gv * 255, bv * 255));
				CPen * oldpen;
				oldpen = dc.SelectObject (&pen);
				dc.MoveTo (r.left + i, r.top);
				dc.LineTo (r.left + i, r.bottom);
				dc.SelectObject (oldpen);
			}
			{
				CBrush brush (GetSysColor (COLOR_BTNFACE));
				CRect rect (r.left - 5, r.bottom, r.right + 5, r.bottom + 6);
				dc.FillRect (&rect, &brush);
			}
		}
		CBrush brush (RGB (0, 0, 0));
		//CBrush * oldbrush;
		dc.SelectObject (&brush);

		{
			DWORD i;
			for (i = 0; i < m_pRedAccessor->GetValueCount(); i++)
			{
				int x;
				float time;
				if (m_pRedAccessor->GetTime (i, &time))
				{
					x = time * r.Width() + r.left;
					dc.BeginPath();
					dc.MoveTo (x, r.bottom);
					dc.LineTo (x - 5, r.bottom + 5);
					dc.LineTo (x + 5, r.bottom + 5);
					dc.LineTo (x, r.bottom);
					dc.EndPath();
					if (i == m_dwSelectedColor)
					{
						dc.StrokeAndFillPath();
					}
					else
					{
						dc.StrokePath();
					}
				}
			}
		}
	}

	// draw alpha
	{
		CRect r;
		m_ctrlAlphaBar.GetWindowRect (&r);
		r.left -= rAll.left;
		r.right -= rAll.left;
		r.top -= rAll.top;
		r.bottom -= rAll.top;
		{
			int i;
			for (i = 0; i < r.Width(); i++)
			{
				float alpha;
				m_pAlphaAccessor->GetTimeValue (i * 1.0f / r.Width(), &alpha);
				CPen pen (PS_SOLID, 1, RGB (alpha * 255, alpha * 255, alpha * 255));
				CPen * oldpen;
				oldpen = dc.SelectObject (&pen);
				dc.MoveTo (r.left + i, r.top);
				dc.LineTo (r.left + i, r.bottom);
				dc.SelectObject (oldpen);
			}
		}
		{
			CBrush brush (GetSysColor (COLOR_BTNFACE));
			CRect rect (r.left - 5, r.bottom, r.right + 5, r.bottom + 6);
			dc.FillRect (&rect, &brush);
		}
		CBrush brush (RGB (0, 0, 0));
		//CBrush * oldbrush;
		dc.SelectObject (&brush);

		{
			DWORD i;
			for (i = 0; i < m_pAlphaAccessor->GetValueCount(); i++)
			{
				int x;
				float time;
				if (m_pAlphaAccessor->GetTime (i, &time))
				{
					x = time * r.Width() + r.left;
					dc.BeginPath();
					dc.MoveTo (x, r.bottom);
					dc.LineTo (x - 5, r.bottom + 5);
					dc.LineTo (x + 5, r.bottom + 5);
					dc.LineTo (x, r.bottom);
					dc.EndPath();
					if (i == m_dwSelectedAlpha)
					{
						dc.StrokeAndFillPath();
					}
					else
					{
						dc.StrokePath();
					}
				}
			}
		}
	}

	this->ValidateRect (0);
	m_ctrlColor.Update();
	m_ctrlAlpha.Invalidate();
	// Do not call CDialog::OnPaint() for painting messages
}

void CColorAnimationDlg::AlignRGB()
{
	struct Aligner
	{
		CTimeEventFloatAccessor* m_pAccessor;
		DWORD m_iIndex;

		Aligner (CTimeEventFloatAccessor * pAccessor)
			: m_pAccessor (pAccessor), m_iIndex (0)
		{
		}

		float MinTime()
		{
			if (m_iIndex < m_pAccessor->GetValueCount())
			{
				float time;
				if (m_pAccessor->GetTime (m_iIndex, &time))
				{
					return time;
				}
			}

			// NOTE : 1.0f is max time

			return 1.1f;
		}

		void Proceed()
		{
			m_iIndex ++;
		}
	};

	Aligner a[3] = { Aligner (m_pRedAccessor), Aligner (m_pGreenAccessor), Aligner (m_pBlueAccessor) };

	while (a[0].MinTime() < 1.1f || a[1].MinTime() < 1.1f || a[2].MinTime() < 1.1f)
	{
		if (a[1].MinTime() == a[2].MinTime())
		{
			if (a[1].MinTime() == a[0].MinTime())
			{
				// Same time, next
			}
			else if (a[1].MinTime() < a[0].MinTime())
			{
				a[0].m_pAccessor->InsertBlend (a[1].MinTime());
			}
			else
			{
				a[1].m_pAccessor->InsertBlend (a[0].MinTime());
				a[2].m_pAccessor->InsertBlend (a[0].MinTime());
			}
		}
		else if (a[1].MinTime() < a[2].MinTime())
		{
			if (a[1].MinTime() == a[0].MinTime())
			{
				a[2].m_pAccessor->InsertBlend (a[0].MinTime());
			}
			else if (a[1].MinTime() < a[0].MinTime())
			{
				a[0].m_pAccessor->InsertBlend (a[1].MinTime());
				a[2].m_pAccessor->InsertBlend (a[1].MinTime());
			}
			else
			{
				a[1].m_pAccessor->InsertBlend (a[0].MinTime());
				a[2].m_pAccessor->InsertBlend (a[0].MinTime());
			}
		}
		else
		{
			if (a[2].MinTime() == a[0].MinTime())
			{
				a[1].m_pAccessor->InsertBlend (a[0].MinTime());
			}
			else if (a[2].MinTime() < a[0].MinTime())
			{
				a[0].m_pAccessor->InsertBlend (a[2].MinTime());
				a[1].m_pAccessor->InsertBlend (a[2].MinTime());
			}
			else
			{
				a[1].m_pAccessor->InsertBlend (a[0].MinTime());
				a[2].m_pAccessor->InsertBlend (a[0].MinTime());
			}
		}

		a[0].Proceed();
		a[1].Proceed();
		a[2].Proceed();
	}
}

void CColorAnimationDlg::OnMouseMove (UINT nFlags, CPoint point)
{
	ClientToScreen (&point);
	CRect rect, recta;
	m_ctrlRGBBar.GetWindowRect (&rect);
	m_ctrlAlphaBar.GetWindowRect (&recta);

	if (m_iCurrentSelection != SELECT_NONE)
	{
		if (point.x < m_iLeftLimit)
		{
			point.x = m_iLeftLimit;
		}

		if (point.x > m_iRightLimit)
		{
			point.x = m_iRightLimit;
		}

		if (point.x < rect.left)
		{
			point.x = rect.left;
		}

		if (point.x >= rect.right)
		{
			point.x = rect.right - 1;
		}

		float newtime = (point.x - rect.left) * 1.0f / rect.Width();

		switch (m_iCurrentSelection)
		{
			case SELECT_COLOR:
				m_pRedAccessor->SetTime (m_dwSelectedColor, newtime);
				m_pGreenAccessor->SetTime (m_dwSelectedColor, newtime);
				m_pBlueAccessor->SetTime (m_dwSelectedColor, newtime);
				break;
			case SELECT_ALPHA:
				m_pAlphaAccessor->SetTime (m_dwSelectedAlpha, newtime);
				break;
		}
		Invalidate (FALSE);
	}

	CDialog::OnMouseMove (nFlags, point);
}

void CColorAnimationDlg::OnLButtonDown (UINT nFlags, CPoint point)
{

	CRect rectColor, rectAlpha;

	ClientToScreen (&point);

	m_ctrlRGBBar.GetWindowRect (&rectColor);
	m_ctrlAlphaBar.GetWindowRect (&rectAlpha);

	int left = rectColor.left;
	int right = rectColor.right;
	int width = rectColor.Width();

	rectColor.left -= 5;
	rectColor.right += 5;

	rectAlpha.left -= 5;
	rectAlpha.right += 5;

	rectColor.bottom += 5;
	rectAlpha.bottom += 5;

	m_iLeftLimit = rectColor.left;
	m_iRightLimit = rectColor.right - 1;

	m_iCurrentSelection = SELECT_NONE;

	DWORD i;
	DWORD oldSelectedColor = m_dwSelectedColor;
	DWORD oldSelectedAlpha = m_dwSelectedAlpha;

	if (PtInRect (&rectColor, point))
	{
		for (i = 0; i < m_pRedAccessor->GetValueCount(); i++)
		{
			float time;
			int tx;
			if (!m_pRedAccessor->GetTime (i, &time))
			{
				continue;
			}
			tx = time * width + left;
			if (abs (point.x - tx) < 4)
			{
				m_dwSelectedColor = i;

				if (i > 0)
				{
					m_pRedAccessor->GetTime (i - 1, &time);
					m_iLeftLimit = 5 + time * width + left;
				}

				if (i < m_pRedAccessor->GetValueCount() - 1)
				{
					m_pRedAccessor->GetTime (i + 1, &time);
					m_iRightLimit = -5 + time * width + left;
				}

				{
					float r, g, b;
					m_pRedAccessor->GetValue (m_dwSelectedColor, &r);
					m_pGreenAccessor->GetValue (m_dwSelectedColor, &g);
					m_pBlueAccessor->GetValue (m_dwSelectedColor, &b);
					m_ctrlColor.SetColor (RGB (r * 255, g * 255, b * 255));
				}

				m_iCurrentSelection = SELECT_COLOR;
				break;
			}
		}

		if (m_iCurrentSelection == SELECT_NONE)
		{
			if (nFlags & MK_CONTROL)
			{
				float fNewTime;
				fNewTime = (point.x - left) * 1.0f / width;
				if (fNewTime >= 0.0f && fNewTime <= 1.0f)
				{
					m_pRedAccessor->InsertBlend (fNewTime);
					m_pGreenAccessor->InsertBlend (fNewTime);
					m_pBlueAccessor->InsertBlend (fNewTime);
					Invalidate (FALSE);
				}
			}
		}
	}

	if (PtInRect (&rectAlpha, point))
	{
		for (i = 0; i < m_pAlphaAccessor->GetValueCount(); i++)
		{
			float time;
			int tx;
			if (!m_pAlphaAccessor->GetTime (i, &time))
			{
				continue;
			}
			tx = time * width + left;
			if (abs (point.x - tx) < 4)
			{
				m_dwSelectedAlpha = i;

				if (i > 0)
				{
					m_pAlphaAccessor->GetTime (i - 1, &time);
					m_iLeftLimit = time * width + left;
				}

				if (i < m_pAlphaAccessor->GetValueCount() - 1)
				{
					m_pAlphaAccessor->GetTime (i + 1, &time);
					m_iRightLimit = time * width + left;
				}

				float a;
				m_pAlphaAccessor->GetValue (m_dwSelectedAlpha, &a);

				m_ctrlAlpha.SetPos (a * 100);

				m_iCurrentSelection = SELECT_ALPHA;
				break;
			}
		}

		if (m_iCurrentSelection == SELECT_NONE)
		{
			if (nFlags & MK_CONTROL)
			{
				float fNewTime;
				fNewTime = (point.x - left) * 1.0f / width;
				if (fNewTime >= 0.0f && fNewTime <= 1.0f)
				{
					m_pAlphaAccessor->InsertBlend (fNewTime);
					Invalidate (FALSE);
				}
			}
		}
	}

	if (m_iCurrentSelection != SELECT_NONE)
	{
		SetCapture();
	}

	CDialog::OnLButtonDown (nFlags, point);
}

void CColorAnimationDlg::OnLButtonUp (UINT nFlags, CPoint point)
{
	ReleaseCapture();
	m_iCurrentSelection = SELECT_NONE;

	CDialog::OnLButtonUp (nFlags, point);
}


BOOL CColorAnimationDlg::Create (CWnd* pParent)
{
	// TODO: Add your specialized code here and/or call the base class
	if (!CDialog::Create (IDD, pParent))
	{
		return FALSE;
	}
	if (!m_ctrlColor.Create (this))
	{
		return FALSE;
	}

	m_ctrlColor.pfnCallBack = CallBack;
	return TRUE;
}

void CColorAnimationDlg::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	if (IDC_COLOR_ANIMATION_ALPHA == m_iSliderIndex && m_dwSelectedAlpha != 0xffffffff)
	{
		m_pAlphaAccessor->SetValue (m_dwSelectedAlpha, m_ctrlAlpha.GetPos() / 100.0f);
		Invalidate (FALSE);
	}

	CDialog::OnHScroll (nSBCode, nPos, pScrollBar);
}


BOOL CColorAnimationDlg::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	m_iSliderIndex = wParam;

	return CDialog::OnNotify (wParam, lParam, pResult);
}

void CColorAnimationDlg::OnRButtonDown (UINT nFlags, CPoint point)
{
	if (nFlags & MK_CONTROL)
	{
		OnLButtonDown (0, point);
		if (m_iCurrentSelection == SELECT_COLOR && m_dwSelectedColor != 0xffffffff)
		{
			m_pRedAccessor->Delete (m_dwSelectedColor);
			m_pGreenAccessor->Delete (m_dwSelectedColor);
			m_pBlueAccessor->Delete (m_dwSelectedColor);
			Invalidate (FALSE);
			m_dwSelectedColor = 0xffffffff;
		}

		if (m_iCurrentSelection == SELECT_ALPHA && m_dwSelectedAlpha != 0xffffffff)
		{
			m_pAlphaAccessor->Delete (m_dwSelectedAlpha);
			Invalidate (FALSE);
			m_dwSelectedAlpha = 0xffffffff;
		}
	}
	CDialog::OnRButtonDown (nFlags, point);
}

void CColorAnimationDlg::OnRButtonUp (UINT nFlags, CPoint point)
{

	CDialog::OnRButtonUp (nFlags, point);
}
