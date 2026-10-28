// EffectMeshPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "EffectMeshPage.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

CTimeEventFloatAccessor MeshAlphaAccessor;

CEffectMeshPage* CEffectMeshPage::ms_pThis = NULL;

/////////////////////////////////////////////////////////////////////////////
// CEffectMeshPage dialog


CEffectMeshPage::CEffectMeshPage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CEffectMeshPage::IDD, pParent)
{
	m_dwElementIndex = 0;
	m_dwMeshIndex = 0;
	ms_pThis = this;
	//{{AFX_DATA_INIT(CEffectMeshPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CEffectMeshPage::DoDataExchange (CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CEffectMeshPage)
	DDX_Control (pDX, IDD_EFFECT_MESH_ALPHA_GRAPH, m_ctrlAlphaGraph);
	DDX_Control (pDX, IDC_EFFECT_MESH_COLOR_OPERATION, m_ctrlColorOperation);
	DDX_Control (pDX, IDC_EFFECT_MESH_COLOR_FACTOR, m_ctrlColorFactor);
	DDX_Control (pDX, IDC_EFFECT_MESH_ANIMATION_DELAY, m_ctrlMeshAnimationDelay);
	DDX_Control (pDX, IDC_EFFECT_MESH_TEXTURE_ANIMATION_DELAY, m_ctrlTextureAnimationDelay);
	DDX_Control (pDX, IDC_EFFECT_MESH_ALPHA_DEST, m_ctrlBlendDestType);
	DDX_Control (pDX, IDC_EFFECT_MESH_ALPHA_SRC, m_ctrlBlendSrcType);
	DDX_Control (pDX, IDC_EFFECT_MESH_LIST, m_ctrlMeshList);
	//}}AFX_DATA_MAP

	m_ctrlMeshAnimationDelay.SetRangeMin (0);
	m_ctrlMeshAnimationDelay.SetRangeMax (50);
	m_ctrlTextureAnimationDelay.SetRangeMin (0);
	m_ctrlTextureAnimationDelay.SetRangeMax (100);

	m_ctrlMeshList.ResetContent();
	m_ctrlMeshList.InsertString (0, "None");
	m_ctrlMeshList.SelectString (0, "None");
}


BEGIN_MESSAGE_MAP (CEffectMeshPage, CPageCtrl)
	//{{AFX_MSG_MAP(CEffectMeshPage)
	ON_BN_CLICKED (IDC_EFFECT_MESH_MODEL_LOAD, OnLoadModel)
	ON_BN_CLICKED (IDC_EFFECT_MESH_BILLBOARD_OFF, OnSelectBillboardOff)
	ON_BN_CLICKED (IDC_EFFECT_MESH_BILLBOARD_ALL, OnSelectBillboardAll)
	ON_BN_CLICKED (IDC_EFFECT_MESH_BILLBOARD_Y, OnSelectBillboardY)
	ON_BN_CLICKED (IDC_EFFECT_MESH_BLEND_ENABLE, OnEnableBlend)
	ON_WM_HSCROLL()
	ON_BN_CLICKED (IDC_EFFECT_MESH_TEXTURE_ANIMATION_START_FRAME_ACCEPT, OnAcceptTextureAnimationStartFrame)
	ON_BN_CLICKED (IDC_EFFECT_MESH_TEXTURE_ALPHA_ENABLE, OnEnableTextureAlpha)
	ON_BN_CLICKED (IDC_EFFECT_MESH_BILLBOARD_ACCEPT_ALL, OnAcceptAllBillboard)
	ON_BN_CLICKED (IDC_EFFECT_MESH_ALPHA_ACCEPT_ALL, OnAcceptAllAlpha)
	ON_BN_CLICKED (IDC_EFFECT_MESH_TEXTURE_ANIMATION_ACCEPT_ALL, OnAcceptAllTextureAnimation)
	ON_BN_CLICKED (IDC_EFFECT_MESH_BILLBOARD_MOVE, OnEffectMeshAlignMove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEffectMeshPage normal functions

BOOL CEffectMeshPage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CEffectMeshPage::IDD, pParent))
	{
		return FALSE;
	}

	if (!m_ctrlBlendSrcType.Create())
	{
		return FALSE;
	}
	if (!m_ctrlBlendDestType.Create())
	{
		return FALSE;
	}
	if (!m_ctrlColorFactor.Create (this))
	{
		return FALSE;
	}
	if (!m_ctrlColorOperation.Create())
	{
		return FALSE;
	}

	m_ctrlAlphaGraph.Initialize (WINDOW_TIMER_ID_MESH_ALPHA_GRAPH);

	m_ctrlColorFactor.pfnCallBack = CallBack;

	return TRUE;
}

void CEffectMeshPage::UpdateUI()
{
}

void CEffectMeshPage::SetData (DWORD dwEffectIndex)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	m_dwElementIndex = dwEffectIndex;

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (dwEffectIndex, &pElement))
	{
		return;
	}

	if (CEffectAccessor::EFFECT_ELEMENT_TYPE_MESH != pElement->iType)
	{
		return;
	}

	CMeshAccessor * pMesh = pElement->pMesh;

	if (strlen (pMesh->GetMeshFileName()) > 0)
	{
		std::string strMeshName;
		GetOnlyFileName (pMesh->GetMeshFileName(), strMeshName);
		SetDlgItemText (IDC_EFFECT_MESH_MODEL_NAME, strMeshName.c_str());
	}
	else
	{
		SetDlgItemText (IDC_EFFECT_MESH_MODEL_NAME, "None");
	}

	// Mesh Animation
	if (pMesh->isMeshAnimationLoop())
	{
		CheckDlgButton (IDC_EFFECT_MESH_ANIMATION_LOOP, TRUE);
	}
	else
	{
		CheckDlgButton (IDC_EFFECT_MESH_ANIMATION_LOOP, FALSE);
	}

	SetDialogIntegerText (GetSafeHwnd(), IDC_EFFECT_MESH_LOOP_COUNT, pMesh->GetMeshAnimationLoopCount());

	// Mesh Animation Looping
	float fMeshDelay = pMesh->GetTextureAnimationFrameDelay (m_dwMeshIndex);
	m_ctrlMeshAnimationDelay.SetPos (int (fMeshDelay * 1000.0f));
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_MESH_ANIMATION_DELAY_PRINT, fMeshDelay);

	//////////////////////////

	pMesh->LoadMeshInstance();
	RefreshMeshElementList();
	SetElement (dwEffectIndex, 0);
}

void CEffectMeshPage::SetElement (DWORD dwEffectIndex, DWORD dwElementIndex)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	m_dwElementIndex = dwEffectIndex;
	m_dwMeshIndex = dwElementIndex;

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (dwEffectIndex, &pElement))
	{
		return;
	}

	if (CEffectAccessor::EFFECT_ELEMENT_TYPE_MESH != pElement->iType)
	{
		return;
	}

	CMeshAccessor * pMesh = pElement->pMesh;

	// Billboard
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_OFF, FALSE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_ALL, FALSE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_Y, FALSE);
	switch (pMesh->GetBillboardType (m_dwMeshIndex))
	{
		case MESH_BILLBOARD_TYPE_NONE:
			CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_OFF, TRUE);
			break;
		case MESH_BILLBOARD_TYPE_ALL:
			CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_ALL, TRUE);
			break;
		case MESH_BILLBOARD_TYPE_Y:
			CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_Y, TRUE);
			break;
		case MESH_BILLBOARD_TYPE_MOVE:
			CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_MOVE, TRUE);
	}

	TTimeEventTableFloat * TimeEventAlpha;

	if (pMesh->GetTimeTableAlphaPointer (m_dwMeshIndex, &TimeEventAlpha))
	{
		MeshAlphaAccessor.SetTablePointer (TimeEventAlpha);
		m_ctrlAlphaGraph.SetAccessorPointer (&MeshAlphaAccessor);
		m_ctrlAlphaGraph.SetMaxValue (1.0f);
		m_ctrlAlphaGraph.SetStartValue (0.0f);
	}
	else
	{
		MeshAlphaAccessor.SetTablePointer (NULL);
		m_ctrlAlphaGraph.SetAccessorPointer (NULL);
	}

	// Blending
	int iSrcBlendingType = pMesh->GetBlendingSrcType (m_dwMeshIndex);
	int iDestBlendingType = pMesh->GetBlendingDestType (m_dwMeshIndex);
	m_ctrlBlendSrcType.SelectBlendType (iSrcBlendingType);
	m_ctrlBlendDestType.SelectBlendType (iDestBlendingType);
	CheckDlgButton (IDC_EFFECT_MESH_BLEND_ENABLE, pMesh->isBlendingEnable (m_dwMeshIndex));

	// Texture Animation
	if (pMesh->isTextureAnimationLoop (m_dwMeshIndex))
	{
		CheckDlgButton (IDC_EFFECT_MESH_TEXTURE_ANIMATION_LOOP, TRUE);
	}
	else
	{
		CheckDlgButton (IDC_EFFECT_MESH_TEXTURE_ANIMATION_LOOP, FALSE);
	}

	// Texture Animation Looping
	float fTextureDelay = pMesh->GetTextureAnimationFrameDelay (m_dwMeshIndex);
	m_ctrlTextureAnimationDelay.SetPos (int (fTextureDelay * 1000.0f));
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_MESH_TEXTURE_ANIMATION_DELAY_PRINT, fTextureDelay);
	SetDialogIntegerText (GetSafeHwnd(), IDC_EFFECT_MESH_TEXTURE_ANIMATION_START_FRAME, pMesh->GetTextureAnimationStartFrame (m_dwMeshIndex));

	BYTE byColorOperationType;
	D3DXCOLOR ColorFactor;
	if (pMesh->GetColorOperationType (m_dwMeshIndex, &byColorOperationType))
	{
		m_ctrlColorOperation.SelectOperationType (byColorOperationType);
	}
	if (pMesh->GetColorFactor (m_dwMeshIndex, &ColorFactor))
	{
		COLORREF Color = RGB (WORD (ColorFactor.r * 255.0f),
							  WORD (ColorFactor.g * 255.0f),
							  WORD (ColorFactor.b * 255.0f));
		m_ctrlColorFactor.SetColor (Color);
		m_ctrlColorFactor.Update();
	}
}

void CEffectMeshPage::CallBack()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectMeshScript::TMeshData * pMeshData;
	if (!ms_pThis->GetMeshElementData (ms_pThis->m_dwElementIndex, ms_pThis->m_dwMeshIndex, &pMeshData))
	{
		return;
	}

	COLORREF ColorFactor;
	ms_pThis->m_ctrlColorFactor.GetColor (&ColorFactor);
	pMeshData->ColorFactor.r = GetRValue (ColorFactor) / 255.0f;
	pMeshData->ColorFactor.g = GetGValue (ColorFactor) / 255.0f;
	pMeshData->ColorFactor.b = GetBValue (ColorFactor) / 255.0f;
}

bool CEffectMeshPage::GetEffectElement (DWORD dwIndex, CEffectAccessor::TEffectElement** ppElement)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectAccessor::TEffectElement * pElement;
	if (!pEffectAccessor->GetElement (dwIndex, &pElement))
	{
		return false;
	}

	if (CEffectAccessor::EFFECT_ELEMENT_TYPE_MESH != pElement->iType)
	{
		return false;
	}

	*ppElement = pElement;

	return true;
}

DWORD CEffectMeshPage::GetMeshElementCount (DWORD dwEffectIndex)
{
	CEffectAccessor::TEffectElement * pEffectElement;
	if (!GetEffectElement (dwEffectIndex, &pEffectElement))
	{
		return 0;
	}

	return pEffectElement->pMesh->GetMeshElementCount();
}

bool CEffectMeshPage::GetMeshElementData (DWORD dwEffectIndex, DWORD dwMeshIndex, CEffectMeshScript::TMeshData** ppMeshData)
{
	CEffectAccessor::TEffectElement * pEffectElement;
	if (!GetEffectElement (dwEffectIndex, &pEffectElement))
	{
		return false;
	}

	if (!pEffectElement->pMesh->GetMeshDataPointer (dwMeshIndex, ppMeshData))
	{
		return false;
	}

	return true;
}

void CEffectMeshPage::RefreshMeshElementList()
{
	CEffectAccessor::TEffectElement * pEffectElement;
	if (!GetEffectElement (m_dwElementIndex, &pEffectElement))
	{
		return;
	}

	m_ctrlMeshList.ResetContent();
	for (DWORD i = 0; i < pEffectElement->pMesh->GetMeshElementCount(); ++i)
	{
		CEffectMesh::TEffectMeshData * pMeshElementData;
		if (!pEffectElement->pMesh->GetMeshElementDataPointer (i, &pMeshElementData))
		{
			continue;
		}

		m_ctrlMeshList.InsertString (i, pMeshElementData->szObjectName);

		if (0 == i)
		{
			m_ctrlMeshList.SelectString (i, pMeshElementData->szObjectName);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEffectMeshPage message handlers

void CEffectMeshPage::OnLoadModel()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Model Files (*.mde)|*.mde|All Files (*.*)|*.*|";

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

		CEffectAccessor::TEffectElement * pElement;
		if (!pEffectAccessor->GetElement (m_dwElementIndex, &pElement))
		{
			return;
		}

		if (CEffectAccessor::EFFECT_ELEMENT_TYPE_MESH != pElement->iType)
		{
			return;
		}

		SetDlgItemText (IDC_EFFECT_MESH_MODEL_NAME, FileOpener.GetFileName());

		pElement->pMesh->SetMeshFileName (FileOpener.GetPathName());
		RefreshMeshElementList();
		SetData (m_dwElementIndex);
	}
}

void CEffectMeshPage::SetBillboardType (int iType)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectMeshScript::TMeshData * pMeshData;
	if (!GetMeshElementData (m_dwElementIndex, m_dwMeshIndex, &pMeshData))
	{
		return;
	}

	pMeshData->byBillboardType = iType;
}

void CEffectMeshPage::SetBlendType (int iSrcType, int iDestType)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

	CEffectMeshScript::TMeshData * pMeshData;
	if (!GetMeshElementData (m_dwElementIndex, m_dwMeshIndex, &pMeshData))
	{
		return;
	}

	pMeshData->byBlendingSrcType = iSrcType;
	pMeshData->byBlendingDestType = iDestType;
}

void CEffectMeshPage::OnSelectBillboardOff()
{
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_OFF, TRUE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_ALL, FALSE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_Y, FALSE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_MOVE, FALSE);
	SetBillboardType (MESH_BILLBOARD_TYPE_NONE);
}
void CEffectMeshPage::OnSelectBillboardAll()
{
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_OFF, FALSE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_ALL, TRUE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_Y, FALSE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_MOVE, FALSE);
	SetBillboardType (MESH_BILLBOARD_TYPE_ALL);
}
void CEffectMeshPage::OnSelectBillboardY()
{
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_OFF, FALSE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_ALL, FALSE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_Y, TRUE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_MOVE, FALSE);
	SetBillboardType (MESH_BILLBOARD_TYPE_Y);
}

void CEffectMeshPage::OnEnableBlend()
{
	CEffectMeshScript::TMeshData * pMeshData;
	if (!GetMeshElementData (m_dwElementIndex, m_dwMeshIndex, &pMeshData))
	{
		return;
	}

	pMeshData->bBlendingEnable = IsDlgButtonChecked (IDC_EFFECT_MESH_BLEND_ENABLE);
}

void CEffectMeshPage::OnEnableTextureAlpha()
{
	CEffectMeshScript::TMeshData * pMeshData;
	if (!GetMeshElementData (m_dwElementIndex, m_dwMeshIndex, &pMeshData))
	{
		return;
	}

	pMeshData->bTextureAlphaEnable = IsDlgButtonChecked (IDC_EFFECT_MESH_TEXTURE_ALPHA_ENABLE);
}

void CEffectMeshPage::OnAcceptTextureAnimationStartFrame()
{
	CEffectMeshScript::TMeshData * pMeshData;
	if (!GetMeshElementData (m_dwElementIndex, m_dwMeshIndex, &pMeshData))
	{
		return;
	}

	CString strStartFrame;
	GetDlgItemText (IDC_EFFECT_MESH_TEXTURE_ANIMATION_START_FRAME, strStartFrame);

	pMeshData->dwTextureAnimationStartFrame = atoi (strStartFrame);
}

void CEffectMeshPage::OnAcceptAllBillboard()
{
	DWORD dwMeshElementCount = GetMeshElementCount (m_dwElementIndex);

	if (0 == dwMeshElementCount)
	{
		return;
	}

	int iType = MESH_BILLBOARD_TYPE_NONE;
	if (IsDlgButtonChecked (IDC_EFFECT_MESH_BILLBOARD_OFF))
	{
		iType = MESH_BILLBOARD_TYPE_NONE;
	}
	else if (IsDlgButtonChecked (IDC_EFFECT_MESH_BILLBOARD_ALL))
	{
		iType = MESH_BILLBOARD_TYPE_ALL;
	}
	else if (IsDlgButtonChecked (IDC_EFFECT_MESH_BILLBOARD_Y))
	{
		iType = MESH_BILLBOARD_TYPE_Y;
	}
	else if (IsDlgButtonChecked (IDC_EFFECT_MESH_BILLBOARD_MOVE))
	{
		iType = MESH_BILLBOARD_TYPE_MOVE;
	}

	for (DWORD i = 0; i < dwMeshElementCount; ++i)
	{
		CEffectMeshScript::TMeshData * pMeshData;
		if (!GetMeshElementData (m_dwElementIndex, i, &pMeshData))
		{
			continue;
		}

		pMeshData->byBillboardType = iType;
	}
}

void CEffectMeshPage::OnAcceptAllAlpha()
{
	DWORD dwMeshElementCount = GetMeshElementCount (m_dwElementIndex);

	if (0 == dwMeshElementCount)
	{
		return;
	}

	BOOL isAlphaEnable = IsDlgButtonChecked (IDC_EFFECT_MESH_BLEND_ENABLE);
	BOOL isTextureAlphaEnable = IsDlgButtonChecked (IDC_EFFECT_MESH_TEXTURE_ALPHA_ENABLE);
	int iSrcType = m_ctrlBlendSrcType.GetBlendType();
	int iDestType = m_ctrlBlendDestType.GetBlendType();
	int iOperationType = m_ctrlColorOperation.GetOperationType();

	D3DXCOLOR Color;
	COLORREF ColorFactor;
	ms_pThis->m_ctrlColorFactor.GetColor (&ColorFactor);
	Color.r = GetRValue (ColorFactor) / 255.0f;
	Color.g = GetGValue (ColorFactor) / 255.0f;
	Color.b = GetBValue (ColorFactor) / 255.0f;

	//TTimeEventTableFloat AlphaTable = AlphaAccessor.m_pTimeEventTable;

	for (DWORD i = 0; i < dwMeshElementCount; ++i)
	{
		CEffectMeshScript::TMeshData * pMeshData;
		if (!GetMeshElementData (m_dwElementIndex, i, &pMeshData))
		{
			continue;
		}

		pMeshData->bBlendingEnable = isAlphaEnable;
		pMeshData->bTextureAlphaEnable = isTextureAlphaEnable;
		pMeshData->byBlendingSrcType = iSrcType;
		pMeshData->byBlendingDestType = iDestType;
		pMeshData->byColorOperationType = iOperationType;
		pMeshData->ColorFactor = Color;

		pMeshData->TimeEventAlpha = *MeshAlphaAccessor.m_pTimeEventTable;
	}
}

void CEffectMeshPage::OnAcceptAllTextureAnimation()
{
	DWORD dwMeshElementCount = GetMeshElementCount (m_dwElementIndex);

	if (0 == dwMeshElementCount)
	{
		return;
	}

	BOOL isTextureAnimationEnable = IsDlgButtonChecked (IDC_EFFECT_MESH_TEXTURE_ANIMATION_LOOP);
	float fTextureAnimationFrameDelay = float (m_ctrlTextureAnimationDelay.GetPos()) / 1000.0f;

	for (DWORD i = 0; i < dwMeshElementCount; ++i)
	{
		CEffectMeshScript::TMeshData * pMeshData;
		if (!GetMeshElementData (m_dwElementIndex, i, &pMeshData))
		{
			continue;
		}

		pMeshData->bTextureAnimationLoopEnable = isTextureAnimationEnable;
		pMeshData->fTextureAnimationFrameDelay = fTextureAnimationFrameDelay;
	}
}

BOOL CEffectMeshPage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD (wParam))
	{
		case IDC_EFFECT_MESH_LOOP_COUNT:
		{
			CEffectAccessor::TEffectElement * pEffect;
			if (GetEffectElement (m_dwElementIndex, &pEffect))
			{
				pEffect->pMesh->SetMeshAnimationLoopCount (GetDialogIntegerText (GetSafeHwnd(), IDC_EFFECT_MESH_LOOP_COUNT));
			}
		}
		break;
		case IDC_EFFECT_MESH_ALPHA_SRC:
		case IDC_EFFECT_MESH_ALPHA_DEST:
			SetBlendType (m_ctrlBlendSrcType.GetBlendType(), m_ctrlBlendDestType.GetBlendType());
			break;
		case IDC_EFFECT_MESH_COLOR_OPERATION:
		{
			CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
			CEffectAccessor * pEffectAccessor = pApplication->GetEffectAccessor();

			CEffectMeshScript::TMeshData * pMeshData;
			if (GetMeshElementData (m_dwElementIndex, m_dwMeshIndex, &pMeshData))
			{
				pMeshData->byColorOperationType = m_ctrlColorOperation.GetOperationType();
			}
		}
		break;
		case IDC_EFFECT_MESH_ANIMATION_LOOP:
		{
			CEffectAccessor::TEffectElement * pEffect;
			if (GetEffectElement (m_dwElementIndex, &pEffect))
			{
				int isChecked = IsDlgButtonChecked (IDC_EFFECT_MESH_ANIMATION_LOOP);
				pEffect->pMesh->SetMeshAnimationFlag (isChecked == 1 ? true : false);
			}
		}
		break;
		case IDC_EFFECT_MESH_TEXTURE_ANIMATION_LOOP:
		{
			CEffectMeshScript::TMeshData * pMeshData;
			if (GetMeshElementData (m_dwElementIndex, m_dwMeshIndex, &pMeshData))
			{
				int isChecked = IsDlgButtonChecked (IDC_EFFECT_MESH_TEXTURE_ANIMATION_LOOP);
				pMeshData->bTextureAnimationLoopEnable = isChecked == 1 ? true : false;
			}
		}
		break;
		case IDC_EFFECT_MESH_LIST:
			m_dwMeshIndex = m_ctrlMeshList.GetCurSel();
			SetElement (m_dwElementIndex, m_dwMeshIndex);
			break;
	}

	return CPageCtrl::OnCommand (wParam, lParam);
}

void CEffectMeshPage::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CEffectAccessor::TEffectElement * pEffectElement;
	if (!GetEffectElement (m_dwElementIndex, &pEffectElement))
	{
		return;
	}
	CEffectMeshScript::TMeshData * pMeshData;
	if (!pEffectElement->pMesh->GetMeshDataPointer (m_dwMeshIndex, &pMeshData))
	{
		return;
	}

	float fMeshDelay = float (m_ctrlMeshAnimationDelay.GetPos()) / 1000.0f;
	pEffectElement->pMesh->SetMeshAnimationFrameDelay (fMeshDelay);
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_MESH_ANIMATION_DELAY_PRINT, fMeshDelay);

	float fTextureDelay = float (m_ctrlTextureAnimationDelay.GetPos()) / 1000.0f;
	pMeshData->fTextureAnimationFrameDelay = fTextureDelay;
	SetDialogFloatText (GetSafeHwnd(), IDC_EFFECT_MESH_TEXTURE_ANIMATION_DELAY_PRINT, fTextureDelay);

	CPageCtrl::OnHScroll (nSBCode, nPos, pScrollBar);
}

void CEffectMeshPage::OnEffectMeshAlignMove()
{
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_OFF, FALSE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_ALL, FALSE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_Y, FALSE);
	CheckDlgButton (IDC_EFFECT_MESH_BILLBOARD_MOVE, TRUE);
	SetBillboardType (MESH_BILLBOARD_TYPE_MOVE);
}
