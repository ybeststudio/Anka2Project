#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_GUILD_DONATION
#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "desc.h"
#include "buffer_manager.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "db.h"
#include "guild_donation.h"
#ifdef ENABLE_GUILD_SKILL_TREE
#include "guild_skill.h"
#endif

// Guild gold / silver (reference Baerim DonationGoldWindow, SendWhisperGuild). The guild bank yang is not touched.
// Tables (player db): guild_donation_coin (gold of a guild), guild_donation_member (silver and daily counters of a player).

#define GUILD_WHISPER_MARK "[[GW]]"	// client uiWhisperMessenger.GUILD_WHISPER_MARK

namespace
{
	enum EGuildDonationResult
	{
		GUILD_DONATION_OK				= 0,
		GUILD_DONATION_LIMIT			= 1,
		GUILD_DONATION_NOT_ENOUGH_YANG	= 2,
		GUILD_DONATION_NOT_ENOUGH_GOLD	= 3,
		GUILD_DONATION_NO_AUTH			= 4,
		GUILD_DONATION_BUSY				= 6,
		GUILD_DONATION_WHISPER_SENT		= 7,
		GUILD_DONATION_NO_SCROLL		= 8,
		GUILD_DONATION_NO_PREMIUM		= 9,
	};

	enum EGuildDonationType
	{
		GUILD_DONATION_SCROLL	= 0,	// reference GiveFreeDonation
		GUILD_DONATION_PREMIUM	= 1,	// reference GivePremiumDonation
	};

	enum EGuildDonationValue
	{
		GUILD_DONATION_SCROLL_VNUM			= 65113,	// reference Katki Parsomeni
		GUILD_DONATION_PREMIUM_VNUM			= 65114,	// reference Asil Katki
		GUILD_DONATION_PREMIUM_DAILY_LIMIT	= 10,
		GUILD_DONATION_SCROLL_GAIN			= 1,		// gold and silver of one donation
		GUILD_DONATION_PREMIUM_GAIN			= 10,
	};

	struct SGuildDonationMember
	{
		unsigned int	silver;
		int				scroll;
		int				premium;

		SGuildDonationMember() : silver(0), scroll(0), premium(0) {}
	};

	int Today()
	{
		time_t now = get_global_time();
		const tm* t = localtime(&now);
		return (1900 + t->tm_year) * 1000 + t->tm_yday + 1;		// reference GuildMemberDailyDonation day: 2026302
	}

	void SendResult(LPCHARACTER ch, int result)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildDonationResult %d", result);
	}

	unsigned int LoadGuildGold(DWORD guildID)
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT gold FROM guild_donation_coin WHERE guild_id = %u", guildID));
		if (pMsg->Get()->uiNumRows == 0)
			return 0;
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		unsigned int gold = 0;
		str_to_number(gold, row[0]);
		return gold;
	}

	SGuildDonationMember LoadMember(DWORD pid)
	{
		SGuildDonationMember member;
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT silver, day, scroll, premium FROM guild_donation_member WHERE pid = %u", pid));
		if (pMsg->Get()->uiNumRows == 0)
			return member;
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		int day = 0;
		str_to_number(member.silver, row[0]);
		str_to_number(day, row[1]);
		if (day == Today())
		{
			str_to_number(member.scroll, row[2]);
			str_to_number(member.premium, row[3]);
		}
		return member;
	}

	struct FSendToGuild
	{
		CGuild*		m_guild;
		const char*	m_command;

		FSendToGuild(CGuild* guild, const char* command) : m_guild(guild), m_command(command) {}

		void operator()(LPCHARACTER ch)
		{
			if (ch->GetGuild() == m_guild)
				ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", m_command);
		}
	};

	void SendToGuild(CGuild* guild, const char* command)
	{
		FSendToGuild f(guild, command);
		CHARACTER_MANAGER::instance().for_each_pc(f);
	}

	void SendPersonal(LPCHARACTER ch, const SGuildDonationMember& member)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "SendDailyGuildDonation %d %d", member.scroll, member.premium);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildSilverCoins %u", member.silver);
	}

	void SendMemberDaily(LPCHARACTER ch, CGuild* guild)
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT m.pid, d.scroll + d.premium FROM guild_member m "
			"JOIN guild_donation_member d ON d.pid = m.pid WHERE m.guild_id = %u AND d.day = %d", guild->GetID(), Today()));

		std::string payload;
		int count = 0;
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			payload += row[0];
			payload += ":";
			payload += row[1];
			payload += ",";
			if (++count == GUILD_DONATION_DAILY_CHUNK)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildMemberDailyDonation %s", payload.c_str());
				payload.clear();
				count = 0;
			}
		}
		if (!payload.empty())
			ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildMemberDailyDonation %s", payload.c_str());
	}

	// reference: a real whisper from the typed sender name, so every member gets the whisper envelope
	void SendWhisperToMembers(LPCHARACTER ch, CGuild* guild, const std::string& sender, const std::string& message)
	{
		TPacketGCWhisper pack;
		pack.bHeader = HEADER_GC_WHISPER;
		const std::string text = std::string(GUILD_WHISPER_MARK) + message;		// the client shows it as a read only guild conversation
		pack.wSize = sizeof(TPacketGCWhisper) + text.size() + 1;
		pack.bType = WHISPER_TYPE_NORMAL;
		strlcpy(pack.szNameFrom, sender.c_str(), sizeof(pack.szNameFrom));

		TEMP_BUFFER buf;
		buf.write(&pack, sizeof(pack));
		buf.write(text.c_str(), text.size() + 1);

		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
			"SELECT p.name FROM guild_member m JOIN player p ON p.id = m.pid WHERE m.guild_id = %u", guild->GetID()));

		MYSQL_ROW row;
		while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
		{
			if (!strcasecmp(row[0], ch->GetName()))
				continue;

			LPCHARACTER target = CHARACTER_MANAGER::instance().FindPC(row[0]);
			if (target)
			{
				if (target->GetDesc())
					target->GetDesc()->Packet(buf.read_peek(), buf.size());
				continue;
			}

			CCI* peer = P2P_MANAGER::instance().Find(row[0]);
			if (peer && peer->pkDesc)
			{
				peer->pkDesc->SetRelay(row[0]);
				peer->pkDesc->Packet(buf.read_peek(), buf.size());
				peer->pkDesc->SetRelay("");
			}
		}
	}

	bool CanUse(LPCHARACTER ch)
	{
		if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->GetShop())
		{
			SendResult(ch, GUILD_DONATION_BUSY);
			return false;
		}
		return true;
	}
}

void GuildDonation_Info(LPCHARACTER ch)
{
	CGuild* guild = ch->GetGuild();
	if (!guild)
		return;

	ch->ChatPacket(CHAT_TYPE_COMMAND, "GuildGoldCoins %u", LoadGuildGold(guild->GetID()));
	SendPersonal(ch, LoadMember(ch->GetPlayerID()));
	SendMemberDaily(ch, guild);
}

void GuildDonation_Donate(LPCHARACTER ch, const char* argument)
{
	CGuild* guild = ch->GetGuild();
	if (!guild || !CanUse(ch))
		return;

	char arg[32];
	one_argument(argument, arg, sizeof(arg));
	int type = -1;
	str_to_number(type, arg);

	if (type != GUILD_DONATION_SCROLL && type != GUILD_DONATION_PREMIUM)
		return;

	const bool isScroll = (type == GUILD_DONATION_SCROLL);
	const DWORD itemVnum = isScroll ? GUILD_DONATION_SCROLL_VNUM : GUILD_DONATION_PREMIUM_VNUM;
	const int gain = isScroll ? GUILD_DONATION_SCROLL_GAIN : GUILD_DONATION_PREMIUM_GAIN;

	SGuildDonationMember member = LoadMember(ch->GetPlayerID());
	if ((isScroll ? member.scroll : member.premium) >= (isScroll ? GUILD_DONATION_SCROLL_DAILY_LIMIT : GUILD_DONATION_PREMIUM_DAILY_LIMIT))
	{
		SendResult(ch, GUILD_DONATION_LIMIT);
		return;
	}
	if (ch->CountSpecifyItem(itemVnum) < 1)
	{
		SendResult(ch, isScroll ? GUILD_DONATION_NO_SCROLL : GUILD_DONATION_NO_PREMIUM);
		return;
	}
	if (isScroll && ch->GetGold() < GUILD_DONATION_SCROLL_PRICE)
	{
		SendResult(ch, GUILD_DONATION_NOT_ENOUGH_YANG);
		return;
	}

	ch->RemoveSpecifyItem(itemVnum, 1);
	if (isScroll)
		ch->PointChange(POINT_GOLD, -GUILD_DONATION_SCROLL_PRICE);

	const int today = Today();
	std::unique_ptr<SQLMsg> pMember(DBManager::instance().DirectQuery(
		"INSERT INTO guild_donation_member (pid, silver, day, scroll, premium) VALUES (%u, %d, %d, %d, %d) "
		"ON DUPLICATE KEY UPDATE silver = silver + %d, scroll = IF(day = %d, scroll, 0) + %d, premium = IF(day = %d, premium, 0) + %d, day = %d",
		ch->GetPlayerID(), gain, today, isScroll ? 1 : 0, isScroll ? 0 : 1,
		gain, today, isScroll ? 1 : 0, today, isScroll ? 0 : 1, today));
	int goldGain = gain;
#ifdef ENABLE_GUILD_SKILL_TREE
	if (number(1, 100) <= GuildSkill_GetValue(guild, GUILD_SKILL_TREE_DONATION))
		goldGain *= 2;
#endif
	std::unique_ptr<SQLMsg> pCoin(DBManager::instance().DirectQuery(
		"INSERT INTO guild_donation_coin (guild_id, gold) VALUES (%u, %d) ON DUPLICATE KEY UPDATE gold = gold + %d", guild->GetID(), goldGain, goldGain));

	member.silver += gain;
	if (isScroll)
		member.scroll += 1;
	else
		member.premium += 1;
	SendPersonal(ch, member);

	char command[CHAT_MAX_LEN + 1];
	snprintf(command, sizeof(command), "GuildGoldCoins %u", LoadGuildGold(guild->GetID()));
	SendToGuild(guild, command);
	snprintf(command, sizeof(command), "GuildMemberDailyDonation %u:%d,", ch->GetPlayerID(), member.scroll + member.premium);
	SendToGuild(guild, command);
	SendResult(ch, GUILD_DONATION_OK);

	sys_log(0, "GUILD_DONATION: %s donated item %u to %s", ch->GetName(), itemVnum, guild->GetName());
}

void GuildDonation_AddGold(CGuild* guild, int gold)
{
	std::unique_ptr<SQLMsg> pCoin(DBManager::instance().DirectQuery(
		"INSERT INTO guild_donation_coin (guild_id, gold) VALUES (%u, %d) ON DUPLICATE KEY UPDATE gold = gold + %d", guild->GetID(), gold, gold));

	char command[CHAT_MAX_LEN + 1];
	snprintf(command, sizeof(command), "GuildGoldCoins %u", LoadGuildGold(guild->GetID()));
	SendToGuild(guild, command);
}

bool GuildDonation_TakeGold(CGuild* guild, int gold)
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
		"UPDATE guild_donation_coin SET gold = gold - %d WHERE guild_id = %u AND gold >= %d", gold, guild->GetID(), gold));
	if (pMsg->Get()->uiAffectedRows == 0)
		return false;

	char command[CHAT_MAX_LEN + 1];
	snprintf(command, sizeof(command), "GuildGoldCoins %u", LoadGuildGold(guild->GetID()));
	SendToGuild(guild, command);
	return true;
}

bool GuildDonation_TakeSilver(LPCHARACTER ch, int silver)
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
		"UPDATE guild_donation_member SET silver = silver - %d WHERE pid = %u AND silver >= %d", silver, ch->GetPlayerID(), silver));
	if (pMsg->Get()->uiAffectedRows == 0)
		return false;
	SendPersonal(ch, LoadMember(ch->GetPlayerID()));
	return true;
}

void GuildDonation_AddSilver(LPCHARACTER ch, int silver)
{
	std::unique_ptr<SQLMsg> pMember(DBManager::instance().DirectQuery(
		"INSERT INTO guild_donation_member (pid, silver) VALUES (%u, %d) ON DUPLICATE KEY UPDATE silver = silver + %d", ch->GetPlayerID(), silver, silver));
	SendPersonal(ch, LoadMember(ch->GetPlayerID()));
}

void GuildDonation_Whisper(LPCHARACTER ch, const char* argument)
{
	CGuild* guild = ch->GetGuild();
	if (!guild)
		return;

	const TGuildMember* member = guild->GetMember(ch->GetPlayerID());
	if (!member || (guild->GetMasterPID() != ch->GetPlayerID() && !guild->HasGradeAuth(member->grade, GUILD_AUTH_NOTICE)))
	{
		SendResult(ch, GUILD_DONATION_NO_AUTH);
		return;
	}

	char sender[CHAT_MAX_LEN + 1];
	const char* text = one_argument(argument, sender, sizeof(sender));
	while (*text == ' ')
		++text;
	if (!*sender || !*text)
		return;

	std::string senderName(sender, MIN(strlen(sender), (size_t)GUILD_WHISPER_SENDER_MAX_LEN));
	std::string message(text, MIN(strlen(text), (size_t)GUILD_WHISPER_TEXT_MAX_LEN));
	std::replace(senderName.begin(), senderName.end(), '|', ' ');
	std::replace(message.begin(), message.end(), '|', ' ');

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery(
		"UPDATE guild_donation_coin SET gold = gold - %d WHERE guild_id = %u AND gold >= %d",
		GUILD_WHISPER_PRICE, guild->GetID(), GUILD_WHISPER_PRICE));
	if (pMsg->Get()->uiAffectedRows == 0)
	{
		SendResult(ch, GUILD_DONATION_NOT_ENOUGH_GOLD);
		return;
	}

	SendWhisperToMembers(ch, guild, senderName, message);

	char command[CHAT_MAX_LEN + 1];
	snprintf(command, sizeof(command), "GuildGoldCoins %u", LoadGuildGold(guild->GetID()));
	SendToGuild(guild, command);
	SendResult(ch, GUILD_DONATION_WHISPER_SENT);
}
#endif
