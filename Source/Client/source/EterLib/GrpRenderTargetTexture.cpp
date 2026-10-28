#include "StdAfx.h"

#ifdef ENABLE_RENDER_TARGET
#include "../EterBase/Stl.h"
#include "GrpRenderTargetTexture.h"
#include "StateManager.h"

CGraphicRenderTargetTexture::CGraphicRenderTargetTexture()
	: m_d3dFormat{ D3DFMT_UNKNOWN }, m_depthStencilFormat{ D3DFMT_UNKNOWN }
{
	Initialize();

	memset(&m_renderRect, 0, sizeof(m_renderRect));
}

CGraphicRenderTargetTexture::~CGraphicRenderTargetTexture()
{
	Reset();
}

void CGraphicRenderTargetTexture::ReleaseTextures()
{
	safe_release(m_lpd3dRenderTexture);
	safe_release(m_lpd3dRenderTargetSurface);
	safe_release(m_lpd3dDepthSurface);
	safe_release(m_lpd3dDepthSurface);
	safe_release(m_lpd3dOriginalRenderTarget);
	safe_release(m_lpd3dOldDepthBufferSurface);

	memset(&m_renderRect, 0, sizeof(m_renderRect));
}

bool CGraphicRenderTargetTexture::Create(const int width, const int height, const D3DFORMAT texFormat, const D3DFORMAT depthFormat)
{
	Reset();

	m_height = height;
	m_width = width;

	if (!CreateRenderTexture(width, height, texFormat))
	{
		return false;
	}

	if (!CreateRenderDepthStencil(width, height, depthFormat))
	{
		return false;
	}

	return true;
}

void CGraphicRenderTargetTexture::CreateTextures()
{
    if (CreateRenderTexture(m_width, m_height, m_d3dFormat))
    {
        CreateRenderDepthStencil(m_width, m_height, m_depthStencilFormat);
    }
}

bool CGraphicRenderTargetTexture::CreateRenderTexture(const int width
                                                        , const int height
                                                        , const D3DFORMAT format)
{
	m_d3dFormat = format;

#ifdef ENABLE_DIRECTX9_UPDATE
    if (FAILED(ms_lpd3dDevice->CreateTexture(width
                                                , height
                                                , 1
                                                , D3DUSAGE_RENDERTARGET
                                                , m_d3dFormat
                                                , D3DPOOL_DEFAULT
                                                , &m_lpd3dRenderTexture
                                                , NULL)))
#else
    if (FAILED(ms_lpd3dDevice->CreateTexture(width
                                                , height
                                                , 1
                                                , D3DUSAGE_RENDERTARGET
                                                , m_d3dFormat
                                                , D3DPOOL_DEFAULT
                                                , &m_lpd3dRenderTexture)))
#endif
    {
        return false;
    }

    if (FAILED(m_lpd3dRenderTexture->GetSurfaceLevel(0, &m_lpd3dRenderTargetSurface)))
    {
        return false;
    }

    return true;
}

bool CGraphicRenderTargetTexture::CreateRenderDepthStencil(const int width
                                                            , const int height
                                                            , const D3DFORMAT format)
{
    m_depthStencilFormat = format;

#ifdef ENABLE_DIRECTX9_UPDATE
    return (ms_lpd3dDevice->CreateDepthStencilSurface(width
                                                        , height
                                                        , m_depthStencilFormat
                                                        , D3DMULTISAMPLE_NONE
                                                        , 0
                                                        , false
                                                        , &m_lpd3dDepthSurface
                                                        , NULL)) == D3D_OK;
#else
    return (ms_lpd3dDevice->CreateDepthStencilSurface(width
                                                        , height
                                                        , m_depthStencilFormat
                                                        , D3DMULTISAMPLE_NONE
                                                        , &m_lpd3dDepthSurface)) == D3D_OK;
#endif
}

bool CGraphicRenderTargetTexture::SetRenderTarget()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CGraphicRenderTargetTexture::SetRenderTarget **");

    m_lpd3dOriginalRenderTarget = nullptr;
    if (FAILED(ms_lpd3dDevice->GetRenderTarget(0, &m_lpd3dOriginalRenderTarget)))
#else
    m_lpd3dOriginalRenderTarget = nullptr;
    if (FAILED(ms_lpd3dDevice->GetRenderTarget(&m_lpd3dOriginalRenderTarget)))
#endif
    {
        TraceError("Failed to backup render target");
        return false;
    }

    m_lpd3dOldDepthBufferSurface = nullptr;
    if (FAILED(ms_lpd3dDevice->GetDepthStencilSurface(&m_lpd3dOldDepthBufferSurface)))
    {
        m_lpd3dOldDepthBufferSurface = nullptr;
    }

#ifdef ENABLE_DIRECTX9_UPDATE
    if (FAILED(ms_lpd3dDevice->SetRenderTarget(0, m_lpd3dRenderTargetSurface)))
    {
        TraceError("Failed to set Render Target");
        if (m_lpd3dOriginalRenderTarget)
        {
            ms_lpd3dDevice->SetRenderTarget(0, m_lpd3dOriginalRenderTarget);
            safe_release(m_lpd3dOriginalRenderTarget);
        }
        if (m_lpd3dOldDepthBufferSurface)
        {
            ms_lpd3dDevice->SetDepthStencilSurface(m_lpd3dOldDepthBufferSurface);
            safe_release(m_lpd3dOldDepthBufferSurface);
        }
        return false;
    }

    if (m_lpd3dDepthSurface)
    {
        if (FAILED(ms_lpd3dDevice->SetDepthStencilSurface(m_lpd3dDepthSurface)))
        {
            TraceError("Failed to set depth stencil surface");
            if (m_lpd3dOriginalRenderTarget)
            {
                ms_lpd3dDevice->SetRenderTarget(0, m_lpd3dOriginalRenderTarget);
                safe_release(m_lpd3dOriginalRenderTarget);
            }
            if (m_lpd3dOldDepthBufferSurface)
            {
                ms_lpd3dDevice->SetDepthStencilSurface(m_lpd3dOldDepthBufferSurface);
                safe_release(m_lpd3dOldDepthBufferSurface);
            }
            return false;
        }
    }
#else
    if (FAILED(ms_lpd3dDevice->SetRenderTarget(m_lpd3dRenderTargetSurface, m_lpd3dDepthSurface)))
    {
        TraceError("Failed to set Render Target");
        if (m_lpd3dOriginalRenderTarget)
        {
            ms_lpd3dDevice->SetRenderTarget(m_lpd3dOriginalRenderTarget, m_lpd3dOldDepthBufferSurface);
            safe_release(m_lpd3dOriginalRenderTarget);
        }
        safe_release(m_lpd3dOldDepthBufferSurface);
        return false;
    }
#endif

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif

    return true;
}

void CGraphicRenderTargetTexture::ResetRenderTarget()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CGraphicRenderTargetTexture::ResetRenderTarget **");

    if (m_lpd3dOriginalRenderTarget)
    {
        if (FAILED(ms_lpd3dDevice->SetRenderTarget(0, m_lpd3dOriginalRenderTarget)))
        {
            TraceError("Failed to switch to backup render target.");
        }
    }

    if (m_lpd3dOldDepthBufferSurface)
    {
        if (FAILED(ms_lpd3dDevice->SetDepthStencilSurface(m_lpd3dOldDepthBufferSurface)))
        {
            TraceError("Failed to switch to backup depth stencil.");
        }
    }
#else
    if (m_lpd3dOriginalRenderTarget)
    {
        if (FAILED(ms_lpd3dDevice->SetRenderTarget(m_lpd3dOriginalRenderTarget, m_lpd3dOldDepthBufferSurface)))
        {
            TraceError("Failed to switch to backup.");
        }
    }
#endif

    safe_release(m_lpd3dOriginalRenderTarget);
    safe_release(m_lpd3dOldDepthBufferSurface);

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CGraphicRenderTargetTexture::Clear()
{
    ms_lpd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
}

void CGraphicRenderTargetTexture::SetRenderingRect(RECT* rect)
{
    m_renderRect = *rect;
}

RECT* CGraphicRenderTargetTexture::GetRenderingRect()
{
    return &m_renderRect;
}

#ifdef ENABLE_DIRECTX9_UPDATE
LPDIRECT3DTEXTURE9 CGraphicRenderTargetTexture::GetRenderTargetTexture() const
#else
LPDIRECT3DTEXTURE8 CGraphicRenderTargetTexture::GetRenderTargetTexture() const
#endif
{
    return m_lpd3dRenderTexture;
}

void CGraphicRenderTargetTexture::Render() const
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 0, 127, 0), L"** CGraphicRenderTargetTexture::Render **");
#endif

	D3DSURFACE_DESC desc;
	m_lpd3dRenderTargetSurface->GetDesc(&desc);

	const auto imgWidth = static_cast<float>(m_renderRect.right - m_renderRect.left);
	const auto imgHeight = static_cast<float>(m_renderRect.bottom - m_renderRect.top);

	const auto su = 0.0f;
	const auto sv = 0.0f;
	const auto eu = imgWidth / static_cast<float>(desc.Width);
	const auto ev = imgHeight / static_cast<float>(desc.Height);

	TPDTVertex pVertices[4];

	pVertices[0].position = TPosition(m_renderRect.left - 0.5f, m_renderRect.top - 0.5f, 0.0f);
	pVertices[0].texCoord = TTextureCoordinate(su, sv);
	pVertices[0].diffuse = 0xffffffff;

	pVertices[1].position = TPosition(m_renderRect.right - 0.5f, m_renderRect.top - 0.5f, 0.0f);
	pVertices[1].texCoord = TTextureCoordinate(eu, sv);
	pVertices[1].diffuse = 0xffffffff;

	pVertices[2].position = TPosition(m_renderRect.left - 0.5f, m_renderRect.bottom - 0.5f, 0.0f);
	pVertices[2].texCoord = TTextureCoordinate(su, ev);
	pVertices[2].diffuse = 0xffffffff;

	pVertices[3].position = TPosition(m_renderRect.right - 0.5f, m_renderRect.bottom - 0.5f, 0.0f);
	pVertices[3].texCoord = TTextureCoordinate(eu, ev);
	pVertices[3].diffuse = 0xffffffff;

	if (SetPDTStream(pVertices, 4))
	{
        SetDefaultIndexBuffer(DEFAULT_IB_FILL_RECT);

        STATEMANAGER.SetTexture(0, GetRenderTargetTexture());
        STATEMANAGER.SetTexture(1, nullptr);
#ifdef ENABLE_DIRECTX9_UPDATE
        STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE);
#else
        STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_DIFFUSE);
#endif
        STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);
    }

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CGraphicRenderTargetTexture::Reset()
{
	Destroy();
	ReleaseTextures();
	m_d3dFormat = D3DFMT_UNKNOWN;
	m_depthStencilFormat = D3DFMT_UNKNOWN;
}
#endif