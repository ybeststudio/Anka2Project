#include <map>
#include "stdafx.h"
#include "ClientManager.h"
#include "Main.h"
#include "CsvReader.h"
#include "ProtoReader.h"

#include "../../common/service.h"
#include "../../common/tables.h"

using namespace std;
extern std::string g_stLocaleNameColumn;

bool CClientManager::InitializeTables()
{
	if (!InitializeMobTable())
	{
		sys_err("InitializeMobTable FAILED");
		return false;
	}

	if (!InitializeItemTable())
	{
		sys_err("InitializeItemTable FAILED");
		return false;
	}

	if (!InitializeShopTable())
	{
		sys_err("InitializeShopTable FAILED");
		return false;
	}

#ifdef ENABLE_RENEWAL_SHOPEX
	if (!InitializeShopEXTable())
	{
		sys_err("InitializeShopEXTable FAILED");
		return false;
	}
#endif

	if (!InitializeSkillTable())
	{
		sys_err("InitializeSkillTable FAILED");
		return false;
	}

	if (!InitializeRefineTable())
	{
		sys_err("InitializeRefineTable FAILED");
		return false;
	}

	if (!InitializeItemAttrTable())
	{
		sys_err("InitializeItemAttrTable FAILED");
		return false;
	}

	if (!InitializeItemRareTable())
	{
		sys_err("InitializeItemRareTable FAILED");
		return false;
	}

	if (!InitializeBanwordTable())
	{
		sys_err("InitializeBanwordTable FAILED");
		return false;
	}

	if (!InitializeLandTable())
	{
		sys_err("InitializeLandTable FAILED");
		return false;
	}

	if (!InitializeObjectProto())
	{
		sys_err("InitializeObjectProto FAILED");
		return false;
	}

	if (!InitializeObjectTable())
	{
		sys_err("InitializeObjectTable FAILED");
		return false;
	}

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	if (!InitializeOfflineShop())
	{
		sys_err("InitializeOfflineShop FAILED");
		return false;
	}
#endif

#ifdef ENABLE_ITEMSHOP
	if (!InitializeItemShop())
	{
		sys_err("InitializeItemShop FAILED");
		return false;
	}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (!InitializeGrowthPetSkillTable())
	{
		sys_err("InitializeGrowthPetSkillTable FAILED");
		return false;
	}
#endif

	return true;
}

bool CClientManager::InitializeRefineTable()
{
	char query[2048];

	snprintf(query, sizeof(query),
			"SELECT id, cost, prob, vnum0, count0, vnum1, count1, vnum2, count2,  vnum3, count3, vnum4, count4 FROM refine_proto%s",
			GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
		return true;

	if (m_pRefineTable)
	{
		sys_log(0, "RELOAD: refine_proto");
		delete [] m_pRefineTable;
		m_pRefineTable = NULL;
	}

	m_iRefineTableSize = pRes->uiNumRows;

	m_pRefineTable = new TRefineTable[m_iRefineTableSize];
	memset(m_pRefineTable, 0, sizeof(TRefineTable) * m_iRefineTableSize);

	TRefineTable* prt = m_pRefineTable;
	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		int col = 0;
		str_to_number(prt->id, data[col++]);
		str_to_number(prt->cost, data[col++]);
		str_to_number(prt->prob, data[col++]);

		for (int i = 0; i < REFINE_MATERIAL_MAX_NUM; i++)
		{
			str_to_number(prt->materials[i].vnum, data[col++]);
			str_to_number(prt->materials[i].count, data[col++]);
			if (prt->materials[i].vnum == 0)
			{
				prt->material_count = i;
				break;
			}
		}

		sys_log(0, "REFINE: id %ld cost %d prob %d mat1 %lu cnt1 %d", prt->id, prt->cost, prt->prob, prt->materials[0].vnum, prt->materials[0].count);

		prt++;
	}
	return true;
}

class FCompareVnum
{
	public:
		bool operator () (const TEntityTable & a, const TEntityTable & b) const
		{
			return (a.dwVnum < b.dwVnum);
		}
};

bool CClientManager::InitializeMobTable()
{
	map<int,const char*> localMap;
	bool isNameFile = true;
	cCsvTable nameData;

	if(!nameData.Load("mob_names.txt",'\t'))
	{
		fprintf(stderr, "Failed to read mob_names.txt\n");
		isNameFile = false;
	}
	else
	{
		nameData.Next();
		while(nameData.Next())
		{
			localMap[atoi(nameData.AsStringByIndex(0))] = nameData.AsStringByIndex(1);
		}
	}

	map<DWORD, TMobTable *> test_map_mobTableByVnum;
	set<int> vnumSet;
	cCsvTable data;

	if(!data.Load("mob_proto.txt",'\t'))
	{
		fprintf(stderr, "Failed to read mob_proto.txt\n");
		return false;
	}
	data.Next();

	int addNumber = 0;
	while(data.Next())
	{
		int vnum = atoi(data.AsStringByIndex(0));
		std::map<DWORD, TMobTable *>::iterator it_map_mobTable;
		it_map_mobTable = test_map_mobTableByVnum.find(vnum);

		if(it_map_mobTable != test_map_mobTableByVnum.end())
		{
			addNumber++;
		}
	}

	data.Destroy();
	if(!data.Load("mob_proto.txt",'\t'))
	{
		fprintf(stderr, "Failed to read mob_proto.txt\n");
		return false;
	}
	data.Next();

	if (!m_vec_mobTable.empty())
	{
		sys_log(0, "RELOAD: mob_proto");
		m_vec_mobTable.clear();
	}

	m_vec_mobTable.resize(data.m_File.GetRowCount()-1 + addNumber);
	memset(&m_vec_mobTable[0], 0, sizeof(TMobTable) * m_vec_mobTable.size());
	TMobTable * mob_table = &m_vec_mobTable[0];

	while (data.Next())
	{
		int col = 0;
		bool isSameRow = true;
		std::map<DWORD, TMobTable *>::iterator it_map_mobTable;
		it_map_mobTable = test_map_mobTableByVnum.find(atoi(data.AsStringByIndex(col)));

		if(it_map_mobTable == test_map_mobTableByVnum.end())
		{
			isSameRow = false;
		}

		if(isSameRow)
		{
			TMobTable *tempTable = it_map_mobTable->second;

			mob_table->dwVnum = tempTable->dwVnum;
			strlcpy(mob_table->szName, tempTable->szName, sizeof(tempTable->szName));
			strlcpy(mob_table->szLocaleName, tempTable->szLocaleName, sizeof(tempTable->szName));
			mob_table->bRank = tempTable->bRank;
			mob_table->bType = tempTable->bType;
			mob_table->bBattleType = tempTable->bBattleType;
			mob_table->bLevel = tempTable->bLevel;
			mob_table->bSize = tempTable->bSize;
			mob_table->dwAIFlag = tempTable->dwAIFlag;
			mob_table->dwRaceFlag = tempTable->dwRaceFlag;
			mob_table->dwImmuneFlag = tempTable->dwImmuneFlag;
			mob_table->bEmpire = tempTable->bEmpire;
			strlcpy(mob_table->szFolder, tempTable->szFolder, sizeof(tempTable->szName));
			mob_table->bOnClickType = tempTable->bOnClickType;
			mob_table->bStr = tempTable->bStr;
			mob_table->bDex = tempTable->bDex;
			mob_table->bCon = tempTable->bCon;
			mob_table->bInt = tempTable->bInt;
			mob_table->dwDamageRange[0] = tempTable->dwDamageRange[0];
			mob_table->dwDamageRange[1] = tempTable->dwDamageRange[1];
			mob_table->dwMaxHP = tempTable->dwMaxHP;
			mob_table->bRegenCycle = tempTable->bRegenCycle;
			mob_table->bRegenPercent = tempTable->bRegenPercent;
			mob_table->dwGoldMin = tempTable->dwGoldMin;
			mob_table->dwGoldMax = tempTable->dwGoldMax;
			mob_table->dwExp = tempTable->dwExp;
			mob_table->wDef = tempTable->wDef;
			mob_table->sAttackSpeed = tempTable->sAttackSpeed;
			mob_table->sMovingSpeed = tempTable->sMovingSpeed;
			mob_table->bAggresiveHPPct = tempTable->bAggresiveHPPct;
			mob_table->wAggressiveSight = tempTable->wAggressiveSight;
			mob_table->wAttackRange = tempTable->wAttackRange;
			mob_table->dwDropItemVnum = tempTable->dwDropItemVnum;
			mob_table->dwResurrectionVnum = tempTable->dwResurrectionVnum;

			for (int i = 0; i < MOB_ENCHANTS_MAX_NUM; ++i)
				mob_table->cEnchants[i] = tempTable->cEnchants[i];

			for (int i = 0; i < MOB_RESISTS_MAX_NUM; ++i)
				mob_table->cResists[i] = tempTable->cResists[i];

			mob_table->fDamMultiply = tempTable->fDamMultiply;
			mob_table->dwSummonVnum = tempTable->dwSummonVnum;
			mob_table->dwDrainSP = tempTable->dwDrainSP;
			mob_table->dwPolymorphItemVnum = tempTable->dwPolymorphItemVnum;

			mob_table->Skills[0].bLevel = tempTable->Skills[0].bLevel;
			mob_table->Skills[0].dwVnum = tempTable->Skills[0].dwVnum;
			mob_table->Skills[1].bLevel = tempTable->Skills[1].bLevel;
			mob_table->Skills[1].dwVnum = tempTable->Skills[1].dwVnum;
			mob_table->Skills[2].bLevel = tempTable->Skills[2].bLevel;
			mob_table->Skills[2].dwVnum = tempTable->Skills[2].dwVnum;
			mob_table->Skills[3].bLevel = tempTable->Skills[3].bLevel;
			mob_table->Skills[3].dwVnum = tempTable->Skills[3].dwVnum;
			mob_table->Skills[4].bLevel = tempTable->Skills[4].bLevel;
			mob_table->Skills[4].dwVnum = tempTable->Skills[4].dwVnum;

			mob_table->bBerserkPoint = tempTable->bBerserkPoint;
			mob_table->bStoneSkinPoint = tempTable->bStoneSkinPoint;
			mob_table->bGodSpeedPoint = tempTable->bGodSpeedPoint;
			mob_table->bDeathBlowPoint = tempTable->bDeathBlowPoint;
			mob_table->bRevivePoint = tempTable->bRevivePoint;
		}
		else
		{
			if (!Set_Proto_Mob_Table(mob_table, data, localMap))
			{
				fprintf(stderr, "Failed to set mob proto table.\n");
			}
		}

		vnumSet.insert(mob_table->dwVnum);

		sys_log(1, "MOB #%-5d %-24s %-24s level: %-3u rank: %u empire: %d", mob_table->dwVnum, mob_table->szName, mob_table->szLocaleName, mob_table->bLevel, mob_table->bRank, mob_table->bEmpire);
		++mob_table;
	}

	sort(m_vec_mobTable.begin(), m_vec_mobTable.end(), FCompareVnum());
	return true;
}

bool CClientManager::InitializeShopTable()
{
	MYSQL_ROW	data;
	int		col;

	static const char * s_szQuery =
		"SELECT "
		"shop.vnum, "
		"shop.npc_vnum, "
		"shop_item.item_vnum, "
		"shop_item.count "
		"FROM shop LEFT JOIN shop_item "
		"ON shop.vnum = shop_item.shop_vnum ORDER BY shop.vnum, shop_item.item_vnum";

	std::unique_ptr<SQLMsg> pkMsg2(CDBManager::instance().DirectQuery(s_szQuery));

	SQLResult * pRes2 = pkMsg2->Get();

	if (!pRes2->uiNumRows)
	{
		sys_err("InitializeShopTable : Table count is zero.");
		return false;
	}

	std::map<int, TShopTable *> map_shop;

	if (m_pShopTable)
	{
		delete [] (m_pShopTable);
		m_pShopTable = NULL;
	}

	TShopTable * shop_table = m_pShopTable;

	while ((data = mysql_fetch_row(pRes2->pSQLResult)))
	{
		col = 0;

		int iShopVnum = 0;
		str_to_number(iShopVnum, data[col++]);

		if (map_shop.end() == map_shop.find(iShopVnum))
		{
			shop_table = new TShopTable;
			memset(shop_table, 0, sizeof(TShopTable));
			shop_table->dwVnum	= iShopVnum;

			map_shop[iShopVnum] = shop_table;
		}
		else
			shop_table = map_shop[iShopVnum];

		str_to_number(shop_table->dwNPCVnum, data[col++]);

		if (!data[col])
			continue;

		TShopItemTable * pItem = &shop_table->items[shop_table->byItemCount];

		str_to_number(pItem->vnum, data[col++]);
		str_to_number(pItem->count, data[col++]);

		++shop_table->byItemCount;
	}

	m_pShopTable = new TShopTable[map_shop.size()];
	m_iShopTableSize = map_shop.size();

	__typeof(map_shop.begin()) it = map_shop.begin();

	int i = 0;

	while (it != map_shop.end())
	{
		thecore_memcpy((m_pShopTable + i), (it++)->second, sizeof(TShopTable));
		sys_log(0, "SHOP: #%d items: %d", (m_pShopTable + i)->dwVnum, (m_pShopTable + i)->byItemCount);
		++i;
	}

	return true;
}

#ifdef ENABLE_RENEWAL_SHOPEX
bool CClientManager::InitializeShopEXTable()
{
	MYSQL_ROW data;

	char s_szQuery[512];
	int len = snprintf(s_szQuery, sizeof(s_szQuery), "SELECT shopex.vnum, shopex.name, shopex.npc_vnum, shopex_item.item_vnum, shopex_item.count, shopex_item.price, shopex_item.price_vnum, shopex_item.price_type+0, ");
	
	for (BYTE i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		len += sprintf(s_szQuery + len, "socket%d, ", i);

	for (BYTE i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++) {
		len += sprintf(s_szQuery + len, "attrtype%d, attrvalue%d ", i, i);

		if (i != ITEM_ATTRIBUTE_MAX_NUM - 1)
			len += sprintf(s_szQuery + len, ", ");
	}

	len += sprintf(s_szQuery + len, "FROM shopex LEFT JOIN shopex_item ON shopex.vnum = shopex_item.shop_vnum ORDER BY shopex.vnum, shopex_item.item_vnum");

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(s_szQuery));

	SQLResult* pRes = pkMsg->Get();

	if (m_pShopEXTable)
	{
		delete[](m_pShopEXTable);
		m_pShopEXTable = NULL;
	}
	
	if (!pRes->uiNumRows)
		return true;

	std::map<int, TShopTable*> map_shop;

	TShopTable* shop_table = m_pShopEXTable;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		int col = 0;
		int iShopVnum = 0;
		str_to_number(iShopVnum, data[col++]);

		if (map_shop.end() == map_shop.find(iShopVnum))
		{
			shop_table = new TShopTable;
			std::memset(shop_table, 0, sizeof(TShopTable));
			shop_table->dwVnum = iShopVnum;
			map_shop[iShopVnum] = shop_table;
		}
		else
			shop_table = map_shop[iShopVnum];

		std::memcpy(shop_table->szShopName, data[col++], sizeof(shop_table->szShopName));
		str_to_number(shop_table->dwNPCVnum, data[col++]);

		TShopItemTable* pItem = &shop_table->items[shop_table->byItemCount];

		str_to_number(pItem->vnum, data[col++]);
		str_to_number(pItem->count, data[col++]);
		str_to_number(pItem->price, data[col++]);
		str_to_number(pItem->price_vnum, data[col++]);
		str_to_number(pItem->price_type, data[col++]);
		
		for (BYTE i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
			str_to_number(pItem->alSockets[i], data[col++]);
		for (BYTE i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; i++) {
			str_to_number(pItem->aAttr[i].bType, data[col++]);
			str_to_number(pItem->aAttr[i].sValue, data[col++]);
		}

		if (pItem->vnum)
			++shop_table->byItemCount;
	}

	m_pShopEXTable = new TShopTable[map_shop.size()];
	m_iShopEXTableSize = map_shop.size();

	int i = 0;
	for (std::map<int, TShopTable*>::const_iterator it = map_shop.begin(); it != map_shop.end(); ++it, ++i)
		std::memcpy((m_pShopEXTable + i), it->second, sizeof(TShopTable));

	return true;
}
#endif

bool CClientManager::InitializeQuestItemTable()
{
	using namespace std;

	static const char * s_szQuery = "SELECT vnum, name, %s FROM quest_item_proto ORDER BY vnum";

	char query[1024];
	snprintf(query, sizeof(query), s_szQuery, g_stLocaleNameColumn.c_str());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("query error or no rows: %s", query);
		return false;
	}

	MYSQL_ROW row;

	while ((row = mysql_fetch_row(pRes->pSQLResult)))
	{
		int col = 0;

		TItemTable tbl;
		memset(&tbl, 0, sizeof(tbl));

		str_to_number(tbl.dwVnum, row[col++]);

		if (row[col])
			strlcpy(tbl.szName, row[col], sizeof(tbl.szName));

		col++;

		if (row[col])
			strlcpy(tbl.szLocaleName, row[col], sizeof(tbl.szLocaleName));

		col++;

		if (m_map_itemTableByVnum.find(tbl.dwVnum) != m_map_itemTableByVnum.end())
		{
			sys_err("QUEST_ITEM_ERROR! %lu vnum already exist! (name %s)", tbl.dwVnum, tbl.szLocaleName);
			continue;
		}

		tbl.bType = ITEM_QUEST; // quest_item_proto ITEM_QUEST
		tbl.bSize = 1;

		m_vec_itemTable.push_back(tbl);
	}

	return true;
}

bool CClientManager::InitializeItemTable()
{
	bool isNameFile = true;
	map<int,const char*> localMap;
	cCsvTable nameData;

	if(!nameData.Load("item_names.txt",'\t'))
	{
		fprintf(stderr, "Failed to read item_names.txt\n");
		isNameFile = false;
	}
	else
	{
		nameData.Next();
		while(nameData.Next())
		{
			localMap[atoi(nameData.AsStringByIndex(0))] = nameData.AsStringByIndex(1);
		}
	}

	map<DWORD, TItemTable *> test_map_itemTableByVnum;
	set<int> vnumSet;
	cCsvTable data;

	if(!data.Load("item_proto.txt",'\t'))
	{
		fprintf(stderr, "Failed to read item_proto.txt\n");
		return false;
	}

	data.Next();

	if (!m_vec_itemTable.empty())
	{
		sys_log(0, "RELOAD: item_proto");
		m_vec_itemTable.clear();
		m_map_itemTableByVnum.clear();
	}

	int addNumber = 0;
	while(data.Next())
	{
		int vnum = atoi(data.AsStringByIndex(0));
		std::map<DWORD, TItemTable *>::iterator it_map_itemTable;
		it_map_itemTable = test_map_itemTableByVnum.find(vnum);

		if(it_map_itemTable != test_map_itemTableByVnum.end())
		{
			addNumber++;
		}
	}

	data.Destroy();

	if(!data.Load("item_proto.txt",'\t'))
	{
		fprintf(stderr, "Failed to read item_proto.txt\n");
		return false;
	}
	data.Next();

	m_vec_itemTable.resize(data.m_File.GetRowCount() - 1 + addNumber);
	memset(&m_vec_itemTable[0], 0, sizeof(TItemTable) * m_vec_itemTable.size());
	int testValue =  m_vec_itemTable.size();

	TItemTable * item_table = &m_vec_itemTable[0];

	while (data.Next())
	{
		int col = 0;

		std::map<DWORD, TItemTable *>::iterator it_map_itemTable;
		it_map_itemTable = test_map_itemTableByVnum.find(atoi(data.AsStringByIndex(col)));
		if(it_map_itemTable == test_map_itemTableByVnum.end())
		{
			if (!Set_Proto_Item_Table(item_table, data, localMap))
			{
				fprintf(stderr, "Failed to set item proto table.\n");
			}
		}
		else
		{
			TItemTable *tempTable = it_map_itemTable->second;

			item_table->dwVnum = tempTable->dwVnum;
			strlcpy(item_table->szName, tempTable->szName, sizeof(item_table->szName));
			strlcpy(item_table->szLocaleName, tempTable->szLocaleName, sizeof(item_table->szLocaleName));
			item_table->bType = tempTable->bType;
			item_table->bSubType = tempTable->bSubType;
			item_table->bSize = tempTable->bSize;
			item_table->dwAntiFlags = tempTable->dwAntiFlags;
			item_table->dwFlags = tempTable->dwFlags;
			item_table->dwWearFlags = tempTable->dwWearFlags;
			item_table->dwImmuneFlag = tempTable->dwImmuneFlag;
			item_table->dwGold = tempTable->dwGold;
			item_table->dwShopBuyPrice = tempTable->dwShopBuyPrice;
			item_table->dwRefinedVnum =tempTable->dwRefinedVnum;
			item_table->wRefineSet =tempTable->wRefineSet;
			item_table->bAlterToMagicItemPct = tempTable->bAlterToMagicItemPct;
			item_table->cLimitRealTimeFirstUseIndex = -1;
			item_table->cLimitTimerBasedOnWearIndex = -1;

			int i;

			for (i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
			{
				item_table->aLimits[i].bType = tempTable->aLimits[i].bType;
				item_table->aLimits[i].lValue = tempTable->aLimits[i].lValue;

				if (LIMIT_REAL_TIME_START_FIRST_USE == item_table->aLimits[i].bType)
					item_table->cLimitRealTimeFirstUseIndex = (char)i;

				if (LIMIT_TIMER_BASED_ON_WEAR == item_table->aLimits[i].bType)
					item_table->cLimitTimerBasedOnWearIndex = (char)i;
			}

			for (i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
			{
				item_table->aApplies[i].bType = tempTable->aApplies[i].bType;
				item_table->aApplies[i].lValue = tempTable->aApplies[i].lValue;
			}

			for (i = 0; i < ITEM_VALUES_MAX_NUM; ++i)
				item_table->alValues[i] = tempTable->alValues[i];

			item_table->bGainSocketPct = tempTable->bGainSocketPct;
			item_table->sAddonType = tempTable->sAddonType;

			item_table->bWeight  = tempTable->bWeight;
		}

		vnumSet.insert(item_table->dwVnum);
		m_map_itemTableByVnum.insert(std::map<DWORD, TItemTable *>::value_type(item_table->dwVnum, item_table));
		++item_table;
	}
	//_______________________________________________________________________//

	// QUEST_ITEM_PROTO_DISABLE
	// InitializeQuestItemTable();
	// END_OF_QUEST_ITEM_PROTO_DISABLE

	m_map_itemTableByVnum.clear();

	itertype(m_vec_itemTable) it = m_vec_itemTable.begin();

	while (it != m_vec_itemTable.end())
	{
		TItemTable * item_table = &(*(it++));

		sys_log(0, "ITEM: #%-5lu %-24s %-24s VAL: %ld %ld %ld %ld %ld %ld WEAR %lu ANTI %lu IMMUNE %lu REFINE %lu REFINE_SET %u MAGIC_PCT %u",
				item_table->dwVnum,
				item_table->szName,
				item_table->szLocaleName,
				item_table->alValues[0],
				item_table->alValues[1],
				item_table->alValues[2],
				item_table->alValues[3],
				item_table->alValues[4],
				item_table->alValues[5],
				item_table->dwWearFlags,
				item_table->dwAntiFlags,
				item_table->dwImmuneFlag,
				item_table->dwRefinedVnum,
				item_table->wRefineSet,
				item_table->bAlterToMagicItemPct);

		m_map_itemTableByVnum.insert(std::map<DWORD, TItemTable *>::value_type(item_table->dwVnum, item_table));
	}

	sort(m_vec_itemTable.begin(), m_vec_itemTable.end(), FCompareVnum());
	return true;
}

bool CClientManager::InitializeSkillTable()
{
	char query[4096];
	snprintf(query, sizeof(query),
		"SELECT dwVnum, szName, bType, bMaxLevel, dwSplashRange, "
		"szPointOn, szPointPoly, szSPCostPoly, szDurationPoly, szDurationSPCostPoly, "
		"szCooldownPoly, szMasterBonusPoly, setFlag+0, setAffectFlag+0, "
		"szPointOn2, szPointPoly2, szDurationPoly2, setAffectFlag2+0, "
		"szPointOn3, szPointPoly3, szDurationPoly3, szGrandMasterAddSPCostPoly, "
		"bLevelStep, bLevelLimit, prerequisiteSkillVnum, prerequisiteSkillLevel, iMaxHit, szSplashAroundDamageAdjustPoly, eSkillType+0, dwTargetRange "
		"FROM skill_proto%s ORDER BY dwVnum",
		GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("no result from skill_proto");
		return false;
	}

	if (!m_vec_skillTable.empty())
	{
		sys_log(0, "RELOAD: skill_proto");
		m_vec_skillTable.clear();
	}

	m_vec_skillTable.reserve(pRes->uiNumRows);

	MYSQL_ROW data;
	int col;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TSkillTable t;
		memset(&t, 0, sizeof(t));

		col = 0;

		str_to_number(t.dwVnum, data[col++]);
		strlcpy(t.szName, data[col++], sizeof(t.szName));
		str_to_number(t.bType, data[col++]);
		str_to_number(t.bMaxLevel, data[col++]);
		str_to_number(t.dwSplashRange, data[col++]);

		strlcpy(t.szPointOn, data[col++], sizeof(t.szPointOn));
		strlcpy(t.szPointPoly, data[col++], sizeof(t.szPointPoly));
		strlcpy(t.szSPCostPoly, data[col++], sizeof(t.szSPCostPoly));
		strlcpy(t.szDurationPoly, data[col++], sizeof(t.szDurationPoly));
		strlcpy(t.szDurationSPCostPoly, data[col++], sizeof(t.szDurationSPCostPoly));
		strlcpy(t.szCooldownPoly, data[col++], sizeof(t.szCooldownPoly));
		strlcpy(t.szMasterBonusPoly, data[col++], sizeof(t.szMasterBonusPoly));

		str_to_number(t.dwFlag, data[col++]);
		str_to_number(t.dwAffectFlag, data[col++]);

		strlcpy(t.szPointOn2, data[col++], sizeof(t.szPointOn2));
		strlcpy(t.szPointPoly2, data[col++], sizeof(t.szPointPoly2));
		strlcpy(t.szDurationPoly2, data[col++], sizeof(t.szDurationPoly2));
		str_to_number(t.dwAffectFlag2, data[col++]);

		// ADD_GRANDMASTER_SKILL
		strlcpy(t.szPointOn3, data[col++], sizeof(t.szPointOn3));
		strlcpy(t.szPointPoly3, data[col++], sizeof(t.szPointPoly3));
		strlcpy(t.szDurationPoly3, data[col++], sizeof(t.szDurationPoly3));

		strlcpy(t.szGrandMasterAddSPCostPoly, data[col++], sizeof(t.szGrandMasterAddSPCostPoly));
		// END_OF_ADD_GRANDMASTER_SKILL

		str_to_number(t.bLevelStep, data[col++]);
		str_to_number(t.bLevelLimit, data[col++]);
		str_to_number(t.preSkillVnum, data[col++]);
		str_to_number(t.preSkillLevel, data[col++]);

		str_to_number(t.lMaxHit, data[col++]);

		strlcpy(t.szSplashAroundDamageAdjustPoly, data[col++], sizeof(t.szSplashAroundDamageAdjustPoly));

		str_to_number(t.bSkillAttrType, data[col++]);
		str_to_number(t.dwTargetRange, data[col++]);

		sys_log(0, "SKILL: #%d %s flag %u point %s affect %u cooldown %s", t.dwVnum, t.szName, t.dwFlag, t.szPointOn, t.dwAffectFlag, t.szCooldownPoly);

		m_vec_skillTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeBanwordTable()
{
	m_vec_banwordTable.clear();

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery("SELECT word FROM banword"));

	SQLResult * pRes = pkMsg->Get();

	if (pRes->uiNumRows == 0)
		return true;

	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TBanwordTable t;

		if (data[0])
		{
			strlcpy(t.szWord, data[0], sizeof(t.szWord));
			m_vec_banwordTable.push_back(t);
		}
	}

	sys_log(0, "BANWORD: total %d", m_vec_banwordTable.size());
	return true;
}

bool CClientManager::InitializeItemAttrTable()
{
	char query[4096];
	snprintf(query, sizeof(query),
		"SELECT apply, apply+0, prob, lv1, lv2, lv3, lv4, lv5, "
		"weapon, body, wrist, foots, neck, head, shield, ear "
#ifdef ENABLE_PENDANT_SYSTEM
		",pendant "
#endif
#ifdef ENABLE_BONUS_COSTUME_SYSTEM
		", costume_body, costume_hair, costume_weapon"
#endif
		" FROM item_attr%s ORDER BY apply",
		GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("no result from item_attr");
		return false;
	}

	if (!m_vec_itemAttrTable.empty())
	{
		sys_log(0, "RELOAD: item_attr");
		m_vec_itemAttrTable.clear();
	}

	m_vec_itemAttrTable.reserve(pRes->uiNumRows);

	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TItemAttrTable t;

		memset(&t, 0, sizeof(TItemAttrTable));

		int col = 0;

		strlcpy(t.szApply, data[col++], sizeof(t.szApply));
		int32_t apply_idx = get_Item_ApplyType_Value(t.szApply);
		if (apply_idx >= 0)
			t.dwApplyIndex = apply_idx;
		else
			str_to_number(t.dwApplyIndex, data[col]);
		col++;
		str_to_number(t.dwProb, data[col++]);
		str_to_number(t.lValues[0], data[col++]);
		str_to_number(t.lValues[1], data[col++]);
		str_to_number(t.lValues[2], data[col++]);
		str_to_number(t.lValues[3], data[col++]);
		str_to_number(t.lValues[4], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_BODY], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_NECK], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_EAR], data[col++]);
#ifdef ENABLE_PENDANT_SYSTEM
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_PENDANT], data[col++]);
#endif
#ifdef ENABLE_BONUS_COSTUME_SYSTEM
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_BODY], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_HEAD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_WEAPON], data[col++]);
#endif

		sys_log(0, "ITEM_ATTR: %-20s %4lu { %3d %3d %3d %3d %3d } { %d %d %d %d %d %d %d %d }",
				t.szApply,
				t.dwProb,
				t.lValues[0],
				t.lValues[1],
				t.lValues[2],
				t.lValues[3],
				t.lValues[4],
				t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON],
				t.bMaxLevelBySet[ATTRIBUTE_SET_BODY],
				t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST],
				t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS],
				t.bMaxLevelBySet[ATTRIBUTE_SET_NECK],
				t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD],
				t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD],
				t.bMaxLevelBySet[ATTRIBUTE_SET_EAR]
#ifdef ENABLE_PENDANT_SYSTEM
				,t.bMaxLevelBySet[ATTRIBUTE_SET_PENDANT]
#endif
#ifdef ENABLE_BONUS_COSTUME_SYSTEM
				,t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_BODY],
				t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_HEAD],
				t.bMaxLevelBySet[ATTRIBUTE_SET_COSTUME_WEAPON]
#endif

				);
		m_vec_itemAttrTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeItemRareTable()
{
	char query[4096];
	snprintf(query, sizeof(query),
	"SELECT apply, apply+0, prob, lv1, lv2, lv3, lv4, lv5, "
	"weapon, body, wrist, foots, neck, head, shield, ear "
#ifdef ENABLE_PENDANT_SYSTEM
	",pendant "
#endif
		" FROM item_attr_rare%s ORDER BY apply",
		GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!pRes->uiNumRows)
	{
		sys_err("no result from item_attr_rare");
		return false;
	}

	if (!m_vec_itemRareTable.empty())
	{
		sys_log(0, "RELOAD: item_attr_rare");
		m_vec_itemRareTable.clear();
	}

	m_vec_itemRareTable.reserve(pRes->uiNumRows);

	MYSQL_ROW data;

	while ((data = mysql_fetch_row(pRes->pSQLResult)))
	{
		TItemAttrTable t;

		memset(&t, 0, sizeof(TItemAttrTable));

		int col = 0;

		strlcpy(t.szApply, data[col++], sizeof(t.szApply));
		int32_t apply_idx = get_Item_ApplyType_Value(t.szApply);
		if (apply_idx >= 0)
			t.dwApplyIndex = apply_idx;
		else
			str_to_number(t.dwApplyIndex, data[col]);
		col++;
		str_to_number(t.dwProb, data[col++]);
		str_to_number(t.lValues[0], data[col++]);
		str_to_number(t.lValues[1], data[col++]);
		str_to_number(t.lValues[2], data[col++]);
		str_to_number(t.lValues[3], data[col++]);
		str_to_number(t.lValues[4], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_BODY], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_NECK], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD], data[col++]);
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_EAR], data[col++]);
#ifdef ENABLE_PENDANT_SYSTEM
		str_to_number(t.bMaxLevelBySet[ATTRIBUTE_SET_PENDANT], data[col++]);
#endif
		sys_log(0, "ITEM_RARE: %-20s %4lu { %3d %3d %3d %3d %3d } { %d %d %d %d %d %d %d }",
				t.szApply,
				t.dwProb,
				t.lValues[0],
				t.lValues[1],
				t.lValues[2],
				t.lValues[3],
				t.lValues[4],
				t.bMaxLevelBySet[ATTRIBUTE_SET_WEAPON],
				t.bMaxLevelBySet[ATTRIBUTE_SET_BODY],
				t.bMaxLevelBySet[ATTRIBUTE_SET_WRIST],
				t.bMaxLevelBySet[ATTRIBUTE_SET_FOOTS],
				t.bMaxLevelBySet[ATTRIBUTE_SET_NECK],
				t.bMaxLevelBySet[ATTRIBUTE_SET_HEAD],
				t.bMaxLevelBySet[ATTRIBUTE_SET_SHIELD],
				t.bMaxLevelBySet[ATTRIBUTE_SET_EAR]

				);

		m_vec_itemRareTable.push_back(t);
	}

	return true;
}

bool CClientManager::InitializeLandTable()
{
	using namespace building;

	char query[4096];

	#ifdef ENABLE_CLEAR_GUILD_LANDS
	CDBManager::instance().DirectQuery(
		"update land "
		"INNER JOIN guild ON land.guild_id = guild.id "
		"INNER JOIN player ON guild.`master` = player.id "
		"set guild_id=0 "
		"where guild_id > 0 and "
		"DATE_SUB(NOW(),INTERVAL 21 DAY) > last_play; "
	);

	CDBManager::instance().DirectQuery(
		"DELETE object "
		"FROM object "
		"INNER JOIN land ON land.id = object.land_id "
		"WHERE land.guild_id=0; "
	);
	#endif

	snprintf(query, sizeof(query),
		"SELECT id, map_index, x, y, width, height, guild_id, guild_level_limit, price "
		"FROM land%s WHERE enable='YES' ORDER BY id",
		GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!m_vec_kLandTable.empty())
	{
		sys_log(0, "RELOAD: land");
		m_vec_kLandTable.clear();
	}

	m_vec_kLandTable.reserve(pRes->uiNumRows);

	MYSQL_ROW	data;

	if (pRes->uiNumRows > 0)
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TLand t;

			memset(&t, 0, sizeof(t));

			int col = 0;

			str_to_number(t.dwID, data[col++]);
			str_to_number(t.lMapIndex, data[col++]);
			str_to_number(t.x, data[col++]);
			str_to_number(t.y, data[col++]);
			str_to_number(t.width, data[col++]);
			str_to_number(t.height, data[col++]);
			str_to_number(t.dwGuildID, data[col++]);
			str_to_number(t.bGuildLevelLimit, data[col++]);
			str_to_number(t.dwPrice, data[col++]);

			sys_log(0, "LAND: %lu map %-4ld %7ldx%-7ld w %-4ld h %-4ld", t.dwID, t.lMapIndex, t.x, t.y, t.width, t.height);

			m_vec_kLandTable.push_back(t);
		}

	return true;
}

void parse_pair_number_string(const char * c_pszString, std::vector<std::pair<int, int> > & vec)
{
	// format: 10,1/20,3/300,50
	const char * t = c_pszString;
	const char * p = strchr(t, '/');
	std::pair<int, int> k;

	char szNum[32 + 1];
	char * comma;

	while (p)
	{
		if (isnhdigit(*t))
		{
			strlcpy(szNum, t, MIN(sizeof(szNum), (p-t)+1));

			comma = strchr(szNum, ',');

			if (comma)
			{
				*comma = '\0';
				str_to_number(k.second, comma+1);
			}
			else
				k.second = 0;

			str_to_number(k.first, szNum);
			vec.push_back(k);
		}

		t = p + 1;
		p = strchr(t, '/');
	}

	if (isnhdigit(*t))
	{
		strlcpy(szNum, t, sizeof(szNum));

		comma = strchr(const_cast<char*>(t), ',');

		if (comma)
		{
			*comma = '\0';
			str_to_number(k.second, comma+1);
		}
		else
			k.second = 0;

		str_to_number(k.first, szNum);
		vec.push_back(k);
	}
}

bool CClientManager::InitializeObjectProto()
{
	using namespace building;

	char query[4096];
	snprintf(query, sizeof(query),
			"SELECT vnum, price, materials, upgrade_vnum, upgrade_limit_time, life, reg_1, reg_2, reg_3, reg_4, npc, group_vnum, dependent_group "
			"FROM object_proto%s ORDER BY vnum",
			GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!m_vec_kObjectProto.empty())
	{
		sys_log(0, "RELOAD: object_proto");
		m_vec_kObjectProto.clear();
	}

	m_vec_kObjectProto.reserve(MAX(0, pRes->uiNumRows));

	MYSQL_ROW	data;

	if (pRes->uiNumRows > 0)
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TObjectProto t;

			memset(&t, 0, sizeof(t));

			int col = 0;

			str_to_number(t.dwVnum, data[col++]);
			str_to_number(t.dwPrice, data[col++]);

			std::vector<std::pair<int, int> > vec;
			parse_pair_number_string(data[col++], vec);

			for (unsigned int i = 0; i < OBJECT_MATERIAL_MAX_NUM && i < vec.size(); ++i)
			{
				std::pair<int, int> & r = vec[i];

				t.kMaterials[i].dwItemVnum = r.first;
				t.kMaterials[i].dwCount = r.second;
			}

			str_to_number(t.dwUpgradeVnum, data[col++]);
			str_to_number(t.dwUpgradeLimitTime, data[col++]);
			str_to_number(t.lLife, data[col++]);
			str_to_number(t.lRegion[0], data[col++]);
			str_to_number(t.lRegion[1], data[col++]);
			str_to_number(t.lRegion[2], data[col++]);
			str_to_number(t.lRegion[3], data[col++]);

			// ADD_BUILDING_NPC
			str_to_number(t.dwNPCVnum, data[col++]);
			str_to_number(t.dwGroupVnum, data[col++]);
			str_to_number(t.dwDependOnGroupVnum, data[col++]);

			t.lNPCX = 0;
			t.lNPCY = MAX(t.lRegion[1], t.lRegion[3])+300;
			// END_OF_ADD_BUILDING_NPC

			sys_log(0, "OBJ_PROTO: vnum %lu price %lu mat %lu %lu",
					t.dwVnum, t.dwPrice, t.kMaterials[0].dwItemVnum, t.kMaterials[0].dwCount);

			m_vec_kObjectProto.push_back(t);
		}

	return true;
}

bool CClientManager::InitializeObjectTable()
{
	using namespace building;

	char query[4096];
	snprintf(query, sizeof(query), "SELECT id, land_id, vnum, map_index, x, y, x_rot, y_rot, z_rot, life FROM object%s ORDER BY id", GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult * pRes = pkMsg->Get();

	if (!m_map_pkObjectTable.empty())
	{
		sys_log(0, "RELOAD: object");
		m_map_pkObjectTable.clear();
	}

	MYSQL_ROW data;

	if (pRes->uiNumRows > 0)
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			TObject * k = new TObject;

			memset(k, 0, sizeof(TObject));

			int col = 0;

			str_to_number(k->dwID, data[col++]);
			str_to_number(k->dwLandID, data[col++]);
			str_to_number(k->dwVnum, data[col++]);
			str_to_number(k->lMapIndex, data[col++]);
			str_to_number(k->x, data[col++]);
			str_to_number(k->y, data[col++]);
			str_to_number(k->xRot, data[col++]);
			str_to_number(k->yRot, data[col++]);
			str_to_number(k->zRot, data[col++]);
			str_to_number(k->lLife, data[col++]);

			sys_log(0, "OBJ: %lu vnum %lu map %-4ld %7ldx%-7ld life %ld",
					k->dwID, k->dwVnum, k->lMapIndex, k->x, k->y, k->lLife);

			m_map_pkObjectTable.insert(std::make_pair(k->dwID, k));
		}

	return true;
}

#ifdef ENABLE_GROWTH_PET_SYSTEM
bool CClientManager::InitializeGrowthPetSkillTable()
{
	char query[4096];
	snprintf(query, sizeof(query), 
		"SELECT dwPetVnum, dwSkillVnum, szName, bType+0, dwCooldown, setAffectFlag+0, szPointOn," 
		"szPointPoly1, szPointPoly2, szPointPoly3, szPointPoly4, szPointPoly5, szPointPoly6, "
		"szPointPoly7, szPointPoly8, szActivatePctPoly, szDurationPoly "
		"FROM growth_pet_skill_proto%s", GetTablePostfix());

	std::unique_ptr<SQLMsg> pkMsg(CDBManager::instance().DirectQuery(query));
	SQLResult* pRes = pkMsg->Get();

	if (!m_vec_growthPetSkillTable.empty())
	{
		sys_log(0, "RELOAD: growth pet skill");
		m_vec_growthPetSkillTable.clear();
	}

	m_vec_growthPetSkillTable.reserve(pRes->uiNumRows);

	MYSQL_ROW data;
	TGrowthPetSkillTable t;

	if (pRes->uiNumRows > 0)
	{
		while ((data = mysql_fetch_row(pRes->pSQLResult)))
		{
			memset(&t, 0, sizeof(t));

			int col = 0;

			str_to_number(t.dwPetVnum, data[col++]);
			str_to_number(t.dwSkillVnum, data[col++]);
			strlcpy(t.szName, data[col++], sizeof(t.szName));
			str_to_number(t.bType, data[col++]);
			str_to_number(t.dwCooldown, data[col++]);
			str_to_number(t.dwAffectFlag, data[col++]);
			strlcpy(t.szPointOn, data[col++], sizeof(t.szPointOn));
			strlcpy(t.szPointPoly1, data[col++], sizeof(t.szPointPoly1));
			strlcpy(t.szPointPoly2, data[col++], sizeof(t.szPointPoly2));
			strlcpy(t.szPointPoly3, data[col++], sizeof(t.szPointPoly3));
			strlcpy(t.szPointPoly4, data[col++], sizeof(t.szPointPoly4));
			strlcpy(t.szPointPoly5, data[col++], sizeof(t.szPointPoly5));
			strlcpy(t.szPointPoly6, data[col++], sizeof(t.szPointPoly6));
			strlcpy(t.szPointPoly7, data[col++], sizeof(t.szPointPoly7));
			strlcpy(t.szPointPoly8, data[col++], sizeof(t.szPointPoly8));
			strlcpy(t.szActivatePctPoly, data[col++], sizeof(t.szActivatePctPoly));
			strlcpy(t.szDurationPoly, data[col++], sizeof(t.szDurationPoly));

			sys_log(0, "GROWTH PET SKILL: PET %u  SKILL %u", t.dwPetVnum, t.dwSkillVnum);

			m_vec_growthPetSkillTable.push_back(t);
		}
	}
	else
		return false;

	return true;
}
#endif