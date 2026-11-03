#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_GUILD_QUEST
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "guild.h"
#include "guild_manager.h"
#include "db.h"
#include "guild_donation.h"
#include "guild_quest.h"
#ifdef ENABLE_GUILD_SKILL_TREE
#include "guild_skill.h"
#endif

// Guild quests (reference Baerim locale/common/config_json/guild.json "quest", uiGuild quest page).
// Tables (player db): guild_quest (one row per guild: running quest, weekly usage), guild_quest_member (score of the running quest).

namespace
{
	enum EGuildQuestResult
	{
		GUILD_QUEST_STARTED			= 0,
		GUILD_QUEST_NO_AUTH			= 1,
		GUILD_QUEST_ALREADY_ACTIVE	= 2,
		GUILD_QUEST_NO_WEEKLY		= 3,
		GUILD_QUEST_NOT_FINISHED	= 4,
		GUILD_QUEST_REWARDED		= 5,
		GUILD_QUEST_CANCELLED		= 6,
		GUILD_QUEST_COMPLETED		= 7,
	};

	enum
	{
		GUILD_QUEST_TYPE_COUNT	= 4,
		GUILD_QUEST_LEVEL_COUNT	= 3,
		GUILD_QUEST_WEEKLY		= 4,
		GUILD_QUEST_DURATION	= 3600,
		GUILD_QUEST_RANK_ROWS	= 8,
	};

	const int s_aObjective[GUILD_QUEST_LEVEL_COUNT][GUILD_QUEST_TYPE_COUNT] =
	{
		{ 500, 15, 20, 250 },
		{ 1500, 25, 30, 500 },
		{ 4000, 40, 50, 1000 },
	};
	const int s_aRewardExp[GUILD_QUEST_LEVEL_COUNT] = { 300000, 375000, 450000 };
	const int s_aRewardGold[GUILD_QUEST_LEVEL_COUNT] = { 140, 180, 225 };
	const int s_aSilverThreshold[GUILD_QUEST_TYPE_COUNT] = { 50, 1, 1, 10 };
	const int s_aSilverAmount[GUILD_QUEST_TYPE_COUNT] = { 3, 6, 6, 3 };

	int CurrentWeek()
	{
		time_t now = get_global_time();
		const tm* t = localtime(&now);
		const int yearDayOfMonday = t->tm_yday - (t->tm_wday + 6) % 7;
		return (1900 + t->tm_year) * 100 + (yearDayOfMonday + 7) / 7;
	}

	bool HasQuestAuth(LPCHARACTER ch, CGuild* guild)
	{
		const TGuildMember* member = guild->GetMember(ch->GetPlayerID());
		return member && (guild->GetMasterPID() == ch->GetPlayerID() || guild->HasGradeAuth(member->grade, GUILD_AUTH_QUEST));
	}

	void SendResult(LPCHARACTER ch, int result)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildQuestResult %d", result);
	}

	struct SGuildQuestRow
	{
		int aiUsed[GUILD_QUEST_TYPE_COUNT];
		int type;
		int level;
		int cur;
		int target;
		int deadline;

		SGuildQuestRow() : type(0), level(0), cur(0), target(0), deadline(0) { memset(aiUsed, 0, sizeof(aiUsed)); }

		bool IsFinished() const { return type != 0 && target > 0 && cur >= target; }
		bool IsRunning() const { return type != 0 && (IsFinished() || deadline > get_global_time()); }
	};

	SGuildQuestRow LoadRow(DWORD guildID)
	{
		SGuildQuestRow data;
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT week, used1, used2, used3, used4, type, level, cur, target, deadline FROM guild_quest WHERE guild_id = %u", guildID));
		if (pMsg->Get()->uiNumRows == 0)
			return data;

		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		int week = 0;
		str_to_number(week, row[0]);
		if (week == CurrentWeek())
		{
			for (int i = 0; i < GUILD_QUEST_TYPE_COUNT; ++i)
				str_to_number(data.aiUsed[i], row[1 + i]);
		}
		str_to_number(data.type, row[5]);
		str_to_number(data.level, row[6]);
		str_to_number(data.cur, row[7]);
		str_to_number(data.target, row[8]);
		str_to_number(data.deadline, row[9]);
		if (!data.IsRunning())
			data.type = 0;
		return data;
	}

	void SendInfo(LPCHARACTER ch, CGuild* guild)
	{
		const SGuildQuestRow data = LoadRow(guild->GetID());
		const int left = data.IsRunning() ? MAX(0, data.deadline - get_global_time()) : 0;
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildQuestInfo %d %d %d %d %d %d %d %d %d",
			data.aiUsed[0], data.aiUsed[1], data.aiUsed[2], data.aiUsed[3], data.type, data.level, data.cur, data.target, left);
	}

	struct FRefreshGuild
	{
		CGuild* m_guild;

		FRefreshGuild(CGuild* guild) : m_guild(guild) {}

		void operator()(LPCHARACTER ch)
		{
			if (ch->GetGuild() == m_guild)
				SendInfo(ch, m_guild);
		}
	};

	void RefreshGuild(CGuild* guild)
	{
		FRefreshGuild f(guild);
		CHARACTER_MANAGER::instance().for_each_pc(f);
	}

	struct FSendProgress
	{
		CGuild*	m_guild;
		int		m_cur;
		int		m_target;

		FSendProgress(CGuild* guild, int cur, int target) : m_guild(guild), m_cur(cur), m_target(target) {}

		void operator()(LPCHARACTER ch)
		{
			if (ch->GetGuild() == m_guild)
				ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildQuestProgress %d %d", m_cur, m_target);
		}
	};

	void Start(LPCHARACTER ch, CGuild* guild, int type, int level)
	{
		if (type < 1 || type > GUILD_QUEST_TYPE_COUNT || level < 1 || level > GUILD_QUEST_LEVEL_COUNT)
			return;
		if (!HasQuestAuth(ch, guild))
		{
			SendResult(ch, GUILD_QUEST_NO_AUTH);
			return;
		}

		SGuildQuestRow data = LoadRow(guild->GetID());
		if (data.IsRunning())
		{
			SendResult(ch, GUILD_QUEST_ALREADY_ACTIVE);
			return;
		}
		int weekly = GUILD_QUEST_WEEKLY;
#ifdef ENABLE_GUILD_SKILL_TREE
		weekly += GuildSkill_GetValue(guild, GUILD_SKILL_TREE_QUEST);
#endif
		if (data.aiUsed[type - 1] >= weekly)
		{
			SendResult(ch, GUILD_QUEST_NO_WEEKLY);
			return;
		}

		data.aiUsed[type - 1] += 1;
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"REPLACE INTO guild_quest (guild_id, week, used1, used2, used3, used4, type, level, cur, target, deadline) "
			"VALUES (%u, %d, %d, %d, %d, %d, %d, %d, 0, %d, %d)",
			guild->GetID(), CurrentWeek(), data.aiUsed[0], data.aiUsed[1], data.aiUsed[2], data.aiUsed[3],
			type, level, s_aObjective[level - 1][type - 1], get_global_time() + GUILD_QUEST_DURATION));
		std::unique_ptr<SQLMsg> pScore(DBManager::instance().DirectQuery("DELETE FROM guild_quest_member WHERE guild_id = %u", guild->GetID()));

		SendResult(ch, GUILD_QUEST_STARTED);
		RefreshGuild(guild);
		sys_log(0, "GUILD_QUEST: %s started type %d level %d for %s", ch->GetName(), type, level, guild->GetName());
	}

	void Abort(LPCHARACTER ch, CGuild* guild)
	{
		if (!HasQuestAuth(ch, guild))
		{
			SendResult(ch, GUILD_QUEST_NO_AUTH);
			return;
		}

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"UPDATE guild_quest SET type = 0, cur = 0, target = 0, deadline = 0 WHERE guild_id = %u AND type <> 0 AND cur < target", guild->GetID()));
		if (pMsg->Get()->uiAffectedRows)
		{
			SendResult(ch, GUILD_QUEST_CANCELLED);
			RefreshGuild(guild);
		}
	}

	void SendRank(LPCHARACTER ch, CGuild* guild)
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT p.name, m.score FROM guild_quest_member m JOIN player p ON p.id = m.pid "
			"WHERE m.guild_id = %u ORDER BY m.score DESC LIMIT %d", guild->GetID(), GUILD_QUEST_RANK_ROWS));

		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildQuestRankBegin");
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildQuestRankRow %s %s", row[0], row[1]);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildQuestRankEnd");
	}
}

void GuildQuest_Command(LPCHARACTER ch, const char* argument)
{
	CGuild* guild = ch->GetGuild();
	if (!guild)
		return;

	char action[32], arg1[32], arg2[32];
	const char* rest = one_argument(argument, action, sizeof(action));
	rest = one_argument(rest, arg1, sizeof(arg1));
	one_argument(rest, arg2, sizeof(arg2));

	if (!strcmp(action, "info"))
		SendInfo(ch, guild);
	else if (!strcmp(action, "rank"))
		SendRank(ch, guild);
	else if (!strcmp(action, "abort"))
		Abort(ch, guild);
	else if (!strcmp(action, "start"))
	{
		int type = 0, level = 0;
		str_to_number(type, arg1);
		str_to_number(level, arg2);
		Start(ch, guild, type, level);
	}
}

void GuildQuest_Reward(LPCHARACTER ch)
{
	CGuild* guild = ch->GetGuild();
	if (!guild)
		return;
	if (!HasQuestAuth(ch, guild))
	{
		SendResult(ch, GUILD_QUEST_NO_AUTH);
		return;
	}

	const SGuildQuestRow data = LoadRow(guild->GetID());
	if (!data.IsFinished() || data.level < 1 || data.level > GUILD_QUEST_LEVEL_COUNT)
	{
		SendResult(ch, GUILD_QUEST_NOT_FINISHED);
		return;
	}

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
		"UPDATE guild_quest SET type = 0, cur = 0, target = 0, deadline = 0 WHERE guild_id = %u AND type <> 0 AND target > 0 AND cur >= target", guild->GetID()));
	if (pMsg->Get()->uiAffectedRows == 0)
		return;

	guild->GuildPointChange(POINT_EXP, s_aRewardExp[data.level - 1], true);
	GuildDonation_AddGold(guild, s_aRewardGold[data.level - 1]);
	SendResult(ch, GUILD_QUEST_REWARDED);
	RefreshGuild(guild);
	sys_log(0, "GUILD_QUEST: %s claimed level %d reward for %s", ch->GetName(), data.level, guild->GetName());
}

void GuildQuest_OnLogin(LPCHARACTER ch)
{
	if (ch->GetGuild())
		SendInfo(ch, ch->GetGuild());
}

void GuildQuest_AddProgress(LPCHARACTER ch, int type)
{
	if (!ch || !ch->IsPC() || type < 1 || type > GUILD_QUEST_TYPE_COUNT)
		return;
	CGuild* guild = ch->GetGuild();
	if (!guild)
		return;

	std::unique_ptr<SQLMsg> pUpdate(DBManager::instance().DirectQuery(
		"UPDATE guild_quest SET cur = cur + 1 WHERE guild_id = %u AND type = %d AND deadline > %d AND cur < target",
		guild->GetID(), type, get_global_time()));
	if (pUpdate->Get()->uiAffectedRows == 0)
		return;

	std::unique_ptr<SQLMsg> pScore(DBManager::instance().DirectQuery(
		"INSERT INTO guild_quest_member (guild_id, pid, score) VALUES (%u, %u, 1) ON DUPLICATE KEY UPDATE score = score + 1",
		guild->GetID(), ch->GetPlayerID()));

	std::unique_ptr<SQLMsg> pState(DBManager::instance().DirectQuery(
		"SELECT q.cur, q.target, m.score FROM guild_quest q JOIN guild_quest_member m ON m.guild_id = q.guild_id AND m.pid = %u WHERE q.guild_id = %u",
		ch->GetPlayerID(), guild->GetID()));
	if (pState->Get()->uiNumRows == 0)
		return;

	MYSQL_ROW row = mysql_fetch_row(pState->Get()->pSQLResult);
	int cur = 0, target = 0, score = 0;
	str_to_number(cur, row[0]);
	str_to_number(target, row[1]);
	str_to_number(score, row[2]);

	if (score % s_aSilverThreshold[type - 1] == 0)
		GuildDonation_AddSilver(ch, s_aSilverAmount[type - 1]);

	FSendProgress progress(guild, cur, target);
	CHARACTER_MANAGER::instance().for_each_pc(progress);

	if (cur >= target)
	{
		SendResult(ch, GUILD_QUEST_COMPLETED);
		RefreshGuild(guild);
	}
}
#endif
