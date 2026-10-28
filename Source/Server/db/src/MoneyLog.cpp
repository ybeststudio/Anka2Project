#include "stdafx.h"
#include "MoneyLog.h"
#include "ClientManager.h"
#include "Peer.h"

#include "../../common/service.h"

CMoneyLog::CMoneyLog()
{
}

CMoneyLog::~CMoneyLog()
{
}

void CMoneyLog::Save()
{
	CPeer* peer = CClientManager::instance().GetAnyPeer();
	if (!peer)
		return;
	for (BYTE bType = 0; bType < MONEY_LOG_TYPE_MAX_NUM; bType ++)
	{
		__typeof(m_MoneyLogContainer[bType].begin()) it;
		for (it = m_MoneyLogContainer[bType].begin(); it != m_MoneyLogContainer[bType].end(); ++it)
		{
			TPacketMoneyLog p;
			p.type = bType;
			p.vnum = it->first;
			p.gold = it->second;
			peer->EncodeHeader(HEADER_DG_MONEY_LOG, 0, sizeof(p));
			peer->Encode(&p, sizeof(p));
		}
		m_MoneyLogContainer[bType].clear();
	}
}

#ifdef ENABLE_GOLD_LIMIT
void CMoneyLog::AddLog(BYTE bType, DWORD dwVnum, long long iGold)
#else
void CMoneyLog::AddLog(BYTE bType, DWORD dwVnum, int iGold)
#endif
{
	m_MoneyLogContainer[bType][dwVnum] += iGold;
}
