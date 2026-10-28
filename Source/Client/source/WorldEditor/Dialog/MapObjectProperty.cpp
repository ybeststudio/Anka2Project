// MapObjectCreateProperty.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapObjectProperty.h"
#include "../../GameLib/Property.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapObjectProperty dialog


CMapObjectProperty::CMapObjectProperty (CWnd* pParent /*=NULL*/)
	: CDialog (CMapObjectProperty::IDD, pParent)
{
	m_strPath = "";
	m_strPropertyName = "";

	m_dwPropertyCRC32 = 0;
	m_pProperty = NULL;

	//{{AFX_DATA_INIT(CMapObjectProperty)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMapObjectProperty::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapObjectProperty)
	DDX_Control (pDX, IDC_MAP_OBJECT_CREATE_PROPERTY_TYPE_LIST, m_ctrlPropertyTypeList);
	//}}AFX_DATA_MAP

	m_ctrlPropertyTypeList.InsertString (prt::PROPERTY_TYPE_NONE, "None");
	m_ctrlPropertyTypeList.InsertString (prt::PROPERTY_TYPE_TREE, "Tree");
	m_ctrlPropertyTypeList.InsertString (prt::PROPERTY_TYPE_BUILDING, "Building");
	m_ctrlPropertyTypeList.InsertString (prt::PROPERTY_TYPE_EFFECT, "Effect");
	m_ctrlPropertyTypeList.InsertString (prt::PROPERTY_TYPE_AMBIENCE, "Ambience");
	m_ctrlPropertyTypeList.InsertString (prt::PROPERTY_TYPE_DUNGEON_BLOCK, "Dugeon Block");
	m_ctrlPropertyTypeList.SelectString (-1, "None");
}


BEGIN_MESSAGE_MAP (CMapObjectProperty, CDialog)
	//{{AFX_MSG_MAP(CMapObjectProperty)
	ON_WM_TIMER()
	ON_BN_CLICKED (IDC_OK, OnOk)
	ON_BN_CLICKED (IDC_CANCEL, OnCancel)
	ON_CBN_SELCHANGE (IDC_MAP_OBJECT_CREATE_PROPERTY_TYPE_LIST, OnChangePropertyType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapObjectProperty normal functions

void CMapObjectProperty::SetPath (const char* c_szPath)
{
	m_strPath = c_szPath;
}
void CMapObjectProperty::SetData (CProperty * pProperty)
{
	m_pProperty = pProperty;
}

int CMapObjectProperty::GetPropertyType()
{
	return m_iPropertyType;
}

const char* CMapObjectProperty::GetPropertyName()
{
	return m_strPropertyName.c_str();
}

DWORD CMapObjectProperty::GetPropertyCRC32()
{
	return m_dwPropertyCRC32;
}

void CMapObjectProperty::SetupProperty()
{
	if (!m_pProperty)
	{
		GetDlgItem (IDC_MAP_OBJECT_PROPERTY_NAME)->EnableWindow (TRUE);
		m_ctrlPropertyTypeList.EnableWindow (TRUE);
		return;
	}

	GetDlgItem (IDC_MAP_OBJECT_PROPERTY_NAME)->EnableWindow (FALSE);
	m_ctrlPropertyTypeList.EnableWindow (FALSE);

	const char* c_szPropertyName;
	const char* c_szPropertyType;
	if (!m_pProperty->GetString ("PropertyName", &c_szPropertyName))
	{
		return;
	}
	if (!m_pProperty->GetString ("PropertyType", &c_szPropertyType))
	{
		return;
	}

	SetDlgItemText (IDC_MAP_OBJECT_PROPERTY_NAME, c_szPropertyName);

	int iPropertyType = prt::GetPropertyType (c_szPropertyType);
	CString strPropertyName;
	m_ctrlPropertyTypeList.GetLBText (iPropertyType, strPropertyName);
	m_ctrlPropertyTypeList.SelectString (-1, strPropertyName);
	OnChangePropertyType();

	if (m_pActivePage)
	{
		m_pActivePage->UpdateUI (m_pProperty);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMapObjectProperty message handlers

BOOL CMapObjectProperty::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iPropertyType = prt::PROPERTY_TYPE_NONE;
	m_pActivePage = NULL;

	SetTimer (WINDOW_TIMER_ID_OBJECT_PROPERTY_PREVIEW, 20, 0);

	CRect Rect;
	GetDlgItem (IDC_MAP_OBJECT_CREATE_PROPERTY_PAGE)->GetWindowRect (&Rect);
	CMapObjectProperty::ScreenToClient (&Rect);

	Rect.left -= 5;
	Rect.top -= 8;

	if (!m_pageTree.Create (this, Rect))
	{
		return FALSE;
	}

	if (!m_pageBuilding.Create (this, Rect))
	{
		return FALSE;
	}

	if (!m_pageEffect.Create (this, Rect))
	{
		return FALSE;
	}

	if (!m_pageAmbience.Create (this, Rect))
	{
		return FALSE;
	}

	if (!m_pageDungeonBlock.Create (this, Rect))
	{
		return FALSE;
	}

	m_pPropertyPage[prt::PROPERTY_TYPE_NONE] = NULL;
	m_pPropertyPage[prt::PROPERTY_TYPE_TREE] = &m_pageTree;
	m_pPropertyPage[prt::PROPERTY_TYPE_BUILDING] = &m_pageBuilding;
	m_pPropertyPage[prt::PROPERTY_TYPE_EFFECT] = &m_pageEffect;
	m_pPropertyPage[prt::PROPERTY_TYPE_AMBIENCE] = &m_pageAmbience;
	m_pPropertyPage[prt::PROPERTY_TYPE_DUNGEON_BLOCK] = &m_pageDungeonBlock;

	SetupProperty();
	OnChangePropertyType();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMapObjectProperty::OnOk()
{
	if (!m_pActivePage)
	{
		LogBox ("[ERROR] CMapObjectProperty::OnOk: You must select a Type.", "Error", GetSafeHwnd());
		return;
	}

	CString strPropertyName;
	GetDlgItemText (IDC_MAP_OBJECT_PROPERTY_NAME, strPropertyName);

	m_strPropertyName = strPropertyName;
	m_strPropertyName += prt::GetPropertyExtension (GetPropertyType());

	m_pActivePage->UpdatePropertyData (strPropertyName);

	if (!m_pActivePage->Save (m_strPath.c_str(), m_pProperty))
	{
		return;
	}

	m_dwPropertyCRC32 = m_pActivePage->GetPropertyCRC32();

	EndDialog (TRUE);
}

void CMapObjectProperty::OnCancel()
{
	EndDialog (FALSE);
}

void CMapObjectProperty::OnTimer (UINT_PTR nIDEvent)
{
	if (IsWindowVisible())
	{
		HWND hWnd = GetDlgItem (IDC_MAP_OBJECT_PROPERTY_PREVIEW)->GetSafeHwnd();
		if (m_pActivePage)
		{
			m_pActivePage->Render (hWnd);
		}
	}

	CDialog::OnTimer (nIDEvent);
}

void CMapObjectProperty::OnChangePropertyType()
{
	m_iPropertyType = m_ctrlPropertyTypeList.GetCurSel();

	for (int i = 0; i < prt::PROPERTY_TYPE_MAX_NUM; ++i)
	{
		if (m_pPropertyPage[i])
		{
			m_pPropertyPage[i]->ShowWindow (SW_HIDE);
		}
	}

	if (m_pPropertyPage[m_iPropertyType])
	{
		m_pPropertyPage[m_iPropertyType]->ShowWindow (SW_SHOW);
		m_pActivePage = m_pPropertyPage[m_iPropertyType];
	}
	else
	{
		m_pActivePage = NULL;
	}
}
