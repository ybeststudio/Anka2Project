#if !defined(AFX_MAPTERRAINTEXLISTCTRL_H__998FD766_C0FF_4AC5_B3FB_DC835041DEDB__INCLUDED_)
#define AFX_MAPTERRAINTEXLISTCTRL_H__998FD766_C0FF_4AC5_B3FB_DC835041DEDB__INCLUDED_

#pragma once

// mapterraintexlistctrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapTerrainTexListCtrl window

class CMapTerrainTexturePreview;

class CMapTerrainTexListCtrl : public CListCtrl
{
		// Construction
	public:
		CMapTerrainTexListCtrl();

		// Attributes
	public:

		// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapTerrainTexListCtrl)
		//}}AFX_VIRTUAL

		// Implementation
		void LoadList();
	public:
		virtual ~CMapTerrainTexListCtrl();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CMapTerrainTexListCtrl)
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPTERRAINTEXLISTCTRL_H__998FD766_C0FF_4AC5_B3FB_DC835041DEDB__INCLUDED_)
