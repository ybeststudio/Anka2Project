// ObjectModelCollisionSubPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectModelCollisionSubPage.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectModelCollisionSubPage dialog


CObjectModelCollisionSubPage::CObjectModelCollisionSubPage (CWnd* pParent /*=NULL*/)
	: CDialog (CObjectModelCollisionSubPage::IDD, pParent)
{
	m_pParent = NULL;
	m_dwAttachingIndex = 0;
	m_dwSphereIndex = 0;

	//{{AFX_DATA_INIT(CObjectModelCollisionSubPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectModelCollisionSubPage::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectModelCollisionSubPage)
	DDX_Control (pDX, IDC_COLLISION_PAGE_DATA_LIST, m_ctrlSphereList);
	DDX_Control (pDX, IDC_COLLISION_PAGE_SIZE, m_ctrlSphereSize);
	//}}AFX_DATA_MAP

	m_ctrlSphereSize.SetRangeMin (1);
	m_ctrlSphereSize.SetRangeMax (1200);

	for (int i = 50; i < 1200; i += 50)
	{
		m_ctrlSphereSize.SetTic (i);
	}
}


BEGIN_MESSAGE_MAP (CObjectModelCollisionSubPage, CDialog)
	//{{AFX_MSG_MAP(CObjectModelCollisionSubPage)
	ON_BN_CLICKED (IDC_COLLISION_PAGE_TYPE_BODY, OnCheckTypeBody)
	ON_BN_CLICKED (IDC_COLLISION_PAGE_TYPE_DEFENDING, OnCheckTypeDefending)
	ON_WM_HSCROLL()
	ON_BN_CLICKED (IDC_COLLISION_PAGE_INSERT_DATA, OnInsertCollisionData)
	ON_BN_CLICKED (IDC_COLLISION_PAGE_DELETE_ONE_DATA, OnDeleteOneCollisionData)
	ON_BN_CLICKED (IDC_COLLISION_PAGE_SIZE_TYPE_1, OnCollisionSizeType1)
	ON_BN_CLICKED (IDC_COLLISION_PAGE_SIZE_TYPE_2, OnCollisionSizeType2)
	ON_BN_CLICKED (IDC_COLLISION_PAGE_SIZE_TYPE_3, OnCollisionSizeType3)
	ON_BN_CLICKED (IDC_COLLISION_PAGE_SIZE_TYPE_4, OnCollisionSizeType4)
	ON_BN_CLICKED (IDC_COLLISION_PAGE_SIZE_TYPE_5, OnCollisionSizeType5)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectModelCollisionSubPage normal functions

BOOL CObjectModelCollisionSubPage::Create (CObjectModelPage * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectModelCollisionSubPage::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	m_pParent = pParent;

	OnCheckTypeBody();
	m_ctrlSphereSize.SetPos (30);

	return TRUE;
}

void CObjectModelCollisionSubPage::SetAttachingIndex (DWORD dwIndex)
{
	m_dwAttachingIndex = dwIndex;
}

void CObjectModelCollisionSubPage::UpdateUI()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	CheckDlgButton (IDC_COLLISION_PAGE_TYPE_BODY, FALSE);
	CheckDlgButton (IDC_COLLISION_PAGE_TYPE_ATTACKING, FALSE);
	CheckDlgButton (IDC_COLLISION_PAGE_TYPE_DEFENDING, FALSE);

	NRaceData::TAttachingData * pAttachingData;
	if (!pObjectData->GetCollisionDataPointer (m_dwAttachingIndex, &pAttachingData))
	{
		return;
	}

	NRaceData::TCollisionData * pCollisionData = pAttachingData->pCollisionData;
	switch (pCollisionData->iCollisionType)
	{
		case NRaceData::COLLISION_TYPE_BODY:
			OnCheckTypeBody();
			break;
		case NRaceData::COLLISION_TYPE_ATTACKING:
			assert (!"Collision attack type no longer exists.");
			break;
		case NRaceData::COLLISION_TYPE_DEFENDING:
			OnCheckTypeDefending();
			break;
	}

	m_ctrlSphereList.ResetContent();
	for (DWORD i = 0; i < pCollisionData->SphereDataVector.size(); ++i)
	{
		char szCollisionDataName[32 + 1];
		_snprintf (szCollisionDataName, 32, "CollisionData %02d", i);
		m_ctrlSphereList.InsertString (i, szCollisionDataName);
		m_ctrlSphereList.SelectString (0, szCollisionDataName);
		m_dwSphereIndex = i;
	}

	UpdateSphereData();
}

void CObjectModelCollisionSubPage::UpdateSphereData()
{
	TSphereData * pSphereData;
	if (!GetSphereDataPointer (m_dwAttachingIndex, m_dwSphereIndex, &pSphereData))
	{
		return;
	}

	m_ctrlSphereSize.SetPos (int (pSphereData->fRadius));
	SetDialogIntegerText (GetSafeHwnd(), IDC_COLLISION_PAGE_SIZE_PRINT, int (pSphereData->fRadius));

	SetDialogFloatText (GetSafeHwnd(), IDC_COLLISION_PAGE_POSITION_X, pSphereData->v3Position.x);
	SetDialogFloatText (GetSafeHwnd(), IDC_COLLISION_PAGE_POSITION_Y, pSphereData->v3Position.y);
	SetDialogFloatText (GetSafeHwnd(), IDC_COLLISION_PAGE_POSITION_Z, pSphereData->v3Position.z);
}

BOOL CObjectModelCollisionSubPage::GetCollisionDataPointer (DWORD dwAttachingIndex, NRaceData::TCollisionData** ppCollisionData)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();
	if (!pObjectData)
	{
		return FALSE;
	}

	NRaceData::TAttachingData * pAttachingData;
	if (!pObjectData->GetCollisionDataPointer (dwAttachingIndex, &pAttachingData))
	{
		return FALSE;
	}

	*ppCollisionData = pAttachingData->pCollisionData;

	return TRUE;
}

BOOL CObjectModelCollisionSubPage::GetSphereDataPointer (DWORD dwAttachingIndex, DWORD dwSphereIndex, TSphereData** ppSphereData)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CObjectData * pObjectData = pApplication->GetObjectData();

	if (!pObjectData)
	{
		return FALSE;
	}

	NRaceData::TAttachingData * pAttachingData;

	if (!pObjectData->GetCollisionDataPointer (dwAttachingIndex, &pAttachingData))
	{
		return FALSE;
	}

	NRaceData::TCollisionData * pCollisionData = pAttachingData->pCollisionData;

	if (dwSphereIndex >= pCollisionData->SphereDataVector.size())
	{
		return FALSE;
	}

	*ppSphereData = &pCollisionData->SphereDataVector[dwSphereIndex].GetAttribute();
	return TRUE;
}

void CObjectModelCollisionSubPage::SetSphereSize (DWORD dwAttachingIndex, DWORD dwSphereIndex, int iSize)
{
	TSphereData * pSphereData;
	if (GetSphereDataPointer (dwAttachingIndex, dwSphereIndex, &pSphereData))
	{
		pSphereData->fRadius = iSize;
		SetDialogIntegerText (GetSafeHwnd(), IDC_COLLISION_PAGE_SIZE_PRINT, iSize);
	}
}

void CObjectModelCollisionSubPage::SetSphereSizeType (DWORD dwAttachingIndex, DWORD dwSphereIndex, DWORD dwSizeType)
{
	assert (dwSizeType < 5);

	const int c_iBodyCollisionSizeType[5] =
	{
		40, 60, 80, 100, 120
	};
	const int c_iDefendingCollisionSizeType[5] =
	{
		50, 70, 90, 110, 130
	};

	NRaceData::TCollisionData * pCollisionData;
	if (!GetCollisionDataPointer (m_dwAttachingIndex, &pCollisionData))
	{
		return;
	}

	int iSize = 0;
	switch (pCollisionData->iCollisionType)
	{
		case NRaceData::COLLISION_TYPE_BODY:
			iSize = c_iBodyCollisionSizeType[dwSizeType];
			break;

		case NRaceData::COLLISION_TYPE_DEFENDING:
			iSize = c_iDefendingCollisionSizeType[dwSizeType];
			break;

		default:
			assert (!"CObjectModelCollisionSubPage::SetSphereSizeType - Invalid/Unsupported type specified.");
			break;
	}

	SetSphereSize (m_dwAttachingIndex, m_dwSphereIndex, iSize);
	m_ctrlSphereSize.SetPos (iSize);
}

/////////////////////////////////////////////////////////////////////////////
// CObjectModelCollisionSubPage message handlers

void CObjectModelCollisionSubPage::OnOK()
{
}

void CObjectModelCollisionSubPage::OnCancel()
{
}

BOOL CObjectModelCollisionSubPage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	TSphereData * pSphereData;
	if (GetSphereDataPointer (m_dwAttachingIndex, m_dwSphereIndex, &pSphereData))
		switch (LOWORD (wParam))
		{
			case IDC_COLLISION_PAGE_POSITION_X:
				pSphereData->v3Position.x = GetDialogFloatText (GetSafeHwnd(), IDC_COLLISION_PAGE_POSITION_X);
				break;
			case IDC_COLLISION_PAGE_POSITION_Y:
				pSphereData->v3Position.y = GetDialogFloatText (GetSafeHwnd(), IDC_COLLISION_PAGE_POSITION_Y);
				break;
			case IDC_COLLISION_PAGE_POSITION_Z:
				pSphereData->v3Position.z = GetDialogFloatText (GetSafeHwnd(), IDC_COLLISION_PAGE_POSITION_Z);
				break;
			case IDC_COLLISION_PAGE_DATA_LIST:
				m_dwSphereIndex = m_ctrlSphereList.GetCurSel();
				UpdateSphereData();
				break;
		}

	return CDialog::OnCommand (wParam, lParam);
}

void CObjectModelCollisionSubPage::OnCheckTypeBody()
{
	NRaceData::TCollisionData * pCollisionData;
	if (GetCollisionDataPointer (m_dwAttachingIndex, &pCollisionData))
	{
		CheckDlgButton (IDC_COLLISION_PAGE_TYPE_BODY, TRUE);
		pCollisionData->iCollisionType = NRaceData::COLLISION_TYPE_BODY;
	}
}

void CObjectModelCollisionSubPage::OnCheckTypeDefending()
{
	NRaceData::TCollisionData * pCollisionData;
	if (GetCollisionDataPointer (m_dwAttachingIndex, &pCollisionData))
	{
		CheckDlgButton (IDC_COLLISION_PAGE_TYPE_DEFENDING, TRUE);
		pCollisionData->iCollisionType = NRaceData::COLLISION_TYPE_DEFENDING;
	}
}

void CObjectModelCollisionSubPage::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int iSize = m_ctrlSphereSize.GetPos();
	SetSphereSize (m_dwAttachingIndex, m_dwSphereIndex, iSize);

	CDialog::OnHScroll (nSBCode, nPos, pScrollBar);
}

void CObjectModelCollisionSubPage::OnInsertCollisionData()
{
	NRaceData::TCollisionData * pCollisionData;

	if (GetCollisionDataPointer (m_dwAttachingIndex, &pCollisionData))
	{
		CSphereCollisionInstance SphereData;
		TSphereData & r = SphereData.GetAttribute();

		r.fRadius = 50.0f;
		r.v3Position = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);

		pCollisionData->SphereDataVector.push_back (SphereData);
	}

	UpdateUI();
}

void CObjectModelCollisionSubPage::OnDeleteOneCollisionData()
{
	NRaceData::TCollisionData * pCollisionData;
	if (GetCollisionDataPointer (m_dwAttachingIndex, &pCollisionData))
		if (pCollisionData->SphereDataVector.size() > 1)
		{
			DeleteVectorItem<CSphereCollisionInstance> (&pCollisionData->SphereDataVector, m_dwSphereIndex);
			UpdateUI();
		}
}

void CObjectModelCollisionSubPage::OnCollisionSizeType1()
{
	SetSphereSizeType (m_dwAttachingIndex, m_dwSphereIndex, 0);
}

void CObjectModelCollisionSubPage::OnCollisionSizeType2()
{
	SetSphereSizeType (m_dwAttachingIndex, m_dwSphereIndex, 1);
}

void CObjectModelCollisionSubPage::OnCollisionSizeType3()
{
	SetSphereSizeType (m_dwAttachingIndex, m_dwSphereIndex, 2);
}

void CObjectModelCollisionSubPage::OnCollisionSizeType4()
{
	SetSphereSizeType (m_dwAttachingIndex, m_dwSphereIndex, 3);
}

void CObjectModelCollisionSubPage::OnCollisionSizeType5()
{
	SetSphereSizeType (m_dwAttachingIndex, m_dwSphereIndex, 4);
}
