// PythonBackground.cpp: implementation of the CPythonBackground class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "../eterlib/CullingManager.h"
#include "../eterlib/Camera.h"
#include "../eterPack/EterPackManager.h"
#include "../gamelib/MapOutDoor.h"
#include "../gamelib/PropertyLoader.h"
#include "../gamelib/Property.h"

#include "PythonBackground.h"
#include "PythonCharacterManager.h"
#include "PythonNetworkStream.h"
#include "PythonMiniMap.h"
#include "PythonSystem.h"
#include "../MilesLib/SoundManager.h"
#include "../EffectLib/EffectManager.h"
#include <algorithm>

std::string g_strEffectName = "d:/ymir work/effect/etc/direction/direction_land.mse";

DWORD CPythonBackground::GetRenderShadowTime()
{
	return m_dwRenderShadowTime;
}

bool CPythonBackground::SetVisiblePart(int eMapOutDoorPart, bool isVisible)
{
	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.SetVisiblePart(eMapOutDoorPart, isVisible);
	return true;
}

void CPythonBackground::EnableTerrainOnlyForHeight()
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.EnableTerrainOnlyForHeight(TRUE);
}

bool CPythonBackground::SetSplatLimit(int iSplatNum)
{
	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.SetSplatLimit(iSplatNum);
	return true;
}

void CPythonBackground::CreateCharacterShadowTexture()
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.CreateCharacterShadowTexture();
}

void CPythonBackground::ReleaseCharacterShadowTexture()
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.ReleaseCharacterShadowTexture();
}

void CPythonBackground::RefreshShadowLevel()
{
	SetGraphicsLevel(CPythonSystem::Instance().GetGraphicsLevel());
	if (CPythonSystem::Instance().GetGraphicsLevel() == 0)
	{
		SetShadowQualityLevel(0);
		SetTerrainShadow(0);
		SetTreeShadow(0);
		SetSpecularLevel(0);
		SetBloodLevel(0);
	}
	else
	{
		SetShadowQualityLevel(CPythonSystem::Instance().GetShadowQualityLevel());
		// Shadow caster level comes from the graphics preset above (SetGraphicsLevel); the saved
		// iTreeShadow was polluted by the old caster toggle (1/0) and capped casting at "player".
		SetRainLevel(CPythonSystem::Instance().GetRainLevel());
		SetSpecularLevel(CPythonSystem::Instance().GetSpecularLevel());
		SetBloodLevel(CPythonSystem::Instance().GetBloodLevel());
	}

	SetGrassType(CPythonSystem::Instance().GetGrassType());
	SetShadowLevel(CPythonSystem::Instance().GetShadowLevel());
	CPythonSystem::Instance().SetSharpnessPower(CPythonSystem::Instance().GetSharpnessPower());
	CPythonSystem::Instance().SetSpecularPower(CPythonSystem::Instance().GetSpecularPower());
}

void CPythonBackground::RefreshGraphicsLevel()
{
	SetGraphicsLevel(CPythonSystem::Instance().GetGraphicsLevel());
}

bool CPythonBackground::SetGrassType(int eLevel)
{
	m_eGrassType = eLevel;

	if (eLevel <= 0)
	{
		Frustum::Instance().realtimeGrass = 0;
	}
	else
	{
		Frustum::Instance().realtimeGrass = 1;
		Frustum::Instance().grassType = 0;
	}

	return true;
}

bool CPythonBackground::SetGraphicsLevel(int eLevel)
{
	m_eGraphicsLevel = eLevel;

	switch (m_eGraphicsLevel)
	{
	case GRAPHICS_OFF:
		Frustum::Instance().graphicsLevel = 0;
		Frustum::Instance().graphicsWaterLevel = 0;
		Frustum::Instance().shadowRender = 0;
		Frustum::Instance().terrainDepthLevel = 0;
		Frustum::Instance().increaseShadowRangeFrustum = 1.0f;
		SetShadowQualityLevel(0);
		SetTreeShadow(0);
		SetGrassType(0);
		SetRainLevel(0);
		SetBloodLevel(0);
		break;

	case GRAPHICS_MEDIUM:
		Frustum::Instance().graphicsLevel = 1;
		Frustum::Instance().graphicsWaterLevel = m_bWaterReflectionOption ? 1 : 0;	// water shader, no reflection pass
		Frustum::Instance().shadowRender = 1;
		Frustum::Instance().increaseShadowRangeFrustum = 4.0f;
		Frustum::Instance().terrainDepthLevel = 1;
		SetShadowQualityLevel(1);
		SetTreeShadow(TREE_SHADOW_high);	// characters + buildings (trees need the Ultra tree depth shader)
		SetGrassType(1);
		SetBloodLevel(1);
		break;

	case GRAPHICS_ULTRA:
		Frustum::Instance().graphicsLevel = 2;
		Frustum::Instance().graphicsWaterLevel = m_bWaterReflectionOption ? 2 : 0;	// + real-time reflection
		Frustum::Instance().shadowRender = 2;
		Frustum::Instance().increaseShadowRangeFrustum = 10.0f;
		Frustum::Instance().terrainDepthLevel = 1;
		SetShadowQualityLevel(3);
		SetTreeShadow(3);
		SetGrassType(1);
		SetBloodLevel(1);
		break;
	}

	if (!m_pkMap)
		return true;

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	return true;
}

bool CPythonBackground::SetEnhancedFoliage(bool bEnable)
{
	// Normally trees collapse to a flat billboard at distance (SetDropToBillboard(true),
	// set unconditionally in CSpeedTreeWrapper::LoadTree). Enhanced foliage keeps full
	// per-leaf SpeedTree geometry at all distances instead.
	CSpeedTreeRT::SetDropToBillboard(!bEnable);
	return true;
}

bool CPythonBackground::SetSpeedTreeDynamicLight(bool bEnable)
{
	// Applies to trees loaded from this point on (CSpeedTreeWrapper::LoadTree reads
	// this flag once, at tree creation) -- takes full effect after a map reload/warp,
	// same as other SpeedTree quality options in this file.
	CSpeedTreeWrapper::ms_bDynamicLeafLighting = bEnable;
	return true;
}

bool CPythonBackground::SetWaterReflection(int eLevel)
{
	if (!m_pkMap)
		return false;

	// The toggle sends 1/0. Ultra keeps its reflection level (2) instead of being dropped to 1.
	m_bWaterReflectionOption = eLevel > 0;
	if (eLevel > 0)
		eLevel = (Frustum::Instance().graphicsLevel == 2) ? 2 : 1;
	if (Frustum::Instance().graphicsLevel == 0)
		eLevel = 0;
	Frustum::Instance().graphicsWaterLevel = eLevel;
	if (eLevel == 0)
		GetMapOutdoorRef().ReleaseWaterShaders();
	else
		GetMapOutdoorRef().CreateWaterShaders();

	return true;
}

int CPythonBackground::GetWaterReflection()
{
	return Frustum::Instance().graphicsWaterLevel;
}

bool CPythonBackground::SetShadowQualityLevel(int eLevel)
{
	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	m_eShadowQualityLevel = eLevel;

	switch (eLevel)
	{
	case SHADOW_QUALITY_BAD:
		rkMap.SetShadowTextureSize(1024);
		rkMap.CreateCharacterShadowTexture();
		break;
	case SHADOW_QUALITY_AVERAGE:
		rkMap.SetShadowTextureSize(1500);
		rkMap.CreateCharacterShadowTexture();
		break;
	case SHADOW_QUALITY_GOOD:
		rkMap.SetShadowTextureSize(1750);
		rkMap.CreateCharacterShadowTexture();
		break;
	case SHADOW_QUALITY_ULTRA:
		rkMap.SetShadowTextureSize(2048);
		rkMap.CreateCharacterShadowTexture();
		break;
	}

	return true;
}

bool CPythonBackground::SetTerrainShadow(int eLevel)
{
	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	m_eTerrainShadow = eLevel;

	switch (eLevel)
	{
	case TERRAIN_SHADOW_off:
		break;
	case TERRAIN_SHADOW_on:
		break;
	}

	return true;
}

//SPECULAR JTX
bool CPythonBackground::SetSpecularLevel(int eLevel)
{
	m_eSpecularLevel = eLevel;

	switch (m_eSpecularLevel)
	{
	case SPECULAR_off:
		Frustum::Instance().realtimeSpecular = 0;
		break;

	case SPECULAR_on:
		Frustum::Instance().realtimeSpecular = 1;
		break;
	}

	return true;
}

bool CPythonBackground::SetAmbianceEffectsLevel(int eLevel)
{
	m_eAmbianceEffectsLevel = eLevel;

	switch (m_eAmbianceEffectsLevel)
	{
	case AEFFECTS_off:
		Frustum::Instance().ambianceEffectsLevel = 0;
		break;

	case AEFFECTS_on:
		Frustum::Instance().ambianceEffectsLevel = 1;
		break;
	}

	return true;
}

bool CPythonBackground::SetBloodLevel(int eLevel)
{
	m_eBloodLevel = eLevel;

	switch (m_eBloodLevel)
	{
	case BLOOD_off:
		Frustum::Instance().bloodLevel = 0;
		break;

	case BLOOD_on:
		Frustum::Instance().bloodLevel = 1;
		break;
	}

	return true;
}

bool CPythonBackground::SetShadowTypeLevel(int eLevel)
{
	Frustum::Instance().shadowType = eLevel;
	return true;
}

bool CPythonBackground::SetTreeShadow(int eLevel)
{
	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	m_eTreeShadow = eLevel;

	switch (eLevel)
	{
	case TREE_SHADOW_on:
		Frustum::Instance().treeShadow = 0;
		break;

	case TREE_SHADOW_off:
		Frustum::Instance().treeShadow = 1;
		break;

	case TREE_SHADOW_high:
		Frustum::Instance().treeShadow = 2;
		break;

	case TREE_SHADOW_all:
		Frustum::Instance().treeShadow = 3;
		break;

	}

	return true;
}

//RAIN
bool CPythonBackground::SetRainLevel(int eLevel)
{
	CSoundManager::Instance().StopSound2D("sound/ambience/rain_light.mp3");
	CSoundManager::Instance().StopSound2D("sound/ambience/rain.mp3");

	m_eRainLevel = eLevel;
	m_dwNextThunderTime = 0;
	m_dwThunderFlashEndTime = 0;

	switch (m_eRainLevel)
	{
	case RAIN_0:
		if (m_SnowEnvironment.GetWeatherMode() == CSnowEnvironment::WEATHER_MODE_RAIN)
			m_SnowEnvironment.Disable();
		Frustum::Instance().selectRainFrustun = 0;
		Frustum::Instance().puddleThreshold = 2.0f;
		Frustum::Instance().isThunderTime = false;
		CSpeedTreeForestDirectX8::Instance().SetWindStrength(CPythonSystem::Instance().GetAdvancedOption("speedtree_wind",1) ? 0.5f : 0.0f);
		break;

	case RAIN_1:
		m_SnowEnvironment.EnableRain(1);
		CSoundManager::Instance().PlaySound2DLoop("sound/ambience/rain_light.mp3");
		CSpeedTreeForestDirectX8::Instance().SetWindStrength(CPythonSystem::Instance().GetAdvancedOption("speedtree_wind",1) ? 0.7f : 0.0f);
		Frustum::Instance().selectRainFrustun = 8;
		Frustum::Instance().puddleThreshold = 0.85f;
		break;

	case RAIN_2:
		m_SnowEnvironment.EnableRain(2);
		CSoundManager::Instance().PlaySound2DLoop("sound/ambience/rain.mp3");
		CSpeedTreeForestDirectX8::Instance().SetWindStrength(CPythonSystem::Instance().GetAdvancedOption("speedtree_wind",1) ? 1.0f : 0.0f);
		Frustum::Instance().selectRainFrustun = 16;
		Frustum::Instance().puddleThreshold = 0.70f;
		m_dwNextThunderTime = CTimer::Instance().GetCurrentMillisecond() + 1000;
		break;

	case RAIN_3:
		m_SnowEnvironment.EnableRain(3);
		CSoundManager::Instance().PlaySound2DLoop("sound/ambience/rain.mp3");
		CSpeedTreeForestDirectX8::Instance().SetWindStrength(CPythonSystem::Instance().GetAdvancedOption("speedtree_wind",1) ? 1.2f : 0.0f);
		Frustum::Instance().selectRainFrustun = 25;
		Frustum::Instance().puddleThreshold = 0.55f;
		m_dwNextThunderTime = CTimer::Instance().GetCurrentMillisecond() + 800;
		break;

	default:
		break;
	}

	return true;
}

bool CPythonBackground::SetShadowLevel(int eLevel)
{
	if (!m_pkMap)
		return false;

	if (m_eShadowLevel == eLevel)
		return true;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	m_eShadowLevel = eLevel;

	switch (m_eShadowLevel)
	{
		case SHADOW_NONE:
			rkMap.SetDrawShadow(false);
			rkMap.SetShadowTextureSize(2048);
			break;

		case SHADOW_GROUND:
			rkMap.SetDrawShadow(true);
			rkMap.SetDrawCharacterShadow(false);
			rkMap.SetShadowTextureSize(2048);
			break;

		case SHADOW_GROUND_AND_SOLO:
			rkMap.SetDrawShadow(true);
			rkMap.SetDrawCharacterShadow(true);
			rkMap.SetShadowTextureSize(2048);
			break;

		case SHADOW_ALL:
			rkMap.SetDrawShadow(true);
			rkMap.SetDrawCharacterShadow(true);
			rkMap.SetShadowTextureSize(2048);
			break;

		case SHADOW_ALL_HIGH:
			rkMap.SetDrawShadow(true);
			rkMap.SetDrawCharacterShadow(true);
			rkMap.SetShadowTextureSize(2048);
			break;

		case SHADOW_ALL_MAX:
			rkMap.SetDrawShadow(true);
			rkMap.SetDrawCharacterShadow(true);
			rkMap.SetShadowTextureSize(2048);
			break;
	}

	return true;
}

void CPythonBackground::SelectViewDistanceNum(int eNum)
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();

	if (!mc_pcurEnvironmentData)
	{
		TraceError("CPythonBackground::SelectViewDistanceNum(int eNum=%d) mc_pcurEnvironmentData is NULL", eNum);
		return;
	}

	m_eViewDistanceNum = eNum;

	TEnvironmentData * env = ((TEnvironmentData *) mc_pcurEnvironmentData);

	if (env->bReserve)
	{
		env->m_fFogNearDistance = m_ViewDistanceSet[m_eViewDistanceNum].m_fFogStart;
		env->m_fFogFarDistance = m_ViewDistanceSet[m_eViewDistanceNum].m_fFogEnd;
		env->v3SkyBoxScale = m_ViewDistanceSet[m_eViewDistanceNum].m_v3SkyBoxScale;
		rkMap.SetEnvironmentSkyBox();
	}
}

void CPythonBackground::SetViewDistanceSet(int eNum, float fFarClip)
{
	if (!m_pkMap)
		return;

	m_ViewDistanceSet[eNum].m_fFogStart		= fFarClip * 0.5f;//0.3333333f;
	m_ViewDistanceSet[eNum].m_fFogEnd		= fFarClip * 0.7f;//0.6666667f;

	float fSkyBoxScale						= fFarClip * 0.6f;//0.5773502f;
	m_ViewDistanceSet[eNum].m_v3SkyBoxScale	= D3DXVECTOR3(fSkyBoxScale, fSkyBoxScale, fSkyBoxScale);
	m_ViewDistanceSet[eNum].m_fFarClip		= fFarClip;

	if (eNum == m_eViewDistanceNum)
		SelectViewDistanceNum(eNum);
}

float CPythonBackground::GetFarClip()
{
	if (!m_pkMap)
		return 50000.0f;

	if (m_ViewDistanceSet[m_eViewDistanceNum].m_fFarClip==0.0f)
	{
		TraceError("CPythonBackground::GetFarClip m_eViewDistanceNum=%d", m_eViewDistanceNum);
		m_ViewDistanceSet[m_eViewDistanceNum].m_fFarClip=25600.0f;
	}

	return m_ViewDistanceSet[m_eViewDistanceNum].m_fFarClip;
}

void CPythonBackground::GetDistanceSetInfo(int * peNum, float * pfStart, float * pfEnd, float * pfFarClip)
{
	if (!m_pkMap)
	{
		*peNum = 4;
		*pfStart= 10000.0f;
		*pfEnd= 15000.0f;
		*pfFarClip = 50000.0f;
		return;
	}
	*peNum = m_eViewDistanceNum;
	*pfStart = m_ViewDistanceSet[m_eViewDistanceNum].m_fFogStart;
	*pfEnd= m_ViewDistanceSet[m_eViewDistanceNum].m_fFogEnd;
	*pfFarClip = m_ViewDistanceSet[m_eViewDistanceNum].m_fFarClip;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPythonBackground::CPythonBackground()
{
	m_dwRenderShadowTime=0;
	m_eViewDistanceNum=0;
	m_eViewDistanceNum=0;
	m_eViewDistanceNum=0;
	m_eShadowLevel=SHADOW_NONE;
	m_dwBaseX=0;
	m_dwBaseY=0;
	m_strMapName="";
	m_iDayMode = DAY_MODE_LIGHT;
	m_iXMasTreeGrade = 0;
	m_bVisibleGuildArea = FALSE;
	m_eGraphicsLevel = -1;
	m_eRainLevel = -1;
	m_dwNextThunderTime = 0;
	m_dwThunderFlashEndTime = 0;

	SetViewDistanceSet(4, 25600.0f);
	SetViewDistanceSet(3, 25600.0f);
	SetViewDistanceSet(2, 25600.0f);
	SetViewDistanceSet(1, 25600.0f);
	SetViewDistanceSet(0, 25600.0f);
	Initialize();
}

CPythonBackground::~CPythonBackground()
{
	Tracen("CPythonBackground Clear");
}

#define ENABLE_ATLASINFO_FROM_ROOT
void CPythonBackground::Initialize()
{
#ifdef ENABLE_ATLASINFO_FROM_ROOT
	std::string stAtlasInfoFileName("AtlasInfo.txt");
#else
	std::string stAtlasInfoFileName (LocaleService_GetLocalePath());
	stAtlasInfoFileName += "/AtlasInfo.txt";
#endif
	SetAtlasInfoFileName(stAtlasInfoFileName.c_str());
	CMapManager::Initialize();
}

void CPythonBackground::__CreateProperty()
{
	if (CEterPackManager::SEARCH_FILE_FIRST == CEterPackManager::Instance().GetSearchMode() &&
		_access("property", 0) == 0)
	{
		m_PropertyManager.Initialize(NULL);

		CPropertyLoader PropertyLoader;
		PropertyLoader.SetPropertyManager(&m_PropertyManager);
		PropertyLoader.Create("*.*", "Property");
	}
	else
	{
		m_PropertyManager.Initialize("pack/property");
	}
}

//////////////////////////////////////////////////////////////////////
// Normal Functions
//////////////////////////////////////////////////////////////////////

bool CPythonBackground::GetPickingPoint(D3DXVECTOR3 * v3IntersectPt)
{
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetPickingPoint(v3IntersectPt);
}

bool CPythonBackground::GetPickingPointWithRay(const CRay & rRay, D3DXVECTOR3 * v3IntersectPt)
{
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetPickingPointWithRay(rRay, v3IntersectPt);
}

bool CPythonBackground::GetPickingPointWithRayOnlyTerrain(const CRay & rRay, D3DXVECTOR3 * v3IntersectPt)
{
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetPickingPointWithRayOnlyTerrain(rRay, v3IntersectPt);
}

BOOL CPythonBackground::GetLightDirection(D3DXVECTOR3 & rv3LightDirection)
{
	if (!mc_pcurEnvironmentData)
		return FALSE;

	rv3LightDirection.x = mc_pcurEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.x;
	rv3LightDirection.y = mc_pcurEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.y;
	rv3LightDirection.z = mc_pcurEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction.z;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CPythonBackground::Destroy()
{
	ClearFallingLeaves();
	CSoundManager::Instance().StopSound2D("data/sound/rain_light.mp3");
	CSoundManager::Instance().StopSound2D("data/sound/rain.mp3");
	Frustum::Instance().isThunderTime = false;
	m_dwNextThunderTime = 0;
	m_dwThunderFlashEndTime = 0;

	CMapManager::Destroy();
	m_SnowEnvironment.Destroy();
	m_bVisibleGuildArea = FALSE;
}

void CPythonBackground::__EnsurePropertyCreated()
{
	static int s_isCreateProperty=false;

	if (!s_isCreateProperty)
	{
		s_isCreateProperty=true;
		__CreateProperty();
	}
}

void CPythonBackground::QueueSelectScreenMapPreload(DWORD dwGlobalX, DWORD dwGlobalY)
{
	TMapInfo* pkMapInfo = GlobalPositionToMapInfo(dwGlobalX, dwGlobalY);
	if (!pkMapInfo)
		return;

	// Same 3x3 sector block CMapOutdoor::Update loads around the start position.
	const int iSectorX = int(dwGlobalX - pkMapInfo->m_dwBaseX) / CTerrainImpl::TERRAIN_XSIZE;
	const int iSectorY = int(dwGlobalY - pkMapInfo->m_dwBaseY) / CTerrainImpl::TERRAIN_YSIZE;
	for (int y = iSectorY - 1; y <= iSectorY + 1; ++y)
	{
		for (int x = iSectorX - 1; x <= iSectorX + 1; ++x)
		{
			if (x < 0 || y < 0)
				continue;

			char szAreaFile[256];
			_snprintf(szAreaFile, sizeof(szAreaFile), "%s\\%06u\\AreaData.txt", pkMapInfo->m_strName.c_str(), (unsigned)(x * 1000 + y));
			if (m_kSet_strPreloadQueued.insert(szAreaFile).second)
				m_kVec_strPreloadAreaFile.push_back(szAreaFile);
		}
	}
}

bool CPythonBackground::SelectScreenMapPreloadStep(DWORD dwBudgetMs)
{
	const DWORD dwStart = ELTimer_GetMSec();
	auto OverBudget = [&]() { return ELTimer_GetMSec() - dwStart >= dwBudgetMs; };

	// 1) area files -> building model names
	while (!m_kVec_strPreloadAreaFile.empty())
	{
		const std::string strAreaFile = m_kVec_strPreloadAreaFile.back();
		m_kVec_strPreloadAreaFile.pop_back();

		if (!CEterPackManager::Instance().isExist(strAreaFile.c_str()))
			continue;

		__EnsurePropertyCreated();

		CTokenVectorMap kTokens;
		if (!LoadMultipleTextData(strAreaFile.c_str(), kTokens) || kTokens.find("objectcount") == kTokens.end())
			continue;

		const DWORD dwCount = atoi(kTokens["objectcount"][0].c_str());
		char szObjectName[32 + 1];
		for (DWORD i = 0; i < dwCount; ++i)
		{
			_snprintf(szObjectName, sizeof(szObjectName), "object%03d", i);
			CTokenVectorMap::iterator it = kTokens.find(szObjectName);
			if (it == kTokens.end() || it->second.size() < 4)
				continue;

			CProperty* pProperty;
			if (!CPropertyManager::Instance().Get((DWORD)_strtoui64(it->second[3].c_str(), nullptr, 10), &pProperty))
				continue;

			const char* c_szPropertyType;
			if (!pProperty->GetString("PropertyType", &c_szPropertyType) ||
				prt::GetPropertyType(c_szPropertyType) != prt::PROPERTY_TYPE_BUILDING)
				continue;

			prt::TPropertyBuilding kData;
			if (prt::PropertyBuildingStringToData(pProperty, &kData) && m_kSet_strPreloadQueued.insert(kData.strFileName).second)
				m_kVec_strPreloadModelFile.push_back(kData.strFileName);
		}

		if (OverBudget())
			return false;
	}

	// 2) load the models (and their textures) into the resource cache, holding a reference
	while (!m_kVec_strPreloadModelFile.empty())
	{
		const std::string strModel = m_kVec_strPreloadModelFile.back();
		m_kVec_strPreloadModelFile.pop_back();

		CResource* pkResource = CResourceManager::Instance().GetResourcePointer(strModel.c_str());
		if (pkResource)
		{
			pkResource->AddReference();
			m_kVec_pkPreloadedResource.push_back(pkResource);
		}

		if (OverBudget())
			return false;
	}

	return true;
}

void CPythonBackground::ReleaseSelectScreenMapPreload()
{
	// The map now holds its own references; drop ours (models of other characters' maps
	// return to the normal resource garbage collection).
	for (CResource* pkResource : m_kVec_pkPreloadedResource)
		pkResource->Release();
	m_kVec_pkPreloadedResource.clear();
	m_kVec_strPreloadAreaFile.clear();
	m_kVec_strPreloadModelFile.clear();
	m_kSet_strPreloadQueued.clear();
}

void CPythonBackground::Create()
{
	__EnsurePropertyCreated();

	CMapManager::Create();

	m_SnowEnvironment.Create();
}

struct FGetPortalID
{
	float m_fRequestX, m_fRequestY;
	std::set<int> m_kSet_iPortalID;
	FGetPortalID(float fRequestX, float fRequestY)
	{
		m_fRequestX=fRequestX;
		m_fRequestY=fRequestY;
	}
	void operator () (CGraphicObjectInstance * pObject)
	{
		for (int i = 0; i < PORTAL_ID_MAX_NUM; ++i)
		{
			int iID = pObject->GetPortal(i);
			if (0 == iID)
				break;

			m_kSet_iPortalID.insert(iID);
		}
	}
};

void CPythonBackground::Update(float fCenterX, float fCenterY, float fCenterZ)
{
	if (!IsMapReady())
		return;
#ifdef __PERFORMANCE_CHECKER__
	DWORD t1=ELTimer_GetMSec();
#endif
	UpdateMap(fCenterX, fCenterY, fCenterZ);
#ifdef __PERFORMANCE_CHECKER__
	DWORD t2=ELTimer_GetMSec();
#endif
	UpdateAroundAmbience(fCenterX, fCenterY, fCenterZ);
#ifdef __PERFORMANCE_CHECKER__
	DWORD t3=ELTimer_GetMSec();
#endif
	m_SnowEnvironment.Update(D3DXVECTOR3(fCenterX, -fCenterY, fCenterZ));
	UpdateFallingLeaves(fCenterX, -fCenterY);
#if defined(ANK_SYSTEM_GRAPHICS)
	UpdateWeather();
#endif

#ifdef __PERFORMANCE_CHECKER__
	{
		static FILE* fp=fopen("perf_bg_update.txt", "w");
		if (t3-t1>5)
		{
			fprintf(fp, "BG.Total %d (Time %f)\n", t3-t1, ELTimer_GetMSec()/1000.0f);
			fprintf(fp, "BG.UpdateMap %d\n", t2-t1);
			fprintf(fp, "BG.UpdateAmb %d\n", t3-t2);
			fflush(fp);
		}
	}
#endif

	// Portal Process
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	if (rkMap.IsEnablePortal())
	{
		CCullingManager & rkCullingMgr = CCullingManager::Instance();
		FGetPortalID kGetPortalID(fCenterX, -fCenterY);

		Vector3d aVector3d;
		aVector3d.Set(fCenterX, -fCenterY, fCenterZ);

		Vector3d toTop;
		toTop.Set(0, 0, 25000.0f);

		rkCullingMgr.ForInRay(aVector3d, toTop, &kGetPortalID);

		std::set<int>::iterator itor = kGetPortalID.m_kSet_iPortalID.begin();
		if (!__IsSame(kGetPortalID.m_kSet_iPortalID, m_kSet_iShowingPortalID))
		{
			ClearPortal();
			std::set<int>::iterator itor=kGetPortalID.m_kSet_iPortalID.begin();
			for (; itor!=kGetPortalID.m_kSet_iPortalID.end(); ++itor)
			{
				AddShowingPortalID(*itor);
			}
			RefreshPortal();

			m_kSet_iShowingPortalID = kGetPortalID.m_kSet_iPortalID;
		}
	}

	// Target Effect Process
	{
		std::map<DWORD, DWORD>::iterator itor = m_kMap_dwTargetID_dwChrID.begin();
		for (; itor != m_kMap_dwTargetID_dwChrID.end(); ++itor)
		{
			DWORD dwTargetID = itor->first;
			DWORD dwChrID = itor->second;

			CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwChrID);

			if (!pInstance)
				continue;

			TPixelPosition kPixelPosition;
			pInstance->NEW_GetPixelPosition(&kPixelPosition);

			CreateSpecialEffect(dwTargetID,
								+kPixelPosition.x,
								-kPixelPosition.y,
								+kPixelPosition.z,
								g_strEffectName.c_str());
		}
	}

	// Reserve Target Effect
	{
		std::map<DWORD, SReserveTargetEffect>::iterator itor = m_kMap_dwID_kReserveTargetEffect.begin();
		for (; itor != m_kMap_dwID_kReserveTargetEffect.end();)
		{
			DWORD dwID = itor->first;
			SReserveTargetEffect & rReserveTargetEffect = itor->second;

			float ilx = float(rReserveTargetEffect.ilx);
			float ily = float(rReserveTargetEffect.ily);

			float fHeight = rkMap.GetHeight(ilx, ily);
			if (0.0f == fHeight)
			{
				++itor;
				continue;
			}

			CreateSpecialEffect(dwID, ilx, ily, fHeight, g_strEffectName.c_str());

			itor = m_kMap_dwID_kReserveTargetEffect.erase(itor);
		}
	}
}

bool CPythonBackground::__IsSame(std::set<int> & rleft, std::set<int> & rright)
{
	std::set<int>::iterator itor_l;
	std::set<int>::iterator itor_r;

	for (itor_l=rleft.begin(); itor_l!=rleft.end(); ++itor_l)
	{
		if (rright.end() == rright.find(*itor_l))
			return false;
	}

	for (itor_r=rright.begin(); itor_r!=rright.end(); ++itor_r)
	{
		if (rleft.end() == rleft.find(*itor_r))
			return false;
	}

	return true;
}

void CPythonBackground::Render()
{
	if (!IsMapReady())
		return;

	m_SnowEnvironment.Deform();

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.Render();
	if (m_bVisibleGuildArea)
		rkMap.RenderMarkedArea();
}

void CPythonBackground::RenderSnow()
{
	m_SnowEnvironment.Render();
}

void CPythonBackground::RenderPCBlocker()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderPCBlocker();
}

void CPythonBackground::RenderCollision()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderCollision();
}


void CPythonBackground::RenderCharacterShadowToTexture()
{
	extern bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW;
	if (GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW)
		return;

	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.SkyUtil();

	D3DXMATRIX matWorld;
	STATEMANAGER.GetTransform(D3DTS_WORLD, &matWorld);

	if (rkMap.BeginRenderCharacterShadowToTexture())
	{
		CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
		if (CPythonSystem::Instance().GetAdvancedOption("shadow_caster_player",1)) rkChrMgr.RenderShadowAllInstances();
		CSpeedTreeForestDirectX8::Instance().Render3(Forest_RenderAll);
		rkMap.EndRenderCharacterShadowToTexture();
	}

	STATEMANAGER.SetTransform(D3DTS_WORLD, &matWorld);
}

void CPythonBackground::RenderCharacterShadowToTexture2()
{
	extern bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW;
	if (GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW)
		return;

	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	if (Frustum::Instance().shadowType != 3)
	{
		if (Frustum::Instance().isIndoor == true)
		{
			if (rkMap.BeginRenderCharacterShadowToTexture2())
			{
				rkMap.RenderDungeon();
				rkMap.EndRenderCharacterShadowToTexture2();
			}
		}
	}

}

// Draws everything that casts into the cascade shadow maps. Characters near the player cast
// (not just the player itself), buildings/terrain from "high", trees from "all"; each caster
// class also honours its advanced option. Trees need the depth-tree pixel shader, which only
// exists at shadowRender 2 -- without it they would write colour into the R32F map.
static void RenderShadowCasters(CMapOutdoor& rkMap)
{
	CPythonSystem& rkSystem = CPythonSystem::Instance();
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();

	if (rkSystem.GetAdvancedOption("shadow_caster_player", 1))
		rkChrMgr.RenderShadowAllInstances();

	// Terrain is not drawn into the map: its vertices have no UVs, so the depth PS alpha test
	// read garbage and the ground shadowed itself (striped/speckled acne everywhere).
	if (Frustum::Instance().treeShadow >= 2)
		rkMap.RenderAreaShadow();

	if (Frustum::Instance().treeShadow == 3 && rkMap.m_lpShadowMapPixelShaderDepthTree &&
		rkSystem.GetAdvancedOption("shadow_caster_trees", 1))
	{
		STATEMANAGER.ForcePixelShader(rkMap.m_lpShadowMapPixelShaderDepthTree);
		CSpeedTreeForestDirectX8::Instance().Render2(Forest_RenderAll); //tree to shadowmap
	}
}

void CPythonBackground::RenderCharacterShadowToTexture3()
{


	extern bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW;
	if (GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW)
		return;

	if (!IsMapReady())
		return;

	if (Frustum::Instance().isIndoor)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	if (Frustum::Instance().shadowType != 3)
	{
		if (rkMap.BeginRenderCharacterShadowToTexture3())
		{
			CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();

			RenderShadowCasters(rkMap);
			rkMap.EndRenderCharacterShadowToTexture3();
		}
	}

}

void CPythonBackground::RenderCharacterShadowToTexture4()
{


	extern bool GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW;
	if (GRAPHICS_CAPS_CAN_NOT_DRAW_SHADOW)
		return;

	if (!IsMapReady())
		return;

	if (Frustum::Instance().isIndoor)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	if (Frustum::Instance().shadowType != 3)
	{
		if (Frustum::Instance().shadowType == 2)
		{
			if (rkMap.BeginRenderCharacterShadowToTexture4())
			{
				CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();

				RenderShadowCasters(rkMap);
				rkMap.EndRenderCharacterShadowToTexture4();
			}
		}
		else
		{
			if (Frustum::Instance().treeShadow == 3)
			{
				if (rkMap.BeginRenderCharacterShadowToTexture4())
				{
					CSpeedTreeForestDirectX8::Instance().Render3(Forest_RenderAll); //tree to shadowmap
					rkMap.EndRenderCharacterShadowToTexture4();
				}
			}
		}
	}
}


void CPythonBackground::RenderWaterReflection()
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	if (!rkMap.IsWaterVisible())
		return;

#ifdef JTX_UNDERWATER_CAMERA
	auto& chrMgr = CPythonCharacterManager::Instance();
	auto mainInstance = chrMgr.GetMainInstancePtr();

	if (!mainInstance)
		return;

	mainInstance->GetUnderwaterCameraPosition();
#endif

	rkMap.SetWaterReflectionRenderTarget();
	CCamera* camera = CCameraManager::Instance().GetCurrentCamera();
	D3DXVECTOR3 cameraEye = camera->GetEye();
	D3DXVECTOR3 cameraTarget = camera->GetTarget();
	D3DXVECTOR3 cameraUp = camera->GetUp();

	static float closestWaterHeight = 0.0f;
	float minDistance = 1e10f;

	float playerX = Frustum::Instance().characterPosition.x;
	float playerY = Frustum::Instance().characterPosition.y;
	float playerZ = Frustum::Instance().characterPosition.z;

	auto& waterPoints = Frustum::Instance().waterPoints;

	// Find the closest water point
	for (const auto& point : waterPoints) {
		float waterX, waterY, waterZ;
		std::tie(waterX, waterY, waterZ) = point;

		float distance = std::sqrt((waterX - playerX) * (waterX - playerX) +
			(waterY - playerY) * (waterY - playerY) +
			(waterZ - playerZ) * (waterZ - playerZ));
		if (distance < minDistance) {
			minDistance = distance;
			closestWaterHeight = waterZ;
		}
	}

	Frustum::Instance().waterLevel = closestWaterHeight;

	D3DXVECTOR3 reflectionEye = D3DXVECTOR3(cameraEye.x, cameraEye.y, Frustum::Instance().waterLevel * 2.0f - cameraEye.z);
	D3DXVECTOR3 reflectionTarget = D3DXVECTOR3(cameraTarget.x, cameraTarget.y, Frustum::Instance().waterLevel * 2.0f - cameraTarget.z);
	D3DXVECTOR3 reflectionUp = D3DXVECTOR3(cameraUp.x, cameraUp.y, -cameraUp.z);

	camera->SetViewParams(reflectionEye, reflectionTarget, reflectionUp);

	D3DXMATRIX matLightView, matLightProj;
	STATEMANAGER.SetTransform(D3DTS_VIEW, &(camera->GetViewMatrix()));
	STATEMANAGER.EnableClipPlane(0, 0.0, 0.0, 1.0, -Frustum::Instance().waterLevel);
	STATEMANAGER.GetTransform(D3DTS_PROJECTION, &matLightProj);

	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix2(cameraEye, camera->GetViewMatrix(), matLightProj);

	{
		STATEMANAGER.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		rkMap.SetInverseViewAndDynamicShaodwMatrices();
		RenderSky();
		RenderCloud();
		CSpeedTreeForestDirectX8::Instance().RenderReflection();
		rkMap.RenderArea();
		rkMap.RenderPCBlocker();
		if (Frustum::Instance().graphicsLevel == 1)
			rkMap.__RenderTerrain_RenderHardwareTransformPatchOriginal();
		else
			rkMap.RenderTerrain();

		SetCharacterDirLight();
		CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
		rkChrMgr.RenderReflectionInstances();
		STATEMANAGER.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	}

	{
		STATEMANAGER.PopRenderTarget();
		camera->SetViewParams(cameraEye, cameraTarget, cameraUp);
		STATEMANAGER.SetTransform(D3DTS_VIEW, &(camera->GetViewMatrix()));
		STATEMANAGER.DisableClipPlane(0);
	}
}

void CPythonBackground::RenderPuddleReflection()
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();

	rkMap.SetPuddleReflectionRenderTarget();
	CCamera* camera = CCameraManager::Instance().GetCurrentCamera();
	D3DXVECTOR3 cameraEye = camera->GetEye();
	D3DXVECTOR3 cameraTarget = camera->GetTarget();
	D3DXVECTOR3 cameraUp = camera->GetUp();

	float targetPuddleLevel = Frustum::Instance().characterPosition.z;
	static float s_smoothedPuddleLevel = 0.0f;
	static bool s_puddleLevelInited = false;
	if (!s_puddleLevelInited || fabsf(targetPuddleLevel - s_smoothedPuddleLevel) > 500.0f)
	{
		s_smoothedPuddleLevel = targetPuddleLevel;
		s_puddleLevelInited = true;
	}
	else
	{
		s_smoothedPuddleLevel += (targetPuddleLevel - s_smoothedPuddleLevel) * 0.15f;
	}
	Frustum::Instance().puddleLevel = s_smoothedPuddleLevel;

	D3DXVECTOR3 reflectionEye = D3DXVECTOR3(cameraEye.x, cameraEye.y, Frustum::Instance().puddleLevel * 2.0f - cameraEye.z);
	D3DXVECTOR3 reflectionTarget = D3DXVECTOR3(cameraTarget.x, cameraTarget.y, Frustum::Instance().puddleLevel * 2.0f - cameraTarget.z);
	D3DXVECTOR3 reflectionUp = D3DXVECTOR3(cameraUp.x, cameraUp.y, -cameraUp.z);

	camera->SetViewParams(reflectionEye, reflectionTarget, reflectionUp);

	D3DXMATRIX matLightProj;
	STATEMANAGER.SaveTransform(D3DTS_VIEW, &(camera->GetViewMatrix()));
	STATEMANAGER.EnableClipPlane(0, 0.0, 0.0, 1.0, -Frustum::Instance().puddleLevel);
	STATEMANAGER.GetTransform(D3DTS_PROJECTION, &matLightProj);

	CSpeedTreeForestDirectX8::Instance().UpdateCompundMatrix2(cameraEye, camera->GetViewMatrix(), matLightProj);

	{
		STATEMANAGER.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		RenderSky();
		RenderCloud();
		CSpeedTreeForestDirectX8::Instance().RenderReflection();
		rkMap.RenderArea();
		rkMap.RenderPCBlocker();
		SetCharacterDirLight();
		CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
		rkChrMgr.RenderReflectionInstances();
		STATEMANAGER.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	}

	{
		STATEMANAGER.PopRenderTarget();
		camera->SetViewParams(cameraEye, cameraTarget, cameraUp);
		STATEMANAGER.RestoreTransform(D3DTS_VIEW);
		STATEMANAGER.DisableClipPlane(0);
	}
}

void CPythonBackground::GetUnderwaterCamera()
{
	//removed
}

void CPythonBackground::RenderGrass()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.RenderGrass();
}


inline float Interpolate(float fStart, float fEnd, float fPercent)
{
	return fStart + (fEnd - fStart) * fPercent;
}
struct CollisionChecker
{
	bool isBlocked;
	CInstanceBase* pInstance;
	CollisionChecker(CInstanceBase* pInstance) : pInstance(pInstance), isBlocked(false) {}
	void operator () (CGraphicObjectInstance* pOpponent)
	{
		if (isBlocked)
			return;

		if (!pOpponent)
			return;

		if (pInstance->IsBlockObject(*pOpponent))
			isBlocked=true;
	}
};

struct CollisionAdjustChecker
{
	bool isBlocked;
	CInstanceBase* pInstance;
	CollisionAdjustChecker(CInstanceBase* pInstance) : pInstance(pInstance), isBlocked(false) {}
	void operator () (CGraphicObjectInstance* pOpponent)
	{
		if (!pOpponent)
			return;

		if (pInstance->AvoidObject(*pOpponent))
			isBlocked=true;
	}
};

bool CPythonBackground::CheckAdvancing(CInstanceBase * pInstance)
{
	if (!IsMapReady())
		return true;

	Vector3d center;
	float radius;
	pInstance->GetGraphicThingInstanceRef().GetBoundingSphere(center,radius);

	CCullingManager & rkCullingMgr = CCullingManager::Instance();

	CollisionAdjustChecker kCollisionAdjustChecker(pInstance);
	rkCullingMgr.ForInRange(center, radius, &kCollisionAdjustChecker);
	if (kCollisionAdjustChecker.isBlocked)
	{
		CollisionChecker kCollisionChecker(pInstance);
		rkCullingMgr.ForInRange(center, radius, &kCollisionChecker);
		if (kCollisionChecker.isBlocked)
		{
			pInstance->BlockMovement();
			return true;
		}
		else
		{
			pInstance->NEW_MoveToDestPixelPositionDirection(pInstance->NEW_GetDstPixelPositionRef());
		}
		return false;
	}
	return false;
}

void CPythonBackground::RenderSky()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.RenderSky();
}

void CPythonBackground::RenderCloud()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderCloud();
}

void CPythonBackground::RenderWater()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderWater();
}

void CPythonBackground::RenderEffect()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderEffect();
}

void CPythonBackground::RenderBeforeLensFlare()
{
    if (!CPythonSystem::Instance().GetAdvancedOption("sun_rays",1)) return;
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderBeforeLensFlare();
}

void CPythonBackground::RenderAfterLensFlare()
{
    if (!CPythonSystem::Instance().GetAdvancedOption("sun_rays",1)) return;
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RenderAfterLensFlare();
}

void CPythonBackground::ClearGuildArea()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.ClearGuildArea();
}

void CPythonBackground::RegisterGuildArea(int isx, int isy, int iex, int iey)
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.RegisterGuildArea(isx, isy, iex, iey);
}

void CPythonBackground::SetCharacterDirLight()
{
	if (!IsMapReady())
		return;

	if (!mc_pcurEnvironmentData)
		return;

	D3DLIGHT9 light = mc_pcurEnvironmentData->DirLights[ENV_DIRLIGHT_CHARACTER];
	if (Frustum::Instance().isThunderTime)
	{
		light.Diffuse.r = min(2.0f, light.Diffuse.r + 1.2f);
		light.Diffuse.g = min(2.0f, light.Diffuse.g + 1.3f);
		light.Diffuse.b = min(2.2f, light.Diffuse.b + 1.6f);
		light.Ambient.r = min(1.5f, light.Ambient.r + 0.8f);
		light.Ambient.g = min(1.5f, light.Ambient.g + 0.85f);
		light.Ambient.b = min(1.8f, light.Ambient.b + 1.1f);
	}
	else
	{
		if (light.Ambient.r < 0.45f) light.Ambient.r = 0.45f;
		if (light.Ambient.g < 0.45f) light.Ambient.g = 0.45f;
		if (light.Ambient.b < 0.45f) light.Ambient.b = 0.45f;
	}
	STATEMANAGER.SetLight(0, &light);
}

void CPythonBackground::SetBackgroundDirLight()
{
	if (!IsMapReady())
		return;
	if (!mc_pcurEnvironmentData)
		return;

	D3DLIGHT9 light = mc_pcurEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND];
	if (Frustum::Instance().isThunderTime)
	{
		light.Diffuse.r = min(2.0f, light.Diffuse.r + 1.2f);
		light.Diffuse.g = min(2.0f, light.Diffuse.g + 1.3f);
		light.Diffuse.b = min(2.2f, light.Diffuse.b + 1.6f);
		light.Ambient.r = min(1.5f, light.Ambient.r + 0.8f);
		light.Ambient.g = min(1.5f, light.Ambient.g + 0.85f);
		light.Ambient.b = min(1.8f, light.Ambient.b + 1.1f);
	}
	else
	{
		if (light.Ambient.r < 0.40f) light.Ambient.r = 0.40f;
		if (light.Ambient.g < 0.40f) light.Ambient.g = 0.40f;
		if (light.Ambient.b < 0.40f) light.Ambient.b = 0.40f;
	}
	STATEMANAGER.SetLight(0, &light);
}

void CPythonBackground::GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY)
{
	rGlobalX-=m_dwBaseX;
	rGlobalY-=m_dwBaseY;
}

void CPythonBackground::LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY)
{
	rLocalX+=m_dwBaseX;
	rLocalY+=m_dwBaseY;
}

void CPythonBackground::RegisterDungeonMapName(const char * c_szMapName)
{
	m_kSet_strDungeonMapName.insert(c_szMapName);
}

CPythonBackground::TMapInfo* CPythonBackground::GlobalPositionToMapInfo(DWORD dwGlobalX, DWORD dwGlobalY)
{
	TMapInfoVector::iterator f = std::find_if(m_kVct_kMapInfo.begin(), m_kVct_kMapInfo.end(), FFindWarpMapName(dwGlobalX, dwGlobalY));
	if (f == m_kVct_kMapInfo.end())
		return NULL;

	return &(*f);
}


void CPythonBackground::Warp(DWORD dwX, DWORD dwY)
{
	ClearFallingLeaves();
	TMapInfo* pkMapInfo = GlobalPositionToMapInfo(dwX, dwY);
	if (!pkMapInfo)
	{
		TraceError("NOT_FOUND_GLOBAL_POSITION(%d, %d)", dwX, dwY);
		return;
	}

	TMapInfo& rMapInfo = *pkMapInfo;
	assert((dwX >= rMapInfo.m_dwBaseX) && (dwY >= rMapInfo.m_dwBaseY));
#if defined(ANK_SYSTEM_GRAPHICS)
	Frustum::Instance().grassTexturesInitialized = false;
	Frustum::Instance().grassMatrices.clear();
	Frustum::Instance().grassShadowMatrices.clear();
#endif
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_UNDERWATER_CAMERA)
	Frustum::Instance().waterPoints.clear();
#endif

	if (!LoadMap(rMapInfo.m_strName, float(dwX - rMapInfo.m_dwBaseX), float(dwY - rMapInfo.m_dwBaseY), 0))
	{
		// LOAD_MAP_ERROR_HANDLING
		PostQuitMessage(0);
		// END_OF_LOAD_MAP_ERROR_HANDLING
		return;
	}

#if defined(ANK_SYSTEM_GRAPHICS)
	SetGraphicsLevel(CPythonSystem::Instance().GetGraphicsLevel());
#endif

	CPythonMiniMap::Instance().LoadAtlas();

	m_dwBaseX=rMapInfo.m_dwBaseX;
	m_dwBaseY=rMapInfo.m_dwBaseY;

	m_strMapName = rMapInfo.m_strName;

	SetXMaxTree(m_iXMasTreeGrade);

	if (m_kSet_strDungeonMapName.end() != m_kSet_strDungeonMapName.find(m_strMapName))
	{
		EnableTerrainOnlyForHeight();

		CMapOutdoor& rkMap=GetMapOutdoorRef();
		rkMap.EnablePortal(TRUE);
	}

	m_kSet_iShowingPortalID.clear();
	m_kMap_dwTargetID_dwChrID.clear();
	m_kMap_dwID_kReserveTargetEffect.clear();
}

void CPythonBackground::VisibleGuildArea()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.VisibleMarkedArea();

	m_bVisibleGuildArea = TRUE;
}

void CPythonBackground::DisableGuildArea()
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.DisableMarkedArea();

	m_bVisibleGuildArea = FALSE;
}

const char * CPythonBackground::GetWarpMapName()
{
	return m_strMapName.c_str();
}

void CPythonBackground::ChangeToDay()
{
	m_iDayMode = DAY_MODE_LIGHT;
}

void CPythonBackground::ChangeToNight()
{
	m_iDayMode = DAY_MODE_DARK;
}

void CPythonBackground::EnableSnowEnvironment()
{
	m_SnowEnvironment.EnableSnow();
}

void CPythonBackground::DisableSnowEnvironment()
{
	m_SnowEnvironment.Disable();
}

#if defined(ANK_SYSTEM_GRAPHICS)
void CPythonBackground::UpdateWeather()
{
	if (m_eRainLevel <= RAIN_0)
	{
		Frustum::Instance().isThunderTime = false;
		m_dwNextThunderTime = 0;
		m_dwThunderFlashEndTime = 0;
		return;
	}

	DWORD dwCurTime = CTimer::Instance().GetCurrentMillisecond();

	if (m_dwNextThunderTime == 0)
	{
		DWORD dwInterval = 5000 + (rand() % 8000);
		m_dwNextThunderTime = dwCurTime + dwInterval;
	}

	if (dwCurTime >= m_dwNextThunderTime)
	{
		Frustum::Instance().isThunderTime = true;
		m_dwThunderFlashEndTime = dwCurTime + 320;

		DWORD dwMinInterval = (m_eRainLevel >= RAIN_3) ? 6000 : (m_eRainLevel >= RAIN_2 ? 9000 : 16000);
		DWORD dwRndRange   = (m_eRainLevel >= RAIN_3) ? 6000 : (m_eRainLevel >= RAIN_2 ? 10000 : 16000);
		m_dwNextThunderTime = dwCurTime + dwMinInterval + (rand() % dwRndRange);

		CSoundManager::Instance().PlaySound2D("sound/ambience/thunder1.mp3");
	}

	if (Frustum::Instance().isThunderTime)
	{
		if (dwCurTime >= m_dwThunderFlashEndTime)
		{
			Frustum::Instance().isThunderTime = false;
		}
	}
}
#endif

const D3DXVECTOR3 c_v3TreePos = D3DXVECTOR3(76500.0f, -60900.0f, 20215.0f);

void CPythonBackground::SetXMaxTree(int iGrade)
{
	if (!m_pkMap)
		return;

	assert(iGrade >= 0 && iGrade <= 3);
	m_iXMasTreeGrade = iGrade;

	CMapOutdoor& rkMap=GetMapOutdoorRef();

	if ("map_n_snowm_01" != m_strMapName)
	{
		rkMap.XMasTree_Destroy();
		return;
	}

	if (0 == iGrade)
	{
		rkMap.XMasTree_Destroy();
		return;
	}

	//////////////////////////////////////////////////////////////////////

	iGrade -= 1;
	iGrade = max(iGrade, 0);
	iGrade = min(iGrade, 2);

	static std::string s_strTreeName[3] = {
		"d:/ymir work/tree/christmastree1.spt",
		"d:/ymir work/tree/christmastree2.spt",
		"d:/ymir work/tree/christmastree3.spt"
	};
	static std::string s_strEffectName[3] = {
		"d:/ymir work/effect/etc/christmas_tree/tree_1s.mse",
		"d:/ymir work/effect/etc/christmas_tree/tree_2s.mse",
		"d:/ymir work/effect/etc/christmas_tree/tree_3s.mse",
	};
	rkMap.XMasTree_Set(c_v3TreePos.x, c_v3TreePos.y, c_v3TreePos.z, s_strTreeName[iGrade].c_str(), s_strEffectName[iGrade].c_str());
}

void CPythonBackground::CreateTargetEffect(DWORD dwID, DWORD dwChrVID)
{
	m_kMap_dwTargetID_dwChrID.emplace(dwID, dwChrVID);
}

void CPythonBackground::CreateTargetEffect(DWORD dwID, long lx, long ly)
{
	if (m_kMap_dwTargetID_dwChrID.end() != m_kMap_dwTargetID_dwChrID.find(dwID))
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();

	DWORD dwBaseX;
	DWORD dwBaseY;
	rkMap.GetBaseXY(&dwBaseX, &dwBaseY);

	int ilx = +(lx-int(dwBaseX));
	int ily = -(ly-int(dwBaseY));

	float fHeight = rkMap.GetHeight(float(ilx), float(ily));

	if (0.0f == fHeight)
	{
		SReserveTargetEffect ReserveTargetEffect;
		ReserveTargetEffect.ilx = ilx;
		ReserveTargetEffect.ily = ily;
		m_kMap_dwID_kReserveTargetEffect.emplace(dwID, ReserveTargetEffect);
		return;
	}

	CreateSpecialEffect(dwID, ilx, ily, fHeight, g_strEffectName.c_str());
}

void CPythonBackground::DeleteTargetEffect(DWORD dwID)
{
	if (m_kMap_dwID_kReserveTargetEffect.end() != m_kMap_dwID_kReserveTargetEffect.find(dwID))
	{
		m_kMap_dwID_kReserveTargetEffect.erase(dwID);
	}
	if (m_kMap_dwTargetID_dwChrID.end() != m_kMap_dwTargetID_dwChrID.find(dwID))
	{
		m_kMap_dwTargetID_dwChrID.erase(dwID);
	}

	DeleteSpecialEffect(dwID);
}

void CPythonBackground::CreateSpecialEffect(DWORD dwID, float fx, float fy, float fz, const char * c_szFileName)
{
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.SpecialEffect_Create(dwID, fx, fy, fz, c_szFileName);
}

void CPythonBackground::DeleteSpecialEffect(DWORD dwID)
{
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.SpecialEffect_Delete(dwID);
}
//martysama0134's cc449580f8a0ea79d66107125c7ee3d3

#if defined(ANKIRA_ENVIRONMENT_EFFECT_OPTION)
void CPythonBackground::SetXMasShowEvent(bool bMode)
{
	m_bXMasShowEvent = bMode;
}

bool CPythonBackground::GetXMasShowEvent() const
{
	return m_bXMasShowEvent;
}

bool CPythonBackground::IsBoomMap(const std::string& sMapName) const
{
	static const std::vector<std::string> vMapName
	{
		"season1/metin2_map_sungzi_flame_hill_01",
		"season1/metin2_map_sungzi_flame_hill_02",
		"season1/metin2_map_sungzi_flame_hill_03",
		"season1/metin2_map_sungzi_desert_01",
		"season1/metin2_map_sungzi_desert_hill_01",
		"season1/metin2_map_sungzi_desert_hill_02",
		"season1/metin2_map_sungzi_desert_hill_03",
		"season2/metin2_map_empirewar03",
		"metin2_map_devilsCatacomb",
		"metin2_map_Mt_Thunder",
		"metin2_map_n_flame_dungeon_01",
		"metin2_map_n_snow_dungeon_01",
		"metin2_map_spiderdungeon_01",
		"metin2_map_spiderdungeon_02",
		"metin2_map_spiderdungeon_03",
		"metin2_map_deviltower1",
		"metin2_map_dawnmist_dungeon_01",
		"metin2_map_Mt_Th_dungeon_01",
		"metin2_12zi_stage",
		"metin2_map_n_flame_dragon",
		"metin2_map_defensewave",
		"metin2_map_defensewave_port",
		"metin2_map_mists_of_island",
		"metin2_map_miniboss_01",
		"metin2_map_miniboss_02",
		"metin2_map_labyrinth",
		"metin2_map_boss_crack_skipia",
		"metin2_map_boss_crack_flame",
		"metin2_map_boss_crack_snow",
		"metin2_map_boss_crack_dawnmist",
		"metin2_map_boss_awaken_skipia",
		"metin2_map_boss_awaken_flame",
		"metin2_map_boss_awaken_snow",
		"metin2_map_boss_awaken_dawnmist",
		"metin2_map_elemental_01",
		"metin2_map_elemental_02",
		"metin2_map_elemental_03",
		"metin2_map_elemental_04",
		"metin2_map_smhdungeon_01",
		"metin2_map_smhdungeon_02",
		"metin2_map_snake_temple_02",
		"metin2_map_whitedragoncave_01",
		"metin2_map_whitedragoncave_02",
		"metin2_map_whitedragoncave_boss",
		"metin2_map_anglar_dungeon_01",
		"metin2_map_otherworld_01",
		"metin2_map_otherworld_02",
		"metin2_map_otherworld_03",
		"metin2_map_otherworld_04",
		"metin2_map_smhgate_a1",
		"metin2_map_smhgate_b1",
		"metin2_map_smhgate_c1",
		"metin2_map_smhgate_threeway",
		"metin2_map_smhgate_desert",
		"metin2_map_smhgate_flame",
		"metin2_map_smhgate_snow",
		"metin2_map_smhgate_devils",
		"metin2_map_smhgate_dawnmist",
	};

	return std::find(vMapName.begin(), vMapName.end(), sMapName) == vMapName.end();
}

bool CPythonBackground::IsSnowMap(const std::string& sMapName) const
{
	static const std::vector<std::string> vMapName
	{
		"metin2_map_n_flame_01",
		"metin2_map_n_desert_01",
		"season1/metin2_map_sungzi_flame_hill_01",
		"season1/metin2_map_sungzi_flame_hill_02",
		"season1/metin2_map_sungzi_flame_hill_03",
		"season1/metin2_map_sungzi_desert_01",
		"season1/metin2_map_sungzi_desert_hill_01",
		"season1/metin2_map_sungzi_desert_hill_02",
		"season1/metin2_map_sungzi_desert_hill_03",
		"season2/metin2_map_empirewar03",
		"metin2_map_devilsCatacomb",
		"metin2_map_Mt_Thunder",
		"metin2_map_n_flame_dungeon_01",
		"metin2_map_spiderdungeon_01",
		"metin2_map_spiderdungeon_02",
		"metin2_map_spiderdungeon_03",
		"metin2_map_deviltower1",
		"metin2_map_dawnmist_dungeon_01",
		"metin2_map_Mt_Th_dungeon_01",
		"metin2_12zi_stage",
		"metin2_map_n_flame_dragon",
		"metin2_map_defensewave",
		"metin2_map_defensewave_port",
		"metin2_map_mists_of_island",
		"metin2_map_miniboss_01",
		"metin2_map_miniboss_02",
		"metin2_map_labyrinth",
		"metin2_map_boss_crack_skipia",
		"metin2_map_boss_crack_flame",
		"metin2_map_boss_crack_snow",
		"metin2_map_boss_crack_dawnmist",
		"metin2_map_boss_awaken_skipia",
		"metin2_map_boss_awaken_flame",
		"metin2_map_boss_awaken_snow",
		"metin2_map_boss_awaken_dawnmist",
		"metin2_guild_pve",
		"metin2_map_elemental_01",
		"metin2_map_elemental_02",
		"metin2_map_elemental_03",
		"metin2_map_elemental_04",
		"metin2_map_battleroyale",
		"metin2_map_maze_dungeon1",
		"metin2_map_maze_dungeon2",
		"metin2_map_maze_dungeon3",
		"metin2_map_smhdungeon_01",
		"metin2_map_smhdungeon_02",
		"metin2_map_snake_temple_02",
		"metin2_map_whitedragoncave_01",
		"metin2_map_whitedragoncave_02",
		"metin2_map_whitedragoncave_boss",
		"metin2_map_anglar_dungeon_01",
		"metin2_map_otherworld_01",
		"metin2_map_otherworld_02",
		"metin2_map_otherworld_03",
		"metin2_map_otherworld_04",
		"metin2_map_secretdungeon_01",
		"metin2_map_smhgate_a1",
		"metin2_map_smhgate_b1",
		"metin2_map_smhgate_c1",
		"metin2_map_smhgate_threeway",
		"metin2_map_smhgate_desert",
		"metin2_map_smhgate_flame",
		"metin2_map_smhgate_snow",
		"metin2_map_smhgate_devils",
		"metin2_map_smhgate_dawnmist",
	};

	return std::find(vMapName.begin(), vMapName.end(), sMapName) == vMapName.end();
}

void CPythonBackground::SetNightModeOption(bool bMode)
{
	m_bNightMode = bMode;
}

bool CPythonBackground::GetNightModeOption() const
{
	return m_bNightMode;
}

void CPythonBackground::SetSnowModeOption(bool bMode)
{
	m_bSnowMode = bMode;
}

bool CPythonBackground::GetSnowModeOption() const
{
	return m_bSnowMode;
}

int CPythonBackground::GetDayMode() const
{
	return m_iDayMode;
}
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
void CPythonBackground::CreateSpecialShopPosEffect(DWORD dwID, float fx, float fy, float fz, const char *c_szFileName)
{
	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.SpecialShopPosEffect_Create(dwID, fx, fy, fz, c_szFileName);
}

void CPythonBackground::DeleteSpecialShopPosEffect()
{
	CMapOutdoor& rkMap = GetMapOutdoorRef();
	rkMap.SpecialShopPosEffect_Delete();
}
#endif

void CPythonBackground::SetFallingLeaves(bool enabled)
{
    m_bFallingLeaves = enabled;
    if (!enabled) ClearFallingLeaves();
    m_dwNextLeafUpdate = 0;
}

void CPythonBackground::ClearFallingLeaves()
{
    for (const auto& leaf : m_leafEffects)
        CEffectManager::Instance().DestroyEffectInstance(leaf.second);
    m_leafEffects.clear();
    m_dwNextLeafUpdate = 0;
}

void CPythonBackground::UpdateFallingLeaves(float x, float y)
{
    if (!m_bFallingLeaves) return;
    const DWORD now = ELTimer_GetMSec();
    if (m_dwNextLeafUpdate && static_cast<LONG>(now - m_dwNextLeafUpdate) < 0) return;
    m_dwNextLeafUpdate = now + 1500;
    std::vector<D3DXVECTOR3> trees;
    CSpeedTreeForestDirectX8::Instance().CollectNearbyTreePositions(x, y, 1800.0f, trees);
    std::sort(trees.begin(), trees.end(), [x, y](const D3DXVECTOR3& a, const D3DXVECTOR3& b) {
        const float ax = a.x-x, ay = a.y-y, bx = b.x-x, by = b.y-y;
        return ax*ax+ay*ay < bx*bx+by*by;
    });
    if (trees.size() > 8) trees.resize(8);
    for (auto it = m_leafEffects.begin(); it != m_leafEffects.end(); )
    {
        const auto found = std::find_if(trees.begin(), trees.end(), [&](const D3DXVECTOR3& p) {
            const D3DXVECTOR3 delta = p - it->first;
            return D3DXVec3LengthSq(&delta) < 1.0f;
        });
        if (found == trees.end())
        {
            CEffectManager::Instance().DestroyEffectInstance(it->second);
            it = m_leafEffects.erase(it);
        }
        else { trees.erase(found); ++it; }
    }
    static const char* effect = "d:/ymir work/zone/plechi_env/valentine2018/effect/pink_leaf_drop.mse";
    CEffectManager& manager = CEffectManager::Instance();
    for (const auto& p : trees)
    {
        manager.RegisterEffect(effect);
        const DWORD id = manager.CreateEffect(effect, p, D3DXVECTOR3(0, 0, 0));
        if (id) m_leafEffects.emplace_back(p, id);
    }
}
