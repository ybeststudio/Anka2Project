#pragma once

#include "StdAfx.h"
#include "PythonWindow.h"
#include "../UserInterface/Locale_inc.h"

namespace UI
{
	enum
	{
		ITEM_WIDTH = 32,
		ITEM_HEIGHT = 32,

		SLOT_ACTIVE_EFFECT_COUNT = 3,
		SLOT_NUMBER_NONE = 0xffffffff,
	};

	enum ESlotStyle
	{
		SLOT_STYLE_NONE,
		SLOT_STYLE_PICK_UP,
		SLOT_STYLE_SELECT,
	};

	enum ESlotColorType
	{
		COLOR_TYPE_ORANGE,
		COLOR_TYPE_WHITE,
		COLOR_TYPE_RED,
		COLOR_TYPE_GREEN,
		COLOR_TYPE_YELLOW,
		COLOR_TYPE_BLUE,
		COLOR_TYPE_SKY,
		COLOR_TYPE_PINK,
	};

	enum ESlotHilight
	{
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		HILIGHTSLOT_ACCE,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		HILIGHTSLOT_AURA,
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		HILIGHTSLOT_CHANGE_LOOK,
#endif
		HILIGHTSLOT_CUBE,

		HILIGHTSLOT_MAX
	};

	enum ESlotState
	{
		SLOT_STATE_LOCK = (1 << 0),
		SLOT_STATE_CANT_USE = (1 << 1),
		SLOT_STATE_DISABLE = (1 << 2),
		SLOT_STATE_ALWAYS_RENDER_COVER = (1 << 3),
#ifdef ENABLE_SLOT_MARKING_SYSTEM
		SLOT_STATE_CANT_MOUSE_EVENT = (1 << 4),
		SLOT_STATE_UNUSABLE = (1 << 5),
#endif
	};

	class CSlotWindow : public CWindow
	{
		public:
			static DWORD Type();

		public:
			class CSlotButton;
			class CCoverButton;
			class CCoolTimeFinishEffect;

			friend class CSlotButton;
			friend class CCoverButton;

			typedef struct SSlot
			{
				DWORD	dwState;
				DWORD	dwSlotNumber;
				DWORD	dwCenterSlotNumber;
				DWORD   dwRealSlotNumber;
				DWORD	dwRealCenterSlotNumber;
				DWORD	dwItemIndex;
				BOOL	isItem;

				// CoolTime
				float	fCoolTime;
				float	fStartCoolTime;
				bool	bInverseCooltime;

				// Toggle
				BOOL	bActive;

				int		ixPosition;
				int		iyPosition;

				int		ixCellSize;
				int		iyCellSize;

				BYTE	byxPlacedItemSize;
				BYTE	byyPlacedItemSize;

				CGraphicImageInstance* pInstance;

				CNumberLine* pNumberLine;

				bool bRenderBaseSlotImage;

				CCoverButton* pCoverButton;
				CSlotButton* pSlotButton;
				CImageBox* pSignImage;
				CAniImageBox* pFinishCoolTimeEffect;

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
				std::shared_ptr< CImageBox > pCoverImage;
#endif

				D3DXCOLOR d3Color;
				D3DXCOLOR d3CoolTimeColor;
				bool bHasCoolTimeColor;
				std::shared_ptr< CImageBox > pSecondCoverImage;
				int iYinAndYangConnection;
			} TSlot;

			typedef std::list<TSlot> TSlotList;
			typedef TSlotList::iterator TSlotListIterator;
			typedef struct SStoreCoolDown { float fCoolTime; float fElapsedTime; bool bActive; };

		public:
			CSlotWindow(PyObject *ppyObject);
			virtual ~CSlotWindow();

			void Destroy();

			// Manage Slot
			void SetSlotType(DWORD dwType);
			void SetSlotStyle(DWORD dwStyle);
			void SetSlotScale(float fx, float fy);

			void AppendSlot(DWORD dwIndex, int ixPosition, int iyPosition, int ixCellSize, int iyCellSize);
			void SetCoverButton(DWORD dwIndex, const char *c_szUpImageName, const char *c_szOverImageName, const char *c_szDownImageName, const char *c_szDisableImageName, BOOL bLeftButtonEnable, BOOL bRightButtonEnable);
			void SetSlotBaseImage(const char *c_szFileName, float fr, float fg, float fb, float fa);
			void AppendSlotButton(const char *c_szUpImageName, const char *c_szOverImageName, const char *c_szDownImageName);
			void AppendRequirementSignImage(const char *c_szImageName);

			void EnableCoverButton(DWORD dwIndex);
			void DisableCoverButton(DWORD dwIndex);
#ifdef ENABLE_FISH_EVENT_SYSTEM
			void DeleteCoverButton(DWORD dwIndex);
#endif
			void SetAlwaysRenderCoverButton(DWORD dwIndex, bool bAlwaysRender = false);

			void ShowSlotBaseImage(DWORD dwIndex);
			void HideSlotBaseImage(DWORD dwIndex);
			BOOL IsDisableCoverButton(DWORD dwIndex);
			BOOL HasSlot(DWORD dwIndex);

			void ClearAllSlot();
			void ClearSlot(DWORD dwIndex);
			void SetSlot(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, CGraphicImage * pImage, D3DXCOLOR& diffuseColor);
#ifdef ENABLE_MINI_GAME
			void SetCardSlot(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, const char* c_szFileName, D3DXCOLOR& diffuseColor);
#endif
			void SetSlotCount(DWORD dwIndex, DWORD dwCount);
			void SetSlotCountNew(DWORD dwIndex, DWORD dwGrade, DWORD dwCount);
			void SetRealSlotNumber(DWORD dwIndex, DWORD dwID);
			void SetSlotCoolTime(DWORD dwIndex, float fCoolTime, float fElapsedTime = 0.0f);
			void SetSlotCoolTimeInverse(DWORD dwIndex, float fCoolTime, float fRemainingTime = 0.0f);
			void StoreSlotCoolTime(DWORD dwKey, DWORD dwSlotIndex, float fCoolTime, float fElapsedTime = .0f);
			void RestoreSlotCoolTime(DWORD dwKey);
			void ActivateSlot(DWORD dwIndex, int iColorType);
			void DeactivateSlot(DWORD dwIndex);
			void RefreshSlot();

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			void SetSlotCoverImage(const DWORD dwIndex, const char* FileName);
			void EnableSlotCoverImage(const DWORD dwIndex, const bool bEnable);
#endif

			BOOL IsActiveSlot(DWORD dwIndex);
			DWORD GetSlotItemVnum(DWORD dwIndex);
			DWORD GetSlotItemCount(DWORD dwIndex);
			BOOL GetSlotLocalPosition(DWORD dwIndex, int * px, int * py);
			BOOL GetSlotGlobalPosition(DWORD dwIndex, int * px, int * py);
			void ResetSlotCoolTime(DWORD dwIndex);
			void SetSlotCoolTimeColor(DWORD dwIndex, float fr, float fg, float fb, float fa);
			void SetSlotDiffuseColor(DWORD dwIndex, int iColorType);
			void SetSlotImage(DWORD dwIndex, const char * c_szFileName);
			void SetSecondSlotCoverImage(const DWORD dwIndex, const char * c_szFileName);
			void EnableSecondSlotCoverImage(const DWORD dwIndex, const bool bEnable);
			void SetSlotConnectedYinAndYang(DWORD dwIndex, int iType);
			void ClearSlotYinAndYangConnection(DWORD dwIndex);

			DWORD GetSlotCount();

			void GetSlotNumber(std::vector<DWORD>* _v);

			void LockSlot(DWORD dwIndex);
			void UnlockSlot(DWORD dwIndex);
			BOOL IsLockSlot(DWORD dwIndex);
			void SetCantUseSlot(DWORD dwIndex);
			void SetUseSlot(DWORD dwIndex);
			BOOL IsCantUseSlot(DWORD dwIndex);
			void EnableSlot(DWORD dwIndex);
			void DisableSlot(DWORD dwIndex);
			BOOL IsEnableSlot(DWORD dwIndex);

#ifdef ENABLE_SLOT_MARKING_SYSTEM
			void SetCanMouseEventSlot(DWORD dwIndex);
			void SetCantMouseEventSlot(DWORD dwIndex);
			void SetUsableSlotOnTopWnd(DWORD dwIndex);
			void SetUnusableSlotOnTopWnd(DWORD dwIndex);
#endif

			// Select
			void ClearSelected();
			void SelectSlot(DWORD dwSelectingIndex);
			BOOL isSelectedSlot(DWORD dwIndex);
			DWORD GetSelectedSlotCount();
			DWORD GetSelectedSlotNumber(DWORD dwIndex);

			// Slot Button
			void ShowSlotButton(DWORD dwSlotNumber);
			void HideSlotButton(DWORD dwSlotNumber);
			void HideAllSlotButton();
			void OnPressedSlotButton(DWORD dwType, DWORD dwSlotNumber, BOOL isLeft = TRUE);

			// Requirement Sign
			void ShowRequirementSign(DWORD dwSlotNumber);
			void HideRequirementSign(DWORD dwSlotNumber);

			// ToolTip
			BOOL OnOverInItem(DWORD dwSlotNumber);
			void OnOverOutItem();
			BOOL IsSlotOver(DWORD dwIndex) const { return m_dwToolTipSlotNumber == dwIndex; }

			// For Usable Item
			void SetUseMode(BOOL bFlag);
			void SetUsableItem(BOOL bFlag);
			void SetAlwaysUsableSlotMode(bool bFlag) { m_bAlwaysUsableSlotMode = bFlag; }

			// Scale
			void SetCoverButtonScale(DWORD dwIndex, float fx, float fy);
			void SetBaseImageScale(float fx, float fy);

			// CallBack
			void ReserveDestroyCoolTimeFinishEffect(DWORD dwSlotIndex);

		protected:
			void __Initialize();
			void __CreateToggleSlotImage();
			void __CreateSlotEnableEffect(int index);
			void __CreateFinishCoolTimeEffect(TSlot * pSlot);
			void __CreateBaseImage(const char *c_szFileName, float fr, float fg, float fb, float fa);

			void __DestroyToggleSlotImage();
			void __DestroySlotEnableEffect(int index);
			void __DestroyFinishCoolTimeEffect(TSlot * pSlot);
			void __DestroyBaseImage();

			// Event
			void OnUpdate();
			void OnRender();
			BOOL OnMouseLeftButtonDown();
			BOOL OnMouseLeftButtonUp();
			BOOL OnMouseRightButtonDown();
			BOOL OnMouseLeftButtonDoubleClick();
			void OnMouseOverOut();
			void OnMouseOver();
			void RenderSlotBaseImage();
			void RenderLockedSlot();
			virtual void OnRenderPickingSlot();
			virtual void OnRenderSelectedSlot();

			// Select
			void OnSelectEmptySlot(int iSlotNumber);
			void OnSelectItemSlot(int iSlotNumber);
			void OnUnselectEmptySlot(int iSlotNumber);
			void OnUnselectItemSlot(int iSlotNumber);
			void OnUseSlot();

			// Manage Slot
			BOOL GetSlotPointer(DWORD dwIndex, TSlot ** ppSlot);
			BOOL GetSelectedSlotPointer(TSlot ** ppSlot);
			virtual BOOL GetPickedSlotPointer(TSlot ** ppSlot);
			void ClearSlot(TSlot * pSlot);
			virtual void OnRefreshSlot();

			// ETC
			BOOL OnIsType(DWORD dwType);

		protected:
			DWORD m_dwSlotType;
			DWORD m_dwSlotStyle;
			std::list<DWORD> m_dwSelectedSlotIndexList;
			TSlotList m_SlotList;
			DWORD m_dwToolTipSlotNumber;
			std::map<DWORD, std::map<DWORD, SStoreCoolDown>> m_CoolDownStore;

			BOOL m_isUseMode;
			BOOL m_isUsableItem;
			bool m_bAlwaysUsableSlotMode;

			CGraphicImageInstance * m_pBaseImageInstance;
			CImageBox * m_pToggleSlotImage;
			CAniImageBox * m_pSlotActiveEffect[SLOT_ACTIVE_EFFECT_COUNT];

			float m_fScaleX;
			float m_fScaleY;

			std::deque<DWORD> m_ReserveDestroyEffectDeque;
	};
};