#if !defined(AFX_MAPTERRAINTEXTUREPREVIEW_H__A32F6C16_3D24_4E79_BC88_7920DE141AB6__INCLUDED_)
#define AFX_MAPTERRAINTEXTUREPREVIEW_H__A32F6C16_3D24_4E79_BC88_7920DE141AB6__INCLUDED_

#pragma once

// mapterraintexturepreview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMapTerrainTexturePreview view

class CMapTerrainTexturePreview : public CWnd, public CScreen
{
	public:
		CMapTerrainTexturePreview();           // protected constructor used by dynamic creation

		// Operations
	public:
		BOOL Create();

	protected:
		ILuint m_ilImage;
		CGraphicDib m_DIB;

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMapTerrainTexturePreview)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		void UpdatePreview (int nTexNum);
		virtual ~CMapTerrainTexturePreview();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CMapTerrainTexturePreview)
		afx_msg void OnPaint();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAPTERRAINTEXTUREPREVIEW_H__A32F6C16_3D24_4E79_BC88_7920DE141AB6__INCLUDED_)
