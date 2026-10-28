// EffectCtrlBar.cpp : implementation file
//
#include "StdAfx.h"
#include "EffectCtrlBar.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

const int c_iTemporaryWidth = 0;
const int c_iTemporaryHeight = 280;

/////////////////////////////////////////////////////////////////////////////
// CEffectCtrlBar

CEffectCtrlBar::CEffectCtrlBar()
{
	m_pdlgTimeBar = NULL;
	m_pdlgBoundingSphere = NULL;
}

CEffectCtrlBar::~CEffectCtrlBar()
{
	if (m_pdlgTimeBar)
	{
		delete m_pdlgTimeBar;
		m_pdlgTimeBar = NULL;
	}
	if (m_pdlgBoundingSphere)
	{
		delete m_pdlgBoundingSphere;
		m_pdlgBoundingSphere = NULL;
	}
}

BEGIN_MESSAGE_MAP (CEffectCtrlBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CEffectCtrlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectCtrlBar normal functions

BOOL CEffectCtrlBar::Create (CWnd * pParent)
{
	if (!CSizingControlBar::Create ("Effect", pParent, CSize (220, 480), TRUE, AFX_IDW_TOOLBAR + 13))
	{
		return FALSE;
	}

	SetBarStyle (GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	return TRUE;
}

void CEffectCtrlBar::CreateTimeBar()
{
	m_pdlgTimeBar = new CEffectTimeBar;
	m_pdlgTimeBar->Create (IDD_EFFECT_TIME_BAR, this);
	m_pdlgTimeBar->SetWindowPos (NULL, 0, 0, 0, 0, TRUE);
	m_pdlgTimeBar->CreateButtons();
	m_pdlgTimeBar->ShowWindow (FALSE);
}

void CEffectCtrlBar::CreateBoundingSphere()
{
	m_pdlgBoundingSphere = new CEffectBoundingSphere;
	m_pdlgBoundingSphere->Create (IDD_EFFECT_BOUNDING_SPHERE, this);
	m_pdlgBoundingSphere->ShowWindow (FALSE);
}

void CEffectCtrlBar::UpdatePage()
{
	DWORD dwSelectedIndex = m_pageTab.GetSelectedIndex();
	m_pdlgTimeBar->SetData (dwSelectedIndex);
	m_pdlgTimeBar->ShowWindow (m_pageTab.IsOpenTimeBar());
	m_pdlgBoundingSphere->RefreshInfo();
	m_pdlgBoundingSphere->ShowWindow (m_pageTab.IsOpenBoundingSphereDialog());

	switch (m_pageTab.GetCurrentTab())
	{
		case CEffectTabPage::PAGE_TYPE_NONE:
			m_pageParticle.ShowWindow (SW_HIDE);
			m_pageMesh.ShowWindow (SW_HIDE);
			m_pageLight.ShowWindow (SW_HIDE);
			break;
		case CEffectTabPage::PAGE_TYPE_PARTICLE:
			m_pageParticle.SetData (dwSelectedIndex);

			m_pageParticle.ShowWindow (SW_SHOW);
			m_pageMesh.ShowWindow (SW_HIDE);
			m_pageLight.ShowWindow (SW_HIDE);
			break;
		case CEffectTabPage::PAGE_TYPE_MESH:
			m_pageMesh.SetData (dwSelectedIndex);

			m_pageParticle.ShowWindow (SW_HIDE);
			m_pageMesh.ShowWindow (SW_SHOW);
			m_pageLight.ShowWindow (SW_HIDE);
			break;
		case CEffectTabPage::PAGE_TYPE_LIGHT:
			m_pageLight.SetData (dwSelectedIndex);

			m_pageParticle.ShowWindow (SW_HIDE);
			m_pageMesh.ShowWindow (SW_HIDE);
			m_pageLight.ShowWindow (SW_SHOW);
			break;
	}
}

void CEffectCtrlBar::Initialize()
{
	m_pageTab.Initialize();
	m_pdlgTimeBar->Initialize();
}

/////////////////////////////////////////////////////////////////////////////
// CEffectCtrlBar message handlers

int CEffectCtrlBar::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBar::OnCreate (lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!m_pageTab.Create (this))
	{
		return -1;
	}

	if (!m_pageParticle.Create (this))
	{
		return -1;
	}
	if (!m_pageMesh.Create (this))
	{
		return -1;
	}
	if (!m_pageLight.Create (this))
	{
		return -1;
	}

	CreateTimeBar();
	CreateBoundingSphere();

	m_pageTab.SetTemporaryHeight (0);
	m_pageTab.ShowWindow (SW_SHOW);

	m_pageParticle.SetTemporaryHeight (c_iTemporaryHeight);
	m_pageParticle.ShowWindow (SW_HIDE);
	m_pageMesh.SetTemporaryHeight (c_iTemporaryHeight);
	m_pageMesh.ShowWindow (SW_HIDE);
	m_pageLight.SetTemporaryHeight (c_iTemporaryHeight);
	m_pageLight.ShowWindow (SW_HIDE);

	return 0;
}

void CEffectCtrlBar::OnSize (UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize (nType, cx, cy);

	CPageCtrl *pWnd = (CPageCtrl*)GetWindow (GW_CHILD);
	while (pWnd != NULL)
	{
		pWnd->CalcLayout();
		pWnd = (CPageCtrl*)pWnd->GetNextWindow();
	}

	RepositioningWindow (m_pageParticle, c_iTemporaryWidth, c_iTemporaryHeight);
	RepositioningWindow (m_pageMesh, c_iTemporaryWidth, c_iTemporaryHeight);
	RepositioningWindow (m_pageLight, c_iTemporaryWidth, c_iTemporaryHeight);
}
