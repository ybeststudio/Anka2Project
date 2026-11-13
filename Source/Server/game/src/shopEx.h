#ifndef __INC_METIN_II_GAME_SHOP_SECONDARY_COIN_H__
#define __INC_METIN_II_GAME_SHOP_SECONDARY_COIN_H__
#include "typedef.h"
#include "shop.h"

struct SShopTable;
typedef struct SShopTableEx : SShopTable
{
	std::string name;
	EShopCoinType coinType;
#ifdef ENABLE_SHOP_TABS_BAERIM
	BYTE width;									// grid columns (5, Baerim weapon shop 10)
	TShopItemTable itemsEx[SHOP_EX_ITEM_MAX_NUM];	// the tab's slots (items[] of SShopTable holds only 40)
#endif
} TShopTableEx;

class CGroupNode;

class CShopEx: public CShop
{
public:
	bool			Create(DWORD dwVnum, DWORD dwNPCVnum);
	bool			AddShopTable(TShopTableEx& shopTable);

	virtual bool	AddGuest(LPCHARACTER ch,DWORD owner_vid, bool bOtherEmpire);
	virtual void	SetPCShop(LPCHARACTER ch) { return; }
	virtual bool	IsPCShop() { return false; }
#ifdef ENABLE_SHOP_TABS_BAERIM
#ifdef ENABLE_GOLD_LIMIT
	virtual long long	Buy(LPCHARACTER ch, WORD pos);
#else
	virtual int		Buy(LPCHARACTER ch, WORD pos);
#endif
#else
#ifdef ENABLE_GOLD_LIMIT
	virtual long long	Buy(LPCHARACTER ch, BYTE pos);
#else
	virtual int		Buy(LPCHARACTER ch, BYTE pos);
#endif
#endif
	virtual bool	IsSellingItem(DWORD itemID) { return false; }

	size_t			GetTabCount() { return m_vec_shopTabs.size(); }
private:
	std::vector <TShopTableEx> m_vec_shopTabs;
};
typedef CShopEx* LPSHOPEX;

#endif
