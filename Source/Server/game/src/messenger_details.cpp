#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_MESSENGER_DETAILS
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "desc.h"
#include "p2p.h"
#include "db.h"
#include "messenger_details.h"

// Client side: game.py "MessengerInfo <group> <name> <level> <race> <channel> <mapIndex> <lastSeen>".
// Online members carry level / race / channel / map and lastSeen 0; offline members carry race and the unix time of
// their last save (player.last_play), level 0 (reference: the offline card shows no level).

namespace
{
	enum EMessengerDetailsGroup
	{
		MESSENGER_DETAILS_GROUP_FRIEND	= 0,	// client MESSENGER_GRUOP_INDEX_FRIEND
		MESSENGER_DETAILS_GROUP_GUILD	= 1,	// client MESSENGER_GRUOP_INDEX_GUILD
	};

	void SendInfo(LPCHARACTER to, int group, const char* name, int level, int race, int channel, long mapIndex, DWORD lastSeen)
	{
		if (!to || !to->GetDesc())
			return;

		to->ChatPacket(CHAT_TYPE_COMMAND, "MessengerInfo %d %s %d %d %d %ld %u", group, name, level, race, channel, mapIndex, lastSeen);
	}

	// dbLevel / dbRace / dbLastSeen come from the player table; live data wins whenever the member is online
	void SendMember(LPCHARACTER to, int group, const char* name, int dbLevel, int dbRace, DWORD dbLastSeen)
	{
		LPCHARACTER member = CHARACTER_MANAGER::instance().FindPC(name);
		if (member && member->GetDesc())
		{
			SendInfo(to, group, name, member->GetLevel(), member->GetRaceNum(), g_bChannel, member->GetMapIndex(), 0);
			return;
		}

		CCI* pkCCI = P2P_MANAGER::instance().Find(name);
		if (pkCCI)
		{
			SendInfo(to, group, name, dbLevel, dbRace, pkCCI->bChannel, pkCCI->lMapIndex, 0);
			return;
		}

		SendInfo(to, group, name, 0, dbRace, 0, 0, dbLastSeen ? dbLastSeen : 1);
	}

	void OnMembersLoaded(DWORD pid, SQLMsg* pMsg)
	{
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pid);
		if (!ch || !ch->GetDesc() || !pMsg || !pMsg->Get())
			return;

		for (uint32_t i = 0; i < pMsg->Get()->uiNumRows; ++i)
		{
			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			if (!row || !row[1])
				continue;

			int group = atoi(row[0]);
			int level = row[2] ? atoi(row[2]) : 0;
			int race = row[3] ? atoi(row[3]) : 0;
			DWORD lastSeen = row[4] ? strtoul(row[4], NULL, 10) : 0;
			SendMember(ch, group, row[1], level, race, lastSeen);
		}
	}

	void OnCompanionLoaded(std::string account, SQLMsg* pMsg)
	{
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account.c_str());
		if (!ch || !ch->GetDesc() || !pMsg || !pMsg->Get() || pMsg->Get()->uiNumRows == 0)
			return;

		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		if (!row || !row[0])
			return;

		SendMember(ch, MESSENGER_DETAILS_GROUP_FRIEND, row[0], row[1] ? atoi(row[1]) : 0, row[2] ? atoi(row[2]) : 0, 0);
	}
}

void MessengerDetails_OnLogin(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
		return;

	char escapedName[CHARACTER_NAME_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(escapedName, sizeof(escapedName), ch->GetName(), strlen(ch->GetName()));

	const DWORD pid = ch->GetPlayerID();
	DBManager::instance().FuncQuery(std::bind(&OnMembersLoaded, pid, std::placeholders::_1),
		"SELECT %d, p.name, p.level, p.job, UNIX_TIMESTAMP(p.last_play) FROM messenger_list%s m, player%s p "
		"WHERE m.account = '%s' AND p.name = m.companion "
		"UNION ALL "
		"SELECT %d, p.name, p.level, p.job, UNIX_TIMESTAMP(p.last_play) FROM guild_member%s g, guild_member%s me, player%s p "
		"WHERE me.pid = %u AND g.guild_id = me.guild_id AND g.pid <> %u AND p.id = g.pid",
		MESSENGER_DETAILS_GROUP_FRIEND, get_table_postfix(), get_table_postfix(), escapedName,
		MESSENGER_DETAILS_GROUP_GUILD, get_table_postfix(), get_table_postfix(), get_table_postfix(), pid, pid);
}

void MessengerDetails_OnFriendLogin(const char* account, const char* companion)
{
	if (!account || !companion || !*companion)
		return;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(account);
	if (!ch || !ch->GetDesc())
		return;

	// same core: live data right away, otherwise the player row gives level / race and the P2P entry channel / map
	if (CHARACTER_MANAGER::instance().FindPC(companion))
	{
		SendMember(ch, MESSENGER_DETAILS_GROUP_FRIEND, companion, 0, 0, 0);
		return;
	}

	char escapedName[CHARACTER_NAME_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(escapedName, sizeof(escapedName), companion, strlen(companion));
	DBManager::instance().FuncQuery(std::bind(&OnCompanionLoaded, std::string(account), std::placeholders::_1),
		"SELECT name, level, job FROM player%s WHERE name = '%s'", get_table_postfix(), escapedName);
}
#endif
