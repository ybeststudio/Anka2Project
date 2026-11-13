#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_GLOBAL_RANKING
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "guild.h"
#include "db.h"
#include "global_ranking.h"

#include <algorithm>

// Weekly global ranking (reference Baerim uiRanking).
// Client: /global_rank _load <page> <time> <filter> <category>
//   page 0 players, 1 guilds, 2 members of the own guild; filter = race 1..4 (pages 0, 2) or empire 1..3 (page 1), 0 = all
// Server: LoadRankData <page> <time> <category> <seconds to the weekly reset> <own value> <rows | empty>
//   player row: index?name?guild?empire?value   guild row: index?name?level?empire?value   rows joined by '#'
// Table (player db): global_ranking (pid, category, week, value). A row of an older week counts as 0 and is
// overwritten by the first new score, so nothing has to be cleared at the weekly reset.

namespace
{
	enum
	{
		GLOBAL_RANK_PAGE_PLAYER	= 0,
		GLOBAL_RANK_PAGE_GUILD	= 1,
		GLOBAL_RANK_PAGE_MEMBER	= 2,
		GLOBAL_RANK_ROWS		= 10,
		GLOBAL_RANK_MIN_LEVEL	= 30,
		DPS_TRACK_TIMEOUT_MS	= 30 * 60 * 1000,
	};

	const int s_aMetin[] = { GLOBAL_RANK_METIN_30, GLOBAL_RANK_METIN_55, GLOBAL_RANK_METIN_75, GLOBAL_RANK_METIN_91, GLOBAL_RANK_METIN_106 };
	const int s_aMetinDamage[] = { GLOBAL_RANK_METIN_DAMAGE_30, GLOBAL_RANK_METIN_DAMAGE_55, GLOBAL_RANK_METIN_DAMAGE_75, GLOBAL_RANK_METIN_DAMAGE_91, GLOBAL_RANK_METIN_DAMAGE_106 };
	const int s_aMetinDps[] = { GLOBAL_RANK_METIN_DPS_30, GLOBAL_RANK_METIN_DPS_55, GLOBAL_RANK_METIN_DPS_75, GLOBAL_RANK_METIN_DPS_91, GLOBAL_RANK_METIN_DPS_106 };
	const int s_aBoss[] = { GLOBAL_RANK_BOSS_1, GLOBAL_RANK_BOSS_2, GLOBAL_RANK_BOSS_3, GLOBAL_RANK_BOSS_4, GLOBAL_RANK_BOSS_5 };

	// Baerim's level bands: 30-54, 55-74, 75-90, 91-105, 106-110
	int LevelBand(int level)
	{
		if (level < GLOBAL_RANK_MIN_LEVEL)
			return -1;
		if (level <= 54)
			return 0;
		if (level <= 74)
			return 1;
		if (level <= 90)
			return 2;
		if (level <= 105)
			return 3;
		return 4;
	}

	// Baerim's week turns on Monday 02:00 server time (UTC): their reset timer ran 2 hours behind a Monday 00:00 one
	const int RESET_OFFSET = 2 * 60 * 60;

	int CurrentWeek()
	{
		time_t now = get_global_time() - RESET_OFFSET;
		const tm* t = localtime(&now);
		const int yearDayOfMonday = t->tm_yday - (t->tm_wday + 6) % 7;
		return (1900 + t->tm_year) * 100 + (yearDayOfMonday + 7) / 7;
	}

	// seconds until next Monday 02:00 (server time)
	int SecondsToReset()
	{
		time_t now = get_global_time() - RESET_OFFSET;
		tm t = *localtime(&now);
		const int daysToMonday = 7 - (t.tm_wday + 6) % 7;
		t.tm_mday += daysToMonday;
		t.tm_hour = t.tm_min = t.tm_sec = 0;
		t.tm_isdst = -1;
		const time_t next = mktime(&t);
		return next > now ? (int)(next - now) : 0;
	}

	bool IsValidCategory(int category)
	{
		return category > 0 && category < GLOBAL_RANK_CATEGORY_MAX;
	}

	// highest value written this week per player and category: max categories only reach the db when beaten
	struct SMaxCache
	{
		int week;
		long long value;
	};
	std::unordered_map<DWORD, std::unordered_map<int, SMaxCache> > s_mapMaxCache;

	// metin dps: first hit time and damage of each player on each metin, until it dies
	struct SDpsTrack
	{
		DWORD firstHit;
		DWORD lastHit;
		long long damage;
	};
	std::unordered_map<DWORD, std::unordered_map<DWORD, SDpsTrack> > s_mapDps;

	void PruneDps(DWORD now)
	{
		for (auto it = s_mapDps.begin(); it != s_mapDps.end();)
		{
			DWORD last = 0;
			for (const auto& kv : it->second)
				last = MAX(last, kv.second.lastHit);

			if (now - last > DPS_TRACK_TIMEOUT_MS)
				it = s_mapDps.erase(it);
			else
				++it;
		}
	}

	// the client splits commands on whitespace: names with spaces (guild "L I T C H") travel with '^'
	std::string Field(const char* text)
	{
		std::string out(text ? text : "");
		std::replace(out.begin(), out.end(), ' ', '^');
		return out;
	}

	void SendRank(LPCHARACTER ch, int page, int timeIdx, int category, long long ownValue, const std::string& rows)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "LoadRankData %d %d %d %d %lld %s", page, timeIdx, category, SecondsToReset(), ownValue, rows.empty() ? "empty" : rows.c_str());
	}

	long long OwnPlayerValue(LPCHARACTER ch, int category, int week)
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT value FROM global_ranking WHERE pid = %u AND category = %d AND week = %d", ch->GetPlayerID(), category, week));
		if (pMsg->Get()->uiNumRows == 0)
			return 0;

		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		return row[0] ? strtoll(row[0], NULL, 10) : 0;
	}

	void LoadPlayers(LPCHARACTER ch, int page, int timeIdx, int filter, int category, int week)
	{
		char szWhere[128] = "";
		if (filter >= 1 && filter <= 4)
			snprintf(szWhere, sizeof(szWhere), " AND p.job %% 4 = %d", filter - 1);

		CGuild* guild = ch->GetGuild();
		if (page == GLOBAL_RANK_PAGE_MEMBER)
		{
			if (!guild)
			{
				SendRank(ch, page, timeIdx, category, 0, "");
				return;
			}

			const size_t len = strlen(szWhere);
			snprintf(szWhere + len, sizeof(szWhere) - len, " AND gm.guild_id = %u", guild->GetID());
		}

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT p.name, IFNULL(g.name, '-'), pi.empire, r.value FROM global_ranking r"
			" INNER JOIN player p ON p.id = r.pid"
			" INNER JOIN player_index pi ON pi.id = p.account_id"
			" LEFT JOIN guild_member gm ON gm.pid = r.pid"
			" LEFT JOIN guild g ON g.id = gm.guild_id"
			" WHERE r.category = %d AND r.week = %d AND r.value > 0%s"
			" AND p.name NOT IN (SELECT mName FROM common.gmlist)"
			" ORDER BY r.value DESC, p.level DESC, p.name ASC LIMIT %d",
			category, week, szWhere, GLOBAL_RANK_ROWS));

		std::string rows;
		MYSQL_ROW row;
		int index = 0;
		while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			char szRow[128];
			snprintf(szRow, sizeof(szRow), "%s%d?%s?%s?%s?%s", index ? "#" : "", index, Field(row[0]).c_str(), Field(row[1]).c_str(), row[2], row[3]);
			rows += szRow;
			++index;
		}

		SendRank(ch, page, timeIdx, category, OwnPlayerValue(ch, category, week), rows);
	}

	void LoadGuilds(LPCHARACTER ch, int page, int timeIdx, int filter, int category, int week)
	{
		char szWhere[64] = "";
		if (filter >= 1 && filter <= 3)
			snprintf(szWhere, sizeof(szWhere), " AND pi.empire = %d", filter);

		// a guild's score is the sum of its members' scores; its empire is the master's
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT g.name, g.level, pi.empire, SUM(r.value) AS score FROM global_ranking r"
			" INNER JOIN guild_member gm ON gm.pid = r.pid"
			" INNER JOIN guild g ON g.id = gm.guild_id"
			" INNER JOIN player m ON m.id = g.master"
			" INNER JOIN player_index pi ON pi.id = m.account_id"
			" WHERE r.category = %d AND r.week = %d%s"
			" GROUP BY g.id HAVING score > 0"
			" ORDER BY score DESC, g.level DESC, g.name ASC LIMIT %d",
			category, week, szWhere, GLOBAL_RANK_ROWS));

		std::string rows;
		MYSQL_ROW row;
		int index = 0;
		while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			char szRow[96];
			snprintf(szRow, sizeof(szRow), "%s%d?%s?%s?%s?%s", index ? "#" : "", index, Field(row[0]).c_str(), row[1] ? row[1] : "1", row[2], row[3]);
			rows += szRow;
			++index;
		}

		long long ownValue = 0;
		if (CGuild* guild = ch->GetGuild())
		{
			std::unique_ptr<SQLMsg> pOwn(DBManager::instance().DirectQuery(
				"SELECT IFNULL(SUM(r.value), 0) FROM global_ranking r INNER JOIN guild_member gm ON gm.pid = r.pid"
				" WHERE gm.guild_id = %u AND r.category = %d AND r.week = %d", guild->GetID(), category, week));
			if (pOwn->Get()->uiNumRows)
			{
				MYSQL_ROW own = mysql_fetch_row(pOwn->Get()->pSQLResult);
				ownValue = own[0] ? strtoll(own[0], NULL, 10) : 0;
			}
		}

		SendRank(ch, page, timeIdx, category, ownValue, rows);
	}
}

void GlobalRanking_Add(LPCHARACTER ch, int category, long long value)
{
	if (!ch || !ch->IsPC() || value <= 0 || !IsValidCategory(category))
		return;

	const int week = CurrentWeek();
	// MySQL applies the assignments left to right: value is computed against the old week
	DBManager::instance().Query(
		"INSERT INTO global_ranking (pid, category, week, value) VALUES (%u, %d, %d, %lld)"
		" ON DUPLICATE KEY UPDATE value = IF(week = VALUES(week), value + VALUES(value), VALUES(value)), week = VALUES(week)",
		ch->GetPlayerID(), category, week, value);
}

void GlobalRanking_Max(LPCHARACTER ch, int category, long long value)
{
	if (!ch || !ch->IsPC() || value <= 0 || !IsValidCategory(category))
		return;

	const int week = CurrentWeek();
	SMaxCache& cache = s_mapMaxCache[ch->GetPlayerID()][category];
	if (cache.week == week && cache.value >= value)
		return;

	cache.week = week;
	cache.value = value;
	DBManager::instance().Query(
		"INSERT INTO global_ranking (pid, category, week, value) VALUES (%u, %d, %d, %lld)"
		" ON DUPLICATE KEY UPDATE value = IF(week = VALUES(week), GREATEST(value, VALUES(value)), VALUES(value)), week = VALUES(week)",
		ch->GetPlayerID(), category, week, value);
}

void GlobalRanking_OnHit(LPCHARACTER attacker, LPCHARACTER victim, int damage)
{
	if (!attacker || !victim || damage <= 0 || !attacker->IsPC() || !victim->IsStone())
		return;

	const int band = LevelBand(victim->GetLevel());
	if (band < 0)
		return;

	GlobalRanking_Max(attacker, s_aMetinDamage[band], damage);

	const DWORD now = get_dword_time();
	SDpsTrack& track = s_mapDps[(DWORD)victim->GetVID()][attacker->GetPlayerID()];
	if (!track.firstHit)
		track.firstHit = now;
	track.lastHit = now;
	track.damage += damage;
}

void GlobalRanking_OnDead(LPCHARACTER victim, LPCHARACTER killer)
{
	if (!victim)
		return;

	if (victim->IsStone())
	{
		const int band = LevelBand(victim->GetLevel());
		if (band >= 0 && killer && killer->IsPC())
			GlobalRanking_Add(killer, s_aMetin[band]);

		auto it = s_mapDps.find((DWORD)victim->GetVID());
		if (it != s_mapDps.end())
		{
			const DWORD now = get_dword_time();
			if (band >= 0)
			{
				for (const auto& kv : it->second)
				{
					LPCHARACTER attacker = CHARACTER_MANAGER::instance().FindByPID(kv.first);
					if (!attacker)
						continue;

					// under a second counts as a second
					const DWORD elapsed = MAX(1000, now - kv.second.firstHit);
					GlobalRanking_Max(attacker, s_aMetinDps[band], kv.second.damage * 1000 / elapsed);
				}
			}
			s_mapDps.erase(it);
		}

		if (s_mapDps.size() > 2000)
			PruneDps(get_dword_time());
		return;
	}

	if (!killer || !killer->IsPC())
		return;

	if (victim->IsPC())
	{
		GlobalRanking_Add(killer, GLOBAL_RANK_PVP_KILLS);
		return;
	}

	// boss stages follow the metin level bands (bosses under level 30 count as stage I)
	if (victim->IsMonster() && victim->GetMobRank() >= MOB_RANK_BOSS)
	{
		const int band = MAX(0, LevelBand(victim->GetLevel()));
		GlobalRanking_Add(killer, s_aBoss[band]);
	}
}

void GlobalRanking_Command(LPCHARACTER ch, const char* argument)
{
	if (!ch || !ch->GetDesc())
		return;

	char arg1[64], arg2[16], arg3[16], arg4[16], arg5[16];
	argument = one_argument(argument, arg1, sizeof(arg1));
	argument = one_argument(argument, arg2, sizeof(arg2));
	argument = one_argument(argument, arg3, sizeof(arg3));
	argument = one_argument(argument, arg4, sizeof(arg4));
	one_argument(argument, arg5, sizeof(arg5));

	if (strcmp(arg1, "_load") || !*arg5)
		return;

	int page = 0, timeIdx = 0, filter = 0, category = 0;
	str_to_number(page, arg2);
	str_to_number(timeIdx, arg3);
	str_to_number(filter, arg4);
	str_to_number(category, arg5);

	if (page < GLOBAL_RANK_PAGE_PLAYER || page > GLOBAL_RANK_PAGE_MEMBER || !IsValidCategory(category) || filter < 0 || filter > 4)
		return;

	const int week = CurrentWeek();
	if (page == GLOBAL_RANK_PAGE_GUILD)
		LoadGuilds(ch, page, timeIdx, filter, category, week);
	else
		LoadPlayers(ch, page, timeIdx, filter, category, week);
}
#endif
