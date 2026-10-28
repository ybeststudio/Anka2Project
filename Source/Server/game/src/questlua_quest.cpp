#include "stdafx.h"

#include "questlua.h"
#include "questmanager.h"

#undef sys_err
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)

namespace quest
{
	int quest_start(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		q.GetCurrentPC()->SetCurrentQuestStartFlag();
		return 0;
	}

	int quest_done(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		q.GetCurrentPC()->SetCurrentQuestDoneFlag();
		return 0;
	}

	int quest_set_title(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (lua_isstring(L, -1))
			q.GetCurrentPC()->SetCurrentQuestTitle(lua_tostring(L, -1));

		return 0;
	}

	int quest_set_another_title(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (lua_isstring(L, 1) && lua_isstring(L, 2))
			q.GetCurrentPC()->SetQuestTitle(lua_tostring(L, 1), lua_tostring(L, 2));

		return 0;
	}

	int quest_set_clock_name(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (lua_isstring(L, -1))
			q.GetCurrentPC()->SetCurrentQuestClockName(lua_tostring(L, -1));

		return 0;
	}

	int quest_set_clock_value(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (lua_isnumber(L, -1))
			q.GetCurrentPC()->SetCurrentQuestClockValue((int)rint(lua_tonumber(L, -1)));

		return 0;
	}

	int quest_set_counter_name(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (lua_isstring(L, -1))
			q.GetCurrentPC()->SetCurrentQuestCounterName(lua_tostring(L, -1));

		return 0;
	}

	int quest_set_counter_value(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (lua_isnumber(L, -1))
			q.GetCurrentPC()->SetCurrentQuestCounterValue((int)rint(lua_tonumber(L, -1)));

		return 0;
	}

	int quest_set_icon_file(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (lua_isstring(L, -1))
			q.GetCurrentPC()->SetCurrentQuestIconFile(lua_tostring(L, -1));

		return 0;
	}

	int quest_setstate(lua_State* L)
	{
		if (lua_tostring(L, -1) == NULL)
		{
			sys_err("state name is empty");
			return 0;
		}

		CQuestManager& q = CQuestManager::instance();
		QuestState* pqs = q.GetCurrentState();
		PC* pPC = q.GetCurrentPC();

		if (L != pqs->co)
		{
			luaL_error(L, "running thread != current thread???");
			if (test_server)
				sys_log(0, "running thread != current thread???");
			return 0;
		}

		if (pPC)
		{
			std::string stCurrentState = lua_tostring(L, -1);
			if (test_server)
				sys_log(0, "questlua->setstate( %s, %s )", pPC->GetCurrentQuestName().c_str(), stCurrentState.c_str());
			pqs->st = q.GetQuestStateIndex(pPC->GetCurrentQuestName(), stCurrentState);
			pPC->SetCurrentQuestStateName(stCurrentState);
		}
		return 0;
	}

	int quest_coroutine_yield(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (q.IsInOtherPCBlock())
		{
			sys_err("FATAL ERROR! Yield occur in other_pc_block.");
			PC* pPC = q.GetOtherPCBlockRootPC();
			if (NULL == pPC)
			{
				sys_err("... FFFAAATTTAAALLL Error. RootPC is NULL");
				return 0;
			}
			QuestState* pQS = pPC->GetRunningQuestState();
			if (NULL == pQS || NULL == q.GetQuestStateName(pPC->GetCurrentQuestName(), pQS->st))
			{
				sys_err("... WHO AM I? WHERE AM I? I only know QuestName(%s)...", pPC->GetCurrentQuestName().c_str());
			}
			else
			{
				sys_err("Current Quest(%s). State(%s)", pPC->GetCurrentQuestName().c_str(), q.GetQuestStateName(pPC->GetCurrentQuestName(), pQS->st));
			}
			return 0;
		}
		return lua_yield(L, lua_gettop(L));
	}

	int quest_no_send(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		q.SetNoSend();
		return 0;
	}

	int quest_get_current_quest_index(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		PC* pPC = q.GetCurrentPC();

		int idx = q.GetQuestIndexByName(pPC->GetCurrentQuestName());
		lua_pushnumber(L, idx);
		return 1;
	}

	int quest_begin_other_pc_block(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		DWORD pid = lua_tonumber(L, -1);
		q.BeginOtherPCBlock(pid);
		return 0;
	}

	int quest_end_other_pc_block(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		q.EndOtherPCBlock();
		return 0;
	}

	void RegisterQuestFunctionTable()
	{
		luaL_reg quest_functions[] =
		{
			{ "setstate",				quest_setstate				},
			{ "set_state",				quest_setstate				},
			{ "yield",					quest_coroutine_yield		},
			{ "set_title",				quest_set_title				},
			{ "set_title2",				quest_set_another_title		},
			{ "set_clock_name",			quest_set_clock_name		},
			{ "set_clock_value",		quest_set_clock_value		},
			{ "set_counter_name",		quest_set_counter_name		},
			{ "set_counter_value",		quest_set_counter_value		},
			{ "set_icon",				quest_set_icon_file			},
			{ "start",					quest_start					},
			{ "done",					quest_done					},
			{ "getcurrentquestindex",	quest_get_current_quest_index},
			{ "no_send",				quest_no_send				},
			{ "begin_other_pc_block",	quest_begin_other_pc_block	},
			{ "end_other_pc_block",		quest_end_other_pc_block	},
			{ NULL,						NULL						}
		};

		CQuestManager::instance().AddLuaFunctionTable("q", quest_functions);
	}
}
