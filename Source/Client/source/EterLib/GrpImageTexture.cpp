#include "StdAfx.h"
#include "../eterBase/MappedFile.h"
#include "../eterPack/EterPackManager.h"
#include "GrpImageTexture.h"

#ifdef ENABLE_DIRECTX9_UPDATE
#include <boost/algorithm/string.hpp>
#include "../EterBase/DDSTextureLoader.h"
#include <d3dx9tex.h>
#endif

#if defined(DirectX9Ex) || defined(DIRECTX9)
#define DEFAULT_IMAGE_TEXTURE_POOL D3DPOOL_DEFAULT
#define DEFAULT_IMAGE_TEXTURE_USAGE D3DUSAGE_DYNAMIC
#else
#define DEFAULT_IMAGE_TEXTURE_POOL D3DPOOL_MANAGED
#define DEFAULT_IMAGE_TEXTURE_USAGE 0
#endif

bool CGraphicImageTexture::Lock(int* pRetPitch, void** ppRetPixels, int level)
{
	D3DLOCKED_RECT lockedRect;
	if (FAILED(m_lpd3dTexture->LockRect(level, &lockedRect, nullptr, 0)))
		return false;

	*pRetPitch = lockedRect.Pitch;
	*ppRetPixels = (void*)lockedRect.pBits;
	return true;
}

void CGraphicImageTexture::Unlock(int level)
{
	assert(m_lpd3dTexture != nullptr);
	m_lpd3dTexture->UnlockRect(level);
}

void CGraphicImageTexture::Initialize()
{
	CGraphicTexture::Initialize();

	m_stFileName = "";

	m_d3dFmt=D3DFMT_UNKNOWN;
	m_dwFilter=0;
}

void CGraphicImageTexture::Destroy()
{
	CGraphicTexture::Destroy();

	Initialize();
}

bool CGraphicImageTexture::CreateDeviceObjects()
{
	assert(ms_lpd3dDevice != nullptr);
	assert(m_lpd3dTexture == nullptr);

    if (m_stFileName.empty())
    {
		DWORD usage = 0;
		D3DPOOL pool = DEFAULT_IMAGE_TEXTURE_POOL;

		if (m_d3dFmt == D3DFMT_A8R8G8B8 || m_d3dFmt == D3DFMT_A4R4G4B4 || m_d3dFmt == D3DFMT_R5G6B5 || m_d3dFmt == D3DFMT_A1R5G5B5)
		{
#ifdef DirectX9Ex
			usage = D3DUSAGE_DYNAMIC;
			pool = D3DPOOL_DEFAULT;
#else
			pool = D3DPOOL_MANAGED;
#endif
		}

#ifdef ENABLE_DIRECTX9_UPDATE
        if (FAILED(ms_lpd3dDevice->CreateTexture(m_width
                                                    , m_height
                                                    , 1
                                                    , usage
                                                    , m_d3dFmt
                                                    , pool
                                                    , &m_lpd3dTexture
                                                    , nullptr)))
#else
        if (FAILED(ms_lpd3dDevice->CreateTexture(m_width
                                                    , m_height
                                                    , 1
                                                    , usage
                                                    , m_d3dFmt
                                                    , pool
                                                    , &m_lpd3dTexture)))
#endif
        {
            TraceError("CGraphicImageTexture::CreateDeviceObjects: CreateTexture failed (w=%u, h=%u, fmt=%d, usage=%u, pool=%d)", m_width, m_height, m_d3dFmt, usage, pool);
            return false;
        }
    }
    else
    {
		CMappedFile	mappedFile;
		LPCVOID		c_pvMap;

		if (!CEterPackManager::Instance().Get(mappedFile, m_stFileName.c_str(), &c_pvMap))
			return false;

		//@fixme002
		if (!CreateFromMemoryFile(mappedFile.Size(), c_pvMap, m_d3dFmt, m_dwFilter))
		{
			TraceError("CGraphicImageTexture::CreateDeviceObjects: CreateFromMemoryFile: texture not found(%s)", m_stFileName.c_str());
			return false;
		}
		return true;
		// return CreateFromMemoryFile(mappedFile.Size(), c_pvMap, m_d3dFmt, m_dwFilter);
	}

	m_bEmpty = false;
	return true;
}

bool CGraphicImageTexture::Create(UINT width, UINT height, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	assert(ms_lpd3dDevice != nullptr);
	Destroy();

	m_width = width;
	m_height = height;
	m_d3dFmt = d3dFmt;
	m_dwFilter = dwFilter;

	return CreateDeviceObjects();
}

void CGraphicImageTexture::CreateFromTexturePointer(const CGraphicTexture * c_pSrcTexture)
{
	if (m_lpd3dTexture)
		m_lpd3dTexture->Release();

	m_width = c_pSrcTexture->GetWidth();
	m_height = c_pSrcTexture->GetHeight();
	m_lpd3dTexture = c_pSrcTexture->GetD3DTexture();

	if (m_lpd3dTexture)
		m_lpd3dTexture->AddRef();

	m_bEmpty = false;
}

#ifndef ENABLE_DIRECTX9_UPDATE
bool CGraphicImageTexture::CreateDDSTexture(CDXTCImage & image, const BYTE * /*c_pbBuf*/)
{
	int mipmapCount = image.m_dwMipMapCount == 0 ? 1 : image.m_dwMipMapCount;

	D3DFORMAT format;
	LPDIRECT3DTEXTURE8 lpd3dTexture;
	D3DPOOL pool = ms_bSupportDXT ? DEFAULT_IMAGE_TEXTURE_POOL : D3DPOOL_SCRATCH;

	if(image.m_CompFormat == PF_DXT5)
		format = D3DFMT_DXT5;
	else if(image.m_CompFormat == PF_DXT3)
		format = D3DFMT_DXT3;
	else
		format = D3DFMT_DXT1;

	UINT uTexBias=0;
	if (IsLowTextureMemory())
		uTexBias=1;

	UINT uMinMipMapIndex=0;
	if (uTexBias>0)
	{
		if (mipmapCount>uTexBias)
		{
			uMinMipMapIndex=uTexBias;
			image.m_nWidth>>=uTexBias;
			image.m_nHeight>>=uTexBias;
			mipmapCount-=uTexBias;
		}
	}

	if (FAILED(D3DXCreateTexture(	ms_lpd3dDevice, image.m_nWidth, image.m_nHeight,
									mipmapCount, DEFAULT_IMAGE_TEXTURE_USAGE, format, pool, &lpd3dTexture)))
	{
		TraceError("CreateDDSTexture: Cannot creatre texture");
		return false;
	}

	for (DWORD i = 0; i < mipmapCount; ++i)
	{
		D3DLOCKED_RECT lockedRect;

		if (FAILED(lpd3dTexture->LockRect(i, &lockedRect, nullptr, 0)))
		{
			TraceError("CreateDDSTexture: Cannot lock texture");
		}
		else
		{
			image.Copy(i+uMinMipMapIndex, (BYTE*)lockedRect.pBits, lockedRect.Pitch);
			lpd3dTexture->UnlockRect(i);
		}
	}

	if(ms_bSupportDXT)
	{
		m_lpd3dTexture = lpd3dTexture;
	}
	else
	{
		if(image.m_CompFormat == PF_DXT3 || image.m_CompFormat == PF_DXT5)
			format = D3DFMT_A4R4G4B4;
		else
			format = D3DFMT_A1R5G5B5;

		UINT imgWidth=image.m_nWidth;
		UINT imgHeight=image.m_nHeight;

		extern bool GRAPHICS_CAPS_HALF_SIZE_IMAGE;

		if (GRAPHICS_CAPS_HALF_SIZE_IMAGE && uTexBias>0 && mipmapCount==0)
		{
			imgWidth>>=uTexBias;
			imgHeight>>=uTexBias;
		}

		if (FAILED(D3DXCreateTexture(	ms_lpd3dDevice, imgWidth, imgHeight,
										mipmapCount, DEFAULT_IMAGE_TEXTURE_USAGE, format, DEFAULT_IMAGE_TEXTURE_POOL, &m_lpd3dTexture)))
		{
				TraceError("CreateDDSTexture: Cannot creatre texture");
				return false;
		}

		IDirect3DTexture8* pkTexSrc=lpd3dTexture;
		IDirect3DTexture8* pkTexDst=m_lpd3dTexture;

		for(int i=0; i<mipmapCount; ++i) {

			IDirect3DSurface8* ppsSrc = nullptr;
			IDirect3DSurface8* ppsDst = nullptr;

			if (SUCCEEDED(pkTexSrc->GetSurfaceLevel(i, &ppsSrc)))
			{
				if (SUCCEEDED(pkTexDst->GetSurfaceLevel(i, &ppsDst)))
				{
					D3DXLoadSurfaceFromSurface(ppsDst, nullptr, nullptr, ppsSrc, nullptr, nullptr, D3DX_FILTER_NONE, 0);
					ppsDst->Release();
				}
				ppsSrc->Release();
			}
		}

		lpd3dTexture->Release();
	}

	m_width = image.m_nWidth;
	m_height = image.m_nHeight;
	m_bEmpty = false;

	return true;
}
#endif


#ifdef DirectX9Ex
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

bool CGraphicImageTexture::CreateDDSTexture(CDXTCImage& image, const BYTE* c_pbBuf, UINT bufSize)
{
	D3DFORMAT format;
	if (image.m_CompFormat == PF_DXT5)
		format = D3DFMT_DXT5;
	else if (image.m_CompFormat == PF_DXT3)
		format = D3DFMT_DXT3;
	else
		format = D3DFMT_DXT1;

	UINT uTexBias = IsLowTextureMemory() ? 1 : 0;
	int width = image.m_nWidth >> uTexBias;
	int height = image.m_nHeight >> uTexBias;

	if (!ms_lpd3dDevice)
	{
		TraceError("CreateDDSTexture: device is NULL");
		return false;
	}
	if (!c_pbBuf || bufSize == 0)
	{
		TraceError("CreateDDSTexture: invalid buffer (size=%u)", bufSize);
		return false;
	}

	SAFE_RELEASE(m_lpd3dTexture);

	D3DXIMAGE_INFO imageInfo;
	if (FAILED(D3DXGetImageInfoFromFileInMemory(c_pbBuf, bufSize, &imageInfo)))
	{
		TraceError("CreateFromMemoryFile: Cannot GetImageInfo from texture");
		return false;
	}

	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx(
		ms_lpd3dDevice,
		c_pbBuf,
		bufSize,
		imageInfo.Width,
		imageInfo.Height,
		imageInfo.MipLevels,
		0,
		format,
		D3DPOOL_DEFAULT,
		D3DX_FILTER_NONE,
		D3DX_FILTER_NONE,
		0,
		NULL,
		NULL,
		&m_lpd3dTexture);

	if (FAILED(hr) || !m_lpd3dTexture)
	{
		TraceError("CreateDDSTexture: load failed (bufSize=%u, hr=0x%08x)", bufSize, hr);
		return false;
	}

	m_width = width;
	m_height = height;
	m_d3dFmt = format;
	m_bEmpty = false;
	return true;
}
#endif

bool CGraphicImageTexture::CreateFromMemoryFile(UINT bufSize, const void * c_pvBuf, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	assert(ms_lpd3dDevice != nullptr);
	assert(m_lpd3dTexture == nullptr);

	if (!c_pvBuf || bufSize == 0)
		return false;

	static CDXTCImage image;

#ifdef DirectX9Ex
	if (image.LoadHeaderFromMemory((const BYTE*)c_pvBuf))
	{
		return CreateDDSTexture(image, (const BYTE*)c_pvBuf, bufSize);
	}
	else if (bufSize > sizeof(DWORD) && *(DWORD*)c_pvBuf == 124) // Headerless DDS
	{
		std::vector<BYTE> ddsBuf(bufSize + 4);
		*(DWORD*)&ddsBuf[0] = MAKEFOURCC('D', 'D', 'S', ' ');
		memcpy(&ddsBuf[4], c_pvBuf, bufSize);
		if (image.LoadHeaderFromMemory(&ddsBuf[0]))
		{
			return CreateDDSTexture(image, &ddsBuf[0], bufSize + 4);
		}
	}

	D3DXIMAGE_INFO imageInfo;
	HRESULT hr = D3DXGetImageInfoFromFileInMemory(c_pvBuf, bufSize, &imageInfo);
	if (FAILED(hr))
	{
		TraceError("CreateFromMemoryFile: Cannot GetImageInfo from texture (bufSize=%u, hr=0x%08x)", bufSize, hr);
		return false;
	}

	DWORD filter = dwFilter ? dwFilter : D3DX_FILTER_NONE;

	hr = D3DXCreateTextureFromFileInMemoryEx(
		ms_lpd3dDevice,
		c_pvBuf,
		bufSize,
		imageInfo.Width,
		imageInfo.Height,
		imageInfo.MipLevels,
		0,
		d3dFmt,
		D3DPOOL_DEFAULT,
		filter,
		filter,
		0xffff00ff,
		&imageInfo,
		NULL,
		&m_lpd3dTexture);

	if (FAILED(hr) || !m_lpd3dTexture)
	{
		TraceError("CreateFromMemoryFile: Cannot create texture %s (hr=0x%08x)", m_stFileName.c_str(), hr);
		return false;
	}

	D3DSURFACE_DESC desc;
	if (SUCCEEDED(m_lpd3dTexture->GetLevelDesc(0, &desc)))
	{
		m_width = desc.Width;
		m_height = desc.Height;
		m_d3dFmt = desc.Format;
	}
	else
	{
		m_width = imageInfo.Width;
		m_height = imageInfo.Height;
		m_d3dFmt = imageInfo.Format;
	}
	m_bEmpty = false;
	return true;
}
#elif defined(ENABLE_DIRECTX9_UPDATE)
    std::string extension = CFileNameHelper::GetExtension(m_stFileName);
    boost::algorithm::to_lower(extension);

    if (!m_stFileName.empty() && extension == "dds")
    {
        HRESULT hr = DirectX::CreateDDSTextureFromMemoryEx(ms_lpd3dDevice
                                                    , (const uint8_t *)c_pvBuf
                                                    , bufSize
                                                    , 0
                                                    , DEFAULT_IMAGE_TEXTURE_POOL
                                                    , false
                                                    , &m_lpd3dTexture);

        if (FAILED(hr))
        {
            D3DXIMAGE_INFO imageInfo;
            hr = D3DXCreateTextureFromFileInMemoryEx(ms_lpd3dDevice
                                                        , c_pvBuf
                                                        , bufSize
                                                        , D3DX_DEFAULT_NONPOW2
                                                        , D3DX_DEFAULT_NONPOW2
                                                        , 1
                                                        , 0
                                                        , m_d3dFmt
                                                        , DEFAULT_IMAGE_TEXTURE_POOL
                                                        , D3DX_FILTER_NONE
                                                        , D3DX_FILTER_NONE
                                                        , 0xffff00ff
                                                        , &imageInfo
                                                        , nullptr
                                                        , &m_lpd3dTexture);

            if (FAILED(hr))
            {
                TraceError("%s: failed to create texture", m_stFileName);
                return false;
            }

            m_bEmpty = false;
            m_width = imageInfo.Width;
            m_height = imageInfo.Height;
            return true;
        }

        D3DSURFACE_DESC desc;
        m_lpd3dTexture->GetLevelDesc(0, &desc);

        m_width = desc.Width;
        m_height = desc.Height;
        m_d3dFmt = desc.Format;
        m_bEmpty = false;
        return true;
    }
#else
    else
    {
        D3DXIMAGE_INFO imageInfo;
//#ifdef ENABLE_DIRECTX9_UPDATE
        if (FAILED(D3DXGetImageInfoFromFileInMemory(c_pvBuf, bufSize, &imageInfo)))
        {
            TraceError("CreateFromMemoryFile: Cannot get image info");
            return false;
        }
//#endif

#ifdef DirectX9Ex
        const D3DPOOL poolType = D3DPOOL_DEFAULT;
#else
        const D3DPOOL poolType = D3DPOOL_MANAGED;
#endif
        if (FAILED(D3DXCreateTextureFromFileInMemoryEx(
                    ms_lpd3dDevice,
                    c_pvBuf,
                    bufSize,
                    imageInfo.Width,
                    imageInfo.Height,
                    D3DX_DEFAULT,
                    0,
                    d3dFmt,
                    poolType,
                    dwFilter,
                    dwFilter,
                    0xffff00ff,
                    &imageInfo,
                    nullptr,
                    &m_lpd3dTexture)))
        {
            TraceError("CreateFromMemoryFile: Cannot create texture");
            return false;
        }

        m_width = imageInfo.Width;
        m_height = imageInfo.Height;

        D3DFORMAT format = imageInfo.Format;
        switch(imageInfo.Format)
        {
            case D3DFMT_A8R8G8B8:
            {
                format = D3DFMT_A4R4G4B4;
                break;
            }
            case D3DFMT_X8R8G8B8:
            case D3DFMT_R8G8B8:
            {
                format = D3DFMT_A1R5G5B5;
                break;
            }
            default:
            {
                break;
            }
        }

		UINT uTexBias=0;

		extern bool GRAPHICS_CAPS_HALF_SIZE_IMAGE;
		if (GRAPHICS_CAPS_HALF_SIZE_IMAGE)
			uTexBias=1;

		if (IsLowTextureMemory())
		if (uTexBias || format!=imageInfo.Format)
		{
#ifdef ENABLE_DIRECTX9_UPDATE
            IDirect3DTexture9* pkTexSrc = m_lpd3dTexture;
            IDirect3DTexture9* pkTexDst;
#else
            IDirect3DTexture8* pkTexSrc = m_lpd3dTexture;
            IDirect3DTexture8* pkTexDst;
#endif

			if (SUCCEEDED(D3DXCreateTexture(
				ms_lpd3dDevice,
				imageInfo.Width>>uTexBias,
				imageInfo.Height>>uTexBias,
				imageInfo.MipLevels,
				DEFAULT_IMAGE_TEXTURE_USAGE,
				format,
				DEFAULT_IMAGE_TEXTURE_POOL,
				&pkTexDst)))
			{
				m_lpd3dTexture=pkTexDst;

				for(int i=0; i< imageInfo.MipLevels; ++i)
                {
#ifdef ENABLE_DIRECTX9_UPDATE
                    IDirect3DSurface9* ppsSrc = nullptr;
                    IDirect3DSurface9* ppsDst = nullptr;
#else
					IDirect3DSurface8* ppsSrc = nullptr;
					IDirect3DSurface8* ppsDst = nullptr;
#endif

					if (SUCCEEDED(pkTexSrc->GetSurfaceLevel(i, &ppsSrc)))
					{
						if (SUCCEEDED(pkTexDst->GetSurfaceLevel(i, &ppsDst)))
						{
							D3DXLoadSurfaceFromSurface(ppsDst, nullptr, nullptr, ppsSrc, nullptr, nullptr, D3DX_FILTER_LINEAR, 0);
							ppsDst->Release();
						}
						ppsSrc->Release();
					}
				}

				pkTexSrc->Release();
			}
		}
	}

	m_bEmpty = false;
	return true;
}
#endif

void CGraphicImageTexture::SetFileName(const char * c_szFileName)
{
	m_stFileName=c_szFileName;
}

bool CGraphicImageTexture::CreateFromDiskFile(const char * c_szFileName, D3DFORMAT d3dFmt, DWORD dwFilter)
{
	Destroy();

	SetFileName(c_szFileName);

	m_d3dFmt = d3dFmt;
	m_dwFilter = dwFilter;
	return CreateDeviceObjects();
}

CGraphicImageTexture::CGraphicImageTexture()
{
	Initialize();
}

CGraphicImageTexture::~CGraphicImageTexture()
{
	Destroy();
}
