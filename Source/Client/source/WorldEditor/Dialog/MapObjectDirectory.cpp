// MapObjectCreateDirectory.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapObjectDirectory.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapObjectDirectory dialog


CMapObjectDirectory::CMapObjectDirectory (CWnd* pParent /*=NULL*/)
	: CDialog (CMapObjectDirectory::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapObjectDirectory)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMapObjectDirectory::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapObjectDirectory)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CMapObjectDirectory, CDialog)
	//{{AFX_MSG_MAP(CMapObjectDirectory)
	ON_BN_CLICKED (IDC_ACCEPT, OnDirectoryAccept)
	ON_BN_CLICKED (IDC_CANCEL, OnDirectoryCancel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapObjectDirectory normal functions

const char* CMapObjectDirectory::GetDirectoryName()
{
	return m_szFileName;
}

/////////////////////////////////////////////////////////////////////////////
// CMapObjectDirectory message handlers

void CMapObjectDirectory::OnOK()
{
	GetDlgItemText (IDC_MAP_OBJECT_CREATE_DIRECTORY_PATH_NAME, m_szFileName, 64);
	EndDialog (TRUE);
}

void CMapObjectDirectory::OnCancel()
{
	EndDialog (FALSE);
}

void CMapObjectDirectory::OnDirectoryAccept()
{
	OnOK();
}

void CMapObjectDirectory::OnDirectoryCancel()
{
	OnCancel();
}
