// MapObjectPropertyPageDungeonBlock.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapObjectProperty.h"
#include "MapObjectPropertyPageDungeonBlock.h"
#include "../../GameLib/Property.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageDungeonBlock dialog


CMapObjectPropertyPageDungeonBlock::CMapObjectPropertyPageDungeonBlock (CWnd* pParent /*=NULL*/)
	: CMapObjectPropertyPageBase (CMapObjectPropertyPageDungeonBlock::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapObjectPropertyPageDungeonBlock)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMapObjectPropertyPageDungeonBlock::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapObjectPropertyPageDungeonBlock)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CMapObjectPropertyPageDungeonBlock, CDialog)
	//{{AFX_MSG_MAP(CMapObjectPropertyPageDungeonBlock)
	ON_BN_CLICKED (IDC_MAP_OBJECT_PROPERTY_DUNGEON_BLOCK_LOAD, OnLoadDungeonBlockFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageDungeonBlock normal functions

BOOL CMapObjectPropertyPageDungeonBlock::Create (CMapObjectProperty * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CMapObjectPropertyPageDungeonBlock::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	m_pParent = pParent;

	return TRUE;
}

void CMapObjectPropertyPageDungeonBlock::OnUpdateUI (CProperty * pProperty)
{
	const char* c_szPropertyType;
	const char* c_szPropertyName;
	if (!pProperty->GetString ("PropertyType", &c_szPropertyType))
	{
		return;
	}
	if (!pProperty->GetString ("PropertyName", &c_szPropertyName))
	{
		return;
	}
	if (prt::PROPERTY_TYPE_DUNGEON_BLOCK != prt::GetPropertyType (c_szPropertyType))
	{
		return;
	}

	const char* c_szBuildingFile;
	if (pProperty->GetString ("DungeonBlockFile", &c_szBuildingFile))
	{
		SetDlgItemText (IDC_MAP_OBJECT_PROPERTY_DUNGEON_BLOCK_FILE, c_szBuildingFile);
	}

	OnUpdatePropertyData (c_szPropertyName);
}

void CMapObjectPropertyPageDungeonBlock::OnUpdatePropertyData (const char* c_szPropertyName)
{
	CString strDungeonBlockFileName;
	GetDlgItemText (IDC_MAP_OBJECT_PROPERTY_DUNGEON_BLOCK_FILE, strDungeonBlockFileName);

	m_propertyDungeonBlock.strName = CFilename (c_szPropertyName);
	m_propertyDungeonBlock.strFileName = CFilename (strDungeonBlockFileName);
}

void CMapObjectPropertyPageDungeonBlock::OnRender (HWND hWnd)
{
	m_Screen.Begin();
	m_Screen.SetClearColor (0.0f, 0.0f, 0.0f);
	m_Screen.Clear();

	// TODO : Please writing here code that renders preview

	m_Screen.Show (hWnd);
	m_Screen.End();
}

bool CMapObjectPropertyPageDungeonBlock::OnSave (const char* c_szPathName, CProperty * pProperty)
{
	std::string strFileName;
	strFileName  = c_szPathName;
	strFileName += "/";
	strFileName += m_propertyDungeonBlock.strName;
	strFileName += prt::c_szPropertyExtension[prt::PROPERTY_TYPE_DUNGEON_BLOCK];

	if (m_propertyDungeonBlock.strName.empty())
	{
		LogBox ("[ERROR] You must enter a name.", "Error", GetSafeHwnd());
		return false;
	}

	if (!pProperty)
	{
		if (CPropertyManager::Instance().Get (strFileName.c_str(), &pProperty))
		{
			LogBox ("[ERROR] A property with the same name already exists.", "Error", GetSafeHwnd());
			return false;
		}

		pProperty = new CProperty (strFileName.c_str());
	}

	prt::PropertyDungeonBlockDataToString (&m_propertyDungeonBlock, pProperty);
	pProperty->Save (strFileName.c_str());

	m_dwCRC = pProperty->GetCRC();
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageDungeonBlock message handlers

void CMapObjectPropertyPageDungeonBlock::OnOK()
{
	m_pParent->OnOk();
}
void CMapObjectPropertyPageDungeonBlock::OnCancel()
{
	m_pParent->OnCancel();
}

void CMapObjectPropertyPageDungeonBlock::OnLoadDungeonBlockFile()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Granny File (*.gr2) |*.gr2|All Files (*.*)|*.*|";

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (FileOpener.DoModal())
	{
		std::string strFullFileName;
		StringPath (FileOpener.GetPathName(), strFullFileName);

		SetDlgItemText (IDC_MAP_OBJECT_PROPERTY_DUNGEON_BLOCK_FILE, strFullFileName.c_str());
	}
}
