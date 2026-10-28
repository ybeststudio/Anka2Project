//IDC_OBJECT_ANIMATION_WEAPON_LENGTH
// ObjectAnimationPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectAnimationPage.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationPage dialog

CObjectAnimationPage::CObjectAnimationPage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CObjectAnimationPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectAnimationPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectAnimationPage::DoDataExchange (CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectAnimationPage)
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_HIT_LIMIT, m_ctrlHitLimit);
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_WEAPON_LENGTH, m_ctrlWeaponLength);
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_HIT_DATA_LIST, m_ctrlHitDataList);
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_MOTION_TYPE, m_ctrlMotionType);
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_ATTACKING_BONE, m_ctrlAttackingBone);
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_SPEED_BAR, m_ctrlAnimationSpeedBar);
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_HITTING_TYPE, m_ctrlHittingType);
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_ATTACKING_TYPE, m_ctrlAttackingType);
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_EVENT_GRAPH, m_ctrlEventGraph);
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_TIME_BAR, m_ctrlAnimationTimeBar);
	//}}AFX_DATA_MAP

	m_ctrlAnimationSpeedBar.SetRangeMin (1);
	m_ctrlAnimationSpeedBar.SetRangeMax (200);
	m_ctrlAnimationSpeedBar.SetTic (100);
	m_ctrlWeaponLength.SetRangeMin (0);
	m_ctrlWeaponLength.SetRangeMax (200);
	m_ctrlHitLimit.SetRangeMin (0);
	m_ctrlHitLimit.SetRangeMax (50);
}


BEGIN_MESSAGE_MAP (CObjectAnimationPage, CPageCtrl)
	//{{AFX_MSG_MAP(CObjectAnimationPage)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_LOAD, OnLoadAnimation)
	ON_WM_VSCROLL()
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_PLAY, OnPlay)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_LOAD_SCRIPT, OnLoadAnimationScript)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_SAVE_SCRIPT, OnSaveAnimationScript)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_ATTACKING_ENABLE, OnCheckAttackingMotionEnable)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_COMBO_MOTION_ENABLE, OnCheckComboMotionEnable)
	ON_WM_HSCROLL()
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_LOOP_MOTION_ENABLE, OnCheckLoopMotionEnable)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_LIMIT_LOOP_ENABLE, OnCheckLimitLoopEnable)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_HIT_DATA_INSERT, OnInsertHitData)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_HIT_DATA_DELETE, OnDeleteHitData)
	ON_CBN_SELCHANGE (IDC_OBJECT_ANIMATION_HIT_DATA_LIST, OnChangeHitData)
	ON_EN_CHANGE (IDC_OBJECT_ANIMATION_LOOP_COUNT, OnChangeLoopCount)
	ON_BN_CLICKED (IDC_BUTTON1, OnUnlimtedLoopCount)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_FRAME_STEP, OnToggleFrameStep)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_ADD_MOTION_EVENT, OnAddMotionEvent)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_DELETE_MOTION_EVENT, OnDeleteMotionEvent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationPage normal functions

BOOL CObjectAnimationPage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CObjectAnimationPage::IDD, pParent))
	{
		return FALSE;
	}

	m_ctrlMotionType.InsertString (0, "None");
	m_ctrlMotionType.InsertString (1, "Normal");
	m_ctrlMotionType.InsertString (2, "Combo");
	m_ctrlMotionType.InsertString (3, "Skill");
	m_ctrlMotionType.SelectString (-1, "None");

	m_ctrlAttackingType.InsertString (0, "Splash");
	m_ctrlAttackingType.InsertString (1, "Snipe");
	m_ctrlAttackingType.SelectString (-1, "Splash");

	m_ctrlHittingType.InsertString (0, "None");
	m_ctrlHittingType.InsertString (1, "Great");
	m_ctrlHittingType.InsertString (2, "Good");
	m_ctrlHittingType.SelectString (-1, "None");

	m_ctrlHitDataList.InsertString (0, "None");
	m_ctrlHitDataList.SelectString (-1, "None");

	return TRUE;
}

void CObjectAnimationPage::Initialize()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	m_ctrlEventGraph.Create();
	m_ctrlEventGraph.SetMotionData (pObjectData->GetMotionDataPointer());
	pSceneObject->SetMotionSpeed (1.0f);
	m_ctrlAnimationSpeedBar.SetPos (100);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_SPEED_BAR_PRINT, 1.0f);

	m_dwcurHitDataIndex = 0;

	UpdateUI();
}

void CObjectAnimationPage::UpdateUI()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	CRaceMotionDataAccessor * pMotionData = pObjectData->GetMotionDataPointer();

	// Register Bone Name List
	if (pSceneObject->isModelData())
	{
		m_ctrlAttackingBone.ResetContent();

		for (DWORD i = 0; i < pSceneObject->GetBoneCount(); ++i)
		{
			std::string strBoneName;
			if (pSceneObject->GetBoneName (i, &strBoneName))
			{
				m_ctrlAttackingBone.InsertString (i, strBoneName.c_str());
				m_ctrlAttackingBone.SelectString (-1, strBoneName.c_str());
			}
		}
	}

	// Register Attacking Motion Data
	if (pMotionData->isAttackingMotion())
	{
		const NRaceData::TMotionAttackData & c_rAttackData = pMotionData->GetMotionAttackDataReference();

		CString strMotionTypeName;
		m_ctrlMotionType.GetLBText (c_rAttackData.iMotionType, strMotionTypeName);
		m_ctrlMotionType.SelectString (-1, strMotionTypeName);

		CString strAttackTypeName;
		m_ctrlAttackingType.GetLBText (c_rAttackData.iAttackType, strAttackTypeName);
		m_ctrlAttackingType.SelectString (-1, strAttackTypeName);

		CString strHittingTypeName;
		m_ctrlHittingType.GetLBText (c_rAttackData.iHittingType, strHittingTypeName);
		m_ctrlHittingType.SelectString (-1, strHittingTypeName);

		SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_STIFFEN_TIME, c_rAttackData.fStiffenTime);
		SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_INVISIBLE_TIME, c_rAttackData.fInvisibleTime);
		SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EXTERNAL_FORCE, c_rAttackData.fExternalForce);

		m_ctrlHitLimit.SetPos (c_rAttackData.iHitLimitCount);
		SetDialogIntegerText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_HIT_LIMIT_PRINT, c_rAttackData.iHitLimitCount);

		__BuildHitDataList();
		__UpdateHitDataList();
	}
	else
	{
		m_ctrlAttackingType.SelectString (-1, "None");
		m_ctrlHittingType.SelectString (-1, "None");
		m_ctrlWeaponLength.SetPos (0);
		m_ctrlHitLimit.SetPos (0);
		SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_STIFFEN_TIME, 0.0f);
		SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_INVISIBLE_TIME, 0.0f);
		SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EXTERNAL_FORCE, 0.0f);
		SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_WEAPON_LENGTH_PRINT, 0.0f);
		SetDialogIntegerText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_HIT_LIMIT_PRINT, 0);
	}

	CheckDlgButton (IDC_OBJECT_ANIMATION_ATTACKING_ENABLE, pMotionData->isAttackingMotion());
	CheckDlgButton (IDC_OBJECT_ANIMATION_COMBO_MOTION_ENABLE, pMotionData->IsComboInputTimeData());
	CheckDlgButton (IDC_OBJECT_ANIMATION_LOOP_MOTION_ENABLE, pMotionData->IsLoopMotion());
	CheckDlgButton (IDC_OBJECT_ANIMATION_CANCEL_ENABLE_SKILL, pMotionData->IsCancelEnableSkill());
	OnCheckAttackingMotionEnable();

	SetDlgItemText (IDC_OBJECT_ANIMATION_SCRIPT_NAME, m_strScriptFileName.c_str());
	SetDlgItemText (IDC_OBJECT_ANIMATION_NAME, m_strMotionFileName.c_str());

	UpdateLoopCount();
	RenderEventGraph();
}

void CObjectAnimationPage::__BuildHitDataList()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();
	CRaceMotionDataAccessor * pMotionData = pObjectData->GetMotionDataPointer();

	m_ctrlHitDataList.ResetContent();
	for (DWORD i = 0; i < pMotionData->GetHitDataCount(); ++i)
	{
		char szHitDataName[32 + 1];
		_snprintf (szHitDataName, 32, "Hit Data %02d", i);
		m_ctrlHitDataList.InsertString (i, szHitDataName);
		m_ctrlHitDataList.SelectString (i, szHitDataName);
	}

	OnChangeHitData();
}

void CObjectAnimationPage::__UpdateHitDataList()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();
	CObjectData * pObjectData = pApplication->GetObjectData();
	CRaceMotionDataAccessor * pMotionData = pObjectData->GetMotionDataPointer();

	const NRaceData::THitData * c_pHitData = pMotionData->GetHitDataPtr (m_dwcurHitDataIndex);

	if (c_pHitData)
	{
		DWORD dwIndex;
		if (pSceneObject->GetBoneIndex (c_pHitData->strBoneName.c_str(), &dwIndex))
		{
			m_ctrlAttackingBone.SelectString (dwIndex, c_pHitData->strBoneName.c_str());
		}
		m_ctrlWeaponLength.SetPos (int (c_pHitData->fWeaponLength));
		SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_WEAPON_LENGTH_PRINT, c_pHitData->fWeaponLength);
	}
}

void CObjectAnimationPage::UpdateLoopCount()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	int iLoopCount = pObjectData->GetMotionDataPointer()->GetLoopCount();
	if (0 == iLoopCount)
	{
		SetDlgItemText (IDC_OBJECT_ANIMATION_LOOP_COUNT, "0");
		CheckDlgButton (IDC_OBJECT_ANIMATION_LIMIT_LOOP_ENABLE, FALSE);
		OnCheckLimitLoopEnable();
	}
	else
	{
		SetDialogIntegerText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_LOOP_COUNT, iLoopCount);
		CheckDlgButton (IDC_OBJECT_ANIMATION_LIMIT_LOOP_ENABLE, TRUE);
		OnCheckLimitLoopEnable();
	}
}

void CObjectAnimationPage::RenderEventGraph()
{
	m_ctrlEventGraph.Render();
}

void CObjectAnimationPage::RegisterObserver()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	pSceneObject->RegisterObserver (&m_AnimationObserver);
	m_AnimationObserver.SetOwner (this);
}

void CObjectAnimationPage::RecvLocalTime (float fDuration, float fLocalTime)
{
	int iMin, iMax;
	m_ctrlAnimationTimeBar.GetRange (iMin, iMax);

	int iPosition = int ((fLocalTime / fDuration) * float (iMax - iMin)) + iMin;
	m_ctrlAnimationTimeBar.SetPos (iPosition);
}
void CObjectAnimationPage::RecvPlay()
{
	CheckDlgButton (IDC_OBJECT_ANIMATION_PLAY, TRUE);
	GetDlgItem (IDC_OBJECT_ANIMATION_LOOP)->EnableWindow (FALSE);
	m_ctrlAnimationTimeBar.EnableWindow (FALSE);
}
void CObjectAnimationPage::RecvStop()
{
	CheckDlgButton (IDC_OBJECT_ANIMATION_PLAY, FALSE);
	GetDlgItem (IDC_OBJECT_ANIMATION_LOOP)->EnableWindow (TRUE);
	m_ctrlAnimationTimeBar.EnableWindow (TRUE);
}

///////////////////////////////////////
void CObjectAnimationPage::CAnimationObserver::SetOwner (CObjectAnimationPage * pOwner)
{
	m_pOwner = pOwner;
}
void CObjectAnimationPage::CAnimationObserver::SetLocalTime (float fDuration, float fLocalTime)
{
	m_pOwner->RecvLocalTime (fDuration, fLocalTime);
}
void CObjectAnimationPage::CAnimationObserver::Play (bool isLoop)
{
	m_pOwner->RecvPlay();
}
void CObjectAnimationPage::CAnimationObserver::Stop()
{
	m_pOwner->RecvStop();
}
///////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationPage message handlers

BOOL CObjectAnimationPage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD (wParam))
	{
		case IDC_OBJECT_ANIMATION_ATTACKING_BONE:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CSceneObject * pSceneObject = pApplication->GetSceneObject();
			CObjectData * pObjectData = pApplication->GetObjectData();
			std::string strBoneName;
			pSceneObject->GetBoneName (m_ctrlAttackingBone.GetCurSel(), &strBoneName);
			pObjectData->GetMotionDataPointer()->SetAttackingBoneName (m_dwcurHitDataIndex, strBoneName);
		}
		break;
		case IDC_OBJECT_ANIMATION_MOTION_TYPE:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();
			pObjectData->GetMotionDataPointer()->SetMotionType (m_ctrlMotionType.GetCurSel());
		}
		break;
		case IDC_OBJECT_ANIMATION_ATTACKING_TYPE:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();
			pObjectData->GetMotionDataPointer()->SetAttackType (m_ctrlAttackingType.GetCurSel());
		}
		break;
		case IDC_OBJECT_ANIMATION_HITTING_TYPE:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();
			pObjectData->GetMotionDataPointer()->SetHittingType (m_ctrlHittingType.GetCurSel());
		}
		break;
		case IDC_OBJECT_ANIMATION_STIFFEN_TIME:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();
			pObjectData->GetMotionDataPointer()->SetStiffenTime (GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_STIFFEN_TIME));
		}
		break;
		case IDC_OBJECT_ANIMATION_INVISIBLE_TIME:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();
			pObjectData->GetMotionDataPointer()->SetInvisibleTime (GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_INVISIBLE_TIME));
		}
		break;
		case IDC_OBJECT_ANIMATION_EXTERNAL_FORCE:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();
			pObjectData->GetMotionDataPointer()->SetExternalForceTime (GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EXTERNAL_FORCE));
		}
		break;
	}

	return CPageCtrl::OnCommand (wParam, lParam);
}

void CObjectAnimationPage::OnLoadAnimation()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	CHECK_RETURN (!pObjectData->isModelThing(), "Load the model first");

	DWORD dwFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Granny Animation Files (*.gr2)|*.gr2|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CHECK_RETURN (!CResourceManager::Instance().IsFileExist (FileOpener.GetPathName()), "There is no file");

		GetOnlyFileName (FileOpener.GetPathName(), m_strMotionFileName);

		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CObjectData * pObjectData = pApplication->GetObjectData();
		CSceneObject * pSceneObject = pApplication->GetSceneObject();

		if (pObjectData->SetMotionThing (FileOpener.GetPathName()) == false)
		{
			return;
		}

		pSceneObject->Refresh();

		CRaceMotionDataAccessor * pRaceMotionData = pObjectData->GetMotionDataPointer();
		pRaceMotionData->ClearAccessor();
		pRaceMotionData->SetMotionFileName (FileOpener.GetPathName());

		m_ctrlEventGraph.SetMotionData (pObjectData->GetMotionDataPointer());
		UpdateUI();
	}
}

void CObjectAnimationPage::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();
	CObjectData * pObjectData = pApplication->GetObjectData();

	float fSpeed = float (m_ctrlAnimationSpeedBar.GetPos()) / 100.0f;
	pSceneObject->SetMotionSpeed (fSpeed);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_SPEED_BAR_PRINT, fSpeed);

	int iWeaponLength = m_ctrlWeaponLength.GetPos();
	pObjectData->GetMotionDataPointer()->SetAttackingWeaponLength (m_dwcurHitDataIndex, float (iWeaponLength));
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_WEAPON_LENGTH_PRINT, iWeaponLength);

	int iHitLimitCount = m_ctrlHitLimit.GetPos();
	pObjectData->GetMotionDataPointer()->SetHitLimitCount (iHitLimitCount);
	SetDialogIntegerText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_HIT_LIMIT_PRINT, iHitLimitCount);

	CPageCtrl::OnHScroll (nSBCode, nPos, pScrollBar);
}

void CObjectAnimationPage::OnVScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	m_iTime = max (1, m_ctrlAnimationTimeBar.GetPos());
	pSceneObject->SetLocalPercentTime (float (m_iTime) / 100.0f);

	CPageCtrl::OnVScroll (nSBCode, nPos, pScrollBar);
}

void CObjectAnimationPage::OnPlay()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	if (pSceneObject->isPlay())
	{
		pSceneObject->Stop();
	}
	else
	{
		pSceneObject->Play (IsDlgButtonChecked (IDC_OBJECT_ANIMATION_LOOP) == 1 ? true : false);
	}
}

void CObjectAnimationPage::OnLoadAnimationScript()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Metin2 Animation Script Files (*.msa)|*.msa|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CObjectData * pObjectData = pApplication->GetObjectData();
		CSceneObject * pSceneObject = pApplication->GetSceneObject();
		CRaceMotionDataAccessor * pMotionData = pObjectData->GetMotionDataPointer();

		pMotionData->ClearAccessor();
		pMotionData->LoadMotionData (FileOpener.GetPathName());
		m_ctrlEventGraph.SetMotionData (pMotionData);

		//////////////////////////////////////////////////////

		CRaceMotionDataAccessor * pRaceMotionData = pObjectData->GetMotionDataPointer();

		/////
		GetOnlyFileName (FileOpener.GetPathName(), m_strScriptFileName);
		GetOnlyFileName (pRaceMotionData->GetMotionFileName(), m_strMotionFileName);
		/////

		if (strlen (pRaceMotionData->GetMotionFileName()) > 0)
		{
			if (pObjectData->SetMotionThing (pRaceMotionData->GetMotionFileName()) == false)
			{
				return;
			}
			pSceneObject->Refresh();
		}

		m_ctrlEventGraph.SetMotionData (pRaceMotionData);
		UpdateUI();
	}
}

void CObjectAnimationPage::OnSaveAnimationScript()
{
	std::string stScriptFileName;

	if (m_strScriptFileName.length() > 0)
	{
		GetOnlyFileName (m_strScriptFileName.c_str(), stScriptFileName);
	}
	else
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CObjectData * pObjectData = pApplication->GetObjectData();
		CRaceMotionDataAccessor * pRaceMotionData = pObjectData->GetMotionDataPointer();

		GetOnlyFileName (pRaceMotionData->GetMotionFileName(), stScriptFileName);

		int pos = stScriptFileName.find_last_of (".", stScriptFileName.length());
		stScriptFileName = stScriptFileName.substr (0, pos);
		stScriptFileName += ".msa";
	}

	DWORD dwFlag = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Metin2 Animation Script Files (*.msa)|*.msa|*.*|All Files (*.*)|";
	CSaveFileDialog FileOpener (this, dwFlag, NULL, c_szFilter, NULL, stScriptFileName.c_str());

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CObjectData * pObjectData = pApplication->GetObjectData();

		CRaceMotionDataAccessor * pMotionData = pObjectData->GetMotionDataPointer();
		pMotionData->SaveMotionData (FileOpener.GetPathName());
	}
}

void CObjectAnimationPage::OnCheckAttackingMotionEnable()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	CRaceMotionDataAccessor * pMotionData = pObjectData->GetMotionDataPointer();
	pMotionData->SetAttackingMotionEnable (IsDlgButtonChecked (IDC_OBJECT_ANIMATION_ATTACKING_ENABLE));
	m_ctrlEventGraph.Refresh();
	RenderEventGraph();

	if (pMotionData->isAttackingMotion())
	{
		GetDlgItem (IDC_OBJECT_ANIMATION_MOTION_TYPE)->EnableWindow (TRUE);
		GetDlgItem (IDC_OBJECT_ANIMATION_ATTACKING_TYPE)->EnableWindow (TRUE);
		GetDlgItem (IDC_OBJECT_ANIMATION_HITTING_TYPE)->EnableWindow (TRUE);
		GetDlgItem (IDC_OBJECT_ANIMATION_STIFFEN_TIME)->EnableWindow (TRUE);
		GetDlgItem (IDC_OBJECT_ANIMATION_INVISIBLE_TIME)->EnableWindow (TRUE);
		GetDlgItem (IDC_OBJECT_ANIMATION_EXTERNAL_FORCE)->EnableWindow (TRUE);
		GetDlgItem (IDC_OBJECT_ANIMATION_ATTACKING_BONE)->EnableWindow (TRUE);
		GetDlgItem (IDC_OBJECT_ANIMATION_HIT_DATA_INSERT)->EnableWindow (TRUE);
		GetDlgItem (IDC_OBJECT_ANIMATION_HIT_DATA_DELETE)->EnableWindow (TRUE);
		GetDlgItem (IDC_OBJECT_ANIMATION_HIT_LIMIT)->EnableWindow (TRUE);
		GetDlgItem (IDC_OBJECT_ANIMATION_HIT_LIMIT_PRINT)->EnableWindow (TRUE);

		BOOL bHitDataFlag = FALSE;
		if (pMotionData->GetHitDataCount() > 0)
		{
			bHitDataFlag = TRUE;
		}

		GetDlgItem (IDC_OBJECT_ANIMATION_HIT_DATA_LIST)->EnableWindow (bHitDataFlag);
		GetDlgItem (IDC_OBJECT_ANIMATION_ATTACKING_BONE)->EnableWindow (bHitDataFlag);
		GetDlgItem (IDC_OBJECT_ANIMATION_WEAPON_LENGTH)->EnableWindow (bHitDataFlag);
		GetDlgItem (IDC_OBJECT_ANIMATION_WEAPON_LENGTH_PRINT)->EnableWindow (bHitDataFlag);
	}
	else
	{
		GetDlgItem (IDC_OBJECT_ANIMATION_MOTION_TYPE)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_ATTACKING_TYPE)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_HITTING_TYPE)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_STIFFEN_TIME)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_INVISIBLE_TIME)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_EXTERNAL_FORCE)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_ATTACKING_BONE)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_HIT_DATA_LIST)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_ATTACKING_BONE)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_WEAPON_LENGTH)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_WEAPON_LENGTH_PRINT)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_HIT_DATA_INSERT)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_HIT_DATA_DELETE)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_HIT_LIMIT)->EnableWindow (FALSE);
		GetDlgItem (IDC_OBJECT_ANIMATION_HIT_LIMIT_PRINT)->EnableWindow (FALSE);
	}
}

void CObjectAnimationPage::OnCheckComboMotionEnable()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	CRaceMotionDataAccessor * pMotionData = pObjectData->GetMotionDataPointer();
	pMotionData->SetComboMotionEnable (IsDlgButtonChecked (IDC_OBJECT_ANIMATION_COMBO_MOTION_ENABLE));
	m_ctrlEventGraph.Refresh();
	RenderEventGraph();
}

void CObjectAnimationPage::OnCheckLoopMotionEnable()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	CRaceMotionDataAccessor * pMotionData = pObjectData->GetMotionDataPointer();
	pMotionData->SetLoopMotionEnable (IsDlgButtonChecked (IDC_OBJECT_ANIMATION_LOOP_MOTION_ENABLE));
	m_ctrlEventGraph.Refresh();
	RenderEventGraph();
}

void CObjectAnimationPage::OnCheckLimitLoopEnable()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	if (IsDlgButtonChecked (IDC_OBJECT_ANIMATION_LIMIT_LOOP_ENABLE))
	{
		int iLoopCount = GetDialogIntegerText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_LOOP_COUNT);
		GetDlgItem (IDC_OBJECT_ANIMATION_LOOP_COUNT)->EnableWindow (TRUE);
		pObjectData->GetMotionDataPointer()->SetLoopCount (iLoopCount);
	}
	else
	{
		SetDlgItemText (IDC_OBJECT_ANIMATION_LOOP_COUNT, "0");
		GetDlgItem (IDC_OBJECT_ANIMATION_LOOP_COUNT)->EnableWindow (FALSE);
		pObjectData->GetMotionDataPointer()->SetLoopCount (0);
	}
}

void CObjectAnimationPage::OnCancelEnable()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	CRaceMotionDataAccessor * pRaceMotionData = pObjectData->GetMotionDataPointer();
	pRaceMotionData->SetCancelEnable (IsDlgButtonChecked (IDC_OBJECT_ANIMATION_CANCEL_ENABLE_SKILL));
}

void CObjectAnimationPage::OnInsertHitData()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	CRaceMotionDataAccessor * pRaceMotionData = pObjectData->GetMotionDataPointer();
	pRaceMotionData->InsertHitData();

	__BuildHitDataList();
	__UpdateHitDataList();
	OnCheckAttackingMotionEnable();
	OnChangeHitData();
}

void CObjectAnimationPage::OnDeleteHitData()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	CRaceMotionDataAccessor * pRaceMotionData = pObjectData->GetMotionDataPointer();
	pRaceMotionData->DeleteHitData (m_dwcurHitDataIndex);

	__BuildHitDataList();
	__UpdateHitDataList();
	OnCheckAttackingMotionEnable();
	OnChangeHitData();
}

void CObjectAnimationPage::OnChangeHitData()
{
	m_dwcurHitDataIndex = DWORD (m_ctrlHitDataList.GetCurSel());
	m_ctrlEventGraph.SetCurrentHitDataIndex (m_ctrlHitDataList.GetCurSel());

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();
	pSceneObject->SetCurrentHitDataIndex (m_ctrlHitDataList.GetCurSel());

	__UpdateHitDataList();
}

void CObjectAnimationPage::OnChangeLoopCount()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	int iLoopCount = GetDialogIntegerText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_LOOP_COUNT);
	pObjectData->GetMotionDataPointer()->SetLoopCount (iLoopCount);
}

void CObjectAnimationPage::OnUnlimtedLoopCount()
{
	SetDialogIntegerText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_LOOP_COUNT, -1);
}

void CObjectAnimationPage::OnToggleFrameStep()
{
	if (IsDlgButtonChecked (IDC_OBJECT_ANIMATION_FRAME_STEP))
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CObjectData * pObjectData = pApplication->GetObjectData();
		float fDuration = pObjectData->GetMotionDataPointer()->GetMotionDuration();

		const float c_fFrameTime = 1.0f / 30.0f;
		int iStep = fDuration / c_fFrameTime;

		int iMin, iMax;
		m_ctrlAnimationTimeBar.GetRange (iMin, iMax);

		for (int i = 0; i < iStep; ++i)
		{
			int iPos = int (float (i) / float (iStep) * float (iMax - iMin)) + iMin;
			m_ctrlAnimationTimeBar.SetTic (iPos);
		}
	}
	else
	{
		m_ctrlAnimationTimeBar.ClearTics (TRUE);
	}

	/////

	m_ctrlEventGraph.ToggleFrameStep();
}

void CObjectAnimationPage::OnAddMotionEvent()
{
}

void CObjectAnimationPage::OnDeleteMotionEvent()
{
}
