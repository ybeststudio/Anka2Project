#ifndef __INC_METIN_II_CHAR_H__
#define __INC_METIN_II_CHAR_H__

#include <unordered_map>
#include "../../common/service.h"
#include "../../common/tables.h"
#include "../../common/stl.h"
#include "entity.h"
#include "FSM.h"
#include "horse_rider.h"
#include "vid.h"
#include "constants.h"
#include "affect.h"
#include "affect_flag.h"
#include "cube.h"
#include "mining.h"
#include "desc.h"

#ifdef ENABLE_RENEWAL_CUBE
	#include "cube_renewal.h"
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	#include <vector>
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	#include "growth_pet.h"
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	#include "changelook.h"
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	#include "soulroulette.h"
#endif

enum eMountType { MOUNT_TYPE_NONE = 0, MOUNT_TYPE_NORMAL = 1, MOUNT_TYPE_COMBAT = 2, MOUNT_TYPE_MILITARY = 3 };
eMountType GetMountLevelByVnum(DWORD dwMountVnum, bool IsNew);
const DWORD GetRandomSkillVnum(BYTE bJob = JOB_MAX_NUM);
#ifdef ENABLE_NINETH_SKILL
const uint32_t GetRandomForgetSkillVnum(uint8_t bJob = JOB_MAX_NUM);
#endif

class CBuffOnAttributes;
class CPetSystem;
#ifdef ENABLE_MOUNT_SYSTEM
class CMountSystem;
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
class COfflineShop;
typedef class COfflineShop* LPOFFLINESHOP;
#endif

#define INSTANT_FLAG_DEATH_PENALTY		(1 << 0)
#define INSTANT_FLAG_SHOP				(1 << 1)
#define INSTANT_FLAG_EXCHANGE			(1 << 2)
#define INSTANT_FLAG_STUN				(1 << 3)
#define INSTANT_FLAG_NO_REWARD			(1 << 4)

#define AI_FLAG_NPC				(1 << 0)
#define AI_FLAG_AGGRESSIVE			(1 << 1)
#define AI_FLAG_HELPER				(1 << 2)
#define AI_FLAG_STAYZONE			(1 << 3)

#ifdef ENABLE_MINIGAME_RUMI_EVENT
	#define MAX_CARDS_IN_HAND	5
	#define MAX_CARDS_IN_FIELD	3
#endif

extern int g_nPortalLimitTime;

enum
{
	MAIN_RACE_WARRIOR_M,
	MAIN_RACE_ASSASSIN_W,
	MAIN_RACE_SURA_M,
	MAIN_RACE_SHAMAN_W,
	MAIN_RACE_WARRIOR_W,
	MAIN_RACE_ASSASSIN_M,
	MAIN_RACE_SURA_W,
	MAIN_RACE_SHAMAN_M,

	MAIN_RACE_MAX_NUM,
};

enum
{
	POISON_LENGTH = 30,
	STAMINA_PER_STEP = 1,
	SAFEBOX_PAGE_SIZE = 9,
	AI_CHANGE_ATTACK_POISITION_TIME_NEAR = 10000,
	AI_CHANGE_ATTACK_POISITION_TIME_FAR = 1000,
	AI_CHANGE_ATTACK_POISITION_DISTANCE = 100,
	SUMMON_MONSTER_COUNT = 3,
};

enum
{
	FLY_NONE,
	FLY_EXP,
	FLY_HP_MEDIUM,
	FLY_HP_BIG,
	FLY_SP_SMALL,
	FLY_SP_MEDIUM,
	FLY_SP_BIG,
	FLY_FIREWORK1,
	FLY_FIREWORK2,
	FLY_FIREWORK3,
	FLY_FIREWORK4,
	FLY_FIREWORK5,
	FLY_FIREWORK6,
	FLY_FIREWORK_CHRISTMAS,
	FLY_CHAIN_LIGHTNING,
	FLY_HP_SMALL,
	FLY_SKILL_MUYEONG,
#ifdef ENABLE_CONQUEROR_LEVEL
	FLY_CONQUEROR_EXP,
#endif
};

enum EDamageType
{
	DAMAGE_TYPE_NONE,
	DAMAGE_TYPE_NORMAL,
	DAMAGE_TYPE_NORMAL_RANGE,
	DAMAGE_TYPE_MELEE,
	DAMAGE_TYPE_RANGE,
	DAMAGE_TYPE_FIRE,
	DAMAGE_TYPE_ICE,
	DAMAGE_TYPE_ELEC,
	DAMAGE_TYPE_MAGIC,
	DAMAGE_TYPE_POISON,
	DAMAGE_TYPE_SPECIAL,
};

enum DamageFlag
{
	DAMAGE_NORMAL	= (1 << 0),
	DAMAGE_POISON	= (1 << 1),
	DAMAGE_DODGE	= (1 << 2),
	DAMAGE_BLOCK	= (1 << 3),
	DAMAGE_PENETRATE= (1 << 4),
	DAMAGE_CRITICAL = (1 << 5),
};

enum EPointTypes
{
	POINT_NONE,                 	// 0
	POINT_LEVEL,                	// 1
	POINT_VOICE,                	// 2
	POINT_EXP,                  	// 3
	POINT_NEXT_EXP,             	// 4
	POINT_HP,                   	// 5
	POINT_MAX_HP,               	// 6
	POINT_SP,                   	// 7
	POINT_MAX_SP,               	// 8
	POINT_STAMINA,              	// 9
	POINT_MAX_STAMINA,          	// 10
	POINT_GOLD,                 	// 11
	POINT_ST,                   	// 12
	POINT_HT,                   	// 13
	POINT_DX,                   	// 14
	POINT_IQ,                   	// 15
	POINT_DEF_GRADE,				// 16
	POINT_ATT_SPEED,            	// 17
	POINT_ATT_GRADE,				// 18
	POINT_MOV_SPEED,            	// 19
	POINT_CLIENT_DEF_GRADE,			// 20
	POINT_CASTING_SPEED,        	// 21
	POINT_MAGIC_ATT_GRADE,      	// 22
	POINT_MAGIC_DEF_GRADE,      	// 23
	POINT_EMPIRE_POINT,         	// 24
	POINT_LEVEL_STEP,           	// 25
	POINT_STAT,                 	// 26
	POINT_SUB_SKILL,				// 27
	POINT_SKILL,					// 28
	POINT_WEAPON_MIN,				// 29
	POINT_WEAPON_MAX,				// 30
	POINT_PLAYTIME,             	// 31
	POINT_HP_REGEN,             	// 32
	POINT_SP_REGEN,             	// 33
	POINT_BOW_DISTANCE,         	// 34
	POINT_HP_RECOVERY,          	// 35
	POINT_SP_RECOVERY,          	// 36
	POINT_POISON_PCT,           	// 37
	POINT_STUN_PCT,             	// 38
	POINT_SLOW_PCT,             	// 39
	POINT_CRITICAL_PCT,         	// 40
	POINT_PENETRATE_PCT,        	// 41
	POINT_CURSE_PCT,            	// 42
	POINT_ATTBONUS_HUMAN,       	// 43
	POINT_ATTBONUS_ANIMAL,      	// 44
	POINT_ATTBONUS_ORC,         	// 45
	POINT_ATTBONUS_MILGYO,      	// 46
	POINT_ATTBONUS_UNDEAD,      	// 47
	POINT_ATTBONUS_DEVIL,       	// 48
	POINT_ATTBONUS_INSECT,      	// 49
	POINT_ATTBONUS_FIRE,        	// 50
	POINT_ATTBONUS_ICE,         	// 51
	POINT_ATTBONUS_DESERT,      	// 52
	POINT_ATTBONUS_MONSTER,     	// 53
	POINT_ATTBONUS_WARRIOR,     	// 54
	POINT_ATTBONUS_ASSASSIN,		// 55
	POINT_ATTBONUS_SURA,			// 56
	POINT_ATTBONUS_SHAMAN,			// 57
	POINT_ATTBONUS_TREE,     		// 58
	POINT_RESIST_WARRIOR,			// 59
	POINT_RESIST_ASSASSIN,			// 60
	POINT_RESIST_SURA,				// 61
	POINT_RESIST_SHAMAN,			// 62
	POINT_STEAL_HP,             	// 63
	POINT_STEAL_SP,             	// 64
	POINT_MANA_BURN_PCT,        	// 65
	POINT_DAMAGE_SP_RECOVER,    	// 66
	POINT_BLOCK,                	// 67
	POINT_DODGE,                	// 68
	POINT_RESIST_SWORD,         	// 69
	POINT_RESIST_TWOHAND,       	// 70
	POINT_RESIST_DAGGER,        	// 71
	POINT_RESIST_BELL,          	// 72
	POINT_RESIST_FAN,           	// 73
	POINT_RESIST_BOW,           	// 74
	POINT_RESIST_FIRE,          	// 75
	POINT_RESIST_ELEC,          	// 76
	POINT_RESIST_MAGIC,         	// 77
	POINT_RESIST_WIND,          	// 78
	POINT_REFLECT_MELEE,        	// 79
	POINT_REFLECT_CURSE,			// 80
	POINT_POISON_REDUCE,			// 81
	POINT_KILL_SP_RECOVER,			// 82
	POINT_EXP_DOUBLE_BONUS,			// 83
	POINT_GOLD_DOUBLE_BONUS,		// 84
	POINT_ITEM_DROP_BONUS,			// 85
	POINT_POTION_BONUS,				// 86
	POINT_KILL_HP_RECOVERY,			// 87
	POINT_IMMUNE_STUN,				// 88
	POINT_IMMUNE_SLOW,				// 89
	POINT_IMMUNE_FALL,				// 90
	POINT_PARTY_ATTACKER_BONUS,		// 91
	POINT_PARTY_TANKER_BONUS,		// 92
	POINT_ATT_BONUS,				// 93
	POINT_DEF_BONUS,				// 94
	POINT_ATT_GRADE_BONUS,			// 95
	POINT_DEF_GRADE_BONUS,			// 96
	POINT_MAGIC_ATT_GRADE_BONUS,	// 97
	POINT_MAGIC_DEF_GRADE_BONUS,	// 98
	POINT_RESIST_NORMAL_DAMAGE,		// 99
	POINT_HIT_HP_RECOVERY,			// 100
	POINT_HIT_SP_RECOVERY, 			// 101
	POINT_MANASHIELD,				// 102
	POINT_PARTY_BUFFER_BONUS,		// 103
	POINT_PARTY_SKILL_MASTER_BONUS,	// 104
	POINT_HP_RECOVER_CONTINUE,		// 105
	POINT_SP_RECOVER_CONTINUE,		// 106
	POINT_STEAL_GOLD,				// 107
	POINT_POLYMORPH,				// 108
	POINT_MOUNT,					// 109
	POINT_PARTY_HASTE_BONUS,		// 110
	POINT_PARTY_DEFENDER_BONUS,		// 111
	POINT_STAT_RESET_COUNT,			// 112
	POINT_HORSE_SKILL,				// 113
	POINT_MALL_ATTBONUS,			// 114
	POINT_MALL_DEFBONUS,			// 115
	POINT_MALL_EXPBONUS,			// 116
	POINT_MALL_ITEMBONUS,			// 117
	POINT_MALL_GOLDBONUS,			// 118
	POINT_MAX_HP_PCT,				// 119
	POINT_MAX_SP_PCT,				// 120
	POINT_SKILL_DAMAGE_BONUS,		// 121
	POINT_NORMAL_HIT_DAMAGE_BONUS,	// 122
	POINT_SKILL_DEFEND_BONUS,		// 123
	POINT_NORMAL_HIT_DEFEND_BONUS,	// 124
	POINT_PC_BANG_EXP_BONUS,		// 125
	POINT_PC_BANG_DROP_BONUS,		// 126
	POINT_RAMADAN_CANDY_BONUS_EXP,	// 127
	POINT_ENERGY 					= 128,
	POINT_ENERGY_END_TIME 			= 129,
	POINT_COSTUME_ATTR_BONUS 		= 130,
	POINT_MAGIC_ATT_BONUS_PER 		= 131,
	POINT_MELEE_MAGIC_ATT_BONUS_PER = 132,
	POINT_RESIST_ICE 				= 133,
	POINT_RESIST_EARTH 				= 134,
	POINT_RESIST_DARK 				= 135,
	POINT_RESIST_CRITICAL 			= 136,
	POINT_RESIST_PENETRATE 			= 137,

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	POINT_ACCEDRAIN_RATE 			= 138,
#endif

#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
	POINT_RESIST_MAGIC_REDUCTION 	= 139,
#endif

	POINT_RESIST_MOUNT_FALL 		= 140, //unk
	POINT_RESIST_HUMAN 				= 141,

	POINT_ENCHANT_ELECT 			= 142,
	POINT_ENCHANT_FIRE 				= 143,
	POINT_ENCHANT_ICE 				= 144,
	POINT_ENCHANT_WIND 				= 145,
	POINT_ENCHANT_EARTH 			= 146,
	POINT_ENCHANT_DARK 				= 147,

	POINT_ATTBONUS_CZ 				= 148,
	POINT_ATTBONUS_SWORD 			= 149,
	POINT_ATTBONUS_TWOHAND 			= 150,
	POINT_ATTBONUS_DAGGER 			= 151,
	POINT_ATTBONUS_BELL 			= 152,
	POINT_ATTBONUS_FAN 				= 153,
	POINT_ATTBONUS_BOW 				= 154,

#ifdef ENABLE_NEW_BONUS_SYSTEM
	POINT_ATTBONUS_STONE 			= 155,
	POINT_ATTBONUS_BOSS 			= 156,
	POINT_ATTBONUS_ELEMENTS			= 157,
	POINT_ENCHANT_ELEMENTS 			= 158,
	POINT_ATTBONUS_CHARACTERS 		= 159,
	POINT_ENCHANT_CHARACTERS 		= 160,
	POINT_RESIST_MONSTER 			= 161,
#endif

#ifdef ENABLE_AVG_PVM
	POINT_ATTBONUS_MEDI_PVM 		= 162,
#endif

#ifdef ENABLE_CONQUEROR_LEVEL
	POINT_CONQUEROR_LEVEL 			= 163,
	POINT_CONQUEROR_LEVEL_STEP 		= 164,
	POINT_SUNGMA_STR 				= 165,
	POINT_SUNGMA_HP					= 166,
	POINT_SUNGMA_MOVE 				= 167,
	POINT_SUNGMA_IMMUNE 			= 168,
	POINT_CONQUEROR_POINT 			= 169,
	POINT_CONQUEROR_EXP 			= 170,
	POINT_CONQUEROR_NEXT_EXP 		= 171,
#endif

	POINT_ATTBONUS_PVM_STR 			= 172,
	POINT_ATTBONUS_PVM_BERSERKER	= 173,

#ifdef ENABLE_COINS_INVENTORY
	POINT_COINS 					= 174,
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	POINT_SOUL 						= 175,
	POINT_SOUL_RE 					= 176,
#endif

#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
	POINT_HALOUNLV 					= 177,
	POINT_HALOUN 					= 178,
	POINT_RHALOUN 					= 179,
#endif

#ifdef ENABLE_GAYA_SYSTEM
	POINT_GEM 						= 180,
#endif

	POINT_RESIST_CLAW				= 181,
	POINT_BLEEDING_REDUCE			= 182,
	POINT_BLEEDING_PCT				= 183,
	POINT_ATTBONUS_WOLFMAN			= 184,
	POINT_RESIST_WOLFMAN			= 185,
	POINT_ATTBONUS_CLAW				= 186,

	POINT_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE = 187,
	POINT_SKILL_DEFEND_BONUS_BOSS_OR_MORE = 188,
	POINT_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE = 189,
	POINT_SKILL_DAMAGE_BONUS_BOSS_OR_MORE = 190,

	POINT_HIT_BUFF_ENCHANT_FIRE		= 191,
	POINT_HIT_BUFF_ENCHANT_ICE		= 192,
	POINT_HIT_BUFF_ENCHANT_ELEC		= 193,
	POINT_HIT_BUFF_ENCHANT_WIND		= 194,
	POINT_HIT_BUFF_ENCHANT_DARK		= 195,
	POINT_HIT_BUFF_ENCHANT_EARTH	= 196,
	POINT_HIT_BUFF_RESIST_FIRE		= 197,
	POINT_HIT_BUFF_RESIST_ICE		= 198,
	POINT_HIT_BUFF_RESIST_ELEC		= 199,

	POINT_MIN_WEP					= 200,
	POINT_MAX_WEP					= 201,
	POINT_MIN_MAGIC_WEP				= 202,
	POINT_MAX_MAGIC_WEP				= 203,
	POINT_HIT_RATE					= 204,
	POINT_EVADE_RATE				= 205,

	POINT_HIT_BUFF_RESIST_WIND		= 206,
	POINT_HIT_BUFF_RESIST_DARK		= 207,
	POINT_HIT_BUFF_RESIST_EARTH		= 208,

	POINT_ALIGNMENT_DAMAGE_BONUS	= 209,
	POINT_NORMAL_DAMAGE_GUARD		= 210,
	POINT_MORE_THEN_HP90_DAMAGE_REDUCE = 211,

	POINT_FIRST_ATTRIBUTE_BONUS		= 212,
	POINT_SECOND_ATTRIBUTE_BONUS	= 213,
	POINT_THIRD_ATTRIBUTE_BONUS		= 214,
	POINT_FOURTH_ATTRIBUTE_BONUS	= 215,
	POINT_FIFTH_ATTRIBUTE_BONUS		= 216,

	POINT_HIT_PCT					= 217,
	POINT_ATTBONUS_PER_HUMAN		= 218,
	POINT_ATTBONUS_PER_ANIMAL		= 219,
	POINT_ATTBONUS_PER_ORC			= 220,
	POINT_ATTBONUS_PER_MILGYO		= 221,
	POINT_ATTBONUS_PER_UNDEAD		= 222,
	POINT_ATTBONUS_PER_DEVIL		= 223,
	POINT_ENCHANT_PER_ELECT			= 224,
	POINT_ENCHANT_PER_FIRE			= 225,
	POINT_ENCHANT_PER_ICE			= 226,
	POINT_ENCHANT_PER_WIND			= 227,
	POINT_ENCHANT_PER_EARTH			= 228,
	POINT_ENCHANT_PER_DARK			= 229,
	POINT_ATTBONUS_PER_CZ			= 230,
	POINT_ATTBONUS_PER_INSECT		= 231,
	POINT_ATTBONUS_PER_DESERT		= 232,
	POINT_ATTBONUS_PER_STONE		= 233,
	POINT_ATTBONUS_PER_MONSTER		= 234,

	POINT_RESIST_PER_HUMAN			= 235,
	POINT_RESIST_PER_ICE			= 236,
	POINT_RESIST_PER_DARK			= 237,
	POINT_RESIST_PER_EARTH			= 238,
	POINT_RESIST_PER_FIRE			= 239,
	POINT_RESIST_PER_ELEC			= 240,
	POINT_RESIST_PER_MAGIC			= 241,
	POINT_RESIST_PER_WIND			= 242,

	POINT_HIT_BUFF_SUNGMA_STR		= 243,
	POINT_HIT_BUFF_SUNGMA_MOVE		= 244,
	POINT_HIT_BUFF_SUNGMA_HP		= 245,
	POINT_HIT_BUFF_SUNGMA_IMMUNE	= 246,

	POINT_MOUNT_MELEE_MAGIC_ATTBONUS_PER = 247,
	POINT_DISMOUNT_MOVE_SPEED_BONUS_PER  = 248,
	POINT_HIT_AUTO_HP_RECOVERY		= 249,
	POINT_HIT_AUTO_SP_RECOVERY		= 250,
	POINT_USE_SKILL_COOLTIME_DECREASE_ALL = 251,
	POINT_HIT_STONE_DEF_GRADE_BONUS	= 252,
	POINT_MONSTER_DEFEND_BONUS		= 253,
	POINT_ATTBONUS_DRAGON			= 254,
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM
};

enum EPositions
{
	POS_DEAD,
	POS_SLEEPING,
	POS_RESTING,
	POS_SITTING,
	POS_FISHING,
	POS_FIGHTING,
	POS_MOUNTING,
	POS_STANDING
};

enum EBlockAction
{
	BLOCK_EXCHANGE			= (1 << 0),
	BLOCK_PARTY_INVITE		= (1 << 1),
	BLOCK_GUILD_INVITE		= (1 << 2),
	BLOCK_WHISPER			= (1 << 3),
	BLOCK_MESSENGER_INVITE	= (1 << 4),
	BLOCK_PARTY_REQUEST		= (1 << 5),
#ifdef ENABLE_TELEPORT_TO_A_FRIEND
	BLOCK_WARP_REQUEST 		= (1 << 6),
#endif
};

// <Factor> Dynamically evaluated CHARACTER* equivalent.
// Referring to SCharDeadEventInfo.
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
enum EAutomaticPickUP
{
	AUTOMATIC_PICK_UP_ACTIVATE = (1 << 0),

	AUTOMATIC_PICK_UP_WEAPON = (1 << 1),
	AUTOMATIC_PICK_UP_ARMOR = (1 << 2),
	AUTOMATIC_PICK_UP_SHIELD = (1 << 3),
	AUTOMATIC_PICK_UP_HELMETS = (1 << 4),
	AUTOMATIC_PICK_UP_BRACELETS = (1 << 5),
	AUTOMATIC_PICK_UP_NECKLACE = (1 << 6),
	AUTOMATIC_PICK_UP_EARRINGS = (1 << 7),
	AUTOMATIC_PICK_UP_SHOES = (1 << 8),
	AUTOMATIC_PICK_UP_OTHERS = (1 << 9),
	AUTOMATIC_PICK_UP_YANG = (1 << 10),
	AUTOMATIC_PICK_UP_CHESTS = (1 << 11),
};
#endif

struct DynamicCharacterPtr
{
	DynamicCharacterPtr() : is_pc(false), id(0) {}
	DynamicCharacterPtr(const DynamicCharacterPtr& o) : is_pc(o.is_pc), id(o.id) {}
	// Returns the LPCHARACTER found in CHARACTER_MANAGER.
	LPCHARACTER Get() const;
	// Clears the current settings.
	void Reset()
	{
		is_pc = false;
		id = 0;
	}

	// Basic assignment operator.
	DynamicCharacterPtr& operator=(const DynamicCharacterPtr& rhs)
	{
		is_pc = rhs.is_pc;
		id = rhs.id;
		return *this;
	}
	// Supports assignment with LPCHARACTER type.
	DynamicCharacterPtr& operator=(LPCHARACTER character);
	// Supports type casting to LPCHARACTER.
	operator LPCHARACTER() const
	{
		return Get();
	}

	bool is_pc;
	uint32_t id;
};

#ifdef ENABLE_KILL_STATISTICS
typedef struct character_kill_statistics
{
	int		iJinnoKills;
	int		iShinsooKills;
	int		iChunjoKills;
	int		iTotalKills;
	int		iTotalDeaths;
	int		iDuelsWon;
	int		iDuelsLost;
	int		iBossesKills;
	int		iStonesKills;
	int		iMobsKills;
	int		top_damage;
} CHARACTER_KILL_STATISTICS;
#endif

typedef struct character_point
{
#ifdef ENABLE_GOLD_LIMIT
	long long points[POINT_MAX_NUM];
#else
	long points[POINT_MAX_NUM];
#endif

	BYTE job;
	BYTE voice;

#ifdef ENABLE_LEVEL_INT
	int level;
#else
	BYTE level;
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
	BYTE conquerorlevel;
	DWORD conqueror_exp;
#endif
	DWORD exp;
#ifdef ENABLE_GOLD_LIMIT
	long long gold;
#else
	long gold;
#endif
#ifdef ENABLE_GAYA_SYSTEM
	int gem;
#endif
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	int soul;
	int soulre;
#endif
#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
	int halounlv;
	long long phaloun;
	int rhaloun;
#endif

	int hp;
	int sp;

	int iRandomHP;
	int iRandomSP;

	int stamina;

	BYTE skill_group;
} CHARACTER_POINT;

typedef struct character_point_instant
{
#ifdef ENABLE_GOLD_LIMIT
	long long points[POINT_MAX_NUM];
#else
	long points[POINT_MAX_NUM];
#endif

	float fRot;

	int iMaxHP;
	int iMaxSP;

	long position;

	long instant_flag;
	DWORD dwAIFlag;
	DWORD dwImmuneFlag;
	DWORD dwLastShoutPulse;

	WORD parts[PART_MAX_NUM];

#ifdef ENABLE_RENEWAL_SWITCHBOT
	LPITEM pSwitchbotItems[SWITCHBOT_SLOT_COUNT];
#endif

	LPITEM pCubeItems[CUBE_MAX_NUM];
	LPCHARACTER pCubeNpc;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	TItemPosEx pAcceMaterials[ACCE_WINDOW_MAX_MATERIALS];
#endif

	LPCHARACTER battle_victim;

	BYTE gm_level;

	BYTE bBasePart;

	int iMaxStamina;

	BYTE bBlockMode;

	int iDragonSoulActiveDeck;
	LPENTITY m_pDragonSoulRefineWindowOpener;

#ifdef ENABLE_ANTI_EXP
	bool anti_exp;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	LPENTITY m_pAuraRefineWindowOpener;
#endif

#ifdef ENABLE_COMPUTE_POINT_FIX
	bool computed;
#endif

} CHARACTER_POINT_INSTANT;

// Most characters are mobs. Keep the large player inventory grids out of every
// CHARACTER allocation; they are created only for PCs or actual item owners.
struct TCharacterItemStorage
{
	LPITEM pItems[INVENTORY_AND_EQUIP_SLOT_MAX];
#ifdef ENABLE_SPECIAL_INVENTORY
	UINT bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];
#else
	BYTE bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];
#endif
	LPITEM pDSItems[DRAGON_SOUL_INVENTORY_MAX_NUM];
	WORD wDSItemGrid[DRAGON_SOUL_INVENTORY_MAX_NUM];
};

#define TRIGGERPARAM LPCHARACTER ch, LPCHARACTER causer

typedef struct trigger
{
	BYTE type;
	int (*func) (TRIGGERPARAM);
	long value;
} TRIGGER;

class CTrigger
{
	public:
		CTrigger() : bType(0), pFunc(NULL) {}

		BYTE bType;
		int (*pFunc) (TRIGGERPARAM);
};

EVENTINFO(char_event_info)
{
	DynamicCharacterPtr ch;
};

#ifdef ENABLE_NINETH_SKILL
EVENTINFO(cheonun_event_info)
{
	DynamicCharacterPtr ch;
	uint8_t bShieldChance;
	uint8_t bShieldDur;
};
#endif


typedef std::map<VID, size_t> target_map;
struct TSkillUseInfo
{
	int iHitCount;
	int iMaxHitCount;
	int iSplashCount;
	DWORD dwNextSkillUsableTime;
	int iRange;
	bool bUsed;
	DWORD dwVID;
	bool isGrandMaster;
#ifdef ENABLE_SKILL_COOLDOWN_CHECK
	bool cooldown{ false };
	int	 skillCount{ 0 };
#endif

	target_map TargetVIDMap;

	TSkillUseInfo() : iHitCount(0), iMaxHitCount(0), iSplashCount(0), dwNextSkillUsableTime(0), iRange(0), bUsed(false), dwVID(0), isGrandMaster(false) {}

	bool HitOnce(DWORD dwVnum = 0);

	bool UseSkill(bool isGrandMaster, DWORD vid, DWORD dwCooltime, int splashcount = 1, int hitcount = -1, int range = -1);
	DWORD GetMainTargetVID() const { return dwVID; }
	void SetMainTargetVID(DWORD vid) { dwVID=vid; }
	void ResetHitCount() { if (iSplashCount) { iHitCount = iMaxHitCount; iSplashCount--; } }
#ifdef ENABLE_SKILL_COOLDOWN_CHECK
	bool IsOnCooldown(int vnum);
#endif
};

typedef struct packet_party_update TPacketGCPartyUpdate;
class CExchange;
class CSkillProto;
class CParty;
class CDungeon;
class CWarMap;
class CAffect;
class CGuild;
class CSafebox;
class CArena;
class CShop;
typedef class CShop* LPSHOP;

class CMob;
class CMobInstance;
typedef struct SMobSkillInfo TMobSkillInfo;

//SKILL_POWER_BY_LEVEL
extern int GetSkillPowerByLevelFromType(int job, int skillgroup, int skilllevel);
//END_SKILL_POWER_BY_LEVEL

namespace marriage
{
	class WeddingMap;
}

class CHARACTER : public CEntity, public CFSM, public CHorseRider
{
	protected:
		virtual void EncodeInsertPacket(LPENTITY entity);
		virtual void EncodeRemovePacket(LPENTITY entity);

	public:
		LPCHARACTER FindCharacterInView(const char * name, bool bFindPCOnly);
		bool IsVictimInView(LPCHARACTER victim) const; // Fix
		void UpdatePacket();

#ifdef ENABLE_AUTO_SELL_SYSTEM
	private:
		bool m_isAutoSellItemsLoaded;
		std::vector<DWORD> m_vecAutoSellItems;
		bool m_bAutoSellStatus;
	
	public:
		void LoadAutoSellItemsFromJson();
		void SaveAutoSellItemsToJson();
		void AddAutoSellItem(DWORD dwVnum);
		void RemoveAutoSellItem(DWORD dwVnum);
		void RemoveAllAutoSellItem();
		bool IsAutoSellItem(DWORD dwVnum) const;
		void SetAutoSellStatus(bool bStatus);
		bool GetAutoSellStatus() const { return m_bAutoSellStatus; }
		void SendItemProcessInfo();
#endif

	protected:
		CStateTemplate<CHARACTER> m_stateMove;
		CStateTemplate<CHARACTER> m_stateBattle;
		CStateTemplate<CHARACTER> m_stateIdle;

	public:
		virtual void StateMove();
		virtual void StateBattle();
		virtual void StateIdle();
		virtual void StateFlag();
		virtual void StateFlagBase();
		void StateHorse();

	protected:
		void __StateIdle_Monster();
		void __StateIdle_Stone();
		void __StateIdle_NPC();

	public:
		DWORD GetAIFlag() const	{ return m_pointsInstant.dwAIFlag; }

		void SetAggressive();
		bool IsAggressive() const;

		void SetCoward();
		bool IsCoward() const;
		void CowardEscape();

		void SetNoAttackShinsu();
		bool IsNoAttackShinsu() const;

		void SetNoAttackChunjo();
		bool IsNoAttackChunjo() const;

		void SetNoAttackJinno();
		bool IsNoAttackJinno() const;

		void SetAttackMob();
		bool IsAttackMob() const;

		virtual void BeginStateEmpty();
		virtual void EndStateEmpty() {}

		void RestartAtSamePos();

#ifdef ENABLE_KILL_STATISTICS
		void			SendKillStatisticsPacket();

		void			AddToJinnoKills() { m_killstatistics.iJinnoKills += 1; }
		int				GetJinnoKills() const { return m_killstatistics.iJinnoKills; }

		void			AddToShinsooKills() { m_killstatistics.iShinsooKills += 1; }
		int				GetShinsooKills() const { return m_killstatistics.iShinsooKills; }

		void			AddToChunjoKills() { m_killstatistics.iChunjoKills += 1; }
		int				GetChunjoKills() const { return m_killstatistics.iChunjoKills; }

		void			AddToTotalKills() { m_killstatistics.iTotalKills += 1; }
		int				GetTotalKills() const { return m_killstatistics.iTotalKills; }

		void			AddToTotalDeaths() { m_killstatistics.iTotalDeaths += 1; }
		int				GetTotalDeaths() const { return m_killstatistics.iTotalDeaths; }

		void			AddToDuelsWon() { m_killstatistics.iDuelsWon += 1; }
		int				GetDuelsWon() const { return m_killstatistics.iDuelsWon; }

		void			AddToDuelsLost() { m_killstatistics.iDuelsLost += 1; }
		int				GetDuelsLost() const { return m_killstatistics.iDuelsLost; }

		void			AddToBossesKills() { m_killstatistics.iBossesKills += 1; }
		int				GetBossesKills() const { return m_killstatistics.iBossesKills; }

		void			AddToStonesKills() { m_killstatistics.iStonesKills += 1; }
		int				GetStonesKills() const { return m_killstatistics.iStonesKills; }

		void			AddToMobsKills() { m_killstatistics.iMobsKills += 1; }
		int				GetMobsKills() const { return m_killstatistics.iMobsKills; }

		void			SetTopDamage(int val) { m_killstatistics.top_damage = val; }
		int				GetTopDamage() const { return m_killstatistics.top_damage; }
#endif

#ifdef ENABLE_SUNG_MAHI_TOWER
		void SetNomove();
		void RemoveNomove();
		bool IsNomove() const;
		
		void SetNoattack();
		void RemoveNoattack();
		bool IsNoattack() const;
#endif
		bool IsNearWater() const;

	protected:
		DWORD m_dwStateDuration;

	public:
		CHARACTER();
		virtual ~CHARACTER();

		void Create(const char * c_pszName, DWORD vid, bool isPC);
		void Destroy();

		void Disconnect(const char * c_pszReason);

	protected:
		void Initialize();

#ifdef ENABLE_MOB_DROP_INFO
	private:
		DWORD dwLastTargetInfoPulse;

	public:
		DWORD GetLastTargetInfoPulse() const { return dwLastTargetInfoPulse; }
		void SetLastTargetInfoPulse(DWORD pulse) { dwLastTargetInfoPulse = pulse; }
#endif

	public:
		DWORD GetPlayerID() const { return m_dwPlayerID; }

		void SetPlayerProto(const TPlayerTable * table);
		void CreatePlayerProto(TPlayerTable & tab);

		void SetProto(const CMob * c_pkMob);
		WORD GetRaceNum() const;
		WORD GetPlayerRace() const;

		void Save();
		void SaveReal();
		void FlushDelayedSaveItem();

		const char * GetName() const;
		const VID & GetVID() const { return m_vid; }

		void SetName(const std::string& name) { m_stName = name; }

		void SetRace(BYTE race);
		bool ChangeSex();

		bool CheckTimeUsed(LPITEM item); // Fix

#ifdef ENABLE_RENEWAL_DEAD_PACKET
		DWORD CalculateDeadTime(BYTE type);
#endif

		DWORD GetAID() const;
		int GetChangeEmpireCount() const;
		void SetChangeEmpireCount();
		int ChangeEmpire(BYTE empire);

		BYTE GetJob() const;
		BYTE GetCharType() const;
#ifdef ENABLE_BOT_PLAYER
		void SetCharType(BYTE bCharType) { m_bCharType = bCharType; }
#endif

		bool IsPC() const { return GetDesc() ? true : false; }
		bool IsNPC() const { return m_bCharType != CHAR_TYPE_PC; }
		bool IsMonster() const { return m_bCharType == CHAR_TYPE_MONSTER; }
		bool IsStone() const { return m_bCharType == CHAR_TYPE_STONE; }
		bool IsDoor() const { return m_bCharType == CHAR_TYPE_DOOR; } 
		bool IsBuilding() const { return m_bCharType == CHAR_TYPE_BUILDING;  }
		bool IsWarp() const { return m_bCharType == CHAR_TYPE_WARP; }
		bool IsGoto() const { return m_bCharType == CHAR_TYPE_GOTO; }
		bool IsPvM() const { return (m_bCharType == CHAR_TYPE_MONSTER) || (m_bCharType == CHAR_TYPE_STONE) || GetMobRank() >= MOB_RANK_BOSS; }
		bool IsBoss() const { return m_bCharType == CHAR_TYPE_MONSTER && GetMobRank() == MOB_RANK_BOSS; }

		DWORD GetLastShoutPulse() const { return m_pointsInstant.dwLastShoutPulse; }
		void SetLastShoutPulse(DWORD pulse) { m_pointsInstant.dwLastShoutPulse = pulse; }
		int GetLevel() const { return m_points.level; }

#ifdef ENABLE_LEVEL_INT
		void SetLevel(int level);
#else
		void SetLevel(BYTE level);
#endif

#ifdef ENABLE_CONQUEROR_LEVEL
		int GetConquerorLevel() const { return m_points.conquerorlevel; }
		void SetConquerorLevel(BYTE level);

		DWORD GetConquerorExp() const { return m_points.conqueror_exp; }
		void SetConquerorExp(DWORD exp) { m_points.conqueror_exp = exp; }
		DWORD			GetConquerorNextExp() const;
		void ResetConquerorPoint(int iLv);
#endif

		BYTE GetGMLevel() const;
		BOOL IsGM() const;
		void SetGMLevel();

		DWORD GetExp() const { return m_points.exp; }
		void SetExp(DWORD exp) { m_points.exp = exp; }
		DWORD GetNextExp() const;
		bool block_exp;
		LPCHARACTER DistributeExp();
		void DistributeHP(LPCHARACTER pkKiller);
		void DistributeSP(LPCHARACTER pkKiller, int iMethod=0);

#ifdef ENABLE_KILL_EVENT_FIX
		LPCHARACTER		GetMostAttacked();
#endif

		void SetPosition(int pos);
		bool IsPosition(int pos) const { return m_pointsInstant.position == pos ? true : false; }
		int GetPosition() const { return m_pointsInstant.position; }

		void SetPart(BYTE bPartPos, WORD wVal);
		WORD GetPart(BYTE bPartPos) const;
		WORD GetOriginalPart(BYTE bPartPos) const;

		void SetHP(int hp) { m_points.hp = hp; }
		int GetHP() const { return m_points.hp; }

		void SetSP(int sp) { m_points.sp = sp; }
		int GetSP() const { return m_points.sp; }

		void SetStamina(int stamina) { m_points.stamina = stamina; }
		int GetStamina() const { return m_points.stamina; }

		void SetMaxHP(int iVal) { m_pointsInstant.iMaxHP = iVal; }
		int GetMaxHP() const { return m_pointsInstant.iMaxHP; }

		void SetMaxSP(int iVal) { m_pointsInstant.iMaxSP = iVal; }
		int GetMaxSP() const { return m_pointsInstant.iMaxSP; }

		void SetMaxStamina(int iVal) { m_pointsInstant.iMaxStamina = iVal; }
		int GetMaxStamina() const { return m_pointsInstant.iMaxStamina; }

		void SetRandomHP(int v) { m_points.iRandomHP = v; }
		void SetRandomSP(int v) { m_points.iRandomSP = v; }

		int GetRandomHP() const { return m_points.iRandomHP; }
		int GetRandomSP() const { return m_points.iRandomSP; }

		int GetHPPct() const;

		void SetRealPoint(BYTE idx, int val);
		int GetRealPoint(BYTE idx) const;

#ifdef ENABLE_GOLD_LIMIT
		void SetPoint(BYTE idx, long long val);
		long long GetPoint(BYTE idx) const;
#else
		void SetPoint(BYTE idx, int val);
		int GetPoint(BYTE idx) const;
#endif

		int GetLimitPoint(BYTE idx) const;
		int GetPolymorphPoint(BYTE idx) const;

		const TMobTable & GetMobTable() const;
		BYTE GetMobRank() const;
		BYTE GetMobBattleType() const;
		BYTE GetMobSize() const;
		DWORD GetMobDamageMin() const;
		DWORD GetMobDamageMax() const;
		WORD GetMobAttackRange() const;
		DWORD GetMobDropItemVnum() const;
		float GetMobDamageMultiply() const;
		float GetMonsterHitRange() const;

		bool IsBerserker() const;
		bool IsBerserk() const;
		void SetBerserk(bool mode);

		bool IsStoneSkinner() const;

		bool IsGodSpeeder() const;
		bool IsGodSpeed() const;
		void SetGodSpeed(bool mode);

		bool IsDeathBlower() const;
		bool IsDeathBlow() const;

		bool IsReviver() const;
		bool HasReviverInParty() const;
		bool IsRevive() const;
		void SetRevive(bool mode);

		bool IsRaceFlag(DWORD dwBit) const;
		bool IsSummonMonster() const;
		DWORD GetSummonVnum() const;

		DWORD GetPolymorphItemVnum() const;
		DWORD GetMonsterDrainSPPoint() const;

		void MainCharacterPacket();

		void ComputePoints();
		void ComputeBattlePoints();
#ifdef ENABLE_GOLD_LIMIT
		void PointChange(BYTE type, long long amount, bool bAmount = false, bool bBroadcast = false);
#else
		void PointChange(BYTE type, int amount, bool bAmount = false, bool bBroadcast = false);
#endif
		void PointsPacket();
		void UpdatePointsPacket(BYTE type, long long val, long long amount = 0, bool bAmount = false, bool bBroadcast = false);
		void ApplyPoint(WORD bApplyType, int iVal);
		void CheckMaximumPoints();

#ifdef ENABLE_SHOW_MOB_INFO
		bool Show(long lMapIndex, long x, long y, long z = LONG_MAX, bool bShowSpawnMotion = false, bool bAggressive = false);
#else
		bool Show(long lMapIndex, long x, long y, long z = LONG_MAX, bool bShowSpawnMotion = false);
#endif

		void Sitdown(int is_ground);
		void Standup();

		void SetRotation(float fRot);
		void SetRotationToXY(long x, long y);
		float GetRotation() const { return m_pointsInstant.fRot; }

		void MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet);
		void Motion(BYTE motion, LPCHARACTER victim = NULL);

		void ChatPacket(BYTE type, const char *format, ...) __attribute__((format(printf, 3, 4)));
#ifdef ENABLE_CLIENT_LOCALE_STRING
		void LocaleChatPacket(BYTE type, DWORD index, const char* format, ...) __attribute__((format(printf, 4, 5)));
		void LocaleWhisperPacket(BYTE type, DWORD index, const char* namefrom, const char* format, ...);
#endif
		void MonsterChat(BYTE bMonsterChatType);
		void SendGreetMessage();

		void ResetPoint(int iLv);

		void SetBlockMode(BYTE bFlag);
		void SetBlockModeForce(BYTE bFlag);
		bool IsBlockMode(BYTE bFlag) const { return (m_pointsInstant.bBlockMode & bFlag)?true:false; }

		bool IsPolymorphed() const { return m_dwPolymorphRace>0; }
		bool IsPolyMaintainStat() const { return m_bPolyMaintainStat; }
		void SetPolymorph(DWORD dwRaceNum, bool bMaintainStat = false);
		DWORD GetPolymorphVnum() const { return m_dwPolymorphRace; }
		int GetPolymorphPower() const;

		// FISING
		void fishing();
		void fishing_take();
		// END_OF_FISHING

		// MINING
		void mining(LPCHARACTER chLoad);
		void mining_cancel();
		void mining_take();
		// END_OF_MINING

		void ResetPlayTime(DWORD dwTimeRemain = 0);

		void CreateFly(BYTE bType, LPCHARACTER pkVictim);

		void ResetChatCounter();
		BYTE IncreaseChatCounter();
		BYTE GetChatCounter() const;

#ifdef ENABLE_MOUNT_SYSTEM
		void ResetMountCounter();
		BYTE IncreaseMountCounter();
		BYTE GetMountCounter() const;
#endif

#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
		void SendPickupItemPacket(int item_vnum);
#endif

	protected:
		DWORD m_dwPolymorphRace;
		bool m_bPolyMaintainStat;
		DWORD m_dwLoginPlayTime;
		DWORD m_dwPlayerID;
		VID m_vid;
		std::string m_stName;
		BYTE m_bCharType;

		CHARACTER_POINT m_points;
		CHARACTER_POINT_INSTANT m_pointsInstant;
		mutable TCharacterItemStorage* m_pItemStorage = nullptr;
		TCharacterItemStorage& ItemStorage() const
		{
			if (!m_pItemStorage)
				m_pItemStorage = new TCharacterItemStorage{};
			return *m_pItemStorage;
		}
#ifdef ENABLE_KILL_STATISTICS
		CHARACTER_KILL_STATISTICS	m_killstatistics;
#endif

		int m_iMoveCount;
		DWORD m_dwPlayStartTime;
		BYTE m_bAddChrState;
		bool m_bSkipSave;
		bool m_bGrantingLevelStepFromExp;
		bool m_bLevelUpConfirmed = false;
		DWORD m_dwLevelUpPromptTime = 0;
		BYTE m_bChatCounter;
#ifdef ENABLE_MOUNT_SYSTEM
		BYTE m_bMountCounter;
#endif

	public:
		bool IsStateMove() const { return IsState((CState&)m_stateMove); }
		bool IsStateIdle() const { return IsState((CState&)m_stateIdle); }
		bool IsWalking() const {
#ifdef ENABLE_BOT_PLAYER
			return m_bNowWalking || (GetStamina() <= 0 && !IsBotCharacter()); // Botlar ST'ye bagli yurume moduna gecmez
#else
			return m_bNowWalking || GetStamina()<=0;
#endif
		}
		void SetWalking(bool bWalkFlag) { m_bWalking=bWalkFlag; }
		void SetNowWalking(bool bWalkFlag);
		void ResetWalking() { SetNowWalking(m_bWalking); }

		bool Goto(long x, long y);
		void Stop();

		bool CanMove() const;

		void SyncPacket();
		bool Sync(long x, long y);
		bool Move(long x, long y);
		void OnMove(bool bIsAttack = false);
		DWORD GetMotionMode() const;
		float GetMoveMotionSpeed() const;
		float GetMoveSpeed() const;
		void CalculateMoveDuration();
		void SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime=0, int iRot=-1);
		DWORD GetCurrentMoveDuration() const { return m_dwMoveDuration; }
		DWORD GetWalkStartTime() const { return m_dwWalkStartTime; }
		DWORD GetLastMoveTime() const { return m_dwLastMoveTime; }
		DWORD GetLastAttackTime() const { return m_dwLastAttackTime; }

		void SetLastAttacked(DWORD time);
		DWORD GetLastAttackedTime() const	{ return m_dwLastAttackedTime;}

		bool SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList = true);
		bool IsSyncOwner(LPCHARACTER ch) const;

		bool WarpSet(long x, long y, long lRealMapIndex = 0);
		void SetWarpLocation(long lMapIndex, long x, long y);
		void WarpEnd();
		const PIXEL_POSITION & GetWarpPosition() const { return m_posWarp; }
		bool WarpToPID(DWORD dwPID);

#ifdef ENABLE_TELEPORT_TO_A_FRIEND
		bool SummonCharacter(LPCHARACTER victim);
		bool IsAllowedMapIndex(int mapindex);
#endif

		void SaveExitLocation();
		void ExitToSavedLocation();

		void StartStaminaConsume();
		void StopStaminaConsume();
		bool IsStaminaConsume() const;
		bool IsStaminaHalfConsume() const;

		void ResetStopTime();
		DWORD GetStopTime() const;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		BYTE GetLanguage() const;
		bool ChangeLanguage(BYTE bLanguage);
		BYTE GetCountryFlagFromGMList(BYTE bFlag, const char* TargetName = "", bool bMessenger = false);
#endif

	protected:
		void ClearSync();

#ifdef ENABLE_FLY_FIX
		DWORD m_fSyncTime;
#else
		float m_fSyncTime;
#endif
		LPCHARACTER m_pkChrSyncOwner;
		CHARACTER_LIST m_kLst_pkChrSyncOwned;

		PIXEL_POSITION m_posDest;
		PIXEL_POSITION m_posStart;
		PIXEL_POSITION m_posWarp;
		long m_lWarpMapIndex;

		PIXEL_POSITION m_posExit;
		long m_lExitMapIndex;

		DWORD m_dwMoveStartTime;
		DWORD m_dwMoveDuration;

		DWORD m_dwLastMoveTime;
		DWORD m_dwLastAttackTime;
		DWORD m_dwWalkStartTime;
		DWORD m_dwLastAttackedTime;
		DWORD m_dwStopTime;

		bool m_bWalking;
		bool m_bNowWalking;
		bool m_bStaminaConsume;

	public:
		void SyncQuickslot(BYTE bType, BYTE bOldPos, BYTE bNewPos);
		bool GetQuickslot(BYTE pos, TQuickslot ** ppSlot);
		bool SetQuickslot(BYTE pos, TQuickslot & rSlot);
		bool DelQuickslot(BYTE pos);
		bool SwapQuickslot(BYTE a, BYTE b);
		void ChainQuickslotItem(LPITEM pItem, BYTE bType, BYTE bOldPos);

	protected:
		TQuickslot m_quickslot[QUICKSLOT_MAX_NUM];
#ifdef ENABLE_FISH_EVENT
		TPlayerFishEventSlot*	m_fishSlots;
#endif

	public:
		void StartAffectEvent();
		void ClearAffect(bool bSave=false, bool bSomeAffect = false);
		void ComputeAffect(CAffect * pkAff, bool bAdd, bool bTemp = false);
		bool AddAffect(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube = false
#ifdef ENABLE_NINETH_SKILL
			, uint8_t bShieldDuration = 0
#endif
		); //@fixme532
		void RefreshAffect();
		bool RemoveAffect(DWORD dwType);
		bool IsAffectFlag(DWORD dwAff) const;

		bool UpdateAffect();
		int ProcessAffect();

		void LoadAffect(DWORD dwCount, TPacketAffectElement * pElements);
		void SaveAffect();

		bool IsLoadedAffect() const { return m_bIsLoadedAffect; }

		bool IsGoodAffect(BYTE bAffectType) const;

		void RemoveGoodAffect();
		void RemoveBadAffect();

		CAffect * FindAffect(DWORD dwType, BYTE bApply=APPLY_NONE) const;
		const std::list<CAffect *> & GetAffectContainer() const	{ return m_list_pkAffect; }
		bool RemoveAffect(CAffect* pkAff, bool single = true); //@fixme433

	protected:
		bool m_bIsLoadedAffect;
		TAffectFlag m_afAffectFlag;
		std::list<CAffect *> m_list_pkAffect;

	public:
		void SetParty(LPPARTY pkParty);
		LPPARTY GetParty() const { return m_pkParty; }

		bool RequestToParty(LPCHARACTER leader);
		void DenyToParty(LPCHARACTER member);
		void AcceptToParty(LPCHARACTER member);
		void PartyInvite(LPCHARACTER pchInvitee);
		void PartyInviteAccept(LPCHARACTER pchInvitee);
		void PartyInviteDeny(DWORD dwPID);

		bool BuildUpdatePartyPacket(TPacketGCPartyUpdate & out);
		int GetLeadershipSkillLevel() const;

		bool CanSummon(int iLeaderShip);

		void SetPartyRequestEvent(LPEVENT pkEvent) { m_pkPartyRequestEvent = pkEvent; }

	protected:
		void PartyJoin(LPCHARACTER pkLeader);

		enum PartyJoinErrCode {
			PERR_NONE = 0,
			PERR_SERVER,
			PERR_DUNGEON,
			PERR_OBSERVER,
			PERR_LVBOUNDARY,
			PERR_LOWLEVEL,
			PERR_HILEVEL,
			PERR_ALREADYJOIN,
			PERR_PARTYISFULL,
			PERR_SEPARATOR,
			PERR_DIFFEMPIRE,
			PERR_MAX
		};

		static PartyJoinErrCode	IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		static PartyJoinErrCode	IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

		LPPARTY m_pkParty;
		DWORD m_dwLastDeadTime;
		LPEVENT m_pkPartyRequestEvent;

		typedef std::map< DWORD, LPEVENT > EventMap;
		EventMap m_PartyInviteEventMap;

	public:
		void SetDungeon(LPDUNGEON pkDungeon);
		LPDUNGEON GetDungeon() const { return m_pkDungeon; }
		LPDUNGEON GetDungeonForce() const;

	protected:
		LPDUNGEON m_pkDungeon;
		int m_iEventAttr;

	public:
		void SetGuild(CGuild * pGuild);
		CGuild* GetGuild() const { return m_pGuild; }

		void SetWarMap(CWarMap* pWarMap);
		CWarMap* GetWarMap() const { return m_pWarMap; }

	protected:
		CGuild * m_pGuild;
		DWORD m_dwUnderGuildWarInfoMessageTime;
		CWarMap * m_pWarMap;

	public:
		bool CanHandleItem(bool bSkipRefineCheck = false, bool bSkipObserver = false);

		bool IsItemLoaded() const { return m_bItemLoaded; }
		void SetItemLoaded() { m_bItemLoaded = true; }

		void ClearItem();
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		void SetItem(TItemPos Cell, LPITEM item, bool bHighlight = false);
#else
		void SetItem(TItemPos Cell, LPITEM item);
#endif
		LPITEM GetItem(TItemPos Cell) const;
		LPITEM GetInventoryItem(WORD wCell) const;

#ifdef ENABLE_SPECIAL_INVENTORY
		LPITEM GetSkillBookInventoryItem(WORD wCell) const;
		LPITEM GetUpgradeItemsInventoryItem(WORD wCell) const;
		LPITEM GetStoneInventoryItem(WORD wCell) const;
		LPITEM GetGiftBoxInventoryItem(WORD wCell) const;
		LPITEM GetChangersInventoryItem(WORD wCell) const;
#endif

		bool IsEmptyItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;

		void SetWear(UINT bCell, LPITEM item);
		LPITEM GetWear(UINT bCell) const;

		void UseSilkBotary(void);
		void UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p);

		bool UseItemEx(LPITEM item, TItemPos DestCell);
		bool UseItem(TItemPos Cell, TItemPos DestCell = NPOS);

		bool IsRefineThroughGuild() const;
		CGuild * GetRefineGuild() const;
		int ComputeRefineFee(int iCost, int iMultiply = 5) const;
		void PayRefineFee(int iTotalMoney);
		void SetRefineNPC(LPCHARACTER ch);

		bool RefineItem(LPITEM pkItem, LPITEM pkTarget);
#ifdef ENABLE_STACK_LIMIT
		bool DropItem(TItemPos Cell, WORD bCount=0);
#else
		bool DropItem(TItemPos Cell, BYTE bCount=0);
#endif
#if defined(ENABLE_DESTROY_DIALOG) && defined(ENABLE_STACK_LIMIT)
		bool DestroyItem(TItemPos Cell, WORD bCount = 0);
#elif defined(ENABLE_DESTROY_DIALOG) && !defined(ENABLE_STACK_LIMIT)
		bool DestroyItem(TItemPos Cell, BYTE bCount = 0);
#endif

#ifdef ENABLE_QUICK_SELL_ITEM
		bool SellItem(TItemPos Cell);
#endif

		bool GiveRecallItem(LPITEM item);
		void ProcessRecallItem(LPITEM item);

		void EffectPacket(int enumEffectType);
		void SpecificEffectPacket(const char filename[128]);

		bool DoRefine(LPITEM item, bool bMoneyOnly = false
#ifdef ENABLE_PITTY_REFINE
, bool bUseSealOfGod = false
#endif
		);

		bool DoRefineWithScroll(LPITEM item
#ifdef ENABLE_PITTY_REFINE
, bool bUseSealOfGod = false
#endif
		);

#ifdef ENABLE_QUEEN_NETHIS
		bool DoRefineSerpent(LPITEM item
#ifdef ENABLE_PITTY_REFINE
, bool bUseSealOfGod = false
#endif
		);
#endif
		bool RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell = -1);

		void SetRefineMode(int iAdditionalCell = -1);
		void ClearRefineMode();

		bool GiveItem(LPCHARACTER victim, TItemPos Cell);
		bool CanReceiveItem(LPCHARACTER from, LPITEM item) const;
		void ReceiveItem(LPCHARACTER from, LPITEM item);
		bool GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector <DWORD> &dwItemVnums, std::vector <DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count);

#ifdef ENABLE_STACK_LIMIT
		bool MoveItem(TItemPos pos, TItemPos change_pos, WORD num);
#else
		bool MoveItem(TItemPos pos, TItemPos change_pos, BYTE num);
#endif
		bool PickupItem(DWORD vid);
		bool EquipItem(LPITEM item, int iCandidateCell = -1);
		bool UnequipItem(LPITEM item);

		bool CanEquipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);
		bool CanUnequipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

		bool SwapItem(UINT bCell, UINT bDestCell);
#ifdef ENABLE_STACK_LIMIT
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		LPITEM AutoGiveItem(DWORD dwItemVnum, WORD bCount=1, int iRarePct = -1, bool bMsg = true, bool bHighlight = false);
#else
		LPITEM AutoGiveItem(DWORD dwItemVnum, WORD bCount=1, int iRarePct = -1, bool bMsg = true);
#endif
#else
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		LPITEM AutoGiveItem(DWORD dwItemVnum, BYTE bCount=1, int iRarePct = -1, bool bMsg = true, bool bHighlight = false);
#else
		LPITEM AutoGiveItem(DWORD dwItemVnum, BYTE bCount=1, int iRarePct = -1, bool bMsg = true);
#endif
#endif

#ifdef ENABLE_PICKUP_ITEM_EFFECT
		void AutoGiveItem(LPITEM item, bool longOwnerShip = false, bool bHighlight = false);
#else
		void AutoGiveItem(LPITEM item, bool longOwnerShip = false);
#endif

		int GetEmptyInventory(BYTE size) const;

		int GetEmptyDragonSoulInventory(LPITEM pItem) const;
		void CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const;

#ifdef ENABLE_SPECIAL_INVENTORY
		int GetEmptySkillBookInventory(BYTE size) const;
		int GetEmptyUpgradeItemsInventory(BYTE size) const;
		int GetEmptyStoneInventory(BYTE size) const;
		int GetEmptyGiftBoxInventory(BYTE size) const;
		int GetEmptyChangersInventory(BYTE size) const;
#endif

		int CountEmptyInventory() const;
#ifdef ENABLE_SKILL_BOOK_READING
		int BKBul(long skillindex) const;
#endif

#ifdef ENABLE_PENDANT_SYSTEM
		void RemoveSpecifyItem(DWORD vnum, DWORD count = 1, int iExceptionCell = -1);
		int CountSpecifyItem(DWORD vnum, int iExceptionCell = -1) const;
#else
		int CountSpecifyItem(DWORD vnum) const;
		void RemoveSpecifyItem(DWORD vnum, DWORD count = 1);
#endif

		LPITEM FindSpecifyItem(DWORD vnum) const;
		LPITEM FindItemByID(DWORD id) const;

		int CountSpecifyTypeItem(BYTE type) const;
		void RemoveSpecifyTypeItem(BYTE type, DWORD count = 1);

		bool IsEquipUniqueItem(DWORD dwItemVnum) const;
		bool IsEquipUniqueGroup(DWORD dwGroupVnum) const;

		void SendEquipment(LPCHARACTER ch);
		bool IsUnderRefine() { return m_bUnderRefine; }

#ifdef ENABLE_ATTENDANCE_EVENT
		std::vector<THitCountInfo> m_hitCount;
#endif

	protected:
#ifdef ENABLE_GOLD_LIMIT
		void SendMyShopPriceListCmd(DWORD dwItemVnum, long long dwItemPrice);
#else
		void SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice);
#endif

		bool m_bNoOpenedShop;

		bool m_bItemLoaded;
		int m_iRefineAdditionalCell;
		bool m_bUnderRefine;
		DWORD m_dwRefineNPCVID;

	public:
#ifdef ENABLE_GOLD_LIMIT
		long long GetGold() const { return m_points.gold; }
		void SetGold(long long gold) { m_points.gold = gold; }
		bool DropGold(INT gold);
		long long GetAllowedGold() const;
		void GiveGold(long long iAmount);
#else
		INT GetGold() const { return m_points.gold; }
		void SetGold(INT gold) { m_points.gold = gold; }
		bool DropGold(INT gold);
		INT GetAllowedGold() const;
		void GiveGold(INT iAmount);
#endif
#ifdef ENABLE_GAYA_SYSTEM
		INT GetGem() const			{ return m_points.gem; }
		void SetGem(INT gem)			{ m_points.gem = gem; }
		void GiveGem(INT iAmount);
#endif
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
		INT GetSoulPoint() const		{ return m_points.soul;	}
		void SetSoulPoint(INT soul)	{ m_points.soul = soul;	}
		INT GetSoulRePoint() const		{ return m_points.soulre;	}
		void SetSoulRePoint(INT soulre)	{ m_points.soulre = soulre;	}
#endif
#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
		int GetHalounLv() const		{ return m_points.halounlv;	}
		void SetHaloun1(int halounlv)	{ m_points.halounlv = halounlv;	}

		long long GetHalounPoints() const		{ return m_points.phaloun;	}
		void SetHaloun2(long long phaloun)	{ m_points.phaloun = phaloun;	}

		int GetRewardHaloun() const		{ return m_points.rhaloun;	}
		void SetHaloun3(int rhaloun)	{ m_points.rhaloun = rhaloun;	}
#endif
#ifdef ENABLE_SORT_INVENTORY
		void SetNextSortInventoryPulse(int pulse) { m_sortInventoryPulse = pulse; }
		int GetSortInventoryPulse() { return m_sortInventoryPulse; }

		void SetNextSortSpecialStoragePulse(int pulse) { m_sortSpecialStoragePulse = pulse; }
		int GetSortSpecialStoragePulse() { return m_sortSpecialStoragePulse; }
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	public:
		void SetSkillColor(DWORD* dwSkillColor);
		DWORD* GetSkillColor() { return m_dwSkillColor[0]; }

	protected:
		DWORD m_dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif

	public:
		void SetShop(LPSHOP pkShop);
		LPSHOP GetShop() const { return m_pkShop; }
		void ShopPacket(BYTE bSubHeader);

		void SetShopOwner(LPCHARACTER ch) { m_pkChrShopOwner = ch; }
		LPCHARACTER GetShopOwner() const { return m_pkChrShopOwner;}

#ifdef ENABLE_STACK_LIMIT
		void OpenMyShop(const char * c_pszSign, TShopItemTable * pTable, WORD bItemCount);
#else
		void OpenMyShop(const char * c_pszSign, TShopItemTable * pTable, BYTE bItemCount);
#endif

#ifdef ENABLE_CHANGE_CHANNEL
		void ChangeChannel(DWORD channelId);
#endif

		LPSHOP GetMyShop() const { return m_pkMyShop; }
		void CloseMyShop();

	protected:
		LPSHOP m_pkShop;
		LPSHOP m_pkMyShop;
		std::string m_stShopSign;
		LPCHARACTER m_pkChrShopOwner;

#ifdef ENABLE_SORT_INVENTORY
		int m_sortInventoryPulse;
		int m_sortSpecialStoragePulse;
#endif

	public:
		bool ExchangeStart(LPCHARACTER victim);
		void SetExchange(CExchange * pkExchange);
		CExchange * GetExchange() const	{ return m_pkExchange;	}

#ifdef ENABLE_FISH_EVENT
		void 			FishEventGeneralInfo();
		void			FishEventUseBox(TItemPos itemPos);
		bool 			FishEventIsValidPosition(BYTE shapePos, BYTE shapeType);
		void 			FishEventPlaceShape(BYTE shapePos, BYTE shapeType);
		void 			FishEventAddShape(BYTE shapePos);
		void 			FishEventCheckEnd();
#endif

	protected:
		CExchange * m_pkExchange;

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	public:
		void SetSoulRoulette(CSoulRoulette* pt);
		CSoulRoulette* GetSoulRoulette() const { return pSoulRoulette; }
	private:
		CSoulRoulette* pSoulRoulette;
#endif

	public:
		struct TBattleInfo
		{
			int iTotalDamage;
			int iAggro;

			TBattleInfo(int iTot, int iAggr) : iTotalDamage(iTot), iAggro(iAggr) {}
		};
		typedef std::map<VID, TBattleInfo> TDamageMap;

		typedef struct SAttackLog
		{
			DWORD dwVID;
			DWORD dwTime;
		} AttackLog;

		bool Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		bool __Profile__Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
		void DeathPenalty(BYTE bExpLossPercent);
		void ReviveInvisible(int iDur);

		bool Attack(LPCHARACTER pkVictim, BYTE bType = 0);
		bool IsAlive() const { return m_pointsInstant.position == POS_DEAD ? false : true; }
		bool CanFight() const;

		bool CanBeginFight() const;
		void BeginFight(LPCHARACTER pkVictim);

		bool CounterAttack(LPCHARACTER pkChr);

		bool IsStun() const;
		void Stun();
		bool IsDead() const;
		void Dead(LPCHARACTER pkKiller = NULL, bool bImmediateDead=false);
		void RewardlessDead();  // DevFix 29

		void Reward(bool bItemDrop);
		void RewardGold(LPCHARACTER pkAttacker);

#ifdef ENABLE_ATTENDANCE_EVENT
		void RewardAttendance();
#endif

		bool Shoot(BYTE bType);
		void FlyTarget(DWORD dwTargetVID, long x, long y, BYTE bHeader);

		void ForgetMyAttacker();
		void AggregateMonster();
		void AttractRanger();
		void PullMonster();
#ifdef ENABLE_SUNG_MAHI_TOWER
		void AggregateMonsterByMaster();
#endif
		int GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, int iArrowCount = 1);
		void UseArrow(LPITEM pkArrow, DWORD dwArrowCount);

		void AttackedByPoison(LPCHARACTER pkAttacker);
		void RemovePoison();

		void AttackedByFire(LPCHARACTER pkAttacker, int amount, int count);
		void RemoveFire();

		void UpdateAlignment(int iAmount);
		int GetAlignment() const;

		int GetRealAlignment() const;
		void ShowAlignment(bool bShow);

		void SetKillerMode(bool bOn);
		bool IsKillerMode() const;
		void UpdateKillerMode();

		BYTE GetPKMode() const;
		void SetPKMode(BYTE bPKMode);

		void ItemDropPenalty(LPCHARACTER pkKiller);

		void UpdateAggrPoint(LPCHARACTER ch, EDamageType type, int dam);

	public:
		void SetComboSequence(BYTE seq);
		BYTE GetComboSequence() const;

		void SetLastComboTime(DWORD time);
		DWORD GetLastComboTime() const;

		int GetValidComboInterval() const;
		void SetValidComboInterval(int interval);

		BYTE GetComboIndex() const;

		void IncreaseComboHackCount(int k = 1);
		void ResetComboHackCount();
		void SkipComboAttackByTime(int interval);
		DWORD GetSkipComboAttackByTime() const;

	protected:
		BYTE m_bComboSequence;
		DWORD m_dwLastComboTime;
		int m_iValidComboInterval;
		BYTE m_bComboIndex;
		int m_iComboHackCount;
		DWORD m_dwSkipComboAttackByTime;

	protected:
		void UpdateAggrPointEx(LPCHARACTER ch, EDamageType type, int dam, TBattleInfo & info);
		void ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim);

		DWORD m_dwFlyTargetID;
		std::vector<DWORD> m_vec_dwFlyTargets;
		TDamageMap m_map_kDamage;
		DWORD m_dwKillerPID;

		int m_iAlignment;
		int m_iRealAlignment;
		int m_iKillerModePulse;
		BYTE m_bPKMode;

		DWORD m_dwLastVictimSetTime;
		int m_iMaxAggro;

	public:
		void SetStone(LPCHARACTER pkChrStone);
#ifdef ENABLE_FIX_EXP_DROP_STONES
		void ClearStone(LPCHARACTER pkKiller = nullptr);
#else
		void ClearStone();
#endif
		void DetermineDropMetinStone();
		DWORD GetDropMetinStoneVnum() const { return m_dwDropMetinStone; }
		BYTE GetDropMetinStonePct() const { return m_bDropMetinStonePct; }

	protected:
		LPCHARACTER m_pkChrStone;
		CHARACTER_SET m_set_pkChrSpawnedBy;
		DWORD m_dwDropMetinStone;
		BYTE m_bDropMetinStonePct;

	public:
		enum
		{
			SKILL_UP_BY_POINT,
			SKILL_UP_BY_BOOK,
			SKILL_UP_BY_TRAIN,
			SKILL_UP_BY_QUEST,
		};

		void SkillLevelPacket();
#ifdef ENABLE_NINETH_SKILL
		bool NineSkillCanUp(uint32_t dwVnum);
		bool IsNineSkill(uint32_t dwSkillVnum);
		bool LearnNineSkillByBook(uint32_t dwSkillVnum, uint8_t bSkillGroup);
#endif
		void SkillLevelUp(DWORD dwVnum, BYTE bMethod = SKILL_UP_BY_POINT);
		bool SkillLevelDown(DWORD dwVnum);

		bool UseSkill(DWORD dwVnum, LPCHARACTER pkVictim, bool bUseGrandMaster = true);
		void ResetSkill();
		void ResetSkillCoolTimes();
		void SetSkillLevel(DWORD dwVnum, BYTE bLev);
		void EnsureSkillLevels();
		int GetUsedSkillMasterType(DWORD dwVnum);

		bool IsLearnableSkill(DWORD dwSkillVnum) const;

		bool CheckSkillHitCount(const BYTE SkillID, const VID dwTargetVID);
		bool CanUseSkill(DWORD dwSkillVnum) const;
		bool IsUsableSkillMotion(DWORD dwMotionIndex) const;
		int GetSkillLevel(DWORD dwVnum) const;
		int GetSkillMasterType(DWORD dwVnum) const;
		int GetSkillPower(DWORD dwVnum, BYTE bLevel = 0) const;

		time_t GetSkillNextReadTime(DWORD dwVnum) const;
		void SetSkillNextReadTime(DWORD dwVnum, time_t time);
		void SkillLearnWaitMoreTimeMessage(DWORD dwVnum);

		void ComputePassiveSkill(DWORD dwVnum);
		int ComputeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#ifdef ENABLE_PVP_BALANCE
		int ComputeGyeongGongSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#endif
		int ComputeSkillAtPosition(DWORD dwVnum, const PIXEL_POSITION& posTarget, BYTE bSkillLevel = 0);
#ifdef ENABLE_PARTY_BUFF_SYSTEM
		int ComputeSkillParty(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel);
#endif
		void ComputeSkillPoints();

		void SetSkillGroup(BYTE bSkillGroup);
		BYTE GetSkillGroup() const { return m_points.skill_group; }

		int ComputeCooltime(int time);

		void GiveRandomSkillBook();

		void DisableCooltime();
		bool LearnSkillByBook(DWORD dwSkillVnum, BYTE bProb = 0);
		bool LearnGrandMasterSkill(DWORD dwSkillVnum);
#ifdef ENABLE_SKILL_COOLDOWN_CHECK
		bool SkillIsOnCooldown(int vnum) { return m_SkillUseInfo[vnum].IsOnCooldown(vnum); }
#endif


	private:
		bool m_bDisableCooltime;
		DWORD m_dwLastSkillTime;

	public:
		bool HasMobSkill() const;
		size_t CountMobSkill() const;
		const TMobSkillInfo * GetMobSkill(unsigned int idx) const;
		bool CanUseMobSkill(unsigned int idx) const;
		bool UseMobSkill(unsigned int idx);
		void ResetMobSkillCooltime();

	protected:
		DWORD m_adwMobSkillCooltime[MOB_SKILL_MAX_NUM];
		// END_OF_MOB_SKILL

		// for SKILL_MUYEONG
	public:
		void StartMuyeongEvent();
		void StopMuyeongEvent();

#ifdef ENABLE_PVP_BALANCE
		void StartGyeongGongEvent();
		void StopGyeongGongEvent();
#endif
#ifdef ENABLE_NINETH_SKILL
		void StartCheonunEvent(uint8_t bShieldChance, uint8_t bShieldDuration);
		void StopCheonunEvent();
#endif

	private:
		LPEVENT m_pkMuyeongEvent;
#ifdef ENABLE_PVP_BALANCE
		LPEVENT m_pkGyeongGongEvent;
#endif
#ifdef ENABLE_NINETH_SKILL
		LPEVENT m_pkCheonunEvent;
#endif

		// for SKILL_CHAIN lighting
	public:
		int GetChainLightningIndex() const { return m_iChainLightingIndex; }
		void IncChainLightningIndex() { ++m_iChainLightingIndex; }
		void AddChainLightningExcept(LPCHARACTER ch) { m_setExceptChainLighting.insert(ch); }
		void ResetChainLightningIndex() { m_iChainLightingIndex = 0; m_setExceptChainLighting.clear(); }
		int GetChainLightningMaxCount() const;
		const CHARACTER_SET& GetChainLightingExcept() const { return m_setExceptChainLighting; }

	private:
		int m_iChainLightingIndex;
		CHARACTER_SET m_setExceptChainLighting;

		// for SKILL_EUNHYUNG
	public:
		void SetAffectedEunhyung();
		void ClearAffectedEunhyung() { m_dwAffectedEunhyungLevel = 0; }
		bool GetAffectedEunhyung() const { return m_dwAffectedEunhyungLevel; }

	private:
		DWORD m_dwAffectedEunhyungLevel;

	protected:
		TPlayerSkill* m_pSkillLevels;
		std::unordered_map<BYTE, int> m_SkillDamageBonus;
		std::map<int, TSkillUseInfo> m_SkillUseInfo;

	public:
		void AssignTriggers(const TMobTable * table);
		LPCHARACTER GetVictim() const;
		void SetVictim(LPCHARACTER pkVictim);
		LPCHARACTER GetNearestVictim(LPCHARACTER pkChr);
		LPCHARACTER GetProtege() const;

		bool Follow(LPCHARACTER pkChr, float fMinimumDistance = 150.0f);
		bool Return();
		bool IsGuardNPC() const;
		bool IsChangeAttackPosition(LPCHARACTER target) const;
		void ResetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time() - AI_CHANGE_ATTACK_POISITION_TIME_NEAR;}
		void SetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time();}
#ifdef ENABLE_DEFENSAWE_SHIP
		bool IsHydraMob() const;
		bool IsHydraMobLP(LPCHARACTER ch) const;
		bool IsHydraNPC() const;
		bool IsHydra() const;
#endif

		bool OnIdle();

		void OnAttack(LPCHARACTER pkChrAttacker);
		void OnClick(LPCHARACTER pkChrCauser);

		VID m_kVIDVictim;

#ifdef ENABLE_MOB_FOLLOW_SYSTEM
		void			AddEnemyNPC(DWORD dwNPCVID);
		void			RemoveEnemyNPC(DWORD dwNPCVID);
		void			ClearEnemyNPCs();
		void			NotifyEnemyNPCs();
#endif

	protected:
		DWORD m_dwLastChangeAttackPositionTime;
		CTrigger m_triggerOnClick;
#ifdef ENABLE_MOB_FOLLOW_SYSTEM
		std::set<DWORD>	m_set_dwEnemyNPCVID;
#endif

	protected:
		LPCHARACTER m_pkChrTarget;
		CHARACTER_SET m_set_pkChrTargetedBy;

	public:
		void SetTarget(LPCHARACTER pkChrTarget);
		void BroadcastTargetPacket();
		void ClearTarget();
		void CheckTarget();
		LPCHARACTER GetTarget() const { return m_pkChrTarget; }

	public:
		int GetSafeboxSize() const;
		void QuerySafeboxSize();
		void SetSafeboxSize(int size);

		CSafebox * GetSafebox() const;
		void LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems);
		void ChangeSafeboxSize(BYTE bSize);
		void CloseSafebox();

		void ReqSafeboxLoad(const char* pszPassword);
		void CancelSafeboxLoad( void ) { m_bOpeningSafebox = false; }

#ifdef ENABLE_SPECIAL_INVENTORY
		void SafeboxLoad(bool forceOpen = false);
		void MallOpen(bool forceOpen = false);
#endif

		void SetMallLoadTime(int t) { m_iMallLoadTime = t; }
		int GetMallLoadTime() const { return m_iMallLoadTime; }

		CSafebox * GetMall() const;
		void LoadMall(int iItemCount, TPlayerItem * pItems);
		void CloseMall();

		void SetSafeboxOpenPosition();
		float GetDistanceFromSafeboxOpen() const;

	protected:
		CSafebox * m_pkSafebox;
		int m_iSafeboxSize;
		int m_iSafeboxLoadTime;
		bool m_bOpeningSafebox;

		CSafebox * m_pkMall;
		int m_iMallLoadTime;

		PIXEL_POSITION m_posSafeboxOpen;

#ifdef ENABLE_UN_SEAL_SCROLL_PLUS
	public:
		void SetUnsealScrollPlusPasswordSuccessTime(DWORD dwTime) { m_dwUnsealScrollPlusPasswordSuccessTime = dwTime; }
		DWORD GetUnsealScrollPlusPasswordSuccessTime() const { return m_dwUnsealScrollPlusPasswordSuccessTime; }
	protected:
		DWORD m_dwUnsealScrollPlusPasswordSuccessTime;
#endif

	public:
		void MountVnum(DWORD vnum);
		DWORD GetMountVnum() const { return m_dwMountVnum; }
		DWORD GetLastMountTime() const { return m_dwMountTime; }

		bool CanUseHorseSkill();

		virtual	void SetHorseLevel(int iLevel);

		virtual	bool StartRiding();
		virtual	bool StopRiding();

		virtual	DWORD GetMyHorseVnum() const;

		virtual	void HorseDie();
		virtual bool ReviveHorse();

		virtual void SendHorseInfo();
		virtual	void ClearHorseInfo();

		void HorseSummon(bool bSummon, bool bFromFar = false, DWORD dwVnum = 0, const char* name = 0);

		LPCHARACTER GetHorse() const { return m_chHorse; }
		LPCHARACTER GetRider() const;
		void SetRider(LPCHARACTER ch);

		bool IsRiding() const;

#ifdef __AUTO_HUNT__
	public:
		void			GetAutoHuntCommand(const char* szArgument);
		void			SetAutoHuntStatus(bool bStatus, bool bMobFarm = false, bool bMetinFarm = false);
		bool			IsAutoHuntAffectHas();
		bool			IsAutoHuntStatus() const { return m_bAutoHuntStatus; }

	protected:
		std::vector<std::pair<BYTE, int>> m_vecAutoHuntItems;
		bool			m_bAutoHuntStatus;
#endif

#ifdef ENABLE_MOUNT_SYSTEM
	public:
		CMountSystem* GetMountSystem() { return m_mountSystem; }

		void MountSummon(LPITEM mountItem);
		void MountUnsummon(LPITEM mountItem);
		void CheckMount();
		bool IsRidingMount();

#ifdef ENABLE_MOUNT_PET_SKIN
		void EquipCostumeMountSkin();
		void RemoveCostumeMountSkin(LPITEM mountSkin);

		bool CanChangeCostumeMountSkin();
		void UpdateChangeCostumeMountSkinTime() { m_iChangeCostumeMountSkinTime = thecore_pulse(); }
		int GetLastChangeCostumeMountSkinTime() const { return m_iChangeCostumeMountSkinTime; }
#endif

	protected:
		CMountSystem* m_mountSystem;

#ifdef ENABLE_MOUNT_PET_SKIN
	private:
		int m_iChangeCostumeMountSkinTime;
#endif
#endif

#ifdef ENABLE_PET_SYSTEM
	public:
		CPetSystem* GetPetSystem() { return m_petSystem; }
		void PetSummon(LPITEM petItem);
		void PetUnsummon(LPITEM petItem);
		void CheckPet();
		bool IsPetSummon();
		void ReloadPetBonus(LPITEM pSummonItem);

#ifdef ENABLE_MOUNT_PET_SKIN
		void EquipCostumePetSkin();
		void RemoveCostumePetSkin(LPITEM petSkin);

		bool CanChangeCostumePetSkin();
		void UpdateChangeCostumePetSkinTime() { m_iChangeCostumePetSkinTime = thecore_pulse(); }
		int GetLastChangeCostumePetSkinTime() const { return m_iChangeCostumePetSkinTime; }
#endif

	protected:
		CPetSystem* m_petSystem;

#ifdef ENABLE_MOUNT_PET_SKIN
	private:
		int m_iChangeCostumePetSkinTime;
#endif
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	public:
		bool IsGrowthPet() { return m_bCharType == CHAR_TYPE_GROWTH_PET; };
		void SetInvincible(bool bInvincible) { m_bInvincible = bInvincible; }
		bool IsInvincible() { return m_bInvincible; }

		void SetPetHatchWindow(bool bState) { m_bIsPetHatchOpen = bState; }
		bool IsPetHatchWindowOpen() { return m_bIsPetHatchOpen; }
		void SetPetChangeNameWindow(bool bState) { m_bIsPetChangeNameOpen = bState; }
		bool IsPetChangeNameWindowOpen() { return m_bIsPetChangeNameOpen; }

		void SetPetWindowType(BYTE bType) { m_bPetWindowType = bType; }
		BYTE GetPetWindowType() { return m_bPetWindowType; }

		void SetGrowthPetLoaded(bool bState) { m_bIsGrowthPetLoaded = bState; }
		bool IsGrowthPetLoaded() { return m_bIsGrowthPetLoaded; }

		void SetCharacterSize(BYTE bSize) { m_bCharacterSize = bSize; }
		BYTE GetCharacterSize() { return m_bCharacterSize; }

		bool SetGrowthPet(LPGROWTH_PET pkPet);
		bool DeleteGrowthPet(DWORD dwID);
		LPGROWTH_PET GetGrowthPet(DWORD dwID);
		void ClearGrowthPet();

		void SetActiveGrowthPet(LPGROWTH_PET pkPet) { m_activeGrowthPet = pkPet;  }
		LPGROWTH_PET GetActiveGrowthPet() { return m_activeGrowthPet; }

	private:
		DWORD m_bInvincible;
		bool m_bIsPetHatchOpen;
		bool m_bIsPetChangeNameOpen;
		BYTE m_bPetWindowType;

		bool m_bIsGrowthPetLoaded;

		CGrowthPetManager::TGrowthPetMap m_growthPetMap;
		LPGROWTH_PET m_activeGrowthPet;

		BYTE m_bCharacterSize;
#endif

	protected:
		LPCHARACTER m_chHorse;
		LPCHARACTER m_chRider;

		DWORD m_dwMountVnum;
		DWORD m_dwMountTime;

		BYTE m_bSendHorseLevel;
		BYTE m_bSendHorseHealthGrade;
		BYTE m_bSendHorseStaminaGrade;

	public:
		void DetailLog() { m_bDetailLog = !m_bDetailLog; }
		void ToggleMonsterLog();
		void MonsterLog(const char* format, ...);
	private:
		bool m_bDetailLog;
		bool m_bMonsterLog;

	public:
		void SetEmpire(BYTE bEmpire);
		BYTE GetEmpire() const { return m_bEmpire; }

	protected:
		BYTE m_bEmpire;

	public:
		void SetRegen(LPREGEN pkRegen);

	protected:
		PIXEL_POSITION m_posRegen;
		float m_fRegenAngle;
		LPREGEN m_pkRegen;
		size_t regen_id_;

	public:
		bool CannotMoveByAffect() const;
		bool IsImmune(DWORD dwImmuneFlag);
		void SetImmuneFlag(DWORD dw) { m_pointsInstant.dwImmuneFlag = dw; }

	protected:
		void ApplyMobAttribute(const TMobTable* table);

	public:
		void SetQuestNPCID(DWORD vid);
		DWORD GetQuestNPCID() const { return m_dwQuestNPCVID; }
		LPCHARACTER GetQuestNPC() const;

		void SetQuestItemPtr(LPITEM item);
		void ClearQuestItemPtr();
		LPITEM GetQuestItemPtr() const;

		void SetQuestBy(DWORD dwQuestVnum) { m_dwQuestByVnum = dwQuestVnum; }
		DWORD GetQuestBy() const { return m_dwQuestByVnum; }

		bool IsLevelUpConfirmationRequired(int iTargetLevel) const;
		void SendLevelUpConfirmationPrompt(bool bForce);
		void ConfirmLevelUp();
		int GetQuestFlag(const std::string& flag) const;
		void SetQuestFlag(const std::string& flag, int value);

		void ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID);

	private:
		DWORD m_dwQuestNPCVID;
		DWORD m_dwQuestByVnum;
		DWORD m_dwQuestItemVID{};

	public:
		bool StartStateMachine(int iPulse = 1);
		void StopStateMachine();
		void UpdateStateMachine(DWORD dwPulse);
		void SetNextStatePulse(int iPulseNext);

		void UpdateCharacter(DWORD dwPulse);

	protected:
		DWORD m_dwNextStatePulse;

	public:
		LPCHARACTER GetMarryPartner() const;
		void SetMarryPartner(LPCHARACTER ch);
		int GetMarriageBonus(DWORD dwItemVnum, bool bSum = true);

		void SetWeddingMap(marriage::WeddingMap* pMap);
		marriage::WeddingMap* GetWeddingMap() const { return m_pWeddingMap; }

	private:
		marriage::WeddingMap* m_pWeddingMap;
		LPCHARACTER m_pkChrMarried;

	public:
		void StartWarpNPCEvent();

	public:
		void StartSaveEvent();
		void StartRecoveryEvent();
		void StartCheckSpeedHackEvent();
		void StartDestroyWhenIdleEvent();

		LPEVENT m_pkDeadEvent;
#ifdef ENABLE_BOT_PLAYER
		LPEVENT m_pkBotCharacterDeadEvent;
#endif
		LPEVENT m_pkStunEvent;
		LPEVENT m_pkSaveEvent;
		LPEVENT m_pkRecoveryEvent;
		LPEVENT m_pkTimedEvent;
		LPEVENT m_pkFishingEvent;
		LPEVENT m_pkAffectEvent;
		LPEVENT m_pkPoisonEvent;
		LPEVENT m_pkFireEvent;
		LPEVENT m_pkWarpNPCEvent;

		LPEVENT m_pkMiningEvent;
		LPEVENT m_pkWarpEvent;
		LPEVENT m_pkCheckSpeedHackEvent;
		LPEVENT m_pkDestroyWhenIdleEvent;
		LPEVENT m_pkPetSystemUpdateEvent;

#ifdef ENABLE_CHANGE_CHANNEL
		LPEVENT m_pkChangeChannelEvent;
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
		LPEVENT m_pkOpenSafeboxEvent;
		LPEVENT m_pkOpenMallEvent;
#endif

		bool IsWarping() const { return m_pkWarpEvent ? true : false; }

		bool m_bHasPoisoned;

		const CMob * m_pkMobData;
		CMobInstance * m_pkMobInst;

		std::map<int, LPEVENT> m_mapMobSkillEvent;

		friend struct FuncSplashDamage;
		friend struct FuncSplashAffect;
		friend class CFuncShoot;

	public:
		int GetPremiumRemainSeconds(BYTE bType) const;

	private:
		int m_aiPremiumTimes[PREMIUM_MAX_NUM];

		// CHANGE_ITEM_ATTRIBUTES
		static const DWORD msc_dwDefaultChangeItemAttrCycle;
		static const char msc_szLastChangeItemAttrFlag[];
		static const char msc_szChangeItemAttrCycleFlag[];
		// END_OF_CHANGE_ITEM_ATTRIBUTES

		// NEW_HAIR_STYLE_ADD
	public:
		bool ItemProcess_Hair(LPITEM item, int iDestCell);
		// END_NEW_HAIR_STYLE_ADD

	public:
		void ClearSkill();
		void ClearSubSkill();

		// RESET_ONE_SKILL
		bool ResetOneSkill(DWORD dwVnum);
		// END_RESET_ONE_SKILL

#ifdef ENABLE_PITTY_REFINE
	public:
		int					GetPittyRefineLevel(DWORD dwItemVnum, int iRefineLevel);
		void				SetPittyRefineLevel(DWORD dwItemVnum, int iRefineLevel, int iValue);

		void				SendPittyInfoClient(DWORD dwItemVnum, int iRefineLevel);
		
		void				SaveRefineFlags();
		void				LoadRefineFlags();

	protected:
		std::map<std::pair<int, int>, int> m_mapRefineFlags;
#endif

	private:
		void SendDamagePacket(LPCHARACTER pAttacker, int Damage, BYTE DamageFlag);

	private:
		CArena *m_pArena;
		bool m_ArenaObserver;
		int m_nPotionLimit;

	public:
		void SetArena(CArena* pArena) { m_pArena = pArena; }
		void SetArenaObserverMode(bool flag) { m_ArenaObserver = flag; }

		CArena* GetArena() const { return m_pArena; }
		bool GetArenaObserverMode() const { return m_ArenaObserver; }

		void SetPotionLimit(int count) { m_nPotionLimit = count; }
		int GetPotionLimit() const { return m_nPotionLimit; }

	public:
#ifdef ENABLE_SPECIAL_INVENTORY
		bool IsOpenSafebox() const { return ((m_isOpenSafebox ? true : false) || (m_pkOpenSafeboxEvent != NULL ? true : false) || (m_pkOpenMallEvent != NULL ? true : false)); }
#else
		bool IsOpenSafebox() const { return m_isOpenSafebox ? true : false; }
#endif
		void SetOpenSafebox(bool b) { m_isOpenSafebox = b; }

		int GetSafeboxLoadTime() const { return m_iSafeboxLoadTime; }
		void SetSafeboxLoadTime() { m_iSafeboxLoadTime = thecore_pulse(); }

	private:
		bool m_isOpenSafebox;

	public:
		int GetSkillPowerByLevel(int level, bool bMob = false) const;

		int GetRefineTime() const { return m_iRefineTime; }
		void SetRefineTime() { m_iRefineTime = thecore_pulse(); } 
		int m_iRefineTime;

		int GetUseSeedOrMoonBottleTime() const { return m_iSeedTime; }
		void SetUseSeedOrMoonBottleTime() { m_iSeedTime = thecore_pulse(); }
		int m_iSeedTime;

		int GetExchangeTime() const { return m_iExchangeTime; }
		void SetExchangeTime() { m_iExchangeTime = thecore_pulse(); }
		int m_iExchangeTime;

		int m_iMyShopTime;
		int GetMyShopTime() const { return m_iMyShopTime; }
		void SetMyShopTime() { m_iMyShopTime = thecore_pulse(); }

		bool IsHack(bool bSendMsg = true, bool bCheckShopOwner = true, int limittime = g_nPortalLimitTime);

		void Say(const std::string & s);

	public:
		bool ItemProcess_Polymorph(LPITEM item);

		LPITEM*	GetCubeItem() { return m_pointsInstant.pCubeItems; }
		bool IsCubeOpen () const { return (m_pointsInstant.pCubeNpc?true:false); }
		void SetCubeNpc(LPCHARACTER npc) { m_pointsInstant.pCubeNpc = npc; }
		bool CanDoCube() const;

	private:
		int m_deposit_pulse;

	public:
		void UpdateDepositPulse();
		bool CanDeposit() const;

	private:
		void __OpenPrivateShop();

	public:
		struct AttackedLog
		{
			DWORD dwPID;
			DWORD dwAttackedTime;

			AttackedLog() : dwPID(0), dwAttackedTime(0) {}
		};

		AttackLog m_kAttackLog;
		AttackedLog m_AttackedLog;
		int m_speed_hack_count;

	private :
		std::string m_strNewName;

	public :
		const std::string GetNewName() const { return this->m_strNewName; }
		void SetNewName(const std::string name) { this->m_strNewName = name; }

	public :
		void GoHome();

	private :
		std::set<DWORD>	m_known_guild;

	public :
		void SendGuildName(CGuild* pGuild);
		void SendGuildName(DWORD dwGuildID);

	private :
		DWORD m_dwLogOffInterval;

	public :
		DWORD GetLogOffInterval() const { return m_dwLogOffInterval; }

#ifdef ENABLE_FISH_EVENT
	private:
		DWORD m_dwFishUseCount;
		BYTE m_bFishAttachedShape;
	public:
		DWORD GetFishEventUseCount() const { return m_dwFishUseCount; }
		void FishEventIncreaseUseCount() { m_dwFishUseCount++; }
		
		BYTE GetFishAttachedShape() const { return m_bFishAttachedShape; }
		void SetFishAttachedShape(BYTE bShape) { m_bFishAttachedShape = bShape; }
#endif

	public:
		bool UnEquipSpecialRideUniqueItem ();

		bool CanWarp () const;

	protected:
		int LastCampFireUse; //@fixme502

	private:
		DWORD m_dwLastGoldDropTime;

	public:
		void AutoRecoveryItemProcess (const EAffectTypes);

	public:
		void BuffOnAttr_AddBuffsFromItem(LPITEM pItem);
		void BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem);

	private:
		void BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue);
		void BuffOnAttr_ClearAll();

		typedef std::map <BYTE, CBuffOnAttributes*> TMapBuffOnAttrs;
		TMapBuffOnAttrs m_map_buff_on_attrs;

	public:
		void SetArmada() { cannot_dead = true; }
		void ResetArmada() { cannot_dead = false; }

	private:
		bool cannot_dead;

#ifdef ENABLE_MOUNT_SYSTEM
	private:
		bool m_bIsMount;

	public:
		void SetMount() { m_bIsMount = true; }
		bool IsMount() { return m_bIsMount; }
#endif

#ifdef ENABLE_PET_SYSTEM
	private:
		bool m_bIsPet;

	public:
		void SetPet() { m_bIsPet = true; }
		bool IsPet() { return m_bIsPet; }
#endif

	private:
		float m_fAttMul;
		float m_fDamMul;

	public:
		float GetAttMul() { return this->m_fAttMul; }
		void SetAttMul(float newAttMul) {this->m_fAttMul = newAttMul; }
		float GetDamMul() { return this->m_fDamMul; }
		void SetDamMul(float newDamMul) {this->m_fDamMul = newDamMul; }

	private:
		bool IsValidItemPosition(TItemPos Pos) const;

	private:
		unsigned int itemAward_vnum;
		char itemAward_cmd[20];

	public:
		unsigned int GetItemAward_vnum() { return itemAward_vnum; }
		char* GetItemAward_cmd() { return itemAward_cmd; }

		void SetItemAward_vnum(unsigned int vnum) { itemAward_vnum = vnum; }
		void SetItemAward_cmd(char* cmd) { strcpy(itemAward_cmd,cmd); }

		//@fixme503
		int waitHackCounter;
		void ClearWaitHackCounter(void) { waitHackCounter = 0; }
		void SetWaitHackCounter(void) { waitHackCounter = 1; }
		int GetWaitHackCounter(void) const { return waitHackCounter; }
		//@end_fixme503

	public:
		void DragonSoul_Initialize();

		bool DragonSoul_IsQualified() const;
		void DragonSoul_GiveQualification();

		int DragonSoul_GetActiveDeck() const;
		bool DragonSoul_IsDeckActivated() const;
		bool DragonSoul_ActivateDeck(int deck_idx);

		void DragonSoul_DeactivateAll();
		void DragonSoul_CleanUp();

	public:
		bool DragonSoul_RefineWindow_Open(LPENTITY pEntity);
		bool DragonSoul_RefineWindow_Close();
		LPENTITY DragonSoul_RefineWindow_GetOpener() { return  m_pointsInstant.m_pDragonSoulRefineWindowOpener; }
		bool DragonSoul_RefineWindow_CanRefine();

	private:
		timeval m_tvLastSyncTime;
		int m_iSyncHackCount;

	public:
		void SetLastSyncTime(const timeval &tv) { memcpy(&m_tvLastSyncTime, &tv, sizeof(timeval)); }
		const timeval& GetLastSyncTime() { return m_tvLastSyncTime; }
		void SetSyncHackCount(int iCount) { m_iSyncHackCount = iCount;}
		int GetSyncHackCount() { return m_iSyncHackCount; }

#ifdef ENABLE_STYLE_ATTRIBUTE_SYSTEM
	public:
		bool	UseItemNewAttribute(TItemPos source_pos, TItemPos target_pos, BYTE* bValues);
#endif

	public:
		int LastDropTime;
		int CountDrops;
		void ClearPMCounter(void) { m_iPMCounter = 0; }
		void IncreasePMCounter(void) { m_iPMCounter++; }
		void SetLastPMPulse(void);
		int GetPMCounter(void) const { return m_iPMCounter; }
		int GetLastPMPulse(void) const { return m_iLastPMPulse; }

	protected:
		int m_iLastPMPulse;
		int m_iPMCounter;

	public:
		int analyze_protect;
		int analyze_protect_count;
		int analyze_protect_other_count;

#ifdef ENABLE_COINS_INVENTORY
		long long GetCoins();
		void SetCoins(long long coins);
		// PERF: PointsPacket her cagrida senkron SELECT atiyordu; sadece gosterim icin 5 sn cache
		long long GetCoinsCached();
	private:
		long long m_llCoinsCache = 0;
		DWORD m_dwCoinsCacheTime = 0;
	public:
#endif

#ifdef ENABLE_ANTI_EXP
		bool GetAntiExp() { return m_pointsInstant.anti_exp; }
		void SetAntiExp(bool flag) { m_pointsInstant.anti_exp = flag; }
#endif

#ifdef ENABLE_TELEPORT_TO_A_FRIEND
		void SetLastWarpRequestTimeNew(CHRONO_steady_clock_point last_warp)
		{
			m_iLastWarpRequestTime = last_warp;
		}

		CHRONO_steady_clock_point GetLastWarpRequestTimeNew()
		{
			return m_iLastWarpRequestTime;
		}

	protected:
		CHRONO_steady_clock_point m_iLastWarpRequestTime;
#endif

#ifdef ENABLE_PENDANT_SYSTEM
	public:
		bool CheckCanAddPendantBonus(LPITEM pkItem);
#endif

#ifdef ENABLE_REAL_TIME_REGEN
	public:
		void		SetRealTimeRegenNum(WORD num)	{ m_wRealTimeRegenNum = num; };
		WORD		GetRealTimeRegenNum() 			{ return m_wRealTimeRegenNum; };

		LPREGEN		GetRegen();

	private:
		WORD		m_wRealTimeRegenNum;
#endif

#ifdef ENABLE_RESP_SYSTEM
	public:
		PIXEL_POSITION GetRegenPos() { return m_posRegen; }

		bool	CanActRespManager() { return g_setRespAllowedMap.find(GetMapIndex()) != g_setRespAllowedMap.end(); }
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	public:
		void SetHideCostumePulse(int iPulse) { m_HideCostumePulse = iPulse; }
		int GetHideCostumePulse() { return m_HideCostumePulse; }

		void SetBodyCostumeHidden(bool hidden, bool pass = false);
		bool IsBodyCostumeHidden() const { return m_bHideBodyCostume; };

		void SetHairCostumeHidden(bool hidden, bool pass = false);
		bool IsHairCostumeHidden() const { return m_bHideHairCostume; };

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		void SetAcceCostumeHidden(bool hidden, bool pass = false);
		bool IsAcceCostumeHidden() const { return m_bHideAcceCostume; };
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		void SetWeaponCostumeHidden(bool hidden, bool pass = false);
		bool IsWeaponCostumeHidden() const { return m_bHideWeaponCostume; };
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
		void SetAuraCostumeHidden(bool hidden, bool pass = false);
		bool IsAuraCostumeHidden() const { return m_bHideAuraCostume; };
#endif

	private:
		int m_HideCostumePulse;
		bool m_bHideBodyCostume;
		bool m_bHideHairCostume;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool m_bHideAcceCostume;
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		bool m_bHideWeaponCostume;
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		bool m_bHideAuraCostume;
#endif
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	protected:
		bool m_bAcceCombination, m_bAcceAbsorption;

	public:
		bool IsAcceOpened(bool bCombination) {return bCombination ? m_bAcceCombination : m_bAcceAbsorption;}
		bool IsAcceOpened() {return (m_bAcceCombination || m_bAcceAbsorption);}
		void OpenAcce(bool bCombination);
		void CloseAcce();
		void ClearAcceMaterials();
		bool CleanAcceAttr(LPITEM pkItem, LPITEM pkTarget);
		std::vector<LPITEM>	GetAcceMaterials();
		const TItemPosEx* GetAcceMaterialsInfo();
		void SetAcceMaterial(int pos, LPITEM ptr);
		bool AcceIsSameGrade(long lGrade);
#ifdef ENABLE_GOLD_LIMIT
		long long GetAcceCombinePrice(long lGrade);
#else
		DWORD GetAcceCombinePrice(long lGrade);
#endif
		void GetAcceCombineResult(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs);
		BYTE CheckEmptyMaterialSlot();
		void AddAcceMaterial(TItemPos tPos, BYTE bPos);
		void RemoveAcceMaterial(BYTE bPos);
		BYTE CanRefineAcceMaterials();
		void RefineAcceMaterials();
#endif

#ifdef ENABLE_MINI_GAME_CATCH_KING
	public:
		void			MiniGameCatchKingSetFieldCards(std::vector<TCatchKingCard> vec) { m_vecCatchKingFieldCards = vec; }
		
		DWORD			MiniGameCatchKingGetScore() const { return dwCatchKingTotalScore; }
		void			MiniGameCatchKingSetScore(DWORD dwScore) { dwCatchKingTotalScore = dwScore; }
		
		DWORD			MiniGameCatchKingGetBetNumber() const { return bCatchKingBetSetNumber; }
		void			MiniGameCatchKingSetBetNumber(BYTE bSetNr) { bCatchKingBetSetNumber = bSetNr; }
		
		BYTE			MiniGameCatchKingGetHandCard() const { return bCatchKingHandCard; }
		void			MiniGameCatchKingSetHandCard(BYTE bKingCard) { bCatchKingHandCard = bKingCard; }
		
		BYTE			MiniGameCatchKingGetHandCardLeft() const { return bCatchKingHandCardLeft; }
		void			MiniGameCatchKingSetHandCardLeft(BYTE bHandCard) { bCatchKingHandCardLeft = bHandCard; }
		
		bool			MiniGameCatchKingGetGameStatus() const { return dwCatchKingGameStatus; }
		void			MiniGameCatchKingSetGameStatus(bool bStatus) { dwCatchKingGameStatus = bStatus; }
		
		std::vector<TCatchKingCard> m_vecCatchKingFieldCards;
	protected:
		BYTE bCatchKingHandCard;
		BYTE bCatchKingHandCardLeft;
		bool dwCatchKingGameStatus;
		
		BYTE bCatchKingBetSetNumber;
		DWORD dwCatchKingTotalScore;
#endif


#ifdef ENABLE_GAYA_SHOP_SYSTEM
	public:
		struct Gem_Shop_Values
		{
			int		value_1;
			int		value_2;
			int		value_3;
			int		value_4;
			int		value_5;
			int		value_6;
			bool operator == (const Gem_Shop_Values& b)
			{
				return (this->value_1 == b.value_1) && (this->value_2 == b.value_2) &&
					(this->value_3 == b.value_3) && (this->value_4 == b.value_4) &&
					(this->value_5 == b.value_5) && (this->value_6 == b.value_6);
			}
		};
	
		struct Gem_Load_Values
		{
			DWORD	items;
			DWORD	gem;
			DWORD	count;
			DWORD	glimmerstone;
			DWORD	gem_expansion;
			DWORD	gem_refresh;
			DWORD	glimmerstone_count;
			DWORD	gem_expansion_count;
			DWORD	gem_refresh_count;
			DWORD	grade_stone;
			DWORD	give_gem;
			DWORD	prob_gem;
			DWORD	cost_gem_yang;
		};
	
		bool CheckItemsFull();
		void UpdateItemsGemMarker0();
		void UpdateItemsGemMarker();
		void InfoGemMarker();
		void ClearGemMarket();
		bool CheckSlotGemMarket(int slot);
		void UpdateSlotGemMarket(int slot);
		void BuyItemsGemMarket(int slot);
		void RefreshItemsGemMarket();
		void CraftGemItems(int slot);
		void MarketGemItems(int slot);
		void RefreshGemItems();
		void LoadGemSystem();
		int GetGemState(const std::string& state) const;
		void SetGemState(const std::string& state, int szValue);
		void StartCheckTimeMarket();
		void StartCheckTimeMarketLogin();
	
	private:
		std::vector<Gem_Shop_Values> info_items;
		std::vector<Gem_Shop_Values> info_slots;
		std::vector<Gem_Load_Values> load_gem_items;
		Gem_Load_Values load_gem_values;
		LPEVENT GemUpdateTime;
#endif

#ifdef ENABLE_MINIGAME_RUMI_EVENT
	public:
		struct S_CARD
		{
			DWORD	type;
			DWORD	value;
		};

		struct CARDS_INFO
		{
			S_CARD	cards_in_hand[MAX_CARDS_IN_HAND];
			S_CARD	cards_in_field[MAX_CARDS_IN_FIELD];
			DWORD	cards_left;
			DWORD	field_points;
			DWORD	points;
		};
		
		void			Cards_open(DWORD safemode);
		void			Cards_clean_list();
		DWORD			GetEmptySpaceInHand();
		void			Cards_pullout();
		void			RandomizeCards();
		bool			CardWasRandomized(DWORD type, DWORD value);
		void			SendUpdatedInformations();
		void			SendReward();
		void			CardsDestroy(DWORD reject_index);
		void			CardsAccept(DWORD accept_index);
		void			CardsRestore(DWORD restore_index);
		DWORD			GetEmptySpaceInField();
		DWORD			GetAllCardsCount();
		bool			TypesAreSame();
		bool			ValuesAreSame();
		bool			CardsMatch();
		DWORD			GetLowestCard();
		bool			CheckReward();
		void			CheckCards();
		void			RestoreField();
		void			ResetField();
		void			CardsEnd();
		void			GetGlobalRank(char * buffer, size_t buflen);
		void			GetRundRank(char * buffer, size_t buflen);
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
		void			GetGlobalRankRoulette(char * buffer, size_t buflen);
#endif
	protected:
		CARDS_INFO	character_cards;
		S_CARD		randomized_cards[24];
#endif


#ifdef ENABLE_BOT_PLAYER
	public:
		void SetBotCharacter(bool isBotCharacter) { m_isBotCharacter = isBotCharacter; }
		bool IsBotCharacter() const { return m_isBotCharacter; }

		BYTE GetBotPlayerComboIndex() const;

		void SetBotVictimOwner(uint32_t botID) { m_botVictimID = botID; }
		bool IsBotVictimOwner(uint32_t botID) const { return m_botVictimID == botID; }

	private:
		bool m_isBotCharacter;
		uint32_t m_botVictimID;
#endif

#ifdef ENABLE_RENEWAL_AFFECT
	public:
		int GetAffectType(LPITEM item);
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
	public:
		bool GetRewardStatus() { return m_bmultiFarmStatus; }
		void SetRewardStatus(bool bValue) { m_bmultiFarmStatus = bValue; }

	protected:
		bool m_bmultiFarmStatus;
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	public:
		void SetOfflineShop(LPOFFLINESHOP pkOfflineShop){m_pkOfflineShop=pkOfflineShop;}
		LPOFFLINESHOP GetOfflineShop() { return m_pkOfflineShop; }

		bool IsOfflineShopNPC() { return (IsNPC() && GetOfflineShop()); }

		bool GetOfflineShopPanel() {return isOfflineShopPanelOpen;}
		void SetOfflineShopPanel(bool flag) {isOfflineShopPanelOpen= flag;}

		unsigned long long GetOfflineShopFlag() { return m_pkOfflineShopFlag; }
		void SetOfflineShopFlag(unsigned long long ptr) { m_pkOfflineShopFlag = ptr; }

		void SetProtectTime(const std::string& flagname, int value);
		int GetProtectTime(const std::string& flagname) const;

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
		bool IsLookingSearchItem(DWORD itemID);
		void SetLookingSearch(const std::vector<const OFFLINE_SHOP_ITEM*>& dataVector);
#endif

		bool CheckWindows(bool bChat);
		bool CanAddItemShop();
		bool CanDestroyShop();
		bool CanOpenOfflineShop();
		bool CanOpenShopPanel();
		bool CanRemoveItemShop();
		bool CanCreateShop();
		bool CanRemoveLogShop();
		bool CanWithdrawMoney();
		bool CanChangeTitle();
		bool CanChangeDecoration();
		bool CanBuyItemOfflineShop();
		bool CanGetBackItems();
		bool CanAddTimeShop();

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
		bool CanSearch();
#endif

	protected:
		LPOFFLINESHOP m_pkOfflineShop;
		bool isOfflineShopPanelOpen;
		unsigned long long m_pkOfflineShopFlag;
		std::map<std::string, int> m_protection_Time;
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
		std::vector<DWORD> m_vecSearchLooking;
#endif
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	public:
		DWORD GetPickUPMode() const { return dwPickUPMode; }
		void SetPickUPMode(DWORD dwMode) { dwPickUPMode = dwMode; }

		void ChangePickUPMode(DWORD dwMode);
		bool CheckItemCanGet(const LPITEM item);

	private:
		DWORD dwPickUPMode;
#endif

#ifdef ENABLE_OFFLINE_MESSAGE
	protected:
		DWORD dwLastOfflinePMTime;

	public:
		DWORD GetLastOfflinePMTime() const { return dwLastOfflinePMTime; }
		void SetLastOfflinePMTime() { dwLastOfflinePMTime = get_dword_time(); }
		void SendOfflineMessage(const char* To, const char* Message);
		void ReadOfflineMessages();
#endif

#ifdef ENABLE_RENEWAL_SKILL_SELECT
	public:
		void RenewalSkillSelect();
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	private:
		BYTE m_bAuraRefineWindowType;
		bool m_bAuraRefineWindowOpen;
#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
		bool m_bItemCombinationOpen;
#endif
		TItemPos m_pAuraRefineWindowItemSlot[AURA_SLOT_MAX];
		TAuraRefineInfo m_bAuraRefineInfo[AURA_REFINE_INFO_SLOT_MAX];

	protected:
		BYTE __GetAuraAbsorptionRate(BYTE bLevel, BYTE bBoostIndex) const;
		TAuraRefineInfo __GetAuraRefineInfo(TItemPos Cell);
		TAuraRefineInfo __CalcAuraRefineInfo(TItemPos Cell, TItemPos MaterialCell);
		TAuraRefineInfo __GetAuraEvolvedRefineInfo(TItemPos Cell);

	public:
		void OpenAuraRefineWindow(LPENTITY pOpener, EAuraWindowType type);
		bool IsAuraRefineWindowOpen() const { return m_bAuraRefineWindowOpen; }
#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
		bool IsItemCombinationOpen() const { return m_bItemCombinationOpen; }
		void SetItemCombinationOpen(bool bOpen) { m_bItemCombinationOpen = bOpen; }
#endif
		BYTE GetAuraRefineWindowType() const { return m_bAuraRefineWindowType; }
		LPENTITY GetAuraRefineWindowOpener() { return m_pointsInstant.m_pAuraRefineWindowOpener; }

		bool IsAuraRefineWindowCanRefine();

		void AuraRefineWindowCheckIn(BYTE bAuraRefineWindowType, TItemPos AuraCell, TItemPos ItemCell);
		void AuraRefineWindowCheckOut(BYTE bAuraRefineWindowType, TItemPos AuraCell);
		void AuraRefineWindowAccept(BYTE bAuraRefineWindowType);
		void AuraRefineWindowClose();
#endif

#ifdef ENABLE_CONQUEROR_LEVEL
	public:
		bool IsConquerorMap(int iMapIndex);
#endif

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	public:
		DWORD GetUnlockSlotsW(DWORD type = 0) const { return inventory_unlock[type];}
		void SetUnlockSlotsW(DWORD slots, DWORD type) {inventory_unlock[type] = slots;}

	private:
		DWORD inventory_unlock[UNLOCK_INVENTORY_MAX];
#endif

#ifdef ENABLE_RENEWAL_TELEPORT_SYSTEM
	public:
		void StartWarpOn(BYTE map_index);
#endif

#ifdef ENABLE_QUEEN_NETHIS
	public:
		int ComputeSnakeSkill(uint32_t dwVnum, LPCHARACTER pkVictim, uint8_t bSkillLevel);
		void ClearSnakeSkillEvent() { m_pkSnakeSkillEvent = nullptr; } // event bittiginde holder sifirlanir
	private:
		LPEVENT m_pkSnakeSkillEvent;
#endif

#ifdef ENABLE_COLLECT_WINDOW
	public:
		void SendCollectPacket(const BYTE windowType, const DWORD time, const DWORD count, const DWORD itemVnum, const DWORD countTotal, const BYTE chance, const DWORD bRenderTargetID, const DWORD bQuestIndex, const DWORD reqLevel);
#endif

#ifdef ENABLE_SUNG_MAHI_TOWER
	protected:
		bool m_bIsUniqueMaster;
		
	public:
		void SetUniqueMaster(bool bInfo) { m_bIsUniqueMaster = bInfo; }
		bool GetUniqueMaster() const { return m_bIsUniqueMaster; }
		void SetDungeonMultipliers(BYTE dungeonLevel);
		bool IsSungMahiDungeon(int mapIdx) const;
#endif

#ifdef ENABLE_ZODIAC_MISSION
	public:
		void				BeadCount(LPCHARACTER ch);
#endif

#ifdef ENABLE_MELEY_LAIR_DUNGEON
	public:
		bool			IsMeley();
		void			SetQuestNPCIDAttack(DWORD vid){ m_dwQuestNPCVIDAttack = vid;}
		DWORD			GetQuestNPCIDAttack() const { return m_dwQuestNPCVIDAttack; }
		LPCHARACTER		GetQuestNPCAttack() const;
	private:
		DWORD			m_dwQuestNPCVIDAttack;
#endif

#ifdef ENABLE_TRACK_WINDOW
	public:
		void	GetDungeonCooldown(WORD bossIndex);
		void	GetDungeonCooldownTest(WORD bossIndex, int value, bool isCooldown);
#endif

#ifdef ENABLE_DUNGEON_INFO
	public:
		void 	SetDamageDoneDungeonInfo(int dam){damage_done_dungeon_info = dam;}
		int 	GetDamageDoneDungeonInfo(){return damage_done_dungeon_info;}


		void 	SetDamageReceivedDungeonInfo(int dam){damage_received_dungeon_info = dam;}
		int 	GetDamageReceivedDungeonInfo(){return damage_received_dungeon_info;}

	private:
		int damage_done_dungeon_info;
		int damage_received_dungeon_info;
#endif

#ifdef __DUNGEON_INFO__
	public:
		void			SendDungeonCooldown(DWORD bossIdx);
		int				GetQuestFlagSpecial(const char* szQuestFlag, ...);
#endif

#ifdef ENABLE_ANTI_EQUIP_FLOOD
	private:
		int m_dwEquipAntiFloodPulse;
		uint32_t m_dwEquipAntiFloodCount;

	public:
		int GetEquipAntiFloodPulse() { return m_dwEquipAntiFloodPulse; }
		uint32_t GetEquipAntiFloodCount() { return m_dwEquipAntiFloodCount; }
		uint32_t IncreaseEquipAntiFloodCount() { return ++m_dwEquipAntiFloodCount; }
		void SetEquipAntiFloodPulse(int dwPulse) { m_dwEquipAntiFloodPulse = dwPulse; }
		void SetEquipAntiFloodCount(uint32_t dwCount) { m_dwEquipAntiFloodCount = dwCount; }
#endif

#ifdef ENABLE_GUILD_TOKEN_AUTH
	public:
		bool IsGuildMaster() const;
		uint64_t GetGuildToken() const;
		void SendGuildToken();
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	public:
		void SetTransmutation(CTransmutation* c);
		CTransmutation* GetTransmutation() const;

	protected:
		CTransmutation* m_pkTransmutation;
#endif

#ifdef ENABLE_HUNTING_SYSTEM
	public:
		void CheckHunting(bool isLevelUp = 0);
		void OpenHuntingWindowMain();
		void OpenHuntingWindowSelect();
		void OpenHuntingWindowReward();

		void UpdateHuntingMission(DWORD dwMonsterVnum);
		void ReciveHuntingRewards();
		void SetCachedRewards();

		void SetRewardRandomItemFromTable();
		void SendRandomItemPacket(bool IsSelectWindow);

		int GetRaceTable();
		int GetMinMaxMoney(DWORD missionLevel, bool AskMax);
		int GetRandomMoney(DWORD missionLevel);
		int GetMinMaxExp(DWORD missionLevel, bool AskMax);
		int GetRandomExp(DWORD missionLevel);
#endif

#ifdef ENABLE_RIDING_EXTENDED
	public:
		void SetMountUpGradeExp(uint32_t exp) { m_mount_up_grade_exp = exp; }
		uint32_t GetMountUpGradeExp() const { return m_mount_up_grade_exp; }

		void SetMountUpGradeFail(uint8_t fail) { m_mount_up_grade_fail = fail; }
		uint8_t IsMountUpGradeFail() const { return m_mount_up_grade_fail; }

	protected:
		uint32_t m_mount_up_grade_exp;
		uint8_t m_mount_up_grade_fail;
#endif

#ifdef ENABLE_SPIRIT_STONE_READING
	public:
		// Performans optimizasyonu: GetQuestFlag yerine direkt member variable
		DWORD GetRuhSureFlag() const { return m_dwRuhSureFlag; }
		void SetRuhSureFlag(DWORD value) { m_dwRuhSureFlag = value; }
		DWORD GetRuhYeniSureFlag() const { return m_dwRuhYeniSureFlag; }
		void SetRuhYeniSureFlag(DWORD value) { m_dwRuhYeniSureFlag = value; }

	protected:
		DWORD m_dwRuhSureFlag;
		DWORD m_dwRuhYeniSureFlag;
#endif

#ifdef ENABLE_SKILL_BOOK_READING
	public:
		// Performans optimizasyonu: GetQuestFlag yerine direkt member variable
		DWORD GetBkYeniSureFlag() const { return m_dwBkYeniSureFlag; }
		void SetBkYeniSureFlag(DWORD value) { m_dwBkYeniSureFlag = value; }

	protected:
		DWORD m_dwBkYeniSureFlag;
#endif

#ifdef ENABLE_HORSE_SPAWN_EXPLOIT_FIX
	public:
		// Performans optimizasyonu: GetQuestFlag yerine direkt member variable
		DWORD GetHorseCheckerFlag() const { return m_dwHorseCheckerFlag; }
		void SetHorseCheckerFlag(DWORD value) { m_dwHorseCheckerFlag = value; }

	protected:
		DWORD m_dwHorseCheckerFlag;
#endif

#ifdef ENABLE_ITEMSHOP
	private:
		bool m_bAccountMoneyLoaded;

		long long m_lldAccountCoins;
#ifdef USE_ITEMSHOP_RENEWED
		long long m_lldAccountJCoins;
#endif

	public:
#ifdef USE_ITEMSHOP_RENEWED
		void GetAccountMoney(long long& lldCoins, long long& lldJCoins, bool bReloadForced = false);
		void SetAccountMoney(long long lldCoins, long long lldJCoins, bool bPlus);
#else
		void GetAccountMoney(long long& lldCoins, bool bReloadForced = false);
		void SetAccountMoney(long long lldCoins, bool bPlus);
#endif
		void RefreshAccountMoney();
#endif

#ifdef ENABLE_ITEM_DUPE_FIX
	public:
		int	iWarpTime;
		int	itemPushTime;
#endif

#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
	public:
		void	OpenHalloween();
		void	IncreaseNivel();
#endif

#ifdef ENABLE_BATTLE_PASS
	public:
#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
		int iMonthBattlePass;
#endif
		struct Struct_BattlePass { DWORD	count;	DWORD	status; };
		void	Load_BattlePass();
		void	ExternBattlePass();
		void	DoMission(int index, long long count);
		void	SendInfosBattlePass(int index);
		void	SendDoneBattlePass(int index);
		void	FinalRewardBattlePass();
	
		struct Infos_BattlePass
		{
			DWORD	vnum1;	DWORD	count1;	DWORD	vnum2;
			DWORD	count2;	DWORD	vnum3;	DWORD	count3;
			char	name[4096];
		};
	
		struct Infos_MissionsBP { DWORD	type;	DWORD	vnum;	DWORD	count; DWORD	ep; };
	
		struct Infos_FinalBP
		{
			DWORD	f_vnum1;	DWORD	f_count1;	DWORD	f_vnum2;
			DWORD	f_count2;	DWORD	f_vnum3;	DWORD	f_count3;
		};
	
		struct Infos_Bonus_BattlePass
		{
			DWORD	vnum1;	DWORD	count1;	DWORD	vnum2;
			DWORD	count2;	DWORD	vnum3;	DWORD	count3;
		};
	
		std::vector<Struct_BattlePass> v_counts;
		std::vector<Infos_BattlePass> rewards_bp;
		std::vector<Infos_MissionsBP> missions_bp;
		std::vector<Infos_FinalBP> final_rewards;
		std::vector<Infos_Bonus_BattlePass> rewards_bonus_bp;
	
	public:
#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
		int iMonthBattlePassPremium;
#endif
		struct Struct_BattlePassPremium { DWORD	count;	DWORD	status; };
		void	Load_BattlePassPremium();
		void	ExternBattlePassPremium();
		void	DoMissionPremium(int index, long long count);
		void	SendInfosBattlePassPremium(int index);
		void	SendDoneBattlePassPremium(int index);
		void	FinalRewardBattlePassPremium();
	
		struct Infos_BattlePassPremium
		{
			DWORD	vnum1;	DWORD	count1;	DWORD	vnum2;
			DWORD	count2;	DWORD	vnum3;	DWORD	count3;
			char	name[4096];
		};
	
		struct Infos_MissionsBPPremium { DWORD	type;	DWORD	vnum;	DWORD	count; DWORD	ep; };
	
		struct Infos_FinalBPPremium
		{
			DWORD	f_vnum1;	DWORD	f_count1;	DWORD	f_vnum2;
			DWORD	f_count2;	DWORD	f_vnum3;	DWORD	f_count3;
		};
	
		struct Infos_Bonus_BattlePassPremium
		{
			DWORD	vnum1;	DWORD	count1;	DWORD	vnum2;
			DWORD	count2;	DWORD	vnum3;	DWORD	count3;
		};
	
		std::vector<Struct_BattlePassPremium> v_counts_premium;
		std::vector<Infos_BattlePassPremium> rewards_bp_premium;
		std::vector<Infos_MissionsBPPremium> missions_bp_premium;
		std::vector<Infos_FinalBPPremium> final_rewards_premium;
		std::vector<Infos_Bonus_BattlePassPremium> rewards_bonus_bp_premium;
#endif

#ifdef ENABLE_RANKING
	protected:
		long long m_lRankPoints[RANKING_MAX_CATEGORIES];

	public:
		long long GetRankPoints(int iArg);
		void SetRankPoints(int iArg, long long lPoint);
		void RankingSubcategory(int iArg);
#endif
};

ESex GET_SEX(LPCHARACTER ch);

#endif
