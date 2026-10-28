#if !defined(AFX_GTCOLORSELECTORWND_H__F818AFD2_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
#define AFX_GTCOLORSELECTORWND_H__F818AFD2_4DE0_11D6_A56D_525400EA266C__INCLUDED_

#pragma once

// GTColorSelectorWnd.h : header file
//

#include "GTDrawHelper.h"

/////////////////////////////////////////////////////////////////////////////
// GTColorSelectorWnd window
const int CONTRAST_MARKER = 1;
const int MARKER_SIZE = 5;

const int	max_picker[6][3] =
{
	{ max_rgb_red, max_rgb_blue, max_rgb_green },	// max_rgb red: Rx, B>, G^
	{ max_rgb_green, max_rgb_blue, max_rgb_red },	// max_rgb green: Gx, B>, R^
	{ max_rgb_blue, max_rgb_red, max_rgb_green },	// max_rgb blue: Bx, R>, G^
	{ max_hsv_hue, max_hsv_sat, max_hsv_value },	// max_hsv hue: Hx, S>, V^
	{ max_hsv_sat, max_hsv_hue, max_hsv_value },	// max_hsv sat: Sx, H>, V^
	{ max_hsv_value, max_hsv_hue, max_hsv_sat }		// max_hsv value: Vx, H>, S^
};


class GTColorSelectorWnd : public CWnd
{
	public:
		// Constroctor
		GTColorSelectorWnd();

		// Attributes
	public:
		//Create Arrow Type Window
		BOOL Create (DWORD dwStyle,
					 CRect rcPos,
					 CWnd* pParent,
					 UINT nID,
					 int nBulletType,
					 BOOL bPopup = FALSE);

		// Operations
	public:
		int		m_nCurMode;
		int		additional_component;
		int		x_pos;
		int		y_pos;
		int		max_x;
		int		max_y;
		int		old_x, old_y;

		bool	m_bNeedPaint;

		// whether mouse is being tracked (drag)
		bool	m_bTracking;

		// Save color.
		COLORREF m_crSaveColor;

		// user interaction handlers
		enum mouse_buttons
		{
			button_left,
			button_right,
			button_middle
		};

		// cache DIB bitmap information
		BITMAPINFO	bmp_info;
		HBITMAP		bmp_handle;
		DWORD*		bmp_data;

		CRect	m_rcPanel;
		CRect   m_rcPaint;
		CRect	m_rcInteraction;

		// control's m_rcFrame (bounding box)
		CRect		m_rcFrame;

	public:
		// parent notification methods
		virtual LRESULT	SendMessageToParent (UINT code);

		// get/set visual mode
		inline int GetDataValue (void) const
		{
			return (m_nCurMode);
		};
		void SetDataValue (int value);

		// get/set additional component's value
		inline int GetControlValue (void) const
		{
			return (additional_component);
		};
		void SetControlValue (int value);

		// Operations
		void DoCalculateColors (COLORREF &lefttop, COLORREF &righttop, COLORREF &leftbottom, COLORREF &rightbottom);

		// Draw all
		void OnDraw (void);

		// draws blend in rgb mode - includes red, green and blue modes
		void DoDrawRGB (void);

		// draws blend in hsv mode - hue, sat and value modes separately
		void DoDrawHSV_Hue (void);
		void DoDrawHSV_Sat (void);
		void DoDrawHSV_Val (void);

		// get/set current color
		COLORREF GetColor (void) const;
		void SetColor (COLORREF value);

		// get/set current x-axis position
		inline int GetXPos (void) const
		{
			return (x_pos);
		};
		void SetXPos (int value);

		// get/set current y-axis position
		inline int GetYPos (void) const
		{
			return (y_pos);
		};
		void SetYPos (int value);

	protected:
		HCURSOR	picker_cursor;

	protected:
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(GTColorSelectorWnd)
	protected:
		virtual BOOL PreCreateWindow (CREATESTRUCT& cs);
		virtual void PreSubclassWindow();
		virtual BOOL OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		//}}AFX_VIRTUAL

		// Implementation
	public:
		// Destructor
		virtual ~GTColorSelectorWnd();

		// Generated message map functions
	protected:
		//{{AFX_MSG(GTColorSelectorWnd)

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
		CWnd* pNotifyWellWnd;
};
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GTCOLORSELECTORWND_H__F818AFD2_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
