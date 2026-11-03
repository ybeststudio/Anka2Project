#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_GUILD_SKILL_TREE
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "guild.h"
#include "guild_manager.h"
#include "db.h"
#include "guild_donation.h"
#include "guild_skill.h"
#include "guild_shop.h"

// Guild skill tree (reference Baerim locale/common/config_json/guild.json "skill", uiGuild skill page).
// Table (player db): guild_skill (exp, points, ten levels, activation end). Energy is the guild SP.

namespace
{
	enum EGuildSkillResult
	{
		GUILD_SKILL_NO_AUTH			= 1,
		GUILD_SKILL_NO_ITEM			= 2,
		GUILD_SKILL_DONATED			= 3,
		GUILD_SKILL_NO_POINT		= 4,
		GUILD_SKILL_NO_GOLD			= 5,
		GUILD_SKILL_MAX_LEVEL		= 6,
		GUILD_SKILL_UPGRADED		= 7,
		GUILD_SKILL_NO_ACTIVE		= 8,
		GUILD_SKILL_NO_ENERGY		= 9,
		GUILD_SKILL_ACTIVATED		= 10,
		GUILD_SKILL_ENERGY_FULL		= 11,
		GUILD_SKILL_NO_ENERGY_ITEM	= 12,
		GUILD_SKILL_RECHARGED		= 13,
		GUILD_SKILL_ALREADY_ACTIVE	= 14,
	};

	enum
	{
		GUILD_SKILL_ACTIVE_COUNT	= 5,
		GUILD_SKILL_MAX_LEVEL_VALUE	= 10,
		GUILD_SKILL_EXP_MAX			= 100000,
		GUILD_SKILL_SUCCESS_RATE	= 30,
		GUILD_SKILL_ENERGY_COST		= 50,	// 3 hours; 12 and 24 hours cost their multiple
		GUILD_SKILL_ITEM_SUCCESS	= 65110,
		GUILD_SKILL_ITEM_CRITICAL	= 65111,
		GUILD_SKILL_ITEM_ENERGY		= 65112,
		GUILD_SKILL_CRITICAL_RATE	= 50,
	};

	const DWORD s_adwDonationItem[3] = { 90010, 90011, 90012 };
	const int s_aiDonationExp[3] = { 1000, 2500, 5000 };
	const int s_aiGoldCost[GUILD_SKILL_MAX_LEVEL_VALUE] = { 750, 1000, 2000, 4000, 7000, 10000, 13000, 16000, 20000, 25000 };
	const int s_aiPointCost[GUILD_SKILL_MAX_LEVEL_VALUE] = { 1, 2, 3, 5, 7, 9, 11, 15, 30, 50 };
	const int s_aiValues[GUILD_SKILL_TREE_COUNT][GUILD_SKILL_MAX_LEVEL_VALUE + 1] =
	{
		{ 0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 15 },
		{ 0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 15 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
		{ 0, 3, 5, 7, 10, 15, 20, 25, 30, 40, 50 },
		{ 0, 2, 4, 6, 8, 10, 12, 15, 20, 25, 30 },
		{ 0, 2, 4, 6, 9, 12, 15, 18, 21, 25, 30 },
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
		{ 0, 3, 5, 7, 10, 15, 20, 25, 30, 40, 50 },
		{ 0, 3, 5, 7, 10, 15, 20, 25, 30, 40, 50 },
		{ 0, 1, 2, 3, 4, 5, 6, 8, 10, 12, 15 },
	};

	struct SGuildSkillRow
	{
		int exp;
		int points;
		int activeUntil;
		int aiLevel[GUILD_SKILL_TREE_COUNT];

		SGuildSkillRow() : exp(0), points(0), activeUntil(0) { memset(aiLevel, 0, sizeof(aiLevel)); }
	};

	SGuildSkillRow LoadRow(DWORD guildID)
	{
		SGuildSkillRow data;
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT exp, points, active_until, lv0, lv1, lv2, lv3, lv4, lv5, lv6, lv7, lv8, lv9 FROM guild_skill WHERE guild_id = %u", guildID));
		if (pMsg->Get()->uiNumRows == 0)
			return data;
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(data.exp, row[0]);
		str_to_number(data.points, row[1]);
		str_to_number(data.activeUntil, row[2]);
		for (int i = 0; i < GUILD_SKILL_TREE_COUNT; ++i)
			str_to_number(data.aiLevel[i], row[3 + i]);
		return data;
	}

	void SendResult(LPCHARACTER ch, int result, int value = 0)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildSkillResult %d %d", result, value);
	}

	void SendInfo(LPCHARACTER ch, CGuild* guild)
	{
		const SGuildSkillRow data = LoadRow(guild->GetID());
		char levels[64] = "";
		int len = 0;
		for (int i = 0; i < GUILD_SKILL_TREE_COUNT; ++i)
			len += snprintf(levels + len, sizeof(levels) - len, " %d", data.aiLevel[i]);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildSkillInfo %d %d %d %d %d%s", data.exp, data.points, guild->GetSP(), guild->GetMaxSP(),
			MAX(0, data.activeUntil - get_global_time()), levels);
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

	bool HasAuth(LPCHARACTER ch, CGuild* guild, int auth)
	{
		const TGuildMember* member = guild->GetMember(ch->GetPlayerID());
		return member && (guild->GetMasterPID() == ch->GetPlayerID() || guild->HasGradeAuth(member->grade, auth));
	}

	void Donate(LPCHARACTER ch, CGuild* guild, int slot, int useSuccess, int useCritical)
	{
		if (slot < 0 || slot >= 3)
			return;

		const DWORD vnum = s_adwDonationItem[slot];
		const int count = ch->CountSpecifyItem(vnum);
		if (count < 1)
		{
			SendResult(ch, GUILD_SKILL_NO_ITEM);
			return;
		}

		const int successItems = useSuccess ? MIN(count, ch->CountSpecifyItem(GUILD_SKILL_ITEM_SUCCESS)) : 0;
		const int criticalItems = useCritical ? MIN(count, ch->CountSpecifyItem(GUILD_SKILL_ITEM_CRITICAL)) : 0;

		int gained = 0;
		for (int i = 0; i < count; ++i)
		{
			if (i >= successItems && number(1, 100) > GUILD_SKILL_SUCCESS_RATE)
				continue;
			int exp = s_aiDonationExp[slot];
			if (i < criticalItems && number(1, 100) <= GUILD_SKILL_CRITICAL_RATE)
				exp = exp * 3 / 2;
			gained += exp;
		}

		ch->RemoveSpecifyItem(vnum, count);
		if (successItems)
			ch->RemoveSpecifyItem(GUILD_SKILL_ITEM_SUCCESS, successItems);
		if (criticalItems)
			ch->RemoveSpecifyItem(GUILD_SKILL_ITEM_CRITICAL, criticalItems);

		std::unique_ptr<SQLMsg> pInsert(DBManager::instance().DirectQuery(
			"INSERT INTO guild_skill (guild_id, exp) VALUES (%u, %d) ON DUPLICATE KEY UPDATE exp = exp + %d", guild->GetID(), gained, gained));
		std::unique_ptr<SQLMsg> pPoints(DBManager::instance().DirectQuery(
			"UPDATE guild_skill SET points = points + exp DIV %d, exp = exp MOD %d WHERE guild_id = %u",
			GUILD_SKILL_EXP_MAX, GUILD_SKILL_EXP_MAX, guild->GetID()));

		GuildLog_Add(guild, GUILD_LOG_DONATION, ch->GetName(), (int)vnum, count, gained);
		SendResult(ch, GUILD_SKILL_DONATED, gained);
		RefreshGuild(guild);
		sys_log(0, "GUILD_SKILL: %s donated %d of %u to %s for %d exp", ch->GetName(), count, vnum, guild->GetName(), gained);
	}

	void Upgrade(LPCHARACTER ch, CGuild* guild, int skill)
	{
		if (skill < 0 || skill >= GUILD_SKILL_TREE_COUNT)
			return;
		if (!HasAuth(ch, guild, GUILD_AUTH_UP_SKILL))
		{
			SendResult(ch, GUILD_SKILL_NO_AUTH);
			return;
		}

		const SGuildSkillRow data = LoadRow(guild->GetID());
		const int level = data.aiLevel[skill];
		if (level >= GUILD_SKILL_MAX_LEVEL_VALUE)
		{
			SendResult(ch, GUILD_SKILL_MAX_LEVEL);
			return;
		}
		if (data.points < s_aiPointCost[level])
		{
			SendResult(ch, GUILD_SKILL_NO_POINT);
			return;
		}
		if (!GuildDonation_TakeGold(guild, s_aiGoldCost[level]))
		{
			SendResult(ch, GUILD_SKILL_NO_GOLD);
			return;
		}

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"UPDATE guild_skill SET points = points - %d, lv%d = lv%d + 1 WHERE guild_id = %u",
			s_aiPointCost[level], skill, skill, guild->GetID()));

		GuildLog_Add(guild, GUILD_LOG_SKILL, ch->GetName(), skill, level + 1, 0);
		SendResult(ch, GUILD_SKILL_UPGRADED);
		RefreshGuild(guild);
	}

	void Activate(LPCHARACTER ch, CGuild* guild, int hours)
	{
		if (hours != 3 && hours != 12 && hours != 24)
			return;
		if (!HasAuth(ch, guild, GUILD_AUTH_USE_SKILL))
		{
			SendResult(ch, GUILD_SKILL_NO_AUTH);
			return;
		}

		const SGuildSkillRow data = LoadRow(guild->GetID());
		int hasSkill = 0;
		for (int i = 0; i < GUILD_SKILL_ACTIVE_COUNT; ++i)
			hasSkill += data.aiLevel[i];
		if (!hasSkill)
		{
			SendResult(ch, GUILD_SKILL_NO_ACTIVE);
			return;
		}
		if (data.activeUntil > get_global_time())
		{
			SendResult(ch, GUILD_SKILL_ALREADY_ACTIVE);
			return;
		}

		int cost = GUILD_SKILL_ENERGY_COST * hours / 3;
		cost -= cost * s_aiValues[GUILD_SKILL_TREE_ENERGY][data.aiLevel[GUILD_SKILL_TREE_ENERGY]] / 100;
		if (guild->GetSP() < cost)
		{
			SendResult(ch, GUILD_SKILL_NO_ENERGY);
			return;
		}

		guild->GuildPointChange(POINT_SP, -cost, true);
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"UPDATE guild_skill SET active_until = %d WHERE guild_id = %u", get_global_time() + hours * 3600, guild->GetID()));
		GuildLog_Add(guild, GUILD_LOG_SKILL, ch->GetName(), -1, hours, 0);
		SendResult(ch, GUILD_SKILL_ACTIVATED);
		RefreshGuild(guild);
	}

	void Recharge(LPCHARACTER ch, CGuild* guild)
	{
		if (guild->GetSP() >= guild->GetMaxSP())
		{
			SendResult(ch, GUILD_SKILL_ENERGY_FULL);
			return;
		}
		if (ch->CountSpecifyItem(GUILD_SKILL_ITEM_ENERGY) < 1)
		{
			SendResult(ch, GUILD_SKILL_NO_ENERGY_ITEM);
			return;
		}

		ch->RemoveSpecifyItem(GUILD_SKILL_ITEM_ENERGY, 1);
		guild->GuildPointChange(POINT_SP, guild->GetMaxSP(), true);
		SendResult(ch, GUILD_SKILL_RECHARGED);
		RefreshGuild(guild);
	}
}

void GuildSkill_Command(LPCHARACTER ch, const char* argument)
{
	CGuild* guild = ch->GetGuild();
	if (!guild)
		return;

	char action[32], arg1[32], arg2[32], arg3[32];
	const char* rest = one_argument(argument, action, sizeof(action));
	rest = one_argument(rest, arg1, sizeof(arg1));
	rest = one_argument(rest, arg2, sizeof(arg2));
	one_argument(rest, arg3, sizeof(arg3));

	int value1 = 0, value2 = 0, value3 = 0;
	str_to_number(value1, arg1);
	str_to_number(value2, arg2);
	str_to_number(value3, arg3);

	if (!strcmp(action, "info"))
		SendInfo(ch, guild);
	else if (!strcmp(action, "donate"))
		Donate(ch, guild, value1, value2, value3);
	else if (!strcmp(action, "up"))
		Upgrade(ch, guild, value1);
	else if (!strcmp(action, "activate"))
		Activate(ch, guild, value1);
	else if (!strcmp(action, "recharge"))
		Recharge(ch, guild);
}

int GuildSkill_GetValue(CGuild* guild, int skill)
{
	if (!guild || skill < 0 || skill >= GUILD_SKILL_TREE_COUNT)
		return 0;

	const SGuildSkillRow data = LoadRow(guild->GetID());
	if (skill < GUILD_SKILL_ACTIVE_COUNT && data.activeUntil <= get_global_time())
		return 0;
	return s_aiValues[skill][MINMAX(0, data.aiLevel[skill], GUILD_SKILL_MAX_LEVEL_VALUE)];
}
#endif
