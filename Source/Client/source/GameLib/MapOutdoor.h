#pragma once

#include "StdAfx.h"

#include "../eterLib/SkyBox.h"
#include "../eterLib/LensFlare.h"
#include "../eterLib/ScreenFilter.h"

#include "../PRTerrainLib/TerrainType.h"
#include "../PRTerrainLib/TextureSet.h"

#include "../SpeedTreeLib/SpeedTreeForestDirectX8.h"
#include "../SpeedTreeLib/SpeedTreeWrapper.h"
#include "../SphereLib/frustum.h"
typedef std::shared_ptr<CSpeedTreeWrapper> SpeedTreeWrapperPtr;

#include "MapBase.h"
#include "Area.h"
#include "AreaTerrain.h"
#include "AreaLoaderThread.h"

#include "MonsterAreaInfo.h"


#define LOAD_SIZE_WIDTH				1

#define AROUND_AREA_NUM				1+(LOAD_SIZE_WIDTH*2)*(LOAD_SIZE_WIDTH*2)*2
#define MAX_PREPARE_SIZE			9
#define MAX_MAPSIZE					256		// 0 ~ 255, cellsize 200 = 64km

#define TERRAINPATCH_LODMAX		3

struct GrassVertex
{
	float x, y, z;
	float u, v;
	static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
};

typedef struct SOutdoorMapCoordinate
{
	short m_sTerrainCoordX;
	short m_sTerrainCoordY;
} TOutdoorMapCoordinate;

typedef std::map<const std::string, TOutdoorMapCoordinate> TOutdoorMapCoordinateMap;

class CTerrainPatchProxy;
class CTerrainQuadtreeNode;

class CMapOutdoor : public CMapBase
{
	public:
		enum
		{
			VIEW_NONE = 0,
			VIEW_PART,
			VIEW_ALL,
		};

		enum EPart
		{
			PART_TERRAIN,
			PART_OBJECT,
			PART_CLOUD,
			PART_WATER,
			PART_TREE,
			PART_SKY,
			PART_NUM,
		};

		enum ETerrainRenderSort
		{
			DISTANCE_SORT,
			TEXTURE_SORT,
		};

	public:
		CMapOutdoor();
		virtual ~CMapOutdoor();

		virtual void	OnBeginEnvironment();

	protected:
		bool			Initialize();
		void			InitializeFog();

		virtual bool	Destroy();
		virtual void	OnSetEnvironmentDataPtr();
		virtual void	OnResetEnvironmentDataPtr();

		virtual void	OnRender();

		virtual void	OnPreAssignTerrainPtr() {};

	public:
		void			SetInverseViewAndDynamicShaodwMatrices();
		virtual bool 	Load(float x, float y, float z);
		virtual float	GetHeight(float x, float y);
		virtual float	GetCacheHeight(float x, float y);

		virtual bool	Update(float fX, float fY, float fZ);
		virtual void	UpdateAroundAmbience(float fX, float fY, float fZ);

	public:
		void			Clear();

		void			SetVisiblePart(int ePart, bool isVisible);
		void			SetSplatLimit(int iSplatNum);
		std::vector<int> &	GetRenderedSplatNum(int * piPatch, int * piSplat, float * pfSplatRatio);
		CArea::TCRCWithNumberVector & GetRenderedGraphicThingInstanceNum(DWORD * pdwGraphicThingInstanceNum, DWORD * pdwCRCNum);

		bool LoadSetting(const char *c_szFileName);

        void ApplyLight(DWORD dwVersion
#ifdef ENABLE_DIRECTX9_UPDATE
, const D3DLIGHT9& c_rkLight
#else
, const D3DLIGHT8& c_rkLight
#endif
        );

		void			SetEnvironmentScreenFilter();
		void			SetEnvironmentSkyBox();
		void			SetEnvironmentLensFlare();

		void			MapTextureReflection();

		void			CreateCharacterShadowTexture();
		void			ReleaseCharacterShadowTexture();
		void			ReleaseCharacterShadowTextureOriginal();
		void			SetShadowTextureSize(WORD size);

		static void		LoadDepthTerrainTexture(const char* textureName, IDirect3DTexture9** texture);
		// Loads shared (map-independent) effect textures and shaders a little per call, so the
		// character-select screen can do it in idle frames instead of the map load.
		// Returns true when everything is loaded.
		static bool		PreloadSharedGraphicsStep(int iGraphicsLevel, DWORD dwBudgetMs);
		void			SnapShadowSpaceMatrix(D3DXMATRIX& mat, float spaceSize);
		void			SkyUtil();
		void			UpdateShaderResidency();
		bool			BeginRenderCharacterShadowToTexture();
		bool			BeginRenderCharacterShadowToTexture2();
		bool			BeginRenderCharacterShadowToTexture3();
		bool			BeginRenderCharacterShadowToTexture4();
		void			EndRenderCharacterShadowToTexture();
		void			EndRenderCharacterShadowToTexture2();
		void			EndRenderCharacterShadowToTexture3();
		void			EndRenderCharacterShadowToTexture4();
		void			CreateCharacterShadowTextureOriginal();
		bool			BeginRenderCharacterShadowToTextureOriginal();
		void			EndRenderCharacterShadowToTextureOriginal();
		void			ReleaseGrass();
		void			RenderGrass();
		void			RenderGrassShadow();
		void			ReleaseReflectionTexture();
		IDirect3DTexture9* GetGrassTextureForInstance(const D3DXVECTOR3& grassPos);
		bool			recreate = false;

		IDirect3DVertexBuffer9*		 m_pGrassVertexBuffer = nullptr;
		IDirect3DIndexBuffer9*		 m_pGrassIndexBuffer = nullptr;
		IDirect3DVertexBuffer9*		 m_pGrassInstanceBuffer = nullptr;
		IDirect3DVertexBuffer9*		 m_pInstanceBuffer = nullptr;
		IDirect3DVertexDeclaration9* m_pVertexDeclaration = nullptr;
		IDirect3DVertexDeclaration9* m_pTerrainVertexDeclaration = nullptr;

		bool			IsWaterVisible();
		void			SetWaterReflectionRenderTarget();
		void			SetPuddleReflectionRenderTarget();
		void			RenderSky(bool isReflection = false);
		void			RenderAreaShadow();
		void			__RenderTerrain_RenderHardwareTransformPatch2();
		void			UpdateCharacterMovement();
		void			__HardwareTransformPatch_RenderPatchSplat2(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType);
		void			__RenderTerrain_RenderHardwareTransformPatchOriginal();
		void			__HardwareTransformPatch_RenderPatchSplatOriginal(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType);
		void			RenderWater();
		void			RenderMarkedArea();
		void			RecurseRenderAttr(CTerrainQuadtreeNode *Node, bool bCullEnable=TRUE);
		void			DrawPatchAttr(long patchnum);
		void			ClearGuildArea();
		void			RegisterGuildArea(int isx, int isy, int iex, int iey);

		void			VisibleMarkedArea();
		void			DisableMarkedArea();

		void			UpdateSky();
		void			RenderCollision();
		void			RenderCloud();
		void			RenderBeforeLensFlare();
		void			RenderAfterLensFlare();
		void			RenderScreenFiltering();

		void			SetWireframe(bool bWireFrame);
		bool			IsWireframe();


		bool			GetPickingPointWithRay(const CRay & rRay, D3DXVECTOR3 * v3IntersectPt);
		bool			GetPickingPointWithRayOnlyTerrain(const CRay & rRay, D3DXVECTOR3 * v3IntersectPt);
		bool			GetPickingPoint(D3DXVECTOR3 * v3IntersectPt);
		void			GetTerrainCount(short * psTerrainCountX, short * psTerrainCountY)
		{
			*psTerrainCountX = m_sTerrainCountX;
			*psTerrainCountY = m_sTerrainCountY;
		}

		bool			SetTerrainCount(short sTerrainCountX, short sTerrainCountY);

		// Shadow
		void			SetDrawShadow(bool bDrawShadow);
		void			SetDrawCharacterShadow(bool bDrawChrShadow);

		DWORD			GetShadowMapColor(float fx, float fy);

	protected:
		bool			__PickTerrainHeight(float& fPos, const D3DXVECTOR3& v3Start, const D3DXVECTOR3& v3End, float fStep, float fRayRange, float fLimitRange, D3DXVECTOR3* pv3Pick);

#if defined(ENABLE_CLIENT_PERFORMANCE) && !defined(WORLD_EDITOR)
		virtual bool LoadTerrain(WORD wTerrainCoordX, WORD wTerrainCoordY);
		virtual bool LoadArea(WORD wAreaCoordX, WORD wAreaCoordY);
#else
		virtual void	__ClearGarvage();
		virtual void	__UpdateGarvage();

		virtual bool	LoadTerrain(WORD wTerrainCoordX, WORD wTerrainCoordY, WORD wCellCoordX, WORD wCellCoordY);
		virtual bool	LoadArea(WORD wAreaCoordX, WORD wAreaCoordY, WORD wCellCoordX, WORD wCellCoordY);
		virtual void	UpdateAreaList(long lCenterX, long lCenterY);
#endif
		bool			isTerrainLoaded(WORD wX, WORD wY);
		bool			isAreaLoaded(WORD wX, WORD wY);

		void			AssignTerrainPtr();

		void			SaveAlphaFogOperation();
		void			RestoreAlphaFogOperation();

		//////////////////////////////////////////////////////////////////////////
		// New
		//////////////////////////////////////////////////////////////////////////
		void			GetHeightMap(const BYTE & c_rucTerrainNum, WORD ** pwHeightMap);
		void			GetNormalMap(const BYTE & c_rucTerrainNum, char ** pucNormalMap);

		// Water
		void			GetWaterMap(const BYTE & c_rucTerrainNum, BYTE ** pucWaterMap);
		void			GetWaterHeight(BYTE byTerrainNum, BYTE byWaterNum, long * plWaterHeight);


	//////////////////////////////////////////////////////////////////////////
	// Terrain
	//////////////////////////////////////////////////////////////////////////
	protected:
		CTerrain *					m_pTerrain[AROUND_AREA_NUM];	// Terrain
		CTerrainPatchProxy *		m_pTerrainPatchProxyList;

		long						m_lViewRadius;
		float						m_fHeightScale;

		short						m_sTerrainCountX, m_sTerrainCountY;

		TOutdoorMapCoordinate		m_CurCoordinate;

		long						m_lCurCoordStartX, m_lCurCoordStartY;
		TOutdoorMapCoordinate		m_PrevCoordinate;
		TOutdoorMapCoordinateMap	m_EntryPointMap;

		WORD						m_wPatchCount;

		//////////////////////////////////////////////////////////////////////////
		// Index Buffer
#ifdef	WORLD_EDITOR
		WORD *						m_pwIndices;		/* temp Index buffer */

		CGraphicIndexBuffer			m_IndexBuffer;
		WORD						m_wNumIndices;
#else
		WORD *						m_pwaIndices[TERRAINPATCH_LODMAX];

		CGraphicIndexBuffer			m_IndexBuffer[TERRAINPATCH_LODMAX];
		WORD						m_wNumIndices[TERRAINPATCH_LODMAX];
#endif
		virtual void	DestroyTerrain();

		void			CreateTerrainPatchProxyList();
		void			DestroyTerrainPatchProxyList();

		void			UpdateTerrain(float fX, float fY);

		void			ConvertTerrainToTnL(long lx, long ly);

		void			AssignPatch(long lPatchNum, long lx0, long ly0, long lx1, long ly1);

		//////////////////////////////////////////////////////////////////////////
		// Index Buffer
		void			ADDLvl1TL(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl1T(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount,  const BYTE & c_rucNumLineWarp);
		void			ADDLvl1TR(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl1L(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl1R(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl1BL(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl1B(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl1BR(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl1M(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl2TL(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl2T(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl2TR(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl2L(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl2R(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl2BL(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl2B(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl2BR(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);
		void			ADDLvl2M(WORD * pIndices, WORD & rwCount, const WORD & c_rwCurCount, const BYTE & c_rucNumLineWarp);

	public:
		BOOL			GetTerrainPointer(BYTE c_ucTerrainNum, CTerrain ** ppTerrain);
		float			GetTerrainHeight(float fx, float fy);
		bool			GetWaterHeight(int iX, int iY, long * plWaterHeight);
		bool			GetNormal(int ix, int iy, D3DXVECTOR3 * pv3Normal);

		void			RenderTerrain();

		const long		GetViewRadius()			{ return m_lViewRadius;		}
		const float		GetHeightScale()		{ return m_fHeightScale;	}

		const TOutdoorMapCoordinate & GetEntryPoint(const std::string & c_rstrEntryPointName) const;
		void SetEntryPoint(const std::string & c_rstrEntryPointName, const TOutdoorMapCoordinate & c_rOutdoorMapCoordinate);
		const TOutdoorMapCoordinate & GetCurCoordinate() { return m_CurCoordinate; }
		const TOutdoorMapCoordinate & GetPrevCoordinate() { return m_PrevCoordinate; }

	//////////////////////////////////////////////////////////////////////////
	// Area
	//////////////////////////////////////////////////////////////////////////
	protected:
		CArea *			m_pArea[AROUND_AREA_NUM];		// Data

		virtual void	DestroyArea();

		void			__UpdateArea(D3DXVECTOR3& v3Player);
#ifdef WORLD_EDITOR
		void			__NEW_WorldEditor_UpdateArea();
#endif
		void			__Game_UpdateArea(D3DXVECTOR3& v3Player);

		void			__BuildDynamicSphereInstanceVector();

		void			__CollectShadowReceiver(D3DXVECTOR3& v3Target, D3DXVECTOR3& v3Light);
		void			__CollectCollisionPCBlocker(D3DXVECTOR3& v3Eye, D3DXVECTOR3& v3Target, float fDistance);
		void			__CollectCollisionShadowReceiver(D3DXVECTOR3& v3Target, D3DXVECTOR3& v3Light);
#ifdef ENABLE_RENDERING_ONLY_IN_AREA_V2
		void			__UpdateAroundAreaList(D3DXVECTOR3& v3Player);
#else
		void			__UpdateAroundAreaList();
#endif
		bool			__IsInShadowReceiverList(CGraphicObjectInstance* pkObjInstTest);
		bool			__IsInPCBlockerList(CGraphicObjectInstance* pkObjInstTest);

		void			ConvertToMapCoords(float fx, float fy, int *iCellX, int *iCellY, BYTE * pucSubCellX, BYTE * pucSubCellY, WORD * pwTerrainNumX, WORD * pwTerrainNumY);

	public:
		BOOL			GetAreaPointer(const BYTE c_ucAreaNum, CArea ** ppArea);
		void			RenderArea(bool bRenderAmbience = true);
		void			RenderBlendArea();
		void			RenderDungeon();
		void			RenderEffect();
		void			RenderPCBlocker();
		void			RenderTree();

	public:
		//////////////////////////////////////////////////////////////////////////
		// For Grass
		//////////////////////////////////////////////////////////////////////////
		float		GetHeight(float* pPos);
		bool		GetBrushColor(float fX, float fY, float* pLowColor, float* pHighColor);
		bool		isAttrOn(float fX, float fY, BYTE byAttr);
#if defined(ANK_SYSTEM_GRAPHICS)
		bool		isPuddleOn(float fX, float fY, BYTE byAttr);
#endif
		bool		GetAttr(float fX, float fY, BYTE * pbyAttr);
		bool		isAttrOn(int iX, int iY, BYTE byAttr);
#if defined(ANK_SYSTEM_GRAPHICS)
		bool		isPuddleOn();
#endif
		bool		GetAttr(int iX, int iY, BYTE * pbyAttr);

		void		SetMaterialDiffuse(float fr, float fg, float fb);
		void		SetMaterialAmbient(float fr, float fg, float fb);
		void		SetTerrainMaterial(const PR_MATERIAL * pMaterial);

		bool		GetTerrainNum(float fx, float fy, BYTE * pbyTerrainNum);
		bool		GetTerrainNumFromCoord(WORD wCoordX, WORD wCoordY, BYTE * pbyTerrainNum);

	protected:
		//////////////////////////////////////////////////////////////////////////
		// New
		//////////////////////////////////////////////////////////////////////////
		long					m_lCenterX, m_lCenterY;
		long					m_lOldReadX, m_lOldReadY;	/* Last center */

		//////////////////////////////////////////////////////////////////////////
		// Octree
		//////////////////////////////////////////////////////////////////////////
		CTerrainQuadtreeNode * 	m_pRootNode;

		void					BuildQuadTree();
		CTerrainQuadtreeNode *	AllocQuadTreeNode(long x0, long y0, long x1, long y1);
		void					SubDivideNode(CTerrainQuadtreeNode * Node);
		void					UpdateQuadTreeHeights(CTerrainQuadtreeNode *Node);


		void					FreeQuadTree();

		struct TPatchDrawStruct
		{
			float fDistance;
			BYTE byTerrainNum;
			long lPatchNum;
			CTerrainPatchProxy * pTerrainPatchProxy;

			bool operator<( const TPatchDrawStruct & rhs) const
			{
				return fDistance < rhs.fDistance;
			}
		};

	public:
		typedef std::vector<BYTE> TTerrainNumVector;
		struct FSortPatchDrawStructWithTerrainNum
		{
			static TTerrainNumVector m_TerrainNumVector;
			FSortPatchDrawStructWithTerrainNum()
			{
				m_TerrainNumVector.clear();
			}

			bool operator () (const TPatchDrawStruct & lhs, const TPatchDrawStruct & rhs)
			{
				DWORD lhsTerrainNumOrder = 0, rhsTerrainNumOrder = 0;
				bool blhsOrderFound = false;
				bool brhsOrderFound = false;

				TTerrainNumVector::iterator lhsIterator = std::find(m_TerrainNumVector.begin(), m_TerrainNumVector.end(), lhs.byTerrainNum);
				TTerrainNumVector::iterator rhsIterator = std::find(m_TerrainNumVector.begin(), m_TerrainNumVector.end(), rhs.byTerrainNum);

				if (lhsIterator != m_TerrainNumVector.end())
				{
					blhsOrderFound = true;
					lhsTerrainNumOrder = lhsIterator - m_TerrainNumVector.begin();
				}
				if (rhsIterator != m_TerrainNumVector.end())
				{
					brhsOrderFound = true;
					rhsTerrainNumOrder = rhsIterator - m_TerrainNumVector.begin();
				}
				if (!brhsOrderFound)
				{
					m_TerrainNumVector.push_back(rhs.byTerrainNum);
					rhsTerrainNumOrder = m_TerrainNumVector.size() -1;
				}
				if (!blhsOrderFound)
				{
					lhsIterator = std::find(m_TerrainNumVector.begin(), m_TerrainNumVector.end(), lhs.byTerrainNum);
					if (lhsIterator != m_TerrainNumVector.end())
					{
						blhsOrderFound = true;
						lhsTerrainNumOrder = lhsIterator - m_TerrainNumVector.begin();
					}
					if (!blhsOrderFound)
					{
						m_TerrainNumVector.push_back(lhs.byTerrainNum);
						lhsTerrainNumOrder = m_TerrainNumVector.size() -1;
					}
				}

				return lhsTerrainNumOrder < rhsTerrainNumOrder;
			}
		};

	protected:

		std::vector<std::pair<float, long> > m_PatchVector;
		std::vector<TPatchDrawStruct> m_PatchDrawStructVector;

		void					SetPatchDrawVector();

		void					NEW_DrawWireFrame(CTerrainPatchProxy * pTerrainPatchProxy, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType);

		void					DrawWireFrame(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType);
		void					DrawWater(long patchnum);
#if defined(ANK_SYSTEM_GRAPHICS)
		void					__SetWaterShaderState(int iWaterLevel);
		void					__SetAtmosphereShaderConstants();
#endif

		bool					m_bDrawWireFrame;
		bool					m_bDrawShadow;
		bool					m_bDrawChrShadow;

		//////////////////////////////////////////////////////////////////////////
		// Water
		D3DXMATRIX				m_matBump;
		void					LoadWaterTexture();
		void					UnloadWaterTexture();
		//Water
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Alpha Fog
		CGraphicImageInstance	m_AlphaFogImageInstance;
		D3DXMATRIX				m_matAlphaFogTexture;
		// Alpha Fog
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
// Character Shadow
public:
		inline static LPDIRECT3DTEXTURE9 TextureReflectionMap = nullptr;
		inline static LPDIRECT3DTEXTURE9 m_lpCharacterShadowMapTextureOriginal = nullptr;
		inline static LPDIRECT3DTEXTURE9 m_lpCharacterShadowMapTexture = nullptr;
		inline static LPDIRECT3DTEXTURE9 m_lpCharacterShadowMapTexture2 = nullptr;
		inline static LPDIRECT3DTEXTURE9 m_lpCharacterShadowMapTexture3 = nullptr;
		inline static LPDIRECT3DTEXTURE9 m_lpCharacterShadowMapTexture4 = nullptr;
		inline static LPDIRECT3DTEXTURE9 m_lpCharacterShadowMapTexture5 = nullptr;
		inline static LPDIRECT3DSURFACE9 m_lpCharacterShadowMapRenderTargetSurfaceOriginal = nullptr;
		inline static LPDIRECT3DSURFACE9 m_lpCharacterShadowMapRenderTargetSurface = nullptr;
		inline static LPDIRECT3DSURFACE9 m_lpCharacterShadowMapRenderTargetSurface2 = nullptr;
		inline static LPDIRECT3DSURFACE9 m_lpCharacterShadowMapRenderTargetSurface3 = nullptr;
		inline static LPDIRECT3DSURFACE9 m_lpCharacterShadowMapRenderTargetSurface4 = nullptr;
		inline static LPDIRECT3DSURFACE9 m_lpCharacterShadowMapRenderTargetSurface5 = nullptr;
		inline static LPDIRECT3DSURFACE9 m_lpCharacterShadowMapDepthSurface = nullptr;
		inline static LPDIRECT3DSURFACE9 m_lpCharacterShadowMapDepthSurfaceOriginal = nullptr;
		inline static LPDIRECT3DSURFACE9 m_lpCharacterShadowMapDepthSurfaceTree = nullptr;
		D3DVIEWPORT9 m_ShadowMapViewport {};
		D3DVIEWPORT9 m_ShadowMapViewportTree {};
		inline static WORD m_wShadowMapSize = 0;
		LPDIRECT3DTEXTURE9 normalMap = nullptr;

		inline static LPDIRECT3DTEXTURE9 m_lpWaterReflectionTexture = nullptr;
		inline static LPDIRECT3DSURFACE9 m_lpWaterReflectionDepthSurface = nullptr;
		inline static LPDIRECT3DSURFACE9 m_lpPuddleReflectionDepthSurface = nullptr;
		int m_waterReflectionTexSize = 0;
		int m_waterPuddleReflectionTexSize = 0;
		inline static LPDIRECT3DQUERY9 m_waterOcclusionQuery = nullptr;

		//New Graphics - Static Cache for Instant Loading (0ms)
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderDepth = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderMain = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderTerrain = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderObjects = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderWater = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderMain2 = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderTerrain2 = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderObjects2 = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderWater2 = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderMain3 = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderTerrain3 = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderTerrain4 = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpVertexShaderTerrainBase = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderObjects3 = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderWater3 = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderDepth = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderMain = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpShadowMapVertexShaderTree = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderTerrain = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderObjects = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderWater = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderMain2 = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderTerrain2 = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderObjects2 = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderWater2 = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderMain3 = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderTerrain3 = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderTerrain4 = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderObjects3 = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderWater3 = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderDepthTree = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpShadowMapPixelShaderTree = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpPixelShaderTerrainBase = nullptr;

		inline static LPDIRECT3DPIXELSHADER9 m_lpPixelShaderGrass = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpVertexShaderGrass = nullptr;
		inline static LPDIRECT3DPIXELSHADER9 m_lpPixelShaderDungeon = nullptr;
		inline static LPDIRECT3DVERTEXSHADER9 m_lpVertexShaderDungeon = nullptr;

		D3DXMATRIX m_MatLightViewProj;
		D3DXMATRIX m_MatLightViewProj2;
		static LPDIRECT3DVERTEXSHADER9 LoadAndBuildVertexShader(const char* filename);
		static LPDIRECT3DPIXELSHADER9 LoadAndBuildPixelShader(const char* filename, bool HLSL = true);

		void			CreateShadowmapShaders();
		void			CreateWaterShaders();
		void			CreateTreeShaders();

		void			ReleaseShaders();
		void			ReleaseWaterShaders();
		void			ReleaseTreeShaders();

		bool			shadersReleased = false;
		bool			shadersWaterReleased = false;
		bool			shadersTreeReleased = false;

		//////////
		bool isPinkEnvironment = false;
		bool isNoapteEnvironment = false;
		bool isEnvironment6 = false;
		bool isEnvironment7 = false;
		bool isEnvironment9 = false;
		bool isEnvironment10 = false;
		bool isEnvironment11 = false;
		bool isEnvironment2 = false;
		bool isEnvironment3 = false;
		bool isEnvironment4 = false;
		bool isEnvironment5 = false;

		// Backup Device Context
		LPDIRECT3DSURFACE9 m_lpBackupRenderTargetSurface = nullptr;
		LPDIRECT3DSURFACE9 m_lpBackupDepthSurface = nullptr;
		D3DVIEWPORT9 m_BackupViewport {};

		// Character Shadow
		//////////////////////////////////////////////////////////////////////////

		// View Frustum Culling
		D3DXPLANE					m_plane[6];

		void BuildViewFrustum(D3DXMATRIX & mat);

		CTextureSet					m_TextureSet;
#if defined(ANK_SYSTEM_GRAPHICS)
		std::string					currentMapName;
		std::string					currentEnvironmentName;
#endif

	protected:
		CSkyBox						m_SkyBox;
		CLensFlare					m_LensFlare;
		CScreenFilter				m_ScreenFilter;

	protected:
		void SetIndexBuffer();
		void SelectIndexBuffer(BYTE byLODLevel, WORD * pwPrimitiveCount, D3DPRIMITIVETYPE * pePrimitiveType);

		D3DXMATRIX m_matWorldForCommonUse;
		D3DXMATRIX m_matViewInverse;

public:
		D3DXMATRIX m_matSplatAlpha;
		D3DXMATRIX m_matStaticShadow;
		D3DXMATRIX m_matDynamicShadow;
		D3DXMATRIX m_matDynamicShadowScale;
		D3DXMATRIX m_matLightView;
		D3DXMATRIX m_matLightView2;
		D3DXVECTOR3 current_v3SunPosition;
		D3DXVECTOR3 shadow_eye;

		float m_fTerrainTexCoordBase;
		float m_fWaterTexCoordBase;

		float m_fXforDistanceCaculation, m_fYforDistanceCaculation;

	protected:
		typedef std::vector<CTerrain *>		TTerrainPtrVector;
		typedef TTerrainPtrVector::iterator TTerrainPtrVectorIterator;
		typedef std::vector<CArea *>		TAreaPtrVector;
		typedef TAreaPtrVector::iterator	TAreaPtrVectorIterator;

		TTerrainPtrVector			m_TerrainVector;
#if !defined(ENABLE_CLIENT_PERFORMANCE) || defined(WORLD_EDITOR)
		TTerrainPtrVector			m_TerrainDeleteVector;
		TTerrainPtrVector			m_TerrainLoadRequestVector;
		TTerrainPtrVector			m_TerrainLoadWaitVector;
		TTerrainPtrVectorIterator	m_TerrainPtrVectorIterator;
#endif

		TAreaPtrVector				m_AreaVector;
#if !defined(ENABLE_CLIENT_PERFORMANCE) || defined(WORLD_EDITOR)
		TAreaPtrVector				m_AreaDeleteVector;
		TAreaPtrVector				m_AreaLoadRequestVector;
		TAreaPtrVector				m_AreaLoadWaitVector;
		TAreaPtrVectorIterator		m_AreaPtrVectorIterator;

		struct FPushToDeleteVector
		{
			enum EDeleteDir
			{
				DELETE_LEFT,
				DELETE_RIGHT,
				DELETE_TOP,
				DELETE_BOTTOM,
			};

			EDeleteDir m_eLRDeleteDir;
			EDeleteDir m_eTBDeleteDir;
			TOutdoorMapCoordinate m_CurCoordinate;

			FPushToDeleteVector(EDeleteDir eLRDeleteDir, EDeleteDir eTBDeleteDir, TOutdoorMapCoordinate CurCoord)
			{
				m_eLRDeleteDir = eLRDeleteDir;
				m_eTBDeleteDir = eTBDeleteDir;
				m_CurCoordinate = CurCoord;
			}
		};

		struct FPushTerrainToDeleteVector : public FPushToDeleteVector
		{
			TTerrainPtrVector	m_ReturnTerrainVector;

			FPushTerrainToDeleteVector(EDeleteDir eLRDeleteDir, EDeleteDir eTBDeleteDir, TOutdoorMapCoordinate CurCoord)
				: FPushToDeleteVector(eLRDeleteDir, eTBDeleteDir, CurCoord)
			{
				m_ReturnTerrainVector.clear();
			}

			void operator() (CTerrain * pTerrain);
		};

		struct FPushAreaToDeleteVector : public FPushToDeleteVector
		{
			TAreaPtrVector		m_ReturnAreaVector;

			FPushAreaToDeleteVector(EDeleteDir eLRDeleteDir, EDeleteDir eTBDeleteDir, TOutdoorMapCoordinate CurCoord)
				: FPushToDeleteVector(eLRDeleteDir, eTBDeleteDir, CurCoord)
			{
				m_ReturnAreaVector.clear();
			}

			void operator() (CArea * pArea);
		};
#endif

	protected:
		void InitializeVisibleParts();

		float __GetNoFogDistance();
		float __GetFogDistance();

	public:
		bool IsVisiblePart(int ePart);


	protected:
		DWORD m_dwVisiblePartFlags;

		int m_iRenderedSplatNumSqSum;
		int m_iRenderedSplatNum;
		int m_iRenderedPatchNum;
		std::vector<int> m_RenderedTextureNumVector;
		int m_iSplatLimit;

	protected:
		int m_iPatchTerrainVertexCount;
		int m_iPatchWaterVertexCount;

		int m_iPatchTerrainVertexSize;
		int m_iPatchWaterVertexSize;

		DWORD m_dwRenderedCRCNum;
		DWORD m_dwRenderedGraphicThingInstanceNum;

		std::list<RECT> m_rkList_kGuildArea;

	protected:
		void __RenderTerrain_RecurseRenderQuadTree(CTerrainQuadtreeNode *Node, bool bCullCheckNeed = true);
		int	 __RenderTerrain_RecurseRenderQuadTree_CheckBoundingCircle(const D3DXVECTOR3 & c_v3Center, const float & c_fRadius);

		void __RenderTerrain_AppendPatch(const D3DXVECTOR3& c_rv3Center, float fDistance, long lPatchNum);

		void __RenderTerrain_RenderSoftwareTransformPatch();
		void __RenderTerrain_RenderHardwareTransformPatch();

	protected:
		void __HardwareTransformPatch_RenderPatchSplat(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType);
		void __HardwareTransformPatch_RenderPatchNone(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType);

	protected:
		struct SoftwareTransformPatch_SData
		{
			enum
			{
				SPLAT_VB_NUM = 8,
				NONE_VB_NUM = 8,
			};

#ifdef ENABLE_DIRECTX9_UPDATE
            IDirect3DVertexBuffer9* m_pkVBSplat[SPLAT_VB_NUM];
            IDirect3DVertexBuffer9* m_pkVBNone[NONE_VB_NUM];
#else
            IDirect3DVertexBuffer8* m_pkVBSplat[SPLAT_VB_NUM];
            IDirect3DVertexBuffer8* m_pkVBNone[NONE_VB_NUM];
#endif

			DWORD m_dwSplatPos;
			DWORD m_dwNonePos;
			DWORD m_dwLightVersion;
		} m_kSTPD;

		struct SoftwareTransformPatch_SRenderState {
			D3DXMATRIX m_m4Proj;
			D3DXMATRIX m_m4Frustum;
			D3DXMATRIX m_m4DynamicShadow;

#ifdef ENABLE_DIRECTX9_UPDATE
            D3DLIGHT9       m_kLight;
            D3DMATERIAL9    m_kMtrl;
#else
            D3DLIGHT8       m_kLight;
            D3DMATERIAL8    m_kMtrl;
#endif

			D3DXVECTOR3 m_v3Player;
			DWORD m_dwFogColor;
			float m_fScreenHalfWidth;
			float m_fScreenHalfHeight;

			float m_fFogNearDistance;
			float m_fFogFarDistance;
			float m_fFogNearTransZ;
			float m_fFogFarTransZ;
			float m_fFogLenInv;
		};

		struct SoftwareTransformPatch_STVertex
		{
			D3DXVECTOR4 kPosition;
		};

		struct SoftwareTransformPatch_STLVertex
		{
			D3DXVECTOR4 kPosition;
			DWORD dwDiffuse;
			DWORD dwFog;
			D3DXVECTOR2 kTexTile;
			D3DXVECTOR2 kTexAlpha;
			D3DXVECTOR2 kTexStaticShadow;
			D3DXVECTOR2 kTexDynamicShadow;
		};

		void __SoftwareTransformPatch_ApplyRenderState();
		void __SoftwareTransformPatch_RestoreRenderState(DWORD dwFogEnable);

		void __SoftwareTransformPatch_Initialize();
		bool __SoftwareTransformPatch_Create();
		void __SoftwareTransformPatch_Destroy();
		void __SoftwareTransformPatch_BuildPipeline(SoftwareTransformPatch_SRenderState& rkTPRS);
		void __SoftwareTransformPatch_BuildPipeline_BuildFogFuncTable(SoftwareTransformPatch_SRenderState& rkTPRS);
		bool __SoftwareTransformPatch_SetTransform(SoftwareTransformPatch_SRenderState& rkTPRS, SoftwareTransformPatch_STLVertex* akTransVertex, CTerrainPatchProxy& rkTerrainPatchProxy, UINT uTerrainX, UINT uTerrainY, bool isFogEnable, bool isDynamicShadow);

		bool __SoftwareTransformPatch_SetSplatStream(SoftwareTransformPatch_STLVertex* akTransVertex);
		bool __SoftwareTransformPatch_SetShadowStream(SoftwareTransformPatch_STLVertex* akTransVertex);

		void __SoftwareTransformPatch_ApplyStaticShadowRenderState();
		void __SoftwareTransformPatch_RestoreStaticShadowRenderState();

		void __SoftwareTransformPatch_ApplyFogShadowRenderState();
		void __SoftwareTransformPatch_RestoreFogShadowRenderState();
		void __SoftwareTransformPatch_ApplyDynamicShadowRenderState();
		void __SoftwareTransformPatch_RestoreDynamicShadowRenderState();
		void __SoftwareTransformPatch_RenderPatchSplat(SoftwareTransformPatch_SRenderState& rkTPRS, long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType, bool isFogEnable);
		void __SoftwareTransformPatch_RenderPatchNone(SoftwareTransformPatch_SRenderState& rkTPRS, long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType);

	protected:
		std::vector<CGraphicObjectInstance *> m_ShadowReceiverVector;
		std::vector<CGraphicObjectInstance *> m_PCBlockerVector;

	protected:
		float	m_fOpaqueWaterDepth;
		CGraphicImageInstance m_WaterInstances[30];

	public:
		float	GetOpaqueWaterDepth() { return m_fOpaqueWaterDepth;	}
		void	SetOpaqueWaterDepth(float fOpaqueWaterDepth) { m_fOpaqueWaterDepth = fOpaqueWaterDepth; }
		void	SetTerrainRenderSort(ETerrainRenderSort eTerrainRenderSort) { m_eTerrainRenderSort = eTerrainRenderSort;}
		ETerrainRenderSort	GetTerrainRenderSort() { return m_eTerrainRenderSort; }

	protected:
		ETerrainRenderSort m_eTerrainRenderSort;

	protected:
		CGraphicImageInstance	m_attrImageInstance;
		CGraphicImageInstance	m_BuildingTransparentImageInstance;
		D3DXMATRIX				m_matBuildingTransparent;

	protected:
		CDynamicPool<CMonsterAreaInfo>		m_kPool_kMonsterAreaInfo;
		TMonsterAreaInfoPtrVector			m_MonsterAreaInfoPtrVector;
		TMonsterAreaInfoPtrVectorIterator	m_MonsterAreaInfoPtrVectorIterator;

	public:
		bool LoadMonsterAreaInfo();

		CMonsterAreaInfo * AddMonsterAreaInfo(long lOriginX, long lOriginY, long lSizeX, long lSizeY);
		void RemoveAllMonsterAreaInfo();

		DWORD GetMonsterAreaInfoCount() { return m_MonsterAreaInfoPtrVector.size();	}
		bool GetMonsterAreaInfoFromVectorIndex(DWORD dwMonsterAreaInfoVectorIndex, CMonsterAreaInfo ** ppMonsterAreaInfo);

		CMonsterAreaInfo * AddNewMonsterAreaInfo(long lOriginX, long lOriginY, long lSizeX, long lSizeY,
			CMonsterAreaInfo::EMonsterAreaInfoType eMonsterAreaInfoType,
			DWORD dwVID, DWORD dwCount, CMonsterAreaInfo::EMonsterDir eMonsterDir);

	public:
		void GetBaseXY(DWORD * pdwBaseX, DWORD * pdwBaseY);
		void SetBaseXY(DWORD dwBaseX, DWORD dwBaseY);

		void SetTransparentTree(bool bTransparentTree) { m_bTransparentTree = bTransparentTree;}
		void EnableTerrainOnlyForHeight(bool bFlag) { m_bEnableTerrainOnlyForHeight = bFlag; }
		void EnablePortal(bool bFlag);
		bool IsEnablePortal() { return m_bEnablePortal; }

	protected:
		DWORD			m_dwBaseX;
		DWORD			m_dwBaseY;

		D3DXVECTOR3		m_v3Player;

		bool			m_bShowEntirePatchTextureCount;
		bool			m_bTransparentTree;
		bool			m_bEnableTerrainOnlyForHeight;
		bool			m_bEnablePortal;

	// XMas
	private:
		struct SXMasTree
		{
			SpeedTreeWrapperPtr m_pkTree;
			int m_iEffectID;
		} m_kXMas;

		void __XMasTree_Initialize();
		void __XMasTree_Create(float x, float y, float z, const char* c_szTreeName, const char* c_szEffName);

	public:
		void XMasTree_Destroy();
		void XMasTree_Set(float x, float y, float z, const char* c_szTreeName, const char* c_szEffName);

	// Special Effect
	private:
		typedef std::map<DWORD, int> TSpecialEffectMap;
		TSpecialEffectMap m_kMap_dwID_iEffectID;

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
	private:
		int m_iLastShopPos;

	public:
		void SpecialShopPosEffect_Create(DWORD dwID, float x, float y, float z, const char *c_szEffName);
		void SpecialShopPosEffect_Delete();
#endif

	public:
		void SpecialEffect_Create(DWORD dwID, float x, float y, float z, const char* c_szEffName);
		void SpecialEffect_Delete(DWORD dwID);
		void SpecialEffect_Destroy();

	private:
		struct SHeightCache
		{
			struct SItem
			{
				DWORD	m_dwKey;
				float	m_fHeight;
			};

			enum
			{
				HASH_SIZE = 100,
			};

			std::vector<SItem> m_akVct_kItem[HASH_SIZE];

			bool m_isUpdated;
		} m_kHeightCache;

		void __HeightCache_Init();
		void __HeightCache_Update();

	public:
		void SetEnvironmentDataName(const std::string& strEnvironmentDataName);
		std::string& GetEnvironmentDataName();

	protected:
		std::string		m_settings_envDataName;
		std::string		m_envDataName;

	private:
		bool m_bSettingTerrainVisible;
};
