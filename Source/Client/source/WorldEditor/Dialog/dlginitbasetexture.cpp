// dlginitbasetexture.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "dlginitbasetexture.h"
#include "../DataCtrl/MapAccessorTerrain.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInitBaseTexture dialog


CDlgInitBaseTexture::CDlgInitBaseTexture (CWnd* pParent /*=NULL*/)
	: CDialog (CDlgInitBaseTexture::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInitBaseTexture)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgInitBaseTexture::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CDlgInitBaseTexture)
	DDX_Control (pDX, IDC_LIST_TEXTURE, m_ListTexture);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CDlgInitBaseTexture, CDialog)
	//{{AFX_MSG_MAP(CDlgInitBaseTexture)
	ON_NOTIFY (LVN_ITEMCHANGED, IDC_LIST_TEXTURE, OnItemchangedListTexture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInitBaseTexture message handlers

void CDlgInitBaseTexture::OnOK()
{
	// TODO: Add extra validation here

	if (!m_TextureNumberVector.empty())
	{
		InitTexture();
	}
	EndDialog (TRUE);
}

BOOL CDlgInitBaseTexture::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	LoadList();
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgInitBaseTexture::LoadList()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	CTextureSet * pTextureSet = CTerrain::GetTextureSet();

	m_ListTexture.SetRedraw (FALSE);
	m_ListTexture.DeleteAllItems();
	m_ListTexture.DeleteColumn (0);

	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	lvColumn.fmt = LVCFMT_LEFT;
	lvColumn.cx = 100;
	lvColumn.pszText = const_cast<LPSTR> ("Texture Name");
	lvColumn.iSubItem = 0;
	m_ListTexture.InsertColumn (0, &lvColumn);

	LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iSubItem = 0;
	item.state = LVIS_SELECTED;
	item.stateMask = LVIS_SELECTED;
	item.iImage = 0;
	item.lParam = 1;
	item.iIndent = 0;

	for (unsigned long i = 1; i < pTextureSet->GetTextureCount(); ++i)
	{
		std::string strName;
		GetOnlyFileName (pTextureSet->GetTexture (i).stFilename.c_str(), strName);
		LPTSTR lpszItem = (char*) (strName.c_str());
		item.iItem = i;
		item.pszText = (char*)lpszItem;
		item.cchTextMax = strlen (item.pszText);
		m_ListTexture.InsertItem (&item);
	}

	m_ListTexture.SetRedraw (TRUE);

	m_TextureNumberVector.clear();
}

void CDlgInitBaseTexture::OnItemchangedListTexture (NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	*pResult = 0;

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor->IsMapReady())
	{
		return;
	}

	m_TextureNumberVector.clear();

	POSITION p = m_ListTexture.GetFirstSelectedItemPosition();

	while (p)
	{
		int nSelected = m_ListTexture.GetNextSelectedItem (p);

		if (nSelected < 0)
		{
			break;
		}

		m_TextureNumberVector.push_back (nSelected + 1);
	}

	pMapManagerAccessor->SetInitTextureBrushVector (m_TextureNumberVector);
}

void CDlgInitBaseTexture::InitTexture()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (!pMapManagerAccessor->IsMapReady())
	{
		return;
	}

	pMapManagerAccessor->InitBaseTexture();
}