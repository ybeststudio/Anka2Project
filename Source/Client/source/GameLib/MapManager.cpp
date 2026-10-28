#include "StdAfx.h"
#include "../eterLib/StateManager.h"
#include "../EterPack/EterPackManager.h"

#include "MapManager.h"
#include "MapOutdoor.h"

#include "PropertyLoader.h"
#if defined(DIRECTX9)
#include "../SpeedTreeLib/SpeedTreeForestDirectX8.h"
#endif
#if defined(ANKIRA_ENVIRONMENT_EFFECT_OPTION)
#include "../UserInterface/PythonBackground.h"
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CMapManager::IsMapOutdoor()
{
	if (m_pkMap)
		return true;

	return false;
}

CMapOutdoor& CMapManager::GetMapOutdoorRef()
{
	assert(NULL!=m_pkMap);
	return *m_pkMap;
}

CMapManager::CMapManager() : mc_pcurEnvironmentData(NULL)
{
	m_pkMap = NULL;
	m_isSoftwareTilingEnableReserved=false;
#if defined(ENABLE_FOG_FIX)
	m_iFogLevel = 1;
#endif
//	Initialize();
}

CMapManager::~CMapManager()
{
	Destroy();
}

bool CMapManager::IsSoftwareTilingEnable()
{
	return CTerrainPatch::SOFTWARE_TRANSFORM_PATCH_ENABLE;
}

void CMapManager::ReserveSoftwareTilingEnable(bool isEnable)
{
	m_isSoftwareTilingEnableReserved=isEnable;
}

void CMapManager::Initialize()
{
	mc_pcurEnvironmentData = NULL;
	__LoadMapInfoVector();
}

void CMapManager::Create()
{
	assert(NULL==m_pkMap && "CMapManager::Create");
	if (m_pkMap)
	{
		Clear();
		return;
	}

	CTerrainPatch::SOFTWARE_TRANSFORM_PATCH_ENABLE=m_isSoftwareTilingEnableReserved;

	m_pkMap = (CMapOutdoor*)AllocMap();

	assert(NULL!=m_pkMap && "CMapManager::Create MAP is NULL");
}

void CMapManager::Destroy()
{
	stl_wipe_second(m_EnvironmentDataMap);

	if (m_pkMap)
	{
		m_pkMap->Clear();
		delete m_pkMap;
		m_pkMap = NULL;
	}
}

void CMapManager::Clear()
{
	if (m_pkMap)
		m_pkMap->Clear();
}

CMapBase * CMapManager::AllocMap()
{
	return new CMapOutdoor;
}

//////////////////////////////////////////////////////////////////////////
// Map
//////////////////////////////////////////////////////////////////////////
void CMapManager::LoadProperty()
{
	CPropertyLoader PropertyLoader;
	PropertyLoader.SetPropertyManager(&m_PropertyManager);
	PropertyLoader.Create("*.*", "Property");
}

bool CMapManager::LoadMap(const std::string & c_rstrMapName, float x, float y, float z)
{
	CMapOutdoor& rkMap = GetMapOutdoorRef();

	rkMap.Leave();
	rkMap.SetName(c_rstrMapName);
	rkMap.LoadProperty();
#if defined(ANK_SYSTEM_GRAPHICS)
	rkMap.currentMapName = c_rstrMapName;
	Frustum::Instance().currentMapNameSave = c_rstrMapName;
#endif

	if ( CMapBase::MAPTYPE_INDOOR == rkMap.GetType())
	{
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_DYNAMIC_LIGHT)
		Frustum::Instance().isIndoor = true;
		if (!rkMap.Load(x, y, z))
		{
			TraceError("CMapManager::LoadMap() Outdoor Map Load Failed");
			return false;
		}

		RegisterEnvironmentData(0, rkMap.GetEnvironmentDataName().c_str());

		SetEnvironmentData(0);
#else
		TraceError("CMapManager::LoadMap() Indoor Map Load Failed");
		return false;
#endif
	}
	else if (CMapBase::MAPTYPE_OUTDOOR == rkMap.GetType())
	{
#ifdef DIRECTX9
		Frustum::Instance().isIndoor = false;
#endif
		if (!rkMap.Load(x, y, z))
		{
			TraceError("CMapManager::LoadMap() Outdoor Map Load Failed");
			return false;
		}

		RegisterEnvironmentData(0, rkMap.GetEnvironmentDataName().c_str());

		SetEnvironmentData(0);
	}
	else
	{
		TraceError("CMapManager::LoadMap() Invalid Map Type");
		return false;
	}

	rkMap.Enter();

	return true;
}

bool CMapManager::IsMapReady()
{
	if (!m_pkMap)
		return false;

	return m_pkMap->IsReady();
}

bool CMapManager::UnloadMap(const std::string c_strMapName)
{
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	if (c_strMapName != rkMap.GetName() && "" != rkMap.GetName())
	{
		LogBoxf("%s: Unload Map Failed", c_strMapName.c_str());
		return false;
	}

	Clear();
	return true;
}

bool CMapManager::UpdateMap(float fx, float fy, float fz)
{
	if (!m_pkMap)
		return false;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.Update(fx, -fy, fz);
}

void CMapManager::UpdateAroundAmbience(float fx, float fy, float fz)
{
	if (!m_pkMap)
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.UpdateAroundAmbience(fx, -fy, fz);
}

float CMapManager::GetHeight(float fx, float fy)
{
	if (!m_pkMap)
	{
		TraceError("CMapManager::GetHeight(%f, %f) - Accessing in a not initialized map", fx, fy);
		return 0.0f;
	}
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetHeight(fx, fy);
}

float CMapManager::GetTerrainHeight(float fx, float fy)
{
	if (!m_pkMap)
	{
		TraceError("CMapManager::GetTerrainHeight(%f, %f) - Accessing in a not initialized map", fx, fy);
		return 0.0f;
	}
	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetTerrainHeight(fx, fy);
}

bool CMapManager::GetWaterHeight(int iX, int iY, long * plWaterHeight)
{
	if (!m_pkMap)
	{
		TraceError("CMapManager::GetTerrainHeight(%f, %f) - Accessing in a not initialized map", iX, iY);
		return false;
	}

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetWaterHeight(iX, iY, plWaterHeight);
}

//////////////////////////////////////////////////////////////////////////
// Environment
//////////////////////////////////////////////////////////////////////////
void CMapManager::BeginEnvironment()
{
	if (!m_pkMap)
		return;

	if (!mc_pcurEnvironmentData)
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();

	// Light always on
 	STATEMANAGER.SaveRenderState(D3DRS_LIGHTING, TRUE);

	// Fog
 	STATEMANAGER.SaveRenderState(D3DRS_FOGENABLE, mc_pcurEnvironmentData->bFogEnable);

	// Material
	STATEMANAGER.SetMaterial(&mc_pcurEnvironmentData->Material);

	// Directional Light
	if (mc_pcurEnvironmentData->bDirLightsEnable[ENV_DIRLIGHT_BACKGROUND])
	{
		ms_lpd3dDevice->LightEnable(0, TRUE);

		rkMap.ApplyLight((DWORD)mc_pcurEnvironmentData, mc_pcurEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND]);
	}
	else
		ms_lpd3dDevice->LightEnable(0, FALSE);

	if (mc_pcurEnvironmentData->bFogEnable)
	{
		STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, TRUE);
		DWORD dwFogColor = mc_pcurEnvironmentData->FogColor;
		STATEMANAGER.SetRenderState(D3DRS_FOGCOLOR, dwFogColor);

		if (mc_pcurEnvironmentData->bDensityFog)
		{
			float fDensity = 0.00015f;
			STATEMANAGER.SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_EXP);					// pixel fog
			STATEMANAGER.SetRenderState(D3DRS_FOGDENSITY, *((DWORD *) &fDensity));			// vertex fog
		}
		else
		{
#if defined(ANK_SYSTEM_GRAPHICS) || defined(DIRECTX9)
			CSpeedTreeForest& rkForest= CSpeedTreeForest::Instance();
#else
			CSpeedTreeForestDirectX8& rkForest=CSpeedTreeForestDirectX8::Instance();
#endif
			rkForest.SetFog(
				mc_pcurEnvironmentData->GetFogNearDistance(),
				mc_pcurEnvironmentData->GetFogFarDistance()
			);

			float fFogNear=mc_pcurEnvironmentData->GetFogNearDistance();
			float fFogFar=mc_pcurEnvironmentData->GetFogFarDistance();
			STATEMANAGER.SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);								// vertex fox
			STATEMANAGER.SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);											// vertex fox
			STATEMANAGER.SetRenderState(D3DRS_FOGSTART, *((DWORD *) &fFogNear));	// USED BY D3DFOG_LINEAR
			STATEMANAGER.SetRenderState(D3DRS_FOGEND, *((DWORD *) &fFogFar));		// USED BY D3DFOG_LINEAR
		}
	}
	else
	{
		STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);
		DWORD dwZero = 0;
		STATEMANAGER.SetRenderState(D3DRS_FOGSTART, dwZero);
		STATEMANAGER.SetRenderState(D3DRS_FOGEND, dwZero);
	}

	rkMap.OnBeginEnvironment();
}

void CMapManager::EndEnvironment()
{
	if (!mc_pcurEnvironmentData)
		return;

	STATEMANAGER.RestoreRenderState(D3DRS_LIGHTING);
	STATEMANAGER.RestoreRenderState(D3DRS_FOGENABLE);
}

void CMapManager::SetEnvironmentData(int nEnvDataIndex)
{
	const TEnvironmentData * c_pEnvironmenData;

	if (GetEnvironmentData(nEnvDataIndex, &c_pEnvironmenData))
		SetEnvironmentDataPtr(c_pEnvironmenData);
}

void CMapManager::SetEnvironmentDataPtr(const TEnvironmentData * c_pEnvironmentData)
{
	if (!m_pkMap)
		return;

	if (!c_pEnvironmentData)
	{
		assert(!"null environment data");
		TraceError("null environment data");
		return;
	}

	CMapOutdoor& rkMap=GetMapOutdoorRef();

	mc_pcurEnvironmentData = c_pEnvironmentData;

	rkMap.SetEnvironmentDataPtr(mc_pcurEnvironmentData);

#if defined(ENABLE_FOG_FIX)
	if (m_iFogLevel >= 0)
		SetFogLevel(m_iFogLevel);
#endif
}

void CMapManager::ResetEnvironmentDataPtr(const TEnvironmentData * c_pEnvironmentData)
{
	if (!m_pkMap)
		return;

	if (!c_pEnvironmentData)
	{
		assert(!"null environment data");
		TraceError("null environment data");
		return;
	}

	CMapOutdoor& rkMap=GetMapOutdoorRef();

	mc_pcurEnvironmentData = c_pEnvironmentData;
	rkMap.ResetEnvironmentDataPtr(mc_pcurEnvironmentData);
}

void CMapManager::BlendEnvironmentData(const TEnvironmentData * c_pEnvironmentData, int iTransitionTime)
{
}

#if defined(ENABLE_FOG_FIX)
void CMapManager::SetEnvironmentFog(bool flag)
{
	if (mc_pcurEnvironmentData)
	{
		const_cast<TEnvironmentData*>(mc_pcurEnvironmentData)->bFogEnable = flag;
		ResetEnvironmentDataPtr(mc_pcurEnvironmentData);
	}
}

bool CMapManager::GetEnvironmentFog()
{
	if (!mc_pcurEnvironmentData)
		return false;

	return mc_pcurEnvironmentData->bFogEnable ? true : false;
}

void CMapManager::SetFogLevel(int level)
{
	m_iFogLevel = level;
	if (!mc_pcurEnvironmentData)
		return;

	TEnvironmentData* pEnv = const_cast<TEnvironmentData*>(mc_pcurEnvironmentData);

	// Capture the map's authored fog once. Previously every level overwrote it with fixed
	// distances (and forced fog on with the default 0.5 grey on maps authored without fog),
	// which is what made every map look like flat grey paint.
	if (!pEnv->m_bFogAuthoredSaved)
	{
		pEnv->m_bFogAuthoredSaved = true;
		pEnv->m_bFogAuthoredEnable = pEnv->bFogEnable;
		pEnv->m_fFogAuthoredNear = pEnv->m_fFogNearDistance;
		pEnv->m_fFogAuthoredFar = pEnv->m_fFogFarDistance;
	}

	if (level == 0 || !pEnv->m_bFogAuthoredEnable || pEnv->m_fFogAuthoredFar <= pEnv->m_fFogAuthoredNear)
	{
		pEnv->bFogEnable = FALSE;
		pEnv->m_fFogNearDistance = 900000.0f;
		pEnv->m_fFogFarDistance = 900000.0f;
		STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);
		DWORD dwZero = 0;
		STATEMANAGER.SetRenderState(D3DRS_FOGSTART, dwZero);
		STATEMANAGER.SetRenderState(D3DRS_FOGEND, dwZero);
	}
	else
	{
		pEnv->bFogEnable = TRUE;
		// 1 = as authored, 2 = denser, 3 = heavy
		const float fScale = (level == 2) ? 0.6f : (level >= 3) ? 0.3f : 1.0f;
		float fNear = pEnv->m_fFogAuthoredNear * fScale;
		float fFar = pEnv->m_fFogAuthoredFar * fScale;

		pEnv->m_fFogNearDistance = fNear;
		pEnv->m_fFogFarDistance = fFar;
		STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, TRUE);
		STATEMANAGER.SetRenderState(D3DRS_FOGSTART, *((DWORD*)&fNear));
		STATEMANAGER.SetRenderState(D3DRS_FOGEND, *((DWORD*)&fFar));
	}

	if (m_pkMap)
	{
		CMapOutdoor& rkMap = GetMapOutdoorRef();
		rkMap.ResetEnvironmentDataPtr(mc_pcurEnvironmentData);
	}
}

int CMapManager::GetFogLevel() const
{
	return m_iFogLevel;
}
#endif

bool CMapManager::RegisterEnvironmentData(DWORD dwIndex, const char * c_szFileName)
{
	TEnvironmentData * pEnvironmentData = AllocEnvironmentData();

	if (!LoadEnvironmentData(c_szFileName, pEnvironmentData))
	{
		DeleteEnvironmentData(pEnvironmentData);
		return false;
	}

	TEnvironmentDataMap::iterator f=m_EnvironmentDataMap.find(dwIndex);
	if (m_EnvironmentDataMap.end()==f)
	{
		m_EnvironmentDataMap.insert(TEnvironmentDataMap::value_type(dwIndex, pEnvironmentData));
	}
	else
	{
		delete f->second;
		f->second=pEnvironmentData;
	}
	return true;
}

void CMapManager::GetCurrentEnvironmentData(const TEnvironmentData ** c_ppEnvironmentData)
{
	*c_ppEnvironmentData = mc_pcurEnvironmentData;
}

bool CMapManager::GetEnvironmentData(DWORD dwIndex, const TEnvironmentData ** c_ppEnvironmentData)
{
	TEnvironmentDataMap::iterator itor = m_EnvironmentDataMap.find(dwIndex);

	if (m_EnvironmentDataMap.end() == itor)
	{
		*c_ppEnvironmentData = NULL;
		return false;
	}

	*c_ppEnvironmentData = itor->second;
	return true;
}

void CMapManager::RefreshPortal()
{
	if (!IsMapReady())
		return;

	CMapOutdoor & rMap = GetMapOutdoorRef();
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CArea * pArea;
		if (!rMap.GetAreaPointer(i, &pArea))
			continue;

		pArea->RefreshPortal();
	}
}

void CMapManager::ClearPortal()
{
	if (!IsMapReady())
		return;

	CMapOutdoor & rMap = GetMapOutdoorRef();
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CArea * pArea;
		if (!rMap.GetAreaPointer(i, &pArea))
			continue;

		pArea->ClearPortal();
	}
}

void CMapManager::AddShowingPortalID(int iID)
{
	if (!IsMapReady())
		return;

	CMapOutdoor & rMap = GetMapOutdoorRef();
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CArea * pArea;
		if (!rMap.GetAreaPointer(i, &pArea))
			continue;

		pArea->AddShowingPortalID(iID);
	}
}

TEnvironmentData * CMapManager::AllocEnvironmentData()
{
	TEnvironmentData * pEnvironmentData = new TEnvironmentData;
	Environment_Init(*pEnvironmentData);
	return pEnvironmentData;
}

void CMapManager::DeleteEnvironmentData(TEnvironmentData * pEnvironmentData)
{
	delete pEnvironmentData;
	pEnvironmentData = NULL;
}

BOOL CMapManager::LoadEnvironmentData(const char * c_szFileName, TEnvironmentData * pEnvironmentData)
{
	if (!pEnvironmentData)
		return FALSE;

	return (BOOL)Environment_Load(*pEnvironmentData, c_szFileName);
}

DWORD CMapManager::GetShadowMapColor(float fx, float fy)
{
	if (!IsMapReady())
		return 0xFFFFFFFF;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetShadowMapColor(fx, fy);
}

std::vector<int> & CMapManager::GetRenderedSplatNum(int * piPatch, int * piSplat, float * pfSplatRatio)
{
	if (!m_pkMap)
	{
		static std::vector<int> s_emptyVector;
		*piPatch = 0;
		*piSplat = 0;
		return s_emptyVector;
	}

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetRenderedSplatNum(piPatch, piSplat, pfSplatRatio);
}

CArea::TCRCWithNumberVector & CMapManager::GetRenderedGraphicThingInstanceNum(DWORD * pdwGraphicThingInstanceNum, DWORD * pdwCRCNum)
{
	if (!m_pkMap)
	{
		static CArea::TCRCWithNumberVector s_emptyVector;
		*pdwGraphicThingInstanceNum = 0;
		*pdwCRCNum = 0;
		return s_emptyVector;
	}

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetRenderedGraphicThingInstanceNum(pdwGraphicThingInstanceNum, pdwCRCNum);
}

bool CMapManager::GetNormal(int ix, int iy, D3DXVECTOR3 * pv3Normal)
{
	if (!IsMapReady())
		return false;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetNormal(ix, iy, pv3Normal);
}

bool CMapManager::isPhysicalCollision(const D3DXVECTOR3 & c_rvCheckPosition)
{
	if (!IsMapReady())
		return false;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.isAttrOn(c_rvCheckPosition.x, -c_rvCheckPosition.y, CTerrainImpl::ATTRIBUTE_BLOCK);
}

bool CMapManager::isAttrOn(float fX, float fY, BYTE byAttr)
{
	if (!IsMapReady())
		return false;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.isAttrOn(fX, fY, byAttr);
}

#if defined(ANK_SYSTEM_GRAPHICS)
bool CMapManager::isPuddleOn(int fX, int fY, BYTE byAttr)
{
	if (!IsMapReady())
		return false;

	auto& rkMap = GetMapOutdoorRef();
	return rkMap.isPuddleOn(fX, fY, byAttr);
}
#endif

bool CMapManager::GetAttr(float fX, float fY, BYTE * pbyAttr)
{
	if (!IsMapReady())
		return false;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetAttr(fX, fY, pbyAttr);
}

bool CMapManager::isAttrOn(int iX, int iY, BYTE byAttr)
{
	if (!IsMapReady())
		return false;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.isAttrOn(iX, iY, byAttr);
}

bool CMapManager::GetAttr(int iX, int iY, BYTE * pbyAttr)
{
	if (!IsMapReady())
		return false;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetAttr(iX, iY, pbyAttr);
}

// 2004.10.14.myevan.TEMP_CAreaLoaderThread
/*
bool CMapManager::BGLoadingEnable()
{
	if (!IsMapReady())
		return false;
	return ((CMapOutdoor*)m_pMap)->BGLoadingEnable();
}

void CMapManager::BGLoadingEnable(bool bBGLoadingEnable)
{
	if (!IsMapReady())
		return;
	((CMapOutdoor*)m_pMap)->BGLoadingEnable(bBGLoadingEnable);
}
*/

void CMapManager::SetTerrainRenderSort(CMapOutdoor::ETerrainRenderSort eTerrainRenderSort)
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.SetTerrainRenderSort(eTerrainRenderSort);
}

void CMapManager::SetTransparentTree(bool bTransparenTree)
{
	if (!IsMapReady())
		return;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.SetTransparentTree(bTransparenTree);
}

CMapOutdoor::ETerrainRenderSort CMapManager::GetTerrainRenderSort()
{
	if (!IsMapReady())
		return CMapOutdoor::DISTANCE_SORT;

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	return rkMap.GetTerrainRenderSort();
}

void CMapManager::GetBaseXY(DWORD * pdwBaseX, DWORD * pdwBaseY)
{
	if (!IsMapReady())
	{
		*pdwBaseX = 0;
		*pdwBaseY = 0;
	}

	CMapOutdoor& rkMap=GetMapOutdoorRef();
	rkMap.GetBaseXY(pdwBaseX, pdwBaseY);
}

void CMapManager::__LoadMapInfoVector()
{
	CMappedFile kFile;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(kFile, m_stAtlasInfoFileName.c_str(), &pData))
		if (!CEterPackManager::Instance().Get(kFile, "AtlasInfo.txt", &pData))
			return;

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(kFile.Size(), pData);

	char szMapName[256];
	int x, y;
	int width, height;
	for (UINT uLineIndex=0; uLineIndex<textFileLoader.GetLineCount(); ++uLineIndex)
	{
		const std::string& c_rstLine=textFileLoader.GetLineString(uLineIndex);
		sscanf(c_rstLine.c_str(), "%s %d %d %d %d",
			szMapName,
			&x, &y, &width, &height);

		if ('\0'==szMapName[0])
			continue;

		TMapInfo kMapInfo;
		kMapInfo.m_strName = szMapName;
		kMapInfo.m_dwBaseX = x;
		kMapInfo.m_dwBaseY = y;

		kMapInfo.m_dwSizeX = width;
		kMapInfo.m_dwSizeY = height;

		kMapInfo.m_dwEndX = kMapInfo.m_dwBaseX + kMapInfo.m_dwSizeX * CTerrainImpl::TERRAIN_XSIZE;
		kMapInfo.m_dwEndY = kMapInfo.m_dwBaseY + kMapInfo.m_dwSizeY * CTerrainImpl::TERRAIN_YSIZE;

		m_kVct_kMapInfo.push_back(kMapInfo);
	}

	return;
}

