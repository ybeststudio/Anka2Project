#pragma once

//EVERDAY = 0,//Hergün
//SUNDAY = 1,// Pazar
//MONDAY = 2,// Pazartesi
//TUESDAY = 3,//Salý
//WEDNESDAY = 4,//Çarþamba
//THURSDAY = 5,//Perþembe
//FRIDAY = 6,//Cuma
//SATURDAY = 7,//Cumartesi

enum ENewRegenFile
{
	R_ID,
	R_MAPINDEX,
	R_MAPNAME,
	R_X,
	R_Y,
	R_DIRECTION,
	R_CHANNEL,
	R_DAY,
	R_HOUR,
	R_MINUTE,
	R_SECOND,
	R_BROADCAST,
	R_SAFE_RANGE,
	R_DAYS_RANGE,
	R_HOURS_RANGE,
	R_MINUTE_RANGE,
	R_VNUM,
};

typedef struct SNewRegen
{
	WORD id;
	long mapIndex;
	long x, y;
	BYTE direction;
	BYTE channel;
	BYTE day, hour, minute, second;
	bool broadcast;
	long safeRange;
	DWORD mob_vnum;
	BYTE days_range;
	BYTE hours_range;
	BYTE minute_range;
	LPCHARACTER	bossPtr;
	bool blockAttack;
	bool p2pAlive;
	int leftTime;
	bool sent30SecNotice;  // 30 sn uyarisi sadece bir kez gonderilsin (2501 Mxxx spam onleme)
} TNewRegen;

class CNewMobTimer : public singleton<CNewMobTimer>
{
	protected:
		LPEVENT			m_pkMobRegenTimerEvent;
		std::vector<TNewRegen> m_vecRegenData;
		std::vector<DWORD> m_vecHasVnums;

	public:
		CNewMobTimer();
		~CNewMobTimer();

		void	Destroy();
		void	Initialize();
		long	Update();
 
		void	CalculateLeftTimeReal(TNewRegen* newRegen);
		int		CalculateLeftTime(TNewRegen* newRegen);
		void	GetTrackData(LPCHARACTER ch, WORD id);
		void	UpdateNewRegen(WORD id, bool isAlive, bool isP2P = false);
		void	UpdatePlayers();
		bool	IsWorldBoss(DWORD mobIndex);

		bool	HasMob(DWORD mobVnum);

		bool	LoadFile(const char* filename);
		bool	ReadLine(FILE* fp, TNewRegen& regen);

		bool	CheckDamage(LPCHARACTER pkAttacker, LPCHARACTER pkVictim);
		void	Dead(LPCHARACTER pkBoss, LPCHARACTER pkKiller);

		void	SpawnBoss(const SECTREE_MAP* sectreeMap, TNewRegen& m_Regen);

};
