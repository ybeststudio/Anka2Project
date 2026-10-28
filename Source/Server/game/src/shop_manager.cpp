#include "stdafx.h"
#include "../../common/service.h"
#include "../../library/libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "desc_client.h"
#include "shop_manager.h"
#include "group_text_parse_tree.h"
#include "shopEx.h"
#include <boost/algorithm/string/predicate.hpp>
#include "shop_manager.h"
#include <cctype>

CShopManager::CShopManager()
{
}

CShopManager::~CShopManager()
{
	Destroy();
}

bool CShopManager::Initialize(TShopTable * table, int size)
{
	if (!m_map_pkShop.empty())
	{
		for (TShopMap::iterator it = m_map_pkShop.begin(); it != m_map_pkShop.end(); it++)
		{
			it->second->RemoveAllGuests();
		}
	}

	m_map_pkShop.clear();
	m_map_pkShopByNPCVnum.clear();

	int i;

	for (i = 0; i < size; ++i, ++table)
	{
		LPSHOP shop = M2_NEW CShop;

		if (!shop->Create(table->dwVnum, table->dwNPCVnum, table->items))
		{
			M2_DELETE(shop);
			continue;
		}

		m_map_pkShop.insert(TShopMap::value_type(table->dwVnum, shop));
		m_map_pkShopByNPCVnum.insert(TShopMap::value_type(table->dwNPCVnum, shop));
	}
	char szShopTableExFileName[256];

	snprintf(szShopTableExFileName, sizeof(szShopTableExFileName),
		"%s/shop_table_ex.txt", LocaleService_GetBasePath().c_str());

#ifdef ENABLE_RENEWAL_SHOPEX
	return true;
#else
	return ReadShopTableEx(szShopTableExFileName);
#endif
}

void CShopManager::Destroy()
{
#ifdef ENABLE_RENEWAL_SHOPEX
	for (TShopMap::iterator it = m_map_pkShopByNPCVnum.begin(); it != m_map_pkShopByNPCVnum.end(); ++it)
		delete it->second;
	m_map_pkShopByNPCVnum.clear();
#else
	TShopMap::iterator it = m_map_pkShop.begin();

	while (it != m_map_pkShop.end())
	{
		M2_DELETE(it->second);
		++it;
	}

	m_map_pkShop.clear();
#endif
}

LPSHOP CShopManager::Get(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShop.find(dwVnum);

	if (it == m_map_pkShop.end())
		return NULL;

	return (it->second);
}

LPSHOP CShopManager::GetByNPCVnum(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShopByNPCVnum.find(dwVnum);

	if (it == m_map_pkShopByNPCVnum.end())
		return NULL;

	return (it->second);
}

#ifdef ENABLE_RENEWAL_SHOPEX
bool CShopManager::InitializeShopEX(TShopTable* table, int size)
{
	typedef std::multimap <DWORD, TShopTableEx> TMapNPCshop;
	TMapNPCshop map_npcShop;

	for (int i = 0; i < size; ++i, ++table)
	{
		TShopTableEx shopTable;
		shopTable.dwVnum = table->dwVnum;
		shopTable.name = table->szShopName;
		shopTable.coinType = SHOP_COIN_TYPE_GOLD;

		if (shopTable.name.length() >= SHOP_TAB_NAME_MAX)
		{
			sys_err("Shop name length must be less than %d. Error %s", SHOP_TAB_NAME_MAX, shopTable.name.c_str());
			return false;
		}

		CGrid grid(5, 9);

		memset(&shopTable.items[0], 0, sizeof(shopTable.items));

		for (size_t j = 0; j < table->byItemCount; j++)
		{
			TItemTable* item_table = ITEM_MANAGER::instance().GetTable(table->items[j].vnum);
			if (!item_table) {
				sys_err("vnum(%d) of group items of group %s does not exist.", table->items[j].vnum, shopTable.name.c_str());
				return false;
			}

			int iPos = grid.FindBlank(1, item_table->bSize);
			if (iPos == -1) {
				sys_err("vnum(%d) of group items of group %s there is no space!", table->items[j].vnum, shopTable.name.c_str());
				return false;
			}

			grid.Put(iPos, 1, item_table->bSize);
			shopTable.items[iPos] = table->items[j];
			shopTable.byItemCount++;
		}
		if (m_map_pkShopByNPCVnum.find(table->dwNPCVnum) != m_map_pkShopByNPCVnum.end())
		{
			sys_err("NPCVNUM(%d) already used.", table->dwNPCVnum);
			return false;
		}

		map_npcShop.insert(TMapNPCshop::value_type(table->dwNPCVnum, shopTable));
	}

	for (TMapNPCshop::iterator it = map_npcShop.begin(); it != map_npcShop.end(); ++it)
	{
		const DWORD npcVnum = it->first;
		TShopTableEx& table = it->second;

		if (m_map_pkShop.find(table.dwVnum) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exists.", table.dwVnum);
			return false;
		}

		TShopMap::const_iterator shop_it = m_map_pkShopByNPCVnum.find(npcVnum);
		LPSHOPEX pkShopEx = NULL;

		if (m_map_pkShopByNPCVnum.end() == shop_it)
		{
			pkShopEx = new CShopEx;
			pkShopEx->Create(0, npcVnum);
			m_map_pkShopByNPCVnum.insert(TShopMap::value_type(npcVnum, pkShopEx));
		}
		else
		{
			pkShopEx = dynamic_cast <CShopEx*> (shop_it->second);
			if (!pkShopEx) {
				sys_err("NPC(%d) Shop is not extended version.", shop_it->first);
				return false;
			}
		}

		if (pkShopEx->GetTabCount() >= SHOP_TAB_COUNT_MAX)
		{
			sys_err("ShopEx cannot have tab more than %d.", SHOP_TAB_COUNT_MAX);
			delete pkShopEx;
			return false;
		}

		pkShopEx->AddShopTable(table);
		m_map_pkShop.insert(TShopMap::value_type(table.dwVnum, pkShopEx));
	}

	return true;
}
#endif

bool CShopManager::StartShopping(LPCHARACTER pkChr, LPCHARACTER pkChrShopKeeper, int iShopVnum)
{
	if (pkChr->GetShopOwner() == pkChrShopKeeper)
		return false;

	if (pkChrShopKeeper->IsPC())
		return false;

	//PREVENT_TRADE_WINDOW
	if (pkChr->IsOpenSafebox() || pkChr->GetExchange() || pkChr->GetMyShop() || pkChr->IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		|| pkChr->IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		|| pkChr->GetTransmutation()
#endif
		)
	{
		pkChr->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade in the shop while another window is open."));
		return false;
	}
	//END_PREVENT_TRADE_WINDOW

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (pkChr->GetActiveGrowthPet())
	{
		pkChr->LocaleChatPacket(CHAT_TYPE_INFO, 737, "");
		return false;
	}

	if (pkChr->GetPetWindowType() == PET_WINDOW_ATTR_CHANGE || pkChr->GetPetWindowType() == PET_WINDOW_PRIMIUM_FEEDSTUFF)
	{
		pkChr->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("While modifying your pet's stats, you cannot trade, sell items, use the storeroom etc."));
		return false;
	}
#endif

	long distance = DISTANCE_APPROX(pkChr->GetX() - pkChrShopKeeper->GetX(), pkChr->GetY() - pkChrShopKeeper->GetY());

	if (distance >= SHOP_MAX_DISTANCE)
	{
		sys_log(1, "SHOP: TOO_FAR: %s distance %d", pkChr->GetName(), distance);
		return false;
	}

	LPSHOP pkShop;

	if (iShopVnum)
		pkShop = Get(iShopVnum);
	else
		pkShop = GetByNPCVnum(pkChrShopKeeper->GetRaceNum());

	if (!pkShop)
	{
		sys_log(1, "SHOP: NO SHOP");
		return false;
	}

	bool bOtherEmpire = false;

	if (pkChr->GetEmpire() != pkChrShopKeeper->GetEmpire())
		bOtherEmpire = true;

	pkShop->AddGuest(pkChr, pkChrShopKeeper->GetVID(), bOtherEmpire);
	pkChr->SetShopOwner(pkChrShopKeeper);
	sys_log(0, "SHOP: START: %s", pkChr->GetName());
	return true;
}

LPSHOP CShopManager::FindPCShop(DWORD dwVID)
{
	TShopMap::iterator it = m_map_pkShopByPC.find(dwVID);

	if (it == m_map_pkShopByPC.end())
		return NULL;

	return it->second;
}

#ifdef ENABLE_STACK_LIMIT
LPSHOP CShopManager::CreatePCShop(LPCHARACTER ch, TShopItemTable* pTable, WORD bItemCount)
#else
LPSHOP CShopManager::CreatePCShop(LPCHARACTER ch, TShopItemTable* pTable, BYTE bItemCount)
#endif
{
	if (FindPCShop(ch->GetVID()))
		return NULL;

	LPSHOP pkShop = M2_NEW CShop;
	pkShop->SetPCShop(ch);
	pkShop->SetShopItems(pTable, bItemCount);

	m_map_pkShopByPC.insert(TShopMap::value_type(ch->GetVID(), pkShop));
	return pkShop;
}

void CShopManager::DestroyPCShop(LPCHARACTER ch)
{
	LPSHOP pkShop = FindPCShop(ch->GetVID());

	if (!pkShop)
		return;

	//PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	m_map_pkShopByPC.erase(ch->GetVID());
	M2_DELETE(pkShop);
}

void CShopManager::StopShopping(LPCHARACTER ch)
{
	LPSHOP shop;

	if (!(shop = ch->GetShop()))
		return;

	//PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	shop->RemoveGuest(ch);
	sys_log(0, "SHOP: END: %s", ch->GetName());
}

void CShopManager::Buy(LPCHARACTER ch, BYTE pos)
{
	if (!ch->GetShop())
		return;

	if (!ch->GetShopOwner())
		return;

	if (DISTANCE_APPROX(ch->GetX() - ch->GetShopOwner()->GetX(), ch->GetY() - ch->GetShopOwner()->GetY()) > 2000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are too far away from the shop to buy something."));
		return;
	}

	CShop* pkShop = ch->GetShop();


	//PREVENT_ITEM_COPY
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	int ret = pkShop->Buy(ch, pos);

	if (SHOP_SUBHEADER_GC_OK != ret)
	{
		TPacketGCShop pack;

		pack.header = HEADER_GC_SHOP;
		pack.subheader = ret;
		pack.size = sizeof(TPacketGCShop);

		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}
}

#ifdef ENABLE_STACK_LIMIT
void CShopManager::Sell(LPCHARACTER ch, BYTE bCell, WORD bCount)
#else
void CShopManager::Sell(LPCHARACTER ch, BYTE bCell, BYTE bCount)
#endif
{
	if (!ch->CanHandleItem())
		return;

#ifndef DISABLE_SHOP_DISTANCE
	if (ch->GetShop())
	{
		if (ch->GetShop()->IsPCShop())
			return;
	}
#else
	if (!ch->GetShop())
		return;

	if (!ch->GetShopOwner())
		return;

	if (ch->GetShop()->IsPCShop())
		return;

	if (DISTANCE_APPROX(ch->GetX()-ch->GetShopOwner()->GetX(), ch->GetY()-ch->GetShopOwner()->GetY())>2000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are too far away from the shop to sell something."));
		return;
	}
#endif

	LPITEM item = ch->GetInventoryItem(bCell);

	if (!item)
		return;

	if (item->IsEquipped() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot sell equipped items."));
		return;
	}

	if (true == item->isLocked())
	{
		return;
	}

	// SECURITY: trade'deki item'in bir kismini satip karsi tarafi kandirma / UAF
	if (item->IsExchanging() || ch->GetExchange())
		return;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SELL))
		return;

#ifdef ENABLE_SEALBIND_SYSTEM
	if (item->IsSealed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ruh bagi ile bagli esyayi satamazsin."));
		return;
	}
#endif

#ifdef ENABLE_GOLD_LIMIT
	long long dwPrice;
#else
	DWORD dwPrice;
#endif

	if (bCount == 0 || bCount > item->GetCount())
		bCount = item->GetCount();

	dwPrice = item->GetShopBuyPrice();

	if (IS_SET(item->GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (dwPrice == 0)
			dwPrice = bCount;
		else
			dwPrice = bCount / dwPrice;
	}
	else
		dwPrice *= bCount;

	dwPrice /= 1;

	if (test_server)
		sys_log(0, "Sell Item price id %d %s itemid %d", ch->GetPlayerID(), ch->GetName(), item->GetID());

#ifdef ENABLE_GOLD_LIMIT
	const long long nTotalMoney = static_cast<long long>(ch->GetGold()) + static_cast<long long>(dwPrice);
#else
	const int64_t nTotalMoney = static_cast<int64_t>(ch->GetGold()) + static_cast<int64_t>(dwPrice);
#endif

	if (GOLD_MAX <= nTotalMoney)
	{
#ifdef ENABLE_GOLD_LIMIT
		sys_err("[OVERFLOW_GOLD] id %u name %s gold %lld", ch->GetPlayerID(), ch->GetName(), ch->GetGold());
#else
		sys_err("[OVERFLOW_GOLD] id %u name %s gold %u", ch->GetPlayerID(), ch->GetName(), ch->GetGold());
#endif
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade because you are carrying more than 2 billion Yang."));
		return;
	}

#ifdef ENABLE_GOLD_LIMIT
	sys_log(0, "SHOP: SELL: %s item name: %s(x%d):%u price: %lld", ch->GetName(), item->GetName(), bCount, item->GetID(), dwPrice);
#else
	sys_log(0, "SHOP: SELL: %s item name: %s(x%d):%u price: %u", ch->GetName(), item->GetName(), bCount, item->GetID(), dwPrice);
#endif

	DBManager::instance().SendMoneyLog(MONEY_LOG_SHOP, item->GetVnum(), dwPrice);

	if (bCount == item->GetCount())
	{
		ITEM_MANAGER::instance().RemoveItem(item, "SELL");
	}
	else
		item->SetCount(item->GetCount() - bCount);

	ch->PointChange(POINT_GOLD, dwPrice, false);
}

bool CompareShopItemName(const SShopItemTable& lhs, const SShopItemTable& rhs)
{
	TItemTable* lItem = ITEM_MANAGER::instance().GetTable(lhs.vnum);
	TItemTable* rItem = ITEM_MANAGER::instance().GetTable(rhs.vnum);
	if (lItem && rItem)
		return strcmp(lItem->szLocaleName, rItem->szLocaleName) < 0;
	else
		return true;
}

bool ConvertToShopItemTable(IN CGroupNode* pNode, OUT TShopTableEx& shopTable)
{
	if (!pNode->GetValue("vnum", 0, shopTable.dwVnum))
	{
		sys_err("Group %s does not have vnum.", pNode->GetNodeName().c_str());
		return false;
	}

	if (!pNode->GetValue("name", 0, shopTable.name))
	{
		sys_err("Group %s does not have name.", pNode->GetNodeName().c_str());
		return false;
	}

	if (shopTable.name.length() >= SHOP_TAB_NAME_MAX)
	{
		sys_err("Shop name length must be less than %d. Error in Group %s, name %s", SHOP_TAB_NAME_MAX, pNode->GetNodeName().c_str(), shopTable.name.c_str());
		return false;
	}

	std::string stCoinType;
	if (!pNode->GetValue("cointype", 0, stCoinType))
	{
		stCoinType = "Gold";
	}

	if (boost::iequals(stCoinType, "Gold"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_GOLD;
	}
	else if (boost::iequals(stCoinType, "SecondaryCoin"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_SECONDARY_COIN;
	}
	else
	{
		sys_err("Group %s has undefine cointype(%s).", pNode->GetNodeName().c_str(), stCoinType.c_str());
		return false;
	}

	CGroupNode* pItemGroup = pNode->GetChildNode("items");
	if (!pItemGroup)
	{
		sys_err("Group %s does not have 'group items'.", pNode->GetNodeName().c_str());
		return false;
	}

	int itemGroupSize = pItemGroup->GetRowCount();
	std::vector <TShopItemTable> shopItems(itemGroupSize);
	if (itemGroupSize >= SHOP_HOST_ITEM_MAX_NUM)
	{
		sys_err("count(%d) of rows of group items of group %s must be smaller than %d", itemGroupSize, pNode->GetNodeName().c_str(), SHOP_HOST_ITEM_MAX_NUM);
		return false;
	}

	for (int i = 0; i < itemGroupSize; i++)
	{
		if (!pItemGroup->GetValue(i, "vnum", shopItems[i].vnum))
		{
			sys_err("row(%d) of group items of group %s does not have vnum column", i, pNode->GetNodeName().c_str());
			return false;
		}

		if (!pItemGroup->GetValue(i, "count", shopItems[i].count))
		{
			sys_err("row(%d) of group items of group %s does not have count column", i, pNode->GetNodeName().c_str());
			return false;
		}
		if (!pItemGroup->GetValue(i, "price", shopItems[i].price))
		{
			sys_err("row(%d) of group items of group %s does not have price column", i, pNode->GetNodeName().c_str());
			return false;
		}
	}
	std::string stSort;
	if (!pNode->GetValue("sort", 0, stSort))
	{
		stSort = "None";
	}

	if (boost::iequals(stSort, "Asc"))
	{
		std::sort(shopItems.begin(), shopItems.end(), CompareShopItemName);
	}
	else if (boost::iequals(stSort, "Desc"))
	{
		std::sort(shopItems.rbegin(), shopItems.rend(), CompareShopItemName);
	}

	CGrid grid = CGrid(5, 9);
	int iPos;

	memset(&shopTable.items[0], 0, sizeof(shopTable.items));

	for (int i = 0; i < shopItems.size(); i++)
	{
		TItemTable * item_table = ITEM_MANAGER::instance().GetTable(shopItems[i].vnum);
		if (!item_table)
		{
			sys_err("vnum(%d) of group items of group %s does not exist", shopItems[i].vnum, pNode->GetNodeName().c_str());
			return false;
		}

		iPos = grid.FindBlank(1, item_table->bSize);

		grid.Put(iPos, 1, item_table->bSize);
		shopTable.items[iPos] = shopItems[i];
	}

	shopTable.byItemCount = shopItems.size();
	return true;
}

bool CShopManager::ReadShopTableEx(const char* stFileName)
{
	FILE* fp = fopen(stFileName, "rb");
	if (NULL == fp)
		return true;
	fclose(fp);

	CGroupTextParseTreeLoader loader;
	if (!loader.Load(stFileName))
	{
		sys_err("%s Load fail.", stFileName);
		return false;
	}

	CGroupNode* pShopNPCGroup = loader.GetGroup("shopnpc");
	if (NULL == pShopNPCGroup)
	{
		sys_err("Group ShopNPC is not exist.");
		return false;
	}

	typedef std::multimap <DWORD, TShopTableEx> TMapNPCshop;
	TMapNPCshop map_npcShop;
	for (int i = 0; i < pShopNPCGroup->GetRowCount(); i++)
	{
		DWORD npcVnum;
		std::string shopName;
		if (!pShopNPCGroup->GetValue(i, "npc", npcVnum) || !pShopNPCGroup->GetValue(i, "group", shopName))
		{
			sys_err("Invalid row(%d). Group ShopNPC rows must have 'npc', 'group' columns", i);
			return false;
		}
		std::transform(shopName.begin(), shopName.end(), shopName.begin(), (int(*)(int))std::tolower);
		CGroupNode* pShopGroup = loader.GetGroup(shopName.c_str());
		if (!pShopGroup)
		{
			sys_err("Group %s is not exist.", shopName.c_str());
			return false;
		}
		TShopTableEx table;
		if (!ConvertToShopItemTable(pShopGroup, table))
		{
			sys_err("Cannot read Group %s.", shopName.c_str());
			return false;
		}
		if (m_map_pkShopByNPCVnum.find(npcVnum) != m_map_pkShopByNPCVnum.end())
		{
			sys_err("%d cannot have both original shop and extended shop", npcVnum);
			return false;
		}

		map_npcShop.insert(TMapNPCshop::value_type(npcVnum, table));
	}

	for (TMapNPCshop::iterator it = map_npcShop.begin(); it != map_npcShop.end(); ++it)
	{
		DWORD npcVnum = it->first;
		TShopTableEx& table = it->second;
		if (m_map_pkShop.find(table.dwVnum) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exists", table.dwVnum);
			return false;
		}
		TShopMap::iterator shop_it = m_map_pkShopByNPCVnum.find(npcVnum);

		LPSHOPEX pkShopEx = NULL;
		if (m_map_pkShopByNPCVnum.end() == shop_it)
		{
			pkShopEx = M2_NEW CShopEx;
			pkShopEx->Create(0, npcVnum);
			m_map_pkShopByNPCVnum.insert(TShopMap::value_type(npcVnum, pkShopEx));
		}
		else
		{
			pkShopEx = dynamic_cast <CShopEx*> (shop_it->second);
			if (NULL == pkShopEx)
			{
				sys_err("WTF!!! It can't be happend. NPC(%d) Shop is not extended version.", shop_it->first);
				return false;
			}
		}

		if (pkShopEx->GetTabCount() >= SHOP_TAB_COUNT_MAX)
		{
			sys_err("ShopEx cannot have tab more than %d", SHOP_TAB_COUNT_MAX);
			return false;
		}

		if (m_map_pkShop.find(table.dwVnum) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exist.", table.dwVnum);
			return false;
		}
		m_map_pkShop.insert(TShopMap::value_type(table.dwVnum, pkShopEx));
		pkShopEx->AddShopTable(table);
	}

	return true;
}