#include "stdafx.h"

#include "../../common/service.h"

#include "config.h"
#include "questmanager.h"
#include "sectree_manager.h"
#include "char.h"
#include "char_manager.h"
#include "affect.h"
#include "item.h"
#include "item_manager.h"
#include "guild_manager.h"
#include "war_map.h"
#include "start_position.h"
#include "marriage.h"
#include "mining.h"
#include "p2p.h"
#include "polymorph.h"
#include "desc_client.h"
#include "messenger_manager.h"
#include "log.h"
#include "utils.h"
#include "unique_item.h"
#include "mob_manager.h"
#ifdef ENABLE_NEWSTUFF
#include "pvp.h"
#endif

#ifdef ENABLE_DICE_SYSTEM
	#include "party.h"
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	#include "offlineshop_manager.h"
#endif

#include <cctype>
#undef sys_err
#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)

extern int g_nPortalLimitTime;
extern LPCLIENT_DESC db_clientdesc;
const int ITEM_BROKEN_METIN_VNUM = 28960;

// #define ENABLE_PC_OPENSHOP

#ifdef ENABLE_PC_OPENSHOP
#include "shop.h"
#include "shop_manager.h"
#endif

namespace quest
{
	//
	// "pc" Lua functions
	//
#ifdef ENABLE_SUNG_MAHI_TOWER
	int pc_sung_mahi_curse_hp(lua_State* L)
	{
		if (lua_isnumber(L, 1))
		{
			CQuestManager & q = CQuestManager::instance();
			LPCHARACTER ch = q.GetCurrentCharacterPtr();
			
			if (ch)
			{
				BYTE bCurseAmount = (BYTE) lua_tonumber(L, 1);
				if (bCurseAmount < 1 or bCurseAmount > 10)
					return 0;
				
				ch->PointChange(POINT_HP, -(ch->GetMaxHP() * (10 * bCurseAmount) / 100));
			}
		}
		else
			sys_err("Invalid argument: arg1 must be number.");
		return 0;
	}
#endif

	int pc_has_master_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		bool bHasMasterSkill = false;
		for (int i=0; i< SKILL_MAX_NUM; i++)
			if (ch->GetSkillMasterType(i) >= SKILL_MASTER && ch->GetSkillLevel(i) >= 21)
			{
				bHasMasterSkill = true;
				break;
			}

		lua_pushboolean(L, bHasMasterSkill);
		return 1;
	}

#ifdef ENABLE_RANKING
	int pc_add_progress_in_rank(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1))
		{
			sys_err("pc_add_progress_in_rank :: WRONG ARGUMENTS");
			return 0;
		}

		BYTE bTypeRank = (BYTE) rint(lua_tonumber(L, 1));

		return 0;
	}

	int pc_ranking_dungeon_complete(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		return 0;
	}
#endif

#ifdef ENABLE_MINIGAME_RUMI_EVENT
	int pc_get_okay_global_rank(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;
		

		char szBuf[4096+1];
		ch->GetGlobalRank(szBuf, sizeof(szBuf));
		lua_pushstring(L, szBuf);
		return 1;
	}
	
	int pc_get_okay_rund_rank(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;
		

		char szBuf[4096+1];
		ch->GetRundRank(szBuf, sizeof(szBuf));
		lua_pushstring(L, szBuf);
		return 1;
	}
#endif

	int pc_remove_skill_book_no_delay(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
		return 0;
	}

	int pc_is_skill_book_no_delay(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushboolean(L, ch->FindAffect(AFFECT_SKILL_NO_BOOK_DELAY) ? true : false);
		return 1;
	}

	int pc_learn_grand_master_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong skill index");
			return 0;
		}

		lua_pushboolean(L, ch->LearnGrandMasterSkill((long)lua_tonumber(L, 1)));
		return 1;
	}

#ifdef ENABLE_MESSENGER_BLOCK
	int pc_is_blocked(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong::pc_is_blocked");
			return 0;
		}

		const char* arg1 = lua_tostring(L, 1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);
		if (!tch || !tch->IsPC())
		{
			sys_err("wrong::pc_is_blocked-2");
			return 0;
		}

		lua_pushboolean(L, MessengerManager::instance().IsBlocked(ch->GetName(), tch->GetName()));
		return 1;
	}

	int pc_is_friend(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong::pc_is_blocked");
			return 0;
		}

		const char* arg1 = lua_tostring(L, 1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);
		if (!tch || !tch->IsPC())
		{
			sys_err("wrong::pc_is_blocked-2");
			return 0;
		}

		lua_pushboolean(L, MessengerManager::instance().IsInList(ch->GetName(), tch->GetName()));
		return 1;
	}
#endif

	int pc_set_warp_location(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong map index");
			return 0;
		}

		if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("wrong coodinate");
			return 0;
		}

		ch->SetWarpLocation((long)lua_tonumber(L,1), (long)lua_tonumber(L,2), (long)lua_tonumber(L,3));
		return 0;
	}

	int pc_set_warp_location_local(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong map index");
			return 0;
		}

		if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("wrong coodinate");
			return 0;
		}

		long lMapIndex = (long) lua_tonumber(L, 1);
		const TMapRegion * region = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);

		if (!region)
		{
			sys_err("invalid map index %d", lMapIndex);
			return 0;
		}

		int x = (int) lua_tonumber(L, 2);
		int y = (int) lua_tonumber(L, 3);

		if (x > region->ex - region->sx)
		{
			sys_err("x coordinate overflow max: %d input: %d", region->ex - region->sx, x);
			return 0;
		}

		if (y > region->ey - region->sy)
		{
			sys_err("y coordinate overflow max: %d input: %d", region->ey - region->sy, y);
			return 0;
		}

		ch->SetWarpLocation(lMapIndex, x, y);
		return 0;
	}

	int pc_get_start_location(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, g_start_map[ch->GetEmpire()]);
		lua_pushnumber(L, g_start_position[ch->GetEmpire()][0] / 100);
		lua_pushnumber(L, g_start_position[ch->GetEmpire()][1] / 100);
		return 3;
	}

	int pc_warp(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			lua_pushboolean(L, false);
			return 1;
		}

		long map_index = 0;

		if (lua_isnumber(L, 3))
			map_index = (int) lua_tonumber(L,3);

		//PREVENT_HACK
		if ( ch->IsHack() )
		{
			lua_pushboolean(L, false);
			return 1;
		}
		//END_PREVENT_HACK

		if ( test_server )
			ch->ChatPacket( CHAT_TYPE_INFO, "pc_warp %d %d %d",(int)lua_tonumber(L,1),
					(int)lua_tonumber(L,2),map_index );
		ch->WarpSet((int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), map_index);

		lua_pushboolean(L, true);

		return 1;
	}

	int pc_warp_local(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("no map index argument");
			return 0;
		}

		if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("no coodinate argument");
			return 0;
		}

		long lMapIndex = (long) lua_tonumber(L, 1);
		const TMapRegion * region = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);

		if (!region)
		{
			sys_err("invalid map index %d", lMapIndex);
			return 0;
		}

		int x = (int) lua_tonumber(L, 2);
		int y = (int) lua_tonumber(L, 3);

		if (x > region->ex - region->sx)
		{
			sys_err("x coordinate overflow max: %d input: %d", region->ex - region->sx, x);
			return 0;
		}

		if (y > region->ey - region->sy)
		{
			sys_err("y coordinate overflow max: %d input: %d", region->ey - region->sy, y);
			return 0;
		}

		CQuestManager::instance().GetCurrentCharacterPtr()->WarpSet(region->sx + x, region->sy + y);
		return 0;
	}

	int pc_warp_exit(lua_State * L)
	{
		CQuestManager::instance().GetCurrentCharacterPtr()->ExitToSavedLocation();
		return 0;
	}

	int pc_in_dungeon(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#if defined(ENABLE_DUNGEON_REJOIN) || defined(ENABLE_CH_CRASH_CORE_FIX)
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, ch->GetDungeon() ? 1 : 0);
		return 1;
	}

	int pc_hasguild(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, ch->GetGuild() ? 1 : 0);
		return 1;
	}

	int pc_getguild(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetGuild() ? ch->GetGuild()->GetID() : 0);
		return 1;
	}

	int pc_isguildmaster(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		CGuild * g = ch->GetGuild();

		if (g)
			lua_pushboolean(L, (ch->GetPlayerID() == g->GetMasterPID()));
		else
			lua_pushboolean(L, 0);

		return 1;
	}

	int pc_destroy_guild(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		CGuild * g = ch->GetGuild();

		if (g)
		{
			// @fixme188 BEGIN
			if (g->UnderAnyWar())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LONCA_SAVASI_DEVAM_EDIYORKEN_LONCAYI_KAPATAMASSIN"));
				return 0;
			}
			else
			{
				// @fixme188 END
				g->RequestDisband(ch->GetPlayerID());
			}
		}
		return 0;
	}

	int pc_remove_from_guild(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		CGuild * g = ch->GetGuild();

		if (g)
		{
			if (g->UnderAnyWar())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LONCA_SAVASI_DEVAM_EDIYORKEN_LONCAYI_KAPATAMASSIN"));
				return 0;
			}
			else
				g->RequestRemoveMember(ch->GetPlayerID());
		}

		return 0;
	}

	int pc_give_gold(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST : wrong argument");
			return 0;
		}

#ifdef ENABLE_GOLD_LIMIT
		long long iAmount = (long long) lua_tonumber(L, 1);
#else
		int iAmount = (int) lua_tonumber(L, 1);
#endif

		if (iAmount <= 0)
		{
			sys_err("QUEST : gold amount less then zero");
			return 0;
		}

		DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), iAmount);
		ch->PointChange(POINT_GOLD, iAmount, true);
		return 0;
	}

#ifdef ENABLE_GAYA_SYSTEM
	int pc_give_gem(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST : wrong argument");
			return 0;
		}

		int iAmount = (int)lua_tonumber(L, 1);

		if (iAmount <= 0)
		{
			sys_err("QUEST : gold amount less then zero");
			return 0;
		}

		DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), iAmount);
		ch->PointChange(POINT_GEM, iAmount, true);
		return 0;
	}
#endif

	int pc_warp_to_guild_war_observer_position(lua_State* L)
	{
		luaL_checknumber(L, 1);
		luaL_checknumber(L, 2);

		DWORD gid1 = (DWORD)lua_tonumber(L, 1);
		DWORD gid2 = (DWORD)lua_tonumber(L, 2);

		CGuild* g1 = CGuildManager::instance().FindGuild(gid1);
		CGuild* g2 = CGuildManager::instance().FindGuild(gid2);

		if (!g1 || !g2)
		{
			luaL_error(L, "no such guild with id %d %d", gid1, gid2);
		}

		PIXEL_POSITION pos;

		DWORD dwMapIndex = g1->GetGuildWarMapIndex(gid2);

		if (!CWarMapManager::instance().GetStartPosition(dwMapIndex, 2, pos))
		{
			luaL_error(L, "not under warp guild war between guild %d %d", gid1, gid2);
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		//PREVENT_HACK
		if ( ch->IsHack() )
			return 0;
		//END_PREVENT_HACK

		ch->SetQuestFlag("war.is_war_member", 0);
		ch->SaveExitLocation();
		ch->WarpSet(pos.x, pos.y, dwMapIndex);
		return 0;
	}

	int pc_give_item_from_special_item_group(lua_State* L)
	{
		luaL_checknumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		DWORD dwGroupVnum = (DWORD) lua_tonumber(L,1);

		std::vector <DWORD> dwVnums;
		std::vector <DWORD> dwCounts;
		std::vector <LPITEM> item_gets;
		int count = 0;

		ch->GiveItemFromSpecialItemGroup(dwGroupVnum, dwVnums, dwCounts, item_gets, count);

		for (int i = 0; i < count; i++)
		{
			if (!item_gets[i])
			{
				if (dwVnums[i] == 1)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 1216, "%d", dwCounts[i]);
				}
				else if (dwVnums[i] == 2)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 121, "");
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 1290, "%d", dwCounts[i]);
				}
			}
		}
		return 0;
	}

	int pc_enough_inventory(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		DWORD item_vnum = (DWORD)lua_tonumber(L, 1);
		TItemTable * pTable = ITEM_MANAGER::instance().GetTable(item_vnum);
		if (!pTable)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		bool bEnoughInventoryForItem = ch->GetEmptyInventory(pTable->bSize) != -1;
		lua_pushboolean(L, bEnoughInventoryForItem);
		return 1;
	}

	int pc_give_item(lua_State* L)
	{
		PC* pPC = CQuestManager::instance().GetCurrentPC();
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isstring(L, 1) || !(lua_isstring(L, 2)||lua_isnumber(L, 2)))
		{
			sys_err("QUEST : wrong argument");
			return 0;
		}

		DWORD dwVnum;

		if (lua_isnumber(L,2))
			dwVnum = (int) lua_tonumber(L, 2);
		else if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, 2), dwVnum))
		{
			sys_err("QUEST Make item call error : wrong item name : %s", lua_tostring(L,1));
			return 0;
		}

		int icount = 1;

		if (lua_isnumber(L, 3) && lua_tonumber(L, 3) > 0)
		{
			icount = (int)rint(lua_tonumber(L, 3));

			if (icount <= 0)
			{
				sys_err("QUEST Make item call error : wrong item count : %g", lua_tonumber(L, 2));
				return 0;
			}
		}

		pPC->GiveItem(lua_tostring(L, 1), dwVnum, icount);

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), dwVnum, icount);
		return 0;
	}

	int pc_give_or_drop_item(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isstring(L, 1) && !lua_isnumber(L, 1))
		{
			sys_err("QUEST Make item call error : wrong argument");
			lua_pushnumber (L, 0);
			return 1;
		}

		DWORD dwVnum;

		if (lua_isnumber(L, 1))
		{
			dwVnum = (int) lua_tonumber(L, 1);
		}
		else if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, 1), dwVnum))
		{
			sys_err("QUEST Make item call error : wrong item name : %s", lua_tostring(L,1));
			lua_pushnumber (L, 0);

			return 1;
		}

		int icount = 1;
		if (lua_isnumber(L,2) && lua_tonumber(L,2)>0)
		{
			icount = (int)rint(lua_tonumber(L,2));
			if (icount<=0)
			{
				sys_err("QUEST Make item call error : wrong item count : %g", lua_tonumber(L,2));
				lua_pushnumber (L, 0);
				return 1;
			}
		}

		sys_log(0, "QUEST [REWARD] item %s to %s", lua_tostring(L, 1), ch->GetName());

		PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), dwVnum, icount);

		LPITEM item = ch->AutoGiveItem(dwVnum, icount);

		if ( item && dwVnum >= 80003 && dwVnum <= 80007 )  // FIX: item may be NULL (dead/stun or missing proto); guard GetID() deref
		{
			LogManager::instance().GoldBarLog(ch->GetPlayerID(), item->GetID(), QUEST, "quest: give_item2");
		}

		if (NULL != item)
			lua_pushnumber (L, item->GetID());
		else
			lua_pushnumber (L, 0);
		return 1;
	}

#ifdef ENABLE_DICE_SYSTEM
	int pc_give_or_drop_item_with_dice(lua_State* L)
	{
		if (!lua_isstring(L, 1) && !lua_isnumber(L, 1))
		{
			sys_err("QUEST Make item call error : wrong argument");
			lua_pushnumber(L, 0);
			return 1;
		}

		DWORD dwVnum;

		if (lua_isnumber(L, 1))
		{
			dwVnum = (int)lua_tonumber(L, 1);
		}
		else if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, 1), dwVnum))
		{
			sys_err("QUEST Make item call error : wrong item name : %s", lua_tostring(L, 1));
			lua_pushnumber(L, 0);

			return 1;
		}

		int icount = 1;
		if (lua_isnumber(L, 2) && lua_tonumber(L, 2) > 0)
		{
			icount = (int)rint(lua_tonumber(L, 2));
			if (icount <= 0)
			{
				sys_err("QUEST Make item call error : wrong item count : %g", lua_tonumber(L, 2));
				lua_pushnumber(L, 0);
				return 1;
			}
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPITEM item = ITEM_MANAGER::instance().CreateItem(dwVnum, icount);
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch || !item)
			return 0;
#endif
		if (ch->GetParty())
		{
			FPartyDropDiceRoll f(item, ch);
			f.Process(NULL);
			f.GetItemOwner()->AutoGiveItem(item);
		}
		else
			ch->AutoGiveItem(item);

		sys_log(0, "QUEST [REWARD] item %s to %s", lua_tostring(L, 1), ch->GetName());

		LogManager::instance().QuestRewardLog(CQuestManager::instance().GetCurrentPC()->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), dwVnum, icount);

		lua_pushnumber(L, (item) ? item->GetID() : 0);
		return 1;
	}
#endif

	int pc_give_or_drop_item_and_select(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isstring(L, 1) && !lua_isnumber(L, 1))
		{
			sys_err("QUEST Make item call error : wrong argument");
			return 0;
		}

		DWORD dwVnum;

		if (lua_isnumber(L, 1))
		{
			dwVnum = (int) lua_tonumber(L, 1);
		}
		else if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, 1), dwVnum))
		{
			sys_err("QUEST Make item call error : wrong item name : %s", lua_tostring(L, 1));
			return 0;
		}

		int icount = 1;
		if (lua_isnumber(L, 2) && lua_tonumber(L, 2) > 0)
		{
			icount = (int)rint(lua_tonumber(L, 2));
			if (icount <= 0)
			{
				sys_err("QUEST Make item call error : wrong item count : %g", lua_tonumber(L, 2));
				return 0;
			}
		}

		sys_log(0, "QUEST [REWARD] item %s to %s", lua_tostring(L, 1), ch->GetName());

		PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), dwVnum, icount);

		LPITEM item = ch->AutoGiveItem(dwVnum, icount);

		if (NULL != item)
			CQuestManager::Instance().SetCurrentItem(item);

		if ( item && dwVnum >= 80003 && dwVnum <= 80007 )  // FIX: item may be NULL (dead/stun or missing proto); guard GetID() deref
		{
			LogManager::instance().GoldBarLog(ch->GetPlayerID(), item->GetID(), QUEST, "quest: give_item2");
		}

		return 0;
	}

	int pc_get_current_map_index(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetMapIndex());
		return 1;
	}

	int pc_get_x(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetX() / 100);
		return 1;
	}

	int pc_get_y(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetY() / 100);
		return 1;
	}

	int pc_get_local_x(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());

		if (pMap)
			lua_pushnumber(L, (ch->GetX() - pMap->m_setting.iBaseX) / 100);
		else
			lua_pushnumber(L, ch->GetX() / 100);

		return 1;
	}

	int pc_get_local_y(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());

		if (pMap)
			lua_pushnumber(L, (ch->GetY() - pMap->m_setting.iBaseY) / 100);
		else
			lua_pushnumber(L, ch->GetY() / 100);

		return 1;
	}

	int pc_count_item(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (lua_isnumber(L, -1))
			lua_pushnumber(L, ch->CountSpecifyItem((DWORD)lua_tonumber(L, -1)));
		else if (lua_isstring(L, -1))
		{
			DWORD item_vnum;

			if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L,1), item_vnum))
			{
				sys_err("QUEST count_item call error : wrong item name : %s", lua_tostring(L,1));
				lua_pushnumber(L, 0);
			}
			else
			{
				lua_pushnumber(L, ch->CountSpecifyItem(item_vnum));
			}
		}
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int pc_remove_item(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (lua_gettop(L) == 1)
		{
			DWORD item_vnum;

			if (lua_isnumber(L,1))
			{
				item_vnum = (DWORD)lua_tonumber(L, 1);
			}
			else if (lua_isstring(L,1))
			{
				if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L,1), item_vnum))
				{
					sys_err("QUEST remove_item call error : wrong item name : %s", lua_tostring(L,1));
					return 0;
				}
			}
			else
			{
				sys_err("QUEST remove_item wrong argument");
				return 0;
			}

			sys_log(0,"QUEST remove a item vnum %d of %s[%d]", item_vnum, CQuestManager::instance().GetCurrentCharacterPtr()->GetName(), CQuestManager::instance().GetCurrentCharacterPtr()->GetPlayerID());
			CQuestManager::instance().GetCurrentCharacterPtr()->RemoveSpecifyItem(item_vnum);
		}
		else if (lua_gettop(L) == 2)
		{
			DWORD item_vnum;

			if (lua_isnumber(L, 1))
			{
				item_vnum = (DWORD)lua_tonumber(L, 1);
			}
			else if (lua_isstring(L, 1))
			{
				if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L,1), item_vnum))
				{
					sys_err("QUEST remove_item call error : wrong item name : %s", lua_tostring(L,1));
					return 0;
				}
			}
			else
			{
				sys_err("QUEST remove_item wrong argument");
				return 0;
			}

			DWORD item_count = (DWORD) lua_tonumber(L, 2);
			sys_log(0, "QUEST remove items(vnum %d) count %d of %s[%d]",
					item_vnum,
					item_count,
					CQuestManager::instance().GetCurrentCharacterPtr()->GetName(),
					CQuestManager::instance().GetCurrentCharacterPtr()->GetPlayerID());

			CQuestManager::instance().GetCurrentCharacterPtr()->RemoveSpecifyItem(item_vnum, item_count);
		}
		return 0;
	}

	int pc_get_leadership(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetLeadershipSkillLevel());
		return 1;
	}

	int pc_reset_point(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->ResetPoint(ch->GetLevel());
		return 0;
	}

	int pc_get_playtime(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetRealPoint(POINT_PLAYTIME));
		return 1;
	}

	int pc_get_vid(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetVID());
		return 1;
	}

	int pc_get_name(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushstring(L, ch->GetName());
		return 1;
	}

	int pc_get_next_exp(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetNextExp());
		return 1;
	}

	int pc_get_exp(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetExp());
		return 1;
	}

	int pc_get_race(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetRaceNum());
		return 1;
	}

#ifdef ENABLE_COSTUME_GENDER_CHANGE_FIX
	int pc_change_sex(lua_State* L)  
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_COSTUME_BODY) || (CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_COSTUME_HAIR)) || (CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_COSTUME_MOUNT)) || (CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_COSTUME_WEAPON)))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("cinsiyet_degismek_icin_kostum_cikart"));
			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->ChangeSex());
		return 1;
	}
#else
	int pc_change_sex(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->ChangeSex());
		return 1;
	}
#endif

#ifdef ENABLE_CHANGE_SEX_QUEST
	int pc_change_sex_degisemen(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (CQuestManager::instance().GetCurrentCharacterPtr()->GetWear(WEAR_COSTUME_BODY))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("cinsiyetdegistirebilmekicinkostumunucikart"));
			lua_pushboolean(L, false);
			return 1;
		}
		return 1;
	}
#endif

	int pc_get_job(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetJob());
		return 1;
	}

	int pc_get_max_sp(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetMaxSP());
		return 1;
	}

	int pc_get_sp(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetSP());
		return 1;
	}

	int pc_change_sp(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			lua_pushboolean(L, 0);
			return 1;
		}

		long val = (long) lua_tonumber(L, 1);

		if (val == 0)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (val > 0)
			ch->PointChange(POINT_SP, val);
		else if (val < 0)
		{
			if (ch->GetSP() < -val)
			{
				lua_pushboolean(L, 0);
				return 1;
			}

			ch->PointChange(POINT_SP, val);
		}

		lua_pushboolean(L, 1);
		return 1;
	}

	int pc_get_max_hp(lua_State * L) 
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetMaxHP());
		return 1;
	}

	int pc_get_hp(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetHP());
		return 1;
	}

	int pc_get_level(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetLevel());
		return 1;
	}

	int pc_set_level(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}
		else
		{
			int newLevel = lua_tonumber(L, 1);
			LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
			if (!ch)
				return 0;
#endif
			sys_log(0,"QUEST [LEVEL] %s jumpint to level %d", ch->GetName(), (int)rint(lua_tonumber(L,1)));

			PC* pPC = CQuestManager::instance().GetCurrentPC();
			LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), newLevel, 0);

			ch->PointChange(POINT_SKILL, newLevel - ch->GetLevel());
			ch->PointChange(POINT_SUB_SKILL, newLevel < 10 ? 0 : newLevel - MAX(ch->GetLevel(), 9));
			ch->PointChange(POINT_STAT, ((MINMAX(1, newLevel, 111) - ch->GetLevel()) * 3) + ch->GetPoint(POINT_LEVEL_STEP));

#if defined(ENABLE_SAFE_LEVEL_CHANGE_FIX)
			ch->PointChange(POINT_LEVEL, newLevel - ch->GetLevel(), false, true);
#else
			ch->PointChange(POINT_LEVEL, newLevel - ch->GetLevel());
#endif
			//HP, SP
			ch->SetRandomHP((newLevel - 1) * number(JobInitialPoints[ch->GetJob()].hp_per_lv_begin, JobInitialPoints[ch->GetJob()].hp_per_lv_end));
			ch->SetRandomSP((newLevel - 1) * number(JobInitialPoints[ch->GetJob()].sp_per_lv_begin, JobInitialPoints[ch->GetJob()].sp_per_lv_end));

			ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
			ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

			ch->ComputePoints();
			ch->PointsPacket();
			ch->SkillLevelPacket();

			return 0;
		}
	}

	int pc_get_weapon(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		LPITEM item = ch->GetWear(WEAR_WEAPON);

		if (!item)
			lua_pushnumber(L, 0);
		else
			lua_pushnumber(L, item->GetVnum());

		return 1;
	}
#ifdef ENABLE_BLOCK_EXP
	int _block_exp (lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->block_exp = true;
		return 0;
	}
	int _unblock_exp (lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->block_exp = false;
		return 0;
	}  

	int _getexp_blocked (lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushboolean(L, ch->block_exp);
		return 1;
	}  
#endif

	int pc_get_armor(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		LPITEM item = ch->GetWear(WEAR_BODY);

		if (!item)
			lua_pushnumber(L, 0);
		else
			lua_pushnumber(L, item->GetVnum());

		return 1;
	}

	int pc_get_wear(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("QUEST wrong set flag");
			return 0;
		}

		UINT bCell = lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		LPITEM item = ch->GetWear(bCell);

		if (!item)
			lua_pushnil(L);
		else
			lua_pushnumber(L, item->GetVnum());

		return 1;
	}

	int pc_get_money(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetGold());
		return 1;
	}

#ifdef ENABLE_GAYA_SYSTEM
	int pc_get_gem(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetGem());
		return 1;
	}
#endif

	int pc_get_real_alignment(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetRealAlignment() / 10);
		return 1;
	}

	int pc_get_alignment(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetAlignment() / 10);
		return 1;
	}

	int pc_change_alignment(lua_State * L)
	{
		int alignment = (int)(lua_tonumber(L, 1)*10);
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->UpdateAlignment(alignment);
		return 0;
	}

	int pc_change_money(lua_State * L)
	{
#ifdef ENABLE_GOLD_LIMIT
		long long gold = (long long)lua_tonumber(L, -1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (gold + ch->GetGold() < 0)
			sys_err("QUEST wrong ChangeGold %lld (now %lld)", gold, ch->GetGold());
#else
		int gold = (int)lua_tonumber(L, -1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (gold + ch->GetGold() < 0)
			sys_err("QUEST wrong ChangeGold %d (now %d)", gold, ch->GetGold());
#endif
		else
		{
			DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), gold);
#ifdef ENABLE_CH_CRASH_CORE_FIX
			ch->PointChange(POINT_GOLD, static_cast<long long>(gold), true);
#else
			ch->PointChange(POINT_GOLD, gold, true);
#endif
		}

		return 0;
	}

#ifdef ENABLE_GAYA_SYSTEM
	int pc_change_gem(lua_State* L)
	{
		int gem = (int)lua_tonumber(L, -1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (gem + ch->GetGem() < 0)
			sys_err("QUEST wrong ChangeGem %d (now %d)", gem, ch->GetGem());
		else
		{
			DBManager::instance().SendMoneyLog(MONEY_LOG_QUEST, ch->GetPlayerID(), gem);
			ch->PointChange(POINT_GEM, gem, true);
		}

		return 0;
	}
#endif

	int pc_set_another_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L, 1) || !lua_isstring(L, 2) || !lua_isnumber(L, 3))
		{
			sys_err("QUEST wrong set flag");
			return 0;
		}
		else
		{
			const char * sz = lua_tostring(L, 1);
			const char * sz2 = lua_tostring(L, 2);
			CQuestManager & q = CQuestManager::Instance();
			PC * pPC = q.GetCurrentPC();
			pPC->SetFlag(string(sz)+"."+sz2, int(rint(lua_tonumber(L,3))));
			return 0;
		}
	}

	int pc_get_another_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isstring(L,2))
		{
			sys_err("QUEST wrong get flag");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L,1);
			const char* sz2 = lua_tostring(L,2);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			if (!pPC)
			{
				return 0;
			}
			lua_pushnumber(L,pPC->GetFlag(string(sz)+"."+sz2));
			return 1;
		}
	}

	int pc_get_flag(lua_State* L)
	{
		if (!lua_isstring(L,-1))
		{
			sys_err("QUEST wrong get flag");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L,-1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			lua_pushnumber(L,pPC->GetFlag(sz));
			return 1;
		}
	}

	int pc_get_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L,-1))
		{
			sys_err("QUEST wrong get flag");
			return 0;
		}
		else
		{
			const char* sz = lua_tostring(L,-1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			lua_pushnumber(L,pPC->GetFlag(pPC->GetCurrentQuestName() + "."+sz));
			if ( test_server )
				sys_log( 0 ,"GetQF ( %s . %s )", pPC->GetCurrentQuestName().c_str(), sz );
		}
		return 1;
	}

	int pc_set_flag(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
		{
			sys_err("QUEST wrong set flag");
		}
		else
		{
			const char* sz = lua_tostring(L,1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			pPC->SetFlag(sz, int(rint(lua_tonumber(L,2))));
		}
		return 0;
	}

	int pc_set_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
		{
			sys_err("QUEST wrong set flag");
		}
		else
		{
			const char* sz = lua_tostring(L,1);
			CQuestManager& q = CQuestManager::Instance();
			PC* pPC = q.GetCurrentPC();
			pPC->SetFlag(pPC->GetCurrentQuestName()+"."+sz, int(rint(lua_tonumber(L,2))));
		}
		return 0;
	}

	int pc_del_quest_flag(lua_State *L)
	{
		if (!lua_isstring(L, 1))
		{
			sys_err("argument error");
			return 0;
		}

		const char * sz = lua_tostring(L, 1);
		PC * pPC = CQuestManager::instance().GetCurrentPC();
		pPC->DeleteFlag(pPC->GetCurrentQuestName()+"."+sz);
		return 0;
	}

	int pc_give_exp2(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1))
			return 0;

		sys_log(0,"QUEST [REWARD] %s give exp2 %d", ch->GetName(), (int)rint(lua_tonumber(L,1)));

		DWORD exp = (DWORD)rint(lua_tonumber(L,1));

		PC* pPC = CQuestManager::instance().GetCurrentPC();
		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), exp, 0);
		ch->PointChange(POINT_EXP, exp);
		return 0;
	}

	int pc_give_exp(lua_State* L)
	{
		if (!lua_isstring(L,1) || !lua_isnumber(L,2))
			return 0;

		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		sys_log(0, "QUEST [REWARD] %s give exp %s %d", ch->GetName(), lua_tostring(L, 1), (int)rint(lua_tonumber(L, 2)));

		DWORD exp = (DWORD)rint(lua_tonumber(L, 2));

		PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), exp, 0);

		pPC->GiveExp(lua_tostring(L,1), exp);
		return 0;
	}

	int pc_give_exp_perc(lua_State* L)
	{
		CQuestManager & q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();

		if (!ch || !lua_isstring(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3))
			return 0;

		int lev = (int)rint(lua_tonumber(L,2));
		double proc = (lua_tonumber(L,3));

		sys_log(0, "QUEST [REWARD] %s give exp %s lev %d percent %g%%", ch->GetName(), lua_tostring(L, 1), lev, proc);

		DWORD exp = (DWORD)((exp_table[MINMAX(0, lev, PLAYER_EXP_TABLE_MAX)] * proc) / 100);
		PC * pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), exp, 0);

		pPC->GiveExp(lua_tostring(L, 1), exp);
		return 0;
	}

	int pc_get_empire(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetEmpire());
		return 1;
	}

	int pc_get_part(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1))
		{
			lua_pushnumber(L, 0);
			return 1;
		}
		int part_idx = (int)lua_tonumber(L, 1);
		lua_pushnumber(L, ch->GetPart(part_idx));
		return 1;
	}

	int pc_set_part(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			return 0;
		}
		int part_idx = (int)lua_tonumber(L, 1);
		int part_value = (int)lua_tonumber(L, 2);
		ch->SetPart(part_idx, part_value);
		ch->UpdatePacket();
		return 0;
	}

	int pc_get_skillgroup(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetSkillGroup());
		return 1;
	}

#ifdef ENABLE_WARRIOR_SKILL_RESET_FIX
	int pc_set_skillgroup(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
			sys_err("QUEST wrong skillgroup number");
		else
		{
			CQuestManager & q = CQuestManager::Instance();
			LPCHARACTER ch = q.GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
			if (!ch)
				return 0;
#endif
#ifdef ENABLE_CHANGE_SKILL_VISUAL_BUG_FIX
			ch->RemoveGoodAffect();
#endif
			ch->SetSkillGroup((BYTE) rint(lua_tonumber(L, 1)));
		}
		return 0;
	}
#else
	int pc_set_skillgroup(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
			sys_err("QUEST wrong skillgroup number");
		else
		{
			CQuestManager & q = CQuestManager::Instance();
			LPCHARACTER ch = q.GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
			if (!ch)
				return 0;
#endif
			ch->RemoveGoodAffect();// @fixme184
			ch->SetSkillGroup((BYTE) rint(lua_tonumber(L, 1)));
		}
		return 0;
	}
#endif

	int pc_is_polymorphed(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, ch->IsPolymorphed());
		return 1;
	}

	int pc_remove_polymorph(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->RemoveAffect(AFFECT_POLYMORPH);
		ch->SetPolymorph(0);
		return 0;
	}

	int pc_polymorph(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		DWORD dwVnum = (DWORD)lua_tonumber(L, 1);
		int iDuration = (int)lua_tonumber(L, 2);
		ch->AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
		return 0;
	}

	int pc_is_mount(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, ch->GetMountVnum());
		return 1;
	}

	int pc_mount(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
			return 0;

		int length = 60;

		if (lua_isnumber(L, 2))
			length = (int)lua_tonumber(L, 2);

		DWORD mount_vnum = (DWORD)lua_tonumber(L, 1);

		if (length < 0)
			length = 60;

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		// If the horse is summoned and followed, remove the horse from the beginning.
		if (ch->GetHorse())
			ch->HorseSummon(false);

		if (mount_vnum)
		{
			ch->AddAffect(AFFECT_MOUNT, POINT_MOUNT, mount_vnum, AFF_NONE, length, 0, true);
			switch (mount_vnum)
			{
				case 20201:
				case 20202:
				case 20203:
				case 20204:
				case 20213:
				case 20216:
					ch->AddAffect(AFFECT_MOUNT, POINT_MOV_SPEED, 30, AFF_NONE, length, 0, true, true);
				break;

				case 20205:
				case 20206:
				case 20207:
				case 20208:
				case 20214:
				case 20217:
					ch->AddAffect(AFFECT_MOUNT, POINT_MOV_SPEED, 40, AFF_NONE, length, 0, true, true);
				break;

				case 20209:
				case 20210:
				case 20211:
				case 20212:
				case 20215:
				case 20218:
					ch->AddAffect(AFFECT_MOUNT, POINT_MOV_SPEED, 50, AFF_NONE, length, 0, true, true);
				break;
			}
		}

		return 0;
	}

	int pc_mount_bonus(lua_State* L)
	{
		BYTE applyOn = static_cast<BYTE>(lua_tonumber(L, 1));
		long value = static_cast<long>(lua_tonumber(L, 2));
		long duration = static_cast<long>(lua_tonumber(L, 3));

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != ch)
		{
			// @fixme134
			if (!ch->GetMountVnum())
				return 0;
			ch->RemoveAffect(AFFECT_MOUNT_BONUS);
			ch->AddAffect(AFFECT_MOUNT_BONUS, aApplyInfo[applyOn].bPointType, value, AFF_NONE, duration, 0, false);
		}

		return 0;
	}

	int pc_unmount(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);
		if (ch->IsHorseRiding())
			ch->StopRiding();
		return 0;
	}

	int pc_get_horse_level(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetHorseLevel());
		return 1;
	}

	int pc_get_horse_hp(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (ch->GetHorseLevel())
			lua_pushnumber(L, ch->GetHorseHealth());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int pc_get_horse_stamina(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (ch->GetHorseLevel())
			lua_pushnumber(L, ch->GetHorseStamina());
		else
			lua_pushnumber(L, 0);

		return 1;
	}

	int pc_is_horse_alive(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, ch->GetHorseLevel() > 0 && ch->GetHorseHealth() > 0);
		return 1;
	}

	int pc_revive_horse(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->ReviveHorse();
		return 0;
	}

	int pc_have_map_scroll(lua_State* L)
	{
		if (!lua_isstring(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		const char * szMapName = lua_tostring(L, 1);
		const TMapRegion * region = SECTREE_MANAGER::instance().FindRegionByPartialName(szMapName);

		if (!region)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		bool bFind = false;
		for (int iCell = 0; iCell < INVENTORY_MAX_NUM; iCell++)
		{
			LPITEM item = ch->GetInventoryItem(iCell);
			if (!item)
				continue;

			if (item->GetType() == ITEM_USE &&
					item->GetSubType() == USE_TALISMAN &&
					(item->GetValue(0) == 1 || item->GetValue(0) == 2))
			{
				int x = item->GetSocket(0);
				int y = item->GetSocket(1);

				if (region->sx <=x && region->sy <= y && x <= region->ex && y <= region->ey)
				{
					bFind = true;
					break;
				}
			}
		}

		lua_pushboolean(L, bFind);
		return 1;
	}

	int pc_get_war_map(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetWarMap() ? ch->GetWarMap()->GetMapIndex() : 0);
		return 1;
	}

	int pc_have_pos_scroll(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			sys_err("invalid x y position");
			lua_pushboolean(L, 0);
			return 1;
		}

		if (!lua_isnumber(L, 2))
		{
			sys_err("invalid radius");
			lua_pushboolean(L, 0);
			return 1;
		}

		int x = (int)lua_tonumber(L, 1);
		int y = (int)lua_tonumber(L, 2);
		float r = (float)lua_tonumber(L, 3);

		bool bFind = false;
		for (int iCell = 0; iCell < INVENTORY_MAX_NUM; iCell++)
		{
			LPITEM item = ch->GetInventoryItem(iCell);
			if (!item)
				continue;

			if (item->GetType() == ITEM_USE &&
					item->GetSubType() == USE_TALISMAN &&
					(item->GetValue(0) == 1 || item->GetValue(0) == 2))
			{
				int item_x = item->GetSocket(0);
				int item_y = item->GetSocket(1);
				if ((x-item_x)*(x-item_x)+(y-item_y)*(y-item_y)<r*r)
				{
					bFind = true;
					break;
				}
			}
		}

		lua_pushboolean(L, bFind);
		return 1;
	}

	int pc_get_equip_refine_level(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		int cell = (int)lua_tonumber(L, 1);
		if (cell < 0 || cell >= WEAR_MAX_NUM)
		{
			sys_err("invalid wear position %d", cell);
			lua_pushnumber(L, 0);
			return 1;
		}

		LPITEM item = ch->GetWear(cell);
		if (!item)
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		lua_pushnumber(L, item->GetRefineLevel());
		return 1;
	}

	int pc_refine_equip(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			sys_err("invalid argument");
			lua_pushboolean(L, 0);
			return 1;
		}

		int cell = (int) lua_tonumber(L, 1);
		int level_limit = (int) lua_tonumber(L, 2);
		int pct = lua_isnumber(L, 3) ? (int)lua_tonumber(L, 3) : 100;

		LPITEM item = ch->GetWear(cell);
		if (!item)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		if (item->GetRefinedVnum() == 0)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		if (item->GetRefineLevel()>level_limit)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		if (pct == 100 || number(1, 100) <= pct)
		{
			lua_pushboolean(L, 1);

			LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(item->GetRefinedVnum(), 1, 0, false);

			if (pkNewItem)
			{
				for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					if (!item->GetSocket(i))
						break;
					else
						pkNewItem->SetSocket(i, 1);

				int set = 0;
				for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
				{
					long socket = item->GetSocket(i);
					if (socket > 2 && socket != 28960)
					{
						pkNewItem->SetSocket(set++, socket);
					}
				}

				item->CopyAttributeTo(pkNewItem);

				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

				pkNewItem->EquipTo(ch, cell);

				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

				LogManager::instance().ItemLog(ch, pkNewItem, "REFINE SUCCESS (QUEST)", pkNewItem->GetName());
			}
		}
		else
		{
			lua_pushboolean(L, 0);
		}

		return 1;
	}

	int pc_get_skill_level(lua_State * L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			lua_pushnumber(L, 0);
			return 1;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		DWORD dwVnum = (DWORD)lua_tonumber(L, 1);
		lua_pushnumber(L, ch->GetSkillLevel(dwVnum));

		return 1;
	}

	int pc_give_lotto(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif

		sys_log(0, "TRY GIVE LOTTO TO pid %u", ch->GetPlayerID());

		DWORD * pdw = M2_NEW DWORD[3];

		pdw[0] = 50001;
		pdw[1] = 1;
		pdw[2] = q.GetEventFlag("lotto_round");

		DBManager::instance().ReturnQuery(QID_LOTTO, ch->GetPlayerID(), pdw,
				"INSERT INTO lotto_list VALUES(0, 'server%s', %u, NOW())",
				get_table_postfix(), ch->GetPlayerID());

		return 0;
	}

	int pc_aggregate_monster(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->AggregateMonster();
		//ch->AttractRanger();
		return 0;
	}

	int pc_forget_my_attacker(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->ForgetMyAttacker();
		return 0;
	}

	int pc_attract_ranger(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->AttractRanger();
		return 0;
	}

	int pc_select_pid(lua_State* L)
	{
		DWORD pid = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		LPCHARACTER new_ch = CHARACTER_MANAGER::instance().FindByPID(pid);

		if (new_ch)
		{
			CQuestManager::instance().GetPC(new_ch->GetPlayerID());

			lua_pushnumber(L, ch->GetPlayerID());
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_select_vid(lua_State* L)
	{
		DWORD vid = (DWORD) lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		LPCHARACTER new_ch = CHARACTER_MANAGER::instance().Find(vid);

		if (new_ch)
		{
			CQuestManager::instance().GetPC(new_ch->GetPlayerID());

#ifdef ENABLE_QUEST_PC_SELECT_FIX
			lua_pushnumber(L, ch ? (DWORD)ch->GetVID() : 0);
#else
			lua_pushnumber(L, (DWORD)ch->GetVID());
#endif
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_sex(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, GET_SEX(ch)); /* 0==MALE, 1==FEMALE */
		return 1;
	}

#ifdef ENABLE_GET_COSTUME_QUEST
	int pc_get_costume(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (ch->GetWear(WEAR_COSTUME_BODY))
			lua_pushnumber(L, true);
		else
			lua_pushboolean(L, false);
		return 1;
	}
#endif

	int pc_is_engaged(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, marriage::CManager::instance().IsEngaged(ch->GetPlayerID()));
		return 1;
	}

	int pc_is_married(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, marriage::CManager::instance().IsMarried(ch->GetPlayerID()));
		return 1;
	}

	int pc_is_engaged_or_married(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, marriage::CManager::instance().IsEngagedOrMarried(ch->GetPlayerID()));
		return 1;
	}

	int pc_is_gm(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, ch->GetGMLevel() >= GM_HIGH_WIZARD);
		return 1;
	}

	int pc_get_gm_level(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetGMLevel());
		return 1;
	}

	int pc_mining(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch || !npc)
			return 0;
#endif
		ch->mining(npc);
		return 0;
	}

	int pc_diamond_refine(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		int cost = (int) lua_tonumber(L, 1);
		int pct = (int)lua_tonumber(L, 2);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch || !npc)
			return 0;
#endif
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		if (item)
			lua_pushboolean(L, mining::OreRefine(ch, npc, item, cost, pct, NULL));
		else
			lua_pushboolean(L, 0);

		return 1;
	}

	int pc_ore_refine(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		int cost = (int) lua_tonumber(L, 1);
		int pct = (int)lua_tonumber(L, 2);
		int metinstone_cell = (int)lua_tonumber(L, 3);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch || !npc)
			return 0;
#endif
		LPITEM item = CQuestManager::instance().GetCurrentItem();

		LPITEM metinstone_item = ch->GetInventoryItem(metinstone_cell);

		if (item && metinstone_item)
			lua_pushboolean(L, mining::OreRefine(ch, npc, item, cost, pct, metinstone_item));
		else
			lua_pushboolean(L, 0);

		return 1;
	}

	int pc_clear_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch == NULL)
			return 0;

		ch->ClearSkill();

		return 0;
	}

	int pc_clear_sub_skill(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch == NULL)
			return 0;

		ch->ClearSubSkill();

		return 0;
	}

	int pc_set_skill_point(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		int newPoint = (int)lua_tonumber(L, 1);

		ch->SetRealPoint(POINT_SKILL, newPoint);
		ch->SetPoint(POINT_SKILL, ch->GetRealPoint(POINT_SKILL));
		ch->PointChange(POINT_SKILL, 0);
		ch->ComputePoints();
		ch->PointsPacket();

		return 0;
	}

	// RESET_ONE_SKILL	
	int pc_clear_one_skill(lua_State* L)
	{
		int vnum = (int)lua_tonumber(L, 1);
		sys_log(0, "%d skill clear", vnum);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (ch == NULL)
		{
			sys_log(0, "skill clear fail");
			lua_pushnumber(L, 0);
			return 1;
		}

		sys_log(0, "%d skill clear", vnum);

		ch->ResetOneSkill(vnum);

		return 0;
	}
	// END_RESET_ONE_SKILL

	int pc_is_clear_skill_group(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, ch->GetQuestFlag("skill_group_clear.clear") == 1);

		return 1;
	}

	int pc_save_exit_location(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->SaveExitLocation();

		return 0;
	}

	int pc_teleport ( lua_State * L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		int x = 0, y = 0;

		if (lua_isnumber(L, 1))
		{
			const int TOWN_NUM = 10;
			struct warp_by_town_name
			{
				const char* name;
				DWORD x;
				DWORD y;
			}

			ws[TOWN_NUM] =
			{
				{"¿µ¾ÈÀ¾¼º",	4743,	9548},
				{"ÀÓÁö°î",		3235,	9086},
				{"ÀÚ¾çÇö",		3531,	8829},
				{"Á¶¾ÈÀ¾¼º",	638,	1664},
				{"½Â·æ°î",		1745,	1909},
				{"º¹Á¤Çö",		1455,	2400},
				{"Æò¹«À¾¼º",	9599,	2692},
				{"¹æ»ê°î",		8036,	2984},
				{"¹Ú¶óÇö",		8639,	2460},
				{"¼­ÇÑ»ê",		4350,	2143},
			};
			int idx = (int)lua_tonumber(L, 1);

			x = ws[idx].x;
			y = ws[idx].y;
			goto teleport_area;
		}

		else
		{
			const char * arg1 = lua_tostring(L, 1);

			LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

			if (!tch)
			{
				const CCI* pkCCI = P2P_MANAGER::instance().Find(arg1);

				if (pkCCI)
				{
					if (pkCCI->bChannel != g_bChannel)
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 849, "%d#%d", pkCCI->bChannel, g_bChannel);
					}
					else
					{
						PIXEL_POSITION pos;

						if (!SECTREE_MANAGER::instance().GetCenterPositionOfMap(pkCCI->lMapIndex, pos))
						{
							ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find map (index %d)", pkCCI->lMapIndex);
						}
						else
						{
							ch->ChatPacket(CHAT_TYPE_INFO, "You warp to ( %d, %d )", pos.x, pos.y);
							ch->WarpSet(pos.x, pos.y);
							lua_pushnumber(L, 1 );
						}
					}
				}
				else if (NULL == CHARACTER_MANAGER::instance().FindPC(arg1))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "There is no one by that name");
				}

				lua_pushnumber(L, 0 );

				return 1;
			}
			else
			{
				x = tch->GetX() / 100;
				y = tch->GetY() / 100;
			}
		}

teleport_area:

		x *= 100;
		y *= 100;

		ch->ChatPacket(CHAT_TYPE_INFO, "You warp to ( %d, %d )", x, y);
		ch->WarpSet(x,y);
		ch->Stop();
		lua_pushnumber(L, 1 );
		return 1;
	}

	int pc_set_skill_level(lua_State* L)
	{
		DWORD dwVnum = (DWORD)lua_tonumber(L, 1);
		BYTE byLev = (BYTE)lua_tonumber(L, 2);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->SetSkillLevel(dwVnum, byLev);

		ch->SkillLevelPacket();

		return 0;
	}

	int pc_give_polymorph_book(lua_State* L)
	{
		if ( lua_isnumber(L, 1) != true && lua_isnumber(L, 2) != true && lua_isnumber(L, 3) != true && lua_isnumber(L, 4) != true )
		{
			sys_err("Wrong Quest Function Arguments: pc_give_polymorph_book");
			return 0;
		}

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		CPolymorphUtils::instance().GiveBook(ch, (DWORD)lua_tonumber(L, 1), (DWORD)lua_tonumber(L, 2), (BYTE)lua_tonumber(L, 3), (BYTE)lua_tonumber(L, 4));

		return 0;
	}

	int pc_upgrade_polymorph_book(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPITEM pItem = CQuestManager::instance().GetCurrentItem();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch || !pItem)
			return 0;
#endif

		bool ret = CPolymorphUtils::instance().BookUpgrade(ch, pItem);

		lua_pushboolean(L, ret);

		return 1;
	}

	int pc_get_premium_remain_sec(lua_State* L)
	{
		int	remain_seconds	= 0;
		int	premium_type	= 0;
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1))
		{
			sys_err("wrong premium index (is not number)");
			return 0;
		}

		premium_type = (int)lua_tonumber(L,1);
		switch (premium_type)
		{
			case PREMIUM_EXP:
			case PREMIUM_ITEM:
			case PREMIUM_SAFEBOX:
			case PREMIUM_AUTOLOOT:
			case PREMIUM_FISH_MIND:
			case PREMIUM_MARRIAGE_FAST:
			case PREMIUM_GOLD:
				break;

			default:
				sys_err("wrong premium index %d", premium_type);
				return 0;
		}

		remain_seconds = ch->GetPremiumRemainSeconds(premium_type);

		lua_pushnumber(L, remain_seconds);
		return 1;
	}

	int pc_send_block_mode(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->SetBlockModeForce((BYTE)lua_tonumber(L, 1));

		return 0;
	}

	int pc_change_empire(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
#ifdef ENABLE_PARTY_FLAG_CHANGE_FIX
		if(ch->GetParty())
		{
			ch->ChatPacket(1, "text");
			lua_pushnumber(L, 4);
			return 0;
		}
#endif

		lua_pushnumber(L, ch->ChangeEmpire((unsigned char)lua_tonumber(L, 1)));

		return 1;
	}

	int pc_get_change_empire_count(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetChangeEmpireCount());

		return 1;
	}

	int pc_set_change_empire_count(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		ch->SetChangeEmpireCount();

		return 0;
	}

	int pc_change_name(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if ( ch->GetNewName().size() != 0 )
		{
			lua_pushnumber(L, 0);
			return 1;
		}

		if ( lua_isstring(L, 1) != true )
		{
			lua_pushnumber(L, 1);
			return 1;
		}

		const char * szName = lua_tostring(L, 1);

		if ( check_name(szName) == false )
		{
			lua_pushnumber(L, 2);
			return 1;
		}

		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "SELECT COUNT(*) FROM player%s WHERE name='%s'", get_table_postfix(), szName);
		std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery(szQuery));

		if ( pmsg->Get()->uiNumRows > 0 )
		{
			MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);

			int	count = 0;
			str_to_number(count, row[0]);

			if ( count != 0 )
			{
				lua_pushnumber(L, 3);
				return 1;
			}
		}

		DWORD pid = ch->GetPlayerID();
		db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
		db_clientdesc->Packet(&pid, sizeof(DWORD));

		/* delete messenger list */
		MessengerManager::instance().RemoveAllList(ch->GetName());

#ifdef ENABLE_MESSENGER_BLOCK
		MessengerManager::instance().RemoveAllBlockList(ch->GetName());
#endif
		/* change_name_log */
		LogManager::instance().ChangeNameLog(pid, ch->GetName(), szName, ch->GetDesc()->GetHostName());

		snprintf(szQuery, sizeof(szQuery), "UPDATE player%s SET name='%s' WHERE id=%u", get_table_postfix(), szName, pid);
		SQLMsg * msg = DBManager::instance().DirectQuery(szQuery);
		M2_DELETE(msg);

		ch->SetNewName(szName);
		lua_pushnumber(L, 4);
		return 1;
	}

	int pc_is_dead(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch != NULL)
		{
			lua_pushboolean(L, ch->IsDead());
			return 1;
		}

		lua_pushboolean(L, true);

		return 1;
	}

	int pc_reset_status(lua_State* L)
	{
		if (lua_isnumber(L, 1) == true)
		{
			int idx = (int)lua_tonumber(L, 1);

			if (idx >= 0 && idx < 4)
			{
				LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
				if (!ch)
					return 0;
#endif
				int point = POINT_NONE;
				char buf[128];

				switch ( idx )
				{
					case 0 : point = POINT_HT; break;
					case 1 : point = POINT_IQ; break;
					case 2 : point = POINT_ST; break;
					case 3 : point = POINT_DX; break;
					default : lua_pushboolean(L, false); return 1;
				}

				int old_val = ch->GetRealPoint(point);
				int old_stat = ch->GetRealPoint(POINT_STAT);

				ch->SetRealPoint(point, 1);
				ch->SetPoint(point, ch->GetRealPoint(point));

				ch->PointChange(POINT_STAT, old_val-1);

				if ( point == POINT_HT )
				{
					BYTE job = ch->GetJob();
					ch->SetRandomHP((ch->GetLevel()-1) * number(JobInitialPoints[job].hp_per_lv_begin, JobInitialPoints[job].hp_per_lv_end));
				}
				else if ( point == POINT_IQ )
				{
					BYTE job = ch->GetJob();
					ch->SetRandomSP((ch->GetLevel()-1) * number(JobInitialPoints[job].sp_per_lv_begin, JobInitialPoints[job].sp_per_lv_end));
				}

				ch->ComputePoints();
				ch->PointsPacket();

				if ( point == POINT_HT )
				{
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				}
				else if ( point == POINT_IQ )
				{
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				}

				switch ( idx )
				{
					case 0 :
						snprintf(buf, sizeof(buf), "reset ht(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
					case 1 :
						snprintf(buf, sizeof(buf), "reset iq(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
					case 2 :
						snprintf(buf, sizeof(buf), "reset st(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
					case 3 :
						snprintf(buf, sizeof(buf), "reset dx(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_STAT));
						break;
				}

				LogManager::instance().CharLog(ch, 0, "RESET_ONE_STATUS", buf);

				lua_pushboolean(L, true);
				return 1;
			}
		}

		lua_pushboolean(L, false);
		return 1;
	}

	int pc_get_ht(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetRealPoint(POINT_HT));
		return 1;
	}

	int pc_set_ht(lua_State* L)
	{
		if (lua_isnumber(L, 1) == false)
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		int usedPoint = newPoint - ch->GetRealPoint(POINT_HT);
		ch->SetRealPoint(POINT_HT, newPoint);
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_get_iq(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetRealPoint(POINT_IQ));
		return 1;
	}

	int pc_set_iq(lua_State* L)
	{
		if ( lua_isnumber(L, 1) == false )
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		int usedPoint = newPoint - ch->GetRealPoint(POINT_IQ);
		ch->SetRealPoint(POINT_IQ, newPoint);
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_get_st(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetRealPoint(POINT_ST));
		return 1;
	}

	int pc_set_st(lua_State* L)
	{
		if (lua_isnumber(L, 1) == false)
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		int usedPoint = newPoint - ch->GetRealPoint(POINT_ST);
		ch->SetRealPoint(POINT_ST, newPoint);
		ch->PointChange(POINT_ST, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_get_dx( lua_State* L )
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushnumber(L, ch->GetRealPoint(POINT_DX));
		return 1;
	}

#ifdef ENABLE_GUILD_RANK_SYSTEM
	int32_t pc_open_guild_ranking(lua_State* L)
	{
		LPCHARACTER pkChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!pkChar || !pkChar->GetDesc())
			return 0;

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT guild.id, guild.name AS guild_name, player.name AS guild_owner, guild.level, guild.win, guild.draw, guild.loss, guild.ladder_point FROM player.guild INNER JOIN player.player ON player.id = guild.master ORDER BY guild.level DESC, guild.win DESC, guild.draw DESC, guild.ladder_point DESC, guild.loss DESC LIMIT %d", GUILD_RANK::GUILD_RANKING_MAX_NUM));
		if (pMsg->uiSQLErrno)
			return 0;

		MYSQL_ROW row;
		while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			const int32_t ID = std::stoi(row[0]);
			const char* GuildName = row[1];
			const char* OwnerName = row[2];
			const int32_t Level = std::stoi(row[3]);
			const int32_t Win = std::stoi(row[4]);
			const int32_t Draw = std::stoi(row[5]);
			const int32_t Loss = std::stoi(row[6]);
			const int32_t Point = std::stoi(row[7]);

			const TPacketGCGuildRankingSend t = TPacketGCGuildRankingSend(ID, GuildName, OwnerName, Level, Point, Win, Draw, Loss);
			pkChar->GetDesc()->Packet(&t, sizeof(t));
		}

		const TPacketGCGuildRankingSend t = TPacketGCGuildRankingSend();
		pkChar->GetDesc()->Packet(&t, sizeof(t));

		return 0;
	}
#endif

	int pc_set_dx(lua_State* L)
	{
		if (lua_isnumber(L, 1) == false)
			return 1;

		int newPoint = (int)lua_tonumber(L, 1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		int usedPoint = newPoint - ch->GetRealPoint(POINT_DX);
		ch->SetRealPoint(POINT_DX, newPoint);
		ch->PointChange(POINT_DX, 0);
		ch->PointChange(POINT_STAT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_is_near_vid(lua_State* L)
	{
		if ( lua_isnumber(L, 1) != true || lua_isnumber(L, 2) != true )
		{
			lua_pushboolean(L, false);
		}
		else
		{
			LPCHARACTER pMe = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
			if (!pMe)
				return 0;
#endif
			LPCHARACTER pOther = CHARACTER_MANAGER::instance().Find((DWORD)lua_tonumber(L, 1));

			if ( pMe != NULL && pOther != NULL )
			{
				lua_pushboolean(L, (DISTANCE_APPROX(pMe->GetX() - pOther->GetX(), pMe->GetY() - pOther->GetY()) < (int)lua_tonumber(L, 2)*100));
			}
			else
			{
				lua_pushboolean(L, false);
			}
		}

		return 1;
	}

	int pc_get_socket_items( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_newtable( L );

		if ( pChar == NULL ) return 1;

		int idx = 1;

		for ( int i=0; i < INVENTORY_MAX_NUM + WEAR_MAX_NUM; i++ )
		{
			LPITEM pItem = pChar->GetInventoryItem(i);

			if ( pItem != NULL )
			{
				if ( pItem->IsEquipped() == false )
				{
					int j = 0;
					for (; j < ITEM_SOCKET_MAX_NUM; j++ )
					{
						long socket = pItem->GetSocket(j);

						if ( socket > 2 && socket != ITEM_BROKEN_METIN_VNUM )
						{
							TItemTable* pItemInfo = ITEM_MANAGER::instance().GetTable( socket );
							if ( pItemInfo != NULL )
							{
								if ( pItemInfo->bType == ITEM_METIN ) break;
							}
						}
					}

					if ( j >= ITEM_SOCKET_MAX_NUM ) continue;

					lua_newtable( L );

					{
						lua_pushstring( L, pItem->GetName() );
						lua_rawseti( L, -2, 1 );

						lua_pushnumber( L, i );
						lua_rawseti( L, -2, 2 );
					}

					lua_rawseti( L, -2, idx++ );
				}
			}
		}

		return 1;
	}

	int pc_get_empty_inventory_count( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			lua_pushnumber(L, pChar->CountEmptyInventory());
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_logoff_interval( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			lua_pushnumber(L, pChar->GetLogOffInterval());
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_player_id( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			lua_pushnumber( L, pChar->GetPlayerID() );
		}
		else
		{
			lua_pushnumber( L, 0 );
		}

		return 1;
	}

	int pc_get_account_id( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if ( pChar != NULL )
		{
			if ( pChar->GetDesc() != NULL )
			{
				lua_pushnumber( L, pChar->GetDesc()->GetAccountTable().id );
				return 1;
			}
		}

		lua_pushnumber( L, 0 );
		return 1;
	}

	int pc_get_account( lua_State* L )
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( NULL != pChar )
		{
			if( NULL != pChar->GetDesc() )
			{
				lua_pushstring( L, pChar->GetDesc()->GetAccountTable().login );
				return 1;
			}
		}

		lua_pushstring( L, "" );
		return 1;
	}

	int pc_is_riding(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( NULL != pChar )
		{
			bool is_riding = pChar->IsRiding();

			lua_pushboolean(L, is_riding);

			return 1;
		}

		lua_pushboolean(L, false);
		return 1;
	}

	int pc_get_special_ride_vnum(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			LPITEM Unique1 = pChar->GetWear(WEAR_UNIQUE1);
			LPITEM Unique2 = pChar->GetWear(WEAR_UNIQUE2);
#ifdef ENABLE_MOUNT_SYSTEM
			LPITEM Unique3 = pChar->GetWear(WEAR_MOUNT);
#endif

			if (NULL != Unique1)
			{
				if (UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup())
				{
					lua_pushnumber(L, Unique1->GetVnum());
					lua_pushnumber(L, Unique1->GetSocket(2));
					return 2;
				}
			}

			if (NULL != Unique2)
			{
				if (UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup())
				{
					lua_pushnumber(L, Unique2->GetVnum());
					lua_pushnumber(L, Unique2->GetSocket(2));
					return 2;
				}
			}

#ifdef ENABLE_MOUNT_SYSTEM
			if (NULL != Unique3)
			{
				if (UNIQUE_GROUP_SPECIAL_RIDE == Unique3->GetSpecialGroup())
				{
					lua_pushnumber(L, Unique3->GetVnum());
					lua_pushnumber(L, Unique3->GetSocket(2));
					return 2;
				}
			}
#endif
		}

		lua_pushnumber(L, 0);
		lua_pushnumber(L, 0);

		return 2;
	}

	int pc_can_warp(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			lua_pushboolean(L, pChar->CanWarp());
		}
		else
		{
			lua_pushboolean(L, false);
		}

		return 1;
	}

	int pc_dec_skill_point(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			pChar->PointChange(POINT_SKILL, -1);
		}

		return 0;
	}

	int pc_get_skill_point(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL != pChar)
		{
			lua_pushnumber(L, pChar->GetPoint(POINT_SKILL));
		}
		else
		{
			lua_pushnumber(L, 0);
		}

		return 1;
	}

	int pc_get_channel_id(lua_State* L)
	{
		lua_pushnumber(L, g_bChannel);

		return 1;
	}

	int pc_give_poly_marble(lua_State* L)
	{
		const int dwVnum = lua_tonumber(L, 1);

		const CMob* MobInfo = CMobManager::instance().Get(dwVnum);

		if (NULL == MobInfo)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		if (0 == MobInfo->m_table.dwPolymorphItemVnum)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		LPITEM item = ITEM_MANAGER::instance().CreateItem( MobInfo->m_table.dwPolymorphItemVnum );

		if (NULL == item)
		{
			lua_pushboolean(L, false);
			return 1;
		}

		item->SetSocket(0, dwVnum);

		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		int iEmptyCell = ch->GetEmptyInventory(item->GetSize());

		if (-1 == iEmptyCell)
		{
			M2_DESTROY_ITEM(item);
			lua_pushboolean(L, false);
			return 1;
		}

		item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyCell));

		const PC* pPC = CQuestManager::instance().GetCurrentPC();

		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetLevel(), MobInfo->m_table.dwPolymorphItemVnum, dwVnum);

		lua_pushboolean(L, true);

		return 1;
	}

	int pc_get_sig_items (lua_State* L)
	{
		DWORD group_vnum = (DWORD)lua_tonumber (L, 1);
		const LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		int count = 0;
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			if (ch->GetInventoryItem(i) != NULL && ch->GetInventoryItem(i)->GetSIGVnum() == group_vnum)
			{
				lua_pushnumber(L, ch->GetInventoryItem(i)->GetID());
				count++;
			}
		}

		return count;
	}

	int pc_charge_cash(lua_State * L)
	{
		TRequestChargeCash packet;

		int amount = lua_isnumber(L, 1) ? (int)lua_tonumber(L, 1) : 0;
		std::string strChargeType = lua_isstring(L, 2) ? lua_tostring(L, 2) : "";

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (NULL == ch || NULL == ch->GetDesc() || 1 > amount || 50000 < amount)
		{
			lua_pushboolean(L, 0);
			return 1;
		}

		packet.dwAID = ch->GetDesc()->GetAccountTable().id;
		packet.dwAmount = (DWORD)amount;
		packet.eChargeType = ERequestCharge_Cash;

		if (0 < strChargeType.length())
			std::transform(strChargeType.begin(), strChargeType.end(), strChargeType.begin(), (int(*)(int))std::tolower);

		if ("mileage" == strChargeType)
			packet.eChargeType = ERequestCharge_Mileage;

		db_clientdesc->DBPacketHeader(HEADER_GD_REQUEST_CHARGE_CASH, 0, sizeof(TRequestChargeCash));
		db_clientdesc->Packet(&packet, sizeof(packet));

		lua_pushboolean(L, 1);
		return 1;
	}

	int pc_give_award(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3))
		{
			sys_err("QUEST give award call error : wrong argument");
			lua_pushnumber (L, 0);
			return 1;
		}

		DWORD dwVnum = (int) lua_tonumber(L, 1);

		int icount = (int) lua_tonumber(L, 2);

		sys_log(0, "QUEST [award] item %d to login %s", dwVnum, ch->GetDesc()->GetAccountTable().login);

		DBManager::instance().Query("INSERT INTO item_award (login, vnum, count, given_time, why, mall)select '%s', %d, %d, now(), '%s', 1 from DUAL where not exists (select login, why from item_award where login = '%s' and why  = '%s') ;",
			ch->GetDesc()->GetAccountTable().login,
			dwVnum,
			icount,
			lua_tostring(L,3),
			ch->GetDesc()->GetAccountTable().login,
			lua_tostring(L,3));

		lua_pushnumber (L, 0);
		return 1;
	}

	int pc_give_award_socket(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isstring(L, 3) || !lua_isstring(L, 4) || !lua_isstring(L, 5) || !lua_isstring(L, 6))
		{
			sys_err("QUEST give award call error : wrong argument");
			lua_pushnumber (L, 0);
			return 1;
		}

		DWORD dwVnum = (int) lua_tonumber(L, 1);

		int icount = (int) lua_tonumber(L, 2);

		sys_log(0, "QUEST [award] item %d to login %s", dwVnum, ch->GetDesc()->GetAccountTable().login);
		DBManager::instance().Query("INSERT INTO item_award (login, vnum, count, given_time, why, mall, socket0, socket1, socket2)select '%s', %d, %d, now(), '%s', 1, %s, %s, %s from DUAL where not exists (select login, why from item_award where login = '%s' and why  = '%s') ;",
			ch->GetDesc()->GetAccountTable().login,
			dwVnum,
			icount,
			lua_tostring(L, 3),
			lua_tostring(L, 4),
			lua_tostring(L, 5),
			lua_tostring(L, 6),
			ch->GetDesc()->GetAccountTable().login,
			lua_tostring(L, 3));

		lua_pushnumber(L, 0);
		return 1;
	}

	int pc_get_informer_type(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( pChar != NULL )
		{
			lua_pushstring(L, pChar->GetItemAward_cmd() );
		}
		else
			lua_pushstring(L, "" );

		return 1;
	}

	int pc_get_informer_item(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();

		if( pChar != NULL )
		{
			lua_pushnumber(L, pChar->GetItemAward_vnum() );
		}
		else
			lua_pushnumber(L,0);

		return 1;
	}

	int pc_get_killee_drop_pct(lua_State* L)
	{
		LPCHARACTER pChar = CQuestManager::instance().GetCurrentCharacterPtr();
		LPCHARACTER pKillee = pChar->GetQuestNPC();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!pChar || !pKillee)
			return 0;
#endif
		int iDeltaPercent, iRandRange;
		if (NULL == pKillee || !ITEM_MANAGER::instance().GetDropPct(pKillee, pChar, iDeltaPercent, iRandRange))
		{
			sys_err("killee is null");
			lua_pushnumber(L, -1);
			lua_pushnumber(L, -1);

			return 2;
		}

		lua_pushnumber(L, iDeltaPercent);
		lua_pushnumber(L, iRandRange);

		return 2;
	}

#ifdef ENABLE_NEWSTUFF

	#define PC_MI0L_ARG1	2		// 1: vnum or locale_name, 2: count
	#define PC_MI0L_ARG2	3		// socket 1-2-3
	#define PC_MI0L_ARG3	7*2		// (type, value)*7
	enum eMakeItemType{PCMI0_GIVE, PCMI0_DROP, PCMI0_DROPWP, PCMI0_MAX};
	int pc_make_item0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (!lua_istable(L, 1) && !lua_istable(L, 2) && !lua_istable(L, 3) && !lua_isnumber(L, 4))
			return 0;

		int m_idx = 0;
		// config arg1
		DWORD m_vnum = 0;
		int m_count = 0;
		// start arg1
		lua_pushnil(L);
		while (lua_next(L, 1))
		{
			switch(m_idx)
			{
				case 0:
					if (lua_isnumber(L, -1))
					{
						if ((m_vnum = lua_tonumber(L, -1))<=0)
						{
							ch->ChatPacket(CHAT_TYPE_INFO, "#%u item not valid by that vnum.", m_vnum);
							return 0;
						}
					}
					else if (lua_isstring(L, -1))
					{
						if (!ITEM_MANAGER::instance().GetVnum(lua_tostring(L, -1), m_vnum))
						{
							ch->ChatPacket(CHAT_TYPE_INFO, "#%u item not exist by that vnum.", m_vnum);
							return 0;
						}
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "item not exist by that unknown vnum.");
						return 0;
					}
					break;
				case 1:
					if (lua_isnumber(L, -1))
					{
						// if ((m_count = MINMAX(1, lua_tonumber(L, -1), ITEM_MAX_COUNT))<=0)
						if ((m_count = lua_tonumber(L, -1))<=0)
						{
							ch->ChatPacket(CHAT_TYPE_INFO, "#%u item not valid by that count.", m_count);
							return 0;
						}
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "item not exist by that unknown count.");
						return 0;
					}
					break;
				default:
					ch->ChatPacket(CHAT_TYPE_INFO, "arg1(%d) index found", m_idx);
					break;
			}
			m_idx++;
			lua_pop(L, 1);
		}
		// ch->ChatPacket(CHAT_TYPE_INFO, "arg1 is %u %d", m_vnum, m_count);
		// end arg1
		// config arg2
		int m_socket[ITEM_SOCKET_MAX_NUM] = {0};
		// start arg2
		// ch->ChatPacket(CHAT_TYPE_INFO, "%d: %s", lua_type(L, 1), lua_typename(L, lua_type(L, 1)));
		m_idx = 0;
		lua_pushnil(L);
		while (lua_next(L, 2) && m_idx<ITEM_SOCKET_MAX_NUM)
		{
			if (!lua_isnumber(L, -1))
				return 0;
			m_socket[m_idx++] = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		// ch->ChatPacket(CHAT_TYPE_INFO, "arg2 is %d %d %d", m_socket[0], m_socket[1], m_socket[2]);
		// end arg2
		// config arg3
		int m_attr[ITEM_ATTRIBUTE_MAX_NUM*2] = {0};
		// start arg3
		m_idx = 0;
		lua_pushnil(L);
		while (lua_next(L, 3) && m_idx<(ITEM_ATTRIBUTE_MAX_NUM*2))
		{
			if (!lua_isnumber(L, -1))
				return 0;
			m_attr[m_idx++] = lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		// ch->ChatPacket(CHAT_TYPE_INFO, "arg3 is %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
			// m_attr[0], m_attr[1], m_attr[2], m_attr[3], m_attr[4], m_attr[5],
			// m_attr[6], m_attr[7], m_attr[8], m_attr[9], m_attr[10], m_attr[11],
			// m_attr[12], m_attr[13]
		// );
		// end arg3
		// config arg4
		DWORD m_state = 0;
		// start arg4
		if ((m_state = lua_tonumber(L, 4))>=PCMI0_MAX)
			return 0;
		// ch->ChatPacket(CHAT_TYPE_INFO, "arg4 is %d", m_state);
		// end arg4

		// create item
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(m_vnum, 1, 0, false);
		if (pkNewItem)
		{
			// socket
			for (int i=0; i<ITEM_SOCKET_MAX_NUM; i++)
				pkNewItem->SetSocket(i, m_socket[i]);
			// attr
			for (int i=0; i<ITEM_ATTRIBUTE_MAX_NUM; i++)
				pkNewItem->SetForceAttribute(i, m_attr[(i*2)+0], m_attr[(i*2)+1]);
			// state
			int iEmptyCell = -1;
			int m_sec = 0;
			PIXEL_POSITION pos;
			switch(m_state)
			{
				case PCMI0_GIVE:
					iEmptyCell = ch->GetEmptyInventory(pkNewItem->GetSize());
					if (-1 == iEmptyCell)
					{
						M2_DESTROY_ITEM(pkNewItem);
						lua_pushboolean(L, false);
						return 1;
					}
					pkNewItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyCell));
					break;
				case PCMI0_DROPWP:
					if (lua_isnumber(L, 5) && (m_sec = lua_tonumber(L, 5)))
						pkNewItem->SetOwnership(ch, m_sec<=0?1:m_sec); //, ch->ChatPacket(CHAT_TYPE_INFO, "arg5 is %d", m_sec<=0?1:m_sec);
					else
						pkNewItem->SetOwnership(ch);
				case PCMI0_DROP:
					pos.x = ch->GetX() + number(-200, 200);
					pos.y = ch->GetY() + number(-200, 200);

					pkNewItem->AddToGround(ch->GetMapIndex(), pos);
					pkNewItem->StartDestroyEvent();
					break;
				default:
					lua_pushboolean(L, false);
					return 1;
			}
			lua_pushboolean(L, true);
		}
		else
			lua_pushboolean(L, false);

		return 1;
	}

	int pc_set_race0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		int amount = MINMAX(0, lua_tonumber(L, 1), JOB_MAX_NUM);
		ESex mySex = GET_SEX(ch);
		DWORD dwRace = MAIN_RACE_WARRIOR_M;
		switch (amount)
		{
			case JOB_WARRIOR:
				dwRace = (mySex==SEX_MALE)?MAIN_RACE_WARRIOR_M:MAIN_RACE_WARRIOR_W;
				break;
			case JOB_ASSASSIN:
				dwRace = (mySex==SEX_MALE)?MAIN_RACE_ASSASSIN_M:MAIN_RACE_ASSASSIN_W;
				break;
			case JOB_SURA:
				dwRace = (mySex==SEX_MALE)?MAIN_RACE_SURA_M:MAIN_RACE_SURA_W;
				break;
			case JOB_SHAMAN:
				dwRace = (mySex==SEX_MALE)?MAIN_RACE_SHAMAN_M:MAIN_RACE_SHAMAN_W;
				break;
#ifdef ENABLE_WOLFMAN_CHARACTER
			case JOB_WOLFMAN:
				dwRace = (mySex==SEX_MALE)?MAIN_RACE_WOLFMAN_M:MAIN_RACE_WOLFMAN_M;
				break;
#endif
		}
		if (dwRace!=ch->GetRaceNum())
		{
			ch->SetRace(dwRace);
			ch->ClearSkill();
			ch->SetSkillGroup(0);
			// quick mesh change workaround begin
			ch->SetPolymorph(101);
			ch->SetPolymorph(0);
			// quick mesh change workaround end
		}
		return 0;
	}

	int pc_del_another_quest_flag(lua_State* L)
	{
		if (!lua_isstring(L, 1) || !lua_isstring(L, 2))
		{
			sys_err("QUEST wrong del flag");
			return 0;
		}
		else
		{
			const char * sz = lua_tostring(L, 1);
			const char * sz2 = lua_tostring(L, 2);
			CQuestManager & q = CQuestManager::Instance();
			PC * pPC = q.GetCurrentPC();
			lua_pushboolean(L, pPC->DeleteFlag(string(sz)+"."+sz2));
			return 1;
		}
	}

	int pc_pointchange(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->PointChange(lua_tonumber(L, 1), lua_tonumber(L, 2), lua_toboolean(L, 3), lua_toboolean(L, 4));
		return 0;
	}

	int pc_pullmob(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->PullMonster();
		return 0;
	}

	int pc_set_level0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		ch->ResetPoint(lua_tonumber(L, 1));
		ch->ClearSkill();
		ch->ClearSubSkill();
		return 0;
	}

	int pc_set_gm_level(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->SetGMLevel();
		return 0;
	}


	int pc_if_fire(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->IsAffectFlag(AFF_FIRE));
		return 1;
	}
	int pc_if_invisible(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->IsAffectFlag(AFF_INVISIBILITY));
		return 1;
	}
	int pc_if_poison(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->IsAffectFlag(AFF_POISON));
		return 1;
	}
	int pc_if_slow(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->IsAffectFlag(AFF_SLOW));
		return 1;
	}
	int pc_if_stun(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->IsAffectFlag(AFF_STUN));
		return 1;
	}

	int pc_sf_fire(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if(lua_toboolean(L, 1))
			ch->AddAffect(AFFECT_FIRE, 0, 0, AFF_FIRE, 3 * 5 + 1, 0, 1, 0);
		else
			ch->RemoveAffect(AFFECT_FIRE);
		return 0;
	}
	int pc_sf_invisible(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if(lua_toboolean(L, 1))
			ch->AddAffect(AFFECT_INVISIBILITY, 0, 0, AFF_INVISIBILITY, 60*60*24*365*60+1, 0, 1, 0);
		else
			ch->RemoveAffect(AFFECT_INVISIBILITY);
		return 0;
	}
	int pc_sf_poison(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if(lua_toboolean(L, 1))
			ch->AddAffect(AFFECT_POISON, 0, 0, AFF_POISON, 30+1, 0, 1, 0);
		else
			ch->RemoveAffect(AFFECT_POISON);
		return 0;
	}
	int pc_sf_slow(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if(lua_toboolean(L, 1))
			ch->AddAffect(AFFECT_SLOW, 19, -30, AFF_SLOW, 30, 0, 1, 0);
		else
			ch->RemoveAffect(AFFECT_SLOW);
		return 0;
	}
	int pc_sf_stun(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if(lua_toboolean(L, 1))
			ch->AddAffect(AFFECT_STUN, 0, 0, AFF_STUN, 30, 0, 1, 0);
		else
			ch->RemoveAffect(AFFECT_STUN);
		return 0;
	}

	int pc_sf_kill(lua_State* L)
	{
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(lua_tostring(L, 1));
		if (ch)
			ch->Dead(0, 0);
		return 0;
	}

	int pc_sf_dead(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->Dead(0, 0);
		return 0;
	}

	int pc_get_exp_level(lua_State* L)
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("arg1 must be a number");
			return 0;
		}
		lua_pushnumber(L, (DWORD)(exp_table[MINMAX(0, lua_tonumber(L, 1), PLAYER_MAX_LEVEL_CONST)] / 100));
		return 1;
	}

	int pc_get_exp_level0(lua_State* L)
	{
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2))
		{
			sys_err("arg1 and arg2 must be numbers");
			return 0;
		}
		lua_pushnumber(L, (DWORD)((exp_table[MINMAX(0, lua_tonumber(L, 1), PLAYER_MAX_LEVEL_CONST)] / 100) * MINMAX(1, lua_tonumber(L, 2), 100)));
		return 1;
	}

	int pc_set_max_health(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
		ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
		return 0;
	}

	int pc_get_ip0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushstring(L, ch->GetDesc()->GetHostName());
		return 1;
	}

	int pc_dc_delayed0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		bool bRet = ch->GetDesc()->DelayedDisconnect(MINMAX(0, lua_tonumber(L, 1), 60*5));
		lua_pushboolean(L, bRet);
		return 1;
	}

	int pc_dc_direct0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->Disconnect(lua_tostring(L, 1));
		return 0;
	}

	int pc_is_trade0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, ch->GetExchange()!=NULL);
		return 1;
	}

	int pc_is_busy0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		lua_pushboolean(L, (ch->GetExchange()
							 || ch->GetShop()
							 || ch->IsOpenSafebox()
							 || ch->IsCubeOpen()
#ifdef __AURA_SYSTEM__
							 || ch->IsAuraRefineWindowOpen()
#endif
#ifdef __SASH_SYSTEM__
							 || ch->isSashOpened(true)
							 || ch->isSashOpened(false)
#endif
		));
		return 1;
	}

	int pc_is_arena0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, ch->GetArena() != NULL);
		return 1;
	}

	int pc_is_arena_observer0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		lua_pushboolean(L, ch->GetArenaObserverMode());
		return 1;
	}

	int pc_equip_slot0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		LPITEM item = ch->GetInventoryItem(lua_tonumber(L, 1));
		lua_pushboolean(L, (item)?ch->EquipItem(item):false);
		return 1;
	}

	int pc_unequip_slot0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		LPITEM item = ch->GetWear(lua_tonumber(L, 1));
		lua_pushboolean(L, (item)?ch->UnequipItem(item):false);
		return 1;
	}

	int pc_is_available0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushboolean(L, ch!=NULL);
		return 1;
	}

	int pc_give_random_book0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		LPITEM item = ch->AutoGiveItem(50300);
		if (item)
		{
			if (lua_isnumber(L, 1))
				item->SetSocket(0, ::GetRandomSkillVnum(lua_tonumber(L, 1)));
			else
				item->SetSocket(0, ::GetRandomSkillVnum());
		}
		lua_pushboolean(L, item!=NULL);
		return 1;
	}

#endif

#ifdef ENABLE_PC_OPENSHOP
	int pc_open_shop0(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
#ifdef ENABLE_CH_CRASH_CORE_FIX
		if (!ch)
			return 0;
#endif
		//PREVENT_TRADE_WINDOW
		if (ch->IsOpenSafebox()
			 || ch->GetExchange()
			 || ch->GetShop()
			 || ch->IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			 || ch->IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			 || ch->isAcceOpened(true)
			 || ch->isAcceOpened(false)
#endif
		)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "917");
			return 0;
		}
		//END_PREVENT_TRADE_WINDOW

		LPSHOP sh = CShopManager::instance().Get(lua_tonumber(L, 1));
		sh->AddGuest(ch, 0, false);
		ch->SetShopOwner(NULL);
		return 0;
	}
#endif

#ifdef ENABLE_NEWGUILDMAKE
	enum MKGLD : int
	{
		MKGLD_INVALID_NAME_LENGTH = -2,
		MKGLD_INVALID_NAME_INPUT = -1,
		MKGLD_GUILD_NOT_CREATED = 0,
		MKGLD_GUILD_CREATED = 1,
		MKGLD_ALREADY_GUILDED = 2,
		MKGLD_ALREADY_MASTER_GUILD = 3
	};

	int pc_make_guild0(lua_State* L)
	{
		// -2 guild name is invalid (strlen <2 or >11!)
		// -1 guild name is invalid (special chars found!)
		// 0 guild not created (guild name already present or already member of a guild)
		// 1 guild created
		// 2 player already part of a guild
		// 3 player already guild master
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch->GetGuild())
		{
			lua_pushnumber(L, (ch->GetPlayerID() == ch->GetGuild()->GetMasterPID())?MKGLD_ALREADY_MASTER_GUILD:MKGLD_ALREADY_GUILDED);
			return 1;
		}
		const char* guild_name = lua_tostring(L, 1);
		size_t guild_lname = strlen(guild_name);
		if (guild_lname<2 || 11<guild_lname)
		{
			lua_pushnumber(L, MKGLD_INVALID_NAME_LENGTH);
			return 1;
		}

		TGuildCreateParameter cp;
		memset(&cp, 0, sizeof(cp));

		cp.master = ch;
		strlcpy(cp.name, guild_name, sizeof(cp.name));

		int ret_type = MKGLD_GUILD_NOT_CREATED;

		const auto checkNameRes = check_name(cp.name);

		if (!checkNameRes)
		{
			ret_type = MKGLD_INVALID_NAME_INPUT;
		}
		else
		{
			if (CGuildManager::instance().CreateGuild(cp))
			{
				ret_type = MKGLD_GUILD_CREATED;
			}
			else
			{
				ret_type = MKGLD_GUILD_NOT_CREATED;
			}
		}

		lua_pushnumber(L, ret_type);
		return 1;
	}
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	int pc_open_acce(lua_State * L)
	{
		if (lua_isboolean(L, 1))
		{
			CQuestManager & qMgr = CQuestManager::instance();
			LPCHARACTER pkChar = qMgr.GetCurrentCharacterPtr();
			if (pkChar)
				pkChar->OpenAcce(lua_toboolean(L, 1));
		}
		else
			sys_err("Invalid argument: arg1 must be boolean.");

		return 0;
	}
#endif

#ifdef ENABLE_BATTLE_PASS
	int pc_do_battle_pass(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;

		if (ch->v_counts.empty())
			return 0;

		if (!ch->v_counts.empty())
		{
			for (int i = 0; i < ch->missions_bp.size(); ++i)
			{
				if (ch->missions_bp[i].type == (int)lua_tonumber(L, 1)) { ch->DoMission(i, (DWORD)lua_tonumber(L, 2)); }
			}
		}
		return 1;
	}

	int pc_do_battle_pass_premium(lua_State * L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;

		if (ch->v_counts_premium.empty())
			return 0;

		if (!ch->v_counts_premium.empty())
		{
			for (int i = 0; i < ch->missions_bp_premium.size(); ++i)
			{
				if (ch->missions_bp_premium[i].type == (int)lua_tonumber(L, 1)) { ch->DoMissionPremium(i, (DWORD)lua_tonumber(L, 2)); }
			}
		}
		return 1;
	}
#endif

	int _horse_test(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (ch->IsHorseRiding() == false)
		{
			if (ch->GetHorse() == NULL)
			{
				lua_pushboolean(L, false);
				return 1;
			}

			lua_pushboolean(L, false);
			return 1;
		}

		lua_pushboolean(L, true);
		return 1;
	}

#ifdef ENABLE_NEW_DUNGEON_LIB
	int pc_jump(lua_State* L)
	{
		if (lua_gettop(L) < 2 || !lua_isnumber(L, 1) || !lua_isnumber(L, 2))
			return 0;

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if(!ch || ch == NULL)
			return 0;

		ch->Show(ch->GetMapIndex(), (int)lua_tonumber(L, 1), (int)lua_tonumber(L, 2), 0);
		ch->Stop();
		return 0;
	}
#endif

#ifdef ENABLE_DRAGON_LAIR
	int pc_set_observer_true(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch || ch == NULL)
			return 0;
		ch->SetObserverMode(true);
		return 0;
	}
	int pc_set_observer_false(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch || ch == NULL)
			return 0;
		ch->SetObserverMode(false);
		return 0;
	}
	int pc_is_observer(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch || ch == NULL)
			return 0;
		lua_pushboolean(L, ch->IsObserverMode());
		return 1;
	}
#endif

#ifdef ENABLE_COLLECT_WINDOW
	int pc_send_collect_info(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		
		if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4) || !lua_isnumber(L, 5) || !lua_isnumber(L, 6) || !lua_isnumber(L, 7) || !lua_isnumber(L, 8) || !lua_isnumber(L, 9))
		{
			sys_err("QUEST send collect info call error : wrong argument");
			return 1;
		}

		const BYTE windowType = (BYTE) lua_tonumber(L, 1);
		const DWORD time = (DWORD) lua_tonumber(L, 2);
		const DWORD count = (DWORD) lua_tonumber(L, 3);
		const DWORD itemVnum = (DWORD) lua_tonumber(L, 4);
		const DWORD countTotal = (DWORD) lua_tonumber(L, 5);
		const BYTE chance = (BYTE) lua_tonumber(L, 6);
		const DWORD renderTargetID = (DWORD) lua_tonumber(L, 7);
		const DWORD questindex = (DWORD) lua_tonumber(L, 8);
		const DWORD required_level = (DWORD) lua_tonumber(L, 9);

		const DWORD remain_sec = time-get_global_time();
		
		if (ch)
			ch->SendCollectPacket(windowType, remain_sec, count, itemVnum, countTotal, chance, renderTargetID, questindex, required_level);
		
		return 1;
	}
#endif

#ifdef ENABLE_RANKING
	int pc_get_rank_dungeon(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetRankPoints(14));
		return 1;
	}

	int pc_set_rank_dungeon(lua_State* L)
	{
		if (lua_isnumber(L, 1))
		{
			long long lPoints = lua_tonumber(L, 1);
			LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
			if (!ch) { return 0; }
			if (ch)
				ch->SetRankPoints(14, lPoints);
		}
		return 0;
	}
#endif

#ifdef ENABLE_CONQUEROR_LEVEL
	int pc_conqueror_reset_status(lua_State* L)
	{
		if (lua_isnumber(L, 1))
		{
			const int idx = static_cast<int>(lua_tonumber(L, 1));

			if (idx >= 0 && idx < 4)
			{
				LPCHARACTER ch = CQuestManager::Instance().GetCurrentCharacterPtr();
				if (!ch)
				{
					lua_pushboolean(L, 0);
					return 1;
				}

				int point = POINT_NONE;
				char buf[128];

				switch (idx)
				{
					case 0:
						point = POINT_SUNGMA_STR;
						break;
					case 1:
						point = POINT_SUNGMA_HP;
						break;
					case 2:
						point = POINT_SUNGMA_MOVE;
						break;
					case 3:
						point = POINT_SUNGMA_IMMUNE;
						break;
					default:
						lua_pushboolean(L, false);
						return 1;
				}

				const int old_val = ch->GetRealPoint(point);
				const int old_stat = ch->GetRealPoint(POINT_CONQUEROR_POINT);

				ch->SetRealPoint(point, 0);
				ch->SetPoint(point, ch->GetRealPoint(point));
				ch->PointChange(POINT_CONQUEROR_POINT, old_val); //@fixme536

				ch->ComputePoints();
				ch->PointsPacket();

				switch (idx)
				{
					case 0:
						snprintf(buf, sizeof(buf), "reset cst(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_CONQUEROR_POINT));
						break;
					case 1:
						snprintf(buf, sizeof(buf), "reset cht(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_CONQUEROR_POINT));
						break;
					case 2:
						snprintf(buf, sizeof(buf), "reset move(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_CONQUEROR_POINT));
						break;
					case 3:
						snprintf(buf, sizeof(buf), "reset immu(%d)->1 stat_point(%d)->(%d)", old_val, old_stat, ch->GetRealPoint(POINT_CONQUEROR_POINT));
						break;
					default:
						break;
				}

				LogManager::Instance().CharLog(ch, 0, "RESET_SUNGMA_ONE_STATUS", buf);

				lua_pushboolean(L, true);
				return 1;
			}
		}

		lua_pushboolean(L, false);
		return 1;
	}

	int pc_get_cht(lua_State* L)
	{
		const LPCHARACTER& ch = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch ? ch->GetRealPoint(POINT_SUNGMA_HP) : 0);
		return 1;
	}

	int pc_set_cht(lua_State* L)
	{
		if (lua_isnumber(L, 1) == false)
			return 1;

		const int newPoint = static_cast<int>(lua_tonumber(L, 1));

		LPCHARACTER ch = CQuestManager::Instance().GetCurrentCharacterPtr();
		if (!ch)
			return 1;

		const int usedPoint = newPoint - ch->GetRealPoint(POINT_SUNGMA_HP);
		// FIX: negatif deger / yetersiz conqueror puaniyla stat alinmasin
		if (newPoint < 0 || newPoint > SHRT_MAX || usedPoint > ch->GetPoint(POINT_CONQUEROR_POINT))
			return 1;
		ch->SetRealPoint(POINT_SUNGMA_HP, newPoint);
		ch->PointChange(POINT_SUNGMA_HP, 0);
		ch->PointChange(POINT_CONQUEROR_POINT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_get_cimmun(lua_State* L)
	{
		const LPCHARACTER& ch = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch ? ch->GetRealPoint(POINT_SUNGMA_IMMUNE) : 0);
		return 1;
	}

	int pc_set_cimmun(lua_State* L)
	{
		if (lua_isnumber(L, 1) == false)
			return 1;

		const int newPoint = static_cast<int>(lua_tonumber(L, 1));

		LPCHARACTER ch = CQuestManager::Instance().GetCurrentCharacterPtr();
		if (!ch)
			return 1;

		const int usedPoint = newPoint - ch->GetRealPoint(POINT_SUNGMA_IMMUNE);
		// FIX: negatif deger / yetersiz conqueror puaniyla stat alinmasin
		if (newPoint < 0 || newPoint > SHRT_MAX || usedPoint > ch->GetPoint(POINT_CONQUEROR_POINT))
			return 1;
		ch->SetRealPoint(POINT_SUNGMA_IMMUNE, newPoint);
		ch->PointChange(POINT_SUNGMA_IMMUNE, 0);
		ch->PointChange(POINT_CONQUEROR_POINT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_get_cst(lua_State* L)
	{
		const LPCHARACTER& ch = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch ? ch->GetRealPoint(POINT_SUNGMA_STR) : 0);
		return 1;
	}

	int pc_set_cst(lua_State* L)
	{
		if (lua_isnumber(L, 1) == false)
			return 1;

		const int newPoint = static_cast<int>(lua_tonumber(L, 1));

		LPCHARACTER ch = CQuestManager::Instance().GetCurrentCharacterPtr();
		if (!ch)
			return 1;

		const int usedPoint = newPoint - ch->GetRealPoint(POINT_SUNGMA_STR);
		// FIX: negatif deger / yetersiz conqueror puaniyla stat alinmasin
		if (newPoint < 0 || newPoint > SHRT_MAX || usedPoint > ch->GetPoint(POINT_CONQUEROR_POINT))
			return 1;
		ch->SetRealPoint(POINT_SUNGMA_STR, newPoint);
		ch->PointChange(POINT_SUNGMA_STR, 0);
		ch->PointChange(POINT_CONQUEROR_POINT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_get_cmove(lua_State* L)
	{
		const LPCHARACTER& ch = CQuestManager::Instance().GetCurrentCharacterPtr();
		lua_pushnumber(L, ch ? ch->GetRealPoint(POINT_SUNGMA_MOVE) : 0);
		return 1;
	}

	int pc_set_cmove(lua_State* L)
	{
		if (lua_isnumber(L, 1) == false)
			return 1;

		const int newPoint = static_cast<int>(lua_tonumber(L, 1));

		LPCHARACTER ch = CQuestManager::Instance().GetCurrentCharacterPtr();
		if (!ch)
			return 1;

		const int usedPoint = newPoint - ch->GetRealPoint(POINT_SUNGMA_MOVE);
		// FIX: negatif deger / yetersiz conqueror puaniyla stat alinmasin
		if (newPoint < 0 || newPoint > SHRT_MAX || usedPoint > ch->GetPoint(POINT_CONQUEROR_POINT))
			return 1;
		ch->SetRealPoint(POINT_SUNGMA_MOVE, newPoint);
		ch->PointChange(POINT_SUNGMA_MOVE, 0);
		ch->PointChange(POINT_CONQUEROR_POINT, -usedPoint);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}

	int pc_set_conqueror_level(lua_State* L)
	{
		const LPCHARACTER ch = CQuestManager::Instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;

		if (ch->GetLevel() != gPlayerMaxLevel)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You didnt reached the maximum Level!");
			return 0;
		}

		if (/*ch->GetLevel() == gPlayerMaxLevel && */ch->GetExp() < (exp_table_common[120] / 4))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You dont have the matching experience!");
			return 0;
		}

		if (ch->GetConquerorLevel() > 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;1754]");
			return 0;
		}

		ch->PointChange(POINT_CONQUEROR_LEVEL, 1, false, true);

		const auto skill_group = ch->GetSkillGroup();
		if (skill_group == 0)
			return 0;

		const auto job = ch->GetJob();
		if (job == 0)
			ch->SetSkillLevel(176, 1);
		else if (job == 1)
			ch->SetSkillLevel(176 + skill_group, 1);
		else if (job == 2)
			ch->SetSkillLevel(178 + skill_group, 1);
		else if (job == 3)
			ch->SetSkillLevel(180 + skill_group, 1);
		else if (job == 4)
			ch->SetSkillLevel(183, 1);
		ch->SkillLevelPacket();

		return 0;
	}

	int pc_get_conqueror_level(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetConquerorLevel());
		return 1;
	}
#endif

#ifdef ENABLE_CONQUEROR_LEVEL
	int pc_get_conquerorlevel(lua_State * L) 
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetConquerorLevel());
		return 1;
	}

	int pc_give_conquerorexp2(lua_State* L)
	{
		CQuestManager& q = CQuestManager::instance();
		LPCHARACTER ch = q.GetCurrentCharacterPtr();
		if (!lua_isnumber(L,1))
			return 0;

		sys_log(0,"QUEST [REWARD] %s give conqueror exp2 %d", ch->GetName(), (int)rint(lua_tonumber(L,1)));

		DWORD exp = (DWORD)rint(lua_tonumber(L,1));

		PC* pPC = CQuestManager::instance().GetCurrentPC();
		LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetConquerorLevel(), exp, 0);
		ch->PointChange(POINT_CONQUEROR_EXP, exp);
		return 0;
	}

	int pc_get_next_conquerorexp(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetConquerorNextExp());
		return 1;
	}

	int pc_get_conquerorexp(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetConquerorExp());
		return 1;
	}

	int pc_set_conquerorlevel(lua_State * L)  
	{
		if (!lua_isnumber(L, 1))
		{
			sys_err("invalid argument");
			return 0;
		}
		else
		{
			int newLevel = lua_tonumber(L, 1);
			LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
			sys_log(0,"QUEST [CONQUERORLEVEL] %s jumpint to conquerorlevel %d", ch->GetName(), (int)rint(lua_tonumber(L,1)));
			PC* pPC = CQuestManager::instance().GetCurrentPC();
			LogManager::instance().QuestRewardLog(pPC->GetCurrentQuestName().c_str(), ch->GetPlayerID(), ch->GetConquerorLevel(), newLevel, 0);
			ch->PointChange(POINT_CONQUEROR_LEVEL, newLevel - ch->GetConquerorLevel());
			ch->ComputePoints();
			ch->PointsPacket();
			return 0;
		}
	}

	int pc_set_conqueror_point( lua_State* L )
	{
		if ( lua_isnumber(L, 1) == false )
			return 1;

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		ch->PointChange(POINT_CONQUEROR_POINT, 1);
		ch->ComputePoints();
		ch->PointsPacket();
		return 1;
	}
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	int pc_open_soul_roulette(lua_State* L)
	{
		LPCHARACTER pkChar = CQuestManager::instance().GetCurrentCharacterPtr();
		if (pkChar && !pkChar->GetSoulRoulette())
			pkChar->SetSoulRoulette(new CSoulRoulette(pkChar));
		return 0;
	}

	int pc_change_soul(lua_State* L)
	{
		int soul = (int)lua_tonumber(L, -1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (soul + ch->GetSoulPoint() < 0)
			sys_err("QUEST wrong ChangeSoul %d (now %d)", soul, ch->GetSoulPoint());
		else
		{
			ch->PointChange(POINT_SOUL, soul, true);
		}

		return 0;
	}

	int pc_get_soul(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetSoulPoint());
		return 1;
	}

	int pc_change_soul_re(lua_State* L)
	{
		int soulre = (int)lua_tonumber(L, -1);

		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (soulre + ch->GetSoulRePoint() < 0)
			sys_err("QUEST wrong ChangeSoulRe %d (now %d)", soulre, ch->GetSoulRePoint());
		else
		{
			ch->PointChange(POINT_SOUL_RE, soulre, true);
		}

		return 0;
	}

	int pc_get_soul_re(lua_State* L)
	{
		lua_pushnumber(L, CQuestManager::instance().GetCurrentCharacterPtr()->GetSoulRePoint());
		return 1;
	}

	int pc_get_soul_global_rank(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
		if (!ch)
			return 0;
		

		char szBuf[4096+1];
		ch->GetGlobalRankRoulette(szBuf, sizeof(szBuf));
		lua_pushstring(L, szBuf);
		return 1;
	}
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	int pc_get_language(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		if (ch && ch->GetDesc())
		{
			lua_pushstring(L, LC_LOCALE(ch->GetDesc()->GetLanguage()));
			return 1;
		}

		lua_pushstring(L, LC_LOCALE(LOCALE_DEFAULT));
		return 1;
	}
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	int pc_has_offlineshop(lua_State* L)
	{
		LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();

		lua_pushboolean(L, COfflineShopManager::Instance().HasOfflineShop(ch) ? 1 : 0);
		return 1;
	}
#endif

	void RegisterPCFunctionTable()
	{
		luaL_reg pc_functions[] =
		{
			{ "get_wear",										pc_get_wear								},
			{ "get_player_id",									pc_get_player_id						},
			{ "get_account_id",									pc_get_account_id						},
			{ "get_account",									pc_get_account							},
			{ "get_level",										pc_get_level							},
			{ "set_level",										pc_set_level							},
			{ "get_next_exp",									pc_get_next_exp							},
			{ "get_exp",										pc_get_exp								},
			{ "get_job",										pc_get_job								},
			{ "get_race",										pc_get_race								},
			{ "change_sex",										pc_change_sex							},
#ifdef ENABLE_CHANGE_SEX_QUEST
			{ "change_sex_degisemen",							pc_change_sex_degisemen					},
#endif
			{ "gethp",											pc_get_hp								},
			{ "get_hp",											pc_get_hp								},
			{ "getmaxhp",										pc_get_max_hp							},
			{ "get_max_hp",										pc_get_max_hp							},
			{ "getsp",											pc_get_sp								},
			{ "get_sp",											pc_get_sp								},
			{ "getmaxsp",										pc_get_max_sp							},
			{ "get_max_sp",										pc_get_max_sp							},
			{ "change_sp",										pc_change_sp							},
			{ "getmoney",										pc_get_money							},
			{ "get_money",										pc_get_money							},
			{ "get_real_alignment",								pc_get_real_alignment					},
			{ "get_alignment",									pc_get_alignment						},
			{ "getweapon",										pc_get_weapon							},
			{ "get_weapon",										pc_get_weapon							},
			{ "getarmor",										pc_get_armor							},
			{ "get_armor",										pc_get_armor							},
			{ "getgold",										pc_get_money							},
			{ "get_gold",										pc_get_money							},
			{ "changegold",										pc_change_money							},
			{ "changemoney",									pc_change_money							},
			{ "changealignment",								pc_change_alignment						},
			{ "change_gold",									pc_change_money							},
			{ "change_money",									pc_change_money							},
			{ "change_alignment",								pc_change_alignment						},
			{ "getname",										pc_get_name								},
			{ "get_name",										pc_get_name								},
			{ "get_vid",										pc_get_vid								},
			{ "getplaytime",									pc_get_playtime							},
			{ "get_playtime",									pc_get_playtime							},
			{ "getleadership",									pc_get_leadership						},
			{ "get_leadership",									pc_get_leadership						},
			{ "getqf",											pc_get_quest_flag						},
			{ "setqf",											pc_set_quest_flag						},
			{ "delqf",											pc_del_quest_flag						},
			{ "getf",											pc_get_another_quest_flag				},
			{ "setf",											pc_set_another_quest_flag				},
			{ "get_x",											pc_get_x								},
			{ "get_y",											pc_get_y								},
			{ "getx",											pc_get_x								},
			{ "gety",											pc_get_y								},
			{ "get_local_x",									pc_get_local_x							},
			{ "get_local_y",									pc_get_local_y							},
			{ "getcurrentmapindex",								pc_get_current_map_index				},
			{ "get_map_index",									pc_get_current_map_index				},
			{ "give_exp",										pc_give_exp								},
			{ "give_exp_perc",									pc_give_exp_perc						},
			{ "give_exp2",										pc_give_exp2							},
			{ "give_item",										pc_give_item							},
			{ "give_item2",										pc_give_or_drop_item					},
			{ "give_item2_select",								pc_give_or_drop_item_and_select			},
			{ "give_gold",										pc_give_gold							},
			{ "count_item",										pc_count_item							},
			{ "remove_item",									pc_remove_item							},
			{ "countitem",										pc_count_item							},
			{ "removeitem",										pc_remove_item							},
			{ "reset_point",									pc_reset_point							},
			{ "has_guild",										pc_hasguild								},
			{ "hasguild",										pc_hasguild								},
			{ "get_guild",										pc_getguild								},
			{ "getguild",										pc_getguild								},
			{ "isguildmaster",									pc_isguildmaster						},
			{ "is_guild_master",								pc_isguildmaster						},
			{ "destroy_guild",									pc_destroy_guild						},
			{ "remove_from_guild",								pc_remove_from_guild					},
			{ "in_dungeon",										pc_in_dungeon							},
			{ "getempire",										pc_get_empire							},
			{ "get_empire",										pc_get_empire							},
			{ "get_skill_group",								pc_get_skillgroup						},
			{ "set_skill_group",								pc_set_skillgroup						},
			{ "warp",											pc_warp									},
			{ "warp_local",										pc_warp_local							},
			{ "warp_exit",										pc_warp_exit							},
			{ "set_warp_location",								pc_set_warp_location					},
			{ "set_warp_location_local",						pc_set_warp_location_local				},
			{ "get_start_location",								pc_get_start_location					},
			{ "has_master_skill",								pc_has_master_skill						},
			{ "set_part",										pc_set_part								},
			{ "get_part",										pc_get_part								},
			{ "is_polymorphed",									pc_is_polymorphed						},
			{ "remove_polymorph",								pc_remove_polymorph						},
			{ "is_mount",										pc_is_mount								},
			{ "polymorph",										pc_polymorph							},
			{ "mount",											pc_mount								},
			{ "mount_bonus",									pc_mount_bonus							},
			{ "unmount",										pc_unmount								},
			{ "warp_to_guild_war_observer_position",			pc_warp_to_guild_war_observer_position	},
			{ "give_item_from_special_item_group",				pc_give_item_from_special_item_group	},
			{ "learn_grand_master_skill",						pc_learn_grand_master_skill				},
			{ "is_skill_book_no_delay",							pc_is_skill_book_no_delay				},
			{ "remove_skill_book_no_delay",						pc_remove_skill_book_no_delay			},
#ifdef ENABLE_SUNG_MAHI_TOWER
			{"sung_mahi_curse_hp", 								pc_sung_mahi_curse_hp					},
#endif
#ifdef ENABLE_RANKING
			{ "add_progress_in_rank",							pc_add_progress_in_rank					},
			{ "rank_dungeon_complete",							pc_ranking_dungeon_complete				},
#endif
			{ "enough_inventory",								pc_enough_inventory						},
			{ "get_horse_level",								pc_get_horse_level						},
			{ "is_horse_alive",									pc_is_horse_alive						},
			{ "revive_horse",									pc_revive_horse							},
			{ "have_pos_scroll",								pc_have_pos_scroll						},
			{ "have_map_scroll",								pc_have_map_scroll						},
			{ "get_war_map",									pc_get_war_map							},
			{ "get_equip_refine_level",							pc_get_equip_refine_level				},
			{ "refine_equip",									pc_refine_equip							},
			{ "get_skill_level",								pc_get_skill_level						},
			{ "give_lotto",										pc_give_lotto							},
			{ "aggregate_monster",								pc_aggregate_monster					},
			{ "forget_my_attacker",								pc_forget_my_attacker					},
			{ "pc_attract_ranger",								pc_attract_ranger						},
			{ "select",											pc_select_vid							},
			{ "get_sex",										pc_get_sex								},
#ifdef ENABLE_GET_COSTUME_QUEST
			{ "get_costume",									pc_get_costume							},
#endif
			{ "is_married",										pc_is_married							},
			{ "is_engaged",										pc_is_engaged							},
			{ "is_engaged_or_married",							pc_is_engaged_or_married				},
			{ "is_gm",											pc_is_gm								},
			{ "get_gm_level",									pc_get_gm_level							},
			{ "mining",											pc_mining								},
			{ "ore_refine",										pc_ore_refine							},
			{ "diamond_refine",									pc_diamond_refine						},
			{ "clear_one_skill",								pc_clear_one_skill						},
			{ "clear_skill",									pc_clear_skill							},
			{ "clear_sub_skill",								pc_clear_sub_skill						},
			{ "set_skill_point",								pc_set_skill_point						},
			{ "is_clear_skill_group",							pc_is_clear_skill_group					},
			{ "save_exit_location",								pc_save_exit_location					},
			{ "teleport",										pc_teleport								},
			{ "set_skill_level",								pc_set_skill_level						},
			{ "give_polymorph_book",							pc_give_polymorph_book					},
			{ "upgrade_polymorph_book",							pc_upgrade_polymorph_book				},
			{ "get_premium_remain_sec",							pc_get_premium_remain_sec				},
			{ "send_block_mode",								pc_send_block_mode						},
			{ "change_empire",									pc_change_empire						},
			{ "get_change_empire_count",						pc_get_change_empire_count				},
			{ "set_change_empire_count",						pc_set_change_empire_count				},
			{ "change_name",									pc_change_name							},
			{ "is_dead",										pc_is_dead								},
			{ "reset_status",									pc_reset_status							},
			{ "get_ht",											pc_get_ht								},
			{ "set_ht",											pc_set_ht								},
			{ "get_iq",											pc_get_iq								},
			{ "set_iq",											pc_set_iq								},
			{ "get_st",											pc_get_st								},
			{ "set_st",											pc_set_st								},
			{ "get_dx",											pc_get_dx								},
			{ "set_dx",											pc_set_dx								},
			{ "is_near_vid",									pc_is_near_vid							},
			{ "get_socket_items",								pc_get_socket_items						},
			{ "get_empty_inventory_count",						pc_get_empty_inventory_count			},
			{ "get_logoff_interval",							pc_get_logoff_interval					},
			{ "is_riding",										pc_is_riding							},
			{ "get_special_ride_vnum",							pc_get_special_ride_vnum				},
			{ "can_warp",										pc_can_warp								},
			{ "dec_skill_point",								pc_dec_skill_point						},
			{ "get_skill_point",								pc_get_skill_point						},
			{ "get_channel_id",									pc_get_channel_id						},
			{ "give_poly_marble",								pc_give_poly_marble						},
			{ "get_sig_items",									pc_get_sig_items						},
			{ "charge_cash",									pc_charge_cash							},
			{ "get_informer_type",								pc_get_informer_type					},
			{ "get_informer_item",								pc_get_informer_item					},
			{ "give_award",										pc_give_award							},
			{ "give_award_socket",								pc_give_award_socket					},
			{ "get_killee_drop_pct",							pc_get_killee_drop_pct					},

#ifdef ENABLE_GUILD_RANK_SYSTEM
			{ "open_guild_ranking",								pc_open_guild_ranking					},
#endif

#ifdef ENABLE_MESSENGER_BLOCK
			{ "is_blocked",										pc_is_blocked							},
			{ "is_friend",										pc_is_friend							},
#endif

#ifdef ENABLE_NEWSTUFF
			//pc.set_race0(race=[0. Warrior, 1. Ninja, 2. Sura, 3. Shaman, 4. Lycan])
			{ "set_race0",			pc_set_race0			},
			//if pc.delf("game_option", "block_cocks") then syschat("now you are unsafe") end
			{ "delf",				pc_del_another_quest_flag},	// delete quest flag [return lua boolean: successfulness]
			//pc.make_item0({vnum|locale_name, count}, {socket1,2,3}, {type1, value1, ... , type7, value7}, gstate(=0: giveitem, 1: dropitem, 2: drop_item_with_leadership)[, countdown_in_secs_before_ownership_vanish(=if <=10 would be 30; default=10->30)])
			{ "make_item0",			pc_make_item0		},	// [return lua boolean: successfulness]
			//pc.pointchange(uint32_t type, int amount, bool bAmount, bool bBroadcast)
			{ "pointchange",		pc_pointchange		},	// [return nothing]
			//pc.pullmob()
			{ "pullmob",			pc_pullmob			},	// [return nothing]
			//pc.select_pid(pc.get_player_id())
			{ "select_pid",			pc_select_pid		},	// [return lua number: old pid]
			//pc.select renamed in pc.select_vid
			{ "select_vid",			pc_select_vid		},	// [return lua number: old vid]
			//pc.set_level0(level)
			{ "set_level0",			pc_set_level0		},	// [return nothing]
			//pc.set_gm_level(); instead of `/reload a` (note: this only refresh gm privileges if you already are gm on common.gm[host|list])
			{ "set_gm_level",		pc_set_gm_level		},	// [return nothing]
			//is_flags (void) [return lua boolean]
			{ "is_flag_fire",			pc_if_fire			},
			{ "is_flag_invisible",		pc_if_invisible		},
			{ "is_flag_poison",			pc_if_poison		},
			{ "is_flag_slow",			pc_if_slow			},
			{ "is_flag_stun",			pc_if_stun			},
			//set_flags (bool) [return nothing]
			{ "set_flag_fire",			pc_sf_fire			},
			{ "set_flag_invisible",		pc_sf_invisible		},
			{ "set_flag_poison",		pc_sf_poison		},
			{ "set_flag_slow",			pc_sf_slow			},
			{ "set_flag_stun",			pc_sf_stun			},
			//pc.set_flag_kill(char_name) [return nothing]
			{ "set_flag_kill",			pc_sf_kill			},
			//pc.set_flag_dead()
			{ "set_flag_dead",			pc_sf_dead			},	// kill themselves [return nothing]
			//pc.get_exp_level(level) [return: lua number]
			{ "get_exp_level",		pc_get_exp_level	},	// get needed exp for <level> level [return: lua number]
			//pc.get_exp_level(level, perc)
			{ "get_exp_level0",		pc_get_exp_level0	},	// get needed exp for <level> level / 100 * perc [return: lua number]
			//pc.set_max_health()
			{ "set_max_health",		pc_set_max_health	},	// [return nothing]
			{ "get_ip0",			pc_get_ip0			},	// [return lua string]
			{ "dc_delayed0",		pc_dc_delayed0	},	// crash a player after x secs [return lua boolean: successfulness]
			{ "dc_direct0",			pc_dc_direct0	},	// crash the <nick> player [return nothing]
			{ "is_trade0",			pc_is_trade0	},	// get if player is trading [return lua boolean]
			{ "is_busy0",			pc_is_busy0		},	// get if player is "busy" (if trade, safebox, npc/myshop, cube are open) [return lua boolean]
			{ "is_arena0",			pc_is_arena0		},	// get if player is in arena [return lua boolean]
			{ "is_arena_observer0",	pc_is_arena_observer0		},	// get if player is in arena as observer [return lua boolean]
			// pc.equip_slot0(cell)
			{ "equip_slot0",		pc_equip_slot0		},	// [return lua boolean: successfulness]
			// pc.unequip_slot0(cell)
			{ "unequip_slot0",		pc_unequip_slot0	},	// [return lua boolean: successfulness]
			{ "is_available0",		pc_is_available0	},	// [return lua boolean]
			{ "give_random_book0",	pc_give_random_book0},	// [return lua boolean]
#endif

#ifdef ENABLE_PC_OPENSHOP
			//pc.open_shop0(id_shop)
			{ "open_shop0",			pc_open_shop0		},	// buy/sell won't work on it [return nothing]
#endif

#ifdef ENABLE_NEWGUILDMAKE
			//pc.make_guild0(guild_name)
			{ "make_guild0",			pc_make_guild0	},	// it returns few state values which you can manage via lua [return lua number]
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			{ "get_language",									pc_get_language							},
#endif

#ifdef ENABLE_DICE_SYSTEM
			{ "give_item2_with_dice",							pc_give_or_drop_item_with_dice			},
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			{ "open_acce",										pc_open_acce							},
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
			{ "has_offlineshop",								pc_has_offlineshop						},
#endif

#ifdef ENABLE_NEW_DUNGEON_LIB
			{ "jump",											pc_jump									},
#endif

#ifdef ENABLE_DRAGON_LAIR
			{ "observer_true",									pc_set_observer_true					},
			{ "observer_false",									pc_set_observer_false					},
			{ "is_observer",									pc_is_observer							},
#endif

#ifdef ENABLE_GAYA_SYSTEM
			{ "get_gem", 										pc_get_gem 								},
			{ "change_gem", 									pc_change_gem 							},
			{ "give_gem", 										pc_give_gem 							},
#endif

#ifdef ENABLE_COLLECT_WINDOW
			{ "send_collect_info",								pc_send_collect_info					},
#endif
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
			{ "open_soul_roulette",								pc_open_soul_roulette					},
			{ "get_soul",										pc_get_soul								},
			{ "change_soul",									pc_change_soul							},
			{ "get_soul_re",									pc_get_soul_re							},
			{ "change_soul_re",									pc_change_soul_re						},
			{ "get_soul_global_rank",							pc_get_soul_global_rank					},
#endif
#ifdef ENABLE_MINIGAME_RUMI_EVENT
			{ "get_okay_global_rank",							pc_get_okay_global_rank					},
			{ "get_okay_rund_rank",								pc_get_okay_rund_rank					},
#endif
#ifdef ENABLE_BATTLE_PASS
			{ "do_mission", 									pc_do_battle_pass 						},
			{ "do_mission_premium", 							pc_do_battle_pass_premium 				},
#endif
#ifdef ENABLE_RANKING
			{"get_rank_dungeon", 								pc_get_rank_dungeon						},
			{"set_rank_dungeon", 								pc_set_rank_dungeon						},
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
			{ "get_conquerorlevel",			pc_get_conquerorlevel		},
			{ "get_conquerorgive_exp2",		pc_give_conquerorexp2		},
			{ "get_conquerornext_exp",		pc_get_next_conquerorexp	},
			{ "get_conquerorget_exp",		pc_get_conquerorexp			},
			{ "set_conquerorlevel",			pc_set_conquerorlevel		},
			{ "set_conqueror_point",		pc_set_conqueror_point		},
#endif
#ifdef ENABLE_BLOCK_EXP
			{ "block_exp",    _block_exp    }, 
			{ "unblock_exp",    _unblock_exp    }, 
			{ "getexp_blocked",    _getexp_blocked    }, 
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
			{ "conqueror_reset_status",							pc_conqueror_reset_status },

			{ "get_cht",										pc_get_cht },
			{ "set_cht",										pc_set_cht },
			{ "get_cimmun",										pc_get_cimmun },
			{ "set_cimmun",										pc_set_cimmun },
			{ "get_cst",										pc_get_cst },
			{ "set_cst",										pc_set_cst },
			{ "get_cmove",										pc_get_cmove },
			{ "set_cmove",										pc_set_cmove },
			{ "set_conqueror_level",							pc_set_conqueror_level },
			{ "get_conqueror_level",							pc_get_conqueror_level },
#endif

			{ NULL,												NULL									}
		};

		CQuestManager::instance().AddLuaFunctionTable("pc", pc_functions);
	}
};