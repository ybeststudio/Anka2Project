#include "StdAfx.h"

#ifdef ENABLE_EVENT_MANAGER
#include "PythonGameEventManager.h"
#include <time.h>

InGameEventManager::InGameEventManager()
{
	m_bIsRequestEventDataed = false;
	m_map_EventReward.clear();
	m_mapEventName.clear();

	BuildEventMap();
	BuildEventNameMap();
}

InGameEventManager::~InGameEventManager()
{
}

bool InGameEventManager::LoadEventRewardList(const char *c_szFileName)
{
	if (!m_map_EventReward.empty())
		return true;

	CTextFileLoader TextFileLoader;
	if (!TextFileLoader.Load(c_szFileName))
		return false;

	TextFileLoader.SetTop();

	for (DWORD i = 0; i < TextFileLoader.GetChildNodeCount(); ++i)
	{
		if (!TextFileLoader.SetChildNode(i))
			continue;

		std::string strName;

		if (!TextFileLoader.GetCurrentNodeName(&strName))
			continue;

		TTokenVector* pTok;
		for (int k = 1; k <= REWARD_MAX_NUM; ++k)
		{
			if (TextFileLoader.GetTokenVector(std::to_string(k), &pTok))
			{
				if (pTok->size() == 2)
				{
					TEventReward e;
					e.dwVnum = atoi(pTok->at(0).c_str());
					e.dwCount = atoi(pTok->at(1).c_str());

					if (e.dwVnum && e.dwCount)
					{
						std::transform(strName.begin(), strName.end(), strName.begin(), toupper);
						const int iEvent = GetEvent(strName);
						if(iEvent)
							m_map_EventReward[iEvent].emplace_back(e);
					}
				}
			}
		}
		TextFileLoader.SetParentNode();
	}
	return true;
}

void InGameEventManager::BuildEventMap()
{
	m_map_Event.clear();

	for (int i = 1; i <= DAY_MAX_NUM; i++)
		m_map_Event[i].clear();
}

void InGameEventManager::BuildEventNameMap()
{
	m_mapEventName["EVENT_TYPE_NONE"] = EVENT_TYPE_NONE;
	m_mapEventName["EVENT_TYPE_EXPERIENCE"] = EVENT_TYPE_EXPERIENCE;
	m_mapEventName["EVENT_TYPE_ITEM_DROP"] = EVENT_TYPE_ITEM_DROP;
	m_mapEventName["EVENT_TYPE_BOSS"] = EVENT_TYPE_BOSS;
	m_mapEventName["EVENT_TYPE_METIN"] = EVENT_TYPE_METIN;
	m_mapEventName["EVENT_TYPE_MINING"] = EVENT_TYPE_MINING;
	m_mapEventName["EVENT_TYPE_GOLD_FROG"] = EVENT_TYPE_GOLD_FROG;
	m_mapEventName["EVENT_TYPE_MOONLIGHT"] = EVENT_TYPE_MOONLIGHT;
	m_mapEventName["EVENT_TYPE_HEXEGONAL_CHEST"] = EVENT_TYPE_HEXEGONAL_CHEST;
	m_mapEventName["EVENT_TYPE_FISHING"] = EVENT_TYPE_FISHING;
	m_mapEventName["EVENT_TYPE_HIDE_AND_SEEK"] = EVENT_TYPE_HIDE_AND_SEEK;
	m_mapEventName["EVENT_TYPE_OX"] = EVENT_TYPE_OX;
	m_mapEventName["EVENT_TYPE_TANAKA"] = EVENT_TYPE_TANAKA;
	m_mapEventName["EVENT_TYPE_YANG"] = EVENT_TYPE_YANG;
#ifdef ENABLE_MINI_GAME_OKEY
	m_mapEventName["EVENT_TYPE_OKEY_CARDS"] = EVENT_TYPE_OKEY_CARDS;
#endif
}

void InGameEventManager::AddEventData(std::vector<TEventTable>& eventVec)
{
	BuildEventMap();

	for (std::vector<TEventTable>::const_iterator itor = eventVec.begin(); itor < eventVec.end(); ++itor)
	{
		const TEventTable eventTable = *itor;
		const time_t startTime = (time_t)eventTable.startTime;
		const tm* t = localtime(&startTime);

		m_map_Event[t->tm_mday].emplace_back(eventTable);
	}
}

int InGameEventManager::GetEvent(const std::string strEventName)
{
	if (m_mapEventName.find(strEventName) != m_mapEventName.end())
		return m_mapEventName[strEventName];

	return EVENT_TYPE_NONE;
}

int InGameEventManager::GetDayEventCount(int iDay)
{
	if (iDay > DAY_MAX_NUM)
		return 0;

	return m_map_Event[iDay].size();
}

void InGameEventManager::GetEventData(int eventIdx, int iDay, TEventTable& table)
{
	if (iDay <= DAY_MAX_NUM)
	{
		EventVector vec = m_map_Event[iDay];
		if (eventIdx < vec.size())
		{
			const TEventTable t = vec.at(eventIdx);
			memcpy(&table, &t, sizeof(TEventTable));
		}
	}
}

void InGameEventManager::GetEventRewardData(int eventIdx, int pos, TEventReward& reward)
{
	const auto it = m_map_EventReward.find(eventIdx);
	if (it != m_map_EventReward.end())
	{
		auto& rewardVec = it->second;

		if (pos < rewardVec.size())
		{
			reward.dwVnum = rewardVec.at(pos).dwVnum;
			reward.dwCount = rewardVec.at(pos).dwCount;
			return;
		}
	}
}
#endif
