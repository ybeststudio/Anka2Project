// PythonBackground.h: interface for the CPythonBackground class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PYTHONBACKGROUND_H__A202CB18_9553_4CF3_8500_5D7062B55432__INCLUDED_)
#define AFX_PYTHONBACKGROUND_H__A202CB18_9553_4CF3_8500_5D7062B55432__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../gamelib/MapManager.h"
#include "../gamelib/TerrainDecal.h"
#include "../gamelib/SnowEnvironment.h"

class CInstanceBase;

class CPythonBackground : public CMapManager, public CSingleton<CPythonBackground>
{
public:
#if defined(ANK_SYSTEM_GRAPHICS)
	enum
	{
		GRAPHICS_OFF,
		GRAPHICS_MEDIUM,
		GRAPHICS_ULTRA,
	};

	enum ERainLevel
	{
		RAIN_0,
		RAIN_1,
		RAIN_2,
		RAIN_3,
		RAIN_4,
	};

	enum ETreeShadow
	{
		TREE_SHADOW_on,
		TREE_SHADOW_off,
		TREE_SHADOW_high,
		TREE_SHADOW_all,
	};

	enum ETerrainShadow
	{
		TERRAIN_SHADOW_off,
		TERRAIN_SHADOW_on,
	};

	enum EShadowQuality
	{
		SHADOW_QUALITY_BAD,
		SHADOW_QUALITY_AVERAGE,
		SHADOW_QUALITY_GOOD,
		SHADOW_QUALITY_ULTRA,
	};

	enum ESpecularQuality
	{
		SPECULAR_on,
		SPECULAR_off,
	};

	enum EBloodQuality
	{
		BLOOD_on,
		BLOOD_off,
	};

	enum EAmbianceEffectsQuality
	{
		AEFFECTS_on,
		AEFFECTS_off,
	};

	enum EGrassType
	{
		GRASS_off = 0,
		GRASS_on = 1,
		GRASS_0 = 0,
		GRASS_1 = 1,
		GRASS_2 = 2,
		GRASS_3 = 3,
	};

	enum EWindQuality
	{
		WIND_0 = 0,
		WIND_1 = 1,
		WIND_2 = 2,
		WIND_3 = 3,
	};

#endif
	enum
	{
		SHADOW_NONE,
		SHADOW_GROUND,
		SHADOW_GROUND_AND_SOLO,
		SHADOW_ALL,
		SHADOW_ALL_HIGH,
		SHADOW_ALL_MAX,
	};

	enum
	{
		DISTANCE0,
		DISTANCE1,
		DISTANCE2,
		DISTANCE3,
		DISTANCE4,
		NUM_DISTANCE_SET
	};

	enum
	{
		DAY_MODE_LIGHT,
		DAY_MODE_DARK,
	};

	typedef struct SVIEWDISTANCESET
	{
		float m_fFogStart;
		float m_fFogEnd;
		float m_fFarClip;
		D3DXVECTOR3 m_v3SkyBoxScale;
	} TVIEWDISTANCESET;

public:
	CPythonBackground();
	virtual ~CPythonBackground();

	void Initialize();

	void Destroy();
	void Create();

	// Character-select preloading of the building models around each character's position,
	// so the first warp finds them already in the resource cache.
	void QueueSelectScreenMapPreload(DWORD dwGlobalX, DWORD dwGlobalY);
	bool SelectScreenMapPreloadStep(DWORD dwBudgetMs);	// true when nothing is left
	void ReleaseSelectScreenMapPreload();
#if defined(ANK_SYSTEM_GRAPHICS)
	int SelectRain;
#endif
	void GlobalPositionToLocalPosition(LONG& rGlobalX, LONG& rGlobalY);
	void LocalPositionToGlobalPosition(LONG& rLocalX, LONG& rLocalY);

	void EnableTerrainOnlyForHeight();
	bool SetSplatLimit(int iSplatNum);
	bool SetVisiblePart(int ePart, bool isVisible);
	bool SetShadowLevel(int eLevel);

	void RefreshShadowLevel();
#if defined(ANK_SYSTEM_GRAPHICS)
	void RefreshGraphicsLevel();
	bool SetGraphicsLevel(int eLevel);
	bool SetRainLevel(int eLevel);
	bool SetTreeShadow(int eLevel);
	bool SetGrassType(int eLevel);
	bool SetTerrainShadow(int eLevel);
	bool SetSpecularLevel(int eLevel);
	bool SetShadowQualityLevel(int eLevel);
	bool SetBloodLevel(int eLevel);
	bool SetAmbianceEffectsLevel(int eLevel);
	bool SetShadowTypeLevel(int eLevel);
	bool SetWaterReflection(int eLevel);
	int GetWaterReflection();
	bool SetEnhancedFoliage(bool bEnable);
	bool SetSpeedTreeDynamicLight(bool bEnable);
#endif
	void SelectViewDistanceNum(int eNum);
	void SetViewDistanceSet(int eNum, float fFarClip);
	float GetFarClip();

	DWORD GetRenderShadowTime();
	void GetDistanceSetInfo(int * peNum, float * pfStart, float * pfEnd, float * pfFarClip);

	bool GetPickingPoint(D3DXVECTOR3 * v3IntersectPt);
	bool GetPickingPointWithRay(const CRay & rRay, D3DXVECTOR3 * v3IntersectPt);
	bool GetPickingPointWithRayOnlyTerrain(const CRay & rRay, D3DXVECTOR3 * v3IntersectPt);
	BOOL GetLightDirection(D3DXVECTOR3 & rv3LightDirection);

	void SetFallingLeaves(bool enabled);
	void UpdateFallingLeaves(float x, float y);
	void ClearFallingLeaves();
	bool m_bFallingLeaves = true;
	DWORD m_dwNextLeafUpdate = 0;
	std::vector<std::pair<D3DXVECTOR3, DWORD>> m_leafEffects;
	void Update(float fCenterX, float fCenterY, float fCenterZ);

	void CreateCharacterShadowTexture();
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_WATER_REFLECTION)
	void CreateReflectionTexture();
#endif
	void ReleaseCharacterShadowTexture();
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_WATER_REFLECTION)
	void ReleaseReflectionTexture();
#endif
	void Render();
	void RenderSnow();
	void RenderPCBlocker();
	void RenderCollision();
#if defined(ANK_SYSTEM_GRAPHICS)
	void RenderShadow();
#endif
	void RenderCharacterShadowToTexture();
#if defined(ANK_SYSTEM_GRAPHICS)
	void RenderCharacterShadowToTexture2();
	void RenderCharacterShadowToTexture3();
	void RenderCharacterShadowToTexture4();
	void RenderGrass();
#endif
	void RenderSky();
	void RenderCloud();
	void RenderWater();
	void RenderEffect();
	void RenderBeforeLensFlare();
	void RenderAfterLensFlare();

	bool CheckAdvancing(CInstanceBase * pInstance);

	void SetCharacterDirLight();
	void SetBackgroundDirLight();
#if defined(ANK_SYSTEM_GRAPHICS) || defined(ANKIRA_WATER_REFLECTION) || defined(ANKIRA_UNDERWATER_CAMERA)
	void GetUnderwaterCamera();
	void RenderWaterReflection();
	void RenderPuddleReflection();
#endif
	void ChangeToDay();
	void ChangeToNight();
	void EnableSnowEnvironment();
	void DisableSnowEnvironment();
	void SetXMaxTree(int iGrade);

	void ClearGuildArea();
	void RegisterGuildArea(int isx, int isy, int iex, int iey);

	void CreateTargetEffect(DWORD dwID, DWORD dwChrVID);
	void CreateTargetEffect(DWORD dwID, long lx, long ly);
	void DeleteTargetEffect(DWORD dwID);

	void CreateSpecialEffect(DWORD dwID, float fx, float fy, float fz, const char * c_szFileName);
	void DeleteSpecialEffect(DWORD dwID);

	void Warp(DWORD dwX, DWORD dwY);

	void VisibleGuildArea();
	void DisableGuildArea();

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
	void CreateSpecialShopPosEffect(DWORD dwID, float fx, float fy, float fz, const char *c_szFileName);
	void DeleteSpecialShopPosEffect();
#endif


	void RegisterDungeonMapName(const char * c_szMapName);
	TMapInfo* GlobalPositionToMapInfo(DWORD dwGlobalX, DWORD dwGlobalY);
	const char* GetWarpMapName();

#if defined(ANKIRA_ENVIRONMENT_EFFECT_OPTION)
public:
	bool IsBoomMap(const std::string& sMapName) const;
	bool IsSnowMap(const std::string& sMapName) const;
	int GetDayMode() const;

	void SetXMasShowEvent(bool bMode);
	bool GetXMasShowEvent() const;

	void SetNightModeOption(bool bMode);
	bool GetNightModeOption() const;

	void SetSnowModeOption(bool bMode);
	bool GetSnowModeOption() const;

private:
	bool m_bXMasShowEvent;
	bool m_bNightMode;
	bool m_bSnowMode;

	bool IsBoomMap();
#endif

protected:
	void __CreateProperty();
	void __EnsurePropertyCreated();

	std::vector<std::string> m_kVec_strPreloadAreaFile;
	std::vector<std::string> m_kVec_strPreloadModelFile;
	std::set<std::string> m_kSet_strPreloadQueued;
	std::vector<CResource*> m_kVec_pkPreloadedResource;
	bool __IsSame(std::set<int> & rleft, std::set<int> & rright);
	void UpdateWeather();

protected:
	std::string m_strMapName;

private:
	CSnowEnvironment m_SnowEnvironment;

	int m_iDayMode;
	int m_iXMasTreeGrade;

	int m_eShadowLevel;
#if defined(ANK_SYSTEM_GRAPHICS)
	int m_eGraphicsLevel;
	bool m_bWaterReflectionOption = true;	// "water_reflection" toggle; presets respect it
	int m_eShadowQualityLevel;
	int m_eShadowTargetLevel;
	int m_eTerrainShadow;
	int m_eTreeShadow;
	int m_eRainLevel;
	int m_eSpecularLevel;
	int m_eBloodLevel;
	int m_eAmbianceEffectsLevel;
	int m_eGrassType;
	DWORD m_dwNextThunderTime;
	DWORD m_dwThunderFlashEndTime;
#endif
	int m_eViewDistanceNum;

	BOOL m_bVisibleGuildArea;

	DWORD m_dwRenderShadowTime;

	DWORD m_dwBaseX;
	DWORD m_dwBaseY;

	TVIEWDISTANCESET m_ViewDistanceSet[NUM_DISTANCE_SET];

	std::set<int> m_kSet_iShowingPortalID;
	std::set<std::string> m_kSet_strDungeonMapName;
	std::map<DWORD, DWORD> m_kMap_dwTargetID_dwChrID;

	struct SReserveTargetEffect
	{
		int ilx;
		int ily;
	};
	std::map<DWORD, SReserveTargetEffect> m_kMap_dwID_kReserveTargetEffect;

	struct FFindWarpMapName
	{
		DWORD m_dwX, m_dwY;
		FFindWarpMapName(DWORD dwX, DWORD dwY)
		{
			m_dwX = dwX;
			m_dwY = dwY;
		}
		bool operator() (TMapInfo & rMapInfo)
		{
			if (m_dwX < rMapInfo.m_dwBaseX || m_dwX >= rMapInfo.m_dwEndX || m_dwY < rMapInfo.m_dwBaseY || m_dwY >= rMapInfo.m_dwEndY)
				return false;
			return true;
		}
	};
#if defined(ANK_SYSTEM_GRAPHICS)
	public:
		int Collision;
		IDirect3DPixelShader9* pShader = NULL;
		IDirect3DPixelShader9* pShader2 = NULL;
		IDirect3DPixelShader9* pBlurShadowBuilding = NULL;
#endif
};

#endif // !defined(AFX_PYTHONBACKGROUND_H__A202CB18_9553_4CF3_8500_5D7062B55432__INCLUDED_)

