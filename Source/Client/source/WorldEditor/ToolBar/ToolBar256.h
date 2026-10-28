// ToolBar256.h : header file
//
// 게임개발자용 맵에디터
// 저작권자 (c) 2001 신용우 (grace77@hanimail.com)
//
// 이 프로그램의 모든 권리는 저작권자에게 있습니다.
// 저작권자의 동의없이 프로그램을 설치/사용/수정/배포할 수 없습니다.

#if !defined(AFX_TOOLBAR256_H__BC9421B2_59EE_11D4_AFB2_004F4C02CAF7__INCLUDED_)
#define AFX_TOOLBAR256_H__BC9421B2_59EE_11D4_AFB2_004F4C02CAF7__INCLUDED_

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CToolBar256 window

class CToolBar256 : public CToolBar
{
		// Construction
	public:
		BOOL LoadBitmap (LPCTSTR lpszResourceName);
		BOOL LoadBitmap (int nResourceID);

		CToolBar256();
		virtual ~CToolBar256();

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CToolBar256)
		//}}AFX_VIRTUAL


	protected:
		BOOL AddReplaceBitmap (HBITMAP hbmImageWell);
		int m_nBitmapID;
		HBITMAP GetBitmap256 (HINSTANCE hInst, HRSRC hRsrc, BOOL bMono);

		// Generated message map functions
		//{{AFX_MSG(CToolBar256)
		afx_msg void OnSysColorChange();
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLBAR256_H__BC9421B2_59EE_11D4_AFB2_004F4C02CAF7__INCLUDED_)
