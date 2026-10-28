#ifndef __INC_METIN_II_LENGTH_H__
#define __INC_METIN_II_LENGTH_H__

#include "service.h"

#define WORD_MAX 0xffff

#ifdef ENABLE_ITEMSHOP
enum EItemShopPriceType : uint8_t
{
	ITEMSHOP_PRICE_COIN = 1,
	ITEMSHOP_PRICE_JETON,
	ITEMSHOP_PRICE_MAX,
};
#endif

enum EMisc
{
	MAX_HOST_LENGTH							= 15,
	IP_ADDRESS_LENGTH						= 15,
	LOGIN_MAX_LEN							= 30,
	PASSWD_MAX_LEN							= 16,
	PLAYER_PER_ACCOUNT						= 5,
	ACCOUNT_STATUS_MAX_LEN					= 8,
	CHARACTER_NAME_MAX_LEN					= 48,

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	OFFLINE_SHOP_HOST_ITEM_MAX_NUM			= 80,
	SHOP_SIGN_MAX_LEN						= 45,
#else
	SHOP_SIGN_MAX_LEN						= 32,
#endif

	INVENTORY_PAGE_SIZE						= 45,
	INVENTORY_PAGE_COUNT					= 4,
	INVENTORY_MAX_NUM						= INVENTORY_PAGE_SIZE * INVENTORY_PAGE_COUNT,

	ABILITY_MAX_NUM							= 50,
	EMPIRE_MAX_NUM							= 4,
	BANWORD_MAX_LEN							= 24,
	SOCIAL_ID_MAX_LEN						= 18,
	GUILD_NAME_MAX_LEN						= 12,

	SHOP_HOST_ITEM_MAX_NUM					= 40,
	SHOP_GUEST_ITEM_MAX_NUM 				= 18,
	SHOP_PRICELIST_MAX_NUM					= 40,
#ifdef ENABLE_EMOTICONS_SYSTEM
	CHAT_MAX_LEN							= 1024,
#else
	CHAT_MAX_LEN							= 512,
#endif
	QUICKSLOT_MAX_NUM						= 36,
	JOURNAL_MAX_NUM							= 2,
	QUERY_MAX_LEN							= 8192,
	FILE_MAX_LEN							= 128,

#ifdef ENABLE_LEVEL_INT
	PLAYER_EXP_TABLE_MAX					= 300,
	PLAYER_MAX_LEVEL_CONST					= 300,
#else
	PLAYER_EXP_TABLE_MAX					= 120,
	PLAYER_MAX_LEVEL_CONST					= 250,
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
	PLAYER_MAX_CONQUEROR_LEVEL_CONST		= 30,
#endif

	GUILD_MAX_LEVEL							= 20,
	MOB_MAX_LEVEL							= 100,

	ATTRIBUTE_MAX_VALUE						= 20,
	CHARACTER_PATH_MAX_NUM					= 64,
	SKILL_MAX_NUM							= 255,
	SKILLBOOK_DELAY_MIN						= 64800,
	SKILLBOOK_DELAY_MAX						= 108000,
	SKILL_MAX_LEVEL							= 40,
#ifdef ENABLE_RIDING_EXTENDED
	HORSE_MAX_LEVEL 						= 40,
#endif

	APPLY_NAME_MAX_LEN						= 32,
	EVENT_FLAG_NAME_MAX_LEN					= 32,
	MOB_SKILL_MAX_NUM						= 5,

	POINT_MAX_NUM							= 255,
	DRAGON_SOUL_BOX_SIZE					= 32,
	DRAGON_SOUL_BOX_COLUMN_NUM				= 8,
	DRAGON_SOUL_BOX_ROW_NUM					= DRAGON_SOUL_BOX_SIZE / DRAGON_SOUL_BOX_COLUMN_NUM,
	DRAGON_SOUL_REFINE_GRID_SIZE			= 15,
	MAX_AMOUNT_OF_MALL_BONUS				= 20,
	WEAR_MAX_NUM							= 64,

#ifdef ENABLE_EXTENDED_SAFEBOX
	SAFEBOX_MAX_PAGE_COUNT					= 6,
	SAFEBOX_MAX_NUM							= 45 * SAFEBOX_MAX_PAGE_COUNT,
#endif

#ifdef ENABLE_GOLD_LIMIT
	GOLD_MAX								= 20000000000LL,
#else
	GOLD_MAX								= 2000000000,
#endif

	SHOP_TAB_NAME_MAX						= 32,

#ifdef ENABLE_RENEWAL_SHOPEX
	SHOP_TAB_COUNT_MAX						= 7,
#else
	SHOP_TAB_COUNT_MAX						= 3,
#endif

	BELT_INVENTORY_SLOT_WIDTH				= 7,	// reference belt inventory: 7x7 (was 5x5)
	BELT_INVENTORY_SLOT_HEIGHT				= 7,
	BELT_INVENTORY_SLOT_COUNT				= BELT_INVENTORY_SLOT_WIDTH * BELT_INVENTORY_SLOT_HEIGHT,

#ifdef ENABLE_RANKING
	RANKING_MAX_CATEGORIES 					= 21,
#endif

#ifdef ENABLE_SUNG_MAHI_TOWER
	SUNG_MAHI_DAMANGE_MULTIPLIER = 5,
	SUNG_MAHI_DEFENCE_MULTIPLIER = 5,
	SUNG_MAHI_HP_MULTIPLIER = 250,
	SUNG_MAHI_MAX_LEVEL = 50,
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	SPECIAL_INVENTORY_PAGE_SIZE				= 45,
	SPECIAL_INVENTORY_PAGE_COUNT			= 3,
	SPECIAL_INVENTORY_MAX_NUM				= SPECIAL_INVENTORY_PAGE_SIZE * SPECIAL_INVENTORY_PAGE_COUNT,

	SKILL_BOOK_INVENTORY_MAX_NUM			= SPECIAL_INVENTORY_MAX_NUM,
	UPGRADE_ITEMS_INVENTORY_MAX_NUM			= SPECIAL_INVENTORY_MAX_NUM,
	STONE_INVENTORY_MAX_NUM					= SPECIAL_INVENTORY_MAX_NUM,
	GIFT_BOX_INVENTORY_MAX_NUM				= SPECIAL_INVENTORY_MAX_NUM,
	CHANGERS_INVENTORY_MAX_NUM				= SPECIAL_INVENTORY_MAX_NUM,
#endif

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	UNLOCK_INVENTORY_MAX					= 6,
#endif

#ifdef ENABLE_GAYA_SYSTEM
	GEM_SLOTS_MAX_NUM = 27,
	GEM_REFRESH_ITEM_VNUM = 39063,
	GEM_UNLOCK_ITEM_VNUM = 39064,
	GEM_GLIMMERSTONE_VNUM = 50926,
	GEM_REFRESH_PULSE = 5,
	GEM_MAX = 999999,
#endif

#ifdef ENABLE_RESP_SYSTEM
	RESP_TELEPORT_COOLDOWN = 1,
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	ROULETTE_ITEM_MAX		= 20,
	SOUL_MAX = 999999,
	SOUL_RE_MAX = 999999,
#endif

#ifdef ENABLE_FISH_EVENT
	FISH_EVENT_SLOTS_NUM 		= 24,
	ITEM_FISH_EVENT_BOX			= 25106,
	ITEM_FISH_EVENT_BOX_SPECIAL	= 25107,
#endif
};

#ifdef ENABLE_RENEWAL_SWITCHBOT
enum SwitchbotValues
{
	SWITCHBOT_SLOT_COUNT = 5,
	SWITCHBOT_ALTERNATIVE_COUNT = 2,
	//1 = De�i�tirme ��esi
	//2 = Yang
	SWITCHBOT_PRICE_TYPE = 1,
	//Yang Fiyat� ile Takas Edilen Miktar
	SWITCHBOT_PRICE_AMOUNT = 1,
};

const DWORD dwSwitchingCostumeItem = 90025;

const DWORD c_arSwitchingLowItems[2] =
{
	76023,
	71151,
};
const DWORD c_arSwitchingItems[3] =
{
	76014,
	39028,
	71084,
};
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
enum EGrowthPet
{
	PET_MAX_LEVEL = 105,

	PET_ATTR_CHANGE_ITEMVNUM = 55033,
	PET_NAME_MAX_SIZE = 20,
	PET_NAME_MIN_SIZE = 4,
	PET_HATCHING_MONEY = 100000,
	PET_SKILL_COUNT_MAX = 3,
	PET_GROWTH_SKILL_LEVEL_MAX = 20,
	PET_SKILL_UPGRADE_PRICE = 2000000,
	PET_GROWTH_SKILL_OPEN_EVOL_LEVEL = 4,
	PET_FEED_SLOT_MAX = 9,
	PET_REVIVE_MATERIAL_SLOT_MAX = 10,

	EGG_TO_UPBRINGING_DELTA = 300,
	MELEY_PET_EGG_VNUM = 55406,

	EXP_TYPE_MOB = 1,
	EXP_TYPE_ITEM = 2,

	PET_EVOL_MAX_ITEM_COUNT = 7,
	PET_LAST_EVOL_MIN_DAY_COUNT = 30,

	PET_MONKEY = 55701,
	PET_SPIDER = 55702,
	PET_RAZADOR = 55703,
	PET_NEMERE = 55704,
	PET_BLUE_DRAGON = 55705,
	PET_RED_DRAGON = 55706,
	PET_AZRAEL = 55707,
	PET_MINI_EXECUTOR = 55708,
	PET_BASHIIDO = 55709,
	PET_NESSIE = 55710,
	PET_EXEDYAR = 55711,
	PET_MAX_NUM = 11,

	PET_MAX_BONUS_NUM = 8,
};

enum EPetState
{
	STATE_NONE,
	STATE_UPBRINGING,
	STATE_BAG,
	STATE_SAFEBOX,
};
#endif

#ifdef ENABLE_FISH_EVENT
enum EFishEventInfo
{
	FISH_EVENT_SHAPE_NONE,
	FISH_EVENT_SHAPE_1,
	FISH_EVENT_SHAPE_2,
	FISH_EVENT_SHAPE_3,
	FISH_EVENT_SHAPE_4,
	FISH_EVENT_SHAPE_5,
	FISH_EVENT_SHAPE_6,
	FISH_EVENT_SHAPE_7,
	FISH_EVENT_SHAPE_MAX_NUM,
};
#endif

enum EWearPositions
{
	WEAR_BODY,
	WEAR_HEAD,
	WEAR_FOOTS,
	WEAR_WRIST,
	WEAR_WEAPON,
	WEAR_NECK,
	WEAR_EAR,
	WEAR_UNIQUE1,
	WEAR_UNIQUE2,
	WEAR_ARROW,
	WEAR_SHIELD,
	WEAR_ABILITY1,
	WEAR_ABILITY2,
	WEAR_ABILITY3,
	WEAR_ABILITY4,
	WEAR_ABILITY5,
	WEAR_ABILITY6,
	WEAR_ABILITY7,
	WEAR_ABILITY8,
	WEAR_COSTUME_BODY,
	WEAR_COSTUME_HAIR,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	WEAR_COSTUME_ACCE,
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	WEAR_COSTUME_WEAPON,
#endif
#ifdef ENABLE_MOUNT_PET_SKIN
	WEAR_COSTUME_MOUNT,
	WEAR_COSTUME_PET,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	WEAR_COSTUME_AURA,
#endif
	WEAR_RING1,
	WEAR_RING2,
	WEAR_BELT,
#ifdef ENABLE_MOUNT_SYSTEM
	WEAR_MOUNT,
#endif
#ifdef ENABLE_PET_SYSTEM
	WEAR_PET,
#endif
#ifdef ENABLE_PENDANT_SYSTEM
	WEAR_PENDANT,
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
	WEAR_PASSIVE,
#endif

	// Reference inventory (Companion / Costume panels). Fixed numbers: the equipped position is stored in the DB.
	WEAR_BUFFI = 33,				// Buffi seal: equipped = summoned
	WEAR_BUFFI_ARMOR = 34,		// Buffi costumes: only change how the Buffi looks, no bonus
	WEAR_BUFFI_WEAPON = 35,
	WEAR_BUFFI_HAIR = 36,
	WEAR_BUFFI_ACCE = 37,
	WEAR_GROWTH_PET = 38,
	WEAR_COSTUME_ACCE_SKIN = 39,
	WEAR_SHINING_WEAPON = 40,
	WEAR_SHINING_ARMOR = 41,

	WEAR_MAX = 64
};

#ifdef ENABLE_FISH_EVENT
enum EDragonSoulDeckType
{
	DRAGON_SOUL_DECK_0,
	DRAGON_SOUL_DECK_1,
	DRAGON_SOUL_DECK_MAX_NUM = 2,

	DRAGON_SOUL_DECK_RESERVED_MAX_NUM = 3,
};
#endif

enum ESex
{
	SEX_MALE,
	SEX_FEMALE
};

enum EDirection
{
	DIR_NORTH,
	DIR_NORTHEAST,
	DIR_EAST,
	DIR_SOUTHEAST,
	DIR_SOUTH,
	DIR_SOUTHWEST,
	DIR_WEST,
	DIR_NORTHWEST,
	DIR_MAX_NUM
};

#define ABILITY_MAX_LEVEL	10

enum EAbilityDifficulty
{
	DIFFICULTY_EASY,
	DIFFICULTY_NORMAL,
	DIFFICULTY_HARD,
	DIFFICULTY_VERY_HARD,
	DIFFICULTY_NUM_TYPES
};

enum EAbilityCategory
{
	CATEGORY_PHYSICAL,
	CATEGORY_MENTAL,
	CATEGORY_ATTRIBUTE,
	CATEGORY_NUM_TYPES
};

enum EJobs
{
	JOB_WARRIOR,
	JOB_ASSASSIN,
	JOB_SURA,
	JOB_SHAMAN,
	JOB_MAX_NUM
};

enum ESkillGroups
{
	SKILL_GROUP_MAX_NUM = 2,
};

enum ERaceFlags
{
	RACE_FLAG_ANIMAL	= (1 << 0),
	RACE_FLAG_UNDEAD	= (1 << 1),
	RACE_FLAG_DEVIL		= (1 << 2),
	RACE_FLAG_HUMAN		= (1 << 3),
	RACE_FLAG_ORC		= (1 << 4),
	RACE_FLAG_MILGYO	= (1 << 5),
	RACE_FLAG_INSECT	= (1 << 6),
	RACE_FLAG_FIRE		= (1 << 7),
	RACE_FLAG_ICE		= (1 << 8),
	RACE_FLAG_DESERT	= (1 << 9),
	RACE_FLAG_TREE		= (1 << 10),
	RACE_FLAG_ATT_ELEC	= (1 << 11),
	RACE_FLAG_ATT_FIRE	= (1 << 12),
	RACE_FLAG_ATT_ICE	= (1 << 13),
	RACE_FLAG_ATT_WIND	= (1 << 14),
	RACE_FLAG_ATT_EARTH	= (1 << 15),
	RACE_FLAG_ATT_DARK	= (1 << 16),
	RACE_FLAG_CZ		= (1 << 17), // 1<<11 official
};

enum ELoads
{
	LOAD_NONE,
	LOAD_LIGHT,
	LOAD_NORMAL,
	LOAD_HEAVY,
	LOAD_MASSIVE
};

enum
{
	QUICKSLOT_TYPE_NONE,
	QUICKSLOT_TYPE_ITEM,
	QUICKSLOT_TYPE_SKILL,
	QUICKSLOT_TYPE_COMMAND,
	QUICKSLOT_TYPE_MAX_NUM,
};

enum EParts
{
	PART_MAIN,
	PART_WEAPON,
	PART_HEAD,
	PART_HAIR,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PART_ACCE,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	PART_AURA,
#endif
	PART_MAX_NUM,
	PART_WEAPON_SUB,
};

enum EChatType
{
	CHAT_TYPE_TALKING,
	CHAT_TYPE_INFO,
	CHAT_TYPE_NOTICE,
	CHAT_TYPE_PARTY,
	CHAT_TYPE_GUILD,
	CHAT_TYPE_COMMAND,
	CHAT_TYPE_SHOUT,
	CHAT_TYPE_WHISPER,
	CHAT_TYPE_BIG_NOTICE,
#ifdef ENABLE_DICE_SYSTEM
	CHAT_TYPE_DICE_INFO,
#endif
#ifdef ENABLE_RENEWAL_OX_EVENT
	CHAT_TYPE_CONTROL_NOTICE,
#endif
#ifdef ENABLE_NEW_DUNGEON_LIB
	CHAT_TYPE_MISSION,
#endif
	CHAT_TYPE_MAX_NUM
};

enum EWhisperType
{
	WHISPER_TYPE_NORMAL				= 0,
	WHISPER_TYPE_NOT_EXIST			= 1,
	WHISPER_TYPE_TARGET_BLOCKED		= 2,
	WHISPER_TYPE_SENDER_BLOCKED		= 3,
	WHISPER_TYPE_ERROR				= 4,
	WHISPER_TYPE_GM					= 5,
#ifdef ENABLE_OFFLINE_MESSAGE
	WHISPER_TYPE_OFFLINE			= 6,
#endif
	WHISPER_TYPE_SYSTEM				= 0xFF
};

enum ECharacterPosition
{
	POSITION_GENERAL,
	POSITION_BATTLE,
	POSITION_DYING,
	POSITION_SITTING_CHAIR,
	POSITION_SITTING_GROUND,
	POSITION_INTRO,
	POSITION_MAX_NUM
};

enum EGMLevels
{
	GM_PLAYER,
	GM_LOW_WIZARD,
	GM_WIZARD,
	GM_HIGH_WIZARD,
	GM_GOD,
	GM_IMPLEMENTOR
};

enum EMobRank
{
	MOB_RANK_PAWN,
	MOB_RANK_S_PAWN,
	MOB_RANK_KNIGHT,
	MOB_RANK_S_KNIGHT,
	MOB_RANK_BOSS,
	MOB_RANK_KING,
	MOB_RANK_MAX_NUM
};

enum ECharType
{
	CHAR_TYPE_MONSTER,
	CHAR_TYPE_NPC,
	CHAR_TYPE_STONE,
	CHAR_TYPE_WARP,
	CHAR_TYPE_DOOR,
	CHAR_TYPE_BUILDING,
	CHAR_TYPE_PC,
	CHAR_TYPE_POLYMORPH_PC,
	CHAR_TYPE_HORSE,
	CHAR_TYPE_GOTO
#ifdef ENABLE_PET_SYSTEM
	,CHAR_TYPE_PET
#endif
#ifdef ENABLE_MOUNT_SYSTEM
	,CHAR_TYPE_MOUNT
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	,CHAR_TYPE_GROWTH_PET
#endif
};

enum EBattleType
{
	BATTLE_TYPE_MELEE,
	BATTLE_TYPE_RANGE,
	BATTLE_TYPE_MAGIC,
	BATTLE_TYPE_SPECIAL,
	BATTLE_TYPE_POWER,
	BATTLE_TYPE_TANKER,
	BATTLE_TYPE_SUPER_POWER,
	BATTLE_TYPE_SUPER_TANKER,
	BATTLE_TYPE_MAX_NUM
};

enum EApplyTypes
{
	APPLY_NONE                                              = 0,
	APPLY_MAX_HP                                            = 1,
	APPLY_MAX_SP                                            = 2,
	APPLY_CON                                               = 3,
	APPLY_INT                                               = 4,
	APPLY_STR                                               = 5,
	APPLY_DEX                                               = 6,
	APPLY_ATT_SPEED                                         = 7,
	APPLY_MOV_SPEED                                         = 8,
	APPLY_CAST_SPEED                                        = 9,
	APPLY_HP_REGEN                                          = 10,
	APPLY_SP_REGEN                                          = 11,
	APPLY_POISON_PCT                                        = 12,
	APPLY_STUN_PCT                                          = 13,
	APPLY_SLOW_PCT                                          = 14,
	APPLY_CRITICAL_PCT                                      = 15,
	APPLY_PENETRATE_PCT                                     = 16,
	APPLY_ATTBONUS_HUMAN                                    = 17,
	APPLY_ATTBONUS_ANIMAL                                   = 18,
	APPLY_ATTBONUS_ORC                                      = 19,
	APPLY_ATTBONUS_MILGYO                                   = 20,
	APPLY_ATTBONUS_UNDEAD                                   = 21,
	APPLY_ATTBONUS_DEVIL                                    = 22,
	APPLY_STEAL_HP                                          = 23,
	APPLY_STEAL_SP                                          = 24,
	APPLY_MANA_BURN_PCT                                     = 25,
	APPLY_DAMAGE_SP_RECOVER                                 = 26,
	APPLY_BLOCK                                             = 27,
	APPLY_DODGE                                             = 28,
	APPLY_RESIST_SWORD                                      = 29,
	APPLY_RESIST_TWOHAND                                    = 30,
	APPLY_RESIST_DAGGER                                     = 31,
	APPLY_RESIST_BELL                                       = 32,
	APPLY_RESIST_FAN                                        = 33,
	APPLY_RESIST_BOW                                        = 34,
	APPLY_RESIST_FIRE                                       = 35,
	APPLY_RESIST_ELEC                                       = 36,
	APPLY_RESIST_MAGIC                                      = 37,
	APPLY_RESIST_WIND                                       = 38,
	APPLY_REFLECT_MELEE                                     = 39,
	APPLY_REFLECT_CURSE                                     = 40,
	APPLY_POISON_REDUCE                                     = 41,
	APPLY_KILL_SP_RECOVER                                   = 42,
	APPLY_EXP_DOUBLE_BONUS                                  = 43,
	APPLY_GOLD_DOUBLE_BONUS                                 = 44,
	APPLY_ITEM_DROP_BONUS                                   = 45,
	APPLY_POTION_BONUS                                      = 46,
	APPLY_KILL_HP_RECOVER                                   = 47,
	APPLY_IMMUNE_STUN                                       = 48,
	APPLY_IMMUNE_SLOW                                       = 49,
	APPLY_IMMUNE_FALL                                       = 50,
	APPLY_SKILL                                             = 51,
	APPLY_BOW_DISTANCE                                      = 52,
	APPLY_ATT_GRADE_BONUS                                   = 53,
	APPLY_DEF_GRADE_BONUS                                   = 54,
	APPLY_MAGIC_ATT_GRADE                                   = 55,
	APPLY_MAGIC_DEF_GRADE                                   = 56,
	APPLY_CURSE_PCT                                         = 57,
	APPLY_MAX_STAMINA                                       = 58,
	APPLY_ATTBONUS_WARRIOR                                  = 59,
	APPLY_ATTBONUS_ASSASSIN                                 = 60,
	APPLY_ATTBONUS_SURA                                     = 61,
	APPLY_ATTBONUS_SHAMAN                                   = 62,
	APPLY_ATTBONUS_MONSTER                                  = 63,
	APPLY_MALL_ATTBONUS                                     = 64,
	APPLY_MALL_DEFBONUS                                     = 65,
	APPLY_MALL_EXPBONUS                                     = 66,
	APPLY_MALL_ITEMBONUS                                    = 67,
	APPLY_MALL_GOLDBONUS                                    = 68,
	APPLY_MAX_HP_PCT                                        = 69,
	APPLY_MAX_SP_PCT                                        = 70,
	APPLY_SKILL_DAMAGE_BONUS                                = 71,
	APPLY_NORMAL_HIT_DAMAGE_BONUS                           = 72,
	APPLY_SKILL_DEFEND_BONUS                                = 73,
	APPLY_NORMAL_HIT_DEFEND_BONUS                           = 74,
	APPLY_EXTRACT_HP_PCT                                    = 75,
	APPLY_PC_BANG_EXP_BONUS                                 = 76,
	APPLY_PC_BANG_DROP_BONUS                                = 77,
	APPLY_RESIST_WARRIOR                                    = 78,
	APPLY_RESIST_ASSASSIN                                   = 79,
	APPLY_RESIST_SURA                                       = 80,
	APPLY_RESIST_SHAMAN                                     = 81,
	APPLY_ENERGY                                            = 82,
	APPLY_DEF_GRADE                                         = 83,
	APPLY_COSTUME_ATTR_BONUS                                = 84,
	APPLY_MAGIC_ATTBONUS_PER                                = 85,
	APPLY_MELEE_MAGIC_ATTBONUS_PER                          = 86,
	APPLY_RESIST_ICE                                        = 87,
	APPLY_RESIST_EARTH                                      = 88,
	APPLY_RESIST_DARK                                       = 89,
	APPLY_ANTI_CRITICAL_PCT                                 = 90,
	APPLY_ANTI_PENETRATE_PCT                                = 91,
	APPLY_BLEEDING_REDUCE                                   = 92,
	APPLY_BLEEDING_PCT                                      = 93,
	APPLY_ATTBONUS_WOLFMAN                                  = 94,
	APPLY_RESIST_WOLFMAN                                    = 95,
	APPLY_RESIST_CLAW                                       = 96,
	APPLY_ACCEDRAIN_RATE                                    = 97,
	APPLY_RESIST_MAGIC_REDUCTION                            = 98,
	APPLY_ENCHANT_ELECT                                     = 99,
	APPLY_ENCHANT_FIRE                                      = 100,
	APPLY_ENCHANT_ICE                                       = 101,
	APPLY_ENCHANT_WIND                                      = 102,
	APPLY_ENCHANT_EARTH                                     = 103,
	APPLY_ENCHANT_DARK                                      = 104,
	APPLY_ATTBONUS_CZ                                       = 105,
	APPLY_ATTBONUS_INSECT                                   = 106,
	APPLY_ATTBONUS_DESERT                                   = 107,
	APPLY_ATTBONUS_SWORD                                    = 108,
	APPLY_ATTBONUS_TWOHAND                                  = 109,
	APPLY_ATTBONUS_DAGGER                                   = 110,
	APPLY_ATTBONUS_BELL                                     = 111,
	APPLY_ATTBONUS_FAN                                      = 112,
	APPLY_ATTBONUS_BOW                                      = 113,
	APPLY_ATTBONUS_CLAW                                     = 114,
	APPLY_RESIST_HUMAN                                      = 115,
	APPLY_RESIST_MOUNT_FALL                                 = 116,
	UNK17                                                   = 117,
	APPLY_MOUNT                                             = 118,
	APPLY_SKILL_DAMAGE_SAMYEON                              = 119,
	APPLY_SKILL_DAMAGE_TANHWAN                              = 120,
	APPLY_SKILL_DAMAGE_PALBANG                              = 121,
	APPLY_SKILL_DAMAGE_GIGONGCHAM                           = 122,
	APPLY_SKILL_DAMAGE_GYOKSAN                              = 123,
	APPLY_SKILL_DAMAGE_GEOMPUNG                             = 124,
	APPLY_SKILL_DAMAGE_AMSEOP                               = 125,
	APPLY_SKILL_DAMAGE_GUNGSIN                              = 126,
	APPLY_SKILL_DAMAGE_CHARYUN                              = 127,
	APPLY_SKILL_DAMAGE_SANGONG                              = 128,
	APPLY_SKILL_DAMAGE_YEONSA                               = 129,
	APPLY_SKILL_DAMAGE_KWANKYEOK                            = 130,
	APPLY_SKILL_DAMAGE_GIGUNG                               = 131,
	APPLY_SKILL_DAMAGE_HWAJO                                = 132,
	APPLY_SKILL_DAMAGE_SWAERYUNG                            = 133,
	APPLY_SKILL_DAMAGE_YONGKWON                             = 134,
	APPLY_SKILL_DAMAGE_PABEOB                               = 135,
	APPLY_SKILL_DAMAGE_MARYUNG                              = 136,
	APPLY_SKILL_DAMAGE_HWAYEOMPOK                           = 137,
	APPLY_SKILL_DAMAGE_MAHWAN                               = 138,
	APPLY_SKILL_DAMAGE_BIPABU                               = 139,
	APPLY_SKILL_DAMAGE_YONGBI                               = 140,
	APPLY_SKILL_DAMAGE_PAERYONG                             = 141,
	APPLY_SKILL_DAMAGE_NOEJEON                              = 142,
	APPLY_SKILL_DAMAGE_BYEURAK                              = 143,
	APPLY_SKILL_DAMAGE_CHAIN                                = 144,
	APPLY_SKILL_DAMAGE_CHAYEOL                              = 145,
	APPLY_SKILL_DAMAGE_SALPOONG                             = 146,
	APPLY_SKILL_DAMAGE_GONGDAB                              = 147,
	APPLY_SKILL_DAMAGE_PASWAE                               = 148,
	APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE              = 149,
	APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE                   = 150,
	APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE              = 151,
	APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE                   = 152,
	APPLY_HIT_BUFF_ENCHANT_FIRE                             = 153,
	APPLY_HIT_BUFF_ENCHANT_ICE                              = 154,
	APPLY_HIT_BUFF_ENCHANT_ELEC                             = 155,
	APPLY_HIT_BUFF_ENCHANT_WIND                             = 156,
	APPLY_HIT_BUFF_ENCHANT_DARK                             = 157,
	APPLY_HIT_BUFF_ENCHANT_EARTH                            = 158,
	APPLY_HIT_BUFF_RESIST_FIRE                              = 159,
	APPLY_HIT_BUFF_RESIST_ICE                               = 160,
	APPLY_HIT_BUFF_RESIST_ELEC                              = 161,
	APPLY_HIT_BUFF_RESIST_WIND                              = 162,
	APPLY_HIT_BUFF_RESIST_DARK                              = 163,
	APPLY_HIT_BUFF_RESIST_EARTH                             = 164,
	APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED                    = 165,
	APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED                = 166,
	APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT                    = 167,
	APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS                 = 168,
	APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS                  = 169,
	APPLY_USE_SKILL_JEOKRANG_DEF_BONUS                      = 170,
	APPLY_USE_SKILL_GWIGEOM_DEF_BONUS                       = 171,
	APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS                  = 172,
	APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS                 = 173,
	APPLY_USE_SKILL_MANASHILED_CASTING_SPEED                = 174,
	APPLY_USE_SKILL_HOSIN_DEF_BONUS                         = 175,
	APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS                 = 176,
	APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS                = 177,
	APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS                    = 178,
	APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS                 = 179,
	APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED                  = 180,
	APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS                 = 181,
	APPLY_SKILL_DURATION_INCREASE_EUNHYUNG                  = 182,
	APPLY_SKILL_DURATION_INCREASE_GYEONGGONG                = 183,
	APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG                 = 184,
	APPLY_SKILL_DURATION_INCREASE_JEOKRANG                  = 185,
	APPLY_USE_SKILL_PALBANG_HP_ABSORB                       = 186,
	APPLY_USE_SKILL_AMSEOP_HP_ABSORB                        = 187,
	APPLY_USE_SKILL_YEONSA_HP_ABSORB                        = 188,
	APPLY_USE_SKILL_YONGBI_HP_ABSORB                        = 189,
	APPLY_USE_SKILL_CHAIN_HP_ABSORB                         = 190,
	APPLY_USE_SKILL_PASWAE_SP_ABSORB                        = 191,
	APPLY_USE_SKILL_GIGONGCHAM_STUN                         = 192,
	APPLY_USE_SKILL_CHARYUN_STUN                            = 193,
	APPLY_USE_SKILL_PABEOB_STUN                             = 194,
	APPLY_USE_SKILL_MAHWAN_STUN                             = 195,
	APPLY_USE_SKILL_GONGDAB_STUN                            = 196,
	APPLY_USE_SKILL_SAMYEON_STUN                            = 197,
	APPLY_USE_SKILL_GYOKSAN_KNOCKBACK                       = 198,
	APPLY_USE_SKILL_SEOMJEON_KNOCKBACK                      = 199,
	APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK                     = 200,
	APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK                    = 201,
	APPLY_USE_SKILL_GONGDAB_KNOCKBACK                       = 202,
	APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK                     = 203,
	APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER    = 204,
	APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER   = 205,
	APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER    = 206,
	APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER  = 207,
	APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER   = 208,
	APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER    = 209,
	APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER     = 210,
	APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER    = 211,
	APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER   = 212,
	APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER     = 213,
	APPLY_ATTBONUS_STONE                                    = 214,
	APPLY_DAMAGE_HP_RECOVERY                                = 215,
	APPLY_DAMAGE_SP_RECOVERY                                = 216,
	APPLY_ALIGNMENT_DAMAGE_BONUS                            = 217,
	APPLY_NORMAL_DAMAGE_GUARD                               = 218,
	APPLY_MORE_THEN_HP90_DAMAGE_REDUCE                      = 219,
	APPLY_USE_SKILL_TUSOK_HP_ABSORB                         = 220,
	APPLY_USE_SKILL_PAERYONG_HP_ABSORB                      = 221,
	APPLY_USE_SKILL_BYEURAK_HP_ABSORB                       = 222,
	APPLY_FIRST_ATTRIBUTE_BONUS                             = 223,
	APPLY_SECOND_ATTRIBUTE_BONUS                            = 224,
	APPLY_THIRD_ATTRIBUTE_BONUS                             = 225,
	APPLY_FOURTH_ATTRIBUTE_BONUS                            = 226,
	APPLY_FIFTH_ATTRIBUTE_BONUS                             = 227,
	APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER    = 228,
	APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER   = 229,
	APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER    = 230,
	APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER  = 231,
	APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER   = 232,
	APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER    = 233,
	APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER     = 234,
	APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER    = 235,
	APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER   = 236,
	APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER     = 237,
	APPLY_USE_SKILL_CHAYEOL_HP_ABSORB                       = 238,
	APPLY_SUNGMA_STR                                        = 239,
	APPLY_SUNGMA_HP                                         = 240,
	APPLY_SUNGMA_MOVE                                       = 241,
	APPLY_SUNGMA_IMMUNE                                     = 242,
	APPLY_HIT_PCT                                           = 243,
	APPLY_RANDOM                                            = 244,
	APPLY_ATTBONUS_PER_HUMAN                                = 245,
	APPLY_ATTBONUS_PER_ANIMAL                               = 246,
	APPLY_ATTBONUS_PER_ORC                                  = 247,
	APPLY_ATTBONUS_PER_MILGYO                               = 248,
	APPLY_ATTBONUS_PER_UNDEAD                               = 249,
	APPLY_ATTBONUS_PER_DEVIL                                = 250,
	APPLY_ENCHANT_PER_ELECT                                 = 251,
	APPLY_ENCHANT_PER_FIRE                                  = 252,
	APPLY_ENCHANT_PER_ICE                                   = 253,
	APPLY_ENCHANT_PER_WIND                                  = 254,
	APPLY_ENCHANT_PER_EARTH                                 = 255,
	APPLY_ENCHANT_PER_DARK                                  = 256,
	APPLY_ATTBONUS_PER_CZ                                   = 257,
	APPLY_ATTBONUS_PER_INSECT                               = 258,
	APPLY_ATTBONUS_PER_DESERT                               = 259,
	APPLY_ATTBONUS_PER_STONE                                = 260,
	APPLY_ATTBONUS_PER_MONSTER                              = 261,
	APPLY_RESIST_PER_HUMAN                                  = 262,
	APPLY_RESIST_PER_ICE                                    = 263,
	APPLY_RESIST_PER_DARK                                   = 264,
	APPLY_RESIST_PER_EARTH                                  = 265,
	APPLY_RESIST_PER_FIRE                                   = 266,
	APPLY_RESIST_PER_ELEC                                   = 267,
	APPLY_RESIST_PER_MAGIC                                  = 268,
	APPLY_RESIST_PER_WIND                                   = 269,
	APPLY_HIT_BUFF_SUNGMA_STR                               = 270,
	APPLY_HIT_BUFF_SUNGMA_MOVE                              = 271,
	APPLY_HIT_BUFF_SUNGMA_HP                                = 272,
	APPLY_HIT_BUFF_SUNGMA_IMMUNE                            = 273,
	APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER                    = 274,
	APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER                     = 275,
	APPLY_HIT_AUTO_HP_RECOVERY                              = 276,
	APPLY_HIT_AUTO_SP_RECOVERY                              = 277,
	APPLY_USE_SKILL_COOLTIME_DECREASE_ALL                   = 278,
	APPLY_HIT_STONE_ATTBONUS_STONE                          = 279,
	APPLY_HIT_STONE_DEF_GRADE_BONUS                         = 280,
	APPLY_KILL_BOSS_ITEM_BONUS                              = 281,
	APPLY_MOB_HIT_MOB_AGGRESSIVE                            = 282,
	APPLY_NO_DEATH_AND_HP_RECOVERY30                        = 283,
	APPLY_AUTO_PICKUP                                       = 284,
	APPLY_MOUNT_NO_KNOCKBACK                                = 285,
	APPLY_SUNGMA_PER_STR                                    = 286,
	APPLY_SUNGMA_PER_HP                                     = 287,
	APPLY_SUNGMA_PER_MOVE                                   = 288,
	APPLY_SUNGMA_PER_IMMUNE                                 = 289,
	APPLY_IMMUNE_POISON100                                  = 290,
	APPLY_IMMUNE_BLEEDING100                                = 291,
	APPLY_MONSTER_DEFEND_BONUS                              = 292,
	APPLY_RESERVED_293                                      = 293,
	APPLY_RESERVED_294                                      = 294,
	APPLY_RESERVED_295                                      = 295,
	APPLY_RESERVED_296                                      = 296,
	APPLY_RESERVED_297                                      = 297,
	APPLY_RESERVED_298                                      = 298,
	APPLY_RESERVED_299                                      = 299,
	APPLY_USE_SKILL_FINISH_NEXT_COOLTIME_DECREASE_PCT       = 300,
	APPLY_USE_SKILL_ILGWANGPYO_NEXT_COOLTIME_DECREASE_PCT   = 301,
	APPLY_USE_SKILL_PUNGLOEPO_NEXT_COOLTIME_DECREASE_PCT    = 302,
	APPLY_USE_SKILL_GEOMAGGWI_NEXT_COOLTIME_DECREASE_PCT    = 303,
	APPLY_USE_SKILL_MABEOBAGGWI_NEXT_COOLTIME_DECREASE_PCT  = 304,
	APPLY_USE_SKILL_METEO_NEXT_COOLTIME_DECREASE_PCT        = 305,
	APPLY_USE_SKILL_CHEONUN_NEXT_COOLTIME_DECREASE_PCT      = 306,
	APPLY_USE_SKILL_ILIPUNGU_NEXT_COOLTIME_DECREASE_PCT     = 307,
	APPLY_GLOVE_BASE_EFFECT_INCREASE                        = 308,
	APPLY_NO_FALLEN                                         = 309,
	APPLY_HIT_AREA_ELEC_ATTACK                              = 310,
	APPLY_BLOCK_PENETRATE_PCT                               = 311,
	APPLY_ATTBONUS_MYSTERY_DUNGEON                          = 312,
	APPLY_ATTBONUS_DRAGON                                   = 313,
	APPLY_RESERVED_314                                      = 314,
	APPLY_RESERVED_315                                      = 315,
	APPLY_RESERVED_316                                      = 316,
	APPLY_RESERVED_317                                      = 317,
	APPLY_RESERVED_318                                      = 318,
	APPLY_RESERVED_319                                      = 319,
	APPLY_RESERVED_320                                      = 320,
	APPLY_RESERVED_321                                      = 321,
	APPLY_ATTBONUS_MOON                                     = 322,
	APPLY_RESERVED_323                                      = 323,
	APPLY_BONUS_BLOCK_PENETRATE_PCT                         = 324,
	APPLY_BONUS_HIT_PCT                                     = 325,
	APPLY_REFUSE_EXP                                        = 326,
	APPLY_RESERVED_327                                      = 327,
	APPLY_RESERVED_328                                      = 328,
	APPLY_RESERVED_329                                      = 329,
	APPLY_ATTBONUS_BOSS                                     = 330,
	APPLY_ATTBONUS_ELEMENTS                                 = 331,
	APPLY_ENCHANT_ELEMENTS                                  = 332,
	APPLY_ATTBONUS_CHARACTERS                               = 333,
	APPLY_ENCHANT_CHARACTERS                                = 334,
	APPLY_RESIST_MONSTER                                    = 335,
	APPLY_ATTBONUS_MEDI_PVM                                 = 336,
	APPLY_ATTBONUS_PVM_STR                                  = 337,
	APPLY_ATTBONUS_PVM_BERSERKER                            = 338,

	MAX_APPLY_NUM = 350,
};


enum EOnClickEvents
{
	ON_CLICK_NONE,
	ON_CLICK_SHOP,
	ON_CLICK_TALK,
	ON_CLICK_MAX_NUM
};

enum EOnIdleEvents
{
	ON_IDLE_NONE,
	ON_IDLE_GENERAL,
	ON_IDLE_MAX_NUM
};

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,				// Basic inventory. (2 pages of 45 spaces = 90 spaces)
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,			// NOTE: Belt inventory provided by the new belt slot item in W2.1 version
#ifdef ENABLE_RENEWAL_SWITCHBOT
	SWITCHBOT,
#endif
#ifdef ENABLE_SPECIAL_INVENTORY
	SKILL_BOOK_INVENTORY,
	UPGRADE_ITEMS_INVENTORY,
	STONE_INVENTORY,
	GIFT_BOX_INVENTORY,
	CHANGERS_INVENTORY,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	AURA_REFINE,
#endif
	GROUND,
};

enum EMobSizes
{
	MOBSIZE_RESERVED,
	MOBSIZE_SMALL,
	MOBSIZE_MEDIUM,
	MOBSIZE_BIG
};

enum EAIFlags
{
	AIFLAG_AGGRESSIVE		= (1 << 0),
	AIFLAG_NOMOVE			= (1 << 1),
	AIFLAG_COWARD			= (1 << 2),
	AIFLAG_NOATTACKSHINSU	= (1 << 3),
	AIFLAG_NOATTACKJINNO	= (1 << 4),
	AIFLAG_NOATTACKCHUNJO	= (1 << 5),
	AIFLAG_ATTACKMOB		= (1 << 6),
	AIFLAG_BERSERK			= (1 << 7),
	AIFLAG_STONESKIN		= (1 << 8),
	AIFLAG_GODSPEED			= (1 << 9),
	AIFLAG_DEATHBLOW		= (1 << 10),
	AIFLAG_REVIVE			= (1 << 11),
#ifdef ENABLE_SUNG_MAHI_TOWER
	AIFLAG_NOATTACK			= (1 << 12),
#endif
};

enum EMobStatType
{
	MOB_STATTYPE_POWER,
	MOB_STATTYPE_TANKER,
	MOB_STATTYPE_SUPER_POWER,
	MOB_STATTYPE_SUPER_TANKER,
	MOB_STATTYPE_RANGE,
	MOB_STATTYPE_MAGIC,
	MOB_STATTYPE_MAX_NUM
};

enum EImmuneFlags
{
	IMMUNE_STUN		= (1 << 0),
	IMMUNE_SLOW		= (1 << 1),
	IMMUNE_FALL		= (1 << 2),
	IMMUNE_CURSE	= (1 << 3),
	IMMUNE_POISON	= (1 << 4),
	IMMUNE_TERROR	= (1 << 5),
	IMMUNE_REFLECT	= (1 << 6),
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

enum
{
	SKILL_ATTR_TYPE_NORMAL = 1,
	SKILL_ATTR_TYPE_MELEE,
	SKILL_ATTR_TYPE_RANGE,
	SKILL_ATTR_TYPE_MAGIC
};

enum
{
	SKILL_NORMAL,
	SKILL_MASTER,
	SKILL_GRAND_MASTER,
	SKILL_PERFECT_MASTER,
};

#ifdef ENABLE_CONQUEROR_LEVEL
enum EConquerorMap
{
	EASTPLAIN_01 = 129,
	EASTPLAIN_02 = 376,
	EASTPLAIN_03 = 377,
	EMPIRECASTLE = 132,
};
#endif

enum EGuildWarType
{
	GUILD_WAR_TYPE_FIELD,
	GUILD_WAR_TYPE_BATTLE,
	GUILD_WAR_TYPE_FLAG,
	GUILD_WAR_TYPE_MAX_NUM
};

enum EGuildWarState
{
	GUILD_WAR_NONE,
	GUILD_WAR_SEND_DECLARE,
	GUILD_WAR_REFUSE,
	GUILD_WAR_RECV_DECLARE,
	GUILD_WAR_WAIT_START,
	GUILD_WAR_CANCEL,
	GUILD_WAR_ON_WAR,
	GUILD_WAR_END,
	GUILD_WAR_OVER,
	GUILD_WAR_RESERVE,

	GUILD_WAR_DURATION = 30*60,
	GUILD_WAR_WIN_POINT = 1000,
	GUILD_WAR_LADDER_HALF_PENALTY_TIME = 12*60*60,
};

enum EAttributeSet
{
	ATTRIBUTE_SET_WEAPON,
	ATTRIBUTE_SET_BODY,
	ATTRIBUTE_SET_WRIST,
	ATTRIBUTE_SET_FOOTS,
	ATTRIBUTE_SET_NECK,
	ATTRIBUTE_SET_HEAD,
	ATTRIBUTE_SET_SHIELD,
	ATTRIBUTE_SET_EAR,
#ifdef ENABLE_PENDANT_SYSTEM
	ATTRIBUTE_SET_PENDANT,
#endif
#ifdef ENABLE_BONUS_COSTUME_SYSTEM
	ATTRIBUTE_SET_COSTUME_BODY,
	ATTRIBUTE_SET_COSTUME_HEAD,
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	ATTRIBUTE_SET_COSTUME_WEAPON,
#endif
#endif

	ATTRIBUTE_SET_MAX_NUM
};

enum EPrivType
{
	PRIV_NONE,
	PRIV_ITEM_DROP,
	PRIV_GOLD_DROP,
	PRIV_GOLD10_DROP,
	PRIV_EXP_PCT,
	MAX_PRIV_NUM,
};

enum EMoneyLogType
{
	MONEY_LOG_RESERVED,
	MONEY_LOG_MONSTER,
	MONEY_LOG_SHOP,
	MONEY_LOG_REFINE,
	MONEY_LOG_QUEST,
	MONEY_LOG_GUILD,
	MONEY_LOG_MISC,
	MONEY_LOG_MONSTER_KILL,
	MONEY_LOG_DROP,
	MONEY_LOG_TYPE_MAX_NUM,
};

enum EPremiumTypes
{
	PREMIUM_EXP,
	PREMIUM_ITEM,
	PREMIUM_SAFEBOX,
	PREMIUM_AUTOLOOT,
	PREMIUM_FISH_MIND,
	PREMIUM_MARRIAGE_FAST,
	PREMIUM_GOLD,
	PREMIUM_MAX_NUM = 9
};

enum SPECIAL_EFFECT
{
	SE_NONE,
	SE_HPUP_RED,
	SE_SPUP_BLUE,
	SE_SPEEDUP_GREEN,
	SE_DXUP_PURPLE,
	SE_CRITICAL,
	SE_PENETRATE,
	SE_BLOCK,
	SE_DODGE,
	SE_CHINA_FIREWORK,
	SE_SPIN_TOP,
	SE_SUCCESS,
	SE_FAIL,
	SE_FR_SUCCESS,
	SE_PERCENT_DAMAGE1,
	SE_PERCENT_DAMAGE2,
	SE_PERCENT_DAMAGE3,

	SE_AUTO_HPUP,
	SE_AUTO_SPUP,

	SE_EQUIP_RAMADAN_RING,
	SE_EQUIP_HALLOWEEN_CANDY,
	SE_EQUIP_HAPPINESS_RING,
	SE_EQUIP_LOVE_PENDANT,

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	SE_EFFECT_ACCE_SUCCEDED,
	SE_EFFECT_ACCE_EQUIP,
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	SE_GYEONGGONG_BOOM,
#endif

#ifdef ENABLE_OCHAO_TEMPLE_SYSTEM
	SE_EFFECT_HEALER,
#endif

#ifdef ENABLE_ZODIAC_MISSION
	EFFECT_SKILL_DAMAGE_ZONE,
	EFFECT_SKILL_DAMAGE_ZONE_BUYUK,
	EFFECT_SKILL_DAMAGE_ZONE_ORTA,
	EFFECT_SKILL_DAMAGE_ZONE_KUCUK,
	EFFECT_SKILL_SAFE_ZONE,
	EFFECT_SKILL_SAFE_ZONE_BUYUK,
	EFFECT_SKILL_SAFE_ZONE_ORTA,
	EFFECT_SKILL_SAFE_ZONE_KUCUK,
	EFFECT_METEOR,
	EFFECT_BEAD_RAIN,
	EFFECT_FALL_ROCK,
	EFFECT_ARROW_RAIN,
	EFFECT_HORSE_DROP,
	EFFECT_EGG_DROP,
	EFFECT_DEAPO_BOOM,
#endif

#ifdef ENABLE_QUEEN_NETHIS
	SE_EFFECT_SNAKE_REGEN,
#endif

#ifdef ENABLE_PASSIVE_SYSTEM
	SE_PASSIVE_EFFECT,
#endif

#ifdef ENABLE_CONQUEROR_LEVEL
	SE_EFFECT_YOHARA,
#endif
};

#include "item_length.h"

// Structure representing the position of inventory
// The reason there is an implicit cast to int is,
// Because all inventory-related functions did not receive window_type, but only received one cell (because there was only one inventory, and inventory type was not necessary)
// It is difficult to modify all the function calls related to inventory.

enum EDragonSoulRefineWindowSize
{
	DRAGON_SOUL_REFINE_GRID_MAX = 15,
};

enum EMisc2
{
	DRAGON_SOUL_EQUIP_SLOT_START		= INVENTORY_MAX_NUM + WEAR_MAX_NUM,
	DRAGON_SOUL_EQUIP_SLOT_END			= DRAGON_SOUL_EQUIP_SLOT_START + (DS_SLOT_MAX * DRAGON_SOUL_DECK_MAX_NUM),
	DRAGON_SOUL_EQUIP_RESERVED_SLOT_END	= DRAGON_SOUL_EQUIP_SLOT_END + (DS_SLOT_MAX * DRAGON_SOUL_DECK_RESERVED_MAX_NUM),

	BELT_INVENTORY_SLOT_START			= DRAGON_SOUL_EQUIP_RESERVED_SLOT_END,
	BELT_INVENTORY_SLOT_END				= BELT_INVENTORY_SLOT_START + BELT_INVENTORY_SLOT_COUNT,

#ifdef ENABLE_SPECIAL_INVENTORY
	SKILL_BOOK_INVENTORY_SLOT_START		= BELT_INVENTORY_SLOT_END,
	SKILL_BOOK_INVENTORY_SLOT_END		= SKILL_BOOK_INVENTORY_SLOT_START + SKILL_BOOK_INVENTORY_MAX_NUM,

	UPGRADE_ITEMS_INVENTORY_SLOT_START	= SKILL_BOOK_INVENTORY_SLOT_END,
	UPGRADE_ITEMS_INVENTORY_SLOT_END	= UPGRADE_ITEMS_INVENTORY_SLOT_START + UPGRADE_ITEMS_INVENTORY_MAX_NUM,

	STONE_INVENTORY_SLOT_START			= UPGRADE_ITEMS_INVENTORY_SLOT_END,
	STONE_INVENTORY_SLOT_END			= STONE_INVENTORY_SLOT_START + STONE_INVENTORY_MAX_NUM,

	GIFT_BOX_INVENTORY_SLOT_START		= STONE_INVENTORY_SLOT_END,
	GIFT_BOX_INVENTORY_SLOT_END			= GIFT_BOX_INVENTORY_SLOT_START + GIFT_BOX_INVENTORY_MAX_NUM,

	CHANGERS_INVENTORY_SLOT_START		= GIFT_BOX_INVENTORY_SLOT_END,
	CHANGERS_INVENTORY_SLOT_END			= CHANGERS_INVENTORY_SLOT_START + CHANGERS_INVENTORY_MAX_NUM,

	INVENTORY_AND_EQUIP_SLOT_MAX		= CHANGERS_INVENTORY_SLOT_END,
#else
	INVENTORY_AND_EQUIP_SLOT_MAX		= BELT_INVENTORY_SLOT_END,
#endif
};

#pragma pack(push, 1)

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
	SItemPos ()
	{
		window_type = INVENTORY;
		cell = WORD_MAX;
	}

	SItemPos (BYTE _window_type, WORD _cell)
	{
		window_type = _window_type;
		cell = _cell;
	}

	bool IsValidItemPosition() const
	{
		switch (window_type)
		{
			case RESERVED_WINDOW:
				return false;
			case INVENTORY:
			case EQUIPMENT:
			case BELT_INVENTORY:
#ifdef ENABLE_SPECIAL_INVENTORY
			case SKILL_BOOK_INVENTORY:
			case UPGRADE_ITEMS_INVENTORY:
			case STONE_INVENTORY:
			case GIFT_BOX_INVENTORY:
			case CHANGERS_INVENTORY:
#endif
				return cell < INVENTORY_AND_EQUIP_SLOT_MAX;

			case DRAGON_SOUL_INVENTORY:
				return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);

			case SAFEBOX:
			case MALL:
				return false;

#ifdef ENABLE_RENEWAL_SWITCHBOT
			case SWITCHBOT:
				return cell < SWITCHBOT_SLOT_COUNT;
#endif
			default:
				return false;
		}
		return false;
	}

	bool IsEquipPosition() const
	{
		return ((INVENTORY == window_type || EQUIPMENT == window_type) && cell >= INVENTORY_MAX_NUM && cell < INVENTORY_MAX_NUM + WEAR_MAX_NUM)
			|| IsDragonSoulEquipPosition();
	}

	bool IsDragonSoulEquipPosition() const
	{
		return (DRAGON_SOUL_EQUIP_SLOT_START <= cell) && (DRAGON_SOUL_EQUIP_SLOT_END > cell);
	}

	bool IsBeltInventoryPosition() const
	{
		return (BELT_INVENTORY_SLOT_START <= cell) && (BELT_INVENTORY_SLOT_END > cell);
	}

	bool IsDefaultInventoryPosition() const
	{
		return INVENTORY == window_type && cell < INVENTORY_MAX_NUM;
	}

#ifdef ENABLE_RENEWAL_SWITCHBOT
	bool IsSwitchbotPosition() const
	{
		return SWITCHBOT == window_type && cell < SWITCHBOT_SLOT_COUNT;
	}
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	bool IsSkillBookInventoryPosition() const
	{
		return ((INVENTORY == window_type) && cell >= SKILL_BOOK_INVENTORY_SLOT_START && cell < SKILL_BOOK_INVENTORY_SLOT_END);
	}

	bool IsUpgradeItemsInventoryPosition() const
	{
		return ((INVENTORY == window_type) && cell >= UPGRADE_ITEMS_INVENTORY_SLOT_START && cell < UPGRADE_ITEMS_INVENTORY_SLOT_END);
	}

	bool IsStoneInventoryPosition() const
	{
		return ((INVENTORY == window_type) && cell >= STONE_INVENTORY_SLOT_START && cell < STONE_INVENTORY_SLOT_END);
	}

	bool IsGiftBoxInventoryPosition() const
	{
		return ((INVENTORY == window_type) && cell >= GIFT_BOX_INVENTORY_SLOT_START && cell < GIFT_BOX_INVENTORY_SLOT_END);
	}

	bool IsChangersInventoryPosition() const
	{
		return ((INVENTORY == window_type) && cell >= CHANGERS_INVENTORY_SLOT_START && cell < CHANGERS_INVENTORY_SLOT_END);
	}
#endif

	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}

	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}

	bool operator!=(const struct SItemPos& rhs) const
	{
		return (window_type != rhs.window_type) || (cell != rhs.cell);
	}
} TItemPos;

const TItemPos NPOS (RESERVED_WINDOW, WORD_MAX);

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
typedef struct SItemPosEx
{
	TItemPos pos;
	int id{0};
} TItemPosEx;
#endif

typedef enum
{
	SHOP_COIN_TYPE_GOLD, // DEFAULT VALUE
	SHOP_COIN_TYPE_SECONDARY_COIN,
} EShopCoinType;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
enum ELocale
{
	LOCALE_EUROPE,
	LOCALE_EN,		// United Kingdom
	LOCALE_RO,		// Romania
	LOCALE_PT,		// Portugal
	LOCALE_ES,		// Spain
	LOCALE_FR,		// France
	LOCALE_DE,		// Germany
	LOCALE_PL,		// Poland
	LOCALE_IT,		// Italy
	LOCALE_CZ,		// Czech
	LOCALE_HU,		// Hungary
	LOCALE_TR,		// Turkey
	LOCALE_MAX_NUM,
	LOCALE_DEFAULT = LOCALE_EN,
};
#endif

#ifdef ENABLE_WHITE_DRAGON
enum
{
	MAP_WHITEDRAGONCAVE_01 = 393,
	MAP_WHITEDRAGONCAVE_02 = 394,
	MAP_WHITEDRAGONCAVE_BOSS = 395,
};
#endif

#ifdef ENABLE_QUEEN_NETHIS
enum {
	MAP_SNAKEVALLEY = 385,
	MAP_SNAKE_TEMPLE_01 = 390,
	MAP_SNAKE_TEMPLE_02 = 391,
};
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
namespace ESkillColorLength
{
	enum ESkillColorLength
	{
#ifdef ENABLE_NINETH_SKILL
		MAX_SKILL_COUNT = 9,
#else
		MAX_SKILL_COUNT = 6,
#endif
		MAX_EFFECT_COUNT = 5,
#if defined(ENABLE_WOLFMAN_CHARACTER) && defined(ENABLE_NINETH_SKILL)
		MAX_BUFF_COUNT = 7,
#elif defined(ENABLE_WOLFMAN_CHARACTER) && !defined(ENABLE_NINETH_SKILL)
		MAX_BUFF_COUNT = 6,
#else
		MAX_BUFF_COUNT = 5,
#endif
		BUFF_BEGIN = MAX_SKILL_COUNT,
	};
}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
typedef struct SAuraRefineInfo
{
	BYTE bAuraRefineInfoLevel;
	BYTE bAuraRefineInfoExpPercent;
} TAuraRefineInfo;

enum EAuraMisc
{
	AURA_MAX_LEVEL = 250,
	AURA_REFINE_MAX_DISTANCE = 1000,
};

enum EAuraWindowType
{
	AURA_WINDOW_TYPE_ABSORB,
	AURA_WINDOW_TYPE_GROWTH,
	AURA_WINDOW_TYPE_EVOLVE,
	AURA_WINDOW_TYPE_MAX,
};

enum EAuraSlotType
{
	AURA_SLOT_MAIN,
	AURA_SLOT_SUB,
	AURA_SLOT_RESULT,
	AURA_SLOT_MAX
};

enum EAuraGradeType
{
	AURA_GRADE_NONE,
	AURA_GRADE_ORDINARY,
	AURA_GRADE_SIMPLE,
	AURA_GRADE_NOBLE,
	AURA_GRADE_SPARKLING,
	AURA_GRADE_MAGNIFICENT,
	AURA_GRADE_RADIANT,
	AURA_GRADE_MAX_NUM,
};

enum EAuraRefineInfoType
{
	AURA_REFINE_INFO_STEP,
	AURA_REFINE_INFO_LEVEL_MIN,
	AURA_REFINE_INFO_LEVEL_MAX,
	AURA_REFINE_INFO_NEED_EXP,
	AURA_REFINE_INFO_MATERIAL_VNUM,
	AURA_REFINE_INFO_MATERIAL_COUNT,
	AURA_REFINE_INFO_NEED_GOLD,
	AURA_REFINE_INFO_EVOLVE_PCT,
	AURA_REFINE_INFO_MAX
};

enum ERefineInfoType
{
	AURA_REFINE_INFO_SLOT_CURRENT,
	AURA_REFINE_INFO_SLOT_NEXT,
	AURA_REFINE_INFO_SLOT_EVOLVED,
	AURA_REFINE_INFO_SLOT_MAX
};
#endif

#ifdef ENABLE_BATTLE_PASS
enum EBattlePassItem
{
	ITEM_BATTLE_PASS = 50118,
	ITEM_BATTLE_PASS_PREMIUM = 50119,
};
#endif

#pragma pack(pop)

#endif
