#include "stdafx.h"

#ifdef ENABLE_RENEWAL_OFFLINESHOP
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "sectree_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"

#include "db.h"
#include "questmanager.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "offline_shop.h"
#include "p2p.h"
#include "offlineshop_manager.h"
#include "desc_client.h"
#include "target.h"
#include "desc_client.h"

void COfflineShop::DestroyEx()
{
	for (auto it = m_map_guest.begin(); it != m_map_guest.end(); ++it)
		RemoveGuest(it->second, true);
	m_map_guest.clear();
	if (GetOfflineShopNPC() != NULL)
		M2_DESTROY_CHARACTER(GetOfflineShopNPC());
	m_pkOfflineShopNPC = NULL;
}

void COfflineShop::Destroy()
{
	for(auto it = m_map_guest.begin();it!= m_map_guest.end();++it)
	{
		RemoveGuest(it->second, true);
	}

	m_map_guest.clear();
	if (GetOfflineShopNPC() != NULL)
		M2_DESTROY_CHARACTER(GetOfflineShopNPC());

	m_pkOfflineShopNPC = NULL;
	m_dwDisplayedCount=0;
	m_dwRealWatcherCount=0;
	memset(&m_data.log, 0, sizeof(m_data.log));
	memset(&m_data.items, 0, sizeof(m_data.items));
	memset(&m_data, 0, sizeof(TOfflineShop));
}

COfflineShop::COfflineShop():m_pkOfflineShopNPC(NULL), m_dwDisplayedCount(0), m_dwRealWatcherCount(0)
{
	Destroy();
}

COfflineShop::~COfflineShop()
{
	Destroy();
}

bool COfflineShop::AddGuest(LPCHARACTER ch, LPCHARACTER npc)
{
	if (!ch)
		return false;

	if (ch->GetOfflineShop())
		return false;

	auto it = m_map_guest.find(ch->GetPlayerID());
	if (it != m_map_guest.end())
		return false;

	TPacketGCShop pack;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_START;

	TPacketGCOfflineShopStart pack2;
	memset(&pack2, 0, sizeof(pack2));
	if(npc)
		pack2.owner_vid = npc->GetVID();

	++m_dwRealWatcherCount;
	if (ch->GetPlayerID() != m_data.owner_id)
		++m_dwDisplayedCount;

	if (m_map_guest.size() > 0)
	{
		DWORD count[2] = { 0,0 };
		count[0] = m_dwDisplayedCount;
		count[1] = m_dwRealWatcherCount;
		TPacketGCShop pack_display;
		TEMP_BUFFER buf;
		pack_display.header = HEADER_GC_OFFLINE_SHOP;
		pack_display.subheader = SHOP_SUBHEADER_GC_REFRESH_COUNT;
		pack_display.size = sizeof(pack_display) + sizeof(count);
		buf.write(&pack_display, sizeof(pack_display));
		buf.write(&count, sizeof(count));
		Broadcast(buf.read_peek(), buf.size());
	}

	strlcpy(pack2.title, m_data.sign, sizeof(pack2.title));
	pack2.m_dwRealWatcherCount = m_dwRealWatcherCount;
	pack2.m_dwDisplayedCount = m_dwDisplayedCount;
	pack2.flag = m_data.slotflag;
	pack2.type = m_data.type;
	if (ch->GetPlayerID() == m_data.owner_id)
	{
		pack2.IsOwner = true;
		pack2.time = m_data.time;
		pack2.price = m_data.price;
		thecore_memcpy(&pack2.log, &m_data.log, sizeof(pack2.log));
	}
	else
		pack2.IsOwner = false;

	for (DWORD i = 0; i < OFFLINE_SHOP_HOST_ITEM_MAX_NUM; ++i)
	{
		OFFLINE_SHOP_ITEM& item = m_data.items[i];
		if (item.vnum == 0)
			continue;
		pack2.items[item.pos].owner_id = item.owner_id;
		pack2.items[item.pos].count = item.count;
		pack2.items[item.pos].vnum = item.vnum;
		pack2.items[item.pos].price = item.price;
		pack2.items[item.pos].status = item.status;
		strlcpy(pack2.items[item.pos].szBuyerName, item.szBuyerName, sizeof(pack2.items[item.pos].szBuyerName));
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		pack2.items[item.pos].transmutation = item.transmutation;
#endif
		thecore_memcpy(&pack2.items[item.pos].alSockets, &item.alSockets, sizeof(pack2.items[item.pos].alSockets));
		thecore_memcpy(&pack2.items[item.pos].aAttr, &item.aAttr, sizeof(pack2.items[item.pos].aAttr));
	}

	pack.size = sizeof(pack) + sizeof(pack2);
	ch->GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
	ch->GetDesc()->Packet(&pack2, sizeof(TPacketGCOfflineShopStart));

	ch->SetOfflineShop(this);
	m_map_guest.insert(std::make_pair(ch->GetPlayerID(),ch));

	sys_log(0, "COfflineShop::AddGuest: shop: %s customer: %s", m_data.owner_name, ch->GetName());
	return true;
}

void COfflineShop::RemoveGuest(LPCHARACTER ch, bool isDestroy)
{
	if(ch && ch->GetDesc())
	{
		ch->SetOfflineShop(NULL);

		TPacketGCShop pack;
		pack.header = HEADER_GC_OFFLINE_SHOP;
		pack.subheader = SHOP_SUBHEADER_GC_END;
		pack.size = sizeof(TPacketGCShop);
		ch->GetDesc()->Packet(&pack, sizeof(pack));

		if (!isDestroy)
		{
			m_map_guest.erase(ch->GetPlayerID());
			--m_dwRealWatcherCount;
			TEMP_BUFFER buf;
			pack.subheader = SHOP_SUBHEADER_GC_REALWATCHER_COUNT;
			pack.size = sizeof(pack) + sizeof(DWORD);
			buf.write(&pack, sizeof(pack));
			buf.write(&m_dwRealWatcherCount, sizeof(DWORD));
			Broadcast(buf.read_peek(), buf.size());
		}
		sys_log(0, "COfflineShop::RemoveGuest: shop: %s customer: %s", m_data.owner_name, ch->GetName());
	}
}

void COfflineShop::BroadcastUpdateItem(BYTE bPos, bool bDestroy,int log_index)
{
	if (m_map_guest.size() == 0)
		return;

	TPacketGCShop pack;
	TPacketGCOfflineShopUpdateItem pack2;
	TEMP_BUFFER buf;
	pack.header = HEADER_GC_OFFLINE_SHOP;
	pack.subheader = SHOP_SUBHEADER_GC_UPDATE_ITEM;
	pack.size = sizeof(pack) + sizeof(pack2);
	pack2.pos = bPos;
	pack2.m_dwDisplayedCount = m_dwDisplayedCount;
	pack2.m_dwRealWatcherCount = m_dwRealWatcherCount;
	pack2.price = m_data.price;
	pack2.flag = m_data.slotflag;
	pack2.time = m_data.time;
	pack2.type = m_data.type;
	strlcpy(pack2.title, m_data.sign, sizeof(pack2.title));
	if (log_index != -1)
		thecore_memcpy(&pack2.log, &m_data.log[log_index], sizeof(pack2.log));
	else
		memset(&pack2.log, 0, sizeof(pack2.log));

	if (bDestroy)
		memset(&pack2.item, 0, sizeof(pack2.item));
	else
	{
		const OFFLINE_SHOP_ITEM& item = m_data.items[bPos];
		pack2.item.owner_id = item.owner_id;
		pack2.item.count = item.count;
		pack2.item.vnum = item.vnum;
		pack2.item.price = item.price;
		pack2.item.status = item.status;
		strlcpy(pack2.item.szBuyerName, item.szBuyerName, sizeof(pack2.item.szBuyerName));
		thecore_memcpy(pack2.item.alSockets, item.alSockets, sizeof(pack2.item.alSockets));
		thecore_memcpy(pack2.item.aAttr, item.aAttr, sizeof(pack2.item.aAttr));
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		pack2.item.transmutation = item.transmutation;
#endif
	}

	buf.write(&pack, sizeof(pack));
	buf.write(&pack2, sizeof(pack2));
	Broadcast(buf.read_peek(), buf.size());
}

void COfflineShop::Broadcast(const void * data, int bytes)
{
	for (auto i = m_map_guest.begin(); i != m_map_guest.end(); ++i)
	{
		LPCHARACTER ch = i->second;
		if (ch && ch->GetDesc())
			ch->GetDesc()->Packet(data, bytes);
	}
}

DWORD COfflineShop::GetItemCount()
{
	DWORD count = 0;
	for (DWORD i = 0; i < OFFLINE_SHOP_HOST_ITEM_MAX_NUM; ++i)
		if (m_data.items[i].vnum > 0 && m_data.items[i].status == 0)
			count += 1;
	return count;
}
#endif
