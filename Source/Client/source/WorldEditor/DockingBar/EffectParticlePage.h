#if !defined(AFX_EFFECTPARTICLEPAGE_H__F69F8DC2_1C3C_4D0A_929C_3375531C5779__INCLUDED_)
#define AFX_EFFECTPARTICLEPAGE_H__F69F8DC2_1C3C_4D0A_929C_3375531C5779__INCLUDED_

#pragma once

// EffectParticlePage.h : header file
//

//------------------------//
// Test Code
#include "../UI/SimpleUI.h"
#include "EffectUI.h"
#include "../Dialog/EffectParticleTimeEventGraph.h"
#include "EffectParticleTexturePreview.h"
//------------------------//

/////////////////////////////////////////////////////////////////////////////
// CEffectParticlePage dialog

class CColorAnimationDlg;

class CEffectParticlePage : public CPageCtrl
{
		// Construction
	public:
		CEffectParticlePage (CWnd* pParent = NULL);  // standard constructor
		virtual ~CEffectParticlePage();
		BOOL Create (CWnd * pParent);

		void UpdateUI();

		void SetData (DWORD dwIndex);

		static void CallBack();

		// Dialog Data
		//{{AFX_DATA(CEffectParticlePage)
		enum { IDD = IDD_EFFECT_PARTICLE };
		COperationTypeComboBox	m_ctrlOperationType;
		CEffectParticleTexturePreview	m_ctrlPreview;
		CComboBox	m_ctrlAdvancedType;
		CComboBox	m_ctrlEmitterType;
		CBlendTypeComboBox	m_ctrlBlendDestType;
		CBlendTypeComboBox	m_ctrlBlendSrcType;
		CSliderCtrl	m_ctrlParticleRotationSpeed;
		CComboBox	m_ctrlRotationType;
		CComboBox	m_ctrlBillboardType;
		CSliderCtrl	m_ctrlTexAniDelay;
		CComboBox	m_ctrlTexAniType;
		CComboBox	m_ctrlTextureList;
		CComboBox	m_ctrlEmitterShape;
		CComboBox	m_ctrlParticleGraphList;
		CEffectTimeEventGraph m_ctrlParticleGraph;
		CComboBox	m_ctrlEmitterGraphList;
		CEffectTimeEventGraph m_ctrlEmitterGraph;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEffectParticlePage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL

		// Implementation
	protected:
		enum
		{
			EMITTER_TIME_EVENT_GRAPH_EMITTING_SIZE,
			EMITTER_TIME_EVENT_GRAPH_EMITTING_ANGULAR_VELOCITY,
			EMITTER_TIME_EVENT_GRAPH_DIRECTION_X,
			EMITTER_TIME_EVENT_GRAPH_DIRECTION_Y,
			EMITTER_TIME_EVENT_GRAPH_DIRECTION_Z,
			EMITTER_TIME_EVENT_GRAPH_VELOCITY,
			EMITTER_TIME_EVENT_GRAPH_EMISSION_COUNT,
			EMITTER_TIME_EVENT_GRAPH_LIFE_TIME,
			EMITTER_TIME_EVENT_GRAPH_SIZE_X,
			EMITTER_TIME_EVENT_GRAPH_SIZE_Y,
		};
		enum
		{
			PARTICLE_TIME_EVENT_GRAPH_SCALE_X,
			PARTICLE_TIME_EVENT_GRAPH_SCALE_Y,
			PARTICLE_TIME_EVENT_GRAPH_ROTATION,

			PARTICLE_TIME_EVENT_GRAPH_GRAVITY,
			PARTICLE_TIME_EVENT_GRAPH_AIR_RESISTANCE,

			PARTICLE_TIME_EVENT_GRAPH_COLOR_RED,
			PARTICLE_TIME_EVENT_GRAPH_COLOR_GREEN,
			PARTICLE_TIME_EVENT_GRAPH_COLOR_BLUE,
			PARTICLE_TIME_EVENT_GRAPH_ALPHA,
		};

		void SetBillboardType (int iType);
		void SetBlendType (int iSrcType, int iDestType);

		void SelectEmitterShape (int iShape);
		void SelectEmitterTimeEvent (int iEvent);
		void SelectParticleTimeEvent (int iEvent);

		void RefreshTextureList();

		// Generated message map functions
		//{{AFX_MSG(CEffectParticlePage)
		afx_msg void OnCheckEmitterFilledEnable();
		afx_msg void OnInsertTexture();
		afx_msg void OnClearAllTexture();
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnCheckTextureAnimationRandomStartFrame();
		afx_msg void OnCheckStretchFlag();
		afx_msg void OnClearOneTexture();
		afx_msg void OnCheckCycleLoopFlag();
		afx_msg void OnCheckAttachFlag();
		afx_msg void OnColorAnimation();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		// Attributes
	protected:
		DWORD m_dwElementIndex;

		CColorAnimationDlg* m_pColorAnimationDlg;

		static CEffectParticlePage* ms_pThis;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTPARTICLEPAGE_H__F69F8DC2_1C3C_4D0A_929C_3375531C5779__INCLUDED_)
