#include "stdafx.h"
#include "questmanager.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "log.h"
#include "refine.h"
#include "db.h"

#undef sys_err
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)

namespace quest
{
	int item_get_cell(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (q.GetCurrentItem())
		{
			lua_pushnumber(L, q.GetCurrentItem()->GetCell());
		}
		else
			lua_pushnumber(L, 0);
		return 1;
	}

	int item_select_cell(lua_State* L)
	{
		lua_pushboolean(L, 0);
		if (!lua_isnumber(L, 1))
		{
			return 1;
		}
		DWORD cell = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPITEM item = ch ? ch->GetInventoryItem(cell) : NULL;

		if (!item)
		{
			return 1;
		}

		CQuestManager::instance().SetCurrentItem(item);
		lua_pushboolean(L, 1);

		return 1;
	}

	int item_select(lua_State* L)
	{
		lua_pushboolean(L, 0);
		if (!lua_isnumber(L, 1))
		{
			return 1;
		}
		DWORD id = (DWORD) lua_tonumber(L, 1);
		LPITEM item = ITEM_MANAGER::instance().Find(id);

		if (!item)
		{
			return 1;
		}

		CQuestManager::instance().SetCurrentItem(item);
		lua_pushboolean(L, 1);

		return 1;
	}

	int item_get_id(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (q.GetCurrentItem())
		{
			lua_pushnumber(L, q.GetCurrentItem()->GetID());
		}
		else
			lua_pushnumber(L, 0);
		return 1;
	}

	int item_remove(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();
		if (item != NULL) {
			if (q.GetCurrentCharacterPtr() == item->GetOwner()) {
				ITEM_MANAGER::instance().RemoveItem(item);
			} else {
				sys_err("Tried to remove invalid item %p", get_pointer(item));
			}
			q.ClearCurrentItem();
		}

		return 0;
	}

	int item_get_socket(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		if (q.GetCurrentItem() && lua_isnumber(L, 1))
		{
			int idx = (int) lua_tonumber(L, 1);
			if (idx < 0 || idx >= ITEM_SOCKET_MAX_NUM)
				lua_pushnumber(L,0);
			else
				lua_pushnumber(L, q.GetCurrentItem()->GetSocket(idx));
		}
		else
		{
			lua_pushnumber(L,0);
		}
		return 1;
	}

	int item_set_socket(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		if (q.GetCurrentItem() && lua_isnumber(L,1) && lua_isnumber(L,2))
		{
			int idx = (int) lua_tonumber(L, 1);
			int value = (int) lua_tonumber(L, 2);
			if (idx >=0 && idx < ITEM_SOCKET_MAX_NUM)
				q.GetCurrentItem()->SetSocket(idx, value);
		}
		return 0;
	}

	int item_get_vnum(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item) 
			lua_pushnumber(L, item->GetVnum());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_has_flag(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (!lua_isnumber(L, 1))
		{
			sys_err("flag is not a number.");
			lua_pushboolean(L, 0);
			return 1;
		}

		if (!item)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		long lCheckFlag = (long) lua_tonumber(L, 1);	
		lua_pushboolean(L, IS_SET(item->GetFlag(), lCheckFlag));

		return 1;
	}

	int item_get_value(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (!item)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		if (!lua_isnumber(L, 1))
		{
			sys_err("index is not a number");
			lua_pushnumber(L, 0);
			return 1;
		}

		int index = (int) lua_tonumber(L, 1);

		if (index < 0 || index >= ITEM_VALUES_MAX_NUM)
		{
			sys_err("index(%d) is out of range (0..%d)", index, ITEM_VALUES_MAX_NUM);
			lua_pushnumber(L, 0);
		}
		else
			lua_pushnumber(L, item->GetValue(index));

		return 1;
	}

	int item_set_value(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (!item)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		if (false == (lua_isnumber(L, 1) && lua_isnumber(L, 2) && lua_isnumber(L, 3)))
		{
			sys_err("index is not a number");
			lua_pushnumber(L, 0);
			return 1;
		}

		item->SetForceAttribute(
			lua_tonumber(L, 1),		// index
			lua_tonumber(L, 2),		// apply type
			lua_tonumber(L, 3)		// apply value
		);

		return 0;
	}

	int item_get_name(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
		{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
			BYTE locale = ch ? ch->GetLanguage() : LOCALE_DEFAULT;
			lua_pushstring(L, item->GetLocaleName(locale));
#else
			lua_pushstring(L, item->GetName());
#endif
		}
		else
			lua_pushstring(L, "");

		return 1;
	}

	int item_get_size(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetSize());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_get_count(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetCount());
		else
			lua_pushnumber(L, 0);

		return 1;
	}
	
#ifdef ENABLE_QUEST_ITEM_FUNCTIONS
	int item_set_count(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();
		if (!item || false == lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}
		const int count = (int)lua_tonumber(L, 1);
		item->SetCount(count);
		if(count == 0)
			q.ClearCurrentItem();
		return 0;
	}
#endif

	int item_get_type(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetType());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_get_sub_type(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetSubType());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_get_refine_vnum(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetRefinedVnum());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_next_refine_vnum(lua_State* L)
	{
		DWORD vnum = 0;
		if (lua_isnumber(L, 1))
			vnum = (DWORD) lua_tonumber(L, 1);

		TItemTable* pTable = ITEM_MANAGER::instance().GetTable(vnum);
		if (pTable)
		{
			lua_pushnumber(L, pTable->dwRefinedVnum);
		}
		else
		{
			sys_err("Cannot find item table of vnum %u", vnum);
			lua_pushnumber(L, 0);
		}
		return 1;
	}

	int item_get_level(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (item)
			lua_pushnumber(L, item->GetRefineLevel());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int item_get_level_limit (lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (q.GetCurrentItem())
		{
			if (q.GetCurrentItem()->GetType() != ITEM_WEAPON && q.GetCurrentItem()->GetType() != ITEM_ARMOR)
			{
				return 0;
			}
			lua_pushnumber(L, q.GetCurrentItem() -> GetLevelLimit());
			return 1;
		}
		return 0;
	}

	int item_start_realtime_expire(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM pItem = q.GetCurrentItem();

		if (pItem)
		{
			pItem->StartRealTimeExpireEvent();
			return 1;
		}

		return 0;
	}
	
	int item_copy_and_give_before_remove(lua_State* L)
	{
		lua_pushboolean(L, 0);
		if (!lua_isnumber(L, 1))
			return 1;

		DWORD vnum = (DWORD)lua_tonumber(L, 1);

		CQuestManager& q = CQuestManager::instance();
		LPITEM pItem = q.GetCurrentItem();
		LPCHARACTER pChar = q.GetCurrentCharacterPtr();

		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(pItem, pkNewItem);
			LogManager::instance().ItemLog(pChar, pkNewItem, "COPY SUCCESS", pkNewItem->GetName());

			BYTE bCell = pItem->GetCell();

			ITEM_MANAGER::instance().RemoveItem(pItem, "REMOVE (COPY SUCCESS)");

			pkNewItem->AddToCharacter(pChar, TItemPos(INVENTORY, bCell)); 
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			pkNewItem->AttrLog();

			lua_pushboolean(L, 1);
		}

		return 1;
	}

	int item_is_available(lua_State* L)
	{
		auto item = CQuestManager::instance().GetCurrentItem();
		lua_pushboolean(L, item != nullptr);
		return 1;
	}

#ifdef ENABLE_QUEST_ITEM_FUNCTIONS
	int item_equip_selected(lua_State* L)
	{
		//current character who use
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		BYTE bCell = 0;
		if (!lua_isnumber(L, 1))
		{
			sys_log(0, "item_equip: No argument provided.");
		} else {
			bCell = (int)lua_tonumber(L, 1);
		}

		if (bCell < 0 || bCell >= WEAR_MAX_NUM)
		{
			sys_err("[item_equip: Invalid wear position %d in item_equip_selected. Index out of range(0..%d)", bCell, WEAR_MAX_NUM);
			lua_pushboolean(L, false);
			return 1;
		}

		//current item in used
		LPITEM item = CQuestManager::instance().GetCurrentItem();
		if(bCell == 0)
		{
			if(item)
				bCell = item->FindEquipCell(ch, 0);
		}
		if(bCell != -1)
		{
			//check the pointers
			if (!ch || !item)
			{
				sys_err("[%s]: Wrong character or item ptr.", ch->GetName());
				lua_pushboolean(L, false);
				return 1;
			}
			DWORD itemwf = item->GetWearFlag();
			if((false == ch->CanEquipNow(item)))
				return 0;
			
			//current equipped item on target slot
			LPITEM equipped = ch->GetWear(bCell);

			//remove the equipped item
			if(equipped)
				if (equipped->GetVnum())
					ch->UnequipItem(equipped);
			if(item->IsEquipped())
				ch->UnequipItem(item);

			//equipping the item to the given slot
			item->EquipTo(ch, bCell);
			lua_pushboolean(L, true);
			return 0;
		} else {
			sys_err("item_equip: Could not find a valid cell!");
			lua_pushboolean(L, false);
			return 0;
		}
	}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	int item_get_transmutation(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (q.GetCurrentItem())
			lua_pushnumber(L, q.GetCurrentItem()->GetTransmutationVnum());
		else
			lua_pushnumber(L, 0);

		return 1;
	}
#endif

#ifdef ENABLE_QUEST_ITEM_FUNCTIONS
	int item_get_flag(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		lua_pushnumber(L, item ? item->GetFlag() : 0);
		return 1;
	}

	int item_get_wearflag(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		lua_pushnumber(L, item ? item->GetWearFlag() : 0);
		return 1;
	}

	int item_get_antiflag(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		lua_pushnumber(L, item ? item->GetAntiFlag() : 0);
		return 1;
	}

	int item_has_antiflag(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!lua_isnumber(L, 1))
		{
			sys_err("Invalid argument.");
			return 0;
		}

		if (!item) return 0;

		long lAntiCheck = (long)lua_tonumber(L, 1);
		lua_pushboolean(L, IS_SET(item->GetAntiFlag(), lAntiCheck));
		return 1;
	}

	int item_get_refine_set(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		lua_pushnumber(L, item ? item->GetRefineSet() : 0);
		return 1;
	}

	int item_get_limit(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;

		if (!lua_isnumber(L, 1))
		{
			sys_err("Wrong argument, need a number from range(0..%d)!", ITEM_LIMIT_MAX_NUM);
			lua_pushnumber(L, 0);
			return 1;
		}

		int byLimitIndex = (int)lua_tonumber(L, 1);
		if (byLimitIndex < 0 || byLimitIndex >= ITEM_LIMIT_MAX_NUM)
		{
			sys_err("Invalid limit type(%d). Out of range(0..%d)", byLimitIndex, ITEM_LIMIT_MAX_NUM);
			lua_pushnumber(L, 0);
			return 1;
		}

		lua_newtable(L);
		{
			lua_pushnumber(L, item->GetLimitType(byLimitIndex));
			lua_rawseti(L, -2, 1);

			lua_pushnumber(L, item->GetLimitValue(byLimitIndex));
			lua_rawseti(L, -2, 2);
		}
		return 1;
	}

	int item_get_apply(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item)
		{
			sys_err("No current item selected!");
			return 0;
		}

		if (!lua_isnumber(L, 1))
		{
			sys_err("Wrong argument, need a number from range(0..%d)!", ITEM_APPLY_MAX_NUM);
			lua_pushnumber(L, 0);
			return 1;
		}

		int bApplyIndex = (int)lua_tonumber(L, 1);
		if (bApplyIndex < 0 || bApplyIndex >= ITEM_APPLY_MAX_NUM)
		{
			sys_err("Invalid apply index(%d). Out of range(0..%d)", bApplyIndex, ITEM_APPLY_MAX_NUM);
			lua_pushnumber(L, 0);
			return 1;
		}

		const TItemTable* itemTable = item->GetProto();

		lua_newtable(L);
		{
			lua_pushnumber(L, itemTable->aLimits[bApplyIndex].bType);
			lua_rawseti(L, -2, 1);

			lua_pushnumber(L, itemTable->aLimits[bApplyIndex].lValue);
			lua_rawseti(L, -2, 2);
		}
		return 1;
	}

	int item_get_applies(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();
		if (!item)
		{
			sys_err("No current item selected!");
			return 0;
		}

		const TItemTable* itemTable = item->GetProto();
		lua_newtable(L);
		{
			for(BYTE i=0; i<ITEM_APPLY_MAX_NUM; i++)
			{
				char Key1[64] = "", Key2[64] = "";

				lua_newtable(L);
				lua_pushnumber(L, itemTable->aLimits[i].bType);
				lua_rawseti(L, -2, 1);

				lua_pushnumber(L, itemTable->aLimits[i].lValue);
				lua_rawseti(L, -2, 2);
				lua_rawseti(L, -2, i);
			}
		}

		return 1;
	}

	int item_get_refine_materials(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();
		if (!item)
		{
			sys_err("No current item selected!");
			return 0;
		}

		const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());
		if (!prt)
		{
			sys_err("Failed to get refine materials!");
			return 0;
		}

		if (prt->cost == 0 && prt->material_count == 0)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		lua_newtable(L);
		{
			lua_pushstring(L, "cost");
			lua_pushnumber(L, prt->cost);
			lua_rawset(L, -3);

			lua_pushstring(L, "material_count");
			lua_pushnumber(L, prt->material_count);
			lua_rawset(L, -3);

			lua_pushstring(L, "materials");
			lua_newtable(L);
			{
				for (BYTE i = 0; i < prt->material_count; i++)
				{
					lua_newtable(L);
					lua_pushnumber(L, prt->materials[i].vnum);
					lua_rawseti(L, -2, 1);

					lua_pushnumber(L, prt->materials[i].count);
					lua_rawseti(L, -2, 2);
					lua_rawseti(L, -2, i+1);
				}
			}
			lua_rawset(L, -3);
		}

		return 1;
	}

	int item_get_addon_type(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;

		const TItemTable* itemTable = item->GetProto();
		lua_pushnumber(L, itemTable ? itemTable->sAddonType : 0);
		return 1;
	}

	int item_dec(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;

		int dec = lua_isnumber(L, 1) ? (int)lua_tonumber(L, 1) : 1;
		if (dec < 1 || dec > ITEM_MAX_COUNT)
			dec = 1;

		if (item->GetCount() - dec < 0)
			return 0;

		if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK) || !item->IsStackable())
			return 0;

		item->SetCount(item->GetCount() - dec);
		return 0;
	}

	int item_inc(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;

		int inc = lua_isnumber(L, 1) ? (int)lua_tonumber(L, 1) : 1;
		if (inc < 1 || inc > ITEM_MAX_COUNT)
			inc = 1;

		if (item->GetCount() + inc > ITEM_MAX_COUNT)
			return 0;

		if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK) || !item->IsStackable())
			return 0;

		item->SetCount(item->GetCount() + inc);
		return 0;
	}

	int item_add_attribute(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;
		if (item->GetType() == ITEM_COSTUME)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		bool bRet = TRUE;
		if (item->GetAttributeCount() < 5)
			item->AddAttribute();
		else
			bRet = FALSE;

		lua_pushboolean(L, bRet);
		return 1;
	}
#endif

	int item_get_attribute(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;

		if (!lua_isnumber(L, 1))
		{
			sys_err("Wrong argument, need a number from range(0..%d)!", ITEM_ATTRIBUTE_MAX_NUM-2);
			lua_pushnumber(L, 0);
			return 1;
		}

		int iAttrIndex = (int)lua_tonumber(L, 1);
		if (iAttrIndex < 0 || iAttrIndex >= ITEM_ATTRIBUTE_MAX_NUM-2)
		{
			sys_err("Invalid index %d. Index out of range(0..%d)", iAttrIndex, ITEM_ATTRIBUTE_MAX_NUM-2);
			lua_pushnumber(L, 0);
			return 1;
		}

		const TPlayerItemAttribute& AttrItem = item->GetAttribute(iAttrIndex);

		lua_newtable(L);

		lua_pushnumber(L, AttrItem.bType);
		lua_rawseti(L, -2, 1);

		lua_pushnumber(L, AttrItem.sValue);
		lua_rawseti(L, -2, 2);
		return 1;
	}

#ifdef ENABLE_QUEST_ITEM_FUNCTIONS
	int item_set_attribute(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;
		if (item->GetType() == ITEM_COSTUME)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		if (!lua_isnumber(L, 1))
		{
			sys_err("Wrong argument[AttrIdx] #1.");
			lua_pushboolean(L, false);
			return 1;
		}
		else if (!lua_isnumber(L, 2))
		{
			sys_err("Wrong argument[AttrType] #2.");
			lua_pushboolean(L, false);
			return 1;
		}
		else if (!lua_isnumber(L, 3))
		{
			sys_err("Wrong argument[AttrValue] #3.");
			lua_pushboolean(L, false);
			return 1;
		}

		int bAttrIndex = (int)lua_tonumber(L, 1);
		if (bAttrIndex < 0 || bAttrIndex >= ITEM_ATTRIBUTE_MAX_NUM-2)
		{
			sys_err("Invalid AttrIndex %d. AttrIndex out of range(0..4)", bAttrIndex);
			lua_pushboolean(L, false);
			return 1;
		}

		int bAttrType = (int)lua_tonumber(L, 2);
		if (bAttrType < 1 || bAttrType >= MAX_APPLY_NUM)
		{
			sys_err("Invalid AttrType %d. AttrType out of range(1..%d)", MAX_APPLY_NUM);
			lua_pushboolean(L, false);
			return 1;
		}

		if (item->HasAttr(bAttrType) && (item->GetAttribute(bAttrIndex).bType != bAttrType))
		{
			sys_err("AttrType[%d] multiplicated.", bAttrType);
			lua_pushboolean(L, false);
			return 1;
		}

		int bAttrValue = (int)lua_tonumber(L, 3);
		if (bAttrValue < 1 || bAttrValue >= 32768)
		{
			sys_err("Invalid AttrValue %d. AttrValue should be between 1 and 32767!", bAttrValue);
			lua_pushboolean(L, false);
			return 1;
		}

		bool bRet = TRUE;
		int bAttrCount = item->GetAttributeCount();
		if (bAttrCount <= 4 && bAttrCount >= 0)
		{
			if (bAttrCount < bAttrIndex)
				bAttrIndex = bAttrCount;

			item->SetForceAttribute(bAttrIndex, bAttrType, bAttrValue);
		}
		else
			bRet = FALSE;

		lua_pushboolean(L, bRet);
		return 1;
	}

	int item_change_attribute(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;
		if (item->GetType() == ITEM_COSTUME)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		bool bRet = TRUE;
		if (item->GetAttributeCount() > 0)
			item->ChangeAttribute();
		else
			bRet = FALSE;

		lua_pushboolean(L, bRet);
		return 1;
	}

	int item_add_rare_attribute(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;
		if (item->GetType() == ITEM_COSTUME)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, item->AddRareAttribute() ? TRUE : FALSE);
		return 1;
	}

	int item_get_rare_attribute(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;
		if (item->GetType() == ITEM_COSTUME)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		if (!lua_isnumber(L, 1))
		{
			sys_err("Wrong argument, need a number from range(0..1)!");
			lua_pushnumber(L, 0);
			return 1;
		}

		int iRareAttrIndex = (int)lua_tonumber(L, 1);
		if (iRareAttrIndex < 0 || iRareAttrIndex > 1)
		{
			sys_err("Invalid index %d. Index out of range(0..1)", iRareAttrIndex);
			lua_pushboolean(L, false);
			return 1;
		}

		const TPlayerItemAttribute& RareAttrItem = item->GetAttribute(iRareAttrIndex+5);

		lua_newtable(L);

		lua_pushnumber(L, RareAttrItem.bType);
		lua_rawseti(L, -2, 1);

		lua_pushnumber(L, RareAttrItem.sValue);
		lua_rawseti(L, -2, 2);
		return 1;
	}

	int item_set_rare_attribute(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;
		if (item->GetType() == ITEM_COSTUME)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		if (!lua_isnumber(L, 1))
		{
			sys_err("Wrong argument[AttrIdx], not number!");
			lua_pushboolean(L, false);
			return 1;
		}
		else if (!lua_isnumber(L, 2))
		{
			sys_err("Wrong argument[AttrType], not number!");
			lua_pushboolean(L, false);
			return 1;
		}
		else if (!lua_isnumber(L, 3))
		{
			sys_err("Wrong argument[AttrValue], not number!");
			lua_pushboolean(L, false);
			return 1;
		}
		

		int iRareAttrIndex = (int)lua_tonumber(L, 1);
		if (iRareAttrIndex < 0 || iRareAttrIndex > 1)
		{
			sys_err("Invalid index %d. Index out of range(0..1)", iRareAttrIndex);
			lua_pushboolean(L, false);
			return 1;
		}

		int iRareAttrType = (int)lua_tonumber(L, 2);
		if (iRareAttrType < 1 || iRareAttrType >= MAX_APPLY_NUM)
		{
			sys_err("Invalid apply %d. Apply out of range(1..%d)", MAX_APPLY_NUM);
			lua_pushboolean(L, false);
			return 1;
		}

		if (item->HasAttr(iRareAttrType) && (item->GetAttribute(iRareAttrIndex).bType != iRareAttrType))
		{
			sys_err("Apply %d muliplicated.", iRareAttrType);
			lua_pushboolean(L, false);
			return 1;
		}

		int iRareAttrValue = (int)lua_tonumber(L, 3);
		if (iRareAttrValue < 1 || iRareAttrValue >= 32768)
		{
			sys_err("Invalid value %d. The value should be between 1 and 32767!", iRareAttrValue);
			lua_pushboolean(L, false);
			return 1;
		}

		bool bRet = TRUE;
		int iRareAttrCount = item->GetRareAttrCount();
		if (iRareAttrCount <= 1 && iRareAttrCount >= 0)
		{
			if (iRareAttrCount < iRareAttrIndex)
				iRareAttrIndex = iRareAttrCount;

			item->SetForceAttribute(iRareAttrIndex+5, iRareAttrType, iRareAttrValue);
		}
		else
			bRet = FALSE;

		lua_pushboolean(L, bRet);
		return 1;
	}

	int item_change_rare_attribute(lua_State* L)
	{
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (!item) return 0;
		if (item->GetType() == ITEM_COSTUME)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, item->ChangeRareAttribute());
		return 1;
	}
	
	int item_get_attr_value(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (!item)
		{
			sys_err("cannot get current item");
			lua_pushnumber(L, 0);
			return 1;
		}

		if (false == lua_isnumber(L, 1))
		{
			sys_err("index is not a number");
			lua_pushnumber(L, 0);
			return 1;
		}

		int index = lua_tonumber(L, 1);
		const TPlayerItemAttribute& attrItem = item->GetAttribute(index);

		lua_pushnumber(L, attrItem.sValue);

		return 1;
	}

	int item_get_attr_type(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPITEM item = q.GetCurrentItem();

		if (!item)
		{
			sys_err("cannot get current item");
			lua_pushnumber(L, 0);
			return 1;
		}

		if (false == lua_isnumber(L, 1))
		{
			sys_err("index is not a number");
			lua_pushnumber(L, 0);
			return 1;
		}

		int index = lua_tonumber(L, 1);
		const TPlayerItemAttribute& attrItem = item->GetAttribute(index);

		lua_pushnumber(L, attrItem.bType);

		return 1;
	}
#endif

	void RegisterITEMFunctionTable()
	{

		luaL_reg item_functions[] =
		{
			{ "get_id",							item_get_id								},
			{ "get_cell",						item_get_cell							},
			{ "select",							item_select								},
			{ "select_cell",					item_select_cell						},
			{ "remove",							item_remove								},
			{ "get_socket",						item_get_socket							},
			{ "set_socket",						item_set_socket							},
			{ "get_vnum",						item_get_vnum							},
			{ "has_flag",						item_has_flag							},
			{ "get_value",						item_get_value							},
			{ "set_value",						item_set_value							},
			{ "get_name",						item_get_name							},
			{ "get_size",						item_get_size							},
			{ "get_count",						item_get_count							},
#ifdef ENABLE_QUEST_ITEM_FUNCTIONS
			{ "set_count",						item_set_count							},
#endif
			{ "get_type",						item_get_type							},
			{ "get_sub_type",					item_get_sub_type						},
			{ "get_refine_vnum",				item_get_refine_vnum					},
			{ "get_level",						item_get_level							},
			{ "next_refine_vnum",				item_next_refine_vnum					},
			{ "get_level_limit", 				item_get_level_limit					},
			{ "start_realtime_expire", 			item_start_realtime_expire				},
			{ "copy_and_give_before_remove",	item_copy_and_give_before_remove		},
#ifdef ENABLE_QUEST_ITEM_FUNCTIONS
			{ "equip",							item_equip_selected						},
#endif
			{ "is_available",					item_is_available						},
#ifdef ENABLE_QUEST_ITEM_FUNCTIONS
			{ "get_flag",						item_get_flag							},
			{ "get_wearflag",					item_get_wearflag						},
			{ "get_antiflag",					item_get_antiflag						},
			{ "has_antiflag",					item_has_antiflag						},
			{ "get_refine_set",					item_get_refine_set						},
			{ "get_limit",						item_get_limit							},
			{ "get_apply",						item_get_apply							},
			{ "get_applies",					item_get_applies,						},
			{ "get_refine_materials",			item_get_refine_materials				},
			{ "get_addon_type",					item_get_addon_type						},
			{ "dec",							item_dec								},
			{ "inc",							item_inc								},
			{ "add_attribute",					item_add_attribute						},
#endif
			{ "get_attribute",					item_get_attribute						},
#ifdef ENABLE_QUEST_ITEM_FUNCTIONS
			{ "set_attribute",		            item_set_attribute						},
			{ "change_attribute",				item_change_attribute					},
			{ "add_rare_attribute",				item_add_rare_attribute					},
			{ "get_rare_attribute",				item_get_rare_attribute					},
			{ "set_rare_attribute",				item_set_rare_attribute					},
			{ "change_rare_attribute",			item_change_rare_attribute				},
			{ "get_attr_type", 					item_get_attr_type 						},
			{ "get_attr_value", 				item_get_attr_value 					},
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			{ "get_transmutation",				item_get_transmutation					},
#endif

			{ NULL,								NULL									}
		};
		CQuestManager::instance().AddLuaFunctionTable("item", item_functions);
	}
}
