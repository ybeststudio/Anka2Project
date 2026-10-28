// ObjectModelTypeEquipmentSubPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectModelTypeEquipmentSubPage.h"
#include "../../GameLib/ItemData.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectModelTypeEquipmentSubPage dialog


CObjectModelTypeEquipmentSubPage::CObjectModelTypeEquipmentSubPage (CWnd* pParent /*=NULL*/)
	: CDialog (CObjectModelTypeEquipmentSubPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectModelTypeEquipmentSubPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectModelTypeEquipmentSubPage::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectModelTypeEquipmentSubPage)
	DDX_Control (pDX, IDC_OBJECT_MODEL_EQUIPMENT_TYPE, m_ctrlEquipmentType);
	DDX_Control (pDX, IDC_OBJECT_MODEL_EQUIPMENT_EFFECT_LIST, m_ctrlEffectList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectModelTypeEquipmentSubPage, CDialog)
	//{{AFX_MSG_MAP(CObjectModelTypeEquipmentSubPage)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_EQUIPMENT_LOAD_SOUND, OnLoadSound)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_EQUIPMENT_CLEAR_ONE_EFFECT, OnClearOneEffect)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_EQUIPMENT_LOAD_EFFECT, OnLoadEffect)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_EQUIPMENT_LOAD_ICON_IMAGE, OnLoadIconImage)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_EQUIPMENT_LOAD_DROP_MODEL, OnLoadDropModel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectModelTypeEquipmentSubPage normal functions

BOOL CObjectModelTypeEquipmentSubPage::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectModelTypeEquipmentSubPage::IDD, pParent))
	{
		return FALSE;
	}

	m_ctrlEquipmentType.InsertString (CItemData::ITEM_TYPE_NONE, "None");
	m_ctrlEquipmentType.InsertString (CItemData::ITEM_TYPE_WEAPON, "Weapon");
	m_ctrlEquipmentType.InsertString (CItemData::ITEM_TYPE_ARMOR, "Armor");
	m_ctrlEquipmentType.SelectString (0, "Weapon");

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	return TRUE;
}


void CObjectModelTypeEquipmentSubPage::UpdateUI()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	CString strEquipmentType;
	m_ctrlEquipmentType.GetLBText (pObjectData->GetEquipmentTypeReference(), strEquipmentType);
	m_ctrlEquipmentType.SelectString (0, strEquipmentType);

	//	SetDlgItemText(IDC_OBJECT_MODEL_EQUIPMENT_SOUND_NAME, pObjectData->GetModelHitSoundFileNameReference().c_str());
	//SetDlgItemText(IDC_OBJECT_MODEL_EQUIPMENT_SOUND_NAME,
	//	RemoveStartString(pObjectData->GetModelHitSoundFileNameReference(),"D:\\Ymir Work\\sound/").c_str());
	SetDlgItemText (IDC_OBJECT_MODEL_EQUIPMENT_ICON_IMAGE_NAME, pObjectData->GetEquipmentIconImageFileNameReference().c_str());
	SetDlgItemText (IDC_OBJECT_MODEL_EQUIPMENT_DROP_MODEL_NAME, pObjectData->GetEquipmentDropModelFileNameReference().c_str());

	//	m_ctrlEffectList.ResetContent();
	//	for (DWORD i = 0; i < pObjectData->GetModelHitEffectCount(); ++i)
	//	{
	//		std::string * pstrName;
	//		if (pObjectData->GetModelHitEffectFileNamePointer(i, &pstrName))
	//		{
	//			m_ctrlEffectList.AddString(RemoveStartString(*pstrName,"D:\\Ymir Work\\effect/").c_str());
	//		}
	//	}
}

/////////////////////////////////////////////////////////////////////////////
// CObjectModelTypeEquipmentSubPage message handlers

void CObjectModelTypeEquipmentSubPage::OnOK()
{
}
void CObjectModelTypeEquipmentSubPage::OnCancel()
{
}

BOOL CObjectModelTypeEquipmentSubPage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD (wParam))
	{
		case IDC_OBJECT_MODEL_EQUIPMENT_TYPE:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();

			pObjectData->GetEquipmentTypeReference() = m_ctrlEquipmentType.GetCurSel();
		}
		break;
		case IDC_OBJECT_MODEL_EQUIPMENT_SOUND_NAME:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();

			if (pObjectData)
			{
				CString strSourceFileName;
				GetDlgItemText (IDC_OBJECT_MODEL_EQUIPMENT_SOUND_NAME, strSourceFileName);

				std::string strSoundFileName;
				StringPath (strSourceFileName, strSoundFileName);

				// 					pObjectData->GetModelHitSoundFileNameReference() = strSoundFileName;
			}
		}
		break;
		case IDC_OBJECT_MODEL_EQUIPMENT_ICON_IMAGE_NAME:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();

			CString strSourceFileName;
			GetDlgItemText (IDC_OBJECT_MODEL_EQUIPMENT_ICON_IMAGE_NAME, strSourceFileName);

			std::string strIconImageFileName;
			StringPath (strSourceFileName, strIconImageFileName);

			pObjectData->GetEquipmentIconImageFileNameReference() = strIconImageFileName;
		}
		break;
		case IDC_OBJECT_MODEL_EQUIPMENT_DROP_MODEL_NAME:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();

			CString strSourceFileName;
			GetDlgItemText (IDC_OBJECT_MODEL_EQUIPMENT_DROP_MODEL_NAME, strSourceFileName);

			std::string strDropModelFileName;
			StringPath (strSourceFileName, strDropModelFileName);

			pObjectData->GetEquipmentDropModelFileNameReference() = strDropModelFileName;
		}
		break;
	}

	return CDialog::OnCommand (wParam, lParam);
}

void CObjectModelTypeEquipmentSubPage::OnLoadSound()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Sound Files (*.wav)|*.wav|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		SetDlgItemText (IDC_OBJECT_MODEL_EQUIPMENT_SOUND_NAME, FileOpener.GetPathName());
	}
}

void CObjectModelTypeEquipmentSubPage::OnClearOneEffect()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	// 	pObjectData->DeleteModelHitEffect(m_ctrlEffectList.GetCurSel());
	UpdateUI();
}

void CObjectModelTypeEquipmentSubPage::OnLoadEffect()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Metin2 Effect Script Files (*.mse)|*.mse|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CObjectData * pObjectData = pApplication->GetObjectData();

		std::string strEffectFileName;
		StringPath (FileOpener.GetPathName(), strEffectFileName);

		//		pObjectData->InsertModelHitEffect(strEffectFileName.c_str());
		UpdateUI();
	}
}

void CObjectModelTypeEquipmentSubPage::OnLoadIconImage()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Metin2 SubImage Files (*.sub)|*.sub|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		SetDlgItemText (IDC_OBJECT_MODEL_EQUIPMENT_ICON_IMAGE_NAME, FileOpener.GetPathName());
	}
}

void CObjectModelTypeEquipmentSubPage::OnLoadDropModel()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Granny Model Files (*.gr2)|*.gr2|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		SetDlgItemText (IDC_OBJECT_MODEL_EQUIPMENT_DROP_MODEL_NAME, FileOpener.GetPathName());
	}
}
