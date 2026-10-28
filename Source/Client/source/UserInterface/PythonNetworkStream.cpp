#include "StdAfx.h"
#include "../EterLib/NetPacketHeaderMap.h"

#include "PythonNetworkStream.h"
#include "Packet.h"
#include "NetworkActorManager.h"
#include "GuildMarkDownloader.h"
#include "GuildMarkUploader.h"
#include "MarkManager.h"
#include "ProcessCRC.h"

// MARK_BUG_FIX
static DWORD gs_nextDownloadMarkTime = 0;
// END_OF_MARK_BUG_FIX

#ifdef ENABLE_LARGE_DYNAMIC_PACKET //@force_add
enum
{
	STATIC_SIZE_PACKET = 0,
	DYNAMIC_SIZE_PACKET,
	LARGE_DYNAMIC_SIZE_PACKET,
};
#endif

// Packet ---------------------------------------------------------------------------
class CMainPacketHeaderMap : public CNetworkPacketHeaderMap
{
#ifndef ENABLE_LARGE_DYNAMIC_PACKET
	public:
		enum
		{
			STATIC_SIZE_PACKET = false,
			DYNAMIC_SIZE_PACKET = true,
		};
#endif

	public:
		CMainPacketHeaderMap()
		{
			Set(HEADER_GC_EMPIRE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCEmpire), STATIC_SIZE_PACKET));
			Set(HEADER_GC_WARP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCWarp), STATIC_SIZE_PACKET));
			Set(HEADER_GC_SKILL_COOLTIME_END, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSkillCoolTimeEnd), STATIC_SIZE_PACKET));
			Set(HEADER_GC_QUEST_INFO, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCQuestInfo), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_REQUEST_MAKE_GUILD, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCBlank), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PVP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPVP), STATIC_SIZE_PACKET));
			Set(HEADER_GC_DUEL_START, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCDuelStart), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_CHARACTER_ADD, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCCharacterAdd), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHAR_ADDITIONAL_INFO, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCCharacterAdditionalInfo), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHARACTER_ADD2, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCCharacterAdd2), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHARACTER_UPDATE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCCharacterUpdate), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHARACTER_UPDATE2, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCCharacterUpdate2), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHARACTER_DEL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCCharacterDelete), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHARACTER_MOVE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMove), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHAT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCChat), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_SYNC_POSITION, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCC2C), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_LOGIN_SUCCESS3, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCLoginSuccess3), STATIC_SIZE_PACKET));
			Set(HEADER_GC_LOGIN_SUCCESS4, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCLoginSuccess4), STATIC_SIZE_PACKET));
			Set(HEADER_GC_LOGIN_FAILURE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCLoginFailure), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PLAYER_CREATE_SUCCESS, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPlayerCreateSuccess), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PLAYER_CREATE_FAILURE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCCreateFailure), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PLAYER_DELETE_SUCCESS, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCBlank), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PLAYER_DELETE_WRONG_SOCIAL_ID, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCBlank), STATIC_SIZE_PACKET));
			Set(HEADER_GC_STUN, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCStun), STATIC_SIZE_PACKET));
			Set(HEADER_GC_DEAD, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCDead), STATIC_SIZE_PACKET));
			Set(HEADER_GC_MAIN_CHARACTER, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMainCharacter), STATIC_SIZE_PACKET));
			Set(HEADER_GC_MAIN_CHARACTER2_EMPIRE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMainCharacter2_EMPIRE), STATIC_SIZE_PACKET));
			Set(HEADER_GC_MAIN_CHARACTER3_BGM, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMainCharacter3_BGM), STATIC_SIZE_PACKET));
			Set(HEADER_GC_MAIN_CHARACTER4_BGM_VOL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMainCharacter4_BGM_VOL), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PLAYER_POINTS, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPoints), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PLAYER_POINT_CHANGE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPointChange), STATIC_SIZE_PACKET));
			/* Server sends point change as 170 (alternate enum); same as PLAYER_POINT_CHANGE */
			Set(170, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPointChange), STATIC_SIZE_PACKET));
			/* Server sends header 144 (e.g. alternate enum); accept and skip 1 byte */
			Set(144, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCBlank), STATIC_SIZE_PACKET));
			/* Server sends header 195 (alternate enum); accept and skip 1 byte */
			Set(195, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCBlank), STATIC_SIZE_PACKET));
			/* Server sends header 33 (alternate enum); accept and skip 1 byte */
			Set(33, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCBlank), STATIC_SIZE_PACKET));
			Set(HEADER_GC_ITEM_SET, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemSet), STATIC_SIZE_PACKET));
			Set(HEADER_GC_ITEM_SET2, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemSet2), STATIC_SIZE_PACKET));
			Set(HEADER_GC_ITEM_USE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemUse), STATIC_SIZE_PACKET));
			Set(HEADER_GC_ITEM_UPDATE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemUpdate), STATIC_SIZE_PACKET));
			Set(HEADER_GC_ITEM_GROUND_ADD, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemGroundAdd), STATIC_SIZE_PACKET));
			Set(HEADER_GC_ITEM_GROUND_DEL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemGroundDel), STATIC_SIZE_PACKET));
			Set(HEADER_GC_ITEM_OWNERSHIP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemOwnership), STATIC_SIZE_PACKET));
			Set(HEADER_GC_QUICKSLOT_ADD, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCQuickSlotAdd), STATIC_SIZE_PACKET));
			Set(HEADER_GC_QUICKSLOT_DEL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCQuickSlotDel), STATIC_SIZE_PACKET));
			Set(HEADER_GC_QUICKSLOT_SWAP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCQuickSlotSwap), STATIC_SIZE_PACKET));
			Set(HEADER_GC_WHISPER, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCWhisper), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHARACTER_POSITION, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPosition), STATIC_SIZE_PACKET));
			Set(HEADER_GC_MOTION, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMotion), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PARTS, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCCharacterUpdate), STATIC_SIZE_PACKET));
			Set(HEADER_GC_SHOP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCShop), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_SHOP_SIGN, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCShopSign), STATIC_SIZE_PACKET));
			Set(HEADER_GC_EXCHANGE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCExchange), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PING, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPing), STATIC_SIZE_PACKET));
			Set(HEADER_GC_SCRIPT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCScript), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_QUEST_CONFIRM, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCQuestConfirm), STATIC_SIZE_PACKET));
			Set(HEADER_GC_TARGET, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCTarget), STATIC_SIZE_PACKET));
			Set(HEADER_GC_MOUNT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMount), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHANGE_SPEED, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCChangeSpeed), STATIC_SIZE_PACKET));
			Set(HEADER_GC_HANDSHAKE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCHandshake), STATIC_SIZE_PACKET));
			Set(HEADER_GC_HANDSHAKE_OK, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCBlank), STATIC_SIZE_PACKET));
			Set(HEADER_GC_BINDUDP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCBindUDP), STATIC_SIZE_PACKET));
			Set(HEADER_GC_OWNERSHIP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCOwnership), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CREATE_FLY, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCCreateFly), STATIC_SIZE_PACKET));
			Set(HEADER_GC_ADD_FLY_TARGETING, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCFlyTargeting), STATIC_SIZE_PACKET));
			Set(HEADER_GC_FLY_TARGETING, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCFlyTargeting), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PHASE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPhase), STATIC_SIZE_PACKET));
			Set(HEADER_GC_SKILL_LEVEL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSkillLevel), STATIC_SIZE_PACKET));
			Set(HEADER_GC_SKILL_LEVEL_NEW, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSkillLevelNew), STATIC_SIZE_PACKET));
			Set(HEADER_GC_MESSENGER, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMessenger), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_GUILD, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCGuild), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_PARTY_INVITE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPartyInvite), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PARTY_ADD, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPartyAdd), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PARTY_UPDATE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPartyUpdate), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PARTY_REMOVE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPartyRemove), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PARTY_LINK, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPartyLink), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PARTY_UNLINK, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPartyUnlink), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PARTY_PARAMETER, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPartyParameter), STATIC_SIZE_PACKET));

#ifdef ENABLE_ITEMSHOP
#ifdef ENABLE_LARGE_DYNAMIC_PACKETS
			Set(HEADER_GC_ITEMSHOP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemShop), LARGE_DYNAMIC_SIZE_PACKET));
#else
			Set(HEADER_GC_ITEMSHOP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemShop), DYNAMIC_SIZE_PACKET));
#endif
#endif
			Set(HEADER_GC_SAFEBOX_SET, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemSet2), STATIC_SIZE_PACKET));
			Set(HEADER_GC_SAFEBOX_DEL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemDel), STATIC_SIZE_PACKET));
			Set(HEADER_GC_SAFEBOX_WRONG_PASSWORD, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSafeboxWrongPassword), STATIC_SIZE_PACKET));
			Set(HEADER_GC_SAFEBOX_SIZE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSafeboxSize), STATIC_SIZE_PACKET));
			Set(HEADER_GC_SAFEBOX_MONEY_CHANGE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSafeboxMoneyChange), STATIC_SIZE_PACKET));
#ifdef ENABLE_UN_SEAL_SCROLL_PLUS
			Set(HEADER_GC_SAFEBOX_PASSWORD_SUCCESS, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSafeboxPasswordSuccess), STATIC_SIZE_PACKET));
#endif
			Set(HEADER_GC_FISHING, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCFishing), STATIC_SIZE_PACKET));
			Set(HEADER_GC_DUNGEON, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCDungeon), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_TIME, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCTime), STATIC_SIZE_PACKET));
			Set(HEADER_GC_WALK_MODE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCWalkMode), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHANGE_SKILL_GROUP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCChangeSkillGroup), STATIC_SIZE_PACKET));
			Set(HEADER_GC_REFINE_INFORMATION, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCRefineInformation), STATIC_SIZE_PACKET));
			Set(HEADER_GC_REFINE_INFORMATION_NEW, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCRefineInformationNew), STATIC_SIZE_PACKET));
			Set(HEADER_GC_SEPCIAL_EFFECT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSpecialEffect), STATIC_SIZE_PACKET));
			Set(HEADER_GC_NPC_POSITION, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCNPCPosition), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_CHANGE_NAME, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCChangeName), STATIC_SIZE_PACKET));
			Set(HEADER_GC_LOGIN_KEY, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCLoginKey), STATIC_SIZE_PACKET));
			Set(HEADER_GC_AUTH_SUCCESS, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCAuthSuccess), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHANNEL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCChannel), STATIC_SIZE_PACKET));
			Set(HEADER_GC_VIEW_EQUIP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCViewEquip), STATIC_SIZE_PACKET));
			Set(HEADER_GC_LAND_LIST, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCLandList), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_TARGET_UPDATE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCTargetUpdate), STATIC_SIZE_PACKET));
			Set(HEADER_GC_TARGET_DELETE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCTargetDelete), STATIC_SIZE_PACKET));
			Set(HEADER_GC_TARGET_CREATE_NEW, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCTargetCreateNew), STATIC_SIZE_PACKET));
			Set(HEADER_GC_AFFECT_ADD, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCAffectAdd), STATIC_SIZE_PACKET));
			Set(HEADER_GC_AFFECT_REMOVE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCAffectRemove), STATIC_SIZE_PACKET));
			Set(HEADER_GC_MALL_OPEN, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMallOpen), STATIC_SIZE_PACKET));
			Set(HEADER_GC_MALL_SET, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemSet2), STATIC_SIZE_PACKET));
			Set(HEADER_GC_MALL_DEL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCItemDel), STATIC_SIZE_PACKET));
			Set(HEADER_GC_LOVER_INFO, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCLoverInfo), STATIC_SIZE_PACKET));
			Set(HEADER_GC_LOVE_POINT_UPDATE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCLovePointUpdate), STATIC_SIZE_PACKET));
			Set(HEADER_GC_DIG_MOTION, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCDigMotion), STATIC_SIZE_PACKET));
			Set(HEADER_GC_DAMAGE_INFO, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCDamageInfo), STATIC_SIZE_PACKET));
			Set(HEADER_GC_SPECIFIC_EFFECT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSpecificEffect), STATIC_SIZE_PACKET));
#ifdef ENABLE_BUFFI_SYSTEM
			Set(HEADER_GC_BUFFI_USE_SKILL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCBuffiUseSkill), STATIC_SIZE_PACKET));
#endif
			Set(HEADER_GC_DRAGON_SOUL_REFINE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCDragonSoulRefine), STATIC_SIZE_PACKET));
#ifdef ENABLE_FISH_EVENT_SYSTEM
			Set(HEADER_GC_FISH_EVENT_INFO, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCFishEventInfo), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_GUILD_RANK_SYSTEM
			Set(HEADER_GC_GUILD_RANKING, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCGuildRankingSend), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_PARTY_POSITION
			Set(HEADER_GC_PARTY_POSITION_INFO,	CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPartyPosition), DYNAMIC_SIZE_PACKET));
#endif
#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
			Set(HEADER_GC_SOUL_ROULETTE,		CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSoulRoulette), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			Set(HEADER_GC_REQUEST_CHANGE_LANGUAGE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketChangeLanguage), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
			Set(HEADER_GC_WHISPER_DETAILS, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCWhisperDetails), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_COLLECT_WINDOW
			Set(HEADER_GC_COLLECT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCCollectWindow), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_RESP_SYSTEM
			Set(HEADER_GC_RESP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCRespHeader), DYNAMIC_SIZE_PACKET));
#endif
#ifdef ENABLE_RENEWAL_SWITCHBOT
			Set(HEADER_GC_SWITCHBOT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSwitchbot), DYNAMIC_SIZE_PACKET));
#endif
#ifdef ENABLE_RENEWAL_CUBE
			Set(HEADER_GC_CUBE_RENEWAL,	CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCCubeRenewalReceive), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			Set(HEADER_GC_ACCE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketAcce), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_MOB_DROP_INFO
			Set(HEADER_GC_TARGET_INFO, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCTargetInfo), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_VIEW_CHEST_DROP
			Set(HEADER_GC_CHEST_DROP_INFO, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCChestDropInfo), DYNAMIC_SIZE_PACKET));
#endif
#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
			Set(HEADER_GC_PICKUP_ITEM_SC, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPickupItemSC), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
			Set(HEADER_GC_OFFLINE_SHOP, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCShop), DYNAMIC_SIZE_PACKET));
			Set(HEADER_GC_OFFLINE_SHOP_SIGN, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCShopSign), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
			Set(HEADER_GC_SHOPSEARCH_SET, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCShopSearchItemSet), DYNAMIC_SIZE_PACKET));
#endif
#ifdef ENABLE_CLIENT_LOCALE_STRING
			Set(HEADER_GC_LOCALE_CHAT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCLocaleChat), STATIC_SIZE_PACKET));
			Set(HEADER_GC_LOCALE_WHISPER, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCLocaleWhisper), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			Set(HEADER_GC_AURA, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCAura), DYNAMIC_SIZE_PACKET));
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
			Set(HEADER_GC_PET, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPet), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PET_SET, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPetSet), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PET_SET_EXCHANGE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPetSet), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PET_DEL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPetDelete), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PET_SUMMON, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPetSummon), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PET_POINT_CHANGE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPetPointUpdate), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PET_NAME_CHANGE_RESULT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPetNameChangeResult), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PET_SKILL_UPDATE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPetSkillUpdate), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PET_SKILL_COOLTIME, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPetSkillCooltime), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PET_DETERMINE_RESULT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPetDetermineResult), STATIC_SIZE_PACKET));
			Set(HEADER_GC_PET_ATTR_CHANGE_RESULT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCPetAttrChangeResult), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			Set(HEADER_GC_CHANGE_LOOK_SET, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCTransmutationItemSet), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHANGE_LOOK_DEL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCTransmutationDel), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHANGE_LOOK_FREE_SET, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCTransmutationItemSet), STATIC_SIZE_PACKET));
			Set(HEADER_GC_CHANGE_LOOK_FREE_DEL, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCTransmutationDel), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_HUNTING_SYSTEM
			Set(HEADER_GC_HUNTING_OPEN_MAIN, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCOpenWindowHuntingMain), STATIC_SIZE_PACKET));
			Set(HEADER_GC_HUNTING_OPEN_SELECT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCOpenWindowHuntingSelect), STATIC_SIZE_PACKET));
			Set(HEADER_GC_HUNTING_OPEN_REWARD, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCOpenWindowReward), STATIC_SIZE_PACKET));
			Set(HEADER_GC_HUNTING_UPDATE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCUpdateHunting), STATIC_SIZE_PACKET));
			Set(HEADER_GC_HUNTING_RECIVE_RAND_ITEMS, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCReciveRandomItems), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_RIDING_EXTENDED
			Set(HEADER_GC_MOUNT_UP_GRADE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMountUpGrade), STATIC_SIZE_PACKET));
			Set(HEADER_GC_MOUNT_UP_GRADE_CHAT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMountUpGradeChat), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_DUNGEON_INFO
			Set(HEADER_GC_DUNGEON_INFO_SYSTEM,	CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCDungeonInfoReceive), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_EVENT_SYSTEM
			Set(HEADER_GC_EVENT_INFO, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCEventInfo), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_ATTENDANCE_EVENT
			Set(HEADER_GC_HIT_COUNT_INFO, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCHitCountInfo), STATIC_SIZE_PACKET));
			Set(HEADER_GC_ATTENDANCE_EVENT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCAttendanceEvent), STATIC_SIZE_PACKET));
			Set(HEADER_GC_ATTENDANCE_EVENT_INFO, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCAttendanceEventInfo), DYNAMIC_SIZE_PACKET));
#endif
#ifdef ENABLE_STONE_EVENT
			Set(HEADER_GC_STONE_EVENT, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCStoneEvent), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_MINI_GAME_CATCH_KING
			Set(HEADER_GC_MINI_GAME_CATCH_KING, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCMiniGameCatchKing), DYNAMIC_SIZE_PACKET));
#endif
#ifdef ENABLE_RANKING
			Set(HEADER_GC_RANKING_SEND, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCRankingTable), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_KILL_STATISTICS
			Set(HEADER_GC_KILL_STATISTICS, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCKillStatistics), STATIC_SIZE_PACKET));
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
			Set(HEADER_GC_SUNGMA_ATTR_UPDATE, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCSungmaAttrUpdate), STATIC_SIZE_PACKET));
#endif
			Set(HEADER_GC_YIN_YANG_ATTR, CNetworkPacketHeaderMap::TPacketType(sizeof(TPacketGCYinAndYangAttr), DYNAMIC_SIZE_PACKET));
		}
};

int g_iLastPacket[2] = { 0, 0 };

void CPythonNetworkStream::ExitApplication()
{
	if (__IsNotPing())
	{
		AbsoluteExitApplication();
	}
	else
	{
		SendChatPacket ("/quit");
	}
}

void CPythonNetworkStream::ExitGame()
{
	if (__IsNotPing())
	{
		LogOutGame();
	}
	else
	{
		SendChatPacket ("/phase_select");
	}
}


void CPythonNetworkStream::LogOutGame()
{
	if (__IsNotPing())
	{
		AbsoluteExitGame();
	}
	else
	{
		SendChatPacket ("/logout");
	}
}

void CPythonNetworkStream::AbsoluteExitGame()
{
	if (!IsOnline())
	{
		return;
	}

	OnRemoteDisconnect();
	Disconnect();
}

void CPythonNetworkStream::AbsoluteExitApplication()
{
	PostQuitMessage (0);
}

bool CPythonNetworkStream::__IsNotPing()
{
	return false;
}

DWORD CPythonNetworkStream::GetGuildID()
{
	return m_dwGuildID;
}

UINT CPythonNetworkStream::UploadMark (const char* c_szImageFileName)
{
	// MARK_BUG_FIX
	if (0 == m_dwGuildID)
	{
		return ERROR_MARK_UPLOAD_NEED_RECONNECT;
	}

	gs_nextDownloadMarkTime = 0;
	// END_OF_MARK_BUG_FIX

	UINT uError=ERROR_UNKNOWN;
	CGuildMarkUploader& rkGuildMarkUploader=CGuildMarkUploader::Instance();
	if (!rkGuildMarkUploader.Connect (m_kMarkAuth.m_kNetAddr, m_kMarkAuth.m_dwHandle, m_kMarkAuth.m_dwRandomKey, m_dwGuildID, c_szImageFileName, &uError))
	{
		switch (uError)
		{
			case CGuildMarkUploader::ERROR_CONNECT:
				return ERROR_CONNECT_MARK_SERVER;
				break;

			case CGuildMarkUploader::ERROR_LOAD:
				return ERROR_LOAD_MARK;
				break;

			case CGuildMarkUploader::ERROR_WIDTH:
				return ERROR_MARK_WIDTH;
				break;

			case CGuildMarkUploader::ERROR_HEIGHT:
				return ERROR_MARK_HEIGHT;
				break;

			default:
				return ERROR_UNKNOWN;
		}
	}

	// MARK_BUG_FIX
	__DownloadMark();
	// END_OF_MARK_BUG_FIX

	if (CGuildMarkManager::INVALID_MARK_ID == CGuildMarkManager::Instance().GetMarkID(m_dwGuildID))
	{
		TraceError("Need reconnecting to guild mark.");
		return ERROR_MARK_CHECK_NEED_RECONNECT;
	}

	return ERROR_NONE;
}

UINT CPythonNetworkStream::UploadSymbol (const char* c_szImageFileName)
{
	UINT uError=ERROR_UNKNOWN;
	CGuildMarkUploader& rkGuildMarkUploader=CGuildMarkUploader::Instance();
	if (!rkGuildMarkUploader.ConnectToSendSymbol (m_kMarkAuth.m_kNetAddr, m_kMarkAuth.m_dwHandle, m_kMarkAuth.m_dwRandomKey, m_dwGuildID, c_szImageFileName, &uError))
	{
		switch (uError)
		{
			case CGuildMarkUploader::ERROR_CONNECT:
				return ERROR_CONNECT_MARK_SERVER;
				break;

			case CGuildMarkUploader::ERROR_LOAD:
				return ERROR_LOAD_MARK;
				break;

			case CGuildMarkUploader::ERROR_WIDTH:
				return ERROR_MARK_WIDTH;
				break;

			case CGuildMarkUploader::ERROR_HEIGHT:
				return ERROR_MARK_HEIGHT;
				break;

			default:
				return ERROR_UNKNOWN;
		}
	}

	return ERROR_NONE;
}

void CPythonNetworkStream::__DownloadMark()
{
	DWORD curTime = ELTimer_GetMSec();

	if (curTime < gs_nextDownloadMarkTime)
	{
		return;
	}

	gs_nextDownloadMarkTime = curTime + 60000 * 3;

	CGuildMarkDownloader& rkGuildMarkDownloader = CGuildMarkDownloader::Instance();
	rkGuildMarkDownloader.Connect (m_kMarkAuth.m_kNetAddr, m_kMarkAuth.m_dwHandle, m_kMarkAuth.m_dwRandomKey);
}

void CPythonNetworkStream::__DownloadSymbol (const std::vector<DWORD>& c_rkVec_dwGuildID)
{
	CGuildMarkDownloader& rkGuildMarkDownloader=CGuildMarkDownloader::Instance();
	rkGuildMarkDownloader.ConnectToRecvSymbol (m_kMarkAuth.m_kNetAddr, m_kMarkAuth.m_dwHandle, m_kMarkAuth.m_dwRandomKey, c_rkVec_dwGuildID);
}

void CPythonNetworkStream::SetPhaseWindow (UINT ePhaseWnd, PyObject* poPhaseWnd)
{
	if (ePhaseWnd>=PHASE_WINDOW_NUM)
	{
		return;
	}

	m_apoPhaseWnd[ePhaseWnd]=poPhaseWnd;
}

void CPythonNetworkStream::ClearPhaseWindow (UINT ePhaseWnd, PyObject* poPhaseWnd)
{
	if (ePhaseWnd>=PHASE_WINDOW_NUM)
	{
		return;
	}

	if (poPhaseWnd != m_apoPhaseWnd[ePhaseWnd])
	{
		return;
	}

	m_apoPhaseWnd[ePhaseWnd]=0;
}

void CPythonNetworkStream::SetServerCommandParserWindow (PyObject* poWnd)
{
	m_poSerCommandParserWnd = poWnd;
}

bool CPythonNetworkStream::IsSelectedEmpire()
{
	if (m_dwEmpireID)
	{
		return true;
	}

	return false;
}

UINT CPythonNetworkStream::GetAccountCharacterSlotDatau (UINT iSlot, UINT eType)
{
	if (iSlot >= PLAYER_PER_ACCOUNT4)
	{
		return 0;
	}

	TSimplePlayerInformation&	rkSimplePlayerInfo=m_akSimplePlayerInfo[iSlot];

	switch (eType)
	{
		case ACCOUNT_CHARACTER_SLOT_ID:
			return rkSimplePlayerInfo.dwID;
		case ACCOUNT_CHARACTER_SLOT_RACE:
			return rkSimplePlayerInfo.byJob;
		case ACCOUNT_CHARACTER_SLOT_LEVEL:
			return rkSimplePlayerInfo.byLevel;
		case ACCOUNT_CHARACTER_SLOT_STR:
			return rkSimplePlayerInfo.byST;
		case ACCOUNT_CHARACTER_SLOT_DEX:
			return rkSimplePlayerInfo.byDX;
		case ACCOUNT_CHARACTER_SLOT_HTH:
			return rkSimplePlayerInfo.byHT;
		case ACCOUNT_CHARACTER_SLOT_INT:
			return rkSimplePlayerInfo.byIQ;
		case ACCOUNT_CHARACTER_SLOT_PLAYTIME:
			return rkSimplePlayerInfo.dwPlayMinutes;
		case ACCOUNT_CHARACTER_SLOT_FORM:
			return rkSimplePlayerInfo.wMainPart;
		case ACCOUNT_CHARACTER_SLOT_PORT:
			return rkSimplePlayerInfo.wPort;
		case ACCOUNT_CHARACTER_SLOT_GUILD_ID:
			return m_adwGuildID[iSlot];
			break;
		case ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG:
			return rkSimplePlayerInfo.bChangeName;
			break;
		case ACCOUNT_CHARACTER_SLOT_HAIR:
			return rkSimplePlayerInfo.wHairPart;
			break;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case ACCOUNT_CHARACTER_SLOT_ACCE:
			return rkSimplePlayerInfo.wAccePart;
			break;
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		case ACCOUNT_CHARACTER_SLOT_AURA:
			return rkSimplePlayerInfo.wAuraPart;
			break;
#endif
		case ACCOUNT_CHARACTER_SLOT_LAST_PLAYTIME:
			return rkSimplePlayerInfo.dwLastPlay;
		case ACCOUNT_CHARACTER_SLOT_MAPINDEX:
			return rkSimplePlayerInfo.wMapIndex;
			break;
#ifdef ENABLE_CONQUEROR_LEVEL
		case ACCOUNT_CHARACTER_SLOT_CONQUEROR_LEVEL:
			return rkSimplePlayerInfo.byConquerorLevel;
			break;
		case ACCOUNT_CHARACTER_SLOT_SUNGMA_ST:
			return rkSimplePlayerInfo.bySungmaST;
			break;
		case ACCOUNT_CHARACTER_SLOT_SUNGMA_HP:
			return rkSimplePlayerInfo.bySungmaHP;
			break;
		case ACCOUNT_CHARACTER_SLOT_SUNGMA_MOVE:
			return rkSimplePlayerInfo.bySungmaMV;
			break;
		case ACCOUNT_CHARACTER_SLOT_SUNGMA_IMMUNE:
			return rkSimplePlayerInfo.bySungmaINM;
			break;
#endif
	}
	return 0;
}

const char* CPythonNetworkStream::GetAccountCharacterSlotDataz (UINT iSlot, UINT eType)
{
	static const char* sc_szEmpty="";

	if (iSlot >= PLAYER_PER_ACCOUNT4)
	{
		return sc_szEmpty;
	}

	TSimplePlayerInformation&	rkSimplePlayerInfo=m_akSimplePlayerInfo[iSlot];

	switch (eType)
	{
		case ACCOUNT_CHARACTER_SLOT_ADDR:
		{
			BYTE ip[4];

			const int LEN = 4;
			for (int i = 0; i < LEN; i++)
			{
				ip[i] = BYTE (rkSimplePlayerInfo.lAddr&0xff);
				rkSimplePlayerInfo.lAddr>>=8;
			}


			static char s_szAddr[256];
			snprintf(s_szAddr, sizeof(s_szAddr), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
			return s_szAddr;
		}
		break;
		case ACCOUNT_CHARACTER_SLOT_NAME:
			return rkSimplePlayerInfo.szName;
			break;
		case ACCOUNT_CHARACTER_SLOT_GUILD_NAME:
			return m_astrGuildName[iSlot].c_str();
			break;
	}
	return sc_szEmpty;
}

void CPythonNetworkStream::ConnectLoginServer (const char* c_szAddr, UINT uPort)
{
	CNetworkStream::Connect (c_szAddr, uPort);
}

void CPythonNetworkStream::SetMarkServer (const char* c_szAddr, UINT uPort)
{
	m_kMarkAuth.m_kNetAddr.Set (c_szAddr, uPort);
}

void CPythonNetworkStream::ConnectGameServer (UINT iChrSlot)
{
	if (iChrSlot >= PLAYER_PER_ACCOUNT4)
	{
		return;
	}

	m_dwSelectedCharacterIndex = iChrSlot;

	__DirectEnterMode_Set (iChrSlot);

	TSimplePlayerInformation&	rkSimplePlayerInfo=m_akSimplePlayerInfo[iChrSlot];
	CNetworkStream::Connect ((DWORD)rkSimplePlayerInfo.lAddr, rkSimplePlayerInfo.wPort);
}

void CPythonNetworkStream::SetLoginInfo (const char* c_szID, const char* c_szPassword)
{
	m_stID=c_szID;
	m_stPassword=c_szPassword;
}

void CPythonNetworkStream::ClearLoginInfo (void)
{
	m_stPassword = "";
}

void CPythonNetworkStream::SetLoginKey (DWORD dwLoginKey)
{
	m_dwLoginKey = dwLoginKey;
}

bool CPythonNetworkStream::CheckPacket (TPacketHeader* pRetHeader)
{
	*pRetHeader = 0;

	static CMainPacketHeaderMap s_packetHeaderMap;

	TPacketHeader header;

	if (!Peek (sizeof (TPacketHeader), &header))
	{
		return false;
	}

	if (0 == header)
	{
		if (!Recv (sizeof (TPacketHeader), &header))
		{
			return false;
		}

		while (Peek (sizeof (TPacketHeader), &header))
		{
			if (0 == header)
			{
				if (!Recv (sizeof (TPacketHeader), &header))
				{
					return false;
				}
			}
			else
			{
				break;
			}
		}

		if (0 == header)
		{
			return false;
		}
	}

	CNetworkPacketHeaderMap::TPacketType PacketType;

	if (!s_packetHeaderMap.Get (header, &PacketType))
	{
		TraceError ("Unknown packet header: %d, last: %d %d", header, g_iLastPacket[0], g_iLastPacket[1]);
		ClearRecvBuffer();

		PostQuitMessage (0);
		return false;
	}

	// Code for dynamic size packet
#ifdef ENABLE_LARGE_DYNAMIC_PACKET
	if (PacketType.iPacketType == DYNAMIC_SIZE_PACKET)
#else
	if (PacketType.isDynamicSizePacket)
#endif
	{
		TDynamicSizePacketHeader DynamicSizePacketHeader;

		if (!Peek(sizeof(TDynamicSizePacketHeader), &DynamicSizePacketHeader))
			return false;

		if (!Peek(DynamicSizePacketHeader.size))
		{
			Tracef("CPythonNetworkStream::CheckPacket - Not enough dynamic packet size: header %d packet size: %d\n",
				DynamicSizePacketHeader.header,
				DynamicSizePacketHeader.size);
			return false;
		}
	}
#ifdef ENABLE_LARGE_DYNAMIC_PACKET
	else if (PacketType.iPacketType == LARGE_DYNAMIC_SIZE_PACKET)
	{
		TLargeDynamicSizePacketHeader largeDynamicSizePacketHeader;
		if (!Peek(sizeof(TLargeDynamicSizePacketHeader), &largeDynamicSizePacketHeader))
			return false;

		if (!Peek(largeDynamicSizePacketHeader.size))
		{
			Tracef("CPythonNetworkStream::CheckPacket - Not enough LARGE dynamic packet size: header %d packet size: %d\n",
				largeDynamicSizePacketHeader.header,
				largeDynamicSizePacketHeader.size);
			return false;
		}
	}
#endif
	else
	{
		if (!Peek (PacketType.iPacketSize))
		{
			//Tracef("Not enough packet size: header %d packet size: %d, recv buffer size: %d",
			//	header,
			//	PacketType.iPacketSize,
			//	GetRecvBufferSize());
			return false;
		}
	}

	if (!header)
	{
		return false;
	}

	*pRetHeader = header;

	g_iLastPacket[0] = g_iLastPacket[1];
	g_iLastPacket[1] = header;
	//Tracenf("header %d size %d", header, PacketType.iPacketSize);
	//Tracenf("header %d size %d outputpos[%d] security %u", header, PacketType.iPacketSize, m_recvBufOutputPos, IsSecurityMode());
	return true;
}

bool CPythonNetworkStream::RecvErrorPacket (int header)
{
	TraceError ("Phase %s does not handle this header (header: %d, last: %d, %d)",
				m_strPhase.c_str(), header, g_iLastPacket[0], g_iLastPacket[1]);

	ClearRecvBuffer();
	return true;
}

bool CPythonNetworkStream::RecvPhasePacket()
{
	TPacketGCPhase packet_phase;

	if (!Recv (sizeof (TPacketGCPhase), &packet_phase))
	{
		return false;
	}

	switch (packet_phase.phase)
	{
		case PHASE_CLOSE:
			ClosePhase();
			break;

		case PHASE_HANDSHAKE:
			SetHandShakePhase();
			break;

		case PHASE_LOGIN:
			SetLoginPhase();
			break;

		case PHASE_SELECT:
			SetSelectPhase();
#ifdef ENABLE_DISCORD_RPC
			Discord_Update(false);
#endif
			BuildProcessCRC();

			// MARK_BUG_FIX
			__DownloadMark();
			// END_OF_MARK_BUG_FIX
			break;

		case PHASE_LOADING:
			SetLoadingPhase();
			break;

		case PHASE_GAME:
			SetGamePhase();
#ifdef ENABLE_DISCORD_RPC
			Discord_Update(true);
#endif

			break;

		case PHASE_DEAD:
			break;
	}

	return true;
}

bool CPythonNetworkStream::RecvPingPacket()
{
	Tracef ("recv ping packet. (securitymode %u)\n", IsSecurityMode());

	TPacketGCPing kPacketPing;

	if (!Recv (sizeof (TPacketGCPing), &kPacketPing))
	{
		return false;
	}

	m_dwLastGamePingTime = ELTimer_GetMSec();

	TPacketCGPong kPacketPong;
	kPacketPong.bHeader = HEADER_CG_PONG;

	if (!Send (sizeof (TPacketCGPong), &kPacketPong))
	{
		return false;
	}

	if (IsSecurityMode())
		return SendSequence();
	else
		return true;
}

bool CPythonNetworkStream::RecvDefaultPacket (int header)
{
	if (!header)
	{
		return true;
	}

	TraceError("Unprocessed packet header %d, state %s\n", header, m_strPhase.c_str());
	ClearRecvBuffer();
	return true;
}

bool CPythonNetworkStream::OnProcess()
{
	if (m_isStartGame)
	{
		m_isStartGame = FALSE;

		PyCallClassMemberFunc (m_poHandler, "SetGamePhase", Py_BuildValue ("()"));
	}

	m_rokNetActorMgr->Update();

	if (m_phaseProcessFunc.IsEmpty())
	{
		return true;
	}

	{
		m_phaseProcessFunc.Run();
	}

	return true;
}

// Set
void CPythonNetworkStream::SetOffLinePhase()
{
	if ("OffLine" != m_strPhase)
	{
		m_phaseLeaveFunc.Run();
	}

	m_strPhase = "OffLine";

	Tracen ("");
	Tracen ("## Network - OffLine Phase ##");
	Tracen ("");

#ifdef ENABLE_DISCORD_RPC
	Discord_Update(false);
#endif

	m_dwChangingPhaseTime = ELTimer_GetMSec();
	m_phaseProcessFunc.Set (this, &CPythonNetworkStream::OffLinePhase);
	m_phaseLeaveFunc.Set (this, &CPythonNetworkStream::__LeaveOfflinePhase);

	SetGameOffline();

	m_dwSelectedCharacterIndex = 0;

	__DirectEnterMode_Initialize();
	__BettingGuildWar_Initialize();
}


void CPythonNetworkStream::ClosePhase()
{
	PyCallClassMemberFunc (m_poHandler, "SetLoginPhase", Py_BuildValue ("()"));
}

// Game Online
void CPythonNetworkStream::SetGameOnline()
{
	m_isGameOnline = TRUE;
}

void CPythonNetworkStream::SetGameOffline()
{
	m_isGameOnline = FALSE;
}

BOOL CPythonNetworkStream::IsGameOnline()
{
	return m_isGameOnline;
}

// Handler
void CPythonNetworkStream::SetHandler (PyObject* poHandler)
{
	m_poHandler = poHandler;
}

// ETC
DWORD CPythonNetworkStream::GetMainActorVID()
{
	return m_dwMainActorVID;
}

DWORD CPythonNetworkStream::GetMainActorRace()
{
	return m_dwMainActorRace;
}

DWORD CPythonNetworkStream::GetMainActorEmpire()
{
	return m_dwMainActorEmpire;
}

DWORD CPythonNetworkStream::GetMainActorSkillGroup()
{
	return m_dwMainActorSkillGroup;
}

void CPythonNetworkStream::SetEmpireID (DWORD dwEmpireID)
{
	m_dwEmpireID = dwEmpireID;
}

DWORD CPythonNetworkStream::GetEmpireID()
{
	return m_dwEmpireID;
}

void CPythonNetworkStream::__ClearSelectCharacterData()
{
	memset (&m_akSimplePlayerInfo, 0, sizeof (m_akSimplePlayerInfo));

	for (int i = 0; i < PLAYER_PER_ACCOUNT4; ++i)
	{
		m_adwGuildID[i] = 0;
		m_astrGuildName[i] = "";
	}
}

void CPythonNetworkStream::__DirectEnterMode_Initialize()
{
	m_kDirectEnterMode.m_isSet = false;
#ifdef ENABLE_CLIENT_PERFORMANCE
	m_kDirectEnterMode.m_isWarp = false;
#endif
	m_kDirectEnterMode.m_dwChrSlotIndex = 0;
}

#ifdef ENABLE_CLIENT_PERFORMANCE
bool CPythonNetworkStream::__DirectEnterMode_IsWarp()
{
	return m_kDirectEnterMode.m_isWarp;
}

void CPythonNetworkStream::__DirectEnterMode_Set(UINT uChrSlotIndex, bool bisWarp)
#else
void CPythonNetworkStream::__DirectEnterMode_Set(UINT uChrSlotIndex)
#endif
{
	m_kDirectEnterMode.m_isSet = true;
#ifdef ENABLE_CLIENT_PERFORMANCE
	m_kDirectEnterMode.m_isWarp = bisWarp;
#endif
	m_kDirectEnterMode.m_dwChrSlotIndex = uChrSlotIndex;
}

bool CPythonNetworkStream::__DirectEnterMode_IsSet()
{
	return m_kDirectEnterMode.m_isSet;
}

void CPythonNetworkStream::__InitializeMarkAuth()
{
	m_kMarkAuth.m_dwHandle=0;
	m_kMarkAuth.m_dwRandomKey=0;
}

void CPythonNetworkStream::__BettingGuildWar_Initialize()
{
	m_kBettingGuildWar.m_dwBettingMoney=0;
	m_kBettingGuildWar.m_dwObserverCount=0;
}

void CPythonNetworkStream::__BettingGuildWar_SetObserverCount (UINT uObserverCount)
{
	m_kBettingGuildWar.m_dwObserverCount=uObserverCount;
}

void CPythonNetworkStream::__BettingGuildWar_SetBettingMoney (UINT uBettingMoney)
{
	m_kBettingGuildWar.m_dwBettingMoney=uBettingMoney;
}

DWORD CPythonNetworkStream::EXPORT_GetBettingGuildWarValue (const char* c_szValueName)
{
	if (stricmp (c_szValueName, "OBSERVER_COUNT") == 0)
	{
		return m_kBettingGuildWar.m_dwObserverCount;
	}

	if (stricmp (c_szValueName, "BETTING_MONEY") == 0)
	{
		return m_kBettingGuildWar.m_dwBettingMoney;
	}

	return 0;
}

void CPythonNetworkStream::__ServerTimeSync_Initialize()
{
	m_kServerTimeSync.m_dwChangeClientTime=0;
	m_kServerTimeSync.m_dwChangeServerTime=0;
}

void CPythonNetworkStream::SetWaitFlag()
{
	m_isWaitLoginKey = TRUE;
}

void CPythonNetworkStream::SendEmoticon (UINT eEmoticon)
{
	if (eEmoticon < m_EmoticonStringVector.size())
	{
		SendChatPacket (m_EmoticonStringVector[eEmoticon].c_str());
	}
	else
	{
		assert (false && "SendEmoticon Error");
	}
}

CPythonNetworkStream::CPythonNetworkStream()
{
	m_rokNetActorMgr=new CNetworkActorManager;

	memset (m_akSimplePlayerInfo, 0, sizeof (m_akSimplePlayerInfo));

	m_phaseProcessFunc.Clear();

	m_dwEmpireID = 0;
	m_dwGuildID = 0;

	m_dwMainActorVID = 0;
	m_dwMainActorRace = 0;
	m_dwMainActorEmpire = 0;
	m_dwMainActorSkillGroup = 0;
	m_poHandler = nullptr;

	m_dwLastGamePingTime = 0;

	m_dwLoginKey = 0;
	m_isWaitLoginKey = FALSE;
	m_isStartGame = FALSE;
	m_isEnableChatInsultFilter = FALSE;
	m_bComboSkillFlag = FALSE;
	m_strPhase = "OffLine";
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	m_bLanguageSet = FALSE;
	m_bLanguage = 0;
#endif

	__InitializeGamePhase();
	__InitializeMarkAuth();

	__DirectEnterMode_Initialize();
	__BettingGuildWar_Initialize();

	std::fill(m_apoPhaseWnd, m_apoPhaseWnd+PHASE_WINDOW_NUM, (PyObject *)nullptr);
	m_poSerCommandParserWnd = nullptr;

	SetOffLinePhase();
}

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
void CPythonNetworkStream::SetLanguage(BYTE bLanguage)
{
	m_bLanguage = bLanguage;
	// E?er game phase'deyse ve dil bilgisi henüz gönderilmediyse gönder
	if (m_strPhase == "Game" && m_bLanguageSet == FALSE && bLanguage != 0)
	{
		SendChangeLanguagePacket(bLanguage);
	}
}
#endif

CPythonNetworkStream::~CPythonNetworkStream()
{
	Tracen ("PythonNetworkMainStream Clear");
}
