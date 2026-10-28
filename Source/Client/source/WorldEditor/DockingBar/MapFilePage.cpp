// MapFilePage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapFilePage.h"
#include "../Dialog/NewMapDlg.h"
#include "../Dialog/DlgGoto.h"
#include "../Dialog/DlginitBaseTexture.h"
#include "../MainFrm.h"
#include "../WorldEditorDoc.h"
#include "../WorldEditorView.h"
#include "../DataCtrl/MapAccessorOutdoor.h"
#include "../Dialog/changebasexydlg.h"
#include "../Dialog/MapPortalDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapFilePage dialog


CMapFilePage::CMapFilePage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CMapFilePage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapFilePage)
	// NOTE: the ClassWizard will add member initialization here
	strEnvironmentDataPath = "D:\\Ymir Work\\environment";
	//}}AFX_DATA_INIT
}

CMapFilePage::~CMapFilePage()
{
	if (m_pPortalDialog)
	{
		delete m_pPortalDialog;
	}

	m_pPortalDialog = NULL;
}

void CMapFilePage::DoDataExchange (CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapFilePage)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CMapFilePage, CPageCtrl)
	//{{AFX_MSG_MAP(CMapFilePage)
	ON_BN_CLICKED (IDC_MAP_TERRAIN_OPTION_WIRE_FRAME, OnCheckOptionWireFrame)
	ON_BN_CLICKED (IDC_CHECK_GRID, OnCheckGrid)
	ON_BN_CLICKED (IDC_CHECK_GRID2, OnCheckGrid2)
	ON_BN_CLICKED (IDC_CHECK_CHARACTER, OnCheckCharacterRendering)
	ON_BN_CLICKED (IDC_MAP_FILE_NEW, OnNewMap)
	ON_BN_CLICKED (IDC_MAP_FILE_LOAD, OnLoadMap)
	ON_BN_CLICKED (IDC_MAP_FILE_SAVE, OnSaveMap)
	ON_BN_CLICKED (IDC_MAP_FILE_SAVEAS, OnSaveAsMap)
	ON_BN_CLICKED (IDC_MAP_FILE_SAVE_COLLISION_DATA, OnSaveCollisionData)
	ON_BN_CLICKED (IDC_MAP_FILE_SAVE_ATTRMAP, OnSaveAttrMap)
	ON_BN_CLICKED (IDC_BUTTON_GOTO, OnGoto)
	ON_BN_CLICKED (IDC_MAP_TERRAIN_OPTION_OBJECT_COLLISION, OnCheckOptionObjectCollisionRendering)
	ON_BN_CLICKED (IDC_MAP_TERRAIN_OPTION_OBJECT, OnCheckOptionObjectRendering)
	ON_BN_CLICKED (IDC_MAP_TERRAIN_OPTION_OBJECT_SHADOW, OnMapTerrainOptionObjectShadow)
	ON_BN_CLICKED (IDC_BUTTON_INIT_BASETEXTUREMAP, OnButtonInitBasetexturemap)
	ON_BN_CLICKED (IDC_CHECK_PATCH_GRID, OnCheckPatchGrid)
	ON_BN_CLICKED (IDC_CHECK_WATER, OnCheckWater)
	ON_BN_CLICKED (IDC_CHECK_COMPASS, OnCheckCompass)
	ON_BN_CLICKED (IDC_BUTTON_SAVE_ATLAS, OnButtonSaveAtlas)
	ON_BN_CLICKED (IDC_BUTTON_CHANGE_BASEXY, OnButtonChangeBasexy)
	ON_BN_CLICKED (IDC_BUTTON_SELECTE_ENVIRONMENT_SET, OnButtonSelecteEnvironmentSet)
	ON_BN_CLICKED (IDC_MAP_TERRAIN_OPTION_TERRAIN, OnCheckTerrainOption)
	ON_BN_CLICKED (IDC_BUTTON_CHANGE_TERRAIN_HEIGHT, OnButtonChangeTerrainHeight)
	ON_BN_CLICKED (IDC_MAP_TERRAIN_OPTION_FAKE_PORTAL_ENABLE, OnMapTerrainOptionFakePortalEnable)
	ON_BN_CLICKED (IDC_MAP_TERRAIN_OPTION_FAKE_PORTAL_ID_LIST, OnMapTerrainOptionFakePortalIdList)
	ON_BN_CLICKED (IDC_MAP_TERRAIN_GUILD_AREA, OnMapTerrainGuildArea)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapFilePage normal functions

BOOL CMapFilePage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CMapFilePage::IDD, pParent))
	{
		return FALSE;
	}

	m_pPortalDialog = new CMapPortalDialog;
	m_pPortalDialog->Create (CMapPortalDialog::IDD, this);
	m_pPortalDialog->ShowWindow (SW_HIDE);

	return TRUE;
}

void CMapFilePage::Initialize()
{
	CheckDlgButton (IDC_MAP_TERRAIN_OPTION_WIRE_FRAME, FALSE);
	CheckDlgButton (IDC_CHECK_GRID, FALSE);
	CheckDlgButton (IDC_CHECK_GRID2, FALSE);
	CheckDlgButton (IDC_CHECK_CHARACTER, FALSE);
	CheckDlgButton (IDC_MAP_TERRAIN_OPTION_OBJECT, TRUE);
	CheckDlgButton (IDC_MAP_TERRAIN_OPTION_OBJECT_COLLISION, FALSE);
	CheckDlgButton (IDC_MAP_TERRAIN_OPTION_OBJECT_SHADOW, FALSE);
	CheckDlgButton (IDC_MAP_TERRAIN_OPTION_TERRAIN, TRUE);
	OnCheckOptionWireFrame();
	OnCheckGrid();
	OnCheckGrid2();
	OnCheckCharacterRendering();
	OnCheckOptionObjectRendering();
	OnCheckOptionObjectCollisionRendering();
}

void CMapFilePage::UpdateUI()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	CMapOutdoor & rMapOutdoor = pMapManagerAccessor->GetMapOutdoorRef();

	m_lViewRadius = rMapOutdoor.GetViewRadius();
	m_fHeightScale = rMapOutdoor.GetHeightScale();

	DWORD dwBaseX, dwBaseY;
	rMapOutdoor.GetBaseXY (&dwBaseX, &dwBaseY);
	std::string strEnvironmentDataName = rMapOutdoor.GetEnvironmentDataName();

	char buf[128];
	SetDlgItemText (IDC_MAP_SCRIPT_NAME_PRINT, rMapOutdoor.GetName().c_str());

	sprintf (buf, "%.2f m", (float) (m_lViewRadius) * (float) (CTerrainImpl::CELLSCALE) / 100.0f);
	SetDlgItemText (IDC_STATIC_VIEW_RADIUS, buf);

	sprintf (buf, "%.2f m", (float) (CTerrainImpl::CELLSCALE) / 100.0f);
	SetDlgItemText (IDC_STATIC_WORLD_SCALE, buf);

	sprintf (buf, "%.2f m", m_fHeightScale * 65535.0f / 100.0f);
	SetDlgItemText (IDC_STATIC_MAX_HEIGHT, buf);

	sprintf (buf, "%d cm", dwBaseX);
	SetDlgItemText (IDC_STATIC_BASEX, buf);

	sprintf (buf, "%d cm", dwBaseY);
	SetDlgItemText (IDC_STATIC_BASEY, buf);

	SetDlgItemText (IDC_STATIC_ENVIRONMENT_SET, strEnvironmentDataName.c_str());
}

void CMapFilePage::RunLoadMapEvent()
{
	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CMapManagerAccessor * pMapManagerAccessor = pDocument->GetMapManagerAccessor();

	m_pPortalDialog->SetMapManagerHandler (pMapManagerAccessor);
}

/////////////////////////////////////////////////////////////////////////////
// CMapFilePage message handlers

void CMapFilePage::OnNewMap()
{
	CNewMapDlg dlg (AfxGetMainWnd());
	if (IDOK != dlg.DoModal())
	{
		return;
	}

	CWorldEditorApp * pApplication = (CWorldEditorApp*) AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	pMapManagerAccessor->CreateNewOutdoorMap();
}

void CMapFilePage::OnLoadMap()
{
	char szSavingFolder[256 + 32];

	if (!XBrowseForFolder (GetSafeHwnd(), g_szProgramWindowPath, szSavingFolder, 256 + 32))
	{
		return;
	}

	char* pszDir = strrchr (szSavingFolder, '\\');

	if (!pszDir)
	{
		return;
	}

	pszDir++;

	CWorldEditorApp * pApplication = (CWorldEditorApp*) AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor->LoadMap (pszDir))
	{
		return;
	}

	CMainFrame * pMainFrame = (CMainFrame*) AfxGetMainWnd();
	CWorldEditorView * pView = (CWorldEditorView*) pMainFrame->GetActiveView();
	pView->UpdateTargetPosition (0.0f, 0.0f);
	pMainFrame->RunLoadMapEvent();

	UpdateUI();
}

void CMapFilePage::OnSaveMap()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	//	char szFileName[256+1];
	//	_snprintf(szFileName, 256, "%s", pMapManagerAccessor->GetMapFileName());
	//
	//	char * pszPath = strrchr(szFileName, '/');
	//
	//	if (!pszPath)
	//	{
	//		if (!(pszPath = strrchr(szFileName, '\\')))
	//		{
	//			LogBox("[ERROR] Cannot save the file: Failed to locate directory name.");
	//			return;
	//		}
	//	}
	//
	//	*(pszPath) = '\0';
	pMapManagerAccessor->SaveMap();
}

void CMapFilePage::OnSaveAsMap()
{
	CString cstrMap;

	if (!GetDlgItemText (IDC_SAVEAS_FILENAME, cstrMap))
	{
		return;
	}

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	pMapManagerAccessor->SaveMap (cstrMap);
}

void CMapFilePage::OnSaveCollisionData()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Metin2 Collision Data File (*.mdc) |*.mdc|All Files (*.*)|*.*|";

	CFileDialog FileOpener (FALSE, "Save", "", dwFlag, c_szFilter, this);
	if (FileOpener.DoModal())
		if (strlen (FileOpener.GetPathName()) > 0)
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
			pMapManagerAccessor->SaveCollisionData (FileOpener.GetPathName());
		}
}

void CMapFilePage::OnSaveAttrMap()
{
	int iResult = MessageBox ("Are you sure you want to initialize?", NULL, MB_YESNO);
	if (6 != iResult)
	{
		return;
	}

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor)
	{
		return;
	}

	pMapManagerAccessor->ResetToDefaultAttr();
}

void CMapFilePage::OnGoto()
{
	CDlgGoto Goto (AfxGetMainWnd());
	if (IDOK != Goto.DoModal())
	{
		return;
	}
}

// Option

void CMapFilePage::OnCheckOptionWireFrame()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	pMapManagerAccessor->SetWireframe (IsDlgButtonChecked (IDC_MAP_TERRAIN_OPTION_WIRE_FRAME) == 1 ? true : false);
}

void CMapFilePage::OnCheckGrid()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CSceneMap* pScene = pDocument->GetSceneMap();
	pScene->SetMeterGrid (IsDlgButtonChecked (IDC_CHECK_GRID) != 0);
}

void CMapFilePage::OnCheckGrid2()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CSceneMap* pScene = pDocument->GetSceneMap();
	pScene->SetMapBoundGrid (IsDlgButtonChecked (IDC_CHECK_GRID2) != 0);
}

void CMapFilePage::OnCheckCharacterRendering()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CSceneMap* pScene = pDocument->GetSceneMap();
	pScene->SetCharacterRendering (IsDlgButtonChecked (IDC_CHECK_CHARACTER) != 0);
}

void CMapFilePage::OnCheckOptionObjectRendering()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CSceneMap* pScene = pDocument->GetSceneMap();
	pScene->SetObjectRendering (IsDlgButtonChecked (IDC_MAP_TERRAIN_OPTION_OBJECT) != 0);
}

void CMapFilePage::OnCheckOptionObjectCollisionRendering()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CSceneMap* pScene = pDocument->GetSceneMap();
	pScene->SetObjectCollisionRendering (IsDlgButtonChecked (IDC_MAP_TERRAIN_OPTION_OBJECT_COLLISION) != 0);
}

void CMapFilePage::OnCheckTerrainOption()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CSceneMap* pScene = pDocument->GetSceneMap();
	pScene->SetTerrainRendering (IsDlgButtonChecked (IDC_MAP_TERRAIN_OPTION_TERRAIN) != 0);
}

// Option

void CMapFilePage::OnMapTerrainOptionObjectShadow()
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CSceneMap* pScene = pDocument->GetSceneMap();
	pScene->SetObjectShadowRendering (IsDlgButtonChecked (IDC_MAP_TERRAIN_OPTION_OBJECT_SHADOW) != 0);
}

void CMapFilePage::OnButtonInitBasetexturemap()
{
	// TODO: Add your control notification handler code here
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor->IsMapReady())
	{
		return;
	}

	CDlgInitBaseTexture InitBaseTexture (AfxGetMainWnd());
	if (IDOK == InitBaseTexture.DoModal())
	{
		pMapManagerAccessor->LoadMap (pMapManagerAccessor->GetMapOutdoorRef().GetName());
	}
}

void CMapFilePage::OnCheckPatchGrid()
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CSceneMap* pScene = pDocument->GetSceneMap();
	pScene->SetPatchGrid (IsDlgButtonChecked (IDC_CHECK_PATCH_GRID) != 0);
}

void CMapFilePage::OnCheckWater()
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CSceneMap* pScene = pDocument->GetSceneMap();
	pScene->SetWaterRendering (IsDlgButtonChecked (IDC_CHECK_WATER) != 0);
}

void CMapFilePage::OnCheckCompass()
{
	// TODO: Add your control notification handler code here
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CSceneMap* pScene = pDocument->GetSceneMap();
	pScene->SetCompass (IsDlgButtonChecked (IDC_CHECK_COMPASS) != 0);

}

void CMapFilePage::OnButtonSaveAtlas()
{
	// TODO: Add your control notification handler code here
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor->IsMapReady())
	{
		return;
	}

	pMapManagerAccessor->SaveAtlas();
}

void CMapFilePage::OnButtonChangeBasexy()
{
	// TODO: Add your control notification handler code here
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor->IsMapReady())
	{
		return;
	}

	CChangeBaseXYDlg aDlg;
	aDlg.DoModal();

	UpdateUI();
}

void CMapFilePage::OnButtonSelecteEnvironmentSet()
{
	// TODO: Add your control notification handler code here
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor->IsMapReady())
	{
		return;
	}

	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Environment Files (*.msenv)|*.msenv|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);
	if (TRUE == FileOpener.DoModal())
	{
		std::string strEnvironmentName;
		GetOnlyFileName (FileOpener.GetPathName(), strEnvironmentName);
		pMapManagerAccessor->GetMapOutdoorRef().SetEnvironmentDataName (strEnvironmentName);
		UpdateUI();
	}
}

void CMapFilePage::OnButtonChangeTerrainHeight()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor->IsMapReady())
	{
		LogBox ("[ERROR] Map is not being created");
		return;
	}

	CMapOutdoorAccessor * pMapOutdoor = pMapManagerAccessor->GetMapOutdoorPtr();
	pMapOutdoor->ArrangeTerrainHeight();
}

void CMapFilePage::OnMapTerrainOptionFakePortalEnable()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor->IsMapReady())
	{
		LogBox ("[ERROR] Map is not being created");
		CheckDlgButton (IDC_MAP_TERRAIN_OPTION_FAKE_PORTAL_ENABLE, FALSE);
		return;
	}

	pMapManagerAccessor->EnablePortal (IsDlgButtonChecked (IDC_MAP_TERRAIN_OPTION_FAKE_PORTAL_ENABLE) != 0);
	pMapManagerAccessor->RefreshPortal();
}

void CMapFilePage::OnMapTerrainOptionFakePortalIdList()
{
}

void CMapFilePage::OnMapTerrainGuildArea()
{
	CMainFrame *pFrame = (CMainFrame*)AfxGetMainWnd();
	CWorldEditorDoc * pDocument = (CWorldEditorDoc*)pFrame->GetActiveDocument();
	CSceneMap* pScene = pDocument->GetSceneMap();
	pScene->SetGuildAreaRendering (IsDlgButtonChecked (IDC_MAP_TERRAIN_GUILD_AREA) != 0);
}
