// MiniMapRenderHelper.cpp: implementation of the CMiniMapRenderHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "../worldeditor.h"
#include "MiniMapRenderHelper.h"
#include "MapAccessorOutdoor.h"
#include "../../EterLib/Camera.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#define new DEBUG_NEW
#endif

CMiniMapRenderHelper aMiniMapRenderHelper;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMiniMapRenderHelper::CMiniMapRenderHelper() :
	m_lpMiniMapRenderTargetSurface (NULL),
	m_lpMiniMapDepthSurface (NULL),
	m_lpMiniMapRenderTargetTexture (NULL),
	m_lpBackupRenderTargetSurface (NULL),
	m_lpBackupDepthSurface (NULL),
	m_pMapOutdoorAccessor (NULL)
{
	m_wCurCoordX = m_wCurCoordY = 0;
	SetMiniMapPower (8);
	m_eMiniMapTextureFilter = D3DTEXF_LINEAR;
}

CMiniMapRenderHelper::~CMiniMapRenderHelper()
{
	ReleaseTextures();
}

void CMiniMapRenderHelper::SetMiniMapPower (BYTE byMiniMapPower)
{
	m_byMiniMapPower = byMiniMapPower;

	m_dwMiniMapSize = 1 << m_byMiniMapPower;

	m_MiniMapViewport.X = 0;
	m_MiniMapViewport.Y = 0;
	m_MiniMapViewport.Width = m_dwMiniMapSize;
	m_MiniMapViewport.Height = m_dwMiniMapSize;
	m_MiniMapViewport.MinZ = 0.0f;
	m_MiniMapViewport.MaxZ = 1.0f;
}

bool CMiniMapRenderHelper::StartRendering()
{
	CTerrain * pTerrain;
	const TOutdoorMapCoordinate & rOutdoorMapCoordinate = m_pMapOutdoorAccessor->GetCurCoordinate();

	BYTE byTerrainNum;
	if (!m_pMapOutdoorAccessor->GetTerrainNumFromCoord (m_wCurCoordX, m_wCurCoordY, &byTerrainNum))
	{
		return false;
	}

	if (!m_pMapOutdoorAccessor->GetTerrainPointer (byTerrainNum, &pTerrain))
	{
		return false;
	}

	bool bSuccess = true;


	D3DXVECTOR3 v3Target = D3DXVECTOR3 (
							   ((float) m_wCurCoordX * 2.0f + 1.0f) * 12800.0f,
							   - ((float) m_wCurCoordY * 2.0f + 1.0f) * 12800.0f,
							   pTerrain->GetHeight ((m_wCurCoordX * 2 + 1) * 12800, (m_wCurCoordY * 2 + 1) * 12800));

	D3DXVECTOR3 v3Eye (v3Target.x, v3Target.y, v3Target.z + 32767.5f);

	D3DXMATRIX matTopProj;
	D3DXMatrixOrthoRH (&matTopProj, 25600.0f, 25600.0f, 0.0f, 65535.0f);

	m_matBackupProj = ms_matProj;

	CCameraManager::Instance().SetCurrentCamera (CCameraManager::DEFAULT_ORTHO_CAMERA);
	CCameraManager::Instance().GetCurrentCamera()->SetViewParams (v3Eye, v3Target, D3DXVECTOR3 (0.0f, 1.0f, 0.0f));
	ms_matProj = matTopProj;
	UpdatePipeLineMatrix();

	CSpeedTreeForestDirectX9::Instance().UpdateCompundMatrix (v3Eye, ms_matView, ms_matProj);

	SaveRenderTarget();

	if (FAILED (ms_lpd3dDevice->SetRenderTarget (0, m_lpMiniMapRenderTargetSurface)))
	{
		LogBox ("[ERROR] CMiniMapRenderHelper::StartRenderingPhase Unable to Set Mini Map Render Target");
		bSuccess = false;
	}

	if (FAILED(ms_lpd3dDevice->SetDepthStencilSurface(m_lpMiniMapDepthSurface)))
	{
		LogBox("[ERROR] CMiniMapRenderHelper::StartRenderingPhase Unable to Set Mini Map Depth Stencil Surface");
		bSuccess = false;
	}

	if (FAILED (ms_lpd3dDevice->Clear (0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB (0x00, 0x00, 0x00), 1.0f, 0)))
	{
		LogBox ("[ERROR] CMiniMapRenderHelper::StartRenderingPhase Unable to Clear Render Target");
		bSuccess = false;
	}

	if (FAILED (ms_lpd3dDevice->GetViewport (&m_BackupViewport)))
	{
		LogBox ("[ERROR] Unable to Save Window Viewport\n");
		return false;
	}

	if (FAILED (ms_lpd3dDevice->SetViewport (&m_MiniMapViewport)))
	{
		Trace ("[ERROR] SetViewport Error : Phase 2\n");
		bSuccess = false;
	}

	return bSuccess;
}

void CMiniMapRenderHelper::EndRendering()
{
	ms_lpd3dDevice->SetViewport (&m_BackupViewport);
	RestoreRenderTarget();

	CCameraManager::Instance().ResetToPreviousCamera();
	ms_matProj = m_matBackupProj;
	UpdatePipeLineMatrix();

	CSpeedTreeForestDirectX9::Instance().UpdateCompundMatrix (CCameraManager::Instance().GetCurrentCamera()->GetEye(), ms_matView, ms_matProj);
}

bool CMiniMapRenderHelper::SaveRenderTarget()
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

void CMiniMapRenderHelper::RestoreRenderTarget()
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

bool CMiniMapRenderHelper::CreateTextures()
{
	// 처음 한번만 만든다.
	if (m_lpMiniMapRenderTargetTexture)
	{
		return true;
	}

	// Release any old textures
	ReleaseTextures();

	// Mini Map
	if (FAILED (ms_lpd3dDevice->CreateTexture (m_dwMiniMapSize, m_dwMiniMapSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_lpMiniMapRenderTargetTexture, nullptr)))
	{
		LogBox ("[ERROR] CMiniMapRenderHelper::CreateTextures: Unable to create MiniMap render target texture");
		return false;
	}

	if (FAILED (m_lpMiniMapRenderTargetTexture->GetSurfaceLevel (0, &m_lpMiniMapRenderTargetSurface)))
	{
		LogBox ("[ERROR] CMiniMapRenderHelper::CreateTextures: Unable to GetSurfaceLevel MiniMap render target texture");
		return false;
	}

	if (FAILED (ms_lpd3dDevice->CreateDepthStencilSurface (m_dwMiniMapSize, m_dwMiniMapSize, D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, true, &m_lpMiniMapDepthSurface, nullptr)))
	{
		LogBox ("[ERROR] CMiniMapRenderHelper::CreateTextures: Unable to create Output MiniMap depth Surface");
		return false;
	}

	return true;
}

void CMiniMapRenderHelper::ReleaseTextures()
{
	SAFE_RELEASE (m_lpMiniMapDepthSurface);
	SAFE_RELEASE (m_lpMiniMapRenderTargetTexture);
	SAFE_RELEASE (m_lpMiniMapRenderTargetSurface);
}
