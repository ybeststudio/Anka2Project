#if !defined(AFX_TOOLBARFILE_H__C9E365AF_0E65_4B36_931D_8688D8F549C9__INCLUDED_)
#define AFX_TOOLBARFILE_H__C9E365AF_0E65_4B36_931D_8688D8F549C9__INCLUDED_

#pragma once

// ToolBarFile.h : header file
//

//------------------//
// Temp Code
#include "ToolBar256.h"
//------------------//

/////////////////////////////////////////////////////////////////////////////
// CToolBarFile window

class CToolBarFile : public CToolBar256
{
		// Construction
	public:
		CToolBarFile();

		// Attributes
	public:

		// Operations
	public:
		void ShowButtonText (BOOL bShow = TRUE);
		BOOL CN_UPDATE_HANDLER (UINT nID, CCmdUI *pCmdUI);
		BOOL CN_COMMAND_HANDLER (UINT nID, CCmdUI *pCmdUI);

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CToolBarFile)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~CToolBarFile();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CToolBarFile)
		// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLBARFILE_H__C9E365AF_0E65_4B36_931D_8688D8F549C9__INCLUDED_)
