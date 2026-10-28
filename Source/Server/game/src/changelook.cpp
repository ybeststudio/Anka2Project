#include "stdafx.h"
#include "changelook.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "unique_item.h"
#include "desc.h"
#include "packet.h"

#ifdef ENABLE_CHANGE_LOOK_SYSTEM

CTransmutation::CTransmutation(const LPCHARACTER ch, const bool bType) :
	m_ch(ch),
	m_type(static_cast<decltype(m_type)>(bType)),
	m_Item({}),
	m_FreeItem(nullptr)
{
	m_ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowChangeDialog %d", bType);
}

CTransmutation::~CTransmutation()
{
}

void CTransmutation::Open(const LPCHARACTER ch, const bool bType)
{
	if (ch == nullptr)
		return;

	if (ch->GetTransmutation() || ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to close other windows."));
		return;
	}

	ch->SetTransmutation(new CTransmutation(ch, bType));
}

void CTransmutation::ItemCheckIn(const TItemPos& pos, const size_t bSlotType)
{
	if (bSlotType >= m_Item.size())
	{
		sys_err("Unknown SlotType m_ch:%s, %d", m_ch->GetName(), bSlotType);
		return;
	}

	LPITEM& item = m_Item.at(bSlotType);
	if (item != nullptr)
		return;

	if (pos.IsDefaultInventoryPosition() == false)
		return;

	const LPITEM inven_item = m_ch->GetItem(pos);
	if (inven_item == nullptr)
		return;

	if (inven_item->isLocked())
		return;

	switch (static_cast<ETRANSMUTATIONSLOTTYPE>(bSlotType))
	{
		case ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_LEFT:
			if (CanAddItem(inven_item) == false)
				return;
			break;

		case ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_RIGHT:
			if (CheckOtherItem(inven_item) == false)
				return;
			break;

		default:
			return;
	}

	TPacketGCTransmutationItemSet pack = 
	{
		HEADER_GC_TRANSMUTATION_SET,
		pos.cell,
		static_cast<BYTE>(bSlotType),
	};

	m_ch->GetDesc()->Packet(&pack, sizeof(pack));

	item = inven_item;
}

void CTransmutation::ItemCheckOut(const size_t bSlotType)
{
	if (bSlotType >= m_Item.size())
	{
		sys_err("Unknown SlotType (m_ch:%s), %d", m_ch->GetName(), bSlotType);
		return;
	}

	LPITEM& item = m_Item.at(bSlotType);
	if (item == nullptr)
		return;

	packet_transmutation_del pack =
	{
		HEADER_GC_TRANSMUTATION_DEL,
		item->GetCell(),
		static_cast<BYTE>(bSlotType)
	};

	m_ch->GetDesc()->Packet(&pack, sizeof(pack));

	item = nullptr;
}

void CTransmutation::FreeItemCheckIn(const TItemPos& pos)
{
	if (m_FreeItem != nullptr)
		return;

	if (pos.IsDefaultInventoryPosition() == false)
		return;

	const LPITEM inven_item = m_ch->GetItem(pos);
	if (inven_item == nullptr)
		return;

	if (inven_item->isLocked())
		return;

	if (CanAddFreeItem(inven_item) == false)
		return;
	
	TPacketGCTransmutationItemSet pack =
	{
		HEADER_GC_TRANSMUTATION_FREE_SET,
		pos.cell,
		0,
	};

	m_ch->GetDesc()->Packet(&pack, sizeof(pack));

	m_FreeItem = inven_item;
}

void CTransmutation::FreeItemCheckOut()
{
	if (m_FreeItem == nullptr)
		return;

	packet_transmutation_del pack =
	{
		HEADER_GC_TRANSMUTATION_FREE_DEL,
		m_FreeItem->GetCell(),
		0
	};

	m_ch->GetDesc()->Packet(&pack, sizeof(pack));

	m_FreeItem = nullptr;
}

void CTransmutation::Accept()
{
	const LPITEM left = GetLeftItem();
	const LPITEM right = GetRightItem();
	const LPITEM free = GetFreeItem();

	if (left == nullptr || right == nullptr)
		return;

#ifdef ENABLE_GOLD_LIMIT
	const long long iPrice = static_cast<long long>(GetPrice());
#else
	const int iPrice = static_cast<int>(GetPrice());
#endif

	if (m_ch->GetGold() < iPrice)
	{
		m_ch->LocaleChatPacket(CHAT_TYPE_INFO, 785, "");
		return;
	}

	if (iPrice)
		m_ch->PointChange(POINT_GOLD, -iPrice);

	left->SetTransmutationVnum(right->GetVnum());
	left->UpdatePacket();

	for (size_t i = 0; i < m_Item.size(); i++)
		ItemCheckOut(i);

	FreeItemCheckOut();

	ITEM_MANAGER::instance().RemoveItem(right, "REMOVE (Transmutation - Item)");
	if (free != nullptr)
		ITEM_MANAGER::instance().RemoveItem(free, "REMOVE (Transmutation - Free Item)");
}

bool CTransmutation::CanAddItem(const LPITEM item) const
{
	const BYTE bItemType = item->GetType();
	const BYTE bItemSubType = item->GetSubType();

	switch (m_type)
	{
		case ETRANSMUTATIONTYPE::TRANSMUTATION_TYPE_MOUNT:
			break;

		case ETRANSMUTATIONTYPE::TRANSMUTATION_TYPE_ITEM:
			if (bItemType == EItemTypes::ITEM_WEAPON && bItemSubType != EWeaponSubTypes::WEAPON_ARROW)
				return true;

			if (bItemType == EItemTypes::ITEM_ARMOR && bItemSubType == EArmorSubTypes::ARMOR_BODY)
				return true;

			if (bItemType == EItemTypes::ITEM_COSTUME && bItemSubType == ECostumeSubTypes::COSTUME_BODY)
				return true;
			break;
	}

	return false;
}

bool CTransmutation::CheckOtherItem(const LPITEM item) const
{
	const auto other_item = GetLeftItem();
	if (other_item == nullptr)
		return false;

	if (other_item == item)
		return false;

	if (other_item->GetVnum() == item->GetVnum())
		return false;

	if (other_item->GetType() != item->GetType())
		return false;

	if (other_item->GetSubType() != item->GetSubType())
		return false;

	if (other_item->GetAntiFlag() != item->GetAntiFlag())
		return false;

	return true;
}

bool CTransmutation::CanAddFreeItem(const LPITEM item) const
{
	const DWORD dwVnum = item->GetVnum();
	return dwVnum == TRANSMUTATION_TICKET_1 || dwVnum == TRANSMUTATION_TICKET_2;
}

bool CTransmutation::IsTypeItem() const
{
	return m_type == ETRANSMUTATIONTYPE::TRANSMUTATION_TYPE_ITEM;
}

bool CTransmutation::IsTypeMount() const
{
	return m_type == ETRANSMUTATIONTYPE::TRANSMUTATION_TYPE_MOUNT;
}

LPITEM CTransmutation::GetLeftItem() const
{
	return m_Item.at(static_cast<size_t>(ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_LEFT));
}

LPITEM CTransmutation::GetRightItem() const
{
	return m_Item.at(static_cast<size_t>(ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_RIGHT));
}

LPITEM CTransmutation::GetFreeItem() const
{
	return m_FreeItem;
}

DWORD CTransmutation::GetPrice() const
{
	if (GetFreeItem() == nullptr)
	{
		if (IsTypeItem())
			return static_cast<DWORD>(ETRANSMUTATIONSETTINGS::TRANSMUTATION_ITEM_PRICE);

		if (IsTypeMount())
			return static_cast<DWORD>(ETRANSMUTATIONSETTINGS::TRANSMUTATION_MOUNT_PRICE);
	}
	return 0;
}
#endif
