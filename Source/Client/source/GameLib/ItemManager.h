#pragma once

#include "StdAfx.h"
#include "ItemData.h"

class CItemManager : public CSingleton<CItemManager>
{
	public:
		enum EItemDescCol
		{
			ITEMDESC_COL_VNUM,
			ITEMDESC_COL_NAME,
			ITEMDESC_COL_DESC,
			ITEMDESC_COL_SUMM,
			ITEMDESC_COL_NUM,
		};

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		enum EItemScaleCol
		{
			ITEMSCALE_COL_VNUM,
			ITEMSCALE_COL_JOB,
			ITEMSCALE_COL_SEX,
			ITEMSCALE_COL_SCALE_X,
			ITEMSCALE_COL_SCALE_Y,
			ITEMSCALE_COL_SCALE_Z,
			ITEMSCALE_COL_PARTICLE_SCALE,
		};
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
		enum EAuraScaleCol
		{
			AURA_SCALE_COL_VNUM,
			AURA_SCALE_COL_JOB,
			AURA_SCALE_COL_SEX,
			AURA_SCALE_COL_MESH_SCALE_X,
			AURA_SCALE_COL_MESH_SCALE_Y,
			AURA_SCALE_COL_MESH_SCALE_Z,
			AURA_SCALE_COL_PARTICLE_SCALE,
			AURA_SCALE_COL_NUM,
		};
#endif

	public:
		typedef std::map<DWORD, CItemData*> TItemMap;
		typedef std::map<std::string, CItemData*> TItemNameMap;

	public:
		CItemManager();
		virtual ~CItemManager();

		void Destroy();

		BOOL SelectItemData(DWORD dwIndex);
		CItemData *GetSelectedItemDataPointer();

		BOOL GetItemDataPointer(DWORD dwItemID, CItemData ** ppItemData);

#if defined(ENABLE_WIKI_SYSTEM) || defined(INSIDE_RENDER) || defined(ENABLE_RENEWAL_OFFLINESHOP)
		TItemMap GetItems() const { return m_ItemMap; }
#endif

		bool LoadItemDesc(const char *c_szFileName);
		bool LoadItemList(const char *c_szFileName);
		bool LoadItemTable(const char *c_szFileName);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool LoadItemScale(const char *c_szFileName);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		bool LoadAuraScale(const char *c_szFileName);
#endif
		CItemData *MakeItemData(DWORD dwIndex);

	protected:
		TItemMap m_ItemMap;
		std::vector<CItemData*> m_vec_ItemRange;
		CItemData *m_pSelectedItemData;
};
