// NewMapDlg.h: interface for the CNewMapDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NEWMAPDLG_H__9B3FC717_E363_4831_9E12_B1410BA80BD0__INCLUDED_)
#define AFX_NEWMAPDLG_H__9B3FC717_E363_4831_9E12_B1410BA80BD0__INCLUDED_

#pragma once

class CNewMapDlg : public CDialog
{
	public:
		CNewMapDlg (CWnd* pParent = NULL);
		// Dialog Data
		//{{AFX_DATA(CNewMapDlg)
		enum { IDD = IDD_DIALOG_NEWMAP };
		CString	m_StringName;
		UINT	m_uiSizeX;
		UINT	m_uiSizeY;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CTexturePropertyDlg)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CTexturePropertyDlg)
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		afx_msg void OnUpdateName();
		afx_msg void OnUpdateSizeX();
		afx_msg void OnUpdateSizeY();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWMAPDLG_H__9B3FC717_E363_4831_9E12_B1410BA80BD0__INCLUDED_)
