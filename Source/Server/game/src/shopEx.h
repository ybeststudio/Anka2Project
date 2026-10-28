#ifndef __INC_METIN_II_GAME_SHOP_SECONDARY_COIN_H__
#define __INC_METIN_II_GAME_SHOP_SECONDARY_COIN_H__
#include "typedef.h"
#include "shop.h"

struct SShopTable;
typedef struct SShopTableEx : SShopTable
{
	std::string name;
	EShopCoinType coinType;
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
#ifdef ENABLE_GOLD_LIMIT
	virtual long long	Buy(LPCHARACTER ch, BYTE pos);
#else
	virtual int		Buy(LPCHARACTER ch, BYTE pos);
#endif
	virtual bool	IsSellingItem(DWORD itemID) { return false; }

	size_t			GetTabCount() { return m_vec_shopTabs.size(); }
private:
	std::vector <TShopTableEx> m_vec_shopTabs;
};
typedef CShopEx* LPSHOPEX;

#endif
