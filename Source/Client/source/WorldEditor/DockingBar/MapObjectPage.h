#if !defined(AFX_MAPOBJECTPAGE_H__677DB8F7_2D3E_4910_BABB_21365427E921__INCLUDED_)
#define AFX_MAPOBJECTPAGE_H__677DB8F7_2D3E_4910_BABB_21365427E921__INCLUDED_

#pragma once

// MapObjectPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPage dialog

class CPropertyTreeControler : public CTreeCtrl
{
	public:
		typedef std::map<HTREEITEM, std::string> TPathMap;
		typedef TPathMap::iterator TPathIterator;

	public:
		CPropertyTreeControler();
		~CPropertyTreeControler();

		void RegisterPath (HTREEITEM hTreeItem, const std::string & c_rstrPathName);
		void DeletePath (HTREEITEM hTreeItem);
		bool GetPath (HTREEITEM hTreeItem, const char** c_ppszPathName);

	protected:
		TPathMap m_PathMap;
};

class CMapObjectPage : public CPageCtrl
{
		// Construction
	public:
		CMapObjectPage (CWnd* pParent = NULL);  // standard constructor
		virtual ~CMapObjectPage();
		BOOL Create (CWnd * pParent = NULL);

		void Initialize();
		void UpdateUI();

		void ClearPropertyTree();
		void LoadProperty();

		// Dialog Data
		//{{AFX_DATA(CMapObjectPage)
		enum { IDD = IDD_MAP_OBJECT };
		CListBox	m_ctrlPortalNumber;
		CStatic	m_StaticSelectedObjectInfo;
		CSliderCtrl	m_ctrlObjectScale;
		CSliderCtrl	m_ctrlCursorYaw;
		CSliderCtrl	m_ctrlCursorRoll;
		CSliderCtrl	m_ctrlCursorPitch;
		CSliderCtrl	m_ctrlObjectHeight;
		CSliderCtrl	m_ctrlGridDistance;
		CPropertyTreeControler	m_ctrlPropertyTree;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapObjectPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual BOOL OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		void __RefreshSelectedObjectProperty();
		void __RefreshSelectedObjectInfo();
		bool GetCurrentParentItem (HTREEITEM * pTreeItem, const char** c_ppszPathName);
		int SliderValueToCursorRotation (int iStep, int iCount, int iAmount);
		int CursorRotationToSliderValue (int iRotation, int iCount, int iAmount);

		// Generated message map functions
		//{{AFX_MSG(CMapObjectPage)
		afx_msg void OnCreateProperty();
		afx_msg void OnCreateDirectory();
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnCheckRandomRotation();
		afx_msg void OnCheckGridModeEnable();
		afx_msg void OnDblclkMapObjectTree (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnSelchangedMapObjectTree (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDelete();
		afx_msg void OnUnselect();
		afx_msg void OnInsertPortalNumber();
		afx_msg void OnDeletePortalNumber();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		// Attributes
	protected:
		CImageList m_TreeImageList;

		CBitmap m_BitmapBrushCircle;
		CBitmap m_BitmapBrushSquare;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPOBJECTPAGE_H__677DB8F7_2D3E_4910_BABB_21365427E921__INCLUDED_)
