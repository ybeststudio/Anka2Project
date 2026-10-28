// changebasexydlg.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "changebasexydlg.h"

#include "../DataCtrl/MapAccessorOutdoor.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChangeBaseXYDlg dialog


CChangeBaseXYDlg::CChangeBaseXYDlg (CWnd* pParent /*=NULL*/)
	: CDialog (CChangeBaseXYDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChangeBaseXYDlg)
	m_lBaseX = 0;
	m_lBaseY = 0;
	//}}AFX_DATA_INIT
}


void CChangeBaseXYDlg::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CChangeBaseXYDlg)
	DDX_Control (pDX, IDC_EDIT_BASEY, m_EditBaseY);
	DDX_Control (pDX, IDC_EDIT_BASEX, m_EditBaseX);
	DDX_Text (pDX, IDC_EDIT_BASEX, m_lBaseX);
	DDX_Text (pDX, IDC_EDIT_BASEY, m_lBaseY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CChangeBaseXYDlg, CDialog)
	//{{AFX_MSG_MAP(CChangeBaseXYDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChangeBaseXYDlg message handlers

void CChangeBaseXYDlg::OnOK()
{
	// TODO: Add extra validation here
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	CMapOutdoorAccessor * pOutdoor = (CMapOutdoorAccessor*) &pMapManagerAccessor->GetMapOutdoorRef();

	UpdateData();

	pOutdoor->SetBaseXY (m_lBaseX, m_lBaseY);
	pMapManagerAccessor->UpdateMapInfo();

	CDialog::OnOK();
}

BOOL CChangeBaseXYDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	CMapOutdoorAccessor * pOutdoor = (CMapOutdoorAccessor*) &pMapManagerAccessor->GetMapOutdoorRef();

	DWORD dwBaseX, dwBaseY;
	pOutdoor->GetBaseXY (&dwBaseX, &dwBaseY);

	m_lBaseX = dwBaseX;
	m_lBaseY = dwBaseY;

	UpdateData (FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
