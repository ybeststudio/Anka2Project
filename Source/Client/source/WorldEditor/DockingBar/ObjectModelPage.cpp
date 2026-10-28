// ObjectModelPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectModelPage.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectModelPage dialog


CObjectModelPage::CObjectModelPage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CObjectModelPage::IDD, pParent)
{
	m_dwcurAttachingDataIndex = 0;
	//{{AFX_DATA_INIT(CObjectModelPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectModelPage::DoDataExchange (CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectModelPage)
	DDX_Control (pDX, IDC_OBJECT_MODEL_TYPE, m_ctrlModelType);
	DDX_Control (pDX, IDC_OBJECT_MODEL_ATTACHING_DATA, m_ctrlAttachingData);
	DDX_Control (pDX, IDC_OBJECT_MODEL_ATTACHING_BONE, m_ctrlAttachingBone);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectModelPage, CPageCtrl)
	//{{AFX_MSG_MAP(CObjectModelPage)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_LOAD, OnLoadModel)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_SAVE_SCRIPT, OnSaveModelScript)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_LOAD_SCRIPT, OnLoadModelScript)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_MAKE_COLLISION_DATA, OnMakeCollisionData)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_SHOW_MAIN_CHARACTER, OnCheckShowingMainCharacterFlag)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_ATTACH_CLEAR_ALL, OnClearAllAttachingData)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_ATTACH_CLEAR_ONE, OnOnClearOneAttachingData)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_ATTACHING_BONE_ENABLE, OnCheckAttachingBoneEnable)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_SHOW_COLLISION_DATA, OnCheckShowingCollisionDataFlag)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_MAKE_OBJECT, OnMakeObject)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_MAKE_EFFECT, OnObjectModelMakeEffect)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_SAVE_ATTR, OnObjectModelSaveAttr)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_NEW, OnModelNew)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectModelPage normal functions

BOOL CObjectModelPage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CObjectModelPage::IDD, pParent))
	{
		return FALSE;
	}

	m_ctrlAttachingData.InsertString (0, "None");
	m_ctrlAttachingData.SelectString (0, "None");

	GetDlgItem (IDC_OBJECT_MODEL_ATTACHING_BONE_ENABLE)->EnableWindow (FALSE);
	GetDlgItem (IDC_OBJECT_MODEL_ATTACHING_BONE)->EnableWindow (FALSE);

	m_ctrlModelType.InsertString (CObjectData::MODEL_TYPE_CHARACTER, "Character");
	m_ctrlModelType.InsertString (CObjectData::MODEL_TYPE_ITEM, "Equipment");
	m_ctrlModelType.InsertString (CObjectData::MODEL_TYPE_OBJECT, "Object");
	m_ctrlModelType.SelectString (0, "Character");

	CreateModelTypePages();
	CreateAttachingPages();

	return TRUE;
}

BOOL CObjectModelPage::CreateModelTypePages()
{
	CRect Rect;
	GetDlgItem (IDC_OBJECT_MODEL_MODEL_DATA_PAGE)->GetWindowRect (&Rect);
	CObjectModelPage::ScreenToClient (&Rect);

	Rect.top += 18;

	if (!m_TypeCharacterSubPage.Create (this, Rect))
	{
		return FALSE;
	}
	if (!m_TypeEquipmentSubPage.Create (this, Rect))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CObjectModelPage::CreateAttachingPages()
{
	CRect Rect;
	GetDlgItem (IDC_OBJECT_MODEL_ATTACHING_DATA_PAGE)->GetWindowRect (&Rect);
	CObjectModelPage::ScreenToClient (&Rect);

	Rect.left -= 5;
	Rect.top -= 8;

	if (!m_AttachCollisionSubPage.Create (this, Rect))
	{
		return FALSE;
	}
	if (!m_AttachObjectSubPage.Create (this, Rect))
	{
		return FALSE;
	}
	if (!m_AttachEffectSubPage.Create (this, Rect))
	{
		return FALSE;
	}

	return TRUE;
}

void CObjectModelPage::UpdateAttachedDataComboBox()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	if (!pObjectData->isModelThing())
	{
		return;
	}

	m_ctrlAttachingData.ResetContent();

	if (0 == pObjectData->GetAttachingDataCount())
	{
		m_ctrlAttachingData.InsertString (0, "None");
		m_ctrlAttachingData.SelectString (0, "None");
		return;
	}

	DWORD dwAttachingDataCount = 0;
	char szAttachingDataName[32 + 1];

	for (DWORD i = 0; i < pObjectData->GetAttachingDataCount(); ++i)
	{
		NRaceData::TAttachingData * pAttachingData;
		if (!pObjectData->GetAttachingDataPointer (i, &pAttachingData))
		{
			m_ctrlAttachingData.InsertString (i, "Unknown");
			m_ctrlAttachingData.SelectString (i, "Unknown");
			continue;
		}

		switch (pAttachingData->dwType)
		{
			case NRaceData::ATTACHING_DATA_TYPE_COLLISION_DATA:
				_snprintf (szAttachingDataName, 32, "Index %d : CollisionData", dwAttachingDataCount++);
				break;
			case NRaceData::ATTACHING_DATA_TYPE_EFFECT:
				_snprintf (szAttachingDataName, 32, "Index %d : Effect", dwAttachingDataCount++);
				break;
			case NRaceData::ATTACHING_DATA_TYPE_OBJECT:
				_snprintf (szAttachingDataName, 32, "Index %d : Object", dwAttachingDataCount++);
				break;
			default:
				continue;
		}
		m_ctrlAttachingData.InsertString (i, szAttachingDataName);
		m_ctrlAttachingData.SelectString (i, szAttachingDataName);
	}
}

void CObjectModelPage::UpdateBoneComboBox()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	if (!pSceneObject->isModelData())
	{
		return;
	}

	m_ctrlAttachingBone.ResetContent();

	std::string strBoneName;
	for (DWORD i = 0; i < pSceneObject->GetBoneCount(); ++i)
	{
		pSceneObject->GetBoneName (i, &strBoneName);
		m_ctrlAttachingBone.InsertString (i, strBoneName.c_str());
		m_ctrlAttachingBone.SelectString (0, strBoneName.c_str());
	}
}

void CObjectModelPage::Initialize()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	CheckDlgButton (IDC_OBJECT_MODEL_SHOW_MAIN_CHARACTER, FALSE);
	CheckDlgButton (IDC_OBJECT_MODEL_SHOW_COLLISION_DATA, TRUE);
	OnCheckShowingMainCharacterFlag();
	OnCheckShowingCollisionDataFlag();
	SelectModelType (CObjectData::MODEL_TYPE_CHARACTER);
}

void CObjectModelPage::UpdateUI()
{
	UpdateAttachedDataComboBox();
	UpdateBoneComboBox();

	/////

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	CString strModelType;
	m_ctrlModelType.GetLBText (pObjectData->GetModelTypeReference(), strModelType);
	m_ctrlModelType.SelectString (0, strModelType);
	SelectModelType (m_ctrlModelType.GetCurSel());

	/////

	SelectAttachingDataPage (m_ctrlAttachingData.GetCurSel());
}

void CObjectModelPage::SelectModelType (DWORD dwModeIndex)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	pObjectData->GetModelTypeReference() = m_ctrlModelType.GetCurSel();

	/*
		switch (dwModeIndex)
		{
			case CObjectData::MODEL_TYPE_CHARACTER:
				m_TypeCharacterSubPage.UpdateUI();
				m_TypeCharacterSubPage.ShowWindow(TRUE);
				m_TypeEquipmentSubPage.ShowWindow(FALSE);
				break;
			case CObjectData::MODEL_TYPE_ITEM:
				m_TypeEquipmentSubPage.UpdateUI();
				m_TypeCharacterSubPage.ShowWindow(FALSE);
				m_TypeEquipmentSubPage.ShowWindow(TRUE);
				break;
			case CObjectData::MODEL_TYPE_OBJECT:
				m_TypeCharacterSubPage.ShowWindow(FALSE);
				m_TypeEquipmentSubPage.ShowWindow(FALSE);
				break;
		}
	*/
}

void CObjectModelPage::SelectAttachingDataPage (DWORD dwPageIndex)
{
	GetDlgItem (IDC_OBJECT_MODEL_ATTACHING_DATA_PAGE)->ShowWindow (FALSE);
	GetDlgItem (IDC_OBJECT_MODEL_ATTACHING_BONE_ENABLE)->EnableWindow (FALSE);
	GetDlgItem (IDC_OBJECT_MODEL_ATTACHING_BONE_ENABLE)->EnableWindow (FALSE);
	GetDlgItem (IDC_OBJECT_MODEL_ATTACHING_BONE)->EnableWindow (FALSE);

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	NRaceData::TAttachingData * pAttachingData;
	if (!pObjectData->GetAttachingDataPointer (dwPageIndex, &pAttachingData))
	{
		m_AttachCollisionSubPage.ShowWindow (FALSE);
		m_AttachObjectSubPage.ShowWindow (FALSE);
		m_AttachEffectSubPage.ShowWindow (FALSE);
		return;
	}

	pApplication->GetSceneObject()->SelectData (pAttachingData);

	m_dwcurAttachingDataIndex = dwPageIndex;

	// TODO : 0을 넣을 경우 0번의 문자열을 체크 못하는 경우가 있었음.
	//        문제가 생길려나..? [levites]
	if (-1 == m_ctrlAttachingBone.SelectString (-1, pAttachingData->strAttachingBoneName.c_str()))
	{
		// NOTE : If failed to find the bone name, then set the new bone name.
		if (m_ctrlAttachingBone.GetCount() > 0)
		{
			CString strNewBoneName;
			m_ctrlAttachingBone.GetLBText (0, strNewBoneName);
			pAttachingData->strAttachingBoneName = strNewBoneName;
		}
	}

	GetDlgItem (IDC_OBJECT_MODEL_ATTACHING_BONE_ENABLE)->EnableWindow (TRUE);
	CheckDlgButton (IDC_OBJECT_MODEL_ATTACHING_BONE_ENABLE, pAttachingData->isAttaching);
	GetDlgItem (IDC_OBJECT_MODEL_ATTACHING_BONE)->EnableWindow (pAttachingData->isAttaching);

	switch (pAttachingData->dwType)
	{
		case NRaceData::ATTACHING_DATA_TYPE_COLLISION_DATA:
			m_AttachCollisionSubPage.ShowWindow (TRUE);
			m_AttachObjectSubPage.ShowWindow (FALSE);
			m_AttachEffectSubPage.ShowWindow (FALSE);
			m_AttachCollisionSubPage.SetAttachingIndex (dwPageIndex);
			m_AttachCollisionSubPage.UpdateUI();
			break;
		case NRaceData::ATTACHING_DATA_TYPE_EFFECT:
			m_AttachCollisionSubPage.ShowWindow (FALSE);
			m_AttachObjectSubPage.ShowWindow (FALSE);
			m_AttachEffectSubPage.ShowWindow (TRUE);
			m_AttachEffectSubPage.SetAttachingIndex (dwPageIndex);
			m_AttachEffectSubPage.UpdateUI();
			break;
		case NRaceData::ATTACHING_DATA_TYPE_OBJECT:
			m_AttachCollisionSubPage.ShowWindow (FALSE);
			m_AttachObjectSubPage.ShowWindow (TRUE);
			m_AttachEffectSubPage.ShowWindow (FALSE);
			m_AttachObjectSubPage.SetAttachingIndex (dwPageIndex);
			m_AttachObjectSubPage.UpdateUI();
			break;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CObjectModelPage message handlers

BOOL CObjectModelPage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD (wParam))
	{
		case IDC_OBJECT_MODEL_ATTACHING_DATA:
			if (1 == HIWORD (wParam))
			{
				SelectAttachingDataPage (m_ctrlAttachingData.GetCurSel());
			}
			break;
		case IDC_OBJECT_MODEL_ATTACHING_BONE:
			if (1 == HIWORD (wParam))
			{
				CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
				CObjectData * pObjectData = pApplication->GetObjectData();
				CSceneObject * pSceneObject = pApplication->GetSceneObject();

				NRaceData::TAttachingData * pAttachingData;
				if (pObjectData->GetAttachingDataPointer (m_dwcurAttachingDataIndex, &pAttachingData))
				{
					CString strBoneName;
					m_ctrlAttachingBone.GetLBText (m_ctrlAttachingBone.GetCurSel(), strBoneName);
					pAttachingData->strAttachingBoneName = strBoneName;
					pSceneObject->RefreshAttachingData();
				}
			}
			break;
		case IDC_OBJECT_MODEL_TYPE:
			SelectModelType (m_ctrlModelType.GetCurSel());
			break;
	}

	return CPageCtrl::OnCommand (wParam, lParam);
}

void CObjectModelPage::OnLoadModel()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Granny Model Files (*.gr2)|*.gr2|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load Model Files", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CHECK_RETURN (!CResourceManager::Instance().IsFileExist (FileOpener.GetPathName()), "File not found.");

		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CObjectData * pObjectData = pApplication->GetObjectData();
		CSceneObject * pSceneObject = pApplication->GetSceneObject();

		pObjectData->Clear();
		if (pObjectData->SetModelThing (FileOpener.GetPathName()) == false)
		{
			pObjectData->Clear();
			return;
		}
		pSceneObject->Refresh();
		pSceneObject->FitCamera();
		UpdateUI();
	}
}

void CObjectModelPage::OnObjectModelSaveAttr()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();
	CFilename strModelFileName = pObjectData->GetModelFileName();
	strModelFileName = strModelFileName.NoPath();
	strModelFileName = strModelFileName.NoExtension();
	strModelFileName += ".txt";

	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Text Files (*.txt)|*.txt|*.*|All Files (*.*)|";
	CFileDialog FileOpener (FALSE, "Save Model Attribute Files", strModelFileName.c_str(), dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CSceneObject * pSceneObject = pApplication->GetSceneObject();
		pSceneObject->SaveAttrFile (FileOpener.GetPathName());
	}
}

void CObjectModelPage::OnLoadModelScript()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Model Script Files (*.msm)|*.msm|Text Files (*.txt)|*.txt|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load Model Script Files", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CObjectData * pObjectData = pApplication->GetObjectData();
		CSceneObject * pSceneObject = pApplication->GetSceneObject();

		pObjectData->Clear();
		m_strLoadedPath = FileOpener.GetPathName();
		if (pObjectData->LoadModelScript (FileOpener.GetPathName()))
		{
			pSceneObject->Refresh();
			pSceneObject->FitCamera();
			//StringPath(FileOpener.GetPathName());
			std::string temp = m_strLoadedPath;
			StringPath (temp);
			SetDlgItemText (IDC_OBJECT_MODEL_SCRIPT_NAME, RemoveStartString (temp, "D:\\Ymir Work\\").c_str());
		}

		pSceneObject->MovePosition (0.0f, 0.0f);

		UpdateUI();
	}
}

void CObjectModelPage::OnSaveModelScript()
{
	DWORD dwFlag = OFN_OVERWRITEPROMPT ;
	0 | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Model Script Files (*.msm)|*.msm|Text Files (*.txt)|*.txt|All Files (*.*)|*.*|";
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath (m_strLoadedPath.c_str(), 0, 0, fname, ext);
	std::string strFile = fname;
	strFile += ext;
	CSaveFileDialog FileOpener (this, dwFlag, NULL, c_szFilter, NULL, strFile.c_str());

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CObjectData * pObjectData = pApplication->GetObjectData();

		pObjectData->SaveModelScript (FileOpener.GetPathName());
		SetDlgItemText (IDC_OBJECT_MODEL_SCRIPT_NAME, RemoveStartString (FileOpener.GetPathName(), "D:\\Ymir Work\\").c_str());
	}
}

void CObjectModelPage::OnMakeCollisionData()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	DWORD dwIndex = pObjectData->CreateAttachingCollisionData();

	/////
	// Set Bone Name
	// NOTE : why? - [levites]
	NRaceData::TAttachingData * pAttachingData;
	if (pObjectData->GetAttachingDataPointer (m_dwcurAttachingDataIndex, &pAttachingData))
	{
		CString strBoneName;
		m_ctrlAttachingBone.GetLBText (0, strBoneName);
		pAttachingData->strAttachingBoneName = strBoneName;
	}
	/////

	pApplication->GetSceneObject()->BuildAttachingData();
	UpdateAttachedDataComboBox();
	SelectAttachingDataPage (dwIndex);
}

void CObjectModelPage::OnObjectModelMakeEffect()
{
	// TODO: Add your control notification handler code here
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	DWORD dwIndex = pObjectData->CreateAttachingEffectData();

	pApplication->GetSceneObject()->BuildAttachingData();
	UpdateAttachedDataComboBox();
	SelectAttachingDataPage (dwIndex);
}

void CObjectModelPage::OnMakeObject()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	DWORD dwIndex = pObjectData->CreateAttachingObjectData();

	pApplication->GetSceneObject()->BuildAttachingData();
	UpdateAttachedDataComboBox();
	SelectAttachingDataPage (dwIndex);
}

void CObjectModelPage::OnOnClearOneAttachingData()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	pObjectData->DeleteOneAttachingData (m_ctrlAttachingData.GetCurSel());

	pSceneObject->BuildAttachingData();
	UpdateAttachedDataComboBox();
	UpdateUI();
}

void CObjectModelPage::OnClearAllAttachingData()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	pObjectData->DeleteAllAttachingData();
	pSceneObject->BuildAttachingData();
	UpdateAttachedDataComboBox();
	UpdateUI();
}

void CObjectModelPage::OnCheckAttachingBoneEnable()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	BOOL isAttachingBone = IsDlgButtonChecked (IDC_OBJECT_MODEL_ATTACHING_BONE_ENABLE);

	NRaceData::TAttachingData * pAttachingData;
	if (pObjectData->GetAttachingDataPointer (m_ctrlAttachingData.GetCurSel(), &pAttachingData))
	{
		pAttachingData->isAttaching = isAttachingBone;
		pApplication->GetSceneObject()->RefreshAttachingData();
	}

	GetDlgItem (IDC_OBJECT_MODEL_ATTACHING_BONE)->EnableWindow (isAttachingBone);
}

void CObjectModelPage::OnCheckShowingMainCharacterFlag()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	pSceneObject->ShowMainCharacter (IsDlgButtonChecked (IDC_OBJECT_MODEL_SHOW_MAIN_CHARACTER));
}

void CObjectModelPage::OnCheckShowingCollisionDataFlag()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	pSceneObject->ShowCollisionData (IsDlgButtonChecked (IDC_OBJECT_MODEL_SHOW_COLLISION_DATA));
}

void CObjectModelPage::OnModelNew()
{
	OnLoadModel();
}
