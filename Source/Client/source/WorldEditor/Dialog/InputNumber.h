#if !defined(AFX_INPUTNUMBER_H__24E48670_A51A_444E_A417_B772143E71BE__INCLUDED_)
#define AFX_INPUTNUMBER_H__24E48670_A51A_444E_A417_B772143E71BE__INCLUDED_

#pragma once

// InputNumber.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInputNumber dialog

class CInputNumber : public CDialog
{
		// Construction
	public:
		CInputNumber (CWnd* pParent = NULL);  // standard constructor

		// Operation
	public:
		int GetNumber();

		// Dialog Data
		//{{AFX_DATA(CInputNumber)
		enum { IDD = IDD_INPUT_NUMBER };
		//}}AFX_DATA

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CInputNumber)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CInputNumber)
		afx_msg void OnOK();
		afx_msg void OnCancel();
		afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		// Variables
	protected:
		int m_iNumber;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTNUMBER_H__24E48670_A51A_444E_A417_B772143E71BE__INCLUDED_)
