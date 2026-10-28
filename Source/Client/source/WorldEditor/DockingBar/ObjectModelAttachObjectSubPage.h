#if !defined(AFX_OBJECTMODELATTACHOBJECTSUBPAGE_H__884B2820_F3B9_4AF1_86B9_5D1CE8130B47__INCLUDED_)
#define AFX_OBJECTMODELATTACHOBJECTSUBPAGE_H__884B2820_F3B9_4AF1_86B9_5D1CE8130B47__INCLUDED_

#pragma once

// ObjectModelAttachObjectSubPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectModelAttachObjectSubPage dialog

class CObjectModelAttachObjectSubPage : public CDialog
{
		// Construction
	public:
		CObjectModelAttachObjectSubPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent, const CRect & c_rRect);
		void SetAttachingIndex (DWORD dwIndex);
		void UpdateUI();

		// Dialog Data
		//{{AFX_DATA(CObjectModelAttachObjectSubPage)
		enum { IDD = IDD_OBJECT_MODEL_ATTACH_OBJECT_PAGE };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectModelAttachObjectSubPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CObjectModelAttachObjectSubPage)
		afx_msg void OnLoadModel();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		DWORD m_dwAttachingIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTMODELATTACHOBJECTSUBPAGE_H__884B2820_F3B9_4AF1_86B9_5D1CE8130B47__INCLUDED_)
