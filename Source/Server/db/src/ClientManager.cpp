#include "stdafx.h"

#include "../../common/service.h"
#include "../../common/building.h"
#include "../../common/VnumHelper.h"
#include "../../library/libgame/include/grid.h"

#include "ClientManager.h"

#include "Main.h"
#include "Config.h"
#include "DBManager.h"
#include "QID.h"
#include "GuildManager.h"
#include "PrivManager.h"
#include "MoneyLog.h"
#include "ItemAwardManager.h"
#include "Marriage.h"
#include "ItemIDRangeManager.h"
#include "Cache.h"
#include <sstream>

#ifdef ENABLE_ITEMSHOP
#include "BufferManager.h"
#endif

extern int g_iPlayerCacheFlushSeconds;
extern int g_iItemCacheFlushSeconds;
extern int g_test_server;
extern int g_log;
extern std::string g_stLocale;
extern std::string g_stLocaleNameColumn;
bool CreateItemTableFromRes(MYSQL_RES * res, std::vector<TPlayerItem> * pVec, DWORD dwPID);

DWORD g_dwUsageMax = 0;
DWORD g_dwUsageAvg = 0;

CPacketInfo g_query_info;
CPacketInfo g_item_info;

int g_item_count = 0;
int g_query_count[2];

CClientManager::CClientManager() :
	m_pkAuthPeer(NULL),
	m_iPlayerIDStart(0),
	m_iPlayerDeleteLevelLimit(0),
	m_iPlayerDeleteLevelLimitLower(0),
	m_bChinaEventServer(false),
	m_iShopTableSize(0),
	m_pShopTable(NULL),
#ifdef ENABLE_RENEWAL_SHOPEX
	m_pShopEXTable(NULL),
	m_iShopEXTableSize(0),
#endif
	m_iRefineTableSize(0),
	m_pRefineTable(NULL),
	m_bShutdowned(FALSE),
	m_iCacheFlushCount(0),
	m_iCacheFlushCountLimit(200)
{
	m_itemRange.dwMin = 0;
	m_itemRange.dwMax = 0;
	m_itemRange.dwUsableItemIDMin = 0;

	memset(g_query_count, 0, sizeof(g_query_count));
}

CClientManager::~CClientManager()
{
	Destroy();
}

void CClientManager::SetPlayerIDStart(int iIDStart)
{
	m_iPlayerIDStart = iIDStart;
}

void CClientManager::Destroy()
{
	m_mChannelStatus.clear();

#ifdef ENABLE_ITEMSHOP
	m_IShopManager.clear();
	m_IShopLogManager.clear();
#endif

	for (itertype(m_peerList) i = m_peerList.begin(); i != m_peerList.end(); ++i)
		(*i)->Destroy();

	m_peerList.clear();

	if (m_fdAccept > 0)
	{
		socket_close(m_fdAccept);
		m_fdAccept = -1;
	}

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	for (auto it = m_Offlineshop.begin(); it != m_Offlineshop.end(); ++it)
	{
		if (it->second)
		{
			char szQuery[1024];
			snprintf(szQuery, sizeof(szQuery), "UPDATE player.offline_shop_npc SET time = %d WHERE owner_id = %u",it->second->time, it->second->owner_id);
			std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));
			delete it->second;
		}
	}
	m_Offlineshop.clear();
#endif

#ifdef ENABLE_OFFLINE_MESSAGE
	m_OfflineMessage.clear();
#endif
}

bool CClientManager::Initialize()
{
	int tmpValue;

	//BOOT_LOCALIZATION
	if (!InitializeLocalization())
	{
		fprintf(stderr, "Failed Localization Infomation so exit\n");
		return false;
	}

	if (!InitializeNowItemID())
	{
		fprintf(stderr, " Item range Initialize Failed. Exit DBCache Server\n");
		return false;
	}

	if (!InitializeTables())
	{
		sys_err("Table Initialize FAILED");
		return false;
	}

	CGuildManager::instance().BootReserveWar();

	if (!CConfig::instance().GetValue("BIND_PORT", &tmpValue))
		tmpValue = 5300;

	char szBindIP[128];

	if (!CConfig::instance().GetValue("BIND_IP", szBindIP, 128))
#ifdef ENABLE_LOCALHOST_IP_FIX
		strlcpy(szBindIP, "0", sizeof(szBindIP));
#else
		strlcpy(szBindIP, "127.0.0.1", sizeof(szBindIP));
#endif
	m_fdAccept = socket_tcp_bind(szBindIP, tmpValue);

	if (m_fdAccept < 0)
	{
		perror("socket");
		return false;
	}

	sys_log(0, "ACCEPT_HANDLE: %u", m_fdAccept);
	fdwatch_add_fd(m_fdWatcher, m_fdAccept, NULL, FDW_READ, false);

	if (!CConfig::instance().GetValue("BACKUP_LIMIT_SEC", &tmpValue))
		tmpValue = 600;

	m_looping = true;

	if (!CConfig::instance().GetValue("PLAYER_DELETE_LEVEL_LIMIT", &m_iPlayerDeleteLevelLimit))
	{
		sys_err("conf.txt: Cannot find PLAYER_DELETE_LEVEL_LIMIT, use default level %d", PLAYER_MAX_LEVEL_CONST + 1);
		m_iPlayerDeleteLevelLimit = PLAYER_MAX_LEVEL_CONST + 1;
	}

	if (!CConfig::instance().GetValue("PLAYER_DELETE_LEVEL_LIMIT_LOWER", &m_iPlayerDeleteLevelLimitLower))
	{
		m_iPlayerDeleteLevelLimitLower = 0;
	}

	sys_log(0, "PLAYER_DELETE_LEVEL_LIMIT set to %d", m_iPlayerDeleteLevelLimit);
	sys_log(0, "PLAYER_DELETE_LEVEL_LIMIT_LOWER set to %d", m_iPlayerDeleteLevelLimitLower);

	m_bChinaEventServer = false;

	int	iChinaEventServer = 0;

	if (CConfig::instance().GetValue("CHINA_EVENT_SERVER", &iChinaEventServer))
		m_bChinaEventServer = (iChinaEventServer);

	sys_log(0, "CHINA_EVENT_SERVER %s", CClientManager::instance().IsChinaEventServer()?"true":"false");


	LoadEventFlag();

	return true;
}

void CClientManager::MainLoop()
{
	SQLMsg * tmp;

	sys_log(0, "ClientManager pointer is %p", this);

	while (!m_bShutdowned)
	{
		while ((tmp = CDBManager::instance().PopResult()))
		{
			AnalyzeQueryResult(tmp);
			delete tmp;
		}

		if (!Process())
			break;

		log_rotate();
	}

	sys_log(0, "MainLoop exited, Starting cache flushing");

	signal_timer_disable();

	itertype(m_map_playerCache) it = m_map_playerCache.begin();

	while (it != m_map_playerCache.end())
	{
		CPlayerTableCache * c = (it++)->second;

		c->Flush();
		delete c;
	}
	m_map_playerCache.clear();

	itertype(m_map_itemCache) it2 = m_map_itemCache.begin();

	while (it2 != m_map_itemCache.end())
	{
		CItemCache * c = (it2++)->second;

		c->Flush();
		delete c;
	}
	m_map_itemCache.clear();

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	itertype(m_map_SkillColorCache) it3 = m_map_SkillColorCache.begin();
	while (it3 != m_map_SkillColorCache.end())
	{
		CSKillColorCache* pCache = it3->second;

		pCache->Flush();
		delete pCache;

		m_map_SkillColorCache.erase(it3++);
	}
	m_map_SkillColorCache.clear();
#endif

	for (itertype(m_mapItemPriceListCache) itPriceList = m_mapItemPriceListCache.begin(); itPriceList != m_mapItemPriceListCache.end(); ++itPriceList)
	{
		CItemPriceListTableCache* pCache = itPriceList->second;
		pCache->Flush();
		delete pCache;
	}

	m_mapItemPriceListCache.clear();

#ifdef ENABLE_GROWTH_PET_SYSTEM
	auto growthPetCache = m_map_growthPetCache.begin();

	while (growthPetCache != m_map_growthPetCache.end())
	{
		CGrowthPetCache* c = (growthPetCache++)->second;

		c->Flush();
		delete c;
	}
	m_map_growthPetCache.clear();
#endif
}

void CClientManager::Quit()
{
	m_bShutdowned = TRUE;
}

void CClientManager::QUERY_BOOT(CPeer* peer, TPacketGDBoot * p)
{
	const BYTE bPacketVersion = 6;

	std::vector<tAdminInfo> vAdmin;
	std::vector<std::string> vHost;

	__GetHostInfo(vHost);
	__GetAdminInfo(p->szIP, vAdmin);

	sys_log(0, "QUERY_BOOT : AdminInfo (Request ServerIp %s) ", p->szIP);

	DWORD dwPacketSize =
		sizeof(DWORD) +
		sizeof(BYTE) +
		sizeof(WORD) + sizeof(WORD) + sizeof(TMobTable) * m_vec_mobTable.size() +
		sizeof(WORD) + sizeof(WORD) + sizeof(TItemTable) * m_vec_itemTable.size() +
		sizeof(WORD) + sizeof(WORD) + sizeof(TShopTable) * m_iShopTableSize +
#ifdef ENABLE_RENEWAL_SHOPEX
		sizeof(WORD) + sizeof(WORD) + sizeof(TShopTable) * m_iShopEXTableSize +
#endif
		sizeof(WORD) + sizeof(WORD) + sizeof(TSkillTable) * m_vec_skillTable.size() +
		sizeof(WORD) + sizeof(WORD) + sizeof(TRefineTable) * m_iRefineTableSize +

		sizeof(WORD) + sizeof(WORD) + sizeof(TItemAttrTable) * m_vec_itemAttrTable.size() +
		sizeof(WORD) + sizeof(WORD) + sizeof(TItemAttrTable) * m_vec_itemRareTable.size() +
		sizeof(WORD) + sizeof(WORD) + sizeof(TBanwordTable) * m_vec_banwordTable.size() +
		sizeof(WORD) + sizeof(WORD) + sizeof(building::TLand) * m_vec_kLandTable.size() +
		sizeof(WORD) + sizeof(WORD) + sizeof(building::TObjectProto) * m_vec_kObjectProto.size() +
		sizeof(WORD) + sizeof(WORD) + sizeof(building::TObject) * m_map_pkObjectTable.size() +
#ifdef ENABLE_GROWTH_PET_SYSTEM
		sizeof(WORD) + sizeof(WORD) + sizeof(TGrowthPetSkillTable) * m_vec_growthPetSkillTable.size() +
#endif
		sizeof(time_t) +
		sizeof(WORD) + sizeof(WORD) + sizeof(TItemIDRangeTable)*2 +
		//ADMIN_MANAGER
		sizeof(WORD) + sizeof(WORD) + 16 * vHost.size() +
		sizeof(WORD) + sizeof(WORD) +  sizeof(tAdminInfo) *  vAdmin.size() +
		//END_ADMIN_MANAGER
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		sizeof(WORD) + sizeof(WORD) + sizeof(TOfflineShop) * m_Offlineshop.size() +
#endif
		sizeof(WORD);

	peer->EncodeHeader(HEADER_DG_BOOT, 0, dwPacketSize);
	peer->Encode(&dwPacketSize, sizeof(DWORD));
	peer->Encode(&bPacketVersion, sizeof(BYTE));

	sys_log(0, "BOOT: PACKET: %d", dwPacketSize);
	sys_log(0, "BOOT: VERSION: %d", bPacketVersion);

	sys_log(0, "sizeof(TMobTable) = %d", sizeof(TMobTable));
	sys_log(0, "sizeof(TItemTable) = %d", sizeof(TItemTable));
	sys_log(0, "sizeof(TShopTable) = %d", sizeof(TShopTable));
	sys_log(0, "sizeof(TSkillTable) = %d", sizeof(TSkillTable));
	sys_log(0, "sizeof(TRefineTable) = %d", sizeof(TRefineTable));
	sys_log(0, "sizeof(TItemAttrTable) = %d", sizeof(TItemAttrTable));
	sys_log(0, "sizeof(TItemRareTable) = %d", sizeof(TItemAttrTable));
	sys_log(0, "sizeof(TBanwordTable) = %d", sizeof(TBanwordTable));
	sys_log(0, "sizeof(TLand) = %d", sizeof(building::TLand));
	sys_log(0, "sizeof(TObjectProto) = %d", sizeof(building::TObjectProto));
	sys_log(0, "sizeof(TObject) = %d", sizeof(building::TObject));
#ifdef ENABLE_GROWTH_PET_SYSTEM
	sys_log(0, "sizeof(TGrowthPetSkillTable) = %d", sizeof(TGrowthPetSkillTable));
#endif
	//ADMIN_MANAGER
	sys_log(0, "sizeof(tAdminInfo) = %d * %d ", sizeof(tAdminInfo) * vAdmin.size());
	//END_ADMIN_MANAGER
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	sys_log(0, "sizeof(TOfflineShop) = %d ", sizeof(TOfflineShop));
#endif
	peer->EncodeWORD(sizeof(TMobTable));
	peer->EncodeWORD(m_vec_mobTable.size());
	peer->Encode(&m_vec_mobTable[0], sizeof(TMobTable) * m_vec_mobTable.size());

	peer->EncodeWORD(sizeof(TItemTable));
	peer->EncodeWORD(m_vec_itemTable.size());
	peer->Encode(&m_vec_itemTable[0], sizeof(TItemTable) * m_vec_itemTable.size());

	peer->EncodeWORD(sizeof(TShopTable));
	peer->EncodeWORD(m_iShopTableSize);
	peer->Encode(m_pShopTable, sizeof(TShopTable) * m_iShopTableSize);

#ifdef ENABLE_RENEWAL_SHOPEX
	peer->EncodeWORD(sizeof(TShopTable));
	peer->EncodeWORD(m_iShopEXTableSize);
	peer->Encode(m_pShopEXTable, sizeof(TShopTable) * m_iShopEXTableSize);
#endif

	peer->EncodeWORD(sizeof(TSkillTable));
	peer->EncodeWORD(m_vec_skillTable.size());
	peer->Encode(&m_vec_skillTable[0], sizeof(TSkillTable) * m_vec_skillTable.size());

	peer->EncodeWORD(sizeof(TRefineTable));
	peer->EncodeWORD(m_iRefineTableSize);
	peer->Encode(m_pRefineTable, sizeof(TRefineTable) * m_iRefineTableSize);

	peer->EncodeWORD(sizeof(TItemAttrTable));
	peer->EncodeWORD(m_vec_itemAttrTable.size());
	peer->Encode(&m_vec_itemAttrTable[0], sizeof(TItemAttrTable) * m_vec_itemAttrTable.size());

	peer->EncodeWORD(sizeof(TItemAttrTable));
	peer->EncodeWORD(m_vec_itemRareTable.size());
	peer->Encode(&m_vec_itemRareTable[0], sizeof(TItemAttrTable) * m_vec_itemRareTable.size());

	peer->EncodeWORD(sizeof(TBanwordTable));
	peer->EncodeWORD(m_vec_banwordTable.size());
	peer->Encode(&m_vec_banwordTable[0], sizeof(TBanwordTable) * m_vec_banwordTable.size());

	peer->EncodeWORD(sizeof(building::TLand));
	peer->EncodeWORD(m_vec_kLandTable.size());
	peer->Encode(&m_vec_kLandTable[0], sizeof(building::TLand) * m_vec_kLandTable.size());

	peer->EncodeWORD(sizeof(building::TObjectProto));
	peer->EncodeWORD(m_vec_kObjectProto.size());
	peer->Encode(&m_vec_kObjectProto[0], sizeof(building::TObjectProto) * m_vec_kObjectProto.size());

	peer->EncodeWORD(sizeof(building::TObject));
	peer->EncodeWORD(m_map_pkObjectTable.size());

	itertype(m_map_pkObjectTable) it = m_map_pkObjectTable.begin();

	while (it != m_map_pkObjectTable.end())
		peer->Encode((it++)->second, sizeof(building::TObject));

#ifdef ENABLE_GROWTH_PET_SYSTEM
	peer->EncodeWORD(sizeof(TGrowthPetSkillTable));
	peer->EncodeWORD(m_vec_growthPetSkillTable.size());
	if (m_vec_growthPetSkillTable.size())
		peer->Encode(&m_vec_growthPetSkillTable[0], sizeof(TGrowthPetSkillTable) * m_vec_growthPetSkillTable.size());
#endif

	time_t now = time(0);
	peer->Encode(&now, sizeof(time_t));

	TItemIDRangeTable itemRange = CItemIDRangeManager::instance().GetRange();
	TItemIDRangeTable itemRangeSpare = CItemIDRangeManager::instance().GetRange();

	peer->EncodeWORD(sizeof(TItemIDRangeTable));
	peer->EncodeWORD(1);
	peer->Encode(&itemRange, sizeof(TItemIDRangeTable));
	peer->Encode(&itemRangeSpare, sizeof(TItemIDRangeTable));

	peer->SetItemIDRange(itemRange);
	peer->SetSpareItemIDRange(itemRangeSpare);

	//ADMIN_MANAGER
	peer->EncodeWORD(16);
	peer->EncodeWORD(vHost.size());

	for (size_t n = 0; n < vHost.size(); ++n)
	{
		peer->Encode(vHost[n].c_str(), 16);
		sys_log(0, "GMHosts %s", vHost[n].c_str());
	}

	peer->EncodeWORD(sizeof(tAdminInfo));
	peer->EncodeWORD(vAdmin.size());

	for (size_t n = 0; n < vAdmin.size(); ++n)
	{
		peer->Encode(&vAdmin[n], sizeof(tAdminInfo));
		sys_log(0, "Admin name %s ConntactIP %s", vAdmin[n].m_szName, vAdmin[n].m_szContactIP);
	}
	//END_ADMIN_MANAGER

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	peer->EncodeWORD(sizeof(TOfflineShop));
	std::vector<TOfflineShop> offlineshop;
	offlineshop.clear();

	for (auto it = m_Offlineshop.begin(); it != m_Offlineshop.end(); it++)
	{
		TOfflineShop p;
		memcpy(&p, it->second, sizeof(TOfflineShop));
		offlineshop.push_back(p);
	}

	peer->EncodeWORD(offlineshop.size());
	if(offlineshop.size())
		peer->Encode(&offlineshop[0], sizeof(TOfflineShop) * offlineshop.size());
#endif

	peer->EncodeWORD(0xffff);

#ifdef ENABLE_ITEMSHOP
	SendItemShopData(peer);
#endif
}

void CClientManager::SendPartyOnSetup(CPeer* pkPeer)
{
	TPartyMap & pm = m_map_pkChannelParty[pkPeer->GetChannel()];

	for (itertype(pm) it_party = pm.begin(); it_party != pm.end(); ++it_party)
	{
		sys_log(0, "PARTY SendPartyOnSetup Party [%u]", it_party->first);
		pkPeer->EncodeHeader(HEADER_DG_PARTY_CREATE, 0, sizeof(TPacketPartyCreate));
		pkPeer->Encode(&it_party->first, sizeof(DWORD));

		for (itertype(it_party->second) it_member = it_party->second.begin(); it_member != it_party->second.end(); ++it_member)
		{
			sys_log(0, "PARTY SendPartyOnSetup Party [%u] Member [%u]", it_party->first, it_member->first);
			pkPeer->EncodeHeader(HEADER_DG_PARTY_ADD, 0, sizeof(TPacketPartyAdd));
			pkPeer->Encode(&it_party->first, sizeof(DWORD));
			pkPeer->Encode(&it_member->first, sizeof(DWORD));
			pkPeer->Encode(&it_member->second.bRole, sizeof(BYTE));

			pkPeer->EncodeHeader(HEADER_DG_PARTY_SET_MEMBER_LEVEL, 0, sizeof(TPacketPartySetMemberLevel));
			pkPeer->Encode(&it_party->first, sizeof(DWORD));
			pkPeer->Encode(&it_member->first, sizeof(DWORD));
#ifdef ENABLE_LEVEL_INT
			pkPeer->Encode(&it_member->second.bLevel, sizeof(int));
#else
			pkPeer->Encode(&it_member->second.bLevel, sizeof(BYTE));
#endif
		}
	}
}

void CClientManager::QUERY_PLAYER_COUNT(CPeer * pkPeer, TPlayerCountPacket * pPacket)
{
	pkPeer->SetUserCount(pPacket->dwCount);
}

void CClientManager::QUERY_QUEST_SAVE(CPeer * pkPeer, TQuestTable * pTable, DWORD dwLen)
{
	if (0 != (dwLen % sizeof(TQuestTable)))
	{
		sys_err("invalid packet size %d, sizeof(TQuestTable) == %d", dwLen, sizeof(TQuestTable));
		return;
	}

	int iSize = dwLen / sizeof(TQuestTable);

	char szQuery[1024];

	for (int i = 0; i < iSize; ++i, ++pTable)
	{
		if (pTable->lValue == 0)
		{
			snprintf(szQuery, sizeof(szQuery),
					"DELETE FROM quest%s WHERE dwPID=%d AND szName='%s' AND szState='%s'",
					GetTablePostfix(), pTable->dwPID, pTable->szName, pTable->szState);
		}
		else
		{
			snprintf(szQuery, sizeof(szQuery),
					"REPLACE INTO quest%s (dwPID, szName, szState, lValue) VALUES(%d, '%s', '%s', %d)",
					GetTablePostfix(), pTable->dwPID, pTable->szName, pTable->szState, pTable->lValue);
		}

		CDBManager::instance().ReturnQuery(szQuery, QID_QUEST_SAVE, pkPeer->GetHandle(), NULL);
	}
}

void CClientManager::QUERY_SAFEBOX_LOAD(CPeer * pkPeer, DWORD dwHandle, TSafeboxLoadPacket * packet, bool bMall)
{
	ClientHandleInfo * pi = new ClientHandleInfo(dwHandle);
	strlcpy(pi->safebox_password, packet->szPassword, sizeof(pi->safebox_password));
	pi->account_id = packet->dwID;
	pi->account_index = 0;
	pi->ip[0] = bMall ? 1 : 0;
	strlcpy(pi->login, packet->szLogin, sizeof(pi->login));

	char szQuery[QUERY_MAX_LEN];
	snprintf(szQuery, sizeof(szQuery),
			"SELECT account_id, size, password FROM safebox%s WHERE account_id=%u",
			GetTablePostfix(), packet->dwID);

	if (g_log)
		sys_log(0, "HEADER_GD_SAFEBOX_LOAD (handle: %d account.id %u is_mall %d)", dwHandle, packet->dwID, bMall ? 1 : 0);

	CDBManager::instance().ReturnQuery(szQuery, QID_SAFEBOX_LOAD, pkPeer->GetHandle(), pi);
}

void CClientManager::RESULT_SAFEBOX_LOAD(CPeer * pkPeer, SQLMsg * msg)
{
	CQueryInfo * qi = (CQueryInfo *) msg->pvUserData;
	ClientHandleInfo * pi = (ClientHandleInfo *) qi->pvData;
	DWORD dwHandle = pi->dwHandle;

	if (pi->account_index == 0)
	{
		char szSafeboxPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
		strlcpy(szSafeboxPassword, pi->safebox_password, sizeof(szSafeboxPassword));

		TSafeboxTable * pSafebox = new TSafeboxTable;
		memset(pSafebox, 0, sizeof(TSafeboxTable));

		SQLResult * res = msg->Get();

		if (res->uiNumRows == 0)
		{
#ifndef ENABLE_SPECIAL_INVENTORY
			if (strcmp("000000", szSafeboxPassword))
			{
				pkPeer->EncodeHeader(HEADER_DG_SAFEBOX_WRONG_PASSWORD, dwHandle, 0);
				delete pSafebox;
				delete pi;
				return;
			}
#endif
		}
		else
		{
			MYSQL_ROW row = mysql_fetch_row(res->pSQLResult);

#ifndef ENABLE_SPECIAL_INVENTORY
			if (((!row[2] || !*row[2]) && strcmp("000000", szSafeboxPassword)) || ((row[2] && *row[2]) && strcmp(row[2], szSafeboxPassword)))
			{
				pkPeer->EncodeHeader(HEADER_DG_SAFEBOX_WRONG_PASSWORD, dwHandle, 0);
				delete pSafebox;
				delete pi;
				return;
			}
#endif

			if (!row[0])
				pSafebox->dwID = 0;
			else
				str_to_number(pSafebox->dwID, row[0]);

			if (!row[1])
				pSafebox->bSize = 0;
			else
				str_to_number(pSafebox->bSize, row[1]);

			if (pi->ip[0] == 1)
			{
				pSafebox->bSize = 1;
				sys_log(0, "MALL id[%d] size[%d]", pSafebox->dwID, pSafebox->bSize);
			}
			else
				sys_log(0, "SAFEBOX id[%d] size[%d]", pSafebox->dwID, pSafebox->bSize);
		}

		if (0 == pSafebox->dwID)
			pSafebox->dwID = pi->account_id;

		pi->pSafebox = pSafebox;

		char szQuery[512];
		snprintf(szQuery, sizeof(szQuery),
				"SELECT id, window+0, pos, count, vnum, "
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
				"transmutation, "
#endif
				"socket0, socket1, socket2, "
				"attrtype0, attrvalue0, "
				"attrtype1, attrvalue1, "
				"attrtype2, attrvalue2, "
				"attrtype3, attrvalue3, "
				"attrtype4, attrvalue4, "
				"attrtype5, attrvalue5, "
				"attrtype6, attrvalue6 "
				"FROM item%s WHERE owner_id=%d AND window='%s'",
				GetTablePostfix(), pi->account_id, pi->ip[0] == 0 ? "SAFEBOX" : "MALL");

		pi->account_index = 1;

		CDBManager::instance().ReturnQuery(szQuery, QID_SAFEBOX_LOAD, pkPeer->GetHandle(), pi);
	}
	else
	{
		if (!pi->pSafebox)
		{
			sys_err("null safebox pointer!");
			delete pi;
			return;
		}

		if (!msg->Get()->pSQLResult)
		{
			sys_err("null safebox result");
			delete pi;
			return;
		}

		static std::vector<TPlayerItem> s_items;
		CreateItemTableFromRes(msg->Get()->pSQLResult, &s_items, pi->account_id);

		std::set<TItemAward *> * pSet = ItemAwardManager::instance().GetByLogin(pi->login);

		if (pSet && !m_vec_itemTable.empty())
		{

			CGrid grid(5, MAX(1, pi->pSafebox->bSize) * 9);
			bool bEscape = false;

			for (DWORD i = 0; i < s_items.size(); ++i)
			{
				TPlayerItem & r = s_items[i];

				itertype(m_map_itemTableByVnum) it = m_map_itemTableByVnum.find(r.vnum);

				if (it == m_map_itemTableByVnum.end())
				{
					bEscape = true;
					sys_err("invalid item vnum %u in safebox: login %s", r.vnum, pi->login);
					break;
				}

				grid.Put(r.pos, 1, it->second->bSize);
			}

			if (!bEscape)
			{
				std::vector<std::pair<DWORD, DWORD> > vec_dwFinishedAwardID;

				__typeof(pSet->begin()) it = pSet->begin();

				char szQuery[512];

				while (it != pSet->end())
				{
					TItemAward * pItemAward = *(it++);
					const DWORD& dwItemVnum = pItemAward->dwVnum;

					if (pItemAward->bTaken)
						continue;

					if (pi->ip[0] == 0 && pItemAward->bMall)
						continue;

					if (pi->ip[0] == 1 && !pItemAward->bMall)
						continue;

					itertype(m_map_itemTableByVnum) it = m_map_itemTableByVnum.find(pItemAward->dwVnum);

					if (it == m_map_itemTableByVnum.end())
					{
						sys_err("invalid item vnum %u in item_award: login %s", pItemAward->dwVnum, pi->login);
						continue;
					}

					TItemTable * pItemTable = it->second;

					int iPos;

					if ((iPos = grid.FindBlank(1, it->second->bSize)) == -1)
						break;

					TPlayerItem item;
					memset(&item, 0, sizeof(TPlayerItem));

					DWORD dwSocket2 = pItemAward->dwSocket2; //Fix

					if (pItemTable->bType == ITEM_UNIQUE)
					{
#ifdef ENABLE_EXTEND_ITEM_AWARD
						const long lValue0 = pItemTable->alValues[ITEM_SOCKET_REMAIN_SEC];
						const long lValue2 = pItemTable->alValues[ITEM_SOCKET_UNIQUE_REMAIN_TIME];
						const time_t tNow = CClientManager::instance().GetCurrentTime();
						dwSocket2 = (lValue2 == 0) ? static_cast<DWORD>(lValue0) : static_cast<DWORD>(tNow + lValue0);
#else
						if (pItemAward->dwSocket2 != 0)
							dwSocket2 = pItemAward->dwSocket2;
						else
							dwSocket2 = pItemTable->alValues[0];
#endif
					}
					else if ((dwItemVnum == 50300 || dwItemVnum == 70037) && pItemAward->dwSocket0 == 0)
					{
#ifndef ENABLE_EXTEND_ITEM_AWARD
						DWORD dwSkillIdx;
						DWORD dwSkillVnum;

						do
						{
							dwSkillIdx = number(0, m_vec_skillTable.size()-1);
							dwSkillVnum = m_vec_skillTable[dwSkillIdx].dwVnum;

							if (!dwSkillVnum > 120)
								continue;

							break;
						} while (1);

						pItemAward->dwSocket0 = dwSkillVnum;
#endif
					}
					else
					{
						switch (dwItemVnum)
						{
							case 72723: case 72724: case 72725: case 72726:
							case 72727: case 72728: case 72729: case 72730:
							case 76004: case 76005: case 76021: case 76022:
							case 79012: case 79013:
								if (pItemAward->dwSocket2 == 0)
								{
									dwSocket2 = pItemTable->alValues[0];
								}
								else
								{
									dwSocket2 = pItemAward->dwSocket2;
								}
								break;
						}
					}

					if (GetItemID () > m_itemRange.dwMax)
					{
						sys_err("UNIQUE ID OVERFLOW!!");
						break;
					}

					{
						itertype(m_map_itemTableByVnum) it = m_map_itemTableByVnum.find (dwItemVnum);
						if (it == m_map_itemTableByVnum.end())
						{
							sys_err ("Invalid item(vnum : %d). It is not in m_map_itemTableByVnum.", dwItemVnum);
							continue;
						}
						TItemTable* item_table = it->second;
						if (item_table == NULL)
						{
							sys_err ("Invalid item_table (vnum : %d). It's value is NULL in m_map_itemTableByVnum.", dwItemVnum);
							continue;
						}
						if (0 == pItemAward->dwSocket0)
						{
							for (int i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
							{
								if (LIMIT_REAL_TIME == item_table->aLimits[i].bType)
								{
									if (0 == item_table->aLimits[i].lValue)
										pItemAward->dwSocket0 = time(0) + 60 * 60 * 24 * 7;
									else
										pItemAward->dwSocket0 = time(0) + item_table->aLimits[i].lValue;

									break;
								}
								else if (LIMIT_REAL_TIME_START_FIRST_USE == item_table->aLimits[i].bType || LIMIT_TIMER_BASED_ON_WEAR == item_table->aLimits[i].bType)
								{
									if (0 == item_table->aLimits[i].lValue)
										pItemAward->dwSocket0 = 60 * 60 * 24 * 7;
									else
										pItemAward->dwSocket0 = item_table->aLimits[i].lValue;

									break;
								}
							}
						}

#ifdef ENABLE_EXTEND_ITEM_AWARD
						ItemAwardManager::instance().CheckItemCount(*pItemAward, *pItemTable);
						ItemAwardManager::instance().CheckItemSocket(*pItemAward, *pItemTable);
						ItemAwardManager::instance().CheckItemBlend(*pItemAward, *pItemTable);
						ItemAwardManager::instance().CheckItemAddonType(*pItemAward, *pItemTable);
						ItemAwardManager::instance().CheckItemSkillBook(*pItemAward, m_vec_skillTable);
#ifdef USE_ITEM_AWARD_CHECK_ATTRIBUTES
						ItemAwardManager::instance().CheckItemAttributes(*pItemAward, *pItemTable, m_vec_itemAttrTable);
#endif

						char szColumns[QUERY_MAX_LEN], szValues[QUERY_MAX_LEN];

						int	iLen = snprintf(szColumns, sizeof(szColumns), "id, owner_id, `window`, pos, vnum, count");
						int	iValueLen = snprintf(szValues, sizeof(szValues), "%u, %u, '%s', %d, %u, %u", GainItemID(), pi->account_id, (pi->ip[0] == 0) ? "SAFEBOX" : "MALL", iPos, pItemAward->dwVnum, pItemAward->dwCount);

						iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", socket0, socket1, socket2");
						iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen, ", %u, %u, %u", pItemAward->dwSocket0, pItemAward->dwSocket1, dwSocket2);

						for (size_t i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
						{
							iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", attrtype%d, attrvalue%d", i, i);
							iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen, ", %d, %d", pItemAward->aAttr[i].bType, pItemAward->aAttr[i].sValue);
						}

						snprintf(szQuery, sizeof(szQuery), "INSERT INTO item%s (%s) VALUES(%s)", GetTablePostfix(), szColumns, szValues);
#else
						snprintf(szQuery, sizeof(szQuery), 
								"INSERT INTO item%s (id, owner_id, window, pos, vnum, count, socket0, socket1, socket2) "
								"VALUES(%u, %u, '%s', %d, %u, %u, %u, %u, %u)",
								GetTablePostfix(),
								GainItemID(),
								pi->account_id,
								pi->ip[0] == 0 ? "SAFEBOX" : "MALL",
								iPos,
								pItemAward->dwVnum, pItemAward->dwCount, pItemAward->dwSocket0, pItemAward->dwSocket1, dwSocket2);
#endif
					}

					std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));
					SQLResult * pRes = pmsg->Get();
					sys_log(0, "SAFEBOX Query : [%s]", szQuery);

					if (pRes->uiAffectedRows == 0 || pRes->uiInsertID == 0 || pRes->uiAffectedRows == (uint32_t)-1)
						break;

					item.id = pmsg->Get()->uiInsertID;
					item.window = pi->ip[0] == 0 ? SAFEBOX : MALL,
					item.pos = iPos;
					item.count = pItemAward->dwCount;
					item.vnum = pItemAward->dwVnum;
					item.alSockets[0] = pItemAward->dwSocket0;
					item.alSockets[1] = pItemAward->dwSocket1;
					item.alSockets[2] = dwSocket2;
#ifdef ENABLE_EXTEND_ITEM_AWARD
					thecore_memcpy(&item.aAttr, pItemAward->aAttr, sizeof(item.aAttr));
#endif
					s_items.push_back(item);

					vec_dwFinishedAwardID.push_back(std::make_pair(pItemAward->dwID, item.id));
					grid.Put(iPos, 1, it->second->bSize);
				}

				for (DWORD i = 0; i < vec_dwFinishedAwardID.size(); ++i)
					ItemAwardManager::instance().Taken(vec_dwFinishedAwardID[i].first, vec_dwFinishedAwardID[i].second);
			}
		}

		pi->pSafebox->wItemCount = s_items.size();

#ifdef ENABLE_GROWTH_PET_SYSTEM
		static std::vector<TGrowthPet> s_petVec;

		if (pi->ip[0] == 0 && !s_items.empty())
		{
			char szQuery[QUERY_MAX_LEN];
			snprintf(szQuery, sizeof(szQuery),
				"SELECT id,vnum,state+0,name,size,level,level_step,evolution,type,hp,sp,def,hp_apply,sp_apply,def_apply,"
				"age_apply,skill_level,exp,item_exp,UNIX_TIMESTAMP(birthday),end_time,max_time "
				"FROM growth_pet%s WHERE owner_id=%d AND state=%d",
				GetTablePostfix(), pi->account_id, STATE_SAFEBOX);

			std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(szQuery));
			MYSQL_RES* pRes = pkMsg->Get()->pSQLResult;
			CreateGrowthPetTableFromRes(pRes, &s_petVec, pi->account_id);
		}
#endif

		pkPeer->EncodeHeader(pi->ip[0] == 0 ? HEADER_DG_SAFEBOX_LOAD : HEADER_DG_MALL_LOAD, dwHandle, sizeof(TSafeboxTable) + sizeof(TPlayerItem) * s_items.size()
#ifdef ENABLE_GROWTH_PET_SYSTEM
			+ sizeof(WORD) + sizeof(TGrowthPet) * s_petVec.size()
#endif
		);

		pkPeer->Encode(pi->pSafebox, sizeof(TSafeboxTable));

		if (!s_items.empty())
			pkPeer->Encode(&s_items[0], sizeof(TPlayerItem) * s_items.size());

#ifdef ENABLE_GROWTH_PET_SYSTEM
		pkPeer->EncodeWORD((WORD)s_petVec.size());

		if (!s_petVec.empty())
			pkPeer->Encode(&s_petVec[0], sizeof(TGrowthPet) * s_petVec.size());
#endif

		delete pi;
	}
}

void CClientManager::QUERY_SAFEBOX_CHANGE_SIZE(CPeer * pkPeer, DWORD dwHandle, TSafeboxChangeSizePacket * p)
{
	ClientHandleInfo * pi = new ClientHandleInfo(dwHandle);
	pi->account_index = p->bSize;

	char szQuery[QUERY_MAX_LEN];

	if (p->bSize == 1)
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO safebox%s (account_id, size) VALUES(%u, %u)", GetTablePostfix(), p->dwID, p->bSize);
	else
		snprintf(szQuery, sizeof(szQuery), "UPDATE safebox%s SET size=%u WHERE account_id=%u", GetTablePostfix(), p->bSize, p->dwID);

	CDBManager::instance().ReturnQuery(szQuery, QID_SAFEBOX_CHANGE_SIZE, pkPeer->GetHandle(), pi);
}

void CClientManager::RESULT_SAFEBOX_CHANGE_SIZE(CPeer * pkPeer, SQLMsg * msg)
{
	CQueryInfo * qi = (CQueryInfo *) msg->pvUserData;
	ClientHandleInfo * p = (ClientHandleInfo *) qi->pvData;
	DWORD dwHandle = p->dwHandle;
	BYTE bSize = p->account_index;

	delete p;

	if (msg->Get()->uiNumRows > 0)
	{
		pkPeer->EncodeHeader(HEADER_DG_SAFEBOX_CHANGE_SIZE, dwHandle, sizeof(BYTE));
		pkPeer->EncodeBYTE(bSize);
	}
}

void CClientManager::QUERY_SAFEBOX_CHANGE_PASSWORD(CPeer * pkPeer, DWORD dwHandle, TSafeboxChangePasswordPacket * p)
{
	ClientHandleInfo * pi = new ClientHandleInfo(dwHandle);
	strlcpy(pi->safebox_password, p->szNewPassword, sizeof(pi->safebox_password));
	strlcpy(pi->login, p->szOldPassword, sizeof(pi->login));
	pi->account_id = p->dwID;

	char szQuery[QUERY_MAX_LEN];
	snprintf(szQuery, sizeof(szQuery), "SELECT password FROM safebox%s WHERE account_id=%u", GetTablePostfix(), p->dwID);

	CDBManager::instance().ReturnQuery(szQuery, QID_SAFEBOX_CHANGE_PASSWORD, pkPeer->GetHandle(), pi);
}

void CClientManager::RESULT_SAFEBOX_CHANGE_PASSWORD(CPeer * pkPeer, SQLMsg * msg)
{
	CQueryInfo * qi = (CQueryInfo *) msg->pvUserData;
	ClientHandleInfo * p = (ClientHandleInfo *) qi->pvData;
	DWORD dwHandle = p->dwHandle;

	if (msg->Get()->uiNumRows > 0)
	{
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		if ((row[0] && *row[0] && !strcasecmp(row[0], p->login)) || ((!row[0] || !*row[0]) && !strcmp("000000", p->login)))
		{
			char szQuery[QUERY_MAX_LEN];
			char escape_pwd[64];
			CDBManager::instance().EscapeString(escape_pwd, p->safebox_password, strlen(p->safebox_password));

			snprintf(szQuery, sizeof(szQuery), "UPDATE safebox%s SET password='%s' WHERE account_id=%u", GetTablePostfix(), escape_pwd, p->account_id);

			CDBManager::instance().ReturnQuery(szQuery, QID_SAFEBOX_CHANGE_PASSWORD_SECOND, pkPeer->GetHandle(), p);
			return;
		}
	}

	delete p;

	// Wrong old password
	pkPeer->EncodeHeader(HEADER_DG_SAFEBOX_CHANGE_PASSWORD_ANSWER, dwHandle, sizeof(BYTE));
	pkPeer->EncodeBYTE(0);
}

void CClientManager::RESULT_SAFEBOX_CHANGE_PASSWORD_SECOND(CPeer * pkPeer, SQLMsg * msg)
{
	CQueryInfo * qi = (CQueryInfo *) msg->pvUserData;
	ClientHandleInfo * p = (ClientHandleInfo *) qi->pvData;
	DWORD dwHandle = p->dwHandle;
	delete p;

	pkPeer->EncodeHeader(HEADER_DG_SAFEBOX_CHANGE_PASSWORD_ANSWER, dwHandle, sizeof(BYTE));
	pkPeer->EncodeBYTE(1);
}

void CClientManager::RESULT_PRICELIST_LOAD(CPeer* peer, SQLMsg* pMsg)
{
	TItemPricelistReqInfo* pReqInfo = (TItemPricelistReqInfo*)static_cast<CQueryInfo*>(pMsg->pvUserData)->pvData;

	TItemPriceListTable table;
	table.dwOwnerID = pReqInfo->second;
	table.byCount = 0;
	
	MYSQL_ROW row;

	while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		str_to_number(table.aPriceInfo[table.byCount].dwVnum, row[0]);
		str_to_number(table.aPriceInfo[table.byCount].dwPrice, row[1]);
		table.byCount++;
	}

	PutItemPriceListCache(&table);

	TPacketMyshopPricelistHeader header;

	header.dwOwnerID = pReqInfo->second;
	header.byCount = table.byCount;

	size_t sizePriceListSize = sizeof(TItemPriceInfo) * header.byCount;

	peer->EncodeHeader(HEADER_DG_MYSHOP_PRICELIST_RES, pReqInfo->first, sizeof(header) + sizePriceListSize);
	peer->Encode(&header, sizeof(header));
	peer->Encode(table.aPriceInfo, sizePriceListSize);

	sys_log(0, "Load MyShopPricelist handle[%d] pid[%d] count[%d]", pReqInfo->first, pReqInfo->second, header.byCount);

	delete pReqInfo;
}

void CClientManager::RESULT_PRICELIST_LOAD_FOR_UPDATE(SQLMsg* pMsg)
{
	TItemPriceListTable* pUpdateTable = (TItemPriceListTable*)static_cast<CQueryInfo*>(pMsg->pvUserData)->pvData;

	TItemPriceListTable table;
	table.dwOwnerID = pUpdateTable->dwOwnerID;
	table.byCount = 0;

	MYSQL_ROW row;

	while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		str_to_number(table.aPriceInfo[table.byCount].dwVnum, row[0]);
		str_to_number(table.aPriceInfo[table.byCount].dwPrice, row[1]);
		table.byCount++;
	}

	PutItemPriceListCache(&table);

	GetItemPriceListCache(pUpdateTable->dwOwnerID)->UpdateList(pUpdateTable);

	delete pUpdateTable;
}

void CClientManager::QUERY_SAFEBOX_SAVE(CPeer * pkPeer, TSafeboxTable * pTable)
{
	char szQuery[QUERY_MAX_LEN];

	snprintf(szQuery, sizeof(szQuery),
			"UPDATE safebox%s SET gold='%u' WHERE account_id=%u",
			GetTablePostfix(), pTable->dwGold, pTable->dwID);

	CDBManager::instance().ReturnQuery(szQuery, QID_SAFEBOX_SAVE, pkPeer->GetHandle(), NULL);
}

void CClientManager::QUERY_EMPIRE_SELECT(CPeer * pkPeer, DWORD dwHandle, TEmpireSelectPacket * p)
{
	char szQuery[QUERY_MAX_LEN];

	snprintf(szQuery, sizeof(szQuery), "UPDATE player_index%s SET empire=%u WHERE id=%u", GetTablePostfix(), p->bEmpire, p->dwAccountID);
	delete CDBManager::instance().DirectQuery(szQuery);

	sys_log(0, "EmpireSelect: %s", szQuery);
	{
		snprintf(szQuery, sizeof(szQuery),
				"SELECT pid1, pid2, pid3, pid4, pid5 FROM player_index%s WHERE id=%u", GetTablePostfix(), p->dwAccountID);

		std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

		SQLResult * pRes = pmsg->Get();

		if (pRes->uiNumRows)
		{
			sys_log(0, "EMPIRE %lu", pRes->uiNumRows);

			MYSQL_ROW row = mysql_fetch_row(pRes->pSQLResult);
			DWORD pids[3];

			UINT g_start_map[4] =
			{
				0,
				1,
				21,
				41
			};

			// FIXME share with game
			DWORD g_start_position[4][2]=
			{
				{      0,      0 },
				{ 469300, 964200 },
				{  55700, 157900 },
				{ 969600, 278400 }
			};

			for (int i = 0; i < 3; ++i)
			{
				str_to_number(pids[i], row[i]);
				sys_log(0, "EMPIRE PIDS[%d]", pids[i]);

				if (pids[i])
				{
					sys_log(0, "EMPIRE move to pid[%d] to villiage of %u, map_index %d",
							pids[i], p->bEmpire, g_start_map[p->bEmpire]);

					snprintf(szQuery, sizeof(szQuery), "UPDATE player%s SET map_index=%u,x=%u,y=%u WHERE id=%u",
							GetTablePostfix(),
							g_start_map[p->bEmpire],
							g_start_position[p->bEmpire][0],
							g_start_position[p->bEmpire][1],
							pids[i]);

					std::unique_ptr<SQLMsg> pmsg2(CDBManager::instance().DirectQuery(szQuery));
				}
			}
		}
	}

	pkPeer->EncodeHeader(HEADER_DG_EMPIRE_SELECT, dwHandle, sizeof(BYTE));
	pkPeer->EncodeBYTE(p->bEmpire);
}

void CClientManager::QUERY_SETUP(CPeer * peer, DWORD dwHandle, const char * c_pData)
{
	TPacketGDSetup * p = (TPacketGDSetup *) c_pData;
	c_pData += sizeof(TPacketGDSetup);

	if (p->bAuthServer)
	{
		sys_log(0, "AUTH_PEER ptr %p", peer);

		m_pkAuthPeer = peer;
		return;
	}

	peer->SetPublicIP(p->szPublicIP);
	peer->SetChannel(p->bChannel);
	peer->SetListenPort(p->wListenPort);
	peer->SetP2PPort(p->wP2PPort);
	peer->SetMaps(p->alMaps);

	//
	// Send which map is on which server
	//
	TMapLocation kMapLocations;

	strlcpy(kMapLocations.szHost, peer->GetPublicIP(), sizeof(kMapLocations.szHost));
	kMapLocations.wPort = peer->GetListenPort();
	thecore_memcpy(kMapLocations.alMaps, peer->GetMaps(), sizeof(kMapLocations.alMaps));

	BYTE bMapCount;

	std::vector<TMapLocation> vec_kMapLocations;

	if (peer->GetChannel() == 1)
	{
		for (itertype(m_peerList) i = m_peerList.begin(); i != m_peerList.end(); ++i)
		{
			CPeer * tmp = *i;

			if (tmp == peer)
				continue;

			if (!tmp->GetChannel())
				continue;

			if (tmp->GetChannel() == GUILD_WARP_WAR_CHANNEL || tmp->GetChannel() == peer->GetChannel())
			{
				TMapLocation kMapLocation2;
				strlcpy(kMapLocation2.szHost, tmp->GetPublicIP(), sizeof(kMapLocation2.szHost));
				kMapLocation2.wPort = tmp->GetListenPort();
				thecore_memcpy(kMapLocation2.alMaps, tmp->GetMaps(), sizeof(kMapLocation2.alMaps));
				vec_kMapLocations.push_back(kMapLocation2);

				tmp->EncodeHeader(HEADER_DG_MAP_LOCATIONS, 0, sizeof(BYTE) + sizeof(TMapLocation));
				bMapCount = 1;
				tmp->EncodeBYTE(bMapCount);
				tmp->Encode(&kMapLocations, sizeof(TMapLocation));
			}
		}
	}
	else if (peer->GetChannel() == GUILD_WARP_WAR_CHANNEL)
	{
		for (itertype(m_peerList) i = m_peerList.begin(); i != m_peerList.end(); ++i)
		{
			CPeer * tmp = *i;

			if (tmp == peer)
				continue;

			if (!tmp->GetChannel())
				continue;

			if (tmp->GetChannel() == 1 || tmp->GetChannel() == peer->GetChannel())
			{
				TMapLocation kMapLocation2;
				strlcpy(kMapLocation2.szHost, tmp->GetPublicIP(), sizeof(kMapLocation2.szHost));
				kMapLocation2.wPort = tmp->GetListenPort();
				thecore_memcpy(kMapLocation2.alMaps, tmp->GetMaps(), sizeof(kMapLocation2.alMaps));
				vec_kMapLocations.push_back(kMapLocation2);
			}

			tmp->EncodeHeader(HEADER_DG_MAP_LOCATIONS, 0, sizeof(BYTE) + sizeof(TMapLocation));
			bMapCount = 1;
			tmp->EncodeBYTE(bMapCount);
			tmp->Encode(&kMapLocations, sizeof(TMapLocation));
		}
	}
	else
	{
		for (itertype(m_peerList) i = m_peerList.begin(); i != m_peerList.end(); ++i)
		{
			CPeer * tmp = *i;

			if (tmp == peer)
				continue;

			if (!tmp->GetChannel())
				continue;

			if (tmp->GetChannel() == GUILD_WARP_WAR_CHANNEL || tmp->GetChannel() == peer->GetChannel())
			{
				TMapLocation kMapLocation2;

				strlcpy(kMapLocation2.szHost, tmp->GetPublicIP(), sizeof(kMapLocation2.szHost));
				kMapLocation2.wPort = tmp->GetListenPort();
				thecore_memcpy(kMapLocation2.alMaps, tmp->GetMaps(), sizeof(kMapLocation2.alMaps));

				vec_kMapLocations.push_back(kMapLocation2);
			}

			if (tmp->GetChannel() == peer->GetChannel())
			{
				tmp->EncodeHeader(HEADER_DG_MAP_LOCATIONS, 0, sizeof(BYTE) + sizeof(TMapLocation));
				bMapCount = 1;
				tmp->EncodeBYTE(bMapCount);
				tmp->Encode(&kMapLocations, sizeof(TMapLocation));
			}
		}
	}

	vec_kMapLocations.push_back(kMapLocations);

	peer->EncodeHeader(HEADER_DG_MAP_LOCATIONS, 0, sizeof(BYTE) + sizeof(TMapLocation) * vec_kMapLocations.size());
	bMapCount = vec_kMapLocations.size();
	peer->EncodeBYTE(bMapCount);
	peer->Encode(&vec_kMapLocations[0], sizeof(TMapLocation) * vec_kMapLocations.size());

	//
	// Setup: Make other peers connect to the connected peer. (P2P connection creation)
	//
	sys_log(0, "SETUP: channel %u listen %u p2p %u count %u", peer->GetChannel(), p->wListenPort, p->wP2PPort, bMapCount);

	TPacketDGP2P p2pSetupPacket;
	p2pSetupPacket.wPort = peer->GetP2PPort();
	p2pSetupPacket.bChannel = peer->GetChannel();
	strlcpy(p2pSetupPacket.szHost, peer->GetPublicIP(), sizeof(p2pSetupPacket.szHost));

	//for (TPeerList::const_iterator i = m_peerList.begin(); i != m_peerList.end(); ++i)
	for (itertype(m_peerList) i = m_peerList.begin(); i != m_peerList.end();++i)
	{
		CPeer * tmp = *i;

		if (tmp == peer)
			continue;

		// If the channel is 0, it can be considered a peer or auth that has not yet received a SETUP packet.
		if (0 == tmp->GetChannel())
			continue;

		tmp->EncodeHeader(HEADER_DG_P2P, 0, sizeof(TPacketDGP2P));
		tmp->Encode(&p2pSetupPacket, sizeof(TPacketDGP2P));
	}

	TPacketLoginOnSetup * pck = (TPacketLoginOnSetup *) c_pData;;

	for (DWORD c = 0; c < p->dwLoginCount; ++c, ++pck)
	{
		CLoginData * pkLD = new CLoginData;

		pkLD->SetKey(pck->dwLoginKey);
		pkLD->SetClientKey(pck->adwClientKey);
		pkLD->SetIP(pck->szHost);

		TAccountTable & r = pkLD->GetAccountRef();

		r.id = pck->dwID;
		trim_and_lower(pck->szLogin, r.login, sizeof(r.login));
		strlcpy(r.social_id, pck->szSocialID, sizeof(r.social_id));
		strlcpy(r.passwd, "TEMP", sizeof(r.passwd));
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		r.bLanguage = pck->bLanguage;
#endif

		InsertLoginData(pkLD);

		if (InsertLogonAccount(pck->szLogin, peer->GetHandle(), pck->szHost))
		{
			sys_log(0, "SETUP: login %u %s login_key %u host %s", pck->dwID, pck->szLogin, pck->dwLoginKey, pck->szHost);
			pkLD->SetPlay(true);
		}
		else
			sys_log(0, "SETUP: login_fail %u %s login_key %u", pck->dwID, pck->szLogin, pck->dwLoginKey);
	}

	SendPartyOnSetup(peer);
	CGuildManager::instance().OnSetup(peer);
	CPrivManager::instance().SendPrivOnSetup(peer);
	SendEventFlagsOnSetup(peer);
	marriage::CManager::instance().OnSetup(peer);
}

void CClientManager::QUERY_ITEM_FLUSH(CPeer * pkPeer, const char * c_pData)
{
	DWORD dwID = *(DWORD *) c_pData;

	if (g_log)
		sys_log(0, "HEADER_GD_ITEM_FLUSH: %u", dwID);

	CItemCache * c = GetItemCache(dwID);

	if (c)
		c->Flush();
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
void CClientManager::QUERY_SKILL_COLOR_SAVE(const char* c_pData)
{
	PutSkillColorCache((TSkillColor*)c_pData);
}
#endif

void CClientManager::QUERY_ITEM_SAVE(CPeer * pkPeer, const char * c_pData)
{
	TPlayerItem * p = (TPlayerItem *) c_pData;

	if (p->window == SAFEBOX || p->window == MALL)
	{
		CItemCache * c = GetItemCache(p->id);

		if (c)
		{
			TItemCacheSetPtrMap::iterator it = m_map_pkItemCacheSetPtr.find(c->Get()->owner);

			if (it != m_map_pkItemCacheSetPtr.end())
			{
				if (g_test_server)
					sys_log(0, "ITEM_CACHE: safebox owner %u id %u", c->Get()->owner, c->Get()->id);

				it->second->erase(c);
			}

			m_map_itemCache.erase(p->id);

			delete c;
		}
		char szQuery[512];
		snprintf(szQuery, sizeof(szQuery),
			"REPLACE INTO item%s (id, owner_id, window, pos, count, vnum, "
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			"transmutation, "
#endif
#ifdef ENABLE_SET_ITEM
			"set_value, "
#endif
			"socket0, socket1, socket2, "
			"attrtype0, attrvalue0, "
			"attrtype1, attrvalue1, "
			"attrtype2, attrvalue2, "
			"attrtype3, attrvalue3, "
			"attrtype4, attrvalue4, "
			"attrtype5, attrvalue5, "
			"attrtype6, attrvalue6) "
			"VALUES(%u, %u, %d, %d, %u, %u, "
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			"%d, "
#endif
#ifdef ENABLE_SET_ITEM
			"%u, "
#endif
			"%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
			GetTablePostfix(),
			p->id,
			p->owner,
			p->window,
			p->pos,
			p->count,
			p->vnum,
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			p->dwTransmutationVnum,
#endif
#ifdef ENABLE_SET_ITEM
			p->dwSetValue,
#endif
			p->alSockets[0],
			p->alSockets[1],
			p->alSockets[2],
			p->aAttr[0].bType, p->aAttr[0].sValue,
			p->aAttr[1].bType, p->aAttr[1].sValue,
			p->aAttr[2].bType, p->aAttr[2].sValue,
			p->aAttr[3].bType, p->aAttr[3].sValue,
			p->aAttr[4].bType, p->aAttr[4].sValue,
			p->aAttr[5].bType, p->aAttr[5].sValue,
			p->aAttr[6].bType, p->aAttr[6].sValue);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEM_SAVE, pkPeer->GetHandle(), NULL);
	}
	else
	{
		if (g_test_server)
			sys_log(0, "QUERY_ITEM_SAVE => PutItemCache() owner %d id %d vnum %d ", p->owner, p->id, p->vnum);

		PutItemCache(p);
	}
}

CClientManager::TItemCacheSet * CClientManager::GetItemCacheSet(DWORD pid)
{
	TItemCacheSetPtrMap::iterator it = m_map_pkItemCacheSetPtr.find(pid);

	if (it == m_map_pkItemCacheSetPtr.end())
		return NULL;

	return it->second;
}

void CClientManager::CreateItemCacheSet(DWORD pid)
{
	if (m_map_pkItemCacheSetPtr.find(pid) != m_map_pkItemCacheSetPtr.end())
		return;

	TItemCacheSet * pSet = new TItemCacheSet;
	m_map_pkItemCacheSetPtr.insert(TItemCacheSetPtrMap::value_type(pid, pSet));

	if (g_log)
		sys_log(0, "ITEM_CACHE: new cache %u", pid);
}

void CClientManager::FlushItemCacheSet(DWORD pid)
{
	TItemCacheSetPtrMap::iterator it = m_map_pkItemCacheSetPtr.find(pid);

	if (it == m_map_pkItemCacheSetPtr.end())
	{
		sys_log(0, "FLUSH_ITEMCACHESET : No ItemCacheSet pid(%d)", pid);
		return;
	}

	TItemCacheSet * pSet = it->second;
	TItemCacheSet::iterator it_set = pSet->begin();

	while (it_set != pSet->end())
	{
		CItemCache * c = *it_set++;
		c->Flush();

		m_map_itemCache.erase(c->Get()->id);
		delete c;
	}

	pSet->clear();
	delete pSet;

	m_map_pkItemCacheSetPtr.erase(it);

	if (g_log)
		sys_log(0, "FLUSH_ITEMCACHESET : Deleted pid(%d)", pid);
}

CItemCache * CClientManager::GetItemCache(DWORD id)
{
	TItemCacheMap::iterator it = m_map_itemCache.find(id);

	if (it == m_map_itemCache.end())
		return NULL;

	return it->second;
}

void CClientManager::PutItemCache(TPlayerItem * pNew, bool bSkipQuery)
{
	CItemCache * c;

	c = GetItemCache(pNew->id);

	if (!c)
	{
		if (g_log)
			sys_log(0, "ITEM_CACHE: PutItemCache ==> New CItemCache id%d vnum%d new owner%d", pNew->id, pNew->vnum, pNew->owner);

		c = new CItemCache;
		m_map_itemCache.insert(TItemCacheMap::value_type(pNew->id, c));
	}

	else
	{
		if (g_log)
			sys_log(0, "ITEM_CACHE: PutItemCache ==> Have Cache");

		if (pNew->owner != c->Get()->owner)
		{
			TItemCacheSetPtrMap::iterator it = m_map_pkItemCacheSetPtr.find(c->Get()->owner);

			if (it != m_map_pkItemCacheSetPtr.end())
			{
				if (g_log)
				sys_log(0, "ITEM_CACHE: delete owner %u id %u new owner %u", c->Get()->owner, c->Get()->id, pNew->owner);
				it->second->erase(c);
			}
		}
	}

	c->Put(pNew, bSkipQuery);

	TItemCacheSetPtrMap::iterator it = m_map_pkItemCacheSetPtr.find(c->Get()->owner);

	if (it != m_map_pkItemCacheSetPtr.end())
	{
		if (g_log)
			sys_log(0, "ITEM_CACHE: save %u id %u", c->Get()->owner, c->Get()->id);
		else
			sys_log(1, "ITEM_CACHE: save %u id %u", c->Get()->owner, c->Get()->id);
		it->second->insert(c);
	}
	else
	{
		if (g_log)
			sys_log(0, "ITEM_CACHE: direct save %u id %u", c->Get()->owner, c->Get()->id);
		else
			sys_log(1, "ITEM_CACHE: direct save %u id %u", c->Get()->owner, c->Get()->id);

		c->OnFlush();
	}
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
CSKillColorCache* CClientManager::GetSkillColorCache(DWORD dwID)
{
	TSkillColorCacheMap::iterator it = m_map_SkillColorCache.find(dwID);

	if (it == m_map_SkillColorCache.end())
		return NULL;

	return it->second;
}

void CClientManager::PutSkillColorCache(const TSkillColor* c_pNew)
{
	CSKillColorCache* pCache = GetSkillColorCache(c_pNew->dwPlayerID);

	if (!pCache)
	{
		pCache = new CSKillColorCache;
		m_map_SkillColorCache.insert(TSkillColorCacheMap::value_type(c_pNew->dwPlayerID, pCache));
	}

	pCache->Put(const_cast<TSkillColor*>(c_pNew), false);
}

void CClientManager::UpdateSkillColorCache()
{
	TSkillColorCacheMap::iterator it = m_map_SkillColorCache.begin();

	while (it != m_map_SkillColorCache.end())
	{
		CSKillColorCache* pCache = it->second;

		if (pCache->CheckFlushTimeout())
		{
			pCache->Flush();
			m_map_SkillColorCache.erase(it++);
		}
		else
			++it;
	}
}
#endif

bool CClientManager::DeleteItemCache(DWORD dwID)
{
	CItemCache * c = GetItemCache(dwID);

	if (!c)
		return false;

	c->Delete();
	return true;
}

CItemPriceListTableCache* CClientManager::GetItemPriceListCache(DWORD dwID)
{
	TItemPriceListCacheMap::iterator it = m_mapItemPriceListCache.find(dwID);

	if (it == m_mapItemPriceListCache.end())
		return NULL;

	return it->second;
}

void CClientManager::PutItemPriceListCache(const TItemPriceListTable* pItemPriceList)
{
	CItemPriceListTableCache* pCache = GetItemPriceListCache(pItemPriceList->dwOwnerID);

	if (!pCache)
	{
		pCache = new CItemPriceListTableCache;
		m_mapItemPriceListCache.insert(TItemPriceListCacheMap::value_type(pItemPriceList->dwOwnerID, pCache));
	}

	pCache->Put(const_cast<TItemPriceListTable*>(pItemPriceList), true);
}

void CClientManager::UpdatePlayerCache()
{
	TPlayerTableCacheMap::iterator it = m_map_playerCache.begin();

	while (it != m_map_playerCache.end())
	{
		CPlayerTableCache * c = (it++)->second;

		if (c->CheckTimeout())
		{
			if (g_log)
				sys_log(0, "UPDATE : UpdatePlayerCache() ==> FlushPlayerCache %d %s ", c->Get(false)->id, c->Get(false)->name);

			c->Flush();

			UpdateItemCacheSet(c->Get()->id);
#ifdef ENABLE_GROWTH_PET_SYSTEM
			UpdateGrowthPetCacheSet(c->Get()->id);
#endif
		}
		else if (c->CheckFlushTimeout())
			c->Flush();
	}
}

void CClientManager::SetCacheFlushCountLimit(int iLimit)
{
	m_iCacheFlushCountLimit = MAX(10, iLimit);
	sys_log(0, "CACHE_FLUSH_LIMIT_PER_SECOND: %d", m_iCacheFlushCountLimit);
}

void CClientManager::UpdateItemCache()
{
	if (m_iCacheFlushCount >= m_iCacheFlushCountLimit)
		return;

	TItemCacheMap::iterator it = m_map_itemCache.begin();

	while (it != m_map_itemCache.end())
	{
		CItemCache * c = (it++)->second;

		if (c->CheckFlushTimeout())
		{
			if (g_test_server)
				sys_log(0, "UpdateItemCache ==> Flush() vnum %d id owner %d", c->Get()->vnum, c->Get()->id, c->Get()->owner);

			c->Flush();

			if (++m_iCacheFlushCount >= m_iCacheFlushCountLimit)
				break;
		}
	}
}

void CClientManager::UpdateItemPriceListCache()
{
	TItemPriceListCacheMap::iterator it = m_mapItemPriceListCache.begin();

	while (it != m_mapItemPriceListCache.end())
	{
		CItemPriceListTableCache* pCache = it->second;

		if (pCache->CheckFlushTimeout())
		{
			pCache->Flush();
			m_mapItemPriceListCache.erase(it++);
		}
		else
			++it;
	}
}

void CClientManager::QUERY_ITEM_DESTROY(CPeer * pkPeer, const char * c_pData)
{
	DWORD dwID = *(DWORD *) c_pData;
	c_pData += sizeof(DWORD);

	DWORD dwPID = *(DWORD *) c_pData;

	if (!DeleteItemCache(dwID))
	{
		char szQuery[64];
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM item%s WHERE id=%u", GetTablePostfix(), dwID);

		if (g_log)
			sys_log(0, "HEADER_GD_ITEM_DESTROY: PID %u ID %u", dwPID, dwID);

		if (dwPID == 0)
			CDBManager::instance().AsyncQuery(szQuery);
		else
			CDBManager::instance().ReturnQuery(szQuery, QID_ITEM_DESTROY, pkPeer->GetHandle(), NULL);
	}
}

void CClientManager::QUERY_FLUSH_CACHE(CPeer * pkPeer, const char * c_pData)
{
	DWORD dwPID = *(DWORD *) c_pData;

	CPlayerTableCache * pkCache = GetPlayerCache(dwPID);

	if (!pkCache)
		return;

	sys_log(0, "FLUSH_CACHE: %u", dwPID);

	pkCache->Flush();
	FlushItemCacheSet(dwPID);
#ifdef ENABLE_GROWTH_PET_SYSTEM
	FlushGrowthPetCacheSet(dwPID);
#endif

	m_map_playerCache.erase(dwPID);
	delete pkCache;
}

void CClientManager::QUERY_RELOAD_PROTO()
{
	if (!InitializeTables())
	{
		sys_err("QUERY_RELOAD_PROTO: cannot load tables");
		return;
	}

	for (TPeerList::iterator i = m_peerList.begin(); i != m_peerList.end(); ++i)
	{
		CPeer * tmp = *i;

		if (!tmp->GetChannel())
			continue;

		tmp->EncodeHeader(HEADER_DG_RELOAD_PROTO, 0,
				sizeof(WORD) + sizeof(TSkillTable) * m_vec_skillTable.size() +
				sizeof(WORD) + sizeof(TBanwordTable) * m_vec_banwordTable.size() +
				sizeof(WORD) + sizeof(TItemTable) * m_vec_itemTable.size() +
				sizeof(WORD) + sizeof(TMobTable) * m_vec_mobTable.size() +
#ifdef ENABLE_GROWTH_PET_SYSTEM
				sizeof(WORD) + sizeof(TGrowthPetSkillTable) * m_vec_growthPetSkillTable.size() +
#endif
				sizeof(WORD) + sizeof(TShopTable) * m_iShopTableSize +
#ifdef ENABLE_RENEWAL_SHOPEX
				sizeof(WORD) + sizeof(TShopTable) * m_iShopEXTableSize +
#endif
				sizeof(WORD) + sizeof(TRefineTable)* m_iRefineTableSize +
				sizeof(WORD) + sizeof(TItemAttrTable)*m_vec_itemAttrTable.size() +
				sizeof(WORD) + sizeof(TItemAttrTable)*m_vec_itemRareTable.size());

		tmp->EncodeWORD(m_vec_skillTable.size());
		tmp->Encode(&m_vec_skillTable[0], sizeof(TSkillTable) * m_vec_skillTable.size());

		tmp->EncodeWORD(m_vec_banwordTable.size());
		tmp->Encode(&m_vec_banwordTable[0], sizeof(TBanwordTable) * m_vec_banwordTable.size());

		tmp->EncodeWORD(m_vec_itemTable.size());
		tmp->Encode(&m_vec_itemTable[0], sizeof(TItemTable) * m_vec_itemTable.size());

		tmp->EncodeWORD(m_vec_mobTable.size());
		tmp->Encode(&m_vec_mobTable[0], sizeof(TMobTable) * m_vec_mobTable.size());

#ifdef ENABLE_GROWTH_PET_SYSTEM
		tmp->EncodeWORD(m_vec_growthPetSkillTable.size());
		if (m_vec_growthPetSkillTable.size())
			tmp->Encode(&m_vec_growthPetSkillTable[0], sizeof(TGrowthPetSkillTable) * m_vec_growthPetSkillTable.size());
#endif

		tmp->EncodeWORD(m_iShopTableSize);
		tmp->Encode(m_pShopTable, sizeof(TShopTable) * m_iShopTableSize);

#ifdef ENABLE_RENEWAL_SHOPEX
		tmp->EncodeWORD(m_iShopEXTableSize);
		tmp->Encode(m_pShopEXTable, sizeof(TShopTable) * m_iShopEXTableSize);
#endif

		tmp->EncodeWORD(m_iRefineTableSize);
		tmp->Encode(m_pRefineTable, sizeof(TRefineTable) * m_iRefineTableSize);

		tmp->EncodeWORD(m_vec_itemAttrTable.size());
		tmp->Encode(&m_vec_itemAttrTable[0], sizeof(TItemAttrTable) * m_vec_itemAttrTable.size());

		tmp->EncodeWORD(m_vec_itemRareTable.size());
		tmp->Encode(&m_vec_itemRareTable[0], sizeof(TItemAttrTable) * m_vec_itemRareTable.size());
	}
}

// ADD_GUILD_PRIV_TIME
/**
 * @version	05/06/08 Bang2ni
 */
void CClientManager::AddGuildPriv(TPacketGiveGuildPriv* p)
{
	CPrivManager::instance().AddGuildPriv(p->guild_id, p->type, p->value, p->duration_sec);
}

void CClientManager::AddEmpirePriv(TPacketGiveEmpirePriv* p)
{
	CPrivManager::instance().AddEmpirePriv(p->empire, p->type, p->value, p->duration_sec);
}
// END_OF_ADD_GUILD_PRIV_TIME

void CClientManager::AddCharacterPriv(TPacketGiveCharacterPriv* p)
{
	CPrivManager::instance().AddCharPriv(p->pid, p->type, p->value);
}

void CClientManager::MoneyLog(TPacketMoneyLog* p)
{
	CMoneyLog::instance().AddLog(p->type, p->vnum, p->gold);
}

CLoginData * CClientManager::GetLoginData(DWORD dwKey)
{
	TLoginDataByLoginKey::iterator it = m_map_pkLoginData.find(dwKey);

	if (it == m_map_pkLoginData.end())
		return NULL;

	return it->second;
}

CLoginData * CClientManager::GetLoginDataByLogin(const char * c_pszLogin)
{
	char szLogin[LOGIN_MAX_LEN + 1];
	trim_and_lower(c_pszLogin, szLogin, sizeof(szLogin));

	TLoginDataByLogin::iterator it = m_map_pkLoginDataByLogin.find(szLogin);

	if (it == m_map_pkLoginDataByLogin.end())
		return NULL;

	return it->second;
}

CLoginData * CClientManager::GetLoginDataByAID(DWORD dwAID)
{
	TLoginDataByAID::iterator it = m_map_pkLoginDataByAID.find(dwAID);

	if (it == m_map_pkLoginDataByAID.end())
		return NULL;

	return it->second;
}

void CClientManager::InsertLoginData(CLoginData * pkLD)
{
	char szLogin[LOGIN_MAX_LEN + 1];
	trim_and_lower(pkLD->GetAccountRef().login, szLogin, sizeof(szLogin));

	m_map_pkLoginData.insert(std::make_pair(pkLD->GetKey(), pkLD));
	m_map_pkLoginDataByLogin.insert(std::make_pair(szLogin, pkLD));
	m_map_pkLoginDataByAID.insert(std::make_pair(pkLD->GetAccountRef().id, pkLD));
}

void CClientManager::DeleteLoginData(CLoginData * pkLD)
{
	m_map_pkLoginData.erase(pkLD->GetKey());
	m_map_pkLoginDataByLogin.erase(pkLD->GetAccountRef().login);
	m_map_pkLoginDataByAID.erase(pkLD->GetAccountRef().id);

	if (m_map_kLogonAccount.find(pkLD->GetAccountRef().login) == m_map_kLogonAccount.end())
		delete pkLD;
	else
		pkLD->SetDeleted(true);
}

void CClientManager::QUERY_AUTH_LOGIN(CPeer * pkPeer, DWORD dwHandle, TPacketGDAuthLogin * p)
{
	if (g_test_server)
		sys_log(0, "QUERY_AUTH_LOGIN %d %d %s", p->dwID, p->dwLoginKey, p->szLogin);
	CLoginData * pkLD = GetLoginDataByLogin(p->szLogin);

	if (pkLD)
	{
		DeleteLoginData(pkLD);
	}

	BYTE bResult;

	if (GetLoginData(p->dwLoginKey))
	{
		sys_err("LoginData already exist key %u login %s", p->dwLoginKey, p->szLogin);
		bResult = 0;

		pkPeer->EncodeHeader(HEADER_DG_AUTH_LOGIN, dwHandle, sizeof(BYTE));
		pkPeer->EncodeBYTE(bResult);
	}
	else
	{
		CLoginData * pkLD = new CLoginData;

		pkLD->SetKey(p->dwLoginKey);
		pkLD->SetClientKey(p->adwClientKey);
		pkLD->SetPremium(p->iPremiumTimes);

		TAccountTable & r = pkLD->GetAccountRef();

		r.id = p->dwID;
		trim_and_lower(p->szLogin, r.login, sizeof(r.login));
		strlcpy(r.social_id, p->szSocialID, sizeof(r.social_id));
		strlcpy(r.passwd, "TEMP", sizeof(r.passwd));
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		r.bLanguage = p->bLanguage;
#endif

		sys_log(0, "AUTH_LOGIN id(%u) login(%s)"
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
				" language(%d)"
#endif
				" social_id(%s) login_key(%u), client_key(%u %u %u %u)",
				p->dwID, p->szLogin,
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
				p->bLanguage,
#endif
				p->szSocialID,
				p->dwLoginKey,
				p->adwClientKey[0], p->adwClientKey[1], p->adwClientKey[2], p->adwClientKey[3]);

		bResult = 1;

		InsertLoginData(pkLD);

		pkPeer->EncodeHeader(HEADER_DG_AUTH_LOGIN, dwHandle, sizeof(BYTE));
		pkPeer->EncodeBYTE(bResult);
	}
}

void CClientManager::GuildDepositMoney(TPacketGDGuildMoney* p)
{
	CGuildManager::instance().DepositMoney(p->dwGuild, p->iGold);
}

void CClientManager::GuildWithdrawMoney(CPeer* peer, TPacketGDGuildMoney* p)
{
	CGuildManager::instance().WithdrawMoney(peer, p->dwGuild, p->iGold);
}

void CClientManager::GuildWithdrawMoneyGiveReply(TPacketGDGuildMoneyWithdrawGiveReply* p)
{
	CGuildManager::instance().WithdrawMoneyReply(p->dwGuild, p->bGiveSuccess, p->iChangeGold);
}

void CClientManager::GuildWarBet(TPacketGDGuildWarBet * p)
{
	CGuildManager::instance().Bet(p->dwWarID, p->szLogin, p->dwGold, p->dwGuild);
}

void CClientManager::CreateObject(TPacketGDCreateObject * p)
{
	using namespace building;

	char szQuery[512];

	snprintf(szQuery, sizeof(szQuery),
			"INSERT INTO object%s (land_id, vnum, map_index, x, y, x_rot, y_rot, z_rot) VALUES(%u, %u, %d, %d, %d, %f, %f, %f)",
			GetTablePostfix(), p->dwLandID, p->dwVnum, p->lMapIndex, p->x, p->y, p->xRot, p->yRot, p->zRot);

	std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

	if (pmsg->Get()->uiInsertID == 0)
	{
		sys_err("cannot insert object");
		return;
	}

	TObject * pkObj = new TObject;

	memset(pkObj, 0, sizeof(TObject));

	pkObj->dwID = pmsg->Get()->uiInsertID;
	pkObj->dwVnum = p->dwVnum;
	pkObj->dwLandID = p->dwLandID;
	pkObj->lMapIndex = p->lMapIndex;
	pkObj->x = p->x;
	pkObj->y = p->y;
	pkObj->xRot = p->xRot;
	pkObj->yRot = p->yRot;
	pkObj->zRot = p->zRot;
	pkObj->lLife = 0;

	ForwardPacket(HEADER_DG_CREATE_OBJECT, pkObj, sizeof(TObject));

	m_map_pkObjectTable.insert(std::make_pair(pkObj->dwID, pkObj));
}

void CClientManager::DeleteObject(DWORD dwID)
{
	char szQuery[128];

	snprintf(szQuery, sizeof(szQuery), "DELETE FROM object%s WHERE id=%u", GetTablePostfix(), dwID);

	std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));

	if (pmsg->Get()->uiAffectedRows == 0 || pmsg->Get()->uiAffectedRows == (uint32_t)-1)
	{
		sys_err("no object by id %u", dwID);
		return;
	}

	itertype(m_map_pkObjectTable) it = m_map_pkObjectTable.find(dwID);

	if (it != m_map_pkObjectTable.end())
	{
		delete it->second;
		m_map_pkObjectTable.erase(it);
	}

	ForwardPacket(HEADER_DG_DELETE_OBJECT, &dwID, sizeof(DWORD));
}

void CClientManager::UpdateLand(DWORD * pdw)
{
	DWORD dwID = pdw[0];
	DWORD dwGuild = pdw[1];

	building::TLand * p = &m_vec_kLandTable[0];

	DWORD i;

	for (i = 0; i < m_vec_kLandTable.size(); ++i, ++p)
	{
		if (p->dwID == dwID)
		{
			char buf[256];
			snprintf(buf, sizeof(buf), "UPDATE land%s SET guild_id=%u WHERE id=%u", GetTablePostfix(), dwGuild, dwID);
			CDBManager::instance().AsyncQuery(buf);

			p->dwGuildID = dwGuild;
			break;
		}
	}

	if (i < m_vec_kLandTable.size())
		ForwardPacket(HEADER_DG_UPDATE_LAND, p, sizeof(building::TLand));
}

// BLOCK_CHAT
void CClientManager::BlockChat(TPacketBlockChat* p)
{
	char szQuery[256];
	snprintf(szQuery, sizeof(szQuery), "SELECT id FROM player%s WHERE name = '%s'", GetTablePostfix(), p->szName);
	std::unique_ptr<SQLMsg> pmsg(CDBManager::instance().DirectQuery(szQuery));
	SQLResult * pRes = pmsg->Get();

	if (pRes->uiNumRows)
	{
		MYSQL_ROW row = mysql_fetch_row(pRes->pSQLResult);
		DWORD pid = strtoul(row[0], NULL, 10);

		TPacketGDAddAffect pa;
		pa.dwPID = pid;
		pa.elem.dwType = 223;
		pa.elem.bApplyOn = 0;
		pa.elem.lApplyValue = 0;
		pa.elem.dwFlag = 0;
		pa.elem.lDuration = p->lDuration;
		pa.elem.lSPCost = 0;
		QUERY_ADD_AFFECT(NULL, &pa);
	}
	else {}
}
// END_OF_BLOCK_CHAT

void CClientManager::MarriageAdd(TPacketMarriageAdd * p)
{
	sys_log(0, "MarriageAdd %u %u %s %s", p->dwPID1, p->dwPID2, p->szName1, p->szName2);
	marriage::CManager::instance().Add(p->dwPID1, p->dwPID2, p->szName1, p->szName2);
}

void CClientManager::MarriageUpdate(TPacketMarriageUpdate * p)
{
	sys_log(0, "MarriageUpdate PID:%u %u LP:%d ST:%d", p->dwPID1, p->dwPID2, p->iLovePoint, p->byMarried);
	marriage::CManager::instance().Update(p->dwPID1, p->dwPID2, p->iLovePoint, p->byMarried);
}

void CClientManager::MarriageRemove(TPacketMarriageRemove * p)
{
	sys_log(0, "MarriageRemove %u %u", p->dwPID1, p->dwPID2);
	marriage::CManager::instance().Remove(p->dwPID1, p->dwPID2);
}

void CClientManager::WeddingRequest(TPacketWeddingRequest * p)
{
	sys_log(0, "WeddingRequest %u %u", p->dwPID1, p->dwPID2);
	ForwardPacket(HEADER_DG_WEDDING_REQUEST, p, sizeof(TPacketWeddingRequest));
}

void CClientManager::WeddingReady(TPacketWeddingReady * p)
{
	sys_log(0, "WeddingReady %u %u", p->dwPID1, p->dwPID2);
	ForwardPacket(HEADER_DG_WEDDING_READY, p, sizeof(TPacketWeddingReady));
	marriage::CManager::instance().ReadyWedding(p->dwMapIndex, p->dwPID1, p->dwPID2);
}

void CClientManager::WeddingEnd(TPacketWeddingEnd * p)
{
	sys_log(0, "WeddingEnd %u %u", p->dwPID1, p->dwPID2);
	marriage::CManager::instance().EndWedding(p->dwPID1, p->dwPID2);
}

void CClientManager::MyshopPricelistUpdate(const TPacketMyshopPricelistHeader* pPacket)
{
	if (pPacket->byCount > SHOP_PRICELIST_MAX_NUM)
	{
		sys_err("count overflow!");
		return;
	}

	CItemPriceListTableCache* pCache = GetItemPriceListCache(pPacket->dwOwnerID);

	if (pCache)
	{
		TItemPriceListTable table;

		table.dwOwnerID = pPacket->dwOwnerID;
		table.byCount = pPacket->byCount;

		const TItemPriceInfo * pInfo = reinterpret_cast<const TItemPriceInfo*>(pPacket + sizeof(TPacketMyshopPricelistHeader));
		thecore_memcpy(table.aPriceInfo, pInfo, sizeof(TItemPriceInfo) * pPacket->byCount);

		pCache->UpdateList(&table);
	}
	else
	{
		TItemPriceListTable* pUpdateTable = new TItemPriceListTable;

		pUpdateTable->dwOwnerID = pPacket->dwOwnerID;
		pUpdateTable->byCount = pPacket->byCount;

		const TItemPriceInfo * pInfo = reinterpret_cast<const TItemPriceInfo*>(pPacket + sizeof(TPacketMyshopPricelistHeader));
		thecore_memcpy(pUpdateTable->aPriceInfo, pInfo, sizeof(TItemPriceInfo) * pPacket->byCount);

		char szQuery[QUERY_MAX_LEN];
		snprintf(szQuery, sizeof(szQuery), "SELECT item_vnum, price FROM myshop_pricelist%s WHERE owner_id=%u", GetTablePostfix(), pPacket->dwOwnerID);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_LOAD_FOR_UPDATE, 0, pUpdateTable);
	}
}

void CClientManager::MyshopPricelistRequest(CPeer* peer, DWORD dwHandle, DWORD dwPlayerID)
{
	if (CItemPriceListTableCache* pCache = GetItemPriceListCache(dwPlayerID))
	{
		sys_log(0, "Cache MyShopPricelist handle[%d] pid[%d]", dwHandle, dwPlayerID);

		TItemPriceListTable* pTable = pCache->Get(false);

		TPacketMyshopPricelistHeader header =
		{
			pTable->dwOwnerID,
			pTable->byCount
		};

		size_t sizePriceListSize = sizeof(TItemPriceInfo) * pTable->byCount;

		peer->EncodeHeader(HEADER_DG_MYSHOP_PRICELIST_RES, dwHandle, sizeof(header) + sizePriceListSize);
		peer->Encode(&header, sizeof(header));
		peer->Encode(pTable->aPriceInfo, sizePriceListSize);

	}
	else
	{
		sys_log(0, "Query MyShopPricelist handle[%d] pid[%d]", dwHandle, dwPlayerID);

		char szQuery[QUERY_MAX_LEN];
		snprintf(szQuery, sizeof(szQuery), "SELECT item_vnum, price FROM myshop_pricelist%s WHERE owner_id=%u", GetTablePostfix(), dwPlayerID);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_LOAD, peer->GetHandle(), new TItemPricelistReqInfo(dwHandle, dwPlayerID));
	}
}

void CPacketInfo::Add(int header)
{
	itertype(m_map_info) it = m_map_info.find(header);

	if (it == m_map_info.end())
		m_map_info.insert(std::map<int, int>::value_type(header, 1));
	else
		++it->second;
}

void CPacketInfo::Reset()
{
	m_map_info.clear();
}

void CClientManager::ProcessPackets(CPeer * peer)
{
	BYTE		header;
	DWORD		dwHandle;
	DWORD		dwLength;
	const char * data = NULL;
	int			i = 0;
	int			iCount = 0;

	while (peer->PeekPacket(i, header, dwHandle, dwLength, &data))
	{
		m_bLastHeader = header;
		++iCount;

#ifdef _TEST
		if (header != 10)
			sys_log(0, " ProcessPacket Header [%d] Handle[%d] Length[%d] iCount[%d]", header, dwHandle, dwLength, iCount);
#endif
		if (g_test_server)
		{
			if (header != 10)
				sys_log(0, " ProcessPacket Header [%d] Handle[%d] Length[%d] iCount[%d]", header, dwHandle, dwLength, iCount);
		}

		switch (header)
		{
			case HEADER_GD_BOOT:
				QUERY_BOOT(peer, (TPacketGDBoot *) data);
				break;

			case HEADER_GD_LOGIN_BY_KEY:
				QUERY_LOGIN_BY_KEY(peer, dwHandle, (TPacketGDLoginByKey *) data);
				break;

			case HEADER_GD_LOGOUT:
				//sys_log(0, "HEADER_GD_LOGOUT (handle: %d length: %d)", dwHandle, dwLength);
				QUERY_LOGOUT(peer, dwHandle, data);
				break;

			case HEADER_GD_PLAYER_LOAD:
				sys_log(1, "HEADER_GD_PLAYER_LOAD (handle: %d length: %d)", dwHandle, dwLength);
				QUERY_PLAYER_LOAD(peer, dwHandle, (TPlayerLoadPacket *) data);
#ifdef ENABLE_SKILL_COLOR_SYSTEM
				QUERY_SKILL_COLOR_LOAD(peer, dwHandle, (TPlayerLoadPacket*)data);
#endif
				break;

			case HEADER_GD_PLAYER_SAVE:
				sys_log(1, "HEADER_GD_PLAYER_SAVE (handle: %d length: %d)", dwHandle, dwLength);
				QUERY_PLAYER_SAVE(peer, dwHandle, (TPlayerTable *) data);
				break;

			case HEADER_GD_PLAYER_CREATE:
				sys_log(0, "HEADER_GD_PLAYER_CREATE (handle: %d length: %d)", dwHandle, dwLength);
				__QUERY_PLAYER_CREATE(peer, dwHandle, (TPlayerCreatePacket *) data);
				sys_log(0, "END");
				break;

			case HEADER_GD_PLAYER_DELETE:
				sys_log(1, "HEADER_GD_PLAYER_DELETE (handle: %d length: %d)", dwHandle, dwLength);
				__QUERY_PLAYER_DELETE(peer, dwHandle, (TPlayerDeletePacket *) data);
				break;

			case HEADER_GD_PLAYER_COUNT:
				QUERY_PLAYER_COUNT(peer, (TPlayerCountPacket *) data);
				break;

			case HEADER_GD_QUEST_SAVE:
				sys_log(1, "HEADER_GD_QUEST_SAVE (handle: %d length: %d)", dwHandle, dwLength);
				QUERY_QUEST_SAVE(peer, (TQuestTable *) data, dwLength);
				break;

			case HEADER_GD_SAFEBOX_LOAD:
				QUERY_SAFEBOX_LOAD(peer, dwHandle, (TSafeboxLoadPacket *) data, 0);
				break;

			case HEADER_GD_SAFEBOX_SAVE:
				sys_log(1, "HEADER_GD_SAFEBOX_SAVE (handle: %d length: %d)", dwHandle, dwLength);
				QUERY_SAFEBOX_SAVE(peer, (TSafeboxTable *) data);
				break;

			case HEADER_GD_SAFEBOX_CHANGE_SIZE:
				QUERY_SAFEBOX_CHANGE_SIZE(peer, dwHandle, (TSafeboxChangeSizePacket *) data);
				break;

			case HEADER_GD_SAFEBOX_CHANGE_PASSWORD:
				QUERY_SAFEBOX_CHANGE_PASSWORD(peer, dwHandle, (TSafeboxChangePasswordPacket *) data);
				break;

			case HEADER_GD_MALL_LOAD:
				QUERY_SAFEBOX_LOAD(peer, dwHandle, (TSafeboxLoadPacket *) data, 1);
				break;

			case HEADER_GD_EMPIRE_SELECT:
				QUERY_EMPIRE_SELECT(peer, dwHandle, (TEmpireSelectPacket *) data);
				break;

			case HEADER_GD_SETUP:
				QUERY_SETUP(peer, dwHandle, data);
				break;

			case HEADER_GD_GUILD_CREATE:
				GuildCreate(peer, *(DWORD *) data);
				break;

			case HEADER_GD_GUILD_SKILL_UPDATE:
				GuildSkillUpdate(peer, (TPacketGuildSkillUpdate *) data);
				break;

			case HEADER_GD_GUILD_EXP_UPDATE:
				GuildExpUpdate(peer, (TPacketGuildExpUpdate *) data);
				break;

			case HEADER_GD_GUILD_ADD_MEMBER:
				GuildAddMember(peer, (TPacketGDGuildAddMember*) data);
				break;

			case HEADER_GD_GUILD_REMOVE_MEMBER:
				GuildRemoveMember(peer, (TPacketGuild*) data);
				break;

			case HEADER_GD_GUILD_CHANGE_GRADE:
				GuildChangeGrade(peer, (TPacketGuild*) data);
				break;

			case HEADER_GD_GUILD_CHANGE_MEMBER_DATA:
				GuildChangeMemberData(peer, (TPacketGuildChangeMemberData*) data);
				break;

			case HEADER_GD_GUILD_DISBAND:
				GuildDisband(peer, (TPacketGuild*) data);
				break;

			case HEADER_GD_GUILD_WAR:
				GuildWar(peer, (TPacketGuildWar*) data);
				break;

			case HEADER_GD_GUILD_WAR_SCORE:
				GuildWarScore(peer, (TPacketGuildWarScore*) data);
				break;

			case HEADER_GD_GUILD_CHANGE_LADDER_POINT:
				GuildChangeLadderPoint((TPacketGuildLadderPoint*) data);
				break;

			case HEADER_GD_GUILD_USE_SKILL:
				GuildUseSkill((TPacketGuildUseSkill*) data);
				break;

			case HEADER_GD_FLUSH_CACHE:
				QUERY_FLUSH_CACHE(peer, data);
				break;

			case HEADER_GD_ITEM_SAVE:
				QUERY_ITEM_SAVE(peer, data);
				break;

			case HEADER_GD_ITEM_DESTROY:
				QUERY_ITEM_DESTROY(peer, data);
				break;

			case HEADER_GD_ITEM_FLUSH:
				QUERY_ITEM_FLUSH(peer, data);
				break;

			case HEADER_GD_ADD_AFFECT:
				sys_log(1, "HEADER_GD_ADD_AFFECT");
				QUERY_ADD_AFFECT(peer, (TPacketGDAddAffect *) data);
				break;

			case HEADER_GD_REMOVE_AFFECT:
				sys_log(1, "HEADER_GD_REMOVE_AFFECT");
				QUERY_REMOVE_AFFECT(peer, (TPacketGDRemoveAffect *) data);
				break;

			case HEADER_GD_HIGHSCORE_REGISTER:
				QUERY_HIGHSCORE_REGISTER(peer, (TPacketGDHighscore *) data);
				break;

			case HEADER_GD_PARTY_CREATE:
				QUERY_PARTY_CREATE(peer, (TPacketPartyCreate*) data);
				break;

			case HEADER_GD_PARTY_DELETE:
				QUERY_PARTY_DELETE(peer, (TPacketPartyDelete*) data);
				break;

			case HEADER_GD_PARTY_ADD:
				QUERY_PARTY_ADD(peer, (TPacketPartyAdd*) data);
				break;

			case HEADER_GD_PARTY_REMOVE:
				QUERY_PARTY_REMOVE(peer, (TPacketPartyRemove*) data);
				break;

			case HEADER_GD_PARTY_STATE_CHANGE:
				QUERY_PARTY_STATE_CHANGE(peer, (TPacketPartyStateChange*) data);
				break;

			case HEADER_GD_PARTY_SET_MEMBER_LEVEL:
				QUERY_PARTY_SET_MEMBER_LEVEL(peer, (TPacketPartySetMemberLevel*) data);
				break;

			case HEADER_GD_RELOAD_PROTO:
				QUERY_RELOAD_PROTO();
				break;

			case HEADER_GD_CHANGE_NAME:
				QUERY_CHANGE_NAME(peer, dwHandle, (TPacketGDChangeName *) data);
				break;

			case HEADER_GD_AUTH_LOGIN:
				QUERY_AUTH_LOGIN(peer, dwHandle, (TPacketGDAuthLogin *) data);
				break;

			case HEADER_GD_REQUEST_GUILD_PRIV:
				AddGuildPriv((TPacketGiveGuildPriv*)data);
				break;

			case HEADER_GD_REQUEST_EMPIRE_PRIV:
				AddEmpirePriv((TPacketGiveEmpirePriv*)data);
				break;

			case HEADER_GD_REQUEST_CHARACTER_PRIV:
				AddCharacterPriv((TPacketGiveCharacterPriv*) data);
				break;

			case HEADER_GD_MONEY_LOG:
				MoneyLog((TPacketMoneyLog*)data);
				break;

			case HEADER_GD_GUILD_DEPOSIT_MONEY:
				GuildDepositMoney((TPacketGDGuildMoney*)data);
				break;

			case HEADER_GD_GUILD_WITHDRAW_MONEY:
				GuildWithdrawMoney(peer, (TPacketGDGuildMoney*)data);
				break;

			case HEADER_GD_GUILD_WITHDRAW_MONEY_GIVE_REPLY:
				GuildWithdrawMoneyGiveReply((TPacketGDGuildMoneyWithdrawGiveReply*)data);
				break;

			case HEADER_GD_GUILD_WAR_BET:
				GuildWarBet((TPacketGDGuildWarBet *) data);
				break;

			case HEADER_GD_SET_EVENT_FLAG:
				SetEventFlag((TPacketSetEventFlag*) data);
				break;

			case HEADER_GD_CREATE_OBJECT:
				CreateObject((TPacketGDCreateObject *) data);
				break;

			case HEADER_GD_DELETE_OBJECT:
				DeleteObject(*(DWORD *) data);
				break;

			case HEADER_GD_UPDATE_LAND:
				UpdateLand((DWORD *) data);
				break;

			case HEADER_GD_MARRIAGE_ADD:
				MarriageAdd((TPacketMarriageAdd *) data);
				break;

			case HEADER_GD_MARRIAGE_UPDATE:
				MarriageUpdate((TPacketMarriageUpdate *) data);
				break;

			case HEADER_GD_MARRIAGE_REMOVE:
				MarriageRemove((TPacketMarriageRemove *) data);
				break;

			case HEADER_GD_WEDDING_REQUEST:
				WeddingRequest((TPacketWeddingRequest *) data);
				break;

			case HEADER_GD_WEDDING_READY:
				WeddingReady((TPacketWeddingReady *) data);
				break;

			case HEADER_GD_WEDDING_END:
				WeddingEnd((TPacketWeddingEnd *) data);
				break;

				// BLOCK_CHAT
			case HEADER_GD_BLOCK_CHAT:
				BlockChat((TPacketBlockChat *) data);
				break;
				// END_OF_BLOCK_CHAT

			case HEADER_GD_MYSHOP_PRICELIST_UPDATE:
				MyshopPricelistUpdate((TPacketMyshopPricelistHeader*)data);
				break;

			case HEADER_GD_MYSHOP_PRICELIST_REQ:
				MyshopPricelistRequest(peer, dwHandle, *(DWORD*)data);
				break;

				//RELOAD_ADMIN
			case HEADER_GD_RELOAD_ADMIN:
				ReloadAdmin(peer, (TPacketReloadAdmin*)data);
				break;
				//END_RELOAD_ADMIN

			case HEADER_GD_BREAK_MARRIAGE:
				BreakMarriage(peer, data);
				break;

			case HEADER_GD_REQ_SPARE_ITEM_ID_RANGE:
				SendSpareItemIDRange(peer);
				break;

			case HEADER_GD_REQ_CHANGE_GUILD_MASTER:
				GuildChangeMaster((TPacketChangeGuildMaster*) data);
				break;

			case HEADER_GD_UPDATE_HORSE_NAME:
				UpdateHorseName((TPacketUpdateHorseName*) data, peer);
				break;

			case HEADER_GD_REQ_HORSE_NAME:
				AckHorseName(*(DWORD*)data, peer);
				break;

			case HEADER_GD_DC:
				DeleteLoginKey((TPacketDC*) data);
				break;

			case HEADER_GD_VALID_LOGOUT:
				ResetLastPlayerID((TPacketNeedLoginLogInfo*)data);
				break;

			case HEADER_GD_REQUEST_CHARGE_CASH:
				ChargeCash((TRequestChargeCash*)data);
				break;

			//delete gift notify icon

			case HEADER_GD_DELETE_AWARDID:
				DeleteAwardId((TPacketDeleteAwardID*) data);
				break;

			case HEADER_GD_UPDATE_CHANNELSTATUS:
				UpdateChannelStatus((SChannelStatus*) data);
				break;

			case HEADER_GD_REQUEST_CHANNELSTATUS:
				RequestChannelStatus(peer, dwHandle);
				break;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			case HEADER_GD_REQUEST_CHANGE_LANGUAGE:
				ChangeLanguage((TRequestChangeLanguage*)data);
				break;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
			case HEADER_GD_SKILL_COLOR_SAVE:
				QUERY_SKILL_COLOR_SAVE(data);
				break;
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
			case HEADER_GD_OFFLINESHOP:
				RecvPackets(data);
				break;
#endif

#ifdef ENABLE_OFFLINE_MESSAGE
			case HEADER_GD_REQUEST_OFFLINE_MESSAGES:
				RequestReadOfflineMessages(peer, dwHandle, (TPacketGDReadOfflineMessage*) data);
				break;

			case HEADER_GD_SEND_OFFLINE_MESSAGE:
				SendOfflineMessage((TPacketGDSendOfflineMessage*) data);
				break;
#endif

#ifdef ENABLE_ITEMSHOP
			case HEADER_GD_ITEMSHOP:
			{
				RecvItemShop(peer, dwHandle, data);
				break;
			}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
			case HEADER_GD_GROWTH_PET_SAVE:
				QUERY_GROWTH_PET_SAVE(peer, data);
				break;

			case HEADER_GD_GROWTH_PET_DELETE:
				QUERY_GROWTH_PET_DELETE(peer, data);
				break;
#endif

#ifdef ENABLE_RANKING
			case HEADER_GD_ADD_RANKING:
				QUERY_ADD_RANKING(peer, (TPacketGDAddRanking *) data, dwLength);
				break;
#endif

			default:
				sys_err("Unknown header (header: %d handle: %d length: %d)", header, dwHandle, dwLength);
				break;
		}
	}

	peer->RecvEnd(i);
}

void CClientManager::AddPeer(socket_t fd)
{
	CPeer * pPeer = new CPeer;

	if (pPeer->Accept(fd))
		m_peerList.push_front(pPeer);
	else
		delete pPeer;
}

void CClientManager::RemovePeer(CPeer * pPeer)
{
	if (m_pkAuthPeer == pPeer)
	{
		m_pkAuthPeer = NULL;
	}
	else
	{
		TLogonAccountMap::iterator it = m_map_kLogonAccount.begin();

		while (it != m_map_kLogonAccount.end())
		{
			CLoginData * pkLD = it->second;

			if (pkLD->GetConnectedPeerHandle() == pPeer->GetHandle())
			{
				if (pkLD->IsPlay())
				{
					pkLD->SetPlay(false);
				}

				if (pkLD->IsDeleted())
				{
					sys_log(0, "DELETING LoginData");
					delete pkLD;
				}

				m_map_kLogonAccount.erase(it++);
			}
			else
				++it;
		}
	}

	m_peerList.remove(pPeer);
	delete pPeer;
}

CPeer * CClientManager::GetPeer(IDENT ident)
{
	for (itertype(m_peerList) i = m_peerList.begin(); i != m_peerList.end();++i)
	{
		CPeer * tmp = *i;

		if (tmp->GetHandle() == ident)
			return tmp;
	}

	return NULL;
}

CPeer * CClientManager::GetAnyPeer()
{
	if (m_peerList.empty())
		return NULL;

	return m_peerList.front();
}

int CClientManager::AnalyzeQueryResult(SQLMsg * msg)
{
	CQueryInfo * qi = (CQueryInfo *) msg->pvUserData;
	CPeer * peer = GetPeer(qi->dwIdent);

#ifdef _TEST
	if (qi->iType != QID_ITEM_AWARD_LOAD)
	sys_log(0, "AnalyzeQueryResult %d", qi->iType);
#endif
	switch (qi->iType)
	{
		case QID_ITEM_AWARD_LOAD:
			ItemAwardManager::instance().Load(msg);
			delete qi;
			return true;

		case QID_GUILD_RANKING:
			CGuildManager::instance().ResultRanking(msg->Get()->pSQLResult);
			break;

		case QID_ITEMPRICE_LOAD_FOR_UPDATE:
			RESULT_PRICELIST_LOAD_FOR_UPDATE(msg);
			break;
	}

	if (!peer)
	{
		delete qi;
		return true;
	}

	switch (qi->iType)
	{
		case QID_PLAYER:
		case QID_ITEM:
		case QID_QUEST:
		case QID_AFFECT:
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		case QID_SKILL_COLOR:
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
		case QID_GROWTH_PET:
#endif
			RESULT_COMPOSITE_PLAYER(peer, msg, qi->iType);
			break;

		case QID_LOGIN:
			RESULT_LOGIN(peer, msg);
			break;

		case QID_SAFEBOX_LOAD:
			sys_log(0, "QUERY_RESULT: HEADER_GD_SAFEBOX_LOAD");
			RESULT_SAFEBOX_LOAD(peer, msg);
			break;

		case QID_SAFEBOX_CHANGE_SIZE:
			sys_log(0, "QUERY_RESULT: HEADER_GD_SAFEBOX_CHANGE_SIZE");
			RESULT_SAFEBOX_CHANGE_SIZE(peer, msg);
			break;

		case QID_SAFEBOX_CHANGE_PASSWORD:
			sys_log(0, "QUERY_RESULT: HEADER_GD_SAFEBOX_CHANGE_PASSWORD %p", msg);
			RESULT_SAFEBOX_CHANGE_PASSWORD(peer, msg);
			break;

		case QID_SAFEBOX_CHANGE_PASSWORD_SECOND:
			sys_log(0, "QUERY_RESULT: HEADER_GD_SAFEBOX_CHANGE_PASSWORD %p", msg);
			RESULT_SAFEBOX_CHANGE_PASSWORD_SECOND(peer, msg);
			break;

		case QID_HIGHSCORE_REGISTER:
			sys_log(0, "QUERY_RESULT: HEADER_GD_HIGHSCORE_REGISTER %p", msg);
			RESULT_HIGHSCORE_REGISTER(peer, msg);
			break;

		case QID_SAFEBOX_SAVE:
		case QID_ITEM_SAVE:
		case QID_ITEM_DESTROY:
		case QID_QUEST_SAVE:
		case QID_PLAYER_SAVE:
		case QID_ITEM_AWARD_TAKEN:
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		case QID_SKILL_COLOR_SAVE:
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
		case QID_GROWTH_PET_SAVE:
		case QID_GROWTH_PET_DELETE:
#endif
			break;

			// PLAYER_INDEX_CREATE_BUG_FIX
		case QID_PLAYER_INDEX_CREATE:
			RESULT_PLAYER_INDEX_CREATE(peer, msg);
			break;
			// END_PLAYER_INDEX_CREATE_BUG_FIX

		case QID_PLAYER_DELETE:
			__RESULT_PLAYER_DELETE(peer, msg);
			break;

		case QID_LOGIN_BY_KEY:
			RESULT_LOGIN_BY_KEY(peer, msg);
			break;

		case QID_ITEMPRICE_LOAD:
			RESULT_PRICELIST_LOAD(peer, msg);
			break;

		default:
			sys_log(0, "CClientManager::AnalyzeQueryResult unknown query result type: %d, str: %s", qi->iType, msg->stQuery.c_str());
			break;
	}

	delete qi;
	return true;
}

void UsageLog()
{
	FILE* fp = NULL;

	time_t		ct;
	char		*time_s;
	struct tm	lt;

	int			avg = g_dwUsageAvg / 3600;

	fp = fopen("usage.txt", "a+");

	if (!fp)
		return;

	ct = time(0);
	lt = *localtime(&ct);
	time_s = asctime(&lt);

	time_s[strlen(time_s) - 1] = '\0';

	fprintf(fp, "| %4d %-15.15s | %5d | %5u |", lt.tm_year + 1900, time_s + 4, avg, g_dwUsageMax);

	fprintf(fp, "\n");
	fclose(fp);

	g_dwUsageMax = g_dwUsageAvg = 0;
}

//#define ENABLE_ITEMAWARD_REFRESH
int CClientManager::Process()
{
	int pulses;

	if (!(pulses = thecore_idle()))
		return 0;

	while (pulses--)
	{
		++thecore_heart->pulse;

		if (!(thecore_heart->pulse % thecore_heart->passes_per_sec))
		{
			if (g_test_server)
			{
				if (!(thecore_heart->pulse % thecore_heart->passes_per_sec * 10))

				{
					pt_log("[%9d] return %d/%d/%d/%d async %d/%d/%d/%d",
							thecore_heart->pulse,
							CDBManager::instance().CountReturnQuery(SQL_PLAYER),
							CDBManager::instance().CountReturnResult(SQL_PLAYER),
							CDBManager::instance().CountReturnQueryFinished(SQL_PLAYER),
							CDBManager::instance().CountReturnCopiedQuery(SQL_PLAYER),
							CDBManager::instance().CountAsyncQuery(SQL_PLAYER),
							CDBManager::instance().CountAsyncResult(SQL_PLAYER),
							CDBManager::instance().CountAsyncQueryFinished(SQL_PLAYER),
							CDBManager::instance().CountAsyncCopiedQuery(SQL_PLAYER));

					if ((thecore_heart->pulse % 50) == 0)
						sys_log(0, "[%9d] return %d/%d/%d async %d/%d/%d",
								thecore_heart->pulse,
								CDBManager::instance().CountReturnQuery(SQL_PLAYER),
								CDBManager::instance().CountReturnResult(SQL_PLAYER),
								CDBManager::instance().CountReturnQueryFinished(SQL_PLAYER),
								CDBManager::instance().CountAsyncQuery(SQL_PLAYER),
								CDBManager::instance().CountAsyncResult(SQL_PLAYER),
								CDBManager::instance().CountAsyncQueryFinished(SQL_PLAYER));
				}
			}
			else
			{
				pt_log("[%9d] return %d/%d/%d/%d async %d/%d/%d%/%d",
						thecore_heart->pulse,
						CDBManager::instance().CountReturnQuery(SQL_PLAYER),
						CDBManager::instance().CountReturnResult(SQL_PLAYER),
						CDBManager::instance().CountReturnQueryFinished(SQL_PLAYER),
						CDBManager::instance().CountReturnCopiedQuery(SQL_PLAYER),
						CDBManager::instance().CountAsyncQuery(SQL_PLAYER),
						CDBManager::instance().CountAsyncResult(SQL_PLAYER),
						CDBManager::instance().CountAsyncQueryFinished(SQL_PLAYER),
						CDBManager::instance().CountAsyncCopiedQuery(SQL_PLAYER));

						if ((thecore_heart->pulse % 50) == 0)
						sys_log(0, "[%9d] return %d/%d/%d async %d/%d/%d",
							thecore_heart->pulse,
							CDBManager::instance().CountReturnQuery(SQL_PLAYER),
							CDBManager::instance().CountReturnResult(SQL_PLAYER),
							CDBManager::instance().CountReturnQueryFinished(SQL_PLAYER),
							CDBManager::instance().CountAsyncQuery(SQL_PLAYER),
							CDBManager::instance().CountAsyncResult(SQL_PLAYER),
							CDBManager::instance().CountAsyncQueryFinished(SQL_PLAYER));
						}

			CDBManager::instance().ResetCounter();

			DWORD dwCount = CClientManager::instance().GetUserCount();

			g_dwUsageAvg += dwCount;
			g_dwUsageMax = MAX(g_dwUsageMax, dwCount);

			memset(&thecore_profiler[0], 0, sizeof(thecore_profiler));

			if (!(thecore_heart->pulse % (thecore_heart->passes_per_sec * 3600)))
				UsageLog();

			m_iCacheFlushCount = 0;

#ifdef ENABLE_SKILL_COLOR_SYSTEM
			UpdateSkillColorCache();
#endif
			UpdatePlayerCache();
			UpdateItemCache();
			UpdateLogoutPlayer();
			UpdateItemPriceListCache();
#ifdef ENABLE_GROWTH_PET_SYSTEM
			UpdateGrowthPetCache();
#endif

			CGuildManager::instance().Update();
			CPrivManager::instance().Update();
			marriage::CManager::instance().Update();
		}

		if (!(thecore_heart->pulse % (thecore_heart->passes_per_sec * 5)))
		{
			ItemAwardManager::instance().RequestLoad();
		}

		if (!(thecore_heart->pulse % (thecore_heart->passes_per_sec * 10)))
		{
			pt_log("QUERY: MAIN[%d] ASYNC[%d]", g_query_count[0], g_query_count[1]);
			g_query_count[0] = 0;
			g_query_count[1] = 0;

			pt_log("ITEM:%d\n", g_item_count);
			g_item_count = 0;
		}

		if (!(thecore_heart->pulse % (thecore_heart->passes_per_sec * 60)))
		{
			CClientManager::instance().SendTime();
		}

		if (!(thecore_heart->pulse % (thecore_heart->passes_per_sec * 3600)))
		{
			CMoneyLog::instance().Save();
		}

#ifdef ENABLE_OFFLINE_MESSAGE
		if (!(thecore_heart->pulse % (thecore_heart->passes_per_sec * 3600)))
		{
			CClientManager::instance().OfflineMessageGarbage();
		}
#endif

	}

	int num_events = fdwatch(m_fdWatcher, 0);
	int idx;
	CPeer * peer;

	for (idx = 0; idx < num_events; ++idx)
	{
		peer = (CPeer *) fdwatch_get_client_data(m_fdWatcher, idx);

		if (!peer)
		{
			if (fdwatch_check_event(m_fdWatcher, m_fdAccept, idx) == FDW_READ)
			{
				AddPeer(m_fdAccept);
				fdwatch_clear_event(m_fdWatcher, m_fdAccept, idx);
			}
			else
			{
				sys_log(0, "FDWATCH: peer null in event: ident %d", fdwatch_get_ident(m_fdWatcher, idx)); // @warme012
			}

			continue;
		}

		switch (fdwatch_check_event(m_fdWatcher, peer->GetFd(), idx))
		{
			case FDW_READ:
				if (peer->Recv() < 0)
				{
					sys_err("Recv failed");
					RemovePeer(peer);
				}
				else
				{
					if (peer == m_pkAuthPeer)
						if (g_log)
							sys_log(0, "AUTH_PEER_READ: size %d", peer->GetRecvLength());

					ProcessPackets(peer);
				}
				break;

			case FDW_WRITE:
				if (peer == m_pkAuthPeer)
					if (g_log)
						sys_log(0, "AUTH_PEER_WRITE: size %d", peer->GetSendLength());

				if (peer->Send() < 0)
				{
					sys_err("Send failed");
					RemovePeer(peer);
				}

				break;

			case FDW_EOF:
				RemovePeer(peer);
				break;

			default:
				sys_err("fdwatch_check_fd returned unknown result");
				RemovePeer(peer);
				break;
		}
	}
	return 1;
}

DWORD CClientManager::GetUserCount()
{
	return m_map_kLogonAccount.size();
}

void CClientManager::SendAllGuildSkillRechargePacket()
{
	ForwardPacket(HEADER_DG_GUILD_SKILL_RECHARGE, NULL, 0);
}

void CClientManager::SendTime()
{
	time_t now = GetCurrentTime();
	ForwardPacket(HEADER_DG_TIME, &now, sizeof(time_t));
}

void CClientManager::ForwardPacket(BYTE header, const void* data, int size, BYTE bChannel, CPeer* except)
{
	for (itertype(m_peerList) it = m_peerList.begin(); it != m_peerList.end(); ++it)
	{
		CPeer * peer = *it;

		if (peer == except)
			continue;

		if (!peer->GetChannel())
			continue;

		if (bChannel && peer->GetChannel() != bChannel)
			continue;

		peer->EncodeHeader(header, 0, size);

		if (size > 0 && data)
			peer->Encode(data, size);
	}
}

void CClientManager::SendNotice(const char * c_pszFormat, ...)
{
#ifdef ENABLE_EMOTICONS_SYSTEM
	char szBuf[1024];
#else
	char szBuf[256];
#endif

	va_list args;

	va_start(args, c_pszFormat);
	int len = vsnprintf(szBuf, sizeof(szBuf), c_pszFormat, args);
	va_end(args);
	szBuf[len] = '\0';

	ForwardPacket(HEADER_DG_NOTICE, szBuf, len + 1);
}

time_t CClientManager::GetCurrentTime()
{
	return time(0);
}

// ITEM_UNIQUE_ID
bool CClientManager::InitializeNowItemID()
{
	DWORD dwMin, dwMax;

	if (!CConfig::instance().GetTwoValue("ITEM_ID_RANGE", &dwMin, &dwMax))
	{
		sys_err("conf.txt: Cannot find ITEM_ID_RANGE [start_item_id] [end_item_id]");
		return false;
	}

	sys_log(0, "ItemRange From File %u ~ %u ", dwMin, dwMax);

	if (CItemIDRangeManager::instance().BuildRange(dwMin, dwMax, m_itemRange) == false)
	{
		sys_err("Can not build ITEM_ID_RANGE");
		return false;
	}

	sys_log(0, " Init Success Start %u End %u Now %u\n", m_itemRange.dwMin, m_itemRange.dwMax, m_itemRange.dwUsableItemIDMin);

	return true;
}

DWORD CClientManager::GainItemID()
{
	return m_itemRange.dwUsableItemIDMin++;
}

DWORD CClientManager::GetItemID()
{
	return m_itemRange.dwUsableItemIDMin;
}
// ITEM_UNIQUE_ID_END
//BOOT_LOCALIZATION

bool CClientManager::InitializeLocalization()
{
	char szQuery[512];
	snprintf(szQuery, sizeof(szQuery), "SELECT mValue, mKey FROM locale");
	SQLMsg * pMsg = CDBManager::instance().DirectQuery(szQuery, SQL_COMMON);

	if (pMsg->Get()->uiNumRows == 0)
	{
		sys_err("InitializeLocalization() ==> DirectQuery failed(%s)", szQuery);
		delete pMsg;
		return false;
	}

	sys_log(0, "InitializeLocalization() - LoadLocaleTable(count:%d)", pMsg->Get()->uiNumRows);

	m_vec_Locale.clear();

	MYSQL_ROW row = NULL;

	for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
	{
		int col = 0;
		tLocale locale;

		strlcpy(locale.szValue, row[col++], sizeof(locale.szValue));
		strlcpy(locale.szKey, row[col++], sizeof(locale.szKey));

		g_stLocale = "utf8mb4";
		g_stLocaleNameColumn = "locale_name";
		CDBManager::instance().SetLocale (g_stLocale.c_str());
		m_vec_Locale.push_back (locale);
	}

	delete pMsg;

	return true;
}
//END_BOOT_LOCALIZATION
//ADMIN_MANAGER

bool CClientManager::__GetAdminInfo(const char *szIP, std::vector<tAdminInfo> & rAdminVec)
{
	char szQuery[512];
	snprintf(szQuery, sizeof(szQuery),
			"SELECT mID,mAccount,mName,mContactIP,mServerIP,mAuthority FROM gmlist WHERE mServerIP='ALL' or mServerIP='%s'",
		   	szIP ? szIP : "ALL");

	SQLMsg * pMsg = CDBManager::instance().DirectQuery(szQuery, SQL_COMMON);

	if (pMsg->Get()->uiNumRows == 0)
	{
// empty table
		sys_err("__GetAdminInfo() ==> DirectQuery failed(%s)", szQuery);
		delete pMsg;
		return false;
	}

	MYSQL_ROW row;
	rAdminVec.reserve(pMsg->Get()->uiNumRows);

	while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		int idx = 0;
		tAdminInfo Info;

		str_to_number(Info.m_ID, row[idx++]);
		trim_and_lower(row[idx++], Info.m_szAccount, sizeof(Info.m_szAccount));
		strlcpy(Info.m_szName, row[idx++], sizeof(Info.m_szName));
		strlcpy(Info.m_szContactIP, row[idx++], sizeof(Info.m_szContactIP));
		strlcpy(Info.m_szServerIP, row[idx++], sizeof(Info.m_szServerIP));
		std::string stAuth = row[idx++];

		if (!stAuth.compare("IMPLEMENTOR"))
			Info.m_Authority = GM_IMPLEMENTOR;
		else if (!stAuth.compare("GOD"))
			Info.m_Authority = GM_GOD;
		else if (!stAuth.compare("HIGH_WIZARD"))
			Info.m_Authority = GM_HIGH_WIZARD;
		else if (!stAuth.compare("LOW_WIZARD"))
			Info.m_Authority = GM_LOW_WIZARD;
		else if (!stAuth.compare("WIZARD"))
			Info.m_Authority = GM_WIZARD;
		else
			continue;

		rAdminVec.push_back(Info);

		sys_log(0, "GM: PID %u Login %s Character %s ContactIP %s ServerIP %s Authority %d[%s]",
			   	Info.m_ID, Info.m_szAccount, Info.m_szName, Info.m_szContactIP, Info.m_szServerIP, Info.m_Authority, stAuth.c_str());
	}

	delete pMsg;

	return true;
}

bool CClientManager::__GetHostInfo(std::vector<std::string> & rIPVec)
{
	char szQuery[512];
	snprintf(szQuery, sizeof(szQuery), "SELECT mIP FROM gmhost");
	SQLMsg * pMsg = CDBManager::instance().DirectQuery(szQuery, SQL_COMMON);

	if (pMsg->Get()->uiNumRows == 0)
	{
		sys_err("__GetHostInfo() ==> DirectQuery failed(%s)", szQuery);
		delete pMsg;
		return false;
	}

	rIPVec.reserve(pMsg->Get()->uiNumRows);

	MYSQL_ROW row;

	while ((row = mysql_fetch_row(pMsg->Get()->pSQLResult)))
	{
		if (row[0] && *row[0])
		{
			rIPVec.push_back(row[0]);
			sys_log(0, "GMHOST: %s", row[0]);
		}
	}

	delete pMsg;
	return true;
}
//END_ADMIN_MANAGER

void CClientManager::ReloadAdmin(CPeer*, TPacketReloadAdmin* p)
{
	std::vector<tAdminInfo> vAdmin;
	std::vector<std::string> vHost;

	__GetHostInfo(vHost);
	__GetAdminInfo(p->szIP, vAdmin);

	DWORD dwPacketSize = sizeof(WORD) + sizeof (WORD) + sizeof(tAdminInfo) * vAdmin.size() +
		  sizeof(WORD) + sizeof(WORD) + 16 * vHost.size();

	for (itertype(m_peerList) it = m_peerList.begin(); it != m_peerList.end(); ++it)
	{
		CPeer * peer = *it;

		if (!peer->GetChannel())
			continue;

		peer->EncodeHeader(HEADER_DG_RELOAD_ADMIN, 0, dwPacketSize);

		peer->EncodeWORD(16);
		peer->EncodeWORD(vHost.size());

		for (size_t n = 0; n < vHost.size(); ++n)
			peer->Encode(vHost[n].c_str(), 16);

		peer->EncodeWORD(sizeof(tAdminInfo));
		peer->EncodeWORD(vAdmin.size());

		for (size_t n = 0; n < vAdmin.size(); ++n)
			peer->Encode(&vAdmin[n], sizeof(tAdminInfo));
	}

	sys_log(0, "ReloadAdmin End %s", p->szIP);
}

//BREAK_MARRIAGE
void CClientManager::BreakMarriage(CPeer * peer, const char * data)
{
	DWORD pid1, pid2;

	pid1 = *(int *) data;
	data += sizeof(int);

	pid2 = *(int *) data;
	data += sizeof(int);

	sys_log(0, "Breaking off a marriage engagement! pid %d and pid %d", pid1, pid2);
	marriage::CManager::instance().Remove(pid1, pid2);
}
//END_BREAK_MARIIAGE

void CClientManager::UpdateItemCacheSet(DWORD pid)
{
	itertype(m_map_pkItemCacheSetPtr) it = m_map_pkItemCacheSetPtr.find(pid);

	if (it == m_map_pkItemCacheSetPtr.end())
	{
		if (g_test_server)
			sys_log(0, "UPDATE_ITEMCACHESET : UpdateItemCacheSet ==> No ItemCacheSet pid(%d)", pid);
		return;
	}

	TItemCacheSet * pSet = it->second;
	TItemCacheSet::iterator it_set = pSet->begin();

	while (it_set != pSet->end())
	{
		CItemCache * c = *it_set++;
		c->Flush();
	}

	if (g_log)
		sys_log(0, "UPDATE_ITEMCACHESET : UpdateItemCachsSet pid(%d)", pid);
}

void CClientManager::SendSpareItemIDRange(CPeer* peer)
{
	peer->SendSpareItemIDRange();
}

void CClientManager::DeleteLoginKey(TPacketDC *data)
{
	char login[LOGIN_MAX_LEN+1] = {0};
	trim_and_lower(data->login, login, sizeof(login));

	CLoginData *pkLD = GetLoginDataByLogin(login);

	if (pkLD)
	{
		TLoginDataByLoginKey::iterator it = m_map_pkLoginData.find(pkLD->GetKey());

		if (it != m_map_pkLoginData.end())
			m_map_pkLoginData.erase(it);
	}
}

// delete gift notify icon
void CClientManager::DeleteAwardId(TPacketDeleteAwardID *data)
{
	std::map<DWORD, TItemAward *>::iterator it;
	it = ItemAwardManager::Instance().GetMapAward().find(data->dwID);

	if ( it != ItemAwardManager::Instance().GetMapAward().end() )
	{
		std::set<TItemAward *> & kSet = ItemAwardManager::Instance().GetMapkSetAwardByLogin()[it->second->szLogin];
		if(kSet.erase(it->second))
			sys_log(0,"erase ItemAward id: %d from cache", data->dwID);
		ItemAwardManager::Instance().GetMapAward().erase(data->dwID);
	}
	else
	{
		sys_log(0,"DELETE_AWARDID : could not find the id: %d", data->dwID);
	}

}

void CClientManager::UpdateChannelStatus(TChannelStatus* pData)
{
	TChannelStatusMap::iterator it = m_mChannelStatus.find(pData->nPort);
	if (it != m_mChannelStatus.end())
	{
		it->second = pData->bStatus;
	}
	else
	{
		m_mChannelStatus.insert(TChannelStatusMap::value_type(pData->nPort, pData->bStatus));
	}
}

void CClientManager::RequestChannelStatus(CPeer* peer, DWORD dwHandle)
{
	const int nSize = m_mChannelStatus.size();
	peer->EncodeHeader(HEADER_DG_RESPOND_CHANNELSTATUS, dwHandle, sizeof(TChannelStatus)*nSize+sizeof(int));
	peer->Encode(&nSize, sizeof(int));

	for (TChannelStatusMap::iterator it = m_mChannelStatus.begin(); it != m_mChannelStatus.end(); it++)
	{
		peer->Encode(&it->first, sizeof(int));
		peer->Encode(&it->second, sizeof(BYTE));
	}
}

void CClientManager::ResetLastPlayerID(const TPacketNeedLoginLogInfo* data)
{
	CLoginData* pkLD = GetLoginDataByAID( data->dwPlayerID );

	if (NULL != pkLD)
	{
		pkLD->SetLastPlayerID( 0 );
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		pkLD->GetAccountRef().bLanguage = data->bLanguage;
#endif
	}
}

void CClientManager::ChargeCash(const TRequestChargeCash* packet)
{
	char szQuery[512];

	if (ERequestCharge_Cash == packet->eChargeType)
		snprintf(szQuery, sizeof(szQuery), "update account set `cash` = `cash` + %d where id = %d limit 1", packet->dwAmount, packet->dwAID);
	else if(ERequestCharge_Mileage == packet->eChargeType)
		snprintf(szQuery, sizeof(szQuery), "update account set `mileage` = `mileage` + %d where id = %d limit 1", packet->dwAmount, packet->dwAID);
	else
	{
		sys_err ("Invalid request charge type (type : %d, amount : %d, aid : %d)", packet->eChargeType, packet->dwAmount, packet->dwAID);
		return;
	}

	sys_err ("Request Charge (type : %d, amount : %d, aid : %d)", packet->eChargeType, packet->dwAmount, packet->dwAID);

	CDBManager::Instance().AsyncQuery(szQuery, SQL_ACCOUNT);
}

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
void CClientManager::ChangeLanguage(const TRequestChangeLanguage* packet)
{
	char szQuery[512];

	if (packet->bLanguage > LOCALE_EUROPE && packet->bLanguage < LOCALE_MAX_NUM)
	{
		snprintf(szQuery, sizeof(szQuery), "update account set `language` = %d where id = %d", packet->bLanguage, packet->dwAID);
	}
	else
	{
		sys_err("Invalid request change language (language : %d, aid : %d)", packet->bLanguage, packet->dwAID);
		return;
	}

	CDBManager::Instance().AsyncQuery(szQuery, SQL_ACCOUNT);
}
#endif

#ifdef ENABLE_OFFLINE_MESSAGE
void CClientManager::RequestReadOfflineMessages(CPeer* pkPeer, DWORD dwHandle, TPacketGDReadOfflineMessage* p)
{
	auto it = m_OfflineMessage.find(p->szName);
	if (it == m_OfflineMessage.end())
		return;

	const auto& vec = it->second;
	TPacketDGReadOfflineMessage r;
	for (const auto& msg : vec)
	{
		strlcpy(r.szFrom, msg->From.c_str(), sizeof(r.szFrom));
		strlcpy(r.szMessage, msg->Message.c_str(), sizeof(r.szMessage));
		pkPeer->EncodeHeader(HEADER_DG_RESPOND_OFFLINE_MESSAGES, dwHandle, sizeof(r));
		pkPeer->Encode(&r, sizeof(r));
	}

	m_OfflineMessage.erase(it);
}

void CClientManager::SendOfflineMessage(TPacketGDSendOfflineMessage* p)
{
	auto msg = std::make_shared<SOfflineMessage>(p->szFrom, p->szMessage);
	m_OfflineMessage[p->szTo].emplace_back(std::move(msg));
}

void CClientManager::OfflineMessageGarbage()
{
	if (m_OfflineMessage.empty())
		return;

	static const int iHour = 5;
	const auto tCurrent = std::chrono::system_clock::now();

	for (auto& mOff : m_OfflineMessage)
	{
		auto& vMessage = mOff.second;
		for (auto iter = vMessage.begin(); iter != vMessage.end();)
		{
			if (std::chrono::duration_cast<std::chrono::hours>(tCurrent - iter->get()->t).count() >= iHour)
				iter = vMessage.erase(iter);
			else
				++iter;
		}
	}
}
#endif

#ifdef ENABLE_ITEMSHOP
void stringToRealTime(struct tm& t, const std::string& strDateTime)
{
	int year = 0, month = 0, day = 0, hour = 0, min = 0, sec = 0;

	if (sscanf(strDateTime.c_str(), "%4d-%2d-%2d %2d:%2d:%2d", &year, &month, &day, &hour, &min, &sec) == 6)
	{
		t.tm_year = year - 1900;
		t.tm_mon = month - 1;
		t.tm_mday = day;
		t.tm_isdst = 0;
		t.tm_hour = hour;
		t.tm_min = min;
		t.tm_sec = sec;
	}
}

bool sortItemShop(const TIShopData& first, const TIShopData& second)
{
	return first.sellCount > second.sellCount;
}

bool CClientManager::InitializeItemShop()
{
	m_IShopManager.clear();

	char szQuery[1024 + 1] = {};
	snprintf(szQuery, sizeof(szQuery), "SELECT `id`, `categoryType`, `categorySubType`, `itemVnum`, `priceType`+0, `itemPrice`, `discount`, `offerTime`, `addedTime`, `sellCount`, `week_limit`, `month_limit`, `maxSellCount` FROM `ishop_data`;");

	std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));

	if (pMsg->Get() && pMsg->Get()->uiNumRows != 0)
	{
		std::vector<TIShopData> m_sortCache;

#ifdef USE_ITEMSHOP_RENEWED
		uint8_t bPriceType = ITEMSHOP_PRICE_MAX;
#endif

		MYSQL_ROW row = NULL;
		for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
		{
			int col = 0;
			TIShopData ishopData;

			BYTE categoryType, categorySubType;
			str_to_number(ishopData.id, row[col++]);
			str_to_number(categoryType, row[col++]);
			str_to_number(categorySubType, row[col++]);
			str_to_number(ishopData.itemVnum, row[col++]);

#ifdef USE_ITEMSHOP_RENEWED
			str_to_number(bPriceType, row[col++]);

			if (bPriceType == ITEMSHOP_PRICE_COIN)
			{
				ishopData.itemPriceJD = 0;
				str_to_number(ishopData.itemPrice, row[col++]);
			}
			else if (bPriceType == ITEMSHOP_PRICE_JETON)
			{
				ishopData.itemPrice = 0;
				str_to_number(ishopData.itemPriceJD, row[col++]);
			}
			else
			{
				sys_err("Unknown bPriceType: %u.", bPriceType);
				continue;
			}
#else
			str_to_number(ishopData.itemPrice, row[col++]);
#endif

			str_to_number(ishopData.discount, row[col++]);

			char eventTime[128 + 1] = {};
			strlcpy(eventTime, row[col++], sizeof(eventTime));

			struct tm offerTimeTm;
			stringToRealTime(offerTimeTm, eventTime);
			time_t offerTime = mktime(&offerTimeTm);

			strlcpy(eventTime, row[col++], sizeof(eventTime));

			struct tm addedTimeTm;
			stringToRealTime(addedTimeTm, eventTime);
			time_t addedTime = mktime(&addedTimeTm);

			ishopData.offerTime = offerTime;
			if (ishopData.offerTime < 0)
			{
				ishopData.offerTime = 0;
			}

			ishopData.addedTime = addedTime;
			if (ishopData.addedTime < 0)
			{
				ishopData.addedTime = 0;
			}

			str_to_number(ishopData.sellCount, row[col++]);
			str_to_number(ishopData.week_limit, row[col++]);
			str_to_number(ishopData.month_limit, row[col++]);
			str_to_number(ishopData.maxSellCount, row[col]);

			ishopData.topSellingIndex = -1;
			if (ishopData.sellCount > 0)
			{
				m_sortCache.emplace_back(ishopData);
			}

			auto itType = m_IShopManager.find(categoryType);
			if (itType != m_IShopManager.end())
			{
				auto itSubType = itType->second.find(categorySubType);
				if (itSubType != itType->second.end())
				{
					itSubType->second.emplace_back(ishopData);
				}
				else
				{
					std::vector<TIShopData> m_vec;
					m_vec.emplace_back(ishopData);

					itType->second.emplace(categorySubType, m_vec);
				}
			}
			else
			{
				std::vector<TIShopData> m_vec;
				m_vec.emplace_back(ishopData);

				std::map<BYTE, std::vector<TIShopData>> m_map;
				m_map.emplace(categorySubType, m_vec);

				m_IShopManager.emplace(categoryType, m_map);
			}
		}

		if (m_sortCache.size())
		{
			std::stable_sort(m_sortCache.begin(), m_sortCache.end(), sortItemShop);
			for (DWORD j = 0; j < m_sortCache.size(); ++j)
			{
				DWORD id = m_sortCache[j].id;
				for (auto itCategory = m_IShopManager.begin(); itCategory != m_IShopManager.end(); ++itCategory)
				{
					if (itCategory->second.size())
					{
						for (auto itSubCategory = itCategory->second.begin(); itSubCategory != itCategory->second.end(); ++itSubCategory)
						{
							for (DWORD x = 0; x < itSubCategory->second.size(); ++x)
							{
								if (itSubCategory->second[x].id == id)
								{
									itSubCategory->second[x].topSellingIndex = j + 1;
								}
							}
						}
					}
				}
			}
		}
	}

	itemShopUpdateTime = time(0);
	return true;
}

void CClientManager::ItemShopIncreaseSellCount(DWORD itemID, int itemCount)
{
	long long sellCount = 0;

	char szQuery[128 + 1] = {};
	snprintf(szQuery, sizeof(szQuery), "SELECT `sellCount` FROM `ishop_data` WHERE `id` = %u;", itemID);

	std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));

	if (pMsg->Get() && pMsg->Get()->uiNumRows > 0)
	{
		MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
		str_to_number(sellCount, row[0]);
	}

	sellCount += 1;
	snprintf(szQuery, sizeof(szQuery), "UPDATE `ishop_data` SET `sellCount` = %lld WHERE `id` = %u;", sellCount, itemID);

	std::unique_ptr<SQLMsg> pMsgLast(CDBManager::instance().DirectQuery(szQuery));
}

#ifdef USE_ITEMSHOP_RENEWED
void CClientManager::SetAccountMoney(DWORD id, long long coins, long long jcoins)
#else
void CClientManager::SetDragonCoin(DWORD id, long long amount)
#endif
{
	if (id == 0)
	{
		return;
	}

	char szQuery[512 + 1] = {};
	snprintf(szQuery, sizeof(szQuery), "UPDATE `account` SET `coins` = %lld"
#ifdef USE_ITEMSHOP_RENEWED
										", `jcoins` = %lld"
#endif
										" WHERE `id` = %u;",
#ifdef USE_ITEMSHOP_RENEWED
										coins,
										jcoins,
#else
										amount,
#endif
										id);

	std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery, SQL_ACCOUNT));
}

#ifdef USE_ITEMSHOP_RENEWED
void CClientManager::GetAccountMoney(DWORD id, long long& coins, long long& jcoins)
#else
long long CClientManager::GetDragonCoin(DWORD id)
#endif
{
	coins = 0;
	jcoins = 0;

	if (id == 0)
	{
#ifdef USE_ITEMSHOP_RENEWED
		return;
#else
		return 0;
#endif
	}

	char szQuery[512 + 1] = {};
	snprintf(szQuery, sizeof(szQuery), "SELECT `coins`"
#ifdef USE_ITEMSHOP_RENEWED
										", `jcoins`"
#endif
										" FROM `account` WHERE id = %u;", id);

	std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery, SQL_ACCOUNT));

	if (!pMsg->Get() || pMsg->Get()->uiNumRows == 0)
	{
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

	str_to_number(coins, row[0]);
	str_to_number(jcoins, row[1]);
}

void CClientManager::RecvItemShop(CPeer* pkPeer, DWORD dwHandle, const char* data)
{
	const BYTE subIndex = *(BYTE*)data;
	data += sizeof(BYTE);

	if (subIndex == ITEMSHOP_LOG)
	{
		if (!pkPeer)
		{
			return;
		}

		const DWORD accountID = *(DWORD*)data;
		data += sizeof(DWORD);

		auto it = m_IShopLogManager.find(accountID);
		if (it == m_IShopLogManager.end())
		{
			char szQuery[128 + 1] = {};
			snprintf(szQuery, sizeof(szQuery), "SELECT * FROM `ishop_log` WHERE `accountID` = %u;", accountID);

			std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));

			if (pMsg->Get() && pMsg->Get()->uiNumRows != 0)
			{
				std::vector<TIShopLogData> m_vec;

				MYSQL_ROW row = NULL;
				for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
				{
					int col = 0;
					TIShopLogData ishopData;
					str_to_number(ishopData.accountID, row[col++]);
					strlcpy(ishopData.playerName, row[col++], sizeof(ishopData.playerName));
					strlcpy(ishopData.buyDate, row[col++], sizeof(ishopData.buyDate));
					str_to_number(ishopData.buyTime, row[col++]);
					strlcpy(ishopData.ipAdress, row[col++], sizeof(ishopData.ipAdress));
					str_to_number(ishopData.itemID, row[col++]);
					str_to_number(ishopData.itemVnum, row[col++]);
					str_to_number(ishopData.itemCount, row[col++]);
					str_to_number(ishopData.itemPrice, row[col++]);
#ifdef USE_ITEMSHOP_RENEWED
					str_to_number(ishopData.itemPriceJD, row[col]);
#endif

					m_vec.emplace_back(ishopData);
				}

				if (!m_vec.empty())
				{
					m_IShopLogManager.emplace(accountID, m_vec);
				}
			}
		}

		it = m_IShopLogManager.find(accountID);
		if (it == m_IShopLogManager.end())
		{
			int logCount = 0;

			pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(int));
			pkPeer->Encode(&subIndex, sizeof(BYTE));
			pkPeer->Encode(&logCount, sizeof(int));
		}
		else
		{
			int logCount = it->second.size();

			pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(int) + (sizeof(TIShopLogData) * logCount));
			pkPeer->Encode(&subIndex, sizeof(BYTE));
			pkPeer->Encode(&logCount, sizeof(int));

			if (logCount)
			{
				pkPeer->Encode(&it->second[0], sizeof(TIShopLogData) * logCount);
			}
		}
	}
	else if (subIndex == ITEMSHOP_RELOAD)
	{
		InitializeItemShop();
		SendItemShopData(NULL, true);
	}
	else if (subIndex == ITEMSHOP_BUY)
	{
		const DWORD accountID = *(DWORD*)data;
		data += sizeof(DWORD);

		char playerName[CHARACTER_NAME_MAX_LEN + 1];
		std::memcpy(&playerName, data, sizeof(playerName));
		data += sizeof(playerName);

		char ipAdress[16] = {};
		std::memcpy(&ipAdress, data, sizeof(ipAdress));
		data += sizeof(ipAdress);

		const int itemID = *(int*)data;
		data += sizeof(int);

		const int itemCount = *(int*)data;
		data += sizeof(int);

		if (itemCount < 1 || itemCount > 20)
		{
			return;
		}

		const bool isLogOpen = *(bool*)data;
		data += sizeof(bool);

		if (!m_IShopManager.empty())
		{
			for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
			{
				if (!it->second.empty())
				{
					for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
					{
						if (!itEx->second.empty())
						{
							for (auto itReal = itEx->second.begin(); itReal != itEx->second.end(); ++itReal)
							{
								TIShopData& itemData = *itReal;
								if (itemData.id == itemID)
								{
#ifdef USE_ITEMSHOP_RENEWED
									long long coins;
									long long jcoins;

									GetAccountMoney(accountID, coins, jcoins);
#else
									long long coins = GetDragonCoin(accountID);
#endif

									long long itemPrice = itemData.itemPrice * itemCount;
#ifdef USE_ITEMSHOP_RENEWED
									long long itemPriceJD = itemData.itemPriceJD * itemCount;
#endif

									if (itemData.discount > 0)
									{
										itemPrice = static_cast<long long>((float(itemPrice) / 100.0) * float(100 - itemData.discount));
#ifdef USE_ITEMSHOP_RENEWED
										itemPriceJD = static_cast<long long>((float(itemPriceJD) / 100.0) * float(100 - itemData.discount));
#endif
									}

									bool needUpdatePacket = false;

									if (itemData.maxSellCount != -1)
									{
										if (itemData.maxSellCount == 0)
										{
											BYTE returnType = 4;

											pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(BYTE));
											pkPeer->Encode(&subIndex, sizeof(BYTE));
											pkPeer->Encode(&returnType, sizeof(BYTE));
											return;
										}

										needUpdatePacket = true;
										itemData.maxSellCount -= 1;
									}

									if (itemPrice < 0)
									{
										BYTE returnType = 5;

										pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(BYTE));
										pkPeer->Encode(&subIndex, sizeof(BYTE));
										pkPeer->Encode(&returnType, sizeof(BYTE));
										return;
									}
#ifdef USE_ITEMSHOP_RENEWED
									else if (itemPriceJD < 0)
									{
										BYTE returnType = 6;

										pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(BYTE));
										pkPeer->Encode(&subIndex, sizeof(BYTE));
										pkPeer->Encode(&returnType, sizeof(BYTE));
										return;
									}
#endif
									else if (itemPrice > coins)
									{
										BYTE returnType = 0;

										pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(BYTE));
										pkPeer->Encode(&subIndex, sizeof(BYTE));
										pkPeer->Encode(&returnType, sizeof(BYTE));
										return;
									}
#ifdef USE_ITEMSHOP_RENEWED
									else if (itemPriceJD > jcoins)
									{
										BYTE returnType = 7;

										pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(BYTE));
										pkPeer->Encode(&subIndex, sizeof(BYTE));
										pkPeer->Encode(&returnType, sizeof(BYTE));
										return;
									}
#endif

									if (itemData.week_limit > 0)
									{
										DWORD weekCount = 0;

										char szQuery[512] = {};
										snprintf(szQuery, sizeof(szQuery), "SELECT `itemCount` FROM ishop_log WHERE `itemID` = %d AND `buyDate` > DATE_SUB(NOW(), INTERVAL 1 WEEK) AND `accountID` = %u;", itemID, accountID);

										std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery));

										if (pMsg->Get() && pMsg->Get()->uiNumRows != 0)
										{
											MYSQL_ROW row = NULL;

											for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
											{
												int buyCount;
												str_to_number(buyCount, row[0]);

												weekCount += buyCount;
											}
										}

										if (weekCount >= itemData.week_limit || weekCount + itemCount > itemData.week_limit)
										{
											BYTE returnType = 1;

											pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(BYTE) + sizeof(int));
											pkPeer->Encode(&subIndex, sizeof(BYTE));
											pkPeer->Encode(&returnType, sizeof(BYTE));
											pkPeer->Encode(&itemData.week_limit, sizeof(int));
											return;
										}
									}

									if (itemData.month_limit > 0)
									{
										DWORD monthCount = 0;

										char szQuery[254 + 1] = {};
										snprintf(szQuery, sizeof(szQuery), "SELECT `itemCount` FROM `ishop_log` WHERE `accountID` = %u AND `itemID` = %d AND `buyDate` > DATE_SUB(NOW(), INTERVAL 1 MONTH);", accountID, itemID);

										std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(szQuery, SQL_PLAYER));

										if (pMsg->Get()->uiNumRows != 0)
										{
											MYSQL_ROW row = NULL;

											for (int n = 0; (row = mysql_fetch_row(pMsg->Get()->pSQLResult)) != NULL; ++n)
											{
												int buyCount;
												str_to_number(buyCount, row[0]);

												monthCount += buyCount;
											}
										}

										if (monthCount >= itemData.month_limit || monthCount+itemCount > itemData.month_limit)
										{
											BYTE returnType = 2;

											pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, sizeof(BYTE) + sizeof(BYTE) + sizeof(int));
											pkPeer->Encode(&subIndex, sizeof(BYTE));
											pkPeer->Encode(&returnType, sizeof(BYTE));
											pkPeer->Encode(&itemData.month_limit, sizeof(int));
											return;
										}
									}

#ifdef USE_ITEMSHOP_RENEWED
									SetAccountMoney(accountID, coins - itemPrice, jcoins - itemPriceJD);
#else
									SetDragonCoin(accountID, coins - itemPrice);
#endif

									char szQuery[512 + 1] = {};

									snprintf(szQuery, sizeof(szQuery), "INSERT INTO `ishop_log` ("
																		"`accountID`"
																		", `playerName`"
																		", `buyDate`"
																		", `buyTime`"
																		", `ipAdress`"
																		", `itemID`"
																		", `itemVnum`"
																		", `itemCount`"
																		", `itemPrice`"
#ifdef USE_ITEMSHOP_RENEWED
																		", `itemPriceJD`"
#endif
																		") VALUES("
																		"%u"
																		", '%s'"
																		", NOW()"
																		", %d"
																		", '%s'"
																		", %d"
																		", %u"
																		", %d"
																		", %lld"
#ifdef USE_ITEMSHOP_RENEWED
																		", %lld"
#endif
																		")",
																		accountID,
																		playerName,
																		time(0),
																		ipAdress,
																		itemID,
																		itemData.itemVnum,
																		itemCount,
																		itemPrice
#ifdef USE_ITEMSHOP_RENEWED
																		, itemPriceJD
#endif
																		);

									delete CDBManager::instance().DirectQuery(szQuery);

									itemData.sellCount += 1;

									snprintf(szQuery, sizeof(szQuery), "UPDATE `ishop_data` SET "
																		"`sellCount` = %lld"
																		", `maxSellCount` = %d"
																		" WHERE id = %d", itemData.sellCount, itemData.maxSellCount, itemID);

									std::unique_ptr<SQLMsg> pMsgLast(CDBManager::instance().DirectQuery(szQuery));

									char szTimeText[21];
									time_t now = time(0);
									struct tm tstruct = *localtime(&now);
									strftime(szTimeText, sizeof(szTimeText), "%Y-%m-%d %X", &tstruct);

									TIShopLogData logData;
									logData.accountID = accountID;
									strlcpy(logData.playerName, playerName, sizeof(logData.playerName));
									strlcpy(logData.buyDate, szTimeText, sizeof(logData.buyDate));
									logData.buyTime = time(0);
									strlcpy(logData.ipAdress, ipAdress, sizeof(logData.ipAdress));
									logData.itemVnum = itemData.itemVnum;
									logData.itemCount = itemCount;
									logData.itemPrice = itemPrice;
#ifdef USE_ITEMSHOP_RENEWED
									logData.itemPriceJD = itemPriceJD;
#endif

									auto it = m_IShopLogManager.find(accountID);
									if (it == m_IShopLogManager.end())
									{
										if (isLogOpen)
										{
											std::vector<TIShopLogData> m_vec;
											m_vec.emplace_back(logData);

											m_IShopLogManager.emplace(accountID, m_vec);
										}
									}
									else
									{
										it->second.emplace_back(logData);
									}

									int packetSize = sizeof(BYTE) + sizeof(BYTE) + sizeof(bool) + sizeof(DWORD) + sizeof(int) + sizeof(long long)
#ifdef USE_ITEMSHOP_RENEWED
 + sizeof(logData.itemPriceJD)
#endif
									;

									if (isLogOpen)
									{
										packetSize += sizeof(TIShopLogData);
									}

									if (needUpdatePacket)
									{
										BYTE updatePacket = ITEMSHOP_UPDATE_ITEM;

										TEMP_BUFFER buf;
										buf.write(&updatePacket, sizeof(BYTE));
										buf.write(&itemData, sizeof(itemData));

										pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, buf.size());
										pkPeer->Encode(buf.read_peek(), buf.size());
									}

									BYTE returnType = 3;

									pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, dwHandle, packetSize);
									pkPeer->Encode(&subIndex, sizeof(BYTE));
									pkPeer->Encode(&returnType, sizeof(BYTE));
									pkPeer->Encode(&isLogOpen, sizeof(bool));
									pkPeer->Encode(&logData.itemVnum, sizeof(DWORD));
									pkPeer->Encode(&logData.itemCount, sizeof(int));
									pkPeer->Encode(&logData.itemPrice, sizeof(long long));
#ifdef USE_ITEMSHOP_RENEWED
									pkPeer->Encode(&logData.itemPriceJD, sizeof(logData.itemPriceJD));
#endif

									if (isLogOpen)
									{
										pkPeer->Encode(&logData, sizeof(TIShopLogData));
									}

									return;
								}
							}
						}
					}
				}
			}
		}
	}
}

void CClientManager::SendItemShopData(CPeer* pkPeer, bool isPacket)
{
	TEMP_BUFFER buf;
	buf.reset();

	BYTE subIndex = ITEMSHOP_LOAD;
	buf.write(&subIndex, sizeof(BYTE));

	buf.write(&itemShopUpdateTime, sizeof(int));
	buf.write(&isPacket, sizeof(bool));
	int categoryTotalSize = m_IShopManager.size();
	buf.write(&categoryTotalSize, sizeof(int));

	if (categoryTotalSize > 0)
	{
		for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
		{
			BYTE categoryIndex = it->first;
			buf.write(&categoryIndex, sizeof(BYTE));

			BYTE categorySize = it->second.size();
			buf.write(&categorySize, sizeof(BYTE));

			if (categorySize > 0)
			{
				for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
				{
					BYTE categorySubIndex = itEx->first;
					buf.write(&categorySubIndex, sizeof(BYTE));

					BYTE categorySubSize = itEx->second.size();
					buf.write(&categorySubSize, sizeof(BYTE));

					if (categorySubSize > 0)
					{
						buf.write(itEx->second.data(), sizeof(TIShopData) * categorySubSize);
					}
				}
			}
		}
	}

	if (pkPeer)
	{
		pkPeer->EncodeHeader(HEADER_DG_ITEMSHOP, 0, buf.size());
		pkPeer->Encode(buf.read_peek(), buf.size());
	}
	else
	{
		ForwardPacket(HEADER_DG_ITEMSHOP, buf.read_peek(), buf.size());
	}
}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
CClientManager::TGrowthPetCacheSet* CClientManager::GetGrowthPetCacheSet(DWORD pid)
{
	auto it = m_map_pkGrowthPetCacheSetPtr.find(pid);

	if (it == m_map_pkGrowthPetCacheSetPtr.end())
		return nullptr;

	return it->second;
}

void CClientManager::CreateGrowthPetCacheSet(DWORD pid)
{
	if (m_map_pkGrowthPetCacheSetPtr.find(pid) != m_map_pkGrowthPetCacheSetPtr.end())
		return;

	TGrowthPetCacheSet* pSet = new TGrowthPetCacheSet;
	m_map_pkGrowthPetCacheSetPtr.insert(TGrowthPetCacheSetPtrMap::value_type(pid, pSet));

	if (g_log)
		sys_log(0, "GROWTH_PET_CACHE: new cache %u", pid);
}

void CClientManager::FlushGrowthPetCacheSet(DWORD pid)
{
	auto it = m_map_pkGrowthPetCacheSetPtr.find(pid);

	if (it == m_map_pkGrowthPetCacheSetPtr.end())
	{
		sys_log(0, "FLUSH_GROWTH_PET_CACHESET : No GrowthPetCacheSet pid(%d)", pid);
		return;
	}

	TGrowthPetCacheSet* pSet = it->second;
	auto it_set = pSet->begin();

	while (it_set != pSet->end())
	{
		CGrowthPetCache* c = *it_set++;
		c->Flush();

		m_map_growthPetCache.erase(c->Get()->dwID);
		delete c;
	}

	pSet->clear();
	delete pSet;

	m_map_pkGrowthPetCacheSetPtr.erase(it);

	if (g_log)
		sys_log(0, "FLUSH_GROWTH_PET_CACHESET : Deleted pid(%d)", pid);
}

CGrowthPetCache* CClientManager::GetGrowthPetCache(DWORD id)
{
	auto it = m_map_growthPetCache.find(id);

	if (it == m_map_growthPetCache.end())
		return nullptr;

	return it->second;
}

void CClientManager::PutGrowthPetCache(TGrowthPet* pNew, bool bSkipQuery)
{
	CGrowthPetCache* c;

	c = GetGrowthPetCache(pNew->dwID);
	if (!c)
	{
		if (g_log)
			sys_log(0, "GROWTH_PET_CACHE: PutGrowthPetCache ==> New CGrowthPetCache id%d vnum%d new owner%d", pNew->dwID, pNew->dwVnum, pNew->dwOwner);

		c = new CGrowthPetCache;
		m_map_growthPetCache.insert(TGrowthPetCacheMap::value_type(pNew->dwID, c));
	}
	else
	{
		if (g_log)
			sys_log(0, "GROWTH_PET_CACHE: PutGrowthPetCache ==> Have Cache");
		if (pNew->dwOwner != c->Get()->dwOwner)
		{
			auto it = m_map_pkGrowthPetCacheSetPtr.find(c->Get()->dwOwner);

			if (it != m_map_pkGrowthPetCacheSetPtr.end())
			{
				if (g_log)
					sys_log(0, "GROWTH_PET_CACHE: delete owner %u id %u new owner %u", c->Get()->dwOwner, c->Get()->dwID, pNew->dwOwner);
				it->second->erase(c);
			}
		}
	}
	c->Put(pNew, bSkipQuery);

	auto it = m_map_pkGrowthPetCacheSetPtr.find(c->Get()->dwOwner);

	if (it != m_map_pkGrowthPetCacheSetPtr.end())
	{
		if (g_log)
			sys_log(0, "GROWTH_PET_CACHE: save %u id %u", c->Get()->dwOwner, c->Get()->dwID);
		else
			sys_log(1, "GROWTH_PET_CACHE: save %u id %u", c->Get()->dwOwner, c->Get()->dwID);
		it->second->insert(c);
	}
	else
	{
		if (g_log)
			sys_log(0, "GROWTH_PET_CACHE: direct save %u id %u", c->Get()->dwOwner, c->Get()->dwID);
		else
			sys_log(1, "GROWTH_PET_CACHE: direct save %u id %u", c->Get()->dwOwner, c->Get()->dwID);

		c->OnFlush();
	}
}

bool CClientManager::DeleteGrowthPetCache(DWORD dwID)
{
	CGrowthPetCache* c = GetGrowthPetCache(dwID);

	if (!c)
		return false;

	c->Delete();
	return true;
}

void CClientManager::UpdateGrowthPetCache()
{
	if (m_iCacheFlushCount >= m_iCacheFlushCountLimit)
		return;

	auto it = m_map_growthPetCache.begin();

	while (it != m_map_growthPetCache.end())
	{
		CGrowthPetCache* c = (it++)->second;

		if (c->CheckFlushTimeout())
		{
			if (g_test_server)
				sys_log(0, "UpdateGrowthPetCache ==> Flush() vnum %d id owner %d", c->Get()->dwVnum, c->Get()->dwID, c->Get()->dwOwner);

			c->Flush();

			if (++m_iCacheFlushCount >= m_iCacheFlushCountLimit)
				break;
		}
	}
}

void CClientManager::UpdateGrowthPetCacheSet(DWORD pid)
{
	auto it = m_map_pkGrowthPetCacheSetPtr.find(pid);

	if (it == m_map_pkGrowthPetCacheSetPtr.end())
	{
		if (g_test_server)
			sys_log(0, "UPDATE_GROWTH_PET_CACHESET : UpdateGrowthPetCacheSet ==> No GrowthPetCacheSet pid(%d)", pid);
		return;
	}

	TGrowthPetCacheSet* pSet = it->second;
	auto it_set = pSet->begin();

	while (it_set != pSet->end())
	{
		CGrowthPetCache* c = *it_set++;
		c->Flush();
	}

	if (g_log)
		sys_log(0, "UPDATE_GROWTH_PET_CACHESET : UpdateGrowthPetCacheSet pid(%d)", pid);
}

void CClientManager::QUERY_GROWTH_PET_SAVE(CPeer* pkPeer, const char* c_pData)
{
	TGrowthPet* p = (TGrowthPet*)c_pData;
	if (g_test_server)
		sys_log(0, "QUERY_GROWTH_PET_SAVE => PutGrowthPetCache() owner %d id %d vnum %d ", p->dwOwner, p->dwID, p->dwVnum);

	PutGrowthPetCache(p);
}

void CClientManager::QUERY_GROWTH_PET_DELETE(CPeer* pkPeer, const char* c_pData)
{
	DWORD dwID = *(DWORD*)c_pData;
	c_pData += sizeof(DWORD);

	DWORD dwPID = *(DWORD*)c_pData;

	if (!DeleteGrowthPetCache(dwID))
	{
		char szQuery[64];
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM growth_pet%s WHERE id=%u", GetTablePostfix(), dwID);

		if (dwPID == 0)
			CDBManager::instance().AsyncQuery(szQuery);
		else
			CDBManager::instance().ReturnQuery(szQuery, QID_GROWTH_PET_DELETE, pkPeer->GetHandle(), NULL);
	}
}

bool CClientManager::CreateGrowthPetTableFromRes(MYSQL_RES* res, std::vector<TGrowthPet>* pVec, DWORD dwPID)
{
	if (!res)
	{
		pVec->clear();
		return true;
	}

	int rows;

	if ((rows = mysql_num_rows(res)) <= 0)
	{
		pVec->clear();
		return true;
	}

	pVec->resize(rows);

	for (int i = 0; i < rows; ++i)
	{
		MYSQL_ROW row = mysql_fetch_row(res);
		TGrowthPet& pet = pVec->at(i);

		int cur = 0;

		str_to_number(pet.dwID, row[cur++]);
		str_to_number(pet.dwVnum, row[cur++]);
		str_to_number(pet.bState, row[cur++]);
		strlcpy(pet.szName, row[cur++], sizeof(pet.szName));
		str_to_number(pet.bSize, row[cur++]);
		str_to_number(pet.dwLevel, row[cur++]);
		str_to_number(pet.bLevelStep, row[cur++]);
		str_to_number(pet.bEvolution, row[cur++]);
		str_to_number(pet.bType, row[cur++]);
		str_to_number(pet.dwHP, row[cur++]);
		str_to_number(pet.dwSP, row[cur++]);
		str_to_number(pet.dwDef, row[cur++]);
		str_to_number(pet.dwHPApply, row[cur++]);
		str_to_number(pet.dwSPApply, row[cur++]);
		str_to_number(pet.dwDefApply, row[cur++]);
		str_to_number(pet.dwAgeApply, row[cur++]);
		thecore_memcpy(pet.aSkill, row[cur++], sizeof(pet.aSkill));
		str_to_number(pet.lExp, row[cur++]);
		str_to_number(pet.lItemExp, row[cur++]);
		str_to_number(pet.lBirthday, row[cur++]);
		str_to_number(pet.lEndTime, row[cur++]);
		str_to_number(pet.lMaxTime, row[cur++]);

		pet.dwOwner = dwPID;
	}

	return true;
}

void CClientManager::RESULT_GROWTH_PET_LOAD(CPeer* peer, MYSQL_RES* pRes, DWORD dwHandle, DWORD dwPID)
{
	static std::vector<TGrowthPet> s_petVec;
	CreateGrowthPetTableFromRes(pRes, &s_petVec, dwPID);
	DWORD dwCount = s_petVec.size();

	peer->EncodeHeader(HEADER_DG_GROWTH_PET_LOAD, dwHandle, sizeof(DWORD) + sizeof(TGrowthPet) * dwCount);
	peer->EncodeDWORD(dwCount);

	CreateGrowthPetCacheSet(dwPID);

	sys_log(0, "GROWTH_PET_LOAD: count %u pid %u", dwCount, dwPID);

	if (dwCount)
	{
		peer->Encode(&s_petVec[0], sizeof(TGrowthPet) * dwCount);

		for (DWORD i = 0; i < dwCount; ++i)
			PutGrowthPetCache(&s_petVec[i], true);
	}
}
#endif

#ifdef ENABLE_RANKING
void CClientManager::QUERY_ADD_RANKING(CPeer * peer, TPacketGDAddRanking * p, DWORD dwLen)
{
	if (0 != (dwLen % sizeof(TPacketGDAddRanking)))
	{
		sys_err("invalid packet size %d, sizeof(TPacketGDAddRanking) == %d", dwLen, sizeof(TPacketGDAddRanking));
		return;
	}

	int iSize = dwLen / sizeof(TPacketGDAddRanking);

	for (int i = 0; i < iSize; ++i, ++p)
	{

		char queryStr[QUERY_MAX_LEN];
		snprintf(queryStr, sizeof(queryStr),
				"REPLACE INTO player_ranking%s (dwPID, rank, lValue) "
				"VALUES(%u, %u, %lld)",
				GetTablePostfix(), p->dwPID, p->bTypeRank, p->lValue);

		CDBManager::instance().AsyncQuery(queryStr);
	}
}
#endif
