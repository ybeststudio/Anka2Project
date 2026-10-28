// ShadowRenderHelper.cpp: implementation of the CShadowRenderHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "../worldeditor.h"
#include "ShadowRenderHelper.h"
#include "MapAccessorOutdoor.h"
#include "../../EterLib/Camera.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#define new DEBUG_NEW
#endif

#ifndef SAFE_RELEASE
	#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = NULL; } }
#endif

CShadowRenderHelper aShadowRenderHelper;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShadowRenderHelper::CShadowRenderHelper() :
	m_lpShadowMapRenderTargetSurface (NULL),
	m_lpShadowMapDepthSurface (NULL),
	m_lpShadowMapRenderTargetTexture (NULL),
	m_lpBackupRenderTargetSurface (NULL),
	m_lpBackupDepthSurface (NULL),
	m_lpIntermediateRenderTargetSurface (NULL),
	m_lpIntermediateDepthSurface (NULL),
	m_lpIntermediateRenderTargetTexture (NULL),
	m_byMaxShadowMapPower (10),
	m_byMinShadowMapPower (8),
	m_byMaxIntermediateShadowMapPower (11),
	m_byMinIntermediateShadowMapPower (9),
	m_pMapOutdoorAccessor (NULL)
{
	SetShadowMapPower (10);
	SetIntermediateShadowMapPower (11);
	m_byPhase = 0;
	m_wCurCoordX = m_wCurCoordY = 0;

	m_eShadowMapTextureFilter = D3DTEXF_NONE;
	m_eIntermediateTextureFilter = D3DTEXF_NONE;
}

CShadowRenderHelper::~CShadowRenderHelper()
{
	ReleaseTextures();
}

void CShadowRenderHelper::SetIntermediateShadowMapPower (BYTE byShadowMapPower)
{
	m_byIntermediateShadowMapPower	= byShadowMapPower;

	if (m_byIntermediateShadowMapPower > m_byMaxIntermediateShadowMapPower)
	{
		m_byIntermediateShadowMapPower = m_byMaxIntermediateShadowMapPower;
	}

	if (m_byIntermediateShadowMapPower < m_byMinIntermediateShadowMapPower)
	{
		m_byIntermediateShadowMapPower = m_byMinIntermediateShadowMapPower;
	}

	m_dwIntermediateShadowMapSize = 1 << m_byIntermediateShadowMapPower;

	m_IntermediateViewport.X = 0;
	m_IntermediateViewport.Y = 0;
	m_IntermediateViewport.Width = m_dwIntermediateShadowMapSize;
	m_IntermediateViewport.Height = m_dwIntermediateShadowMapSize;
	m_IntermediateViewport.MinZ = 0.0f;
	m_IntermediateViewport.MaxZ = 1.0f;
}

void CShadowRenderHelper::SetShadowMapPower (BYTE byShadowMapPower)
{
	m_byShadowMapPower	= byShadowMapPower;

	if (m_byShadowMapPower > m_byMaxShadowMapPower)
	{
		m_byShadowMapPower = m_byMaxShadowMapPower;
	}
	if (m_byShadowMapPower < m_byMinShadowMapPower)
	{
		m_byShadowMapPower = m_byMinShadowMapPower;
	}

	m_dwShadowMapSize = 1 << m_byShadowMapPower;

	m_ShadowMapViewport.X = 0;
	m_ShadowMapViewport.Y = 0;
	m_ShadowMapViewport.Width = m_dwShadowMapSize;
	m_ShadowMapViewport.Height = m_dwShadowMapSize;
	m_ShadowMapViewport.MinZ = 0.0f;
	m_ShadowMapViewport.MaxZ = 1.0f;
}

bool CShadowRenderHelper::StartRenderingPhase (BYTE byPhase)
{
	CTerrain * pTerrain;
	const TOutdoorMapCoordinate & rOutdoorMapCoordinate = m_pMapOutdoorAccessor->GetCurCoordinate();
	const BYTE byNumTerrain = (m_wCurCoordY - rOutdoorMapCoordinate.m_sTerrainCoordY + 1) * 3 + (m_wCurCoordX - rOutdoorMapCoordinate.m_sTerrainCoordX + 1);

	if (!m_pMapOutdoorAccessor->GetTerrainPointer (byNumTerrain, &pTerrain))
	{
		return false;
	}

	bool bSuccess = true;

	switch (byPhase)
	{
		case 1:
		{
			D3DXVECTOR3 v3Target = D3DXVECTOR3 (
									   ((float) m_wCurCoordX * 2.0f + 1.0f) * 12800.0f,
									   - ((float) m_wCurCoordY * 2.0f + 1.0f) * 12800.0f,
									   pTerrain->GetHeight ((m_wCurCoordX * 2 + 1) * 12800, (m_wCurCoordY * 2 + 1) * 12800));
			// 				pTerrain->GetTerrainHeight(((float) m_wCurCoordX * 2.0f + 1.0f ) * 12800.0f, ((float) m_wCurCoordY * 2.0f + 1.0f) * 12800.0f));

			// 2시 방향으로 그림자 고정!
			D3DXVECTOR3 v3Eye (v3Target.x - 17320.0f, v3Target.y - 10000.0f, v3Target.z + 34640.0f);

			D3DXVECTOR3 pUp (0.0f, 0.0f, 1.0f);
			D3DXMatrixLookAtRH (&m_matLightView, &v3Eye, &v3Target, &pUp);

			D3DXMatrixOrthoRH (&m_matLightProj, 38400.0f, 38400.0f, 100.0f, 75000.0f);

			STATEMANAGER.SaveTransform (D3DTS_VIEW, &m_matLightView);
			STATEMANAGER.SaveTransform (D3DTS_PROJECTION, &m_matLightProj);

			CSpeedTreeForestDirectX9::Instance().UpdateCompundMatrix (v3Eye, m_matLightView, m_matLightProj);

			SaveRenderTarget();

			if (FAILED (ms_lpd3dDevice->SetRenderTarget (0, m_lpIntermediateRenderTargetSurface)))
			{
				Trace ("CShadowRenderHelper::StartRenderingPhase Unable to Set Intermediate Render Target");
				bSuccess = false;
			}
			if (FAILED(ms_lpd3dDevice->SetDepthStencilSurface(m_lpIntermediateDepthSurface)))
			{
				Trace("CShadowRenderHelper::StartRenderingPhase Unable to Set Intermediate Depth Stencil Surface");
				bSuccess = false;
			}

			/*
							if (FAILED(ms_lpd3dDevice->BeginScene()))
							{
								Trace("CShadowRenderHelper::StartRenderingPhase Unable to Being Scene");
								bSuccess = false;
							}
			*/

			if (FAILED (ms_lpd3dDevice->Clear (0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB (0xFF, 0xFF, 0xFF), 1.0f, 0)))
			{
				Trace ("CShadowRenderHelper::StartRenderingPhase Unable to Clear Render Target");
				bSuccess = false;
			}

			if (FAILED (ms_lpd3dDevice->GetViewport (&m_BackupViewport)))
			{
				LogBox ("Unable to Save Window Viewport\n");
				return false;
			}

			if (FAILED (ms_lpd3dDevice->SetViewport (&m_IntermediateViewport)))
			{
				Trace ("SetViewport Error : Phase 1\n");
				bSuccess = false;
			}

			STATEMANAGER.SaveRenderState (D3DRS_ALPHABLENDENABLE, FALSE);
			STATEMANAGER.SaveRenderState (D3DRS_TEXTUREFACTOR, 0xFF808080);
			STATEMANAGER.SaveTextureStageState (0, D3DTSS_COLORARG1,	D3DTA_TFACTOR);
			STATEMANAGER.SaveTextureStageState (0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
			STATEMANAGER.SaveSamplerState (0, D3DSAMP_MINFILTER,	m_eIntermediateTextureFilter);
			STATEMANAGER.SaveSamplerState (0, D3DSAMP_MAGFILTER,	m_eIntermediateTextureFilter);
			STATEMANAGER.SetTextureStageState (0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);

			STATEMANAGER.SetTextureStageState (1, D3DTSS_COLOROP,	D3DTOP_DISABLE);
			STATEMANAGER.SetTextureStageState (1, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);
		}
		break;

		case 2:
		{
			D3DXMATRIX textureMatrix, matTemp, matTopView, matTopProj;

			D3DXVECTOR3 v3Target = D3DXVECTOR3 (
									   ((float) m_wCurCoordX * 2.0f + 1.0f) * 12800.0f,
									   - ((float) m_wCurCoordY * 2.0f + 1.0f) * 12800.0f,
									   pTerrain->GetHeight ((m_wCurCoordX * 2 + 1) * 12800, (m_wCurCoordY * 2 + 1) * 12800));
			//					pTerrain->GetTerrainHeight(((float) m_wCurCoordX * 2.0f + 1.0f) * 12800.0f, ((float) m_wCurCoordY * 2.0f + 1.0f) * 12800.0f));

			D3DXVECTOR3 v3Eye (v3Target.x, v3Target.y, v3Target.z + 30000.0f);

			D3DXVECTOR3 pUp (0.0f, 1.0f, 0.0f);
			D3DXMatrixLookAtRH (&matTopView, &v3Eye, &v3Target, &pUp);

			float fDeterminantMatView = D3DXMatrixDeterminant (&matTopView);
			D3DXMatrixInverse (&textureMatrix, &fDeterminantMatView, &matTopView);

			textureMatrix *= m_matLightView;

			D3DXMatrixScaling (&matTemp, 1.0f / 38400.0f, -1.0f / 38400.0f, 1.0f);
			textureMatrix *= matTemp;

			D3DXMatrixTranslation (&matTemp, 0.5f, 0.5f, 0.0f);
			textureMatrix *= matTemp;

			D3DXMatrixOrthoRH (&matTopProj, 25600.0f, 25600.0f, 0.0f, 50000.0f);

			STATEMANAGER.SaveTransform (D3DTS_VIEW, &matTopView);
			STATEMANAGER.SaveTransform (D3DTS_PROJECTION, &matTopProj);

			SaveRenderTarget();

			if (FAILED (ms_lpd3dDevice->SetRenderTarget (0, m_lpShadowMapRenderTargetSurface)))
			{
				LogBox ("CShadowRenderHelper::StartRenderingPhase Unable to Set Shadow Map Render Target");
				bSuccess = false;
			}

			if (FAILED(ms_lpd3dDevice->SetDepthStencilSurface(m_lpShadowMapDepthSurface)))
			{
				LogBox("CShadowRenderHelper::StartRenderingPhase Unable to Set Shadow Map Depth Stencil Surface");
				bSuccess = false;
			}

			if (FAILED (ms_lpd3dDevice->Clear (0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB (0xFF, 0xFF, 0xFF), 1.0f, 0)))
			{
				LogBox ("CShadowRenderHelper::StartRenderingPhase Unable to Clear Render Target");
				bSuccess = false;
			}

			if (FAILED (ms_lpd3dDevice->GetViewport (&m_BackupViewport)))
			{
				LogBox ("Unable to Save Window Viewport\n");
				return false;
			}


			if (FAILED (ms_lpd3dDevice->SetViewport (&m_ShadowMapViewport)))
			{
				Trace ("SetViewport Error : Phase 2\n");
				bSuccess = false;
			}

			STATEMANAGER.SaveRenderState (D3DRS_ALPHABLENDENABLE, FALSE);

			// Bind shadow map depth texture to unit 0
			STATEMANAGER.SetTexture (0, m_lpIntermediateRenderTargetTexture);
			STATEMANAGER.SetTexture (1, NULL);

			STATEMANAGER.SaveSamplerState (0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			STATEMANAGER.SaveSamplerState (0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
			// use linear filtering
			STATEMANAGER.SaveSamplerState (0, D3DSAMP_MINFILTER, m_eShadowMapTextureFilter);
			STATEMANAGER.SaveSamplerState (0, D3DSAMP_MAGFILTER, m_eShadowMapTextureFilter);

			STATEMANAGER.SaveTransform (D3DTS_TEXTURE0, &textureMatrix);

			// Set up texture coordinate generation
			STATEMANAGER.SaveTextureStageState (0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
			STATEMANAGER.SaveTextureStageState (0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

			STATEMANAGER.SetTextureStageState (0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
			STATEMANAGER.SetTextureStageState (0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		}
		break;
	}

	return bSuccess;
}

void CShadowRenderHelper::EndRenderingPhase (BYTE byPhase)
{
	switch (byPhase)
	{
		case 1:
		{
			STATEMANAGER.RestoreRenderState (D3DRS_ALPHABLENDENABLE);
			STATEMANAGER.RestoreRenderState (D3DRS_TEXTUREFACTOR);
			STATEMANAGER.RestoreTextureStageState (0, D3DTSS_COLOROP);
			STATEMANAGER.RestoreTextureStageState (0, D3DTSS_COLORARG1);
			STATEMANAGER.RestoreSamplerState (0, D3DSAMP_MINFILTER);
			STATEMANAGER.RestoreSamplerState (0, D3DSAMP_MAGFILTER);

			STATEMANAGER.RestoreTransform (D3DTS_VIEW);
			STATEMANAGER.RestoreTransform (D3DTS_PROJECTION);

			ms_lpd3dDevice->SetViewport (&m_BackupViewport);
			RestoreRenderTarget();
		}
		break;

		case 2:
		{
			STATEMANAGER.RestoreTransform (D3DTS_TEXTURE0);
			STATEMANAGER.RestoreTransform (D3DTS_VIEW);
			STATEMANAGER.RestoreTransform (D3DTS_PROJECTION);

			STATEMANAGER.RestoreRenderState (D3DRS_ALPHABLENDENABLE);
			STATEMANAGER.RestoreSamplerState (0, D3DSAMP_ADDRESSU);
			STATEMANAGER.RestoreSamplerState (0, D3DSAMP_ADDRESSV);
			STATEMANAGER.RestoreSamplerState (0, D3DSAMP_MINFILTER);
			STATEMANAGER.RestoreSamplerState (0, D3DSAMP_MAGFILTER);
			STATEMANAGER.RestoreTextureStageState (0, D3DTSS_TEXCOORDINDEX);
			STATEMANAGER.RestoreTextureStageState (0, D3DTSS_TEXTURETRANSFORMFLAGS);

			ms_lpd3dDevice->SetViewport (&m_BackupViewport);
			RestoreRenderTarget();

			CSpeedTreeForestDirectX9::Instance().UpdateCompundMatrix (CCameraManager::Instance().GetCurrentCamera()->GetEye(), ms_matView, ms_matProj);
		}
		break;
	}
}

bool CShadowRenderHelper::SaveRenderTarget()
{
	if (FAILED (ms_lpd3dDevice->GetRenderTarget (0, &m_lpBackupRenderTargetSurface)))
	{
		LogBox ("Unable to Save Window Render Target\n");
		return false;
	}

	if (FAILED (ms_lpd3dDevice->GetDepthStencilSurface (&m_lpBackupDepthSurface)))
	{
		LogBox ("Unable to Save Window Depth Surface\n");
		return false;
	}

	return true;
}

void CShadowRenderHelper::RestoreRenderTarget()
{
	if (m_lpBackupRenderTargetSurface == NULL && m_lpBackupDepthSurface == NULL)
	{
		return;
	}

	ms_lpd3dDevice->SetRenderTarget (0, m_lpBackupRenderTargetSurface);
	ms_lpd3dDevice->SetDepthStencilSurface(m_lpBackupDepthSurface);

	SAFE_RELEASE (m_lpBackupDepthSurface);
	SAFE_RELEASE (m_lpBackupRenderTargetSurface);
}

bool CShadowRenderHelper::CreateTextures()
{
	// 처음 한번만 만든다.
	if (m_lpShadowMapRenderTargetTexture && m_lpIntermediateRenderTargetTexture)
	{
		return true;
	}

	// Release any old textures
	ReleaseTextures();

	// Shadow Map
	if (FAILED (ms_lpd3dDevice->CreateTexture (m_dwShadowMapSize, m_dwShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT,
											   &m_lpShadowMapRenderTargetTexture, nullptr)))
	{
		LogBox ("Unable to create ShadowMap render target texture");
		return false;
	}

	if (FAILED (m_lpShadowMapRenderTargetTexture->GetSurfaceLevel (0, &m_lpShadowMapRenderTargetSurface)))
	{
		LogBox ("Unable to GetSurfaceLevel ShadowMap render target texture");
		return false;
	}

	if (FAILED (ms_lpd3dDevice->CreateDepthStencilSurface (m_dwShadowMapSize, m_dwShadowMapSize, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, true, &m_lpShadowMapDepthSurface, nullptr)))
	{
		LogBox ("Unable to create Output ShadowMap depth Surface");
		return false;
	}

	// Intermediate
	if (FAILED (ms_lpd3dDevice->CreateTexture (m_dwIntermediateShadowMapSize, m_dwIntermediateShadowMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT,
											   &m_lpIntermediateRenderTargetTexture, nullptr)))
	{
		LogBox ("Unable to create Intermediate Shadow render target texture");
		return false;
	}

	if (FAILED (m_lpIntermediateRenderTargetTexture->GetSurfaceLevel (0, &m_lpIntermediateRenderTargetSurface)))
	{
		LogBox ("Unable to GetSurfaceLevel Intermediate Shadow render target texture");
		return false;
	}

	if (FAILED (ms_lpd3dDevice->CreateDepthStencilSurface (m_dwIntermediateShadowMapSize, m_dwIntermediateShadowMapSize, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, true,
														   &m_lpIntermediateDepthSurface, nullptr)))
	{
		LogBox ("Unable to create Intermediate Shadow depth Surface");
		return false;
	}

	return true;
}

void CShadowRenderHelper::ReleaseTextures()
{
	SAFE_RELEASE (m_lpShadowMapDepthSurface);
	SAFE_RELEASE (m_lpShadowMapRenderTargetTexture);
	SAFE_RELEASE (m_lpShadowMapRenderTargetSurface);

	SAFE_RELEASE (m_lpIntermediateDepthSurface);
	SAFE_RELEASE (m_lpIntermediateRenderTargetTexture);
	SAFE_RELEASE (m_lpIntermediateRenderTargetSurface);
}
