#if !defined(AFX_MAPTABPAGE_H__25725C00_AB0B_4BA0_A4BF_99BBDA94E1CC__INCLUDED_)
#define AFX_MAPTABPAGE_H__25725C00_AB0B_4BA0_A4BF_99BBDA94E1CC__INCLUDED_

#pragma once

// MapTabPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapTabPage dialog

class CMapTabPage : public CPageCtrl
{
	public:
		enum
		{
			PAGE_TYPE_FILE,
			PAGE_TYPE_TERRAIN,
			PAGE_TYPE_OBJECT,
			PAGE_TYPE_ENVIRONMENT,
			PAGE_TYPE_ATTRIBUTE
		};

		// Construction
	public:
		CMapTabPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent = NULL);

		void Initialize();

		int GetCurrentTab();

		void UpdateUI();

		// Dialog Data
		//{{AFX_DATA(CMapTabPage)
		enum { IDD = IDD_MAP_TAB };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapTabPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CMapTabPage)
		afx_msg void OnSelectTab();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		CBitmap m_BitmapFile;
		CBitmap m_BitmapTerrain;
		CBitmap m_BitmapObject;
		CBitmap m_BitmapEnvironment;
		CBitmap m_BitmapAttribute;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPTABPAGE_H__25725C00_AB0B_4BA0_A4BF_99BBDA94E1CC__INCLUDED_)
