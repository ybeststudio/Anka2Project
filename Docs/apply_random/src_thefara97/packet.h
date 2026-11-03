#ifndef __INC_PACKET_H__
#define __INC_PACKET_H__
#include "stdafx.h"

enum
{
	HEADER_CG_HANDSHAKE				= 0xff,
	HEADER_CG_PONG					= 0xfe,
	HEADER_CG_TIME_SYNC				= 0xfc,
	HEADER_CG_KEY_AGREEMENT			= 0xfb, // _IMPROVED_PACKET_ENCRYPTION_

	HEADER_CG_LOGIN					= 1,
	HEADER_CG_ATTACK				= 2,
	HEADER_CG_CHAT					= 3,
	HEADER_CG_CHARACTER_CREATE		= 4,
	HEADER_CG_CHARACTER_DELETE		= 5,
	HEADER_CG_CHARACTER_SELECT		= 6,
	HEADER_CG_MOVE					= 7,
	HEADER_CG_SYNC_POSITION			= 8,
	HEADER_CG_ENTERGAME				= 10,

	HEADER_CG_ITEM_USE				= 11,
	HEADER_CG_ITEM_DROP				= 12,
	HEADER_CG_ITEM_MOVE				= 13,
	HEADER_CG_ITEM_PICKUP			= 15,

	HEADER_CG_QUICKSLOT_ADD			= 16,
	HEADER_CG_QUICKSLOT_DEL			= 17,
	HEADER_CG_QUICKSLOT_SWAP		= 18,
	HEADER_CG_WHISPER				= 19,
	HEADER_CG_ITEM_DROP2			= 20,

	HEADER_CG_ON_CLICK				= 26,
	HEADER_CG_EXCHANGE				= 27,
	HEADER_CG_CHARACTER_POSITION	= 28,
	HEADER_CG_SCRIPT_ANSWER			= 29,
	HEADER_CG_QUEST_INPUT_STRING	= 30,
	HEADER_CG_QUEST_CONFIRM			= 31,

	HEADER_CG_SHOP					= 50,
	HEADER_CG_FLY_TARGETING			= 51,
	HEADER_CG_USE_SKILL				= 52,
	HEADER_CG_ADD_FLY_TARGETING		= 53,
	HEADER_CG_SHOOT					= 54,
	HEADER_CG_MYSHOP				= 55,
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	HEADER_CG_OFFLINE_SHOP			= 56,
	HEADER_CG_MY_OFFLINE_SHOP		= 57,
#endif
	HEADER_CG_ITEM_USE_TO_ITEM		= 60,
	HEADER_CG_TARGET			 	= 61,

	HEADER_CG_TEXT					= 64,
	HEADER_CG_WARP					= 65,
	HEADER_CG_SCRIPT_BUTTON			= 66,
	HEADER_CG_MESSENGER				= 67,

	HEADER_CG_MALL_CHECKOUT			= 69,
	HEADER_CG_SAFEBOX_CHECKIN		= 70,
	HEADER_CG_SAFEBOX_CHECKOUT		= 71,

	HEADER_CG_PARTY_INVITE			= 72,
	HEADER_CG_PARTY_INVITE_ANSWER	= 73,
	HEADER_CG_PARTY_REMOVE			= 74,
	HEADER_CG_PARTY_SET_STATE		= 75,
	HEADER_CG_PARTY_USE_SKILL		= 76,
	HEADER_CG_SAFEBOX_ITEM_MOVE		= 77,
	HEADER_CG_PARTY_PARAMETER		= 78,

	HEADER_CG_GUILD					= 80,
	HEADER_CG_ANSWER_MAKE_GUILD		= 81,

	HEADER_CG_FISHING				= 82,

	HEADER_CG_ITEM_GIVE				= 83,

	HEADER_CG_EMPIRE				= 90,

	HEADER_CG_REFINE				= 96,

	HEADER_CG_MARK_LOGIN			= 100,
	HEADER_CG_MARK_CRCLIST			= 101,
	HEADER_CG_MARK_UPLOAD			= 102,
	HEADER_CG_MARK_IDXLIST			= 104,

	HEADER_CG_HACK					= 105,
	HEADER_CG_CHANGE_NAME			= 106,
	HEADER_CG_LOGIN2				= 109,
	HEADER_CG_DUNGEON				= 110,
	HEADER_CG_LOGIN3				= 111,

	HEADER_CG_GUILD_SYMBOL_UPLOAD	= 112,
	HEADER_CG_SYMBOL_CRC			= 113,

	// SCRIPT_SELECT_ITEM
	HEADER_CG_SCRIPT_SELECT_ITEM	= 114,
	// END_OF_SCRIPT_SELECT_ITEM

#ifdef TITLE_SYSTEM_BYLUZER
	HEADER_CG_TITLE_ACTIVE			= 129,
#endif
#ifdef WEEKLY_RANK_BYLUZER
	HEADER_CG_WEEKLY_RANK			= 130,
#endif
#ifdef __ENABLE_MINING_WITH_SPACE__
	HEADER_CG_MINING = 131,
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	HEADER_CG_POLY_SYSTEM = 138,
#endif
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
	HEADER_CG_IN_GAME_LOG			= 139,
#endif
#ifdef ENABLE_SWITCHBOT
	HEADER_CG_SWITCHBOT				= 171,
#endif
#ifdef ENABLE_SECONDARY_LEVEL
	HEADER_CG_SECONDARY_LEVEL = 181,
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	HEADER_CG_BUFF_NPC_CREATE = 186,
	HEADER_CG_BUFF_NPC_ACTION = 187,
#endif

	HEADER_CG_DRAGON_SOUL_REFINE	= 205,

	HEADER_CG_STATE_CHECKER			= 206,
#ifdef ENABLE_SAVE_LOCATION_SYSTEM
	HEADER_CG_SAVE_LOCATION = 212,
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
	HEADER_CG_WON_EXCHANGE = 222,
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
	HEADER_CG_PET_ACTION			= 223,
#endif
#ifdef ENABLE_OFFLINE_SHOP
	HEADER_CG_NEW_OFFLINESHOP = 225,
#endif
#ifdef RANKING_SYSTEM
	HEADER_CG_LOAD_SERVER_RANKING = 226,
	HEADER_CG_LOAD_RANKING = 227,
#endif
	HEADER_CG_FAST_STACK				= 230,

#ifdef ENABLE_AURA_SYSTEM
	HEADER_CG_AURA					= 232,
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
 	HEADER_CG_MOUNT_NPC_ACTION		= 233,
#endif
	HEADER_CG_GIVE_ITEMS = 234,
#if defined(__BL_MOVE_CHANNEL__)
	HEADER_CG_MOVE_CHANNEL = 240,
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_CG_PRIVATE_SHOP = 241,
#endif
	HEADER_CG_CUBE = 242,
#ifdef ENABLE_EVENT_MANAGER
	HEADER_CG_EVENT_ACTIVATE = 246,
#endif

	HEADER_CG_CLIENT_VERSION		= 0xfd,
	HEADER_CG_CLIENT_VERSION2		= 0xf1,

	/********************************************************/
	HEADER_GC_KEY_AGREEMENT_COMPLETED			= 0xfa, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_KEY_AGREEMENT						= 0xfb, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_TIME_SYNC							= 0xfc,
	HEADER_GC_PHASE								= 0xfd,
	HEADER_GC_HANDSHAKE							= 0xff,

	HEADER_GC_CHARACTER_ADD						= 1,
	HEADER_GC_CHARACTER_DEL						= 2,
	HEADER_GC_MOVE								= 3,
	HEADER_GC_CHAT								= 4,
	HEADER_GC_SYNC_POSITION						= 5,

	HEADER_GC_LOGIN_SUCCESS						= 6,
	HEADER_GC_LOGIN_SUCCESS_NEWSLOT				= 32,
	HEADER_GC_LOGIN_FAILURE						= 7,

	HEADER_GC_CHARACTER_CREATE_SUCCESS			= 8,
	HEADER_GC_CHARACTER_CREATE_FAILURE			= 9,
	HEADER_GC_CHARACTER_DELETE_SUCCESS			= 10,
	HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID	= 11,

	HEADER_GC_ATTACK							= 12,
	HEADER_GC_STUN								= 13,
	HEADER_GC_DEAD								= 14,

	HEADER_GC_MAIN_CHARACTER_OLD				= 15,
	HEADER_GC_CHARACTER_POINTS					= 16,
	HEADER_GC_CHARACTER_POINT_CHANGE			= 17,
	HEADER_GC_CHANGE_SPEED						= 18,
	HEADER_GC_CHARACTER_UPDATE					= 19,

	HEADER_GC_ITEM_DEL							= 20,
	HEADER_GC_ITEM_SET							= 21,
	HEADER_GC_ITEM_USE							= 22,
	HEADER_GC_ITEM_DROP							= 23,
	HEADER_GC_ITEM_UPDATE						= 25,

	HEADER_GC_ITEM_GROUND_ADD					= 26,
	HEADER_GC_ITEM_GROUND_DEL					= 27,

	HEADER_GC_QUICKSLOT_ADD						= 28,
	HEADER_GC_QUICKSLOT_DEL						= 29,
	HEADER_GC_QUICKSLOT_SWAP					= 30,

	HEADER_GC_ITEM_OWNERSHIP					= 31,

	HEADER_GC_WHISPER							= 34,

	HEADER_GC_MOTION							= 36,
	HEADER_GC_PARTS								= 37,

	HEADER_GC_SHOP								= 38,
	HEADER_GC_SHOP_SIGN							= 39,

	HEADER_GC_DUEL_START						= 40,
	HEADER_GC_PVP                               = 41,
	HEADER_GC_EXCHANGE							= 42,
	HEADER_GC_CHARACTER_POSITION				= 43,

	HEADER_GC_PING								= 44,
	HEADER_GC_SCRIPT							= 45,
	HEADER_GC_QUEST_CONFIRM						= 46,
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	HEADER_GC_OFFLINE_SHOP						= 47,
	HEADER_GC_OFFLINE_SHOP_SIGN					= 48,
	HEADER_GC_GOLD								= 49,
	HEADER_GC_OFFLINE_SHOP_LOG					= 52,
#endif


	HEADER_GC_ANTYEXP 							= 58,
#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
	HEADER_GC_WPADANIE							= 59,
#endif

	HEADER_GC_MOUNT								= 61,
	HEADER_GC_OWNERSHIP							= 62,
	HEADER_GC_TARGET			 				= 63,

	HEADER_GC_WARP								= 65,

	HEADER_GC_ADD_FLY_TARGETING					= 69,
	HEADER_GC_CREATE_FLY						= 70,
	HEADER_GC_FLY_TARGETING						= 71,
	HEADER_GC_SKILL_LEVEL_OLD					= 72,
	HEADER_GC_SKILL_LEVEL						= 76,

	HEADER_GC_MESSENGER							= 74,
	HEADER_GC_GUILD								= 75,

	HEADER_GC_PARTY_INVITE						= 77,
	HEADER_GC_PARTY_ADD							= 78,
	HEADER_GC_PARTY_UPDATE						= 79,
	HEADER_GC_PARTY_REMOVE						= 80,
	HEADER_GC_QUEST_INFO						= 81,
	HEADER_GC_REQUEST_MAKE_GUILD				= 82,
	HEADER_GC_PARTY_PARAMETER					= 83,

	HEADER_GC_SAFEBOX_SET						= 85,
	HEADER_GC_SAFEBOX_DEL						= 86,
	HEADER_GC_SAFEBOX_WRONG_PASSWORD			= 87,
	HEADER_GC_SAFEBOX_SIZE						= 88,

	HEADER_GC_FISHING							= 89,

	HEADER_GC_EMPIRE							= 90,

	HEADER_GC_PARTY_LINK						= 91,
	HEADER_GC_PARTY_UNLINK						= 92,
#ifdef ENABLE_ITEMSHOP
	HEADER_GC_ITEMSHOP = 94,
#endif

	HEADER_GC_REFINE_INFORMATION_OLD			= 95,

	HEADER_GC_VIEW_EQUIP						= 99,

	HEADER_GC_MARK_BLOCK						= 100,
	HEADER_GC_MARK_IDXLIST						= 102,
#ifdef ENABLE_MINIMAP_DUNGEONINFO
	HEADER_GC_MINIMAP_DUNGEONINFO				= 103,
#endif
	HEADER_GC_TIME								= 106,
	HEADER_GC_CHANGE_NAME						= 107,

	HEADER_GC_DUNGEON							= 110,

	HEADER_GC_WALK_MODE							= 111,
	HEADER_GC_SKILL_GROUP						= 112,
	HEADER_GC_MAIN_CHARACTER					= 113,

	HEADER_GC_SEPCIAL_EFFECT					= 114,

	HEADER_GC_NPC_POSITION						= 115,

	HEADER_GC_LOGIN_KEY							= 118,
	HEADER_GC_REFINE_INFORMATION				= 119,
	HEADER_GC_CHANNEL							= 121,

	HEADER_GC_TARGET_UPDATE						= 123,
	HEADER_GC_TARGET_DELETE						= 124,
	HEADER_GC_TARGET_CREATE						= 125,

	HEADER_GC_AFFECT_ADD						= 126,
	HEADER_GC_AFFECT_REMOVE						= 127,

	HEADER_GC_MALL_OPEN							= 122,
	HEADER_GC_MALL_SET							= 128,
	HEADER_GC_MALL_DEL							= 129,

	HEADER_GC_LAND_LIST							= 130,
	HEADER_GC_LOVER_INFO						= 131,
	HEADER_GC_LOVE_POINT_UPDATE					= 132,

	HEADER_GC_SYMBOL_DATA						= 133,

	// MINING
	HEADER_GC_DIG_MOTION						= 134,
	// END_OF_MINING

	HEADER_GC_DAMAGE_INFO						= 135,
	HEADER_GC_CHAR_ADDITIONAL_INFO				= 136,

	// SUPPORT_BGM
	HEADER_GC_MAIN_CHARACTER3_BGM				= 137,
	HEADER_GC_MAIN_CHARACTER4_BGM_VOL			= 138,
	// END_OF_SUPPORT_BGM

	HEADER_GC_AUTH_SUCCESS						= 150,

	HEADER_GC_PANAMA_PACK						= 151,

	//HYBRID CRYPT
	HEADER_GC_HYBRIDCRYPT_KEYS					= 152,
	HEADER_GC_HYBRIDCRYPT_SDB					= 153, // SDB means Supplmentary Data Blocks
	//HYBRID CRYPT
#ifdef ENABLE_COLLECT_WINDOW
	HEADER_GC_COLLECT							= 155,
#endif
#ifdef ENABLE_AURA_SYSTEM
	HEADER_GC_AURA = 157,
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	HEADER_GC_TITLE_INFO = 163,
	HEADER_GC_TITLE_ACTIVE = 164,
#endif
#ifdef WEEKLY_RANK_BYLUZER
	HEADER_GC_WEEKLY_RANK				= 165,
	HEADER_GC_WEEKLY_PAGE				= 166,
#endif
#ifdef ENABLE_IN_GAME_LOG_SYSTEM
	HEADER_GC_IN_GAME_LOG				= 167,
#endif
#ifdef ENABLE_SWITCHBOT
	HEADER_GC_SWITCHBOT						= 171,
#endif
#ifdef ENABLE_EVENT_MANAGER
	HEADER_GC_EVENT_MANAGER 				= 172,
#endif

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
	HEADER_GC_LEGEND_DAMAGE_DATA				= 173,
#endif
#ifdef ENABLE_OFFLINE_SHOP
	HEADER_GC_NEW_OFFLINESHOP = 174,
#endif
#ifdef RANKING_SYSTEM
	HEADER_GC_SERVER_RANKING_LOAD = 175,
	HEADER_GC_RANKING_LOAD = 176,
#endif
#ifdef _ENABLE_BATTLEPASS_
	HEADER_GC_BATTLE_PASS						= 177,
#endif
#ifdef ENABLE_SECONDARY_LEVEL
	HEADER_GC_SECONDARY_LEVEL = 181,
#endif
#ifdef __ENABLE_POLYMORPH_SYSTEM__
	HEADER_GC_POLY_SYSTEM = 183,
#endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
	HEADER_GC_BUFF_NPC_ACTION = 188,
	HEADER_GC_BUFF_NPC_USE_SKILL = 189,
#endif
#ifdef ENABLE_NOTIFICATION_SYSTEM
	HEADER_GC_NOTIFICATION = 190,
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	HEADER_GC_TITLE_ACHIEVEMENT				= 192,
#endif

	// ROULETTE
	HEADER_GC_ROULETTE							= 200,
	// END_ROULETTE

	HEADER_GC_SPECIFIC_EFFECT					= 208,

	HEADER_GC_DRAGON_SOUL_REFINE				= 209,
	HEADER_GC_RESPOND_CHANNELSTATUS				= 210,
#ifdef __INVENTORY_BUFFERING__
	HEADER_GC_ITEM_BUFFERED = 213,
#endif
#ifdef ENABLE_SAVE_LOCATION_SYSTEM
	HEADER_GC_SAVE_LOCATION = 216,
#endif
#ifdef ENABLE_ENTITY_PRELOADING
	HEADER_GC_PRELOAD_ENTITIES 					= 217,
#endif
#if defined(__BL_KILL_BAR__)
	HEADER_GC_KILLBAR 						= 218,
#endif
#ifdef ENABLE_PVP_RANKING
	HEADER_GC_PVP_RANKING	= 222,
#endif
#ifdef ENABLE_VS_SHOP_SEARCH
	HEADER_CG_SEARCH = 226,
#endif

	HEADER_GC_GIVE_ITEMS						= 234,
// #ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// 	HEADER_GC_MOUNT_NPC_ACTION 					= 235,
// #endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_GC_PRIVATE_SHOP = 235,
#endif
	HEADER_GC_CUBE = 236,
#ifdef ENABLE_MOUNT_SYSTEM
	HEADER_GC_MOUNT_INFO						= 238,
#endif
	HEADER_GC_SMART_PACKET,
#ifdef ENABLE_EVENT_MANAGER
	HEADER_GC_EVENT_ACTIVATE_RESULT				= 240,
#endif
	/////////////////////////////////////////////////////////////////////////////

	HEADER_GG_LOGIN				= 1,
	HEADER_GG_LOGOUT				= 2,
	HEADER_GG_RELAY				= 3,
	HEADER_GG_NOTICE				= 4,
	HEADER_GG_SHUTDOWN				= 5,
	HEADER_GG_GUILD				= 6,
	HEADER_GG_DISCONNECT			= 7,
	HEADER_GG_SHOUT				= 8,
	HEADER_GG_SETUP				= 9,
	HEADER_GG_MESSENGER_ADD                     = 10,
	HEADER_GG_MESSENGER_REMOVE                  = 11,
	HEADER_GG_FIND_POSITION			= 12,
	HEADER_GG_WARP_CHARACTER			= 13,
	HEADER_GG_MESSENGER_MOBILE			= 14,
	HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX		= 15,
	HEADER_GG_TRANSFER				= 16,
	HEADER_GG_XMAS_WARP_SANTA			= 17,
	HEADER_GG_XMAS_WARP_SANTA_REPLY		= 18,
	HEADER_GG_RELOAD_CRC_LIST			= 19,
	HEADER_GG_LOGIN_PING			= 20,
	HEADER_GG_CHECK_CLIENT_VERSION		= 21,
	HEADER_GG_BLOCK_CHAT			= 22,

	HEADER_GG_SIEGE					= 25,
	HEADER_GG_MONARCH_NOTICE		= 26,
	HEADER_GG_MONARCH_TRANSFER		= 27,

	HEADER_GG_CHECK_AWAKENESS		= 29,
#ifdef ENABLE_FULL_NOTICE
	HEADER_GG_BIG_NOTICE			= 30,
#endif
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	HEADER_GG_REMOVE_OFFLINE_SHOP           = 31,
	HEADER_GG_CHANGE_OFFLINE_SHOP_TIME      = 32,
	HEADER_GG_OFFLINE_SHOP_BUY              = 33,
#endif

#ifdef ENABLE_SWITCHBOT
	HEADER_GG_SWITCHBOT					= 34,
#endif
#ifdef __PREMIUM_PRIVATE_SHOP__
	HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH_RESULT,
	HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH,
	HEADER_GG_PRIVATE_SHOP_ITEM_SEARCH_UPDATE,
#endif
#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
	HEADER_GG_CREATE_DUNGEON_INSTANCE,
	#ifdef __DUNGEON_RETURN_ENABLE__
	HEADER_GG_REJOIN_DUNGEON,
	HEADER_GG_CHECK_REJOIN_DUNGEON,
	#endif
#endif
	HEADER_GG_RELOAD,
#ifdef __TEAM_MEMBER_STATUS__
	HEADER_GG_TEAM_MEMBER,
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	HEADER_GG_CMDCHAT,
#endif
#ifdef __WORLD_BOSS_YUMA__
	HEADER_GG_NEW_NOTICE,
#endif
	HEADER_GG_WORLD_BOSS_ALIVE,
	HEADER_GG_DISCONNECT_PLAYER,
#ifdef ENABLE_MESSENGER_BLOCK
	HEADER_GG_MESSENGER_BLOCK_ADD,
	HEADER_GG_MESSENGER_BLOCK_REMOVE,
#endif
	HEADER_GG_HANDSHAKE_VALIDATION,
#ifdef ENABLE_MULTI_FARM_BLOCK
	HEADER_GG_MULTI_FARM = 99,
#endif
};

#pragma pack(1)
#ifdef __TEAM_MEMBER_STATUS__
enum ETeamListSubHeaders
{
	SUBHEADER_GG_TEAM_MEMBER_ADD,
	SUBHEADER_GG_TEAM_MEMBER_REMOVE,
	SUBHEADER_GG_TEAM_MEMBER_RELOAD,
};
typedef struct SPacketGGTeamList
{
	BYTE bHeader;
	BYTE bSubHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGTeamList;
#endif
typedef struct SPacketGGSetup
{
	BYTE	bHeader;
	WORD	wPort;
	BYTE	bChannel;
} TPacketGGSetup;

typedef struct SPacketGGLogin
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD	dwPID;
	BYTE	bEmpire;
	int32_t	lMapIndex;
	BYTE	bChannel;
} TPacketGGLogin;

typedef struct SPacketGGLogout
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGLogout;

typedef struct SPacketGGRelay
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	int32_t	lSize;
} TPacketGGRelay;

typedef struct SPacketGGNotice
{
	BYTE	bHeader;
	int32_t	lSize;
} TPacketGGNotice;

#ifdef __WORLD_BOSS_YUMA__
typedef struct SPacketGGNewNotice
{
	BYTE	bHeader;
	int32_t	lSize;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	int		iSecondsToSpawn;
} TPacketGGNewNotice;
#endif

typedef struct SPacketGGWorldBossAlive
{
	BYTE		bHeader;
	uint32_t	dwVnum;
	bool		bAlive;
	time_t		tNextSpawn;
} TPacketGGWorldBossAlive;

typedef struct SPacketGGMonarchNotice
{
	BYTE	bHeader;
	BYTE	bEmpire;
	int32_t	lSize;
} TPacketGGMonarchNotice;

//FORKED_ROAD
typedef struct SPacketGGForkedMapInfo
{
	BYTE	bHeader;
	BYTE	bPass;
	BYTE	bSungzi;
} TPacketGGForkedMapInfo;
//END_FORKED_ROAD
typedef struct SPacketGGShutdown
{
	BYTE	bHeader;
} TPacketGGShutdown;

typedef struct SPacketGGGuild
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dwGuild;
} TPacketGGGuild;

enum
{
	GUILD_SUBHEADER_GG_CHAT,
	GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS,
	GUILD_SUBHEADER_GG_DONATE_MATERIALS,
	GUILD_SUBHEADER_GG_CHANGE_EXP,
};

typedef struct SPacketGGGuildChangeExp
{
	BYTE  byLevel;
	DWORD dwExp;
} TPacketGGGuildChangeExp;

typedef struct SPacketGGGuildDonateMaterials
{
	int donate_stone;
	int donate_log;
	int donate_plywood;
} TPacketGGGuildDonateMaterials;

typedef struct SPacketGGGuildChat
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dwGuild;
	char	szText[CHAT_MAX_LEN + 1];
} TPacketGGGuildChat;

typedef struct SPacketGGParty
{
	BYTE	header;
	BYTE	subheader;
	DWORD	pid;
	DWORD	leaderpid;
} TPacketGGParty;

enum
{
	PARTY_SUBHEADER_GG_CREATE,
	PARTY_SUBHEADER_GG_DESTROY,
	PARTY_SUBHEADER_GG_JOIN,
	PARTY_SUBHEADER_GG_QUIT,
};

typedef struct SPacketGGDisconnect
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGDisconnect;

typedef struct SPacketGGShout
{
	BYTE	bHeader;
	BYTE	bEmpire;
	char	szText[CHAT_MAX_LEN + 1];
} TPacketGGShout;

typedef struct SPacketGGXmasWarpSanta
{
	BYTE	bHeader;
	BYTE	bChannel;
	int32_t	lMapIndex;
} TPacketGGXmasWarpSanta;

typedef struct SPacketGGXmasWarpSantaReply
{
	BYTE	bHeader;
	BYTE	bChannel;
} TPacketGGXmasWarpSantaReply;

typedef struct SMessengerData
{
	char        szMobile[MOBILE_MAX_LEN + 1];
} TMessengerData;

typedef struct SPacketGGMessenger
{
	BYTE        bHeader;
	char        szAccount[CHARACTER_NAME_MAX_LEN + 1];
	char        szCompanion[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGMessenger;

typedef struct SPacketGGMessengerMobile
{
	BYTE        bHeader;
	char        szName[CHARACTER_NAME_MAX_LEN + 1];
	char        szMobile[MOBILE_MAX_LEN + 1];
} TPacketGGMessengerMobile;

typedef struct SPacketGGFindPosition
{
	BYTE header;
	DWORD dwFromPID;
	DWORD dwTargetPID;
	bool bIsGM;
} TPacketGGFindPosition;

typedef struct SPacketGGWarpCharacter
{
	BYTE header;
	DWORD pid;
	int32_t x;
	int32_t y;
    int32_t lAddr;
    int32_t lMapIndex;
    WORD wPort;
} TPacketGGWarpCharacter;

//  HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX	    = 15,

typedef struct SPacketGGGuildWarMapIndex
{
	BYTE bHeader;
	DWORD dwGuildID1;
	DWORD dwGuildID2;
	int32_t lMapIndex;
} TPacketGGGuildWarMapIndex;

typedef struct SPacketGGTransfer
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	int32_t	lX, lY;
} TPacketGGTransfer;

typedef struct SPacketGGLoginPing
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGLoginPing;

typedef struct SPacketGGBlockChat
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	int32_t	lBlockDuration;
} TPacketGGBlockChat;

typedef struct SPacketGGDisconnectPlayer
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGDisconnectPlayer;

typedef struct command_text
{
	BYTE	bHeader;
} TPacketCGText;

typedef struct command_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	int32_t	lDelta;
} TPacketCGHandshake;

typedef struct command_login
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	DWORD	dwLoginKey;
	DWORD	adwClientKey[4];
} TPacketCGLogin2;

typedef struct command_login3
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
	char	pin[PASSWD_MAX_LEN + 1];
	DWORD	adwClientKey[4];
#ifdef CHECK_CLIENT_VERSION
	DWORD	client_version;
#endif
#ifdef ENABLE_HWID_SYSTEM
	char	cpu_id[CPU_ID_MAX_LEN + 1];
	char	hdd_model[HDD_MODEL_MAX_LEN + 1];
	char	machine_guid[MACHINE_GUID_MAX_LEN + 1];
	char	mac_addr[MAC_ADDR_MAX_LEN + 1];
	char	hdd_serial[HDD_SERIAL_MAX_LEN + 1];
#endif
} TPacketCGLogin3;

typedef struct packet_login_key
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
} TPacketGCLoginKey;

typedef struct packet_give_items
{
	BYTE	bHeader;
} TPacketGCGiveItem;
typedef struct packet_give_items1
{
	BYTE	bHeader;
	DWORD	itemcount;
} TPacketCGGiveItems;

typedef struct packet_received_items
{
	BYTE	bHeader;
	DWORD	item_count;
} TPacketCGReceivedItems;

typedef struct command_player_select
{
	BYTE	header;
	BYTE	index;
} TPacketCGPlayerSelect;

typedef struct command_player_delete
{
	BYTE	header;
	BYTE	index;
	char	private_code[8];
} TPacketCGPlayerDelete;

typedef struct command_player_create
{
	BYTE        header;
	BYTE        index;
	char        name[CHARACTER_NAME_MAX_LEN + 1];
	WORD        job;
	BYTE	shape;
	BYTE	Con;
	BYTE	Int;
	BYTE	Str;
	BYTE	Dex;
} TPacketCGPlayerCreate;

typedef struct command_player_create_success
{
	BYTE		header;
	BYTE		bAccountCharacterIndex;
	TSimplePlayer	player;
} TPacketGCPlayerCreateSuccess;

typedef struct command_attack
{
	BYTE	bHeader;
	BYTE	bType;
	DWORD	dwVID;
	BYTE	bCRCMagicCubeProcPiece;
	BYTE	bCRCMagicCubeFilePiece;
} TPacketCGAttack;

enum EMoveFuncType
{
	FUNC_WAIT,
	FUNC_MOVE,
	FUNC_ATTACK,
	FUNC_COMBO,
	FUNC_MOB_SKILL,
	_FUNC_SKILL,
	FUNC_MAX_NUM,
	FUNC_SKILL = 0x80,
};

typedef struct command_move
{
	BYTE	bHeader;
	BYTE	bFunc;
	BYTE	bArg;
	BYTE	bRot;
	int32_t	lX;
	int32_t	lY;
	DWORD	dwTime;
} TPacketCGMove;

typedef struct command_sync_position_element
{
	DWORD	dwVID;
	int32_t	lX;
	int32_t	lY;
} TPacketCGSyncPositionElement;

typedef struct command_sync_position
{
	BYTE	bHeader;
	WORD	wSize;
} TPacketCGSyncPosition;

typedef struct command_chat
{
	BYTE	header;
	WORD	size;
	BYTE	type;
} TPacketCGChat;

typedef struct command_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	char 	szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

typedef struct command_entergame
{
	BYTE	header;
} TPacketCGEnterGame;

typedef struct command_item_use
{
	BYTE 	header;
	TItemPos 	Cell;
} TPacketCGItemUse;

typedef struct command_item_use_to_item
{
	BYTE	header;
	TItemPos	Cell;
	TItemPos	TargetCell;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE 	header;
	TItemPos 	Cell;
	DWORD	gold;
#ifdef ENABLE_CHEQUE_SYSTEM
	DWORD	cheque;
#endif
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
	BYTE 	header;
	TItemPos 	Cell;
	DWORD	gold;
#ifdef ENABLE_CHEQUE_SYSTEM
	DWORD	cheque;
#endif
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t count;
#else
	BYTE count;
#endif
} TPacketCGItemDrop2;

typedef struct command_item_move
{
	BYTE 	header;
	TItemPos	Cell;
	TItemPos	CellTo;
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t count;
#else
	BYTE count;
#endif
} TPacketCGItemMove;

typedef struct packet_fast_stack
{
	BYTE header;
	TItemPos pos;
} TPacketCGFastStack;

typedef struct command_item_pickup
{
	BYTE 	header;
	DWORD	vid;
} TPacketCGItemPickup;

typedef struct command_quickslot_add
{
	BYTE	header;
	BYTE	pos;
	TQuickslot	slot;
} TPacketCGQuickslotAdd;

typedef struct command_quickslot_del
{
	BYTE	header;
	BYTE	pos;
} TPacketCGQuickslotDel;

typedef struct command_quickslot_swap
{
	BYTE	header;
	BYTE	pos;
	BYTE	change_pos;
} TPacketCGQuickslotSwap;

enum
{
	SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	,SHOP_SUBHEADER_CG_ADD_ITEM,
	SHOP_SUBHEADER_CG_REMOVE_ITEM,
	SHOP_SUBHEADER_CG_DESTROY_OFFLINE_SHOP,
	SHOP_SUBHEADER_CG_REFRESH,
	SHOP_SUBHEADER_CG_REFRESH_MONEY,
	SHOP_SUBHEADER_CG_REFRESH_CHEQUE,
	SHOP_SUBHEADER_CG_WITHDRAW_MONEY,
	SHOP_SUBHEADER_CG_WITHDRAW_CHEQUE,
#endif
};

typedef struct command_shop_buy
{
	BYTE	count;
} TPacketCGShopBuy;

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
typedef struct command_offlineshop_logs_packet
{
	BYTE bHeader;
	ShopLog		log[20];
} TPacketGCOfflineShopLogs;
#endif

#if defined(__BL_MOVE_CHANNEL__)
typedef struct command_move_channel
{
	BYTE		header;
	BYTE		channel;
} TPacketCGMoveChannel;
#endif

#ifdef __EXTENDED_ITEM_COUNT__
typedef struct command_shop_sell
{
	WORD wPos;
	uint16_t sCount;
	BYTE bType;
} TPacketCGShopSell;
#else
typedef struct command_shop_sell
{
	BYTE pos;
	BYTE count;
} TPacketCGShopSell;
#endif

typedef struct command_shop
{
	BYTE	header;
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t subheader;
#else
	BYTE subheader;
#endif
} TPacketCGShop;

typedef struct command_on_click
{
	BYTE	header;
	DWORD	vid;
} TPacketCGOnClick;

enum
{
	EXCHANGE_SUBHEADER_CG_START,	/* arg1 == vid of target character */
	EXCHANGE_SUBHEADER_CG_ITEM_ADD,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ITEM_DEL,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ELK_ADD,	/* arg1 == amount of gold */
#ifdef ENABLE_CHEQUE_SYSTEM
	EXCHANGE_SUBHEADER_CG_CHEQUE_ADD,
#endif
	EXCHANGE_SUBHEADER_CG_ACCEPT,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_CG_CANCEL,	/* arg1 == not used */
};

typedef struct command_exchange
{
	BYTE	header;
	BYTE	sub_header;
#ifdef ENABLE_LONG_LONG
	int64_t arg1;
#else
	DWORD	arg1;
#endif
	BYTE	arg2;
	TItemPos	Pos;
#if defined(ITEM_CHECKINOUT_UPDATE)
	bool	SelectPosAuto;
#endif
} TPacketCGExchange;

typedef struct command_position
{
	BYTE	header;
	BYTE	position;
} TPacketCGPosition;

typedef struct command_script_answer
{
	BYTE	header;
	BYTE	answer;
	//char	file[32 + 1];
	//BYTE	answer[16 + 1];
} TPacketCGScriptAnswer;

typedef struct command_script_button
{
	BYTE        header;
	unsigned int	idx;
#ifdef ENABLE_COLLECT_WINDOW
	BYTE		button;
#endif
} TPacketCGScriptButton;

typedef struct command_quest_input_string
{
	BYTE header;
	char msg[64+1];
} TPacketCGQuestInputString;

typedef struct command_quest_confirm
{
	BYTE header;
	BYTE answer;
	DWORD requestPID;
} TPacketCGQuestConfirm;

typedef struct packet_quest_confirm
{
	BYTE header;
	char msg[64+1];
	int32_t timeout;
	DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	int32_t	lDelta;
} TPacketGCHandshake;

enum EPhase
{
	PHASE_CLOSE,
	PHASE_HANDSHAKE,
	PHASE_LOGIN,
	PHASE_SELECT,
	PHASE_LOADING,
	PHASE_GAME,
	PHASE_DEAD,

	PHASE_CLIENT_CONNECTING,
	PHASE_DBCLIENT,
	PHASE_P2P,
	PHASE_AUTH,
};

typedef struct packet_phase
{
	BYTE	header;
	BYTE	phase;
} TPacketGCPhase;


enum
{
	LOGIN_FAILURE_ALREADY	= 1,
	LOGIN_FAILURE_ID_NOT_EXIST	= 2,
	LOGIN_FAILURE_WRONG_PASS	= 3,
	LOGIN_FAILURE_FALSE		= 4,
	LOGIN_FAILURE_NOT_TESTOR	= 5,
	LOGIN_FAILURE_NOT_TEST_TIME	= 6,
	LOGIN_FAILURE_FULL		= 7
};

typedef struct packet_login_success
{
	BYTE		bHeader;
	TSimplePlayer	players[PLAYER_PER_ACCOUNT];
	DWORD		guild_id[PLAYER_PER_ACCOUNT];
	char		guild_name[PLAYER_PER_ACCOUNT][GUILD_NAME_MAX_LEN+1];

	DWORD		handle;
	DWORD		random_key;
} TPacketGCLoginSuccess;

typedef struct packet_auth_success
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
	BYTE	bResult;
} TPacketGCAuthSuccess;



typedef struct packet_login_failure
{
	BYTE	header;
	char	szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
} TPacketGCLoginFailure;

typedef struct packet_create_failure
{
	BYTE	header;
	BYTE	bType;
} TPacketGCCreateFailure;

enum
{
	ADD_CHARACTER_STATE_DEAD		= (1 << 0),
	ADD_CHARACTER_STATE_SPAWN		= (1 << 1),
	ADD_CHARACTER_STATE_GUNGON		= (1 << 2),
	ADD_CHARACTER_STATE_KILLER		= (1 << 3),
	ADD_CHARACTER_STATE_PARTY		= (1 << 4),
};

enum ECharacterEquipmentPart
{
	CHR_EQUIPPART_ARMOR,
	CHR_EQUIPPART_WEAPON,
	CHR_EQUIPPART_HEAD,
	CHR_EQUIPPART_HAIR,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	CHR_EQUIPPART_ACCE,
#endif
#ifdef ENABLE_AURA_SYSTEM
	CHR_EQUIPPART_AURA,
#endif
#ifdef ENABLE_COSTUME_EMBLEMAT
	CHR_EQUIPPART_EMBLEMAT,
#endif
	CHR_EQUIPPART_NUM,
};

#ifdef ENABLE_AURA_SYSTEM
enum
{
	AURA_SUBHEADER_GC_OPEN = 0,
	AURA_SUBHEADER_GC_CLOSE,
	AURA_SUBHEADER_GC_ADDED,
	AURA_SUBHEADER_GC_REMOVED,
	AURA_SUBHEADER_GC_REFINED,
	AURA_SUBHEADER_CG_CLOSE = 0,
	AURA_SUBHEADER_CG_ADD,
	AURA_SUBHEADER_CG_REMOVE,
	AURA_SUBHEADER_CG_REFINE,
};

typedef struct SPacketAura
{
	BYTE	header;
	BYTE	subheader;
	bool	bWindow;
	DWORD	dwPrice;
	BYTE	bPos;
	TItemPos	tPos;
	DWORD	dwItemVnum;
	DWORD	dwMinAbs;
	DWORD	dwMaxAbs;
} TPacketAura;
#endif

typedef struct packet_add_char
{
	BYTE	header;
	DWORD	dwVID;

	float	angle;
	int32_t	x;
	int32_t	y;
	int32_t	z;

	BYTE	bType;
	DWORD	wRaceNum; // @fixme501
	BYTE	bMovingSpeed;
	BYTE	bAttackSpeed;

	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];
#ifdef TITLE_SYSTEM_BYLUZER
	BYTE	title_active;
#endif
} TPacketGCCharacterAdd;

typedef struct packet_char_additional_info
{
	BYTE    header;
	DWORD   dwVID;
	char    name[CHARACTER_NAME_MAX_LEN + 1];
	DWORD   awPart[CHR_EQUIPPART_NUM]; // @fixme502
	BYTE	bEmpire;
	DWORD   dwGuildID;
	DWORD   dwLevel;
#ifdef ENABLE_SECONDARY_LEVEL
	DWORD	dwSecondaryLevel;
#endif
	int64_t	sAlignment;
	BYTE	bPKMode;
	DWORD	dwMountVnum;
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	BYTE	title_active;
#endif
#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
	uint8_t	bEnlightLvl;
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	BYTE	bTitleAchievement;
	BYTE	bTitleAchievementPremium;
#endif
#ifdef __SKILL_COSTUME__
	BYTE	bSkillMotion[ESkillMotionLength::MAX_SKILL_COUNT + ESkillMotionLength::MAX_BUFF_COUNT];
#endif
} TPacketGCCharacterAdditionalInfo;

typedef struct packet_update_char
{
	BYTE	header;
	DWORD	dwVID;

	DWORD	awPart[CHR_EQUIPPART_NUM]; // @fixme502
	BYTE	bMovingSpeed;
	BYTE	bAttackSpeed;

	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];

	DWORD	dwGuildID;
	int64_t	sAlignment;
#ifdef ENABLE_NEW_PET_SYSTEM
	DWORD	dwLevel;
#endif
	BYTE	bPKMode;
	DWORD	dwMountVnum;
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
	BYTE	title_active;
#endif
#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
	BYTE	bTitleAchievement;
	BYTE	bTitleAchievementPremium;
#endif
#ifdef __SKILL_COSTUME__
	BYTE	bSkillMotion[ESkillMotionLength::MAX_SKILL_COUNT + ESkillMotionLength::MAX_BUFF_COUNT];
#endif
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE	header;
	DWORD	id;
#ifdef ENABLE_ENITY_CHARACTER_FIX
	bool fix_enity;
#endif
} TPacketGCCharacterDelete;

typedef struct packet_chat
{
	BYTE	header;
	WORD	size;
	BYTE	type;
	DWORD	id;
	BYTE	bEmpire;
#if defined(__BL_CLIENT_LOCALE_STRING__)
	bool	bCanFormat;
	packet_chat() : bCanFormat(true) {}
#endif
} TPacketGCChat;

typedef struct packet_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bType;
	char	szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
#if defined(__BL_CLIENT_LOCALE_STRING__)
	bool	bCanFormat;
	packet_whisper() : bCanFormat(true) {}
#endif
} TPacketGCWhisper;

typedef struct packet_main_character
{
	BYTE        header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	int32_t	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter;

// SUPPORT_BGM
typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	int32_t	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	DWORD	wRaceNum; // @fixme501
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	float	fBGMVol;
	int32_t	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter4_BGM_VOL;
// END_OF_SUPPORT_BGM

typedef struct packet_points
{
	BYTE	header;
#ifdef ENABLE_LONG_LONG
int64_t	points[POINT_MAX_NUM];
#else
	int		points[POINT_MAX_NUM];
#endif
} TPacketGCPoints;

typedef struct packet_skill_level
{
	BYTE		bHeader;
	TPlayerSkill	skills[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

typedef struct packet_point_change
{
	int		header;
	DWORD	dwVID;
	BYTE	type;
#ifdef ENABLE_LONG_LONG
	int64_t	amount;
	int64_t	value;
#else
	int32_t	amount;
	int32_t	value;
#endif
} TPacketGCPointChange;

typedef struct packet_stun
{
	BYTE	header;
	DWORD	vid;
} TPacketGCStun;

#ifdef RENEWAL_DEAD_PACKET
enum EReviveTypes
{
	REVIVE_TYPE_HERE,
	REVIVE_TYPE_TOWN,
	REVIVE_TYPE_AUTO_TOWN,
	REVIVE_TYPE_MAX
};
#endif
typedef struct packet_dead
{
#ifdef RENEWAL_DEAD_PACKET
	packet_dead() {	memset(&t_d, 0, sizeof(t_d)); }
#endif
	BYTE	header;
	DWORD	vid;
#ifdef RENEWAL_DEAD_PACKET
	BYTE	t_d[REVIVE_TYPE_MAX];
#endif
} TPacketGCDead;

typedef struct SPacketGGHandshakeValidate
{
	BYTE header;
	char sUserIP[64];
} TPacketGGHandshakeValidate;

struct TPacketGCItemDelDeprecated
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t count;
#else
	BYTE count;
#endif
	int32_t	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};

typedef struct packet_item_set
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t		count;
#else
	BYTE	count;
#endif
	DWORD	flags;
	DWORD	anti_flags;
	bool	highlight;
	int32_t	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TPacketGCItemSet;

typedef struct packet_item_del
{
	BYTE	header;
	BYTE	pos;
} TPacketGCItemDel;

struct packet_item_use
{
	BYTE	header;
	TItemPos Cell;
	DWORD	ch_vid;
	DWORD	victim_vid;
	DWORD	vnum;
};

struct packet_item_move
{
	BYTE	header;
	TItemPos Cell;
	TItemPos CellTo;
};

typedef struct packet_item_update
{
	BYTE	header;
	TItemPos Cell;
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t count;
#else
	BYTE count;
#endif
	int32_t	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TPacketGCItemUpdate;

typedef struct packet_item_ground_add
{
#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
	packet_item_ground_add()
	{
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttrs, 0, sizeof(aAttrs));
	}
#endif

	BYTE	bHeader;
	int32_t 	x, y, z;
	DWORD	dwVID;
	DWORD	dwVnum;
	int		count;
#ifdef ENABLE_EXTENDED_ITEMNAME_ON_GROUND
	int32_t	alSockets[ITEM_SOCKET_MAX_NUM];//3
	TPlayerItemAttribute aAttrs[ITEM_ATTRIBUTE_MAX_NUM];//7
#endif
} TPacketGCItemGroundAdd;

typedef struct packet_item_ownership
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

typedef struct packet_item_ground_del
{
	BYTE	bHeader;
	DWORD	dwVID;
} TPacketGCItemGroundDel;

struct packet_quickslot_add
{
	BYTE	header;
	BYTE	pos;
	TQuickslot	slot;
};

struct packet_quickslot_del
{
	BYTE	header;
	BYTE	pos;
};

struct packet_quickslot_swap
{
	BYTE	header;
	BYTE	pos;
	BYTE	pos_to;
};

struct packet_motion
{
	BYTE	header;
	DWORD	vid;
	DWORD	victim_vid;
	WORD	motion;
};

enum EPacketShopSubHeaders
{
	SHOP_SUBHEADER_GC_START,
	SHOP_SUBHEADER_GC_END,
	SHOP_SUBHEADER_GC_UPDATE_ITEM,
	SHOP_SUBHEADER_GC_UPDATE_PRICE,
	SHOP_SUBHEADER_GC_OK,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY,
#ifdef ENABLE_CHEQUE_SYSTEM
	SHOP_SUBHEADER_GC_NOT_ENOUGH_CHEQUE,
#endif
	SHOP_SUBHEADER_GC_SOLDOUT,
	SHOP_SUBHEADER_GC_INVENTORY_FULL,
	SHOP_SUBHEADER_GC_INVALID_POS,
	SHOP_SUBHEADER_GC_SOLD_OUT,
	SHOP_SUBHEADER_GC_START_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX,
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	SHOP_SUBHEADER_GC_UPDATE_ITEM2,
	SHOP_SUBHEADER_GC_REFRESH_MONEY,
	SHOP_SUBHEADER_GC_REFRESH_CHEQUE,
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_CHEQUE,
#endif
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	SHOP_SUBHEADER_GC_NOT_ENOUGH_PKT_OSIAG,
#endif
};

struct packet_shop_item
{
	DWORD       vnum;
#ifdef ENABLE_LONG_LONG
	int64_t price;
#else
	DWORD price;
#endif
#ifdef ENABLE_CHEQUE_SYSTEM
	int64_t cheque;
#endif
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t count;
#else
	BYTE count;
#endif
#ifdef ENABLE_BUY_WITH_ITEM
	TShopItemPrice	itemprice[MAX_SHOP_PRICES];
#endif
	BYTE		display_pos;
	int32_t	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};

typedef struct packet_shop_start
{
	DWORD   owner_vid;
	struct packet_shop_item	items[SHOP_HOST_ITEM_MAX_NUM];
#ifdef ENABLE_PUNKTY_OSIAGNIEC
	bool	ispktosiagshop;
#endif
} TPacketGCShopStart;

typedef struct packet_shop_start_ex
{
	typedef struct sub_packet_shop_tab
	{
		char name[SHOP_TAB_NAME_MAX];
		BYTE coin_type;
		packet_shop_item items[SHOP_HOST_ITEM_MAX_NUM];
	} TSubPacketShopTab;
	DWORD	owner_vid;
	//DWORD	IsOnline;
	BYTE	shop_tab_count;
} TPacketGCShopStartEx;

typedef struct packet_shop_update_item
{
	BYTE			pos;
	struct packet_shop_item	item;
} TPacketGCShopUpdateItem;

typedef struct packet_shop_update_price
{
#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	BYTE			bPos;
#endif
#ifdef ENABLE_LONG_LONG
	int64_t		iPrice;
#else
	int				iPrice;
#endif
	int64_t		iPrice2;
} TPacketGCShopUpdatePrice;

typedef struct packet_shop
{
	BYTE        header;
	WORD	size;
	BYTE        subheader;
} TPacketGCShop;

struct packet_exchange
{
	BYTE	header;
	BYTE	sub_header;
	BYTE	is_me;
#ifdef ENABLE_LONG_LONG
	int64_t arg1;
#else
	DWORD	arg1;	// vnum
#endif
	TItemPos	arg2;	// cell
	DWORD	arg3;	// count
#ifdef WJ_ENABLE_TRADABLE_ICON
	TItemPos	arg4;	// srccell
#endif
	int32_t	alSockets[ITEM_SOCKET_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};

enum EPacketTradeSubHeaders
{
	EXCHANGE_SUBHEADER_GC_START,	/* arg1 == vid */
	EXCHANGE_SUBHEADER_GC_ITEM_ADD,	/* arg1 == vnum  arg2 == pos  arg3 == count */
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,
	EXCHANGE_SUBHEADER_GC_GOLD_ADD,	/* arg1 == gold */
#ifdef ENABLE_CHEQUE_SYSTEM
	EXCHANGE_SUBHEADER_GC_CHEQUE_ADD,
#endif
	EXCHANGE_SUBHEADER_GC_ACCEPT,	/* arg1 == accept */
	EXCHANGE_SUBHEADER_GC_END,		/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_ALREADY,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_LESS_GOLD,	/* arg1 == not used */
#ifdef ENABLE_CHEQUE_SYSTEM
	EXCHANGE_SUBHEADER_GC_LESS_CHEQUE,
#endif
};

struct packet_position
{
	BYTE	header;
	DWORD	vid;
	BYTE	position;
};

typedef struct packet_ping
{
	BYTE	header;
} TPacketGCPing;

typedef struct packet_pong
{
	BYTE header;
} TPacketCGPong;

struct packet_script
{
	BYTE	header;
	WORD	size;
	BYTE	skin;
	WORD	src_size;
#ifdef ENABLE_QUEST_CATEGORY
	BYTE	quest_flag;
#endif
};

typedef struct packet_change_speed
{
	BYTE		header;
	DWORD		vid;
	WORD		moving_speed;
} TPacketGCChangeSpeed;

struct packet_mount
{
	BYTE	header;
	DWORD	vid;
	DWORD	mount_vid;
	BYTE	pos;
	DWORD	x, y;
};

typedef struct packet_move
{
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	DWORD		dwVID;
	int32_t		lX;
	int32_t		lY;
	DWORD		dwTime;
	DWORD		dwDuration;
} TPacketGCMove;

typedef struct packet_ownership
{
	BYTE		bHeader;
	DWORD		dwOwnerVID;
	DWORD		dwVictimVID;
} TPacketGCOwnership;

typedef struct packet_sync_position_element
{
	DWORD	dwVID;
	int32_t	lX;
	int32_t	lY;
} TPacketGCSyncPositionElement;

typedef struct packet_sync_position
{
	BYTE	bHeader;
	WORD	wSize;
} TPacketGCSyncPosition;

typedef struct packet_fly
{
	BYTE	bHeader;
	BYTE	bType;
	DWORD	dwStartVID;
	DWORD	dwEndVID;
} TPacketGCCreateFly;

typedef struct command_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwTargetVID;
	int32_t		x, y;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwShooterVID;
	DWORD		dwTargetVID;
	int32_t		x, y;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
	BYTE		bHeader;
	BYTE		bType;
} TPacketCGShoot;

typedef struct packet_duel_start
{
	BYTE	header;
	WORD	wSize;
} TPacketGCDuelStart;

enum EPVPModes
{
	PVP_MODE_NONE,
	PVP_MODE_AGREE,
	PVP_MODE_FIGHT,
	PVP_MODE_REVENGE
};

typedef struct packet_pvp
{
	BYTE        bHeader;
	DWORD       dwVIDSrc;
	DWORD       dwVIDDst;
	BYTE        bMode;
} TPacketGCPVP;

typedef struct command_use_skill
{
	BYTE	bHeader;
	DWORD	dwVnum;
	DWORD	dwVID;
} TPacketCGUseSkill;

typedef struct command_target
{
	BYTE	header;
	DWORD	dwVID;
} TPacketCGTarget;

typedef struct packet_target
{
	BYTE	header;
	DWORD	dwVID;
	// BYTE	bHPPercent;
#ifdef __VIEW_TARGET_DECIMAL_HP__
	int64_t	iMinHP;
	int64_t	iMaxHP;
#endif
} TPacketGCTarget;

typedef struct packet_warp
{
	BYTE	bHeader;
	int32_t	lX;
	int32_t	lY;
	int32_t	lAddr;
	WORD	wPort;
} TPacketGCWarp;

typedef struct command_warp
{
	BYTE	bHeader;
} TPacketCGWarp;

struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
#ifdef __QUEST_RENEWAL__
	WORD c_index;
#endif
	BYTE flag;
};

enum 
{
	MESSENGER_SUBHEADER_GC_LIST,
	MESSENGER_SUBHEADER_GC_LOGIN,
	MESSENGER_SUBHEADER_GC_LOGOUT,
	MESSENGER_SUBHEADER_GC_INVITE,
	MESSENGER_SUBHEADER_GC_MOBILE,
#ifdef ENABLE_MESSENGER_BLOCK
	MESSENGER_SUBHEADER_GC_BLOCK_LIST,
	MESSENGER_SUBHEADER_GC_BLOCK_LOGIN,
	MESSENGER_SUBHEADER_GC_BLOCK_LOGOUT,
	MESSENGER_SUBHEADER_GC_REMOVE_BLOCK,
#endif
};

typedef struct packet_messenger
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCMessenger;

typedef struct packet_messenger_guild_list
{
	BYTE connected;
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildList;

typedef struct packet_messenger_guild_login
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogin;

typedef struct packet_messenger_guild_logout
{
	BYTE length;

	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogout;

#ifdef ENABLE_MESSENGER_BLOCK
typedef struct packet_messenger_block_list_offline
{
	BYTE connected; // always 0
	BYTE length;
} TPacketGCMessengerBlockListOffline;

typedef struct packet_messenger_block_list_online
{
	BYTE connected; // always 1
	BYTE length;
} TPacketGCMessengerBlockListOnline;
#endif

typedef struct packet_messenger_list_offline
{
	BYTE connected; // always 0
	BYTE length;
} TPacketGCMessengerListOffline;

typedef struct packet_messenger_list_online
{
	BYTE connected; // always 1
	BYTE length;
} TPacketGCMessengerListOnline;

enum
{
	MESSENGER_SUBHEADER_CG_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_REMOVE,
	MESSENGER_SUBHEADER_CG_INVITE_ANSWER,
#ifdef ENABLE_MESSENGER_BLOCK
	MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_BLOCK_REMOVE,
	MESSENGER_SUBHEADER_CG_BLOCK_REMOVE_BY_VID,
#endif
};

typedef struct command_messenger
{
	BYTE header;
	BYTE subheader;
} TPacketCGMessenger;

typedef struct command_messenger_add_by_vid
{
	DWORD vid;
} TPacketCGMessengerAddByVID;

typedef struct command_messenger_add_by_name
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketCGMessengerAddByName;

typedef struct command_messenger_remove
{
	char login[LOGIN_MAX_LEN+1];
	//DWORD account;
} TPacketCGMessengerRemove;

#ifdef ENABLE_MESSENGER_BLOCK
typedef struct command_messenger_add_block_by_vid
{
	DWORD vid;
} TPacketCGMessengerAddBlockByVID;

typedef struct command_messenger_remove_block_by_vid
{
	DWORD vid;
} TPacketCGMessengerRemoveBlockByVID;

typedef struct command_messenger_add_block_by_name
{
	BYTE length;
} TPacketCGMessengerAddBlockByName;
#endif

typedef struct command_safebox_checkout
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
} TPacketCGSafeboxCheckout;

typedef struct command_safebox_checkin
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
} TPacketCGSafeboxCheckin;

///////////////////////////////////////////////////////////////////////////////////
// Party

typedef struct command_party_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketCGPartyParameter;

typedef struct paryt_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketGCPartyParameter;

typedef struct packet_party_add
{
	BYTE	header;
	DWORD	pid;
	char	name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCPartyAdd;

typedef struct command_party_invite
{
	BYTE	header;
	DWORD	vid;
} TPacketCGPartyInvite;

typedef struct packet_party_invite
{
	BYTE	header;
	DWORD	leader_vid;
} TPacketGCPartyInvite;

typedef struct command_party_invite_answer
{
	BYTE	header;
	DWORD	leader_vid;
	BYTE	accept;
} TPacketCGPartyInviteAnswer;

typedef struct packet_party_update
{
	BYTE	header;
	DWORD	pid;
	BYTE	role;
	BYTE	percent_hp;
	short	affects[7];
#if defined(__BL_PARTY_POSITION__)
	int32_t	x;
	int32_t	y;
#endif
} TPacketGCPartyUpdate;

typedef struct packet_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketGCPartyRemove;

typedef struct packet_party_link
{
	BYTE header;
	DWORD pid;
	DWORD vid;
} TPacketGCPartyLink;

typedef struct packet_party_unlink
{
	BYTE header;
	DWORD pid;
	DWORD vid;
} TPacketGCPartyUnlink;

typedef struct command_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
	BYTE header;
	DWORD pid;
	BYTE byRole;
	BYTE flag;
} TPacketCGPartySetState;

enum
{
	PARTY_SKILL_HEAL = 1,
	PARTY_SKILL_WARP = 2
};

typedef struct command_party_use_skill
{
	BYTE header;
	BYTE bySkillIndex;
	DWORD vid;
} TPacketCGPartyUseSkill;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketCGSafeboxSize;

typedef struct packet_safebox_wrong_password
{
	BYTE	bHeader;
} TPacketCGSafeboxWrongPassword;

typedef struct command_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketCGEmpire;

typedef struct packet_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketGCEmpire;

enum
{
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
	BYTE        bHeader;
	BYTE        bState;
	int32_t	lMoney;
} TPacketCGSafeboxMoney;

typedef struct packet_safebox_money_change
{
	BYTE	bHeader;
	int32_t	lMoney;
} TPacketGCSafeboxMoneyChange;

// Guild

enum
{
	GUILD_SUBHEADER_GC_LOGIN,
	GUILD_SUBHEADER_GC_LOGOUT,
	GUILD_SUBHEADER_GC_LIST,
	GUILD_SUBHEADER_GC_GRADE,
	GUILD_SUBHEADER_GC_ADD,
	GUILD_SUBHEADER_GC_REMOVE,
	GUILD_SUBHEADER_GC_GRADE_NAME,
	GUILD_SUBHEADER_GC_GRADE_AUTH,
	GUILD_SUBHEADER_GC_INFO,
	GUILD_SUBHEADER_GC_COMMENTS,
	GUILD_SUBHEADER_GC_CHANGE_EXP,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_GC_SKILL_INFO,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_GC_GUILD_INVITE,
	GUILD_SUBHEADER_GC_WAR,
	GUILD_SUBHEADER_GC_GUILD_NAME,
	GUILD_SUBHEADER_GC_GUILD_WAR_LIST,
	GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST,
	GUILD_SUBHEADER_GC_WAR_SCORE,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
	GUILD_SUBHEADER_GC_GUARD_ITEMS_SET_DATA,
	GUILD_SUBHEADER_GC_GUARD_ITEM_REMOVE_DATA,
	GUILD_SUBHEADER_GC_GUARD_BASE_INFO,
	GUILD_SUBHEADER_GC_NOTE,
	GUILD_SUBHEADER_GC_DUNGEON_OPEN_INFO,
	GUILD_SUBHEADER_GC_REFRESH_TICKETS,
	GUILD_SUBHEADER_GC_DUNGEON_HISTORY_DATA,
	GUILD_SUBHEADER_GC_DUNGEON_BOSS_TOP_DMG,
	GUILD_SUBHEADER_GC_MY_GIVEN_EXP_UPDATE,
	GUILD_SUBHEADER_GC_SET_LOGS,
	GUILD_SUBHEADER_GC_STORAGE_INFO,
};

enum GUILD_SUBHEADER_CG
{
	GUILD_SUBHEADER_CG_ADD_MEMBER,
	GUILD_SUBHEADER_CG_REMOVE_MEMBER,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY,
	GUILD_SUBHEADER_CG_OFFER,
	GUILD_SUBHEADER_CG_POST_COMMENT,
	GUILD_SUBHEADER_CG_DELETE_COMMENT,
	GUILD_SUBHEADER_CG_REFRESH_COMMENT,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_CG_USE_SKILL,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER,
	GUILD_SUBHEADER_CG_CHARGE_GSP,
	GUILD_SUBHEADER_CG_DEPOSIT_MONEY,
	GUILD_SUBHEADER_CG_WITHDRAW_MONEY,
	GUILD_SUBHEADER_CG_REFRESH_GUARD_ITEMS,
	GUILD_SUBHEADER_CG_SET_NEW_NOTE,
	GUILD_SUBHEADER_CG_REFRESH_NOTE,
	GUILD_SUBHEADER_CG_REFRESH_DUNGEON_HISTORY,
	GUILD_SUBHEADER_CG_REFRESH_LOGS,
};

typedef struct packet_guild
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCGuild;

typedef struct packet_guild_name_t
{
	BYTE header;
	WORD size;
	BYTE subheader;
	DWORD	guildID;
	char	guildName[GUILD_NAME_MAX_LEN];
} TPacketGCGuildName;

typedef struct packet_guild_war
{
	DWORD	dwGuildSelf;
	DWORD	dwGuildOpp;
	BYTE	bType;
	BYTE 	bWarState;
} TPacketGCGuildWar;

typedef struct command_guild
{
	BYTE header;
	BYTE subheader;
} TPacketCGGuild;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN+1];
} TPacketCGAnswerMakeGuild;

typedef struct command_guild_use_skill
{
	DWORD	dwVnum;
	DWORD	dwPID;
} TPacketCGGuildUseSkill;

// Guild Mark
typedef struct command_mark_login
{
	BYTE    header;
	DWORD   handle;
	DWORD   random_key;
} TPacketCGMarkLogin;

typedef struct command_mark_upload
{
	BYTE	header;
	DWORD	gid;
	BYTE	image[16*12*4];
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
	BYTE	header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
	BYTE	header;
	BYTE	imgIdx;
	DWORD	crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
	BYTE    header;
	DWORD	bufSize;
	WORD	count;
} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
	BYTE	header;
	DWORD	bufSize;
	BYTE	imgIdx;
	DWORD	count;
} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE	header;
	WORD	size;
	DWORD	guild_id;
} TPacketCGGuildSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE header;
	DWORD guild_id;
	DWORD crc;
	DWORD size;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
	BYTE header;
	WORD size;
	DWORD guild_id;
} TPacketGCGuildSymbolData;

// Fishing

typedef struct command_fishing
{
	BYTE header;
	BYTE dir;
} TPacketCGFishing;

typedef struct packet_fishing
{
	BYTE header;
	BYTE subheader;
	DWORD info;
	BYTE dir;
} TPacketGCFishing;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
	FISHING_SUBHEADER_GC_FISH,
};

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t byItemCount;
#else
	BYTE byItemCount;
#endif
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
	BYTE	bHeader;
	char	szBuf[255 + 1];
} TPacketCGHack;

// SubHeader - Dungeon
enum
{
	DUNGEON_SUBHEADER_GC_TIME_ATTACK_START = 0,
	DUNGEON_SUBHEADER_GC_DESTINATION_POSITION = 1,
};

typedef struct packet_dungeon
{
	BYTE bHeader;
	WORD size;
	BYTE subheader;
} TPacketGCDungeon;

typedef struct packet_dungeon_dest_position
{
	int32_t x;
	int32_t y;
} TPacketGCDungeonDestPosition;

typedef struct SPacketGCShopSign
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;

typedef struct SPacketCGMyShop
{
	BYTE	bHeader;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t bCount;
#else
	BYTE bCount;
#endif
} TPacketCGMyShop;

typedef struct SPacketGCTime
{
	BYTE	bHeader;
	int32_t	time;
} TPacketGCTime;

enum
{
	WALKMODE_RUN,
	WALKMODE_WALK,
};

typedef struct SPacketGCWalkMode
{
	BYTE	header;
	DWORD	vid;
	BYTE	mode;
} TPacketGCWalkMode;

typedef struct SPacketGCChangeSkillGroup
{
	BYTE        header;
	BYTE        skill_group;
} TPacketGCChangeSkillGroup;

typedef struct SPacketCGRefine
{
	BYTE	header;
	BYTE	pos;
	BYTE	type;
} TPacketCGRefine;

typedef struct SPacketCGRequestRefineInfo
{
	BYTE	header;
	BYTE	pos;
} TPacketCGRequestRefineInfo;

typedef struct SPacketGCRefineInformaion
{
	BYTE	header;
	BYTE	type;
	BYTE	pos;
	DWORD	src_vnum;
	DWORD	result_vnum;
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t material_count;
#else
	BYTE material_count;
#endif
	int		cost;
	int		cost2;
	int		cost3;
	int		prob;
	uint8_t probExtra;
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
#if defined(__ITEM_APPLY_RANDOM__)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
} TPacketGCRefineInformation;

struct TNPCPosition
{
	BYTE bType;
	char name[CHARACTER_NAME_MAX_LEN+1];
	int32_t x;
	int32_t y;
};

typedef struct SPacketGCNPCPosition
{
	BYTE header;
	WORD size;
	WORD count;

	// array of TNPCPosition
} TPacketGCNPCPosition;

typedef struct SPacketGCSpecialEffect
{
	BYTE header;
	BYTE type;
	DWORD vid;
} TPacketGCSpecialEffect;

typedef struct SPacketCGChangeName
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketCGChangeName;

typedef struct SPacketGCChangeName
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCChangeName;

typedef struct command_client_version
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion;

typedef struct command_client_version2
{
	BYTE header;
	char filename[32+1];
	char timestamp[32+1];
} TPacketCGClientVersion2;

typedef struct packet_channel
{
	BYTE header;
	BYTE channel;
} TPacketGCChannel;

#if defined(__BL_KILL_BAR__)
typedef struct command_kill_bar
{
	BYTE	bHeader;
	BYTE	bKillerRace;
	BYTE	bKillerWeaponType;
	BYTE	bVictimRace;
	char	szKiller[CHARACTER_NAME_MAX_LEN + 1];
	char	szVictim[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCKillBar;
#endif

typedef struct SEquipmentItemSet
{
	DWORD   vnum;
	BYTE    count;
	int32_t    alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TEquipmentItemSet;

typedef struct pakcet_view_equip
{
	BYTE  header;
	DWORD vid;
	TEquipmentItemSet equips[WEAR_MAX_NUM];
} TPacketViewEquip;

typedef struct SLandPacketElement
{
	DWORD	dwID;
	int32_t	x, y;
	int32_t	width, height;
	DWORD	dwGuildID;
} TLandPacketElement;

typedef struct packet_land_list
{
	BYTE	header;
	WORD	size;
} TPacketGCLandList;

typedef struct SPacketGCTargetCreate
{
	BYTE	bHeader;
	int32_t	lID;
	char	szName[32+1];
	DWORD	dwVID;
	BYTE	bType;
} TPacketGCTargetCreate;

typedef struct SPacketGCTargetUpdate
{
	BYTE	bHeader;
	int32_t	lID;
	int32_t	lX, lY;
} TPacketGCTargetUpdate;

typedef struct SPacketGCTargetDelete
{
	BYTE	bHeader;
	int32_t	lID;
} TPacketGCTargetDelete;

typedef struct SPacketGCAffectAdd
{
	BYTE		bHeader;
	TPacketAffectElement elem;
} TPacketGCAffectAdd;

typedef struct SPacketGCAffectRemove
{
	BYTE	bHeader;
	DWORD	dwType;
	BYTE	bApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_lover_info
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	BYTE love_point;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE header;
	BYTE love_point;
} TPacketGCLovePointUpdate;

// MINING
typedef struct packet_dig_motion
{
	BYTE header;
	DWORD vid;
	DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;
// END_OF_MINING

// SCRIPT_SELECT_ITEM
typedef struct command_script_select_item
{
	BYTE header;
	DWORD selection;
} TPacketCGScriptSelectItem;
// END_OF_SCRIPT_SELECT_ITEM

typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int damage;
} TPacketGCDamageInfo;

typedef struct tag_GGSiege
{
	BYTE	bHeader;
	BYTE	bEmpire;
	BYTE	bTowerCount;
} TPacketGGSiege;

typedef struct SPacketGGMonarchTransfer
{
	BYTE	bHeader;
	DWORD	dwTargetPID;
	int32_t	x;
	int32_t	y;
} TPacketMonarchGGTransfer;

typedef struct SPacketGGCheckAwakeness
{
	BYTE bHeader;
} TPacketGGCheckAwakeness;

typedef struct SPacketGCPanamaPack
{
	BYTE	bHeader;
	char	szPackName[256];
	BYTE	abIV[32];
} TPacketGCPanamaPack;

typedef struct SPacketGCHybridCryptKeys
{
	SPacketGCHybridCryptKeys() : m_pStream(NULL) {}
	~SPacketGCHybridCryptKeys()
	{
		if( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + KeyStreamLen;
	}

	BYTE* GetStreamData()
	{
		if( m_pStream )
			delete[] m_pStream;

		uDynamicPacketSize = (WORD)GetStreamSize();

		m_pStream = new BYTE[ uDynamicPacketSize ];

		memcpy( m_pStream, &bHeader, 1 );
		memcpy( m_pStream+1, &uDynamicPacketSize, 2 );
		memcpy( m_pStream+3, &KeyStreamLen, 4 );

		if( KeyStreamLen > 0 )
			memcpy( m_pStream+7, pDataKeyStream, KeyStreamLen );

		return m_pStream;
	}

	BYTE	bHeader;
	WORD    uDynamicPacketSize;
	int		KeyStreamLen;
	BYTE*   pDataKeyStream;

private:
	BYTE* m_pStream;
} TPacketGCHybridCryptKeys;

typedef struct SPacketGCPackageSDB
{
	SPacketGCPackageSDB() : m_pDataSDBStream(NULL), m_pStream(NULL) {}
	~SPacketGCPackageSDB()
	{
		if( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + iStreamLen;
	}

	BYTE* GetStreamData()
	{
		if( m_pStream )
			delete[] m_pStream;

		uDynamicPacketSize =  GetStreamSize();

		m_pStream = new BYTE[ uDynamicPacketSize ];

		memcpy( m_pStream, &bHeader, 1 );
		memcpy( m_pStream+1, &uDynamicPacketSize, 2 );
		memcpy( m_pStream+3, &iStreamLen, 4 );

		if( iStreamLen > 0 )
			memcpy( m_pStream+7, m_pDataSDBStream, iStreamLen );

		return m_pStream;
	}

	BYTE	bHeader;
	WORD    uDynamicPacketSize;
	int		iStreamLen;
	BYTE*   m_pDataSDBStream;

private:
	BYTE* m_pStream;
} TPacketGCPackageSDB;

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
typedef struct SPacketGGRemoveOfflineShop
{
	BYTE	bHeader;
    int32_t    lMapIndex;
	char    szNpcName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGRemoveOfflineShop;

typedef struct SPacketGGOfflineShopBuy
{
	BYTE    bHeader;
	int64_t	dwMoney;
	int64_t	dwCheque;
	char    szBuyerName[CHARACTER_NAME_MAX_LEN + 1];        
} TPacketGGOfflineShopBuy;

typedef struct packet_offline_shop_start
{
	DWORD	owner_vid;
	DWORD	IsOnline;
	struct packet_shop_item items[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCOfflineShopStart;

typedef struct packet_offline_shop_money
{
	int64_t	dwMoney;
	int64_t	dwCheque;
	int64_t	zarobekYang;
	int64_t	zarobekCheque;
} TPacketGCOfflineShopMoney;

typedef struct TPacketCGMyOfflineShop
{
	BYTE	bHeader;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
	int		bCount;
} TPacketCGMyOfflineShop;

typedef struct packet_gold_new
{
	BYTE header;
	DWORD vid;
	int64_t gold;
} TPacketGold;
#endif

#ifdef _IMPROVED_PACKET_ENCRYPTION_
struct TPacketKeyAgreement
{
	static const int MAX_DATA_LEN = 256;
	BYTE bHeader;
	WORD wAgreedLength;
	WORD wDataLength;
	BYTE data[MAX_DATA_LEN];
};

struct TPacketKeyAgreementCompleted
{
	BYTE bHeader;
	BYTE data[3]; // dummy (not used)
};

#endif // _IMPROVED_PACKET_ENCRYPTION_

#define MAX_EFFECT_FILE_NAME 128
typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[MAX_EFFECT_FILE_NAME];
} TPacketGCSpecificEffect;

enum EDragonSoulRefineWindowRefineType
{
	DragonSoulRefineWindow_UPGRADE,
	DragonSoulRefineWindow_IMPROVEMENT,
	DragonSoulRefineWindow_REFINE,
};

enum EPacketCGDragonSoulSubHeaderType
{
	DS_SUB_HEADER_OPEN,
	DS_SUB_HEADER_CLOSE,
	DS_SUB_HEADER_DO_REFINE_GRADE,
	DS_SUB_HEADER_DO_REFINE_STEP,
	DS_SUB_HEADER_DO_REFINE_STRENGTH,
	DS_SUB_HEADER_REFINE_FAIL,
	DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE,
	DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL,
	DS_SUB_HEADER_REFINE_SUCCEED,
#if defined(__DS_CHANGE_ATTR__)
	DS_SUB_HEADER_OPEN_CHANGE_ATTR,
	DS_SUB_HEADER_DO_CHANGE_ATTR,
#endif
};
typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header (HEADER_CG_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DRAGON_SOUL_REFINE_GRID_SIZE];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;

typedef struct SPacketCGStateCheck
{
	BYTE header;
	uint32_t key;
	uint32_t index;
} TPacketCGStateCheck;

typedef struct SPacketGCStateCheck
{
	BYTE header;
	uint32_t key;
	uint32_t index;
	unsigned char state;
} TPacketGCStateCheck;

#ifdef __REMOVE_ITEM__
enum ERemoveItemConfig
{
	HEADER_GC_REMOVE_ITEM = 57,
	HEADER_CG_REMOVE_ITEM = 68,
	REMOVE_ITEM_SLOT_COUNT_X = 8,
	REMOVE_ITEM_SLOT_COUNT_Y = 6,
};

typedef struct SPacketGCRemoveItem
{
	BYTE	header;
} TPacketGCRemoveItem;

typedef struct SPacketCGRemoveItem
{
	BYTE	header;
	BYTE	item_count;
} TPacketCGRemoveItem;
#endif

#ifdef ENABLE_ODLAMKI_SYSTEM
enum
{
	HEADER_CG_ODLAMKI_ITEM = 224,
	HEADER_GC_ODLAMKI_ITEM = 225,
};
typedef struct SPacketGCOdlamki
{
	BYTE	header;
} TPacketGCOdlamki;

typedef struct SPacketCGOdlamki {
	BYTE	header;
	BYTE	item_count;
} TPacketCGOdlamki;
#endif
#ifdef ENABLE_VS_SHOP_SEARCH
typedef struct SPacketCGSearch {
	BYTE	header;
	BYTE	items;
} TPacketCGSearch;
#endif


#ifdef ENABLE_ENTITY_PRELOADING
/*** HEADER_GC_PRELOAD_ENTITIES ***/
typedef struct packet_preload_entities
{
	uint8_t header;
	uint16_t size;
	uint16_t count;
} TPacketGCPreloadEntities;
#endif

#ifdef ENABLE_NEW_PET_SYSTEM
typedef struct SPetAction
{
	BYTE	header;
	DWORD	skillvnum;
} TPacketPetAction;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
enum
{
	HEADER_CG_ACCE = 211,
	HEADER_GC_ACCE = 215,
	ACCE_SUBHEADER_GC_OPEN = 0,
	ACCE_SUBHEADER_GC_CLOSE,
	ACCE_SUBHEADER_GC_ADDED,
	ACCE_SUBHEADER_GC_REMOVED,
	ACCE_SUBHEADER_CG_REFINED,
	ACCE_SUBHEADER_CG_CLOSE = 0,
	ACCE_SUBHEADER_CG_ADD,
	ACCE_SUBHEADER_CG_REMOVE,
	ACCE_SUBHEADER_CG_REFINE,
};

typedef struct SPacketAcce
{
	BYTE	header;
	BYTE	subheader;
	bool	bWindow;
	int64_t	dwPrice;
	int64_t	dwCheque;
	BYTE	bChance;
	AcceMaterials	materials[2];
	BYTE	bPos;
	TItemPos	tPos;
	DWORD	dwItemVnum;
	DWORD	dwMinAbs;
	DWORD	dwMaxAbs;
} TPacketAcce;
#endif

#ifdef ENABLE_DROP_INFO
enum EDropInfoConfig
{
	HEADER_GC_DROP_INFO = 93,
	HEADER_CG_DROP_INFO = 99,
};

typedef struct SPacketDropInfoItem
{
	DWORD	dwVnum;
#ifdef __EXTENDED_ITEM_COUNT__
	uint16_t	byMinCount;
	uint16_t	byMaxCount;
#else
	BYTE	byMinCount;
	BYTE	byMaxCount;
#endif
#ifdef ENABLE_DROP_INFO_PCT
	float	pct;
#endif
} TPacketDropInfoItem;

typedef struct SPacketDropInfo
{
	BYTE	header;
	WORD	size;
	int		item_count;
	DWORD	gold_min;
	DWORD	gold_max;
	DWORD	mob_vnum;
} TPacketDropInfo;
#endif

#ifdef ENABLE_COLLECT_WINDOW
typedef struct SPacketGCCollectWindow
{
	BYTE	bHeader;
	BYTE	bWindowType;
	DWORD	dwTime;
	DWORD	bCount;
	DWORD	dwItemVnum;
	DWORD	bCountTotal;
	BYTE	bChance;
	DWORD	bRenderTargetID;
	DWORD	bQuestIndex;
	DWORD	RequiredLevel;
} TPacketGCCollectWindow;
#endif

#ifdef WEEKLY_RANK_BYLUZER
typedef struct packet_send_weekly_rank
{
	int		rank;
	char	name[CHARACTER_NAME_MAX_LEN+1];
	int		points;
	int		empire;
	int		job;
} TPacketGCTableWeeklyRank;

typedef struct packet_send_weekly_rank2
{
	BYTE 			bHeader;
	TPacketGCTableWeeklyRank rankingData[10];
} TPacketGCSendWeeklyRank;

typedef struct packet_send_weekly_page
{
	BYTE bHeader;
	BYTE page;
	int season;
	bool can_select;
} TPacketGCSelectWeeklyPage;

typedef struct packet_select_weekly_rank
{
	BYTE			bHeader;
	BYTE			page;
} TPacketCGSelectWeeklyRank;
#endif
#ifdef TITLE_SYSTEM_BYLUZER
typedef struct packet_title_active
{
	BYTE		bHeader;
	TPlayerTitle	titles[TITLE_MAX_NUM];
	BYTE		actually_active;
} TPacketGCTitleActive;

typedef struct packet_title_active_now
{
	BYTE		bHeader;
	BYTE		id;
} TPacketGCTitleActiveNow;

typedef struct packet_title_active_client
{
	BYTE		bHeader;
	int			id;
} TPacketCGTitleActive;
#endif

#ifdef ENABLE_SWITCHBOT
struct TPacketGGSwitchbot
{
	BYTE bHeader;
	WORD wPort;
	TSwitchbotTable table;

	TPacketGGSwitchbot() : bHeader(HEADER_GG_SWITCHBOT), wPort(0)
	{
		table = {};
	}
};

enum ECGSwitchbotSubheader
{
	SUBHEADER_CG_SWITCHBOT_START,
	SUBHEADER_CG_SWITCHBOT_STOP,
};

struct TPacketCGSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
	//DWORD switchingItemVnum;
};

enum EGCSwitchbotSubheader
{
	SUBHEADER_GC_SWITCHBOT_UPDATE,
	SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM,
	SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION,
};

struct TPacketGCSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

struct TSwitchbotUpdateItem
{
	BYTE	slot;
	BYTE	vnum;
	BYTE	count;
	int32_t	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};
#endif


#ifdef ENABLE_RESP_SYSTEM
enum ERespConfig
{
	HEADER_GC_RESP = 158,
	HEADER_CG_RESP = 87,

	RESP_CG_SUBHEADER_FETCH_RESP = 0,
	RESP_CG_SUBHEADER_FETCH_DROP,
	RESP_CG_SUBHEADER_TELEPORT,

	RESP_GC_SUBHEADER_DATA_RESP = 0,
	RESP_GC_SUBHEADER_DATA_DROP,
	RESP_GC_SUBHEADER_DATA_MOB,
	RESP_GC_SUBHEADER_REFRESH_RESP,
};

typedef struct SPacketGCRespHeader
{
	uint8_t	header;
	uint16_t	size;
	uint8_t	subheader;
} TPacketGCRespHeader;

typedef struct SPacketCGRespHeader
{
	uint8_t	header;
	uint8_t	subheader;
} TPacketCGRespHeader;

typedef struct SPacketGCRespData
{
	uint32_t id;
	uint32_t x;
	uint32_t y;
	uint32_t time;
	int64_t nextRespTime;
} TPacketGCRespData;

typedef struct SPacketGCRespGold
{
	uint32_t	minGold;
	uint32_t	maxGold;
} TPacketGCRespGold;

typedef struct SPacketGCRespItem
{
	uint32_t	vnum;
	uint8_t	minCount;
	uint8_t	maxCount;
} TPacketGCRespItem;

typedef struct SPacketGCMapData
{
	uint8_t	mobCount;
	uint32_t	currentMetinCount;
	uint32_t	maxMetinCount;
	uint32_t	currentBossCount;
	uint32_t	maxBossCount;
} TPacketGCMapData;

typedef struct SPacketGCRespRefresh
{
	uint32_t	id;
	uint32_t	vnum;
	int64_t	nextRespTime;
	uint32_t	x;
	uint32_t	y;
} TPacketGCRespRefresh;
#endif

typedef struct packet_anty_exp_info
{
	BYTE	bHeader;
	DWORD	status;
} TPacketGCAntyExp;

#ifdef ENABLE_SPLIT_INVENTORY_SYSTEM
typedef struct packet_wpadanie_info
{
	BYTE	bHeader;
	DWORD	status;
} TPacketGCWpadanie;
#endif

// #ifdef ENABLE_MOUNT_COSTUME_SYSTEM
// typedef struct SPacketCGMountNPCAction
// {
// 	BYTE	bHeader;
// 	BYTE	bAction;
// 	DWORD	dValue0;
// } TPacketCGMountNPCAction;

// typedef struct SPacketGCMountNPCAction
// {
// 	BYTE	bHeader;
// 	BYTE	bAction;
// 	DWORD	dValue0;
// } TPacketGCMountNPCAction;
// #endif

#ifdef ENABLE_ASLAN_BUFF_NPC_SYSTEM
typedef struct SPacketCGBuffNPCCreate
{
	BYTE bHeader;
	BYTE	bAction;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE	bSex;
}TPacketCGBuffNPCCreate;

typedef struct SPacketCGBuffNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
	DWORD	dValue1;
} TPacketCGBuffNPCAction;

typedef struct SPacketGCBuffNPCAction
{
	BYTE	bHeader;
	BYTE	bAction;
	DWORD	dValue0;
	DWORD	dValue1;
} TPacketGCBuffNPCAction;

typedef struct SPacketGCBuffNPCUseSkill
{
	BYTE	bHeader;
	DWORD	dSkillVnum;
	DWORD	dVid;
	DWORD	dSkillLevel;
}TPacketGCBuffNPCUseSkill;
#endif

#ifdef ENABLE_CHEQUE_SYSTEM
enum EWonExchangeCGSubHeader
{
	WON_EXCHANGE_CG_SUBHEADER_SELL,
	WON_EXCHANGE_CG_SUBHEADER_BUY
};

typedef struct SPacketCGWonExchange
{
	SPacketCGWonExchange(): bHeader(HEADER_CG_WON_EXCHANGE)
	{}

	BYTE bHeader;
	BYTE bSubHeader;
	WORD wValue;
} TPacketCGWonExchange;
#endif

#ifdef ENABLE_MINIMAP_DUNGEONINFO
enum
{
	MINIMAP_DUNGEONINFO_SUBHEADER_STATUS,
	MINIMAP_DUNGEONINFO_SUBHEADER_STAGE,
	MINIMAP_DUNGEONINFO_SUBHEADER_GAUGE,
	MINIMAP_DUNGEONINFO_SUBHEADER_NOTICE,
	MINIMAP_DUNGEONINFO_SUBHEADER_BUTTON,
	MINIMAP_DUNGEONINFO_SUBHEADER_TIMER,
};

typedef struct SPacketGCMiniMapDungeonInfo
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dValue1;
	DWORD	dValue2;
	DWORD	dValue3;
	DWORD	buttonvalue;
	DWORD	timerstatus;
	DWORD	time;
	char	szNotice[256];
}TPacketGCMiniMapDungeonInfo;
#endif

#ifdef ENABLE_EVENT_MANAGER
typedef struct SPacketGCEventManager
{
	BYTE	header;
	DWORD	size;
} TPacketGCEventManager;

typedef struct SPacketCGEventActivate
{
	BYTE	header;
	WORD	eventID;
} TPacketCGEventActivate;

enum EEventActivateResult
{
	EVENT_ACTIVATE_SUCCESS = 0,
	EVENT_ACTIVATE_ALREADY  = 1,
	EVENT_ACTIVATE_FAIL     = 2,
};

typedef struct SPacketGCEventActivateResult
{
	BYTE	header;
	WORD	eventID;
	BYTE	result;
} TPacketGCEventActivateResult;
#endif

#ifdef ENABLE_ITEMSHOP
typedef struct SPacketGCItemShop
{
	BYTE	header;
#ifdef ENABLE_LARGE_DYNAMIC_PACKETS
	int		size;
#else
	WORD	size;
#endif
} TPacketGCItemShop;
#endif

#ifdef TAKE_LEGEND_DAMAGE_BOARD_SYSTEM
typedef struct packet_send_legend_damage
{
	char		cName[CHARACTER_NAME_MAX_LEN + 1];
	int			iLevel;
	int			iRace;
	int			iEmpire;
	int64_t	iDamage;
} TPacketGCTableLegendDamage;

typedef struct packet_send_legend_damage2
{
	BYTE	bHeader;
	DWORD	dwBossVID;
	char	cTarget[CHARACTER_NAME_MAX_LEN + 1];
	TPacketGCTableLegendDamage legendDamageData[LEGEND_DAMAGE_BOARD_MAX_ENTRIES];
} TPacketGCSendLegendDamage;
#endif

#ifdef __PREMIUM_PRIVATE_SHOP__
enum EPrivateShopGCSubheader
{
	SUBHEADER_GC_PRIVATE_SHOP_ADD_ENTITY,
	SUBHEADER_GC_PRIVATE_SHOP_DEL_ENTITY,
	SUBHEADER_GC_PRIVATE_SHOP_TITLE,
	SUBHEADER_GC_PRIVATE_SHOP_LOAD,
	SUBHEADER_GC_PRIVATE_SHOP_SET_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_SET_SALE,
	SUBHEADER_GC_PRIVATE_SHOP_BALANCE_UPDATE,
	SUBHEADER_GC_PRIVATE_SHOP_OPEN_PANEL,
	SUBHEADER_GC_PRIVATE_SHOP_CLOSE_PANEL,
	SUBHEADER_GC_PRIVATE_SHOP_CLOSE,
	SUBHEADER_GC_PRIVATE_SHOP_START,
	SUBHEADER_GC_PRIVATE_SHOP_END,
	SUBHEADER_GC_PRIVATE_SHOP_REMOVE_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_REMOVE_MY_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_ADD_ITEM,
	SUBHEADER_GC_PRIVATE_SHOP_STATE_UPDATE,
	SUBHEADER_GC_PRIVATE_SHOP_WITHDRAW,
	SUBHEADER_GC_PRIVATE_SHOP_ITEM_PRICE_CHANGE,
	SUBHEADER_GC_PRIVATE_SHOP_ITEM_MOVE,
	SUBHEADER_GC_PRIVATE_SHOP_TITLE_CHANGE,
	SUBHEADER_GC_PRIVATE_SHOP_UNLOCKED_SLOTS_CHANGE,

	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_LOOK_MODE,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_OPEN_TRADE_MODE,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_RESULT,
	SUBHEADER_GC_PRIVATE_SHOP_SEARCH_UPDATE,

	SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_RESULT,
	SUBHEADER_GC_PRIVATE_SHOP_MARKET_ITEM_PRICE_RESULT,
};

enum EPrivateShopCGSubheader
{
	SUBHEADER_CG_PRIVATE_SHOP_BUILD,
	SUBHEADER_CG_PRIVATE_SHOP_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN,
	SUBHEADER_CG_PRIVATE_SHOP_PANEL_OPEN_CHECK,
	SUBHEADER_CG_PRIVATE_SHOP_PANEL_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_START,
	SUBHEADER_CG_PRIVATE_SHOP_END,
	SUBHEADER_CG_PRIVATE_SHOP_BUY,
	SUBHEADER_CG_PRIVATE_SHOP_WITHDRAW,
	SUBHEADER_CG_PRIVATE_SHOP_MODIFY,
	SUBHEADER_CG_PRIVATE_SHOP_STATE_UPDATE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_PRICE_CHANGE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_MOVE,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKIN,
	SUBHEADER_CG_PRIVATE_SHOP_ITEM_CHECKOUT,
	SUBHEADER_CG_PRIVATE_SHOP_TITLE_CHANGE,
	SUBHEADER_CG_PRIVATE_SHOP_WARP_REQUEST,
	SUBHEADER_CG_PRIVATE_SHOP_SLOT_UNLOCK_REQUEST,

	SUBHEADER_CG_PRIVATE_SHOP_SEARCH_CLOSE,
	SUBHEADER_CG_PRIVATE_SHOP_SEARCH,
	SUBHEADER_CG_PRIVATE_SHOP_SEARCH_BUY,

	SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_DATA_REQUEST,
	SUBHEADER_CG_PRIVATE_SHOP_MARKET_ITEM_PRICE_REQUEST,
};

typedef struct SPrivateShopItem
{
	TItemPos	TPos;
	TItemPrice	TPrice;
	WORD		wDisplayPos;
} TPrivateShopItem;

typedef struct SPrivateShopItemData
{
	DWORD					dwVnum;
	TItemPrice				TPrice;
	int64_t					tCheckin;
	DWORD					dwCount;
	WORD					wPos;
	int32_t					alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
    DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
    DWORD dwRefineElement;
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
   TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
} TPrivateShopItemData;

typedef struct SPrivateShopSaleData
{
	char					szCustomer[CHARACTER_NAME_MAX_LEN + 1];
	int64_t					tTime;
	TPrivateShopItemData	TItem;
} TPrivateShopSaleData;

typedef struct SPrivateShopSearchData
{
	DWORD					dwShopID;
	char					szOwnerName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD					dwVnum;
	TItemPrice				TPrice;
	DWORD					dwCount;
	WORD					wPos;
	int32_t					alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	int64_t					tCheckin;
	BYTE					bState;
	int32_t					lMapIndex;
#ifdef ENABLE_PRIVATE_SHOP_CHANGE_LOOK
    DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_PRIVATE_SHOP_REFINE_ELEMENT
    DWORD dwRefineElement;
#endif
#ifdef ENABLE_PRIVATE_SHOP_APPLY_RANDOM
    TPlayerItemAttribute aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
} TPrivateShopSearchData;

typedef struct SPacketCGPrivateShop
{
	BYTE	bHeader;
	BYTE	bSubHeader;
} TPacketCGPrivateShop;

typedef struct SPacketGCPrivateShopAddEntity
{
	int32_t		lX;
	int32_t		lY;
	int32_t		lZ;
	DWORD		dwVID;
	DWORD		dwVnum;
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE		bTitleType;
	char		szTitle[TITLE_MAX_LEN + 1];
} TPacketGCPrivateShopAddEntity;

typedef struct SPacketGCPrivateShopDelEntity
{
	DWORD		dwVID;
} TPacketGCPrivateShopDelEntity;

typedef struct SPacketGCPrivateShopTitle
{
	DWORD		dwVID;
	char		szTitle[TITLE_MAX_LEN + 1];
	BYTE		bTitleType;
} TPacketGCPrivateShopTitle;

typedef struct SPacketCGPrivateShopBuild
{
	char	szTitle[TITLE_MAX_LEN + 1];
	DWORD	dwPolyVnum;
	BYTE	bTitleType;
	BYTE	bPageCount;
	WORD	wItemCount;
} TPacketCGPrivateShopBuild;

typedef struct SPacketCGPrivateShopItemPriceChange
{
	WORD		wPos;
	TItemPrice	TPrice;
} TPacketCGPrivateShopItemPriceChange;

typedef struct SPacketCGPrivateShopItemMove
{
	WORD		wPos;
	WORD		wChangePos;
} TPacketCGPrivateShopItemMove;

typedef struct SPrivateShopSearchFilter
{
	DWORD	dwVnum;
	char	szOwnerName[CHARACTER_NAME_MAX_LEN + 1];

	int		iItemType;
	int		iItemSubType;

	int		iJob;
	int		iGender;

	WORD	wMinStack;
	WORD	wMaxStack;

	BYTE	bMinRefine;
	BYTE	bMaxRefine;

	DWORD	dwMinLevel;
	DWORD	dwMaxLevel;

	TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	WORD					wSashAbsorption;
	BYTE					bAlchemyLevel;
	BYTE					bAlchemyClarity;
	int						iItemValue0;
	bool					bSearchMode;
} TPrivateShopSearchFilter;

typedef struct SPacketCGPrivateShopSearch
{
	TPrivateShopSearchFilter	Filter;
	bool						bUseFilter;
} TPacketCGPrivateShopSearch;

typedef struct SPrivateShopSearchSelectedItem
{
	DWORD		dwShopID;
	WORD		wPos;
	TItemPrice	TPrice;
} SPrivateShopSearchSelectedItem;

typedef struct SPacketCGPrivateShopSearchBuy
{
	SPrivateShopSearchSelectedItem aSelectedItems[SELECTED_ITEM_MAX_NUM];
} TPacketCGPrivateShopSearchBuy;

typedef struct SPacketGCPrivateShopSearchUpdate
{
	DWORD	dwShopID;
	int		iSpecificItemPos;
	BYTE	bState;
} TPacketGCPrivateShopSearchUpdate;

typedef struct SPacketGCPrivateShop
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bSubHeader;
} TPacketGCPrivateShop;

typedef struct SPacketGCPrivateShopLoad
{
	char		szTitle[SHOP_SIGN_MAX_LEN + 1];
	int64_t	llGold;
	DWORD		dwCheque;
	int32_t		lX;
	int32_t		lY;
	BYTE		bChannel;
	BYTE		bState;
	BYTE		bPageCount;
} TPacketGCPrivateShopLoad;

typedef struct SPacketGCPrivateShopOpen
{
	char					szTitle[SHOP_SIGN_MAX_LEN + 1];
	BYTE					bState;
	BYTE					bPageCount;
	WORD					wUnlockedSlots;
	DWORD					dwVID;
	TPrivateShopItemData	aItems[PRIVATE_SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCPrivateShopOpen;

typedef struct SPacketGCPrivateStateUpdate
{
	BYTE	bState;
	bool	bIsMainPlayerPrivateShop;
} TPacketGCPrivateStateUpdate;

typedef struct SPacketGCPrivateShopItemPriceChange
{
	WORD		wPos;
	TItemPrice	TPrice;
} TPacketGCPrivateShopItemPriceChange;

typedef struct SPacketGCPrivateShopItemMove
{
	WORD		wPos;
	WORD		wChangePos;
} TPacketGCPrivateShopItemMove;

typedef struct SPacketGCPrivateShopBalanceUpdate
{
	TItemPrice	TPrice;
} TPacketGCPrivateShopBalanceUpdate;

typedef struct SPacketCGPrivateShopItemCheckin
{
	TItemPos	TSrcPos;
	int64_t	llGold;
	DWORD		dwCheque;
	int			iDstPos;
} TPacketCGPrivateShopItemCheckin;

typedef struct SPacketCGPrivateShopItemCheckout
{
	WORD		wSrcPos;
	int			iDstPos;
} TPacketCGPrivateShopItemCheckout;

typedef struct SPacketGGPrivateShopItemRemove
{
	BYTE					bHeader;
	DWORD					dwShopID;
	WORD					wPos;
} TPacketGGPrivateShopItemRemove;

typedef struct SPacketGGPrivateShopItemSearch
{
	BYTE						bHeader;
	DWORD						dwCustomerID;
	DWORD						dwCustomerPort;
	bool						bUseFilter;
	TPrivateShopSearchFilter	Filter;
} TPacketGGPrivateShopItemSearch;

typedef struct SPacketGGPrivateShopItemSearchUpdate
{
	BYTE					bHeader;
	DWORD					dwShopID;
	int						iSpecificItemPos;
	BYTE					bState;
} TPacketGGPrivateShopItemSearchUpdate;

typedef struct SPacketGGPrivateShopItemSearchResult
{
	BYTE					bHeader;
	WORD					wSize;
	DWORD					dwCustomerID;
} TPacketGGPrivateShopItemSearchResult;
#endif

enum eReloadTypes : uint8_t { RELOAD_DROPS, RELOAD_VERSION };

typedef struct SPacketGGReload {
    uint8_t header;
    uint8_t type;
} TPacketGGReload;

#ifdef __ENABLE_POLYMORPH_SYSTEM__
enum EPolySystem : BYTE
{
	SUBHEADER_POLY_OPEN = 0,
	SUBHEADER_POLY_CLOSE = 1,
	SUBHEADER_POLY_BUY = 2,
	SUBHEADER_POLY_SKIN_UNLOCK = 3,
	SUBHEADER_POLY_SKIN_CHANGE = 4,
	SUBHEADER_POLY_DATA = 5,
};

typedef struct SPacketGCPolySystem
{
	BYTE byHeader;
	WORD wSize;
	WORD bySubheader;
} TPacketGCPolySystem;

typedef struct SPacketCGPolySystem
{
	BYTE byHeader;
	BYTE bySubheader;
} TPacketCGPolySystem;
#endif

#ifdef __INVENTORY_BUFFERING__
typedef struct SPacketGCInventoryHeader
{
	BYTE bHeader;
	WORD wSize;
} TPacketGCInventoryHeader;
#endif


#ifdef ENABLE_OFFLINE_SHOP
typedef struct SPacketGCNewOfflineshop
{
	BYTE bHeader;
#ifdef ENABLE_LARGE_DYNAMIC_PACKET
	int wSize;
#else
	WORD wSize;
#endif
	BYTE bSubHeader;
} TPacketGCNewOfflineshop;

typedef struct SPacketCGNewOfflineShop
{
	BYTE bHeader;
	uint16_t wSize;
	BYTE bSubHeader;
} TPacketCGNewOfflineShop;

namespace offlineshop
{
	typedef struct SFilterInfo
	{
		BYTE bType;
		BYTE bSubType;
		char szName[OFFLINE_SHOP_ITEM_MAX_LEN];
		TPriceInfo priceStart, priceEnd;
		BYTE iLevelStart, iLevelEnd;
		DWORD dwWearFlag;
		bool FindCharacter;//Finding by character name
		MAX_COUNT minCount, maxCount;
		BYTE minAbs, maxAbs;
		BYTE minAvg;
		BYTE alchemyGrade;
		BYTE alchemyPurity;
		BYTE sashLevel;
	} TFilterInfo;

	typedef struct SShopItemInfo
	{
		TItemPos pos;
		TPriceInfo price;
#ifdef ENABLE_OFFLINE_SHOP_GRID
		TItemDisplayPos display_pos;
#endif
	} TShopItemInfo;

	enum eSubHeaderGC
	{
		SUBHEADER_GC_SHOP_LIST,
		SUBHEADER_GC_SHOP_OPEN,
		SUBHEADER_GC_SHOP_OPEN_OWNER,
		SUBHEADER_GC_SHOP_OPEN_OWNER_NO_SHOP,
		SUBHEADER_GC_SHOP_CLOSE,
		SUBHEADER_GC_SHOP_BUY_ITEM_FROM_SEARCH,
		SUBHEADER_GC_SHOP_FILTER_RESULT,
		SUBHEADER_GC_SHOP_SAFEBOX_REFRESH,
#ifdef ENABLE_SHOPS_IN_CITIES
		SUBHEADER_GC_INSERT_SHOP_ENTITY,
		SUBHEADER_GC_REMOVE_SHOP_ENTITY,
#endif
#ifdef ENABLE_OFFLINESHOP_NOTIFICATION
		SUBHEADER_GC_NOTIFICATION,
#endif
#ifdef ENABLE_AVERAGE_PRICE
		SUBHEADER_GC_AVERAGE_PRICE,
#endif
	};

	typedef struct SSubPacketGCShopList
	{
		DWORD dwShopCount;
	} TSubPacketGCShopList;

	typedef struct SSubPacketGCShopOpen
	{
		TShopInfo shop;
	} TSubPacketGCShopOpen;

	typedef struct SSubPacketGCShopOpenOwner
	{
		TShopInfo shop;
	} TSubPacketGCShopOpenOwner;

	typedef struct SSubPacketGCShopFilterResult
	{
		DWORD dwCount;
	} TSubPacketGCShopFilterResult;

	typedef struct SSubPacketGCShopSafeboxRefresh
	{
		uint64_t valute;
		DWORD dwItemCount;
	} TSubPacketGCShopSafeboxRefresh;

	typedef struct SSubPacketGCShopBuyItemFromSearch
	{
		DWORD dwOwnerID;
		DWORD dwItemID;
	} TSubPacketGCShopBuyItemFromSearch;

#ifdef ENABLE_SHOPS_IN_CITIES
	typedef struct SSubPacketGCInsertShopEntity
	{
		DWORD dwVID;
		char szName[OFFLINE_SHOP_NAME_MAX_LEN];
		int iType;
		int32_t x, y, z;
#ifdef ENABLE_SHOP_DECORATION
		DWORD	dwShopDecoration;
#endif
	} TSubPacketGCInsertShopEntity;

	typedef struct SSubPacketGCRemoveShopEntity
	{
		DWORD dwVID;
	} TSubPacketGCRemoveShopEntity;
#endif

#ifdef ENABLE_OFFLINESHOP_NOTIFICATION
	typedef struct NotifTable 
	{
		DWORD dwItemID;
		int64_t dwItemPrice;
#ifdef ENABLE_EXTENDED_COUNT_ITEMS
		MAX_COUNT dwItemCount;
#else
		WORD dwItemCount;
#endif
	} TSubPacketGCNotification;
#endif

#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
	typedef struct sExtendTime
	{
		DWORD dwTime;
	}TSubPacketCGShopExtendTime;
#endif


#ifdef ENABLE_AVERAGE_PRICE
	typedef struct SSubPacketAveragePrice
	{
		SSubPacketAveragePrice(DWORD vnm, int64_t prc)
		{
			vnum = vnm;
			price = prc;
		}
		DWORD vnum;
		int64_t price;
	}TSubPacketAveragePrice;
#endif

	enum eSubHeaderCG
	{
		SUBHEADER_CG_SHOP_CREATE_NEW,
		SUBHEADER_CG_SHOP_CHANGE_NAME,
		SUBHEADER_CG_SHOP_FORCE_CLOSE,
		SUBHEADER_CG_SHOP_REQUEST_SHOPLIST,
		SUBHEADER_CG_SHOP_OPEN,
		SUBHEADER_CG_SHOP_OPEN_OWNER,
		SUBHEADER_CG_SHOP_BUY_ITEM,
		SUBHEADER_CG_SHOP_ADD_ITEM,
		SUBHEADER_CG_SHOP_REMOVE_ITEM,
		SUBHEADER_CG_SHOP_EDIT_ITEM,
		SUBHEADER_CG_SHOP_FILTER_REQUEST,
		SUBHEADER_CG_SHOP_SAFEBOX_OPEN,
		SUBHEADER_CG_SHOP_SAFEBOX_GET_ITEM,
		SUBHEADER_CG_SHOP_SAFEBOX_GET_VALUTES,
		SUBHEADER_CG_SHOP_SAFEBOX_CLOSE,
		SUBHEADER_CG_CLOSE_BOARD,
#ifdef ENABLE_SHOPS_IN_CITIES
		SUBHEADER_CG_CLICK_ENTITY,
#endif
#ifdef ENABLE_OFFLINESHOP_EXTEND_TIME
		SUBHEADER_CG_SHOP_EXTEND_TIME,
#endif
#ifdef ENABLE_AVERAGE_PRICE
		SUBHEADER_CG_AVERAGE_PRICE,
#endif
	};

	typedef struct SSubPacketCGShopCreate
	{
		TShopInfo shop;
	} TSubPacketCGShopCreate;

	typedef struct SSubPacketCGShopChangeName
	{
		char szName[OFFLINE_SHOP_NAME_MAX_LEN];
	} TSubPacketCGShopChangeName;

	typedef struct SSubPacketCGShopOpen
	{
		DWORD dwOwnerID;
	} TSubPacketCGShopOpen;

	typedef struct SSubPacketCGAddItem
	{
		TItemPos pos;
		TPriceInfo price;
#ifdef ENABLE_OFFLINE_SHOP_GRID
		TItemDisplayPos display_pos;
#endif
	} TSubPacketCGAddItem;

	typedef struct SSubPacketCGRemoveItem
	{
		DWORD dwItemID;
	} TSubPacketCGRemoveItem;

	typedef struct SSubPacketCGEditItem
	{
		DWORD dwItemID;
		TPriceInfo price;
		bool allEdit;
	} TSubPacketCGEditItem;

	typedef struct SSubPacketCGFilterRequest
	{
		TFilterInfo filter;
	} TSubPacketCGFilterRequest;

	typedef struct SSubPacketCGShopSafeboxGetItem
	{
		DWORD dwItemID;
	} TSubPacketCGShopSafeboxGetItem;

	typedef struct SSubPacketCGShopSafeboxGetValutes
	{
		uint64_t gold;
	}TSubPacketCGShopSafeboxGetValutes;

	typedef struct SSubPacketCGShopBuyItem
	{
		DWORD dwOwnerID;
		DWORD dwItemID;
		bool bIsSearch;
	} TSubPacketCGShopBuyItem;

#ifdef ENABLE_SHOPS_IN_CITIES
	typedef struct SSubPacketCGShopClickEntity
	{
		DWORD dwShopVID;
	} TSubPacketCGShopClickEntity;
#endif
}
#endif

#ifdef ENABLE_SAVE_LOCATION_SYSTEM
enum ESaveLocationInfo
{
	SAVE_LOCATION_MAX_NUM = 20,
	SAVE_LOCATION_MAX_NUM_NORMAL = 10,
};

enum
{
	SAVELOCATION_SUBHEADER_CG_ADD = 0,
	SAVELOCATION_SUBHEADER_CG_DEL,
	SAVELOCATION_SUBHEADER_CG_WARP,
	SAVELOCATION_SUBHEADER_CG_RENAME,

	SAVELOCATION_SUBHEADER_GC_ADDED = 0,
	SAVELOCATION_SUBHEADER_GC_DELED,
	SAVELOCATION_SUBHEADER_GC_UPDATE,
};

typedef struct SPacketSaveLocation
{
	BYTE	header;
	BYTE	subheader;
	BYTE	pos;
	TSaveLocation	location[SAVE_LOCATION_MAX_NUM];
} TPacketSaveLocation;
#endif

using TPacketHeader = uint8_t;
using TDefaultPacket = struct SDefaultPacket
{
	public:
	    SDefaultPacket() = default;
	    ~SDefaultPacket() = default;
	    SDefaultPacket(const SDefaultPacket& rhs) = default;
	    SDefaultPacket(SDefaultPacket&& rhs) noexcept = default;
	    SDefaultPacket& operator=(const SDefaultPacket& rhs) = default;
	    SDefaultPacket& operator=(SDefaultPacket&& rhs) noexcept = default;

	public:
		/*const*/ uint8_t  GetHeader() const noexcept { return header; }
		/*const*/ uint8_t  GetSubheader() const noexcept { return subheader; }
		constexpr static /*const*/ uint16_t GetFixedSize() noexcept { return sizeof(SDefaultPacket); }
		/*const*/ uint16_t GetSize() const  noexcept { return size; }

	public:
		void SetPacket(uint8_t header, uint8_t subheader) noexcept
		{
			this->header = header;
			this->subheader = subheader;
		}

		void SetHeader(uint8_t header) noexcept
		{
			this->header = header;
		}

		void SetSubheader(uint8_t subheader) noexcept
		{
			this->subheader = subheader;
		}

		void IncreaseSize(uint16_t size)
		{
			if (this->size + size > UINT16_MAX)
				return;

			this->size += size;
		}

		inline SDefaultPacket& operator += (uint16_t size)
		{
			IncreaseSize(size);
			return *this;
		}

	protected:
		TPacketHeader	header{ 0 };
		uint16_t		size{ 0 };
		uint8_t			subheader{ 0 };
};

namespace packets
{
	struct base_packet {
		uint8_t header;
	};

	struct base_packet_dynamic : public base_packet {
		uint32_t size;
	};

	struct base_dynamic_packet_with_subheader_t
		: public base_packet_dynamic {
		uint8_t subheader;
	};

	struct base_packet_static_subheader_t
		: public base_packet {
		uint8_t subheader;
	};

	struct serialized_packet  : public base_packet_dynamic
	{
		uint32_t vnum;
		std::vector<uint32_t> elements;
	};

	namespace GGPackets
	{

	};
}

#ifdef RANKING_SYSTEM
enum ServerRankingTypes
{
	SERVER_RANK_TYPE_STONES,
	SERVER_RANK_TYPE_BOSSES,
	SERVER_RANK_TYPE_DUNGEONS,
	SERVER_RANK_TYPE_MONSTERS,
	SERVER_RANK_TYPE_ALCHEMY,
	SERVER_RANK_TYPE_REFINE,
	SERVER_RANK_TYPE_DRAGON_COINS,
	SERVER_RANK_TYPE_MAX_NUM,
};

typedef struct SPacketCGLoadServerRanking
{
	uint8_t		header;
	uint8_t		catIdx;
} TPacketCGLoadServerRanking;

typedef struct SPacketGCServerRankingLoad
{
	uint8_t		header;
	uint16_t	size;
} TPacketGCServerRankingLoad;

enum RankingTypes
{
	RANK_TYPE_STONES,
	RANK_TYPE_BOSSES,
	RANK_TYPE_DUNGEONS,
	RANK_TYPE_MONSTERS,
	RANK_TYPE_ALCHEMY,
	RANK_TYPE_REFINE,
	RANK_TYPE_DRAGON_COINS,
	RANK_TYPE_MAX_NUM,
};

typedef struct SPacketCGLoadRanking
{
	uint8_t		header;
	uint8_t		catIdx;
} TPacketCGLoadRanking;

typedef struct SPacketGCRankingLoad
{
	uint8_t		header;
	uint16_t	size;
	uint32_t	nextRefreshTime;
	uint32_t	sundayEndTime;
} TPacketGCRankingLoad;
#endif

#ifdef ENABLE_SECONDARY_LEVEL
enum ESecondaryLevelSubheader
{
	SUBHEADER_SECONDARY_LEVEL_GC_SEND_INFO,
	SUBHEADER_SECONDARY_LEVEL_GC_MAX, 

	SUBHEADER_SECONDARY_LEVEL_CG_DO_UPGRADE = 0,
	SUBHEADER_SECONDARY_LEVEL_CG_MAX,
};

typedef struct SPacketGCSecondaryLevel
{
	BYTE header;
	int	 size;
	BYTE subheader;

	SPacketGCSecondaryLevel() :
		header(HEADER_GC_SECONDARY_LEVEL),
		subheader(SUBHEADER_SECONDARY_LEVEL_GC_MAX)
	{
		size = sizeof(SPacketGCSecondaryLevel);
	}
} TPacketGCSecondaryLevel;

typedef struct SPacketCGSecondaryLevel
{
	BYTE bHeader;
	BYTE bSubHeader;
	int	 iSize;

	SPacketCGSecondaryLevel() :
		bHeader(HEADER_CG_SECONDARY_LEVEL),
		bSubHeader(SUBHEADER_SECONDARY_LEVEL_CG_MAX)
	{
		iSize = sizeof(SPacketCGSecondaryLevel);
	}
} TPacketCGSecondaryLevel;

typedef struct SPacketGCSecondaryLevelInfo
{
	BYTE	bLevel;
	DWORD	dwApplyType[SL_MAX_BONUSES];
	int32_t	lApplyValue[SL_MAX_BONUSES];
	DWORD	dwRequiredItemVnum[SL_MAX_ITEMS];
	int32_t	lRequiredItemCount[SL_MAX_ITEMS];
	int64_t	llRequiredGold[SL_MAX_UPGRADE_OPTIONS];
#ifdef ENABLE_CHEQUE_SYSTEM
	int32_t	dwRequiredCheque[SL_MAX_UPGRADE_OPTIONS];
#endif
	int32_t	dwRequiredAP[SL_MAX_UPGRADE_OPTIONS];
	DWORD	dwChance[SL_MAX_UPGRADE_OPTIONS];
} TPacketGCSecondaryLevelInfo;

typedef struct SPacketCGSecondaryLevelUpgrade
{
	BYTE	bOption;
} TPacketCGSecondaryLevelUpgrade;
#endif

#ifdef ENABLE_ENLIGHTENMENT_SYSTEM
typedef struct SPacketGGCmdchat
{
	BYTE	bHeader;
	int32_t	lSize;
} TPacketGGCmdchat;
#endif

#ifdef __ENABLE_MINING_WITH_SPACE__
enum EMiningSubheaders
{
	MINING_SUBHEADER_CANCEL,
	MINING_SUBHEADER_NEAREST,
};

typedef struct SPacketCGMining
{
	BYTE header;
	BYTE subheader;
} TPacketCGMining;
#endif

#ifdef ENABLE_MOUNT_SYSTEM
typedef struct SPacketMountInfo
{
	BYTE	bHeader;
	DWORD	dwVnum;
	char	szName[64];
	WORD	wLevel;
	int		iExp;
	int		iRequiredExp;
	bool	bSummoned;
} TPacketGCMountInfo;
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
enum
{
	MULTI_FARM_SET,
	MULTI_FARM_REMOVE,
};
typedef struct SPacketGGMultiFarm
{
	BYTE	header;
	DWORD	size;
	BYTE	subHeader;
	char	playerHWID[COMPOSITE_HWID_MAX_LEN + 1];
	DWORD	playerID;
	char	playerName[CHARACTER_NAME_MAX_LEN+1];
	bool	farmStatus;
	BYTE	affectType;
	int		affectTime;
} TPacketGGMultiFarm;
#endif

#ifdef ENABLE_IN_GAME_LOG_SYSTEM
namespace InGameLog
{
	enum InGameLogGCSubHeader
	{
		SUBHEADER_GC_OFFLINESHOP_LOG_OPEN,
		SUBHEADER_GC_OFFLINESHOP_LOG_ADD,
		SUBHEADER_GC_TRADE_LOG_OPEN,
		SUBHEADER_GC_TRADE_LOG_DETAILS_OPEN
	};

	enum InGameLogCGSubHeader
	{
		SUBHEADER_CG_OFFLINESHOP_LOG_OPEN,
		SUBHEADER_CG_TRADE_LOG_OPEN,
		SUBHEADER_CG_TRADE_LOG_DETAILS_OPEN,
	};

	typedef struct SPacketGCInGameLog
	{
		BYTE header;
		WORD size;
		BYTE subHeader;
	}TPacketGCInGameLog;

	typedef struct SPacketCGInGameLog
	{
		BYTE header;
		BYTE subHeader;
		DWORD logID;
	}TPacketCGInGameLog;
}
#endif

#ifdef ENABLE_PVP_RANKING
typedef struct SPacketGCPvPRanking {
	BYTE	bHeader;
	char	szNames[CHARACTER_NAME_MAX_LEN + 1][10];
	BYTE	bJobs[10];
	BYTE	bEmpire[10];
	BYTE	bLevels[10];
	int		iKill[10];
	int		iDead[10];
	BYTE	bStatus[10];
	int		iMaxKill;
#ifdef ENABLE_PVP_RANKING_WITH_EMPIRE
	BYTE	bIsRankEmpire;
	long long	llEmpire1;
	long long	llEmpire2;
	long long	llEmpire3;
#endif
}TPacketGCPvPRanking;
#endif

#ifdef ENABLE_TITLE_ACHIEVEMENT_SYSTEM
typedef struct SPacketGCTitleAchievementInfo
{
	SPacketGCTitleAchievementInfo() : bHeader(HEADER_GC_TITLE_ACHIEVEMENT) {}
	BYTE bHeader;
	BYTE bTitleUnlocked[24];
	BYTE bCurrentTitle;
	BYTE bCurrentTitlePremium;
	WORD wAffType[3];
	WORD bAffValue[3];
	WORD wAffTypePremium[3];
	WORD bAffValuePremium[3];
} TPacketGCTitleAchievementInfo;
#endif

#ifdef ENABLE_NOTIFICATION_SYSTEM
enum ENotification
{
	NOTIFICATION_CATEGORY_SPECIAL_ITEM_EXPIRED = 1,
	NOTIFICATION_CATEGORY_BIOLOG_MISSION_READY = 2,
	NOTIFICATION_CATEGORY_ALCHEMY_EXPIRED = 3,
	NOTIFICATION_CATEGORY_MELT_EXPIRED = 4,
};

typedef struct SPacketGCNotification
{
	BYTE byHeader;
	BYTE byCategory;
	DWORD dwValue;
	DWORD dwAdditionalValue;
} TPacketGCNotification;
#endif

#ifdef _ENABLE_BATTLEPASS_
enum { BATTLEPASS_GC_SUB_INIT, BATTLEPASS_GC_SUB_UPDATE };

typedef struct SPacketBattlePass
{
	BYTE    bHeader;
	WORD    wSize;
	BYTE    bSubHeader;
	
	// Normal battle pass
	bool isPremium;
	int points;
	
	// Event battle pass
	bool eventIsPremium;
	int eventPoints;
	bool eventActive;
} TPacketBattlePass;

typedef struct SPacketSubBattlePassUpdate
{
	uint8_t type;
	uint8_t missionVnum;
	uint32_t progress;
	uint32_t endTime;
	uint8_t battlePassType;
} TPacketSubBattlePassUpdate;
#endif

struct DynamicPacketInfo
{
    uint8_t header;
    uint8_t sub_header;
};
#pragma pack()
#endif
