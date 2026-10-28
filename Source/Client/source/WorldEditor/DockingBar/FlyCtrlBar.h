#pragma once

#include "SizeCBar.h"
#include "FlyTabPage.h"
/////////////////////////////////////////////////////////////////////////////
// CFlyCtrlBar window

class CFlyCtrlBar : public CSizingControlBar
{
		// Construction
	public:
		CFlyCtrlBar();

		BOOL Create (CWnd * pParent);

		void Initialize();

		void UpdatePage();
		// Attributes
	public:

		// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CFlyCtrlBar)
		// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~CFlyCtrlBar();

		// Generated message map functions
	protected:
		CFlyTabPage m_pageTab;
		//{{AFX_MSG(CFlyCtrlBar)
		afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
