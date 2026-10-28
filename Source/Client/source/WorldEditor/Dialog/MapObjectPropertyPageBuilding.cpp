// MapObjectPropertyPageBuilding.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapObjectPropertyPageBuilding.h"
#include "../../GameLib/Property.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

// Test Code
#include "MapObjectProperty.h"

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageBuilding dialog


CMapObjectPropertyPageBuilding::CMapObjectPropertyPageBuilding (CWnd* pParent /*=NULL*/)
	: CMapObjectPropertyPageBase (CMapObjectPropertyPageBuilding::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapObjectPropertyPageBuilding)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMapObjectPropertyPageBuilding::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapObjectPropertyPageBuilding)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CMapObjectPropertyPageBuilding, CDialog)
	//{{AFX_MSG_MAP(CMapObjectPropertyPageBuilding)
	ON_BN_CLICKED (IDC_MAP_OBJECT_PROPERTY_BUILDING_LOAD, OnLoadBuildingFile)
	ON_BN_CLICKED (IDC_MAP_OBJECT_PROPERTY_BUILDING_SHADOW_FLAG, OnCheckShadowFlag)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageBuilding normal functions

BOOL CMapObjectPropertyPageBuilding::Create (CMapObjectProperty * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CMapObjectPropertyPageBuilding::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	m_pParent = pParent;

	m_propertyBuilding.strName = "";
	m_propertyBuilding.strFileName = "";
	m_propertyBuilding.strAttributeDataFileName = "";
	m_propertyBuilding.isShadowFlag = TRUE;
	return TRUE;
}

void CMapObjectPropertyPageBuilding::OnUpdateUI (CProperty * pProperty)
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
	if (prt::PROPERTY_TYPE_BUILDING != prt::GetPropertyType (c_szPropertyType))
	{
		return;
	}

	const char* c_szBuildingFile;
	if (pProperty->GetString ("BuildingFile", &c_szBuildingFile))
	{
		SetDlgItemText (IDC_MAP_OBJECT_PROPERTY_BUILDING_FILE, c_szBuildingFile);
	}

	const char* c_szShadowFlag;
	if (!pProperty->GetString ("ShadowFlag", &c_szShadowFlag))
	{
		CheckDlgButton (IDC_MAP_OBJECT_PROPERTY_BUILDING_SHADOW_FLAG, TRUE);
	}
	else
	{
		if (atoi (c_szShadowFlag))
		{
			CheckDlgButton (IDC_MAP_OBJECT_PROPERTY_BUILDING_SHADOW_FLAG, TRUE);
		}
		else
		{
			CheckDlgButton (IDC_MAP_OBJECT_PROPERTY_BUILDING_SHADOW_FLAG, FALSE);
		}
	}

	OnUpdatePropertyData (c_szPropertyName);
}

void CMapObjectPropertyPageBuilding::OnUpdatePropertyData (const char* c_szPropertyName)
{
	CString strBuildingFileName;
	GetDlgItemText (IDC_MAP_OBJECT_PROPERTY_BUILDING_FILE, strBuildingFileName);

	m_propertyBuilding.strName = c_szPropertyName;
	m_propertyBuilding.strFileName = CFilename (strBuildingFileName);
}

bool CMapObjectPropertyPageBuilding::OnSave (const char* c_szPathName, CProperty * pProperty)
{
	std::string strFileName;
	strFileName  = c_szPathName;
	strFileName += "/";
	strFileName += m_propertyBuilding.strName;
	strFileName += ".prb";

	if (m_propertyBuilding.strName.empty())
	{
		LogBox ("[ERROR] You must enter a name.", "Error", GetSafeHwnd());
		return false;
	}

	if (m_propertyBuilding.strFileName.empty())
	{
		LogBox ("[ERROR] You must enter a model file name.", "Error", GetSafeHwnd());
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

	prt::PropertyBuildingDataToString (&m_propertyBuilding, pProperty);
	pProperty->Save (strFileName.c_str());

	m_dwCRC = pProperty->GetCRC();
	return true;
}

void CMapObjectPropertyPageBuilding::OnRender (HWND hWnd)
{
	m_Screen.Begin();
	m_Screen.SetClearColor (0.0f, 0.0f, 0.0f);
	m_Screen.Clear();

	// TODO : Please writing here code that renders preview

	m_Screen.Show (hWnd);
	m_Screen.End();
}

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageBuilding message handlers

void CMapObjectPropertyPageBuilding::OnOK()
{
	m_pParent->OnOk();
}
void CMapObjectPropertyPageBuilding::OnCancel()
{
	m_pParent->OnCancel();
}

void CMapObjectPropertyPageBuilding::OnLoadBuildingFile()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Granny File (*.gr2) |*.gr2|All Files (*.*)|*.*|";

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (FileOpener.DoModal())
	{
		std::string strFullFileName;
		StringPath (FileOpener.GetPathName(), strFullFileName);

		SetDlgItemText (IDC_MAP_OBJECT_PROPERTY_BUILDING_FILE, strFullFileName.c_str());
	}
}

void CMapObjectPropertyPageBuilding::OnCheckShadowFlag()
{
	m_propertyBuilding.isShadowFlag = IsDlgButtonChecked (IDC_MAP_OBJECT_PROPERTY_BUILDING_SHADOW_FLAG);
}
