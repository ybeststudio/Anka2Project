// ShadowRenderHelper.h: interface for the CShadowRenderHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWRENDERHELPER_H__262445E9_97FE_41E5_92DC_406DA02D72B4__INCLUDED_)
#define AFX_SHADOWRENDERHELPER_H__262445E9_97FE_41E5_92DC_406DA02D72B4__INCLUDED_

#pragma once

class CShadowRenderHelper : public CScreen, public CSingleton<CShadowRenderHelper>
{
	public:
		CShadowRenderHelper();
		virtual ~CShadowRenderHelper();

		bool CreateTextures();
		void ReleaseTextures();

		// Assign
		void SetMapOutdoorAccssorPointer (CMapOutdoorAccessor * pMapOutdoorAccessor)
		{
			m_pMapOutdoorAccessor = pMapOutdoorAccessor;
		}
		void SetTargetTerrainCoord (WORD wCoordX, WORD wCoordY)
		{
			m_wCurCoordX = wCoordX;
			m_wCurCoordY = wCoordY;
		}

		// Size
		void SetShadowMapPower (BYTE byShadowMapPower);
		void SetIntermediateShadowMapPower (BYTE byShadowMapPower);
		BYTE GetShadowMapPower()
		{
			return m_byShadowMapPower;
		}
		BYTE GetIntermediateShadowMapPower()
		{
			return m_byIntermediateShadowMapPower;
		}

		// Filter
		void SetShadowMapFilter (D3DTEXTUREFILTERTYPE eTextureFilter);
		void SetIntermediateShadowMapFilter (D3DTEXTUREFILTERTYPE eTextureFilter);

		// Output
		LPDIRECT3DTEXTURE9 GetShadowTexture()
		{
			return m_lpShadowMapRenderTargetTexture;
		}
		LPDIRECT3DTEXTURE9 GetIntermediateShadowTexture()
		{
			return m_lpIntermediateRenderTargetTexture;
		}

		// Render
		bool StartRenderingPhase (BYTE byPhase);
		void EndRenderingPhase (BYTE byPhase);

	private:

		// Backup
		bool SaveRenderTarget();
		void RestoreRenderTarget();

		// CMapOutdoorAccessor Pointer
		CMapOutdoorAccessor* 	m_pMapOutdoorAccessor;

		// ·£´õ¸µÇÒ ¶¥ ÁÂÇ¥
		WORD					m_wCurCoordX;
		WORD					m_wCurCoordY;

		// Rendering Phase
		BYTE					m_byPhase;

		// Size
		BYTE					m_byShadowMapPower;
		const BYTE				m_byMaxShadowMapPower;
		const BYTE				m_byMinShadowMapPower;
		DWORD					m_dwShadowMapSize;

		BYTE					m_byIntermediateShadowMapPower;
		const BYTE				m_byMaxIntermediateShadowMapPower;
		const BYTE				m_byMinIntermediateShadowMapPower;
		DWORD					m_dwIntermediateShadowMapSize;

		// Shadow Map
		LPDIRECT3DSURFACE9		m_lpShadowMapRenderTargetSurface;
		LPDIRECT3DSURFACE9		m_lpShadowMapDepthSurface;
		LPDIRECT3DTEXTURE9		m_lpShadowMapRenderTargetTexture;
		D3DVIEWPORT9			m_ShadowMapViewport;

		D3DTEXTUREFILTERTYPE	m_eShadowMapTextureFilter;

		// Backup
		LPDIRECT3DSURFACE9		m_lpBackupRenderTargetSurface;
		LPDIRECT3DSURFACE9		m_lpBackupDepthSurface;
		D3DVIEWPORT9			m_BackupViewport;

		// Áß°£ ·£´õ
		LPDIRECT3DSURFACE9		m_lpIntermediateRenderTargetSurface;
		LPDIRECT3DSURFACE9		m_lpIntermediateDepthSurface;
		LPDIRECT3DTEXTURE9		m_lpIntermediateRenderTargetTexture;
		D3DVIEWPORT9			m_IntermediateViewport;
		D3DXMATRIX				m_matLightView;
		D3DXMATRIX				m_matLightProj;

		D3DTEXTUREFILTERTYPE	m_eIntermediateTextureFilter;
};

#endif // !defined(AFX_SHADOWRENDERHELPER_H__262445E9_97FE_41E5_92DC_406DA02D72B4__INCLUDED_)
