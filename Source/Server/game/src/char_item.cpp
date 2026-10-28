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
#ifdef ENABLE_BUFFI_SYSTEM
#include "buffi_system.h"
#endif
#endif

#ifdef ENABLE_QUEEN_NETHIS
	#include "SnakeLair.h"
#endif

const int ITEM_BROKEN_METIN_VNUM = 28960;

// CHANGE_ITEM_ATTRIBUTES
const DWORD CHARACTER::msc_dwDefaultChangeItemAttrCycle = 10;
const char CHARACTER::msc_szLastChangeItemAttrFlag[] = "Item.LastChangeItemAttr";
const char CHARACTER::msc_szChangeItemAttrCycleFlag[] = "change_itemattr_cycle";
// END_OF_CHANGE_ITEM_ATTRIBUTES
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

#ifdef ENABLE_SUNG_MAHI_TOWER
bool IS_SUNG_MAHI_ENABLE_ITEM(int iMapIndex, DWORD dwVnum)
{
	bool isDungeonMap = (iMapIndex >= 3540000 && iMapIndex < 3550000);
	if (isDungeonMap && !(dwVnum >= 79707 && dwVnum <= 79712))
		return false;

	if ((dwVnum >= 79707 && dwVnum <= 79712) && !isDungeonMap)
		return false;

	return true;
}
#endif

static bool FN_check_item_socket(LPITEM item)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item->GetSocket(i) != item->GetProto()->alSockets[i])
			return false;
	}

	return true;
}

static void FN_copy_item_socket(LPITEM dest, LPITEM src)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		dest->SetSocket(i, src->GetSocket(i));
	}
}

static bool FN_check_item_sex(LPCHARACTER ch, LPITEM item)
{
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE))
	{
		if (SEX_MALE == GET_SEX(ch))
			return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE))
	{
		if (SEX_FEMALE == GET_SEX(ch))
			return false;
	}

	return true;
}

static bool FN_compare_item_socket(const LPITEM pkItemSrc, const LPITEM pkItemDest)
{
	if (!pkItemSrc || !pkItemDest)
		return false;

	return memcmp(pkItemSrc->GetSockets(), pkItemDest->GetSockets(), sizeof(int32_t) * ITEM_SOCKET_MAX_NUM) == 0;
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

#ifdef ENABLE_ITEM_DUPE_FIX
	if ((thecore_pulse() - iWarpTime) < PASSES_PER_SEC(10))
	{
		return false;
	}

	itemPushTime = thecore_pulse();

	if ((thecore_pulse() - g_nPortalLimitTime) < PASSES_PER_SEC(10))
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
	if (!IsValidItemPosition(Cell) || !m_pItemStorage)
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
		return ItemStorage().pItems[wCell];

	case DRAGON_SOUL_INVENTORY:
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid DS item cell %d", wCell);
			return NULL;
		}
		return ItemStorage().pDSItems[wCell];

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

			LPITEM pOld = ItemStorage().pItems[wCell];

			if (pOld)
			{
				if (wCell < INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
							continue;

						if (ItemStorage().pItems[p] && ItemStorage().pItems[p] != pOld)
							continue;

						ItemStorage().bItemGrid[p] = 0;
					}
				}
				else
					ItemStorage().bItemGrid[wCell] = 0;
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

						ItemStorage().bItemGrid[p] = wCell + 1;
					}
				}
				else
					ItemStorage().bItemGrid[wCell] = wCell + 1;
			}

			ItemStorage().pItems[wCell] = pItem;
		}
		break;

	case DRAGON_SOUL_INVENTORY:
		{
			LPITEM pOld = ItemStorage().pDSItems[wCell];

			if (pOld)
			{
				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
							continue;

						if (ItemStorage().pDSItems[p] && ItemStorage().pDSItems[p] != pOld)
							continue;

						ItemStorage().wDSItemGrid[p] = 0;
					}
				}
				else
					ItemStorage().wDSItemGrid[wCell] = 0;
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

						ItemStorage().wDSItemGrid[p] = wCell + 1;
					}
				}
				else
					ItemStorage().wDSItemGrid[wCell] = wCell + 1;
			}

			ItemStorage().pDSItems[wCell] = pItem;
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
#ifdef ENABLE_SET_ITEM
			pack.dwSetValue = pItem->GetSetValue();
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
			pack.sealbind = pItem->GetSealDate();
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
#ifdef ENABLE_SET_ITEM
			pack.dwSetValue = 0;
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
			pack.sealbind = 0;
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

	return m_pItemStorage ? m_pItemStorage->pItems[INVENTORY_MAX_NUM + bCell] : NULL;
}

void CHARACTER::SetWear(UINT bCell, LPITEM item)
{
	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::SetItem: invalid item cell %d", bCell);
		return;
	}

	SetItem(TItemPos (INVENTORY, INVENTORY_MAX_NUM + bCell), item);

#ifdef ENABLE_TELEPORT_SKILL
	/*if (!item && bCell == WEAR_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}*/
#else
	if (!item && bCell == WEAR_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}
#endif
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
#else
					// reference: "A belt is required for this inventory."
					if (NULL == GetWear(WEAR_BELT))
						return false;
#endif

					if (ItemStorage().bItemGrid[bCell])
					{
						if (ItemStorage().bItemGrid[bCell] == iExceptionCell)
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

					if (ItemStorage().bItemGrid[bCell])
					{
						if (ItemStorage().bItemGrid[bCell] == iExceptionCell)
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

								if (ItemStorage().bItemGrid[p])
								{
									if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

							if (ItemStorage().bItemGrid[p])
							{
								if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

					if (ItemStorage().bItemGrid[bCell])
					{
						if (ItemStorage().bItemGrid[bCell] == iExceptionCell)
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

								if (ItemStorage().bItemGrid[p])
								{
									if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

							if (ItemStorage().bItemGrid[p])
							{
								if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

					if (ItemStorage().bItemGrid[bCell])
					{
						if (ItemStorage().bItemGrid[bCell] == iExceptionCell)
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

								if (ItemStorage().bItemGrid[p])
								{
									if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

							if (ItemStorage().bItemGrid[p])
							{
								if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

					if (ItemStorage().bItemGrid[bCell])
					{
						if (ItemStorage().bItemGrid[bCell] == iExceptionCell)
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

								if (ItemStorage().bItemGrid[p])
								{
									if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

							if (ItemStorage().bItemGrid[p])
							{
								if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

					if (ItemStorage().bItemGrid[bCell])
					{
						if (ItemStorage().bItemGrid[bCell] == iExceptionCell)
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

								if (ItemStorage().bItemGrid[p])
								{
									if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

							if (ItemStorage().bItemGrid[p])
							{
								if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

				if (ItemStorage().bItemGrid[bCell])
				{
					if (ItemStorage().bItemGrid[bCell] == iExceptionCell)
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

							if (ItemStorage().bItemGrid[p])
								if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

						if (ItemStorage().bItemGrid[p])
							if (ItemStorage().bItemGrid[p] != iExceptionCell)
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

				if (ItemStorage().wDSItemGrid[wCell])
				{
					if (ItemStorage().wDSItemGrid[wCell] == iExceptionCell)
					{
						if (bSize == 1)
							return true;

						int j = 1;

						do
						{
							BYTE p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

							if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
								return false;

							if (ItemStorage().wDSItemGrid[p])
								if (ItemStorage().wDSItemGrid[p] != iExceptionCell)
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

						if (ItemStorage().bItemGrid[p])
							if (ItemStorage().wDSItemGrid[p] != iExceptionCell)
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

	std::copy(ItemStorage().wDSItemGrid, ItemStorage().wDSItemGrid + DRAGON_SOUL_INVENTORY_MAX_NUM, vDragonSoulItemGrid.begin());
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
	// ACCESSORY_REFINE
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
		}
	}
	// END_OF_ACCESSORY_REFINE
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

bool CHARACTER::DoRefine(LPITEM item, bool bMoneyOnly/* = false*/
#ifdef ENABLE_PITTY_REFINE
, bool bUseSealOfGod/* = false*/
#endif
)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

#ifdef ENABLE_STONES_STACKFIX
	DWORD pos = GetEmptyInventory(item->GetSize());

	if (-1 == pos)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Çok fazla eþya taþýyorsun."));
		return false;
	}
#endif

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
		LocaleChatPacket(CHAT_TYPE_INFO, 1464, "");
		return false;
	}
#endif

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
		return false;

	DWORD result_vnum = item->GetRefinedVnum();

	// REFINE_COST
	int cost = ComputeRefineFee(prt->cost);

	int RefineChance = GetQuestFlag("main_quest_lv7.refine_chance");

	if (RefineChance > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The free weapon improvements can only be used on weapons up to level 20."));
			return false;
		}

		cost = 0;
		SetQuestFlag("main_quest_lv7.refine_chance", RefineChance - 1);
	}
	// END_OF_REFINE_COST

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No further improvements possible."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
		return false;

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefine NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		return false;
	}

	// REFINE_COST
	if (GetGold() < cost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang for an upgrade."));
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

				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not the right material for an upgrade."));
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

#ifdef ENABLE_PITTY_REFINE
	if (bUseSealOfGod && CountSpecifyItem(ITEM_VNUM_SEAL_OF_GOD) > 0)
	{
		RemoveSpecifyItem(ITEM_VNUM_SEAL_OF_GOD, 1);
		prob -= EXTRA_CHANCE_SEAL_OF_GOD;
		if (prob <= 0)
		{
			prob = 1;
		}
	}
#endif


	// END_OF_REFINE_COST

	if (prob <= prt->prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
#ifdef ENABLE_BATTLE_PASS
			if (!v_counts.empty())
			{
				for (int i = 0; i < missions_bp.size(); ++i)
				{
					if (missions_bp[i].type == 6)
					{
						DoMission(i, 1);
					}
				}
			}

			if (!v_counts_premium.empty())
			{
				for (int i = 0; i < missions_bp_premium.size(); ++i)
				{
					if (missions_bp_premium[i].type == 6)
					{
						DoMissionPremium(i, 1);
					}
				}
			}
#endif
#ifdef ENABLE_RANKING
			SetRankPoints(10, GetRankPoints(10) + 1);
#endif
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			UINT bCell = item->GetCell();

			// DETAIL_REFINE_LOG
			NotifyRefineSuccess(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);

#ifdef ENABLE_STONES_STACKFIX
			DWORD dwStonesVnum = item->GetVnum();

			if (dwStonesVnum >= 28000 && dwStonesVnum <= 29000)
			{
				item->SetCount(item->GetCount() - 1);
				pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, pos));
				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			}
			else
			{
				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
#ifdef ENABLE_ITEM_UPGRADE_NOTICE
				if (pkNewItem->GetRefineLevel() >= ITEM_UPGRADE_NOTICE_MIN_LEVEL) 
				{
					char gelistirmeduyuru[QUERY_MAX_LEN];
					snprintf(gelistirmeduyuru, sizeof(gelistirmeduyuru), "|cff00FF7F[%s]|r isimli oyuncu |cffffcc00[%s]|r elde etti.", GetName(), pkNewItem->GetName()); 
					BroadcastNotice(gelistirmeduyuru);
				}
#endif
				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
				pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			}
#else
			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
#ifdef ENABLE_ITEM_UPGRADE_NOTICE
			if (pkNewItem->GetRefineLevel() >= ITEM_UPGRADE_NOTICE_MIN_LEVEL) 
			{
				char gelistirmeduyuru[QUERY_MAX_LEN];
				snprintf(gelistirmeduyuru, sizeof(gelistirmeduyuru), "|cff00FF7F[%s]|r isimli oyuncu |cffffcc00[%s]|r elde etti.", GetName(), pkNewItem->GetName()); 
				BroadcastNotice(gelistirmeduyuru);
			}
#endif
#endif

			sys_log(0, "Refine Success %d", cost);
			pkNewItem->AttrLog();
			sys_log(0, "PayPee %d", cost);
			PayRefineFee(cost);
			sys_log(0, "PayPee End %d", cost);
		}
		else
		{
			// DETAIL_REFINE_LOG
			sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			// END_OF_DETAIL_REFINE_LOG
		}
	}
	else
	{
		DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
		NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
		item->AttrLog();
#ifdef ENABLE_STONES_STACKFIX
		item->SetCount(item->GetCount() - 1);
#else
		ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");
#endif

		PayRefineFee(cost);
	}

	return true;
}

enum enum_RefineScrolls
{
	CHUKBOK_SCROLL			= 0,	// 0%	Segenssschriftrolle [25040, 72301, 76016]
	HYUNIRON_CHN			= 1,	// 0%	Magischer Stein [39001, 25041]
	YONGSIN_SCROLL			= 2,	// 10%	Schriftrolle des Drachen	[39022, 71032, 72314, 76009]
	MUSIN_SCROLL			= 3,	// 100%	Schriftrolle des Krieges [39014, 71021]									-> Nur bis +4
	YAGONG_SCROLL			= 4,	// 20%	Schmiede-Handbuch [39007, 70039, 72309]
	MEMO_SCROLL				= 5,	// 100%	Notiz des Schmieds, Notiz des Weisen [-[70027, 72306], [70028, 72307]-]	-> Nur +4 Items
	BDRAGON_SCROLL			= 6,	// 
};

bool CHARACTER::DoRefineWithScroll(LPITEM item
#ifdef ENABLE_PITTY_REFINE
, bool bUseSealOfGod/* = false*/
#endif
)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

#ifdef ENABLE_STONES_STACKFIX
	DWORD pos = GetEmptyInventory(item->GetSize());

	if (-1 == pos) {
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Çok fazla eþya taþýyorsun."));
		return false;
	}
#endif

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
		LocaleChatPacket(CHAT_TYPE_INFO, 1464, "");
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
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No further improvements possible."));
		return false;
	}

	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		if (item->GetRefineLevel() >= 4)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Scroll of War does not work on items with +4 or above."));
			return false;
		}
	}
	else if (pkItemScroll->GetValue(0) == MEMO_SCROLL)
	{
		if (item->GetRefineLevel() != pkItemScroll->GetValue(1))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot upgrade items with this Scroll."));
			return false;
		}
	}
	else if (pkItemScroll->GetValue(0) == BDRAGON_SCROLL)
	{
		if (item->GetType() != ITEM_METIN || item->GetRefineLevel() != 4)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot upgrade this item."));
			return false;
		}
	}

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefineWithScroll NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		return false;
	}

	if (GetGold() < prt->cost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang for an upgrade."));
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

			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not the right material for an upgrade."));
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

		if (pkItemScroll->GetValue(0) == HYUNIRON_CHN)
			bDestroyWhenFail = true;


		// DETAIL_REFINE_LOG
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
		// END_OF_DETAIL_REFINE_LOG
	}
	// DETAIL_REFINE_LOG
	if (pkItemScroll->GetValue(0) == MUSIN_SCROLL)
	{
		success_prob = 100;

		szRefineType = "MUSIN_SCROLL";
	}
	// END_OF_DETAIL_REFINE_LOG
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

	// fix pergaminos
	if (pkItemScroll->GetVnum() == 39022 || pkItemScroll->GetVnum() == 71032 || pkItemScroll->GetVnum() == 76009)
		success_prob = prt->prob + 10;

	if (test_server)
	{
		ChatPacket(CHAT_TYPE_INFO, "[Only Test] Success_Prob %d, RefineLevel %d ", success_prob, item->GetRefineLevel());
	}

	pkItemScroll->SetCount(pkItemScroll->GetCount() - 1);

#ifdef ENABLE_PITTY_REFINE
	if (bUseSealOfGod && CountSpecifyItem(ITEM_VNUM_SEAL_OF_GOD) > 0)
	{
		RemoveSpecifyItem(ITEM_VNUM_SEAL_OF_GOD, 1);

		success_prob = success_prob + EXTRA_CHANCE_SEAL_OF_GOD;
		if (success_prob > 100)
		{
			success_prob = 100;
		}
	}

	int iMaxPittyLevel = item->GetLimitPittyRefine();
	int iGetPittyRefineLevel = GetPittyRefineLevel(item->GetVnum(), item->GetRefineLevel());

	if (iMaxPittyLevel != 0)
	{
		// If you are pitty level, then make 100% chance of success...
		if (iMaxPittyLevel == iGetPittyRefineLevel)
			prob = success_prob;

		if (prob <= success_prob) // succesed
			SetPittyRefineLevel(item->GetVnum(), item->GetRefineLevel(), 0);
		else if (!bDestroyWhenFail && result_fail_vnum) // should downgrade item-1
			SetPittyRefineLevel(item->GetVnum(), item->GetRefineLevel(), 0);
		else
		{
			if (iGetPittyRefineLevel < iMaxPittyLevel)
				SetPittyRefineLevel(item->GetVnum(), item->GetRefineLevel(), iGetPittyRefineLevel + 1);
		}

		SendPittyInfoClient(item->GetVnum(), item->GetRefineLevel());
	}
#endif

	if (prob <= success_prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
#ifdef ENABLE_BATTLE_PASS
			if (!v_counts.empty())
			{
				for (int i = 0; i < missions_bp.size(); ++i)
				{
					if (missions_bp[i].type == 6)
					{
						DoMission(i, 1);
					}
				}
			}

			if (!v_counts_premium.empty())
			{
				for (int i = 0; i < missions_bp_premium.size(); ++i)
				{
					if (missions_bp_premium[i].type == 6)
					{
						DoMissionPremium(i, 1);
					}
				}
			}
#endif
#ifdef ENABLE_RANKING
			SetRankPoints(10, GetRankPoints(10) + 1);
#endif
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			UINT bCell = item->GetCell();

			NotifyRefineSuccess(this, item, szRefineType);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
#ifdef ENABLE_STONES_STACKFIX
			DWORD dwStonesVnum = item->GetVnum();

			if (dwStonesVnum >= 28000 && dwStonesVnum <= 29000)
			{
				item->SetCount(item->GetCount() - 1);
				pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, pos));
				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			}
			else
			{
				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
#ifdef ENABLE_ITEM_UPGRADE_NOTICE
				if (pkNewItem->GetRefineLevel() >= ITEM_UPGRADE_NOTICE_MIN_LEVEL) 
				{
					char gelistirmeduyuru[QUERY_MAX_LEN];
					snprintf(gelistirmeduyuru, sizeof(gelistirmeduyuru), "|cff00FF7F[%s]|r isimli oyuncu |cffffcc00[%s]|r elde etti.", GetName(), pkNewItem->GetName()); 
					BroadcastNotice(gelistirmeduyuru);
				}
#endif
				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
				pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			}
#else
			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
#ifdef ENABLE_ITEM_UPGRADE_NOTICE
			if (pkNewItem->GetRefineLevel() >= ITEM_UPGRADE_NOTICE_MIN_LEVEL) 
			{
				char gelistirmeduyuru[QUERY_MAX_LEN];
				snprintf(gelistirmeduyuru, sizeof(gelistirmeduyuru), "|cff00FF7F[%s]|r isimli oyuncu |cffffcc00[%s]|r elde etti.", GetName(), pkNewItem->GetName()); 
				BroadcastNotice(gelistirmeduyuru);
			}
#endif
#endif
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -prt->cost);
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
#ifdef ENABLE_STONES_STACKFIX
			DWORD dwStonesVnum = item->GetVnum();

			if (dwStonesVnum >= 28000 && dwStonesVnum <= 29000)
			{
				item->SetCount(item->GetCount() - 1);
				pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, pos));
				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			}
			else
			{
				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");
				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
				pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			}
#else
			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");
#endif

			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			pkNewItem->AttrLog();

			//PointChange(POINT_GOLD, -prt->cost);
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

#ifdef ENABLE_QUEEN_NETHIS
bool CHARACTER::DoRefineSerpent(LPITEM item
#ifdef ENABLE_PITTY_REFINE
, bool bUseSealOfGod/* = false*/
#endif

)
{
	if (!item || !CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

#ifdef USE_CRYSTAL_SYSTEM
	if (item->GetType() == ITEM_CRYSTAL)
	{
		const auto* pProto = ITEM_MANAGER::instance().GetTable(MAGIC_SPIRAL_VNUM);
		if (pProto)
		{
			char szItemName[ITEM_NAME_MAX_LEN + 1] = {};
			snprintf(szItemName, sizeof(szItemName), "I%u", MAGIC_SPIRAL_VNUM);
			ChatPacket(CHAT_TYPE_INFO, "1518 %s %s", item->GetName(), szItemName);
			// ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s can be improved just through %s."), item->GetName(), pProto->szLocaleName);
		}

		ClearRefineMode();
		return false;
	}
#endif

	const TRefineTable* prt = CRefineManager::Instance().GetRefineRecipe(item->GetRefineSet());
	if (!prt)
		return false;

	const uint32_t result_vnum = item->GetRefinedVnum();

	// REFINE_COST
	const int cost = ComputeRefineFee(prt->cost);
	// END_OF_REFINE_COST

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;991]");
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
		return false;

	const TItemTable* pProto = ITEM_MANAGER::Instance().GetTable(item->GetRefinedVnum());
	if (!pProto)
	{
		sys_err("DoRefineSerpent NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, "[LS;1002]");
		return false;
	}

	// REFINE_COST
	if (GetGold() < cost)
	{
		ChatPacket(CHAT_TYPE_INFO, "[LS;67]");
		return false;
	}

	int prob = number(1, 100);
	prob -= 10;
	// END_OF_REFINE_COST

#ifdef ENABLE_PITTY_REFINE
	if (bUseSealOfGod && CountSpecifyItem(ITEM_VNUM_SEAL_OF_GOD) > 0)
	{
		RemoveSpecifyItem(ITEM_VNUM_SEAL_OF_GOD, 1);
		prob -= EXTRA_CHANCE_SEAL_OF_GOD;
		if (prob <= 0)
			prob = 1;
	}
#endif

	if (prob <= prt->prob)
	{
		// Success! All items disappear, and other items with the same attribute are acquired
		LPITEM pkNewItem = ITEM_MANAGER::Instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
#ifdef ENABLE_SEALBIND_SYSTEM
			pkNewItem->SetSealDate(item->GetSealDate());
#endif
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::Instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			const uint16_t wCell = item->GetCell();	//@fixme519

			// DETAIL_REFINE_LOG
			NotifyRefineSuccess(this, item, "SERPENT");
			DBManager::Instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);

			ITEM_MANAGER::Instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
			// END_OF_DETAIL_REFINE_LOG

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
			ITEM_MANAGER::Instance().FlushDelayedSave(pkNewItem);

			sys_log(0, "Refine Success %d", cost);
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -cost);
			sys_log(0, "PayPee %d", cost);
			PayRefineFee(cost);
			sys_log(0, "PayPee End %d", cost);
		}
		else
		{
			// DETAIL_REFINE_LOG
			// Failed to create item -> Considered as improvement failure
			// sys_err("cannot create item %u", result_vnum);
			NotifyRefineFail(this, item, "SERPENT");
			// END_OF_DETAIL_REFINE_LOG
		}
	}
	else
	{
		// Failure! All items disappear.
		DBManager::Instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
		NotifyRefineFail(this, item, "SERPENT");
		PayRefineFee(cost);
	}

	return true;
}
#endif

bool CHARACTER::RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell)
{
	if (bCell > INVENTORY_MAX_NUM)
		return false;

	LPITEM item = GetInventoryItem(bCell);

	if (!item)
	{
		return false;
	}

	// REFINE_COST
	if (bType == REFINE_TYPE_MONEY_ONLY)
	{
#ifdef ENABLE_QUEEN_NETHIS
		if (SnakeLair::CSnk::Instance().IsSnakeMap(GetMapIndex()))
		{
			if (get_global_time() < GetQuestFlag("snake_lair.refine_time"))
			{
				ChatPacket(CHAT_TYPE_INFO, "Yýlan Tapýnaðý Demircisi için her 24 saatte bir ödül alabilirsin."); // Custom
				return false;
			}
		}
		else
#endif
		{
			if (!GetQuestFlag("deviltower_zone.can_refine"))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only be rewarded once for the Demon Tower Quest."));
				return false;
			}
		}
	}
	// END_OF_REFINE_COST

	if (item->GetRefinedVnum() == 0)
	{
		sys_err("RefineInformation p.result_vnum == 0 (%u)", item->GetVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
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
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		if (bType == 0)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be advanced this way."));
			return false;
		}
		else
		{
			LPITEM itemScroll = GetInventoryItem(iAdditionalCell);
			if (!itemScroll || item->GetVnum() == itemScroll->GetVnum())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot combine identical Advancement Scrolls."));
				return false;
			}
		}
	}

	CRefineManager & rm = CRefineManager::instance();

	const TRefineTable* prt = rm.GetRefineRecipe(item->GetRefineSet());

	if (!prt)
	{
		sys_err("RefineInformation NOT GET REFINE SET %d", item->GetRefineSet());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		return false;
	}

	//MAIN_QUEST_LV7
	if (GetQuestFlag("main_quest_lv7.refine_chance") > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The free weapon improvements can only be used on weapons up to level 20."));
			return false;
		}
		p.cost = 0;
	}
	else
		p.cost = ComputeRefineFee(prt->cost);

	//END_MAIN_QUEST_LV7
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
	// END_OF_REFINE_COST

	GetDesc()->Packet(&p, sizeof(TPacketGCRefineInformation));

	SetRefineMode(iAdditionalCell);

#ifdef ENABLE_PITTY_REFINE
	int iMaxPittyLevel = item->GetLimitPittyRefine();

	if (iMaxPittyLevel != 0)
	{
		SendPittyInfoClient(item->GetVnum(), item->GetRefineLevel());
	}
#endif

	return true;
}

bool CHARACTER::RefineItem(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
		return false;

	if (pkItem->GetSubType() == USE_TUNING)
	{
		// MUSIN_SCROLL
		if (pkItem->GetValue(0) == MUSIN_SCROLL)
			RefineInformation(pkTarget->GetCell(), REFINE_TYPE_MUSIN, pkItem->GetCell());
		// END_OF_MUSIN_SCROLL
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
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is no Stone available to take out."));
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
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot store this location."));
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
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There isn't enough space in your inventory."));
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
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your level is too low to use this item."));
				return;
			}
			else
				break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot teleport to a safe position in a foreign Kingdom."));
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
	ChatPacket(CHAT_TYPE_COMMAND, "%s", szLine);
	sys_log(0, "%s", szLine);
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
		const int curLife		= ch->GetHP();
		const int needPercent	= WARP_NEED_LIFE_PERCENT;
		const int needLife = ch->GetMaxHP() * needPercent / 100;

		if (curLife < needLife)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough HP."));
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
	// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM
	return consumeLife;
}

int CalculateConsumeSP(LPCHARACTER lpChar)
{
	static const int NEED_WARP_SP_PERCENT = 30;

	const int curSP = lpChar->GetSP();
	const int needSP = lpChar->GetMaxSP() * NEED_WARP_SP_PERCENT / 100;

	if (curSP < needSP)
	{
		lpChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough Spell Points (SP) to use this."));
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
				LocaleChatPacket(CHAT_TYPE_INFO, 437, "%d", pGetTime[0]);
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
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your level is too low to use this item."));
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

#ifdef ENABLE_ITEM_DUPE_FIX
	if (!CanHandleItem())
	{
		return false;
	}
#endif

#ifdef ENABLE_BUFFI_SYSTEM
	if (CBuffiManager::instance().OnUseItem(this, item))
		return true;
#endif

	if (CArenaManager::instance().IsLimitedItem( GetMapIndex(), item->GetVnum() ) == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
		return false;
	}

	// @fixme402 (IsLoadedAffect to block affect hacking)
	if (!IsLoadedAffect())
	{
		ChatPacket(CHAT_TYPE_INFO, "Etkiler henüz yüklenmedi!");
		return false;
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (item->GetType() == ITEM_GROWTH_PET && item->GetSubType() == PET_BAG)
		iLimitRealtimeStartFirstUseFlagIndex = -1;
#endif

	// After the initial use of an item, time is deducted even if it is not used.
	if (-1 != iLimitRealtimeStartFirstUseFlagIndex)
	{
		// Whether the item has been used even once is determined by looking at Socket1. (Record the number of uses in Socket1)
		if (0 == item->GetSocket(1))
		{
			// Use the limit value as the default value for the available time, but if there is a value in Socket0, use that value. (Unit is seconds)
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
#ifdef ENABLE_GEM_BAG_ANTI_SELL_TOOLTIP
			if (item->GetSubType() == QUEST_GEM_BAG)
			{
				int gemAmount = item->GetSocket(2);
				if (gemAmount > 0)
				{
					if (GetGem() + gemAmount >= GEM_MAX)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Zaten maksimum sayida Gayaya sahipsin."));
						return false;
					}

					PointChange(POINT_GEM, gemAmount);
					ITEM_MANAGER::instance().RemoveItem(item);
					return true;
				}
				else
				{
					if (GetGem() <= 0)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Keseye koymak icin yeterli Gaya'niz yok."));
						return false;
					}

					int fillAmount = MIN(GetGem(), 9999);
					PointChange(POINT_GEM, -fillAmount);
					item->SetSocket(2, fillAmount);
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Gaya kesesine %d Gaya dolduruldu."), fillAmount);
					return true;
				}
			}
#endif
			if (GetArena() != NULL || IsObserverMode() == true)
			{
				if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053 || item->GetVnum() == 50049)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
					return false;
				}
			}
#ifdef __AUTO_HUNT__
			if (item->GetVnum() == 49324)
			{
				if (FindAffect(AFFECT_AUTO_HUNT))
				{
					ChatPacket(CHAT_TYPE_INFO, "Bu etkiye zaten sahipsin.");
					return false;
				}
				AddAffect(AFFECT_AUTO_HUNT, POINT_NONE, 0, AFF_NONE, 60 * 60 * 24 * item->GetValue(0), 0, false);
				ChatPacket(CHAT_TYPE_INFO, "Etki %d gün süreyle eklendi.", item->GetValue(0));
				item->SetCount(item->GetCount() - 1);
				return true;
			}
#endif
			if (GetWear(WEAR_MOUNT))
			{
				if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053 || item->GetVnum() == 50049)
				{
					ChatPacket(CHAT_TYPE_INFO, "Bineðin Varken Atýný Çaðýramazsýn.");
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
				if (thecore_pulse() < LastCampFireUse + 60)	{	//@fixme502
					LocaleChatPacket(CHAT_TYPE_INFO, 789, "");
					return false;
				}

				float fx, fy;
				GetDeltaByDegree(GetRotation(), 100.0f, &fx, &fy);

				LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy));

				if (!tree)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot build a campfire here."));
					return false;
				}

				if (tree->IsAttr((long)(GetX()+fx), (long)(GetY()+fy), ATTR_WATER))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot build a campfire under water."));
					return false;
				}

				LPCHARACTER campfire = CHARACTER_MANAGER::instance().SpawnMob(fishing::CAMPFIRE_MOB, GetMapIndex(), (long)(GetX()+fx), (long)(GetY()+fy), 0, false, number(0, 359));

				char_event_info* info = AllocEventInfo<char_event_info>();

				info->ch = campfire;

				campfire->m_pkMiningEvent = event_create(kill_campfire_event, info, PASSES_PER_SEC(40));

				item->SetCount(item->GetCount() - 1);

				LastCampFireUse = thecore_pulse();	//@fixme502
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
										return false;
									}

									AddAffect(AFFECT_UNIQUE_ABILITY, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true, true);
									EffectPacket(SE_DXUP_PURPLE);
									break;

								case APPLY_ATT_SPEED:
									if (FindAffect(AFFECT_ATT_SPEED))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
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
							}
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
								if (item->GetVnum() == 71221)
								{
									if(FindAffect(AFFECT_DECORATION))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
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
		case ITEM_PICK:
#ifdef ENABLE_PASSIVE_SYSTEM
		case ITEM_PASSIVE:
#endif
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
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
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
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be opened with a key."));
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
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d Yang."), dwCounts[i]);
									break;

								case CSpecialItemGroup::EXP:
									LocaleChatPacket(CHAT_TYPE_INFO, 401, "");
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d experience points."), dwCounts[i]);
									break;

								case CSpecialItemGroup::MOB:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
									break;

								case CSpecialItemGroup::SLOW:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the red smoke coming out of the box, your speed will increase!"));
									break;

								case CSpecialItemGroup::DRAIN_HP:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The box suddenly exploded! You have lost Hit Points (HP)."));
									break;

								case CSpecialItemGroup::POISON:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the green smoke that is coming out of the box, the poison will spread through your body!"));
									break;

								case CSpecialItemGroup::MOB_GROUP:
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
									break;

								default:
#ifndef ENABLE_RENEWAL_SPECIAL_CHAT
									if (item_gets[i])
									{
										if (dwCounts[i] > 1)
											LocaleChatPacket(CHAT_TYPE_INFO, 24, "%s#%d", item_gets[i]->GetLocaleName(), dwCounts[i]);
										else
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item_gets[i]->GetLocaleName());
									}
#endif
									break;
							}
						}
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This key does not seem to fit the lock."));
						return false;
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This key does not seem to fit the lock."));
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

				if (dwBoxVnum == 50033 && LC_IsYMIR())
				{
					if (GetLevel() < 15)
					{
						ChatPacket(CHAT_TYPE_INFO, "15·¹º§ ÀÌÇÏ¿¡¼­´Â »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù.");
						return false;
					}
				}

#ifdef ENABLE_DRAGONSOUL_SYSTEM
				if( (dwBoxVnum > 51500 && dwBoxVnum < 52000) || (dwBoxVnum >= 50255 && dwBoxVnum <= 50260) )
				{
					if (!(this->DragonSoul_IsQualified()))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Before you open the Cor Draconis, you have to complete the Dragon Stone quest and activate the Dragon Stone Alchemy."));
						return false;
					}
				}
#endif
				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
#ifdef ENABLE_RANKING
					SetRankPoints(15, GetRankPoints(15) + 1);
#endif
					item->SetCount(item->GetCount()-1);
#ifdef ENABLE_BATTLE_PASS
				if (!v_counts.empty())
				{
					for (int i = 0; i < missions_bp.size(); ++i)
					{
						if (missions_bp[i].type == 34)
						{
							DoMission(i, 1);
						}
					}
				}

				if (!v_counts_premium.empty())
				{
					for (int i = 0; i < missions_bp_premium.size(); ++i)
					{
						if (missions_bp_premium[i].type == 34)
						{
							DoMissionPremium(i, 1);
						}
					}
				}
#endif

					for (int i = 0; i < count; i++)
					{
						switch (dwVnums[i])
						{
							case CSpecialItemGroup::GOLD:
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d Yang."), dwCounts[i]);
								break;

							case CSpecialItemGroup::EXP:
								LocaleChatPacket(CHAT_TYPE_INFO, 401, "");
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d experience points."), dwCounts[i]);
								break;

							case CSpecialItemGroup::MOB:
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
								break;

							case CSpecialItemGroup::SLOW:
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the red smoke coming out of the box, your speed will increase!"));
								break;

							case CSpecialItemGroup::DRAIN_HP:
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The box suddenly exploded! You have lost Hit Points (HP)."));
								break;

							case CSpecialItemGroup::POISON:
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the green smoke that is coming out of the box, the poison will spread through your body!"));
								break;

							case CSpecialItemGroup::MOB_GROUP:
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
								break;

							default:
#ifndef ENABLE_RENEWAL_SPECIAL_CHAT
								if (item_gets[i])
								{
									if (dwCounts[i] > 1)
										LocaleChatPacket(CHAT_TYPE_INFO, 24, "%s#%d", item_gets[i]->GetLocaleName(), dwCounts[i]);
									else
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item_gets[i]->GetLocaleName());
								}
#endif
								break;
						}
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have not received anything."));
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
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have lowered your Skill Level."));
				}
				else
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot lower your Skill Level."));
			}
			break;

		case ITEM_SKILLBOOK:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read a book while you are transformed."));
					return false;
				}

				DWORD dwVnum = 0;

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

#ifdef ENABLE_NINETH_SKILL
					if (IsNineSkill(dwVnum))
					{
						const auto bSkillGroup = static_cast<uint8_t>(item->GetValue(3));
						if (LearnNineSkillByBook(dwVnum, bSkillGroup))
						{
#ifdef ENABLE_BOOKS_STACKFIX
							item->SetCount(item->GetCount() - 1);
#else
							ITEM_MANAGER::Instance().RemoveItem(item);
#endif
						}
						return false;
					}
#endif

					if (true == LearnSkillByBook(dwVnum))
					{
#ifndef DEFAULT_REMOVE_ALL_BOOKSS
						item->SetCount(item->GetCount() - 1);
#else
						ITEM_MANAGER::instance().RemoveItem(item);
#endif
						int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

						if (distribution_test_server)
						SetSkillNextReadTime(dwVnum, get_global_time() + iReadDelay);
					}
				}
			}

			break;

		case ITEM_USE:
			{
#ifdef ENABLE_GEM_CONVERTER
				if (item->GetVnum() == 50935)
				{
					if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Tm pencereleri kapat!"));
						return false;
					}

					LPITEM item2 = GetItem(DestCell);
					if (!item2)
						return false;

					if (item2->GetVnum() != 50926)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu eya sadece Ss Ta zerinde kullanlabilir."));
						return false;
					}

					if (GetGem() >= GEM_MAX)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Zaten maksimum sayda Gayaya sahipsin."));
						return false;
					}

					int need_gemstone = (item->GetValue(0) > 0) ? item->GetValue(0) : 10;
					int need_yang = (item->GetValue(1) > 0) ? item->GetValue(1) : 15000;
					int give_gem = (item->GetValue(2) > 0) ? item->GetValue(2) : 1;

					if (item2->GetCount() < need_gemstone)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("En az %d ss tana ihtiyacn var."), need_gemstone);
						return false;
					}

					if (GetGold() < need_yang)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince Yang'n yok."));
						return false;
					}

					PointChange(POINT_GOLD, -need_yang);
					item->SetCount(item->GetCount() - 1);
					item2->SetCount(item2->GetCount() - need_gemstone);

					int prob = load_gem_values.prob_gem > 0 ? load_gem_values.prob_gem : 30;
					if (number(1, 100) <= prob)
					{
						PointChange(POINT_GEM, give_gem);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d Gaya dntrc kullandn."), 1);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d ss ta tketildi."), need_gemstone);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Gaya> Kazanlan gaya miktar: %d"), give_gem);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Gaya> Bileme ilemi baarl oldu."));
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d Gaya dntrc kullandn."), 1);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d ss ta tketildi."), need_gemstone);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Gaya> Bileme ilemi baarsz oldu."));
					}
					return true;
				}
#endif
#ifdef ENABLE_AURA_CLEAR
				if (item->GetVnum() == 49980 || item->GetVnum() == 89999)
				{
					if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Tum pencereleri kapat!"));
						return false;
					}

					LPITEM item2 = GetItem(DestCell);
					if (!item2)
						return false;

#ifdef ENABLE_AURA_CLEAR_MESSAGE_WHEN_EQUIP
					if (item2->IsEquipped())
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Giyiliyken bu nesneyi kullanamazsin."));
						return false;
					}
#endif

#ifdef ENABLE_SEALBIND_SYSTEM
					if (item2->IsSealed())
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ruh bagli nesnede bunu kullanamazsin."));
						return false;
					}
#endif

					if (item2->GetType() != ITEM_COSTUME || item2->GetSubType() != COSTUME_AURA)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu nesne sadece Aura Giysisi uzerinde kullanilabilir."));
						return false;
					}

					if (item2->GetSocket(ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM) == 0)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura Giysisinde emilmis herhangi bir deger yok."));
						return false;
					}

					item2->SetSocket(ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM, 0);
					item2->ClearAttribute();
					item2->UpdatePacket();

					LogManager::instance().ItemLog(this, item2, "AURA_CLEAR_SUCCESS", item2->GetName());
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aura Giysisinin emilen tum degerleri basariyla kaldirildi."));

					item->SetCount(item->GetCount() - 1);
					return true;
				}
#endif
#ifdef ENABLE_COLLECT_WINDOW
				switch(item->GetVnum())
				{
					case 72348:
					case 72349:
						{
							if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
							{
								ChatPacket(CHAT_TYPE_INFO, "Tüm pencereleri kapat!");
								return false;
							}
							int val = item->GetValue(0);
							int type = item->GetValue(1);
							if (type == 0) {
								time_t item_delay = GetQuestFlag("biologist.item_delay");
								time_t current_time = get_global_time();
								time_t remaining_time = item_delay - current_time;
				
								time_t reduced_time = remaining_time - (remaining_time * val / 100);
				
								if (reduced_time > 10) {
									SetQuestFlag("biologist.item_delay", current_time + reduced_time);
				
									ChatPacket(CHAT_TYPE_COMMAND, "Güncelleme Zamaný 0 %d", reduced_time);
									item->SetCount(item->GetCount() - 1);
				
									ChatPacket(CHAT_TYPE_INFO, "Eþyayý teslim etme bekleme süresi baþarýyla kýsaltýldý.");
								} else {
									ChatPacket(CHAT_TYPE_INFO, "Bekleme süresi zaten maksimum düzeyde kýsaltýldý.");
									return false;
								}
							} else if (type == 1) {
								int collector_state = GetQuestFlag("collector.state");
				
								std::string delay_flag_key = "collector.item_delay" + std::to_string(collector_state);
				
								time_t item_delay = GetQuestFlag(delay_flag_key);
								time_t current_time = get_global_time();
								time_t remaining_time = item_delay - current_time;
				
								time_t reduced_time = remaining_time - (remaining_time * val / 100); 
				
								if (reduced_time > 10) {
									SetQuestFlag(delay_flag_key, current_time + reduced_time);
				
									ChatPacket(CHAT_TYPE_COMMAND, "Güncelleme Zamaný 1 %d", reduced_time);
									item->SetCount(item->GetCount() - 1);
				
									ChatPacket(CHAT_TYPE_INFO, "Eþyayý teslim etme bekleme süresi baþarýyla kýsaltýldý.");
								} else {
									ChatPacket(CHAT_TYPE_INFO, "Bekleme süresi zaten maksimum düzeyde kýsaltýldý.");
									return false;
								}
							}
							break;
						}
				
					case 71036:
					case 71037:
						{
							if (GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen() || GetShop())
							{
								ChatPacket(CHAT_TYPE_INFO, "Tüm pencereleri kapat!");
								return false;
							}
							int val = item->GetValue(0);
							int type = item->GetValue(1);
							int max_increase = item->GetValue(2);
							if (type == 0) {
								int take_chance = GetQuestFlag("biologist.take_chance");
								if (take_chance == max_increase || take_chance > max_increase) {
									ChatPacket(CHAT_TYPE_INFO, "Teslim etme þansý zaten maksimum seviyede!");
									return false;
								}
								SetQuestFlag("biologist.take_chance", take_chance+val);
								item->SetCount(item->GetCount()-1);
				
								ChatPacket(CHAT_TYPE_INFO, "Teslim etme þansý %%25 artýrýldý.");
				
								int new_chance = GetQuestFlag("biologist.take_chance");
				
								ChatPacket(CHAT_TYPE_INFO, "Mevcut Þans: %d", new_chance);
								ChatPacket(CHAT_TYPE_COMMAND, "Güncelleme Þansý 0 %d", new_chance);
							} else if (type == 1) {
								int take_chance = GetQuestFlag("collector.take_chance");
								if (take_chance == max_increase || take_chance > max_increase) {
									ChatPacket(CHAT_TYPE_INFO, "Teslim etme þansý zaten maksimum seviyeye çýkarýldý!");
									return false;
								}
								SetQuestFlag("collector.take_chance", take_chance+val);
								item->SetCount(item->GetCount()-1);
				
								ChatPacket(CHAT_TYPE_INFO, "Teslim etme þansý %%25 artýrýldý.");
				
								int new_chance = GetQuestFlag("collector.take_chance");
				
								ChatPacket(CHAT_TYPE_INFO, "Mevcut Þans: %d", new_chance);
								ChatPacket(CHAT_TYPE_COMMAND, "Güncelleme Þansý 1 %d", new_chance);
							}
							break;
						}
				}
#endif
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
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
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
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
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
#ifdef ENABLE_SUNG_MAHI_TOWER
							if (!IS_SUNG_MAHI_ENABLE_ITEM(GetMapIndex(), item->GetVnum()))
							{
								ChatPacket(CHAT_TYPE_INFO, "Bu tür iksiri burada kullanamazsýn.");
								return false;
							}
#endif
							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
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
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That is over the limit."));
												return false;
											}
										}
										break;

									default :
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
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
										ChatPacket(CHAT_TYPE_INFO, "Ay Keki veya Tohum kullanýldý.");

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
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
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
										snprintf(buf, sizeof(buf), "Inc %ds by item{VN:%d SOC%d:%d}", ret, item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
									else
										snprintf(buf, sizeof(buf), "Inc %ds by item{VN:%d VAL%d:%d}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));

									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d seconds have been added."), ret);
									item->SetCount(item->GetCount() - 1);
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
									return true;
								}
								else
								{
									if (item->GetVnum() == DRAGON_HEART_VNUM)
										snprintf(buf, sizeof(buf), "No change by item{VN:%d SOC%d:%d}", item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
									else
										snprintf(buf, sizeof(buf), "No change by item{VN:%d VAL%d:%d}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));

									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot recharge this Dragon Stone."));
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
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d seconds have been added."), ret);
									snprintf(buf, sizeof(buf), "Increase %ds by item{VN:%d VAL%d:%d}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
									LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
									item->SetCount(item->GetCount() - 1);
									return true;
								}
								else
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot recharge this Dragon Stone."));
									snprintf(buf, sizeof(buf), "No change by item{VN:%d VAL%d:%d}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
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
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
							case 70002:
							{
								if(FindAffect(AFFECT_AUTO_PICK_UP))
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
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
#ifdef ENABLE_SET_ITEM
							case 39115: // Set Clear Scroll
								{
									LPITEM item2;
									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging())
										return false;

									if (item2->isLocked())
										return false;

									if (item2->GetSetValue() == 0)
										return false;

									item2->SetSetValue(0);
									item2->UpdatePacket();

									item->SetCount(item->GetCount() - 1);
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SET_ITEM_CLEAR_SUCCESS"));
								}
								break;
#endif
#ifdef ENABLE_ZODIAC_MISSION
							case 33031: // 12 Bead 
							{
								int bCount = GetQuestFlag("Bead.Count");
								SetQuestFlag("Bead.Count", GetQuestFlag("Bead.Count") + 12);
								bCount = GetQuestFlag("Bead.Count"); // Update
								ChatPacket(CHAT_TYPE_COMMAND, "SetBeadCount %d", bCount);
								item->SetCount(item->GetCount() - 1);
								ChatPacket(CHAT_TYPE_INFO, ("%d Adet Animasfer eklendi. "), 12);
							}
							break;
							case 33032: // 24 Bead 
							{
								int bCount = GetQuestFlag("Bead.Count");
								SetQuestFlag("Bead.Count", GetQuestFlag("Bead.Count") + 24);
								bCount = GetQuestFlag("Bead.Count"); // Update
								ChatPacket(CHAT_TYPE_COMMAND, "SetBeadCount %d", bCount);
								item->SetCount(item->GetCount() - 1);
								ChatPacket(CHAT_TYPE_INFO, ("%d Adet Animasfer eklendi. "), 24);
							}
							break;
							case 33033: // 36 Bead 
							{
								int bCount = GetQuestFlag("Bead.Count");
								SetQuestFlag("Bead.Count", GetQuestFlag("Bead.Count") + 36);
								bCount = GetQuestFlag("Bead.Count"); // Update
								ChatPacket(CHAT_TYPE_COMMAND, "SetBeadCount %d", bCount);
								item->SetCount(item->GetCount() - 1);
								ChatPacket(CHAT_TYPE_INFO, ("%d Adet Animasfer eklendi. "), 36);
							}
							break;
				
							case 33034: // 48 Bead 
							{
								int bCount = GetQuestFlag("Bead.Count");
								SetQuestFlag("Bead.Count", GetQuestFlag("Bead.Count") + 12);
								bCount = GetQuestFlag("Bead.Count"); // Update
								ChatPacket(CHAT_TYPE_COMMAND, "SetBeadCount %d", bCount);
								item->SetCount(item->GetCount() - 1);
								ChatPacket(CHAT_TYPE_INFO, ("%d Adet Animasfer eklendi. "), 12);
							}
							break;
				
							case 33035: // 60 Bead 
							{
								int bCount = GetQuestFlag("Bead.Count");
								SetQuestFlag("Bead.Count", GetQuestFlag("Bead.Count") + 60);
								bCount = GetQuestFlag("Bead.Count"); // Update
								ChatPacket(CHAT_TYPE_COMMAND, "SetBeadCount %d", bCount);
								item->SetCount(item->GetCount() - 1);
								ChatPacket(CHAT_TYPE_INFO, ("%d Adet Animasfer eklendi. "), 60);
							}
							break;
#endif
							case ITEM_NOG_POCKET:
								{
									if (FindAffect(AFFECT_NOG_ABILITY))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
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
									if (FindAffect(AFFECT_RAMADAN_ABILITY)) // @fixme61
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BU_ETKI_DEVAM_EDIYOR"));
										return false;
									}
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
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
												break;
											}
										}

										if (pMarriage->ch2 != NULL)
										{
											if (CArenaManager::instance().IsArenaMap(pMarriage->ch2->GetMapIndex()) == true)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot wear a Wedding Ring if you are not married."));
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
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use the Event Detector from this position."));
											return false;
										}
									}

									int dist = 0;
									float distance = (DISTANCE_SQRT(GetX()-item->GetSocket(1), GetY()-item->GetSocket(2)));

									if (distance < 1000.0f)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Event Detector vanished in a mysterious light."));

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

														DBManager::instance().ReturnQuery(QID_LOTTO, GetPlayerID(), pdw,
																"INSERT INTO lotto_list VALUES(0, 'server%s', %u, NOW())", 
																get_table_postfix(), GetPlayerID());
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Event Detector has vanished."));
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
												++stone;	//@fixme541
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
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is currently no Metin Stone on this map."));
											}
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is currently no Metin Stone on this map."));
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

#ifdef ENABLE_BATTLE_PASS
							case ITEM_BATTLE_PASS:
							{
								if (!v_counts.empty())
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1073"));
									return false;
								}

								FILE* fileID;
								char file_name[256 + 1];

								snprintf(file_name, sizeof(file_name), "%s/battlepass_players/%s.txt", LocaleService_GetBasePath().c_str(), GetName());
								fileID = fopen(file_name, "w");

								if (NULL == fileID)
									return false;

								for (int i = 0; i < missions_bp.size(); ++i)
								{
									fprintf(fileID, "MISSION	%d	%d\n", 0, 0);
								}

								fclose(fileID);

								Load_BattlePass();
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1074"));
								item->SetCount(item->GetCount() - 1);
							}
							break;
							case ITEM_BATTLE_PASS_PREMIUM:
							{
								if (!v_counts_premium.empty())
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1073"));
									return false;
								}

								FILE* fileID;
								char file_name[256 + 1];

								snprintf(file_name, sizeof(file_name), "%s/battlepass_premium_players/%s.txt", LocaleService_GetBasePath().c_str(), GetName());
								fileID = fopen(file_name, "w");
				
								if (NULL == fileID)
									return false;

								for (int i = 0; i < missions_bp_premium.size(); ++i)
								{
									fprintf(fileID, "MISSION	%d	%d\n", 0, 0);
								}

								fclose(fileID);

								Load_BattlePassPremium();
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1074"));
								item->SetCount(item->GetCount() - 1);
							}
							break;
#endif

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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your status while you are transformed."));
										return false;
									}

									int lv = GetSkillLevel(SKILL_LEADERSHIP);

									if (lv < item->GetValue(0))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("It isn't easy to understand this book."));
										return false;
									}

									if (lv >= item->GetValue(1))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This book will not help you."));
										return false;
									}

									if (LearnSkillByBook(SKILL_LEADERSHIP))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read a book while you are transformed."));
										return false;
									}
									if (GetSkillLevel(SKILL_COMBO) == 0 && GetLevel() < 30)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to have a minimum level of 30 to understand this book."));
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) == 1 && GetLevel() < 50)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a minimum level of 50 to understand this book."));
										return false;
									}

									if (GetSkillLevel(SKILL_COMBO) >= 2)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't train any more Combos."));
										return false;
									}

									int iPct = item->GetValue(0);

									if (LearnSkillByBook(SKILL_COMBO, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read a book while you are transformed."));
										return false;
									}

									DWORD dwSkillVnum = item->GetValue(0);
									int iPct = MINMAX(0, item->GetValue(1), 100);
									if (GetSkillLevel(dwSkillVnum) >= 20 || dwSkillVnum-SKILL_LANGUAGE1+1 == GetEmpire())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already understand this language."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										item->SetCount(item->GetCount() - 1);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;

							case 50061:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read a book while you are transformed."));
										return false;
									}

									DWORD dwSkillVnum = item->GetValue(0);
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum) >= 10)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot improve this skill any further."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your status while you are transformed."));
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to improve your Level to read this Book."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= 40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot improve this skill any further."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) < iSkillLevelLowLimit)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("It isn't easy to understand this book."));
										return false;
									}

									if (GetSkillLevel(dwSkillVnum) >= iSkillLevelHighLimit)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot train with this Book any more."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read a book while you are transformed."));
										return false;
									}

									DWORD dwSkillVnum = SKILL_CREATE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum) >= 40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot improve this skill any further."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);

										if (test_server) 
										{
											ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Success to learn skill ");
										}
									}
									else
									{
										if (test_server) 
										{
											ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Failed to learn skill ");
										}
									}
								}
								break;

								// MINING
							case ITEM_MINING_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read a book while you are transformed."));
										return false;
									}

									DWORD dwSkillVnum = SKILL_MINING;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetSkillLevel(dwSkillVnum) >= 40)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot improve this skill any further."));
										return false;
									}

									if (LearnSkillByBook(dwSkillVnum, iPct))
									{
										ITEM_MANAGER::instance().RemoveItem(item);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
								}
								break;
								// END_OF_MINING

							case ITEM_HORSE_SKILL_TRAIN_BOOK:
								{
									if (IsPolymorphed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read a book while you are transformed."));
										return false;
									}

									DWORD dwSkillVnum = SKILL_HORSE;
									int iPct = MINMAX(0, item->GetValue(1), 100);

									if (GetLevel() < 50)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a minimum level of 50 to get riding training."));
										return false;
									}

									if (!test_server && get_global_time() < GetSkillNextReadTime(dwSkillVnum))
									{
										if (FindAffect(AFFECT_SKILL_NO_BOOK_DELAY))
										{
											RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have escaped the evil ghost curse with the help of an Exorcism Scroll."));
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read any more Riding Guides."));
										return false;
									}

									if (number(1, 100) <= iPct)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You read the Horse Riding Manual and received a Riding Point."));
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can use this point to improve your riding skill!"));
										PointChange(POINT_HORSE_SKILL, 1);

										int iReadDelay = number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
										if (distribution_test_server) iReadDelay /= 3;

										if (!test_server)
											SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You did not understand the riding guide."));
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your mind is clear. You can concentrate really well now."));
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your rank has increased by %d points."), delta/10);
									}
								}
								break;

							case 71107:
								{
									int val = item->GetValue(0);
									int interval = item->GetValue(1);
									quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
									if (!pPC)// @fixme208
										return false;
									int last_use_time = pPC->GetFlag("mythical_peach.last_use_time");

									if (get_global_time() - last_use_time < interval * 60 * 60)
									{
										if (test_server == false)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¾ÆÁ÷ »ç¿ëÇÒ ¼ö ¾ø½À´Ï´Ù."));
											return false;
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Å×½ºÆ® ¼­¹ö ½Ã°£Á¦ÇÑ Åë°ú"));
										}
									}
									
									if (GetAlignment() == 200000)
										return false;

									if (200000 - GetAlignment() < val * 10)
										val = (200000 - GetAlignment()) / 10;

									int old_alignment = GetAlignment() / 10;

									UpdateAlignment(val*10);

									item->SetCount(item->GetCount() - 1);
									pPC->SetFlag("mythical_peach.last_use_time", get_global_time());

									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your mind is clear. You can concentrate really well now."));
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your rank has increased by %d points."), val);

									char buf[256 + 1];
									snprintf(buf, sizeof(buf), "%d %d", old_alignment, GetAlignment() / 10);
									LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
								}
								break;

#ifdef ENABLE_SEALBIND_SYSTEM
							case 50263: // Seal Scroll (Baglama Parsomeni)
							case 72343:
								{
									LPITEM item2;
									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() || item2->IsEquipped())
										return false;

									if (item2->IsSealed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu esya zaten ruh bagi ile bagli."));
										return false;
									}

									if (item2->GetType() == ITEM_USE && item2->GetSubType() == USE_SPECIAL)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu esyayi ruh bagi ile baglayamazsin."));
										return false;
									}

									item2->SetSealDate(-1);
									item2->UpdatePacket();
									item2->Save();

									item->SetCount(item->GetCount() - 1);
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Esya basariyla ruh bagi ile baglandi."));
								}
								break;

							case 50264: // Normal Unseal Scroll (Ruh Bagi Ayirma Parsomeni)
							case 71151:
								{
									LPITEM item2;
									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() || item2->IsEquipped())
										return false;

									if (!item2->IsSealed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu esya ruh bagi ile bagli degil."));
										return false;
									}

									if (item2->GetSealDate() > 0)
									{
										if (get_global_time() >= item2->GetSealDate())
										{
											item2->SetSealDate(0);
											item2->UpdatePacket();
											item2->Save();
											item->SetCount(item->GetCount() - 1);
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ruh bagi basariyla ayrildi."));
											return true;
										}
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu esyanin ruh bagi ayirma sureci henuz tamamlanmadi."));
										return false;
									}

									long lUnsealTime = get_global_time() + 259200; // 72 hours
									item2->SetSealDate(lUnsealTime);
									item2->UpdatePacket();
									item2->Save();

									item->SetCount(item->GetCount() - 1);
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ruh bagi ayirma sureci baslatildi (72 saat)."));
								}
								break;
#endif

#ifdef ENABLE_UN_SEAL_SCROLL_PLUS
							case 50262: // Unseal Scroll Plus (Ruh Bagi Ayirma Parsomeni+)
							case 72344:
								{
									LPITEM item2;
									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() || item2->IsEquipped())
										return false;

									if (!item2->IsSealed())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu esya ruh bagi ile bagli degil."));
										return false;
									}

									DWORD dwSuccessTime = GetUnsealScrollPlusPasswordSuccessTime();
									if (dwSuccessTime == 0 || get_global_time() - dwSuccessTime > 60)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Depo sifresi dogrulamasi zaman asimina ugradi, lutfen tekrar deneyin."));
										return false;
									}

									item2->SetSealDate(0);
									item2->UpdatePacket();
									item2->Save();

									item->SetCount(item->GetCount() - 1);
									SetUnsealScrollPlusPasswordSuccessTime(0);
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ruh bagi basariyla kaldirildi!"));
								}
								break;
#endif

							case 71109:
							case 72719:
								{
									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
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
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is no stone available for removal."));
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is no stone available for removal."));
										return false;
									}

									LPITEM pItemReward = AutoGiveItem(socket.top());

									if (pItemReward != NULL)
									{
										item2->SetSocket(idx, 1);

										char buf[256+1];
										snprintf(buf, sizeof(buf), "%s(%u) %s(%u)", 
												item2->GetName(), item2->GetID(), pItemReward->GetName(), pItemReward->GetID());
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
									// NEW_HAIR_STYLE_ADD
									if (GetPart(PART_HAIR) >= 1001)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot dye or bleach your current hairstyle."));
									}
									// END_NEW_HAIR_STYLE_ADD
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
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to have reached level %d to be able to dye your hair again."), last_dye_level + 3);
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
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d Yang."), dwCounts[i]);
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


									if (((item->GetVnum() == ITEM_VALENTINE_ROSE) && (SEX_MALE==GET_SEX(this))) ||
										((item->GetVnum() == ITEM_VALENTINE_CHOCOLATE) && (SEX_FEMALE==GET_SEX(this))))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can only be opened by the another gender."));
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


									if (((item->GetVnum() == ITEM_WHITEDAY_CANDY) && (SEX_MALE==GET_SEX(this))) ||
										((item->GetVnum() == ITEM_WHITEDAY_ROSE) && (SEX_FEMALE==GET_SEX(this))))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can only be opened by the another gender."));
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
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d Yang."), dwCounts[i]);
													break;

												case CSpecialItemGroup::EXP:
													LocaleChatPacket(CHAT_TYPE_INFO, 401, "");
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d experience points."), dwCounts[i]);
													break;

												case CSpecialItemGroup::MOB:
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
													break;

												case CSpecialItemGroup::SLOW:
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the red smoke coming out of the box, your speed will increase!"));
													break;

												case CSpecialItemGroup::DRAIN_HP:
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The box suddenly exploded! You have lost Hit Points (HP)."));
													break;

												case CSpecialItemGroup::POISON:
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the green smoke that is coming out of the box, the poison will spread through your body!"));
													break;

												case CSpecialItemGroup::MOB_GROUP:
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
													break;

												default:
#ifndef ENABLE_RENEWAL_SPECIAL_CHAT
													if (item_gets[i])
													{
														if (dwCounts[i] > 1)
															LocaleChatPacket(CHAT_TYPE_INFO, 24, "%s#%d", item_gets[i]->GetLocaleName(), dwCounts[i]);
														else
															ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item_gets[i]->GetLocaleName());
													}
#endif
													break;
											}
										}
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have not received anything."));
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
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
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d Yang."), iGold);
									PointChange(POINT_GOLD, iGold);
								}
								break;

#ifdef ENABLE_GAYA_TICKET_SYSTEM
							case 50000:
							{
								if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("°Å·¡Ã¢,Ã¢°í µîÀ» ¿¬ »óÅÂ¿¡¼­´Â ±ÍÈ¯ºÎ,±ÍÈ¯±â¾ïºÎ ¸¦ »ç¿ëÇÒ¼ö ¾ø½À´Ï´Ù."));
									return false;
								}
								
								int iGem = item->GetSocket(0);
								if (iGem == 0)
								{
									char buf[128];
									snprintf(buf, sizeof(buf), "OpenGemTicket %d", item->GetCell());
									ChatPacket(CHAT_TYPE_COMMAND, "%s", buf);
								}
								else
								{
									if (GetGem() + iGem > 999999)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Envanterinde fazla gaya tasiyorsun."));
										return false;
									}
									ITEM_MANAGER::instance().RemoveItem(item);
									PointChange(POINT_GEM, iGem, true);
								}
							}
							break;
#endif

#ifdef __SYSTEM_SEARCH_ITEM_MOB__
							case 93101: 
								{

									ChatPacket(CHAT_TYPE_COMMAND, "open_searched");
								}
								break;
#endif

							case 27995: {}
								break;

#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
							case 70604:
								{
									if (GetHalounPoints() > 0)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Halloween_Etkin"));
										return false;
									}
									
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Halloween_10_Puan_Aldin"));
									PointChange(POINT_HALOUN, 10);

									OpenHalloween();
									
									item->SetCount(item->GetCount()-1);
								}
								break;
#endif
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
#ifndef ENABLE_RANKING
										SetRankPoints(13, GetRankPoints(13) + 1);
#endif
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
#ifndef ENABLE_RANKING
										SetRankPoints(13, GetRankPoints(13) + 1);
#endif
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
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
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There isn't enough space in your inventory."));
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot be used yet."));
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
#ifdef ENABLE_SUNG_MAHI_TOWER
							if (!IS_SUNG_MAHI_ENABLE_ITEM(GetMapIndex(), item->GetVnum()))
							{
								ChatPacket(CHAT_TYPE_INFO, "Bu tür iksiri burada kullanamazsýn.");
								return false;
							}
#endif

							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
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
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That is over the limit."));
												return false;
											}
										}
										break;

									default :
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
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
#ifdef ENABLE_SUNG_MAHI_TOWER
						if (!IS_SUNG_MAHI_ENABLE_ITEM(GetMapIndex(), item->GetVnum()))
						{
							ChatPacket(CHAT_TYPE_INFO, "Bu tür iksiri burada kullanamazsýn.");
							return false;
						}
#endif
						if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
						{
							if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
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
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That is over the limit."));
											return false;
										}
									}
									break;

								default :
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
										return false;
									}

									AddAffect(AFFECT_MOV_SPEED, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true);
									EffectPacket(SE_DXUP_PURPLE);
									break;

								case APPLY_ATT_SPEED:
									if (FindAffect(AFFECT_ATT_SPEED))
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
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
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this from your current position."));
								return false;
							}

							if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
								return false;
							}

							if (m_pkWarpEvent)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are ready to warp, so you cannot use the Scroll of the Location."));
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s%s cannot be used in a dungeon."), item->GetLocaleName(), "");
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

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
								return false;

							if (item2->GetVnum() >= 28330 && item2->GetVnum() <= 28343)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("+3 spirit stones can not be upgraded by this item."));
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
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The spirit stone can not be advanced by this item."));
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
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
								return false;
							}

							switch (item->GetSubType())
							{
								case USE_CHANGE_COSTUME_ATTR:
									{
										if (item2->GetAttributeCount() == 0)
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is no upgrade that you can change."));
											return false;
										}

										if (item2->GetSubType() == COSTUME_HAIR)
											item2->ChangeAttrCostumeHair();
										else
											item2->ChangeAttrCostume();

										item->SetCount(item->GetCount() - 1);
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have changed the upgrade."));
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

											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Upgrade successfully added."));
											item->SetCount(item->GetCount() - 1);
										}
										else
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can no longer be improved. The maximum number of bonuses has been reached."));
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

							if (item2->GetVnum() == 50201 || item2->GetVnum() == 50202 || item2->GetVnum() == 11901 || item2->GetVnum() == 11902 || item2->GetVnum() == 11903 || item2->GetVnum() == 11904 || item2->GetVnum() == 11911 || item2->GetVnum() == 11912 || item2->GetVnum() == 11913 || item2->GetVnum() == 11914)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Buket Gelinlik ve Smokinlere efsun basilamaz."));
								return false;
							}

							if (ITEM_COSTUME == item2->GetType())
#ifdef ENABLE_AURA_COSTUME_SYSTEM
							if (item->GetSubType() != USE_PUT_INTO_AURA_SOCKET)
#endif
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
								return false;
							}

							if (item2->IsExchanging())
								return false;

							if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
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
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There aren't any Pieces of Broken Stone available for removal."));
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
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
										return false;
									}

									if (item2->GetAttributeCount() == 0)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is no upgrade that you can change."));
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
											ChatPacket(CHAT_TYPE_INFO, "Bu nesneyi sadece Ortalama ve Becerili silahlarda kullanabilirsiniz.");
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
											ChatPacket(CHAT_TYPE_INFO, "Bu nesneyi sadece Ortalama ve Becerili silahlarda kullanabilirsiniz.");
											return false;
										}
									}
									#endif
									else
									{
										if (item->GetVnum() == 71151 || item->GetVnum() == 76023)
										{
											if ((item2->GetType() == ITEM_WEAPON)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
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
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The level of your item is too high."));
													break;
												}
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Can only be used on weapons and armour."));
												break;
											}
										}
										item2->ChangeAttribute();
									}

									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have changed the upgrade."));

									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_ATTRIBUTE", buf);
									}

									item->SetCount(item->GetCount() - 1);
#ifndef ENABLE_RANKING
									SetRankPoints(12, GetRankPoints(12) + 1);
#endif
									break;

								case USE_ADD_ATTRIBUTE:
									if (ITEM_COSTUME == item2->GetType())
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("¨ùO¨ù¨¬A¡í ¨¬?¡Æ©¡CO ¨ùo ¨ú©ª¢¥A ¨ú¨¡AIAUAO¢¥I¢¥U."));
										return false;
									}

									if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
										return false;

									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
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
											if ((item2->GetType() == ITEM_WEAPON)
												|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
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
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The level of your item is too high."));
													break;
												}
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Can only be used on weapons and armour."));
												break;
											}
										}

										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										//if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
										if (number(1, 100) <= 100)
										{
#ifdef ENABLE_MULTI_AFFIX_APPLY_SYSTEM
											while (item2->GetAttributeCount() < 5)//tekefsunda 5 efsun gelsin.
												item2->AddAttribute();
#else
											item2->AddAttribute();
#endif
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Upgrade successfully added."));

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
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No upgrade added."));
											LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE_FAIL", buf);
										}
										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You must use the Blessing Marble in order to add another bonus to this item."));
									}
									break;

								case USE_ADD_ATTRIBUTE2:
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
										return false;
									}

									if (item2->GetAttributeCount() == 4)
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										// 5. efsun duzenlemesi
										//if ((number(1, 100) <= aiItemAttributeAddPercent[4]))
										if (number(1, 100) <= 100)
										{
#ifdef ENABLE_MULTI_AFFIX_APPLY_SYSTEM
											while (item2->GetAttributeCount() < 5)//tekefsunda 5 efsun gelsin.
												item2->AddAttribute();
#else
											item2->AddAttribute();
#endif
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Upgrade successfully added."));

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
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No upgrade added."));
											LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE2_FAIL", buf);
										}

										item->SetCount(item->GetCount() - 1);
									}
									else if (item2->GetAttributeCount() == 5)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can no longer be improved. The maximum number of bonuses has been reached."));
									}
									else if (item2->GetAttributeCount() < 4)
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only use the Blessing Marble with an item which already has 4 bonuses."));
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
												if (number(1, 100) <= 100)
												{
													item2->SetAccessorySocketMaxGrade(item2->GetAccessorySocketMaxGrade() + 1);
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Socket successfully added."));
													LogManager::instance().ItemLog(this, item, "ADD_SOCKET_SUCCESS", buf);
												}
												else
												{
													ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No socket added."));
													LogManager::instance().ItemLog(this, item, "ADD_SOCKET_FAIL", buf);
												}

												item->SetCount(item->GetCount() - 1);
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No additional sockets could be added to this item."));
											}
										}
										else
										{
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot add a socket to this item."));
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
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Arming successful."));
												LogManager::instance().ItemLog(this, item, "PUT_SOCKET_SUCCESS", buf);
											}
											else
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Arming has failed."));
												LogManager::instance().ItemLog(this, item, "PUT_SOCKET_FAIL", buf);
											}

											item->SetCount(item->GetCount() - 1);
										}
										else
										{
											if (item2->GetAccessorySocketMaxGrade() == 0)
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to add a socket first. Use a diamond in order to do this."));
											else if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
											{
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There are no sockets for gemstones in this item."));
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to add a socket if you want to use a Diamond."));
											}
											else
												ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No more gems can be added to this item."));
										}
									}
									else
									{
										ChatPacket(CHAT_TYPE_INFO, LC_TEXT("These items cannot be used together."));
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

											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Aura> Aura booster has been successfully connected."));

											LogManager::instance().ItemLog(this, item, "PUT_AURA_SOCKET", buf);

											if (IS_SET(item->GetFlag(), ITEM_FLAG_STACKABLE) && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && item->GetCount() > 1)
												item->SetCount(item->GetCount() - 1);
											else
												ITEM_MANAGER::instance().RemoveItem(item, "PUT_AURA_SOCKET_REMOVE");
										}
										else
											ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Aura> Aura boost cannot be added to this item."));
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
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the Bait whilst fishing."));
								return false;
							}

							LPITEM weapon = GetWear(WEAR_WEAPON);

							if (!weapon || weapon->GetType() != ITEM_ROD)
								return false;

							if (weapon->GetSocket(2))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are exchanging the current Bait for %s."), item->GetLocaleName());
							}
							else
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You attached %s to the hook as bait."), item->GetLocaleName());
							}

							weapon->SetSocket(2, item->GetValue(0));
							item->SetCount(item->GetCount() - 1);
						}
						break;

					case USE_MOVE:
					case USE_TREASURE_BOX:
					case USE_MONEYBAG:
						break;

#ifdef ENABLE_TITLE_SYSTEM
					case USE_TITLE:
					{
						int iDuration = item->GetValue(1);
						if (iDuration <= 0)
							iDuration = item->GetValue(2);

						const DWORD dwVnum = item->GetVnum();
						const int iBundle0[5] = { 1000, 1001, 1002, 1003, 1004 };
						const int iBundle1[5] = { 1005, 2000, 2001, 2002, 2003 };
						const int iBundle2[5] = { 3000, 3001, 3002, 3003, 3004 };

						const int* pBundle = NULL;
						int iBundleCount = 0;
						switch (dwVnum)
						{
							case 57000: pBundle = iBundle0; iBundleCount = 5; break;
							case 57001: pBundle = iBundle1; iBundleCount = 5; break;
							case 57002: pBundle = iBundle2; iBundleCount = 5; break;
						}

						if (pBundle && iBundleCount > 0)
						{
							int iUnlockedCount = 0;
							for (int i = 0; i < iBundleCount; ++i)
							{
								const int iTitleID = pBundle[i];
								char szOwned[64];
								char szExpire[64];
								snprintf(szOwned, sizeof(szOwned), "title_system.owned.%d", iTitleID);
								snprintf(szExpire, sizeof(szExpire), "title_system.expire.%d", iTitleID);

								if (GetQuestFlag(szOwned) > 0)
									continue;

								SetQuestFlag(szOwned, 1);
								if (iDuration > 0)
									SetQuestFlag(szExpire, get_global_time() + iDuration);
								else
									SetQuestFlag(szExpire, 0);
								++iUnlockedCount;
							}

							if (iUnlockedCount <= 0)
							{
								ChatPacket(CHAT_TYPE_INFO, "You already own all titles in this package.");
								break;
							}

							item->SetCount(item->GetCount() - 1);
							ChatPacket(CHAT_TYPE_INFO, "Title package unlocked: %d new titles.", iUnlockedCount);
							break;
						}

						int iTitleID = item->GetValue(0);
						if (iTitleID <= 0)
						{
							ChatPacket(CHAT_TYPE_INFO, "Invalid title certificate.");
							break;
						}

						char szOwned[64];
						char szExpire[64];
						snprintf(szOwned, sizeof(szOwned), "title_system.owned.%d", iTitleID);
						snprintf(szExpire, sizeof(szExpire), "title_system.expire.%d", iTitleID);
						if (GetQuestFlag(szOwned) > 0)
						{
							ChatPacket(CHAT_TYPE_INFO, "You already own this title.");
							break;
						}

						const int iActiveTitle = GetQuestFlag("title_system.active");
						if (iActiveTitle > 0 && iActiveTitle != iTitleID)
						{
							ChatPacket(CHAT_TYPE_INFO, "Remove your current title first.");
							break;
						}

						SetQuestFlag(szOwned, 1);
						if (iDuration > 0)
							SetQuestFlag(szExpire, get_global_time() + iDuration);
						else
							SetQuestFlag(szExpire, 0);

						SetQuestFlag("title_system.active", iTitleID);
						const char* c_szTitleEffect = "";
						switch (iTitleID)
						{
							case 1000: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_05_medal.mse"; break;
							case 1001: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_06_banner_gold.mse"; break;
							case 1002: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_07_banner_red.mse"; break;
							case 1003: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_08_banner_blue.mse"; break;
							case 1004: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_02_dragon.mse"; break;
							case 1005: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_01_shield.mse"; break;
							case 2000: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_04_trophy.mse"; break;
							case 2001: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_03_fist.mse"; break;
							case 2002: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_02_dragon.mse"; break;
							case 2003: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_01_shield.mse"; break;
							case 3000: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_06_banner_gold.mse"; break;
							case 3001: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_07_banner_red.mse"; break;
							case 3002: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_08_banner_blue.mse"; break;
							case 3003: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_02_dragon.mse"; break;
							case 3004: c_szTitleEffect = "d:/ymir work/effect/etc/title/title_05_medal.mse"; break;
						}

						if (c_szTitleEffect && *c_szTitleEffect)
							SpecificEffectPacket(c_szTitleEffect);

						ChatPacket(CHAT_TYPE_INFO, "Title unlocked and equipped (ID: %d)", iTitleID);
						item->SetCount(item->GetCount() - 1);
					}
					break;
#endif

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
					case USE_COLLECTION_SCROLL:
					{
						if (!CanHandleItem() || item->IsExchanging())
						{
							return false;
						}
			
						if (GetQuestFlag("collection.percent_up"))
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Zaten yüzdesini artýrdýn."));
							return false;
						}
			
						SetQuestFlag("collection.percent_up", 1);
						ChatPacket(CHAT_TYPE_COMMAND, "RECV_CollectionIncrease %d", 1);
						item->SetCount(item->GetCount() - 1);
					}
					break;
#endif

					case USE_AFFECT:
						{
							if (FindAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType))
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
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
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
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

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
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

			sys_log(0,"ITEM_BLEND!!");
			if (Blend_Item_find(item->GetVnum()))
			{
				int affect_type = AFFECT_BLEND;
				if (item->GetSocket(0) >= (int)_countof(aApplyInfo))
				{
					sys_err ("INVALID BLEND ITEM(id : %d, vnum : %d). APPLY TYPE IS %d.", item->GetID(), item->GetVnum(), item->GetSocket(0));
					return false;
				}

				int		apply_type		= aApplyInfo[item->GetSocket(0)].bPointType;
				int		apply_value		= item->GetSocket(1);
				int		apply_duration	= item->GetSocket(2);

				if (FindAffect(affect_type, apply_type))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
				}
				else
				{
					if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, POINT_RESIST_MAGIC))
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
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

#ifdef ENABLE_NEW_ITEM_TYPE_GACHA
		case ITEM_GACHA:
			{
				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				// int pos = GetEmptyInventory(item->GetSize());
				// if(pos == -1)
				// {
					// ChatPacket(CHAT_TYPE_INFO, LC_TEXT("yer_yok_haci"));
					// return false;
				// }

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					long lOpenCount = item->GetSocket(0);
					WORD wCount = lOpenCount / item->GetLimitValue(1), wRest = (lOpenCount % item->GetLimitValue(1));
					item->SetSocket(0, item->GetSocket(0) - 1);
					if (wRest == 1)
						if (IS_SET(item->GetFlag(), ITEM_FLAG_STACKABLE) && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && item->GetCount() > 1)
							item->SetCount(item->GetCount() - 1);
						else
							ITEM_MANAGER::instance().RemoveItem(item);

					for (int i = 0; i < count; i++)
					{
						switch (dwVnums[i])
						{
						case CSpecialItemGroup::GOLD:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("µ· %lld HATA_VERDIGINDE_LOCALE_EKLE."), static_cast<long long>(dwCounts[i]));
							break;
						case CSpecialItemGroup::EXP:
							break;
						case CSpecialItemGroup::MOB:
							break;
						case CSpecialItemGroup::SLOW:
							break;
						case CSpecialItemGroup::DRAIN_HP:
							break;
						case CSpecialItemGroup::POISON:
							break;
						case CSpecialItemGroup::MOB_GROUP:
							break;
						}
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("lCa«°Íµ?lnR?L?lrlú¨R??."));
					return false;
				}
			}
			break;
#endif

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
	//WORD wDestCell = DestCell.cell;
	//BYTE bDestInven = DestCell.window_type;
	LPITEM item;

	if (!CanHandleItem())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
			return false;

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
			LocaleChatPacket(CHAT_TYPE_INFO, 431, "");
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
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item because you do not fulfil all the requirements."));
		return false;
	}

	if (IsStun())
		return false;

	if (false == FN_check_item_sex(this, item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are not able to use that item because you do not have the right gender."));
		return false;
	}

	if (IS_SUMMON_ITEM(item->GetVnum()))
	{
		if (false == IS_SUMMONABLE_ZONE(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This function is not available right now."));
			return false;
		}

		int iPulse = thecore_pulse();
		if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After opening the Storeroom you cannot use a Scroll of the Location for %d seconds."), g_nPortalLimitTime);
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
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use a Scroll of the Location while another window is open."));
			return false;
		}

		{
			if (iPulse - GetRefineTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After a trade, you cannot use a scroll for another %d seconds."), g_nPortalLimitTime);
				return false;
			}
		}

		{
			if (iPulse - GetMyShopTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After opening a storeroom you cannot use a Scroll of the Location for another %d seconds."), g_nPortalLimitTime);
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
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use the Scroll of the Location because the distance is too small."));
				return false;
			}
		}

		if (iPulse - GetExchangeTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After a trade you cannot use a Scroll of the Location for %d seconds."), g_nPortalLimitTime);
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
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open the Storeroom if another window is already open."));
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
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
			return false;
		}
	}
#endif

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item if you're using quests"));
		return false;
	}
	// @fixme150 END

#ifdef ENABLE_PASSIVE_SYSTEM
	if (item->GetVnum() == 100100 || item->GetVnum() == 100101)
	{
		LPITEM pkRelic = GetWear(WEAR_PASSIVE);
		if (!pkRelic)
		{
			ChatPacket(CHAT_TYPE_INFO, "Equip a relic first.");
			return false;
		}

		const int iSuccessChance = (item->GetVnum() == 100101) ? 100 : 30;
		const int iEmptyPos = GetEmptyInventory(pkRelic->GetSize());
		if (iEmptyPos < 0)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 431, "");
			return false;
		}

		if (item->GetCount() > 0)
			item->SetCount(item->GetCount() - 1);

		if (pkRelic->GetSocket(1) != 0)
		{
			pkRelic->ModifyPoints(false);
			pkRelic->SetSocket(1, 0);
			RemoveAffect(AFFECT_PASSIVE_RELIC_STONE_DEF);
			RemoveAffect(AFFECT_PASSIVE_RELIC_DISMOUNT_SPEED);
			ComputeBattlePoints();
			UpdatePacket();
		}

		if (number(1, 100) > iSuccessChance)
		{
			pkRelic->RemoveFromCharacter();
			pkRelic->SetCount(0);
			ChatPacket(CHAT_TYPE_INFO, "Relic extraction failed. The relic was destroyed.");
			return true;
		}

		pkRelic->RemoveFromCharacter();
		pkRelic->AddToCharacter(this, TItemPos(INVENTORY, iEmptyPos));
		ChatPacket(CHAT_TYPE_INFO, "Relic extraction succeeded.");
		return true;
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

		if (NULL == ITEM_MANAGER::instance().FindByVID(vid))
		{
			LogManager::instance().ItemLog(this, vid, vnum, "REMOVE", hint);
		}
		else if (oldCount != item->GetCount())
		{
			snprintf(hint + len, sizeof(hint) - len, " %u", oldCount - 1);
			LogManager::instance().ItemLog(this, vid, vnum, "USE_ITEM", hint);
		}

		return ret;
	}
	else
	{
		bool ret = UseItemEx(item, DestCell);
		return ret;
	}
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
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));
		return false;
	}

	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

#ifdef ENABLE_SEALBIND_SYSTEM
	if (item->IsSealed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ruh bagi ile bagli esyayi yere atamazsin."));
		return false;
	}
#endif

	if (true == item->isLocked())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP | ITEM_ANTIFLAG_GIVE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot drop this item."));
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

		// copy item socket -- by mhh
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
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The dropped item will vanish in 5 minutes."));
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
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));
		return false;
	}

	if (IsDead())
		return false;

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;

	if (item->IsExchanging())
		return false;

#ifdef ENABLE_SEALBIND_SYSTEM
	if (item->IsSealed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Ruh bagi ile bagli esyayi silemezsin."));
		return false;
	}
#endif

	if (true == item->isLocked())
		return false;

	if(IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DESTROY))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 442, "");
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

	// FIX(UAF): item linki RemoveItem'dan once olusturulur
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

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);
	ITEM_MANAGER::instance().RemoveItem(item);

	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have deleted %s."), buf);

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
		LocaleChatPacket(CHAT_TYPE_INFO, 1956, "");
		return false;
	}

	// FIX(UAF): SetCount(0) item'i yok eder; link once olusturulur
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

	item->SetCount(item->GetCount() - item->GetCount());
	PointChange(POINT_GOLD, dwPrice, false);
#ifndef ENABLE_RANKING
	SetRankPoints(10, GetRankPoints(10) + dwPrice);
#endif

	LocaleChatPacket(CHAT_TYPE_INFO, 510, "%s", buf);

	return true;
}
#endif

bool CHARACTER::DropGold(int gold)
{
#ifdef ENABLE_DROP_GOLD
	if (gold <= 0 || gold > GetGold())
		return false;

	if (!CanHandleItem())
		return false;

	if (0 != g_GoldDropTimeLimitValue)
	{
		if (get_dword_time() < m_dwLastGoldDropTime + g_GoldDropTimeLimitValue)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot throw the Gold away yet."));
			return false;
		}
	}

	m_dwLastGoldDropTime = get_dword_time();

	LPITEM item = ITEM_MANAGER::instance().CreateItem(1, gold);

	if (item)
	{
		const PIXEL_POSITION pos = GetXYZ();

		if (item->AddToGround(GetMapIndex(), pos))
		{
			//Motion(MOTION_PICKUP);
			PointChange(POINT_GOLD, -gold, true);

			if (gold > DROPABLE_GOLD_LIMIT) // Records over 1,000 won.
				LogManager::instance().CharLog(this, gold, "DROP_GOLD", "");

			item->StartDestroyEvent();
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The dropped item will disappear after %d minutes."), 150/60);
		}

		Save();
		return true;
	}
#else
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't drop gold."));
#endif

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
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));

#ifdef ENABLE_AURA_COSTUME_SYSTEM
		if (IsAuraRefineWindowOpen())
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));
#endif

		return false;
	}

	// At the request of the planner, only certain types of items can be placed in the belt inventory.
	if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip this item in your belt inventory."));
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
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you want to change weapons, you must remove the weapon skin first."));
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

	// Güvenlik: SwitchBot'tan Special Inventory'ye transfer engeli (Item Duplication önleme)
#ifdef ENABLE_SPECIAL_INVENTORY
	if (Cell.IsSwitchbotPosition())
	{
		if (DestCell.IsSkillBookInventoryPosition() ||
			DestCell.IsUpgradeItemsInventoryPosition() ||
			DestCell.IsStoneInventoryPosition() ||
			DestCell.IsGiftBoxInventoryPosition() ||
			DestCell.IsChangersInventoryPosition())
		{
			sys_err("[SECURITY] Player %s (PID:%u) attempted to transfer item from SwitchBot to Special Inventory (Cell:%u, DestCell:%u, ItemID:%u). Item duplication prevented.",
				GetName(), GetPlayerID(), Cell.cell, DestCell.cell, item->GetID());
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
			return false;
		}
	}

	if (DestCell.IsSwitchbotPosition())
	{
		if (Cell.IsSkillBookInventoryPosition() ||
			Cell.IsUpgradeItemsInventoryPosition() ||
			Cell.IsStoneInventoryPosition() ||
			Cell.IsGiftBoxInventoryPosition() ||
			Cell.IsChangersInventoryPosition())
		{
			sys_err("[SECURITY] Player %s (PID:%u) attempted to transfer item from Special Inventory to SwitchBot (Cell:%u, DestCell:%u, ItemID:%u). Item duplication prevented.",
				GetName(), GetPlayerID(), Cell.cell, DestCell.cell, item->GetID());
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
			return false;
		}
	}
#endif
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	if (DestCell.IsSkillBookInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't move this item into this inventory."));
		return false;
	}

	if (DestCell.IsUpgradeItemsInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't move this item into this inventory."));
		return false;
	}

	if (DestCell.IsStoneInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't move this item into this inventory."));
		return false;
	}

	if (DestCell.IsGiftBoxInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't move this item into this inventory."));
		return false;
	}

	if (DestCell.IsChangersInventoryPosition() && (item->IsEquipped() || Cell.IsBeltInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't move this item into this inventory."));
		return false;
	}

	if ((Cell.IsSkillBookInventoryPosition() && !DestCell.IsSkillBookInventoryPosition() && !DestCell.IsDefaultInventoryPosition()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't move this item into this inventory."));
		return false;
	}

	if (Cell.IsUpgradeItemsInventoryPosition() && !DestCell.IsUpgradeItemsInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't move this item into this inventory."));
		return false;
	}

	if (Cell.IsStoneInventoryPosition() && !DestCell.IsStoneInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't move this item into this inventory."));
		return false;
	}

	if (Cell.IsGiftBoxInventoryPosition() && !DestCell.IsGiftBoxInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't move this item into this inventory."));
		return false;
	}

	if (Cell.IsChangersInventoryPosition() && !DestCell.IsChangersInventoryPosition() && !DestCell.IsDefaultInventoryPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't move this item into this inventory."));
		return false;
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsSkillBookInventoryPosition())
	{
		if (!item->IsSkillBook())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only move skill books into this inventory."));
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsUpgradeItemsInventoryPosition())
	{
		if (!item->IsUpgradeItem())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only move upgrade items into this inventory."));
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsStoneInventoryPosition())
	{
		if (!item->IsStone())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only move stones into this inventory."));
			return false;
		}
	}

	if (Cell.IsDefaultInventoryPosition() && DestCell.IsGiftBoxInventoryPosition())
	{
		if (!item->IsGiftBox())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only move chests into this inventory."));
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
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have already equipped this kind of Dragon Stone."));
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

		if ((item2 = GetItem(DestCell)) && item != item2 && item2->IsStackable() &&
				!IS_SET(item2->GetAntiFlag(), ITEM_ANTIFLAG_STACK) &&
				item2->GetVnum() == item->GetVnum())
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

			if (INVENTORY == Cell.window_type && INVENTORY == DestCell.window_type)
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, DestCell.cell);
		}
		else if (count < item->GetCount())
		{
			sys_log(0, "%s: ITEM_SPLIT %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell, 
				DestCell.window_type, DestCell.cell, count);

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

#ifdef ENABLE_GAYA_SYSTEM
	struct FGemDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		int		iGem;

		FGemDistributor(LPCHARACTER center, int iGem)
			: total(0), c(center), x(center->GetX()), y(center->GetY()), iGem(iGem)
		{
		}

		void operator ()(LPCHARACTER ch)
		{
			if (ch != c)
				if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange(POINT_GEM, iGem, true);

					if (iGem > 1000)
						LogManager::instance().CharLog(ch, iGem, "GET_GEM", "");
				}
		}
	};
#endif
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
		if (!pParty)
			return;

#ifdef ENABLE_GOLD_LIMIT
		long long dwTotal = iAmount;
		long long dwMyAmount = dwTotal;
#else
		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;
#endif

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnMapMember(funcCountNearMember, GetMapIndex());	//@fixme522

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FMoneyDistributor funcMoneyDist(this, dwShare);
			pParty->ForEachOnMapMember(funcMoneyDist, GetMapIndex());	//@fixme522
		}

		PointChange(POINT_GOLD, dwMyAmount, true);

		if (dwMyAmount > 1000) // Records over 1,000 won.
			LogManager::instance().CharLog(this, dwMyAmount, "GET_GOLD", "");
	}
	else
	{
		PointChange(POINT_GOLD, iAmount, true);

		if (iAmount > 1000)
			LogManager::instance().CharLog(this, iAmount, "GET_GOLD", "");
	}
}

#ifdef ENABLE_GAYA_SYSTEM
void CHARACTER::GiveGem(int iAmount)
{
	if (iAmount <= 0)
		return;

	sys_log(0, "GIVE_GEM: %s %lld", GetName(), iAmount);

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FGemDistributor funcGemDist(this, dwShare);

			pParty->ForEachOnlineMember(funcGemDist);
		}

		PointChange(POINT_GEM, dwMyAmount, true);

		if (dwMyAmount > 1000)
			LogManager::instance().CharLog(this, dwMyAmount, "GET_GEM", "");
	}
	else
	{
		PointChange(POINT_GEM, iAmount, true);

		if (LC_IsBrazil() == true)
		{
			if (iAmount >= 213)
				LogManager::instance().CharLog(this, iAmount, "GET_GEM", "");
		}
		else
		{
			if (iAmount > 1000)
				LogManager::instance().CharLog(this, iAmount, "GET_GEM", "");
		}
	}
}
#endif

bool CHARACTER::PickupItem(DWORD dwVID)
{
	if (IsPC() && IsDead())// @fixme243
		return false;

	LPITEM item = ITEM_MANAGER::instance().FindByVID(dwVID);

	if (IsObserverMode())
		return false;

	if (!item || !item->GetSectree())
		return false;

	if (item->DistanceValid(this))
	{
		// @fixme150 BEGIN
		if (item->GetType() == ITEM_QUEST)
		{
			if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot pickup this item if you're using quests"));
				return false;
			}
		}
		// @fixme150 END
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
#ifndef ENABLE_RANKING
				SetRankPoints(10, GetRankPoints(10) + item->GetCount());
#endif

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
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item2->GetLocaleName());
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
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item2->GetLocaleName());
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
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item2->GetLocaleName());
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
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your inventory."));
						return false;
					}
				}
#ifdef ENABLE_SPECIAL_INVENTORY
				else if (item->IsSkillBook())
				{
					if ((iEmptyCell = GetEmptySkillBookInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your special inventory."));
						return false;
					}
				}
				else if (item->IsUpgradeItem())
				{
					if ((iEmptyCell = GetEmptyUpgradeItemsInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your special inventory."));
						return false;
					}
				}
				else if (item->IsStone())
				{
					if ((iEmptyCell = GetEmptyStoneInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your special inventory."));
						return false;
					}
				}
				else if (item->IsGiftBox())
				{
					if ((iEmptyCell = GetEmptyGiftBoxInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your special inventory."));
						return false;
					}
				}
				else if (item->IsChanger())
				{
					if ((iEmptyCell = GetEmptyChangersInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your special inventory."));
						return false;
					}
				}
#endif
				else
				{
					if ((iEmptyCell = GetEmptyInventory(item->GetSize())) == -1)
					{
						sys_log(0, "No empty inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your inventory."));
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

				char szHint[32 + 1];
				snprintf(szHint, sizeof(szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
				LogManager::instance().ItemLog(this, item, "GET", szHint);
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
				SendPickupItemPacket(item->GetVnum());
#else
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetLocaleName());
#endif

				if (item->GetType() == ITEM_QUEST)
					quest::CQuestManager::instance().PickupItem (GetPlayerID(), item);
			}

			//Motion(MOTION_PICKUP);
			return true;
		}
		else if (!IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_DROP) && GetParty())
		{
			// If you want to give another party member ownership item
			NPartyPickupDistribute::FFindOwnership funcFindOwnership(item);
			GetParty()->ForEachOnMapMember(funcFindOwnership, GetMapIndex());	//@fixme522

			LPCHARACTER owner = funcFindOwnership.owner;
			// @fixme115
			if (!owner)
				return false;
			int iEmptyCell;

			if (item->IsDragonSoul())
			{
				if (!(owner && (iEmptyCell = owner->GetEmptyDragonSoulInventory(item)) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyDragonSoulInventory(item)) == -1)
					{
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your inventory."));
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
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your book inventory."));
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
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your upgrade inventory."));
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
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your stone inventory."));
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
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your gift inventory."));
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
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your inventory."));
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
						owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your inventory."));
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
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetLocaleName());
#endif
			else
			{
				owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s receives %s."), GetName(), item->GetLocaleName());
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Item Trade: %s, %s"), owner->GetName(), item->GetLocaleName());
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

	// Check for correct cell
	// Dragon Spirit Stone cannot be swapped, so it gets stuck here.
	//if (wCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM || wDestCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM)
	if (srcCell.IsDragonSoulEquipPosition() || destCell.IsDragonSoulEquipPosition())
		return false;

	if (bCell == bDestCell)
		return false;

	// If both are in the equipment window, you cannot swap.
	if (srcCell.IsEquipPosition() && destCell.IsEquipPosition())
		return false;

	LPITEM item1, item2;

	// so that item2 is in the equipment window.
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

	// Check if item2 can enter the wCell location.
	if (!IsEmptyItemGrid(TItemPos (INVENTORY, item1->GetCell()), item2->GetSize(), item1->GetCell()))
	{
#ifdef ENABLE_FIX_SWAP_ITEM
		if (TItemPos(EQUIPMENT, item2->GetCell()).IsEquipPosition())
		{
			int iEquipCell = item2->GetCell() - INVENTORY_MAX_NUM;
			if (UnequipItem(item2))
				return EquipItem(item1, iEquipCell);
		}
		return false;
#else
		return false;
#endif
    }

	// If the item to be changed is in the equipment window
	if (TItemPos(EQUIPMENT, item2->GetCell()).IsEquipPosition())
	{
		BYTE bEquipCell = item2->GetCell() - INVENTORY_MAX_NUM;
		BYTE bInvenCell = item1->GetCell();

		// The item being worn can be removed, and the item to be worn must be in a wearable state.
		if (false == CanUnequipNow(item2) || false == CanEquipNow(item1))
			return false;

		if (bEquipCell != item1->FindEquipCell(this)) // Allowed only in the same location
			return false;

#ifdef ENABLE_MOUNT_SYSTEM
		if(ITEM_MOUNT == item2->GetType())
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item2, false);
#endif

#ifdef ENABLE_PET_SYSTEM
		if(ITEM_PET == item2->GetType())
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
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you want to change weapons, you must remove the weapon skin first."));
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
#ifdef ENABLE_BUFFI_SYSTEM
	// Companion panel: Buffi seal / Buffi costumes are checked against the Buffi, not the player
	if (CBuffiManager::IsBuffiWearCell(iCandidateCell))
		return CBuffiManager::instance().EquipToBuffi(this, item, iCandidateCell);
#endif
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
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use it whilst riding a Horse."));
		return false;
	}

	if (iWearCell != WEAR_ARROW && IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the equipped item while you are transformed."));
		return false;
	}

	if (FN_check_item_sex(this, item) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are not able to use that item because you do not have the right gender."));
		return false;
	}

	if(item->IsRideItem() && IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You're already riding. Get off first."));
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
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to stand still to equip the item."));
		return false;
	}

#ifdef __FIX_COSTUM_NUNTA_PESTE_COSTUM_NORMAL__
	if (GetWear(WEAR_BODY) && GetWear(WEAR_BODY)->GetVnum() >= 11901 && GetWear(WEAR_BODY)->GetVnum() <= 11904 && 
		item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_BODY)
    {
        ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot wear a costume as long as you have equipped a wedding costume"));
        return false;
    }
   
    if (GetWear(WEAR_COSTUME_BODY) && item->GetVnum() >= 11901 && item->GetVnum() <= 11904)
    {
        ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip a wedding costume as long as you have a costume on"));
        return false;
    }
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (iWearCell == WEAR_WEAPON)
	{
		if (item->GetType() == ITEM_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && costumeWeapon->GetValue(3) != item->GetSubType() && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you want to change weapons, you must remove the weapon skin first."));
				return false;
			}
		}
		else //fishrod/pickaxe
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you want to change weapons, you must remove the weapon skin first."));
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
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this costume for this weapon."));
				return false;
			}
		}
	}
#endif

	if (item->IsDragonSoul())
	{
		if(GetInventoryItem(INVENTORY_MAX_NUM + iWearCell))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are already carrying a Dragon Stone of this type."));
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
#ifdef ENABLE_PASSIVE_SYSTEM
		else if (true == CItemVnumHelper::IsPassive(dwVnum))
		{
			this->EffectPacket(SE_PASSIVE_EFFECT);
			this->SpecificEffectPacket("d:/ymir work/effect/etc/buff/buff_passive_01.mse");
		}
#endif
#ifdef ENABLE_EQUIPMENT_HAND_EFFECT
		else if (item->GetVnum() == 72703)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_kaplan_kemigi_kupe_1.mse");
		else if (item->GetVnum() == 727003)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_kaplan_kemigi_kupe_1.mse");
		else if (item->GetVnum() == 727005)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_kaplan_kemigi_kupe_2.mse");
		else if (item->GetVnum() == 727007)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_kaplan_kemigi_kupe_3.mse");
		else if (item->GetVnum() == 72704)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_ejder_kemigi_kupe_1.mse");
		else if (item->GetVnum() == 727004)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_ejder_kemigi_kupe_1.mse");
		else if (item->GetVnum() == 727006)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_ejder_kemigi_kupe_2.mse");
		else if (item->GetVnum() == 727008)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_ejder_kemigi_kupe_3.mse");
		else if (item->GetVnum() == 727009)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_kaplan_kemik_bileklik_1.mse");
		else if (item->GetVnum() == 72705)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_kaplan_kemik_bileklik_1.mse");
		else if (item->GetVnum() == 727015)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_kaplan_kemik_bileklik_1.mse");
		else if (item->GetVnum() == 727011)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_kaplan_kemik_bileklik_2.mse");
		else if (item->GetVnum() == 727017)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_kaplan_kemik_bileklik_2.mse");
		else if (item->GetVnum() == 727013)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_kaplan_kemik_bileklik_3.mse");
		else if (item->GetVnum() == 727019)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_kaplan_kemik_bileklik_3.mse");
		else if (item->GetVnum() == 727010)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_ejder_kemik_bilezik_1.mse");
		else if (item->GetVnum() == 72706)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_ejder_kemik_bilezik_1.mse");
		else if (item->GetVnum() == 727016)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_ejder_kemik_bilezik_1.mse");
		else if (item->GetVnum() == 727012)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_ejder_kemik_bilezik_2.mse");
		else if (item->GetVnum() == 727018)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_ejder_kemik_bilezik_2.mse");
		else if (item->GetVnum() == 727014)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_ejder_kemik_bilezik_3.mse");
		else if (item->GetVnum() == 727020)
			SpecificEffectPacket("d:/ymir work/effect/etc/buff/kemikeff/buff_ejder_kemik_bilezik_3.mse");
#endif
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
			if ((m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID())) || item->IsExchanging()) // SECURITY: trade'deki item sayilmaz
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
			if ((m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID())) || item->IsExchanging()) // SECURITY: trade'deki item sayilmaz
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

		// SECURITY: trade'deki item silinmez (CountSpecifyItem ile tutarli)
		if (GetInventoryItem(i)->IsExchanging())
			continue;

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

		// SECURITY: trade'deki item silinmez (CountSpecifyItem ile tutarli)
		if (GetInventoryItem(i)->IsExchanging())
			continue;

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
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetLocaleName());
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
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetLocaleName());
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
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetLocaleName());
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
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetLocaleName());
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
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetLocaleName());
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
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetLocaleName());
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
		for (int i=0; i < INVENTORY_AND_EQUIP_SLOT_MAX; i++)
		{
			LPITEM inv_item = GetInventoryItem(i);

			if (inv_item == NULL) continue;

			if (inv_item->GetType() == ITEM_BLEND)
			{
				if (inv_item->GetVnum() == item->GetVnum())
				{
					if (inv_item->GetSocket(0) == item->GetSocket(0) &&
						inv_item->GetSocket(1) == item->GetSocket(1) &&
						inv_item->GetSocket(2) == item->GetSocket(2) &&
						inv_item->GetCount() < ITEM_MAX_COUNT)
					{
						inv_item->SetCount(inv_item->GetCount() + item->GetCount());
						M2_DESTROY_ITEM(item);
						return inv_item;
					}
				}
			}
		}
	}

	int iEmptyCell;
	if (item->IsDragonSoul())
	{
		iEmptyCell = GetEmptyDragonSoulInventory(item);
	}
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
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetLocaleName());
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
	}

	sys_log(0, 
		"7: %d %d", dwItemVnum, bCount);
	return item;
}

bool CHARACTER::GiveItem(LPCHARACTER victim, TItemPos Cell)
{
	if (!CanHandleItem())
		return false;

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot take this item if you're using quests"));
		return false;
	}
	// @fixme150 END

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

	// TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX
	if (DISTANCE_APPROX(GetX() - from->GetX(), GetY() - from->GetY()) > 2000)
		return false;
	// END_OF_TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX

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

			// BUILDING_NPC
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
			// END_OF_BUILDING_NPC

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
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Monkey Herbs cannot be fed to living horses. It is used to revive dead horses."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a dead Horse."));
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
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Monkey Herbs cannot be fed to living horses. It is used to revive dead horses."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a dead Horse."));
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
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Monkey Herbs cannot be fed to living horses. It is used to revive dead horses."));
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				if (IsDead())
				{
					from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a dead Horse."));
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
				// TAKE_ITEM_BUG_FIX
				from->SetQuestNPCID(GetVID());
				// END_OF_TAKE_ITEM_BUG_FIX
				quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
			}
			break;

#ifdef ENABLE_QUEEN_NETHIS
	case SnakeLair::PILAR_STEP_4:
	{
		if (from->IsPC())
		{
			if (SnakeLair::CSnk::instance().IsSnakeMap(from->GetMapIndex()))
				if (item || from || this)
					SnakeLair::CSnk::instance().OnKillPilar(item, from, this);
		}
	}
	break;

	case SnakeLair::BLACKSMITH_5:
	{
		if (from->IsPC())
		{
			if (SnakeLair::CSnk::instance().IsSnakeMap(from->GetMapIndex()))
				if (item || from || this)
					SnakeLair::CSnk::instance().OnKillBlackSmith(item, from, this);
		}
	}
	break;

	case SnakeLair::SNAKE_STATUE1:
	case SnakeLair::SNAKE_STATUE2:
	case SnakeLair::SNAKE_STATUE3:
	case SnakeLair::SNAKE_STATUE4:
	{
		if (from->IsPC())
		{
			if (SnakeLair::CSnk::instance().IsSnakeMap(from->GetMapIndex()))
				if (item || from || this)
					SnakeLair::CSnk::instance().OnStatueSetRotation(item, from, this);
		}
	}
	break;

	case SnakeLair::VNUM_BLACKSMITH_SERPENT:
	{
#ifdef ENABLE_GLOVE_SYSTEM
		if (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_GLOVE)
		{
			from->SetRefineNPC(this);
			from->RefineInformation(item->GetCell(), REFINE_TYPE_MONEY_ONLY);
		}
		else
#endif
		{
			from->ChatPacket(CHAT_TYPE_INFO, "[LS;1002]");
		}
	}
	break;
#endif

		// DEVILTOWER_NPC
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
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
			}
			break;
			// END_OF_DEVILTOWER_NPC

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
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
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
			if (item->GetVnum() == ITEM_REVIVE_HORSE_1 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_2 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				from->ReviveHorse();
				item->SetCount(item->GetCount()-1);
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You fed the Horse with Herbs."));
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 ||
					item->GetVnum() == ITEM_HORSE_FOOD_2 ||
					item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				from->FeedHorse();
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have fed the Horse."));
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
// END_OF_CHECK_UNIQUE_GROUP

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
		//sys_err("cannot find special item group %d", dwGroupNum);
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

// NEW_HAIR_STYLE_ADD
bool CHARACTER::ItemProcess_Hair(LPITEM item, int iDestCell)
{
	if (item->CheckItemUseLevel(GetLevel()) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your level is too low to wear this Hairstyle."));
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
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already have this Hairstyle."));
		return true;
	}

	item->SetCount(item->GetCount() - 1);

	SetPart(PART_HAIR, hair);
	UpdatePacket();

	return true;
}

// END_NEW_HAIR_STYLE_ADD

bool CHARACTER::ItemProcess_Polymorph(LPITEM item)
{
	if (IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have already transformed."));
		return false;
	}

	if (true == IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot transform while you are riding."));
		return false;
	}

	DWORD dwVnum = item->GetSocket(0);

	if (dwVnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That's the wrong trading item."));
		item->SetCount(item->GetCount()-1);
		return false;
	}

	const CMob* pMob = CMobManager::instance().Get(dwVnum);

	if (pMob == NULL)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That's the wrong trading item."));
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
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot transform into a monster who has a higher level than you."));
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
				else
				{
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

#ifdef __DOJANG_SRC_FUNCTIONS__
	if (type == AFFECT_AUTO_HP_RECOVERY && GetMapIndex() == DOJANG_MAPINDEX)
		return;
#endif
#ifdef ENABLE_SUNG_MAHI_TOWER
	bool isDungeonMap = (GetMapIndex() >= 3540000 && GetMapIndex() < 3550000);
	if (isDungeonMap)
		return;
#endif

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
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your level is too low to equip this item."));
					return false;
				}
				break;

			case LIMIT_STR:
				if (GetPoint(POINT_ST) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are not strong enough to equip yourself with this item."));
					return false;
				}
				break;

			case LIMIT_INT:
				if (GetPoint(POINT_IQ) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your intelligence is too low to equip yourself with this item."));
					return false;
				}
				break;

			case LIMIT_DEX:
				if (GetPoint(POINT_DX) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your dexterity is too low to equip yourself with this item."));
					return false;
				}
				break;

			case LIMIT_CON:
				if (GetPoint(POINT_HT) < limit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your vitality is too low to equip yourself with this item."));
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
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip this item twice."));
			return false;
		}

		if (marriage::CManager::instance().IsMarriageUniqueItem(item->GetVnum()) &&
			!marriage::CManager::instance().IsMarried(GetPlayerID()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item because you are not married."));
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
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("iki kez bu objeyi takamazsin!"));
					return false;
				}
			}
		}
	}

	return true;
}

bool CHARACTER::CanUnequipNow(const LPITEM item, const TItemPos & srcCell, const TItemPos & destCell) /*const*/
{
#ifdef ENABLE_TELEPORT_SKILL
	if (item->GetType() == ITEM_WEAPON)
	{
		if (IsAffectFlag(AFF_GWIGUM))
			RemoveAffect(SKILL_GWIGEOM);

		if (IsAffectFlag(AFF_GEOMGYEONG))
			RemoveAffect(SKILL_GEOMKYUNG);
	}
#endif

#ifdef DEFAULT_BELT_SYSTEM
	if (ITEM_ARMOR == item->GetType() && ARMOR_BELT == item->GetSubType())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 1095, "");
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
			LocaleChatPacket(CHAT_TYPE_INFO, 431, "");
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
