#if !defined(AFX_OBJECTTABPAGE_H__B136FBA4_4842_45DF_8C81_89DA3E24FF90__INCLUDED_)
#define AFX_OBJECTTABPAGE_H__B136FBA4_4842_45DF_8C81_89DA3E24FF90__INCLUDED_

#pragma once

// ObjectTabPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectTabPage dialog

class CObjectTabPage : public CPageCtrl
{
	public:
		enum
		{
			PAGE_TYPE_MODEL,
			PAGE_TYPE_ANIMATION,
		};

		// Construction
	public:
		CObjectTabPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent);

		void UpdateUI();

		int GetCurrentTab();

		// Dialog Data
		//{{AFX_DATA(CObjectTabPage)
		enum { IDD = IDD_OBJECT_TAB };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectTabPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(CObjectTabPage)
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		void OnSelectTab();

	protected:
		CBitmap m_BitmapModel;
		CBitmap m_BitmapAnimation;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTTABPAGE_H__B136FBA4_4842_45DF_8C81_89DA3E24FF90__INCLUDED_)
