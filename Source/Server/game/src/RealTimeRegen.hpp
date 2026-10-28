#ifdef ENABLE_REAL_TIME_REGEN
#pragma once

#include "char.h"

#include <unordered_map>

typedef std::tuple<BYTE, DWORD, long, long, long, long, long, std::vector<long>> TRegen;
typedef std::tuple<long, long> TRegenTime;

static const std::string file_name = "real_time_regen.txt";
static const bool bRefreshRegen = true;
static const bool bRefreshAllMobs = false;
static const BYTE bRefreshAllMobsHour = 5;

enum ERegen : BYTE
{
	REGEN_TYPE,
	REGEN_VNUM,
	REGEN_MAP_INDEX,
	REGEN_X,
	REGEN_Y,
	REGEN_RANGE_X,
	REGEN_RANGE_Y,
	REGEN_TIMES,
};

enum ERegenType : BYTE
{
	REAL_TIME_REGEN_STANDARD,
	REAL_TIME_REGEN_PRECISE,
};

class CRealTimeRegen : public singleton<CRealTimeRegen>
{
	public:
		CRealTimeRegen();
		~CRealTimeRegen() {};
		
	public:
		void Initialize();
		void Destroy();
		void Process();

#ifdef ENABLE_RESP_SYSTEM	
		static const std::tm GetTime();
		int GetNextSpawnTime(const WORD& wNum);
		int SecondsToSpawn(const long hour, const long minute);
#endif
	private:
		bool LoadRegen();
		void SpawnMonster(const WORD & wNum, const DWORD & dwVnum, const long & lMapIndex, const long & lX, const long & lY, const long & lRangeX, const long & lRangeY);
		void SendRegenNotice(const std::string & szNotice);
		
	private:
		std::unordered_map<WORD, TRegen> mapRespawnTimes;
		std::unordered_map<WORD, TRegenTime> mapNextRespawn;
		
		bool m_bIsInitialized;
		DWORD m_dwLastTick;
		DWORD m_dwLastMobsRefresh;
};
#endif
