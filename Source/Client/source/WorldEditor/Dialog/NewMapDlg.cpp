// NewMapDlg.cpp: implementation of the CNewMapDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "NewMapDlg.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNewMapDlg::CNewMapDlg (CWnd* pParent /*=NULL*/)
	: CDialog (CNewMapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDialog)
	//}}AFX_DATA_INIT
}

void CNewMapDlg::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CTexturePropertyDlg)
	DDX_Text (pDX, IDC_EDIT_NEWMAP_NAME, m_StringName);
	DDX_Text (pDX, IDC_EDIT_NEWMAP_SIZEX, m_uiSizeX);
	DDV_MinMaxUInt (pDX, m_uiSizeX, 0, 1000);
	DDX_Text (pDX, IDC_EDIT_NEWMAP_SIZEY, m_uiSizeY);
	DDV_MinMaxUInt (pDX, m_uiSizeY, 0, 1000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CNewMapDlg, CDialog)
	//{{AFX_MSG_MAP(CNewMapDlg)
	//	ON_EN_CHANGE(IDC_EDIT_NEWMAP_NAME, OnUpdateName)
	//	ON_EN_CHANGE(IDC_EDIT_NEWMAP_SIZEX, OnUpdateSizeX)
	//	ON_EN_CHANGE(IDC_EDIT_NEWMAP_SIZEY, OnUpdateSizeY)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNewMapDlg message handlers

BOOL CNewMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_StringName	= "";
	m_uiSizeX		= 0;
	m_uiSizeY		= 0;

	UpdateData (FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNewMapDlg::OnOK()
{
	// TODO: Add extra validation here
	UpdateData();
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapData = pApplication->GetMapManagerAccessor();

	if ("" == m_StringName || 0 == m_uiSizeX || 0 == m_uiSizeY)
	{
		LogBoxf ("Data of New Map is invalid. Name: [%s] Size: [%u, %u].", m_StringName.GetBuffer (0), m_uiSizeX, m_uiSizeY);
		return;
	}
	pMapData->SetNewMapName (m_StringName.GetBuffer (0));
	pMapData->SetNewMapSizeX (m_uiSizeX);
	pMapData->SetNewMapSizeY (m_uiSizeY);
	CDialog::OnOK();
}

void CNewMapDlg::OnUpdateName()
{
	UpdateData();
}

void CNewMapDlg::OnUpdateSizeX()
{
	UpdateData();
}

void CNewMapDlg::OnUpdateSizeY()
{
	UpdateData();
}
