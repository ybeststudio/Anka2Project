// MapObjectPropertyPageAmbience.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapObjectProperty.h"
#include "MapObjectPropertyPageAmbience.h"
#include "../../GameLib/Property.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageAmbience dialog


CMapObjectPropertyPageAmbience::CMapObjectPropertyPageAmbience (CWnd* pParent /*=NULL*/)
	: CMapObjectPropertyPageBase (CMapObjectPropertyPageAmbience::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMapObjectPropertyPageAmbience)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMapObjectPropertyPageAmbience::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapObjectPropertyPageAmbience)
	DDX_Control (pDX, IDC_MAP_OBJECT_PROPERTY_AMBIENCE_PLAY_TYPE, m_ctrlPlayType);
	DDX_Control (pDX, IDC_MAP_OBJECT_PROPERTY_AMBIENCE_FILE_LIST, m_ctrlSoundFileList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CMapObjectPropertyPageAmbience, CDialog)
	//{{AFX_MSG_MAP(CMapObjectPropertyPageAmbience)
	ON_BN_CLICKED (IDC_MAP_OBJECT_PROPERTY_AMBIENCE_FILE_NEW, OnNewSoundFile)
	ON_BN_CLICKED (IDC_MAP_OBJECT_PROPERTY_AMBIENCE_FILE_DELETE, OnDeleteSoundFile)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE (IDC_MAP_OBJECT_PROPERTY_AMBIENCE_PLAY_TYPE, OnChangePlayType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageAmbience normal functions

BOOL CMapObjectPropertyPageAmbience::Create (CMapObjectProperty * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CMapObjectPropertyPageAmbience::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	m_pParent = pParent;

	m_ctrlPlayType.InsertString (0, "ONCE");
	m_ctrlPlayType.InsertString (1, "STEP");
	m_ctrlPlayType.InsertString (2, "LOOP");
	m_ctrlPlayType.SelectString (-1, "ONCE");
	OnChangePlayType();

	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_OBJECT_PROPERTY_AMBIENCE_INTERVAL, 0.0f);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_OBJECT_PROPERTY_AMBIENCE_INTERVAL_VARIATION, 0.0f);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_OBJECT_PROPERTY_AMBIENCE_MAX_VOLUME_AREA_PERCENTAGE, 0.0f);

	return TRUE;
}

void CMapObjectPropertyPageAmbience::OnUpdateUI (CProperty * pProperty)
{
	prt::PropertyAmbienceStringToData (pProperty, &m_propertyAmbience);

	auto & rSoundFileNameVector = m_propertyAmbience.AmbienceSoundVector;
	for (DWORD dwIndex = 0; dwIndex < rSoundFileNameVector.size(); ++dwIndex)
	{
		m_ctrlSoundFileList.InsertString (dwIndex, rSoundFileNameVector[dwIndex].c_str());
	}

	m_ctrlPlayType.SelectString (-1, m_propertyAmbience.strPlayType.c_str());

	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_OBJECT_PROPERTY_AMBIENCE_INTERVAL, m_propertyAmbience.fPlayInterval);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_OBJECT_PROPERTY_AMBIENCE_INTERVAL_VARIATION, m_propertyAmbience.fPlayIntervalVariation);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_OBJECT_PROPERTY_AMBIENCE_MAX_VOLUME_AREA_PERCENTAGE, m_propertyAmbience.fMaxVolumeAreaPercentage * 100.0f);

	OnUpdatePropertyData (m_propertyAmbience.strName.c_str());
	OnChangePlayType();
}

void CMapObjectPropertyPageAmbience::OnUpdatePropertyData (const char* c_szPropertyName)
{
	m_propertyAmbience.strName = c_szPropertyName;
	m_propertyAmbience.strPlayType = __GetPlayTypeName();
	m_propertyAmbience.fPlayInterval = GetDialogFloatText (GetSafeHwnd(), IDC_MAP_OBJECT_PROPERTY_AMBIENCE_INTERVAL);
	m_propertyAmbience.fPlayIntervalVariation = GetDialogFloatText (GetSafeHwnd(), IDC_MAP_OBJECT_PROPERTY_AMBIENCE_INTERVAL_VARIATION);
	m_propertyAmbience.fMaxVolumeAreaPercentage = GetDialogFloatText (GetSafeHwnd(), IDC_MAP_OBJECT_PROPERTY_AMBIENCE_MAX_VOLUME_AREA_PERCENTAGE) / 100.0f;
}

void CMapObjectPropertyPageAmbience::OnRender (HWND hWnd)
{
	m_Screen.Begin();
	m_Screen.SetClearColor (0.0f, 0.0f, 0.0f);
	m_Screen.Clear();

	// TODO : Please writing here code that renders preview

	m_Screen.Show (hWnd);
	m_Screen.End();
}

bool CMapObjectPropertyPageAmbience::OnSave (const char* c_szPathName, CProperty * pProperty)
{
	std::string strFileName;
	strFileName  = c_szPathName;
	strFileName += "/";
	strFileName += m_propertyAmbience.strName;
	strFileName += prt::c_szPropertyExtension[prt::PROPERTY_TYPE_AMBIENCE];

	if (m_propertyAmbience.strName.empty())
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

	prt::PropertyAmbienceDataToString (&m_propertyAmbience, pProperty);
	pProperty->Save (strFileName.c_str());

	m_dwCRC = pProperty->GetCRC();
	return true;
}

const char* CMapObjectPropertyPageAmbience::__GetPlayTypeName()
{
	static CString strPlayTypeName;
	int iPlayType = m_ctrlPlayType.GetCurSel();
	m_ctrlPlayType.GetLBText (iPlayType, strPlayTypeName);
	return strPlayTypeName;
}

/////////////////////////////////////////////////////////////////////////////
// CMapObjectPropertyPageAmbience message handlers

void CMapObjectPropertyPageAmbience::OnOK()
{
	m_pParent->OnOk();
}

void CMapObjectPropertyPageAmbience::OnCancel()
{
	m_pParent->OnCancel();
}

//////////////
// Sound List
void CMapObjectPropertyPageAmbience::OnNewSoundFile()
{
	DWORD dwFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Sound Files (*.wav, *.mp3)|*.wav;*.mp3|All Files (*.*)|*.*|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CString path = FileOpener.GetPathName();
		CFilename filename = path.GetString();
		filename.StringPath();
		const char* c_szLocalPath = GetProgramExcutingPath (filename.c_str());

		// 같은 파일 이름이 존재 하는지 체크
		for (DWORD i = 0; i < m_propertyAmbience.AmbienceSoundVector.size(); ++i)
		{
			const CFilename & c_rfilename = m_propertyAmbience.AmbienceSoundVector[i];
			if (0 == c_rfilename.compare (c_szLocalPath))
			{
				MessageBox ("[ERROR] The same file is already registered.", "ERROR");
				return;
			}
		}

		// 등록
		m_propertyAmbience.AmbienceSoundVector.push_back (c_szLocalPath);
		m_ctrlSoundFileList.InsertString (0, c_szLocalPath);
	}
}

void CMapObjectPropertyPageAmbience::OnDeleteSoundFile()
{
	DWORD dwCurSel = DWORD (m_ctrlSoundFileList.GetCurSel());
	if (dwCurSel >= m_propertyAmbience.AmbienceSoundVector.size())
	{
		return;
	}

	DeleteVectorItem (&m_propertyAmbience.AmbienceSoundVector, dwCurSel);
	m_ctrlSoundFileList.DeleteString (dwCurSel);
}

void CMapObjectPropertyPageAmbience::OnChangePlayType()
{
	const char* c_szPlayTypeName = __GetPlayTypeName();
	if (!strcmp (c_szPlayTypeName, "STEP"))
	{
		GetDlgItem (IDC_MAP_OBJECT_PROPERTY_AMBIENCE_INTERVAL)->EnableWindow (TRUE);
		GetDlgItem (IDC_MAP_OBJECT_PROPERTY_AMBIENCE_INTERVAL_VARIATION)->EnableWindow (TRUE);
	}
	else
	{
		GetDlgItem (IDC_MAP_OBJECT_PROPERTY_AMBIENCE_INTERVAL)->EnableWindow (FALSE);
		GetDlgItem (IDC_MAP_OBJECT_PROPERTY_AMBIENCE_INTERVAL_VARIATION)->EnableWindow (FALSE);
	}
}
