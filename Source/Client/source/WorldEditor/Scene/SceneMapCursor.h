#pragma once

#include "../DataCtrl/MapManagerAccessor.h"

class CCursorRenderer : public CScreen
{
	public:
		enum
		{
			BRUSH_TYPE_CIRCLE,
			BRUSH_TYPE_SQUARE,
		};

		enum
		{
			CURSOR_TYPE_NONE,
			CURSOR_TYPE_TREE,
			CURSOR_TYPE_BUILDING,
			CURSOR_TYPE_EFFECT,
			CURSOR_TYPE_AMBIENCE,
			CURSOR_TYPE_DUNGEON_BLOCK,
		};

		typedef struct SCursorPosition
		{
			int ix;
			int iy;
		} TCursorPosition;

	public:
		CCursorRenderer();
		~CCursorRenderer();

		void ClearCursor();
		void SetHeightObserver (CMapManagerAccessor::CHeightObserver * pHeightObserver);

		void SetCenterPosition (D3DXVECTOR3 & rv3Position);
		void SetCursorPosition (D3DXVECTOR3 & rv3Position);
		DWORD GetCurrentCRC();
		void SetCursor (DWORD dwCRC);
		DWORD GetCursorRotation();
		void SetCursorRotation (DWORD dwRotation);

		void SetCursorYaw (float fYaw);
		void SetCursorPitch (float fPitch);
		void SetCursorRoll (float fRoll);
		void SetCursorScale (DWORD dwScale);

		float GetCursorYaw();
		float GetCursorPitch();
		float GetCursorRoll();
		float GetObjectHeight();
		DWORD GetCursorScale();

		const D3DXVECTOR3& GetCursorPosition();
		DWORD GetSettingCursorCount();
		bool GetSettingCursorPosition (DWORD dwIndex, int* px, int* py);

		// Normal
		void SetObjectBrushType (int iType);
		void SetObjectAreaSize (int iSize);
		void SetObjectAreaDensity (int iDensity);
		void SetObjectAreaRandom (int iRandom);
		void SetGridMode (BOOL byGridMode);
		void SetGridDistance (float fDistance);
		void SetObjectHeight (float fHeight);

		// Tree
		void SetCursorTree (CProperty * pProperty);

		// Building
		void SetCursorBuilding (CProperty * pProperty);

		// Effect
		void SetCursorEffect (CProperty * pProperty);

		// Ambience
		void SetCursorAmbience (CProperty * pProperty);

		// Dungeon
		void SetCursorDungeonBlock (CProperty * pProperty);

		// Mouse
		void GetSelectArea (float* pfxStart, float* pfyStart, float* pfxEnd, float* pfyEnd);
		bool IsSelecting();
		void SelectStart();
		void SelectEnd();
		bool IsPicking();
		void PickStart();
		void PickEnd();
		const D3DXVECTOR3& GetPickedPosition();
		void UpdatePickedPosition();
		// Mouse

		void Update();
		void Render();
		void RenderCursorArea();
		void RefreshCursor();

		void RenderCursorSquare (float fxStart, float fyStart, float fxEnd, float fyEnd, int iStep = 50);
		void RenderCursorCircle (float fx, float fy, float fz, float fRadius, int iStep = 50);

	protected:
		void __Initialize();

		void RenderPieceLine (float fxStart, float fyStart, float fxEnd, float fyEnd, int iStep);

	protected:
		int								m_iCursorType;

		float							m_fYaw;
		float							m_fPitch;
		float							m_fRoll;
		CMapManagerAccessor::CHeightObserver* 		m_pHeightObserver;
		D3DXVECTOR3						m_v3CenterPosition;
		D3DXVECTOR3						m_v3CursorPosition;
		DWORD							m_dwCurrentCRC;
		std::vector<TCursorPosition>	m_CursorPositionVector;

		bool							m_isSelecting;
		D3DXVECTOR3						m_v3SelectStartPosition;
		bool							m_isPicking;
		D3DXVECTOR3						m_v3PickStartPosition;

		int								m_iObjectBrushType;
		int								m_iObjectAreaHalfSize;
		int								m_iObjectAreaDensity;
		int								m_iObjectAreaRandom;
		BOOL							m_isGridMode;
		float							m_fGridDistance;
		float							m_fObjectHeight;

		CSpeedTreeWrapper* 				m_pTreeCursor;

		// Building
		CGraphicThingInstance			m_BuildingInstance;

		// Effect
		DWORD							m_dwEffectInstanceIndex;

		// Ambience
		DWORD							m_dwAmbienceRange;

		// Dungeon Block
		CDungeonBlock					m_DungeonBlock;
};
