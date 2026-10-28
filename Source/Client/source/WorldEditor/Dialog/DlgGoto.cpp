// DlgGoto.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "DlgGoto.h"

#include "../MainFrm.h"
#include "../WorldEditorDoc.h"
#include "../WorldEditorView.h"
#include "../DataCtrl/MapAccessorOutdoor.h"
#include "../../EterLib/Camera.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgGoto dialog


CDlgGoto::CDlgGoto (CWnd* pParent /*=NULL*/)
	: CDialog (CDlgGoto::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgGoto)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgGoto::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CDlgGoto)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CDlgGoto, CDialog)
	//{{AFX_MSG_MAP(CDlgGoto)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgGoto message handlers


BOOL CDlgGoto::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDialogIntegerText (GetSafeHwnd(), IDC_EDIT_GOTO_POSITION_X, 0);
	SetDialogIntegerText (GetSafeHwnd(), IDC_EDIT_GOTO_POSITION_Y, 0);

	GetDlgItem (IDC_EDIT_GOTO_POSITION_X)->SetFocus();
	return FALSE;
}

void CDlgGoto::OnOK()
{
	DWORD dwxGoto = GetDialogIntegerText (GetSafeHwnd(), IDC_EDIT_GOTO_POSITION_X);
	DWORD dwyGoto = GetDialogIntegerText (GetSafeHwnd(), IDC_EDIT_GOTO_POSITION_Y);

	Goto (dwxGoto, dwyGoto);
	EndDialog (TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgGoto normal functions

void CDlgGoto::Goto (DWORD dwxGoto, DWORD dwyGoto)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	CWorldEditorView * pView = (CWorldEditorView*)pFrame->GetActiveView();
	D3DXVECTOR3 v3Target = CCameraManager::Instance().GetCurrentCamera()->GetTarget();

	short sTerrainCountX, sTerrainCountY;

	CMapOutdoorAccessor * pOutdoor = (CMapOutdoorAccessor*) &pMapManagerAccessor->GetMapOutdoorRef();
	pOutdoor->GetTerrainCount (&sTerrainCountX, &sTerrainCountY);

	if (dwxGoto >= (UINT) sTerrainCountX - 1)
	{
		dwxGoto = sTerrainCountX - 1;
	}

	if (dwyGoto >= (UINT) sTerrainCountY - 1)
	{
		dwyGoto = sTerrainCountY - 1;
	}

	pMapManagerAccessor->SaveTerrains();
	pMapManagerAccessor->SaveAreas();
	pMapManagerAccessor->SaveMonsterAreaInfo();
	pMapManagerAccessor->SetTerrainModified();
	pMapManagerAccessor->UpdateMap ((float) (dwxGoto * CTerrainImpl::TERRAIN_XSIZE), - (float) (dwyGoto * CTerrainImpl::TERRAIN_YSIZE), 0.0f);
	pView->UpdateTargetPosition ((float) (dwxGoto * CTerrainImpl::TERRAIN_XSIZE) - v3Target.x, - (float) (dwyGoto * CTerrainImpl::TERRAIN_YSIZE) - v3Target.y);
}
