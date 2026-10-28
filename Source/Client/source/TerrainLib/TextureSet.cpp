#include "StdAfx.h"
#include "TextureSet.h"

CTextureSet::CTextureSet()
{
	Initialize();
}

CTextureSet::~CTextureSet()
{
	Clear();
}

void CTextureSet::Initialize()
{
}

void CTextureSet::Create()
{
	CResource * pResource = CResourceManager::Instance().GetResourcePointer("d:/ymir work/special/error.tga");
	if (pResource && pResource->IsType(CGraphicImage::Type()))
	{
		m_ErrorTexture.ImageInstance.SetImagePointer(static_cast<CGraphicImage *>(pResource));
		CGraphicImage * pImage = m_ErrorTexture.ImageInstance.GetGraphicImagePointer();
		if (pImage && pImage->GetTexturePointer())
		{
			m_ErrorTexture.pd3dTexture = pImage->GetTexturePointer()->GetD3DTexture();
		}
	}
	m_ErrorTexture.bLoaded = true;
	AddEmptyTexture();
}

bool CTextureSet::Load(const char *c_szTextureSetFileName, float fTerrainTexCoordBase)
{
	NANOBEGIN
	Clear();

	CTokenVectorMap stTokenVectorMap;

	if (!LoadMultipleTextData(c_szTextureSetFileName, stTokenVectorMap))
	{
		TraceError("TextureSet::Load : cannot load %s", c_szTextureSetFileName);
		return false;
	}

	if (stTokenVectorMap.end() == stTokenVectorMap.find("textureset"))
	{
		TraceError("TextureSet::Load : syntax error, TextureSet (filename: %s)", c_szTextureSetFileName);
		return false;
	}
	
	if (stTokenVectorMap.end() == stTokenVectorMap.find("texturecount"))
	{
		TraceError("TextureSet::Load : syntax error, TextureCount (filename: %s)", c_szTextureSetFileName);
		return false;
	}

	Create();

	const std::string & c_rstrCount = stTokenVectorMap["texturecount"][0];

	long lCount = atol(c_rstrCount.c_str());
	char szTextureName[32 + 1];

	m_Textures.resize(lCount + 1);

	for (long i = 0; i < lCount; ++i)
	{
		_snprintf(szTextureName, sizeof(szTextureName), "texture%03d", i + 1);

		if (stTokenVectorMap.end() == stTokenVectorMap.find(szTextureName))
			continue;

		const CTokenVector & rVector = stTokenVectorMap[szTextureName];

		const std::string & c_rstrFileName	= rVector[0].c_str();
		const std::string & c_rstrUScale	= rVector[1].c_str();
		const std::string & c_rstrVScale	= rVector[2].c_str();
		const std::string & c_rstrUOffset	= rVector[3].c_str();
		const std::string & c_rstrVOffset	= rVector[4].c_str();
		const std::string & c_rstrbSplat	= rVector[5].c_str();
		const std::string & c_rstrBegin		= rVector[6].c_str();
		const std::string & c_rstrEnd		= rVector[7].c_str();

		float fuScale, fvScale, fuOffset, fvOffset;
		bool bSplat;
		unsigned short usBegin, usEnd;

		fuScale	= atof(c_rstrUScale.c_str());
		fvScale = atof(c_rstrVScale.c_str());
		fuOffset = atof(c_rstrUOffset.c_str());
		fvOffset = atof(c_rstrVOffset.c_str());
		bSplat = 0 != atoi(c_rstrbSplat.c_str());
		usBegin = static_cast<unsigned short>(atoi(c_rstrBegin.c_str()));
		usEnd = static_cast<unsigned short>(atoi(c_rstrEnd.c_str()));

		if (!SetTexture(i + 1, c_rstrFileName.c_str(), fuScale, fvScale, fuOffset, fvOffset, bSplat, usBegin, usEnd, fTerrainTexCoordBase))
			TraceError("CTextureSet::Load : SetTexture failed : Filename: %s", c_rstrFileName.c_str());
	}

	m_stFileName.assign(c_szTextureSetFileName);
	NANOEND
	return true;
}

void CTextureSet::Clear()
{
	m_ErrorTexture.ImageInstance.Destroy();
	m_ErrorTexture.pd3dTexture = nullptr;
	m_ErrorTexture.bLoaded = false;
	m_Textures.clear();
	Initialize();
}

void CTextureSet::AddEmptyTexture()
{
	TTerrainTexture eraser;
	m_Textures.push_back(eraser);
}

unsigned long CTextureSet::GetTextureCount()
{
	return m_Textures.size();
}

TTerrainTexture	& CTextureSet::GetTexture(unsigned long ulIndex)
{
	if (GetTextureCount() <= ulIndex)
		return m_ErrorTexture;

	TTerrainTexture & tex = m_Textures[ulIndex];
	if (!tex.bLoaded && !tex.stFilename.empty())
	{
		tex.bLoaded = true;
		CResource * pResource = CResourceManager::Instance().GetResourcePointer(tex.stFilename.c_str());
		if (pResource && pResource->IsType(CGraphicImage::Type()))
		{
			tex.ImageInstance.SetImagePointer(static_cast<CGraphicImage *>(pResource));
			CGraphicImage * pImage = tex.ImageInstance.GetGraphicImagePointer();
			if (pImage && pImage->GetTexturePointer())
			{
				tex.pd3dTexture = pImage->GetTexturePointer()->GetD3DTexture();
			}
		}
		if (!tex.pd3dTexture)
		{
			tex.pd3dTexture = m_ErrorTexture.pd3dTexture;
		}
	}

	return tex;
}

bool CTextureSet::SetTexture(unsigned long ulIndex,
							 const char *c_szFileName,
							 float fuScale,
							 float fvScale,
							 float fuOffset,
							 float fvOffset,
							 bool bSplat,
							 unsigned short usBegin,
							 unsigned short usEnd,
							 float fTerrainTexCoordBase)
{
	if (ulIndex >= m_Textures.size())
	{
		TraceError("CTextureSet::SetTexture : Index Error : Index(%d) is Larger than TextureSet Size(%d)", ulIndex, m_Textures.size());
		return false;
	}

	TTerrainTexture & tex = m_Textures[ulIndex];

	tex.stFilename = c_szFileName;
	tex.UScale = fuScale;
	tex.VScale = fvScale;
	tex.UOffset = fuOffset;
	tex.VOffset = fvOffset;
	tex.bSplat = bSplat;
	tex.Begin = usBegin;
	tex.End = usEnd;
	tex.bLoaded = false;
	tex.pd3dTexture = nullptr;
	
	D3DXMatrixScaling(&tex.m_matTransform, fTerrainTexCoordBase * tex.UScale, -fTerrainTexCoordBase * tex.VScale, 0.0f);
	tex.m_matTransform._41 = tex.UOffset;
	tex.m_matTransform._42 = -tex.VOffset;
	return true;
}

void CTextureSet::Reload(float fTerrainTexCoordBase)
{
	for (DWORD dwIndex = 1; dwIndex < GetTextureCount(); ++dwIndex)
	{
		TTerrainTexture & tex = m_Textures[dwIndex];
		if (!tex.bLoaded || !tex.pd3dTexture)
			continue;

		tex.ImageInstance.ReloadImagePointer((CGraphicImage *) CResourceManager::Instance().GetResourcePointer(tex.stFilename.c_str()));
		CGraphicImage * pImage = tex.ImageInstance.GetGraphicImagePointer();
		if (pImage && pImage->GetTexturePointer())
			tex.pd3dTexture = pImage->GetTexturePointer()->GetD3DTexture();

		D3DXMatrixScaling(&tex.m_matTransform, fTerrainTexCoordBase * tex.UScale, -fTerrainTexCoordBase * tex.VScale, 0.0f);
		tex.m_matTransform._41 = tex.UOffset;
		tex.m_matTransform._42 = -tex.VOffset;
	}
}
							 
bool CTextureSet::AddTexture(const char *c_szFileName,
							 float fuScale,
							 float fvScale,
							 float fuOffset,
							 float fvOffset,
							 bool bSplat,
							 unsigned short usBegin,
							 unsigned short usEnd,
							 float fTerrainTexCoordBase)
{
	if (GetTextureCount() >= 256)
	{
		LogBox("You cannot add more than 255 texture.");
		return false;
	}

	for (unsigned long i = 1; i < GetTextureCount(); ++i)
	{
		if (0 == m_Textures[i].stFilename.compare(c_szFileName))
		{
			LogBox("Texture of the same name already exists.", "Duplicate");
			return false;
		}
	}

	m_Textures.reserve(m_Textures.size() + 1);

	SetTexture(m_Textures.size() - 1,
			   c_szFileName,
			   fuScale,
			   fvScale,
			   fuOffset,
			   fvOffset,
			   bSplat,
			   usBegin,
			   usEnd,
			   fTerrainTexCoordBase);

	return true;
}

bool CTextureSet::RemoveTexture(unsigned long ulIndex)
{
	if (GetTextureCount() <= ulIndex)
		return false;

	TTextureVector::iterator itor = m_Textures.begin() + ulIndex;
	m_Textures.erase(itor);
	return true;
}

bool CTextureSet::Save(const char *c_pszFileName)
{
	FILE * pFile = fopen(c_pszFileName, "w");
	
	if (!pFile)
		return false;
	
	fprintf(pFile, "TextureSet\n");
	fprintf(pFile, "\n");
	
	fprintf(pFile, "TextureCount %ld\n", GetTextureCount() - 1);
	fprintf(pFile, "\n");

	for (DWORD i = 1; i < GetTextureCount(); ++i)
	{
		TTerrainTexture & rTex = m_Textures[i];
		
		fprintf(pFile, "Start Texture%03d\n", i);
		fprintf(pFile, "    \"%s\"\n", rTex.stFilename.c_str());
		fprintf(pFile, "    %f\n", rTex.UScale);
		fprintf(pFile, "    %f\n", rTex.VScale);
		fprintf(pFile, "    %f\n", rTex.UOffset);
		fprintf(pFile, "    %f\n", rTex.VOffset);
		fprintf(pFile, "    %d\n", rTex.bSplat);
		fprintf(pFile, "    %hu\n", rTex.Begin);
		fprintf(pFile, "    %hu\n", rTex.End);
		fprintf(pFile, "End Texture%03d\n", i);
	}
	
	fclose(pFile);
	return true;
}