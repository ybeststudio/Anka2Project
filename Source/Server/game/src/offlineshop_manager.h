#pragma once
#ifdef ENABLE_RENEWAL_OFFLINESHOP

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
	#include <unordered_map>
#endif

class COfflineShopManager : public singleton<COfflineShopManager>
{
	public:
		COfflineShopManager();
		~COfflineShopManager();
		void Initialize();

		LPOFFLINESHOP FindOfflineShopPID(DWORD pid);

		void CompareOffShopEventTime();

		void OpenMyOfflineShop(LPCHARACTER ch, const char* c_pszSign, TOfflineShopItemTable* pTable, BYTE bItemCount, DWORD shopVnum, BYTE titleType);
		void CreateOfflineShop(TOfflineShop* offlineshop);
		void StopShopping(LPCHARACTER ch);
		void OpenOfflineShop(LPCHARACTER ch);
		void OpenOfflineShopWithVID(LPCHARACTER ch, DWORD vid);
		bool HasOfflineShop(LPCHARACTER ch);

		void Buy(LPCHARACTER ch, DWORD vid, BYTE bPos);
		void BuyItemReal(TOfflineShopBuy* item);

		void AddItem(LPCHARACTER ch, BYTE bDisplayPos, TItemPos bPos, long long iPrice);
		void AddItemReal(OFFLINE_SHOP_ITEM* item);

		void OpenSlot(LPCHARACTER ch, BYTE bPos);
		void OpenSlotReal(TOfflineShopOpenSlot* ch);

		void RemoveItem(LPCHARACTER ch, BYTE bPos);
		void RemoveItemReal(OFFLINE_SHOP_ITEM* item);

		void ShopLogRemove(LPCHARACTER ch);
		void ShopLogRemoveReal(DWORD ch);

		void ChangeDecoration(LPCHARACTER ch, TShopDecoration* data);
		void ChangeDecorationReal(TShopDecoration* ch);

		void WithdrawMoney(LPCHARACTER ch);
		void WithdrawMoneyReal(DWORD ch);

		void DestroyOfflineShop(LPCHARACTER ch);
		void DestroyOfflineShopReal(DWORD ch);

		void ChangeTitle(LPCHARACTER ch, const char* title);
		void ChangeTitleReal(TOfflineShopChangeTitle* p);

		void CloseOfflineShopForTime(LPOFFLINESHOP offlineshop);
		void CloseOfflineShopForTimeReal(DWORD offlineshop);

		void GetBackItem(LPCHARACTER ch);
		void GetBackItemReal(TOfflineShopBackItem* ch);

		void ShopAddTime(LPCHARACTER ch);
		void ShopAddTimeReal(DWORD ch);

		void RecvPackets(const char * data);

		DWORD FindShopWithName(const char* playerName);

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
		void LoadSearchLanguage();
		bool CheckItemNames(LPCHARACTER ch, const char* searchInput, std::vector<DWORD>& m_vecItemVnums);

		void ClearItem(DWORD id);
		void InsertItem(OFFLINE_SHOP_ITEM* p);
		void SearchItem(LPCHARACTER ch, const char * data);
		void RemoveInMemory(OFFLINE_SHOP_ITEM* item);
		void SaveInMemory(OFFLINE_SHOP_ITEM* item);
		bool getMap(TPacketCGShopSearch* pinfo, std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*>::iterator& it, std::unordered_map<DWORD, OFFLINE_SHOP_ITEM*>::iterator& itend);
#endif

		std::vector<DWORD> m_Map_pkShopTimes;

	private:
		LPEVENT m_pShopTimeEvent;

		std::vector<DWORD> m_Map_pkOfflineShopCache;
		std::map<DWORD, COfflineShop*> m_Map_pkOfflineShopByNPC;
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
		std::unordered_map<DWORD,OFFLINE_SHOP_ITEM*> m_itemMap;
		std::unordered_map<DWORD,OFFLINE_SHOP_ITEM*> m_itemMapWeapon;
		std::unordered_map<DWORD,OFFLINE_SHOP_ITEM*> m_itemMapEquipment;
		std::unordered_map<DWORD,OFFLINE_SHOP_ITEM*> m_itemMapCostume;
		std::unordered_map<DWORD,OFFLINE_SHOP_ITEM*> m_itemMapBook;
		std::unordered_map<DWORD,OFFLINE_SHOP_ITEM*> m_itemMapPets;
		std::unordered_map<DWORD,OFFLINE_SHOP_ITEM*> m_itemMapOther;
		std::map<BYTE, std::vector<std::pair<std::string, DWORD>>> m_mapItemNames;
#endif
};
#endif
