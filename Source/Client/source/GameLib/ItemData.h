#pragma once

#include "StdAfx.h"

#include "../EterLib/GrpSubImage.h"
#include "../EterGrnLib/Thing.h"
#include "../UserInterface/Locale_inc.h"

class CItemData
{
	public:
		enum
		{
			ITEM_NAME_MAX_LEN = 48,
			ITEM_LIMIT_MAX_NUM = 2,
			ITEM_VALUES_MAX_NUM = 6,
			ITEM_SMALL_DESCR_MAX_LEN = 256,
			ITEM_APPLY_MAX_NUM = 3,
			ITEM_SOCKET_MAX_NUM = 3,
#ifdef ENABLE_BONUS_COSTUME_SYSTEM
			COSTUME_ATTRIBUTE_MAX_NUM = 3,
#endif
		};

		enum EItemType
		{
			ITEM_TYPE_NONE,					// 0
			ITEM_TYPE_WEAPON,				// 1
			ITEM_TYPE_ARMOR,				// 2
			ITEM_TYPE_USE,					// 3
			ITEM_TYPE_AUTOUSE,				// 4
			ITEM_TYPE_MATERIAL,				// 5
			ITEM_TYPE_SPECIAL,				// 6
			ITEM_TYPE_TOOL,					// 7
			ITEM_TYPE_LOTTERY,				// 8
			ITEM_TYPE_ELK,					// 9
			ITEM_TYPE_METIN,				// 10
			ITEM_TYPE_CONTAINER,			// 11
			ITEM_TYPE_FISH,					// 12
			ITEM_TYPE_ROD,					// 13
			ITEM_TYPE_RESOURCE,				// 14
			ITEM_TYPE_CAMPFIRE,				// 15
			ITEM_TYPE_UNIQUE,				// 16
			ITEM_TYPE_SKILLBOOK,			// 17
			ITEM_TYPE_QUEST,				// 18
			ITEM_TYPE_POLYMORPH,			// 19
			ITEM_TYPE_TREASURE_BOX,			// 20
			ITEM_TYPE_TREASURE_KEY,			// 21
			ITEM_TYPE_SKILLFORGET,			// 22
			ITEM_TYPE_GIFTBOX,				// 23
			ITEM_TYPE_PICK,					// 24
			ITEM_TYPE_HAIR,					// 25
			ITEM_TYPE_TOTEM,				// 26
			ITEM_TYPE_BLEND,				// 27
			ITEM_TYPE_COSTUME,				// 28
			ITEM_TYPE_DS,					// 29
			ITEM_TYPE_SPECIAL_DS,			// 30
			ITEM_TYPE_EXTRACT,				// 31
			ITEM_TYPE_SECONDARY_COIN,		// 32
			ITEM_TYPE_RING,					// 33
#ifdef ENABLE_MOUNT_SYSTEM
			ITEM_TYPE_MOUNT,				// 34
#endif
#ifdef ENABLE_PET_SYSTEM
			ITEM_TYPE_PET,					// 35
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
			ITEM_TYPE_GROWTH_PET,			// 36
#endif
#ifdef ENABLE_NEW_ITEM_TYPE_GACHA
			ITEM_TYPE_GACHA,				// 37 gacha
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
			ITEM_TYPE_PASSIVE,				// 38 passive
#endif
#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
			ITEM_TYPE_MEDIUM,
#endif

			ITEM_TYPE_MAX_NUM,
		};

		enum EWeaponSubTypes
		{
			WEAPON_SWORD, //0
			WEAPON_DAGGER,
			WEAPON_BOW, //2
			WEAPON_TWO_HANDED, //3
			WEAPON_BELL, //4
			WEAPON_FAN, //5
			WEAPON_ARROW, //6
			WEAPON_QUIVER, //7
			WEAPON_NUM_TYPES, //11 2015/11/12

			WEAPON_NONE = WEAPON_NUM_TYPES+1,
		};

		enum EMaterialSubTypes
		{
			MATERIAL_LEATHER,
			MATERIAL_BLOOD,
			MATERIAL_ROOT,
			MATERIAL_NEEDLE,
			MATERIAL_JEWEL,
			MATERIAL_DS_REFINE_NORMAL,
			MATERIAL_DS_REFINE_BLESSED,
			MATERIAL_DS_REFINE_HOLLY,
#ifdef ENABLE_PASSIVE_SYSTEM
			MATERIAL_PASSIVE_WEAPON,
			MATERIAL_PASSIVE_ARMOR,
			MATERIAL_PASSIVE_ACCE,
			MATERIAL_PASSIVE_ELEMENT,
#endif
		};

		enum EArmorSubTypes
		{
			ARMOR_BODY,
			ARMOR_HEAD,
			ARMOR_SHIELD,
			ARMOR_WRIST,
			ARMOR_FOOTS,
			ARMOR_NECK,
			ARMOR_EAR,
			ARMOR_BELT,
#ifdef ENABLE_PENDANT_SYSTEM
			ARMOR_PENDANT,
#endif
			ARMOR_NUM_TYPES
		};

		enum ECostumeSubTypes
		{
			COSTUME_BODY,				//0
			COSTUME_HAIR,				//1
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			COSTUME_ACCE,				//2
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
			COSTUME_WEAPON,				//3
#endif
#ifdef ENABLE_MOUNT_PET_SKIN
			COSTUME_MOUNT,				//4
			COSTUME_PET,				//5
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			COSTUME_AURA,				//5
#endif
			COSTUME_NUM_TYPES,
		};

#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
		enum EMediumSubTypes
		{
			MEDIUM_MOVE_COSTUME_ATTR = 0,
			MEDIUM_MOVE_COSTUME_ACCE_ATTR = 1,
		};
#endif

		enum EUseSubTypes
		{
			USE_POTION,
			USE_TALISMAN,
			USE_TUNING,
			USE_MOVE,
			USE_TREASURE_BOX,
			USE_MONEYBAG,
			USE_BAIT,
			USE_ABILITY_UP,
			USE_AFFECT,
			USE_CREATE_STONE,
			USE_SPECIAL,
			USE_POTION_NODELAY,
			USE_CLEAR,
			USE_INVISIBILITY,
			USE_DETACHMENT,
			USE_BUCKET,
			USE_POTION_CONTINUE,
			USE_CLEAN_SOCKET,
			USE_CHANGE_ATTRIBUTE,
			USE_ADD_ATTRIBUTE,
			USE_ADD_ACCESSORY_SOCKET,
			USE_PUT_INTO_ACCESSORY_SOCKET,
			USE_ADD_ATTRIBUTE2,
			USE_RECIPE,
			USE_CHANGE_ATTRIBUTE2,
			USE_BIND,
			USE_UNBIND,
			USE_TIME_CHARGE_PER,
			USE_TIME_CHARGE_FIX,
			USE_PUT_INTO_BELT_SOCKET,
			USE_PUT_INTO_RING_SOCKET,
#ifdef ENABLE_TITLE_SYSTEM
			USE_TITLE,
#endif
#ifdef ENABLE_RENEWAL_AFFECT
			USE_AFFECT_PLUS,
#endif
#ifdef ENABLE_BONUS_COSTUME_SYSTEM
			USE_CHANGE_COSTUME_ATTR,
			USE_RESET_COSTUME_ATTR,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			USE_PUT_INTO_AURA_SOCKET,
#endif
#ifdef ENABLE_COLLECTIONS_SYSTEM
			USE_COLLECTION_SCROLL,
#endif
		};

#ifdef ENABLE_PASSIVE_SYSTEM
		enum EPassiveSubTypes
		{
			PASSIVE_JOB,
		};
#endif

		enum EResourceSubTypes
		{
			RESOURCE_FISHBONE,
			RESOURCE_WATERSTONEPIECE,
			RESOURCE_WATERSTONE,
			RESOURCE_BLOOD_PEARL,
			RESOURCE_BLUE_PEARL,
			RESOURCE_WHITE_PEARL,
			RESOURCE_BUCKET,
			RESOURCE_CRYSTAL,
			RESOURCE_GEM,
			RESOURCE_STONE,
			RESOURCE_METIN,
			RESOURCE_ORE,
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			RESOURCE_AURA,
#endif
		};

#ifdef ENABLE_GROWTH_PET_SYSTEM
		enum EPetSubTypes
		{
			PET_EGG,
			PET_UPBRINGING,
			PET_BAG,
			PET_FEEDSTUFF,
			PET_SKILL,
			PET_SKILL_DEL_BOOK,
			PET_NAME_CHANGE,
			PET_EXPFOOD,
			PET_SKILL_ALL_DEL_BOOK,
			PET_EXPFOOD_PER,
			PET_ATTR_DETERMINE,
			PET_ATTR_CHANGE,
			PET_PAY,
			PET_PRIMIUM_FEEDSTUFF,
		};

		enum EEggUse
		{
			EGG_USE_SUCCESS,
			EGG_USE_FAILED_BECAUSE_NAME,
			EGG_USE_FAILED_TIMEOVER,
		};

		enum EPetEggUse
		{
			PET_EGG_USE_TRUE,
			PET_EGG_USE_FAILED_BECAUSE_TRADING,
			PET_EGG_USE_FAILED_BECAUSE_SHOP_OPEN,
			PET_EGG_USE_FAILED_BECAUSE_MALL_OPEN,
			PET_EGG_USE_FAILED_BECAUSE_SAFEBOX_OPEN,
		};

		enum EPetNameChange
		{
			NAME_CHANGE_USE_SUCCESS,
			NAME_CHANGE_USE_FAILED_BECAUSE_NAME,
		};

		enum EPet
		{
			PET_ATTR_CHANGE_ITEMVNUM = 55033,
			PET_HATCHING_MONEY = 100000,
			PET_NAME_MAX_SIZE = 20,
			PET_NAME_MIN_SIZE = 4,
			E_SEAL_DATE_DEFAULT_TIMESTAMP = -1,
		};
#endif

		enum EWeddingItem
		{
			WEDDING_TUXEDO1 = 11901,
			WEDDING_TUXEDO2 = 11902,
			WEDDING_BRIDE_DRESS1 = 11903,
			WEDDING_BRIDE_DRESS2 = 11904,
			WEDDING_TUXEDO3 = 11911,
			WEDDING_TUXEDO4 = 11912,
			WEDDING_BRIDE_DRESS3 = 11913,
			WEDDING_BRIDE_DRESS4 = 11914,
			WEDDING_BOUQUET1 = 50201,
			WEDDING_BOUQUET2 = 50202,
		};

		enum EDragonSoulSubType
		{
			DS_SLOT1,
			DS_SLOT2,
			DS_SLOT3,
			DS_SLOT4,
			DS_SLOT5,
			DS_SLOT6,
#ifdef ENABLE_DRAGON_SOUL_7_SLOT
			DS_SLOT7,
			DS_SLOT_NUM_TYPES = 7,
#else
			DS_SLOT_NUM_TYPES = 6,
#endif
		};

		enum EMetinSubTypes
		{
			METIN_NORMAL,
			METIN_GOLD,
		};

		enum EQuestSubTypes
		{
			QUEST_NONE,
			QUEST_PET_PAY,
			QUEST_WARP,
			QUEST_GEM_BAG,
		};

		enum ELimitTypes
		{
			LIMIT_NONE,
			LIMIT_LEVEL,
			LIMIT_STR,
			LIMIT_DEX,
			LIMIT_INT,
			LIMIT_CON,
			LIMIT_REAL_TIME,
			LIMIT_REAL_TIME_START_FIRST_USE,
			LIMIT_TIMER_BASED_ON_WEAR,
#ifdef ENABLE_PITTY_REFINE
			LIMIT_PITTY_REFINE,
#endif
			LIMIT_MAX_NUM
		};

		enum EItemAntiFlag
		{
			ITEM_ANTIFLAG_FEMALE		= (1 << 0),
			ITEM_ANTIFLAG_MALE			= (1 << 1),
			ITEM_ANTIFLAG_WARRIOR		= (1 << 2),
			ITEM_ANTIFLAG_ASSASSIN		= (1 << 3),
			ITEM_ANTIFLAG_SURA			= (1 << 4),
			ITEM_ANTIFLAG_SHAMAN		= (1 << 5),
			ITEM_ANTIFLAG_GET			= (1 << 6),
			ITEM_ANTIFLAG_DROP			= (1 << 7),
			ITEM_ANTIFLAG_SELL			= (1 << 8),
			ITEM_ANTIFLAG_EMPIRE_A		= (1 << 9),
			ITEM_ANTIFLAG_EMPIRE_B		= (1 << 10),
			ITEM_ANTIFLAG_EMPIRE_R		= (1 << 11),
			ITEM_ANTIFLAG_SAVE			= (1 << 12),
			ITEM_ANTIFLAG_GIVE			= (1 << 13),
			ITEM_ANTIFLAG_PKDROP		= (1 << 14),
			ITEM_ANTIFLAG_STACK			= (1 << 15),
			ITEM_ANTIFLAG_MYSHOP		= (1 << 16),
			ITEM_ANTIFLAG_SAFEBOX		= (1 << 17),
#ifdef ENABLE_DESTROY_DIALOG
			ITEM_ANTIFLAG_DESTROY		= (1 << 18),
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
			ITEM_ANTIFLAG_PETFEED		= (1 << 19),
#endif
			ITEM_ANTIFLAG_RT_REMOVE		= (1 << 20),
		};

		enum EItemFlag
		{
			ITEM_FLAG_REFINEABLE		= (1 << 0),
			ITEM_FLAG_SAVE				= (1 << 1),
			ITEM_FLAG_STACKABLE			= (1 << 2),
			ITEM_FLAG_COUNT_PER_1GOLD	= (1 << 3),
			ITEM_FLAG_SLOW_QUERY		= (1 << 4),
			ITEM_FLAG_RARE				= (1 << 5),
			ITEM_FLAG_UNIQUE			= (1 << 6),
			ITEM_FLAG_MAKECOUNT			= (1 << 7),
			ITEM_FLAG_IRREMOVABLE		= (1 << 8),
			ITEM_FLAG_CONFIRM_WHEN_USE	= (1 << 9),
			ITEM_FLAG_QUEST_USE			= (1 << 10),
			ITEM_FLAG_QUEST_USE_MULTIPLE= (1 << 11),
			ITEM_FLAG_UNUSED03			= (1 << 12),
			ITEM_FLAG_LOG				= (1 << 13),
			ITEM_FLAG_APPLICABLE		= (1 << 14),
		};

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
			WEAR_COSTUME_ACCE,		// 21
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
			WEAR_COSTUME_WEAPON,		// 22
#endif
#ifdef ENABLE_MOUNT_PET_SKIN
			WEAR_COSTUME_MOUNT,		// 23
			WEAR_COSTUME_PET,		// 24
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			WEAR_COSTUME_AURA,		// 25
#endif
			WEAR_RING1,		// 26
			WEAR_RING2,		// 27
			WEAR_BELT,		// 28
#ifdef ENABLE_MOUNT_SYSTEM
			WEAR_MOUNT,		// 29
#endif
#ifdef ENABLE_PET_SYSTEM
			WEAR_PET,		// 30
#endif
#ifdef ENABLE_PENDANT_SYSTEM
			WEAR_PENDANT,		// 31
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
			WEAR_PASSIVE,			// 32
#endif

			WEAR_MAX_NUM = 64,
		};

		enum EItemWearableFlag
		{
			WEARABLE_BODY					= (1 << 0),
			WEARABLE_HEAD					= (1 << 1),
			WEARABLE_FOOTS					= (1 << 2),
			WEARABLE_WRIST					= (1 << 3),
			WEARABLE_WEAPON					= (1 << 4),
			WEARABLE_NECK					= (1 << 5),
			WEARABLE_EAR					= (1 << 6),
			WEARABLE_UNIQUE					= (1 << 7),
			WEARABLE_SHIELD					= (1 << 8),
			WEARABLE_ARROW					= (1 << 9),
			WEARABLE_HAIR					= (1 << 10),
			WEARABLE_ABILITY 				= (1 << 11),
			WEARABLE_COSTUME_BODY			= (1 << 12),
			WEARABLE_COSTUME_HAIR			= (1 << 13),
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			WEARABLE_COSTUME_ACCE			= (1 << 14),
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
			WEARABLE_COSTUME_WEAPON			= (1 << 15),
#endif
#ifdef ENABLE_MOUNT_PET_SKIN
			WEARABLE_COSTUME_MOUNT			= (1 << 16),
			WEARABLE_COSTUME_PET			= (1 << 17),
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			WEARABLE_COSTUME_AURA			= (1 << 18),
#endif
			WEARABLE_BELT					= (1 << 19),
#ifdef ENABLE_MOUNT_SYSTEM
			WEARABLE_MOUNT					= (1 << 20),
#endif
#ifdef ENABLE_PET_SYSTEM
			WEARABLE_PET					= (1 << 21),
#endif
#ifdef ENABLE_PENDANT_SYSTEM
			WEARABLE_PENDANT				= (1 << 22),
#endif
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

		enum EImmuneFlags
		{
			IMMUNE_PARA         = (1 << 0),
			IMMUNE_CURSE        = (1 << 1),
			IMMUNE_STUN         = (1 << 2),
			IMMUNE_SLEEP        = (1 << 3),
			IMMUNE_SLOW         = (1 << 4),
			IMMUNE_POISON       = (1 << 5),
			IMMUNE_TERROR       = (1 << 6),
		};

#pragma pack(push)
#pragma pack(1)
		typedef struct SItemLimit
		{
			BYTE bType;
			long lValue;
		} TItemLimit;

		typedef struct SItemApply
		{
			WORD bType;
			long lValue;
		} TItemApply;

		typedef struct SItemTable
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

			DWORD dwIBuyItemPrice;
			DWORD dwISellItemPrice;

			TItemLimit aLimits[ITEM_LIMIT_MAX_NUM];
			TItemApply aApplies[ITEM_APPLY_MAX_NUM];
			long alValues[ITEM_VALUES_MAX_NUM];
			long alSockets[ITEM_SOCKET_MAX_NUM];
			DWORD dwRefinedVnum;
			WORD wRefineSet;
			BYTE bAlterToMagicItemPct;
			BYTE bSpecular;
			BYTE bGainSocketPct;
		} TItemTable;

#pragma pack(pop)

	public:
		CItemData();
		virtual ~CItemData();

		void Clear();
		void SetSummary(const std::string& c_rstSumm);
		void SetDescription(const std::string& c_rstDesc);

		CGraphicThing *GetModelThing();
		CGraphicThing *GetSubModelThing();
		CGraphicThing *GetDropModelThing();
		CGraphicSubImage *GetIconImage();

		DWORD GetLODModelThingCount();
		BOOL GetLODModelThingPointer(DWORD dwIndex, CGraphicThing ** ppModelThing);

		DWORD GetAttachingDataCount();
		BOOL GetCollisionDataPointer(DWORD dwIndex, const NRaceData::TAttachingData ** c_ppAttachingData);
		BOOL GetAttachingDataPointer(DWORD dwIndex, const NRaceData::TAttachingData ** c_ppAttachingData);

		const TItemTable* GetTable() const;
		DWORD GetIndex() const;
		const char *GetName() const;
		const char *GetDescription() const;
		const char *GetSummary() const;
		BYTE GetType() const;
		BYTE GetSubType() const;
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		DWORD GetAntiFlags() const;
#endif
		UINT GetRefine() const;
		const char *GetUseTypeString() const;
		DWORD GetWeaponType() const;
		BYTE GetSize() const;
		BOOL IsAntiFlag(DWORD dwFlag) const;
		BOOL IsFlag(DWORD dwFlag) const;
		BOOL IsWearableFlag(DWORD dwFlag) const;
		BOOL HasNextGrade() const;
#if defined(ENABLE_WIKI_SYSTEM) || defined(INSIDE_RENDER)
		WORD GetRefinedSet() const;
		WORD GetRefinedVnum() const;
#endif

		DWORD GetWearFlags() const;
		DWORD GetIBuyItemPrice() const;
		DWORD GetISellItemPrice() const;
		BOOL GetLimit(BYTE byIndex, TItemLimit * pItemLimit) const;
		BOOL GetApply(BYTE byIndex, TItemApply * pItemApply) const;
		long GetValue(BYTE byIndex) const;
		long GetSocket(BYTE byIndex) const;
		long SetSocket(BYTE byIndex,DWORD value);
		int GetSocketCount() const;
		DWORD GetIconNumber() const;

		UINT GetSpecularPoweru() const;
		float GetSpecularPowerf() const;

		BOOL IsEquipment() const;

		BYTE GetItemMaskType() const;
		BYTE GetItemMaskSubType() const;
		DWORD GetAttr67MaterialVnum() const;
		int GetRefineLevel() const;

		static bool IsCostumeMountItem(BYTE bType, BYTE bSubType);
		static bool IsWhiteDragonBlessItem(DWORD dwVnum);
		static bool IsIronDragonBlessItem(DWORD dwVnum);

		void SetDefaultItemData(const char *c_szIconFileName, const char *c_szModelFileName  = nullptr);
		void SetItemTableData(TItemTable * pItemTable);

	protected:
		void __LoadFiles();
		void __SetIconImage(const char *c_szFileName);

	protected:
		std::string m_strModelFileName;
		std::string m_strSubModelFileName;
		std::string m_strDropModelFileName;
		std::string m_strIconFileName;
		std::string m_strDescription;
		std::string m_strSummary;
		std::vector<std::string> m_strLODModelFileNameVector;

		CGraphicThing * m_pModelThing;
		CGraphicThing * m_pSubModelThing;
		CGraphicThing * m_pDropModelThing;
		CGraphicSubImage * m_pIconImage;
		std::vector<CGraphicThing *> m_pLODModelThingVector;

		NRaceData::TAttachingDataVector m_AttachingDataVector;
		DWORD m_dwVnum;
		TItemTable m_ItemTable;
		BYTE m_bMaskType;
		BYTE m_bMaskSubType;
		DWORD m_dwAttr67MaterialVnum;

	public:
		static void DestroySystem();

		static CItemData* New();
		static void Delete(CItemData* pkItemData);

		static CDynamicPool<CItemData> ms_kPool;

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	protected:
		typedef struct SItemScaleTable
		{
			D3DXVECTOR3 v3Scale[NRaceData::SEX_MAX_NUM][NRaceData::JOB_MAX_NUM];
			float fScaleParticle[NRaceData::SEX_MAX_NUM][NRaceData::JOB_MAX_NUM];
		} TItemScaleTable;
		TItemScaleTable m_ItemScaleTable;

	public:
		float GetItemParticleScale(BYTE bJob, BYTE bSex);
		void SetItemTableScaleData(BYTE bJob, BYTE bSex, float fScaleX, float fScaleY, float fScaleZ, float fScaleParticle);
		D3DXVECTOR3 & GetItemScaleVector(BYTE bJob, BYTE bSex);
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	public:
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

		enum EAuraItem
		{
			AURA_BOOST_ITEM_VNUM_BASE = 49980
		};

		enum EAuraBoostIndex
		{
			ITEM_AURA_BOOST_ERASER,
			ITEM_AURA_BOOST_WEAK,
			ITEM_AURA_BOOST_NORMAL,
			ITEM_AURA_BOOST_STRONG,
			ITEM_AURA_BOOST_ULTIMATE,
			ITEM_AURA_BOOST_MAX,
		};

	protected:
		typedef struct SAuraScaleTable
		{
			D3DXVECTOR3 v3MeshScale[NRaceData::SEX_MAX_NUM][NRaceData::JOB_MAX_NUM];
			float fParticleScale[NRaceData::SEX_MAX_NUM][NRaceData::JOB_MAX_NUM];
		} TAuraScaleTable;

		TAuraScaleTable m_AuraScaleTable;
		DWORD m_dwAuraEffectID;

	public:
		void SetAuraScaleTableData(BYTE byJob, BYTE bySex, float fMeshScaleX, float fMeshScaleY, float fMeshScaleZ, float fParticleScale);
		D3DXVECTOR3& GetAuraMeshScaleVector(BYTE byJob, BYTE bySex);
		float GetAuraParticleScale(BYTE byJob, BYTE bySex);

		void SetAuraEffectID(const char* szAuraEffectPath);
		DWORD GetAuraEffectID() const { return m_dwAuraEffectID; }
#endif
};
