#include "stdafx.h"

#ifdef ENABLE_WHITE_DRAGON
#undef sys_err
#ifndef __WIN32__
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

#include "questlua.h"
#include "questmanager.h"
#include "WhiteDragon.h"
#include "char.h"
#include "party.h"

namespace quest
{
	int WhiteDragon_Access(lua_State* L)
	{
		if (!lua_isboolean(L, 1))
		{
			sys_err("invalid argument!");
			return 0;
		}

		LPCHARACTER pkChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!pkChar)
			lua_pushboolean(L, false);

		WhiteDragon::CWhDr::instance().Access(pkChar, lua_toboolean(L, 1)); // Easy [false] | Hard [true]
		lua_pushboolean(L, true);
		return 2;
	}

	int WhiteDragon_StartDungeon(lua_State* L)
	{
		LPCHARACTER pkChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!pkChar)
			return 0;

		WhiteDragon::CWhDr::instance().StartDungeon(pkChar);

		return 1;
	}

	void RegisterWhiteLairFunctionTable()
	{
		luaL_reg functions[] =
		{
			{"Access", WhiteDragon_Access},
			{"StartDungeon", WhiteDragon_StartDungeon},
			{NULL, NULL}
		};

		CQuestManager::instance().AddLuaFunctionTable("WhiteLair", functions);
	}
};
#endif
