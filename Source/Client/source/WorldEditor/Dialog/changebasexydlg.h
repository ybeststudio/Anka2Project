#if !defined(AFX_CHANGEBASEXYDLG_H__30125D1A_E543_4FF5_90D1_6ED5EDCE6139__INCLUDED_)
#define AFX_CHANGEBASEXYDLG_H__30125D1A_E543_4FF5_90D1_6ED5EDCE6139__INCLUDED_

#pragma once

// changebasexydlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChangeBaseXYDlg dialog

class CChangeBaseXYDlg : public CDialog
{
		// Construction
	public:
		CChangeBaseXYDlg (CWnd* pParent = NULL);  // standard constructor

		// Dialog Data
		//{{AFX_DATA(CChangeBaseXYDlg)
		enum { IDD = IDD_DIALOG_CHANGE_BASEXY };
		CEdit	m_EditBaseY;
		CEdit	m_EditBaseX;
		long	m_lBaseX;
		long	m_lBaseY;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CChangeBaseXYDlg)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CChangeBaseXYDlg)
		virtual void OnOK();
		virtual BOOL OnInitDialog();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEBASEXYDLG_H__30125D1A_E543_4FF5_90D1_6ED5EDCE6139__INCLUDED_)
