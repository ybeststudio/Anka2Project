#pragma once
#include "stdafx.h"
enum
{
	EVENT_FOOTBALL				= 0,
	EVENT_AYISIGI				= 1,
	EVENT_OKEYCARD				= 2,
	EVENT_FISH					= 3,
	EVENT_ATTENDANCE			= 4,
	EVENT_CATCHKING				= 5,
	EVENT_WORD					= 6,
	EVENT_HALLOWEEN				= 7,
	EVENT_RITUELSOUL			= 8,
	EVENT_EASTER				= 9,
	EVENT_STONEKILL				= 10,
	EVENT_XMAS					= 11,
	EVENT_RAMADAN				= 12,
	EVENT_CHEQUEDESK			= 13,
	EVENT_MAX_NUM				= 14,
};

class CGameEventsManager : public singleton<CGameEventsManager>
{
public:
	CGameEventsManager();
	~CGameEventsManager();

	void	SendEventCharacter(LPCHARACTER ch);
	void	SetActivateEvent(BYTE bEventID, bool isActivate);
	void	SetEventTime(BYTE bEventID, DWORD eventTime, bool bP2P = true);
	bool	IsActivateEvent(BYTE bEventID);
	bool	IsLimitedEvent(BYTE bEventID);
	DWORD	GetEventTime(BYTE bEventID);
protected:
	bool	m_pkActivateEvents[EVENT_MAX_NUM];
	DWORD	m_dwEventEndTime[EVENT_MAX_NUM];
	bool	m_bLimitedEvent[EVENT_MAX_NUM];
private:
};