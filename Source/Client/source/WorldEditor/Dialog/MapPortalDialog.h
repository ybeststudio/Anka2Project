#if !defined(AFX_MAPPORTALDIALOG_H__06366FFE_4144_4292_88F8_5EABBCB3CBC7__INCLUDED_)
#define AFX_MAPPORTALDIALOG_H__06366FFE_4144_4292_88F8_5EABBCB3CBC7__INCLUDED_

#pragma once

// MapPortalDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapPortalDialog dialog

class CMapManagerAccessor;

class CMapPortalDialog : public CDialog
{
		// Construction
	public:
		CMapPortalDialog (CWnd* pParent = NULL);  // standard constructor

		// Operation
	public:
		void SetMapManagerHandler (CMapManagerAccessor * pAccessor);

		// Dialog Data
		//{{AFX_DATA(CMapPortalDialog)
		enum { IDD = IDD_MAP_PORTAL };
		CListCtrl	m_ctrlPortalList;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapPortalDialog)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CMapPortalDialog)
		virtual BOOL OnInitDialog();
		afx_msg void OnItemchangedPortalList (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnPortalShowAll();
		afx_msg void OnPortalHideAll();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		// Operations
	protected:
		void __RefreshPortalNumberList();
		void __ShowAll();
		void __HideAll();
		void __RefreshPortalIDList();

		// Variables
	protected:
		CMapManagerAccessor* m_pAccessor;
		BOOL m_iEnableCheckCallBack;

		std::vector<int> m_kVec_iPortalID;
		std::set<int> m_kSet_iShowingPortalID;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPPORTALDIALOG_H__06366FFE_4144_4292_88F8_5EABBCB3CBC7__INCLUDED_)
