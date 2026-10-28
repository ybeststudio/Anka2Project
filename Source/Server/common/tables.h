#ifndef __INC_TABLES_H__
#define __INC_TABLES_H__
#include <stdint.h>
#include "service.h"
#include "length.h"

typedef DWORD IDENT;

enum
{
	HEADER_GD_LOGIN									= 1,
	HEADER_GD_LOGOUT								= 2,
	HEADER_GD_PLAYER_LOAD							= 3,
	HEADER_GD_PLAYER_SAVE							= 4,
	HEADER_GD_PLAYER_CREATE							= 5,
	HEADER_GD_PLAYER_DELETE							= 6,
	HEADER_GD_LOGIN_KEY								= 7,
	HEADER_GD_BOOT									= 9,
	HEADER_GD_PLAYER_COUNT							= 10,
	HEADER_GD_QUEST_SAVE							= 11,
	HEADER_GD_SAFEBOX_LOAD							= 12,
	HEADER_GD_SAFEBOX_SAVE							= 13,
	HEADER_GD_SAFEBOX_CHANGE_SIZE					= 14,
	HEADER_GD_EMPIRE_SELECT							= 15,
	HEADER_GD_SAFEBOX_CHANGE_PASSWORD				= 16,
	HEADER_GD_SAFEBOX_CHANGE_PASSWORD_SECOND		= 17,
	HEADER_GD_DIRECT_ENTER							= 18,
	HEADER_GD_GUILD_SKILL_UPDATE					= 19,
	HEADER_GD_GUILD_EXP_UPDATE						= 20,
	HEADER_GD_GUILD_ADD_MEMBER						= 21,
	HEADER_GD_GUILD_REMOVE_MEMBER					= 22,
	HEADER_GD_GUILD_CHANGE_GRADE					= 23,
	HEADER_GD_GUILD_CHANGE_MEMBER_DATA				= 24,
	HEADER_GD_GUILD_DISBAND							= 25,
	HEADER_GD_GUILD_WAR								= 26,
	HEADER_GD_GUILD_WAR_SCORE						= 27,
	HEADER_GD_GUILD_CREATE							= 28,
	HEADER_GD_ITEM_SAVE								= 30,
	HEADER_GD_ITEM_DESTROY							= 31,
	HEADER_GD_ADD_AFFECT							= 32,
	HEADER_GD_REMOVE_AFFECT							= 33,
	HEADER_GD_HIGHSCORE_REGISTER					= 34,
	HEADER_GD_ITEM_FLUSH							= 35,
	HEADER_GD_PARTY_CREATE							= 36,
	HEADER_GD_PARTY_DELETE							= 37,
	HEADER_GD_PARTY_ADD								= 38,
	HEADER_GD_PARTY_REMOVE							= 39,
	HEADER_GD_PARTY_STATE_CHANGE					= 40,
	HEADER_GD_PARTY_HEAL_USE						= 41,
	HEADER_GD_FLUSH_CACHE							= 42,
	HEADER_GD_RELOAD_PROTO							= 43,
	HEADER_GD_CHANGE_NAME							= 44,
	HEADER_GD_GUILD_CHANGE_LADDER_POINT				= 46,
	HEADER_GD_GUILD_USE_SKILL						= 47,
	HEADER_GD_REQUEST_EMPIRE_PRIV					= 48,
	HEADER_GD_REQUEST_GUILD_PRIV					= 49,
	HEADER_GD_MONEY_LOG								= 50,
	HEADER_GD_GUILD_DEPOSIT_MONEY					= 51,
	HEADER_GD_GUILD_WITHDRAW_MONEY					= 52,
	HEADER_GD_GUILD_WITHDRAW_MONEY_GIVE_REPLY		= 53,
	HEADER_GD_REQUEST_CHARACTER_PRIV				= 54,
	HEADER_GD_SET_EVENT_FLAG						= 55,
	HEADER_GD_PARTY_SET_MEMBER_LEVEL				= 56,
	HEADER_GD_GUILD_WAR_BET							= 57,

	HEADER_GD_CREATE_OBJECT							= 60,
	HEADER_GD_DELETE_OBJECT							= 61,
	HEADER_GD_UPDATE_LAND							= 62,
	HEADER_GD_MARRIAGE_ADD							= 70,
	HEADER_GD_MARRIAGE_UPDATE						= 71,
	HEADER_GD_MARRIAGE_REMOVE						= 72,
	HEADER_GD_WEDDING_REQUEST						= 73,
	HEADER_GD_WEDDING_READY							= 74,
	HEADER_GD_WEDDING_END							= 75,
	HEADER_GD_AUTH_LOGIN							= 100,
	HEADER_GD_LOGIN_BY_KEY							= 101,
	HEADER_GD_MALL_LOAD								= 107,
	HEADER_GD_MYSHOP_PRICELIST_UPDATE				= 108,
	HEADER_GD_MYSHOP_PRICELIST_REQ					= 109,
	HEADER_GD_BLOCK_CHAT							= 110,
	HEADER_GD_RELOAD_ADMIN							= 115,
	HEADER_GD_BREAK_MARRIAGE						= 116,
	HEADER_GD_REQ_CHANGE_GUILD_MASTER				= 129,
	HEADER_GD_REQ_SPARE_ITEM_ID_RANGE				= 130,
	HEADER_GD_UPDATE_HORSE_NAME						= 131,
	HEADER_GD_REQ_HORSE_NAME						= 132,
	HEADER_GD_DC									= 133,
	HEADER_GD_VALID_LOGOUT							= 134,
	HEADER_GD_REQUEST_CHARGE_CASH					= 137,
	HEADER_GD_DELETE_AWARDID						= 138,
	HEADER_GD_UPDATE_CHANNELSTATUS					= 139,
	HEADER_GD_REQUEST_CHANNELSTATUS					= 140,

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	HEADER_GD_OFFLINESHOP							= 142,
#endif

#ifdef ENABLE_OFFLINE_MESSAGE
	HEADER_GD_REQUEST_OFFLINE_MESSAGES				= 143,
	HEADER_GD_SEND_OFFLINE_MESSAGE					= 144,
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	HEADER_GD_REQUEST_CHANGE_LANGUAGE				= 145,
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	HEADER_GD_SKILL_COLOR_SAVE						= 146,
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	HEADER_GD_GROWTH_PET_SAVE						= 147,
	HEADER_GD_GROWTH_PET_DELETE						= 148,
#endif

#ifdef ENABLE_RANKING
	// RANKING_SYSTEM
	HEADER_GD_ADD_RANKING,
	// END_OF_RANKING_SYSTEM
#endif

	HEADER_GD_SETUP									= 0xff,


	HEADER_DG_NOTICE								= 1,
	HEADER_DG_LOGIN_SUCCESS							= 30,
	HEADER_DG_LOGIN_NOT_EXIST						= 31,
	HEADER_DG_LOGIN_WRONG_PASSWD					= 33,
	HEADER_DG_LOGIN_ALREADY							= 34,
	HEADER_DG_PLAYER_LOAD_SUCCESS					= 35,
	HEADER_DG_PLAYER_LOAD_FAILED					= 36,
	HEADER_DG_PLAYER_CREATE_SUCCESS					= 37,
	HEADER_DG_PLAYER_CREATE_ALREADY					= 38,
	HEADER_DG_PLAYER_CREATE_FAILED					= 39,
	HEADER_DG_PLAYER_DELETE_SUCCESS					= 40,
	HEADER_DG_PLAYER_DELETE_FAILED					= 41,
	HEADER_DG_ITEM_LOAD								= 42,
	HEADER_DG_BOOT									= 43,
	HEADER_DG_QUEST_LOAD							= 44,
	HEADER_DG_SAFEBOX_LOAD							= 45,
	HEADER_DG_SAFEBOX_CHANGE_SIZE					= 46,
	HEADER_DG_SAFEBOX_WRONG_PASSWORD				= 47,
	HEADER_DG_SAFEBOX_CHANGE_PASSWORD_ANSWER		= 48,
	HEADER_DG_EMPIRE_SELECT							= 49,
	HEADER_DG_AFFECT_LOAD							= 50,
	HEADER_DG_MALL_LOAD								= 51,
	HEADER_DG_DIRECT_ENTER							= 55,
	HEADER_DG_GUILD_SKILL_UPDATE					= 56,
	HEADER_DG_GUILD_SKILL_RECHARGE					= 57,
	HEADER_DG_GUILD_EXP_UPDATE						= 58,
	HEADER_DG_PARTY_CREATE							= 59,
	HEADER_DG_PARTY_DELETE							= 60,
	HEADER_DG_PARTY_ADD								= 61,
	HEADER_DG_PARTY_REMOVE							= 62,
	HEADER_DG_PARTY_STATE_CHANGE					= 63,
	HEADER_DG_PARTY_HEAL_USE						= 64,
	HEADER_DG_PARTY_SET_MEMBER_LEVEL				= 65,

#ifdef ENABLE_ITEMSHOP
	HEADER_DG_ITEMSHOP 								= 76,
	HEADER_GD_ITEMSHOP 								= 76,
#endif
	HEADER_DG_TIME									= 90,
	HEADER_DG_ITEM_ID_RANGE							= 91,
	HEADER_DG_GUILD_ADD_MEMBER						= 92,
	HEADER_DG_GUILD_REMOVE_MEMBER					= 93,
	HEADER_DG_GUILD_CHANGE_GRADE					= 94,
	HEADER_DG_GUILD_CHANGE_MEMBER_DATA				= 95,
	HEADER_DG_GUILD_DISBAND							= 96,
	HEADER_DG_GUILD_WAR								= 97,
	HEADER_DG_GUILD_WAR_SCORE						= 98,
	HEADER_DG_GUILD_TIME_UPDATE						= 99,
	HEADER_DG_GUILD_LOAD							= 100,
	HEADER_DG_GUILD_LADDER							= 101,
	HEADER_DG_GUILD_SKILL_USABLE_CHANGE				= 102,
	HEADER_DG_GUILD_MONEY_CHANGE					= 103,
	HEADER_DG_GUILD_WITHDRAW_MONEY_GIVE				= 104,
	HEADER_DG_SET_EVENT_FLAG						= 105,
	HEADER_DG_GUILD_WAR_RESERVE_ADD					= 106,
	HEADER_DG_GUILD_WAR_RESERVE_DEL					= 107,
	HEADER_DG_GUILD_WAR_BET							= 108,
	HEADER_DG_RELOAD_PROTO							= 120,
	HEADER_DG_CHANGE_NAME							= 121,
	HEADER_DG_AUTH_LOGIN							= 122,
	HEADER_DG_CHANGE_EMPIRE_PRIV					= 124,
	HEADER_DG_CHANGE_GUILD_PRIV						= 125,
	HEADER_DG_MONEY_LOG								= 126,
	HEADER_DG_CHANGE_CHARACTER_PRIV					= 127,
	HEADER_DG_CREATE_OBJECT							= 140,
	HEADER_DG_DELETE_OBJECT							= 141,
	HEADER_DG_UPDATE_LAND							= 142,
	HEADER_DG_MARRIAGE_ADD							= 150,
	HEADER_DG_MARRIAGE_UPDATE						= 151,
	HEADER_DG_MARRIAGE_REMOVE						= 152,
	HEADER_DG_WEDDING_REQUEST						= 153,
	HEADER_DG_WEDDING_READY							= 154,
	HEADER_DG_WEDDING_START							= 155,
	HEADER_DG_WEDDING_END							= 156,
	HEADER_DG_MYSHOP_PRICELIST_RES					= 157,
	HEADER_DG_RELOAD_ADMIN							= 158,
	HEADER_DG_BREAK_MARRIAGE						= 159,
	HEADER_DG_ACK_CHANGE_GUILD_MASTER				= 173,
	HEADER_DG_ACK_SPARE_ITEM_ID_RANGE				= 174,
	HEADER_DG_UPDATE_HORSE_NAME 					= 175,
	HEADER_DG_ACK_HORSE_NAME						= 176,
	HEADER_DG_NEED_LOGIN_LOG						= 177,
	HEADER_DG_RESULT_CHARGE_CASH					= 179,
	HEADER_DG_ITEMAWARD_INFORMER					= 180,
	HEADER_DG_RESPOND_CHANNELSTATUS					= 181,
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	HEADER_DG_OFFLINESHOP							= 182,
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	HEADER_DG_SKILL_COLOR_LOAD						= 183,
#endif
#ifdef ENABLE_OFFLINE_MESSAGE
	HEADER_DG_RESPOND_OFFLINE_MESSAGES				= 184,
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	HEADER_DG_GROWTH_PET_LOAD						= 185,
#endif

	HEADER_DG_MAP_LOCATIONS							= 0xfe,
	HEADER_DG_P2P									= 0xff,
};

#pragma pack(1)
enum ERequestChargeType
{
	ERequestCharge_Cash = 0,
	ERequestCharge_Mileage,
};

typedef struct SRequestChargeCash
{
	DWORD dwAID;
	DWORD dwAmount;
	ERequestChargeType eChargeType;

} TRequestChargeCash;

typedef struct SSimplePlayer
{
	DWORD dwID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE byJob;
#ifdef ENABLE_LEVEL_INT
	int byLevel;
#else
	BYTE byLevel;
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
	BYTE byConquerorLevel;
	BYTE bySungmaST, bySungmaHP, bySungmaMV, bySungmaINM;
#endif
	DWORD dwPlayMinutes;
	BYTE byST, byHT, byDX, byIQ;
	WORD wMainPart;
	BYTE bChangeName;
	WORD wHairPart;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	WORD wAccePart;
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	WORD wAuraPart;
#endif
	WORD wMapIndex;
	BYTE bDummy[4];
	int32_t x, y;
	int32_t lAddr;
	WORD wPort;
	BYTE skill_group;
	DWORD last_play;
} TSimplePlayer;

typedef struct SAccountTable
{
	DWORD id;
	char login[LOGIN_MAX_LEN + 1];
	char passwd[PASSWD_MAX_LEN + 1];
	char social_id[SOCIAL_ID_MAX_LEN + 1];
	char status[ACCOUNT_STATUS_MAX_LEN + 1];
	BYTE bEmpire;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
#endif
	TSimplePlayer players[PLAYER_PER_ACCOUNT];
} TAccountTable;

typedef struct SPacketDGCreateSuccess
{
	BYTE bAccountCharacterIndex;
	TSimplePlayer player;
} TPacketDGCreateSuccess;

typedef struct TPlayerItemAttribute
{
	WORD bType;
	short sValue;
} TPlayerItemAttribute;

typedef struct SPlayerItem
{
	DWORD id;
	DWORD owner;
	BYTE window;
	WORD pos;
	DWORD count;

	DWORD vnum;
	int32_t alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_SET_ITEM
	DWORD dwSetValue;
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
	long sealbind;
#endif
} TPlayerItem;

typedef struct SQuickslot
{
	BYTE type;
	BYTE pos;
} TQuickslot;

#ifdef ENABLE_FISH_EVENT
typedef struct SPlayerFishEventSlot
{
	bool	bIsMain;
	BYTE	bShape;
} TPlayerFishEventSlot;
#endif

typedef struct SPlayerSkill
{
	BYTE bMasterType;
	BYTE bLevel;
	uint32_t tNextRead;
} TPlayerSkill;

struct	THorseInfo
{
	BYTE bLevel;
	BYTE bRiding;
	short sStamina;
	short sHealth;
	DWORD dwHorseHealthDropTime;
};

typedef struct SPlayerTable
{
	DWORD id;

	char name[CHARACTER_NAME_MAX_LEN + 1];
	char ip[IP_ADDRESS_LENGTH + 1];

	WORD job;
	BYTE voice;

#ifdef ENABLE_LEVEL_INT
	int level;
#else
	BYTE level;
#endif
	BYTE level_step;
	short st, ht, dx, iq;

	DWORD exp;

#ifdef ENABLE_GOLD_LIMIT
	long long gold;
#else
	INT gold;
#endif

#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
	int halounlv;
	long long phaloun;
	int rhaloun;
#endif

	BYTE dir;
	INT x, y, z;
	INT lMapIndex;

	int32_t lExitX, lExitY;
	int32_t lExitMapIndex;

	// @fixme301
#ifdef ENABLE_WARP_HP_LOSS_FIX
	int hp;
	int sp;
#else
	short hp;
	short sp;
#endif

	int sRandomHP;
	int sRandomSP;

	int playtime;

	short stat_point;
	short skill_point;
	short sub_skill_point;
	short horse_skill_point;

	TPlayerSkill skills[SKILL_MAX_NUM];

	TQuickslot quickslot[QUICKSLOT_MAX_NUM];

	BYTE part_base;
	WORD parts[PART_MAX_NUM];

	short stamina;

	BYTE skill_group;
	int32_t lAlignment;
#ifdef ENABLE_CONQUEROR_LEVEL
	BYTE	conquerorlevel;
	BYTE	conqueror_level_step;
	short	sungma_str, sungma_hp, sungma_move, sungma_immune;
	DWORD	conqueror_exp;
	short	conqueror_point;
#endif
	short stat_reset_count;

	THorseInfo horse;

	DWORD logoff_interval;
	DWORD last_play;

	int aiPremiumTimes[PREMIUM_MAX_NUM];

#ifdef ENABLE_ANTI_EXP
	bool anti_exp;
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	unsigned long long shopFlag;
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	DWORD dwPickUPMode;
#endif

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	DWORD inventory_unlock[UNLOCK_INVENTORY_MAX];
#endif

#ifdef ENABLE_RIDING_EXTENDED
	uint32_t mount_up_grade_exp;
	uint8_t mount_up_grade_fail;
#endif

#ifdef ENABLE_FISH_EVENT
	DWORD	fishEventUseCount;
	TPlayerFishEventSlot fishSlots[FISH_EVENT_SLOTS_NUM];
#endif

#ifdef ENABLE_GAYA_SYSTEM
	INT gem;
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	INT		soul;
	INT		soulre;
#endif
#ifdef ENABLE_KILL_STATISTICS
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
#endif
#ifdef ENABLE_RANKING
	long long	lRankPoints[RANKING_MAX_CATEGORIES];
#endif
} TPlayerTable;

typedef struct SMobSkillLevel
{
	DWORD	dwVnum;
	BYTE	bLevel;
} TMobSkillLevel;

typedef struct SEntityTable
{
	DWORD dwVnum;
} TEntityTable;

typedef struct SMobTable : public SEntityTable
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	char szLocaleName[CHARACTER_NAME_MAX_LEN + 1];

	BYTE bType; // Monster, NPC
	BYTE bRank; // PAWN, KNIGHT, KING
	BYTE bBattleType; // MELEE, etc..
	BYTE bLevel; // Level
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

typedef struct SSkillTable
{
	DWORD dwVnum;
	char szName[32 + 1];
	BYTE bType;
	BYTE bMaxLevel;
	DWORD dwSplashRange;

	char szPointOn[64];
	char szPointPoly[100 + 1];
	char szSPCostPoly[100 + 1];
	char szDurationPoly[100 + 1];
	char szDurationSPCostPoly[100 + 1];
	char szCooldownPoly[100 + 1];
	char szMasterBonusPoly[100 + 1];
	//char szAttackGradePoly[100 + 1];
	char szGrandMasterAddSPCostPoly[100 + 1];
	DWORD dwFlag;
	DWORD dwAffectFlag;

	// Data for secondary skill
	char szPointOn2[64];
	char szPointPoly2[100 + 1];
	char szDurationPoly2[100 + 1];
	DWORD dwAffectFlag2;

	// Data for grand master point
	char szPointOn3[64];
	char szPointPoly3[100 + 1];
	char szDurationPoly3[100 + 1];

	BYTE bLevelStep;
	BYTE bLevelLimit;
	DWORD preSkillVnum;
	BYTE preSkillLevel;

	int32_t lMaxHit;
	char szSplashAroundDamageAdjustPoly[100 + 1];

	BYTE bSkillAttrType;

	DWORD dwTargetRange;
} TSkillTable;

#ifdef ENABLE_RENEWAL_SHOPEX
enum STableExTypes
{
	SHOPEX_GOLD = 1,
	SHOPEX_COINS,
	SHOPEX_ITEM,
	SHOPEX_EXP,
	SHOPEX_MAX,
};
#endif

typedef struct SShopItemTable
{
	DWORD vnum;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif

	TItemPos pos; // PC
#ifdef ENABLE_GOLD_LIMIT
	long long price;
#else
	DWORD price; // PC, shop_table_ex.txt
#endif
	BYTE display_pos; // PC, shop_table_ex.txt

#ifdef ENABLE_RENEWAL_SHOPEX
	int32_t alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];

	BYTE price_type;
	DWORD price_vnum;

	SShopItemTable() : price_type(SHOPEX_GOLD), price_vnum(0)
	{
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttr, 0, sizeof(aAttr));
	}
#endif
} TShopItemTable;

typedef struct SShopTable
{
	DWORD dwVnum;
	DWORD dwNPCVnum;

#ifdef ENABLE_STACK_LIMIT
	WORD byItemCount;
#else
	BYTE byItemCount;
#endif
	TShopItemTable items[SHOP_HOST_ITEM_MAX_NUM];

#ifdef ENABLE_RENEWAL_SHOPEX
	char szShopName[SHOP_TAB_NAME_MAX + 1];
#endif
} TShopTable;

#define QUEST_NAME_MAX_LEN	32
#define QUEST_STATE_MAX_LEN	64

typedef struct SQuestTable
{
	DWORD dwPID;
	char szName[QUEST_NAME_MAX_LEN + 1];
	char szState[QUEST_STATE_MAX_LEN + 1];
	int32_t lValue;
} TQuestTable;

typedef struct SItemLimit
{
	BYTE bType;
	int32_t lValue;
} TItemLimit;

typedef struct SItemApply
{
	WORD bType;
	int32_t lValue;
} TItemApply;

typedef struct SItemTable : public SEntityTable
{
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

#ifdef ENABLE_GOLD_LIMIT
	long long dwGold;
	long long dwShopBuyPrice;
#else
	DWORD dwGold;
	DWORD dwShopBuyPrice;
#endif

	TItemLimit aLimits[ITEM_LIMIT_MAX_NUM];
	TItemApply aApplies[ITEM_APPLY_MAX_NUM];
	int32_t alValues[ITEM_VALUES_MAX_NUM];
	int32_t alSockets[ITEM_SOCKET_MAX_NUM];
	DWORD dwRefinedVnum;
	WORD wRefineSet;
	BYTE bAlterToMagicItemPct;
	BYTE bSpecular;
	BYTE bGainSocketPct;

	short int sAddonType;

	char cLimitRealTimeFirstUseIndex;
	char cLimitTimerBasedOnWearIndex;

} TItemTable;

struct TItemAttrTable
{
	TItemAttrTable() :
		dwApplyIndex(0),
		dwProb(0)
	{
		szApply[0] = 0;
		memset(&lValues, 0, sizeof(lValues));
		memset(&bMaxLevelBySet, 0, sizeof(bMaxLevelBySet));
	}

	char szApply[APPLY_NAME_MAX_LEN + 1];
	DWORD dwApplyIndex;
	DWORD dwProb;
	int32_t lValues[5];
	BYTE bMaxLevelBySet[ATTRIBUTE_SET_MAX_NUM];
};

typedef struct SConnectTable
{
	char login[LOGIN_MAX_LEN + 1];
	IDENT ident;
} TConnectTable;

typedef struct SLoginPacket
{
	char login[LOGIN_MAX_LEN + 1];
	char passwd[PASSWD_MAX_LEN + 1];
} TLoginPacket;

typedef struct SPlayerLoadPacket
{
	DWORD account_id;
	DWORD player_id;
	BYTE account_index;
} TPlayerLoadPacket;

typedef struct SPlayerCreatePacket
{
	char login[LOGIN_MAX_LEN + 1];
	char passwd[PASSWD_MAX_LEN + 1];
	DWORD account_id;
	BYTE account_index;
	TPlayerTable player_table;
} TPlayerCreatePacket;

typedef struct SPlayerDeletePacket
{
	char login[LOGIN_MAX_LEN + 1];
	DWORD player_id;
	BYTE account_index;
	char private_code[8];
} TPlayerDeletePacket;

typedef struct SLogoutPacket
{
	char login[LOGIN_MAX_LEN + 1];
	char passwd[PASSWD_MAX_LEN + 1];
} TLogoutPacket;

typedef struct SPlayerCountPacket
{
	DWORD dwCount;
} TPlayerCountPacket;

#ifndef ENABLE_EXTENDED_SAFEBOX
	#define SAFEBOX_MAX_NUM 135
#endif

#define SAFEBOX_PASSWORD_MAX_LEN 6

typedef struct SSafeboxTable
{
	DWORD dwID;
	BYTE bSize;
	DWORD dwGold;
	WORD wItemCount;
} TSafeboxTable;

typedef struct SSafeboxChangeSizePacket
{
	DWORD dwID;
	BYTE bSize;
} TSafeboxChangeSizePacket;

typedef struct SSafeboxLoadPacket
{
	DWORD dwID;
	char szLogin[LOGIN_MAX_LEN + 1];
	char szPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
} TSafeboxLoadPacket;

typedef struct SSafeboxChangePasswordPacket
{
	DWORD dwID;
	char szOldPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
	char szNewPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
} TSafeboxChangePasswordPacket;

typedef struct SSafeboxChangePasswordPacketAnswer
{
	BYTE flag;
} TSafeboxChangePasswordPacketAnswer;

typedef struct SEmpireSelectPacket
{
	DWORD dwAccountID;
	BYTE bEmpire;
} TEmpireSelectPacket;

typedef struct SPacketGDSetup
{
	char szPublicIP[16]; // Public IP which listen to users
	BYTE bChannel;
	WORD wListenPort;
	WORD wP2PPort;
	int32_t alMaps[64];
	DWORD dwLoginCount;
	BYTE bAuthServer;
} TPacketGDSetup;

typedef struct SPacketDGMapLocations
{
	BYTE bCount;
} TPacketDGMapLocations;

typedef struct SMapLocation
{
	int32_t alMaps[64];
	char szHost[MAX_HOST_LENGTH + 1];
	WORD wPort;
} TMapLocation;

typedef struct SPacketDGP2P
{
	char szHost[MAX_HOST_LENGTH + 1];
	WORD wPort;
	BYTE bChannel;
} TPacketDGP2P;

typedef struct SPacketGDDirectEnter
{
	char login[LOGIN_MAX_LEN + 1];
	char passwd[PASSWD_MAX_LEN + 1];
	BYTE index;
} TPacketGDDirectEnter;

typedef struct SPacketDGDirectEnter
{
	TAccountTable accountTable;
	TPlayerTable playerTable;
} TPacketDGDirectEnter;

typedef struct SPacketGuildSkillUpdate
{
	DWORD guild_id;
	int amount;
	BYTE skill_levels[12];
	BYTE skill_point;
	BYTE save;
} TPacketGuildSkillUpdate;

typedef struct SPacketGuildExpUpdate
{
	DWORD guild_id;
	int amount;
} TPacketGuildExpUpdate;

typedef struct SPacketGuildChangeMemberData
{
	DWORD guild_id;
	DWORD pid;
	DWORD offer;
#ifdef ENABLE_LEVEL_INT
	int level;
#else
	BYTE level;
#endif
	BYTE grade;
} TPacketGuildChangeMemberData;

typedef struct SPacketDGLoginAlready
{
	char szLogin[LOGIN_MAX_LEN + 1];
} TPacketDGLoginAlready;

typedef struct TPacketAffectElement
{
	DWORD dwType;
	BYTE bApplyOn;
	int32_t lApplyValue;
	DWORD dwFlag;
	int32_t lDuration;
	int32_t lSPCost;
} TPacketAffectElement;

typedef struct SPacketGDAddAffect
{
	DWORD dwPID;
	TPacketAffectElement elem;
} TPacketGDAddAffect;

typedef struct SPacketGDRemoveAffect
{
	DWORD dwPID;
	DWORD dwType;
	BYTE bApplyOn;
} TPacketGDRemoveAffect;

typedef struct SPacketGDHighscore
{
	DWORD dwPID;
	int32_t lValue;
	char cDir;
	char szBoard[21];
} TPacketGDHighscore;

typedef struct SPacketPartyCreate
{
	DWORD dwLeaderPID;
} TPacketPartyCreate;

typedef struct SPacketPartyDelete
{
	DWORD dwLeaderPID;
} TPacketPartyDelete;

typedef struct SPacketPartyAdd
{
	DWORD dwLeaderPID;
	DWORD dwPID;
	BYTE bState;
} TPacketPartyAdd;

typedef struct SPacketPartyRemove
{
	DWORD dwLeaderPID;
	DWORD dwPID;
} TPacketPartyRemove;

typedef struct SPacketPartyStateChange
{
	DWORD dwLeaderPID;
	DWORD dwPID;
	BYTE bRole;
	BYTE bFlag;
} TPacketPartyStateChange;

typedef struct SPacketPartySetMemberLevel
{
	DWORD dwLeaderPID;
	DWORD dwPID;
#ifdef ENABLE_LEVEL_INT
	int bLevel;
#else
	BYTE bLevel;
#endif
} TPacketPartySetMemberLevel;

typedef struct SPacketGDBoot
{
	DWORD dwItemIDRange[2];
	char szIP[16];
} TPacketGDBoot;

typedef struct SPacketGuild
{
	DWORD dwGuild;
	DWORD dwInfo;
} TPacketGuild;

typedef struct SPacketGDGuildAddMember
{
	DWORD dwPID;
	DWORD dwGuild;
	BYTE bGrade;
} TPacketGDGuildAddMember;

typedef struct SPacketDGGuildMember
{
	DWORD dwPID;
	DWORD dwGuild;
	BYTE bGrade;
	BYTE isGeneral;
	BYTE bJob;
#ifdef ENABLE_LEVEL_INT
	int bLevel;
#else
	BYTE bLevel;
#endif
	DWORD dwOffer;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketDGGuildMember;

typedef struct SPacketGuildWar
{
	BYTE bType;
	BYTE bWar;
	DWORD dwGuildFrom;
	DWORD dwGuildTo;
	int32_t lWarPrice;
	int32_t lInitialScore;
} TPacketGuildWar;

typedef struct SPacketGuildWarScore
{
	DWORD dwGuildGainPoint;
	DWORD dwGuildOpponent;
	int32_t lScore;
	int32_t lBetScore;
} TPacketGuildWarScore;

typedef struct SRefineMaterial
{
	DWORD vnum;
	int count;
} TRefineMaterial;

typedef struct SRefineTable
{
	DWORD id;
#ifdef ENABLE_STACK_LIMIT
	WORD material_count;
#else
	BYTE material_count;
#endif
	int cost;
	int prob;
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TRefineTable;

typedef struct SBanwordTable
{
	char szWord[BANWORD_MAX_LEN + 1];
} TBanwordTable;

typedef struct SPacketGDChangeName
{
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGDChangeName;

typedef struct SPacketDGChangeName
{
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketDGChangeName;

typedef struct SPacketGuildLadder
{
	DWORD dwGuild;
	int32_t lLadderPoint;
	int32_t lWin;
	int32_t lDraw;
	int32_t lLoss;
} TPacketGuildLadder;

typedef struct SPacketGuildLadderPoint
{
	DWORD dwGuild;
	int32_t lChange;
} TPacketGuildLadderPoint;

typedef struct SPacketGuildUseSkill
{
	DWORD dwGuild;
	DWORD dwSkillVnum;
	DWORD dwCooltime;
} TPacketGuildUseSkill;

typedef struct SPacketGuildSkillUsableChange
{
	DWORD dwGuild;
	DWORD dwSkillVnum;
	BYTE bUsable;
} TPacketGuildSkillUsableChange;

typedef struct SPacketGDLoginKey
{
	DWORD dwAccountID;
	DWORD dwLoginKey;
} TPacketGDLoginKey;

typedef struct SPacketGDAuthLogin
{
	DWORD dwID;
	DWORD dwLoginKey;
	char szLogin[LOGIN_MAX_LEN + 1];
	char szSocialID[SOCIAL_ID_MAX_LEN + 1];
	DWORD adwClientKey[4];
	int iPremiumTimes[PREMIUM_MAX_NUM];
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
#endif
} TPacketGDAuthLogin;

typedef struct SPacketGDLoginByKey
{
	char szLogin[LOGIN_MAX_LEN + 1];
	DWORD dwLoginKey;
	DWORD adwClientKey[4];
	char szIP[MAX_HOST_LENGTH + 1];
} TPacketGDLoginByKey;

typedef struct SPacketGiveGuildPriv
{
	BYTE type;
	int value;
	DWORD guild_id;
	time_t duration_sec;
} TPacketGiveGuildPriv;

typedef struct SPacketGiveEmpirePriv
{
	BYTE type;
	int value;
	BYTE empire;
	time_t duration_sec;
} TPacketGiveEmpirePriv;

typedef struct SPacketGiveCharacterPriv
{
	BYTE type;
	int value;
	DWORD pid;
} TPacketGiveCharacterPriv;

typedef struct SPacketRemoveGuildPriv
{
	BYTE type;
	DWORD guild_id;
} TPacketRemoveGuildPriv;

typedef struct SPacketRemoveEmpirePriv
{
	BYTE type;
	BYTE empire;
} TPacketRemoveEmpirePriv;

typedef struct SPacketDGChangeCharacterPriv
{
	BYTE type;
	int value;
	DWORD pid;
	BYTE bLog;
} TPacketDGChangeCharacterPriv;

typedef struct SPacketDGChangeGuildPriv
{
	BYTE type;
	int value;
	DWORD guild_id;
	BYTE bLog;
	time_t end_time_sec;
} TPacketDGChangeGuildPriv;

typedef struct SPacketDGChangeEmpirePriv
{
	BYTE type;
	int value;
	BYTE empire;
	BYTE bLog;
	time_t end_time_sec;
} TPacketDGChangeEmpirePriv;

typedef struct SPacketMoneyLog
{
	BYTE type;
	DWORD vnum;
#ifdef ENABLE_GOLD_LIMIT
	long long gold;
#else
	INT gold;
#endif
} TPacketMoneyLog;

typedef struct SPacketGDGuildMoney
{
	DWORD dwGuild;
	INT iGold;
} TPacketGDGuildMoney;

typedef struct SPacketDGGuildMoneyChange
{
	DWORD dwGuild;
	INT iTotalGold;
} TPacketDGGuildMoneyChange;

typedef struct SPacketDGGuildMoneyWithdraw
{
	DWORD dwGuild;
	INT iChangeGold;
} TPacketDGGuildMoneyWithdraw;

typedef struct SPacketGDGuildMoneyWithdrawGiveReply
{
	DWORD dwGuild;
	INT iChangeGold;
	BYTE bGiveSuccess;
} TPacketGDGuildMoneyWithdrawGiveReply;

typedef struct SPacketSetEventFlag
{
	char szFlagName[EVENT_FLAG_NAME_MAX_LEN + 1];
	int32_t lValue;
} TPacketSetEventFlag;

typedef struct SPacketLoginOnSetup
{
	DWORD dwID;
	char szLogin[LOGIN_MAX_LEN + 1];
	char szSocialID[SOCIAL_ID_MAX_LEN + 1];
	char szHost[MAX_HOST_LENGTH + 1];
	DWORD dwLoginKey;
	DWORD adwClientKey[4];
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
#endif
} TPacketLoginOnSetup;

typedef struct SPacketGDCreateObject
{
	DWORD dwVnum;
	DWORD dwLandID;
	INT lMapIndex;
	INT x, y;
	float xRot;
	float yRot;
	float zRot;
} TPacketGDCreateObject;

typedef struct SGuildReserve
{
	DWORD dwID;
	DWORD dwGuildFrom;
	DWORD dwGuildTo;
	DWORD dwTime;
	BYTE bType;
	int32_t lWarPrice;
	int32_t lInitialScore;
	bool bStarted;
	DWORD dwBetFrom;
	DWORD dwBetTo;
	int32_t lPowerFrom;
	int32_t lPowerTo;
	int32_t lHandicap;
} TGuildWarReserve;

typedef struct
{
	DWORD dwWarID;
	char szLogin[LOGIN_MAX_LEN + 1];
	DWORD dwGold;
	DWORD dwGuild;
} TPacketGDGuildWarBet;

// Marriage
typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
	time_t tMarryTime;
	char szName1[CHARACTER_NAME_MAX_LEN + 1];
	char szName2[CHARACTER_NAME_MAX_LEN + 1];
} TPacketMarriageAdd;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
	INT iLovePoint;
	BYTE byMarried;
} TPacketMarriageUpdate;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketMarriageRemove;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingRequest;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
	DWORD dwMapIndex;
} TPacketWeddingReady;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingStart;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingEnd;

typedef struct SPacketMyshopPricelistHeader
{
	DWORD dwOwnerID;
	BYTE byCount;
} TPacketMyshopPricelistHeader;

typedef struct SItemPriceInfo
{
	DWORD dwVnum;
#ifdef ENABLE_GOLD_LIMIT
	long long dwPrice;
#else
	DWORD dwPrice;
#endif
} TItemPriceInfo;

typedef struct SItemPriceListTable
{
	DWORD dwOwnerID;
	BYTE byCount;

	TItemPriceInfo aPriceInfo[SHOP_PRICELIST_MAX_NUM];
} TItemPriceListTable;

typedef struct
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	int32_t lDuration;
} TPacketBlockChat;

typedef struct TAdminInfo
{
	int m_ID;
	char m_szAccount[32];
	char m_szName[32];
	char m_szContactIP[16];
	char m_szServerIP[16];
	int m_Authority;
} tAdminInfo;

struct tLocale
{
	char szValue[32];
	char szKey[32];
};

typedef struct SPacketReloadAdmin
{
	char szIP[16];
} TPacketReloadAdmin;

typedef struct tChangeGuildMaster
{
	DWORD dwGuildID;
	DWORD idFrom;
	DWORD idTo;
} TPacketChangeGuildMaster;

typedef struct tItemIDRange
{
	DWORD dwMin;
	DWORD dwMax;
	DWORD dwUsableItemIDMin;
} TItemIDRangeTable;

typedef struct tUpdateHorseName
{
	DWORD dwPlayerID;
	char szHorseName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketUpdateHorseName;

typedef struct tDC
{
	char login[LOGIN_MAX_LEN + 1];
} TPacketDC;

typedef struct tNeedLoginLogInfo
{
	DWORD dwPlayerID;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
#endif
} TPacketNeedLoginLogInfo;

typedef struct tItemAwardInformer
{
	char login[LOGIN_MAX_LEN + 1];
	char command[20];
	unsigned int vnum;
} TPacketItemAwardInfromer;

typedef struct tDeleteAwardID
{
	DWORD dwID;
} TPacketDeleteAwardID;

#ifdef ENABLE_AUTO_SELL_SYSTEM
typedef struct SAutoSellTable
{
	DWORD	dwVnum;
} TAutoSellTable;
#endif

typedef struct SChannelStatus
{
	int nPort;
	BYTE bStatus;
} TChannelStatus;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
typedef struct SRequestChangeLanguage
{
	DWORD dwAID;
	BYTE bLanguage;
} TRequestChangeLanguage;
#endif

#ifdef ENABLE_MESSENGER_BLOCK
enum MessengerBlock
{
	MESSENGER_BLOCK,
	MESSENGER_FRIEND
};
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
struct TSwitchbotAttributeAlternativeTable
{
	TPlayerItemAttribute attributes[MAX_NORM_ATTR_NUM];

	bool IsConfigured() const
	{
		for (const auto& it : attributes)
		{
			if (it.bType && it.sValue)
			{
				return true;
			}
		}
		return false;
	}
};

struct TSwitchbotTable
{
	DWORD player_id;
	bool active[SWITCHBOT_SLOT_COUNT];
	bool finished[SWITCHBOT_SLOT_COUNT];
	DWORD items[SWITCHBOT_SLOT_COUNT];
	TSwitchbotAttributeAlternativeTable alternatives[SWITCHBOT_SLOT_COUNT][SWITCHBOT_ALTERNATIVE_COUNT];

	TSwitchbotTable() : player_id(0)
	{
		memset(&items, 0, sizeof(items));
		memset(&alternatives, 0, sizeof(alternatives));
		memset(&active, false, sizeof(active));
		memset(&finished, false, sizeof(finished));
	}
};

struct TSwitchbottAttributeTable
{
	BYTE attribute_set;
	int apply_num;
	int32_t max_value;
};
#endif

#ifdef ENABLE_DUNGEON_INFO
typedef struct SFDateInfoPacket
{
	int 	id_dungeon;
	char 	name[100];
	char 	img[100];
	int 	lv_min;
	int 	lv_max;
	int 	party_max;
	DWORD 	respawn;
	DWORD 	time_room;
	int   	time_respawn_dungeon;
	int 	time_room_dungeon;
	char 	entrance[100];
	char 	resistance[100];
	char 	force[100];
	int 	vnum_item;
	int 	count_item;
	int 	count_finish;
	int 	time_finish;
	int 	difficulty;
	int 	damage_done;
	int 	damage_received;
	int 	id_boss;


}TFDateInfoPacket;

typedef struct SFDateMision
{
	
	int 	vnum_mob_mision;
	int 	count_mob_mision;
	int 	count_mob_a_mision;
	int 	status_mision;
}TFDateMision;

typedef struct SFDateInfo
{
	int 	id;
	SFDateInfoPacket dateinfo;
	int 	index_map;
	int 	x_dungeon_join;
	int 	y_dungeon_join;
	int 	x_map_npc;
	int 	y_map_npc;
	SFDateMision	dateinfom;
}TFDateInfo;

typedef struct  SFDateRanking
{
	int vnum_mob_r;
	int type;
	char name_r[24];
	int extra_r;

}TFDateInfoRanking;

#endif

#ifdef ENABLE_ITEMSHOP
enum EItemShopSubheaders : uint8_t
{
	ITEMSHOP_LOAD			= 0,
	ITEMSHOP_LOG			= 1,
	ITEMSHOP_BUY			= 2,
	ITEMSHOP_DRAGONCOIN		= 3,
	ITEMSHOP_RELOAD			= 4,
	ITEMSHOP_UPDATE_ITEM	= 5,
};

typedef struct SIShopData
{
	DWORD		id;
	DWORD		itemVnum;
	long long	itemPrice;
#ifdef USE_ITEMSHOP_RENEWED
	long long	itemPriceJD;
#endif
	int			topSellingIndex;
	BYTE		discount;
	int			offerTime;
	int			addedTime;
	long long	sellCount;
	int			week_limit;
	int			month_limit;
	int 		maxSellCount;
} TIShopData;

typedef struct SIShopLogData
{
	DWORD		accountID;
	char		playerName[CHARACTER_NAME_MAX_LEN+1];
	char		buyDate[21];
	int			buyTime;
	char		ipAdress[16];
	DWORD		itemID;
	DWORD		itemVnum;
	int			itemCount;
	long long	itemPrice;
#ifdef USE_ITEMSHOP_RENEWED
	long long	itemPriceJD;
#endif
} TIShopLogData;
#endif
#ifdef ENABLE_MULTI_FARM_BLOCK
typedef struct SMultiFarm
{
	DWORD playerID;
	bool farmStatus;
	BYTE affectType;
	int affectTime;
	char playerName[CHARACTER_NAME_MAX_LEN+1];
	SMultiFarm(DWORD id_, const char* playerName_, bool status_, BYTE type_, int time_) : playerID(id_), farmStatus(status_), affectType(type_), affectTime(time_){
		strlcpy(playerName, playerName_, sizeof(playerName));
	}
}TMultiFarm;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
typedef struct
{
	DWORD dwPlayerID;
	DWORD dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
} TSkillColor;
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
typedef struct offline_shop_item
{
	DWORD id;
	DWORD owner_id;
	BYTE pos;
	int count;
	long long price;
	DWORD vnum;
	int32_t alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	BYTE status;
	char szBuyerName[CHARACTER_NAME_MAX_LEN+1];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD transmutation;
#endif
	char szOwnerName[CHARACTER_NAME_MAX_LEN+1];
} OFFLINE_SHOP_ITEM;

typedef struct ShopLog
{
	char name[CHARACTER_NAME_MAX_LEN+1];
	char date[25];
	DWORD itemVnum;
	DWORD itemCount;
	long long price;
} TShopLog;

typedef struct SShopDecoration
{
	char sign[SHOP_SIGN_MAX_LEN + 1];
	DWORD owner_id;
	BYTE vnum;
	BYTE type;
} TShopDecoration;

typedef struct SGDOfflineShopCreate
{
	DWORD owner_id;
	char owner_name[CHARACTER_NAME_MAX_LEN+1];
	char sign[SHOP_SIGN_MAX_LEN + 1];
	int32_t x;
	int32_t y;
	int32_t z;
	DWORD mapindex;
	DWORD type;
	BYTE channel;
	unsigned long long slotflag;
	OFFLINE_SHOP_ITEM items[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
	long long price;
	ShopLog log[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
	int time;
} TOfflineShop;

typedef struct SOfflineShopBuy
{
	OFFLINE_SHOP_ITEM item;
	DWORD customer_id;
	char customer_name[CHARACTER_NAME_MAX_LEN + 1];
	ShopLog log;
	int log_index;
} TOfflineShopBuy;

typedef struct SOfflineShopChangeTitle
{
	DWORD owner_id;
	char sign[SHOP_SIGN_MAX_LEN + 1];
} TOfflineShopChangeTitle;

typedef struct SOfflineShopItemTable
{
	DWORD vnum;
	int count;
	TItemPos pos;
	long long price;
	BYTE display_pos;
} TOfflineShopItemTable;

typedef struct SOfflineShopAddItem
{
	TItemPos bPos;
	BYTE bDisplayPos;
	long long lPrice;
} TOfflineShopAddItem;

typedef struct SOfflineShopOpenSlot
{
	DWORD owner_id;
	unsigned long long flag;
} TOfflineShopOpenSlot;

typedef struct SOfflineShopBackItem
{
	DWORD owner_id;
	OFFLINE_SHOP_ITEM items[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
} TOfflineShopBackItem;

enum {
	CREATE_OFFLINESHOP,
	DESTROY_OFFLINESHOP,
	REMOVE_ITEM,
	ADD_ITEM,
	BUY_ITEM,
	WITHDRAW_MONEY,
	CHANGE_TITLE,
	CLEAR_LOG,
	CHANGE_DECORATION,
	CHANGE_OPEN_SLOT,
	TIME_DONE,
	GET_BACK_ITEM,
	ADD_TIME,
};

struct shop_decoration
{
	BYTE subheader;
	TShopDecoration decoration;
};

struct shop_create
{
	BYTE subheader;
	TOfflineShop offlineshop;
};

struct shop_buy
{
	BYTE subheader;
	TOfflineShopBuy buyItem;
};

struct shop_title
{
	BYTE subheader;
	TOfflineShopChangeTitle title;
};

struct shop_owner
{
	BYTE subheader;
	DWORD owner_id;
};

struct shop_item
{
	BYTE subheader;
	OFFLINE_SHOP_ITEM item;
};

struct shop_slot
{
	BYTE subheader;
	TOfflineShopOpenSlot ch;
};

struct shop_back
{
	BYTE subheader;
	TOfflineShopBackItem back;
};
#endif

#ifdef ENABLE_OFFLINE_MESSAGE
typedef struct SPacketGDReadOfflineMessage
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGDReadOfflineMessage;

typedef struct SPacketDGReadOfflineMessage
{
	char szFrom[CHARACTER_NAME_MAX_LEN + 1];
	char szMessage[CHAT_MAX_LEN + 1];
} TPacketDGReadOfflineMessage;

typedef struct SPacketGDSendOfflineMessage
{
	char szFrom[CHARACTER_NAME_MAX_LEN + 1];
	char szTo[CHARACTER_NAME_MAX_LEN + 1];
	char szMessage[CHAT_MAX_LEN + 1];
} TPacketGDSendOfflineMessage;
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
typedef struct SPetSkill
{
	bool bLocked;
	BYTE bSkill;
	BYTE bLevel;
	DWORD dwCooltime;

	SPetSkill()
	{
		bLocked = true;
		bSkill = 0;
		bLevel = 0;
		dwCooltime = 0;
	}
} TPetSkill;

typedef struct SGrowthPet
{
	DWORD dwID;
	DWORD dwOwner;
	DWORD dwVnum;
	BYTE bState;
	char szName[PET_NAME_MAX_SIZE + 1];
	BYTE bSize;

	DWORD dwLevel;
	BYTE bLevelStep;
	BYTE bEvolution;
	BYTE bType;
	DWORD dwHP;
	DWORD dwSP;
	DWORD dwDef;

	DWORD dwHPApply;
	DWORD dwSPApply;
	DWORD dwDefApply;
	DWORD dwAgeApply;

	TPetSkill aSkill[PET_SKILL_COUNT_MAX];

	DWORD lExp;
	DWORD lItemExp;

	time_t lBirthday;
	time_t lEndTime;
	time_t lMaxTime;
} TGrowthPet;

typedef struct SGrowthPetSkillTable
{
	DWORD dwPetVnum;
	DWORD dwSkillVnum;
	char szName[32 + 1];
	BYTE bType;
	DWORD dwCooldown;
	DWORD dwAffectFlag;

	char szPointOn[64];
	char szPointPoly1[100 + 1];
	char szPointPoly2[100 + 1];
	char szPointPoly3[100 + 1];
	char szPointPoly4[100 + 1];
	char szPointPoly5[100 + 1];
	char szPointPoly6[100 + 1];
	char szPointPoly7[100 + 1];
	char szPointPoly8[100 + 1];
	char szActivatePctPoly[100 + 1];
	char szDurationPoly[100 + 1];
} TGrowthPetSkillTable;
#endif

#ifdef __DUNGEON_INFO__
typedef struct SDungeonRank
{
	char name[CHARACTER_NAME_MAX_LEN + 1];
	BYTE level;
	int	value;
}TDungeonRank;
#endif

#ifdef ENABLE_RANKING
typedef struct SPacketGDAddRanking
{
	uint32_t		dwPID;
	uint8_t		bTypeRank;
	int64_t	lValue;
} TPacketGDAddRanking;
#endif

#pragma pack()
#endif
