// EffectLightPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "../Dialog/EffectParticleTimeEventGraph.h"

#include "EffectLightPage.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

CEffectLightPage* CEffectLightPage::ms_pThis = NULL;

CTimeEventFloatAccessor LightRange;

/////////////////////////////////////////////////////////////////////////////
// CEffectLightPage dialog


CEffectLightPage::CEffectLightPage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CEffectLightPage::IDD, pParent)
{
	ms_pThis = this;
	//{{AFX_DATA_INIT(CEffectLightPage)
	//}}AFX_DATA_INIT
}


void CEffectLightPage::DoDataExchange (CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CEffectLightPage)
	DDX_Control (pDX, IDD_EFFECT_LIGHT_RANGE_GRAPH, m_ctrlRangeGraph);
	DDX_Control (pDX, IDC_EFFECT_LIGHT_DIFFUSE_ALPHA, m_ctrlDiffuseAlpha);
	DDX_Control (pDX, IDC_EFFECT_LIGHT_AMBIENT_ALPHA, m_ctrlAmbientAlpha);
	DDX_Control (pDX, IDC_EFFECT_LIGHT_AMBIENT, m_ctrlAmbient);
	DDX_Control (pDX, IDC_EFFECT_LIGHT_DIFFUSE, m_ctrlDiffuse);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP (CEffectLightPage, CPageCtrl)
	//{{AFX_MSG_MAP(CEffectLightPage)
	ON_WM_HSCROLL()
	ON_BN_CLICKED (IDC_EFFECT_LIGHT_LOOP, OnEffectLightLoop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
// CEffectLightPage normal functions

BOOL CEffectLightPage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CEffectLightPage::IDD, pParent))
	{
		return FALSE;
	}

	if (!m_ctrlAmbient.Create (this))
	{
		return FALSE;
	}

	if (!m_ctrlDiffuse.Create (this))
	{
		return FALSE;
	}

	m_ctrlRangeGraph.Initialize (WINDOW_TIMER_ID_LIGHT_GRAPH);

	/*CWorldEditorApp * pApplication = (CWorldEditorApp *)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement(m_dwIndex, &pElement))
		return FALSE;

	CSimpleLightAccessor* pAccessor = pElement->pSimpleLight;*/


	m_ctrlAmbient.pfnCallBack = CallBack;
	m_ctrlDiffuse.pfnCallBack = CallBack;

	return TRUE;
}

void CEffectLightPage::UpdateUI()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (m_dwIndex, &pElement))
	{
		return;
	}

	CLightAccessor* pAccessor = pElement->pLight;

	COLORREF DiffuseColor = RGB (WORD (pAccessor->GetDiffuseColorReference().r * 255.0f),
								 WORD (pAccessor->GetDiffuseColorReference().g * 255.0f),
								 WORD (pAccessor->GetDiffuseColorReference().b * 255.0f));
	COLORREF AmbientColor = RGB (WORD (pAccessor->GetAmbientColorReference().r * 255.0f),
								 WORD (pAccessor->GetAmbientColorReference().g * 255.0f),
								 WORD (pAccessor->GetAmbientColorReference().b * 255.0f));

	m_ctrlDiffuse.SetColor (DiffuseColor);
	m_ctrlAmbient.SetColor (AmbientColor);

	m_ctrlDiffuse.Update();
	m_ctrlAmbient.Update();

	LightRange.SetTablePointer (pAccessor->GetEmitterTimeEventTableRange());
	m_ctrlRangeGraph.SetAccessorPointer (&LightRange);
	m_ctrlRangeGraph.SetMaxValue (1.0f);
	m_ctrlRangeGraph.SetStartValue (0.0f);

	m_ctrlAmbientAlpha.SetPos (pAccessor->GetAmbientColorReference().a * 100.0f);
	m_ctrlDiffuseAlpha.SetPos (pAccessor->GetDiffuseColorReference().a * 100.0f);
}

void CEffectLightPage::SetData (DWORD dwIndex)
{
	m_dwIndex = dwIndex;

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (m_dwIndex, &pElement))
	{
		return;
	}

	CLightAccessor* pAccessor = pElement->pLight;

	// Color

	// Range
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_LIGHT_RANGE, pAccessor->GetMaxRangeReference());

	// Duration
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_LIGHT_DURATION, pAccessor->GetDurationReference());
	CheckDlgButton (IDC_EFFECT_LIGHT_LOOP, pAccessor->GetLoopFlagReference());
	SetDialogIntegerText (GetSafeHwnd(), IDC_EFFECT_LIGHT_LOOP_COUNT, pAccessor->GetLoopCount());

	// Attenuation
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_LIGHT_ATT0, pAccessor->GetAttenuation0Reference());
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_LIGHT_ATT1, pAccessor->GetAttenuation1Reference());
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_LIGHT_ATT2, pAccessor->GetAttenuation2Reference());

	UpdateUI();

}

void CEffectLightPage::Initialize()
{
	UpdateUI();
}

void CEffectLightPage::CallBack()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (ms_pThis->m_dwIndex, &pElement))
	{
		return;
	}

	CLightAccessor* pAccessor = pElement->pLight;

	COLORREF Diffuse;
	COLORREF Ambient;

	ms_pThis->m_ctrlDiffuse.GetColor (&Diffuse);
	ms_pThis->m_ctrlAmbient.GetColor (&Ambient);

	pAccessor->SetDiffuseColor (GetRValue (Diffuse) / 255.0f, GetGValue (Diffuse) / 255.0f, GetBValue (Diffuse) / 255.0f);
	pAccessor->SetAmbientColor (GetRValue (Ambient) / 255.0f, GetGValue (Ambient) / 255.0f, GetBValue (Ambient) / 255.0f);
}

/////////////////////////////////////////////////////////////////////////////
// CEffectLightPage message handlers

BOOL CEffectLightPage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (pEffectAccessor->GetElement (ms_pThis->m_dwIndex, &pElement))
	{
		CLightAccessor* pAccessor = pElement->pLight;

		switch (LOWORD (wParam))
		{
			case IDC_EFFECT_LIGHT_ATT0:
				pAccessor->GetAttenuation0Reference() = GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_LIGHT_ATT0);
				break;
			case IDC_EFFECT_LIGHT_ATT1:
				pAccessor->GetAttenuation1Reference() = GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_LIGHT_ATT1);
				break;
			case IDC_EFFECT_LIGHT_ATT2:
				pAccessor->GetAttenuation2Reference() = GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_LIGHT_ATT2);
				break;
			case IDC_EFFECT_LIGHT_RANGE:
				pAccessor->GetMaxRangeReference() = GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_LIGHT_RANGE);
				break;
			case IDC_EFFECT_LIGHT_DURATION:
				pAccessor->GetDurationReference() = GetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_LIGHT_DURATION);
				break;
			case IDC_EFFECT_LIGHT_LOOP_COUNT:
				pAccessor->GetLoopCountReference() = GetDialogIntegerText (GetSafeHwnd(), IDC_EFFECT_LIGHT_LOOP_COUNT);
				break;

		}
	}

	return CPageCtrl::OnCommand (wParam, lParam);
}

void CEffectLightPage::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (ms_pThis->m_dwIndex, &pElement))
	{
		return;
	}

	CLightAccessor* pAccessor = pElement->pLight;

	switch (m_iAlphaSliderIndex)
	{
		case IDC_EFFECT_LIGHT_AMBIENT_ALPHA:
			pAccessor->GetAmbientColorReference().a = float (m_ctrlAmbientAlpha.GetPos()) / 100.0f;
			break;
		case IDC_EFFECT_LIGHT_DIFFUSE_ALPHA:
			pAccessor->GetDiffuseColorReference().a = float (m_ctrlDiffuseAlpha.GetPos()) / 100.0f;
			break;
	}

	CPageCtrl::OnHScroll (nSBCode, nPos, pScrollBar);
}

BOOL CEffectLightPage::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	m_iAlphaSliderIndex = wParam;

	return CPageCtrl::OnNotify (wParam, lParam, pResult);
}

void CEffectLightPage::OnEffectLightLoop()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (ms_pThis->m_dwIndex, &pElement))
	{
		return;
	}

	CLightAccessor* pAccessor = pElement->pLight;

	pAccessor->GetLoopFlagReference() =
		IsDlgButtonChecked (IDC_EFFECT_LIGHT_LOOP);
}
