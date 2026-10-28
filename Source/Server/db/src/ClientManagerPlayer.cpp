#include "stdafx.h"

#include "../../common/service.h"
#include "../../common/length.h"

#include "ClientManager.h"
#include "Main.h"
#include "QID.h"
#include "ItemAwardManager.h"
#include "Cache.h"

extern int g_test_server;
extern int g_log;

//
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// !!!!!!!!!!! IMPORTANT !!!!!!!!!!!!
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// Check all SELECT syntax on item table before change this function!!!
//
bool CreateItemTableFromRes(MYSQL_RES * res, std::vector<TPlayerItem> * pVec, DWORD dwPID)
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
		TPlayerItem & item = pVec->at(i);

		int cur = 0;

		// Check all SELECT syntax on item table before change this function!!!
		// Check all SELECT syntax on item table before change this function!!!
		// Check all SELECT syntax on item table before change this function!!!
		str_to_number(item.id, row[cur++]);
		str_to_number(item.window, row[cur++]);
		str_to_number(item.pos, row[cur++]);
		str_to_number(item.count, row[cur++]);
		str_to_number(item.vnum, row[cur++]);
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		str_to_number(item.dwTransmutationVnum, row[cur++]);
#endif
#ifdef ENABLE_SET_ITEM
		str_to_number(item.dwSetValue, row[cur++]);
#endif
		str_to_number(item.alSockets[0], row[cur++]);
		str_to_number(item.alSockets[1], row[cur++]);
		str_to_number(item.alSockets[2], row[cur++]);
#ifdef ENABLE_SEALBIND_SYSTEM
		str_to_number(item.sealbind, row[cur++]);
#endif

		for (int j = 0; j < ITEM_ATTRIBUTE_MAX_NUM; j++)
		{
			str_to_number(item.aAttr[j].bType, row[cur++]);
			str_to_number(item.aAttr[j].sValue, row[cur++]);
		}

		item.owner = dwPID;
	}

	return true;
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
bool CreateSkillColorTableFromRes(MYSQL_RES* res, DWORD* dwSkillColor)
{
	if (!res || mysql_num_rows(res) == 0)
		return false;

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
		return false;

	const unsigned int nFields = mysql_num_fields(res);

	for (BYTE bySlot = 0; bySlot < ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT; ++bySlot)
	{
		for (BYTE byLayer = 0; byLayer < ESkillColorLength::MAX_EFFECT_COUNT; ++byLayer)
		{
			const unsigned int idx = byLayer + (unsigned int)(bySlot * ESkillColorLength::MAX_EFFECT_COUNT);
			DWORD val = 0;
			if (idx < nFields && row[idx] != NULL)
				val = (DWORD)strtoul(row[idx], NULL, 10);
			*(dwSkillColor++) = val;
		}
	}

	return true;
}
#endif

size_t CreatePlayerSaveQuery(char * pszQuery, size_t querySize, TPlayerTable * pkTab)
{
	size_t queryLen;

	queryLen = snprintf(pszQuery, querySize,
			"UPDATE player%s SET "
			"job = %d, "
			"voice = %d, "
			"dir = %d, "
			"x = %d, "
			"y = %d, "
			"z = %d, "
			"map_index = %d, "
			"exit_x = %d, "
			"exit_y = %d, "
			"exit_map_index = %d, "
			"hp = %d, "
			"mp = %d, "
			"stamina = %d, "
			"random_hp = %d, "
			"random_sp = %d, "
			"playtime = %d, "
			"level = %d, "
			"level_step = %d, "
			"st = %d, "
			"ht = %d, "
			"dx = %d, "
			"iq = %d, "
#ifdef ENABLE_GOLD_LIMIT
			"gold = %lld, "
#else
			"gold = %d, "
#endif
#ifdef ENABLE_GAYA_SYSTEM
			"gem = %d, "
#endif
			"exp = %u, "
			"stat_point = %d, "
			"skill_point = %d, "
			"sub_skill_point = %d, "
			"stat_reset_count = %d, "
			"ip = '%s', "
			"part_main = %d, "
			"part_hair = %d, "
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			"part_acce = %d, "
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			"part_aura = %d, "
#endif
			"last_play = NOW(), "
			"skill_group = %d, "
			"alignment = %d, "
#ifdef ENABLE_CONQUEROR_LEVEL
			"conquerorlevel = %d, "
			"conqueror_level_step = %d, "
			"sungma_str = %d, "
			"sungma_hp = %d, "
			"sungma_move = %d, "
			"sungma_immune = %d, "
			"conqueror_exp = %u, "
			"conqueror_point = %d, "
#endif


			"horse_level = %d, "
			"horse_riding = %d, "
			"horse_hp = %d, "
			"horse_hp_droptime = %u, "
			"horse_stamina = %d, "
			"horse_skill_point = %d, "
#ifdef ENABLE_ANTI_EXP
			"anti_exp = %d, "
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
			"shop_flag = %llu, "
#endif
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
			"pick_up_mode = %d, "
#endif
#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
			"inventory_0 = %d, "
			"inventory_1 = %d, "
			"inventory_2 = %d, "
			"inventory_3 = %d, "
			"inventory_4 = %d, "
			"inventory_5 = %d, "
#endif
#ifdef ENABLE_RIDING_EXTENDED
			"mount_up_grade_exp = %d, "
			"mount_up_grade_fail = %d, "
#endif
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
			"soul = %d, "
			"soulre = %d, "
#endif
#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
			"halounlv = %d, "
			"phaloun = %lld, "
			"rhaloun = %d, "
#endif
#ifdef ENABLE_KILL_STATISTICS
			"statistics_jinno_kills = %d, "
			"statistics_shinsoo_kills = %d, "
			"statistics_chunjo_kills = %d, "
			"statistics_total_kills = %d, "
			"statistics_total_deaths = %d, "
			"statistics_duels_won = %d, "
			"statistics_duels_lost = %d, "
			"statistics_bosses_kills = %d, "
			"statistics_stones_kills = %d, "
			"statistics_mobs_kills = %d, "
			"top_damage = %d, "
#endif
#ifdef ENABLE_RANKING
			"r0 = %lld, "
			"r1 = %lld, "
			"r2 = %lld, "
			"r3 = %lld, "
			"r4 = %lld, "
			"r5 = %lld, "
			"r6 = %lld, "
			"r7 = %lld, "
			"r8 = %lld, "
			"r9 = %lld, "
			"r10 = %lld, "
			"r11 = %lld, "
			"r12 = %lld, "
			"r13 = %lld, "
			"r14 = %lld, "
			"r15 = %lld, "
			"r16 = %lld, "
			"r17 = %lld, "
			"r18 = %lld, "
			"r19 = %lld, "
			"r20 = %lld, "
#endif
			,
		GetTablePostfix(),
		pkTab->job,
		pkTab->voice,
		pkTab->dir,
		pkTab->x,
		pkTab->y,
		pkTab->z,
		pkTab->lMapIndex,
		pkTab->lExitX,
		pkTab->lExitY,
		pkTab->lExitMapIndex,
		pkTab->hp,
		pkTab->sp,
		pkTab->stamina,
		pkTab->sRandomHP,
		pkTab->sRandomSP,
		pkTab->playtime,
		pkTab->level,
		pkTab->level_step,
		pkTab->st,
		pkTab->ht,
		pkTab->dx,
		pkTab->iq,
		pkTab->gold,
#ifdef ENABLE_GAYA_SYSTEM
		pkTab->gem,
#endif
		pkTab->exp,
		pkTab->stat_point,
		pkTab->skill_point,
		pkTab->sub_skill_point,
		pkTab->stat_reset_count,
		pkTab->ip,
		pkTab->parts[PART_MAIN],
		pkTab->parts[PART_HAIR],
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		pkTab->parts[PART_ACCE],
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		pkTab->parts[PART_AURA],
#endif
		pkTab->skill_group,
		pkTab->lAlignment,
#ifdef ENABLE_CONQUEROR_LEVEL
		pkTab->conquerorlevel,
		pkTab->conqueror_level_step,
		pkTab->sungma_str,
		pkTab->sungma_hp,
		pkTab->sungma_move,
		pkTab->sungma_immune,
		pkTab->conqueror_exp,
		pkTab->conqueror_point,
#endif
		pkTab->horse.bLevel,
		pkTab->horse.bRiding,
		pkTab->horse.sHealth,
		pkTab->horse.dwHorseHealthDropTime,
		pkTab->horse.sStamina,
		pkTab->horse_skill_point
#ifdef ENABLE_ANTI_EXP
		,pkTab->anti_exp
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		,pkTab->shopFlag
#endif
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
		,pkTab->dwPickUPMode
#endif
#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
		,pkTab->inventory_unlock[0]
		,pkTab->inventory_unlock[1]
		,pkTab->inventory_unlock[2]
		,pkTab->inventory_unlock[3]
		,pkTab->inventory_unlock[4]
		,pkTab->inventory_unlock[5]
#endif
#ifdef ENABLE_RIDING_EXTENDED
		,pkTab->mount_up_grade_exp
		,pkTab->mount_up_grade_fail
#endif
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
		,pkTab->soul
		,pkTab->soulre
#endif
#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
		,pkTab->halounlv
		,pkTab->phaloun
		,pkTab->rhaloun
#endif
#ifdef ENABLE_KILL_STATISTICS
		, pkTab->iJinnoKills,
		pkTab->iShinsooKills,
		pkTab->iChunjoKills,
		pkTab->iTotalKills,
		pkTab->iTotalDeaths,
		pkTab->iDuelsWon,
		pkTab->iDuelsLost,
		pkTab->iBossesKills,
		pkTab->iStonesKills,
		pkTab->iMobsKills,
		pkTab->top_damage
#endif
#ifdef ENABLE_RANKING
		, pkTab->lRankPoints[0],
		pkTab->lRankPoints[1],
		pkTab->lRankPoints[2],
		pkTab->lRankPoints[3],
		pkTab->lRankPoints[4],
		pkTab->lRankPoints[5],
		pkTab->lRankPoints[6],
		pkTab->lRankPoints[7],
		pkTab->lRankPoints[8],
		pkTab->lRankPoints[9],
		pkTab->lRankPoints[10],
		pkTab->lRankPoints[11],
		pkTab->lRankPoints[12],
		pkTab->lRankPoints[13],
		pkTab->lRankPoints[14],
		pkTab->lRankPoints[15],
		pkTab->lRankPoints[16],
		pkTab->lRankPoints[17],
		pkTab->lRankPoints[18],
		pkTab->lRankPoints[19],
		pkTab->lRankPoints[20]
#endif
		);

	static char text[8192 + 1];

	CDBManager::instance().EscapeString(text, pkTab->skills, sizeof(pkTab->skills));
	queryLen += snprintf(pszQuery + queryLen, querySize - queryLen, "skill_level = '%s', ", text);

	CDBManager::instance().EscapeString(text, pkTab->quickslot, sizeof(pkTab->quickslot));
	queryLen += snprintf(pszQuery + queryLen, querySize - queryLen, "quickslot = '%s' ", text);

	queryLen += snprintf(pszQuery + queryLen, querySize - queryLen, " WHERE id=%d", pkTab->id);
	return queryLen;
}

CPlayerTableCache * CClientManager::GetPlayerCache(DWORD id)
{
	TPlayerTableCacheMap::iterator it = m_map_playerCache.find(id);

	if (it == m_map_playerCache.end())
		return NULL;

	TPlayerTable* pTable = it->second->Get(false);
	pTable->logoff_interval = GetCurrentTime() - it->second->GetLastUpdateTime();
	return it->second;
}

void CClientManager::PutPlayerCache(TPlayerTable * pNew)
{
	CPlayerTableCache * c;

	c = GetPlayerCache(pNew->id);

	if (!c)
	{
		c = new CPlayerTableCache;
		m_map_playerCache.insert(TPlayerTableCacheMap::value_type(pNew->id, c));
	}

	c->Put(pNew);
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
void CClientManager::QUERY_SKILL_COLOR_LOAD(CPeer* peer, DWORD dwHandle, TPlayerLoadPacket* packet)
{
	CSKillColorCache* c;
	TSkillColor* p;

	if ((c = GetSkillColorCache(packet->player_id)))
	{
		p = c->Get();
		peer->EncodeHeader(HEADER_DG_SKILL_COLOR_LOAD, dwHandle, sizeof(p->dwSkillColor));
		peer->Encode(p->dwSkillColor, sizeof(p->dwSkillColor));
	}
	else
	{
		char szQuery[QUERY_MAX_LEN];
#if !defined(ENABLE_NINETH_SKILL) && !defined(ENABLE_WOLFMAN_CHARACTER)
		snprintf(szQuery, sizeof(szQuery), "SELECT "
			"skillSlot1_Col1, skillSlot1_Col2, skillSlot1_Col3, skillSlot1_Col4, skillSlot1_Col5"
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
			" FROM skill_color%s WHERE player_id = %d",
			GetTablePostfix(), packet->player_id);
#elif !defined(ENABLE_NINETH_SKILL)
		snprintf(szQuery, sizeof(szQuery), "SELECT "
			"skillSlot1_Col1, skillSlot1_Col2, skillSlot1_Col3, skillSlot1_Col4, skillSlot1_Col5"
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
			" FROM skill_color%s WHERE player_id = %d",
			GetTablePostfix(), packet->player_id);
#elif !defined(ENABLE_WOLFMAN_CHARACTER)
		snprintf(szQuery, sizeof(szQuery), "SELECT "
			"skillSlot1_Col1, skillSlot1_Col2, skillSlot1_Col3, skillSlot1_Col4, skillSlot1_Col5"
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
			" FROM skill_color%s WHERE player_id = %d",
			GetTablePostfix(), packet->player_id);
#else
		snprintf(szQuery, sizeof(szQuery), "SELECT "
			"skillSlot1_Col1, skillSlot1_Col2, skillSlot1_Col3, skillSlot1_Col4, skillSlot1_Col5"
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
			" FROM skill_color%s WHERE player_id = %d",
			GetTablePostfix(), packet->player_id);
#endif
		CDBManager::instance().ReturnQuery(szQuery, QID_SKILL_COLOR, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id));
	}
}
#endif

/*
 * PLAYER LOAD
 */
void CClientManager::QUERY_PLAYER_LOAD(CPeer * peer, DWORD dwHandle, TPlayerLoadPacket * packet)
{
	CPlayerTableCache * c;
	TPlayerTable * pTab;

	CLoginData * pLoginData = GetLoginDataByAID(packet->account_id);

	if (pLoginData)
	{
		for (int n = 0; n < PLAYER_PER_ACCOUNT; ++n)
			if (pLoginData->GetAccountRef().players[n].dwID != 0)
				DeleteLogoutPlayer(pLoginData->GetAccountRef().players[n].dwID);
	}

	if ((c = GetPlayerCache(packet->player_id)))
	{
		CLoginData * pkLD = GetLoginDataByAID(packet->account_id);

		if (!pkLD || pkLD->IsPlay())
		{
			sys_log(0, "PLAYER_LOAD_ERROR: LoginData %p IsPlay %d", pkLD, pkLD ? pkLD->IsPlay() : 0);
			peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_FAILED, dwHandle, 0);
			return;
		}

		pTab = c->Get();

		pkLD->SetPlay(true);
		thecore_memcpy(pTab->aiPremiumTimes, pkLD->GetPremiumPtr(), sizeof(pTab->aiPremiumTimes));

		peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_SUCCESS, dwHandle, sizeof(TPlayerTable));
		peer->Encode(pTab, sizeof(TPlayerTable));

		if (packet->player_id != pkLD->GetLastPlayerID())
		{
			TPacketNeedLoginLogInfo logInfo;
			logInfo.dwPlayerID = packet->player_id;

			pkLD->SetLastPlayerID( packet->player_id );

			peer->EncodeHeader( HEADER_DG_NEED_LOGIN_LOG, dwHandle, sizeof(TPacketNeedLoginLogInfo) );
			peer->Encode( &logInfo, sizeof(TPacketNeedLoginLogInfo) );
		}

		char szQuery[1024] = { 0, };

		TItemCacheSet * pSet = GetItemCacheSet(pTab->id);

#ifdef ENABLE_GOLD_LIMIT
		sys_log(0, "[PLAYER_LOAD] ID %s pid %d gold %lld ", pTab->name, pTab->id, pTab->gold);
#else
		sys_log(0, "[PLAYER_LOAD] ID %s pid %d gold %d ", pTab->name, pTab->id, pTab->gold);
#endif

		if (pSet)
		{
			static std::vector<TPlayerItem> s_items;
			s_items.resize(pSet->size());

			DWORD dwCount = 0;
			TItemCacheSet::iterator it = pSet->begin();

			while (it != pSet->end())
			{
				CItemCache * c = *it++;
				TPlayerItem * p = c->Get();

				if (p->vnum)
					thecore_memcpy(&s_items[dwCount++], p, sizeof(TPlayerItem));
			}

			if (g_test_server)
				sys_log(0, "ITEM_CACHE: HIT! %s count: %u", pTab->name, dwCount);

			peer->EncodeHeader(HEADER_DG_ITEM_LOAD, dwHandle, sizeof(DWORD) + sizeof(TPlayerItem) * dwCount);
			peer->EncodeDWORD(dwCount);

			if (dwCount)
				peer->Encode(&s_items[0], sizeof(TPlayerItem) * dwCount);

			// Quest
			snprintf(szQuery, sizeof(szQuery),
					"SELECT dwPID,szName,szState,lValue FROM quest%s WHERE dwPID=%d AND lValue<>0",
					GetTablePostfix(), pTab->id);

			CDBManager::instance().ReturnQuery(szQuery, QID_QUEST, peer->GetHandle(), new ClientHandleInfo(dwHandle,0,packet->account_id));

			// Affect
			snprintf(szQuery, sizeof(szQuery),
					"SELECT dwPID,bType,bApplyOn,lApplyValue,dwFlag,lDuration,lSPCost"
					" FROM affect%s WHERE dwPID=%d",
					GetTablePostfix(), pTab->id);
			// @fixme402 ClientHandleInfo+pTab->id
			CDBManager::instance().ReturnQuery(szQuery, QID_AFFECT, peer->GetHandle(), new ClientHandleInfo(dwHandle, pTab->id));
		}

		else
		{
			snprintf(szQuery, sizeof(szQuery),
				"SELECT "
				"id,"
				"window+0,"
				"pos,"
				"count,"
				"vnum,"
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
				"transmutation,"
#endif
#ifdef ENABLE_SET_ITEM
				"set_value,"
#endif
				"socket0,socket1,socket2,"
#ifdef ENABLE_SEALBIND_SYSTEM
				"bind,"
#endif
				"attrtype0,attrvalue0,"
				"attrtype1,attrvalue1,"
				"attrtype2,attrvalue2,"
				"attrtype3,attrvalue3,"
				"attrtype4,attrvalue4,"
				"attrtype5,attrvalue5,"
				"attrtype6,attrvalue6 "
				"FROM item%s "

#ifdef ENABLE_RENEWAL_SWITCHBOT
				"WHERE owner_id=%d AND (`window` in ('INVENTORY','EQUIPMENT','DRAGON_SOUL_INVENTORY','BELT_INVENTORY','SWITCHBOT'))",
#else
				"WHERE owner_id=%d AND (`window` in ('INVENTORY','EQUIPMENT','DRAGON_SOUL_INVENTORY','BELT_INVENTORY'))",
#endif
				GetTablePostfix(), pTab->id);

			CDBManager::instance().ReturnQuery(szQuery, QID_ITEM, peer->GetHandle(), new ClientHandleInfo(dwHandle, pTab->id));
			snprintf(szQuery, sizeof(szQuery), "SELECT dwPID, szName, szState, lValue FROM quest%s WHERE dwPID=%d", GetTablePostfix(), pTab->id);

			CDBManager::instance().ReturnQuery(szQuery, QID_QUEST, peer->GetHandle(), new ClientHandleInfo(dwHandle, pTab->id));
			snprintf(szQuery, sizeof(szQuery), "SELECT dwPID, bType, bApplyOn, lApplyValue, dwFlag, lDuration, lSPCost FROM affect%s WHERE dwPID=%d", GetTablePostfix(), pTab->id);

			CDBManager::instance().ReturnQuery(szQuery, QID_AFFECT, peer->GetHandle(), new ClientHandleInfo(dwHandle, pTab->id));
		}
	}
	else
	{
		sys_log(0, "[PLAYER_LOAD] Load from PlayerDB pid[%d]", packet->player_id);

		char queryStr[QUERY_MAX_LEN];

		snprintf(queryStr, sizeof(queryStr),
				"SELECT "
				"id, "
				"name, "
				"job, "
				"voice, "
				"dir, "
				"x, "
				"y, "
				"z, "
				"map_index, "
				"exit_x, "
				"exit_y, "
				"exit_map_index, "
				"hp, "
				"mp, "
				"stamina, "
				"random_hp, "
				"random_sp, "
				"playtime, "
				"gold, "
#ifdef ENABLE_GAYA_SYSTEM
				"gem, "
#endif
				"level, "
				"level_step, "
				"st, "
				"ht, "
				"dx, "
				"iq, "
				"exp, "
				"stat_point, "
				"skill_point, "
				"sub_skill_point, "
				"stat_reset_count, "
				"part_base, "
				"part_hair, "
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
				"part_acce, "
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
				"part_aura, "
#endif
				"skill_level, "
				"quickslot, "
				"skill_group, "
				"alignment, "
#ifdef ENABLE_CONQUEROR_LEVEL
				"conquerorlevel, "
				"conqueror_level_step, "
				"sungma_str, "
				"sungma_hp, "
				"sungma_move, "
				"sungma_immune, "
				"conqueror_exp, "
				"conqueror_point, "
#endif
				"horse_level, "
				"horse_riding, "
				"horse_hp, "
				"horse_hp_droptime, "
				"horse_stamina, "
				"UNIX_TIMESTAMP(NOW())-UNIX_TIMESTAMP(last_play), "
				"horse_skill_point"
#ifdef ENABLE_ANTI_EXP
				",anti_exp "
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
				",shop_flag "
#endif
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
				",pick_up_mode "
#endif
#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
				",inventory_0 "
				",inventory_1 "
				",inventory_2 "
				",inventory_3 "
				",inventory_4 "
				",inventory_5 "
#endif
#ifdef ENABLE_RIDING_EXTENDED
				",mount_up_grade_exp "
				",mount_up_grade_fail "
#endif
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
				",soul "
				",soulre "
#endif
#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
				",halounlv "
				",phaloun "
				",rhaloun "
#endif
#ifdef ENABLE_KILL_STATISTICS
				", statistics_jinno_kills, "
				"statistics_shinsoo_kills, "
				"statistics_chunjo_kills, "
				"statistics_total_kills, "
				"statistics_total_deaths, "
				"statistics_duels_won, "
				"statistics_duels_lost, "
				"statistics_bosses_kills, "
				"statistics_stones_kills, "
				"statistics_mobs_kills, "
				"top_damage "
#endif
#ifdef ENABLE_RANKING
				", r0, "
				"r1, "
				"r2, "
				"r3, "
				"r4, "
				"r5, "
				"r6, "
				"r7, "
				"r8, "
				"r9, "
				"r10, "
				"r11, "
				"r12, "
				"r13, "
				"r14, "
				"r15, "
				"r16, "
				"r17, "
				"r18, "
				"r19, "
				"r20 "
#endif
				"FROM player%s WHERE id=%d"
				,
				GetTablePostfix(),
				packet->player_id
				);

		ClientHandleInfo * pkInfo = new ClientHandleInfo(dwHandle, packet->player_id);
		pkInfo->account_id = packet->account_id;
		CDBManager::instance().ReturnQuery(queryStr, QID_PLAYER, peer->GetHandle(), pkInfo);

		snprintf(queryStr, sizeof(queryStr),
				"SELECT "
				"id,"
				"window+0,"
				"pos,"
				"count,"
				"vnum,"
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
				"transmutation,"
#endif
#ifdef ENABLE_SET_ITEM
				"set_value,"
#endif
				"socket0,socket1,socket2,"
#ifdef ENABLE_SEALBIND_SYSTEM
				"bind,"
#endif
				"attrtype0,attrvalue0,"
				"attrtype1,attrvalue1,"
				"attrtype2,attrvalue2,"
				"attrtype3,attrvalue3,"
				"attrtype4,attrvalue4,"
				"attrtype5,attrvalue5,"
				"attrtype6,attrvalue6 "
				"FROM item%s "

#ifdef ENABLE_RENEWAL_SWITCHBOT
				"WHERE owner_id=%d AND (`window` in ('INVENTORY','EQUIPMENT','DRAGON_SOUL_INVENTORY','BELT_INVENTORY','SWITCHBOT'))",
#else
				"WHERE owner_id=%d AND (`window` in ('INVENTORY','EQUIPMENT','DRAGON_SOUL_INVENTORY','BELT_INVENTORY'))",
#endif
				GetTablePostfix(), packet->player_id);

		CDBManager::instance().ReturnQuery(queryStr, QID_ITEM, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id));

		snprintf(queryStr, sizeof(queryStr), "SELECT dwPID,szName,szState,lValue FROM quest%s WHERE dwPID=%d", GetTablePostfix(), packet->player_id);
		CDBManager::instance().ReturnQuery(queryStr, QID_QUEST, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id,packet->account_id));

		snprintf(queryStr, sizeof(queryStr), "SELECT dwPID,bType,bApplyOn,lApplyValue,dwFlag,lDuration,lSPCost FROM affect%s WHERE dwPID=%d", GetTablePostfix(), packet->player_id);
		CDBManager::instance().ReturnQuery(queryStr, QID_AFFECT, peer->GetHandle(), new ClientHandleInfo(dwHandle, packet->player_id));
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	TGrowthPetCacheSet* pSet = GetGrowthPetCacheSet(packet->player_id);
	if (pSet)
	{
		static std::vector<TGrowthPet> s_petVec;
		s_petVec.resize(pSet->size());

		DWORD dwCount = 0;
		auto it = pSet->begin();

		while (it != pSet->end())
		{
			CGrowthPetCache* c = *it++;
			TGrowthPet* p = c->Get();

			if (p->dwOwner)
				thecore_memcpy(&s_petVec[dwCount++], p, sizeof(TGrowthPet));
		}

		peer->EncodeHeader(HEADER_DG_GROWTH_PET_LOAD, dwHandle, sizeof(DWORD) + sizeof(TGrowthPet) * dwCount);
		peer->EncodeDWORD(dwCount);

		if (dwCount)
			peer->Encode(&s_petVec[0], sizeof(TGrowthPet) * dwCount);

	}
	else
	{
		char szQuery[QUERY_MAX_LEN];
		snprintf(szQuery, sizeof(szQuery),
			"SELECT id,vnum,state+0,name,size,level,level_step,evolution,type,hp,sp,def,hp_apply,sp_apply,def_apply,"
			"age_apply,skill_level,exp,item_exp,UNIX_TIMESTAMP(birthday),end_time,max_time "
			"FROM growth_pet%s WHERE owner_id=%d AND state < %d",
			GetTablePostfix(), packet->player_id, STATE_SAFEBOX);

		CDBManager::instance().ReturnQuery(szQuery,
			QID_GROWTH_PET,
			peer->GetHandle(),
			new ClientHandleInfo(dwHandle, packet->player_id));
	}
#endif
}

void CClientManager::ItemAward(CPeer * peer,char* login)
{
	char login_t[LOGIN_MAX_LEN + 1] = "";
	strlcpy(login_t,login,LOGIN_MAX_LEN + 1);
	std::set<TItemAward *> * pSet = ItemAwardManager::instance().GetByLogin(login_t);
	if(pSet == NULL)
		return;
	__typeof(pSet->begin()) it = pSet->begin();
	while(it != pSet->end() )
	{
		TItemAward * pItemAward = *(it++);
		char* whyStr = pItemAward->szWhy; //why colum read
		char cmdStr[100] = ""; //Copy the value read from the why Colum into a temporary string
		strlcpy(cmdStr, whyStr, sizeof(cmdStr)); //If you write a token in the process of obtaining the command, the original is also tokenized.
		char command[20] = "";
		// @fixme203 directly GetCommand instead of strcpy
		GetCommand(cmdStr, command); // Get command
		if( !(strcmp(command,"GIFT") )) // If command is GIFT
		{
			TPacketItemAwardInfromer giftData;
			strlcpy(giftData.login, pItemAward->szLogin, sizeof(giftData.login)); //Login ID Copy
			strlcpy(giftData.command, command, sizeof(giftData.command)); //Command copy
			giftData.vnum = pItemAward->dwVnum; //Copy item vnum too
			ForwardPacket(HEADER_DG_ITEMAWARD_INFORMER,&giftData,sizeof(TPacketItemAwardInfromer));
		}
	}
}
char* CClientManager::GetCommand(char* str, char* command) // @fixme203
{
	char* tok;

	if( str[0] == '[' )
	{
		tok = strtok(str,"]");
		if (tok)
			strlcat(command, &tok[1], 20); // command buffers are char[20] at every call site
	}

	return command;
}

bool CreatePlayerTableFromRes(MYSQL_RES * res, TPlayerTable * pkTab)
{
	if (mysql_num_rows(res) == 0) // no data
		return false;

	memset(pkTab, 0, sizeof(TPlayerTable));

	MYSQL_ROW row = mysql_fetch_row(res);

	int	col = 0;

	// "id,name,job,voice,dir,x,y,z,map_index,exit_x,exit_y,exit_map_index,hp,mp,stamina,random_hp,random_sp,playtime,"
	// "gold,level,level_step,st,ht,dx,iq,exp,"
	// "stat_point,skill_point,sub_skill_point,stat_reset_count,part_base,part_hair,"
	// "skill_level,quickslot,skill_group,alignment,mobile,horse_level,horse_riding,horse_hp,horse_stamina FROM player%s WHERE id=%d",
	str_to_number(pkTab->id, row[col++]);
	strlcpy(pkTab->name, row[col++], sizeof(pkTab->name));
	str_to_number(pkTab->job, row[col++]);
	str_to_number(pkTab->voice, row[col++]);
	str_to_number(pkTab->dir, row[col++]);
	str_to_number(pkTab->x, row[col++]);
	str_to_number(pkTab->y, row[col++]);
	str_to_number(pkTab->z, row[col++]);
	str_to_number(pkTab->lMapIndex, row[col++]);
	str_to_number(pkTab->lExitX, row[col++]);
	str_to_number(pkTab->lExitY, row[col++]);
	str_to_number(pkTab->lExitMapIndex,  row[col++]);
	str_to_number(pkTab->hp, row[col++]);
	str_to_number(pkTab->sp, row[col++]);
	str_to_number(pkTab->stamina, row[col++]);
	str_to_number(pkTab->sRandomHP, row[col++]);
	str_to_number(pkTab->sRandomSP, row[col++]);
	str_to_number(pkTab->playtime, row[col++]);
	str_to_number(pkTab->gold, row[col++]);
#ifdef ENABLE_GAYA_SYSTEM
	str_to_number(pkTab->gem, row[col++]);
#endif
	str_to_number(pkTab->level, row[col++]);
	str_to_number(pkTab->level_step, row[col++]);
	str_to_number(pkTab->st, row[col++]);
	str_to_number(pkTab->ht, row[col++]);
	str_to_number(pkTab->dx, row[col++]);
	str_to_number(pkTab->iq, row[col++]);
	str_to_number(pkTab->exp, row[col++]);
	str_to_number(pkTab->stat_point, row[col++]);
	str_to_number(pkTab->skill_point, row[col++]);
	str_to_number(pkTab->sub_skill_point, row[col++]);
	str_to_number(pkTab->stat_reset_count, row[col++]);
	str_to_number(pkTab->part_base, row[col++]);
	str_to_number(pkTab->parts[PART_HAIR], row[col++]);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	str_to_number(pkTab->parts[PART_ACCE], row[col++]);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	str_to_number(pkTab->parts[PART_AURA], row[col++]);
#endif

	if (row[col])
		thecore_memcpy(pkTab->skills, row[col], sizeof(pkTab->skills));
	else
		memset(&pkTab->skills, 0, sizeof(pkTab->skills));

	col++;

	if (row[col])
		thecore_memcpy(pkTab->quickslot, row[col], sizeof(pkTab->quickslot));
	else
		memset(pkTab->quickslot, 0, sizeof(pkTab->quickslot));

	col++;

	str_to_number(pkTab->skill_group, row[col++]);
	str_to_number(pkTab->lAlignment, row[col++]);
#ifdef ENABLE_CONQUEROR_LEVEL
	str_to_number(pkTab->conquerorlevel, row[col++]);
	str_to_number(pkTab->conqueror_level_step, row[col++]);
	str_to_number(pkTab->sungma_str, row[col++]);
	str_to_number(pkTab->sungma_hp, row[col++]);
	str_to_number(pkTab->sungma_move, row[col++]);
	str_to_number(pkTab->sungma_immune, row[col++]);
	str_to_number(pkTab->conqueror_exp, row[col++]);
	str_to_number(pkTab->conqueror_point, row[col++]);
#endif
	str_to_number(pkTab->horse.bLevel, row[col++]);
	str_to_number(pkTab->horse.bRiding, row[col++]);
	str_to_number(pkTab->horse.sHealth, row[col++]);
	str_to_number(pkTab->horse.dwHorseHealthDropTime, row[col++]);
	str_to_number(pkTab->horse.sStamina, row[col++]);
	str_to_number(pkTab->logoff_interval, row[col++]);
	str_to_number(pkTab->horse_skill_point, row[col++]);
#ifdef ENABLE_ANTI_EXP
	str_to_number(pkTab->anti_exp, row[col++]);
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	str_to_number(pkTab->shopFlag, row[col++]);
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	str_to_number(pkTab->dwPickUPMode, row[col++]);
#endif

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	str_to_number(pkTab->inventory_unlock[0], row[col++]);
	str_to_number(pkTab->inventory_unlock[1], row[col++]);
	str_to_number(pkTab->inventory_unlock[2], row[col++]);
	str_to_number(pkTab->inventory_unlock[3], row[col++]);
	str_to_number(pkTab->inventory_unlock[4], row[col++]);
	str_to_number(pkTab->inventory_unlock[5], row[col++]);
#endif

#ifdef ENABLE_RIDING_EXTENDED
	str_to_number(pkTab->mount_up_grade_exp, row[col++]);
	str_to_number(pkTab->mount_up_grade_fail, row[col++]);
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	str_to_number(pkTab->soul, row[col++]);
	str_to_number(pkTab->soulre, row[col++]);
#endif

#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
	str_to_number(pkTab->halounlv, row[col++]);
	str_to_number(pkTab->phaloun, row[col++]);
	str_to_number(pkTab->rhaloun, row[col++]);
#endif

#ifdef ENABLE_KILL_STATISTICS
	str_to_number(pkTab->iJinnoKills, row[col++]);
	str_to_number(pkTab->iShinsooKills, row[col++]);
	str_to_number(pkTab->iChunjoKills, row[col++]);
	str_to_number(pkTab->iTotalKills, row[col++]);
	str_to_number(pkTab->iTotalDeaths, row[col++]);
	str_to_number(pkTab->iDuelsWon, row[col++]);
	str_to_number(pkTab->iDuelsLost, row[col++]);
	str_to_number(pkTab->iBossesKills, row[col++]);
	str_to_number(pkTab->iStonesKills, row[col++]);
	str_to_number(pkTab->iMobsKills, row[col++]);
	str_to_number(pkTab->top_damage, row[col++]);
#endif

#ifdef ENABLE_RANKING
	for (int i = 0; i < RANKING_MAX_CATEGORIES; ++i)
		str_to_number(pkTab->lRankPoints[i], row[col++]);
#endif

	// reset sub_skill_point
	{
		pkTab->skills[123].bLevel = 0; // SKILL_CREATE

		if (pkTab->level > 9)
		{
			int max_point = pkTab->level - 9;

			int skill_point =
				MIN(20, pkTab->skills[121].bLevel) +	// SKILL_LEADERSHIP
				MIN(20, pkTab->skills[124].bLevel) +	// SKILL_MINING
				MIN(10, pkTab->skills[131].bLevel) +	// SKILL_HORSE_SUMMON
				MIN(20, pkTab->skills[141].bLevel) +	// SKILL_ADD_HP
				MIN(20, pkTab->skills[142].bLevel);		// SKILL_RESIST_PENETRATE

			pkTab->sub_skill_point = max_point - skill_point;
		}
		else
			pkTab->sub_skill_point = 0;
	}

	return true;
}

void CClientManager::RESULT_COMPOSITE_PLAYER(CPeer * peer, SQLMsg * pMsg, DWORD dwQID)
{
	CQueryInfo * qi = (CQueryInfo *) pMsg->pvUserData;
	std::unique_ptr<ClientHandleInfo> info((ClientHandleInfo *) qi->pvData);

	MYSQL_RES * pSQLResult = pMsg->Get()->pSQLResult;
	if (!pSQLResult)
	{
		sys_err("null MYSQL_RES QID %u", dwQID);
		return;
	}

	switch (dwQID)
	{
		case QID_PLAYER:
			sys_log(0, "QID_PLAYER %u %u", info->dwHandle, info->player_id);
			RESULT_PLAYER_LOAD(peer, pSQLResult, info.get());

			break;

		case QID_ITEM:
			sys_log(0, "QID_ITEM %u", info->dwHandle);
			RESULT_ITEM_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
			break;

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		case QID_SKILL_COLOR:
			sys_log(0, "QID_SKILL_COLOR %u  %u", info->dwHandle, info->player_id);
			RESULT_SKILL_COLOR_LOAD(peer, pSQLResult, info->dwHandle);
			break;
#endif

        case QID_QUEST:
        {
            sys_log(0, "QID_QUEST %u", info->dwHandle);
            RESULT_QUEST_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);

            ClientHandleInfo* temp1 = info.get();
            if (temp1 == NULL)
            {
                break;
            }

            CLoginData* pLoginData1 = GetLoginDataByAID(temp1->account_id);
            if (pLoginData1 == NULL)
            {
                break;
            }

            const char* login = pLoginData1->GetAccountRef().login;
            if (!login)
            {
                break;
            }

            sys_log(0, "info of pLoginData1 before call ItemAwardfunction %d", pLoginData1);
            ItemAward(peer, pLoginData1->GetAccountRef().login);
            break;
        }

		case QID_AFFECT:
			sys_log(0, "QID_AFFECT %u", info->dwHandle);
			// @fixme402 RESULT_AFFECT_LOAD+info->player_id
			RESULT_AFFECT_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
			break;

#ifdef ENABLE_GROWTH_PET_SYSTEM
		case QID_GROWTH_PET:
			sys_log(0, "QID_GROWTH_PET %u", info->dwHandle);
			RESULT_GROWTH_PET_LOAD(peer, pSQLResult, info->dwHandle, info->player_id);
			break;
#endif
	}
}

void CClientManager::RESULT_PLAYER_LOAD(CPeer * peer, MYSQL_RES * pRes, ClientHandleInfo * pkInfo)
{
	TPlayerTable tab;

	if (!CreatePlayerTableFromRes(pRes, &tab))
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_FAILED, pkInfo->dwHandle, 0);
		return;
	}

	CLoginData * pkLD = GetLoginDataByAID(pkInfo->account_id);

	if (!pkLD || pkLD->IsPlay())
	{
		sys_log(0, "PLAYER_LOAD_ERROR: LoginData %p IsPlay %d", pkLD, pkLD ? pkLD->IsPlay() : 0);
		peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_FAILED, pkInfo->dwHandle, 0);
		return;
	}

	pkLD->SetPlay(true);
	thecore_memcpy(tab.aiPremiumTimes, pkLD->GetPremiumPtr(), sizeof(tab.aiPremiumTimes));

	peer->EncodeHeader(HEADER_DG_PLAYER_LOAD_SUCCESS, pkInfo->dwHandle, sizeof(TPlayerTable));
	peer->Encode(&tab, sizeof(TPlayerTable));

	if (tab.id != pkLD->GetLastPlayerID())
	{
		TPacketNeedLoginLogInfo logInfo;
		logInfo.dwPlayerID = tab.id;

		pkLD->SetLastPlayerID( tab.id );

		peer->EncodeHeader( HEADER_DG_NEED_LOGIN_LOG, pkInfo->dwHandle, sizeof(TPacketNeedLoginLogInfo) );
		peer->Encode( &logInfo, sizeof(TPacketNeedLoginLogInfo) );
	}
}

void CClientManager::RESULT_ITEM_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwPID)
{
	static std::vector<TPlayerItem> s_items;
	// DB
	CreateItemTableFromRes(pRes, &s_items, dwPID);
	DWORD dwCount = s_items.size();

	peer->EncodeHeader(HEADER_DG_ITEM_LOAD, dwHandle, sizeof(DWORD) + sizeof(TPlayerItem) * dwCount);
	peer->EncodeDWORD(dwCount);

	// Create a CacheSet
	CreateItemCacheSet(dwPID);

	// ITEM_LOAD_LOG_ATTACH_PID
	sys_log(0, "ITEM_LOAD: count %u pid %u", dwCount, dwPID);
	// END_OF_ITEM_LOAD_LOG_ATTACH_PID

	if (dwCount)
	{
		peer->Encode(&s_items[0], sizeof(TPlayerItem) * dwCount);

		for (DWORD i = 0; i < dwCount; ++i)
			PutItemCache(&s_items[i], true); // Since there is no need to save the loaded one, put true in the argument bSkipQuery.
	}
}

#ifdef ENABLE_SKILL_COLOR_SYSTEM
void CClientManager::RESULT_SKILL_COLOR_LOAD(CPeer* peer, MYSQL_RES* pRes, DWORD dwHandle)
{
	DWORD dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
	memset(dwSkillColor, 0, sizeof(dwSkillColor));

	CreateSkillColorTableFromRes(pRes, *dwSkillColor);
	sys_log(0, "SKILL_COLOR_LOAD (SkillSlot1) %i, %i, %i, %i, %i", dwSkillColor[0][0], dwSkillColor[0][1], dwSkillColor[0][2], dwSkillColor[0][3], dwSkillColor[0][4]);
	sys_log(0, "SKILL_COLOR_LOAD (SkillSlot2) %i, %i, %i, %i, %i", dwSkillColor[1][0], dwSkillColor[1][1], dwSkillColor[1][2], dwSkillColor[1][3], dwSkillColor[1][4]);
	sys_log(0, "SKILL_COLOR_LOAD (SkillSlot3) %i, %i, %i, %i, %i", dwSkillColor[2][0], dwSkillColor[2][1], dwSkillColor[2][2], dwSkillColor[2][3], dwSkillColor[2][4]);
	sys_log(0, "SKILL_COLOR_LOAD (SkillSlot4) %i, %i, %i, %i, %i", dwSkillColor[3][0], dwSkillColor[3][1], dwSkillColor[3][2], dwSkillColor[3][3], dwSkillColor[3][4]);
	sys_log(0, "SKILL_COLOR_LOAD (SkillSlot5) %i, %i, %i, %i, %i", dwSkillColor[4][0], dwSkillColor[4][1], dwSkillColor[4][2], dwSkillColor[4][3], dwSkillColor[4][4]);
	sys_log(0, "SKILL_COLOR_LOAD (SkillSlot6) %i, %i, %i, %i, %i", dwSkillColor[5][0], dwSkillColor[5][1], dwSkillColor[5][2], dwSkillColor[5][3], dwSkillColor[5][4]);
	sys_log(0, "SKILL_COLOR_LOAD (SkillSlot7) %i, %i, %i, %i, %i", dwSkillColor[6][0], dwSkillColor[6][1], dwSkillColor[6][2], dwSkillColor[6][3], dwSkillColor[6][4]);
	sys_log(0, "SKILL_COLOR_LOAD (SkillSlot8) %i, %i, %i, %i, %i", dwSkillColor[7][0], dwSkillColor[7][1], dwSkillColor[7][2], dwSkillColor[7][3], dwSkillColor[7][4]);
	sys_log(0, "SKILL_COLOR_LOAD (SkillSlot9) %i, %i, %i, %i, %i", dwSkillColor[8][0], dwSkillColor[8][1], dwSkillColor[8][2], dwSkillColor[8][3], dwSkillColor[8][4]);
	sys_log(0, "SKILL_COLOR_LOAD (SkillSlot10) %i, %i, %i, %i, %i", dwSkillColor[9][0], dwSkillColor[9][1], dwSkillColor[9][2], dwSkillColor[9][3], dwSkillColor[9][4]);
	sys_log(0, "SKILL_COLOR_LOAD (BuffSkill1) %i, %i, %i, %i, %i", dwSkillColor[10][0], dwSkillColor[10][1], dwSkillColor[10][2], dwSkillColor[10][3], dwSkillColor[10][4]);
	sys_log(0, "SKILL_COLOR_LOAD (BuffSkill2) %i, %i, %i, %i, %i", dwSkillColor[11][0], dwSkillColor[11][1], dwSkillColor[11][2], dwSkillColor[11][3], dwSkillColor[11][4]);
	sys_log(0, "SKILL_COLOR_LOAD (BuffSkill3) %i, %i, %i, %i, %i", dwSkillColor[12][0], dwSkillColor[12][1], dwSkillColor[12][2], dwSkillColor[12][3], dwSkillColor[12][4]);
	sys_log(0, "SKILL_COLOR_LOAD (BuffSkill4) %i, %i, %i, %i, %i", dwSkillColor[13][0], dwSkillColor[13][1], dwSkillColor[13][2], dwSkillColor[13][3], dwSkillColor[13][4]);
	sys_log(0, "SKILL_COLOR_LOAD (BuffSkill5) %i, %i, %i, %i, %i", dwSkillColor[14][0], dwSkillColor[14][1], dwSkillColor[14][2], dwSkillColor[14][3], dwSkillColor[14][4]);
	sys_log(0, "SKILL_COLOR_LOAD (BuffSkill6) %i, %i, %i, %i, %i", dwSkillColor[15][0], dwSkillColor[15][1], dwSkillColor[15][2], dwSkillColor[15][3], dwSkillColor[15][4]);
	peer->EncodeHeader(HEADER_DG_SKILL_COLOR_LOAD, dwHandle, sizeof(dwSkillColor));
	peer->Encode(&dwSkillColor, sizeof(dwSkillColor));
}
#endif

// @fixme402 (RESULT_AFFECT_LOAD +dwRealPID)
void CClientManager::RESULT_AFFECT_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD dwRealPID)
{
	int iNumRows;

	if ((iNumRows = mysql_num_rows(pRes)) == 0)
	{
		// @fixme402 begin
		static DWORD dwPID;
		static DWORD dwCount = 0; //1;
		static TPacketAffectElement paeTable = {0};

		dwPID = dwRealPID;
		sys_log(0, "AFFECT_LOAD: count %u PID %u RealPID %u", dwCount, dwPID, dwRealPID);

		peer->EncodeHeader(HEADER_DG_AFFECT_LOAD, dwHandle, sizeof(DWORD) + sizeof(DWORD) + sizeof(TPacketAffectElement) * dwCount);
		peer->Encode(&dwPID, sizeof(DWORD));
		peer->Encode(&dwCount, sizeof(DWORD));
		peer->Encode(&paeTable, sizeof(TPacketAffectElement) * dwCount);
		// @fixme402 end
		return;
	}

	static std::vector<TPacketAffectElement> s_elements;
	s_elements.resize(iNumRows);

	DWORD dwPID = 0;

	MYSQL_ROW row;

	for (int i = 0; i < iNumRows; ++i)
	{
		TPacketAffectElement & r = s_elements[i];
		row = mysql_fetch_row(pRes);

		if (dwPID == 0)
			str_to_number(dwPID, row[0]);

		str_to_number(r.dwType, row[1]);
		str_to_number(r.bApplyOn, row[2]);
		str_to_number(r.lApplyValue, row[3]);
		str_to_number(r.dwFlag, row[4]);
		str_to_number(r.lDuration, row[5]);
		str_to_number(r.lSPCost, row[6]);
	}

	sys_log(0, "AFFECT_LOAD: count %d PID %u", s_elements.size(), dwPID);

	DWORD dwCount = s_elements.size();

	peer->EncodeHeader(HEADER_DG_AFFECT_LOAD, dwHandle, sizeof(DWORD) + sizeof(DWORD) + sizeof(TPacketAffectElement) * dwCount);
	peer->Encode(&dwPID, sizeof(DWORD));
	peer->Encode(&dwCount, sizeof(DWORD));
	peer->Encode(&s_elements[0], sizeof(TPacketAffectElement) * dwCount);
}

void CClientManager::RESULT_QUEST_LOAD(CPeer * peer, MYSQL_RES * pRes, DWORD dwHandle, DWORD pid)
{
	int iNumRows;

	if ((iNumRows = mysql_num_rows(pRes)) == 0)
	{
		DWORD dwCount = 0;
		peer->EncodeHeader(HEADER_DG_QUEST_LOAD, dwHandle, sizeof(DWORD));
		peer->Encode(&dwCount, sizeof(DWORD));
		return;
	}

	static std::vector<TQuestTable> s_table;
	s_table.resize(iNumRows);

	MYSQL_ROW row;

	for (int i = 0; i < iNumRows; ++i)
	{
		TQuestTable & r = s_table[i];

		row = mysql_fetch_row(pRes);

		str_to_number(r.dwPID, row[0]);
		strlcpy(r.szName, row[1], sizeof(r.szName));
		strlcpy(r.szState, row[2], sizeof(r.szState));
		str_to_number(r.lValue, row[3]);
	}

	sys_log(0, "QUEST_LOAD: count %d PID %u", s_table.size(), s_table[0].dwPID);

	DWORD dwCount = s_table.size();

	peer->EncodeHeader(HEADER_DG_QUEST_LOAD, dwHandle, sizeof(DWORD) + sizeof(TQuestTable) * dwCount);
	peer->Encode(&dwCount, sizeof(DWORD));
	peer->Encode(&s_table[0], sizeof(TQuestTable) * dwCount);
}

/*
 * PLAYER SAVE
 */

void CClientManager::QUERY_PLAYER_SAVE(CPeer * peer, DWORD dwHandle, TPlayerTable * pkTab)
{
	if (g_test_server)
		sys_log(0, "PLAYER_SAVE: %s", pkTab->name);

	PutPlayerCache(pkTab);
}

typedef std::map<DWORD, time_t> time_by_id_map_t;
static time_by_id_map_t s_createTimeByAccountID;

/*
 * PLAYER CREATE
 */
void CClientManager::__QUERY_PLAYER_CREATE(CPeer *peer, DWORD dwHandle, TPlayerCreatePacket* packet)
{
	char	queryStr[QUERY_MAX_LEN];
	int		queryLen;
	int		player_id;

	time_by_id_map_t::iterator it = s_createTimeByAccountID.find(packet->account_id);

	if (it != s_createTimeByAccountID.end())
	{
		time_t curtime = time(0);

		if (curtime - it->second < 30)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
			return;
		}
	}

	queryLen = snprintf(queryStr, sizeof(queryStr),
			"SELECT pid%u FROM player_index%s WHERE id=%d", packet->account_index + 1, GetTablePostfix(), packet->account_id);

	std::unique_ptr<SQLMsg> pMsg0(CDBManager::instance().DirectQuery(queryStr));

	if (pMsg0->Get()->uiNumRows != 0)
	{
		if (!pMsg0->Get()->pSQLResult)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
			return;
		}

		MYSQL_ROW row = mysql_fetch_row(pMsg0->Get()->pSQLResult);

		DWORD dwPID = 0; str_to_number(dwPID, row[0]);
		if (row[0] && dwPID > 0)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_ALREADY, dwHandle, 0);
			sys_log(0, "ALREADY EXIST AccountChrIdx %d ID %d", packet->account_index, dwPID);
			return;
		}
	}
	else
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
		return;
	}

	snprintf(queryStr, sizeof(queryStr), "SELECT COUNT(*) as count FROM player%s WHERE name='%s'", GetTablePostfix(), packet->player_table.name);

	std::unique_ptr<SQLMsg> pMsg1(CDBManager::instance().DirectQuery(queryStr));

	if (pMsg1->Get()->uiNumRows)
	{
		if (!pMsg1->Get()->pSQLResult)
		{
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
			return;
		}

		MYSQL_ROW row = mysql_fetch_row(pMsg1->Get()->pSQLResult);

		if (*row[0] != '0')
		{
			sys_log(0, "ALREADY EXIST name %s, row[0] %s query %s", packet->player_table.name, row[0], queryStr);
			peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_ALREADY, dwHandle, 0);
			return;
		}
	}
	else
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
		return;
	}

	static char text[4096 + 1];

	queryLen = snprintf(queryStr, sizeof(queryStr),
			"INSERT INTO player%s ("
			"id, "
			"account_id, "
			"name, "
			"level, "
			"st, "
			"ht, "
			"dx, "
			"iq, "
			"job, "
			"voice, "
			"dir, "
			"x, "
			"y, "
			"z, "
			"hp, "
			"mp, "
			"random_hp, "
			"random_sp, "
			"stat_point, "
			"stamina, "
			"part_base, "
			"part_main, "
			"part_hair, "
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			"part_acce, "
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			"part_aura, "
#endif
			"gold, "
#ifdef ENABLE_GAYA_SYSTEM
			"gem, "
#endif
			"playtime, "
#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
			"halounlv, phaloun, rhaloun, "
#endif
			"skill_level, "
			"quickslot"
			") VALUES("
			"0, "
			"%u, "
			"'%s', "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"%d, "
			"0, "
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			"0, "
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			"0, "
#endif
#ifdef ENABLE_GOLD_LIMIT
			"%lld, "
#else
			"%d, "
#endif
#ifdef ENABLE_GAYA_SYSTEM
			"%d, "
#endif
			"0, "
#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
			"%d, %lld, %d, "
#endif
			,
			GetTablePostfix(),
			packet->account_id,
			packet->player_table.name,
			packet->player_table.level,
			packet->player_table.st,
			packet->player_table.ht,
			packet->player_table.dx,
			packet->player_table.iq,
			packet->player_table.job,
			packet->player_table.voice,
			packet->player_table.dir,
			packet->player_table.x,
			packet->player_table.y,
			packet->player_table.z,
			packet->player_table.hp,
			packet->player_table.sp,
			packet->player_table.sRandomHP,
			packet->player_table.sRandomSP,
			packet->player_table.stat_point,
			packet->player_table.stamina,
			packet->player_table.part_base,
			packet->player_table.part_base,
			packet->player_table.gold
#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
			,packet->player_table.halounlv, packet->player_table.phaloun, packet->player_table.rhaloun
#endif
			);

#ifdef ENABLE_GOLD_LIMIT
	sys_log(0, "PlayerCreate accountid %d name %s level %d gold %lld"
#else
	sys_log(0, "PlayerCreate accountid %d name %s level %d gold %d"
#endif
#ifdef ENABLE_GAYA_SYSTEM
			" gem %d"
#endif
			", st %d ht %d job %d"
			,packet->account_id
			,packet->player_table.name
			,packet->player_table.level
			,packet->player_table.gold
#ifdef ENABLE_GAYA_SYSTEM
			, packet->player_table.gem
#endif
			,packet->account_id
			,packet->player_table.name
			,packet->player_table.level
			,packet->player_table.gold
			,packet->player_table.st
			,packet->player_table.ht
			,packet->player_table.job);

	CDBManager::instance().EscapeString(text, packet->player_table.skills, sizeof(packet->player_table.skills));
	queryLen += snprintf(queryStr + queryLen, sizeof(queryStr) - queryLen, "'%s', ", text);
	if (g_test_server)
		sys_log(0, "Create_Player queryLen[%d] TEXT[%s]", queryLen, text);

	CDBManager::instance().EscapeString(text, packet->player_table.quickslot, sizeof(packet->player_table.quickslot));
	queryLen += snprintf(queryStr + queryLen, sizeof(queryStr) - queryLen, "'%s')", text);

	std::unique_ptr<SQLMsg> pMsg2(CDBManager::instance().DirectQuery(queryStr));
	if (g_test_server)
		sys_log(0, "Create_Player queryLen[%d] TEXT[%s]", queryLen, text);

	if (pMsg2->Get()->uiAffectedRows <= 0)
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_ALREADY, dwHandle, 0);
		sys_log(0, "ALREADY EXIST3 query: %s AffectedRows %lu", queryStr, pMsg2->Get()->uiAffectedRows);
		return;
	}

	player_id = pMsg2->Get()->uiInsertID;

	snprintf(queryStr, sizeof(queryStr), "UPDATE player_index%s SET pid%d=%d WHERE id=%d",
			GetTablePostfix(), packet->account_index + 1, player_id, packet->account_id);
	std::unique_ptr<SQLMsg> pMsg3(CDBManager::instance().DirectQuery(queryStr));

	if (pMsg3->Get()->uiAffectedRows <= 0)
	{
		sys_err("QUERY_ERROR: %s", queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM player%s WHERE id=%d", GetTablePostfix(), player_id);
		CDBManager::instance().DirectQuery(queryStr);

		peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_FAILED, dwHandle, 0);
		return;
	}

	TPacketDGCreateSuccess pack;
	memset(&pack, 0, sizeof(pack));

	pack.bAccountCharacterIndex = packet->account_index;

	pack.player.dwID			= player_id;
	strlcpy(pack.player.szName, packet->player_table.name, sizeof(pack.player.szName));
	pack.player.byJob			= packet->player_table.job;
	pack.player.byLevel			= 1;
	pack.player.dwPlayMinutes	= 0;
	pack.player.byST			= packet->player_table.st;
	pack.player.byHT			= packet->player_table.ht;
	pack.player.byDX 			= packet->player_table.dx;
	pack.player.byIQ			= packet->player_table.iq;
#ifdef ENABLE_CONQUEROR_LEVEL
	pack.player.bySungmaST			= packet->player_table.sungma_str;
	pack.player.bySungmaHP			= packet->player_table.sungma_hp;
	pack.player.bySungmaMV 			= packet->player_table.sungma_move;
	pack.player.bySungmaINM			= packet->player_table.sungma_immune;
#endif
	pack.player.wMainPart		= packet->player_table.part_base;
	pack.player.x			= packet->player_table.x;
	pack.player.y			= packet->player_table.y;

	peer->EncodeHeader(HEADER_DG_PLAYER_CREATE_SUCCESS, dwHandle, sizeof(TPacketDGCreateSuccess));
	peer->Encode(&pack, sizeof(TPacketDGCreateSuccess));

	sys_log(0, "7 name %s job %d", pack.player.szName, pack.player.byJob);

	s_createTimeByAccountID[packet->account_id] = time(0);
}

/*
 * PLAYER DELETE
 */
void CClientManager::__QUERY_PLAYER_DELETE(CPeer* peer, DWORD dwHandle, TPlayerDeletePacket* packet)
{
	if (!packet->login[0] || !packet->player_id || packet->account_index >= PLAYER_PER_ACCOUNT)
	{
		return;
	}

	CLoginData * ld = GetLoginDataByLogin(packet->login);

	if (!ld)
	{
		peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, dwHandle, 1);
		peer->EncodeBYTE(packet->account_index);
		return;
	}

	TAccountTable & r = ld->GetAccountRef();

	char szQuery[128];
	snprintf(szQuery, sizeof(szQuery), "SELECT p.id, p.level, p.name FROM player_index%s AS i, player%s AS p WHERE pid%u=%u AND pid%u=p.id",
			GetTablePostfix(), GetTablePostfix(), packet->account_index + 1, packet->player_id, packet->account_index + 1);

	ClientHandleInfo * pi = new ClientHandleInfo(dwHandle, packet->player_id);
	pi->account_index = packet->account_index;

	sys_log(0, "PLAYER_DELETE TRY: %s %d pid%d", packet->login, packet->player_id, packet->account_index + 1);
	CDBManager::instance().ReturnQuery(szQuery, QID_PLAYER_DELETE, peer->GetHandle(), pi);
}

void CClientManager::__RESULT_PLAYER_DELETE(CPeer *peer, SQLMsg* msg)
{
	CQueryInfo * qi = (CQueryInfo *) msg->pvUserData;
	ClientHandleInfo * pi = (ClientHandleInfo *) qi->pvData;

	if (msg->Get() && msg->Get()->uiNumRows)
	{
		MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);

		DWORD dwPID = 0;
		str_to_number(dwPID, row[0]);

		int deletedLevelLimit = 0;
		str_to_number(deletedLevelLimit, row[1]);

		char szName[64];
		strlcpy(szName, row[2], sizeof(szName));

		if (deletedLevelLimit >= m_iPlayerDeleteLevelLimit && !IsChinaEventServer())
		{
#ifdef ENABLE_LEVEL_INT
			sys_log(0, "PLAYER_DELETE FAILED LEVEL %d >= DELETE LIMIT %d", deletedLevelLimit, m_iPlayerDeleteLevelLimit);
#else
			sys_log(0, "PLAYER_DELETE FAILED LEVEL %u >= DELETE LIMIT %d", deletedLevelLimit, m_iPlayerDeleteLevelLimit);
#endif
			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		if (deletedLevelLimit < m_iPlayerDeleteLevelLimitLower)
		{
			sys_log(0, "PLAYER_DELETE FAILED LEVEL %u < DELETE LIMIT %d", deletedLevelLimit, m_iPlayerDeleteLevelLimitLower);
			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		char queryStr[QUERY_MAX_LEN];

		snprintf(queryStr, sizeof(queryStr), "INSERT INTO player_deleted%s SELECT * FROM player%s WHERE id=%d",
				GetTablePostfix(), GetTablePostfix(), pi->player_id);
		std::unique_ptr<SQLMsg> pIns(CDBManager::instance().DirectQuery(queryStr));

		if (pIns->Get()->uiAffectedRows == 0 || pIns->Get()->uiAffectedRows == (uint32_t)-1)
		{
			sys_log(0, "PLAYER_DELETE FAILED %u CANNOT INSERT TO player_deleted%s", dwPID, GetTablePostfix());

			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		sys_log(0, "PLAYER_DELETE SUCCESS %u", dwPID);

		char account_index_string[16];

		snprintf(account_index_string, sizeof(account_index_string), "player_id%d", m_iPlayerIDStart + pi->account_index);

		CPlayerTableCache * pkPlayerCache = GetPlayerCache(pi->player_id);

		if (pkPlayerCache)
		{
			m_map_playerCache.erase(pi->player_id);
			delete pkPlayerCache;
		}

		TItemCacheSet * pSet = GetItemCacheSet(pi->player_id);

		if (pSet)
		{
			TItemCacheSet::iterator it = pSet->begin();

			while (it != pSet->end())
			{
				CItemCache * pkItemCache = *it++;
				DeleteItemCache(pkItemCache->Get()->id);
			}

			pSet->clear();
			delete pSet;

			m_map_pkItemCacheSetPtr.erase(pi->player_id);
		}

		snprintf(queryStr, sizeof(queryStr), "UPDATE player_index%s SET pid%u=0 WHERE pid%u=%d",
				GetTablePostfix(),
				pi->account_index + 1,
				pi->account_index + 1,
				pi->player_id);

		std::unique_ptr<SQLMsg> pMsg(CDBManager::instance().DirectQuery(queryStr));

		if (pMsg->Get()->uiAffectedRows == 0 || pMsg->Get()->uiAffectedRows == (uint32_t)-1)
		{
			sys_log(0, "PLAYER_DELETE FAIL WHEN UPDATE account table");
			peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
			peer->EncodeBYTE(pi->account_index);
			return;
		}

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM player%s WHERE id=%d", GetTablePostfix(), pi->player_id);
		delete CDBManager::instance().DirectQuery(queryStr);

#ifdef ENABLE_RENEWAL_SWITCHBOT
		snprintf(queryStr, sizeof(queryStr), "DELETE FROM item%s WHERE owner_id=%d AND (`window` in ('INVENTORY','EQUIPMENT','DRAGON_SOUL_INVENTORY','BELT_INVENTORY','SWITCHBOT'))", GetTablePostfix(), pi->player_id);
#else
		snprintf(queryStr, sizeof(queryStr), "DELETE FROM item%s WHERE owner_id=%d AND (`window` in ('INVENTORY','EQUIPMENT','DRAGON_SOUL_INVENTORY','BELT_INVENTORY'))", GetTablePostfix(), pi->player_id);
#endif

		delete CDBManager::instance().DirectQuery(queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM quest%s WHERE dwPID=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM affect%s WHERE dwPID=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM guild_member%s WHERE pid=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);

		// MYSHOP_PRICE_LIST
		snprintf(queryStr, sizeof(queryStr), "DELETE FROM myshop_pricelist%s WHERE owner_id=%d", GetTablePostfix(), pi->player_id);
		CDBManager::instance().AsyncQuery(queryStr);
		// END_OF_MYSHOP_PRICE_LIST

		snprintf(queryStr, sizeof(queryStr), "DELETE FROM messenger_list%s WHERE account='%s' OR companion='%s'", GetTablePostfix(), szName, szName);
		CDBManager::instance().AsyncQuery(queryStr);

		peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_SUCCESS, pi->dwHandle, 1);
		peer->EncodeBYTE(pi->account_index);
	}
	else
	{
		sys_log(0, "PLAYER_DELETE FAIL NO ROW");
		peer->EncodeHeader(HEADER_DG_PLAYER_DELETE_FAILED, pi->dwHandle, 1);
		peer->EncodeBYTE(pi->account_index);
	}
}

void CClientManager::QUERY_ADD_AFFECT(CPeer * peer, TPacketGDAddAffect * p)
{
	char queryStr[QUERY_MAX_LEN];
	snprintf(queryStr, sizeof(queryStr),
			"REPLACE INTO affect%s (dwPID, bType, bApplyOn, lApplyValue, dwFlag, lDuration, lSPCost) "
			"VALUES(%u, %u, %u, %d, %u, %d, %d)",
			GetTablePostfix(),
			p->dwPID,
			p->elem.dwType,
			p->elem.bApplyOn,
			p->elem.lApplyValue,
			p->elem.dwFlag,
			p->elem.lDuration,
			p->elem.lSPCost);

	CDBManager::instance().AsyncQuery(queryStr);
}

void CClientManager::QUERY_REMOVE_AFFECT(CPeer * peer, TPacketGDRemoveAffect * p)
{
	char queryStr[QUERY_MAX_LEN];

	snprintf(queryStr, sizeof(queryStr),
			"DELETE FROM affect%s WHERE dwPID=%u AND bType=%u AND bApplyOn=%u",
			GetTablePostfix(), p->dwPID, p->dwType, p->bApplyOn);

	CDBManager::instance().AsyncQuery(queryStr);
}

void CClientManager::QUERY_HIGHSCORE_REGISTER(CPeer* peer, TPacketGDHighscore * data)
{
	char szQuery[128];
	snprintf(szQuery, sizeof(szQuery), "SELECT value FROM highscore%s WHERE board='%s' AND pid = %u", GetTablePostfix(), data->szBoard, data->dwPID);

	sys_log(0, "HEADER_GD_HIGHSCORE_REGISTER: PID %u", data->dwPID);

	ClientHandleInfo * pi = new ClientHandleInfo(0);
	strlcpy(pi->login, data->szBoard, sizeof(pi->login));
	pi->account_id = (DWORD)data->lValue;
	pi->player_id = data->dwPID;
	pi->account_index = (data->cDir > 0);

	CDBManager::instance().ReturnQuery(szQuery, QID_HIGHSCORE_REGISTER, peer->GetHandle(), pi);
}

void CClientManager::RESULT_HIGHSCORE_REGISTER(CPeer * pkPeer, SQLMsg * msg)
{
	CQueryInfo * qi = (CQueryInfo *) msg->pvUserData;
	ClientHandleInfo * pi = (ClientHandleInfo *) qi->pvData;

	char szBoard[21];
	strlcpy(szBoard, pi->login, sizeof(szBoard));
	int value = (int)pi->account_id;

	SQLResult * res = msg->Get();

	if (res->uiNumRows == 0)
	{
		char buf[256];
		snprintf(buf, sizeof(buf), "INSERT INTO highscore%s VALUES('%s', %u, %d)", GetTablePostfix(), szBoard, pi->player_id, value);
		CDBManager::instance().AsyncQuery(buf);
	}
	else
	{
		if (!res->pSQLResult)
		{
			delete pi;
			return;
		}

		MYSQL_ROW row = mysql_fetch_row(res->pSQLResult);
		if (row && row[0])
		{
			int current_value = 0; str_to_number(current_value, row[0]);
			if (((pi->account_index)&&(current_value >= value)) || ((!pi->account_index)&&(current_value <= value)))
			{
				value = current_value;
			}
			else
			{
				char buf[256];
				snprintf(buf, sizeof(buf), "REPLACE INTO highscore%s VALUES('%s', %u, %d)", GetTablePostfix(), szBoard, pi->player_id, value);
				CDBManager::instance().AsyncQuery(buf);
			}
		}
		else
		{
			char buf[256];
			snprintf(buf, sizeof(buf), "INSERT INTO highscore%s VALUES('%s', %u, %d)", GetTablePostfix(), szBoard, pi->player_id, value);
			CDBManager::instance().AsyncQuery(buf);
		}
	}

	delete pi;
}

void CClientManager::InsertLogoutPlayer(DWORD pid)
{
	TLogoutPlayerMap::iterator it = m_map_logout.find(pid);

	if (it != m_map_logout.end())
	{
		if (g_log)
			sys_log(0, "LOGOUT: Update player time pid(%d)", pid);

		it->second->time = time(0);
		return;
	}

	TLogoutPlayer * pLogout = new TLogoutPlayer;
	pLogout->pid = pid;
	pLogout->time = time(0);
	m_map_logout.insert(std::make_pair(pid, pLogout));

	if (g_log)
		sys_log(0, "LOGOUT: Insert player pid(%d)", pid);
}

void CClientManager::DeleteLogoutPlayer(DWORD pid)
{
	TLogoutPlayerMap::iterator it = m_map_logout.find(pid);

	if (it != m_map_logout.end())
	{
		delete it->second;
		m_map_logout.erase(it);
	}
}

extern int g_iLogoutSeconds;

void CClientManager::UpdateLogoutPlayer()
{
	time_t now = time(0);

	TLogoutPlayerMap::iterator it = m_map_logout.begin();

	while (it != m_map_logout.end())
	{
		TLogoutPlayer* pLogout = it->second;

		if (now - g_iLogoutSeconds > pLogout->time)
		{
			FlushItemCacheSet(pLogout->pid);
			FlushPlayerCacheSet(pLogout->pid);
#ifdef ENABLE_GROWTH_PET_SYSTEM
			FlushGrowthPetCacheSet(pLogout->pid);
#endif

			delete pLogout;
			m_map_logout.erase(it++);
		}
		else
			++it;
	}
}

void CClientManager::FlushPlayerCacheSet(DWORD pid)
{
	TPlayerTableCacheMap::iterator it = m_map_playerCache.find(pid);

	if (it != m_map_playerCache.end())
	{
		CPlayerTableCache * c = it->second;
		m_map_playerCache.erase(it);

		c->Flush();
		delete c;
	}
}

