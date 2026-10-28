// ObjectTabPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectTabPage.h"

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
// CObjectTabPage dialog


CObjectTabPage::CObjectTabPage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CObjectTabPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectTabPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectTabPage::DoDataExchange (CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectTabPage)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectTabPage, CPageCtrl)
	//{{AFX_MSG_MAP(CObjectTabPage)
	ON_BN_CLICKED (IDC_OBJECT_TAB_MODEL, OnSelectTab)
	ON_BN_CLICKED (IDC_OBJECT_TAB_ANIMATION, OnSelectTab)
	ON_BN_CLICKED (IDC_OBJECT_TAB_LIGHT, OnSelectTab)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectTabPage normal functions

BOOL CObjectTabPage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CObjectTabPage::IDD, pParent))
	{
		return FALSE;
	}

	CreateBitmapButton ((CButton*)GetDlgItem (IDC_OBJECT_TAB_MODEL), IDB_OBJECT_MODEL, m_BitmapModel);
	CreateBitmapButton ((CButton*)GetDlgItem (IDC_OBJECT_TAB_ANIMATION), IDB_OBJECT_ANIMATION, m_BitmapAnimation);
	CheckRadioButton (IDC_OBJECT_TAB_MODEL, IDC_OBJECT_TAB_ANIMATION, IDC_OBJECT_TAB_MODEL);

	return TRUE;
}

void CObjectTabPage::UpdateUI()
{
}

int CObjectTabPage::GetCurrentTab()
{
	if (TRUE == IsDlgButtonChecked (IDC_OBJECT_TAB_MODEL))
	{
		return PAGE_TYPE_MODEL;
	}

	else if (TRUE == IsDlgButtonChecked (IDC_OBJECT_TAB_ANIMATION))
	{
		return PAGE_TYPE_ANIMATION;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// CObjectTabPage message handlers

void CObjectTabPage::OnSelectTab()
{
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();

	pFrame->UpdateObjectControlBar();
}
