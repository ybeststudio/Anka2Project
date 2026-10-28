// MapCtrlBar.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapCtrlBar.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

const int c_iTemporaryWidth = 0;
const int c_iTemporaryHeight = 25;

/////////////////////////////////////////////////////////////////////////////
// CMapCtrlBar

CMapCtrlBar::CMapCtrlBar()
{
}

CMapCtrlBar::~CMapCtrlBar()
{
}


BEGIN_MESSAGE_MAP (CMapCtrlBar, CSizingControlBar)
	//{{AFX_MSG_MAP(CMapCtrlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapCtrlBar normal functions

BOOL CMapCtrlBar::Create (CWnd *pParent)
{
	if (!CSizingControlBar::Create ("Map Control Bar", pParent, CSize (220, 480), TRUE, AFX_IDW_TOOLBAR + 11))
	{
		return FALSE;
	}

	SetBarStyle (GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	return TRUE;
}

void CMapCtrlBar::Initialize()
{
	m_pageFile.Initialize();
	m_pageTerrain.Initialize();
	m_pageObject.Initialize();
	m_pageEnvironment.Initialize();
	m_pageAttribute.Initialize();

	UpdatePage();
}

void CMapCtrlBar::RunLoadMapEvent()
{
	m_pageFile.RunLoadMapEvent();
}

unsigned char CMapCtrlBar::UpdatePage()
{
	unsigned char ucMode;
	switch (m_pageTab.GetCurrentTab())
	{
		case CMapTabPage::PAGE_TYPE_FILE:
			m_pageFile.UpdateUI();
			m_pageFile.ShowWindow (SW_SHOW);
			m_pageTerrain.ShowWindow (SW_HIDE);
			m_pageObject.ShowWindow (SW_HIDE);
			m_pageEnvironment.ShowWindow (SW_HIDE);
			m_pageAttribute.ShowWindow (SW_HIDE);
			ucMode = CSceneMap::EDITING_MODE_NONE;
			break;

		case CMapTabPage::PAGE_TYPE_TERRAIN:
			m_pageTerrain.UpdateUI();
			m_pageFile.ShowWindow (SW_HIDE);
			m_pageTerrain.ShowWindow (SW_SHOW);
			m_pageObject.ShowWindow (SW_HIDE);
			m_pageEnvironment.ShowWindow (SW_HIDE);
			m_pageAttribute.ShowWindow (SW_HIDE);
			ucMode = CSceneMap::EDITING_MODE_TERRAIN;
			break;

		case CMapTabPage::PAGE_TYPE_OBJECT:
			m_pageObject.UpdateUI();
			m_pageFile.ShowWindow (SW_HIDE);
			m_pageTerrain.ShowWindow (SW_HIDE);
			m_pageObject.ShowWindow (SW_SHOW);
			m_pageEnvironment.ShowWindow (SW_HIDE);
			m_pageAttribute.ShowWindow (SW_HIDE);
			ucMode = CSceneMap::EDITING_MODE_OBJECT;
			break;

		case CMapTabPage::PAGE_TYPE_ENVIRONMENT:
			m_pageEnvironment.UpdateUI();
			m_pageFile.ShowWindow (SW_HIDE);
			m_pageTerrain.ShowWindow (SW_HIDE);
			m_pageObject.ShowWindow (SW_HIDE);
			m_pageEnvironment.ShowWindow (SW_SHOW);
			m_pageAttribute.ShowWindow (SW_HIDE);
			ucMode = CSceneMap::EDITING_MODE_ENVIRONMENT;
			break;
		case CMapTabPage::PAGE_TYPE_ATTRIBUTE:
			m_pageAttribute.UpdateUI();
			m_pageFile.ShowWindow (SW_HIDE);
			m_pageTerrain.ShowWindow (SW_HIDE);
			m_pageObject.ShowWindow (SW_HIDE);
			m_pageEnvironment.ShowWindow (SW_HIDE);
			m_pageAttribute.ShowWindow (SW_SHOW);
			ucMode = CSceneMap::EDITING_MODE_ATTRIBUTE;
			break;
	}
	return ucMode;
}

/////////////////////////////////////////////////////////////////////////////
// CMapCtrlBar message handlers

int CMapCtrlBar::OnCreate (LPCREATESTRUCT lpCreateStruct)
{
	if (CSizingControlBar::OnCreate (lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!m_pageTab.Create (this))
	{
		return -1;
	}
	if (!m_pageFile.Create (this))
	{
		return -1;
	}
	if (!m_pageTerrain.Create (this))
	{
		return -1;
	}
	if (!m_pageObject.Create (this))
	{
		return -1;
	}
	if (!m_pageEnvironment.Create (this))
	{
		return -1;
	}
	if (!m_pageAttribute.Create (this))
	{
		return -1;
	}

	m_pageTab.ShowWindow (SW_SHOW);
	m_pageFile.ShowWindow (SW_SHOW);
	m_pageTerrain.ShowWindow (SW_HIDE);
	m_pageObject.ShowWindow (SW_HIDE);
	m_pageEnvironment.ShowWindow (SW_HIDE);
	m_pageAttribute.ShowWindow (SW_HIDE);

	m_pageFile.SetTemporaryHeight (100);
	m_pageTerrain.SetTemporaryHeight (100);
	m_pageObject.SetTemporaryHeight (100);
	m_pageEnvironment.SetTemporaryHeight (100);
	m_pageAttribute.SetTemporaryHeight (100);

	return 0;
}

void CMapCtrlBar::OnSize (UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize (nType, cx, cy);

	CPageCtrl *pWnd = (CPageCtrl*)GetWindow (GW_CHILD);
	while (pWnd != NULL)
	{
		pWnd->CalcLayout();
		pWnd = (CPageCtrl*)pWnd->GetNextWindow();
	}

	RepositioningWindow (m_pageFile, c_iTemporaryWidth, c_iTemporaryHeight);
	RepositioningWindow (m_pageTerrain, c_iTemporaryWidth, c_iTemporaryHeight);
	RepositioningWindow (m_pageObject, c_iTemporaryWidth, c_iTemporaryHeight);
	RepositioningWindow (m_pageEnvironment, c_iTemporaryWidth, c_iTemporaryHeight);
	RepositioningWindow (m_pageAttribute, c_iTemporaryWidth, c_iTemporaryHeight);
}
