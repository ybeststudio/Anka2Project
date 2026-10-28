#if !defined(AFX_EFFECTTABPAGE_H__654E3468_CE80_47F3_8458_7A75C4AF563C__INCLUDED_)
#define AFX_EFFECTTABPAGE_H__654E3468_CE80_47F3_8458_7A75C4AF563C__INCLUDED_

#pragma once

// EffectTabPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEffectTabPage dialog

class CEffectTabPage : public CPageCtrl
{
	public:
		enum
		{
			PAGE_TYPE_NONE = 0xfffffff,
			PAGE_TYPE_PARTICLE = CEffectAccessor::EFFECT_ELEMENT_TYPE_PARTICLE,
			PAGE_TYPE_MESH = CEffectAccessor::EFFECT_ELEMENT_TYPE_MESH,
			PAGE_TYPE_LIGHT = CEffectAccessor::EFFECT_ELEMENT_TYPE_LIGHT,
		};

		// Construction
	public:
		CEffectTabPage (CWnd* pParent = NULL);  // standard constructor
		BOOL Create (CWnd * pParent);

		void Initialize();

		int GetCurrentTab();
		int GetSelectedIndex();

		void UpdateUIWithNewItem();
		void UpdateUI();

		BOOL IsOpenTimeBar();
		BOOL IsOpenBoundingSphereDialog();

		// Dialog Data
		//{{AFX_DATA(CEffectTabPage)
		enum { IDD = IDD_EFFECT_TAB };
		CListBox	m_ctrlEffectList;
		BOOL	m_bUsingSceneObject;
		BOOL	m_bShowCursor;
		BOOL	m_bShowCharacter;
		BOOL	m_bShowBoundingSphere;
		//}}AFX_DATA


		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEffectTabPage)
	protected:
		virtual void DoDataExchange (CDataExchange* pDX);   // DDX/DDV support
		//}}AFX_VIRTUAL

		// Implementation
	protected:

		void OnLoadScript (const char* c_szFileName);
		void OnSaveScript (const char* c_szFileName);
		void SetFileName (const char* c_szFileName);

		// Generated message map functions
		//{{AFX_MSG(CEffectTabPage)
		afx_msg int OnCreate (LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnChangeEffectList();
		afx_msg void OnCreateParticle();
		afx_msg void OnCreateMesh();
		afx_msg void OnLoadScript();
		afx_msg void OnSaveScript();
		afx_msg void OnClearEffectElementOne();
		afx_msg void OnClearEffectElementAll();
		afx_msg void OnTimeBarEnable();
		afx_msg void OnCreateLight();
		afx_msg void OnEffectDuplicate();
		afx_msg void OnEffectMergeScript();
		afx_msg void OnEffectUsingSceneobject();
		afx_msg void OnEffectShowCursor();
		afx_msg void OnEffectShowCharacter();
		afx_msg void OnEffectShowBoundingSphere();
		afx_msg void OnShow();
		afx_msg void OnHide();
		afx_msg void OnShowAll();
		afx_msg void OnHideAll();
		afx_msg void OnLoadEnvironment();
		afx_msg void OnSaveAsScript();
		afx_msg void OnOpenBoundSphereDialog();
		afx_msg void OnEffectMoveUp();
		afx_msg void OnEffectMoveDown();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

		// Attributes
	protected:
		CBitmap m_BitmapParticle;
		CBitmap m_BitmapMesh;
		CBitmap m_BitmapLight;
		CFilename m_strFileName;

		int m_iSelectedIndex;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTABPAGE_H__654E3468_CE80_47F3_8458_7A75C4AF563C__INCLUDED_)
