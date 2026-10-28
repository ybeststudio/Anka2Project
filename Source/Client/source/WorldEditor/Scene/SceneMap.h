#pragma once

// Test Code
#include "SceneMapCursor.h"

#include "../GameLib/TerrainDecal.h"
#include "../GameLib/SnowEnvironment.h"

class CMapManagerAccessor;
class CMonsterAreaInfo;

class CSceneMap : public CSceneBase
{
	public:
		enum
		{
			EDITING_MODE_NONE,
			EDITING_MODE_TERRAIN,
			EDITING_MODE_OBJECT,
			EDITING_MODE_ENVIRONMENT,
			EDITING_MODE_ATTRIBUTE,
		};

		enum
		{
			BRUSH_TYPE_CIRCLE,
			BRUSH_TYPE_SQUARE,
		};

	public:
		CSceneMap();
		virtual ~CSceneMap();

		void Initialize();
		void CreateEnvironment();
		void SetMapManagerAccessor (CMapManagerAccessor * pMapManagerAccessor);

		void SetEditingMode (int iMode);

		void RefreshArea();

		/////////////////////////////////////////////////////////////////
		// Set Cursor
		CCursorRenderer* GetCursorRenererPointer();
		void SetObjectBrushType (int iType);
		void SetObjectBrushScale (int iScale);
		void SetObjectBrushDensity (int iDensity);
		void SetObjectBrushRandom (int iRandom);
		void SetCheckingNearObject (bool bFlag);
		void SetRandomRotation (bool bFlag);

		void SetCursorYaw (float fYaw);
		void SetCursorPitch (float fPitch);
		void SetCursorRoll (float fRoll);
		void SetCursorScale (DWORD dwScale);

		void SetGridMode (BYTE byGridMode);
		void SetGridDistance (float fDistance);
		void SetObjectHeight (float fHeight);

		void RefreshCursor();
		void ClearCursor();
		void ChangeCursor (DWORD dwCRC);

		float GetCursorYaw();
		float GetCursorPitch();
		float GetCursorRoll();
		float GetCursorObjectHeight();
		DWORD GetCursorScale();
		/////////////////////////////////////////////////////////////////

		void UpdateSelecting();
		void RenderSelectedObject();

		void SetPatchGrid (bool bOn)
		{
			m_bPatchGridOn = bOn;
		}
		bool GetPatchGrid()
		{
			return m_bPatchGridOn;
		}
		void SetMapBoundGrid (bool bOn)
		{
			m_bMapBoundGridOn = bOn;
		}
		bool GetMapBoundGrid()
		{
			return m_bMapBoundGridOn;
		}

		void SetCompass (bool bOn)
		{
			m_bCompassOn = bOn;
		}
		void SetMeterGrid (bool bOn)
		{
			m_bMeterGridOn = bOn;
		}

		void SetCharacterRendering (bool bOn)
		{
			m_bCharacterRenderingOn = bOn;
		}
		void SetWaterRendering (bool bOn)
		{
			m_bWaterRenderingOn = bOn;
		}
		void SetObjectRendering (bool bOn)
		{
			m_bObjectRenderingOn = bOn;
		}
		void SetObjectCollisionRendering (bool bOn)
		{
			m_bObjectCollisionRenderingOn = bOn;
		}
		void SetTerrainRendering (bool bOn)
		{
			m_bTerrainRenderingOn = bOn;
		}
		void SetObjectShadowRendering (bool bOn);
		void SetGuildAreaRendering (bool bOn);

		D3DXVECTOR3 GetMouseMapIntersect();
		void OnMovePosition (float fx, float fy);

		void LightPositionEditingStart()
		{
			m_bLightPositionEditingInProgress = true;
		}
		void LightPositionEditingEnd()
		{
			m_bLightPositionEditingInProgress = false;
		}
		void SetLightPositionEditingOn()
		{
			m_bLightPositionEditingOn = true;
		}
		void SetLightPositionEditingOff()
		{
			m_bLightPositionEditingOn = false;
		}

	protected:
		void OnUpdate();
		void OnRender (BOOL bClear);
		void OnRenderUI (float fx, float fy);
		void OnRenderLightDirection();
		void OnRenderEnvironmentMap();

		void OnRenderSceneAttribute();
		void OnRenderMonsterAreaInfo (CMonsterAreaInfo * pMonsterAreaInfo);

		void OnSetCamera();

		void OnKeyDown (int iChar);
		void OnKeyUp (int iChar);
		void OnMouseMove (long x, long y);
		void OnLButtonDown (UINT nFlags, CPoint point);
		void OnLButtonUp();
		void OnRButtonDown();
		void OnRButtonUp();
		BOOL OnMouseWheel (short zDelta);

		void OnRenderCenterCursor();
		void OnRenderCompass();
		void OnRenderTerrainEditingArea();
		void OnRenderSelectedObject();
		void OnRenderObjectSettingArea();

		void OnRenderPatchGrid();
		void OnRenderMeterGrid();
		void OnRenderMapBoundGrid();
		void OnRenderCharacter();
		void OnRenderObjectCollision();

		void OnLightMove (const long& c_rlx, const long& c_rly);

		void SaveMiniMapWithMonsterAreaInfo();

		void __ClearCursor();

	protected:
		CMapManagerAccessor* 					m_pMapManagerAccessor;
		CMapManagerAccessor::CHeightObserver* 	m_pHeightObserver;
		CMapOutdoorAccessor* 		m_pMapAccessor;
		D3DXCOLOR					m_ClearColor;

		int							m_iEditingMode;
		D3DXVECTOR3					m_vecMouseMapIntersectPosition;
		D3DXCOLOR					m_EditCenterColor;
		D3DXCOLOR					m_EditablePointColor;
		D3DXCOLOR					m_PickingPointColor;

		// About Object Cursor
		int							m_iInsertedObjectIndex;
		BOOL						m_bObjectIsMoved;
		DWORD						m_dwCursorObjectCRC;
		CGraphicTextInstance		m_TextInstance[3];

		bool						m_isCheckingNearObject;
		bool						m_isRandomRotation;

		bool						m_bCursorYawPitchChange;
		POINT						m_poCursorMouse;
		float						m_fBaseYaw;
		float						m_fBasePitch;
		CCursorRenderer				m_CursorRenderer;

		bool						m_bCompassOn;
		bool						m_bMeterGridOn;
		bool						m_bPatchGridOn;
		bool						m_bMapBoundGridOn;
		bool						m_bCharacterRenderingOn;
		bool						m_bWaterRenderingOn;
		bool						m_bObjectRenderingOn;
		bool						m_bObjectCollisionRenderingOn;
		bool						m_bTerrainRenderingOn;
		bool						m_bShadowRenderingOn;
		bool						m_bGuildAreaRenderingOn;
		LPDIRECT3DVERTEXBUFFER9		m_pBigSquareVB;

		bool						m_bLightPositionEditingInProgress;
		bool						m_bLightPositionEditingOn;

		CPoint						m_ptClick;
		long						m_loldX;
		long						m_loldY;

		CTerrainDecal 				m_aConpasTerrainDecal;
		CGraphicImageInstance		m_pCompasGraphicImageInstance;

		// UI
		CGraphicTextInstance		m_textInstanceSplatTextureCount;
		CGraphicTextInstance		m_textInstanceSplatMeshCount;
		CGraphicTextInstance		m_textInstanceSplatMeshPercentage;
		CGraphicTextInstance		m_textInstancePatchSplatTileCount;
		CGraphicTextInstance		m_textInstanceTexture0Count;

		// Shadow
		bool						m_bTerrainShadowMapUpdateNeeded;

		// Monster
		CGraphicTextInstance		m_textInstanceMonsterInfo;

		// Snow
		CSnowEnvironment			m_kSnowEnvironment;

		// Visible
		TOutdoorMapCoordinate		m_kPrevCoordinate;
};
