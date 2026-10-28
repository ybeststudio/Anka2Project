#include "stdafx.h"
#include "questmanager.h"
#include "dungeon_info.h"
#include "char.h"


#ifdef ENABLE_DUNGEON_INFO
namespace quest
{
	int dungeon_get_min_level(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		int id_map_dungeon = (int)lua_tonumber(L,1);
		lua_pushnumber(L, CDungeonInfoExtern::instance().GetMinLevel(id_map_dungeon));
		return 1;
	}

	int dungeon_get_max_level(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		int id_map_dungeon = (int)lua_tonumber(L,1);
		lua_pushnumber(L, CDungeonInfoExtern::instance().GetMaxLevel(id_map_dungeon));
		return 1;
	}

	int dungeon_get_party_members(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		int id_map_dungeon = (int)lua_tonumber(L,1);
		lua_pushnumber(L, CDungeonInfoExtern::instance().GetPartyMembers(id_map_dungeon));
		return 1;
	}

	int dungeon_get_time_room(lua_State* L)
	{

		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}


		int id_map_dungeon = (int)lua_tonumber(L,1);
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		long index_map_dungeon = CDungeonInfoExtern::instance().GetIndexMapDungeon(id_map_dungeon);

		lua_pushnumber(L, CDungeonInfoExtern::instance().GetTimeRoomDungeonActual(ch,index_map_dungeon));
		return 1;
	}

	int dungeon_get_time_respawn(lua_State* L)
	{

		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}


		int id_map_dungeon = (int)lua_tonumber(L,1);
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		long index_map_dungeon = CDungeonInfoExtern::instance().GetIndexMapDungeon(id_map_dungeon);

		lua_pushnumber(L, CDungeonInfoExtern::instance().GetTimeRespawnDungeonActual(ch,index_map_dungeon));
		return 1;
	}

	int dungeon_get_vnum_item(lua_State* L)
	{

		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}


		int id_map_dungeon = (int)lua_tonumber(L,1);

		lua_pushnumber(L, CDungeonInfoExtern::instance().GetVnumItem(id_map_dungeon));
		return 1;
	}

	int dungeon_get_count_item(lua_State* L)
	{

		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}


		int id_map_dungeon = (int)lua_tonumber(L,1);
		lua_pushnumber(L, CDungeonInfoExtern::instance().GetCountItem(id_map_dungeon));
		return 1;
	}


	int dungeon_get_x_join(lua_State* L)
	{

		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}


		int id_map_dungeon = (int)lua_tonumber(L,1);
		lua_pushnumber(L, CDungeonInfoExtern::instance().GetXMapDungeon(id_map_dungeon));
		return 1;
	}

	int dungeon_get_y_join(lua_State* L)
	{

		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}


		int id_map_dungeon = (int)lua_tonumber(L,1);
		lua_pushnumber(L, CDungeonInfoExtern::instance().GetYMapDungeon(id_map_dungeon));
		return 1;
	}


	int dungeon_get_index_map(lua_State* L)
	{

		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}


		int id_map_dungeon = (int)lua_tonumber(L,1);
		lua_pushnumber(L, CDungeonInfoExtern::instance().GetIndexMapDungeon(id_map_dungeon));
		return 1;
	}

	int dungeon_get_item_check(lua_State* L)
	{

		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}


		int id_map_dungeon = (int)lua_tonumber(L,1);
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		long index_map_dungeon = CDungeonInfoExtern::instance().GetIndexMapDungeon(id_map_dungeon);

		lua_pushnumber(L, CDungeonInfoExtern::instance().GetItemCheckDungeonActual(ch,index_map_dungeon));
		return 1;
	}


	int dungeon_reset_item_check(lua_State* L)
	{

		if (!lua_isnumber(L, 1))
		{
			return 1;
		}


		int id_map_dungeon = (int)lua_tonumber(L,1);
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		long index_map_dungeon = CDungeonInfoExtern::instance().GetIndexMapDungeon(id_map_dungeon);

		char fix_item_check[1024];
		snprintf(fix_item_check, sizeof(fix_item_check), "dungeon_info.fix_item_check_%ld", index_map_dungeon);
		ch->SetQuestFlag(fix_item_check,0);

		return 1;
	}


	int dungeon_give_item(lua_State* L)
	{

		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			return 1;
		}


		DWORD vnum = (int)lua_tonumber(L,1);
		BYTE count = (int)lua_tonumber(L,2);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		CDungeonInfoExtern::instance().StartGiveItemDungeon(ch,vnum,count);

		return 1;
	}

	void RegisterDungeonInfoFunctionTable()
	{
		luaL_reg dragon_info_functions[] =
		{
			{	"get_min_level",	dungeon_get_min_level	},
			{	"get_max_level",	dungeon_get_max_level	},
			{	"get_party_members",	dungeon_get_party_members	},
			{	"get_time_room",	dungeon_get_time_room	},
			{	"get_time_respawn",	dungeon_get_time_respawn	},
			{   "get_vnum_item", dungeon_get_vnum_item},
			{   "get_count_item", dungeon_get_count_item},
			{   "get_x_join", dungeon_get_x_join},
			{   "get_y_join", dungeon_get_y_join},
			{   "get_index_map", dungeon_get_index_map},
			{	"get_item_check", dungeon_get_item_check},
			{	"reset_item_check",dungeon_reset_item_check},
			{	"give_item",dungeon_give_item},
			{	NULL,			NULL			}
		};

		CQuestManager::instance().AddLuaFunctionTable("DungeonInfo", dragon_info_functions);
	}
}
#endif

