#include "StdAfx.h"

bool CObjectData::CheckLightIndex (DWORD dwIndex)
{
	if (dwIndex >= LIGHT_MAX_NUM)
	{
		return false;
	}

	return true;
}

void CObjectData::SetLightEnable (DWORD dwIndex, bool bFlag)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return;
	}

	pData->bLightEnable = bFlag;
}

void CObjectData::SetSpecularEnable (bool bFlag)
{
	m_LightData.bSpecularEnable = bFlag;
}

int CObjectData::GetLightType (DWORD dwIndex)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return LIGHT_TYPE_DIRECTIONAL;
	}

	return pData->Light.Type;
}

void CObjectData::SetLightType (DWORD dwIndex, int iType)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return;
	}

	pData->Light.Type = (D3DLIGHTTYPE)iType;
}

bool CObjectData::isLightEnable (DWORD dwIndex)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return false;
	}

	return pData->bLightEnable;
}

bool CObjectData::isSpecularEnable()
{
	return m_LightData.bSpecularEnable;
}

bool CObjectData::GetLightDataPointer (DWORD dwIndex, TLightElementData** ppData)
{
	if (!CheckLightIndex (dwIndex))
	{
		return false;
	}

	*ppData = &m_LightData.LightElementData[dwIndex];

	return true;
}

// Light
DWORD CObjectData::GetSelectedLightIndex()
{
	return m_dwSelectedLightIndex;
}

void CObjectData::SelectLight (DWORD dwIndex)
{
	m_dwSelectedLightIndex = dwIndex;
}

bool CObjectData::GetLightName (DWORD dwIndex, std::string** ppString)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return false;
	}

	*ppString = &pData->strName;

	return true;
}

void CObjectData::SetLightName (DWORD dwIndex, const char* pszLightName)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return;
	}

	pData->strName = pszLightName;

	return;
}

bool CObjectData::GetLight (DWORD dwIndex, const D3DLIGHT9** ppd3dLight)
{
	TLightElementData * pLightData;
	if (!GetLightDataPointer (dwIndex, &pLightData))
	{
		return false;
	}

	*ppd3dLight = &pLightData->Light;

	return true;
}

void CObjectData::SetLightDirection (DWORD dwIndex, float fx, float fy, float fz)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return;
	}

	pData->Light.Direction.x = fx;
	pData->Light.Direction.y = fy;
	pData->Light.Direction.z = fz;
}

void CObjectData::SetLightDiffuse (DWORD dwIndex, int iRed, int iGreen, int iBlue)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return;
	}

	pData->Light.Diffuse.r = float (iRed) / 255.0f;
	pData->Light.Diffuse.g = float (iGreen) / 255.0f;
	pData->Light.Diffuse.b = float (iBlue) / 255.0f;
}
void CObjectData::SetLightAmbient (DWORD dwIndex, int iRed, int iGreen, int iBlue)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return;
	}

	pData->Light.Ambient.r = float (iRed) / 255.0f;
	pData->Light.Ambient.g = float (iGreen) / 255.0f;
	pData->Light.Ambient.b = float (iBlue) / 255.0f;
}
void CObjectData::SetLightSpecular (DWORD dwIndex, int iRed, int iGreen, int iBlue)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return;
	}

	pData->Light.Specular.r = float (iRed) / 255.0f;
	pData->Light.Specular.g = float (iGreen) / 255.0f;
	pData->Light.Specular.b = float (iBlue) / 255.0f;
}


// Material
void CObjectData::GetMaterial (const D3DMATERIAL9** ppd3dMaterial)
{
	*ppd3dMaterial = &m_LightData.Material;
}

void CObjectData::SetMaterialDiffuse (int iRed, int iGreen, int iBlue)
{
	m_LightData.Material.Diffuse.r = float (iRed) / 255.0f;
	m_LightData.Material.Diffuse.g = float (iGreen) / 255.0f;
	m_LightData.Material.Diffuse.b = float (iBlue) / 255.0f;
}
void CObjectData::SetMaterialAmbient (int iRed, int iGreen, int iBlue)
{
	m_LightData.Material.Ambient.r = float (iRed) / 255.0f;
	m_LightData.Material.Ambient.g = float (iGreen) / 255.0f;
	m_LightData.Material.Ambient.b = float (iBlue) / 255.0f;
}
void CObjectData::SetMaterialSpecular (int iRed, int iGreen, int iBlue)
{
	m_LightData.Material.Specular.r = float (iRed) / 255.0f;
	m_LightData.Material.Specular.g = float (iGreen) / 255.0f;
	m_LightData.Material.Specular.b = float (iBlue) / 255.0f;
}
void CObjectData::SetMaterialEmissive (int iRed, int iGreen, int iBlue)
{
	m_LightData.Material.Emissive.r = float (iRed) / 255.0f;
	m_LightData.Material.Emissive.g = float (iGreen) / 255.0f;
	m_LightData.Material.Emissive.b = float (iBlue) / 255.0f;
}
void CObjectData::SetMaterialPower (float fPower)
{
	m_LightData.Material.Power = fPower;
}
void CObjectData::SetMaterialDiffuseAlpha (float fAlpha)
{
	m_LightData.Material.Diffuse.a = fAlpha;
}
void CObjectData::SetMaterialAmbientAlpha (float fAlpha)
{
	m_LightData.Material.Ambient.a = fAlpha;
}
void CObjectData::SetMaterialSpecularAlpha (float fAlpha)
{
	m_LightData.Material.Specular.a = fAlpha;
}

void CObjectData::SetMaterialEmissiveAlpha (float fAlpha)
{
	m_LightData.Material.Emissive.a = fAlpha;
}


int CObjectData::GetTexturingOperation (int iStage, int iType)
{
	return m_LightData.TextureState[iStage][iType].iOperation;
}

int CObjectData::GetTexturingArgument (int iStage, int iType, int iIndex)
{
	return m_LightData.TextureState[iStage][iType].iArgument[iIndex];
}

void CObjectData::SetTexturingOperation (int iStage, int iType, int iOperation)
{
	m_LightData.TextureState[iStage][iType].iOperation = iOperation;
}

void CObjectData::SetTexturingArgument (int iStage, int iType, int iIndex, int iArgument)
{
	m_LightData.TextureState[iStage][iType].iArgument[iIndex] = iArgument;
}

bool CObjectData::isEnableTexture (int iIndex)
{
	return m_LightData.TextureData[iIndex].bEnable;
}

void CObjectData::SetEnableTexture (int iIndex, BOOL bFlag)
{
	m_LightData.TextureData[iIndex].bEnable = bFlag == 1 ? TRUE : FALSE;
}

void CObjectData::LoadTexture (int iIndex, const char* c_szFileName)
{
	CGraphicImage * pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer (c_szFileName);

	m_LightData.TextureData[iIndex].Texture.SetImagePointer (pImage);
	m_LightData.TextureData[iIndex].strFileName = c_szFileName;
	GetExceptionPathName (c_szFileName, m_LightData.TextureData[iIndex].strName);
}

bool CObjectData::GetTexture (int iIndex, LPDIRECT3DTEXTURE9 * plpTexture)
{
	TTextureData & rTextureData = m_LightData.TextureData[iIndex];

	if (!rTextureData.bEnable)
	{
		return false;
	}

	if (rTextureData.Texture.IsEmpty())
	{
		return false;
	}

	CGraphicTexture * pTexture = rTextureData.Texture.GetTexturePointer();

	if (!pTexture)
	{
		return false;
	}

	if (pTexture->IsEmpty())
	{
		return false;
	}

	*plpTexture = pTexture->GetD3DTexture();

	return true;
}

const char* CObjectData::GetTextureName (int iIndex)
{
	return m_LightData.TextureData[iIndex].strName.c_str();
}

void CObjectData::SaveLightScript (const char* c_szFileName)
{
	FILE * File;

	File = fopen (c_szFileName, "w");

	if (!File)
	{
		return;
	}

	fprintf (File, "Title                      LightScript\n");
	fprintf (File, "Version                    1.0\n");
	fprintf (File, "\n");

	fprintf (File, "MaterialDiffuse            %f %f %f %f\n", m_LightData.Material.Diffuse.r, m_LightData.Material.Diffuse.g, m_LightData.Material.Diffuse.b,
			 m_LightData.Material.Diffuse.a);
	fprintf (File, "MaterialAmbient            %f %f %f %f\n", m_LightData.Material.Ambient.r, m_LightData.Material.Ambient.g, m_LightData.Material.Ambient.b,
			 m_LightData.Material.Ambient.a);
	fprintf (File, "MaterialSpecular           %f %f %f %f\n", m_LightData.Material.Specular.r, m_LightData.Material.Specular.g, m_LightData.Material.Specular.b,
			 m_LightData.Material.Specular.a);
	fprintf (File, "MaterialEmissive           %f %f %f %f\n", m_LightData.Material.Emissive.r, m_LightData.Material.Emissive.g, m_LightData.Material.Emissive.b,
			 m_LightData.Material.Emissive.a);
	fprintf (File, "MaterialPower              %f\n", m_LightData.Material.Power);
	fprintf (File, "SpecularEnable             %d\n", m_LightData.bSpecularEnable);
	fprintf (File, "\n");

	fprintf (File, "FirstTextureEnable         %d\n", m_LightData.TextureData[0].bEnable);
	fprintf (File, "FirstTextureFileName       \"%s\"\n", m_LightData.TextureData[0].strFileName.c_str());
	fprintf (File, "SecondTextureEnable        %d\n", m_LightData.TextureData[1].bEnable);
	fprintf (File, "SecondTextureFileName      \"%s\"\n", m_LightData.TextureData[1].strFileName.c_str());
	for (DWORD dwStage = 0; dwStage < 2; ++dwStage)
		fprintf (File, "TextureStateStage%d         %d %d %d  %d %d %d\n", dwStage,
				 m_LightData.TextureState[dwStage][0].iArgument[0],
				 m_LightData.TextureState[dwStage][0].iArgument[1],
				 m_LightData.TextureState[dwStage][0].iOperation,
				 m_LightData.TextureState[dwStage][1].iArgument[0],
				 m_LightData.TextureState[dwStage][1].iArgument[1],
				 m_LightData.TextureState[dwStage][1].iOperation);
	fprintf (File, "\n");

	for (DWORD i = 0; i < LIGHT_MAX_NUM; ++i)
	{
		TLightElementData * pData;
		if (!GetLightDataPointer (i, &pData))
		{
			continue;
		}

		fprintf (File, "    %dLightName                  \"%s\"\n", i, pData->strName.c_str());
		fprintf (File, "    %dLightEnable                %d\n", i, pData->bLightEnable);
		fprintf (File, "    %dLightDirection             %f %f %f\n", i, pData->Light.Direction.x, pData->Light.Direction.y, pData->Light.Direction.z);
		fprintf (File, "    %dLightPosition              %f %f %f\n", i, pData->Light.Position.x, pData->Light.Position.y, pData->Light.Position.z);
		fprintf (File, "    %dLightDiffuse               %f %f %f %f\n", i, pData->Light.Diffuse.r, pData->Light.Diffuse.g, pData->Light.Diffuse.b, pData->Light.Diffuse.a);
		fprintf (File, "    %dLightAmbient               %f %f %f %f\n", i, pData->Light.Ambient.r, pData->Light.Ambient.g, pData->Light.Ambient.b, pData->Light.Diffuse.a);
		fprintf (File, "    %dLightSpecular              %f %f %f %f\n", i, pData->Light.Specular.r, pData->Light.Specular.g, pData->Light.Specular.b, pData->Light.Diffuse.a);
		fprintf (File, "    %dLightRange                 %f\n", i, pData->Light.Range);
		fprintf (File, "    %dLightFalloff               %f\n", i, pData->Light.Falloff);
		fprintf (File, "    %dLightTheta                 %f\n", i, pData->Light.Theta);
		fprintf (File, "    %dLightPhi                   %f\n", i, pData->Light.Phi);
		fprintf (File, "    %dLightAttenuation0          %f\n", i, pData->Light.Attenuation0);
		fprintf (File, "    %dLightAttenuation1          %f\n", i, pData->Light.Attenuation1);
		fprintf (File, "    %dLightAttenuation2          %f\n", i, pData->Light.Attenuation2);
		fprintf (File, "\n");
		fprintf (File, "\n");
	}

	fclose (File);
}

const char* IndexingKey (int iIndex, const char* c_szFileName)
{
	static char szKey[64 + 1];

	_snprintf (szKey, 64, "%d%s", iIndex, c_szFileName);

	return szKey;
}

bool CObjectData::LoadLightScript (const char* c_szFileName)
{
	CTokenVectorMap stTokenVectorMap;
	if (!LoadMultipleTextData (c_szFileName, stTokenVectorMap))
	{
		return false;
	}

	if (!stTokenVectorMap.contains("title") || !stTokenVectorMap.contains("version"))
	{
		return false;
	}

	if (0 != stTokenVectorMap["title"][0].compare ("LightScript"))
	{
		return false;
	}

	if (c_fLight_Script_Version != atof (stTokenVectorMap["version"][0].c_str()))
	{
		return false;
	}

	// Material
	CTokenVector & rMaterialDiffuse		= stTokenVectorMap["materialdiffuse"];
	CTokenVector & rMaterialAmbient		= stTokenVectorMap["materialambient"];
	CTokenVector & rMaterialSpecular	= stTokenVectorMap["materialspecular"];
	CTokenVector & rMaterialEmissive	= stTokenVectorMap["materialemissive"];
	CTokenVector & rMaterialPower		= stTokenVectorMap["materialpower"];
	CTokenVector & rSpecularEnable		= stTokenVectorMap["specularenable"];

	m_LightData.Material.Diffuse = TokenToColor (rMaterialDiffuse);
	m_LightData.Material.Ambient = TokenToColor (rMaterialAmbient);
	m_LightData.Material.Specular = TokenToColor (rMaterialSpecular);
	m_LightData.Material.Emissive = TokenToColor (rMaterialEmissive);
	m_LightData.Material.Power = atof (rMaterialPower[0].c_str());
	m_LightData.bSpecularEnable = atoi (rSpecularEnable[0].c_str()) == 1 ? true : false;

	// Texturing
	CTokenVector & rFirstTextureEnable		= stTokenVectorMap["firsttextureenable"];
	CTokenVector & rFirstTextureFileName	= stTokenVectorMap["firsttexturefilename"];
	CTokenVector & rSecondTextureEnable		= stTokenVectorMap["secondtextureenable"];
	CTokenVector & rSecondTextureFileName	= stTokenVectorMap["secondtexturefilename"];

	m_LightData.TextureData[0].bEnable = atoi (rFirstTextureEnable[0].c_str()) == 1 ? true : false;
	m_LightData.TextureData[0].strFileName = rFirstTextureFileName[0].c_str();
	LoadTexture (0, rFirstTextureFileName[0].c_str());

	m_LightData.TextureData[1].bEnable = atoi (rSecondTextureEnable[0].c_str()) == 1 ? true : false;
	m_LightData.TextureData[1].strFileName = rSecondTextureFileName[0].c_str();
	GetOnlyFileName (rSecondTextureFileName[0].c_str(), m_LightData.TextureData[1].strName);
	LoadTexture (1, rSecondTextureFileName[0].c_str());

	for (DWORD dwStage = 0; dwStage < 2; ++dwStage)
	{
		char szKeyName[64 + 1];
		_snprintf (szKeyName, 64, "texturestatestage%d", dwStage);

		CTokenVector & rTextureStateStage = stTokenVectorMap[szKeyName];
		m_LightData.TextureState[dwStage][0].iArgument[0] = atoi (rTextureStateStage[0].c_str());
		m_LightData.TextureState[dwStage][0].iArgument[1] = atoi (rTextureStateStage[1].c_str());
		m_LightData.TextureState[dwStage][0].iOperation = atoi (rTextureStateStage[2].c_str());
		m_LightData.TextureState[dwStage][1].iArgument[0] = atoi (rTextureStateStage[3].c_str());
		m_LightData.TextureState[dwStage][1].iArgument[1] = atoi (rTextureStateStage[4].c_str());
		m_LightData.TextureState[dwStage][1].iOperation = atoi (rTextureStateStage[5].c_str());
	}

	// Light
	for (DWORD i = 0; i < LIGHT_MAX_NUM; ++i)
	{
		TLightElementData * pData;
		if (!GetLightDataPointer (i, &pData))
		{
			continue;
		}

		CTokenVector & rLightName			= stTokenVectorMap[IndexingKey (i, "lightname")];
		CTokenVector & rLightEnable			= stTokenVectorMap[IndexingKey (i, "lightenable")];
		CTokenVector & rSpecularEnable		= stTokenVectorMap[IndexingKey (i, "lightspecularenable")];
		CTokenVector & rLightDirection		= stTokenVectorMap[IndexingKey (i, "lightdirection")];
		CTokenVector & rLightPosition		= stTokenVectorMap[IndexingKey (i, "lightposition")];
		CTokenVector & rLightDiffuse		= stTokenVectorMap[IndexingKey (i, "lightdiffuse")];
		CTokenVector & rLightAmbient		= stTokenVectorMap[IndexingKey (i, "lightambient")];
		CTokenVector & rLightRange			= stTokenVectorMap[IndexingKey (i, "lightrange")];
		CTokenVector & rLightFalloff		= stTokenVectorMap[IndexingKey (i, "lightfalloff")];
		CTokenVector & rLightTheta			= stTokenVectorMap[IndexingKey (i, "lighttheta")];
		CTokenVector & rLightPhi			= stTokenVectorMap[IndexingKey (i, "lightphi")];
		CTokenVector & rLightAttenuation0	= stTokenVectorMap[IndexingKey (i, "lightattenuation0")];
		CTokenVector & rLightAttenuation1	= stTokenVectorMap[IndexingKey (i, "lightattenuation1")];
		CTokenVector & rLightAttenuation2	= stTokenVectorMap[IndexingKey (i, "lightattenuation2")];

		pData->strName = rLightName[0];
		pData->bLightEnable = atoi (rLightEnable[0].c_str()) == 1 ? true : false;
		pData->Light.Direction = TokenToVector (rLightDirection);
		pData->Light.Position = TokenToVector (rLightPosition);
		pData->Light.Diffuse = TokenToColor (rLightDiffuse);
		pData->Light.Ambient = TokenToColor (rLightAmbient);
		pData->Light.Range = atof (rLightRange[0].c_str());
		pData->Light.Falloff = atof (rLightFalloff[0].c_str());
		pData->Light.Theta = atof (rLightTheta[0].c_str());
		pData->Light.Phi = atof (rLightPhi[0].c_str());
		pData->Light.Attenuation0 = atof (rLightAttenuation0[0].c_str());
		pData->Light.Attenuation1 = atof (rLightAttenuation1[0].c_str());
		pData->Light.Attenuation2 = atof (rLightAttenuation2[0].c_str());
	}

	return true;
}

void CObjectData::SaveLightElementScript (DWORD dwIndex, const char* c_szFileName)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return;
	}

	FILE * File;

	File = fopen (c_szFileName, "w");

	if (!File)
	{
		return;
	}

	fprintf (File, "Title                      LightElementScript\n");
	fprintf (File, "Version                    1.0\n");
	fprintf (File, "\n");

	fprintf (File, "LightName                  \"%s\"\n", pData->strName.c_str());
	fprintf (File, "LightEnable                %d\n", pData->bLightEnable);
	fprintf (File, "\n");

	fprintf (File, "LightDirection             %f %f %f\n", pData->Light.Direction.x, pData->Light.Direction.y, pData->Light.Direction.z);
	fprintf (File, "LightPosition              %f %f %f\n", pData->Light.Position.x, pData->Light.Position.y, pData->Light.Position.z);
	fprintf (File, "LightDiffuse               %f %f %f %f\n", pData->Light.Diffuse.r, pData->Light.Diffuse.g, pData->Light.Diffuse.b, pData->Light.Diffuse.a);
	fprintf (File, "LightAmbient               %f %f %f %f\n", pData->Light.Ambient.r, pData->Light.Ambient.g, pData->Light.Ambient.b, pData->Light.Diffuse.a);
	fprintf (File, "LightSpecular              %f %f %f %f\n", pData->Light.Specular.r, pData->Light.Specular.g, pData->Light.Specular.b, pData->Light.Diffuse.a);
	fprintf (File, "LightRange                 %f\n", pData->Light.Range);
	fprintf (File, "LightFalloff               %f\n", pData->Light.Falloff);
	fprintf (File, "LightTheta                 %f\n", pData->Light.Theta);
	fprintf (File, "LightPhi                   %f\n", pData->Light.Phi);
	fprintf (File, "LightAttenuation0          %f\n", pData->Light.Attenuation0);
	fprintf (File, "LightAttenuation1          %f\n", pData->Light.Attenuation1);
	fprintf (File, "LightAttenuation2          %f\n", pData->Light.Attenuation2);
	fprintf (File, "\n");

	fclose (File);
}

bool CObjectData::LoadLightElementScript (DWORD dwIndex, const char* c_szFileName)
{
	CTokenVectorMap stTokenVectorMap;
	if (!LoadMultipleTextData (c_szFileName, stTokenVectorMap))
	{
		return false;
	}

	if (!stTokenVectorMap.contains("title") || !stTokenVectorMap.contains("version"))
	{
		return false;
	}

	if (0 != stTokenVectorMap["title"][0].compare ("LightElementScript"))
	{
		return false;
	}

	if (c_fLight_Element_Script_Version != atof (stTokenVectorMap["version"][0].c_str()))
	{
		return false;
	}

	CTokenVector & rLightName			= stTokenVectorMap["lightname"];
	CTokenVector & rLightEnable			= stTokenVectorMap["lightenable"];
	CTokenVector & rSpecularEnable		= stTokenVectorMap["lightspecularenable"];
	CTokenVector & rLightDirection		= stTokenVectorMap["lightdirection"];
	CTokenVector & rLightPosition		= stTokenVectorMap["lightposition"];
	CTokenVector & rLightDiffuse		= stTokenVectorMap["lightdiffuse"];
	CTokenVector & rLightAmbient		= stTokenVectorMap["lightambient"];
	CTokenVector & rLightSpecular		= stTokenVectorMap["lightspecular"];
	CTokenVector & rLightRange			= stTokenVectorMap["lightrange"];
	CTokenVector & rLightFalloff		= stTokenVectorMap["lightfalloff"];
	CTokenVector & rLightTheta			= stTokenVectorMap["lighttheta"];
	CTokenVector & rLightPhi			= stTokenVectorMap["lightphi"];
	CTokenVector & rLightAttenuation0	= stTokenVectorMap["lightattenuation0"];
	CTokenVector & rLightAttenuation1	= stTokenVectorMap["lightattenuation1"];
	CTokenVector & rLightAttenuation2	= stTokenVectorMap["lightattenuation2"];

	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return false;
	}

	pData->strName = rLightName[0];
	pData->bLightEnable = atoi (rLightEnable[0].c_str()) == 1 ? true : false;
	pData->Light.Direction = TokenToVector (rLightDirection);
	pData->Light.Position = TokenToVector (rLightPosition);
	pData->Light.Diffuse = TokenToColor (rLightDiffuse);
	pData->Light.Ambient = TokenToColor (rLightAmbient);
	pData->Light.Specular = TokenToColor (rLightSpecular);
	pData->Light.Range = atof (rLightRange[0].c_str());
	pData->Light.Falloff = atof (rLightFalloff[0].c_str());
	pData->Light.Theta = atof (rLightTheta[0].c_str());
	pData->Light.Phi = atof (rLightPhi[0].c_str());
	pData->Light.Attenuation0 = atof (rLightAttenuation0[0].c_str());
	pData->Light.Attenuation1 = atof (rLightAttenuation1[0].c_str());
	pData->Light.Attenuation2 = atof (rLightAttenuation2[0].c_str());

	return true;
}

void CObjectData::ClearLightElementData (DWORD dwIndex)
{
	TLightElementData * pData;
	if (!GetLightDataPointer (dwIndex, &pData))
	{
		return;
	}

	char szName[32 + 1];
	_snprintf (szName, 32, "Noname");
	pData->strName = szName;
	pData->strFileName = "";
	pData->bLightEnable = false;

	memset (&pData->Light, 0, sizeof (D3DLIGHT9));

	pData->Light.Type = D3DLIGHT_DIRECTIONAL;
	pData->Light.Direction = D3DXVECTOR3 (-1.0f, 0.0f, 0.0f);
	pData->Light.Position = D3DXVECTOR3 (0.0f, 0.0f, 0.0f);
	pData->Light.Diffuse = D3DXCOLOR (0.4f, 1.0f, 1.0f, 1.0f);
	pData->Light.Specular = D3DXCOLOR (0.4f, 0.0f, 0.0f, 1.0f);
	pData->Light.Ambient = D3DXCOLOR (0.0f, 0.0f, 0.0f, 1.0f);

	pData->Light.Range = 1000.0f; // Used by Point Light & Spot Light
	pData->Light.Falloff = 1.0f;  // Used by Spot Light
	pData->Light.Theta = 0.0f;    // Used by Spot Light
	pData->Light.Phi = 0.0f;      // Used by Spot Light

	pData->Light.Attenuation0 = 0.0f;
	pData->Light.Attenuation1 = 1.0f;
	pData->Light.Attenuation2 = 0.0f;
}

void CObjectData::ClearAllLightData()
{
	for (DWORD i = 0; i < LIGHT_MAX_NUM; ++i)
	{
		ClearLightElementData (i);
	}

	m_LightData.LightElementData[0].bLightEnable = TRUE;

	m_LightData.Material.Diffuse = D3DXCOLOR (1.0f, 1.0f, 1.0f, 1.0f);
	m_LightData.Material.Specular = D3DXCOLOR (0.0f, 0.0f, 0.0f, 0.0f);
	m_LightData.Material.Ambient = D3DXCOLOR (0.8f, 0.8f, 0.8f, 1.0f);
	m_LightData.Material.Emissive = D3DXCOLOR (0.8f, 0.8f, 0.8f, 1.0f);
	m_LightData.Material.Power = 0.0f;

	for (DWORD j = 0; j < 2 ; ++j)
	{
		m_LightData.TextureData[j].bEnable = false;
		m_LightData.TextureData[j].strName = "";
		m_LightData.TextureData[j].strFileName = "";
		m_LightData.TextureData[j].Texture.Destroy();
	}

	m_LightData.TextureState[0][TEXTURING_TYPE_COLOR].iArgument[0] = TEXTURING_ARG_DIFFUSE;
	m_LightData.TextureState[0][TEXTURING_TYPE_COLOR].iArgument[1] = TEXTURING_ARG_TEXTURE;
	m_LightData.TextureState[0][TEXTURING_TYPE_COLOR].iOperation = TEXTURING_OPERATION_MODULATE;
	m_LightData.TextureState[0][TEXTURING_TYPE_ALPHA].iArgument[0] = TEXTURING_ARG_DIFFUSE;
	m_LightData.TextureState[0][TEXTURING_TYPE_ALPHA].iArgument[1] = TEXTURING_ARG_TEXTURE;
	m_LightData.TextureState[0][TEXTURING_TYPE_ALPHA].iOperation = TEXTURING_OPERATION_MODULATE;

	m_LightData.TextureState[1][TEXTURING_TYPE_COLOR].iArgument[0] = TEXTURING_ARG_CURRENT;
	m_LightData.TextureState[1][TEXTURING_TYPE_COLOR].iArgument[1] = TEXTURING_ARG_TEXTURE;
	m_LightData.TextureState[1][TEXTURING_TYPE_COLOR].iOperation = TEXTURING_OPERATION_MODULATEALPHA_ADDCOLOR;
	m_LightData.TextureState[1][TEXTURING_TYPE_ALPHA].iArgument[0] = TEXTURING_ARG_DIFFUSE;
	m_LightData.TextureState[1][TEXTURING_TYPE_ALPHA].iArgument[1] = TEXTURING_ARG_TEXTURE;
	m_LightData.TextureState[1][TEXTURING_TYPE_ALPHA].iOperation = TEXTURING_OPERATION_SELECTARG2;
}
