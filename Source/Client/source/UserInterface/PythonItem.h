#pragma once

#include "StdAfx.h"

#include "../EterGrnLib/ThingInstance.h"

class CItemData;

class CPythonItem : public CSingleton<CPythonItem>
{
	public:
		enum
		{
			INVALID_ID = 0xffffffff,
		};

		enum
		{
			VNUM_MONEY = 1,
		};

		enum
		{
			USESOUND_NONE,
			USESOUND_DEFAULT,
			USESOUND_ARMOR,
			USESOUND_WEAPON,
			USESOUND_BOW,
			USESOUND_ACCESSORY,
			USESOUND_POTION,
			USESOUND_PORTAL,
			USESOUND_NUM,
		};

		enum
		{
			DROPSOUND_DEFAULT,
			DROPSOUND_ARMOR,
			DROPSOUND_WEAPON,
			DROPSOUND_BOW,
			DROPSOUND_ACCESSORY,
			DROPSOUND_NUM
		};

#ifdef ENABLE_FALLING_EFFECT_SYSTEM
        enum EDropItemEffects
        {
            DROP_ITEM_EFFECT_NORMAL,
            DROP_ITEM_EFFECT_RARE,
            DROP_ITEM_EFFECT_EPIC,
            DROP_ITEM_EFFECT_LEGENARY,
            DROP_ITEM_EFFECT_NUM,
        };
#endif

		typedef struct SGroundItemInstance
		{
			DWORD dwVirtualNumber;
			D3DXVECTOR3 v3EndPosition;

			D3DXVECTOR3 v3RotationAxis;
			D3DXQUATERNION qEnd;
			D3DXVECTOR3 v3Center;
			CGraphicThingInstance ThingInstance;
			DWORD dwStartTime;
			DWORD dwEndTime;

			DWORD eDropSoundType;

			bool bAnimEnded;
			bool Update();
			void Clear();

			DWORD dwEffectInstanceIndex;
			std::string stOwnership;

			static void __PlayDropSound(DWORD eItemType, const D3DXVECTOR3& c_rv3Pos);
			static std::string ms_astDropSoundFileName[DROPSOUND_NUM];

			SGroundItemInstance() {}
			virtual ~SGroundItemInstance() {}
		} TGroundItemInstance;

		typedef std::map<DWORD, TGroundItemInstance *>	TGroundItemInstanceMap;

	public:
		CPythonItem(void);
		virtual ~CPythonItem(void);

		// Initialize
		void	Destroy();
		void	Create();

		void	PlayUseSound(DWORD dwItemID);
		void	PlayDropSound(DWORD dwItemID);
		void	PlayUsePotionSound();

		void	SetUseSoundFileName(DWORD eItemType, const std::string& c_rstFileName);
		void	SetDropSoundFileName(DWORD eItemType, const std::string& c_rstFileName);

		void	GetInfo(std::string* pstInfo);

		void	DeleteAllItems();

		void	Render();
		void	Update(const POINT& c_rkPtMouse);

#ifdef ENABLE_RENEWAL_BOOK_NAME
		void	CreateItem(DWORD dwVirtualID, DWORD dwVirtualNumber, float x, float y, float z, long lSocket0 = 0, bool bDrop = true);
#else
		void	CreateItem(DWORD dwVirtualID, DWORD dwVirtualNumber, float x, float y, float z, bool bDrop = true);
#endif
		void	DeleteItem(DWORD dwVirtualID);
		void	SetOwnership(DWORD dwVID, const char *c_pszName);
		bool	GetOwnership(DWORD dwVID, const char ** c_pszName);

		BOOL	GetGroundItemPosition(DWORD dwVirtualID, TPixelPosition * pPosition);

		bool	GetPickedItemID(DWORD* pdwPickedItemID);

		bool	GetCloseItem(const TPixelPosition & c_rPixelPosition, DWORD* pdwItemID, DWORD dwDistance=300);
		bool	GetCloseMoney(const TPixelPosition & c_rPixelPosition, DWORD* dwItemID, DWORD dwDistance=300);

#if defined(ENABLE_INSTANT_PICKUP) || defined(ENABLE_AUTOMATIC_PICK_UP_SYSTEM)
		bool	GetCloseItemVector(const std::string& myName, const TPixelPosition& c_rPixelPosition, std::vector<DWORD>& itemVidList);
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		bool	CanAddChangeLookItem(const CItemData* item, const CItemData* other_item) const;
		bool	CanAddChangeLookFreeItem(const DWORD dwVnum) const;
		bool	IsChangeLookClearScrollItem(const DWORD dwVnum) const;
#endif

		DWORD	GetVirtualNumberOfGroundItem(DWORD dwVID);

		void	BuildNoGradeNameData(int iType);
		DWORD	GetNoGradeNameDataCount();
		CItemData * GetNoGradeNameDataPtr(DWORD dwIndex);

	protected:
#ifdef ENABLE_FALLING_EFFECT_SYSTEM
        void __RegisterEffect(int iIndex, const char* szFile);
        int __AttachEffect(DWORD dwVnum, BYTE byType, BYTE bySubType);
#endif
		DWORD __Pick(const POINT& c_rkPtMouse);
		DWORD __GetUseSoundType(const CItemData& c_rkItemData);
		DWORD __GetDropSoundType(const CItemData& c_rkItemData);

	protected:
		TGroundItemInstanceMap m_GroundItemInstanceMap;
		CDynamicPool<TGroundItemInstance> m_GroundItemInstancePool;

#ifdef ENABLE_FALLING_EFFECT_SYSTEM
		DWORD m_dwDropItemEffectID[DROP_ITEM_EFFECT_NUM];
#else
		DWORD m_dwDropItemEffectID;
#endif
		DWORD m_dwPickedItemID;

		int m_nMouseX;
		int m_nMouseY;

		std::string m_astUseSoundFileName[USESOUND_NUM];

		std::vector<CItemData *> m_NoGradeNameItemData;

#ifdef ENABLE_CLIENT_PERFORMANCE
	public:
		void SendPythonData(PyObject *obj, const char *funcname);
#endif
};