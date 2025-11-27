#include "stdafx.h"

#include "../../common/service.h"
#include "../../common/VnumHelper.h"

#include <stack>
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "packet.h"
#include "affect.h"
#include "skill.h"
#include "start_position.h"
#include "mob_manager.h"
#include "db.h"
#include "log.h"
#include "vector.h"
#include "buffer_manager.h"
#include "questmanager.h"
#include "fishing.h"
#include "party.h"
#include "dungeon.h"
#include "refine.h"
#include "unique_item.h"
#include "war_map.h"
#include "xmas_event.h"
#include "marriage.h"
#include "polymorph.h"
#include "blend_item.h"
#include "BattleArena.h"
#include "arena.h"
#include "safebox.h"
#include "shop.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"

#ifdef ENABLE_RENEWAL_SWITCHBOT
	#include "switchbot_manager.h"
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
#include "growth_pet.h"
#endif

const int ITEM_BROKEN_METIN_VNUM = 28960;

const DWORD CHARACTER::msc_dwDefaultChangeItemAttrCycle = 10;
const char CHARACTER::msc_szLastChangeItemAttrFlag[] = "Item.LastChangeItemAttr";
const char CHARACTER::msc_szChangeItemAttrCycleFlag[] = "change_itemattr_cycle";

const BYTE g_aBuffOnAttrPoints[] = { POINT_ENERGY, POINT_COSTUME_ATTR_BONUS };

struct FFindStone
{
	std::map<DWORD, LPCHARACTER> m_mapStone;

	void operator()(LPENTITY pEnt)
	{
		if (pEnt->IsType(ENTITY_CHARACTER) == true)
		{
			LPCHARACTER pChar = (LPCHARACTER)pEnt;

			if (pChar->IsStone() == true)
			{
				m_mapStone[(DWORD)pChar->GetVID()] = pChar;
			}
		}
	}
};

static bool IS_SUMMON_ITEM(int vnum)
{
	switch (vnum)
	{
		case 22000:
		case 22010:
		case 22011:
		case 22020:
		case ITEM_MARRIAGE_RING:
			return true;
	}

	return false;
}

static bool IS_MONKEY_DUNGEON(int map_index)
{
	switch (map_index)
	{
		case 5:
		case 25:
		case 45:
		case 108:
		case 109:
			return true;;
	}

	return false;
}

bool IS_SUMMONABLE_ZONE(int map_index)
{
	if (IS_MONKEY_DUNGEON(map_index))
		return false;

	switch (map_index)
	{
		case 66 :
		case 71 :
		case 72 :
		case 73 :
		case 193 :
#if 0
		case 184 :
		case 185 :
		case 186 :
		case 187 :
		case 188 :
		case 189 :
#endif
		case 216 :
		case 217 :
		case 208 :
			return false;
	}

	if (CBattleArena::IsBattleArenaMap(map_index)) return false;

	if (map_index > 10000) return false;

	return true;
}

bool IS_BOTARYABLE_ZONE(int nMapIndex)
{
	switch (nMapIndex)
	{
		case 1 :
		case 3 :
		case 21 :
		case 23 :
		case 41 :
		case 43 :
			return true;
	}
	return false;
}

static bool FN_check_item_socket(LPITEM item)
{
	if (!item)
		return false;

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item->GetSocket(i) != item->GetProto()->alSockets[i])
			return false;
	}

	return true;
}

static void FN_copy_item_socket(LPITEM dest, LPITEM src)
{
	if (!dest || !src)
		return;

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		dest->SetSocket(i, src->GetSocket(i));
	}
}

static bool FN_check_item_sex(LPCHARACTER ch, LPITEM item)
{
	if (!ch || !item)
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE))
	{
		if (SEX_MALE==GET_SEX(ch))
			return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE)) 
	{
		if (SEX_FEMALE==GET_SEX(ch))
			return false;
	}

	return true;
}

static bool FN_compare_item_socket(const LPITEM pkItemSrc, const LPITEM pkItemDest)
{
	if (!pkItemSrc || !pkItemDest)
		return false;

	return memcmp(pkItemSrc->GetSockets(), pkItemDest->GetSockets(), sizeof(long) * ITEM_SOCKET_MAX_NUM) == 0;
}

bool CHARACTER::CanHandleItem(bool bSkipCheckRefine, bool bSkipObserver)
{
	if (!bSkipObserver)
		if (m_bIsObserver)
			return false;

	if (GetMyShop())
		return false;

	if (!bSkipCheckRefine)
		if (m_bUnderRefine)
			return false;

	if (IsCubeOpen() || NULL != DragonSoul_RefineWindow_GetOpener())
		return false;

	if (IsWarping())
		return false;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if ((m_bAcceCombination) || (m_bAcceAbsorption))
		return false;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	if (IsAuraRefineWindowOpen() || NULL != GetAuraRefineWindowOpener())
		return false;
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	if (GetTransmutation())
		return false;
#endif

	// Fix
	LPDESC desc = GetDesc();
	if (desc != NULL)
	{
		if (desc->IsDisconnectEvent())
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
			return false;
		}
	}

	return true;
}

LPITEM CHARACTER::GetInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}

#ifdef ENABLE_SPECIAL_INVENTORY
LPITEM CHARACTER::GetSkillBookInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}

LPITEM CHARACTER::GetUpgradeItemsInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}

LPITEM CHARACTER::GetStoneInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}

LPITEM CHARACTER::GetGiftBoxInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}

LPITEM CHARACTER::GetChangersInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}
#endif

LPITEM CHARACTER::GetItem(TItemPos Cell) const
{
	if (!IsValidItemPosition(Cell))
		return NULL;
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	switch (window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pItems[wCell];

	case DRAGON_SOUL_INVENTORY:
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid DS item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pDSItems[wCell];

#ifdef ENABLE_RENEWAL_SWITCHBOT
	case SWITCHBOT:
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid switchbot item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pSwitchbotItems[wCell];
#endif

	default:
		return NULL;
	}
	return NULL;
}

#ifdef ENABLE_PICKUP_ITEM_EFFECT
void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem, bool bHighlight)
#else
void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem)
#endif
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	if ((unsigned long)((CItem*)pItem) == 0xff || (unsigned long)((CItem*)pItem) == 0xffffffff)
	{
		sys_err("!!! FATAL ERROR !!! item == 0xff (char: %s cell: %u)", GetName(), wCell);
		core_dump();
		return;
	}

	if (pItem && pItem->GetOwner())
	{
		assert(!"GetOwner exist");
		return;
	}

	switch(window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		{
			if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
			{
				sys_err("CHARACTER::SetItem: invalid item cell %d", wCell);
				return;
			}

			LPITEM pOld = m_pointsInstant.pItems[wCell];

			if (pOld)
			{
				if (wCell < INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
							continue;

						if (m_pointsInstant.pItems[p] && m_pointsInstant.pItems[p] != pOld)
							continue;

						m_pointsInstant.bItemGrid[p] = 0;
					}
				}
				else
					m_pointsInstant.bItemGrid[wCell] = 0;
			}

			if (pItem)
			{
				if (wCell < INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
							continue;

						m_pointsInstant.bItemGrid[p] = wCell + 1;
					}
				}
				else
					m_pointsInstant.bItemGrid[wCell] = wCell + 1;
			}

			m_pointsInstant.pItems[wCell] = pItem;
		}
		break;

	case DRAGON_SOUL_INVENTORY:
		{
			LPITEM pOld = m_pointsInstant.pDSItems[wCell];

			if (pOld)
			{
				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							continue;

						if (m_pointsInstant.pDSItems[p] && m_pointsInstant.pDSItems[p] != pOld)
							continue;

						m_pointsInstant.wDSItemGrid[p] = 0;
					}
				}
				else
					m_pointsInstant.wDSItemGrid[wCell] = 0;
			}

			if (pItem)
			{
				if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					sys_err("CHARACTER::SetItem: invalid DS item cell %d", wCell);
					return;
				}

				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							continue;

						m_pointsInstant.wDSItemGrid[p] = wCell + 1;
					}
				}
				else
					m_pointsInstant.wDSItemGrid[wCell] = wCell + 1;
			}

			m_pointsInstant.pDSItems[wCell] = pItem;
		}
		break;

#ifdef ENABLE_RENEWAL_SWITCHBOT
	case SWITCHBOT:
	{
		LPITEM pOld = m_pointsInstant.pSwitchbotItems[wCell];
		if (pItem && pOld)
		{
			return;
		}

		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::SetItem: invalid switchbot item cell %d", wCell);
			return;
		}

		if (pItem)
		{
			CSwitchbotManager::Instance().RegisterItem(GetPlayerID(), pItem->GetID(), wCell);
		}
		else
		{
			CSwitchbotManager::Instance().UnregisterItem(GetPlayerID(), wCell);
		}

		m_pointsInstant.pSwitchbotItems[wCell] = pItem;
	}
	break;
#endif

	default:
		sys_err ("Invalid Inventory type %d", window_type);
		return;
	}

	if (GetDesc())
	{

		if (pItem)
		{
			TPacketGCItemSet pack;
			pack.header = HEADER_GC_ITEM_SET;
			pack.Cell = Cell;
			pack.count = pItem->GetCount();
			pack.vnum = pItem->GetVnum();
			pack.flags = pItem->GetFlag();
			pack.anti_flags	= pItem->GetAntiFlag();

#ifdef ENABLE_PICKUP_ITEM_EFFECT
			if (bHighlight)
				pack.highlight = true;
			else
				pack.highlight = (Cell.window_type == DRAGON_SOUL_INVENTORY);
#else
			pack.highlight = (Cell.window_type == DRAGON_SOUL_INVENTORY);
#endif

			thecore_memcpy(pack.alSockets, pItem->GetSockets(), sizeof(pack.alSockets));
			thecore_memcpy(pack.aAttr, pItem->GetAttributes(), sizeof(pack.aAttr));
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			pack.dwTransmutationVnum = pItem->GetTransmutationVnum();
#endif

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemSet));
		}
		else
		{
			TPacketGCItemDelDeprecated pack;
			pack.header = HEADER_GC_ITEM_DEL;
			pack.Cell = Cell;
			pack.count = 0;
			pack.vnum = 0;
			memset(pack.alSockets, 0, sizeof(pack.alSockets));
			memset(pack.aAttr, 0, sizeof(pack.aAttr));
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			pack.dwTransmutationVnum = 0;
#endif

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemDelDeprecated));
		}
	}

	if (pItem)
	{
		pItem->SetCell(this, wCell);
		switch (window_type)
		{
			case INVENTORY:
			case EQUIPMENT:
#ifdef ENABLE_SPECIAL_INVENTORY
				if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell) ||
				(SKILL_BOOK_INVENTORY_SLOT_START <= wCell && SKILL_BOOK_INVENTORY_SLOT_END > wCell) ||
				(UPGRADE_ITEMS_INVENTORY_SLOT_START <= wCell && UPGRADE_ITEMS_INVENTORY_SLOT_END > wCell) ||
				(STONE_INVENTORY_SLOT_START <= wCell && STONE_INVENTORY_SLOT_END > wCell) ||
				(GIFT_BOX_INVENTORY_SLOT_START <= wCell && GIFT_BOX_INVENTORY_SLOT_END > wCell) || 
				(CHANGERS_INVENTORY_SLOT_START <= wCell && CHANGERS_INVENTORY_SLOT_END > wCell))
#else
				if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell))
#endif
					pItem->SetWindow(INVENTORY);
				else
					pItem->SetWindow(EQUIPMENT);
				break;

			case DRAGON_SOUL_INVENTORY:
				pItem->SetWindow(DRAGON_SOUL_INVENTORY);
				break;

#ifdef ENABLE_RENEWAL_SWITCHBOT
			case SWITCHBOT:
				pItem->SetWindow(SWITCHBOT);
				break;
#endif
		}
	}
}

LPITEM CHARACTER::GetWear(UINT bCell) const
{
	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::GetWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_pointsInstant.pItems[INVENTORY_MAX_NUM + bCell];
}

void CHARACTER::SetWear(UINT bCell, LPITEM item)
{
	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::SetItem: invalid item cell %d", bCell);
		return;
	}

	SetItem(TItemPos (INVENTORY, INVENTORY_MAX_NUM + bCell), item);

	if (!item && bCell == WEAR_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}
}

void CHARACTER::ClearItem()
{
	int		i;
	LPITEM	item;

	for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		if ((item = GetInventoryItem(i)))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);

			SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
		}
	}

	for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}

#ifdef ENABLE_RENEWAL_SWITCHBOT
	for (i = 0; i < SWITCHBOT_SLOT_COUNT; ++i)
	{
		if ((item = GetItem(TItemPos(SWITCHBOT, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
}

bool CHARACTER::IsEmptyItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell) const
{
	switch (Cell.window_type)
	{
		case INVENTORY:
			{
				UINT bCell = Cell.cell;

				++iExceptionCell;

				if (Cell.IsBeltInventoryPosition())
				{
#ifdef DEFAULT_BELT_SYSTEM
					LPITEM beltItem = GetWear(WEAR_BELT);

					if (NULL == beltItem)
						return false;

					if (false == CBeltInventoryHelper::IsAvailableCell(bCell - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
						return false;
#endif

					if (m_pointsInstant.bItemGrid[bCell])
					{
						if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
							return true;

						return false;
					}

					if (bSize == 1)
						return true;
				}
#ifdef ENABLE_SPECIAL_INVENTORY
				else if (Cell.IsSkillBookInventoryPosition())
				{
					if (bCell < SKILL_BOOK_INVENTORY_SLOT_START)
					{
						return false;
					}

					if (bCell > SKILL_BOOK_INVENTORY_SLOT_END)
						return false;

					if (m_pointsInstant.bItemGrid[bCell])
					{
						if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
						{
							if (bSize == 1)
								return true;

							int j = 1;
							UINT bPage = (bCell - SKILL_BOOK_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT);
							do
							{
								UINT p = bCell + (5 * j);

								if (p >= SKILL_BOOK_INVENTORY_SLOT_END)
									return false;

								if ((p-SKILL_BOOK_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT) != bPage)
								{
									return false;
								}

								if (m_pointsInstant.bItemGrid[p])
								{
									if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
									{
										return false;
									}
								}
							}
							while (++j < bSize);
							return true;
						}
						else
						{
							return false;
						}
					}

					if (1 == bSize)
						return true;
					else
					{
						int j = 1;
						UINT bPage = (bCell - SKILL_BOOK_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT);
						do
						{
							UINT p = bCell + (5 * j);

							if (p >= SKILL_BOOK_INVENTORY_SLOT_END)
								return false;

							if ((p - SKILL_BOOK_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT) != bPage)
							{
								return false;
							}

							if (m_pointsInstant.bItemGrid[p])
							{
								if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
								{
									return false;
								}
							}
						}
						while (++j < bSize);
						return true;
					}
				}
				else if (Cell.IsUpgradeItemsInventoryPosition())
				{
					if (bCell < UPGRADE_ITEMS_INVENTORY_SLOT_START)
						return false;

					if (bCell > UPGRADE_ITEMS_INVENTORY_SLOT_END)
						return false;

					if (m_pointsInstant.bItemGrid[bCell])
					{
						if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
						{
							if (bSize == 1)
								return true;

							int j = 1;
							BYTE bPage = (bCell - UPGRADE_ITEMS_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT);

							do
							{
								UINT p = bCell + (5 * j);

								if (p >= UPGRADE_ITEMS_INVENTORY_SLOT_END)
									return false;

								if ((p - UPGRADE_ITEMS_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT) != bPage)
									return false;

								if (m_pointsInstant.bItemGrid[p])
								{
									if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
									{
										return false;
									}
								}
							}
							while (++j < bSize);
							return true;
						}
						else
							return false;
					}

					if (1 == bSize)
						return true;
					else
					{
						int j = 1;
						BYTE bPage = (bCell - UPGRADE_ITEMS_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT);

						do
						{
							UINT p = bCell + (5 * j);

							if (p >= UPGRADE_ITEMS_INVENTORY_SLOT_END)
								return false;

							if ((p - UPGRADE_ITEMS_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT) != bPage)
								return false;

							if (m_pointsInstant.bItemGrid[p])
							{
								if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
								{
									return false;
								}
							}
						}
						while (++j < bSize);
						return true;
					}
				}
				else if (Cell.IsStoneInventoryPosition())
				{
					if (bCell < STONE_INVENTORY_SLOT_START)
						return false;

					if (bCell > STONE_INVENTORY_SLOT_END)
						return false;

					if (m_pointsInstant.bItemGrid[bCell])
					{
						if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
						{
							if (bSize == 1)
								return true;

							int j = 1;
							BYTE bPage = (bCell - STONE_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT);

							do
							{
								UINT p = bCell + (5 * j);

								if (p >= STONE_INVENTORY_SLOT_END)
									return false;

								if ((p - STONE_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT) != bPage)
									return false;

								if (m_pointsInstant.bItemGrid[p])
								{
									if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
									{
										return false;
									}
								}
							}
							while (++j < bSize);
							return true;
						}
						else
							return false;
					}

					if (1 == bSize)
						return true;
					else
					{
						int j = 1;
						BYTE bPage = (bCell - STONE_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT);

						do
						{
							UINT p = bCell + (5 * j);

							if (p >= STONE_INVENTORY_SLOT_END)
								return false;

							if ((p - STONE_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT) != bPage)
								return false;

							if (m_pointsInstant.bItemGrid[p])
							{
								if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
								{
									return false;
								}
							}
						}
						while (++j < bSize);
						return true;
					}
				}
				else if (Cell.IsGiftBoxInventoryPosition())
				{
					if (bCell < GIFT_BOX_INVENTORY_SLOT_START)
						return false;

					if (bCell > GIFT_BOX_INVENTORY_SLOT_END)
						return false;

					if (m_pointsInstant.bItemGrid[bCell])
					{
						if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
						{
							if (bSize == 1)
								return true;

							int j = 1;
							BYTE bPage = (bCell-GIFT_BOX_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT);

							do
							{
								UINT p = bCell + (5 * j);

								if (p >= GIFT_BOX_INVENTORY_SLOT_END)
									return false;

								if ((p - GIFT_BOX_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT) != bPage)
									return false;

								if (m_pointsInstant.bItemGrid[p])
								{
									if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
									{
										return false;
									}
								}
							}
							while (++j < bSize);
							return true;
						}
						else
							return false;
					}

					if (1 == bSize)
						return true;
					else
					{
						int j = 1;
						BYTE bPage = (bCell-GIFT_BOX_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT);

						do
						{
							UINT p = bCell + (5 * j);

							if (p >= GIFT_BOX_INVENTORY_SLOT_END)
								return false;

							if ((p - GIFT_BOX_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT) != bPage)
								return false;

							if (m_pointsInstant.bItemGrid[p])
							{
								if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
								{
									return false;
								}
							}
						}
						while (++j < bSize);
						return true;
					}
				}
				else if (Cell.IsChangersInventoryPosition())
				{
					if (bCell < CHANGERS_INVENTORY_SLOT_START)
						return false;

					if (bCell > CHANGERS_INVENTORY_SLOT_END)
						return false;

					if (m_pointsInstant.bItemGrid[bCell])
					{
						if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
						{
							if (bSize == 1)
								return true;

							int j = 1;
							BYTE bPage = (bCell-CHANGERS_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT);

							do
							{
								UINT p = bCell + (5 * j);

								if (p >= CHANGERS_INVENTORY_SLOT_END)
									return false;

								if ((p - CHANGERS_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT) != bPage)
									return false;

								if (m_pointsInstant.bItemGrid[p])
								{
									if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
									{
										return false;
									}
								}
							}
							while (++j < bSize);
							return true;
						}
						else
							return false;
					}

					if (1 == bSize)
						return true;
					else
					{
						int j = 1;
						BYTE bPage = (bCell-CHANGERS_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT);

						do
						{
							UINT p = bCell + (5 * j);

							if (p >= CHANGERS_INVENTORY_SLOT_END)
								return false;

							if ((p - CHANGERS_INVENTORY_SLOT_START) / (SPECIAL_INVENTORY_MAX_NUM / SPECIAL_INVENTORY_PAGE_COUNT) != bPage)
								return false;

							if (m_pointsInstant.bItemGrid[p])
							{
								if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
								{
									return false;
								}
							}
						}
						while (++j < bSize);
						return true;
					}
				}
#endif
#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
				else if (bCell >= 90+(5*GetUnlockSlotsW()))
					return false;
				else if (bSize == 2)
				{
					if (bCell+5 >= 90+(5*GetUnlockSlotsW()))
						return false;
				}
				else if (bSize == 3)
				{
					if (bCell+10 >= 90+(5*GetUnlockSlotsW()))
						return false;
				}
#else
				else if (bCell >= INVENTORY_MAX_NUM)
					return false;
#endif

				if (m_pointsInstant.bItemGrid[bCell])
				{
					if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
					{
						if (bSize == 1)
							return true;

						int j = 1;
						BYTE bPage = bCell / (INVENTORY_MAX_NUM / 4);

						do
						{
							BYTE p = bCell + (5 * j);

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
							if (p >= 90+(5*GetUnlockSlotsW()))
								return false;
#else
							if (p >= INVENTORY_MAX_NUM)
								return false;
#endif

							if (p / (INVENTORY_MAX_NUM / 4) != bPage)
								return false;

							if (m_pointsInstant.bItemGrid[p])
								if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
									return false;
						}
						while (++j < bSize);

						return true;
					}
					else
						return false;
				}

				if (1 == bSize)
					return true;
				else
				{
					int j = 1;
					BYTE bPage = bCell / (INVENTORY_MAX_NUM / 4);

					do
					{
						BYTE p = bCell + (5 * j);

						if (p >= INVENTORY_MAX_NUM)
							return false;

						if (p / (INVENTORY_MAX_NUM / 4) != bPage)
							return false;

						if (m_pointsInstant.bItemGrid[p])
							if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
								return false;
					}
					while (++j < bSize);

					return true;
				}
			}
			break;

		case DRAGON_SOUL_INVENTORY:
			{
				WORD wCell = Cell.cell;
				if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
					return false;

				iExceptionCell++;

				if (m_pointsInstant.wDSItemGrid[wCell])
				{
					if (m_pointsInstant.wDSItemGrid[wCell] == iExceptionCell)
					{
						if (bSize == 1)
							return true;

						int j = 1;

						do
						{
							BYTE p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

							if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
								return false;

							if (m_pointsInstant.wDSItemGrid[p])
								if (m_pointsInstant.wDSItemGrid[p] != iExceptionCell)
									return false;
						}
						while (++j < bSize);

						return true;
					}
					else
						return false;
				}

				if (1 == bSize)
					return true;
				else
				{
					int j = 1;

					do
					{
						BYTE p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							return false;

						if (m_pointsInstant.bItemGrid[p])
							if (m_pointsInstant.wDSItemGrid[p] != iExceptionCell)
								return false;
					}
					while (++j < bSize);

					return true;
				}
			}

#ifdef ENABLE_RENEWAL_SWITCHBOT
		case SWITCHBOT:
			{
				WORD wCell = Cell.cell;
				if (wCell >= SWITCHBOT_SLOT_COUNT)
				{
					return false;
				}

				if (m_pointsInstant.pSwitchbotItems[wCell])
				{
					return false;
				}

				return true;
			}
#endif

	}
}

int CHARACTER::GetEmptyInventory(BYTE size) const
{
#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	for (int i = 0; i < 90+(5*GetUnlockSlotsW()); ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
		if (IsEmptyItemGrid(TItemPos (INVENTORY, i), size))
			return i;
	return -1;
}

int CHARACTER::GetEmptyDragonSoulInventory(LPITEM pItem) const
{
	if (NULL == pItem || !pItem->IsDragonSoul())
		return -1;

	if (!DragonSoul_IsQualified())
	{
		return -1;
	}

	BYTE bSize = pItem->GetSize();
	WORD wBaseCell = DSManager::instance().GetBasePosition(pItem);

	if (WORD_MAX == wBaseCell)
		return -1;

	for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
		if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
			return i + wBaseCell;

	return -1;
}

void CHARACTER::CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const
{
	vDragonSoulItemGrid.resize(DRAGON_SOUL_INVENTORY_MAX_NUM);

	std::copy(m_pointsInstant.wDSItemGrid, m_pointsInstant.wDSItemGrid + DRAGON_SOUL_INVENTORY_MAX_NUM, vDragonSoulItemGrid.begin());
}

#ifdef ENABLE_SPECIAL_INVENTORY
int CHARACTER::GetEmptySkillBookInventory(BYTE size) const
{
	for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return GetEmptyInventory(size);
}

int CHARACTER::GetEmptyUpgradeItemsInventory(BYTE size) const
{
	for (int i = UPGRADE_ITEMS_INVENTORY_SLOT_START; i < UPGRADE_ITEMS_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return GetEmptyInventory(size);
}

int CHARACTER::GetEmptyStoneInventory(BYTE size) const
{
	for (int i = STONE_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return GetEmptyInventory(size);
}

int CHARACTER::GetEmptyGiftBoxInventory(BYTE size) const
{
	for (int i = GIFT_BOX_INVENTORY_SLOT_START; i < GIFT_BOX_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return GetEmptyInventory(size);
}

int CHARACTER::GetEmptyChangersInventory(BYTE size) const
{
	for (int i = CHANGERS_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			return i;

	return GetEmptyInventory(size);
}
#endif

int CHARACTER::CountEmptyInventory() const
{
	int count = 0;

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	int slot = 90+(5*GetUnlockSlotsW());
	for (int i = 0; i < 90+(5*GetUnlockSlotsW()); ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
		if (GetInventoryItem(i))
			count += GetInventoryItem(i)->GetSize();

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	return (slot - count);
#else
	return (INVENTORY_MAX_NUM - count);
#endif
}

void TransformRefineItem(LPITEM pkOldItem, LPITEM pkNewItem)
{
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
		}
	}
	else
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			if (!pkOldItem->GetSocket(i))
				break;
			else
				pkNewItem->SetSocket(i, 1);
		}

		int slot = 0;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			long socket = pkOldItem->GetSocket(i);

			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				pkNewItem->SetSocket(slot++, socket);
		}
	}

	pkOldItem->CopyAttributeTo(pkNewItem);
}

void NotifyRefineSuccess(LPCHARACTER ch, LPITEM item, const char* way)
{
	if (NULL != ch && item != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineSuceeded");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), 1, way);
	}
}

void NotifyRefineFail(LPCHARACTER ch, LPITEM item, const char* way, int success = 0)
{
	if (NULL != ch && NULL != item)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailed");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), success, way);
	}
}

void CHARACTER::SetRefineNPC(LPCHARACTER ch)
{
	if ( ch != NULL )
	{
		m_dwRefineNPCVID = ch->GetVID();
	}
	else
	{
		m_dwRefineNPCVID = 0;
	}
}

bool CHARACTER::DoRefine(LPITEM item, bool bMoneyOnly)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (GetActiveGrowthPet())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 737, "");
		return false;
	}
#endif

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	DWORD result_vnum = item->GetRefinedVnum();

	int cost = ComputeRefineFee(prt->cost);

	int RefineChance = GetQuestFlag("main_quest_lv7.refine_chance");

	if (RefineChance > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 407, "");
			return false;
		}

		cost = 0;
		SetQuestFlag("main_quest_lv7.refine_chance", RefineChance - 1);
	}

	if (result_vnum == 0)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 408, "");
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
		return false;

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefine NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		LocaleChatPacket(CHAT_TYPE_INFO, 409, "");
		return false;
	}

	if (GetGold() < cost)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 70, "");
		return false;
	}

	if (!bMoneyOnly && !RefineChance)
	{
		for (int i = 0; i < prt->material_count; ++i)
		{
#ifdef ENABLE_PENDANT_SYSTEM
			if (CountSpecifyItem(prt->materials[i].vnum, item->GetCell()) < prt->materials[i].count)
#else
			if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
#endif
			{
				if (test_server)
					ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);

				LocaleChatPacket(CHAT_TYPE_INFO, 226, "");
				return false;
			}
		}

		for (int i = 0; i < prt->material_count; ++i)
#ifdef ENABLE_PENDANT_SYSTEM
			RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count, item->GetCell());
#else
			RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);
#endif
	}

	int prob = number(1, 100);

	if (IsRefineThroughGuild() || bMoneyOnly)
		prob -= 10;

	if (prob <= prt->prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
#ifdef ENABLE_RENEWAL_BATTLE_PASS
			UpdateExtBattlePassMissionProgress(BP_ITEM_REFINE, 1, item->GetVnum());
#endif

			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			UINT bCell = item->GetCell();

			NotifyRefineSuccess(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell)); 
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			sys_log(0, "Refine Success %d", cost);
			pkNewItem->AttrLog();
			sys_log(0, "PayPee %d", cost);
			PayRefineFee(cost);
			sys_log(0, "PayPee End %d", cost);
		}
		else
		{
			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
		}
	}
	else
	{
		DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
		NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
		item->AttrLog();
		ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

		PayRefineFee(cost);
	}

	return true;
}

enum enum_RefineScrolls
{
	CHUKBOK_SCROLL = 0,
	HYUNIRON_CHN = 1,
	YONGSIN_SCROLL = 2,
	MUSIN_SCROLL = 3,
	YAGONG_SCROLL = 4,
	MEMO_SCROLL = 5,
	BDRAGON_SCROLL = 6,
};

bool CHARACTER::DoRefineWithScroll(LPITEM item)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	ClearRefineMode();

	if (quest::CQuestManager::instance().GetEventFlag("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag("update_refine_time") + (60 * 5))
		{
			sys_log(0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (GetActiveGrowthPet())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 737, "");
		return false;
	}
#endif

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	LPITEM pkItemScroll;

	if (m_iRefineAdditionalCell < 0)
		return false;

	pkItemScroll = GetInventoryItem(m_iRefineAdditionalCell);

	if (!pkItemScroll)
		return false;

	if (!(pkItemScroll->GetType() == ITEM_USE && pkItemScroll->GetSubType() == USE_TUNING))
		return false;

	if (pkItemScroll->GetVnum() == item->GetVnum())
		return false;

	DWORD result_vnum = item->GetRefinedVnum();
	DWORD result_fail_vnum = item->GetRefineFromVnum();

	if (result_vnum == 0)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 408, "");
		return false;
	}

	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		if (item->GetRefineLevel() >= 4)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 410, "");
			return false;
		}
	}
	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		if (item->GetRefineLevel() != pkItemScroll->GetValue(1))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 411, "");
			return false;
		}
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		if (item->GetType() != ITEM_METIN || item->GetRefineLevel() != 4)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 409, "");
			return false;
		}
	}

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefineWithScroll NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		LocaleChatPacket(CHAT_TYPE_INFO, 409, "");
		return false;
	}

	if (GetGold() < prt->cost)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 70, "");
		return false;
	}

	for (int i = 0; i < prt->material_count; ++i)
	{
#ifdef ENABLE_PENDANT_SYSTEM
		if (CountSpecifyItem(prt->materials[i].vnum, item->GetCell()) < prt->materials[i].count)
#else
		if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
#endif
		{
			if (test_server)
				ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);

			LocaleChatPacket(CHAT_TYPE_INFO, 226, "");
			return false;
		}
	}

	for (int i = 0; i < prt->material_count; ++i)
#ifdef ENABLE_PENDANT_SYSTEM
		RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count, item->GetCell());
#else
		RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);
#endif

	int prob = number(1, 100);
	int success_prob = prt->prob;
	bool bDestroyWhenFail = false;

	const char* szRefineType = "SCROLL";

	if (pkItemScroll->GetValue(0) == HYUNIRON_CHN || 
		pkItemScroll->GetValue(0) == YONGSIN_SCROLL || 
		pkItemScroll->GetValue(0) == YAGONG_SCROLL)
	{
		const char hyuniron_prob[9] = { 100, 75, 65, 55, 45, 40, 35, 25, 20 };
		const char yagong_prob[9] = { 100, 100, 90, 80, 70, 60, 50, 30, 20 };

		if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			success_prob = hyuniron_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			success_prob = yagong_prob[MINMAX(0, item->GetRefineLevel(), 8)];
		}
		else
		{
			sys_err("REFINE : Unknown refine scroll item. Value0: %d", pkItemScroll->GetValue(0));
		}

		if (test_server) 
		{
			ChatPacket(CHAT_TYPE_INFO, "[Only Test] Success_Prob %d, RefineLevel %d ", success_prob, item->GetRefineLevel());
		}
		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
			bDestroyWhenFail = true;

		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
		{
			szRefineType = "HYUNIRON";
		}
		else if (pkItemScroll->GetValue(0) == YONGSIN_SCROLL)
		{
			szRefineType = "GOD_SCROLL";
		}
		else if (pkItemScroll->GetValue(0) == YAGONG_SCROLL)
		{
			szRefineType = "YAGONG_SCROLL";
		}
	}

	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		success_prob = 100;

		szRefineType = "MUSIN_SCROLL";
	}

	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		success_prob = 100;
		szRefineType = "MEMO_SCROLL";
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		success_prob = 80;
		szRefineType = "BDRAGON_SCROLL";
	}

	pkItemScroll->SetCount(pkItemScroll->GetCount() - 1);

	if (prob <= success_prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
#ifdef ENABLE_RENEWAL_BATTLE_PASS
			UpdateExtBattlePassMissionProgress(BP_ITEM_REFINE, 1, item->GetVnum());
#endif

			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			UINT bCell = item->GetCell();

			NotifyRefineSuccess(this, item, szRefineType);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell)); 
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			pkNewItem->AttrLog();
			PayRefineFee(prt->cost);
		}
		else
		{
			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else if (!bDestroyWhenFail && result_fail_vnum)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_fail_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE FAIL", pkNewItem->GetName());

			UINT bCell = item->GetCell();

			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			NotifyRefineFail(this, item, szRefineType, -1);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell)); 
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			pkNewItem->AttrLog();
			PayRefineFee(prt->cost);
		}
		else
		{
			sys_err("cannot create item %u", result_fail_vnum);
			NotifyRefineFail(this, item, szRefineType);
		}
	}
	else
	{
		NotifyRefineFail(this, item, szRefineType);
		
		PayRefineFee(prt->cost);
	}

	return true;
}

bool CHARACTER::RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell)
{
	if (bCell > INVENTORY_MAX_NUM)
		return false;

	LPITEM item = GetInventoryItem(bCell);

	if (!item)
		return false;

	if (bType == REFINE_TYPE_MONEY_ONLY && !GetQuestFlag("deviltower_zone.can_refine"))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 117, "");
		return false;
	}

	TPacketGCRefineInformation p;

	p.header = HEADER_GC_REFINE_INFORMATION;
	p.pos = bCell;
	p.src_vnum = item->GetVnum();
	p.result_vnum = item->GetRefinedVnum();
	p.type = bType;

	if (p.result_vnum == 0)
	{
		sys_err("RefineInformation p.result_vnum == 0");
		LocaleChatPacket(CHAT_TYPE_INFO, 409, "");
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		if (bType == 0)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 358, "");
			return false;
		}
		else
		{
			LPITEM itemScroll = GetInventoryItem(iAdditionalCell);
			if (!itemScroll || item->GetVnum() == itemScroll->GetVnum())
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 412, "");
				return false;
			}
		}
	}

	CRefineManager & rm = CRefineManager::instance();

	const TRefineTable* prt = rm.GetRefineRecipe(item->GetRefineSet());

	if (!prt)
	{
		sys_err("RefineInformation NOT GET REFINE SET %d", item->GetRefineSet());
		LocaleChatPacket(CHAT_TYPE_INFO, 409, "");
		return false;
	}

	if (GetQuestFlag("main_quest_lv7.refine_chance") > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 407, "");
			return false;
		}
		p.cost = 0;
	}
	else
		p.cost = ComputeRefineFee(prt->cost);

	p.prob = prt->prob;
	if (bType == REFINE_TYPE_MONEY_ONLY)
	{
		p.material_count = 0;
		memset(p.materials, 0, sizeof(p.materials));
	}
	else
	{
		p.material_count = prt->material_count;
		thecore_memcpy(&p.materials, prt->materials, sizeof(prt->materials));
	}

	GetDesc()->Packet(&p, sizeof(TPacketGCRefineInformation));

	SetRefineMode(iAdditionalCell);
	return true;
}

bool CHARACTER::RefineItem(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;

	if (pkItem->GetSubType() == USE_TUNING)
	{
		if (pkItem->GetValue(0) == MUSIN_SCROLL)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_MUSIN, pkItem->GetCell());
		else if (pkItem->GetValue(0) == HYUNIRON_CHN)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_HYUNIRON, pkItem->GetCell());
		else if (pkItem->GetValue(0) == BDRAGON_SCROLL)
		{
			if (pkTarget->GetRefineSet() != 702) return false;
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_BDRAGON, pkItem->GetCell());
		}
		else
		{
			if (pkTarget->GetRefineSet() == 501) return false;
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_SCROLL, pkItem->GetCell());
		}
	}
	else if (pkItem->GetSubType() == USE_DETACHMENT && IS_SET(pkTarget->GetFlag(), ITEM_FLAG_REFINEABLE))
	{
		LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT", pkTarget->GetName());

		bool bHasMetinStone = false;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		{
			long socket = pkTarget->GetSocket(i);
			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
			{
				bHasMetinStone = true;
				break;
			}
		}

		if (bHasMetinStone)
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			{
				long socket = pkTarget->GetSocket(i);
				if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				{
					AutoGiveItem(socket);
					pkTarget->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
				}
			}
			pkItem->SetCount(pkItem->GetCount() - 1);
			return true;
		}
		else
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 413, "");
			return false;
		}
	}

	return false;
}

EVENTFUNC(kill_campfire_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "kill_campfire_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL)
	{
		return 0;
	}

	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

bool CHARACTER::GiveRecallItem(LPITEM item)
{
	int idx = GetMapIndex();
	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 414, "");
		return false;
	}

	int pos;

	if (item->GetCount() == 1)
	{
		item->SetSocket(0, GetX());
		item->SetSocket(1, GetY());
	}
	else if ((pos = GetEmptyInventory(item->GetSize())) != -1)
	{
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);

		if (NULL != item2)
		{
			item2->SetSocket(0, GetX());
			item2->SetSocket(1, GetY());
			item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

			item->SetCount(item->GetCount() - 1);
		}
	}
	else
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
		return false;
	}

	return true;
}

void CHARACTER::ProcessRecallItem(LPITEM item)
{
	int idx;

	if ((idx = SECTREE_MANAGER::instance().GetMapIndex(item->GetSocket(0), item->GetSocket(1))) == 0)
		return;

	int iEmpireByMapIndex = -1;

	if (idx < 20)
		iEmpireByMapIndex = 1;
	else if (idx < 40)
		iEmpireByMapIndex = 2;
	else if (idx < 60)
		iEmpireByMapIndex = 3;
	else if (idx < 10000)
		iEmpireByMapIndex = 0;

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;

		case 301:
		case 302:
		case 303:
		case 304:
			if( GetLevel() < 90 )
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 415, "");
				return;
			}
			else
				break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 416, "");
		item->SetSocket(0, 0);
		item->SetSocket(1, 0);
	}
	else
	{
		sys_log(1, "Recall: %s %d %d -> %d %d", GetName(), GetX(), GetY(), item->GetSocket(0), item->GetSocket(1));
		WarpSet(item->GetSocket(0), item->GetSocket(1));
		item->SetCount(item->GetCount() - 1);
	}
}

void CHARACTER::__OpenPrivateShop()
{
	unsigned bodyPart = GetPart(PART_MAIN);
	switch (bodyPart)
	{
		case 0:
		case 1:
		case 2:
			ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
			break;
		default:
			LocaleChatPacket(CHAT_TYPE_INFO, 17, "");
			break;
	}
}

#ifdef ENABLE_GOLD_LIMIT
void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, long long dwItemPrice)
#else
void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice)
#endif
{
	char szLine[256];
#ifdef ENABLE_GOLD_LIMIT
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %lld", dwItemVnum, dwItemPrice);
#else
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %u", dwItemVnum, dwItemPrice);
#endif
	ChatPacket(CHAT_TYPE_COMMAND, szLine);
	sys_log(0, szLine);
}

void CHARACTER::UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p)
{
	const TItemPriceInfo* pInfo = (const TItemPriceInfo*)(p + 1);

	if (!p->byCount)
		SendMyShopPriceListCmd(1, 0);
	else {
		for (int idx = 0; idx < p->byCount; idx++)
			SendMyShopPriceListCmd(pInfo[ idx ].dwVnum, pInfo[ idx ].dwPrice);
	}

	__OpenPrivateShop();
}

void CHARACTER::UseSilkBotary(void)
{
	if (m_bNoOpenedShop) {
		DWORD dwPlayerID = GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_REQ, GetDesc()->GetHandle(), &dwPlayerID, sizeof(DWORD));
		m_bNoOpenedShop = false;
	} else {
		__OpenPrivateShop();
	}
}


int CalculateConsume(LPCHARACTER ch)
{
	static const int WARP_NEED_LIFE_PERCENT	= 30;
	static const int WARP_MIN_LIFE_PERCENT	= 10;

	int consumeLife = 0;
	{
		const int curLife = ch->GetHP();
		const int needPercent = WARP_NEED_LIFE_PERCENT;
		const int needLife = ch->GetMaxHP() * needPercent / 100;

		if (curLife < needLife)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 417, "");
			return -1;
		}

		consumeLife = needLife;

		const int minPercent = WARP_MIN_LIFE_PERCENT;
		const int minLife = ch->GetMaxHP() * minPercent / 100;

		if (curLife - needLife < minLife)
			consumeLife = curLife - minLife;

		if (consumeLife < 0)
			consumeLife = 0;
	}

	return consumeLife;
}

int CalculateConsumeSP(LPCHARACTER lpChar)
{
	static const int NEED_WARP_SP_PERCENT = 30;

	const int curSP = lpChar->GetSP();
	const int needSP = lpChar->GetMaxSP() * NEED_WARP_SP_PERCENT / 100;

	if (curSP < needSP)
	{
		lpChar->LocaleChatPacket(CHAT_TYPE_INFO, 418, "");
		return -1;
	}

	return needSP;
}

// Fix
bool CHARACTER::CheckTimeUsed(LPITEM item)
{
	if (item->GetType() == ITEM_BLEND
		|| (item->GetType() == ITEM_USE && item->GetSubType() == USE_ABILITY_UP)
		|| (item->GetType() == ITEM_USE && item->GetSubType() == USE_AFFECT)
	)
	{
		int pGetTime[] = {10};
		int pGetFlag = GetQuestFlag("item.last_time");

		if (pGetFlag)
		{
			if (get_global_time() < pGetFlag + pGetTime[0])
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 419, "%s#%u", item->GetLocaleName(), pGetTime[0]);
				return false;
			}
		}
	}
	return true;
}

bool CHARACTER::UseItemEx(LPITEM item, TItemPos DestCell)
{
	int iLimitRealtimeStartFirstUseFlagIndex = -1;
	int iLimitTimerBasedOnWearFlagIndex = -1;

	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limitValue)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 415, "");
					return false;
				}
				break;

			case LIMIT_REAL_TIME_START_FIRST_USE:
				iLimitRealtimeStartFirstUseFlagIndex = i;
				break;

			case LIMIT_TIMER_BASED_ON_WEAR:
				iLimitTimerBasedOnWearFlagIndex = i;
				break;
		}
	}

	if (test_server)
	{
		sys_log(0, "USE_ITEM %s, Inven %d, Cell %d, ItemType %d, SubType %d", item->GetName(), bDestInven, wDestCell, item->GetType(), item->GetSubType());
	}

	if (CArenaManager::instance().IsLimitedItem( GetMapIndex(), item->GetVnum() ) == true)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
		return false;
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (item->GetType() == ITEM_GROWTH_PET && item->GetSubType() == PET_BAG)
		iLimitRealtimeStartFirstUseFlagIndex = -1;
#endif

	if (-1 != iLimitRealtimeStartFirstUseFlagIndex)
	{
		if (0 == item->GetSocket(1))
		{
			long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[iLimitRealtimeStartFirstUseFlagIndex].lValue;

			if (0 == duration)
				duration = 60 * 60 * 24 * 7;

			item->SetSocket(0, time(0) + duration);
			item->StartRealTimeExpireEvent();
		}

		if (false == item->IsEquipped())
			item->SetSocket(1, item->GetSocket(1) + 1);
	}

	switch (item->GetType())
	{
#ifdef ENABLE_GROWTH_PET_SYSTEM
		case ITEM_GROWTH_PET:
			{
				switch (item->GetSubType())
				{
					case PET_UPBRINGING:
					{
						LPGROWTH_PET pPet = GetGrowthPet(item->GetSocket(2));
						if (pPet)
						{
							if (m_activeGrowthPet && m_activeGrowthPet->GetPetID() == pPet->GetPetID())
							{
								m_activeGrowthPet->Unsummon();
								m_activeGrowthPet = NULL;
								return true;
							}
							else if (m_activeGrowthPet)
								m_activeGrowthPet->Unsummon();

							m_activeGrowthPet = pPet->Summon(item);
						}
					}
					break;

					case PET_FEEDSTUFF:
					{
						LPITEM item2;

						if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
							return false;

						if (item2->GetType() != ITEM_GROWTH_PET && item2->GetSubType() != PET_UPBRINGING)
							return false;

						LPGROWTH_PET pPet = GetGrowthPet(item2->GetSocket(2));
						if (!pPet)
							return false;

						if (item2->GetSocket(0) < get_global_time())
						{
							pPet->Revive(item2, PET_REVIVE_TYPE_NORMAL);
							ITEM_MANAGER::instance().RemoveItem(item);
						}
						else
						{
							pPet->RewardFood(item, PET_FEED_TYPE_FULL);
						}
					}
					break;

					case PET_BAG:
					{
						LPITEM item2 = GetItem(DestCell);

						if (item2)
						{
							if (item2->GetType() != ITEM_GROWTH_PET)
								return false;

							if (item2->GetSubType() != PET_UPBRINGING && item2->GetSubType() != PET_BAG)
								return false;

							switch (item2->GetSubType())
							{
								case PET_UPBRINGING:
								{
									LPGROWTH_PET pPet = GetGrowthPet(item2->GetSocket(2));
									if (!pPet)
										return false;

									pPet->SetState(STATE_BAG);
									pPet->SetSummonItem(NULL);
									pPet->Save();

									time_t currentTime = time(0);
									item->SetSocket(0, currentTime + item->GetLimitValue(0));
									item->SetSocket(1, time(0));
									item->SetSocket(2, pPet->GetPetID());
									item->StartRealTimeExpireEvent();

									ITEM_MANAGER::instance().RemoveItem(item2, "REMOVE (UPBRINGING-BAG TRANSFER)");
								}
								break;

								case PET_BAG:
								{
									LPGROWTH_PET pPet = GetGrowthPet(item2->GetSocket(2));
									if (!pPet)
										return false;

									time_t currentTime = time(0);
									DWORD dwBagTime = currentTime - (item2->GetSocket(0) - item2->GetLimitValue(0));
									DWORD dwPetAge = currentTime - pPet->GetPetPoint(POINT_UPBRINGING_BIRTHDAY) - dwBagTime;

									pPet->ChangePetPoint(POINT_UPBRINGING_BIRTHDAY, currentTime - dwPetAge, true);
									pPet->Save();

									item2->SetSocket(0, currentTime + item2->GetLimitValue(0));
									item2->StartRealTimeExpireEvent();

									ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (BAG-BAG TRANSFER)");
								}
								break;
							}
						}
						else
						{
							if (!item->GetSocket(2))
								return false;

							LPGROWTH_PET pPet = GetGrowthPet(item->GetSocket(2));
							if (!pPet)
								return false;

							time_t currentTime = time(0);
							DWORD dwBagTime = item->GetSocket(0) - item->GetLimitValue(0);
							DWORD dwPetAge = currentTime - pPet->GetPetPoint(POINT_UPBRINGING_BIRTHDAY) - (currentTime - dwBagTime);
							DWORD dwPetDuration = currentTime + pPet->GetPetPoint(POINT_UPBRINGING_MAX_DURATION);
							LPITEM pUpBringingItem = AutoGiveItem(pPet->GetSummonItemVnum());

							TGrowthPet petTable;
							pPet->CreateGrowthPetProto(&petTable);
							petTable.dwID = pUpBringingItem->GetID();
							petTable.lBirthday = currentTime - dwPetAge;
							petTable.lEndTime = dwPetDuration;
							petTable.bState = STATE_UPBRINGING;

							LPGROWTH_PET pNewPet = CGrowthPetManager::Instance().CreateGrowthPet(this, pUpBringingItem->GetID());
							pNewPet->SetGrowthPetProto(&petTable);

							pUpBringingItem->SetSocket(0, dwPetDuration);
							pUpBringingItem->SetSocket(1, petTable.lMaxTime);
							pUpBringingItem->SetSocket(2, pUpBringingItem->GetID());

							pPet->SetSummonItem(pUpBringingItem);
							pPet->Save();

							SetGrowthPet(pNewPet);

							CGrowthPetManager::Instance().DeleteGrowthPet(item->GetSocket(2), true);
							ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (BAG-UPBRINGING TRANSFER)");
						}
					}
					break;
				}
			}
			break;
#endif

		case ITEM_HAIR:
			return ItemProcess_Hair(item, wDestCell);

		case ITEM_POLYMORPH:
			return ItemProcess_Polymorph(item);

		case ITEM_QUEST:
#ifdef __AUTO_HUNT__
			if (item->GetVnum() == 49324)
			{
				if (FindAffect(AFFECT_AUTO_HUNT))
				{
					ChatPacket(CHAT_TYPE_INFO, "You already has affect");
					return false;
				}
				AddAffect(AFFECT_AUTO_HUNT, POINT_NONE, 0, AFF_NONE, 60 * 60 * 24 * item->GetValue(0), 0, false);
				ChatPacket(CHAT_TYPE_INFO, "Affect added for %d day.", item->GetValue(0));
				item->SetCount(item->GetCount() - 1);
				return true;
			}
#endif
			if (GetArena() != NULL || IsObserverMode() == true)
			{
				if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
					return false;
				}
			}

			if (!IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE | ITEM_FLAG_QUEST_USE_MULTIPLE))
			{
				if (item->GetSIGVnum() == 0)
				{
					quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
				}
				else
				{
					quest::CQuestManager::instance().SIGUse(GetPlayerID(), item->GetSIGVnum(), item, false);
				}
			}
			break;

		case ITEM_CAMPFIRE:
			{
				float fx, fy;
				GetDeltaByDegree(GetRotation(), 100.0f, &fx, &fy);

				LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy));

				if (!tree)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 420, "");
					return false;
				}

				if (tree->IsAttr((long)(GetX()+fx), (long)(GetY()+fy), ATTR_WATER))
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 420, "");
					return false;
				}

#ifdef ENABLE_CAMPFIRE_BUG_FIX
				// Performans optimizasyonu: GetQuestFlag yerine direkt member variable
				if (GetCampfireFlag() > 0 && get_global_time() - GetCampfireFlag() < 60)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 789, "");
					return false;
				}

				SetCampfireFlag(get_global_time());
#endif

				LPCHARACTER campfire = CHARACTER_MANAGER::instance().SpawnMob(fishing::CAMPFIRE_MOB, GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy), 0, false, number(0, 359));

				char_event_info* info = AllocEventInfo<char_event_info>();

				info->ch = campfire;

				campfire->m_pkMiningEvent = event_create(kill_campfire_event, info, PASSES_PER_SEC(40));

				item->SetCount(item->GetCount() - 1);
			}
			break;

		case ITEM_UNIQUE:
			{
				switch (item->GetSubType())
				{
					case USE_ABILITY_UP:
						{
							// Fix
							if ((CheckTimeUsed(item) == false))
								return false;

							switch (item->GetValue(0))
							{
								case APPLY_MOV_SPEED:
									if (FindAffect(AFFECT_MOV_SPEED))
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
										return false;
									}

									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true, true);
									EffectPacket(SE_DXUP_PURPLE);
									break;

								case APPLY_ATT_SPEED:
									if (FindAffect(AFFECT_ATT_SPEED))
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
										return false;
									}

									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true, true);
									EffectPacket(SE_SPEEDUP_GREEN);
									break;

								case APPLY_STR:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_DEX:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_CON:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_INT:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_CAST_SPEED:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_RESIST_MAGIC:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_RESIST_MAGIC, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_ATT_GRADE_BONUS:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_GRADE_BONUS, 
											item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;

								case APPLY_DEF_GRADE_BONUS:
									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DEF_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true, true);
									break;
							}
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					default:
						{
							if (item->GetSubType() == USE_SPECIAL)
							{
								sys_log(0, "ITEM_UNIQUE: USE_SPECIAL %u", item->GetVnum());

#ifdef ENABLE_MULTI_FARM_BLOCK
								if (item->GetVnum() >= 55610 && item->GetVnum() <= 55615)
								{
#ifdef ENABLE_RENEWAL_PREMIUM_SYSTEM
									if (IsPremium())
									{
#endif
										if (FindAffect(AFFECT_MULTI_FARM_PREMIUM))
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 421, "");
											return false;
										}
										else
										{
											AddAffect(AFFECT_MULTI_FARM_PREMIUM, POINT_NONE, item->GetValue(1), AFF_NONE, item->GetValue(0), 0, false);
											item->SetCount(item->GetCount() - 1);
											CHARACTER_MANAGER::Instance().CheckMultiFarmAccount(GetDesc()->GetHostName(), GetPlayerID(), GetName(), GetRewardStatus());
											LocaleChatPacket(CHAT_TYPE_INFO, 422, "");
											LocaleChatPacket(CHAT_TYPE_INFO, 423, "");
										}
#ifdef ENABLE_RENEWAL_PREMIUM_SYSTEM
									}
									else
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 424, "");
										return false;
									}
#endif
								}
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
								if (item->GetVnum() == 71221)
								{
									if(FindAffect(AFFECT_DECORATION))
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
										return false;
									}

									AddAffect(AFFECT_DECORATION, POINT_NONE, 0, AFF_NONE, 60*60*24*item->GetValue(0), false, true);
									item->SetCount(item->GetCount() - 1);
									LocaleChatPacket(CHAT_TYPE_INFO, 601, "%d", item->GetValue(0));
									return true;
								}
#endif

								switch (item->GetVnum())
								{
									case 71049:
										UseSilkBotary();
										break;
								}
							}
							else
							{
								if (!item->IsEquipped())
									EquipItem(item);
								else
									UnequipItem(item);
							}
						}
						break;
				}
			}
			break;

		case ITEM_COSTUME:
		case ITEM_WEAPON:
		case ITEM_ARMOR:
		case ITEM_ROD:
		case ITEM_RING:
#ifdef ENABLE_MOUNT_SYSTEM
		case ITEM_MOUNT:
#endif
#ifdef ENABLE_PET_SYSTEM
		case ITEM_PET:
#endif
#ifdef ENABLE_TITLE_SYSTEM
		case ITEM_TITLE:
#endif
		case ITEM_PICK:
			if (!item->IsEquipped())
				EquipItem(item);
			else
				UnequipItem(item);
			break;

		case ITEM_DS:
			{
				if (!item->IsEquipped())
					return false;
				return DSManager::instance().PullOut(this, NPOS, item);
			break;
			}

		case ITEM_SPECIAL_DS:
			if (!item->IsEquipped())
				EquipItem(item);
			else
				UnequipItem(item);
			break;

		case ITEM_FISH:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
					return false;
				}

				if (item->GetSubType() == FISH_ALIVE)
					fishing::UseFish(this, item);
			}
			break;

		case ITEM_TREASURE_BOX:
			{
				return false;
			}
			break;

		case ITEM_TREASURE_KEY:
			{
				LPITEM item2;

				if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging())
					return false;

				if (item2->GetType() != ITEM_TREASURE_BOX)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 425, "");
					return false;
				}

				if (item->GetValue(0) == item2->GetValue(0))
				{
					DWORD dwBoxVnum = item2->GetVnum();
					std::vector <DWORD> dwVnums;
					std::vector <DWORD> dwCounts;
					std::vector <LPITEM> item_gets;
					int count = 0;

					if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
					{
						// Fix
						item->SetCount(item->GetCount() - 1);
						item2->SetCount(item2->GetCount() - 1);

						for (int i = 0; i < count; i++)
						{
							switch (dwVnums[i])
							{
								case CSpecialItemGroup::GOLD:
									LocaleChatPacket(CHAT_TYPE_INFO, 120, "%d", dwCounts[i]);
									break;

								case CSpecialItemGroup::EXP:
									LocaleChatPacket(CHAT_TYPE_INFO, 401, "");
									LocaleChatPacket(CHAT_TYPE_INFO, 22, "%d", dwCounts[i]);
									break;

								case CSpecialItemGroup::MOB:
									LocaleChatPacket(CHAT_TYPE_INFO, 402, "");
									break;

								case CSpecialItemGroup::SLOW:
									LocaleChatPacket(CHAT_TYPE_INFO, 403, "");
									break;

								case CSpecialItemGroup::DRAIN_HP:
									LocaleChatPacket(CHAT_TYPE_INFO, 404, "");
									break;

								case CSpecialItemGroup::POISON:
									LocaleChatPacket(CHAT_TYPE_INFO, 405, "");
									break;

								case CSpecialItemGroup::MOB_GROUP:
									LocaleChatPacket(CHAT_TYPE_INFO, 402, "");
									break;

								default:
#ifndef ENABLE_RENEWAL_SPECIAL_CHAT
									if (item_gets[i])
									{
										if (dwCounts[i] > 1)
											LocaleChatPacket(CHAT_TYPE_INFO, 427, "%s#%d", item_gets[i]->GetLocaleName(), dwCounts[i]);
										else
											LocaleChatPacket(CHAT_TYPE_INFO, 428, "%s ", item_gets[i]->GetLocaleName());
									}
#endif
									break;
							}
						}
					}
					else
					{
						LocaleChatPacket(CHAT_TYPE_INFO, 426, "");
						return false;
					}
				}
				else
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 426, "");
					return false;
				}
			}
			break;

		case ITEM_GIFTBOX:
			{
				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets;
				int count = 0;

#ifdef ENABLE_DRAGONSOUL_SYSTEM
				if( (dwBoxVnum > 51500 && dwBoxVnum < 52000) || (dwBoxVnum >= 50255 && dwBoxVnum <= 50260) )
				{
					if (!(this->DragonSoul_IsQualified()))
					{
						LocaleChatPacket(CHAT_TYPE_INFO, 323, "");
						return false;
					}
				}
#endif
				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					item->SetCount(item->GetCount()-1);

					for (int i = 0; i < count; i++)
					{
						switch (dwVnums[i])
						{
							case CSpecialItemGroup::GOLD:
								LocaleChatPacket(CHAT_TYPE_INFO, 120, "%d", dwCounts[i]);
								break;

							case CSpecialItemGroup::EXP:
								LocaleChatPacket(CHAT_TYPE_INFO, 401, "");
								LocaleChatPacket(CHAT_TYPE_INFO, 22, "%d", dwCounts[i]);
								break;

							case CSpecialItemGroup::MOB:
								LocaleChatPacket(CHAT_TYPE_INFO, 402, "");
								break;

							case CSpecialItemGroup::SLOW:
								LocaleChatPacket(CHAT_TYPE_INFO, 403, "");
								break;

							case CSpecialItemGroup::DRAIN_HP:
								LocaleChatPacket(CHAT_TYPE_INFO, 404, "");
								break;

							case CSpecialItemGroup::POISON:
								LocaleChatPacket(CHAT_TYPE_INFO, 405, "");
								break;

							case CSpecialItemGroup::MOB_GROUP:
								LocaleChatPacket(CHAT_TYPE_INFO, 402, "");
								break;

							default:
#ifndef ENABLE_RENEWAL_SPECIAL_CHAT
								if (item_gets[i])
								{
									if (dwCounts[i] > 1)
										LocaleChatPacket(CHAT_TYPE_INFO, 427, "%s#%d", item_gets[i]->GetLocaleName(), dwCounts[i]);
									else
										LocaleChatPacket(CHAT_TYPE_INFO, 428, "%s ", item_gets[i]->GetLocaleName());
								}
#endif
								break;
						}
					}
				}
				else
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 406, "");
					return false;
				}
			}
			break;

		case ITEM_SKILLFORGET:
			{
				if (!item->GetSocket(0))
				{
					ITEM_MANAGER::instance().RemoveItem(item);
					return false;
				}

				DWORD dwVnum = item->GetSocket(0);

				if (SkillLevelDown(dwVnum))
				{
					ITEM_MANAGER::instance().RemoveItem(item);
					LocaleChatPacket(CHAT_TYPE_INFO, 429, "");
				}
				else
					LocaleChatPacket(CHAT_TYPE_INFO, 430, "");
			}
			break;

		case ITEM_SKILLBOOK:
			{
				if (IsPolymorphed())
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 431, "");
					return false;
				}

#ifdef ENABLE_SKILL_BOOK_READING
				if (item->GetVnum() == 50300)
					ChatPacket(CHAT_TYPE_COMMAND, "bkekranac");
#endif
				else
				{
					if (IsPolymorphed())
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ÂºÂ¯Â½Ã…ÃžÃŸÂ¿Â¡Â´Ã‚ ÃƒÂ¥Ã€Â» Ã€ÄžÃ€Â»Â¼Ã¶ Â¾Ã¸Â½Ã€Â´ÃžÂ´Ã™."));
						return false;
					}
	
					DWORD dwVnum = 0;
	
					if (item->GetVnum() == 50300)
					{
						dwVnum = item->GetSocket(0);
					}
					else
					{
						dwVnum = item->GetValue(0);
					}
	
					if (0 == dwVnum)
					{
						ITEM_MANAGER::instance().RemoveItem(item);
	
						return false;
					}

					if (true == LearnSkillByBook(dwVnum))
					{
#ifndef DEFAULT_REMOVE_ALL_BOOKSS
						item->SetCount(item->GetCount() - 1);
#else	
						ITEM_MANAGER::instance().RemoveItem(item);
#endif	
						int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
						SetSkillNextReadTime(dwVnum, get_global_time() + iReadDelay);
					}
				}
			}

			break;

		case ITEM_USE:
			{
				if (item->GetVnum() > 50800 && item->GetVnum() <= 50820)
				{
					if (test_server)
						sys_log (0, "ADD addtional effect : vnum(%d) subtype(%d)", item->GetOriginalVnum(), item->GetSubType());

					int affect_type = AFFECT_EXP_BONUS_EURO_FREE;
					int apply_type = aApplyInfo[item->GetValue(0)].bPointType;
					int apply_value = item->GetValue(2);
					int apply_duration = item->GetValue(1);

					switch (item->GetSubType())
					{
						case USE_ABILITY_UP:
							if (FindAffect(affect_type, apply_type))
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
								return false;
							}

							{
								switch (item->GetValue(0))
								{
									case APPLY_MOV_SPEED:
										AddAffect(affect_type, apply_type, apply_value, AFF_MOV_SPEED_POTION, apply_duration, 0, true, true);
										EffectPacket(SE_DXUP_PURPLE);
										break;

									case APPLY_ATT_SPEED:
										AddAffect(affect_type, apply_type, apply_value, AFF_ATT_SPEED_POTION, apply_duration, 0, true, true);
										EffectPacket(SE_SPEEDUP_GREEN);
										break;

									case APPLY_STR:
									case APPLY_DEX:
									case APPLY_CON:
									case APPLY_INT:
									case APPLY_CAST_SPEED:
									case APPLY_RESIST_MAGIC:
									case APPLY_ATT_GRADE_BONUS:
									case APPLY_DEF_GRADE_BONUS:
										AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, true, true);
										break;
								}
							}

							if (GetDungeon())
								GetDungeon()->UsePotion(this);

							if (GetWarMap())
								GetWarMap()->UsePotion(this, item);

							item->SetCount(item->GetCount() - 1);
							break;

					case USE_AFFECT:
						{
							// Fix
							if ((CheckTimeUsed(item) == false))
								return false;

							if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType))
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
							}
							else
							{
								AddAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false, true);
								item->SetCount(item->GetCount() - 1);
							}
						}
						break;

					case USE_POTION_NODELAY:
						{
							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
								{
									LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
									return false;
								}

								switch (item->GetVnum())
								{
									case 70020 :
									case 71018 :
									case 71019 :
									case 71020 :
										if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
										{
											if (m_nPotionLimit <= 0)
											{
												LocaleChatPacket(CHAT_TYPE_INFO, 432, "");
												return false;
											}
										}
										break;

									default :
										LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
										return false;
										break;
								}
							}

							bool used = false;

							if (item->GetValue(0) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(1) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (item->GetValue(3) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(4) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (used)
							{
								if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
								{
									if (test_server)
										ChatPacket(CHAT_TYPE_INFO, "Used Moon Cake or Seed.");

									SetUseSeedOrMoonBottleTime();
								}

								if (GetDungeon())
									GetDungeon()->UsePotion(this);

								if (GetWarMap())
									GetWarMap()->UsePotion(this, item);

								m_nPotionLimit--;

								item->SetCount(item->GetCount() - 1);
							}
						}
						break;
					}

					return true;
				}


				if (item->GetVnum() >= 27863 && item->GetVnum() <= 27883)
				{
					if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
					{
						LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
						return false;
					}
				}

				if (test_server)
				{
					 sys_log (0, "USE_ITEM %s Type %d SubType %d vnum %d", item->GetName(), item->GetType(), item->GetSubType(), item->GetOriginalVnum());
				}

				switch (item->GetSubType())
				{
					case USE_TIME_CHARGE_PER:
						{
							LPITEM pDestItem = GetItem(DestCell);
							if (NULL == pDestItem)
							{
								return false;
							}
							if (pDestItem->IsDragonSoul())
							{
								int ret;
								char buf[128];
								if (item->GetVnum() == DRAGON_HEART_VNUM)
								{
									ret = pDestItem->GiveMoreTime_Per((float)item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
								}
								else
								{
									ret = pDestItem->GiveMoreTime_Per((float)item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
								}
								if (ret > 0)
								{
									if (item->GetVnum() == DRAGON_HEART_VNUM)
										sprintf(buf, "Inc %ds by item{VN:%d SOC%d:%d}", ret, item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
									else
										sprintf(buf, "Inc %ds by item{VN:%d VAL%d:%d}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));

									LocaleChatPacket(CHAT_TYPE_INFO, 433, "%d", ret);
									item->SetCount(item->GetCount() - 1);
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
									return true;
								}
								else
								{
									if (item->GetVnum() == DRAGON_HEART_VNUM)
										sprintf(buf, "No change by item{VN:%d SOC%d:%d}", item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
									else
										sprintf(buf, "No change by item{VN:%d VAL%d:%d}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));

									LocaleChatPacket(CHAT_TYPE_INFO, 73, "");
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
									return false;
								}
							}
							else
								return false;
						}
						break;
					case USE_TIME_CHARGE_FIX:
						{
							LPITEM pDestItem = GetItem(DestCell);
							if (NULL == pDestItem)
							{
								return false;
							}

							if (pDestItem->IsDragonSoul())
							{
								int ret = pDestItem->GiveMoreTime_Fix(item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
								char buf[128];
								if (ret)
								{
									LocaleChatPacket(CHAT_TYPE_INFO, 433, "%d", ret);
									sprintf(buf, "Increase %ds by item{VN:%d VAL%d:%d}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
									item->SetCount(item->GetCount() - 1);
									return true;
								}
								else
								{
									LocaleChatPacket(CHAT_TYPE_INFO, 73, "");
									sprintf(buf, "No change by item{VN:%d VAL%d:%d}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
									return false;
								}
							}
							else
								return false;
						}
						break;
					case USE_SPECIAL:
						switch (item->GetVnum())
						{
#ifdef ENABLE_RENEWAL_PREMIUM_SYSTEM
							case 90013:
							case 90014:
							case 90015:
							{
								if (IsPremium())
								{
									LocaleChatPacket(CHAT_TYPE_INFO, 101, "");
									return false;
								}

								time_t seconds = 0;
								if (item->GetVnum() == 90013)
									seconds = 7 * (60*60*24); // 7 days
								else if (item->GetVnum() == 90014)
									seconds = 15 * (60*60*24); // 15 days
								else if (item->GetVnum() == 90015)
									seconds = 30 * (60*60*24); // 30 days
								else
									return false;

								if (seconds == 0)
									return false;

								ActivatePremium(seconds);
								item->SetCount(item->GetCount() - 1);
							}
							break;
#endif
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
							case 70002:
							{
								if(FindAffect(AFFECT_AUTO_PICK_UP))
								{
									LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
									return false;
								}

								const int affectime = 60 * 60 * 24 * 30;
								AddAffect(AFFECT_AUTO_PICK_UP, POINT_NONE, 0, AFF_NONE, affectime, 0, false, true);
								item->SetCount(item->GetCount() - 1);
								LocaleChatPacket(CHAT_TYPE_INFO, 236, "%d", 30);
								return true;
							}
							break;
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
							case TRANSMUTATION_REVERSAL:
								LPITEM item2;
								if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
									return false;

								if (item2->IsExchanging())
									return false;
								
								if (item2->isLocked())
									return false;

								if (item2->GetTransmutationVnum() == 0)
									return false;

								item2->SetTransmutationVnum(0);
								item2->UpdatePacket();

								item->SetCount(item->GetCount() - 1);
								break;
#endif
							case ITEM_NOG_POCKET:
								{
									if (FindAffect(AFFECT_NOG_ABILITY))
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
										return false;
									}
									long time = item->GetValue(0);
									long moveSpeedPer = item->GetValue(1);
									long attPer	= item->GetValue(2);
									long expPer = item->GetValue(3);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
									EffectPacket(SE_DXUP_PURPLE);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
									AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
									item->SetCount(item->GetCount() - 1);
								}
								break;

							case ITEM_RAMADAN_CANDY:
								{
									long time = item->GetValue(0);
									long moveSpeedPer	= item->GetValue(1);
									long attPer	= item->GetValue(2);
									long expPer			= item->GetValue(3);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
									AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
									item->SetCount(item->GetCount() - 1);
								}
								break;
							case ITEM_MARRIAGE_RING:
								{
									marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
									if (pMarriage)
									{
										if (pMarriage->ch1 != NULL)
										{
											if (CArenaManager::instance().IsArenaMap(pMarriage->ch1->GetMapIndex()) == true)
											{
												LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
												break;
											}
										}

										if (pMarriage->ch2 != NULL)
										{
											if (CArenaManager::instance().IsArenaMap(pMarriage->ch2->GetMapIndex()) == true)
											{
												LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
												break;
											}
										}

										int consumeSP = CalculateConsumeSP(this);

										if (consumeSP < 0)
											return false;

										PointChange(POINT_SP, -consumeSP, false);

										WarpToPID(pMarriage->GetOther(GetPlayerID()));
									}
									else
										LocaleChatPacket(CHAT_TYPE_INFO, 434, "");
								}
								break;

							case UNIQUE_ITEM_CAPE_OF_COURAGE:
							case 70057:
							case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
								AggregateMonster();

#ifndef DEFAULT_FINITE_ITEMS
								item->SetCount(item->GetCount()-0);
#else
								item->SetCount(item->GetCount()-1);
#endif
								break;

							case UNIQUE_ITEM_WHITE_FLAG:
								ForgetMyAttacker();
								item->SetCount(item->GetCount()-1);
								break;

							case UNIQUE_ITEM_TREASURE_BOX:
								break;

							case 30093:
							case 30094:
							case 30095:
							case 30096:
								{
									const int MAX_BAG_INFO = 26;
									static struct LuckyBagInfo
									{
										DWORD count;
										int prob;
										DWORD vnum;
									} b1[MAX_BAG_INFO] =
									{
										{ 1000,	302,	1 },
										{ 10,	150,	27002 },
										{ 10,	75,	27003 },
										{ 10,	100,	27005 },
										{ 10,	50,	27006 },
										{ 10,	80,	27001 },
										{ 10,	50,	27002 },
										{ 10,	80,	27004 },
										{ 10,	50,	27005 },
										{ 1,	10,	50300 },
										{ 1,	6,	92 },
										{ 1,	2,	132 },
										{ 1,	6,	1052 },
										{ 1,	2,	1092 },
										{ 1,	6,	2082 },
										{ 1,	2,	2122 },
										{ 1,	6,	3082 },
										{ 1,	2,	3122 },
										{ 1,	6,	5052 },
										{ 1,	2,	5082 },
										{ 1,	6,	7082 },
										{ 1,	2,	7122 },
										{ 1,	1,	11282 },
										{ 1,	1,	11482 },
										{ 1,	1,	11682 },
										{ 1,	1,	11882 },
									};

									struct LuckyBagInfo b2[MAX_BAG_INFO] =
									{
										{ 1000,	302,	1 },
										{ 10,	150,	27002 },
										{ 10,	75,	27002 },
										{ 10,	100,	27005 },
										{ 10,	50,	27005 },
										{ 10,	80,	27001 },
										{ 10,	50,	27002 },
										{ 10,	80,	27004 },
										{ 10,	50,	27005 },
										{ 1,	10,	50300 },
										{ 1,	6,	92 },
										{ 1,	2,	132 },
										{ 1,	6,	1052 },
										{ 1,	2,	1092 },
										{ 1,	6,	2082 },
										{ 1,	2,	2122 },
										{ 1,	6,	3082 },
										{ 1,	2,	3122 },
										{ 1,	6,	5052 },
										{ 1,	2,	5082 },
										{ 1,	6,	7082 },
										{ 1,	2,	7122 },
										{ 1,	1,	11282 },
										{ 1,	1,	11482 },
										{ 1,	1,	11682 },
										{ 1,	1,	11882 },
									};

									LuckyBagInfo * bi = NULL;
									bi = b1;

									int pct = number(1, 1000);

									int i;
									for (i=0;i<MAX_BAG_INFO;i++)
									{
										if (pct <= bi[i].prob)
											break;
										pct -= bi[i].prob;
									}
									if (i>=MAX_BAG_INFO)
										return false;

									if (bi[i].vnum == 50300)
									{
										GiveRandomSkillBook();
									}
									else if (bi[i].vnum == 1)
									{
										PointChange(POINT_GOLD, 1000, true);
									}
									else
									{
										AutoGiveItem(bi[i].vnum, bi[i].count);
									}
									ITEM_MANAGER::instance().RemoveItem(item);
								}
								break;

							case 50004:
								{
									if (item->GetSocket(0))
									{
										item->SetSocket(0, item->GetSocket(0) + 1);
									}
									else
									{
										int iMapIndex = GetMapIndex();

										PIXEL_POSITION pos;

										if (SECTREE_MANAGER::instance().GetRandomLocation(iMapIndex, pos, 700))
										{
											item->SetSocket(0, 1);
											item->SetSocket(1, pos.x);
											item->SetSocket(2, pos.y);
										}
										else
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 435, "");
											return false;
										}
									}

									int dist = 0;
									float distance = (DISTANCE_SQRT(GetX()-item->GetSocket(1), GetY()-item->GetSocket(2)));

									if (distance < 1000.0f)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 436, "");

										struct TEventStoneInfo
										{
											DWORD dwVnum;
											int count;
											int prob;
										};

										const int EVENT_STONE_MAX_INFO = 15;
										TEventStoneInfo info_10[EVENT_STONE_MAX_INFO] =
										{
											{ 27001, 10,  8 },
											{ 27004, 10,  6 },
											{ 27002, 10, 12 },
											{ 27005, 10, 12 },
											{ 27100,  1,  9 },
											{ 27103,  1,  9 },
											{ 27101,  1, 10 },
											{ 27104,  1, 10 },
											{ 27999,  1, 12 },

											{ 25040,  1,  4 },

											{ 27410,  1,  0 },
											{ 27600,  1,  0 },
											{ 25100,  1,  0 },

											{ 50001,  1,  0 },
											{ 50003,  1,  1 },
										};

										TEventStoneInfo info_7[EVENT_STONE_MAX_INFO] =
										{
											{ 27001, 10,  1 },
											{ 27004, 10,  1 },
											{ 27004, 10,  9 },
											{ 27005, 10,  9 },
											{ 27100,  1,  5 },
											{ 27103,  1,  5 },
											{ 27101,  1, 10 },
											{ 27104,  1, 10 },
											{ 27999,  1, 14 },

											{ 25040,  1,  5 },

											{ 27410,  1,  5 },
											{ 27600,  1,  5 },
											{ 25100,  1,  5 },

											{ 50001,  1,  0 },
											{ 50003,  1,  5 },
										};

										TEventStoneInfo info_4[EVENT_STONE_MAX_INFO] =
										{
											{ 27001, 10,  0 },
											{ 27004, 10,  0 },
											{ 27002, 10,  0 },
											{ 27005, 10,  0 },
											{ 27100,  1,  0 },
											{ 27103,  1,  0 },
											{ 27101,  1,  0 },
											{ 27104,  1,  0 },
											{ 27999,  1, 25 },

											{ 25040,  1,  0 },

											{ 27410,  1,  0 },
											{ 27600,  1,  0 },
											{ 25100,  1, 15 },

											{ 50001,  1, 10 },
											{ 50003,  1, 50 },
										};

										{
											TEventStoneInfo* info;
											if (item->GetSocket(0) <= 4)
												info = info_4;
											else if (item->GetSocket(0) <= 7)
												info = info_7;
											else
												info = info_10;

											int prob = number(1, 100);

											for (int i = 0; i < EVENT_STONE_MAX_INFO; ++i)
											{
												if (!info[i].prob)
													continue;

												if (prob <= info[i].prob)
												{
													if (info[i].dwVnum == 50001)
													{
														DWORD * pdw = M2_NEW DWORD[2];

														pdw[0] = info[i].dwVnum;
														pdw[1] = info[i].count;
													}
													else
														AutoGiveItem(info[i].dwVnum, info[i].count);

													break;
												}
												prob -= info[i].prob;
											}
										}

										char chatbuf[CHAT_MAX_LEN + 1];
										int len = snprintf(chatbuf, sizeof(chatbuf), "StoneDetect %u 0 0", (DWORD)GetVID());

										if (len < 0 || len >= (int) sizeof(chatbuf))
											len = sizeof(chatbuf) - 1;

										++len;

										TPacketGCChat pack_chat;
										pack_chat.header = HEADER_GC_CHAT;
										pack_chat.size = sizeof(TPacketGCChat) + len;
										pack_chat.type = CHAT_TYPE_COMMAND;
										pack_chat.id = 0;
										pack_chat.bEmpire = GetDesc()->GetEmpire();

										TEMP_BUFFER buf;
										buf.write(&pack_chat, sizeof(TPacketGCChat));
										buf.write(chatbuf, len);

										PacketAround(buf.read_peek(), buf.size());

										ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 1");
										return true;
									}
									else if (distance < 20000)
										dist = 1;
									else if (distance < 70000)
										dist = 2;
									else
										dist = 3;

									const int STONE_DETECT_MAX_TRY = 10;
									if (item->GetSocket(0) >= STONE_DETECT_MAX_TRY)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 437, "");
										ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (DETECT_EVENT_STONE) 0");
										AutoGiveItem(27002);
										return true;
									}

									if (dist)
									{
										char chatbuf[CHAT_MAX_LEN + 1];
										int len = snprintf(chatbuf, sizeof(chatbuf), "StoneDetect %u %d %d",
											(DWORD)GetVID(), dist, (int)GetDegreeFromPositionXY(GetX(), item->GetSocket(2), item->GetSocket(1), GetY()));

										if (len < 0 || len >= (int) sizeof(chatbuf))
											len = sizeof(chatbuf) - 1;

										++len;

										TPacketGCChat pack_chat;
										pack_chat.header = HEADER_GC_CHAT;
										pack_chat.size = sizeof(TPacketGCChat) + len;
										pack_chat.type = CHAT_TYPE_COMMAND;
										pack_chat.id = 0;
										pack_chat.bEmpire = GetDesc()->GetEmpire();

										TEMP_BUFFER buf;
										buf.write(&pack_chat, sizeof(TPacketGCChat));
										buf.write(chatbuf, len);

										PacketAround(buf.read_peek(), buf.size());
									}

								}
								break;

							case 27989:
							case 76006:
								{
									LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

									if (pMap != NULL)
									{
										item->SetSocket(0, item->GetSocket(0) + 1);

										FFindStone f;

										pMap->for_each(f);

										if (f.m_mapStone.size() > 0)
										{
											std::map<DWORD, LPCHARACTER>::iterator stone = f.m_mapStone.begin();

											DWORD max = UINT_MAX;
											LPCHARACTER pTarget = stone->second;

											while (stone != f.m_mapStone.end())
											{
												DWORD dist = (DWORD)DISTANCE_SQRT(GetX()-stone->second->GetX(), GetY()-stone->second->GetY());

												if (dist != 0 && max > dist)
												{
													max = dist;
													pTarget = stone->second;
												}
												stone++;
											}

											if (pTarget != NULL)
											{
												int val = 3;

												if (max < 10000) val = 2;
												else if (max < 70000) val = 1;

												ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u %d %d", (DWORD)GetVID(), val,
														(int)GetDegreeFromPositionXY(GetX(), pTarget->GetY(), pTarget->GetX(), GetY()));
											}
											else
											{
												LocaleChatPacket(CHAT_TYPE_INFO, 438, "");
											}
										}
										else
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 438, "");
										}

										if (item->GetSocket(0) >= 6)
										{
											ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u 0 0", (DWORD)GetVID());
											ITEM_MANAGER::instance().RemoveItem(item);
										}
									}
									break;
								}
								break;

#ifdef ENABLE_SPIRIT_STONE_READING
							case 50513:
								ChatPacket(CHAT_TYPE_COMMAND, "ruhtasiekranac");
								break;
#endif

							case 27996:
								item->SetCount(item->GetCount() - 1);
								break;

							case 27987:
								{
									item->SetCount(item->GetCount() - 1);

									int r = number(1, 100);

									if (r <= 50)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 439, "");
										AutoGiveItem(27990);
									}
									else
									{
										const int prob_table[] =
										{
											70, 83, 91
										};

										if (r <= prob_table[0])
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 440, "");
										}
										else if (r <= prob_table[1])
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 441, "");
											AutoGiveItem(27992);
										}
										else if (r <= prob_table[2])
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 442, "");
											AutoGiveItem(27993);
										}
										else
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 443, "");
											AutoGiveItem(27994);
										}
									}
								}
								break;

							case 71013:
								CreateFly(number(FLY_FIREWORK1, FLY_FIREWORK6), this);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50100:
							case 50101:
							case 50102:
							case 50103:
							case 50104:
							case 50105:
							case 50106:
								CreateFly(item->GetVnum() - 50100 + FLY_FIREWORK1, this);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50200:
								{
									__OpenPrivateShop();
								}
								break;

							case fishing::FISH_MIND_PILL_VNUM:
								AddAffect(AFFECT_FISH_MIND_PILL, POINT_NONE, 0, AFF_FISH_MIND, 20*60, 0, true);
								item->SetCount(item->GetCount() - 1);
								break;

							case 50301:
							case 50302:
							case 50303:
								{
									if (IsPolymorphed() == true)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 245, "");
										return false;
									}

									int lv = GetSkillLevel(SKILL_LEADERSHIP);

									if (lv < item->GetValue(0))
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 444, "");
										return false;
									}

									if (lv >= item->GetValue(1))
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 445, "");
										return false;
									}

									if (LearnSkillByBook(SKILL_LEADERSHIP))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(SKILL_LEADERSHIP, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50304:
							case 50305:
							case 50306:
								{
									if (IsPolymorphed())
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 431, "");
										return false;
									}
									if (GetSkillLevel(SKILL_COMBO) == 0 && GetLevel() < 30)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 446, "");
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) == 1 && GetLevel() < 50)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 447, "");
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) >= 2)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 448, "");
										return false;
									}

									int iPct = item->GetValue(0);

									if (LearnSkillByBook(SKILL_COMBO, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(SKILL_COMBO, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50311:
							case 50312:
							case 50313:
								{
									if (IsPolymorphed())
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 431, "");
										return false;
									}

									DWORD dwSkillVnum = item->GetValue(0);
									int iPct = MINMAX(0, item->GetValue(1), 100);
									if (GetSkillLevel(dwSkillVnum) >= 20 || dwSkillVnum-SKILL_LANGUAGE1+1 == GetEmpire())
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 449, "");
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50061:
								{
									if (IsPolymorphed())
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 431, "");
										return false;
									}

									DWORD dwSkillVnum = item->GetValue(0);
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum) >= 10)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 227, "");
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50314: case 50315: case 50316:
							case 50323: case 50324:
							case 50325: case 50326:
								{
									if (IsPolymorphed() == true)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 245, "");
										return false;
									}
									
									int iSkillLevelLowLimit = item->GetValue(0);
									int iSkillLevelHighLimit = item->GetValue(1);
									int iPct = MINMAX(0, item->GetValue(2), 100);
									int iLevelLimit = item->GetValue(3);
									DWORD dwSkillVnum = 0;
									
									switch (item->GetVnum())
									{
										case 50314: case 50315: case 50316:
											dwSkillVnum = SKILL_POLYMORPH;
											break;

										case 50323: case 50324:
											dwSkillVnum = SKILL_ADD_HP;
											break;

										case 50325: case 50326:
											dwSkillVnum = SKILL_RESIST_PENETRATE;
											break;

										default:
											return false;
									}

									if (0 == dwSkillVnum)
										return false;

									if (GetLevel() < iLevelLimit)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 450, "");
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= 40)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 227, "");
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) < iSkillLevelLowLimit)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 444, "");
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= iSkillLevelHighLimit)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 451, "");
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50902:
							case 50903:
							case 50904:
								{
									if (IsPolymorphed())
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 431, "");
										return false;
									}

									DWORD dwSkillVnum = SKILL_CREATE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum) >= 40)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 227, "");
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case ITEM_MINING_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 431, "");
										return false;
									}

									DWORD dwSkillVnum = SKILL_MINING;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum) >= 40)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 227, "");
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case ITEM_HORSE_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 431, "");
										return false;
									}

									DWORD dwSkillVnum = SKILL_HORSE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetLevel() < 50)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 452, "");
										return false;
									}

									if (!test_server && get_global_time() < GetSkillNextReadTime(dwSkillVnum))
									{
										if (FindAffect(AFFECT_SKILL_NO_BOOK_DELAY))
										{
											RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
											LocaleChatPacket(CHAT_TYPE_INFO, 240, "");
										}
										else
										{
											SkillLearnWaitMoreTimeMessage(GetSkillNextReadTime(dwSkillVnum) - get_global_time());
											return false;
										}
									}

									if (GetPoint(POINT_HORSE_SKILL) >= 20 || 
											GetSkillLevel(SKILL_HORSE_WILDATTACK) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60 ||
											GetSkillLevel(SKILL_HORSE_WILDATTACK_RANGE) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 453, "");
										return false;
									}

									if (number(1, 100) <= iPct)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 454, "");
										LocaleChatPacket(CHAT_TYPE_INFO, 455, "");
										PointChange(POINT_HORSE_SKILL, 1);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

										if (!test_server)
											SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
									else
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 456, "");
									}

									ITEM_MANAGER::instance().RemoveItem(item);
								}
								break;

							case 70102:
							case 70103:
								{
									if (GetAlignment() >= 0)
										return false;

									int delta = MIN(-GetAlignment(), item->GetValue(0));

									sys_log(0, "%s ALIGNMENT ITEM %d", GetName(), delta);

									UpdateAlignment(delta);
									item->SetCount(item->GetCount() - 1);

									if (delta / 10 > 0)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 457, "");
										LocaleChatPacket(CHAT_TYPE_INFO, 458, "%d", delta/10);
									}
								}
								break;

							case 71107:
								{
									int val = item->GetValue(0);
									int interval = item->GetValue(1);
									quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
									int last_use_time = pPC->GetFlag("mythical_peach.last_use_time");

									if (GetAlignment() == 200000)
										return false;

									if (200000 - GetAlignment() < val * 10)
										val = (200000 - GetAlignment()) / 10;

									int old_alignment = GetAlignment() / 10;

									UpdateAlignment(val*10);

									item->SetCount(item->GetCount() - 1);
									pPC->SetFlag("mythical_peach.last_use_time", get_global_time());

									LocaleChatPacket(CHAT_TYPE_INFO, 457, "");
									LocaleChatPacket(CHAT_TYPE_INFO, 458, "%d", val);

									char buf[256 + 1];
									snprintf(buf, sizeof(buf), "%d %d", old_alignment, GetAlignment() / 10);
									LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
								}
								break;

							case 71109:
							case 72719:
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() == true)
										return false;

									if (item2->GetSocketCount() == 0)
										return false;

									#ifdef ENABLE_STONE_ITEM_BUG_FIX
									if (item2->IsEquipped())
										return false;
									#endif

									switch (item2->GetType())
									{
										case ITEM_WEAPON:
											break;

										case ITEM_ARMOR:
											switch (item2->GetSubType())
											{
												case ARMOR_EAR:
												case ARMOR_WRIST:
												case ARMOR_NECK:
													LocaleChatPacket(CHAT_TYPE_INFO, 413, "");
													return false;
											}
											break;

										default:
											return false;
									}

									std::stack<long> socket;

									for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
										socket.push(item2->GetSocket(i));

									int idx = ITEM_SOCKET_MAX_NUM - 1;

									while (socket.size() > 0)
									{
										if (socket.top() > 2 && socket.top() != ITEM_BROKEN_METIN_VNUM)
											break;

										idx--;
										socket.pop();
									}

									if (socket.size() == 0)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 413, "");
										return false;
									}

									LPITEM pItemReward = AutoGiveItem(socket.top());

									if (pItemReward != NULL)
									{
										item2->SetSocket(idx, 1);

										char buf[256+1];
										snprintf(buf, sizeof(buf), "%s(%u) %s(%u)", item2->GetName(), item2->GetID(), pItemReward->GetName(), pItemReward->GetID());
										LogManager::instance().ItemLog(this, item, "USE_DETACHMENT_ONE", buf);

										item->SetCount(item->GetCount() - 1);
									}
								}
								break;

							case 70201:
							case 70202:
							case 70203:
							case 70204:
							case 70205:
							case 70206:
								{
									if (GetPart(PART_HAIR) >= 1001)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 459, "");
									}
									else
									{
										quest::CQuestManager& q = quest::CQuestManager::instance();
										quest::PC* pPC = q.GetPC(GetPlayerID());

										if (pPC)
										{
											int last_dye_level = pPC->GetFlag("dyeing_hair.last_dye_level");

											if (last_dye_level == 0 ||
													last_dye_level+3 <= GetLevel() ||
													item->GetVnum() == 70201)
											{
												SetPart(PART_HAIR, item->GetVnum() - 70201);

												if (item->GetVnum() == 70201)
													pPC->SetFlag("dyeing_hair.last_dye_level", 0);
												else
													pPC->SetFlag("dyeing_hair.last_dye_level", GetLevel());

												item->SetCount(item->GetCount() - 1);
												UpdatePacket();
											}
											else
											{
												LocaleChatPacket(CHAT_TYPE_INFO, 460, "%d", last_dye_level + 3);
											}
										}
									}
								}
								break;

							case ITEM_NEW_YEAR_GREETING_VNUM:
								{
									DWORD dwBoxVnum = ITEM_NEW_YEAR_GREETING_VNUM;
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets;
									int count = 0;

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
									{
										for (int i = 0; i < count; i++)
										{
											if (dwVnums[i] == CSpecialItemGroup::GOLD)
												LocaleChatPacket(CHAT_TYPE_INFO, 120, "%d", dwCounts[i]);
										}

										item->SetCount(item->GetCount() - 1);
									}
								}
								break;

							case ITEM_VALENTINE_ROSE:
							case ITEM_VALENTINE_CHOCOLATE:
								{
									DWORD dwBoxVnum = item->GetVnum();
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets;
									int count = 0;

									if (item->GetVnum() == ITEM_VALENTINE_ROSE && SEX_MALE==GET_SEX(this) || item->GetVnum() == ITEM_VALENTINE_CHOCOLATE && SEX_FEMALE==GET_SEX(this))
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 461, "");
										return false;
									}

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
										item->SetCount(item->GetCount()-1);
								}
								break;

							case ITEM_WHITEDAY_CANDY:
							case ITEM_WHITEDAY_ROSE:
								{
									DWORD dwBoxVnum = item->GetVnum();
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets;
									int count = 0;

									if (item->GetVnum() == ITEM_WHITEDAY_CANDY && SEX_MALE==GET_SEX(this) || item->GetVnum() == ITEM_WHITEDAY_ROSE && SEX_FEMALE==GET_SEX(this))
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 461, "");
										return false;
									}

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
										item->SetCount(item->GetCount()-1);
								}
								break;

							case 50011:
								{
									DWORD dwBoxVnum = 50011;
									std::vector <DWORD> dwVnums;
									std::vector <DWORD> dwCounts;
									std::vector <LPITEM> item_gets;
									int count = 0;

									if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
									{
										for (int i = 0; i < count; i++)
										{
											char buf[50 + 1];
											snprintf(buf, sizeof(buf), "%u %u", dwVnums[i], dwCounts[i]);
											LogManager::instance().ItemLog(this, item, "MOONLIGHT_GET", buf);

											item->SetCount(item->GetCount() - 1);

											switch (dwVnums[i])
											{
												case CSpecialItemGroup::GOLD:
													LocaleChatPacket(CHAT_TYPE_INFO, 120, "%d", dwCounts[i]);
													break;

												case CSpecialItemGroup::EXP:
													LocaleChatPacket(CHAT_TYPE_INFO, 401, "");
													LocaleChatPacket(CHAT_TYPE_INFO, 22, "%d", dwCounts[i]);
													break;

												case CSpecialItemGroup::MOB:
													LocaleChatPacket(CHAT_TYPE_INFO, 402, "");
													break;

												case CSpecialItemGroup::SLOW:
													LocaleChatPacket(CHAT_TYPE_INFO, 403, "");
													break;

												case CSpecialItemGroup::DRAIN_HP:
													LocaleChatPacket(CHAT_TYPE_INFO, 404, "");
													break;

												case CSpecialItemGroup::POISON:
													LocaleChatPacket(CHAT_TYPE_INFO, 405, "");
													break;

												case CSpecialItemGroup::MOB_GROUP:
													LocaleChatPacket(CHAT_TYPE_INFO, 402, "");
													break;

												default:
#ifndef ENABLE_RENEWAL_SPECIAL_CHAT
													if (item_gets[i])
													{
														if (dwCounts[i] > 1)
															LocaleChatPacket(CHAT_TYPE_INFO, 427, "%s#%d", item_gets[i]->GetLocaleName(), dwCounts[i]);
														else
															LocaleChatPacket(CHAT_TYPE_INFO, 428, "%s ", item_gets[i]->GetLocaleName());
													}
#endif
													break;
											}
										}
									}
									else
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 406, "");
										return false;
									}
								}
								break;

							case ITEM_GIVE_STAT_RESET_COUNT_VNUM:
								{
									PointChange(POINT_STAT_RESET_COUNT, 1);
									item->SetCount(item->GetCount()-1);
								}
								break;

							case 50107:
								{
									EffectPacket(SE_CHINA_FIREWORK);
									AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
									item->SetCount(item->GetCount()-1);
								}
								break;

							case 50108:
								{
									if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
										return false;
									}

									EffectPacket(SE_SPIN_TOP);
									AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
									item->SetCount(item->GetCount()-1);
								}
								break;

							case ITEM_WONSO_BEAN_VNUM:
								PointChange(POINT_HP, GetMaxHP() - GetHP());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_WONSO_SUGAR_VNUM:
								PointChange(POINT_SP, GetMaxSP() - GetSP());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_WONSO_FRUIT_VNUM:
								PointChange(POINT_STAMINA, GetMaxStamina()-GetStamina());
								item->SetCount(item->GetCount()-1);
								break;

							case ITEM_ELK_VNUM:
								{
									int iGold = item->GetSocket(0);
									ITEM_MANAGER::instance().RemoveItem(item);
									LocaleChatPacket(CHAT_TYPE_INFO, 120, "%d", iGold);
									PointChange(POINT_GOLD, iGold);
								}
								break;

							case 27995: {}
								break;

							case 71092 :
								{
									if (m_pkChrTarget != NULL)
									{
										if (m_pkChrTarget->IsPolymorphed())
										{
											m_pkChrTarget->SetPolymorph(0);
											m_pkChrTarget->RemoveAffect(AFFECT_POLYMORPH);
										}
									}
									else
									{
										if (IsPolymorphed())
										{
											SetPolymorph(0);
											RemoveAffect(AFFECT_POLYMORPH);
										}
									}
								}
								break;

							case 71051 :
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetInventoryItem(wDestCell)))
										return false;

									if (item2->IsExchanging() == true)
										return false;

									if (item2->IsEquipped() == true) // Fix
										return false;

									if (item2->GetType() == ITEM_COSTUME) // Fix
										return false;

									if (item2->GetAttributeSetIndex() == -1)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 462, "");
										return false;
									}

									if (item2->AddRareAttribute() == true)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 472, "");

										int iAddedIdx = item2->GetRareAttrCount() + 4;
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										LogManager::instance().ItemLog(
												GetPlayerID(),
												item2->GetAttributeType(iAddedIdx),
												item2->GetAttributeValue(iAddedIdx),
												item->GetID(),
												"ADD_RARE_ATTR",
												buf,
												GetDesc()->GetHostName(),
												item->GetOriginalVnum());

										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
									}
								}
								break;

							case 71052 :
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() == true)
										return false;

									if (item2->IsEquipped() == true) // Fix
										return false;

									if (item2->GetType() == ITEM_COSTUME) // Fix
										return false;

									if (item2->GetAttributeSetIndex() == -1)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 462, "");
										return false;
									}

									if (item2->ChangeRareAttribute() == true)
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_RARE_ATTR", buf);

										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 462, "");
									}
								}
								break;

#ifdef ENABLE_COINS_INVENTORY
								case 80014:
								case 80015:
								case 80016:
								case 80017:
									{
										long long lCoins = item->GetValue(0);
										item->SetCount(item->GetCount() - 1);
										LocaleChatPacket(CHAT_TYPE_INFO, 463, "%d", lCoins);
										PointChange(POINT_COINS, lCoins);
									}
									break;
#endif

#ifdef ENABLE_EXTEND_TIME_COSTUME_SYSTEM
								case 84014:
								case 84015:
								case 84016:
									{
										LPITEM item2;

										if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
											return false;

										if (item2->IsExchanging() || item2->IsEquipped())
											return false;

										if (item2->GetType() != ITEM_COSTUME)
											return false;

										if (!(item2->GetSubType() == COSTUME_BODY || item2->GetSubType() == COSTUME_HAIR 
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
											|| item2->GetSubType() == COSTUME_WEAPON
#endif
										))
											return false;

										auto day = 60 * 60 * 24;
										auto limit_time = day * 365;
										auto existing_duration = item2->GetSocket(0);
										auto additional_duration = item->GetValue(0) * day;
										auto new_duration = existing_duration + additional_duration;

										if ((existing_duration == 0) || (new_duration - time(0) >= limit_time))
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 464, "");
											return false;
										}

										item->SetCount(item->GetCount() - 1);
										LocaleChatPacket(CHAT_TYPE_INFO, 465, "");
										item2->SetSocket(0, new_duration);
									}
									break;
#endif

							case ITEM_AUTO_HP_RECOVERY_S:
							case ITEM_AUTO_HP_RECOVERY_M:
							case ITEM_AUTO_HP_RECOVERY_L:
							case ITEM_AUTO_HP_RECOVERY_X:
							case ITEM_AUTO_SP_RECOVERY_S:
							case ITEM_AUTO_SP_RECOVERY_M:
							case ITEM_AUTO_SP_RECOVERY_L:
							case ITEM_AUTO_SP_RECOVERY_X:
							case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS: 
							case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S: 
							case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS: 
							case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
								{
									if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
										return false;
									}

#ifndef TIMER_AUTO_POTION
									quest::CQuestManager& q = quest::CQuestManager::instance();
									quest::PC* pPC = q.GetPC(GetPlayerID());

									if (pPC != NULL)
									{
										int last_use_time = pPC->GetFlag("auto_recovery.last_use_time");

										if (get_global_time() - last_use_time < 2)
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", 2 - (get_global_time() - last_use_time));
											return false;
										}

										pPC->SetFlag("auto_recovery.last_use_time", get_global_time());
									}
#endif

									EAffectTypes type = AFFECT_NONE;
									bool isSpecialPotion = false;

									switch (item->GetVnum())
									{
										case ITEM_AUTO_HP_RECOVERY_X:
											isSpecialPotion = true;

										case ITEM_AUTO_HP_RECOVERY_S:
										case ITEM_AUTO_HP_RECOVERY_M:
										case ITEM_AUTO_HP_RECOVERY_L:
										case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
										case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
											type = AFFECT_AUTO_HP_RECOVERY;
											break;

										case ITEM_AUTO_SP_RECOVERY_X:
											isSpecialPotion = true;

										case ITEM_AUTO_SP_RECOVERY_S:
										case ITEM_AUTO_SP_RECOVERY_M:
										case ITEM_AUTO_SP_RECOVERY_L:
										case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
										case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
											type = AFFECT_AUTO_SP_RECOVERY;
											break;
									}

									if (AFFECT_NONE == type)
										break;

									if (item->GetCount() > 1)
									{
										int pos = GetEmptyInventory(item->GetSize());

										if (-1 == pos)
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
											break;
										}

										item->SetCount( item->GetCount() - 1 );

										LPITEM item2 = ITEM_MANAGER::instance().CreateItem( item->GetVnum(), 1 );
										item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

										if (item->GetSocket(1) != 0)
										{
											item2->SetSocket(1, item->GetSocket(1));
										}

										item = item2;
									}

									CAffect* pAffect = FindAffect( type );

									if (NULL == pAffect)
									{
										EPointTypes bonus = POINT_NONE;

										if (true == isSpecialPotion)
										{
											if (type == AFFECT_AUTO_HP_RECOVERY)
											{
												bonus = POINT_MAX_HP_PCT;
											}
											else if (type == AFFECT_AUTO_SP_RECOVERY)
											{
												bonus = POINT_MAX_SP_PCT;
											}
										}

										AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

										item->Lock(true);
										item->SetSocket(0, true);

										AutoRecoveryItemProcess( type );
									}
									else
									{
										if (item->GetID() == pAffect->dwFlag)
										{
											RemoveAffect( pAffect );

											item->Lock(false);
											item->SetSocket(0, false);
										}
										else
										{
											LPITEM old = FindItemByID( pAffect->dwFlag );

											if (NULL != old)
											{
												old->Lock(false);
												old->SetSocket(0, false);
											}

											RemoveAffect( pAffect );

											EPointTypes bonus = POINT_NONE;

											if (true == isSpecialPotion)
											{
												if (type == AFFECT_AUTO_HP_RECOVERY)
												{
													bonus = POINT_MAX_HP_PCT;
												}
												else if (type == AFFECT_AUTO_SP_RECOVERY)
												{
													bonus = POINT_MAX_SP_PCT;
												}
											}

											AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

											item->Lock(true);
											item->SetSocket(0, true);

											AutoRecoveryItemProcess( type );
										}
									}
								}
								break;
	
#ifdef ENABLE_AURA_COSTUME_SYSTEM
							case ITEM_AURA_BOOST_ITEM_VNUM_BASE + ITEM_AURA_BOOST_ERASER:
								{
									LPITEM item2;
									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() || item2->IsEquipped())
										return false;

									if (item2->GetSocket(ITEM_SOCKET_AURA_BOOST) == 0)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 693, "");
										return false;
									}

									if (IS_SET(item->GetFlag(), ITEM_FLAG_STACKABLE) && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && item->GetCount() > 1)
										item->SetCount(item->GetCount() - 1);
									else
										ITEM_MANAGER::instance().RemoveItem(item);

									item2->SetSocket(ITEM_SOCKET_AURA_BOOST, 0);
								}
								break;
#endif
						}
						break;

					case USE_CLEAR:
						{
							RemoveBadAffect();
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_INVISIBILITY:
						{
							if (item->GetVnum() == 70026)
							{
								quest::CQuestManager& q = quest::CQuestManager::instance();
								quest::PC* pPC = q.GetPC(GetPlayerID());

								if (pPC != NULL)
								{
									int last_use_time = pPC->GetFlag("mirror_of_disapper.last_use_time");

									if (get_global_time() - last_use_time < 10*60)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
										return false;
									}

									pPC->SetFlag("mirror_of_disapper.last_use_time", get_global_time());
								}
							}

							AddAffect(AFFECT_INVISIBILITY, POINT_NONE, 0, AFF_INVISIBILITY, 300, 0, true);
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_POTION_NODELAY:
						{
							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
								{
									LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
									return false;
								}

								switch (item->GetVnum())
								{
									case 70020 :
									case 71018 :
									case 71019 :
									case 71020 :
										if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
										{
											if (m_nPotionLimit <= 0)
											{
												LocaleChatPacket(CHAT_TYPE_INFO, 432, "");
												return false;
											}
										}
										break;

									default :
										LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
										return false;
								}
							}

							bool used = false;

							if (item->GetValue(0) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(1) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (item->GetValue(3) != 0)
							{
								if (GetHP() < GetMaxHP())
								{
									PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
									EffectPacket(SE_HPUP_RED);
									used = TRUE;
								}
							}

							if (item->GetValue(4) != 0)
							{
								if (GetSP() < GetMaxSP())
								{
									PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
									EffectPacket(SE_SPUP_BLUE);
									used = TRUE;
								}
							}

							if (used)
							{
								if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
									SetUseSeedOrMoonBottleTime();

								if (GetDungeon())
									GetDungeon()->UsePotion(this);

								if (GetWarMap())
									GetWarMap()->UsePotion(this, item);

								m_nPotionLimit--;

								item->SetCount(item->GetCount() - 1);
							}
						}
						break;

					case USE_POTION:
						if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
						{
							if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
								return false;
							}
						
							switch (item->GetVnum())
							{
								case 27001 :
								case 27002 :
								case 27003 :
								case 27004 :
								case 27005 :
								case 27006 :
									if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
									{
										if (m_nPotionLimit <= 0)
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 432, "");
											return false;
										}
									}
									break;

								default :
									LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
									return false;
							}
						}
						
						if (item->GetValue(1) != 0)
						{
							if (GetPoint(POINT_SP_RECOVERY) + GetSP() >= GetMaxSP())
							{
								return false;
							}

							PointChange(POINT_SP_RECOVERY, item->GetValue(1) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
							StartAffectEvent();
							EffectPacket(SE_SPUP_BLUE);
						}

						if (item->GetValue(0) != 0)
						{
							if (GetPoint(POINT_HP_RECOVERY) + GetHP() >= GetMaxHP())
							{
								return false;
							}

							PointChange(POINT_HP_RECOVERY, item->GetValue(0) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
							StartAffectEvent();
							EffectPacket(SE_HPUP_RED);
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

#ifndef DEFAULT_FINITE_ITEMS
						item->SetCount(item->GetCount() - 0);
#else
						item->SetCount(item->GetCount() - 1);
#endif

						m_nPotionLimit--;
						break;

					case USE_POTION_CONTINUE:
						{
							if (item->GetValue(0) != 0)
							{
								AddAffect(AFFECT_HP_RECOVER_CONTINUE, POINT_HP_RECOVER_CONTINUE, item->GetValue(0), 0, item->GetValue(2), 0, true);
							}
							else if (item->GetValue(1) != 0)
							{
								AddAffect(AFFECT_SP_RECOVER_CONTINUE, POINT_SP_RECOVER_CONTINUE, item->GetValue(1), 0, item->GetValue(2), 0, true);
							}
							else
								return false;
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					case USE_ABILITY_UP:
						{
							switch (item->GetValue(0))
							{
								case APPLY_MOV_SPEED:
									if (FindAffect(AFFECT_MOV_SPEED))
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
										return false;
									}

									AddAffect(AFFECT_MOV_SPEED, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true);
									EffectPacket(SE_DXUP_PURPLE);
									break;

								case APPLY_ATT_SPEED:
									if (FindAffect(AFFECT_ATT_SPEED))
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
										return false;
									}

									AddAffect(AFFECT_ATT_SPEED, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true);
									EffectPacket(SE_SPEEDUP_GREEN);
									break;

								case APPLY_STR:
									AddAffect(AFFECT_STR, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_DEX:
									AddAffect(AFFECT_DEX, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_CON:
									AddAffect(AFFECT_CON, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_INT:
									AddAffect(AFFECT_INT, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_CAST_SPEED:
									AddAffect(AFFECT_CAST_SPEED, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_ATT_GRADE_BONUS:
									AddAffect(AFFECT_ATT_GRADE, POINT_ATT_GRADE_BONUS, 
											item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;

								case APPLY_DEF_GRADE_BONUS:
									AddAffect(AFFECT_DEF_GRADE, POINT_DEF_GRADE_BONUS,
											item->GetValue(2), 0, item->GetValue(1), 0, true);
									break;
							}
						}

						if (GetDungeon())
							GetDungeon()->UsePotion(this);

						if (GetWarMap())
							GetWarMap()->UsePotion(this, item);

						item->SetCount(item->GetCount() - 1);
						break;

					case USE_TALISMAN:
						{
							const int TOWN_PORTAL	= 1;
							const int MEMORY_PORTAL = 2;

							if (GetMapIndex() == 200 || GetMapIndex() == 113)
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
								return false;
							}

							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 400, "");
								return false;
							}

							if (m_pkWarpEvent)
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
								return false;
							}

							int consumeLife = CalculateConsume(this);

							if (consumeLife < 0)
								return false;

							if (item->GetValue(0) == TOWN_PORTAL)
							{
								if (item->GetSocket(0) == 0)
								{
									if (!GetDungeon())
										if (!GiveRecallItem(item))
											return false;

									PIXEL_POSITION posWarp;

									if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp))
									{
										PointChange(POINT_HP, -consumeLife, false);

										WarpSet(posWarp.x, posWarp.y);
									}
									else
									{
										sys_err("CHARACTER::UseItem : cannot find spawn position (name %s, %d x %d)", GetName(), GetX(), GetY());
									}
								}
								else
								{
									ProcessRecallItem(item);
								}
							}
							else if (item->GetValue(0) == MEMORY_PORTAL)
							{
								if (item->GetSocket(0) == 0)
								{
									if (GetDungeon())
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 466, "%s#%s", item->GetLocaleName());
										return false;
									}

									if (!GiveRecallItem(item))
										return false;
								}
								else
								{
									PointChange(POINT_HP, -consumeLife, false);
									ProcessRecallItem(item);
								}
							}
						}
						break;

					case USE_TUNING:
					case USE_DETACHMENT:
						{
							LPITEM item2;

							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsExchanging())
								return false;
	
							if (item2->IsEquipped()) // Fix
								return false;
	
							if (item2->GetVnum() >= 28330 && item2->GetVnum() <= 28343)
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
								return false;
							}

							#ifdef ENABLE_STONE_ITEM_BUG_FIX
							if (item2->IsEquipped())
								return false;
							#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
							if (item->GetValue(0) == ACCE_CLEAN_ATTR_VALUE0 || item->GetVnum() == ACCE_REVERSAL_VNUM_1 || item->GetVnum() == ACCE_REVERSAL_VNUM_2)
							{
								if (!CleanAcceAttr(item, item2))
									return false;

								item->SetCount(item->GetCount()-1);
								return true;
							}
#endif
							if (item2->GetVnum() >= 28430 && item2->GetVnum() <= 28443)
							{
								if (item->GetVnum() == 71056)
									RefineItem(item, item2);
								else
									LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
							}
							else
							{
								RefineItem(item, item2);
							}
						}
						break;

#ifdef ENABLE_BONUS_COSTUME_SYSTEM
					case USE_CHANGE_COSTUME_ATTR:
					case USE_RESET_COSTUME_ATTR:
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsEquipped()) // Fix
								return false;

							if (item2->IsExchanging())
								return false;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
							if (ITEM_COSTUME != item2->GetType() || (item2->GetSubType() != COSTUME_BODY && item2->GetSubType() != COSTUME_HAIR && item2->GetSubType() != COSTUME_WEAPON))
#else
							if (ITEM_COSTUME != item2->GetType())
#endif
								return false;

							if (item2->GetAttributeSetIndex() == -1)
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 462, "");
								return false;
							}

							switch (item->GetSubType())
							{
								case USE_CHANGE_COSTUME_ATTR:
									{
										if (item2->GetAttributeCount() == 0)
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
											return false;
										}

										if (item2->GetSubType() == COSTUME_HAIR)
											item2->ChangeAttrCostumeHair();
										else
											item2->ChangeAttrCostume();

										item->SetCount(item->GetCount() - 1);
										LocaleChatPacket(CHAT_TYPE_INFO, 470, "");
									}
									break;

								case USE_RESET_COSTUME_ATTR:
									{
										DWORD bonus_count = 3;

										if (item2->GetVnum() >= 73001 and item2->GetVnum() <= 73012 or item2->GetVnum() >= 73251 and item2->GetVnum() <= 73262 or item2->GetVnum() >= 73501 and item2->GetVnum() <= 73512 or item2->GetVnum() >= 73751 and item2->GetVnum() <= 73762)
											return false;

										if (item2->GetAttributeCount() < bonus_count)
										{
											if (item2->GetSubType() == COSTUME_HAIR)
												item2->AddAttrCostumeHair();
											else
												item2->AddAttrCostume();

											LocaleChatPacket(CHAT_TYPE_INFO, 472, "");
											item->SetCount(item->GetCount() - 1);
										}
										else
											LocaleChatPacket(CHAT_TYPE_INFO, 475, "");
									}
									break;
							}
						}
						break;
#endif

					case USE_PUT_INTO_BELT_SOCKET:
					case USE_PUT_INTO_RING_SOCKET:
					case USE_PUT_INTO_ACCESSORY_SOCKET:
					case USE_ADD_ACCESSORY_SOCKET:
					case USE_CLEAN_SOCKET:
					case USE_CHANGE_ATTRIBUTE:
					case USE_CHANGE_ATTRIBUTE2:
					case USE_ADD_ATTRIBUTE:
					case USE_ADD_ATTRIBUTE2:
#ifdef ENABLE_AURA_COSTUME_SYSTEM
					case USE_PUT_INTO_AURA_SOCKET:
#endif
						{
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsEquipped())
							{
								BuffOnAttr_RemoveBuffsFromItem(item2);
							}

							if (ITEM_COSTUME == item2->GetType())
#ifdef ENABLE_AURA_COSTUME_SYSTEM
							if (item->GetSubType() != USE_PUT_INTO_AURA_SOCKET)
#endif
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 462, "");
								return false;
							}

							if (item2->IsExchanging())
								return false;

							if (item2->IsEquipped()) // Fix
								return false;

							switch (item->GetSubType())
							{
								case USE_CLEAN_SOCKET:
									{
										int i;
										for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
										{
											if (item2->GetSocket(i) == ITEM_BROKEN_METIN_VNUM)
												break;
										}

										if (i == ITEM_SOCKET_MAX_NUM)
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 467, "");
											return false;
										}

										int j = 0;

										for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
										{
											if (item2->GetSocket(i) != ITEM_BROKEN_METIN_VNUM && item2->GetSocket(i) != 0)
												item2->SetSocket(j++, item2->GetSocket(i));
										}

										for (; j < ITEM_SOCKET_MAX_NUM; ++j)
										{
											if (item2->GetSocket(j) > 0)
												item2->SetSocket(j, 1);
										}

										{
											char buf[21];
											snprintf(buf, sizeof(buf), "%u", item2->GetID());
											LogManager::instance().ItemLog(this, item, "CLEAN_SOCKET", buf);
										}

										item->SetCount(item->GetCount() - 1);

									}
									break;

								case USE_CHANGE_ATTRIBUTE:
									if (item2->GetAttributeSetIndex() == -1)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 462, "");
										return false;
									}

									if (item2->GetAttributeCount() == 0)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
										return false;
									}

									if (GM_PLAYER == GetGMLevel() && false == test_server)
									{
										DWORD dwChangeItemAttrCycle = quest::CQuestManager::instance().GetEventFlag(msc_szChangeItemAttrCycleFlag);
										if (dwChangeItemAttrCycle < msc_dwDefaultChangeItemAttrCycle)
											dwChangeItemAttrCycle = msc_dwDefaultChangeItemAttrCycle;

										quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());

										if (pPC)
										{
											DWORD dwNowMin = get_global_time() / 60;
											pPC->SetFlag(msc_szLastChangeItemAttrFlag, dwNowMin);
										}
									}

									if (item->GetSubType() == USE_CHANGE_ATTRIBUTE2)
									{
										int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] = 
										{
											0, 0, 30, 40, 3
										};

										item2->ChangeAttribute(aiChangeProb);
									}
									else if (item->GetVnum() == 76014)
									{
										int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] = 
										{
											0, 10, 50, 39, 1
										};

										item2->ChangeAttribute(aiChangeProb);
									}
									#ifdef ENABLE_ITEM_MODIFIER_AVG_SKILL
									else if (item->GetVnum() == 75888)
									{
										if ((item2-> GetType() == ITEM_WEAPON) && ((item2->GetAttributeType(0) == 72) && (item2->GetAttributeType(1) == 71)) || ((item2->GetAttributeType(1) == 72) && (item2->GetAttributeType(0) == 71)))
										{
											int32_t value1 = number(50, 70);
											int32_t value2 = -(number(18,23));
											item2->SetForceAttribute(0,72,value1);
											item2->SetForceAttribute(1,71,value2);
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, "Bu eÅŸyayÄ± sadece Ortalama ve Becerili silahlarda kullanabilirsiniz.");
											return false;
										}
									}

									else if (item->GetVnum() == 75891)
									{
										if ((item2-> GetType() == ITEM_WEAPON) && ((item2->GetAttributeType(0) == 72) && (item2->GetAttributeType(1) == 71)) || ((item2->GetAttributeType(1) == 72) && (item2->GetAttributeType(0) == 71)))
										{
											int32_t value1 = -(number(27, 34));
											int32_t value2 = number(25,30);
											item2->SetForceAttribute(0, 72, value1);
											item2->SetForceAttribute(1, 71, value2);
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, "Bu eÅŸyayÄ± sadece Ortalama ve Becerili silahlarda kullanabilirsiniz.");
											return false;
										}
									}
									#endif
									else
									{
										if (item->GetVnum() == 71151 || item->GetVnum() == 76023)
										{
											if ((item2->GetType() == ITEM_WEAPON) || (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
											{
												bool bCanUse = true;
												for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
												{
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
													{
														bCanUse = false;
														break;
													}
												}
												if (false == bCanUse)
												{
													LocaleChatPacket(CHAT_TYPE_INFO, 468, "");
													break;
												}
											}
											else
											{
												LocaleChatPacket(CHAT_TYPE_INFO, 469, "");
												break;
											}
										}
										item2->ChangeAttribute();
									}

									LocaleChatPacket(CHAT_TYPE_INFO, 470, "");

									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_ATTRIBUTE", buf);
									}

									item->SetCount(item->GetCount() - 1);
									break;

								case USE_ADD_ATTRIBUTE:
									if (item2->GetAttributeSetIndex() == -1)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 462, "");
										return false;
									}

#ifdef ENABLE_PENDANT_SYSTEM
									if (item2->IsPendant() && !CheckCanAddPendantBonus(item2))
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 471, "");
										return false;
									}
#endif
									if (item2->GetAttributeCount() < 4)
									{
										if (item->GetVnum() == 71152 || item->GetVnum() == 76024)
										{
											if ((item2->GetType() == ITEM_WEAPON) || (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
											{
												bool bCanUse = true;
												for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
												{
													if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
													{
														bCanUse = false;
														break;
													}
												}
												if (false == bCanUse)
												{
													LocaleChatPacket(CHAT_TYPE_INFO, 468, "");
													break;
												}
											}
											else
											{
												LocaleChatPacket(CHAT_TYPE_INFO, 469, "");
												break;
											}
										}

										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
										{
											item2->AddAttribute();
											LocaleChatPacket(CHAT_TYPE_INFO, 472, "");

											int iAddedIdx = item2->GetAttributeCount() - 1;
											LogManager::instance().ItemLog(
													GetPlayerID(), 
													item2->GetAttributeType(iAddedIdx),
													item2->GetAttributeValue(iAddedIdx),
													item->GetID(), 
													"ADD_ATTRIBUTE_SUCCESS",
													buf,
													GetDesc()->GetHostName(),
													item->GetOriginalVnum());
										}
										else
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 473, "");
											LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE_FAIL", buf);
										}
										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 474, "");
									}
									break;

								case USE_ADD_ATTRIBUTE2:
									if (item2->GetAttributeSetIndex() == -1)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 462, "");
										return false;
									}

									if (item2->GetAttributeCount() == 4)
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
										{
											item2->AddAttribute();
											LocaleChatPacket(CHAT_TYPE_INFO, 472, "");

											int iAddedIdx = item2->GetAttributeCount() - 1;
											LogManager::instance().ItemLog(
													GetPlayerID(),
													item2->GetAttributeType(iAddedIdx),
													item2->GetAttributeValue(iAddedIdx),
													item->GetID(),
													"ADD_ATTRIBUTE2_SUCCESS",
													buf,
													GetDesc()->GetHostName(),
													item->GetOriginalVnum());
										}
										else
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 473, "");
											LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE2_FAIL", buf);
										}

										item->SetCount(item->GetCount() - 1);
									}
									else if (item2->GetAttributeCount() == 5)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 475, "");
									}
									else if (item2->GetAttributeCount() < 4)
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 476, "");
									}
									else
									{
										sys_err("ADD_ATTRIBUTE2 : Item has wrong AttributeCount(%d)", item2->GetAttributeCount());
									}
									break;

								case USE_ADD_ACCESSORY_SOCKET:
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (item2->IsAccessoryForSocket())
										{
											if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
											{
												if (number(1, 100) <= 50)
												{
													item2->SetAccessorySocketMaxGrade(item2->GetAccessorySocketMaxGrade() + 1);
													LocaleChatPacket(CHAT_TYPE_INFO, 477, "");
													LogManager::instance().ItemLog(this, item, "ADD_SOCKET_SUCCESS", buf);
												}
												else
												{
													LocaleChatPacket(CHAT_TYPE_INFO, 478, "");
													LogManager::instance().ItemLog(this, item, "ADD_SOCKET_FAIL", buf);
												}

												item->SetCount(item->GetCount() - 1);
											}
											else
											{
												LocaleChatPacket(CHAT_TYPE_INFO, 479, "");
											}
										}
										else
										{
											LocaleChatPacket(CHAT_TYPE_INFO, 480, "");
										}
									}
									break;

								case USE_PUT_INTO_BELT_SOCKET:
								case USE_PUT_INTO_ACCESSORY_SOCKET:
									if (item2->IsAccessoryForSocket() && item->CanPutInto(item2))
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										if (item2->GetAccessorySocketGrade() < item2->GetAccessorySocketMaxGrade())
										{
											if (number(1, 100) <= aiAccessorySocketPutPct[item2->GetAccessorySocketGrade()])
											{
												item2->SetAccessorySocketGrade(item2->GetAccessorySocketGrade() + 1);
												LocaleChatPacket(CHAT_TYPE_INFO, 481, "");
												LogManager::instance().ItemLog(this, item, "PUT_SOCKET_SUCCESS", buf);
											}
											else
											{
												LocaleChatPacket(CHAT_TYPE_INFO, 482, "");
												LogManager::instance().ItemLog(this, item, "PUT_SOCKET_FAIL", buf);
											}

											item->SetCount(item->GetCount() - 1);
										}
										else
										{
											if (item2->GetAccessorySocketMaxGrade() == 0)
												LocaleChatPacket(CHAT_TYPE_INFO, 483, "");
											else if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
											{
												LocaleChatPacket(CHAT_TYPE_INFO, 484, "");
												LocaleChatPacket(CHAT_TYPE_INFO, 485, "");
											}
											else
												LocaleChatPacket(CHAT_TYPE_INFO, 486, "");
										}
									}
									else
									{
										LocaleChatPacket(CHAT_TYPE_INFO, 487, "");
									}
									break;

#ifdef ENABLE_AURA_COSTUME_SYSTEM
								case USE_PUT_INTO_AURA_SOCKET:
									{
										if (item2->IsAuraBoosterForSocket() && item->CanPutInto(item2))
										{
											char buf[21];
											snprintf(buf, sizeof(buf), "%lu", item2->GetID());

											const BYTE c_bAuraBoostIndex = item->GetOriginalVnum() - ITEM_AURA_BOOST_ITEM_VNUM_BASE;
											item2->SetSocket(ITEM_SOCKET_AURA_BOOST, c_bAuraBoostIndex * 100000000 + item->GetValue(ITEM_AURA_BOOST_TIME_VALUE));

											LocaleChatPacket(CHAT_TYPE_INFO, 502, "");

											LogManager::instance().ItemLog(this, item, "PUT_AURA_SOCKET", buf);

											if (IS_SET(item->GetFlag(), ITEM_FLAG_STACKABLE) && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && item->GetCount() > 1)
												item->SetCount(item->GetCount() - 1);
											else
												ITEM_MANAGER::instance().RemoveItem(item, "PUT_AURA_SOCKET_REMOVE");
										}
										else
											LocaleChatPacket(CHAT_TYPE_INFO, 499, "");
									}
									break;
#endif
							}
							if (item2->IsEquipped())
							{
								BuffOnAttr_AddBuffsFromItem(item2);
							}
						}
						break;

					case USE_BAIT:
						{
							if (m_pkFishingEvent)
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 488, "");
								return false;
							}

							LPITEM weapon = GetWear(WEAR_WEAPON);

							if (!weapon || weapon->GetType() != ITEM_ROD)
								return false;

							if (weapon->GetSocket(2))
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 489, "%s", item->GetLocaleName());
							}
							else
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 490, "%s", item->GetLocaleName());
							}

							weapon->SetSocket(2, item->GetValue(0));
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_MOVE:
					case USE_TREASURE_BOX:
					case USE_MONEYBAG:
						break;

					case USE_AFFECT:
						{
							if (FindAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType))
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
							}
							else
							{
								AddAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false);
								item->SetCount(item->GetCount() - 1);
							}
						}
						break;

#ifdef ENABLE_RENEWAL_AFFECT
					case USE_AFFECT_PLUS:
						{
							int affect_type = GetAffectType(item);
							int apply_type = aApplyInfo[item->GetValue(0)].bPointType;
							int apply_value = item->GetValue(1);
							bool active_blend = (item->GetSocket(1) == 1) ? true : false;
							int apply_duration = item->GetSocket(2);

							if (!active_blend && (((affect_type == AFFECT_BLEND || affect_type == AFFECT_BLEND_PLUS) && (FindAffect(AFFECT_BLEND, apply_type) || FindAffect(AFFECT_BLEND_PLUS, apply_type)))
								|| ((affect_type == AFFECT_MALL_PLUS && (FindAffect(AFFECT_MALL, apply_type) || FindAffect(AFFECT_MALL_PLUS, apply_type)))))
							)
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
								return false;
							}

							if ((affect_type == AFFECT_BLEND_PLUS || affect_type == AFFECT_MALL_PLUS))
							{
								if (active_blend)
								{
									RemoveAffect(FindAffect(affect_type, apply_type));
									item->Lock(false);
									item->SetSocket(1, 0);
								}
								else
								{
									AddAffect(affect_type, apply_type, apply_value, 0, INFINITE_AFFECT_DURATION, 0, false);
									item->Lock(true);
									item->SetSocket(1, 1);
								}
							}
							else
							{
								AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
								item->SetCount(item->GetCount() - 1);
							}
						}
						break;
#endif

					case USE_CREATE_STONE:
						AutoGiveItem(number(28000, 28013));
						item->SetCount(item->GetCount() - 1);
						break;

					case USE_RECIPE:
						{
							LPITEM pSource1 = FindSpecifyItem(item->GetValue(1));
							DWORD dwSourceCount1 = item->GetValue(2);

							LPITEM pSource2 = FindSpecifyItem(item->GetValue(3));
							DWORD dwSourceCount2 = item->GetValue(4);

							if (dwSourceCount1 != 0)
							{
								if (pSource1 == NULL)
								{
									LocaleChatPacket(CHAT_TYPE_INFO, 491, "");
									return false;
								}
							}

							if (dwSourceCount2 != 0)
							{
								if (pSource2 == NULL)
								{
									LocaleChatPacket(CHAT_TYPE_INFO, 491, "");
									return false;
								}
							}

							if (pSource1 != NULL)
							{
								if (pSource1->GetCount() < dwSourceCount1)
								{
									LocaleChatPacket(CHAT_TYPE_INFO, 492, "%s", pSource1->GetLocaleName());
									return false;
								}

								pSource1->SetCount(pSource1->GetCount() - dwSourceCount1);
							}

							if (pSource2 != NULL)
							{
								if (pSource2->GetCount() < dwSourceCount2)
								{
									LocaleChatPacket(CHAT_TYPE_INFO, 492, "%s", pSource2->GetLocaleName());
									return false;
								}

								pSource2->SetCount(pSource2->GetCount() - dwSourceCount2);
							}

							LPITEM pBottle = FindSpecifyItem(50901);

							if (!pBottle || pBottle->GetCount() < 1)
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 493, "");
								return false;
							}

							pBottle->SetCount(pBottle->GetCount() - 1);

							if (number(1, 100) > item->GetValue(5))
							{
								LocaleChatPacket(CHAT_TYPE_INFO, 494, "");
								return false;
							}

							AutoGiveItem(item->GetValue(0));
						}
						break;
				}
			}
			break;

		case ITEM_METIN:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging())
					return false;

				if (item2->IsEquipped()) //Fix
					return false;

				if (item2->GetType() == ITEM_PICK) return false;
				if (item2->GetType() == ITEM_ROD) return false;

				int i;

				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				{
					DWORD dwVnum;   

					if ((dwVnum = item2->GetSocket(i)) <= 2)
						continue;

					TItemTable * p = ITEM_MANAGER::instance().GetTable(dwVnum);

					if (!p)
						continue;

					if (item->GetValue(5) == p->alValues[5])
					{
						LocaleChatPacket(CHAT_TYPE_INFO, 498, "");
						return false;
					}
				}

				if (item2->GetType() == ITEM_ARMOR)
				{
					if (!IS_SET(item->GetWearFlag(), WEARABLE_BODY) || !IS_SET(item2->GetWearFlag(), WEARABLE_BODY))
					{
						LocaleChatPacket(CHAT_TYPE_INFO, 499, "");
						return false;
					}
				}
				else if (item2->GetType() == ITEM_WEAPON)
				{
					if (!IS_SET(item->GetWearFlag(), WEARABLE_WEAPON))
					{
						LocaleChatPacket(CHAT_TYPE_INFO, 500, "");
						return false;
					}
				}
				else
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 501, "");
					return false;
				}

				for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					if (item2->GetSocket(i) >= 1 && item2->GetSocket(i) <= 2 && item2->GetSocket(i) >= item->GetValue(2))
					{
						if (number(1, 100) <= 30)
						{
							LocaleChatPacket(CHAT_TYPE_INFO, 502, "");
							item2->SetSocket(i, item->GetVnum());
						}
						else
						{
							LocaleChatPacket(CHAT_TYPE_INFO, 503, "");
							item2->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
						}

						LogManager::instance().ItemLog(this, item2, "SOCKET", item->GetName());
						item->SetCount(item->GetCount() - 1);
						break;
					}

				if (i == ITEM_SOCKET_MAX_NUM)
					LocaleChatPacket(CHAT_TYPE_INFO, 501, "");
			}
			break;

		case ITEM_AUTOUSE:
		case ITEM_MATERIAL:
		case ITEM_SPECIAL:
		case ITEM_TOOL:
		case ITEM_LOTTERY:
			break;

		case ITEM_TOTEM:
			{
				if (!item->IsEquipped())
					EquipItem(item);
			}
			break;

		case ITEM_BLEND:
			// Fix
			if ((CheckTimeUsed(item) == false))
				return false;

			if (Blend_Item_find(item->GetVnum()))
			{
				int affect_type = AFFECT_BLEND;
				if (item->GetSocket(0) >= (int)_countof(aApplyInfo))
				{
					sys_err ("INVALID BLEND ITEM(id : %d, vnum : %d). APPLY TYPE IS %d.", item->GetID(), item->GetVnum(), item->GetSocket(0));
					return false;
				}

				int apply_type = aApplyInfo[item->GetSocket(0)].bPointType;
				int apply_value = item->GetSocket(1);
				int apply_duration = item->GetSocket(2);

				if (FindAffect(affect_type, apply_type))
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
				}
				else
				{
					if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, POINT_RESIST_MAGIC))
					{
						LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
					}
					else
					{
						AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
						item->SetCount(item->GetCount() - 1);
					}
				}
			}
			break;

		case ITEM_EXTRACT:
			{
				LPITEM pDestItem = GetItem(DestCell);
				if (NULL == pDestItem)
				{
					return false;
				}
				switch (item->GetSubType())
				{
					case EXTRACT_DRAGON_SOUL:
						if (pDestItem->IsDragonSoul())
						{
							return DSManager::instance().PullOut(this, NPOS, pDestItem, item);
						}
						return false;

					case EXTRACT_DRAGON_HEART:
						if (pDestItem->IsDragonSoul())
						{
							return DSManager::instance().ExtractDragonHeart(this, pDestItem, item);
						}
						return false;

					default:
						return false;
				}
			}
			break;

		case ITEM_NONE:
			sys_err("Item type NONE %s", item->GetName());
			break;

		default:
			sys_log(0, "UseItemEx: Unknown type %s %d", item->GetName(), item->GetType());
			return false;
	}

	return true;
}

int g_nPortalLimitTime = 10;

#ifdef ENABLE_SKILL_BOOK_READING
int CHARACTER::BKBul(long skillindex) const
{
	int	cell = 999;
	LPITEM item;

	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		item = GetInventoryItem(i);
		if (NULL != item && item->GetSocket(0) == skillindex && item->GetVnum() == 50300)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				cell = item->GetCell();
			}
		}
	}

	return cell;
}
#endif

bool CHARACTER::UseItem(TItemPos Cell, TItemPos DestCell)
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	LPITEM item;

	if (!CanHandleItem())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
			return false;

	sys_log(0, "%s: USE_ITEM %s (inven %d, cell: %d)", GetName(), item->GetName(), window_type, wCell);

	if (item->IsExchanging())
		return false;

#ifdef ENABLE_RENEWAL_SWITCHBOT
	if (Cell.IsSwitchbotPosition())
	{
		CSwitchbot* pkSwitchbot = CSwitchbotManager::Instance().FindSwitchbot(GetPlayerID());
		if (pkSwitchbot && pkSwitchbot->IsActive(Cell.cell))
		{
			return false;
		}

		int iEmptyCell = GetEmptyInventory(item->GetSize());

		if (iEmptyCell == -1)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}
#endif

#ifdef ENABLE_LUA_ESC_BEHAVIOR_FIX
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 797, "");
		GetDesc()->DelayedDisconnect(3);
		return false;
	}
#endif

	if (!item->CanUsedBy(this))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (IsStun())
		return false;

	if (false == FN_check_item_sex(this, item))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (IS_SUMMON_ITEM(item->GetVnum()))
	{
		if (false == IS_SUMMONABLE_ZONE(GetMapIndex()))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
			return false;
		}

		int iPulse = thecore_pulse();
		if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 504, "%d", g_nPortalLimitTime);
			return false;
		}

		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			|| IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			|| GetTransmutation()
#endif
			)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 74, "");
			return false;
		}

		{
			if (iPulse - GetRefineTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 60, "%d", g_nPortalLimitTime);
				return false;
			}
		}

		{
			if (iPulse - GetMyShopTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 504, "%d", g_nPortalLimitTime);
				return false;
			}
		}

		if (item->GetVnum() != 70302)
		{
			PIXEL_POSITION posWarp;

			int x = 0;
			int y = 0;

			double nDist = 0;
			const double nDistant = 5000.0;

			if (item->GetVnum() == 22010)
			{
				x = item->GetSocket(0) - GetX();
				y = item->GetSocket(1) - GetY();
			}
			else if (item->GetVnum() == 22000) 
			{
				SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp);

				if (item->GetSocket(0) == 0)
				{
					x = posWarp.x - GetX();
					y = posWarp.y - GetY();
				}
				else
				{
					x = item->GetSocket(0) - GetX();
					y = item->GetSocket(1) - GetY();
				}
			}

			nDist = sqrt(pow((float)x,2) + pow((float)y,2));

			if (nDistant > nDist)
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 278, "");
				return false;
			}
		}

		if (iPulse - GetExchangeTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 60, "%d", g_nPortalLimitTime);
			return false;
		}
	}

	if (item->GetVnum() == 50200 | item->GetVnum() == 71049)
	{
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			|| IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			|| GetTransmutation()
#endif
			)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 74, "");
			return false;
		}
	}

#ifdef ENABLE_RENEWAL_AFFECT
	if (item->GetType() == ITEM_BLEND || (item->GetType() == ITEM_USE && (item->GetSubType() == USE_AFFECT || item->GetSubType() == USE_AFFECT_PLUS)))
	{
		int apply_type;
		if (item->GetType() == ITEM_BLEND)
			apply_type = aApplyInfo[item->GetSocket(0)].bPointType;
		else if (item->GetType() == ITEM_USE && (item->GetSubType() == USE_AFFECT || item->GetSubType() == USE_AFFECT_PLUS))
			apply_type = aApplyInfo[item->GetValue(1)].bPointType;
		else
			apply_type = aApplyInfo[item->GetValue(0)].bPointType;

		if ((item->IsItemBlend() && FindAffect(AFFECT_BLEND_PLUS, apply_type))
			|| (item->IsItemBlendPlus() && FindAffect(AFFECT_BLEND, apply_type))
			|| (item->IsItemDragonGod() && FindAffect(AFFECT_MALL_PLUS, apply_type))
			|| (item->IsItemDragonGodPlus() && FindAffect(AFFECT_MALL, apply_type))
		)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 399, "");
			return false;
		}
	}
#endif

	if (IS_SET(item->GetFlag(), ITEM_FLAG_LOG))
	{
		DWORD vid = item->GetVID();
		DWORD oldCount = item->GetCount();
		DWORD vnum = item->GetVnum();

		char hint[ITEM_NAME_MAX_LEN + 32 + 1];
		int len = snprintf(hint, sizeof(hint) - 32, "%s", item->GetName());

		if (len < 0 || len >= (int) sizeof(hint) - 32)
			len = (sizeof(hint) - 32) - 1;

		bool ret = UseItemEx(item, DestCell);

#ifdef ENABLE_RENEWAL_BATTLE_PASS
		if (ret and item->GetType() == ITEM_USE or ret and item->GetType() == ITEM_SKILLBOOK or ret and item->GetType() == ITEM_GIFTBOX)
			UpdateExtBattlePassMissionProgress(BP_ITEM_USE, 1, item->GetVnum());
#endif

		if (NULL == ITEM_MANAGER::instance().FindByVID(vid))
		{
			LogManager::instance().ItemLog(this, vid, vnum, "REMOVE", hint);
		}
		else if (oldCount != item->GetCount())
		{
			snprintf(hint + len, sizeof(hint) - len, " %u", oldCount - 1);
			LogManager::instance().ItemLog(this, vid, vnum, "USE_ITEM", hint);
		}
		return (ret);
	}
	else
#ifdef ENABLE_RENEWAL_BATTLE_PASS
	{
		bool ret = UseItemEx(item, DestCell);

		if (ret and item->GetType() == ITEM_USE or ret and item->GetType() == ITEM_SKILLBOOK or ret and item->GetType() == ITEM_GIFTBOX)
			UpdateExtBattlePassMissionProgress(BP_ITEM_USE, 1, item->GetVnum());

		return (ret);
	}
#else
		return UseItemEx(item, DestCell);
#endif
}

#ifdef ENABLE_STACK_LIMIT
bool CHARACTER::DropItem(TItemPos Cell, WORD bCount)
#else
bool CHARACTER::DropItem(TItemPos Cell, BYTE bCount)
#endif
{
	LPITEM item = NULL; 

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (true == item->isLocked())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP | ITEM_ANTIFLAG_GIVE))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 505, "");
		return false;
	}

	if (bCount == 0 || bCount > item->GetCount())
#ifdef ENABLE_STACK_LIMIT
		bCount = (WORD)item->GetCount();
#else
		bCount = (BYTE)item->GetCount();
#endif

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);

	LPITEM pkItemToDrop;

	if (bCount == item->GetCount())
	{
		item->RemoveFromCharacter();
		pkItemToDrop = item;
	}
	else
	{
		if (bCount == 0)
		{
			if (test_server)
				sys_log(0, "[DROP_ITEM] drop item count == 0");
			return false;
		}

		item->SetCount(item->GetCount() - bCount);
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		pkItemToDrop = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), bCount);

		FN_copy_item_socket(pkItemToDrop, item);

		char szBuf[51 + 1];
		snprintf(szBuf, sizeof(szBuf), "%u %u", pkItemToDrop->GetID(), pkItemToDrop->GetCount());
		LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
	}

	PIXEL_POSITION pxPos = GetXYZ();

	if (thecore_pulse() > LastDropTime + 25)
		CountDrops = 0;

	if (thecore_pulse() < LastDropTime + 25 && CountDrops >= 4)
	{
		CountDrops = 0;
		sys_err("%s[%d] has been disconnected because of drophack using", GetName(), GetPlayerID());
		GetDesc()->SetPhase(PHASE_CLOSE);
		return false;
	}

	if (pkItemToDrop->AddToGround(GetMapIndex(), pxPos))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 506, "");
		pkItemToDrop->StartDestroyEvent(30);

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemToDrop);

		char szHint[32 + 1];
		snprintf(szHint, sizeof(szHint), "%s %u %u", pkItemToDrop->GetName(), pkItemToDrop->GetCount(), pkItemToDrop->GetOriginalVnum());
		LogManager::instance().ItemLog(this, pkItemToDrop, "DROP", szHint);

		LastDropTime = thecore_pulse();
		CountDrops++;
	}

	return true;
}

#ifdef ENABLE_DESTROY_DIALOG
#ifdef ENABLE_STACK_LIMIT
bool CHARACTER::DestroyItem(TItemPos Cell, WORD bCount)
#else
bool CHARACTER::DestroyItem(TItemPos Cell, BYTE bCount)
#endif
{
	bCount = abs(bCount);

	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (true == item->isLocked())
		return false;

	if(IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DESTROY))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 507, "");
		return false;
	}

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if (item->GetCount() <= 0)
		return false;

	if (bCount == 0 || bCount > item->GetCount())
#ifdef ENABLE_STACK_LIMIT
		bCount = (WORD)item->GetCount();
#else
		bCount = (BYTE)item->GetCount();
#endif

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);
	ITEM_MANAGER::instance().RemoveItem(item);

	char buf[1024];
	char itemlink[256];
	int len;
	len = snprintf(itemlink, sizeof(itemlink), "item:%x:%x", item->GetVnum(), item->GetFlag());

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x", item->GetSocket(i));

	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		if (item->GetAttributeType(i) != 0)
			len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x:%d", item->GetAttributeType(i), item->GetAttributeValue(i));

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	const char* itemLocaleName = item->GetLocaleName(GetDesc()->GetLanguage()); // Fix
	snprintf(buf, sizeof(buf), "|cffffc700|H%s|h[%s]|h|r", itemlink, itemLocaleName);
#else
	snprintf(buf, sizeof(buf), "|cffffc700|H%s|h[%s]|h|r", itemlink, item->GetLocaleName());
#endif

	LocaleChatPacket(CHAT_TYPE_INFO, 508, "%s", buf);

	return true;
}
#endif

#ifdef ENABLE_QUICK_SELL_ITEM
bool CHARACTER::SellItem(TItemPos Cell)
{
	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging() || item->IsEquipped())
		return false;

	if (true == item->isLocked())
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SELL))
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if (item->GetCount() <= 0)
		return false;

#ifdef ENABLE_GOLD_LIMIT
	long long dwPrice = item->GetShopBuyPrice();
#else
	DWORD dwPrice = item->GetShopBuyPrice();
#endif

	dwPrice *= item->GetCount();

#ifdef ENABLE_GOLD_LIMIT
	const long long nTotalMoney = static_cast<long long>(GetGold()) + static_cast<long long>(dwPrice);
#else
	const int64_t nTotalMoney = static_cast<int64_t>(GetGold()) + static_cast<int64_t>(dwPrice);
#endif

	if (GOLD_MAX <= nTotalMoney)
	{
#ifdef ENABLE_GOLD_LIMIT
		sys_err("[OVERFLOW_GOLD] OriGold %lld AddedGold %lld id %u Name %s ", GetGold(), dwPrice, GetPlayerID(), GetName());
#else
		sys_err("[OVERFLOW_GOLD] OriGold %d AddedGold %d id %u Name %s ", GetGold(), dwPrice, GetPlayerID(), GetName());
#endif
		LocaleChatPacket(CHAT_TYPE_INFO, 509, "");
		return false;
	}

	item->SetCount(item->GetCount() - item->GetCount());
	PointChange(POINT_GOLD, dwPrice, false);

	char buf[1024];
	char itemlink[256];
	int len;
	len = snprintf(itemlink, sizeof(itemlink), "item:%x:%x", item->GetVnum(), item->GetFlag());

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x", item->GetSocket(i));

	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		if (item->GetAttributeType(i) != 0)
			len += snprintf(itemlink + len, sizeof(itemlink) - len, ":%x:%d", item->GetAttributeType(i), item->GetAttributeValue(i));

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	const char* itemLocaleName = item->GetLocaleName(GetDesc()->GetLanguage()); // Fix
	snprintf(buf, sizeof(buf), "|cffffc700|H%s|h[%s]|h|r", itemlink, itemLocaleName);
#else
	snprintf(buf, sizeof(buf), "|cffffc700|H%s|h[%s]|h|r", itemlink, item->GetLocaleName());
#endif

	LocaleChatPacket(CHAT_TYPE_INFO, 510, "%s", buf);

	return true;
}
#endif

bool CHARACTER::DropGold(int gold)
{
	if (gold <= 0 || gold > GetGold())
		return false;

	if (!CanHandleItem())
		return false;

	if (0 != g_GoldDropTimeLimitValue)
	{
		if (get_dword_time() < m_dwLastGoldDropTime+g_GoldDropTimeLimitValue)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 313, "");
			return false;
		}
	}

	m_dwLastGoldDropTime = get_dword_time();

	LPITEM item = ITEM_MANAGER::instance().CreateItem(1, gold);

	if (item)
	{
		PIXEL_POSITION pos = GetXYZ();

		if (item->AddToGround(GetMapIndex(), pos))
		{
			PointChange(POINT_GOLD, -gold, true);

			if (gold > 1000)
				LogManager::instance().CharLog(this, gold, "DROP_GOLD", "");

			item->StartDestroyEvent(150);
			LocaleChatPacket(CHAT_TYPE_INFO, 511, "%d", 150/60);
		}

		Save();
		return true;
	}

	return false;
}

#ifdef ENABLE_STACK_LIMIT
bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell, WORD count)
#else
bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell, BYTE count)
#endif
{
	LPITEM item = NULL;

	if (!IsValidItemPosition(Cell))
		return false;

	if (!(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

	if (item->GetCount() < count)
		return false;

	if (INVENTORY == Cell.window_type && Cell.cell >= INVENTORY_MAX_NUM && IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;

	if (true == item->isLocked())
		return false;

	if (!IsValidItemPosition(DestCell))
	{
		return false;
	}

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");

#ifdef ENABLE_AURA_COSTUME_SYSTEM
		if (IsAuraRefineWindowOpen())
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
#endif

		return false;
	}

	// At the request of the planner, only certain types of items can be placed in the belt inventory.
	if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(item))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (Cell.IsEquipPosition())
	{
		if (!CanUnequipNow(item))
			return false;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		int iWearCell = item->FindEquipCell(this);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
				return false;
			}

			if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
				return UnequipItem(item);
		}
#endif
	}

#ifdef ENABLE_RENEWAL_SWITCHBOT
	if (Cell.IsSwitchbotPosition() && CSwitchbotManager::Instance().IsActive(GetPlayerID(), Cell.cell))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (DestCell.IsSwitchbotPosition() && !SwitchbotHelper::IsValidItem(item))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 512, "");
		return false;
	}
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	if (DestCell.IsSkillBookInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (DestCell.IsUpgradeItemsInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (DestCell.IsStoneInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (DestCell.IsGiftBoxInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (DestCell.IsChangersInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if ((Cell.IsSkillBookInventoryPosition() && !DestCell.IsSkillBookInventoryPosition() && !DestCell.IsDefaultInventoryPosition()))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (Cell.IsUpgradeItemsInventoryPosition() && !DestCell.IsUpgradeItemsInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (Cell.IsStoneInventoryPosition() && !DestCell.IsStoneInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (Cell.IsGiftBoxInventoryPosition() && !DestCell.IsGiftBoxInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (Cell.IsChangersInventoryPosition() && !DestCell.IsChangersInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsSkillBookInventoryPosition())
	{
		if (!item->IsSkillBook())
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsUpgradeItemsInventoryPosition())
	{
		if (!item->IsUpgradeItem())
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsStoneInventoryPosition())
	{
		if (!item->IsStone())
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsGiftBoxInventoryPosition())
	{
		if (!item->IsGiftBox())
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsChangersInventoryPosition())
	{
		if (!item->IsChanger())
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
			return false;
		}
	}
#endif

	if (Cell.IsEquipPosition() && !CanUnequipNow(item))
		return false;

	if (DestCell.IsEquipPosition())
	{
		if (GetItem(DestCell))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 513, "");
			return false;
		}
		EquipItem(item, DestCell.cell - INVENTORY_MAX_NUM);
	}
	else
	{
		if (item->IsDragonSoul())
		{
			if (item->IsEquipped())
			{
				return DSManager::instance().PullOut(this, DestCell, item);
			}
			else
			{
				if (DestCell.window_type != DRAGON_SOUL_INVENTORY)
				{
					return false;
				}
				if (!DSManager::instance().IsValidCellForThisItem(item, DestCell))
					return false;
			}
		}
		else if (DRAGON_SOUL_INVENTORY == DestCell.window_type)
			return false;

		LPITEM item2;

		if ((item2 = GetItem(DestCell)) && item != item2 && item2->IsStackable() && !IS_SET(item2->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && item2->GetVnum() == item->GetVnum())
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				if (item2->GetSocket(i) != item->GetSocket(i))
					return false;

			if (count == 0)
#ifdef ENABLE_STACK_LIMIT
				count = (WORD)item->GetCount();
#else
				count = (BYTE)item->GetCount();
#endif

			sys_log(0, "%s: ITEM_STACK %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell, 
				DestCell.window_type, DestCell.cell, count);

#ifdef ENABLE_STACK_LIMIT
			count = MIN(ITEM_MAX_COUNT - item2->GetCount(), count);
#else
			count = MIN(200 - item2->GetCount(), count);
#endif

			item->SetCount(item->GetCount() - count);
			item2->SetCount(item2->GetCount() + count);
			return true;
		}

		if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
			return false;

		if (count == 0 || count >= item->GetCount() || !item->IsStackable() || IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
		{
			sys_log(0, "%s: ITEM_MOVE %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell, DestCell.window_type, DestCell.cell, count);

			item->RemoveFromCharacter();

#ifdef ENABLE_PICKUP_ITEM_EFFECT
			SetItem(DestCell, item, false);
#else
			SetItem(DestCell, item);
#endif

#ifdef ENABLE_MOUNT_SYSTEM
			if(ITEM_MOUNT == item->GetType())
				quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
#endif

#ifdef ENABLE_PET_SYSTEM
			if(ITEM_PET == item->GetType())
				quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
#endif

#ifdef ENABLE_TITLE_SYSTEM
			if(ITEM_TITLE == item->GetType())
				quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
#endif

			if (INVENTORY == Cell.window_type && INVENTORY == DestCell.window_type)
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, DestCell.cell);
		}
		else if (count < item->GetCount())
		{
			sys_log(0, "%s: ITEM_SPLIT %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell, DestCell.window_type, DestCell.cell, count);

			item->SetCount(item->GetCount() - count);
			LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), count);

			FN_copy_item_socket(item2, item);

#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item2->AddToCharacter(this, DestCell, false);
#else
			item2->AddToCharacter(this, DestCell);
#endif

			char szBuf[51+1];
			snprintf(szBuf, sizeof(szBuf), "%u %u %u %u ", item2->GetID(), item2->GetCount(), item->GetCount(), item->GetCount() + item2->GetCount());
			LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
		}
	}

	return true;
}

namespace NPartyPickupDistribute
{
	struct FFindOwnership
	{
		LPITEM item;
		LPCHARACTER owner;

		FFindOwnership(LPITEM item) 
			: item(item), owner(NULL)
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (item->IsOwnership(ch))
				owner = ch;
		}
	};

	struct FCountNearMember
	{
		int		total;
		int		x, y;

		FCountNearMember(LPCHARACTER center )
			: total(0), x(center->GetX()), y(center->GetY())
		{
		}

		void operator () (LPCHARACTER ch)
		{
			if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				total += 1;
		}
	};

	struct FMoneyDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		int		iMoney;

		FMoneyDistributor(LPCHARACTER center, int iMoney) 
			: total(0), c(center), x(center->GetX()), y(center->GetY()), iMoney(iMoney) 
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch!=c)
				if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange(POINT_GOLD, iMoney, true);

					if (iMoney > 1000)
						LogManager::instance().CharLog(ch, iMoney, "GET_GOLD", "");
				}
		}
	};
}

#ifdef ENABLE_GOLD_LIMIT
void CHARACTER::GiveGold(long long iAmount)
#else
void CHARACTER::GiveGold(int iAmount)
#endif
{
	if (iAmount <= 0)
		return;

#ifdef ENABLE_GOLD_LIMIT
	sys_log(0, "GIVE_GOLD: %s %lld", GetName(), iAmount);
#else
	sys_log(0, "GIVE_GOLD: %s %d", GetName(), iAmount);
#endif

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

#ifdef ENABLE_GOLD_LIMIT
		long long dwTotal = iAmount;
		long long dwMyAmount = dwTotal;
#else
		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;
#endif

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnMapMember(funcCountNearMember, GetMapIndex());

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FMoneyDistributor funcMoneyDist(this, dwShare);

			pParty->ForEachOnMapMember(funcMoneyDist, GetMapIndex());
		}

		PointChange(POINT_GOLD, dwMyAmount, true);

#ifdef ENABLE_RENEWAL_BATTLE_PASS
		UpdateExtBattlePassMissionProgress(YANG_COLLECT, dwMyAmount, GetMapIndex());
#endif

		if (dwMyAmount > 1000)
			LogManager::instance().CharLog(this, dwMyAmount, "GET_GOLD", "");
	}
	else
	{
		PointChange(POINT_GOLD, iAmount, true);

#ifdef ENABLE_RENEWAL_BATTLE_PASS
		UpdateExtBattlePassMissionProgress(YANG_COLLECT, iAmount, GetMapIndex());
#endif

		if (iAmount > 1000)
			LogManager::instance().CharLog(this, iAmount, "GET_GOLD", "");
	}
}

bool CHARACTER::PickupItem(DWORD dwVID)
{
	LPITEM item = ITEM_MANAGER::instance().FindByVID(dwVID);

	if (IsObserverMode())
		return false;

	if (!item || !item->GetSectree())
		return false;

	if (item->DistanceValid(this))
	{
		if (item->IsOwnership(this))
		{

#ifdef ENABLE_AUTO_SELL_SYSTEM
			if (GetAutoSellStatus() && IsAutoSellItem(item->GetVnum()))
			{
				int price = item->GetISellItemPrice();
				PointChange(POINT_GOLD, price);
				ChatPacket(CHAT_TYPE_INFO, "<Otomatik Satma> %d yang kazandin", price);
				item->RemoveFromGround();
				M2_DESTROY_ITEM(item);
				return true;
			}
#endif
			if (item->GetType() == ITEM_ELK)
			{
				GiveGold(item->GetCount());
				item->RemoveFromGround();
				M2_DESTROY_ITEM(item);

				Save();
			}
			else
			{
				if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
				{
#ifdef ENABLE_STACK_LIMIT
					WORD bCount = item->GetCount();
#else
					BYTE bCount = item->GetCount();
#endif
					for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
					{
						LPITEM item2 = GetInventoryItem(i);

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

#ifdef ENABLE_STACK_LIMIT
							WORD bCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), bCount);
#else
							BYTE bCount2 = MIN(200 - item2->GetCount(), bCount);
#endif
							bCount -= bCount2;

							item2->SetCount(item2->GetCount() + bCount2);

							if (bCount == 0)
							{
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
								SendPickupItemPacket(item2->GetVnum());
#else
								LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item2->GetLocaleName());
#endif
								M2_DESTROY_ITEM(item);
								if (item2->GetType() == ITEM_QUEST)
									quest::CQuestManager::instance().PickupItem (GetPlayerID(), item2);
								return true;
							}
						}
					}

					for (int i = BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END; ++i) // Fix
					{
						LPITEM item2 = GetInventoryItem(i);

						if(!item2)
							continue;

						if (item2->GetVnum() == item->GetVnum())
						{
							int j;

							for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
								if (item2->GetSocket(j) != item->GetSocket(j))
									break;

							if (j != ITEM_SOCKET_MAX_NUM)
								continue;

#ifdef ENABLE_STACK_LIMIT
							WORD bCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), bCount);
#else
							BYTE bCount2 = MIN(200 - item2->GetCount(), bCount);
#endif
							bCount -= bCount2;

							item2->SetCount(item2->GetCount() + bCount2);

							if (bCount == 0)
							{
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
								SendPickupItemPacket(item2->GetVnum());
#else
								LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item2->GetLocaleName());
#endif
								M2_DESTROY_ITEM(item);
								if (item2->GetType() == ITEM_QUEST)
									quest::CQuestManager::instance().PickupItem (GetPlayerID(), item2);
								return true;
							}
						}
					}

					item->SetCount(bCount);
				}

#ifdef ENABLE_SPECIAL_INVENTORY
				if ((item->IsSkillBook() || item->IsUpgradeItem() || item->IsStone() || item->IsGiftBox() || item->IsChanger()) && item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
				{
#ifdef ENABLE_STACK_LIMIT
					WORD bCount = item->GetCount();
#else
					BYTE bCount = item->GetCount();
#endif
					for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
					{
						LPITEM item2 = GetInventoryItem(i);

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

#ifdef ENABLE_STACK_LIMIT
							WORD bCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), bCount);
#else
							BYTE bCount2 = MIN(200 - item2->GetCount(), bCount);
#endif
							bCount -= bCount2;

							item2->SetCount(item2->GetCount() + bCount2);

							if (bCount == 0)
							{
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
								SendPickupItemPacket(item2->GetVnum());
#else
								LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item2->GetLocaleName());
#endif
								M2_DESTROY_ITEM(item);
								if (item2->GetType() == ITEM_QUEST)
									quest::CQuestManager::instance().PickupItem (GetPlayerID(), item2);
								return true;
							}
						}
					}
					item->SetCount(bCount);
				}
#endif

				int iEmptyCell;
				if (item->IsDragonSoul())
				{
					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
#ifdef ENABLE_SPECIAL_INVENTORY
				else if (item->IsSkillBook())
				{
					if ((iEmptyCell = GetEmptySkillBookInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
				else if (item->IsUpgradeItem())
				{
					if ((iEmptyCell = GetEmptyUpgradeItemsInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
				else if (item->IsStone())
				{
					if ((iEmptyCell = GetEmptyStoneInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
				else if (item->IsGiftBox())
				{
					if ((iEmptyCell = GetEmptyGiftBoxInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
				else if (item->IsChanger())
				{
					if ((iEmptyCell = GetEmptyChangersInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
#endif
				else
				{
					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}

				item->RemoveFromGround();

				if (item->IsDragonSoul())
					item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
#ifdef ENABLE_SPECIAL_INVENTORY
				else if (item->IsSkillBook())
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
				else if (item->IsUpgradeItem())
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
				else if (item->IsStone())
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
				else if (item->IsGiftBox())
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
				else if (item->IsChanger())
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
#endif
				else
					item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));

				char szHint[32+1];
				snprintf(szHint, sizeof(szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
				LogManager::instance().ItemLog(this, item, "GET", szHint);
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
				SendPickupItemPacket(item->GetVnum());
#else
				LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item->GetLocaleName());
#endif

				if (item->GetType() == ITEM_QUEST)
					quest::CQuestManager::instance().PickupItem (GetPlayerID(), item);
			}

			return true;
		}
		else if (!IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_DROP) && GetParty())
		{
			NPartyPickupDistribute::FFindOwnership funcFindOwnership(item);

			GetParty()->ForEachOnMapMember(funcFindOwnership, GetMapIndex());

			LPCHARACTER owner = funcFindOwnership.owner;

			int iEmptyCell;

			if (item->IsDragonSoul())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyDragonSoulInventory(item)) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
						owner->LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
			}
#ifdef ENABLE_SPECIAL_INVENTORY
			else if (item->IsSkillBook())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptySkillBookInventory(item->GetSize())) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptySkillBookInventory(item->GetSize())) == -1)
					{
						owner->LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
			}
			else if (item->IsUpgradeItem())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyUpgradeItemsInventory(item->GetSize())) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyUpgradeItemsInventory(item->GetSize())) == -1)
					{
						owner->LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
			}
			else if (item->IsStone())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyStoneInventory(item->GetSize())) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyStoneInventory(item->GetSize())) == -1)
					{
						owner->LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
			}
			else if (item->IsGiftBox())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyGiftBoxInventory(item->GetSize())) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyGiftBoxInventory(item->GetSize())) == -1)
					{
						owner->LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
			}
			else if (item->IsChanger())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyChangersInventory(item->GetSize())) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyChangersInventory(item->GetSize())) == -1)
					{
						owner->LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
			}
#endif
			else
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyInventory(item->GetSize())) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
					{
						owner->LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
						return false;
					}
				}
			}

			item->RemoveFromGround();

			if (item->IsDragonSoul())
				item->AddToCharacter(owner, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
#ifdef ENABLE_SPECIAL_INVENTORY
			else if (item->IsSkillBook())
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
			else if (item->IsUpgradeItem())
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
			else if (item->IsStone())
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
			else if (item->IsGiftBox())
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
			else if (item->IsChanger())
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
#endif
			else
				item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));

			char szHint[32+1];
			snprintf(szHint, sizeof(szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
			LogManager::instance().ItemLog(owner, item, "GET", szHint);

			if (owner == this)
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
				SendPickupItemPacket(item->GetVnum());
#else
				LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item->GetLocaleName());
#endif
			else
			{
				owner->LocaleChatPacket(CHAT_TYPE_INFO, 523, "%s#%s", GetName(), item->GetLocaleName());
				LocaleChatPacket(CHAT_TYPE_INFO, 524, "%s#%s", owner->GetName(), item->GetLocaleName());
			}

			if (item->GetType() == ITEM_QUEST)
				quest::CQuestManager::instance().PickupItem (owner->GetPlayerID(), item);

			return true;
		}
	}

	return false;
}

bool CHARACTER::SwapItem(UINT bCell, UINT bDestCell)
{
	if (!CanHandleItem())
		return false;

	TItemPos srcCell(INVENTORY, bCell), destCell(INVENTORY, bDestCell);

	if (srcCell.IsDragonSoulEquipPosition() || destCell.IsDragonSoulEquipPosition())
		return false;

	if (bCell == bDestCell)
		return false;

	if (srcCell.IsEquipPosition() && destCell.IsEquipPosition())
		return false;

	LPITEM item1, item2;

	if (srcCell.IsEquipPosition())
	{
		item1 = GetInventoryItem(bDestCell);
		item2 = GetInventoryItem(bCell);
	}
	else
	{
		item1 = GetInventoryItem(bCell);
		item2 = GetInventoryItem(bDestCell);
	}

	if (!item1 || !item2)
		return false;

	if (item1 == item2)
	{
		sys_log(0, "[WARNING][WARNING][HACK USER!] : %s %d %d", m_stName.c_str(), bCell, bDestCell);
		return false;
	}

	if (!IsEmptyItemGrid(TItemPos (INVENTORY, item1->GetCell()), item2->GetSize(), item1->GetCell()))
		return false;

	if (TItemPos(EQUIPMENT, item2->GetCell()).IsEquipPosition())
	{
		BYTE bEquipCell = item2->GetCell() - INVENTORY_MAX_NUM;
		BYTE bInvenCell = item1->GetCell();

		if (false == CanUnequipNow(item2) || false == CanEquipNow(item1))
			return false;

		if (bEquipCell != item1->FindEquipCell(this))
			return false;

#ifdef ENABLE_MOUNT_SYSTEM
		if(ITEM_MOUNT == item2->GetType())
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item2, false);
#endif

#ifdef ENABLE_PET_SYSTEM
		if(ITEM_PET == item2->GetType())
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item2, false);
#endif

#ifdef ENABLE_TITLE_SYSTEM
		if(ITEM_TITLE == item2->GetType())
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item2, false);
#endif

		item2->RemoveFromCharacter();

#ifdef ENABLE_ITEM_SWAP_FIX
		if (item1->EquipTo(this, bEquipCell)) 
		{
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item2->AddToCharacter(this, TItemPos(INVENTORY, bInvenCell), false);
#else
			item2->AddToCharacter(this, TItemPos(INVENTORY, bInvenCell));
#endif
			item2->ModifyPoints(false); //item_swap fix ds_aim
			ComputePoints();			// item_swap fix ds_aim
		} 
		else
		{
			sys_err("SwapItem cannot equip %s! item1 %s", item2->GetName(), item1->GetName());
		}
#else
		if (item1->EquipTo(this, bEquipCell))
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item2->AddToCharacter(this, TItemPos(INVENTORY, bInvenCell), false);
#else
			item2->AddToCharacter(this, TItemPos(INVENTORY, bInvenCell));
#endif
		else
			sys_err("SwapItem cannot equip %s! item1 %s", item2->GetName(), item1->GetName());
#endif
	}
	else
	{
		UINT bCell1 = item1->GetCell();
		UINT bCell2 = item2->GetCell();

		item1->RemoveFromCharacter();
		item2->RemoveFromCharacter();

#ifdef ENABLE_PICKUP_ITEM_EFFECT
		item1->AddToCharacter(this, TItemPos(INVENTORY, bCell2), false);
		item2->AddToCharacter(this, TItemPos(INVENTORY, bCell1), false);
#else
		item1->AddToCharacter(this, TItemPos(INVENTORY, bCell2));
		item2->AddToCharacter(this, TItemPos(INVENTORY, bCell1));
#endif
	}

	return true;
}

bool CHARACTER::UnequipItem(LPITEM item)
{

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	int iWearCell = item->FindEquipCell(this);
	if (iWearCell == WEAR_WEAPON)
	{
		LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
		if (costumeWeapon && !UnequipItem(costumeWeapon))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
			return false;
		}
	}
#endif

	int pos;

	if (false == CanUnequipNow(item))
		return false;

	if (item->IsDragonSoul())
		pos = GetEmptyDragonSoulInventory(item);
#ifdef ENABLE_SPECIAL_INVENTORY
	else if (item->IsSkillBook())
		pos = GetEmptySkillBookInventory(item->GetSize());
	else if (item->IsUpgradeItem())
		pos = GetEmptyUpgradeItemsInventory(item->GetSize());
	else if (item->IsStone())
		pos = GetEmptyStoneInventory(item->GetSize());
	else if (item->IsGiftBox())
		pos = GetEmptyGiftBoxInventory(item->GetSize());
	else if (item->IsChanger())
		pos = GetEmptyChangersInventory(item->GetSize());
#endif
	else
		pos = GetEmptyInventory(item->GetSize());

	if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		ShowAlignment(true);

#ifdef ENABLE_MOUNT_SYSTEM
	if(ITEM_MOUNT == item->GetType())
		quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
#endif

#ifdef ENABLE_PET_SYSTEM
	if(ITEM_PET == item->GetType())
		quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
#endif

#ifdef ENABLE_TITLE_SYSTEM
	if(ITEM_TITLE == item->GetType())
		quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
#endif

	item->RemoveFromCharacter();
	if (item->IsDragonSoul())
		item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, pos));
#ifdef ENABLE_SPECIAL_INVENTORY
	else if (item->IsSkillBook())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		item->AddToCharacter(this, TItemPos(INVENTORY, pos), false);
#else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));
#endif
	else if (item->IsUpgradeItem())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		item->AddToCharacter(this, TItemPos(INVENTORY, pos), false);
#else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));
#endif
	else if (item->IsStone())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		item->AddToCharacter(this, TItemPos(INVENTORY, pos), false);
#else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));
#endif
	else if (item->IsGiftBox())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		item->AddToCharacter(this, TItemPos(INVENTORY, pos), false);
#else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));
#endif
	else if (item->IsChanger())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		item->AddToCharacter(this, TItemPos(INVENTORY, pos), false);
#else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));
#endif
#endif
	else
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		item->AddToCharacter(this, TItemPos(INVENTORY, pos), false);
#else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));
#endif

	CheckMaximumPoints();

	return true;
}

bool CHARACTER::EquipItem(LPITEM item, int iCandidateCell)
{
	if (item->IsExchanging())
		return false;

	if (false == item->IsEquipable())
		return false;

	if (false == CanEquipNow(item))
		return false;

	int iWearCell = item->FindEquipCell(this, iCandidateCell);

	if (iWearCell < 0)
		return false;

	if (iWearCell == WEAR_BODY && IsRiding() && (item->GetVnum() >= 11901 && item->GetVnum() <= 11904))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 95, "");
		return false;
	}

	if (iWearCell != WEAR_ARROW && IsPolymorphed())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 525, "");
		return false;
	}

	if (FN_check_item_sex(this, item) == false)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 526, "");
		return false;
	}

	if(item->IsRideItem() && IsRiding())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 282, "");
		return false;
	}

#ifdef ENABLE_ANTI_EQUIP_FLOOD
	if (!IsGM())
	{
		if (thecore_pulse() > GetEquipAntiFloodPulse() + PASSES_PER_SEC(1))
		{
			SetEquipAntiFloodCount(0);
			SetEquipAntiFloodPulse(thecore_pulse());
		}

		if (IncreaseEquipAntiFloodCount() >= 20)
		{
			GetDesc()->DelayedDisconnect(0);
			return false;
		}
	}
#endif

	// Fix
	if (item->GetCount() > 1)
	{
		item->SetCount(item->GetCount() - 1);
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);
		item2->EquipTo(this, iWearCell);

		if (iWearCell == 8)
			return false;
	}

	DWORD dwCurTime = get_dword_time();

	if (iWearCell != WEAR_ARROW && (dwCurTime - GetLastAttackTime() <= 1500 || dwCurTime - m_dwLastSkillTime <= 1500))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 527, "");
		return false;
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (iWearCell == WEAR_WEAPON)
	{
		if (item->GetType() == ITEM_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && costumeWeapon->GetValue(3) != item->GetSubType() && !UnequipItem(costumeWeapon))
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
				return false;
			}
		}
		else //fishrod/pickaxe
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
				return false;
			}
		}
	}
	else if (iWearCell == WEAR_COSTUME_WEAPON)
	{
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
		{
			LPITEM pkWeapon = GetWear(WEAR_WEAPON);
			if (!pkWeapon || pkWeapon->GetType() != ITEM_WEAPON || item->GetValue(3) != pkWeapon->GetSubType())
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 528, "");
				return false;
			}
		}
	}
#endif

	if (item->IsDragonSoul())
	{
		if(GetInventoryItem(INVENTORY_MAX_NUM + iWearCell))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 513, "");
			return false;
		}

		if (!item->EquipTo(this, iWearCell))
		{
			return false;
		}
	}
	else
	{
		if (GetWear(iWearCell) && !IS_SET(GetWear(iWearCell)->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		{
			if (item->GetWearFlag() == WEARABLE_ABILITY)
				return false;

			if (false == SwapItem(item->GetCell(), INVENTORY_MAX_NUM + iWearCell))
			{
				return false;
			}
		}
		else
		{
			BYTE bOldCell = item->GetCell();

			if (item->EquipTo(this, iWearCell))
			{
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, bOldCell, iWearCell);
			}
		}
	}

	if (true == item->IsEquipped())
	{
		// After the initial use of the item, time is deducted even if it is not used
		if (-1 != item->GetProto()->cLimitRealTimeFirstUseIndex)
		{
			// Whether the item has been used even once is determined by looking at Socket1. (Record the number of uses in Socket1)
			if (0 == item->GetSocket(1))
			{
				// Use the limit value as the default value for the available time, but if there is a value in Socket0, use that value. (Unit is seconds)
				long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[item->GetProto()->cLimitRealTimeFirstUseIndex].lValue;

				if (0 == duration)
					duration = 60 * 60 * 24 * 7;

				item->SetSocket(0, time(0) + duration);
				item->StartRealTimeExpireEvent();
			}

			item->SetSocket(1, item->GetSocket(1) + 1);
		}

		if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
			ShowAlignment(false);

		const DWORD& dwVnum = item->GetVnum();

		if (true == CItemVnumHelper::IsRamadanMoonRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_RAMADAN_RING);
		}

		else if (true == CItemVnumHelper::IsHalloweenCandy(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HALLOWEEN_CANDY);
		}

		else if (true == CItemVnumHelper::IsHappinessRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HAPPINESS_RING);
		}

		else if (true == CItemVnumHelper::IsLovePendant(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_LOVE_PENDANT);
		}
		else if (ITEM_UNIQUE == item->GetType() && 0 != item->GetSIGVnum())
		{
			const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(item->GetSIGVnum());
			if (NULL != pGroup)
			{
				const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(pGroup->GetAttrVnum(item->GetVnum()));
				if (NULL != pAttrGroup)
				{
					const std::string& std = pAttrGroup->m_stEffectFileName;
					SpecificEffectPacket(std.c_str());
				}
			}
		}

		if (UNIQUE_SPECIAL_RIDE == item->GetSubType() && IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE)
			|| (ITEM_UNIQUE == item->GetType() && UNIQUE_SPECIAL_MOUNT_RIDE == item->GetSubType() && IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE))
		)

		{
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
		}

#ifdef ENABLE_MOUNT_SYSTEM
		else if (ITEM_MOUNT == item->GetType())
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
#endif

#ifdef ENABLE_PET_SYSTEM
		else if (ITEM_PET == item->GetType())
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		else if ((item->GetType() == ITEM_COSTUME) && (item->GetSubType() == COSTUME_ACCE))
			this->EffectPacket(SE_EFFECT_ACCE_EQUIP);
#endif

#ifdef ENABLE_TITLE_SYSTEM
		else if(ITEM_TITLE == item->GetType())
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
#endif
	}

	return true;
}

void CHARACTER::BuffOnAttr_AddBuffsFromItem(LPITEM pItem)
{
	for (int i = 0; i < sizeof(g_aBuffOnAttrPoints)/sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->AddBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem)
{
	for (int i = 0; i < sizeof(g_aBuffOnAttrPoints)/sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->RemoveBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_ClearAll()
{
	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); it++)
	{
		CBuffOnAttributes* pBuff = it->second;
		if (pBuff)
		{
			pBuff->Initialize();
		}
	}
}

void CHARACTER::BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue)
{
	TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(bType);

	if (0 == bNewValue)
	{
		if (m_map_buff_on_attrs.end() == it)
			return;
		else
			it->second->Off();
	}
	else if(0 == bOldValue)
	{
		CBuffOnAttributes* pBuff;
		if (m_map_buff_on_attrs.end() == it)
		{
			switch (bType)
			{
			case POINT_ENERGY:
				{
					static BYTE abSlot[] =
					{
						WEAR_BODY,
						WEAR_HEAD,
						WEAR_FOOTS,
						WEAR_WRIST,
						WEAR_WEAPON,
						WEAR_NECK,
						WEAR_EAR,
						WEAR_SHIELD
						,WEAR_BELT
#ifdef ENABLE_PENDANT_SYSTEM
						,WEAR_PENDANT
#endif
					};
					static std::vector <BYTE> vec_slots (abSlot, abSlot + _countof(abSlot));
					pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
				}
				break;
			case POINT_COSTUME_ATTR_BONUS:
				{
					static BYTE abSlot[] =
					{
						WEAR_COSTUME_BODY,
						WEAR_COSTUME_HAIR
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
						,WEAR_COSTUME_WEAPON
#endif
					};

					static std::vector <BYTE> vec_slots (abSlot, abSlot + _countof(abSlot));
					pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
				}
				break;
			default:
				break;
			}
			m_map_buff_on_attrs.insert(TMapBuffOnAttrs::value_type(bType, pBuff));

		}
		else
			pBuff = it->second;
		pBuff->On(bNewValue);
	}
	else
	{
		if (m_map_buff_on_attrs.end() == it)
			return;
		else
			it->second->ChangeBuffValue(bNewValue);
	}
}

LPITEM CHARACTER::FindSpecifyItem(DWORD vnum) const
{
#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	for (int i = 0; i < (90 + (5*GetUnlockSlotsW())); ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
		if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum)
			return GetInventoryItem(i);

#ifdef ENABLE_SPECIAL_INVENTORY
	LPITEM item = NULL;
	for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
	{
		item = GetInventoryItem(i);
		if (item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
				continue;

			return item;
		}
	}
#endif

	return NULL;
}

LPITEM CHARACTER::FindItemByID(DWORD id) const
{
#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	for (int i = 0; i < (90 + (5*GetUnlockSlotsW())); ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

	for (int i=BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END ; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			return GetInventoryItem(i);
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	LPITEM item = NULL;
	for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
	{
		item = GetInventoryItem(i);
		if (item && item->GetID() == id)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
				continue;

			return item;
		}
	}
#endif

	return NULL;
}

#ifdef ENABLE_PENDANT_SYSTEM
int CHARACTER::CountSpecifyItem(DWORD vnum, int iExceptionCell) const
#else
int CHARACTER::CountSpecifyItem(DWORD vnum) const
#endif
{
	int	count = 0;
	LPITEM item;

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	for (int i = 0; i < (90 + (5*GetUnlockSlotsW())); ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
#ifdef ENABLE_PENDANT_SYSTEM
		if(i == iExceptionCell)
			continue;
#endif
		item = GetInventoryItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
	{
#ifdef ENABLE_PENDANT_SYSTEM
		if(i == iExceptionCell)
			continue;
#endif
		item = GetInventoryItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}
#endif

	return count;
}

#ifdef ENABLE_PENDANT_SYSTEM
void CHARACTER::RemoveSpecifyItem(DWORD vnum, DWORD count, int iExceptionCell)
#else
void CHARACTER::RemoveSpecifyItem(DWORD vnum, DWORD count)
#endif
{
	if (0 == count)
		return;

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	for (UINT i = 0; i < (90 + (5*GetUnlockSlotsW())); ++i)
#else
	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
#ifdef ENABLE_PENDANT_SYSTEM
		if(i == iExceptionCell)
			continue;
#endif
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetVnum() != vnum)
			continue;

		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (vnum >= 80003 && vnum <= 80007)
			LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	for (UINT i = SKILL_BOOK_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
	{
#ifdef ENABLE_PENDANT_SYSTEM
		if(i == iExceptionCell)
			continue;
#endif
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetVnum() != vnum)
			continue;

		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (vnum >= 80003 && vnum <= 80007)
			LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}
#endif

	if (count)
		sys_log(0, "CHARACTER::RemoveSpecifyItem cannot remove enough item vnum %u, still remain %d", vnum, count);
}

int CHARACTER::CountSpecifyTypeItem(BYTE type) const
{
	int count = 0;

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	for (int i = 0; i < (90 + (5*GetUnlockSlotsW())); ++i)
#else
	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem != NULL && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	for (UINT i = SKILL_BOOK_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem != NULL && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}
#endif

	return count;
}

void CHARACTER::RemoveSpecifyTypeItem(BYTE type, DWORD count)
{
	if (0 == count)
		return;

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	for (UINT i = 0; i < (90 + (5*GetUnlockSlotsW())); ++i)
#else
	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetType() != type)
			continue;

		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}

#ifdef ENABLE_SPECIAL_INVENTORY
	for (UINT i = SKILL_BOOK_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
	{
		if (NULL == GetInventoryItem(i))
			continue;

		if (GetInventoryItem(i)->GetType() != type)
			continue;

		if(m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
				continue;
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
				return;
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}
#endif
}

#ifdef ENABLE_PICKUP_ITEM_EFFECT
void CHARACTER::AutoGiveItem(LPITEM item, bool longOwnerShip, bool bHighlight)
#else
void CHARACTER::AutoGiveItem(LPITEM item, bool longOwnerShip)
#endif
{
	if (NULL == item)
	{
		sys_err ("NULL point.");
		return;
	}
	
	// Güvenlik: Ölü, stun veya NPC karakterlere item verilemez
	if (IsDead() || IsStun() || !IsPC())
	{
		M2_DESTROY_ITEM(item);
		return;
	}
	
	if (item->GetOwner())
	{
		sys_err ("item %d 's owner exists!", item->GetID());
		return;
	}

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
#ifdef ENABLE_STACK_LIMIT
	WORD bCount = item->GetCount();
#else
	BYTE bCount = item->GetCount();
#endif

	if (item->GetType() == ITEM_BLEND)
	{
		for (int i = 0; i < INVENTORY_MAX_NUM; i++)
		{
			LPITEM inv_item = GetInventoryItem(i);
			if (inv_item == NULL)
				continue;

			if (inv_item->GetType() == ITEM_BLEND && inv_item->GetVnum() == item->GetVnum())
			{
				if (inv_item->GetSocket(0) == item->GetSocket(0) && inv_item->GetSocket(1) == item->GetSocket(1) && inv_item->GetSocket(2) == item->GetSocket(2) && inv_item->GetCount() < ITEM_MAX_COUNT)
				{
					inv_item->SetCount(inv_item->GetCount() + item->GetCount());
					M2_DESTROY_ITEM(item);
					return;
				}
			}
		}
	}
#ifdef ENABLE_SPECIAL_INVENTORY
	else if ((item->IsSkillBook() || item->IsUpgradeItem() || item->IsStone() || item->IsGiftBox() || item->IsChanger()) && item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
	{
		for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item2 = GetInventoryItem(i);

			if (!item2)
				continue;

			// Beceri kitaplarý için socket kontrolü de yapýlmalý (socket[0]'daki skill vnum'ý)
			// Farklý skill vnum'larýna sahip beceri kitaplarý birleþtirilmemeli
			if (item2->GetVnum() == item->GetVnum())
			{
				// Beceri kitaplarý için socket[0] kontrolü (skill vnum)
				if (item->IsSkillBook())
				{
					if (item2->GetSocket(0) != item->GetSocket(0))
						continue;
				}

#ifdef ENABLE_STACK_LIMIT
				WORD bCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(200 - item2->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item2->SetCount(item2->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (item2->GetType() == ITEM_QUEST || item2->GetType() == ITEM_MATERIAL)
						quest::CQuestManager::instance().PickupItem(GetPlayerID(), item2);

					M2_DESTROY_ITEM(item);
					return;
				}
			}
		}
		
	}
#endif
	else
	{
		TItemTable * p = ITEM_MANAGER::instance().GetTable(item->GetVnum());
		if (!p)
		{
			M2_DESTROY_ITEM(item);
			return;
		}

		if (p->dwFlags & ITEM_FLAG_STACKABLE)
		{
			for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
			{
				LPITEM item2 = GetInventoryItem(i);

				if (!item2)
					continue;

				if (item2->GetVnum() == item->GetVnum() && FN_check_item_socket(item2))
				{
					if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
					{
						if (bCount < p->alValues[1])
							bCount = p->alValues[1];
					}

#ifdef ENABLE_STACK_LIMIT
					WORD bCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), bCount);
#else
					BYTE bCount2 = MIN(200 - item2->GetCount(), bCount);
#endif
					bCount -= bCount2;

					item2->SetCount(item2->GetCount() + bCount2);

					if (bCount == 0)
					{
						if (item2->GetType() == ITEM_QUEST || item2->GetType() == ITEM_MATERIAL)
							quest::CQuestManager::instance().PickupItem (GetPlayerID(), item2);

						M2_DESTROY_ITEM(item);
						return;
					}
				}
			}

			for (int i = BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END; ++i) // Fix
			{
				LPITEM item2 = GetInventoryItem(i);

				if(!item2)
					continue;

				if (item2->GetVnum() == item->GetVnum())
				{
					if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
					{
						if (bCount < p->alValues[1])
							bCount = p->alValues[1];
					}

#ifdef ENABLE_STACK_LIMIT
					WORD bCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), bCount);
#else
					BYTE bCount2 = MIN(200 - item2->GetCount(), bCount);
#endif
					bCount -= bCount2;

					item2->SetCount(item2->GetCount() + bCount2);

					if (bCount == 0)
					{
						if (item2->GetType() == ITEM_QUEST || item2->GetType() == ITEM_MATERIAL)
							quest::CQuestManager::instance().PickupItem (GetPlayerID(), item2);

						M2_DESTROY_ITEM(item);
						return;
					}
				}
			}
		}
	}

	item->SetCount(bCount);
#endif

	int cell;
	if (item->IsDragonSoul())
	{
		cell = GetEmptyDragonSoulInventory(item);
	}
#ifdef ENABLE_SPECIAL_INVENTORY
	else if (item->IsSkillBook())
	{
		cell = GetEmptySkillBookInventory(item->GetSize());
	}
	else if (item->IsUpgradeItem())
	{
		cell = GetEmptyUpgradeItemsInventory(item->GetSize());
	}
	else if (item->IsStone())
	{
		cell = GetEmptyStoneInventory(item->GetSize());
	}
	else if (item->IsGiftBox())
	{
		cell = GetEmptyGiftBoxInventory(item->GetSize());
	}
	else if (item->IsChanger())
	{
		cell = GetEmptyChangersInventory(item->GetSize());
	}
#endif
	else
	{
		cell = GetEmptyInventory (item->GetSize());
	}

	if (cell != -1)
	{
		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, cell));
#ifdef ENABLE_SPECIAL_INVENTORY
		else if (item->IsSkillBook())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, cell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
#endif
		else if (item->IsUpgradeItem())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, cell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
#endif
		else if (item->IsStone())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, cell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
#endif
		else if (item->IsGiftBox())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, cell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
#endif
		else if (item->IsChanger())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, cell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
#endif
#endif
		else
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, cell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
#endif

		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = cell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround (GetMapIndex(), GetXYZ());
		item->StartDestroyEvent();

		if (longOwnerShip)
			item->SetOwnership (this, 300);
		else
			item->SetOwnership (this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}
}

#ifdef ENABLE_STACK_LIMIT
#ifdef ENABLE_PICKUP_ITEM_EFFECT
LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, WORD bCount, int iRarePct, bool bMsg, bool bHighlight)
#else
LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, WORD bCount, int iRarePct, bool bMsg)
#endif
#else
#ifdef ENABLE_PICKUP_ITEM_EFFECT
LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, BYTE bCount, int iRarePct, bool bMsg, bool bHighlight)
#else
LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, BYTE bCount, int iRarePct, bool bMsg)
#endif
#endif
{
	// Güvenlik: Ölü, stun veya NPC karakterlere item verilemez
	if (IsDead() || IsStun() || !IsPC())
		return NULL;
	
	TItemTable * p = ITEM_MANAGER::instance().GetTable(dwItemVnum);

	if (!p)
		return NULL;

	DBManager::instance().SendMoneyLog(MONEY_LOG_DROP, dwItemVnum, bCount);

	if (p->dwFlags & ITEM_FLAG_STACKABLE && p->bType != ITEM_BLEND)
	{
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = GetInventoryItem(i);

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef ENABLE_STACK_LIMIT
				WORD bCount2 = MIN(ITEM_MAX_COUNT - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(200 - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
						SendPickupItemPacket(item->GetVnum());
#else
						LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item->GetLocaleName());
#endif

					return item;
				}
			}
		}

#ifdef ENABLE_SPECIAL_INVENTORY
		for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetSkillBookInventoryItem(i);

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef ENABLE_STACK_LIMIT
				WORD bCount2 = MIN(ITEM_MAX_COUNT - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(200 - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
						SendPickupItemPacket(item->GetVnum());
#else
						LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item->GetLocaleName());
#endif

					return item;
				}
			}
		}

		for (int i = UPGRADE_ITEMS_INVENTORY_SLOT_START; i < UPGRADE_ITEMS_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetUpgradeItemsInventoryItem(i);

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef ENABLE_STACK_LIMIT
				WORD bCount2 = MIN(ITEM_MAX_COUNT - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(200 - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
						SendPickupItemPacket(item->GetVnum());
#else
						LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item->GetLocaleName());
#endif

					return item;
				}
			}
		}

		for (int i = STONE_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetStoneInventoryItem(i);

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef ENABLE_STACK_LIMIT
				WORD bCount2 = MIN(ITEM_MAX_COUNT - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(200 - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
						SendPickupItemPacket(item->GetVnum());
#else
						LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item->GetLocaleName());
#endif

					return item;
				}
			}
		}

		for (int i = GIFT_BOX_INVENTORY_SLOT_START; i < GIFT_BOX_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetGiftBoxInventoryItem(i);

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef ENABLE_STACK_LIMIT
				WORD bCount2 = MIN(ITEM_MAX_COUNT - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(200 - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
						SendPickupItemPacket(item->GetVnum());
#else
						LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item->GetLocaleName());
#endif

					return item;
				}
			}
		}
		
		for (int i = CHANGERS_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
		{
			LPITEM item = GetChangersInventoryItem(i);

			if (!item)
				continue;

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket(item))
			{
				if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
						bCount = p->alValues[1];
				}

#ifdef ENABLE_STACK_LIMIT
				WORD bCount2 = MIN(ITEM_MAX_COUNT - item->GetCount(), bCount);
#else
				BYTE bCount2 = MIN(200 - item->GetCount(), bCount);
#endif
				bCount -= bCount2;

				item->SetCount(item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
						SendPickupItemPacket(item->GetVnum());
#else
						LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item->GetLocaleName());
#endif

					return item;
				}
			}
		}
#endif
	}

	LPITEM item = ITEM_MANAGER::instance().CreateItem(dwItemVnum, bCount, 0, true);

	if (!item)
	{
		sys_err("cannot create item by vnum %u (name: %s)", dwItemVnum, GetName());
		return NULL;
	}

	if (item->GetType() == ITEM_BLEND)
	{
		if (item->IsStackable())
		{
			for (int i=0; i < INVENTORY_MAX_NUM; i++)
			{
				LPITEM inv_item = GetInventoryItem(i);

				if (inv_item == NULL) continue;

				if (inv_item->GetType() == ITEM_BLEND && inv_item->GetVnum() == item->GetVnum())
				{
					const DWORD dwTotalCount = inv_item->GetCount() + item->GetCount();
					if (dwTotalCount <= ITEM_MAX_COUNT && FN_compare_item_socket(inv_item, item))
					{
						inv_item->SetCount(dwTotalCount);
						M2_DESTROY_ITEM(item);
						return inv_item;
					}
				}
			}
		}
	}

	int iEmptyCell;
	if (item->IsDragonSoul())
		iEmptyCell = GetEmptyDragonSoulInventory(item);
#ifdef ENABLE_SPECIAL_INVENTORY
	else if (item->IsSkillBook())
		iEmptyCell = GetEmptySkillBookInventory(item->GetSize());
	else if (item->IsUpgradeItem())
		iEmptyCell = GetEmptyUpgradeItemsInventory(item->GetSize());
	else if (item->IsStone())
		iEmptyCell = GetEmptyStoneInventory(item->GetSize());
	else if (item->IsGiftBox())
		iEmptyCell = GetEmptyGiftBoxInventory(item->GetSize());
	else if (item->IsChanger())
		iEmptyCell = GetEmptyChangersInventory(item->GetSize());
#endif
	else
		iEmptyCell = GetEmptyInventory(item->GetSize());

	if (iEmptyCell != -1)
	{
		if (bMsg)
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
			SendPickupItemPacket(item->GetVnum());
#else
			LocaleChatPacket(CHAT_TYPE_INFO, 495, "%s", item->GetLocaleName());
#endif

		if (item->IsDragonSoul())
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
#ifdef ENABLE_SPECIAL_INVENTORY
		else if (item->IsSkillBook())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
#endif
		else if (item->IsUpgradeItem())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
#endif
		else if (item->IsStone())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
#endif
		else if (item->IsGiftBox())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
#endif
		else if (item->IsChanger())
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
#endif
#endif
		else
#ifdef ENABLE_PICKUP_ITEM_EFFECT
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell), bHighlight);
#else
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
#endif

		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = iEmptyCell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		// Memory leak önleme: Inventory dolu ise item yere düþürülüyor
		item->AddToGround(GetMapIndex(), GetXYZ());
		item->StartDestroyEvent();

		if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP))
			item->SetOwnership(this, 300);
		else
			item->SetOwnership(this, 60);
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
		sys_log(0, "AutoGiveItem: %s inventory full, dropping item %u (count: %u) to ground", GetName(), dwItemVnum, bCount);
	}

	sys_log(0, 
		"7: %d %d", dwItemVnum, bCount);
	return item;
}

bool CHARACTER::GiveItem(LPCHARACTER victim, TItemPos Cell)
{
	if (!CanHandleItem())
		return false;

	LPITEM item = GetItem(Cell);

	if (item && !item->IsExchanging())
	{
		if (victim->CanReceiveItem(this, item))
		{
			victim->ReceiveItem(this, item);
			return true;
		}
	}

	return false;
}

bool CHARACTER::CanReceiveItem(LPCHARACTER from, LPITEM item) const
{
	if (IsPC())
		return false;

	if (DISTANCE_APPROX(GetX() - from->GetX(), GetY() - from->GetY()) > 2000)
		return false;

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH && 
					(item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
				return true;
			break;

		case fishing::FISHER_MOB:
			if (item->GetType() == ITEM_ROD)
				return true;
			break;

		case BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
			if (item->GetType() == ITEM_WEAPON && 
					item->GetRefinedVnum())
				return true;
			else
				return false;
			break;

		case BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
			if (item->GetType() == ITEM_ARMOR && (item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD
#ifdef ENABLE_PENDANT_SYSTEM
				|| item->GetSubType() == ARMOR_PENDANT
#endif
				) && item->GetRefinedVnum())
				return true;
			else
				return false;
			break;

		case BLACKSMITH_ACCESSORY_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
			if (item->GetType() == ITEM_ARMOR && !(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD
#ifdef ENABLE_PENDANT_SYSTEM
				|| item->GetSubType() == ARMOR_PENDANT
#endif
				) && item->GetRefinedVnum())
				return true;
			else
				return false;
			break;

		case BLACKSMITH_MOB:
			if (item->GetRefinedVnum() && item->GetRefineSet() < 500)
			{
				return true;
			}
			else
			{
				return false;
			}

		case BLACKSMITH2_MOB:
			if (item->GetRefineSet() >= 500)
			{
				return true;
			}
			else
			{
				return false;
			}

		case ALCHEMIST_MOB:
			if (item->GetRefinedVnum())
				return true;
			break;

		case 20101:
		case 20102:
		case 20103:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_1)
			{
				if (!IsDead())
				{
					from->LocaleChatPacket(CHAT_TYPE_INFO, 514, "");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1)
			{
				if (IsDead())
				{
					from->LocaleChatPacket(CHAT_TYPE_INFO, 515, "");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20104:
		case 20105:
		case 20106:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_2)
			{
				if (!IsDead())
				{
					from->LocaleChatPacket(CHAT_TYPE_INFO, 514, "");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				if (IsDead())
				{
					from->LocaleChatPacket(CHAT_TYPE_INFO, 515, "");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20107:
		case 20108:
		case 20109:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				if (!IsDead())
				{
					from->LocaleChatPacket(CHAT_TYPE_INFO, 514, "");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				if (IsDead())
				{
					from->LocaleChatPacket(CHAT_TYPE_INFO, 515, "");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				return false;
			}
			break;
#ifdef ENABLE_RIDING_EXTENDED
		case 20149:
		case 20150:
		case 20151:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_4)
			{
				if (!IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Monkey Herbs cannot be fed to living horses. It is used to revive dead horses."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_4)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a dead Horse."));
					return false;
				}
	
				// At Doyduðunda Beslenmesini Engelleyin
				unsigned char horselevel = from->GetHorseLevel();
				if (from->GetHorseHealth() >= c_aHorseStat[horselevel].iMaxHealth)
				{
					from->ChatPacket (CHAT_TYPE_INFO, LC_TEXT("Your horse's health has reached the maximum, you cannot feed it any more.."));
					return false;
				}
				return true;
			}
			break;
#endif
	}

	{
		return true;
	}

	return false;
}

void CHARACTER::ReceiveItem(LPCHARACTER from, LPITEM item)
{
	if (IsPC())
		return;

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH && (item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
				fishing::Grill(from, item);
			else
			{
				from->SetQuestNPCID(GetVID());
				quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
			}
			break;

		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
			if (item->GetRefinedVnum() != 0 && item->GetRefineSet() != 0 && item->GetRefineSet() < 500)
			{
				from->SetRefineNPC(this);
				from->RefineInformation(item->GetCell(), REFINE_TYPE_MONEY_ONLY);
			}
			else
			{
				from->LocaleChatPacket(CHAT_TYPE_INFO, 409, "");
			}
			break;

		case BLACKSMITH_MOB:
		case BLACKSMITH2_MOB:
		case BLACKSMITH_WEAPON_MOB:
		case BLACKSMITH_ARMOR_MOB:
		case BLACKSMITH_ACCESSORY_MOB:
			if (item->GetRefinedVnum())
			{
				from->SetRefineNPC(this);
				from->RefineInformation(item->GetCell(), REFINE_TYPE_NORMAL);
			}
			else
			{
				from->LocaleChatPacket(CHAT_TYPE_INFO, 409, "");
			}
			break;

		case 20101:
		case 20102:
		case 20103:
		case 20104:
		case 20105:
		case 20106:
		case 20107:
		case 20108:
		case 20109:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_1 || item->GetVnum() == ITEM_REVIVE_HORSE_2 || item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				from->ReviveHorse();
				item->SetCount(item->GetCount()-1);
				from->LocaleChatPacket(CHAT_TYPE_INFO, 516, "");
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_2 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				from->FeedHorse();
				from->LocaleChatPacket(CHAT_TYPE_INFO, 517, "");
				item->SetCount(item->GetCount()-1);
				EffectPacket(SE_HPUP_RED);
			}
			break;

#ifdef ENABLE_RIDING_EXTENDED
		case 20149:
		case 20150:
		case 20151:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_4)
			{
				from->ReviveHorse();
				item->SetCount(item->GetCount() - 1);
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You fed the Horse with Herbs."));
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_4)
			{
				from->FeedHorse();
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have fed the Horse."));
				item->SetCount(item->GetCount() - 1);
				EffectPacket(SE_HPUP_RED);
			}
			break;
#endif

		default:
			sys_log(0, "TakeItem %s %d %s", from->GetName(), GetRaceNum(), item->GetName());
			from->SetQuestNPCID(GetVID());
			quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
			break;
	}
}

bool CHARACTER::IsEquipUniqueItem(DWORD dwItemVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}

#ifdef ENABLE_MOUNT_SYSTEM
	{
		LPITEM u = GetWear(WEAR_MOUNT);

		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}
#endif

	if (dwItemVnum == UNIQUE_ITEM_RING_OF_LANGUAGE)
		return IsEquipUniqueItem(UNIQUE_ITEM_RING_OF_LANGUAGE_SAMPLE);

	return false;
}

// CHECK_UNIQUE_GROUP
bool CHARACTER::IsEquipUniqueGroup(DWORD dwGroupVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
			return true;
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
			return true;
	}

#ifdef ENABLE_MOUNT_SYSTEM
	{
		LPITEM u = GetWear(WEAR_MOUNT);

		if (u && u->GetSpecialGroup() == (int)dwGroupVnum)
			return true;
	}
#endif

	return false;
}

void CHARACTER::SetRefineMode(int iAdditionalCell)
{
	m_iRefineAdditionalCell = iAdditionalCell;
	m_bUnderRefine = true;
}

void CHARACTER::ClearRefineMode()
{
	m_bUnderRefine = false;
	SetRefineNPC( NULL );
}

bool CHARACTER::GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector<DWORD> &dwItemVnums, 
											std::vector<DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count)
{
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwGroupNum);

	if (!pGroup)
	{
		sys_err("cannot find special item group %d", dwGroupNum);
		return false;
	}

	std::vector <int> idxes;
	int n = pGroup->GetMultiIndex(idxes);

	bool bSuccess;

	for (int i = 0; i < n; i++)
	{
		bSuccess = false;
		int idx = idxes[i];
		DWORD dwVnum = pGroup->GetVnum(idx);
		DWORD dwCount = pGroup->GetCount(idx);
		int	iRarePct = pGroup->GetRarePct(idx);
		LPITEM item_get = NULL;
		switch (dwVnum)
		{
			case CSpecialItemGroup::GOLD:
				PointChange(POINT_GOLD, dwCount);
				LogManager::instance().CharLog(this, dwCount, "TREASURE_GOLD", "");

				bSuccess = true;
				break;
			case CSpecialItemGroup::EXP:
				{
					PointChange(POINT_EXP, dwCount);
					LogManager::instance().CharLog(this, dwCount, "TREASURE_EXP", "");

					bSuccess = true;
				}
				break;

			case CSpecialItemGroup::MOB:
				{
					sys_log(0, "CSpecialItemGroup::MOB %d", dwCount);
					int x = GetX() + number(-500, 500);
					int y = GetY() + number(-500, 500);

					LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMob(dwCount, GetMapIndex(), x, y, 0, true, -1);
					if (ch)
						ch->SetAggressive();
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::SLOW:
				{
					sys_log(0, "CSpecialItemGroup::SLOW %d", -(int)dwCount);
					AddAffect(AFFECT_SLOW, POINT_MOV_SPEED, -(int)dwCount, AFF_SLOW, 300, 0, true);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::DRAIN_HP:
				{
					int iDropHP = GetMaxHP()*dwCount/100;
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					iDropHP = MIN(iDropHP, GetHP()-1);
					sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
					PointChange(POINT_HP, -iDropHP);
					bSuccess = true;
				}
				break;
			case CSpecialItemGroup::POISON:
				{
					AttackedByPoison(NULL);
					bSuccess = true;
				}
				break;

			case CSpecialItemGroup::MOB_GROUP:
				{
					int sx = GetX() - number(300, 500);
					int sy = GetY() - number(300, 500);
					int ex = GetX() + number(300, 500);
					int ey = GetY() + number(300, 500);
					CHARACTER_MANAGER::instance().SpawnGroup(dwCount, GetMapIndex(), sx, sy, ex, ey, NULL, true);

					bSuccess = true;
				}
				break;
			default:
				{
#ifdef ENABLE_PICKUP_ITEM_EFFECT
					item_get = AutoGiveItem(dwVnum, dwCount, iRarePct, true, true);
#else
					item_get = AutoGiveItem(dwVnum, dwCount, iRarePct);
#endif

					if (item_get)
					{
						bSuccess = true;
					}
				}
				break;
		}
	
		if (bSuccess)
		{
			dwItemVnums.push_back(dwVnum);
			dwItemCounts.push_back(dwCount);
			item_gets.push_back(item_get);
			count++;

		}
		else
		{
			return false;
		}
	}
	return bSuccess;
}

bool CHARACTER::ItemProcess_Hair(LPITEM item, int iDestCell)
{
	if (item->CheckItemUseLevel(GetLevel()) == false)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 529, "");
		return false;
	}

	DWORD hair = item->GetVnum();

	switch (GetJob())
	{
		case JOB_WARRIOR :
			hair -= 72000;
			break;

		case JOB_ASSASSIN :
			hair -= 71250;
			break;

		case JOB_SURA :
			hair -= 70500;
			break;

		case JOB_SHAMAN :
			hair -= 69750;
			break;

		default :
			return false;
			break;
	}

	if (hair == GetPart(PART_HAIR))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 530, "");
		return true;
	}

	item->SetCount(item->GetCount() - 1);

	SetPart(PART_HAIR, hair);
	UpdatePacket();

	return true;
}

bool CHARACTER::ItemProcess_Polymorph(LPITEM item)
{
	if (IsPolymorphed())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 531, "");
		return false;
	}

	if (true == IsRiding())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	DWORD dwVnum = item->GetSocket(0);

	if (dwVnum == 0)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 532, "");
		item->SetCount(item->GetCount()-1);
		return false;
	}

	const CMob* pMob = CMobManager::instance().Get(dwVnum);

	if (pMob == NULL)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 532, "");
		item->SetCount(item->GetCount()-1);
		return false;
	}

	switch (item->GetVnum())
	{
		case 70104 :
		case 70105 :
		case 70106 :
		case 70107 :
		case 71093 :
			{
				sys_log(0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", GetPlayerID(), dwVnum);

				int iPolymorphLevelLimit = MAX(0, 20 - GetLevel() * 3 / 10);
				if (pMob->m_table.bLevel >= GetLevel() + iPolymorphLevelLimit)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 533, "");
					return false;
				}

				int iDuration = GetSkillLevel(POLYMORPH_SKILL_ID) == 0 ? 5 : (5 + (5 + GetSkillLevel(POLYMORPH_SKILL_ID)/40 * 25));
				iDuration *= 60;

				DWORD dwBonus = (2 + GetSkillLevel(POLYMORPH_SKILL_ID)/40) * 100;

#ifdef ENABLE_TRANSFORMATION_SKILL_CLOSE_FIX
				if (IsAffectFlag(AFF_GEOMGYEONG))
					RemoveAffect(SKILL_GEOMKYUNG);

				if (IsAffectFlag(AFF_GWIGUM))
					RemoveAffect(SKILL_GWIGEOM);
#endif

				AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
				AddAffect(AFFECT_POLYMORPH, POINT_ATT_BONUS, dwBonus, AFF_POLYMORPH, iDuration, 0, false);
				
				item->SetCount(item->GetCount()-1);
			}
			break;

		case 50322:
			{
				sys_log(0, "USE_POLYMORPH_BOOK: %s(%u) vnum(%u)", GetName(), GetPlayerID(), dwVnum);

				if (CPolymorphUtils::instance().PolymorphCharacter(this, item, pMob) == true)
				{
					CPolymorphUtils::instance().UpdateBookPracticeGrade(this, item);
				}
			}
			break;

		default :
			sys_err("POLYMORPH invalid item passed PID(%d) vnum(%d)", GetPlayerID(), item->GetOriginalVnum());
			return false;
	}

	return true;
}

bool CHARACTER::CanDoCube() const
{
	if (m_bIsObserver)				return false;
	if (GetShop())					return false;
	if (GetMyShop())				return false;
	if (m_bUnderRefine)				return false;
	if (IsWarping())				return false;
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	if (IsAuraRefineWindowOpen())	return false;
#endif

	return true;
}

bool CHARACTER::UnEquipSpecialRideUniqueItem()
{
	LPITEM Unique1 = GetWear(WEAR_UNIQUE1);
	LPITEM Unique2 = GetWear(WEAR_UNIQUE2);
#ifdef ENABLE_MOUNT_SYSTEM
	LPITEM Unique3 = GetWear(WEAR_MOUNT);
#endif

	if( NULL != Unique1 )
	{
		if( UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup() )
		{
			return UnequipItem(Unique1);
		}
	}

	if( NULL != Unique2 )
	{
		if( UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup() )
		{
			return UnequipItem(Unique2);
		}
	}

#ifdef ENABLE_MOUNT_SYSTEM
	if (NULL != Unique3)
	{
		if (UNIQUE_GROUP_SPECIAL_RIDE == Unique3->GetSpecialGroup())
		{
			return UnequipItem(Unique3);
		}
	}
#endif

	return true;
}

void CHARACTER::AutoRecoveryItemProcess(const EAffectTypes type)
{
	if (true == IsDead() || true == IsStun())
		return;

	if (false == IsPC())
		return;

	if (AFFECT_AUTO_HP_RECOVERY != type && AFFECT_AUTO_SP_RECOVERY != type)
		return;

	if (NULL != FindAffect(AFFECT_STUN))
		return;

	{
		const DWORD stunSkills[] = { SKILL_TANHWAN, SKILL_GEOMPUNG, SKILL_BYEURAK, SKILL_GIGUNG };

		for (size_t i = 0; i < sizeof(stunSkills) / sizeof(DWORD); ++i)
		{
			const CAffect* p = FindAffect(stunSkills[i]);

			if (NULL != p && AFF_STUN == p->dwFlag)
				return;
		}
	}

	const CAffect* pAffect = FindAffect(type);
	const size_t idx_of_amount_of_used = 1;
	const size_t idx_of_amount_of_full = 2;

	if (NULL != pAffect)
	{
		LPITEM pItem = FindItemByID(pAffect->dwFlag);

		if (NULL != pItem && true == pItem->GetSocket(0))
		{
			if (false == CArenaManager::instance().IsArenaMap(GetMapIndex()))
			{
				const long amount_of_used = pItem->GetSocket(idx_of_amount_of_used);
				const long amount_of_full = pItem->GetSocket(idx_of_amount_of_full);

				const int32_t avail = amount_of_full - amount_of_used;

				int32_t amount = 0;

				if (AFFECT_AUTO_HP_RECOVERY == type)
				{
					amount = GetMaxHP() - (GetHP() + GetPoint(POINT_HP_RECOVERY));
				}
				else if (AFFECT_AUTO_SP_RECOVERY == type)
				{
					amount = GetMaxSP() - (GetSP() + GetPoint(POINT_SP_RECOVERY));
				}

				if (amount > 0)
				{
					if (avail > amount)
					{
						const int pct_of_used = amount_of_used * 100 / amount_of_full;
						const int pct_of_will_used = (amount_of_used + amount) * 100 / amount_of_full;

						bool bLog = false;

						if ((pct_of_will_used / 10) - (pct_of_used / 10) >= 1)
							bLog = true;
						pItem->SetSocket(idx_of_amount_of_used, amount_of_used + amount, bLog);
					}
					else
					{
						amount = avail;

						ITEM_MANAGER::instance().RemoveItem( pItem );
					}

					if (AFFECT_AUTO_HP_RECOVERY == type)
					{
						PointChange( POINT_HP_RECOVERY, amount );
						EffectPacket( SE_AUTO_HPUP );
					}
					else if (AFFECT_AUTO_SP_RECOVERY == type)
					{
						PointChange( POINT_SP_RECOVERY, amount );
						EffectPacket( SE_AUTO_SPUP );
					}
				}
			}
			else
			{
				pItem->Lock(false);
				pItem->SetSocket(0, false);
				RemoveAffect( const_cast<CAffect*>(pAffect) );
			}
		}
		else
		{
			RemoveAffect( const_cast<CAffect*>(pAffect) );
		}
	}
}

bool CHARACTER::IsValidItemPosition(TItemPos Pos) const
{
	BYTE window_type = Pos.window_type;
	WORD cell = Pos.cell;

	switch (window_type)
	{
	case RESERVED_WINDOW:
		return false;

	case INVENTORY:
	case EQUIPMENT:
		return cell < (INVENTORY_AND_EQUIP_SLOT_MAX);

	case DRAGON_SOUL_INVENTORY:
		return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);

	case SAFEBOX:
		if (NULL != m_pkSafebox)
			return m_pkSafebox->IsValidPosition(cell);
		else
			return false;

	case MALL:
		if (NULL != m_pkMall)
			return m_pkMall->IsValidPosition(cell);
		else
			return false;

#ifdef ENABLE_RENEWAL_SWITCHBOT
	case SWITCHBOT:
		return cell < SWITCHBOT_SLOT_COUNT;
#endif

	default:
		return false;
	}
}

bool CHARACTER::CanEquipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell)
{
	const TItemTable* itemTable = item->GetProto();
	BYTE itemType = item->GetType();
	BYTE itemSubType = item->GetSubType();

	switch (GetJob())
	{
		case JOB_WARRIOR:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
				return false;
			break;

		case JOB_ASSASSIN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
				return false;
			break;

		case JOB_SHAMAN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
				return false;
			break;

		case JOB_SURA:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
				return false;
			break;
	}

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limit = itemTable->aLimits[i].lValue;
		switch (itemTable->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limit)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 518, "");
					return false;
				}
				break;

			case LIMIT_STR:
				if (GetPoint(POINT_ST) < limit)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 519, "");
					return false;
				}
				break;

			case LIMIT_INT:
				if (GetPoint(POINT_IQ) < limit)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 520, "");
					return false;
				}
				break;

			case LIMIT_DEX:
				if (GetPoint(POINT_DX) < limit)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 521, "");
					return false;
				}
				break;

			case LIMIT_CON:
				if (GetPoint(POINT_HT) < limit)
				{
					LocaleChatPacket(CHAT_TYPE_INFO, 522, "");
					return false;
				}
				break;
		}
	}

	if (item->GetWearFlag() & WEARABLE_UNIQUE)
	{
		if
		(
			(GetWear(WEAR_UNIQUE1) && GetWear(WEAR_UNIQUE1)->IsSameSpecialGroup(item)) || (GetWear(WEAR_UNIQUE2) && GetWear(WEAR_UNIQUE2)->IsSameSpecialGroup(item))
#ifdef ENABLE_MOUNT_SYSTEM
			|| (GetWear(WEAR_MOUNT) && GetWear(WEAR_MOUNT)->IsSameSpecialGroup(item))
#endif
		)

		{
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
			return false;
		}

		if (marriage::CManager::instance().IsMarriageUniqueItem(item->GetVnum()) && !marriage::CManager::instance().IsMarried(GetPlayerID()))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 497, "");
			return false;
		}
	}

#ifdef ENABLE_WEDDING_COSTUME_EQUIP_FIX
	if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_BODY)
	{
		LPITEM atakanxd = GetWear(WEAR_BODY);
		if (atakanxd && (atakanxd->GetVnum() >= 11901 && atakanxd->GetVnum() <= 11914))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Binek üzerinde gelinlik veya smokin varken kostüm giyemezsin."));
			return false;
		}
	}

	if (item->GetVnum() >= 11901 && item->GetVnum() <= 11914)
	{
		LPITEM atakan = GetWear(WEAR_COSTUME_BODY);
		if (atakan && (atakan->GetType() == ITEM_COSTUME && atakan->GetSubType() == COSTUME_BODY))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Binek üzerinde gelinlik veya smokin varken kostüm giyemezsin."));
			return false;
		}
	}
#endif

#ifdef ENABLE_MOUNT_PET_SKIN
	if (item->IsCostumePetSkin() && !CanChangeCostumePetSkin())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 496, "");
		return false;
	}

	if (item->IsCostumeMountSkin() && (FindAffect(AFFECT_MOUNT)))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (item->IsCostumeMountSkin() && !CanChangeCostumeMountSkin())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 496, "");
		return false;
	}
#endif

	if (item->GetType() == ITEM_RING) // Fix
	{
		LPITEM ring[2] = { GetWear(WEAR_RING1), GetWear(WEAR_RING2) };
		for (int i = 0; i < 2; i++)
		{
			if (ring[i])
			{
				if (ring[i]->GetVnum() == item->GetVnum())
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool CHARACTER::CanUnequipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell)
{
#ifdef DEFAULT_BELT_SYSTEM
	if (ITEM_ARMOR == item->GetType() && ARMOR_BELT == item->GetSubType())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
		return false;
	}
#endif

	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	if (IsAuraRefineWindowOpen())
		return false;
#endif

	{
		int pos = -1;

		if (item->IsDragonSoul())
			pos = GetEmptyDragonSoulInventory(item);
#ifdef ENABLE_SPECIAL_INVENTORY
		else if (item->IsSkillBook())
			pos = GetEmptySkillBookInventory(item->GetSize());
		else if (item->IsUpgradeItem())
			pos = GetEmptyUpgradeItemsInventory(item->GetSize());
		else if (item->IsStone())
			pos = GetEmptyStoneInventory(item->GetSize());
		else if (item->IsGiftBox())
			pos = GetEmptyGiftBoxInventory(item->GetSize());
		else if (item->IsChanger())
			pos = GetEmptyChangersInventory(item->GetSize());
#endif
		else
			pos = GetEmptyInventory(item->GetSize());

		if (pos == -1)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
			return false;
		}
	}

#ifdef ENABLE_MOUNT_PET_SKIN
	if (item->IsCostumePetSkin() && !CanChangeCostumePetSkin())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 496, "");
		return false;
	}

	if (item->IsCostumeMountSkin() && (FindAffect(AFFECT_MOUNT)))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return false;
	}

	if (item->IsCostumeMountSkin() && !CanChangeCostumeMountSkin())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 496, "");
		return false;
	}
#endif

	return true;
}

#ifdef ENABLE_PENDANT_SYSTEM
struct SPendantBonus
{
	SPendantBonus(BYTE recvRefineLevel, BYTE recvActualBonus) : refineLevel(recvRefineLevel), actualBonus(recvActualBonus) {}
	BYTE refineLevel, actualBonus;

	bool checkPendantBonus()
	{
		switch (actualBonus)
		{
			case 0:
				return true;

			case 1:
				if (refineLevel >= 5)
					return true;
				break;

			case 2:
				if (refineLevel >= 9)
					return true;
				break;

			case 3:
				if (refineLevel >= 13)
					return true;
				break;

			case 4:
				if (refineLevel >= 17)
					return true;
				break;

			case 5:
				return false;
		}
		return false;
	}
};

bool CHARACTER::CheckCanAddPendantBonus(LPITEM pkItem)
{
	if (!pkItem || (pkItem && !pkItem->IsPendant())) return false;

	const BYTE pendantBonus = pkItem->GetAttributeCount();
	const BYTE pendantRefineLevel = pkItem->GetRefineLevel();
	SPendantBonus s_PendantBonus(pendantRefineLevel, pendantBonus);

	return s_PendantBonus.checkPendantBonus();
}
#endif

#ifdef ENABLE_RENEWAL_AFFECT
int CHARACTER::GetAffectType(LPITEM item)
{
	switch (item->GetValue(2))
	{
		case 1: return AFFECT_MALL_PLUS;
		default: return AFFECT_BLEND_PLUS;
	}
}
#endif

#ifdef ENABLE_STYLE_ATTRIBUTE_SYSTEM
bool CHARACTER::UseItemNewAttribute(TItemPos source_pos, TItemPos target_pos, BYTE* bValues)
{
	LPITEM item;
	LPITEM item_target;

	for (int i = 0; i < 5; ++i)
	{
		if (bValues[i] < 0 || bValues[i] > 255)
			return false;
	}

	if (!CanHandleItem())
		return false;

	if (!IsValidItemPosition(source_pos) || !(item = GetItem(source_pos)))
		return false;

	if (!IsValidItemPosition(target_pos) || !(item_target = GetItem(target_pos)))
		return false;

	if (item->GetVnum() != 49999)
		return false;

	if (item->IsExchanging())
		return false;

	if (item_target->IsExchanging())
		return false;

	if (item_target->IsEquipped())
		return false;

	if (ITEM_COSTUME == item_target->GetType())
		return false;

	if (item_target->GetVnum() == 50201 || item_target->GetVnum() == 50202 || item_target->GetVnum() == 11901 || item_target->GetVnum() == 11902 || item_target->GetVnum() == 11903 || item_target->GetVnum() == 11904 || item_target->GetVnum() == 11911 || item_target->GetVnum() == 11912 || item_target->GetVnum() == 11913 || item_target->GetVnum() == 11914)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't apply this attribute on this item."));
		return false;
	}
	int processNewAttr = item_target->AddNewStyleAttribute(bValues);

	if (processNewAttr == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't apply this attribute on this item."));
		return false;
	}
	else if (processNewAttr == 2)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't apply an attribute twice."));
		return false;
	}
	else if (processNewAttr == 3)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Attributes has been added successfully!"));
		item->SetCount(item->GetCount() - 1);
		return true;
	}

	//sys_log(0, "%s: UseItemNewAttribute %s (inven %d, cell: %d)", GetName(), item->GetName(), window_type, wCell);

	return false;
}
#endif
