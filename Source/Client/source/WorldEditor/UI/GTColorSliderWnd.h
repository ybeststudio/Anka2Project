#if !defined(AFX_GTCOLORSLIDERWND_H__F818AFD3_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
#define AFX_GTCOLORSLIDERWND_H__F818AFD3_4DE0_11D6_A56D_525400EA266C__INCLUDED_

#pragma once

// GTColorSliderWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// GTColorSliderWnd window

#include "GTDrawHelper.h"

// slider visual modes against max, max_component_1 and max_component_2
const int max_slider[6][3] =
{
	{ max_rgb_red, max_rgb_green, max_rgb_blue },	// max_rgb red: R, G, B
	{ max_rgb_green, max_rgb_red, max_rgb_blue },	// max_rgb green: G, R, B
	{ max_rgb_blue, max_rgb_red, max_rgb_green },	// max_rgb blue: B, R, G
	{ max_hsv_hue, max_hsv_sat, max_hsv_value },	// max_hsv hue: H, S, V
	{ max_hsv_sat, max_hsv_hue, max_hsv_value },	// max_hsv sat: S, H, V
	{ max_hsv_value, max_hsv_hue, max_hsv_sat }		// max_hsv value: V, H, S
};

class GTColorSliderWnd : public CWnd
{
	public:
		// Constroctor
		GTColorSliderWnd();

		// Attributes
	public:
		//Create Arrow Type Window
		BOOL Create (DWORD dwStyle,
					 CRect rcPos,
					 CWnd* pParent,
					 UINT nID,
					 int nBulletType,
					 BOOL bPopup = FALSE);

		// get/set layout
		inline int GetLayoutValue (void) const
		{
			return (m_nCurLayout);
		};
		void SetLayoutValue (int value);

		// get/set visual mode
		int GetDataValue (void) const
		{
			return (m_nCurMode & modes_mask);
		};
		void SetDataValue (int value);

		// get/set value at the begining of the slider
		inline int GetStartValue (void) const
		{
			return (m_nStartValue);
		};
		void SetStartValue (int value);

		// get/set value at the end of the slider
		inline int GetEndValue (void) const
		{
			return (m_nEndValue);
		};
		void SetEndValue (IN int value);

		// get/set position of the slider
		inline int GetPosition (void) const
		{
			return (m_nCurMode & modes_reverse ? m_nEndValue - m_nCurPosition : m_nCurPosition);
		};
		void SetPosition (IN int value);

		// get/set one-of slider additional components
		inline int GetControlValue (int index) const
		{
			return (additional_components[index]);
		};
		void SetControlValue (int index, int value);

		// painting methods
		void OnDraw (void);

		// draws blend in rgb mode - includes red, green and blue modes
		void DoDrawRGB (void);

		// draws blend in hsv mode - hue, sat and value modes separately
		void DoDrawHSV_Hue (void);
		void DoDrawHSV_Sat (void);
		void DoDrawHSV_Val (void);

		// parent notification methods
		virtual LRESULT	SendMessageToParent (IN UINT code);

	public:
		// Operations
		enum layouts
		{
			layout_horizontal = 0x8000,	// horizontal slider
			layout_tr_top = 0x2000,		// triangles
			layout_tr_bottom = 0x1000,

			layout_vertical = 0x4000,	// vertical slider
			layout_tr_left = 0x2000,	// triangles
			layout_tr_right = 0x1000,
		};

	protected:
		int		m_nCurMode;	// visual mode (colorspace)
		int		m_nCurLayout;

		int		m_nStartValue;
		int		m_nEndValue;
		int		m_nCurPosition;
		int		m_nPreviousPos;
		int		m_nSteps;
		int		m_nPageSizes;

		CRect	m_rcPanel;
		CRect	m_rcPaint;
		int		m_nTriangle;

		// additional components depends on current visual mode (indexes given below):
		// - rgb red - 0 is green in [0, 255], 1 is blue in [0, 255]
		// - rgb green - 0 is red in [0, 255], 1 is blue in [0, 255]
		// - rgb blue - 0 is red in [0, 255], 1 is green in [0, 255]
		// - hsv hue - 0 is sat in [0, 1000 (1000 is 100.0% or 1.0)], 1 is value in [0, 1000]
		// - hsv sat - 0 is hue in [0, 3599 (3599 is 359.9 degs)], 1 is value in [0, 1000]
		// - hsv value - 0 is hue in [0, 3599], 1 is sat in [0, 1000]
		int		additional_components[4];

		DWORD*	row_buffer;			// 1px-high 32-bit bitmap, having same width as blend
		int		m_nBuffSize;			// size of the row buffer in doublewords

		// cache DIB bitmap information
		BITMAPINFO	bmp_info;
		HBITMAP		bmp_handle;
		DWORD*		bmp_data;

		CRect	m_rcFrame;				// control's m_rcFrame (bounding box)
		CRect	m_rcInteraction;	// area, where mouse-drags will be traced (client area)
		bool	m_bTracking;			// whether mouse is being tracked (drag)

	public:
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(GTColorSliderWnd)
	protected:
		virtual BOOL PreCreateWindow (CREATESTRUCT& cs);
		virtual void PreSubclassWindow();
		virtual BOOL OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		//}}AFX_VIRTUAL

		// Implementation
	public:
		// Destructor
		virtual ~GTColorSliderWnd();

		// Generated message map functions
	protected:
		//{{AFX_MSG(GTColorSliderWnd)

		//message map function On Erase Background
		afx_msg BOOL OnEraseBkgnd (CDC* pDC);

		//message map function On left button double clicked
		afx_msg void OnLButtonDblClk (UINT nFlags, CPoint point);

		//message map function On left button down
		afx_msg void OnLButtonDown (UINT nFlags, CPoint point);

		//message map function On left button up
		afx_msg void OnLButtonUp (UINT nFlags, CPoint point);

		//message map function On middle button double clicked
		afx_msg void OnMButtonDblClk (UINT nFlags, CPoint point);

		//message map function On middle button down
		afx_msg void OnMButtonDown (UINT nFlags, CPoint point);

		//message map function On middle button up
		afx_msg void OnMButtonUp (UINT nFlags, CPoint point);

		//message map function On Mouse Move
		afx_msg void OnMouseMove (UINT nFlags, CPoint point);

		// message map function On keystroke  nonsystem character
		afx_msg void OnChar (UINT nChar, UINT nRepCnt, UINT nFlags);

		//message map function On window Destroyed
		afx_msg void OnDestroy();

		//message map function On window painted
		afx_msg void OnPaint();

		//message map function On right button down
		afx_msg void OnRButtonDown (UINT nFlags, CPoint point);

		//message map function On right button up
		afx_msg void OnRButtonUp (UINT nFlags, CPoint point);

		//message map function On Cancel Mode
		afx_msg void OnCancelMode();

		//message map function On Key Down
		afx_msg void OnKeyDown (UINT nChar, UINT nRepCnt, UINT nFlags);
		//message map function On window size changed
		afx_msg void OnSize (UINT nType, int cx, int cy);
		// System color change.
		afx_msg void OnSysColorChange();
		afx_msg UINT OnGetDlgCode();
		//}}AFX_MSG

		//message map function On Select Day OK
		afx_msg LRESULT OnSelectBulletOK (WPARAM wParam, LPARAM lParam);
		DECLARE_MESSAGE_MAP()

	public:
		// The pointer to Notify Window
		CWnd* pNotifyWnd;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GTCOLORSLIDERWND_H__F818AFD3_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
