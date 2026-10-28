// ObjectAnimationEventEffect.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "ObjectAnimationEventEffect.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventEffect dialog


CObjectAnimationEventEffect::CObjectAnimationEventEffect (CWnd* pParent /*=NULL*/)
	: CObjectAnimationEventBase (CObjectAnimationEventEffect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectAnimationEventEffect)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectAnimationEventEffect::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectAnimationEventEffect)
	DDX_Control (pDX, IDC_OBJECT_ANIMATION_EVENT_EFFECT_ATTACHING_BONE, m_ctrlAttachingBone);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectAnimationEventEffect, CDialog)
	//{{AFX_MSG_MAP(CObjectAnimationEventEffect)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_EVENT_EFFECT_LOAD, OnLoadEffect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventBase normal functions

BOOL CObjectAnimationEventEffect::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectAnimationEventEffect::IDD, (CWnd*)pParent))
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
		m_ctrlAttachingBone.InsertString (i, strBoneName.c_str());
		m_ctrlAttachingBone.SelectString (-1, strBoneName.c_str());
	}

	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_POSITION_X, 0.0f);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_POSITION_Y, 0.0f);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_POSITION_Z, 0.0f);

	return TRUE;
}

BOOL CObjectAnimationEventEffect::canClose()
{
	GetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_EFFECT_FILE_NAME, m_strEffectFileName);
	if (0 == strlen (m_strEffectFileName))
	{
		LogBox ("[ERROR] No file name provided.", "Error");
		return FALSE;
	}

	return TRUE;
}

void CObjectAnimationEventEffect::Close()
{
	GetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_EFFECT_FILE_NAME, m_strEffectFileName);

	m_isAttachingEnable = IsDlgButtonChecked (IDC_OBJECT_ANIMATION_EVENT_EFFECT_ATTACHING_BONE_ENABLE);
	m_isFollowingEnable = IsDlgButtonChecked (IDC_OBJECT_ANIMATION_EVENT_EFFECT_ATTACHING_FOLLOWING);
	m_isIndependentEnable = IsDlgButtonChecked (IDC_OBJECT_ANIMATION_EVENT_EFFECT_INDEPENDENT);
	m_v3EffectPosition.x = GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_POSITION_X);
	m_v3EffectPosition.y = GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_POSITION_Y);
	m_v3EffectPosition.z = GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_POSITION_Z);

	/////

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	DWORD dwIndex = m_ctrlAttachingBone.GetCurSel();
	if (dwIndex < pSceneObject->GetBoneCount())
	{
		pSceneObject->GetBoneName (dwIndex, &m_strAttachingBoneName);
	}

	/////

	CEffectManager::Instance().RegisterEffect (m_strEffectFileName, true);
}

void CObjectAnimationEventEffect::GetData (CRaceMotionDataAccessor::TMotionEventData * pData)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	std::string strEffectFileName;
	StringPath (m_strEffectFileName, strEffectFileName);

	CRaceMotionDataAccessor::TMotionEffectEventData * pEffectEventData = (CRaceMotionDataAccessor::TMotionEffectEventData*)pData;
	pEffectEventData->isAttaching = m_isAttachingEnable;
	pEffectEventData->isFollowing = m_isFollowingEnable;
	pEffectEventData->isIndependent = m_isIndependentEnable;
	pEffectEventData->strAttachingBoneName = m_strAttachingBoneName;
	pEffectEventData->strEffectFileName = strEffectFileName;
	pEffectEventData->dwEffectIndex = GetCaseCRC32 (strEffectFileName.c_str(), strEffectFileName.length());
	pEffectEventData->v3EffectPosition = m_v3EffectPosition;
}

void CObjectAnimationEventEffect::SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData)
{
	const CRaceMotionDataAccessor::TMotionEffectEventData * c_pEffectEventData = (const CRaceMotionDataAccessor::TMotionEffectEventData*)c_pData;

	const CEffectData * c_pEffectData;
	if (!CEffectManager::Instance().GetEffectData (c_pEffectEventData->dwEffectIndex, &c_pEffectData))
	{
		return;
	}

	SetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_EFFECT_FILE_NAME, c_pEffectData->GetFileName());
	CheckDlgButton (IDC_OBJECT_ANIMATION_EVENT_EFFECT_ATTACHING_BONE_ENABLE, c_pEffectEventData->isAttaching);
	CheckDlgButton (IDC_OBJECT_ANIMATION_EVENT_EFFECT_ATTACHING_FOLLOWING, c_pEffectEventData->isFollowing);
	CheckDlgButton (IDC_OBJECT_ANIMATION_EVENT_EFFECT_INDEPENDENT, c_pEffectEventData->isIndependent);

	m_ctrlAttachingBone.SelectString (-1, c_pEffectEventData->strAttachingBoneName.c_str());

	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_POSITION_X, c_pEffectEventData->v3EffectPosition.x);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_POSITION_Y, c_pEffectEventData->v3EffectPosition.y);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_POSITION_Z, c_pEffectEventData->v3EffectPosition.z);
}

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventEffect message handlers

void CObjectAnimationEventEffect::OnLoadEffect()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Metin2 Effect Script Files (*.mse)|*.mse|*.*|All Files (*.*)|";
	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		SetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_EFFECT_FILE_NAME, FileOpener.GetPathName());
	}
}

void CObjectAnimationEventEffect::OnOK()
{
}

void CObjectAnimationEventEffect::OnCancel()
{
}
