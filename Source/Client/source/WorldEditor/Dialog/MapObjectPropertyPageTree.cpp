// MapObjectPropertyPageTree.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapObjectPropertyPageTree.h"
#include "../../GameLib/Property.h"
#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

// Test Code
#include "MapObjectProperty.h"

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageTree dialog


CMapObjectPropertyPageTree::CMapObjectPropertyPageTree (CWnd* pParent /*=NULL*/)
	: CMapObjectPropertyPageBase (CMapObjectPropertyPageTree::IDD, pParent)
	  //	: CDialog(CMapObjectPropertyPageTree::IDD, pParent)
{
	m_pParent = NULL;
	m_pTree = NULL;
	//{{AFX_DATA_INIT(CMapObjectPropertyPageTree)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMapObjectPropertyPageTree::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapObjectPropertyPageTree)
	DDX_Control (pDX, IDC_MAP_OBJECT_PROPERTY_TREE_SIZE_VARIATION, m_ctrlTreeSizeVariance);
	DDX_Control (pDX, IDC_MAP_OBJECT_PROPERTY_TREE_SIZE, m_ctrlTreeSize);
	//}}AFX_DATA_MAP

	m_ctrlTreeSize.SetRangeMin (1);
	m_ctrlTreeSize.SetRangeMax (3000);
	m_ctrlTreeSize.SetPos (1000);
	m_ctrlTreeSizeVariance.SetRangeMin (0);
	m_ctrlTreeSizeVariance.SetRangeMax (1000);
	m_ctrlTreeSizeVariance.SetPos (0);
}


BEGIN_MESSAGE_MAP (CMapObjectPropertyPageTree, CDialog)
	//{{AFX_MSG_MAP(CMapObjectPropertyPageTree)
	ON_BN_CLICKED (IDC_MAP_OBJECT_PROPERTY_TREE_LOAD, OnLoadTreeFile)
	ON_WM_HSCROLL()
	ON_BN_CLICKED (IDC_MAP_OBJECT_PROPERTY_TREE_ACCEPT, OnAcceptTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageTree normal function

BOOL CMapObjectPropertyPageTree::Create (CMapObjectProperty * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CMapObjectPropertyPageTree::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), 0, TRUE);
	ShowWindow (SW_HIDE);

	m_pParent = pParent;
	UpdateScrollBarState();

	return TRUE;
}

void CMapObjectPropertyPageTree::OnUpdateUI (CProperty * pProperty)
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
	if (prt::PROPERTY_TYPE_TREE != prt::GetPropertyType (c_szPropertyType))
	{
		return;
	}

	const char* c_szTreeFile;
	const char* c_szTreeSize;
	const char* c_szTreeSizeVariance;
	if (!pProperty->GetString ("TreeFile", &c_szTreeFile))
	{
		return;
	}
	if (!pProperty->GetString ("TreeSize", &c_szTreeSize))
	{
		return;
	}
	if (!pProperty->GetString ("TreeVariance", &c_szTreeSizeVariance))
	{
		return;
	}

	SetDlgItemText (IDC_MAP_OBJECT_PROPERTY_TREE_FILE, c_szTreeFile);

	int iTreeSize = atoi (c_szTreeSize);
	int iTreeSizeVariance = atoi (c_szTreeSizeVariance);

	m_ctrlTreeSize.SetPos (iTreeSize);
	m_ctrlTreeSizeVariance.SetPos (iTreeSizeVariance);

	UpdateScrollBarState();
	OnUpdatePropertyData (c_szPropertyName);
	OnAcceptTree();
}

void CMapObjectPropertyPageTree::OnUpdatePropertyData (const char* c_szPropertyName)
{
	CString strTreeFileName;
	GetDlgItemText (IDC_MAP_OBJECT_PROPERTY_TREE_FILE, strTreeFileName);
	int iTreeSize = m_ctrlTreeSize.GetPos();
	int iTreeVariance = m_ctrlTreeSizeVariance.GetPos();

	m_propertyTree.strName = c_szPropertyName;
	m_propertyTree.strFileName = CFilename (strTreeFileName);
	m_propertyTree.fSize = float (iTreeSize);
	m_propertyTree.fVariance = float (iTreeVariance);
}

bool CMapObjectPropertyPageTree::OnSave (const char* c_szPathName, CProperty * pProperty)
{
	std::string strFileName;

	strFileName  = c_szPathName;
	strFileName += "\\";
	strFileName += m_propertyTree.strName;
	strFileName += ".prt";

	if (m_propertyTree.strName.empty() || m_propertyTree.strFileName.empty())
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

	prt::PropertyTreeDataToString (&m_propertyTree, pProperty);
	pProperty->Save (strFileName.c_str());

	m_dwCRC = pProperty->GetCRC();
	return true;
}

void CMapObjectPropertyPageTree::OnRender (HWND hWnd)
{
	m_Screen.Begin();
	m_Screen.SetClearColor (0.5f, 0.5f, 0.5f);
	m_Screen.Clear();

	CRect Rect;
	::GetWindowRect (hWnd, Rect);

	m_Screen.SetPerspective (40.0f, float (Rect.Width()) / float (Rect.Height()), 100.0f, 10000.0f);
	m_Screen.SetPositionCamera (0.0f, 0.0f, 500.0f, 2500.0f, 10.0f, 0.0f);

	if (m_pTree)
	{
		m_pTree->Render();
	}

	m_Screen.SetDiffuseOperation();
	RenderBackGroundCharacter (200.0f, -200.0f, 0.0f);

	m_Screen.SetDiffuseColor (1.0f, 1.0f, 1.0f);
	m_Screen.RenderBar3d (-100.0f, 100.0f, 0.0f, 100.0f, -100.0f, 0.0f);

	m_Screen.Show (hWnd);
	m_Screen.End();
}

void CMapObjectPropertyPageTree::UpdateScrollBarState()
{
	m_propertyTree.fSize = float (m_ctrlTreeSize.GetPos());
	m_propertyTree.fVariance = float (m_ctrlTreeSizeVariance.GetPos());

	char szSize[8 + 1];
	char szSizeVariance[8 + 1];
	_snprintf (szSize, 8, "%.0f", m_propertyTree.fSize);
	_snprintf (szSizeVariance, 8, "%.0f", m_propertyTree.fVariance);

	SetDlgItemText (IDC_MAP_OBJECT_PROPERTY_TREE_SIZE_PRINT, szSize);
	SetDlgItemText (IDC_MAP_OBJECT_PROPERTY_TREE_SIZE_VARIANCE_PRINT, szSizeVariance);
}

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageTree message handlers

void CMapObjectPropertyPageTree::OnOK()
{
	OnClearTree();
	m_pParent->OnOk();
}
void CMapObjectPropertyPageTree::OnCancel()
{
	OnClearTree();
	m_pParent->OnCancel();
}

void CMapObjectPropertyPageTree::OnLoadTreeFile()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "IDV Speed Tree File (*.spt) |*.spt|All Files (*.*) |*.*|";

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (FileOpener.DoModal())
	{
		std::string strFullFileName;
		StringPath (FileOpener.GetPathName(), strFullFileName);
		SetDlgItemText (IDC_MAP_OBJECT_PROPERTY_TREE_FILE, strFullFileName.c_str());
	}
}

void CMapObjectPropertyPageTree::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateScrollBarState();

	CDialog::OnHScroll (nSBCode, nPos, pScrollBar);
}

void CMapObjectPropertyPageTree::OnAcceptTree()
{
	OnClearTree();
	UpdateScrollBarState();

	//////////////
	CString strFileName;
	GetDlgItemText (IDC_MAP_OBJECT_PROPERTY_TREE_FILE, strFileName);

	m_pTree = new CSpeedTreeWrapper;

	if (!m_pTree->LoadTree (strFileName))
	{
		delete m_pTree;
		m_pTree = NULL;
		return;
	}
}

void CMapObjectPropertyPageTree::OnClearTree()
{
}
