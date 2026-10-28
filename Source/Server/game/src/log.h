#ifndef __INC_LOG_MANAGER_H__
#define __INC_LOG_MANAGER_H__

#include "../../common/service.h"
#include "../../library/libsql/AsyncSQL.h"

enum GOLDBAR_HOW
{
	PERSONAL_SHOP_BUY = 1 ,
	PERSONAL_SHOP_SELL = 2 ,
	SHOP_BUY = 3 ,
	SHOP_SELL = 4 ,
	EXCHANGE_TAKE = 5 ,
	EXCHANGE_GIVE = 6 ,
	QUEST = 7 ,
};

class LogManager : public singleton<LogManager>
{
	public:
		LogManager();
		virtual ~LogManager();

		bool IsConnected();

		bool Connect(const char * host, const int port, const char * user, const char * pwd, const char * db);

		void ItemLog(DWORD dwPID, DWORD x, DWORD y, DWORD dwItemID, const char * c_pszText, const char * c_pszHint, const char * c_pszIP, DWORD dwVnum);
		void ItemLog(LPCHARACTER ch, LPITEM item, const char * c_pszText, const char * c_pszHint);
		void ItemLog(LPCHARACTER ch, int itemID, int itemVnum, const char * c_pszText, const char * c_pszHint);

		void CharLog(DWORD dwPID, DWORD x, DWORD y, DWORD dw, const char * c_pszText, const char * c_pszHint, const char * c_pszIP);
		void CharLog(LPCHARACTER ch, DWORD dw, const char * c_pszText, const char * c_pszHint);

#ifdef ENABLE_LEVEL_INT
		void LoginLog(bool isLogin, DWORD dwAccountID, DWORD dwPID, int bLevel, BYTE bJob, DWORD dwPlayTime);
#else
		void LoginLog(bool isLogin, DWORD dwAccountID, DWORD dwPID, BYTE bLevel, BYTE bJob, DWORD dwPlayTime);
#endif

#ifdef ENABLE_GOLD_LIMIT
		void MoneyLog(BYTE type, DWORD vnum, long long gold);
#else
		void MoneyLog(BYTE type, DWORD vnum, int gold);
#endif
		void HackLog(const char * c_pszHackName, const char * c_pszLogin, const char * c_pszName, const char * c_pszIP);
		void HackLog(const char * c_pszHackName, LPCHARACTER ch);
		void HackCRCLog(const char * c_pszHackName, const char * c_pszLogin, const char * c_pszName, const char * c_pszIP, DWORD dwCRC);
		void GoldBarLog(DWORD dwPID, DWORD dwItemID, GOLDBAR_HOW eHow, const char * c_pszHint);
		void CubeLog(DWORD dwPID, DWORD x, DWORD y, DWORD item_vnum, DWORD item_uid, int item_count, bool success);
		void GMCommandLog(DWORD dwPID, const char * szName, const char * szIP, BYTE byChannel, const char * szCommand);
		void SpeedHackLog(DWORD pid, DWORD x, DWORD y, int hack_count);
		void ChangeNameLog(DWORD pid, const char * old_name, const char * new_name, const char * ip);
		void RefineLog(DWORD pid, const char * item_name, DWORD item_id, int item_refine_level, int is_success, const char * how);
		void ShoutLog(BYTE bChannel, BYTE bEmpire, const char * pszText);
		void LevelLog(LPCHARACTER pChar, unsigned int level, unsigned int playhour);
		void BootLog(const char * c_pszHostName, BYTE bChannel);
		void FishLog(DWORD dwPID, int prob_idx, int fish_id, int fish_level, DWORD dwMiliseconds, DWORD dwVnum = false, DWORD dwValue = 0);
		void QuestRewardLog(const char * c_pszQuestName, DWORD dwPID, DWORD dwLevel, int iValue1, int iValue2);
		void DetailLoginLog(bool isLogin, LPCHARACTER ch);
		void DragonSlayLog(DWORD dwGuildID, DWORD dwDragonVnum, DWORD dwStartTime, DWORD dwEndTime);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		void AcceLog(DWORD dwPID, DWORD x, DWORD y, DWORD item_vnum, DWORD item_uid, int item_count, int abs_chance, bool success);
#endif
#ifdef ENABLE_ITEMSHOP
		void WheelOfFortuneLog(DWORD id, DWORD pid, long mapindex, long x, long y, long long price);
#endif
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
		void SoulRouletteLog(const char* table, const char* Name, const int vnum, const int count, const bool state);
#endif
#ifdef ENABLE_MINIGAME_RUMI_EVENT
		void OkayEventLog(int dwPID, const char * c_pszText, int points);
#endif

	private:
		void Query(const char * c_pszFormat, ...);

		CAsyncSQL m_sql;
		bool m_bIsConnect;
};

#endif
