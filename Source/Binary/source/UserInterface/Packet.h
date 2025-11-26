#pragma once

#include "StdAfx.h"

#include "../gamelib/RaceData.h"
#include "../gamelib/ItemData.h"

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
#ifdef ENABLE_BIOLOG_SYSTEM
	HEADER_CG_BIOLOG_MANAGER					= 25,
#endif
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
	//HEADER_CG_EMPTY							= 34,
	//HEADER_CG_EMPTY							= 35,
	//HEADER_CG_EMPTY							= 36,
	//HEADER_CG_EMPTY							= 37,
	//HEADER_CG_EMPTY							= 38,
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
	//HEADER_CG_EMPTY							= 81,
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
	//HEADER_CG_EMPTY							= 93,
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
	//HEADER_CG_EMPTY							= 115,
#ifdef ENABLE_EVENT_MANAGER
	HEADER_CG_REQUEST_EVENT_QUEST				= 116,
	HEADER_CG_REQUEST_EVENT_DATA				= 117,
#endif
#ifdef ENABLE_RENEWAL_BATTLE_PASS
	HEADER_CG_EXT_BATTLE_PASS_ACTION			= 118,
	HEADER_CG_EXT_SEND_BP_PREMIUM_ITEM			= 119,
#endif
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
	//HEADER_CG_EMPTY							= 134,
	//HEADER_CG_EMPTY							= 135,
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
	//HEADER_CG_EMPTY							= 207,
	//HEADER_CG_EMPTY							= 208,
	//HEADER_CG_EMPTY							= 209,
	//HEADER_CG_EMPTY							= 210,
	//HEADER_CG_EMPTY							= 212,
	//HEADER_CG_EMPTY							= 213,
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
	//HEADER_CG_EMPTY							= 229,
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
	//HEADER_GC_EMPTY							= 37,
	HEADER_GC_SHOP								= 38,
	HEADER_GC_SHOP_SIGN							= 39,
	HEADER_GC_DUEL_START						= 40,
	HEADER_GC_PVP								= 41,
	HEADER_GC_EXCHANGE							= 42,
	HEADER_GC_CHARACTER_POSITION				= 43,
	HEADER_GC_PING								= 44,
	HEADER_GC_SCRIPT							= 45,
	HEADER_GC_QUEST_CONFIRM						= 46,
#ifdef ENABLE_BIOLOG_SYSTEM
	HEADER_GC_BIOLOG_MANAGER					= 47,
#endif
	HEADER_GC_GUILDMARK_PASS					= 48,
#ifdef ENABLE_EVENT_MANAGER
	HEADER_GC_EVENT_INFO						= 49,
	HEADER_GC_EVENT_RELOAD						= 50,
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	HEADER_GC_OFFLINE_SHOP						= 51,
	HEADER_GC_OFFLINE_SHOP_SIGN					= 52,
#endif
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
	HEADER_GC_SHOPSEARCH_SET					= 53,
#endif
	//HEADER_GC_EMPTY							= 54,
	//HEADER_GC_EMPTY							= 55,
	//HEADER_GC_EMPTY							= 56,
	//HEADER_GC_EMPTY							= 57,
#ifdef ENABLE_MOB_DROP_INFO
	HEADER_GC_TARGET_INFO						= 58,
#endif
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
	HEADER_GC_PICKUP_ITEM_SC					= 59,
#endif
	//HEADER_GC_EMPTY							= 60,
	HEADER_GC_MOUNT								= 61,
	HEADER_GC_OWNERSHIP							= 62,
	HEADER_GC_TARGET							= 63,
	//HEADER_GC_EMPTY							= 64,
	HEADER_GC_WARP								= 65,
	//HEADER_GC_EMPTY							= 66,
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
	//HEADER_GC_EMPTY							= 93,
	//HEADER_GC_EMPTY							= 94,
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
	//HEADER_GC_EMPTY							= 157,
	//HEADER_GC_EMPTY							= 158,
	//HEADER_GC_EMPTY							= 159,
#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	HEADER_GC_WHISPER_DETAILS					= 160,
#endif
#ifdef ENABLE_GUILD_TOKEN_AUTH
	HEADER_GC_GUILD_TOKEN						= 162,
#endif
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
#ifdef ENABLE_RENEWAL_INGAME_ITEMSHOP
	HEADER_GC_ITEMSHOP							= 172,
#endif
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
	//HEADER_GC_EMPTY							= 185,
	//HEADER_GC_EMPTY							= 186,
	//HEADER_GC_EMPTY							= 187,
	//HEADER_GC_EMPTY							= 188,
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
	//HEADER_GC_EMPTY							= 213,
	//HEADER_GC_EMPTY							= 214,
#ifdef ENABLE_PARTY_POSITION
	HEADER_GC_PARTY_POSITION_INFO				= 215,
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	HEADER_GC_REQUEST_CHANGE_LANGUAGE			= 216,
#endif
#ifdef ENABLE_RENEWAL_CUBE
	HEADER_GC_CUBE_RENEWAL						= 218,
#endif
#ifdef ENABLE_RENEWAL_BATTLE_PASS
	HEADER_GC_EXT_BATTLE_PASS_OPEN				= 219,
	HEADER_GC_EXT_BATTLE_PASS_GENERAL_INFO		= 220,
	HEADER_GC_EXT_BATTLE_PASS_MISSION_INFO		= 221,
	HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE	= 222,
	HEADER_GC_EXT_BATTLE_PASS_SEND_RANKING		= 223,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	HEADER_GC_AURA								= 224,
#endif
	//HEADER_GC_EMPTY							= 225,
	//HEADER_GC_EMPTY							= 226,
	//HEADER_GC_EMPTY							= 227,
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

	MARK_CRC_NUM = 8*8,
	MARK_DATA_SIZE = 16*12,
	SYMBOL_DATA_SIZE = 128*256,
	QUEST_INPUT_STRING_MAX_NUM = 64,

#ifdef ENABLE_RENEWAL_OX_EVENT
	QUEST_INPUT_LONG_STRING_MAX_NUM = 128,
#endif

#ifdef ENABLE_EVENT_MANAGER
	QUEST_NAME_MAX_NUM = 64,
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
	BYTE header;
	DWORD handle;
	DWORD random_key;
} TPacketCGMarkLogin;

typedef struct command_mark_upload
{
	BYTE header;
	DWORD gid;
	DWORD markpass;
	BYTE image[16*12*4];
#ifdef ENABLE_GUILD_TOKEN_AUTH
	uint64_t token;
#endif
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
	BYTE header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
	BYTE header;
	BYTE imgIdx;
	DWORD crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
	BYTE header;
	DWORD bufSize;
	WORD count;
} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
	BYTE header;
	DWORD bufSize;
	BYTE imgIdx;
	DWORD count;
} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE header;
	WORD size;
	DWORD handle;
	DWORD markpass;
#ifdef ENABLE_GUILD_TOKEN_AUTH
	uint64_t token;
#endif
} TPacketCGSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE header;
	DWORD dwGuildID;
	DWORD dwCRC;
	DWORD dwSize;
} TPacketCGSymbolCRC;

typedef struct packet_markpass
{
	BYTE header;
	DWORD markpass;
} TPacketMarkPass;

typedef struct packet_symbol_data
{
	BYTE header;
	WORD size;
	DWORD guild_id;
} TPacketGCGuildSymbolData;

typedef struct packet_observer_add
{
	BYTE header;
	DWORD vid;
	WORD x;
	WORD y;
} TPacketGCObserverAdd;

typedef struct packet_observer_move
{
	BYTE header;
	DWORD vid;
	WORD x;
	WORD y;
} TPacketGCObserverMove;

typedef struct packet_observer_remove
{
	BYTE header;
	DWORD vid;
} TPacketGCObserverRemove;

typedef struct command_checkin
{
	BYTE header;
	char name[ID_MAX_NUM+1];
	char pwd[PASS_MAX_NUM+1];
} TPacketCGCheckin;

typedef struct command_login
{
	BYTE header;
	char name[ID_MAX_NUM + 1];
	char pwd[PASS_MAX_NUM + 1];
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE header;
	char name[ID_MAX_NUM + 1];
	DWORD login_key;
	DWORD adwClientKey[4];
} TPacketCGLogin2;

typedef struct command_login3
{
	BYTE header;
	char name[ID_MAX_NUM + 1];
	char pwd[PASS_MAX_NUM + 1];
	DWORD adwClientKey[4];
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
#endif
#ifdef ENABLE_RENEWAL_CLIENT_VERSION
	char ClientVersion[10];
#endif
} TPacketCGLogin3;

typedef struct command_direct_enter
{
	BYTE bHeader;
	char login[ID_MAX_NUM + 1];
	char passwd[PASS_MAX_NUM + 1];
	BYTE index;
} TPacketCGDirectEnter;

typedef struct command_player_select
{
	BYTE header;
	BYTE player_index;
} TPacketCGSelectCharacter;

typedef struct command_attack
{
	BYTE header;
	BYTE bType;
	DWORD dwVictimVID;
	BYTE bCRCMagicCubeProcPiece;
	BYTE bCRCMagicCubeFilePiece;
} TPacketCGAttack;

typedef struct command_chat
{
	BYTE header;
	WORD length;
	BYTE type;
} TPacketCGChat;

typedef struct command_whisper
{
	BYTE bHeader;
	WORD wSize;
	char szNameTo[CHARACTER_NAME_MAX_LEN + 1];
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
#ifdef ENABLE_TITLE_SYSTEM
	int iTitleID;
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
#ifdef ENABLE_TITLE_SYSTEM
	int iTitleID;
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
#ifdef ENABLE_TITLE_SYSTEM
	int iTitleID;
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
	POINT_NONE,							// 0
	POINT_LEVEL,						// 1
	POINT_VOICE,						// 2
	POINT_EXP,							// 3
	POINT_NEXT_EXP,						// 4
	POINT_HP,							// 5
	POINT_MAX_HP,						// 6
	POINT_SP,							// 7
	POINT_MAX_SP,						// 8
	POINT_STAMINA,						// 9
	POINT_MAX_STAMINA,					// 10
	POINT_GOLD,							// 11
	POINT_ST,							// 12
	POINT_HT,							// 13
	POINT_DX,							// 14
	POINT_IQ,							// 15
	POINT_ATT_POWER,					// 16
	POINT_ATT_SPEED,					// 17
	POINT_EVADE_RATE,					// 18
	POINT_MOV_SPEED,					// 19
	POINT_DEF_GRADE,					// 20
	POINT_CASTING_SPEED,				// 21
	POINT_MAGIC_ATT_GRADE,				// 22
	POINT_MAGIC_DEF_GRADE,				// 23
	POINT_EMPIRE_POINT,					// 24
	POINT_LEVEL_STEP,					// 25
	POINT_STAT,							// 26
	POINT_SUB_SKILL,					// 27
	POINT_SKILL,						// 28
	POINT_MIN_ATK,						// 29
	POINT_MAX_ATK,						// 30
	POINT_PLAYTIME,						// 31
	POINT_HP_REGEN,						// 32
	POINT_SP_REGEN,						// 33
	POINT_BOW_DISTANCE,					// 34
	POINT_HP_RECOVERY,					// 35
	POINT_SP_RECOVERY,					// 36
	POINT_POISON_PCT,					// 37
	POINT_STUN_PCT,						// 38
	POINT_SLOW_PCT,						// 39
	POINT_CRITICAL_PCT,					// 40
	POINT_PENETRATE_PCT,				// 41
	POINT_CURSE_PCT,					// 42
	POINT_ATTBONUS_HUMAN,				// 43
	POINT_ATTBONUS_ANIMAL,				// 44
	POINT_ATTBONUS_ORC,					// 45
	POINT_ATTBONUS_MILGYO,				// 46
	POINT_ATTBONUS_UNDEAD,				// 47
	POINT_ATTBONUS_DEVIL,				// 48
	POINT_ATTBONUS_INSECT,				// 49
	POINT_ATTBONUS_FIRE,				// 50
	POINT_ATTBONUS_ICE,					// 51
	POINT_ATTBONUS_DESERT,				// 52
	POINT_ATTBONUS_MONSTER,				// 53
	POINT_ATTBONUS_WARRIOR,				// 54
	POINT_ATTBONUS_ASSASSIN,			// 55
	POINT_ATTBONUS_SURA,				// 56
	POINT_ATTBONUS_SHAMAN,				// 57
	POINT_ATTBONUS_TREE,				// 58
	POINT_RESIST_WARRIOR,				// 59
	POINT_RESIST_ASSASSIN,				// 60
	POINT_RESIST_SURA,					// 61
	POINT_RESIST_SHAMAN,				// 62
	POINT_STEAL_HP,						// 63
	POINT_STEAL_SP,						// 64
	POINT_MANA_BURN_PCT,				// 65
	POINT_DAMAGE_SP_RECOVER,			// 66
	POINT_BLOCK,						// 67
	POINT_DODGE,						// 68
	POINT_RESIST_SWORD,					// 69
	POINT_RESIST_TWOHAND,				// 70
	POINT_RESIST_DAGGER,				// 71
	POINT_RESIST_BELL,					// 72
	POINT_RESIST_FAN,					// 73
	POINT_RESIST_BOW,					// 74
	POINT_RESIST_FIRE,					// 75
	POINT_RESIST_ELEC,					// 76
	POINT_RESIST_MAGIC,					// 77
	POINT_RESIST_WIND,					// 78
	POINT_REFLECT_MELEE,				// 79
	POINT_REFLECT_CURSE,				// 80
	POINT_POISON_REDUCE,				// 81
	POINT_KILL_SP_RECOVER,				// 82
	POINT_EXP_DOUBLE_BONUS,				// 83
	POINT_GOLD_DOUBLE_BONUS,			// 84
	POINT_ITEM_DROP_BONUS,				// 85
	POINT_POTION_BONUS,					// 86
	POINT_KILL_HP_RECOVER,				// 87
	POINT_IMMUNE_STUN,					// 88
	POINT_IMMUNE_SLOW,					// 89
	POINT_IMMUNE_FALL,					// 90
	POINT_PARTY_ATT_GRADE,				// 91
	POINT_PARTY_DEF_GRADE,				// 92
	POINT_ATT_BONUS,					// 93
	POINT_DEF_BONUS,					// 94
	POINT_ATT_GRADE_BONUS,				// 95
	POINT_DEF_GRADE_BONUS,				// 96
	POINT_MAGIC_ATT_GRADE_BONUS,		// 97
	POINT_MAGIC_DEF_GRADE_BONUS,		// 98
	POINT_RESIST_NORMAL_DAMAGE,			// 99
	POINT_HIT_HP_RECOVERY,				// 100
	POINT_HIT_SP_RECOVERY,				// 101
	POINT_MANASHIELD,					// 102
	POINT_PARTY_BUFFER_BONUS,			// 103
	POINT_PARTY_SKILL_MASTER_BONUS,		// 104
	POINT_HP_RECOVER_CONTINUE,			// 105
	POINT_SP_RECOVER_CONTINUE,			// 106
	POINT_STEAL_GOLD,					// 107
	POINT_POLYMORPH,					// 108 
	POINT_MOUNT,						// 109
	POINT_PARTY_HASTE_BONUS,			// 110
	POINT_PARTY_DEFENDER_BONUS,			// 111
	POINT_STAT_RESET_COUNT,				// 112
	POINT_HORSE_SKILL,					// 113
	POINT_MALL_ATTBONUS,				// 114
	POINT_MALL_DEFBONUS,				// 115
	POINT_MALL_EXPBONUS,				// 116
	POINT_MALL_ITEMBONUS,				// 117
	POINT_MALL_GOLDBONUS,				// 118
	POINT_MAX_HP_PCT,					// 119
	POINT_MAX_SP_PCT,					// 120
	POINT_SKILL_DAMAGE_BONUS,			// 121
	POINT_NORMAL_HIT_DAMAGE_BONUS,		// 122
	POINT_SKILL_DEFEND_BONUS,			// 123
	POINT_NORMAL_HIT_DEFEND_BONUS,		// 124
	POINT_RAMADAN_CANDY_BONUS_EXP,		// 125
	POINT_ENERGY,						// 126
	POINT_ENERGY_END_TIME,				// 127
	POINT_COSTUME_ATTR_BONUS,			// 128
	POINT_MAGIC_ATT_BONUS_PER,			// 129
	POINT_MELEE_MAGIC_ATT_BONUS_PER,	// 130
	POINT_RESIST_ICE,					// 131
	POINT_RESIST_EARTH,					// 132
	POINT_RESIST_DARK,					// 133
	POINT_RESIST_CRITICAL,				// 134
	POINT_RESIST_PENETRATE,				// 135

#ifdef ENABLE_PENDANT_SYSTEM
	POINT_ATTBONUS_ELEC,				// 136
	POINT_ATTBONUS_WIND,				// 137
	POINT_ATTBONUS_EARTH,				// 138
	POINT_ATTBONUS_DARK,				// 139
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	POINT_ACCEDRAIN_RATE,				// 140
#endif

#ifdef ENABLE_ATTBONUS_METIN
	POINT_ATTBONUS_METIN,				// 141
#endif

#ifdef ENABLE_COINS_INVENTORY
	POINT_COINS,						// 142
#endif

#ifdef ENABLE_TITLE_SYSTEM
	POINT_TITLE,						// 143
	POINT_TITLE_1,						// 144
	POINT_TITLE_2,						// 145
	POINT_TITLE_3,						// 146
	POINT_TITLE_4,						// 147
	POINT_TITLE_5,						// 148
	POINT_TITLE_6,						// 149
	POINT_TITLE_7,						// 150
	POINT_TITLE_8,						// 151
	POINT_TITLE_9,						// 152
	POINT_TITLE_10,						// 153
	POINT_TITLE_11,						// 154
	POINT_TITLE_12,						// 155
	POINT_TITLE_13,						// 156
	POINT_TITLE_14,						// 157
	POINT_TITLE_15,						// 158
	POINT_TITLE_16,						// 159
	POINT_TITLE_17,						// 160
	POINT_TITLE_18,						// 161
	POINT_TITLE_19,						// 162
	POINT_TITLE_20,						// 163
#endif

#ifdef ENABLE_ATTBONUS_BOSS
	POINT_ATTBONUS_BOSS,				// 164
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
	POINT_BATTLE_PASS_PREMIUM_ID,		// 165
#endif

#ifdef ENABLE_RENEWAL_BONUS_BOARD
	POINT_BLUE_PLAYER_KILLED,			// 166
	POINT_YELLOW_PLAYER_KILLED,			// 167
	POINT_RED_PLAYER_KILLED,			// 168
	POINT_ALL_PLAYER_KILLED,			// 169
	POINT_KILL_DUELWON,					// 170
	POINT_KILL_DUELLOST,				// 171
	POINT_MONSTER_KILLED,				// 172
	POINT_STONE_KILLED,					// 173
	POINT_BOSS_KILLED,					// 174
#endif

	POINT_MIN_WEP						= 200,
	POINT_MAX_WEP,						// 201
	POINT_MIN_MAGIC_WEP,				// 202
	POINT_MAX_MAGIC_WEP,				// 203
	POINT_HIT_RATE,						// 204
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
};

typedef struct packet_shop
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCShop;

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
	time_t tNextRead;
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
	time_t time;
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
#ifdef ENABLE_RENEWAL_BATTLE_PASS
	SE_EFFECT_BP_NORMAL_MISSION_COMPLETED,
	SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED,
	SE_EFFECT_BP_NORMAL_BATTLEPASS_COMPLETED,
	SE_EFFECT_BP_PREMIUM_BATTLEPASS_COMPLETED,
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	SE_GYEONGGONG_BOOM,
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
#ifdef ENABLE_RENEWAL_REGEN
	DWORD mobVnum;
#else
	char name[CHARACTER_NAME_MAX_LEN+1];
#endif
	long x;
	long y;
#ifdef ENABLE_RENEWAL_REGEN
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

typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[128];
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

#ifdef ENABLE_BIOLOG_SYSTEM
enum ECGBiologSubHeaders
{
	CG_BIOLOG_MANAGER_OPEN,
	CG_BIOLOG_MANAGER_SEND,
	CG_BIOLOG_MANAGER_TIMER,
};

enum EGCBiologSubHeaders
{
	GC_BIOLOG_MANAGER_OPEN,
	GC_BIOLOG_MANAGER_ALERT,
	GC_BIOLOG_MANAGER_CLOSE,
};

typedef struct SPacketCGBiologManagerAction
{
	BYTE bHeader;
	BYTE bSubHeader;
} TPacketCGBiologManagerAction;

typedef struct SPacketGCBiologManager
{
	BYTE bHeader;
	WORD wSize;
	BYTE bSubHeader;
} TPacketGCBiologManager;

typedef struct SPacketGCBiologManagerInfo
{
	bool bUpdate;
	BYTE bRequiredLevel;
	DWORD iRequiredItem;
	WORD wGivenItems;
	WORD wRequiredItemCount;
	time_t iGlobalCooldown;
	time_t iCooldown;
	bool iCooldownReminder;
	BYTE bChance;
	DWORD bApplyType[MAX_BONUSES_LENGTH];
	long lApplyValue[MAX_BONUSES_LENGTH];
	DWORD dRewardItem;
	WORD wRewardItemCount;
} TPacketGCBiologManagerInfo;
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

#ifdef ENABLE_EVENT_MANAGER
typedef struct command_request_event_quest
{
	BYTE bHeader;
	char szName[QUEST_NAME_MAX_NUM + 1];
} TPacketCGRequestEventQuest;

typedef struct SPacketCGRequestEventData
{
	BYTE bHeader;
	BYTE bMonth;
} TPacketCGRequestEventData;

typedef struct SPacketGCEventInfo
{
	BYTE bHeader;
	WORD wSize; // Fix
} TPacketGCEventInfo;

typedef struct SPacketGCEventReload
{
	BYTE bHeader;
} TPacketGCEventReload;

typedef struct SPacketEventData
{
	DWORD dwID;
	BYTE bType;
	long startTime;
	long endTime;
	DWORD dwVnum;
	int iPercent;
	int iDropType;
	bool bCompleted;
} TPacketEventData;
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
typedef struct SPacketCGExtBattlePassAction
{
	BYTE bHeader;
	BYTE bAction;
} TPacketCGExtBattlePassAction;

typedef struct SPacketCGExtBattlePassSendPremiumItem
{
	BYTE bHeader;
	int iSlotIndex;
} TPacketCGExtBattlePassSendPremiumItem;

typedef struct SPacketGCExtBattlePassOpen
{
	BYTE bHeader;
} TPacketGCExtBattlePassOpen;

typedef struct SPacketGCExtBattlePassGeneralInfo
{
	BYTE bHeader;
	BYTE bBattlePassType;
	char szSeasonName[64+1];
	DWORD dwBattlePassID;
	DWORD dwBattlePassStartTime;
	DWORD dwBattlePassEndTime;
} TPacketGCExtBattlePassGeneralInfo;

typedef struct SPacketGCExtBattlePassMissionInfo
{
	BYTE bHeader;
	WORD wSize;
	WORD wRewardSize;
	BYTE bBattlePassType;
	DWORD dwBattlePassID;
} TPacketGCExtBattlePassMissionInfo;

typedef struct SPacketGCExtBattlePassMissionUpdate
{
	BYTE bHeader;
	BYTE bBattlePassType;
	BYTE bMissionIndex;
	BYTE bMissionType;
	DWORD dwNewProgress;
} TPacketGCExtBattlePassMissionUpdate;

typedef struct SPacketGCExtBattlePassRanking
{
	BYTE bHeader;
	char szPlayerName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE bBattlePassType;
	BYTE bBattlePassID;
	DWORD dwStartTime;
	DWORD dwEndTime;
} TPacketGCExtBattlePassRanking;
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

#ifdef ENABLE_RENEWAL_INGAME_ITEMSHOP
enum
{
	ITEMSHOP_LOAD,
	ITEMSHOP_LOG,
	ITEMSHOP_BUY,
	ITEMSHOP_DRAGONCOIN,
	ITEMSHOP_RELOAD,
	ITEMSHOP_LOG_ADD,
	ITEMSHOP_UPDATE_ITEM,
};

typedef struct SIShopData
{
	DWORD id;
	DWORD itemVnum;
	long long itemPrice;
	int topSellingIndex;
	BYTE discount;
	int offerTime;
	int addedTime;
	long long sellCount;
	int week_limit;
	int month_limit;
	int maxSellCount;
} TIShopData;

typedef struct SIShopLogData
{
	DWORD accountID;
	char playerName[CHARACTER_NAME_MAX_LEN + 1];
	char buyDate[21];
	int buyTime;
	char ipAdress[16];
	DWORD itemID;
	DWORD itemVnum;
	int itemCount;
	long long itemPrice;
} TIShopLogData;

typedef struct SPacketGCItemShop
{
	BYTE header;
	DWORD size;
} TPacketGCItemShop;
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

#ifdef ENABLE_RENEWAL_REGEN
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

#ifdef ENABLE_GUILD_TOKEN_AUTH
struct TPacketGCGuildToken
{
	uint8_t header;
	uint64_t token;
};
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

#pragma pack(pop)
