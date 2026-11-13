#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_COSTUME_SHOP_BAERIM
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "locale_service.h"
#include "costume_shop.h"

#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>

namespace
{
	struct SCostumeShopEntry
	{
		DWORD	id;
		BYTE	category;
		DWORD	vnum;
		DWORD	cost[COSTUME_SHOP_COUPON_COUNT];
		DWORD	startTime;
		DWORD	endTime;
	};

	const DWORD c_adwCoupon[COSTUME_SHOP_COUPON_COUNT] = { COSTUME_SHOP_COUPON_COSTUME, COSTUME_SHOP_COUPON_MOUNT, COSTUME_SHOP_COUPON_PET };

	std::vector<SCostumeShopEntry>	s_vecEntry;
	std::map<DWORD, size_t>			s_mapEntry;
	bool							s_bLoaded = false;
	std::map<DWORD, DWORD>			s_mapShopNPC;	// pid -> vid of the npc the shop was opened at

	void Load()
	{
		s_bLoaded = true;
		s_vecEntry.clear();
		s_mapEntry.clear();

		char szPath[256];
		snprintf(szPath, sizeof(szPath), "%s/costume_shop.txt", LocaleService_GetBasePath().c_str());
		std::ifstream in(szPath);
		if (!in.is_open())
		{
			sys_err("costume shop: cannot open %s", szPath);
			return;
		}

		std::string line;
		int iLine = 0, iSkipped = 0;
		while (std::getline(in, line))
		{
			++iLine;
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);
			if (line.empty() || line[0] == '#')
				continue;

			std::istringstream ss(line);
			SCostumeShopEntry e{};
			unsigned int uCategory = 0;
			if (!(ss >> e.id >> uCategory >> e.vnum >> e.cost[0] >> e.cost[1] >> e.cost[2] >> e.startTime >> e.endTime))
			{
				sys_err("costume shop: %s line %d is malformed", szPath, iLine);
				continue;
			}
			e.category = static_cast<BYTE>(uCategory);

			if (!e.id || s_mapEntry.find(e.id) != s_mapEntry.end())
			{
				sys_err("costume shop: line %d has a duplicate / zero id %u", iLine, e.id);
				continue;
			}
			if (!ITEM_MANAGER::instance().GetTable(e.vnum))
			{
				++iSkipped;	// item not imported (yet)
				continue;
			}
			s_mapEntry[e.id] = s_vecEntry.size();
			s_vecEntry.push_back(e);
		}
		sys_log(0, "costume shop: %u entries loaded, %d skipped (no item)", (unsigned)s_vecEntry.size(), iSkipped);
	}

	bool IsAvailable(const SCostumeShopEntry& e, DWORD dwNow)
	{
		if (e.startTime && dwNow < e.startTime)
			return false;
		if (e.endTime && dwNow > e.endTime)
			return false;
		return true;
	}

	void SendResult(LPCHARACTER ch, int iResult)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CostumeShopResult %d", iResult);
	}

	bool IsNearShopNPC(LPCHARACTER ch)
	{
		const auto it = s_mapShopNPC.find(ch->GetPlayerID());
		if (it == s_mapShopNPC.end())
			return false;
		LPCHARACTER npc = CHARACTER_MANAGER::instance().Find(it->second);
		if (!npc || npc->GetRaceNum() != COSTUME_SHOP_NPC_VNUM || npc->GetMapIndex() != ch->GetMapIndex())
			return false;
		return DISTANCE_APPROX(ch->GetX() - npc->GetX(), ch->GetY() - npc->GetY()) <= COSTUME_SHOP_NPC_DISTANCE;
	}

	void Buy(LPCHARACTER ch, const char* szList)
	{
		if (!IsNearShopNPC(ch))
		{
			SendResult(ch, COSTUME_SHOP_RESULT_FAR);
			return;
		}
		if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
			return;

		const DWORD dwNow = get_global_time();
		std::vector<const SCostumeShopEntry*> vecBuy;
		std::set<DWORD> setSeen;
		DWORD adwNeed[COSTUME_SHOP_COUPON_COUNT] = {};
		int iCells = 0;

		std::string list(szList);
		std::replace(list.begin(), list.end(), ',', ' ');
		std::istringstream ss(list);
		DWORD dwID;
		while (ss >> dwID)
		{
			const auto it = s_mapEntry.find(dwID);
			if (it == s_mapEntry.end() || !setSeen.insert(dwID).second || vecBuy.size() >= COSTUME_SHOP_MAX_BUY)
			{
				SendResult(ch, COSTUME_SHOP_RESULT_INVALID);
				return;
			}
			const SCostumeShopEntry& e = s_vecEntry[it->second];
			if (!IsAvailable(e, dwNow))
			{
				SendResult(ch, COSTUME_SHOP_RESULT_INVALID);
				return;
			}
			const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(e.vnum);
			if (!pTable)
			{
				SendResult(ch, COSTUME_SHOP_RESULT_INVALID);
				return;
			}
			for (int i = 0; i < COSTUME_SHOP_COUPON_COUNT; ++i)
				adwNeed[i] += e.cost[i];
			iCells += MAX(1, (int)pTable->bSize);
			vecBuy.push_back(&e);
		}
		if (vecBuy.empty())
		{
			SendResult(ch, COSTUME_SHOP_RESULT_INVALID);
			return;
		}

		for (int i = 0; i < COSTUME_SHOP_COUPON_COUNT; ++i)
		{
			if (adwNeed[i] && ch->CountSpecifyItem(c_adwCoupon[i]) < (int)adwNeed[i])
			{
				SendResult(ch, COSTUME_SHOP_RESULT_NO_COUPON);
				return;
			}
		}

		if (ch->CountEmptyInventory() < iCells)
		{
			SendResult(ch, COSTUME_SHOP_RESULT_NO_SPACE);
			return;
		}
		for (const SCostumeShopEntry* e : vecBuy)
		{
			const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(e->vnum);
			if (ch->GetEmptyInventory(pTable->bSize) == -1)
			{
				SendResult(ch, COSTUME_SHOP_RESULT_NO_SPACE);
				return;
			}
		}

		for (int i = 0; i < COSTUME_SHOP_COUPON_COUNT; ++i)
		{
			if (adwNeed[i])
				ch->RemoveSpecifyItem(c_adwCoupon[i], adwNeed[i]);
		}
		for (const SCostumeShopEntry* e : vecBuy)
		{
			ch->AutoGiveItem(e->vnum, 1);
			sys_log(0, "COSTUME_SHOP: %s (%u) bought entry %u item %u", ch->GetName(), ch->GetPlayerID(), e->id, e->vnum);
		}
		SendResult(ch, COSTUME_SHOP_RESULT_OK);
	}
}

#ifdef ENABLE_WARDROBE_SYSTEM
void CostumeShop_GetCatalog(std::vector<DWORD>& vecVnum)
{
	if (!s_bLoaded)
		Load();
	vecVnum.reserve(s_vecEntry.size());
	for (const SCostumeShopEntry& e : s_vecEntry)
		vecVnum.push_back(e.vnum);
}
#endif

void CostumeShop_Open(LPCHARACTER ch, LPCHARACTER npc)
{
	if (!ch || !ch->GetDesc() || !npc)
		return;
	if (!s_bLoaded)
		Load();

	s_mapShopNPC[ch->GetPlayerID()] = npc->GetVID();

	const DWORD dwNow = get_global_time();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "CostumeShopClear");

	std::string chunk;
	for (const SCostumeShopEntry& e : s_vecEntry)
	{
		if (e.endTime && dwNow > e.endTime)
			continue;
		char sz[96];
		snprintf(sz, sizeof(sz), "%u,%u,%u,%u,%u,%u,%u,%u", e.id, e.category, e.vnum, e.cost[0], e.cost[1], e.cost[2], e.startTime, e.endTime);
		if (!chunk.empty() && chunk.size() + strlen(sz) + 1 > 380)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "CostumeShopAdd %s", chunk.c_str());
			chunk.clear();
		}
		if (!chunk.empty())
			chunk += ';';
		chunk += sz;
	}
	if (!chunk.empty())
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CostumeShopAdd %s", chunk.c_str());

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CostumeShopOpen %u", dwNow);
}

void CostumeShop_Command(LPCHARACTER ch, const char* argument)
{
	if (!ch || !ch->GetDesc())
		return;
	if (!s_bLoaded)
		Load();

	char arg1[32];
	argument = one_argument(argument, arg1, sizeof(arg1));

	if (!strcmp(arg1, "_buy"))
	{
		char arg2[256];
		one_argument(argument, arg2, sizeof(arg2));
		Buy(ch, arg2);
	}
	else if (!strcmp(arg1, "_reload") && ch->GetGMLevel() >= GM_IMPLEMENTOR)
	{
		Load();
		ch->ChatPacket(CHAT_TYPE_INFO, "costume shop: %u entries", (unsigned)s_vecEntry.size());
	}
}
#endif
