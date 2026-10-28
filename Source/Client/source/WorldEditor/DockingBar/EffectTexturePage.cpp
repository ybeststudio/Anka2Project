// EffectTexturePage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "EffectTexturePage.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectTexturePage dialog


CEffectTexturePage::CEffectTexturePage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CEffectTexturePage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEffectTexturePage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEffectTexturePage::DoDataExchange (CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CEffectTexturePage)
	DDX_Control (pDX, IDC_EFFECT_TEXTURE_ALPHA_DEST, m_ctrlBlendDestType);
	DDX_Control (pDX, IDC_EFFECT_TEXTURE_ALPHA_SRC, m_ctrlBlendSrcType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CEffectTexturePage, CPageCtrl)
	//{{AFX_MSG_MAP(CEffectTexturePage)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectTexturePage normal functions

BOOL CEffectTexturePage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CEffectTexturePage::IDD, pParent))
	{
		return FALSE;
	}

	if (!m_ctrlBlendDestType.Create())
	{
		return FALSE;
	}
	if (!m_ctrlBlendSrcType.Create())
	{
		return FALSE;
	}

	return TRUE;
}

void CEffectTexturePage::UpdateUI()
{
}

void CEffectTexturePage::SetData (DWORD dwIndex)
{
}

/////////////////////////////////////////////////////////////////////////////
// CEffectTexturePage message handlers

BOOL CEffectTexturePage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD (wParam))
	{
		case IDC_EFFECT_TEXTURE_ALPHA_SRC:
		case IDC_EFFECT_TEXTURE_ALPHA_DEST:
			//			SetBlendType(m_ctrlBlendSrcType.GetBlendType(), m_ctrlBlendDestType.GetBlendType());
			break;
	}

	return CPageCtrl::OnCommand (wParam, lParam);
}