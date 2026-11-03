#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_GUILD_SHOP
#include "utils.h"
#include "config.h"
#include "char.h"
#include "guild.h"
#include "guild_manager.h"
#include "db.h"
#include "guild_donation.h"
#include "guild_shop.h"

// Guild shop (reference Baerim guild.json "shop") and the donation / skill logs.
// Table (player db): guild_log. Items our item_proto does not have are left out of the lists.

namespace
{
	enum EGuildShopResult
	{
		GUILD_SHOP_BOUGHT		= 1,
		GUILD_SHOP_LOCKED		= 2,
		GUILD_SHOP_NO_SILVER	= 3,
		GUILD_SHOP_NO_GOLD		= 4,
		GUILD_SHOP_NO_AUTH		= 5,
	};

	struct SGuildShopItem
	{
		DWORD	vnum;
		int		count;
		int		price;
		int		level;
	};

	const SGuildShopItem s_aSilverItems[] =
	{
		{ 71001, 1, 5, 1 },
		{ 25040, 5, 3, 1 },
		{ 71094, 1, 7, 2 },
		{ 70064, 10, 4, 3 },
		{ 70063, 10, 2, 3 },
		{ 70043, 1, 10, 4 },
		{ 50260, 10, 30, 5 },
		{ 70005, 1, 10, 5 },
		{ 80019, 10, 20, 6 },
		{ 100300, 1, 10, 6 },
		{ 71153, 1, 40, 7 },
		{ 90010, 1, 40, 8 },
		{ 40220, 1, 10, 8 },
		{ 90011, 1, 100, 9 },
		{ 70035, 1, 75, 9 },
		{ 90012, 1, 200, 10 },
		{ 71025, 1, 200, 10 },
		{ 71001, 40, 140, 11 },
		{ 71094, 40, 200, 11 },
		{ 40209, 10, 120, 12 },
		{ 40210, 5, 50, 14 },
		{ 50260, 200, 500, 15 },
		{ 80019, 200, 350, 16 },
		{ 40220, 5, 40, 16 },
		{ 71153, 5, 105, 17 },
		{ 90010, 5, 160, 18 },
		{ 90011, 5, 400, 19 },
		{ 70035, 5, 300, 19 },
		{ 90012, 5, 800, 20 },
		{ 71025, 5, 800, 20 }
	};

	const SGuildShopItem s_aGoldItems[] =
	{
		{ 65112, 1, 300, 1 },
		{ 65120, 1, 1000, 1 }
	};

	const int GUILD_SHOP_MAX_STACK = 200;

	void SendResult(LPCHARACTER ch, int result)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildShopResult %d", result);
	}

	void Buy(LPCHARACTER ch, CGuild* guild, int kind, int index, int buyCount)
	{
		const SGuildShopItem* list = kind ? s_aGoldItems : s_aSilverItems;
		const int count = kind ? (int)(sizeof(s_aGoldItems) / sizeof(s_aGoldItems[0])) : (int)(sizeof(s_aSilverItems) / sizeof(s_aSilverItems[0]));
		if (index < 0 || index >= count)
			return;

		const SGuildShopItem& entry = list[index];
		if (buyCount < 1 || buyCount * entry.count > GUILD_SHOP_MAX_STACK)
			return;

		const int price = entry.price * buyCount;
		if (guild->GetLevel() < entry.level)
		{
			SendResult(ch, GUILD_SHOP_LOCKED);
			return;
		}

		if (kind)
		{
			const TGuildMember* member = guild->GetMember(ch->GetPlayerID());
			if (!member || (guild->GetMasterPID() != ch->GetPlayerID() && !guild->HasGradeAuth(member->grade, GUILD_AUTH_SHOP)))
			{
				SendResult(ch, GUILD_SHOP_NO_AUTH);
				return;
			}
			if (!GuildDonation_TakeGold(guild, price))
			{
				SendResult(ch, GUILD_SHOP_NO_GOLD);
				return;
			}
		}
		else if (!GuildDonation_TakeSilver(ch, price))
		{
			SendResult(ch, GUILD_SHOP_NO_SILVER);
			return;
		}

		for (int i = 0; i < buyCount; ++i)
			ch->AutoGiveItem(entry.vnum, entry.count);
		SendResult(ch, GUILD_SHOP_BOUGHT);
		sys_log(0, "GUILD_SHOP: %s bought %u x%d from %s", ch->GetName(), entry.vnum, entry.count * buyCount, guild->GetName());
	}

	void SendLog(LPCHARACTER ch, CGuild* guild, int type, int daysAgo)
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT DATE_FORMAT(ts, '%%Y-%%m-%%d-%%H:%%i:%%s'), name, a, b, c FROM guild_log "
			"WHERE guild_id = %u AND type = %d AND DATE(ts) = DATE_SUB(CURDATE(), INTERVAL %d DAY) ORDER BY id DESC LIMIT 60",
			guild->GetID(), type, daysAgo));

		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildLogBegin %d", type);
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildLogRow %s %s %s %s %s", row[0], row[1], row[2], row[3], row[4]);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildLogEnd");
	}
}

void GuildLog_Add(CGuild* guild, int type, const char* name, int a, int b, int c)
{
	DBManager::instance().Query("INSERT INTO guild_log (guild_id, type, name, a, b, c, ts) VALUES (%u, %d, '%s', %d, %d, %d, NOW())",
		guild->GetID(), type, name, a, b, c);
}

void GuildShop_Command(LPCHARACTER ch, const char* argument)
{
	CGuild* guild = ch->GetGuild();
	if (!guild)
		return;

	char action[32], arg1[32], arg2[32], arg3[32];
	const char* rest = one_argument(argument, action, sizeof(action));
	rest = one_argument(rest, arg1, sizeof(arg1));
	rest = one_argument(rest, arg2, sizeof(arg2));
	one_argument(rest, arg3, sizeof(arg3));
	int value1 = 0, value2 = 0, value3 = 1;
	str_to_number(value1, arg1);
	str_to_number(value2, arg2);
	if (*arg3)
		str_to_number(value3, arg3);

	if (!strcmp(action, "buy"))
		Buy(ch, guild, value1, value2, value3);
	else if (!strcmp(action, "log") && (value1 == GUILD_LOG_DONATION || value1 == GUILD_LOG_SKILL) && value2 >= 0 && value2 < 7)
		SendLog(ch, guild, value1, value2);
}
#endif
