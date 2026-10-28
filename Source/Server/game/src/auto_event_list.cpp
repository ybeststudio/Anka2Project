#include "stdafx.h"
#include "auto_event_list.h"
#include "char.h"
#include "packet.h"
#include "desc.h"
#include "quest.h"
#include "questmanager.h"
#include "utils.h"
#include "p2p.h"

CGameEventsManager::CGameEventsManager()
{
	// Server baþlangýcýnda tüm eventleri temizle
	for (int i = 0; i < EVENT_MAX_NUM; ++i)
	{
		m_pkActivateEvents[i] = false;
		m_dwEventEndTime[i] = 0;
		m_bLimitedEvent[i] = false;
	}
	
	sys_log(0, "CGameEventsManager: Tüm eventler temizlendi (reboot sonrasý)");
}

CGameEventsManager::~CGameEventsManager()
{
}

void CGameEventsManager::SendEventCharacter(LPCHARACTER ch)
{
	if (!ch)
		return;

	DWORD currentTime = get_global_time();

	for (int i = 0; i < EVENT_MAX_NUM; ++i)
	{
		bool isActivate = IsActivateEvent(i);
		DWORD eventTime = GetEventTime(i);
		
		// Eðer etkinlik aktifse ama zamaný dolmuþsa, etkinliði deaktif olarak gönder
		if (isActivate && eventTime > 0 && eventTime <= currentTime)
		{
			isActivate = false;
			eventTime = 0;
		}
		
		TPacketGCEventInfo pPacket;
		pPacket.bHeader = HEADER_GC_EVENT_INFO;
		pPacket.event_id = i;
		pPacket.isActivate = isActivate;
		pPacket.event_time = eventTime;
		ch->GetDesc()->Packet(&pPacket, sizeof(TPacketGCEventInfo));
	}
}

void CGameEventsManager::SetActivateEvent(BYTE bEventID, bool isActivate)
{
	// Event info.
	if (isActivate)
		sys_log(0, "<--! Event Activated (%d) !-->", bEventID);
	else
		sys_log(0, "<--! Event Deactivated (%d) !-->", bEventID);

	m_pkActivateEvents[bEventID] = isActivate;
}

void CGameEventsManager::SetEventTime(BYTE bEventID, DWORD eventTime, bool bP2P)
{
	// Event info.
	sys_log(0, "<--! Event SetTime (%d) !-->", eventTime);

	m_dwEventEndTime[bEventID] = get_global_time() + eventTime;
	m_bLimitedEvent[bEventID] = true;

	// P2P
	if (bP2P)
	{
		TPacketGGEventInfo pack_p2p;
		pack_p2p.header = HEADER_GG_EVENT_TIME;
		pack_p2p.event_id = bEventID;
		pack_p2p.event_time = eventTime;
		P2P_MANAGER::instance().Send(&pack_p2p, sizeof(pack_p2p));
	}
}

bool CGameEventsManager::IsActivateEvent(BYTE bEventID)
{
	if (bEventID >= EVENT_MAX_NUM)
		return false;
	
	// Eðer etkinlik aktifse ama zamaný dolmuþsa, false döndür
	if (m_pkActivateEvents[bEventID])
	{
		DWORD eventTime = m_dwEventEndTime[bEventID];
		if (eventTime > 0 && eventTime <= get_global_time())
			return false;
	}
	
	return m_pkActivateEvents[bEventID];
}

DWORD CGameEventsManager::GetEventTime(BYTE bEventID)
{
	if (bEventID >= EVENT_MAX_NUM)
		return 0;
	
	// Eðer zamaný dolmuþ etkinlik varsa, 0 döndür
	DWORD eventTime = m_dwEventEndTime[bEventID];
	if (eventTime > 0 && eventTime <= get_global_time())
		return 0;
	
	return eventTime;
}

bool CGameEventsManager::IsLimitedEvent(BYTE bEventID)
{
	return m_bLimitedEvent[bEventID];
}