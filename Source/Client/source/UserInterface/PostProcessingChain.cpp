#include "stdafx.h"
#ifndef DISABLE_JTX_DYNAMIC_LIGHT
#include "../EterLib/StdAfx.h"
#include "../EterLib/StateManager.h"
#include "../EterPack/EterPackManager.h"
#include "PostProcessingChain.h"
#include "PythonSystem.h"
#include <algorithm>
#include "../UserInterface/PythonBackground.h"

typedef struct SRTTVertex
{
	D3DXVECTOR4 p;
	D3DXVECTOR2 t;
} TRTTVertex;

CPostProcessingChain::CPostProcessingChain() :
	m_lpSourceTexture(nullptr),
	m_lpLastPass(nullptr),
	m_lpTarget(nullptr),
	m_lpBackupTargetSurface(nullptr),
	m_lpSourceTextureSurface(nullptr),
	m_lpTargetSurface(nullptr),
	m_lpDepthStencilSurface(nullptr),
	m_lpDepthStencilBackup(nullptr),
	m_lpScreenQuad(nullptr),
	m_lpLastPassSurface(nullptr),
	m_pCopyPixelShader(nullptr),
	m_pFxaaPixelShader(nullptr),
	m_bSceneBegun(false)
{
}
CPostProcessingChain::~CPostProcessingChain()
{
	ReleaseResources();
}

void CPostProcessingChain::ReleaseResources()
{
	m_bSceneBegun = false;
	if (m_lpBackupTargetSurface)
	{
		m_lpBackupTargetSurface->Release();
		m_lpBackupTargetSurface = nullptr;
	}
	if (m_lpDepthStencilBackup)
	{
		m_lpDepthStencilBackup->Release();
		m_lpDepthStencilBackup = nullptr;
	}
	if (m_lpSourceTextureSurface)
	{
		m_lpSourceTextureSurface->Release();
		m_lpSourceTextureSurface = nullptr;
	}
	if (m_lpSourceTexture)
	{
		m_lpSourceTexture->Release();
		m_lpSourceTexture = nullptr;
	}
	if (m_lpLastPassSurface)
	{
		m_lpLastPassSurface->Release();
		m_lpLastPassSurface = nullptr;
	}
	if (m_lpLastPass)
	{
		m_lpLastPass->Release();
		m_lpLastPass = nullptr;
	}
	if (m_lpTarget)
	{
		m_lpTarget->Release();
		m_lpTarget = nullptr;
	}
	if (m_lpTargetSurface)
	{
		m_lpTargetSurface->Release();
		m_lpTargetSurface = nullptr;
	}
	if (m_lpDepthStencilSurface)
	{
		m_lpDepthStencilSurface->Release();
		m_lpDepthStencilSurface = nullptr;
	}
	if (m_lpScreenQuad)
	{
		m_lpScreenQuad->Release();
		m_lpScreenQuad = nullptr;
	}
	if (m_pCopyPixelShader)
	{
		m_pCopyPixelShader->Release();
		m_pCopyPixelShader = nullptr;
	}
	if (m_pFxaaPixelShader)
	{
		m_pFxaaPixelShader->Release();
		m_pFxaaPixelShader = nullptr;
	}

	RemoveAllEffects();
}
bool CPostProcessingChain::CreateResources()
{
	ReleaseResources();

	D3DSURFACE_DESC kDesc;
	IDirect3DSurface9* pSurface = nullptr;
	if (FAILED(STATEMANAGER.GetDevice()->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface)) || !pSurface)
	{
		TraceError("CPostProcessingChain unable to get back buffer");
		return false;
	}
	pSurface->GetDesc(&kDesc);
	pSurface->Release();
	pSurface = nullptr;

	const UINT uWidth = kDesc.Width;
	const UINT uHeight = kDesc.Height;
	const D3DFORMAT targetFormat = D3DFMT_A8R8G8B8;
	// The 3D scene target has no alpha channel, like the original X8R8G8B8 backbuffer. Many
	// effects (e.g. Ninja/Sura skills, 119 .mse files) blend with D3DBLEND_DESTALPHA; with an
	// alpha target they were multiplied by the alpha terrain/water shaders write and turned black.
	const D3DFORMAT sceneFormat = D3DFMT_X8R8G8B8;

	if (FAILED(STATEMANAGER.GetDevice()->CreateTexture(uWidth, uHeight, 1, D3DUSAGE_RENDERTARGET, sceneFormat, D3DPOOL_DEFAULT, &m_lpSourceTexture, nullptr)))
	{
		TraceError("CPostProcessingChain unable to create render target");
		ReleaseResources();
		return false;
	}
	m_lpSourceTexture->GetSurfaceLevel(0, &m_lpSourceTextureSurface);

	if (FAILED(STATEMANAGER.GetDevice()->CreateTexture(uWidth, uHeight, 1, D3DUSAGE_RENDERTARGET, targetFormat, D3DPOOL_DEFAULT, &m_lpLastPass, nullptr)))
	{
		TraceError("CPostProcessingChain unable to create pass render target");
		ReleaseResources();
		return false;
	}
	m_lpLastPass->GetSurfaceLevel(0, &m_lpLastPassSurface);

	if (FAILED(STATEMANAGER.GetDevice()->CreateTexture(uWidth, uHeight, 1, D3DUSAGE_RENDERTARGET, targetFormat, D3DPOOL_DEFAULT, &m_lpTarget, nullptr)))
	{
		TraceError("CPostProcessingChain unable to create final render target");
		ReleaseResources();
		return false;
	}
	m_lpTarget->GetSurfaceLevel(0, &m_lpTargetSurface);

	if (SUCCEEDED(STATEMANAGER.GetDevice()->GetDepthStencilSurface(&pSurface)) && pSurface)
	{
		pSurface->GetDesc(&kDesc);
		pSurface->Release();
		pSurface = nullptr;

		if (FAILED(STATEMANAGER.GetDevice()->CreateDepthStencilSurface(kDesc.Width, kDesc.Height, kDesc.Format, kDesc.MultiSampleType, kDesc.MultiSampleQuality, FALSE, &m_lpDepthStencilSurface, nullptr)))
		{
			TraceError("CPostProcessingChain unable to create depth surface");
		}
	}

#ifdef DirectX9Ex
	if (FAILED(STATEMANAGER.GetDevice()->CreateVertexBuffer(sizeof(TRTTVertex) * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_DEFAULT, &m_lpScreenQuad, nullptr)))
#else
	if (FAILED(STATEMANAGER.GetDevice()->CreateVertexBuffer(sizeof(TRTTVertex) * 6, 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_lpScreenQuad, nullptr)))
#endif
	{
		TraceError("CPostProcessingChain unable to create screen quad");
		ReleaseResources();
		return false;
	}

	float fWidth = static_cast<float>(uWidth) - 0.5f;
	float fHeight = static_cast<float>(uHeight) - 0.5f;

	TRTTVertex* pVertices = nullptr;
	if (SUCCEEDED(m_lpScreenQuad->Lock(0, 0, reinterpret_cast<void**>(&pVertices), 0)) && pVertices)
	{
		pVertices[0].p = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
		pVertices[0].t = D3DXVECTOR2(0.0f, 0.0f);
		pVertices[1].p = D3DXVECTOR4(-0.5f, fHeight, 0.0f, 1.0f);
		pVertices[1].t = D3DXVECTOR2(0.0f, 1.0f);
		pVertices[2].p = D3DXVECTOR4(fWidth, fHeight, 0.0f, 1.0f);
		pVertices[2].t = D3DXVECTOR2(1.0f, 1.0f);
		pVertices[3].p = D3DXVECTOR4(-0.5f, -0.5f, 0.0f, 1.0f);
		pVertices[3].t = D3DXVECTOR2(0.0f, 0.0f);
		pVertices[4].p = D3DXVECTOR4(fWidth, fHeight, 0.0f, 1.0f);
		pVertices[4].t = D3DXVECTOR2(1.0f, 1.0f);
		pVertices[5].p = D3DXVECTOR4(fWidth, -0.5f, 0.0f, 1.0f);
		pVertices[5].t = D3DXVECTOR2(1.0f, 0.0f);
		m_lpScreenQuad->Unlock();
	}

	// Final pass: contrast-adaptive sharpening (AMD CAS style: per-pixel strength from the local
	// min/max, so no halos or ringing) followed by the colour adjustments. This replaces a 4-tap
	// "AA" blur that ran after the sharpen effect and undid it.
	const char* c_szCopyShader =
		"sampler2D s0 : register(s0);\n"
		"float4 adjust : register(c0);\n"
		"float4 sampling : register(c1);\n"
		"float4 tint : register(c2);\n"
		"float4 main(float2 uv : TEXCOORD0) : COLOR0 {\n"
		" float4 center = tex2D(s0, uv);\n"
		" float3 color = center.rgb;\n"
		" if (sampling.z > 0.001) {\n"
		"  float3 n = tex2D(s0, uv+float2(0,-sampling.y)).rgb;\n"
		"  float3 s = tex2D(s0, uv+float2(0, sampling.y)).rgb;\n"
		"  float3 e = tex2D(s0, uv+float2( sampling.x,0)).rgb;\n"
		"  float3 w = tex2D(s0, uv+float2(-sampling.x,0)).rgb;\n"
		"  float3 mn = min(color, min(min(n,s), min(e,w)));\n"
		"  float3 mx = max(color, max(max(n,s), max(e,w)));\n"
		"  float3 amp = sqrt(saturate(min(mn, 1.0-mx) / max(mx, 1e-4)));\n"
		"  float3 wgt = amp * (-1.0 / lerp(8.0, 5.0, sampling.z));\n"
		"  color = saturate((color + (n+s+e+w)*wgt) / (1.0 + 4.0*wgt));\n"
		" }\n"
		" float luminance = dot(color,float3(.299,.587,.114));\n"
		" color = lerp(luminance.xxx,color,adjust.y);\n"
		" color = (color-.5)*adjust.z+.5;\n"
		" color *= adjust.x;\n"
		" color += float3(adjust.w,0,-adjust.w);\n"
		" return float4(saturate(color*tint.rgb),center.a);\n"
		"}\n";

	// First pass: FXAA (luma-directed edge blend, after Lottes). It runs on the raw scene before
	// any effect, so the sharpening afterwards no longer re-amplifies the jaggies.
	const char* c_szFxaaShader =
		"sampler2D s0 : register(s0);\n"
		"float4 rcpFrame : register(c3);\n"
		"float L(float3 c){ return dot(c, float3(0.299,0.587,0.114)); }\n"
		"float3 T(float2 uv){ return tex2D(s0, uv).rgb; }\n"
		"float4 main(float2 uv : TEXCOORD0) : COLOR0 {\n"
		" float4 m = tex2D(s0, uv);\n"
		" float lNW = L(T(uv+float2(-1,-1)*rcpFrame.xy)); float lNE = L(T(uv+float2(1,-1)*rcpFrame.xy));\n"
		" float lSW = L(T(uv+float2(-1, 1)*rcpFrame.xy)); float lSE = L(T(uv+float2(1, 1)*rcpFrame.xy));\n"
		" float lM = L(m.rgb);\n"
		" float lMin = min(lM, min(min(lNW,lNE), min(lSW,lSE)));\n"
		" float lMax = max(lM, max(max(lNW,lNE), max(lSW,lSE)));\n"
		" if (lMax - lMin < max(0.0312, lMax*0.125)) return m;\n"
		" float2 dir = float2(-((lNW+lNE)-(lSW+lSE)), (lNW+lSW)-(lNE+lSE));\n"
		" float red = max((lNW+lNE+lSW+lSE)*(0.25/8.0), 1.0/128.0);\n"
		" float rcpMin = 1.0/(min(abs(dir.x),abs(dir.y))+red);\n"
		" dir = clamp(dir*rcpMin, -8.0, 8.0)*rcpFrame.xy;\n"
		" float3 a = 0.5*(T(uv+dir*(1.0/3.0-0.5))+T(uv+dir*(2.0/3.0-0.5)));\n"
		" float3 b = a*0.5+0.25*(T(uv-dir*0.5)+T(uv+dir*0.5));\n"
		" float lB = L(b);\n"
		" return float4((lB<lMin||lB>lMax)?a:b, m.a);\n"
		"}\n";

	auto CompilePS = [](const char* szSrc, IDirect3DPixelShader9** ppShader)
	{
		// ps_2_x: the screen quad is drawn with the fixed-function (XYZRHW) vertex path, and D3D9
		// does not allow a ps_3_0 shader without a vs_3_0 one.
		LPD3DXBUFFER pCode = nullptr;
		LPD3DXBUFFER pError = nullptr;
		HRESULT hr = D3DXCompileShader(szSrc, strlen(szSrc), nullptr, nullptr, "main", "ps_2_0", 0, &pCode, &pError, nullptr);
		if (FAILED(hr))
		{
			if (pError) { pError->Release(); pError = nullptr; }
			hr = D3DXCompileShader(szSrc, strlen(szSrc), nullptr, nullptr, "main", "ps_2_b", 0, &pCode, &pError, nullptr);
		}
		if (SUCCEEDED(hr) && pCode)
			STATEMANAGER.GetDevice()->CreatePixelShader(reinterpret_cast<const DWORD*>(pCode->GetBufferPointer()), ppShader);
		else
			TraceError("CPostProcessingChain shader compile failed: %s", pError ? (const char*)pError->GetBufferPointer() : "?");
		if (pCode)
			pCode->Release();
		if (pError)
			pError->Release();
	};
	CompilePS(c_szCopyShader, &m_pCopyPixelShader);
	CompilePS(c_szFxaaShader, &m_pFxaaPixelShader);

	// Sharpening is built into the final pass (CAS); the packed sharpness.fxd is not in the
	// sound pack, so AddEffect silently dropped it and the slider did nothing.
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_DEATH_CAMERA)
	AddEffect("sound/shaders/DeathCam.fxd");
#endif
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_UNDERWATER_CAMERA)
	AddEffect("sound/shaders/underwatercamera.fxd");
	AddEffect("sound/shaders/waterbubbles.fxd");
#endif
	return true;
}

bool CPostProcessingChain::BeginScene()
{
	if (m_bSceneBegun)
		return true;

	if (!m_lpSourceTexture || !m_lpLastPass || !m_lpTarget || !m_lpScreenQuad)
	{
		if (!CreateResources())
		{
			return false;
		}
	}

	if (FAILED(STATEMANAGER.GetDevice()->GetRenderTarget(0, &m_lpBackupTargetSurface)) || !m_lpBackupTargetSurface)
	{
		m_lpBackupTargetSurface = nullptr;
		return false;
	}

	if (FAILED(STATEMANAGER.GetDevice()->GetDepthStencilSurface(&m_lpDepthStencilBackup)) || !m_lpDepthStencilBackup)
	{
		m_lpDepthStencilBackup = nullptr;
		if (m_lpBackupTargetSurface)
		{
			m_lpBackupTargetSurface->Release();
			m_lpBackupTargetSurface = nullptr;
		}
		return false;
	}

	STATEMANAGER.GetDevice()->SetRenderTarget(0, m_lpSourceTextureSurface);

	// Clear the render target and the depth stencil
	STATEMANAGER.GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	m_bSceneBegun = true;
	return true;
}

bool CPostProcessingChain::EndScene()
{
	if (!m_bSceneBegun)
		return false;

	m_bSceneBegun = false;

	// Restore the original render target
	if (m_lpBackupTargetSurface)
	{
		STATEMANAGER.GetDevice()->SetRenderTarget(0, m_lpBackupTargetSurface);
		m_lpBackupTargetSurface->Release();
		m_lpBackupTargetSurface = nullptr;
	}

	// Restore the original depth stencil surface
	if (m_lpDepthStencilBackup)
	{
		STATEMANAGER.GetDevice()->SetDepthStencilSurface(m_lpDepthStencilBackup);
		m_lpDepthStencilBackup->Release();
		m_lpDepthStencilBackup = nullptr;
	}

	// Set the fixed-function vertex format
	STATEMANAGER.GetDevice()->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	STATEMANAGER.ForcePixelShader(nullptr);
	STATEMANAGER.GetDevice()->SetVertexShader(nullptr);
	return true;
}





bool CPostProcessingChain::Render()
{
	if (!m_bSceneBegun || !m_lpScreenQuad || !m_lpSourceTexture || !m_lpLastPassSurface || !m_lpBackupTargetSurface)
		return false;

	// Save and set necessary render states
	STATEMANAGER.SaveRenderState(D3DRS_ZENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_FOGENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_LIGHTING, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// Ensure clean texture stage 0 and 1 states
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	// Set the render target for the last pass surface
	STATEMANAGER.GetDevice()->SetRenderTarget(0, m_lpLastPassSurface);

	// Clear the render target
	STATEMANAGER.GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	// Set the source texture and vertex format
	STATEMANAGER.GetDevice()->SetTexture(0, m_lpSourceTexture);
	STATEMANAGER.GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	STATEMANAGER.GetDevice()->SetStreamSource(0, m_lpScreenQuad, 0, sizeof(TRTTVertex));
	// Post passes sample with bilinear + clamp (WRAP pulled the opposite screen edge in).
	DWORD dwSampler0[4];
	STATEMANAGER.GetSamplerState(0, D3DSAMP_ADDRESSU, &dwSampler0[0]);
	STATEMANAGER.GetSamplerState(0, D3DSAMP_ADDRESSV, &dwSampler0[1]);
	STATEMANAGER.GetSamplerState(0, D3DSAMP_MINFILTER, &dwSampler0[2]);
	STATEMANAGER.GetSamplerState(0, D3DSAMP_MAGFILTER, &dwSampler0[3]);
	STATEMANAGER.GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	STATEMANAGER.GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	STATEMANAGER.GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	STATEMANAGER.GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    const float identityAdjust[4] = {1,1,1,0};
    const float identitySampling[4] = {0,0,0,0};
    const float identityTint[4] = {1,1,1,1};
    STATEMANAGER.GetDevice()->SetPixelShaderConstantF(2, identityTint, 1);
    STATEMANAGER.GetDevice()->SetPixelShaderConstantF(0, identityAdjust, 1);
    STATEMANAGER.GetDevice()->SetPixelShaderConstantF(1, identitySampling, 1);

	D3DSURFACE_DESC sourceDesc;
	m_lpSourceTextureSurface->GetDesc(&sourceDesc);
	const float rcpFrame[4] = { 1.0f / sourceDesc.Width, 1.0f / sourceDesc.Height, 0.0f, 0.0f };
	STATEMANAGER.GetDevice()->SetPixelShaderConstantF(3, rcpFrame, 1);

	if (m_pFxaaPixelShader && CPythonSystem::Instance().GetAdvancedOption("anti_aliasing", 1))
		STATEMANAGER.ForcePixelShader(m_pFxaaPixelShader);
	else if (m_pCopyPixelShader)
		STATEMANAGER.ForcePixelShader(m_pCopyPixelShader);
	else
		STATEMANAGER.ForcePixelShader(nullptr);

	STATEMANAGER.GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// Iterate through post-processing effects
	IDirect3DSurface9* pCurrentTarget = m_lpTargetSurface;
	STATEMANAGER.GetDevice()->SetTexture(1, m_lpSourceTexture);

	for (auto& effect : m_kEffects)
	{
		D3DSURFACE_DESC kDesc;
		pCurrentTarget->GetDesc(&kDesc);

		D3DXVECTOR4 vScreenSize(static_cast<float>(kDesc.Width), static_cast<float>(kDesc.Height), 0.0f, 0.0f);
		if (effect.GetConstants())
			effect.GetConstants()->SetFloatArray(STATEMANAGER.GetDevice(), "ScreenSize", reinterpret_cast<float*>(&vScreenSize), 4);

		if (effect.Apply())
		{
			STATEMANAGER.GetDevice()->SetRenderTarget(0, pCurrentTarget);
			STATEMANAGER.GetDevice()->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
			STATEMANAGER.GetDevice()->SetTexture(0, pCurrentTarget == m_lpLastPassSurface ? m_lpTarget : m_lpLastPass);
			STATEMANAGER.GetDevice()->SetStreamSource(0, m_lpScreenQuad, 0, sizeof(TRTTVertex));
			STATEMANAGER.GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
			pCurrentTarget = (pCurrentTarget == m_lpLastPassSurface) ? m_lpTargetSurface : m_lpLastPassSurface;
		}
	}

	STATEMANAGER.GetDevice()->SetTexture(1, nullptr);

	// Set the render target back to the backup target surface
	STATEMANAGER.GetDevice()->SetRenderTarget(0, m_lpBackupTargetSurface);
	STATEMANAGER.GetDevice()->SetTexture(0, pCurrentTarget == m_lpLastPassSurface ? m_lpTarget : m_lpLastPass);
	STATEMANAGER.GetDevice()->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	STATEMANAGER.GetDevice()->SetStreamSource(0, m_lpScreenQuad, 0, sizeof(TRTTVertex));
    const auto& options = CPythonSystem::Instance();
    const float colorAdjust[4] = {
        2.0f*(1.0f-options.GetAdvancedOption("env_darkness_pos",.5f)),
        2.0f*options.GetAdvancedOption("env_saturation_pos",.5f),
        2.0f*options.GetAdvancedOption("env_contrast_pos",.5f),
        .4f*(options.GetAdvancedOption("env_temperature_pos",.5f)-.5f)};
    D3DSURFACE_DESC outputDesc;
    m_lpLastPassSurface->GetDesc(&outputDesc);
    // Sharpness slider is 0..5 (CPythonSystem); map to CAS amount 0..1.
    const float fSharpen = std::clamp(CPythonSystem::Instance().GetSharpnessPower() / 3.0f, 0.0f, 1.0f);
    const float sampling[4] = {1.0f/outputDesc.Width,1.0f/outputDesc.Height,fSharpen,0};
    const float tint[4] = {options.GetAdvancedOption("environment_color_r",1),options.GetAdvancedOption("environment_color_g",1),options.GetAdvancedOption("environment_color_b",1),1};
    STATEMANAGER.GetDevice()->SetPixelShaderConstantF(2, tint, 1);
    STATEMANAGER.GetDevice()->SetPixelShaderConstantF(0, colorAdjust, 1);
    STATEMANAGER.GetDevice()->SetPixelShaderConstantF(1, sampling, 1);
	if (m_pCopyPixelShader)
		STATEMANAGER.ForcePixelShader(m_pCopyPixelShader);
	else
		STATEMANAGER.ForcePixelShader(nullptr);

	STATEMANAGER.GetDevice()->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// Reset pixel shader to null
	STATEMANAGER.ForcePixelShader(nullptr);
	STATEMANAGER.GetDevice()->SetTexture(0, nullptr);
	STATEMANAGER.GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSU, dwSampler0[0]);
	STATEMANAGER.GetDevice()->SetSamplerState(0, D3DSAMP_ADDRESSV, dwSampler0[1]);
	STATEMANAGER.GetDevice()->SetSamplerState(0, D3DSAMP_MINFILTER, dwSampler0[2]);
	STATEMANAGER.GetDevice()->SetSamplerState(0, D3DSAMP_MAGFILTER, dwSampler0[3]);

	// Restore the original render states
	STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
	STATEMANAGER.RestoreRenderState(D3DRS_LIGHTING);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_FOGENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);

	return true;
}

CPostProcessingEffect CPostProcessingChain::AddEffect(const char* c_pszFileName)
{
	CPostProcessingEffect kEffect;
	CMappedFile kFile;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(kFile, c_pszFileName, &pData))
	{
		return kEffect;
	}
	if (!kEffect.CreateShader(kFile))
	{
		return CPostProcessingEffect();
	}
	AddEffect(kEffect);
	return kEffect;
}


bool CPostProcessingChain::AddEffect(const CPostProcessingEffect& c_kEffect)
{
	m_kEffects.push_back(c_kEffect);
	return true;
}
bool CPostProcessingChain::RemoveEffect(const CPostProcessingEffect& c_kEffect)
{
	bool bResult = false;
	for (std::list<CPostProcessingEffect>::iterator it = m_kEffects.begin(); it != m_kEffects.end(); ++it)
	{
		if ((*it) == c_kEffect)
		{
			m_kEffects.erase(it--);
			bResult = true;
		}
	}
	return bResult;
}

bool CPostProcessingChain::RemoveAllEffects()
{
	m_kEffects.clear();
	return true;
}

std::list<CPostProcessingEffect>& CPostProcessingChain::GetEffects()
{
	return m_kEffects;
}
#endif
