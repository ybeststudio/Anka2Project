#if !defined(AFX_MAPENVIRONMENTPAGE_H__A720959E_9F88_4F75_9409_D9FB275A7711__INCLUDED_)
#define AFX_MAPENVIRONMENTPAGE_H__A720959E_9F88_4F75_9409_D9FB275A7711__INCLUDED_

#pragma once

// MapEnvironmentPage.h : header file
//

// Test Code
#include "../UI/SimpleUI.h"
#include "EffectUI.h"

/////////////////////////////////////////////////////////////////////////////
// CMapEnvironmentPage dialog

class CMapEnvironmentPage : public CPageCtrl
{
		// Construction
	public:
		CMapEnvironmentPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent = NULL);
		void Initialize();

		void UpdateUI();

		// Dialog Data
		//{{AFX_DATA(CMapEnvironmentPage)
		enum { IDD = IDD_MAP_ENVIRONMENT };
		CSliderCtrl	m_ctrlMainFlareSize;
		CSliderCtrl	m_ctrlLensFlareMaxBrightness;
		CColorBox	m_ctrlLensFlareBrightnessColor;
		CColorBox	m_ctrlSkyBoxGradientSecondColor;
		CColorBox	m_ctrlSkyBoxGradientFirstColor;
		CComboBox	m_ctrlSkyBoxGradientList;
		CBlendTypeComboBox	m_ctrlFilteringAlphaDest;
		CBlendTypeComboBox	m_ctrlFilteringAlphaSrc;
		CSliderCtrl	m_ctrlFilteringAlpha;
		CColorBox	m_ctrlFilteringColor;
		CSliderCtrl	m_ctrlWindRandom;
		CSliderCtrl	m_ctrlWindStrength;
		CEdit	m_ctrlFogNearDistance;
		CEdit	m_ctrlFogFarDistance;
		CColorBox	m_ctrlMaterialEmissive;
		CColorBox	m_ctrlLightAmbient;
		CColorBox	m_ctrlLightDiffuse;
		CColorBox	m_ctrlMaterialAmbient;
		CColorBox	m_ctrlMaterialDiffuse;
		CColorBox	m_ctrlFogColor;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapEnvironmentPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual BOOL OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		static void CallBack();
		static void TurnOnCallBack();
		static void TurnOffCallBack();

		// Generated message map functions
		//{{AFX_MSG(CMapEnvironmentPage)
		afx_msg void OnEnableFog();
		afx_msg void OnEnableLight();
		afx_msg void OnEnableFiltering();
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnLoadEnvironmentScript();
		afx_msg void OnSaveEnvironmentScript();
		afx_msg void OnCheckLightpositionedit();
		afx_msg void OnLoadCloudTexture();
		afx_msg void OnInsertGradientUpper();
		afx_msg void OnOnInsertGradientLower();
		afx_msg void OnDeleteGradient();
		afx_msg void OnCheckLensFlareEnable();
		afx_msg void OnLoadLensFlareTexture();
		afx_msg void OnCheckMainFlareEnable();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		void RebuildGradientList();
		void SelectGradient (DWORD dwIndex);
		BOOL GetMapManagerAccessor (CMapManagerAccessor** ppMapData);
		void SetSkyBoxTexture (int iFaceIndex);

	protected:
		int m_iSliderIndex;

		static BOOL ms_isCallBackFlag;
		static CMapEnvironmentPage* ms_pThis;
	public:
		afx_msg void OnBnClickedSkyBoxModeTexture();
		afx_msg void OnSetSkyBoxFrontTexture();
		afx_msg void OnSetSkyBoxBackTexture();
		afx_msg void OnSetSkyBoxLeftTexture();
		afx_msg void OnSetSkyBoxRightTexture();
		afx_msg void OnSetSkyBoxTopTexture();
		afx_msg void OnSetSkyBoxBottomTexture();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPENVIRONMENTPAGE_H__A720959E_9F88_4F75_9409_D9FB275A7711__INCLUDED_)
