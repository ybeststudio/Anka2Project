#if !defined(AFX_EFFECTLIGHTPAGE_H__0EDAF9B7_D87C_4452_AF47_747D8C5E57A0__INCLUDED_)
#define AFX_EFFECTLIGHTPAGE_H__0EDAF9B7_D87C_4452_AF47_747D8C5E57A0__INCLUDED_

#pragma once

// EffectLightPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEffectLightPage dialog

class CEffectLightPage : public CPageCtrl
{
		// Construction
	public:
		CEffectLightPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent);
		void Initialize();
		void UpdateUI();

		void SetData (DWORD dwIndex);

		static void CallBack();

		// Dialog Data
		//{{AFX_DATA(CEffectLightPage)
		enum { IDD = IDD_EFFECT_LIGHT };
		CEffectTimeEventGraph	m_ctrlRangeGraph;
		CSliderCtrl	m_ctrlDiffuseAlpha;
		CSliderCtrl	m_ctrlAmbientAlpha;
		CColorBox	m_ctrlAmbient;
		CColorBox	m_ctrlDiffuse;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEffectLightPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);
		virtual BOOL OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CEffectLightPage)
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnEffectLightLoop();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		DWORD m_dwIndex;
		DWORD m_iAlphaSliderIndex;
		static CEffectLightPage* ms_pThis;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTLIGHTPAGE_H__0EDAF9B7_D87C_4452_AF47_747D8C5E57A0__INCLUDED_)
