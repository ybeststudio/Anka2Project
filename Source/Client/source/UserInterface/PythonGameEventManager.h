#pragma once

#ifdef ENABLE_EVENT_MANAGER
#include <unordered_map>

class InGameEventManager : public CSingleton<InGameEventManager>
{
	public:
		struct TEventTable
		{
			DWORD dwID;
			BYTE bType;
			long startTime;
			long endTime;
			DWORD dwVnum;
			int iPercent;
			int iDropType;
			bool bCompleted;
		};

		enum EEventTypes
		{
			EVENT_TYPE_NONE,
			EVENT_TYPE_EXPERIENCE,
			EVENT_TYPE_ITEM_DROP,
			EVENT_TYPE_BOSS,
			EVENT_TYPE_METIN,
			EVENT_TYPE_MINING,
			EVENT_TYPE_GOLD_FROG,
			EVENT_TYPE_MOONLIGHT,
			EVENT_TYPE_HEXEGONAL_CHEST,
			EVENT_TYPE_FISHING,
			EVENT_TYPE_HIDE_AND_SEEK,
			EVENT_TYPE_OX,
			EVENT_TYPE_TANAKA,
			EVENT_TYPE_YANG,
#ifdef ENABLE_MINI_GAME_OKEY
			EVENT_TYPE_OKEY_CARDS,
#endif
			EVENT_MAX_NUM
		};

		enum
		{
			MONTH_MAX_NUM = 11,
			DAY_MAX_NUM = 31,
			REWARD_MAX_NUM = 3,
		};

		InGameEventManager();
		~InGameEventManager();

		typedef struct SEventReward
		{
			DWORD dwVnum;
			DWORD dwCount;
		} TEventReward;

		bool	LoadEventRewardList(const char *c_szFileName);

		void	BuildEventMap();
		void	BuildEventNameMap();
		void	AddEventData(std::vector<TEventTable>& eventVec);

		int		GetEvent(const std::string strEventName);

		void	SetRequestEventData(bool isRequested) { m_bIsRequestEventDataed = isRequested; };
		bool	GetRequestEventData() { return m_bIsRequestEventDataed; };

		int		GetDayEventCount(int iDay);
		void	GetEventData(int eventIdx, int iDay, TEventTable& table);
		void	GetEventRewardData(int eventIdx, int pos, TEventReward& reward);

	private:
		typedef std::vector<TEventTable> EventVector;
		typedef std::map<int, EventVector> EventMap;

		EventVector m_vec_EventData;
		EventMap m_map_Event;

		std::unordered_map<int, std::vector<TEventReward>> m_map_EventReward;

		std::unordered_map<std::string, int> m_mapEventName;

		bool m_bIsRequestEventDataed;
};
#endif
