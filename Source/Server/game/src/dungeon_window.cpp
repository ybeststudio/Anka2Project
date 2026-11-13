#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_DUNGEON_WINDOW_BAERIM
#include <tuple>
#include "utils.h"
#include "config.h"
#include "constants.h"
#include "char.h"
#include "char_manager.h"
#include "sectree_manager.h"
#include "db.h"
#include "item_manager.h"
#include "locale_service.h"
#include "dungeon_window.h"

// Baerim dungeon window (reference uiTeleport dungeon page / dungeonData):
// - best boss DPS per player (quest flag dungeon.<boss>_dps); completed / fastest come from the Arena2 quests
// - daily entry limit, counted when a player enters a new instance of the dungeon, reset at midnight
// - server average / best run time of all players
// - "Giriş Yap": checks level, cooldown and daily limit and warps next to the dungeon's entry NPC
// Client protocol: the dungeon_info_cooldown rows get the extra fields of DungeonWindow_ExtraInfo,
// results of /dungeon_info enter come back as "DungeonWindowResult <code> <boss>".

namespace
{
	struct SDungeonWindowConfig
	{
		DWORD bossIdx;
		long baseMap;		// instance map index / 10000
		int dailyLimit;
		long warpX;			// next to the entry NPC (global coordinates)
		long warpY;
	};

	// filled from the quests (entry NPC, instance map); daily limits follow Baerim's dungeonData where the dungeon matches
	const SDungeonWindowConfig s_aConfig[] =
	{
#include "dungeon_window_config.h"
	};

	enum EResult
	{
		RESULT_OK = 0,
		RESULT_LEVEL = 1,
		RESULT_COOLDOWN = 2,
		RESULT_DAILY_LIMIT = 3,
		RESULT_BUSY = 4,
		RESULT_NO_ENTRY = 5,
		RESULT_TEST_UNAVAILABLE = 6,
		RESULT_DAILY_KICKED = 7,
		RESULT_PC_LIMIT = 8,	// ENABLE_DUNGEON_LIMIT_RENEWAL: the computer used up the dungeon's daily limit
	};

	const SDungeonWindowConfig* FindByBoss(DWORD bossIdx)
	{
		for (const auto& cfg : s_aConfig)
			if (cfg.bossIdx == bossIdx)
				return &cfg;
		return nullptr;
	}

	const SDungeonWindowConfig* FindByMap(long mapIndex)
	{
		if (mapIndex < 10000)
			return nullptr;
		const long base = mapIndex / 10000;
		for (const auto& cfg : s_aConfig)
			if (cfg.baseMap == base)
				return &cfg;
		return nullptr;
	}

	int Flag(LPCHARACTER ch, DWORD bossIdx, const char* name)
	{
		char sz[64];
		snprintf(sz, sizeof(sz), "dungeon.%u_%s", bossIdx, name);
		return ch->GetQuestFlag(sz);
	}

	void SetFlag(LPCHARACTER ch, DWORD bossIdx, const char* name, int value)
	{
		char sz[64];
		snprintf(sz, sizeof(sz), "dungeon.%u_%s", bossIdx, name);
		ch->SetQuestFlag(sz, value);
	}

	int Today()
	{
		time_t now = get_global_time();
		const tm* t = localtime(&now);
		return (1900 + t->tm_year) * 1000 + t->tm_yday;
	}

	int SecondsToMidnight()
	{
		time_t now = get_global_time();
		tm t = *localtime(&now);
		t.tm_mday += 1;
		t.tm_hour = t.tm_min = t.tm_sec = 0;
		t.tm_isdst = -1;
		const time_t next = mktime(&t);
		return next > now ? (int)(next - now) : 0;
	}

	int DailyUsed(LPCHARACTER ch, DWORD bossIdx)
	{
		return Flag(ch, bossIdx, "daily_day") == Today() ? Flag(ch, bossIdx, "daily") : 0;
	}

#ifdef ENABLE_DUNGEON_LIMIT_RENEWAL
	// Baerim daily limit (DUNGEON_DAILY_HELP_*):
	// - the PC (computer) limit comes first: every character of one computer shares it (table dungeon_pc_limit, all cores)
	// - weekly stack: the unused entries of a day are added to the next days when at least 1 run was completed that day;
	//   the stack is capped at the daily limit, spent once the daily limit is used up and reset every Monday 00:00
	std::unordered_map<DWORD, std::string> s_mapPcId;	// pid -> computer identity sent by the client (/dungeon_info pc_id)

	void PcTable();

	// the identity is sent after entering the game, but the entry into a dungeon instance is checked on arrival (often on
	// another core): it is kept in dungeon_pc_ident as well, so every core knows it before the client sends it again
	std::string PcId(LPCHARACTER ch)
	{
		const DWORD pid = ch->GetPlayerID();
		auto it = s_mapPcId.find(pid);
		if (it == s_mapPcId.end())
		{
			PcTable();
			std::string id;
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT pc_id FROM dungeon_pc_ident WHERE pid = %u", pid));
			if (pMsg->Get()->uiNumRows)
			{
				MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
				if (row[0])
					id = row[0];
			}
			it = s_mapPcId.emplace(pid, id).first;
		}
		if (!it->second.empty())
			return it->second;
		// a client without the identity: its address stands for the computer
		return ch->GetDesc() ? std::string("ip") + ch->GetDesc()->GetHostName() : std::string();
	}

	bool IsSafeId(const std::string& id)
	{
		if (id.empty() || id.size() > 40)
			return false;
		for (char c : id)
			if (!isalnum((unsigned char)c) && c != '.' && c != ':')
				return false;
		return true;
	}

	struct SPcUsed
	{
		int used;
		time_t until;
	};
	std::map<std::pair<std::string, DWORD>, SPcUsed> s_mapPcUsed;
	bool s_bPcTableReady = false;

	void PcTable()
	{
		if (s_bPcTableReady)
			return;
		s_bPcTableReady = true;
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"CREATE TABLE IF NOT EXISTS dungeon_pc_limit (pc_id VARCHAR(40) NOT NULL, boss INT UNSIGNED NOT NULL,"
			" day INT NOT NULL DEFAULT 0, used INT NOT NULL DEFAULT 0, PRIMARY KEY (pc_id, boss)) ENGINE=InnoDB"));
		std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery(
			"CREATE TABLE IF NOT EXISTS dungeon_pc_ident (pid INT UNSIGNED NOT NULL, pc_id VARCHAR(40) NOT NULL,"
			" PRIMARY KEY (pid)) ENGINE=InnoDB"));
	}

	// entries of the computer today; fresh = read the table (entering), else a 30 s cache (window display)
	int PcUsed(LPCHARACTER ch, DWORD bossIdx, bool fresh)
	{
		const std::string pc = PcId(ch);
		if (!IsSafeId(pc))
			return 0;

		SPcUsed& cache = s_mapPcUsed[std::make_pair(pc, bossIdx)];
		if (!fresh && cache.until > get_global_time())
			return cache.used;

		PcTable();
		cache.used = 0;
		cache.until = get_global_time() + 30;
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT day, used FROM dungeon_pc_limit WHERE pc_id = '%s' AND boss = %u", pc.c_str(), bossIdx));
		if (pMsg->Get()->uiNumRows)
		{
			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			int day = 0, used = 0;
			str_to_number(day, row[0]);
			str_to_number(used, row[1]);
			cache.used = day == Today() ? used : 0;
		}
		return cache.used;
	}

	void PcAdd(LPCHARACTER ch, DWORD bossIdx)
	{
		const std::string pc = PcId(ch);
		if (!IsSafeId(pc))
			return;
		PcTable();
		const int today = Today();
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"INSERT INTO dungeon_pc_limit (pc_id, boss, day, used) VALUES ('%s', %u, %d, 1)"
			" ON DUPLICATE KEY UPDATE used = IF(day = %d, used + 1, 1), day = %d", pc.c_str(), bossIdx, today, today, today));
		s_mapPcUsed.erase(std::make_pair(pc, bossIdx));
	}

	// Monday based week number
	int WeekId()
	{
		const time_t now = get_global_time();
		const tm t = *localtime(&now);
		const long day = (long)((now + t.tm_gmtoff) / 86400);
		return (int)((day + 3) / 7);	// 1970-01-01 was a Thursday
	}

	// a new day for this character and dungeon: carry the unused entries into the weekly stack, start the new count
	void RollDay(LPCHARACTER ch, DWORD bossIdx, int dailyLimit)
	{
		const int today = Today();
		if (Flag(ch, bossIdx, "daily_day") == today)
			return;

		const int week = WeekId();
		int stack = Flag(ch, bossIdx, "stack");
		if (Flag(ch, bossIdx, "stack_week") != week)
		{
			stack = 0;
			SetFlag(ch, bossIdx, "stack_week", week);
		}

		const int completed = Flag(ch, bossIdx, "completed");
		if (Flag(ch, bossIdx, "daily_day") && Flag(ch, bossIdx, "daily_week") == week && completed > Flag(ch, bossIdx, "daily_cstart"))
			stack = MIN(dailyLimit, stack + MAX(0, dailyLimit - Flag(ch, bossIdx, "daily")));

		SetFlag(ch, bossIdx, "stack", stack);
		SetFlag(ch, bossIdx, "daily_day", today);
		SetFlag(ch, bossIdx, "daily", 0);
		SetFlag(ch, bossIdx, "daily_cstart", completed);
		SetFlag(ch, bossIdx, "daily_week", week);
	}

	// RESULT_OK, RESULT_PC_LIMIT (checked first) or RESULT_DAILY_LIMIT
	int CheckDaily(LPCHARACTER ch, const SDungeonWindowConfig* cfg)
	{
		if (ch->IsGM() || cfg->dailyLimit <= 0)
			return RESULT_OK;
		RollDay(ch, cfg->bossIdx, cfg->dailyLimit);
		const int limit = cfg->dailyLimit + Flag(ch, cfg->bossIdx, "stack");
		if (PcUsed(ch, cfg->bossIdx, true) >= limit)
			return RESULT_PC_LIMIT;
		if (Flag(ch, cfg->bossIdx, "daily") >= limit)
			return RESULT_DAILY_LIMIT;
		return RESULT_OK;
	}

	// an entry over the daily limit is paid from the weekly stack
	void CountEntry(LPCHARACTER ch, const SDungeonWindowConfig* cfg)
	{
		const DWORD bossIdx = cfg->bossIdx;
		const int daily = Flag(ch, bossIdx, "daily");
		if (cfg->dailyLimit > 0 && daily >= cfg->dailyLimit && Flag(ch, bossIdx, "stack") > 0)
			SetFlag(ch, bossIdx, "stack", Flag(ch, bossIdx, "stack") - 1);
		SetFlag(ch, bossIdx, "daily", daily + 1);
		PcAdd(ch, bossIdx);
	}
#endif

	// server-wide run times, read from the players' fastest runs every 5 minutes
	struct STimeCache
	{
		int average;
		int best;
		time_t until;
	};
	std::map<DWORD, STimeCache> s_mapTimes;

	const STimeCache& ServerTimes(DWORD bossIdx)
	{
		STimeCache& cache = s_mapTimes[bossIdx];
		if (cache.until > get_global_time())
			return cache;

		cache.until = get_global_time() + 300;
		cache.average = cache.best = 0;
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT IFNULL(ROUND(AVG(lValue)), 0), IFNULL(MIN(lValue), 0) FROM quest"
			" WHERE szName = 'dungeon' AND szState = '%u_fastest' AND lValue > 0", bossIdx));
		if (pMsg->Get()->uiNumRows)
		{
			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			str_to_number(cache.average, row[0]);
			str_to_number(cache.best, row[1]);
		}
		return cache;
	}

	// boss dps: first hit time and damage of each player on each boss, until it dies
	struct SDpsTrack
	{
		DWORD firstHit;
		DWORD lastHit;
		long long damage;
	};
	std::unordered_map<DWORD, std::unordered_map<DWORD, SDpsTrack> > s_mapDps;

	void SendResult(LPCHARACTER ch, int result, DWORD bossIdx)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "DungeonWindowResult %d %u", result, bossIdx);
	}

	// ---------------------------------------------------------------- statistics window: ranking
	// Baerim "Zindan Istatistikleri" > Siralama: top 10 per dungeon, value (completed / fastest / boss dps) and class filter
	enum ERankType
	{
		RANK_COMPLETED = 0,
		RANK_FASTEST = 1,
		RANK_DPS = 2,
		RANK_MAX_NUM,
	};

	enum
	{
		RANK_RACE_ALL = 0,		// 1 warrior, 2 ninja, 3 sura, 4 shaman (player.job % 4 + 1)
		RANK_RACE_MAX_NUM = 5,
		RANK_ROWS = 10,
		RANK_CACHE_SECONDS = 300,
	};

	struct SRankRow
	{
		std::string name;
		int value;
		int level;
		int job;
	};

	struct SRankCache
	{
		std::vector<SRankRow> rows;
		time_t until = 0;
	};
	std::map<std::tuple<DWORD, int, int>, SRankCache> s_mapRank;

	const std::vector<SRankRow>& Ranking(DWORD bossIdx, int rankIdx, int race)
	{
		SRankCache& cache = s_mapRank[std::make_tuple(bossIdx, rankIdx, race)];
		if (cache.until > get_global_time())
			return cache.rows;

		cache.until = get_global_time() + RANK_CACHE_SECONDS;
		cache.rows.clear();

		static const char* s_flags[RANK_MAX_NUM] = { "completed", "fastest", "dps" };
		char raceFilter[64] = "";
		if (race != RANK_RACE_ALL)
			snprintf(raceFilter, sizeof(raceFilter), " AND p.job < 8 AND MOD(p.job, 4) = %d", race - 1);

		// one join instead of a player lookup per row
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT p.name, q.lValue, p.level, p.job FROM quest q INNER JOIN player p ON p.id = q.dwPID"
			" WHERE q.szName = 'dungeon' AND q.szState = '%u_%s' AND q.lValue > 0%s ORDER BY q.lValue %s LIMIT %d",
			bossIdx, s_flags[rankIdx], raceFilter, rankIdx == RANK_FASTEST ? "ASC" : "DESC", RANK_ROWS));
		if (!pMsg->Get()->pSQLResult)
			return cache.rows;

		MYSQL_ROW row;
		while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			SRankRow r;
			r.name = row[0] ? row[0] : "";
			r.value = r.level = r.job = 0;
			str_to_number(r.value, row[1]);
			str_to_number(r.level, row[2]);
			str_to_number(r.job, row[3]);
			cache.rows.push_back(r);
		}
		return cache.rows;
	}

	// ---------------------------------------------------------------- statistics window: missions
	// Baerim > Gorevler: 9 missions per dungeon (3x "faster than", 3x "boss dps", 3x "completed"), each claimable once.
	// Targets and rewards: locale/<country>/dungeon_mission.txt (boss  no  type  target  reward vnum  reward count)
	enum EMissionType
	{
		MISSION_TIME,
		MISSION_DPS,
		MISSION_COMPLETED,
	};

	enum EMissionResult
	{
		MISSION_RESULT_OK = 0,
		MISSION_RESULT_NOT_DONE = 1,
		MISSION_RESULT_CLAIMED = 2,
		MISSION_RESULT_INVENTORY = 3,
		MISSION_RESULT_UNKNOWN = 4,
	};

	enum
	{
		MISSION_MAX_NUM = 31,	// claimed missions are bits of dungeon.<boss>_mclaim
	};

	struct SMission
	{
		int no;
		int type;
		int target;
		DWORD rewardVnum;
		int rewardCount;
	};

	std::map<DWORD, std::vector<SMission> > s_mapMissions;
	bool s_bMissionsLoaded = false;

	void LoadMissions()
	{
		s_bMissionsLoaded = true;
		s_mapMissions.clear();

		const std::string path = LocaleService_GetBasePath() + "/dungeon_mission.txt";
		FILE* fp = fopen(path.c_str(), "r");
		if (!fp)
		{
			sys_err("DungeonWindow: cannot open %s", path.c_str());
			return;
		}

		char line[256];
		int lineNo = 0;
		while (fgets(line, sizeof(line), fp))
		{
			++lineNo;
			char* p = line;
			while (*p == ' ' || *p == '\t')
				++p;
			if (*p == '#' || *p == '\r' || *p == '\n' || *p == '\0')
				continue;

			DWORD boss = 0, vnum = 0;
			int no = 0, target = 0, count = 0;
			char type[32] = "";
			if (sscanf(p, "%u %d %31s %d %u %d", &boss, &no, type, &target, &vnum, &count) != 6 || no < 1 || no > MISSION_MAX_NUM)
			{
				sys_err("DungeonWindow: %s line %d is invalid", path.c_str(), lineNo);
				continue;
			}

			SMission m;
			m.no = no;
			m.target = target;
			m.rewardVnum = vnum;
			m.rewardCount = MINMAX(1, count, 200);
			if (!strcmp(type, "time"))
				m.type = MISSION_TIME;
			else if (!strcmp(type, "dps"))
				m.type = MISSION_DPS;
			else if (!strcmp(type, "completed"))
				m.type = MISSION_COMPLETED;
			else
			{
				sys_err("DungeonWindow: %s line %d: unknown type %s", path.c_str(), lineNo, type);
				continue;
			}

			if (!ITEM_MANAGER::instance().GetTable(vnum))
			{
				sys_err("DungeonWindow: %s line %d: reward item %u does not exist", path.c_str(), lineNo, vnum);
				continue;
			}
			s_mapMissions[boss].push_back(m);
		}
		fclose(fp);
	}

	const std::vector<SMission>* Missions(DWORD bossIdx)
	{
		if (!s_bMissionsLoaded)
			LoadMissions();
		const auto it = s_mapMissions.find(bossIdx);
		return it == s_mapMissions.end() ? nullptr : &it->second;
	}

	bool MissionDone(LPCHARACTER ch, DWORD bossIdx, const SMission& m)
	{
		switch (m.type)
		{
			case MISSION_TIME:
			{
				const int fastest = Flag(ch, bossIdx, "fastest");
				return fastest > 0 && fastest <= m.target;
			}
			case MISSION_DPS:
				return Flag(ch, bossIdx, "dps") >= m.target;
			case MISSION_COMPLETED:
				return Flag(ch, bossIdx, "completed") >= m.target;
		}
		return false;
	}

	// DungeonMissionData <boss> <fastest> <dps> <completed> <claimed mask> <no:type:target:vnum:count#... | ->
	void SendMissions(LPCHARACTER ch, DWORD bossIdx)
	{
		static const char s_types[] = { 't', 'd', 'c' };
		std::string list;
		if (const std::vector<SMission>* missions = Missions(bossIdx))
		{
			char sz[64];
			for (const SMission& m : *missions)
			{
				snprintf(sz, sizeof(sz), "%d:%c:%d:%u:%d#", m.no, s_types[m.type], m.target, m.rewardVnum, m.rewardCount);
				list += sz;
			}
		}
		if (list.empty())
			list = "-";

		ch->ChatPacket(CHAT_TYPE_COMMAND, "DungeonMissionData %u %d %d %d %d %s", bossIdx, Flag(ch, bossIdx, "fastest"),
			Flag(ch, bossIdx, "dps"), Flag(ch, bossIdx, "completed"), Flag(ch, bossIdx, "mclaim"), list.c_str());
	}

	void ClaimMission(LPCHARACTER ch, DWORD bossIdx, int no)
	{
		const std::vector<SMission>* missions = Missions(bossIdx);
		const SMission* mission = nullptr;
		if (missions)
			for (const SMission& m : *missions)
				if (m.no == no)
					mission = &m;

		int result = MISSION_RESULT_OK;
		const int claimed = Flag(ch, bossIdx, "mclaim");
		if (!mission)
			result = MISSION_RESULT_UNKNOWN;
		else if (claimed & (1 << no))
			result = MISSION_RESULT_CLAIMED;
		else if (!MissionDone(ch, bossIdx, *mission))
			result = MISSION_RESULT_NOT_DONE;
		else
		{
			// never drop the reward on the ground: refuse when there is no room
			const TItemTable* table = ITEM_MANAGER::instance().GetTable(mission->rewardVnum);
			if (!table || ch->GetEmptyInventory(table->bSize) == -1)
				result = MISSION_RESULT_INVENTORY;
			else
			{
				SetFlag(ch, bossIdx, "mclaim", claimed | (1 << no));
				ch->AutoGiveItem(mission->rewardVnum, mission->rewardCount);
			}
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, "DungeonMissionResult %d %u %d", result, bossIdx, no);
		SendMissions(ch, bossIdx);
	}

	// completion and fastest run are written by the Arena2 dungeon quests (Dungeon.Complete ->
	// d.increase_completed / d.set_fastest); here the players of the instance only get the new numbers
	struct FRefreshDungeon
	{
		DWORD bossIdx;
		FRefreshDungeon(DWORD boss) : bossIdx(boss) {}

		void operator () (LPENTITY ent)
		{
			if (!ent->IsType(ENTITY_CHARACTER))
				return;
			LPCHARACTER ch = static_cast<LPCHARACTER>(ent);
			if (ch->IsPC())
				ch->SendDungeonCooldown(bossIdx);
		}
	};
}

void DungeonWindow_OnEnterGame(LPCHARACTER ch)
{
	if (!ch || !ch->IsPC())
		return;

	const long mapIndex = ch->GetMapIndex();
	const SDungeonWindowConfig* cfg = FindByMap(mapIndex);
	if (!cfg || Flag(ch, cfg->bossIdx, "instance") == mapIndex)
		return;

	// a new instance: count the entry, start the run timer
	const DWORD bossIdx = cfg->bossIdx;
#ifdef ENABLE_DUNGEON_LIMIT_RENEWAL
	const int check = CheckDaily(ch, cfg);
	if (check != RESULT_OK)
	{
		SendResult(ch, check == RESULT_PC_LIMIT ? RESULT_PC_LIMIT : RESULT_DAILY_KICKED, bossIdx);
		ch->WarpSet(cfg->warpX, cfg->warpY);
		return;
	}
	CountEntry(ch, cfg);
	SetFlag(ch, bossIdx, "instance", mapIndex);
	SetFlag(ch, bossIdx, "enter", get_global_time());
#else
	if (Flag(ch, bossIdx, "daily_day") != Today())
	{
		SetFlag(ch, bossIdx, "daily_day", Today());
		SetFlag(ch, bossIdx, "daily", 0);
	}

	if (!ch->IsGM() && cfg->dailyLimit > 0 && Flag(ch, bossIdx, "daily") >= cfg->dailyLimit)
	{
		SendResult(ch, RESULT_DAILY_KICKED, bossIdx);
		ch->WarpSet(cfg->warpX, cfg->warpY);
		return;
	}

	SetFlag(ch, bossIdx, "daily", Flag(ch, bossIdx, "daily") + 1);
	SetFlag(ch, bossIdx, "instance", mapIndex);
	SetFlag(ch, bossIdx, "enter", get_global_time());
#endif
}

void DungeonWindow_OnBossHit(LPCHARACTER attacker, LPCHARACTER boss, int damage)
{
	if (!attacker || !boss || damage <= 0 || !attacker->IsPC() || boss->IsPC())
		return;
	if (m_mapDungeonList.find(boss->GetRaceNum()) == m_mapDungeonList.end())
		return;

	const DWORD now = get_dword_time();
	SDpsTrack& track = s_mapDps[(DWORD)boss->GetVID()][attacker->GetPlayerID()];
	if (!track.firstHit)
		track.firstHit = now;
	track.lastHit = now;
	track.damage += damage;
}

void DungeonWindow_OnBossDead(LPCHARACTER boss)
{
	if (!boss || boss->IsPC())
		return;

	const DWORD bossIdx = boss->GetRaceNum();
	if (m_mapDungeonList.find(bossIdx) == m_mapDungeonList.end())
		return;

	// best dps on the boss
	auto it = s_mapDps.find((DWORD)boss->GetVID());
	if (it != s_mapDps.end())
	{
		const DWORD now = get_dword_time();
		for (const auto& kv : it->second)
		{
			LPCHARACTER attacker = CHARACTER_MANAGER::instance().FindByPID(kv.first);
			if (!attacker)
				continue;
			const DWORD elapsed = MAX(1000, now - kv.second.firstHit);
			const long long dps = kv.second.damage * 1000 / elapsed;
			if (dps > Flag(attacker, bossIdx, "dps"))
				SetFlag(attacker, bossIdx, "dps", (int)MIN(dps, (long long)INT_MAX));
		}
		s_mapDps.erase(it);
	}

	// new dps for everyone in the instance
	const long mapIndex = boss->GetMapIndex();
	if (mapIndex < 10000)
		return;
	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(mapIndex);
	if (pMap)
	{
		FRefreshDungeon f(bossIdx);
		pMap->for_each(f);
	}

	// stale trackers (boss despawned without dying)
	if (s_mapDps.size() > 500)
	{
		const DWORD now = get_dword_time();
		for (auto dit = s_mapDps.begin(); dit != s_mapDps.end();)
		{
			DWORD last = 0;
			for (const auto& kv : dit->second)
				last = MAX(last, kv.second.lastHit);
			if (now - last > 60 * 60 * 1000)
				dit = s_mapDps.erase(dit);
			else
				++dit;
		}
	}
}

std::string DungeonWindow_ExtraInfo(LPCHARACTER ch, DWORD bossIdx)
{
	// |dps|daily used|daily limit|server average|server best|seconds to the daily reset
	const SDungeonWindowConfig* cfg = FindByBoss(bossIdx);
	const STimeCache& times = ServerTimes(bossIdx);
	char sz[128];
#ifdef ENABLE_DUNGEON_LIMIT_RENEWAL
	// ...|pc used today|weekly stack
	if (cfg && cfg->dailyLimit > 0)
		RollDay(ch, bossIdx, cfg->dailyLimit);
	snprintf(sz, sizeof(sz), "|%d|%d|%d|%d|%d|%d|%d|%d", Flag(ch, bossIdx, "dps"), DailyUsed(ch, bossIdx), cfg ? cfg->dailyLimit : 0,
		times.average, times.best, SecondsToMidnight(), PcUsed(ch, bossIdx, false), Flag(ch, bossIdx, "stack"));
#else
	snprintf(sz, sizeof(sz), "|%d|%d|%d|%d|%d|%d", Flag(ch, bossIdx, "dps"), DailyUsed(ch, bossIdx), cfg ? cfg->dailyLimit : 0,
		times.average, times.best, SecondsToMidnight());
#endif
	return sz;
}

void DungeonWindow_Command(LPCHARACTER ch, const std::vector<std::string>& args)
{
	if (!ch || args.size() < 2)
		return;

#ifdef ENABLE_DUNGEON_LIMIT_RENEWAL
	// /dungeon_info pc_id <32 hex>: computer identity of the client, sent on every game phase
	if (args[1] == "pc_id")
	{
		if (args.size() < 3 || args[2].size() != 32)
			return;
		for (char c : args[2])
			if (!isxdigit((unsigned char)c))
				return;
		std::string& known = s_mapPcId[ch->GetPlayerID()];
		if (known != args[2])
		{
			known = args[2];
			PcTable();
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
				"REPLACE INTO dungeon_pc_ident (pid, pc_id) VALUES (%u, '%s')", ch->GetPlayerID(), known.c_str()));
		}
		return;
	}
#endif

	if (args[1] == "mission_reload")
	{
		if (ch->IsGM())
		{
			LoadMissions();
			ch->ChatPacket(CHAT_TYPE_INFO, "dungeon_mission.txt: %u dungeons", (unsigned)s_mapMissions.size());
		}
		return;
	}

	if (args.size() < 3)
		return;

	DWORD bossIdx = 0;
	if (!str_to_number(bossIdx, args[2].c_str()))
		return;

	const auto itList = m_mapDungeonList.find(bossIdx);
	if (itList == m_mapDungeonList.end())
		return;

	// /dungeon_info stat_rank <boss> <0 completed | 1 fastest | 2 dps> <race 0-4>
	if (args[1] == "stat_rank")
	{
		int rankIdx = 0, race = 0;
		if (args.size() < 5 || !str_to_number(rankIdx, args[3].c_str()) || !str_to_number(race, args[4].c_str()))
			return;
		if (rankIdx < 0 || rankIdx >= RANK_MAX_NUM || race < 0 || race >= RANK_RACE_MAX_NUM)
			return;

		std::string list;
		char sz[96];
		for (const SRankRow& r : Ranking(bossIdx, rankIdx, race))
		{
			snprintf(sz, sizeof(sz), "%s|%d|%d|%d#", r.name.c_str(), r.value, r.level, r.job);
			list += sz;
		}
		if (list.empty())
			list = "-";
		// DungeonStatRank <boss> <rank> <race> <name|value|level|job#... | ->
		ch->ChatPacket(CHAT_TYPE_COMMAND, "DungeonStatRank %u %d %d %s", bossIdx, rankIdx, race, list.c_str());
		return;
	}

	// /dungeon_info missions <boss>
	if (args[1] == "missions")
	{
		SendMissions(ch, bossIdx);
		return;
	}

	// /dungeon_info mission_claim <boss> <no>
	if (args[1] == "mission_claim")
	{
		int no = 0;
		if (args.size() < 4 || !str_to_number(no, args[3].c_str()) || no < 1 || no > MISSION_MAX_NUM)
			return;
		if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsCubeOpen() || ch->GetSafebox())
			return;
		ClaimMission(ch, bossIdx, no);
		return;
	}

	if (args[1] == "test")
	{
		SendResult(ch, RESULT_TEST_UNAVAILABLE, bossIdx);
		return;
	}
	if (args[1] != "enter")
		return;

	const SDungeonWindowConfig* cfg = FindByBoss(bossIdx);
	if (!cfg || (!cfg->warpX && !cfg->warpY))
	{
		SendResult(ch, RESULT_NO_ENTRY, bossIdx);
		return;
	}

	if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsCubeOpen() || ch->GetSafebox() || ch->IsObserverMode()
		|| ch->GetMapIndex() >= 10000)
	{
		SendResult(ch, RESULT_BUSY, bossIdx);
		return;
	}

	const int level = ch->GetLevel();
	if (level < itList->second.first.first || level > itList->second.first.second)
	{
		SendResult(ch, RESULT_LEVEL, bossIdx);
		return;
	}

	if (ch->GetQuestFlagSpecial(strchr(itList->second.second.c_str(), '.') ? "%s" : "%s.cooldown", itList->second.second.c_str()) > get_global_time())
	{
		SendResult(ch, RESULT_COOLDOWN, bossIdx);
		return;
	}

#ifdef ENABLE_DUNGEON_LIMIT_RENEWAL
	const int check = CheckDaily(ch, cfg);
	if (check != RESULT_OK)
	{
		SendResult(ch, check, bossIdx);
		return;
	}
#else
	if (!ch->IsGM() && cfg->dailyLimit > 0 && DailyUsed(ch, bossIdx) >= cfg->dailyLimit)
	{
		SendResult(ch, RESULT_DAILY_LIMIT, bossIdx);
		return;
	}
#endif

	SendResult(ch, RESULT_OK, bossIdx);
	ch->WarpSet(cfg->warpX, cfg->warpY);
}
#endif
