#include "stdafx.h"

#include "../../common/service.h"

#include "Cache.h"
#include "QID.h"
#include "ClientManager.h"
#include "Main.h"

extern CPacketInfo g_item_info;
extern int g_iPlayerCacheFlushSeconds;
extern int g_iItemCacheFlushSeconds;
extern int g_test_server;
extern int g_iItemPriceListTableCacheFlushSeconds;
extern int g_item_count;

CItemCache::CItemCache()
{
	m_expireTime = MIN(1800, g_iItemCacheFlushSeconds);
}

CItemCache::~CItemCache()
{
}

void CItemCache::Delete()
{
	if (m_data.vnum == 0)
		return;

	if (g_test_server)
		sys_log(0, "ItemCache::Delete : DELETE %u", m_data.id);

	m_data.vnum = 0;
	m_bNeedQuery = true;
	m_lastUpdateTime = time(0);
	OnFlush();
}

void CItemCache::OnFlush()
{
	if (m_data.vnum == 0)
	{
		char szQuery[QUERY_MAX_LEN];
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM item%s WHERE id=%u", GetTablePostfix(), m_data.id);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEM_DESTROY, 0, NULL);

		if (g_test_server)
			sys_log(0, "ItemCache::Flush : DELETE %u %s", m_data.id, szQuery);
	}
	else
	{
		long alSockets[ITEM_SOCKET_MAX_NUM];
		TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
		bool isSocket = false, isAttr = false;

		memset(&alSockets, 0, sizeof(long) * ITEM_SOCKET_MAX_NUM);
		memset(&aAttr, 0, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM);

		TPlayerItem * p = &m_data;

		if (memcmp(alSockets, p->alSockets, sizeof(long) * ITEM_SOCKET_MAX_NUM))
			isSocket = true;

		if (memcmp(aAttr, p->aAttr, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM))
			isAttr = true;

		char szColumns[QUERY_MAX_LEN];
		char szValues[QUERY_MAX_LEN];
		char szUpdate[QUERY_MAX_LEN];

#if defined(ENABLE_CHANGE_LOOK_SYSTEM) && defined(ENABLE_SET_ITEM)
		int iLen = snprintf(szColumns, sizeof(szColumns), "id, owner_id, window, pos, count, vnum, transmutation, set_value");
		int iValueLen = snprintf(szValues, sizeof(szValues), "%u, %u, %d, %d, %u, %u, %u, %u", p->id, p->owner, p->window, p->pos, p->count, p->vnum, p->dwTransmutationVnum, p->dwSetValue);
		int iUpdateLen = snprintf(szUpdate, sizeof(szUpdate), "owner_id=%u, window=%d, pos=%d, count=%u, vnum=%u, transmutation=%u, set_value=%u", p->owner, p->window, p->pos, p->count, p->vnum, p->dwTransmutationVnum, p->dwSetValue);
#elif defined(ENABLE_CHANGE_LOOK_SYSTEM)
		int iLen = snprintf(szColumns, sizeof(szColumns), "id, owner_id, window, pos, count, vnum, transmutation");
		int iValueLen = snprintf(szValues, sizeof(szValues), "%u, %u, %d, %d, %u, %u, %u", p->id, p->owner, p->window, p->pos, p->count, p->vnum, p->dwTransmutationVnum);
		int iUpdateLen = snprintf(szUpdate, sizeof(szUpdate), "owner_id=%u, window=%d, pos=%d, count=%u, vnum=%u, transmutation=%u", p->owner, p->window, p->pos, p->count, p->vnum, p->dwTransmutationVnum);
#elif defined(ENABLE_SET_ITEM)
		int iLen = snprintf(szColumns, sizeof(szColumns), "id, owner_id, window, pos, count, vnum, set_value");
		int iValueLen = snprintf(szValues, sizeof(szValues), "%u, %u, %d, %d, %u, %u, %u", p->id, p->owner, p->window, p->pos, p->count, p->vnum, p->dwSetValue);
		int iUpdateLen = snprintf(szUpdate, sizeof(szUpdate), "owner_id=%u, window=%d, pos=%d, count=%u, vnum=%u, set_value=%u", p->owner, p->window, p->pos, p->count, p->vnum, p->dwSetValue);
#else
		int iLen = snprintf(szColumns, sizeof(szColumns), "id, owner_id, window, pos, count, vnum");
		int iValueLen = snprintf(szValues, sizeof(szValues), "%u, %u, %d, %d, %u, %u", p->id, p->owner, p->window, p->pos, p->count, p->vnum);
#endif

#ifdef ENABLE_SEALBIND_SYSTEM
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", bind");
		iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen, ", %ld", p->sealbind);
		iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen, ", bind=%ld", p->sealbind);
#endif

		if (isSocket)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", socket0, socket1, socket2");
			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen, ", %lu, %lu, %lu", p->alSockets[0], p->alSockets[1], p->alSockets[2]);
			iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen, ", socket0=%lu, socket1=%lu, socket2=%lu", p->alSockets[0], p->alSockets[1], p->alSockets[2]);
		}

		if (isAttr)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, 
					", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3"
					", attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6");

			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
					", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
					p->aAttr[0].bType, p->aAttr[0].sValue,
					p->aAttr[1].bType, p->aAttr[1].sValue,
					p->aAttr[2].bType, p->aAttr[2].sValue,
					p->aAttr[3].bType, p->aAttr[3].sValue,
					p->aAttr[4].bType, p->aAttr[4].sValue,
					p->aAttr[5].bType, p->aAttr[5].sValue,
					p->aAttr[6].bType, p->aAttr[6].sValue);

			iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
					", attrtype0=%d, attrvalue0=%d"
					", attrtype1=%d, attrvalue1=%d"
					", attrtype2=%d, attrvalue2=%d"
					", attrtype3=%d, attrvalue3=%d"
					", attrtype4=%d, attrvalue4=%d"
					", attrtype5=%d, attrvalue5=%d"
					", attrtype6=%d, attrvalue6=%d",
					p->aAttr[0].bType, p->aAttr[0].sValue,
					p->aAttr[1].bType, p->aAttr[1].sValue,
					p->aAttr[2].bType, p->aAttr[2].sValue,
					p->aAttr[3].bType, p->aAttr[3].sValue,
					p->aAttr[4].bType, p->aAttr[4].sValue,
					p->aAttr[5].bType, p->aAttr[5].sValue,
					p->aAttr[6].bType, p->aAttr[6].sValue);
		}

		char szItemQuery[QUERY_MAX_LEN + QUERY_MAX_LEN];
		snprintf(szItemQuery, sizeof(szItemQuery), "REPLACE INTO item%s (%s) VALUES(%s)", GetTablePostfix(), szColumns, szValues);

		if (g_test_server)
			sys_log(0, "ItemCache::Flush :REPLACE  (%s)", szItemQuery);

		CDBManager::instance().ReturnQuery(szItemQuery, QID_ITEM_SAVE, 0, NULL);

		//g_item_info.Add(p->vnum);
		++g_item_count;
	}

	m_bNeedQuery = false;
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
extern int g_iSkillColorCacheFlushSeconds;

CSKillColorCache::CSKillColorCache()
{
	m_expireTime = MIN(1800, g_iSkillColorCacheFlushSeconds);
}

CSKillColorCache::~CSKillColorCache()
{
}

void CSKillColorCache::OnFlush()
{
	char szQuery[QUERY_MAX_LEN];
#if !defined(ENABLE_NINETH_SKILL) && !defined(ENABLE_WOLFMAN_CHARACTER)
	snprintf(szQuery, sizeof(szQuery),
		"REPLACE INTO skill_color%s (player_id"
		", skillSlot1_Col1, skillSlot1_Col2, skillSlot1_Col3, skillSlot1_Col4, skillSlot1_Col5"
		", skillSlot2_Col1, skillSlot2_Col2, skillSlot2_Col3, skillSlot2_Col4, skillSlot2_Col5"
		", skillSlot3_Col1, skillSlot3_Col2, skillSlot3_Col3, skillSlot3_Col4, skillSlot3_Col5"
		", skillSlot4_Col1, skillSlot4_Col2, skillSlot4_Col3, skillSlot4_Col4, skillSlot4_Col5"
		", skillSlot5_Col1, skillSlot5_Col2, skillSlot5_Col3, skillSlot5_Col4, skillSlot5_Col5"
		", skillSlot6_Col1, skillSlot6_Col2, skillSlot6_Col3, skillSlot6_Col4, skillSlot6_Col5"
		", buffSkill1_Col1, buffSkill1_Col2, buffSkill1_Col3, buffSkill1_Col4, buffSkill1_Col5"
		", buffSkill2_Col1, buffSkill2_Col2, buffSkill2_Col3, buffSkill2_Col4, buffSkill2_Col5"
		", buffSkill3_Col1, buffSkill3_Col2, buffSkill3_Col3, buffSkill3_Col4, buffSkill3_Col5"
		", buffSkill4_Col1, buffSkill4_Col2, buffSkill4_Col3, buffSkill4_Col4, buffSkill4_Col5"
		", buffSkill5_Col1, buffSkill5_Col2, buffSkill5_Col3, buffSkill5_Col4, buffSkill5_Col5"
		", buffSkill6_Col1, buffSkill6_Col2, buffSkill6_Col3, buffSkill6_Col4, buffSkill6_Col5"
		") VALUES (%d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, 0, 0, 0, 0, 0)"
		, GetTablePostfix(), m_data.dwPlayerID
		, m_data.dwSkillColor[0][0], m_data.dwSkillColor[0][1], m_data.dwSkillColor[0][2], m_data.dwSkillColor[0][3], m_data.dwSkillColor[0][4]
		, m_data.dwSkillColor[1][0], m_data.dwSkillColor[1][1], m_data.dwSkillColor[1][2], m_data.dwSkillColor[1][3], m_data.dwSkillColor[1][4]
		, m_data.dwSkillColor[2][0], m_data.dwSkillColor[2][1], m_data.dwSkillColor[2][2], m_data.dwSkillColor[2][3], m_data.dwSkillColor[2][4]
		, m_data.dwSkillColor[3][0], m_data.dwSkillColor[3][1], m_data.dwSkillColor[3][2], m_data.dwSkillColor[3][3], m_data.dwSkillColor[3][4]
		, m_data.dwSkillColor[4][0], m_data.dwSkillColor[4][1], m_data.dwSkillColor[4][2], m_data.dwSkillColor[4][3], m_data.dwSkillColor[4][4]
		, m_data.dwSkillColor[5][0], m_data.dwSkillColor[5][1], m_data.dwSkillColor[5][2], m_data.dwSkillColor[5][3], m_data.dwSkillColor[5][4]
		, m_data.dwSkillColor[6][0], m_data.dwSkillColor[6][1], m_data.dwSkillColor[6][2], m_data.dwSkillColor[6][3], m_data.dwSkillColor[6][4]
		, m_data.dwSkillColor[7][0], m_data.dwSkillColor[7][1], m_data.dwSkillColor[7][2], m_data.dwSkillColor[7][3], m_data.dwSkillColor[7][4]
		, m_data.dwSkillColor[8][0], m_data.dwSkillColor[8][1], m_data.dwSkillColor[8][2], m_data.dwSkillColor[8][3], m_data.dwSkillColor[8][4]
		, m_data.dwSkillColor[9][0], m_data.dwSkillColor[9][1], m_data.dwSkillColor[9][2], m_data.dwSkillColor[9][3], m_data.dwSkillColor[9][4]
		, m_data.dwSkillColor[10][0], m_data.dwSkillColor[10][1], m_data.dwSkillColor[10][2], m_data.dwSkillColor[10][3], m_data.dwSkillColor[10][4]
		);
#elif !defined(ENABLE_NINETH_SKILL)
	snprintf(szQuery, sizeof(szQuery),
		"REPLACE INTO skill_color%s (player_id"
		", skillSlot1_Col1, skillSlot1_Col2, skillSlot1_Col3, skillSlot1_Col4, skillSlot1_Col5"
		", skillSlot2_Col1, skillSlot2_Col2, skillSlot2_Col3, skillSlot2_Col4, skillSlot2_Col5"
		", skillSlot3_Col1, skillSlot3_Col2, skillSlot3_Col3, skillSlot3_Col4, skillSlot3_Col5"
		", skillSlot4_Col1, skillSlot4_Col2, skillSlot4_Col3, skillSlot4_Col4, skillSlot4_Col5"
		", skillSlot5_Col1, skillSlot5_Col2, skillSlot5_Col3, skillSlot5_Col4, skillSlot5_Col5"
		", skillSlot6_Col1, skillSlot6_Col2, skillSlot6_Col3, skillSlot6_Col4, skillSlot6_Col5"
		", buffSkill1_Col1, buffSkill1_Col2, buffSkill1_Col3, buffSkill1_Col4, buffSkill1_Col5"
		", buffSkill2_Col1, buffSkill2_Col2, buffSkill2_Col3, buffSkill2_Col4, buffSkill2_Col5"
		", buffSkill3_Col1, buffSkill3_Col2, buffSkill3_Col3, buffSkill3_Col4, buffSkill3_Col5"
		", buffSkill4_Col1, buffSkill4_Col2, buffSkill4_Col3, buffSkill4_Col4, buffSkill4_Col5"
		", buffSkill5_Col1, buffSkill5_Col2, buffSkill5_Col3, buffSkill5_Col4, buffSkill5_Col5"
		", buffSkill6_Col1, buffSkill6_Col2, buffSkill6_Col3, buffSkill6_Col4, buffSkill6_Col5"
		") VALUES (%d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)"
		, GetTablePostfix(), m_data.dwPlayerID
		, m_data.dwSkillColor[0][0], m_data.dwSkillColor[0][1], m_data.dwSkillColor[0][2], m_data.dwSkillColor[0][3], m_data.dwSkillColor[0][4]
		, m_data.dwSkillColor[1][0], m_data.dwSkillColor[1][1], m_data.dwSkillColor[1][2], m_data.dwSkillColor[1][3], m_data.dwSkillColor[1][4]
		, m_data.dwSkillColor[2][0], m_data.dwSkillColor[2][1], m_data.dwSkillColor[2][2], m_data.dwSkillColor[2][3], m_data.dwSkillColor[2][4]
		, m_data.dwSkillColor[3][0], m_data.dwSkillColor[3][1], m_data.dwSkillColor[3][2], m_data.dwSkillColor[3][3], m_data.dwSkillColor[3][4]
		, m_data.dwSkillColor[4][0], m_data.dwSkillColor[4][1], m_data.dwSkillColor[4][2], m_data.dwSkillColor[4][3], m_data.dwSkillColor[4][4]
		, m_data.dwSkillColor[5][0], m_data.dwSkillColor[5][1], m_data.dwSkillColor[5][2], m_data.dwSkillColor[5][3], m_data.dwSkillColor[5][4]
		, m_data.dwSkillColor[6][0], m_data.dwSkillColor[6][1], m_data.dwSkillColor[6][2], m_data.dwSkillColor[6][3], m_data.dwSkillColor[6][4]
		, m_data.dwSkillColor[7][0], m_data.dwSkillColor[7][1], m_data.dwSkillColor[7][2], m_data.dwSkillColor[7][3], m_data.dwSkillColor[7][4]
		, m_data.dwSkillColor[8][0], m_data.dwSkillColor[8][1], m_data.dwSkillColor[8][2], m_data.dwSkillColor[8][3], m_data.dwSkillColor[8][4]
		, m_data.dwSkillColor[9][0], m_data.dwSkillColor[9][1], m_data.dwSkillColor[9][2], m_data.dwSkillColor[9][3], m_data.dwSkillColor[9][4]
		, m_data.dwSkillColor[10][0], m_data.dwSkillColor[10][1], m_data.dwSkillColor[10][2], m_data.dwSkillColor[10][3], m_data.dwSkillColor[10][4]
		, m_data.dwSkillColor[11][0], m_data.dwSkillColor[11][1], m_data.dwSkillColor[11][2], m_data.dwSkillColor[11][3], m_data.dwSkillColor[11][4]
		);
#elif !defined(ENABLE_WOLFMAN_CHARACTER)
	snprintf(szQuery, sizeof(szQuery),
		"REPLACE INTO skill_color%s (player_id"
		", skillSlot1_Col1, skillSlot1_Col2, skillSlot1_Col3, skillSlot1_Col4, skillSlot1_Col5"
		", skillSlot2_Col1, skillSlot2_Col2, skillSlot2_Col3, skillSlot2_Col4, skillSlot2_Col5"
		", skillSlot3_Col1, skillSlot3_Col2, skillSlot3_Col3, skillSlot3_Col4, skillSlot3_Col5"
		", skillSlot4_Col1, skillSlot4_Col2, skillSlot4_Col3, skillSlot4_Col4, skillSlot4_Col5"
		", skillSlot5_Col1, skillSlot5_Col2, skillSlot5_Col3, skillSlot5_Col4, skillSlot5_Col5"
		", skillSlot6_Col1, skillSlot6_Col2, skillSlot6_Col3, skillSlot6_Col4, skillSlot6_Col5"
		", skillSlot7_Col1, skillSlot7_Col2, skillSlot7_Col3, skillSlot7_Col4, skillSlot7_Col5"
		", skillSlot8_Col1, skillSlot8_Col2, skillSlot8_Col3, skillSlot8_Col4, skillSlot8_Col5"
		", skillSlot9_Col1, skillSlot9_Col2, skillSlot9_Col3, skillSlot9_Col4, skillSlot9_Col5"
		", buffSkill1_Col1, buffSkill1_Col2, buffSkill1_Col3, buffSkill1_Col4, buffSkill1_Col5"
		", buffSkill2_Col1, buffSkill2_Col2, buffSkill2_Col3, buffSkill2_Col4, buffSkill2_Col5"
		", buffSkill3_Col1, buffSkill3_Col2, buffSkill3_Col3, buffSkill3_Col4, buffSkill3_Col5"
		", buffSkill4_Col1, buffSkill4_Col2, buffSkill4_Col3, buffSkill4_Col4, buffSkill4_Col5"
		", buffSkill5_Col1, buffSkill5_Col2, buffSkill5_Col3, buffSkill5_Col4, buffSkill5_Col5"
		", buffSkill6_Col1, buffSkill6_Col2, buffSkill6_Col3, buffSkill6_Col4, buffSkill6_Col5"
		", buffSkill7_Col1, buffSkill7_Col2, buffSkill7_Col3, buffSkill7_Col4, buffSkill7_Col5"
		") VALUES (%d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)"
		, GetTablePostfix(), m_data.dwPlayerID
		, m_data.dwSkillColor[0][0], m_data.dwSkillColor[0][1], m_data.dwSkillColor[0][2], m_data.dwSkillColor[0][3], m_data.dwSkillColor[0][4]
		, m_data.dwSkillColor[1][0], m_data.dwSkillColor[1][1], m_data.dwSkillColor[1][2], m_data.dwSkillColor[1][3], m_data.dwSkillColor[1][4]
		, m_data.dwSkillColor[2][0], m_data.dwSkillColor[2][1], m_data.dwSkillColor[2][2], m_data.dwSkillColor[2][3], m_data.dwSkillColor[2][4]
		, m_data.dwSkillColor[3][0], m_data.dwSkillColor[3][1], m_data.dwSkillColor[3][2], m_data.dwSkillColor[3][3], m_data.dwSkillColor[3][4]
		, m_data.dwSkillColor[4][0], m_data.dwSkillColor[4][1], m_data.dwSkillColor[4][2], m_data.dwSkillColor[4][3], m_data.dwSkillColor[4][4]
		, m_data.dwSkillColor[5][0], m_data.dwSkillColor[5][1], m_data.dwSkillColor[5][2], m_data.dwSkillColor[5][3], m_data.dwSkillColor[5][4]
		, m_data.dwSkillColor[6][0], m_data.dwSkillColor[6][1], m_data.dwSkillColor[6][2], m_data.dwSkillColor[6][3], m_data.dwSkillColor[6][4]
		, m_data.dwSkillColor[7][0], m_data.dwSkillColor[7][1], m_data.dwSkillColor[7][2], m_data.dwSkillColor[7][3], m_data.dwSkillColor[7][4]
		, m_data.dwSkillColor[8][0], m_data.dwSkillColor[8][1], m_data.dwSkillColor[8][2], m_data.dwSkillColor[8][3], m_data.dwSkillColor[8][4]
		, m_data.dwSkillColor[9][0], m_data.dwSkillColor[9][1], m_data.dwSkillColor[9][2], m_data.dwSkillColor[9][3], m_data.dwSkillColor[9][4]
		, m_data.dwSkillColor[10][0], m_data.dwSkillColor[10][1], m_data.dwSkillColor[10][2], m_data.dwSkillColor[10][3], m_data.dwSkillColor[10][4]
		, m_data.dwSkillColor[11][0], m_data.dwSkillColor[11][1], m_data.dwSkillColor[11][2], m_data.dwSkillColor[11][3], m_data.dwSkillColor[11][4]
		, m_data.dwSkillColor[12][0], m_data.dwSkillColor[12][1], m_data.dwSkillColor[12][2], m_data.dwSkillColor[12][3], m_data.dwSkillColor[12][4]
		, m_data.dwSkillColor[13][0], m_data.dwSkillColor[13][1], m_data.dwSkillColor[13][2], m_data.dwSkillColor[13][3], m_data.dwSkillColor[13][4]
		);
#else
	snprintf(szQuery, sizeof(szQuery),
		"REPLACE INTO skill_color%s (player_id"
		", skillSlot1_Col1, skillSlot1_Col2, skillSlot1_Col3, skillSlot1_Col4, skillSlot1_Col5"
		", skillSlot2_Col1, skillSlot2_Col2, skillSlot2_Col3, skillSlot2_Col4, skillSlot2_Col5"
		", skillSlot3_Col1, skillSlot3_Col2, skillSlot3_Col3, skillSlot3_Col4, skillSlot3_Col5"
		", skillSlot4_Col1, skillSlot4_Col2, skillSlot4_Col3, skillSlot4_Col4, skillSlot4_Col5"
		", skillSlot5_Col1, skillSlot5_Col2, skillSlot5_Col3, skillSlot5_Col4, skillSlot5_Col5"
		", skillSlot6_Col1, skillSlot6_Col2, skillSlot6_Col3, skillSlot6_Col4, skillSlot6_Col5"
		", skillSlot7_Col1, skillSlot7_Col2, skillSlot7_Col3, skillSlot7_Col4, skillSlot7_Col5"
		", skillSlot8_Col1, skillSlot8_Col2, skillSlot8_Col3, skillSlot8_Col4, skillSlot8_Col5"
		", skillSlot9_Col1, skillSlot9_Col2, skillSlot9_Col3, skillSlot9_Col4, skillSlot9_Col5"
		", buffSkill1_Col1, buffSkill1_Col2, buffSkill1_Col3, buffSkill1_Col4, buffSkill1_Col5"
		", buffSkill2_Col1, buffSkill2_Col2, buffSkill2_Col3, buffSkill2_Col4, buffSkill2_Col5"
		", buffSkill3_Col1, buffSkill3_Col2, buffSkill3_Col3, buffSkill3_Col4, buffSkill3_Col5"
		", buffSkill4_Col1, buffSkill4_Col2, buffSkill4_Col3, buffSkill4_Col4, buffSkill4_Col5"
		", buffSkill5_Col1, buffSkill5_Col2, buffSkill5_Col3, buffSkill5_Col4, buffSkill5_Col5"
		", buffSkill6_Col1, buffSkill6_Col2, buffSkill6_Col3, buffSkill6_Col4, buffSkill6_Col5"
		", buffSkill7_Col1, buffSkill7_Col2, buffSkill7_Col3, buffSkill7_Col4, buffSkill7_Col5"
		") VALUES (%d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d"
		", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)"
		, GetTablePostfix(), m_data.dwPlayerID
		, m_data.dwSkillColor[0][0], m_data.dwSkillColor[0][1], m_data.dwSkillColor[0][2], m_data.dwSkillColor[0][3], m_data.dwSkillColor[0][4]
		, m_data.dwSkillColor[1][0], m_data.dwSkillColor[1][1], m_data.dwSkillColor[1][2], m_data.dwSkillColor[1][3], m_data.dwSkillColor[1][4]
		, m_data.dwSkillColor[2][0], m_data.dwSkillColor[2][1], m_data.dwSkillColor[2][2], m_data.dwSkillColor[2][3], m_data.dwSkillColor[2][4]
		, m_data.dwSkillColor[3][0], m_data.dwSkillColor[3][1], m_data.dwSkillColor[3][2], m_data.dwSkillColor[3][3], m_data.dwSkillColor[3][4]
		, m_data.dwSkillColor[4][0], m_data.dwSkillColor[4][1], m_data.dwSkillColor[4][2], m_data.dwSkillColor[4][3], m_data.dwSkillColor[4][4]
		, m_data.dwSkillColor[5][0], m_data.dwSkillColor[5][1], m_data.dwSkillColor[5][2], m_data.dwSkillColor[5][3], m_data.dwSkillColor[5][4]
		, m_data.dwSkillColor[6][0], m_data.dwSkillColor[6][1], m_data.dwSkillColor[6][2], m_data.dwSkillColor[6][3], m_data.dwSkillColor[6][4]
		, m_data.dwSkillColor[7][0], m_data.dwSkillColor[7][1], m_data.dwSkillColor[7][2], m_data.dwSkillColor[7][3], m_data.dwSkillColor[7][4]
		, m_data.dwSkillColor[8][0], m_data.dwSkillColor[8][1], m_data.dwSkillColor[8][2], m_data.dwSkillColor[8][3], m_data.dwSkillColor[8][4]
		, m_data.dwSkillColor[9][0], m_data.dwSkillColor[9][1], m_data.dwSkillColor[9][2], m_data.dwSkillColor[9][3], m_data.dwSkillColor[9][4]
		, m_data.dwSkillColor[10][0], m_data.dwSkillColor[10][1], m_data.dwSkillColor[10][2], m_data.dwSkillColor[10][3], m_data.dwSkillColor[10][4]
		, m_data.dwSkillColor[11][0], m_data.dwSkillColor[11][1], m_data.dwSkillColor[11][2], m_data.dwSkillColor[11][3], m_data.dwSkillColor[11][4]
		, m_data.dwSkillColor[12][0], m_data.dwSkillColor[12][1], m_data.dwSkillColor[12][2], m_data.dwSkillColor[12][3], m_data.dwSkillColor[12][4]
		, m_data.dwSkillColor[13][0], m_data.dwSkillColor[13][1], m_data.dwSkillColor[13][2], m_data.dwSkillColor[13][3], m_data.dwSkillColor[13][4]
		, m_data.dwSkillColor[14][0], m_data.dwSkillColor[14][1], m_data.dwSkillColor[14][2], m_data.dwSkillColor[14][3], m_data.dwSkillColor[14][4]
		, m_data.dwSkillColor[15][0], m_data.dwSkillColor[15][1], m_data.dwSkillColor[15][2], m_data.dwSkillColor[15][3], m_data.dwSkillColor[15][4]
		);
#endif

	CDBManager::instance().ReturnQuery(szQuery, QID_SKILL_COLOR_SAVE, 0, NULL);

	if (g_test_server)
		sys_log(0, "SkillColorCache::Flush :REPLACE %u (%s)", m_data.dwPlayerID, szQuery);

	m_bNeedQuery = false;
}
#endif

//
// CPlayerTableCache
//
CPlayerTableCache::CPlayerTableCache()
{
	m_expireTime = MIN(1800, g_iPlayerCacheFlushSeconds);
}

CPlayerTableCache::~CPlayerTableCache()
{
}

void CPlayerTableCache::OnFlush()
{
	if (g_test_server)
		sys_log(0, "PlayerTableCache::Flush : %s", m_data.name);

	char szQuery[QUERY_MAX_LEN];
	CreatePlayerSaveQuery(szQuery, sizeof(szQuery), &m_data);
	CDBManager::instance().ReturnQuery(szQuery, QID_PLAYER_SAVE, 0, NULL);
}

const int CItemPriceListTableCache::s_nMinFlushSec = 1800;

CItemPriceListTableCache::CItemPriceListTableCache()
{
	m_expireTime = MIN(s_nMinFlushSec, g_iItemPriceListTableCacheFlushSeconds);
}

void CItemPriceListTableCache::UpdateList(const TItemPriceListTable* pUpdateList)
{
	std::vector<TItemPriceInfo> tmpvec;

	for (uint idx = 0; idx < m_data.byCount; ++idx)
	{
		const TItemPriceInfo* pos = pUpdateList->aPriceInfo;
		for (; pos != pUpdateList->aPriceInfo + pUpdateList->byCount && m_data.aPriceInfo[idx].dwVnum != pos->dwVnum; ++pos)
			;

		if (pos == pUpdateList->aPriceInfo + pUpdateList->byCount)
			tmpvec.push_back(m_data.aPriceInfo[idx]);
	}

	if (pUpdateList->byCount > SHOP_PRICELIST_MAX_NUM)
	{
		sys_err("Count overflow!");
		return;
	}

	m_data.byCount = pUpdateList->byCount;

	thecore_memcpy(m_data.aPriceInfo, pUpdateList->aPriceInfo, sizeof(TItemPriceInfo) * pUpdateList->byCount);

	int nDeletedNum;

	if (pUpdateList->byCount < SHOP_PRICELIST_MAX_NUM)
	{
		size_t sizeAddOldDataSize = SHOP_PRICELIST_MAX_NUM - pUpdateList->byCount;

		if (tmpvec.size() < sizeAddOldDataSize)
			sizeAddOldDataSize = tmpvec.size();

		thecore_memcpy(m_data.aPriceInfo + pUpdateList->byCount, &tmpvec[0], sizeof(TItemPriceInfo) * sizeAddOldDataSize);
		m_data.byCount += sizeAddOldDataSize;

		nDeletedNum = tmpvec.size() - sizeAddOldDataSize;
	}
	else
		nDeletedNum = tmpvec.size();

	m_bNeedQuery = true;

	sys_log(0,
		"ItemPriceListTableCache::UpdateList : OwnerID[%u] Update [%u] Items, Delete [%u] Items, Total [%u] Items",
		m_data.dwOwnerID, pUpdateList->byCount, nDeletedNum, m_data.byCount);
}

void CItemPriceListTableCache::OnFlush()
{
	char szQuery[QUERY_MAX_LEN];

	snprintf(szQuery, sizeof(szQuery), "DELETE FROM myshop_pricelist%s WHERE owner_id = %u", GetTablePostfix(), m_data.dwOwnerID);
	CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_DESTROY, 0, NULL);

	for (int idx = 0; idx < m_data.byCount; ++idx)
	{
		snprintf(szQuery, sizeof(szQuery),
			"REPLACE myshop_pricelist%s(owner_id, item_vnum, price) VALUES(%u, %u, %u)", // @fixme204 (INSERT INTO -> REPLACE)
			GetTablePostfix(), m_data.dwOwnerID, m_data.aPriceInfo[idx].dwVnum, m_data.aPriceInfo[idx].dwPrice);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_SAVE, 0, NULL);
	}

	sys_log(0, "ItemPriceListTableCache::Flush : OwnerID[%u] Update [%u]Items", m_data.dwOwnerID, m_data.byCount);

	m_bNeedQuery = false;
}

#ifdef ENABLE_GROWTH_PET_SYSTEM
CGrowthPetCache::CGrowthPetCache()
{
	m_expireTime = MIN(5, g_iItemCacheFlushSeconds);
}

CGrowthPetCache::~CGrowthPetCache()
{
}

void CGrowthPetCache::Delete()
{
	if (m_data.dwVnum == 0)
		return;

	if (g_test_server)
		sys_log(0, "CGrowthPetCache::Delete : DELETE %u", m_data.dwID);

	m_data.dwVnum = 0;
	m_bNeedQuery = true;
	m_lastUpdateTime = time(0);
	OnFlush();
}

void CGrowthPetCache::OnFlush()
{
	char szQuery[QUERY_MAX_LEN];

	if (m_data.dwVnum == 0)
	{
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM growth_pet%s WHERE id=%u", GetTablePostfix(), m_data.dwID);
		CDBManager::instance().ReturnQuery(szQuery, QID_GROWTH_PET_DELETE, 0, NULL);

		if (g_test_server)
			sys_log(0, "CGrowthPetCache::Flush : DELETE %u %s", m_data.dwID, szQuery);
	}
	else
	{
		size_t queryLen;
		queryLen = snprintf(szQuery, sizeof(szQuery),
			"REPLACE INTO growth_pet%s SET "
			"id = %d, "
			"owner_id = %d, "
			"vnum = %d, "
			"state = %d, "
			"name = '%s', "
			"size = %d, "
			"level = %d,"
			"level_step = %d,"
			"evolution = %d, "
			"type = %d, "
			"hp = %d, "
			"sp = %d, "
			"def = %d, "
			"hp_apply = %d, "
			"sp_apply = %d, "
			"def_apply = %d, "
			"age_apply = %d, "
			"exp = %d, "
			"item_exp = %d, "
			"birthday = FROM_UNIXTIME(%d), "
			"end_time = %d, "
			"max_time = %d, ",

			GetTablePostfix(),
			m_data.dwID,
			m_data.dwOwner,
			m_data.dwVnum,
			m_data.bState,
			m_data.szName,
			m_data.bSize,
			m_data.dwLevel,
			m_data.bLevelStep,
			m_data.bEvolution,
			m_data.bType,
			m_data.dwHP,
			m_data.dwSP,
			m_data.dwDef,
			m_data.dwHPApply,
			m_data.dwSPApply,
			m_data.dwDefApply,
			m_data.dwAgeApply,
			m_data.lExp,
			m_data.lItemExp,
			m_data.lBirthday,
			m_data.lEndTime,
			m_data.lMaxTime
		);

		static char buf[QUERY_MAX_LEN + 1];

		CDBManager::instance().EscapeString(buf, m_data.aSkill, sizeof(m_data.aSkill));
		snprintf(szQuery + queryLen, sizeof(szQuery) - queryLen, "skill_level = '%s' ", buf);

		CDBManager::instance().ReturnQuery(szQuery, QID_GROWTH_PET_SAVE, 0, NULL);
		m_bNeedQuery = false;
	}
}
#endif