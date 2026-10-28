// MapEnvironmentPage.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "MapEnvironmentPage.h"

#include "MainFrm.h"
#include "WorldEditorDoc.h"

#include "../DataCtrl/MapAccessorOutdoor.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

BOOL CMapEnvironmentPage::ms_isCallBackFlag = TRUE;
CMapEnvironmentPage* CMapEnvironmentPage::ms_pThis = NULL;

/////////////////////////////////////////////////////////////////////////////
// CMapEnvironmentPage dialog


CMapEnvironmentPage::CMapEnvironmentPage (CWnd* pParent /*=NULL*/)
	: CPageCtrl (CMapEnvironmentPage::IDD, pParent)
{
	ms_pThis = this;
	ms_isCallBackFlag = TRUE;
	//{{AFX_DATA_INIT(CMapEnvironmentPage)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMapEnvironmentPage::DoDataExchange (CDataExchange* pDX)
{
	CPageCtrl::DoDataExchange (pDX);
	//{{AFX_DATA_MAP(CMapEnvironmentPage)
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_LENSFLARE_MAIN_FLARE_SIZE, m_ctrlMainFlareSize);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_LENSFLARE_MAX_BRIGHTNESS, m_ctrlLensFlareMaxBrightness);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_LENSFLARE_COLOR, m_ctrlLensFlareBrightnessColor);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_GRADIENT_SECOND_COLOR, m_ctrlSkyBoxGradientSecondColor);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_GRADIENT_FIRST_COLOR, m_ctrlSkyBoxGradientFirstColor);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_GRADIENT_LIST, m_ctrlSkyBoxGradientList);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_FILTERING_ALPHA_DEST, m_ctrlFilteringAlphaDest);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_FILTERING_ALPHA_SRC, m_ctrlFilteringAlphaSrc);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_FILTERING_ALPHA, m_ctrlFilteringAlpha);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_FILTERING_COLOR, m_ctrlFilteringColor);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_WIND_RANDOM, m_ctrlWindRandom);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_WIND_STRENGTH, m_ctrlWindStrength);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_FOG_NEAR_DISTANCE, m_ctrlFogNearDistance);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_FOG_FAR_DISTANCE, m_ctrlFogFarDistance);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_MATERIAL_EMISSIVE, m_ctrlMaterialEmissive);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_LIGHT_AMBIENT, m_ctrlLightAmbient);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_LIGHT_DIFFUSE, m_ctrlLightDiffuse);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_MATERIAL_AMBIENT, m_ctrlMaterialAmbient);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_MATERIAL_DIFFUSE, m_ctrlMaterialDiffuse);
	DDX_Control (pDX, IDC_MAP_ENVIRONMENT_FOG_COLOR, m_ctrlFogColor);
	//}}AFX_DATA_MAP

	m_ctrlWindStrength.SetRangeMin (0);
	m_ctrlWindStrength.SetRangeMax (200);
	m_ctrlWindStrength.SetPos (0);
	m_ctrlWindRandom.SetRangeMin (0);
	m_ctrlWindRandom.SetRangeMax (100);
	m_ctrlWindRandom.SetPos (0);
	m_ctrlFilteringAlpha.SetRangeMin (0);
	m_ctrlFilteringAlpha.SetRangeMax (100);
	m_ctrlFilteringAlpha.SetPos (0);
	m_ctrlLensFlareMaxBrightness.SetRangeMin (0);
	m_ctrlLensFlareMaxBrightness.SetRangeMax (100);
	m_ctrlMainFlareSize.SetRangeMin (0);
	m_ctrlMainFlareSize.SetRangeMax (100);
}


BEGIN_MESSAGE_MAP (CMapEnvironmentPage, CPageCtrl)
	//{{AFX_MSG_MAP(CMapEnvironmentPage)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_FOG_ENABLE, OnEnableFog)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LIGHT_ENABLE, OnEnableLight)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_FILTERING_ENABLE, OnEnableFiltering)
	ON_WM_HSCROLL()
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LOAD_SCRIPT, OnLoadEnvironmentScript)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_SAVE_SCRIPT, OnSaveEnvironmentScript)
	ON_BN_CLICKED (IDC_CHECK_LIGHTPOSITIONEDIT, OnCheckLightpositionedit)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LOAD_CLOUD_TEXTURE, OnLoadCloudTexture)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_INSERT_UPPER_GRADIENT, OnInsertGradientUpper)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_INSERT_LOWER_GRADIENT, OnOnInsertGradientLower)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_DELETE_GRADIENT, OnDeleteGradient)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LENSFLARE_ENABLE, OnCheckLensFlareEnable)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LOAD_LENSFLARE_TEXTURE, OnLoadLensFlareTexture)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_MAINFLARE_ENABLE, OnCheckMainFlareEnable)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LENSFLARE_ENABLE2, &CMapEnvironmentPage::OnBnClickedSkyBoxModeTexture)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LOAD_CLOUD_TEXTURE2, &CMapEnvironmentPage::OnSetSkyBoxFrontTexture)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LOAD_CLOUD_TEXTURE4, &CMapEnvironmentPage::OnSetSkyBoxBackTexture)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LOAD_CLOUD_TEXTURE5, &CMapEnvironmentPage::OnSetSkyBoxLeftTexture)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LOAD_CLOUD_TEXTURE6, &CMapEnvironmentPage::OnSetSkyBoxRightTexture)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LOAD_CLOUD_TEXTURE3, &CMapEnvironmentPage::OnSetSkyBoxTopTexture)
	ON_BN_CLICKED (IDC_MAP_ENVIRONMENT_LOAD_CLOUD_TEXTURE8, &CMapEnvironmentPage::OnSetSkyBoxBottomTexture)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapEnvironmentPage normal functions

BOOL CMapEnvironmentPage::Create (CWnd * pParent)
{
	if (!CPageCtrl::Create (CMapEnvironmentPage::IDD, pParent))
	{
		return FALSE;
	}

	if (!m_ctrlMaterialDiffuse.Create (this))
	{
		return FALSE;
	}
	if (!m_ctrlMaterialAmbient.Create (this))
	{
		return FALSE;
	}
	if (!m_ctrlMaterialEmissive.Create (this))
	{
		return FALSE;
	}
	if (!m_ctrlLightDiffuse.Create (this))
	{
		return FALSE;
	}
	if (!m_ctrlLightAmbient.Create (this))
	{
		return FALSE;
	}
	if (!m_ctrlFogColor.Create (this))
	{
		return FALSE;
	}
	if (!m_ctrlFilteringColor.Create (this))
	{
		return FALSE;
	}
	if (!m_ctrlFilteringAlphaDest.Create())
	{
		return FALSE;
	}
	if (!m_ctrlFilteringAlphaSrc.Create())
	{
		return FALSE;
	}
	if (!m_ctrlSkyBoxGradientFirstColor.Create (this))
	{
		return FALSE;
	}
	if (!m_ctrlSkyBoxGradientSecondColor.Create (this))
	{
		return FALSE;
	}
	if (!m_ctrlLensFlareBrightnessColor.Create (this))
	{
		return FALSE;
	}

	m_ctrlMaterialDiffuse.pfnCallBack = CallBack;
	m_ctrlMaterialAmbient.pfnCallBack = CallBack;
	m_ctrlMaterialEmissive.pfnCallBack = CallBack;
	m_ctrlLightDiffuse.pfnCallBack = CallBack;
	m_ctrlLightAmbient.pfnCallBack = CallBack;
	m_ctrlFogColor.pfnCallBack = CallBack;
	m_ctrlFilteringColor.pfnCallBack = CallBack;
	m_ctrlSkyBoxGradientFirstColor.pfnCallBack = CallBack;
	m_ctrlSkyBoxGradientSecondColor.pfnCallBack = CallBack;
	m_ctrlLensFlareBrightnessColor.pfnCallBack = CallBack;

	m_iSliderIndex = 0;

	return TRUE;
}

void CMapEnvironmentPage::Initialize()
{
	UpdateUI();
}

void CMapEnvironmentPage::UpdateUI()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	const TEnvironmentData * c_pEnvironmentData;
	pMapManagerAccessor->GetEnvironmentData (&c_pEnvironmentData);

	COLORREF MaterialDiffuseColor = RGB (WORD (c_pEnvironmentData->Material.Diffuse.r * 255.0f),
										 WORD (c_pEnvironmentData->Material.Diffuse.g * 255.0f),
										 WORD (c_pEnvironmentData->Material.Diffuse.b * 255.0f));
	COLORREF MaterialAmbientColor = RGB (WORD (c_pEnvironmentData->Material.Ambient.r * 255.0f),
										 WORD (c_pEnvironmentData->Material.Ambient.g * 255.0f),
										 WORD (c_pEnvironmentData->Material.Ambient.b * 255.0f));
	COLORREF MaterialEmissiveColor = RGB (WORD (c_pEnvironmentData->Material.Emissive.r * 255.0f),
										  WORD (c_pEnvironmentData->Material.Emissive.g * 255.0f),
										  WORD (c_pEnvironmentData->Material.Emissive.b * 255.0f));
	COLORREF LightDiffuseColor = RGB (WORD (c_pEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.r * 255.0f),
									  WORD (c_pEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.g * 255.0f),
									  WORD (c_pEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.b * 255.0f));
	COLORREF LightAmbientColor = RGB (WORD (c_pEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.r * 255.0f),
									  WORD (c_pEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.g * 255.0f),
									  WORD (c_pEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.b * 255.0f));
	COLORREF FogColor		   = RGB (WORD (c_pEnvironmentData->FogColor.r * 255.0f),
								  WORD (c_pEnvironmentData->FogColor.g * 255.0f),
								  WORD (c_pEnvironmentData->FogColor.b * 255.0f));
	COLORREF FilteringColor    = RGB (WORD (c_pEnvironmentData->FilteringColor.r * 255.0f),
									  WORD (c_pEnvironmentData->FilteringColor.g * 255.0f),
									  WORD (c_pEnvironmentData->FilteringColor.b * 255.0f));

	m_ctrlMaterialDiffuse.SetColor (MaterialDiffuseColor);
	m_ctrlMaterialAmbient.SetColor (MaterialAmbientColor);
	m_ctrlMaterialEmissive.SetColor (MaterialEmissiveColor);
	m_ctrlLightDiffuse.SetColor (LightDiffuseColor);
	m_ctrlLightAmbient.SetColor (LightAmbientColor);
	m_ctrlFogColor.SetColor (FogColor);
	m_ctrlFilteringColor.SetColor (FilteringColor);
	m_ctrlMaterialDiffuse.Update();
	m_ctrlMaterialAmbient.Update();
	m_ctrlMaterialEmissive.Update();
	m_ctrlLightDiffuse.Update();
	m_ctrlLightAmbient.Update();
	m_ctrlFogColor.Update();
	m_ctrlFilteringColor.Update();
	m_ctrlWindStrength.SetPos (long (c_pEnvironmentData->fWindStrength));
	m_ctrlWindRandom.SetPos (long (c_pEnvironmentData->fWindRandom));
	m_ctrlFilteringAlpha.SetPos (long (c_pEnvironmentData->FilteringColor.a * 100.0f));

	m_ctrlFogNearDistance.SetLimitText (16);
	m_ctrlFogFarDistance.SetLimitText (16);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_FOG_FAR_DISTANCE, c_pEnvironmentData->m_fFogFarDistance);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_FOG_NEAR_DISTANCE, c_pEnvironmentData->m_fFogNearDistance);

	CheckDlgButton (IDC_MAP_ENVIRONMENT_FOG_ENABLE, c_pEnvironmentData->bFogEnable);
	CheckDlgButton (IDC_MAP_ENVIRONMENT_LIGHT_ENABLE, c_pEnvironmentData->bDirLightsEnable[ENV_DIRLIGHT_BACKGROUND]);
	CheckDlgButton (IDC_MAP_ENVIRONMENT_FILTERING_ENABLE, c_pEnvironmentData->bFilteringEnable);

	m_ctrlFilteringAlphaSrc.SelectBlendType (c_pEnvironmentData->byFilteringAlphaSrc);
	m_ctrlFilteringAlphaDest.SelectBlendType (c_pEnvironmentData->byFilteringAlphaDest);

	// SkyBox
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_SCALE_X, c_pEnvironmentData->v3SkyBoxScale.x);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_SCALE_Y, c_pEnvironmentData->v3SkyBoxScale.y);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_SCALE_Z, c_pEnvironmentData->v3SkyBoxScale.z);

	//Check
	CheckDlgButton (IDC_MAP_ENVIRONMENT_LENSFLARE_ENABLE2, c_pEnvironmentData->bSkyBoxTextureRenderMode);
	SetDlgItemText (IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_FILE_NAME2, c_pEnvironmentData->strSkyBoxFaceFileName[0].c_str()); // front
	SetDlgItemText (IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_FILE_NAME4, c_pEnvironmentData->strSkyBoxFaceFileName[1].c_str()); // back
	SetDlgItemText (IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_FILE_NAME5, c_pEnvironmentData->strSkyBoxFaceFileName[2].c_str()); // left
	SetDlgItemText (IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_FILE_NAME6, c_pEnvironmentData->strSkyBoxFaceFileName[3].c_str()); // right
	SetDlgItemText (IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_FILE_NAME3, c_pEnvironmentData->strSkyBoxFaceFileName[4].c_str()); // top
	SetDlgItemText (IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_FILE_NAME8, c_pEnvironmentData->strSkyBoxFaceFileName[5].c_str()); // bottom

	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_CLOUD_SCALE_X, c_pEnvironmentData->v2CloudScale.x);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_CLOUD_SCALE_Y, c_pEnvironmentData->v2CloudScale.y);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_SCALE_X, c_pEnvironmentData->v2CloudTextureScale.x);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_SCALE_Y, c_pEnvironmentData->v2CloudTextureScale.y);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_CLOUD_SPEED_X, c_pEnvironmentData->v2CloudSpeed.x);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_CLOUD_SPEED_Y, c_pEnvironmentData->v2CloudSpeed.y);
	SetDialogFloatText (GetSafeHwnd(), IDC_MAP_ENVIRONMENT_CLOUD_HEIGHT, c_pEnvironmentData->fCloudHeight);
	SetDlgItemText (IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_FILE_NAME, c_pEnvironmentData->strCloudTextureFileName.c_str());

	CheckDlgButton (IDC_MAP_ENVIRONMENT_LENSFLARE_ENABLE, c_pEnvironmentData->bLensFlareEnable);
	m_ctrlLensFlareBrightnessColor.SetColor (c_pEnvironmentData->LensFlareBrightnessColor);
	m_ctrlLensFlareMaxBrightness.SetPos (int (c_pEnvironmentData->fLensFlareMaxBrightness * 100.0f));
	SetDlgItemText (IDC_MAP_ENVIRONMENT_LENSFLARE_TEXTURE_FILE_NAME, c_pEnvironmentData->strMainFlareTextureFileName.c_str());

	CheckDlgButton (IDC_MAP_ENVIRONMENT_MAINFLARE_ENABLE, pMapManagerAccessor->GetMainFlareEnableReference());
	m_ctrlMainFlareSize.SetPos (int (c_pEnvironmentData->fMainFlareSize * 100.0f));

	COLORREF BrightnessColor = RGB (WORD (c_pEnvironmentData->LensFlareBrightnessColor.r * 255.0f),
									WORD (c_pEnvironmentData->LensFlareBrightnessColor.g * 255.0f),
									WORD (c_pEnvironmentData->LensFlareBrightnessColor.b * 255.0f));
	m_ctrlLensFlareBrightnessColor.SetColor (BrightnessColor);

	RebuildGradientList();
}

void CMapEnvironmentPage::RebuildGradientList()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	const TEnvironmentData * c_pEnvironmentData;
	pMapManagerAccessor->GetEnvironmentData (&c_pEnvironmentData);

	char szGradientName[32 + 1];
	DWORD dwControlIndex = 0;

	m_ctrlSkyBoxGradientList.ResetContent();
	for (DWORD i = 0; i < c_pEnvironmentData->bySkyBoxGradientLevelUpper; ++i)
	{
		_snprintf (szGradientName, 32, "Upper %02d", i);
		m_ctrlSkyBoxGradientList.InsertString (dwControlIndex++, szGradientName);
	}
	for (DWORD j = 0; j < c_pEnvironmentData->bySkyBoxGradientLevelLower; ++j)
	{
		_snprintf (szGradientName, 32, "Lower %02d", j);
		m_ctrlSkyBoxGradientList.InsertString (dwControlIndex++, szGradientName);
	}

	m_ctrlSkyBoxGradientList.InsertString (dwControlIndex++, "Cloud");

	if (m_ctrlSkyBoxGradientList.GetCount() > 0)
	{
		CString strGradientName;
		m_ctrlSkyBoxGradientList.GetLBText (0, strGradientName);
		m_ctrlSkyBoxGradientList.SelectString (0, strGradientName);
		SelectGradient (0);
	}
	else
	{
		m_ctrlSkyBoxGradientList.InsertString (0, "None");
		m_ctrlSkyBoxGradientList.SelectString (0, "None");
	}
}

void CMapEnvironmentPage::CallBack()
{
	if (!ms_isCallBackFlag)
	{
		return;
	}

	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	COLORREF MaterialDiffuse;
	COLORREF MaterialAmbient;
	COLORREF MaterialEmissive;
	COLORREF LightDiffuse;
	COLORREF LightAmbient;
	COLORREF FogColor;
	COLORREF FilteringColor;
	COLORREF FirstGradientColor;
	COLORREF SecondGradientColor;
	COLORREF BrightnessColor;

	ms_pThis->m_ctrlMaterialDiffuse.GetColor (&MaterialDiffuse);
	ms_pThis->m_ctrlMaterialAmbient.GetColor (&MaterialAmbient);
	ms_pThis->m_ctrlMaterialEmissive.GetColor (&MaterialEmissive);
	ms_pThis->m_ctrlLightDiffuse.GetColor (&LightDiffuse);
	ms_pThis->m_ctrlLightAmbient.GetColor (&LightAmbient);
	ms_pThis->m_ctrlFogColor.GetColor (&FogColor);
	ms_pThis->m_ctrlFilteringColor.GetColor (&FilteringColor);
	ms_pThis->m_ctrlSkyBoxGradientFirstColor.GetColor (&FirstGradientColor);
	ms_pThis->m_ctrlSkyBoxGradientSecondColor.GetColor (&SecondGradientColor);
	ms_pThis->m_ctrlLensFlareBrightnessColor.GetColor (&BrightnessColor);

	pMapManagerAccessor->RefreshEnvironmentData();

	pMapManagerAccessor->SetMaterialDiffuseColor (GetRValue (MaterialDiffuse) / 255.0f, GetGValue (MaterialDiffuse) / 255.0f, GetBValue (MaterialDiffuse) / 255.0f);
	pMapManagerAccessor->SetMaterialAmbientColor (GetRValue (MaterialAmbient) / 255.0f, GetGValue (MaterialAmbient) / 255.0f, GetBValue (MaterialAmbient) / 255.0f);
	pMapManagerAccessor->SetMaterialEmissiveColor (GetRValue (MaterialEmissive) / 255.0f, GetGValue (MaterialEmissive) / 255.0f, GetBValue (MaterialEmissive) / 255.0f);
	pMapManagerAccessor->SetLightDiffuseColor (GetRValue (LightDiffuse) / 255.0f, GetGValue (LightDiffuse) / 255.0f, GetBValue (LightDiffuse) / 255.0f);
	pMapManagerAccessor->SetLightAmbientColor (GetRValue (LightAmbient) / 255.0f, GetGValue (LightAmbient) / 255.0f, GetBValue (LightAmbient) / 255.0f);
	pMapManagerAccessor->SetFogColor (GetRValue (FogColor) / 255.0f, GetGValue (FogColor) / 255.0f, GetBValue (FogColor) / 255.0f);
	pMapManagerAccessor->SetFilteringColor (GetRValue (FilteringColor) / 255.0f, GetGValue (FilteringColor) / 255.0f, GetBValue (FilteringColor) / 255.0f);

	TGradientColor * pGradientColor;
	if (pMapManagerAccessor->GetSkyBoxGradientColorPointer (ms_pThis->m_ctrlSkyBoxGradientList.GetCurSel(), &pGradientColor))
	{
		pGradientColor->m_SecondColor = TColor (GetRValue (SecondGradientColor) / 255.0f, GetGValue (SecondGradientColor) / 255.0f, GetBValue (SecondGradientColor) / 255.0f);
		pGradientColor->m_FirstColor = TColor (GetRValue (FirstGradientColor) / 255.0f, GetGValue (FirstGradientColor) / 255.0f, GetBValue (FirstGradientColor) / 255.0f);
		pMapManagerAccessor->RefreshSkyBox();
	}

	if (pMapManagerAccessor->GetLensFlareBrightnessColorReference().r != GetRValue (BrightnessColor) / 255.0f ||
																		 pMapManagerAccessor->GetLensFlareBrightnessColorReference().g != GetGValue (BrightnessColor) / 255.0f ||
																			 pMapManagerAccessor->GetLensFlareBrightnessColorReference().b != GetBValue (BrightnessColor) / 255.0f)
	{
		pMapManagerAccessor->GetLensFlareBrightnessColorReference().r = GetRValue (BrightnessColor) / 255.0f;
		pMapManagerAccessor->GetLensFlareBrightnessColorReference().g = GetGValue (BrightnessColor) / 255.0f;
		pMapManagerAccessor->GetLensFlareBrightnessColorReference().b = GetBValue (BrightnessColor) / 255.0f;
		pMapManagerAccessor->RefreshLensFlare();
	}

	pMapManagerAccessor->SetTerrainModified();
}

void CMapEnvironmentPage::TurnOnCallBack()
{
	ms_isCallBackFlag = TRUE;
}

void CMapEnvironmentPage::TurnOffCallBack()
{
	ms_isCallBackFlag = FALSE;
}

void CMapEnvironmentPage::SelectGradient (DWORD dwIndex)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	TGradientColor * pGradientColor;
	if (!pMapManagerAccessor->GetSkyBoxGradientColorPointer (dwIndex, &pGradientColor))
	{
		return;
	}

	COLORREF FirstColor = RGB (WORD (pGradientColor->m_FirstColor.r * 255.0f),
							   WORD (pGradientColor->m_FirstColor.g * 255.0f),
							   WORD (pGradientColor->m_FirstColor.b * 255.0f));
	COLORREF SecondColor = RGB (WORD (pGradientColor->m_SecondColor.r * 255.0f),
								WORD (pGradientColor->m_SecondColor.g * 255.0f),
								WORD (pGradientColor->m_SecondColor.b * 255.0f));

	m_ctrlSkyBoxGradientFirstColor.SetColor (FirstColor);
	m_ctrlSkyBoxGradientFirstColor.Update();
	m_ctrlSkyBoxGradientSecondColor.SetColor (SecondColor);
	m_ctrlSkyBoxGradientSecondColor.Update();
}

/////////////////////////////////////////////////////////////////////////////
// CMapEnvironmentPage message handlers

void CMapEnvironmentPage::OnEnableFog()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	pMapManagerAccessor->EnableFog (IsDlgButtonChecked (IDC_MAP_ENVIRONMENT_FOG_ENABLE));
}

void CMapEnvironmentPage::OnEnableLight()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	pMapManagerAccessor->EnableLight (IsDlgButtonChecked (IDC_MAP_ENVIRONMENT_LIGHT_ENABLE));
}

void CMapEnvironmentPage::OnEnableFiltering()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	pMapManagerAccessor->EnableFiltering (IsDlgButtonChecked (IDC_MAP_ENVIRONMENT_FILTERING_ENABLE));
}

BOOL CMapEnvironmentPage::OnNotify (WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	m_iSliderIndex = wParam;

	return CPageCtrl::OnNotify (wParam, lParam, pResult);
}

BOOL CMapEnvironmentPage::GetMapManagerAccessor (CMapManagerAccessor** ppMapManagerAccessor)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	*ppMapManagerAccessor = pApplication->GetMapManagerAccessor();

	if (NULL == *ppMapManagerAccessor)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CMapEnvironmentPage::OnCommand (WPARAM wParam, LPARAM lParam)
{
	CMapManagerAccessor * pMapManagerAccessor;
	if (GetMapManagerAccessor (&pMapManagerAccessor))
		switch (LOWORD (wParam))
		{
			// Fog
			case IDC_MAP_ENVIRONMENT_FOG_NEAR_DISTANCE:
			{
				char szDistance[256];
				//				m_ctrlFogNearDistance.GetLine(0, szDistance);
				m_ctrlFogNearDistance.GetWindowText (szDistance, 256);

				float fNear = atof (szDistance);

				Tracef ("[INFO] Read changed fNear value: : %s -> %f\n", szDistance, fNear);

				//				m_ctrlFogFarDistance.GetLine(0, szDistance);
				m_ctrlFogFarDistance.GetWindowText (szDistance, 256);

				float fFar = atof (szDistance);

				Tracef ("[INFO] Read existing fFar value: %s -> %f\n", szDistance, fFar);

				if (fNear >= fFar)
				{
					fNear = fFar;
				}

				pMapManagerAccessor->SetFogNearDistance (fNear);
				Tracef ("[INFO] Setting fNear: %f\n", fNear);
			}
			break;
			case IDC_MAP_ENVIRONMENT_FOG_FAR_DISTANCE:
			{
				char szDistance[256];
				//				m_ctrlFogFarDistance.GetLine(0, szDistance);
				m_ctrlFogFarDistance.GetWindowText (szDistance, 256);

				float fFar = atof (szDistance);

				Tracef ("[INFO] Read changed fFar value: %s -> %f\n", szDistance, fFar);

				//				m_ctrlFogNearDistance.GetLine(0, szDistance);
				m_ctrlFogNearDistance.GetWindowText (szDistance, 256);

				float fNear = atof (szDistance);

				Tracef ("[INFO] Read existing fNear value: %s -> %f\n", szDistance, fNear);

				if (fNear >= fFar)
				{
					fFar = fNear;
				}

				pMapManagerAccessor->SetFogFarDistance (fFar);
				Tracef ("[INFO] Setting fFar: %f\n", fFar);
			}
			break;

			// Filtering
			case IDC_MAP_ENVIRONMENT_FILTERING_ALPHA_SRC:
				pMapManagerAccessor->SetFilteringAlphaSrc (m_ctrlFilteringAlphaSrc.GetBlendType());
				break;
			case IDC_MAP_ENVIRONMENT_FILTERING_ALPHA_DEST:
				pMapManagerAccessor->SetFilteringAlphaDest (m_ctrlFilteringAlphaDest.GetBlendType());
				break;

			// SkyBox
			case IDC_MAP_ENVIRONMENT_SCALE_X:
				pMapManagerAccessor->GetSkyBoxScaleReference().x = GetDialogFloatText (GetSafeHwnd(), LOWORD (wParam));
				pMapManagerAccessor->RefreshSkyBox();
				break;
			case IDC_MAP_ENVIRONMENT_SCALE_Y:
				pMapManagerAccessor->GetSkyBoxScaleReference().y = GetDialogFloatText (GetSafeHwnd(), LOWORD (wParam));
				pMapManagerAccessor->RefreshSkyBox();
				break;
			case IDC_MAP_ENVIRONMENT_SCALE_Z:
				pMapManagerAccessor->GetSkyBoxScaleReference().z = GetDialogFloatText (GetSafeHwnd(), LOWORD (wParam));
				pMapManagerAccessor->RefreshSkyBox();
				break;
			case IDC_MAP_ENVIRONMENT_CLOUD_SCALE_X:
				pMapManagerAccessor->GetSkyBoxCloudScaleReference().x = GetDialogFloatText (GetSafeHwnd(), LOWORD (wParam));
				pMapManagerAccessor->RefreshSkyBox();
				break;
			case IDC_MAP_ENVIRONMENT_CLOUD_SCALE_Y:
				pMapManagerAccessor->GetSkyBoxCloudScaleReference().y = GetDialogFloatText (GetSafeHwnd(), LOWORD (wParam));
				pMapManagerAccessor->RefreshSkyBox();
				break;
			case IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_SCALE_X:
				pMapManagerAccessor->GetSkyBoxCloudTextureScaleReference().x = GetDialogFloatText (GetSafeHwnd(), LOWORD (wParam));
				pMapManagerAccessor->RefreshSkyBox();
				break;
			case IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_SCALE_Y:
				pMapManagerAccessor->GetSkyBoxCloudTextureScaleReference().y = GetDialogFloatText (GetSafeHwnd(), LOWORD (wParam));
				pMapManagerAccessor->RefreshSkyBox();
				break;
			case IDC_MAP_ENVIRONMENT_CLOUD_SPEED_X:
				pMapManagerAccessor->GetSkyBoxCloudSpeedReference().x = GetDialogFloatText (GetSafeHwnd(), LOWORD (wParam));
				pMapManagerAccessor->RefreshSkyBox();
				break;
			case IDC_MAP_ENVIRONMENT_CLOUD_SPEED_Y:
				pMapManagerAccessor->GetSkyBoxCloudSpeedReference().y = GetDialogFloatText (GetSafeHwnd(), LOWORD (wParam));
				pMapManagerAccessor->RefreshSkyBox();
				break;
			case IDC_MAP_ENVIRONMENT_CLOUD_HEIGHT:
				pMapManagerAccessor->GetSkyBoxCloudHeightReference() = GetDialogFloatText (GetSafeHwnd(), LOWORD (wParam));
				pMapManagerAccessor->RefreshSkyBox();
				break;
			case IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_FILE_NAME:
			{
				CString strTextureFileName;
				GetDlgItemText (IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_FILE_NAME, strTextureFileName);
				pMapManagerAccessor->GetSkyBoxCloudTextureFileNameReference() = strTextureFileName;
				pMapManagerAccessor->RefreshSkyBox();
			}
			break;

			case IDC_MAP_ENVIRONMENT_GRADIENT_LIST:
				SelectGradient (m_ctrlSkyBoxGradientList.GetCurSel());
				break;

			// LensFlare
			case IDC_MAP_ENVIRONMENT_LENSFLARE_TEXTURE_FILE_NAME:
			{
				CString strTextureFileName;
				GetDlgItemText (IDC_MAP_ENVIRONMENT_LENSFLARE_TEXTURE_FILE_NAME, strTextureFileName);
				pMapManagerAccessor->GetMainFlareTextureFileNameReference() = strTextureFileName;
				pMapManagerAccessor->RefreshLensFlare();
			}
			break;
		}
	return CPageCtrl::OnCommand (wParam, lParam);
}

void CMapEnvironmentPage::OnHScroll (UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	switch (m_iSliderIndex)
	{
		case IDC_MAP_ENVIRONMENT_WIND_STRENGTH:
			pMapManagerAccessor->SetWindStrength (m_ctrlWindStrength.GetPos() / 100.0f);
			break;

		case IDC_MAP_ENVIRONMENT_WIND_RANDOM:
			pMapManagerAccessor->SetWindStrength (float (m_ctrlWindRandom.GetPos()) / 100.0f);
			break;

		case IDC_MAP_ENVIRONMENT_FILTERING_ALPHA:
			pMapManagerAccessor->SetFilteringAlpha (float (m_ctrlFilteringAlpha.GetPos()) / 100.0f);
			break;

		case IDC_MAP_ENVIRONMENT_LENSFLARE_MAX_BRIGHTNESS:
			pMapManagerAccessor->GetLensFlareMaxBrightnessReference() = float (m_ctrlLensFlareMaxBrightness.GetPos()) / 100.0f;
			pMapManagerAccessor->RefreshLensFlare();
			break;

		case IDC_MAP_ENVIRONMENT_LENSFLARE_MAIN_FLARE_SIZE:
			pMapManagerAccessor->GetMainFlareSizeReference() = float (m_ctrlMainFlareSize.GetPos()) / 100.0f;
			pMapManagerAccessor->RefreshLensFlare();
			break;
	}

	CPageCtrl::OnHScroll (nSBCode, nPos, pScrollBar);
}

void CMapEnvironmentPage::OnLoadEnvironmentScript()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Metin2 Environment Script Files (*.msenv)|*.msenv|All Files (*.*)|*.*|";

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

		pMapManagerAccessor->LoadEnvironmentScript (FileOpener.GetPathName());

		// NOTE: 환경 Load시 진짜로 데이터 로드만 하고, 맵 데이터에 해당 환경에 대한 정보는 저장하지 않아서 맵 저장시 환경 이름이 사라지는 문제 수정
		std::string strEnvironmentName;
		GetOnlyFileName (FileOpener.GetPathName(), strEnvironmentName);
		pApplication->GetMapManagerAccessor()->GetMapOutdoorPtr()->SetEnvironmentDataName (strEnvironmentName);

		TurnOffCallBack();
		UpdateUI();
		TurnOnCallBack();
	}
}

void CMapEnvironmentPage::OnSaveEnvironmentScript()
{
	DWORD dwFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Metin2 Environment Script Files (*.msenv)|*.msenv|All Files (*.*)|*.*|";

	CFileDialog FileOpener (FALSE, "Save", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

		pMapManagerAccessor->SaveEnvironmentScript (FileOpener.GetPathName());
	}
}

void CMapEnvironmentPage::OnCheckLightpositionedit()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CSceneMap * pSceneMap = pApplication->GetSceneMap();

	CMainFrame* mainFrame = (CMainFrame*)AfxGetApp()->m_pMainWnd;
	CWorldEditorDoc* doc = (CWorldEditorDoc*)mainFrame->GetActiveView()->GetDocument();

	if (TRUE == IsDlgButtonChecked (IDC_CHECK_LIGHTPOSITIONEDIT))
	{
		pSceneMap->SetLightPositionEditingOn();
		doc->SetEditingDirectionalLight (true);
	}
	else// if (TRUE == IsDlgButtonChecked(IDC_CHECK_LIGHTPOSITIONEDIT))
	{
		pSceneMap->SetLightPositionEditingOff();
		doc->SetEditingDirectionalLight (false);
	}
}

void CMapEnvironmentPage::OnLoadCloudTexture()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Texture Files (*.tga)|*.tga|All Files (*.*)|*.*|";

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

		pMapManagerAccessor->GetSkyBoxCloudTextureFileNameReference() = FileOpener.GetPathName();
		pMapManagerAccessor->RefreshSkyBox();

		SetDlgItemText (IDC_MAP_ENVIRONMENT_CLOUD_TEXTURE_FILE_NAME, FileOpener.GetPathName());
	}
}

void CMapEnvironmentPage::OnInsertGradientUpper()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	pMapManagerAccessor->InsertGradientUpper();

	RebuildGradientList();
	pMapManagerAccessor->RefreshSkyBox();
}

void CMapEnvironmentPage::OnOnInsertGradientLower()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	pMapManagerAccessor->InsertGradientLower();

	RebuildGradientList();
	pMapManagerAccessor->RefreshSkyBox();
}

void CMapEnvironmentPage::OnDeleteGradient()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	pMapManagerAccessor->DeleteGradient (m_ctrlSkyBoxGradientList.GetCurSel());

	RebuildGradientList();
	pMapManagerAccessor->RefreshSkyBox();
}

void CMapEnvironmentPage::OnCheckLensFlareEnable()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	pMapManagerAccessor->GetLensFlareEnableReference() = IsDlgButtonChecked (IDC_MAP_ENVIRONMENT_LENSFLARE_ENABLE);
	pMapManagerAccessor->RefreshLensFlare();
}

void CMapEnvironmentPage::OnCheckMainFlareEnable()
{
	CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
	CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

	pMapManagerAccessor->GetMainFlareEnableReference() = IsDlgButtonChecked (IDC_MAP_ENVIRONMENT_MAINFLARE_ENABLE);
	pMapManagerAccessor->RefreshLensFlare();
}

void CMapEnvironmentPage::OnLoadLensFlareTexture()
{
	DWORD dwFlag = OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	const char* c_szFilter = "Texture Files (*.tga)|*.tga|All Files (*.*)|*.*|";

	CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

	if (TRUE == FileOpener.DoModal())
	{
		CWorldEditorApp * pApplication = (CWorldEditorApp*)AfxGetApp();
		CMapManagerAccessor * pMapManagerAccessor = pApplication->GetMapManagerAccessor();

		pMapManagerAccessor->GetMainFlareTextureFileNameReference() = FileOpener.GetPathName();
		pMapManagerAccessor->RefreshLensFlare();

		SetDlgItemText (IDC_MAP_ENVIRONMENT_LENSFLARE_TEXTURE_FILE_NAME, FileOpener.GetPathName());
	}
}


void CMapEnvironmentPage::OnBnClickedSkyBoxModeTexture()
{
	CMapManagerAccessor* pAccessor = NULL;
	GetMapManagerAccessor (&pAccessor);

	if (pAccessor)
	{
		pAccessor->SetSkyBoxTextureRenderMode (IsDlgButtonChecked (IDC_MAP_ENVIRONMENT_LENSFLARE_ENABLE2));
	}
}

void CMapEnvironmentPage::SetSkyBoxTexture (int iFaceIndex)
{
	CMapManagerAccessor* pAccessor = NULL;
	GetMapManagerAccessor (&pAccessor);

	if (pAccessor)
	{
		DWORD dwFlag = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
		const char* c_szFilter = "Texture Files (*.dds;*.jpg)|*.dds;*.jpg|All Files (*.*)|*.*|";
		CFileDialog FileOpener (TRUE, "Load", "", dwFlag, c_szFilter, this);

		if (TRUE == FileOpener.DoModal())
		{
			pAccessor->SetSkyBoxFaceTexture (FileOpener.GetPathName(), iFaceIndex);
			pAccessor->RefreshSkyBox();

			UpdateUI();
		}
	}
}

void CMapEnvironmentPage::OnSetSkyBoxFrontTexture()
{
	SetSkyBoxTexture (0);
}

void CMapEnvironmentPage::OnSetSkyBoxBackTexture()
{
	SetSkyBoxTexture (1);
}

void CMapEnvironmentPage::OnSetSkyBoxLeftTexture()
{
	SetSkyBoxTexture (2);
}

void CMapEnvironmentPage::OnSetSkyBoxRightTexture()
{
	SetSkyBoxTexture (3);
}

void CMapEnvironmentPage::OnSetSkyBoxTopTexture()
{
	SetSkyBoxTexture (4);
}

void CMapEnvironmentPage::OnSetSkyBoxBottomTexture()
{
	SetSkyBoxTexture (5);
}
