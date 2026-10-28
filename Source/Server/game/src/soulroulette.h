#pragma once
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
class CSoulRoulette
{
public:
	CSoulRoulette(LPCHARACTER ch);
	~CSoulRoulette();
	void TurnWheel();
	void GiveGift();
	DWORD GetGiftVnum() const;
	void SendPacket(BYTE option, int arg1 = 0 , int arg2 = 0);
	
	enum Error { LOAD, ADD, GIVE, SHUTDOWN };
	enum Packet { OPEN, CLOSE, TURN };

	static bool ReadRouletteData(bool NoMoreItem = false);
	static void StateError(BYTE, LPCHARACTER);

	struct SRoulette
	{
		DWORD vnum;
		BYTE count;
		BYTE chance; // max 255
		SRoulette(DWORD m_vnum, BYTE m_count, BYTE m_chance)
			: vnum(m_vnum), count(m_count), chance(m_chance) {}

		struct ByChance
		{
			bool operator ()(const CSoulRoulette::SRoulette* a, const CSoulRoulette::SRoulette* b) const
			{
				return a->chance < b->chance;
			}
		};
	};

private:
	struct ErrorData
	{
		DWORD vnum;
		BYTE count;
		std::string name;
		ErrorData(DWORD m_vnum, BYTE m_count, const char* m_name)
			: vnum(m_vnum), count(m_count), name(m_name)
		{}
		struct FindName
		{
			std::string s_Name;
			FindName(const char* c_name) : s_Name(c_name) {}
			bool operator () (const ErrorData* p) { return !p->name.compare(s_Name); }
		};
	};
	
	void SetGift(const DWORD vnum, const BYTE count);
	int PickAGift();
	BYTE GetGiftCount() const;
	WORD GetTurnCount() const;
	BYTE GetChance() const;

	LPCHARACTER ch;
	DWORD gift_vnum;
	BYTE gift_count;
	WORD turn_count;
};
#endif
