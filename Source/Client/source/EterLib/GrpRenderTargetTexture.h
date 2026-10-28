#pragma once

#ifdef ENABLE_RENDER_TARGET
#include "GrpBase.h"
#include "GrpTexture.h"

class CGraphicRenderTargetTexture : public CGraphicTexture
{
public:
	CGraphicRenderTargetTexture();
	virtual ~CGraphicRenderTargetTexture();

protected:
	void Reset();

#ifdef ENABLE_DIRECTX9_UPDATE
	LPDIRECT3DTEXTURE9 m_lpd3dRenderTexture{};
	LPDIRECT3DSURFACE9 m_lpd3dRenderTargetSurface{};
	LPDIRECT3DSURFACE9 m_lpd3dDepthSurface{};

	LPDIRECT3DSURFACE9 m_lpd3dOriginalRenderTarget{};
	LPDIRECT3DSURFACE9 m_lpd3dOldDepthBufferSurface{};
#else
	LPDIRECT3DTEXTURE8 m_lpd3dRenderTexture{};
	LPDIRECT3DSURFACE8 m_lpd3dRenderTargetSurface{};
	LPDIRECT3DSURFACE8 m_lpd3dDepthSurface{};

	LPDIRECT3DSURFACE8 m_lpd3dOriginalRenderTarget{};
	LPDIRECT3DSURFACE8 m_lpd3dOldDepthBufferSurface{};
#endif

	D3DFORMAT m_d3dFormat;
	D3DFORMAT m_depthStencilFormat;
	RECT m_renderRect{};

public:
	bool Create(int width, int height, D3DFORMAT texFormat, D3DFORMAT depthFormat);
	void CreateTextures();
	bool CreateRenderTexture(int width, int height, D3DFORMAT format);
	bool CreateRenderDepthStencil(int width, int height, D3DFORMAT format);

	bool SetRenderTarget();
	void ResetRenderTarget();

	void SetRenderingRect(RECT* rect);
	RECT* GetRenderingRect();

#ifdef ENABLE_DIRECTX9_UPDATE
	LPDIRECT3DTEXTURE9 GetRenderTargetTexture() const;
#else
	LPDIRECT3DTEXTURE8 GetRenderTargetTexture() const;
#endif

	void ReleaseTextures();
	static void Clear();

	void Render() const;
};
#endif
