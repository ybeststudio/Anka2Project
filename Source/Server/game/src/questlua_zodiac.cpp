#include "stdafx.h"
#include <sstream>
#include "constants.h"
#include "char.h"
#include "char_manager.h"

#include "questmanager.h"
#include "questlua.h"
#include "questevent.h"
#include "config.h"
#include "mining.h"
#include "fishing.h"
#include "priv_manager.h"
#include "utils.h"
#include "p2p.h"
#include "item_manager.h"
#include "db.h"
#include "mob_manager.h"
#include "start_position.h"
#include "OXEvent.h"
#include "regen.h"
#include "cmd.h"
#include "guild.h"
#include "guild_manager.h" 
#include "sectree_manager.h"

#undef sys_err
#ifndef __WIN32__
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)

#endif

#ifdef ENABLE_ZODIAC_MISSION
namespace quest
{
	int dungeon_mission(lua_State* L)
	{
		if (!lua_isstring(L, 1))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if(!pDungeon || pDungeon == NULL)
			return 0;

		if (pDungeon)
			pDungeon->MissionNotice(lua_isnumber(L, 1)?(int)lua_tonumber(L, 1):pDungeon->GetMapIndex(),lua_tostring(L, 2));
		return 0;
	}

	int dungeon_command_new(lua_State* L)
	{
		if (!lua_isstring(L, 1))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if(!pDungeon || pDungeon == NULL)
			return 0;

		if (pDungeon)
			pDungeon->DungeonCommand(pDungeon->GetMapIndex(),lua_tostring(L, 1));
		return 0;
	}

	int dungeon_give_flag(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if(pDungeon)
			pDungeon->ZodiacFlag(pDungeon->GetMapIndex());
		return 0;
	}

	int dungeon_give_chest(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDUNGEON pDungeon = q.GetCurrentDungeon();
		if(pDungeon)
			pDungeon->ZodiacGiveChest(pDungeon->GetMapIndex());
		return 0;
	}

	int dungeon_is_zodiac_stun(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentNPCCharacterPtr();
		if(ch != 0)
			lua_pushboolean(L, (ch->GetRaceNum() >= 9884 && ch->GetRaceNum() <= 9895)?true:false);
		return 1;
	}

	void RegisterZodiacFunctionTable()
	{
		luaL_reg Zodiac_functions[] =
		{
			{ "mission_notice",			dungeon_mission		},
			{ "command_new",			dungeon_command_new		},
			{ "give_flag",			dungeon_give_flag		},
			{ "give_chest",			dungeon_give_chest		},
			{ "is_zodiac_stun",			dungeon_is_zodiac_stun		},
			{NULL, NULL}
		};
		CQuestManager::instance().AddLuaFunctionTable("zodiac", Zodiac_functions);
	}
}
#endif
