#if !defined(AFX_GTCOLORWELLWND_H__F818AFD5_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
#define AFX_GTCOLORWELLWND_H__F818AFD5_4DE0_11D6_A56D_525400EA266C__INCLUDED_

#pragma once

// GTColorWellWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// GTColorWellWnd window
class GTColorWellWnd : public CWnd
{
	public:
		// Constroctor
		GTColorWellWnd();

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
		COLORREF m_crColor;

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(GTColorWellWnd)
	protected:

		virtual BOOL PreCreateWindow (CREATESTRUCT& cs);
		virtual void PreSubclassWindow();
		//}}AFX_VIRTUAL

		// Implementation
	public:
		// Destructor
		virtual ~GTColorWellWnd();

		// Generated message map functions
	protected:
		//{{AFX_MSG(GTColorWellWnd)

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
		class CGTColorDialogDlg*	m_pParent;
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_GTCOLORWELLWND_H__F818AFD5_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
