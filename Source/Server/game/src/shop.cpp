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

/* ------------------------------------------------------------------------------------ */
CShop::CShop() : m_dwVnum(0), m_dwNPCVnum(0), m_pkPC(NULL)
{
	m_pGrid = M2_NEW CGrid(5, 9);
}

CShop::~CShop()
{
	TPacketGCShop pack;

	pack.header = HEADER_GC_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_END;
	pack.size = sizeof(TPacketGCShop);

	Broadcast(&pack, sizeof(pack));

	GuestMapType::iterator it;

	it = m_map_guest.begin();

	while (it != m_map_guest.end())
	{
		LPCHARACTER ch = it->first;
		ch->SetShop(NULL);
		++it;
	}

	M2_DELETE(m_pGrid);
}

void CShop::SetPCShop(LPCHARACTER ch)
{
	m_pkPC = ch;
}

bool CShop::Create(DWORD dwVnum, DWORD dwNPCVnum, TShopItemTable * pTable)
{
	sys_log(0, "SHOP #%d (Shopkeeper %d)", dwVnum, dwNPCVnum);

	m_dwVnum = dwVnum;
	m_dwNPCVnum = dwNPCVnum;

#ifdef ENABLE_STACK_LIMIT
	WORD bItemCount;
#else
	BYTE bItemCount;
#endif

	for (bItemCount = 0; bItemCount < SHOP_HOST_ITEM_MAX_NUM; ++bItemCount)
		if (0 == (pTable + bItemCount)->vnum)
			break;

	SetShopItems(pTable, bItemCount);
	return true;
}

#ifdef ENABLE_STACK_LIMIT
void CShop::SetShopItems(TShopItemTable* pTable, WORD bItemCount)
#else
void CShop::SetShopItems(TShopItemTable* pTable, BYTE bItemCount)
#endif
{
	if (bItemCount > SHOP_HOST_ITEM_MAX_NUM)
		return;

	m_pGrid->Clear();

	m_itemVector.resize(SHOP_HOST_ITEM_MAX_NUM);
	memset(&m_itemVector[0], 0, sizeof(SHOP_ITEM) * m_itemVector.size());

	for (int i = 0; i < bItemCount; ++i)
	{
		LPITEM pkItem = NULL;
		const TItemTable * item_table;

		if (m_pkPC)
		{
			pkItem = m_pkPC->GetItem(pTable->pos);

			if (!pkItem)
			{
				sys_err("cannot find item on pos (%d, %d) (name: %s)", pTable->pos.window_type, pTable->pos.cell, m_pkPC->GetName());
				continue;
			}

			item_table = pkItem->GetProto();
		}
		else
		{
			if (!pTable->vnum)
				continue;

			item_table = ITEM_MANAGER::instance().GetTable(pTable->vnum);
		}

		if (!item_table)
		{
			sys_err("Shop: no item table by item vnum #%d", pTable->vnum);
			continue;
		}

		int iPos;

		if (IsPCShop())
		{
			sys_log(0, "MyShop: use position %d", pTable->display_pos);
			iPos = pTable->display_pos;
		}
		else
			iPos = m_pGrid->FindBlank(1, item_table->bSize);

		if (iPos < 0)
		{
			sys_err("not enough shop window");
			continue;
		}

		if (!m_pGrid->IsEmpty(iPos, 1, item_table->bSize))
		{
			if (IsPCShop())
			{
				sys_err("not empty position for pc shop %s[%d]", m_pkPC->GetName(), m_pkPC->GetPlayerID());
			}
			else
			{
				sys_err("not empty position for npc shop");
			}
			continue;
		}

		m_pGrid->Put(iPos, 1, item_table->bSize);

		SHOP_ITEM & item = m_itemVector[iPos];

		item.pkItem = pkItem;
		item.itemid = 0;

		if (item.pkItem)
		{
			item.vnum = pkItem->GetVnum();
			item.count = pkItem->GetCount();
			item.price = pTable->price;
			item.itemid	= pkItem->GetID();
		}
		else
		{
			item.vnum = pTable->vnum;
			item.count = pTable->count;

			if (IS_SET(item_table->dwFlags, ITEM_FLAG_COUNT_PER_1GOLD))
			{
				if (item_table->dwGold == 0)
					item.price = item.count;
				else
					item.price = item.count / item_table->dwGold;
			}
			else
				item.price = item_table->dwGold * item.count;
		}

		char name[36];
		snprintf(name, sizeof(name), "%-20s(#%-5d) (x %d)", item_table->szName, (int) item.vnum, item.count);

		sys_log(0, "SHOP_ITEM: %-36s PRICE %-5d", name, item.price);
		++pTable;
	}
}

#ifdef ENABLE_GOLD_LIMIT
long long CShop::Buy(LPCHARACTER ch, BYTE pos)
#else
int CShop::Buy(LPCHARACTER ch, BYTE pos)
#endif
{
	if (pos >= m_itemVector.size())
	{
		sys_log(0, "Shop::Buy : invalid position %d : %s", pos, ch->GetName());
		return SHOP_SUBHEADER_GC_INVALID_POS;
	}

	sys_log(0, "Shop::Buy : name %s pos %d", ch->GetName(), pos);

	GuestMapType::iterator it = m_map_guest.find(ch);

	if (it == m_map_guest.end())
		return SHOP_SUBHEADER_GC_END;

	SHOP_ITEM& r_item = m_itemVector[pos];

	if (r_item.price < 0)//@fixme275
	{
		LogManager::instance().HackLog("SHOP_BUY_GOLD_OVERFLOW", ch);
		return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
	}

	LPITEM pkSelectedItem = ITEM_MANAGER::instance().Find(r_item.itemid);

	if (IsPCShop())
	{
		if (!pkSelectedItem)
		{
			sys_log(0, "Shop::Buy : Critical: This user seems to be a hacker : invalid pcshop item : BuyerPID:%d SellerPID:%d", ch->GetPlayerID(), m_pkPC->GetPlayerID());
			return false;
		}

		if ((pkSelectedItem->GetOwner() != m_pkPC))
		{
			sys_log(0, "Shop::Buy : Critical: This user seems to be a hacker : invalid pcshop item : BuyerPID:%d SellerPID:%d", ch->GetPlayerID(), m_pkPC->GetPlayerID());
			return false;
		}
	}

#ifdef ENABLE_GOLD_LIMIT
	long long dwPrice = r_item.price;
#else
	DWORD dwPrice = r_item.price;
#endif

#ifdef ENABLE_GOLD_LIMIT
	if (ch->GetGold() < (long long) dwPrice)
	{
		sys_log(1, "Shop::Buy : Not enough money : %s has %lld, price %lld", ch->GetName(), ch->GetGold(), dwPrice);
		return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
	}
#else
	if (ch->GetGold() < (int) dwPrice)
	{
		sys_log(1, "Shop::Buy : Not enough money : %s has %d, price %d", ch->GetName(), ch->GetGold(), dwPrice);
		return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
	}
#endif

#ifdef ENABLE_PCSHOP_GOLD_OVERFLOW_FIX
	// Seller must be able to receive the payment, otherwise the gold would vanish
	if (IsPCShop() && m_pkPC)
	{
		if ((long long) m_pkPC->GetGold() + (long long) dwPrice >= (long long) GOLD_MAX)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s", "Satici bu kadar altin tasiyamaz, alim iptal edildi.");
			return SHOP_SUBHEADER_GC_SOLD_OUT;
		}
	}
#endif

#ifdef ENABLE_ZODIAC_MISSION
	if((r_item.vnum >= 33001 && r_item.vnum <= 33030) || (r_item.vnum == 33034))
	{
		char text[256];
		snprintf(text,sizeof(text),"Zodiac_Item_%d",r_item.vnum);
		if(ch->GetProtectTime(text) <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Nesne tukendi."));
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_LUCKY;
		}
		else
		{
			ch->SetProtectTime(text,ch->GetProtectTime(text)-50);
		}
	}
#endif

	LPITEM item;

	if (m_pkPC)
		item = r_item.pkItem;
	else
		item = ITEM_MANAGER::instance().CreateItem(r_item.vnum, r_item.count);

	if (!item)
		return SHOP_SUBHEADER_GC_SOLD_OUT;

	int iEmptyPos;
	if (item->IsDragonSoul())
	{
		iEmptyPos = ch->GetEmptyDragonSoulInventory(item);
	}
#ifdef ENABLE_SPECIAL_INVENTORY
	else if (item->IsSkillBook())
	{
		iEmptyPos = ch->GetEmptySkillBookInventory(item->GetSize());
	}
	else if (item->IsUpgradeItem())
	{
		iEmptyPos = ch->GetEmptyUpgradeItemsInventory(item->GetSize());
	}
	else if (item->IsStone())
	{
		iEmptyPos = ch->GetEmptyStoneInventory(item->GetSize());
	}
	else if (item->IsGiftBox())
	{
		iEmptyPos = ch->GetEmptyGiftBoxInventory(item->GetSize());
	}
	else if (item->IsChanger())
	{
		iEmptyPos = ch->GetEmptyChangersInventory(item->GetSize());
	}
#endif
	else
	{
		iEmptyPos = ch->GetEmptyInventory(item->GetSize());
	}

	if (iEmptyPos < 0)
	{
		if (m_pkPC)
		{
			sys_log(1, "Shop::Buy at PC Shop : Inventory full : %s size %d", ch->GetName(), item->GetSize());
			return SHOP_SUBHEADER_GC_INVENTORY_FULL;
		}
		else
		{
			sys_log(1, "Shop::Buy : Inventory full : %s size %d", ch->GetName(), item->GetSize());
			M2_DESTROY_ITEM(item);
			return SHOP_SUBHEADER_GC_INVENTORY_FULL;
		}
	}

	ch->PointChange(POINT_GOLD, -dwPrice, false);

	if (m_pkPC)
	{
		m_pkPC->SyncQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 255);

		char buf[512];

		if (item->GetVnum() >= 80003 && item->GetVnum() <= 80007)
		{
#ifdef ENABLE_GOLD_LIMIT
			snprintf(buf, sizeof(buf), "%s FROM: %u TO: %u PRICE: %lld", item->GetName(), ch->GetPlayerID(), m_pkPC->GetPlayerID(), dwPrice);
#else
			snprintf(buf, sizeof(buf), "%s FROM: %u TO: %u PRICE: %u", item->GetName(), ch->GetPlayerID(), m_pkPC->GetPlayerID(), dwPrice);
#endif
			LogManager::instance().GoldBarLog(ch->GetPlayerID(), item->GetID(), SHOP_BUY, buf);
			LogManager::instance().GoldBarLog(m_pkPC->GetPlayerID(), item->GetID(), SHOP_SELL, buf);
		}
		
		item->RemoveFromCharacter();
		if (item->IsDragonSoul())
			item->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
#ifdef ENABLE_SPECIAL_INVENTORY
		else if (item->IsSkillBook())
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (item->IsUpgradeItem())
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (item->IsStone())
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (item->IsGiftBox())
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (item->IsChanger())
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
		else
		{
			WORD bCount = item->GetCount();

#ifdef ENABLE_SHOP_STACK_MERGE
			if (IS_SET(item->GetFlag(), ITEM_FLAG_STACKABLE))
			{
				for (WORD i = 0; i < INVENTORY_MAX_NUM; ++i)
				{
					LPITEM item2 = ch->GetInventoryItem(i);

					if (!item2)
						continue;

					if (item2->GetVnum() == item->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
							if (item2->GetSocket(j) != item->GetSocket(j))
								break;

						if (j != ITEM_SOCKET_MAX_NUM)
							continue;

						WORD bCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), bCount);
						bCount -= bCount2;
						item2->SetCount(item2->GetCount() + bCount2);

						if (bCount == 0)
							break;
					}
				}

				item->SetCount(bCount);
			}
#endif

			if (bCount > 0)
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else
				M2_DESTROY_ITEM(item);
		}

		ITEM_MANAGER::instance().FlushDelayedSave(item);

#ifdef ENABLE_GOLD_LIMIT
		snprintf(buf, sizeof(buf), "%s %u(%s) %lld %u", item->GetName(), m_pkPC->GetPlayerID(), m_pkPC->GetName(), dwPrice, item->GetCount());
#else
		snprintf(buf, sizeof(buf), "%s %u(%s) %u %u", item->GetName(), m_pkPC->GetPlayerID(), m_pkPC->GetName(), dwPrice, item->GetCount());
#endif
		LogManager::instance().ItemLog(ch, item, "SHOP_BUY", buf);

#ifdef ENABLE_GOLD_LIMIT
		snprintf(buf, sizeof(buf), "%s %u(%s) %lld %u", item->GetName(), ch->GetPlayerID(), ch->GetName(), dwPrice, item->GetCount());
#else
		snprintf(buf, sizeof(buf), "%s %u(%s) %u %u", item->GetName(), ch->GetPlayerID(), ch->GetName(), dwPrice, item->GetCount());
#endif
		LogManager::instance().ItemLog(m_pkPC, item, "SHOP_SELL", buf);

		r_item.pkItem = NULL;
		BroadcastUpdateItem(pos);

		m_pkPC->PointChange(POINT_GOLD, dwPrice, false);
	}
	else
	{
		if (item->IsDragonSoul())
			item->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
#ifdef ENABLE_SPECIAL_INVENTORY
		else if (item->IsSkillBook())
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (item->IsUpgradeItem())
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (item->IsStone())
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (item->IsGiftBox())
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		else if (item->IsChanger())
			item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
		else
		{
			WORD bCount = item->GetCount();

#ifdef ENABLE_SHOP_STACK_MERGE
			if (IS_SET(item->GetFlag(), ITEM_FLAG_STACKABLE))
			{
				for (WORD i = 0; i < INVENTORY_MAX_NUM; ++i)
				{
					LPITEM item2 = ch->GetInventoryItem(i);

					if (!item2)
						continue;

					if (item2->GetVnum() == item->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
							if (item2->GetSocket(j) != item->GetSocket(j))
								break;

						if (j != ITEM_SOCKET_MAX_NUM)
							continue;

						WORD bCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), bCount);
						bCount -= bCount2;
						item2->SetCount(item2->GetCount() + bCount2);

						if (bCount == 0)
							break;
					}
				}

				item->SetCount(bCount);
			}
#endif

			if (bCount > 0)
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			else
				M2_DESTROY_ITEM(item);
		}

		ITEM_MANAGER::instance().FlushDelayedSave(item);
		LogManager::instance().ItemLog(ch, item, "BUY", item->GetName());

		if (item->GetVnum() >= 80003 && item->GetVnum() <= 80007)
		{
			LogManager::instance().GoldBarLog(ch->GetPlayerID(), item->GetID(), PERSONAL_SHOP_BUY, "");
		}

		DBManager::instance().SendMoneyLog(MONEY_LOG_SHOP, item->GetVnum(), -dwPrice);
	}

	if (item)
#ifdef ENABLE_GOLD_LIMIT
		sys_log(0, "SHOP: BUY: name %s %s(x %d):%u price %lld", ch->GetName(), item->GetName(), item->GetCount(), item->GetID(), dwPrice);
#else
		sys_log(0, "SHOP: BUY: name %s %s(x %d):%u price %u", ch->GetName(), item->GetName(), item->GetCount(), item->GetID(), dwPrice);
#endif
	ch->Save();

	return (SHOP_SUBHEADER_GC_OK);
}

bool CShop::AddGuest(LPCHARACTER ch, DWORD owner_vid, bool bOtherEmpire)
{
	if (!ch)
		return false;

	if (ch->GetExchange())
		return false;

	if (ch->GetShop())
		return false;

	ch->SetShop(this);

	m_map_guest.insert(GuestMapType::value_type(ch, bOtherEmpire));

	TPacketGCShop pack;

	pack.header = HEADER_GC_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_START;

	TPacketGCShopStart pack2;

	memset(&pack2, 0, sizeof(pack2));
	pack2.owner_vid = owner_vid;

	for (DWORD i = 0; i < m_itemVector.size() && i < SHOP_HOST_ITEM_MAX_NUM; ++i)
	{
		const SHOP_ITEM & item = m_itemVector[i];

		//END_HIVALUE_ITEM_EVENT
		if (m_pkPC && !item.pkItem)
			continue;

		pack2.items[i].vnum = item.vnum;
		pack2.items[i].price = item.price;
		pack2.items[i].count = item.count;
#ifdef ENABLE_RENEWAL_SHOPEX
		pack2.items[i].price_type = SHOPEX_GOLD;
#endif

		if (item.pkItem)
		{
			thecore_memcpy(pack2.items[i].alSockets, item.pkItem->GetSockets(), sizeof(pack2.items[i].alSockets));
			thecore_memcpy(pack2.items[i].aAttr, item.pkItem->GetAttributes(), sizeof(pack2.items[i].aAttr));
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			pack2.items[i].dwTransmutationVnum = item.pkItem->GetTransmutationVnum();
#endif
		}
	}

	pack.size = sizeof(pack) + sizeof(pack2);

	ch->GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
	ch->GetDesc()->Packet(&pack2, sizeof(TPacketGCShopStart));
	return true;
}

void CShop::RemoveGuest(LPCHARACTER ch)
{
	if (ch->GetShop() != this)
		return;

	m_map_guest.erase(ch);
	ch->SetShop(NULL);

	TPacketGCShop pack;

	pack.header		= HEADER_GC_SHOP;
	pack.subheader	= SHOP_SUBHEADER_GC_END;
	pack.size		= sizeof(TPacketGCShop);

	ch->GetDesc()->Packet(&pack, sizeof(pack));
}

void CShop::Broadcast(const void * data, int bytes)
{
	sys_log(1, "Shop::Broadcast %p %d", data, bytes);

	GuestMapType::iterator it;

	it = m_map_guest.begin();

	while (it != m_map_guest.end())
	{
		LPCHARACTER ch = it->first;

		if (ch->GetDesc())
			ch->GetDesc()->Packet(data, bytes);

		++it;
	}
}

void CShop::BroadcastUpdateItem(BYTE pos)
{
	TPacketGCShop pack;
	TPacketGCShopUpdateItem pack2;

	TEMP_BUFFER buf;

	pack.header = HEADER_GC_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_UPDATE_ITEM;
	pack.size = sizeof(pack) + sizeof(pack2);

	pack2.pos = pos;

	if (m_pkPC && !m_itemVector[pos].pkItem)
		pack2.item.vnum = 0;
	else
	{
		pack2.item.vnum	= m_itemVector[pos].vnum;
		if (m_itemVector[pos].pkItem)
		{
			thecore_memcpy(pack2.item.alSockets, m_itemVector[pos].pkItem->GetSockets(), sizeof(pack2.item.alSockets));
			thecore_memcpy(pack2.item.aAttr, m_itemVector[pos].pkItem->GetAttributes(), sizeof(pack2.item.aAttr));
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			pack2.item.dwTransmutationVnum = m_itemVector[pos].pkItem->GetTransmutationVnum();
#endif
		}
		else
		{
			memset(pack2.item.alSockets, 0, sizeof(pack2.item.alSockets));
			memset(pack2.item.aAttr, 0, sizeof(pack2.item.aAttr));
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			pack2.item.dwTransmutationVnum = 0;
#endif
		}
	}

	pack2.item.price = m_itemVector[pos].price;
	pack2.item.count = m_itemVector[pos].count;

	buf.write(&pack, sizeof(pack));
	buf.write(&pack2, sizeof(pack2));

	Broadcast(buf.read_peek(), buf.size());
}

int CShop::GetNumberByVnum(DWORD dwVnum)
{
	int itemNumber = 0;

	for (DWORD i = 0; i < m_itemVector.size() && i < SHOP_HOST_ITEM_MAX_NUM; ++i)
	{
		const SHOP_ITEM & item = m_itemVector[i];

		if (item.vnum == dwVnum)
		{
			itemNumber += item.count;
		}
	}

	return itemNumber;
}

bool CShop::IsSellingItem(DWORD itemID)
{
	bool isSelling = false;

	for (DWORD i = 0; i < m_itemVector.size() && i < SHOP_HOST_ITEM_MAX_NUM; ++i)
	{
		if (m_itemVector[i].itemid == itemID)
		{
			isSelling = true;
			break;
		}
	}

	return isSelling;

}

void CShop::RemoveAllGuests()
{
	if (m_map_guest.empty())
		return;

	for (GuestMapType::iterator it = m_map_guest.begin(); it != m_map_guest.end(); it++)
	{
		LPCHARACTER ch = it->first;
		if (ch)
		{
			if (ch->GetDesc() && ch->GetShop() == this)
			{
				ch->SetShop(NULL);

				TPacketGCShop pack;

				pack.header = HEADER_GC_SHOP;
				pack.subheader = SHOP_SUBHEADER_GC_END;
				pack.size = sizeof(TPacketGCShop);

				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
	}

	m_map_guest.clear();
}
