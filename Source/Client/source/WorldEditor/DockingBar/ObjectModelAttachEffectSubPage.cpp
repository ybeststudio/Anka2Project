// ObjectModelAttachEffectSubPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "objectmodelattacheffectsubpage.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectModelAttachEffectSubPage dialog


CObjectModelAttachEffectSubPage::CObjectModelAttachEffectSubPage (CWnd* pParent /*=NULL*/)
	: CDialog (CObjectModelAttachEffectSubPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectModelAttachEffectSubPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectModelAttachEffectSubPage::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectModelAttachEffectSubPage)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectModelAttachEffectSubPage, CDialog)
	//{{AFX_MSG_MAP(CObjectModelAttachEffectSubPage)
	ON_BN_CLICKED (IDC_EFFECT_PAGE_LOAD_EFFECT, OnEffectPageLoadEffect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectModelAttachEffectSubPage message handlers

BOOL CObjectModelAttachEffectSubPage::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectModelAttachEffectSubPage::IDD, pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	return TRUE;
}

void CObjectModelAttachEffectSubPage::SetAttachingIndex (DWORD dwIndex)
{
	m_dwAttachingIndex = dwIndex;
}

void CObjectModelAttachEffectSubPage::UpdateUI()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	NRaceData::TAttachingData * pAttachingData;
	if (pObjectData->GetObjectDataPointer (m_dwAttachingIndex, &pAttachingData))
	{
		SetDlgItemText (IDC_EFFECT_PAGE_EFFECT_FILE, pAttachingData->pEffectData->strFileName.c_str());
	}
}

void CObjectModelAttachEffectSubPage::OnEffectPageLoadEffect()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Metin2 Effect Script Files (*.mse)|*.mse|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CObjectData * pObjectData = pApplication->GetObjectData();
		CSceneObject * pSceneObject = pApplication->GetSceneObject();

		NRaceData::TAttachingData * pAttachingData;
		if (pObjectData->GetObjectDataPointer (m_dwAttachingIndex, &pAttachingData))
		{
			pAttachingData->pEffectData->strFileName = FileOpener.GetPathName();
		}

		SetDlgItemText (IDC_EFFECT_PAGE_EFFECT_FILE, FileOpener.GetPathName());

		CEffectManager::Instance().RegisterEffect (FileOpener.GetPathName(), true);

		pSceneObject->BuildAttachingData();
	}
}
