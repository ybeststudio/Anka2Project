// MapTabPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapTabPage.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

//------------------//
// Temp Code
#include "../MainFrm.h"
//------------------//

/////////////////////////////////////////////////////////////////////////////
// CMapTabPage dialog


CMapTabPage::CMapTabPage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CMapTabPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapTabPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMapTabPage::DoDataExchange (CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapTabPage)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CMapTabPage, CPageCtrl)
	//{{AFX_MSG_MAP(CMapTabPage)
	ON_BN_CLICKED (IDC_MAP_TAB_FILE, OnSelectTab)
	ON_BN_CLICKED (IDC_MAP_TAB_TERRAIN, OnSelectTab)
	ON_BN_CLICKED (IDC_MAP_TAB_OBJECT, OnSelectTab)
	ON_BN_CLICKED (IDC_MAP_TAB_ENVIRONMENT, OnSelectTab)
	ON_BN_CLICKED (IDC_MAP_TAB_ATTRIBUTE, OnSelectTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapTabPage normal functions

CToolBarFile m_wndToolBar;

BOOL CMapTabPage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CMapTabPage::IDD, pParent))
	{
		return FALSE;
	}

	CreateBitmapButton ((CButton*)GetDlgItem (IDC_MAP_TAB_FILE), IDB_MAP_FILE, m_BitmapFile);
	CreateBitmapButton ((CButton*)GetDlgItem (IDC_MAP_TAB_TERRAIN), IDB_MAP_TERRAIN, m_BitmapTerrain);
	CreateBitmapButton ((CButton*)GetDlgItem (IDC_MAP_TAB_OBJECT), IDB_MAP_OBJECT, m_BitmapObject);
	CreateBitmapButton ((CButton*)GetDlgItem (IDC_MAP_TAB_ENVIRONMENT), IDB_MAP_ENVIRONMENT, m_BitmapEnvironment);
	CreateBitmapButton ((CButton*)GetDlgItem (IDC_MAP_TAB_ATTRIBUTE), IDB_MAP_ATTRIBUTE, m_BitmapAttribute);

	CheckRadioButton (IDC_MAP_TAB_EDIT, IDC_MAP_TAB_FILE, IDC_MAP_TAB_FILE);

	return TRUE;
}

void CMapTabPage::UpdateUI()
{
}

/////////////////////////////////////////////////////////////////////////////
// CMapTabPage normal functions

void CMapTabPage::Initialize()
{
}

int CMapTabPage::GetCurrentTab()
{
	if (TRUE == IsDlgButtonChecked (IDC_MAP_TAB_FILE))
	{
		return PAGE_TYPE_FILE;
	}

	else if (TRUE == IsDlgButtonChecked (IDC_MAP_TAB_TERRAIN))
	{
		return PAGE_TYPE_TERRAIN;
	}

	else if (TRUE == IsDlgButtonChecked (IDC_MAP_TAB_OBJECT))
	{
		return PAGE_TYPE_OBJECT;
	}

	else if (TRUE == IsDlgButtonChecked (IDC_MAP_TAB_ENVIRONMENT))
	{
		return PAGE_TYPE_ENVIRONMENT;
	}

	else if (TRUE == IsDlgButtonChecked (IDC_MAP_TAB_ATTRIBUTE))
	{
		return PAGE_TYPE_ATTRIBUTE;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// CMapTabPage message handlers

void CMapTabPage::OnSelectTab()
{
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();

	pFrame->UpdateMapControlBar();
}
