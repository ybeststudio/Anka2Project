#if !defined(AFX_DLGGOTO_H__692712FF_116E_4F64_8A7B_ECA54C74471A__INCLUDED_)
#define AFX_DLGGOTO_H__692712FF_116E_4F64_8A7B_ECA54C74471A__INCLUDED_

#pragma once

// DlgGoto.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgGoto dialog

class CDlgGoto : public CDialog
{
		// Construction
	public:
		CDlgGoto (CWnd* pParent = NULL);  // standard constructor

		// Dialog Data
		//{{AFX_DATA(CDlgGoto)
		enum { IDD = IDD_DIALOG_GOTO };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CDlgGoto)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CDlgGoto)
		virtual void OnOK();
		virtual BOOL OnInitDialog();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		void Goto (DWORD dwxGoto, DWORD dwyGoto);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGGOTO_H__692712FF_116E_4F64_8A7B_ECA54C74471A__INCLUDED_)
