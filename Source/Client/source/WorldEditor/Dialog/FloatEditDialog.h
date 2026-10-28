#if !defined(AFX_FLOATEDITDIALOG_H__2BAFCD81_4F48_4A0E_A39E_CEB96D2C286B__INCLUDED_)
#define AFX_FLOATEDITDIALOG_H__2BAFCD81_4F48_4A0E_A39E_CEB96D2C286B__INCLUDED_

#pragma once

// FloatEditDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFloatEditDialog dialog

class CFloatEditDialog : public CDialog
{
		// Construction
		typedef CEffectTimeEventGraph::IValueAccessor ValueAccessor;
	public:
		CFloatEditDialog (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd* pParentWnd, ValueAccessor * pAccessor = NULL);
		void Update (ValueAccessor * pAccessor);

		// Dialog Data
		//{{AFX_DATA(CFloatEditDialog)
		enum { IDD = IDD_FLOAT_EDIT_DIALOG };
		CListCtrl	m_ctrlListValue;
		CListCtrl	m_ctrlListTime;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CFloatEditDialog)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CFloatEditDialog)
		afx_msg void OnSize (UINT nType, int cx, int cy);
		afx_msg void OnEndlabeleditFloatEditListTime (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnClickFloatEditListTime (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnEndlabeleditFloatEditListValue (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclkFloatEditListTime (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnClickFloatEditListValue (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclkFloatEditListValue (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnRclickFloatEditListValue (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnRclickFloatEditListTime (NMHDR* pNMHDR, LRESULT* pResult);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		CEffectTimeEventGraph::IValueAccessor* m_pAccessor;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLOATEDITDIALOG_H__2BAFCD81_4F48_4A0E_A39E_CEB96D2C286B__INCLUDED_)
