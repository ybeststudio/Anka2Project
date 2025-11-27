#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "protocol.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "cmd.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "battle.h"
#include "exchange.h"
#include "questmanager.h"
#include "profiler.h"
#include "messenger_manager.h"
#include "party.h"
#include "p2p.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "log.h"
#include "banword.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "building.h"
#include "locale_service.h"
#include "gm.h"
#include "spam.h"
#include "ani.h"
#include "motion.h"
#include "OXEvent.h"
#include "locale_service.h"
#include "DragonSoul.h"
#include "belt_inventory_helper.h"
#include <unordered_map>

#include "../../common/service.h"
#ifdef ENABLE_RENEWAL_SWITCHBOT
	#include "switchbot_manager.h"
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
	#include "biolog_manager.h"
#endif

#ifdef ENABLE_EVENT_MANAGER
	#include "event_manager.h"
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
	#include "battlepass_manager.h"
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	#include "offline_shop.h"
	#include "offlineshop_manager.h"
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	#include "growth_pet.h"
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	#include "changelook.h"
#endif

#ifdef ENABLE_BOT_PLAYER
#include "BotPlayer.h"
#endif
#ifdef ENABLE_RIDING_EXTENDED
	#include "mount_up_grade.h"
#endif

extern void SendShout(const char * szText, BYTE bEmpire
#ifdef ENABLE_MESSENGER_BLOCK
	, const char* c_szName
#endif
);
extern int g_nPortalLimitTime;

static int __deposit_limit()
{
	return (1000*10000);
}

#ifdef ENABLE_MOB_DROP_INFO
void CInputMain::TargetInfoLoad(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGTargetInfoLoad* p = (TPacketCGTargetInfoLoad*)c_pData;
	TPacketGCTargetInfo pInfo;
	pInfo.header = HEADER_GC_TARGET_INFO;
	static std::vector<std::pair<int,int> > s_vec_item;
	s_vec_item.clear();
	LPCHARACTER m_pkChrTarget = CHARACTER_MANAGER::instance().Find(p->dwVID);

	if (!ch || !m_pkChrTarget)
		return;

	// Gerçek oyuncu kontrolü
	if (!ch->IsPC())
		return;

	// Oyuncularý hariç tut (sadece yaratýklar ve taþlar için çalýþýr)
	if (m_pkChrTarget->IsPC())
		return;

#ifdef ENABLE_BOT_PLAYER
    // Bot karakterleri tamamen hariç tut
    if (ch->IsBotCharacter() || m_pkChrTarget->IsBotCharacter())
        return;
#endif

	if ((m_pkChrTarget->IsMonster() || m_pkChrTarget->IsStone()) && ITEM_MANAGER::instance().CreateDropItemVector(m_pkChrTarget, ch, s_vec_item))
	{
		for(std::vector<std::pair<int,int> >::const_iterator iter = s_vec_item.begin(); iter != s_vec_item.end();++iter)
		{
			pInfo.dwVID	= m_pkChrTarget->GetVID();
			pInfo.race = m_pkChrTarget->GetRaceNum();
			pInfo.dwVnum = iter->first;
			pInfo.count = iter->second;
			ch->GetDesc()->Packet(&pInfo, sizeof(TPacketGCTargetInfo));
		}
	}
}
#endif

#ifdef ENABLE_EVENT_MANAGER
void CInputMain::RequestEventQuest(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGRequestEventQuest* p = (TPacketCGRequestEventQuest*)c_pData;

	if (ch && ch->GetDesc())
		quest::CQuestManager::instance().RequestEventQuest(p->szName, ch->GetPlayerID());
}

void CInputMain::RequestEventData(LPCHARACTER ch, const char* c_pData)
{
	if (ch && ch->GetDesc())
	{
		const TPacketCGRequestEventData* p = reinterpret_cast<const TPacketCGRequestEventData*>(c_pData);
		CEventManager::Instance().SendEventInfo(ch, p->bMonth);
	}
}
#endif

void SendBlockChatInfo(LPCHARACTER ch, int sec)
{
	if (sec <= 0)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 164, "");
		return;
	}

	long hour = sec / 3600;
	sec -= hour * 3600;

	long min = (sec / 60);
	sec -= min * 60;

	if (hour > 0 && min > 0)
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 165, "%d#%d#%d", hour, min, sec);
	else if (hour > 0 && min == 0)
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 166, "%d#%d", hour, sec);
	else if (hour == 0 && min > 0)
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 167, "%d#%d", hour, sec);
	else
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 168, "%d", sec);
}

EVENTINFO(spam_event_info)
{
	char host[MAX_HOST_LENGTH+1];

	spam_event_info()
	{
		::memset(host, 0, MAX_HOST_LENGTH+1);
	}
};

typedef std::unordered_map<std::string, std::pair<unsigned int, LPEVENT> > spam_score_of_ip_t;
spam_score_of_ip_t spam_score_of_ip;

EVENTFUNC(block_chat_by_ip_event)
{
	spam_event_info* info = dynamic_cast<spam_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "block_chat_by_ip_event> <Factor> Null pointer" );
		return 0;
	}

	const char * host = info->host;

	spam_score_of_ip_t::iterator it = spam_score_of_ip.find(host);

	if (it != spam_score_of_ip.end())
	{
		it->second.first = 0;
		it->second.second = NULL;
	}

	return 0;
}

bool SpamBlockCheck(LPCHARACTER ch, const char* const buf, const size_t buflen)
{
	extern int g_iSpamBlockMaxLevel;

	if (ch->GetLevel() < g_iSpamBlockMaxLevel)
	{
		spam_score_of_ip_t::iterator it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());

		if (it == spam_score_of_ip.end())
		{
			spam_score_of_ip.insert(std::make_pair(ch->GetDesc()->GetHostName(), std::make_pair(0, (LPEVENT) NULL)));
			it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());
		}

		if (it->second.second)
		{
			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);
			return true;
		}

		unsigned int score;
		const char * word = SpamManager::instance().GetSpamScore(buf, buflen, score);

		it->second.first += score;

		if (word)
			sys_log(0, "SPAM_SCORE: %s text: %s score: %u total: %u word: %s", ch->GetName(), buf, score, it->second.first, word);

		extern unsigned int g_uiSpamBlockScore;
		extern unsigned int g_uiSpamBlockDuration;

		if (it->second.first >= g_uiSpamBlockScore)
		{
			spam_event_info* info = AllocEventInfo<spam_event_info>();
			strlcpy(info->host, ch->GetDesc()->GetHostName(), sizeof(info->host));

			it->second.second = event_create(block_chat_by_ip_event, info, PASSES_PER_SEC(g_uiSpamBlockDuration));
			sys_log(0, "SPAM_IP: %s for %u seconds", info->host, g_uiSpamBlockDuration);

			LogManager::instance().CharLog(ch, 0, "SPAM", word);

			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);

			return true;
		}
	}

	return false;
}

enum
{
	TEXT_TAG_PLAIN,
	TEXT_TAG_TAG,
	TEXT_TAG_COLOR,
	TEXT_TAG_HYPERLINK_START,
	TEXT_TAG_HYPERLINK_END,
	TEXT_TAG_RESTORE_COLOR,
#ifdef ENABLE_EMOTICONS_SYSTEM
	TEXT_TAG_EMOTICON_START,
	TEXT_TAG_EMOTICON_END,
#endif
};

int GetTextTag(const char * src, int maxLen, int & tagLen, std::string & extraInfo)
{
	tagLen = 1;

	if (maxLen < 2 || *src != '|')
		return TEXT_TAG_PLAIN;

	const char * cur = ++src;

	if (*cur == '|')
	{
		tagLen = 2;
		return TEXT_TAG_TAG;
	}
	else if (*cur == 'c')
	{
		tagLen = 2;
		return TEXT_TAG_COLOR;
	}
	else if (*cur == 'H')
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_START;
	}
	else if (*cur == 'h')
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_END;
	}
#ifdef ENABLE_EMOTICONS_SYSTEM
	else if (*cur == 'T')
	{
		tagLen = 2;
		return TEXT_TAG_EMOTICON_START;
	}
	else if (*cur == 't')
	{
		tagLen = 2;
		return TEXT_TAG_EMOTICON_END;
	}
#endif

	return TEXT_TAG_PLAIN;
}

void GetTextTagInfo(const char * src, int src_len, int & hyperlinks, bool & colored
#ifdef ENABLE_EMOTICONS_SYSTEM
	, int & emojilinks
#endif
)
{
	colored = false;
	hyperlinks = 0;
#ifdef ENABLE_EMOTICONS_SYSTEM
	emojilinks = 0;
#endif

	int len;
	std::string extraInfo;

	for (int i = 0; i < src_len;)
	{
		int tag = GetTextTag(&src[i], src_len - i, len, extraInfo);

		if (tag == TEXT_TAG_HYPERLINK_START)
			++hyperlinks;

		if (tag == TEXT_TAG_COLOR)
			colored = true;

#ifdef ENABLE_EMOTICONS_SYSTEM
		if (tag == TEXT_TAG_EMOTICON_START)
			++emojilinks;
#endif

		i += len;
	}
}

int ProcessTextTag(LPCHARACTER ch, const char * c_pszText, size_t len)
{
	return 0;
	int hyperlinks;
	bool colored;
#ifdef ENABLE_EMOTICONS_SYSTEM
	int emojilinks;
#endif

	GetTextTagInfo(c_pszText, len, hyperlinks, colored
#ifdef ENABLE_EMOTICONS_SYSTEM
		, emojilinks
#endif
	);

	if (colored == true && hyperlinks == 0
#ifdef ENABLE_EMOTICONS_SYSTEM
		&& emojilinks == 0
#endif
	)
		return 4;

	if (ch->GetExchange())
	{
		if (hyperlinks == 0)
			return 0;
		else
			return 3;
	}

	int nPrismCount = ch->CountSpecifyItem(ITEM_PRISM);

	if (nPrismCount < hyperlinks)
		return 1;

	if (!ch->GetMyShop())
	{
		ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
		return 0;
	}
	else
	{
		int sellingNumber = ch->GetMyShop()->GetNumberByVnum(ITEM_PRISM);
		if(nPrismCount - sellingNumber < hyperlinks)
		{
			return 2;
		}
		else
		{
			ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
			return 0;
		}
	}

	return 4;
}

int CInputMain::Whisper(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGWhisper* pinfo = reinterpret_cast<const TPacketCGWhisper*>(data);

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGWhisper);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (ch->GetLastPMPulse() < thecore_pulse())
		ch->ClearPMCounter();
		
	if (ch->GetPMCounter() > 3 && ch->GetLastPMPulse() > thecore_pulse())
	{
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (ch->FindAffect(AFFECT_BLOCK_CHAT))
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 164, "");
		return (iExtraLen);
	}

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(pinfo->szNameTo);

	if (pkChr == ch)
		return (iExtraLen);
	
	ch->IncreasePMCounter();
	ch->SetLastPMPulse();

	LPDESC pkDesc = NULL;

	BYTE bOpponentEmpire = 0;

	if (test_server)
	{
		if (!pkChr)
			sys_log(0, "Whisper to %s(%s) from %s", "Null", pinfo->szNameTo, ch->GetName());
		else
			sys_log(0, "Whisper to %s(%s) from %s", pkChr->GetName(), pinfo->szNameTo, ch->GetName());
	}
		
	if (ch->IsBlockMode(BLOCK_WHISPER))
	{
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(pack));
		}
		return iExtraLen;
	}

	if (!pkChr)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

		if (pkCCI)
		{
			pkDesc = pkCCI->pkDesc;
			pkDesc->SetRelay(pinfo->szNameTo);
			bOpponentEmpire = pkCCI->bEmpire;

			if (test_server)
				sys_log(0, "Whisper to %s from %s (Channel %d Mapindex %d)", "Null", ch->GetName(), pkCCI->bChannel, pkCCI->lMapIndex);
		}
	}
	else
	{
		pkDesc = pkChr->GetDesc();
		bOpponentEmpire = pkChr->GetEmpire();
	}

	if (!pkDesc)
	{
		if (ch->GetDesc())
		{
#ifdef ENABLE_OFFLINE_MESSAGE
			const BYTE bDelay = 10;
			if (get_dword_time() - ch->GetLastOfflinePMTime() > bDelay * 1000)
			{
				char buf[CHAT_MAX_LEN + 1];
				strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
				const size_t buflen = strlen(buf);

				CBanwordManager::instance().ConvertString(buf, buflen);

				int processReturn = ProcessTextTag(ch, buf, buflen);
				if (0 != processReturn)
				{
					TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

					if (pTable)
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 171, "%s",
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
							LC_LOCALE_ITEM(ITEM_PRISM, ch->GetLanguage())
#else
							pTable->szLocaleName
#endif
						);
					}
					return (iExtraLen);
				}

				if (buflen > 0)
				{
					ch->SendOfflineMessage(pinfo->szNameTo, buf);
					ch->LocaleWhisperPacket(WHISPER_TYPE_SYSTEM, 611, pinfo->szNameTo, "");
				}
				else
					return (iExtraLen);
			}
			else
				ch->LocaleWhisperPacket(WHISPER_TYPE_SYSTEM, 77, pinfo->szNameTo, "%d", bDelay);
#else
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_NOT_EXIST;
			pack.wSize = sizeof(TPacketGCWhisper);
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			ch->GetDesc()->Packet(&pack, sizeof(TPacketGCWhisper));
			sys_log(0, "WHISPER: no player");
#endif
		}
	}
	else
	{
		if (ch->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else if (pkChr && pkChr->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_TARGET_BLOCKED;
				pack.wSize = sizeof(TPacketGCWhisper);
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
#ifdef ENABLE_MESSENGER_BLOCK
		else if (pkChr && MessengerManager::instance().IsBlocked(ch->GetName(), pkChr->GetName()))
		{
			if (ch->GetDesc())
			{
				ch->LocaleWhisperPacket(WHISPER_TYPE_SYSTEM, 169, pinfo->szNameTo, "%s", pkChr->GetName());
			}
		}
		else if (pkChr && MessengerManager::instance().IsBlocked(pkChr->GetName(), ch->GetName()))
		{
			if (ch->GetDesc())
			{
				ch->LocaleWhisperPacket(WHISPER_TYPE_SYSTEM, 170, pinfo->szNameTo, "%s", pkChr->GetName());
			}
		}
#endif
		else
		{
			BYTE bType = WHISPER_TYPE_NORMAL;

			char buf[CHAT_MAX_LEN + 1];
			strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
			const size_t buflen = strlen(buf);

			if (true == SpamBlockCheck(ch, buf, buflen))
			{
				if (!pkChr)
				{
					CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

					if (pkCCI)
					{
						pkDesc->SetRelay("");
					}
				}
				return iExtraLen;
			}

			CBanwordManager::instance().ConvertString(buf, buflen);

			if (g_bEmpireWhisper)
				if (!ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
					if (!(pkChr && pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)))
						if (bOpponentEmpire != ch->GetEmpire() && ch->GetEmpire() && bOpponentEmpire && ch->GetGMLevel() == GM_PLAYER && gm_get_level(pinfo->szNameTo) == GM_PLAYER)
						{
							if (!pkChr)
							{
								bType = ch->GetEmpire() << 4;
							}
							else
							{
								ConvertEmpireText(ch->GetEmpire(), buf, buflen, 10 + 2 * pkChr->GetSkillPower(SKILL_LANGUAGE1 + ch->GetEmpire() - 1));
							}
						}

			int processReturn = ProcessTextTag(ch, buf, buflen);
			if (0 != processReturn)
			{
				if (ch->GetDesc())
				{
					TItemTable * pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

					if (pTable)
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 171, "%s",
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
							LC_LOCALE_ITEM(ITEM_PRISM, ch->GetLanguage())
#else
							pTable->szLocaleName
#endif
						);
					}
				}

				pkDesc->SetRelay("");
				return (iExtraLen);
			}

			if (ch->IsGM())
				bType = (bType & 0xF0) | WHISPER_TYPE_GM;

			if (buflen > 0)
			{
				TPacketGCWhisper pack;

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + buflen;
				pack.bType = bType;
				strlcpy(pack.szNameFrom, ch->GetName(), sizeof(pack.szNameFrom));

				TEMP_BUFFER tmpbuf;

				tmpbuf.write(&pack, sizeof(pack));
				tmpbuf.write(buf, buflen);

				pkDesc->Packet(tmpbuf.read_peek(), tmpbuf.size());
			}
		}
	}
	if(pkDesc)
		pkDesc->SetRelay("");

	return (iExtraLen);
}

struct RawPacketToCharacterFunc
{
	const void * m_buf;
	int	m_buf_len;

	RawPacketToCharacterFunc(const void * buf, int buf_len) : m_buf(buf), m_buf_len(buf_len)
	{
	}

	void operator () (LPCHARACTER c)
	{
		if (!c->GetDesc())
			return;

		c->GetDesc()->Packet(m_buf, m_buf_len);
	}
};

struct FEmpireChatPacket
{
	packet_chat& p;
	const char* orig_msg;
	int orig_len;
	char converted_msg[CHAT_MAX_LEN+1];

	BYTE bEmpire;
	int iMapIndex;
	int namelen;

	FEmpireChatPacket(packet_chat& p, const char* chat_msg, int len, BYTE bEmpire, int iMapIndex, int iNameLen)
		: p(p), orig_msg(chat_msg), orig_len(len), bEmpire(bEmpire), iMapIndex(iMapIndex), namelen(iNameLen)
	{
		memset( converted_msg, 0, sizeof(converted_msg) );
	}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != iMapIndex)
			return;

		d->BufferedPacket(&p, sizeof(packet_chat));

		if (d->GetEmpire() == bEmpire ||
			bEmpire == 0 ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			d->Packet(orig_msg, orig_len);
		}
		else
		{
			size_t len = strlcpy(converted_msg, orig_msg, sizeof(converted_msg));

			if (len >= sizeof(converted_msg))
				len = sizeof(converted_msg) - 1;

			ConvertEmpireText(bEmpire, converted_msg + namelen, len - namelen, 10 + 2 * d->GetCharacter()->GetSkillPower(SKILL_LANGUAGE1 + bEmpire - 1));
			d->Packet(converted_msg, orig_len);
		}
	}
};

struct FYmirChatPacket
{
	packet_chat& packet;
	const char* m_szChat;
	size_t m_lenChat;
	const char* m_szName;
	
	int m_iMapIndex;
	BYTE m_bEmpire;
	bool m_ring;

	char m_orig_msg[CHAT_MAX_LEN+1];
	int m_len_orig_msg;
	char m_conv_msg[CHAT_MAX_LEN+1];
	int m_len_conv_msg;

	FYmirChatPacket(packet_chat& p, const char* chat, size_t len_chat, const char* name, size_t len_name, int iMapIndex, BYTE empire, bool ring)
		: packet(p), m_szChat(chat), m_lenChat(len_chat), m_szName(name), m_iMapIndex(iMapIndex), m_bEmpire(empire), m_ring(ring)
	{
		m_len_orig_msg = snprintf(m_orig_msg, sizeof(m_orig_msg), "%s : %s", m_szName, m_szChat) + 1;

		if (m_len_orig_msg < 0 || m_len_orig_msg >= (int) sizeof(m_orig_msg))
			m_len_orig_msg = sizeof(m_orig_msg) - 1;

		m_len_conv_msg = snprintf(m_conv_msg, sizeof(m_conv_msg), "??? : %s", m_szChat) + 1;

		if (m_len_conv_msg < 0 || m_len_conv_msg >= (int) sizeof(m_conv_msg))
			m_len_conv_msg = sizeof(m_conv_msg) - 1;

		ConvertEmpireText(m_bEmpire, m_conv_msg + 6, m_len_conv_msg - 6, 10);
	}

	void operator() (LPDESC d)
	{
		if (!d->GetCharacter())
			return;

		if (d->GetCharacter()->GetMapIndex() != m_iMapIndex)
			return;

		if (m_ring ||
			d->GetEmpire() == m_bEmpire ||
			d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
			d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			packet.size = m_len_orig_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_orig_msg, m_len_orig_msg);
		}
		else
		{
			packet.size = m_len_conv_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_conv_msg, m_len_conv_msg);
		}
	}
};

int CInputMain::Chat(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGChat* pinfo = reinterpret_cast<const TPacketCGChat*>(data);

	if (uiBytes < pinfo->size)
		return -1;

	const int iExtraLen = pinfo->size - sizeof(TPacketCGChat);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->size, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char buf[CHAT_MAX_LEN - (CHARACTER_NAME_MAX_LEN + 3) + 1];
	strlcpy(buf, data + sizeof(TPacketCGChat), MIN(iExtraLen + 1, sizeof(buf)));
	const size_t buflen = strlen(buf);

	if (buflen > 1 && *buf == '/')
	{
		interpret_command(ch, buf + 1, buflen - 1);
		return iExtraLen;
	}

	if (ch->IncreaseChatCounter() >= 10)
	{
		if (ch->GetChatCounter() == 10)
		{
			sys_log(0, "CHAT_HACK: %s", ch->GetName());
			ch->GetDesc()->DelayedDisconnect(5);
		}

		return iExtraLen;
	}

	const CAffect* pAffect = ch->FindAffect(AFFECT_BLOCK_CHAT);

	if (pAffect != NULL)
	{
		SendBlockChatInfo(ch, pAffect->lDuration);
		return iExtraLen;
	}

	if (true == SpamBlockCheck(ch, buf, buflen))
	{
		return iExtraLen;
	}

	char chatbuf[CHAT_MAX_LEN + 1];
	char szGlobalChat[CHAT_MAX_LEN + 1];

#ifdef ENABLE_GLOBAL_CHAT
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	char szLocaleName[64];

	if (ch->IsGM() && ch->GetCountryFlagFromGMList(1) != 0 && ch->GetCountryFlagFromGMList(2) == LOCALE_MAX_NUM)
		snprintf(szLocaleName, sizeof(szLocaleName), "|Eflag/%s|e", get_locale(ch->GetCountryFlagFromGMList(1)));
	else if (ch->IsGM() && ch->GetCountryFlagFromGMList(2) != 0 && ch->GetCountryFlagFromGMList(2) < LOCALE_MAX_NUM)
		snprintf(szLocaleName, sizeof(szLocaleName), "|Eflag/%s|e |Eflag/%s|e", get_locale(ch->GetCountryFlagFromGMList(1)), get_locale(ch->GetCountryFlagFromGMList(2)));
	else
		snprintf(szLocaleName, sizeof(szLocaleName), "|Eflag/%s|e ", get_locale(ch->GetLanguage()));
#endif

	std::string strEmpireFlagToken;
	switch (ch->GetEmpire())
	{
		case 1: strEmpireFlagToken = "|Eempire/shinsoo|e "; break;
		case 2: strEmpireFlagToken = "|Eempire/chunjo|e "; break;
		case 3: strEmpireFlagToken = "|Eempire/jinno|e "; break;
	}

	if (ch->GetGMLevel() != GM_PLAYER)
	{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		snprintf(szGlobalChat, sizeof(szGlobalChat), "%s |cFFFFA200|H|h[GM]|h|r |Hpm:%s|h%s|h|r : %s", szLocaleName, ch->GetName(), ch->GetName(), buf);
#else
		snprintf(szGlobalChat, sizeof(szGlobalChat), "|cFFFFA200|H|h[GM]|h|r |Hpm:%s|h%s|h|r : %s", ch->GetName(), ch->GetName(), buf);
#endif
	}
#ifdef ENABLE_RENEWAL_PREMIUM_SYSTEM
	else if (ch->IsPremium())
	{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		snprintf(szGlobalChat, sizeof(szGlobalChat), "%s %s |cFFFFC125|H|h[VIP]|h|r |Hpm:%s|h%s|h|r : %s", szLocaleName, strEmpireFlagToken.c_str(), ch->GetName(), ch->GetName(), buf);
#else
		snprintf(szGlobalChat, sizeof(szGlobalChat), "%s |cFFFFC125|H|h[VIP]|h|r |Hpm:%s|h%s|h|r : %s", strEmpireFlagToken.c_str(), ch->GetName(), ch->GetName(), buf);
#endif
	}
#endif
	else
	{
		if (CHAT_TYPE_SHOUT == pinfo->type)
		{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			snprintf(szGlobalChat, sizeof(szGlobalChat), "%s %s |Hpm:%s|h%s|h|r : %s", szLocaleName, strEmpireFlagToken.c_str(), ch->GetName(), ch->GetName(), buf);
#else
			snprintf(szGlobalChat, sizeof(szGlobalChat), "%s |Hpm:%s|h%s|h|r : %s", strEmpireFlagToken.c_str(), ch->GetName(), ch->GetName(), buf);
#endif
		}
		else
		{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			snprintf(szGlobalChat, sizeof(szGlobalChat), "%s %s |Hpm:%s|h%s|h|r : %s", szLocaleName, strEmpireFlagToken.c_str(), ch->GetName(), ch->GetName(), buf);
#else
			snprintf(szGlobalChat, sizeof(szGlobalChat), "%s |Hpm:%s|h%s|h|r : %s", strEmpireFlagToken.c_str(), ch->GetName(), ch->GetName(), buf);
#endif
		}
	}

	int len = snprintf(chatbuf, sizeof(chatbuf), "%s", szGlobalChat);
#else
	int len = snprintf(chatbuf, sizeof(chatbuf), "%s : %s", ch->GetName(), buf);
#endif

	if (CHAT_TYPE_SHOUT == pinfo->type)
	{
		LogManager::instance().ShoutLog(g_bChannel, ch->GetEmpire(), chatbuf);
	}

	CBanwordManager::instance().ConvertString(buf, buflen);

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	int processReturn = ProcessTextTag(ch, chatbuf, len);
	if (0 != processReturn)
	{
		const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

		if (NULL != pTable)
		{
			if (3 == processReturn)
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 46, pTable->szLocaleName);
			else
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 171, "%s",
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
					LC_LOCALE_ITEM(ITEM_PRISM, ch->GetLanguage())
#else
					pTable->szLocaleName
#endif
				);
		}

		return iExtraLen;
	}

	if (pinfo->type == CHAT_TYPE_SHOUT)
	{
		const int SHOUT_LIMIT_LEVEL = 15;

		if (ch->GetLevel() < SHOUT_LIMIT_LEVEL)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 172, "%d", SHOUT_LIMIT_LEVEL);
			return (iExtraLen);
		}

		if (thecore_heart->pulse - (int) ch->GetLastShoutPulse() < passes_per_sec * 15)
			return (iExtraLen);

		ch->SetLastShoutPulse(thecore_heart->pulse);

		TPacketGGShout p;

		p.bHeader = HEADER_GG_SHOUT;
		p.bEmpire = ch->GetEmpire();
		strlcpy(p.szText, chatbuf, sizeof(p.szText));
#ifdef ENABLE_MESSENGER_BLOCK
		strlcpy(p.szName, ch->GetName(), sizeof(p.szName));
#endif

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShout));
 
		SendShout(chatbuf, ch->GetEmpire()
#ifdef ENABLE_MESSENGER_BLOCK
			, ch->GetName()
#endif
		);

		return (iExtraLen);
	}

	TPacketGCChat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = pinfo->type;
	pack_chat.id = ch->GetVID();

	switch (pinfo->type)
	{
		case CHAT_TYPE_TALKING:
			{
				const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

				if (false)
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(), FYmirChatPacket(pack_chat,
								buf, strlen(buf), ch->GetName(), strlen(ch->GetName()), ch->GetMapIndex(), ch->GetEmpire(), ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)));
				}
				else
				{
					std::for_each(c_ref_set.begin(), c_ref_set.end(), FEmpireChatPacket(pack_chat,
								chatbuf, len, (ch->GetGMLevel() > GM_PLAYER || ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)) ? 0 : ch->GetEmpire(), ch->GetMapIndex(), strlen(ch->GetName())));
				}
			}
			break;

		case CHAT_TYPE_PARTY:
			{
				if (!ch->GetParty())
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 173, "");
				else
				{
					TEMP_BUFFER tbuf;
					
					tbuf.write(&pack_chat, sizeof(pack_chat));
					tbuf.write(chatbuf, len);

					RawPacketToCharacterFunc f(tbuf.read_peek(), tbuf.size());
					ch->GetParty()->ForEachOnlineMember(f);
				}
			}
			break;

		case CHAT_TYPE_GUILD:
			{
				if (!ch->GetGuild())
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 174, "");
				else
					ch->GetGuild()->Chat(chatbuf);
			}
			break;

		default:
			sys_err("Unknown chat type %d", pinfo->type);
			break;
	}

	return (iExtraLen);
}

void CInputMain::ItemUse(LPCHARACTER ch, const char * data)
{
	ch->UseItem(((struct command_item_use *) data)->Cell);
}

void CInputMain::ItemToItem(LPCHARACTER ch, const char * pcData)
{
	TPacketCGItemUseToItem * p = (TPacketCGItemUseToItem *) pcData;
	if (ch)
		ch->UseItem(p->Cell, p->TargetCell);
}

void CInputMain::ItemDrop(LPCHARACTER ch, const char * data)
{
	struct command_item_drop * pinfo = (struct command_item_drop *) data;

	if (!ch)
		return;

	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DropItem(pinfo->Cell);
}

void CInputMain::ItemDrop2(LPCHARACTER ch, const char * data)
{
	TPacketCGItemDrop2 * pinfo = (TPacketCGItemDrop2 *) data;

	if (!ch)
		return;
	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DropItem(pinfo->Cell, pinfo->count);
}

#ifdef ENABLE_DESTROY_DIALOG
void CInputMain::ItemDestroy(LPCHARACTER ch, const char * data)
{
	struct command_item_destroy * pinfo = (struct command_item_destroy *) data;

	if (ch)
		ch->DestroyItem(pinfo->Cell);

	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->DestroyItem(pinfo->Cell, pinfo->count);
}
#endif

#ifdef ENABLE_QUICK_SELL_ITEM
void CInputMain::ItemSell(LPCHARACTER ch, const char * data)
{
	TPacketCGItemSell * pinfo = (TPacketCGItemSell *) data;

	if (!ch)
		return;

	if (pinfo->gold > 0)
		ch->DropGold(pinfo->gold);
	else
		ch->SellItem(pinfo->Cell);
}
#endif

void CInputMain::ItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (ch)
		ch->MoveItem(pinfo->Cell, pinfo->CellTo, pinfo->count);
}

void CInputMain::ItemPickup(LPCHARACTER ch, const char * data)
{
	struct command_item_pickup * pinfo = (struct command_item_pickup*) data;
	if (ch)
		ch->PickupItem(pinfo->vid);
}

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
int CInputMain::ItemsPickup(LPCHARACTER ch, const char * c_pData, size_t uiBytes)
{
	TPacketCGItemsPickUp * p = (TPacketCGItemsPickUp *)c_pData;
	
	// Integer overflow kontrolü: count deðerini kontrol et
	if (p->count == 0 || p->count > 1000)
		return -1;
	
	const int iExtraLen = p->count * sizeof(DWORD);

	// Integer overflow kontrolü: iExtraLen negatif veya çok büyük olamaz
	if (iExtraLen < 0 || iExtraLen > (int)uiBytes)
		return -1;

	if (uiBytes < sizeof(TPacketCGItemsPickUp) + iExtraLen)
		return -1;

	const DWORD pickupMode = ch->GetPickUPMode();
	if (IS_SET(pickupMode, AUTOMATIC_PICK_UP_ACTIVATE))
	{
		const DWORD *dwVIDS = (DWORD *)(c_pData + sizeof(TPacketCGItemsPickUp));
		for (int i = 0; i < p->count; i++)
		{
			const DWORD dwVID = *(dwVIDS + i);
			LPITEM item = ITEM_MANAGER::instance().FindByVID(dwVID);

			if (!item || !item->GetSectree() || !item->DistanceValid(ch))
				continue;

			const bool canGet = ch->CheckItemCanGet(item);
			if(canGet)
				ch->PickupItem(dwVID);
		}
	}
	return iExtraLen;
}
#endif


#ifdef ENABLE_QUICK_SLOT_FIX
void CInputMain::QuickslotAdd(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_add * pinfo = (struct command_quickslot_add *) data;
	if(pinfo->slot.type == QUICKSLOT_TYPE_ITEM)
	{
		LPITEM item = NULL;

		TItemPos srcCell(INVENTORY, pinfo->slot.pos);

		if (!(item = ch->GetItem(srcCell)))
			return;

		if (item->GetType() != ITEM_USE && item->GetType() != ITEM_QUEST)
			return;
	}
	ch->SetQuickslot(pinfo->pos, pinfo->slot);
}
#else
void CInputMain::QuickslotAdd(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_add * pinfo = (struct command_quickslot_add *) data;
	ch->SetQuickslot(pinfo->pos, pinfo->slot);
}
#endif

void CInputMain::QuickslotDelete(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_del * pinfo = (struct command_quickslot_del *) data;
	ch->DelQuickslot(pinfo->pos);
}

void CInputMain::QuickslotSwap(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_swap * pinfo = (struct command_quickslot_swap *) data;
	ch->SwapQuickslot(pinfo->pos, pinfo->change_pos);
}

int CInputMain::Messenger(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGMessenger* p = (TPacketCGMessenger*) c_pData;
	
	if (uiBytes < sizeof(TPacketCGMessenger))
		return -1;

	c_pData += sizeof(TPacketCGMessenger);
	uiBytes -= sizeof(TPacketCGMessenger);

	switch (p->subheader)
	{
		case MESSENGER_SUBHEADER_CG_ADD_BY_VID:
			{
				if (uiBytes < sizeof(TPacketCGMessengerAddByVID))
					return -1;

				TPacketCGMessengerAddByVID * p2 = (TPacketCGMessengerAddByVID *) c_pData;
				LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

				if (!ch_companion)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch->IsObserverMode())
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch_companion->IsBlockMode(BLOCK_MESSENGER_INVITE))
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 175, "");
					return sizeof(TPacketCGMessengerAddByVID);
				}

#ifdef ENABLE_BOT_PLAYER
				if (ch_companion->IsBotCharacter())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TXT_2"));
					return sizeof(TPacketCGMessengerAddByVID);
				}
#endif

				LPDESC d = ch_companion->GetDesc();

				if (!d)
					return sizeof(TPacketCGMessengerAddByVID);

				if (ch->GetGMLevel() == GM_PLAYER && ch_companion->GetGMLevel() != GM_PLAYER)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 176, "");
					return sizeof(TPacketCGMessengerAddByVID);
				}

				if (ch->GetDesc() == d)
					return sizeof(TPacketCGMessengerAddByVID);

#ifdef ENABLE_MESSENGER_BLOCK
				if (MessengerManager::instance().IsBlocked(ch->GetName(), ch_companion->GetName()))
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 177, "%s", ch_companion->GetName());
					return sizeof(TPacketCGMessengerAddByVID);
				}

				if (MessengerManager::instance().IsBlocked(ch_companion->GetName(), ch->GetName()))
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 170, "%s", ch_companion->GetName());
					return sizeof(TPacketCGMessengerAddByVID);
				}
#endif

				MessengerManager::instance().RequestToAdd(ch, ch_companion);
			}
			return sizeof(TPacketCGMessengerAddByVID);

		case MESSENGER_SUBHEADER_CG_ADD_BY_NAME:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(name, c_pData, sizeof(name));

				if (ch->GetGMLevel() == GM_PLAYER && gm_get_level(name) != GM_PLAYER)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 176, "");
					return CHARACTER_NAME_MAX_LEN;
				}

				LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

				if (!tch)
#ifdef ENABLE_CROSS_CHANNEL_REQUESTS
					MessengerManager::instance().P2PRequestToAdd_Stage1(ch, name);
#else
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 178, "%s", name);
#endif
				else
				{
					if (tch == ch)
						return CHARACTER_NAME_MAX_LEN;

#ifdef ENABLE_BOT_PLAYER
					if (tch->IsBotCharacter())
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TXT_1"));
						return CHARACTER_NAME_MAX_LEN;
					}
#endif
					if (tch->IsBlockMode(BLOCK_MESSENGER_INVITE) == true)
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 175, "");
					}
					else
					{
#ifdef ENABLE_MESSENGER_BLOCK
						if (MessengerManager::instance().IsBlocked(ch->GetName(), tch->GetName()))
						{
							ch->LocaleChatPacket(CHAT_TYPE_INFO, 177, "%s", tch->GetName());
							return CHARACTER_NAME_MAX_LEN;
						}

						if (MessengerManager::instance().IsBlocked(tch->GetName(), ch->GetName()))
						{
							ch->LocaleChatPacket(CHAT_TYPE_INFO, 170, "%s", tch->GetName());
							return CHARACTER_NAME_MAX_LEN;
						}
#endif
						MessengerManager::instance().RequestToAdd(ch, tch);
					}
				}
			}
			return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_REMOVE:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char char_name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(char_name, c_pData, sizeof(char_name));
				MessengerManager::instance().RemoveFromList(ch->GetName(), char_name);
#ifdef ENABLE_FRIEND_LIST_REMOVE_FIX
				MessengerManager::Instance().RemoveFromList(char_name, ch->GetName());
#endif
			}
			return CHARACTER_NAME_MAX_LEN;

#ifdef ENABLE_MESSENGER_BLOCK
		case MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_VID:
			{
				if (uiBytes < sizeof(TPacketCGMessengerAddBlockByVID))
					return -1;

				const TPacketCGMessengerAddBlockByVID* p2 = (TPacketCGMessengerAddBlockByVID*)c_pData;
				const LPCHARACTER& ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

				if (!ch_companion)
					return sizeof(TPacketCGMessengerAddBlockByVID);

				if (ch->IsObserverMode())
					return sizeof(TPacketCGMessengerAddBlockByVID);

				const LPDESC& d = ch_companion->GetDesc();

				if (!d)
					return sizeof(TPacketCGMessengerAddByVID);

				LPCHARACTER pkPartner = ch->GetMarryPartner();
				if (pkPartner)
				{
					if (ch_companion->GetName() == pkPartner->GetName())
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 179, "");
						return sizeof(TPacketCGMessengerAddBlockByVID);
					}
				}

				if (MessengerManager::instance().IsBlocked(ch->GetName(), ch_companion->GetName()))
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 180, "%s", ch_companion->GetName());
					return sizeof(TPacketCGMessengerAddBlockByVID);
				}

				if (MessengerManager::instance().IsBlocked(ch->GetName(), ch_companion->GetName()))
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 181, "%s", ch_companion->GetName());
					return sizeof(TPacketCGMessengerAddBlockByVID);
				}

				if (ch->GetGMLevel() == GM_PLAYER && ch_companion->GetGMLevel() != GM_PLAYER && !test_server)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 182, "");
					return sizeof(TPacketCGMessengerAddByVID);
				}

				if (ch->GetDesc() == d)
					return sizeof(TPacketCGMessengerAddBlockByVID);

				MessengerManager::instance().AddToBlockList(ch->GetName(), ch_companion->GetName());
			}
		return sizeof(TPacketCGMessengerAddBlockByVID);

		case MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_NAME:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(name, c_pData, sizeof(name));

				if (gm_get_level(name) != GM_PLAYER && !test_server)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 182, "");
					return CHARACTER_NAME_MAX_LEN;
				}

				const LPCHARACTER& tch = CHARACTER_MANAGER::instance().FindPC(name);

				if (!tch)
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 178, "%s", name);
				else
				{
					if (tch == ch)
						return CHARACTER_NAME_MAX_LEN;

					LPCHARACTER partner = ch->GetMarryPartner();
					if (partner)
					{
						if (tch->GetName() == partner->GetName())
						{
							ch->LocaleChatPacket(CHAT_TYPE_INFO, 179, "");
							return CHARACTER_NAME_MAX_LEN;
						}
					}

					if (MessengerManager::instance().IsBlocked(ch->GetName(), tch->GetName()))
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 180, "%s", tch->GetName());
						return CHARACTER_NAME_MAX_LEN;
					}

					if (MessengerManager::instance().IsBlocked(ch->GetName(), tch->GetName()))
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 181, "%s", tch->GetName());
						return CHARACTER_NAME_MAX_LEN;
					}

					MessengerManager::instance().AddToBlockList(ch->GetName(), tch->GetName());
				}
			}
		return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_BLOCK_REMOVE_BLOCK:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
					return -1;

				char char_name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(char_name, c_pData, sizeof(char_name));

				if (!MessengerManager::instance().IsBlocked(ch->GetName(), char_name))
					return CHARACTER_NAME_MAX_LEN;

				MessengerManager::instance().RemoveFromBlockList(ch->GetName(), char_name);
			}
		return CHARACTER_NAME_MAX_LEN;
#endif

#ifdef ENABLE_TELEPORT_TO_A_FRIEND
		case MESSENGER_SUBHEADER_CG_REQUEST_WARP_BY_NAME:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
				{
					return -1;
				}

				char name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(name, c_pData, sizeof(name));

				LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

				if (!tch)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 178, "%s", name);
				}
				else
				{
					if (tch == ch)
					{
						return CHARACTER_NAME_MAX_LEN;
					}

					if (tch->IsBlockMode(BLOCK_WARP_REQUEST))
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 183, "%s", tch->GetName());
						return CHARACTER_NAME_MAX_LEN;
					}

					constexpr std::chrono::duration<double> interval_seconds{ 60.0 };
					const auto elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(CHRONO_steady_clock_now - tch->GetLastWarpRequestTimeNew());
					const auto seconds_left = std::chrono::duration_cast<std::chrono::seconds>(interval_seconds - elapsed_time).count();

					if (seconds_left > 0)
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 184, "%d", seconds_left);
						return CHARACTER_NAME_MAX_LEN;
					}

					tch->ChatPacket(CHAT_TYPE_COMMAND, "RequestWarpToCharacter %s", ch->GetName());
				}
			}
			return CHARACTER_NAME_MAX_LEN;

		case MESSENGER_SUBHEADER_CG_SUMMON_BY_NAME:
			{
				if (uiBytes < CHARACTER_NAME_MAX_LEN)
				{
					return -1;
				}

				char name[CHARACTER_NAME_MAX_LEN + 1];
				strlcpy(name, c_pData, sizeof(name));

				LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

				if (!tch)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 178, "%s", name);
				}
				else
				{
					if (tch == ch)
					{
						return CHARACTER_NAME_MAX_LEN;
					}

					ch->SummonCharacter(tch);
					ch->SetLastWarpRequestTimeNew(CHRONO_steady_clock_now);
				}
			}
			return CHARACTER_NAME_MAX_LEN;
#endif

		default:
			sys_err("CInputMain::Messenger : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

#ifdef ENABLE_STACK_LIMIT
typedef struct fckOFF
{
	BYTE		bySlot;
	WORD		byCount;
} TfckOFF;
#endif

int CInputMain::Shop(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	TPacketCGShop * p = (TPacketCGShop *) data;

	if (uiBytes < sizeof(TPacketCGShop))
		return -1;

	if (test_server)
		sys_log(0, "CInputMain::Shop() ==> SubHeader %d", p->subheader);

	const char * c_pData = data + sizeof(TPacketCGShop);
	uiBytes -= sizeof(TPacketCGShop);

	switch (p->subheader)
	{
		case SHOP_SUBHEADER_CG_END:
			sys_log(1, "INPUT: %s SHOP: END", ch->GetName());
			CShopManager::instance().StopShopping(ch);
			return 0;

		case SHOP_SUBHEADER_CG_BUY:
			{
				if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
					return -1;

				BYTE bPos = *(c_pData + 1);
				sys_log(1, "INPUT: %s SHOP: BUY %d", ch->GetName(), bPos);
				CShopManager::instance().Buy(ch, bPos);
				return (sizeof(BYTE) + sizeof(BYTE));
			}

		case SHOP_SUBHEADER_CG_SELL:
			{
				if (uiBytes < sizeof(BYTE))
					return -1;

				BYTE pos = *c_pData;

				sys_log(0, "INPUT: %s SHOP: SELL", ch->GetName());
				CShopManager::instance().Sell(ch, pos);
				return sizeof(BYTE);
			}

		case SHOP_SUBHEADER_CG_SELL2:
			{
#ifdef ENABLE_STACK_LIMIT
				if (uiBytes < sizeof(TfckOFF))
#else
				if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
#endif
					return -1;

#ifdef ENABLE_STACK_LIMIT
				TfckOFF*p2 = (TfckOFF*)c_pData;
#else
				BYTE pos = *(c_pData++);
				BYTE count = *(c_pData);
#endif

				sys_log(0, "INPUT: %s SHOP: SELL2", ch->GetName());

#ifdef ENABLE_STACK_LIMIT
				CShopManager::instance().Sell(ch, p2->bySlot, p2->byCount);
				return sizeof(TfckOFF);
#else
				CShopManager::instance().Sell(ch, pos, count);
				return sizeof(BYTE) + sizeof(BYTE);
#endif
			}

		default:
			sys_err("CInputMain::Shop : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}

void CInputMain::OnClick(LPCHARACTER ch, const char * data)
{
	struct command_on_click *	pinfo = (struct command_on_click *) data;
	LPCHARACTER			victim;

	if ((victim = CHARACTER_MANAGER::instance().Find(pinfo->vid)))
		victim->OnClick(ch);
	else if (test_server)
	{
		sys_err("CInputMain::OnClick %s.Click.NOT_EXIST_VID[%d]", ch->GetName(), pinfo->vid);
	}
}

void CInputMain::Exchange(LPCHARACTER ch, const char * data)
{
	struct command_exchange * pinfo = (struct command_exchange *) data;
	LPCHARACTER	to_ch = NULL;

	if (!ch->CanHandleItem())
		return;

	int iPulse = thecore_pulse(); 

	if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
	{
		if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			to_ch->LocaleChatPacket(CHAT_TYPE_INFO, 185, "%d", g_nPortalLimitTime);
			return;
		}

		if( true == to_ch->IsDead() )
		{
			return;
		}
	}

	sys_log(0, "CInputMain()::Exchange()  SubHeader %d ", pinfo->sub_header);

	if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 185, "%d", g_nPortalLimitTime);
		return;
	}

	switch (pinfo->sub_header)
	{
		case EXCHANGE_SUBHEADER_CG_START:
			if (!ch->GetExchange())
			{
				if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
				{
					if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 186, "%d", g_nPortalLimitTime);

						if (test_server)
							ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

					if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
					{
						to_ch->LocaleChatPacket(CHAT_TYPE_INFO, 186, "%d", g_nPortalLimitTime);

						if (test_server)
							to_ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, to_ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
						return;
					}

					if (ch->GetGold() >= GOLD_MAX)
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 18, "");

#ifdef ENABLE_GOLD_LIMIT
						sys_err("[OVERFLOG_GOLD] START (%lld) id %u name %s ", ch->GetGold(), ch->GetPlayerID(), ch->GetName());
#else
						sys_err("[OVERFLOG_GOLD] START (%u) id %u name %s ", ch->GetGold(), ch->GetPlayerID(), ch->GetName());
#endif
						return;
					}

					if (to_ch->IsPC())
					{
						if (quest::CQuestManager::instance().GiveItemToPC(ch->GetPlayerID(), to_ch))
						{
							sys_log(0, "Exchange canceled by quest %s %s", ch->GetName(), to_ch->GetName());
							return;
						}
					}

					if (ch->GetMyShop() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
						|| ch->IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
						|| ch->GetTransmutation()
#endif
						)
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 74, "");
						return;
					}

					ch->ExchangeStart(to_ch);
				}
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_ADD:
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddItem(pinfo->Pos, pinfo->arg2);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ITEM_DEL:
			if (ch->GetExchange())
			{
				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->RemoveItem(pinfo->arg1);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ELK_ADD:
			if (ch->GetExchange())
			{
#ifdef ENABLE_GOLD_LIMIT
				const long long nTotalGold = static_cast<long long>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<long long>(pinfo->arg1);
#else
				const int64_t nTotalGold = static_cast<int64_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<int64_t>(pinfo->arg1);
#endif

				if (GOLD_MAX <= nTotalGold)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 187, "");

#ifdef ENABLE_GOLD_LIMIT
					sys_err("[OVERFLOW_GOLD] ELK_ADD (%lld) id %u name %s ",
#else
					sys_err("[OVERFLOW_GOLD] ELK_ADD (%u) id %u name %s ",
#endif
							ch->GetExchange()->GetCompany()->GetOwner()->GetGold(),
							ch->GetExchange()->GetCompany()->GetOwner()->GetPlayerID(),
							ch->GetExchange()->GetCompany()->GetOwner()->GetName());

					return;
				}

				if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
					ch->GetExchange()->AddGold(pinfo->arg1);
			}
			break;

		case EXCHANGE_SUBHEADER_CG_ACCEPT:
			if (ch->GetExchange())
			{
				sys_log(0, "CInputMain()::Exchange() ==> ACCEPT ");
				ch->GetExchange()->Accept(true);
			}

			break;

		case EXCHANGE_SUBHEADER_CG_CANCEL:
			if (ch->GetExchange())
				ch->GetExchange()->Cancel();
			break;
	}
}

void CInputMain::Position(LPCHARACTER ch, const char * data)
{
	struct command_position * pinfo = (struct command_position *) data;

	switch (pinfo->position)
	{
		case POSITION_GENERAL:
			ch->Standup();
			break;

		case POSITION_SITTING_CHAIR:
			ch->Sitdown(0);
			break;

		case POSITION_SITTING_GROUND:
			ch->Sitdown(1);
			break;
	}
}

static const int ComboSequenceBySkillLevel[3][8] = 
{
	{ 14, 15, 16, 17,  0,  0,  0,  0 },
	{ 14, 15, 16, 18, 20,  0,  0,  0 },
	{ 14, 15, 16, 18, 19, 17,  0,  0 },
};

#define COMBO_HACK_ALLOWABLE_MS	100

DWORD ClacValidComboInterval( LPCHARACTER ch, BYTE bArg )
{
	int nInterval = 300;
	float fAdjustNum = 1.5f;

	if( !ch )
	{
		sys_err( "ClacValidComboInterval() ch is NULL");
		return nInterval;
	}	

	if( bArg == 13 )
	{
		float normalAttackDuration = CMotionManager::instance().GetNormalAttackDuration(ch->GetRaceNum());
		nInterval = (int) (normalAttackDuration / (((float) ch->GetPoint(POINT_ATT_SPEED) / 100.f) * 900.f) + fAdjustNum );
	}
	else if( bArg == 14 )
	{		
		nInterval = (int)(ani_combo_speed(ch, 1 ) / ((ch->GetPoint(POINT_ATT_SPEED) / 100.f) + fAdjustNum) );
	}
	else if( bArg > 14 && bArg << 22 )
	{
		nInterval = (int)(ani_combo_speed(ch, bArg - 13 ) / ((ch->GetPoint(POINT_ATT_SPEED) / 100.f) + fAdjustNum) );
	}
	else
	{
		sys_err( "ClacValidComboInterval() Invalid bArg(%d) ch(%s)", bArg, ch->GetName() );		
	}

	return nInterval;
}

bool CheckComboHack(LPCHARACTER ch, BYTE bArg, DWORD dwTime, bool CheckSpeedHack)
{
	if (ch->IsStun() || ch->IsDead())
		return false;

	int ComboInterval = dwTime - ch->GetLastComboTime();
	int HackScalar = 0;

#if 0
	sys_log(0, "COMBO: %s arg:%u seq:%u delta:%d checkspeedhack:%d",
			ch->GetName(), bArg, ch->GetComboSequence(), ComboInterval - ch->GetValidComboInterval(), CheckSpeedHack);
#endif
	if (bArg == 14)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
		{
		}

		ch->SetComboSequence(1);
		ch->SetValidComboInterval( ClacValidComboInterval(ch, bArg) );
		ch->SetLastComboTime(dwTime);
	}
	else if (bArg > 14 && bArg < 22)
	{
		int idx = MIN(2, ch->GetComboIndex());

		if (ch->GetComboSequence() > 5)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);
			sys_log(0, "COMBO_HACK: 5 %s combo_seq:%d", ch->GetName(), ch->GetComboSequence());
		}
		else if (bArg == 21 &&
				 idx == 2 &&
				 ch->GetComboSequence() == 5 &&
				 ch->GetJob() == JOB_ASSASSIN &&
				 ch->GetWear(WEAR_WEAPON) &&
				 ch->GetWear(WEAR_WEAPON)->GetSubType() == WEAPON_DAGGER)
			ch->SetValidComboInterval(300);
		else if (ComboSequenceBySkillLevel[idx][ch->GetComboSequence()] != bArg)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);

			sys_log(0, "COMBO_HACK: 3 %s arg:%u valid:%u combo_idx:%d combo_seq:%d",
					ch->GetName(),
					bArg,
					ComboSequenceBySkillLevel[idx][ch->GetComboSequence()],
					idx,
					ch->GetComboSequence());
		}
		else
		{
			if (CheckSpeedHack && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
			{
				HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 100;

				sys_log(0, "COMBO_HACK: 2 %s arg:%u interval:%d valid:%u atkspd:%u riding:%s",
						ch->GetName(),
						bArg,
						ComboInterval,
						ch->GetValidComboInterval(),
						ch->GetPoint(POINT_ATT_SPEED),
						ch->IsRiding() ? "yes" : "no");
			}

			if (ch->IsRiding())
				ch->SetComboSequence(ch->GetComboSequence() == 1 ? 2 : 1);
			else
				ch->SetComboSequence(ch->GetComboSequence() + 1);

			ch->SetValidComboInterval( ClacValidComboInterval(ch, bArg) );
			ch->SetLastComboTime(dwTime);
		}
	}
	else if (bArg == 13)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS) {}

		if (ch->GetRaceNum() >= MAIN_RACE_MAX_NUM)
		{
			ch->SetValidComboInterval( ClacValidComboInterval(ch, bArg) );
			ch->SetLastComboTime(dwTime);
		}
		else
		{
		}
	}
	else
	{
		if (ch->GetDesc()->DelayedDisconnect(number(2, 9)))
		{
			LogManager::instance().HackLog("Hacker", ch);
			sys_log(0, "HACKER: %s arg %u", ch->GetName(), bArg);
		}

		HackScalar = 10;
		ch->SetValidComboInterval(300);
	}

	if (HackScalar)
	{
		if (get_dword_time() - ch->GetLastMountTime() > 1500)
			ch->IncreaseComboHackCount(1 + HackScalar);

		ch->SkipComboAttackByTime(ch->GetValidComboInterval());
	}

	return HackScalar;
}

void CInputMain::Move(LPCHARACTER ch, const char * data)
{
	if (!ch->CanMove())
		return;

	struct command_move * pinfo = (struct command_move *) data;

	if (pinfo->bFunc >= FUNC_MAX_NUM && !(pinfo->bFunc & 0x80))
	{
		sys_err("invalid move type: %s", ch->GetName());
		return;
	}

	const float fDist = DISTANCE_SQRT((ch->GetX() - pinfo->lX) / 100, (ch->GetY() - pinfo->lY) / 100);

	if (((false == ch->IsRiding() && fDist > 25) || fDist > 60) && OXEVENT_MAP_INDEX != ch->GetMapIndex())
	{
		sys_log(0, "MOVE: %s trying to move too far (dist: %.1fm) Riding(%d)", ch->GetName(), fDist, ch->IsRiding());

		ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
		ch->Stop();
		return;
	}

	DWORD dwCurTime = get_dword_time();
	bool CheckSpeedHack = (false == ch->GetDesc()->IsHandshaking() && dwCurTime - ch->GetDesc()->GetClientTime() > 7000);

	if (CheckSpeedHack)
	{
		int iDelta = (int) (pinfo->dwTime - ch->GetDesc()->GetClientTime());
		int iServerDelta = (int) (dwCurTime - ch->GetDesc()->GetClientTime());

		iDelta = (int) (dwCurTime - pinfo->dwTime);

		if (iDelta >= 30000)
		{
			sys_log(0, "SPEEDHACK: slow timer name %s delta %d", ch->GetName(), iDelta);
		}
		else if (iDelta < -(iServerDelta / 50))
		{
			sys_log(0, "SPEEDHACK: DETECTED! %s (delta %d %d)", ch->GetName(), iDelta, iServerDelta);
		}
	}

	if (pinfo->bFunc == FUNC_COMBO && g_bCheckMultiHack)
	{
		CheckComboHack(ch, pinfo->bArg, pinfo->dwTime, CheckSpeedHack);
	}

	if (pinfo->bFunc == FUNC_MOVE)
	{
		if (ch->GetLimitPoint(POINT_MOV_SPEED) == 0)
			return;

		ch->SetRotation(pinfo->bRot * 5);
		ch->ResetStopTime();

		ch->Goto(pinfo->lX, pinfo->lY);
	}
	else
	{
		if (pinfo->bFunc == FUNC_ATTACK || pinfo->bFunc == FUNC_COMBO)
			ch->OnMove(true);
		else if (pinfo->bFunc & FUNC_SKILL)
		{
			const int MASK_SKILL_MOTION = 0x7F;
			unsigned int motion = pinfo->bFunc & MASK_SKILL_MOTION;

			if (!ch->IsUsableSkillMotion(motion))
			{
				const char* name = ch->GetName();
				unsigned int job = ch->GetJob();
				unsigned int group = ch->GetSkillGroup();

				char szBuf[256];
				snprintf(szBuf, sizeof(szBuf), "SKILL_HACK: name=%s, job=%d, group=%d, motion=%d", name, job, group, motion);
				LogManager::instance().HackLog(szBuf, ch->GetDesc()->GetAccountTable().login, ch->GetName(), ch->GetDesc()->GetHostName());
				sys_log(0, "%s", szBuf);

				if (test_server)
				{
					ch->GetDesc()->DelayedDisconnect(number(2, 8));
					ch->ChatPacket(CHAT_TYPE_INFO, szBuf);
				}
				else
				{
					ch->GetDesc()->DelayedDisconnect(number(150, 500));
				}
			}

			ch->OnMove();
		}

		ch->SetRotation(pinfo->bRot * 5);
		ch->ResetStopTime();

		ch->Move(pinfo->lX, pinfo->lY);
		ch->Stop();
		ch->StopStaminaConsume();
	}

	TPacketGCMove pack;

	pack.bHeader = HEADER_GC_MOVE;
	pack.bFunc = pinfo->bFunc;
	pack.bArg = pinfo->bArg;
	pack.bRot = pinfo->bRot;
	pack.dwVID = ch->GetVID();
	pack.lX = pinfo->lX;
	pack.lY = pinfo->lY;
	pack.dwTime = pinfo->dwTime;
	pack.dwDuration = (pinfo->bFunc == FUNC_MOVE) ? ch->GetCurrentMoveDuration() : 0;

	ch->PacketAround(&pack, sizeof(TPacketGCMove), ch);
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
void CInputMain::SetSkillColor(LPCHARACTER ch, const char* pcData)
{
	if (!ch)
		return;

	TPacketCGSkillColor* CGPacket = (TPacketCGSkillColor*)pcData;

	// Array bounds kontrolü: bSkillSlot deðeri geçerli aralýkta olmalý
	// Array boyutu: MAX_SKILL_COUNT + MAX_BUFF_COUNT
	if (CGPacket->bSkillSlot >= (ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT))
		return;

	if (GetLastSortTime() > get_global_time())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetLastSortTime() - get_global_time());
		return;
	}

	DWORD dwData[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
	memcpy(dwData, ch->GetSkillColor(), sizeof(dwData));

	// Array bounds kontrolü ile güvenli eriþim (zaten yukarýda kontrol edildi ama ekstra güvenlik için)
	if (CGPacket->bSkillSlot < (ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT))
	{
		// Ýkinci boyut kontrolü: MAX_EFFECT_COUNT kontrolü (5 deðer yazýlýyor, index 0-4)
		if (ESkillColorLength::MAX_EFFECT_COUNT >= 5)
		{
			dwData[CGPacket->bSkillSlot][0] = CGPacket->dwCol1;
			dwData[CGPacket->bSkillSlot][1] = CGPacket->dwCol2;
			dwData[CGPacket->bSkillSlot][2] = CGPacket->dwCol3;
			dwData[CGPacket->bSkillSlot][3] = CGPacket->dwCol4;
			dwData[CGPacket->bSkillSlot][4] = CGPacket->dwCol5;
		}
	}

	ch->LocaleChatPacket(CHAT_TYPE_INFO, 188, "");

	ch->SetSkillColor(dwData[0]);

	TSkillColor GDPacket;
	memcpy(GDPacket.dwSkillColor, dwData, sizeof(dwData));
	GDPacket.dwPlayerID = ch->GetPlayerID();
	db_clientdesc->DBPacketHeader(HEADER_GD_SKILL_COLOR_SAVE, 0, sizeof(TSkillColor));
	db_clientdesc->Packet(&GDPacket, sizeof(TSkillColor));
	SetLastSortTime(get_global_time() + 15);
}
#endif

void CInputMain::Attack(LPCHARACTER ch, const BYTE header, const char* data)
{
	if (NULL == ch)
		return;

	struct type_identifier
	{
		BYTE header;
		BYTE type;
	};

	const struct type_identifier* const type = reinterpret_cast<const struct type_identifier*>(data);

	if (type->type > 0)
	{
		if (false == ch->CanUseSkill(type->type))
		{
			return;
		}

		switch (type->type)
		{
			case SKILL_GEOMPUNG:
			case SKILL_SANGONG:
			case SKILL_YEONSA:
			case SKILL_KWANKYEOK:
			case SKILL_HWAJO:
			case SKILL_GIGUNG:
			case SKILL_PABEOB:
			case SKILL_MARYUNG:
			case SKILL_TUSOK:
			case SKILL_MAHWAN:
			case SKILL_BIPABU:
			case SKILL_NOEJEON:
			case SKILL_CHAIN:
			case SKILL_HORSE_WILDATTACK_RANGE:
				if (HEADER_CG_SHOOT != type->header)
				{
					if (test_server)
						ch->ChatPacket(CHAT_TYPE_INFO, "Attack :name[%s] Vnum[%d] can't use skill by attack(warning)", type->type);
					return;
				}
				break;
		}
	}

	switch (header)
	{
		case HEADER_CG_ATTACK:
			{
				if (NULL == ch->GetDesc())
					return;

				const TPacketCGAttack* const packMelee = reinterpret_cast<const TPacketCGAttack*>(data);

				ch->GetDesc()->AssembleCRCMagicCube(packMelee->bCRCMagicCubeProcPiece, packMelee->bCRCMagicCubeFilePiece);

				LPCHARACTER	victim = CHARACTER_MANAGER::instance().Find(packMelee->dwVID);

				if (NULL == victim || ch == victim)
					return;

				switch (victim->GetCharType())
				{
					case CHAR_TYPE_NPC:
					case CHAR_TYPE_WARP:
					case CHAR_TYPE_GOTO:
#ifdef ENABLE_PET_SYSTEM
					case CHAR_TYPE_PET:
#endif
#ifdef ENABLE_MOUNT_SYSTEM
					case CHAR_TYPE_MOUNT:
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
					case CHAR_TYPE_GROWTH_PET:
#endif
						return;
				}

				if (packMelee->bType > 0)
				{
					if (false == ch->CheckSkillHitCount(packMelee->bType, victim->GetVID()))
					{
						return;
					}
				}

				ch->Attack(victim, packMelee->bType);
			}
			break;

		case HEADER_CG_SHOOT:
			{
				const TPacketCGShoot* const packShoot = reinterpret_cast<const TPacketCGShoot*>(data);

				ch->Shoot(packShoot->bType);
			}
			break;
	}
}

int CInputMain::SyncPosition(LPCHARACTER ch, const char * c_pcData, size_t uiBytes)
{
	const TPacketCGSyncPosition* pinfo = reinterpret_cast<const TPacketCGSyncPosition*>( c_pcData );

	if (uiBytes < pinfo->wSize)
		return -1;

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGSyncPosition);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (0 != (iExtraLen % sizeof(TPacketCGSyncPositionElement)))
	{
		sys_err("invalid packet length %d (name: %s)", pinfo->wSize, ch->GetName());
		return iExtraLen;
	}

	int iCount = iExtraLen / sizeof(TPacketCGSyncPositionElement);

	if (iCount <= 0)
		return iExtraLen;

	static const int nCountLimit = 16;

	if( iCount > nCountLimit )
	{
		//LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );
		sys_err( "Too many SyncPosition Count(%d) from Name(%s)", iCount, ch->GetName() );
		//ch->GetDesc()->SetPhase(PHASE_CLOSE);
		//return -1;
		iCount = nCountLimit;
	}

	TEMP_BUFFER tbuf;
	LPBUFFER lpBuf = tbuf.getptr();

	TPacketGCSyncPosition * pHeader = (TPacketGCSyncPosition *) buffer_write_peek(lpBuf);
	buffer_write_proceed(lpBuf, sizeof(TPacketGCSyncPosition));

	const TPacketCGSyncPositionElement* e =
		reinterpret_cast<const TPacketCGSyncPositionElement*>(c_pcData + sizeof(TPacketCGSyncPosition));

	timeval tvCurTime;
	gettimeofday(&tvCurTime, NULL);

	for (int i = 0; i < iCount; ++i, ++e)
	{
		LPCHARACTER victim = CHARACTER_MANAGER::instance().Find(e->dwVID);

		if (!victim)
			continue;

		switch (victim->GetCharType())
		{
			case CHAR_TYPE_NPC:
			case CHAR_TYPE_WARP:
			case CHAR_TYPE_GOTO:
#ifdef ENABLE_PET_SYSTEM
			case CHAR_TYPE_PET:
#endif
#ifdef ENABLE_MOUNT_SYSTEM
			case CHAR_TYPE_MOUNT:
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
			case CHAR_TYPE_GROWTH_PET:
#endif
				continue;
		}

		// Ownership check
		if (!victim->SetSyncOwner(ch))
			continue;

		const float fDistWithSyncOwner = DISTANCE_SQRT( (victim->GetX() - ch->GetX()) / 100, (victim->GetY() - ch->GetY()) / 100 );
		static const float fLimitDistWithSyncOwner = 2500.f + 1000.f;
		// If the distance to the victim is more than 2500 + a, it is considered a nucleus.
		// distance reference: client's __GetSkillTargetRange, __GetBowRange functions
		// 2500: The range of the skill with the longest range in the skill proto, or the range of the bow
		// a = POINT_BOW_DISTANCE value... but I don't know if it is actually used. There are no items, potions, skills, or quests...
		// Even so, in case you want to use it as a buffer, set it to 1000.f...
		if (fDistWithSyncOwner > fLimitDistWithSyncOwner)
		{
			// Look up to g_iSyncHackLimitCount.
			if (ch->GetSyncHackCount() < g_iSyncHackLimitCount)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too far SyncPosition DistanceWithSyncOwner(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
					fDistWithSyncOwner, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );
#ifndef FIX_SyncPosition
				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
#endif
			}
		}

		const float fDist = DISTANCE_SQRT( (victim->GetX() - e->lX) / 100, (victim->GetY() - e->lY) / 100 );
		static const long g_lValidSyncInterval = 100 * 1000; // 100ms
		const timeval &tvLastSyncTime = victim->GetLastSyncTime();
		timeval *tvDiff = timediff(&tvCurTime, &tvLastSyncTime);

		if (tvDiff->tv_sec == 0 && tvDiff->tv_usec < g_lValidSyncInterval)
		{
			if (ch->GetSyncHackCount() < g_iSyncHackLimitCount)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too often SyncPosition Interval(%ldms)(%s) from Name(%s) VICTIM(%d,%d) SYNC(%d,%d)",
					tvDiff->tv_sec * 1000 + tvDiff->tv_usec / 1000, victim->GetName(), ch->GetName(), victim->GetX(), victim->GetY(),
					e->lX, e->lY );

#ifndef FIX_SyncPosition
				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
#endif
			}
		}
		else if( fDist > 25.0f )
		{
			LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

			sys_err( "Too far SyncPosition Distance(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
				   	fDist, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
				  e->lX, e->lY );

#ifndef FIX_SyncPosition
				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
#endif
		}
		else
		{
			victim->SetLastSyncTime(tvCurTime);
			victim->Sync(e->lX, e->lY);
			buffer_write(lpBuf, e, sizeof(TPacketCGSyncPositionElement));
		}
	}

	if (buffer_size(lpBuf) != sizeof(TPacketGCSyncPosition))
	{
		pHeader->bHeader = HEADER_GC_SYNC_POSITION;
		pHeader->wSize = buffer_size(lpBuf);

		ch->PacketAround(buffer_read_peek(lpBuf), buffer_size(lpBuf), ch);
	}

	return iExtraLen;
}

void CInputMain::FlyTarget(LPCHARACTER ch, const char * pcData, BYTE bHeader)
{
	TPacketCGFlyTargeting * p = (TPacketCGFlyTargeting *) pcData;
	ch->FlyTarget(p->dwTargetVID, p->x, p->y, bHeader);
}

void CInputMain::UseSkill(LPCHARACTER ch, const char * pcData)
{
	TPacketCGUseSkill * p = (TPacketCGUseSkill *) pcData;
	ch->UseSkill(p->dwVnum, CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::ScriptButton(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptButton * p = (TPacketCGScriptButton *) c_pData;
	sys_log(0, "QUEST ScriptButton pid %d idx %u", ch->GetPlayerID(), p->idx);

	quest::PC* pc = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (pc && pc->IsConfirmWait())
	{
		quest::CQuestManager::instance().Confirm(ch->GetPlayerID(), quest::CONFIRM_TIMEOUT);
	}
	else if (p->idx & 0x80000000)
	{
		quest::CQuestManager::Instance().QuestInfo(ch->GetPlayerID(), p->idx & 0x7fffffff);
	}
	else
	{
		quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), p->idx);
	}
}

void CInputMain::ScriptAnswer(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptAnswer * p = (TPacketCGScriptAnswer *) c_pData;
	sys_log(0, "QUEST ScriptAnswer pid %d answer %d", ch->GetPlayerID(), p->answer);

	if (p->answer > 250)
	{
		quest::CQuestManager::Instance().Resume(ch->GetPlayerID());
	}
	else
	{
		quest::CQuestManager::Instance().Select(ch->GetPlayerID(),  p->answer);
	}
}

// SCRIPT_SELECT_ITEM
void CInputMain::ScriptSelectItem(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptSelectItem* p = (TPacketCGScriptSelectItem*) c_pData;
	sys_log(0, "QUEST ScriptSelectItem pid %d answer %d", ch->GetPlayerID(), p->selection);
	quest::CQuestManager::Instance().SelectItem(ch->GetPlayerID(), p->selection);
}
// END_OF_SCRIPT_SELECT_ITEM

void CInputMain::QuestInputString(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestInputString * p = (TPacketCGQuestInputString*) c_pData;

	char msg[65];
	strlcpy(msg, p->msg, sizeof(msg));
	sys_log(0, "QUEST InputString pid %u msg %s", ch->GetPlayerID(), msg);

	quest::CQuestManager::Instance().Input(ch->GetPlayerID(), msg);
}

void CInputMain::QuestConfirm(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestConfirm* p = (TPacketCGQuestConfirm*) c_pData;
	LPCHARACTER ch_wait = CHARACTER_MANAGER::instance().FindByPID(p->requestPID);
	if (p->answer)
		p->answer = quest::CONFIRM_YES;
	sys_log(0, "QuestConfirm from %s pid %u name %s answer %d", ch->GetName(), p->requestPID, (ch_wait)?ch_wait->GetName():"", p->answer);
	if (ch_wait)
	{
		quest::CQuestManager::Instance().Confirm(ch_wait->GetPlayerID(), (quest::EQuestConfirmType) p->answer, ch->GetPlayerID());
	}
}

void CInputMain::Target(LPCHARACTER ch, const char * pcData)
{
	TPacketCGTarget * p = (TPacketCGTarget *) pcData;

	building::LPOBJECT pkObj = building::CManager::instance().FindObjectByVID(p->dwVID);

	if (pkObj)
	{
		TPacketGCTarget pckTarget;
		pckTarget.header = HEADER_GC_TARGET;
		pckTarget.dwVID = p->dwVID;
		ch->GetDesc()->Packet(&pckTarget, sizeof(TPacketGCTarget));
	}
	else
		ch->SetTarget(CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::Warp(LPCHARACTER ch, const char * pcData)
{
	ch->WarpEnd();
}

void CInputMain::SafeboxCheckin(LPCHARACTER ch, const char * c_pData)
{
	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
		return;

	TPacketCGSafeboxCheckin * p = (TPacketCGSafeboxCheckin *) c_pData;

	if (!ch->CanHandleItem())
		return;

	CSafebox * pkSafebox = ch->GetSafebox();
	LPITEM pkItem = ch->GetItem(p->ItemPos);

	if (!pkSafebox || !pkItem)
		return;

#ifdef ENABLE_EQUIPPED_ITEM_STORAGE_FIX
	if (true == pkItem->IsEquipped())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 788, "");
		return;
	}
#endif

	if ((pkItem->GetType() == ITEM_ARMOR && pkItem->GetSubType() == ARMOR_BELT) && pkItem->IsEquipped()) // Fix
		return;

	if ((ITEM_ARMOR == pkItem->GetType() && ARMOR_BELT == pkItem->GetSubType()) && CBeltInventoryHelper::IsExistItemInBeltInventory(ch)) // Fix
		return;

	if (ch->GetItem(p->ItemPos) && ch->GetItem(p->ItemPos)->IsEquipped())
		return;

	if (pkItem->GetCell() >= INVENTORY_MAX_NUM && IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 189, "");
		return;
	}

	if (!pkSafebox->IsEmpty(p->bSafePos, pkItem->GetSize()))
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 189, "");
		return;
	}

	if (pkItem->GetVnum() == UNIQUE_ITEM_SAFEBOX_EXPAND)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 190, "");
		return;
	}

	if (IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_SAFEBOX))
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 190, "");
		return;
	}

	if (true == pkItem->isLocked())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 190, "");
		return;
	}

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	if (pkItem->IsEquipped())
	{
		int iWearCell = pkItem->FindEquipCell(ch);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !ch->UnequipItem(costumeWeapon))
			{
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
				return;
			}
		}
	}
#endif

	pkItem->RemoveFromCharacter();

	if (!pkItem->IsDragonSoul())
		ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, p->ItemPos.cell, 255);

	pkSafebox->Add(p->bSafePos, pkItem);

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (pkItem->GetType() == ITEM_GROWTH_PET)
	{
		if (pkItem->GetSubType() == PET_UPBRINGING || pkItem->GetSubType() == PET_BAG)
		{
			LPGROWTH_PET pPet = ch->GetGrowthPet(pkItem->GetSocket(2));
			if (pPet)
				pkSafebox->AddPet(pPet);
		}
	}
#endif

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX PUT", szHint);
}

void CInputMain::SafeboxCheckout(LPCHARACTER ch, const char * c_pData, bool bMall)
{
	TPacketCGSafeboxCheckout * p = (TPacketCGSafeboxCheckout *) c_pData;

	if (!ch->CanHandleItem())
		return;

	CSafebox * pkSafebox;

	if (bMall)
		pkSafebox = ch->GetMall();
	else
		pkSafebox = ch->GetSafebox();

	if (!pkSafebox)
		return;

	LPITEM pkItem = pkSafebox->Get(p->bSafePos);

	if (!pkItem)
		return;

	if (!ch->IsEmptyItemGrid(p->ItemPos, pkItem->GetSize()))
		return;

#ifdef ENABLE_SPECIAL_INVENTORY
	if (p->ItemPos.IsSkillBookInventoryPosition() && !pkItem->IsSkillBook())
		return;

	if (p->ItemPos.IsUpgradeItemsInventoryPosition() && !pkItem->IsUpgradeItem())
		return;

	if (p->ItemPos.IsStoneInventoryPosition() && !pkItem->IsStone())
		return;

	if (p->ItemPos.IsGiftBoxInventoryPosition() && !pkItem->IsGiftBox())
		return;

	if (p->ItemPos.IsChangersInventoryPosition() && !pkItem->IsChanger())
		return;
#endif

	if (pkItem->IsDragonSoul())
	{
		if (bMall)
		{
			DSManager::instance().DragonSoulItemInitialize(pkItem);
		}

		if (DRAGON_SOUL_INVENTORY != p->ItemPos.window_type)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 189, "");
			return;
		}

		TItemPos DestPos = p->ItemPos;
		if (!DSManager::instance().IsValidCellForThisItem(pkItem, DestPos))
		{
			int iCell = ch->GetEmptyDragonSoulInventory(pkItem);
			if (iCell < 0)
			{
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 189, "");
				return ;
			}
			DestPos = TItemPos (DRAGON_SOUL_INVENTORY, iCell);
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, DestPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
	else
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 189, "");
			return;
		}

		pkSafebox->Remove(p->bSafePos);
		if (bMall)
		{
			if (NULL == pkItem->GetProto())
			{
				sys_err ("pkItem->GetProto() == NULL (id : %d)",pkItem->GetID());
				return ;
			}

			if (100 == pkItem->GetProto()->bAlterToMagicItemPct && 0 == pkItem->GetAttributeCount())
			{
				pkItem->AlterToMagicItem();
			}
		}
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);

#ifdef ENABLE_GROWTH_PET_SYSTEM
		if (pkItem->GetType() == ITEM_GROWTH_PET)
		{
			if (pkItem->GetSubType() == PET_UPBRINGING || pkItem->GetSubType() == PET_BAG)
				pkSafebox->RemovePet(pkItem);
		}
#endif
	}

	DWORD dwID = pkItem->GetID();
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_FLUSH, 0, sizeof(DWORD));
	db_clientdesc->Packet(&dwID, sizeof(DWORD));

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	if (bMall)
		LogManager::instance().ItemLog(ch, pkItem, "MALL GET", szHint);
	else
		LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX GET", szHint);
}

void CInputMain::SafeboxItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (!ch->CanHandleItem())
		return;

	if (!ch->GetSafebox())
		return;

	ch->GetSafebox()->MoveItem(pinfo->Cell.cell, pinfo->CellTo.cell, pinfo->count);
}

void CInputMain::PartyInvite(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 192, "");
		return;
	}

	TPacketCGPartyInvite * p = (TPacketCGPartyInvite*) c_pData;

	LPCHARACTER pInvitee = CHARACTER_MANAGER::instance().Find(p->vid);

	if (!pInvitee || !ch->GetDesc() || !pInvitee->GetDesc())
	{
		sys_err("PARTY Cannot find invited character");
		return;
	}

#ifdef ENABLE_MESSENGER_BLOCK
	if (MessengerManager::instance().IsBlocked(ch->GetName(), pInvitee->GetName()))
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 177, "%s", pInvitee->GetName());
		return;
	}
	else if (MessengerManager::instance().IsBlocked(pInvitee->GetName(), ch->GetName()))
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 170, "%s", pInvitee->GetName());
		return;
	}
#endif

#ifdef ENABLE_BOT_PLAYER
	if (pInvitee->IsBotCharacter())
		return;
#endif

	ch->PartyInvite(pInvitee);
}

void CInputMain::PartyInviteAnswer(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 192, "");
		return;
	}

	TPacketCGPartyInviteAnswer * p = (TPacketCGPartyInviteAnswer*) c_pData;

	LPCHARACTER pInviter = CHARACTER_MANAGER::instance().Find(p->leader_vid);

	if (!pInviter)
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 55, "");
	else if (!p->accept)
		pInviter->PartyInviteDeny(ch->GetPlayerID());
	else
		pInviter->PartyInviteAccept(ch);
}

void CInputMain::PartySetState(LPCHARACTER ch, const char* c_pData)
{
	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 30, "");
		return;
	}

	TPacketCGPartySetState* p = (TPacketCGPartySetState*) c_pData;

	if (!ch->GetParty())
		return;

	if (ch->GetParty()->GetLeaderPID() != ch->GetPlayerID())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 193, "");
		return;
	}

	if (!ch->GetParty()->IsMember(p->pid))
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 39, "");
		return;
	}

	DWORD pid = p->pid;
	sys_log(0, "PARTY SetRole pid %d to role %d state %s", pid, p->byRole, p->flag ? "on" : "off");

	switch (p->byRole)
	{
		case PARTY_ROLE_NORMAL:
			break;

		case PARTY_ROLE_ATTACKER:
		case PARTY_ROLE_TANKER:
		case PARTY_ROLE_BUFFER:
		case PARTY_ROLE_SKILL_MASTER:
		case PARTY_ROLE_HASTE:
		case PARTY_ROLE_DEFENDER:
			if (ch->GetParty()->SetRole(pid, p->byRole, p->flag))
			{
				TPacketPartyStateChange pack;
				pack.dwLeaderPID = ch->GetPlayerID();
				pack.dwPID = p->pid;
				pack.bRole = p->byRole;
				pack.bFlag = p->flag;
				db_clientdesc->DBPacket(HEADER_GD_PARTY_STATE_CHANGE, 0, &pack, sizeof(pack));
			}
			break;

		default:
			sys_err("wrong byRole in PartySetState Packet name %s state %d", ch->GetName(), p->byRole);
			break;
	}
}

void CInputMain::PartyRemove(LPCHARACTER ch, const char* c_pData)
{
	if (ch->GetArena())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 192, "");
		return;
	}

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 30, "");
		return;
	}

	if (ch->GetDungeon())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 194, "");
		return;
	}

	TPacketCGPartyRemove* p = (TPacketCGPartyRemove*) c_pData;

	if (!ch->GetParty())
		return;

	LPPARTY pParty = ch->GetParty();
	if (pParty->GetLeaderPID() == ch->GetPlayerID())
	{
		if (ch->GetDungeon())
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 194, "");
		}
		else
		{
			if (p->pid == ch->GetPlayerID() || pParty->GetMemberCount() == 2)
			{
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
				LPCHARACTER B = CHARACTER_MANAGER::instance().FindByPID(p->pid);
				if (B)
				{
					B->LocaleChatPacket(CHAT_TYPE_INFO, 195, "");
				}
				pParty->Quit(p->pid);
			}
		}
	}
	else
	{
		if (p->pid == ch->GetPlayerID())
		{
			if (ch->GetDungeon())
			{
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 196, "");
			}
			else
			{
				if (pParty->GetMemberCount() == 2)
				{
					CPartyManager::instance().DeleteParty(pParty);
				}
				else
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 197, "");
					pParty->Quit(ch->GetPlayerID());
				}
			}
		}
		else
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 198, "");
		}
	}
}

void CInputMain::PartyUseSkill(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGPartyUseSkill* p = (TPacketCGPartyUseSkill*) c_pData;
	if (!ch->GetParty())
		return;

	if (ch->GetPlayerID() != ch->GetParty()->GetLeaderPID())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 203, "");
		return;
	}

	switch (p->bySkillIndex)
	{
		case PARTY_SKILL_HEAL:
			ch->GetParty()->HealParty();
			break;
		case PARTY_SKILL_WARP:
			{
				if (ch->GetSkillLevel(SKILL_LEADERSHIP) >= 10)
				{
					LPCHARACTER pch = CHARACTER_MANAGER::instance().Find(p->vid);
					if (pch)
					{
						ch->GetParty()->SummonToLeader(pch->GetPlayerID()); 
					}
					else
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 128, "");
					}
				}
				else
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 204, "");
				}
			}
			break;
	}
}

void CInputMain::PartyParameter(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGPartyParameter * p = (TPacketCGPartyParameter *) c_pData;

	if (ch->GetParty())
		ch->GetParty()->SetParameter(p->bDistributeMode);
}

size_t GetSubPacketSize(const GUILD_SUBHEADER_CG& header)
{
	switch (header)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:				return sizeof(int);
		case GUILD_SUBHEADER_CG_ADD_MEMBER:					return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:			return 10;
		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:		return sizeof(BYTE) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_OFFER:						return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_CHARGE_GSP:					return sizeof(int);
		case GUILD_SUBHEADER_CG_POST_COMMENT:				return 1;
		case GUILD_SUBHEADER_CG_DELETE_COMMENT:				return sizeof(DWORD);
		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:			return 0;
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_USE_SKILL:					return sizeof(TPacketCGGuildUseSkill);
		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:		return sizeof(DWORD) + sizeof(BYTE);
		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:		return sizeof(DWORD) + sizeof(BYTE);
	}

	return 0;
}

int CInputMain::Guild(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	if (uiBytes < sizeof(TPacketCGGuild))
		return -1;

	const TPacketCGGuild* p = reinterpret_cast<const TPacketCGGuild*>(data);
	const char* c_pData = data + sizeof(TPacketCGGuild);

	uiBytes -= sizeof(TPacketCGGuild);

	const GUILD_SUBHEADER_CG SubHeader = static_cast<GUILD_SUBHEADER_CG>(p->subheader);
	const size_t SubPacketLen = GetSubPacketSize(SubHeader);

	if (uiBytes < SubPacketLen)
	{
		return -1;
	}

	CGuild* pGuild = ch->GetGuild();

	if (NULL == pGuild)
	{
		if (SubHeader != GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 205, "");
			return SubPacketLen;
		}
	}

	switch (SubHeader)
	{
		case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:
			{
				return SubPacketLen;

				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), __deposit_limit());

				if (gold < 0)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 206, "");
					return SubPacketLen;
				}

				if (ch->GetGold() < gold)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 70, "");
					return SubPacketLen;
				}

				pGuild->RequestDepositMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:
			{
				return SubPacketLen;

#ifdef ENABLE_GUILD_YANG_ACCOUNTING_FIX
				const int gold = *reinterpret_cast<const int*>(c_pData);
#else
				const int gold = MIN(*reinterpret_cast<const int*>(c_pData), 500000);
#endif

				if (gold < 0)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 206, "");
					return SubPacketLen;
				}

#ifdef ENABLE_GUILD_YANG_ACCOUNTING_FIX
				if(ch->GetGold()+gold/**/ >= /**/ GOLD_MAX)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Yang miktarýn maksimum seviyeye ulaþmýþ.");
					return SubPacketLen;
				}
#endif

				pGuild->RequestWithdrawMoney(ch, gold);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_ADD_MEMBER:
			{
				const DWORD vid = *reinterpret_cast<const DWORD*>(c_pData);
				LPCHARACTER newmember = CHARACTER_MANAGER::instance().Find(vid);

				if (!newmember || !newmember->IsPC()) // Fix
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 207, "");
					return SubPacketLen;
				}

				if (!ch->IsPC())
					return SubPacketLen;

#ifdef ENABLE_MESSENGER_BLOCK
				if (MessengerManager::instance().IsBlocked(ch->GetName(), newmember->GetName()))
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 177, "%s", newmember->GetName());
					return SubPacketLen;
				}
				else if (MessengerManager::instance().IsBlocked(newmember->GetName(), ch->GetName()))
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 170, "%s", newmember->GetName());
					return SubPacketLen;
				}
#endif
				pGuild->Invite(ch, newmember);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REMOVE_MEMBER:
			{
				if (pGuild->UnderAnyWar() != 0)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 208, "");
					return SubPacketLen;
				}

				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				LPCHARACTER member = CHARACTER_MANAGER::instance().FindByPID(pid);

				if (member)
				{
					if (member->GetGuild() != pGuild)
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 209, "");
						return SubPacketLen;
					}

					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 198, "");
						return SubPacketLen;
					}

					member->SetQuestFlag("guild_manage.new_withdraw_time", get_global_time());
					pGuild->RequestRemoveMember(member->GetPlayerID());

					if (g_bGuildInviteLimit)
					{
						DBManager::instance().Query("REPLACE INTO guild_invite_limit VALUES(%d, %d)", pGuild->GetID(), get_global_time());
					}
				}
				else
				{
					if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 198, "");
						return SubPacketLen;
					}

					if (pGuild->RequestRemoveMember(pid))
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 210, "");
					else
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 207, "");
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:
			{
				char gradename[GUILD_GRADE_NAME_MAX_LEN + 1];
				strlcpy(gradename, c_pData + 1, sizeof(gradename));

				const TGuildMember * m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 211, "");
				}
				else if (*c_pData == GUILD_LEADER_GRADE)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 212, "");
				}
				else if (!check_name(gradename))
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 213, "");
				}
				else
				{
					pGuild->ChangeGradeName(*c_pData, gradename);
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:
			{
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 214, "");
				}
				else if (*c_pData == GUILD_LEADER_GRADE)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 212, "");
				}
				else
				{
					pGuild->ChangeGradeAuth(*c_pData, *(c_pData + 1));
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_OFFER:
			{
				DWORD offer = *reinterpret_cast<const DWORD*>(c_pData);

				if (pGuild->GetLevel() >= GUILD_MAX_LEVEL)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 215, "");
				}
				else
				{
					offer /= 100;
					offer *= 100;

					if (pGuild->OfferExp(ch, offer))
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 216, "%u", offer);
					}
					else
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 217, "");
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHARGE_GSP:
			{
				const int offer = *reinterpret_cast<const int*>(c_pData);
				const int gold = offer * 100;

				if (offer < 0 || gold < offer || gold < 0 || ch->GetGold() < gold)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 218, "");
					return SubPacketLen;
				}

				if (!pGuild->ChargeSP(ch, offer))
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 219, "");
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_POST_COMMENT:
			{
				const size_t length = *c_pData;

				if (length > GUILD_COMMENT_MAX_LEN)
				{
					sys_err("POST_COMMENT: %s comment too long (length: %u)", ch->GetName(), length);
					ch->GetDesc()->SetPhase(PHASE_CLOSE);
					return -1;
				}

				if (uiBytes < 1 + length)
					return -1;

				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (length && !pGuild->HasGradeAuth(m->grade, GUILD_AUTH_NOTICE) && *(c_pData + 1) == '!')
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 220, "");
				}
				else
				{
					std::string str(c_pData + 1, length);
					pGuild->AddComment(ch, str);
				}

				return (1 + length);
			}

		case GUILD_SUBHEADER_CG_DELETE_COMMENT:
			{
				const DWORD comment_id = *reinterpret_cast<const DWORD*>(c_pData);

				pGuild->DeleteComment(ch, comment_id);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_REFRESH_COMMENT:
			pGuild->RefreshComment(ch);
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE grade = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 214, "");
				else if (ch->GetPlayerID() == pid)
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 212, "");
				else if (grade == 1)
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 214, "");
				else
					pGuild->ChangeMemberGrade(pid, grade);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_USE_SKILL:
			{
				const TPacketCGGuildUseSkill* p = reinterpret_cast<const TPacketCGGuildUseSkill*>(c_pData);

				pGuild->UseSkill(p->dwVnum, ch, p->dwPID);
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:
			{
				const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE is_general = *(c_pData + sizeof(DWORD));
				const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

				if (NULL == m)
					return -1;

				if (m->grade != GUILD_LEADER_GRADE)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 221, "");
				}
				else
				{
					if (!pGuild->ChangeMemberGeneral(pid, is_general))
					{
						ch->LocaleChatPacket(CHAT_TYPE_INFO, 222, "");
					}
				}
			}
			return SubPacketLen;

		case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:
			{
				const DWORD guild_id = *reinterpret_cast<const DWORD*>(c_pData);
				const BYTE accept = *(c_pData + sizeof(DWORD));

				CGuild * g = CGuildManager::instance().FindGuild(guild_id);

				if (g)
				{
					if (accept)
						g->InviteAccept(ch);
					else
						g->InviteDeny(ch->GetPlayerID());
				}
			}
			return SubPacketLen;

	}

	return 0;
}

void CInputMain::Fishing(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGFishing* p = (TPacketCGFishing*)c_pData;
	ch->SetRotation(p->dir * 5);
	ch->fishing();
	return;
}

void CInputMain::ItemGive(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGGiveItem* p = (TPacketCGGiveItem*) c_pData;
	LPCHARACTER to_ch = CHARACTER_MANAGER::instance().Find(p->dwTargetVID);

	if (to_ch)
		ch->GiveItem(to_ch, p->ItemPos);
	else
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 223, "");
}

void CInputMain::Hack(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGHack * p = (TPacketCGHack *) c_pData;

	char buf[sizeof(p->szBuf)];
	strlcpy(buf, p->szBuf, sizeof(buf));

	sys_err("HACK_DETECT: %s %s", ch->GetName(), buf);
	ch->GetDesc()->SetPhase(PHASE_CLOSE);
}

int CInputMain::MyShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes)
{
	TPacketCGMyShop * p = (TPacketCGMyShop *) c_pData;
	int iExtraLen = p->bCount * sizeof(TShopItemTable);

	if (uiBytes < sizeof(TPacketCGMyShop) + iExtraLen)
		return -1;

	if (ch->GetGold() >= GOLD_MAX)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 18, "");
		sys_log(0, "MyShop ==> OverFlow Gold id %u name %s ", ch->GetPlayerID(), ch->GetName());
		return (iExtraLen);
	}

	if (ch->IsStun() || ch->IsDead())
		return (iExtraLen);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		|| ch->IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		|| ch->GetTransmutation()
#endif
		)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 74, "");
		return (iExtraLen);
	}

	sys_log(0, "MyShop count %d", p->bCount);
	ch->OpenMyShop(p->szSign, (TShopItemTable *) (c_pData + sizeof(TPacketCGMyShop)), p->bCount);
	return (iExtraLen);
}

#ifdef ENABLE_VIEW_CHEST_DROP
void CInputMain::ChestDropInfo(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGChestDropInfo* p = (TPacketCGChestDropInfo*) c_pData;

#ifdef ENABLE_SPECIAL_INVENTORY
	if(p->wInventoryCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
#else
	if(p->wInventoryCell >= INVENTORY_MAX_NUM)
#endif
		return;

	LPITEM pkItem = ch->GetInventoryItem(p->wInventoryCell);

	if (!pkItem)
		return;

	std::vector<TChestDropInfoTable> vec_ItemList;
	ITEM_MANAGER::instance().GetChestItemList(pkItem->GetVnum(), vec_ItemList);

	TPacketGCChestDropInfo packet;
	packet.bHeader = HEADER_GC_CHEST_DROP_INFO;
	packet.wSize = sizeof(packet) + sizeof(TChestDropInfoTable) * vec_ItemList.size();
	packet.dwChestVnum = pkItem->GetVnum();

	ch->GetDesc()->BufferedPacket(&packet, sizeof(packet));
	ch->GetDesc()->Packet(&vec_ItemList[0], sizeof(TChestDropInfoTable) * vec_ItemList.size());
}
#endif

void CInputMain::Refine(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGRefine* p = reinterpret_cast<const TPacketCGRefine*>(c_pData);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		|| ch->IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		|| ch->GetTransmutation()
#endif
		)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 74, "");
		ch->ClearRefineMode();
		return;
	}

	if (p->type == 255)
	{
		ch->ClearRefineMode();
		return;
	}

	if (p->pos >= INVENTORY_MAX_NUM)
	{
		ch->ClearRefineMode();
		return;
	}

	LPITEM item = ch->GetInventoryItem(p->pos);

	if (!item)
	{
		ch->ClearRefineMode();
		return;
	}

	ch->SetRefineTime();

	if (p->type == REFINE_TYPE_NORMAL)
	{
		sys_log (0, "refine_type_noraml");
		ch->DoRefine(item);
	}
	else if (p->type == REFINE_TYPE_SCROLL || p->type == REFINE_TYPE_HYUNIRON || p->type == REFINE_TYPE_MUSIN || p->type == REFINE_TYPE_BDRAGON)
	{
		sys_log (0, "refine_type_scroll, ...");
		ch->DoRefineWithScroll(item);
	}
	else if (p->type == REFINE_TYPE_MONEY_ONLY)
	{
		const LPITEM item = ch->GetInventoryItem(p->pos);

		if (NULL != item)
		{
			if (500 <= item->GetRefineSet())
			{
				LogManager::instance().HackLog("DEVIL_TOWER_REFINE_HACK", ch);
			}
			else
			{
#ifdef ENABLE_DEMON_TOWER_SMALL_FIX
				if (ch->GetQuestFlag("deviltower_zone.can_refine"))
				{
					if (ch->DoRefine(item, true))
					{
						ch->SetQuestFlag("deviltower_zone.can_refine", 0);
					}
				}
#else
				if (ch->GetQuestFlag("deviltower_zone.can_refine"))
				{
					ch->DoRefine(item, true);
					ch->SetQuestFlag("deviltower_zone.can_refine", 0);
				}
#endif
				else
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 117, "");
				}
			}
		}
	}

	ch->ClearRefineMode();
}

#ifdef ENABLE_RENEWAL_CUBE
void CInputMain::CubeRenewalSend(LPCHARACTER ch, const char* data)
{
	struct packet_send_cube_renewal * pinfo = (struct packet_send_cube_renewal *) data;
	switch (pinfo->subheader)
	{
		case CUBE_RENEWAL_SUB_HEADER_MAKE_ITEM:
		{

			int index_item = pinfo->index_item;
			int count_item = pinfo->count_item;
			int index_item_improve = pinfo->index_item_improve;

			Cube_Make(ch,index_item,count_item,index_item_improve);
		}
		break;

		case CUBE_RENEWAL_SUB_HEADER_CLOSE:
		{
			Cube_close(ch);
		}
		break;
	}
}
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void CInputMain::Acce(LPCHARACTER pkChar, const char* c_pData)
{
	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(pkChar->GetPlayerID());
	if (pPC->IsRunning())
		return;

	TPacketAcce * sPacket = (TPacketAcce*) c_pData;
	switch (sPacket->subheader)
	{
		case ACCE_SUBHEADER_CG_CLOSE:
		{
			pkChar->CloseAcce();
		}
		break;
		case ACCE_SUBHEADER_CG_ADD:
		{
			pkChar->AddAcceMaterial(sPacket->tPos, sPacket->bPos);
		}
		break;
		case ACCE_SUBHEADER_CG_REMOVE:
		{
			pkChar->RemoveAcceMaterial(sPacket->bPos);
		}
		break;
		case ACCE_SUBHEADER_CG_REFINE:
		{
			pkChar->RefineAcceMaterials();
		}
		break;

		default:
			break;
	}
}
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
int CInputMain::ReciveExtBattlePassActions(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	TPacketCGExtBattlePassAction* p = (TPacketCGExtBattlePassAction*)data;

	if (uiBytes < sizeof(TPacketCGExtBattlePassAction))
		return -1;

	const char* c_pData = data + sizeof(TPacketCGExtBattlePassAction);
	uiBytes -= sizeof(TPacketCGExtBattlePassAction);

	switch (p->bAction)
	{
		case 1:
			CBattlePassManager::instance().BattlePassRequestOpen(ch);
			return 0;

		case 2:
			if(get_dword_time() < ch->GetLastReciveExtBattlePassOpenRanking())
			{
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", ((ch->GetLastReciveExtBattlePassOpenRanking() - get_dword_time()) / 1000) + 1 );
				return 0;
			}

			ch->SetLastReciveExtBattlePassOpenRanking(get_dword_time() + 10000);

			for (BYTE bBattlePassType = 1; bBattlePassType <= 3 ; ++bBattlePassType)
			{
				BYTE bBattlePassID;
				if (bBattlePassType == 1)
					bBattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();

				if (bBattlePassType == 2)
				{
					bBattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
					if (bBattlePassID != ch->GetExtBattlePassPremiumID())
						continue;
				}

				std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT player_name, battlepass_type+0, battlepass_id, UNIX_TIMESTAMP(start_time), UNIX_TIMESTAMP(end_time) FROM player.battlepass_playerindex WHERE battlepass_type = %d and battlepass_id = %d and battlepass_completed = 1 and not player_name LIKE '[%%' ORDER BY (UNIX_TIMESTAMP(end_time)-UNIX_TIMESTAMP(start_time)) ASC LIMIT 40", bBattlePassType, bBattlePassID));
				if (pMsg->uiSQLErrno)
					return 0;

				MYSQL_ROW row;

				while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
				{
					TPacketGCExtBattlePassRanking pack;
					pack.bHeader = HEADER_GC_EXT_BATTLE_PASS_SEND_RANKING;
					strlcpy(pack.szPlayerName, row[0], sizeof(pack.szPlayerName));
					pack.bBattlePassType = std::atoi(row[1]);
					pack.bBattlePassID = std::atoll(row[2]);
					pack.dwStartTime = std::atoll(row[3]);
					pack.dwEndTime = std::atoll(row[4]);

					ch->GetDesc()->Packet(&pack, sizeof(pack));
				}
			}
			break;

		case 10:
			CBattlePassManager::instance().BattlePassRequestReward(ch, 1);
			return 0;

		case 11:
			CBattlePassManager::instance().BattlePassRequestReward(ch, 2);
			return 0;

		default:
			break;
	}

	return 0;
}

int CInputMain::ReciveExtBattlePassPremiumItem(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	TPacketCGExtBattlePassSendPremiumItem* p = (TPacketCGExtBattlePassSendPremiumItem*)data;

	if (uiBytes < sizeof(TPacketCGExtBattlePassSendPremiumItem))
		return -1;

	const char* c_pData = data + sizeof(TPacketCGExtBattlePassSendPremiumItem);
	uiBytes -= sizeof(TPacketCGExtBattlePassSendPremiumItem);

	LPITEM item = ch->GetInventoryItem(p->iSlotIndex);
	if (item != NULL and item->GetVnum() == 93100)
	{
		ch->PointChange(POINT_BATTLE_PASS_PREMIUM_ID, CBattlePassManager::instance().GetPremiumBattlePassID());
		CBattlePassManager::instance().BattlePassRequestOpen(ch);
		item->SetCount(item->GetCount() - 1);
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 345, "");
	}
	return 0;
}
#endif

#ifdef ENABLE_RIDING_EXTENDED
void CInputMain::MountUpGrade(LPCHARACTER ch, const char* c_pData)
{
	if (!ch)
	{
		sys_err("CInputMain::MountUpGrade - Unknown identifier)");
		return;
	}
	
	const auto* p = reinterpret_cast<const TPacketCGMountUpGrade*>(c_pData);
	
	if (!p) return;
	
	switch (p->iSubHeader)
	{
	case CMountUpGrade::EMountUpGradeCGSubheaderType::SUBHEADER_CG_MOUNT_UP_GRADE_EXP:
		CMountUpGrade::Instance().SetExp(ch);
		break;
	
	case CMountUpGrade::EMountUpGradeCGSubheaderType::SUBHEADER_CG_MOUNT_UP_GRADE_LEVEL_UP:
		CMountUpGrade::Instance().SetLevel(ch);
		break;
	
	default:
		sys_err("Unknown iSubHeader (Name: %s) - (iSubHeader: %d)", ch->GetName(), p->iSubHeader);
		return;
	}
}
#endif

#ifdef ENABLE_STYLE_ATTRIBUTE_SYSTEM
void CInputMain::ItemNewAttributes(LPCHARACTER ch, const char* pcData)
{
	TPacketCGItemNewAttribute* p = (TPacketCGItemNewAttribute*)pcData;

	if (ch)
		ch->UseItemNewAttribute(p->source_pos, p->target_pos, p->bValues);
}
#endif

int CInputMain::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (ch->GetDesc()->GetPhase() != PHASE_GAME && ch->GetDesc()->GetPhase() != PHASE_DEAD)
	{
		sys_err("no character in game");
		sys_log(0, "fix pid : %u phase : %u header : %u ip address : %s", ch->GetPlayerID(), ch->GetDesc()->GetPhase(), bHeader, ch->GetDesc()->GetHostName());
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	int iExtraLen = 0;

	if (test_server && bHeader != HEADER_CG_MOVE)
		sys_log(0, "CInputMain::Analyze() ==> Header [%d] ", bHeader);

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d); 
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if (test_server)
			{
				char* pBuf = (char*)c_pData;
				sys_log(0, "%s", pBuf + sizeof(TPacketCGChat));
			}
	
			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MOVE:
			Move(ch, c_pData);
			break;

		case HEADER_CG_CHARACTER_POSITION:
			Position(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE:
			if (!ch->IsObserverMode())
				ItemUse(ch, c_pData);
			break;

		case HEADER_CG_ITEM_DROP:
			if (!ch->IsObserverMode())
			{
				ItemDrop(ch, c_pData);
			}
			break;

		case HEADER_CG_ITEM_DROP2:
			if (!ch->IsObserverMode())
				ItemDrop2(ch, c_pData);
			break;

#ifdef ENABLE_AUTO_SELL_SYSTEM
		case HEADER_CG_AUTO_SELL_ADD:
		{
			if (!ch->IsPC())
				return sizeof(TPacketCGAutoSellAdd);

			AutoSellAdd(ch, c_pData);
			return sizeof(TPacketCGAutoSellAdd);
		}
		break;

		case HEADER_CG_AUTO_SELL_REMOVE:
		{
			if (!ch->IsPC())
				return sizeof(TPacketCGAutoSellRemove);

			AutoSellRemove(ch, c_pData);
			return sizeof(TPacketCGAutoSellRemove);
		}
		break;

		case HEADER_CG_AUTO_SELL_STATUS:
		{
			if (!ch->IsPC())
				return sizeof(TPacketCGAutoSellStatus);

			AutoSellStatus(ch, c_pData);
			return sizeof(TPacketCGAutoSellStatus);
		}
		break;
#endif

#ifdef ENABLE_STYLE_ATTRIBUTE_SYSTEM
		case HEADER_CG_ITEM_USE_NEW_ATTRIBUTE:
			if (!ch->IsObserverMode())
				ItemNewAttributes(ch, c_pData);
			break;
#endif

		case HEADER_CG_ITEM_MOVE:
			if (!ch->IsObserverMode())
				ItemMove(ch, c_pData);
			break;

		case HEADER_CG_ITEM_PICKUP:
			if (!ch->IsObserverMode())
				ItemPickup(ch, c_pData);
			break;

		case HEADER_CG_ITEM_USE_TO_ITEM:
			if (!ch->IsObserverMode())
				ItemToItem(ch, c_pData);
			break;

		case HEADER_CG_ITEM_GIVE:
			if (!ch->IsObserverMode())
				ItemGive(ch, c_pData);
			break;

		case HEADER_CG_EXCHANGE:
			if (!ch->IsObserverMode())
				Exchange(ch, c_pData);
			break;

		case HEADER_CG_ATTACK:
		case HEADER_CG_SHOOT:
			if (!ch->IsObserverMode())
			{
				Attack(ch, bHeader, c_pData);
			}
			break;

		case HEADER_CG_USE_SKILL:
			if (!ch->IsObserverMode())
				UseSkill(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_ADD:
			QuickslotAdd(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_DEL:
			QuickslotDelete(ch, c_pData);
			break;

		case HEADER_CG_QUICKSLOT_SWAP:
			QuickslotSwap(ch, c_pData);
			break;

		case HEADER_CG_SHOP:
			if ((iExtraLen = Shop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MESSENGER:
			if ((iExtraLen = Messenger(ch, c_pData, m_iBufferLeft))<0)
				return -1;
			break;

		case HEADER_CG_ON_CLICK:
			OnClick(ch, c_pData);
			break;

		case HEADER_CG_SYNC_POSITION:
			if ((iExtraLen = SyncPosition(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_ADD_FLY_TARGETING:
		case HEADER_CG_FLY_TARGETING:
			FlyTarget(ch, c_pData, bHeader);
			break;

		case HEADER_CG_SCRIPT_BUTTON:
			ScriptButton(ch, c_pData);
			break;

		case HEADER_CG_SCRIPT_SELECT_ITEM:
			ScriptSelectItem(ch, c_pData);
			break;

		case HEADER_CG_SCRIPT_ANSWER:
			ScriptAnswer(ch, c_pData);
			break;

		case HEADER_CG_QUEST_INPUT_STRING:
			QuestInputString(ch, c_pData);
			break;

		case HEADER_CG_QUEST_CONFIRM:
			QuestConfirm(ch, c_pData);
			break;

		case HEADER_CG_TARGET:
			Target(ch, c_pData);
			break;

		case HEADER_CG_WARP:
			Warp(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKIN:
			SafeboxCheckin(ch, c_pData);
			break;

		case HEADER_CG_SAFEBOX_CHECKOUT:
			SafeboxCheckout(ch, c_pData, false);
			break;

		case HEADER_CG_SAFEBOX_ITEM_MOVE:
			SafeboxItemMove(ch, c_pData);
			break;

		case HEADER_CG_MALL_CHECKOUT:
			SafeboxCheckout(ch, c_pData, true);
			break;

		case HEADER_CG_PARTY_INVITE:
			PartyInvite(ch, c_pData);
			break;

		case HEADER_CG_PARTY_REMOVE:
			PartyRemove(ch, c_pData);
			break;

		case HEADER_CG_PARTY_INVITE_ANSWER:
			PartyInviteAnswer(ch, c_pData);
			break;

		case HEADER_CG_PARTY_SET_STATE:
			PartySetState(ch, c_pData);
			break;

		case HEADER_CG_PARTY_USE_SKILL:
			PartyUseSkill(ch, c_pData);
			break;

		case HEADER_CG_PARTY_PARAMETER:
			PartyParameter(ch, c_pData);
			break;

		case HEADER_CG_GUILD:
			if ((iExtraLen = Guild(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_FISHING:
			Fishing(ch, c_pData);
			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

		case HEADER_CG_MYSHOP:
			if ((iExtraLen = MyShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_REFINE:
			Refine(ch, c_pData);
			break;

		case HEADER_CG_DRAGON_SOUL_REFINE:
			{
				TPacketCGDragonSoulRefine* p = reinterpret_cast <TPacketCGDragonSoulRefine*>((void*)c_pData);
				switch(p->bSubType)
				{
				case DS_SUB_HEADER_CLOSE:
					ch->DragonSoul_RefineWindow_Close();
					break;
				case DS_SUB_HEADER_DO_REFINE_GRADE:
					{
						DSManager::instance().DoRefineGrade(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STEP:
					{
						DSManager::instance().DoRefineStep(ch, p->ItemGrid);
					}
					break;
				case DS_SUB_HEADER_DO_REFINE_STRENGTH:
					{
						DSManager::instance().DoRefineStrength(ch, p->ItemGrid);
					}
					break;
				}
			}
			break;

#ifdef ENABLE_DESTROY_DIALOG
		case HEADER_CG_ITEM_DESTROY:
			if (!ch->IsObserverMode())
				ItemDestroy(ch, c_pData);
			break;
#endif

#ifdef ENABLE_QUICK_SELL_ITEM
		case HEADER_CG_ITEM_SELL:
			if (!ch->IsObserverMode())
				ItemSell(ch, c_pData);
			break;
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		case HEADER_CG_CHANGE_LANGUAGE:
		{
			TPacketChangeLanguage* p = reinterpret_cast <TPacketChangeLanguage*>((void*)c_pData);
			ChangeLanguage(ch, p->bLanguage);
		}
		break;
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
		case HEADER_CG_WHISPER_DETAILS:
			WhisperDetails(ch, c_pData);
			break;
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
		case HEADER_CG_SWITCHBOT:
			if ((iExtraLen = Switchbot(ch, c_pData, m_iBufferLeft)) < 0)
			{
				return -1;
			}
			break;
#endif

#ifdef ENABLE_RENEWAL_CUBE
		case HEADER_CG_CUBE_RENEWAL:
			CubeRenewalSend(ch, c_pData);
			break;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case HEADER_CG_ACCE:
			Acce(ch, c_pData);
			break;
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
		case HEADER_CG_BIOLOG_MANAGER:
			if ((iExtraLen = BiologManager(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

#ifdef ENABLE_MOB_DROP_INFO
		case HEADER_CG_TARGET_INFO_LOAD:
		{
			TargetInfoLoad(ch, c_pData);
		}
		break;
#endif

#ifdef ENABLE_VIEW_CHEST_DROP
		case HEADER_CG_CHEST_DROP_INFO:
			ChestDropInfo(ch, c_pData);
			break;
#endif

#ifdef ENABLE_EVENT_MANAGER
		case HEADER_CG_REQUEST_EVENT_QUEST:
			RequestEventQuest(ch, c_pData);
			break;

		case HEADER_CG_REQUEST_EVENT_DATA:
			RequestEventData(ch, c_pData);
			break;
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
		case HEADER_CG_EXT_BATTLE_PASS_ACTION:
			if ((iExtraLen = ReciveExtBattlePassActions(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_EXT_SEND_BP_PREMIUM_ITEM:
			if ((iExtraLen = ReciveExtBattlePassPremiumItem(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		case HEADER_CG_SKILL_COLOR:
			SetSkillColor(ch, c_pData);
			break;
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
		case HEADER_CG_OFFLINE_SHOP:
			if ((iExtraLen = OfflineShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_CG_MY_OFFLINE_SHOP:
			if ((iExtraLen = MyOfflineShop(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
		case HEADER_CG_SHOP_SEARCH:
			ShopSearch(ch, c_pData);
			break;
#endif

#ifdef ENABLE_RENEWAL_OX_EVENT
		case HEADER_CG_QUEST_INPUT_LONG_STRING:
			QuestInputLongString(ch, c_pData);
			break;
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
		case HEADER_CG_ITEMS_PICKUP:
			if ((iExtraLen = ItemsPickup(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
		case HEADER_CG_AURA:
			if ((iExtraLen = Aura(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
		case HEADER_CG_PET_HATCH:
			PetHatch(ch, c_pData);
			break;

		case HEADER_CG_PET_WINDOW:
			PetWindow(ch, c_pData);
			break;

		case HEADER_CG_PET_WINDOW_TYPE:
			PetWindowType(ch, c_pData);
			break;

		case HEADER_CG_PET_NAME_CHANGE:
			PetNameChange(ch, c_pData);
			break;

		case HEADER_CG_PET_FEED:
			PetFeed(ch, c_pData);
			break;

		case HEADER_CG_PET_DETERMINE:
			PetDetermine(ch, c_pData);
			break;

		case HEADER_CG_PET_ATTR_CHANGE:
			PetAttrChange(ch, c_pData);
			break;

		case HEADER_CG_PET_REVIVE:
			PetRevive(ch, c_pData);
			break;

		case HEADER_CG_PET_LEARN_SKILL:
			PetLearnSkill(ch, c_pData);
			break;

		case HEADER_CG_PET_SKILL_UPGRADE:
			PetSkillUpgrade(ch, c_pData);
			break;

		case HEADER_CG_PET_DELETE_SKILL:
			PetDeleteSkill(ch, c_pData);
			break;

		case HEADER_CG_PET_DELETE_ALL_SKILL:
			PetDeleteAllSkill(ch, c_pData);
			break;
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		case HEADER_CG_TRANSMUTATION:
			Transmutation(ch, c_pData);
			break;
#endif

#ifdef ENABLE_HUNTING_SYSTEM
		case HEADER_CG_SEND_HUNTING_ACTION:
			ReciveHuntingAction(ch, c_pData);
			break;
#endif

#ifdef ENABLE_RIDING_EXTENDED
		case HEADER_CG_MOUNT_UP_GRADE:
			MountUpGrade(ch, c_pData);
			break;
#endif
	}
	return (iExtraLen);
}

int CInputDead::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		return -1;
	}

	if (ch && ch->IsPC())
	{
		if (get_global_time() < ch->analyze_protect)
		{
			ch->analyze_protect_count = (ch->analyze_protect_count + 1);

			if (ch->analyze_protect_count >= 300)
			{
				ch->analyze_protect_count = 0;
				d->SetPhase(PHASE_CLOSE);
				return -1;
			}
		}
		else
			ch->analyze_protect_count = 0;

		ch->analyze_protect = (get_global_time() + 1);
	}

	int iExtraLen = 0;

	switch (bHeader)
	{
		case HEADER_CG_PONG:
			Pong(d); 
			break;

		case HEADER_CG_TIME_SYNC:
			Handshake(d, c_pData);
			break;

		case HEADER_CG_CHAT:
			if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_WHISPER:
			if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
				return -1;

			break;

		case HEADER_CG_HACK:
			Hack(ch, c_pData);
			break;

		default:
			return -1;
	}

	return (iExtraLen);
}

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
void CInputMain::ChangeLanguage(LPCHARACTER ch, BYTE bLanguage)
{
	if (!ch)
		return;

	if (!ch->GetDesc())
		return;

	if (ch->GetLanguage() == bLanguage)
		return;

	if (bLanguage > LOCALE_EUROPE && bLanguage < LOCALE_MAX_NUM)
	{
		TRequestChangeLanguage packet;
		packet.dwAID = ch->GetDesc()->GetAccountTable().id;
		packet.bLanguage = bLanguage;

		db_clientdesc->DBPacketHeader(HEADER_GD_REQUEST_CHANGE_LANGUAGE, 0, sizeof(TRequestChangeLanguage));
		db_clientdesc->Packet(&packet, sizeof(packet));

		ch->ChangeLanguage(bLanguage);
	}
}
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
void CInputMain::WhisperDetails(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGWhisperDetails* CGWhisperDetails = (TPacketCGWhisperDetails*)c_pData;

	if (!*CGWhisperDetails->name)
		return;

	TPacketGCWhisperDetails GCWhisperDetails;
	GCWhisperDetails.header = HEADER_GC_WHISPER_DETAILS;
	strncpy(GCWhisperDetails.name, CGWhisperDetails->name, sizeof(GCWhisperDetails.name) - 1);

	BYTE bLanguage = LOCALE_DEFAULT;

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(CGWhisperDetails->name);

	if (!pkChr)
	{
		LPDESC pkDesc = NULL;
		CCI* pkCCI = P2P_MANAGER::instance().Find(CGWhisperDetails->name);

		if (pkCCI)
		{
			pkDesc = pkCCI->pkDesc;
			if (pkDesc)
				bLanguage = pkCCI->bLanguage;
		}
	}
	else
	{
		if (pkChr->GetDesc())
			bLanguage = pkChr->GetDesc()->GetLanguage();
	}

	GCWhisperDetails.bLanguage = bLanguage;
	ch->GetDesc()->Packet(&GCWhisperDetails, sizeof(GCWhisperDetails));
}
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
int CInputMain::Switchbot(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	const TPacketCGSwitchbot* p = reinterpret_cast<const TPacketCGSwitchbot*>(data);

	if (uiBytes < sizeof(TPacketCGSwitchbot))
	{
		return -1;
	}

	const char* c_pData = data + sizeof(TPacketCGSwitchbot);
	uiBytes -= sizeof(TPacketCGSwitchbot);

	switch (p->subheader)
	{
		case SUBHEADER_CG_SWITCHBOT_START:
		{
			size_t extraLen = sizeof(BYTE) + sizeof(TSwitchbotAttributeAlternativeTable) * SWITCHBOT_ALTERNATIVE_COUNT;
			// size_t extraLen = sizeof(TSwitchbotAttributeAlternativeTable) * SWITCHBOT_ALTERNATIVE_COUNT;
			if (uiBytes < extraLen)
			{
				return -1;
			}

			std::vector<TSwitchbotAttributeAlternativeTable> vec_alternatives;

			for (BYTE alternative = 0; alternative < SWITCHBOT_ALTERNATIVE_COUNT; ++alternative)
			{
				const TSwitchbotAttributeAlternativeTable* pAttr = reinterpret_cast<const TSwitchbotAttributeAlternativeTable*>(c_pData);
				c_pData += sizeof(TSwitchbotAttributeAlternativeTable);

				vec_alternatives.emplace_back(*pAttr);
			}

			CSwitchbotManager::Instance().Start(ch->GetPlayerID(), p->slot, vec_alternatives);
			return extraLen;
		}

		case SUBHEADER_CG_SWITCHBOT_STOP:
		{
			CSwitchbotManager::Instance().Stop(ch->GetPlayerID(), p->slot);
			return 0;
		}
	}

	return 0;
}
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
int CInputMain::BiologManager(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	if (!ch)
	{
		return -1;
	}

	TPacketCGBiologManagerAction* p = (TPacketCGBiologManagerAction*)c_pData;
	c_pData += sizeof(TPacketCGBiologManagerAction);

	CBiologSystem* pkBiologManager = ch->GetBiologManager();
	if (!pkBiologManager)
	{
		return -1;
	}

	return pkBiologManager->RecvClientPacket(p->bSubHeader, c_pData, uiBytes);
}
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
int CInputMain::MyOfflineShop(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGMyOfflineShop* p = (TPacketCGMyOfflineShop*)c_pData;
	int iExtraLen = p->bCount * sizeof(TOfflineShopItemTable);

	if (uiBytes < sizeof(TPacketCGMyOfflineShop) + iExtraLen)
		return -1;

	COfflineShopManager::instance().OpenMyOfflineShop(ch, p->szSign, (TOfflineShopItemTable*)(c_pData + sizeof(TPacketCGMyOfflineShop)), p->bCount, p->shopVnum, p->shopTitle);
	return (iExtraLen);
}

int CInputMain::OfflineShop(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	TPacketCGShop* p = (TPacketCGShop*)data;

	if (uiBytes < sizeof(TPacketCGShop))
		return -1;

	if (!ch || !ch->GetDesc())
		return -1;

	const char* c_pData = data + sizeof(TPacketCGShop);
	uiBytes -= sizeof(TPacketCGShop);

	switch (p->subheader)
	{
		case SHOP_SUBHEADER_CG_END:
			COfflineShopManager::instance().StopShopping(ch);
			break;

		case SHOP_SUBHEADER_CG_BUY:
			COfflineShopManager::instance().Buy(ch, p->vid, p->pos);
			break;

		case SHOP_SUBHEADER_CG_DESTROY_OFFLINE_SHOP:
			COfflineShopManager::instance().DestroyOfflineShop(ch);
			break;

		case SHOP_SUBHEADER_CG_ADD_ITEM:
		{
			if (uiBytes < sizeof(TOfflineShopAddItem))
				return -1;

			TOfflineShopAddItem* pTable = (TOfflineShopAddItem*)(c_pData);
			COfflineShopManager::instance().AddItem(ch, pTable->bDisplayPos, pTable->bPos, pTable->lPrice);
			return (sizeof(TOfflineShopAddItem));
		}

		case SHOP_SUBHEADER_CG_REMOVE_ITEM:
		{
			if (uiBytes < sizeof(BYTE))
				return -1;

			BYTE bPos = *c_pData;
			COfflineShopManager::instance().RemoveItem(ch, bPos);
			return (sizeof(BYTE));
		}

		case SHOP_SUBHEADER_CG_OPEN_SLOT:
		{
			if (uiBytes < sizeof(BYTE))
				return -1;

			BYTE bPos = *c_pData;
			COfflineShopManager::instance().OpenSlot(ch, bPos);
			return (sizeof(BYTE));
		}

		case SHOP_SUBHEADER_CG_CHECK:
			COfflineShopManager::instance().HasOfflineShop(ch);
			break;

		case SHOP_SUBHEADER_CG_CHANGE_TITLE:
			COfflineShopManager::instance().ChangeTitle(ch, p->title);
			break;

		case SHOP_SUBHEADER_CG_TAKE_MONEY:
			COfflineShopManager::instance().WithdrawMoney(ch);
			break;

		case SHOP_SUBHEADER_CG_CHANGE_DECORATION:
		{
			if (uiBytes < sizeof(TShopDecoration))
				return -1;

			TShopDecoration* decoration = (TShopDecoration*)(c_pData);
			COfflineShopManager::instance().ChangeDecoration(ch, decoration);
			return (sizeof(TShopDecoration));
		}
		break;

		case SHOP_SUBHEADER_CG_LOG_REMOVE:
			COfflineShopManager::instance().ShopLogRemove(ch);
			break;

		case SHOP_SUBHEADER_CG_BUTTON:
			COfflineShopManager::instance().OpenOfflineShop(ch);
			break;

		case SHOP_SUBHEADER_CG_GET_ITEM:
			COfflineShopManager::instance().GetBackItem(ch);
			break;

		case SHOP_SUBHEADER_CG_ADD_TIME:
			COfflineShopManager::instance().ShopAddTime(ch);
			break;

		case SHOP_SUBHEADER_CG_OPEN_WITH_VID:
		{
			if (uiBytes < sizeof(DWORD))
				return -1;

			DWORD bVID = *c_pData;
			COfflineShopManager::instance().OpenOfflineShopWithVID(ch, bVID);
			return (sizeof(DWORD));
		}
		break;

		default:
			sys_err("CInputMain::OfflineShop : Unknown subheader %d : %s", p->subheader, ch->GetName());
			break;
	}

	return 0;
}
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
void CInputMain::ShopSearch(LPCHARACTER ch, const char* data)
{
	if (!ch || !data)
		return;

	if (ch->CanSearch())
		return;

	COfflineShopManager::instance().SearchItem(ch, data);
}
#endif

#ifdef ENABLE_RENEWAL_OX_EVENT
void CInputMain::QuestInputLongString(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestInputLongString * p = (TPacketCGQuestInputLongString*)c_pData;

	char msg[129];
	strlcpy(msg, p->msg, sizeof(msg));
	sys_log(0, "QUEST InputLongString pid %u msg %s", ch->GetPlayerID(), msg);

	quest::CQuestManager::Instance().Input(ch->GetPlayerID(), msg);
}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
size_t GetAuraSubPacketLength(const EPacketCGAuraSubHeader& SubHeader)
{
	switch (SubHeader)
	{
		case AURA_SUBHEADER_CG_REFINE_CHECKIN:
			return sizeof(TSubPacketCGAuraRefineCheckIn);

		case AURA_SUBHEADER_CG_REFINE_CHECKOUT:
			return sizeof(TSubPacketCGAuraRefineCheckOut);

		case AURA_SUBHEADER_CG_REFINE_ACCEPT:
			return sizeof(TSubPacketCGAuraRefineAccept);

		case AURA_SUBHEADER_CG_REFINE_CANCEL:
			return 0;
	}

	return 0;
}

int CInputMain::Aura(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	if (uiBytes < sizeof(TPacketCGAura))
		return -1;

	const TPacketCGAura* pinfo = reinterpret_cast<const TPacketCGAura*>(data);
	const char* c_pData = data + sizeof(TPacketCGAura);

	uiBytes -= sizeof(TPacketCGAura);

	const EPacketCGAuraSubHeader SubHeader = static_cast<EPacketCGAuraSubHeader>(pinfo->bSubHeader);
	const size_t SubPacketLength = GetAuraSubPacketLength(SubHeader);
	if (uiBytes < SubPacketLength)
	{
		sys_err("invalid aura subpacket length (sublen %d size %u buffer %u)", SubPacketLength, sizeof(TPacketCGAura), uiBytes);
		return -1;
	}

	switch (SubHeader)
	{
		case AURA_SUBHEADER_CG_REFINE_CHECKIN:
			{
				const TSubPacketCGAuraRefineCheckIn* sp = reinterpret_cast<const TSubPacketCGAuraRefineCheckIn*>(c_pData);
				ch->AuraRefineWindowCheckIn(sp->byAuraRefineWindowType, sp->AuraCell, sp->ItemCell);
			}
			return SubPacketLength;

		case AURA_SUBHEADER_CG_REFINE_CHECKOUT:
			{
				const TSubPacketCGAuraRefineCheckOut* sp = reinterpret_cast<const TSubPacketCGAuraRefineCheckOut*>(c_pData);
				ch->AuraRefineWindowCheckOut(sp->byAuraRefineWindowType, sp->AuraCell);
			}
			return SubPacketLength;

		case AURA_SUBHEADER_CG_REFINE_ACCEPT:
			{
				const TSubPacketCGAuraRefineAccept* sp = reinterpret_cast<const TSubPacketCGAuraRefineAccept*>(c_pData);
				ch->AuraRefineWindowAccept(sp->byAuraRefineWindowType);
			}
			return SubPacketLength;

		case AURA_SUBHEADER_CG_REFINE_CANCEL:
			{
				ch->AuraRefineWindowClose();
			}
			return SubPacketLength;
	}

	return 0;
}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
void CInputMain::PetHatch(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetHatch* p = reinterpret_cast<const TPacketCGPetHatch*>(c_pData);

	if (!ch || !ch->GetDesc())
		return;

	CGrowthPetManager::Instance().EggHatch(ch, p->name, p->eggPos);
}

void CInputMain::PetWindow(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetWindow* p = reinterpret_cast<const TPacketCGPetWindow*>(c_pData);

	if (!ch || !ch->GetDesc())
		return;

	switch (p->window)
	{
		case PET_WINDOW_HATCH:
			ch->SetPetHatchWindow(p->state);
			break;

		case PET_WINDOW_NAME_CHANGE:
			ch->SetPetChangeNameWindow(p->state);
			break;
	}
}

void CInputMain::PetWindowType(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetWindowType* p = reinterpret_cast<const TPacketCGPetWindowType*>(c_pData);

	if (!ch || !ch->GetDesc())
		return;

	BYTE bWindowType = p->type;

	if (bWindowType == ch->GetPetWindowType())
		return;

	if (bWindowType == PET_WINDOW_ATTR_CHANGE || bWindowType == PET_WINDOW_PRIMIUM_FEEDSTUFF)
	{

		if ((ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner()) || ch->IsCubeOpen())
			return;
	}

	ch->SetPetWindowType(bWindowType);

	TPacketGCPet packet;
	packet.header = HEADER_GC_PET;
	packet.subheader = SUBHEADER_PET_WINDOW_TYPE_INFO + bWindowType;

	ch->GetDesc()->Packet(&packet, sizeof(packet));
}

void CInputMain::PetFeed(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetFeed* p = reinterpret_cast<const TPacketCGPetFeed*>(c_pData);

	if (!ch || !ch->GetDesc())
		return;

	if (ch->GetActiveGrowthPet())
		ch->GetActiveGrowthPet()->Feed(p);
	else
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 742, "");
}

void CInputMain::PetDetermine(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetDetermine* p = reinterpret_cast<const TPacketCGPetDetermine*>(c_pData);

	if (!ch || !ch->GetDesc())
		return;

	LPITEM pDetermineItem = ch->GetInventoryItem(p->determinePos.cell);
	if (!pDetermineItem)
		return;

	if (pDetermineItem->GetType() != ITEM_GROWTH_PET || pDetermineItem->GetSubType() != PET_ATTR_DETERMINE)
		return;

	if (!ch->GetActiveGrowthPet())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 742, "");
		return;
	}

	ch->GetActiveGrowthPet()->AttrDetermine(pDetermineItem);
}

void CInputMain::PetAttrChange(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetAttrChange* p = reinterpret_cast<const TPacketCGPetAttrChange*>(c_pData);

	if (!ch || !ch->GetDesc())
		return;

	LPITEM pUpBringingItem = ch->GetInventoryItem(p->upBringingPos.cell);
	if (!pUpBringingItem)
		return;

	if (pUpBringingItem->GetType() != ITEM_GROWTH_PET || pUpBringingItem->GetSubType() != PET_UPBRINGING)
		return;

	LPITEM pChangeAttrItem = ch->GetInventoryItem(p->attrChangePos.cell);
	if (!pChangeAttrItem)
		return;

	if (pChangeAttrItem->GetType() != ITEM_GROWTH_PET || pChangeAttrItem->GetSubType() != PET_ATTR_CHANGE)
		return;

	LPGROWTH_PET pPet = ch->GetGrowthPet(pUpBringingItem->GetSocket(2));
	if (!pPet)
		return;

	pPet->AttrChange(pUpBringingItem, pChangeAttrItem);
}

void CInputMain::PetRevive(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetRevive* p = reinterpret_cast<const TPacketCGPetRevive*>(c_pData);

	if (!ch || !ch->GetDesc())
		return;

	LPITEM pUpBringingItem = ch->GetInventoryItem(p->upBringingPos.cell);
	if (!pUpBringingItem)
		return;

	if (pUpBringingItem->GetType() != ITEM_GROWTH_PET || pUpBringingItem->GetSubType() != PET_UPBRINGING)
		return;

	LPGROWTH_PET pPet = ch->GetGrowthPet(pUpBringingItem->GetSocket(2));
	if (!pPet)
		return;

	pPet->PremiumFeed(p, pUpBringingItem);
}

void CInputMain::PetLearnSkill(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetLearnSkill* p = reinterpret_cast<const TPacketCGPetLearnSkill*>(c_pData);

	if (!ch || !ch->GetDesc())
		return;

	LPITEM pSkillBookItem = ch->GetInventoryItem(p->learnSkillPos.cell);
	if (!pSkillBookItem)
		return;

	if (pSkillBookItem->GetType() != ITEM_GROWTH_PET || pSkillBookItem->GetSubType() != PET_SKILL)
		return;

	if (!ch->GetActiveGrowthPet())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 742, "");
		return;
	}

	ch->GetActiveGrowthPet()->LearnSkill(p->slotIndex, pSkillBookItem);
}

void CInputMain::PetSkillUpgrade(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetSkillUpgrade* p = reinterpret_cast<const TPacketCGPetSkillUpgrade*>(c_pData);

	if (!ch->GetActiveGrowthPet())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 742, "");
		return;
	}

	ch->GetActiveGrowthPet()->SkillUpgrade(p->slotIndex);
}

void CInputMain::PetDeleteSkill(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetDeleteSkill* p = reinterpret_cast<const TPacketCGPetDeleteSkill*>(c_pData);

	if (!ch || !ch->GetDesc())
		return;

	LPITEM pDeleteSkillItem = ch->GetInventoryItem(p->deleteSkillPos.cell);
	if (!pDeleteSkillItem)
		return;

	if (pDeleteSkillItem->GetType() != ITEM_GROWTH_PET || pDeleteSkillItem->GetSubType() != PET_SKILL_DEL_BOOK)
		return;

	if (!ch->GetActiveGrowthPet())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 742, "");
		return;
	}

	ch->GetActiveGrowthPet()->DeleteSkill(p->slotIndex, pDeleteSkillItem);
}

void CInputMain::PetDeleteAllSkill(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetDeleteAllSkill* p = reinterpret_cast<const TPacketCGPetDeleteAllSkill*>(c_pData);

	if (!ch || !ch->GetDesc())
		return;

	LPITEM pDeleteAllSkillItem = ch->GetInventoryItem(p->deleteAllSkillPos.cell);
	if (!pDeleteAllSkillItem)
		return;

	if (pDeleteAllSkillItem->GetType() != ITEM_GROWTH_PET || pDeleteAllSkillItem->GetSubType() != PET_SKILL_ALL_DEL_BOOK)
		return;

	if (!ch->GetActiveGrowthPet())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 742, "");
		return;
	}

	ch->GetActiveGrowthPet()->DeleteAllSkill(pDeleteAllSkillItem);
}

void CInputMain::PetNameChange(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGPetNameChange* p = reinterpret_cast<const TPacketCGPetNameChange*>(c_pData);

	if (!ch || !ch->GetDesc())
		return;

	LPITEM pNameChangeItem = ch->GetInventoryItem(p->changeNamePos.cell);
	if (!pNameChangeItem)
		return;

	if (pNameChangeItem->GetType() != ITEM_GROWTH_PET || pNameChangeItem->GetSubType() != PET_NAME_CHANGE)
		return;

	LPITEM pUpBringingItem = ch->GetInventoryItem(p->upBringingPos.cell);
	if (!pUpBringingItem)
		return;

	if (pUpBringingItem->GetType() != ITEM_GROWTH_PET || pUpBringingItem->GetSubType() != PET_UPBRINGING)
		return;

	LPGROWTH_PET pPet = ch->GetGrowthPet(pUpBringingItem->GetSocket(2));
	if (!pPet)
		return;

	if (pPet->GetOwner() != ch)
		return;

	pPet->NameChange(p->name, pNameChangeItem, pUpBringingItem);
}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
void CInputMain::Transmutation(LPCHARACTER ch, const char* c_pData)
{
	auto p = reinterpret_cast<const TPacketCGTransmutation*>(c_pData);

	CTransmutation* CLook = ch->GetTransmutation();
	if (CLook == nullptr)
		return;

	switch (static_cast<ECG_TRANSMUTATION_SHEADER>(p->subheader))
	{
		case ECG_TRANSMUTATION_SHEADER::ITEM_CHECK_IN:
			CLook->ItemCheckIn(p->pos, p->slot_type);
			break;

		case ECG_TRANSMUTATION_SHEADER::ITEM_CHECK_OUT:
			CLook->ItemCheckOut(p->slot_type);
			break;

		case ECG_TRANSMUTATION_SHEADER::FREE_ITEM_CHECK_IN:
			CLook->FreeItemCheckIn(p->pos);
			break;

		case ECG_TRANSMUTATION_SHEADER::FREE_ITEM_CHECK_OUT:
			CLook->FreeItemCheckOut();
			break;

		case ECG_TRANSMUTATION_SHEADER::ACCEPT:
			CLook->Accept();
			break;

		case ECG_TRANSMUTATION_SHEADER::CANCEL:
			ch->SetTransmutation(nullptr);
			break;

		default:
			sys_err("Unknown Subheader ch:%s, %d", ch->GetName(), p->subheader);
			return;
	}
}
#endif

#ifdef ENABLE_HUNTING_SYSTEM
int CInputMain::ReciveHuntingAction(LPCHARACTER ch, const char* c_pData)
{
	TPacketGCHuntingAction* p = (TPacketGCHuntingAction*)c_pData;

	switch (p->bAction)
	{
		case 1:
			ch->OpenHuntingWindowMain();
			break;

		case 2:
			if (ch->GetQuestFlag("hunting_system.is_active") == -1)
			{

				if (ch->GetLevel() < ch->GetQuestFlag("hunting_system.level"))
					return 0;

				ch->SetQuestFlag("hunting_system.is_active", 1);
				ch->SetQuestFlag("hunting_system.type", p->dValue);
				ch->SetQuestFlag("hunting_system.count", 0);

				ch->OpenHuntingWindowMain();
			}
			else
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 200, "");
			break;

		case 3:
			ch->OpenHuntingWindowReward();
			break;

		case 4:
			ch->ReciveHuntingRewards();
			break;

		default:
			break;
	}

	return 0;
}
#endif

#ifdef ENABLE_AUTO_SELL_SYSTEM
void CInputMain::AutoSellAdd(LPCHARACTER ch, const char* data)
{
	const TPacketCGAutoSellAdd* p = reinterpret_cast<const TPacketCGAutoSellAdd*>(data);
	
	if (!ch)
		return;
		
	ch->AddAutoSellItem(p->vnum);
}

void CInputMain::AutoSellRemove(LPCHARACTER ch, const char* data)
{
	const TPacketCGAutoSellRemove* p = reinterpret_cast<const TPacketCGAutoSellRemove*>(data);
	
	if (!ch)
		return;
		
	ch->RemoveAutoSellItem(p->vnum);
}

void CInputMain::AutoSellStatus(LPCHARACTER ch, const char* data)
{
	const TPacketCGAutoSellStatus* p = reinterpret_cast<const TPacketCGAutoSellStatus*>(data);
	
	if (!ch)
		return;
		
	ch->SetAutoSellStatus(p->status != 0);
}
#endif
