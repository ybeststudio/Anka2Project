#pragma once

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
#include "Packet.h"

class CPythonPrivateShopSearch : public CSingleton<CPythonPrivateShopSearch>
{
	public:
		CPythonPrivateShopSearch();
		virtual ~CPythonPrivateShopSearch();

		void AddItemData(TOfflineShopItemData& rItemData);
		void ClearItemData();
		DWORD GetItemDataCount() { return m_ItemInstanceVector.size(); }
		DWORD GetItemDataPtr(DWORD index, TOfflineShopItemData** ppInstance);

	protected:
		std::vector<TOfflineShopItemData> m_ItemInstanceVector;
};
#endif
