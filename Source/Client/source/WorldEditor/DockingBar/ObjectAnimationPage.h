#if !defined(AFX_OBJECTANIMATIONPAGE_H__AD27E02D_FA1F_4716_8226_B622C832CA99__INCLUDED_)
#define AFX_OBJECTANIMATIONPAGE_H__AD27E02D_FA1F_4716_8226_B622C832CA99__INCLUDED_

#pragma once

// ObjectAnimationPage.h : header file
//

// TestCode
#include "ObjectAnimationEventGraph.h"
// TestCode

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationPage dialog

class CObjectAnimationPage : public CPageCtrl
{
		// Construction
	public:
		CObjectAnimationPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent);

		void RegisterObserver();

		void Initialize();
		void UpdateUI();
		void UpdateLoopCount();
		void RenderEventGraph();

		void RecvLocalTime (float fDuration, float fLocalTime);
		void RecvPlay();
		void RecvStop();

		// Dialog Data
		//{{AFX_DATA(CObjectAnimationPage)
		enum { IDD = IDD_OBJECT_ANIMATION };
		CSliderCtrl	m_ctrlHitLimit;
		CSliderCtrl	m_ctrlWeaponLength;
		CComboBox	m_ctrlHitDataList;
		CComboBox	m_ctrlMotionType;
		CComboBox	m_ctrlAttackingBone;
		CSliderCtrl	m_ctrlAnimationSpeedBar;
		CComboBox	m_ctrlHittingType;
		CComboBox	m_ctrlAttackingType;
		CObjectAnimationEventGraph	m_ctrlEventGraph;
		CSliderCtrl	m_ctrlAnimationTimeBar;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CObjectAnimationPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);
		//}}AFX_VIRTUAL

		// Implementation
	protected:
		// Generated message map functions
		//{{AFX_MSG(CObjectAnimationPage)
		afx_msg void OnLoadAnimation();
		afx_msg void OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnPlay();
		afx_msg void OnLoadAnimationScript();
		afx_msg void OnSaveAnimationScript();
		afx_msg void OnCheckAttackingMotionEnable();
		afx_msg void OnCheckComboMotionEnable();
		afx_msg void OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
		afx_msg void OnCheckLoopMotionEnable();
		afx_msg void OnCheckLimitLoopEnable();
		afx_msg void OnCancelEnable();
		afx_msg void OnInsertHitData();
		afx_msg void OnDeleteHitData();
		afx_msg void OnChangeHitData();
		afx_msg void OnChangeLoopCount();
		afx_msg void OnUnlimtedLoopCount();
		afx_msg void OnToggleFrameStep();
		afx_msg void OnAddMotionEvent();
		afx_msg void OnDeleteMotionEvent();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	protected:
		void __BuildHitDataList();
		void __UpdateHitDataList();

	protected:
		DWORD m_dwcurHitDataIndex;

		std::string m_strScriptFileName;
		std::string m_strMotionFileName;

		int m_iTime;

		class CAnimationObserver : public ISceneObserver
		{
			public:
				CAnimationObserver() : m_pOwner (NULL) {}
				virtual ~CAnimationObserver() {}

				void SetOwner (CObjectAnimationPage * pOwner);

				void SetLocalTime (float fDuration, float fLocalTime);
				void Play (bool isLoop);
				void Stop();

			protected:
				CObjectAnimationPage* m_pOwner;
		} m_AnimationObserver;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTANIMATIONPAGE_H__AD27E02D_FA1F_4716_8226_B622C832CA99__INCLUDED_)
