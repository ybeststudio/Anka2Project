#pragma once

#ifdef ENABLE_EVENT_MANAGER
#include "../../common/tables.h"
#include <unordered_map>

class CEventManager : public singleton<CEventManager>
{
	public:
		CEventManager();
		~CEventManager();

		void Enqueue(BYTE bQueue, TEventTable* table);
		void Dequeue(BYTE bQueue, TEventTable* table);
		void CancelActiveEvents();

		void Initialize(TEventTable* pTab, int iSize, bool bReload = false);
		void UpdateEventStatus(DWORD dwID);

		int GetEvent(const std::string strEventName);
		std::string GetEventString(int iEvent);
		void SendEventInfo(const LPCHARACTER ch, int iMonth);

		void BuildEventNameMap();
		bool GetEventState(int iEvent);
		void SetEventState(TEventTable* table, bool bState);
		void UpdateGameFlag(const char* c_szFlag, bool bState);

		void SetReloadMode(bool bReload) { m_bReload = bReload; }
		bool GetReloadMode() { return m_bReload; }

		bool SetExperienceEvent(bool bActive, const TEventTable* table);
		bool SetItemDropEvent(bool bActive, const TEventTable* table);
		bool SetBossEvent(bool bActive);
		bool SetMetinEvent(bool bActive);
		bool SetMiningEvent(bool bActive);
		bool SetGoldFrogEvent(bool bActive);
		bool SetMoonlightEvent(bool bActive, const TEventTable* table);
		bool SetHexegonalEvent(bool bActive, const TEventTable* table);
		bool SetFishingEvent(bool bActive);
		bool SetHideAndSeekEvent(bool bActive);
		bool SetOXEvent(bool bActive);
		bool SetTanakaEvent(bool bActive, TEventTable* table);
		bool SetYangEvent(bool bActive, const TEventTable* table);
#ifdef ENABLE_MINIGAME_OKEY_CARDS_SYSTEM
		bool SetOkeyCardsEvent(bool bActive, const TEventTable* table);
#endif

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
#ifdef ENABLE_MINIGAME_OKEY_CARDS_SYSTEM
			EVENT_TYPE_OKEY_CARDS,
#endif
			EVENT_MAX_NUM
		};

		enum EEvent
		{
			CLEAR_ENTITY_STAGE1_ROUND = 1,
			CLEAR_ENTITY_STAGE2_ROUND = 6,

			DYNAMIC_CYCLE_STAGE1 = 5,
			DYNAMIC_CYCLE_STAGE2 = 10,

			STATIC_CYCLE_MAX_ROUND = 3,

			TANAKA_EVENT_MAP = 63,
			EVENT_MAP_INDEX = 230,

			EVENT_CHANNEL = 99,
		};

		enum EQueueType
		{
			QUEUE_TYPE_NONE,
			QUEUE_TYPE_START,
			QUEUE_TYPE_END,
		};

		enum EDropTypes
		{
			DROP_TYPE_NONE = 1,
			DROP_TYPE_GENERAL,
			DROP_TYPE_BOSS_AND_MORE,
			DROP_TYPE_BOSS,
			DROP_TYPE_STONE,
			DROP_TYPE_MAX
		};

	private:
		std::unordered_map<std::string, int> m_mapEventName;
		std::unordered_map<DWORD, TEventTable> m_mapEvent;

		typedef std::unordered_map<DWORD, LPEVENT> EventQueue;
		EventQueue m_mapEventStartQueue;
		EventQueue m_mapEventEndQueue;

		bool m_bReload;
};
#endif