// MapObjectPropertyPageEffect.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapObjectProperty.h"
#include "MapObjectPropertyPageEffect.h"
#include "../../GameLib/Property.h"
#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageEffect dialog


CMapObjectPropertyPageEffect::CMapObjectPropertyPageEffect (CWnd* pParent /*=NULL*/)
	: CMapObjectPropertyPageBase (CMapObjectPropertyPageEffect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapObjectPropertyPageEffect)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMapObjectPropertyPageEffect::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapObjectPropertyPageEffect)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CMapObjectPropertyPageEffect, CDialog)
	//{{AFX_MSG_MAP(CMapObjectPropertyPageEffect)
	ON_BN_CLICKED (IDC_MAP_OBJECT_PROPERTY_EFFECT_LOAD, OnLoadEffectFile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageEffect message handlers

BOOL CMapObjectPropertyPageEffect::Create (CMapObjectProperty * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CMapObjectPropertyPageEffect::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	m_pParent = pParent;

	m_propertyEffect.strName = "";
	m_propertyEffect.strFileName = "";
	return TRUE;
}

void CMapObjectPropertyPageEffect::OnUpdateUI (CProperty * pProperty)
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
	if (prt::PROPERTY_TYPE_EFFECT != prt::GetPropertyType (c_szPropertyType))
	{
		return;
	}

	const char* c_szEffectFile;
	if (pProperty->GetString ("EffectFile", &c_szEffectFile))
	{
		SetDlgItemText (IDC_MAP_OBJECT_PROPERTY_EFFECT_FILE, c_szEffectFile);
	}

	OnUpdatePropertyData (c_szPropertyName);
}

void CMapObjectPropertyPageEffect::OnUpdatePropertyData (const char* c_szPropertyName)
{
	CString strEffectFileName;
	GetDlgItemText (IDC_MAP_OBJECT_PROPERTY_EFFECT_FILE, strEffectFileName);

	m_propertyEffect.strName = c_szPropertyName;
	m_propertyEffect.strFileName = CFilename (strEffectFileName);
}

bool CMapObjectPropertyPageEffect::OnSave (const char* c_szPathName, CProperty * pProperty)
{
	std::string strFileName;
	strFileName  = c_szPathName;
	strFileName += "/";
	strFileName += m_propertyEffect.strName;
	strFileName += ".pre";

	if (m_propertyEffect.strName.empty())
	{
		LogBox ("[ERROR] You must enter a name.", "Error", GetSafeHwnd());
		return false;
	}

	if (m_propertyEffect.strFileName.empty())
	{
		LogBox ("[ERROR] You must enter an effect file name.", "Error", GetSafeHwnd());
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

	prt::PropertyEffectDataToString (&m_propertyEffect, pProperty);
	pProperty->Save (strFileName.c_str());

	m_dwCRC = pProperty->GetCRC();
	return true;

}

void CMapObjectPropertyPageEffect::OnRender (HWND hWnd)
{
	m_Screen.Begin();
	m_Screen.SetClearColor (0.0f, 0.0f, 0.0f);
	m_Screen.Clear();

	// TODO : Please writing here code that renders preview

	m_Screen.Show (hWnd);
	m_Screen.End();
}

void CMapObjectPropertyPageEffect::OnOK()
{
	m_pParent->OnOk();
}
void CMapObjectPropertyPageEffect::OnCancel()
{
	m_pParent->OnCancel();
}

void CMapObjectPropertyPageEffect::OnLoadEffectFile()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Effect File (*.mse) |*.mse|All Files (*.*)|*.*|";

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (FileOpener.DoModal())
	{
		std::string strFullFileName;
		StringPath (FileOpener.GetPathName(), strFullFileName);

		SetDlgItemText (IDC_MAP_OBJECT_PROPERTY_EFFECT_FILE, strFullFileName.c_str());
	}
}

