#include "stdafx.h"

#ifdef ENABLE_QUEEN_NETHIS
#undef sys_err
#ifndef __WIN32__
	#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
	#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

#include "questlua.h"
#include "questmanager.h"
#include "SnakeLair.h"
#include "char.h"
#include "party.h"

namespace quest
{
	int SnakeLair_Access(lua_State* L)
	{
		LPCHARACTER pkChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (pkChar)
		{
			bool bTry = pkChar->GetParty() ? SnakeLair::CSnk::instance().Access(pkChar) : false;
			lua_pushboolean(L, bTry);
		}
		else
		{
			lua_pushboolean(L, false);
		}
		
		return 2;
	}

	int SnakeLair_SetDungeonStep(lua_State* L)
	{
		LPCHARACTER pkChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!pkChar)
			return 0;

		SnakeLair::CSnk::instance().SetDungeonStep(pkChar);

		return 1;		
	}

	void RegisterSnakeLairFunctionTable()
	{
		luaL_reg functions[] =
		{
			{"Access", SnakeLair_Access},
			{"SetDungeonStep", SnakeLair_SetDungeonStep},
			{NULL, NULL}
		};
		
		CQuestManager::instance().AddLuaFunctionTable("SnakeLair", functions);
	}
};
#endif
