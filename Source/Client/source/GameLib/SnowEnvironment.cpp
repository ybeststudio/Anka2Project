#include "StdAfx.h"
#include "SnowEnvironment.h"

#include "../EterLib/StateManager.h"
#include "../EterLib/Camera.h"
#include "../EterLib/ResourceManager.h"
#include "SnowParticle.h"

void CSnowEnvironment::Enable()
{
	if (m_eWeatherMode == WEATHER_MODE_NONE)
		m_eWeatherMode = WEATHER_MODE_SNOW;

	if (!m_bSnowEnable)
	{
		Create();
	}

	m_bSnowEnable = TRUE;
}

void CSnowEnvironment::Disable()
{
	m_bSnowEnable = FALSE;
	m_eWeatherMode = WEATHER_MODE_NONE;
	m_iRainLevel = 0;
	stl_wipe(m_kVct_pkParticleSnow);
}

void CSnowEnvironment::EnableSnow()
{
	if (m_eWeatherMode != WEATHER_MODE_SNOW)
	{
		stl_wipe(m_kVct_pkParticleSnow);
	}

	m_eWeatherMode = WEATHER_MODE_SNOW;
	m_iRainLevel = 0;

	if (!m_pSnowImage)
	{
		m_pSnowImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer("d:/ymir work/special/snow.dds");
	}

	if (m_pImageInstance && m_pSnowImage)
	{
		m_pImageInstance->SetImagePointer(m_pSnowImage);
	}

	Enable();
}

void CSnowEnvironment::EnableRain(int rainLevel)
{
	if (m_eWeatherMode != WEATHER_MODE_RAIN)
	{
		stl_wipe(m_kVct_pkParticleSnow);
	}

	m_eWeatherMode = WEATHER_MODE_RAIN;
	m_iRainLevel = rainLevel;

	if (!m_pRainImage)
	{
		m_pRainImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer("d:/ymir work/special/rain.dds");
	}

	if (m_pImageInstance && m_pRainImage)
	{
		m_pImageInstance->SetImagePointer(m_pRainImage);
	}

	Enable();
}

void CSnowEnvironment::Update(const D3DXVECTOR3 & c_rv3Pos)
{
	if (!m_bSnowEnable)
	{
		if (m_kVct_pkParticleSnow.empty())
			return;
	}

	m_v3Center=c_rv3Pos;
}

void CSnowEnvironment::Deform()
{
	if (!m_bSnowEnable)
	{
		if (m_kVct_pkParticleSnow.empty())
			return;
	}

	const D3DXVECTOR3 & c_rv3Pos=m_v3Center;

	static long s_lLastTime = CTimer::Instance().GetCurrentMillisecond();
	long lcurTime = CTimer::Instance().GetCurrentMillisecond();
	float fElapsedTime = float(lcurTime - s_lLastTime) / 1000.0f;
	s_lLastTime = lcurTime;

	if (fElapsedTime > 0.1f)
		fElapsedTime = 0.1f;
	else if (fElapsedTime <= 0.0f)
		fElapsedTime = 0.016f;

	CCamera * pCamera = CCameraManager::Instance().GetCurrentCamera();
	if (!pCamera)
		return;

	const D3DXVECTOR3 & c_rv3View = pCamera->GetView();

	D3DXVECTOR3 v3ChangedPos = c_rv3View * 3500.0f + c_rv3Pos;
	v3ChangedPos.z = c_rv3Pos.z;

	std::vector<CSnowParticle*>::iterator itor = m_kVct_pkParticleSnow.begin();
	for (; itor != m_kVct_pkParticleSnow.end();)
	{
		CSnowParticle * pSnow = *itor;
		pSnow->Update(fElapsedTime, v3ChangedPos);

		if (!pSnow->IsActivate())
		{
			CSnowParticle::Delete(pSnow);

			itor = m_kVct_pkParticleSnow.erase(itor);
		}
		else
		{
			++itor;
		}
	}

	if (m_bSnowEnable)
	{
		int iParticleType = (m_eWeatherMode == WEATHER_MODE_RAIN) ? CSnowParticle::TYPE_RAIN : CSnowParticle::TYPE_SNOW;
		int iMaxSpawn = 10;
		if (m_eWeatherMode == WEATHER_MODE_RAIN)
		{
			iMaxSpawn = (m_iRainLevel >= 2) ? 60 : 30;
		}

		int iSpawnCount = min(iMaxSpawn, int(m_dwParticleMaxNum - m_kVct_pkParticleSnow.size()));
		for (int p = 0; p < iSpawnCount; ++p)
		{
			CSnowParticle * pSnowParticle = CSnowParticle::New();
			pSnowParticle->Init(v3ChangedPos, iParticleType);
			m_kVct_pkParticleSnow.push_back(pSnowParticle);
		}
	}
}

void CSnowEnvironment::__BeginBlur()
{
	if (!m_bBlurEnable)
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    ms_lpd3dDevice->GetRenderTarget(0, &m_lpOldSurface);
#else
    ms_lpd3dDevice->GetRenderTarget(&m_lpOldSurface);
#endif

#ifdef ENABLE_DIRECTX9_UPDATE
    ms_lpd3dDevice->SetDepthStencilSurface(m_lpSnowDepthSurface);
    ms_lpd3dDevice->SetRenderTarget(0, m_lpSnowDepthSurface);
#else
    ms_lpd3dDevice->SetRenderTarget(m_lpSnowRenderTargetSurface, m_lpSnowDepthSurface);
#endif

	ms_lpd3dDevice->Clear(0L, nullptr, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0L);

	STATEMANAGER.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	STATEMANAGER.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_DESTALPHA);
}

void CSnowEnvironment::__ApplyBlur()
{
	if (!m_bBlurEnable)
		return;

//			{
//				STATEMANAGER.SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
//				STATEMANAGER.SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
//				STATEMANAGER.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//				STATEMANAGER.SetRenderState( D3DRS_COLORVERTEX ,TRUE);
//				STATEMANAGER.SetRenderState( D3DRS_DIFFUSEMATERIALSOURCE , D3DMCS_COLOR1 );
//				STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
//				STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
//				STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
//				STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
//				STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
//				STATEMANAGER.SetTextureStageState(0,  D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
//				DWORD	alphaColor = 0xFFFFFF | ((DWORD)(0.6f*255.0f) << 24);
//
//				BlurVertex V[4] = { BlurVertex(D3DXVECTOR3(0.0f,0.0f,0.0f),1.0f,		alphaColor, 0,0) ,
//									BlurVertex(D3DXVECTOR3(wTextureSize,0.0f,0.0f),1.0f,		alphaColor, 1,0) ,
//									BlurVertex(D3DXVECTOR3(0.0f,wTextureSize,0.0f),1.0f,		alphaColor, 0,1) ,
//									BlurVertex(D3DXVECTOR3(wTextureSize,wTextureSize,0.0f),1.0f,	alphaColor, 1,1) };
//				STATEMANAGER.SetTexture(0,m_lpAccumTexture);
//				STATEMANAGER.SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE|D3DFVF_TEX1 );
//				STATEMANAGER.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,V,sizeof(BlurVertex));
//			}
//
//			{
//				STATEMANAGER.SetRenderTarget(m_lpAccumRenderTargetSurface, m_lpAccumDepthSurface);
//
//				BlurVertex V[4] = { BlurVertex(D3DXVECTOR3(0.0f,0.0f,0.0f),1.0f,		0xFFFFFF, 0,0) ,
//									BlurVertex(D3DXVECTOR3(wTextureSize,0.0f,0.0f),1.0f,		0xFFFFFF, 1,0) ,
//									BlurVertex(D3DXVECTOR3(0.0f,wTextureSize,0.0f),1.0f,		0xFFFFFF, 0,1) ,
//									BlurVertex(D3DXVECTOR3(wTextureSize,wTextureSize,0.0f),1.0f,	0xFFFFFF, 1,1) };
//
//				STATEMANAGER.SetTexture(0,m_lpSnowTexture);
//				STATEMANAGER.SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE);
//				STATEMANAGER.SetVertexShader( D3DFVF_XYZRHW | D3DFVF_DIFFUSE|D3DFVF_TEX1 );
//				STATEMANAGER.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,V,sizeof(BlurVertex));
//			}

	///////////////
	{
#ifdef ENABLE_DIRECTX9_UPDATE
        ms_lpd3dDevice->SetDepthStencilSurface(m_lpOldDepthStencilSurface);
        ms_lpd3dDevice->SetRenderTarget(0, m_lpOldDepthStencilSurface);
#else
        ms_lpd3dDevice->SetRenderTarget(m_lpOldSurface, m_lpOldDepthStencilSurface);
#endif

		STATEMANAGER.SetTexture(0,m_lpSnowTexture);
		STATEMANAGER.SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE);

		D3DSURFACE_DESC	desc;
		m_lpOldSurface->GetDesc(&desc);
		float sx = (float)desc.Width ;
		float sy = (float)desc.Height;
		SAFE_RELEASE( m_lpOldSurface );
		SAFE_RELEASE( m_lpOldDepthStencilSurface );

		BlurVertex V[4] = {	BlurVertex(D3DXVECTOR3(0.0f,0.0f,0.0f),1.0f	,0xFFFFFF, 0,0) ,
							BlurVertex(D3DXVECTOR3(sx,0.0f,0.0f),1.0f	,0xFFFFFF, 1,0) ,
							BlurVertex(D3DXVECTOR3(0.0f,sy,0.0f),1.0f	,0xFFFFFF, 0,1) ,
							BlurVertex(D3DXVECTOR3(sx,sy,0.0f),1.0f		,0xFFFFFF, 1,1) };

#ifdef ENABLE_DIRECTX9_UPDATE
        STATEMANAGER.SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
#else
        STATEMANAGER.SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
#endif

		STATEMANAGER.DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,2,V,sizeof(BlurVertex));
	}
}

void CSnowEnvironment::Render()
{
	if (!m_bSnowEnable)
	{
		if (m_kVct_pkParticleSnow.empty())
			return;
	}

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CSnowEnvironment::__BeginBlur **");
#endif

	__BeginBlur();

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif

	DWORD dwParticleCount = min(m_dwParticleMaxNum, m_kVct_pkParticleSnow.size());

	CCamera * pCamera = CCameraManager::Instance().GetCurrentCamera();
	if (!pCamera)
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CSnowEnvironment::Render **");
#endif

	const D3DXVECTOR3 & c_rv3Up = pCamera->GetUp();
	const D3DXVECTOR3 & c_rv3Cross = pCamera->GetCross();

	SParticleVertex * pv3Verticies;
#ifdef ENABLE_DIRECTX9_UPDATE
    if (SUCCEEDED(m_pVB->Lock(0
                                , sizeof(SParticleVertex) * dwParticleCount * 4
                                , (void**) &pv3Verticies
                                , D3DLOCK_DISCARD)))
#else
    if (SUCCEEDED(m_pVB->Lock(0, sizeof(SParticleVertex)*dwParticleCount*4, (BYTE **) &pv3Verticies, D3DLOCK_DISCARD)))
#endif
	{
		int i = 0;
		std::vector<CSnowParticle*>::iterator itor = m_kVct_pkParticleSnow.begin();
		for (; i < dwParticleCount && itor != m_kVct_pkParticleSnow.end(); ++i, ++itor)
		{
			CSnowParticle * pSnow = *itor;
			pSnow->SetCameraVertex(c_rv3Up, c_rv3Cross);
			pSnow->GetVerticies(pv3Verticies[i*4+0],
								pv3Verticies[i*4+1],
								pv3Verticies[i*4+2],
								pv3Verticies[i*4+3]);
		}
		m_pVB->Unlock();
	}

	STATEMANAGER.SaveRenderState(D3DRS_ZWRITEENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	STATEMANAGER.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	STATEMANAGER.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTexture(1, nullptr);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	if (!m_pImageInstance || !m_pImageInstance->GetGraphicImagePointer())
	{
		STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
		STATEMANAGER.RestoreRenderState(D3DRS_ZWRITEENABLE);
		STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
		return;
	}

	m_pImageInstance->GetGraphicImagePointer()->GetTextureReference().SetTextureStage(0);
	STATEMANAGER.SetIndices(m_pIB, 0);
	STATEMANAGER.SetStreamSource(0, m_pVB, sizeof(SParticleVertex));

#ifdef ENABLE_DIRECTX9_UPDATE
    STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
#else
    STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX1);
#endif

	STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, dwParticleCount*4, 0, dwParticleCount*2);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ZWRITEENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();

    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CSnowEnvironment::__ApplyBlur **");
#endif

	__ApplyBlur();

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

bool CSnowEnvironment::__CreateBlurTexture()
{
    if (!m_bBlurEnable)
    {
        return true;
    }

#ifdef ENABLE_DIRECTX9_UPDATE
    if (FAILED(ms_lpd3dDevice->CreateTexture(m_wBlurTextureSize
                                                , m_wBlurTextureSize
                                                , 1
                                                , D3DUSAGE_RENDERTARGET
                                                , D3DFMT_X8R8G8B8
                                                , D3DPOOL_DEFAULT
                                                , &m_lpSnowTexture
                                                , nullptr)))
#else
    if (FAILED(ms_lpd3dDevice->CreateTexture(m_wBlurTextureSize, m_wBlurTextureSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_lpSnowTexture)))
#endif
    {
        return false;
    }

    if (FAILED(m_lpSnowTexture->GetSurfaceLevel(0, &m_lpSnowRenderTargetSurface)))
    {
        return false;
    }

#ifdef ENABLE_DIRECTX9_UPDATE
    if (FAILED(ms_lpd3dDevice->CreateDepthStencilSurface(m_wBlurTextureSize
                                                            , m_wBlurTextureSize
                                                            , D3DFMT_D16
                                                            , D3DMULTISAMPLE_NONE
                                                            , 0
                                                            , TRUE
                                                            , &m_lpSnowDepthSurface
                                                            , nullptr)))
#else
    if (FAILED(ms_lpd3dDevice->CreateDepthStencilSurface(m_wBlurTextureSize, m_wBlurTextureSize, D3DFMT_D16, D3DMULTISAMPLE_NONE, &m_lpSnowDepthSurface)))
#endif
    {
        return false;
    }

#ifdef ENABLE_DIRECTX9_UPDATE
    if (FAILED(ms_lpd3dDevice->CreateTexture(m_wBlurTextureSize
                                                , m_wBlurTextureSize
                                                , 1
                                                , D3DUSAGE_RENDERTARGET
                                                , D3DFMT_X8R8G8B8
                                                , D3DPOOL_DEFAULT
                                                , &m_lpAccumTexture
                                                , nullptr)))
#else
    if (FAILED(ms_lpd3dDevice->CreateTexture(m_wBlurTextureSize, m_wBlurTextureSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_lpAccumTexture)))
#endif
    {
        return false;
    }

    if (FAILED(m_lpAccumTexture->GetSurfaceLevel(0, &m_lpAccumRenderTargetSurface)))
    {
        return false;
    }

#ifdef ENABLE_DIRECTX9_UPDATE
    if (FAILED(ms_lpd3dDevice->CreateDepthStencilSurface(m_wBlurTextureSize
                                                            , m_wBlurTextureSize
                                                            , D3DFMT_D16
                                                            , D3DMULTISAMPLE_NONE
                                                            , 0
                                                            , TRUE
                                                            , &m_lpAccumDepthSurface
                                                            , nullptr)))
#else
    if (FAILED(ms_lpd3dDevice->CreateDepthStencilSurface(m_wBlurTextureSize, m_wBlurTextureSize, D3DFMT_D16, D3DMULTISAMPLE_NONE, &m_lpAccumDepthSurface)))
#endif
    {
        return false;
    }

    return true;
}

bool CSnowEnvironment::__CreateGeometry()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    if (FAILED(ms_lpd3dDevice->CreateVertexBuffer(sizeof(SParticleVertex) * m_dwParticleMaxNum * 4
                                                    , D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY
                                                    , D3DFVF_XYZ | D3DFVF_TEX1
                                                    , D3DPOOL_SYSTEMMEM
                                                    , &m_pVB
                                                    , nullptr)))
#else
	if (FAILED(ms_lpd3dDevice->CreateVertexBuffer(sizeof(SParticleVertex)*m_dwParticleMaxNum*4,
											D3DUSAGE_DYNAMIC|D3DUSAGE_WRITEONLY,
											D3DFVF_XYZ | D3DFVF_TEX1,
											D3DPOOL_SYSTEMMEM,
											&m_pVB)))
#endif
    {
        return false;
    }

#ifdef DirectX9Ex
    if (FAILED(ms_lpd3dDevice->CreateIndexBuffer(sizeof(WORD) * m_dwParticleMaxNum * 6
                                                    , D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC
                                                    , D3DFMT_INDEX16
                                                    , D3DPOOL_DEFAULT
                                                    , &m_pIB
                                                    , nullptr)))
#elif defined(ENABLE_DIRECTX9_UPDATE)
    if (FAILED(ms_lpd3dDevice->CreateIndexBuffer(sizeof(WORD) * m_dwParticleMaxNum * 6
                                                    , D3DUSAGE_WRITEONLY
                                                    , D3DFMT_INDEX16
                                                    , D3DPOOL_MANAGED
                                                    , &m_pIB
                                                    , nullptr)))
#else
	if (FAILED(ms_lpd3dDevice->CreateIndexBuffer(sizeof(WORD)*m_dwParticleMaxNum*6,
										   D3DUSAGE_WRITEONLY,
										   D3DFMT_INDEX16,
										   D3DPOOL_MANAGED,
										   &m_pIB)))
#endif
    {
        return false;
    }

	WORD* dstIndices;
#ifdef ENABLE_DIRECTX9_UPDATE
    if (FAILED(m_pIB->Lock(0
                            , sizeof(WORD) * m_dwParticleMaxNum * 6
                            , (void**)&dstIndices
                            , 0)))
#else
	if (FAILED(m_pIB->Lock(0, sizeof(WORD)*m_dwParticleMaxNum*6, (BYTE**)&dstIndices, 0)))
#endif
    {
        return false;
    }

	const WORD c_awFillRectIndices[6] = { 0, 2, 1, 2, 3, 1, };
	for (int i = 0; i < m_dwParticleMaxNum; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			dstIndices[i*6 + j] = i*4 + c_awFillRectIndices[j];
		}
	}

	m_pIB->Unlock();
	return true;
}

bool CSnowEnvironment::Create()
{
	Destroy();

	if (!__CreateBlurTexture())
		return false;

	if (!__CreateGeometry())
		return false;

	m_pSnowImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer("d:/ymir work/special/snow.dds");
	m_pRainImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer("d:/ymir work/special/rain.dds");

	m_pImageInstance = CGraphicImageInstance::New();
	if (m_eWeatherMode == WEATHER_MODE_RAIN && m_pRainImage)
	{
		m_pImageInstance->SetImagePointer(m_pRainImage);
	}
	else
	{
		m_pImageInstance->SetImagePointer(m_pSnowImage);
	}

	return true;
}

void CSnowEnvironment::Destroy()
{
	SAFE_RELEASE(m_lpSnowTexture);
	SAFE_RELEASE(m_lpSnowRenderTargetSurface);
	SAFE_RELEASE(m_lpSnowDepthSurface);
	SAFE_RELEASE(m_lpAccumTexture);
	SAFE_RELEASE(m_lpAccumRenderTargetSurface);
	SAFE_RELEASE(m_lpAccumDepthSurface);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);

	stl_wipe(m_kVct_pkParticleSnow);
	CSnowParticle::DestroyPool();

	if (m_pImageInstance)
	{
		CGraphicImageInstance::Delete(m_pImageInstance);
		m_pImageInstance = nullptr;
	}

	__Initialize();
}

void CSnowEnvironment::__Initialize()
{
	m_bSnowEnable = FALSE;
	m_eWeatherMode = WEATHER_MODE_NONE;
	m_iRainLevel = 0;
	m_pSnowImage = nullptr;
	m_pRainImage = nullptr;
	m_lpSnowTexture = nullptr;
	m_lpSnowRenderTargetSurface = nullptr;
	m_lpSnowDepthSurface = nullptr;
	m_lpAccumTexture = nullptr;
	m_lpAccumRenderTargetSurface = nullptr;
	m_lpAccumDepthSurface = nullptr;
	m_pVB = nullptr;
	m_pIB = nullptr;
	m_pImageInstance = nullptr;

	m_kVct_pkParticleSnow.reserve(m_dwParticleMaxNum);
}

CSnowEnvironment::CSnowEnvironment()
{
	m_bBlurEnable = FALSE;
	m_dwParticleMaxNum = 3000;
	m_wBlurTextureSize = 512;

	__Initialize();
}
CSnowEnvironment::~CSnowEnvironment()
{
	Destroy();
}
