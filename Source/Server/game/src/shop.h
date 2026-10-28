#ifndef __INC_METIN_II_GAME_SHOP_H__
#define __INC_METIN_II_GAME_SHOP_H__

enum
{
	SHOP_MAX_DISTANCE = 1000
};

class CGrid;

class CShop
{
	public:
		typedef struct shop_item
		{
			DWORD vnum;
#ifdef ENABLE_GOLD_LIMIT
			long long price;
#else
			long price;
#endif

#ifdef ENABLE_STACK_LIMIT
			WORD count;
#else
			BYTE count;
#endif

			LPITEM pkItem;
			int itemid;

			shop_item()
			{
				vnum = 0;
				price = 0;
				count = 0;
				itemid = 0;
				pkItem = NULL;
			}
		} SHOP_ITEM;

		CShop();
		virtual ~CShop();

		bool	Create(DWORD dwVnum, DWORD dwNPCVnum, TShopItemTable * pItemTable);

#ifdef ENABLE_STACK_LIMIT
		void	SetShopItems(TShopItemTable* pItemTable, WORD bItemCount);
#else
		void	SetShopItems(TShopItemTable* pItemTable, BYTE bItemCount);
#endif

		virtual void	SetPCShop(LPCHARACTER ch);
		virtual bool	IsPCShop()	{ return m_pkPC ? true : false; }

		virtual bool	AddGuest(LPCHARACTER ch,DWORD owner_vid, bool bOtherEmpire);
		void	RemoveGuest(LPCHARACTER ch);
		void	RemoveAllGuests();

#ifdef ENABLE_GOLD_LIMIT
		virtual long long	Buy(LPCHARACTER ch, BYTE pos);
#else
		virtual int	Buy(LPCHARACTER ch, BYTE pos);
#endif

		void	BroadcastUpdateItem(BYTE pos);

		int		GetNumberByVnum(DWORD dwVnum);

		virtual bool	IsSellingItem(DWORD itemID);

		DWORD	GetVnum() { return m_dwVnum; }
		DWORD	GetNPCVnum() { return m_dwNPCVnum; }

	protected:
		void	Broadcast(const void * data, int bytes);

	protected:
		DWORD m_dwVnum;
		DWORD m_dwNPCVnum;

		CGrid* m_pGrid;

		typedef std::unordered_map<LPCHARACTER, bool> GuestMapType;
		GuestMapType m_map_guest;
		std::vector<SHOP_ITEM> m_itemVector;

		LPCHARACTER m_pkPC;
};

#endif
