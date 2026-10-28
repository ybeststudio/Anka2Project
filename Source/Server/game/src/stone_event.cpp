#include "stdafx.h"
#include "vector.h"
#include "char.h"
#include "utils.h"
#include "log.h"
#include "db.h"
#include "locale_service.h"
#include <stdlib.h>
#include "guild.h"
#include "guild_manager.h"
#include <sstream>
#include "config.h"
#include "desc.h"
#include "desc_manager.h"
#include "char_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "desc_client.h"
#include "group_text_parse_tree.h"
#include <boost/algorithm/string/predicate.hpp>
#include <cctype>
#include "p2p.h"
#include "entity.h"
#include "sectree_manager.h"
#include "stone_event.h"
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include "questmanager.h"

#ifdef ENABLE_STONE_EVENT_SYSTEM
CStoneEvent::CStoneEvent(){}
CStoneEvent::~CStoneEvent(){}

void CStoneEvent::DeleteStoneEvent()
{
	m_Stone.clear();
	OnePlayer = 0;
	OnePlayerPoint = 0;
}

void CStoneEvent::RewardItem()
{
	if (IsStoneEventSystemStatus() == false)
		return;
	
	int item_vnum = 32142;
	int item_count = 1;

	// if (OnePlayer <= 0)
	// {
		// char winner_notice[256];
		// snprintf(winner_notice, sizeof(winner_notice), LC_TEXT("<Metin taþý etkinliði> Kimse metin taþý kesmediði için kazanan olmadý."));
		// SendNotice(winner_notice);
		// return;
	// }
	
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(OnePlayer);

	if (ch && ch->IsPC())
	{
		DBManager::instance().Query("INSERT INTO player.mail_box SET reader_name = '%s', sender_name  = 'Etkinlik Ödülü', title = 'Etkinlik Ödülü', type = 2, time = NOW(), post = 'Metin taþý etkinliðine katýldýgýn için teþekkür ederiz sana bir ödülümüz var', item_count = %d , item_vnum = %d;", ch->GetName(), item_count, item_vnum);
		char winner_notice[256];
		snprintf(winner_notice, sizeof(winner_notice), LC_TEXT("<Metin Taþý Etkinliði> Metin Taþý Etkinliði sona erdi. Etkinliðin kazananý %s olmuþtur."), ch->GetName());
		SendNotice(winner_notice);

		// if (ch->GetDesc() && ch->IsPC())
			// ch->ChatPacket(CHAT_TYPE_COMMAND, "MailBoxOpen");

		DeleteStoneEvent();
	}
}

void CStoneEvent::StoneInformation(LPCHARACTER pkChr)
{
	if (IsStoneEventSystemStatus() == false)
		return;
		
	if (!pkChr)
		return;
	
	if (!pkChr->GetDesc())
		return;
	
	if (!pkChr->IsPC())
		return;
	
	if (IsStoneEvent() == false)
	{
		pkChr->ChatPacket(CHAT_TYPE_COMMAND, "IsStoneEvent 0");
		return;
	}
	else
	{
		pkChr->ChatPacket(CHAT_TYPE_COMMAND, "IsStoneEvent 1");
	}
	

	TPacketGCStoneEvent p;
	p.header 		= HEADER_GC_STONE_EVENT;
	p.stone_point	= GetStoneEventPoint(pkChr);

	if (pkChr->GetDesc() != NULL)
		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCStoneEvent));

	pkChr->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Metin taþý etkinliði> Kazanýlan Puan : %d - Toplam Puan : %d"), STONE_EVENT_POINT, GetStoneEventPoint(pkChr));
}

void CStoneEvent::SetStoneKill(DWORD PID)
{
	if (IsStoneEventSystemStatus() == true)
	{
		if (!PID)
		{
			sys_err("StoneEvent PID Not Found");
			return;
		}
		
		//boost::unordered_map<DWORD, short>::iterator it = m_Stone.find(PID);
		auto it = m_Stone.find(PID); // c++11
		if (it == m_Stone.end())
		{
			m_Stone.insert(std::make_pair(PID, STONE_EVENT_POINT));
			if (OnePlayer <= 0)
			{
				OnePlayer = PID;
				OnePlayerPoint = STONE_EVENT_POINT;
			}
		}
		else
		{
			it->second += STONE_EVENT_POINT;
			if (it->second > OnePlayerPoint)
			{
				OnePlayer = it->first;
				OnePlayerPoint = it->second;
			}
		}
		
		// LPCHARACTER ch_one = CHARACTER_MANAGER::instance().FindByPID(OnePlayer);
		//pkKiller->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Metin taþý etkinliði> Metin taþý etkinliðinde %s oyuncusu birinci olarak devam ediyor."), ch_one->GetName());
	}
}

void CStoneEvent::StoneUpdateP2PPacket(DWORD PID)
{
	if (IsStoneEventSystemStatus() == true)
	{
		if (PID != 0)
		{
			/*** Send P2P Packet Start ***/
			TPacketGGStoneEvent p;
			p.header = HEADER_GG_STONE_EVENT;
			p.pid = PID;
			P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGStoneEvent));
			/*** Send P2P Packet End ***/
		}
	}
}

short CStoneEvent::GetStoneEventPoint(LPCHARACTER pkChr)
{
	if (IsStoneEventSystemStatus() == true)
	{
		if (pkChr)
		{
			if (pkChr->GetDesc())
			{
				if (pkChr->IsPC())
				{
					//boost::unordered_map<DWORD, short>::iterator it = m_Stone.find(pkChr->GetPlayerID());
					auto it = m_Stone.find(pkChr->GetPlayerID()); // c++11
					if (it == m_Stone.end())
						return 0;
					else
						return it->second;
				}
			}
		}
	}
}

bool CStoneEvent::IsStoneEventSystemStatus()
{
	if (quest::CQuestManager::instance().GetEventFlag("enable_stone_event_system") == 1)
		return false;

	return true;
}

bool CStoneEvent::IsStoneEvent()
{
	if (quest::CQuestManager::instance().GetEventFlag("stone_kill_event") == 0)
		return false;

	return true;
}
#endif