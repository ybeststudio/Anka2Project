#if !defined(AFX_MAPTERRAINPAGE_H__DC2463C4_34C1_40C3_A0B7_3FCD626DA567__INCLUDED_)
#define AFX_MAPTERRAINPAGE_H__DC2463C4_34C1_40C3_A0B7_3FCD626DA567__INCLUDED_

#pragma once

// MapTerrainPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapTerrainPage dialog

#include "mapterraintexlistctrl.h"
#include "mapterraintexturepreview.h"

class CMapTerrainPage : public CPageCtrl
{
		// Construction
	public:
		CMapTerrainPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent = NULL);

		void Initialize();
		void UpdateUI();

		// Dialog Data
		//{{AFX_DATA(CMapTerrainPage)
		enum { IDD = IDD_MAP_TERRAIN };
		CListCtrl	m_ctrTerrainTexList;
		CSliderCtrl	m_SliderWaterHeight;
		CEdit	m_EditWaterHeight;
		CMapTerrainTexturePreview	m_ctrTerrainTexturePreview;
		CEdit	m_StrengthSizeEdit;
		CEdit	m_BrushSizeEdit;
		CSliderCtrl	m_ctrStrengthScale;
		CSliderCtrl	m_ctrBrushScale;
		int		m_iBrushSize;
		int		m_iStrengthSize;
		int		m_iWaterheight;
		//}}AFX_DATA
		int GetBrushSize()
		{
			return m_iBrushSize;
		}
		int GetBrushStrength()
		{
			return m_iStrengthSize;
		}

		void UpdateTextureList();

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapTerrainPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		char m_szTextLabel[16];
		//	char szBrushStrengthText[16];
		//	char szAlphaScaleText[16];

		void UpdateBrushShape();
		void UpdateBrushType();
		void UpdateBrushApply();
		void UpdateBrushSize();
		void UpdateBrushStrength();
		void UpdateBrushWaterHeight();

		// Generated message map functions
		//{{AFX_MSG(CMapTerrainPage)
		afx_msg void OnSelectBrushShapeCircle();
		afx_msg void OnSelectBrushShapeSquare();
		afx_msg void OnSelectBrushTypeUp();
		afx_msg void OnSelectBrushTypeDown();
		afx_msg void OnSelectBrushTypePlateau();
		afx_msg void OnSelectBrushTypeNoise();
		afx_msg void OnSelectBrushTypeSmooth();
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnSelectHeightBrush();
		afx_msg void OnSelectTextureBrush();
		afx_msg void OnClickListTerraintexture (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnDblclkListTerraintexture (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnButtonAddtexture();
		afx_msg void OnKeydownListTerraintexture (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnCheckEraser();
		afx_msg void OnCheckTerrain0();
		afx_msg void OnCheckTerrain1();
		afx_msg void OnCheckTerrain2();
		afx_msg void OnCheckTerrain3();
		afx_msg void OnCheckTerrain4();
		afx_msg void OnCheckTerrain5();
		afx_msg void OnCheckTerrain6();
		afx_msg void OnCheckTerrain7();
		afx_msg void OnCheckTerrain8();
		afx_msg void OnCheckWaterbrush();
		afx_msg void OnCheckEraserwater();
		afx_msg void OnItemchangedListTerraintexture (NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnCheckDrawOnBlankTileOnly();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		CBitmap m_BitmapUpBrush;
		CBitmap m_BitmapDownBrush;
		CBitmap m_BitmapPlateauBrush;
		CBitmap m_BitmapNoiseBrush;
		CBitmap m_BitmapSmoothBrush;

		CBitmap m_BitmapBrushShapeCircle;
		CBitmap m_BitmapBrushShapeSquare;

		void	LoadList();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPTERRAINPAGE_H__DC2463C4_34C1_40C3_A0B7_3FCD626DA567__INCLUDED_)
