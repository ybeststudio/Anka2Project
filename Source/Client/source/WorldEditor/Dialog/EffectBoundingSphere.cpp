// EffectBoundingSphere.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "EffectBoundingSphere.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectBoundingSphere dialog


CEffectBoundingSphere::CEffectBoundingSphere (CWnd* pParent /*=NULL*/)
	: CDialog (CEffectBoundingSphere::IDD, pParent)
{
	m_bCallBackEnable = FALSE;

	//{{AFX_DATA_INIT(CEffectBoundingSphere)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEffectBoundingSphere::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CEffectBoundingSphere)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CEffectBoundingSphere, CDialog)
	//{{AFX_MSG_MAP(CEffectBoundingSphere)
	ON_EN_CHANGE (IDC_EFFECT_BOUNDING_SPHERE_RADIUS, OnChangeEffectBoundingSphereRadius)
	ON_EN_CHANGE (IDC_EFFECT_BOUNDING_SPHERE_POS_X, OnChangeEffectBoundingSpherePosX)
	ON_EN_CHANGE (IDC_EFFECT_BOUNDING_SPHERE_POS_Y, OnChangeEffectBoundingSpherePosY)
	ON_EN_CHANGE (IDC_EFFECT_BOUNDING_SPHERE_POS_Z, OnChangeEffectBoundingSpherePosZ)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectBoundingSphere message handlers

void CEffectBoundingSphere::OnOK()
{
}

void CEffectBoundingSphere::OnCancel()
{
}

BOOL CEffectBoundingSphere::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

void CEffectBoundingSphere::RefreshInfo()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	if (!pApplication)
	{
		return;
	}
	CSceneEffect * pSceneEffect = pApplication->GetSceneEffect();
	if (!pSceneEffect)
	{
		return;
	}
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();
	if (!pEffectAccessor)
	{
		return;
	}

	m_bCallBackEnable = FALSE;

	const D3DXVECTOR3 & c_rv3Position = pEffectAccessor->GetBoundingSpherePosition();
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_BOUNDING_SPHERE_POS_X, c_rv3Position.x);
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_BOUNDING_SPHERE_POS_Y, c_rv3Position.y);
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_BOUNDING_SPHERE_POS_Z, c_rv3Position.z);
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_BOUNDING_SPHERE_RADIUS, pEffectAccessor->GetBoundingSphereRadius());
	CheckDlgButton (IDC_EFFECT_BOUNDING_SPHERE_ENABLE, pEffectAccessor->GetBoundingSphereRadius() > 0.0f);

	m_bCallBackEnable = TRUE;
}

void CEffectBoundingSphere::SetBoundingSphereData()
{
	if (!m_bCallBackEnable)
	{
		return;
	}

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	if (!pApplication)
	{
		return;
	}
	CSceneEffect * pSceneEffect = pApplication->GetSceneEffect();
	if (!pSceneEffect)
	{
		return;
	}
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();
	if (!pEffectAccessor)
	{
		return;
	}

	float fRadius = GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_BOUNDING_SPHERE_RADIUS);
	D3DXVECTOR3 v3Position (GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_BOUNDING_SPHERE_POS_X),
							GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_BOUNDING_SPHERE_POS_Y),
							GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_BOUNDING_SPHERE_POS_Z));

	pEffectAccessor->SetBoundingSphereRadius (fRadius);
	pEffectAccessor->SetBoundingSpherePosition (v3Position);
}

void CEffectBoundingSphere::OnChangeEffectBoundingSphereRadius()
{
	SetBoundingSphereData();
}

void CEffectBoundingSphere::OnChangeEffectBoundingSpherePosX()
{
	SetBoundingSphereData();
}

void CEffectBoundingSphere::OnChangeEffectBoundingSpherePosY()
{
	SetBoundingSphereData();
}

void CEffectBoundingSphere::OnChangeEffectBoundingSpherePosZ()
{
	SetBoundingSphereData();
}
