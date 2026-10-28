#ifndef __INC_METIN_II_GAME_CONSTANTS_H__
#define __INC_METIN_II_GAME_CONSTANTS_H__

#include "../../common/service.h"
#include "../../common/tables.h"

#ifdef ENABLE_GROWTH_PET_SYSTEM
	#include "growth_pet.h"
#endif


enum EMonsterChatState
{
	MONSTER_CHAT_WAIT,
	MONSTER_CHAT_ATTACK,
	MONSTER_CHAT_CHASE,
	MONSTER_CHAT_ATTACKED,
};

typedef struct SMobRankStat
{
	int iGoldPercent;
} TMobRankStat;

typedef struct SMobStat
{
	BYTE byLevel;
	WORD HP;
	DWORD dwExp;
	WORD wDefGrade;
} TMobStat;

typedef struct SBattleTypeStat
{
	int AttGradeBias;
	int DefGradeBias;
	int MagicAttGradeBias;
	int MagicDefGradeBias;
} TBattleTypeStat;

typedef struct SJobInitialPoints
{
	int		st, ht, dx, iq;
	int		max_hp, max_sp;
	int		hp_per_ht, sp_per_iq;
	int		hp_per_lv_begin, hp_per_lv_end;
	int		sp_per_lv_begin, sp_per_lv_end;
	int		max_stamina;
	int		stamina_per_con;
	int		stamina_per_lv_begin, stamina_per_lv_end;
} TJobInitialPoints;

typedef struct __coord
{
	int		x, y;
} Coord;

typedef struct SApplyInfo
{
	BYTE	bPointType;                          // APPLY -> POINT
} TApplyInfo;

enum {
	FORTUNE_BIG_LUCK,
	FORTUNE_LUCK,
	FORTUNE_SMALL_LUCK,
	FORTUNE_NORMAL,
	FORTUNE_SMALL_BAD_LUCK,
	FORTUNE_BAD_LUCK,
	FORTUNE_BIG_BAD_LUCK,
	FORTUNE_MAX_NUM,
};

const int STONE_INFO_MAX_NUM = 10;
const int STONE_LEVEL_MAX_NUM = 4;

struct SStoneDropInfo
{
	DWORD dwMobVnum;
	int iDropPct;
	int iLevelPct[STONE_LEVEL_MAX_NUM+1];
};

inline bool operator < (const SStoneDropInfo& l, DWORD r)
{
	return l.dwMobVnum < r;
}

inline bool operator < (DWORD l, const SStoneDropInfo& r)
{
	return l < r.dwMobVnum;
}

inline bool operator < (const SStoneDropInfo& l, const SStoneDropInfo& r)
{
	return l.dwMobVnum < r.dwMobVnum;
}

extern const TApplyInfo aApplyInfo[MAX_APPLY_NUM];
extern const TMobRankStat MobRankStats[MOB_RANK_MAX_NUM];

extern TBattleTypeStat BattleTypeStats[BATTLE_TYPE_MAX_NUM];

extern const DWORD party_exp_distribute_table[PLAYER_MAX_LEVEL_CONST + 1];
#ifdef ENABLE_CONQUEROR_LEVEL
extern const DWORD		exp_coqueror_table[PLAYER_MAX_CONQUEROR_LEVEL_CONST + 1];
#endif

extern const DWORD exp_table_common[PLAYER_EXP_TABLE_MAX + 1];

extern const DWORD *exp_table;

extern const DWORD guild_exp_table[GUILD_MAX_LEVEL + 1];

#ifdef ENABLE_AURA_COSTUME_SYSTEM
static int s_aiAuraRefineTable[AURA_GRADE_MAX_NUM][AURA_REFINE_INFO_MAX];
int *GetAuraRefineInfo(BYTE bLevel);
int GetAuraRefineInfo(BYTE bGrade, BYTE bInfo);
#endif

#define MAX_EXP_DELTA_OF_LEV 31
#define PERCENT_LVDELTA(me, victim) aiPercentByDeltaLev[MINMAX(0, (victim + 15) - me, MAX_EXP_DELTA_OF_LEV - 1)]
#define PERCENT_LVDELTA_BOSS(me, victim) aiPercentByDeltaLevForBoss[MINMAX(0, (victim + 15) - me, MAX_EXP_DELTA_OF_LEV - 1)]
#define CALCULATE_VALUE_LVDELTA(me, victim, val) ((val * PERCENT_LVDELTA(me, victim)) / 100)
extern const int aiPercentByDeltaLev_euckr[MAX_EXP_DELTA_OF_LEV];
extern const int aiPercentByDeltaLevForBoss_euckr[MAX_EXP_DELTA_OF_LEV];
extern const int* aiPercentByDeltaLev;
extern const int* aiPercentByDeltaLevForBoss;

#define ARROUND_COORD_MAX_NUM 161
extern Coord aArroundCoords[ARROUND_COORD_MAX_NUM];
extern TJobInitialPoints JobInitialPoints[JOB_MAX_NUM];

extern const int aiMobEnchantApplyIdx[MOB_ENCHANTS_MAX_NUM];
extern const int aiMobResistsApplyIdx[MOB_RESISTS_MAX_NUM];

extern const int aSkillAttackAffectProbByRank[MOB_RANK_MAX_NUM];

extern const int aiItemMagicAttributePercentHigh[ITEM_ATTRIBUTE_MAX_LEVEL];
extern const int aiItemMagicAttributePercentLow[ITEM_ATTRIBUTE_MAX_LEVEL];

extern const int aiItemAttributeAddPercent[ITEM_ATTRIBUTE_MAX_NUM];

extern const int aiWeaponSocketQty[WEAPON_NUM_TYPES];
extern const int aiArmorSocketQty[ARMOR_NUM_TYPES];
extern const int aiSocketPercentByQty[5][4];

extern const int aiExpLossPercents[PLAYER_EXP_TABLE_MAX + 1];

extern const int* aiSkillPowerByLevel;
extern const int aiSkillPowerByLevel_euckr[SKILL_MAX_LEVEL + 1];

extern const int aiPolymorphPowerByLevel[SKILL_MAX_LEVEL + 1];

extern const int aiSkillBookCountForLevelUp[10];
extern const int aiGrandMasterSkillBookCountForLevelUp[10];
extern const int aiGrandMasterSkillBookMinCount[10];
extern const int aiGrandMasterSkillBookMaxCount[10];
extern const int CHN_aiPartyBonusExpPercentByMemberCount[9];

typedef std::map<DWORD, TItemAttrTable> TItemAttrMap;
extern TItemAttrMap g_map_itemAttr;
extern TItemAttrMap g_map_itemRare;

extern const int *aiChainLightningCountBySkillLevel;
extern const int aiChainLightningCountBySkillLevel_euckr[SKILL_MAX_LEVEL + 1];

extern const char *c_apszEmpireNames[EMPIRE_MAX_NUM];
extern const char *c_apszPrivNames[MAX_PRIV_NUM];
extern const SStoneDropInfo aStoneDrop[STONE_INFO_MAX_NUM];

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
extern std::string m_closeText[LOCALE_MAX_NUM];
extern std::string m_horseText[LOCALE_MAX_NUM];
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
typedef struct SPetEvolution
{
	DWORD dwLevel;
	char szEvolutionName[CHARACTER_NAME_MAX_LEN + 1];
	std::pair<DWORD, WORD> dwItems[PET_EVOL_MAX_ITEM_COUNT];
} TPetEvolution;

typedef struct SPetHatch
{
	DWORD dwVnum;
	BYTE bMaxSKillCount;
	BYTE bMinDuration;
	BYTE bMaxDuration;
} TPetHatch;

typedef struct SPetBonus
{
	BYTE bMinValue;
	BYTE bMaxValue;
} TPetBonus;

extern const TPetEvolution arPetEvolutionTable[3];
extern const TPetHatch arPetHatchTable[PET_MAX_NUM];
extern const TPetBonus arPetHPBonusTable[PET_MAX_BONUS_NUM];
extern const TPetBonus arPetSPBonusTable[PET_MAX_BONUS_NUM];
extern const TPetBonus arPetDefBonusTable[PET_MAX_BONUS_NUM];
#endif

#ifdef ENABLE_BIOLOGIST_SYSTEM
extern const DWORD		BiyologSistemi[11][14];
#endif

#ifdef ENABLE_HUNTING_SYSTEM
extern const DWORD THuntingMissions[HUNTING_MISSION_COUNT+1][2][2];
extern const DWORD THuntingRewardItem[HUNTING_MISSION_COUNT+1][2][4][2];

struct SHuntingRewardMoney
{
	DWORD dLevelFrom;
	DWORD dLevelTo;
	DWORD dRandMoney[5];
};

struct SHuntingRewardEXP
{
	DWORD dLevelFrom;
	DWORD dLevelTo;
	DWORD dPerc[2];
};

extern const SHuntingRewardMoney THuntingRewardMoney[HUNTING_MONEY_TABLE_SIZE];
extern const SHuntingRewardEXP THuntingRewardEXP[HUNTING_EXP_TABLE_SIZE];

extern const DWORD THuntingRandomItem_01_20[6][2];
extern const DWORD THuntingRandomItem_21_40[13][2];
extern const DWORD THuntingRandomItem_41_60[13][2];
extern const DWORD THuntingRandomItem_61_80[13][2];
extern const DWORD THuntingRandomItem_81_90[13][2];
#endif

typedef struct
{
	long lMapIndex;
	int iWarPrice;
	int iWinnerPotionRewardPctToWinner;
	int iLoserPotionRewardPctToWinner;
	int iInitialScore;
	int iEndScore;
} TGuildWarInfo;

extern TGuildWarInfo KOR_aGuildWarInfo[GUILD_WAR_TYPE_MAX_NUM];

// ACCESSORY_REFINE
enum
{
	ITEM_ACCESSORY_SOCKET_MAX_NUM = 3
};

extern const int aiAccessorySocketAddPct[ITEM_ACCESSORY_SOCKET_MAX_NUM];
extern const int aiAccessorySocketEffectivePct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
extern const int aiAccessorySocketDegradeTime[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
extern const int aiAccessorySocketPutPct[ITEM_ACCESSORY_SOCKET_MAX_NUM + 1];
long FN_get_apply_type(const char *apply_type_string);
#ifdef ENABLE_RESP_SYSTEM
extern std::set<uint32_t> g_setRespAllowedMap;
#endif

// END_OF_ACCESSORY_REFINE

#ifdef __DUNGEON_INFO__
extern const std::map<DWORD, std::pair<std::pair<BYTE, BYTE>, std::string>> m_mapDungeonList;
#endif
#endif
