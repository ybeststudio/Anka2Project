// GTColorSliderWnd.cpp : implementation file
//

#include "StdAfx.h"
#include "GTColorSliderWnd.h"
#include "GTColorSelectorWnd.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// GTColorSliderWnd

GTColorSliderWnd::GTColorSliderWnd()
{
	pNotifyWnd = NULL;
	// initialize
	m_nCurMode = 0;
	m_nCurLayout = 0x50017280;
	m_nStartValue = -1;
	m_nEndValue = -1;
	m_nCurPosition = -1;
	m_nPreviousPos = -1;
	m_nSteps = -1;
	m_nPageSizes = -1;
	m_nTriangle = 6;
	m_bTracking = false;
	bmp_handle = NULL;
	row_buffer = NULL;
	additional_components[0] =
		additional_components[1] =
			additional_components[2] =
				additional_components[3] = -1;
}

GTColorSliderWnd::~GTColorSliderWnd()
{
	if (bmp_handle)
	{
		DeleteObject (bmp_handle);
	}
	if (row_buffer)
	{
		delete [] row_buffer;
	}
}


BEGIN_MESSAGE_MAP (GTColorSliderWnd, CWnd)
	//{{AFX_MSG_MAP(GTColorSliderWnd)
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
// GTColorSliderWnd message handlers
BOOL GTColorSliderWnd::Create (DWORD dwStyle, CRect rcPos, CWnd* pParent, UINT nID, int nBulletType, BOOL bPopup)
{
	nBulletType;
	bPopup;
	LPVOID lp = (LPVOID)NULL;
	if (!CreateEx (0,
				   AfxRegisterWndClass (CS_DBLCLKS, ::LoadCursor (NULL, IDC_ARROW)),
				   _T ("GTColorSliderWnd"),
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
		TRACE0 ("Failed to create GTColorSliderWnd\n");
		return FALSE;
	}
	pNotifyWnd = NULL;
	// no default mode; should be set using window style
	SetLayoutValue (0x50017280);
	SetDataValue (1);

	UpdateWindow();
	return TRUE;
}

BOOL GTColorSliderWnd::OnEraseBkgnd (CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	pDC;
	return TRUE;
}

void GTColorSliderWnd::OnLButtonDblClk (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnLButtonDblClk (nFlags, point);
}

void GTColorSliderWnd::OnLButtonDown (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// TODO: Add your message handler code here and/or call default
	POINT	pt = point;
	::SetFocus (m_hWnd);
	if (PtInRect (&m_rcInteraction, pt))
	{
		::SetCapture (m_hWnd);
		m_bTracking = true;
		OnMouseMove (nFlags, point);
	}
}

LRESULT GTColorSliderWnd::SendMessageToParent (IN UINT code)
{
	UINT	control_id;
	HWND	parent_window;
	NMHDR	message1;

	ASSERT (m_hWnd != NULL);

	control_id = GetDlgCtrlID();
	if (control_id == 0)
		// failed to retrieve control's id
	{
		return (-1);
	}

	// setup structure
	message1.code = code;
	message1.hwndFrom = m_hWnd;
	message1.idFrom = control_id;

	if (pNotifyWnd != NULL)
	{
		// send message as notification to the controller window
		return (::SendMessage (pNotifyWnd->m_hWnd, WM_FO_SELECTBULLETTYPEOK, control_id, (LPARAM) &message1));
	}

	parent_window = (HWND)GetParent();
	if (parent_window != NULL)
		// send WM_NOTIFY message to the parent window
	{
		return (::SendMessage (parent_window, WM_FO_SELECTBULLETTYPEOK, control_id, (LPARAM) &message1));
	}

	return (-1);
}

void GTColorSliderWnd::OnLButtonUp (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (m_bTracking)
	{
		ReleaseCapture();
		m_bTracking = false;
		SendMessageToParent (m_nCurPosition);
	}
	CWnd::OnLButtonUp (nFlags, point);
}

void GTColorSliderWnd::OnMButtonDblClk (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonDblClk (nFlags, point);
}

void GTColorSliderWnd::OnMButtonDown (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonDown (nFlags, point);
}

void GTColorSliderWnd::OnMButtonUp (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonUp (nFlags, point);
}

void GTColorSliderWnd::OnMouseMove (UINT nFlags, CPoint point)
{
	nFlags;
	// TODO: Add your message handler code here and/or call default
	double	d;
	int		new_pos;

	SetCursor (LoadCursor (NULL, IDC_ARROW));
	if (!m_bTracking)
	{
		return;
	}
	int x;
	int y;
	x = point.x;
	y = point.y;
	if (m_nCurLayout & layout_horizontal)
	{
		// horizontal
		in_range (x, m_rcPaint.left, m_rcPaint.right);

		d = static_cast <double> (x - m_rcPaint.left) /
			static_cast <double> (m_rcPaint.right - m_rcPaint.left);
	}
	else // if (mode & mode_vertical)
	{
		// vertical
		in_range (y, m_rcPaint.top, m_rcPaint.bottom);

		d = static_cast <double> (y - m_rcPaint.top) /
			static_cast <double> (m_rcPaint.bottom - m_rcPaint.top);
	}

	new_pos = static_cast <int> ((m_nEndValue - m_nStartValue) * d +
								 (m_nEndValue > m_nStartValue ? 0.5 : -0.5)) + m_nStartValue;
	if (new_pos == m_nCurPosition)
		// don't redraw if not necessary
	{
		return;
	}

	m_nCurPosition = new_pos;
	InvalidateRect (NULL, FALSE);

	//	debug_state("mouse_move, m_nCurPosition = %d in [%d, %d]", m_nCurPosition, m_nStartValue, m_nEndValue);

	SendMessageToParent (m_nCurPosition);

}

void GTColorSliderWnd::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnChar (nChar, nRepCnt, nFlags);
}

void GTColorSliderWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here

}

void GTColorSliderWnd::OnPaint()
{
	CPaintDC dcPaint (this); // device context for painting
	HDC		hdc;

	// create compatible DC
	hdc = CreateCompatibleDC (dcPaint.m_hDC);
	if (hdc != NULL)
	{
		HBRUSH	bg_fill;
		double	d;

		SelectObject (hdc, bmp_handle);

		// paint control's background
		bg_fill = CreateSolidBrush (GetSysColor (COLOR_BTNFACE));
		SelectObject (hdc, GetStockObject (NULL_PEN));
		SelectObject (hdc, bg_fill);
		Rectangle (hdc, 0, 0, m_rcFrame.right - m_rcFrame.left + 1, m_rcFrame.bottom - m_rcFrame.top + 1);
		SelectObject (hdc, GetStockObject (WHITE_BRUSH));
		DeleteObject (bg_fill);

		// paint blend and m_rcFrame
		GTDrawHelper m_Panel;
		m_Panel.DrawPanel (hdc, &m_rcPanel, GTDrawHelper::sunken | GTDrawHelper::blackbox);
		GdiFlush();
		OnDraw();

		d = (double) (m_nCurPosition - m_nStartValue) / (m_nEndValue - m_nStartValue);

		if (m_nCurLayout & layout_horizontal)
		{
			// horizontal slider
			int		x;

			x = (int) ((m_rcPaint.right - m_rcPaint.left) * d);

			if (m_nCurLayout & layout_tr_top)
			{
				GTDrawHelper m_Panel;
			}

			m_Panel.DrawTriangle (hdc, m_rcPaint.left + x, m_rcPanel.top - 2,
								  GTDrawHelper::bottom, m_nTriangle,
								  GetSysColor (COLOR_WINDOWTEXT), GetSysColor (COLOR_WINDOW));

			if (m_nCurLayout & layout_tr_bottom)
			{
				GTDrawHelper m_Panel;
			}

			m_Panel.DrawTriangle (hdc, m_rcPaint.left + x, m_rcPanel.bottom + 2,
								  GTDrawHelper::top, m_nTriangle,
								  GetSysColor (COLOR_WINDOWTEXT), GetSysColor (COLOR_WINDOW));
		}
		else // if (m_nCurLayout & layout_vertical)
		{
			// vertical slider
			int	y;

			y = (int) ((m_rcPaint.bottom - m_rcPaint.top) * d);

			if (m_nCurLayout & layout_tr_left)
			{
				GTDrawHelper m_Panel;
			}

			m_Panel.DrawTriangle (hdc, m_rcPanel.left - 2, m_rcPaint.top + y,
								  GTDrawHelper::right, m_nTriangle,
								  GetSysColor (COLOR_WINDOWTEXT), GetSysColor (COLOR_WINDOW));

			if (m_nCurLayout & layout_tr_right)
			{
				GTDrawHelper m_Panel;
			}

			m_Panel.DrawTriangle (hdc, m_rcPanel.right + 2, m_rcPaint.top + y,
								  GTDrawHelper::left, m_nTriangle,
								  GetSysColor (COLOR_WINDOWTEXT), GetSysColor (COLOR_WINDOW));
		}

		m_nPreviousPos = m_nCurPosition;

		if (::GetFocus() == m_hWnd)
		{
			DrawFocusRect (hdc, &m_rcFrame);
		}

		// bitblt and delete cache DC
		GdiFlush();
		BitBlt (dcPaint.m_hDC, 0, 0,
				m_rcFrame.right - m_rcFrame.left, m_rcFrame.bottom - m_rcFrame.top, hdc, 0, 0, SRCCOPY);
		DeleteDC (hdc);
	}
}

void GTColorSliderWnd::OnRButtonDown (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CWnd::OnRButtonDown (nFlags, point);
}

void GTColorSliderWnd::OnRButtonUp (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnRButtonUp (nFlags, point);
}

void GTColorSliderWnd::OnCancelMode()
{
	CWnd::OnCancelMode();
}

void GTColorSliderWnd::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	nFlags;
	nRepCnt;
	int		new_pos;
	int		delta;

	// don't process if mouse is being tracked
	if (m_bTracking)
	{
		return;
	}

	// process scroll operation
	new_pos = m_nCurPosition;
	delta = 0;
	switch (nChar)
	{
		case	VK_UP:
		case	VK_LEFT:
			delta = -m_nSteps;
			break;
		case	VK_DOWN:
		case	VK_RIGHT:
			delta = m_nSteps;
			break;
		case	VK_PRIOR:
			delta = -m_nPageSizes;
			break;
		case	VK_NEXT:
			delta = m_nPageSizes;
			break;
		case	VK_HOME:
			new_pos = m_nStartValue;
			break;
		case	VK_END:
			new_pos = m_nEndValue;
			break;
		default:
			return;
	}

	if (m_nEndValue > m_nStartValue)
	{
		new_pos += delta;
		in_range (new_pos, m_nStartValue, m_nEndValue);
	}
	else
	{
		new_pos -= delta;
		in_range (new_pos, m_nEndValue, m_nStartValue);
	}

	if (new_pos == m_nCurPosition)
		// don't redraw if not necessary
	{
		return;
	}

	m_nCurPosition = new_pos;
	SendMessageToParent (m_nCurPosition);
	InvalidateRect (NULL, FALSE);

	//	debug_state("key_down, m_nCurPosition = %d in [%d, %d]", m_nCurPosition, m_nStartValue, m_nEndValue);

}

BOOL GTColorSliderWnd::PreCreateWindow (CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::PreCreateWindow (cs);
}

void GTColorSliderWnd::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	CWnd::PreSubclassWindow();
}

void GTColorSliderWnd::OnSize (UINT nType, int cx, int cy)
{
	CWnd::OnSize (nType, cx, cy);

	SetRect (&m_rcFrame, 0, 0, cx, cy);
	SetRect (&m_rcInteraction, 2, 2, cx - 3, cy - 3);
	CopyRect (&m_rcPanel, &m_rcInteraction);
	if (m_nCurLayout & layout_horizontal)
	{
		// horizontal slider
		if (m_nCurLayout & layout_tr_top)
		{
			m_rcPanel.top += (m_nTriangle + 1);
		}
		if (m_nCurLayout & layout_tr_bottom)
		{
			m_rcPanel.bottom -= (m_nTriangle + 1);
		}
		InflateRect (&m_rcPanel, - (m_nTriangle - 3), 0);
	}
	else // if (m_nCurLayout & layout_vertical)
	{
		// vertical slider
		if (m_nCurLayout & layout_tr_left)
		{
			m_rcPanel.left += (m_nTriangle + 1);
		}
		if (m_nCurLayout & layout_tr_right)
		{
			m_rcPanel.right -= (m_nTriangle + 1);
		}
		InflateRect (&m_rcPanel, 0, - (m_nTriangle - 3));
	}

	CopyRect (&m_rcPaint, &m_rcPanel);
	InflateRect (&m_rcPaint, -2, -2);

	if (m_nCurLayout & layout_horizontal)
	{
		m_nBuffSize = m_rcPaint.right - m_rcPaint.left + 1;
	}
	else
	{
		m_nBuffSize = m_rcPaint.bottom - m_rcPaint.top + 1;
	}
	if (row_buffer)
	{
		delete [] row_buffer;
	}
	row_buffer = new DWORD [m_nBuffSize];
	if (row_buffer == NULL)
	{
		return;
	}

	// create cache bitmap
	bmp_info.bmiHeader.biSize = sizeof (BITMAPINFO);
	bmp_info.bmiHeader.biWidth = cx;
	bmp_info.bmiHeader.biHeight = cy;
	bmp_info.bmiHeader.biPlanes = 1;
	bmp_info.bmiHeader.biBitCount = 32;
	bmp_info.bmiHeader.biCompression = BI_RGB;
	bmp_info.bmiHeader.biSizeImage = cx * cy * 32 / 8;
	bmp_info.bmiHeader.biXPelsPerMeter =
	bmp_info.bmiHeader.biYPelsPerMeter = 72 * 2 * 1000;
	bmp_info.bmiHeader.biClrUsed = 0;
	bmp_info.bmiHeader.biClrImportant = 0;
	bmp_data = NULL;
	bmp_handle = CreateDIBSection (NULL, &bmp_info, DIB_RGB_COLORS, (void**) &bmp_data, NULL, 0);
	if (bmp_handle == NULL)
	{
		return;
	}
	return;

}

LRESULT GTColorSliderWnd::OnSelectBulletOK (WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;
	// We receive this on a color change from one of the well buttons
	// or one of the embedded color wells.
	GTColorSelectorWnd *picker = (GTColorSelectorWnd*)pNotifyWnd;
	switch (m_nCurMode & modes_mask)
	{
		case	modes_rgb_red:
		case	modes_rgb_green:
		case	modes_rgb_blue:
			SetControlValue (0, picker->y_pos);
			SetControlValue (1, picker->x_pos);
			SetPosition (picker->GetControlValue());
			break;

		case	modes_hsv_hue:
		case	modes_hsv_sat:
		case	modes_hsv_value:
			SetControlValue (0, picker->x_pos);
			SetControlValue (1, picker->y_pos);
			SetPosition (picker->GetControlValue());
			break;
	}
	return 0L;
}

void GTColorSliderWnd::OnSysColorChange()
{
	//	gfxData.OnSysColorChange();
	Invalidate();
}

UINT GTColorSliderWnd::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default
	ASSERT_VALID (this);

	UINT nDlgCode = DLGC_WANTARROWS | DLGC_WANTCHARS;
	nDlgCode |= DLGC_WANTTAB;

	return nDlgCode;
}

void GTColorSliderWnd::SetLayoutValue (IN int value)
{
	m_nCurLayout = value;
	Invalidate (FALSE);
}

void GTColorSliderWnd::SetDataValue (IN int value)
{
	if ((value & modes_mask) < 0 ||
		(value & modes_mask) > modes_max)
		// invalid mode
	{
		return;
	}

	m_nCurMode = value;
	switch (m_nCurMode & modes_mask)
	{
		case	modes_rgb_red:
		case	modes_rgb_green:
		case	modes_rgb_blue:
			m_nStartValue = 0;
			m_nEndValue = max_rgb;
			m_nSteps = scale_rgb;
			m_nPageSizes = scale_rgb * 10;
			break;

		case	modes_hsv_hue:
			m_nStartValue = 0;
			m_nEndValue = max_hsv_hue;
			m_nSteps = scale_hsv_hue;
			m_nPageSizes = scale_hsv_hue * 10;
			break;

		case	modes_hsv_sat:
			m_nStartValue = 0;
			m_nEndValue = max_hsv_sat;
			m_nSteps = scale_hsv_sat / 100;
			m_nPageSizes = scale_hsv_sat / 10;
			break;

		case	modes_hsv_value:
			m_nStartValue = 0;
			m_nEndValue = max_hsv_value;
			m_nSteps = scale_hsv_value / 100;
			m_nPageSizes = scale_hsv_value / 10;
			break;
	}

	// position is not preserved
	m_nPreviousPos = -9999;
	m_nCurPosition = (m_nCurMode & modes_reverse ? m_nEndValue : m_nStartValue);

	// reinitialize components
	additional_components[0] =
		additional_components[1] =
			additional_components[2] =
				additional_components[3] = 0;

	Invalidate (FALSE);
}

void GTColorSliderWnd::SetStartValue (int value)
{
	if (m_nStartValue == value)
	{
		return;
	}
	m_nStartValue = value;
	if (m_nStartValue < m_nEndValue)
	{
		if (m_nCurPosition < m_nStartValue)
		{
			m_nCurPosition = m_nStartValue;
		}
		if (m_nCurPosition > m_nEndValue)
		{
			m_nCurPosition = m_nEndValue;
		}
	}
	else
	{
		if (m_nCurPosition < m_nEndValue)
		{
			m_nCurPosition = m_nEndValue;
		}
		if (m_nCurPosition > m_nStartValue)
		{
			m_nCurPosition = m_nStartValue;
		}
	}
	Invalidate (FALSE);
}

void GTColorSliderWnd::SetEndValue (int value)
{
	if (m_nEndValue == value)
	{
		return;
	}
	m_nEndValue = value;
	if (m_nStartValue < m_nEndValue)
	{
		if (m_nCurPosition < m_nStartValue)
		{
			m_nCurPosition = m_nStartValue;
		}
		if (m_nCurPosition > m_nEndValue)
		{
			m_nCurPosition = m_nEndValue;
		}
	}
	else
	{
		if (m_nCurPosition < m_nEndValue)
		{
			m_nCurPosition = m_nEndValue;
		}
		if (m_nCurPosition > m_nStartValue)
		{
			m_nCurPosition = m_nStartValue;
		}
	}
	Invalidate (FALSE);
}

void GTColorSliderWnd::SetPosition (int value)
{
	if (m_nCurMode & modes_reverse)
		// invert if neccessary
	{
		value = m_nEndValue - value;
	}
	if (m_nCurPosition == value)
	{
		return;
	}
	m_nCurPosition = value;
	if (m_nStartValue < m_nEndValue)
	{
		if (m_nCurPosition < m_nStartValue)
		{
			m_nCurPosition = m_nStartValue;
		}
		if (m_nCurPosition > m_nEndValue)
		{
			m_nCurPosition = m_nEndValue;
		}
	}
	else
	{
		if (m_nCurPosition < m_nEndValue)
		{
			m_nCurPosition = m_nEndValue;
		}
		if (m_nCurPosition > m_nStartValue)
		{
			m_nCurPosition = m_nStartValue;
		}
	}
	Invalidate (FALSE);
}

void GTColorSliderWnd::SetControlValue (int index, int value)
{
	if (index < 0 || index > 1)
		// invalid index
	{
		return;
	}
	if (additional_components[index] == value)
	{
		return;
	}
	if (value < 0 || value > max_slider[m_nCurMode & modes_mask][index + 1])
		// value out of range
	{
		return;
	}
	additional_components[index] = value;
	Invalidate (FALSE);
}


void GTColorSliderWnd::OnDraw (void)
{
	int	i;

	DWORD	*prgb;	// pointer to first pixel from the blend
	int		nSkip;	// number of pixels to skip after current row and before next one

	DWORD	*p;		// loop pointers

	int		width = m_rcFrame.right - m_rcFrame.left;
	int		height = m_rcFrame.bottom - m_rcFrame.top;
	int		blend_width = m_rcPaint.right - m_rcPaint.left + 1;
	int		blend_height = m_rcPaint.bottom - m_rcPaint.top + 1;

	//
	//	horizontal blend schema:
	//
	//	+--+--+--+--+--+--+
	//	|**|  |**|  |**|  | - selection m_rcFrame
	//	+--+--+--+--+--+--+
	//	|  |  |  |  |  |  | - blank
	//	+--+--+--+--+--+--+
	//	................... - triangle(triangle-size blend_height)
	//	+--+--+--+--+--+--+
	//	|  |  |  |  |  |  | - blank
	//	+--+--+--+--+--+--+
	//	|**|  |xx|xx|xx|xx| - relief m_rcFrame
	//	+--+--+--+--+--+--+
	//	|  |  |xx|XX|XX|XX| - black m_rcFrame
	//	+--+--+--+--+--+--+
	//	|**|  |xx|XX| X+--+-- blend's 1st row(prgb pointer)
	//	+--+--+--+--+--+--+
	//

	// fill row buffer
	switch (m_nCurMode & modes_mask)
	{
		case modes_rgb_red:
		case modes_rgb_green:
		case modes_rgb_blue:
			DoDrawRGB();
			break;
		case modes_hsv_hue:
			DoDrawHSV_Hue();
			break;
		case modes_hsv_sat:
			DoDrawHSV_Sat();
			break;
		case modes_hsv_value:
			DoDrawHSV_Val();
			break;
		default:
			// mode not supported?
			ASSERT (FALSE);
			return;
	}

	// reverse if necessary
	//	when slider is horizontal, x = 0 is at the left
	//	when slider is vertical, y = 0 is at the bottom
	if ((m_nCurLayout & layout_vertical) && ! (m_nCurMode & modes_reverse) ||
		(m_nCurLayout & layout_horizontal) && (m_nCurMode & modes_reverse))
	{
		GTDrawHelper m_Panel;
		m_Panel.reverse (row_buffer, m_nBuffSize);
	}

	// prepare
	prgb = bmp_data +
		   (height - m_rcPaint.bottom - 1) * width + // top rows
		   m_rcPaint.left; // pixels on first row
	nSkip = m_rcPaint.left +
			width - m_rcPaint.right - 1;
	p = prgb;

	if (m_nCurLayout & layout_horizontal)
	{
		i = blend_height;
		while (i--)
		{
			GTDrawHelper m_Panel;
			m_Panel.copy (&p, row_buffer, blend_width), p += nSkip;
		}
	}
	else // if (m_nCurLayout & layout_vertical)
	{
		DWORD	*source = row_buffer;

		i = blend_height;
		while (i--)
		{
			GTDrawHelper m_Panel;
			m_Panel.set (&p, *source++, blend_width), p += nSkip;
		}
	}
}


void GTColorSliderWnd::DoDrawRGB (void)
{
	COLORREF	start_color, end_color;
	BYTE	c1, c2;

	c1 = (BYTE) ((int) additional_components[0] * 255 / max_rgb);
	c2 = (BYTE) ((int) additional_components[1] * 255 / max_rgb);

	// setup start and end color depending of current mode
	switch (m_nCurMode & modes_mask)
	{
		case	modes_rgb_red:
			start_color = RGB (0, c1, c2);
			end_color = RGB (255, c1, c2);
			break;

		case	modes_rgb_green:
			start_color = RGB (c1, 0, c2);
			end_color = RGB (c1, 255, c2);
			break;

		case	modes_rgb_blue:
			start_color = RGB (c1, c2, 0);
			end_color = RGB (c1, c2, 255);
			break;

		default:
			ASSERT (FALSE);
			return;
	}

	GTDrawHelper m_Panel;
	m_Panel.GetRGB (row_buffer, m_nBuffSize, start_color, end_color);
}


void GTColorSliderWnd::DoDrawHSV_Hue (void)
{
	GTDrawHelper m_Panel;
	m_Panel.HSV_HUE (row_buffer, m_nBuffSize,
					 additional_components[0] / (double) scale_hsv_sat,
					 additional_components[1] / (double) scale_hsv_value);
}


void GTColorSliderWnd::DoDrawHSV_Sat (void)
{
	GTDrawHelper m_Panel;
	m_Panel.HSV_SAT (row_buffer, m_nBuffSize,
					 additional_components[0] / (double) scale_hsv_hue,
					 additional_components[1] / (double) scale_hsv_value);
}


void GTColorSliderWnd::DoDrawHSV_Val (void)
{
	GTDrawHelper m_Panel;
	m_Panel.HSV_VAL (row_buffer, m_nBuffSize,
					 additional_components[0] / (double) scale_hsv_hue,
					 additional_components[1] / (double) scale_hsv_sat);
}

BOOL GTColorSliderWnd::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::OnNotify (wParam, lParam, pResult);
}

