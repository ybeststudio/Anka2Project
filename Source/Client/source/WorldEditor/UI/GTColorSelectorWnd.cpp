// GTColorSelectorWnd.cpp : implementation file
//

#include "StdAfx.h"
#include "GTColorSelectorWnd.h"
#include "GTColorSliderWnd.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// GTColorSelectorWnd

GTColorSelectorWnd::GTColorSelectorWnd()
{
	pNotifyWnd = NULL;
	pNotifyWellWnd = NULL;
	m_nCurMode = modes_hsv_hue;
	additional_component = -1;
	x_pos = -1;
	y_pos = -1;
	max_x = -1;
	max_y = -1;
	old_x = -1;
	old_y = -1;
	m_bNeedPaint = true;
	bmp_handle = NULL;
	m_bTracking = false;
	picker_cursor = LoadCursor (NULL, IDC_CROSS);
	m_crSaveColor = RGB (0, 0, 0);
}

GTColorSelectorWnd::~GTColorSelectorWnd()
{
	if (bmp_handle)
	{
		DeleteObject (bmp_handle);
	}
}


BEGIN_MESSAGE_MAP (GTColorSelectorWnd, CWnd)
	//{{AFX_MSG_MAP(GTColorSelectorWnd)
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
// GTColorSelectorWnd message handlers
BOOL GTColorSelectorWnd::Create (DWORD dwStyle, CRect rcPos, CWnd* pParent, UINT nID, int nBulletType, BOOL bPopup)
{
	nBulletType;
	bPopup;
	LPVOID lp = (LPVOID)NULL;
	if (!CreateEx (0,
				   AfxRegisterWndClass (CS_DBLCLKS, ::LoadCursor (NULL, IDC_ARROW)),
				   _T ("GTColorSelectorWnd"),
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
		TRACE0 ("Failed to create GTColorSelectorWnd\n");
		return FALSE;
	}
	pNotifyWnd = NULL;
	pNotifyWellWnd = NULL;
	SetDataValue (modes_rgb_red);
	UpdateWindow();
	return TRUE;
}

BOOL GTColorSelectorWnd::OnEraseBkgnd (CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	pDC;
	return TRUE;
}

void GTColorSelectorWnd::OnLButtonDblClk (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	//	m_pBulletControl->OnLButtonDblClk(nFlags,point);
	CWnd::OnLButtonDblClk (nFlags, point);
}

void GTColorSelectorWnd::OnLButtonDown (UINT nFlags, CPoint point)
{
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

void GTColorSelectorWnd::OnLButtonUp (UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		ReleaseCapture();
		m_bTracking = false;
		// notify parent window
		COLORREF crColor = GetColor();
		m_crSaveColor = crColor;
		SendMessageToParent (crColor);
	}
	CWnd::OnLButtonUp (nFlags, point);
}

void GTColorSelectorWnd::OnMButtonDblClk (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonDblClk (nFlags, point);
}

void GTColorSelectorWnd::SetDataValue (IN int value)
{
	if (m_nCurMode == value)
	{
		return;
	}

	if ((value & modes_mask) < 0 ||
		(value & modes_mask) > modes_max)
		// invalid mode
	{
		return;
	}

	//	debug_state("mode_set = %d", value);

	// current position is not preserved
	x_pos = 0;
	y_pos = 0;
	additional_component = 0;
	m_nCurMode = value & modes_mask;

	// blend should be drawn, because visual mode have changed
	m_bNeedPaint = true;

	max_x = max_picker[m_nCurMode][1];
	max_y = max_picker[m_nCurMode][2];
	Invalidate (FALSE);
}

void GTColorSelectorWnd::OnMButtonDown (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	POINT	pt = point;

	OnSetFocus (this);
	if (PtInRect (&m_rcInteraction, pt))
	{
		::SetCapture (m_hWnd);
		m_bTracking = true;
		OnMouseMove (nFlags, point);
	}
	CWnd::OnMButtonDown (nFlags, point);
}

void GTColorSelectorWnd::OnMButtonUp (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	SetFocus();
	CWnd::OnMButtonUp (nFlags, point);
}

void GTColorSelectorWnd::OnMouseMove (UINT nFlags, CPoint point)
{
	nFlags;
	// TODO: Add your message handler code here and/or call default
	if (m_bTracking)
	{
		SetCursor (picker_cursor);

		// make sure x and y are inside m_rcPaint
		in_range (point.x, m_rcPaint.left, m_rcPaint.right);
		in_range (point.y, m_rcPaint.top, m_rcPaint.bottom);

		// scale x and y in [0, max_x] and [0, max_y];
		//	use ceil, otherwise there is a difference between cursor position
		//	and selection circle drawn
		point.x = (short) ceil ((point.x - m_rcPaint.left) * (double) max_x / (m_rcPaint.right - m_rcPaint.left));
		point.y = (short) ceil ((m_rcPaint.bottom - point.y) * (double) max_y / (m_rcPaint.bottom - m_rcPaint.top));
		if (point.x == x_pos && point.y == y_pos)
			// don't redraw if not necessary
		{
			return;
		}

		x_pos = point.x;
		y_pos = point.y;
		InvalidateRect (NULL, FALSE);
		// color has changed
		COLORREF crColor = GetColor();
		m_crSaveColor = crColor;
		SendMessageToParent (crColor);
	}
	else
	{
		POINT	pt = { point.x, point.y };
		if (PtInRect (&m_rcInteraction, pt))
		{
			SetCursor (picker_cursor);
		}
		else
		{
			SetCursor (LoadCursor (NULL, IDC_ARROW));
		}
	}
}

void GTColorSelectorWnd::OnChar (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	//	m_pBulletControl->OnChar(nChar,nRepCnt,nFlags);
	CWnd::OnChar (nChar, nRepCnt, nFlags);
}

void GTColorSelectorWnd::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: Add your message handler code here

}

void GTColorSelectorWnd::OnPaint()
{
	CPaintDC dcPaint (this); // device context for painting
	HDC	hdc;

	// create compatible DC
	hdc = CreateCompatibleDC (dcPaint.m_hDC);

	if (hdc != NULL)
	{
		HBRUSH	bg_fill;

		SelectObject (hdc, bmp_handle);

		if (m_bNeedPaint || CONTRAST_MARKER)
		{
			old_x = -9999;
			old_y = -9999;

			bg_fill = CreateSolidBrush (GetSysColor (COLOR_BTNFACE));
			SelectObject (hdc, GetStockObject (NULL_PEN));
			SelectObject (hdc, bg_fill);
			Rectangle (hdc, 0, 0, m_rcFrame.right + 1, m_rcFrame.bottom + 1);
			SelectObject (hdc, GetStockObject (WHITE_BRUSH));
			DeleteObject (bg_fill);

			CRect rcTemp = m_rcFrame;
			rcTemp.DeflateRect (1, 1, 1, 1);
			GTDrawHelper m_Panel;
			m_Panel.DrawPanel (hdc, &rcTemp, GTDrawHelper::sunken | GTDrawHelper::blackbox);

			GdiFlush();
			OnDraw();
		}
		else
		{
			HPEN	pen_m_rcFrame = CreatePen (PS_SOLID, 1, GetSysColor (COLOR_BTNFACE));

			SelectObject (hdc, pen_m_rcFrame);
			SelectObject (hdc, GetStockObject (NULL_BRUSH));
			Rectangle (hdc, m_rcFrame.left, m_rcFrame.top, m_rcFrame.right, m_rcFrame.bottom);
			SelectObject (hdc, GetStockObject (BLACK_PEN));
			DeleteObject (pen_m_rcFrame);
		}

		// current position
		if (CONTRAST_MARKER)
		{
			int	x, y, size;
			COLORREF	c;
			int	val;
			HPEN	marker, old;

			// arc size
			size = MARKER_SIZE;

			IntersectClipRect (hdc, m_rcPaint.left, m_rcPaint.top,
							   m_rcPaint.right + 1, m_rcPaint.bottom + 1);

			// current position
			x = x_pos * (m_rcPaint.right - m_rcPaint.left) / max_x + m_rcPaint.left;
			y = m_rcPaint.bottom - y_pos * (m_rcPaint.bottom - m_rcPaint.top) / max_y;

			c = GetColor();
			val = max (max (GetRValue (c), GetGValue (c)), GetBValue (c));
			if (val < 128)
			{
				marker = CreatePen (PS_SOLID, 1, RGB (255, 255, 255));
			}
			else
			{
				marker = CreatePen (PS_SOLID, 1, RGB (0, 0, 0));
			}
			old = (HPEN) SelectObject (hdc, marker);
			Arc (hdc, x - size, y - size, x + size + 1, y + size + 1,
				 x, y - size, x, y - size);
			SelectObject (hdc, old);
			DeleteObject (marker);
		}
		else
		{
			int	x, y, size;

			// arc size
			size = MARKER_SIZE;

			SelectObject (hdc, GetStockObject (WHITE_PEN));
			IntersectClipRect (hdc, m_rcPaint.left, m_rcPaint.top,
							   m_rcPaint.right + 1, m_rcPaint.bottom + 1);
			SetROP2 (hdc, R2_XORPEN);

			// current position
			x = x_pos * (m_rcPaint.right - m_rcPaint.left) / max_x + m_rcPaint.left;
			y = m_rcPaint.bottom - y_pos * (m_rcPaint.bottom - m_rcPaint.top) / max_y;
			Arc (hdc, x - size, y - size, x + size + 1, y + size + 1,
				 x, y - size, x, y - size);

			// old position
			x = old_x * (m_rcPaint.right - m_rcPaint.left) / max_x + m_rcPaint.left;
			y = m_rcPaint.bottom - old_y * (m_rcPaint.bottom - m_rcPaint.top) / max_y;
			Arc (hdc, x - size, y - size, x + size + 1, y + size + 1,
				 x, y - size, x, y - size);

			old_x = x_pos;
			old_y = y_pos;

			// restore raster ops mode
			SetROP2 (hdc, R2_COPYPEN);
		}

		// bitblt and delete cache DC
		GdiFlush();

		BitBlt (dcPaint.m_hDC, 0, 0, m_rcFrame.right, m_rcFrame.bottom, hdc, 0, 0, SRCCOPY);
		DeleteDC (hdc);

		// blend is cached in the bitmap until next mode change or component change
		m_bNeedPaint = false;
	}
}

void GTColorSelectorWnd::OnRButtonDown (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	//	m_pBulletControl->OnRButtonDown(nFlags,point);
	CWnd::OnRButtonDown (nFlags, point);
}

void GTColorSelectorWnd::OnRButtonUp (UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CWnd::OnRButtonUp (nFlags, point);
}

void GTColorSelectorWnd::OnCancelMode()
{
	CWnd::OnCancelMode();
	//	m_pBulletControl->OnCancelMode();
}

void GTColorSelectorWnd::OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags)
{
	nRepCnt;
	nFlags;
	int	x, y;

	// don't process if mouse is being tracked
	if (m_bTracking)
	{
		return;
	}

	x = x_pos;
	y = y_pos;
	switch (nChar)
	{
		case	VK_UP:
			y++;
			break;
		case	VK_LEFT:
			x--;
			break;
		case	VK_DOWN:
			y--;
			break;
		case	VK_RIGHT:
			x++;
			break;
		case	VK_PRIOR:
			x = max_x;
			y = max_y;
			break;
		case	VK_NEXT:
			x = max_x;
			y = 0;
			break;
		case	VK_HOME:
			x = 0;
			y = max_y;
			break;
		case	VK_END:
			x = 0;
			y = 0;
			break;
		default:
			return;
	}

	in_range (x, 0, max_x);
	in_range (y, 0, max_y);
	if (x == x_pos && y == y_pos)
		// don't redraw if not necessary
	{
		return;
	}

	x_pos = x;
	y_pos = y;
	InvalidateRect (NULL, FALSE);
	// color has changed
	COLORREF crColor = GetColor();
	m_crSaveColor = crColor;
	SendMessageToParent (crColor);
}

BOOL GTColorSelectorWnd::PreCreateWindow (CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::PreCreateWindow (cs);
}

void GTColorSelectorWnd::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	CWnd::PreSubclassWindow();
}

void GTColorSelectorWnd::OnSize (UINT nType, int cx, int cy)
{
	CWnd::OnSize (nType, cx, cy);

	SetRect (&m_rcFrame, 0, 0, cx, cy);
	SetRect (&m_rcInteraction, 1, 1, cx - 1, cy - 3);

	SetRect (&m_rcPanel, 1, 1, cx - 1, cy - 1);
	InflateRect (&m_rcPanel,
				 - 1,
				 - 1);

	CopyRect (&m_rcPaint, &m_rcPanel);
	InflateRect (&m_rcPaint, -1, -1);
	CopyRect (&m_rcInteraction, &m_rcPaint);


	// delete cache bitmap if window is resized
	if (bmp_handle != NULL)
	{
		DeleteObject (bmp_handle);
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

	// blend should be drawn, because control size has changed
	m_bNeedPaint = true;
}

LRESULT GTColorSelectorWnd::OnSelectBulletOK (WPARAM wParam, LPARAM lParam)
{
	wParam;
	lParam;
	GTColorSliderWnd *slider = (GTColorSliderWnd*)pNotifyWnd;
	SetControlValue (slider->GetPosition());
	return 0L;
}

void GTColorSelectorWnd::OnSysColorChange()
{
	//	gfxData.OnSysColorChange();
	Invalidate (FALSE);
}

void GTColorSelectorWnd::SetXPos (int value)
{
	if (x_pos == value)
	{
		return;
	}
	x_pos = value;
	in_range (x_pos, 0, max_x);
	Invalidate (FALSE);
}

void GTColorSelectorWnd::SetYPos (int value)
{
	if (y_pos == value)
	{
		return;
	}
	y_pos = value;
	in_range (y_pos, 0, max_y);
	Invalidate (FALSE);
}

UINT GTColorSelectorWnd::OnGetDlgCode()
{
	// TODO: Add your message handler code here and/or call default
	ASSERT_VALID (this);

	UINT nDlgCode = DLGC_WANTARROWS | DLGC_WANTCHARS;
	nDlgCode |= DLGC_WANTTAB;

	return nDlgCode;
}

void GTColorSelectorWnd::OnDraw (void)
{
	switch (m_nCurMode)
	{
		case	modes_rgb_red:
		case	modes_rgb_green:
		case	modes_rgb_blue:
			DoDrawRGB();
			break;
		case	modes_hsv_hue:
			DoDrawHSV_Hue();
			break;
		case	modes_hsv_sat:
			DoDrawHSV_Sat();
			break;
		case	modes_hsv_value:
			DoDrawHSV_Val();
			break;
	}
}


void GTColorSelectorWnd::DoCalculateColors (COLORREF &lefttop, COLORREF &righttop, COLORREF &leftbottom, COLORREF &rightbottom)
{
	BYTE	c = (BYTE) ((int) additional_component * 255 / max_rgb);

	switch (m_nCurMode)
	{
		case	modes_rgb_red:
			lefttop = RGB (c, 255, 0);
			righttop = RGB (c, 255, 255);
			leftbottom = RGB (c, 0, 0);
			rightbottom = RGB (c, 0, 255);
			break;

		case	modes_rgb_green:
			lefttop = RGB (255, c, 0);
			righttop = RGB (255, c, 255);
			leftbottom = RGB (0, c, 0);
			rightbottom = RGB (0, c, 255);
			break;

		case	modes_rgb_blue:
			lefttop = RGB (0, 255, c);
			righttop = RGB (255, 255, c);
			leftbottom = RGB (0, 0, c);
			rightbottom = RGB (255, 0, c);
			break;
	}
}


void GTColorSelectorWnd::DoDrawRGB (void)
{
	int		i;		// loop counters
	int		j;

	int		nSkip;	// number of pixels to skip after current row and before next one

	DWORD	*p;		// pointer to first pixel from the blend & a loop pointer

	int		width = m_rcFrame.right - m_rcFrame.left;
	int		height = m_rcFrame.bottom - m_rcFrame.top;
	int		blend_width = m_rcPaint.right - m_rcPaint.left + 1;
	int		blend_height = m_rcPaint.bottom - m_rcPaint.top + 1;

	// left side components, extended with int_extend bits
	int		left_red, left_green, left_blue;
	// left side component advance, extended with int_extend bits
	int		left_red_adv, left_green_adv, left_blue_adv;
	int		right_red, right_green, right_blue;
	int		right_red_adv, right_green_adv, right_blue_adv;

	// current components, extended with int_extend bits
	int		red, green, blue;
	// advance, extended with int_extend bits
	int		red_adv, green_adv, blue_adv;

	COLORREF	lefttop, righttop, leftbottom, rightbottom;

	p = bmp_data + (height - m_rcPaint.bottom - 1) * width + m_rcPaint.left;
	nSkip = m_rcPaint.left + width - m_rcPaint.right - 1;

	DoCalculateColors (lefttop, righttop, leftbottom, rightbottom);

	left_red = scaled_red (leftbottom);
	left_green = scaled_green (leftbottom);
	left_blue = scaled_blue (leftbottom);
	left_red_adv = (scaled_red (lefttop) - left_red) / (blend_height - 1);
	left_green_adv = (scaled_green (lefttop) - left_green) / (blend_height - 1);
	left_blue_adv = (scaled_blue (lefttop) - left_blue) / (blend_height - 1);

	right_red = scaled_red (rightbottom);
	right_green = scaled_green (rightbottom);
	right_blue = scaled_blue (rightbottom);
	right_red_adv = (scaled_red (righttop) - right_red) / (blend_height - 1);
	right_green_adv = (scaled_green (righttop) - right_green) / (blend_height - 1);
	right_blue_adv = (scaled_blue (righttop) - right_blue) / (blend_height - 1);

	// outer loop - rows
	i = blend_height;
	while (i--)
	{
		red = left_red;
		green = left_green;
		blue = left_blue;
		red_adv = (right_red - red) / (blend_width - 1);
		green_adv = (right_green - green) / (blend_width - 1);
		blue_adv = (right_blue - blue) / (blend_width - 1);

		// inner loop - pixels @ each row
		j = blend_width;
		while (j--)
		{
			// in DIB bitmap values are 0BGR
			*p++ = RGB (
					   blue >> int_extend,
					   green >> int_extend,
					   red >> int_extend);

			// advance to the next pixel
			red += red_adv;
			green += green_adv;
			blue += blue_adv;
		}

		// advance to the next row
		left_red += left_red_adv;
		left_green += left_green_adv;
		left_blue += left_blue_adv;
		right_red += right_red_adv;
		right_green += right_green_adv;
		right_blue += right_blue_adv;
		p += nSkip;
	}
}


void GTColorSelectorWnd::DoDrawHSV_Hue (void)
{
	int		i;		// loop counters

	DWORD	*p;		// pointer to first pixel from the blend & a loop pointer
	int		nSkip;	// number of pixels to skip after current row and before next one

	int		width = m_rcFrame.right - m_rcFrame.left;
	int		height = m_rcFrame.bottom - m_rcFrame.top;
	int		blend_width = m_rcPaint.right - m_rcPaint.left + 1;
	int		blend_height = m_rcPaint.bottom - m_rcPaint.top + 1;

	double	hue;
	double	val, val_adv;

	p = bmp_data +
		(height - m_rcPaint.bottom - 1) * width + // top rows
		m_rcPaint.left; // pixels on first row
	nSkip = m_rcPaint.left +
			width - m_rcPaint.right - 1;

	// constant
	hue = additional_component / (double) scale_hsv_hue;

	// outer loop - rows
	i = blend_height;

	// initial and change
	val = 0.0;
	val_adv = 1.0 / HSV_LOOP_STEPS (i);

	while (i--)
	{
		GTDrawHelper m_Panel;
		m_Panel.HSV_SAT (p, blend_width, hue, val);
		val += val_adv;
		p += (blend_width + nSkip);
	}
}


void GTColorSelectorWnd::DoDrawHSV_Sat (void)
{
	int		i;		// loop counters

	int		nSkip;	// number of pixels to skip after current row and before next one

	DWORD	*p;		// pointer to first pixel from the blend & a loop pointer


	int		width = m_rcFrame.right - m_rcFrame.left;
	int		height = m_rcFrame.bottom - m_rcFrame.top;
	int		blend_width = m_rcPaint.right - m_rcPaint.left + 1;
	int		blend_height = m_rcPaint.bottom - m_rcPaint.top + 1;

	double	sat;
	double	val, val_adv;

	p = bmp_data +
		(height - m_rcPaint.bottom - 1) * width + // top rows
		m_rcPaint.left; // pixels on first row
	nSkip = m_rcPaint.left +
			width - m_rcPaint.right - 1;

	// constant
	sat = additional_component / (double) scale_hsv_sat;

	// outer loop - rows
	i = blend_height;

	// initial and change
	val = 0.0;
	val_adv = 1.0 / HSV_LOOP_STEPS (i);

	while (i--)
	{
		GTDrawHelper m_Panel;
		m_Panel.HSV_HUE (p, blend_width, sat, val);
		val += val_adv;
		p += (blend_width + nSkip);
	}
}

COLORREF GTColorSelectorWnd::GetColor (void) const
{
	switch (m_nCurMode)
	{
		case	modes_rgb_red:
			return (RGB (
						additional_component * 255 / max_rgb_red,
						y_pos * 255 / max_rgb_green,
						x_pos * 255 / max_rgb_blue));

		case	modes_rgb_green:
			return (RGB (
						y_pos * 255 / max_rgb_red,
						additional_component * 255 / max_rgb_green,
						x_pos * 255 / max_rgb_blue));

		case	modes_rgb_blue:
			return (RGB (
						x_pos * 255 / max_rgb_red,
						y_pos * 255 / max_rgb_green,
						additional_component * 255 / max_rgb_blue));

		case	modes_hsv_hue:
		{
			GTDrawHelper m_Panel;
			return (m_Panel.HSV2RGB (
						additional_component / (double) scale_hsv_hue,
						x_pos / (double) max_x,
						y_pos / (double) max_y));
		}

		case	modes_hsv_sat:
		{
			GTDrawHelper m_Panel;
			return (m_Panel.HSV2RGB (
						x_pos / (double) scale_hsv_hue,
						additional_component / (double) scale_hsv_sat,
						y_pos / (double) max_y));
		}

		case	modes_hsv_value:
		{
			GTDrawHelper m_Panel;
			return (m_Panel.HSV2RGB (
						x_pos / (double) scale_hsv_hue,
						y_pos / (double) max_y,
						additional_component / (double) scale_hsv_value));
		}
	}
	return (RGB (0, 0, 0));
}


LRESULT GTColorSelectorWnd::SendMessageToParent (UINT code)
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
		::SendMessage (pNotifyWnd->m_hWnd, WM_FO_SELECTBULLETTYPEOK, control_id, (LPARAM) &message1);
	}

	if (pNotifyWellWnd != NULL)
	{
		// send message as notification to the controller window
		::SendMessage (pNotifyWellWnd->m_hWnd, WM_FO_SELECTBULLETTYPEOK, control_id, (LPARAM) &message1);
	}

	parent_window = (HWND)GetParent();
	if (parent_window != NULL)
		// send WM_NOTIFY message to the parent window
	{
		return (::SendMessage (parent_window, WM_FO_SELECTBULLETTYPEOK, control_id, (LPARAM) &message1));
	}

	return (-1);
}

void GTColorSelectorWnd::SetColor (COLORREF value)
{
	int	x, y, comp;
	BYTE	red, green, blue;
	double	hue, sat, val;

	//	debug_state("color_set = %x", value);

	red = GetRValue (value);
	green = GetGValue (value);
	blue = GetBValue (value);

	switch (m_nCurMode)
	{
		case	modes_rgb_red:
			x = blue * max_rgb_blue / 255;
			y = green * max_rgb_green / 255;
			comp = red * max_rgb_red / 255;
			break;

		case	modes_rgb_green:
			x = blue * max_rgb_blue / 255;
			y = red * max_rgb_red / 255;
			comp = green * max_rgb_green / 255;
			break;

		case	modes_rgb_blue:
			x = red * max_rgb_red / 255;
			y = green * max_rgb_green / 255;
			comp = blue * max_rgb_blue / 255;
			break;

		case	modes_hsv_hue:
		{
			GTDrawHelper m_Panel;
			m_Panel.RGB2HSV (
				red / (double) max_rgb_red,
				green / (double) max_rgb_green,
				blue / (double) max_rgb_blue,
				&hue, &sat, &val);
			x = (int) ceil (sat * scale_hsv_sat);
			y = (int) ceil (val * scale_hsv_value);
			comp = (int) ceil (hue * scale_hsv_hue);
		}
		break;

		case	modes_hsv_sat:
		{
			GTDrawHelper m_Panel;
			m_Panel.RGB2HSV (
				red / (double) max_rgb_red,
				green / (double) max_rgb_green,
				blue / (double) max_rgb_blue,
				&hue, &sat, &val);
			x = (int) ceil (hue * scale_hsv_hue);
			y = (int) ceil (val * scale_hsv_value);
			comp = (int) ceil (sat * scale_hsv_sat);
		}
		break;

		case	modes_hsv_value:
		{
			GTDrawHelper m_Panel;
			m_Panel.RGB2HSV (
				red / (double) max_rgb_red,
				green / (double) max_rgb_green,
				blue / (double) max_rgb_blue,
				&hue, &sat, &val);
			x = (int) ceil (hue * scale_hsv_hue);
			y = (int) ceil (sat * scale_hsv_sat);
			comp = (int) ceil (val * scale_hsv_value);
		}
		break;

		default:
			x = 0;
			y = 0;
			comp = 0;
	}

	if (x_pos == x && y_pos == y && additional_component == comp)
	{
		return;
	}

	ASSERT (x >= 0 && x <= max_x);
	ASSERT (y >= 0 && y <= max_x);
	ASSERT (comp >= 0 && comp <= max_picker[m_nCurMode][0]);

	// set redraw blend flag if component has changed
	m_bNeedPaint |= (additional_component != comp);
	x_pos = x;
	y_pos = y;
	additional_component = comp;
	COLORREF crColor = GetColor();
	m_crSaveColor = crColor;
	SendMessageToParent (crColor);
	//repaint();
	Invalidate (FALSE);
}

void GTColorSelectorWnd::SetControlValue (int value)
{
	if (value < 0 || value > max_picker[m_nCurMode][0])
		// new value is not in range
	{
		return;
	}

	additional_component = value;

	m_bNeedPaint = true;
	COLORREF crColor = GetColor();
	m_crSaveColor = crColor;
	SendMessageToParent (crColor);
	Invalidate (FALSE);
}

void GTColorSelectorWnd::DoDrawHSV_Val (void)
{
	int		i;		// loop counters
	int		nSkip;	// number of pixels to skip after current row and before next one
	DWORD	*p;		// pointer to first pixel from the blend & a loop pointer

	int		width = m_rcFrame.right - m_rcFrame.left;
	int		height = m_rcFrame.bottom - m_rcFrame.top;
	int		blend_width = m_rcPaint.right - m_rcPaint.left + 1;
	int		blend_height = m_rcPaint.bottom - m_rcPaint.top + 1;

	double	sat;
	double	sat_adv;
	double	val;

	p = bmp_data +
		(height - m_rcPaint.bottom - 1) * width + // top rows
		m_rcPaint.left; // pixels on first row
	nSkip = m_rcPaint.left +
			width - m_rcPaint.right - 1;

	// constant
	val = additional_component / (double) scale_hsv_value;

	// outer loop - rows
	i = blend_height;
	sat = 0.0;
	sat_adv = 1.0 / HSV_LOOP_STEPS (i);

	while (i--)
	{
		GTDrawHelper m_Panel;
		m_Panel.HSV_HUE (p, blend_width, sat, val);
		sat += sat_adv;
		p += (blend_width + nSkip);
	}
}


BOOL GTColorSelectorWnd::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	return CWnd::OnNotify (wParam, lParam, pResult);
}
