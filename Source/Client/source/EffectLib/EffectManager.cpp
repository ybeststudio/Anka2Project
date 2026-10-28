#include "StdAfx.h"
#include "../eterBase/Random.h"
#include "../eterlib/StateManager.h"
#include "EffectManager.h"

void CEffectManager::GetInfo(std::string* pstInfo)
{
	char szInfo[256];

	sprintf(szInfo, "Effect: Inst - ED %d, EI %d Pool - PSI %d, MI %d, LI %d, PI %d, EI %d, ED %d, PSD %d, EM %d, LD %d",
		m_kEftDataMap.size(),
		m_kEftInstMap.size(),
		CParticleSystemInstance::ms_kPool.GetCapacity(),
		CEffectMeshInstance::ms_kPool.GetCapacity(),
		CLightInstance::ms_kPool.GetCapacity(),
		CParticleInstance::ms_kPool.GetCapacity(),
		CEffectInstance::ms_kPool.GetCapacity(),
		CEffectData::ms_kPool.GetCapacity(),
		CParticleSystemData::ms_kPool.GetCapacity(),
		CEffectMeshScript::ms_kPool.GetCapacity(),
		CLightData::ms_kPool.GetCapacity()
	);
	pstInfo->append(szInfo);
}

void CEffectManager::UpdateSound()
{
	for (TEffectInstanceMap::iterator itor = m_kEftInstMap.begin(); itor != m_kEftInstMap.end(); ++itor)
	{
		CEffectInstance * pEffectInstance = itor->second;

		pEffectInstance->UpdateSound();
	}
}

bool CEffectManager::IsAliveEffect(DWORD dwInstanceIndex)
{
	TEffectInstanceMap::iterator f = m_kEftInstMap.find(dwInstanceIndex);
	if (m_kEftInstMap.end()==f)
		return false;

	return f->second->isAlive() ? true : false;
}

#ifdef __ENABLE_STEALTH_FIX__
void CEffectManager::ApplyAlwaysHidden()
{
	if (!m_pSelectedEffectInstance)
		return;

	m_pSelectedEffectInstance->ApplyAlwaysHidden();
}


void CEffectManager::ReleaseAlwaysHidden()
{
	if (!m_pSelectedEffectInstance)
		return;

	m_pSelectedEffectInstance->ReleaseAlwaysHidden();
}
#endif

void CEffectManager::Update()
{
	for (TEffectInstanceMap::iterator itor = m_kEftInstMap.begin(); itor != m_kEftInstMap.end();)
	{
		CEffectInstance * pEffectInstance = itor->second;

		pEffectInstance->Update();

		if (!pEffectInstance->isAlive())
		{
			m_optionFiltered.erase(itor->first);
			itor = m_kEftInstMap.erase(itor);
			CEffectInstance::Delete(pEffectInstance);
		}
		else
		{
			++itor;
		}
	}
}

struct CEffectManager_LessEffectInstancePtrRenderOrder
{
	bool operator() (CEffectInstance* pkLeft, CEffectInstance* pkRight)
	{
		return pkLeft->LessRenderOrder(pkRight);
	}
};

struct CEffectManager_FEffectInstanceRender
{
	inline void operator () (CEffectInstance * pkEftInst)
	{
		pkEftInst->Render();
	}
};

void CEffectManager::Render()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(251, 50, 50, 0), L"** CEffectManager::Render **");
#endif
	STATEMANAGER.SetVertexShader(nullptr);
	STATEMANAGER.SetPixelShader(nullptr);

	if (m_isDisableSortRendering)
	{
		for (TEffectInstanceMap::iterator itor = m_kEftInstMap.begin(); itor != m_kEftInstMap.end();)
		{
			CEffectInstance * pEffectInstance = itor->second;
			if (!m_optionFiltered.count(itor->first)) pEffectInstance->Render();
			++itor;
		}
	}
	else
	{
		static std::vector<CEffectInstance*> s_kVct_pkEftInstSort;
		s_kVct_pkEftInstSort.clear();

		TEffectInstanceMap& rkMap_pkEftInstSrc=m_kEftInstMap;
		TEffectInstanceMap::iterator i;
		for (i=rkMap_pkEftInstSrc.begin(); i!=rkMap_pkEftInstSrc.end(); ++i)
			if (!m_optionFiltered.count(i->first)) s_kVct_pkEftInstSort.push_back(i->second);

		std::sort(s_kVct_pkEftInstSort.begin(), s_kVct_pkEftInstSort.end(), CEffectManager_LessEffectInstancePtrRenderOrder());
		std::for_each(s_kVct_pkEftInstSort.begin(), s_kVct_pkEftInstSort.end(), CEffectManager_FEffectInstanceRender());
	}
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

#ifdef ENABLE_RENDER_TARGET
void CEffectManager::RenderOne(DWORD id)
{
	STATEMANAGER.SetTexture(0, nullptr);
	STATEMANAGER.SetTexture(1, nullptr);

	const auto& pEffectInstance = m_kEftInstMap.find(id);

	if (pEffectInstance != m_kEftInstMap.end())
	{
		pEffectInstance->second->SetIgnoreFrustum(true);
		pEffectInstance->second->Show();
		pEffectInstance->second->Render();
	}
	else
		TraceError("!RenderOne, not found");
}
#endif

BOOL CEffectManager::RegisterEffect(const char *c_szFileName, bool isExistDelete, bool isNeedCache
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	, const char *name
#endif
)
{
	std::string strFileName;
	StringPath(c_szFileName, strFileName);

	DWORD dwCRC = GetCaseCRC32(strFileName.c_str(), strFileName.length()
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		, name
#endif
	);

	TEffectDataMap::iterator itor = m_kEftDataMap.find(dwCRC);
	if (m_kEftDataMap.end() != itor)
	{
		if (isExistDelete)
		{
			CEffectData* pkEftData=itor->second;
			CEffectData::Delete(pkEftData);
			m_kEftDataMap.erase(itor);
		}
		else
		{
			return TRUE;
		}
	}

	CEffectData * pkEftData = CEffectData::New();

	if (!pkEftData->LoadScript(c_szFileName))
	{
		CEffectData::Delete(pkEftData);
		return FALSE;
	}

	m_kEftDataMap.insert(TEffectDataMap::value_type(dwCRC, pkEftData));

	if (isNeedCache)
	{
		if (m_kEftCacheMap.find(dwCRC)==m_kEftCacheMap.end())
		{
			CEffectInstance* pkNewEftInst=CEffectInstance::New();
			pkNewEftInst->SetEffectDataPointer(pkEftData);
			m_kEftCacheMap.insert(TEffectInstanceMap::value_type(dwCRC, pkNewEftInst));
		}
	}

	return TRUE;
}

BOOL CEffectManager::RegisterEffect2(const char *c_szFileName, DWORD* pdwRetCRC, bool isNeedCache
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	, const char *name
#endif
)
{
	std::string strFileName;
	StringPath(c_szFileName, strFileName);

	DWORD dwCRC = GetCaseCRC32(strFileName.c_str(), strFileName.length()
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		, name
#endif
	);

	*pdwRetCRC = dwCRC;

	return RegisterEffect(c_szFileName, false, isNeedCache
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		, name
#endif
	);
}

int CEffectManager::CreateEffect(const char *c_szFileName, const D3DXVECTOR3 & c_rv3Position, const D3DXVECTOR3 & c_rv3Rotation)
{
	DWORD dwID = GetCaseCRC32(c_szFileName, strlen(c_szFileName));
	return CreateEffect(dwID, c_rv3Position, c_rv3Rotation);
}

int CEffectManager::CreateEffect(DWORD dwID, const D3DXVECTOR3 & c_rv3Position, const D3DXVECTOR3 & c_rv3Rotation
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	,DWORD *dwSkillColor
#endif
)
{
	int iInstanceIndex = GetEmptyIndex();

	CreateEffectInstance(iInstanceIndex, dwID
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		, dwSkillColor
#endif
	);

	SelectEffectInstance(iInstanceIndex);
	D3DXMATRIX mat;
	D3DXMatrixRotationYawPitchRoll(&mat,D3DXToRadian(c_rv3Rotation.x),D3DXToRadian(c_rv3Rotation.y),D3DXToRadian(c_rv3Rotation.z));
	mat._41 = c_rv3Position.x;
	mat._42 = c_rv3Position.y;
	mat._43 = c_rv3Position.z;
	SetEffectInstanceGlobalMatrix(mat);

	return iInstanceIndex;
}

void CEffectManager::CreateEffectInstance(DWORD dwInstanceIndex, DWORD dwID
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	, DWORD* dwSkillColor
#endif
)
{
	if (!dwID)
		return;

	CEffectData * pEffect;
	if (!GetEffectData(dwID, &pEffect))
	{
		Tracef("CEffectManager::CreateEffectInstance - NO DATA :%d\n", dwID);
		return;
	}

	CEffectInstance * pEffectInstance = CEffectInstance::New();

	pEffectInstance->SetEffectDataPointer(pEffect
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		, dwSkillColor, dwID
#endif
	);

	m_kEftInstMap.insert(TEffectInstanceMap::value_type(dwInstanceIndex, pEffectInstance));
}

bool CEffectManager::DestroyEffectInstance(DWORD dwInstanceIndex)
{
	TEffectInstanceMap::iterator itor = m_kEftInstMap.find(dwInstanceIndex);

	if (itor == m_kEftInstMap.end())
		return false;

	CEffectInstance * pEffectInstance = itor->second;

	m_optionFiltered.erase(dwInstanceIndex);
	m_kEftInstMap.erase(itor);

	CEffectInstance::Delete(pEffectInstance);

	return true;
}

#ifdef ENABLE_GRAPHIC_ON_OFF
void CEffectManager::ActiveEffectInstance(DWORD dwInstanceIndex)
{
	TEffectInstanceMap::iterator itor = m_kEftInstMap.find(dwInstanceIndex);

	if (itor == m_kEftInstMap.end())
		return;

	CEffectInstance* pEffectInstance = itor->second;
	pEffectInstance->SetActive();
}
#endif

void CEffectManager::DeactiveEffectInstance(DWORD dwInstanceIndex)
{
	TEffectInstanceMap::iterator itor = m_kEftInstMap.find(dwInstanceIndex);

	if (itor == m_kEftInstMap.end())
		return;

	CEffectInstance * pEffectInstance = itor->second;
	pEffectInstance->SetDeactive();
}

void CEffectManager::CreateUnsafeEffectInstance(DWORD dwEffectDataID, CEffectInstance ** ppEffectInstance)
{
	CEffectData * pEffect;
	if (!GetEffectData(dwEffectDataID, &pEffect))
	{
		Tracef("CEffectManager::CreateEffectInstance - NO DATA :%d\n", dwEffectDataID);
		return;
	}

	CEffectInstance* pkEftInstNew=CEffectInstance::New();
	pkEftInstNew->SetEffectDataPointer(pEffect);

	*ppEffectInstance = pkEftInstNew;
}

bool CEffectManager::DestroyUnsafeEffectInstance(CEffectInstance * pEffectInstance)
{
	if (!pEffectInstance)
		return false;

	CEffectInstance::Delete(pEffectInstance);

	return true;
}

BOOL CEffectManager::SelectEffectInstance(DWORD dwInstanceIndex)
{
	TEffectInstanceMap::iterator itor = m_kEftInstMap.find(dwInstanceIndex);

	m_pSelectedEffectInstance = nullptr;

	if (m_kEftInstMap.end() == itor)
		return FALSE;

	m_pSelectedEffectInstance = itor->second;

	return TRUE;
}

void CEffectManager::SetEffectTextures(DWORD dwID, std::vector<std::string> textures)
{
	CEffectData * pEffectData;
	if (!GetEffectData(dwID, &pEffectData))
	{
		Tracef("CEffectManager::CreateEffectInstance - NO DATA :%d\n", dwID);
		return;
	}

	for(DWORD i = 0; i < textures.size(); i++)
	{
		CParticleSystemData * pParticle = pEffectData->GetParticlePointer(i);
		pParticle->ChangeTexture(textures.at(i).c_str());
	}
}

void CEffectManager::SetEffectInstancePosition(const D3DXVECTOR3 & c_rv3Position)
{
	if (!m_pSelectedEffectInstance)
	{
		return;
	}

	m_pSelectedEffectInstance->SetPosition(c_rv3Position);
}

void CEffectManager::SetEffectInstanceRotation(const D3DXVECTOR3 & c_rv3Rotation)
{
	if (!m_pSelectedEffectInstance)
	{
		return;
	}

	m_pSelectedEffectInstance->SetRotation(c_rv3Rotation.x,c_rv3Rotation.y,c_rv3Rotation.z);
}

void CEffectManager::SetEffectInstanceGlobalMatrix(const D3DXMATRIX & c_rmatGlobal)
{
	if (!m_pSelectedEffectInstance)
		return;

	m_pSelectedEffectInstance->SetGlobalMatrix(c_rmatGlobal);
}

void CEffectManager::ShowEffect()
{
	if (!m_pSelectedEffectInstance)
		return;

	m_pSelectedEffectInstance->Show();
}

void CEffectManager::HideEffect()
{
	if (!m_pSelectedEffectInstance)
		return;

	m_pSelectedEffectInstance->Hide();
}

bool CEffectManager::GetEffectData(DWORD dwID, CEffectData ** ppEffect)
{
	TEffectDataMap::iterator itor = m_kEftDataMap.find(dwID);

	if (itor == m_kEftDataMap.end())
		return false;

	*ppEffect = itor->second;

	return true;
}

bool CEffectManager::GetEffectData(DWORD dwID, const CEffectData ** c_ppEffect)
{
	TEffectDataMap::iterator itor = m_kEftDataMap.find(dwID);

	if (itor == m_kEftDataMap.end())
		return false;

	*c_ppEffect = itor->second;

	return true;
}

DWORD CEffectManager::GetRandomEffect()
{
	int iIndex = random() % m_kEftDataMap.size();

	TEffectDataMap::iterator itor = m_kEftDataMap.begin();
	for (int i = 0; i < iIndex; ++i, ++itor);

	return itor->first;
}

int CEffectManager::GetEmptyIndex()
{
	static int iMaxIndex=1;

	if (iMaxIndex>2100000000)
		iMaxIndex = 1;

	int iNextIndex = iMaxIndex++;
	while(m_kEftInstMap.find(iNextIndex) != m_kEftInstMap.end())
		iNextIndex++;

	return iNextIndex;
}

void CEffectManager::DeleteAllInstances()
{
	__DestroyEffectInstanceMap();
}

void CEffectManager::__DestroyEffectInstanceMap()
{
	for (TEffectInstanceMap::iterator i = m_kEftInstMap.begin(); i != m_kEftInstMap.end(); ++i)
	{
		CEffectInstance * pkEftInst = i->second;
		CEffectInstance::Delete(pkEftInst);
	}

	m_kEftInstMap.clear();
    m_optionFiltered.clear();
}

void CEffectManager::__DestroyEffectCacheMap()
{
	for (TEffectInstanceMap::iterator i = m_kEftCacheMap.begin(); i != m_kEftCacheMap.end(); ++i)
	{
		CEffectInstance * pkEftInst = i->second;
		CEffectInstance::Delete(pkEftInst);
	}

	m_kEftCacheMap.clear();
}

void CEffectManager::__DestroyEffectDataMap()
{
	for (TEffectDataMap::iterator i = m_kEftDataMap.begin(); i != m_kEftDataMap.end(); ++i)
	{
		CEffectData * pData = i->second;
		CEffectData::Delete(pData);
	}

	m_kEftDataMap.clear();
}

void CEffectManager::Destroy()
{
	__DestroyEffectInstanceMap();
	__DestroyEffectCacheMap();
	__DestroyEffectDataMap();

	__Initialize();
}

void CEffectManager::__Initialize()
{
	m_pSelectedEffectInstance = nullptr;
	m_isDisableSortRendering = false;
}

CEffectManager::CEffectManager()
{
	__Initialize();
}

CEffectManager::~CEffectManager()
{
	Destroy();
}

// just for map effect

#ifdef ENABLE_WIKI_SYSTEM
void CEffectManager::RenderOneWiki(DWORD id)
{
	STATEMANAGER.SetTexture(0, NULL);
	STATEMANAGER.SetTexture(1, NULL);
	const auto& pEffectInstance = m_kEftInstMap.find(id);
	if (pEffectInstance != m_kEftInstMap.end())
	{
		pEffectInstance->second->SetWikiIgnoreFrustum(true);
		pEffectInstance->second->Show();
		pEffectInstance->second->Render();
	}
}
#endif

void CEffectManager::RenderEffect()
{
	if (!m_pSelectedEffectInstance)
		return;

	m_pSelectedEffectInstance->Render();
}

#ifdef ENABLE_AURA_COSTUME_SYSTEM
int CEffectManager::CreateEffectWithScale(DWORD dwID, const D3DXVECTOR3& c_rv3Position, const D3DXVECTOR3& c_rv3Rotation, float fParticleScale
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	, DWORD * dwSkillColor
#endif
)
{
	int iInstanceIndex = GetEmptyIndex();

	CreateEffectInstanceWithScale(iInstanceIndex, dwID, fParticleScale);

	SelectEffectInstance(iInstanceIndex);
	D3DXMATRIX mat;
	D3DXMatrixRotationYawPitchRoll(&mat, D3DXToRadian(c_rv3Rotation.x), D3DXToRadian(c_rv3Rotation.y), D3DXToRadian(c_rv3Rotation.z));
	mat._41 = c_rv3Position.x;
	mat._42 = c_rv3Position.y;
	mat._43 = c_rv3Position.z;
	SetEffectInstanceGlobalMatrix(mat);

	return iInstanceIndex;
}

void CEffectManager::CreateEffectInstanceWithScale(DWORD dwInstanceIndex, DWORD dwID, float fParticleScale, const D3DXVECTOR3* c_pv3MeshScale
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	, DWORD * dwSkillColor
#endif
)
{
	if (!dwID)
		return;

	CEffectData* pEffect;
	if (!GetEffectData(dwID, &pEffect))
	{
		Tracef("CEffectManager::CreateEffectInstance - NO DATA :%d\n", dwID);
		return;
	}

	CEffectInstance* pEffectInstance = CEffectInstance::New();
	pEffectInstance->SetParticleScale(fParticleScale);
	if (c_pv3MeshScale)
		pEffectInstance->SetMeshScale(*c_pv3MeshScale);

	pEffectInstance->SetEffectDataPointer(pEffect
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		, dwSkillColor, dwID
#endif
	);

	m_kEftInstMap.insert(TEffectInstanceMap::value_type(dwInstanceIndex, pEffectInstance));
}
#endif

void CEffectManager::SetOptionColorTint(DWORD index, const D3DXCOLOR& color)
{
    auto found = m_kEftInstMap.find(index);
    if (found != m_kEftInstMap.end()) found->second->SetOptionColorTint(color);
}
