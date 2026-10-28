#if !defined(AFX_EFFECTCTRLBAR_H__41C7ADDD_CCB4_45F4_BDDF_76ECD6F6FF50__INCLUDED_)
#define AFX_EFFECTCTRLBAR_H__41C7ADDD_CCB4_45F4_BDDF_76ECD6F6FF50__INCLUDED_

#pragma once

// EffectCtrlBar.h : header file
//

//--------------------------//
// Temp Code
#include "SizeCBar.h"
#include "EffectTabPage.h"
#include "EffectParticlePage.h"
#include "EffectMeshPage.h"
#include "EffectLightPage.h"
#include "EffectTexturePage.h"
#include "../Dialog/EffectTimeBar.h"
#include "../Dialog/EffectBoundingSphere.h"
//--------------------------//

/////////////////////////////////////////////////////////////////////////////
// CEffectCtrlBar window

class CEffectCtrlBar : public CSizingControlBar
{
		// Construction
	public:
		CEffectCtrlBar();
		BOOL Create (CWnd * pParent);
		void CreateTimeBar();
		void CreateBoundingSphere();

		void Initialize();

		// Attributes
	public:
		CEffectTabPage m_pageTab;
		CEffectParticlePage m_pageParticle;
		CEffectMeshPage m_pageMesh;
		CEffectLightPage m_pageLight;

		CEffectTimeBar* m_pdlgTimeBar;
		CEffectBoundingSphere* m_pdlgBoundingSphere;

		// Operations
	public:
		void UpdatePage();

		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEffectCtrlBar)
		//}}AFX_VIRTUAL

		// Implementation
	public:
		virtual ~CEffectCtrlBar();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CEffectCtrlBar)
		afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize (UINT nType, int cx, int cy);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTCTRLBAR_H__41C7ADDD_CCB4_45F4_BDDF_76ECD6F6FF50__INCLUDED_)
