// PageCtrl.h : header file
//
// 게임개발자용 맵에디터
// 저작권자 (c) 2001 신용우 (grace77@hanimail.com)
//
// 이 프로그램의 모든 권리는 저작권자에게 있습니다.
// 저작권자의 동의없이 프로그램을 설치/사용/수정/배포할 수 없습니다.

#if !defined(AFX_PAGECTRL_H__E5A7FC20_2B95_11D4_B73E_004F4C02CAF7__INCLUDED_)
#define AFX_PAGECTRL_H__E5A7FC20_2B95_11D4_B73E_004F4C02CAF7__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CPageCtrl dialog

class CPageCtrl : public CDialog
{
		// Construction
	public:
		virtual void UpdateUI() = 0;

		virtual void OnCancel();
		virtual void OnOK();
		void CalcLayout();

		void SetTemporaryHeight (int nTemporaryHeight);

		BOOL EnableWindow (BOOL bEnable = TRUE);

		CPageCtrl (UINT nIDTemplate, CWnd* pParent = NULL);  // standard constructor

		// Dialog Data
		//{{AFX_DATA(CPageCtrl)
		enum { IDD = 0 };
		// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CPageCtrl)
	public:
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:
		CPoint m_pt;
		int m_nScrollPos;
		int m_nTemporaryHeight;

		// Generated message map functions
		//{{AFX_MSG(CPageCtrl)
		afx_msg void OnLButtonDown (UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp (UINT nFlags, CPoint point);
		afx_msg void OnMouseMove (UINT nFlags, CPoint point);
		afx_msg BOOL OnMouseWheel (UINT nFlags, short zDelta, CPoint pt);
		afx_msg BOOL OnSetCursor (CWnd* pWnd, UINT nHitTest, UINT message);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGECTRL_H__E5A7FC20_2B95_11D4_B73E_004F4C02CAF7__INCLUDED_)
