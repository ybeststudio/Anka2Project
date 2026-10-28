#include "StdAfx.h"
#include "../UserInterface/PythonSystem.h"
#include "MapOutdoor.h"
#if defined(ANK_SYSTEM_GRAPHICS)
#include "../EterLib/Camera.h"
#endif

#include "../eterlib/StateManager.h"
#if defined(ANK_SYSTEM_GRAPHICS)
#include "../ScriptLib/StdAfx.h"
#include "../UserInterface/PythonSystem.h"
#include "../UserInterface/PythonBackground.h"
#endif

void CMapOutdoor::__RenderTerrain_RenderHardwareTransformPatch()
{
	DWORD dwFogColor;
	float fFogFarDistance;
	float fFogNearDistance;
	if (mc_pEnvironmentData)
	{
		dwFogColor = mc_pEnvironmentData->FogColor;
		fFogNearDistance = mc_pEnvironmentData->GetFogNearDistance();
		fFogFarDistance = mc_pEnvironmentData->GetFogFarDistance();
	}
	else
	{
		dwFogColor = 0xffffffff;
		fFogNearDistance = 5000.0f;
		fFogFarDistance = 10000.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	// Render State & TextureStageState

	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0x00000000);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	// Effect/particle pass'leri blend modunu (ONE/ONE vb.) geri almiyor; terrain splat'i her zaman normal alpha blend ile ciz.
	STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, dwFogColor);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
#ifdef DIRECTX9
	STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
#else
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
#endif

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
#ifdef DIRECTX9
	STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
#else
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
#endif

#if defined(ANK_SYSTEM_GRAPHICS)
	if (Frustum::Instance().dynamicLight == 3)
	{
		STATEMANAGER.SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		STATEMANAGER.SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}
#endif

#ifdef WORLD_EDITOR
	if (GetAsyncKeyState(VK_CAPITAL))
	{
		CSpeedTreeWrapper::ms_bSelfShadowOn = false;
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_GAUSSIANCUBIC);
	}
	else
	{
		CSpeedTreeWrapper::ms_bSelfShadowOn = true;
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	}
#else
	CSpeedTreeWrapper::ms_bSelfShadowOn = true;
	STATEMANAGER.SetBestFiltering(0);
	STATEMANAGER.SetBestFiltering(1);
#endif

	m_matWorldForCommonUse._41 = 0.0f;
	m_matWorldForCommonUse._42 = 0.0f;
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorldForCommonUse);
#if defined(ANK_SYSTEM_GRAPHICS)
	D3DXMATRIX worldMat;
	D3DXMatrixTranspose(&worldMat, &m_matWorldForCommonUse);
	STATEMANAGER.SetVertexShaderConstant(20, &worldMat, 4);
#endif
	STATEMANAGER.SaveTransform(D3DTS_TEXTURE0, &m_matWorldForCommonUse);
	STATEMANAGER.SaveTransform(D3DTS_TEXTURE1, &m_matWorldForCommonUse);

	// Render State & TextureStageState
	//////////////////////////////////////////////////////////////////////////

#ifdef DIRECTX9
	STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
#else
	STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL);
#endif

	m_iRenderedSplatNumSqSum = 0;
	m_iRenderedPatchNum = 0;
	m_iRenderedSplatNum = 0;
	m_RenderedTextureNumVector.clear();

	std::pair<float, long> fog_far(fFogFarDistance + 1600.0f, 0);
	std::pair<float, long> fog_near(fFogNearDistance - 3200.0f, 0);

	std::vector<std::pair<float, long> >::iterator far_it = std::upper_bound(m_PatchVector.begin(), m_PatchVector.end(), fog_far);
	std::vector<std::pair<float, long> >::iterator near_it = std::upper_bound(m_PatchVector.begin(), m_PatchVector.end(), fog_near);

#ifdef WORLD_EDITOR
	near_it = m_PatchVector.begin();
	far_it = m_PatchVector.end();
#endif

	WORD wPrimitiveCount;
	D3DPRIMITIVETYPE ePrimitiveType;

	BYTE byCUrrentLODLevel = 0;

	float fLODLevel1Distance = __GetNoFogDistance();
	float fLODLevel2Distance = __GetFogDistance();

	SelectIndexBuffer(0, &wPrimitiveCount, &ePrimitiveType);

	DWORD dwFogEnable = STATEMANAGER.GetRenderState(D3DRS_FOGENABLE);
	std::vector<std::pair<float, long> >::iterator it = m_PatchVector.begin();

#ifndef WORLD_EDITOR
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);

	for (; it != near_it; ++it)
	{
		if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
		{
			byCUrrentLODLevel = 1;
			SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
		}
		else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
		{
			byCUrrentLODLevel = 2;
			SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
		}

		__HardwareTransformPatch_RenderPatchSplat(it->second, wPrimitiveCount, ePrimitiveType);
		if (m_iRenderedSplatNum >= m_iSplatLimit)
			break;

		if (m_bDrawWireFrame)
			DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
	}
#endif

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, dwFogEnable);

	if (m_iRenderedSplatNum < m_iSplatLimit)
	{
		for (it = near_it; it != far_it; ++it)
		{
			if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
			{
				byCUrrentLODLevel = 1;
				SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
			}
			else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
			{
				byCUrrentLODLevel = 2;
				SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
			}

			__HardwareTransformPatch_RenderPatchSplat(it->second, wPrimitiveCount, ePrimitiveType);

			if (m_iRenderedSplatNum >= m_iSplatLimit)
				break;

			if (m_bDrawWireFrame)
				DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
		}
	}

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SetTexture(0, NULL);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, FALSE);

	STATEMANAGER.SetTexture(1, NULL);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, FALSE);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	if (m_iRenderedSplatNum < m_iSplatLimit)
	{
		for (it = far_it; it != m_PatchVector.end(); ++it)
		{
			if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
			{
				byCUrrentLODLevel = 1;
				SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
			}
			else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
			{
				byCUrrentLODLevel = 2;
				SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
			}

			__HardwareTransformPatch_RenderPatchNone(it->second, wPrimitiveCount, ePrimitiveType);

			if (m_iRenderedSplatNum >= m_iSplatLimit)
				break;

			if (m_bDrawWireFrame)
				DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
		}
	}

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	// Restore the caller's fog state (as the Original path does) instead of forcing it off,
	// which left every fixed-function draw after terrain unfogged.
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, dwFogEnable);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);

	std::sort(m_RenderedTextureNumVector.begin(), m_RenderedTextureNumVector.end());

	//////////////////////////////////////////////////////////////////////////
	// Render State & TextureStageState

	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE0);
	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE1);

	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS);

	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);
	STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);

	// Render State & TextureStageState
	//////////////////////////////////////////////////////////////////////////
}

void CMapOutdoor::__RenderTerrain_RenderHardwareTransformPatch2()
{
	m_matWorldForCommonUse._41 = 0.0f;
	m_matWorldForCommonUse._42 = 0.0f;
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorldForCommonUse);
	D3DXMATRIX worldMat;
	D3DXMatrixTranspose(&worldMat, &m_matWorldForCommonUse);
	STATEMANAGER.SetVertexShaderConstant(20, &worldMat, 4);

	std::pair<float, long> fog_near(400000.0f, 0);

	std::vector<std::pair<float, long> >::iterator near_it = std::upper_bound(m_PatchVector.begin(), m_PatchVector.end(), fog_near);


	WORD wPrimitiveCount;
	D3DPRIMITIVETYPE ePrimitiveType;

	SelectIndexBuffer(0, &wPrimitiveCount, &ePrimitiveType);

	DWORD dwFogEnable = STATEMANAGER.GetRenderState(D3DRS_FOGENABLE);
	std::vector<std::pair<float, long> >::iterator it = m_PatchVector.begin();

#ifndef WORLD_EDITOR
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);

	for (; it != near_it; ++it)
	{
		__HardwareTransformPatch_RenderPatchSplat2(it->second, wPrimitiveCount, ePrimitiveType);
	}
#endif

	std::sort(m_RenderedTextureNumVector.begin(), m_RenderedTextureNumVector.end());
}

void CMapOutdoor::UpdateCharacterMovement()
{
	static D3DXVECTOR3 previousPosition = Frustum::Instance().characterPosition;
	D3DXVECTOR3 currentPosition = Frustum::Instance().characterPosition;

	const float epsilon = 0.0001f;
	bool isMoving =
		(fabs(currentPosition.x - previousPosition.x) > epsilon) ||
		(fabs(currentPosition.y - previousPosition.y) > epsilon) ||
		(fabs(currentPosition.z - previousPosition.z) > epsilon);

	float characterMoved = isMoving ? 1.0f : 0.0f;

	STATEMANAGER.SetPixelShaderConstant(93, &previousPosition, 1);
	STATEMANAGER.SetPixelShaderConstant(94, &characterMoved, 1);

	if (isMoving)
	{
		if (Frustum::Instance().frameCounter % 8 == 0)
			previousPosition = currentPosition;
	}
}

float time3 = 0.0f;
float timeAuraEnabled = 0.0f;
float wetness = 1.0f;
float checkMap = 0.0f;
float shinessRain = 0.0f;
float puddleThreshold = 0.0f;
void CMapOutdoor::__HardwareTransformPatch_RenderPatchSplat(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
{
	assert(NULL != m_pTerrainPatchProxyList && "__HardwareTransformPatch_RenderPatchSplat");
	CTerrainPatchProxy* pTerrainPatchProxy = &m_pTerrainPatchProxyList[patchnum];

	if (!pTerrainPatchProxy->isUsed())
		return;

	long sPatchNum = pTerrainPatchProxy->GetPatchNum();
	if (sPatchNum < 0)
		return;

	BYTE ucTerrainNum = pTerrainPatchProxy->GetTerrainNum();
	if (0xFF == ucTerrainNum)
		return;

	CTerrain* pTerrain;
	if (!GetTerrainPointer(ucTerrainNum, &pTerrain))
		return;

	DWORD dwFogColor;
	if (mc_pEnvironmentData)
		dwFogColor = mc_pEnvironmentData->FogColor;
	else
		dwFogColor = 0xffffffff;

	WORD wCoordX, wCoordY;
	pTerrain->GetCoordinate(&wCoordX, &wCoordY);

	TTerrainSplatPatch& rTerrainSplatPatch = pTerrain->GetTerrainSplatPatch();

	D3DXMATRIX matTexTransform, matSplatAlphaTexTransform, matSplatColorTexTransform;
	m_matWorldForCommonUse._41 = -(float)(wCoordX * CTerrainImpl::TERRAIN_XSIZE);
	m_matWorldForCommonUse._42 = (float)(wCoordY * CTerrainImpl::TERRAIN_YSIZE);
	D3DXMatrixMultiply(&matTexTransform, &m_matViewInverse, &m_matWorldForCommonUse);
	D3DXMatrixMultiply(&matSplatAlphaTexTransform, &matTexTransform, &m_matSplatAlpha);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &matSplatAlphaTexTransform);

	D3DXMATRIX matTexTransformAlpha;
	D3DXMatrixMultiplyTranspose(&matTexTransformAlpha, &m_matWorldForCommonUse, &m_matSplatAlpha);
	STATEMANAGER.SetVertexShaderConstant(28, &matTexTransformAlpha, 4);

	D3DXMATRIX matTiling;
	D3DXMatrixScaling(&matTiling, 1.0f / 640.0f, -1.0f / 640.0f, 0.0f);
	matTiling._41 = 0.0f;
	matTiling._42 = 0.0f;

	D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &matTiling);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);

	CGraphicVertexBuffer* pkVB = pTerrainPatchProxy->HardwareTransformPatch_GetVertexBufferPtr();
	if (!pkVB)
		return;

	STATEMANAGER.SetStreamSource(0, pkVB->GetD3DVertexBuffer(), m_iPatchTerrainVertexSize);

	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);

	//TraceError("Map: %s", Frustum::Instance().currentMapNameSave.c_str());

	//JTX UPDATE V1.0.4
	// Transformations
	if (Frustum::Instance().dynamicLight == 3 && std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_n_flame_01") != 0)
	{
		D3DXMATRIX worldMat;
		STATEMANAGER.GetTransform(D3DTS_WORLD, &worldMat);
		D3DXMatrixTranspose(&worldMat, &worldMat);
		STATEMANAGER.SetVertexShaderConstant(20, &worldMat, 4);
		// View-Projection
		D3DXMATRIX viewProjMat, viewMat, projMat;
		STATEMANAGER.GetTransform(D3DTS_VIEW, &viewMat);
		STATEMANAGER.GetTransform(D3DTS_PROJECTION, &projMat);
		D3DXMatrixMultiplyTranspose(&viewProjMat, &viewMat, &projMat);
		STATEMANAGER.SetVertexShaderConstant(4, &viewProjMat, 4);
	}

	int iPrevRenderedSplatNum=m_iRenderedSplatNum;

#ifdef WORLD_EDITOR
#else
	// Go through STATEMANAGER (not the raw device) so the cache knows about it and the bias
	// can be reset at the end of this patch.
	float fDepthBias = 0.00001f;
	STATEMANAGER.SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)(&fDepthBias));

	if (Frustum::Instance().dynamicLight == 3 && std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_n_flame_01") != 0)
	{
		STATEMANAGER.SetVertexShader(m_lpVertexShaderTerrainBase);
		STATEMANAGER.SetPixelShader(m_lpPixelShaderTerrainBase);

		// Shadow receiving: the cascades are rendered every frame (PythonApplication shadow
		// passes) but terrain never sampled them. Only R32F maps hold usable depth.
		Frustum& rkFrustum = Frustum::Instance();
		auto IsDepthMap = [](LPDIRECT3DTEXTURE9 pTex)
		{
			D3DSURFACE_DESC desc;
			return pTex && SUCCEEDED(pTex->GetLevelDesc(0, &desc)) && desc.Format == D3DFMT_R32F;
		};
		const bool bShadows = rkFrustum.graphicsLevel > 0 && !rkFrustum.isIndoor && rkFrustum.shadowType != 3 &&
			IsDepthMap(m_lpCharacterShadowMapTexture);
		const bool bCascade2 = bShadows && rkFrustum.shadowType == 2 && IsDepthMap(m_lpCharacterShadowMapTexture4);

		STATEMANAGER.SetTexture(3, bShadows ? m_lpCharacterShadowMapTexture : nullptr);
		STATEMANAGER.SetTexture(9, bCascade2 ? m_lpCharacterShadowMapTexture4 : nullptr);
		// Point-sampled, clamped depth. Set on the device directly: the StateManager sampler
		// cache only covers stages 0-7 (sampler 3 is restored to linear after the patch).
		for (DWORD s : { 3UL, 9UL })
		{
			ms_lpd3dDevice->SetSamplerState(s, D3DSAMP_MINFILTER, D3DTEXF_POINT);
			ms_lpd3dDevice->SetSamplerState(s, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			ms_lpd3dDevice->SetSamplerState(s, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
			ms_lpd3dDevice->SetSamplerState(s, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			ms_lpd3dDevice->SetSamplerState(s, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		}
		const float shadowParams[4] = { m_wShadowMapSize ? 1.0f / (float)m_wShadowMapSize : 1.0f / 2048.0f,
			bCascade2 ? 1.0f : 0.0f, 0.55f, bShadows ? 1.0f : 0.0f };
		STATEMANAGER.SetPixelShaderConstant(20, shadowParams, 1);
	}

	bool isFirst = true;
	for (DWORD j = 1; j < pTerrain->GetNumTextures(); ++j)
	{
		TTerainSplat& rSplat = rTerrainSplatPatch.Splats[j];

		if (!rSplat.Active)
			continue;

		if (rTerrainSplatPatch.PatchTileCount[sPatchNum][j] == 0)
			continue;

		const TTerrainTexture& rTexture = m_TextureSet.GetTexture(j);

		if (Frustum::Instance().dynamicLight != 3)
		{
			D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &rTexture.m_matTransform);
			STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);
			if (isFirst)
			{
				STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
				STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
				STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
				STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
				STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				isFirst = false;
			}
			else
			{
				STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
				STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
				STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
			}
		}
		else
		{
			D3DXMATRIX matTexTransform;
			D3DXMatrixTranspose(&matTexTransform, &rTexture.m_matTransform);
			STATEMANAGER.SetVertexShaderConstant(24, &matTexTransform, 4);
			float alphaOverride[4] = { 0.0, 0.0, 0.0, 0.0 };

			if (isFirst)
			{
				alphaOverride[0] = 1.0;
				isFirst = false;
			}
			STATEMANAGER.SetPixelShaderConstant(15, alphaOverride, 1);

			// Light space
			STATEMANAGER.SetVertexShaderConstant(8, &m_MatLightViewProj, 4);

			if (Frustum::Instance().shadowType == 2)
			{
				STATEMANAGER.SetVertexShaderConstant(16, &m_MatLightViewProj2, 4);
			}

			D3DMATERIAL9 material;
			STATEMANAGER.GetMaterial(&material);
			D3DLIGHT9 light;
			STATEMANAGER.GetLight(0, &light);
			STATEMANAGER.SetPixelShaderConstant(0, &material.Diffuse, 1);
			D3DXVECTOR3 sunPosition = current_v3SunPosition;
			sunPosition.z = sunPosition.z / 1.5f;
			STATEMANAGER.SetPixelShaderConstant(1, &sunPosition, 1);
			if (Frustum::Instance().isThunderTime)
			{
				D3DCOLORVALUE thunderLight = { 1.6f, 1.7f, 2.0f, 1.0f };
				STATEMANAGER.SetPixelShaderConstant(2, &thunderLight, 1);
				STATEMANAGER.SetPixelShaderConstant(3, &thunderLight, 1);
			}
			else
			{
				STATEMANAGER.SetPixelShaderConstant(2, &light.Diffuse, 1);
				STATEMANAGER.SetPixelShaderConstant(3, &light.Diffuse, 1);
			}
			// Fog settings
			D3DVECTOR cameraPos = CCameraManager::instance().GetCurrentCamera()->GetEye();
			STATEMANAGER.SetPixelShaderConstant(4, &cameraPos, 1);
			D3DXCOLOR fogColorX = mc_pEnvironmentData ? mc_pEnvironmentData->FogColor : D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			STATEMANAGER.SetPixelShaderConstant(5, (float*)fogColorX, 1);
			D3DVECTOR fogProps;
			STATEMANAGER.GetRenderState(D3DRS_FOGSTART, (DWORD*)&fogProps.x);
			STATEMANAGER.GetRenderState(D3DRS_FOGEND, (DWORD*)&fogProps.y);
			if (mc_pEnvironmentData && mc_pEnvironmentData->bFogEnable)
			{
				fogProps.x = mc_pEnvironmentData->GetFogNearDistance();
				fogProps.y = mc_pEnvironmentData->GetFogFarDistance();
				if (fogProps.y <= fogProps.x)
					fogProps.y = fogProps.x + 1000.0f;
			}
			else
			{
				fogProps.x = 900000.0f;
				fogProps.y = 900000.0f;
			}
			STATEMANAGER.SetPixelShaderConstant(6, &fogProps, 1);
			__SetAtmosphereShaderConstants();

			STATEMANAGER.SetPixelShaderConstant(7, &material.Diffuse, 1);

			float shadowOpacity = 70.0f;
			STATEMANAGER.SetPixelShaderConstant(10, (int*)&shadowOpacity, 1);

			int specularLightEnabled = Frustum::Instance().realtimeSpecular;
			STATEMANAGER.SetPixelShaderConstant(11, &specularLightEnabled, 1);

			float shiness;
			D3DXVECTOR3 specularLight;
			float reducePointLightIntensity = 15.0f;

			if (Frustum::Instance().selectRainFrustun == 0)
			{
				shiness = 2.0f;
				specularLight = { 0.3f, 0.3f, 0.3f };
				STATEMANAGER.SetPixelShaderConstant(8, &shiness, 1);
			}
			else
			{
				if (Frustum::Instance().isThunderTime == true)
				{

					shinessRain += 0.1f;

					if (shinessRain > 5.0f)
						shinessRain = 5.0f;

					specularLight = { 1.0f, 1.0f, 1.0f };
					STATEMANAGER.SetPixelShaderConstant(8, &shinessRain, 1);
				}
				else
				{
					shiness = 8.0f;
					specularLight = { 20.5f, 20.5f, 20.5f };
					STATEMANAGER.SetPixelShaderConstant(8, &shiness, 1);
					shinessRain = 0.0f;
				}
			}

			STATEMANAGER.SetPixelShaderConstant(9, &specularLight, 1);
			STATEMANAGER.SetPixelShaderConstant(12, &time3, 1);

			float enableLights = 1.0f;
			STATEMANAGER.SetPixelShaderConstant(13, &enableLights, 1);

			float terrainLightMultiply;
			terrainLightMultiply = 1.75f;


			STATEMANAGER.SetPixelShaderConstant(14, &terrainLightMultiply, 1);
			if (!CPythonSystem::Instance().GetAdvancedOption("local_light",1)) reducePointLightIntensity = 0.0f;
			STATEMANAGER.SetPixelShaderConstant(15, &reducePointLightIntensity, 1);
			STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
			STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);

			STATEMANAGER.SetTexture(5, Frustum::Instance().brown_mud_03_diff_2k);
			STATEMANAGER.SetTexture(6, Frustum::Instance().dry_ground_01_diff_4k);
			STATEMANAGER.SetTexture(7, Frustum::Instance().m_lpPuddleReflectionTexture ? Frustum::Instance().m_lpPuddleReflectionTexture : Frustum::Instance().m_lpWaterReflectionTexture);
			STATEMANAGER.SetTexture(8, Frustum::Instance().foam);

			float uv = rTexture.UScale / 5;
			float textureSpecularPower = 3.0f;
			float isIceTerrain = 0.0f;

			if (rTexture.stFilename.find("tile01") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapTile01);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapTile01);
			}
			else if (rTexture.stFilename.find("sand02") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapRacketDirt);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapRacketDirt);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("snow_01_diff_4k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapSnow);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapSnow);
				textureSpecularPower = 1.1f;
			}
			else if (rTexture.stFilename.find("snow_02") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapSnow1);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapSnow1);
				textureSpecularPower = 1.1f;
			}
			else if (rTexture.stFilename.find("snow1") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapSnow2);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapSnow2);
				textureSpecularPower = 0.6f;
			}
			else if (rTexture.stFilename.find("snow2") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapSnow3);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapSnow3);
				textureSpecularPower = 0.6f;
			}
			else if (rTexture.stFilename.find("snow3") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapSnow4);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapSnow4);
				textureSpecularPower = 0.6f;
			}
			else if (rTexture.stFilename.find("ice") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapSnowIce);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapSnowIce);
				textureSpecularPower = 4.0f;
				isIceTerrain = 1.0f;
			}
			else if (rTexture.stFilename.find("beach") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapBitch);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapBitch);
				uv = 3.0f;
			}
			else if (rTexture.stFilename.find("field") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapRacketDirt);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapRacketDirt);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("sand_01_diff_4k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapSand03);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapSand03);
				textureSpecularPower = 2.75f;
			}
			else if (rTexture.stFilename.find("dry_ground_rocks") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapDryGroundRock);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapDryGroundRock);
				textureSpecularPower = 1.75f;
			}
			else if (rTexture.stFilename.find("cobblestone_color") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapCobblestoneTemple1);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapCobblestoneTemple1);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("cobblestone_floor_13") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapCobblestoneTemple2);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapCobblestoneTemple2);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("aerial_grass_rock_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapAerialGrass);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapAerialGrass);
				textureSpecularPower = 3.5f;
			}
			//else if (rTexture.stFilename.find("stone") != std::string::npos)
			//{
			//	STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapStone);
			//}
			else if (rTexture.stFilename.find("forest_leaves_03_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().forest_leaves_03_diff_1k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().forest_leaves_03_diff_1k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("dirt_aerial_02_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().dirt_aerial_02_diff_1k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().dirt_aerial_02_diff_1k);
			}
			else if (rTexture.stFilename.find("mud_cracked_dry_riverbed_002_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapSand03);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapSand03);
				textureSpecularPower = 2.0f;
			}
			else if (rTexture.stFilename.find("rocky_trail_02_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().rocky_trail_02_diff_1k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().rocky_trail_02_diff_1k);
			}
			else if (rTexture.stFilename.find("rock_wall_02_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().rock_wall_02_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().rock_wall_02_diff_2k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("mud_forest_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().mud_forest_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().mud_forest_diff_2k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("grass_map1") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().grass_map1);
				STATEMANAGER.SetTexture(4, Frustum::Instance().grass_map1);
				textureSpecularPower = 1.3f;
			}
			else if (rTexture.stFilename.find("mossy_rock_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().mossy_rock_diff_1k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().mossy_rock_diff_1k);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("leafy_grass_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapGrass);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapGrass);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("brown_mud_leaves_01_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().brown_mud_leaves_01_diff_1k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().brown_mud_leaves_01_diff_1k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("raked_dirt_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapRacketDirt);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapRacketDirt);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("cobblestone_large_01_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().cobblestone_large_01_diff_1k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().cobblestone_large_01_diff_1k);
				textureSpecularPower = 1.75f;
			}
			else if (rTexture.stFilename.find("cobblestone_05_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().cobblestone_05_diff_1k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().cobblestone_05_diff_1k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("playground_sand_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().playground_sand_diff_1k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().playground_sand_diff_1k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("monastery_stone_floor_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().monastery_stone_floor_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().monastery_stone_floor_diff_2k);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("cobblestone_floor_01_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().cobblestone_floor_01_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().cobblestone_floor_01_diff_2k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("cobblestone_floor_02_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().cobblestone_floor_02_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().cobblestone_floor_02_diff_2k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("concrete_rock_path_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().concrete_rock_path_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().concrete_rock_path_diff_2k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("dirt_floor_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().dirt_floor_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().dirt_floor_diff_2k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("forest_floor_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().forest_floor_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().forest_floor_diff_2k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("forrest_ground_03_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().forrest_ground_03_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().forrest_ground_03_diff_2k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("gray_rocks_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().gray_rocks_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().gray_rocks_diff_2k);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("grey_stone_path_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().grey_stone_path_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().grey_stone_path_diff_2k);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("leaves_forest_ground_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().leaves_forest_ground_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().leaves_forest_ground_diff_2k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("rock_06_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().rock_06_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().rock_06_diff_2k);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("rock_boulder_cracked_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().rock_boulder_cracked_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().rock_boulder_cracked_diff_2k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("rock_face_03_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().rock_face_03_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().rock_face_03_diff_2k);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("rustic_stone_wall_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().rustic_stone_wall_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().rustic_stone_wall_diff_2k);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("sand_02_diff_2k_wet") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().sand_02_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().sand_02_diff_2k);
				textureSpecularPower = 4.0f;
			}
			else if (rTexture.stFilename.find("sand_02_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().sand_02_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().sand_02_diff_2k);
				textureSpecularPower = 4.0f;
			}
			else if (rTexture.stFilename.find("ground_0019_color_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().ground_0019_color_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().waterLeavesSpecularTexture);
				textureSpecularPower = 7.0f;
			}
			else if (rTexture.stFilename.find("ground_0007_color_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().ground_0007_color_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().waterSandSpecularTexture);
				textureSpecularPower = 3.9f;
			}
			else if (rTexture.stFilename.find("sand_dunes") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().sand_dunes);
				STATEMANAGER.SetTexture(4, Frustum::Instance().sand_dunes);
				textureSpecularPower = 0.9f;
			}
			else if (rTexture.stFilename.find("sandstone_cracks_diff_4k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().sandstone_cracks_diff_4k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().sandstone_cracks_diff_4k);
				textureSpecularPower = 0.9f;
			}
			else if (rTexture.stFilename.find("dry_ground_01_diff_4k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().dry_ground_01_diff_4k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().dry_ground_01_diff_4k);
				textureSpecularPower = 1.9f;
			}
			else if (rTexture.stFilename.find("ice_0002_color_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().ice_0002_color_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().ice_0002_color_2k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("ground_0030_color_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().ground_0030_color_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().iceSpecularTexture);
				textureSpecularPower = 2.2f;
			}
			else if (rTexture.stFilename.find("ground_0031_color_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().ground_0031_color_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().ground_0031_color_2k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("rock_0008_color_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().rock_0008_color_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().rock_0008_color_2k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("aerial_rocks_04_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().aerial_rocks_04_diff_1k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().aerial_rocks_04_diff_1k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("coast_sand_rocks_02_diff_1k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().coast_sand_rocks_02_diff_1k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().coast_sand_rocks_02_diff_1k);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("brown_mud_03_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().brown_mud_03_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().brown_mud_03_diff_2k);
				textureSpecularPower = 4.5f;
			}
			else if (rTexture.stFilename.find("grassy_cobblestone_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().grassy_cobblestone_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().grassy_cobblestone_diff_2k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("slate_driveway_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().slate_driveway_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().slate_driveway_diff_2k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("cracked_red_ground_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().cracked_red_ground_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().cracked_red_ground_diff_2k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("concrete_floor_painted_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().concrete_floor_painted_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().concrete_floor_painted_diff_2k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("burned_ground_01_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().burned_ground_01_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().burned_ground_01_diff_2k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("brown_mud_dry_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().brown_mud_dry_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().brown_mud_dry_diff_2k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("brown_mud_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().brown_mud_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().brown_mud_diff_2k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("brown_mud_02_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().brown_mud_02_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().brown_mud_02_diff_2k);
				textureSpecularPower = 2.5f;
			}
			else if (rTexture.stFilename.find("lichen_rock_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().lichen_rock_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().lichen_rock_diff_2k);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("lava1") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().lava1);
				STATEMANAGER.SetTexture(4, Frustum::Instance().lava1);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("lava2") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().lava2);
				STATEMANAGER.SetTexture(4, Frustum::Instance().lava2);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("gravel014_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Gravel014_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Gravel014_2K);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("ground035_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground035_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground035_2K);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("ground033_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground033_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground033_2K);
				textureSpecularPower = 0.85f;
			}
			else if (rTexture.stFilename.find("ground044_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground044_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground044_2K);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("ground049b_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground049B_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground049B_2K);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("ground050_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground050_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().sandSpecularTexture);
				textureSpecularPower = 2.0f;
			}
			else if (rTexture.stFilename.find("ground051_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground051_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground051_2K);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("ground054_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground054_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground054_2K);
				textureSpecularPower = 0.8f;
			}
			else if (rTexture.stFilename.find("ground059_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground059_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground059_2K);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("ground071_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground071_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground071_2K);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("rock029_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Rock029_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Rock029_2K);
				textureSpecularPower = 3.5f;
			}
			else if (rTexture.stFilename.find("ground013") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().ground013);
				STATEMANAGER.SetTexture(4, Frustum::Instance().ground013);
				textureSpecularPower = 1.5f;
			}
			else if (rTexture.stFilename.find("ground019") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().ground019);
				STATEMANAGER.SetTexture(4, Frustum::Instance().ground019);
				textureSpecularPower = 2.0f;
			}
			else if (rTexture.stFilename.find("ground030") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().ground030);
				STATEMANAGER.SetTexture(4, Frustum::Instance().ground030);
				textureSpecularPower = 0.3f;
			}
			else if (rTexture.stFilename.find("ground037") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().ground037);
				STATEMANAGER.SetTexture(4, Frustum::Instance().ground037);
				textureSpecularPower = 0.9f;
			}
			else if (rTexture.stFilename.find("ground069") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().ground069);
				STATEMANAGER.SetTexture(4, Frustum::Instance().ground069);
				textureSpecularPower = 0.8f;
			}
			else if (rTexture.stFilename.find("pavingstones009") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().pavingstones009);
				STATEMANAGER.SetTexture(4, Frustum::Instance().pavingstones009);
				textureSpecularPower = 4.0f;
			}
			else if (rTexture.stFilename.find("rock028") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().rock028);
				STATEMANAGER.SetTexture(4, Frustum::Instance().rock028);
				textureSpecularPower = 3.1f;
			}
			else if (rTexture.stFilename.find("rock051") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().rock051);
				STATEMANAGER.SetTexture(4, Frustum::Instance().rock051);
				textureSpecularPower = 3.1f;
			}
			else if (rTexture.stFilename.find("forest_leaves_02_diffuse_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().forest_leaves_02_diffuse_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().forest_leaves_02_diffuse_2k);
				textureSpecularPower = 1.1f;
			}
			else if (rTexture.stFilename.find("forest_leaves_04_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().forest_leaves_04_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().forest_leaves_04_diff_2k);
				textureSpecularPower = 1.1f;
			}
			else if (rTexture.stFilename.find("forrest_ground_01_diff_2k") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().forrest_ground_01_diff_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().forrest_ground_01_diff_2k);
				textureSpecularPower = 1.1f;
			}
			else if (rTexture.stFilename.find("ground018") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground018_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground018_2K);
				textureSpecularPower = 1.1f;
			}
			else if (rTexture.stFilename.find("ground023") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground023_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground023_2K);
				textureSpecularPower = 2.1f;
			}
			else if (rTexture.stFilename.find("ground024") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground024_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground024_2K);
				textureSpecularPower = 1.7f;
			}
			else if (rTexture.stFilename.find("ground038") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground038_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground038_2K);
				textureSpecularPower = 1.1f;
			}
			else if (rTexture.stFilename.find("ground040") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground040_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground040_2K);
				textureSpecularPower = 1.1f;
			}
			else if (rTexture.stFilename.find("ground042") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground042_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground042_2K);
				textureSpecularPower = 1.1f;
			}
			else if (rTexture.stFilename.find("ground067") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground067_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground067_2K);
				textureSpecularPower = 0.9f;
			}
			else if (rTexture.stFilename.find("ground068") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().Ground068_2K);
				STATEMANAGER.SetTexture(4, Frustum::Instance().Ground068_2K);
				textureSpecularPower = 1.6f;
			}
			else if (rTexture.stFilename.find("sand") != std::string::npos)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapSand01);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapSand01);
				textureSpecularPower = 1.2f;
			}
			else if (rTexture.stFilename.find("grass") != std::string::npos) //disable grass NM
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().grass_2k);
				STATEMANAGER.SetTexture(4, Frustum::Instance().grass_2k);
				textureSpecularPower = 0.2f;
			}
			else
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().normalMapRacketDirt);
				STATEMANAGER.SetTexture(4, Frustum::Instance().normalMapRacketDirt);
				textureSpecularPower = 0.5f;
			}

			if (Frustum::Instance().terrainDepthLevel == 0)
			{
				STATEMANAGER.SetTexture(2, Frustum::Instance().emptyNormalMap);
				STATEMANAGER.SetTexture(4, Frustum::Instance().emptyNormalMap);
				textureSpecularPower = 0.25f;
			}

			STATEMANAGER.SetPixelShaderConstant(17, &uv, 1);


			textureSpecularPower = textureSpecularPower * 6;


			STATEMANAGER.SetPixelShaderConstant(18, &textureSpecularPower, 1);
			STATEMANAGER.SetPixelShaderConstant(19, &isIceTerrain, 1);

			float isNightEnvironment = Frustum::Instance().isNightEnvironment;
			STATEMANAGER.SetPixelShaderConstant(89, &isNightEnvironment, 1);

			Frustum::Instance().kingdomMapSelector = 0.0f;

			if (std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_a1") == 0)
				Frustum::Instance().kingdomMapSelector = 1.0f;
			else if (std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_b1") == 0)
				Frustum::Instance().kingdomMapSelector = 2.0f;
			else if (std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_c1") == 0)
				Frustum::Instance().kingdomMapSelector = 3.0f;


			STATEMANAGER.SetPixelShaderConstant(86, &Frustum::Instance().characterPosition, 1);
			float fPuddleThreshold = CPythonSystem::Instance().GetAdvancedOption("wet_ground",1) ? Frustum::Instance().puddleThreshold : 2.0f;
			if (rTexture.stFilename.find("tile") != std::string::npos ||
				rTexture.stFilename.find("stone") != std::string::npos ||
				rTexture.stFilename.find("cobblestone") != std::string::npos ||
				rTexture.stFilename.find("paving") != std::string::npos ||
				rTexture.stFilename.find("bridge") != std::string::npos ||
				rTexture.stFilename.find("wood") != std::string::npos ||
				rTexture.stFilename.find("plank") != std::string::npos)
			{
				fPuddleThreshold = 2.0f;
			}
			STATEMANAGER.SetPixelShaderConstant(87, &fPuddleThreshold, 1);

			STATEMANAGER.SetPixelShaderConstant(88, &Frustum::Instance().kingdomMapSelector, 1);

			float isStormEnvironment = Frustum::Instance().isStormEnvironment;
			STATEMANAGER.SetPixelShaderConstant(90, &isStormEnvironment, 1);

			STATEMANAGER.SetPixelShaderConstant(10, &Frustum::Instance().terrainLightPowerEnvBased, 1);

			STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
			// ----- */

		}

		std::vector<int>::iterator aIterator = std::find(m_RenderedTextureNumVector.begin(), m_RenderedTextureNumVector.end(), (int)j);
		if (aIterator == m_RenderedTextureNumVector.end())
			m_RenderedTextureNumVector.push_back(j);
		++m_iRenderedSplatNum;
		if (m_iRenderedSplatNum >= m_iSplatLimit)
			break;
	}

	if (Frustum::Instance().dynamicLight == 3 && std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "metin2_map_n_flame_01") != 0)
	{
		STATEMANAGER.SetVertexShader(nullptr);
		STATEMANAGER.SetPixelShader(nullptr);
		for (int s = 0; s < 10; ++s)
			STATEMANAGER.SetTexture(s, nullptr);

		// Sampler 3 is shared with the water reflection, which expects linear filtering and
		// saves/restores its own address mode.
		ms_lpd3dDevice->SetSamplerState(3, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		ms_lpd3dDevice->SetSamplerState(3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		ms_lpd3dDevice->SetSamplerState(3, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		ms_lpd3dDevice->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		ms_lpd3dDevice->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}

#endif // END_IFDEF_WORLD_EDITOR

	// ???

	fDepthBias = -0.00003f;
	STATEMANAGER.SetRenderState(D3DRS_DEPTHBIAS, *(DWORD*)(&fDepthBias));

	time3 += 0.1f;

	if (Frustum::Instance().shadowType != 3 && Frustum::Instance().isIndoor == false && Frustum::Instance().dynamicLight != 3)
	{
		//ms_lpd3dDevice->SetRenderState(D3DRS_FOGENABLE, 1); // replaced with fog in the shader
		STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);

		STATEMANAGER.SetRenderState(D3DRS_FOGCOLOR, 0xFFFFFFFF);
		STATEMANAGER.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		STATEMANAGER.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);

		//STATEMANAGER.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		//STATEMANAGER.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
		//STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, 0);

		D3DXMATRIX matShadowTexTransform;
		D3DXMatrixMultiply(&matShadowTexTransform, &matTexTransform, &m_matStaticShadow);

		//STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &m_matDynamicShadow);

		STATEMANAGER.SetTexture(0, m_lpCharacterShadowMapTexture);

		if (Frustum::Instance().isIndoor == false && Frustum::Instance().shadowType != 3)
		{
#if defined(ANK_SYSTEM_GRAPHICS)
			if (Frustum::Instance().shadowType == 2)
			{
				STATEMANAGER.SetVertexShader(m_lpShadowMapVertexShaderTerrain);
				STATEMANAGER.SetPixelShader(m_lpShadowMapPixelShaderTerrain);
			}

			if (Frustum::Instance().shadowType == 1)
			{
				STATEMANAGER.SetVertexShader(m_lpShadowMapVertexShaderTerrain2);
				STATEMANAGER.SetPixelShader(m_lpShadowMapPixelShaderTerrain2);
			}

			if (Frustum::Instance().shadowType == 0)
			{
				STATEMANAGER.SetVertexShader(m_lpShadowMapVertexShaderTerrain3);
				STATEMANAGER.SetPixelShader(m_lpShadowMapPixelShaderTerrain3);
			}
			// Transformations
#endif

			D3DXMATRIX worldMat;
			STATEMANAGER.GetTransform(D3DTS_WORLD, &worldMat);
			D3DXMatrixTranspose(&worldMat, &worldMat);
			STATEMANAGER.SetVertexShaderConstant(20, &worldMat, 4);
			// View-Projection
			D3DXMATRIX viewProjMat, viewMat, projMat;
			STATEMANAGER.GetTransform(D3DTS_VIEW, &viewMat);
			STATEMANAGER.GetTransform(D3DTS_PROJECTION, &projMat);
			D3DXMatrixMultiplyTranspose(&viewProjMat, &viewMat, &projMat);
			STATEMANAGER.SetVertexShaderConstant(4, &viewProjMat, 4);
			// Light space
			STATEMANAGER.SetVertexShaderConstant(8, &m_MatLightViewProj, 4);

#if defined(ANK_SYSTEM_GRAPHICS)
			if (Frustum::Instance().shadowType == 2)
			{
				STATEMANAGER.SetVertexShaderConstant(16, &m_MatLightViewProj2, 4);
			}
#endif

			D3DMATERIAL9 material;
			STATEMANAGER.GetMaterial(&material);
			D3DLIGHT8 light;
			STATEMANAGER.GetLight(0, &light);
			STATEMANAGER.SetPixelShaderConstant(0, &material.Diffuse, 1);
			STATEMANAGER.SetPixelShaderConstant(1, &current_v3SunPosition, 1);
			if (Frustum::Instance().isThunderTime)
			{
				D3DCOLORVALUE thunderLight = { 1.6f, 1.7f, 2.0f, 1.0f };
				STATEMANAGER.SetPixelShaderConstant(2, &thunderLight, 1);
				STATEMANAGER.SetPixelShaderConstant(3, &thunderLight, 1);
			}
			else
			{
				STATEMANAGER.SetPixelShaderConstant(2, &light.Diffuse, 1);
				STATEMANAGER.SetPixelShaderConstant(3, &light.Diffuse, 1);
			}
			// Fog settings
			D3DVECTOR cameraPos = CCameraManager::instance().GetCurrentCamera()->GetEye();
			STATEMANAGER.SetPixelShaderConstant(4, &cameraPos, 1);
			D3DXCOLOR fogColorX = mc_pEnvironmentData ? mc_pEnvironmentData->FogColor : D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			STATEMANAGER.SetPixelShaderConstant(5, (float*)fogColorX, 1);
			D3DVECTOR fogProps;
			STATEMANAGER.GetRenderState(D3DRS_FOGSTART, (DWORD*)&fogProps.x);
			STATEMANAGER.GetRenderState(D3DRS_FOGEND, (DWORD*)&fogProps.y);
			if (mc_pEnvironmentData && mc_pEnvironmentData->bFogEnable)
			{
				fogProps.x = mc_pEnvironmentData->GetFogNearDistance();
				fogProps.y = mc_pEnvironmentData->GetFogFarDistance();
				if (fogProps.y <= fogProps.x)
					fogProps.y = fogProps.x + 1000.0f;
			}
			else
			{
				fogProps.x = 900000.0f;
				fogProps.y = 900000.0f;
			}
			STATEMANAGER.SetPixelShaderConstant(6, &fogProps, 1);

			STATEMANAGER.SetPixelShaderConstant(7, &material.Diffuse, 1);

#if defined(ANK_SYSTEM_GRAPHICS)
			float shadowOpacity = GetShadowIntensity() / 1.1;
			if (shadowOpacity < 42.0f)
			shadowOpacity = 42.0f;
			STATEMANAGER.SetPixelShaderConstant(10, (int*)&shadowOpacity, 1);


			float specularLightEnabled = (float)Frustum::Instance().realtimeSpecular;
#endif
			STATEMANAGER.SetPixelShaderConstant(11, &specularLightEnabled, 1);

			float shiness;
			D3DXVECTOR3 specularLight;
			float reducePointLightIntensity = 15.0f;

#if defined(ANK_SYSTEM_GRAPHICS)
			if (Frustum::Instance().selectRainFrustun == 0)
			{
				shiness = 47.0f;
				specularLight = { 1.5f,1.5f,1.5f };
				

				if (Frustum::Instance().isCerApus4Environment == true || Frustum::Instance().isCerApus4EnvironmentAzrael == true)
				{
					if (std::strcmp(currentMapName.c_str(), "metin2_map_n_desert_01") == 0)
					{
						shiness = 5.0f;
						specularLight = { 0.5f,0.5f,0.5f };
					}
					else
					{
						shiness = 47.0f;
						specularLight = { 0.6f,0.6f,0.6f };
					}
				}

				if (isPinkEnvironment == true)
				{
					shiness = 47.0f;
					specularLight = { 1.7f,1.7f,1.7f };
				}

				if (isNoapteEnvironment == true || isEnvironment6 == true || isEnvironment7 == true)
				{
					shiness = 45.0f;
					specularLight = { 0.3f, 0.3f, 0.3f };
				}

				if (isEnvironment9 == true || isEnvironment10 == true || isEnvironment11 == true)
				{
					shiness = 17.0f;
					specularLight = { 1.3f,1.3f,1.3f };
				}

				if (isEnvironment2 == true)
				{
					shiness = 800.0f;
					specularLight = { 0.0001f, 0.0001f, 0.0001f };
				}

				if (isEnvironment3 == true)
				{
					shiness = 7.0f;
					specularLight = { 0.55f, 0.55f, 0.55f };
				}

				if (isEnvironment4 == true)
				{
					shiness = 2.0f;
					specularLight = { 0.25f, 0.25f, 0.25f };
				}

				if (isEnvironment5 == true)
				{
					shiness = 12.0f;
					specularLight = { 0.25f, 0.25f, 0.25f };
				}


			}
			else
			{
				if (Frustum::Instance().isThunderTime == true)
				{
					shiness = 0.0f;
					specularLight = { 8.0f, 8.5f, 11.0f };
				}
				else
				{
					shiness = 0.0f;
					specularLight = { 0.499999f, 0.499999f, 0.499999f };
				}
			}

			if (std::strcmp(Frustum::Instance().currentMapNameSave.c_str(), "map_n_snowm_01") == 0)
			{
				if (CPythonBackground::Instance().SelectRain == 16 || CPythonBackground::Instance().SelectRain == 25 || CPythonBackground::Instance().SelectRain == 8)
				{
					shiness = 12.0f;
					specularLight = { 2.75f, 2.75f, 2.75f };
				}
				else
				{
					shiness = 16.0f;
					specularLight = { 1.75f, 1.75f, 2.0f };

					if (Frustum::Instance().isCerApus4Environment == true || Frustum::Instance().isCerApus4EnvironmentAzrael == true)
					{
						shiness = 69.0f;
						specularLight = { 0.3f, 0.3f, 0.3f };
					}

					if (isNoapteEnvironment)
					{
						shiness = 12.0f;
						specularLight = { 4.0f, 4.0f, 4.0f };
					}
				}
			}
#endif

			STATEMANAGER.SetPixelShaderConstant(8, &shiness, 1);
			STATEMANAGER.SetPixelShaderConstant(9, &specularLight, 1);
			STATEMANAGER.SetPixelShaderConstant(12, &time3, 1);

#if defined(ANK_SYSTEM_GRAPHICS)
			float enableLights;

			if (Frustum::Instance().ambianceEffectsLevel == 0)
				enableLights = 0.0f;
			if (Frustum::Instance().ambianceEffectsLevel == 1)
				enableLights = 1.0f;

			STATEMANAGER.SetPixelShaderConstant(13, &enableLights, 1);

			if (isNoapteEnvironment || isEnvironment5 || isEnvironment11 || Frustum::Instance().selectRainFrustun == 16 || Frustum::Instance().selectRainFrustun == 25 || Frustum::Instance().selectRainFrustun == 8)
			{
				if(isEnvironment5)
					reducePointLightIntensity = Frustum::Instance().pointLightIntensity = 0.75f;
				else if(isNoapteEnvironment)
					reducePointLightIntensity = Frustum::Instance().pointLightIntensity = 1.0f;
				else if(Frustum::Instance().selectRainFrustun == 25)
					reducePointLightIntensity = Frustum::Instance().pointLightIntensity = 0.75f;
				else
					reducePointLightIntensity = Frustum::Instance().pointLightIntensity = 1.2f;			
			}
#endif
			if (!CPythonSystem::Instance().GetAdvancedOption("local_light",1)) reducePointLightIntensity = 0.0f;
			STATEMANAGER.SetPixelShaderConstant(15, &reducePointLightIntensity, 1);
		}

		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
#ifdef DIRECTX9
		STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
#else
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
#endif


		//STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &m_matDynamicShadow);

		if (Frustum::Instance().isIndoor == false && Frustum::Instance().shadowType != 3)
		{
			if (m_lpCharacterShadowMapTexture)
				STATEMANAGER.SetTexture(1, m_lpCharacterShadowMapTexture);
			if (m_lpCharacterShadowMapTexture4)
				STATEMANAGER.SetTexture(2, m_lpCharacterShadowMapTexture4);


			//STATEMANAGER.SetTexture(1, NULL);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

			STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		}
		else
		{
			STATEMANAGER.SetTexture(1, NULL);
		}

		ms_faceCount += wPrimitiveCount;
		STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
		++m_iRenderedSplatNum;

		if (Frustum::Instance().shadowType != 3)
		{
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		}

		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
#ifdef DIRECTX9
		STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
#else
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
#endif

		STATEMANAGER.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		STATEMANAGER.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		STATEMANAGER.SetRenderState(D3DRS_FOGCOLOR, dwFogColor);

		STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

#if defined(ANK_SYSTEM_GRAPHICS)
		if (Frustum::Instance().isIndoor == false && Frustum::Instance().shadowType != 3)
		{
			STATEMANAGER.SetVertexShader(nullptr);
			STATEMANAGER.SetPixelShader(nullptr);
		}
#endif
	}

#ifndef WORLD_EDITOR
	// The bias above is for this patch's splat/overlay passes only. Left set, it applied to
	// every object, tree and character drawn after terrain, letting them show through
	// hill crests at distance.
	STATEMANAGER.SetRenderState(D3DRS_DEPTHBIAS, 0);
#endif
	++m_iRenderedPatchNum;

	int iCurRenderedSplatNum = m_iRenderedSplatNum - iPrevRenderedSplatNum;

	m_iRenderedSplatNumSqSum += iCurRenderedSplatNum * iCurRenderedSplatNum;
}


void CMapOutdoor::__HardwareTransformPatch_RenderPatchSplat2(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
{
	assert(NULL != m_pTerrainPatchProxyList && "__HardwareTransformPatch_RenderPatchSplat");
	CTerrainPatchProxy* pTerrainPatchProxy = &m_pTerrainPatchProxyList[patchnum];

	if (!pTerrainPatchProxy->isUsed())
		return;

	long sPatchNum = pTerrainPatchProxy->GetPatchNum();
	if (sPatchNum < 0)
		return;

	BYTE ucTerrainNum = pTerrainPatchProxy->GetTerrainNum();
	if (0xFF == ucTerrainNum)
		return;

	CTerrain* pTerrain;
	if (!GetTerrainPointer(ucTerrainNum, &pTerrain))
		return;

	DWORD dwFogColor;
	if (mc_pEnvironmentData)
		dwFogColor = mc_pEnvironmentData->FogColor;
	else
		dwFogColor = 0xffffffff;

	WORD wCoordX, wCoordY;
	pTerrain->GetCoordinate(&wCoordX, &wCoordY);

	TTerrainSplatPatch& rTerrainSplatPatch = pTerrain->GetTerrainSplatPatch();

	D3DXMATRIX matTexTransform, matSplatAlphaTexTransform, matSplatColorTexTransform;
	m_matWorldForCommonUse._41 = -(float)(wCoordX * CTerrainImpl::TERRAIN_XSIZE);
	m_matWorldForCommonUse._42 = (float)(wCoordY * CTerrainImpl::TERRAIN_YSIZE);
	D3DXMatrixMultiply(&matTexTransform, &m_matViewInverse, &m_matWorldForCommonUse);
	D3DXMatrixMultiply(&matSplatAlphaTexTransform, &matTexTransform, &m_matSplatAlpha);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &matSplatAlphaTexTransform);

	D3DXMATRIX matTiling;
	D3DXMatrixScaling(&matTiling, 1.0f / 640.0f, -1.0f / 640.0f, 0.0f);
	matTiling._41 = 0.0f;
	matTiling._42 = 0.0f;

	D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &matTiling);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);

	CGraphicVertexBuffer* pkVB = pTerrainPatchProxy->HardwareTransformPatch_GetVertexBufferPtr();
	if (!pkVB)
		return;

	STATEMANAGER.SetStreamSource(0, pkVB->GetD3DVertexBuffer(), m_iPatchTerrainVertexSize);


	int iPrevRenderedSplatNum = m_iRenderedSplatNum;

#if defined(ANK_SYSTEM_GRAPHICS)
	TTerrainTexture& rTexture = m_TextureSet.GetTexture(2);
#endif

	D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &rTexture.m_matTransform);

	STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
	STATEMANAGER.SetTexture(1, nullptr);  // Disable the splat texture

	STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
	++m_iRenderedPatchNum;

	int iCurRenderedSplatNum = m_iRenderedSplatNum - iPrevRenderedSplatNum;

	m_iRenderedSplatNumSqSum += iCurRenderedSplatNum * iCurRenderedSplatNum;
}

void CMapOutdoor::__HardwareTransformPatch_RenderPatchNone(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
{
	assert(NULL != m_pTerrainPatchProxyList && "__HardwareTransformPatch_RenderPatchNone");
	CTerrainPatchProxy* pTerrainPatchProxy = &m_pTerrainPatchProxyList[patchnum];

	if (!pTerrainPatchProxy->isUsed())
		return;

	CGraphicVertexBuffer* pkVB = pTerrainPatchProxy->HardwareTransformPatch_GetVertexBufferPtr();
	if (!pkVB)
		return;

	STATEMANAGER.SetStreamSource(0, pkVB->GetD3DVertexBuffer(), m_iPatchTerrainVertexSize);
	STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
}

#if defined(ANK_SYSTEM_GRAPHICS)
void CMapOutdoor::__RenderTerrain_RenderHardwareTransformPatchOriginal()
{
	DWORD dwFogColor;
	float fFogFarDistance;
	float fFogNearDistance;
	if (mc_pEnvironmentData)
	{
		dwFogColor = mc_pEnvironmentData->FogColor;
		fFogNearDistance = mc_pEnvironmentData->GetFogNearDistance();
		fFogFarDistance = mc_pEnvironmentData->GetFogFarDistance();
	}
	else
	{
		dwFogColor = 0xffffffff;
		fFogNearDistance = 5000.0f;
		fFogFarDistance = 10000.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	// Render State & TextureStageState

	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0x00000000);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	// Effect/particle pass'leri blend modunu (ONE/ONE vb.) geri almiyor; terrain splat'i her zaman normal alpha blend ile ciz.
	STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, dwFogColor);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
#ifdef DIRECTX9
	STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	STATEMANAGER.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
#else
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP);
#endif

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
#ifdef DIRECTX9
	STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	STATEMANAGER.SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
#else
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
#endif

#ifdef WORLD_EDITOR
	if (GetAsyncKeyState(VK_CAPITAL))
	{
		CSpeedTreeWrapper::ms_bSelfShadowOn = false;
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_GAUSSIANCUBIC);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_GAUSSIANCUBIC);
	}
	else
	{
		CSpeedTreeWrapper::ms_bSelfShadowOn = true;
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
	}
#else
	CSpeedTreeWrapper::ms_bSelfShadowOn = true;
	STATEMANAGER.SetBestFiltering(2);
#endif

	m_matWorldForCommonUse._41 = 0.0f;
	m_matWorldForCommonUse._42 = 0.0f;
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorldForCommonUse);

	STATEMANAGER.SaveTransform(D3DTS_TEXTURE0, &m_matWorldForCommonUse);
	STATEMANAGER.SaveTransform(D3DTS_TEXTURE1, &m_matWorldForCommonUse);

	// Render State & TextureStageState
	//////////////////////////////////////////////////////////////////////////

#ifdef DIRECTX9
	STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
#else
	STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL);
#endif

	m_iRenderedSplatNumSqSum = 0;
	m_iRenderedPatchNum = 0;
	m_iRenderedSplatNum = 0;
	m_RenderedTextureNumVector.clear();

	std::pair<float, long> fog_far(fFogFarDistance + 1600.0f, 0);
	std::pair<float, long> fog_near(fFogNearDistance - 3200.0f, 0);

	std::vector<std::pair<float, long> >::iterator far_it = std::upper_bound(m_PatchVector.begin(), m_PatchVector.end(), fog_far);
	std::vector<std::pair<float, long> >::iterator near_it = std::upper_bound(m_PatchVector.begin(), m_PatchVector.end(), fog_near);

#ifdef WORLD_EDITOR
	near_it = m_PatchVector.begin();
	far_it = m_PatchVector.end();
#endif

	WORD wPrimitiveCount;
	D3DPRIMITIVETYPE ePrimitiveType;

	BYTE byCUrrentLODLevel = 0;

	float fLODLevel1Distance = __GetNoFogDistance();
	float fLODLevel2Distance = __GetFogDistance();

	SelectIndexBuffer(0, &wPrimitiveCount, &ePrimitiveType);

	DWORD dwFogEnable = STATEMANAGER.GetRenderState(D3DRS_FOGENABLE);
	std::vector<std::pair<float, long> >::iterator it = m_PatchVector.begin();

#ifndef WORLD_EDITOR
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);

	for (; it != near_it; ++it)
	{
		if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
		{
			byCUrrentLODLevel = 1;
			SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
		}
		else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
		{
			byCUrrentLODLevel = 2;
			SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
		}

		__HardwareTransformPatch_RenderPatchSplat(it->second, wPrimitiveCount, ePrimitiveType);
		if (m_iRenderedSplatNum >= m_iSplatLimit)
			break;

		if (m_bDrawWireFrame)
			DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
	}
#endif

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, dwFogEnable);

	if (m_iRenderedSplatNum < m_iSplatLimit)
	{
		for (it = near_it; it != far_it; ++it)
		{
			if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
			{
				byCUrrentLODLevel = 1;
				SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
			}
			else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
			{
				byCUrrentLODLevel = 2;
				SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
			}

			__HardwareTransformPatch_RenderPatchSplat(it->second, wPrimitiveCount, ePrimitiveType);

			if (m_iRenderedSplatNum >= m_iSplatLimit)
				break;

			if (m_bDrawWireFrame)
				DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
		}
	}

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	STATEMANAGER.SetTexture(0, NULL);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, FALSE);

	STATEMANAGER.SetTexture(1, NULL);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, FALSE);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	if (m_iRenderedSplatNum < m_iSplatLimit)
	{
		for (it = far_it; it != m_PatchVector.end(); ++it)
		{
			if (byCUrrentLODLevel == 0 && fLODLevel1Distance <= it->first)
			{
				byCUrrentLODLevel = 1;
				SelectIndexBuffer(1, &wPrimitiveCount, &ePrimitiveType);
			}
			else if (byCUrrentLODLevel == 1 && fLODLevel2Distance <= it->first)
			{
				byCUrrentLODLevel = 2;
				SelectIndexBuffer(2, &wPrimitiveCount, &ePrimitiveType);
			}

			__HardwareTransformPatch_RenderPatchNone(it->second, wPrimitiveCount, ePrimitiveType);

			if (m_iRenderedSplatNum >= m_iSplatLimit)
				break;

			if (m_bDrawWireFrame)
				DrawWireFrame(it->second, wPrimitiveCount, ePrimitiveType);
		}
	}

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

#endif
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, dwFogEnable);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);

	std::sort(m_RenderedTextureNumVector.begin(),m_RenderedTextureNumVector.end());

	//////////////////////////////////////////////////////////////////////////
	// Render State & TextureStageState

	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE0);
	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE1);

	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS);

	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);
	STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);

	for (int s = 0; s < 9; ++s)
		STATEMANAGER.SetTexture(s, nullptr);

	// Render State & TextureStageState
	//////////////////////////////////////////////////////////////////////////
}

#if defined(ANK_SYSTEM_GRAPHICS)
void CMapOutdoor::__HardwareTransformPatch_RenderPatchSplatOriginal(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
#else
void CMapOutdoor::__HardwareTransformPatch_RenderPatchSplat(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
#endif
{
	assert(NULL!=m_pTerrainPatchProxyList && "__HardwareTransformPatch_RenderPatchSplat");
	CTerrainPatchProxy * pTerrainPatchProxy = &m_pTerrainPatchProxyList[patchnum];

	if (!pTerrainPatchProxy->isUsed())
		return;

	long sPatchNum = pTerrainPatchProxy->GetPatchNum();
	if (sPatchNum < 0)
		return;

	BYTE ucTerrainNum = pTerrainPatchProxy->GetTerrainNum();
	if (0xFF == ucTerrainNum)
		return;

	CTerrain * pTerrain;
	if (!GetTerrainPointer(ucTerrainNum, &pTerrain))
		return;

	DWORD dwFogColor;
	if (mc_pEnvironmentData)
		dwFogColor=mc_pEnvironmentData->FogColor;
	else
		dwFogColor=0xffffffff;

	WORD wCoordX, wCoordY;
	pTerrain->GetCoordinate(&wCoordX, &wCoordY);

	TTerrainSplatPatch & rTerrainSplatPatch = pTerrain->GetTerrainSplatPatch();

	D3DXMATRIX matTexTransform, matSplatAlphaTexTransform, matSplatColorTexTransform;
	m_matWorldForCommonUse._41 = -(float) (wCoordX * CTerrainImpl::TERRAIN_XSIZE);
	m_matWorldForCommonUse._42 = (float) (wCoordY * CTerrainImpl::TERRAIN_YSIZE);
	D3DXMatrixMultiply(&matTexTransform, &m_matViewInverse, &m_matWorldForCommonUse);
	D3DXMatrixMultiply(&matSplatAlphaTexTransform, &matTexTransform, &m_matSplatAlpha);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &matSplatAlphaTexTransform);

	D3DXMATRIX matTiling;
	D3DXMatrixScaling(&matTiling, 1.0f/640.0f, -1.0f/640.0f, 0.0f);
	matTiling._41=0.0f;
	matTiling._42=0.0f;

	D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &matTiling);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);

	CGraphicVertexBuffer* pkVB=pTerrainPatchProxy->HardwareTransformPatch_GetVertexBufferPtr();
	if (!pkVB)
		return;

	STATEMANAGER.SetStreamSource(0, pkVB->GetD3DVertexBuffer(), m_iPatchTerrainVertexSize);

	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

	int iPrevRenderedSplatNum = m_iRenderedSplatNum;


	bool isFirst = true;
	for (DWORD j = 1; j < pTerrain->GetNumTextures(); ++j)
	{
		TTerainSplat & rSplat = rTerrainSplatPatch.Splats[j];

		if (!rSplat.Active)
			continue;

		if (rTerrainSplatPatch.PatchTileCount[sPatchNum][j] == 0)
			continue;

		const TTerrainTexture & rTexture = m_TextureSet.GetTexture(j);

		D3DXMatrixMultiply(&matSplatColorTexTransform, &m_matViewInverse, &rTexture.m_matTransform);
		STATEMANAGER.SetTransform(D3DTS_TEXTURE0, &matSplatColorTexTransform);
		if (isFirst)
		{
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
			STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
			STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
			STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
			STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
			isFirst=false;
		}
		else
		{
			STATEMANAGER.SetTexture(0, rTexture.pd3dTexture);
			STATEMANAGER.SetTexture(1, rSplat.pd3dTexture);
			STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);
		}

		std::vector<int>::iterator aIterator = std::find(m_RenderedTextureNumVector.begin(), m_RenderedTextureNumVector.end(), (int)j);
		if (aIterator == m_RenderedTextureNumVector.end())
			m_RenderedTextureNumVector.push_back(j);
		++m_iRenderedSplatNum;
		if (m_iRenderedSplatNum >= m_iSplatLimit)
			break;

	}

	++m_iRenderedPatchNum;

	int iCurRenderedSplatNum=m_iRenderedSplatNum-iPrevRenderedSplatNum;

	m_iRenderedSplatNumSqSum+=iCurRenderedSplatNum*iCurRenderedSplatNum;
}
