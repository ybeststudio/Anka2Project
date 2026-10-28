#if !defined(AFX_FLYTABPAGE_H__D6EBEBF7_3288_4B24_A18B_04C9B612AC37__INCLUDED_)
#define AFX_FLYTABPAGE_H__D6EBEBF7_3288_4B24_A18B_04C9B612AC37__INCLUDED_

#pragma once

// FlyTabPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlyTabPage dialog

class CFlyTabPage : public CPageCtrl
{
		// Construction
	public:
		CFlyTabPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent);

		// for Color dialog
		static void CallBack();
	protected:
		static CFlyTabPage* ms_pThis;

	public:

		// Dialog Data
		//{{AFX_DATA(CFlyTabPage)
		enum { IDD = IDD_FLY_TAB };
		CSliderCtrl	m_ctrlTailAlpha;
		CColorBox	m_ctrlTailColor;
		CListBox	m_ctrlList;
		//}}AFX_DATA

		void UpdateUI();
		void RebuildAttachData();

		void Initialize();

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CFlyTabPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);
		virtual BOOL OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		int m_iSliderIndex;
		int m_iSelectIndex;

		void SelectAttachIndex (int iIndex);
		void TailUIEnable (bool bEnable);

		CBitmap m_BitmapEffect;
		CBitmap m_BitmapObject;

		CBitmap m_BitmapTypeLine;
		CBitmap m_BitmapTypeSine;
		CBitmap m_BitmapTypeExp;
		CBitmap m_BitmapTypeMultiLine;

		// Generated message map functions
		//{{AFX_MSG(CFlyTabPage)
		afx_msg void OnFlyHoming();
		afx_msg void OnFlySpreading();
		afx_msg void OnFlyTabNewEffect();
		afx_msg void OnFlyTabNewObject();
		afx_msg void OnFlyLoadBombEffect();
		afx_msg void OnFlyTypeLine();
		afx_msg void OnFlyTypeMultiLine();
		afx_msg void OnFlyTypeSine();
		afx_msg void OnFlyTypeExp();
		afx_msg void OnFlyClearOne();
		afx_msg void OnFlyClearAll();
		afx_msg void OnFlyLoadAttachFile();
		afx_msg void OnFlyDuplicate();
		afx_msg void OnFlyTabHasTail();
		afx_msg void OnFlyTabTailTriRadio();
		afx_msg void OnFlyTabTailRectRadio();
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnFlySaveScript();
		afx_msg void OnFlyLoadScript();
		afx_msg void OnFlyTabHitOnAnotherMonster();
		afx_msg void OnFlyTabHitOnBackground();
		afx_msg void OnMaintainParallel();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLYTABPAGE_H__D6EBEBF7_3288_4B24_A18B_04C9B612AC37__INCLUDED_)
