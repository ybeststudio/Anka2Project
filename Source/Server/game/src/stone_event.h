#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

class CStoneEvent : public singleton<CStoneEvent>
{
	public:
		CStoneEvent();
		enum EStoneEvent
		{
			STONE_EVENT_POINT = 1,
		};
		DWORD		OnePlayer = 0;
		DWORD		OnePlayerPoint = 0;
		virtual		~CStoneEvent();
		void		SetStoneKill(DWORD PID);
		void		StoneInformation(LPCHARACTER pkChr);
		void		DeleteStoneEvent();
		void		RewardItem();
		short		GetStoneEventPoint(LPCHARACTER pkChr);
		bool		IsStoneEvent();
		bool		IsStoneEventSystemStatus();
		void		StoneUpdateP2PPacket(DWORD PID);
		boost::unordered_map<DWORD, short>	m_Stone;
};