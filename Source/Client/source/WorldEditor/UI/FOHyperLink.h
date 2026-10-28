// FOHyperLink.h: interface for the CFOHyperLink class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOHYPERLINK_H__44FC73BC_F596_11DD_A43A_525400EA266C__INCLUDED_)
#define AFX_FOHYPERLINK_H__44FC73BC_F596_11DD_A43A_525400EA266C__INCLUDED_

#pragma once

class CFOHyperLink : public CStatic
{
		// Construction/destruction
	public:
		CFOHyperLink();
		virtual ~CFOHyperLink();

		// Attributes
	public:

		// Operations
	public:

		virtual void SetURL (CString strURL);
		virtual CString GetURL() const;

		virtual void SetColors (COLORREF crLinkColor, COLORREF crVisitedColor,
								COLORREF crHoverColor = -1);
		virtual COLORREF GetLinkColor() const;
		virtual COLORREF GetVisitedColor() const;
		virtual COLORREF GetHoverColor() const;

		virtual void SetVisited (BOOL bVisited = TRUE);
		virtual BOOL GetVisited() const;

		virtual void SetLinkCursor (HCURSOR hCursor);
		virtual HCURSOR GetLinkCursor() const;

		virtual void SetUnderline (BOOL bUnderline = TRUE);
		virtual BOOL GetUnderline() const;

		virtual void SetAutoSize (BOOL bAutoSize = TRUE);
		virtual BOOL GetAutoSize() const;

		virtual HINSTANCE GotoURL (LPCTSTR url, int showcmd);

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CFOHyperLink)
	public:
		virtual BOOL PreTranslateMessage (MSG* pMsg);
	protected:
		virtual void PreSubclassWindow();
		//}}AFX_VIRTUAL

		// Implementation
	protected:
		virtual void ReportError (int nError);
		virtual LONG GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata);
		virtual void PositionWindow();
		virtual void SetDefaultCursor();

		// Protected attributes
	protected:
		COLORREF m_crLinkColor, m_crVisitedColor;
		COLORREF m_crHoverColor;
		BOOL     m_bOverControl;
		BOOL     m_bVisited;
		BOOL     m_bUnderline;
		BOOL     m_bAdjustToFit;
		CString  m_strURL;
		CFont    m_Font;
		HCURSOR  m_hLinkCursor;
		CToolTipCtrl m_ToolTip;

		// Generated message map functions
	protected:
		//{{AFX_MSG(CFOHyperLink)
		afx_msg HBRUSH CtlColor (CDC* pDC, UINT nCtlColor);
		afx_msg BOOL OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message);
		afx_msg void OnMouseMove (UINT nFlags, CPoint point);
		//}}AFX_MSG
		afx_msg void OnClicked();
		DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_FOHYPERLINK_H__44FC73BC_F596_11DD_A43A_525400EA266C__INCLUDED_)
