#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "desc.h"
#include "sectree_manager.h"
#include "packet.h"
#include "protocol.h"
#include "log.h"
#include "skill.h"
#include "unique_item.h"
#include "profiler.h"
#include "marriage.h"
#include "item_addon.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"
#include "affect.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "../../common/VnumHelper.h"
#include "../../common/CommonDefines.h"
#include "mob_manager.h"
#ifdef __PREMIUM_PRIVATE_SHOP__
#include "private_shop_manager.h"
#include "private_shop.h"
#endif

#ifdef __ENABLE_ITEM_TOGGLE__
#include "ItemToggle.hpp"
#endif

const uint32_t PICKUP_DISTANCE = 3 * 300;

CItem::CItem(DWORD dwVnum)
	: m_dwVnum(dwVnum), m_bWindow(0), m_dwID(0), m_bEquipped(false), m_dwVID(0), m_wCell(0), m_dwCount(0), m_lFlag(0), m_dwLastOwnerPID(0),
	m_bExchanging(false), m_pkDestroyEvent(NULL), m_pkExpireEvent(NULL), m_pkUniqueExpireEvent(NULL),
	m_pkTimerBasedOnWearExpireEvent(NULL), m_pkRealTimeExpireEvent(NULL),
#ifdef ENABLE_ITEMSHOP_TIMED_BONUSES
	m_pkRealItemShopBonusTimeExpireEvent(NULL),
#endif
   	m_pkAccessorySocketExpireEvent(NULL), m_pkOwnershipEvent(NULL), m_dwOwnershipPID(0), m_bSkipSave(false), m_isLocked(false),
	m_dwMaskVnum(0), m_dwSIGVnum (0)
#ifdef ENABLE_NEW_COSTUME_BONUS
	, m_costumeTimeoutEvent(NULL)
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	, m_pPrivateShop(nullptr), m_llGoldPrice(0), m_dwChequePrice(0), m_tPrivateShopCheckin(0)
#endif
{
	memset( &m_alSockets, 0, sizeof(m_alSockets) );
#if defined(__ITEM_APPLY_RANDOM__)
	memset( &m_aApplyRandom, 0, sizeof(m_aApplyRandom));
#endif
	memset( &m_aAttr, 0, sizeof(m_aAttr) );
#if defined(ENABLE_SWITCHBOT)
	m_bSkipUpdatePacket = false;
#endif
}

CItem::~CItem()
{
	Destroy();
	ITEM_MANAGER::instance().RemoveFromDelayedSave(this); // @fixme323
}

void CItem::Initialize()
{
	CEntity::Initialize(ENTITY_ITEM);
#if defined(ENABLE_SWITCHBOT)
	m_bSkipUpdatePacket = false;
#endif

	m_bWindow = RESERVED_WINDOW;
	m_pOwner = NULL;
	m_dwID = 0;
	m_bEquipped = false;
	m_dwVID = m_wCell = m_dwCount = m_lFlag = 0;
	m_pProto = NULL;
	m_bExchanging = false;
	memset(&m_alSockets, 0, sizeof(m_alSockets));
#if defined(__ITEM_APPLY_RANDOM__)
	memset(&m_aApplyRandom, 0, sizeof(m_aApplyRandom));
#endif

	memset(&m_aAttr, 0, sizeof(m_aAttr));

	m_bUpdateStatus = false;

	m_pkDestroyEvent = NULL;
	m_pkOwnershipEvent = NULL;
	m_dwOwnershipPID = 0;
	m_pkUniqueExpireEvent = NULL;
	m_pkTimerBasedOnWearExpireEvent = NULL;
	m_pkRealTimeExpireEvent = NULL;
#ifdef ENABLE_ITEMSHOP_TIMED_BONUSES
	m_pkRealItemShopBonusTimeExpireEvent = NULL;
#endif
	m_pkAccessorySocketExpireEvent = NULL;

	m_bSkipSave = false;
	m_dwLastOwnerPID = 0;

	m_dwLastOwnerPID = 0;
#ifdef __PREMIUM_PRIVATE_SHOP__
	m_pPrivateShop = nullptr;
	m_llGoldPrice = 0;
	m_dwChequePrice = 0;
	m_tPrivateShopCheckin = 0;
#endif
}

void CItem::Destroy()
{
	event_cancel(&m_pkDestroyEvent);
	event_cancel(&m_pkOwnershipEvent);
	event_cancel(&m_pkUniqueExpireEvent);
	event_cancel(&m_pkTimerBasedOnWearExpireEvent);
	event_cancel(&m_pkRealTimeExpireEvent);
#ifdef ENABLE_ITEMSHOP_TIMED_BONUSES
	event_cancel(&m_pkRealItemShopBonusTimeExpireEvent);
#endif
	event_cancel(&m_pkAccessorySocketExpireEvent);
#ifdef ENABLE_NEW_COSTUME_BONUS
	event_cancel(&m_costumeTimeoutEvent);
#endif

	CEntity::Destroy();

	if (GetSectree())
		GetSectree()->RemoveEntity(this);
}

EVENTFUNC(item_destroy_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "item_destroy_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	if (pkItem->GetOwner())
		sys_err("item_destroy_event: Owner exist. (item %s owner %s)", pkItem->GetName(), pkItem->GetOwner()->GetName());

	pkItem->SetDestroyEvent(NULL);
	M2_DESTROY_ITEM(pkItem);
	return 0;
}
#ifdef ENABLE_NEW_COSTUME_BONUS
EVENTFUNC(costume_bonus_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("costume_bonus_event <Factor> Null pointer");
		return 0;
	}
	
	LPITEM pkItem = info->item;
	
	if (get_global_time() < pkItem->GetSocket(NEW_COSTUME_BONUS_SOCKET))
		return 0;
	if (pkItem->IsEquipped())
		pkItem->ModifyCostumePoints(false);
	
	pkItem->SetSocket(NEW_COSTUME_BONUS_SOCKET, 0);
	pkItem->SetSocket(NEW_COSTUME_BONUS_SOCKET_VAL, 0);
	
	return 0;
}
#endif
void CItem::SetDestroyEvent(LPEVENT pkEvent)
{
	m_pkDestroyEvent = pkEvent;
}

void CItem::StartDestroyEvent(int iSec)
{
	if (m_pkDestroyEvent)
		return;

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetDestroyEvent(event_create(item_destroy_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::EncodeInsertPacket(LPENTITY ent)
{
	LPDESC d;

	if (!(d = ent->GetDesc()))
		return;

	const PIXEL_POSITION & c_pos = GetXYZ();

	struct packet_item_ground_add pack;

	pack.bHeader	= HEADER_GC_ITEM_GROUND_ADD;
	pack.x		= c_pos.x;
	pack.y		= c_pos.y;
	pack.z		= c_pos.z;
	pack.dwVnum		= GetVnum();
	pack.dwVID		= m_dwVID;
	pack.count		= m_dwCount;

#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
	for (size_t i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		pack.alSockets[i] = GetSocket(i);

	thecore_memcpy(pack.aAttrs, GetAttributes(), sizeof(pack.aAttrs));
#endif

	d->Packet(&pack, sizeof(pack));

	if (m_pkOwnershipEvent != NULL)
	{
		item_event_info * info = dynamic_cast<item_event_info *>(m_pkOwnershipEvent->info);

		if ( info == NULL )
		{
			sys_err( "CItem::EncodeInsertPacket> <Factor> Null pointer" );
			return;
		}

		TPacketGCItemOwnership p;

		p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
		p.dwVID = m_dwVID;
		strlcpy(p.szName, info->szOwnerName, sizeof(p.szName));

		d->Packet(&p, sizeof(TPacketGCItemOwnership));
	}
}

void CItem::EncodeRemovePacket(LPENTITY ent)
{
	LPDESC d;

	if (!(d = ent->GetDesc()))
		return;

	struct packet_item_ground_del pack;

	pack.bHeader	= HEADER_GC_ITEM_GROUND_DEL;
	pack.dwVID		= m_dwVID;

	d->Packet(&pack, sizeof(pack));
	sys_log(2, "Item::EncodeRemovePacket %s to %s", GetName(), ((LPCHARACTER) ent)->GetName());
}

void CItem::SetProto(const TItemTable * table)
{
	assert(table != NULL);
	m_pProto = table;
	SetFlag(m_pProto->dwFlags);
}

void CItem::UsePacketEncode(LPCHARACTER ch, LPCHARACTER victim, struct packet_item_use *packet)
{
	if (!GetVnum())
		return;

	packet->header 	= HEADER_GC_ITEM_USE;
	packet->ch_vid 	= ch->GetVID();
	packet->victim_vid 	= victim->GetVID();
	packet->Cell = TItemPos(GetWindow(), m_wCell);
	packet->vnum	= GetVnum();
}

void CItem::RemoveFlag(int32_t bit)
{
	REMOVE_BIT(m_lFlag, bit);
}

void CItem::AddFlag(int32_t bit)
{
	SET_BIT(m_lFlag, bit);
}

void CItem::UpdatePacket()
{
#ifdef ENABLE_SWITCHBOT
	if (m_bSkipUpdatePacket)
		return;
#endif
	if (!m_pOwner || !m_pOwner->GetDesc())
		return;
	
#ifdef ENABLE_SWITCHBOT
	if (GetUpdateStatus())
		return;
#endif
#ifdef __INVENTORY_BUFFERING__
	if (m_pOwner->IsInvBuffOn())
	{
		// Set it to be processed later
		m_pOwner->AddItemToInvBuff(this);
		return;
	}
#endif

	TPacketGCItemUpdate pack;

	pack.header = HEADER_GC_ITEM_UPDATE;
	pack.Cell = TItemPos(GetWindow(), m_wCell);
	pack.count	= m_dwCount;

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		pack.alSockets[i] = m_alSockets[i];

#if defined(__ITEM_APPLY_RANDOM__)
	thecore_memcpy(pack.aApplyRandom, GetRandomApplies(), sizeof(pack.aApplyRandom));
#endif
	thecore_memcpy(pack.aAttr, GetAttributes(), sizeof(pack.aAttr));

	sys_log(2, "UpdatePacket %s -> %s", GetName(), m_pOwner->GetName());
	m_pOwner->GetDesc()->Packet(&pack, sizeof(pack));
}

DWORD CItem::GetCount()
{
#ifdef ENABLE_CHEQUE_SYSTEM
	if (GetType() == ITEM_ELK || GetType() == ITEM_CHEQUE) return MIN(m_dwCount, INT_MAX);
#else
	if (GetType() == ITEM_ELK) return MIN(m_dwCount, INT_MAX);
#endif
	else
	{
		return MIN(m_dwCount, g_bItemCountLimit);
	}
}

bool CItem::SetCount(DWORD count)
{
#ifdef ENABLE_CHEQUE_SYSTEM
	if (GetType() == ITEM_ELK || GetType() == ITEM_CHEQUE)
#else
	if (GetType() == ITEM_ELK)
#endif
	{
		m_dwCount = MIN(count, INT_MAX);
	}
	else
	{
		m_dwCount = MIN(count, g_bItemCountLimit);
	}

	if (count == 0 && m_pOwner)
	{
		if (GetSubType() == USE_ABILITY_UP || GetSubType() == USE_POTION || GetVnum() == 70020)
		{
			LPCHARACTER pOwner = GetOwner();
			WORD wCell = GetCell();

			RemoveFromCharacter();

			if (!IsDragonSoul())
			{
				LPITEM pItem = pOwner->FindSpecifyItem(GetVnum());

				if (NULL != pItem)
				{
					pOwner->ChainQuickslotItem(pItem, QUICKSLOT_TYPE_ITEM, wCell);
				}
				else
				{
					pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, wCell, 255);
				}
			}
			M2_DESTROY_ITEM(this);
		}
		else
		{
			if (!IsDragonSoul())
			{
				m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, m_wCell, 255);
			}
			
			M2_DESTROY_ITEM(RemoveFromCharacter());
		}

		return false;
	}

	UpdatePacket();

	Save();
	return true;
}

LPITEM CItem::RemoveFromCharacter()
{
	if (!m_pOwner)
	{
		sys_err("Item::RemoveFromCharacter owner null");
		return (this);
	}

	if (!GetProto())
	{
		sys_err("Item::RemoveFromCharacter: item id=%u has null proto (corrupt?), owner=%s cell=%d",
			GetID(), m_pOwner->GetName(), m_wCell);
		m_pOwner = NULL;
		return (this);
	}

	OnRemoveFromCharacter();

	LPCHARACTER pOwner = m_pOwner;
#ifdef __INVENTORY_BUFFERING__
	if (m_pOwner->IsInvBuffOn())
		// Item is removed - no longer need to buffer it
		m_pOwner->RemoveItemFromInvBuff(this);
#endif

	if (m_bEquipped)
	{
		Unequip();

		SetWindow(RESERVED_WINDOW);
		Save();
		return (this);
	}
	else
	{
		if (GetWindow() != SAFEBOX && GetWindow() != MALL)
		{
			if (IsDragonSoul())
			{
				if (m_wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
					sys_err("CItem::RemoveFromCharacter: pos >= DRAGON_SOUL_INVENTORY_MAX_NUM");
				else
					pOwner->SetItem(TItemPos(m_bWindow, m_wCell), NULL);
			}
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
			else if (m_bWindow == BUFF_EQUIPMENT)
			{
				if (m_wCell >= BUFF_WINDOW_SLOT_MAX_NUM)
				{
					sys_err("CItem::RemoveFromCharacter: pos >= BUFF_WINDOW_SLOT_MAX_NUM");
				}
				else
				{
					pOwner->SetItem(TItemPos(BUFF_EQUIPMENT, m_wCell), NULL);
				}
			}
#endif
#ifdef ENABLE_SWITCHBOT
			else if (m_bWindow == SWITCHBOT)
			{
				if (m_wCell >= SWITCHBOT_SLOT_COUNT)
				{
					sys_err("CItem::RemoveFromCharacter: pos >= SWITCHBOT_SLOT_COUNT");
				}
				else
				{
					pOwner->SetItem(TItemPos(SWITCHBOT, m_wCell), NULL);
				}
			}
#endif
			else
			{
				TItemPos cell(INVENTORY, m_wCell);

				if (false == cell.IsDefaultInventoryPosition() && false == cell.IsBeltInventoryPosition()
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
					&& false == cell.IsSkillBookInventoryPosition() && false == cell.IsUpgradeItemsInventoryPosition()
					&& false == cell.IsStoneInventoryPosition() && false == cell.IsBoxInventoryPosition()
					&& false == cell.IsEfsunInventoryPosition() && false == cell.IsCicekInventoryPosition()
#endif
					)
					sys_err("CItem::RemoveFromCharacter: Invalid Item Position");
				else
				{
					pOwner->SetItem(cell, NULL);
				}
			}
		}

		m_pOwner = NULL;
		m_wCell = 0;

		SetWindow(RESERVED_WINDOW);
		Save();
		return (this);
	}
}

bool CItem::AddToCharacter(LPCHARACTER ch, TItemPos Cell)
{
	assert(GetSectree() == NULL);
	assert(m_pOwner == NULL);

	WORD pos = Cell.cell;
	BYTE window_type = Cell.window_type;

	if (INVENTORY == window_type)
	{
		if (m_wCell >= INVENTORY_MAX_NUM && BELT_INVENTORY_SLOT_START > m_wCell)
		{
			sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}
	else if (DRAGON_SOUL_INVENTORY == window_type)
	{
		if (m_wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}
#ifdef ENABLE_SWITCHBOT
	else if (SWITCHBOT == window_type)
	{
		if (m_wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CItem::AddToCharacter:switchbot cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}
#endif
#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	else if (BUFF_EQUIPMENT == window_type)
	{
		if (m_wCell >= BUFF_WINDOW_SLOT_MAX_NUM)
		{
			sys_err("CItem::AddToCharacter: BuffEquipment cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}
#endif
#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
	bool bWereMine = (GetLastOwnerPID() == ch->GetPlayerID()) || (GetWindow() == EQUIPMENT);
#endif

	if (ch->GetDesc())
		m_dwLastOwnerPID = ch->GetPlayerID();

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if ((GetType() == ITEM_COSTUME) && (GetSubType() == COSTUME_ACCE) && (GetSocket(ACCE_ABSORPTION_SOCKET) == 0))
	{
		int32_t lVal = GetValue(ACCE_GRADE_VALUE_FIELD);
		switch (lVal)
		{
		case 2:
			lVal = ACCE_GRADE_2_ABS;
			break;
		case 3:
			lVal = ACCE_GRADE_3_ABS;
			break;
		case 4:
			lVal = number(ACCE_GRADE_4_ABS_MIN, ACCE_GRADE_4_ABS_MAX_COMB);
			break;
		default:
			lVal = ACCE_GRADE_1_ABS;
			break;
		}
		SetSocket(ACCE_ABSORPTION_SOCKET, lVal);
	}
#endif

#ifdef ENABLE_AURA_SYSTEM
	if ((GetType() == ITEM_COSTUME && GetSubType() == COSTUME_AURA) && (GetSocket(AURA_ABSORPTION_SOCKET) == 0))
	{
		int32_t lVal = GetValue(AURA_GRADE_VALUE_FIELD);
		if (lVal < 1)
		{
			SetSocket(AURA_GRADE_VALUE_FIELD, 1);
			SetSocket(AURA_ABSORPTION_SOCKET, 1);
		}
		else
			SetSocket(AURA_ABSORPTION_SOCKET, lVal);
	}
#endif

	event_cancel(&m_pkDestroyEvent);

#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
	ch->SetItem(TItemPos(window_type, pos), this, bWereMine);
#else
	ch->SetItem(TItemPos(window_type, pos), this);
#endif
	m_pOwner = ch;

	Save();

	OnAddToCharacter();

	return true;
}

LPITEM CItem::RemoveFromGround()
{
	if (GetSectree())
	{
		SetOwnership(NULL);

		GetSectree()->RemoveEntity(this);

		ViewCleanup();

		Save();
	}

	return (this);
}

bool CItem::AddToGround(int32_t lMapIndex, const PIXEL_POSITION & pos, bool skipOwnerCheck)
{
	if (0 == lMapIndex)
	{
		sys_err("wrong map index argument: %d", lMapIndex);
		return false;
	}

	if (GetSectree())
	{
		sys_err("sectree already assigned");
		return false;
	}

	if (!skipOwnerCheck && m_pOwner)
	{
		sys_err("owner pointer not null");
		return false;
	}

	LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, pos.x, pos.y);

	if (!tree)
	{
		sys_err("cannot find sectree by %dx%d", pos.x, pos.y);
		return false;
	}

	SetWindow(GROUND);
	SetXYZ(pos.x, pos.y, pos.z);
	tree->InsertEntity(this);
	UpdateSectree();
	Save();
	return true;
}

bool CItem::DistanceValid(LPCHARACTER ch)
{
	if (!GetSectree())
		return false;

	int iDist = DISTANCE_SQRT(GetX() - ch->GetX(), GetY() - ch->GetY());

	if (iDist > PICKUP_DISTANCE) // @fixme173 300 to 600
		return false;

	return true;
}

bool CItem::CanUsedBy(LPCHARACTER ch)
{
	// Anti flag check
	switch (ch->GetJob())
	{
		case JOB_WARRIOR:
			if (GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
				return false;
			break;

		case JOB_ASSASSIN:
			if (GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
				return false;
			break;

		case JOB_SHAMAN:
			if (GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
				return false;
			break;

		case JOB_SURA:
			if (GetAntiFlag() & ITEM_ANTIFLAG_SURA)
				return false;
			break;
#ifdef ENABLE_WOLFMAN_CHARACTER
		case JOB_WOLFMAN:
			if (GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
				return false;
			break;
#endif
	}

	return true;
}

int CItem::FindEquipCell(LPCHARACTER ch, int iCandidateCell)
{
#ifdef ENABLE_NEW_PET_SYSTEM
	if (GetType() == ITEM_NEW_PET)
	{
		return WEAR_NEW_PET;
	}
	else if (GetType() == ITEM_NEW_PET_EQ)
	{
		if (!ch->GetWear(WEAR_NEW_PET_EQ0))
		{
			return WEAR_NEW_PET_EQ0;
		}
		else if (!ch->GetWear(WEAR_NEW_PET_EQ1))
		{
			return WEAR_NEW_PET_EQ1;
		}
		else if (!ch->GetWear(WEAR_NEW_PET_EQ2))
		{
			return WEAR_NEW_PET_EQ2;
		}
		else if (!ch->GetWear(WEAR_NEW_PET_EQ3))
		{
			return WEAR_NEW_PET_EQ3;
		}
		else if (!ch->GetWear(WEAR_NEW_PET_EQ4))
		{
			return WEAR_NEW_PET_EQ4;
		}
		else
		{
			return -1;
		}
	}
#endif
	if ((0 == GetWearFlag() || ITEM_TOTEM == GetType()) && ITEM_COSTUME != GetType() && ITEM_DS != GetType() && ITEM_SPECIAL_DS != GetType() && ITEM_RING != GetType() && ITEM_BELT != GetType() && ITEM_RUNE != GetType() && ITEM_RUNE_RED != GetType() && ITEM_RUNE_BLUE != GetType() && ITEM_RUNE_GREEN != GetType() && ITEM_RUNE_YELLOW != GetType() && ITEM_RUNE_BLACK != GetType()
		
#ifdef ENABLE_ARTEFAKT_SYSTEM
			&& ITEM_ARTEFAKT != GetType()
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			&& ITEM_MOUNT_EQUIPMENT != GetType()
#endif
#ifdef ENABLE_MOUNT_SYSTEM
			&& ITEM_MOUNT != GetType()
#endif
			&& ITEM_SKILL_COSTUME != GetType()
	)
		return -1;
	if (GetType() == ITEM_DS || GetType() == ITEM_SPECIAL_DS)
	{
		if (iCandidateCell < 0)
		{
			return WEAR_MAX_NUM + GetSubType();
		}
		else
		{
			for (int i = 0; i < DRAGON_SOUL_DECK_MAX_NUM; i++)
			{
				if (WEAR_MAX_NUM + i * DS_SLOT_MAX + GetSubType() == iCandidateCell)
				{
					return iCandidateCell;
				}
			}
			return -1;
		}
	}
#ifdef __PENDANT__
	else if (GetType() == ITEM_ARMOR && GetSubType() >= ARMOR_PENDANT_FIRE && GetSubType() <= ARMOR_PENDANT_SOUL)
	{
		switch (GetSubType())
		{
		case ARMOR_PENDANT_FIRE:
			return WEAR_PENDANT_FIRE;
		case ARMOR_PENDANT_ICE:
			return WEAR_PENDANT_ICE;
		case ARMOR_PENDANT_THUNDER:
			return WEAR_PENDANT_THUNDER;
		case ARMOR_PENDANT_WIND:
			return WEAR_PENDANT_WIND;
		case ARMOR_PENDANT_DARK:
			return WEAR_PENDANT_DARK;
		case ARMOR_PENDANT_EARTH:
			return WEAR_PENDANT_EARTH;
		case ARMOR_PENDANT_SOUL:
			return WEAR_PENDANT_SOUL_FIRST;
		}
		return -1;
	}
#endif
	else if (GetType() == ITEM_COSTUME)
	{
		if (GetSubType() == COSTUME_BODY)
			return WEAR_COSTUME_BODY;
		else if (GetSubType() == COSTUME_HAIR)
			return WEAR_COSTUME_HAIR;
#ifdef ENABLE_COSTUME_EMBLEMAT
		else if (GetSubType() == COSTUME_EMBLEMAT)
		{
			return WEAR_COSTUME_EMBLEMAT;
		}
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
		else if (GetSubType() == COSTUME_MOUNT)
			return WEAR_COSTUME_MOUNT;
#endif
#ifdef ENABLE_NEW_MOUNT_SYSTEM
		else if (GetSubType() == COSTUME_MOUNT)
			return WEAR_COSTUME_MOUNT;
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		else if (GetSubType() == COSTUME_ACCE)
		{
			//if (ch->GetWear(WEAR_COSTUME_STOLE))
			//{
			//	LPITEM stoleCostume = ch->GetWear(WEAR_COSTUME_STOLE);
			//	ch->UnequipItem(stoleCostume);
			//}
			//else {
			return WEAR_COSTUME_ACCE;
			//}
		}
#endif
#ifdef ENABLE_STOLE_COSTUME
		else if (GetSubType() == COSTUME_STOLE)
		{
			//if (!ch->GetWear(WEAR_COSTUME_ACCE))
			//	ch->ChatPacket(CHAT_TYPE_INFO, "[LS;2340]");
			//else
			return WEAR_COSTUME_STOLE;
		}
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		else if (GetSubType() == COSTUME_WEAPON)
			return WEAR_COSTUME_WEAPON;
#endif
#ifdef ENABLE_AURA_SYSTEM
		else if (GetSubType() == COSTUME_AURA)
			return WEAR_COSTUME_AURA;
#endif
	}
	else if (GetType() == ITEM_RING)
	{
		if (ch->GetWear(WEAR_RING1))
			if (GetValue(0) == 1) {
				return WEAR_UNIQUE1;
			} else {
				return WEAR_RING2;
			}
		else
			if (GetValue(0) == 2) {
				return WEAR_UNIQUE2;
			} else {
				return WEAR_RING1;
			}
	}
	else if (GetType() == ITEM_BELT)
		return WEAR_BELT;
#ifdef ENABLE_ARTEFAKT_SYSTEM

	else if (GetType() == ITEM_ARTEFAKT)
	{
		if (GetSubType() == ARTEFAKT_1)
			return WEAR_ARTEFAKT1;
		else if (GetSubType() == ARTEFAKT_2)
			return WEAR_ARTEFAKT2;
		else if (GetSubType() == ARTEFAKT_3)
			return WEAR_ARTEFAKT3;
		else if (GetSubType() == ARTEFAKT_4)
			return WEAR_ARTEFAKT4;
		else if (GetSubType() == ARTEFAKT_5)
			return WEAR_ARTEFAKT5;
		else if (GetSubType() == ARTEFAKT_6)
			return WEAR_ARTEFAKT6;
		else if (GetSubType() == ARTEFAKT_7)
			return WEAR_ARTEFAKT7;
		else if (GetSubType() == ARTEFAKT_8)
			return WEAR_ARTEFAKT8;
		else if (GetSubType() == ARTEFAKT_9)
			return WEAR_ARTEFAKT9;
		else if (GetSubType() == ARTEFAKT_10)
			return WEAR_ARTEFAKT10;
		else if (GetSubType() == ARTEFAKT_11)
			return WEAR_ARTEFAKT11;
		else if (GetSubType() == ARTEFAKT_12)
			return WEAR_ARTEFAKT12;
		else if (GetSubType() == ARTEFAKT_13)
			return WEAR_ARTEFAKT13;
		else if (GetSubType() == ARTEFAKT_14)
			return WEAR_ARTEFAKT14;

	}
#endif
#ifdef __SKILL_COSTUME__
	else if (GetType() == ITEM_SKILL_COSTUME)
	{
		if (GetSubType() == SKILL_COSTUME_1) 
			return WEAR_SKILL_COSTUME_1;
		else if (GetSubType() == SKILL_COSTUME_2) 
			return WEAR_SKILL_COSTUME_2;
		else if (GetSubType() == SKILL_COSTUME_3) 
			return WEAR_SKILL_COSTUME_3;
		else if (GetSubType() == SKILL_COSTUME_4) 
			return WEAR_SKILL_COSTUME_4;
		else if (GetSubType() == SKILL_COSTUME_5) 
			return WEAR_SKILL_COSTUME_5;
		else if (GetSubType() == SKILL_COSTUME_6) 
			return WEAR_SKILL_COSTUME_6;
	}
#endif
	else if (GetWearFlag() & WEARABLE_BODY)
		return WEAR_BODY;
	else if (GetWearFlag() & WEARABLE_HEAD)
		return WEAR_HEAD;
	else if (GetWearFlag() & WEARABLE_FOOTS)
		return WEAR_FOOTS;
	else if (GetWearFlag() & WEARABLE_WRIST)
		return WEAR_WRIST;
	else if (GetWearFlag() & WEARABLE_WEAPON)
		return WEAR_WEAPON;
	else if (GetWearFlag() & WEARABLE_SHIELD)
		return WEAR_SHIELD;
	else if (GetWearFlag() & WEARABLE_NECK)
		return WEAR_NECK;
	else if (GetWearFlag() & WEARABLE_EAR)
		return WEAR_EAR;
	else if (GetWearFlag() & WEARABLE_ARROW)
		return WEAR_ARROW;
#ifdef ENABLE_GLOVE_SYSTEM
	else if (GetType() == ITEM_ARMOR && GetSubType() == ARMOR_GLOVE)
		return WEAR_GLOVE;
#endif
#ifdef ENABLE_SYSTEM_RUNE
	else if (GetType() == ITEM_RUNE)
		return WEAR_RUNE;
	else if (GetType() == ITEM_RUNE_RED)
		return WEAR_RUNE_RED;
	else if (GetType() == ITEM_RUNE_BLUE)
		return WEAR_RUNE_BLUE;
	else if (GetType() == ITEM_RUNE_GREEN)
		return WEAR_RUNE_GREEN;
	else if (GetType() == ITEM_RUNE_YELLOW)
		return WEAR_RUNE_YELLOW;
	else if (GetType() == ITEM_RUNE_BLACK)
		return WEAR_RUNE_BLACK;
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	else if (GetType() == ITEM_MOUNT_EQUIPMENT)
		return WEAR_MOUNT_EQUIPMENT;
#endif
#ifdef ENABLE_NEW_MOUNT_SYSTEM
	else if (GetType() == COSTUME_MOUNT)
		return WEAR_COSTUME_MOUNT;
#endif
	else if (GetWearFlag() & WEARABLE_UNIQUE)
	{
		const LPITEM pUnique1 = ch->GetWear(WEAR_UNIQUE1);
		if (pUnique1)
		{
			// Prevent equipping two ITEM_UNIQUE items of the same subtype in both slots
			// (e.g. two premium/VIP duration items simultaneously)
			if (pUnique1->GetType() == GetType() && pUnique1->GetSubType() == GetSubType())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "[LS;10149]");
				return -1;
			}
			return WEAR_UNIQUE2;
		}
		return WEAR_UNIQUE1;
	}

	else if (GetWearFlag() & WEARABLE_ABILITY)
	{
		if (!ch->GetWear(WEAR_ABILITY1))
		{
			return WEAR_ABILITY1;
		}
		else if (!ch->GetWear(WEAR_ABILITY2))
		{
			return WEAR_ABILITY2;
		}
		else if (!ch->GetWear(WEAR_ABILITY3))
		{
			return WEAR_ABILITY3;
		}
		else if (!ch->GetWear(WEAR_ABILITY4))
		{
			return WEAR_ABILITY4;
		}
		else if (!ch->GetWear(WEAR_ABILITY5))
		{
			return WEAR_ABILITY5;
		}
		else if (!ch->GetWear(WEAR_ABILITY6))
		{
			return WEAR_ABILITY6;
		}
		else if (!ch->GetWear(WEAR_ABILITY7))
		{
			return WEAR_ABILITY7;
		}
		else if (!ch->GetWear(WEAR_ABILITY8))
		{
			return WEAR_ABILITY8;
		}
		else
		{
			return -1;
		}
	}
#ifdef ENABLE_MOUNT_SYSTEM
	else if (GetType() == ITEM_MOUNT)
	{
		if (GetSubType() == MOUNT_SHOE)
			return WEAR_MOUNT_EQUIPMENT_SHOE;
		else if (GetSubType() == MOUNT_SADDLE)
			return WEAR_MOUNT_EQUIPMENT_SADDLE;
		else if (GetSubType() == MOUNT_ARMOR)
			return WEAR_MOUNT_EQUIPMENT_ARMOR;
	}
#endif

	return -1;
}

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM 
int CItem::FindBuffEquipCell(LPCHARACTER ch, int iCandidateCell)
{
	if ((ITEM_TOTEM == GetType()) && ITEM_COSTUME != GetType() && ITEM_DS != GetType() && ITEM_SPECIAL_DS != GetType() && ITEM_RING != GetType() && ITEM_BELT != GetType())
		return -1;

	// WEARABLE_HAIR
	else if (GetType() == ITEM_COSTUME) {
		if (GetSubType() == BUFF_COSTUME_BODY)
			return BUFF_WEAR_BODY;
		else if (GetSubType() == BUFF_COSTUME_HAIR)
			return BUFF_WEAR_HEAD;
		else if (GetSubType() == BUFF_COSTUME_WEAPON)
			return BUFF_WEAR_WEAPON;
	}

	else if (GetVnum() == ASLAN_BUFF_EXP_RING_ITEM_1 || GetVnum() == ASLAN_BUFF_EXP_RING_ITEM_2 || GetVnum() == ASLAN_BUFF_EXP_RING_ITEM_3)
		return BUFF_WEAR_UNIQUE;
	
	return -1;
}
#endif

#ifdef ENABLE_NEW_COSTUME_BONUS
void CItem::SetCostume()
{
	event_cancel(&m_costumeTimeoutEvent);
	if (GetSocket(NEW_COSTUME_BONUS_SOCKET) != 0)
	{
		item_event_info* info = AllocEventInfo<item_event_info>();
		info->item = this;
		m_costumeTimeoutEvent = event_create(costume_bonus_event, info, PASSES_PER_SEC(GetSocket(NEW_COSTUME_BONUS_SOCKET) -  get_global_time()));
	}
}
void CItem::ModifyCostumePoints(bool bAdd)
{
	if (GetSocket(NEW_COSTUME_BONUS_SOCKET_VAL) == 0)
		return;

	if (!m_pOwner)
		return;

	const TItemTable* pUsedProto = ITEM_MANAGER::instance().GetTable(GetSocket(NEW_COSTUME_BONUS_SOCKET_VAL));
	if (!pUsedProto)
		return;

	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (pUsedProto->aApplies[i].bType != APPLY_NONE && pUsedProto->aApplies[i].lValue != 0)
		{
			m_pOwner->ApplyPoint(pUsedProto->aApplies[i].bType, pUsedProto->aApplies[i].lValue * (bAdd ? 1 : -1));
		}
	}
	
	pUsedProto = ITEM_MANAGER::instance().GetTable(GetSocket(NEW_COSTUME_BONUS_SOCKET_VAL2));
	
	if (!pUsedProto)
		return;
	
	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (pUsedProto->aApplies[i].bType != APPLY_NONE && pUsedProto->aApplies[i].lValue != 0)
		{
			m_pOwner->ApplyPoint(pUsedProto->aApplies[i].bType, pUsedProto->aApplies[i].lValue * (bAdd ? 1 : -1));
		}
	}
	
	pUsedProto = ITEM_MANAGER::instance().GetTable(GetSocket(NEW_COSTUME_BONUS_SOCKET_VAL3));
	
	if (!pUsedProto)
		return;
	
	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (pUsedProto->aApplies[i].bType != APPLY_NONE && pUsedProto->aApplies[i].lValue != 0)
		{
			m_pOwner->ApplyPoint(pUsedProto->aApplies[i].bType, pUsedProto->aApplies[i].lValue * (bAdd ? 1 : -1));
		}
	}
}
#endif

void CItem::ModifyPoints(bool bAdd)
{
#ifdef ENABLE_MOUNT_SYSTEM
    if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_MOUNT)
    {
        if (m_pOwner->IsHorseRiding() == false)
            return;
    }

    if (GetType() == ITEM_MOUNT)
    {
        if (m_pOwner->IsHorseRiding() == false && m_pOwner->GetHorse() == NULL)
            return;
    }
#endif

	if (!this->GetOwner())
		return;

	auto* proto = ITEM_MANAGER::instance().GetTable(m_dwVnum);
	if (!proto)
		return;

	int accessoryGrade;

//#ifdef __RENEWAL_MOUNT__
//	if (IsCostumeMountItem() && !m_pOwner->IsRiding())
//		return;
//#endif

	if (false == IsAccessoryForSocket())
	{
#ifdef ENABLE_QUIVER_SYSTEM
		if (m_pProto->bType == ITEM_WEAPON || m_pProto->bType == ITEM_ARMOR && m_pProto->bSubType != WEAPON_QUIVER)
#else
		if (m_pProto->bType == ITEM_WEAPON || m_pProto->bType == ITEM_ARMOR) 
#endif
		{
#ifdef ENABLE_EXTENDED_SOCKETS
			for (int i = 0; i < ITEM_STONES_MAX_NUM; i++)
#else
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
#endif
			{
				DWORD dwVnum;
				if ((dwVnum = GetSocket(i)) <= 2)
					continue;

				TItemTable * p = ITEM_MANAGER::instance().GetTable(dwVnum);

				if (!p)
				{
					sys_err("cannot find table by vnum %u", dwVnum);
					continue;
				}

				if (ITEM_METIN == p->bType)
				{
					//m_pOwner->ApplyPoint(p->alValues[0], bAdd ? p->alValues[1] : -p->alValues[1]);
					for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
					{
						if (p->aApplies[i].bType == APPLY_NONE)
							continue;

						if (p->aApplies[i].bType == APPLY_SKILL)
							m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? p->aApplies[i].lValue : p->aApplies[i].lValue ^ 0x00800000);
						else
							m_pOwner->ApplyPoint(p->aApplies[i].bType, bAdd ? p->aApplies[i].lValue : -p->aApplies[i].lValue);
					}
				}
			}
		}

		accessoryGrade = 0;
	}
	else
	{
		accessoryGrade = MIN(GetAccessorySocketGrade(), ITEM_ACCESSORY_SOCKET_MAX_NUM);
	}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if ((GetType() == ITEM_COSTUME) && (GetSubType() == COSTUME_ACCE) && (GetSocket(ACCE_ABSORBED_SOCKET)))
	{
		TItemTable * pkItemAbsorbed = ITEM_MANAGER::instance().GetTable(GetSocket(ACCE_ABSORBED_SOCKET));
		if (pkItemAbsorbed)
		{
			if ((pkItemAbsorbed->bType == ITEM_ARMOR) && (pkItemAbsorbed->bSubType == ARMOR_BODY))
			{
				// basic defense value from armor
				auto lDefGrade = CalcAcceBonus(pkItemAbsorbed->alValues[1] + (pkItemAbsorbed->alValues[5] * 2));
				m_pOwner->ApplyPoint(APPLY_DEF_GRADE_BONUS, bAdd ? lDefGrade : -lDefGrade);
				// basic magic defense value from armor
				auto lDefMagicBonus = CalcAcceBonus(pkItemAbsorbed->alValues[0]);
				m_pOwner->ApplyPoint(APPLY_MAGIC_DEF_GRADE, bAdd ? lDefMagicBonus : -lDefMagicBonus);
			}
			else if (pkItemAbsorbed->bType == ITEM_WEAPON)
			{
				// basic attack value from weapon
				if (pkItemAbsorbed->alValues[3] + pkItemAbsorbed->alValues[4] > 0)
				{
					auto lAttGrade = CalcAcceBonus(pkItemAbsorbed->alValues[4] + pkItemAbsorbed->alValues[5]);
					if (pkItemAbsorbed->alValues[3] > pkItemAbsorbed->alValues[4])
						lAttGrade = CalcAcceBonus(pkItemAbsorbed->alValues[3] + pkItemAbsorbed->alValues[5]);
					m_pOwner->ApplyPoint(APPLY_ATT_GRADE_BONUS, bAdd ? lAttGrade : -lAttGrade);
				}
				// basic magic attack value from weapon
				if (pkItemAbsorbed->alValues[1] + pkItemAbsorbed->alValues[2] > 0)
				{
					int32_t lAttMagicGrade = CalcAcceBonus(pkItemAbsorbed->alValues[2] + pkItemAbsorbed->alValues[5]);
					if (pkItemAbsorbed->alValues[1] > pkItemAbsorbed->alValues[2])
						lAttMagicGrade = CalcAcceBonus(pkItemAbsorbed->alValues[1] + pkItemAbsorbed->alValues[5]);
					m_pOwner->ApplyPoint(APPLY_MAGIC_ATT_GRADE, bAdd ? lAttMagicGrade : -lAttMagicGrade);
				}
			}
		}
	}
#endif


#ifdef ENABLE_AURA_SYSTEM
	if ((GetType() == ITEM_COSTUME) && (GetSubType() == COSTUME_AURA) && (GetSocket(AURA_ABSORBED_SOCKET)))
	{
		TItemTable* pkItemAbsorbed = ITEM_MANAGER::instance().GetTable(GetSocket(AURA_ABSORBED_SOCKET));
		if (pkItemAbsorbed)
		{
			if ((pkItemAbsorbed->bType == ITEM_ARMOR) && (pkItemAbsorbed->bSubType == ARMOR_BODY))
			{
				int32_t lDefGrade = pkItemAbsorbed->alValues[1] + int32_t(pkItemAbsorbed->alValues[5] * 2);
				double dValue = lDefGrade * (float(GetSocket(AURA_ABSORPTION_SOCKET) / 10.0f));
				dValue = (double)dValue / 100;
				dValue = (double)dValue + .5;
				lDefGrade = (int32_t)dValue;
				if (((pkItemAbsorbed->alValues[1] > 0) && (lDefGrade <= 0)) || ((pkItemAbsorbed->alValues[5] > 0) && (lDefGrade < 1)))
					lDefGrade += 1;
				else if ((pkItemAbsorbed->alValues[1] > 0) || (pkItemAbsorbed->alValues[5] > 0))
					lDefGrade += 1;

				m_pOwner->ApplyPoint(APPLY_DEF_GRADE_BONUS, bAdd ? lDefGrade : -lDefGrade);

				int32_t lDefMagicBonus = pkItemAbsorbed->alValues[0];
				dValue = lDefMagicBonus * (float(GetSocket(AURA_ABSORPTION_SOCKET) / 10.0f));
				dValue = (double)dValue / 100;
				dValue = (double)dValue + .5;
				lDefMagicBonus = (int32_t)dValue;
				if ((pkItemAbsorbed->alValues[0] > 0) && (lDefMagicBonus < 1))
					lDefMagicBonus += 1;
				else if (pkItemAbsorbed->alValues[0] > 0)
					lDefMagicBonus += 1;

				m_pOwner->ApplyPoint(APPLY_MAGIC_DEF_GRADE, bAdd ? lDefMagicBonus : -lDefMagicBonus);
			}
			else if (pkItemAbsorbed->bType == ITEM_WEAPON)
			{
				int32_t lAttGrade = pkItemAbsorbed->alValues[4] + pkItemAbsorbed->alValues[5];
				if (pkItemAbsorbed->alValues[3] > pkItemAbsorbed->alValues[4])
					lAttGrade = pkItemAbsorbed->alValues[3] + pkItemAbsorbed->alValues[5];

				double dValue = lAttGrade * (float(GetSocket(AURA_ABSORPTION_SOCKET) / 10.0f));
				dValue = (double)dValue / 100;
				dValue = (double)dValue + .5;
				lAttGrade = (int32_t)dValue;
				if (((pkItemAbsorbed->alValues[3] > 0) && (lAttGrade < 1)) || ((pkItemAbsorbed->alValues[4] > 0) && (lAttGrade < 1)))
					lAttGrade += 1;
				else if ((pkItemAbsorbed->alValues[3] > 0) || (pkItemAbsorbed->alValues[4] > 0))
					lAttGrade += 1;

				m_pOwner->ApplyPoint(APPLY_ATT_GRADE_BONUS, bAdd ? lAttGrade : -lAttGrade);

				int32_t lAttMagicGrade = pkItemAbsorbed->alValues[2] + pkItemAbsorbed->alValues[5];
				if (pkItemAbsorbed->alValues[1] > pkItemAbsorbed->alValues[2])
					lAttMagicGrade = pkItemAbsorbed->alValues[1] + pkItemAbsorbed->alValues[5];

				dValue = lAttMagicGrade * (float(GetSocket(AURA_ABSORPTION_SOCKET) / 10.0f));
				dValue = (double)dValue / 100;
				dValue = (double)dValue + .5;
				lAttMagicGrade = (int32_t)dValue;
				if (((pkItemAbsorbed->alValues[1] > 0) && (lAttMagicGrade < 1)) || ((pkItemAbsorbed->alValues[2] > 0) && (lAttMagicGrade < 1)))
					lAttMagicGrade += 1;
				else if ((pkItemAbsorbed->alValues[1] > 0) || (pkItemAbsorbed->alValues[2] > 0))
					lAttMagicGrade += 1;

				m_pOwner->ApplyPoint(APPLY_MAGIC_ATT_GRADE, bAdd ? lAttMagicGrade : -lAttMagicGrade);
			}
		}
	}
#endif

	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (m_pProto->aApplies[i].bType == APPLY_NONE)
			continue;

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
		if(IsMountItem())
			continue;

		if (GetType() == ITEM_MOUNT_EQUIPMENT) {
			if (!m_pOwner->IsRidingMount())
				continue;
		}
#endif

		int value = m_pProto->aApplies[i].lValue;
		if (m_pProto->aApplies[i].bType == APPLY_SKILL)
			m_pOwner->ApplyPoint(m_pProto->aApplies[i].bType, bAdd ? value : value ^ 0x00800000);
		else
		{
			if (0 != accessoryGrade && i < ITEM_APPLY_MAX_NUM - 1) // @fixme170
				value += MAX(accessoryGrade, value * aiAccessorySocketEffectivePct[accessoryGrade] / 100);
			m_pOwner->ApplyPoint(m_pProto->aApplies[i].bType, bAdd ? value : -value);
		}
	}
#ifdef ENABLE_NEW_COSTUME_BONUS
	ModifyCostumePoints(bAdd);
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_ACCE)
	{
		TItemTable * pkItemAbsorbed = ITEM_MANAGER::instance().GetTable(GetSocket(ACCE_ABSORBED_SOCKET));
		if (pkItemAbsorbed)
		{
			for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
			{
				if (pkItemAbsorbed->aApplies[i].bType == APPLY_NONE)
					continue;
				int value = CalcAcceBonus(pkItemAbsorbed->aApplies[i].lValue);
				if (pkItemAbsorbed->aApplies[i].bType == APPLY_SKILL)
					m_pOwner->ApplyPoint(pkItemAbsorbed->aApplies[i].bType, bAdd ? value : value ^ 0x00800000);
				else
				{
					if (0 != accessoryGrade && i < ITEM_APPLY_MAX_NUM - 1) // @fixme170
						value += MAX(accessoryGrade, value * aiAccessorySocketEffectivePct[accessoryGrade] / 100);
					m_pOwner->ApplyPoint(pkItemAbsorbed->aApplies[i].bType, bAdd ? value : -value);
				}
			}
		}
	}
#endif

#ifdef ENABLE_AURA_SYSTEM
	if ((GetType() == ITEM_COSTUME) && (GetSubType() == COSTUME_AURA))
	{
		TItemTable* pkItemAbsorbed = ITEM_MANAGER::instance().GetTable(GetSocket(AURA_ABSORBED_SOCKET));
		if (pkItemAbsorbed)
		{
			for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
			{
				BYTE bType = m_pProto->aApplies[i].bType;
				int32_t value = m_pProto->aApplies[i].lValue;
				if (pkItemAbsorbed->aApplies[i].bType == APPLY_NONE)
					continue;

				bType = pkItemAbsorbed->aApplies[i].bType;
				value = pkItemAbsorbed->aApplies[i].lValue;
				if (value < 0)
					continue;

				double dValue = value * (float(GetSocket(AURA_ABSORPTION_SOCKET) / 10.0f));
				dValue = (double)dValue / 100;
				dValue = (double)dValue + .5;
				value = (int32_t)dValue;
				if ((pkItemAbsorbed->aApplies[i].lValue > 0) && (value <= 0))
					value += 1;
			}
		}
	}
#endif
#if defined(__ITEM_APPLY_RANDOM__)
	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (m_pProto->aApplies[i].bType == APPLY_RANDOM)
		{
			BYTE bType = GetRandomApply(i).bType;
			short sValue = GetRandomApply(i).sValue;

				if (bType == APPLY_SKILL)
				{
					m_pOwner->ApplyPoint(bType, bAdd ? sValue : sValue ^ 0x00800000);
				}
				else
				{
					if (0 != accessoryGrade)
						sValue += MAX(accessoryGrade, sValue * aiAccessorySocketEffectivePct[accessoryGrade] / 100);

					m_pOwner->ApplyPoint(bType, bAdd ? sValue : -sValue);
				}
		}
	}
#endif
#ifdef ENABLE_ITEMSHOP_TIMED_BONUSES
	if (IsItemShopBonusExpireItem() && GetSocket(2) != 0)
	{
		const TItemTable* pkBonusItem = ITEM_MANAGER::instance().GetTable(GetSocket(2));
		if (pkBonusItem)
		{
			for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
			{
				if (pkBonusItem->aApplies[i].bType == APPLY_NONE)
					continue;

				m_pOwner->ApplyPoint(pkBonusItem->aApplies[i].bType, bAdd ? pkBonusItem->aApplies[i].lValue : -pkBonusItem->aApplies[i].lValue);
			}
		}
	}
#endif
	if (true == CItemVnumHelper::IsRamadanMoonRing(GetVnum()) || true == CItemVnumHelper::IsHalloweenCandy(GetVnum())
		|| true == CItemVnumHelper::IsHappinessRing(GetVnum()) || true == CItemVnumHelper::IsLovePendant(GetVnum()))
	{
		// Do not anything.
	}
	else
	{
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (GetAttributeType(i))
			{
				const TPlayerItemAttribute &ia = GetAttribute(i);
				auto value = ia.sValue;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
				if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_ACCE)
					value = CalcAcceBonus(value);
#endif
#ifdef ENABLE_AURA_SYSTEM
				if ((GetType() == ITEM_COSTUME) && (GetSubType() == COSTUME_AURA))
				{
					double dValue = value * GetSocket(AURA_ABSORPTION_SOCKET);
					dValue = (double)dValue / 100;
					dValue = (double)dValue + .5;
					value = (int32_t)dValue;
					if ((ia.sValue > 0) && (value <= 0))
						value += 1;
				}
#endif
				if (ia.bType == APPLY_SKILL)
					m_pOwner->ApplyPoint(ia.bType, bAdd ? value : value ^ 0x00800000);
				else
					m_pOwner->ApplyPoint(ia.bType, bAdd ? value : -value);
			}
		}
	}

	switch (m_pProto->bType)
	{
		case ITEM_PICK:
		case ITEM_ROD:
			{
				if (bAdd)
				{
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
						m_pOwner->SetPart(PART_WEAPON, GetVnum());
				}
				else
				{
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
						m_pOwner->SetPart(PART_WEAPON, 0);
				}
			}
			break;

		case ITEM_WEAPON:
			{
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
				if (0 != m_pOwner->GetWear(WEAR_COSTUME_WEAPON))
					break;
#endif

				if (bAdd)
				{
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
						m_pOwner->SetPart(PART_WEAPON, GetVnum());
				}
				else
				{
					if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
						m_pOwner->SetPart(PART_WEAPON, 0);
				}
			}
			break;

		case ITEM_ARMOR:
			{
				if (0 != m_pOwner->GetWear(WEAR_COSTUME_BODY))
					break;

				if (GetSubType() == ARMOR_BODY || GetSubType() == ARMOR_HEAD || GetSubType() == ARMOR_FOOTS || GetSubType() == ARMOR_SHIELD)
				{
					if (bAdd)
					{
						if (GetProto()->bSubType == ARMOR_BODY)
							m_pOwner->SetPart(PART_MAIN, GetVnum());
					}
					else
					{
						if (GetProto()->bSubType == ARMOR_BODY)
							m_pOwner->SetPart(PART_MAIN, m_pOwner->GetOriginalPart(PART_MAIN));
					}
				}
			}
			break;

		case ITEM_COSTUME:
			{
				DWORD toSetValue = this->GetVnum();
				EParts toSetPart = PART_MAX_NUM;

				if (GetSubType() == COSTUME_BODY)
				{
					toSetPart = PART_MAIN;

					if (false == bAdd)
					{
						const CItem* pArmor = m_pOwner->GetWear(WEAR_BODY);
						toSetValue = (NULL != pArmor) ? pArmor->GetVnum() : m_pOwner->GetOriginalPart(PART_MAIN);
					}

				}

				else if (GetSubType() == COSTUME_HAIR)
				{
					toSetPart = PART_HAIR;
					toSetValue = (true == bAdd) ? this->GetValue(3) : 0;
				}
#ifdef ENABLE_NEW_MOUNT_SYSTEM
				else if (GetSubType() == COSTUME_MOUNT)
				{
					// not need to do a thing in here
				}
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
				else if (GetSubType() == COSTUME_ACCE)
				{
					toSetValue -= 85000;
					if (GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
						toSetValue += 1000;
					
					toSetValue = (bAdd == true) ? toSetValue : 0;
					// toSetValue = (bAdd == true) ? this->GetAcceVnum() : 0;
					toSetPart = PART_ACCE;
				}
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
				else if (GetSubType() == COSTUME_EMBLEMAT)
				{
                    toSetValue = (true == bAdd) ? this->GetVnum() : 0;
					toSetPart = PART_EMBLEMAT;
				}
#endif
#ifdef ENABLE_AURA_SYSTEM
				else if (GetSubType() == COSTUME_AURA)
				{
					toSetValue = (bAdd == true) ? toSetValue : 0;
					toSetPart = PART_AURA;
				}
#endif
#ifdef ENABLE_STOLE_COSTUME
				else if (GetSubType() == COSTUME_STOLE)
				{
					toSetValue -= 85000;
					if (!bAdd)
					{
						CItem* pAcce = m_pOwner->GetWear(WEAR_COSTUME_ACCE);
						if (!pAcce)
						{
							toSetValue = 0;
						}
						else 
						{
							toSetValue = pAcce->GetVnum();
							toSetValue -= 85000;
							if (pAcce->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_EFFECT_FROM_ABS)
							{
								toSetValue += 1000;
							}
						}
					}
					else
					{
						toSetValue += 1000;
					}
					
					toSetPart = PART_ACCE;
				}
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
				else if (GetSubType() == COSTUME_WEAPON)
				{
					toSetPart = PART_WEAPON;
					if (false == bAdd)
					{
						const CItem* pWeapon = m_pOwner->GetWear(WEAR_WEAPON);
						toSetValue = (NULL != pWeapon) ? pWeapon->GetVnum() : 0;
					}
				}
#endif

				if (PART_MAX_NUM != toSetPart)
				{
					m_pOwner->SetPart((BYTE)toSetPart, toSetValue);
					m_pOwner->UpdatePacket();
				}
			}
			break;
		case ITEM_UNIQUE:
			{
				if (0 != GetSIGVnum())
				{
					const CSpecialItemGroup* pItemGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(GetSIGVnum());
					if (NULL == pItemGroup)
						break;
					DWORD dwAttrVnum = pItemGroup->GetAttrVnum(GetVnum());
					const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(dwAttrVnum);
					if (NULL == pAttrGroup)
						break;
					for (itertype (pAttrGroup->m_vecAttrs) it = pAttrGroup->m_vecAttrs.begin(); it != pAttrGroup->m_vecAttrs.end(); it++)
					{
						m_pOwner->ApplyPoint(it->apply_type, bAdd ? it->apply_value : -it->apply_value);
					}
				}
			}
			break;
	}
}

bool CItem::IsEquipable() const
{
	switch (this->GetType())
	{
	case ITEM_COSTUME:
	case ITEM_ARMOR:
	case ITEM_WEAPON:
	case ITEM_ROD:
	case ITEM_PICK:
	case ITEM_UNIQUE:
	case ITEM_DS:
	case ITEM_SPECIAL_DS:
	case ITEM_RING:
	case ITEM_BELT:
#ifdef ENABLE_NEW_PET_SYSTEM
	case ITEM_NEW_PET:
	case ITEM_NEW_PET_EQ:
#endif
#ifdef ENABLE_SYSTEM_RUNE
	case ITEM_RUNE:
	case ITEM_RUNE_RED:
	case ITEM_RUNE_BLUE:
	case ITEM_RUNE_GREEN:
	case ITEM_RUNE_YELLOW:
	case ITEM_RUNE_BLACK:
#endif
#ifdef ENABLE_ARTEFAKT_SYSTEM
	case ITEM_ARTEFAKT:
#endif
#ifdef ENABLE_MOUNT_SYSTEM
	case ITEM_MOUNT:
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	case ITEM_MOUNT_EQUIPMENT:
#endif
#ifdef __SKILL_COSTUME__
	case ITEM_SKILL_COSTUME:
#endif
		return true;
	}

	return false;
}

#define ENABLE_IMMUNE_FIX
// return false on error state
#ifdef ENABLE_NEW_PET_SYSTEM

bool CItem::EquipTo(LPCHARACTER ch, BYTE bWearCell, bool is_item_load)
{

#else
bool CItem::EquipTo(LPCHARACTER ch, BYTE bWearCell)
{

#endif
	if (!ch)
	{
		sys_err("EquipTo: nil character");
		return false;
	}

	if (IsDragonSoul())
	{
		if (bWearCell < WEAR_MAX_NUM || bWearCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
		{
			sys_err("EquipTo: invalid dragon soul cell (this: #%d %s wearflag: %d cell: %d)", GetOriginalVnum(), GetName(), GetSubType(), bWearCell - WEAR_MAX_NUM);
			return false;
		}
	}
	else
	{
		if (bWearCell >= WEAR_MAX_NUM)
		{
			sys_err("EquipTo: invalid wear cell (this: #%d %s wearflag: %d cell: %d)", GetOriginalVnum(), GetName(), GetWearFlag(), bWearCell);
			return false;
		}
	}

	if (ch->GetWear(bWearCell))
	{
		sys_err("EquipTo: item already exist (this: #%d %s cell: %d %s)", GetOriginalVnum(), GetName(), bWearCell, ch->GetWear(bWearCell)->GetName());
		return false;
	}

	if (GetOwner())
		RemoveFromCharacter();

	ch->SetWear(bWearCell, this);

	OnEquip();

	m_pOwner = ch;
	m_bEquipped = true;
	m_wCell	= INVENTORY_MAX_NUM + bWearCell;

#ifndef ENABLE_IMMUNE_FIX
	DWORD dwImmuneFlag = 0;

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		if (m_pOwner->GetWear(i))
		{
			// m_pOwner->ChatPacket(CHAT_TYPE_INFO, "unequip immuneflag(%u)", m_pOwner->GetWear(i)->m_pProto->dwImmuneFlag); // always 0
			SET_BIT(dwImmuneFlag, m_pOwner->GetWear(i)->m_pProto->dwImmuneFlag);
		}
	}

	m_pOwner->SetImmuneFlag(dwImmuneFlag);
#endif

	if (IsDragonSoul())
	{
		DSManager::instance().ActivateDragonSoul(this);
	}
	else
	{
		ModifyPoints(true);
		StartUniqueExpireEvent();
		if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
			StartTimerBasedOnWearExpireEvent();

		// ACCESSORY_REFINE
		StartAccessorySocketExpireEvent();
		// END_OF_ACCESSORY_REFINE
	}

	ch->BuffOnAttr_AddBuffsFromItem(this);
#ifdef ENABLE_NEW_PET_SYSTEM
	if (!is_item_load && bWearCell == WEAR_NEW_PET)
	{
		ch->SendPetInfo(true);
	}
#endif
	m_pOwner->ComputeBattlePoints();

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (IsMountItem())
		ch->MountSummon(this);
#endif

	m_pOwner->UpdatePacket();

#ifdef __PENDANT__
	if (IsPendant())
		m_pOwner->ComputePoints();
	else
		m_pOwner->UpdatePacket();
#else
	m_pOwner->UpdatePacket();
#endif

#ifdef ENABLE_HIGHLIGHT_NEW_ITEM
	SetLastOwnerPID(m_pOwner->GetPlayerID());
#endif
	Save();

	return (true);
}

bool CItem::Unequip()
{
	if (!m_pOwner || GetCell() < INVENTORY_MAX_NUM)
	{
		// ITEM_OWNER_INVALID_PTR_BUG
		sys_err("%s %u m_pOwner %p, GetCell %d",
				GetName(), GetID(), get_pointer(m_pOwner), GetCell());
		// END_OF_ITEM_OWNER_INVALID_PTR_BUG
		return false;
	}

	if (this != m_pOwner->GetWear(GetCell() - INVENTORY_MAX_NUM))
	{
		sys_err("m_pOwner->GetWear() != this");
		return false;
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (IsMountItem())
		m_pOwner->MountUnsummon(this);
#endif

	if (IsRideItem())
		ClearMountAttributeAndAffect();

	if (IsDragonSoul())
	{
		DSManager::instance().DeactivateDragonSoul(this);
	}
	else
	{
		ModifyPoints(false);
	}

	StopUniqueExpireEvent();

	if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
		StopTimerBasedOnWearExpireEvent();

	// ACCESSORY_REFINE
	StopAccessorySocketExpireEvent();
	// END_OF_ACCESSORY_REFINE

	m_pOwner->BuffOnAttr_RemoveBuffsFromItem(this);

	m_pOwner->SetWear(GetCell() - INVENTORY_MAX_NUM, NULL);
	OnUnequip();

#ifndef ENABLE_IMMUNE_FIX
	DWORD dwImmuneFlag = 0;

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		if (m_pOwner->GetWear(i))
		{
			// m_pOwner->ChatPacket(CHAT_TYPE_INFO, "unequip immuneflag(%u)", m_pOwner->GetWear(i)->m_pProto->dwImmuneFlag); // always 0
			SET_BIT(dwImmuneFlag, m_pOwner->GetWear(i)->m_pProto->dwImmuneFlag);
		}
	}

	m_pOwner->SetImmuneFlag(dwImmuneFlag);
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	if (GetType() == ITEM_NEW_PET)
	{
		m_pOwner->SendPetInfo(true);
	}
#endif
	m_pOwner->ComputeBattlePoints();

#ifdef __PENDANT__
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(m_pOwner->GetPlayerID());
	if (IsPendant() && pPC)
		m_pOwner->ComputePoints();
	else
		m_pOwner->UpdatePacket();
#else
	m_pOwner->UpdatePacket();
#endif

	m_pOwner = NULL;
	m_wCell = 0;
	m_bEquipped	= false;

	return true;
}

int32_t CItem::GetValue(DWORD idx) const
{
	assert(idx < ITEM_VALUES_MAX_NUM);
	return GetProto()->alValues[idx];
}

void CItem::SetExchanging(bool bOn)
{
	m_bExchanging = bOn;
}

void CItem::Save()
{
#ifdef ENABLE_NEW_COSTUME_BONUS
	if (GetType() == ITEM_COSTUME && GetSubType() != COSTUME_ACCE)
		SetCostume();
#endif
	if (m_bSkipSave)
		return;

	ITEM_MANAGER::instance().DelayedSave(this);
}
bool CItem::CreateSocket(BYTE bSlot, BYTE bGold)
{
	assert(bSlot < ITEM_SOCKET_MAX_NUM);

	if (m_alSockets[bSlot] != 0)
	{
		sys_err("Item::CreateSocket : socket already exist %s %d", GetName(), bSlot);
		return false;
	}

	if (bGold)
		m_alSockets[bSlot] = 2;
	else
		m_alSockets[bSlot] = 1;

	UpdatePacket();

	Save();
	return true;
}

void CItem::SetSockets(const int32_t * c_al)
{
	thecore_memcpy(m_alSockets, c_al, sizeof(m_alSockets));
	Save();
}

void CItem::SetSocket(int i, int32_t v, bool bLog)
{
	assert(i < ITEM_SOCKET_MAX_NUM);
	m_alSockets[i] = v;
	UpdatePacket();
	Save();
	if (bLog)
	{
#ifdef ENABLE_NEWSTUFF
		if (g_iDbLogLevel>=LOG_LEVEL_MAX)
#endif
		LogManager::instance().ItemLog(i, v, 0, GetID(), "SET_SOCKET", "", "", GetOriginalVnum());
	}
}

#ifdef ENABLE_LONG_LONG
int64_t CItem::GetGold()
#else
int CItem::GetGold()
#endif
{
	if (IS_SET(GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (GetProto()->dwGold == 0)
			return GetCount();
		else
			return GetCount() / GetProto()->dwGold;
	}
	else
		return GetProto()->dwGold;
}

#ifdef ENABLE_LONG_LONG
int64_t CItem::GetShopBuyPrice()
#else
int CItem::GetShopBuyPrice()
#endif
{
	return GetProto()->dwShopBuyPrice;
}

bool CItem::IsOwnership(LPCHARACTER ch)
{
	if (!m_pkOwnershipEvent)
		return true;

	return m_dwOwnershipPID == ch->GetPlayerID() ? true : false;
}

EVENTFUNC(ownership_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if (!info)
	{
		sys_err( "ownership_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	pkItem->SetOwnershipEvent(NULL);

	TPacketGCItemOwnership p;

	p.bHeader	= HEADER_GC_ITEM_OWNERSHIP;
	p.dwVID	= pkItem->GetVID();
	p.szName[0]	= '\0';

	pkItem->PacketAround(&p, sizeof(p));
	return 0;
}

void CItem::SetOwnershipEvent(LPEVENT pkEvent)
{
	m_pkOwnershipEvent = pkEvent;
}

void CItem::SetOwnership(LPCHARACTER ch, int iSec)
{
	if (!ch)
	{
		if (m_pkOwnershipEvent)
		{
			event_cancel(&m_pkOwnershipEvent);
			m_dwOwnershipPID = 0;

			TPacketGCItemOwnership p;

			p.bHeader	= HEADER_GC_ITEM_OWNERSHIP;
			p.dwVID	= m_dwVID;
			p.szName[0]	= '\0';

			PacketAround(&p, sizeof(p));
		}
		return;
	}

	if (m_pkOwnershipEvent)
		return;

	if (iSec <= 10)
		iSec = 30;

	m_dwOwnershipPID = ch->GetPlayerID();

	item_event_info* info = AllocEventInfo<item_event_info>();
	strlcpy(info->szOwnerName, ch->GetName(), sizeof(info->szOwnerName));
	info->item = this;

	SetOwnershipEvent(event_create(ownership_event, info, PASSES_PER_SEC(iSec)));

	TPacketGCItemOwnership p;

	p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
	p.dwVID = m_dwVID;
	strlcpy(p.szName, ch->GetName(), sizeof(p.szName));

	PacketAround(&p, sizeof(p));
}

int CItem::GetSocketCount()
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
	{
		if (GetSocket(i) == 0)
			return i;
	}
	return ITEM_SOCKET_MAX_NUM;
}

bool CItem::AddSocket()
{
	int count = GetSocketCount();
	if (count == ITEM_SOCKET_MAX_NUM)
		return false;
	m_alSockets[count] = 1;
	return true;
}

void CItem::AlterToSocketItem(int iSocketCount)
{
	if (iSocketCount >= ITEM_SOCKET_MAX_NUM)
	{
		sys_log(0, "Invalid Socket Count %d, set to maximum", ITEM_SOCKET_MAX_NUM);
		iSocketCount = ITEM_SOCKET_MAX_NUM;
	}

	for (int i = 0; i < iSocketCount; ++i)
		SetSocket(i, 1);
}

void CItem::AlterToMagicItem()
{
	int idx = GetAttributeSetIndex();

	if (idx < 0)
		return;

	// Appearance Second Third
	// Weapon 50		20	 5
	// Armor  30		10	 2
	// Acc	20		10	 1

	int iSecondPct;
	int iThirdPct;

	switch (GetType())
	{
		case ITEM_WEAPON:
			iSecondPct = 20;
			iThirdPct = 5;
			break;

		case ITEM_ARMOR:
		case ITEM_COSTUME:
			if (GetSubType() == ARMOR_BODY)
			{
				iSecondPct = 10;
				iThirdPct = 2;
			}
			else
			{
				iSecondPct = 10;
				iThirdPct = 1;
			}
			break;

		default:
			return;
	}

	PutAttribute(aiItemMagicAttributePercentHigh);

	if (number(1, 100) <= iSecondPct)
		PutAttribute(aiItemMagicAttributePercentLow);

	if (number(1, 100) <= iThirdPct)
		PutAttribute(aiItemMagicAttributePercentLow);
}

DWORD CItem::GetRefineFromVnum()
{
	return ITEM_MANAGER::instance().GetRefineFromVnum(GetVnum());
}

int CItem::GetRefineLevel()
{
	const char* name = GetBaseName();
	char* p = const_cast<char*>(strrchr(name, '+'));

	if (!p)
		return 0;

	int	rtn = 0;
	str_to_number(rtn, p+1);

	const char* locale_name = GetName();
	p = const_cast<char*>(strrchr(locale_name, '+'));

	if (p)
	{
		int	locale_rtn = 0;
		str_to_number(locale_rtn, p+1);
		if (locale_rtn != rtn)
		{
			sys_err("refine_level_based_on_NAME(%d) is not equal to refine_level_based_on_LOCALE_NAME(%d).", rtn, locale_rtn);
		}
	}

	return rtn;
}

bool CItem::IsPolymorphItem()
{
	return GetType() == ITEM_POLYMORPH;
}

EVENTFUNC(unique_expire_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if (!info)
	{
		sys_err( "unique_expire_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	if (pkItem->GetValue(2) == 0)
	{
		if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) <= 1)
		{
#ifdef ENABLE_NOTIFICATION_SYSTEM
			if (pkItem)
			{
				if (auto const owner = pkItem->GetOwner())
				{
					owner->SendNotification(ENotification::NOTIFICATION_CATEGORY_SPECIAL_ITEM_EXPIRED, pkItem->GetVnum());
				}
			}
#endif
			sys_log(0, "UNIQUE_ITEM: expire %s %u", pkItem->GetName(), pkItem->GetID());
			pkItem->SetUniqueExpireEvent(NULL);

#ifdef __PREMIUM_PRIVATE_SHOP__
			if (pkItem->GetPrivateShop())
			{
				sys_log(0, "%s PRIVATE_SHOP: ITEM UNIQUE_EXPIRE id %u vnum %u cell %u", pkItem->GetPrivateShop()->GetOwnerName().c_str(),
					pkItem->GetID(), pkItem->GetVnum(), pkItem->GetCell());
				CPrivateShopManager::Instance().SendItemExpire(pkItem);
				return 0;
			}
#endif

			ITEM_MANAGER::instance().RemoveItem(pkItem, "UNIQUE_EXPIRE");
			return 0;
		}
		else
		{
			pkItem->SetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME, pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - 1);
			return PASSES_PER_SEC(60);
		}
	}
	else
	{
		time_t cur = get_global_time();

		if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) <= cur)
		{
#ifdef ENABLE_NOTIFICATION_SYSTEM
			if (pkItem)
			{
				if (auto const owner = pkItem->GetOwner())
				{
					owner->SendNotification(ENotification::NOTIFICATION_CATEGORY_SPECIAL_ITEM_EXPIRED, pkItem->GetVnum());
				}
			}
#endif
			pkItem->SetUniqueExpireEvent(NULL);

#ifdef __PREMIUM_PRIVATE_SHOP__
			if (pkItem->GetPrivateShop())
			{
				sys_log(0, "%s PRIVATE_SHOP: ITEM UNIQUE_EXPIRE id %u vnum %u cell %u", pkItem->GetPrivateShop()->GetOwnerName().c_str(),
					pkItem->GetID(), pkItem->GetVnum(), pkItem->GetCell());
				CPrivateShopManager::Instance().SendItemExpire(pkItem);
				return 0;
			}
#endif
			ITEM_MANAGER::instance().RemoveItem(pkItem, "UNIQUE_EXPIRE");
			return 0;
		}
		else
		{
			// by rtsummit
			if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - cur < 600)
				return PASSES_PER_SEC(pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - cur);
			else
				return PASSES_PER_SEC(600);
		}
	}
}

EVENTFUNC(timer_based_on_wear_expire_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if (!info)
	{
		sys_err( "expire_event <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;
	int remain_time = pkItem->GetSocket(ITEM_SOCKET_REMAIN_SEC) - processing_time/passes_per_sec;
	if (remain_time <= 0)
	{
		sys_log(0, "ITEM EXPIRED : expired %s %u", pkItem->GetName(), pkItem->GetID());
		pkItem->SetTimerBasedOnWearExpireEvent(NULL);
		pkItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, 0);

		if (pkItem->IsDragonSoul())
		{
#ifdef ENABLE_NOTIFICATION_SYSTEM
			if (pkItem)
			{
				if (auto const owner = pkItem->GetOwner())
				{
					owner->SendNotification(ENotification::NOTIFICATION_CATEGORY_ALCHEMY_EXPIRED, pkItem->GetVnum());
				}
			}
#endif
			DSManager::instance().DeactivateDragonSoul(pkItem);
		}
		else
		{
#ifdef __ENABLE_ITEM_TOGGLE__
			if ((pkItem && pkItem->GetType() == ITEM_TOGGLE && pkItem->GetValue(ITEM_VALUE_TOGGLE_FORCE_NOT_DESTROY)))
			{
				pkItem->OnUse();
				return 0;
			}
#endif
#ifdef ENABLE_NOTIFICATION_SYSTEM
			if (pkItem)
			{
				if (auto const owner = pkItem->GetOwner())
				{
					owner->SendNotification(ENotification::NOTIFICATION_CATEGORY_SPECIAL_ITEM_EXPIRED, pkItem->GetVnum());
				}
			}
#endif

			ITEM_MANAGER::instance().RemoveItem(pkItem, "TIMER_BASED_ON_WEAR_EXPIRE");
		}
		return 0;
	}
	pkItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, remain_time);
	return PASSES_PER_SEC (MIN (60, remain_time));
}

void CItem::SetUniqueExpireEvent(LPEVENT pkEvent)
{
	m_pkUniqueExpireEvent = pkEvent;
}

void CItem::SetTimerBasedOnWearExpireEvent(LPEVENT pkEvent)
{
	m_pkTimerBasedOnWearExpireEvent = pkEvent;
}

#ifdef ENABLE_ITEMSHOP_TIMED_BONUSES
EVENTFUNC(itemshop_bonus_time_expire_event)
{
	const item_vid_event_info* info = reinterpret_cast<const item_vid_event_info*>(event->info);
	if (NULL == info)
		return 0;

	const LPITEM item = ITEM_MANAGER::instance().FindByVID(info->item_vid);
	if (NULL == item)
		return 0;

	if (item->GetSocket(2) == 0)
	{
		item->SetItemshopBonusTimeExpireEvent(NULL);
		return 0;
	}

	const time_t current = get_global_time();
	if (current > item->GetSocket(1))
	{
		const bool isEquipped = item->GetOwner() && item->IsEquipped();

		if (test_server)
			sys_log(0, "ITEMSHOP_BONUS: Delete from item ID %lu", item->GetID());

		if (item->GetVnum() && item->IsNewMountItem()) // @fixme152
			item->ClearMountAttributeAndAffect();

		item->SetSocket(1, 0);
		item->SetSocket(2, 0);

		item->Save();

		if (isEquipped)
			item->GetOwner()->ComputePoints();
			item->GetOwner()->PointsPacket();

		item->SetItemshopBonusTimeExpireEvent(NULL);
		return 0;
	}

	return PASSES_PER_SEC(1);
}

void CItem::SetItemshopBonusTimeExpireEvent(LPEVENT pkEvent)
{
	m_pkRealItemShopBonusTimeExpireEvent = pkEvent;
}

void CItem::StartItemshopBonusTimeExpireEvent()
{
	if (m_pkRealItemShopBonusTimeExpireEvent)
		return;

	item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
	info->item_vid = GetVID();

	SetItemshopBonusTimeExpireEvent(event_create(itemshop_bonus_time_expire_event, info, PASSES_PER_SEC(1)));
}

bool CItem::IsItemShopBonusExpireItem()
{
	if (!GetProto())
		return false;

	if (GetType() == ITEM_COSTUME 
		&& (GetSubType() == COSTUME_BODY 
			|| GetSubType() == COSTUME_HAIR 
			|| GetSubType() == COSTUME_WEAPON
			|| GetSubType() == COSTUME_MOUNT
			|| GetSubType() == COSTUME_STOLE
		))
		return true;

	if (GetType() == ITEM_NEW_PET)
		return true;

	return false;
}
#endif


EVENTFUNC(real_time_expire_event)
{
	const item_vid_event_info* info = reinterpret_cast<const item_vid_event_info*>(event->info);

	if (!info)
		return 0;

	const LPITEM item = ITEM_MANAGER::instance().FindByVID( info->item_vid );

	if (!item)
		return 0;

	LPCHARACTER owner = item->GetOwner();

	if (!owner)
		return 0;

	const time_t current = get_global_time();

	if (current > item->GetSocket(0))
	{
		// if (item->GetVnum() && item->IsNewMountItem()) // @fixme152
		// 	item->ClearMountAttributeAndAffect();

#ifdef __PREMIUM_PRIVATE_SHOP__
		if (item->GetPrivateShop())
		{
			sys_log(0, "%s PRIVATE_SHOP: ITEM EXPIRE id %u vnum %u cell %u", item->GetPrivateShop()->GetOwnerName().c_str(),
				item->GetID(), item->GetVnum(), item->GetCell());
			CPrivateShopManager::Instance().SendItemExpire(item);
			return 0;
		}
#endif
#ifdef ENABLE_NOTIFICATION_SYSTEM
		if (item)
		{
			if (auto const owner = item->GetOwner())
			{
				owner->SendNotification(ENotification::NOTIFICATION_CATEGORY_SPECIAL_ITEM_EXPIRED, item->GetVnum());
			}
		}
#endif

		ITEM_MANAGER::instance().RemoveItem(item, "REAL_TIME_EXPIRE");

		return 0;
	}
	
	return PASSES_PER_SEC(1);
}

void CItem::StartRealTimeExpireEvent()
{
	if (m_pkRealTimeExpireEvent)
		return;
	for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType || LIMIT_REAL_TIME_START_FIRST_USE == GetProto()->aLimits[i].bType)
		{
			item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
			info->item_vid = GetVID();

			m_pkRealTimeExpireEvent = event_create( real_time_expire_event, info, PASSES_PER_SEC(1));

			sys_log(0, "REAL_TIME_EXPIRE: StartRealTimeExpireEvent");

			return;
		}
	}
}

bool CItem::IsRealTimeItem()
{
	if(!GetProto())
		return false;
	for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType)
			return true;
	}
	return false;
}

void CItem::StartUniqueExpireEvent()
{
	if (GetType() != ITEM_UNIQUE)
		return;

	if (m_pkUniqueExpireEvent)
		return;

	if (IsRealTimeItem())
		return;

	// HARD CODING
	if (GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		m_pOwner->ShowAlignment(false);

	int iSec = GetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME);

	if (iSec == 0)
		iSec = 60;
	else
		iSec = MIN(iSec, 60);

	SetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME, 0);

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetUniqueExpireEvent(event_create(unique_expire_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::StartTimerBasedOnWearExpireEvent()
{
	if (m_pkTimerBasedOnWearExpireEvent)
		return;

	if (IsRealTimeItem())
		return;

	if (-1 == GetProto()->cLimitTimerBasedOnWearIndex)
		return;

	int iSec = GetSocket(0);

	if (0 != iSec)
	{
		iSec %= 60;
		if (0 == iSec)
			iSec = 60;
	}

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetTimerBasedOnWearExpireEvent(event_create(timer_based_on_wear_expire_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::StopUniqueExpireEvent()
{
	if (!m_pkUniqueExpireEvent)
		return;

	if (GetValue(2) != 0)
		return;

	// HARD CODING
	if (GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		m_pOwner->ShowAlignment(true);

	SetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME, event_time(m_pkUniqueExpireEvent) / passes_per_sec);
	event_cancel(&m_pkUniqueExpireEvent);

	ITEM_MANAGER::instance().SaveSingleItem(this);
}

void CItem::StopTimerBasedOnWearExpireEvent()
{
	if (!m_pkTimerBasedOnWearExpireEvent)
		return;

	int remain_time = GetSocket(ITEM_SOCKET_REMAIN_SEC) - event_processing_time(m_pkTimerBasedOnWearExpireEvent) / passes_per_sec;

	SetSocket(ITEM_SOCKET_REMAIN_SEC, remain_time);
	event_cancel(&m_pkTimerBasedOnWearExpireEvent);

	ITEM_MANAGER::instance().SaveSingleItem(this);
}

void CItem::ApplyAddon(int iAddonType)
{
	CItemAddonManager::instance().ApplyAddonTo(iAddonType, this);
}

int CItem::GetSpecialGroup() const
{
	return ITEM_MANAGER::instance().GetSpecialGroupFromItem(GetVnum());
}

bool CItem::IsAccessoryForSocket()
{
	return (m_pProto->bType == ITEM_ARMOR && (m_pProto->bSubType == ARMOR_WRIST || m_pProto->bSubType == ARMOR_NECK || m_pProto->bSubType == ARMOR_EAR)) ||
		(m_pProto->bType == ITEM_BELT);
}

void CItem::SetAccessorySocketGrade(int iGrade)
{
	SetSocket(0, MINMAX(0, iGrade, GetAccessorySocketMaxGrade()));

	int iDownTime = aiAccessorySocketDegradeTime[GetAccessorySocketGrade()];

	SetAccessorySocketDownGradeTime(iDownTime);
}

void CItem::SetAccessorySocketMaxGrade(int iMaxGrade)
{
	SetSocket(1, MINMAX(0, iMaxGrade, ITEM_ACCESSORY_SOCKET_MAX_NUM));
}

void CItem::SetAccessorySocketDownGradeTime(DWORD time)
{
	SetSocket(2, time);
}

EVENTFUNC(accessory_socket_expire_event)
{
	item_vid_event_info* info = dynamic_cast<item_vid_event_info*>( event->info );

	if (!info)
	{
		sys_err( "accessory_socket_expire_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM item = ITEM_MANAGER::instance().FindByVID(info->item_vid);

	if (item->GetAccessorySocketDownGradeTime() <= 1)
	{
degrade:
		item->SetAccessorySocketExpireEvent(NULL);
		item->AccessorySocketDegrade();
		return 0;
	}
	else
	{
		int iTime = item->GetAccessorySocketDownGradeTime() - 60;

		if (iTime <= 1)
			goto degrade;

		item->SetAccessorySocketDownGradeTime(iTime);

		if (iTime > 60)
			return PASSES_PER_SEC(60);
		else
			return PASSES_PER_SEC(iTime);
	}
}

void CItem::StartAccessorySocketExpireEvent()
{
	if (!IsAccessoryForSocket())
		return;

	if (m_pkAccessorySocketExpireEvent)
		return;

	if (GetAccessorySocketMaxGrade() == 0)
		return;

	if (GetAccessorySocketGrade() == 0)
		return;

	int iSec = GetAccessorySocketDownGradeTime();
	SetAccessorySocketExpireEvent(NULL);

	if (iSec <= 1)
		iSec = 5;
	else
		iSec = MIN(iSec, 60);

	item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
	info->item_vid = GetVID();

	SetAccessorySocketExpireEvent(event_create(accessory_socket_expire_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::StopAccessorySocketExpireEvent()
{
	if (!m_pkAccessorySocketExpireEvent)
		return;

	if (!IsAccessoryForSocket())
		return;

	int new_time = GetAccessorySocketDownGradeTime() - (60 - event_time(m_pkAccessorySocketExpireEvent) / passes_per_sec);

	event_cancel(&m_pkAccessorySocketExpireEvent);

	if (new_time <= 1)
	{
		AccessorySocketDegrade();
	}
	else
	{
		SetAccessorySocketDownGradeTime(new_time);
	}
}

bool CItem::bonusOnlyByPremiumItem()
{
	
	switch(GetType())
	{
		case ITEM_COSTUME:
			return true;
	}
	
	if (GetVnum() >= 53001 && GetVnum() <= 53999)
		return true;
	
	return false;
}

bool CItem::IsPremiumBonusItem()
{
	switch(GetVnum())
	{
		case 33050:
		case 33051:
		case 33052:
		case 33053:
		case 33054:
		case 33055:
			return true;
	}
	return false;
}

bool CItem::IsRideItem()
{
	if (ITEM_UNIQUE == GetType() && UNIQUE_SPECIAL_RIDE == GetSubType())
		return true;
	if (ITEM_UNIQUE == GetType() && UNIQUE_SPECIAL_MOUNT_RIDE == GetSubType())
		return true;
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (ITEM_COSTUME == GetType() && COSTUME_MOUNT == GetSubType())
		return true;
#endif
	return false;
}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
bool CItem::IsMountItem()
{
	if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_MOUNT)
		return true;
	
	return false;
}
#endif

bool CItem::IsRamadanRing()
{
	if (GetVnum() == UNIQUE_ITEM_RAMADAN_RING)
		return true;
	return false;
}

void CItem::ClearMountAttributeAndAffect()
{
	LPCHARACTER ch = GetOwner();
	if (!ch) // @fixme186
		return;

	ch->RemoveAffect(AFFECT_MOUNT);
	ch->RemoveAffect(AFFECT_MOUNT_BONUS);

	ch->MountVnum(0);

	ch->PointChange(POINT_ST, 0);
	ch->PointChange(POINT_DX, 0);
	ch->PointChange(POINT_HT, 0);
	ch->PointChange(POINT_IQ, 0);
}

bool CItem::IsNewMountItem()
{
#ifdef ENABLE_NEW_MOUNT_SYSTEM
	if (ITEM_COSTUME == GetType() && COSTUME_MOUNT == GetSubType())
		return true;
#endif
	return false;
}

void CItem::SetAccessorySocketExpireEvent(LPEVENT pkEvent)
{
	m_pkAccessorySocketExpireEvent = pkEvent;
}

void CItem::AccessorySocketDegrade()
{
	if (GetAccessorySocketGrade() > 0)
	{
		LPCHARACTER ch = GetOwner();

		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;471;%s]", GetName());
#ifdef ENABLE_NOTIFICATION_SYSTEM
			ch->SendNotification(ENotification::NOTIFICATION_CATEGORY_MELT_EXPIRED, GetVnum());
#endif
		}

		ModifyPoints(false);
		SetAccessorySocketGrade(GetAccessorySocketGrade()-1);
		ModifyPoints(true);

		int iDownTime = aiAccessorySocketDegradeTime[GetAccessorySocketGrade()];

		if (test_server)
			iDownTime /= 60;

		SetAccessorySocketDownGradeTime(iDownTime);

		if (iDownTime)
			StartAccessorySocketExpireEvent();
	}
}

static const bool CanPutIntoRing(LPITEM ring, LPITEM item)
{
	//const DWORD vnum = item->GetVnum();
	return false;
}

bool CItem::CanPutInto(LPITEM item)
{
	if (!item)
		return false;

	if (item->GetType() == ITEM_BELT)
		return this->GetSubType() == USE_PUT_INTO_BELT_SOCKET || this->GetVnum() == 50649;

	else if(item->GetType() == ITEM_RING)
		return CanPutIntoRing(item, this);

	else if (item->GetType() != ITEM_ARMOR)
		return false;

	DWORD vnum = item->GetVnum();

	struct JewelAccessoryInfo
	{
		DWORD jewel;
		DWORD wrist;
		DWORD neck;
		DWORD ear;
	};
	const static JewelAccessoryInfo infos[] = {
		// { 50634, 14420, 16220, 17220 },
		// { 50635, 14500, 16500, 17500 },
		// { 50636, 14520, 16520, 17520 },
		// { 50637, 14540, 16540, 17540 },
		// { 50638, 14560, 16560, 17560 },
		{ 50639, 89110, 89120, 89130 }, //eq exp start - 50
		{ 50640, 32180, 32280, 32380 },
		{ 50641, 32120, 32220, 32320 },
		{ 50642, 32150, 32250, 32350 },
		{ 50643, 32130, 32230, 32330 },
		{ 50644, 32160, 32260, 32360 },
		{ 50647, 32670,	32680,	32690 },
		{ 50639, 17200, 14200, 16200 }, // eq pvp start - 50
		{ 50640, 32170, 32270, 32370 },
		{ 50641, 32140, 32240, 32340 },
		{ 50642, 32100, 32200, 32300 },
		{ 50643, 32190, 32290, 32390 },
		{ 50644, 32110, 32210, 32310 },
	};

	DWORD item_type = (item->GetVnum() / 10) * 10;
	for (size_t i = 0; i < sizeof(infos) / sizeof(infos[0]); i++)
	{
		const JewelAccessoryInfo& info = infos[i];
		switch(item->GetSubType())
		{
		case ARMOR_WRIST:
			if (info.wrist == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		case ARMOR_NECK:
			if (info.neck == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		case ARMOR_EAR:
			if (info.ear == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		}
	}
	if (item->GetSubType() == ARMOR_WRIST)
		vnum -= 14000;
	else if (item->GetSubType() == ARMOR_NECK)
		vnum -= 16000;
	else if (item->GetSubType() == ARMOR_EAR)
		vnum -= 17000;
	else
		return false;

	DWORD type = vnum / 20;

	if (type < 0 || type > 11)
	{
		type = (vnum - 170) / 20;

		if (50623 + type != GetVnum())
			return false;
		else
			return true;
	}
	else if (item->GetVnum() >= 16210 && item->GetVnum() <= 16219)
	{
		if (50625 != GetVnum())
			return false;
		else
			return true;
	}
	else if (item->GetVnum() >= 16230 && item->GetVnum() <= 16239)
	{
		if (50626 != GetVnum())
			return false;
		else
			return true;
	}

	return 50623 + type == GetVnum();
}

bool CItem::CheckItemUseLevel(int nLevel)
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (this->m_pProto->aLimits[i].bType == LIMIT_LEVEL)
		{
			if (this->m_pProto->aLimits[i].lValue > nLevel) return false;
			else return true;
		}
	}
	return true;
}

int32_t CItem::FindApplyValue(BYTE bApplyType)
{
	if (m_pProto == NULL)
		return 0;

	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (m_pProto->aApplies[i].bType == bApplyType)
			return m_pProto->aApplies[i].lValue;
	}

	return 0;
}

void CItem::CopySocketTo(LPITEM pItem)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		pItem->m_alSockets[i] = m_alSockets[i];
	}
}

int CItem::GetAccessorySocketGrade()
{
   	return MINMAX(0, GetSocket(0), GetAccessorySocketMaxGrade());
}

int CItem::GetAccessorySocketMaxGrade()
{
   	return MINMAX(0, GetSocket(1), ITEM_ACCESSORY_SOCKET_MAX_NUM);
}

int CItem::GetAccessorySocketDownGradeTime()
{
	return MINMAX(0, GetSocket(2), aiAccessorySocketDegradeTime[GetAccessorySocketGrade()]);
}

void CItem::AttrLog()
{
	const char * pszIP = NULL;

	if (GetOwner() && GetOwner()->GetDesc())
		pszIP = GetOwner()->GetDesc()->GetHostName();

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (m_alSockets[i])
		{
#ifdef ENABLE_NEWSTUFF
			if (g_iDbLogLevel>=LOG_LEVEL_MAX)
#endif
			LogManager::instance().ItemLog(i, m_alSockets[i], 0, GetID(), "INFO_SOCKET", "", pszIP ? pszIP : "", GetOriginalVnum());
		}
	}

	for (int i = 0; i<ITEM_ATTRIBUTE_MAX_NUM; ++i)
	{
		int	type	= m_aAttr[i].bType;
		int value	= m_aAttr[i].sValue;

		if (type)
		{
#ifdef ENABLE_NEWSTUFF
			if (g_iDbLogLevel>=LOG_LEVEL_MAX)
#endif
			LogManager::instance().ItemLog(i, type, value, GetID(), "INFO_ATTR", "", pszIP ? pszIP : "", GetOriginalVnum());
		}
	}
}

int CItem::GetLevelLimit()
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (this->m_pProto->aLimits[i].bType == LIMIT_LEVEL)
		{
			return this->m_pProto->aLimits[i].lValue;
		}
	}
	return 0;
}

int CItem::GetRealUseLimit()
{
	for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++) {
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType || LIMIT_REAL_TIME_START_FIRST_USE == GetProto()->aLimits[i].bType)
			return this->m_pProto->aLimits[i].lValue;
	}
	return 0;
}

bool CItem::OnAfterCreatedItem()
{
	if (-1 != this->GetProto()->cLimitRealTimeFirstUseIndex)
	{
		if (0 != GetSocket(1))
		{
			StartRealTimeExpireEvent();
		}
	}
#ifdef ENABLE_EXTENDED_BLEND
	if (IsItemBlend())
	{
		if (0 != GetSocket(1))
		{
			Lock(true);
		}
	}
#endif

	return true;
}

#ifdef ENABLE_EXTENDED_BLEND
bool CItem::IsItemBlend()
{
	return GetType() == ITEM_BLEND;
}

// Could've used VnumHelper.h but who cares.
bool CItem::IsDragonGodItem()
{
	switch(GetVnum())
	{
		// case 71044: case 71045: case 72046: case 72047:
		case 72046: case 72047:
		case 72048: case 39024: case 39025: case 72025:
		case 72026: case 72027: case 76035: case 76036:
			return true;
	}

	if (GetVnum() >= 39017 && GetVnum() <= 39020)
		return true;

	// if (GetVnum() >= 71027 && GetVnum() <= 71030)
	// 	return true;

	if (GetVnum() >= 72031 && GetVnum() <= 72042)
		return true;

	return false;
}

bool CItem::IsWaterItem()
{
	return (GetVnum() >= 50813 && GetVnum() <= 50820)
		|| (GetVnum() >= 50821 && GetVnum() <= 50826);
}
#endif

bool CItem::IsDragonSoul()
{
	return GetType() == ITEM_DS;
}

int CItem::GiveMoreTime_Per(float fPercent)
{
	if (IsDragonSoul())
	{
		DWORD duration = DSManager::instance().GetDuration(this);
		DWORD remain_sec = GetSocket(ITEM_SOCKET_REMAIN_SEC);
		DWORD given_time = fPercent * duration / 100u;
		if (remain_sec == duration)
			return false;
		if ((given_time + remain_sec) >= duration)
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, duration);
			return duration - remain_sec;
		}
		else
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, given_time + remain_sec);
			return given_time;
		}
	}

	else
		return 0;
}

int CItem::GiveMoreTime_Fix(DWORD dwTime)
{
	if (IsDragonSoul())
	{
		DWORD duration = DSManager::instance().GetDuration(this);
		DWORD remain_sec = GetSocket(ITEM_SOCKET_REMAIN_SEC);
		if (remain_sec == duration)
			return false;
		if ((dwTime + remain_sec) >= duration)
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, duration);
			return duration - remain_sec;
		}
		else
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, dwTime + remain_sec);
			return dwTime;
		}
	}

	else
		return 0;
}

int	CItem::GetDuration()
{
	if(!GetProto())
		return -1;

	for (int i=0 ; i < ITEM_LIMIT_MAX_NUM ; i++)
	{
		if (LIMIT_REAL_TIME == GetProto()->aLimits[i].bType)
			return GetProto()->aLimits[i].lValue;
	}

	if (GetProto()->cLimitTimerBasedOnWearIndex >= 0)
	{
		BYTE cLTBOWI = GetProto()->cLimitTimerBasedOnWearIndex;
		return GetProto()->aLimits[cLTBOWI].lValue;
	}

	return -1;
}

bool CItem::IsSameSpecialGroup(const LPITEM item) const
{
	if (this->GetVnum() == item->GetVnum())
		return true;

	if (GetSpecialGroup() && (item->GetSpecialGroup() == GetSpecialGroup()))
		return true;

	return false;
}

#ifdef __RENEWAL_MOUNT__
bool CItem::IsCostumeMountItem()
{
	return GetType() == ITEM_COSTUME && GetSubType() == COSTUME_MOUNT;
}
#endif

//test
bool CItem::IsSameSubType(const LPITEM item) const
{
	if (this->GetSubType() == item->GetSubType())
		return true;

	if (GetSpecialGroup() && (item->GetSpecialGroup() == GetSpecialGroup()))
		return true;

	return false;
}

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
bool CItem::IsSkillBook()
{
	if (GetSpecialInvType() == 1) {
		return true;
	} else {
		return false;
	}
	// return (GetVnum() == 50300);
}

bool CItem::IsUpgradeItem()
{
	if (GetSpecialInvType() == 2) {
		return true;
	} else {
		return false;
	}
}

bool CItem::IsStone()
{
	if (GetSpecialInvType() == 3 || GetType() == ITEM_METIN) {
		return true;
	} else {
		return false;
	}
}

bool CItem::IsBox()
{
	if (GetSpecialInvType() == 4) {
		return true;
	} else {
		return false;
	}
}

bool CItem::IsEfsun()
{
	if (GetSpecialInvType() == 5) {
		return true;
	} else {
		return false;
	}
}

bool CItem::IsCicek()
{
	if (GetSpecialInvType() == 6) {
		return true;
	} else {
		return false;
	}
}

DWORD CItem::GetSpecialWindowType()
{
	if (IsSkillBook())
	{
		return SKILL_BOOK_INVENTORY;
	}
	else if (IsUpgradeItem())
	{
		return UPGRADE_ITEMS_INVENTORY;
	}
	else if (IsStone())
	{
		return STONE_INVENTORY;
	}
	else if (IsBox())
	{
		return BOX_INVENTORY;
	}
	else if (IsEfsun())
	{
		return EFSUN_INVENTORY;
	}
	else if (IsCicek())
	{
		return CICEK_INVENTORY;
	}

	return 0;
}
#endif

#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
const char * CItem::GetName()
{
	static char szItemName[128];
	memset(szItemName, 0, sizeof(szItemName));
	if (GetProto())
	{
		int len = 0;
		switch (GetType())
		{
			case ITEM_POLYMORPH:
			{
				const DWORD dwMobVnum = GetSocket(0);
				const CMob* pMob = CMobManager::instance().Get(dwMobVnum);
				if (pMob)
					len = snprintf(szItemName, sizeof(szItemName), "%s", pMob->m_table.szLocaleName);

				break;
			}
			case ITEM_SKILLBOOK:
			case ITEM_SKILLFORGET:
			{
				const DWORD dwSkillVnum = (GetVnum() == ITEM_SKILLBOOK_VNUM || GetVnum() == ITEM_SKILLFORGET_VNUM) ? GetSocket(0) : 0;
				const CSkillProto* pSkill = (dwSkillVnum != 0) ? CSkillManager::instance().Get(dwSkillVnum) : NULL;
				if (pSkill)
					len = snprintf(szItemName, sizeof(szItemName), "%s", pSkill->szName);

				break;
			}
		}
		len += snprintf(szItemName + len, sizeof(szItemName) - len, (len>0)?" %s":"%s", GetProto()->szLocaleName);
	}

	return szItemName;
}
#endif

bool CItem::IsRingSameGroup(const LPITEM item) const
{
	if (this->GetVnum() == item->GetVnum())
		return true;

	return false;
}

bool CItem::IsStackable(const CItem* other) const {
    if (!IsStackable() || !other->IsStackable()) {
        return false;
    }

    if (IS_SET(GetAntiFlag(), ITEM_ANTIFLAG_STACK) ||
        IS_SET(other->GetAntiFlag(), ITEM_ANTIFLAG_STACK)) {
        return false;
    }

    if (GetVnum() != other->GetVnum()) {
        return false;
    }

    for (size_t i = 0; i < ITEM_SOCKET_MAX_NUM; ++i) {
#ifdef ENABLE_SWITCHER_CHEST
        if (IsChangeAttributeChest() && other->IsChangeAttributeChest() &&
            i == CHANGE_ATTRIBUTE_CHEST_SOCKET_INDEX)
            continue;
#endif
        if (GetSocket(i) != other->GetSocket(i)) {
            return false;
        }
    }

    return true;
}

uint32_t CItem::GetAntiFlag() const {
    if (GetProto())
        return GetProto()->dwAntiFlags;

	return 0;
}

bool CItem::IsSameItemData(LPITEM item, bool checkWindow, bool checkSameOwner)
{
	if (!item)
		return false;

	if (GetOriginalVnum() != item->GetOriginalVnum())
		return false;

	if (GetID() == item->GetID())
		return false;

	if (checkSameOwner && GetOwner() != item->GetOwner())
		return false;

	if (checkWindow && GetWindow() != item->GetWindow())
		return false;

	int i = 0;
	for (; i < ITEM_SOCKET_MAX_NUM; i++)
	{
		if (GetSocket(i) != item->GetSocket(i))
			return false;
	}

	for (i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++)
	{
		if (GetAttributeType(i) != item->GetAttributeType(i) || GetAttributeValue(i) != item->GetAttributeValue(i))
			return false;
	}

	return true;
}

const TItemLimit* CItem::FindLimit(uint8_t type) const {
	for (const auto& limit : m_pProto->aLimits)
		if (limit.bType == type)
			return &limit;

	return nullptr;
}

void CItem::OnCreate() {}

void CItem::OnLoad() {}

void CItem::OnRemove() {}

bool CItem::OnUse() {
	return true;
}

bool CItem::OnUseOn(CItem& other) {
	return true;
}

void CItem::OnEquip() {}

void CItem::OnUnequip() {}

void CItem::OnAddToCharacter() {}
void CItem::OnRemoveFromCharacter() {}

CItem* CItem::New(uint32_t vnum) {
	auto proto = ITEM_MANAGER::Instance().GetTable(vnum);
	if (proto)
		return New(vnum, *proto);

	return nullptr;
}

CItem* CItem::New(uint32_t vnum, const TItemTable& table) {
	switch (table.bType) {
#ifdef __ENABLE_ITEM_TOGGLE__
	case ITEM_TOGGLE:
		return NewItemToggle(vnum, table);
#endif
	}

	return new CItem(vnum);
}

#ifdef __ENABLE_ITEM_TOGGLE__
CItem* CItem::NewItemToggle(uint32_t vnum, const TItemTable& table) {
	switch (table.bSubType) {

	}

	return new item::ItemToggle(vnum);
}
#endif

#ifdef __PENDANT__
bool CItem::IsPendant()
{
	if (GetType() == ITEM_ARMOR)
	{
		switch (GetSubType())
		{
		case ARMOR_PENDANT_FIRE:
		case ARMOR_PENDANT_ICE:
		case ARMOR_PENDANT_THUNDER:
		case ARMOR_PENDANT_WIND:
		case ARMOR_PENDANT_DARK:
		case ARMOR_PENDANT_EARTH:
		case ARMOR_PENDANT_SOUL:
			return true;
		}
	}
	return false;
}
#endif
