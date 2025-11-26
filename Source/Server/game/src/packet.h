#ifndef __INC_PACKET_H__
#define __INC_PACKET_H__

#include "../../common/service.h"
#include "../../common/tables.h"

enum PacketHeaderCG
{
	HEADER_CG_LOGIN							= 1, // unused - to be removed
	HEADER_CG_ATTACK						= 2,
	HEADER_CG_CHAT							= 3,
	HEADER_CG_CHARACTER_CREATE				= 4,
	HEADER_CG_CHARACTER_DELETE				= 5,
	HEADER_CG_CHARACTER_SELECT				= 6,
	HEADER_CG_MOVE							= 7,
	HEADER_CG_SYNC_POSITION					= 8,
	HEADER_CG_DIRECT_ENTER					= 9,
	HEADER_CG_ENTERGAME						= 10,
	HEADER_CG_ITEM_USE						= 11,
	HEADER_CG_ITEM_DROP						= 12,
	HEADER_CG_ITEM_MOVE						= 13,
	//HEADER_CG_EMPTY						= 14,
	HEADER_CG_ITEM_PICKUP					= 15,
	HEADER_CG_QUICKSLOT_ADD					= 16,
	HEADER_CG_QUICKSLOT_DEL					= 17,
	HEADER_CG_QUICKSLOT_SWAP				= 18,
	HEADER_CG_WHISPER						= 19,
	HEADER_CG_ITEM_DROP2					= 20,
	HEADER_CG_STATE_CHECKER					= 21,
#ifdef ENABLE_QUICK_SELL_ITEM
	HEADER_CG_ITEM_SELL						= 22,
#endif
#ifdef ENABLE_DESTROY_DIALOG
	HEADER_CG_ITEM_DESTROY					= 23,
#endif
	//HEADER_CG_EMPTY						= 24,
#ifdef ENABLE_BIOLOG_SYSTEM
	HEADER_CG_BIOLOG_MANAGER				= 25,
#endif
	HEADER_CG_ON_CLICK						= 26,
	HEADER_CG_EXCHANGE						= 27,
	HEADER_CG_CHARACTER_POSITION			= 28,
	HEADER_CG_SCRIPT_ANSWER					= 29,
	HEADER_CG_QUEST_INPUT_STRING			= 30,
	HEADER_CG_QUEST_CONFIRM					= 31,
	//HEADER_CG_EMPTY						= 32,
#ifdef ENABLE_RENEWAL_OX_EVENT
	HEADER_CG_QUEST_INPUT_LONG_STRING		= 33,
#endif
	//HEADER_CG_EMPTY						= 34,
	//HEADER_CG_EMPTY						= 35,
	//HEADER_CG_EMPTY						= 36,
	//HEADER_CG_EMPTY						= 37,
	//HEADER_CG_EMPTY						= 38,
	//HEADER_CG_EMPTY						= 39,
	//HEADER_CG_EMPTY						= 40,
	HEADER_CG_PVP							= 41,
	//HEADER_CG_EMPTY						= 42,
	//HEADER_CG_EMPTY						= 43,
	//HEADER_CG_EMPTY						= 44,
	//HEADER_CG_EMPTY						= 45,
	//HEADER_CG_EMPTY						= 46,
	//HEADER_CG_EMPTY						= 47,
	//HEADER_CG_EMPTY						= 48,
	//HEADER_CG_EMPTY						= 49,
	HEADER_CG_SHOP							= 50,
	HEADER_CG_FLY_TARGETING					= 51,
	HEADER_CG_USE_SKILL						= 52,
	HEADER_CG_ADD_FLY_TARGETING				= 53,
	HEADER_CG_SHOOT							= 54,
	HEADER_CG_MYSHOP						= 55,
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	HEADER_CG_SKILL_COLOR					= 56,
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	HEADER_CG_OFFLINE_SHOP					= 57,
	HEADER_CG_MY_OFFLINE_SHOP				= 58,
#endif
#ifdef ENABLE_MOB_DROP_INFO
	HEADER_CG_TARGET_INFO_LOAD				= 59,
#endif
	HEADER_CG_ITEM_USE_TO_ITEM				= 60,
	HEADER_CG_TARGET			 			= 61,
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
	HEADER_CG_SHOP_SEARCH					= 62,
#endif
	//HEADER_CG_EMPTY						= 63,
	//HEADER_CG_EMPTY						= 64,
	HEADER_CG_WARP							= 65,
	HEADER_CG_SCRIPT_BUTTON					= 66,
	HEADER_CG_MESSENGER						= 67,
	//HEADER_CG_EMPTY						= 68,
	HEADER_CG_MALL_CHECKOUT					= 69,
	HEADER_CG_SAFEBOX_CHECKIN				= 70,
	HEADER_CG_SAFEBOX_CHECKOUT				= 71,
	HEADER_CG_PARTY_INVITE					= 72,
	HEADER_CG_PARTY_INVITE_ANSWER			= 73,
	HEADER_CG_PARTY_REMOVE					= 74,
	HEADER_CG_PARTY_SET_STATE				= 75,
	HEADER_CG_PARTY_USE_SKILL				= 76,
	HEADER_CG_SAFEBOX_ITEM_MOVE				= 77,
	HEADER_CG_PARTY_PARAMETER				= 78,
	//HEADER_CG_EMPTY						= 79,
	HEADER_CG_GUILD							= 80,
	//HEADER_CG_EMPTY						= 81,
	HEADER_CG_FISHING						= 82,
	HEADER_CG_ITEM_GIVE						= 83,
	//HEADER_CG_EMPTY						= 84,
	//HEADER_CG_EMPTY						= 85,
	//HEADER_CG_EMPTY						= 86,
	//HEADER_CG_EMPTY						= 87,
	//HEADER_CG_EMPTY						= 88,
	//HEADER_CG_EMPTY						= 89,
	HEADER_CG_EMPIRE						= 90,
	//HEADER_CG_EMPTY						= 91,
	//HEADER_CG_EMPTY						= 92,
	//HEADER_CG_EMPTY						= 93,
	//HEADER_CG_EMPTY						= 94,
	//HEADER_CG_EMPTY						= 95,
	HEADER_CG_REFINE						= 96,
	//HEADER_CG_EMPTY						= 97,
	//HEADER_CG_EMPTY						= 98,
#ifdef ENABLE_HUNTING_SYSTEM
	HEADER_CG_SEND_HUNTING_ACTION			= 99,
#endif
	HEADER_CG_MARK_LOGIN					= 100,
	HEADER_CG_MARK_CRCLIST					= 101,
	HEADER_CG_MARK_UPLOAD					= 102,
	HEADER_CG_CRC_REPORT					= 103,
	HEADER_CG_MARK_IDXLIST					= 104,
	HEADER_CG_HACK							= 105,
	HEADER_CG_CHANGE_NAME					= 106,
	//HEADER_CG_EMPTY						= 107,
	//HEADER_CG_EMPTY						= 108,
	HEADER_CG_LOGIN2						= 109,
	HEADER_CG_DUNGEON						= 110,
	HEADER_CG_LOGIN3						= 111,
	HEADER_CG_GUILD_SYMBOL_UPLOAD			= 112,
	HEADER_CG_SYMBOL_CRC					= 113,
	HEADER_CG_SCRIPT_SELECT_ITEM			= 114,
	//HEADER_CG_EMPTY						= 115,
#ifdef ENABLE_EVENT_MANAGER
	HEADER_CG_REQUEST_EVENT_QUEST			= 116,
	HEADER_CG_REQUEST_EVENT_DATA			= 117,
#endif
#ifdef ENABLE_RENEWAL_BATTLE_PASS
	HEADER_CG_EXT_BATTLE_PASS_ACTION		= 118,
	HEADER_CG_EXT_SEND_BP_PREMIUM_ITEM		= 119,
#endif
#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	HEADER_CG_WHISPER_DETAILS				= 120,
#endif
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	HEADER_CG_ITEMS_PICKUP					= 121,
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	HEADER_CG_PET_HATCH						= 122,
	HEADER_CG_PET_WINDOW_TYPE				= 123,
	HEADER_CG_PET_WINDOW					= 124,
	HEADER_CG_PET_NAME_CHANGE				= 125,
	HEADER_CG_PET_FEED						= 126,
	HEADER_CG_PET_DETERMINE					= 127,
	HEADER_CG_PET_ATTR_CHANGE				= 128,
	HEADER_CG_PET_REVIVE					= 129,
	HEADER_CG_PET_LEARN_SKILL				= 130,
	HEADER_CG_PET_SKILL_UPGRADE				= 131,
	HEADER_CG_PET_DELETE_SKILL				= 132,
	HEADER_CG_PET_DELETE_ALL_SKILL			= 133,
#endif
	//HEADER_CG_EMPTY						= 134,
	//HEADER_CG_EMPTY						= 135,
	//HEADER_CG_EMPTY						= 136,
	//HEADER_CG_EMPTY						= 137,
	//HEADER_CG_EMPTY						= 138,
	//HEADER_CG_EMPTY						= 139,
	//HEADER_CG_EMPTY						= 140,
	//HEADER_CG_EMPTY						= 141,
	//HEADER_CG_EMPTY						= 142,
	//HEADER_CG_EMPTY						= 143,
	//HEADER_CG_EMPTY						= 144,
	//HEADER_CG_EMPTY						= 145,
	//HEADER_CG_EMPTY						= 146,
	//HEADER_CG_EMPTY						= 147,
	//HEADER_CG_EMPTY						= 148,
	//HEADER_CG_EMPTY						= 149,
	//HEADER_CG_EMPTY						= 150,
	//HEADER_CG_EMPTY						= 151,
	//HEADER_CG_EMPTY						= 152,
	//HEADER_CG_EMPTY						= 153,
	//HEADER_CG_EMPTY						= 154,
	//HEADER_CG_EMPTY						= 155,
	//HEADER_CG_EMPTY						= 156,
	//HEADER_CG_EMPTY						= 157,
	//HEADER_CG_EMPTY						= 158,
	//HEADER_CG_EMPTY						= 159,
	//HEADER_CG_EMPTY						= 160,
	//HEADER_CG_EMPTY						= 161,
	//HEADER_CG_EMPTY						= 162,
	//HEADER_CG_EMPTY						= 163,
	//HEADER_CG_EMPTY						= 164,
	//HEADER_CG_EMPTY						= 165,
	//HEADER_CG_EMPTY						= 166,
	//HEADER_CG_EMPTY						= 167,
	//HEADER_CG_EMPTY						= 168,
	//HEADER_CG_EMPTY						= 169,
	//HEADER_CG_EMPTY						= 170,
#ifdef ENABLE_RENEWAL_SWITCHBOT
	HEADER_CG_SWITCHBOT						= 171,
#endif
	//HEADER_CG_EMPTY						= 172,
	//HEADER_CG_EMPTY						= 173,
	//HEADER_CG_EMPTY						= 174,
	//HEADER_CG_EMPTY						= 175,
	//HEADER_CG_EMPTY						= 176,
	//HEADER_CG_EMPTY						= 177,
	//HEADER_CG_EMPTY						= 178,
	//HEADER_CG_EMPTY						= 179,
	//HEADER_CG_EMPTY						= 180,
	//HEADER_CG_EMPTY						= 181,
	//HEADER_CG_EMPTY						= 182,
	//HEADER_CG_EMPTY						= 183,
	//HEADER_CG_EMPTY						= 184,
	//HEADER_CG_EMPTY						= 185,
	//HEADER_CG_EMPTY						= 186,
	//HEADER_CG_EMPTY						= 187,
	//HEADER_CG_EMPTY						= 188,
	//HEADER_CG_EMPTY						= 189,
	//HEADER_CG_EMPTY						= 190,
	//HEADER_CG_EMPTY						= 191,
	//HEADER_CG_EMPTY						= 192,
	//HEADER_CG_EMPTY						= 193,
	//HEADER_CG_EMPTY						= 194,
	//HEADER_CG_EMPTY						= 195,
	//HEADER_CG_EMPTY						= 196,
	//HEADER_CG_EMPTY						= 197,
	//HEADER_CG_EMPTY						= 198,
	//HEADER_CG_EMPTY						= 199,
	//HEADER_CG_EMPTY						= 200,
#ifdef ENABLE_AUTO_SELL_SYSTEM
    HEADER_CG_AUTO_SELL_ADD 				= 201,
    HEADER_CG_AUTO_SELL_REMOVE 				= 202,
    HEADER_CG_AUTO_SELL_STATUS 				= 203,
#endif
#ifdef ENABLE_RIDING_EXTENDED
	HEADER_CG_MOUNT_UP_GRADE 				= 204,
#endif
	HEADER_CG_DRAGON_SOUL_REFINE			= 205,
	//HEADER_CG_EMPTY						= 207,
	//HEADER_CG_EMPTY						= 208,
	//HEADER_CG_EMPTY						= 209,
	//HEADER_CG_EMPTY						= 210,
	//HEADER_CG_EMPTY						= 212,
	//HEADER_CG_EMPTY						= 213,
	//HEADER_CG_EMPTY						= 214,
	//HEADER_CG_EMPTY						= 215,
	//HEADER_CG_EMPTY						= 216,
	//HEADER_CG_EMPTY						= 217,
#ifdef ENABLE_RENEWAL_CUBE
	HEADER_CG_CUBE_RENEWAL					= 218,
#endif
	//HEADER_CG_EMPTY						= 219,
	//HEADER_CG_EMPTY						= 220,
	//HEADER_CG_EMPTY						= 221,
	//HEADER_CG_EMPTY						= 222,
	//HEADER_CG_EMPTY						= 223,
	//HEADER_CG_EMPTY						= 224,
	//HEADER_CG_EMPTY						= 225,
	//HEADER_CG_EMPTY						= 226,
	//HEADER_CG_EMPTY						= 227,
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	HEADER_CG_TRANSMUTATION					= 228,
#endif
	//HEADER_CG_EMPTY						= 229,
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	HEADER_CG_CHANGE_LANGUAGE				= 230,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	HEADER_CG_AURA							= 231,
#endif
	//HEADER_CG_EMPTY						= 232,
	//HEADER_CG_EMPTY						= 233,
	//HEADER_CG_EMPTY						= 234,
	//HEADER_CG_EMPTY						= 235,
	//HEADER_CG_EMPTY						= 236,
	//HEADER_CG_EMPTY						= 237,
	//HEADER_CG_EMPTY						= 238,
	//HEADER_CG_EMPTY						= 239,
	//HEADER_CG_EMPTY						= 240,
	//HEADER_CG_EMPTY						= 241,
	//HEADER_CG_EMPTY						= 242,
	//HEADER_CG_EMPTY						= 243,
	//HEADER_CG_EMPTY						= 244,
	//HEADER_CG_EMPTY						= 245,
	//HEADER_CG_EMPTY						= 246,
	//HEADER_CG_EMPTY						= 247,
	//HEADER_CG_EMPTY						= 248,
	//HEADER_CG_EMPTY						= 249,
	//HEADER_CG_EMPTY						= 250,
	//HEADER_CG_EMPTY						= 251,
	HEADER_CG_TIME_SYNC						= 0xfc,
	//HEADER_CG_EMPTY						= 253,
	HEADER_CG_PONG							= 0xfe,
	HEADER_CG_HANDSHAKE						= 0xff,
};

enum PacketHeaderGC
{
	HEADER_GC_CHARACTER_ADD							= 1,
	HEADER_GC_CHARACTER_DEL							= 2,
	HEADER_GC_MOVE									= 3,
	HEADER_GC_CHAT									= 4,
	HEADER_GC_SYNC_POSITION							= 5,
	HEADER_GC_LOGIN_SUCCESS							= 6,
	HEADER_GC_LOGIN_FAILURE							= 7,
	HEADER_GC_CHARACTER_CREATE_SUCCESS				= 8,
	HEADER_GC_CHARACTER_CREATE_FAILURE				= 9,
	HEADER_GC_CHARACTER_DELETE_SUCCESS				= 10,
	HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID		= 11,
	HEADER_GC_ATTACK								= 12,
	HEADER_GC_STUN									= 13,
	HEADER_GC_DEAD									= 14,
	HEADER_GC_MAIN_CHARACTER_OLD					= 15,
	HEADER_GC_CHARACTER_POINTS						= 16,
	HEADER_GC_CHARACTER_POINT_CHANGE				= 17,
	HEADER_GC_CHANGE_SPEED							= 18,
	HEADER_GC_CHARACTER_UPDATE						= 19,
	HEADER_GC_ITEM_DEL								= 20,
	HEADER_GC_ITEM_SET								= 21,
	HEADER_GC_ITEM_USE								= 22,
	HEADER_GC_ITEM_DROP								= 23,
	//HEADER_GC_EMPTY								= 24,
	HEADER_GC_ITEM_UPDATE							= 25,
	HEADER_GC_ITEM_GROUND_ADD						= 26,
	HEADER_GC_ITEM_GROUND_DEL						= 27,
	HEADER_GC_QUICKSLOT_ADD							= 28,
	HEADER_GC_QUICKSLOT_DEL							= 29,
	HEADER_GC_QUICKSLOT_SWAP						= 30,
	HEADER_GC_ITEM_OWNERSHIP						= 31,
	HEADER_GC_LOGIN_SUCCESS_NEWSLOT					= 32,
	//HEADER_GC_EMPTY								= 33,
	HEADER_GC_WHISPER								= 34,
	//HEADER_GC_EMPTY								= 35,
	HEADER_GC_MOTION								= 36,
	HEADER_GC_PARTS									= 37,
	HEADER_GC_SHOP									= 38,
	HEADER_GC_SHOP_SIGN								= 39,
	HEADER_GC_DUEL_START							= 40,
	HEADER_GC_PVP									= 41,
	HEADER_GC_EXCHANGE								= 42,
	HEADER_GC_CHARACTER_POSITION					= 43,
	HEADER_GC_PING									= 44,
	HEADER_GC_SCRIPT								= 45,
	HEADER_GC_QUEST_CONFIRM							= 46,
#ifdef ENABLE_BIOLOG_SYSTEM
	HEADER_GC_BIOLOG_MANAGER						= 47,
#endif
	HEADER_GC_GUILDMARK_PASS						= 48,
#ifdef ENABLE_EVENT_MANAGER
	HEADER_GC_EVENT_INFO							= 49,
	HEADER_GC_EVENT_RELOAD							= 50,
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	HEADER_GC_OFFLINE_SHOP							= 51,
	HEADER_GC_OFFLINE_SHOP_SIGN						= 52,
#endif
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
	HEADER_GC_SHOPSEARCH_SET						= 53,
#endif
	//HEADER_GC_EMPTY								= 54,
	//HEADER_GC_EMPTY								= 55,
	//HEADER_GC_EMPTY								= 56,
	//HEADER_GC_EMPTY								= 57,
#ifdef ENABLE_MOB_DROP_INFO
	HEADER_GC_TARGET_INFO							= 58,
#endif
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
	HEADER_GC_PICKUP_ITEM_SC						= 59,
#endif
	//HEADER_GC_EMPTY								= 60,
	HEADER_GC_MOUNT									= 61,
	HEADER_GC_OWNERSHIP								= 62,
	HEADER_GC_TARGET			 					= 63,
	//HEADER_GC_EMPTY								= 64,
	HEADER_GC_WARP									= 65,
	//HEADER_GC_EMPTY								= 66,
	//HEADER_GC_EMPTY								= 67,
	//HEADER_GC_EMPTY								= 68,
	HEADER_GC_ADD_FLY_TARGETING						= 69,
	HEADER_GC_CREATE_FLY							= 70,
	HEADER_GC_FLY_TARGETING							= 71,
	HEADER_GC_SKILL_LEVEL_OLD						= 72,
	HEADER_GC_SKILL_COOLTIME_END					= 73,
	HEADER_GC_MESSENGER								= 74,
	HEADER_GC_GUILD									= 75,
	HEADER_GC_SKILL_LEVEL							= 76,
	HEADER_GC_PARTY_INVITE							= 77,
	HEADER_GC_PARTY_ADD								= 78,
	HEADER_GC_PARTY_UPDATE							= 79,
	HEADER_GC_PARTY_REMOVE							= 80,
	HEADER_GC_QUEST_INFO							= 81,
	HEADER_GC_REQUEST_MAKE_GUILD					= 82,
	HEADER_GC_PARTY_PARAMETER						= 83,
	HEADER_GC_SAFEBOX_MONEY_CHANGE					= 84,
	HEADER_GC_SAFEBOX_SET							= 85,
	HEADER_GC_SAFEBOX_DEL							= 86,
	HEADER_GC_SAFEBOX_WRONG_PASSWORD				= 87,
	HEADER_GC_SAFEBOX_SIZE							= 88,
	HEADER_GC_FISHING								= 89,
	HEADER_GC_EMPIRE								= 90,
	HEADER_GC_PARTY_LINK							= 91,
	HEADER_GC_PARTY_UNLINK							= 92,
	//HEADER_GC_EMPTY								= 93,
	//HEADER_GC_EMPTY								= 94,
	HEADER_GC_REFINE_INFORMATION_OLD				= 95,
	HEADER_GC_OBSERVER_ADD							= 96,
	HEADER_GC_OBSERVER_REMOVE						= 97,
	HEADER_GC_OBSERVER_MOVE							= 98,
	HEADER_GC_VIEW_EQUIP							= 99,
	HEADER_GC_MARK_BLOCK							= 100,
	HEADER_GC_MARK_DIFF_DATA						= 101,
	HEADER_GC_MARK_IDXLIST							= 102,
	//HEADER_GC_EMPTY								= 103,
	//HEADER_GC_EMPTY								= 104,
	//HEADER_GC_EMPTY								= 105,
	HEADER_GC_TIME									= 106,
	HEADER_GC_CHANGE_NAME							= 107,
	//HEADER_GC_EMPTY								= 108,
	//HEADER_GC_EMPTY								= 109,
	HEADER_GC_DUNGEON								= 110,
	HEADER_GC_WALK_MODE								= 111,
	HEADER_GC_SKILL_GROUP							= 112,
	HEADER_GC_MAIN_CHARACTER						= 113,
	HEADER_GC_SEPCIAL_EFFECT						= 114,
	HEADER_GC_NPC_POSITION							= 115,
	//HEADER_GC_EMPTY								= 116,
	HEADER_GC_CHARACTER_UPDATE2						= 117,
	HEADER_GC_LOGIN_KEY								= 118,
	HEADER_GC_REFINE_INFORMATION					= 119,
	HEADER_GC_CHARACTER_ADD2						= 120,
	HEADER_GC_CHANNEL								= 121,
	HEADER_GC_MALL_OPEN								= 122,
	HEADER_GC_TARGET_UPDATE							= 123,
	HEADER_GC_TARGET_DELETE							= 124,
	HEADER_GC_TARGET_CREATE							= 125,
	HEADER_GC_AFFECT_ADD							= 126,
	HEADER_GC_AFFECT_REMOVE							= 127,
	HEADER_GC_MALL_SET								= 128,
	HEADER_GC_MALL_DEL								= 129,
	HEADER_GC_LAND_LIST								= 130,
	HEADER_GC_LOVER_INFO							= 131,
	HEADER_GC_LOVE_POINT_UPDATE						= 132,
	HEADER_GC_SYMBOL_DATA							= 133,
	HEADER_GC_DIG_MOTION							= 134,
	HEADER_GC_DAMAGE_INFO							= 135,
	HEADER_GC_CHAR_ADDITIONAL_INFO					= 136,
	HEADER_GC_MAIN_CHARACTER3_BGM					= 137,
	HEADER_GC_MAIN_CHARACTER4_BGM_VOL				= 138,
#ifdef ENABLE_HUNTING_SYSTEM
	HEADER_GC_HUNTING_OPEN_MAIN						= 139,
	HEADER_GC_HUNTING_OPEN_SELECT					= 140,
	HEADER_GC_HUNTING_OPEN_REWARD					= 141,
	HEADER_GC_HUNTING_UPDATE						= 142,
	HEADER_GC_HUNTING_RECIVE_RAND_ITEMS				= 143,
#endif
	//HEADER_GC_EMPTY								= 144,
	//HEADER_GC_EMPTY								= 145,
	//HEADER_GC_EMPTY								= 146,
	//HEADER_GC_EMPTY								= 147,
	//HEADER_GC_EMPTY								= 148,
	//HEADER_GC_EMPTY								= 149,
	HEADER_GC_AUTH_SUCCESS							= 150,
#ifdef ENABLE_CLIENT_LOCALE_STRING
	HEADER_GC_LOCALE_CHAT							= 151,
	HEADER_GC_LOCALE_WHISPER						= 152,
#endif
	//HEADER_GC_EMPTY								= 153,
	//HEADER_GC_EMPTY								= 154,
#ifdef ENABLE_RIDING_EXTENDED
	HEADER_GC_MOUNT_UP_GRADE 						= 155,
	HEADER_GC_MOUNT_UP_GRADE_CHAT 					= 156,
#endif
	//HEADER_GC_EMPTY								= 157,
	//HEADER_GC_EMPTY								= 158,
	//HEADER_GC_EMPTY								= 159,
#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	HEADER_GC_WHISPER_DETAILS						= 160,
#endif
#ifdef ENABLE_GUILD_TOKEN_AUTH
	HEADER_GC_GUILD_TOKEN							= 162,
#endif
	//HEADER_GC_EMPTY								= 163,
	//HEADER_GC_EMPTY								= 164,
	//HEADER_GC_EMPTY								= 165,
	//HEADER_GC_EMPTY								= 166,
	//HEADER_GC_EMPTY								= 167,
	//HEADER_GC_EMPTY								= 168,
	//HEADER_GC_EMPTY								= 169,
	//HEADER_GC_EMPTY								= 170,
#ifdef ENABLE_RENEWAL_SWITCHBOT
	HEADER_GC_SWITCHBOT								= 171,
#endif
#ifdef ENABLE_RENEWAL_INGAME_ITEMSHOP
	HEADER_GC_ITEMSHOP								= 172,
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	HEADER_GC_PET									= 173,
	HEADER_GC_PET_SET								= 174,
	HEADER_GC_PET_SET_EXCHANGE						= 175,
	HEADER_GC_PET_DEL								= 176,
	//HEADER_GC_EMPTY								= 177,
	HEADER_GC_PET_SUMMON							= 178,
	HEADER_GC_PET_POINT_CHANGE						= 179,
	HEADER_GC_PET_NAME_CHANGE_RESULT				= 180,
	HEADER_GC_PET_DETERMINE_RESULT					= 181,
	HEADER_GC_PET_ATTR_CHANGE_RESULT				= 182,
	HEADER_GC_PET_SKILL_UPDATE						= 183,
	HEADER_GC_PET_SKILL_COOLTIME					= 184,
#endif
	//HEADER_GC_EMPTY								= 185,
	//HEADER_GC_EMPTY								= 186,
	//HEADER_GC_EMPTY								= 187,
	//HEADER_GC_EMPTY								= 188,
	//HEADER_GC_EMPTY								= 189,
	//HEADER_GC_EMPTY								= 190,
	//HEADER_GC_EMPTY								= 191,
	//HEADER_GC_EMPTY								= 192,
	//HEADER_GC_EMPTY								= 193,
	//HEADER_GC_EMPTY								= 194,
	//HEADER_GC_EMPTY								= 195,
	//HEADER_GC_EMPTY								= 196,
	//HEADER_GC_EMPTY								= 197,
	//HEADER_GC_EMPTY								= 198,
	//HEADER_GC_EMPTY								= 199,
	//HEADER_GC_EMPTY								= 200,
	//HEADER_GC_EMPTY								= 201,
	//HEADER_GC_EMPTY								= 202,
	//HEADER_GC_EMPTY								= 203,
	//HEADER_GC_EMPTY								= 204,
	//HEADER_GC_EMPTY								= 205,
	//HEADER_GC_EMPTY								= 206,
	//HEADER_GC_EMPTY								= 207,
	HEADER_GC_SPECIFIC_EFFECT						= 208,
	HEADER_GC_DRAGON_SOUL_REFINE					= 209,
	HEADER_GC_RESPOND_CHANNELSTATUS					= 210,
	//HEADER_GC_EMPTY								= 212,
	//HEADER_GC_EMPTY								= 213,
	//HEADER_GC_EMPTY								= 214,
#ifdef ENABLE_PARTY_POSITION
	HEADER_GC_PARTY_POSITION_INFO 					= 215,
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	HEADER_GC_REQUEST_CHANGE_LANGUAGE				= 216,
#endif
#ifdef ENABLE_RENEWAL_CUBE
	HEADER_GC_CUBE_RENEWAL							= 218,
#endif
#ifdef ENABLE_RENEWAL_BATTLE_PASS
	HEADER_GC_EXT_BATTLE_PASS_OPEN					= 219,
	HEADER_GC_EXT_BATTLE_PASS_GENERAL_INFO			= 220,
	HEADER_GC_EXT_BATTLE_PASS_MISSION_INFO			= 221,
	HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE		= 222,
	HEADER_GC_EXT_BATTLE_PASS_SEND_RANKING			= 223,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	HEADER_GC_AURA									= 224,
#endif
	//HEADER_GC_EMPTY								= 225,
	//HEADER_GC_EMPTY								= 226,
	//HEADER_GC_EMPTY								= 227,
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	HEADER_GC_TRANSMUTATION_SET						= 228,
	HEADER_GC_TRANSMUTATION_DEL						= 229,
	HEADER_GC_TRANSMUTATION_FREE_SET				= 230,
	HEADER_GC_TRANSMUTATION_FREE_DEL				= 231,
#endif
	//HEADER_GC_EMPTY								= 232,
	//HEADER_GC_EMPTY								= 233,
	//HEADER_GC_EMPTY								= 234,
	//HEADER_GC_EMPTY								= 235,
	//HEADER_GC_EMPTY								= 236,
	//HEADER_GC_EMPTY								= 237,
	//HEADER_GC_EMPTY								= 238,
	//HEADER_GC_EMPTY								= 239,
	//HEADER_GC_EMPTY								= 240,
	//HEADER_GC_EMPTY								= 241,
	//HEADER_GC_EMPTY								= 242,
	//HEADER_GC_EMPTY								= 243,
	//HEADER_GC_EMPTY								= 244,
	//HEADER_GC_EMPTY								= 245,
	//HEADER_GC_EMPTY								= 246,
	//HEADER_GC_EMPTY								= 247,
	//HEADER_GC_EMPTY								= 248,
	//HEADER_GC_EMPTY								= 249,
	//HEADER_GC_EMPTY								= 250,
	//HEADER_GC_EMPTY								= 251,
	HEADER_GC_TIME_SYNC								= 0xfc,
	HEADER_GC_PHASE									= 0xfd,
	HEADER_GC_BINDUDP								= 0xfe,
	HEADER_GC_HANDSHAKE								= 0xff,
};

enum PacketHeaderGG
{
	HEADER_GG_LOGIN							= 1,
	HEADER_GG_LOGOUT						= 2,
	HEADER_GG_RELAY							= 3,
	HEADER_GG_NOTICE						= 4,
	HEADER_GG_SHUTDOWN						= 5,
	HEADER_GG_GUILD							= 6,
	HEADER_GG_DISCONNECT					= 7,
	HEADER_GG_SHOUT							= 8,
	HEADER_GG_SETUP							= 9,
	HEADER_GG_MESSENGER_ADD					= 10,
	HEADER_GG_MESSENGER_REMOVE				= 11,
	HEADER_GG_FIND_POSITION					= 12,
	HEADER_GG_WARP_CHARACTER				= 13,
#ifdef ENABLE_CROSS_CHANNEL_REQUESTS
	HEADER_GG_MESSENGER_REQUEST_ADD			= 14,
#endif
	HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX		= 15,
	HEADER_GG_TRANSFER						= 16,
	HEADER_GG_XMAS_WARP_SANTA				= 17,
	HEADER_GG_XMAS_WARP_SANTA_REPLY			= 18,
	HEADER_GG_RELOAD_CRC_LIST				= 19,
	HEADER_GG_LOGIN_PING					= 20,
	HEADER_GG_BLOCK_CHAT					= 21,
	HEADER_GG_BLOCK_EXCEPTION				= 22,
	HEADER_GG_CHECK_AWAKENESS				= 23,
#ifdef ENABLE_MESSENGER_BLOCK
	HEADER_GG_MESSENGER_BLOCK_ADD			= 24,
	HEADER_GG_MESSENGER_BLOCK_REMOVE		= 25,
#endif
#ifdef ENABLE_RENEWAL_SWITCHBOT
	HEADER_GG_SWITCHBOT						= 26,
#endif
#ifdef ENABLE_EVENT_MANAGER
	HEADER_GG_EVENT_RELOAD					= 27,
	HEADER_GG_EVENT							= 28,
#endif
#ifdef ENABLE_MULTI_FARM_BLOCK
	HEADER_GG_MULTI_FARM					= 29,
#endif
#ifdef ENABLE_CLIENT_LOCALE_STRING
	HEADER_GG_LOCALE_CHAT_NOTICE			= 30,
#endif
#ifdef ENABLE_RENEWAL_REGEN
	HEADER_GG_NEW_REGEN						= 31,
#endif
};

#pragma pack(1)
typedef struct packet_markpass
{
	BYTE header;
	DWORD markpass;
} TPacketMarkPass;

typedef struct SPacketGGSetup
{
	BYTE bHeader;
	WORD wPort;
	BYTE bChannel;
} TPacketGGSetup;

typedef struct SPacketGGLogin
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD dwPID;
	BYTE bEmpire;
	long lMapIndex;
	BYTE bChannel;
#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
	BYTE bLanguage;
#endif
} TPacketGGLogin;

typedef struct SPacketGGLogout
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGLogout;

typedef struct SPacketGGRelay
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lSize;
} TPacketGGRelay;

typedef struct SPacketGGNotice
{
	BYTE bHeader;
	long lSize;
} TPacketGGNotice;

typedef struct SPacketGGShutdown
{
	BYTE bHeader;
} TPacketGGShutdown;

typedef struct SPacketGGGuild
{
	BYTE bHeader;
	BYTE bSubHeader;
	DWORD dwGuild;
} TPacketGGGuild;

enum
{
	GUILD_SUBHEADER_GG_CHAT,
	GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS,
};

typedef struct SPacketGGGuildChat
{
	BYTE bHeader;
	BYTE bSubHeader;
	DWORD dwGuild;
	char szText[CHAT_MAX_LEN + 1];
} TPacketGGGuildChat;

typedef struct SPacketGGParty
{
	BYTE header;
	BYTE subheader;
	DWORD pid;
	DWORD leaderpid;
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
	BYTE bHeader;
	char szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGDisconnect;

typedef struct SPacketGGShout
{
	BYTE bHeader;
	BYTE bEmpire;
	char szText[CHAT_MAX_LEN + 1];
#ifdef ENABLE_MESSENGER_BLOCK
	char szName[CHARACTER_NAME_MAX_LEN + 1];
#endif
} TPacketGGShout;

typedef struct SPacketGGXmasWarpSanta
{
	BYTE bHeader;
	BYTE bChannel;
	long lMapIndex;
} TPacketGGXmasWarpSanta;

typedef struct SPacketGGXmasWarpSantaReply
{
	BYTE bHeader;
	BYTE bChannel;
} TPacketGGXmasWarpSantaReply;

typedef struct SPacketGGMessenger
{
	BYTE bHeader;
	char szAccount[CHARACTER_NAME_MAX_LEN + 1];
	char szCompanion[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGMessenger;

typedef struct SPacketGGFindPosition
{
	BYTE header;
	DWORD dwFromPID;
	DWORD dwTargetPID;
} TPacketGGFindPosition;

typedef struct SPacketGGWarpCharacter
{
	BYTE header;
	DWORD pid;
	long x;
	long y;
} TPacketGGWarpCharacter;

typedef struct SPacketGGGuildWarMapIndex
{
	BYTE bHeader;
	DWORD dwGuildID1;
	DWORD dwGuildID2;
	long lMapIndex;
} TPacketGGGuildWarMapIndex;

typedef struct SPacketGGTransfer
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lX, lY;
} TPacketGGTransfer;

typedef struct SPacketGGLoginPing
{
	BYTE bHeader;
	char szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGLoginPing;

typedef struct SPacketGGBlockChat
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lBlockDuration;
} TPacketGGBlockChat;

typedef struct command_text
{
	BYTE bHeader;
} TPacketCGText;

typedef struct command_handshake
{
	BYTE bHeader;
	DWORD dwHandshake;
	DWORD dwTime;
	long lDelta;
} TPacketCGHandshake;

typedef struct command_login
{
	BYTE header;
	char login[LOGIN_MAX_LEN + 1];
	char passwd[PASSWD_MAX_LEN + 1];
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE header;
	char login[LOGIN_MAX_LEN + 1];
	DWORD dwLoginKey;
	DWORD adwClientKey[4];
} TPacketCGLogin2;

typedef struct command_login3
{
	BYTE header;
	char login[LOGIN_MAX_LEN + 1];
	char passwd[PASSWD_MAX_LEN + 1];
	DWORD adwClientKey[4];
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE bLanguage;
#endif
#ifdef ENABLE_RENEWAL_CLIENT_VERSION
	char ClientVersion[10];
#endif
} TPacketCGLogin3;

typedef struct packet_login_key
{
	BYTE bHeader;
	DWORD dwLoginKey;
} TPacketGCLoginKey;

typedef struct command_player_select
{
	BYTE header;
	BYTE index;
} TPacketCGPlayerSelect;

typedef struct command_player_delete
{
	BYTE header;
	BYTE index;
	char private_code[8];
} TPacketCGPlayerDelete;

typedef struct command_player_create
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	WORD job;
	BYTE shape;
	BYTE Con;
	BYTE Int;
	BYTE Str;
	BYTE Dex;
} TPacketCGPlayerCreate;

typedef struct command_player_create_success
{
	BYTE header;
	BYTE bAccountCharacterIndex;
	TSimplePlayer player;
} TPacketGCPlayerCreateSuccess;

typedef struct command_attack
{
	BYTE bHeader;
	BYTE bType;
	DWORD dwVID;
	BYTE bCRCMagicCubeProcPiece;
	BYTE bCRCMagicCubeFilePiece;
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
	BYTE bHeader;
	BYTE bFunc;
	BYTE bArg;
	BYTE bRot;
	long lX;
	long lY;
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

typedef struct command_chat
{
	BYTE header;
	WORD size;
	BYTE type;
} TPacketCGChat;

typedef struct command_whisper
{
	BYTE bHeader;
	WORD wSize;
	char szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

typedef struct command_entergame
{
	BYTE header;
} TPacketCGEnterGame;

typedef struct command_item_use
{
	BYTE header;
	TItemPos Cell;
} TPacketCGItemUse;

typedef struct command_item_use_to_item
{
	BYTE header;
	TItemPos Cell;
	TItemPos TargetCell;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE header;
	TItemPos Cell;
	DWORD gold;
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
	BYTE header;
	TItemPos Cell;
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
	TItemPos Cell;
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
	TItemPos Cell;
	TItemPos CellTo;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
} TPacketCGItemMove;

#ifdef ENABLE_QUICK_SELL_ITEM
typedef struct command_item_sell
{
	BYTE header;
	TItemPos Cell;
	DWORD gold;
} TPacketCGItemSell;
#endif

typedef struct command_item_pickup
{
	BYTE header;
	DWORD vid;
} TPacketCGItemPickup;

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
	TQuickslot slot;
} TPacketCGQuickslotAdd;

typedef struct command_quickslot_del
{
	BYTE header;
	BYTE pos;
} TPacketCGQuickslotDel;

typedef struct command_quickslot_swap
{
	BYTE header;
	BYTE pos;
	BYTE change_pos;
} TPacketCGQuickslotSwap;

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
	SHOP_SUBHEADER_CG_OPEN_WITH_VID,
#endif
};

typedef struct command_shop_buy
{
	BYTE count;
} TPacketCGShopBuy;

typedef struct command_shop_sell
{
	BYTE pos;
	BYTE count;
} TPacketCGShopSell;

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

typedef struct command_on_click
{
	BYTE header;
	DWORD vid;
} TPacketCGOnClick;

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
	BYTE sub_header;
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

typedef struct command_quest_input_string
{
	BYTE header;
	char msg[64+1];
} TPacketCGQuestInputString;

#ifdef ENABLE_RENEWAL_OX_EVENT
typedef struct command_quest_input_long_string
{
	BYTE header;
	char msg[128 + 1];
} TPacketCGQuestInputLongString;
#endif

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
	long timeout;
	DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_handshake
{
	BYTE bHeader;
	DWORD dwHandshake;
	DWORD dwTime;
	long lDelta;
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
	BYTE header;
	BYTE phase;
} TPacketGCPhase;

typedef struct packet_bindudp
{
	BYTE header;
	DWORD addr;
	WORD port;
} TPacketGCBindUDP;

enum
{
	LOGIN_FAILURE_ALREADY = 1,
	LOGIN_FAILURE_ID_NOT_EXIST = 2,
	LOGIN_FAILURE_WRONG_PASS = 3,
	LOGIN_FAILURE_FALSE = 4,
	LOGIN_FAILURE_NOT_TESTOR = 5,
	LOGIN_FAILURE_NOT_TEST_TIME = 6,
	LOGIN_FAILURE_FULL = 7
};

typedef struct packet_login_success
{
	BYTE bHeader;
	TSimplePlayer players[PLAYER_PER_ACCOUNT];
	DWORD guild_id[PLAYER_PER_ACCOUNT];
	char guild_name[PLAYER_PER_ACCOUNT][GUILD_NAME_MAX_LEN+1];
	DWORD handle;
	DWORD random_key;
} TPacketGCLoginSuccess;

typedef struct packet_auth_success
{
	BYTE bHeader;
	DWORD dwLoginKey;
	BYTE bResult;
} TPacketGCAuthSuccess;

typedef struct packet_login_failure
{
	BYTE header;
	char szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
} TPacketGCLoginFailure;

typedef struct packet_create_failure
{
	BYTE header;
	BYTE bType;
} TPacketGCCreateFailure;

enum
{
	ADD_CHARACTER_STATE_DEAD = (1 << 0),
	ADD_CHARACTER_STATE_SPAWN = (1 << 1),
	ADD_CHARACTER_STATE_GUNGON = (1 << 2),
	ADD_CHARACTER_STATE_KILLER = (1 << 3),
	ADD_CHARACTER_STATE_PARTY = (1 << 4),
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
	DWORD dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	BYTE dwGuildLeader;
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	BYTE bCharacterSize;
#endif
} TPacketGCCharacterAdditionalInfo;

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
	DWORD dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + ESkillColorLength::MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	BYTE dwGuildLeader;
#endif
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE header;
	DWORD id;
} TPacketGCCharacterDelete;

typedef struct packet_chat
{
	BYTE header;
	WORD size;
	BYTE type;
	DWORD id;
	BYTE bEmpire;
} TPacketGCChat;

typedef struct packet_whisper
{
	BYTE bHeader;
	WORD wSize;
	BYTE bType;
	char szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCWhisper;

typedef struct packet_main_character
{
	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lx, ly, lz;
	BYTE empire;
	BYTE skill_group;
} TPacketGCMainCharacter;

typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char szBGMName[MUSIC_NAME_LEN + 1];
	long lx, ly, lz;
	BYTE empire;
	BYTE skill_group;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE header;
	DWORD dwVID;
	WORD wRaceNum;
	char szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char szBGMName[MUSIC_NAME_LEN + 1];
	float fBGMVol;
	long lx, ly, lz;
	BYTE empire;
	BYTE skill_group;
} TPacketGCMainCharacter4_BGM_VOL;

typedef struct packet_points
{
	BYTE header;
#ifdef ENABLE_GOLD_LIMIT
	long long points[POINT_MAX_NUM];
#else
	INT points[POINT_MAX_NUM];
#endif
} TPacketGCPoints;

typedef struct packet_skill_level
{
	BYTE bHeader;
	TPlayerSkill skills[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

typedef struct packet_point_change
{
	int header;
	DWORD dwVID;
	BYTE type;
#ifdef ENABLE_GOLD_LIMIT
	long long amount;
	long long value;
#else
	long amount;
	long value;
#endif
} TPacketGCPointChange;

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
	GUILD_RANKING_MAX_NUM = 100,
	HEADER_GC_GUILD_RANKING = 161,
};

typedef struct packet_guild_ranking_send
{
	packet_guild_ranking_send()
	{
		std::strncpy(szOwnerName, "", sizeof(szOwnerName));
		std::strncpy(szGuildName, "", sizeof(szGuildName));
	}

	packet_guild_ranking_send(int32_t c_id, const char* c_guild, const char* c_name, int32_t c_level, int32_t c_point, int32_t c_win, int32_t c_draw, int32_t c_loss)
		: subheader(GUILD_RANK::SUBHEADER_GUILD_RANKING_SEND), id(c_id), level(c_level), point(c_point), win(c_win), draw(c_draw), loss(c_loss)
	{
		std::strncpy(szOwnerName, c_name, sizeof(szOwnerName));
		std::strncpy(szGuildName, c_guild, sizeof(szGuildName));
	}

	uint8_t header = HEADER_GC_GUILD_RANKING;
	uint8_t subheader = SUBHEADER_GUILD_RANKING_OPEN;
	int32_t id = 0;
	char szGuildName[GUILD_NAME_MAX_LEN + 1];
	char szOwnerName[CHARACTER_NAME_MAX_LEN + 1];
	int32_t level = 0;
	int32_t point = 0;
	int32_t win = 0;
	int32_t draw = 0;
	int32_t loss = 0;
} TPacketGCGuildRankingSend;
#endif

struct TPacketGCItemDelDeprecated
{
	BYTE header;
	TItemPos Cell;
	DWORD vnum;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
};

typedef struct packet_item_set
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
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
} TPacketGCItemSet;

typedef struct packet_item_del
{
	BYTE header;
#ifdef ENABLE_EXTENDED_SAFEBOX
	DWORD pos;
#else
	BYTE pos;
#endif
} TPacketGCItemDel;

struct packet_item_use
{
	BYTE header;
	TItemPos Cell;
	DWORD ch_vid;
	DWORD victim_vid;
	DWORD vnum;
};

struct packet_item_move
{
	BYTE header;
	TItemPos Cell;
	TItemPos CellTo;
};

typedef struct packet_item_update
{
	BYTE header;
	TItemPos Cell;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
} TPacketGCItemUpdate;

typedef struct packet_item_ground_add
{
	BYTE bHeader;
	long x, y, z;
#ifdef ENABLE_RENEWAL_BOOK_NAME
	long lSocket0;
#endif
	DWORD dwVID;
	DWORD dwVnum;
} TPacketGCItemGroundAdd;

typedef struct packet_item_ownership
{
	BYTE bHeader;
	DWORD dwVID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

typedef struct packet_item_ground_del
{
	BYTE bHeader;
	DWORD dwVID;
} TPacketGCItemGroundDel;

struct packet_quickslot_add
{
	BYTE header;
	BYTE pos;
	TQuickslot slot;
};

struct packet_quickslot_del
{
	BYTE header;
	BYTE pos;
};

struct packet_quickslot_swap
{
	BYTE header;
	BYTE pos;
	BYTE pos_to;
};

struct packet_motion
{
	BYTE header;
	DWORD vid;
	DWORD victim_vid;
	WORD motion;
};

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

struct packet_shop_item
{
	DWORD vnum;
#ifdef ENABLE_GOLD_LIMIT
	long long price;
#else
	DWORD price;
#endif

#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
	BYTE display_pos;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];

#ifdef ENABLE_RENEWAL_SHOPEX
	BYTE price_type;
	DWORD price_vnum;

	packet_shop_item() : price_type(SHOPEX_GOLD), price_vnum(0)
	{
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttr, 0, sizeof(aAttr));
	}
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
};

typedef struct packet_shop_start
{
	DWORD owner_vid;
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
	long long iPrice;
#else
	int iPrice;
#endif
} TPacketGCShopUpdatePrice;

typedef struct packet_shop
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCShop;

struct packet_exchange
{
	BYTE header;
	BYTE sub_header;
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
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
};

enum EPacketTradeSubHeaders
{
	EXCHANGE_SUBHEADER_GC_START,
	EXCHANGE_SUBHEADER_GC_ITEM_ADD,
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,
	EXCHANGE_SUBHEADER_GC_GOLD_ADD,
	EXCHANGE_SUBHEADER_GC_ACCEPT,
	EXCHANGE_SUBHEADER_GC_END,
	EXCHANGE_SUBHEADER_GC_ALREADY,
	EXCHANGE_SUBHEADER_GC_LESS_GOLD,
};

struct packet_position
{
	BYTE header;
	DWORD vid;
	BYTE position;
};

typedef struct packet_ping
{
	BYTE header;
} TPacketGCPing;

struct packet_script
{
	BYTE header;
	WORD size;
	BYTE skin;
	WORD src_size;
};

typedef struct packet_change_speed
{
	BYTE header;
	DWORD vid;
	WORD moving_speed;
} TPacketGCChangeSpeed;

struct packet_mount
{
	BYTE header;
	DWORD vid;
	DWORD mount_vid;
	BYTE pos;
	DWORD x, y;
};

typedef struct packet_move
{
	BYTE bHeader;
	BYTE bFunc;
	BYTE bArg;
	BYTE bRot;
	DWORD dwVID;
	long lX;
	long lY;
	DWORD dwTime;
	DWORD dwDuration;
} TPacketGCMove;

typedef struct packet_ownership
{
	BYTE bHeader;
	DWORD dwOwnerVID;
	DWORD dwVictimVID;
} TPacketGCOwnership;

typedef struct packet_sync_position_element
{
	DWORD dwVID;
	long lX;
	long lY;
} TPacketGCSyncPositionElement;

typedef struct packet_sync_position
{
	BYTE bHeader;
	WORD wSize;
} TPacketGCSyncPosition;

typedef struct packet_fly
{
	BYTE bHeader;
	BYTE bType;
	DWORD dwStartVID;
	DWORD dwEndVID;
} TPacketGCCreateFly;

typedef struct command_fly_targeting
{
	BYTE bHeader;
	DWORD dwTargetVID;
	long x, y;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
	BYTE bHeader;
	DWORD dwShooterVID;
	DWORD dwTargetVID;
	long x, y;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
	BYTE bHeader;
	BYTE bType;
} TPacketCGShoot;

typedef struct packet_duel_start
{
	BYTE header;
	WORD wSize;
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
	BYTE bHeader;
	DWORD dwVIDSrc;
	DWORD dwVIDDst;
	BYTE bMode;
} TPacketGCPVP;

typedef struct command_use_skill
{
	BYTE bHeader;
	DWORD dwVnum;
	DWORD dwVID;
} TPacketCGUseSkill;

typedef struct command_target
{
	BYTE header;
	DWORD dwVID;
} TPacketCGTarget;

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

typedef struct packet_warp
{
	BYTE bHeader;
	long lX;
	long lY;
	long lAddr;
	WORD wPort;
} TPacketGCWarp;

typedef struct command_warp
{
	BYTE bHeader;
} TPacketCGWarp;

struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
#ifdef ENABLE_RENEWAL_QUEST
	WORD c_index;
#endif
	BYTE flag;
};

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
	MESSENGER_SUBHEADER_GC_INVITE
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
} TPacketGCMessengerGuildList;

typedef struct packet_messenger_guild_login
{
	BYTE length;
} TPacketGCMessengerGuildLogin;

typedef struct packet_messenger_guild_logout
{
	BYTE length;
} TPacketGCMessengerGuildLogout;

typedef struct packet_messenger_list_offline
{
	BYTE connected;
	BYTE length;
} TPacketGCMessengerListOffline;

typedef struct packet_messenger_list_online
{
	BYTE connected;
	BYTE length;
} TPacketGCMessengerListOnline;

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
	MESSENGER_SUBHEADER_CG_INVITE_ANSWER,
};

typedef struct command_messenger
{
	BYTE header;
	BYTE subheader;
} TPacketCGMessenger;

#ifdef ENABLE_MESSENGER_BLOCK
typedef struct command_messenger_add_block_by_vid
{
	DWORD vid;
} TPacketCGMessengerAddBlockByVID;

typedef struct command_messenger_add_block_by_name
{
	BYTE length;
} TPacketCGMessengerAddBlockByName;

typedef struct command_messenger_remove_block
{
	char login[LOGIN_MAX_LEN+1];
} TPacketCGMessengerRemoveBlock;
#endif

typedef struct command_messenger_add_by_vid
{
	DWORD vid;
} TPacketCGMessengerAddByVID;

typedef struct command_messenger_add_by_name
{
	BYTE length;
} TPacketCGMessengerAddByName;

typedef struct command_messenger_remove
{
	char login[LOGIN_MAX_LEN+1];
} TPacketCGMessengerRemove;

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

typedef struct command_party_parameter
{
	BYTE bHeader;
	BYTE bDistributeMode;
} TPacketCGPartyParameter;

typedef struct paryt_parameter
{
	BYTE bHeader;
	BYTE bDistributeMode;
} TPacketGCPartyParameter;

typedef struct packet_party_add
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN+1];
} TPacketGCPartyAdd;

typedef struct command_party_invite
{
	BYTE header;
	DWORD vid;
} TPacketCGPartyInvite;

typedef struct packet_party_invite
{
	BYTE header;
	DWORD leader_vid;
} TPacketGCPartyInvite;

typedef struct command_party_invite_answer
{
	BYTE header;
	DWORD leader_vid;
	BYTE accept;
} TPacketCGPartyInviteAnswer;

typedef struct packet_party_update
{
	BYTE header;
	DWORD pid;
	BYTE role;
	BYTE percent_hp;
	short affects[7];
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
	BYTE bHeader;
} TPacketCGSafeboxWrongPassword;

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
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
	BYTE bHeader;
	BYTE bState;
	long lMoney;
} TPacketCGSafeboxMoney;

typedef struct packet_safebox_money_change
{
	BYTE bHeader;
	long lMoney;
} TPacketGCSafeboxMoneyChange;

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
	DWORD guildID;
	char guildName[GUILD_NAME_MAX_LEN];
} TPacketGCGuildName;

typedef struct packet_guild_war
{
	DWORD dwGuildSelf;
	DWORD dwGuildOpp;
	BYTE bType;
	BYTE bWarState;
} TPacketGCGuildWar;

typedef struct command_guild
{
	BYTE header;
	BYTE subheader;
} TPacketCGGuild;

typedef struct command_guild_use_skill
{
	DWORD dwVnum;
	DWORD dwPID;
} TPacketCGGuildUseSkill;

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
	DWORD guild_id;
	DWORD markpass;
#ifdef ENABLE_GUILD_TOKEN_AUTH
	uint64_t token;
#endif
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
	long x;
	long y;
} TPacketGCDungeonDestPosition;

#ifdef ENABLE_RENEWAL_OFFLINESHOP
typedef struct packet_offline_shop_item
{
	DWORD id;
	DWORD vnum;
	long long price;
	int count;
	BYTE display_pos;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	BYTE status;
	char szBuyerName[CHARACTER_NAME_MAX_LEN+1];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD transmutation;
#endif
	DWORD owner_id;
} TOfflineShopItem;

typedef struct SPacketGCShopSign
{
	BYTE bHeader;
	DWORD dwVID;
	char szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;

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
	TOfflineShopItem items[OFFLINE_SHOP_HOST_ITEM_MAX_NUM];
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
	TOfflineShopItem item;
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

typedef struct SPacketCGRefine
{
	BYTE header;
	BYTE pos;
	BYTE type;
} TPacketCGRefine;

typedef struct SPacketCGRequestRefineInfo
{
	BYTE header;
	BYTE pos;
} TPacketCGRequestRefineInfo;

typedef struct SPacketGCRefineInformaion
{
	BYTE header;
	BYTE type;
	BYTE pos;
	DWORD src_vnum;
	DWORD result_vnum;
#ifdef ENABLE_STACK_LIMIT
	WORD material_count;
#else
	BYTE material_count;
#endif
	int cost;
	int prob;
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TPacketGCRefineInformation;

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

typedef struct SPacketGCNPCPosition
{
	BYTE header;
	WORD size;
	WORD count;
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

typedef struct packet_channel
{
	BYTE header;
	BYTE channel;
#ifdef ENABLE_ANTI_EXP
	bool anti_exp;
#endif
} TPacketGCChannel;

typedef struct pakcet_view_equip
{
	BYTE header;
	DWORD vid;
	struct
	{
		DWORD vnum;
#ifdef ENABLE_STACK_LIMIT
		WORD count;
#else
		BYTE count;
#endif
		long alSockets[ITEM_SOCKET_MAX_NUM];
		TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		DWORD dwTransmutationVnum;
#endif
	} equips[WEAR_MAX_NUM];
} TPacketViewEquip;

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
	char szName[32+1];
	DWORD dwVID;
	BYTE bType;
} TPacketGCTargetCreate;

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

typedef struct packet_dig_motion
{
	BYTE header;
	DWORD vid;
	DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;

typedef struct command_script_select_item
{
	BYTE header;
	DWORD selection;
} TPacketCGScriptSelectItem;

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

typedef struct SPacketGGCheckAwakeness
{
	BYTE bHeader;
} TPacketGGCheckAwakeness;

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
};

typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header (HEADER_CG_DRAGON_SOUL_REFINE) {}
	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DRAGON_SOUL_REFINE_GRID_SIZE];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE) {}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;

typedef struct SPacketCGStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
} TPacketCGStateCheck;

typedef struct SPacketGCStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
	unsigned char state;
} TPacketGCStateCheck;

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
struct TPacketGGSwitchbot
{
	BYTE bHeader;
	WORD wPort;
	TSwitchbotTable table;
	TPacketGGSwitchbot() : bHeader(HEADER_GG_SWITCHBOT), wPort(0) { table = {}; }
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
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
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
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
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

typedef struct SPacketGGReloadEvent
{
	BYTE bHeader;
} TPacketGGReloadEvent;

typedef struct SPacketGGEvent
{
	BYTE bHeader;
	TEventTable table;
	bool bState;
} TPacketGGEvent;

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

#ifdef ENABLE_CROSS_CHANNEL_REQUESTS
typedef struct SPacketGGMessengerRequest
{
	BYTE header;
	char account[CHARACTER_NAME_MAX_LEN + 1];
	char target[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGMessengerRequest;
#endif

#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
typedef struct pickup_item_packet
{
	BYTE bHeader;
	int item_vnum;
} TPacketGCPickupItemSC;
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
enum
{
	MULTI_FARM_SET,
	MULTI_FARM_REMOVE,
};

typedef struct SPacketGGMultiFarm
{
	BYTE header;
	DWORD size;
	BYTE subHeader;
	char playerIP[IP_ADDRESS_LENGTH + 1];
	DWORD playerID;
	char playerName[CHARACTER_NAME_MAX_LEN+1];
	bool farmStatus;
	BYTE affectType;
	int affectTime;
} TPacketGGMultiFarm;
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

typedef struct packet_locale_chat_notice
{
	BYTE header;
	WORD size;
	BYTE type;
	DWORD id;
	BYTE empire;
	long mapidx;
} TPacketGGLocaleChatNotice;

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
typedef struct SPacketGCItemShop
{
	BYTE header;
	DWORD size;
} TPacketGCItemShop;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
typedef struct SItemData
{
	DWORD vnum;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
	DWORD flags;
	DWORD anti_flags;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TItemData;

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

typedef struct SPacketGCPetSet
{
	BYTE header;
	DWORD dwID;
	DWORD dwSummonItemVnum;
	char szName[PET_NAME_MAX_SIZE + 1];
	TPetSkill aSkill[PET_SKILL_COUNT_MAX];
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
	char szName[PET_NAME_MAX_SIZE + 1];
} TPacketGCPetNameChangeResult;

typedef struct SPacketCGPetHatch
{
	BYTE header;
	char name[PET_NAME_MAX_SIZE + 1];
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
	char name[PET_NAME_MAX_SIZE + 1];
	TItemPos changeNamePos;
	TItemPos upBringingPos;
} TPacketCGPetNameChange;
#endif

#ifdef ENABLE_RENEWAL_REGEN
typedef struct SGGPacketNewRegen
{
	BYTE header;
	BYTE subHeader;
	WORD id;
	bool isAlive;
} TGGPacketNewRegen;

enum
{
	NEW_REGEN_LOAD,
	NEW_REGEN_REFRESH,
};
#endif

#ifdef ENABLE_GUILD_TOKEN_AUTH
struct TPacketGCGuildToken
{
	uint8_t header;
	uint64_t token;
};
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
enum class ECG_TRANSMUTATION_SHEADER : BYTE
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
typedef struct packet_auto_sell_add
{
	BYTE		header;
	DWORD		vnum;
} TPacketCGAutoSellAdd;

typedef struct packet_auto_sell_remove
{
	BYTE		header;
	DWORD		vnum;
} TPacketCGAutoSellRemove;

typedef struct packet_auto_sell_status
{
	BYTE		header;
	BYTE		status;
} TPacketCGAutoSellStatus;
#endif

#pragma pack()
#endif
