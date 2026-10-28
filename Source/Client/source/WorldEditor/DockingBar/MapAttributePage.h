#if !defined(AFX_MAPATTRIBUTEPAGE_H__A65E8717_F547_46B2_A415_CD7056010156__INCLUDED_)
#define AFX_MAPATTRIBUTEPAGE_H__A65E8717_F547_46B2_A415_CD7056010156__INCLUDED_

#pragma once

// mapattributepage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapAttributePage dialog

class CMonsterAreaInfo;

class CMapAttributePage : public CPageCtrl
{
		// Construction
	public:
		enum EMobRank
		{
			MOB_RANK_PAWN = 0,
			MOB_RANK_S_PAWN,
			MOB_RANK_KNIGHT,
			MOB_RANK_S_KNIGHT,
			MOB_RANK_BOSS,
			MOB_RANK_KING,
			MOB_RANK_MAX_NUM,
		};

		CMapAttributePage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent = NULL);

		void Initialize();
		void UpdateUI();

		// Dialog Data
		//{{AFX_DATA(CMapAttributePage)
		enum { IDD = IDD_MAP_ATTRIBUTE };
		CSliderCtrl	m_SliderAttrNum;
		CEdit	m_EditMonsterAreaSizeY;
		CSliderCtrl	m_SliderMonsterAreaScale;
		CComboBox	m_ComboSelectedMonsterName;
		CEdit	m_EditSelectedMonsterCount;
		CComboBox	m_ComboSelectedMonsterDirection;
		CComboBox	m_ComboDirection;
		CEdit	m_EditMonsterAreaInfoSizeY;
		CEdit	m_EditMonsterAreaInfoSizeX;
		CEdit	m_EditMonsterAreaInfoOriginY;
		CEdit	m_EditMonsterAreaInfoOriginX;
		CEdit	m_EditMonsterCount;
		CComboBox	m_ComboMonsterName;
		CEdit	m_BrushSizeEdit;
		CSliderCtrl	m_ctrAttrScale;
		CString	m_strNewMonsterCount;
		CString	m_strSelectedMonsterCount;
		CString	m_strSizeX;
		CString	m_strSizeY;
		CString	m_strOriginX;
		CString	m_strOriginY;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapAttributePage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL

		// Implementation
	protected:
		CBitmap m_BitmapBrushShapeCircle;
		CBitmap m_BitmapBrushShapeSquare;

		CMonsterAreaInfo* m_pSelectedMonsterAreaInfo;

		char szTextLabel[16];

		int		m_iBrushSize;
		int		m_iBrushSizeY;

		void UpdateBrushMode();
		void UpdateBrushShape();
		void UpdateBrushSize();
		BYTE GetAttributeFlag();
		void BuildMonsterAreaInfo();
		void UpdateSelectedMonsterAreainfo();

		//////////////////////////////////////////////////////////////////////////
		void UpdateNewMonsterSelect();
		void UpdateNewMonsterDir();

		void UpdateSelectedMonsterSelect();
		void UpdateSelectedMonsterDir();

		// Generated message map functions
		//{{AFX_MSG(CMapAttributePage)
		afx_msg void OnCheckCellselect();
		afx_msg void OnCheckCelleedit();
		afx_msg void OnCheckAttreraser();
		afx_msg void OnMapAttrBrushShapeCircle();
		afx_msg void OnMapAttrBrushShapeSquare();
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnMapAttrCheckbox();
		afx_msg void OnButtonMonsterAreaInfoRemove();
		afx_msg void OnCheckShowAllMonsterAreaInfo();
		afx_msg void OnChangeEditMonsterareainfoMonstercount();
		afx_msg void OnChangeEditMonsterareainfoOriginx();
		afx_msg void OnChangeEditMonsterareainfoOriginy();
		afx_msg void OnChangeEditMonsterareainfoSizex();
		afx_msg void OnChangeEditMonsterareainfoSizey();
		afx_msg void OnButtonSave();
		afx_msg void OnChangeEditSelectedmonsterareainfoMonstercount();
		afx_msg void OnCheckMonsterareainfoAdd();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPATTRIBUTEPAGE_H__A65E8717_F547_46B2_A415_CD7056010156__INCLUDED_)
