#include <stdio.h>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <stdarg.h>
#include <time.h>
#if defined(_MSC_VER)
#include <direct.h>
#include <windows.h>
#endif

#include "lzo.h"

#include "CsvFile.h"
#include "ItemCSVReader.h"

#pragma comment(lib, "lzo.lib")

#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))

#define ENABLE_MULTI_LANGUAGE_SYSTEM

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;

#if defined(_MSC_VER)
static FILE* g_fpDumpProtoLog = NULL;
static const char* g_ctxPhase = "?";
static const char* g_ctxLang = "?";
static int g_ctxRow = 0;
static DWORD g_ctxVnum = 0;

static void EnsureLogDir()
{
	_mkdir("logs");
}

#if defined(_MSC_VER)
static void EnsureBuildLangDir(const char* cLang)
{
	_mkdir("build");
	char dirPath[256];
	snprintf(dirPath, sizeof(dirPath), "build/%s", cLang);
	_mkdir(dirPath);
}
#else
static void EnsureBuildLangDir(const char* cLang) { (void)cLang; }
#endif

static void OpenLogFile()
{
	if (g_fpDumpProtoLog)
		return;

	EnsureLogDir();

	time_t t = time(NULL);
	struct tm tmv;
	localtime_s(&tmv, &t);

	char path[256];
	snprintf(path, sizeof(path), "logs/dump_proto_%04d%02d%02d_%02d%02d%02d.log",
		tmv.tm_year + 1900, tmv.tm_mon + 1, tmv.tm_mday,
		tmv.tm_hour, tmv.tm_min, tmv.tm_sec);

	g_fpDumpProtoLog = fopen(path, "a");
	if (g_fpDumpProtoLog)
	{
		fprintf(g_fpDumpProtoLog, "dump_proto log start\n");
		fflush(g_fpDumpProtoLog);
	}
}

static void LogProtoError(const char* fmt, ...)
{
	OpenLogFile();

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");
	va_end(ap);

	if (!g_fpDumpProtoLog)
		return;

	va_start(ap, fmt);
	vfprintf(g_fpDumpProtoLog, fmt, ap);
	fprintf(g_fpDumpProtoLog, "\n");
	va_end(ap);
	fflush(g_fpDumpProtoLog);
}

static void CloseLogFile()
{
	if (!g_fpDumpProtoLog)
		return;

	fprintf(g_fpDumpProtoLog, "dump_proto log end\n");
	fclose(g_fpDumpProtoLog);
	g_fpDumpProtoLog = NULL;
}

static LONG WINAPI DumpProtoUnhandledExceptionFilter(EXCEPTION_POINTERS* p)
{
	OpenLogFile();
	if (g_fpDumpProtoLog && p && p->ExceptionRecord)
	{
		fprintf(g_fpDumpProtoLog, "FATAL: unhandled exception code=0x%08X addr=%p\n",
			(unsigned)p->ExceptionRecord->ExceptionCode,
			p->ExceptionRecord->ExceptionAddress);
		fprintf(g_fpDumpProtoLog, "CTX: phase=%s lang=%s row=%d vnum=%u\n",
			g_ctxPhase ? g_ctxPhase : "?", g_ctxLang ? g_ctxLang : "?", g_ctxRow, (unsigned)g_ctxVnum);
		fflush(g_fpDumpProtoLog);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}
#else
static void LogProtoError(const char* fmt, ...) { (void)fmt; }
#endif

#if defined(_MSC_VER)
static void SetCtx(const char* phase, const char* lang, int row, DWORD vnum)
{
	g_ctxPhase = phase ? phase : "?";
	g_ctxLang = lang ? lang : "?";
	g_ctxRow = row;
	g_ctxVnum = vnum;
}
#else
static void SetCtx(const char* phase, const char* lang, int row, DWORD vnum) { (void)phase; (void)lang; (void)row; (void)vnum; }
#endif

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

	float fHitRange;
} TMobTable;
#pragma pack()

using namespace std;

TMobTable * m_pMobTable = NULL;
int m_iMobTableSize = 0;

enum EItemMisc
{
	ITEM_NAME_MAX_LEN			= 48,
	ITEM_VALUES_MAX_NUM			= 6,
	ITEM_SMALL_DESCR_MAX_LEN	= 256,
	ITEM_LIMIT_MAX_NUM			= 2,
	ITEM_APPLY_MAX_NUM			= 3,
	ITEM_SOCKET_MAX_NUM			= 3,
	ITEM_MAX_COUNT				= 200,
	ITEM_ATTRIBUTE_MAX_NUM		= 7,
	ITEM_ATTRIBUTE_MAX_LEVEL	= 5,
	ITEM_AWARD_WHY_MAX_LEN		= 50,

	REFINE_MATERIAL_MAX_NUM		= 5,

	ITEM_ELK_VNUM				= 50026,
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
	WORD bType;
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
	if (csvTable.ColCount() >= 85)
	{
		// Official GF 94-column format
		mobTable->bRank = get_Mob_Rank_Value(csvTable.AsStringByIndex(col++));
		mobTable->bType = get_Mob_Type_Value(csvTable.AsStringByIndex(col++));
		mobTable->bBattleType = get_Mob_BattleType_Value(csvTable.AsStringByIndex(col++));
		mobTable->bLevel = atoi(csvTable.AsStringByIndex(col++));

		col++; // 6: ScalePct
		mobTable->bSize = atoi(csvTable.AsStringByIndex(col++)); // 7: Size
		mobTable->dwAIFlag = get_Mob_AIFlag_Value(csvTable.AsStringByIndex(col++)); // 8: AIFlags
		mobTable->bMountCapacity = atoi(csvTable.AsStringByIndex(col++)); // 9: MountCapacity
		mobTable->dwRaceFlag = get_Mob_RaceFlag_Value(csvTable.AsStringByIndex(col++)); // 10: RaceFlags
		mobTable->dwImmuneFlag = get_Mob_ImmuneFlag_Value(csvTable.AsStringByIndex(col++)); // 11: ImmuneFlags (STUN, etc.)
		mobTable->bEmpire = atoi(csvTable.AsStringByIndex(col++)); // 12: Empire
		strncpy(mobTable->szFolder, csvTable.AsStringByIndex(col++), sizeof(mobTable->szFolder)); // 13: Folder
		mobTable->bOnClickType = atoi(csvTable.AsStringByIndex(col++)); // 14: OnClick
		mobTable->bStr = atoi(csvTable.AsStringByIndex(col++)); // 15: St
		mobTable->bDex = atoi(csvTable.AsStringByIndex(col++)); // 16: Dx
		mobTable->bCon = atoi(csvTable.AsStringByIndex(col++)); // 17: Ht
		mobTable->bInt = atoi(csvTable.AsStringByIndex(col++)); // 18: Iq

		col += 4; // 19..22: SungMa stats (SungMaSt, SungMaDx, SungMaHt, SungMaIq)

		mobTable->dwDamageRange[0] = atoi(csvTable.AsStringByIndex(col++)); // 23: MinDamage
		mobTable->dwDamageRange[1] = atoi(csvTable.AsStringByIndex(col++)); // 24: MaxDamage
		mobTable->dwMaxHP = atoi(csvTable.AsStringByIndex(col++)); // 25: MaxHp
		mobTable->bRegenCycle = atoi(csvTable.AsStringByIndex(col++)); // 26: RegenCycle
		mobTable->bRegenPercent = atoi(csvTable.AsStringByIndex(col++)); // 27: RegenPercent
		mobTable->dwGoldMin = atoi(csvTable.AsStringByIndex(col++)); // 28: MinGold
		mobTable->dwGoldMax = atoi(csvTable.AsStringByIndex(col++)); // 29: MaxGold
		mobTable->dwExp = atoi(csvTable.AsStringByIndex(col++)); // 30: Exp

		col++; // 31: SungMaExp

		mobTable->wDef = atoi(csvTable.AsStringByIndex(col++)); // 32: Def
		mobTable->sAttackSpeed = atoi(csvTable.AsStringByIndex(col++)); // 33: AttackSpeed
		mobTable->sMovingSpeed = atoi(csvTable.AsStringByIndex(col++)); // 34: MoveSpeed
		mobTable->bAggresiveHPPct = atoi(csvTable.AsStringByIndex(col++)); // 35: AggressiveHpPct
		mobTable->wAggressiveSight = atoi(csvTable.AsStringByIndex(col++)); // 36: AggressiveSight
		mobTable->wAttackRange = atoi(csvTable.AsStringByIndex(col++)); // 37: AttackRange
		mobTable->dwDropItemVnum = atoi(csvTable.AsStringByIndex(col++)); // 38: DropItemGroup
		mobTable->dwResurrectionVnum = atoi(csvTable.AsStringByIndex(col++)); // 39: ResurrectionVnum

		// 40..45: 6 Enchants: Curse, Slow, Poison, Stun, Critical, Penetrate
		for (int i = 0; i < MOB_ENCHANTS_MAX_NUM && i < 6; ++i)
			mobTable->cEnchants[i] = atoi(csvTable.AsStringByIndex(col++));

		col++; // 46: ResistFist (skip)

		// 47..52: Sword, TwoHand, Dagger, Bell, Fan, Bow
		for (int i = 0; i < 6; ++i)
			mobTable->cResists[i] = atoi(csvTable.AsStringByIndex(col++));

		col++; // 53: ResistClaw (skip)

		// 54..58: Fire, Elect, Magic, Wind, Poison
		for (int i = 6; i < MOB_RESISTS_MAX_NUM && i < 11; ++i)
			mobTable->cResists[i] = atoi(csvTable.AsStringByIndex(col++));

		col += 10; // 59..68: ResistBleeding, AttElec..AttDark, ResistDark, ResistIce, ResistEarth

		mobTable->fDamMultiply = (float)atof(csvTable.AsStringByIndex(col++)); // 69: DamMultiply
		mobTable->dwSummonVnum = atoi(csvTable.AsStringByIndex(col++)); // 70: SummonVnum
		mobTable->dwDrainSP = atoi(csvTable.AsStringByIndex(col++)); // 71: DrainSp
		mobTable->dwMobColor = atoi(csvTable.AsStringByIndex(col++)); // 72: MobColor
		mobTable->dwPolymorphItemVnum = atoi(csvTable.AsStringByIndex(col++)); // 73: PolymorphItem

		// 74..83: 5 Skills (Level, Vnum)
		for (int i = 0; i < MOB_SKILL_MAX_NUM && i < 5; ++i)
		{
			mobTable->Skills[i].bLevel = atoi(csvTable.AsStringByIndex(col++));
			mobTable->Skills[i].dwVnum = atoi(csvTable.AsStringByIndex(col++));
		}

		// 84..88: SpBerserk..SpRevive
		mobTable->bBerserkPoint = atoi(csvTable.AsStringByIndex(col++));
		mobTable->bStoneSkinPoint = atoi(csvTable.AsStringByIndex(col++));
		mobTable->bGodSpeedPoint = atoi(csvTable.AsStringByIndex(col++));
		mobTable->bDeathBlowPoint = atoi(csvTable.AsStringByIndex(col++));
		mobTable->bRevivePoint = atoi(csvTable.AsStringByIndex(col++));

		col += 4; // 89..92: SpHealer..SpRHpRegen

		if (col < (int)csvTable.ColCount())
			mobTable->fHitRange = (float)atof(csvTable.AsStringByIndex(col++));
		else
			mobTable->fHitRange = 0.0f;
	}
	else
	{
		// Legacy (71-72 columns)
		// 4. RANK
		int rankValue = get_Mob_Rank_Value(csvTable.AsStringByIndex(col++));
		mobTable->bRank = rankValue;
		// 5. TYPE
		int typeValue = get_Mob_Type_Value(csvTable.AsStringByIndex(col++));
		mobTable->bType = typeValue;
		// 6. BATTLE_TYPE
		int battleTypeValue = get_Mob_BattleType_Value(csvTable.AsStringByIndex(col++));
		mobTable->bBattleType = battleTypeValue;

		mobTable->bLevel = atoi(csvTable.AsStringByIndex(col++));

		int sizeValue = get_Mob_Size_Value(csvTable.AsStringByIndex(col++));
		mobTable->bSize = sizeValue;
		// 9. AI_FLAG
		int aiFlagValue = get_Mob_AIFlag_Value(csvTable.AsStringByIndex(col++));
		mobTable->dwAIFlag = aiFlagValue;
		col++; // mount_capacity;
		// 10. RACE_FLAG
		int raceFlagValue = get_Mob_RaceFlag_Value(csvTable.AsStringByIndex(col++));
		mobTable->dwRaceFlag = raceFlagValue;
		// 11. IMMUNE_FLAG
		int immuneFlagValue = get_Mob_ImmuneFlag_Value(csvTable.AsStringByIndex(col++));
		mobTable->dwImmuneFlag = immuneFlagValue;

		mobTable->bEmpire = atoi(csvTable.AsStringByIndex(col++));

		// FOLDER
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

		mobTable->dwPolymorphItemVnum = atoi(csvTable.AsStringByIndex(col++));

		for (int i = 0; i < MOB_SKILL_MAX_NUM; ++i)
		{
			mobTable->Skills[i].bLevel = atoi(csvTable.AsStringByIndex(col++));
			mobTable->Skills[i].dwVnum = atoi(csvTable.AsStringByIndex(col++));
		}

		mobTable->bBerserkPoint = atoi(csvTable.AsStringByIndex(col++));
		mobTable->bStoneSkinPoint = atoi(csvTable.AsStringByIndex(col++));
		mobTable->bGodSpeedPoint = atoi(csvTable.AsStringByIndex(col++));
		mobTable->bDeathBlowPoint = atoi(csvTable.AsStringByIndex(col++));
		mobTable->bRevivePoint = atoi(csvTable.AsStringByIndex(col++));

		if (col < (int)csvTable.ColCount())
			mobTable->fHitRange = (float)atof(csvTable.AsStringByIndex(col++));
		else
			mobTable->fHitRange = 0.0f;
	}

	return true;
}

static bool BuildMobTable(const char * cLang)
{
	fprintf(stderr, "sizeof(TMobTable): %u\n", sizeof(TMobTable));
	SetCtx("BuildMobTable", cLang, 0, 0);

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
			if (nameData.ColCount() >= 2)
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
		delete[] m_pMobTable;
		m_pMobTable = NULL;
	}

	int addNumber = 0;
	int validRowCount = 0;
	while(data.Next())
	{
		if (data.ColCount() < 71)
			continue;
		validRowCount++;
		int vnum = atoi(data.AsStringByIndex(0));
		std::map<DWORD, TMobTable *>::iterator it_map_mobTable;
		it_map_mobTable = map_mobTableByVnum.find(vnum);

		if(it_map_mobTable != map_mobTableByVnum.end())
		{
			addNumber++;
		}
	}

	m_iMobTableSize = validRowCount + addNumber;

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

	int rowIndex = 0;
	while (data.Next())
	{
		++rowIndex;
		if (data.ColCount() < 71)
			continue;
		int col = 0;
		DWORD vnum = (DWORD)atoi(data.AsStringByIndex(col));
		SetCtx("BuildMobTable:row", cLang, rowIndex, vnum);

		std::map<DWORD, TMobTable *>::iterator it_map_mobTable;
		it_map_mobTable = map_mobTableByVnum.find((DWORD)atoi(data.AsStringByIndex(col)));

		if(it_map_mobTable == map_mobTableByVnum.end())
		{
			if (!Set_Proto_Mob_Table(mob_table, data, localMap))
			{
				LogProtoError("[MOB][%s] row=%d vnum=%u Failed to set mob proto table.", cLang ? cLang : "?", rowIndex, (unsigned)vnum);
			}
		}
		else
		{
			TMobTable *tempTable = it_map_mobTable->second;

			mob_table->dwVnum               = tempTable->dwVnum;
			strncpy(mob_table->szName, tempTable->szName, CHARACTER_NAME_MAX_LEN);
			strncpy(mob_table->szLocaleName, tempTable->szLocaleName, CHARACTER_NAME_MAX_LEN);
			mob_table->bRank                = tempTable->bRank;
			mob_table->bType                = tempTable->bType;
			mob_table->bBattleType          = tempTable->bBattleType;
			mob_table->bLevel				= tempTable->bLevel;
			mob_table->bSize				= tempTable->bSize;
			mob_table->dwAIFlag				= tempTable->dwAIFlag;
			mob_table->dwRaceFlag				= tempTable->dwRaceFlag;
			mob_table->dwImmuneFlag				= tempTable->dwImmuneFlag;
			mob_table->bEmpire				= tempTable->bEmpire;
			strncpy(mob_table->szFolder, tempTable->szFolder, CHARACTER_NAME_MAX_LEN);
			mob_table->bOnClickType         = tempTable->bOnClickType;
			mob_table->bStr                 = tempTable->bStr;
			mob_table->bDex                 = tempTable->bDex;
			mob_table->bCon                 = tempTable->bCon;
			mob_table->bInt                 = tempTable->bInt;
			mob_table->dwDamageRange[0]     = tempTable->dwDamageRange[0];
			mob_table->dwDamageRange[1]     = tempTable->dwDamageRange[1];
			mob_table->dwMaxHP              = tempTable->dwMaxHP;
			mob_table->bRegenCycle          = tempTable->bRegenCycle;
			mob_table->bRegenPercent        = tempTable->bRegenPercent;
			mob_table->dwExp                = tempTable->dwExp;
			mob_table->wDef                 = tempTable->wDef;
			mob_table->sAttackSpeed         = tempTable->sAttackSpeed;
			mob_table->sMovingSpeed         = tempTable->sMovingSpeed;
			mob_table->bAggresiveHPPct      = tempTable->bAggresiveHPPct;
			mob_table->wAggressiveSight	= tempTable->wAggressiveSight;
			mob_table->wAttackRange		= tempTable->wAttackRange;
			mob_table->dwDropItemVnum	= tempTable->dwDropItemVnum;

			for (int i = 0; i < MOB_ENCHANTS_MAX_NUM; ++i)
				mob_table->cEnchants[i] = tempTable->cEnchants[i];

			for (int i = 0; i < MOB_RESISTS_MAX_NUM; ++i)
				mob_table->cResists[i] = tempTable->cResists[i];

			mob_table->fDamMultiply = tempTable->fDamMultiply;
			mob_table->dwSummonVnum = tempTable->dwSummonVnum;
			mob_table->dwDrainSP = tempTable->dwDrainSP;
			mob_table->dwMobColor = tempTable->dwMobColor;
			mob_table->dwPolymorphItemVnum = tempTable->dwPolymorphItemVnum;
			for (int i = 0; i < MOB_SKILL_MAX_NUM; ++i)
				mob_table->Skills[i] = tempTable->Skills[i];
			mob_table->bBerserkPoint = tempTable->bBerserkPoint;
			mob_table->bStoneSkinPoint = tempTable->bStoneSkinPoint;
			mob_table->bGodSpeedPoint = tempTable->bGodSpeedPoint;
			mob_table->bDeathBlowPoint = tempTable->bDeathBlowPoint;
			mob_table->bRevivePoint = tempTable->bRevivePoint;
			mob_table->fHitRange = tempTable->fHitRange;
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
	SetCtx("SaveMobProto", cLang, 0, 0);

	if (!m_pMobTable || m_iMobTableSize == 0)
	{
		LogProtoError("SaveMobProto skip: m_pMobTable=%p m_iMobTableSize=%d", (void*)m_pMobTable, (int)m_iMobTableSize);
		return;
	}

	EnsureBuildLangDir(cLang);
	char fileName[256];
	snprintf(fileName, sizeof(fileName), "build/%s/mob_proto", cLang);

	FILE * fp;
	fp = fopen(fileName, "wb");

	if (!fp)
	{
		LogProtoError("cannot open %s for writing", fileName);
		return;
	}

	DWORD fourcc = MAKEFOURCC('M', 'M', 'P', 'T');
	fwrite(&fourcc, sizeof(DWORD), 1, fp);

	DWORD dwElements = m_iMobTableSize;
	fwrite(&dwElements, sizeof(DWORD), 1, fp);

	CLZObject zObj;

	printf("sizeof(TMobTable) %d\n", sizeof(TMobTable));

	if (!CLZO::instance().GetWorkMemory())
	{
		LogProtoError("SaveMobProto: LZO work memory not initialized");
		fclose(fp);
		return;
	}
	if (!CLZO::instance().CompressEncryptedMemory(zObj, m_pMobTable, sizeof(TMobTable) * m_iMobTableSize, g_adwMobProtoKey))
	{
		LogProtoError("SaveMobProto: compress failed");
		fclose(fp);
		return;
	}

	const CLZObject::THeader & r = zObj.GetHeader();

	printf("MobProto count %u\n%u --Compress--> %u --Encrypt--> %u, GetSize %u\n",
			m_iMobTableSize, r.dwRealSize, r.dwCompressedSize, r.dwEncryptSize, zObj.GetSize());

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

static bool EnsureItemCols(cCsvTable& csvTable, const char* vnumStr, const char* lang, int rowIndex)
{
	// Required columns up to: bGainSocketPct + AddonType (skipped)
	// vnum(0) + 32 fields (1..32) => ColCount must be >= 33.
	const size_t required = 33;
	const size_t have = csvTable.ColCount();
	if (have < required)
	{
		LogProtoError("[ITEM][%s] row=%d vnum=%s colcount=%u required=%u (row too short)",
			lang ? lang : "?", rowIndex, vnumStr ? vnumStr : "?", (unsigned)have, (unsigned)required);
		return false;
	}
	return true;
}

bool Set_Proto_Item_Table(TClientItemTable *itemTable, cCsvTable &csvTable, std::map<int,const char*> &nameMap, const char* cLang, int rowIndex)
{
	try
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

	if (!EnsureItemCols(csvTable, csvTable.AsStringByIndex(0), cLang, rowIndex))
		return false;

	strncpy(itemTable->szName, csvTable.AsStringByIndex(1), ITEM_NAME_MAX_LEN);
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

	if (csvTable.ColCount() >= 42)
	{
		// Official GF (42 cols)
		int col = 2;
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
		col++; // 13: 67AttrMaterial
		itemTable->bAlterToMagicItemPct = atoi(csvTable.AsStringByIndex(col++)); // 14

		for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
		{
			itemTable->aLimits[i].bType = get_Item_LimitType_Value(csvTable.AsStringByIndex(col++));
			itemTable->aLimits[i].lValue = atoi(csvTable.AsStringByIndex(col++));
		}

		for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
		{
			itemTable->aApplies[i].bType = get_Item_ApplyType_Value(csvTable.AsStringByIndex(col++));
			itemTable->aApplies[i].lValue = atoi(csvTable.AsStringByIndex(col++));
		}

		col += 2; // 25..26: ApplyType3, ApplyValue3 (skip GF 4th apply)

		for (int i = 0; i < ITEM_VALUES_MAX_NUM; ++i)
			itemTable->alValues[i] = atoi(csvTable.AsStringByIndex(col++));

		col += 6; // 33..38: Value6..Value11 (skip GF extra values)

		itemTable->bSpecular = atoi(csvTable.AsStringByIndex(col++)); // 39
		itemTable->bGainSocketPct = atoi(csvTable.AsStringByIndex(col++)); // 40
		col++; // 41: AddonType

		itemTable->bWeight = 0;
	}
	else
	{
		// Legacy (33 cols)
		int col = 2;
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
		col++; // AddonType

		itemTable->bWeight = 0;
	}

	return true;
	}
	catch (...)
	{
		const char* vnumStr = "?";
		try { vnumStr = csvTable.AsStringByIndex(0); } catch (...) {}
		LogProtoError("[ITEM][%s] row=%d vnum=%s (exception during parse)", cLang ? cLang : "?", rowIndex, vnumStr);
		return false;
	}
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
			if (nameData.ColCount() >= 2)
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
		delete[] m_pItemTable;
		m_pItemTable = NULL;
	}

	int addNumber = 0;
	int validItemCount = 0;
	while(data.Next())
	{
		if (data.ColCount() < 33)
			continue;
		validItemCount++;
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

	m_iItemTableSize = validItemCount + addNumber;
	m_pItemTable = new TClientItemTable[m_iItemTableSize];
	memset(m_pItemTable, 0, sizeof(TClientItemTable) * m_iItemTableSize);

	TClientItemTable * item_table = m_pItemTable;

	int rowIndex = 0;
	while (data.Next())
	{
		++rowIndex;
		int col = 0;

		std::map<DWORD, TClientItemTable *>::iterator it_map_itemTable;
		it_map_itemTable = map_itemTableByVnum.find(atoi(data.AsStringByIndex(col)));

		if(it_map_itemTable == map_itemTableByVnum.end())
		{
			if (!Set_Proto_Item_Table(item_table, data, localMap, cLang, rowIndex))
			{
				const char* vnumStr = "?";
				if (data.ColCount() > 0)
					vnumStr = data.AsStringByIndex(0);
				LogProtoError("[ITEM][%s] row=%d vnum=%s skipped (Failed to set item proto table)", cLang ? cLang : "?", rowIndex, vnumStr);
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
	SetCtx("SaveItemProto", cLang, 0, 0);

	if (!m_pItemTable || m_iItemTableSize == 0)
	{
		LogProtoError("SaveItemProto skip: m_pItemTable=%p m_iItemTableSize=%d", (void*)m_pItemTable, (int)m_iItemTableSize);
		return;
	}

	EnsureBuildLangDir(cLang);
	char fileName[256];
	snprintf(fileName, sizeof(fileName), "build/%s/item_proto", cLang);

	FILE * fp;
	fp = fopen(fileName, "wb");

	if (!fp)
	{
		LogProtoError("cannot open %s for writing", fileName);
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
	sort(&m_pItemTable[0], &m_pItemTable[0] + m_iItemTableSize);

	if (!CLZO::instance().GetWorkMemory())
	{
		LogProtoError("SaveItemProto: LZO work memory not initialized");
		fclose(fp);
		return;
	}
	if (!CLZO::instance().CompressEncryptedMemory(zObj, m_pItemTable, sizeof(TClientItemTable) * m_iItemTableSize, g_adwItemProtoKey))
	{
		LogProtoError("SaveItemProto: compress failed");
		fclose(fp);
		return;
	}

	const CLZObject::THeader & r = zObj.GetHeader();

	printf("Elements %d\n%u --Compress--> %u --Encrypt--> %u, GetSize %u\n",
			m_iItemTableSize,
			r.dwRealSize,
			r.dwCompressedSize,
			r.dwEncryptSize,
			zObj.GetSize());

	DWORD dwDataSize = zObj.GetSize();
	fwrite(&dwDataSize, sizeof(DWORD), 1, fp);
	fwrite(zObj.GetBuffer(), dwDataSize, 1, fp);

	fclose(fp);

	// Optional sanity-check: re-open the file we just wrote.
	fp = fopen(fileName, "rb");
	if (fp)
	{
		DWORD readFourcc = 0;
		DWORD readVersion = 0;
		DWORD readStride = 0;
		DWORD readElements = 0;

		fread(&readFourcc, sizeof(DWORD), 1, fp);
		fread(&readVersion, sizeof(DWORD), 1, fp);
		fread(&readStride, sizeof(DWORD), 1, fp);
		fread(&readElements, sizeof(DWORD), 1, fp);

		printf("Elements Check %u fourcc match %d\n", readElements, readFourcc == MAKEFOURCC('M', 'I', 'P', 'X'));
		fclose(fp);
	}
}

int main(int argc, char ** argv)
{
#if defined(_MSC_VER)
	OpenLogFile();
	SetUnhandledExceptionFilter(DumpProtoUnhandledExceptionFilter);
	atexit(CloseLogFile);
#endif

	bool bXPhase = false;

	if (bXPhase)
		LoadMobProto();
	else
	{
		for (int i = 0; i < 11; i++)
		{
			const char* lang = MLanguage[i].c_str();
			SetCtx("MobPhase", lang, 0, 0);
#if defined(_MSC_VER)
			__try
			{
				if (BuildMobTable(lang))
					SaveMobProto(lang);
			}
			__except (DumpProtoUnhandledExceptionFilter(GetExceptionInformation()))
			{
				LogProtoError("[MOB][%s] FATAL: crash in mob phase (continuing)", lang ? lang : "?");
			}
#else
			if (BuildMobTable(lang))
				SaveMobProto(lang);
#endif
		}
	}

	for (int i = 0; i < 11; i++)
	{
		const char* lang = MLanguage[i].c_str();
		SetCtx("ItemPhase", lang, 0, 0);
#if defined(_MSC_VER)
		__try
		{
			if (BuildItemTable(lang))
				SaveItemProto(lang);
		}
		__except (DumpProtoUnhandledExceptionFilter(GetExceptionInformation()))
		{
			LogProtoError("[ITEM][%s] FATAL: crash in item phase (continuing)", lang ? lang : "?");
		}
#else
		if (BuildItemTable(lang))
			SaveItemProto(lang);
#endif
	}

	return 0;
}
