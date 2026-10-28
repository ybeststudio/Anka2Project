#include "stdafx.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "utils.h"
#include "config.h"
#include "constants.h"
#ifdef ENABLE_12ZI
#include "zodiac_temple_portal.h"
#endif
#ifdef ENABLE_EVENT_SYSTEM
#include "auto_event_list.h"
#endif
#ifdef ENABLE_AUTO_EVENTS
#include "auto_event_manager.h"
#include "questmanager.h"

static LPEVENT running_event = NULL;

EVENTINFO(EventsManagerInfoData)
{
	CEventsManager *pEvents;

	EventsManagerInfoData()
	: pEvents(0)
	{
	}
};

EVENTFUNC(automatic_event_timer)
{
	if (event == NULL)
		return 0;

	if (event->info == NULL)
		return 0;

	EventsManagerInfoData* info = dynamic_cast<EventsManagerInfoData*>(event->info);

	if (info == NULL)
		return 0;
	
	CEventsManager* pInstance = info->pEvents;

	if (pInstance == NULL)
		return 0;
	

	CEventsManager::instance().PrepareChecker();
	return PASSES_PER_SEC(1);
}

void CEventsManager::PrepareChecker()
{
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);

	int day = vKey.tm_wday;
	int hour = vKey.tm_hour;
	int minute = vKey.tm_min;
	int second = vKey.tm_sec;
#ifdef ENABLE_12ZI
	C12zi::instance().Check(day, hour, minute, second);
#endif
#ifdef ENABLE_EVENT_SYSTEM
	CEventsManager::instance().Check(day, hour, minute, second);
#endif
}

void CEventsManager::Check(int day, int hour, int minute, int second)
{
#ifdef ENABLE_EVENT_SYSTEM
	for (int i = 0; i < EVENT_MAX_NUM; ++i)
	{
		if (CGameEventsManager::instance().IsActivateEvent(i) == true)
		{
			if (CGameEventsManager::instance().GetEventTime(i) <= get_global_time())
			{
				CGameEventsManager::instance().SetActivateEvent(i, false);
				if (i == EVENT_FOOTBALL)
				{
					quest::CQuestManager::instance().SetEventFlag("football_event", 0);
				}
				else if (i == EVENT_AYISIGI)
				{
					quest::CQuestManager::instance().SetEventFlag("ayisigi_event", 0);
				}
				else if (i == EVENT_OKEYCARD)
				{
					quest::CQuestManager::instance().SetEventFlag("okeycard_event", 0);
				}
				else if (i == EVENT_FISH)
				{
					quest::CQuestManager::instance().SetEventFlag("enable_fish_event", 0);
				}
				else if (i == EVENT_ATTENDANCE)
				{
					quest::CQuestManager::instance().SetEventFlag("ex_enable_attendance_event", 0);
					quest::CQuestManager::instance().SetEventFlag("enable_attendance_event", 0);
				}
				else if (i == EVENT_CATCHKING)
				{
					quest::CQuestManager::instance().SetEventFlag("ex_enable_catch_king_event", 0);
					quest::CQuestManager::instance().SetEventFlag("enable_catch_king_event", 0);
				}
				else if (i == EVENT_WORD)
				{
					quest::CQuestManager::instance().SetEventFlag("word_event", 0);
				}
				else if (i == EVENT_HALLOWEEN)
				{
					quest::CQuestManager::instance().SetEventFlag("halloween_event", 0);
				}
				else if (i == EVENT_RITUELSOUL)
				{
					quest::CQuestManager::instance().SetEventFlag("ex_ritualsoul", 0);
					quest::CQuestManager::instance().SetEventFlag("xmas_soul", 0);
				}
				else if (i == EVENT_EASTER)
				{
					quest::CQuestManager::instance().SetEventFlag("ex_enable_easter_event", 0);
					quest::CQuestManager::instance().SetEventFlag("enable_easter_event", 0);
				}
				else if (i == EVENT_STONEKILL)
				{
					quest::CQuestManager::instance().SetEventFlag("stone_kill_event", 0);
				}
				else if (i == EVENT_XMAS)
				{
					quest::CQuestManager::instance().SetEventFlag("ex_xmas_drop", 0);
				}
				else if (i == EVENT_RAMADAN)
				{
					quest::CQuestManager::instance().SetEventFlag("ex_ramadan_drop", 0);
				}
				else if (i == EVENT_CHEQUEDESK)
				{
					quest::CQuestManager::instance().SetEventFlag("2019_cheque_desk_event", 0);
				}
			}
		}
	}
#endif
}

bool CEventsManager::Initialize()
{
	if (running_event != NULL)
	{
		event_cancel(&running_event);
		running_event = NULL;
	}
	
	EventsManagerInfoData* info = AllocEventInfo<EventsManagerInfoData>();
	info->pEvents = this;

	running_event = event_create(automatic_event_timer, info, PASSES_PER_SEC(1));
	return true;
}

void CEventsManager::Destroy()
{

	if (running_event != NULL)
	{
		event_cancel(&running_event);
		running_event = NULL;
	}
}
#endif
