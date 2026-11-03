#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_GUILD_APPLICATION
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "guild.h"
#include "guild_manager.h"
#include "db.h"
#include "questmanager.h"
#include "locale.hpp"
#include "guild_application.h"

// Guild application (reference Baerim uiGuildApplication / ServerCommand_Guild_JoinNotify).
// Tables (player db): guild_application_setting (per guild settings), guild_application (pending applications).

namespace
{
	enum EGuildAppStatus
	{
		GUILD_APP_STATUS_OPEN		= 0,	// reference GUILD_TYPE_DIC: 0 Açık, 1 Kapalı, 2 Onay Gerekli
		GUILD_APP_STATUS_CLOSED		= 1,
		GUILD_APP_STATUS_APPROVAL	= 2,
	};

	struct SGuildAppInfo
	{
		BYTE		empire;
		BYTE		status;
		BYTE		lang;
		int			min_level;
		std::string	desc;

		SGuildAppInfo() : empire(0), status(GUILD_APP_STATUS_OPEN), lang(0), min_level(1) {}
	};

	std::map<DWORD, SGuildAppInfo>	s_info;			// guild id -> settings + master empire
	DWORD							s_infoTime = 0;	// get_dword_time() of the last reload
	std::map<DWORD, DWORD>			s_lastList;		// pid -> last list request (ms)
	std::map<DWORD, time_t>			s_nextNotify;	// pid -> next join notification
	LPEVENT							s_notifyEvent = NULL;

	std::string Encode(const std::string& text)
	{
		std::string out(text);
		for (size_t i = 0; i < out.size(); ++i)
		{
			if (out[i] == ' ' || out[i] == '\n' || out[i] == '\r' || out[i] == '\t')
				out[i] = '\x1f';
		}
		return out;
	}

	void ReloadInfo(bool force = false)
	{
		const DWORD now = get_dword_time();
		if (!force && s_infoTime && now - s_infoTime < GUILD_APPLICATION_CACHE_MS)
			return;

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT g.id, IFNULL(pi.empire, 0), IFNULL(s.status, 0), IFNULL(s.lang, 0), IFNULL(s.min_level, 1), IFNULL(s.description, '') "
			"FROM guild g "
			"LEFT JOIN player p ON p.id = g.master "
			"LEFT JOIN player_index pi ON pi.id = p.account_id "
			"LEFT JOIN guild_application_setting s ON s.guild_id = g.id"));
		if (!pMsg || !pMsg->Get())
			return;

		s_info.clear();
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			SGuildAppInfo info;
			const DWORD gid = strtoul(row[0], NULL, 10);
			info.empire = (BYTE)atoi(row[1]);
			info.status = (BYTE)atoi(row[2]);
			info.lang = (BYTE)atoi(row[3]);
			info.min_level = atoi(row[4]);
			info.desc = row[5] ? row[5] : "";
			s_info[gid] = info;
		}
		s_infoTime = now;
	}

	SGuildAppInfo& GetInfo(DWORD gid)
	{
		ReloadInfo();
		return s_info[gid];
	}

	bool HasManageAccess(LPCHARACTER ch, CGuild* g)
	{
		// reference __IsLeaderModeAvailable / __HasGeneralAccess: master or general
		if (!g)
			return false;
		return g->GetMasterPID() == ch->GetPlayerID() || g->IsGuildGeneral(ch->GetPlayerID());
	}

	BYTE JobFromRace(int race)
	{
		// reference JOB_NAME: 0 warrior, 1 ninja, 2 sura, 3 shaman, 4 lycan
		if (race >= 8)	// lycan
			return 4;
		return (BYTE)(race % 4);
	}

	void ReportJoinError(LPCHARACTER ch, int code)
	{
		// same texts as CGuild::InviteAccept (CGuild::GuildJoinErrCode)
		switch (code)
		{
			case 1:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This player can be invited again after %d day(s)."), quest::CQuestManager::instance().GetEventFlag("guild_withdraw_delay"));
				break;
			case 2:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] After the rearrangement you can invite members again after %d days."), quest::CQuestManager::instance().GetEventFlag("guild_disband_delay"));
				break;
			case 3:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This person is already a member of another Guild."));
				break;
			case 4:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The maximum guild capacity has been reached."));
				break;
			case 5:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This guild is taking part in a battle at the moment."));
				break;
			case 6:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Guild> New subscription is currently restricted."));
				break;
			default:
				break;
		}
	}

	void DeleteApplicationsOf(DWORD pid)
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("DELETE FROM guild_application WHERE pid = %u", pid));
	}

	void SendSettings(LPCHARACTER ch, CGuild* g)
	{
		const SGuildAppInfo& info = GetInfo(g->GetID());
		char buf[CHAT_MAX_LEN + 1];
		snprintf(buf, sizeof(buf), "GuildAppSettings %u %u %u %d #%s", g->GetID(), info.status, info.lang, info.min_level, Encode(info.desc).c_str());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", buf);
	}

	void SendList(LPCHARACTER ch)
	{
		const DWORD now = get_dword_time();
		DWORD& last = s_lastList[ch->GetPlayerID()];
		if (last && now - last < GUILD_APPLICATION_LIST_DELAY_MS)	// reference CLICK_LIMIT_TIME
			return;
		last = now;

		ReloadInfo(true);

		std::set<DWORD> applied;
		{
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT guild_id FROM guild_application WHERE pid = %u", ch->GetPlayerID()));
			if (pMsg && pMsg->Get())
			{
				MYSQL_ROW row;
				while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
					applied.insert(strtoul(row[0], NULL, 10));
			}
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildAppListBegin");
		for (std::map<DWORD, SGuildAppInfo>::const_iterator it = s_info.begin(); it != s_info.end(); ++it)
		{
			CGuild* g = CGuildManager::instance().FindGuild(it->first);
			if (!g)
				continue;

			const SGuildAppInfo& info = it->second;
			const int maxMember = MIN(g->GetMaxMemberCount(), 9999);
			char buf[CHAT_MAX_LEN + 1];
			snprintf(buf, sizeof(buf), "GuildAppRow %u %u %u %d %d %d %u %u %d %d %s #%s",
				g->GetID(), info.status, g->GetLevel(), g->GetMemberCount(), maxMember, info.min_level,
				info.lang, info.empire, g->GetLadderPoint(), applied.count(g->GetID()) ? 1 : 0,
				Encode(g->GetName()).c_str(), Encode(info.desc).c_str());
			ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", buf);
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildAppListEnd");
	}

	void Apply(LPCHARACTER ch, DWORD gid)
	{
		CGuild* g = CGuildManager::instance().FindGuild(gid);
		if (!g)
			return;

		if (ch->GetGuild())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This person is already a member of another Guild."));
			return;
		}

		const SGuildAppInfo& info = GetInfo(gid);
		if (info.empire && info.empire != ch->GetEmpire())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You cannot join a guild of another kingdom."));
			return;
		}
		if (ch->GetLevel() < info.min_level)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Your level is too low to join this guild."));
			return;
		}
		if (info.status == GUILD_APP_STATUS_CLOSED)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This guild does not accept new members at the moment."));
			return;
		}

		const int code = g->VerifyApplicationJoin(ch);
		if (code != 0)
		{
			ReportJoinError(ch, code);
			return;
		}

		if (info.status == GUILD_APP_STATUS_OPEN)
		{
			// open guild: "Katil" joins at once
			DeleteApplicationsOf(ch->GetPlayerID());
			g->RequestAddMember(ch);
			sys_log(0, "GUILD_APPLICATION: %s joined open guild %s", ch->GetName(), g->GetName());
			return;
		}

		std::unique_ptr<SQLMsg> pInsert(DBManager::instance().DirectQuery(
			"INSERT IGNORE INTO guild_application (guild_id, pid, apply_time) VALUES (%u, %u, %u)",
			gid, ch->GetPlayerID(), (unsigned)get_global_time()));
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildAppApplied %u 1", gid);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Your application has been sent to %s."), g->GetName());

		LPCHARACTER master = CHARACTER_MANAGER::instance().FindByPID(g->GetMasterPID());
		if (master)
		{
			master->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] %s has applied to join the guild."), ch->GetName());
			master->ChatPacket(CHAT_TYPE_COMMAND, "GuildAppNewRequest");
		}
		sys_log(0, "GUILD_APPLICATION: %s applied to %s", ch->GetName(), g->GetName());
	}

	void Withdraw(LPCHARACTER ch, DWORD gid)
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"DELETE FROM guild_application WHERE guild_id = %u AND pid = %u", gid, ch->GetPlayerID()));
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildAppApplied %u 0", gid);
	}

	void SendRequests(LPCHARACTER ch)
	{
		CGuild* g = ch->GetGuild();
		if (!HasManageAccess(ch, g))
			return;

		SendSettings(ch, g);

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT a.pid, p.level, p.job, IFNULL(UNIX_TIMESTAMP(p.last_play), a.apply_time), p.name "
			"FROM guild_application a JOIN player p ON p.id = a.pid "
			"WHERE a.guild_id = %u ORDER BY a.apply_time", g->GetID()));

		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildAppReqBegin");
		if (pMsg && pMsg->Get())
		{
			MYSQL_ROW row;
			while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildAppReq %s %s %u %s %s",
					row[0], row[1], JobFromRace(atoi(row[2])), row[3], row[4]);
			}
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildAppReqEnd");
	}

	void Answer(LPCHARACTER ch, DWORD pid, bool approve)
	{
		CGuild* g = ch->GetGuild();
		if (!HasManageAccess(ch, g))
			return;

		std::unique_ptr<SQLMsg> pDelete(DBManager::instance().DirectQuery(
			"DELETE FROM guild_application WHERE guild_id = %u AND pid = %u", g->GetID(), pid));
		if (!pDelete || !pDelete->Get() || pDelete->Get()->uiAffectedRows == 0)
		{
			SendRequests(ch);
			return;
		}

		LPCHARACTER target = CHARACTER_MANAGER::instance().FindByPID(pid);
		if (!approve)
		{
			if (target)
			{
				target->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Your application to %s has been rejected."), g->GetName());
				target->ChatPacket(CHAT_TYPE_COMMAND, "GuildAppApplied %u 0", g->GetID());
			}
			SendRequests(ch);
			return;
		}

		{
			std::unique_ptr<SQLMsg> pMember(DBManager::instance().DirectQuery("SELECT guild_id FROM guild_member WHERE pid = %u", pid));
			if (pMember && pMember->Get() && pMember->Get()->uiNumRows > 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This person is already a member of another Guild."));
				SendRequests(ch);
				return;
			}
		}

		if (target)
		{
			const int code = g->VerifyApplicationJoin(target);
			if (code != 0)
			{
				ReportJoinError(ch, code);
				SendRequests(ch);
				return;
			}
			DeleteApplicationsOf(pid);
			g->RequestAddMember(target);
		}
		else
		{
			if (g->GetMemberCount() >= g->GetMaxMemberCount())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The maximum guild capacity has been reached."));
				SendRequests(ch);
				return;
			}
			// offline / other core: same DB request as CGuild::RequestAddMember
			DeleteApplicationsOf(pid);
			TPacketGDGuildAddMember gd;
			gd.dwPID = pid;
			gd.dwGuild = g->GetID();
			gd.bGrade = 15;
			db_clientdesc->DBPacket(HEADER_GD_GUILD_ADD_MEMBER, 0, &gd, sizeof(TPacketGDGuildAddMember));
		}
		sys_log(0, "GUILD_APPLICATION: %s approved pid %u for %s", ch->GetName(), pid, g->GetName());
		SendRequests(ch);
	}

	void SaveSetting(LPCHARACTER ch, const char* column, const char* value)
	{
		CGuild* g = ch->GetGuild();
		if (!HasManageAccess(ch, g))
			return;

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"INSERT INTO guild_application_setting (guild_id, %s) VALUES (%u, %s) ON DUPLICATE KEY UPDATE %s = VALUES(%s)",
			column, g->GetID(), value, column, column));
		ReloadInfo(true);
		SendSettings(ch, g);
	}

	EVENTINFO(guild_app_notify_info)
	{
		guild_app_notify_info() {}
	};

	void Notify(LPCHARACTER ch)
	{
		ReloadInfo();

		std::vector<DWORD> candidates;
		for (std::map<DWORD, SGuildAppInfo>::const_iterator it = s_info.begin(); it != s_info.end(); ++it)
		{
			const SGuildAppInfo& info = it->second;
			if (info.status == GUILD_APP_STATUS_CLOSED || info.empire != ch->GetEmpire() || ch->GetLevel() < info.min_level)
				continue;
			CGuild* g = CGuildManager::instance().FindGuild(it->first);
			if (!g || g->GetMemberCount() >= g->GetMaxMemberCount())
				continue;
			candidates.push_back(it->first);
		}
		if (candidates.empty())
			return;

		const DWORD gid = candidates[number(0, (int)candidates.size() - 1)];
		CGuild* g = CGuildManager::instance().FindGuild(gid);
		char buf[CHAT_MAX_LEN + 1];
		snprintf(buf, sizeof(buf), "Guild_JoinNotify %u %s #%s", gid, Encode(g->GetName()).c_str(), Encode(s_info[gid].desc).c_str());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", buf);
	}

	EVENTFUNC(guild_app_notify_event)
	{
		const time_t now = get_global_time();
		std::map<DWORD, time_t> next;

		const DESC_MANAGER::DESC_SET& c_set = DESC_MANAGER::instance().GetClientSet();
		for (DESC_MANAGER::DESC_SET::const_iterator it = c_set.begin(); it != c_set.end(); ++it)
		{
			LPCHARACTER ch = (*it)->GetCharacter();
			if (!ch || !ch->IsPC() || ch->GetGuild() || ch->GetLevel() < GUILD_APPLICATION_NOTIFY_MIN_LEVEL)
				continue;

			std::map<DWORD, time_t>::const_iterator f = s_nextNotify.find(ch->GetPlayerID());
			time_t when = (f != s_nextNotify.end()) ? f->second : now + GUILD_APPLICATION_NOTIFY_FIRST;
			if (now >= when)
			{
				Notify(ch);
				when = now + GUILD_APPLICATION_NOTIFY_INTERVAL;
			}
			next[ch->GetPlayerID()] = when;
		}
		s_nextNotify.swap(next);
		return PASSES_PER_SEC(30);
	}
}

void GuildApplication_OnLogin(LPCHARACTER ch)
{
	if (!ch || !ch->IsPC())
		return;

	if (!s_notifyEvent)
	{
		guild_app_notify_info* info = AllocEventInfo<guild_app_notify_info>();
		s_notifyEvent = event_create(guild_app_notify_event, info, PASSES_PER_SEC(30));
	}
	s_nextNotify[ch->GetPlayerID()] = get_global_time() + GUILD_APPLICATION_NOTIFY_FIRST;
}

void GuildApplication_Command(LPCHARACTER ch, const char* argument)
{
	if (!ch || !ch->IsPC())
		return;

	char sub[32];
	const char* rest = one_argument(argument, sub, sizeof(sub));
	while (*rest == ' ')
		++rest;

	if (!strcmp(sub, "list"))
		SendList(ch);
	else if (!strcmp(sub, "apply"))
		Apply(ch, strtoul(rest, NULL, 10));
	else if (!strcmp(sub, "withdraw"))
		Withdraw(ch, strtoul(rest, NULL, 10));
	else if (!strcmp(sub, "requests"))
		SendRequests(ch);
	else if (!strcmp(sub, "approve"))
		Answer(ch, strtoul(rest, NULL, 10), true);
	else if (!strcmp(sub, "reject"))
		Answer(ch, strtoul(rest, NULL, 10), false);
	else if (!strcmp(sub, "status"))
	{
		const int v = atoi(rest);
		if (v < GUILD_APP_STATUS_OPEN || v > GUILD_APP_STATUS_APPROVAL)
			return;
		char value[8];
		snprintf(value, sizeof(value), "%d", v);
		SaveSetting(ch, "status", value);
	}
	else if (!strcmp(sub, "lang"))
	{
		const int v = atoi(rest);
		if (v < 0 || v > GUILD_APPLICATION_LANG_MAX)
			return;
		char value[8];
		snprintf(value, sizeof(value), "%d", v);
		SaveSetting(ch, "lang", value);
	}
	else if (!strcmp(sub, "minlevel"))
	{
		const int v = atoi(rest);
		if (v < 1 || v > PLAYER_MAX_LEVEL_CONST)
			return;
		char value[8];
		snprintf(value, sizeof(value), "%d", v);
		SaveSetting(ch, "min_level", value);
	}
	else if (!strcmp(sub, "desc"))
	{
		std::string text(rest);
		if (text.size() > GUILD_APPLICATION_DESC_MAX_LEN)	// reference DESCRIPTION_MAX_LEN
			text.resize(GUILD_APPLICATION_DESC_MAX_LEN);
		for (size_t i = 0; i < text.size(); ++i)
		{
			if (text[i] == '\x1f' || text[i] == '\n' || text[i] == '\r' || text[i] == '|')
				text[i] = ' ';
		}
		char escaped[GUILD_APPLICATION_DESC_MAX_LEN * 2 + 1];
		DBManager::instance().EscapeString(escaped, sizeof(escaped), text.c_str(), text.size());
		std::string value = std::string("'") + escaped + "'";
		SaveSetting(ch, "description", value.c_str());
	}
}
#endif
