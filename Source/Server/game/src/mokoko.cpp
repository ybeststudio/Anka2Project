#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_MOKOKO_SYSTEM
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "desc.h"
#include "db.h"
#include "item.h"
#include "item_manager.h"
#include "locale_service.h"
#include "sectree_manager.h"
#include "packet.h"
#include "mokoko.h"

#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <memory>

namespace
{
	const DWORD MOKOKO_SEED_VNUM = 9101;
	const int MOKOKO_COLLECT_DISTANCE = 700;

	struct SSeed
	{
		DWORD	id;
		int		region;
		long	mapIndex;
		long	x;			// relative to the map base
		long	y;
	};

	struct SRewardItem
	{
		DWORD	vnum;
		WORD	count;
	};

	struct SReward
	{
		DWORD	id;
		int		type;		// 0 milestone, 1 chest
		int		required;
		std::vector<SRewardItem> items;
	};

	struct SPlayerData
	{
		std::set<DWORD>	seeds;
		std::set<DWORD>	rewards;
	};

	std::map<DWORD, SSeed>			s_mapSeed;
	std::map<int, int>				s_mapRegionTotal;
	std::map<DWORD, SReward>		s_mapReward;		// ordered by id: milestones 1.., chests 101..
	std::map<DWORD, DWORD>			s_mapSeedByVID;
	std::map<DWORD, SPlayerData>	s_mapPlayer;
	bool							s_bSeedsLoaded = false;
	bool							s_bRewardsLoaded = false;

	bool ReadLine(std::ifstream& in, std::string& line)
	{
		while (std::getline(in, line))
		{
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);
			if (!line.empty() && line[0] != '#')
				return true;
		}
		return false;
	}

	void LoadSeeds()
	{
		s_bSeedsLoaded = true;
		s_mapSeed.clear();
		s_mapRegionTotal.clear();

		char szPath[256];
		snprintf(szPath, sizeof(szPath), "%s/mokoko_seeds.txt", LocaleService_GetBasePath().c_str());
		std::ifstream in(szPath);
		if (!in.is_open())
		{
			sys_err("mokoko: cannot open %s", szPath);
			return;
		}

		std::string line;
		while (ReadLine(in, line))
		{
			std::istringstream ss(line);
			SSeed s{};
			if (!(ss >> s.id >> s.region >> s.mapIndex >> s.x >> s.y) || !s.id)
			{
				sys_err("mokoko: bad seed line '%s'", line.c_str());
				continue;
			}
			if (s_mapSeed.insert(std::make_pair(s.id, s)).second)
				++s_mapRegionTotal[s.region];
		}
		sys_log(0, "mokoko: %u seeds", (unsigned) s_mapSeed.size());
	}

	void LoadRewards()
	{
		s_bRewardsLoaded = true;
		s_mapReward.clear();

		char szPath[256];
		snprintf(szPath, sizeof(szPath), "%s/mokoko_rewards.txt", LocaleService_GetBasePath().c_str());
		std::ifstream in(szPath);
		if (!in.is_open())
		{
			sys_err("mokoko: cannot open %s", szPath);
			return;
		}

		std::string line;
		while (ReadLine(in, line))
		{
			std::istringstream ss(line);
			DWORD id = 0, vnum = 0;
			int type = 0, required = 0, count = 0;
			if (!(ss >> id >> type >> required >> vnum >> count) || !id || required <= 0 || count <= 0 || count > 200
				|| !ITEM_MANAGER::instance().GetTable(vnum))
			{
				sys_err("mokoko: bad reward line '%s'", line.c_str());
				continue;
			}
			SReward& r = s_mapReward[id];
			r.id = id;
			r.type = type;
			r.required = required;
			r.items.push_back(SRewardItem{ vnum, (WORD) count });
		}
		sys_log(0, "mokoko: %u rewards", (unsigned) s_mapReward.size());
	}

	const std::map<DWORD, SSeed>& Seeds()
	{
		if (!s_bSeedsLoaded)
			LoadSeeds();
		return s_mapSeed;
	}

	const std::map<DWORD, SReward>& Rewards()
	{
		if (!s_bRewardsLoaded)
			LoadRewards();
		return s_mapReward;
	}

	SPlayerData& Data(const CHARACTER* ch)
	{
		const DWORD dwPID = ch->GetPlayerID();
		auto it = s_mapPlayer.find(dwPID);
		if (it != s_mapPlayer.end())
			return it->second;

		SPlayerData& d = s_mapPlayer[dwPID];
		if (!dwPID)
			return d;

		{
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT seed_id FROM mokoko_seed WHERE pid = %u", dwPID));
			if (pMsg->Get())
				for (unsigned int n = 0; n < pMsg->Get()->uiNumRows; ++n)
				{
					MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
					DWORD id = 0;
					str_to_number(id, row[0]);
					d.seeds.insert(id);
				}
		}
		{
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT reward_id FROM mokoko_reward WHERE pid = %u", dwPID));
			if (pMsg->Get())
				for (unsigned int n = 0; n < pMsg->Get()->uiNumRows; ++n)
				{
					MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
					DWORD id = 0;
					str_to_number(id, row[0]);
					d.rewards.insert(id);
				}
		}
		return d;
	}

	// only seeds that are still in the seed list count (an edited list must not leave old ids behind)
	int FoundCount(const SPlayerData& d)
	{
		const std::map<DWORD, SSeed>& seeds = Seeds();
		int n = 0;
		for (DWORD id : d.seeds)
			if (seeds.find(id) != seeds.end())
				++n;
		return n;
	}

	int PendingCount(const SPlayerData& d, int iFound)
	{
		int n = 0;
		for (const auto& kv : Rewards())
			if (kv.second.required <= iFound && d.rewards.find(kv.first) == d.rewards.end())
				++n;
		return n;
	}

	void SendData(LPCHARACTER ch)
	{
		const SPlayerData& d = Data(ch);
		const std::map<DWORD, SSeed>& seeds = Seeds();
		std::map<int, int> regionFound;
		for (DWORD id : d.seeds)
		{
			auto it = seeds.find(id);
			if (it != seeds.end())
				++regionFound[it->second.region];
		}
		const int iFound = FoundCount(d);

		std::string regions;
		char buf[64];
		for (const auto& kv : s_mapRegionTotal)
		{
			snprintf(buf, sizeof(buf), "%s%d|%d|%d", regions.empty() ? "" : "#", kv.first, regionFound[kv.first], kv.second);
			regions += buf;
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "MokokoData %d %u %d %s", iFound, (unsigned) seeds.size(), PendingCount(d, iFound),
			regions.empty() ? "0" : regions.c_str());
	}

	void SendRewards(LPCHARACTER ch)
	{
		const SPlayerData& d = Data(ch);
		const int iFound = FoundCount(d);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "MokokoRewardsBegin %d %u", iFound, (unsigned) Seeds().size());
		for (const auto& kv : Rewards())
		{
			const SReward& r = kv.second;
			std::string items;
			char buf[32];
			for (const SRewardItem& it : r.items)
			{
				snprintf(buf, sizeof(buf), "%s%u:%u", items.empty() ? "" : ",", it.vnum, it.count);
				items += buf;
			}
			ch->ChatPacket(CHAT_TYPE_COMMAND, "MokokoRewardEntry %u %d %d %d %s", r.id, r.type, r.required,
				d.rewards.find(r.id) != d.rewards.end() ? 1 : 0, items.c_str());
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "MokokoRewardsEnd %d", PendingCount(d, iFound));
	}

	// every reward item must fit before anything is given: no half claimed rewards
	bool HasRoom(LPCHARACTER ch, const SReward& r)
	{
		int iNeeded = 0;
		for (const SRewardItem& it : r.items)
		{
			const TItemTable* t = ITEM_MANAGER::instance().GetTable(it.vnum);
			if (!t || ch->GetEmptyInventory(t->bSize) < 0)
				return false;
			iNeeded += (t->dwFlags & ITEM_FLAG_STACKABLE) ? 1 : it.count;
		}
		if (iNeeded <= 1)
			return true;

		// several items: count free single cells of the open inventory pages
		int iFree = 0;
#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
		const int iMax = 90 + (5 * ch->GetUnlockSlotsW());
#else
		const int iMax = INVENTORY_MAX_NUM;
#endif
		for (int i = 0; i < iMax && iFree < iNeeded; ++i)
			if (ch->IsEmptyItemGrid(TItemPos(INVENTORY, i), 1))
				++iFree;
		return iFree >= iNeeded;
	}

	// the seed stays in the world for everybody else: only this character's client drops it
	void RemoveSeedFor(LPCHARACTER ch, LPCHARACTER pkSeed)
	{
		if (!ch->GetDesc())
			return;
		TPacketGCCharacterDelete pack;
		pack.header = HEADER_GC_CHARACTER_DEL;
		pack.id = pkSeed->GetVID();
		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}

	int Claim(LPCHARACTER ch, DWORD dwRewardID)
	{
		const std::map<DWORD, SReward>& rewards = Rewards();
		auto it = rewards.find(dwRewardID);
		if (it == rewards.end())
			return MOKOKO_REWARD_RESULT_INVALID;
		const SReward& r = it->second;

		SPlayerData& d = Data(ch);
		if (d.rewards.find(r.id) != d.rewards.end())
			return MOKOKO_REWARD_RESULT_ALREADY_CLAIMED;
		if (FoundCount(d) < r.required)
			return MOKOKO_REWARD_RESULT_LOCKED;
		if (ch->IsDead() || !ch->CanHandleItem() || ch->GetExchange() || ch->GetMyShop())
			return MOKOKO_REWARD_RESULT_INVALID;
		if (!HasRoom(ch, r))
			return MOKOKO_REWARD_RESULT_INVENTORY_FULL;

		// the database row is the guard: a second core or a double click can never claim the same reward twice
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"INSERT IGNORE INTO mokoko_reward (pid, reward_id) VALUES (%u, %u)", ch->GetPlayerID(), r.id));
		if (!pMsg->Get() || pMsg->Get()->uiAffectedRows != 1)
		{
			d.rewards.insert(r.id);
			return pMsg->Get() ? MOKOKO_REWARD_RESULT_ALREADY_CLAIMED : MOKOKO_REWARD_RESULT_DATABASE_ERROR;
		}
		d.rewards.insert(r.id);

		for (const SRewardItem& item : r.items)
		{
			const TItemTable* t = ITEM_MANAGER::instance().GetTable(item.vnum);
			if (t && !(t->dwFlags & ITEM_FLAG_STACKABLE))
				for (int i = 0; i < item.count; ++i)
					ch->AutoGiveItem(item.vnum, 1);
			else
				ch->AutoGiveItem(item.vnum, item.count);
		}
		sys_log(0, "mokoko: %s claimed reward %u", ch->GetName(), r.id);
		return MOKOKO_REWARD_RESULT_OK;
	}
}

void Mokoko_SpawnMap(long lMapIndex, long lBaseX, long lBaseY)
{
	int iSpawned = 0;
	for (const auto& kv : Seeds())
	{
		const SSeed& s = kv.second;
		if (s.mapIndex != lMapIndex)
			continue;

		LPCHARACTER pkSeed = CHARACTER_MANAGER::instance().SpawnMob(MOKOKO_SEED_VNUM, lMapIndex, lBaseX + s.x, lBaseY + s.y, 0, false, -1, true);
		if (!pkSeed)
		{
			sys_err("mokoko: seed %u cannot spawn on map %ld (%ld %ld)", s.id, lMapIndex, s.x, s.y);
			continue;
		}
		s_mapSeedByVID[pkSeed->GetVID()] = s.id;
		++iSpawned;
	}
	if (iSpawned)
		sys_log(0, "mokoko: map %ld %d seeds", lMapIndex, iSpawned);
}

bool Mokoko_IsHiddenFor(const CHARACTER* pkSeed, const CHARACTER* pkViewer)
{
	if (pkSeed->GetRaceNum() != MOKOKO_SEED_VNUM || !pkViewer->IsPC())
		return false;

	auto it = s_mapSeedByVID.find(pkSeed->GetVID());
	if (it == s_mapSeedByVID.end())
		return false;

	const SPlayerData& d = Data(pkViewer);
	return d.seeds.find(it->second) != d.seeds.end();
}

bool Mokoko_OnClick(LPCHARACTER ch, LPCHARACTER pkSeed)
{
	if (pkSeed->GetRaceNum() != MOKOKO_SEED_VNUM)
		return false;

	auto itVID = s_mapSeedByVID.find(pkSeed->GetVID());
	if (itVID == s_mapSeedByVID.end())
		return false;	// a plain 9101 (quest spawn): normal NPC click

	if (!ch->IsPC() || !ch->GetDesc() || ch->IsDead())
		return true;

	if (ch->GetMapIndex() != pkSeed->GetMapIndex() || DISTANCE_APPROX(ch->GetX() - pkSeed->GetX(), ch->GetY() - pkSeed->GetY()) > MOKOKO_COLLECT_DISTANCE)
		return true;

	// one seed a second: the click packet can be repeated
	if (ch->GetProtectTime("mokoko_collect") > get_global_time())
		return true;
	ch->SetProtectTime("mokoko_collect", get_global_time() + 1);

	SPlayerData& d = Data(ch);
	const DWORD dwSeed = itVID->second;
	if (!d.seeds.insert(dwSeed).second)
	{
		RemoveSeedFor(ch, pkSeed);
		return true;
	}

	DBManager::instance().Query("INSERT IGNORE INTO mokoko_seed (pid, seed_id) VALUES (%u, %u)", ch->GetPlayerID(), dwSeed);

	if (ch->IsRiding())
		ch->StopRiding();

	RemoveSeedFor(ch, pkSeed);

	const std::map<DWORD, SSeed>& seeds = Seeds();
	auto itSeed = seeds.find(dwSeed);
	const int iRegion = itSeed != seeds.end() ? itSeed->second.region : 0;
	int iRegionFound = 0;
	for (DWORD id : d.seeds)
	{
		auto it = seeds.find(id);
		if (it != seeds.end() && it->second.region == iRegion)
			++iRegionFound;
	}
	const int iFound = FoundCount(d);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "MokokoFound %d %u %d %d %d %d", iFound, (unsigned) seeds.size(), iRegion, iRegionFound,
		s_mapRegionTotal[iRegion], PendingCount(d, iFound));
	sys_log(0, "mokoko: %s found seed %u (%d)", ch->GetName(), dwSeed, iFound);
	return true;
}

void Mokoko_OnLogin(LPCHARACTER ch)
{
	if (ch && ch->IsPC())
		Data(ch);
}

void Mokoko_OnDestroy(LPCHARACTER ch)
{
	if (ch && ch->GetPlayerID())
		s_mapPlayer.erase(ch->GetPlayerID());
}

void Mokoko_Command(LPCHARACTER ch, const char* szCommand, const char* argument)
{
	if (!ch || !ch->GetDesc())
		return;

	if (!strcmp(szCommand, "info"))
	{
		SendData(ch);
		return;
	}

	if (!strcmp(szCommand, "rewards"))
	{
		SendRewards(ch);
		return;
	}

	if (!strcmp(szCommand, "claim"))
	{
		char arg1[64];
		one_argument(argument, arg1, sizeof(arg1));
		DWORD dwID = 0;
		if (!*arg1 || !str_to_number(dwID, arg1))
			return;

		if (ch->GetProtectTime("mokoko_claim") > get_global_time())
			return;
		ch->SetProtectTime("mokoko_claim", get_global_time() + 1);

		const int iResult = Claim(ch, dwID);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "MokokoRewardResult %d %u", iResult, dwID);
		if (iResult == MOKOKO_REWARD_RESULT_OK || iResult == MOKOKO_REWARD_RESULT_ALREADY_CLAIMED)
		{
			SendRewards(ch);
			SendData(ch);
		}
		return;
	}

	if (!strcmp(szCommand, "reload") && ch->GetGMLevel() >= GM_IMPLEMENTOR)
	{
		LoadRewards();	// seeds stay: they are spawned when the map loads
		ch->ChatPacket(CHAT_TYPE_INFO, "mokoko_rewards.txt: %u", (unsigned) s_mapReward.size());
	}
}
#endif
