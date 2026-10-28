// ObjectAnimationEventFly.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectAnimationEventFly.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventFly dialog


CObjectAnimationEventFly::CObjectAnimationEventFly (CWnd* pParent /*=NULL*/)
	: CObjectAnimationEventBase (CObjectAnimationEventFly::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectAnimationEventFly)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectAnimationEventFly::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectAnimationEventFly)
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_EVENT_FLY_ATTACHING_BONE, m_ctrlBone);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectAnimationEventFly, CDialog)
	//{{AFX_MSG_MAP(CObjectAnimationEventFly)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_EVENT_FLY_LOAD, OnObjectAnimationEventFlyLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventFly message handlers

void CObjectAnimationEventFly::OnObjectAnimationEventFlyLoad()
{
	DWORD dwFlag = OFN_HIDEREADONLY;
	const char* c_szFilter = "Metin2 Fly Script Files (*.msf)|*.msf|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		SetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_FLY_FILE_NAME, FileOpener.GetPathName());
	}

}

BOOL CObjectAnimationEventFly::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectAnimationEventFly::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	////////////////////////////////////////////////////////////////

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	std::string strBoneName;
	for (DWORD i = 0; i < pSceneObject->GetBoneCount(); ++i)
	{
		pSceneObject->GetBoneName (i, &strBoneName);
		m_ctrlBone.InsertString (i, strBoneName.c_str());
		m_ctrlBone.SelectString (0, strBoneName.c_str());
	}

	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_FLY_POSITION_X, 0.0f);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_FLY_POSITION_Y, 0.0f);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_FLY_POSITION_Z, 0.0f);

	return TRUE;
}

BOOL CObjectAnimationEventFly::canClose()
{
	GetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_FLY_FILE_NAME, m_strFlyFileName);
	if (0 == strlen (m_strFlyFileName))
	{
		LogBox ("[ERROR] No file name provided.", "Error");
		return FALSE;
	}

	return TRUE;
}

void CObjectAnimationEventFly::Close()
{
	GetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_FLY_FILE_NAME, m_strFlyFileName);

	m_isAttachingEnable = IsDlgButtonChecked (IDC_OBJECT_ANIMATION_EVENT_FLY_ATTACHING_BONE_ENABLE);
	m_v3FlyPosition.x = GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_FLY_POSITION_X);
	m_v3FlyPosition.y = GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_FLY_POSITION_Y);
	m_v3FlyPosition.z = GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_FLY_POSITION_Z);

	/////

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	DWORD dwIndex = m_ctrlBone.GetCurSel();
	if (dwIndex < pSceneObject->GetBoneCount())
	{
		pSceneObject->GetBoneName (dwIndex, &m_strAttachingBoneName);
	}

	/////

	CFlyingManager::Instance().RegisterFlyingData (m_strFlyFileName);
}

void CObjectAnimationEventFly::OnOK()
{
}

void CObjectAnimationEventFly::OnCancel()
{
}

void CObjectAnimationEventFly::GetData (CRaceMotionDataAccessor::TMotionEventData * pData)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	std::string strFlyFileName;
	StringPath (m_strFlyFileName, strFlyFileName);

	CRaceMotionDataAccessor::TMotionFlyEventData * pFlyEventData = (CRaceMotionDataAccessor::TMotionFlyEventData*)pData;
	pFlyEventData->isAttaching = m_isAttachingEnable;
	pFlyEventData->strAttachingBoneName = m_strAttachingBoneName;
	pFlyEventData->strFlyFileName = strFlyFileName;
	pFlyEventData->dwFlyIndex = GetCaseCRC32 (strFlyFileName.c_str(), strFlyFileName.length());
	pFlyEventData->v3FlyPosition = m_v3FlyPosition;
}

void CObjectAnimationEventFly::SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData)
{
	const CRaceMotionDataAccessor::TMotionFlyEventData * c_pFlyEventData = (const CRaceMotionDataAccessor::TMotionFlyEventData*)c_pData;
	SetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_FLY_FILE_NAME, c_pFlyEventData->strFlyFileName.c_str());
	CheckDlgButton (IDC_OBJECT_ANIMATION_EVENT_FLY_ATTACHING_BONE_ENABLE, c_pFlyEventData->isAttaching);

	m_ctrlBone.SelectString (0, c_pFlyEventData->strAttachingBoneName.c_str());

	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_FLY_POSITION_X, c_pFlyEventData->v3FlyPosition.x);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_FLY_POSITION_Y, c_pFlyEventData->v3FlyPosition.y);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_FLY_POSITION_Z, c_pFlyEventData->v3FlyPosition.z);
}
