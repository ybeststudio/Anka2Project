// mapattributepage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "mapattributepage.h"
#include "../DataCtrl/MapAccessorTerrain.h"
#include "../DataCtrl/NonPlayerCharacterInfo.h"
#include "../../GameLib/MonsterAreaInfo.h"

#include "MainFrm.h"
#include "WorldEditorDoc.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

//std::string strRankName[CMapAttributePage::MOB_RANK_MAX_NUM] = { "PAWN", "S_PAWN", "KNIGHT", "S_KNIGHT", "BOSS", "KING", };

/////////////////////////////////////////////////////////////////////////////
// CMapAttributePage dialog


CMapAttributePage::CMapAttributePage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CMapAttributePage::IDD, pParent), m_pSelectedMonsterAreaInfo (NULL)
{
	//{{AFX_DATA_INIT(CMapAttributePage)
	m_strNewMonsterCount = _T ("");
	m_strSelectedMonsterCount = _T ("");
	m_strSizeX = _T ("");
	m_strSizeY = _T ("");
	m_strOriginX = _T ("");
	m_strOriginY = _T ("");
	//}}AFX_DATA_INIT
}


void CMapAttributePage::DoDataExchange (CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapAttributePage)
	DDX_Control (pDX, IDC_SLIDER_ATTR, m_SliderAttrNum);
	DDX_Control (pDX, IDC_MAP_ATTR_SCALEY_PRINT, m_EditMonsterAreaSizeY);
	DDX_Control (pDX, IDC_MAP_ATTR_SCALEY, m_SliderMonsterAreaScale);
	DDX_Control (pDX, IDC_COMBO_SelectedMonsterName, m_ComboSelectedMonsterName);
	DDX_Control (pDX, IDC_EDIT_SELECTEDMONSTERAREAINFO_MONSTERCOUNT, m_EditSelectedMonsterCount);
	DDX_Control (pDX, IDC_COMBO_SELECTEDMONSTERAREAINFO_DIRECTION, m_ComboSelectedMonsterDirection);
	DDX_Control (pDX, IDC_COMBO_DIRECTION, m_ComboDirection);
	DDX_Control (pDX, IDC_EDIT_MONSTERAREAINFO_SIZEY, m_EditMonsterAreaInfoSizeY);
	DDX_Control (pDX, IDC_EDIT_MONSTERAREAINFO_SIZEX, m_EditMonsterAreaInfoSizeX);
	DDX_Control (pDX, IDC_EDIT_MONSTERAREAINFO_ORIGINY, m_EditMonsterAreaInfoOriginY);
	DDX_Control (pDX, IDC_EDIT_MONSTERAREAINFO_ORIGINX, m_EditMonsterAreaInfoOriginX);
	DDX_Control (pDX, IDC_EDIT_MONSTERAREAINFO_MONSTERCOUNT, m_EditMonsterCount);
	DDX_Control (pDX, IDC_COMBO_MonsterName, m_ComboMonsterName);
	DDX_Control (pDX, IDC_MAP_ATTR_SCALE_PRINT, m_BrushSizeEdit);
	DDX_Control (pDX, IDC_MAP_ATTR_SCALE, m_ctrAttrScale);
	DDX_Text (pDX, IDC_EDIT_MONSTERAREAINFO_MONSTERCOUNT, m_strNewMonsterCount);
	DDX_Text (pDX, IDC_EDIT_SELECTEDMONSTERAREAINFO_MONSTERCOUNT, m_strSelectedMonsterCount);
	DDX_Text (pDX, IDC_EDIT_MONSTERAREAINFO_SIZEX, m_strSizeX);
	DDX_Text (pDX, IDC_EDIT_MONSTERAREAINFO_SIZEY, m_strSizeY);
	DDX_Text (pDX, IDC_EDIT_MONSTERAREAINFO_ORIGINX, m_strOriginX);
	DDX_Text (pDX, IDC_EDIT_MONSTERAREAINFO_ORIGINY, m_strOriginY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CMapAttributePage, CPageCtrl)
	//{{AFX_MSG_MAP(CMapAttributePage)
	ON_BN_CLICKED (IDC_CHECK_CELLEEDIT, OnCheckCelleedit)
	ON_BN_CLICKED (IDC_CHECK_ATTRERASER, OnCheckAttreraser)
	ON_BN_CLICKED (IDC_MAP_ATTR_BRUSH_SHAPE_CIRCLE, OnMapAttrBrushShapeCircle)
	ON_BN_CLICKED (IDC_MAP_ATTR_BRUSH_SHAPE_SQUARE, OnMapAttrBrushShapeSquare)
	ON_WM_HSCROLL()
	ON_BN_CLICKED (IDC_MAP_ATTR0_CHECKBOX, OnMapAttrCheckbox)
	ON_BN_CLICKED (IDC_BUTTON_MONSTERAREAINFO_REMOVE, OnButtonMonsterAreaInfoRemove)
	ON_BN_CLICKED (IDC_CHECK_SHOW_ALLMONSTERAREAINFO, OnCheckShowAllMonsterAreaInfo)
	ON_EN_CHANGE (IDC_EDIT_MONSTERAREAINFO_MONSTERCOUNT, OnChangeEditMonsterareainfoMonstercount)
	ON_EN_CHANGE (IDC_EDIT_MONSTERAREAINFO_ORIGINX, OnChangeEditMonsterareainfoOriginx)
	ON_EN_CHANGE (IDC_EDIT_MONSTERAREAINFO_ORIGINY, OnChangeEditMonsterareainfoOriginy)
	ON_EN_CHANGE (IDC_EDIT_MONSTERAREAINFO_SIZEX, OnChangeEditMonsterareainfoSizex)
	ON_EN_CHANGE (IDC_EDIT_MONSTERAREAINFO_SIZEY, OnChangeEditMonsterareainfoSizey)
	ON_BN_CLICKED (IDC_BUTTON_SAVE, OnButtonSave)
	ON_EN_CHANGE (IDC_EDIT_SELECTEDMONSTERAREAINFO_MONSTERCOUNT, OnChangeEditSelectedmonsterareainfoMonstercount)
	ON_BN_CLICKED (IDC_CHECK_MONSTERAREAINFO_ADD, OnCheckMonsterareainfoAdd)
	ON_BN_CLICKED (IDC_MAP_ATTR1_CHECKBOX, OnMapAttrCheckbox)
	ON_BN_CLICKED (IDC_MAP_ATTR2_CHECKBOX, OnMapAttrCheckbox)
	ON_BN_CLICKED (IDC_MAP_ATTR3_CHECKBOX, OnMapAttrCheckbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapAttributePage message handlers

BOOL CMapAttributePage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CMapAttributePage::IDD, pParent))
	{
		return FALSE;
	}

	CreateBitmapButton ((CButton*)GetDlgItem (IDC_MAP_ATTR_BRUSH_SHAPE_CIRCLE), IDB_MAP_TERRAIN_BRUSH_SHAPE_CIRCLE, m_BitmapBrushShapeCircle);
	CreateBitmapButton ((CButton*)GetDlgItem (IDC_MAP_ATTR_BRUSH_SHAPE_SQUARE), IDB_MAP_TERRAIN_BRUSH_SHAPE_SQUARE, m_BitmapBrushShapeSquare);

	CheckRadioButton (IDC_MAP_ATTR_BRUSH_SHAPE_CIRCLE, IDC_MAP_ATTR_BRUSH_SHAPE_SQUARE, IDC_MAP_ATTR_BRUSH_SHAPE_CIRCLE);
	CheckRadioButton (IDC_RADIO0, IDC_RADIO7, IDC_RADIO0);
	CheckDlgButton (IDC_CHECK_SHOW_ALLMONSTERAREAINFO, 1);

	m_ctrAttrScale.SetRange (0, 120);
	m_SliderMonsterAreaScale.SetRange (0, 120);
	m_SliderAttrNum.SetRange (0, 15);
	UpdateBrushSize();
	UpdateBrushShape();
	SetDlgItemInt (IDC_ATTR_NUM, m_SliderAttrNum.GetPos(), FALSE);

	return TRUE;
}

void CMapAttributePage::Initialize()
{
	CheckDlgButton (IDC_CHECK_CELLEEDIT, 0);
	CheckDlgButton (IDC_CHECK_MONSTERAREAINFO_ADD, 0);

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}

	DWORD dwMonsterAreaInfoCount = pMapManagerAccessor->GetMonsterAreaInfoCount();

	m_ComboMonsterName.ResetContent();
	m_ComboSelectedMonsterName.ResetContent();
	char szMonsterName[128 + 1];

	CNonPlayerCharacterInfo & rNonPlayerCharacterInfo = CNonPlayerCharacterInfo::Instance();

	DWORD dwNonPlayerCount = rNonPlayerCharacterInfo.GetNonPlayerCount();
	for (DWORD dwNonPlayerMapIndex = 0; dwNonPlayerMapIndex < dwNonPlayerCount; ++dwNonPlayerMapIndex)
	{
		BYTE byLowLevelLimit, byHighLevelLimit;
		rNonPlayerCharacterInfo.GetLevelRangeByMapIndex (dwNonPlayerMapIndex, &byLowLevelLimit, &byHighLevelLimit);
		BYTE byMonsterRank = rNonPlayerCharacterInfo.GetRankByMapIndex (dwNonPlayerMapIndex);
		std::string strMonsterName = rNonPlayerCharacterInfo.GetNameByMapIndex (dwNonPlayerMapIndex);
		DWORD dwMonsterVID = rNonPlayerCharacterInfo.GetVIDByMapIndex (dwNonPlayerMapIndex);

		_snprintf (szMonsterName, 128, "%d:%s(%02d~%02d):%d", byMonsterRank,	strMonsterName.c_str(), byLowLevelLimit, byHighLevelLimit, dwMonsterVID);
		m_ComboMonsterName.AddString (szMonsterName);
		m_ComboSelectedMonsterName.AddString (szMonsterName);
	}

	DWORD dwNPCGroupMapCount = rNonPlayerCharacterInfo.GetNPCGroupCount();
	for (DWORD dwNPCGroupMapIndex = 0; dwNPCGroupMapIndex < dwNPCGroupMapCount; ++dwNPCGroupMapIndex)
	{
		std::string strNPCGroupName = rNonPlayerCharacterInfo.GetNPCGroupNameByMapIndex (dwNPCGroupMapIndex);
		DWORD dwNPCGroupID = rNonPlayerCharacterInfo.GetNPCGroupIDByMapIndex (dwNPCGroupMapIndex);
		std::string strNPCGroupLeaderName = rNonPlayerCharacterInfo.GetNPCGroupLeaderNameByMapIndex (dwNPCGroupMapIndex);
		DWORD dwNPCGroupFollowerCount = rNonPlayerCharacterInfo.GetNPCGroupFollowerCountByMapIndex (dwNPCGroupMapIndex);

		_snprintf (szMonsterName, 128, "G:%s(%s,%d):%d", strNPCGroupName.c_str(), strNPCGroupLeaderName.c_str(), dwNPCGroupFollowerCount, dwNPCGroupID);
		m_ComboMonsterName.AddString (szMonsterName);
		m_ComboSelectedMonsterName.AddString (szMonsterName);
	}

	if (m_ComboMonsterName.GetCount() > 0)
	{
		CString strFirstName;
		m_ComboMonsterName.GetLBText (0, strFirstName);
		m_ComboMonsterName.SelectString (0, strFirstName);
	}
	else
	{
		m_ComboMonsterName.AddString ("No mob");
		m_ComboMonsterName.SelectString (0, "No mob");
	}
}

void CMapAttributePage::UpdateUI()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}

	pMapManagerAccessor->SetMaxBrushSize (m_SliderMonsterAreaScale.GetRangeMax());

	UpdateBrushSize();
	UpdateBrushMode();
	BuildMonsterAreaInfo();
	OnCheckShowAllMonsterAreaInfo();
}

void CMapAttributePage::UpdateBrushMode()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}

	CMainFrame* mainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CWorldEditorDoc* doc = (CWorldEditorDoc*)mainFrame->GetActiveView()->GetDocument();

	doc->SetEditingMapAttribute (IsDlgButtonChecked (IDC_CHECK_CELLEEDIT));

	pMapManagerAccessor->SetHeightEditing (false);
	pMapManagerAccessor->SetTextureEditing (false);
	pMapManagerAccessor->SetWaterEditing (false);
	pMapManagerAccessor->SetAttrEditing (TRUE == IsDlgButtonChecked (IDC_CHECK_CELLEEDIT));
	pMapManagerAccessor->SetEraseAttr (TRUE == IsDlgButtonChecked (IDC_CHECK_ATTRERASER));
	pMapManagerAccessor->SetMonsterAreaInfoEditing (TRUE == IsDlgButtonChecked (IDC_CHECK_MONSTERAREAINFO_ADD));
	if (TRUE == IsDlgButtonChecked (IDC_CHECK_MONSTERAREAINFO_ADD))
	{
		CheckRadioButton (IDC_MAP_ATTR_BRUSH_SHAPE_CIRCLE, IDC_MAP_ATTR_BRUSH_SHAPE_SQUARE, IDC_MAP_ATTR_BRUSH_SHAPE_SQUARE);
		m_ComboDirection.SelectString (0, "RANDOM");
		CString strNewMonsterName;
		if (CB_ERR == m_ComboMonsterName.GetCurSel())
		{
			m_ComboMonsterName.GetLBText (0, strNewMonsterName);
			m_ComboMonsterName.SelectString (0, strNewMonsterName);
		}
		else
		{
			m_ComboMonsterName.GetLBText (m_ComboMonsterName.GetCurSel(), strNewMonsterName);
			m_ComboMonsterName.SelectString (0, strNewMonsterName);
		}
		UpdateNewMonsterSelect();
		UpdateNewMonsterDir();
		UpdateBrushShape();
	}
}

void CMapAttributePage::OnCheckCelleedit()
{
	// TODO: Add your control notification handler code here
	CheckDlgButton (IDC_CHECK_MONSTERAREAINFO_ADD, 0);
	UpdateBrushMode();
}

void CMapAttributePage::OnCheckAttreraser()
{
	// TODO: Add your control notification handler code here
	CheckDlgButton (IDC_CHECK_MONSTERAREAINFO_ADD, 0);
	CheckDlgButton (IDC_CHECK_CELLEEDIT, 1);
	UpdateBrushMode();
}

void CMapAttributePage::OnMapAttrBrushShapeCircle()
{
	// TODO: Add your control notification handler code here
	CheckRadioButton (IDC_MAP_ATTR_BRUSH_SHAPE_CIRCLE, IDC_MAP_ATTR_BRUSH_SHAPE_SQUARE, IDC_MAP_ATTR_BRUSH_SHAPE_CIRCLE);
	UpdateBrushShape();
}

void CMapAttributePage::OnMapAttrBrushShapeSquare()
{
	// TODO: Add your control notification handler code here
	CheckRadioButton (IDC_MAP_ATTR_BRUSH_SHAPE_CIRCLE, IDC_MAP_ATTR_BRUSH_SHAPE_SQUARE, IDC_MAP_ATTR_BRUSH_SHAPE_SQUARE);
	UpdateBrushShape();
}

void CMapAttributePage::UpdateBrushShape()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor)
	{
		return;
	}

	if (TRUE == IsDlgButtonChecked (IDC_MAP_ATTR_BRUSH_SHAPE_CIRCLE))
	{
		pMapManagerAccessor->SetBrushShape (CTerrainAccessor::BRUSH_SHAPE_CIRCLE);
		SetDlgItemText (IDC_MAP_ATTR_BRUSH_SHAPE_PRINT, "Circle");
	}
	else if (TRUE == IsDlgButtonChecked (IDC_MAP_ATTR_BRUSH_SHAPE_SQUARE))
	{
		pMapManagerAccessor->SetBrushShape (CTerrainAccessor::BRUSH_SHAPE_SQUARE);
		SetDlgItemText (IDC_MAP_ATTR_BRUSH_SHAPE_PRINT, "Square");
	}
}

void CMapAttributePage::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData();

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor)
	{
		return;
	}

	if (pScrollBar->GetSafeHwnd() == m_ctrAttrScale.GetSafeHwnd())
	{
		pMapManagerAccessor->SetBrushSize (m_ctrAttrScale.GetPos());
		UpdateBrushSize();
	}
	else if (pScrollBar->GetSafeHwnd() == m_SliderMonsterAreaScale.GetSafeHwnd())
	{
		pMapManagerAccessor->SetBrushSizeY (m_SliderMonsterAreaScale.GetPos());
		UpdateBrushSize();
	}
	else if (pScrollBar->GetSafeHwnd() == m_SliderAttrNum.GetSafeHwnd())
	{
		OnMapAttrCheckbox();
		SetDlgItemInt (IDC_ATTR_NUM, m_SliderAttrNum.GetPos(), FALSE);
	}
}

void CMapAttributePage::UpdateBrushSize()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}

	m_iBrushSize = pMapManagerAccessor->GetBrushSize();
	m_iBrushSizeY = pMapManagerAccessor->GetBrushSizeY();

	char szTextLabel[16];
	memset (szTextLabel, 0, sizeof (char) * 16);
	sprintf (szTextLabel, "%d", m_iBrushSize);
	m_BrushSizeEdit.SetWindowText (szTextLabel);
	m_ctrAttrScale.SetPos (m_iBrushSize);

	memset (szTextLabel, 0, sizeof (char) * 16);
	sprintf (szTextLabel, "%d", m_iBrushSizeY);
	m_EditMonsterAreaSizeY.SetWindowText (szTextLabel);
	m_SliderMonsterAreaScale.SetPos (m_iBrushSizeY);
}

BYTE CMapAttributePage::GetAttributeFlag()
{
	int iAttrNum = m_SliderAttrNum.GetPos();

	int aiChecked[8] =
	{
		IsDlgButtonChecked (IDC_MAP_ATTR0_CHECKBOX),
		IsDlgButtonChecked (IDC_MAP_ATTR1_CHECKBOX),
		IsDlgButtonChecked (IDC_MAP_ATTR2_CHECKBOX),
		IsDlgButtonChecked (IDC_MAP_ATTR3_CHECKBOX),
		iAttrNum % 2,
		(iAttrNum / 2) % 2,
		(iAttrNum / 4) % 2,
		(iAttrNum / 8) % 2
	};

	BYTE bFlag = 0;

	Tracef ("%d, %d, %d, %d, %d, %d, %d, %d\n", aiChecked[0], aiChecked[1], aiChecked[2], aiChecked[3], aiChecked[4], aiChecked[5], aiChecked[6], aiChecked[7]);

	for (int i = 0; i < 8; ++i)
	{
		if (aiChecked[i])
		{
			bFlag |= (1 << i);
		}
	}

	return bFlag;
}

void CMapAttributePage::OnMapAttrCheckbox()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}
	pMapManagerAccessor->SetSelectedAttrFlag (GetAttributeFlag());
}

void CMapAttributePage::OnButtonMonsterAreaInfoRemove()
{
	// TODO: Add your control notification handler code here
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}

	if (!m_pSelectedMonsterAreaInfo)
	{
		return;
	}

	pMapManagerAccessor->RemoveMonsterAreaInfoPtr (m_pSelectedMonsterAreaInfo);

	m_pSelectedMonsterAreaInfo = NULL;
}

BOOL CMapAttributePage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	switch (LOWORD (wParam))
	{
		case IDC_COMBO_MonsterName:
			UpdateNewMonsterSelect();
			break;
		case IDC_COMBO_SelectedMonsterName:
			UpdateSelectedMonsterSelect();
			break;
		case IDC_COMBO_DIRECTION:
			UpdateNewMonsterDir();
			break;
		case IDC_COMBO_SELECTEDMONSTERAREAINFO_DIRECTION:
			UpdateSelectedMonsterDir();
			break;
	}
	return CPageCtrl::OnCommand (wParam, lParam);
}

void CMapAttributePage::UpdateNewMonsterSelect()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}

	CString strSelectedMonsterName;
	m_ComboMonsterName.GetLBText (m_ComboMonsterName.GetCurSel(), strSelectedMonsterName);

	std::string strMonsterNameWithTypeAndRankAndLevelRange = strSelectedMonsterName.GetBuffer (128);
	int iMonsterNameEndPos = strMonsterNameWithTypeAndRankAndLevelRange.find_first_of ("(", 0);
	int iMonsterVIDBeginPos = strMonsterNameWithTypeAndRankAndLevelRange.find_first_of (":", iMonsterNameEndPos) + 1;
	std::string strMonsterType = strMonsterNameWithTypeAndRankAndLevelRange.substr (0, 1);
	std::string strMonsterVID = strMonsterNameWithTypeAndRankAndLevelRange.substr (iMonsterVIDBeginPos, strMonsterNameWithTypeAndRankAndLevelRange.size() - iMonsterVIDBeginPos);
	DWORD dwMonsterVID = (DWORD) atoi (strMonsterVID.c_str());

	pMapManagerAccessor->SetNewMonsterVID (dwMonsterVID);
	if (0 == strMonsterType.compare ("G"))
	{
		pMapManagerAccessor->SetNewMonsterAreaInfoType (CMonsterAreaInfo::MONSTERAREAINFOTYPE_GROUP);
	}
	else
	{
		pMapManagerAccessor->SetNewMonsterAreaInfoType (CMonsterAreaInfo::MONSTERAREAINFOTYPE_MONSTER);
	}

}

void CMapAttributePage::UpdateNewMonsterDir()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}

	pMapManagerAccessor->SetNewMonsterDir ((CMonsterAreaInfo::EMonsterDir) m_ComboDirection.GetCurSel());
}

void CMapAttributePage::UpdateSelectedMonsterSelect()
{
	if (!m_pSelectedMonsterAreaInfo)
	{
		return;
	}

	CString strSelectedMonsterName;
	m_ComboSelectedMonsterName.GetLBText (m_ComboSelectedMonsterName.GetCurSel(), strSelectedMonsterName);

	std::string strMonsterNameWithTypeAndRankAndLevelRange = strSelectedMonsterName.GetBuffer (128);
	int iMonsterNameBeginPos = strMonsterNameWithTypeAndRankAndLevelRange.find_first_of (":", 0) + 1;
	int iMonsterNameEndPos = strMonsterNameWithTypeAndRankAndLevelRange.find_first_of ("(", 0);
	int iMonsterVIDBeginPos = strMonsterNameWithTypeAndRankAndLevelRange.find_first_of (":", iMonsterNameEndPos) + 1;
	std::string strMonsterType = strMonsterNameWithTypeAndRankAndLevelRange.substr (0, 1);
	std::string strMonsterName = strMonsterNameWithTypeAndRankAndLevelRange.substr (iMonsterNameBeginPos, iMonsterNameEndPos - iMonsterNameBeginPos);
	std::string strMonsterVID = strMonsterNameWithTypeAndRankAndLevelRange.substr (iMonsterVIDBeginPos, strMonsterNameWithTypeAndRankAndLevelRange.size() - iMonsterVIDBeginPos);
	DWORD dwMonsterVID = (DWORD) atoi (strMonsterVID.c_str());

	if (0 == strMonsterType.compare ("G"))
	{
		//G:%s(%s,%d):%d
		int iMonsterGroupLeaderNameBeginPos = strMonsterNameWithTypeAndRankAndLevelRange.find_first_of ("(", 0) + 1;
		int iMonsterGroupLeaderNameEndPos = strMonsterNameWithTypeAndRankAndLevelRange.find_first_of (",", 0);
		std::string strMonsterGroupLeaderName = strMonsterNameWithTypeAndRankAndLevelRange.substr (iMonsterGroupLeaderNameBeginPos,
																								   iMonsterGroupLeaderNameEndPos - iMonsterGroupLeaderNameBeginPos);

		int iMonsterGroupFollowerCountBeginPos = strMonsterNameWithTypeAndRankAndLevelRange.find_first_of (",", 0) + 1;
		int iMonsterGroupFollowerCountEndPos = strMonsterNameWithTypeAndRankAndLevelRange.find_first_of (")", iMonsterGroupFollowerCountBeginPos);
		std::string strMonsterGroupFollowerCount = strMonsterNameWithTypeAndRankAndLevelRange.substr (iMonsterGroupFollowerCountBeginPos,
																									  iMonsterGroupFollowerCountEndPos - iMonsterGroupFollowerCountBeginPos);
		DWORD dwMonsterGroupFollowerCount = (DWORD) atoi (strMonsterGroupFollowerCount.c_str());

		m_pSelectedMonsterAreaInfo->SetMonsterAreaInfoType (CMonsterAreaInfo::MONSTERAREAINFOTYPE_GROUP);
		m_pSelectedMonsterAreaInfo->SetMonsterGroupID (dwMonsterVID);
		m_pSelectedMonsterAreaInfo->SetMonsterGroupName (strMonsterName);
		m_pSelectedMonsterAreaInfo->SetMonsterGroupLeaderName (strMonsterGroupLeaderName);
		m_pSelectedMonsterAreaInfo->SetMonsterGroupFollowerCount (dwMonsterGroupFollowerCount);
	}
	else
	{
		m_pSelectedMonsterAreaInfo->SetMonsterAreaInfoType (CMonsterAreaInfo::MONSTERAREAINFOTYPE_MONSTER);
		m_pSelectedMonsterAreaInfo->SetMonsterVID (dwMonsterVID);
		m_pSelectedMonsterAreaInfo->SetMonsterName (strMonsterName);
	}

}

void CMapAttributePage::UpdateSelectedMonsterDir()
{
	if (!m_pSelectedMonsterAreaInfo)
	{
		return;
	}
	m_pSelectedMonsterAreaInfo->SetMonsterDirection ((CMonsterAreaInfo::EMonsterDir) m_ComboSelectedMonsterDirection.GetCurSel());
}

void CMapAttributePage::BuildMonsterAreaInfo()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}

	m_pSelectedMonsterAreaInfo = pMapManagerAccessor->GetSelectedMonsterAreaInfo();
	if (!m_pSelectedMonsterAreaInfo)
	{
		m_EditSelectedMonsterCount.EnableWindow (FALSE);
		m_ComboSelectedMonsterDirection.EnableWindow (FALSE);

		m_EditMonsterAreaInfoOriginX.EnableWindow (FALSE);
		m_EditMonsterAreaInfoOriginY.EnableWindow (FALSE);
		m_EditMonsterAreaInfoSizeX.EnableWindow (FALSE);
		m_EditMonsterAreaInfoSizeY.EnableWindow (FALSE);
		return;
	}

	UpdateSelectedMonsterAreainfo();
}

void CMapAttributePage::UpdateSelectedMonsterAreainfo()
{
	CNonPlayerCharacterInfo & rNonPlayerCharacterInfo = CNonPlayerCharacterInfo::Instance();
	std::string strMonsterName = "No name";
	char szMonsterName[128 + 1];
	switch (m_pSelectedMonsterAreaInfo->GetMonsterAreaInfoType())
	{
		case CMonsterAreaInfo::MONSTERAREAINFOTYPE_MONSTER:
		{
			strMonsterName = m_pSelectedMonsterAreaInfo->GetMonsterName();
			if (0 == strMonsterName.compare ("No name"))
			{
				DWORD dwMonsterVID = m_pSelectedMonsterAreaInfo->GetMonsterVID();
				if (dwMonsterVID != 0)
				{
					strMonsterName = rNonPlayerCharacterInfo.GetNameByVID (dwMonsterVID);
					if (0 != strMonsterName.compare ("No name"))
					{
						m_pSelectedMonsterAreaInfo->SetMonsterName (strMonsterName);
					}
				}
			}

			DWORD dwVID = m_pSelectedMonsterAreaInfo->GetMonsterVID();
			BYTE byLowLevelLimit, byHighLevelLimit;
			rNonPlayerCharacterInfo.GetLevelRangeByVID (dwVID, &byLowLevelLimit, &byHighLevelLimit);

			_snprintf (szMonsterName, 128, "%d:%s(%02d~%02d):%d",
					   rNonPlayerCharacterInfo.GetRankByVID (dwVID),
					   strMonsterName.c_str(),
					   byLowLevelLimit, byHighLevelLimit, dwVID);

			m_ComboSelectedMonsterName.SelectString (0, szMonsterName);
		}
		break;
		case CMonsterAreaInfo::MONSTERAREAINFOTYPE_GROUP:
		{
			strMonsterName = m_pSelectedMonsterAreaInfo->GetMonsterGroupName();
			if (0 == strMonsterName.compare ("No name"))
			{
				DWORD dwMonsterGroupID = m_pSelectedMonsterAreaInfo->GetMonsterGroupID();
				if (dwMonsterGroupID != 0)
				{
					strMonsterName = rNonPlayerCharacterInfo.GetNPCGroupNameByGroupID (dwMonsterGroupID);
					if (0 != strMonsterName.compare ("No name"))
					{
						m_pSelectedMonsterAreaInfo->SetMonsterGroupName (strMonsterName);
					}
				}
			}

			DWORD dwMonsterGroupID = m_pSelectedMonsterAreaInfo->GetMonsterGroupID();
			std::string strMonsterGroupLeaderName = rNonPlayerCharacterInfo.GetNPCGroupLeaderNameByGroupID (dwMonsterGroupID);
			DWORD dwMonsterGroupFollowerCount = rNonPlayerCharacterInfo.GetNPCGroupFollowerCountByGroupID (dwMonsterGroupID);

			//////////////////////////////////////////////////////////////////////////
			// G:%s(%s,%d):%d
			//
			_snprintf (szMonsterName, 128, "G:%s(%s,%d):%d",
					   strMonsterName.c_str(),
					   strMonsterGroupLeaderName.c_str(),
					   dwMonsterGroupFollowerCount,
					   dwMonsterGroupID);

			m_ComboSelectedMonsterName.SelectString (0, szMonsterName);
		}
		break;
	}

	long lOriginX, lOriginY, lSizeX, lSizeY;
	m_pSelectedMonsterAreaInfo->GetOrigin (&lOriginX, &lOriginY);
	m_pSelectedMonsterAreaInfo->GetSize (&lSizeX, &lSizeY);

	DWORD dwMonsterCount = m_pSelectedMonsterAreaInfo->GetMonsterCount();
	CMonsterAreaInfo::EMonsterDir eMonsterDir = m_pSelectedMonsterAreaInfo->GetMonsterDir();

	if (0 == strMonsterName.compare ("No name"))
	{
		m_EditSelectedMonsterCount.EnableWindow (FALSE);
		m_ComboSelectedMonsterDirection.EnableWindow (FALSE);

		m_EditMonsterAreaInfoOriginX.EnableWindow (FALSE);
		m_EditMonsterAreaInfoOriginY.EnableWindow (FALSE);
		m_EditMonsterAreaInfoSizeX.EnableWindow (FALSE);
		m_EditMonsterAreaInfoSizeY.EnableWindow (FALSE);
	}
	else
	{
		UpdateData();

		m_EditSelectedMonsterCount.EnableWindow (TRUE);
		m_ComboSelectedMonsterDirection.EnableWindow (TRUE);

		m_EditMonsterAreaInfoOriginX.EnableWindow (TRUE);
		m_EditMonsterAreaInfoOriginY.EnableWindow (TRUE);
		m_EditMonsterAreaInfoSizeX.EnableWindow (TRUE);
		m_EditMonsterAreaInfoSizeY.EnableWindow (TRUE);

		char szMonsterAreaInfoText[32 + 1];
		_snprintf (szMonsterAreaInfoText, 32, "%d", lOriginX);
		SetDlgItemText (IDC_EDIT_MONSTERAREAINFO_ORIGINX, szMonsterAreaInfoText);

		_snprintf (szMonsterAreaInfoText, 32, "%d", lOriginY);
		SetDlgItemText (IDC_EDIT_MONSTERAREAINFO_ORIGINY, szMonsterAreaInfoText);

		_snprintf (szMonsterAreaInfoText, 32, "%d", lSizeX);
		SetDlgItemText (IDC_EDIT_MONSTERAREAINFO_SIZEX, szMonsterAreaInfoText);

		_snprintf (szMonsterAreaInfoText, 32, "%d", lSizeY);
		SetDlgItemText (IDC_EDIT_MONSTERAREAINFO_SIZEY, szMonsterAreaInfoText);

		char szText[32];
		sprintf (szText, "%d", dwMonsterCount);
		m_EditSelectedMonsterCount.SetWindowText (szText);
		switch (eMonsterDir)
		{
			case CMonsterAreaInfo::DIR_RANDOM:
				m_ComboSelectedMonsterDirection.SelectString (0, "RANDOM");
				break;
			case CMonsterAreaInfo::DIR_NORTH:
				m_ComboSelectedMonsterDirection.SelectString (0, "NORTH");
				break;
			case CMonsterAreaInfo::DIR_NORTHEAST:
				m_ComboSelectedMonsterDirection.SelectString (0, "NORTHEAST");
				break;
			case CMonsterAreaInfo::DIR_EAST:
				m_ComboSelectedMonsterDirection.SelectString (0, "EAST");
				break;
			case CMonsterAreaInfo::DIR_SOUTHEAST:
				m_ComboSelectedMonsterDirection.SelectString (0, "SOUTHEAST");
				break;
			case CMonsterAreaInfo::DIR_SOUTH:
				m_ComboSelectedMonsterDirection.SelectString (0, "SOUTH");
				break;
			case CMonsterAreaInfo::DIR_SOUTHWEST:
				m_ComboSelectedMonsterDirection.SelectString (0, "SOUTHWEST");
				break;
			case CMonsterAreaInfo::DIR_WEST:
				m_ComboSelectedMonsterDirection.SelectString (0, "WEST");
				break;
			case CMonsterAreaInfo::DIR_NORTHWEST:
				m_ComboSelectedMonsterDirection.SelectString (0, "NORTHWEST");
				break;
		}
	}
}

void CMapAttributePage::OnCheckShowAllMonsterAreaInfo()
{
	// TODO: Add your control notification handler code here
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}

	pMapManagerAccessor->ShowAllMonsterAreaInfo (TRUE == IsDlgButtonChecked (IDC_CHECK_SHOW_ALLMONSTERAREAINFO));

}

void CMapAttributePage::OnChangeEditMonsterareainfoMonstercount()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPageCtrl::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}

	UpdateData();

	DWORD dwNewMonsterCount = (DWORD) atoi (m_strNewMonsterCount);

	pMapManagerAccessor->SetNewMonsterCount (dwNewMonsterCount);
}

void CMapAttributePage::OnChangeEditMonsterareainfoOriginx()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPageCtrl::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here
	if (!m_pSelectedMonsterAreaInfo)
	{
		return;
	}

	UpdateData();

	long lOriginX, lOriginY;
	m_pSelectedMonsterAreaInfo->GetOrigin (&lOriginX, &lOriginY);

	lOriginX = atol (m_strOriginX);

	m_pSelectedMonsterAreaInfo->SetOrigin (lOriginX, lOriginY);
}

void CMapAttributePage::OnChangeEditMonsterareainfoOriginy()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPageCtrl::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here
	if (!m_pSelectedMonsterAreaInfo)
	{
		return;
	}

	UpdateData();

	long lOriginX, lOriginY;
	m_pSelectedMonsterAreaInfo->GetOrigin (&lOriginX, &lOriginY);
	lOriginY = atol (m_strOriginY);

	m_pSelectedMonsterAreaInfo->SetOrigin (lOriginX, lOriginY);
}

void CMapAttributePage::OnChangeEditMonsterareainfoSizex()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPageCtrl::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here
	if (!m_pSelectedMonsterAreaInfo)
	{
		return;
	}

	UpdateData();

	long lSizeX, lSizeY;
	m_pSelectedMonsterAreaInfo->GetSize (&lSizeX, &lSizeY);
	lSizeX = atol (m_strSizeX);

	m_pSelectedMonsterAreaInfo->SetSize (lSizeX, lSizeY);
}

void CMapAttributePage::OnChangeEditMonsterareainfoSizey()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPageCtrl::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here
	if (!m_pSelectedMonsterAreaInfo)
	{
		return;
	}

	UpdateData();

	long lSizeX, lSizeY;
	m_pSelectedMonsterAreaInfo->GetSize (&lSizeX, &lSizeY);
	lSizeY = atol (m_strSizeY);

	m_pSelectedMonsterAreaInfo->SetSize (lSizeX, lSizeY);
}

void CMapAttributePage::OnButtonSave()
{
	// TODO: Add your control notification handler code here
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();
	if (!pMapManagerAccessor)
	{
		return;
	}

	pMapManagerAccessor->SaveMonsterAreaInfo();
	UpdateUI();
}

void CMapAttributePage::OnChangeEditSelectedmonsterareainfoMonstercount()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CPageCtrl::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here
	if (!m_pSelectedMonsterAreaInfo)
	{
		return;
	}

	UpdateData();
	DWORD dwSelctedMonsterCount = (DWORD) atoi (m_strSelectedMonsterCount);

	m_pSelectedMonsterAreaInfo->SetMonsterCount (dwSelctedMonsterCount);
}

void CMapAttributePage::OnCheckMonsterareainfoAdd()
{
	// TODO: Add your control notification handler code here
	CheckDlgButton (IDC_CHECK_CELLEEDIT, 0);
	CheckDlgButton (IDC_CHECK_ATTRERASER, 0);
	UpdateBrushMode();

}
