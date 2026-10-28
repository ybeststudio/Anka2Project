#pragma once

#include "../../GameLib/AreaTerrain.h"

class CTerrainAccessor : public CTerrain
{
	public:
		enum EBrushShape
		{
			BRUSH_SHAPE_NONE	= 0,
			BRUSH_SHAPE_CIRCLE	= 1 << 0,
			BRUSH_SHAPE_SQUARE	= 1 << 1,
			BRUSH_SHAPE_MAX		= 2,
		};

		enum EBrushType
		{
			BRUSH_TYPE_NONE			= 0,
			BRUSH_TYPE_UP			= 1 << 0,
			BRUSH_TYPE_DOWN			= 1 << 1,
			BRUSH_TYPE_PLATEAU		= 1 << 2,
			BRUSH_TYPE_NOISE		= 1 << 3,
			BRUSH_TYPE_SMOOTH		= 1 << 4,
			BRUSH_TYPE_MAX			= 5,
		};

	public:
		CTerrainAccessor();
		virtual ~CTerrainAccessor();

		virtual void	Clear();

		void DrawHeightBrush (DWORD dwBrushShape,
							  DWORD dwBrushType,
							  long lCellX,
							  long lCellY,
							  BYTE byBrushSize,
							  BYTE byBrushStrength);

		void DrawTextureBrush (DWORD dwBrushShape,
							   const std::vector<BYTE>& rVectorTextureNum,	// 텍스춰셋 인덱스들의 벡터 (랜덤 찍기를 위해 벡터로 전달 한다)
							   long lCellX,
							   long lCellY,
							   BYTE bySubCellX,
							   BYTE bySubCellY,
							   BYTE byBrushSize,
							   bool bErase,
							   bool bDrawOnlyOnBlankTile);

		void DrawAttrBrush (DWORD dwBrushShape,
							BYTE byAttrPowerNum,
							long lCellX,
							long lCellY,
							BYTE bySubCellX,
							BYTE bySubCellY,
							BYTE byBrushSize,
							bool bErase);

		void DrawWaterBrush (DWORD dwBrushShape,
							 long lCellX,
							 long lCellY,
							 BYTE byBrushSize,
							 WORD wWaterHeight,
							 bool bErase);

		void RAW_ResetTextures (const BYTE * pbyTileMap = NULL);
		void RAW_RestoreMaps (const WORD * pHeightMap, const BYTE * pbyTileMap, const char* pNormalMap);
		bool SaveProperty (const std::string & c_rstrMapName);

		void TerrainPutHeightmap (long x, long y, WORD val, bool bRecursive = true);

		// NewMap
		bool NewHeightMap (const std::string & c_rstrMapName);
		bool NewTileMap (const std::string & c_rstrMapName);
		bool NewAttrMap (const std::string & c_rstrMapName);

		// SaveMap
		bool SaveHeightMap (const std::string & c_rstrMapName);
		bool RAW_SaveTileMap (const std::string & c_rstrMapName);
		bool SaveAttrMap (const std::string & c_rstrMapName);
		bool SaveWaterMap (const std::string & c_rstrMapName);
		bool SaveShadowFromD3DTexture9 (const std::string & c_rstrMapName, LPDIRECT3DTEXTURE9 lpShadowTexture);
		bool SaveMiniMapFromD3DTexture9 (const std::string & c_rstrMapName, LPDIRECT3DTEXTURE9 lpShadowTexture);
		bool ReloadShadowTexture (const std::string & c_rstrMapName);

		//////////////////////////////////////////////////////////////////////////
		// 유틸리티
		bool RAW_LoadAndSaveTileMap (const char* tilename, const std::string & c_rstrMapName, const std::vector<BYTE>& c_rVectorBaseTexture);

		//////////////////////////////////////////////////////////////////////////
		// Attr
		void RAW_AllocateAttrSplats();
		void RAW_DeallocateAttrSplats();
		void RAW_GenerateAttrSplat();
		void RAW_UpdateAttrSplat();
		void RAW_ResetAttrSplat();

		TTerrainSplatPatch& RAW_GetAttrSplatPatch()
		{
			return m_RAWAttrSplatPatch;
		}
		void RAW_NotifyAttrModified();
		bool RAW_LoadAndSaveDefaultAttrMap (const std::string & c_rstrMapName);

	protected:
		// 		virtual void	RAW_CountTiles();

	protected:
		// HeightMap 편집 함수..
		void UpTerrain (DWORD dwBrushShape, long x, long y, BYTE byBrushSize, BYTE byBrushStrength);
		void DownTerrain (DWORD dwBrushShape, long x, long y, BYTE byBrushSize, BYTE byBrushStrength);
		void FlatTerrain (DWORD dwBrushShape, long x, long y, BYTE byBrushSize, BYTE byBrushStrength);
		void NoiseTerrain (DWORD dwBrushShape, long x, long y, BYTE byBrushSize, BYTE byBrushStrength);
		void SmoothTerrain (DWORD dwBrushShape, long x, long y, BYTE byBrushSize, BYTE byBrushStrength);

		// AttrMap 갱신
		//		void UpdateAttrMapFromHeightMap(long lx, long ly);

		// WaterMap 갱신
		void RecalculateWaterMap();

		void RecalculateTile (long lX, long lY, BYTE byNewTileNum);

	protected:
		TTerrainSplatPatch	m_RAWAttrSplatPatch;
		LPDIRECT3DTEXTURE9	m_lpAttrTexture;

		BOOL m_isDestroied;
};
