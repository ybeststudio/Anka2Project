#include "StdAfx.h"
#include "MapManagerAccessor.h"
#include "MapAccessorOutdoor.h"

void CMapManagerAccessor::GetEnvironmentData (const TEnvironmentData** c_ppEnvironmentData)
{
	*c_ppEnvironmentData = &m_EnvironmentData;
}

void CMapManagerAccessor::RefreshEnvironmentData()
{
	SetEnvironmentDataPtr (&m_EnvironmentData);
}

void CMapManagerAccessor::SetMaterialDiffuseColor (float fr, float fg, float fb)
{
	m_EnvironmentData.Material.Diffuse.r = fr;
	m_EnvironmentData.Material.Diffuse.g = fg;
	m_EnvironmentData.Material.Diffuse.b = fb;
}

void CMapManagerAccessor::SetMaterialAmbientColor (float fr, float fg, float fb)
{
	m_EnvironmentData.Material.Ambient.r = fr;
	m_EnvironmentData.Material.Ambient.g = fg;
	m_EnvironmentData.Material.Ambient.b = fb;
}

void CMapManagerAccessor::SetMaterialEmissiveColor (float fr, float fg, float fb)
{
	m_EnvironmentData.Material.Emissive.r = fr;
	m_EnvironmentData.Material.Emissive.g = fg;
	m_EnvironmentData.Material.Emissive.b = fb;
}

void CMapManagerAccessor::SetLightDirection (float fx, float fy, float fz)
{
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.x = fx;
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.y = fy;
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.z = fz;
}

void CMapManagerAccessor::SetLightDiffuseColor (float fr, float fg, float fb)
{
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.r = fr;
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.g = fg;
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.b = fb;
}

void CMapManagerAccessor::SetLightAmbientColor (float fr, float fg, float fb)
{
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.r = fr;
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.g = fg;
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.b = fb;
}

void CMapManagerAccessor::EnableLight (BOOL bFlag)
{
	m_EnvironmentData.bDirLightsEnable[ENV_DIRLIGHT_BACKGROUND] = bFlag;
}

void CMapManagerAccessor::EnableFog (BOOL bFlag)
{
	m_EnvironmentData.bFogEnable = bFlag;
}

void CMapManagerAccessor::SetFogColor (float fr, float fg, float fb)
{
	m_EnvironmentData.FogColor.r = fr;
	m_EnvironmentData.FogColor.g = fg;
	m_EnvironmentData.FogColor.b = fb;
}

void CMapManagerAccessor::SetFogNearDistance (float fDistance)
{
	m_EnvironmentData.m_fFogNearDistance = fDistance;
}

void CMapManagerAccessor::SetFogFarDistance (float fDistance)
{
	m_EnvironmentData.m_fFogFarDistance = fDistance;
}

void CMapManagerAccessor::SetWindStrength (float fStrength)
{
	m_EnvironmentData.fWindStrength = fStrength;
}

void CMapManagerAccessor::SetWindRandom (float fRandom)
{
	m_EnvironmentData.fWindRandom = fRandom;
}

void CMapManagerAccessor::EnableFiltering (BOOL bFlag)
{
	m_EnvironmentData.bFilteringEnable = bFlag;
}

void CMapManagerAccessor::SetFilteringColor (float fr, float fg, float fb)
{
	m_EnvironmentData.FilteringColor.r = fr;
	m_EnvironmentData.FilteringColor.g = fg;
	m_EnvironmentData.FilteringColor.b = fb;
}

void CMapManagerAccessor::SetFilteringAlpha (float fAlpha)
{
	m_EnvironmentData.FilteringColor.a = fAlpha;
}

void CMapManagerAccessor::SetFilteringAlphaSrc (BYTE byAlphaSrc)
{
	m_EnvironmentData.byFilteringAlphaSrc = byAlphaSrc;
}

void CMapManagerAccessor::SetFilteringAlphaDest (BYTE byAlphaDest)
{
	m_EnvironmentData.byFilteringAlphaDest = byAlphaDest;
}

void CMapManagerAccessor::SetSkyBoxTextureRenderMode (BOOL bTextureMode)
{
	m_EnvironmentData.bSkyBoxTextureRenderMode = bTextureMode;
}

BOOL CMapManagerAccessor::IsSkyBoxTextureRenderMode()
{
	return m_EnvironmentData.bSkyBoxTextureRenderMode;
}

void CMapManagerAccessor::SetSkyBoxFaceTexture (const char* pFileName, int iFaceIndex)
{
	if (iFaceIndex < 0 || iFaceIndex > 5)
	{
		return;
	}

	m_EnvironmentData.strSkyBoxFaceFileName[iFaceIndex] = pFileName;
}

std::string CMapManagerAccessor::GetSkyBoxFaceTextre (int iFaceIndex)
{
	if (iFaceIndex < 0 || iFaceIndex > 5)
	{
		return "";
	}

	return m_EnvironmentData.strSkyBoxFaceFileName[iFaceIndex];
}

D3DXVECTOR3& CMapManagerAccessor::GetSkyBoxScaleReference()
{
	return m_EnvironmentData.v3SkyBoxScale;
}

D3DXVECTOR2& CMapManagerAccessor::GetSkyBoxCloudScaleReference()
{
	return m_EnvironmentData.v2CloudScale;
}

D3DXVECTOR2& CMapManagerAccessor::GetSkyBoxCloudTextureScaleReference()
{
	return m_EnvironmentData.v2CloudTextureScale;
}

D3DXVECTOR2& CMapManagerAccessor::GetSkyBoxCloudSpeedReference()
{
	return m_EnvironmentData.v2CloudSpeed;
}

float& CMapManagerAccessor::GetSkyBoxCloudHeightReference()
{
	return m_EnvironmentData.fCloudHeight;
}

std::string& CMapManagerAccessor::GetSkyBoxCloudTextureFileNameReference()
{
	return m_EnvironmentData.strCloudTextureFileName;
}

BYTE& CMapManagerAccessor::GetSkyBoxGradientUpperReference()
{
	return m_EnvironmentData.bySkyBoxGradientLevelUpper;
}
BYTE& CMapManagerAccessor::GetSkyBoxGradientLowerReference()
{
	return m_EnvironmentData.bySkyBoxGradientLevelLower;
}
BOOL CMapManagerAccessor::GetSkyBoxGradientColorPointer (DWORD dwIndex, TGradientColor** ppGradientColor)
{
	if (dwIndex >= m_EnvironmentData.SkyBoxGradientColorVector.size())
	{
		if (dwIndex == m_EnvironmentData.SkyBoxGradientColorVector.size())
		{
			*ppGradientColor = &m_EnvironmentData.CloudGradientColor;
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}

	*ppGradientColor = &m_EnvironmentData.SkyBoxGradientColorVector[dwIndex];

	return TRUE;
}

void CMapManagerAccessor::InsertGradientUpper()
{
	std::vector<TGradientColor> TempGradientVector;

	++m_EnvironmentData.bySkyBoxGradientLevelUpper;
	TempGradientVector = m_EnvironmentData.SkyBoxGradientColorVector;

	m_EnvironmentData.SkyBoxGradientColorVector.clear();
	m_EnvironmentData.SkyBoxGradientColorVector.resize (m_EnvironmentData.bySkyBoxGradientLevelUpper + m_EnvironmentData.bySkyBoxGradientLevelLower);

	for (DWORD i = 0; i < TempGradientVector.size(); ++i)
	{
		m_EnvironmentData.SkyBoxGradientColorVector[i] = TempGradientVector[i];
	}

	m_EnvironmentData.SkyBoxGradientColorVector[m_EnvironmentData.bySkyBoxGradientLevelUpper - 1].m_FirstColor = TColor (0.0f, 0.0f, 0.0f, 0.0f);
	m_EnvironmentData.SkyBoxGradientColorVector[m_EnvironmentData.bySkyBoxGradientLevelUpper - 1].m_SecondColor = TColor (0.0f, 0.0f, 0.0f, 0.0f);

	for (DWORD j = 0; j < m_EnvironmentData.bySkyBoxGradientLevelLower; ++j)
	{
		m_EnvironmentData.SkyBoxGradientColorVector[m_EnvironmentData.bySkyBoxGradientLevelUpper + j] = TempGradientVector[ (m_EnvironmentData.bySkyBoxGradientLevelUpper - 1) + j];
	}
}

void CMapManagerAccessor::InsertGradientLower()
{
	std::vector<TGradientColor> TempGradientVector;

	++m_EnvironmentData.bySkyBoxGradientLevelLower;
	TempGradientVector = m_EnvironmentData.SkyBoxGradientColorVector;

	m_EnvironmentData.SkyBoxGradientColorVector.clear();
	m_EnvironmentData.SkyBoxGradientColorVector.resize (m_EnvironmentData.bySkyBoxGradientLevelUpper + m_EnvironmentData.bySkyBoxGradientLevelLower);

	for (DWORD i = 0; i < TempGradientVector.size(); ++i)
	{
		m_EnvironmentData.SkyBoxGradientColorVector[i] = TempGradientVector[i];
	}
	for (DWORD j = 0; j < m_EnvironmentData.bySkyBoxGradientLevelLower; ++j)
	{
		m_EnvironmentData.SkyBoxGradientColorVector[m_EnvironmentData.bySkyBoxGradientLevelUpper + j] = TempGradientVector[m_EnvironmentData.bySkyBoxGradientLevelUpper + j];
	}

	m_EnvironmentData.SkyBoxGradientColorVector[m_EnvironmentData.bySkyBoxGradientLevelUpper + m_EnvironmentData.bySkyBoxGradientLevelLower - 1].m_FirstColor = TColor (0.0f, 0.0f,
		0.0f, 0.0f);
	m_EnvironmentData.SkyBoxGradientColorVector[m_EnvironmentData.bySkyBoxGradientLevelUpper + m_EnvironmentData.bySkyBoxGradientLevelLower - 1].m_SecondColor = TColor (0.0f,
		0.0f, 0.0f, 0.0f);
}

void CMapManagerAccessor::DeleteGradient (DWORD dwIndex)
{
	if (dwIndex >= m_EnvironmentData.SkyBoxGradientColorVector.size())
	{
		return;
	}

	DeleteVectorItem<TGradientColor> (&m_EnvironmentData.SkyBoxGradientColorVector, dwIndex);

	if (dwIndex < m_EnvironmentData.bySkyBoxGradientLevelUpper)
	{
		if (m_EnvironmentData.bySkyBoxGradientLevelUpper > 0)
		{
			m_EnvironmentData.bySkyBoxGradientLevelUpper -= 1;
		}
	}
	else
	{
		if (m_EnvironmentData.bySkyBoxGradientLevelLower > 0)
		{
			m_EnvironmentData.bySkyBoxGradientLevelLower -= 1;
		}
	}
}

void CMapManagerAccessor::RefreshSkyBox()
{
	m_pMapAccessor->SetEnvironmentSkyBox();
}

void CMapManagerAccessor::RefreshLensFlare()
{
	m_pMapAccessor->SetEnvironmentLensFlare();
}

BOOL& CMapManagerAccessor::GetLensFlareEnableReference()
{
	return m_EnvironmentData.bLensFlareEnable;
}

D3DXCOLOR& CMapManagerAccessor::GetLensFlareBrightnessColorReference()
{
	return m_EnvironmentData.LensFlareBrightnessColor;
}

float& CMapManagerAccessor::GetLensFlareMaxBrightnessReference()
{
	return m_EnvironmentData.fLensFlareMaxBrightness;
}

BOOL& CMapManagerAccessor::GetMainFlareEnableReference()
{
	return m_EnvironmentData.bMainFlareEnable;
}

std::string& CMapManagerAccessor::GetMainFlareTextureFileNameReference()
{
	return m_EnvironmentData.strMainFlareTextureFileName;
}

float& CMapManagerAccessor::GetMainFlareSizeReference()
{
	return m_EnvironmentData.fMainFlareSize;
}

void CMapManagerAccessor::LoadEnvironmentScript (const char* c_szFileName)
{
	LoadEnvironmentData (c_szFileName, &m_EnvironmentData);
}

void CMapManagerAccessor::SaveEnvironmentScript (const char* c_szFileName)
{
	const float c_fEnvironment_Script_Version = 1.0f;

	SetFileAttributes (c_szFileName, FILE_ATTRIBUTE_NORMAL);
	FILE * File = fopen (c_szFileName, "w");
	if (!File)
	{
		return;
	}

	fprintf (File, "ScriptType         EnvrionmentData\n");
	fprintf (File, "ScriptVersion      %.4f\n", c_fEnvironment_Script_Version);
	fprintf (File, "\n");

	PrintfTabs (File, 0, "Group DirectionalLight\n");
	PrintfTabs (File, 0, "{\n");
	PrintfTabs (File, 1, "Direction     %f %f %f\n", m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.x,
				m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.y, m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.z);
	fprintf (File, "\n");

	PrintfTabs (File, 1, "Group Background\n");
	PrintfTabs (File, 1, "{\n");
	PrintfTabs (File, 2, "Enable        %d\n", m_EnvironmentData.bDirLightsEnable[ENV_DIRLIGHT_BACKGROUND]);
	PrintfTabs (File, 2, "Diffuse       %f %f %f %f\n", m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.r,
				m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.g, m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.b,
				m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.a);
	PrintfTabs (File, 2, "Ambient       %f %f %f %f\n", m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.r,
				m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.g, m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.b,
				m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.a);
	PrintfTabs (File, 1, "}\n");
	PrintfTabs (File, 1, "\n");

	PrintfTabs (File, 1, "Group Character\n");
	PrintfTabs (File, 1, "{\n");
	PrintfTabs (File, 2, "Enable        %d\n", m_EnvironmentData.bDirLightsEnable[ENV_DIRLIGHT_BACKGROUND]);
	PrintfTabs (File, 2, "Diffuse       %f %f %f %f\n", m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.r,
				m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.g, m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse.b,
				m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.a);
	PrintfTabs (File, 2, "Ambient       %f %f %f %f\n", m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.r + 0.15f,
				m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.g + 0.15f, m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.b + 0.15f,
				m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient.a);
	PrintfTabs (File, 1, "}\n");
	PrintfTabs (File, 0, "}\n");

	PrintfTabs (File, 0, "Group Material\n");
	PrintfTabs (File, 0, "{\n");
	PrintfTabs (File, 1, "Diffuse       %f %f %f %f\n", m_EnvironmentData.Material.Diffuse.r, m_EnvironmentData.Material.Diffuse.g, m_EnvironmentData.Material.Diffuse.b,
				m_EnvironmentData.Material.Ambient.a);
	PrintfTabs (File, 1, "Ambient       %f %f %f %f\n", m_EnvironmentData.Material.Ambient.r, m_EnvironmentData.Material.Ambient.g, m_EnvironmentData.Material.Ambient.b,
				m_EnvironmentData.Material.Ambient.a);
	PrintfTabs (File, 1, "Emissive      %f %f %f %f\n", m_EnvironmentData.Material.Emissive.r, m_EnvironmentData.Material.Emissive.g, m_EnvironmentData.Material.Emissive.b,
				m_EnvironmentData.Material.Emissive.a);
	PrintfTabs (File, 0, "}\n");
	PrintfTabs (File, 0, "\n");

	PrintfTabs (File, 0, "Group Fog\n");
	PrintfTabs (File, 0, "{\n");
	PrintfTabs (File, 1, "Enable        %d\n", m_EnvironmentData.bFogEnable);
	PrintfTabs (File, 1, "NearDistance  %f\n", m_EnvironmentData.m_fFogNearDistance);
	PrintfTabs (File, 1, "FarDistance   %f\n", m_EnvironmentData.m_fFogFarDistance);
	PrintfTabs (File, 1, "Color         %f %f %f %f\n", m_EnvironmentData.FogColor.r, m_EnvironmentData.FogColor.g, m_EnvironmentData.FogColor.b, m_EnvironmentData.FogColor.a);
	PrintfTabs (File, 0, "}\n");
	PrintfTabs (File, 0, "\n");

	PrintfTabs (File, 0, "Group Filter\n");
	PrintfTabs (File, 0, "{\n");
	PrintfTabs (File, 1, "Enable        %d\n", m_EnvironmentData.bFilteringEnable);
	PrintfTabs (File, 1, "Color         %f %f %f %f\n", m_EnvironmentData.FilteringColor.r,
				m_EnvironmentData.FilteringColor.g,
				m_EnvironmentData.FilteringColor.b,
				m_EnvironmentData.FilteringColor.a);
	PrintfTabs (File, 1, "AlphaSrc      %d\n", m_EnvironmentData.byFilteringAlphaSrc);
	PrintfTabs (File, 1, "AlphaDest     %d\n", m_EnvironmentData.byFilteringAlphaDest);
	PrintfTabs (File, 0, "}\n");
	PrintfTabs (File, 0, "\n");

	PrintfTabs (File, 0, "Group SkyBox\n");
	PrintfTabs (File, 0, "{\n");
	PrintfTabs (File, 1, "bTextureRenderMode    %d\n", m_EnvironmentData.bSkyBoxTextureRenderMode);
	PrintfTabs (File, 1, "Scale                 %f %f %f\n", m_EnvironmentData.v3SkyBoxScale.x, m_EnvironmentData.v3SkyBoxScale.y, m_EnvironmentData.v3SkyBoxScale.z);
	PrintfTabs (File, 1, "GradientLevelUpper    %d\n", m_EnvironmentData.bySkyBoxGradientLevelUpper);
	PrintfTabs (File, 1, "GradientLevelLower    %d\n", m_EnvironmentData.bySkyBoxGradientLevelLower);
	PrintfTabs (File, 1, "FrontFaceFileName	   \"%s\"\n", m_EnvironmentData.strSkyBoxFaceFileName[0].c_str());
	PrintfTabs (File, 1, "BackFaceFileName	   \"%s\"\n", m_EnvironmentData.strSkyBoxFaceFileName[1].c_str());
	PrintfTabs (File, 1, "LeftFaceFileName	   \"%s\"\n", m_EnvironmentData.strSkyBoxFaceFileName[2].c_str());
	PrintfTabs (File, 1, "RightFaceFileName	   \"%s\"\n", m_EnvironmentData.strSkyBoxFaceFileName[3].c_str());
	PrintfTabs (File, 1, "TopFaceFileName	   \"%s\"\n", m_EnvironmentData.strSkyBoxFaceFileName[4].c_str());
	PrintfTabs (File, 1, "BottomFaceFileName	   \"%s\"\n", m_EnvironmentData.strSkyBoxFaceFileName[5].c_str());
	PrintfTabs (File, 1, "\n");

	PrintfTabs (File, 1, "CloudScale            %f %f\n", m_EnvironmentData.v2CloudScale.x, m_EnvironmentData.v2CloudScale.y);
	PrintfTabs (File, 1, "CloudHeight           %f\n", m_EnvironmentData.fCloudHeight);
	PrintfTabs (File, 1, "CloudTextureScale     %f %f\n", m_EnvironmentData.v2CloudTextureScale.x, m_EnvironmentData.v2CloudTextureScale.y);
	PrintfTabs (File, 1, "CloudSpeed            %f %f\n", m_EnvironmentData.v2CloudSpeed.x, m_EnvironmentData.v2CloudSpeed.y);
	PrintfTabs (File, 1, "CloudTextureFileName  \"%s\"\n", m_EnvironmentData.strCloudTextureFileName.c_str());
	PrintfTabs (File, 1, "List CloudColor\n");
	PrintfTabs (File, 1, "{\n");
	TGradientColor & GradientColorCloud = m_EnvironmentData.CloudGradientColor;
	PrintfTabs (File, 2, "%f %f %f %f\n", GradientColorCloud.m_FirstColor.r, GradientColorCloud.m_FirstColor.g, GradientColorCloud.m_FirstColor.b,
				GradientColorCloud.m_FirstColor.a);
	PrintfTabs (File, 2, "%f %f %f %f\n", GradientColorCloud.m_SecondColor.r, GradientColorCloud.m_SecondColor.g, GradientColorCloud.m_SecondColor.b,
				GradientColorCloud.m_SecondColor.a);
	PrintfTabs (File, 1, "}\n");

	if (!m_EnvironmentData.SkyBoxGradientColorVector.empty())
	{
		PrintfTabs (File, 1, "List Gradient\n");
		PrintfTabs (File, 1, "{\n");
		for (DWORD k = 0; k < m_EnvironmentData.SkyBoxGradientColorVector.size(); ++k)
		{
			TGradientColor & GradientColor = m_EnvironmentData.SkyBoxGradientColorVector[k];
			PrintfTabs (File, 2, "%f %f %f %f\n", GradientColor.m_FirstColor.r, GradientColor.m_FirstColor.g, GradientColor.m_FirstColor.b, GradientColor.m_FirstColor.a);
			PrintfTabs (File, 2, "%f %f %f %f\n", GradientColor.m_SecondColor.r, GradientColor.m_SecondColor.g, GradientColor.m_SecondColor.b, GradientColor.m_SecondColor.a);

			if (k < m_EnvironmentData.SkyBoxGradientColorVector.size() - 1)
			{
				PrintfTabs (File, 2, "\n");
			}
		}
		PrintfTabs (File, 1, "}\n");
	}
	PrintfTabs (File, 0, "}\n");
	PrintfTabs (File, 0, "\n");

	PrintfTabs (File, 0, "Group LensFlare\n");
	PrintfTabs (File, 0, "{\n");
	PrintfTabs (File, 1, "Enable                     %d\n", m_EnvironmentData.bLensFlareEnable);
	PrintfTabs (File, 1, "BrightnessColor            %f %f %f %f\n", m_EnvironmentData.LensFlareBrightnessColor.r, m_EnvironmentData.LensFlareBrightnessColor.g,
				m_EnvironmentData.LensFlareBrightnessColor.b, m_EnvironmentData.LensFlareBrightnessColor.a);
	PrintfTabs (File, 1, "MaxBrightness              %f\n", m_EnvironmentData.fLensFlareMaxBrightness);
	PrintfTabs (File, 1, "MainFlareEnable            %d\n", m_EnvironmentData.bMainFlareEnable);
	PrintfTabs (File, 1, "MainFlareTextureFileName   \"%s\"\n", m_EnvironmentData.strMainFlareTextureFileName.c_str());
	PrintfTabs (File, 1, "MainFlareSize              %f\n", m_EnvironmentData.fMainFlareSize);
	PrintfTabs (File, 0, "}\n");
	PrintfTabs (File, 0, "\n");

	fclose (File);
}

void CMapManagerAccessor::InitializeEnvironmentData()
{
	m_EnvironmentData.bDirLightsEnable[ENV_DIRLIGHT_BACKGROUND] = TRUE;
	m_EnvironmentData.bDensityFog = FALSE;
	memset (&m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND], 0, sizeof (D3DLIGHT9));
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Type = D3DLIGHT_DIRECTIONAL;
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Direction = D3DXVECTOR3 (0.5f, 0.5f, -0.5f);
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Position = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Specular = D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f);
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Diffuse = D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f);
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Ambient = D3DXCOLOR (0.25f, 0.32f, 0.31f, 1.0f);
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Range = 0.0f; // Used by Point Light & Spot Light
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Falloff = 1.0f;  // Used by Spot Light
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Theta = 0.0f;    // Used by Spot Light
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Phi = 0.0f;      // Used by Spot Light
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Attenuation0 = 0.0f;
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Attenuation1 = 1.0f;
	m_EnvironmentData.DirLights[ENV_DIRLIGHT_BACKGROUND].Attenuation2 = 0.0f;

	m_EnvironmentData.Material.Diffuse = D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f);
	m_EnvironmentData.Material.Ambient = D3DXCOLOR (0.5f, 0.45f, 0.32f, 1.0f);
	m_EnvironmentData.Material.Emissive = D3DXCOLOR (0.2f, 0.2f, 0.2f, 1.0f);
	m_EnvironmentData.Material.Specular = D3DXCOLOR (0.0f, 0.0f, 0.0f, 1.0f);
	m_EnvironmentData.Material.Power = 0.0f;

	m_EnvironmentData.bFogEnable = FALSE;
	m_EnvironmentData.FogColor = D3DXCOLOR (0.5f, 0.5f, 0.5f, 1.0f);
	m_EnvironmentData.m_fFogNearDistance = 20000.0f;
	m_EnvironmentData.m_fFogFarDistance = 50000.0f;

	m_EnvironmentData.fWindStrength = 0.2f;
	m_EnvironmentData.fWindRandom = 0.0f;

	m_EnvironmentData.bFilteringEnable = FALSE;
	m_EnvironmentData.FilteringColor = D3DXCOLOR (0.3f, 0.1f, 0.1f, 0.0f);
	m_EnvironmentData.byFilteringAlphaSrc = D3DBLEND_ONE;
	m_EnvironmentData.byFilteringAlphaDest = D3DBLEND_ONE;

	m_EnvironmentData.v3SkyBoxScale = D3DXVECTOR3 (3500.0f, 3500.0f, 3500.0f);
	m_EnvironmentData.bSkyBoxTextureRenderMode = FALSE;
	m_EnvironmentData.bySkyBoxGradientLevelUpper = 0;
	m_EnvironmentData.bySkyBoxGradientLevelLower = 0;
	m_EnvironmentData.SkyBoxGradientColorVector.clear();

	m_EnvironmentData.v2CloudScale = D3DXVECTOR2 (200000.0f, 200000.0f);
	m_EnvironmentData.fCloudHeight = 30000.0f;
	m_EnvironmentData.v2CloudTextureScale = D3DXVECTOR2 (4.0f, 4.0f);
	m_EnvironmentData.v2CloudSpeed = D3DXVECTOR2 (0.001f, 0.001f);
	m_EnvironmentData.strCloudTextureFileName = "";
	memset (&m_EnvironmentData.CloudGradientColor, 0, sizeof (TGradientColor));

	m_EnvironmentData.bLensFlareEnable = FALSE;
	m_EnvironmentData.LensFlareBrightnessColor = D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f);
	m_EnvironmentData.fLensFlareMaxBrightness = 1.0f;
	m_EnvironmentData.bMainFlareEnable = FALSE;
	m_EnvironmentData.strMainFlareTextureFileName = "";
	m_EnvironmentData.fMainFlareSize = 0.2f;
}
