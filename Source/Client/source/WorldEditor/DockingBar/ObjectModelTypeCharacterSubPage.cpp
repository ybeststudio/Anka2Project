// CObjectModelTypeCharacterSubPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectModelTypeCharacterSubPage.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectModelTypeCharacterSubPage dialog


CObjectModelTypeCharacterSubPage::CObjectModelTypeCharacterSubPage (CWnd* pParent /*=NULL*/)
	: CDialog (CObjectModelTypeCharacterSubPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectModelTypeCharacterSubPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectModelTypeCharacterSubPage::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectModelTypeCharacterSubPage)
	DDX_Control (pDX, IDC_OBJECT_MODEL_CHARACTER_EFFECT_NAME_LIST, m_ctrlEffectList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectModelTypeCharacterSubPage, CDialog)
	//{{AFX_MSG_MAP(CObjectModelTypeCharacterSubPage)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_CHARACTER_LOAD_SOUND, OnLoadHittingSound)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_CHARACTER_LOAD_EFFECT, OnLoadHittingEffect)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_CHARACTER_HIT_TEST, OnHitTest)
	ON_BN_CLICKED (IDC_OBJECT_MODEL_CHARACTER_CLEAR_ONE_EFFECT, OnClearOneEffect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectModelTypeCharacterSubPage normal functions

BOOL CObjectModelTypeCharacterSubPage::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectModelTypeCharacterSubPage::IDD, pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	return TRUE;
}

void CObjectModelTypeCharacterSubPage::UpdateUI()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	SetDialogIntegerText (GetSafeHwnd(), IDC_OBJECT_MODEL_CHARACTER_PART_COUNT, pObjectData->GetModelPartCountReference());
	//	SetDlgItemText(IDC_OBJECT_MODEL_CHARACTER_SOUND_NAME, pObjectData->GetModelHitSoundFileNameReference().c_str());

	//	m_ctrlEffectList.ResetContent();
	//	for (DWORD i = 0; i < pObjectData->GetModelHitEffectCount(); ++i)
	//	{
	//		std::string * pstrName;
	//		if (pObjectData->GetModelHitEffectFileNamePointer(i, &pstrName))
	//			m_ctrlEffectList.AddString(RemoveStartString(*pstrName,"D:\\Ymir Work\\effect/").c_str());
	////			m_ctrlEffectList.AddString(pstrName->c_str());
	//	}
}

/////////////////////////////////////////////////////////////////////////////
// CObjectModelTypeCharacterSubPage message handlers

void CObjectModelTypeCharacterSubPage::OnOK()
{
}
void CObjectModelTypeCharacterSubPage::OnCancel()
{
}

BOOL CObjectModelTypeCharacterSubPage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD (wParam))
	{
		case IDC_OBJECT_MODEL_CHARACTER_PART_COUNT:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();

			if (pObjectData)
			{
				CString strPartCound;
				GetDlgItemText (IDC_OBJECT_MODEL_CHARACTER_PART_COUNT, strPartCound);

				pObjectData->GetModelPartCountReference() = atoi (strPartCound);
			}
		}
		break;
		case IDC_OBJECT_MODEL_CHARACTER_SOUND_NAME:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();

			CString strSourceFileName;
			GetDlgItemText (IDC_OBJECT_MODEL_CHARACTER_SOUND_NAME, strSourceFileName);

			std::string strSoundFileName;
			StringPath (strSourceFileName, strSoundFileName);

			//				pObjectData->GetModelHitSoundFileNameReference() = strSoundFileName;
		}
		break;
		case IDC_OBJECT_MODEL_EQUIPMENT_EFFECT_LIST:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CObjectData * pObjectData = pApplication->GetObjectData();

			//				std::string * pstrEffectFileName;
			//				if (pObjectData->GetModelHitEffectFileNamePointer(m_ctrlEffectList.GetCurSel(), &pstrEffectFileName))
			//				{
			//					Tracef(" -----> %s\n", pstrEffectFileName->c_str());
			//				}
		}
		break;
	}

	return CDialog::OnCommand (wParam, lParam);
}

void CObjectModelTypeCharacterSubPage::OnLoadHittingSound()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Sound Files (*.wav)|*.wav|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		SetDlgItemText (IDC_OBJECT_MODEL_CHARACTER_SOUND_NAME, FileOpener.GetPathName());
	}
}

void CObjectModelTypeCharacterSubPage::OnClearOneEffect()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	//	pObjectData->DeleteModelHitEffect(m_ctrlEffectList.GetCurSel());
	UpdateUI();
}

void CObjectModelTypeCharacterSubPage::OnLoadHittingEffect()
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

void CObjectModelTypeCharacterSubPage::OnHitTest()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	//	pSceneObject->HitTest();
}
