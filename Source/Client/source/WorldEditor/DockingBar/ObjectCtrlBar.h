#if !defined(AFX_OBJECTCTRLBAR_H__0B8DAE54_B592_4B83_B2B9_1EBD3C9B7E9F__INCLUDED_)
#define AFX_OBJECTCTRLBAR_H__0B8DAE54_B592_4B83_B2B9_1EBD3C9B7E9F__INCLUDED_

#pragma once

// ObjectCtrlBar.h : header file
//

//--------------------------//
// Temp Code
#include "SizeCBar.h"
#include "ObjectTabPage.h"
#include "ObjectModelPage.h"
#include "ObjectAnimationPage.h"
//--------------------------//

/////////////////////////////////////////////////////////////////////////////
// CObjectCtrlBar window

class CObjectCtrlBar : public CSizingControlBar
{
		// Construction
	public:
		CObjectCtrlBar();
		BOOL Create (CWnd * pParent);
		void Initialize();

		// Attributes
	public:
		CObjectTabPage m_pageTab;
		CObjectModelPage m_pageModel;
		CObjectAnimationPage m_pageAnimation;

		// Operations
	public:
		void UpdatePage();
		void UpdateAnimationPage();

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectCtrlBar)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~CObjectCtrlBar();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CObjectCtrlBar)
		afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize (UINT nType, int cx, int cy);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTCTRLBAR_H__0B8DAE54_B592_4B83_B2B9_1EBD3C9B7E9F__INCLUDED_)
