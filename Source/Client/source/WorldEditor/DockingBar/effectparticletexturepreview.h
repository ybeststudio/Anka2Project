#if !defined(AFX_EFFECTPARTICLETEXTUREPREVIEW_H__15B021C0_F8BB_41E3_9B48_C67E6DE404F8__INCLUDED_)
#define AFX_EFFECTPARTICLETEXTUREPREVIEW_H__15B021C0_F8BB_41E3_9B48_C67E6DE404F8__INCLUDED_

#pragma once

// effectparticletexturepreview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEffectParticleTexturePreview window

class CEffectParticleTexturePreview : public CWnd, private CScreen
{
		// Construction
	public:
		CEffectParticleTexturePreview();

		// Attributes
	public:
	protected:
		CGraphicImage* m_pImage;
		CGraphicImageInstance m_ImageInstance;

		// Operations
	public:

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEffectParticleTexturePreview)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		void UpdatePreview (CGraphicImage * pImage);

		virtual ~CEffectParticleTexturePreview();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CEffectParticleTexturePreview)
		afx_msg void OnPaint();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTPARTICLETEXTUREPREVIEW_H__15B021C0_F8BB_41E3_9B48_C67E6DE404F8__INCLUDED_)
