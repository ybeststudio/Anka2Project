// MapPortalDialog.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapPortalDialog.h"
#include "../DataCtrl/MapManagerAccessor.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapPortalDialog dialog


CMapPortalDialog::CMapPortalDialog (CWnd* pParent /*=NULL*/)
	: CDialog (CMapPortalDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapPortalDialog)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pAccessor = NULL;
	m_iEnableCheckCallBack = TRUE;
}


void CMapPortalDialog::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapPortalDialog)
	DDX_Control (pDX, IDC_PORTAL_LIST, m_ctrlPortalList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CMapPortalDialog, CDialog)
	//{{AFX_MSG_MAP(CMapPortalDialog)
	ON_NOTIFY (LVN_ITEMCHANGED, IDC_PORTAL_LIST, OnItemchangedPortalList)
	ON_BN_CLICKED (IDC_PORTAL_SHOW_ALL, OnPortalShowAll)
	ON_BN_CLICKED (IDC_PORTAL_HIDE_ALL, OnPortalHideAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapPortalDialog normal functions

void CMapPortalDialog::SetMapManagerHandler (CMapManagerAccessor * pAccessor)
{
	m_pAccessor = pAccessor;

	__RefreshPortalNumberList();
}

void CMapPortalDialog::__RefreshPortalNumberList()
{
	m_iEnableCheckCallBack = FALSE;

	std::set<int> kSet_iPortalNumber;
	m_pAccessor->CollectPortalNumber (&kSet_iPortalNumber);

	int iIndex = 0;
	BOOL bCheck = FALSE;

	m_ctrlPortalList.DeleteAllItems();
	m_kSet_iShowingPortalID.clear();
	m_kVec_iPortalID.clear();
	m_kVec_iPortalID.resize (kSet_iPortalNumber.size());

	std::set<int>::iterator itor = kSet_iPortalNumber.begin();
	for (; itor != kSet_iPortalNumber.end(); ++itor)
	{
		int iID = *itor;
		m_kVec_iPortalID[iIndex] = iID;

		m_ctrlPortalList.InsertItem (iIndex, _getf ("Portal Number : %d", iID));
		ListView_SetItemState (m_ctrlPortalList.GetSafeHwnd(), iIndex, UINT ((int (bCheck) + 1) << 12), LVIS_STATEIMAGEMASK);

		++iIndex;
	}

	m_iEnableCheckCallBack = TRUE;
}

void CMapPortalDialog::__ShowAll()
{
	for (DWORD i = 0; i < m_kVec_iPortalID.size(); ++i)
	{
		int iID = m_kVec_iPortalID[i];
		m_kSet_iShowingPortalID.insert (iID);
	}
	__RefreshPortalIDList();
	m_pAccessor->RefreshPortal();
}

void CMapPortalDialog::__HideAll()
{
	m_kSet_iShowingPortalID.clear();
	m_pAccessor->ClearPortal();
	m_pAccessor->RefreshPortal();
}

void CMapPortalDialog::__RefreshPortalIDList()
{
	m_pAccessor->ClearPortal();
	std::set<int>::iterator itor = m_kSet_iShowingPortalID.begin();
	for (; itor != m_kSet_iShowingPortalID.end(); ++itor)
	{
		int iID = *itor;
		m_pAccessor->AddShowingPortalID (iID);
	}
	m_pAccessor->RefreshPortal();
}

/////////////////////////////////////////////////////////////////////////////
// CMapPortalDialog message handlers

BOOL CMapPortalDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	ListView_SetExtendedListViewStyle (m_ctrlPortalList.GetSafeHwnd(), LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

	BOOL bCheck = 1;

	m_ctrlPortalList.InsertColumn (0, "Portal Number", LVCFMT_LEFT, 500);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMapPortalDialog::OnItemchangedPortalList (NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	// No change
	if (pNMListView->uOldState == 0 && pNMListView->uNewState == 0)
	{
		return;
	}

	// Old check box state
	BOOL bPrevState = (BOOL) (((pNMListView->uOldState & LVIS_STATEIMAGEMASK) >> 12) -1);
	if (bPrevState < 0) // On startup there's no previous state
	{
		bPrevState = 0;    // so assign as false (unchecked)
	}

	// New check box state
	BOOL bChecked = (BOOL) (((pNMListView->uNewState & LVIS_STATEIMAGEMASK) >> 12) -1);
	if (bChecked < 0) // On non-checkbox notifications assume false
	{
		bChecked = 0;
	}

	if (bPrevState == bChecked) // No change in check box
	{
		return;
	}

	if (!m_iEnableCheckCallBack)
	{
		return;
	}

	int iListIndex = pNMListView->iItem;
	if (iListIndex >= int (m_kVec_iPortalID.size()))
	{
		return;
	}

	int iID = m_kVec_iPortalID[iListIndex];

	if (bChecked)
	{
		m_kSet_iShowingPortalID.insert (iID);
	}
	else
	{
		m_kSet_iShowingPortalID.erase (iID);
	}

	__RefreshPortalIDList();
}

void CMapPortalDialog::OnPortalShowAll()
{
	m_iEnableCheckCallBack = FALSE;

	for (DWORD i = 0; i < m_kVec_iPortalID.size(); ++i)
	{
		ListView_SetItemState (m_ctrlPortalList.GetSafeHwnd(), i, UINT ((int (TRUE) + 1) << 12), LVIS_STATEIMAGEMASK);
	}

	__ShowAll();

	m_iEnableCheckCallBack = TRUE;
}

void CMapPortalDialog::OnPortalHideAll()
{
	m_iEnableCheckCallBack = FALSE;

	for (DWORD i = 0; i < m_kVec_iPortalID.size(); ++i)
	{
		ListView_SetItemState (m_ctrlPortalList.GetSafeHwnd(), i, UINT ((int (FALSE) + 1) << 12), LVIS_STATEIMAGEMASK);
	}

	__HideAll();

	m_iEnableCheckCallBack = TRUE;
}
