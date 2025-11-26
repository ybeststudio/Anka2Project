#include <stdio.h>
#include <string>
#include <map>
#include <set>
#include <algorithm>

#include "lzo.h"

#include "CsvFile.h"
#include "ItemCSVReader.h"

#pragma comment(lib, "lzo.lib")

#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

#define ENABLE_MULTI_LANGUAGE_SYSTEM

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
static std::string MLanguage[] =
{
	"en", "ro", "pt", "es", "fr", "de", "pl", "it", "cz", "hu", "tr"
};
#endif

enum EMisc
{
	CHARACTER_NAME_MAX_LEN = 48,
	MOB_SKILL_MAX_NUM = 5,
};

enum EMobEnchants
{
	MOB_ENCHANT_CURSE,
	MOB_ENCHANT_SLOW,
	MOB_ENCHANT_POISON,
	MOB_ENCHANT_STUN,
	MOB_ENCHANT_CRITICAL,
	MOB_ENCHANT_PENETRATE,
	MOB_ENCHANTS_MAX_NUM
};

enum EMobResists
{
	MOB_RESIST_SWORD,
	MOB_RESIST_TWOHAND,
	MOB_RESIST_DAGGER,
	MOB_RESIST_BELL,
	MOB_RESIST_FAN,
	MOB_RESIST_BOW,
	MOB_RESIST_FIRE,
	MOB_RESIST_ELECT,
	MOB_RESIST_MAGIC,
	MOB_RESIST_WIND,
	MOB_RESIST_POISON,
	MOB_RESISTS_MAX_NUM
};


#pragma pack(1)
typedef struct SMobSkillLevel
{
	DWORD dwVnum;
	BYTE bLevel;
} TMobSkillLevel;
#pragma pack()

#pragma pack(1)
typedef struct SMobTable
{
	DWORD dwVnum;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	char szLocaleName[CHARACTER_NAME_MAX_LEN + 1];

	BYTE bType;
	BYTE bRank;
	BYTE bBattleType;
	BYTE bLevel;
	BYTE bSize;

	DWORD dwGoldMin;
	DWORD dwGoldMax;
	DWORD dwExp;
	DWORD dwMaxHP;
	BYTE bRegenCycle;
	BYTE bRegenPercent;
	WORD wDef;

	DWORD dwAIFlag;
	DWORD dwRaceFlag;
	DWORD dwImmuneFlag;

	BYTE bStr, bDex, bCon, bInt;
	DWORD dwDamageRange[2];

	short sAttackSpeed;
	short sMovingSpeed;
	BYTE bAggresiveHPPct;
	WORD wAggressiveSight;
	WORD wAttackRange;

	char cEnchants[MOB_ENCHANTS_MAX_NUM];
	char cResists[MOB_RESISTS_MAX_NUM];

	DWORD dwResurrectionVnum;
	DWORD dwDropItemVnum;

	BYTE bMountCapacity;
	BYTE bOnClickType;

	BYTE bEmpire;
	char szFolder[64 + 1];

	float fDamMultiply;

	DWORD dwSummonVnum;
	DWORD dwDrainSP;
	DWORD dwMobColor;
	DWORD dwPolymorphItemVnum;

	TMobSkillLevel Skills[MOB_SKILL_MAX_NUM];

	BYTE bBerserkPoint;
	BYTE bStoneSkinPoint;
	BYTE bGodSpeedPoint;
	BYTE bDeathBlowPoint;
	BYTE bRevivePoint;
} TMobTable;
#pragma pack()

using namespace std;

TMobTable * m_pMobTable = NULL;
int m_iMobTableSize = 0;


enum EItemMisc
{
	ITEM_NAME_MAX_LEN = 48,
	ITEM_VALUES_MAX_NUM = 6,
	ITEM_SMALL_DESCR_MAX_LEN = 256,
	ITEM_LIMIT_MAX_NUM = 2,
	ITEM_APPLY_MAX_NUM = 3,
	ITEM_SOCKET_MAX_NUM = 3,
	ITEM_MAX_COUNT = 200,
	ITEM_ATTRIBUTE_MAX_NUM = 7,
	ITEM_ATTRIBUTE_MAX_LEVEL = 5,
	ITEM_AWARD_WHY_MAX_LEN = 50,

	REFINE_MATERIAL_MAX_NUM = 5,

	ITEM_ELK_VNUM = 50026,
};
#pragma pack(1)
typedef struct SItemLimit
{
	BYTE bType;
	long lValue;
} TItemLimit;
#pragma pack()

#pragma pack(1)
typedef struct SItemApply
{
	BYTE bType;
	long lValue;
} TItemApply;
#pragma pack()

#pragma pack(1)
typedef struct 
{
	DWORD dwVnum;
	DWORD dwVnumRange;
	char szName[ITEM_NAME_MAX_LEN + 1];
	char szLocaleName[ITEM_NAME_MAX_LEN + 1];
	BYTE bType;
	BYTE bSubType;

	BYTE bWeight;
	BYTE bSize;

	DWORD dwAntiFlags;
	DWORD dwFlags;
	DWORD dwWearFlags;
	DWORD dwImmuneFlag;

	DWORD dwGold;
	DWORD dwShopBuyPrice;

	TItemLimit aLimits[ITEM_LIMIT_MAX_NUM];
	TItemApply aApplies[ITEM_APPLY_MAX_NUM];
	long alValues[ITEM_VALUES_MAX_NUM];
	long alSockets[ITEM_SOCKET_MAX_NUM];
	DWORD dwRefinedVnum;
	WORD wRefineSet;
	BYTE bAlterToMagicItemPct;
	BYTE bSpecular;
	BYTE bGainSocketPct;
} TClientItemTable;
#pragma pack()

bool operator < (const TClientItemTable& lhs, const TClientItemTable& rhs)
{
	return lhs.dwVnum < rhs.dwVnum;
}

TClientItemTable * m_pItemTable = NULL;
int m_iItemTableSize = 0;


bool Set_Proto_Mob_Table(TMobTable *mobTable, cCsvTable &csvTable, std::map<int,const char*> &nameMap)
{
	int col = 0;

	mobTable->dwVnum = atoi(csvTable.AsStringByIndex(col++));
	strncpy(mobTable->szName, csvTable.AsStringByIndex(col++), CHARACTER_NAME_MAX_LEN);

	map<int,const char*>::iterator it;
	it = nameMap.find(mobTable->dwVnum);
	if (it != nameMap.end())
	{
		const char * localeName = it->second;
		strncpy(mobTable->szLocaleName, localeName, sizeof (mobTable->szLocaleName));
	}
	else
	{
		strncpy(mobTable->szLocaleName, mobTable->szName, sizeof (mobTable->szLocaleName));
	}

	int rankValue = get_Mob_Rank_Value(csvTable.AsStringByIndex(col++));
	mobTable->bRank = rankValue;

	int typeValue = get_Mob_Type_Value(csvTable.AsStringByIndex(col++));
	mobTable->bType = typeValue;

	int battleTypeValue = get_Mob_BattleType_Value(csvTable.AsStringByIndex(col++));
	mobTable->bBattleType = battleTypeValue;

	mobTable->bLevel = atoi(csvTable.AsStringByIndex(col++));

	int sizeValue = get_Mob_Size_Value(csvTable.AsStringByIndex(col++));
	mobTable->bSize = sizeValue;

	int aiFlagValue = get_Mob_AIFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwAIFlag = aiFlagValue;
	col++;

	int raceFlagValue = get_Mob_RaceFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwRaceFlag = raceFlagValue;

	int immuneFlagValue = get_Mob_ImmuneFlag_Value(csvTable.AsStringByIndex(col++));
	mobTable->dwImmuneFlag = immuneFlagValue;

	mobTable->bEmpire = atoi(csvTable.AsStringByIndex(col++));

	strncpy(mobTable->szFolder, csvTable.AsStringByIndex(col++), sizeof(mobTable->szFolder));


	mobTable->bOnClickType = atoi(csvTable.AsStringByIndex(col++));

	mobTable->bStr = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bDex = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bCon = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bInt = atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwDamageRange[0] = atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwDamageRange[1] = atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwMaxHP = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bRegenCycle = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bRegenPercent = atoi(csvTable.AsStringByIndex(col++));

	col++;
	col++;
	mobTable->dwExp = atoi(csvTable.AsStringByIndex(col++));
	mobTable->wDef = atoi(csvTable.AsStringByIndex(col++));
	mobTable->sAttackSpeed = atoi(csvTable.AsStringByIndex(col++));
	mobTable->sMovingSpeed = atoi(csvTable.AsStringByIndex(col++));
	mobTable->bAggresiveHPPct = atoi(csvTable.AsStringByIndex(col++));
	mobTable->wAggressiveSight = atoi(csvTable.AsStringByIndex(col++));
	mobTable->wAttackRange = atoi(csvTable.AsStringByIndex(col++));

	mobTable->dwDropItemVnum = atoi(csvTable.AsStringByIndex(col++));
	col++;

	for (int i = 0; i < MOB_ENCHANTS_MAX_NUM; ++i)
		mobTable->cEnchants[i] = atoi(csvTable.AsStringByIndex(col++));

	for (int i = 0; i < MOB_RESISTS_MAX_NUM; ++i)
		mobTable->cResists[i] = atoi(csvTable.AsStringByIndex(col++));

	mobTable->fDamMultiply = atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwSummonVnum = atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwDrainSP = atoi(csvTable.AsStringByIndex(col++));
	mobTable->dwMobColor = atoi(csvTable.AsStringByIndex(col++));

	return true;
}

static bool BuildMobTable(const char * cLang)
{
	fprintf(stderr, "sizeof(TMobTable): %u\n", sizeof(TMobTable));

	bool isNameFile = true;
	map<int,const char*> localMap;

	char fileName[256];
	snprintf(fileName, sizeof(fileName), "%s/mob_names.txt", cLang);

	cCsvTable nameData;
	if (!nameData.Load(fileName,'\t'))
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

	set<int> vnumSet;
	map<DWORD, TMobTable *> map_mobTableByVnum;

	cCsvTable data;
	if (!data.Load("mob_proto.txt",'\t'))
	{
		fprintf(stderr, "Failed to read mob_proto.txt\n");
		return false;
	}
	data.Next();

	if (m_pMobTable)
	{
		delete m_pMobTable;
		m_pMobTable = NULL;
	}

	int addNumber = 0;
	while(data.Next())
	{
		int vnum = atoi(data.AsStringByIndex(0));
		std::map<DWORD, TMobTable *>::iterator it_map_mobTable;
		it_map_mobTable = map_mobTableByVnum.find(vnum);

		if(it_map_mobTable != map_mobTableByVnum.end())
		{
			addNumber++;
		}
	}

	m_iMobTableSize = data.m_File.GetRowCount()-1 + addNumber;

	m_pMobTable = new TMobTable[m_iMobTableSize];
	memset(m_pMobTable, 0, sizeof(TMobTable) * m_iMobTableSize);

	TMobTable * mob_table = m_pMobTable;

	data.Destroy();
	if (!data.Load("mob_proto.txt",'\t'))
	{
		fprintf(stderr, "Failed to read mob_proto.txt\n");
		return false;
	}
	data.Next();

	while (data.Next())
	{
		int col = 0;

		std::map<DWORD, TMobTable *>::iterator it_map_mobTable;
		it_map_mobTable = map_mobTableByVnum.find(atoi(data.AsStringByIndex(col)));

		if(it_map_mobTable == map_mobTableByVnum.end())
		{
			if (!Set_Proto_Mob_Table(mob_table, data, localMap))
			{
				fprintf(stderr, "Failed to set mob proto table.\n");
			}
		}
		else
		{
			TMobTable *tempTable = it_map_mobTable->second;

			mob_table->dwVnum = tempTable->dwVnum;
			strncpy(mob_table->szName, tempTable->szName, CHARACTER_NAME_MAX_LEN);
			strncpy(mob_table->szLocaleName, tempTable->szLocaleName, CHARACTER_NAME_MAX_LEN);
			mob_table->bRank = tempTable->bRank;
			mob_table->bType = tempTable->bType;
			mob_table->bBattleType = tempTable->bBattleType;
			mob_table->bLevel = tempTable->bLevel;
			mob_table->bSize = tempTable->bSize;
			mob_table->dwAIFlag = tempTable->dwAIFlag;
			mob_table->dwRaceFlag = tempTable->dwRaceFlag;
			mob_table->dwImmuneFlag = tempTable->dwImmuneFlag;
			mob_table->bEmpire = tempTable->bEmpire;
			strncpy(mob_table->szFolder, tempTable->szFolder, CHARACTER_NAME_MAX_LEN);
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
			mob_table->dwExp = tempTable->dwExp;
			mob_table->wDef = tempTable->wDef;
			mob_table->sAttackSpeed = tempTable->sAttackSpeed;
			mob_table->sMovingSpeed = tempTable->sMovingSpeed;
			mob_table->bAggresiveHPPct = tempTable->bAggresiveHPPct;
			mob_table->wAggressiveSight = tempTable->wAggressiveSight;
			mob_table->wAttackRange = tempTable->wAttackRange;
			mob_table->dwDropItemVnum = tempTable->dwDropItemVnum;

			for (int i = 0; i < MOB_ENCHANTS_MAX_NUM; ++i)
				mob_table->cEnchants[i] = tempTable->cEnchants[i];

			for (int i = 0; i < MOB_RESISTS_MAX_NUM; ++i)
				mob_table->cResists[i] = tempTable->cResists[i];

			mob_table->fDamMultiply = tempTable->fDamMultiply;
			mob_table->dwSummonVnum = tempTable->dwSummonVnum;
			mob_table->dwDrainSP = tempTable->dwDrainSP;
			mob_table->dwMobColor = tempTable->dwMobColor;
		}

		fprintf(stdout, "MOB #%-5d %-16s %-16s sight: %u color %u[%s]\n", mob_table->dwVnum, mob_table->szName, mob_table->szLocaleName, mob_table->wAggressiveSight, mob_table->dwMobColor, 0);

		vnumSet.insert(mob_table->dwVnum);

		++mob_table;
	}
	return true;
}

DWORD g_adwMobProtoKey[4] =
{
	4813894,
	18955,
	552631,
	6822045
};

static void SaveMobProto(const char * cLang)
{
	char fileName[256];
	snprintf(fileName, sizeof(fileName), "build/%s/mob_proto", cLang);

	FILE * fp;
	fp = fopen(fileName, "wb");

	if (!fp)
	{
		printf("cannot open %s for writing\n", fileName);
		return;
	}

	DWORD fourcc = MAKEFOURCC('M', 'M', 'P', 'T');
	fwrite(&fourcc, sizeof(DWORD), 1, fp);

	DWORD dwElements = m_iMobTableSize;
	fwrite(&dwElements, sizeof(DWORD), 1, fp);

	CLZObject zObj;

	printf("sizeof(TMobTable) %d\n", sizeof(TMobTable));

	if (!CLZO::instance().CompressEncryptedMemory(zObj, m_pMobTable, sizeof(TMobTable) * m_iMobTableSize, g_adwMobProtoKey))  
	{
		printf("cannot compress\n");
		fclose(fp);
		return;
	}

	const CLZObject::THeader & r = zObj.GetHeader();

	printf("MobProto count %u\n%u --Compress--> %u --Encrypt--> %u, GetSize %u\n", m_iMobTableSize, r.dwRealSize, r.dwCompressedSize, r.dwEncryptSize, zObj.GetSize());

	DWORD dwDataSize = zObj.GetSize();
	fwrite(&dwDataSize, sizeof(DWORD), 1, fp);
	fwrite(zObj.GetBuffer(), dwDataSize, 1, fp);

	fclose(fp);
}

void LoadMobProto()
{
	FILE * fp;
	DWORD fourcc, tableSize, dataSize;

	fp = fopen("mob_proto", "rb");

	fread(&fourcc, sizeof(DWORD), 1, fp);
	fread(&tableSize, sizeof(DWORD), 1, fp);
	fread(&dataSize, sizeof(DWORD), 1, fp);
	BYTE * data = (BYTE *) malloc(dataSize);

	if (data)
	{
		fread(data, dataSize, 1, fp);

		CLZObject zObj;

		if (CLZO::instance().Decompress(zObj, data, g_adwMobProtoKey))
		{
			printf("real_size %u\n", zObj.GetSize());

			for (DWORD i = 0; i < tableSize; ++i)
			{
				TMobTable & rTable = *((TMobTable *) zObj.GetBuffer() + i);
				printf("%u %s\n", rTable.dwVnum, rTable.szName);
			}
		}

		free(data);
	}

	fclose(fp);
}

bool Set_Proto_Item_Table(TClientItemTable *itemTable, cCsvTable &csvTable, std::map<int,const char*> &nameMap)
{
	{
		std::string s(csvTable.AsStringByIndex(0));
		int pos = s.find("~");

		if (std::string::npos == pos)
		{
			itemTable->dwVnum = atoi(s.c_str());
			if (0 == itemTable->dwVnum)
			{
				printf ("INVALID VNUM %s\n", s.c_str());
				return false;
			}
			itemTable->dwVnumRange = 0;
		}
		else
		{
			std::string s_start_vnum (s.substr(0, pos));
			std::string s_end_vnum (s.substr(pos +1 ));

			int start_vnum = atoi(s_start_vnum.c_str());
			int end_vnum = atoi(s_end_vnum.c_str());
			if (0 == start_vnum || (0 != end_vnum && end_vnum < start_vnum))
			{
				printf ("INVALID VNUM RANGE%s\n", s.c_str());
				return false;
			}
			itemTable->dwVnum = start_vnum;
			itemTable->dwVnumRange = end_vnum - start_vnum;
		}
	}

	int col = 1;

	strncpy(itemTable->szName, csvTable.AsStringByIndex(col++), ITEM_NAME_MAX_LEN);

	map<int,const char*>::iterator it;
	it = nameMap.find(itemTable->dwVnum);
	if (it != nameMap.end())
	{
		const char * localeName = it->second;
		strncpy(itemTable->szLocaleName, localeName, sizeof(itemTable->szLocaleName));
	}
	else
	{
		strncpy(itemTable->szLocaleName, itemTable->szName, sizeof(itemTable->szLocaleName));
	}
	itemTable->bType = get_Item_Type_Value(csvTable.AsStringByIndex(col++));
	itemTable->bSubType = get_Item_SubType_Value(itemTable->bType, csvTable.AsStringByIndex(col++));
	itemTable->bSize = atoi(csvTable.AsStringByIndex(col++));
	itemTable->dwAntiFlags = get_Item_AntiFlag_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwFlags = get_Item_Flag_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwWearFlags = get_Item_WearFlag_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwImmuneFlag = get_Item_Immune_Value(csvTable.AsStringByIndex(col++));
	itemTable->dwGold = atoi(csvTable.AsStringByIndex(col++));
	itemTable->dwShopBuyPrice = atoi(csvTable.AsStringByIndex(col++));
	itemTable->dwRefinedVnum = atoi(csvTable.AsStringByIndex(col++));
	itemTable->wRefineSet = atoi(csvTable.AsStringByIndex(col++));
	itemTable->bAlterToMagicItemPct = atoi(csvTable.AsStringByIndex(col++));

	int i;

	for (i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		itemTable->aLimits[i].bType = get_Item_LimitType_Value(csvTable.AsStringByIndex(col++));
		itemTable->aLimits[i].lValue = atoi(csvTable.AsStringByIndex(col++));
	}

	for (i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		itemTable->aApplies[i].bType = get_Item_ApplyType_Value(csvTable.AsStringByIndex(col++));
		itemTable->aApplies[i].lValue = atoi(csvTable.AsStringByIndex(col++));
	}

	for (i = 0; i < ITEM_VALUES_MAX_NUM; ++i)
		itemTable->alValues[i] = atoi(csvTable.AsStringByIndex(col++));

	itemTable->bSpecular = atoi(csvTable.AsStringByIndex(col++));
	itemTable->bGainSocketPct = atoi(csvTable.AsStringByIndex(col++));
	col++;

	itemTable->bWeight = 0;

	return true;
}

static bool BuildItemTable(const char * cLang)
{
	fprintf(stderr, "sizeof(TClientItemTable): %u\n", sizeof(TClientItemTable));

	bool isNameFile = true;
	map<int,const char*> localMap;

	char fileName[256];
	snprintf(fileName, sizeof(fileName), "%s/item_names.txt", cLang);

	cCsvTable nameData;
	if (!nameData.Load(fileName,'\t'))
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

	map<DWORD, TClientItemTable *> map_itemTableByVnum;
	set<int> vnumSet;

	cCsvTable data;
	if (!data.Load("item_proto.txt",'\t'))
	{
		fprintf(stderr, "Failed to read item_proto.txt\n");
		return false;
	}
	data.Next();

	if (m_pItemTable)
	{
		free(m_pItemTable);
		m_pItemTable = NULL;
	}

	int addNumber = 0;
	while(data.Next())
	{
		int vnum = atoi(data.AsStringByIndex(0));
		std::map<DWORD, TClientItemTable *>::iterator it_map_itemTable;
		it_map_itemTable = map_itemTableByVnum.find(vnum);

		if(it_map_itemTable != map_itemTableByVnum.end())
		{
			addNumber++;
		}
	}

	data.Destroy();
	if (!data.Load("item_proto.txt",'\t'))
	{
		fprintf(stderr, "Failed to read item_proto.txt\n");
		return false;
	}
	data.Next();

	m_iItemTableSize = data.m_File.GetRowCount()-1+addNumber;
	m_pItemTable = new TClientItemTable[m_iItemTableSize];
	memset(m_pItemTable, 0, sizeof(TClientItemTable) * m_iItemTableSize);

	TClientItemTable * item_table = m_pItemTable;

	while (data.Next())
	{
		int col = 0;

		std::map<DWORD, TClientItemTable *>::iterator it_map_itemTable;
		it_map_itemTable = map_itemTableByVnum.find(atoi(data.AsStringByIndex(col)));

		if(it_map_itemTable == map_itemTableByVnum.end())
		{
			if (!Set_Proto_Item_Table(item_table, data, localMap))
			{
				fprintf(stderr, "Failed to set item proto table.\n");
			}
		}
		else
		{
			TClientItemTable *tempTable = it_map_itemTable->second;

			item_table->dwVnum = tempTable->dwVnum;
			strncpy(item_table->szName, tempTable->szName, ITEM_NAME_MAX_LEN);
			strncpy(item_table->szLocaleName, tempTable->szLocaleName, ITEM_NAME_MAX_LEN);
			item_table->bType = tempTable->bType;
			item_table->bSubType = tempTable->bSubType;
			item_table->bSize = tempTable->bSize;
			item_table->dwAntiFlags = tempTable->dwAntiFlags;
			item_table->dwFlags = tempTable->dwFlags;
			item_table->dwWearFlags = tempTable->dwWearFlags;
			item_table->dwImmuneFlag = tempTable->dwImmuneFlag;
			item_table->dwGold = tempTable->dwGold;
			item_table->dwShopBuyPrice = tempTable->dwShopBuyPrice;
			item_table->dwRefinedVnum = tempTable->dwRefinedVnum;
			item_table->wRefineSet = tempTable->wRefineSet;
			item_table->bAlterToMagicItemPct = tempTable->bAlterToMagicItemPct;

			int i;
			for (i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
			{
				item_table->aLimits[i].bType = tempTable->aLimits[i].bType;
				item_table->aLimits[i].lValue = tempTable->aLimits[i].lValue;
			}

			for (i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
			{
				item_table->aApplies[i].bType = tempTable->aApplies[i].bType;
				item_table->aApplies[i].lValue = tempTable->aApplies[i].lValue;
			}

			for (i = 0; i < ITEM_VALUES_MAX_NUM; ++i)
				item_table->alValues[i] = tempTable->alValues[i];

			item_table->bSpecular = tempTable->bSpecular;
			item_table->bGainSocketPct = tempTable->bGainSocketPct;
			item_table->bWeight = tempTable->bWeight;

		}

		fprintf(stdout, "ITEM #%-5u %-24s %-24s VAL: %ld %ld %ld %ld %ld %ld WEAR %u ANTI %u IMMUNE %u REFINE %u\n",
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
				item_table->dwRefinedVnum);

		vnumSet.insert(item_table->dwVnum);
		++item_table;
	}

	return true;
}

DWORD g_adwItemProtoKey[4] =
{
	173217,
	72619434,
	408587239,
	27973291
};

static void SaveItemProto(const char * cLang)
{
	char fileName[256];
	snprintf(fileName, sizeof(fileName), "build/%s/item_proto", cLang);

	FILE * fp;
	fp = fopen(fileName, "wb");

	if (!fp)
	{
		printf("cannot open %s for writing\n", fileName);
		return;
	}

	DWORD fourcc = MAKEFOURCC('M', 'I', 'P', 'X');
	fwrite(&fourcc, sizeof(DWORD), 1, fp);

	DWORD dwVersion = 0x00000001;
	fwrite(&dwVersion, sizeof(DWORD), 1, fp);

	DWORD dwStride = sizeof(TClientItemTable);
	fwrite(&dwStride, sizeof(DWORD), 1, fp);

	DWORD dwElements = m_iItemTableSize;
	fwrite(&dwElements, sizeof(DWORD), 1, fp);

	CLZObject zObj;
	std::vector <TClientItemTable> vec_item_table (&m_pItemTable[0], &m_pItemTable[m_iItemTableSize - 1]);
	sort (&m_pItemTable[0], &m_pItemTable[0] + m_iItemTableSize);

	if (!CLZO::instance().CompressEncryptedMemory(zObj, m_pItemTable, sizeof(TClientItemTable) * m_iItemTableSize, g_adwItemProtoKey)) 
	{
		printf("cannot compress\n");
		fclose(fp);
		return;
	}

	const CLZObject::THeader & r = zObj.GetHeader();

	printf("Elements %d\n%u --Compress--> %u --Encrypt--> %u, GetSize %u\n", m_iItemTableSize, r.dwRealSize, r.dwCompressedSize, r.dwEncryptSize, zObj.GetSize());

	DWORD dwDataSize = zObj.GetSize();
	fwrite(&dwDataSize, sizeof(DWORD), 1, fp);
	fwrite(zObj.GetBuffer(), dwDataSize, 1, fp);

	fclose(fp);

	fp = fopen("item_proto", "rb");

	if (!fp)
	{
		printf("Error!!\n");
		return;
	}

	fread(&fourcc, sizeof(DWORD), 1, fp);
	fread(&dwElements, sizeof(DWORD), 1, fp);

	printf("Elements Check %u fourcc match %d\n", dwElements, fourcc == MAKEFOURCC('M', 'I', 'P', 'T'));
	fclose(fp);
}

int main(int argc, char ** argv)
{
	bool bXPhase = false;

	if (bXPhase)
		LoadMobProto();
	else
	{
		for (int i = 0; i < 11; i++)
		{
			if (BuildMobTable(MLanguage[i].c_str()))
				SaveMobProto(MLanguage[i].c_str());
		}
	}

	for (int i = 0; i < 11; i++)
	{
		if (BuildItemTable(MLanguage[i].c_str()))
			SaveItemProto(MLanguage[i].c_str());
	}

	return 0;
}
