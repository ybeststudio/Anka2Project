#pragma once

#ifdef ENABLE_RENEWAL_OFFLINESHOP
class COfflineShop
{
	public:
		TOfflineShop m_data;
		COfflineShop();
		virtual ~COfflineShop();

		virtual bool AddGuest(LPCHARACTER ch, LPCHARACTER npc);
		void RemoveGuest(LPCHARACTER ch, bool isDestroy = false);

		DWORD GetItemCount();

		void DestroyEx();
		void Destroy();
		void BroadcastUpdateItem(BYTE bPos, bool bDestroy = false, int log_index = -1);

		DWORD GetOwner(){return m_data.owner_id;}
		void SetShopSign(const char* f){strcpy(m_data.sign,f);}
		const char* GetShopSign() {return m_data.sign;}

		virtual void SetOfflineShopNPC(LPCHARACTER npc) { m_pkOfflineShopNPC = npc; }
		LPCHARACTER GetOfflineShopNPC() { return m_pkOfflineShopNPC; }

		void Broadcast(const void * data, int bytes);

	private:
		std::map<DWORD, LPCHARACTER> m_map_guest;
		LPCHARACTER m_pkOfflineShopNPC;
		DWORD m_dwDisplayedCount;
		DWORD m_dwRealWatcherCount;
};
#endif
