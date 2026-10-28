// EffectTranslationDialog.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "EffectTranslationDialog.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectTranslationDialog dialog


CEffectTranslationDialog::CEffectTranslationDialog (IEventReciever * pEventReciever)
	: CDialog (CEffectTranslationDialog::IDD, NULL)
{
	//{{AFX_DATA_INIT(CEffectTranslationDialog)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pEventReciever = pEventReciever;
}


void CEffectTranslationDialog::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CEffectTranslationDialog)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CEffectTranslationDialog, CDialog)
	//{{AFX_MSG_MAP(CEffectTranslationDialog)
	ON_EN_CHANGE (IDC_EFFECT_TRANSLATION_X, OnChangePosition)
	ON_EN_CHANGE (IDC_EFFECT_TRANSLATION_Y, OnChangePosition)
	ON_EN_CHANGE (IDC_EFFECT_TRANSLATION_Z, OnChangePosition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectTranslationDialog normal functions

void CEffectTranslationDialog::GetPosition (float* pfx, float* pfy, float* pfz)
{
	*pfx = GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_TRANSLATION_X);
	*pfy = GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_TRANSLATION_Y);
	*pfz = GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_TRANSLATION_Z);
}

void CEffectTranslationDialog::SetPosition (float fx, float fy, float fz)
{
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_TRANSLATION_X, fx);
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_TRANSLATION_Y, fy);
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_TRANSLATION_Z, fz);
}

/////////////////////////////////////////////////////////////////////////////
// CEffectTranslationDialog message handlers

void CEffectTranslationDialog::OnChangePosition()
{
	m_pEventReciever->OnEvent();
}
