#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "sectree_manager.h"
#include "mob_manager.h"
#include "packet.h"
#include "cmd.h"
#include "regen.h"
#include "guild.h"
#include "guild_manager.h"
#include "p2p.h"
#include "buffer_manager.h"
#include "fishing.h"
#include "mining.h"
#include "questmanager.h"
#include "vector.h"
#include "affect.h"
#include "db.h"
#include "priv_manager.h"
#include "building.h"
#include "battle.h"
#include "arena.h"
#include "start_position.h"
#include "party.h"
#include "BattleArena.h"
#include "xmas_event.h"
#include "log.h"
#include "unique_item.h"
#include "DragonSoul.h"
#include "../../common/service.h"

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	#include "offlineshop_manager.h"
	#include "offline_shop.h"
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	#include "growth_pet.h"
#endif

#ifdef ENABLE_DUNGEON_INFO
	#include "dungeon_info.h"
#endif

#ifdef ENABLE_ULTIMATE_REGEN
	#include "new_mob_timer.h"
#endif

extern bool DropEvent_RefineBox_SetValue(const std::string& name, int value);

// ADD_COMMAND_SLOW_STUN
enum
{
	COMMANDAFFECT_STUN,
	COMMANDAFFECT_SLOW,
};

void Command_ApplyAffect(LPCHARACTER ch, const char* argument, const char* affectName, int cmdAffect)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	sys_log(0, arg1);

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: %s <name>", affectName);
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);
	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s ayný haritada deðil", arg1);
		return;
	}

	switch (cmdAffect)
	{
		case COMMANDAFFECT_STUN:
			SkillAttackAffect(tch, 1000, IMMUNE_STUN, AFFECT_STUN, POINT_NONE, 0, AFF_STUN, 30, "GM_STUN");
			break;
		case COMMANDAFFECT_SLOW:
			SkillAttackAffect(tch, 1000, IMMUNE_SLOW, AFFECT_SLOW, POINT_MOV_SPEED, -30, AFF_SLOW, 30, "GM_SLOW");
			break;
	}

	sys_log(0, "%s %s", arg1, affectName);

	ch->ChatPacket(CHAT_TYPE_INFO, "%s %s", arg1, affectName);
}

ACMD(do_stun)
{
	Command_ApplyAffect(ch, argument, "stun", COMMANDAFFECT_STUN);
}

ACMD(do_slow)
{
	Command_ApplyAffect(ch, argument, "slow", COMMANDAFFECT_SLOW);
}

ACMD(do_transfer)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: transfer <name>");
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1); 
	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Hedef %d kanalýnda (benim kanalým %d)", pkCCI->bChannel, g_bChannel);
				return;
			}

			TPacketGGTransfer pgg;

			pgg.bHeader = HEADER_GG_TRANSFER;
			strlcpy(pgg.szName, arg1, sizeof(pgg.szName));
			pgg.lX = ch->GetX();
			pgg.lY = ch->GetY();

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGTransfer));
			ch->ChatPacket(CHAT_TYPE_INFO, "Transfer talep edildi.");
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Bu isimde(%s) bir karakter yok", arg1);
			sys_log(0, "There is no character(%s) by that name", arg1);
		}

		return;
	}

	if (ch == tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Transfer me?!?");
		return;
	}

	tch->WarpSet(ch->GetX(), ch->GetY(), ch->GetMapIndex());
}

struct GotoInfo
{
	std::string st_name;

	BYTE empire;
	int mapIndex;
	DWORD x, y;

	GotoInfo()
	{
		st_name = "";
		empire = 0;
		mapIndex = 0;

		x = 0;
		y = 0;
	}
	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}
	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}
	void __copy__(const GotoInfo& c_src)
	{
		st_name = c_src.st_name;
		empire = c_src.empire;
		mapIndex = c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

static std::vector<GotoInfo> gs_vec_gotoInfo;

void CHARACTER_AddGotoInfo(const std::string& c_st_name, BYTE empire, int mapIndex, DWORD x, DWORD y)
{
	GotoInfo newGotoInfo;
	newGotoInfo.st_name = c_st_name;
	newGotoInfo.empire = empire;
	newGotoInfo.mapIndex = mapIndex;
	newGotoInfo.x = x;
	newGotoInfo.y = y;
	gs_vec_gotoInfo.push_back(newGotoInfo);

	sys_log(0, "AddGotoInfo(name=%s, empire=%d, mapIndex=%d, pos=(%d, %d))", c_st_name.c_str(), empire, mapIndex, x, y);
}

bool FindInString(const char * c_pszFind, const char * c_pszIn)
{
	const char * c = c_pszIn;
	const char * p;

	p = strchr(c, '|');

	if (!p)
		return (0 == strncasecmp(c_pszFind, c_pszIn, strlen(c_pszFind)));
	else
	{
		char sz[64 + 1];

		do
		{
			strlcpy(sz, c, MIN(sizeof(sz), (p - c) + 1));

			if (!strncasecmp(c_pszFind, sz, strlen(c_pszFind)))
				return true;

			c = p + 1;
		} while ((p = strchr(c, '|')));

		strlcpy(sz, c, sizeof(sz));

		if (!strncasecmp(c_pszFind, sz, strlen(c_pszFind)))
			return true;
	}

	return false;
}

bool CHARACTER_GoToName(LPCHARACTER ch, BYTE empire, int mapIndex, const char* gotoName)
{
	std::vector<GotoInfo>::iterator i;
	for (i = gs_vec_gotoInfo.begin(); i != gs_vec_gotoInfo.end(); ++i)
	{
		const GotoInfo& c_eachGotoInfo = *i;

		if (mapIndex != 0)
		{
			if (mapIndex != c_eachGotoInfo.mapIndex)
				continue;
		}
		else if (!FindInString(gotoName, c_eachGotoInfo.st_name.c_str()))
			continue;

		if (c_eachGotoInfo.empire == 0 || c_eachGotoInfo.empire == empire)
		{
			int x = c_eachGotoInfo.x * 100;
			int y = c_eachGotoInfo.y * 100;

			ch->ChatPacket(CHAT_TYPE_INFO, "( %d, %d ) konumuna ýþýnlandýnýz", x, y);
			ch->WarpSet(x, y);
			ch->Stop();
			return true;
		}
	}
	return false;
}

ACMD(do_goto)
{
	char arg1[256], arg2[256];
	int x = 0, y = 0, z = 0;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 && !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: goto <x meter> <y meter>");
		return;
	}

	if (isnhdigit(*arg1) && isnhdigit(*arg2))
	{
		str_to_number(x, arg1);
		str_to_number(y, arg2);

		PIXEL_POSITION p;

		if (SECTREE_MANAGER::instance().GetMapBasePosition(ch->GetX(), ch->GetY(), p))
		{
			x += p.x / 100;
			y += p.y / 100;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, "( %d, %d ) konumuna gidiyorsunuz", x, y);
	}
	else
	{
		int mapIndex = 0;
		BYTE empire = 0;

		if (*arg1 == '#')
			str_to_number(mapIndex,  (arg1 + 1));

		if (*arg2 && isnhdigit(*arg2))
		{
			str_to_number(empire, arg2);
			empire = MINMAX(1, empire, 3);
		}
		else
			empire = ch->GetEmpire();

		if (CHARACTER_GoToName(ch, empire, mapIndex, arg1))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Harita bulunamadý, komut sözdizimi: /goto <haritaadý> [imparatorluk]");
			return;
		}

		return;

		/*
		   int iMapIndex = 0;
		   for (int i = 0; aWarpInfo[i].c_pszName != NULL; ++i)
		   {
		   if (iMapIndex != 0)
		   {
		   if (iMapIndex != aWarpInfo[i].iMapIndex)
		   continue;
		   }
		   else if (!FindInString(arg1, aWarpInfo[i].c_pszName))
		   continue;

		   if (aWarpInfo[i].bEmpire == 0 || aWarpInfo[i].bEmpire == bEmpire)
		   {
		   x = aWarpInfo[i].x * 100;
		   y = aWarpInfo[i].y * 100;

		   ch->ChatPacket(CHAT_TYPE_INFO, "( %d, %d ) konumuna ýþýnlandýnýz", x, y);
		   ch->WarpSet(x, y);
		   ch->Stop();
		   return;
		   }
		   }
		 */
	}

	x *= 100;
	y *= 100;

	ch->Show(ch->GetMapIndex(), x, y, z);
	ch->Stop();
}

ACMD(do_warp)
{
	char arg1[256], arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: warp <character name> | <x meter> <y meter>");
		return;
	}

	int x = 0, y = 0;

	if (isnhdigit(*arg1) && isnhdigit(*arg2))
	{
		str_to_number(x, arg1);
		str_to_number(y, arg2);
	}
	else
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

		if (NULL == tch)
		{
			const CCI* pkCCI = P2P_MANAGER::instance().Find(arg1);

			if (NULL != pkCCI)
			{
				if (pkCCI->bChannel != g_bChannel)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Hedef %d kanalýnda (benim kanalým %d)", pkCCI->bChannel, g_bChannel);
					return;
				}

				ch->WarpToPID( pkCCI->dwPID );
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Bu isimde kimse yok");
			}

			return;
		}
		else
		{
			x = tch->GetX() / 100;
			y = tch->GetY() / 100;
		}
	}

	x *= 100;
	y *= 100;

	ch->ChatPacket(CHAT_TYPE_INFO, "( %d, %d ) konumuna ýþýnlandýnýz", x, y);
	ch->WarpSet(x, y);
	ch->Stop();
}

ACMD(do_item)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: item <item vnum>");
		return;
	}

	int iCount = 1;

	if (*arg2)
	{
		str_to_number(iCount, arg2);
		iCount = MINMAX(1, iCount, ITEM_MAX_COUNT);
	}

	DWORD dwVnum;

	if (isnhdigit(*arg1))
		str_to_number(dwVnum, arg1);
	else
	{
		if (!ITEM_MANAGER::instance().GetVnum(arg1, dwVnum))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "#%u vnum'una sahip eþya mevcut deðil.", dwVnum);
			return;
		}
	}

	LPITEM item = ITEM_MANAGER::instance().CreateItem(dwVnum, iCount, 0, true);

	if (item)
	{
		if (item->IsDragonSoul())
		{
			int iEmptyPos = ch->GetEmptyDragonSoulInventory(item);

			if (iEmptyPos != -1)
			{
				item->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
				LogManager::instance().ItemLog(ch, item, "GM", item->GetName());
			}
			else
			{
				M2_DESTROY_ITEM(item);
				if (!ch->DragonSoul_IsQualified())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough inventory space."));
				}
				else
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough inventory space."));
			}
		}
#ifdef ENABLE_SPECIAL_INVENTORY
		else if (item->IsSkillBook())
		{
			int iEmptyPos = ch->GetEmptySkillBookInventory(item->GetSize());

			if (iEmptyPos != -1)
			{
#ifdef ENABLE_PICKUP_ITEM_EFFECT
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos), true);
#else
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
				LogManager::instance().ItemLog(ch, item, "GM", item->GetName());
			}
			else
			{
				M2_DESTROY_ITEM(item);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough inventory space."));
			}
		}
		else if (item->IsUpgradeItem())
		{
			int iEmptyPos = ch->GetEmptyUpgradeItemsInventory(item->GetSize());

			if (iEmptyPos != -1)
			{
#ifdef ENABLE_PICKUP_ITEM_EFFECT
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos), true);
#else
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
				LogManager::instance().ItemLog(ch, item, "GM", item->GetName());
			}
			else
			{
				M2_DESTROY_ITEM(item);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough inventory space."));
			}
		}
		else if (item->IsStone())
		{
			int iEmptyPos = ch->GetEmptyStoneInventory(item->GetSize());

			if (iEmptyPos != -1)
			{
#ifdef ENABLE_PICKUP_ITEM_EFFECT
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos), true);
#else
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
				LogManager::instance().ItemLog(ch, item, "GM", item->GetName());
			}
			else
			{
				M2_DESTROY_ITEM(item);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough inventory space."));
			}
		}
		else if (item->IsGiftBox())
		{
			int iEmptyPos = ch->GetEmptyGiftBoxInventory(item->GetSize());

			if (iEmptyPos != -1)
			{
#ifdef ENABLE_PICKUP_ITEM_EFFECT
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos), true);
#else
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
				LogManager::instance().ItemLog(ch, item, "GM", item->GetName());
			}
			else
			{
				M2_DESTROY_ITEM(item);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough inventory space."));
			}
		}
		else if (item->IsChanger())
		{
			int iEmptyPos = ch->GetEmptyChangersInventory(item->GetSize());

			if (iEmptyPos != -1)
			{
#ifdef ENABLE_PICKUP_ITEM_EFFECT
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos), true);
#else
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
				LogManager::instance().ItemLog(ch, item, "GM", item->GetName());
			}
			else
			{
				M2_DESTROY_ITEM(item);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough inventory space."));
			}
		}
#endif
		else
		{
			int iEmptyPos = ch->GetEmptyInventory(item->GetSize());

			if (iEmptyPos != -1)
			{
#ifdef ENABLE_PICKUP_ITEM_EFFECT
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos), true);
#else
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
#endif
				LogManager::instance().ItemLog(ch, item, "GM", item->GetName());
			}
			else
			{
				M2_DESTROY_ITEM(item);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough inventory space."));
			}
		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "#%u vnum'una sahip eþya mevcut deðil.", dwVnum);
	}
}

ACMD(do_group_random)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: grrandom <group vnum>");
		return;
	}

	DWORD dwVnum = 0;
	str_to_number(dwVnum, arg1);
	CHARACTER_MANAGER::instance().SpawnGroupGroup(dwVnum, ch->GetMapIndex(), ch->GetX() - 500, ch->GetY() - 500, ch->GetX() + 500, ch->GetY() + 500);
}

ACMD(do_group)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: group <group vnum>");
		return;
	}

	DWORD dwVnum = 0;
	str_to_number(dwVnum, arg1);

	if (test_server)
		sys_log(0, "COMMAND GROUP SPAWN %u at %u %u %u", dwVnum, ch->GetMapIndex(), ch->GetX(), ch->GetY());

	CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, ch->GetMapIndex(), ch->GetX() - 500, ch->GetY() - 500, ch->GetX() + 500, ch->GetY() + 500);
}

ACMD(do_mob_coward)
{
	char	arg1[256], arg2[256];
	DWORD	vnum = 0;
	LPCHARACTER	tch;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mc <vnum>");
		return;
	}

	const CMob * pkMob;

	if (isdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
			vnum = 0;
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
			vnum = pkMob->m_table.dwVnum;
	}

	if (vnum == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu vnum'a sahip bir canavar yok");
		return;
	}

	int iCount = 0;

	if (*arg2)
		str_to_number(iCount, arg2);
	else
		iCount = 1;

	iCount = MIN(20, iCount);

	while (iCount--)
	{
		tch = CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
			ch->GetMapIndex(),
			ch->GetX() - number(200, 750),
			ch->GetY() - number(200, 750),
			ch->GetX() + number(200, 750),
			ch->GetY() + number(200, 750),
			true,
			pkMob->m_table.bType == CHAR_TYPE_STONE);
		if (tch)
			tch->SetCoward();
	}
}

ACMD(do_mob_map)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: mm <vnum>");
		return;
	}

	DWORD vnum = 0;
	str_to_number(vnum, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRandomPosition(vnum, ch->GetMapIndex());

	if (tch)
		ch->ChatPacket(CHAT_TYPE_INFO, "%s, %dx%d konumunda oluþturuldu", tch->GetName(), tch->GetX(), tch->GetY());
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "Oluþturma baþarýsýz.");
}

ACMD(do_mob_aggresive)
{
	char	arg1[256], arg2[256];
	DWORD	vnum = 0;

#ifndef ENABLE_SHOW_MOB_INFO
	LPCHARACTER	tch;
#endif

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mob <mob vnum>");
		return;
	}

	const CMob * pkMob;

	if (isdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
			vnum = 0;
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
			vnum = pkMob->m_table.dwVnum;
	}

	if (vnum == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu vnum'a sahip bir canavar yok");
		return;
	}

	int iCount = 0;

	if (*arg2)
		str_to_number(iCount, arg2);
	else
		iCount = 1;

	iCount = MIN(20, iCount);

	while (iCount--)
	{
#ifdef ENABLE_SHOW_MOB_INFO
		CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
#else
		tch = CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
#endif
			ch->GetMapIndex(),
			ch->GetX() - number(200, 750),
			ch->GetY() - number(200, 750),
			ch->GetX() + number(200, 750),
			ch->GetY() + number(200, 750),
			true,
			pkMob->m_table.bType == CHAR_TYPE_STONE
#ifdef ENABLE_SHOW_MOB_INFO
			,true
#endif
			);

#ifndef ENABLE_SHOW_MOB_INFO
		if (tch)
			tch->SetAggressive();
#endif
	}
}

ACMD(do_mob)
{
	char	arg1[256], arg2[256];
	DWORD	vnum = 0;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mob <mob vnum>");
		return;
	}

	const CMob* pkMob = NULL;

	if (isnhdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
			vnum = 0;
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
			vnum = pkMob->m_table.dwVnum;
	}

	if (vnum == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu vnum'a sahip bir canavar yok");
		return;
	}

	int iCount = 0;

	if (*arg2)
		str_to_number(iCount, arg2);
	else
		iCount = 1;

	if (test_server)
		iCount = MIN(40, iCount);
	else
		iCount = MIN(20, iCount);

	while (iCount--)
	{
		CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
			ch->GetMapIndex(),
			ch->GetX() - number(200, 750),
			ch->GetY() - number(200, 750),
			ch->GetX() + number(200, 750),
			ch->GetY() + number(200, 750),
			true,
			pkMob->m_table.bType == CHAR_TYPE_STONE);
	}
}

ACMD(do_mob_ld)
{
	char	arg1[256], arg2[256], arg3[256], arg4[256];
	DWORD	vnum = 0;

	two_arguments(two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3), arg4, sizeof(arg4));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mob <mob vnum>");
		return;
	}

	const CMob* pkMob = NULL;

	if (isnhdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
			vnum = 0;
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
			vnum = pkMob->m_table.dwVnum;
	}

	if (vnum == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu vnum'a sahip bir canavar yok");
		return;
	}

	int dir = 1;
	long x, y;

	if (*arg2)
		str_to_number(x, arg2);
	if (*arg3)
		str_to_number(y, arg3);
	if (*arg4)
		str_to_number(dir, arg4);

	CHARACTER_MANAGER::instance().SpawnMob(vnum,
		ch->GetMapIndex(),
		x * 100,
		y * 100,
		ch->GetZ(),
		pkMob->m_table.bType == CHAR_TYPE_STONE,
		dir);
}

struct FuncPurge
{
	LPCHARACTER m_pkGM;
	bool m_bAll;

	FuncPurge(LPCHARACTER ch) : m_pkGM(ch), m_bAll(false) {}

	void operator () (LPENTITY ent)
	{
		if (!ent->IsType(ENTITY_CHARACTER))
			return;

		LPCHARACTER pkChr = (LPCHARACTER) ent;

		int iDist = DISTANCE_APPROX(pkChr->GetX() - m_pkGM->GetX(), pkChr->GetY() - m_pkGM->GetY());

		if (!m_bAll && iDist >= 1000)
			return;

#ifdef ENABLE_RENEWAL_OFFLINESHOP
		if (pkChr->IsOfflineShopNPC())
			return;
#endif

#ifdef ENABLE_BOT_PLAYER
		if (pkChr->IsBotCharacter())
			return;
#endif

#ifdef ENABLE_ULTIMATE_REGEN
		if(pkChr->GetProtectTime("IAMBOSS"))
			return;
#endif

		sys_log(0, "PURGE: %s %d", pkChr->GetName(), iDist);

		if (pkChr->IsNPC() && !pkChr->IsPet()
#ifdef ENABLE_MOUNT_SYSTEM
			&& !pkChr->IsMount()
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
			&& !pkChr->IsGrowthPet()
#endif
			&& pkChr->GetRider() == NULL
		)

		{
			M2_DESTROY_CHARACTER(pkChr);
		}
	}
};

ACMD(do_purge)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	FuncPurge func(ch);

	if (*arg1 && !strcmp(arg1, "map"))
	{
		CHARACTER_MANAGER::instance().DestroyCharacterInMap(ch->GetMapIndex());
	}
	else
	{
		if (*arg1 && !strcmp(arg1, "all"))
			func.m_bAll = true;
		LPSECTREE sectree = ch->GetSectree();

		if (sectree)
			sectree->ForEachAround(func);
		else
			sys_err("PURGE_ERROR.NULL_SECTREE(mapIndex=%d, pos=(%d, %d)", ch->GetMapIndex(), ch->GetX(), ch->GetY());
	}
}

ACMD(do_item_purge)
{
	int i;
	LPITEM item;

	for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		if ((item = ch->GetInventoryItem(i)))
		{
#ifdef ENABLE_GROWTH_PET_SYSTEM
			if (item->GetType() == ITEM_GROWTH_PET)
			{
				if (item->GetSubType() == PET_UPBRINGING || item->GetSubType() == PET_BAG)
					CGrowthPetManager::Instance().DeleteGrowthPet(item->GetSocket(2), true);
			}
#endif
			ITEM_MANAGER::instance().RemoveItem(item, "PURGE");
			ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
		}
	}
	for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = ch->GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i ))))
		{
			ITEM_MANAGER::instance().RemoveItem(item, "PURGE");
		}
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (ch)
	{
		ch->SetPart(PART_WEAPON, 0);
		ch->UpdatePacket();
	}
#endif
	ch->ComputePoints(); //@fixme300
}

ACMD(do_state)
{
	char arg1[256];
	LPCHARACTER tch;

	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		if (arg1[0] == '#')
		{
			tch = CHARACTER_MANAGER::instance().Find(strtoul(arg1 + 1, NULL, 10));
		}
		else
		{
			LPDESC d = DESC_MANAGER::instance().FindByCharacterName(arg1);

			if (!d)
				tch = NULL;
			else
				tch = d->GetCharacter();
		}
	}
	else
		tch = ch;

	if (!tch)
		return;

	char buf[256];

	snprintf(buf, sizeof(buf), "%s's State: ", tch->GetName());

	if (tch->IsPosition(POS_FIGHTING))
		strlcat(buf, "Battle", sizeof(buf));
	else if (tch->IsPosition(POS_DEAD))
		strlcat(buf, "Dead", sizeof(buf));
	else
		strlcat(buf, "Standing", sizeof(buf));

	if (ch->GetShop())
		strlcat(buf, ", Shop", sizeof(buf));

	if (ch->GetExchange())
		strlcat(buf, ", Exchange", sizeof(buf));

	ch->ChatPacket(CHAT_TYPE_INFO, "%s", buf);

	int len = snprintf(buf, sizeof(buf), "Coordinate %ldx%ld (%ldx%ld)",
		tch->GetX(), tch->GetY(), tch->GetX() / 100, tch->GetY() / 100);

	len = snprintf(buf, sizeof(buf), "Hostname %s Channel %u (port %u)", g_stHostname.c_str(), g_bChannel, mother_port);

	if (len < 0 || len >= (int)sizeof(buf))
		len = sizeof(buf) - 1;

	LPSECTREE pSec = SECTREE_MANAGER::instance().Get(tch->GetMapIndex(), tch->GetX(), tch->GetY());
	if (pSec)
	{
		TMapSetting& map_setting = SECTREE_MANAGER::instance().GetMap(tch->GetMapIndex())->m_setting;
		snprintf(buf + len, sizeof(buf) - len, " MapIndex %ld Attribute %08X Local Position (%ld x %ld)",
			tch->GetMapIndex(), pSec->GetAttribute(tch->GetX(), tch->GetY()), (tch->GetX() - map_setting.iBaseX) / 100, (tch->GetY() - map_setting.iBaseY) / 100);
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "%s", buf);

	ch->ChatPacket(CHAT_TYPE_INFO, "LEV %d", tch->GetLevel());
	ch->ChatPacket(CHAT_TYPE_INFO, "HP %d/%d", tch->GetHP(), tch->GetMaxHP());
	ch->ChatPacket(CHAT_TYPE_INFO, "SP %d/%d", tch->GetSP(), tch->GetMaxSP());
	ch->ChatPacket(CHAT_TYPE_INFO, "ATT %d MAGIC_ATT %d SPD %d CRIT %d%% PENE %d%% ATT_BONUS %d%%",
		tch->GetPoint(POINT_ATT_GRADE),
		tch->GetPoint(POINT_MAGIC_ATT_GRADE),
		tch->GetPoint(POINT_ATT_SPEED),
		tch->GetPoint(POINT_CRITICAL_PCT),
		tch->GetPoint(POINT_PENETRATE_PCT),
		tch->GetPoint(POINT_ATT_BONUS));
	ch->ChatPacket(CHAT_TYPE_INFO, "DEF %d MAGIC_DEF %d BLOCK %d%% DODGE %d%% DEF_BONUS %d%%",
		tch->GetPoint(POINT_DEF_GRADE),
		tch->GetPoint(POINT_MAGIC_DEF_GRADE),
		tch->GetPoint(POINT_BLOCK),
		tch->GetPoint(POINT_DODGE),
		tch->GetPoint(POINT_DEF_BONUS));
#ifdef ENABLE_MOUNT_COSTUME_EX_SYSTEM
	ch->ChatPacket(CHAT_TYPE_INFO, "MOUNT %d", tch->GetPoint(POINT_MOUNT));
#endif

	ch->ChatPacket(CHAT_TYPE_INFO, "RESISTANCES:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   WARR:%3d%% ASAS:%3d%% SURA:%3d%% SHAM:%3d%%"
#ifdef ENABLE_WOLFMAN_CHARACTER
		" WOLF:%3d%%"
#endif
		" HUMAN:%3d%%"
		,
		tch->GetPoint(POINT_RESIST_WARRIOR),
		tch->GetPoint(POINT_RESIST_ASSASSIN),
		tch->GetPoint(POINT_RESIST_SURA),
		tch->GetPoint(POINT_RESIST_SHAMAN),
#ifdef ENABLE_WOLFMAN_CHARACTER
		tch->GetPoint(POINT_RESIST_WOLFMAN),
#endif
		tch->GetPoint(POINT_RESIST_HUMAN)
	);
	ch->ChatPacket(CHAT_TYPE_INFO, "   SWORD:%3d%% THSWORD:%3d%% DAGGER:%3d%% BELL:%3d%% FAN:%3d%% BOW:%3d%%"
#ifdef ENABLE_WOLFMAN_CHARACTER
		" CLAW:%3d%%"
#endif
		,
		tch->GetPoint(POINT_RESIST_SWORD),
		tch->GetPoint(POINT_RESIST_TWOHAND),
		tch->GetPoint(POINT_RESIST_DAGGER),
		tch->GetPoint(POINT_RESIST_BELL),
		tch->GetPoint(POINT_RESIST_FAN),
		tch->GetPoint(POINT_RESIST_BOW)
#ifdef ENABLE_WOLFMAN_CHARACTER
		, tch->GetPoint(POINT_RESIST_CLAW)
#endif
	);

	ch->ChatPacket(CHAT_TYPE_INFO, "   ELEC:%3d%% FIRE:%3d%% ICE:%3d%% WIND:%3d%% EARTH:%3d%% DARK:%3d%%",
		tch->GetPoint(POINT_RESIST_ELEC),
		tch->GetPoint(POINT_RESIST_FIRE),
		tch->GetPoint(POINT_RESIST_ICE),
		tch->GetPoint(POINT_RESIST_WIND),
		tch->GetPoint(POINT_RESIST_EARTH),
		tch->GetPoint(POINT_RESIST_DARK));

	ch->ChatPacket(CHAT_TYPE_INFO, "   MAGIC:%3d%% CRIT:%3d%% PENE:%3d%% MOUNT_FALL:%3d%%",
		tch->GetPoint(POINT_RESIST_MAGIC),
		tch->GetPoint(POINT_RESIST_CRITICAL),
		tch->GetPoint(POINT_RESIST_PENETRATE),
		tch->GetPoint(POINT_RESIST_MOUNT_FALL)
	);

	ch->ChatPacket(CHAT_TYPE_INFO, "   ZODIAC:%3d%% INSECT:%3d%% DESERT:%3d%%",
		tch->GetPoint(POINT_ATTBONUS_CZ),
		tch->GetPoint(POINT_ATTBONUS_INSECT),
		tch->GetPoint(POINT_ATTBONUS_DESERT));

#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
	ch->ChatPacket(CHAT_TYPE_INFO, "   MAGIC REDUCTION:%3d%%", tch->GetPoint(POINT_RESIST_MAGIC_REDUCTION));
#endif

	ch->ChatPacket(CHAT_TYPE_INFO, "ENCHANT:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   ELEC:%3d%% FIRE:%3d%% ICE:%3d%% WIND:%3d%% EARTH:%3d%% DARK:%3d%%",
		tch->GetPoint(POINT_ENCHANT_ELECT),
		tch->GetPoint(POINT_ENCHANT_FIRE),
		tch->GetPoint(POINT_ENCHANT_ICE),
		tch->GetPoint(POINT_ENCHANT_WIND),
		tch->GetPoint(POINT_ENCHANT_EARTH),
		tch->GetPoint(POINT_ENCHANT_DARK));

	ch->ChatPacket(CHAT_TYPE_INFO, "MALL:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   ATT:%3d%% DEF:%3d%% EXP:%3d%% ITEMx%d GOLDx%d",
		tch->GetPoint(POINT_MALL_ATTBONUS),
		tch->GetPoint(POINT_MALL_DEFBONUS),
		tch->GetPoint(POINT_MALL_EXPBONUS),
		tch->GetPoint(POINT_MALL_ITEMBONUS) / 10,
		tch->GetPoint(POINT_MALL_GOLDBONUS) / 10);

	ch->ChatPacket(CHAT_TYPE_INFO, "BONUS:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   SKILL:%3d%% NORMAL:%3d%% SKILL_DEF:%3d%% NORMAL_DEF:%3d%%",
		tch->GetPoint(POINT_SKILL_DAMAGE_BONUS),
		tch->GetPoint(POINT_NORMAL_HIT_DAMAGE_BONUS),
		tch->GetPoint(POINT_SKILL_DEFEND_BONUS),
		tch->GetPoint(POINT_NORMAL_HIT_DEFEND_BONUS));

	ch->ChatPacket(CHAT_TYPE_INFO, "ATTBONUS:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   HUMAN:%3d%% ANIMAL:%3d%% ORC:%3d%% MILGYO:%3d%% UNDEAD:%3d%%",
		tch->GetPoint(POINT_ATTBONUS_HUMAN),
		tch->GetPoint(POINT_ATTBONUS_ANIMAL),
		tch->GetPoint(POINT_ATTBONUS_ORC),
		tch->GetPoint(POINT_ATTBONUS_MILGYO),
		tch->GetPoint(POINT_ATTBONUS_UNDEAD));

	ch->ChatPacket(CHAT_TYPE_INFO, "   DEVIL:%3d%% INSECT:%3d%% FIRE:%3d%% ICE:%3d%% DESERT:%3d%%",
		tch->GetPoint(POINT_ATTBONUS_DEVIL),
		tch->GetPoint(POINT_ATTBONUS_INSECT),
		tch->GetPoint(POINT_ATTBONUS_FIRE),
		tch->GetPoint(POINT_ATTBONUS_ICE),
		tch->GetPoint(POINT_ATTBONUS_DESERT));

	ch->ChatPacket(CHAT_TYPE_INFO, "   TREE:%3d%% MONSTER:%3d%%"
#ifdef ENABLE_AVG_PVM
			"MEDI_PVM:%3d%%"
#endif
		,
		tch->GetPoint(POINT_ATTBONUS_TREE),
		tch->GetPoint(POINT_ATTBONUS_MONSTER)
#ifdef ENABLE_AVG_PVM
		,tch->GetPoint(POINT_ATTBONUS_MEDI_PVM)
#endif
		);

	ch->ChatPacket(CHAT_TYPE_INFO, "   WARR:%3d%% ASSA:%3d%% SURA:%3d%% SHAM:%3d%%"
#ifdef ENABLE_WOLFMAN_CHARACTER
		" WOLF:%3d%%"
#endif
		,
		tch->GetPoint(POINT_ATTBONUS_WARRIOR),
		tch->GetPoint(POINT_ATTBONUS_ASSASSIN),
		tch->GetPoint(POINT_ATTBONUS_SURA),
		tch->GetPoint(POINT_ATTBONUS_SHAMAN)
#ifdef ENABLE_WOLFMAN_CHARACTER
		, tch->GetPoint(POINT_ATTBONUS_WOLFMAN)
#endif
	);

	ch->ChatPacket(CHAT_TYPE_INFO, "   SWORD:%3d%% THSWORD:%3d%% DAGGER:%3d%% BELL:%3d%% FAN:%3d%% BOW:%3d%%"
#ifdef ENABLE_WOLFMAN_CHARACTER
		" CLAW:%3d%%"
#endif
		,
		tch->GetPoint(POINT_ATTBONUS_SWORD),
		tch->GetPoint(POINT_ATTBONUS_TWOHAND),
		tch->GetPoint(POINT_ATTBONUS_DAGGER),
		tch->GetPoint(POINT_ATTBONUS_BELL),
		tch->GetPoint(POINT_ATTBONUS_FAN),
		tch->GetPoint(POINT_ATTBONUS_BOW)
#ifdef ENABLE_WOLFMAN_CHARACTER
		, tch->GetPoint(POINT_ATTBONUS_CLAW)
#endif
	);

	ch->ChatPacket(CHAT_TYPE_INFO, "IMMUNE:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   STUN:%d SLOW:%d FALL:%d",
		tch->GetPoint(POINT_IMMUNE_STUN),
		tch->GetPoint(POINT_IMMUNE_SLOW),
		tch->GetPoint(POINT_IMMUNE_FALL));

	for (int i = 0; i < MAX_PRIV_NUM; ++i)
	{
		if (CPrivManager::instance().GetPriv(tch, i))
		{
			int iByEmpire = CPrivManager::instance().GetPrivByEmpire(tch->GetEmpire(), i);
			int iByGuild = 0;

			if (tch->GetGuild())
				iByGuild = CPrivManager::instance().GetPrivByGuild(tch->GetGuild()->GetID(), i);

			int iByPlayer = CPrivManager::instance().GetPrivByCharacter(tch->GetPlayerID(), i);

			if (iByEmpire)
				tch->LocaleChatPacket(CHAT_TYPE_INFO, 569, "%s#%d", c_apszPrivNames[i], iByEmpire);

			if (iByGuild)
				tch->LocaleChatPacket(CHAT_TYPE_INFO, 570, "%s#%d", c_apszPrivNames[i], iByGuild);

			if (iByPlayer)
				tch->LocaleChatPacket(CHAT_TYPE_INFO, 571, "%s#%d", c_apszPrivNames[i], iByPlayer);
		}
	}
	
	ch->ChatPacket(CHAT_TYPE_INFO, "Kanal %u", g_bChannel);
}

struct notice_packet_func
{
	const char * m_str;

	notice_packet_func(const char * str) : m_str(str) {}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		d->GetCharacter()->ChatPacket(CHAT_TYPE_NOTICE, "%s", m_str);
	}
};

void SendNotice(const char* c_pszBuf)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), notice_packet_func(c_pszBuf));
}

struct notice_map_packet_func
{
	const char* m_str;
	int m_mapIndex;
	bool m_bBigFont;

	notice_map_packet_func(const char* str, int idx, bool bBigFont) : m_str(str), m_mapIndex(idx), m_bBigFont(bBigFont)
	{
	}

	void operator() (LPDESC d)
	{
		if (d->GetCharacter() == NULL) return;
		if (d->GetCharacter()->GetMapIndex() != m_mapIndex) return;

		d->GetCharacter()->ChatPacket(m_bBigFont == true ? CHAT_TYPE_BIG_NOTICE : CHAT_TYPE_NOTICE, "%s", m_str);
	}
};

void SendNoticeMap(const char* c_pszBuf, int nMapIndex, bool bBigFont)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), notice_map_packet_func(c_pszBuf, nMapIndex, bBigFont));
}

#ifdef ENABLE_CLIENT_LOCALE_STRING
struct renewal_packet_notice
{
	BYTE m_type;
	BYTE m_empire;
	long m_mapidx;
	DWORD m_id;
	const char* m_str;

	renewal_packet_notice(BYTE type, BYTE empire, long mapidx, DWORD id, const char* format) : m_type(type), m_empire(empire), m_mapidx(mapidx), m_id(id), m_str(format) {}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (m_empire == 0)
		{
			if (m_mapidx == 0)
			{
				d->GetCharacter()->LocaleChatPacket(m_type, m_id, m_str);
			}
			else if (d->GetCharacter()->GetMapIndex() == m_mapidx)
			{
				d->GetCharacter()->LocaleChatPacket(m_type, m_id, m_str);
			}
		}
		else if (d->GetCharacter()->GetEmpire() == m_empire)
		{
			if (m_mapidx == 0)
			{
				d->GetCharacter()->LocaleChatPacket(m_type, m_id, m_str);
			}
			else if (d->GetCharacter()->GetMapIndex() == m_mapidx)
			{
				d->GetCharacter()->LocaleChatPacket(m_type, m_id, m_str);
			}
		}
	}
};

void SendLocaleNotice(BYTE type, BYTE empire, long mapidx, DWORD id, const char* format, ...)
{
	char chatbuf[256];
	va_list args;
	va_start(args, format);
	vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), renewal_packet_notice(type, empire, mapidx, id, chatbuf));
}

void BroadcastLocaleNotice(BYTE type, BYTE empire, long mapidx, DWORD id, const char * format, ...)
{
	char chatbuf[256];
	va_list args;
	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);
	if (len < 0) len = 0; else if (len >= (int)sizeof(chatbuf)) len = sizeof(chatbuf) - 1; // FIX: truncation'da stack verisi gonderilmesin

	TPacketGGLocaleChatNotice packet;
	packet.header = HEADER_GG_LOCALE_CHAT_NOTICE;
	packet.type = type;
	packet.empire = empire;
	packet.mapidx = mapidx;
	packet.id = id;
	packet.size = len;

	TEMP_BUFFER buf;
	buf.write(&packet, sizeof(packet));

	if (len > 0)
	{
		buf.write(chatbuf, len);
	}

	P2P_MANAGER::instance().Send(buf.read_peek(), buf.size());
	SendLocaleNotice(type, empire, mapidx, id, chatbuf);
}
#endif

#ifdef ENABLE_RENEWAL_OX_EVENT
struct notice_ox_map_packet_func
{
	const char* m_str;
	int m_mapIndex;

	notice_ox_map_packet_func(const char* str, int idx) : m_str(str), m_mapIndex(idx) {}

	void operator() (LPDESC d)
	{
		if (d->GetCharacter() == NULL) return;
		if (d->GetCharacter()->GetMapIndex() != m_mapIndex) return;

		d->GetCharacter()->ChatPacket(CHAT_TYPE_CONTROL_NOTICE, "%s", m_str);
	}
};

void SendNoticeOxMap(const char* c_pszBuf, int nMapIndex)
{
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::Instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), notice_ox_map_packet_func(c_pszBuf, nMapIndex));
}
#endif

struct log_packet_func
{
	const char * m_str;

	log_packet_func(const char * str) : m_str(str) {}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetGMLevel() > GM_PLAYER)
			d->GetCharacter()->ChatPacket(CHAT_TYPE_NOTICE, "%s", m_str);
	}
};


void SendLog(const char * c_pszBuf)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), log_packet_func(c_pszBuf));
}

void BroadcastNotice(const char * c_pszBuf)
{
	TPacketGGNotice p;
	p.bHeader = HEADER_GG_NOTICE;
	p.lSize = strlen(c_pszBuf) + 1;

	TEMP_BUFFER buf;
	buf.write(&p, sizeof(p));
	buf.write(c_pszBuf, p.lSize);

	P2P_MANAGER::instance().Send(buf.read_peek(), buf.size());

	SendNotice(c_pszBuf);
}

ACMD(do_notice)
{
	char chatbuf[CHAT_MAX_LEN + 1];
	snprintf(chatbuf, sizeof(chatbuf), "%s :%s", ch->GetName(), argument);
	BroadcastNotice(chatbuf);
}

ACMD(do_map_notice)
{
	SendNoticeMap(argument, ch->GetMapIndex(), false);
}

ACMD(do_big_notice)
{
	ch->ChatPacket(CHAT_TYPE_BIG_NOTICE, "%s", argument);
}

ACMD(do_who)
{
	int iTotal;
	int * paiEmpireUserCount;
	int iLocal;

	DESC_MANAGER::instance().GetUserCount(iTotal, &paiEmpireUserCount, iLocal);

	ch->ChatPacket(CHAT_TYPE_INFO, "Toplam [%d] %d / %d / %d (bu sunucu %d)",
			iTotal, paiEmpireUserCount[1], paiEmpireUserCount[2], paiEmpireUserCount[3], iLocal);
}

class user_func
{
	public:
		LPCHARACTER	m_ch;
		static int count;
		static char str[128];
		static int str_len;

		user_func()
			: m_ch(NULL)
		{}

		void initialize(LPCHARACTER ch)
		{
			m_ch = ch;
			str_len = 0;
			count = 0;
			str[0] = '\0';
		}

		void operator () (LPDESC d)
		{
			if (!d->GetCharacter())
				return;

#ifdef ENABLE_COORDINATES_COMMAND
			TMapSetting& map_setting = SECTREE_MANAGER::instance().GetMap(d->GetCharacter()->GetMapIndex())->m_setting;
			int len = snprintf(str + str_len, sizeof(str) - str_len, "%s (%ld,%ld)%-16s", d->GetCharacter()->GetName(),(d->GetCharacter()->GetX() - map_setting.iBaseX)/100,(d->GetCharacter()->GetY() - map_setting.iBaseY)/100,"");
#else
			int len = snprintf(str + str_len, sizeof(str) - str_len, "%-16s ", d->GetCharacter()->GetName());
#endif
			if (len < 0 || len >= (int) sizeof(str) - str_len)
				len = (sizeof(str) - str_len) - 1;

			str_len += len;
			++count;

			if (!(count % 4))
			{
				m_ch->ChatPacket(CHAT_TYPE_INFO, "%s", str);

				str[0] = '\0';
				str_len = 0;
			}
		}
};

int	user_func::count = 0;
char user_func::str[128] = { 0, };
int	user_func::str_len = 0;

ACMD(do_user)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	user_func func;

	func.initialize(ch);
	std::for_each(c_ref_set.begin(), c_ref_set.end(), func);

	if (func.count % 4)
		ch->ChatPacket(CHAT_TYPE_INFO, "%s", func.str);

	ch->ChatPacket(CHAT_TYPE_INFO, "Toplam %d", func.count);
}

ACMD(do_disconnect)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "ex) /dc <player name>");
		return;
	}

	LPDESC d = DESC_MANAGER::instance().FindByCharacterName(arg1);
	LPCHARACTER	tch = d ? d->GetCharacter() : NULL;

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s: no such a player.", arg1);
		return;
	}

	if (tch == ch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "cannot disconnect myself");
		return;
	}

	DESC_MANAGER::instance().DestroyDesc(d);
}

ACMD(do_kill)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "ex) /kill <player name>");
		return;
	}

	LPDESC	d = DESC_MANAGER::instance().FindByCharacterName(arg1);
	LPCHARACTER tch = d ? d->GetCharacter() : NULL;

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s: no such a player", arg1);
		return;
	}

	tch->Dead();
}

#define MISC    0
#define BINARY  1
#define NUMBER  2

const struct set_struct
{
	const char *cmd;
	const char type;
}

set_fields[] =
{
	{ "gold",		NUMBER	},
	{ "race",		BINARY	},
	{ "sex",		BINARY	},
	{ "exp",		NUMBER	},
	{ "max_hp",		NUMBER	},
	{ "max_sp",		NUMBER	},
	{ "skill",		NUMBER	},
	{ "alignment",	NUMBER	},
	{ "align",		NUMBER	},
#ifdef ENABLE_COINS_INVENTORY
	{ "coins",		NUMBER	},
#endif
#ifdef ENABLE_GAYA_SYSTEM
	{ "gem",		NUMBER	},
#endif
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	{ "soul",		NUMBER	},
	{ "soulre",		NUMBER	},
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
	{ "cexp",		NUMBER	},
#endif
	{ "\n",		MISC	}
};

ACMD(do_set)
{
	char arg1[256], arg2[256], arg3[256];

	LPCHARACTER tch = NULL;

	int i, len;
	const char* line;

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: set <name> <field> <value>");
		return;
	}

	tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s mevcut deðil.", arg1);
		return;
	}

	len = strlen(arg2);

	for (i = 0; *(set_fields[i].cmd) != '\n'; i++)
		if (!strncmp(arg2, set_fields[i].cmd, len))
			break;

	switch (i)
	{
		case 0:
			{
#ifdef ENABLE_GOLD_LIMIT
				long long gold = 0;
#else
				int gold = 0;
#endif
				str_to_number(gold, arg3);
				DBManager::instance().SendMoneyLog(MONEY_LOG_MISC, 3, gold);
				int before_gold = tch->GetGold();
				tch->PointChange(POINT_GOLD, gold, true);
				int after_gold = tch->GetGold();
				if (0 == after_gold && 0 != before_gold)
				{
					LogManager::instance().CharLog(tch, gold, "ZERO_GOLD", "GM");
				}
			}
			break;

		case 1:
			break;

		case 2:
			break;

		case 3:
			{
				int amount = 0;
				str_to_number(amount, arg3);
				tch->PointChange(POINT_EXP, amount, true);
			}
			break;

		case 4:
			{
				int amount = 0;
				str_to_number(amount, arg3);
				tch->PointChange(POINT_MAX_HP, amount, true);
			}
			break;

		case 5:
			{
				int amount = 0;
				str_to_number(amount, arg3);
				tch->PointChange(POINT_MAX_SP, amount, true);
			}
			break;

		case 6:
			{
				int amount = 0;
				str_to_number(amount, arg3);
				tch->PointChange(POINT_SKILL, amount, true);
			}
			break;

		case 7:
		case 8:
			{
				int amount = 0;
				str_to_number(amount, arg3);
				//tch->UpdateAlignment(amount - ch->GetRealAlignment());
				tch->UpdateAlignment(amount - tch->GetRealAlignment());	//GM'lerin /set karakteradý alignment ile derece verdikleri komutun hata düzeltmesidir.
			}
			break;

#ifdef ENABLE_COINS_INVENTORY
		case 9:
			{
				long long coins = 0;
				str_to_number(coins, arg3);
				tch->PointChange(POINT_COINS, coins, true);
			}
			break;
#endif

#ifdef ENABLE_GAYA_SYSTEM
		case 10:
			{
				long long gem = 0;
				str_to_number(gem, arg3);
				tch->PointChange(POINT_GEM, gem, true);
			}
			break;
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
		case 11:
			{
				int amount = 0;
				str_to_number(amount, arg3);
				if (amount + tch->GetSoulPoint() > SOUL_MAX)
					return;
				tch->PointChange(POINT_SOUL, amount, true);
			}
			break;

		case 12:
			{
				int amount = 0;
				str_to_number(amount, arg3);
				if (amount + tch->GetSoulRePoint() > SOUL_RE_MAX)
					return;
				tch->PointChange(POINT_SOUL_RE, amount, true);
			}
			break;
#endif

#ifdef ENABLE_CONQUEROR_LEVEL
		case 13:
			{
				int amount = 0;
				str_to_number(amount, arg3);
				tch->PointChange(POINT_CONQUEROR_EXP, amount, true);
			}
			break;
#endif
	}

	if (set_fields[i].type == NUMBER)
	{
		int	amount = 0;
		str_to_number(amount, arg3);
		ch->ChatPacket(CHAT_TYPE_INFO, "%s's %s set to [%d]", tch->GetName(), set_fields[i].cmd, amount);
	}
}

ACMD(do_reset)
{
	ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
	ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
	ch->Save();
}

ACMD(do_advance)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: advance <name> <level>");
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s mevcut deðil.", arg1);
		return;
	}

	int level = 0;
	str_to_number(level, arg2);

	tch->ResetPoint(MINMAX(0, level, gPlayerMaxLevel));
}

ACMD(do_respawn)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1 && !strcasecmp(arg1, "all"))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Respawn Everywhere");
		regen_reset(0, 0);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Respawn Around");
		regen_reset(ch->GetX(), ch->GetY());
	}
}

ACMD(do_safebox_size)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int size = 0;

	if (*arg1)
		str_to_number(size, arg1);

	if (size > 3 || size < 0)
		size = 0;

	ch->ChatPacket(CHAT_TYPE_INFO, "Depo boyutu %d olarak ayarlandý", size);
	ch->ChangeSafeboxSize(size);
}

ACMD(do_makeguild)
{
	if (ch->GetGuild())
		return;

	CGuildManager& gm = CGuildManager::instance();

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	TGuildCreateParameter cp{};

	cp.master = ch;
	strlcpy(cp.name, arg1, sizeof(cp.name));

	if (!check_name(cp.name))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This guild name is invalid."));
		return;
	}

	auto guildID = gm.CreateGuild(cp);
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("(%s) guild has been created. [Temporary]"), cp.name);
}

ACMD(do_deleteguild)
{
	if (ch->GetGuild())
		ch->GetGuild()->RequestDisband(ch->GetPlayerID());
}

ACMD(do_greset)
{
	if (ch->GetGuild())
		ch->GetGuild()->Reset();
}

// REFINE_ROD_HACK_BUG_FIX
ACMD(do_refine_rod)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	BYTE cell = 0;
	str_to_number(cell, arg1);
	LPITEM item = ch->GetInventoryItem(cell);

	if (item)
		fishing::RealRefineRod(ch, item);
}
// END_OF_REFINE_ROD_HACK_BUG_FIX

// REFINE_PICK
ACMD(do_refine_pick)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	BYTE cell = 0;
	str_to_number(cell, arg1);
	LPITEM item = ch->GetInventoryItem(cell);

	if (item)
	{
		mining::CHEAT_MAX_PICK(ch, item);
		mining::RealRefinePick(ch, item);
	}
}

ACMD(do_max_pick)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	BYTE cell = 0;
	str_to_number(cell, arg1);
	LPITEM item = ch->GetInventoryItem(cell);
	if (item)
	{
		mining::CHEAT_MAX_PICK(ch, item);
	}
}
// END_OF_REFINE_PICK

ACMD(do_fishing_simul)
{
	char arg1[256];
	char arg2[256];
	char arg3[256];
	argument = one_argument(argument, arg1, sizeof(arg1));
	two_arguments(argument, arg2, sizeof(arg2), arg3, sizeof(arg3));

	int count = 1000;
	int prob_idx = 0;
	int level = 100;

	ch->ChatPacket(CHAT_TYPE_INFO, "Usage: fishing_simul <level> <prob index> <count>");

	if (*arg1)
		str_to_number(level, arg1);

	if (*arg2)
		str_to_number(prob_idx, arg2);

	if (*arg3)
		str_to_number(count, arg3);

	fishing::Simulation(level, count, prob_idx, ch);
}

ACMD(do_invisibility)
{
	if (!ch)
		return;

	#ifdef ENABLE_GM_INV_EFFECT
	if (ch->IsAffectFlag(AFF_INVISIBILITY))
	{
		ch->RemoveAffect(AFFECT_INVISIBILITY);
		ch->SpecificEffectPacket("d:\\ymir work\\effect\\monster\\yellow_tigerman_24_1.mse");
	}
	#else
	if (ch->IsAffectFlag(AFF_INVISIBILITY))
	{
		ch->RemoveAffect(AFFECT_INVISIBILITY);
	}
	#endif
	else
	{
		ch->AddAffect(AFFECT_INVISIBILITY, POINT_NONE, 0, AFF_INVISIBILITY, INFINITE_AFFECT_DURATION, 0, true);
	}
}

ACMD(do_event_flag)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!(*arg1) || !(*arg2))
		return;

	int value = 0;
	str_to_number(value, arg2);

	if (!strcmp(arg1, "mob_item") ||
		!strcmp(arg1, "mob_exp") ||
		!strcmp(arg1, "mob_gold") ||
		!strcmp(arg1, "mob_dam") ||
		!strcmp(arg1, "mob_gold_pct") ||
		!strcmp(arg1, "mob_item_buyer") ||
		!strcmp(arg1, "mob_exp_buyer") ||
		!strcmp(arg1, "mob_gold_buyer") ||
		!strcmp(arg1, "mob_gold_pct_buyer")
		)
		value = MINMAX(0, value, 1000);

	quest::CQuestManager::instance().RequestSetEventFlag(arg1, value);
	ch->ChatPacket(CHAT_TYPE_INFO, "RequestSetEventFlag %s %d", arg1, value);
	sys_log(0, "RequestSetEventFlag %s %d", arg1, value);
}

ACMD(do_get_event_flag)
{
	quest::CQuestManager::instance().SendEventFlagList(ch);
}

ACMD(do_private)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Kullaným: private <harita indeksi>");
		return;
	}

	long lMapIndex;
	long map_index = 0;
	str_to_number(map_index, arg1);
	if ((lMapIndex = SECTREE_MANAGER::instance().CreatePrivateMap(map_index)))
	{
		ch->SaveExitLocation();

		LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
		ch->WarpSet(pkSectreeMap->m_setting.posSpawn.x, pkSectreeMap->m_setting.posSpawn.y, lMapIndex);
	}
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "%d indeksine sahip harita bulunamadý", map_index);
}

ACMD(do_qf)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
#ifdef ENABLE_CH_CRASH_CORE_FIX
	if (!pPC)
	{
		return;
	}
#endif
	std::string questname = pPC->GetCurrentQuestName();

	if (!questname.empty())
	{
		int value = quest::CQuestManager::Instance().GetQuestStateIndex(questname, arg1);

		pPC->SetFlag(questname + ".__status", value);
		pPC->ClearTimer();

		quest::PC::QuestInfoIterator it = pPC->quest_begin();
		unsigned int questindex = quest::CQuestManager::instance().GetQuestIndexByName(questname);

		while (it!= pPC->quest_end())
		{
			if (it->first == questindex)
			{
				it->second.st = value;
				break;
			}

			++it;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, "Görev durumu bayraðý ayarlandý: %s %s %d", questname.c_str(), arg1, value);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Görev durumu bayraðý ayarlanamadý");
	}
}

LPCHARACTER chHori, chForge, chLib, chTemple, chTraining, chTree, chPortal, chBall;

ACMD(do_b1)
{
	chHori = CHARACTER_MANAGER::instance().SpawnMobRange(14017, ch->GetMapIndex(), 304222, 742858, 304222, 742858, true, false);
	chHori->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_BUILDING_CONSTRUCTION_SMALL, 65535, 0, true);
	chHori->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);

	for (int i = 0; i < 30; ++i)
	{
		int rot = number(0, 359);
		float fx, fy;
		GetDeltaByDegree(rot, 800, &fx, &fy);

		LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRange(number(701, 706),
				ch->GetMapIndex(),
				304222 + (int)fx,
				742858 + (int)fy,
				304222 + (int)fx,
				742858 + (int)fy,
				true,
				false);
		tch->SetAggressive();
	}

	for (int i = 0; i < 5; ++i)
	{
		int rot = number(0, 359);
		float fx, fy;
		GetDeltaByDegree(rot, 800, &fx, &fy);

		LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRange(8009,
				ch->GetMapIndex(),
				304222 + (int)fx,
				742858 + (int)fy,
				304222 + (int)fx,
				742858 + (int)fy,
				true,
				false);
		tch->SetAggressive();
	}
}

ACMD(do_b2)
{
	chHori->RemoveAffect(AFFECT_DUNGEON_UNIQUE);
}

ACMD(do_b3)
{
	chForge = CHARACTER_MANAGER::instance().SpawnMobRange(14003, ch->GetMapIndex(), 307500, 746300, 307500, 746300, true, false);
	chForge->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);

	chLib = CHARACTER_MANAGER::instance().SpawnMobRange(14007, ch->GetMapIndex(), 307900, 744500, 307900, 744500, true, false);
	chLib->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);

	chTemple = CHARACTER_MANAGER::instance().SpawnMobRange(14004, ch->GetMapIndex(), 307700, 741600, 307700, 741600, true, false);
	chTemple->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);

	chTraining= CHARACTER_MANAGER::instance().SpawnMobRange(14010, ch->GetMapIndex(), 307100, 739500, 307100, 739500, true, false);
	chTraining->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);

	chTree= CHARACTER_MANAGER::instance().SpawnMobRange(14013, ch->GetMapIndex(), 300800, 741600, 300800, 741600, true, false);
	chTree->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);

	chPortal= CHARACTER_MANAGER::instance().SpawnMobRange(14001, ch->GetMapIndex(), 300900, 744500, 300900, 744500, true, false);
	chPortal->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);

	chBall = CHARACTER_MANAGER::instance().SpawnMobRange(14012, ch->GetMapIndex(), 302500, 746600, 302500, 746600, true, false);
	chBall->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);
}

ACMD(do_b4)
{
	chLib->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_BUILDING_UPGRADE, 65535, 0, true);

	for (int i = 0; i < 30; ++i)
	{
		int rot = number(0, 359);
		float fx, fy;
		GetDeltaByDegree(rot, 1200, &fx, &fy);

		LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRange(number(701, 706),
				ch->GetMapIndex(),
				307900 + (int)fx,
				744500 + (int)fy,
				307900 + (int)fx,
				744500 + (int)fy,
				true,
				false);
		tch->SetAggressive();
	}

	for (int i = 0; i < 5; ++i)
	{
		int rot = number(0, 359);
		float fx, fy;
		GetDeltaByDegree(rot, 1200, &fx, &fy);

		LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRange(8009,
				ch->GetMapIndex(),
				307900 + (int)fx,
				744500 + (int)fy,
				307900 + (int)fx,
				744500 + (int)fy,
				true,
				false);
		tch->SetAggressive();
	}
}

ACMD(do_b5)
{
	M2_DESTROY_CHARACTER(chLib);

	chLib = CHARACTER_MANAGER::instance().SpawnMobRange(14008, ch->GetMapIndex(), 307900, 744500, 307900, 744500, true, false);
	chLib->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);
}

ACMD(do_b6)
{
	chLib->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_BUILDING_UPGRADE, 65535, 0, true);
}
ACMD(do_b7)
{
	M2_DESTROY_CHARACTER(chLib);

	chLib = CHARACTER_MANAGER::instance().SpawnMobRange(14009, ch->GetMapIndex(), 307900, 744500, 307900, 744500, true, false);
	chLib->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);
}

ACMD(do_book)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	CSkillProto * pkProto;

	if (isnhdigit(*arg1))
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		pkProto = CSkillManager::instance().Get(vnum);
	}
	else
		pkProto = CSkillManager::instance().Get(arg1);

	if (!pkProto)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Böyle bir yetenek yok.");
		return;
	}

	LPITEM item = ch->AutoGiveItem(50300);
	if (!item)  // FIX: AutoGiveItem can return NULL (dead/stun/missing proto) -> guard deref
		return;
	item->SetSocket(0, pkProto->dwVnum);
}

ACMD(do_setskillother)
{
	char arg1[256], arg2[256], arg3[256];
	argument = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(argument, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3 || !isdigit(*arg3))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: setskillother <target> <skillname> <lev>");
		return;
	}

	LPCHARACTER tch;

	tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Böyle bir karakter yok");
		return;
	}

	CSkillProto * pk;

	if (isdigit(*arg2))
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg2);
		pk = CSkillManager::instance().Get(vnum);
	}
	else
		pk = CSkillManager::instance().Get(arg2);

	if (!pk)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu isimde bir yetenek bulunamadý.");
		return;
	}

	BYTE level = 0;
	str_to_number(level, arg3);
	tch->SetSkillLevel(pk->dwVnum, level);
	tch->ComputePoints();
	tch->SkillLevelPacket();
}

ACMD(do_setskill)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2 || !isdigit(*arg2))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: setskill <name> <lev>");
		return;
	}

	CSkillProto * pk;

	if (isdigit(*arg1))
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		pk = CSkillManager::instance().Get(vnum);
	}

	else
		pk = CSkillManager::instance().Get(arg1);

	if (!pk)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu isimde bir yetenek bulunamadý.");
		return;
	}

	BYTE level = 0;
	str_to_number(level, arg2);
	ch->SetSkillLevel(pk->dwVnum, level);
	ch->ComputePoints();
	ch->SkillLevelPacket();
}

ACMD(do_set_skill_point)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int skill_point = 0;
	if (*arg1)
		str_to_number(skill_point, arg1);

	ch->SetRealPoint(POINT_SKILL, skill_point);
	ch->SetPoint(POINT_SKILL, ch->GetRealPoint(POINT_SKILL));
	ch->PointChange(POINT_SKILL, 0);
}

ACMD(do_set_skill_group)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int skill_group = 0;
	if (*arg1)
		str_to_number(skill_group, arg1);

	ch->SetSkillGroup(skill_group);

	ch->ClearSkill();
	ch->ChatPacket(CHAT_TYPE_INFO, "skill group to %d.", skill_group);
}

ACMD(do_reload)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

#ifdef ENABLE_RELOAD_COMMAND_ALL_CORES
	TPacketGGReloadCommand p2p_packet;
	p2p_packet.header = HEADER_GG_RELOAD_COMMAND;
	strlcpy(p2p_packet.argument, arg1, sizeof(p2p_packet.argument));
	bool bSendP2P = false;
#endif

	if (*arg1)
	{
		switch (LOWER(*arg1))
		{
			// u l p s q y i f a c x r z
			// /reload u		// "Reloading state_user_count."
			// /reload l		// "Reloading new banwords"
			// /reload p		// "Reloading prototype tables"
			// /reload s		// "Reloading notice string"
			// /reload q		// "Reloading quest"
			// /reload y		// "Reloading common_drop_new.txt"
			// /reload i		// "Reloading New Regen newregen.txt"
			// /reload f		// "Reloading fishing"
			// /reload a		// "Reloading Admin infomation."
			// /reload c		// "Reloading cube"
			// /reload x		// "Reloading anticheat blacklist."	
			// /reload r		// "Reloading New Regen newregen.txt"
			// /reload z		// ITEMSHOP
			case 'u':
				ch->ChatPacket(CHAT_TYPE_INFO, "Kullanýcý durum sayýsý (state_user_count) yeniden yükleniyor.");
				LoadStateUserCount();
				break;

			case 'p':
				ch->ChatPacket(CHAT_TYPE_INFO, "Prototip tablolar yeniden yükleniyor,");
				db_clientdesc->DBPacket(HEADER_GD_RELOAD_PROTO, 0, NULL, 0);
				break;

			case 's':
				ch->ChatPacket(CHAT_TYPE_INFO, "Bildirim metni yeniden yükleniyor.");
				DBManager::instance().LoadDBString();
				break;

			case 'q':
				ch->ChatPacket(CHAT_TYPE_INFO, "Görevler yeniden yükleniyor.");
				quest::CQuestManager::instance().Reload();
				break;

			case 'f':
				fishing::Initialize();
				break;

				//RELOAD_ADMIN
			case 'a':
				ch->ChatPacket(CHAT_TYPE_INFO, "Yönetici bilgileri yeniden yükleniyor.");
				db_clientdesc->DBPacket(HEADER_GD_RELOAD_ADMIN, 0, NULL, 0);
				sys_log(0, "Yönetici bilgileri yeniden yükleniyor.");
				break;
				//END_RELOAD_ADMIN

			case 'c':	// cube
				// Only local processes are updated.
				Cube_init ();
				break;

#ifdef ENABLE_ULTIMATE_REGEN
			case 'x':
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Newregen.txt yeniden yükleniyor");
				char buf[250];
				snprintf(buf, sizeof(buf),"%s/newregen.txt", LocaleService_GetBasePath().c_str());
				CNewMobTimer::Instance().LoadFile(buf);
				CNewMobTimer::Instance().UpdatePlayers();

				TGGPacketNewRegen p;
				p.header = HEADER_GG_NEW_REGEN;
				p.subHeader = NEW_REGEN_LOAD;
				P2P_MANAGER::Instance().Send(&p, sizeof(p));
			}
			break;
#endif

#ifdef ENABLE_ITEMSHOP
			case 'z':
			{
				BYTE subIndex = ITEMSHOP_RELOAD;

				db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP, 0, sizeof(BYTE));
				db_clientdesc->Packet(&subIndex, sizeof(BYTE));
				break;
			}
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
			case 'r':
				if (!CSoulRoulette::ReadRouletteData()) {
					ch->ChatPacket(CHAT_TYPE_INFO, "Error Reloading <CSoulRoulette>!");
					CSoulRoulette::ReadRouletteData(true); // reset
				}
				else
					ch->ChatPacket(CHAT_TYPE_INFO, "<CSoulRoulette> Reloaded!");
				break;
#endif

			default:
				const int FILE_NAME_LEN = 256;
				if (strstr(arg1, "drop"))
				{
					char szETCDropItemFileName[FILE_NAME_LEN];
					char szMOBDropItemFileName[FILE_NAME_LEN];
					char szSpecialItemGroupFileName[FILE_NAME_LEN];

					snprintf(szETCDropItemFileName, sizeof(szETCDropItemFileName),
						"%s/etc_drop_item.txt", LocaleService_GetBasePath().c_str());
					snprintf(szMOBDropItemFileName, sizeof(szMOBDropItemFileName),
						"%s/mob_drop_item.txt", LocaleService_GetBasePath().c_str());
					snprintf(szSpecialItemGroupFileName, sizeof(szSpecialItemGroupFileName),
						"%s/special_item_group.txt", LocaleService_GetBasePath().c_str());

					ch->ChatPacket(CHAT_TYPE_INFO, "Yeniden yükleniyor: ETCDropItem: %s", szETCDropItemFileName);
					if (!ITEM_MANAGER::instance().ReadEtcDropItemFile(szETCDropItemFileName, true))
						ch->ChatPacket(CHAT_TYPE_INFO, "ETCDropItem yeniden yüklenemedi: %s", szETCDropItemFileName);
					else
						ch->ChatPacket(CHAT_TYPE_INFO, "Yeniden yükleme baþarýlý: ETCDropItem: %s", szETCDropItemFileName);

					ch->ChatPacket(CHAT_TYPE_INFO, "Yeniden yükleniyor: SpecialItemGroup: %s", szSpecialItemGroupFileName);
					if (!ITEM_MANAGER::instance().ReadSpecialDropItemFile(szSpecialItemGroupFileName, true))
						ch->ChatPacket(CHAT_TYPE_INFO, "SpecialItemGroup yeniden yüklenemedi: %s", szSpecialItemGroupFileName);
					else
						ch->ChatPacket(CHAT_TYPE_INFO, "Yeniden yükleme baþarýlý: SpecialItemGroup: %s", szSpecialItemGroupFileName);

					ch->ChatPacket(CHAT_TYPE_INFO, "Yeniden yükleniyor: MOBDropItemFile: %s", szMOBDropItemFileName);
					if (!ITEM_MANAGER::instance().ReadMonsterDropItemGroup(szMOBDropItemFileName, true))
						ch->ChatPacket(CHAT_TYPE_INFO, "MOBDropItemFile dosyasý yeniden yüklenemedi: %s", szMOBDropItemFileName);
					else
						ch->ChatPacket(CHAT_TYPE_INFO, "Yeniden yükleme baþarýlý: MOBDropItemFile: %s", szMOBDropItemFileName);
				}
				else if (strstr(arg1, "group"))
				{
					char szGroupFileName[FILE_NAME_LEN];
					char szGroupGroupFileName[FILE_NAME_LEN];

					snprintf(szGroupFileName, sizeof(szGroupGroupFileName),
						"%s/group.txt", LocaleService_GetBasePath().c_str());
					snprintf(szGroupGroupFileName, sizeof(szGroupGroupFileName),
						"%s/group_group.txt", LocaleService_GetBasePath().c_str());

					ch->ChatPacket(CHAT_TYPE_INFO, "Yeniden yükleniyor: mob gruplarý: %s", szGroupFileName);
					if (!CMobManager::instance().LoadGroup(szGroupFileName, true))
						ch->ChatPacket(CHAT_TYPE_INFO, "Mob gruplarý yeniden yüklenemedi: %s", szGroupFileName);

					ch->ChatPacket(CHAT_TYPE_INFO, "Yeniden yükleniyor: mob grup grubu: %s", szGroupGroupFileName);
					if (!CMobManager::instance().LoadGroupGroup(szGroupGroupFileName, true))
						ch->ChatPacket(CHAT_TYPE_INFO, "Mob grup grubu yeniden yüklenemedi: %s", szGroupGroupFileName);
				}
				else if (strstr(arg1, "regen"))
				{
					SendNoticeMap("Regen.txt yeniden noktalarý tekrar yükleniyor!", ch->GetMapIndex(), false);
					regen_free_map(ch->GetMapIndex());
					CHARACTER_MANAGER::instance().DestroyCharacterInMap(ch->GetMapIndex());
					regen_reload(ch->GetMapIndex());
					SendNoticeMap("Regen.txt yeniden yüklendi!", ch->GetMapIndex(), false);
				}
				break;
		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Kullanýcý  state_user_count yeniden yükleniyor.");
		LoadStateUserCount();

		ch->ChatPacket(CHAT_TYPE_INFO, "Tablolar tekrar yükleniyor.");
		db_clientdesc->DBPacket(HEADER_GD_RELOAD_PROTO, 0, NULL, 0);

#ifdef ENABLE_RELOAD_COMMAND_ALL_CORES
		strlcpy(p2p_packet.argument, "u", sizeof(p2p_packet.argument));
		bSendP2P = true;
#endif

		ch->ChatPacket(CHAT_TYPE_INFO, "Duyuru metni yeniden yükleniyor.");
		DBManager::instance().LoadDBString();
	}
}

ACMD(do_cooltime)
{
	ch->DisableCooltime();
}

ACMD(do_level)
{
	char arg2[256];
	one_argument(argument, arg2, sizeof(arg2));

	if (!*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: level <level>");
		return;
	}

	int	level = 0;
	str_to_number(level, arg2);
	ch->ResetPoint(MINMAX(1, level, gPlayerMaxLevel));

	ch->ClearSkill();
	ch->ClearSubSkill();
}

ACMD(do_gwlist)
{
	ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("This guild is at war."));
	CGuildManager::instance().ShowGuildWarList(ch);
}

ACMD(do_stop_guild_war)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	int id1 = 0, id2 = 0;

	str_to_number(id1, arg1);
	str_to_number(id2, arg2);

	if (!id1 || !id2)
		return;

	if (id1 > id2)
	{
		std::swap(id1, id2);
	}

	ch->ChatPacket(CHAT_TYPE_TALKING, "%d %d", id1, id2);
	CGuildManager::instance().RequestEndWar(id1, id2);
}

ACMD(do_cancel_guild_war)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	int id1 = 0, id2 = 0;
	str_to_number(id1, arg1);
	str_to_number(id2, arg2);

	if (id1 > id2)
		std::swap(id1, id2);

	CGuildManager::instance().RequestCancelWar(id1, id2);
}

ACMD(do_guild_state)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	CGuild* pGuild = CGuildManager::instance().FindGuildByName(arg1);
	if (pGuild != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "GuildID: %d", pGuild->GetID());
		ch->ChatPacket(CHAT_TYPE_INFO, "GuildMasterPID: %d", pGuild->GetMasterPID());
		ch->ChatPacket(CHAT_TYPE_INFO, "IsInWar: %d", pGuild->UnderAnyWar());
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s: This guild does not exist."), arg1);
	}
}

struct FuncWeaken
{
	LPCHARACTER m_pkGM;
	bool	m_bAll;

	FuncWeaken(LPCHARACTER ch) : m_pkGM(ch), m_bAll(false)
	{
	}

	void operator () (LPENTITY ent)
	{
		if (!ent->IsType(ENTITY_CHARACTER))
			return;

		LPCHARACTER pkChr = (LPCHARACTER) ent;

		int iDist = DISTANCE_APPROX(pkChr->GetX() - m_pkGM->GetX(), pkChr->GetY() - m_pkGM->GetY());

		if (!m_bAll && iDist >= 1000)
			return;

		if (pkChr->IsNPC())
			pkChr->PointChange(POINT_HP, (10 - pkChr->GetHP()));
	}
};

ACMD(do_weaken)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	FuncWeaken func(ch);

	if (*arg1 && !strcmp(arg1, "all"))
		func.m_bAll = true;

	ch->GetSectree()->ForEachAround(func);
}

ACMD(do_getqf)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	LPCHARACTER tch;

	if (!*arg1)
		tch = ch;
	else
	{
		tch = CHARACTER_MANAGER::instance().FindPC(arg1);

		if (!tch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Böyle bir karakter yok");
			return;
		}
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(tch->GetPlayerID());

	if (pPC)
		pPC->SendFlagList(ch);
}

ACMD(do_set_state)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
#ifdef ENABLE_CH_CRASH_CORE_FIX
	if (!pPC)
	{
		return;
	}
#endif
	std::string questname = arg1;
	std::string statename = arg2;

	if (!questname.empty())
	{
		int value = quest::CQuestManager::Instance().GetQuestStateIndex(questname, statename);

		pPC->SetFlag(questname + ".__status", value);
		pPC->ClearTimer();

		quest::PC::QuestInfoIterator it = pPC->quest_begin();
		unsigned int questindex = quest::CQuestManager::instance().GetQuestIndexByName(questname);

		while (it!= pPC->quest_end())
		{
			if (it->first == questindex)
			{
				it->second.st = value;
				break;
			}

			++it;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, "Görev durumu bayraðý ayarlandý: %s %s %d", questname.c_str(), arg1, value);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Görev durumu bayraðý ayarlanamadý");
	}
}

ACMD(do_setqf)
{
	char arg1[256];
	char arg2[256];
	char arg3[256];

	one_argument(two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: setqf <flagname> <value> [<character name>]");
		return;
	}

	LPCHARACTER tch = ch;

	if (*arg3)
		tch = CHARACTER_MANAGER::instance().FindPC(arg3);

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Böyle bir karakter yok");
		return;
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(tch->GetPlayerID());

	if (pPC)
	{
		int value = 0;
		str_to_number(value, arg2);
		pPC->SetFlag(arg1, value);
		ch->ChatPacket(CHAT_TYPE_INFO, "Görev bayraðý ayarlandý: %s %d", arg1, value);
	}
}

ACMD(do_delqf)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: delqf <flagname> [<character name>]");
		return;
	}

	LPCHARACTER tch = ch;

	if (*arg2)
		tch = CHARACTER_MANAGER::instance().FindPC(arg2);

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Böyle bir karakter yok");
		return;
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(tch->GetPlayerID());

	if (pPC)
	{
		if (pPC->DeleteFlag(arg1))
			ch->ChatPacket(CHAT_TYPE_INFO, "Öðe baþarýyla silindi");
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "Silme baþarýsýz. Görev bayraðý mevcut deðil.");
	}
}

ACMD(do_forgetme)
{
	ch->ForgetMyAttacker();
}

ACMD(do_aggregate)
{
	ch->AggregateMonster();
	//ch->AttractRanger();
}

ACMD(do_attract_ranger)
{
	ch->AttractRanger();
}

ACMD(do_pull_monster)
{
	ch->PullMonster();
}

ACMD(do_polymorph)
{
	char arg1[256], arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	if (*arg1)
	{
		DWORD dwVnum = 0;
		str_to_number(dwVnum, arg1);
		bool bMaintainStat = false;
		if (*arg2)
		{
			int value = 0;
			str_to_number(value, arg2);
			bMaintainStat = (value>0);
		}

		ch->SetPolymorph(dwVnum, bMaintainStat);
	}
}

ACMD(do_polymorph_item)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD dwVnum = 0;
		str_to_number(dwVnum, arg1);

		LPITEM item = ITEM_MANAGER::instance().CreateItem(70104, 1, 0, true);
		if (item)
		{
			item->SetSocket(0, dwVnum);
			int iEmptyPos = ch->GetEmptyInventory(item->GetSize());

			if (iEmptyPos != -1)
			{
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				LogManager::instance().ItemLog(ch, item, "GM", item->GetName());
			}
			else
			{
				M2_DESTROY_ITEM(item);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough inventory space."));
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "#%d numaralý eþya bulunamadý.", 70103);
		}
	}
}

ACMD(do_priv_empire)
{
	char arg1[256] = {0};
	char arg2[256] = {0};
	char arg3[256] = {0};
	char arg4[256] = {0};
	int empire = 0;
	int type = 0;
	int value = 0;
	int duration = 0;

	const char* line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		goto USAGE;

	if (!line)
		goto USAGE;

	two_arguments(line, arg3, sizeof(arg3), arg4, sizeof(arg4));

	if (!*arg3 || !*arg4)
		goto USAGE;

	str_to_number(empire, arg1);
	str_to_number(type,	arg2);
	str_to_number(value,	arg3);
	value = MINMAX(0, value, 1000);
	str_to_number(duration, arg4);

	if (empire < 0 || 3 < empire)
		goto USAGE;

	if (type < 1 || 4 < type)
		goto USAGE;

	if (value < 0)
		goto USAGE;

	if (duration < 0)
		goto USAGE;

	duration = duration * (60*60);

	sys_log(0, "_give_empire_privileage(empire=%d, type=%d, value=%d, duration=%d) by command", 
			empire, type, value, duration);
	CPrivManager::instance().RequestGiveEmpirePriv(empire, type, value, duration);
	return;

USAGE:
	ch->ChatPacket(CHAT_TYPE_INFO, "Kullaným: priv_empire <imparatorluk> <tip> <deðer> <süre>");
	ch->ChatPacket(CHAT_TYPE_INFO, "  <empire>    0 - 3 (0==all)");
	ch->ChatPacket(CHAT_TYPE_INFO, "  <tip>      1:item_drop, 2:gold_drop, 3:gold10_drop, 4:exp");
	ch->ChatPacket(CHAT_TYPE_INFO, "  <deðer>     yüzde");
	ch->ChatPacket(CHAT_TYPE_INFO, "  <süre>  saat");
}

ACMD(do_priv_guild)
{
	static const char msg[] = { '\0' };

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		CGuild * g = CGuildManager::instance().FindGuildByName(arg1);

		if (!g)
		{
			DWORD guild_id = 0;
			str_to_number(guild_id, arg1);
			g = CGuildManager::instance().FindGuild(guild_id);
		}

		if (!g)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A guild with this name or number does not exist."));
		else
		{
			char buf[1024+1];
			snprintf(buf, sizeof(buf), msg, g->GetID());

			using namespace quest;
			PC * pc = CQuestManager::instance().GetPC(ch->GetPlayerID());
			QuestState qs = CQuestManager::instance().OpenState("ADMIN_QUEST", QUEST_FISH_REFINE_STATE_INDEX);
			luaL_loadbuffer(qs.co, buf, strlen(buf), "ADMIN_QUEST");
			pc->SetQuest("ADMIN_QUEST", qs);

			QuestState & rqs = *pc->GetRunningQuestState();

			if (!CQuestManager::instance().RunState(rqs))
			{
				CQuestManager::instance().CloseState(rqs);
				pc->EndRunning();
				return;
			}
		}
	}
}

ACMD(do_mount_test)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		ch->MountVnum(vnum);
	}
}

ACMD(do_observer)
{
	ch->SetObserverMode(!ch->IsObserverMode());
}

ACMD(do_socket_item)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1)
	{
		DWORD dwVnum = 0;
		str_to_number(dwVnum, arg1);
	
		int iSocketCount = 0;
		str_to_number(iSocketCount, arg2);
	
		if (!iSocketCount || iSocketCount >= ITEM_SOCKET_MAX_NUM)
			iSocketCount = 3;
	
		if (!dwVnum)
		{
			if (!ITEM_MANAGER::instance().GetVnum(arg1, dwVnum))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "#%d eþyasý bu VNUM ile mevcut deðil.", dwVnum);
				return;
			}
		}

		LPITEM item = ch->AutoGiveItem(dwVnum);
	
		if (item)
		{
			for (int i = 0; i < iSocketCount; ++i)
				item->SetSocket(i, 1);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "#%d öðe oluþturulamýyor.", dwVnum);
		}
	}
}

ACMD(do_xmas)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int flag = 0;

	if (*arg1)
		str_to_number(flag, arg1);

	switch (subcmd)
	{
		case SCMD_XMAS_SNOW:
			quest::CQuestManager::instance().RequestSetEventFlag("xmas_snow", flag);
			break;

		case SCMD_XMAS_BOOM:
			quest::CQuestManager::instance().RequestSetEventFlag("xmas_boom", flag);
			break;

		case SCMD_XMAS_SANTA:
			quest::CQuestManager::instance().RequestSetEventFlag("xmas_santa", flag);
			break;

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
		case SCMD_XMAS_SOUL:
			quest::CQuestManager::instance().RequestSetEventFlag("xmas_soul", flag);
			break;
#endif
	}
}

ACMD(do_block_chat_list)
{
	if (!ch || (ch->GetGMLevel() < GM_HIGH_WIZARD && ch->GetQuestFlag("chat_privilege.block") <= 0))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This command does not exist."));
		return;
	}

	DBManager::instance().ReturnQuery(QID_BLOCK_CHAT_LIST, ch->GetPlayerID(), NULL, 
			"SELECT p.name, a.lDuration FROM affect%s as a, player%s as p WHERE a.bType = %d AND a.dwPID = p.id",
			get_table_postfix(), get_table_postfix(), AFFECT_BLOCK_CHAT);
}

ACMD(do_vote_block_chat)
{
	return;

	char arg1[256];
	argument = one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: vote_block_chat <name>");
		return;
	}

	const char* name = arg1;
	long lBlockDuration = 10;
	sys_log(0, "vote_block_chat %s %d", name, lBlockDuration);

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(name);

		if (pkCCI)
		{
			TPacketGGBlockChat p;

			p.bHeader = HEADER_GG_BLOCK_CHAT;
			strlcpy(p.szName, name, sizeof(p.szName));
			p.lBlockDuration = lBlockDuration;
			P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGBlockChat));
		}
		else
		{
			TPacketBlockChat p;

			strlcpy(p.szName, name, sizeof(p.szName));
			p.lDuration = lBlockDuration;
			db_clientdesc->DBPacket(HEADER_GD_BLOCK_CHAT, ch ? ch->GetDesc()->GetHandle() : 0, &p, sizeof(p));

		}

		if (ch)
			ch->ChatPacket(CHAT_TYPE_INFO, "Sohbet engelleme talep edildi");

		return;
	}

	if (tch && ch != tch)
		tch->AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, lBlockDuration, 0, true);
}

ACMD(do_block_chat)
{
	if (ch && (ch->GetGMLevel() < GM_HIGH_WIZARD && ch->GetQuestFlag("chat_privilege.block") <= 0))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This command does not exist."));
		return;
	}

	char arg1[256];
	argument = one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		if (ch)
			ch->ChatPacket(CHAT_TYPE_INFO, "Usage: block_chat <name> <time> (0 to off)");

		return;
	}

	const char* name = arg1;
	long lBlockDuration = parse_time_str(argument);

	if (lBlockDuration < 0)
		return;

	sys_log(0, "BLOCK CHAT %s %d", name, lBlockDuration);

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(name);

		if (pkCCI)
		{
			TPacketGGBlockChat p;

			p.bHeader = HEADER_GG_BLOCK_CHAT;
			strlcpy(p.szName, name, sizeof(p.szName));
			p.lBlockDuration = lBlockDuration;
			P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGBlockChat));
		}
		else
		{
			TPacketBlockChat p;

			strlcpy(p.szName, name, sizeof(p.szName));
			p.lDuration = lBlockDuration;
			db_clientdesc->DBPacket(HEADER_GD_BLOCK_CHAT, ch ? ch->GetDesc()->GetHandle() : 0, &p, sizeof(p));
		}

		if (ch)
			ch->ChatPacket(CHAT_TYPE_INFO, "Sohbet engelleme talep edildi");

		return;
	}

	if (tch && ch != tch)
		tch->AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, lBlockDuration, 0, true);
}

ACMD(do_build)
{
	using namespace building;

	char arg1[256], arg2[256], arg3[256], arg4[256];
	const char* line = one_argument(argument, arg1, sizeof(arg1));
	BYTE GMLevel = ch->GetGMLevel();

	CLand* pkLand = CManager::instance().FindLand(ch->GetMapIndex(), ch->GetX(), ch->GetY());

	if (!pkLand)
	{
		sys_err("%s trying to build on not buildable area.", ch->GetName());
		return;
	}

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Invalid syntax: no command");
		return;
	}

	if (GMLevel == GM_PLAYER)
	{
		if ((!ch->GetGuild() || ch->GetGuild()->GetID() != pkLand->GetOwner()))
		{
			sys_err("%s trying to build on not owned land.", ch->GetName());
			return;
		}

		if (ch->GetGuild()->GetMasterPID() != ch->GetPlayerID())
		{
			sys_err("%s trying to build while not the guild master.", ch->GetName());
			return;
		}
	}

	switch (LOWER(*arg1))
	{
		case 'c':
			{
				char arg5[256], arg6[256];
				line = one_argument(two_arguments(line, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3));
				one_argument(two_arguments(line, arg4, sizeof(arg4), arg5, sizeof(arg5)), arg6, sizeof(arg6));

				if (!*arg1 || !*arg2 || !*arg3 || !*arg4 || !*arg5 || !*arg6)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Invalid syntax");
					return;
				}

				DWORD dwVnum = 0;
				str_to_number(dwVnum,  arg1);

				using namespace building;

				const TObjectProto * t = CManager::instance().GetObjectProto(dwVnum);
				if (!t)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The building does not exist."));
					return;
				}

				const DWORD BUILDING_MAX_PRICE = 100000000;

				if (t->dwGroupVnum)
				{
					if (pkLand->FindObjectByGroup(t->dwGroupVnum))
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This type of building can only be erected once."));
						return;
					}
				}

				if (t->dwDependOnGroupVnum)
				{
					{
						if (!pkLand->FindObjectByGroup(t->dwDependOnGroupVnum))
						{
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Main Building has to be erected first."));
							return;
						}
					}
				}

				if (test_server || GMLevel == GM_PLAYER)
				{
					if (t->dwPrice > BUILDING_MAX_PRICE)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Building failed because of incorrect pricing."));
						return;
					}

					if (ch->GetGold() < (int)t->dwPrice)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your guild does not have enough Yang to erect this building."));
						return;
					}

					int i;
					for (i = 0; i < OBJECT_MATERIAL_MAX_NUM; ++i)
					{
						DWORD dwItemVnum = t->kMaterials[i].dwItemVnum;
						DWORD dwItemCount = t->kMaterials[i].dwCount;

						if (dwItemVnum == 0)
							break;

						if ((int) dwItemCount > ch->CountSpecifyItem(dwItemVnum))
						{
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough resources to build a building."));
							return;
						}
					}
				}

				float x_rot = atof(arg4);
				float y_rot = atof(arg5);
				float z_rot = atof(arg6);

				long map_x = 0;
				str_to_number(map_x, arg2);
				long map_y = 0;
				str_to_number(map_y, arg3);

				bool isSuccess = pkLand->RequestCreateObject(dwVnum, 
						ch->GetMapIndex(),
						map_x,
						map_y,
						x_rot,
						y_rot,
						z_rot, true);

				if (!isSuccess)
				{
					if (test_server)
						ch->ChatPacket(CHAT_TYPE_INFO, "Bu alana bina inþa edemezsiniz");
					return;
				}

				if (test_server || GMLevel == GM_PLAYER)
				{
					ch->PointChange(POINT_GOLD, -t->dwPrice);

					{
						int i;
						for (i = 0; i < OBJECT_MATERIAL_MAX_NUM; ++i)
						{
							DWORD dwItemVnum = t->kMaterials[i].dwItemVnum;
							DWORD dwItemCount = t->kMaterials[i].dwCount;

							if (dwItemVnum == 0)
								break;

							sys_log(0, "BUILD: material %d %u %u", i, dwItemVnum, dwItemCount);
							ch->RemoveSpecifyItem(dwItemVnum, dwItemCount);
						}
					}
				}
			}
			break;

		case 'd' :
			{
				one_argument(line, arg1, sizeof(arg1));

				if (!*arg1)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Invalid syntax");
					return;
				}

				DWORD vid = 0;
				str_to_number(vid, arg1);
				pkLand->RequestDeleteObjectByVID(vid);
			}
			break;

		case 'w' :
			if (GMLevel > GM_PLAYER) 
			{
				int mapIndex = ch->GetMapIndex();

				one_argument(line, arg1, sizeof(arg1));
				
				sys_log(0, "guild.wall.build map[%d] direction[%s]", mapIndex, arg1);

				switch (arg1[0])
				{
					case 's':
						pkLand->RequestCreateWall(mapIndex,   0.0f);
						break;
					case 'n':
						pkLand->RequestCreateWall(mapIndex, 180.0f);
						break;
					case 'e':
						pkLand->RequestCreateWall(mapIndex,  90.0f);
						break;
					case 'w':
						pkLand->RequestCreateWall(mapIndex, 270.0f);
						break;
					default:
						ch->ChatPacket(CHAT_TYPE_INFO, "guild.wall.build unknown_direction[%s]", arg1);
						sys_err("guild.wall.build unknown_direction[%s]", arg1);
						break;
				}

			}
			break;

		case 'e':
			if (GMLevel > GM_PLAYER)
			{
				pkLand->RequestDeleteWall();
			}
			break;

		case 'W' :
			if (GMLevel >  GM_PLAYER) 
			{
				int setID = 0, wallSize = 0;
				char arg5[256], arg6[256];
				line = two_arguments(line, arg1, sizeof(arg1), arg2, sizeof(arg2));
				line = two_arguments(line, arg3, sizeof(arg3), arg4, sizeof(arg4));
				two_arguments(line, arg5, sizeof(arg5), arg6, sizeof(arg6));

				str_to_number(setID, arg1);
				str_to_number(wallSize, arg2);

				if (setID != 14105 && setID != 14115 && setID != 14125)
				{
					sys_log(0, "BUILD_WALL: wrong wall set id %d", setID);
					break;
				}
				else 
				{
					bool door_east = false;
					str_to_number(door_east, arg3);
					bool door_west = false;
					str_to_number(door_west, arg4);
					bool door_south = false;
					str_to_number(door_south, arg5);
					bool door_north = false;
					str_to_number(door_north, arg6);
					pkLand->RequestCreateWallBlocks(setID, ch->GetMapIndex(), wallSize, door_east, door_west, door_south, door_north);
				}
			}
			break;

		case 'E' :
			if (GMLevel > GM_PLAYER)
			{
				one_argument(line, arg1, sizeof(arg1));
				DWORD id = 0;
				str_to_number(id, arg1);
				pkLand->RequestDeleteWallBlocks(id);
			}
			break;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, "Geçersiz komut: %s", arg1);
			break;
	}
}

ACMD(do_clear_quest)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
#ifdef ENABLE_CH_CRASH_CORE_FIX
	if (!pPC)
	{
		return;
	}
#endif
	pPC->ClearQuest(arg1);
}

ACMD(do_horse_state)
{
	ch->ChatPacket(CHAT_TYPE_INFO, "At Bilgileri:");
	ch->ChatPacket(CHAT_TYPE_INFO, "    Seviye  %d", ch->GetHorseLevel());
	ch->ChatPacket(CHAT_TYPE_INFO, "    Can %d/%d (%d%%)", ch->GetHorseHealth(), ch->GetHorseMaxHealth(), ch->GetHorseHealth() * 100 / ch->GetHorseMaxHealth());
	ch->ChatPacket(CHAT_TYPE_INFO, "    Dayanýklýlýk   %d/%d (%d%%)", ch->GetHorseStamina(), ch->GetHorseMaxStamina(), ch->GetHorseStamina() * 100 / ch->GetHorseMaxStamina());
}

ACMD(do_horse_level)
{
	char arg1[256] = {0};
	char arg2[256] = {0};
	LPCHARACTER victim;
	int	level = 0;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "usage : /horse_level <name> <level>");
		return;
	}

	victim = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (NULL == victim)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This character does not exist."));
		return;
	}

	str_to_number(level, arg2);
	level = MINMAX(0, level, HORSE_MAX_LEVEL);

	ch->ChatPacket(CHAT_TYPE_INFO, "At seviyesi ayarlandý (%s: %d)", victim->GetName(), level);

	victim->SetHorseLevel(level);
	victim->ComputePoints();
	victim->SkillLevelPacket();
	return;
}

ACMD(do_horse_ride)
{
	if (ch->IsHorseRiding())
		ch->StopRiding();
	else
		ch->StartRiding();
}

ACMD(do_horse_summon)
{
#ifdef ENABLE_MOUNT_SYSTEM
	if (ch->IsRidingMount())
		return;
#endif

	ch->HorseSummon(true, true);
}

ACMD(do_horse_unsummon)
{
	ch->HorseSummon(false, true);
}

ACMD(do_horse_set_stat)
{
	char arg1[256], arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2)
	{
		int hp = 0;
		str_to_number(hp, arg1);
		int stam = 0;
		str_to_number(stam, arg2);
		ch->UpdateHorseHealth(hp - ch->GetHorseHealth());
		ch->UpdateHorseStamina(stam - ch->GetHorseStamina());
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage : /horse_set_stat <hp> <stamina>");
	}
}

ACMD(do_save_attribute_to_image) // command "/saveati" for alias
{
	char szFileName[256];
	char szMapIndex[256];

	two_arguments(argument, szMapIndex, sizeof(szMapIndex), szFileName, sizeof(szFileName));

	if (!*szMapIndex || !*szFileName)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: /saveati <map_index> <filename>");
		return;
	}

	long lMapIndex = 0;
	str_to_number(lMapIndex, szMapIndex);

	if (SECTREE_MANAGER::instance().SaveAttributeToImage(lMapIndex, szFileName))
		ch->ChatPacket(CHAT_TYPE_INFO, "Kaydedildi");
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "Kaydetme baþarýsýz");
}

ACMD(do_affect_remove)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: /affect_remove <player name>");
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: /affect_remove <type> <point>");

		LPCHARACTER tch = ch;

		if (*arg1)
			if (!(tch = CHARACTER_MANAGER::instance().FindPC(arg1)))
				tch = ch;

		ch->ChatPacket(CHAT_TYPE_INFO, "-- Affect List of %s -------------------------------", tch->GetName());
		ch->ChatPacket(CHAT_TYPE_INFO, "Tür / Puan / Modifiye Süresi / Bayrak");

		const std::list<CAffect *> & cont = tch->GetAffectContainer();

		itertype(cont) it = cont.begin();

		while (it != cont.end())
		{
			CAffect * pkAff = *it++;

			ch->ChatPacket(CHAT_TYPE_INFO, "%4d %5d %5d %8d %u",
					pkAff->dwType, pkAff->bApplyOn, pkAff->lApplyValue, pkAff->lDuration, pkAff->dwFlag);
		}
		return;
	}

	bool removed = false;

	CAffect * af;

	DWORD	type = 0;
	str_to_number(type, arg1);
	BYTE	point = 0;
	str_to_number(point, arg2);
	while ((af = ch->FindAffect(type, point)))
	{
		ch->RemoveAffect(af);
		removed = true;
	}

	if (removed)
		ch->ChatPacket(CHAT_TYPE_INFO, "Etkisi baþarýyla kaldýrýldý");
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu tür ve puan tarafýndan etkilenmez.");
}

ACMD(do_change_attr)
{
	LPITEM weapon = ch->GetWear(WEAR_WEAPON);
	if (weapon)
		weapon->ChangeAttribute();
}

ACMD(do_add_attr)
{
	LPITEM weapon = ch->GetWear(WEAR_WEAPON);
	if (weapon)
		weapon->AddAttribute();
}

ACMD(do_add_socket)
{
	LPITEM weapon = ch->GetWear(WEAR_WEAPON);
	if (weapon)
		weapon->AddSocket();
}

ACMD(do_show_arena_list)
{
	CArenaManager::instance().SendArenaMapListTo(ch);
}

ACMD(do_end_all_duel)
{
	CArenaManager::instance().EndAllDuel();
}

ACMD(do_end_duel)
{
	char szName[256];

	one_argument(argument, szName, sizeof(szName));

	LPCHARACTER pChar = CHARACTER_MANAGER::instance().FindPC(szName);
	if (pChar == NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This character does not exist."));
		return;
	}

	if (CArenaManager::instance().EndDuel(pChar->GetPlayerID()) == false)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Duel has not been successfully cancelled."));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Duel cancelled successfully."));
	}
}

ACMD(do_duel)
{
	char szName1[256];
	char szName2[256];
	char szSet[256];
	char szMinute[256];
	int set = 0;
	int minute = 0;

	argument = two_arguments(argument, szName1, sizeof(szName1), szName2, sizeof(szName2));
	two_arguments(argument, szSet, sizeof(szSet), szMinute, sizeof(szMinute));

	str_to_number(set, szSet);

	if (set < 0) set = 1;
	if (set > 5) set = 5;

	if (!str_to_number(minute, szMinute))
	{
		minute = 5;
	}

	if (minute < 5)
		minute = 5;

	LPCHARACTER pChar1 = CHARACTER_MANAGER::instance().FindPC(szName1);
	LPCHARACTER pChar2 = CHARACTER_MANAGER::instance().FindPC(szName2);

	if (pChar1 != NULL && pChar2 != NULL)
	{
		pChar1->RemoveGoodAffect();
		pChar2->RemoveGoodAffect();

		pChar1->RemoveBadAffect();
		pChar2->RemoveBadAffect();

		LPPARTY pParty = pChar1->GetParty();
		if (pParty != NULL)
		{
			if (pParty->GetMemberCount() == 2)
			{
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
				pChar1->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You have left the group."));
				pParty->Quit(pChar1->GetPlayerID());
			}
		}

		pParty = pChar2->GetParty();
		if (pParty != NULL)
		{
			if (pParty->GetMemberCount() == 2)
			{
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
				pChar2->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You have left the group."));
				pParty->Quit(pChar2->GetPlayerID());
			}
		}

		if (CArenaManager::instance().StartDuel(pChar1, pChar2, set, minute) == true)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The duel has been successfully started."));
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is a problem with initiating the duel."));
		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There are no combatants."));
	}
}

ACMD(do_stat_plus_amount)
{
	char szPoint[256];

	one_argument(argument, szPoint, sizeof(szPoint));

	if (*szPoint == '\0')
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your status while you are transformed."));
		return;
	}

	int nRemainPoint = ch->GetPoint(POINT_STAT);

	if (nRemainPoint <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No status points left."));
		return;
	}

	int nPoint = 0;
	str_to_number(nPoint, szPoint);

	if (nRemainPoint < nPoint)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining status points are too low."));
		return;
	}

	if (nPoint < 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You entered an incorrect value."));
		return;
	}

	switch (subcmd)
	{
		case POINT_HT :
			if (nPoint + ch->GetPoint(POINT_HT) > 90)
			{
				nPoint = 90 - ch->GetPoint(POINT_HT);
			}
			break;

		case POINT_IQ :
			if (nPoint + ch->GetPoint(POINT_IQ) > 90)
			{
				nPoint = 90 - ch->GetPoint(POINT_IQ);
			}
			break;

		case POINT_ST :
			if (nPoint + ch->GetPoint(POINT_ST) > 90)
			{
				nPoint = 90 - ch->GetPoint(POINT_ST);
			}
			break;

		case POINT_DX :
			if (nPoint + ch->GetPoint(POINT_DX) > 90)
			{
				nPoint = 90 - ch->GetPoint(POINT_DX);
			}
			break;

		default :
			ch->ChatPacket(CHAT_TYPE_INFO, "Alt sipariþ veya ana sipariþ hatalý.");
			return;
			break;
	}

	if (nPoint != 0)
	{
		ch->SetRealPoint(subcmd, ch->GetRealPoint(subcmd) + nPoint);
		ch->SetPoint(subcmd, ch->GetPoint(subcmd) + nPoint);
		ch->ComputePoints();
		ch->PointChange(subcmd, 0);

		ch->PointChange(POINT_STAT, -nPoint);
		ch->ComputePoints();
	}
}

struct tTwoPID
{
	int pid1;
	int pid2;
};

ACMD(do_break_marriage)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	tTwoPID pids = { 0, 0 };

	str_to_number(pids.pid1, arg1);
	str_to_number(pids.pid2, arg2);

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Broken contract between player %d and player %d."), pids.pid1, pids.pid2);
	db_clientdesc->DBPacket(HEADER_GD_BREAK_MARRIAGE, 0, &pids, sizeof(pids));
}

ACMD(do_effect)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	int	effect_type = 0;
	str_to_number(effect_type, arg1);
	ch->EffectPacket(effect_type);
}

struct FCountInMap
{
	int m_Count[4];
	FCountInMap() { memset(m_Count, 0, sizeof(int) * 4); }
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch && ch->IsPC())
				++m_Count[ch->GetEmpire()];
		}
	}
	int GetCount(BYTE bEmpire) { return m_Count[bEmpire]; }
};

ACMD(do_reset_subskill)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: reset_subskill <name>");
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (tch == NULL)
		return;

	tch->ClearSubSkill();
	ch->ChatPacket(CHAT_TYPE_INFO, "[%s] karakterinin alt becerisi sýfýrlandý", tch->GetName());
}

ACMD(do_flush)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

#ifdef FLUSH_AT_SHUTDOWN
	DWORD pid = 0;
	if(0 == arg1[0])
	{
		pid = ch->GetPlayerID();
	}
	else
	{
		pid = (DWORD)strtoul(arg1, NULL, 10);
	}
	if (pid != 0)
	{
		ch->SaveReal();
		db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
		db_clientdesc->Packet(&pid, sizeof(DWORD));
	}
#else
	if (0 == arg1[0])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "usage : /flush player_id");
		return;
	}

	DWORD pid = (DWORD) strtoul(arg1, NULL, 10);

	db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
	db_clientdesc->Packet(&pid, sizeof(DWORD));
#endif
}

ACMD(do_eclipse)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (strtol(arg1, NULL, 10) == 1)
	{
		quest::CQuestManager::instance().RequestSetEventFlag("eclipse", 1);
	}
	else
	{
		quest::CQuestManager::instance().RequestSetEventFlag("eclipse", 0);
	}
}

ACMD(do_weeklyevent)
{
	char arg1[256];
	int empire = 0;

	if (CBattleArena::instance().IsRunning() == false)
	{
		one_argument(argument, arg1, sizeof(arg1));

		empire = strtol(arg1, NULL, 10);

		if (empire == 1 || empire == 2 || empire == 3)
		{
			CBattleArena::instance().Start(empire);
		}
		else
		{
			CBattleArena::instance().Start(rand()%3 + 1);
		}
		ch->ChatPacket(CHAT_TYPE_INFO, "Haftalýk Etkinlik Baþladý");
	}
	else
	{
		CBattleArena::instance().ForceEnd();
		ch->ChatPacket(CHAT_TYPE_INFO, "Haftalýk Etkinlik Sona Erdi");
	}
}

ACMD(do_event_helper)
{
	char arg1[256];
	int mode = 0;

	one_argument(argument, arg1, sizeof(arg1));
	str_to_number(mode, arg1);

	if (mode == 1)
	{
		xmas::SpawnEventHelper(true);
		ch->ChatPacket(CHAT_TYPE_INFO, "Etkinlik Yardýmcýsý Oluþturuldu");
	}
	else
	{
		xmas::SpawnEventHelper(false);
		ch->ChatPacket(CHAT_TYPE_INFO, "Etkinlik Yardýmcýsý Silindi");
	}
}

struct FMobCounter
{
	int nCount;

	void operator () (LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);

			if (pChar->IsMonster() == true || pChar->IsStone())
			{
				nCount++;
			}
		}
	}
};

ACMD(do_get_mob_count)
{
	LPSECTREE_MAP pSectree = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());

	if (pSectree == NULL)
		return;

	FMobCounter f;
	f.nCount = 0;

	pSectree->for_each(f);

	ch->ChatPacket(CHAT_TYPE_INFO, "Harita ID: %d Canavar Sayýsý: %d", ch->GetMapIndex(), f.nCount);
}

ACMD(do_clear_land)
{
	const building::CLand* pLand = building::CManager::instance().FindLand(ch->GetMapIndex(), ch->GetX(), ch->GetY());

	if( NULL == pLand )
	{
		return;
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "Lonca arazisi(%d) temizlendi", pLand->GetID());

	building::CManager::instance().ClearLand(pLand->GetID());
}

ACMD(do_special_item)
{
	ITEM_MANAGER::instance().ConvSpecialDropItemFile();
}

ACMD(do_set_stat)
{
	char szName [256];
	char szChangeAmount[256];

	two_arguments (argument, szName, sizeof (szName), szChangeAmount, sizeof(szChangeAmount));

	if (!*szName || *szChangeAmount == '\0')
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Hatalý parametre");
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(szName);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(szName);

		if (pkCCI)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Oyuncu (%s) bulunamadý. %s oyun sunucunda deðil.", szName, szName);
			return;
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Oyuncu (%s) bulunamadý. Belki %s giriþ yapmamýþ ya da mevcut deðil.", szName, szName);
			return;
		}
	}
	else
	{
		if (tch->IsPolymorphed())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your status while you are transformed."));
			return;
		}

		if (subcmd != POINT_HT && subcmd != POINT_IQ && subcmd != POINT_ST && subcmd != POINT_DX)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Alt sipariþ veya ana sipariþ hatalý.");
			return;
		}
		int nRemainPoint = tch->GetPoint(POINT_STAT);
		int nCurPoint = tch->GetRealPoint(subcmd);
		int nChangeAmount = 0;
		str_to_number(nChangeAmount, szChangeAmount);
		int nPoint = nCurPoint + nChangeAmount;
		
		int n;
		switch (subcmd)
		{
		case POINT_HT:
			if (nPoint < JobInitialPoints[tch->GetJob()].ht)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Stat deðeri baþlangýç deðerinin altýna ayarlanamaz.");
				return;
			}
			n = 0;
			break;
		case POINT_IQ:
			if (nPoint < JobInitialPoints[tch->GetJob()].iq)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Stat deðeri baþlangýç deðerinin altýna ayarlanamaz.");
				return;
			}
			n = 1;
			break;
		case POINT_ST:
			if (nPoint < JobInitialPoints[tch->GetJob()].st)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Stat deðeri baþlangýç deðerinin altýna ayarlanamaz.");
				return;
			}
			n = 2;
			break;
		case POINT_DX:
			if (nPoint < JobInitialPoints[tch->GetJob()].dx)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Stat deðeri baþlangýç deðerinin altýna ayarlanamaz.");
				return;
			}
			n = 3;
			break;
		}

		if (nPoint > 90)
		{
			nChangeAmount -= nPoint - 90;
			nPoint = 90;
		}

		if (nRemainPoint < nChangeAmount)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining status points are too low."));
			return;
		}

		tch->SetRealPoint(subcmd, nPoint);
		tch->SetPoint(subcmd, tch->GetPoint(subcmd) + nChangeAmount);
		tch->ComputePoints();
		tch->PointChange(subcmd, 0);

		tch->PointChange(POINT_STAT, -nChangeAmount);
		tch->ComputePoints();

		const char* stat_name[4] = {"con", "int", "str", "dex"};

		ch->ChatPacket(CHAT_TYPE_INFO, "%s's %s change %d to %d", szName, stat_name[n], nCurPoint, nPoint);
	}
}

ACMD(do_get_item_id_list)
{
	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; i++)
	{
		LPITEM item = ch->GetInventoryItem(i);
		if (item != NULL)
			ch->ChatPacket(CHAT_TYPE_INFO, "cell : %d, name : %s, id : %d", item->GetCell(), item->GetLocaleName(), item->GetID());
	}
}

ACMD(do_set_socket)
{
	char arg1 [256];
	char arg2 [256];
	char arg3 [256];

	one_argument (two_arguments (argument, arg1, sizeof (arg1), arg2, sizeof(arg2)), arg3, sizeof (arg3));

	int item_id, socket_num, value;
	if (!str_to_number (item_id, arg1) || !str_to_number (socket_num, arg2) || !str_to_number (value, arg3))
		return;

	LPITEM item = ITEM_MANAGER::instance().Find (item_id);
	if (item)
		item->SetSocket (socket_num, value);
}

ACMD (do_can_dead)
{
	if (subcmd)
		ch->SetArmada();
	else
		ch->ResetArmada();
}

ACMD (do_full_set)
{
	extern void do_all_skill_master(LPCHARACTER ch, const char *argument, int cmd, int subcmd);
	do_all_skill_master(ch, NULL, 0, 0);
	extern void do_item_full_set(LPCHARACTER ch, const char *argument, int cmd, int subcmd);
	do_item_full_set(ch, NULL, 0, 0);
	extern void do_attr_full_set(LPCHARACTER ch, const char *argument, int cmd, int subcmd);
	do_attr_full_set(ch, NULL, 0, 0);
}

ACMD (do_all_skill_master)
{
	ch->SetHorseLevel(SKILL_MAX_LEVEL);
	for (int i = 0; i < SKILL_MAX_NUM; i++)
	{
		if (true == ch->CanUseSkill(i))
		{
			ch->SetSkillLevel(i, SKILL_MAX_LEVEL);
		}
		else
		{
			switch(i)
			{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:
				ch->SetSkillLevel(i, SKILL_MAX_LEVEL);
				break;
			}
		}
	}
	ch->ComputePoints();
	ch->SkillLevelPacket();
}

ACMD (do_item_full_set)
{
	BYTE job = ch->GetJob();
	LPITEM item;
	for (int i = 0; i < 6; i++)
	{
		item = ch->GetWear(i);
		if (item != NULL)
			ch->UnequipItem(item);
	}
	item = ch->GetWear(WEAR_SHIELD);
	if (item != NULL)
		ch->UnequipItem(item);

	switch (job)
	{
		case JOB_SURA:
			{
				item = ITEM_MANAGER::instance().CreateItem(11699);
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(13049);
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(15189 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(189 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(12529 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(14109 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(17209 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(16209 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
			}
			break;
		case JOB_WARRIOR:
			{
				item = ITEM_MANAGER::instance().CreateItem(11299);
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(13049);
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(15189 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(3159 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(12249 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(14109 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(17109 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(16109 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
			}
			break;
		case JOB_SHAMAN:
			{
				item = ITEM_MANAGER::instance().CreateItem(11899);
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(13049);
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(15189 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(7159 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(12669 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(14109 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(17209 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(16209 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
			}
			break;
		case JOB_ASSASSIN:
			{
				item = ITEM_MANAGER::instance().CreateItem(11499);
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(13049);
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(15189 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(1139 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(12389 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(14109 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(17189 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
				item = ITEM_MANAGER::instance().CreateItem(16189 );
				if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
					M2_DESTROY_ITEM(item);
			}
			break;
	}
}

ACMD (do_attr_full_set)
{
	BYTE job = ch->GetJob();
	LPITEM item;

	switch (job)
	{
		case JOB_WARRIOR:
		case JOB_ASSASSIN:
		case JOB_SURA:
		case JOB_SHAMAN:
			{
				item = ch->GetWear(WEAR_HEAD);
				if (item != NULL)
				{
					item->ClearAttribute();
					item->SetForceAttribute( 0, APPLY_ATT_SPEED, 8);
					item->SetForceAttribute( 1, APPLY_HP_REGEN, 30);
					item->SetForceAttribute( 2, APPLY_SP_REGEN, 30);
					item->SetForceAttribute( 3, APPLY_DODGE, 15);
					item->SetForceAttribute( 4, APPLY_STEAL_SP, 10);
				}
				item = ch->GetWear(WEAR_WEAPON);
				if (item != NULL)
				{
					item->ClearAttribute();
					item->SetForceAttribute( 0, APPLY_CAST_SPEED, 20);
					item->SetForceAttribute( 1, APPLY_CRITICAL_PCT, 10);
					item->SetForceAttribute( 2, APPLY_PENETRATE_PCT, 10);
					item->SetForceAttribute( 3, APPLY_ATTBONUS_DEVIL, 20);
					item->SetForceAttribute( 4, APPLY_STR, 12);
				}
				item = ch->GetWear(WEAR_SHIELD);
				if (item != NULL)
				{
					item->ClearAttribute();
					item->SetForceAttribute( 0, APPLY_CON, 12);
					item->SetForceAttribute( 1, APPLY_BLOCK, 15);
					item->SetForceAttribute( 2, APPLY_REFLECT_MELEE, 10);
					item->SetForceAttribute( 3, APPLY_IMMUNE_STUN, 1);
					item->SetForceAttribute( 4, APPLY_IMMUNE_SLOW, 1);
				}
				item = ch->GetWear(WEAR_BODY);
				if (item != NULL)
				{
					item->ClearAttribute();
					item->SetForceAttribute( 0, APPLY_MAX_HP, 2000);
					item->SetForceAttribute( 1, APPLY_CAST_SPEED, 20);
					item->SetForceAttribute( 2, APPLY_STEAL_HP, 10);
					item->SetForceAttribute( 3, APPLY_REFLECT_MELEE, 10);
					item->SetForceAttribute( 4, APPLY_ATT_GRADE_BONUS, 50);
				}
				item = ch->GetWear(WEAR_FOOTS);
				if (item != NULL)
				{
					item->ClearAttribute();
					item->SetForceAttribute( 0, APPLY_MAX_HP, 2000);
					item->SetForceAttribute( 1, APPLY_MAX_SP, 80);
					item->SetForceAttribute( 2, APPLY_MOV_SPEED, 8);
					item->SetForceAttribute( 3, APPLY_ATT_SPEED, 8);
					item->SetForceAttribute( 4, APPLY_CRITICAL_PCT, 10);
				}
				item = ch->GetWear(WEAR_WRIST);
				if (item != NULL)
				{
					item->ClearAttribute();
					item->SetForceAttribute( 0, APPLY_MAX_HP, 2000);
					item->SetForceAttribute( 1, APPLY_MAX_SP, 80);
					item->SetForceAttribute( 2, APPLY_PENETRATE_PCT, 10);
					item->SetForceAttribute( 3, APPLY_STEAL_HP, 10);
					item->SetForceAttribute( 4, APPLY_MANA_BURN_PCT, 10);
				}
				item = ch->GetWear(WEAR_NECK);
				if (item != NULL)
				{
					item->ClearAttribute();
					item->SetForceAttribute( 0, APPLY_MAX_HP, 2000);
					item->SetForceAttribute( 1, APPLY_MAX_SP, 80);
					item->SetForceAttribute( 2, APPLY_CRITICAL_PCT, 10);
					item->SetForceAttribute( 3, APPLY_PENETRATE_PCT, 10);
					item->SetForceAttribute( 4, APPLY_STEAL_SP, 10);
				}
				item = ch->GetWear(WEAR_EAR);
				if (item != NULL)
				{
					item->ClearAttribute();
					item->SetForceAttribute( 0, APPLY_MOV_SPEED, 20);
					item->SetForceAttribute( 1, APPLY_MANA_BURN_PCT, 10);
					item->SetForceAttribute( 2, APPLY_POISON_REDUCE, 5);
					item->SetForceAttribute( 3, APPLY_ATTBONUS_DEVIL, 20);
					item->SetForceAttribute( 4, APPLY_ATTBONUS_UNDEAD, 20);
				}
			}
			break;
	}
}

ACMD (do_use_item)
{
	char arg1 [256];

	one_argument (argument, arg1, sizeof (arg1));

	int cell;
	str_to_number(cell, arg1);

	LPITEM item = ch->GetInventoryItem(cell);

	if (item)
		ch->UseItem(TItemPos (INVENTORY, cell));
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "This item doesn't exist.");
}

ACMD (do_clear_affect)
{
	ch->ClearAffect(true);
}

ACMD (do_dragon_soul)
{
	char arg1[512];
	const char* rest = one_argument (argument, arg1, sizeof(arg1));
	switch (arg1[0])
	{
		case 'a':
			{
				one_argument (rest, arg1, sizeof(arg1));
				int deck_idx;
				if (str_to_number(deck_idx, arg1) == false)
				{
					return;
				}
				ch->DragonSoul_ActivateDeck(deck_idx);
			}
			break;
		case 'd':
			{
				ch->DragonSoul_DeactivateAll();
			}
			break;
	}
}

ACMD (do_ds_list)
{
	for (int i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; i++)
	{
		TItemPos cell(DRAGON_SOUL_INVENTORY, i);

		LPITEM item = ch->GetItem(cell);
		if (item != NULL)
			ch->ChatPacket(CHAT_TYPE_INFO, "cell : %d, name : %s, id : %d", item->GetCell(), item->GetLocaleName(), item->GetID());
	}
}

ACMD(do_free_regen)
{
	ch->ChatPacket(CHAT_TYPE_INFO, "freeing regens on mapindex %ld", ch->GetMapIndex());
	regen_free_map(ch->GetMapIndex());
	ch->ChatPacket(CHAT_TYPE_INFO, "the regens now FREEEE! :)");
}

#ifdef ENABLE_RENEWAL_OFFLINESHOP
ACMD(do_set_shop)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);

	if (vecArgs.size() < 2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Command Examples:");
		ch->ChatPacket(CHAT_TYPE_INFO, "/set_shop close playerName");
		ch->ChatPacket(CHAT_TYPE_INFO, "/set_shop title playerName newTitle");
	}
	else if (vecArgs[1] == "info")
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Command Examples:");
		ch->ChatPacket(CHAT_TYPE_INFO, "/set_shop close playerName");
		ch->ChatPacket(CHAT_TYPE_INFO, "/set_shop title playerName newTitle");
	}
	else if (vecArgs[1] == "close")
	{
		if (vecArgs.size() < 3)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Command Examples:");
			ch->ChatPacket(CHAT_TYPE_INFO, "/set_shop close playerName");
			ch->ChatPacket(CHAT_TYPE_INFO, "/set_shop title playerName newTitle");
			return;
		}

		char playerName[CHARACTER_NAME_MAX_LEN + 1];
		strlcpy(playerName, vecArgs[2].c_str(), sizeof(playerName));
		DWORD playerID = COfflineShopManager::Instance().FindShopWithName(playerName);

		if (playerID == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "This shop doesn't exist!");
			return;
		}

		COfflineShopManager::Instance().CloseOfflineShopForTimeReal(playerID);
		ch->ChatPacket(CHAT_TYPE_INFO, "%s-%d succesfuly shop close.", playerName, playerID);
		sys_log(0, "ShopCloseCommand: %s-%d succesfuly shop close by %s", playerName, playerID, ch->GetName());
	}
	else if (vecArgs[1] == "title")
	{
		if (vecArgs.size() < 4)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Command Examples:");
			ch->ChatPacket(CHAT_TYPE_INFO, "/set_shop close playerName");
			ch->ChatPacket(CHAT_TYPE_INFO, "/set_shop title playerName newTitle");
			return;
		}

		char playerName[CHARACTER_NAME_MAX_LEN + 1];
		std::string shopTitle = "";
		strlcpy(playerName, vecArgs[2].c_str(), sizeof(playerName));
		for (DWORD j = 3; j < vecArgs.size(); ++j)
		{
			if (j > 3)
				shopTitle += " ";
			shopTitle += vecArgs[j].c_str();
		}

		DWORD playerID = COfflineShopManager::Instance().FindShopWithName(playerName);
		if (playerID == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "This shop doesn't exist!");
			return;
		}
		else if (getInjectText(shopTitle.c_str()))
		{
			sys_err("ShopSetTitleCommand: %s gm try to write inject sql!!!", ch->GetName());
			return;
		}

		LPOFFLINESHOP pkOfflineShop = COfflineShopManager::Instance().FindOfflineShopPID(playerID);
		if (!pkOfflineShop)
			return;
		char sign[SHOP_SIGN_MAX_LEN + 1];
		snprintf(sign, sizeof(sign), "%c%s", pkOfflineShop->m_data.sign[0], shopTitle.c_str());

		shop_title n;
		n.subheader = CHANGE_TITLE;
		n.title.owner_id = playerID;
		strlcpy(n.title.sign, sign, sizeof(n.title.sign));
		db_clientdesc->DBPacket(HEADER_GD_OFFLINESHOP, 0, &n, sizeof(shop_title));

		ch->ChatPacket(CHAT_TYPE_INFO, "%s-%d succesfuly set title. new title is %s", playerName, playerID, shopTitle.c_str());
		sys_log(0, "ShopSetTitleCommand: %s-%d succesfuly set title. new title is %s set by", playerName, playerID, shopTitle.c_str(), ch->GetName());
	}
}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
const char* pet_set_fields[] = {
	"level",
	"evo",
	"type",
	"exp",
	"iexp",
	"hp",
	"sp",
	"def",
	"itemevo",
	"time",
	"age",
	"\n",
};

ACMD(do_pet_set)
{
	char arg1[256], arg2[256];

	int i, len;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: pet_set <field> <value>");
		return;
	}

	if(!ch->GetActiveGrowthPet())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You need to summon your pet first.");
		return;
	}

	len = strlen(arg1);

	for (i = 0; *(pet_set_fields[i]) != '\n'; i++)
		if (!strncmp(arg1, pet_set_fields[i], len))
			break;

	switch (i)
	{
		case 0:
		{
			int level = 0;
			str_to_number(level, arg2);

			ch->GetActiveGrowthPet()->ChangePetPoint(POINT_UPBRINGING_PET_LEVEL, level, true);
		}
		break;

		case 1:
		{
			int evolution = 0;
			str_to_number(evolution, arg2);

			ch->GetActiveGrowthPet()->Evolve(evolution);
		}
		break;

		case 2:
		{
			int type = 0;
			str_to_number(type, arg2);
		}
		break;

		case 3:
		{
			int exp = 0;
			str_to_number(exp, arg2);

			ch->GetActiveGrowthPet()->RewardEXP(EXP_TYPE_MOB, exp);
		}
		break;

		case 4:
		{
			int item_exp = 0;
			str_to_number(item_exp, arg2);

			ch->GetActiveGrowthPet()->RewardEXP(EXP_TYPE_ITEM, item_exp);
		}
		break;

		case 5:
		{
			int hp = 0;
			str_to_number(hp, arg2);

			ch->GetActiveGrowthPet()->ChangePetPoint(POINT_UPBRINGING_PET_HP, hp, true);
		}
		break;

		case 6:
		{
			int sp = 0;
			str_to_number(sp, arg2);

			ch->GetActiveGrowthPet()->ChangePetPoint(POINT_UPBRINGING_PET_SP, sp, true);
		}
		break;

		case 7:
		{
			int def = 0;
			str_to_number(def, arg2);

			ch->GetActiveGrowthPet()->ChangePetPoint(POINT_UPBRINGING_PET_DEF_GRADE, def, true);
		}
		break;

		case 8:
		{
			int evolution = 0;
			str_to_number(evolution, arg2);

			if (!arPetEvolutionTable[evolution - 2].szEvolutionName)
				return;

			for (int i = 0; i < PET_EVOL_MAX_ITEM_COUNT; ++i)
			{
				std::pair<DWORD, WORD> itemPair = arPetEvolutionTable[evolution - 2].dwItems[i];
				ch->AutoGiveItem(itemPair.first, itemPair.second);
			}
		}
		break;

		case 9:
		{
			int endtime = 0;
			str_to_number(endtime, arg2);

			ch->GetActiveGrowthPet()->ChangePetPoint(POINT_UPBRINGING_DURATION, time(0) + endtime, true);
			ch->GetActiveGrowthPet()->GetSummonItem()->SetSocket(0, time(0) + endtime);
		}
		break;

		case 10:
		{
			int age = 0;
			str_to_number(age, arg2);
			DWORD dwAge = time(0) - (age * 3600 * 24);
			ch->GetActiveGrowthPet()->ChangePetPoint(POINT_UPBRINGING_BIRTHDAY, dwAge, true);
		}
		break;
	}
}
#endif

#ifdef ENABLE_BOT_PLAYER
#include "BotPlayer.h"
ACMD(do_BotCharacter)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int32_t spawn_count = 1;

	if (*arg1 && str_to_number(spawn_count, arg1) && spawn_count < 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Geçersiz spawn sayýsý. Lütfen 1 veya daha büyük bir deðer girin.");
		return;
	}

	CBotCharacterManager::instance().BotSpawn(ch, spawn_count);
}

// Empire bazlý bot spawn komutlarý
ACMD(do_BotSpawnA)  // Kýrmýzý Krallýk (Shinsoo)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int32_t spawn_count = 1;

	if (*arg1 && str_to_number(spawn_count, arg1) && spawn_count < 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Geçersiz spawn sayýsý. Lütfen 1 veya daha büyük bir deðer girin.");
		return;
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "Kýrmýzý Krallýk (Shinsoo) botlarý oluþturuluyor...");
	CBotCharacterManager::instance().BotSpawnShinsoo(ch, spawn_count);
}

ACMD(do_BotSpawnB)  // Sarý Krallýk (Chunjo)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int32_t spawn_count = 1;

	if (*arg1 && str_to_number(spawn_count, arg1) && spawn_count < 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Geçersiz spawn sayýsý. Lütfen 1 veya daha büyük bir deðer girin.");
		return;
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "Sarý Krallýk (Chunjo) botlarý oluþturuluyor...");
	CBotCharacterManager::instance().BotSpawnChunjo(ch, spawn_count);
}

ACMD(do_BotSpawnC)  // Mavi Krallýk (Jinno)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int32_t spawn_count = 1;

	if (*arg1 && str_to_number(spawn_count, arg1) && spawn_count < 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Geçersiz spawn sayýsý. Lütfen 1 veya daha büyük bir deðer girin.");
		return;
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "Mavi Krallýk (Jinno) botlarý oluþturuluyor...");
	CBotCharacterManager::instance().BotSpawnJinno(ch, spawn_count);
}

ACMD(do_BotCharacterDelete)
{
	if (ch)
	{
		CBotCharacterManager::instance().BotFullRemove();
	}
}

ACMD(do_BotCharacterDeleteName)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: /bot_delete_name <name>");
		return;
	}

	CBotCharacterManager::instance().BotCharacterRemove(arg1);
}

ACMD(do_BotCharacterTotal)
{
	if (ch)
	{
		const int botCount = CBotCharacterManager::instance().BotCharacterCount();
		ch->ChatPacket(CHAT_TYPE_INFO, "Sunucuda toplam %d bot bulunuyor.", botCount);
	}
}

// Bot dosyalarýný yeniden yükle
ACMD(do_BotReload)
{
	if (!ch)
		return;

	ch->ChatPacket(CHAT_TYPE_INFO, "Bot dosyalarý yeniden yükleniyor...");
	CBotCharacterManager::instance().Reload();
	ch->ChatPacket(CHAT_TYPE_INFO, "Bot dosyalarý baþarýyla yeniden yüklendi!");
	ch->ChatPacket(CHAT_TYPE_INFO, "Yeni botlar oluþturduðunuzda güncel ayarlar kullanýlacak.");
}
#endif

#ifdef ENABLE_DUNGEON_INFO
ACMD(do_reset_time_dungeon)
{
	char arg1[512];
	one_argument (argument, arg1, sizeof(arg1));


	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: reset_time_dr <id_dungeon>");
		return;
	}

	int id_dungeon = atoi(arg1);

	if(CDungeonInfoExtern::instance().GetIdDungeon(id_dungeon) ==false){
		ch->ChatPacket(CHAT_TYPE_INFO,"[Reset_Time_Respawn_Dungeon]No exist id dungeon");
		return;
	}

	if (CDungeonInfoExtern::instance().GetTimeRespawnDungeonActual(ch,CDungeonInfoExtern::instance().GetIndexMapDungeon(id_dungeon)) > 0){
		char dungeon_time_respawn[1024];
		snprintf(dungeon_time_respawn, sizeof(dungeon_time_respawn), "dungeon_info.dungeon_time_respawn_%ld", CDungeonInfoExtern::instance().GetIndexMapDungeon(id_dungeon));
		ch->SetQuestFlag(dungeon_time_respawn,get_global_time());

		ch->ChatPacket(CHAT_TYPE_INFO,"[Reset_Time_Respawn_Dungeon]Successfully");
	}

	ch->ChatPacket(CHAT_TYPE_INFO,"[Reset_Time_Respawn_Dungeon]No time Reset");

}
#endif

#ifdef ENABLE_MINI_GAME_CATCH_KING
ACMD(do_catch_king_event)
{
	char arg1[256], arg2[256];
	int iCommand = 1;
	int iDays = 1;
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 && !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: catch_king_event <command> <days_number>");
		ch->ChatPacket(CHAT_TYPE_INFO, "	0 = Disable.");
		ch->ChatPacket(CHAT_TYPE_INFO, "	1 = Enable.");
		ch->ChatPacket(CHAT_TYPE_INFO, "<days_number> Is the number of days that event is on.");
		return;
	}

	if (isnhdigit(*arg1) && isnhdigit(*arg2))
	{
		str_to_number(iCommand, arg1);
		str_to_number(iDays, arg2);
	}
	
	if(iDays <= 0)
		return;

	if(iCommand == 1)
	{
		if(quest::CQuestManager::instance().GetEventFlag("enable_catch_king_event") == 0)
		{
			int iEndTime = time(0) + 60*60*24*iDays;
			
			quest::CQuestManager::instance().RequestSetEventFlag("enable_catch_king_event", 1);
			quest::CQuestManager::instance().RequestSetEventFlag("enable_catch_king_event_drop", 1);
			quest::CQuestManager::instance().RequestSetEventFlag("catch_king_event_end_day", iEndTime);

			SendNotice("Catch king event is now active. Relog and check the icon beside minimap.");
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "This event is already opened.");
		}
	}
	else
	{
		quest::CQuestManager::instance().RequestSetEventFlag("enable_catch_king_event", 0);
		quest::CQuestManager::instance().RequestSetEventFlag("enable_catch_king_event_drop", 0);
		ch->ChatPacket(CHAT_TYPE_INFO, "You deactivated catch king event.");
	}
}
#endif

#ifdef ENABLE_EVENT_BANNER_FLAG
ACMD(do_banner)
{
	char arg1[256], arg2[256];
	int iEnable = 0;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2 || !isnhdigit(*arg1))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: /banner <1:0> <banner_name>");
		ch->ChatPacket(CHAT_TYPE_INFO, "Example: /banner 1 easter");
		return;
	}

	str_to_number(iEnable, arg1);

	CHARACTER_MANAGER::instance().SpawnBanners(iEnable, arg2);

	return;
}
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
ACMD(do_attendance)
{
	char arg1[256];
	int iCommand = 1;
	time_t iTime; time(&iTime); tm* pTimeInfo = localtime(&iTime);

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: attendance <command>");
		ch->ChatPacket(CHAT_TYPE_INFO, "	0 = Deaktif.");
		ch->ChatPacket(CHAT_TYPE_INFO, "	1 = Aktif.");
		return;
	}

	if (isnhdigit(*arg1))
	{
		str_to_number(iCommand, arg1);
	}

	if(iCommand == 1)
	{
		if(quest::CQuestManager::instance().GetEventFlag("enable_attendance_event") == 0)
		{
			quest::CQuestManager::instance().RequestSetEventFlag("enable_attendance_event", 1);
			quest::CQuestManager::instance().RequestSetEventFlag("attendance_start_day", pTimeInfo->tm_yday);

			//SendNotice("Attendance event is now active. Relog and check the icon beside minimap.");
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Zaten acik.");
		}
	}
	else
	{
		quest::CQuestManager::instance().RequestSetEventFlag("enable_attendance_event", 0);
		ch->ChatPacket(CHAT_TYPE_INFO, "Etkinligi kapattiniz.");
	}
}

ACMD(do_easter_event)
{
	char arg1[256];
	int iCommand = 1;
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: easter_event <command>");
		ch->ChatPacket(CHAT_TYPE_INFO, "	0 = Deaktif.");
		ch->ChatPacket(CHAT_TYPE_INFO, "	1 = Aktif.");
		return;
	}

	if (isnhdigit(*arg1))
	{
		str_to_number(iCommand, arg1);
	}
		
	if(iCommand == 1)
	{
		if(quest::CQuestManager::instance().GetEventFlag("enable_easter_event") == 0)
		{
			quest::CQuestManager::instance().RequestSetEventFlag("enable_easter_event", 1);
			//SendNotice("Paskalya eventi aktif.");
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Zaten acik.");
		}
	}
	else
	{
		quest::CQuestManager::instance().RequestSetEventFlag("enable_easter_event", 0);
		ch->ChatPacket(CHAT_TYPE_INFO, "Event Kapandi.");
	}
}
#endif

#ifdef ENABLE_CONQUEROR_LEVEL
ACMD(do_clevel)
{
	char arg2[256];
	one_argument(argument, arg2, sizeof(arg2));

	if (!*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: clevel <clevel>");
		return;
	}

	int	clevel = 0;
	str_to_number(clevel, arg2);

#ifdef ENABLE_CONQUEROR_LEVEL
	if (ch && clevel)
		ch->ResetConquerorPoint(MINMAX(0, clevel, gPlayerConquerorMaxLevel));
#endif

	//ch->ClearSkill();
	//ch->ClearSubSkill();
}

ACMD(do_conqueror_plus_amount)
{
	char szPoint[256];

	one_argument(argument, szPoint, sizeof(szPoint));

	if (*szPoint == '\0')
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;521]");
		return;
	}

	int nRemainPoint = ch->GetPoint(POINT_CONQUEROR_POINT);

	if (nRemainPoint <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;760]");
		return;
	}

	int nPoint = 0;
	str_to_number(nPoint, szPoint);

	if (nRemainPoint < nPoint)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;761]");
		return;
	}

	if (nPoint < 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;762]");
		return;
	}

	switch (subcmd)
	{
	case POINT_SUNGMA_HP:
		if (nPoint + ch->GetRealPoint(POINT_SUNGMA_HP) > 30) // FIX: bonus dahil GetPoint negatif nPoint uretiyordu
		{
			nPoint = 30 - ch->GetRealPoint(POINT_SUNGMA_HP);
		}
		break;

	case POINT_SUNGMA_IMMUNE:
		if (nPoint + ch->GetRealPoint(POINT_SUNGMA_IMMUNE) > 30) // FIX: bonus dahil GetPoint negatif nPoint uretiyordu
		{
			nPoint = 30 - ch->GetRealPoint(POINT_SUNGMA_IMMUNE);
		}
		break;

	case POINT_SUNGMA_STR:
		if (nPoint + ch->GetRealPoint(POINT_SUNGMA_STR) > 30) // FIX: bonus dahil GetPoint negatif nPoint uretiyordu
		{
			nPoint = 30 - ch->GetRealPoint(POINT_SUNGMA_STR);
		}
		break;

	case POINT_SUNGMA_MOVE:
		if (nPoint + ch->GetRealPoint(POINT_SUNGMA_MOVE) > 30) // FIX: bonus dahil GetPoint negatif nPoint uretiyordu
		{
			nPoint = 30 - ch->GetRealPoint(POINT_SUNGMA_MOVE);
		}
		break;

	default:
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;763]");
		return;
		break;
	}

	if (nPoint > 0 && nPoint <= ch->GetPoint(POINT_CONQUEROR_POINT)) // FIX: negatif/yetersiz puan
	{
		ch->SetRealPoint(subcmd, ch->GetRealPoint(subcmd) + nPoint);
		ch->SetPoint(subcmd, ch->GetPoint(subcmd) + nPoint);
		ch->ComputePoints();
		ch->PointChange(subcmd, 0);

		ch->PointChange(POINT_CONQUEROR_POINT, -nPoint);
		ch->ComputePoints();
	}
}

ACMD(do_set_conqueror)
{
	char szName[256];
	char szChangeAmount[256];

	two_arguments(argument, szName, sizeof(szName), szChangeAmount, sizeof(szChangeAmount));

	if (*szName == '\0' || *szChangeAmount == '\0')
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Invalid argument.");
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::Instance().FindPC(szName);

	if (!tch)
	{
		CCI* pkCCI = P2P_MANAGER::Instance().Find(szName);

		if (pkCCI)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find player(%s). %s is not in your game server.", szName, szName);
			return;
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find player(%s). Perhaps %s doesn't login or exist.", szName, szName);
			return;
		}
	}
	else
	{
		if (tch->IsPolymorphed())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;521]");
			return;
		}

		if (subcmd != POINT_SUNGMA_HP && subcmd != POINT_SUNGMA_IMMUNE && subcmd != POINT_SUNGMA_STR && subcmd != POINT_SUNGMA_MOVE)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;763]");
			return;
		}
		int nRemainPoint = tch->GetPoint(POINT_CONQUEROR_POINT);
		int nCurPoint = tch->GetRealPoint(subcmd);
		int nChangeAmount = 0;
		str_to_number(nChangeAmount, szChangeAmount);
		int nPoint = nCurPoint + nChangeAmount;

		int n = -1;
		switch (subcmd)
		{
		case POINT_SUNGMA_HP:
			if (nPoint < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot set stat under initial stat.");
				return;
			}
			n = 0;
			break;

		case POINT_SUNGMA_IMMUNE:
			if (nPoint < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot set stat under initial stat.");
				return;
			}
			n = 1;
			break;

		case POINT_SUNGMA_STR:
			if (nPoint < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot set stat under initial stat.");
				return;
			}
			n = 2;
			break;

		case POINT_SUNGMA_MOVE:
			if (nPoint < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Cannot set stat under initial stat.");
				return;
			}
			n = 3;
			break;
		}

		if (nPoint > 30)
		{
			nChangeAmount -= nPoint - 30;
			nPoint = 30;
		}

		if (nRemainPoint < nChangeAmount)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "[LS;761]");
			return;
		}

		tch->SetRealPoint(subcmd, nPoint);
		tch->SetPoint(subcmd, tch->GetPoint(subcmd) + nChangeAmount);
		tch->ComputePoints();
		tch->PointChange(subcmd, 0);

		tch->PointChange(POINT_CONQUEROR_POINT, -nChangeAmount);
		tch->ComputePoints();

		const char* stat_name[4] = { "ccon", "cimu", "cstr", "cmov" };
		if (-1 == n)
			return;
		ch->ChatPacket(CHAT_TYPE_INFO, "%s's %s change %d to %d", szName, stat_name[n], nCurPoint, nPoint);
	}
}

ACMD(do_state_sungma)
{
	char arg1[256];
	LPCHARACTER tch;

	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		if (arg1[0] == '#')
		{
			tch = CHARACTER_MANAGER::Instance().Find(strtoul(arg1 + 1, NULL, 10));
		}
		else
		{
			LPDESC d = DESC_MANAGER::Instance().FindByCharacterName(arg1);

			if (!d)
				tch = NULL;
			else
				tch = d->GetCharacter();
		}
	}
	else
		tch = ch;

	if (!tch)
		return;

	char buf[256];

	snprintf(buf, sizeof(buf), "%s's State: ", tch->GetName());

	if (tch->IsPosition(POS_FIGHTING))
		strlcat(buf, "Battle", sizeof(buf));
	else if (tch->IsPosition(POS_DEAD))
		strlcat(buf, "Dead", sizeof(buf));
	else
		strlcat(buf, "Standing", sizeof(buf));

	if (tch->GetShop()) //@fixme526
		strlcat(buf, ", Shop", sizeof(buf));

	if (tch->GetExchange()) //@fixme526
		strlcat(buf, ", Exchange", sizeof(buf));

	ch->ChatPacket(CHAT_TYPE_INFO, "%s", buf);

	int len;
	len = snprintf(buf, sizeof(buf), "Coordinate %ldx%ld (%ldx%ld)",
		tch->GetX(), tch->GetY(), tch->GetX() / 100, tch->GetY() / 100);

	if (len < 0 || len >= (int)sizeof(buf))
		len = sizeof(buf) - 1;

	LPSECTREE pSec = SECTREE_MANAGER::Instance().Get(tch->GetMapIndex(), tch->GetX(), tch->GetY());

	if (pSec)
	{
		TMapSetting& map_setting = SECTREE_MANAGER::Instance().GetMap(tch->GetMapIndex())->m_setting;
		snprintf(buf + len, sizeof(buf) - len, " MapIndex %ld Attribute %08X Local Position (%ld x %ld)",
			tch->GetMapIndex(), pSec->GetAttribute(tch->GetX(), tch->GetY()), (tch->GetX() - map_setting.iBaseX) / 100, (tch->GetY() - map_setting.iBaseY) / 100);
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "%s", buf);

	ch->ChatPacket(CHAT_TYPE_INFO, "C_LEV %d C_LEV_STEP %d", tch->GetConquerorLevel(), tch->GetPoint(POINT_CONQUEROR_LEVEL_STEP));
	ch->ChatPacket(CHAT_TYPE_INFO, "C_POINT %d", tch->GetPoint(POINT_CONQUEROR_POINT));

	ch->ChatPacket(CHAT_TYPE_INFO, "SUNGMA:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   SUNGMA_STR:%d SUNGMA_HP:%d SUNGMA_MOVE:%d SUNGMA_IMMUN:%d",
		tch->GetPoint(POINT_SUNGMA_STR),
		tch->GetPoint(POINT_SUNGMA_HP),
		tch->GetPoint(POINT_SUNGMA_MOVE),
		tch->GetPoint(POINT_SUNGMA_IMMUNE)
	);
}
#endif
