#include "stdafx.h"
#include "../../common/service.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "party.h"
#include "messenger_manager.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "xmas_event.h"
#include "affect.h"
#include "locale_service.h"
#include "questmanager.h"
#include "skill.h"

#ifdef ENABLE_RENEWAL_SWITCHBOT
	#include "switchbot_manager.h"
#endif

#ifdef ENABLE_CROSS_CHANNEL_REQUESTS
	#include "crc32.h"
#endif

#ifdef ENABLE_MAINTENANCE_SYSTEM
	#include "maintenance.h"

	extern long int global_time_maintenance;
#endif

#ifdef ENABLE_RELOAD_COMMAND_ALL_CORES
ACMD(do_reload);
#endif

#ifdef ENABLE_STONE_EVENT_SYSTEM
	#include "stone_event.h"
#endif

#ifdef ENABLE_EVENT_SYSTEM
	#include "auto_event_list.h"
#endif
#ifdef ENABLE_EVENT_SYSTEM
	#include "auto_event_list.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// Input Processor
CInputP2P::CInputP2P()
{
	BindPacketInfo(&m_packetInfoGG);
}

void CInputP2P::Login(LPDESC d, const char * c_pData)
{
	P2P_MANAGER::instance().Login(d, (TPacketGGLogin *) c_pData);
}

void CInputP2P::Logout(LPDESC d, const char * c_pData)
{
	TPacketGGLogout * p = (TPacketGGLogout *) c_pData;
	P2P_MANAGER::instance().Logout(p->szName);
}

int CInputP2P::Relay(LPDESC d, const char * c_pData, size_t uiBytes)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return -1;
#endif
	TPacketGGRelay * p = (TPacketGGRelay *) c_pData;

	if (uiBytes < sizeof(TPacketGGRelay) + p->lSize)
		return -1;

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	sys_log(0, "InputP2P::Relay : %s size %d", p->szName, p->lSize);

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(p->szName);

	const BYTE* c_pbData = (const BYTE *) (c_pData + sizeof(TPacketGGRelay));

	if (!pkChr)
		return p->lSize;

	if (*c_pbData == HEADER_GC_WHISPER)
	{
		if (pkChr->IsBlockMode(BLOCK_WHISPER))
		{
			return p->lSize;
		}

		char buf[1024];
		memcpy(buf, c_pbData, MIN(p->lSize, sizeof(buf)));

		TPacketGCWhisper* p2 = (TPacketGCWhisper*) buf;

		BYTE bToEmpire = (p2->bType >> 4);
		p2->bType = p2->bType & 0x0F;
		if (p2->bType == 0x0F)
		{
			p2->bType = WHISPER_TYPE_SYSTEM;
		}
		else
		{
			if (!pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
				if (bToEmpire >= 1 && bToEmpire <= 3 && pkChr->GetEmpire() != bToEmpire)
				{
					ConvertEmpireText(bToEmpire,
						buf + sizeof(TPacketGCWhisper),
						p2->wSize - sizeof(TPacketGCWhisper),
						10 + 2 * pkChr->GetSkillPower(SKILL_LANGUAGE1 + bToEmpire - 1));
				}
		}

		pkChr->GetDesc()->Packet(buf, p->lSize);
	}
	else
		pkChr->GetDesc()->Packet(c_pbData, p->lSize);

	return (p->lSize);
}

int CInputP2P::Notice(LPDESC d, const char * c_pData, size_t uiBytes)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return -1;
#endif
	TPacketGGNotice * p = (TPacketGGNotice *) c_pData;

	if (uiBytes < sizeof(TPacketGGNotice) + p->lSize)
		return -1;

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char szBuf[256+1];
	strlcpy(szBuf, c_pData + sizeof(TPacketGGNotice), MIN(p->lSize + 1, sizeof(szBuf)));
	SendNotice(szBuf);
	return (p->lSize);
}

#ifdef ENABLE_CLIENT_LOCALE_STRING
int CInputP2P::LocaleChatNotice(LPDESC d, const char * c_pData, size_t uiBytes)
{
	TPacketGGLocaleChatNotice* p = (TPacketGGLocaleChatNotice *) c_pData;
	if (uiBytes < sizeof(TPacketGGLocaleChatNotice) + p->size)
	{
		return -1;
	}

	if (p->size < 0)
	{
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char chatbuf[256 + 1];
	strlcpy(chatbuf, c_pData + sizeof(TPacketGGLocaleChatNotice), p->size + 1);

	SendLocaleNotice(p->type, p->empire, p->mapidx, p->id, chatbuf);
	return p->size;
}
#endif

int CInputP2P::Guild(LPDESC d, const char* c_pData, size_t uiBytes)
{
	TPacketGGGuild * p = (TPacketGGGuild *) c_pData;
	uiBytes -= sizeof(TPacketGGGuild);
	c_pData += sizeof(TPacketGGGuild);

	CGuild * g = CGuildManager::instance().FindGuild(p->dwGuild);

	switch (p->bSubHeader)
	{
		case GUILD_SUBHEADER_GG_CHAT:
			{
				if (uiBytes < sizeof(TPacketGGGuildChat))
					return -1;

				TPacketGGGuildChat * p = (TPacketGGGuildChat *) c_pData;

				if (g)
					g->P2PChat(p->szText);

				return sizeof(TPacketGGGuildChat);
			}

		case GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS:
			{
				if (uiBytes < sizeof(int))
					return -1;

				int iBonus = *((int *) c_pData);
				CGuild* pGuild = CGuildManager::instance().FindGuild(p->dwGuild);
				if (pGuild)
				{
					pGuild->SetMemberCountBonus(iBonus);
				}
				return sizeof(int);
			}
		default:
			sys_err ("UNKNOWN GUILD SUB PACKET");
			break;
	}
	return 0;
}

struct FuncShout
{
	const char * m_str;
	BYTE m_bEmpire;
#ifdef ENABLE_MESSENGER_BLOCK
	const char* m_szName;
#endif

	FuncShout(const char * str, BYTE bEmpire
#ifdef ENABLE_MESSENGER_BLOCK
		, const char* c_szName
#endif
	) : m_str(str), m_bEmpire(bEmpire)
#ifdef ENABLE_MESSENGER_BLOCK
		, m_szName(c_szName)
#endif
	{}

	void operator () (LPDESC d)
	{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
		if (!d)
			return;
#endif
		if (!d->GetCharacter())
			return;

		d->GetCharacter()->ChatPacket(CHAT_TYPE_SHOUT, "%s", m_str);

#ifdef ENABLE_MESSENGER_BLOCK
		if (MessengerManager::Instance().IsBlocked(d->GetCharacter()->GetName(), m_szName))
			return;
#endif
	}
};

void SendShout(const char * szText, BYTE bEmpire
#ifdef ENABLE_MESSENGER_BLOCK
, const char* c_szName
#endif
)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), FuncShout(szText, bEmpire
#ifdef ENABLE_MESSENGER_BLOCK
		, c_szName
#endif
	));
}

void CInputP2P::Shout(const char * c_pData)
{
	TPacketGGShout * p = (TPacketGGShout *) c_pData;
	SendShout(p->szText, p->bEmpire
#ifdef ENABLE_MESSENGER_BLOCK
		, p->szName
#endif
	);
}

void CInputP2P::Disconnect(const char * c_pData)
{
	TPacketGGDisconnect * p = (TPacketGGDisconnect *) c_pData;

	LPDESC d = DESC_MANAGER::instance().FindByLoginName(p->szLogin);

	if (!d)
		return;

	if (!d->GetCharacter())
	{
		d->SetPhase(PHASE_CLOSE);
	}
	else
		d->DisconnectOfSameLogin();
}

void CInputP2P::Setup(LPDESC d, const char * c_pData)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	TPacketGGSetup * p = (TPacketGGSetup *) c_pData;
	sys_log(0, "P2P: Setup %s:%d", d->GetHostName(), p->wPort);
	d->SetP2P(d->GetHostName(), p->wPort, p->bChannel);
}

#ifdef ENABLE_CROSS_CHANNEL_REQUESTS
void CInputP2P::MessengerRequestAdd(const char* c_pData)
{
	TPacketGGMessengerRequest* p = (TPacketGGMessengerRequest*)c_pData;
	sys_log(0, "P2P: Messenger: Friend Request from %s to %s", p->account, p->target);

	LPCHARACTER tch = CHARACTER_MANAGER::Instance().FindPC(p->target);
	MessengerManager::instance().P2PRequestToAdd_Stage2(p->account, tch);
}
#endif

void CInputP2P::MessengerAdd(const char * c_pData)
{
	TPacketGGMessenger * p = (TPacketGGMessenger *) c_pData;
	sys_log(0, "P2P: Messenger Add %s %s", p->szAccount, p->szCompanion);
	MessengerManager::instance().__AddToList(p->szAccount, p->szCompanion);
}

void CInputP2P::MessengerRemove(const char * c_pData)
{
	TPacketGGMessenger * p = (TPacketGGMessenger *) c_pData;
	sys_log(0, "P2P: Messenger Remove %s %s", p->szAccount, p->szCompanion);
	MessengerManager::instance().__RemoveFromList(p->szAccount, p->szCompanion);
}

#ifdef ENABLE_MESSENGER_BLOCK
void CInputP2P::MessengerBlockAdd(const char * c_pData)
{
	TPacketGGMessenger * p = (TPacketGGMessenger *) c_pData;
	MessengerManager::instance().__AddToBlockList(p->szAccount, p->szCompanion);
}

void CInputP2P::MessengerBlockRemove(const char * c_pData)
{
	TPacketGGMessenger * p = (TPacketGGMessenger *) c_pData;
	MessengerManager::instance().__RemoveFromBlockList(p->szAccount, p->szCompanion);
}
#endif

void CInputP2P::FindPosition(LPDESC d, const char* c_pData)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	TPacketGGFindPosition* p = (TPacketGGFindPosition*) c_pData;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(p->dwTargetPID);
	if (ch && ch->GetMapIndex() < 10000)
	{
		TPacketGGWarpCharacter pw;
		pw.header = HEADER_GG_WARP_CHARACTER;
		pw.pid = p->dwFromPID;
		pw.x = ch->GetX();
		pw.y = ch->GetY();
		d->Packet(&pw, sizeof(pw));
	}
}

void CInputP2P::WarpCharacter(const char* c_pData)
{
	TPacketGGWarpCharacter* p = (TPacketGGWarpCharacter*) c_pData;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(p->pid);
	if (ch)
	{
		ch->WarpSet(p->x, p->y);
	}
}

void CInputP2P::GuildWarZoneMapIndex(const char* c_pData)
{
	TPacketGGGuildWarMapIndex * p = (TPacketGGGuildWarMapIndex*) c_pData;
	CGuildManager & gm = CGuildManager::instance();

	sys_log(0, "P2P: GuildWarZoneMapIndex g1(%u) vs g2(%u), mapIndex(%d)", p->dwGuildID1, p->dwGuildID2, p->lMapIndex);

	CGuild * g1 = gm.FindGuild(p->dwGuildID1);
	CGuild * g2 = gm.FindGuild(p->dwGuildID2);

	if (g1 && g2)
	{
		g1->SetGuildWarMapIndex(p->dwGuildID2, p->lMapIndex);
		g2->SetGuildWarMapIndex(p->dwGuildID1, p->lMapIndex);
	}
}

void CInputP2P::Transfer(const char * c_pData)
{
	TPacketGGTransfer * p = (TPacketGGTransfer *) c_pData;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(p->szName);

	if (ch)
		ch->WarpSet(p->lX, p->lY);
}

void CInputP2P::XmasWarpSanta(const char * c_pData)
{
	TPacketGGXmasWarpSanta * p =(TPacketGGXmasWarpSanta *) c_pData;

	if (p->bChannel == g_bChannel && map_allow_find(p->lMapIndex))
	{
		int	iNextSpawnDelay = 50 * 60;

		xmas::SpawnSanta(p->lMapIndex, iNextSpawnDelay);

		TPacketGGXmasWarpSantaReply pack_reply;
		pack_reply.bHeader = HEADER_GG_XMAS_WARP_SANTA_REPLY;
		pack_reply.bChannel = g_bChannel;
		P2P_MANAGER::instance().Send(&pack_reply, sizeof(pack_reply));
	}
}

void CInputP2P::XmasWarpSantaReply(const char* c_pData)
{
	TPacketGGXmasWarpSantaReply* p = (TPacketGGXmasWarpSantaReply*) c_pData;

	if (p->bChannel == g_bChannel)
	{
		CharacterVectorInteractor i;

		if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(xmas::MOB_SANTA_VNUM, i))
		{
			CharacterVectorInteractor::iterator it = i.begin();

			while (it != i.end()) {
				M2_DESTROY_CHARACTER(*it++);
			}
		}
	}
}

void CInputP2P::LoginPing(LPDESC d, const char * c_pData)
{
	TPacketGGLoginPing * p = (TPacketGGLoginPing *) c_pData;

	if (!g_pkAuthMasterDesc) // If I am master, I have to broadcast
		P2P_MANAGER::instance().Send(p, sizeof(TPacketGGLoginPing), d);
}

// BLOCK_CHAT
void CInputP2P::BlockChat(const char * c_pData)
{
	TPacketGGBlockChat * p = (TPacketGGBlockChat *) c_pData;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(p->szName);

	if (ch)
	{
		sys_log(0, "BLOCK CHAT apply name %s dur %d", p->szName, p->lBlockDuration);
		ch->AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, p->lBlockDuration, 0, true);
	}
	else
	{
		sys_log(0, "BLOCK CHAT fail name %s dur %d", p->szName, p->lBlockDuration);
	}
}
// END_OF_BLOCK_CHAT
//

void CInputP2P::IamAwake(LPDESC d, const char * c_pData)
{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
	if (!d)
		return;
#endif
	std::string hostNames;
	P2P_MANAGER::instance().GetP2PHostNames(hostNames);
	sys_log(0, "P2P Awakeness check from %s. My P2P connection number is %d. and details...\n%s", d->GetHostName(), P2P_MANAGER::instance().GetDescCount(), hostNames.c_str());
}

#ifdef ENABLE_RENEWAL_SWITCHBOT
void CInputP2P::Switchbot(LPDESC d, const char* c_pData)
{
	const TPacketGGSwitchbot* p = reinterpret_cast<const TPacketGGSwitchbot*>(c_pData);
	if (p->wPort != mother_port)
	{
		return;
	}

	CSwitchbotManager::Instance().P2PReceiveSwitchbot(p->table);
}
#endif

#ifdef ENABLE_STONE_EVENT_SYSTEM
void CInputP2P::StoneEvent(LPDESC d, const char * c_pData)
{
	TPacketGGStoneEvent * p = (TPacketGGStoneEvent *) c_pData;
	CStoneEvent::instance().SetStoneKill(p->pid);
}
#endif

int CInputP2P::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	if (test_server)
		sys_log(0, "CInputP2P::Anlayze[Header %d]", bHeader);

	int iExtraLen = 0;

	switch (bHeader)
	{
		case HEADER_GG_SETUP:
			Setup(d, c_pData);
			break;

		case HEADER_GG_LOGIN:
			Login(d, c_pData);
			break;

		case HEADER_GG_LOGOUT:
			Logout(d, c_pData);
			break;

		case HEADER_GG_RELAY:
			if ((iExtraLen = Relay(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_GG_NOTICE:
			if ((iExtraLen = Notice(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_GG_SHUTDOWN:
			sys_err("Accept shutdown p2p command from %s.", d->GetHostName());
#ifdef ENABLE_MAINTENANCE_SYSTEM
			Shutdown(global_time_maintenance);
#else
			Shutdown(10);
#endif
			break;

		case HEADER_GG_GUILD:
			if ((iExtraLen = Guild(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;

		case HEADER_GG_SHOUT:
			Shout(c_pData);
			break;

		case HEADER_GG_DISCONNECT:
			Disconnect(c_pData);
			break;

#ifdef ENABLE_CROSS_CHANNEL_REQUESTS
		case HEADER_GG_MESSENGER_REQUEST_ADD:
			MessengerRequestAdd(c_pData);
			break;
#endif

		case HEADER_GG_MESSENGER_ADD:
			MessengerAdd(c_pData);
			break;

#ifdef ENABLE_MESSENGER_BLOCK
		case HEADER_GG_MESSENGER_BLOCK_ADD:
			MessengerBlockAdd(c_pData);
			break;

		case HEADER_GG_MESSENGER_BLOCK_REMOVE:
			MessengerBlockRemove(c_pData);
			break;
#endif

		case HEADER_GG_MESSENGER_REMOVE:
			MessengerRemove(c_pData);
			break;

		case HEADER_GG_FIND_POSITION:
			FindPosition(d, c_pData);
			break;

		case HEADER_GG_WARP_CHARACTER:
			WarpCharacter(c_pData);
			break;

		case HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX:
			GuildWarZoneMapIndex(c_pData);
			break;

		case HEADER_GG_TRANSFER:
			Transfer(c_pData);
			break;

		case HEADER_GG_XMAS_WARP_SANTA:
			XmasWarpSanta(c_pData);
			break;

		case HEADER_GG_XMAS_WARP_SANTA_REPLY:
			XmasWarpSantaReply(c_pData);
			break;

		case HEADER_GG_RELOAD_CRC_LIST:
			LoadValidCRCList();
			break;

		case HEADER_GG_LOGIN_PING:
			LoginPing(d, c_pData);
			break;

		case HEADER_GG_BLOCK_CHAT:
			BlockChat(c_pData);
			break;

		case HEADER_GG_CHECK_AWAKENESS:
			IamAwake(d, c_pData);
			break;

#ifdef ENABLE_ULTIMATE_REGEN
		case HEADER_GG_NEW_REGEN:
			NewRegen(c_pData);
			break;
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
		case HEADER_GG_SWITCHBOT:
			Switchbot(d, c_pData);
			break;
#endif

#ifdef ENABLE_RELOAD_COMMAND_ALL_CORES
		case HEADER_GG_RELOAD_COMMAND:
			do_reload(NULL, ((TPacketGGReloadCommand*)c_pData)->argument, 0, 0);
			break;
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
		case HEADER_GG_MULTI_FARM:
			MultiFarm(c_pData);
			break;
#endif

#ifdef ENABLE_CLIENT_LOCALE_STRING
		case HEADER_GG_LOCALE_CHAT_NOTICE:
			if ((iExtraLen = LocaleChatNotice(d, c_pData, m_iBufferLeft)) < 0)
				return -1;
			break;
#endif

#ifdef ENABLE_STONE_EVENT_SYSTEM
		case HEADER_GG_STONE_EVENT:
			StoneEvent(d, c_pData);
			break;
#endif

#ifdef ENABLE_EVENT_SYSTEM
		case HEADER_GG_EVENT_TIME:
			EventTime(c_pData);
			break;
#endif

	}

	return (iExtraLen);
}

#ifdef ENABLE_EVENT_SYSTEM
void CInputP2P::EventTime(const char* c_pData)
{
	TPacketGGEventInfo* p = (TPacketGGEventInfo*)c_pData;
	CGameEventsManager::instance().SetEventTime(p->event_id, p->event_time, false);
}
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
void CInputP2P::MultiFarm(const char* c_pData)
{
	TPacketGGMultiFarm* p = (TPacketGGMultiFarm*)c_pData;
	if(p->subHeader == MULTI_FARM_SET)
		CHARACTER_MANAGER::instance().CheckMultiFarmAccount(p->playerIP, p->playerID, p->playerName, p->farmStatus, p->affectType, p->affectTime, true);
	else if (p->subHeader == MULTI_FARM_REMOVE)
		CHARACTER_MANAGER::instance().RemoveMultiFarm(p->playerIP, p->playerID, true);
}
#endif

#ifdef ENABLE_ULTIMATE_REGEN
#include "new_mob_timer.h"
void CInputP2P::NewRegen(const char* c_pData)
{
	TGGPacketNewRegen* p = (TGGPacketNewRegen*)c_pData;
	if (p->subHeader == NEW_REGEN_LOAD)
	{
		char buf[250];
		snprintf(buf, sizeof(buf), "%s/newregen.txt", LocaleService_GetBasePath().c_str());
		CNewMobTimer::Instance().LoadFile(buf);
		CNewMobTimer::Instance().UpdatePlayers();
		sys_log(0, "Reloading New Regen");
	}
	else if (p->subHeader == NEW_REGEN_REFRESH)
	{
		CNewMobTimer::Instance().UpdateNewRegen(p->id, p->isAlive, true);
	}
}
#endif

