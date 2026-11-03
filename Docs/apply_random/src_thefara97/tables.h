#ifndef __INC_TABLES_H__
#define __INC_TABLES_H__

#include <memory>

#include "length.h"
#include "item_length.h"
#include "CommonDefines.h"
#ifdef __PREMIUM_PRIVATE_SHOP__
#include <boost/functional/hash.hpp>
#endif
#include <string>
#include <vector>

typedef	DWORD IDENT;

enum
{
	HEADER_GD_LOGIN				= 1,
	HEADER_GD_LOGOUT			= 2,

	HEADER_GD_PLAYER_LOAD		= 3,
	HEADER_GD_PLAYER_SAVE		= 4,
	HEADER_GD_PLAYER_CREATE		= 5,
	HEADER_GD_PLAYER_DELETE		= 6,

	HEADER_GD_LOGIN_KEY			= 7,
	// 8 empty
	HEADER_GD_BOOT				= 9,
	HEADER_GD_PLAYER_COUNT		= 10,
	HEADER_GD_QUEST_SAVE		= 11,
	HEADER_GD_SAFEBOX_LOAD		= 12,
	HEADER_GD_SAFEBOX_SAVE		= 13,
	HEADER_GD_SAFEBOX_CHANGE_SIZE	= 14,
	HEADER_GD_EMPIRE_SELECT		= 15,

	HEADER_GD_SAFEBOX_CHANGE_PASSWORD		= 16,
	HEADER_GD_SAFEBOX_CHANGE_PASSWORD_SECOND	= 17, // Not really a packet, used internal
	HEADER_GD_DIRECT_ENTER		= 18,

	HEADER_GD_GUILD_SKILL_UPDATE	= 19,
	HEADER_GD_GUILD_EXP_UPDATE		= 20,
	HEADER_GD_GUILD_ADD_MEMBER		= 21,
	HEADER_GD_GUILD_REMOVE_MEMBER	= 22,
	HEADER_GD_GUILD_CHANGE_GRADE	= 23,
	HEADER_GD_GUILD_CHANGE_MEMBER_DATA	= 24,
	HEADER_GD_GUILD_DISBAND		= 25,
	HEADER_GD_GUILD_WAR			= 26,
	HEADER_GD_GUILD_WAR_SCORE		= 27,
	HEADER_GD_GUILD_CREATE		= 28,

	HEADER_GD_ITEM_SAVE			= 30,
	HEADER_GD_ITEM_DESTROY		= 31,

	HEADER_GD_ADD_AFFECT		= 32,
	HEADER_GD_REMOVE_AFFECT		= 33,

	HEADER_GD_HIGHSCORE_REGISTER	= 34,
	HEADER_GD_ITEM_FLUSH		= 35,

	HEADER_GD_PARTY_CREATE		= 36,
	HEADER_GD_PARTY_DELETE		= 37,
	HEADER_GD_PARTY_ADD			= 38,
	HEADER_GD_PARTY_REMOVE		= 39,
	HEADER_GD_PARTY_STATE_CHANGE	= 40,
	HEADER_GD_PARTY_HEAL_USE		= 41,

	HEADER_GD_FLUSH_CACHE		= 42,
	HEADER_GD_RELOAD_PROTO		= 43,

	HEADER_GD_CHANGE_NAME		= 44,
	HEADER_GD_SMS				= 45,

	HEADER_GD_GUILD_CHANGE_LADDER_POINT	= 46,
	HEADER_GD_GUILD_USE_SKILL		= 47,

	HEADER_GD_REQUEST_EMPIRE_PRIV	= 48,
	HEADER_GD_REQUEST_GUILD_PRIV	= 49,

	HEADER_GD_MONEY_LOG				= 50,

	HEADER_GD_GUILD_DEPOSIT_MONEY				= 51,
	HEADER_GD_GUILD_WITHDRAW_MONEY				= 52,
	HEADER_GD_GUILD_WITHDRAW_MONEY_GIVE_REPLY	= 53,

	HEADER_GD_REQUEST_CHARACTER_PRIV	= 54,

	HEADER_GD_SET_EVENT_FLAG			= 55,

	HEADER_GD_PARTY_SET_MEMBER_LEVEL	= 56,

	HEADER_GD_GUILD_WAR_BET		= 57,

	HEADER_GD_CREATE_OBJECT		= 60,
	HEADER_GD_DELETE_OBJECT		= 61,
	HEADER_GD_UPDATE_LAND		= 62,

	HEADER_GD_MARRIAGE_ADD		= 70,
	HEADER_GD_MARRIAGE_UPDATE	= 71,
	HEADER_GD_MARRIAGE_REMOVE	= 72,

	HEADER_GD_WEDDING_REQUEST	= 73,
	HEADER_GD_WEDDING_READY		= 74,
	HEADER_GD_WEDDING_END		= 75,

	HEADER_GD_GUILD_REFRESH_NOTE = 76,
	HEADER_GD_GUILD_NOTICE_DUNGEON_OPEN = 77,
	HEADER_GD_GUILD_NOTICE_DUNGEON_END = 78,
	HEADER_GD_GUILD_NOTICE_TICKETS_CHANGED = 79,
	HEADER_GD_GUILD_UPDATE_GUARD_ITEM_DATA = 80,
	HEADER_GD_GUILD_CHANGE_MONEY = 83,
	HEADER_GD_GUILD_UPDATE_GUARD_ACTIVE_SET_DATA = 84,
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
	HEADER_GD_IN_GAME_LOG		= 86,
#endif
#ifdef ENABLE_ITEMSHOP
	HEADER_GD_ITEMSHOP = 87,
#endif

	HEADER_GD_AUTH_LOGIN		= 100,
	HEADER_GD_LOGIN_BY_KEY		= 101,
	HEADER_GD_BILLING_EXPIRE	= 104,
	HEADER_GD_VCARD				= 105,
	HEADER_GD_BILLING_CHECK		= 106,
	HEADER_GD_MALL_LOAD			= 107,

	HEADER_GD_MYSHOP_PRICELIST_UPDATE	= 108,
	HEADER_GD_MYSHOP_PRICELIST_REQ		= 109,

	HEADER_GD_BLOCK_CHAT				= 110,

	HEADER_GD_RELOAD_ADMIN			= 115,
	HEADER_GD_BREAK_MARRIAGE		= 116,
	HEADER_GD_ELECT_MONARCH			= 117,
	HEADER_GD_CANDIDACY				= 118,
	HEADER_GD_ADD_MONARCH_MONEY		= 119,
	HEADER_GD_TAKE_MONARCH_MONEY	= 120,
	HEADER_GD_COME_TO_VOTE			= 121,
	HEADER_GD_RMCANDIDACY			= 122,
	HEADER_GD_SETMONARCH			= 123,
	HEADER_GD_RMMONARCH				= 124,
	HEADER_GD_DEC_MONARCH_MONEY		= 125,

	HEADER_GD_CHANGE_MONARCH_LORD	= 126,

	HEADER_GD_REQ_CHANGE_GUILD_MASTER	= 129,

	HEADER_GD_REQ_SPARE_ITEM_ID_RANGE	= 130,

	HEADER_GD_UPDATE_HORSE_NAME		= 131,
	HEADER_GD_REQ_HORSE_NAME		= 132,

	HEADER_GD_DC					= 133,

	HEADER_GD_VALID_LOGOUT			= 134,

	HEADER_GD_REQUEST_CHARGE_CASH	= 137,

	HEADER_GD_DELETE_AWARDID		= 138,	// delete gift notify icon

	HEADER_GD_UPDATE_CHANNELSTATUS	= 139,
	HEADER_GD_REQUEST_CHANNELSTATUS	= 140,

#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
	HEADER_GD_RELOAD_DAMAGE_RESTRICTION = 145,
#endif
#ifdef ENABLE_NEW_PET_SYSTEM
	HEADER_GD_UPDATE_PET_NAME = 150,
	HEADER_GD_REQ_PET_NAME = 151,
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_GD_PRIVATE_SHOP = 152,
#endif
#ifdef WEEKLY_RANK_BYLUZER
	HEADER_GD_WEEKLY_REWARDS = 153,
#endif

#if defined(BL_OFFLINE_MESSAGE)
	HEADER_GD_REQUEST_OFFLINE_MESSAGES = 154,
	HEADER_GD_SEND_OFFLINE_MESSAGE = 155,
#endif
#ifdef __DUNGEON_INFO_ENABLE__
	HEADER_GD_DUNGEONINFO_RANKING,
	HEADER_GD_DUNGEONINFO_RANKING_BOOT,
#endif
#ifdef ENABLE_EVENT_MANAGER
	HEADER_GD_EVENT_MANAGER = 212,
#endif

#ifdef ENABLE_OFFLINE_SHOP
	HEADER_GD_NEW_OFFLINESHOP				= 213,
#endif
#ifdef RANKING_SYSTEM
	HEADER_GD_SERVER_RANKING_LOAD			= 214,
	HEADER_GD_SERVER_RANKING_LOAD_MY_POS	= 215,
	HEADER_GD_RANKING_LOAD					= 216,
	HEADER_GD_RANKING_LOAD_MY_POS			= 217,
#endif
	HEADER_GD_FISH_VNUM_RANKING_LOAD		= 218,

#if defined(__BL_MOVE_CHANNEL__)
	HEADER_GD_MOVE_CHANNEL = 240,
#endif


	HEADER_GD_SETUP			= 0xff,

	///////////////////////////////////////////////
	HEADER_DG_NOTICE			= 1,

	HEADER_DG_LOGIN_SUCCESS			= 30,
	HEADER_DG_LOGIN_NOT_EXIST		= 31,
	HEADER_DG_LOGIN_WRONG_PASSWD	= 33,
	HEADER_DG_LOGIN_ALREADY			= 34,

	HEADER_DG_PLAYER_LOAD_SUCCESS	= 35,
	HEADER_DG_PLAYER_LOAD_FAILED	= 36,
	HEADER_DG_PLAYER_CREATE_SUCCESS	= 37,
	HEADER_DG_PLAYER_CREATE_ALREADY	= 38,
	HEADER_DG_PLAYER_CREATE_FAILED	= 39,
	HEADER_DG_PLAYER_DELETE_SUCCESS	= 40,
	HEADER_DG_PLAYER_DELETE_FAILED	= 41,

	HEADER_DG_ITEM_LOAD			= 42,

	HEADER_DG_BOOT				= 43,
	HEADER_DG_QUEST_LOAD		= 44,

	HEADER_DG_SAFEBOX_LOAD					= 45,
	HEADER_DG_SAFEBOX_CHANGE_SIZE			= 46,
	HEADER_DG_SAFEBOX_WRONG_PASSWORD		= 47,
	HEADER_DG_SAFEBOX_CHANGE_PASSWORD_ANSWER = 48,

	HEADER_DG_EMPIRE_SELECT		= 49,

	HEADER_DG_AFFECT_LOAD		= 50,
	HEADER_DG_MALL_LOAD			= 51,

	HEADER_DG_DIRECT_ENTER		= 55,

	HEADER_DG_GUILD_SKILL_UPDATE	= 56,
	HEADER_DG_GUILD_SKILL_RECHARGE	= 57,
	HEADER_DG_GUILD_EXP_UPDATE		= 58,

	HEADER_DG_PARTY_CREATE		= 59,
	HEADER_DG_PARTY_DELETE		= 60,
	HEADER_DG_PARTY_ADD			= 61,
	HEADER_DG_PARTY_REMOVE		= 62,
	HEADER_DG_PARTY_STATE_CHANGE	= 63,
	HEADER_DG_PARTY_HEAL_USE		= 64,
	HEADER_DG_PARTY_SET_MEMBER_LEVEL	= 65,
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
	HEADER_DG_IN_GAME_LOG		= 83,
#endif
#ifdef ENABLE_ITEMSHOP
	HEADER_DG_ITEMSHOP = 87,
#endif

	HEADER_DG_TIME			= 90,
	HEADER_DG_ITEM_ID_RANGE		= 91,

	HEADER_DG_GUILD_ADD_MEMBER		= 92,
	HEADER_DG_GUILD_REMOVE_MEMBER	= 93,
	HEADER_DG_GUILD_CHANGE_GRADE	= 94,
	HEADER_DG_GUILD_CHANGE_MEMBER_DATA	= 95,
	HEADER_DG_GUILD_DISBAND		= 96,
	HEADER_DG_GUILD_WAR			= 97,
	HEADER_DG_GUILD_WAR_SCORE		= 98,
	HEADER_DG_GUILD_TIME_UPDATE		= 99,
	HEADER_DG_GUILD_LOAD		= 100,
	HEADER_DG_GUILD_LADDER		= 101,
	HEADER_DG_GUILD_SKILL_USABLE_CHANGE	= 102,
	HEADER_DG_GUILD_MONEY_CHANGE	= 103,
	HEADER_DG_GUILD_WITHDRAW_MONEY_GIVE	= 104,

	HEADER_DG_SET_EVENT_FLAG		= 105,

	HEADER_DG_GUILD_WAR_RESERVE_ADD	= 106,
	HEADER_DG_GUILD_WAR_RESERVE_DEL	= 107,
	HEADER_DG_GUILD_WAR_BET		= 108,
	HEADER_DG_GUILD_REFRESH_NOTE	= 109,
	HEADER_DG_GUILD_NOTICE_DUNGEON_OPEN	= 110,
	HEADER_DG_GUILD_NOTICE_DUNGEON_END	= 111,
	HEADER_DG_GUILD_NOTICE_TICKETS_CHANGED	= 112,
	HEADER_DG_GUILD_UPDATE_GUARD_ITEM_DATA	= 113,
	HEADER_DG_GUILD_UPDATE_GUARD_ACTIVE_SET_DATA = 116,

	HEADER_DG_RELOAD_PROTO		= 120,
	HEADER_DG_CHANGE_NAME		= 121,

	HEADER_DG_AUTH_LOGIN		= 122,

	HEADER_DG_CHANGE_EMPIRE_PRIV	= 124,
	HEADER_DG_CHANGE_GUILD_PRIV		= 125,

	HEADER_DG_MONEY_LOG			= 126,

	HEADER_DG_CHANGE_CHARACTER_PRIV	= 127,

	HEADER_DG_BILLING_REPAIR		= 128,
	HEADER_DG_BILLING_EXPIRE		= 129,
	HEADER_DG_BILLING_LOGIN		= 130,
	HEADER_DG_VCARD			= 131,
	HEADER_DG_BILLING_CHECK		= 132,

	HEADER_DG_CREATE_OBJECT		= 140,
	HEADER_DG_DELETE_OBJECT		= 141,
	HEADER_DG_UPDATE_LAND		= 142,

	HEADER_DG_MARRIAGE_ADD		= 150,
	HEADER_DG_MARRIAGE_UPDATE		= 151,
	HEADER_DG_MARRIAGE_REMOVE		= 152,

	HEADER_DG_WEDDING_REQUEST		= 153,
	HEADER_DG_WEDDING_READY		= 154,
	HEADER_DG_WEDDING_START		= 155,
	HEADER_DG_WEDDING_END		= 156,

	HEADER_DG_MYSHOP_PRICELIST_RES	= 157,
	HEADER_DG_RELOAD_ADMIN = 158,
	HEADER_DG_BREAK_MARRIAGE = 159,
	HEADER_DG_ELECT_MONARCH			= 160,
	HEADER_DG_CANDIDACY				= 161,
	HEADER_DG_ADD_MONARCH_MONEY		= 162,
	HEADER_DG_TAKE_MONARCH_MONEY	= 163,
	HEADER_DG_COME_TO_VOTE			= 164,
	HEADER_DG_RMCANDIDACY			= 165,
	HEADER_DG_SETMONARCH			= 166,
	HEADER_DG_RMMONARCH			= 167,
	HEADER_DG_DEC_MONARCH_MONEY = 168,

	HEADER_DG_CHANGE_MONARCH_LORD_ACK = 169,
	HEADER_DG_UPDATE_MONARCH_INFO	= 170,

	HEADER_DG_ACK_CHANGE_GUILD_MASTER = 173,

	HEADER_DG_ACK_SPARE_ITEM_ID_RANGE = 174,

	HEADER_DG_UPDATE_HORSE_NAME 	= 175,
	HEADER_DG_ACK_HORSE_NAME		= 176,

	HEADER_DG_NEED_LOGIN_LOG		= 177,

	HEADER_DG_RESULT_CHARGE_CASH	= 179,
	HEADER_DG_ITEMAWARD_INFORMER	= 180,	//gift notify
	HEADER_DG_RESPOND_CHANNELSTATUS	= 181,
	HEADER_GD_REQ_USER_COUNT = 183,
	HEADER_DG_RET_USER_COUNT = 184,
#if defined(__BL_MOVE_CHANNEL__)
	HEADER_DG_RESPOND_MOVE_CHANNEL = 188,
#endif
#if defined(BL_OFFLINE_MESSAGE)
	HEADER_DG_RESPOND_OFFLINE_MESSAGES = 189,
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_DG_PRIVATE_SHOP = 190,
#endif
#ifdef ENABLE_EVENT_MANAGER
	HEADER_DG_EVENT_MANAGER = 212,
#endif
#ifdef ENABLE_OFFLINE_SHOP
	HEADER_DG_NEW_OFFLINESHOP				= 213,
#endif
#ifdef RANKING_SYSTEM
	HEADER_DG_SERVER_RANKING_LOAD			= 214,
	HEADER_DG_SERVER_RANKING_LOAD_MY_POS	= 215,
	HEADER_DG_RANKING_LOAD					= 216,
	HEADER_DG_RANKING_LOAD_MY_POS			= 217,
	HEADER_DG_RANKING_DATA_STATUS			= 218,
#endif
	HEADER_DG_FISH_VNUM_RANKING_LOAD		= 219,

#ifdef ENABLE_NEW_PET_SYSTEM
	HEADER_DG_UPDATE_PET_NAME = 232,
	HEADER_DG_ACK_PET_NAME = 233,
#endif
#ifdef __DUNGEON_INFO_ENABLE__
	HEADER_DG_DUNGEONINFO_RANKING,
#endif
#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
	HEADER_DG_RELOAD_DAMAGE_RESTRICTIONS = 235,
#endif

	HEADER_DG_MAP_LOCATIONS			= 0xfe,

	HEADER_DG_P2P					= 0xff,
};

/* ----------------------------------------------
 * table
 * ----------------------------------------------
 */

/* game Server -> DB Server */
#pragma pack(1)
enum ERequestChargeType
{
	ERequestCharge_Cash = 0,
	ERequestCharge_Mileage,
};

typedef struct SRequestChargeCash
{
	DWORD		dwAID;		// id(primary key) - Account Table
	DWORD		dwAmount;
	ERequestChargeType	eChargeType;
} TRequestChargeCash;

#if defined(BL_OFFLINE_MESSAGE)
typedef struct
{
	char 	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGDReadOfflineMessage;

typedef struct
{
	char	szFrom[CHARACTER_NAME_MAX_LEN + 1];
	char	szMessage[CHAT_MAX_LEN + 1];
} TPacketDGReadOfflineMessage;

typedef struct
{
	char	szFrom[CHARACTER_NAME_MAX_LEN + 1];
	char	szTo[CHARACTER_NAME_MAX_LEN + 1];
	char	szMessage[CHAT_MAX_LEN + 1];
} TPacketGDSendOfflineMessage;
#endif

typedef struct SSimplePlayer
{
	DWORD		dwID;
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE		byJob;
	BYTE		byLevel;
	DWORD		dwPlayMinutes;
	BYTE		byST, byHT, byDX, byIQ;
	DWORD		wMainPart; // @fixme502
	BYTE		bChangeName;
	DWORD		wHairPart; // @fixme502
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	DWORD		wAccePart;
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	DWORD	wEmblematPart;
#endif
	BYTE		bDummy[4];
	int32_t		x, y;
	int32_t		lAddr;
	WORD		wPort;
	BYTE		skill_group;
} TSimplePlayer;

typedef struct SAccountTable
{
	DWORD		id;
	char		login[LOGIN_MAX_LEN + 1];
	char		passwd[PASSWD_MAX_LEN + 1];
	char		social_id[SOCIAL_ID_MAX_LEN + 1];
	char		status[ACCOUNT_STATUS_MAX_LEN + 1];
	BYTE		bEmpire;
	TSimplePlayer	players[PLAYER_PER_ACCOUNT];
} TAccountTable;

typedef struct SPacketDGCreateSuccess
{
	BYTE		bAccountCharacterIndex;
	TSimplePlayer	player;
} TPacketDGCreateSuccess;

typedef struct TPlayerItemAttribute
{
	BYTE	bType;
	int16_t	sValue;
#if defined(__ITEM_APPLY_RANDOM__)
	BYTE bPath;
#endif
} TPlayerItemAttribute;

typedef struct SPlayerItem
{
	DWORD	id;
	BYTE	window;
	WORD	pos;
	DWORD	count;

	DWORD	vnum;
	int32_t	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute    aAttr[ITEM_ATTRIBUTE_MAX_NUM];

	DWORD	owner;
} TPlayerItem;

typedef struct SQuickslot
{
	BYTE	type;
	BYTE	pos;
} TQuickslot;

typedef struct SPlayerSkill
{
	BYTE	bMasterType;
	BYTE	bLevel;
	int32_t	tNextRead;
} TPlayerSkill;

#ifdef TITLE_SYSTEM_BYLUZER
typedef struct SPlayerTitle
{
	BYTE	titletype;
	BYTE	active;
} TPlayerTitle;
#endif

struct	THorseInfo
{
	BYTE	bLevel;
	BYTE	bRiding;
	short	sStamina;
	short	sHealth;
	DWORD	dwHorseHealthDropTime;
};

#ifdef _ENABLE_BATTLEPASS_
enum EBattlePassType
{
	BATTLE_PASS_NORMAL = 0,
	BATTLE_PASS_EVENT = 1,
};

enum EBattlePassMissionsPerPlayer
{
	BATTLEPASS_MISSIONS_PER_PLAYER = 16,  // 8 normal + 8 premium
};

enum EBattlePassMissionTime
{
	MISSION_TIME_DAILY,
	MISSION_TIME_WEEKLY,
	MISSION_TIME_EVENT,
};
enum EBattlePassMissionTypes
{

	MISSION_TYPE_KILL, // specific / global
	MISSION_TYPE_SELL, // specific / global

	MISSION_TYPE_ITEM_DESTROY, // specific / global
	MISSION_TYPE_ITEM_DROP, // specific / global

	MISSION_TYPE_CRAFT, // specific / global
	MISSION_TYPE_CHEST_OPEN, // specific / global
	MISSION_TYPE_SASH,

	MISSION_TYPE_USE_ITEM, // specific / global
	MISSION_TYPE_FINISH_DUNGEON, // specific / global

	MISSION_TYPE_POLYMORPH,
	MISSION_TYPE_MESSAGES,

	MISSION_TYPE_PLAYTIME,

	MISSION_TYPE_REFINE,

	MISSION_TYPE_SWITCH_ITEMS,

	MISSION_TYPE_AVERAGE_DAMAGE,
	MISSION_TYPE_KILL_STONES,
	MISSION_TYPE_KILL_BOSS,
	MISSION_TYPE_CHRISTMAS_RAFFLE,
	MISSION_TYPE_CHRISTMAS_MAP,
};

enum EBattlePassMissionTypeSpecific
{
	MISSION_TYPE_SPECIFIC,
	MISSION_TYPE_GLOBAL,
};

typedef struct BattlePassParser
{
	uint16_t id;
	uint8_t missionType;
	uint8_t missionSpecific;
	std::vector<uint32_t> vnums;
	uint32_t maxProgress;
	uint8_t timeInfo;
	uint16_t pointsReward;
	std::string name;
	uint8_t battlePassType;
} TBattlePassParser;

typedef struct BattlePassStruct
{
	// Normal
	uint8_t maxDailyMissions;
	uint8_t maxWeeklyMissions;
	uint64_t endTime;
	std::vector<std::pair<int32_t, int32_t>> rewardsFree;
	std::vector<std::pair<int32_t, int32_t>> rewardsPremium;
	
	// Event
	uint8_t maxEventMissions;
	uint64_t eventEndTime;
	bool eventActive;
	std::vector<std::pair<int32_t, int32_t>> rewardsEvent;
} TBattlePassSettings;

typedef struct PlayerBattlePass
{
	uint16_t missionID;
	uint32_t progress;
	uint8_t type;
	uint32_t endTime;
	uint8_t battlePassType;

	PlayerBattlePass() : missionID(0), progress(0), type(0), endTime(0), battlePassType(BATTLE_PASS_NORMAL) {}
} TPlayerBattlePass;

// In-memory player mission slot (not persisted directly - serialised to TPlayerBattlePass on save)
struct BPSlot
{
	uint16_t defId = 0;
	uint32_t progress = 0;
	uint8_t  track = 0; // MISSION_TIME_DAILY / WEEKLY / EVENT
	uint8_t  passType = 0; // BATTLE_PASS_NORMAL / BATTLE_PASS_EVENT
	uint64_t expiresAt = 0;
};
#endif

typedef struct SPlayerTable
{
	DWORD	id;

	char	name[CHARACTER_NAME_MAX_LEN + 1];
	char	ip[IP_ADDRESS_LENGTH + 1];

	WORD	job;
	BYTE	voice;

	BYTE	level;
	BYTE	level_step;
#ifdef ENABLE_SECONDARY_LEVEL
	BYTE	secondary_level;
#endif
	short	st, ht, dx, iq;
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	uint8_t enlightLevel;
#endif

	DWORD	exp;
#ifdef ENABLE_LONG_LONG
	int64_t	gold;
#else
	INT		gold;
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
	int64_t	cheque;
#endif

	BYTE	dir;
	INT		x, y, z;
	INT		lMapIndex;

	int32_t	lExitX, lExitY;
	int32_t	lExitMapIndex;

	// @fixme301
	int hp;
	int sp;

	int sRandomHP;
	int sRandomSP;

	int         playtime;

	short	stat_point;
	short	skill_point;
	short	sub_skill_point;
	short	horse_skill_point;

#ifdef ENABLE_MOUNT_SYSTEM
	int		iMountLevel;
	int		iMountExp;
#endif

	TPlayerSkill skills[SKILL_MAX_NUM];

	TQuickslot  quickslot[QUICKSLOT_MAX_NUM];

	BYTE	part_base;
	DWORD	parts[PART_MAX_NUM]; // @fixme502

	short	stamina;

	BYTE	skill_group;
	int32_t	lAlignment;
	char	szMobile[MOBILE_MAX_LEN + 1];

	short	stat_reset_count;

	THorseInfo	horse;

	DWORD	logoff_interval;

#ifdef __ENABLE_POLYMORPH_SYSTEM__
	uint8_t	bPolySkin;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
	WORD	wPrivateShopUnlockedSlot;
#endif

	int		aiPremiumTimes[PREMIUM_MAX_NUM];
#ifdef WEEKLY_RANK_BYLUZER
	int w_monsters;
	int w_bosses;
	int w_metins;
	int w_dungs;
	int w_ds;
	int w_refine;
	int w_sm;
	int weekly_season;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	TPlayerTitle titles[TITLE_MAX_NUM];
#endif
#ifdef _ENABLE_BATTLEPASS_
	TPlayerBattlePass battlePass[BATTLEPASS_MISSIONS_PER_PLAYER];
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	INT		pkt_osiag;
#endif
#ifdef ENABLE_PVP_RANKING
	int rank_Kill;
	int rank_Dead;
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	BYTE	bTitleAchievement;
	BYTE	bTitleAchievementPremium;
#endif
#ifdef __SKILL_COSTUME__
	BYTE	bSkillMotion[ESkillMotionLength::MAX_SKILL_COUNT + ESkillMotionLength::MAX_BUFF_COUNT];
#endif
} TPlayerTable;

#ifdef WEEKLY_RANK_BYLUZER
typedef struct GDWeeklyRewards
{
	DWORD	item_vnum;
	DWORD	account_id;
} TWeeklyRewardsGD;
#endif

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
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	char	szLocaleName[CHARACTER_NAME_MAX_LEN + 1];

	BYTE	bType;			// Monster, NPC
	BYTE	bRank;			// PAWN, KNIGHT, KING
	BYTE	bBattleType;		// MELEE, etc..
	BYTE	bLevel;			// Level
	BYTE	bSize;

	DWORD	dwGoldMin;
	DWORD	dwGoldMax;
	DWORD	dwExp;
	int64_t	dwMaxHP;
	BYTE	bRegenCycle;
	BYTE	bRegenPercent;
	WORD	wDef;

	DWORD	dwAIFlag;
	DWORD	dwRaceFlag;
	DWORD	dwImmuneFlag;

	BYTE	bStr, bDex, bCon, bInt;
	DWORD	dwDamageRange[2];

	short	sAttackSpeed;
	short	sMovingSpeed;
	BYTE	bAggresiveHPPct;
	WORD	wAggressiveSight;
	WORD	wAttackRange;

	char	cEnchants[MOB_ENCHANTS_MAX_NUM];
	char	cResists[MOB_RESISTS_MAX_NUM];

	DWORD	dwResurrectionVnum;
	DWORD	dwDropItemVnum;

	BYTE	bMountCapacity;
	BYTE	bOnClickType;

	BYTE	bEmpire;
	char	szFolder[64 + 1];

	float	fDamMultiply;

	DWORD	dwSummonVnum;
	DWORD	dwDrainSP;
	DWORD	dwMobColor;
	DWORD	dwPolymorphItemVnum;

	TMobSkillLevel Skills[MOB_SKILL_MAX_NUM];

	BYTE	bBerserkPoint;
	BYTE	bStoneSkinPoint;
	BYTE	bGodSpeedPoint;
	BYTE	bDeathBlowPoint;
	BYTE	bRevivePoint;
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	short	sPktOsiag;
#endif
} TMobTable;

typedef struct SSkillTable
{
	DWORD	dwVnum;
	char	szName[32 + 1];
	BYTE	bType;
	BYTE	bMaxLevel;
	DWORD	dwSplashRange;

	char	szPointOn[64];
	char	szPointPoly[100 + 1];
	char	szSPCostPoly[100 + 1];
	char	szDurationPoly[100 + 1];
	char	szDurationSPCostPoly[100 + 1];
	char	szCooldownPoly[100 + 1];
	char	szMasterBonusPoly[100 + 1];
	//char	szAttackGradePoly[100 + 1];
	char	szGrandMasterAddSPCostPoly[100 + 1];
	DWORD	dwFlag;
	DWORD	dwAffectFlag;

	// Data for secondary skill
	char 	szPointOn2[64];
	char 	szPointPoly2[100 + 1];
	char 	szDurationPoly2[100 + 1];
	DWORD 	dwAffectFlag2;

	// Data for grand master point
	char 	szPointOn3[64];
	char 	szPointPoly3[100 + 1];
	char 	szDurationPoly3[100 + 1];

	BYTE	bLevelStep;
	BYTE	bLevelLimit;
	DWORD	preSkillVnum;
	BYTE	preSkillLevel;

	int32_t	lMaxHit;
	char	szSplashAroundDamageAdjustPoly[100 + 1];

	BYTE	bSkillAttrType;

	DWORD	dwTargetRange;
} TSkillTable;

#ifdef ENABLE_BUY_WITH_ITEM
typedef struct SShopItemPrice
{
	DWORD		vnum;
	DWORD		count;
} TShopItemPrice;
#endif

typedef struct SShopItemTable
{
	DWORD		vnum;
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t count;
#else
	BYTE count;
#endif
#ifdef ENABLE_BUY_WITH_ITEM
	TShopItemPrice	itemprice[MAX_SHOP_PRICES];
#endif

    TItemPos	pos;
#ifdef ENABLE_LONG_LONG
	int64_t	price;
#else
	DWORD		price;
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	int64_t	cheque;
#endif
	BYTE		display_pos;
} TShopItemTable;

#ifdef ENABLE_VS_SHOP_SEARCH
typedef struct search_item
{
	DWORD	vnum;
} TSearchItems;
#endif

typedef struct SShopTable
{
	DWORD		dwVnum;
	DWORD		dwNPCVnum;

#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t byItemCount;
#else
	BYTE byItemCount;
#endif
	TShopItemTable	items[SHOP_HOST_ITEM_MAX_NUM];
} TShopTable;

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
typedef struct SSOfflineShopItemTable
{
	BYTE		bDisplayPos;
	TItemPos	itemPos;
	int64_t	lPrice;
	int64_t	lPrice2;
} TOfflineShopItemTable;

typedef struct ShopLog {
	DWORD	itemVnum;
	int		itemCount;
	int64_t	price;
	int64_t	price2;
	char		date[20];
	char		action[5];
}TShopLog;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
struct AcceMaterials
{
    DWORD vnum;
    DWORD count;
};
#endif

#define QUEST_NAME_MAX_LEN	32
#define QUEST_STATE_MAX_LEN	64

typedef struct SQuestTable
{
	DWORD		dwPID;
	char		szName[QUEST_NAME_MAX_LEN + 1];
	char		szState[QUEST_STATE_MAX_LEN + 1];
	int32_t		lValue;
} TQuestTable;

typedef struct SItemLimit
{
	BYTE	bType;
	int32_t	lValue;
} TItemLimit;

typedef struct SItemApply
{
	BYTE	bType;
	int32_t	lValue;
} TItemApply;

typedef struct SItemTable : public SEntityTable
{
	DWORD		dwVnumRange;
	char        szName[ITEM_NAME_MAX_LEN + 1];
	char	szLocaleName[ITEM_NAME_MAX_LEN + 1];
	BYTE	bType;
	BYTE	bSubType;

	BYTE        bWeight;
	BYTE	bSize;

	DWORD	dwAntiFlags;
	DWORD	dwFlags;
	DWORD	dwWearFlags;
	DWORD	dwImmuneFlag;

#ifdef ENABLE_LONG_LONG
	int64_t	dwGold;
	int64_t	dwShopBuyPrice;
#else
	DWORD		dwGold;
	DWORD		dwShopBuyPrice;
#endif

	TItemLimit	aLimits[ITEM_LIMIT_MAX_NUM];
	TItemApply	aApplies[ITEM_APPLY_MAX_NUM];
	int32_t        alValues[ITEM_VALUES_MAX_NUM];
	int32_t	alSockets[ITEM_SOCKET_MAX_NUM];
	DWORD	dwRefinedVnum;
	WORD	wRefineSet;
	BYTE	bAlterToMagicItemPct;
	BYTE	bSpecular;
	BYTE	bGainSocketPct;

	short int	sAddonType;

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	BYTE	SpecialInvType;
	BYTE	CanUseInSpecialInv;
#endif

	char		cLimitRealTimeFirstUseIndex;
	char		cLimitTimerBasedOnWearIndex;
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

	char    szApply[APPLY_NAME_MAX_LEN + 1];
	DWORD   dwApplyIndex;
	DWORD   dwProb;
	int32_t    lValues[ITEM_ATTRIBUTE_MAX_LEVEL];
	BYTE    bMaxLevelBySet[ATTRIBUTE_SET_MAX_NUM];
};

typedef struct SConnectTable
{
	char	login[LOGIN_MAX_LEN + 1];
	IDENT	ident;
} TConnectTable;

typedef struct SLoginPacket
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TLoginPacket;

typedef struct SPlayerLoadPacket
{
	DWORD	account_id;
	DWORD	player_id;
	BYTE	account_index;
} TPlayerLoadPacket;

typedef struct SPlayerCreatePacket
{
	char		login[LOGIN_MAX_LEN + 1];
	char		passwd[PASSWD_MAX_LEN + 1];
	DWORD		account_id;
	BYTE		account_index;
	TPlayerTable	player_table;
} TPlayerCreatePacket;

typedef struct SPlayerDeletePacket
{
	char	login[LOGIN_MAX_LEN + 1];
	DWORD	player_id;
	BYTE	account_index;
	//char	name[CHARACTER_NAME_MAX_LEN + 1];
	char	private_code[8];
} TPlayerDeletePacket;

typedef struct SLogoutPacket
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TLogoutPacket;

typedef struct SPlayerCountPacket
{
	DWORD	dwCount;
} TPlayerCountPacket;

#define SAFEBOX_MAX_NUM			135
#define SAFEBOX_PASSWORD_MAX_LEN	6

typedef struct SSafeboxTable
{
	DWORD	dwID;
	BYTE	bSize;
	DWORD	dwGold;
	WORD	wItemCount;
} TSafeboxTable;

typedef struct SSafeboxChangeSizePacket
{
	DWORD	dwID;
	BYTE	bSize;
} TSafeboxChangeSizePacket;

typedef struct SSafeboxLoadPacket
{
	DWORD	dwID;
	char	szLogin[LOGIN_MAX_LEN + 1];
	char	szPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
} TSafeboxLoadPacket;

#if defined(__BL_MOVE_CHANNEL__)
typedef struct SMoveChannel
{
	BYTE	bChannel;
	int32_t	lMapIndex;
} TMoveChannel;

typedef struct SRespondMoveChannel
{
	WORD	wPort;
	int32_t	lAddr;
} TRespondMoveChannel;
#endif

typedef struct SSafeboxChangePasswordPacket
{
	DWORD	dwID;
	char	szOldPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
	char	szNewPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
} TSafeboxChangePasswordPacket;

typedef struct SSafeboxChangePasswordPacketAnswer
{
	BYTE	flag;
} TSafeboxChangePasswordPacketAnswer;

typedef struct SEmpireSelectPacket
{
	DWORD	dwAccountID;
	BYTE	bEmpire;
} TEmpireSelectPacket;

typedef struct SPacketGDSetup
{
	char	szPublicIP[16];	// Public IP which listen to users
	BYTE	bChannel;
	WORD	wListenPort;
	WORD	wP2PPort;
	int32_t	alMaps[MAP_ALLOW_LIMIT];
	DWORD	dwLoginCount;
	BYTE	bAuthServer;
} TPacketGDSetup;

typedef struct SPacketDGMapLocations
{
	BYTE	bCount;
} TPacketDGMapLocations;

typedef struct SMapLocation
{
	int32_t	alMaps[MAP_ALLOW_LIMIT];
	char	szHost[MAX_HOST_LENGTH + 1];
	WORD	wPort;
} TMapLocation;

typedef struct SPacketDGP2P
{
	char	szHost[MAX_HOST_LENGTH + 1];
	WORD	wPort;
	BYTE	bChannel;
} TPacketDGP2P;

typedef struct SPacketGDDirectEnter
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
	BYTE	index;
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
	BYTE level;
	BYTE grade;
} TPacketGuildChangeMemberData;

typedef struct SPacketDGLoginAlready
{
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketDGLoginAlready;

typedef struct TPacketAffectElement
{
	DWORD	dwType;
	BYTE	bApplyOn;
	int32_t	lApplyValue;
	DWORD	dwFlag;
	int32_t	lDuration;
	int32_t	lSPCost;
} TPacketAffectElement;

typedef struct SPacketGDAddAffect
{
	DWORD			dwPID;
	TPacketAffectElement	elem;
} TPacketGDAddAffect;

typedef struct SPacketGDRemoveAffect
{
	DWORD	dwPID;
	DWORD	dwType;
	BYTE	bApplyOn;
} TPacketGDRemoveAffect;

typedef struct SPacketGDHighscore
{
	DWORD	dwPID;
	int32_t	lValue;
	char	cDir;
	char	szBoard[21];
} TPacketGDHighscore;

typedef struct SPacketPartyCreate
{
	DWORD	dwLeaderPID;
} TPacketPartyCreate;

typedef struct SPacketPartyDelete
{
	DWORD	dwLeaderPID;
} TPacketPartyDelete;

typedef struct SPacketPartyAdd
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bState;
} TPacketPartyAdd;

typedef struct SPacketPartyRemove
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
} TPacketPartyRemove;

typedef struct SPacketPartyStateChange
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bRole;
	BYTE	bFlag;
} TPacketPartyStateChange;

typedef struct SPacketPartySetMemberLevel
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bLevel;
} TPacketPartySetMemberLevel;

typedef struct SPacketGDBoot
{
    DWORD	dwItemIDRange[2];
	char	szIP[16];
} TPacketGDBoot;

typedef struct SPacketGuild
{
	DWORD	dwGuild;
	DWORD	dwInfo;
} TPacketGuild;

typedef struct SPacketGDGuildAddMember
{
	DWORD	dwPID;
	DWORD	dwGuild;
	BYTE	bGrade;
} TPacketGDGuildAddMember;

typedef struct SPacketDGGuildMember
{
	DWORD	dwPID;
	DWORD	dwGuild;
	BYTE	bGrade;
	BYTE	isGeneral;
	BYTE	bJob;
	BYTE	bLevel;
	DWORD	dwOffer;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketDGGuildMember;

typedef struct SPacketGuildWar
{
	BYTE	bType;
	BYTE	bWar;
	DWORD	dwGuildFrom;
	DWORD	dwGuildTo;
	int32_t	lWarPrice;
	int32_t	lInitialScore;
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
	//DWORD src_vnum;
	//DWORD result_vnum;
	DWORD id;
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t material_count;
#else
	BYTE material_count;
#endif
	int cost;
	int cost2;
	int cost3;
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

typedef struct SPacketGDSMS
{
	char szFrom[CHARACTER_NAME_MAX_LEN + 1];
	char szTo[CHARACTER_NAME_MAX_LEN + 1];
	char szMobile[MOBILE_MAX_LEN + 1];
	char szMsg[SMS_MAX_LEN + 1];
} TPacketGDSMS;

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
	DWORD	dwID;
	DWORD	dwLoginKey;
	char	szLogin[LOGIN_MAX_LEN + 1];
	char	szSocialID[SOCIAL_ID_MAX_LEN + 1];
	DWORD	adwClientKey[4];
	BYTE	bBillType;
	DWORD	dwBillID;
	int		iPremiumTimes[PREMIUM_MAX_NUM];
} TPacketGDAuthLogin;

typedef struct SPacketGDLoginByKey
{
	char	szLogin[LOGIN_MAX_LEN + 1];
	DWORD	dwLoginKey;
	DWORD	adwClientKey[4];
	char	szIP[MAX_HOST_LENGTH + 1];
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

typedef struct SPacketDGUserCount
{
	DWORD user;
} TPacketDGUserCount;

typedef struct SPacketMoneyLog
{
	BYTE type;
	DWORD vnum;
#ifdef ENABLE_LONG_LONG
	int64_t gold;
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
	char	szFlagName[EVENT_FLAG_NAME_MAX_LEN + 1];
	int32_t	lValue;
} TPacketSetEventFlag;

typedef struct SPacketBillingLogin
{
	DWORD	dwLoginKey;
	BYTE	bLogin;
} TPacketBillingLogin;

typedef struct SPacketBillingRepair
{
	DWORD	dwLoginKey;
	char	szLogin[LOGIN_MAX_LEN + 1];
	char	szHost[MAX_HOST_LENGTH + 1];
} TPacketBillingRepair;

typedef struct SPacketBillingExpire
{
	char	szLogin[LOGIN_MAX_LEN + 1];
	BYTE	bBillType;
	DWORD	dwRemainSeconds;
} TPacketBillingExpire;

typedef struct SPacketLoginOnSetup
{
	DWORD   dwID;
	char    szLogin[LOGIN_MAX_LEN + 1];
	char    szSocialID[SOCIAL_ID_MAX_LEN + 1];
	char    szHost[MAX_HOST_LENGTH + 1];
	DWORD   dwLoginKey;
	DWORD   adwClientKey[4];
#ifdef __PREMIUM_PRIVATE_SHOP__
	DWORD	dwPID;
	DWORD	dwHandle;
	bool	bHasPrivateShop;
#endif
} TPacketLoginOnSetup;

typedef struct SPacketGDCreateObject
{
	DWORD	dwVnum;
	DWORD	dwLandID;
	INT		lMapIndex;
	INT	 	x, y;
	float	xRot;
	float	yRot;
	float	zRot;
} TPacketGDCreateObject;

typedef struct SPacketGDVCard
{
	DWORD	dwID;
	char	szSellCharacter[CHARACTER_NAME_MAX_LEN + 1];
	char	szSellAccount[LOGIN_MAX_LEN + 1];
	char	szBuyCharacter[CHARACTER_NAME_MAX_LEN + 1];
	char	szBuyAccount[LOGIN_MAX_LEN + 1];
} TPacketGDVCard;

typedef struct SGuildReserve
{
	DWORD       dwID;
	DWORD       dwGuildFrom;
	DWORD       dwGuildTo;
	DWORD       dwTime;
	BYTE        bType;
	int32_t        lWarPrice;
	int32_t        lInitialScore;
	bool        bStarted;
	DWORD	dwBetFrom;
	DWORD	dwBetTo;
	int32_t	lPowerFrom;
	int32_t	lPowerTo;
	int32_t	lHandicap;
} TGuildWarReserve;

typedef struct SPacketGDGuildWarBet
{
	DWORD	dwWarID;
	char	szLogin[LOGIN_MAX_LEN + 1];
	DWORD	dwGold;
	DWORD	dwGuild;
} TPacketGDGuildWarBet;

// Marriage

typedef struct SPacketMarriageAdd
{
	DWORD dwPID1;
	DWORD dwPID2;
	time_t tMarryTime;
	char szName1[CHARACTER_NAME_MAX_LEN + 1];
	char szName2[CHARACTER_NAME_MAX_LEN + 1];
} TPacketMarriageAdd;

typedef struct SPacketMarriageUpdate
{
	DWORD dwPID1;
	DWORD dwPID2;
	INT  iLovePoint;
	BYTE  byMarried;
} TPacketMarriageUpdate;

typedef struct SPacketMarriageRemove
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketMarriageRemove;

typedef struct SPacketWeddingRequest
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingRequest;

typedef struct SPacketWeddingReady
{
	DWORD dwPID1;
	DWORD dwPID2;
	DWORD dwMapIndex;
} TPacketWeddingReady;

typedef struct SPacketWeddingStart
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingStart;

typedef struct SPacketWeddingEnd
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingEnd;

typedef struct SPacketMyshopPricelistHeader
{
	DWORD	dwOwnerID;
	BYTE	byCount;
} TPacketMyshopPricelistHeader;

typedef struct SItemPriceInfo
{
	DWORD	dwVnum;
	int64_t	dwPrice;
} TItemPriceInfo;

typedef struct SItemPriceListTable
{
	DWORD	dwOwnerID;
	BYTE	byCount;

	TItemPriceInfo	aPriceInfo[SHOP_PRICELIST_MAX_NUM];
} TItemPriceListTable;

typedef struct SPacketBlockChat
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	int32_t lDuration;
} TPacketBlockChat;

//ADMIN_MANAGER
typedef struct TAdminInfo
{
	int m_ID;
	char m_szAccount[32];
	char m_szName[32];
	char m_szContactIP[16];
	char m_szServerIP[16];
	int m_Authority;
} tAdminInfo;
//END_ADMIN_MANAGER

//BOOT_LOCALIZATION
struct tLocale
{
	char szValue[32];
	char szKey[32];
};
//BOOT_LOCALIZATION

//RELOAD_ADMIN
typedef struct SPacketReloadAdmin
{
	char szIP[16];
} TPacketReloadAdmin;
//END_RELOAD_ADMIN

typedef struct TMonarchInfo
{
	DWORD pid[4];
	int64_t money[4];
	char name[4][32];
	char date[4][32];
} MonarchInfo;

typedef struct TMonarchElectionInfo
{
	DWORD pid;
	DWORD selectedpid;
	char date[32];
} MonarchElectionInfo;

typedef struct tMonarchCandidacy
{
	DWORD pid;
	char name[32];
	char date[32];
} MonarchCandidacy;

typedef struct tChangeMonarchLord
{
	BYTE bEmpire;
	DWORD dwPID;
} TPacketChangeMonarchLord;

typedef struct tChangeMonarchLordACK
{
	BYTE bEmpire;
	DWORD dwPID;
	char szName[32];
	char szDate[32];
} TPacketChangeMonarchLordACK;

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
	char	login[LOGIN_MAX_LEN + 1];
} TPacketDC;

typedef struct tNeedLoginLogInfo
{
	DWORD dwPlayerID;
} TPacketNeedLoginLogInfo;

typedef struct tItemAwardInformer
{
	char	login[LOGIN_MAX_LEN + 1];
	char	command[20];
	unsigned int vnum;
} TPacketItemAwardInfromer;

typedef struct tDeleteAwardID
{
	DWORD dwID;
} TPacketDeleteAwardID;

typedef struct SChannelStatus
{
	WORD nPort; // backward change for client @fixme024
	BYTE bStatus;
} TChannelStatus;

#ifdef ENABLE_SWITCHBOT
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

#ifdef __PREMIUM_PRIVATE_SHOP__
typedef struct SPrivateShop
{
	DWORD				dwOwner;
	char				szTitle[TITLE_MAX_LEN + 1];
	char				szOwnerName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE				bState;

	DWORD				dwVnum;
	BYTE				bTitleType;

	int32_t				lX;
	int32_t				lY;
	int32_t				lMapIndex;
	BYTE				bChannel;
	WORD				wPort;

	int64_t			llGold;
	DWORD				dwCheque;
	BYTE				bPageCount;
	time_t				tPremiumTime;
	WORD				wUnlockedSlots;
} TPrivateShop;

typedef struct SItemPrice
{
	int64_t	llGold;
	DWORD		dwCheque;
} TItemPrice;

typedef struct SPlayerPrivateShopItem
{
	DWORD					dwID;
	WORD					wPos;
	DWORD					dwCount;

	DWORD					dwVnum;
	int32_t					alSockets[ITEM_SOCKET_MAX_NUM];

	TPlayerItemAttribute    aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	time_t					tCheckin;

	TItemPrice				TPrice;
	DWORD					dwOwner;

#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
    DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
    DWORD dwRefineElement;
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
    TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
} TPlayerPrivateShopItem;

typedef struct SPrivateShopSale
{
	DWORD		dwID;
	DWORD		dwOwner;
	DWORD		dwCustomer;
	char		szCustomerName[CHARACTER_NAME_MAX_LEN + 1];
	time_t		tTime;
	TPlayerPrivateShopItem	TItem;

	void AssignID()
	{
		size_t seededID = 0;

		// Both time of sale and sale item's id are a unique pair
		boost::hash_combine(seededID, tTime);
		boost::hash_combine(seededID, TItem.dwID);

		dwID = seededID;
	}
} TPrivateShopSale;

typedef struct SMarketItemPrice
{
	DWORD dwVnum;
	TItemPrice TPrice;
} TMarketItemPrice;

/* Game -> Database */
enum EPrivateShopGDSubheader
{
	PRIVATE_SHOP_GD_SUBHEADER_LOGOUT,
	PRIVATE_SHOP_GD_SUBHEADER_CREATE,
	PRIVATE_SHOP_GD_SUBHEADER_CLOSE,
	PRIVATE_SHOP_GD_SUBHEADER_DELETE,
	PRIVATE_SHOP_GD_SUBHEADER_DESPAWN,
	PRIVATE_SHOP_GD_SUBHEADER_WITHDRAW_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_MODIFY_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_BUY_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_PRICE_CHANGE_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_MOVE_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKIN_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKOUT_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_TITLE_CHANGE_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_WARP_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_SLOT_UNLOCK_REQUEST,
	PRIVATE_SHOP_GD_SUBHEADER_WITHDRAW,
	PRIVATE_SHOP_GD_SUBHEADER_BUY,
	PRIVATE_SHOP_GD_SUBHEADER_FAILED_BUY,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKIN_UPDATE,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_CHECKOUT_UPDATE,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_TRANSFER,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_DELETE,
	PRIVATE_SHOP_GD_SUBHEADER_ITEM_EXPIRE,
	PRIVATE_SHOP_GD_SUBHEADER_PREMIUM_TIME_UPDATE,
	PRIVATE_SHOP_GD_SUBHEADER_INIT,
};

typedef struct SSelectedItem
{
	DWORD		dwShopID;
	WORD		wPos;
	TItemPrice	TPrice;
} TSelectedItem;

typedef struct SPacketGDPrivateShopBuyRequest
{
	DWORD			dwCustomerPID;
	int64_t		llGoldBalance;
	DWORD			dwChequeBalance;
	TSelectedItem	aSelectedItems[SELECTED_ITEM_MAX_NUM];
} TPacketGDPrivateShopBuyRequest;

typedef struct SPacketGDPrivateShopItemCheckin
{
	DWORD					dwShopID;
	TPlayerPrivateShopItem	TItem;
	int						iPos;
} TPacketGDPrivateShopItemCheckin;

typedef struct SPacketGDPrivateShopItemCheckout
{
	DWORD		dwPID;
	WORD		wSrcPos;
	TItemPos	TDstPos;
	TPlayerPrivateShopItem	TItem;
} TPacketGDPrivateShopItemCheckout;

typedef struct SPacketGDPrivateShopBuy
{
	TPlayerPrivateShopItem	TItem;
	DWORD					dwCustomer;
	char					szCustomerName[CHARACTER_NAME_MAX_LEN + 1];
	time_t					tTime;
} TPacketGDPrivateShopBuy;

typedef struct SPacketGDPrivateShopFailedBuy
{
	DWORD		dwShopID;
	WORD		wPos;
} TPacketGDPrivateShopFailedBuy;

typedef struct SPacketGDPrivateShopItemDelete
{
	DWORD		dwShopID;
	DWORD		dwItemID;
} TPacketGDPrivateShopItemDelete;

typedef struct SPacketGDPrivateShopItemExpire
{
	DWORD		dwShopID;
	WORD		wPos;
} TPacketGDPrivateShopItemExpire;


typedef struct SPacketGDPrivateShopPremiumTimeUpdate
{
	DWORD		dwAID;
	DWORD		dwPID;
	time_t		tPremiumTime;
} TPacketGDPrivateShopPremiumTimeUpdate;

/* Database -> Game */
enum EPrivateShopDGSubheader
{
	PRIVATE_SHOP_DG_SUBHEADER_CREATE_RESULT,
	PRIVATE_SHOP_DG_SUBHEADER_NO_SHOP,
	PRIVATE_SHOP_DG_SUBHEADER_CLOSE_RESULT_BALANCE_AVAILABLE,
	PRIVATE_SHOP_DG_SUBHEADER_CLOSE,
	PRIVATE_SHOP_DG_SUBHEADER_SPAWN,
	PRIVATE_SHOP_DG_SUBHEADER_DESTROY,
	PRIVATE_SHOP_DG_SUBHEADER_DESPAWN,
	PRIVATE_SHOP_DG_SUBHEADER_LOAD,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_LOAD,
	PRIVATE_SHOP_DG_SUBHEADER_SALE_LOAD,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_FALSE_ITEM,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_FALSE_PRICE,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_MODIFY_STATE,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_NO_GOLD,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_RESULT_NO_CHEQUE,
	PRIVATE_SHOP_DG_SUBHEADER_BUY_REQUEST,
	PRIVATE_SHOP_DG_SUBHEADER_REMOVE_ITEM,
	PRIVATE_SHOP_DG_SUBHEADER_ADD_ITEM,
	PRIVATE_SHOP_DG_SUBHEADER_SALE_UPDATE,
	PRIVATE_SHOP_DG_SUBHEADER_STATE_UPDATE,
	PRIVATE_SHOP_DG_SUBHEADER_WITHDRAW_RESULT_NO_BALANCE,
	PRIVATE_SHOP_DG_SUBHEADER_WITHDRAW,
	PRIVATE_SHOP_DG_SUBHEADER_NOT_MODIFY_STATE,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_PRICE_CHANGE,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_MOVE,
	PRIVATE_SHOP_DG_SUBHEADER_CANNOT_MOVE_ITEM,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_CHECKIN_REQ,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_CHECKIN_FALSE_ITEM,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_CHECKOUT_REQ,
	PRIVATE_SHOP_DG_SUBHEADER_ITEM_EXPIRE,
	PRIVATE_SHOP_DG_SUBHEADER_SHOP_NOT_AVAILABLE,
	PRIVATE_SHOP_DG_SUBHEADER_TITLE_CHANGE,
	PRIVATE_SHOP_DG_SUBHEADER_WARP,
	PRIVATE_SHOP_DG_SUBHEADER_UNLOCK_SLOT_RES,
	PRIVATE_SHOP_DG_SUBHEADER_NO_AVAILABLE_SPACE,
	PRIVATE_SHOP_DG_SUBHEADER_MARKET_ITEM_PRICE_DATA_UPDATE,
};

typedef struct SPacketDGPrivateShopCreateResult
{
	TPrivateShop		privateShopTable;
	bool				bSuccess;
} TPacketDGPrivateShopCreateResult;

typedef struct SPacketDGPrivateShopBuyRequest
{
	DWORD					dwCustomerPID;
	TPlayerPrivateShopItem	arRequestedItems[SELECTED_ITEM_MAX_NUM];
} TPacketDGPrivateShopBuyRequest;

typedef struct SPacketDGPrivateShopStateUpdate
{
	DWORD		dwPID;
	BYTE		bState;
} TPacketDGPrivateShopStateUpdate;

typedef struct SPacketDGPrivateShopWithdraw
{
	int64_t	llGold;
	DWORD		dwCheque;
} TPacketDGPrivateShopWithdraw;

typedef struct SPacketDGPrivateShopItemCheckin
{
	DWORD		dwPID;
	TPlayerPrivateShopItem	TItem;
} TPacketDGPrivateShopItemCheckin;

typedef struct SPacketDGPrivateShopItemCheckout
{
	DWORD		dwPID;
	WORD		wSrcPos;
	TItemPos	TDstPos;
} TPacketDGPrivateShopItemCheckout;

/* Database <-> Game */
typedef struct SPacketPrivateShopItemMove
{
	DWORD		dwShopID;
	WORD		wPos;
	WORD		wChangePos;
} TPacketPrivateShopItemMove;

typedef struct SPacketPrivateShopItemPriceChange
{
	DWORD		dwShopID;
	WORD		wPos;
	TItemPrice	TPrice;
} TPacketPrivateShopItemPriceChange;

typedef struct SPacketPrivateShopTitleChange
{
	DWORD		dwPID;
	char		szTitle[SHOP_SIGN_MAX_LEN + 1];
} TPacketPrivateShopTitleChange;

typedef struct SPacketGDPrivateShopWarpReq
{
	DWORD		dwPID;
	DWORD		dwMapIndex;
	WORD		wListenPort;
	BYTE		bChannel;
} TPacketGDPrivateShopWarpReq;

typedef struct SPacketDGPrivateShopWarp
{
	WORD		wListenPort;
	int32_t		lAddr;
} TPacketDGPrivateShopWarp;

typedef struct SPacketGDPrivateShopSlotUnlockReq
{
	DWORD		dwPID;
	WORD		wCount;
} TPacketGDPrivateShopSlotUnlockReq;

typedef struct SPacketGDPrivateShopSlotUnlockRes
{
	DWORD		dwShopID;
	WORD		wUnlockedSlots;
} TPacketGDPrivateShopSlotUnlockRes;
#endif

#ifdef __ENABLE_POLYMORPH_SYSTEM__
typedef struct SPolymorphSkin
{
	uint8_t byIndex;
	uint32_t dwSkinVnum;
	uint32_t dwSkinUnlock;
} TPolymorphSkin;

typedef struct SPolymorphRequired
{
	uint32_t dwVnum;
	uint16_t wCount;
} TPolyRequired;

typedef struct SPolymorphStage
{
	uint8_t byStage;
	uint32_t dwVnum;
	uint32_t dwMonster;
	TItemApply tApplies[MAX_POLY_BONUS];
	TPolyRequired tRequired[MAX_POLY_REQUIRED_ITEMS];
} TPolymorphStage;
#endif

#ifdef __DUNGEON_INFO_ENABLE__
enum class EDIHighScoreTypes : int
{
	RUN_COUNT,
	QUICKER_RUN,
	GREATEST_DAMAGE,
	MAX_NUM,
};

enum class EDIEnums : int
{
	DUNGEON_INFO_RANKING_MAX = 100,
	DUNGEON_INFO_RANKIGN_CACHE = 60*30,
};

typedef struct SPacketGDDIRanking
{
	WORD wPos;
	int iType;
	char sKey[CHARACTER_NAME_MAX_LEN+1];
	char sName[CHARACTER_NAME_MAX_LEN+1];
	WORD wLevel;
	int64_t lValue;
} TPacketGDDIRanking;

typedef struct SPacketGDDIRankingBoot
{
	char sKey[CHARACTER_NAME_MAX_LEN+1];
} TPacketGDDIRankingBoot;
#endif

#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
typedef struct SPacketGGCreateDungeonInstance
{
	BYTE bHeader;
	int32_t lX;
	int32_t lY;
	int32_t lMapIndex;
	BYTE bChannel;
	int32_t lAddr;
	WORD wPort;
	bool bRequest;
	BYTE bCount;
	DWORD aPids[32]; // Better move PARTY_MAX_NUM to tables.h
} TPacketGGCreateDungeonInstance;
#endif

#ifdef ENABLE_EVENT_MANAGER
typedef struct event_struct_
{
	WORD	eventID;
	BYTE	eventIndex;
	int		startTime;
	int		endTime;
	BYTE	empireFlag;
	BYTE	channelFlag;
	DWORD	value[4];
	bool	eventStatus;
	bool	eventTypeOnlyStart;
	char	startTimeText[25];
	char	endTimeText[25];
}TEventManagerData;
enum
{
	EVENT_MANAGER_LOAD,
	EVENT_MANAGER_EVENT_STATUS,
	EVENT_MANAGER_REMOVE_EVENT,
	EVENT_MANAGER_UPDATE,
	EVENT_MANAGER_PLAYER_ACTIVATED,

	BONUS_EVENT = 1,
	DOUBLE_BOSS_LOOT_EVENT = 2,
	DOUBLE_METIN_LOOT_EVENT = 3,
	DOUBLE_MISSION_BOOK_EVENT = 4,
	DUNGEON_COOLDOWN_EVENT = 5,
	DUNGEON_CHEST_LOOT_EVENT = 6,
	EMPIRE_WAR_EVENT = 7,
	MOONLIGHT_EVENT = 8,
	TOURNAMENT_EVENT = 9,
	WHELL_OF_FORTUNE_EVENT = 10,
	HALLOWEEN_EVENT = 11,
	NPC_SEARCH_EVENT = 12,
	EXP_EVENT = 13,
	ITEM_DROP_EVENT = 14,
	YANG_DROP_EVENT = 15,
	SWITCH_RARE_EVENT = 16,
	VALENTINE_EVENT = 17,
	SWITCH_AVERAGE_EVENT = 18,
	SWITCH_RUNE_EVENT = 19,
	DOUBLE_ACHIEVEMENT_POINTS = 20,
	MINING_CHEST_EVENT = 21,
	DOUBLE_ORE_EVENT = 22,
	BIGGER_STAR_CHANCE = 23,
};
#endif


#ifdef ENABLE_OFFLINE_SHOP
typedef struct SPacketGDNewOfflineShop
{
	BYTE bSubHeader;
} TPacketGDNewOfflineShop;

typedef struct SPacketDGNewOfflineShop
{
	BYTE bSubHeader;
} TPacketDGNewOfflineShop;

namespace offlineshop
{
	enum class ExpirationType
	{
		EXPIRE_NONE,
		EXPIRE_REAL_TIME,
		EXPIRE_REAL_TIME_FIRST_USE,
	};	

	typedef struct SPriceInfo
	{
		int64_t illYang;
		SPriceInfo() : illYang(0)
		{}

		bool operator < (const SPriceInfo& rItem) const
		{
			return GetTotalYangAmount() < rItem.GetTotalYangAmount();
		}

		int64_t GetTotalYangAmount() const
		{
			int64_t total = illYang;
			return total;
		}
	} TPriceInfo;

#ifdef ENABLE_OFFLINE_SHOP_GRID
	typedef struct SItemDisplayPos
	{
		BYTE pos;
		BYTE page;
	}TItemDisplayPos;
#endif

	typedef struct SItemInfoEx
	{
		DWORD dwVnum;
		DWORD dwCount;
		int32_t alSockets[ITEM_SOCKET_MAX_NUM];
		TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef __ENABLE_CHANGELOOK_SYSTEM__
		DWORD dwTransmutation;
#endif
		ExpirationType expiration;
#ifdef ENABLE_OFFLINE_SHOP_GRID
		TItemDisplayPos display_pos;
#endif
	} TItemInfoEx;

	typedef struct SItemInfo
	{
		DWORD dwOwnerID, dwItemID;
		TPriceInfo price;
		TItemInfoEx item;
		char dwOwnerName[CHARACTER_NAME_MAX_LEN + 1];
	} TItemInfo;

	typedef struct SShopInfo
	{
		DWORD dwOwnerID;
		DWORD dwDuration;
		char szName[OFFLINE_SHOP_NAME_MAX_LEN];
#ifdef ENABLE_SHOP_DECORATION
		DWORD	dwShopDecoration;
#endif
		DWORD dwCount;
	} TShopInfo;

#ifdef ENABLE_IRA_REWORK 
	typedef struct SShopPosition
	{
		int32_t lMapIndex;
		int32_t x, y;
		BYTE bChannel;
	} TShopPosition;
#endif

	enum eNewOfflineshopSubHeaderGD
	{
		SUBHEADER_GD_BUY_ITEM = 0,
		SUBHEADER_GD_BUY_LOCK_ITEM,
		SUBHEADER_GD_CANNOT_BUY_LOCK_ITEM,
		SUBHEADER_GD_EDIT_ITEM,
		SUBHEADER_GD_REMOVE_ITEM,
		SUBHEADER_GD_ADD_ITEM,
		SUBHEADER_GD_SHOP_FORCE_CLOSE,
		SUBHEADER_GD_SHOP_CREATE_NEW,
		SUBHEADER_GD_SHOP_CHANGE_NAME,
		SUBHEADER_GD_SAFEBOX_GET_ITEM,
		SUBHEADER_GD_SAFEBOX_GET_VALUTES,
		SUBHEADER_GD_SAFEBOX_ADD_ITEM,
#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
		SUBHEADER_GD_SHOP_EXTEND_TIME,
#endif
	};

	typedef struct SSubPacketGDBuyItem
	{
		DWORD dwOwnerID, dwItemID, dwGuestID;
	} TSubPacketGDBuyItem;

	typedef struct SSubPacketGDLockBuyItem
	{
		DWORD dwOwnerID, dwItemID, dwGuestID;
	} TSubPacketGDLockBuyItem;

	typedef struct SSubPacketGDCannotBuyLockItem
	{
		DWORD dwOwnerID, dwItemID;
	} TSubPacketGDCannotBuyLockItem;

	typedef struct SSubPacketGDEditItem
	{
		DWORD dwOwnerID, dwItemID;
		TPriceInfo priceInfo;
		bool allEdit;
	} TSubPacketGDEditItem;

	typedef struct SSubPacketGDRemoveItem
	{
		DWORD dwOwnerID;
		DWORD dwItemID;
	} TSubPacketGDRemoveItem;

	typedef struct SSubPacketGDAddItem
	{
		DWORD dwOwnerID;
		TItemInfo itemInfo;
	} TSubPacketGDAddItem;

	typedef struct SSubPacketGDShopForceClose
	{
		DWORD dwOwnerID;
	} TSubPacketGDShopForceClose;

	typedef struct SSubPacketGDShopCreateNew
	{
		TShopInfo shop;
#ifdef ENABLE_IRA_REWORK
		TShopPosition pos;
#endif
	} TSubPacketGDShopCreateNew;

	typedef struct SSubPacketGDShopChangeName
	{
		DWORD dwOwnerID;
		char szName[OFFLINE_SHOP_NAME_MAX_LEN];
	} TSubPacketGDShopChangeName;

	typedef struct SSubPacketGDSafeboxGetItem
	{
		DWORD dwOwnerID;
		DWORD dwItemID;
	} TSubPacketGDSafeboxGetItem;

	typedef struct SSubPacketGDSafeboxAddItem
	{
		DWORD			dwOwnerID;
		TItemInfoEx		item;
	} TSubPacketGDSafeboxAddItem;

	typedef struct SSubPacketGDSafeboxGetValutes
	{
		DWORD dwOwnerID;
		uint64_t valute;
	} TSubPacketGDSafeboxGetValutes;

#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
	typedef struct extendTimeGameDB
	{
		DWORD	dwOwnerID;
		DWORD	dwTime;
	} TSubPacketGDShopExtendTime;

	typedef struct extendTimeDBGame
	{
		DWORD dwOwnerID;
		DWORD dwTime;
	} TSubPacketDGShopExtendTime;
#endif

	enum eSubHeaderDGNewOfflineshop
	{
		SUBHEADER_DG_BUY_ITEM,
		SUBHEADER_DG_LOCKED_BUY_ITEM,
		SUBHEADER_DG_EDIT_ITEM,
		SUBHEADER_DG_REMOVE_ITEM,
		SUBHEADER_DG_ADD_ITEM,
		SUBHEADER_DG_SHOP_FORCE_CLOSE,
		SUBHEADER_DG_SHOP_CREATE_NEW,
		SUBHEADER_DG_SHOP_CHANGE_NAME,
		SUBHEADER_DG_SHOP_EXPIRED,
		SUBHEADER_DG_LOAD_TABLES,
		SUBHEADER_DG_SAFEBOX_ADD_ITEM,
		SUBHEADER_DG_SAFEBOX_ADD_VALUTES,
		SUBHEADER_DG_SAFEBOX_LOAD,
		SUBHEADER_DG_SAFEBOX_EXPIRED_ITEM,
#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
		SUBHEADER_DG_SHOP_EXTEND_TIME,
#endif
	};

	typedef struct SSubPacketDGBuyItem
	{
		DWORD dwOwnerID, dwItemID, dwBuyerID;
	} TSubPacketDGBuyItem;

	typedef struct SSubPacketDGLockedBuyItem
	{
		DWORD dwOwnerID, dwItemID, dwBuyerID;
	} TSubPacketDGLockedBuyItem;

	typedef struct SSubPacketDGEditItem
	{
		DWORD dwOwnerID, dwItemID;
		TPriceInfo price;
	} TSubPacketDGEditItem;

	typedef struct SSubPacketDGRemoveItem
	{
		DWORD dwOwnerID, dwItemID;
	} TSubPacketDGRemoveItem;

	typedef struct SSubPacketDGAddItem
	{
		DWORD dwOwnerID, dwItemID;
		TItemInfo item;
	} TSubPacketDGAddItem;

	typedef struct SSubPacketDGShopForceClose
	{
		DWORD dwOwnerID;
	} TSubPacketDGShopForceClose;

	typedef struct SSubPacketDGShopCreateNew
	{
		TShopInfo shop;
#ifdef ENABLE_IRA_REWORK
		TShopPosition pos;
#endif
	} TSubPacketDGShopCreateNew;

	typedef struct SSubPacketDGShopChangeName
	{
		DWORD dwOwnerID;
		char szName[OFFLINE_SHOP_NAME_MAX_LEN];
	} TSubPacketDGShopChangeName;

	typedef struct SSubPacketDGLoadTables
	{
		DWORD dwShopCount;
	} TSubPacketDGLoadTables;

	typedef struct SSubPacketDGShopExpired
	{
		DWORD dwOwnerID;
	} TSubPacketDGShopExpired;

	typedef struct SSubPacketDGSafeboxAddItem
	{
		DWORD dwOwnerID, dwItemID;
		TItemInfoEx item;
	} TSubPacketDGSafeboxAddItem;

	typedef struct SSubPacketDGSafeboxAddValutes
	{
		DWORD dwOwnerID;
		uint64_t valute;
	} TSubPacketDGSafeboxAddValutes;

	typedef struct SSubPacketDGSafeboxLoad
	{
		DWORD dwOwnerID;
		uint64_t valute;
		DWORD dwItemCount;
	} TSubPacketDGSafeboxLoad;

	typedef struct SSubPacketDGSafeboxExpiredItem
	{
		DWORD dwOwnerID;
		DWORD dwItemID;
	} TSubPacketDGSafeboxExpiredItem;
}
#endif

#ifdef RANKING_SYSTEM
typedef struct SLoadServerRanking
{
	uint8_t		catIdx;
	uint32_t	playerID;
} TLoadServerRanking;

typedef struct SLoadServerRankingMyPos
{
	uint8_t		catIdx;
	uint32_t	playerID;
} TLoadServerRankingMyPos;

typedef struct SServerRankTable
{
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	uint8_t		level;
	uint8_t		raceWithskillGroup;
	uint8_t		empire;
	uint32_t	value;
} TServerRankTable;

typedef struct SServerRankMyData
{
	uint32_t	position;
	uint32_t	value;
} TServerRankMyData;

typedef struct SLoadRanking
{
	uint8_t		catIdx;
	uint32_t	playerID;
} TLoadRanking;

typedef struct SLoadRankingMyPos
{
	uint8_t		catIdx;
	uint32_t	playerID;
} TLoadRankingMyPos;

typedef struct SRankTable
{
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	uint8_t		level;
	uint8_t		race;
	uint32_t	value;
} TRankTable;

typedef struct SRankMyData
{
	uint32_t	position;
	uint32_t	value;
} TRankMyData;
#endif

#ifdef ENABLE_SECONDARY_LEVEL
typedef struct SSecondaryLevels
{
	BYTE	bLevel;
	DWORD	dwApplyType[SL_MAX_BONUSES];
	int32_t	lApplyValue[SL_MAX_BONUSES];
	DWORD	dwRequiredItemVnum[SL_MAX_ITEMS];
	int32_t	lRequiredItemCount[SL_MAX_ITEMS];
	int64_t	llRequiredGold[SL_MAX_UPGRADE_OPTIONS];
	DWORD	dwChance[SL_MAX_UPGRADE_OPTIONS];
} TSecondaryLevelProto;
#endif


#ifdef ENABLE_IN_GAME_LOG_SYSTEM
namespace InGameLog
{
	typedef struct SItemInfo
	{
		DWORD dwVnum;
		DWORD dwCount;
		long alSockets[ITEM_SOCKET_MAX_NUM];
		TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	}TItemInfo;

	typedef struct SOfflineShopSoldLog
	{
		TItemInfo item;
		unsigned long long sellPrice;
		char buyerName[24];
		char soldDate[24];
	}TOfflineShopSoldLog;

	typedef struct STradeLogCharacterInfo
	{
		DWORD pid;
		char name[24];
		uint64_t gold;
	}TTradeLogCharacterInfo;

	typedef struct STradeLogInfo
	{
		DWORD logID;
		TTradeLogCharacterInfo owner;
		TTradeLogCharacterInfo victim;
		char tradingDate[24];
	}TTradeLogInfo;

	typedef struct STradeLogInfoAdd
	{
		TTradeLogCharacterInfo owner;
		TTradeLogCharacterInfo victim;
	}TTradeLogInfoAdd;

	typedef struct STradeLogRequestInfo
	{
		DWORD logID;
		char victimName[24];
		char tradingDate[32];
	}TTradeLogRequestInfo;

	typedef struct STradeLogDetailsRequest
	{
		DWORD logID;
		BYTE itemCount;
		uint64_t ownerGold;
		uint64_t victimGold;
	}TTradeLogDetailsRequest;

	typedef struct STradeLogRequestItem
	{
		BYTE pos;
		bool isOwner;
		TItemInfo item;
	}TTradeLogRequestItem;

	typedef struct STradeLogItemInfo
	{
		DWORD ownerID;
		BYTE pos;
		TItemInfo item;
	}TTradeLogItemInfo;

	//GD
	enum IngameLogGDsubHeader
	{
		SUBHEADER_IGL_GD_OFFLINESHOP_ADDLOG,
		SUBHEADER_IGL_GD_OFFLINESHOP_REQUESTLOG,
		SUBHEADER_IGL_GD_TRADE_ADDLOG,
		SUBHEADER_IGL_GD_TRADE_REQUESTLOG,
		SUBHEADER_IGL_GD_TRADE_DETAILS_REQUESTLOG,
	};

	typedef struct SPacketGDInGameLog
	{
		BYTE subHeader;
	}TPacketGDInGameLog;

	typedef struct SOfflineShopSoldQuery
	{
		DWORD ownerID;
		TOfflineShopSoldLog log;
	}TSubPackOfflineShopAddGD;

	typedef struct SOfflineShopRequestGD
	{
		DWORD ownerID;
	}TOfflineShopRequestGD;

	typedef struct SSubPackTradeAddGD
	{
		BYTE itemCount;
		TTradeLogCharacterInfo owner;
		TTradeLogCharacterInfo victim;
	}TSubPackTradeAddGD;

	typedef struct STradeRequestGD
	{
		DWORD ownerID;
	}TTradeRequestGD;

	typedef struct STradeDetailsRequestGD
	{
		DWORD ownerID;
		DWORD logID;
	}TTradeDetailsRequestGD;
	//DG
	enum IngameLogDGsubHeader
	{
		SUBHEADER_IGL_DG_OFFLINESHOP_REQUESTLOG,
		SUBHEADER_IGL_DG_TRADE_REQUESTLOG,
		SUBHEADER_IGL_DG_TRADE_DETAILS_REQUESTLOG,
	};

	typedef struct SPacketDGInGameLog
	{
		BYTE subHeader;
	}TPacketDGInGameLog;
	
	typedef struct SInGameLogRequestDG
	{
		DWORD ownerID;
		WORD logCount;
	}TInGameLogRequestDG;

}
#endif

typedef struct SPacketGDGuildNoticeDungeonOpened
{
	uint32_t dwGuild;
	uint8_t	dungeonIdx;
	int		dungeonMapIdx;
} TPacketGDGuildNoticeDungeonOpened;

typedef struct SPacketGDGuildNoticeDungeonEnd
{
	uint32_t dwGuild;
	uint8_t		dungeonIdx;
	bool		isEndSuccess;
	uint32_t	doneTime;
} TPacketGDGuildNoticeDungeonEnd;

typedef struct SPacketGDGuildNoticeTicketsChanged
{
	uint32_t dwGuild;
	uint8_t	tickets[GUILD_TICKETS_TYPE_COUNT];
} TPacketGDGuildNoticeTicketsChanged;

typedef struct SPacketGDGuildUpdateGuardItemData
{
	uint32_t dwGuild;
	uint8_t	setID;
	uint8_t	pos;
	uint32_t vnum;
} TPacketGDGuildUpdateGuardItemData;

typedef struct SPacketGuildUpdateGuardActiveSetData
{
	uint32_t dwGuild;
	uint8_t	activeSetID;
	time_t	nextCanChangeBonusActiveStatusTime;
	time_t	guard_active_expire_time;
} TPacketGuildUpdateGuardActiveSetData;

#ifdef ENABLE_PVP_RANKING
typedef struct SPvPRankInfo
{
	int	iDead;
	int	VID;
} TPvPRankInfo;
#endif

typedef struct SLoadFishRanking
{
	uint32_t	fishVnum;
	uint32_t	playerID;
} TLoadFishRanking;

typedef struct SFishVnumRankTable
{
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	uint32_t	fishLength;
	uint8_t		rodLevel;
} TFishVnumRankTable;

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

#ifdef __DUNGEON_INFO__
typedef struct SDungeonRank
{
	char name[CHARACTER_NAME_MAX_LEN + 1];
	BYTE level;
	int	value;
}TDungeonRank;
#endif

#ifdef __ENABLE_DAMAGE_RESTRICTIONS__
typedef struct SMobDamageRestriction
{
	uint32_t dwVnum;
	int64_t llMaxHitDamage;
	int64_t llMaxSkillDamage;
	BYTE byMinLevel;
} TMonsterRestriction;
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
typedef struct SMultiFarm
{
	DWORD	playerID;
	bool	farmStatus;
	BYTE	affectType;
	int		affectTime;
	char	playerName[CHARACTER_NAME_MAX_LEN+1];
	SMultiFarm(DWORD id_, const char* playerName_, bool status_, BYTE type_, int time_) : playerID(id_), farmStatus(status_), affectType(type_), affectTime(time_){
		strlcpy(playerName, playerName_, sizeof(playerName));
	}
}TMultiFarm;
#endif

#pragma pack()
#endif
//martysama0134's ceqyqttoaf71vasf9t71218
