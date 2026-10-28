#ifndef __INC_MONEY_LOG
#define __INC_MONEY_LOG

#include "../../common/service.h"

#include <map>

class CMoneyLog : public singleton<CMoneyLog>
{
	public:
		CMoneyLog();
		virtual ~CMoneyLog();

		void Save();
#ifdef ENABLE_GOLD_LIMIT
		void AddLog(BYTE bType, DWORD dwVnum, long long iGold);
#else
		void AddLog(BYTE bType, DWORD dwVnum, int iGold);
#endif

	private:
#ifdef ENABLE_GOLD_LIMIT
		std::map<DWORD, long long> m_MoneyLogContainer[MONEY_LOG_TYPE_MAX_NUM];
#else
		std::map<DWORD, int> m_MoneyLogContainer[MONEY_LOG_TYPE_MAX_NUM];
#endif
};

#endif
