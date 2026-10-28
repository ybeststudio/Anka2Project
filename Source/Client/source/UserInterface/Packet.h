#pragma once

#include "StdAfx.h"

#include "../gamelib/RaceData.h"
#include "../gamelib/ItemData.h"
#include "GameType.h"

typedef BYTE TPacketHeader;

enum PacketHeaderCG
{
	HEADER_CG_LOGIN								= 1, // unused - to be removed
	HEADER_CG_ATTACK							= 2,
	HEADER_CG_CHAT								= 3,
	HEADER_CG_PLAYER_CREATE						= 4,
	HEADER_CG_PLAYER_DESTROY					= 5,
	HEADER_CG_PLAYER_SELECT						= 6,
	HEADER_CG_CHARACTER_MOVE					= 7,
	HEADER_CG_SYNC_POSITION						= 8,
	HEADER_CG_DIRECT_ENTER						= 9,
	HEADER_CG_ENTERGAME							= 10,
	HEADER_CG_ITEM_USE							= 11,
	HEADER_CG_ITEM_DROP							= 12,
	HEADER_CG_ITEM_MOVE							= 13,
	//HEADER_CG_EMPTY							= 14,
	HEADER_CG_ITEM_PICKUP						= 15,
	HEADER_CG_QUICKSLOT_ADD						= 16,
	HEADER_CG_QUICKSLOT_DEL						= 17,
	HEADER_CG_QUICKSLOT_SWAP					= 18,
	HEADER_CG_WHISPER							= 19,
	HEADER_CG_ITEM_DROP2						= 20,
	HEADER_CG_STATE_CHECKER						= 21,
#ifdef ENABLE_QUICK_SELL_ITEM
	HEADER_CG_ITEM_SELL							= 22,
#endif
#ifdef ENABLE_DESTROY_DIALOG
	HEADER_CG_ITEM_DESTROY						= 23,
#endif
	//HEADER_CG_EMPTY							= 24,
	//HEADER_CG_EMPTY							= 25,

	HEADER_CG_ON_CLICK							= 26,
	HEADER_CG_EXCHANGE							= 27,
	HEADER_CG_CHARACTER_POSITION				= 28,
	HEADER_CG_SCRIPT_ANSWER						= 29,
	HEADER_CG_QUEST_INPUT_STRING				= 30,
	HEADER_CG_QUEST_CONFIRM						= 31,
	//HEADER_CG_EMPTY							= 32,
#ifdef ENABLE_RENEWAL_OX_EVENT
	HEADER_CG_QUEST_INPUT_LONG_STRING			= 33,
#endif
#ifdef ENABLE_RESP_SYSTEM
	HEADER_CG_RESP 								= 34,
#endif
#ifdef ENABLE_ATTENDANCE_EVENT
	HEADER_CG_ATTENDANCE_REWARD 				= 35,
#endif
#ifdef ENABLE_FISH_EVENT_SYSTEM
	HEADER_CG_FISH_EVENT_SEND 					= 36,
#endif
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	HEADER_CG_SOUL_ROULETTE						= 37,
#endif
#ifdef ENABLE_MINI_GAME_CATCH_KING
	HEADER_CG_MINI_GAME_CATCH_KING 				= 38,
#endif
	//HEADER_CG_EMPTY							= 39,
	//HEADER_CG_EMPTY							= 40,
	HEADER_CG_PVP								= 41,
	//HEADER_CG_EMPTY							= 42,
	//HEADER_CG_EMPTY							= 43,
	//HEADER_CG_EMPTY							= 44,
	//HEADER_CG_EMPTY							= 45,
	//HEADER_CG_EMPTY							= 46,
	//HEADER_CG_EMPTY							= 47,
	//HEADER_CG_EMPTY							= 48,
	//HEADER_CG_EMPTY							= 49,
	HEADER_CG_SHOP								= 50,
	HEADER_CG_FLY_TARGETING						= 51,
	HEADER_CG_USE_SKILL							= 52,
	HEADER_CG_ADD_FLY_TARGETING					= 53,
	HEADER_CG_SHOOT								= 54,
	HEADER_CG_MYSHOP							= 55,
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	HEADER_CG_SKILL_COLOR						= 56,
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	HEADER_CG_OFFLINE_SHOP						= 57,
	HEADER_CG_MY_OFFLINE_SHOP					= 58,
#endif
#ifdef ENABLE_MOB_DROP_INFO
	HEADER_CG_TARGET_INFO_LOAD					= 59,
#endif
	HEADER_CG_ITEM_USE_TO_ITEM					= 60,
	HEADER_CG_TARGET							= 61,
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
	HEADER_CG_SHOP_SEARCH						= 62,
#endif
	//HEADER_CG_EMPTY							= 63,
	//HEADER_CG_EMPTY							= 64,
	HEADER_CG_WARP								= 65,
	HEADER_CG_SCRIPT_BUTTON						= 66,
	HEADER_CG_MESSENGER							= 67,
	//HEADER_CG_EMPTY							= 68,
	HEADER_CG_MALL_CHECKOUT						= 69,
	HEADER_CG_SAFEBOX_CHECKIN					= 70,
	HEADER_CG_SAFEBOX_CHECKOUT					= 71,
	HEADER_CG_PARTY_INVITE						= 72,
	HEADER_CG_PARTY_INVITE_ANSWER				= 73,
	HEADER_CG_PARTY_REMOVE						= 74,
	HEADER_CG_PARTY_SET_STATE					= 75,
	HEADER_CG_PARTY_USE_SKILL					= 76,
	HEADER_CG_SAFEBOX_ITEM_MOVE					= 77,
	HEADER_CG_PARTY_PARAMETER					= 78,
	//HEADER_CG_EMPTY							= 79,
	HEADER_CG_GUILD								= 80,
	HEADER_CG_ANSWER_MAKE_GUILD					= 81,
	HEADER_CG_FISHING							= 82,
	HEADER_CG_GIVE_ITEM							= 83,
	//HEADER_CG_EMPTY							= 84,
	//HEADER_CG_EMPTY							= 85,
	//HEADER_CG_EMPTY							= 86,
	//HEADER_CG_EMPTY							= 87,
	//HEADER_CG_EMPTY							= 88,
	//HEADER_CG_EMPTY							= 89,
	HEADER_CG_EMPIRE							= 90,
	//HEADER_CG_EMPTY							= 91,
	//HEADER_CG_EMPTY							= 92,
#ifdef ENABLE_UN_SEAL_SCROLL_PLUS
	HEADER_CG_SAFEBOX_PASSWORD_TYPE				= 93,
#else
	//HEADER_CG_EMPTY							= 93,
#endif
	//HEADER_CG_EMPTY							= 94,
	//HEADER_CG_EMPTY							= 95,
	HEADER_CG_REFINE							= 96,
	//HEADER_CG_EMPTY							= 97,
	//HEADER_CG_EMPTY							= 98,
#ifdef ENABLE_HUNTING_SYSTEM
	HEADER_CG_SEND_HUNTING_ACTION				= 99,
#endif
	HEADER_CG_MARK_LOGIN						= 100,
	HEADER_CG_MARK_CRCLIST						= 101,
	HEADER_CG_MARK_UPLOAD						= 102,
	HEADER_CG_CRC_REPORT						= 103,
	HEADER_CG_MARK_IDXLIST						= 104,
	HEADER_CG_HACK								= 105,
	HEADER_CG_CHANGE_NAME						= 106,
	//HEADER_CG_EMPTY							= 107,
	//HEADER_CG_EMPTY							= 108,
	HEADER_CG_LOGIN2							= 109,
	HEADER_CG_DUNGEON							= 110,
	HEADER_CG_LOGIN3							= 111,
	HEADER_CG_GUILD_SYMBOL_UPLOAD				= 112,
	HEADER_CG_GUILD_SYMBOL_CRC					= 113,
	HEADER_CG_SCRIPT_SELECT_ITEM				= 114,
#ifdef ENABLE_STYLE_ATTRIBUTE_SYSTEM
	HEADER_CG_ITEM_USE_NEW_ATTRIBUTE 			= 115,
#endif

	//HEADER_CG_EMPTY							= 116,
	//HEADER_CG_EMPTY							= 117,
	//HEADER_CG_EMPTY							= 118,
	//HEADER_CG_EMPTY							= 119,

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	HEADER_CG_WHISPER_DETAILS					= 120,
#endif
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	HEADER_CG_ITEMS_PICKUP						= 121,
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	HEADER_CG_PET_HATCH							= 122,
	HEADER_CG_PET_WINDOW_TYPE					= 123,
	HEADER_CG_PET_WINDOW						= 124,
	HEADER_CG_PET_NAME_CHANGE					= 125,
	HEADER_CG_PET_FEED							= 126,
	HEADER_CG_PET_DETERMINE						= 127,
	HEADER_CG_PET_ATTR_CHANGE					= 128,
	HEADER_CG_PET_REVIVE						= 129,
	HEADER_CG_PET_LEARN_SKILL					= 130,
	HEADER_CG_PET_SKILL_UPGRADE					= 131,
	HEADER_CG_PET_DELETE_SKILL					= 132,
	HEADER_CG_PET_DELETE_ALL_SKILL				= 133,
#endif
#ifdef ENABLE_LEFT_SEAT_SYSTEM
	HEADER_CG_LEFT_SEAT							= 134,
#endif
#ifdef ENABLE_YIN_AND_YANG_ATTR
	HEADER_CG_YIN_YANG_ATTR						= 135,
#endif
	//HEADER_CG_EMPTY							= 136,
	//HEADER_CG_EMPTY							= 137,
	//HEADER_CG_EMPTY							= 138,
	//HEADER_CG_EMPTY							= 139,
	//HEADER_CG_EMPTY							= 140,
	//HEADER_CG_EMPTY							= 141,
	//HEADER_CG_EMPTY							= 142,
	//HEADER_CG_EMPTY							= 143,
	//HEADER_CG_EMPTY							= 144,
	//HEADER_CG_EMPTY							= 145,
	//HEADER_CG_EMPTY							= 146,
	//HEADER_CG_EMPTY							= 147,
	//HEADER_CG_EMPTY							= 148,
	//HEADER_CG_EMPTY							= 149,
	//HEADER_CG_EMPTY							= 150,
	//HEADER_CG_EMPTY							= 151,
	//HEADER_CG_EMPTY							= 152,
	//HEADER_CG_EMPTY							= 153,
	//HEADER_CG_EMPTY							= 154,
	//HEADER_CG_EMPTY							= 155,
	//HEADER_CG_EMPTY							= 156,
	//HEADER_CG_EMPTY							= 157,
	//HEADER_CG_EMPTY							= 158,
	//HEADER_CG_EMPTY							= 159,
	//HEADER_CG_EMPTY							= 160,
	//HEADER_CG_EMPTY							= 161,
	//HEADER_CG_EMPTY							= 162,
	//HEADER_CG_EMPTY							= 163,
	//HEADER_CG_EMPTY							= 164,
	//HEADER_CG_EMPTY							= 165,
	//HEADER_CG_EMPTY							= 166,
	//HEADER_CG_EMPTY							= 167,
	//HEADER_CG_EMPTY							= 168,
	//HEADER_CG_EMPTY							= 169,
	//HEADER_CG_EMPTY							= 170,
#ifdef ENABLE_RENEWAL_SWITCHBOT
	HEADER_CG_SWITCHBOT							= 171,
#endif
	//HEADER_CG_EMPTY							= 172,
	//HEADER_CG_EMPTY							= 173,
	//HEADER_CG_EMPTY							= 174,
	//HEADER_CG_EMPTY							= 175,
	//HEADER_CG_EMPTY							= 176,
	//HEADER_CG_EMPTY							= 177,
	//HEADER_CG_EMPTY							= 178,
	//HEADER_CG_EMPTY							= 179,
	//HEADER_CG_EMPTY							= 180,
	//HEADER_CG_EMPTY							= 181,
	//HEADER_CG_EMPTY							= 182,
	//HEADER_CG_EMPTY							= 183,
	//HEADER_CG_EMPTY							= 184,
	//HEADER_CG_EMPTY							= 185,
	//HEADER_CG_EMPTY							= 186,
	//HEADER_CG_EMPTY							= 187,
	//HEADER_CG_EMPTY							= 188,
	//HEADER_CG_EMPTY							= 189,
	//HEADER_CG_EMPTY							= 190,
	//HEADER_CG_EMPTY							= 191,
	//HEADER_CG_EMPTY							= 192,
	//HEADER_CG_EMPTY							= 193,
	//HEADER_CG_EMPTY							= 194,
	//HEADER_CG_EMPTY							= 195,
	//HEADER_CG_EMPTY							= 196,
	//HEADER_CG_EMPTY							= 197,
	//HEADER_CG_EMPTY							= 198,
	//HEADER_CG_EMPTY							= 199,
#ifdef ENABLE_AUTO_SELL_SYSTEM
    HEADER_CG_AUTO_SELL_ADD 					= 201,
    HEADER_CG_AUTO_SELL_REMOVE 					= 202,
    HEADER_CG_AUTO_SELL_STATUS 					= 203,
#endif
#ifdef ENABLE_RIDING_EXTENDED
	HEADER_CG_MOUNT_UP_GRADE 					= 204,
#endif
	//HEADER_CG_EMPTY							= 203,
	//HEADER_CG_EMPTY							= 204,
	HEADER_CG_DRAGON_SOUL_REFINE				= 205,
	//KULLANILIYOR								= 206,
	//HEADER_CG_EMPTY							= 207,
	//HEADER_CG_EMPTY							= 208,
	//HEADER_CG_EMPTY							= 209,
	//HEADER_CG_EMPTY							= 210,
	//HEADER_CG_EMPTY							= 212,
#ifdef ENABLE_DUNGEON_INFO
	HEADER_CG_DUNGEON_INFO_SYSTEM 				= 213,
#endif
	//HEADER_CG_EMPTY							= 214,
	//HEADER_CG_EMPTY							= 215,
	//HEADER_CG_EMPTY							= 216,
	//HEADER_CG_EMPTY							= 217,
#ifdef ENABLE_RENEWAL_CUBE
	HEADER_CG_CUBE_RENEWAL						= 218,
#endif
	//HEADER_CG_EMPTY							= 219,
	//HEADER_CG_EMPTY							= 220,
	//HEADER_CG_EMPTY							= 221,
	//HEADER_CG_EMPTY							= 222,
	//HEADER_CG_EMPTY							= 223,
	//HEADER_CG_EMPTY							= 224,
	//HEADER_CG_EMPTY							= 225,
	//HEADER_CG_EMPTY							= 226,
	//HEADER_CG_EMPTY							= 227,
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	HEADER_CG_CHANGE_LOOK						= 228,
#endif
#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
	HEADER_CG_ITEM_COMBINATION					= 229,
#else
	//HEADER_CG_EMPTY							= 229,
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	HEADER_CG_CHANGE_LANGUAGE					= 230,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	HEADER_CG_AURA								= 231,
#endif
	//HEADER_CG_EMPTY							= 232,
	//HEADER_CG_EMPTY							= 233,
	//HEADER_CG_EMPTY							= 234,
	//HEADER_CG_EMPTY							= 235,
	//HEADER_CG_EMPTY							= 236,
	//HEADER_CG_EMPTY							= 237,
	//HEADER_CG_EMPTY							= 238,
	//HEADER_CG_EMPTY							= 239,
	//HEADER_CG_EMPTY							= 240,
	//HEADER_CG_EMPTY							= 241,
	//HEADER_CG_EMPTY							= 242,
	//HEADER_CG_EMPTY							= 243,
	//HEADER_CG_EMPTY							= 244,
	//HEADER_CG_EMPTY							= 245,
	//HEADER_CG_EMPTY							= 246,
	//HEADER_CG_EMPTY							= 247,
	//HEADER_CG_EMPTY							= 248,
	//HEADER_CG_EMPTY							= 249,
	//HEADER_CG_EMPTY							= 250,
	//HEADER_CG_EMPTY							= 251,
	HEADER_CG_TIME_SYNC							= 0xfc,
	//HEADER_CG_EMPTY							= 253,
	HEADER_CG_PONG								= 0xfe,
	HEADER_CG_HANDSHAKE							= 0xff,
};

enum PacketHeaderGC
{
	HEADER_GC_CHARACTER_ADD						= 1,
	HEADER_GC_CHARACTER_DEL						= 2,
	HEADER_GC_CHARACTER_MOVE					= 3,
	HEADER_GC_CHAT								= 4,
	HEADER_GC_SYNC_POSITION 					= 5,
	HEADER_GC_LOGIN_SUCCESS3					= 6,
	HEADER_GC_LOGIN_FAILURE						= 7,
	HEADER_GC_PLAYER_CREATE_SUCCESS				= 8,
	HEADER_GC_PLAYER_CREATE_FAILURE				= 9,
	HEADER_GC_PLAYER_DELETE_SUCCESS				= 10,
	HEADER_GC_PLAYER_DELETE_WRONG_SOCIAL_ID		= 11,
	//HEADER_GC_EMPTY							= 12,
	HEADER_GC_STUN								= 13,
	HEADER_GC_DEAD								= 14,
	HEADER_GC_MAIN_CHARACTER					= 15,
	HEADER_GC_PLAYER_POINTS						= 16,
	HEADER_GC_PLAYER_POINT_CHANGE				= 17,
	HEADER_GC_CHANGE_SPEED						= 18,
	HEADER_GC_CHARACTER_UPDATE					= 19,
	HEADER_GC_ITEM_SET							= 20,
	HEADER_GC_ITEM_SET2							= 21,
	HEADER_GC_ITEM_USE							= 22,
	HEADER_GC_ITEM_DROP							= 23,
	//HEADER_GC_EMPTY							= 24,
	HEADER_GC_ITEM_UPDATE						= 25,
	HEADER_GC_ITEM_GROUND_ADD					= 26,
	HEADER_GC_ITEM_GROUND_DEL					= 27,
	HEADER_GC_QUICKSLOT_ADD						= 28,
	HEADER_GC_QUICKSLOT_DEL						= 29,
	HEADER_GC_QUICKSLOT_SWAP					= 30,
	HEADER_GC_ITEM_OWNERSHIP					= 31,
	HEADER_GC_LOGIN_SUCCESS4					= 32,
	//HEADER_GC_EMPTY							= 33,
	HEADER_GC_WHISPER							= 34,
	//HEADER_GC_EMPTY							= 35,
	HEADER_GC_MOTION							= 36,
	HEADER_GC_PARTS								= 37,	/* server sends appearance update; match server enum */
	HEADER_GC_SHOP								= 38,
	HEADER_GC_SHOP_SIGN							= 39,
	HEADER_GC_DUEL_START						= 40,
	HEADER_GC_PVP								= 41,
	HEADER_GC_EXCHANGE							= 42,
	HEADER_GC_CHARACTER_POSITION				= 43,
	HEADER_GC_PING								= 44,
	HEADER_GC_SCRIPT							= 45,
	HEADER_GC_QUEST_CONFIRM						= 46,

	//HEADER_CG_EMPTY							= 47,

	HEADER_GC_GUILDMARK_PASS					= 48,

#ifdef ENABLE_RESP_SYSTEM
	HEADER_GC_RESP 								= 49,
#endif
#ifdef ENABLE_FISH_EVENT_SYSTEM
	HEADER_GC_FISH_EVENT_INFO 					= 50,
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	HEADER_GC_OFFLINE_SHOP						= 51,
	HEADER_GC_OFFLINE_SHOP_SIGN					= 52,
#endif
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
	HEADER_GC_SHOPSEARCH_SET					= 53,
#endif
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	HEADER_GC_SOUL_ROULETTE						= 54,
#endif
#ifdef ENABLE_ATTENDANCE_EVENT
	HEADER_GC_HIT_COUNT_INFO 					= 55,
	HEADER_GC_ATTENDANCE_EVENT 					= 56,
	HEADER_GC_ATTENDANCE_EVENT_INFO 			= 57,
#endif
#ifdef ENABLE_MOB_DROP_INFO
	HEADER_GC_TARGET_INFO						= 58,
#endif
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
	HEADER_GC_PICKUP_ITEM_SC					= 59,
#endif
#ifdef ENABLE_MINI_GAME_CATCH_KING
	HEADER_GC_MINI_GAME_CATCH_KING 				= 60,
#endif
	HEADER_GC_MOUNT								= 61,
	HEADER_GC_OWNERSHIP							= 62,
	HEADER_GC_TARGET							= 63,
#ifdef ENABLE_STONE_EVENT
	HEADER_GC_STONE_EVENT 						= 64,
#endif
	HEADER_GC_WARP								= 65,
#ifdef ENABLE_KILL_STATISTICS
	HEADER_GC_KILL_STATISTICS 					= 66,
#endif
	//HEADER_GC_EMPTY							= 67,
	//HEADER_GC_EMPTY							= 68,
	HEADER_GC_ADD_FLY_TARGETING					= 69,
	HEADER_GC_CREATE_FLY						= 70,
	HEADER_GC_FLY_TARGETING						= 71,
	HEADER_GC_SKILL_LEVEL						= 72,
	HEADER_GC_SKILL_COOLTIME_END				= 73,
	HEADER_GC_MESSENGER							= 74,
	HEADER_GC_GUILD								= 75,
	HEADER_GC_SKILL_LEVEL_NEW					= 76,
	HEADER_GC_PARTY_INVITE						= 77,
	HEADER_GC_PARTY_ADD							= 78,
	HEADER_GC_PARTY_UPDATE						= 79,
	HEADER_GC_PARTY_REMOVE						= 80,
	HEADER_GC_QUEST_INFO						= 81,
	HEADER_GC_REQUEST_MAKE_GUILD				= 82,
	HEADER_GC_PARTY_PARAMETER					= 83,
	HEADER_GC_SAFEBOX_MONEY_CHANGE				= 84,
	HEADER_GC_SAFEBOX_SET						= 85,
	HEADER_GC_SAFEBOX_DEL						= 86,
	HEADER_GC_SAFEBOX_WRONG_PASSWORD			= 87,
	HEADER_GC_SAFEBOX_SIZE						= 88,
	HEADER_GC_FISHING							= 89,
	HEADER_GC_EMPIRE							= 90,
	HEADER_GC_PARTY_LINK						= 91,
	HEADER_GC_PARTY_UNLINK						= 92,
#ifdef ENABLE_ITEMSHOP
	HEADER_GC_ITEMSHOP 							= 93,
#endif
#ifdef ENABLE_UN_SEAL_SCROLL_PLUS
	HEADER_GC_SAFEBOX_PASSWORD_SUCCESS			= 94,
#else
	//HEADER_GC_EMPTY							= 94,
#endif
	HEADER_GC_REFINE_INFORMATION				= 95,
	HEADER_GC_OBSERVER_ADD						= 96,
	HEADER_GC_OBSERVER_REMOVE					= 97,
	HEADER_GC_OBSERVER_MOVE						= 98,
	HEADER_GC_VIEW_EQUIP						= 99,
	HEADER_GC_MARK_BLOCK						= 100,
	HEADER_GC_MARK_DIFF_DATA					= 101,
	HEADER_GC_MARK_IDXLIST						= 102,
	//HEADER_GC_EMPTY							= 103,
	//HEADER_GC_EMPTY							= 104,
	//HEADER_GC_EMPTY							= 105,
	HEADER_GC_TIME								= 106,
	HEADER_GC_CHANGE_NAME						= 107,
	//HEADER_GC_EMPTY							= 108,
	//HEADER_GC_EMPTY							= 109,
	HEADER_GC_DUNGEON							= 110,
	HEADER_GC_WALK_MODE							= 111,
	HEADER_GC_CHANGE_SKILL_GROUP				= 112,
	HEADER_GC_MAIN_CHARACTER2_EMPIRE			= 113,
	HEADER_GC_SEPCIAL_EFFECT					= 114,
	HEADER_GC_NPC_POSITION						= 115,
	//HEADER_GC_EMPTY							= 116,
	HEADER_GC_CHARACTER_UPDATE2					= 117,
	HEADER_GC_LOGIN_KEY							= 118,
	HEADER_GC_REFINE_INFORMATION_NEW			= 119,
	HEADER_GC_CHARACTER_ADD2					= 120,
	HEADER_GC_CHANNEL							= 121,
	HEADER_GC_MALL_OPEN							= 122,
	HEADER_GC_TARGET_UPDATE						= 123,
	HEADER_GC_TARGET_DELETE						= 124,
	HEADER_GC_TARGET_CREATE_NEW					= 125,
	HEADER_GC_AFFECT_ADD						= 126,
	HEADER_GC_AFFECT_REMOVE						= 127,
	HEADER_GC_MALL_SET							= 128,
	HEADER_GC_MALL_DEL							= 129,
	HEADER_GC_LAND_LIST							= 130,
	HEADER_GC_LOVER_INFO						= 131,
	HEADER_GC_LOVE_POINT_UPDATE					= 132,
	HEADER_GC_GUILD_SYMBOL_DATA					= 133,
	HEADER_GC_DIG_MOTION						= 134,
	HEADER_GC_DAMAGE_INFO						= 135,
	HEADER_GC_CHAR_ADDITIONAL_INFO				= 136,
	HEADER_GC_MAIN_CHARACTER3_BGM				= 137,
	HEADER_GC_MAIN_CHARACTER4_BGM_VOL			= 138,
#ifdef ENABLE_HUNTING_SYSTEM
	HEADER_GC_HUNTING_OPEN_MAIN					= 139,
	HEADER_GC_HUNTING_OPEN_SELECT				= 140,
	HEADER_GC_HUNTING_OPEN_REWARD				= 141,
	HEADER_GC_HUNTING_UPDATE					= 142,
	HEADER_GC_HUNTING_RECIVE_RAND_ITEMS			= 143,
#endif
	//HEADER_GC_EMPTY							= 144,
	//HEADER_GC_EMPTY							= 145,
	//HEADER_GC_EMPTY							= 146,
	//HEADER_GC_EMPTY							= 147,
	//HEADER_GC_EMPTY							= 148,
	//HEADER_GC_EMPTY							= 149,
	HEADER_GC_AUTH_SUCCESS						= 150,
#ifdef ENABLE_CLIENT_LOCALE_STRING
	HEADER_GC_LOCALE_CHAT						= 151,
	HEADER_GC_LOCALE_WHISPER					= 152,
#endif
	//HEADER_GC_EMPTY							= 153,
	//HEADER_GC_EMPTY							= 154,
#ifdef ENABLE_RIDING_EXTENDED
	HEADER_GC_MOUNT_UP_GRADE 					= 155,
	HEADER_GC_MOUNT_UP_GRADE_CHAT 				= 156,
#endif
#ifdef ENABLE_COLLECT_WINDOW
	HEADER_GC_COLLECT 							= 157,
#endif
	//HEADER_GC_EMPTY							= 158,
	//HEADER_GC_EMPTY							= 159,
#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	HEADER_GC_WHISPER_DETAILS					= 160,
#endif
	//HEADER_GC_EMPTY							= 162,
	//HEADER_GC_EMPTY							= 163,
	//HEADER_GC_EMPTY							= 164,
	//HEADER_GC_EMPTY							= 165,
	//HEADER_GC_EMPTY							= 166,
	//HEADER_GC_EMPTY							= 167,
	//HEADER_GC_EMPTY							= 168,
	//HEADER_GC_EMPTY							= 169,
	//HEADER_GC_EMPTY							= 170,
#ifdef ENABLE_RENEWAL_SWITCHBOT
	HEADER_GC_SWITCHBOT							= 171,
#endif
	//HEADER_GC_EMPTY							= 172,
#ifdef ENABLE_GROWTH_PET_SYSTEM
	HEADER_GC_PET								= 173,
	HEADER_GC_PET_SET							= 174,
	HEADER_GC_PET_SET_EXCHANGE					= 175,
	HEADER_GC_PET_DEL							= 176,
	//HEADER_GC_EMPTY							= 177,
	HEADER_GC_PET_SUMMON						= 178,
	HEADER_GC_PET_POINT_CHANGE					= 179,
	HEADER_GC_PET_NAME_CHANGE_RESULT			= 180,
	HEADER_GC_PET_DETERMINE_RESULT				= 181,
	HEADER_GC_PET_ATTR_CHANGE_RESULT			= 182,
	HEADER_GC_PET_SKILL_UPDATE					= 183,
	HEADER_GC_PET_SKILL_COOLTIME				= 184,
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
	HEADER_GC_SUNGMA_ATTR_UPDATE 				= 185,
#endif
#ifdef ENABLE_STATUS_EFFECT_SHOWER
	HEADER_GC_STATUS_EFFECT						= 186,
#endif
#ifdef ENABLE_YIN_AND_YANG_ATTR
	HEADER_GC_YIN_YANG_ATTR						= 187,
#endif
#ifdef ENABLE_BUFFI_SYSTEM
	HEADER_GC_BUFFI_USE_SKILL					= 188,
#else
	//HEADER_GC_EMPTY							= 188,
#endif
	//HEADER_GC_EMPTY							= 189,
	//HEADER_GC_EMPTY							= 190,
	//HEADER_GC_EMPTY							= 191,
	//HEADER_GC_EMPTY							= 192,
	//HEADER_GC_EMPTY							= 193,
	//HEADER_GC_EMPTY							= 194,
	//HEADER_GC_EMPTY							= 195,
	//HEADER_GC_EMPTY							= 196,
	//HEADER_GC_EMPTY							= 197,
	//HEADER_GC_EMPTY							= 198,
	//HEADER_GC_EMPTY							= 199,
	//HEADER_GC_EMPTY							= 200,
	//HEADER_GC_EMPTY							= 201,
	//HEADER_GC_EMPTY							= 202,
	//HEADER_GC_EMPTY							= 203,
	//HEADER_GC_EMPTY							= 204,
	//HEADER_GC_EMPTY							= 205,
	//HEADER_GC_EMPTY							= 206,
	//HEADER_GC_EMPTY							= 207,
	HEADER_GC_SPECIFIC_EFFECT					= 208,
	HEADER_GC_DRAGON_SOUL_REFINE				= 209,
	HEADER_GC_RESPOND_CHANNELSTATUS				= 210,
	//HEADER_GC_EMPTY							= 212,
#ifdef ENABLE_EVENT_SYSTEM
	HEADER_GC_EVENT_INFO 						= 213,
#endif
	//HEADER_GC_EMPTY							= 214,
#ifdef ENABLE_PARTY_POSITION
	HEADER_GC_PARTY_POSITION_INFO				= 215,
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	HEADER_GC_REQUEST_CHANGE_LANGUAGE			= 216,
#endif
	//KULLANILIYOR 								= 217,
#ifdef ENABLE_RENEWAL_CUBE
	HEADER_GC_CUBE_RENEWAL						= 218,
#endif

	//HEADER_CG_EMPTY							= 219,
	//HEADER_CG_EMPTY							= 220,
	//HEADER_CG_EMPTY							= 221,
	//HEADER_CG_EMPTY							= 222,
	//HEADER_CG_EMPTY							= 223,

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	HEADER_GC_AURA								= 224,
#endif
#ifdef ENABLE_DUNGEON_INFO
	HEADER_GC_DUNGEON_INFO_SYSTEM 				= 225,
#endif
	//HEADER_GC_EMPTY							= 226,
#ifdef ENABLE_RANKING
	HEADER_GC_RANKING_SEND 						= 227,
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	HEADER_GC_CHANGE_LOOK_SET					= 228,
	HEADER_GC_CHANGE_LOOK_DEL					= 229,
	HEADER_GC_CHANGE_LOOK_FREE_SET				= 230,
	HEADER_GC_CHANGE_LOOK_FREE_DEL				= 231,
#endif
	//HEADER_GC_EMPTY							= 232,
	//HEADER_GC_EMPTY							= 233,
	//HEADER_GC_EMPTY							= 234,
	//HEADER_GC_EMPTY							= 235,
	//HEADER_GC_EMPTY							= 236,
	//HEADER_GC_EMPTY							= 237,
	//HEADER_GC_EMPTY							= 238,
	//HEADER_GC_EMPTY							= 239,
	//HEADER_GC_EMPTY							= 240,
	//HEADER_GC_EMPTY							= 241,
	//HEADER_GC_EMPTY							= 242,
	//HEADER_GC_EMPTY							= 243,
	//HEADER_GC_EMPTY							= 244,
	//HEADER_GC_EMPTY							= 245,
	//HEADER_GC_EMPTY							= 246,
	//HEADER_GC_EMPTY							= 247,
	//HEADER_GC_EMPTY							= 248,
	//HEADER_GC_EMPTY							= 249,
	//HEADER_GC_EMPTY							= 250,
	//HEADER_GC_EMPTY							= 251,
	HEADER_GC_HANDSHAKE_OK						= 0xfc,
	HEADER_GC_PHASE								= 0xfd,
	HEADER_GC_BINDUDP							= 0xfe,
	HEADER_GC_HANDSHAKE							= 0xff,
};

enum
{
	ID_MAX_NUM = 30,
	PASS_MAX_NUM = 16,
	CHAT_MAX_NUM = 128,
	PATH_NODE_MAX_NUM = 64,

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	OFFLINE_SHOP_HOST_ITEM_MAX_NUM = 80,
	SHOP_SIGN_MAX_LEN = 45,
#else
	SHOP_SIGN_MAX_LEN = 32,
#endif

	PLAYER_PER_ACCOUNT3 = 3,
	PLAYER_PER_ACCOUNT4 = 5,
	PLAYER_PER_ACCOUNT5 = 5,

	PLAYER_ITEM_SLOT_MAX_NUM = 20,

	QUICKSLOT_MAX_LINE = 4,
	QUICKSLOT_MAX_COUNT_PER_LINE = 8,
	QUICKSLOT_MAX_COUNT = QUICKSLOT_MAX_LINE * QUICKSLOT_MAX_COUNT_PER_LINE,

	QUICKSLOT_MAX_NUM = 36,

	SHOP_HOST_ITEM_MAX_NUM = 40,

	METIN_SOCKET_COUNT = 6,

	PARTY_AFFECT_SLOT_MAX_NUM = 7,

	GUILD_GRADE_NAME_MAX_LEN = 8,
	GUILD_NAME_MAX_LEN = 12,
	GUILD_GRADE_COUNT = 15,
	GULID_COMMENT_MAX_LEN = 50,

	MARK_CRC_NUM = 8 * 8,
	MARK_DATA_SIZE = 16 * 12,
	SYMBOL_DATA_SIZE = 128 * 256,
	QUEST_INPUT_STRING_MAX_NUM = 64,

#ifdef ENABLE_RENEWAL_OX_EVENT
	QUEST_INPUT_LONG_STRING_MAX_NUM = 128,
#endif

	PRIVATE_CODE_LENGTH = 8,

	REFINE_MATERIAL_MAX_NUM = 5,

	WEAR_MAX_NUM = 64,

	SHOP_TAB_NAME_MAX = 32,

#ifdef ENABLE_RENEWAL_SHOPEX
	SHOP_TAB_COUNT_MAX = 7,
#else
	SHOP_TAB_COUNT_MAX = 3,
#endif
};

#pragma pack(push)
#pragma pack(1)

typedef struct command_mark_login
{
	BYTE    header;
	DWORD   handle;
	DWORD   random_key;
} TPacketCGMarkLogin;

typedef struct command_mark_upload
{
	BYTE    header;
	DWORD   gid;
	BYTE    image[16 * 12 * 4];
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
	BYTE    header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
	BYTE    header;
	BYTE    imgIdx;
	DWORD   crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
	BYTE    header;
	DWORD	bufSize;
	WORD    count;
} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
	BYTE    header;
	DWORD   bufSize;
	BYTE	imgIdx;
	DWORD   count;
} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE	header;
	WORD	size;
	DWORD	handle;
} TPacketCGSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE	header;
	DWORD	dwGuildID;
	DWORD	dwCRC;
	DWORD	dwSize;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
	BYTE header;
	WORD size;
	DWORD guild_id;
} TPacketGCGuildSymbolData;

typedef struct packet_observer_add
{
	BYTE	header;
	DWORD	vid;
	WORD	x;
	WORD	y;
} TPacketGCObserverAdd;

typedef struct packet_observer_move
{
	BYTE	header;
	DWORD	vid;
	WORD	x;
	WORD	y;
} TPacketGCObserverMove;

typedef struct packet_observer_remove
{
	BYTE	header;
	DWORD	vid;
} TPacketGCObserverRemove;

typedef struct command_checkin
{
	BYTE header;
	char name[ID_MAX_NUM + 1];
	char pwd[PASS_MAX_NUM + 1];
} TPacketCGCheckin;

typedef struct command_login
{
	BYTE header;
	char name[ID_MAX_NUM + 1];
	char pwd[PASS_MAX_NUM + 1];
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE	header;
	char	name[ID_MAX_NUM + 1];
	DWORD	login_key;
	DWORD	adwClientKey[4];
} TPacketCGLogin2;

typedef struct command_login3
{
	BYTE	header;
	char	name[ID_MAX_NUM + 1];
	char	pwd[PASS_MAX_NUM + 1];
	DWORD	adwClientKey[4];
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE	bLanguage;
#endif
#ifdef ENABLE_RENEWAL_CLIENT_VERSION
	char	ClientVersion[10];
#endif
} TPacketCGLogin3;

typedef struct command_direct_enter
{
	BYTE	bHeader;
	char	login[ID_MAX_NUM + 1];
	char	passwd[PASS_MAX_NUM + 1];
	BYTE	index;
} TPacketCGDirectEnter;

typedef struct command_player_select
{
	BYTE	header;
	BYTE	player_index;
} TPacketCGSelectCharacter;

typedef struct command_attack
{
	BYTE	header;
	BYTE	bType;
	DWORD	dwVictimVID;
	BYTE	bCRCMagicCubeProcPiece;
	BYTE	bCRCMagicCubeFilePiece;
} TPacketCGAttack;

typedef struct command_chat
{
	BYTE	header;
	WORD	length;
	BYTE	type;
} TPacketCGChat;

typedef struct command_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	char	szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

enum EBattleMode
{
	BATTLEMODE_ATTACK = 0,
	BATTLEMODE_DEFENSE = 1,
};

typedef struct command_EnterFrontGame
{
	BYTE header;
} TPacketCGEnterFrontGame;

typedef struct command_item_use
{
	BYTE header;
	TItemPos pos;
} TPacketCGItemUse;

typedef struct command_item_use_to_item
{
	BYTE header;
	TItemPos source_pos;
	TItemPos target_pos;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE header;
	TItemPos pos;
	DWORD elk;
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
	BYTE header;
	TItemPos pos;
	DWORD gold;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
} TPacketCGItemDrop2;

#ifdef ENABLE_DESTROY_DIALOG
typedef struct command_item_destroy
{
	BYTE header;
	TItemPos pos;
	DWORD gold;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
} TPacketCGItemDestroy;
#endif

typedef struct command_item_move
{
	BYTE header;
	TItemPos pos;
	TItemPos change_pos;
#ifdef ENABLE_STACK_LIMIT
	WORD num;
#else
	BYTE num;
#endif
} TPacketCGItemMove;

#ifdef ENABLE_QUICK_SELL_ITEM
typedef struct command_item_sell
{
	BYTE header;
	TItemPos pos;
	DWORD gold;
} TPacketCGItemSell;
#endif

typedef struct command_item_pickup
{
	BYTE header;
	DWORD vid;
} TPacketCGItemPickUp;

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
typedef struct command_items_pickup
{
	BYTE header;
	DWORD count;
} TPacketCGItemsPickUp;
#endif

typedef struct command_quickslot_add
{
	BYTE header;
	BYTE pos;
	TQuickSlot slot;
}TPacketCGQuickSlotAdd;

typedef struct command_quickslot_del
{
	BYTE header;
	BYTE pos;
}TPacketCGQuickSlotDel;

typedef struct command_quickslot_swap
{
	BYTE header;
	BYTE pos;
	BYTE change_pos;
}TPacketCGQuickSlotSwap;

typedef struct command_on_click
{
	BYTE header;
	DWORD vid;
} TPacketCGOnClick;

enum
{
	SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2,
#ifdef ENABLE_SHOP_REPURCHASE
	SHOP_SUBHEADER_CG_REPURCHASE_OPEN = 4,
	SHOP_SUBHEADER_CG_REPURCHASE_CLOSE = 5,
	SHOP_SUBHEADER_CG_REPURCHASE = 6,
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	SHOP_SUBHEADER_CG_ADD_ITEM,
	SHOP_SUBHEADER_CG_REMOVE_ITEM,
	SHOP_SUBHEADER_CG_DESTROY_OFFLINE_SHOP,
	SHOP_SUBHEADER_CG_CHECK,
	SHOP_SUBHEADER_CG_BUTTON,
	SHOP_SUBHEADER_CG_CHANGE_TITLE,
	SHOP_SUBHEADER_CG_TAKE_MONEY,
	SHOP_SUBHEADER_CG_LOG_REMOVE,
	SHOP_SUBHEADER_CG_CHANGE_DECORATION,
	SHOP_SUBHEADER_CG_OPEN_SLOT,
	SHOP_SUBHEADER_CG_GET_ITEM,
	SHOP_SUBHEADER_CG_ADD_TIME,
	SHOP_SUBHEADER_CG_OPEN_WITH_VID
#endif
};

typedef struct command_shop
{
	BYTE header;
#ifdef ENABLE_STACK_LIMIT
	WORD subheader;
#else
	BYTE subheader;
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	char title[50 + 1];
	DWORD vid;
	BYTE pos;
#endif
} TPacketCGShop;

#ifdef ENABLE_SHOP_REPURCHASE
#pragma pack(push, 1)
typedef struct command_shop_repurchase_open
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketCGShopRepurchaseOpen;

typedef struct command_shop_repurchase_close
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketCGShopRepurchaseClose;

typedef struct command_shop_repurchase
{
	BYTE header;
	WORD size;
	BYTE subheader;
	DWORD item_id;
	WORD pos;
	BYTE count;
} TPacketCGShopRepurchase;
#pragma pack(pop)
#endif

enum
{
	EXCHANGE_SUBHEADER_CG_START,
	EXCHANGE_SUBHEADER_CG_ITEM_ADD,
	EXCHANGE_SUBHEADER_CG_ITEM_DEL,
	EXCHANGE_SUBHEADER_CG_ELK_ADD,
	EXCHANGE_SUBHEADER_CG_ACCEPT,
	EXCHANGE_SUBHEADER_CG_CANCEL,
};

typedef struct command_exchange
{
	BYTE header;
	BYTE subheader;
#ifdef ENABLE_GOLD_LIMIT
	long long arg1;
#else
	DWORD arg1;
#endif
	BYTE arg2;
	TItemPos Pos;
} TPacketCGExchange;

typedef struct command_position
{
	BYTE header;
	BYTE position;
} TPacketCGPosition;

typedef struct command_script_answer
{
	BYTE header;
	BYTE answer;
} TPacketCGScriptAnswer;

typedef struct command_script_button
{
	BYTE header;
	unsigned int idx;
#ifdef ENABLE_COLLECT_WINDOW
	BYTE button;
#endif
} TPacketCGScriptButton;

typedef struct command_target
{
	BYTE header;
	DWORD dwVID;
} TPacketCGTarget;

typedef struct command_move
{
	BYTE bHeader;
	BYTE bFunc;
	BYTE bArg;
	BYTE bRot;
	LONG lX;
	LONG lY;
	DWORD dwTime;
} TPacketCGMove;

typedef struct command_sync_position_element
{
	DWORD dwVID;
	long lX;
	long lY;
} TPacketCGSyncPositionElement;

typedef struct command_sync_position
{
	BYTE bHeader;
	WORD wSize;
} TPacketCGSyncPosition;

typedef struct command_fly_targeting
{
	BYTE bHeader;
	DWORD dwTargetVID;
	long lX;
	long lY;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
	BYTE bHeader;
	DWORD dwShooterVID;
	DWORD dwTargetVID;
	long lX;
	long lY;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
	BYTE bHeader;
	BYTE bType;
} TPacketCGShoot;

typedef struct command_warp
{
	BYTE bHeader;
} TPacketCGWarp;

enum
{
#ifdef ENABLE_MESSENGER_TEAM
	MESSENGER_SUBHEADER_GC_TEAM_LIST,
	MESSENGER_SUBHEADER_GC_TEAM_LOGIN,
	MESSENGER_SUBHEADER_GC_TEAM_LOGOUT,
#endif
#ifdef ENABLE_MESSENGER_BLOCK
	MESSENGER_SUBHEADER_GC_BLOCK_LIST,
	MESSENGER_SUBHEADER_GC_BLOCK_LOGIN,
	MESSENGER_SUBHEADER_GC_BLOCK_LOGOUT,
	MESSENGER_SUBHEADER_GC_BLOCK_INVITE,
#endif
	MESSENGER_SUBHEADER_GC_LIST,
	MESSENGER_SUBHEADER_GC_LOGIN,
	MESSENGER_SUBHEADER_GC_LOGOUT,
	MESSENGER_SUBHEADER_GC_REMOVE_FRIEND,
	MESSENGER_SUBHEADER_GC_INVITE,
};

typedef struct packet_messenger
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCMessenger;

typedef struct packet_messenger_list_offline
{
	BYTE connected;
	BYTE length;
} TPacketGCMessengerListOffline;

#ifdef ENABLE_MESSENGER_TEAM
typedef struct packet_messenger_team_list_offline
{
	BYTE connected;
	BYTE length;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE lang;
	BYTE lang2nd;
#endif
} TPacketGCMessengerTeamListOffline;

typedef struct packet_messenger_team_list_online
{
	BYTE connected;
	BYTE length;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE lang;
	BYTE lang2nd;
#endif
} TPacketGCMessengerTeamListOnline;
#endif

#ifdef ENABLE_MESSENGER_BLOCK
typedef struct packet_messenger_block_list_offline
{
	BYTE connected;
	BYTE length;
} TPacketGCMessengerBlockListOffline;

typedef struct packet_messenger_block_list_online
{
	BYTE connected;
	BYTE length;
} TPacketGCMessengerBlockListOnline;
#endif

enum
{
	MESSENGER_CONNECTED_STATE_OFFLINE,
	MESSENGER_CONNECTED_STATE_ONLINE,
};

typedef struct packet_messenger_list_online
{
	BYTE connected;
	BYTE length;
} TPacketGCMessengerListOnline;

typedef struct packet_messenger_login
{
	BYTE length;
} TPacketGCMessengerLogin;

typedef struct packet_messenger_logout
{
	BYTE length;
} TPacketGCMessengerLogout;

enum
{
#ifdef ENABLE_MESSENGER_BLOCK
	MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_BLOCK_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_BLOCK_REMOVE_BLOCK,
#endif
#ifdef ENABLE_TELEPORT_TO_A_FRIEND
	MESSENGER_SUBHEADER_CG_REQUEST_WARP_BY_NAME,
	MESSENGER_SUBHEADER_CG_SUMMON_BY_NAME,
#endif
	MESSENGER_SUBHEADER_CG_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_REMOVE,
};

typedef struct command_messenger
{
	BYTE header;
	BYTE subheader;
} TPacketCGMessenger;

typedef struct command_messenger_remove
{
	BYTE length;
} TPacketCGMessengerRemove;

enum
{
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
	BYTE bHeader;
	BYTE bState;
	DWORD dwMoney;
} TPacketCGSafeboxMoney;

typedef struct command_safebox_checkout
{
	BYTE bHeader;
#ifdef ENABLE_EXTENDED_SAFEBOX
	DWORD bSafePos;
#else
	BYTE bSafePos;
#endif
	TItemPos ItemPos;
} TPacketCGSafeboxCheckout;

typedef struct command_safebox_checkin
{
	BYTE bHeader;
#ifdef ENABLE_EXTENDED_SAFEBOX
	DWORD bSafePos;
#else
	BYTE bSafePos;
#endif
	TItemPos ItemPos;
} TPacketCGSafeboxCheckin;

#ifdef ENABLE_UN_SEAL_SCROLL_PLUS
typedef struct command_safebox_password_type
{
	BYTE bHeader;
	WORD wSize;
	BYTE bType;
	char szPassword[7];
} TPacketCGSafeboxPasswordType;
#endif

typedef struct command_mall_checkout
{
	BYTE bHeader;
#ifdef ENABLE_EXTENDED_SAFEBOX
	DWORD bMallPos;
#else
	BYTE bMallPos;
#endif
	TItemPos ItemPos;
} TPacketCGMallCheckout;

typedef struct command_use_skill
{
	BYTE bHeader;
	DWORD dwVnum;
	DWORD dwTargetVID;
} TPacketCGUseSkill;

typedef struct command_party_invite
{
	BYTE header;
	DWORD vid;
} TPacketCGPartyInvite;

typedef struct command_party_invite_answer
{
	BYTE header;
	DWORD leader_pid;
	BYTE accept;
} TPacketCGPartyInviteAnswer;

typedef struct command_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
	BYTE byHeader;
	DWORD dwVID;
	BYTE byState;
	BYTE byFlag;
} TPacketCGPartySetState;

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

typedef struct command_party_use_skill
{
	BYTE byHeader;
	BYTE bySkillIndex;
	DWORD dwTargetVID;
} TPacketCGPartyUseSkill;

enum
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
};

typedef struct command_guild
{
	BYTE byHeader;
	BYTE bySubHeader;
} TPacketCGGuild;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN+1];
} TPacketCGAnswerMakeGuild;

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
#ifdef ENABLE_STACK_LIMIT
	WORD byItemCount;
#else
	BYTE byItemCount;
#endif
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
	BYTE bHeader;
	char szBuf[255 + 1];
} TPacketCGHack;

typedef struct command_dungeon
{
	BYTE bHeader;
	WORD size;
} TPacketCGDungeon;

typedef struct SShopItemTable
{
	DWORD vnum;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif

	TItemPos pos;
#ifdef ENABLE_GOLD_LIMIT
	long long price;
#else
	DWORD price;
#endif
	BYTE display_pos;

#ifdef ENABLE_RENEWAL_SHOPEX
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];

	BYTE price_type;
	DWORD price_vnum;

	SShopItemTable() : price_type(SHOPEX_GOLD), price_vnum(0)
	{
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttr, 0, sizeof(aAttr));
	}
#endif
} TShopItemTable;

typedef struct SPacketCGMyShop
{
	BYTE bHeader;
	char szSign[SHOP_SIGN_MAX_LEN + 1];
#ifdef ENABLE_STACK_LIMIT
	WORD bCount;
#else
	BYTE bCount;
#endif
} TPacketCGMyShop;

typedef struct SPacketCGRefine
{
	BYTE header;
	BYTE pos;
	BYTE type;
#ifdef ENABLE_PITTY_REFINE
	bool seal_of_god;
#endif
} TPacketCGRefine;

typedef struct SPacketCGChangeName
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketCGChangeName;

typedef struct command_crc_report
{
	BYTE header;
	BYTE byPackMode;
	DWORD dwBinaryCRC32;
	DWORD dwProcessCRC32;
	DWORD dwRootPackCRC32;
} TPacketCGCRCReport;

enum EPartyExpDistributionType
{
	PARTY_EXP_DISTRIBUTION_NON_PARITY,
	PARTY_EXP_DISTRIBUTION_PARITY,
};

typedef struct command_party_parameter
{
	BYTE bHeader;
	BYTE bDistributeMode;
} TPacketCGPartyParameter;

typedef struct command_quest_input_string
{
	BYTE bHeader;
	char szString[QUEST_INPUT_STRING_MAX_NUM+1];
} TPacketCGQuestInputString;

#ifdef ENABLE_RENEWAL_OX_EVENT
typedef struct command_quest_input_long_string
{
	BYTE bHeader;
	char szString[QUEST_INPUT_LONG_STRING_MAX_NUM + 1];
} TPacketCGQuestInputLongString;
#endif

typedef struct command_quest_confirm
{
	BYTE header;
	BYTE answer;
	DWORD requestPID;
} TPacketCGQuestConfirm;

typedef struct command_script_select_item
{
	BYTE header;
	DWORD selection;
} TPacketCGScriptSelectItem;

enum EPhase
{
	PHASE_CLOSE,
	PHASE_HANDSHAKE,
	PHASE_LOGIN,
	PHASE_SELECT,
	PHASE_LOADING,
	PHASE_GAME,
	PHASE_DEAD,
	PHASE_DBCLIENT_CONNECTING,
	PHASE_DBCLIENT,
	PHASE_P2P,
	PHASE_AUTH,
};

typedef struct packet_phase
{
	BYTE header;
	BYTE phase;
} TPacketGCPhase;

typedef struct packet_blank
{
	BYTE header;
} TPacketGCBlank;

typedef struct packet_blank_dynamic
{
	BYTE header;
	WORD size;
} TPacketGCBlankDynamic;

typedef struct packet_header_handshake
{
	BYTE header;
	DWORD dwHandshake;
	DWORD dwTime;
	LONG lDelta;
} TPacketGCHandshake;

typedef struct packet_header_bindudp
{
	BYTE header;
	DWORD addr;
	WORD port;
} TPacketGCBindUDP;

typedef struct packet_header_dynamic_size
{
	BYTE header;
	WORD size;
} TDynamicSizePacketHeader;

#ifdef ENABLE_LARGE_DYNAMIC_PACKET
typedef struct packet_header_large_dynamic_size
{
	BYTE header;
	int size;
} TLargeDynamicSizePacketHeader;
#endif

typedef struct SSimplePlayerInformation
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
	long x, y;
	LONG lAddr;
	WORD wPort;
	BYTE bySkillGroup;
	DWORD dwLastPlay;
} TSimplePlayerInformation;

typedef struct packet_login_success3
{
	BYTE header;
	TSimplePlayerInformation akSimplePlayerInformation[PLAYER_PER_ACCOUNT3];
	DWORD guild_id[PLAYER_PER_ACCOUNT3];
	char guild_name[PLAYER_PER_ACCOUNT3][GUILD_NAME_MAX_LEN+1];
	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess3;

typedef struct packet_login_success4
{
	BYTE header;
	TSimplePlayerInformation akSimplePlayerInformation[PLAYER_PER_ACCOUNT4];
	DWORD guild_id[PLAYER_PER_ACCOUNT4];
	char guild_name[PLAYER_PER_ACCOUNT4][GUILD_NAME_MAX_LEN+1];
	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess4;

typedef struct packet_login_success5
{
	BYTE header;
	TSimplePlayerInformation akSimplePlayerInformation[PLAYER_PER_ACCOUNT5];
	DWORD guild_id[PLAYER_PER_ACCOUNT5];
	char guild_name[PLAYER_PER_ACCOUNT5][GUILD_NAME_MAX_LEN + 1];
	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess5;

enum { LOGIN_STATUS_MAX_LEN = 8 };
typedef struct packet_login_failure
{
	BYTE header;
	char szStatus[LOGIN_STATUS_MAX_LEN + 1];
} TPacketGCLoginFailure;

typedef struct command_player_create
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	WORD job;
	BYTE shape;
	BYTE CON;
	BYTE INT;
	BYTE STR;
	BYTE DEX;
} TPacketCGCreateCharacter;

typedef struct command_player_create_success
{
	BYTE header;
	BYTE bAccountCharacterSlot;
	TSimplePlayerInformation kSimplePlayerInfomation;
} TPacketGCPlayerCreateSuccess;

typedef struct command_create_failure
{
	BYTE header;
	BYTE bType;
} TPacketGCCreateFailure;

typedef struct command_player_delete
{
	BYTE header;
	BYTE index;
	char szPrivateCode[PRIVATE_CODE_LENGTH];
} TPacketCGDestroyCharacter;

typedef struct packet_player_delete_success
{
	BYTE header;
	BYTE account_index;
} TPacketGCDestroyCharacterSuccess;

enum
{
	ADD_CHARACTER_STATE_DEAD = (1 << 0),
	ADD_CHARACTER_STATE_SPAWN = (1 << 1),
	ADD_CHARACTER_STATE_GUNGON = (1 << 2),
	ADD_CHARACTER_STATE_KILLER = (1 << 3),
	ADD_CHARACTER_STATE_PARTY = (1 << 4),
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM,
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
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	CHR_EQUIPPART_AURA,
#endif

	CHR_EQUIPPART_NUM,
};

typedef struct packet_char_additional_info
{
	BYTE header;
	DWORD dwVID;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	WORD awPart[CHR_EQUIPPART_NUM];
	BYTE bEmpire;
	DWORD dwGuildID;
	DWORD dwLevel;
	short sAlignment;
#ifdef ENABLE_CONQUEROR_LEVEL
	DWORD dwConquerorLevel;
#endif
	BYTE bPKMode;
	DWORD dwMountVnum;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
	BYTE bLanguage2;
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	DWORD dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	BYTE dwGuildLeader;
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	BYTE bCharacterSize;
#endif
} TPacketGCCharacterAdditionalInfo;

typedef struct packet_add_char
{
	BYTE header;
	DWORD dwVID;

#ifdef ENABLE_SHOW_MOB_INFO
	DWORD dwLevel;
	DWORD dwAIFlag;
#endif

	float angle;
	long x;
	long y;
	long z;

	BYTE bType;
	WORD wRaceNum;
	BYTE bMovingSpeed;
	BYTE bAttackSpeed;

	BYTE bStateFlag;
	DWORD dwAffectFlag[2];
} TPacketGCCharacterAdd;

typedef struct packet_add_char2
{
	BYTE header;

	DWORD dwVID;

	char name[CHARACTER_NAME_MAX_LEN + 1];

	float angle;
	long x;
	long y;
	long z;

	BYTE bType;
	WORD wRaceNum;
	WORD awPart[CHR_EQUIPPART_NUM];
	BYTE bMovingSpeed;
	BYTE bAttackSpeed;

	BYTE bStateFlag;
	DWORD dwAffectFlag[2];
	BYTE bEmpire;

	DWORD dwGuild;
	short sAlignment;
	BYTE bPKMode;
	DWORD dwMountVnum;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	BYTE dwGuildLeader;
#endif
} TPacketGCCharacterAdd2;

typedef struct packet_update_char
{
	BYTE header;
	DWORD dwVID;

	WORD awPart[CHR_EQUIPPART_NUM];
	BYTE bMovingSpeed;
	BYTE bAttackSpeed;

	BYTE bStateFlag;
	DWORD dwAffectFlag[2];

	DWORD dwGuildID;
	short sAlignment;
#ifdef ENABLE_CONQUEROR_LEVEL
	DWORD dwConquerorLevel;
#endif
	BYTE bPKMode;
	DWORD dwMountVnum;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
	BYTE bLanguage2;
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	DWORD dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	BYTE dwGuildLeader;
#endif
} TPacketGCCharacterUpdate;

typedef struct packet_update_char2
{
	BYTE header;
	DWORD dwVID;

	WORD awPart[CHR_EQUIPPART_NUM];
	BYTE bMovingSpeed;
	BYTE bAttackSpeed;

	BYTE bStateFlag;
	DWORD dwAffectFlag[2];

	DWORD dwGuildID;
	short sAlignment;
	BYTE bPKMode;
	DWORD dwMountVnum;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
	BYTE bLanguage2;
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	DWORD dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	BYTE dwGuildLeader;
#endif
} TPacketGCCharacterUpdate2;

typedef struct packet_del_char
{
	BYTE header;
	DWORD dwVID;
} TPacketGCCharacterDelete;

typedef struct packet_GlobalTime
{
	BYTE header;
	float GlobalTime;
} TPacketGCGlobalTime;

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
	CHAT_TYPE_MAX_NUM,
};

typedef struct packet_chatting
{
	BYTE header;
	WORD size;
	BYTE type;
	DWORD dwVID;
	BYTE bEmpire;
} TPacketGCChat;

typedef struct packet_whisper
{
	BYTE bHeader;
	WORD wSize;
	BYTE bType;
	char szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCWhisper;

typedef struct packet_stun
{
	BYTE header;
	DWORD vid;
} TPacketGCStun;

#ifdef ENABLE_RENEWAL_DEAD_PACKET
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
#ifdef ENABLE_RENEWAL_DEAD_PACKET
	packet_dead() {	memset(&t_d, 0, sizeof(t_d)); }
#endif
	BYTE header;
	DWORD vid;
#ifdef ENABLE_RENEWAL_DEAD_PACKET
	BYTE t_d[REVIVE_TYPE_MAX];
#endif
} TPacketGCDead;

#ifdef ENABLE_PARTY_POSITION
struct TPartyPosition
{
	DWORD dwPID;
	long lX;
	long lY;
	float fRot;
};

typedef struct SPacketGCPartyPosition
{
	BYTE bHeader;
	WORD wSize;
} TPacketGCPartyPosition;
#endif

#ifdef ENABLE_GUILD_RANK_SYSTEM
enum GUILD_RANK
{
	SUBHEADER_GUILD_RANKING_SEND,
	SUBHEADER_GUILD_RANKING_OPEN,
	HEADER_GC_GUILD_RANKING = 161,
};

typedef struct packet_guild_ranking_send
{
	BYTE header;
	BYTE subheader;
	int id;
	char szGuildName[GUILD_NAME_MAX_LEN + 1];
	char szOwnerName[CHARACTER_NAME_MAX_LEN + 1];
	int level;
	int point;
	int win;
	int draw;
	int loss;
} TPacketGCGuildRankingSend;
#endif

typedef struct packet_main_character
{
	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lX, lY, lZ;
	BYTE byEmpire;
	BYTE bySkillGroup;
} TPacketGCMainCharacter;

typedef struct packet_main_character2_empire
{
	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lX, lY, lZ;
	BYTE byEmpire;
	BYTE bySkillGroup;
} TPacketGCMainCharacter2_EMPIRE;

typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_MAX_LEN = 24,
	};

	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szUserName[CHARACTER_NAME_MAX_LEN + 1];
	char szBGMName[MUSIC_NAME_MAX_LEN + 1];
	long lX, lY, lZ;
	BYTE byEmpire;
	BYTE bySkillGroup;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_MAX_LEN = 24,
	};

	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szUserName[CHARACTER_NAME_MAX_LEN + 1];
	char szBGMName[MUSIC_NAME_MAX_LEN + 1];
	float fBGMVol;
	long lX, lY, lZ;
	BYTE byEmpire;
	BYTE bySkillGroup;
} TPacketGCMainCharacter4_BGM_VOL;

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
	// Must match server's EPointTypes numeric values (char.h)
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
	POINT_MIN_ATK,					// 29
	POINT_MAX_ATK,					// 30
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
	POINT_ATTBONUS_MONSTER,			// 53
	POINT_ATTBONUS_WARRIOR,			// 54
	POINT_ATTBONUS_ASSASSIN,		// 55
	POINT_ATTBONUS_SURA,			// 56
	POINT_ATTBONUS_SHAMAN,			// 57
	POINT_ATTBONUS_TREE,			// 58
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
	POINT_POTION_BONUS,		 		// 86
	POINT_KILL_HP_RECOVER,			// 87
	POINT_IMMUNE_STUN,				// 88
	POINT_IMMUNE_SLOW,				// 89
	POINT_IMMUNE_FALL,				// 90
	POINT_PARTY_ATT_GRADE,			// 91
	POINT_PARTY_DEF_GRADE,			// 92
	POINT_ATT_BONUS,				// 93
	POINT_DEF_BONUS,				// 94
	POINT_ATT_GRADE_BONUS,			// 95
	POINT_DEF_GRADE_BONUS,			// 96
	POINT_MAGIC_ATT_GRADE_BONUS,	// 97
	POINT_MAGIC_DEF_GRADE_BONUS,	// 98
	POINT_RESIST_NORMAL_DAMAGE,		// 99
	POINT_STAT_RESET_COUNT 			= 112,
	POINT_HORSE_SKILL 				= 113,
	POINT_MALL_ATTBONUS,
	POINT_MALL_DEFBONUS,
	POINT_MALL_EXPBONUS,
	POINT_MALL_ITEMBONUS,
	POINT_MALL_GOLDBONUS,
	POINT_MAX_HP_PCT,
	POINT_MAX_SP_PCT,
	POINT_SKILL_DAMAGE_BONUS,
	POINT_NORMAL_HIT_DAMAGE_BONUS,
	POINT_SKILL_DEFEND_BONUS,
	POINT_NORMAL_HIT_DEFEND_BONUS,
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

typedef struct packet_points
{
	BYTE header;
#ifdef ENABLE_GOLD_LIMIT
	long long points[POINT_MAX_NUM];
#else
	long points[POINT_MAX_NUM];
#endif
} TPacketGCPoints;

typedef struct packet_point_change
{
	int header;

	DWORD dwVID;
	BYTE Type;

#ifdef ENABLE_GOLD_LIMIT
	long long amount;
	long long value;
#else
	long amount;
	long value;
#endif
} TPacketGCPointChange;

typedef struct packet_motion
{
	BYTE header;
	DWORD vid;
	DWORD victim_vid;
	WORD motion;
} TPacketGCMotion;

typedef struct packet_set_item
{
	BYTE header;
	TItemPos Cell;
	DWORD vnum;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_SET_ITEM
	DWORD dwSetValue;
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
	long sealbind;
#endif
} TPacketGCItemSet;

typedef struct packet_set_item2
{
	BYTE header;
	TItemPos Cell;
	DWORD vnum;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
	DWORD flags;
	DWORD anti_flags;
	bool highlight;
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_SET_ITEM
	DWORD dwSetValue;
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
	long sealbind;
#endif
} TPacketGCItemSet2;

typedef struct packet_item_del
{
	BYTE header;
#ifdef ENABLE_EXTENDED_SAFEBOX
	DWORD pos;
#else
	BYTE pos;
#endif
} TPacketGCItemDel;

typedef struct packet_use_item
{
	BYTE header;
	TItemPos Cell;
	DWORD ch_vid;
	DWORD victim_vid;

	DWORD vnum;
} TPacketGCItemUse;

typedef struct packet_update_item
{
	BYTE header;
	TItemPos Cell;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_SET_ITEM
	DWORD dwSetValue;
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
	long sealbind;
#endif
} TPacketGCItemUpdate;

typedef struct packet_ground_add_item
{
	BYTE bHeader;
	long lX;
	long lY;
	long lZ;

#ifdef ENABLE_RENEWAL_BOOK_NAME
	long lSocket0;
#endif

	DWORD dwVID;
	DWORD dwVnum;
} TPacketGCItemGroundAdd;

typedef struct packet_ground_del_item
{
	BYTE header;
	DWORD vid;
} TPacketGCItemGroundDel;

typedef struct packet_item_ownership
{
	BYTE bHeader;
	DWORD dwVID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

typedef struct packet_quickslot_add
{
	BYTE header;
	BYTE pos;
	TQuickSlot slot;
} TPacketGCQuickSlotAdd;

typedef struct packet_quickslot_del
{
	BYTE header;
	BYTE pos;
} TPacketGCQuickSlotDel;

typedef struct packet_quickslot_swap
{
	BYTE header;
	BYTE pos;
	BYTE change_pos;
} TPacketGCQuickSlotSwap;

typedef struct packet_shop_start
{
	struct packet_shop_item items[SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCShopStart;

typedef struct packet_shop_start_ex
{
	typedef struct sub_packet_shop_tab
	{
		char name[SHOP_TAB_NAME_MAX];
		BYTE coin_type;
		packet_shop_item items[SHOP_HOST_ITEM_MAX_NUM];
	} TSubPacketShopTab;

	DWORD owner_vid;
	BYTE shop_tab_count;
} TPacketGCShopStartEx;

typedef struct packet_shop_update_item
{
	BYTE pos;
	struct packet_shop_item item;
} TPacketGCShopUpdateItem;

typedef struct packet_shop_update_price
{
#ifdef ENABLE_GOLD_LIMIT
	long long iElkAmount;
#else
	int iElkAmount;
#endif
} TPacketGCShopUpdatePrice;

enum EPacketShopSubHeaders
{
	SHOP_SUBHEADER_GC_START,
	SHOP_SUBHEADER_GC_END,
	SHOP_SUBHEADER_GC_UPDATE_ITEM,
	SHOP_SUBHEADER_GC_UPDATE_PRICE,
	SHOP_SUBHEADER_GC_OK,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY,
	SHOP_SUBHEADER_GC_SOLDOUT,
	SHOP_SUBHEADER_GC_INVENTORY_FULL,
	SHOP_SUBHEADER_GC_INVALID_POS,
	SHOP_SUBHEADER_GC_SOLD_OUT,
	SHOP_SUBHEADER_GC_START_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_COINS,
#ifdef ENABLE_RENEWAL_SHOPEX
	SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_EXP,
	SHOP_SUBHEADER_GC_YOU_ALREADY_HAVE_ONE,
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	SHOP_SUBHEADER_GC_REFRESH_MONEY,
	SHOP_SUBHEADER_GC_CHECK_RESULT,
	SHOP_SUBHEADER_GC_REFRESH_DISPLAY_COUNT,
	SHOP_SUBHEADER_GC_REALWATCHER_COUNT,
	SHOP_SUBHEADER_GC_REFRESH_COUNT,
	SHOP_SUBHEADER_GC_CHANGE_TITLE,
#endif
#ifdef ENABLE_ZODIAC_MISSION
	SHOP_SUBHEADER_GC_NOT_ENOUGH_LUCKY,
#endif
#ifdef ENABLE_SHOP_REPURCHASE
	SHOP_SUBHEADER_GC_REPURCHASE_OPEN = 24,
	SHOP_SUBHEADER_GC_REPURCHASE_CLOSE = 25,
	SHOP_SUBHEADER_GC_REPURCHASE_ADD_ITEM = 26,
	SHOP_SUBHEADER_GC_REPURCHASE_RESULT = 27,
	SHOP_SUBHEADER_GC_REPURCHASE_ITEM_LIST = 28,
#endif
};

typedef struct packet_shop
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCShop;

#ifdef ENABLE_SHOP_REPURCHASE
#pragma pack(push, 1)
typedef struct packet_shop_repurchase_item_extra
{
	DWORD dwID;
	DWORD dwPrice;
	__int64 tExpireTime;
} TSubPacketGCShopRepurchaseItem;

typedef struct packet_shop_repurchase_result
{
	BYTE bResult;
	WORD wPos;
} TPacketGCShopRepurchaseResult;
#pragma pack(pop)
#endif

typedef struct packet_exchange
{
	BYTE header;
	BYTE subheader;
	BYTE is_me;
#ifdef ENABLE_GOLD_LIMIT
	long long arg1;
#else
	DWORD arg1;
#endif
	TItemPos arg2;
	DWORD arg3;
#ifdef ENABLE_SLOT_MARKING_SYSTEM
	TItemPos arg4;
#endif
	long alValues[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_SET_ITEM
	DWORD dwSetValue;
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
	long sealbind;
#endif
} TPacketGCExchange;

enum
{
	EXCHANGE_SUBHEADER_GC_START,
	EXCHANGE_SUBHEADER_GC_ITEM_ADD,
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,
	EXCHANGE_SUBHEADER_GC_ELK_ADD,
	EXCHANGE_SUBHEADER_GC_ACCEPT,
	EXCHANGE_SUBHEADER_GC_END,
	EXCHANGE_SUBHEADER_GC_ALREADY,
	EXCHANGE_SUBHEADER_GC_LESS_ELK,
};

typedef struct packet_position
{
	BYTE header;
	DWORD vid;
	BYTE position;
} TPacketGCPosition;

typedef struct packet_ping
{
	BYTE header;
} TPacketGCPing;

typedef struct packet_pong
{
	BYTE bHeader;
} TPacketCGPong;

typedef struct packet_script
{
	BYTE header;
	WORD size;
	BYTE skin;
	WORD src_size;
} TPacketGCScript;

typedef struct packet_target
{
	BYTE header;
	DWORD dwVID;
	BYTE bHPPercent;
#ifdef ENABLE_VIEW_TARGET_DECIMAL_HP
	int iMinHP;
	int iMaxHP;
#endif
} TPacketGCTarget;

#ifdef ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX
typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVictimVID;
	DWORD dwAttackerVID;
	BYTE flag;
	int damage;
} TPacketGCDamageInfo;
#else
typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int damage;
} TPacketGCDamageInfo;
#endif

typedef struct packet_mount
{
	BYTE header;
	DWORD vid;
	DWORD mount_vid;
	BYTE pos;
	DWORD _x, _y;
} TPacketGCMount;

typedef struct packet_change_speed
{
	BYTE header;
	DWORD vid;
	WORD moving_speed;
} TPacketGCChangeSpeed;

typedef struct packet_move
{
	BYTE bHeader;
	BYTE bFunc;
	BYTE bArg;
	BYTE bRot;
	DWORD dwVID;
	LONG lX;
	LONG lY;
	DWORD dwTime;
	DWORD dwDuration;
} TPacketGCMove;

enum
{
	QUEST_SEND_IS_BEGIN = 1 << 0,
	QUEST_SEND_TITLE = 1 << 1,
	QUEST_SEND_CLOCK_NAME = 1 << 2,
	QUEST_SEND_CLOCK_VALUE = 1 << 3,
	QUEST_SEND_COUNTER_NAME = 1 << 4,
	QUEST_SEND_COUNTER_VALUE = 1 << 5,
	QUEST_SEND_ICON_FILE = 1 << 6,
};

typedef struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
#ifdef ENABLE_RENEWAL_QUEST
	WORD c_index;
#endif
	BYTE flag;
} TPacketGCQuestInfo;

typedef struct packet_quest_confirm
{
	BYTE header;
	char msg[64+1];
	long timeout;
	DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_attack
{
	BYTE header;
	DWORD dwVID;
	DWORD dwVictimVID;
	BYTE bType;
} TPacketGCAttack;

typedef struct packet_c2c
{
	BYTE header;
	WORD wSize;
} TPacketGCC2C;

typedef struct packetd_sync_position_element
{
	DWORD dwVID;
	long lX;
	long lY;
} TPacketGCSyncPositionElement;

typedef struct packetd_sync_position
{
	BYTE bHeader;
	WORD wSize;
} TPacketGCSyncPosition;

typedef struct packet_ownership
{
	BYTE bHeader;
	DWORD dwOwnerVID;
	DWORD dwVictimVID;
} TPacketGCOwnership;

#define	SKILL_MAX_NUM 255

typedef struct packet_skill_level
{
	BYTE bHeader;
	BYTE abSkillLevels[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

typedef struct SPlayerSkill
{
	BYTE bMasterType;
	BYTE bLevel;
	uint32_t tNextRead;
} TPlayerSkill;

typedef struct packet_skill_level_new
{
	BYTE bHeader;
	TPlayerSkill skills[SKILL_MAX_NUM];
} TPacketGCSkillLevelNew;

typedef struct packet_fly
{
	BYTE bHeader;
	BYTE bType;
	DWORD dwStartVID;
	DWORD dwEndVID;
} TPacketGCCreateFly;

enum EPVPModes
{
	PVP_MODE_NONE,
	PVP_MODE_AGREE,
	PVP_MODE_FIGHT,
	PVP_MODE_REVENGE,
};

typedef struct packet_duel_start
{
	BYTE header ;
	WORD wSize ;
} TPacketGCDuelStart ;

typedef struct packet_pvp
{
	BYTE header;
	DWORD dwVIDSrc;
	DWORD dwVIDDst;
	BYTE bMode;
} TPacketGCPVP;

typedef struct packet_skill_cooltime_end
{
	BYTE header;
	BYTE bSkill;
} TPacketGCSkillCoolTimeEnd;

typedef struct packet_warp
{
	BYTE bHeader;
	LONG lX;
	LONG lY;
	LONG lAddr;
	WORD wPort;
} TPacketGCWarp;

typedef struct packet_party_invite
{
	BYTE header;
	DWORD leader_pid;
} TPacketGCPartyInvite;

typedef struct packet_party_add
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCPartyAdd;

typedef struct packet_party_update
{
	BYTE header;
	DWORD pid;
	BYTE state;
	BYTE percent_hp;
	short affects[PARTY_AFFECT_SLOT_MAX_NUM];
} TPacketGCPartyUpdate;

typedef struct packet_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketGCPartyRemove;

typedef TPacketCGSafeboxCheckout TPacketGCSafeboxCheckout;
typedef TPacketCGSafeboxCheckin TPacketGCSafeboxCheckin;

typedef struct packet_safebox_wrong_password
{
	BYTE bHeader;
} TPacketGCSafeboxWrongPassword;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketGCSafeboxSize;

typedef struct packet_safebox_money_change
{
	BYTE bHeader;
	DWORD dwMoney;
} TPacketGCSafeboxMoneyChange;

#ifdef ENABLE_UN_SEAL_SCROLL_PLUS
typedef struct packet_safebox_password_success
{
	BYTE bHeader;
	WORD wSize;
	BYTE bType;
} TPacketGCSafeboxPasswordSuccess;
#endif

typedef struct command_empire
{
	BYTE bHeader;
	BYTE bEmpire;
} TPacketCGEmpire;

typedef struct packet_empire
{
	BYTE bHeader;
	BYTE bEmpire;
} TPacketGCEmpire;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
	FISHING_SUBHEADER_GC_FISH,
};

typedef struct packet_fishing
{
	BYTE header;
	BYTE subheader;
	DWORD info;
	BYTE dir;
} TPacketGCFishing;

typedef struct paryt_parameter
{
	BYTE bHeader;
	BYTE bDistributeMode;
} TPacketGCPartyParameter;

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
	GUILD_SUBHEADER_GC_WAR_POINT,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
};

typedef struct packet_guild
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCGuild;

enum
{
	GUILD_AUTH_ADD_MEMBER		= (1 << 0),
	GUILD_AUTH_REMOVE_MEMBER	= (1 << 1),
	GUILD_AUTH_NOTICE			= (1 << 2),
	GUILD_AUTH_SKILL			= (1 << 3),
#ifdef ENABLE_GUILDRENEWAL_SYSTEM
	GUILD_AUTH_WAR				= (1 << 4),
	GUILD_AUTH_BANK				= (1 << 5),
#endif
};

typedef struct packet_guild_sub_grade
{
	char grade_name[GUILD_GRADE_NAME_MAX_LEN+1];
	BYTE auth_flag;
} TPacketGCGuildSubGrade;

typedef struct packet_guild_sub_member
{
	DWORD pid;
	BYTE byGrade;
	BYTE byIsGeneral;
	BYTE byJob;
#ifdef ENABLE_LEVEL_INT
	int byLevel;
#else
	BYTE byLevel;
#endif
	DWORD dwOffer;
	BYTE byNameFlag;
} TPacketGCGuildSubMember;

typedef struct packet_guild_sub_info
{
	WORD member_count;
	WORD max_member_count;
	DWORD guild_id;
	DWORD master_pid;
	DWORD exp;
	BYTE level;
	char name[GUILD_NAME_MAX_LEN+1];
	DWORD gold;
	BYTE hasLand;
} TPacketGCGuildInfo;

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

	GUILD_WAR_DURATION = 2*60*60,
};

typedef struct packet_guild_war
{
	DWORD dwGuildSelf;
	DWORD dwGuildOpp;
	BYTE bType;
	BYTE bWarState;
} TPacketGCGuildWar;

typedef struct SPacketGuildWarPoint
{
	DWORD dwGainGuildID;
	DWORD dwOpponentGuildID;
	long lPoint;
} TPacketGuildWarPoint;

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

#ifdef ENABLE_RENEWAL_OFFLINESHOP
typedef struct packet_offline_shop_item
{
	DWORD id;
	DWORD vnum;
	long long price;
	int count;
	BYTE display_pos;
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
	BYTE status;
	char szBuyerName[CHARACTER_NAME_MAX_LEN+1];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD transmutation;
#endif
	DWORD owner_id;
} TOfflineShopItemData;

typedef struct ShopLog
{
	char name[CHARACTER_NAME_MAX_LEN + 1];
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

typedef struct SPacketGCShopSign
{
	BYTE bHeader;
	DWORD dwVID;
	char szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;

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
	TItemPos bDisplayPos;
	BYTE bPos;
	long long lPrice;
} TOfflineShopAddItem;

typedef struct SPacketCGMyOfflineShop
{
	BYTE bHeader;
	char szSign[SHOP_SIGN_MAX_LEN + 1];
	BYTE bCount;
	DWORD shopVnum;
	BYTE shopTitle;
} TPacketCGMyOfflineShop;

typedef struct packet_offline_shop_start
{
	DWORD owner_vid;
	TOfflineShopItemData items[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
	DWORD m_dwDisplayedCount;
	char title[SHOP_SIGN_MAX_LEN + 1];
	long long price;
	DWORD m_dwRealWatcherCount;
	ShopLog log[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
	unsigned long long flag;
	int time;
	DWORD type;
	bool IsOwner;
} TPacketGCOfflineShopStart;

typedef struct packet_offline_shop_update_item
{
	BYTE pos;
	TOfflineShopItemData item;
	DWORD m_dwDisplayedCount;
	DWORD m_dwRealWatcherCount;
	char title[SHOP_SIGN_MAX_LEN + 1];
	long long price;
	ShopLog log;
	unsigned long long flag;
	int time;
	DWORD type;
} TPacketGCOfflineShopUpdateItem;
#else
typedef struct SPacketGCShopSign
{
	BYTE bHeader;
	DWORD dwVID;
	char szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
typedef struct packet_priv_shop_item_set
{
	BYTE header;
	DWORD size;
} TPacketGCShopSearchItemSet;

typedef struct command_shop_search
{
	BYTE header;
	int ItemCat;
	int SubCat;
	char searchInput[50];
	BYTE avg[2];
	BYTE skill[2];
	BYTE abs[2];
	BYTE level[2];
	BYTE refine[2];
	BYTE sex;
	BYTE enchant;
	BYTE alchemy;
	BYTE character;
	long long price[2];
} TPacketCGShopSearch;
#endif

typedef struct SPacketGCTime
{
	BYTE bHeader;
	uint32_t time;
} TPacketGCTime;

enum
{
	WALKMODE_RUN,
	WALKMODE_WALK,
};

typedef struct SPacketGCWalkMode
{
	BYTE header;
	DWORD vid;
	BYTE mode;
} TPacketGCWalkMode;

typedef struct SPacketGCChangeSkillGroup
{
	BYTE header;
	BYTE skill_group;
} TPacketGCChangeSkillGroup;

struct TMaterial
{
	DWORD vnum;
	DWORD count;
};

typedef struct SRefineTable
{
	DWORD src_vnum;
	DWORD result_vnum;
#ifdef ENABLE_STACK_LIMIT
	WORD material_count;
#else
	BYTE material_count;
#endif
	int cost;
	int prob;
	TMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TRefineTable;

typedef struct SPacketGCRefineInformation
{
	BYTE header;
	BYTE pos;
	TRefineTable refine_table;
} TPacketGCRefineInformation;

typedef struct SPacketGCRefineInformationNew
{
	BYTE header;
	BYTE type;
	BYTE pos;
	TRefineTable refine_table;
} TPacketGCRefineInformationNew;

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
};

typedef struct SPacketGCSpecialEffect
{
	BYTE header;
	BYTE type;
	DWORD vid;
} TPacketGCSpecialEffect;

typedef struct SPacketGCNPCPosition
{
	BYTE header;
	WORD size;
	WORD count;
} TPacketGCNPCPosition;

struct TNPCPosition
{
	BYTE bType;
#ifdef ENABLE_ULTIMATE_REGEN
	DWORD mobVnum;
#else
	char name[CHARACTER_NAME_MAX_LEN+1];
#endif
	long x;
	long y;
#ifdef ENABLE_ULTIMATE_REGEN
	int regenTime;
#endif
};

typedef struct SPacketGCChangeName
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCChangeName;

enum EBlockAction
{
	BLOCK_EXCHANGE = (1 << 0),
	BLOCK_PARTY_INVITE = (1 << 1),
	BLOCK_GUILD_INVITE = (1 << 2),
	BLOCK_WHISPER = (1 << 3),
	BLOCK_MESSENGER_INVITE = (1 << 4),
	BLOCK_PARTY_REQUEST = (1 << 5),
#ifdef ENABLE_TELEPORT_TO_A_FRIEND
	BLOCK_WARP_REQUEST = (1 << 6),
#endif
};

typedef struct packet_login_key
{
	BYTE bHeader;
	DWORD dwLoginKey;
} TPacketGCLoginKey;

typedef struct packet_auth_success
{
	BYTE bHeader;
	DWORD dwLoginKey;
	BYTE bResult;
} TPacketGCAuthSuccess;

typedef struct packet_channel
{
	BYTE header;
	BYTE channel;
#ifdef ENABLE_ANTI_EXP
	bool anti_exp;
#endif
} TPacketGCChannel;

typedef struct SEquipmentItemSet
{
	DWORD vnum;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_SUNGMA_SCORE
	DWORD dwSungmaScore;
#endif
#ifdef ENABLE_SET_ITEM
	DWORD dwSetValue;
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
	long sealbind;
#endif
} TEquipmentItemSet;

typedef struct pakcet_view_equip
{
	BYTE header;
	DWORD dwVID;
	TEquipmentItemSet equips[WEAR_MAX_NUM];
} TPacketGCViewEquip;

typedef struct packet_land_element
{
	DWORD dwID;
	long x, y;
	long width, height;
	DWORD dwGuildID;
} TLandPacketElement;

typedef struct packet_land_list
{
	BYTE header;
	WORD size;
} TPacketGCLandList;

typedef struct packet_target_create
{
	BYTE bHeader;
	long lID;
	char szTargetName[32+1];
} TPacketGCTargetCreate;

enum
{
	CREATE_TARGET_TYPE_NONE,
	CREATE_TARGET_TYPE_LOCATION,
	CREATE_TARGET_TYPE_CHARACTER,
};

typedef struct
{
	BYTE bHeader;
	long lID;
	char szTargetName[32+1];
	DWORD dwVID;
	BYTE byType;
} TPacketGCTargetCreateNew;

typedef struct packet_target_update
{
	BYTE bHeader;
	long lID;
	long lX, lY;
} TPacketGCTargetUpdate;

typedef struct packet_target_delete
{
	BYTE bHeader;
	long lID;
} TPacketGCTargetDelete;

typedef struct packet_effect_element
{
	DWORD dwType;
	BYTE bPointIdxApplyOn;
	long lApplyValue;
	DWORD dwFlag;
	long lDuration;
	long lSPCost;
} TPacketAffectElement;

typedef struct packet_affect_add
{
	BYTE bHeader;
	TPacketAffectElement elem;
} TPacketGCAffectAdd;

typedef struct packet_affect_remove
{
	BYTE bHeader;
	DWORD dwType;
	BYTE bApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_mall_open
{
	BYTE bHeader;
	BYTE bSize;
} TPacketGCMallOpen;

#ifdef ENABLE_CONQUEROR_LEVEL
typedef struct packet_sungma_attr_update
{
	BYTE bHeader;
	BYTE bSTR;
	BYTE bHP;
	BYTE bMOVE;
	BYTE bIMMUNE;
} TPacketGCSungmaAttrUpdate;
#endif

typedef struct packet_lover_info
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE byLovePoint;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE bHeader;
	BYTE byLovePoint;
} TPacketGCLovePointUpdate;

typedef struct packet_dig_motion
{
	BYTE header;
	DWORD vid;
	DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;

typedef struct SPacketGCOnTime
{
	BYTE header;
	int ontime;
} TPacketGCOnTime;

typedef struct SPacketGCResetOnTime
{
	BYTE header;
} TPacketGCResetOnTime;

#ifdef ENABLE_FISH_EVENT_SYSTEM
typedef struct SPacketGCHybridSDB
{
private:
	SPacketGCHybridSDB() : m_pStream(NULL) {}

public:
	SPacketGCHybridSDB(int iStreamSize) : iSDBStreamLen(iStreamSize)
	{
		m_pStream = new BYTE[iStreamSize];
	}
	~SPacketGCHybridSDB()
	{
		delete[] m_pStream;
		m_pStream = NULL;
	}
	static int GetFixedHeaderSize()
	{
		return sizeof(BYTE)+sizeof(WORD)+sizeof(int);
	}

	BYTE	bHeader;
	WORD    wDynamicPacketSize;
	int		iSDBStreamLen;
	BYTE*	m_pStream;

} TPacketGCHybridSDB;
#endif

typedef struct packet_state
{
	BYTE bHeader;
	BYTE bFunc;
	BYTE bArg;
	BYTE bRot;
	DWORD dwVID;
	DWORD dwTime;
	TPixelPosition kPPos;
} TPacketCCState;

typedef struct packet_autoban_quiz
{
	BYTE bHeader;
	BYTE bDuration;
	BYTE bCaptcha[64*32];
	char szQuiz[256];
} TPacketGCAutoBanQuiz;

#ifdef ENABLE_BUFFI_SYSTEM
typedef struct SPacketGCBuffiUseSkill
{
	BYTE	bHeader;
	DWORD	dwVID;
	DWORD	dwSkillVnum;
	BYTE	bSkillLevel;
} TPacketGCBuffiUseSkill;
#endif

typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[128];
} TPacketGCSpecificEffect;

#ifdef ENABLE_FISH_EVENT_SYSTEM
enum
{
	//HEADER_CG_FISH_EVENT_SEND = 36,
	//HEADER_GC_FISH_EVENT_INFO = 50,
};

enum
{
	FISH_EVENT_SUBHEADER_BOX_USE,
	FISH_EVENT_SUBHEADER_SHAPE_ADD,
	FISH_EVENT_SUBHEADER_GC_REWARD,
	FISH_EVENT_SUBHEADER_GC_ENABLE,
};

typedef struct SPacketGCFishEvent {
	BYTE	bHeader;
	BYTE	bSubheader;
} TPacketCGFishEvent;

typedef struct SPacketGCFishEventInfo {
	BYTE	bHeader;
	BYTE	bSubheader;
	DWORD	dwFirstArg;
	DWORD	dwSecondArg;
} TPacketGCFishEventInfo;
#endif

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
	DS_SUB_HEADER_DO_UPGRADE,
	DS_SUB_HEADER_DO_IMPROVEMENT,
	DS_SUB_HEADER_DO_REFINE,
	DS_SUB_HEADER_REFINE_FAIL,
	DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE,
	DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL,
	DS_SUB_HEADER_REFINE_SUCCEED,
};

typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header (HEADER_CG_DRAGON_SOUL_REFINE) {}

	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DS_REFINE_WINDOW_MAX_NUM];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE) {}

	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;

typedef struct SChannelStatus
{
	int nPort;
	BYTE bStatus;
} TChannelStatus;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
typedef struct SPacketChangeLanguage
{
	BYTE bHeader;
	BYTE bLanguage;
} TPacketChangeLanguage;
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
typedef struct SPacketCGWhisperDetails
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisperDetails;

typedef struct SPacketGCWhisperDetails
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
#endif
} TPacketGCWhisperDetails;
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
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
	BYTE slot;
	BYTE vnum;
	BYTE count;
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
};
#endif

#ifdef ENABLE_RENEWAL_CUBE
enum
{
	CUBE_RENEWAL_SUB_HEADER_OPEN_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_CLEAR_DATES_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_DATES_RECEIVE,
	CUBE_RENEWAL_SUB_HEADER_DATES_LOADING,

	CUBE_RENEWAL_SUB_HEADER_MAKE_ITEM,
	CUBE_RENEWAL_SUB_HEADER_CLOSE,
};

typedef struct packet_send_cube_renewal
{
	BYTE header;
	BYTE subheader;
	DWORD index_item;
	DWORD count_item;
	DWORD index_item_improve;
} TPacketCGCubeRenewalSend;

typedef struct dates_cube_renewal
{
	DWORD npc_vnum;
	DWORD index;

	DWORD vnum_reward;
	int count_reward;

	bool item_reward_stackable;

	DWORD vnum_material_1;
	int count_material_1;

	DWORD vnum_material_2;
	int count_material_2;

	DWORD vnum_material_3;
	int count_material_3;

	DWORD vnum_material_4;
	int count_material_4;

	DWORD vnum_material_5;
	int count_material_5;

	int gold;
#ifdef ENABLE_GAYA_SYSTEM
	int gem;
#endif
	int percent;

	bool allowCopyAttr;

	char category[100];
} TInfoDateCubeRenewal;

typedef struct packet_receive_cube_renewal
{
	packet_receive_cube_renewal(): header(HEADER_GC_CUBE_RENEWAL) {}

	BYTE header;
	BYTE subheader;
	TInfoDateCubeRenewal date_cube_renewal;
} TPacketGCCubeRenewalReceive;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
enum EAcceInfo
{
	ACCE_ABSORPTION_SOCKET = 0,
	ACCE_ABSORBED_SOCKET = 1,
	ACCE_CLEAN_ATTR_VALUE0 = 7,
	ACCE_WINDOW_MAX_MATERIALS = 2,
	ACCE_REVERSAL_VNUM_1 = 39046,
	ACCE_REVERSAL_VNUM_2 = 90000,
};

enum
{
	HEADER_CG_ACCE = 211,
	HEADER_GC_ACCE = 211,
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
	BYTE header;
	BYTE subheader;
	bool bWindow;
#ifdef ENABLE_GOLD_LIMIT
	long long dwPrice;
#else
	DWORD dwPrice;
#endif
	BYTE bPos;
	TItemPos tPos;
	DWORD dwItemVnum;
	DWORD dwMinAbs;
	DWORD dwMaxAbs;
} TPacketAcce;

typedef struct SAcceMaterial
{
	BYTE bHere;
	WORD wCell;
} TAcceMaterial;

typedef struct SAcceResult
{
	DWORD dwItemVnum;
	DWORD dwMinAbs;
	DWORD dwMaxAbs;
} TAcceResult;
#endif

#ifdef ENABLE_MOB_DROP_INFO
typedef struct packet_target_info
{
	BYTE header;
	DWORD dwVID;
	DWORD race;
	DWORD dwVnum;

#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif

} TPacketGCTargetInfo;

typedef struct packet_target_info_load
{
	BYTE header;
	DWORD dwVID;
} TPacketCGTargetInfoLoad;
#endif

#ifdef ENABLE_VIEW_CHEST_DROP
enum
{
	HEADER_CG_CHEST_DROP_INFO = 206,
	HEADER_GC_CHEST_DROP_INFO = 217,
};

typedef struct SPacketCGChestDropInfo
{
	BYTE header;
	WORD wInventoryCell;
} TPacketCGChestDropInfo;

typedef struct SChestDropInfoTable
{
	BYTE bPageIndex;
	BYTE bSlotIndex;
	DWORD dwItemVnum;

#ifdef ENABLE_STACK_LIMIT
	WORD bItemCount;
#else
	BYTE bItemCount;
#endif

} TChestDropInfoTable;

typedef struct SPacketGCChestDropInfo
{
	BYTE bHeader;
	WORD wSize;
	DWORD dwChestVnum;
} TPacketGCChestDropInfo;
#endif

#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
typedef struct pickup_item_packet
{
	BYTE bHeader;
	int item_vnum;
} TPacketGCPickupItemSC;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
typedef struct packet_skill_color
{
	BYTE bHeader;
	BYTE bSkillSlot;
	DWORD dwCol1;
	DWORD dwCol2;
	DWORD dwCol3;
	DWORD dwCol4;
	DWORD dwCol5;
} TPacketCGSkillColor;
#endif

#ifdef ENABLE_CLIENT_LOCALE_STRING
typedef struct packet_locale_chat
{
	BYTE header;
	WORD size;
	BYTE type;
	DWORD id;
} TPacketGCLocaleChat;

typedef struct packet_locale_whisper
{
	BYTE header;
	WORD size;
	BYTE type;
	DWORD id;
	char namefrom[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCLocaleWhisper;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
enum EPacketGCAuraSubHeader
{
	AURA_SUBHEADER_GC_OPEN,
	AURA_SUBHEADER_GC_CLOSE,
	AURA_SUBHEADER_GC_SET_ITEM,
	AURA_SUBHEADER_GC_CLEAR_SLOT,
	AURA_SUBHEADER_GC_CLEAR_ALL,
	AURA_SUBHEADER_GC_CLEAR_RIGHT,
	AURA_SUBHEADER_GC_REFINE_INFO,
};

typedef struct SSubPacketGCAuraOpenClose
{
	BYTE bAuraWindowType;
} TSubPacketGCAuraOpenClose;

typedef struct SSubPacketGCAuraSetItem
{
	TItemPos Cell;
	TItemPos AuraCell;
	TItemData pItem;
} TSubPacketGCAuraSetItem;

typedef struct SSubPacketGCAuraClearSlot
{
	BYTE bAuraSlotPos;
} TSubPacketGCAuraClearSlot;

typedef struct SSubPacketGCAuraRefineInfo
{
	BYTE bAuraRefineInfoType;
	BYTE bAuraRefineInfoLevel;
	BYTE bAuraRefineInfoExpPercent;
} TSubPacketGCAuraRefineInfo;

enum EPacketCGAuraSubHeader
{
	AURA_SUBHEADER_CG_REFINE_CHECKIN,
	AURA_SUBHEADER_CG_REFINE_CHECKOUT,
	AURA_SUBHEADER_CG_REFINE_ACCEPT,
	AURA_SUBHEADER_CG_REFINE_CANCEL,
};

typedef struct SSubPacketCGAuraRefineCheckIn
{
	TItemPos ItemCell;
	TItemPos AuraCell;
	BYTE byAuraRefineWindowType;
} TSubPacketCGAuraRefineCheckIn;

typedef struct SSubPacketCGAuraRefineCheckOut
{
	TItemPos AuraCell;
	BYTE byAuraRefineWindowType;
} TSubPacketCGAuraRefineCheckOut;

typedef struct SSubPacketCGAuraRefineAccept
{
	BYTE byAuraRefineWindowType;
} TSubPacketCGAuraRefineAccept;

typedef struct SPacketGCAura
{
	SPacketGCAura() : bHeader(HEADER_GC_AURA) {}
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCAura;

typedef struct SPacketCGAura
{
	SPacketCGAura() : bHeader(HEADER_CG_AURA) {}
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketCGAura;
#endif

#ifdef ENABLE_LEFT_SEAT_SYSTEM
enum ELeftSeatSubPacket
{
	LEFT_SEAT_SUB_OFF = 0,
	LEFT_SEAT_SUB_SET_TIME = 1,
};

#pragma pack(push, 1)
typedef struct SPacketCGLeftSeat
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSub;
} TPacketCGLeftSeat;

typedef struct SPacketCGLeftSeatSetTime
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSub;
	BYTE bWaitTimeIndex;
	BYTE bSubMode;
} TPacketCGLeftSeatSetTime;
#pragma pack(pop)
#endif // ENABLE_LEFT_SEAT_SYSTEM

#ifdef ENABLE_GROWTH_PET_SYSTEM
enum EGrowthPetPoints
{
	POINT_UPBRINGING_PET_LEVEL,
	POINT_UPBRINGING_PET_EXP,
	POINT_UPBRINGING_PET_ITEM_EXP,
	POINT_UPBRINGING_PET_NEXT_EXP,
	POINT_UPBRINGING_PET_EVOL_LEVEL,
	POINT_UPBRINGING_PET_HP,
	POINT_UPBRINGING_PET_DEF_GRADE,
	POINT_UPBRINGING_PET_SP,
	POINT_UPBRINGING_DURATION,
	POINT_UPBRINGING_MAX_DURATION,
	POINT_UPBRINGING_BIRTHDAY,

	POINT_UPBRINGING_MAX_NUM
};

enum EGrowthPetSubheader
{
	SUBHEADER_PET_EGG_USE_SUCCESS,
	SUBHEADER_PET_EGG_USE_FAILED_BECAUSE_NAME,
	SUBHEADER_PET_EGG_USE_FAILED_TIMEOVER,
	SUBHEADER_PET_UNSUMMON,
	SUBHEADER_PET_FEED_FAILED,
	SUBHEADER_PET_FEED_SUCCESS,
	SUBHEADER_PET_REVIVE_FAILED,
	SUBHEADER_PET_REVIVE_SUCCESS,
	SUBHEADER_PET_NAME_CHANGE_FAILED,
	SUBHEADER_PET_NAME_CHANGE_SUCCESS,
	SUBHEADER_PET_WINDOW_TYPE_INFO,
	SUBHEADER_PET_WINDOW_TYPE_ATTR_CHANGE,
	SUBHEADER_PET_WINDOW_TYPE_PREMIUM_FEED,
};

enum EGrowthPetWindow
{
	PET_WINDOW_HATCH,
	PET_WINDOW_NAME_CHANGE,
};

typedef struct SPacketGCPet
{
	BYTE header;
	BYTE subheader;
} TPacketGCPet;

typedef struct SPetSkillPacket
{
	bool bLocked;
	BYTE bSkill;
	BYTE bLevel;
	DWORD dwCooltime;
} TPetSkillPacket;

typedef struct SPacketGCPetSet
{
	BYTE header;
	DWORD dwID;
	DWORD dwSummonItemVnum;
	char szName[CItemData::PET_NAME_MAX_SIZE + 1];
	TPetSkillPacket aSkill[PET_SKILL_COUNT_MAX];
	DWORD dwPoints[POINT_UPBRINGING_MAX_NUM];
} TPacketGCPetSet;

typedef struct SPacketGCPetDelete
{
	BYTE header;
	DWORD dwID;
} TPacketGCPetDelete;

typedef struct SPacketGCPetPointUpdate
{
	BYTE header;
	DWORD dwID;
	BYTE bPoint;
	DWORD dwValue;
} TPacketGCPetPointUpdate;

typedef struct SPacketGCPetSummon
{
	BYTE header;
	DWORD dwID;
} TPacketGCPetSummon;

typedef struct SPacketGCPetDetermineResult
{
	BYTE header;
	BYTE type;
} TPacketGCPetDetermineResult;

typedef struct SPacketGCPetAttrChangeResult
{
	BYTE header;
	BYTE type;
	TItemPos pos;
} TPacketGCPetAttrChangeResult;

typedef struct SPetSkillUpdatePacket
{
	bool bLocked;
	BYTE bSkill;
	BYTE bLevel;
	DWORD dwCooltime;
	BYTE bSkillFormula1[PET_GROWTH_SKILL_LEVEL_MAX];
	WORD wSkillFormula2[PET_GROWTH_SKILL_LEVEL_MAX];
	BYTE bSkillFormula3[PET_GROWTH_SKILL_LEVEL_MAX];
} TPetSkillUpdatePacket;

typedef struct SPacketGCPetSkillCooltime
{
	BYTE header;
	DWORD dwID;
	BYTE bSlot;
	DWORD dwCooltime;
} TPacketGCPetSkillCooltime;

typedef struct SPacketGCPetSkillUpdate
{
	BYTE header;
	DWORD dwID;
	TPetSkillUpdatePacket aSkill[PET_SKILL_COUNT_MAX];
} TPacketGCPetSkillUpdate;

typedef struct SPacketGCPetNameChangeResult
{
	BYTE header;
	BYTE subheader;
	DWORD dwID;
	char szName[CItemData::PET_NAME_MAX_SIZE + 1];
} TPacketGCPetNameChangeResult;

typedef struct SPacketCGPetHatch
{
	BYTE header;
	char name[CItemData::PET_NAME_MAX_SIZE + 1];
	TItemPos eggPos;
} TPacketCGPetHatch;

typedef struct SPacketCGPetWindow
{
	BYTE header;
	BYTE window;
	bool state;
} TPacketCGPetWindow;

typedef struct SPacketCGPetWindowType
{
	BYTE header;
	BYTE type;
} TPacketCGPetWindowType;

typedef struct SPacketCGPeFeed
{
	BYTE header;
	BYTE index;
	WORD pos[PET_FEED_SLOT_MAX];
	WORD count[PET_FEED_SLOT_MAX];
} TPacketCGPetFeed;

typedef struct SPacketCGPetDetermine
{
	BYTE header;
	TItemPos determinePos;
} TPacketCGPetDetermine;

typedef struct SPacketCGPetAttrChange
{
	BYTE header;
	TItemPos upBringingPos;
	TItemPos attrChangePos;
} TPacketCGPetAttrChange;

typedef struct SPacketCGPetRevive
{
	BYTE header;
	TItemPos upBringingPos;
	WORD pos[PET_REVIVE_MATERIAL_SLOT_MAX];
	WORD count[PET_REVIVE_MATERIAL_SLOT_MAX];
} TPacketCGPetRevive;

typedef struct SPacketCGPetLearnSkill
{
	BYTE header;
	BYTE slotIndex;
	TItemPos learnSkillPos;
} TPacketCGPetLearnSkill;

typedef struct SPacketCGPetUpgradeSkill
{
	BYTE header;
	BYTE slotIndex;
} TPacketCGPetSkillUpgrade;

typedef struct SPacketCGPetDeleteSkill
{
	BYTE header;
	BYTE slotIndex;
	TItemPos deleteSkillPos;
} TPacketCGPetDeleteSkill;

typedef struct SPacketCGPetAllDeleteSkill
{
	BYTE header;
	TItemPos deleteAllSkillPos;
} TPacketCGPetDeleteAllSkill;

typedef struct SPacketCGPetNameChange
{
	BYTE header;
	char name[CItemData::PET_NAME_MAX_SIZE + 1];
	TItemPos changeNamePos;
	TItemPos upBringingPos;
} TPacketCGPetNameChange;
#endif

#ifdef ENABLE_ULTIMATE_REGEN
typedef struct SNewRegen
{
	long x, y;
	BYTE day, hour, minute, second;
	long safeRange;
	DWORD mob_vnum;
	BYTE hours_range;
	BYTE minute_range;
	int leftTime;
} TNewRegen;
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
enum class ECG_TRANSMUTATION_SHEADER: BYTE
{
	ITEM_CHECK_IN,
	ITEM_CHECK_OUT,
	FREE_ITEM_CHECK_IN,
	FREE_ITEM_CHECK_OUT,
	ACCEPT,
	CANCEL
};

typedef struct packet_set_transmutation
{
	BYTE header;
	WORD wCell;
	BYTE bSlotType;
} TPacketGCTransmutationItemSet;

typedef struct packet_transmutation_del
{
	BYTE header;
	WORD wCell;
	BYTE bSlotType;
} TPacketGCTransmutationDel;

typedef struct command_transmutation
{
	command_transmutation(const BYTE _Sub) : header(HEADER_CG_CHANGE_LOOK), subheader(_Sub), slot_type(255) {}
	BYTE header;
	BYTE subheader;
	BYTE slot_type;
	TItemPos pos;
} TPacketCGTransmutation;
#endif

#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
enum
{
	SUBHEADER_CG_ITEM_COMBINATION_COMBINE,
	SUBHEADER_CG_ITEM_COMBINATION_CANCEL,
};

typedef struct SPacketCGItemCombination
{
	BYTE bHeader;
	BYTE bSubHeader;
	TItemPos ItemPos1;
	TItemPos ItemPos2;
	TItemPos ItemPos3;
} TPacketCGItemCombination;
#endif


#ifdef ENABLE_HUNTING_SYSTEM
typedef struct SPacketCGHuntingAction
{
	BYTE bHeader;
	BYTE bAction;
	DWORD dValue;
} TPacketGCHuntingAction;

typedef struct SPacketCGOpenWindowHuntingMain
{
	BYTE bHeader;
	DWORD dLevel;
	DWORD dMonster;
	DWORD dCurCount;
	DWORD dDestCount;
	DWORD dMoneyMin;
	DWORD dMoneyMax;
	DWORD dExpMin;
	DWORD dExpMax;
	DWORD dRaceItem;
	DWORD dRaceItemCount;
} TPacketGCOpenWindowHuntingMain;

typedef struct SPacketCGOpenWindowHuntingSelect
{
	BYTE bHeader;
	DWORD dLevel;
	BYTE bType;
	DWORD dMonster;
	DWORD dCount;
	DWORD dMoneyMin;
	DWORD dMoneyMax;
	DWORD dExpMin;
	DWORD dExpMax;
	DWORD dRaceItem;
	DWORD dRaceItemCount;
} TPacketGCOpenWindowHuntingSelect;

typedef struct SPacketGCOpenWindowReward
{
	BYTE bHeader;
	DWORD dLevel;
	DWORD dReward;
	DWORD dRewardCount;
	DWORD dRandomReward;
	DWORD dRandomRewardCount;
	DWORD dMoney;
	BYTE bExp;
} TPacketGCOpenWindowReward;

typedef struct SPacketGCUpdateHunting
{
	BYTE bHeader;
	DWORD dCount;
} TPacketGCUpdateHunting;

typedef struct SPacketGCReciveRandomItems
{
	BYTE bHeader;
	BYTE bWindow;
	DWORD dItemVnum;
	DWORD dItemCount;
} TPacketGCReciveRandomItems;
#endif

#ifdef ENABLE_RIDING_EXTENDED
typedef struct SPacketCGMountUpGrade
{
	uint8_t header;
	uint8_t iSubHeader;
} TPacketCGMountUpGrade;

typedef struct SPacketGCMountUpGrade
{
	uint8_t header;
	uint8_t subheader;
	uint8_t horse_level;
	uint8_t is_fail;
	uint32_t existing_exp;
} TPacketGCMountUpGrade;

typedef struct SPacketGCMountUpGradeChat
{
	uint8_t header;
	uint8_t type;
	uint16_t value;
} TPacketGCMountUpGradeChat;
#endif

#ifdef ENABLE_AUTO_SELL_SYSTEM
typedef struct command_auto_sell_add
{
	BYTE header;
	DWORD vnum;
} TPacketCGAutoSellAdd;

typedef struct command_auto_sell_remove
{
	BYTE header;
	DWORD vnum;
} TPacketCGAutoSellRemove;

typedef struct command_auto_sell_status
{
	BYTE header;
	BYTE status;
} TPacketCGAutoSellStatus;
#endif

#ifdef ENABLE_STYLE_ATTRIBUTE_SYSTEM
typedef struct command_item_new_attribute
{
	BYTE  header;
	TItemPos source_pos;
	TItemPos target_pos;
	BYTE bValues[4 + 1];
} TPacketCGItemNewAttribute;
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
	char		playerName[CHARACTER_NAME_MAX_LEN + 1];
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

#ifdef ENABLE_DUNGEON_INFO
enum 
{
	DUNGEON_INFO_SUB_HEADER_OPEN,
	DUNGEON_INFO_SUB_HEADER_TELEPORT,
	DUNGEON_INFO_SUB_HEADER_MISION,
	DUNGEON_INFO_SUB_HEADER_OPEN_SEND,
	DUNGEON_INFO_SUB_HEADER_DATES_SEND,
	DUNGEON_INFO_SUB_HEADER_DATES_CLEAR,
	DUNGEON_INFO_SUB_HEADER_DATES_RANKING_CLEAR,
	DUNGEON_INFO_SUB_HEADER_DATES_RANKING_SEND,
	DUNGEON_INFO_SUB_HEADER_DATES_RANKING_LOAD,
	DUNGEON_INFO_SUB_HEADER_RANKING,

	DUNGEON_INFO_SUB_HEADER_DATES_MISION_CLEAR,
	DUNGEON_INFO_SUB_HEADER_DATES_MISION_SEND,
	DUNGEON_INFO_SUB_HEADER_DATES_MISION_LOAD,
};

typedef struct packet_send_dungeon_info_system
{
	BYTE header;
	BYTE subheader;
	int  index;

} TPacketCGDungeonInfoSend;

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

typedef struct  SFDateRanking
{
	int 	vnum_mob_r;
	int 	type;
	char 	name_r[24];
	int 	extra_r;

}TFDateInfoRanking;

typedef struct SFDateMision
{
	
	int 	vnum_mob_mision;
	int 	count_mob_mision;
	int 	count_mob_a_mision;
	int 	status_mision;
}TFDateMision;

typedef struct SPacketGCDungeonInfoReceive
{
	SPacketGCDungeonInfoReceive() : header(HEADER_GC_DUNGEON_INFO_SYSTEM)
	{}
	BYTE header;
	BYTE bSubType;

	SFDateInfoPacket dateinfo;
	SFDateRanking dateinfo_ranking;
	SFDateMision		dateinfom;

} TPacketGCDungeonInfoReceive;
#endif

#ifdef ENABLE_RESP_SYSTEM
enum ERespConfig
{
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
	uint32_t nextRespTime;
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
	uint32_t	nextRespTime;
	uint32_t	x;
	uint32_t	y;
} TPacketGCRespRefresh;
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

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
typedef struct command_cgroulette
{
	BYTE	header;
	BYTE 	option;
} TPacketCGSoulRoulette;
typedef struct command_gcroulette
{
	BYTE	header;
	BYTE 	option;
	int		yang;
	int		soul;
	struct {
		DWORD vnum;
		BYTE count;
	} ItemInfo[ROULETTE_ITEM_MAX];
} TPacketGCSoulRoulette;
#endif

#ifdef ENABLE_EVENT_SYSTEM
typedef struct packet_event_info
{
	BYTE		bHeader;
	BYTE		event_id;
	bool		isActivate;
	DWORD		event_time;
} TPacketGCEventInfo;
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
enum
{
	//HEADER_CG_ATTENDANCE_REWARD = = 35,
	
	//HEADER_GC_HIT_COUNT_INFO = = 55,
	//HEADER_GC_ATTENDANCE_EVENT = = 55,
	//HEADER_GC_ATTENDANCE_EVENT_INFO = = 55,
};

typedef struct SHitCountInfo
{
	SHitCountInfo(){ dwVid = dwCount = 0;}
	SHitCountInfo(DWORD vid, DWORD count)
	{
		dwVid = vid;
		dwCount = count;
	}
	DWORD	dwVid;
	DWORD	dwCount;
} THitCountInfo;

typedef struct SRewardItem
{
	BYTE bDay;
	DWORD dwVnum;
	DWORD dwCount;
} TRewardItem;

typedef struct SPacketGCHitCountInfo {
	BYTE	bHeader;
	DWORD	dwVid;
	DWORD	dwCount;
} TPacketGCHitCountInfo;

typedef struct SPacketGCAttendanceEventInfo {
	BYTE	bHeader;
	WORD	wSize;
	bool	bIsEnable;
	BYTE	bDay;
	BYTE	bClear;
	BYTE	bReward;
} TPacketGCAttendanceEventInfo;

typedef struct SPacketGCAttendanceEvent {
	BYTE	bHeader;
	BYTE	bType;
	BYTE	bValue;
} TPacketGCAttendanceEvent;
#endif

#ifdef ENABLE_MINI_GAME_CATCH_KING
enum
{
	//HEADER_CG_MINI_GAME_CATCH_KING = 37,
	//HEADER_GC_MINI_GAME_CATCH_KING = 60,
};

enum
{
	SUBHEADER_GC_CATCH_KING_EVENT_INFO,
	SUBHEADER_GC_CATCH_KING_START,
	SUBHEADER_GC_CATCH_KING_SET_CARD,
	SUBHEADER_GC_CATCH_KING_RESULT_FIELD,
	SUBHEADER_GC_CATCH_KING_SET_END_CARD,
	SUBHEADER_GC_CATCH_KING_REWARD
};

typedef struct SPacketCGMiniGameCatchKing {
	BYTE	bHeader;
	BYTE	bSubheader;
	BYTE	bSubArgument;
} TPacketCGMiniGameCatchKing;

typedef struct SPacketGCMiniGameCatchKing {
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bSubheader;
} TPacketGCMiniGameCatchKing;

typedef struct SPacketGCCatchKingEventInfo {
	BYTE	bHeader;
	bool	bIsEnable;
} TPacketGCCatchKingEventInfo;

typedef struct SPacketGCMiniGameCatchKingResult {
	DWORD	dwPoints;
	BYTE	bRowType;
	BYTE	bCardPos;
	BYTE	bCardValue;
	bool	bKeepFieldCard;
	bool	bDestroyHandCard;
	bool	bGetReward;
	bool 	bIsFiveNearBy;
} TPacketGCMiniGameCatchKingResult;

typedef struct SPacketGCMiniGameCatchKingSetEndCard {
	BYTE	bCardPos;
	BYTE	bCardValue;
} TPacketGCMiniGameCatchKingSetEndCard;
#endif

#ifdef ENABLE_STONE_EVENT
typedef struct packet_stone_event
{
	BYTE	header;
	int		stone_point;
} TPacketGCStoneEvent;
#endif

#ifdef ENABLE_KILL_STATISTICS
typedef struct command_kill_statistics_packet
{
	BYTE bHeader;
	int  iJinnoKills;
	int	iShinsooKills;
	int	iChunjoKills;
	int iTotalKills;
	int iTotalDeaths;
	int iDuelsWon;
	int iDuelsLost;
	int	iBossesKills;
	int	iStonesKills;
	int	iMobsKills;
	int	top_damage;
} TPacketGCKillStatistics;
#endif

#ifdef ENABLE_RANKING
enum
{
	MAX_RANKING_LIST = 51,
};

typedef struct SPacketGCList
{
	int		iPosition, iRealPosition, iLevel;
	long long	iPoints;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCList;

typedef struct SPacketGCRankingTable
{
	SPacketGCRankingTable() : bHeader(HEADER_GC_RANKING_SEND)
	{}
	BYTE 			bHeader;
	TPacketGCList	list[MAX_RANKING_LIST];
} TPacketGCRankingTable;
#endif

#ifdef ENABLE_STATUS_EFFECT_SHOWER
enum
{
	SUB_HEADER_GC_STATUS_EFFECT_INITIALIZE = 0,
	SUB_HEADER_GC_STATUS_EFFECT_ELEMENT_ENCHANT = 1,
	SUB_HEADER_GC_STATUS_EFFECT_ADD = 2,
	SUB_HEADER_GC_STATUS_EFFECT_REMOVE = 3,
	SUB_HEADER_GC_STATUS_EFFECT_REMOVE_2 = 4,
	SUB_HEADER_GC_STATUS_EFFECT_PET_SKILL = 5,
	SUB_HEADER_GC_STATUS_EFFECT_CLEAR_PET_SKILL = 6,
};

typedef struct packet_status_effect
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCStatusEffect;

typedef struct packet_status_effect_item
{
	DWORD dwType;
	WORD wPointType;
	long lPointValue;
	long lDuration;
	long lFlag;
} TPacketGCStatusEffectItem;

typedef struct packet_status_effect_element_enchant
{
	DWORD dwVID;
	DWORD adwElement[6];
} TPacketGCStatusEffectElementEnchant;

typedef struct packet_status_effect_pet_skill
{
	DWORD adwPetSkill[3];
	DWORD adwCoolTime[3];
} TPacketGCStatusEffectPetSkill;
#endif // ENABLE_STATUS_EFFECT_SHOWER

#ifdef ENABLE_YIN_AND_YANG_ATTR
enum EYinAndYangAttrSubHeadersCG
{
	SUBHEADER_CG_YIN_YANG_ATTR_ATTACH = 0,
	SUBHEADER_CG_YIN_YANG_ATTR_MOVE = 1,
	SUBHEADER_CG_YIN_YANG_ATTR_DELETE = 2,
	SUBHEADER_CG_YIN_YANG_ATTR_REGISTER_RECENTLY_GAINED = 3,
	SUBHEADER_CG_YIN_YANG_ATTR_CLOSE = 4,
	SUBHEADER_CG_YIN_YANG_ATTR_EXPANSION = 5,
	SUBHEADER_CG_YIN_YANG_ATTR_ENABLE = 6,
	SUBHEADER_CG_YIN_YANG_ATTR_DISABLE = 7,
};

typedef struct packet_cg_yin_and_yang_attr
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketCGYinAndYangAttr;

typedef struct packet_cg_yin_and_yang_attr_attach
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
	BYTE bFromPos;
	BYTE bToPos;
} TPacketCGYinAndYangAttrAttach;

typedef struct packet_cg_yin_and_yang_attr_move
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
	BYTE bFromPos;
	BYTE bToPos;
} TPacketCGYinAndYangAttrMove;

typedef struct packet_cg_yin_and_yang_attr_delete
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
	BYTE bPos;
} TPacketCGYinAndYangAttrDelete;

enum EYinAndYangAttrSubHeadersGC
{
	SUBHEADER_GC_YIN_YANG_ATTR_OPEN = 0,
	SUBHEADER_GC_YIN_YANG_ATTR_SET_DATA = 1,
	SUBHEADER_GC_YIN_YANG_ATTR_DELETE = 2,
	SUBHEADER_GC_YIN_YANG_ATTR_UPDATE = 3,
	SUBHEADER_GC_YIN_YANG_ATTR_MOVE = 4,
	SUBHEADER_GC_YIN_YANG_ATTR_EXPANSION = 5,
	SUBHEADER_GC_YIN_YANG_ATTR_ATTR_ACTIVATED = 6,
	SUBHEADER_GC_YIN_YANG_ATTR_ENABLE = 7,
	SUBHEADER_GC_YIN_YANG_ATTR_DISABLE = 8,
};

typedef struct packet_gc_yin_and_yang_attr
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCYinAndYangAttr;

typedef struct packet_gc_yin_and_yang_attr_item_data
{
	BYTE bSlotPos;
	DWORD dwAttrVal;
	BYTE byAttrType;
	DWORD dwRemainTime;
} TPacketGCYinAndYangAttrItemData;

typedef struct packet_gc_yin_and_yang_attr_update
{
	BYTE bSlotPos;
	DWORD dwAttrVal;
	BYTE byAttrType;
	DWORD dwRemainTime;
} TPacketGCYinAndYangAttrUpdate;

typedef struct packet_gc_yin_and_yang_attr_move
{
	BYTE bFromPos;
	BYTE bToPos;
} TPacketGCYinAndYangAttrMove;
#endif // ENABLE_YIN_AND_YANG_ATTR

#pragma pack(pop)
