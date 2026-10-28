#pragma once

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

class CPythonGameEvents : public CSingleton<CPythonGameEvents>
{
public:
	CPythonGameEvents();
	virtual ~CPythonGameEvents();

	void	SetActivateEvent(bool isActivate, BYTE bEventID);
	void	SetEventTime(BYTE bEventID, DWORD event_time);
	bool	IsActivateEvent(BYTE bEventID);
	DWORD	GetEventTime(BYTE bEventID);
protected:
	bool	m_pkActivateEvents[EVENT_MAX_NUM];
	DWORD	m_dwEventEndTime[EVENT_MAX_NUM];
private:
};