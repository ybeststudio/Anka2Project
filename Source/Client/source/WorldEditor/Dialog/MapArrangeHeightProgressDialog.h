#if !defined(AFX_MAPARRANGEHEIGHTPROGRESS_H__8899239D_155E_4D17_A866_8AD1E2EA038C__INCLUDED_)
#define AFX_MAPARRANGEHEIGHTPROGRESS_H__8899239D_155E_4D17_A866_8AD1E2EA038C__INCLUDED_

#pragma once

// MapArrangeHeightProgress.h : header file
//

#include "../resource.h"

/////////////////////////////////////////////////////////////////////////////
// CMapArrangeHeightProgress dialog

class CMapArrangeHeightProgress : public CDialog
{
		// Construction
	public:
		CMapArrangeHeightProgress (CWnd* pParent = NULL);  // standard constructor

		// Dialog Data
		//{{AFX_DATA(CMapArrangeHeightProgress)
		enum { IDD = IDD_MAP_ARRANGE_HEIGHT };
		CStatic	m_ctrlNotify;
		CProgressCtrl	m_ctrlProgress;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapArrangeHeightProgress)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Operations
	public:
		void Init();
		void SetProgress (int iPos);
		void AddLine (const char* c_szFmt, ...);

		// Variables
	protected:
		bool m_bStopFlag;
		std::vector<std::string> m_kVec_strLine;

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CMapArrangeHeightProgress)
		afx_msg void OnOK();
		afx_msg void OnCancel();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPARRANGEHEIGHTPROGRESS_H__8899239D_155E_4D17_A866_8AD1E2EA038C__INCLUDED_)
