// GTColorDialogDlg.h : header file
//
/*
	Modified 7/15/2002 by David Swigger (dwswigger@yahoo.com)

	-interactive numeric display of HSV,RGB codes,
	-interactive numeric display of HexCode (html code)
*/
#if !defined(AFX_GTCOLORDIALOGDLG_H__F818AFC9_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
#define AFX_GTCOLORDIALOGDLG_H__F818AFC9_4DE0_11D6_A56D_525400EA266C__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CGTColorDialogDlg dialog

#include "GTColorSelectorWnd.h"
#include "GTColorWellWnd.h"
#include "GTColorSliderWnd.h"
#include "../Resource.h"

class CGTColorDialogDlg : public CDialog
{
	public:
		class IObserver
		{
			public:
				virtual void SendColor (COLORREF & rColor) = 0;
		};

		typedef struct SHSL
		{
			float fHue;
			float fSaturation;
			float fLightness;
		} THSL;

		// Construction
	public:
		CGTColorDialogDlg (CWnd* pParent = NULL);	// standard constructor

		// Dialog Data
		//{{AFX_DATA(CGTColorDialogDlg)
		enum { IDD = IDD_GTCOLORDIALOG_DIALOG };
		CEdit	m_edtHex;
		CEdit	m_edt_HSV_V;
		CEdit	m_edt_HSV_S;
		CEdit	m_edt_HSV_H;
		CEdit	m_edt_RGB_R;
		CEdit	m_edt_RGB_G;
		CEdit	m_edt_RGB_B;
		//}}AFX_DATA
		GTColorSelectorWnd	m_wndBulleted;
		GTColorWellWnd		m_wndWell;
		GTColorSliderWnd	m_wndSlider;
		CFont				m_hxFont;
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CGTColorDialogDlg)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);	// DDX/DDV support
		//}}AFX_VIRTUAL

	public:
		// Set color.
		void SetColor (COLORREF crColor);
		void ColorChanged();// called by wellwnd

		// Get color.
		COLORREF GetColor();

		// Return color by callback function.
		void SetObserver (IObserver * pObserver);

		void RGBToHSL (THSL * pHSL, COLORREF & rColor);
		void HSLToRGB (COLORREF * pColor, THSL & rHSL);
		// Implementation
	protected:
		HICON m_hIcon;

		// Generated message map functions
		//{{AFX_MSG(CGTColorDialogDlg)
		virtual BOOL OnInitDialog();
		afx_msg void OnSysCommand (UINT nID, LPARAM lParam);
		afx_msg void OnPaint();
		afx_msg HCURSOR OnQueryDragIcon();
		afx_msg void OnRadio1();
		afx_msg void OnRadio2();
		afx_msg void OnRadio3();
		afx_msg void OnRadio4();
		afx_msg void OnRadio5();
		afx_msg void OnRadio6();
		virtual void OnOK();
		afx_msg void OnChangeEdtHsvH();
		afx_msg void OnChangeEdtHsvS();
		afx_msg void OnChangeEdtHsvV();
		afx_msg void OnChangeEdtRgbB();
		afx_msg void OnChangeEdtRgbG();
		afx_msg void OnChangeEdtRgbR();
		afx_msg void OnChangeEdithex();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		// owner
		IObserver* 		m_pObserver;

		// funxtions
		CString			ColorToString (COLORREF cr);
		void			GetHSV (double& hue, double& sat, double& val);
		void			HandleHSVEdit();
		void			HandleRGBEdit();

		int				hex (char* szhex);
		COLORREF		html2rgb (char* szHTMLColorValue);

		// data
		COLORREF		m_crColor;
		volatile BOOL	m_bChangingHSV;
		volatile BOOL	m_bChangingRGB;
		volatile BOOL	m_bChangingHEX;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GTCOLORDIALOGDLG_H__F818AFC9_4DE0_11D6_A56D_525400EA266C__INCLUDED_)
