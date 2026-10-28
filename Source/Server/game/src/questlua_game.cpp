#include "stdafx.h"

#include "../../common/service.h"

#include "questlua.h"
#include "questmanager.h"
#include "desc_client.h"
#include "char.h"
#include "item_manager.h"
#include "item.h"
#include "cmd.h"
#include "packet.h"
#include "utils.h"
#include "db.h"
#ifdef ENABLE_DICE_SYSTEM
	#include "party.h"
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	#include "changelook.h"
#endif

#ifdef ENABLE_RIDING_EXTENDED
	#include "mount_up_grade.h"
#endif

#ifdef ENABLE_EVENT_SYSTEM
	#include "auto_event_list.h"
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
	#include "minigame.h"
#endif

#undef sys_err
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)

extern ACMD(do_in_game_mall);

namespace quest
{
	int game_set_event_flag(lua_State* L)
	{
		CQuestManager & q = CQuestManager::instance();

		if (lua_isstring(L,1) && lua_isnumber(L, 2))
			q.RequestSetEventFlag(lua_tostring(L,1), (int)lua_tonumber(L,2));

		return 0;
	}

	int game_get_event_flag(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		if (lua_isstring(L,1))
			lua_pushnumber(L, q.GetEventFlag(lua_tostring(L,1)));
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int game_request_make_guild(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPDESC d = q.GetCurrentCharacterPtr()->GetDesc();
		if (d)
		{
			BYTE header = HEADER_GC_REQUEST_MAKE_GUILD;
			d->Packet(&header, 1);
		}
		return 0;
	}

	int game_get_safebox_level(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		lua_pushnumber(L, q.GetCurrentCharacterPtr()->GetSafeboxSize()/SAFEBOX_PAGE_SIZE);
		return 1;
	}

	int game_set_safebox_level(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();

		TSafeboxChangeSizePacket p;
		p.dwID = q.GetCurrentCharacterPtr()->GetDesc()->GetAccountTable().id;
		p.bSize = (int)lua_tonumber(L,-1);
		db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_SIZE,  q.GetCurrentCharacterPtr()->GetDesc()->GetHandle(), &p, sizeof(p));

		q.GetCurrentCharacterPtr()->SetSafeboxSize(SAFEBOX_PAGE_SIZE * (int)lua_tonumber(L,-1));
		return 0;
	}

	int game_open_safebox(lua_State* /*L*/)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		ch->SetSafeboxOpenPosition();
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
		return 0;
	}

#ifdef ENABLE_GAYA_SHOP_SYSTEM
	int game_open_gem_c(lua_State*)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenGuiGem");
		return 0;
	}

	int game_open_gem_m(lua_State*)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (ch->CheckItemsFull() == false)
		{
			//ch->SetGemState("gem_refresh_time", init_gemTime() + (60*60*5));
			ch->UpdateItemsGemMarker();
			ch->InfoGemMarker();
			ch->StartCheckTimeMarket();
		}
		else
		{
			ch->InfoGemMarker();
			ch->StartCheckTimeMarket();
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenGuiGemMarket");

		return 0;
	}
#endif

	int game_open_mall(lua_State* /*L*/)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		ch->SetSafeboxOpenPosition();
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
		return 0;
	}

	int game_drop_item(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		DWORD item_vnum = (DWORD) lua_tonumber(L, 1);
		int count = (int) lua_tonumber(L, 2);
		long x = ch->GetX();
		long y = ch->GetY();

		LPITEM item = ITEM_MANAGER::instance().CreateItem(item_vnum, count);

		if (!item)
		{
			sys_err("cannot create item vnum %d count %d", item_vnum, count);
			return 0;
		}

		PIXEL_POSITION pos;
		pos.x = x + number(-200, 200);
		pos.y = y + number(-200, 200);

		item->AddToGround(ch->GetMapIndex(), pos);
		item->StartDestroyEvent();

		return 0;
	}

	int game_drop_item_with_ownership(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		LPITEM item = NULL;
		switch (lua_gettop(L))
		{
		case 1:
			item = ITEM_MANAGER::instance().CreateItem((DWORD) lua_tonumber(L, 1));
			break;
		case 2:
		case 3:
			item = ITEM_MANAGER::instance().CreateItem((DWORD) lua_tonumber(L, 1), (int) lua_tonumber(L, 2));
			break;
		default:
			return 0;
		}

		if ( item == NULL )
		{
			return 0;
		}

		if (lua_isnumber(L, 3))
		{
			int sec = (int) lua_tonumber(L, 3);
			if (sec <= 0)
			{
				item->SetOwnership( ch );
			}
			else
			{
				item->SetOwnership( ch, sec );
			}
		}
		else
			item->SetOwnership( ch );

		PIXEL_POSITION pos;
		pos.x = ch->GetX() + number(-200, 200);
		pos.y = ch->GetY() + number(-200, 200);

		item->AddToGround(ch->GetMapIndex(), pos);
		item->StartDestroyEvent();

		return 0;
	}

#ifdef ENABLE_DICE_SYSTEM
	int game_drop_item_with_ownership_and_dice(lua_State* L)
	{
		LPITEM item = NULL;
		switch (lua_gettop(L))
		{
		case 1:
			item = ITEM_MANAGER::instance().CreateItem((DWORD) lua_tonumber(L, 1));
			break;
		case 2:
		case 3:
			item = ITEM_MANAGER::instance().CreateItem((DWORD) lua_tonumber(L, 1), (int) lua_tonumber(L, 2));
			break;
		default:
			return 0;
		}

		if ( item == NULL )
		{
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch->GetParty())
		{
			FPartyDropDiceRoll f(item, ch);
			f.Process(NULL);
		}

		if (lua_isnumber(L, 3))
		{
			int sec = (int) lua_tonumber(L, 3);
			if (sec <= 0)
			{
				item->SetOwnership( ch );
			}
			else
			{
				item->SetOwnership( ch, sec );
			}
		}
		else
			item->SetOwnership( ch );

		PIXEL_POSITION pos;
		pos.x = ch->GetX() + number(-200, 200);
		pos.y = ch->GetY() + number(-200, 200);

		item->AddToGround(ch->GetMapIndex(), pos);
		item->StartDestroyEvent();

		return 0;
	}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	int game_open_aura_absorb_window(lua_State* L)
	{
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (NULL == ch)
		{
			sys_err("NULL POINT ERROR");
			return 0;
		}

		ch->OpenAuraRefineWindow(CQuestManager::instance().GetCurrentNPCCharacterPtr(), AURA_WINDOW_TYPE_ABSORB);
		return 0;
	}

	int game_open_aura_growth_window(lua_State* L)
	{
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (NULL == ch)
		{
			sys_err("NULL POINT ERROR");
			return 0;
		}

		ch->OpenAuraRefineWindow(CQuestManager::instance().GetCurrentNPCCharacterPtr(), AURA_WINDOW_TYPE_GROWTH);
		return 0;
	}

	int game_open_aura_evolve_window(lua_State* L)
	{
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (NULL == ch)
		{
			sys_err("NULL POINT ERROR");
			return 0;
		}

		ch->OpenAuraRefineWindow(CQuestManager::instance().GetCurrentNPCCharacterPtr(), AURA_WINDOW_TYPE_EVOLVE);
		return 0;
	}
#endif

#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
	int game_open_item_combination(lua_State* L)
	{
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;
		ch->SetItemCombinationOpen(true);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowItemCombinationDialog");
		return 0;
	}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	int game_open_transmutation(lua_State* L)
	{
		if (lua_isboolean(L, 1))
		{
			const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
			const bool bType = lua_toboolean(L, 1);
			CTransmutation::Open(ch, bType);
		}

		return 0;
	}
#endif

#ifdef ENABLE_RIDING_EXTENDED
	int game_open_mount_up_grade(lua_State* L)
	{
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (nullptr == ch)
		{
			sys_err("game_open_mount_up_grade ERROR");
			return 0;
		}

		CMountUpGrade::Instance().OpenMountUpGrade(ch);

		return 0;
	}
#endif

	int game_web_mall(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( ch != NULL )
		{
			do_in_game_mall(ch, const_cast<char*>(""), 0, 0);
		}
		return 0;
	}

#ifdef ENABLE_ZODIAC_MISSION
	int game_open_zodiac(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if(!ch || ch == NULL)
			return 0;
		if (ch->GetProtectTime("Zodiac12Zi") > get_global_time())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Biraz beklemelisin.");
			return 0;
		}
		ch->SetProtectTime("Zodiac12Zi",get_global_time()+1);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenUI12zi %d %d %d %d", ch->GetQuestFlag("Quest_ZodiacTemple.YellowMark"), ch->GetQuestFlag("Quest_ZodiacTemple.GreenMark"), ch->GetQuestFlag("Quest_ZodiacTemple.YellowReward"), ch->GetQuestFlag("Quest_ZodiacTemple.GreenReward"));
		return 0;
	}
#endif

#ifdef ENABLE_MINI_GAME_CATCH_KING
	int mini_game_catch_king_get_score(lua_State* L)
	{
		DWORD dwArg = (DWORD) lua_tonumber(L, 1);
		bool isTotal = dwArg ? true : false;
		
		CMiniGame::instance().MiniGameCatchKingGetScore(L, isTotal);
		return 1;
	}
#endif

#ifdef ENABLE_EVENT_SYSTEM
	int game_set_event_time(lua_State* L)
	{
		if (lua_isnumber(L, 1) && lua_isnumber(L, 2))
			CGameEventsManager::instance().SetEventTime((int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2));

		return 0;
	}
#endif

#ifdef ENABLE_QUEST_MYSQL_QUERY
	int game_mysql_query(lua_State* L)
	{
		//MYSQL_FIELD *field;
		// FIX: lua string format olarak kullaniliyordu (format-string) ve SQLMsg leak oluyordu
		const char* szQuery = lua_tostring(L, 1);
		if (!szQuery)
			return 0;
		std::unique_ptr<SQLMsg> run(DBManager::instance().DirectQuery("%s", szQuery));
		if (!run || !run->Get())
			return 0;
		MYSQL_RES* res=run->Get()->pSQLResult;
		if (!res){
			lua_pushnumber(L, 0);
			return 0;
		}
		MYSQL_ROW row;
		lua_newtable(L);			
		int rowcount = 1;
		while((row = mysql_fetch_row(res))){
			lua_newtable(L);
			lua_pushnumber(L, rowcount);
			lua_pushvalue(L, -2);
			lua_settable(L, -4);
			unsigned int fields = mysql_num_fields(res);
			for(unsigned int i = 0; i < fields; i++){
				lua_pushnumber(L, i + 1);
				lua_pushstring(L, row[i]);
				lua_settable(L, -3);
			}
			lua_pop(L, 1);
			rowcount++;
		}
		return 1;
	}
#endif
	
	void RegisterGameFunctionTable()
	{
		luaL_reg game_functions[] =
		{
			{ "get_safebox_level",						game_get_safebox_level						},
			{ "request_make_guild",						game_request_make_guild						},
			{ "set_safebox_level",						game_set_safebox_level						},
			{ "open_safebox",							game_open_safebox							},
			{ "open_mall",								game_open_mall								},
			{ "get_event_flag",							game_get_event_flag							},
			{ "set_event_flag",							game_set_event_flag							},
			{ "drop_item",								game_drop_item								},
			{ "drop_item_with_ownership",				game_drop_item_with_ownership				},
			{ "open_web_mall",							game_web_mall								},
#ifdef ENABLE_DICE_SYSTEM
			{ "drop_item_with_ownership_and_dice",		game_drop_item_with_ownership_and_dice		},
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			{ "open_aura_absorb_window",				game_open_aura_absorb_window				},
			{ "open_aura_growth_window",				game_open_aura_growth_window				},
			{ "open_aura_evolve_window",				game_open_aura_evolve_window				},
#endif
#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
			{ "open_item_combination",					game_open_item_combination					},
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			{ "open_transmutation",						game_open_transmutation						},
#endif
#ifdef ENABLE_RIDING_EXTENDED
			{ "open_mount_up_grade", 					game_open_mount_up_grade					},
#endif
#ifdef ENABLE_ZODIAC_MISSION
			{ "open_zodiac", 							game_open_zodiac 							},
#endif
#ifdef ENABLE_MINI_GAME_CATCH_KING
			{ "mini_game_catch_king_get_score",	mini_game_catch_king_get_score		},
#endif
#ifdef ENABLE_GAYA_SHOP_SYSTEM
			{ "open_gem",					game_open_gem_c					},
			{ "open_gem_market",			game_open_gem_m					},
#endif
#ifdef ENABLE_EVENT_SYSTEM
			{ "set_event_time",				game_set_event_time				},
#endif
#ifdef ENABLE_QUEST_MYSQL_QUERY
			{ "mysql_query",	game_mysql_query },
#endif

			{ NULL,					NULL				}
		};

		CQuestManager::instance().AddLuaFunctionTable("game", game_functions);
	}
}

