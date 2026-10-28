#include "StdAfx.h"
#include "../eterLib/StateManager.h"
#include "../eterLib/ResourceManager.h"
#if defined(ANK_SYSTEM_GRAPHICS)
#include "../EterLib/Camera.h"
#include "../UserInterface/PythonBackground.h"
#endif
#include "MapOutdoor.h"
#include "../UserInterface/PythonSystem.h"
#include "TerrainPatch.h"
#if defined(ANK_SYSTEM_GRAPHICS)
#include "FlyingObjectManager.h"
#include "MapManager.h"
#include "MapOutdoor.h"
#include <unordered_set>
#endif

void CMapOutdoor::LoadWaterTexture()
{
	UnloadWaterTexture();
	char buf[256];
	for (int i = 0; i < 30; ++i)
	{
#if defined(ANK_SYSTEM_GRAPHICS)
		if (std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_n_flame_dungeon_01") == 0 ||
			std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_n_flame_01") == 0)
		{
			sprintf(buf, "d:/ymir Work/special/lava/%02d.dds", i + 1);
		}
		else
		{
			sprintf(buf, "d:/ymir Work/special/water/%02d.dds", i + 1);
		}

		m_WaterInstances[i].SetImagePointer((CGraphicImage*)CResourceManager::Instance().GetResourcePointer(buf));
	}
#endif
}

void CMapOutdoor::UnloadWaterTexture()
{
	for (int i = 0; i < 30; ++i)
		m_WaterInstances[i].Destroy();
}

#if defined(ANK_SYSTEM_GRAPHICS)
bool CMapOutdoor::IsWaterVisible()
{
	if (!m_waterOcclusionQuery)
		return true;

	DWORD numberOfPixels = 0;
	if (m_waterOcclusionQuery->GetData(&numberOfPixels, sizeof(DWORD), D3DGETDATA_FLUSH) != S_OK)
		return true;

	return (numberOfPixels > 0);
}

D3DXVECTOR3 rippleOrigin = { 0.0f,0.0f,0.0f };
#endif
void CMapOutdoor::RenderWater()
{
	if (m_PatchVector.empty())
		return;

	if (!IsVisiblePart(PART_WATER))
		return;

	D3DXMATRIX matTexTransformWater;

	STATEMANAGER.SaveRenderState(D3DRS_ZWRITEENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	// Don't inherit the blend mode: terrain passes leave ZERO/SRCCOLOR (multiply) behind.
	STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	STATEMANAGER.SaveRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	STATEMANAGER.SaveRenderState(D3DRS_COLORVERTEX, TRUE);

	STATEMANAGER.SetTexture(0, m_WaterInstances[((ELTimer_GetMSec() / 70) % 30)].GetTexturePointer()->GetD3DTexture());

	// Resolve the water quality once per frame (was a string-keyed option lookup per patch).
	const int iWaterLevel = CPythonSystem::Instance().GetAdvancedOption("enhanced_water", 1) ? Frustum::Instance().graphicsWaterLevel : 0;
	const bool bWaterShader = iWaterLevel >= 1 && m_lpShadowMapVertexShaderWater && m_lpShadowMapPixelShaderWater;

	if (!bWaterShader)
	{
		STATEMANAGER.SetTexture(1, NULL);
		STATEMANAGER.SetTexture(2, NULL);
	}




	D3DXMatrixScaling(&matTexTransformWater, m_fWaterTexCoordBase * 2, -m_fWaterTexCoordBase * 2, 0.0f);
	D3DXMatrixMultiply(&matTexTransformWater, &m_matViewInverse, &matTexTransformWater);

	STATEMANAGER.SaveTransform(D3DTS_TEXTURE0, &matTexTransformWater);

	STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);


	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	STATEMANAGER.SaveSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// Stage 0
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// Stage 1

	// RenderState
	//////////////////////////////////////////////////////////////////////////

	static float s_fWaterHeightCurrent = 0;
	static float s_fWaterHeightBegin = 0;
	static float s_fWaterHeightEnd = 0;
	static DWORD s_dwLastHeightChangeTime = CTimer::Instance().GetCurrentMillisecond();
	static DWORD s_dwBlendtime = 300;

	if ((CTimer::Instance().GetCurrentMillisecond() - s_dwLastHeightChangeTime) > s_dwBlendtime)
	{
		s_dwBlendtime = random_range(1000, 6000);

		if (s_fWaterHeightEnd == 0)
			s_fWaterHeightEnd = -random_range(0, 30);
		else
			s_fWaterHeightEnd = 0;

		s_fWaterHeightBegin = s_fWaterHeightCurrent;
		s_dwLastHeightChangeTime = CTimer::Instance().GetCurrentMillisecond();
	}

	s_fWaterHeightCurrent = s_fWaterHeightBegin + (s_fWaterHeightEnd - s_fWaterHeightBegin) * (float)((CTimer::Instance().GetCurrentMillisecond() - s_dwLastHeightChangeTime) / (float)s_dwBlendtime);
	m_matWorldForCommonUse._43 = s_fWaterHeightCurrent;
	m_matWorldForCommonUse._41 = 0.0f;
	m_matWorldForCommonUse._42 = 0.0f;

	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorldForCommonUse);

	if (!m_waterOcclusionQuery)
		ms_lpd3dDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &m_waterOcclusionQuery);
	if (m_waterOcclusionQuery)
		m_waterOcclusionQuery->Issue(D3DISSUE_BEGIN);

	// Shader, constants and samplers are identical for every patch: bind them once.
	// Previously DrawWater re-set everything per patch and then cleared samplers 0-7,
	// so every patch after the first sampled NULL reflection/normal/foam textures and
	// turned black at grazing angles (fresnel -> reflection weight 1).
	if (bWaterShader)
		__SetWaterShaderState(iWaterLevel);

	std::vector<std::pair<float, long> >::iterator i;

	for (i = m_PatchVector.begin(); i != m_PatchVector.end(); ++i)
	{
		DrawWater(i->second);
	}

	if (bWaterShader)
	{
		STATEMANAGER.SetVertexShader(nullptr);
		STATEMANAGER.SetPixelShader(nullptr);
		STATEMANAGER.RestoreSamplerState(3, D3DSAMP_ADDRESSU);
		STATEMANAGER.RestoreSamplerState(3, D3DSAMP_ADDRESSV);
	}

	for (int s = 0; s < 8; ++s)
	{
		STATEMANAGER.SetTexture(s, nullptr);
	}
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	if (m_waterOcclusionQuery)
		m_waterOcclusionQuery->Issue(D3DISSUE_END);

	m_matWorldForCommonUse._43 = 0.0f;

	//////////////////////////////////////////////////////////////////////////
	// RenderState
#ifdef DIRECTX9
	STATEMANAGER.RestoreFVF();
#else
	STATEMANAGER.RestoreVertexShader();
#endif
	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE0);
#ifdef DIRECTX9
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MINFILTER);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MAGFILTER);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MIPFILTER);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_ADDRESSU);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_ADDRESSV);
#else
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_MINFILTER);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_MAGFILTER);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_MIPFILTER);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ADDRESSU);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ADDRESSV);
#endif
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS);
	STATEMANAGER.RestoreRenderState(D3DRS_DIFFUSEMATERIALSOURCE);
	STATEMANAGER.RestoreRenderState(D3DRS_COLORVERTEX);
	STATEMANAGER.RestoreRenderState(D3DRS_ZWRITEENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
}

void CMapOutdoor::SetWaterReflectionRenderTarget()
{
	STATEMANAGER.PushRenderTarget();
	if (m_lpWaterReflectionDepthSurface && Frustum::Instance().m_lpWaterReflectionTexture)
	{
		// Set the water reflection depth surface
		ms_lpd3dDevice->SetDepthStencilSurface(m_lpWaterReflectionDepthSurface);

		// Set the water reflection render target
		IDirect3DSurface9* targetSurf;
		Frustum::Instance().m_lpWaterReflectionTexture->GetSurfaceLevel(0, &targetSurf);
		if (FAILED(ms_lpd3dDevice->SetRenderTarget(0, targetSurf)))
		{
			TraceError("CMapOutdoor Failed to set water render target\n");
			targetSurf->Release();  // Ensure we release the surface if we fail to set it
			return;
		}
		targetSurf->Release();  // Release the surface after setting it

		D3DVIEWPORT9 reflectionViewport;
		reflectionViewport.X = 0;
		reflectionViewport.Y = 0;
		reflectionViewport.Width = m_waterReflectionTexSize;
		reflectionViewport.Height = m_waterReflectionTexSize;
		reflectionViewport.MinZ = 0.0f;
		reflectionViewport.MaxZ = 1.0f;
		ms_lpd3dDevice->SetViewport(&reflectionViewport);

		// Only clear when the reflection target is actually bound; otherwise this wiped the backbuffer.
#ifdef __DEBUG
		ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xFF, 0x00, 0xFF), 1.0f, 0);
#else
		ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
#endif
	}
}

void CMapOutdoor::SetPuddleReflectionRenderTarget()
{
	STATEMANAGER.PushRenderTarget();
	if (m_lpPuddleReflectionDepthSurface && Frustum::Instance().m_lpPuddleReflectionTexture)
	{
		// Set the water reflection depth surface
		ms_lpd3dDevice->SetDepthStencilSurface(m_lpPuddleReflectionDepthSurface);

		// Set the water reflection render target
		IDirect3DSurface9* targetSurf;
		Frustum::Instance().m_lpPuddleReflectionTexture->GetSurfaceLevel(0, &targetSurf);
		if (FAILED(ms_lpd3dDevice->SetRenderTarget(0, targetSurf)))
		{
			TraceError("CMapOutdoor Failed to set water render target\n");
			targetSurf->Release(); 
			return;
		}
		targetSurf->Release(); 

		D3DVIEWPORT9 reflectionViewport;
		reflectionViewport.X = 0;
		reflectionViewport.Y = 0;
		reflectionViewport.Width = m_waterPuddleReflectionTexSize;
		reflectionViewport.Height = m_waterPuddleReflectionTexSize;
		reflectionViewport.MinZ = 0.0f;
		reflectionViewport.MaxZ = 1.0f;
		ms_lpd3dDevice->SetViewport(&reflectionViewport);
	}

#ifdef __DEBUG
	ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0xFF, 0x00, 0xFF), 1.0f, 0);
#else
	ms_lpd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
#endif
}
#if defined(ANK_SYSTEM_GRAPHICS)

#else
void CMapOutdoor::RenderWater()
{
	if (m_PatchVector.empty())
		return;

	if (!IsVisiblePart(PART_WATER))
		return;

	//////////////////////////////////////////////////////////////////////////
	// RenderState
	D3DXMATRIX matTexTransformWater;

	STATEMANAGER.SaveRenderState(D3DRS_ZWRITEENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	STATEMANAGER.SaveRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
	STATEMANAGER.SaveRenderState(D3DRS_COLORVERTEX, TRUE);

	STATEMANAGER.SetTexture(0, m_WaterInstances[((ELTimer_GetMSec() / 70) % 30)].GetTexturePointer()->GetD3DTexture());

	D3DXMatrixScaling(&matTexTransformWater, m_fWaterTexCoordBase, -m_fWaterTexCoordBase, 0.0f);
	D3DXMatrixMultiply(&matTexTransformWater, &m_matViewInverse, &matTexTransformWater);

	STATEMANAGER.SaveTransform(D3DTS_TEXTURE0, &matTexTransformWater);
#if defined(DIRECTX9)
	STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
#else
	STATEMANAGER.SaveVertexShader(D3DFVF_XYZ|D3DFVF_DIFFUSE);
#endif
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
#ifndef DIRECTX9
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
#endif
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	STATEMANAGER.SetTexture(1,NULL);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	// RenderState
	//////////////////////////////////////////////////////////////////////////

	static float s_fWaterHeightCurrent = 0;
	static float s_fWaterHeightBegin = 0;
	static float s_fWaterHeightEnd = 0;
	static DWORD s_dwLastHeightChangeTime = CTimer::Instance().GetCurrentMillisecond();
	static DWORD s_dwBlendtime = 300;

	if ((CTimer::Instance().GetCurrentMillisecond() - s_dwLastHeightChangeTime) > s_dwBlendtime)
	{
		s_dwBlendtime = random_range(1000, 3000);

		if (s_fWaterHeightEnd == 0)
			s_fWaterHeightEnd = -random_range(0, 15);
		else
			s_fWaterHeightEnd = 0;

		s_fWaterHeightBegin = s_fWaterHeightCurrent;
		s_dwLastHeightChangeTime = CTimer::Instance().GetCurrentMillisecond();
	}

	s_fWaterHeightCurrent = s_fWaterHeightBegin + (s_fWaterHeightEnd - s_fWaterHeightBegin) * (float) ((CTimer::Instance().GetCurrentMillisecond() - s_dwLastHeightChangeTime) / (float) s_dwBlendtime);
	m_matWorldForCommonUse._43 = s_fWaterHeightCurrent;

	m_matWorldForCommonUse._41 = 0.0f;
	m_matWorldForCommonUse._42 = 0.0f;
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorldForCommonUse);

	float fFogDistance = __GetFogDistance();

	std::vector<std::pair<float, long> >::iterator i;

	for(i = m_PatchVector.begin();i != m_PatchVector.end(); ++i)
	{
		if (i->first<fFogDistance)
			DrawWater(i->second);
	}

	STATEMANAGER.SetTexture(0, NULL);
	STATEMANAGER.SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	for(i = m_PatchVector.begin();i != m_PatchVector.end(); ++i)
	{
		if (i->first>=fFogDistance)
			DrawWater(i->second);
	}

	m_matWorldForCommonUse._43 = 0.0f;

	for (int s = 0; s < 8; ++s)
		STATEMANAGER.SetTexture(s, nullptr);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	//////////////////////////////////////////////////////////////////////////
	// RenderState
#if defined(DIRECTX9)
	STATEMANAGER.RestoreFVF();
#else
	STATEMANAGER.RestoreVertexShader();
#endif
	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE0);
	//STATEMANAGER.RestoreTransform(D3DTS_TEXTURE1);
#if defined(DIRECTX9)
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MINFILTER);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MAGFILTER);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MIPFILTER);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_ADDRESSU);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_ADDRESSV);

	//STATEMANAGER.RestoreSamplerState(1, D3DSAMP_MINFILTER);
	//STATEMANAGER.RestoreSamplerState(1, D3DSAMP_MAGFILTER);
	//STATEMANAGER.RestoreSamplerState(1, D3DSAMP_MIPFILTER);
	//STATEMANAGER.RestoreSamplerState(1, D3DSAMP_ADDRESSU);
	//STATEMANAGER.RestoreSamplerState(1, D3DSAMP_ADDRESSV);
#else
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_MINFILTER);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_MAGFILTER);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_MIPFILTER);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ADDRESSU);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ADDRESSV);
#endif
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS);

	STATEMANAGER.RestoreRenderState(D3DRS_DIFFUSEMATERIALSOURCE);
	STATEMANAGER.RestoreRenderState(D3DRS_COLORVERTEX);
	STATEMANAGER.RestoreRenderState(D3DRS_ZWRITEENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
}
#endif

void CMapOutdoor::DrawWater(long patchnum)
{
	assert(NULL!=m_pTerrainPatchProxyList);
	if (!m_pTerrainPatchProxyList)
		return;

	CTerrainPatchProxy& rkTerrainPatchProxy = m_pTerrainPatchProxyList[patchnum];

	if (!rkTerrainPatchProxy.isUsed())
		return;

	if (!rkTerrainPatchProxy.isWaterExists())
		return;

	CGraphicVertexBuffer* pkVB=rkTerrainPatchProxy.GetWaterVertexBufferPointer();
	if (!pkVB)
		return;

	if (!pkVB->GetD3DVertexBuffer())
		return;

	UINT uPriCount=rkTerrainPatchProxy.GetWaterFaceCount();
	if (!uPriCount)
		return;

	STATEMANAGER.SetStreamSource(0, pkVB->GetD3DVertexBuffer(), sizeof(SWaterVertex));
	STATEMANAGER.DrawPrimitive(D3DPT_TRIANGLELIST, 0, uPriCount);

	ms_faceCount += uPriCount;
}

#if defined(ANK_SYSTEM_GRAPHICS)
// Pixel constants c95-c97 read by ApplyAtmosphere() in the terrain, water and grass shaders.
void CMapOutdoor::__SetAtmosphereShaderConstants()
{
	// Light direction convention used by these shaders: L = -sunPosition points toward the sun.
	D3DXVECTOR3 toSun = -current_v3SunPosition;
	if (D3DXVec3LengthSq(&toSun) < 1e-6f)
		toSun = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVec3Normalize(&toSun, &toSun);

	D3DLIGHT9 light;
	STATEMANAGER.GetLight(0, &light);

	const float sunDir[4] = { toSun.x, toSun.y, toSun.z, 8.0f };
	const float sunColor[4] = { light.Diffuse.r * 0.45f, light.Diffuse.g * 0.40f, light.Diffuse.b * 0.30f, 0.96f };
	const float height[4] = { 1.0f / 4000.0f, Frustum::Instance().characterPosition.z, 0.0f, 0.0f };
	STATEMANAGER.SetPixelShaderConstant(95, sunDir, 1);
	STATEMANAGER.SetPixelShaderConstant(96, sunColor, 1);
	STATEMANAGER.SetPixelShaderConstant(97, height, 1);
}

// Binds the water shader pair, its constants and samplers once for all water patches.
// Must be called after D3DTS_WORLD is set (the per-frame water height lives in m_matWorldForCommonUse).
void CMapOutdoor::__SetWaterShaderState(int iWaterLevel)
{
	STATEMANAGER.SetVertexShader(m_lpShadowMapVertexShaderWater);
	STATEMANAGER.SetPixelShader(m_lpShadowMapPixelShaderWater);

	// Transformations
	D3DXMATRIX worldMat;
	D3DXMatrixTranspose(&worldMat, &m_matWorldForCommonUse);
	STATEMANAGER.SetVertexShaderConstant(20, &worldMat, 4);

	D3DXMATRIX viewProjMat, viewMat, projMat;
	STATEMANAGER.GetTransform(D3DTS_VIEW, &viewMat);
	STATEMANAGER.GetTransform(D3DTS_PROJECTION, &projMat);
	D3DXMatrixMultiplyTranspose(&viewProjMat, &viewMat, &projMat);
	STATEMANAGER.SetVertexShaderConstant(4, &viewProjMat, 4);

	// Light space
	STATEMANAGER.SetVertexShaderConstant(8, &m_MatLightViewProj, 4);
	if (Frustum::Instance().shadowType == 2)
		STATEMANAGER.SetVertexShaderConstant(16, &m_MatLightViewProj2, 4);
	else
		STATEMANAGER.SetVertexShaderConstant(16, &m_MatLightViewProj, 4);

	// Ripples
	const float waveSpeed[4] = { 0.00325f, 0.0f, 0.0f, 0.0f };
	const float waveHeight[4] = { CPythonSystem::Instance().GetAdvancedOption("water_ripple", 1) ? 60.0f : 0.0f, 0.0f, 0.0f, 0.0f };
	const float waveFrequency[4] = { 6.25f, 6.25f, 0.0f, 0.0f };
	const float rippleOrigin[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	STATEMANAGER.SetVertexShaderConstant(31, waveSpeed, 1);
	STATEMANAGER.SetVertexShaderConstant(32, waveHeight, 1);
	STATEMANAGER.SetVertexShaderConstant(33, waveFrequency, 1);
	STATEMANAGER.SetVertexShaderConstant(34, rippleOrigin, 1);

	const float wavesTimer[4] = { (float)CTimer::Instance().GetCurrentMillisecond(), 0.0f, 0.0f, 0.0f };
	STATEMANAGER.SetPixelShaderConstant(73, wavesTimer, 1);
	STATEMANAGER.SetVertexShaderConstant(30, wavesTimer, 1);

	// Lighting
	D3DMATERIAL9 material;
	STATEMANAGER.GetMaterial(&material);
	D3DLIGHT8 light;
	STATEMANAGER.GetLight(0, &light);
	STATEMANAGER.SetPixelShaderConstant(0, &material.Diffuse, 1);
	const float waterSunPosition[4] = { current_v3SunPosition.x, current_v3SunPosition.y, current_v3SunPosition.z / 1.5f, 0.0f };
	STATEMANAGER.SetPixelShaderConstant(1, waterSunPosition, 1);
	STATEMANAGER.SetPixelShaderConstant(2, &light.Diffuse, 1);
	STATEMANAGER.SetPixelShaderConstant(3, &light.Diffuse, 1);

	// Fog settings
	const D3DXVECTOR3 eye = CCameraManager::instance().GetCurrentCamera()->GetEye();
	const float cameraPos[4] = { eye.x, eye.y, eye.z, 1.0f };
	STATEMANAGER.SetPixelShaderConstant(4, cameraPos, 1);
	D3DCOLOR fogColor;
	STATEMANAGER.GetRenderState(D3DRS_FOGCOLOR, &fogColor);
	D3DXCOLOR fogColorX(fogColor);
	STATEMANAGER.SetPixelShaderConstant(5, (float*)fogColorX, 1);
	DWORD dwFogEnable = FALSE;
	STATEMANAGER.GetRenderState(D3DRS_FOGENABLE, &dwFogEnable);
	float fogProps[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	if (dwFogEnable)
	{
		STATEMANAGER.GetRenderState(D3DRS_FOGSTART, (DWORD*)&fogProps[0]);
		STATEMANAGER.GetRenderState(D3DRS_FOGEND, (DWORD*)&fogProps[1]);
	}
	STATEMANAGER.SetPixelShaderConstant(6, fogProps, 1);
	__SetAtmosphereShaderConstants();

	STATEMANAGER.SetPixelShaderConstant(7, &light.Diffuse, 1);

	const float shadowOpacity[4] = { 35.0f, 0.0f, 0.0f, 0.0f };
	STATEMANAGER.SetPixelShaderConstant(10, shadowOpacity, 1);

	const float specularLightEnabled[4] = { (float)Frustum::Instance().realtimeSpecular, 0.0f, 0.0f, 0.0f };
	STATEMANAGER.SetPixelShaderConstant(11, specularLightEnabled, 1);

	const float tint[4] = { 1.0f, 1.0f, 1.0f, 0.0f };
	STATEMANAGER.SetPixelShaderConstant(12, tint, 1);

	const float enableLights[4] = { Frustum::Instance().ambianceEffectsLevel == 1 ? 1.0f : 0.0f, 0.0f, 0.0f, 0.0f };
	STATEMANAGER.SetPixelShaderConstant(13, enableLights, 1);

	const float shiness[4] = { 169.0f, 0.0f, 0.0f, 0.0f };
	const float specularLight[4] = { 50.75f, 50.75f, 50.75f, 0.0f };
	STATEMANAGER.SetPixelShaderConstant(8, shiness, 1);
	STATEMANAGER.SetPixelShaderConstant(9, specularLight, 1);

	const float waterOpacity[4] = { Frustum::Instance().waterOpacity, 0.0f, 0.0f, 0.0f };
	STATEMANAGER.SetPixelShaderConstant(72, waterOpacity, 1);

	// Samplers. The reflection target is only rendered at water level 2; at level 1 the
	// shader falls back to a sky/fog colour instead of sampling a stale or empty target.
	Frustum& rkFrustum = Frustum::Instance();
	const bool bReflection = iWaterLevel == 2 && rkFrustum.m_lpWaterReflectionTexture;
	const bool bNormals = rkFrustum.brown_mud_03_diff_2k && rkFrustum.dry_ground_01_diff_4k;
	const bool bFoam = rkFrustum.foam != nullptr;

	STATEMANAGER.SetTexture(1, m_lpCharacterShadowMapTexture);
	STATEMANAGER.SetTexture(2, m_lpCharacterShadowMapTexture4 ? m_lpCharacterShadowMapTexture4 : m_lpCharacterShadowMapTexture);
	STATEMANAGER.SetTexture(3, bReflection ? rkFrustum.m_lpWaterReflectionTexture : nullptr);
	STATEMANAGER.SetTexture(4, rkFrustum.brown_mud_03_diff_2k);
	STATEMANAGER.SetTexture(5, rkFrustum.dry_ground_01_diff_4k);
	STATEMANAGER.SetTexture(6, rkFrustum.foam);

	const float features[4] = { bReflection ? 1.0f : 0.0f, bNormals ? 1.0f : 0.0f, bFoam ? 1.0f : 0.0f,
		(m_lpCharacterShadowMapTexture ? 1.0f : 0.0f) };
	STATEMANAGER.SetPixelShaderConstant(74, features, 1);

	// The perturbed reflection lookup wanders past the texture edge; clamp it instead of
	// wrapping the opposite side of the image in. Restored in RenderWater.
	STATEMANAGER.SaveSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	STATEMANAGER.SaveSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
}
#endif

