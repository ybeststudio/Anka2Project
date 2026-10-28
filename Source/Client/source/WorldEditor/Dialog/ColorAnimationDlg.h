#if !defined(AFX_COLORANIMATIONDLG_H__CCBE6EC5_4556_4875_B925_74674BD146E8__INCLUDED_)
#define AFX_COLORANIMATIONDLG_H__CCBE6EC5_4556_4875_B925_74674BD146E8__INCLUDED_

#pragma once

// ColorAnimationDlg.h : header file
//

#include "EffectParticleTimeEventGraph.h"

/////////////////////////////////////////////////////////////////////////////
// CColorAnimationDlg dialog

class CColorAnimationDlg : public CDialog
{
		// Construction
	public:
		CColorAnimationDlg (CWnd* pParent = NULL);  // standard constructor

		static void CallBack();

		void Update();
		virtual BOOL Create (CWnd* pParent);

		void SetRGBAAccessor (
			CTimeEventFloatAccessor * pRedAccessor,
			CTimeEventFloatAccessor * pGreenAccessor,
			CTimeEventFloatAccessor * pBlueAccessor,
			CTimeEventFloatAccessor * pAlphaAccessor);

		// Dialog Data
		//{{AFX_DATA(CColorAnimationDlg)
		enum { IDD = IDD_COLOR_ANIMATION_DIALOG };
		CSliderCtrl	m_ctrlAlpha;
		CColorBox	m_ctrlColor;
		CStatic	m_ctrlRGBBar;
		CStatic	m_ctrlAlphaBar;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CColorAnimationDlg)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual BOOL OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		CTimeEventFloatAccessor* m_pRedAccessor;
		CTimeEventFloatAccessor* m_pGreenAccessor;
		CTimeEventFloatAccessor* m_pBlueAccessor;
		CTimeEventFloatAccessor* m_pAlphaAccessor;

		int m_iLeftLimit;
		int m_iRightLimit;

		int m_iSliderIndex;

		DWORD m_dwSelectedColor;
		DWORD m_dwSelectedAlpha;

		enum
		{
			SELECT_NONE,
			SELECT_COLOR,
			SELECT_ALPHA,
		};

		int m_iCurrentSelection;

		void AlignRGB();

		static CColorAnimationDlg* ms_pThis;

		// Generated message map functions
		//{{AFX_MSG(CColorAnimationDlg)
		afx_msg void OnPaint();
		afx_msg void OnMouseMove (UINT nFlags, CPoint point);
		afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnRButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnRButtonUp (UINT nFlags, CPoint point);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORANIMATIONDLG_H__CCBE6EC5_4556_4875_B925_74674BD146E8__INCLUDED_)
