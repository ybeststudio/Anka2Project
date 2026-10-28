// ObjectAnimationEventEffectToTarget.cpp : implementation file
//

#include "StdAfx.h"
#include "../worldeditor.h"
#include "ObjectAnimationEventEffectToTarget.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventEffectToTarget dialog


CObjectAnimationEventEffectToTarget::CObjectAnimationEventEffectToTarget (CWnd* pParent /*=NULL*/)
	: CObjectAnimationEventBase (CObjectAnimationEventEffectToTarget::IDD, pParent)
{
	//{{AFX_DATA_INIT(CObjectAnimationEventEffectToTarget)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CObjectAnimationEventEffectToTarget::DoDataExchange (CDataExchange* pDX)
{
	CObjectAnimationEventBase::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CObjectAnimationEventEffectToTarget)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CObjectAnimationEventEffectToTarget, CObjectAnimationEventBase)
	//{{AFX_MSG_MAP(CObjectAnimationEventEffectToTarget)
	ON_BN_CLICKED (IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_LOAD, OnLoadEffect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventEffectToTarget normal functions

BOOL CObjectAnimationEventEffectToTarget::Create (CWnd * pParent, const CRect & c_rRect)
{
	if (!CDialog::Create (CObjectAnimationEventEffectToTarget::IDD, (CWnd*)pParent))
	{
		return FALSE;
	}

	SetWindowPos (NULL, c_rRect.left, c_rRect.top, c_rRect.Width(), c_rRect.Height(), TRUE);
	ShowWindow (SW_HIDE);

	////////////////////////////////////////////////////////////////

	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_POSITION_X, 0.0f);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_POSITION_Y, 0.0f);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_POSITION_Z, 0.0f);

	return TRUE;
}

void CObjectAnimationEventEffectToTarget::Close()
{
	GetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_FILE_NAME, m_strEffectFileName);

	m_v3EffectPosition.x = GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_POSITION_X);
	m_v3EffectPosition.y = GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_POSITION_Y);
	m_v3EffectPosition.z = GetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_POSITION_Z);
	m_isFollowingEnable = IsDlgButtonChecked (IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_FOLLOWING);
	m_isFishingEffectFlag = IsDlgButtonChecked (IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_FISHING_EFFECT_FLAG);

	/////

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();
	CEffectManager::Instance().RegisterEffect (m_strEffectFileName, true);
}

void CObjectAnimationEventEffectToTarget::GetData (CRaceMotionDataAccessor::TMotionEventData * pData)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneObject * pSceneObject = pApplication->GetSceneObject();

	std::string strEffectFileName;
	StringPath (m_strEffectFileName, strEffectFileName);

	CRaceMotionDataAccessor::TMotionEffectToTargetEventData * pEffectToTargetEventData = (CRaceMotionDataAccessor::TMotionEffectToTargetEventData*)pData;
	pEffectToTargetEventData->dwEffectIndex = GetCaseCRC32 (strEffectFileName.c_str(), strEffectFileName.length());
	pEffectToTargetEventData->strEffectFileName = strEffectFileName;
	pEffectToTargetEventData->v3EffectPosition = m_v3EffectPosition;
	pEffectToTargetEventData->isFollowing = m_isFollowingEnable;
	pEffectToTargetEventData->isFishingEffect = m_isFishingEffectFlag;
}

void CObjectAnimationEventEffectToTarget::SetData (const CRaceMotionDataAccessor::TMotionEventData * c_pData)
{
	const CRaceMotionDataAccessor::TMotionEffectToTargetEventData * c_pEffectToTargetEventData = (const CRaceMotionDataAccessor::TMotionEffectToTargetEventData*)c_pData;

	const CEffectData * c_pEffectData;
	if (!CEffectManager::Instance().GetEffectData (c_pEffectToTargetEventData->dwEffectIndex, &c_pEffectData))
	{
		return;
	}

	SetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_FILE_NAME, c_pEffectData->GetFileName());
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_POSITION_X, c_pEffectToTargetEventData->v3EffectPosition.x);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_POSITION_Y, c_pEffectToTargetEventData->v3EffectPosition.y);
	SetDialogFloatText (GetSafeHwnd(), IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_POSITION_Z, c_pEffectToTargetEventData->v3EffectPosition.z);
	CheckDlgButton (IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_FOLLOWING, c_pEffectToTargetEventData->isFollowing);
	CheckDlgButton (IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_FISHING_EFFECT_FLAG, c_pEffectToTargetEventData->isFishingEffect);
}

/////////////////////////////////////////////////////////////////////////////
// CObjectAnimationEventEffectToTarget message handlers

void CObjectAnimationEventEffectToTarget::OnLoadEffect()
{
	DWORD dwFlag = OFN_NOCHANGEDIR;
	const char* c_szFilter = "Metin2 Effect Script Files (*.mse)|*.mse|*.*|All Files (*.*)|";
	COpenFileDialog FileOpener (this, dwFlag, NULL, c_szFilter, c_strYmirWorkPath.c_str());

	if (TRUE == FileOpener.DoModal())
	{
		SetDlgItemText (IDC_OBJECT_ANIMATION_EVENT_EFFECT_TO_TARGET_FILE_NAME, FileOpener.GetPathName());
	}
}

void CObjectAnimationEventEffectToTarget::OnOK()
{
}

void CObjectAnimationEventEffectToTarget::OnCancel()
{
}
