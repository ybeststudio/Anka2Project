#include "StdAfx.h"
#include "EffectInstance.h"
#include "ParticleSystemInstance.h"
#include "SimpleLightInstance.h"

#include "../eterBase/Stl.h"
#include "../eterLib/StateManager.h"
#include "../MilesLib/SoundManager.h"
#ifdef USE_EFFECTS_LOD
#include "../EterLib/Camera.h"
#endif

CDynamicPool<CEffectInstance>	CEffectInstance::ms_kPool;
int CEffectInstance::ms_iRenderingEffectCount = 0;

bool CEffectInstance::LessRenderOrder(CEffectInstance* pkEftInst)
{
	return (m_pkEftData<pkEftInst->m_pkEftData);
}

void CEffectInstance::ResetRenderingEffectCount()
{
	ms_iRenderingEffectCount = 0;
}

int CEffectInstance::GetRenderingEffectCount()
{
	return ms_iRenderingEffectCount;
}

CEffectInstance* CEffectInstance::New()
{
	CEffectInstance* pkEftInst=ms_kPool.Alloc();
	return pkEftInst;
}

void CEffectInstance::Delete(CEffectInstance* pkEftInst)
{
	pkEftInst->Clear();
	ms_kPool.Free(pkEftInst);
}

void CEffectInstance::DestroySystem()
{
	ms_kPool.Destroy();

	CParticleSystemInstance::DestroySystem();
	CEffectMeshInstance::DestroySystem();
	CLightInstance::DestroySystem();
}

void CEffectInstance::UpdateSound()
{
	if (m_pSoundInstanceVector)
	{
		CSoundManager& rkSndMgr=CSoundManager::Instance();
		rkSndMgr.UpdateSoundInstance(m_matGlobal._41, m_matGlobal._42, m_matGlobal._43, m_dwFrame, m_pSoundInstanceVector);
	}
	++m_dwFrame;
}

struct FEffectUpdator
{
	BOOL isAlive;
	float fElapsedTime;
	FEffectUpdator(float fElapsedTime)
		: isAlive(FALSE), fElapsedTime(fElapsedTime)
	{
	}
	void operator () (CEffectElementBaseInstance * pInstance)
	{
		if (pInstance->Update(fElapsedTime))
			isAlive = TRUE;
	}
};

#ifdef USE_EFFECTS_LOD
void CEffectInstance::SetHiddenByLod()
{
    for (auto &particle : m_ParticleInstanceVector)
    {
        particle->SetHiddenByLod();
    }

    for (auto &mesh : m_MeshInstanceVector)
    {
        mesh->SetHiddenByLod();
    }

    for (auto &light : m_LightInstanceVector)
    {
        light->SetHiddenByLod();
    }
}

void CEffectInstance::SetLodShow()
{
    for (auto &particle : m_ParticleInstanceVector)
    {
        particle->SetShownByLod();
    }

    for (auto &mesh : m_MeshInstanceVector)
    {
        mesh->SetShownByLod();
    }

    for (auto &light : m_LightInstanceVector)
    {
        light->SetShownByLod();
    }
}

bool CEffectInstance::IsHiddenByLod()
{
    bool anyActive = false;

    for (auto &particle : m_ParticleInstanceVector)
    {
        anyActive = particle->IsHiddenByLod();
    }

    for (auto &mesh : m_MeshInstanceVector)
    {
        anyActive = mesh->IsHiddenByLod();
    }

    for (auto &light : m_LightInstanceVector)
    {
        anyActive = light->IsHiddenByLod();
    }

    return anyActive;
}

void CEffectInstance::UpdateLODLevel()
{
    auto cameraInst = CCameraManager::Instance().GetCurrentCamera();
    if (!cameraInst)
    {
        TraceError("CEffectInstance::UpdateLODLevel - GetCurrentCamera() == NULL");
        return;
    }

    const D3DXVECTOR3& c_rv3Eye = cameraInst->GetEye();
    const D3DXVECTOR3& c_rv3Pos = D3DXVECTOR3(m_matGlobal._41, m_matGlobal._42, m_matGlobal._43);

    auto dist = (c_rv3Eye - c_rv3Pos);
    float fdist = D3DXVec3Length(&dist);
    auto lod = std::abs((1.0f * fdist * tan (0.5f * GetFOV())) / (0.5f * (float)ms_iHeight));

    if (lod < 20.0f)
    {
        SetLodShow();
    }
    else
    {
        SetHiddenByLod();
    }
}
#endif


// Conservative view test on the effect origin, padded by a radius so large effects whose origin
// is just off-screen still count as visible. Clip space: inside when |x|,|y| <= w (+ margin).
bool CEffectInstance::__IsOffscreenOrFar(bool* pbFar) const
{
	*pbFar = false;
#ifdef ENABLE_RENDER_TARGET
	if (m_ignoreFrustum)
		return false;
#endif
#ifdef ENABLE_WIKI_SYSTEM
	if (m_wikiIgnoreFrustum)
		return false;
#endif
	const float fRadius = (std::max)(1500.0f, m_fBoundingSphereRadius);
	D3DXMATRIX matViewProj;
	D3DXMatrixMultiply(&matViewProj, &ms_matView, &ms_matProj);
	const D3DXVECTOR3 v3Pos(m_matGlobal._41, m_matGlobal._42, m_matGlobal._43);
	D3DXVECTOR4 v4Clip;
	D3DXVec3Transform(&v4Clip, &v3Pos, &matViewProj);
	const float fMarginX = fRadius * fabsf(ms_matProj._11);
	const float fMarginY = fRadius * fabsf(ms_matProj._22);
	if (v4Clip.w < -fRadius ||
		v4Clip.x > v4Clip.w + fMarginX || v4Clip.x < -v4Clip.w - fMarginX ||
		v4Clip.y > v4Clip.w + fMarginY || v4Clip.y < -v4Clip.w - fMarginY)
		return true;
	*pbFar = v4Clip.w > 3000.0f;
	return false;
}

void CEffectInstance::OnUpdate()
{
	Transform();

	// Off-screen effects are still simulated every frame (skipping updates gave particle systems
	// large time steps and produced dark/opaque quads in crowds); only their draw is skipped.
	{
		bool bFar = false;
		m_bOffscreen = __IsOffscreenOrFar(&bFar);
	}

#ifdef USE_EFFECTS_LOD
    UpdateLODLevel();
#endif

#ifdef WORLD_EDITOR
    FEffectUpdator f(CTimer::Instance().GetElapsedSecond());
#else
    FEffectUpdator f(CTimer::Instance().GetCurrentSecond()-m_fLastTime);
#endif

    f = std::for_each(m_ParticleInstanceVector.begin(), m_ParticleInstanceVector.end(),f);
    f = std::for_each(m_MeshInstanceVector.begin(), m_MeshInstanceVector.end(),f);
    f = std::for_each(m_LightInstanceVector.begin(), m_LightInstanceVector.end(),f);
    m_isAlive = f.isAlive;

    if (m_pSoundInstanceVector)
    {
        auto& sndMgr = CSoundManager::Instance();
        sndMgr.UpdateSoundInstance(m_matGlobal._41
                                    , m_matGlobal._42
                                    , m_matGlobal._43
                                    , m_dwFrame
                                    , m_pSoundInstanceVector);
    }

    m_fLastTime = CTimer::Instance().GetCurrentSecond();
    ++m_dwFrame;
}

void CEffectInstance::OnRender()
{
	if (m_bOffscreen)
		return;

#if defined(ENABLE_DIRECTX9_UPDATE) || defined(DIRECTX9)
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
#else
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_NONE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_NONE);
#endif
	STATEMANAGER.SaveRenderState(D3DRS_LIGHTING, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_FOGENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	STATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	STATEMANAGER.SaveRenderState(D3DRS_ZWRITEENABLE, FALSE);
	/////

#if defined(ENABLE_DIRECTX9_UPDATE) || defined(DIRECTX9)
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	STATEMANAGER.SaveFVF(D3DFVF_XYZ | D3DFVF_TEX1);
#else
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
	STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_TEX1);
#endif

	std::for_each(m_ParticleInstanceVector.begin(), m_ParticleInstanceVector.end(), std::mem_fn(&CEffectElementBaseInstance::Render));
	std::for_each(m_MeshInstanceVector.begin(), m_MeshInstanceVector.end(), std::mem_fn(&CEffectElementBaseInstance::Render));

	/////
#if defined(ENABLE_DIRECTX9_UPDATE) || defined(DIRECTX9)
	STATEMANAGER.RestoreFVF();
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MINFILTER);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MAGFILTER);
#else
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_MINFILTER);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_MAGFILTER);
#endif
	STATEMANAGER.RestoreRenderState(D3DRS_ZWRITEENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_FOGENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_LIGHTING);

#if defined(ENABLE_DIRECTX9_UPDATE) || defined(DIRECTX9)
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG2);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAARG1);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAARG2);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAOP);
#endif

	++ms_iRenderingEffectCount;
}

void CEffectInstance::SetGlobalMatrix(const D3DXMATRIX & c_rmatGlobal)
{
	m_matGlobal = c_rmatGlobal;
}

BOOL CEffectInstance::isAlive()
{
	return m_isAlive;
}

void CEffectInstance::SetActive()
{
	std::for_each(
		m_ParticleInstanceVector.begin(),
		m_ParticleInstanceVector.end(),
		std::mem_fn(&CEffectElementBaseInstance::SetActive));
	std::for_each(
		m_MeshInstanceVector.begin(),
		m_MeshInstanceVector.end(),
		std::mem_fn(&CEffectElementBaseInstance::SetActive));
	std::for_each(
		m_LightInstanceVector.begin(),
		m_LightInstanceVector.end(),
		std::mem_fn(&CEffectElementBaseInstance::SetActive));
}

void CEffectInstance::SetDeactive()
{
	std::for_each(
		m_ParticleInstanceVector.begin(),
		m_ParticleInstanceVector.end(),
		std::mem_fn(&CEffectElementBaseInstance::SetDeactive));
	std::for_each(
		m_MeshInstanceVector.begin(),
		m_MeshInstanceVector.end(),
		std::mem_fn(&CEffectElementBaseInstance::SetDeactive));
	std::for_each(
		m_LightInstanceVector.begin(),
		m_LightInstanceVector.end(),
		std::mem_fn(&CEffectElementBaseInstance::SetDeactive));
}

void CEffectInstance::__SetParticleData(CParticleSystemData * pData)
{
	CParticleSystemInstance * pInstance = CParticleSystemInstance::New();
	pInstance->SetDataPointer(pData);
	pInstance->SetLocalMatrixPointer(&m_matGlobal);
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	pInstance->SetParticleScale(GetParticleScale());
#endif

	m_ParticleInstanceVector.push_back(pInstance);
}

void CEffectInstance::__SetMeshData(CEffectMeshScript * pMesh)
{
	CEffectMeshInstance * pMeshInstance = CEffectMeshInstance::New();
	pMeshInstance->SetDataPointer(pMesh);
	pMeshInstance->SetLocalMatrixPointer(&m_matGlobal);
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	pMeshInstance->SetMeshScale(GetMeshScale());
#endif

	m_MeshInstanceVector.push_back(pMeshInstance);
}

void CEffectInstance::__SetLightData(CLightData* pData)
{
	CLightInstance * pInstance = CLightInstance::New();
	pInstance->SetDataPointer(pData);
	pInstance->SetLocalMatrixPointer(&m_matGlobal);

	m_LightInstanceVector.push_back(pInstance);
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
void CEffectInstance::SetEffectDataPointer(CEffectData * pEffectData, DWORD * dwSkillColor, DWORD EffectID)
#else
void CEffectInstance::SetEffectDataPointer(CEffectData * pEffectData)
#endif
{
	m_isAlive=true;

	m_pkEftData=pEffectData;

	m_fLastTime = CTimer::Instance().GetCurrentSecond();
	m_fBoundingSphereRadius = pEffectData->GetBoundingSphereRadius();
	m_v3BoundingSpherePosition = pEffectData->GetBoundingSpherePosition();

	if (m_fBoundingSphereRadius > 0.0f)
		CGraphicObjectInstance::RegisterBoundingSphere();

	DWORD i;

	for (i = 0; i < pEffectData->GetParticleCount(); ++i)
	{
		CParticleSystemData * pParticle = pEffectData->GetParticlePointer(i);

#if defined(ENABLE_SKILL_COLOR_SYSTEM) && !defined(WORLD_EDITOR)
		if (dwSkillColor != nullptr)
		{
			DWORD skill;

			if (i >= 5)
				skill = dwSkillColor[4];
			else
				skill = dwSkillColor[i];

			CParticleProperty* prob = pParticle->GetParticlePropertyPointer();
			if (skill != 99999999 && skill != 0)
			{
				D3DXCOLOR c = D3DXCOLOR(skill);
				D3DXCOLOR d;
				for (TTimeEventTableColor::iterator it = prob->m_TimeEventColor.begin(); it != prob->m_TimeEventColor.end(); ++it)
				{
					d = D3DXCOLOR(it->m_Value.m_dwColor);
					c.a = d.a;
					it->m_Value.m_dwColor = (DWORD)c;
				}
			}
			else if (skill == 0)
			{
				CParticleProperty* prob_backup = pParticle->GetParticlePropertyBackupPointer();
				for (TTimeEventTableColor::iterator it = prob->m_TimeEventColor.begin(); it != prob->m_TimeEventColor.end(); ++it)
					it->m_Value.m_dwColor = prob_backup->m_TimeEventColor[distance(prob->m_TimeEventColor.begin(), it)].m_Value.m_dwColor;
			}
		}
#endif

		__SetParticleData(pParticle);
	}

	for (i = 0; i < pEffectData->GetMeshCount(); ++i)
	{
		CEffectMeshScript * pMesh = pEffectData->GetMeshPointer(i);

		__SetMeshData(pMesh);
	}

	for (i = 0; i < pEffectData->GetLightCount(); ++i)
	{
		CLightData * pLight = pEffectData->GetLightPointer(i);

		__SetLightData(pLight);
	}

	m_pSoundInstanceVector = pEffectData->GetSoundInstanceVector();
}

bool CEffectInstance::GetBoundingSphere(D3DXVECTOR3 & v3Center, float & fRadius)
{
	v3Center.x = m_matGlobal._41 + m_v3BoundingSpherePosition.x;
	v3Center.y = m_matGlobal._42 + m_v3BoundingSpherePosition.y;
	v3Center.z = m_matGlobal._43 + m_v3BoundingSpherePosition.z;
	fRadius = m_fBoundingSphereRadius;
	return true;
}

void CEffectInstance::Clear()
{
	if (!m_ParticleInstanceVector.empty())
	{
		std::for_each(m_ParticleInstanceVector.begin(), m_ParticleInstanceVector.end(), CParticleSystemInstance::Delete);
		m_ParticleInstanceVector.clear();
	}

	if (!m_MeshInstanceVector.empty())
	{
		std::for_each(m_MeshInstanceVector.begin(), m_MeshInstanceVector.end(), CEffectMeshInstance::Delete);
		m_MeshInstanceVector.clear();
	}

	if (!m_LightInstanceVector.empty())
	{
		std::for_each(m_LightInstanceVector.begin(), m_LightInstanceVector.end(), CLightInstance::Delete);
		m_LightInstanceVector.clear();
	}

	__Initialize();
}

void CEffectInstance::__Initialize()
{
	m_isAlive = FALSE;
	m_dwFrame = 0;
	m_bOffscreen = false;
#ifdef ENABLE_WIKI_SYSTEM
	m_wikiIgnoreFrustum = false;
#endif
#ifdef ENABLE_RENDER_TARGET
	m_ignoreFrustum = false;
#endif
	m_pSoundInstanceVector = nullptr;
	m_fBoundingSphereRadius = 0.0f;
	m_v3BoundingSpherePosition.x = m_v3BoundingSpherePosition.y = m_v3BoundingSpherePosition.z = 0.0f;

	m_pkEftData = nullptr;
#ifdef __ENABLE_STEALTH_FIX__
	m_isAlwaysHidden = false;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	m_fParticleScale = 1.0f;
	m_v3MeshScale.x = m_v3MeshScale.y = m_v3MeshScale.z = 1.0f;
#endif

	D3DXMatrixIdentity(&m_matGlobal);
}

CEffectInstance::CEffectInstance()
{
	__Initialize();
}
CEffectInstance::~CEffectInstance()
{
	assert(m_ParticleInstanceVector.empty());
	assert(m_MeshInstanceVector.empty());
	assert(m_LightInstanceVector.empty());
}

void CEffectInstance::SetOptionColorTint(const D3DXCOLOR& color)
{
    for (auto* particle : m_ParticleInstanceVector) particle->SetOptionColorTint(color);
}
