#pragma once

#include "../../GameLib/MapManager.h"
#include "../../GameLib/MonsterAreaInfo.h"

class CShadowRenderHelper;
class CPropertyTreeControler;
class CMapOutdoor;
class CMapOutdoorAccessor;
class CTerrainAccessor;
class CMonsterAreaInfo;

class CMapManagerAccessor : public CMapManager
{
	public:
		class CHeightObserver
		{
			public:
				CHeightObserver (CMapOutdoor * pMapOutdoor) : m_pMap (pMapOutdoor) {}
				~CHeightObserver() {}

				float GetHeight (float fx, float fy);
				CMapOutdoor* m_pMap;
		};

	public:
		CMapManagerAccessor();
		virtual ~CMapManagerAccessor();

		void				Initialize();
		void				Destroy();

		void				DestroyShadowTexture();
		void				RecreateShadowTexture();

		virtual void		Clear();
		virtual CMapBase* 	AllocMap();
		virtual bool		LoadMap (const std::string & c_rstrMapName);

		void				UpdateEditing();

		void				ToggleWireframe();
		void				SetWireframe (bool isWireframe);

		// Cursor
		void				SetEditingCursorPosition (const D3DXVECTOR3 & c_rv3Position);

		// Wind
		void				SetWindStrength (float fStrength);
		void				SetWindRandom (float fRandom);

		// Material
		void				SetMaterialDiffuseColor (float fr, float fg, float fb);
		void				SetMaterialAmbientColor (float fr, float fg, float fb);
		void				SetMaterialEmissiveColor (float fr, float fg, float fb);

		// Light
		void				SetLightDirection (float fx, float fy, float fz);
		void				SetLightDiffuseColor (float fr, float fg, float fb);
		void				SetLightAmbientColor (float fr, float fg, float fb);
		void				EnableLight (BOOL bFlag);

		// Fog
		void				EnableFog (BOOL bFlag);
		void				SetFogColor (float fr, float fg, float fb);
		void				SetFogNearDistance (float fDistance);
		void				SetFogFarDistance (float fDistance);
		void				EnableFiltering (BOOL bFlag);

		// Filtering
		void				SetFilteringColor (float fr, float fg, float fb);
		void				SetFilteringAlpha (float fAlpha);
		void				SetFilteringAlphaSrc (BYTE byAlphaSrc);
		void				SetFilteringAlphaDest (BYTE byAlphaDest);

		// SkyBox
		void				SetSkyBoxTextureRenderMode (BOOL bTextureMode);
		BOOL				IsSkyBoxTextureRenderMode();

		void				SetSkyBoxFaceTexture (const char* pFileName, int iFaceIndex);
		std::string		    GetSkyBoxFaceTextre (int iFaceIndex);  // 부하가 있는건 알지만 상관없다 툴이자나.

		D3DXVECTOR3&			GetSkyBoxScaleReference();
		D3DXVECTOR2& 		GetSkyBoxCloudScaleReference();
		D3DXVECTOR2& 		GetSkyBoxCloudTextureScaleReference();
		D3DXVECTOR2& 		GetSkyBoxCloudSpeedReference();
		float& 				GetSkyBoxCloudHeightReference();
		std::string& 		GetSkyBoxCloudTextureFileNameReference();
		BYTE& 				GetSkyBoxGradientUpperReference();
		BYTE& 				GetSkyBoxGradientLowerReference();
		BOOL				GetSkyBoxGradientColorPointer (DWORD dwIndex, TGradientColor** ppGradientColor);
		void				InsertGradientUpper();
		void				InsertGradientLower();
		void				DeleteGradient (DWORD dwIndex);

		// LensFlare
		BOOL& 				GetLensFlareEnableReference();
		D3DXCOLOR& 			GetLensFlareBrightnessColorReference();
		float& 				GetLensFlareMaxBrightnessReference();
		BOOL& 				GetMainFlareEnableReference();
		std::string& 		GetMainFlareTextureFileNameReference();
		float& 				GetMainFlareSizeReference();

		////////////////////////////////////////////
		// Environment
		void				GetEnvironmentData (const TEnvironmentData** c_ppEnvironmentData);
		void				RefreshEnvironmentData();

		void				InitializeEnvironmentData();
		void				RefreshScreenFilter();
		void				RefreshSkyBox();
		void				RefreshLensFlare();
		void				LoadEnvironmentScript (const char* c_szFileName);
		void				SaveEnvironmentScript (const char* c_szFileName);
		////////////////////////////////////////////

		////////////////////////////////////////////
		// About Brush
		// NOTE : Replace to the CSceneMap
		void				SetBrushShape (DWORD dwShape);
		void				SetBrushType (DWORD dwType);

		BYTE				GetBrushSize()
		{
			return m_byBrushSize;
		}
		BYTE				GetBrushSizeY()
		{
			return m_byBrushSizeY;
		}
		void				SetBrushSize (BYTE ucBrushSize);
		void				SetBrushSizeY (BYTE ucBrushSize);
		void				SetMaxBrushSize (BYTE ucMaxBrushSize);
		BYTE				GetBrushStrength()
		{
			return m_byBrushStrength;
		}
		void				SetBrushStrength (BYTE ucBrushSize);
		void				SetMaxBrushStrength (BYTE ucMaxBrushStrength);
		long				GetBrushWaterHeight()
		{
			return m_lBrushWaterHeight;
		}
		void				SetBrushWaterHeight (long wBrushWaterHeight);

		void				SetTextureBrushVector (std::vector<BYTE>& rTextureBrushNumberVector);

		void				SetSelectedAttrFlag (BYTE bFlag);
		BYTE 				GetSelectedAttrFlag()
		{
			return m_bySelectedAttrFlag;
		}

		void				GetEditArea (int* iEditX,
							  int* iEditY,
							  unsigned char* usSubEditX,
							  unsigned char* usSubEditY,
							  WORD * usTerrainNumX,
							  WORD * usTerrainNumY);

		DWORD				GetBrushShape()
		{
			return m_dwBrushShape;
		}

		void				EditingStart();
		void				EditingEnd();

		void				SetTerrainModified();

		void				SetHeightEditing (bool bOn)
		{
			m_bHeightEditingOn = bOn;
		}
		void				SetTextureEditing (bool bOn)
		{
			m_bTextureEditingOn = bOn;
		}
		void				SetWaterEditing (bool bOn)
		{
			m_bWaterEditingOn = bOn;
		}
		void				SetAttrEditing (bool bOn)
		{
			m_bAttrEditingOn = bOn;
		}
		void				SetMonsterAreaInfoEditing (bool bOn)
		{
			m_bMonsterAreaInfoEditingOn = bOn;
		}
		const bool			isHeightEditing()
		{
			return m_bHeightEditingOn;
		}
		const bool			isTextureEditing()
		{
			return m_bTextureEditingOn;
		}
		const bool			isWaterEditing()
		{
			return m_bWaterEditingOn;
		}
		const bool			isAttrEditing()
		{
			return m_bAttrEditingOn;
		}
		const bool			isMonsterAreaInfoEditing()
		{
			return m_bMonsterAreaInfoEditingOn;
		}
		////////////////////////////////////////////

		////////////////////////////////////////////
		// Texture Set
		bool	AddTerrainTexture (const char* pFilename);
		bool	RemoveTerrainTexture (long lTexNum);
		void	ResetTerrainTexture();
		// Texture Set
		////////////////////////////////////////////

		////////////////////////////////////////////
		// Base Texture
		void	SetInitTextureBrushVector (std::vector<BYTE>& rTextureBrushNumberVector);
		bool	InitBaseTexture (const char* c_szMapName = NULL);
		// Base Texture
		////////////////////////////////////////////

		////////////////////////////////////////////
		// About Object Picking & Control
		int GetPickedPickedObjectIndex();
		BOOL IsSelectedObject (int iIndex);
		int GetSelectedObjectCount();
		void SelectObject (int iIndex);
		BOOL Picking();

		void RenderSelectedObject();
		void CancelSelect();
		BOOL SelectObject (float fxStart, float fyStart, float fxEnd, float fyEnd);
		void DeleteSelectedObject();
		void MoveSelectedObject (float fx, float fy);
		void MoveSelectedObjectHeight (float fHeight);
		void AddSelectedAmbienceScale (int iAddScale);
		void AddSelectedAmbienceMaxVolumeAreaPercentage (float fPercentage);
		void AddSelectedObjectRotation (float fYaw, float fPitch, float fRoll);
		void SetSelectedObjectPortalNumber (int iID);
		void DelSelectedObjectPortalNumber (int iID);
		void CollectPortalNumber (std::set<int>* pkSet_iPortalNumber);
		void EnablePortal (BOOL bFlag);
		BOOL IsSelected();

		void InsertObject (float fx, float fy, float fz, int iRotation, DWORD dwCRC);
		void InsertObject (float fx, float fy, float fz, float fYaw, float fPitch, float fRoll, DWORD dwScale, DWORD dwCRC);
		void RefreshObjectHeight (float fx, float fy, float fHalfSize);
		// About Object Picking
		////////////////////////////////////////////

		////////////////////////////////////////////
		// For Undo System
		void BackupObject();
		void BackupObjectCurrent();
		void BackupTerrain();
		void BackupTerrainCurrent();
		////////////////////////////////////////////

		////////////////////////////////////////////
		bool SaveMapProperty (const std::string & c_rstrFolder);		// 전체 맵 Property를 저장한다.
		bool SaveMapSetting (const std::string & c_rstrFolder);		// 전체 맵 Setting을 저장한다.
		////////////////////////////////////////////

		////////////////////////////////////////////
		// 새로운 맴버들
		void InitMap();
		bool NewMap (const char* c_szMapName);
		bool SaveMap (const char* c_szMapName = NULL);				// 전체 맵 관련 데이타를 저장한다.
		bool SaveTerrains();										// 현재 메모리에 있는 최대 9개 Terrain 전체를 저장한다.
		bool SaveAreas();											// 현재 메모리에 있는 최대 9개 Area 전체를 저장한다.

		void SetNewMapName (const char* c_szNewMapName)
		{
			m_strNewMapName = c_szNewMapName;
		}
		void SetNewMapSizeX (WORD wNewMapSizeX)
		{
			m_wNewMapSizeX = wNewMapSizeX;
		}
		void SetNewMapSizeY (WORD wNewMapSizeY)
		{
			m_wNewMapSizeY = wNewMapSizeY;
		}

		bool CreateNewOutdoorMap();

		BOOL GetEditArea (CAreaAccessor** ppAreaAccessor);
		BOOL GetArea (const BYTE & c_ucAreaNum, CAreaAccessor** ppAreaAccessor);

		BOOL GetEditTerrain (CTerrainAccessor** ppTerrainAccessor);
		BOOL GetTerrain (const BYTE & c_ucTerrainNum, CTerrainAccessor** ppTerrainAccessor);

		const BYTE GetTerrainNum (float fx, float fy);
		const BYTE GetEditTerrainNum();

		void RefreshArea();
		////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Attr
		void SetEraseAttr (const bool & bErase)
		{
			m_bEraseAttr = bErase;
		}
		void RenderAttr();
		bool ResetToDefaultAttr();
		void RenderObjectCollision();
		// Attr
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Water
		void SetEraseWater (const bool & bErase)
		{
			m_bEraseWater = bErase;
		}
		void PreviewEditWater();
		// Water
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Texture
		void SetEraseTexture (const bool & bErase)
		{
			m_bEraseTexture = bErase;
		}
		void SetDrawOnlyOnBlankTile (const bool & bOn)
		{
			m_bDrawOnlyOnBlankTile = bOn;
		}
		// Texture
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Shadow
		void UpdateTerrainShadowMap();
		void ReloadTerrainShadowTexture();
		// Shadow
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// MiniMap
		void SaveMiniMap();
		void SaveAtlas();
		// MiniMap
		//////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////
		// Monster
		void ShowAllMonsterAreaInfo (bool bShow)
		{
			m_bShowAllMonsterAreaInfo = bShow;
		}
		bool ShowAllMonsterAreaInfo()
		{
			return m_bShowAllMonsterAreaInfo;
		}

		bool SaveMonsterAreaInfo();

		void RemoveMonsterAreaInfoPtr (CMonsterAreaInfo * pMonsterAreaInfo);

		void SetNewMonsterVID (DWORD dwVID)
		{
			m_dwNewMonsterVID = dwVID;
		}
		void SetNewMonsterCount (DWORD dwCount)
		{
			m_dwNewMonsterCount = dwCount;
		}
		void SetNewMonsterDir (CMonsterAreaInfo::EMonsterDir eNewMonsterDir)
		{
			m_eNewMonsterDir = eNewMonsterDir;
		}

		void SetNewMonsterAreaInfoType (CMonsterAreaInfo::EMonsterAreaInfoType eNewMonsterAreaInfoType)
		{
			m_eNewMonsterAreaInfoType = eNewMonsterAreaInfoType;
		}

		CMonsterAreaInfo* AddNewMonsterAreaInfo (long lOriginX, long lOriginY, long lSizeX, long lSizeY,
												 CMonsterAreaInfo::EMonsterAreaInfoType eMonsterAreaInfoType,
												 DWORD dwVID, DWORD dwCount, CMonsterAreaInfo::EMonsterDir eNewMonsterDir);
		void SelectMonsterAreaInfo();
		void SelectNextMonsterAreaInfo (bool bForward = true);
		CMonsterAreaInfo* GetSelectedMonsterAreaInfo();

		void SelectMonsterAreaInfoStart()
		{
			m_bSelectMonsterAreaInfoStart = true;
		}
		void SelectMonsterAreaInfoEnd()
		{
			m_bSelectMonsterAreaInfoStart = false;
		}
		bool isSelectMonsterAreaInfoStarted()
		{
			return m_bSelectMonsterAreaInfoStart;
		}

		DWORD GetMonsterAreaInfoCount();
		bool GetMonsterAreaInfoFromVectorIndex (DWORD dwMonsterAreaInfoVectorIndex, CMonsterAreaInfo** ppMonsterAreaInfo);
		////////////////////////////////////////////

		////////////////////////////////////////////
		// ETC
		void LoadProperty (CPropertyTreeControler * pTreeControler);
		void SaveCollisionData (const char* c_szFileName);

		void UpdateHeightFieldEditingPt (D3DXVECTOR3 * v3IntersectPt);
		CHeightObserver* GetHeightObserverPointer();

		void RenderGuildArea();
		void UpdateMapInfo();
		CMapOutdoorAccessor* GetMapOutdoorPtr();
		////////////////////////////////////////////

	protected:
		void EditTerrain();
		void EditTerrainTexture();
		void EditWater();
		void EditAttr();
		void EditMonsterAreaInfo();
		void __LoadMapInfoVector();
		void __AddMapInfo();
		void __RefreshMapID (const char* c_szMapName);

	protected:
		DWORD m_dwBrushShape;
		DWORD m_dwBrushType;

		BYTE m_byBrushSize;
		BYTE m_byBrushSizeY;
		BYTE m_byBrushStrength;
		long m_lBrushWaterHeight;

		BYTE m_byMAXBrushSize;
		BYTE m_byMAXBrushStrength;

		std::vector<BYTE> m_TextureBrushNumberVector;
		std::vector<BYTE> m_InitTextureBrushNumberVector;

		int m_ixEdit;
		int m_iyEdit;

		BYTE m_bySubCellX;
		BYTE m_bySubCellY;
		WORD m_wEditTerrainNumX;
		WORD m_wEditTerrainNumY;

		bool m_bEditingInProgress;
		bool m_bHeightEditingOn;
		bool m_bTextureEditingOn;
		bool m_bWaterEditingOn;
		bool m_bAttrEditingOn;
		bool m_bMonsterAreaInfoEditingOn;

		// 포인터 저장용...
		CMapOutdoorAccessor* 	m_pMapAccessor;
		CHeightObserver* 		m_pHeightObserver;

		// NewMap 관련...
		std::string				m_strNewMapName;
		WORD					m_wNewMapSizeX, m_wNewMapSizeY;

		WORD					m_wOldEditX, m_wOldEditY;

		// Attr
		BYTE					m_bySelectedAttrFlag;
		bool					m_bEraseAttr;

		// Water
		bool					m_bEraseWater;

		// Texture
		bool					m_bEraseTexture;
		bool					m_bDrawOnlyOnBlankTile;

		// Environment
		TEnvironmentData		m_EnvironmentData;

		// CursorPosition
		D3DXVECTOR3				m_v3EditingCursorPosition;

		// Monster
	protected:
		bool					m_bShowAllMonsterAreaInfo;
		DWORD					m_dwNewMonsterVID;
		DWORD					m_dwNewMonsterCount;
		CMonsterAreaInfo::EMonsterAreaInfoType	m_eNewMonsterAreaInfoType;
		CMonsterAreaInfo::EMonsterDir			m_eNewMonsterDir;
		std::vector<CMonsterAreaInfo*>			m_pSelectedMonsterAreaInfoVector;
		std::map<std::string, int>				m_kMap_strMapName_iID;
		DWORD					m_dwSelectedMonsterAreaInfoIndex;
		bool					m_bSelectMonsterAreaInfoStart;
		long					m_lOldOriginX;
		long					m_lOldOriginY;
};
