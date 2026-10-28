#pragma once

#include "../../GameLib/MapOutdoor.h"

class CTerrainAccessor;
class CMapArrangeHeightProgress;

class CMapOutdoorAccessor : public CMapOutdoor
{
	public:
		///////////////////////////////////////////////////////////////////////////////////////////
		// Undo
		class CObjectUndoData : public CUndoBuffer::IUndoData
		{
			public:
				CObjectUndoData (CMapOutdoorAccessor * pOutdoorAccessor);
				virtual ~CObjectUndoData();

				void Backup();
				void Restore();

				void BackupStatement();
				void RestoreStatement();

			protected:
				CAreaAccessor::TObjectDataVector	m_backupObjectDataVector[AROUND_AREA_NUM];
				CAreaAccessor* 						m_pbackupArea[AROUND_AREA_NUM];

				short								m_sCenterCoordX, m_sCenterCoordY;
				float								m_fCameraX, m_fCameraY;

			private:
				CMapOutdoorAccessor* 				m_pOwner;
		};

		class CTerrainUndoData : public CUndoBuffer::IUndoData
		{
			public:
				class CTerrainUndoDataSet
				{
					public:
						CTerrainUndoDataSet();
						virtual ~CTerrainUndoDataSet();

						void DeleteMaps();

						short						m_sCenterCoordX, m_sCenterCoordY;
						float						m_fCameraX, m_fCameraY;
						short						m_sCoordX, m_sCoordY;

						WORD* 						m_pRawHeightMap;
						BYTE* 						m_pbyTileMap;
						char* 						m_pNormalMap;
				};

				typedef std::vector<CTerrainUndoDataSet*> TTerrainUndoDataSetPtrVector;
				typedef TTerrainUndoDataSetPtrVector::iterator TTerrainUndoDataSetPtrVectorIterator;

			public:
				CTerrainUndoData (CMapOutdoorAccessor * pOwner);
				virtual ~CTerrainUndoData();

				void Clear();

				void AddTerrainUndoDataSets();

				void Backup();
				void Restore();

				void BackupStatement();
				void RestoreStatement();

			protected:
				void AddTerrainUndoDataSet (BYTE byTerrainNum);

				float						m_fCameraX, m_fCameraY;
				short						m_sCoordX, m_sCoordY;

			private:
				TTerrainUndoDataSetPtrVector m_TerrainUndoDataSetPtrVector;
				CMapOutdoorAccessor* m_pOwner;
		};
		// Undo
		///////////////////////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// For Attr
		enum
		{
			RENDER_SHADOW,
			RENDER_ATTR,
		};

		//////////////////////////////////////////////////////////////////////////
		// Portal
		typedef std::vector<int> TPortalNumberVector;

	public:
		CMapOutdoorAccessor();
		virtual ~CMapOutdoorAccessor();

		// Tool에서 COutdoorMap, CArea, CTerrain을 활용하기 위한 Virtual function
		virtual void OnPreAssignTerrainPtr();
		virtual bool Load (float x, float y, float z);

		// 세이부~
		bool SaveProperty (const std::string & c_rstrFolder);
		bool SaveSetting (const std::string & c_rstrFolder);

		bool SaveTerrains();
		bool SaveAreas();

		void DrawHeightBrush (DWORD c_rdwBrushShape,
							  DWORD c_rdwBrushType,
							  WORD wTerrainNumX,
							  WORD wTerrainNumY,
							  long lCellX,
							  long lCellY,
							  BYTE byBrushSize,
							  BYTE c_rucBrushStrength);

		void DrawTextureBrush (DWORD dwBrushShape,
							   std::vector<BYTE>& rVectorTextureNum,
							   WORD wTerrainNumX,
							   WORD wTerrainNumY,
							   long lCellX,
							   long lCellY,
							   BYTE bySubCellX,
							   BYTE bySubCellY,
							   BYTE byBrushSize,
							   bool bErase = false,
							   bool bDrawOnlyOnBlankTile = false);

		void DrawWaterBrush (DWORD c_rdwBrushShape,
							 WORD wTerrainNumX,
							 WORD wTerrainNumY,
							 long lCellX,
							 long lCellY,
							 BYTE byBrushSize,
							 WORD wWaterHeight,
							 bool bErase = false);

		void PreviewWaterBrush (DWORD c_rdwBrushShape,
								WORD wTerrainNumX,
								WORD wTerrainNumY,
								long lCellX,
								long lCellY,
								BYTE byBrushSize,
								WORD wWaterHeight,
								bool bErase = false);

		void DrawAttrBrush (DWORD dwBrushShape,
							BYTE byAttrFlag,
							WORD wTerrainNumX,
							WORD wTerrainNumY,
							long lCellX,
							long lCellY,
							BYTE bySubCellX,
							BYTE bySubCellY,
							BYTE byBrushSize,
							bool bErase = false);

		void ResetTextures();
		void ResetAttrSplats();	// Attr Brush로 그린 다음에 업데이트를 담당

		int GetMapID();
		void SetMapID (int iID);
		void SetTerrainModified();

		bool CreateNewTerrainFiles (WORD wCoordX, WORD wCoordY);

		//////////////////////////////////////////////////////////////////////////
		void SetMaxBrushStrength (BYTE byMaxBrushStrength)
		{
			m_byMAXBrushStrength = byMaxBrushStrength;
		}
		BYTE GetMaxBrushStrength()
		{
			return m_byMAXBrushStrength;
		}

		BOOL GetAreaAccessor (DWORD dwIndex, CAreaAccessor** ppAreaAccessor);

		bool GetPickingCoordinate (D3DXVECTOR3 * v3IntersectPt, int* piCellX, int* piCellY, BYTE * pbySubCellX, BYTE * pbySubCellY, WORD * pwTerrainNumX, WORD * pwTerrainNumY);
		bool GetPickingCoordinateWithRay (const CRay & rRay, D3DXVECTOR3 * v3IntersectPt, int* piCellX, int* piCellY, BYTE * pbySubCellX, BYTE * pbySubCellY, WORD * pwTerrainNumX,
										  WORD * pwTerrainNumY);
		////////////////////////////////////////////
		// For Undo System
		void BackupObject();
		void BackupObjectCurrent();
		void BackupTerrain();
		void BackupTerrainCurrent();
		////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Utility
		bool RAW_InitBaseTexture (const std::vector<BYTE>& c_rVectorBaseTexture);
		void ReloadBuildingTexture();

		//////////////////////////////////////////////////////////////////////////
		// ATTR
		void RenderAccessorTerrain (BYTE byRenderMode, BYTE byAttrFlag);
		bool ResetToDefaultAttr();
		void RenderObjectCollision();

		// ShadowMap
		void RenderToShadowMap();
		void RenderShadow();

		// MiniMap
		void RenderMiniMap();

		// Reload
		void ReloadTerrainTextures();

		// System Value
		float GetSplatValue();
		void SetSplatValue (float fValue);

		// ETC
		void ArrangeTerrainHeight();

		// Guild Area List
		bool LoadGuildAreaList (const char* c_szFileName);

		// Selected Object
		void SetSelectedObjectName (const char* szName);
		void ClearSelectedPortalNumber();
		void AddSelectedObjectPortalNumber (int iNum);
		const TPortalNumberVector& GetSelectedObjectPortalVectorRef();
		const char* GetSelectedObjectName();

	protected:
		virtual void	__ClearGarvage();
		virtual void	__UpdateGarvage();

		virtual bool	LoadTerrain (WORD wTerrainCoordX, WORD wTerrainCoordY, WORD wCellCoordX, WORD wCellCoordY);
		virtual bool	LoadArea (WORD wAreaCoordX, WORD wAreaCoordY, WORD wCellCoordX, WORD wCellCoordY);
		virtual void	UpdateAreaList (long lCenterX, long lCenterY);
		virtual bool	Destroy();

		static void		main_ArrangeTerrainHeight (void* pv);
		void			__CreateProgressDialog();
		void			__HideProgressDialog();
		void			__DestroyProgressDialog();
		void			__ACCESS_ConvertToMapCoords (float fx, float fy, int* iCellX, int* iCellY, BYTE * pucSubCellX, BYTE * pucSubCellY, WORD * pwTerrainNumX, WORD * pwTerrainNumY);

	private:
		void RecurseRenderAccessorTerrain (CTerrainQuadtreeNode *Node, BYTE byRenderMode, BYTE byAttrFlag, bool bCullEnable = true);
		void DrawPatchAttr (long patchnum, BYTE byAttrFlag);

		//////////////////////////////////////////////////////////////////////////
		// For ShadowRender
		void DrawMeshOnly (long patchnum);
		//////////////////////////////////////////////////////////////////////////

		void ResetTerrainPatchVertexBuffer();

		unsigned char			m_byMAXBrushStrength;
		bool					m_bLoadTextureAttr;

		float					m_fSplatValue;

		int						m_iMapID;

		std::string				m_strSelectedObjectName;
		TPortalNumberVector		m_kVec_iPortalNumber;

		static CMapOutdoorAccessor* 			ms_pThis;
		static CMapArrangeHeightProgress* 		ms_pkProgressDialog;

	public:
		void SetMonsterNames();
		void RemoveMonsterAreaInfoPtr (CMonsterAreaInfo * pMonsterAreaInfo);
		bool SaveMonsterAreaInfo();

		BOOL m_bNowAccessGarvage;
};
