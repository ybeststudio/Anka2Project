// ObjectModelAttachObjectSubPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectModelAttachObjectSubPage.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectModelAttachObjectSubPage dialog


CObjectModelAttachObjectSubPage::CObjectModelAttachObjectSubPage (CWnd* pParent /*=NULL*/)
	: CDialog (CObjectModelAttachObjectSubPage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectModelAttachObjectSubPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectModelAttachObjectSubPage::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectModelAttachObjectSubPage)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectModelAttachObjectSubPage, CDialog)
	//{{AFX_MSG_MAP(CObjectModelAttachObjectSubPage)
	ON_BN_CLICKED (IDC_OBJECT_PAGE_LOAD_MODEL, OnLoadModel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectModelAttachObjectSubPage normal functions

BOOL CObjectModelAttachObjectSubPage::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectModelAttachObjectSubPage::IDD, pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	return TRUE;
}

void CObjectModelAttachObjectSubPage::SetAttachingIndex (DWORD dwIndex)
{
	m_dwAttachingIndex = dwIndex;
}

void CObjectModelAttachObjectSubPage::UpdateUI()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	NRaceData::TAttachingData * pAttachingData;
	if (pObjectData->GetObjectDataPointer (m_dwAttachingIndex, &pAttachingData))
	{
		SetDlgItemText (IDD_OBJECT_PAGE_MODEL_FILE, pAttachingData->pObjectData->strFileName.c_str());
	}
}

/////////////////////////////////////////////////////////////////////////////
// CObjectModelAttachObjectSubPage message handlers

void CObjectModelAttachObjectSubPage::OnLoadModel()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Metin2 Model Script Files (*.gr2)|*.gr2|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CObjectData * pObjectData = pApplication->GetObjectData();
		CSceneObject * pSceneObject = pApplication->GetSceneObject();

		NRaceData::TAttachingData * pAttachingData;
		if (pObjectData->GetObjectDataPointer (m_dwAttachingIndex, &pAttachingData))
		{
			pAttachingData->pObjectData->strFileName = FileOpener.GetPathName();
		}

		SetDlgItemText (IDD_OBJECT_PAGE_MODEL_FILE, FileOpener.GetPathName());
		pSceneObject->BuildAttachingData();
	}
}
