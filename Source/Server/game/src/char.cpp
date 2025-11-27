#include "stdafx.h"

#include "../../common/VnumHelper.h"

#include "char.h"
#include "config.h"
#include "utils.h"
#include "crc32.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "item_manager.h"
#include "motion.h"
#include "vector.h"
#include "packet.h"
#include "cmd.h"
#include "fishing.h"
#include "exchange.h"
#include "battle.h"
#include "affect.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "pvp.h"
#include "party.h"
#include "start_position.h"
#include "questmanager.h"
#include "log.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "unique_item.h"
#include "priv_manager.h"
#include "war_map.h"
#include "xmas_event.h"
#include "banword.h"
#include "target.h"
#include "wedding.h"
#include "mob_manager.h"
#include "mining.h"
#include "arena.h"
#include "horsename_manager.h"
#include "gm.h"
#include "map_location.h"
#include "BlueDragon_Binder.h"
#include "skill_power.h"
#include "buff_on_attributes.h"
#include "../../common/service.h"
#include "../../common/tables.h"
#include "DragonSoul.h"
#include "OXEvent.h"
#include "locale.hpp"

#ifdef ENABLE_MOUNT_SYSTEM
	#include "MountSystem.h"
#endif

#ifdef ENABLE_PET_SYSTEM
	#include "PetSystem.h"
#endif

#ifdef ENABLE_RENEWAL_SWITCHBOT
	#include "switchbot_manager.h"
#endif

#ifdef ENABLE_MOB_DROP_INFO
	#include <algorithm>
	#include <iterator>
	using namespace std;
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
	#include "battlepass_manager.h"
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	#include "offline_shop.h"
	#include "offlineshop_manager.h"
#endif

#ifdef ENABLE_BOT_PLAYER
	#include "BotPlayer.h"
#endif

extern const BYTE g_aBuffOnAttrPoints;
extern bool RaceToJob(unsigned race, unsigned *ret_job);

extern int g_nPortalLimitTime;
extern int test_server;

extern bool IS_SUMMONABLE_ZONE(int map_index); // char_item.cpp
bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index);

bool CAN_ENTER_ZONE(const LPCHARACTER& ch, int map_index)
{
	switch (map_index)
	{
		case 301:
		case 302:
		case 303:
		case 304:
			if (ch->GetLevel() < 90)
				return false;
	}
	return true;
}

LPCHARACTER DynamicCharacterPtr::Get() const
{
	LPCHARACTER p = NULL;
	if (is_pc)
	{
		p = CHARACTER_MANAGER::instance().FindByPID(id);
	}
	else
	{
		p = CHARACTER_MANAGER::instance().Find(id);
	}
	return p;
}

DynamicCharacterPtr& DynamicCharacterPtr::operator=(LPCHARACTER character)
{
	if (character == NULL)
	{
		Reset();
		return *this;
	}
	if (character->IsPC())
	{
		is_pc = true;
		id = character->GetPlayerID();
	}
	else
	{
		is_pc = false;
		id = character->GetVID();
	}
	return *this;
}

CHARACTER::CHARACTER()
{
	m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateIdle, &CHARACTER::EndStateEmpty);
	m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
	m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateBattle, &CHARACTER::EndStateEmpty);

	Initialize();
}

CHARACTER::~CHARACTER()
{
	Destroy();
}

void CHARACTER::Initialize()
{
	CEntity::Initialize(ENTITY_CHARACTER);

	m_bNoOpenedShop = true;

	m_bOpeningSafebox = false;

	m_fSyncTime = get_float_time()-3;
	m_dwPlayerID = 0;
	m_dwKillerPID = 0;

	m_iMoveCount = 0;
	CountDrops = 0;
	LastDropTime = 0;
	m_iLastPMPulse = 0;
	m_iPMCounter = 0;

	m_pkRegen = NULL;
#ifdef __AUTO_HUNT__
	m_bAutoHuntStatus = false;
#endif
	regen_id_ = 0;
	m_posRegen.x = m_posRegen.y = m_posRegen.z = 0;
	m_posStart.x = m_posStart.y = 0;
	m_posDest.x = m_posDest.y = 0;
	m_fRegenAngle = 0.0f;

	m_pkMobData = NULL;
	m_pkMobInst = NULL;
	m_pkShop = NULL;
	m_pkChrShopOwner = NULL;
	m_pkMyShop = NULL;
	m_pkExchange = NULL;
	m_pkParty = NULL;
	m_pkPartyRequestEvent = NULL;
	m_pGuild = NULL;
	m_pkChrTarget = NULL;
	m_pkMuyeongEvent = NULL;
	m_pkWarpNPCEvent = NULL;
	m_pkDeadEvent = NULL;
#ifdef ENABLE_BOT_PLAYER
	m_pkBotCharacterDeadEvent = NULL;
#endif
	m_pkStunEvent = NULL;
	m_pkSaveEvent = NULL;
	m_pkRecoveryEvent = NULL;
	m_pkTimedEvent = NULL;
	m_pkFishingEvent = NULL;
	m_pkWarpEvent = NULL;

	// MINING
	m_pkMiningEvent = NULL;
	// END_OF_MINING

#ifdef ENABLE_CHANGE_CHANNEL
	m_pkChangeChannelEvent = NULL;
#endif

	m_pkPoisonEvent = NULL;
	m_pkFireEvent = NULL;
	m_pkCheckSpeedHackEvent	= NULL;
	m_speed_hack_count = 0;

	m_pkAffectEvent = NULL;
	m_afAffectFlag = TAffectFlag(0, 0);

	m_pkDestroyWhenIdleEvent = NULL;

	m_pkChrSyncOwner = NULL;

	memset(&m_points, 0, sizeof(m_points));
	memset(&m_pointsInstant, 0, sizeof(m_pointsInstant));
	memset(&m_quickslot, 0, sizeof(m_quickslot));

	m_bCharType = CHAR_TYPE_MONSTER;

	SetPosition(POS_STANDING);

	m_dwPlayStartTime = m_dwLastMoveTime = get_dword_time();

	GotoState(m_stateIdle);
	m_dwStateDuration = 1;

#ifdef ENABLE_LAST_ATTACK_TIME_FIX
	m_dwLastAttackTime = std::min(0UL, static_cast<uint32_t>(get_dword_time()) - 120000UL); //2m ago (for some reason?)
#else
	m_dwLastAttackTime = get_dword_time() - 20000;
#endif

	m_bAddChrState = 0;

	m_pkChrStone = NULL;

	m_pkSafebox = NULL;
	m_iSafeboxSize = -1;
	m_iSafeboxLoadTime = 0;

	m_pkMall = NULL;
	m_iMallLoadTime = 0;

	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;
	m_lWarpMapIndex = 0;

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;

	m_pSkillLevels = NULL;

	m_dwMoveStartTime = 0;
	m_dwMoveDuration = 0;

	m_dwFlyTargetID = 0;

	m_dwNextStatePulse = 0;

	m_dwLastDeadTime = get_dword_time()-180000;

	m_bSkipSave = false;

	m_bItemLoaded = false;

	m_bHasPoisoned = false;

	m_pkDungeon = NULL;
	m_iEventAttr = 0;

	m_kAttackLog.dwVID = 0;
	m_kAttackLog.dwTime = 0;

	m_bNowWalking = m_bWalking = false;
	ResetChangeAttackPositionTime();

	m_bDetailLog = false;
	m_bMonsterLog = false;

	m_bDisableCooltime = false;

	m_iAlignment = 0;
	m_iRealAlignment = 0;

	m_iKillerModePulse = 0;
	m_bPKMode = PK_MODE_PEACE;

	m_dwQuestNPCVID = 0;
	m_dwQuestByVnum = 0;
	m_dwQuestItemVID = 0;

	m_dwUnderGuildWarInfoMessageTime = get_dword_time()-60000;

	m_bUnderRefine = false;

	// REFINE_NPC
	m_dwRefineNPCVID = 0;
	// END_OF_REFINE_NPC

	m_dwPolymorphRace = 0;

	m_bStaminaConsume = false;

	ResetChainLightningIndex();

	m_dwMountVnum = 0;
	m_chHorse = NULL;
	m_chRider = NULL;

#ifdef ENABLE_RIDING_EXTENDED
	m_mount_up_grade_exp = 0;
	m_mount_up_grade_fail = 0;
#endif

#ifdef ENABLE_SPIRIT_STONE_READING
	m_dwRuhSureFlag = 0;
	m_dwRuhYeniSureFlag = 0;
#endif

#ifdef ENABLE_SKILL_BOOK_READING
	m_dwBkYeniSureFlag = 0;
#endif

#ifdef ENABLE_HORSE_SPAWN_EXPLOIT_FIX
	m_dwHorseCheckerFlag = 0;
#endif

#ifdef ENABLE_CAMPFIRE_BUG_FIX
	m_dwCampfireFlag = 0;
#endif

	m_pWarMap = NULL;
	m_pWeddingMap = NULL;
	m_bChatCounter = 0;
#ifdef ENABLE_MOUNT_SYSTEM
	m_bMountCounter = 0;
#endif

	ResetStopTime();

	m_dwLastVictimSetTime = get_dword_time() - 3000;
	m_iMaxAggro = -100;

	m_bSendHorseLevel = 0;
	m_bSendHorseHealthGrade = 0;
	m_bSendHorseStaminaGrade = 0;

	m_dwLoginPlayTime = 0;

	m_pkChrMarried = NULL;

	m_posSafeboxOpen.x = -1000;
	m_posSafeboxOpen.y = -1000;

	// EQUIP_LAST_SKILL_DELAY
	m_dwLastSkillTime = get_dword_time();
	// END_OF_EQUIP_LAST_SKILL_DELAY

	// MOB_SKILL_COOLTIME
	memset(m_adwMobSkillCooltime, 0, sizeof(m_adwMobSkillCooltime));
	// END_OF_MOB_SKILL_COOLTIME

	// ARENA
	m_pArena = NULL;
	m_nPotionLimit = quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count");
	// END_ARENA

	//PREVENT_TRADE_WINDOW
	m_isOpenSafebox = 0;
	//END_PREVENT_TRADE_WINDOW

	//PREVENT_REFINE_HACK
	m_iRefineTime = 0;
	//END_PREVENT_REFINE_HACK

	//RESTRICT_USE_SEED_OR_MOONBOTTLE
	m_iSeedTime = 0;
	//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
	//PREVENT_PORTAL_AFTER_EXCHANGE
	m_iExchangeTime = 0;
	//END_PREVENT_PORTAL_AFTER_EXCHANGE

	m_iSafeboxLoadTime = 0;

	m_iMyShopTime = 0;
	m_deposit_pulse = 0;


#ifdef ENABLE_STR_NEW_NAME_FIX
	m_strNewName.clear();
#else
	m_strNewName = "";
#endif

	m_known_guild.clear();

	m_dwLogOffInterval = 0;

	m_bComboSequence = 0;
	m_dwLastComboTime = 0;
	m_bComboIndex = 0;
	m_iComboHackCount = 0;
	m_dwSkipComboAttackByTime = 0;
	m_dwMountTime = 0;
	m_dwLastGoldDropTime = 0;

	m_bIsLoadedAffect = false;
	cannot_dead = false;

	m_fAttMul = 1.0f;
	m_fDamMul = 1.0f;

	m_pointsInstant.iDragonSoulActiveDeck = -1;

	memset(&m_tvLastSyncTime, 0, sizeof(m_tvLastSyncTime));
	m_iSyncHackCount = 0;

#ifdef ENABLE_MOUNT_SYSTEM
	m_mountSystem = 0;
	m_bIsMount = false;
#endif

#ifdef ENABLE_PET_SYSTEM
	m_petSystem = 0;
	m_bIsPet = false;
#endif

#ifdef ENABLE_SORT_INVENTORY
	m_sortInventoryPulse = 0;
	m_sortSpecialStoragePulse = 0;
#endif

#ifdef ENABLE_TELEPORT_TO_A_FRIEND
	m_iLastWarpRequestTime = {};
#endif

#ifdef ENABLE_MOUNT_PET_SKIN
	m_iChangeCostumeMountSkinTime = 0;
	m_iChangeCostumePetSkinTime = 0;
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	m_HideCostumePulse = 0;
	m_bHideBodyCostume = false;
	m_bHideHairCostume = false;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	m_bHideAcceCostume = false;
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	m_bHideWeaponCostume = false;
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	m_bHideAuraCostume = false;
#endif
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	m_bAcceCombination = false;
	m_bAcceAbsorption = false;
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
	m_pkBiologManager = NULL;
#endif

#ifdef ENABLE_MOB_DROP_INFO
	dwLastTargetInfoPulse = 0;
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	m_pkOpenSafeboxEvent = NULL;
	m_pkOpenMallEvent = NULL;
#endif

#ifdef ENABLE_MINIGAME_OKEY_CARDS_SYSTEM
	memset(&character_cards, 0, sizeof(character_cards));
	memset(&randomized_cards, 0, sizeof(randomized_cards));
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
	m_listExtBattlePass.clear();
	m_bIsLoadedExtBattlePass = false;
	m_dwLastReciveExtBattlePassInfoTime = 0;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	memset(&m_dwSkillColor, 0, sizeof(m_dwSkillColor));
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	m_pkOfflineShop = NULL;
	isOfflineShopPanelOpen = false;
	m_protection_Time.clear();
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	dwPickUPMode = 0;
#endif

#ifdef ENABLE_OFFLINE_MESSAGE
	dwLastOfflinePMTime = 0;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	m_bAuraRefineWindowType = AURA_WINDOW_TYPE_MAX;
	m_bAuraRefineWindowOpen = false;

	for (BYTE i = AURA_SLOT_MAIN; i < AURA_SLOT_MAX; i++)
		m_pAuraRefineWindowItemSlot[i] = NPOS;

	memset(&m_bAuraRefineInfo, 0, AURA_REFINE_INFO_SLOT_MAX * sizeof(TAuraRefineInfo));
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	m_bInvincible = false;

	m_bIsPetHatchOpen = false;
	m_bIsPetChangeNameOpen = false;
	m_bIsGrowthPetLoaded = false;

	m_bPetWindowType = 0;

	m_activeGrowthPet = NULL;
	m_growthPetMap.clear();

	m_bCharacterSize = 0;
#endif

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	memset(inventory_unlock, 0, sizeof(inventory_unlock));
#endif

#ifdef ENABLE_ANTI_EQUIP_FLOOD
	m_dwEquipAntiFloodCount = 0;
	m_dwEquipAntiFloodPulse = 0;
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	m_pkTransmutation = nullptr;
#endif

#ifdef ENABLE_BOT_PLAYER
	m_isBotCharacter = false;
	m_botVictimID = 0;
#endif
}

void CHARACTER::Create(const char * c_pszName, DWORD vid, bool isPC)
{
	static int s_crc = 172814;

	char crc_string[128+1];
	snprintf(crc_string, sizeof(crc_string), "%s%p%d", c_pszName, this, ++s_crc);
	m_vid = VID(vid, GetCRC32(crc_string, strlen(crc_string)));

	if (isPC)
		m_stName = c_pszName;

#ifdef ENABLE_AUTO_SELL_SYSTEM
	m_isAutoSellItemsLoaded = false;
	m_bAutoSellStatus = false;
	if (isPC)
		LoadAutoSellItemsFromJson();
#endif
}

void CHARACTER::Destroy()
{
	CloseMyShop();

	if (m_pkRegen)
	{
		if (m_pkDungeon)
		{
			if (m_pkDungeon->IsValidRegen(m_pkRegen, regen_id_))
			{
				--m_pkRegen->count;
			}
		}
		else
		{
			if (is_valid_regen(m_pkRegen))
				--m_pkRegen->count;
		}
		m_pkRegen = NULL;
	}

	if (m_pkDungeon)
	{
		SetDungeon(NULL);
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (IsPC())
	{
		ClearGrowthPet();
	}
#endif

#ifdef ENABLE_MOUNT_SYSTEM
	if (m_mountSystem)
	{
		m_mountSystem->Destroy();
		delete m_mountSystem;

		m_mountSystem = 0;
	}

	if(GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}
#endif

#ifdef ENABLE_PET_SYSTEM
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;

		m_petSystem = 0;
	}
#endif

	HorseSummon(false);

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	SetTransmutation(nullptr);
#endif

	if (GetRider())
		GetRider()->ClearHorseInfo();

	if (GetDesc())
	{
		GetDesc()->BindCharacter(NULL);
	}

	if (m_pkExchange)
		m_pkExchange->Cancel();

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	if (GetOfflineShop())
		COfflineShopManager::Instance().StopShopping(this);
#endif

	SetVictim(NULL);

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	ClearStone();
	ClearSync();
	ClearTarget();

	if (NULL == m_pkMobData)
	{
		DragonSoul_CleanUp();
		ClearItem();
	}

	// <Factor> m_pkParty becomes NULL after CParty destructor call!
	LPPARTY party = m_pkParty;
	if (party)
	{
		if (party->GetLeaderPID() == GetVID() && !IsPC())
		{
			M2_DELETE(party);
		}
		else
		{
			party->Unlink(this);

			if (!IsPC())
				party->Quit(GetVID());
		}

		SetParty(NULL);
	}

	if (m_pkMobInst)
	{
		M2_DELETE(m_pkMobInst);
		m_pkMobInst = NULL;
	}

	m_pkMobData = NULL;
#ifdef ENABLE_MULTI_FARM_BLOCK
	m_bmultiFarmStatus = false;
#endif

	if (m_pkSafebox)
	{
		M2_DELETE(m_pkSafebox);
		m_pkSafebox = NULL;
	}

	if (m_pkMall)
	{
		M2_DELETE(m_pkMall);
		m_pkMall = NULL;
	}

	m_set_pkChrSpawnedBy.clear();

	StopMuyeongEvent();
	event_cancel(&m_pkWarpNPCEvent);
	event_cancel(&m_pkRecoveryEvent);
	event_cancel(&m_pkDeadEvent);
#ifdef ENABLE_BOT_PLAYER
	event_cancel(&m_pkBotCharacterDeadEvent);
#endif
	event_cancel(&m_pkSaveEvent);
	event_cancel(&m_pkTimedEvent);
	event_cancel(&m_pkStunEvent);
	event_cancel(&m_pkFishingEvent);
	event_cancel(&m_pkPoisonEvent);
	event_cancel(&m_pkFireEvent);
	event_cancel(&m_pkPartyRequestEvent);
	event_cancel(&m_pkWarpEvent);
	event_cancel(&m_pkCheckSpeedHackEvent);
	event_cancel(&m_pkMiningEvent);
#ifdef ENABLE_CHANGE_CHANNEL
	event_cancel(&m_pkChangeChannelEvent);
#endif

	for (itertype(m_mapMobSkillEvent) it = m_mapMobSkillEvent.begin(); it != m_mapMobSkillEvent.end(); ++it)
	{
		LPEVENT pkEvent = it->second;
		event_cancel(&pkEvent);
	}
	m_mapMobSkillEvent.clear();

	ClearAffect();

	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin();  it != m_map_buff_on_attrs.end(); it++)
	{
		if (NULL != it->second)
		{
			M2_DELETE(it->second);
		}
	}
	m_map_buff_on_attrs.clear();

	event_cancel(&m_pkDestroyWhenIdleEvent);

	if (m_pSkillLevels)
	{
		M2_DELETE_ARRAY(m_pSkillLevels);
		m_pSkillLevels = NULL;
	}

	CEntity::Destroy();

	if (GetSectree())
		GetSectree()->RemoveEntity(this);

	if (m_bMonsterLog)
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);

#ifdef ENABLE_AUTO_SELL_SYSTEM
	if (IsPC())
		SaveAutoSellItemsToJson();
#endif
}

const char * CHARACTER::GetName() const
{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	return m_stName.empty() ? (m_pkMobData ? LC_LOCALE_MOB(GetRaceNum(), GetLanguage()) : "") : m_stName.c_str();
#else
	return m_stName.empty() ? (m_pkMobData ? m_pkMobData->m_table.szLocaleName : "") : m_stName.c_str();
#endif
}

#ifdef ENABLE_STACK_LIMIT
void CHARACTER::OpenMyShop(const char* c_pszSign, TShopItemTable* pTable, WORD bItemCount)
#else
void CHARACTER::OpenMyShop(const char* c_pszSign, TShopItemTable* pTable, BYTE bItemCount)
#endif
{
#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (GetActiveGrowthPet())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 737, "");
		return;
	}

	if (GetPetWindowType() == PET_WINDOW_ATTR_CHANGE || GetPetWindowType() == PET_WINDOW_PRIMIUM_FEEDSTUFF)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 738, "");
		return;
	}
#endif

#ifdef DISABLE_ARMOR_CHECK
	if (GetPart(PART_MAIN) > 2)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 17, "");
		return;
	}
#endif

	if (GetMyShop())
	{
		CloseMyShop();
		return;
	}

	quest::PC * pPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	if (pPC->IsRunning())
		return;

	if (bItemCount == 0)
		return;

#ifdef ENABLE_GOLD_LIMIT
	long long nTotalMoney = 0;

	for (int n = 0; n < bItemCount; ++n)
	{
		nTotalMoney += static_cast<long long>((pTable+n)->price);
	}

	nTotalMoney += static_cast<long long>(GetGold());
#else
	int64_t nTotalMoney = 0;

	for (int n = 0; n < bItemCount; ++n)
	{
		nTotalMoney += static_cast<int64_t>((pTable+n)->price);
	}

	nTotalMoney += static_cast<int64_t>(GetGold());
#endif

	if (GOLD_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] Overflow (GOLD_MAX) id %u name %s", GetPlayerID(), GetName());
		LocaleChatPacket(CHAT_TYPE_INFO, 18, "");
		return;
	}

	char szSign[SHOP_SIGN_MAX_LEN+1];
	strlcpy(szSign, c_pszSign, sizeof(szSign));

	m_stShopSign = szSign;

	if (m_stShopSign.length() == 0)
		return;

	if (CBanwordManager::instance().CheckString(m_stShopSign.c_str(), m_stShopSign.length()))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 19, "");
		return;
	}

	std::map<DWORD, DWORD> itemkind;

	std::set<TItemPos> cont;
#ifdef ENABLE_STACK_LIMIT
	for (WORD i = 0; i < bItemCount; ++i)
#else
	for (BYTE i = 0; i < bItemCount; ++i)
#endif
	{
		if (cont.find((pTable + i)->pos) != cont.end())
		{
			sys_err("MYSHOP: duplicate shop item detected! (name: %s)", GetName());
			return;
		}

		LPITEM pkItem = GetItem((pTable + i)->pos);

		if (pkItem)
		{
			const TItemTable * item_table = pkItem->GetProto();

			if (item_table && (IS_SET(item_table->dwAntiFlags, ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_MYSHOP)))
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 20, "");
				return;
			}

			if (pkItem->IsEquipped() == true)
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 21, "");
				return;
			}

			if (pkItem->isLocked() == true)
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 21, "");
				return;
			}

			itemkind[pkItem->GetVnum()] = (pTable + i)->price / pkItem->GetCount();
		}

		cont.insert((pTable + i)->pos);
	}

	if (CountSpecifyItem(71049))
	{

		TPacketMyshopPricelistHeader header;
		TItemPriceInfo info;

		header.dwOwnerID = GetPlayerID();
		header.byCount = itemkind.size();

		TEMP_BUFFER buf;
		buf.write(&header, sizeof(header));

		for (itertype(itemkind) it = itemkind.begin(); it != itemkind.end(); ++it)
		{
			info.dwVnum = it->first;
			info.dwPrice = it->second;

			buf.write(&info, sizeof(info));
		}

		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_UPDATE, 0, buf.read_peek(), buf.size());
	}
	else if (CountSpecifyItem(50200))
		RemoveSpecifyItem(50200, 1);
	else
		return;

	if (m_pkExchange)
		m_pkExchange->Cancel();

	TPacketGCShopSign p;

	p.bHeader = HEADER_GC_SHOP_SIGN;
	p.dwVID = GetVID();
	strlcpy(p.szSign, c_pszSign, sizeof(p.szSign));

	PacketAround(&p, sizeof(TPacketGCShopSign));

	m_pkMyShop = CShopManager::instance().CreatePCShop(this, pTable, bItemCount);

	if (IsPolymorphed() == true)
	{
		RemoveAffect(AFFECT_POLYMORPH);
	}

	if (GetHorse())
	{
		HorseSummon( false, true );
	}

	else if (GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}

	SetPolymorph(30000, true);
}

void CHARACTER::CloseMyShop()
{
	if (GetMyShop())
	{
		m_stShopSign.clear();
		CShopManager::instance().DestroyPCShop(this);
		m_pkMyShop = NULL;

		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		p.szSign[0] = '\0';

		PacketAround(&p, sizeof(p));
		SetPolymorph(GetJob(), true);
	}
}

void EncodeMovePacket(TPacketGCMove & pack, DWORD dwVID, BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, BYTE bRot)
{
	pack.bHeader = HEADER_GC_MOVE;
	pack.bFunc = bFunc;
	pack.bArg = bArg;
	pack.dwVID = dwVID;
	pack.dwTime = dwTime ? dwTime : get_dword_time();
	pack.bRot = bRot;
	pack.lX = x;
	pack.lY = y;
	pack.dwDuration	= dwDuration;
}

void CHARACTER::RestartAtSamePos()
{
	if (m_bIsObserver)
		return;

	EncodeRemovePacket(this);
	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;

		EncodeRemovePacket(entity);
		if (!m_bIsObserver)
			EncodeInsertPacket(entity);

		if( entity->IsType(ENTITY_CHARACTER) )
		{
			LPCHARACTER lpChar = (LPCHARACTER)entity;
			if( lpChar->IsPC() || lpChar->IsNPC() || lpChar->IsMonster()
#ifdef ENABLE_BOT_PLAYER
				|| lpChar->IsBotCharacter()
#endif
				)
			{
				if (!entity->IsObserverMode())
					entity->EncodeInsertPacket(this);
			}
		}
		else
		{
			if( !entity->IsObserverMode())
			{
				entity->EncodeInsertPacket(this);
			}
		}
	}
}

// #define ENABLE_SHOWNPCLEVEL
// Sends a packet to the entity stating that I have appeared.
void CHARACTER::EncodeInsertPacket(LPENTITY entity)
{
	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	LPCHARACTER ch = (LPCHARACTER) entity;
	ch->SendGuildName(GetGuild());

	TPacketGCCharacterAdd pack;

	pack.header = HEADER_GC_CHARACTER_ADD;
	pack.dwVID = m_vid;

#ifdef ENABLE_SHOW_MOB_INFO
	if (IsMonster() || IsStone())
	{
		pack.dwLevel = GetLevel();
		pack.dwAIFlag = IsMonster() ? GetAIFlag() : 0;
	}
	else
	{
		pack.dwLevel = 0;
		pack.dwAIFlag = 0;
	}
#endif

	pack.bType = GetCharType();
	pack.angle = GetRotation();
	pack.x = GetX();
	pack.y = GetY();
	pack.z = GetZ();
	pack.wRaceNum = GetRaceNum();

	if (IsPet())
	{
		pack.bMovingSpeed = 150;
	}
	else
	{
		pack.bMovingSpeed = GetLimitPoint(POINT_MOV_SPEED);
	}

	pack.bAttackSpeed = GetLimitPoint(POINT_ATT_SPEED);
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];

	pack.bStateFlag = m_bAddChrState;

	int iDur = 0;

	if (m_posDest.x != pack.x || m_posDest.y != pack.y)
	{
		iDur = (m_dwMoveStartTime + m_dwMoveDuration) - get_dword_time();

		if (iDur <= 0)
		{
			pack.x = m_posDest.x;
			pack.y = m_posDest.y;
		}
	}

	d->Packet(&pack, sizeof(pack));

	if (IsPC() == true || CMobVnumHelper::IsNPCType(m_bCharType)
#ifdef ENABLE_BOT_PLAYER
		|| IsBotCharacter()
#endif
	)
	{
		TPacketGCCharacterAdditionalInfo addPacket;
		memset(&addPacket, 0, sizeof(TPacketGCCharacterAdditionalInfo));

		addPacket.header = HEADER_GC_CHAR_ADDITIONAL_INFO;
		addPacket.dwVID = m_vid;

		addPacket.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
		addPacket.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
		addPacket.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
		addPacket.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		addPacket.awPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		addPacket.awPart[CHR_EQUIPPART_AURA] = GetPart(PART_AURA);
#endif

		addPacket.bPKMode = m_bPKMode;
		addPacket.dwMountVnum = GetMountVnum();
		addPacket.bEmpire = m_bEmpire;
		addPacket.dwLevel = 0;
		addPacket.dwGuildID = 0;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		if (IsGM() && GetCountryFlagFromGMList(1) != 0 && GetCountryFlagFromGMList(2) == LOCALE_MAX_NUM)
		{
			addPacket.bLanguage = (IsPC()) ? GetCountryFlagFromGMList(1) : 0;
			addPacket.bLanguage2 = 0;
		}
		else if (IsGM() && GetCountryFlagFromGMList(2) != 0 && GetCountryFlagFromGMList(2) < LOCALE_MAX_NUM)
		{
			addPacket.bLanguage = (IsPC()) ? GetCountryFlagFromGMList(1) : 0;
			addPacket.bLanguage2 = (IsPC()) ? GetCountryFlagFromGMList(2) : 0;
		}
		else
		{
			addPacket.bLanguage = GetLanguage();
			addPacket.bLanguage2 = 0;
		}
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		memcpy(addPacket.dwSkillColor, GetSkillColor(), sizeof(addPacket.dwSkillColor));
#endif

		if (IsPC() == true
#ifdef ENABLE_BOT_PLAYER
			|| IsBotCharacter()
#endif
		)
		{
			addPacket.dwLevel = GetLevel();
		}
		else
		{
			addPacket.dwLevel = 0;
		}

#ifdef ENABLE_GROWTH_PET_SYSTEM
		if (IsGrowthPet())
		{
			addPacket.dwLevel = GetLevel();
			addPacket.bCharacterSize = GetCharacterSize();
		}
#endif

		if (false)
		{
			LPCHARACTER ch = (LPCHARACTER) entity;

			if (GetEmpire() == ch->GetEmpire() || ch->GetGMLevel() > GM_PLAYER || m_bCharType == CHAR_TYPE_NPC)
			{
				goto show_all_info;
			}
			else
			{
				memset(addPacket.name, 0, CHARACTER_NAME_MAX_LEN);
				addPacket.dwGuildID = 0;
				addPacket.sAlignment = 0;
#ifdef ENABLE_TITLE_SYSTEM
				addPacket.iTitleID = 0;
#endif
			}
		}
		else
		{
		show_all_info:
			strlcpy(addPacket.name, GetName(), sizeof(addPacket.name));

			if (GetGuild() != NULL)
			{
				addPacket.dwGuildID = GetGuild()->GetID();

#ifdef ENABLE_GUILD_LEADER_TEXTAIL
				CGuild* pGuild = this->GetGuild();
				if (pGuild->GetMasterPID() == GetPlayerID())
					addPacket.dwGuildLeader = 3;
				else if (pGuild->IsGuildGeneral(GetPlayerID()) == true)
					addPacket.dwGuildLeader = 2;
				else
					addPacket.dwGuildLeader = 1;
#endif
			}
			else
			{
				addPacket.dwGuildID = 0;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
				addPacket.dwGuildLeader = 0;
#endif
			}

			addPacket.sAlignment = m_iAlignment / 10;
#ifdef ENABLE_TITLE_SYSTEM
			addPacket.iTitleID = GetPoint(POINT_TITLE);
#endif
		}

		d->Packet(&addPacket, sizeof(TPacketGCCharacterAdditionalInfo));
	}

	if (iDur)
	{
		TPacketGCMove pack;
		EncodeMovePacket(pack, GetVID(), FUNC_MOVE, 0, m_posDest.x, m_posDest.y, iDur, 0, (BYTE) (GetRotation() / 5));
		d->Packet(&pack, sizeof(pack));

		TPacketGCWalkMode p;
		p.vid = GetVID();
		p.header = HEADER_GC_WALK_MODE;
		p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

		d->Packet(&p, sizeof(p));
	}

	if (entity->IsType(ENTITY_CHARACTER) && GetDesc())
	{
		LPCHARACTER ch = (LPCHARACTER) entity;
		if (ch->IsWalking())
		{
			TPacketGCWalkMode p;
			p.vid = ch->GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = ch->m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;
			GetDesc()->Packet(&p, sizeof(p));
		}
	}

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	if (IsOfflineShopNPC())
	{
		LPOFFLINESHOP shop = GetOfflineShop();
		if (shop)
		{
			TPacketGCShopSign p;
			p.bHeader = HEADER_GC_OFFLINE_SHOP_SIGN;
			p.dwVID = GetVID();
			strlcpy(p.szSign, shop->GetShopSign(), sizeof(p.szSign));
			d->Packet(&p, sizeof(TPacketGCShopSign));
		}
	}
#endif

	if (GetMyShop())
	{
		TPacketGCShopSign p;

		p.bHeader = HEADER_GC_SHOP_SIGN;
		p.dwVID = GetVID();
		strlcpy(p.szSign, m_stShopSign.c_str(), sizeof(p.szSign));

		d->Packet(&p, sizeof(TPacketGCShopSign));
	}

	if (entity->IsType(ENTITY_CHARACTER))
	{
		sys_log(3, "EntityInsert %s (RaceNum %d) (%d %d) TO %s", GetName(), GetRaceNum(), GetX() / SECTREE_SIZE, GetY() / SECTREE_SIZE, ((LPCHARACTER)entity)->GetName());
	}
}

void CHARACTER::EncodeRemovePacket(LPENTITY entity)
{
	if (entity->GetType() != ENTITY_CHARACTER)
		return;

	LPDESC d;

	if (!(d = entity->GetDesc()))
		return;

	TPacketGCCharacterDelete pack;

	pack.header	= HEADER_GC_CHARACTER_DEL;
	pack.id	= m_vid;

	d->Packet(&pack, sizeof(TPacketGCCharacterDelete));

	if (entity->IsType(ENTITY_CHARACTER))
		sys_log(3, "EntityRemove %s(%d) FROM %s", GetName(), (DWORD) m_vid, ((LPCHARACTER) entity)->GetName());
}

void CHARACTER::UpdatePacket()
{
	if (GetSectree() == NULL)
		return;

	if (!this)
	{
		sys_err( "There's no one here!" );
		return;
	}

	if (IsPC() && (!GetDesc() || !GetDesc()->GetCharacter()))
		return;// @fixme190

#ifdef ENABLE_COMPUTE_POINT_FIX
	// UpdatePacket() is called several times during ComputePoints(),
	// e.g. when re-applying the equipped items. Since that happens for every
	// item equipped, the first update packets will contain invalid parts etc.
	if (!m_pointsInstant.computed)
		return;
#endif

	TPacketGCCharacterUpdate pack;
	TPacketGCCharacterUpdate pack2;

	pack.header = HEADER_GC_CHARACTER_UPDATE;
	pack.dwVID = m_vid;

	pack.awPart[CHR_EQUIPPART_ARMOR] = GetPart(PART_MAIN);
	pack.awPart[CHR_EQUIPPART_WEAPON] = GetPart(PART_WEAPON);
	pack.awPart[CHR_EQUIPPART_HEAD] = GetPart(PART_HEAD);
	pack.awPart[CHR_EQUIPPART_HAIR] = GetPart(PART_HAIR);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	pack.awPart[CHR_EQUIPPART_ACCE] = GetPart(PART_ACCE);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	pack.awPart[CHR_EQUIPPART_AURA] = GetPart(PART_AURA);
#endif

	pack.bMovingSpeed = GetLimitPoint(POINT_MOV_SPEED);
	pack.bAttackSpeed = GetLimitPoint(POINT_ATT_SPEED);
	pack.bStateFlag = m_bAddChrState;
	pack.dwAffectFlag[0] = m_afAffectFlag.bits[0];
	pack.dwAffectFlag[1] = m_afAffectFlag.bits[1];
	pack.dwGuildID = 0;
	pack.sAlignment = m_iAlignment / 10;
#ifdef ENABLE_TITLE_SYSTEM
	pack.iTitleID = GetPoint(POINT_TITLE);
#endif
	pack.bPKMode = m_bPKMode;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	if (IsGM() && GetCountryFlagFromGMList(1) != 0 && GetCountryFlagFromGMList(2) == LOCALE_MAX_NUM)
	{
		pack.bLanguage = (IsPC()) ? GetCountryFlagFromGMList(1) : 0;
		pack.bLanguage2 = 0;
	}
	else if (IsGM() && GetCountryFlagFromGMList(2) != 0 && GetCountryFlagFromGMList(2) < LOCALE_MAX_NUM)
	{
		pack.bLanguage = (IsPC()) ? GetCountryFlagFromGMList(1) : 0;
		pack.bLanguage2 = (IsPC()) ? GetCountryFlagFromGMList(2) : 0;
	}
	else
	{
		pack.bLanguage = GetLanguage();
		pack.bLanguage2 = 0;
	}
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	memcpy(pack.dwSkillColor, GetSkillColor(), sizeof(pack.dwSkillColor));
#endif

	if (GetGuild())
		pack.dwGuildID = GetGuild()->GetID();

	pack.dwMountVnum = GetMountVnum();

#ifdef ENABLE_GUILD_LEADER_TEXTAIL
	CGuild* pGuild = this->GetGuild();
	if (pGuild)
	{
		if (pGuild->GetMasterPID() == GetPlayerID())
			pack.dwGuildLeader = 3;
		else if (pGuild->IsGuildGeneral(GetPlayerID()) == true)
			pack.dwGuildLeader = 2;
		else
			pack.dwGuildLeader = 1;
	}
	else
	{
		pack.dwGuildLeader = 0;
	}
#endif

	pack2 = pack;
	pack2.dwGuildID = 0;
	pack2.sAlignment = 0;

	if (false)
	{
		if (m_bIsObserver != true)
		{
			for (ENTITY_MAP::iterator iter = m_map_view.begin(); iter != m_map_view.end(); iter++)
			{
				LPENTITY pEntity = iter->first;

				if (pEntity != NULL)
				{
					if (pEntity->IsType(ENTITY_CHARACTER) == true)
					{
						if (pEntity->GetDesc() != NULL)
						{
							LPCHARACTER pChar = (LPCHARACTER)pEntity;

							if (GetEmpire() == pChar->GetEmpire() || pChar->GetGMLevel() > GM_PLAYER)
							{
								pEntity->GetDesc()->Packet(&pack, sizeof(pack));
							}
							else
							{
								pEntity->GetDesc()->Packet(&pack2, sizeof(pack2));
							}
						}
					}
					else
					{
						if (pEntity->GetDesc() != NULL)
						{
							pEntity->GetDesc()->Packet(&pack, sizeof(pack));
						}
					}
				}
			}
		}

		if (GetDesc() != NULL)
		{
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}
	else
	{
		PacketAround(&pack, sizeof(pack));
	}
}

LPCHARACTER CHARACTER::FindCharacterInView(const char * c_pszName, bool bFindPCOnly)
{
	ENTITY_MAP::iterator it = m_map_view.begin();

	for (; it != m_map_view.end(); ++it)
	{
		if (!it->first->IsType(ENTITY_CHARACTER))
			continue;

		LPCHARACTER tch = (LPCHARACTER) it->first;

		if (bFindPCOnly && tch->IsNPC())
			continue;

		if (!strcasecmp(tch->GetName(), c_pszName))
			return (tch);
	}

	return NULL;
}

// Fix
bool CHARACTER::IsVictimInView(LPCHARACTER victim) const
{
	return victim ? m_map_view.find(victim) != m_map_view.end() : false;
}

void CHARACTER::SetPosition(int pos)
{
	if (pos == POS_STANDING)
	{
		REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_STUN);

		event_cancel(&m_pkDeadEvent);
#ifdef ENABLE_BOT_PLAYER
		event_cancel(&m_pkBotCharacterDeadEvent);
#endif
		event_cancel(&m_pkStunEvent);
	}
	else if (pos == POS_DEAD)
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_DEAD);

	if (!IsStone())
	{
		switch (pos)
		{
			case POS_FIGHTING:
				if (!IsState(m_stateBattle))
					MonsterLog("[BATTLE] ½Î¿ì´Â »óÅÂ");

				GotoState(m_stateBattle);
				break;

			default:
				if (!IsState(m_stateIdle))
					MonsterLog("[IDLE] ½¬´Â »óÅÂ");

				GotoState(m_stateIdle);
				break;
		}
	}

	m_pointsInstant.position = pos;
}

void CHARACTER::Save()
{
	if (!m_bSkipSave)
		CHARACTER_MANAGER::instance().DelayedSave(this);
}

void CHARACTER::CreatePlayerProto(TPlayerTable & tab)
{
	memset(&tab, 0, sizeof(TPlayerTable));

	if (GetNewName().empty())
	{
		strlcpy(tab.name, GetName(), sizeof(tab.name));
	}
	else
	{
		strlcpy(tab.name, GetNewName().c_str(), sizeof(tab.name));
	}

	strlcpy(tab.ip, GetDesc()->GetHostName(), sizeof(tab.ip));

	tab.id = m_dwPlayerID;
	tab.voice = GetPoint(POINT_VOICE);
	tab.level = GetLevel();
	tab.level_step = GetPoint(POINT_LEVEL_STEP);
	tab.exp = GetExp();
	tab.gold = GetGold();
	tab.job = m_points.job;
	tab.part_base = m_pointsInstant.bBasePart;
	tab.skill_group = m_points.skill_group;
#ifdef ENABLE_ANTI_EXP
	tab.anti_exp = GetAntiExp();
#endif
#ifdef ENABLE_BIOLOG_SYSTEM
	tab.m_BiologActualMission = GetBiologMissions();
	tab.m_BiologCollectedItems = GetBiologCollectedItems();
	tab.m_BiologCooldownReminder = GetBiologCooldownReminder();
	tab.m_BiologCooldown = GetBiologCooldown();
#endif
#ifdef ENABLE_RIDING_EXTENDED
	tab.mount_up_grade_exp = GetMountUpGradeExp();
	tab.mount_up_grade_fail = IsMountUpGradeFail();
#endif

	DWORD dwPlayedTime = (get_dword_time() - m_dwPlayStartTime);

	if (dwPlayedTime > 60000)
	{
		if (GetSectree() && !GetSectree()->IsAttr(GetX(), GetY(), ATTR_BANPK))
		{
			if (GetRealAlignment() < 0)
			{
				if (IsEquipUniqueItem(UNIQUE_ITEM_FASTER_ALIGNMENT_UP_BY_TIME))
					UpdateAlignment(120 * (dwPlayedTime / 60000));
				else
					UpdateAlignment(60 * (dwPlayedTime / 60000));
			}
			else
				UpdateAlignment(5 * (dwPlayedTime / 60000));
		}

		SetRealPoint(POINT_PLAYTIME, GetRealPoint(POINT_PLAYTIME) + dwPlayedTime / 60000);
		ResetPlayTime(dwPlayedTime % 60000);
	}

	tab.playtime = GetRealPoint(POINT_PLAYTIME);
	tab.lAlignment = m_iRealAlignment;

	if (m_posWarp.x != 0 || m_posWarp.y != 0)
	{
		tab.x = m_posWarp.x;
		tab.y = m_posWarp.y;
		tab.z = 0;
		tab.lMapIndex = m_lWarpMapIndex;
	}
	else
	{
		tab.x = GetX();
		tab.y = GetY();
		tab.z = GetZ();
		tab.lMapIndex = GetMapIndex();
	}

	if (m_lExitMapIndex == 0)
	{
		tab.lExitMapIndex = tab.lMapIndex;
		tab.lExitX = tab.x;
		tab.lExitY = tab.y;
	}
	else
	{
		tab.lExitMapIndex = m_lExitMapIndex;
		tab.lExitX = m_posExit.x;
		tab.lExitY = m_posExit.y;
	}

	sys_log(0, "SAVE: %s %dx%d", GetName(), tab.x, tab.y);

	tab.st = GetRealPoint(POINT_ST);
	tab.ht = GetRealPoint(POINT_HT);
	tab.dx = GetRealPoint(POINT_DX);
	tab.iq = GetRealPoint(POINT_IQ);

	tab.stat_point = GetPoint(POINT_STAT);
	tab.skill_point = GetPoint(POINT_SKILL);
	tab.sub_skill_point = GetPoint(POINT_SUB_SKILL);
	tab.horse_skill_point = GetPoint(POINT_HORSE_SKILL);

	tab.stat_reset_count = GetPoint(POINT_STAT_RESET_COUNT);

	tab.hp = GetHP();
	tab.sp = GetSP();

	tab.stamina = GetStamina();

	tab.sRandomHP = m_points.iRandomHP;
	tab.sRandomSP = m_points.iRandomSP;

	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
		tab.quickslot[i] = m_quickslot[i];

	thecore_memcpy(tab.parts, m_pointsInstant.parts, sizeof(tab.parts));
	thecore_memcpy(tab.skills, m_pSkillLevels, sizeof(TPlayerSkill) * SKILL_MAX_NUM);

	tab.horse = GetHorseData();

#ifdef ENABLE_RENEWAL_BATTLE_PASS
	tab.battle_pass_premium_id = GetExtBattlePassPremiumID();
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	tab.shopFlag = GetOfflineShopFlag();
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	tab.dwPickUPMode = GetPickUPMode();
#endif

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	for (int i = 0; i < UNLOCK_INVENTORY_MAX; ++i)
		tab.inventory_unlock[i] = GetUnlockSlotsW(i);
#endif

#ifdef ENABLE_RENEWAL_BONUS_BOARD
	tab.llMonsterKilled = GetMonsterKilled();
	tab.llStoneKilled = GetStoneKilled();
	tab.llBossKilled = GetBossKilled();
	tab.llBluePlayerKilled = GetBluePlayerKilled();
	tab.llYellowPlayerKilled = GetYellowPlayerKilled();
	tab.llRedPlayerKilled = GetRedPlayerKilled();
	tab.llAllPlayerKilled = GetAllPlayerKilled();
	tab.llDuelWon = GetDuelWon();
	tab.llDuelLost = GetDuelLose();
#endif
}

void CHARACTER::SaveReal()
{
	if (m_bSkipSave)
		return;

	if (!GetDesc())
	{
		sys_err("Character::Save : no descriptor when saving (name: %s)", GetName());
		return;
	}

	TPlayerTable table;
	CreatePlayerProto(table);

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_SAVE, GetDesc()->GetHandle(), &table, sizeof(TPlayerTable));

	quest::PC * pkQuestPC = quest::CQuestManager::instance().GetPCForce(GetPlayerID());

	if (!pkQuestPC)
		sys_err("CHARACTER::Save : null quest::PC pointer! (name %s)", GetName());
	else
	{
		pkQuestPC->Save();
	}

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
	if (pMarriage)
		pMarriage->Save();
}

void CHARACTER::FlushDelayedSaveItem()
{
	LPITEM item;

	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
		if ((item = GetInventoryItem(i)))
			ITEM_MANAGER::instance().FlushDelayedSave(item);
}

void CHARACTER::Disconnect(const char * c_pszReason)
{
	assert(GetDesc() != NULL);

#ifdef __AUTO_HUNT__
	// Oyuncu disconnect olduðunda tüm rezervasyonlarýný kaldýr
	if (IsPC() && GetPlayerID())
	{
		CHARACTER_MANAGER::instance().ReleaseAllTargetsByPlayer(GetPlayerID());
	}
#endif

	sys_log(0, "DISCONNECT: %s (%s)", GetName(), c_pszReason ? c_pszReason : "unset" );

	if (GetShop())
	{
		GetShop()->RemoveGuest(this);
		SetShop(NULL);
	}

	if (GetArena() != NULL)
	{
		GetArena()->OnDisconnect(GetPlayerID());
	}

	if (GetParty() != NULL)
	{
		GetParty()->UpdateOfflineState(GetPlayerID());
	}

	// Fix
	if (m_pkExchange != NULL)
	{
		m_pkExchange->Cancel();
		m_pkExchange = NULL;
	}

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	if (GetOfflineShop())
	{
		GetOfflineShop()->RemoveGuest(this);
		SetOfflineShop(NULL);
	}
#endif

	marriage::CManager::instance().Logout(this);
#ifdef ENABLE_MULTI_FARM_BLOCK
	CHARACTER_MANAGER::instance().CheckMultiFarmAccount(GetDesc() ? GetDesc()->GetHostName() : "", GetPlayerID(), GetName(), false);
#endif
	// P2P Logout
	TPacketGGLogout p;
	p.bHeader = HEADER_GG_LOGOUT;
	strlcpy(p.szName, GetName(), sizeof(p.szName));
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogout));
	char buf[51];
#ifdef ENABLE_GOLD_LIMIT
	snprintf(buf, sizeof(buf), "%s %lld %d %ld %d", 
#else
	snprintf(buf, sizeof(buf), "%s %d %d %ld %d",
#endif
		inet_ntoa(GetDesc()->GetAddr().sin_addr), GetGold(), g_bChannel, GetMapIndex(), GetAlignment());

	LogManager::instance().CharLog(this, 0, "LOGOUT", buf);

	if (m_pWarMap)
		SetWarMap(NULL);

	if (m_pWeddingMap)
	{
		SetWeddingMap(NULL);
	}

	if (GetGuild())
		GetGuild()->LogoutMember(this);

	quest::CQuestManager::instance().LogoutPC(this);

#ifdef ENABLE_RENEWAL_BATTLE_PASS
	ListExtBattlePassMap::iterator itext = m_listExtBattlePass.begin();
	while (itext != m_listExtBattlePass.end())
	{
		TPlayerExtBattlePassMission* pkMission = *itext++;

		if (!pkMission->bIsUpdated)
			continue;

		db_clientdesc->DBPacket(HEADER_GD_SAVE_EXT_BATTLE_PASS, 0, pkMission, sizeof(TPlayerExtBattlePassMission));
	}
	m_bIsLoadedExtBattlePass = false;
#endif

	if (GetParty())
		GetParty()->Unlink(this);

	if (IsStun() || IsDead())
	{
		DeathPenalty(0);
		PointChange(POINT_HP, 50 - GetHP());
	}

	if (!CHARACTER_MANAGER::instance().FlushDelayedSave(this))
	{
		SaveReal();
	}

#ifdef ENABLE_BIOLOG_SYSTEM
	if (m_pkBiologManager)
	{
		delete m_pkBiologManager;
		m_pkBiologManager = NULL;
	}
#endif

	FlushDelayedSaveItem();

	SaveAffect();
	m_bIsLoadedAffect = false;

	m_bSkipSave = true;

	quest::CQuestManager::instance().DisconnectPC(this);

	CloseSafebox();

	CloseMall();

	CPVPManager::instance().Disconnect(this);

	CTargetManager::instance().Logout(GetPlayerID());

	MessengerManager::instance().Logout(GetName());

	// Fix
	if (GetMapIndex() == OXEVENT_MAP_INDEX)
	{
		COXEventManager::instance().RemoveFromAttenderList(GetPlayerID());
	}

#ifdef ENABLE_MOUNT_SYSTEM
	if(GetMountVnum())
	{
		RemoveAffect(AFFECT_MOUNT);
		RemoveAffect(AFFECT_MOUNT_BONUS);
	}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	if (IsAuraRefineWindowOpen())
		AuraRefineWindowClose();
#endif

	if (GetDesc())
	{
		packet_point_change pack;
		pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
		pack.dwVID = m_vid;
		pack.type = POINT_PLAYTIME;
		pack.value = GetRealPoint(POINT_PLAYTIME) + (get_dword_time() - m_dwPlayStartTime) / 60000;
		pack.amount = 0;
		GetDesc()->Packet(&pack, sizeof(struct packet_point_change));
		GetDesc()->BindCharacter(NULL);
	}

	M2_DESTROY_CHARACTER(this);
}

#ifdef ENABLE_SHOW_MOB_INFO
bool CHARACTER::Show(long lMapIndex, long x, long y, long z, bool bShowSpawnMotion, bool bAggressive)
#else
bool CHARACTER::Show(long lMapIndex, long x, long y, long z, bool bShowSpawnMotion)
#endif
{
	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "cannot find sectree by %dx%d mapindex %d", x, y, lMapIndex);
		return false;
	}

	SetMapIndex(lMapIndex);

	bool bChangeTree = false;

	if (!GetSectree() || GetSectree() != sectree)
		bChangeTree = true;

	if (bChangeTree)
	{
		if (GetSectree())
			GetSectree()->RemoveEntity(this);

		ViewCleanup(IsPC());
	}

	if (!IsNPC())
	{
		sys_log(0, "SHOW: %s %dx%dx%d", GetName(), x, y, z);
		if (GetStamina() < GetMaxStamina())
			StartAffectEvent();
	}
	else if (m_pkMobData && m_pkMobInst)
	{
		m_pkMobInst->m_posLastAttacked.x = x;
		m_pkMobInst->m_posLastAttacked.y = y;
		m_pkMobInst->m_posLastAttacked.z = z;
	}

	if (bShowSpawnMotion)
	{
		SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
		m_afAffectFlag.Set(AFF_SPAWN);
	}

#ifdef ENABLE_SHOW_MOB_INFO
	if (bAggressive)
		SetAggressive();
#endif

	SetXYZ(x, y, z);

	m_posDest.x = x;
	m_posDest.y = y;
	m_posDest.z = z;

	m_posStart.x = x;
	m_posStart.y = y;
	m_posStart.z = z;

	if (bChangeTree)
	{
		EncodeInsertPacket(this);
		sectree->InsertEntity(this);

		UpdateSectree();
	}
	else
	{
		ViewReencode();
		sys_log(0, "      in same sectree");
	}

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	
	SetValidComboInterval(0);
	return true;
}

struct BGMInfo
{
	std::string name;
	float vol;
};

typedef std::map<unsigned, BGMInfo> BGMInfoMap;

static BGMInfoMap gs_bgmInfoMap;
static bool gs_bgmVolEnable = false;

void CHARACTER_SetBGMVolumeEnable()
{
	gs_bgmVolEnable = true;
	sys_log(0, "bgm_info.set_bgm_volume_enable");
}

void CHARACTER_AddBGMInfo(unsigned mapIndex, const char* name, float vol)
{
	BGMInfo newInfo;
	newInfo.name = name;
	newInfo.vol = vol;

	gs_bgmInfoMap[mapIndex] = newInfo;

	sys_log(0, "bgm_info.add_info(%d, '%s', %f)", mapIndex, name, vol);
}

const BGMInfo& CHARACTER_GetBGMInfo(unsigned mapIndex)
{
	BGMInfoMap::iterator f = gs_bgmInfoMap.find(mapIndex);
	if (gs_bgmInfoMap.end() == f)
	{
		static BGMInfo s_empty = {"", 0.0f};
		return s_empty;
	}
	return f->second;
}

bool CHARACTER_IsBGMVolumeEnable()
{
	return gs_bgmVolEnable;
}

void CHARACTER::MainCharacterPacket()
{
	const unsigned mapIndex = GetMapIndex();
	const BGMInfo& bgmInfo = CHARACTER_GetBGMInfo(mapIndex);

	if (!bgmInfo.name.empty())
	{
		if (CHARACTER_IsBGMVolumeEnable())
		{
			sys_log(1, "bgm_info.play_bgm_vol(%d, name='%s', vol=%f)", mapIndex, bgmInfo.name.c_str(), bgmInfo.vol);
			TPacketGCMainCharacter4_BGM_VOL mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER4_BGM_VOL;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));

			mainChrPacket.fBGMVol = bgmInfo.vol;
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter4_BGM_VOL));
		}
		else
		{
			sys_log(1, "bgm_info.play(%d, '%s')", mapIndex, bgmInfo.name.c_str());
			TPacketGCMainCharacter3_BGM mainChrPacket;
			mainChrPacket.header = HEADER_GC_MAIN_CHARACTER3_BGM;
			mainChrPacket.dwVID = m_vid;
			mainChrPacket.wRaceNum = GetRaceNum();
			mainChrPacket.lx = GetX();
			mainChrPacket.ly = GetY();
			mainChrPacket.lz = GetZ();
			mainChrPacket.empire = GetDesc()->GetEmpire();
			mainChrPacket.skill_group = GetSkillGroup();
			strlcpy(mainChrPacket.szChrName, GetName(), sizeof(mainChrPacket.szChrName));
			strlcpy(mainChrPacket.szBGMName, bgmInfo.name.c_str(), sizeof(mainChrPacket.szBGMName));
			GetDesc()->Packet(&mainChrPacket, sizeof(TPacketGCMainCharacter3_BGM));
		}
	}
	else
	{
		sys_log(0, "bgm_info.play(%d, DEFAULT_BGM_NAME)", mapIndex);

		TPacketGCMainCharacter pack;
		pack.header = HEADER_GC_MAIN_CHARACTER;
		pack.dwVID = m_vid;
		pack.wRaceNum = GetRaceNum();
		pack.lx = GetX();
		pack.ly = GetY();
		pack.lz = GetZ();
		pack.empire = GetDesc()->GetEmpire();
		pack.skill_group = GetSkillGroup();
		strlcpy(pack.szName, GetName(), sizeof(pack.szName));
		GetDesc()->Packet(&pack, sizeof(TPacketGCMainCharacter));
	}
}

void CHARACTER::PointsPacket()
{
	if (!GetDesc())
		return;

	TPacketGCPoints pack;

	pack.header	= HEADER_GC_CHARACTER_POINTS;

	pack.points[POINT_LEVEL] = GetLevel();
	pack.points[POINT_EXP] = GetExp();
	pack.points[POINT_NEXT_EXP] = GetNextExp();
	pack.points[POINT_HP] = GetHP();
	pack.points[POINT_MAX_HP] = GetMaxHP();
	pack.points[POINT_SP] = GetSP();
	pack.points[POINT_MAX_SP] = GetMaxSP();
	pack.points[POINT_GOLD] = GetGold();
#ifdef ENABLE_COINS_INVENTORY
	pack.points[POINT_COINS] = GetCoins();
#endif
	pack.points[POINT_STAMINA] = GetStamina();
	pack.points[POINT_MAX_STAMINA] = GetMaxStamina();

	for (int i = POINT_ST; i < POINT_MAX_NUM; ++i)
		pack.points[i] = GetPoint(i);

#ifdef ENABLE_TITLE_SYSTEM
	for (int w = TITLE_0; w < TITLES_MAX_NUM; ++w)
	{
		BYTE bType = POINT_TITLE + w;
		pack.points[bType] = GetPoint(bType);
	}
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
	pack.points[POINT_BATTLE_PASS_PREMIUM_ID] = GetExtBattlePassPremiumID();
#endif

#ifdef ENABLE_RENEWAL_BONUS_BOARD
	pack.points[POINT_MONSTER_KILLED] = GetMonsterKilled();
	pack.points[POINT_STONE_KILLED] = GetStoneKilled();
	pack.points[POINT_BOSS_KILLED] = GetBossKilled();
	pack.points[POINT_BLUE_PLAYER_KILLED] = GetBluePlayerKilled();
	pack.points[POINT_YELLOW_PLAYER_KILLED] = GetYellowPlayerKilled();
	pack.points[POINT_RED_PLAYER_KILLED] = GetRedPlayerKilled();
	pack.points[POINT_ALL_PLAYER_KILLED] = GetAllPlayerKilled();
	pack.points[POINT_KILL_DUELWON] = GetDuelWon();
	pack.points[POINT_KILL_DUELLOST] = GetDuelLose();
#endif

	GetDesc()->Packet(&pack, sizeof(TPacketGCPoints));
}

bool CHARACTER::ChangeSex()
{
	int src_race = GetRaceNum();

	switch (src_race)
	{
		case MAIN_RACE_WARRIOR_M:
			m_points.job = MAIN_RACE_WARRIOR_W;
			break;

		case MAIN_RACE_WARRIOR_W:
			m_points.job = MAIN_RACE_WARRIOR_M;
			break;

		case MAIN_RACE_ASSASSIN_M:
			m_points.job = MAIN_RACE_ASSASSIN_W;
			break;

		case MAIN_RACE_ASSASSIN_W:
			m_points.job = MAIN_RACE_ASSASSIN_M;
			break;

		case MAIN_RACE_SURA_M:
			m_points.job = MAIN_RACE_SURA_W;
			break;

		case MAIN_RACE_SURA_W:
			m_points.job = MAIN_RACE_SURA_M;
			break;

		case MAIN_RACE_SHAMAN_M:
			m_points.job = MAIN_RACE_SHAMAN_W;
			break;

		case MAIN_RACE_SHAMAN_W:
			m_points.job = MAIN_RACE_SHAMAN_M;
			break;

		default:
			sys_err("CHANGE_SEX: %s unknown race %d", GetName(), src_race);
			return false;
	}

	// Fix
	UpdatePacket();
	SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	m_afAffectFlag.Set(AFF_SPAWN);
	ViewReencode();
	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	m_afAffectFlag.Reset(AFF_SPAWN);

	sys_log(0, "CHANGE_SEX: %s (%d -> %d)", GetName(), src_race, m_points.job);
	return true;
}

WORD CHARACTER::GetRaceNum() const
{
	if (m_dwPolymorphRace)
		return m_dwPolymorphRace;

	if (m_pkMobData)
		return m_pkMobData->m_table.dwVnum;

	return m_points.job;
}

WORD CHARACTER::GetPlayerRace() const
{
	return m_points.job;
}

void CHARACTER::SetRace(BYTE race)
{
	if (race >= MAIN_RACE_MAX_NUM)
	{
		sys_err("CHARACTER::SetRace(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
		return;
	}

	m_points.job = race;
}

BYTE CHARACTER::GetJob() const
{
	unsigned race = m_points.job;
	unsigned job;

	if (RaceToJob(race, &job))
		return job;

	sys_err("CHARACTER::GetJob(name=%s, race=%d).OUT_OF_RACE_RANGE", GetName(), race);
	return JOB_WARRIOR;
}

#ifdef ENABLE_LEVEL_INT
void CHARACTER::SetLevel(int level)
#else
void CHARACTER::SetLevel(BYTE level)
#endif
{
	m_points.level = level;

	if (IsPC())
	{
		if (level < PK_PROTECT_LEVEL)
			SetPKMode(PK_MODE_PROTECT);
		else if (GetGMLevel() != GM_PLAYER)
			SetPKMode(PK_MODE_PROTECT);
		else if (m_bPKMode == PK_MODE_PROTECT)
			SetPKMode(PK_MODE_PEACE);
	}
}

void CHARACTER::SetEmpire(BYTE bEmpire)
{
	m_bEmpire = bEmpire;
}

#ifdef ENABLE_COINS_INVENTORY
long long CHARACTER::GetCoins()
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT coins FROM account.account WHERE id = '%d';", GetDesc()->GetAccountTable().id));

	if (pMsg->Get()->uiNumRows == 0)
		return 0;

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	long long dc = 0;
	str_to_number(dc, row[0]);
	return dc;
}

void CHARACTER::SetCoins(long long coins)
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("UPDATE account.account SET coins = '%lld' WHERE id = '%d';", coins, GetDesc()->GetAccountTable().id));
	SetPoint(POINT_COINS, coins);
}
#endif

void CHARACTER::SetPlayerProto(const TPlayerTable * t)
{
	if (!GetDesc() || !*GetDesc()->GetHostName())
		sys_err("cannot get desc or hostname");
	else
		SetGMLevel();

	m_bCharType = CHAR_TYPE_PC;

	m_dwPlayerID = t->id;

	m_iAlignment = t->lAlignment;
	m_iRealAlignment = t->lAlignment;

	m_points.voice = t->voice;

	m_points.skill_group = t->skill_group; 

	m_pointsInstant.bBasePart = t->part_base;
	SetPart(PART_HAIR, t->parts[PART_HAIR]);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	SetPart(PART_ACCE, t->parts[PART_ACCE]);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	SetPart(PART_AURA, t->parts[PART_AURA]);
#endif

	m_points.iRandomHP = t->sRandomHP;
	m_points.iRandomSP = t->sRandomSP;

	if (m_pSkillLevels)
		M2_DELETE_ARRAY(m_pSkillLevels);

	m_pSkillLevels = M2_NEW TPlayerSkill[SKILL_MAX_NUM];
	thecore_memcpy(m_pSkillLevels, t->skills, sizeof(TPlayerSkill) * SKILL_MAX_NUM);

	if (t->lMapIndex >= 10000)
	{
		m_posWarp.x = t->lExitX;
		m_posWarp.y = t->lExitY;
		m_lWarpMapIndex = t->lExitMapIndex;
	}

	SetRealPoint(POINT_PLAYTIME, t->playtime);
	m_dwLoginPlayTime = t->playtime;
	SetRealPoint(POINT_ST, t->st);
	SetRealPoint(POINT_HT, t->ht);
	SetRealPoint(POINT_DX, t->dx);
	SetRealPoint(POINT_IQ, t->iq);

	SetPoint(POINT_ST, t->st);
	SetPoint(POINT_HT, t->ht);
	SetPoint(POINT_DX, t->dx);
	SetPoint(POINT_IQ, t->iq);

	SetPoint(POINT_STAT, t->stat_point);
	SetPoint(POINT_SKILL, t->skill_point);
	SetPoint(POINT_SUB_SKILL, t->sub_skill_point);
	SetPoint(POINT_HORSE_SKILL, t->horse_skill_point);

	SetPoint(POINT_STAT_RESET_COUNT, t->stat_reset_count);

	SetPoint(POINT_LEVEL_STEP, t->level_step);
	SetRealPoint(POINT_LEVEL_STEP, t->level_step);

	SetRace(t->job);

	SetLevel(t->level);
	SetExp(t->exp);
	SetGold(t->gold);
#ifdef ENABLE_ANTI_EXP
	SetAntiExp(t->anti_exp);
#endif
#ifdef ENABLE_RENEWAL_BATTLE_PASS
	SetExtBattlePassPremiumID(t->battle_pass_premium_id);
#endif
	SetMapIndex(t->lMapIndex);
	SetXYZ(t->x, t->y, t->z);

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	SetOfflineShopFlag(t->shopFlag);
#endif
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	SetPickUPMode(t->dwPickUPMode);
#endif
#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	for (int i = 0; i < UNLOCK_INVENTORY_MAX; ++i)
		SetUnlockSlotsW(t->inventory_unlock[i],i);
#endif
#ifdef ENABLE_RENEWAL_BONUS_BOARD
	SetMonsterKilled(t->llMonsterKilled);
	SetStoneKilled(t->llStoneKilled);
	SetBossKilled(t->llBossKilled);
	SetBluePlayerKilled(t->llBluePlayerKilled);
	SetYellowPlayerKilled(t->llYellowPlayerKilled);
	SetRedPlayerKilled(t->llRedPlayerKilled);
	SetAllPlayerKilled(t->llAllPlayerKilled);
	SetDuelWon(t->llDuelWon);
	SetDuelLose(t->llDuelLost);
#endif
#ifdef ENABLE_RIDING_EXTENDED
	SetMountUpGradeExp(t->mount_up_grade_exp);
	SetMountUpGradeFail(t->mount_up_grade_fail);
#endif

#ifdef ENABLE_SPIRIT_STONE_READING
	m_dwRuhSureFlag = 0;
	m_dwRuhYeniSureFlag = 0;
#endif

#ifdef ENABLE_SKILL_BOOK_READING
	m_dwBkYeniSureFlag = 0;
#endif

#ifdef ENABLE_HORSE_SPAWN_EXPLOIT_FIX
	m_dwHorseCheckerFlag = 0;
#endif

#ifdef ENABLE_CAMPFIRE_BUG_FIX
	m_dwCampfireFlag = 0;
#endif

	ComputePoints();

#ifdef ENABLE_WARP_HP_LOSS_FIX
	SetHP(GetMaxHP());
	SetSP(GetMaxSP());
#else
	SetHP(t->hp);
	SetSP(t->sp);
#endif
	SetStamina(t->stamina);

	if (!test_server)
	{
		if (GetGMLevel() > GM_LOW_WIZARD)
		{
			m_afAffectFlag.Set(AFF_YMIR);
			m_bPKMode = PK_MODE_PROTECT;
		}
	}

	if (GetLevel() < PK_PROTECT_LEVEL)
		m_bPKMode = PK_MODE_PROTECT;

	SetHorseData(t->horse);

	if (GetHorseLevel() > 0)
		UpdateHorseDataByLogoff(t->logoff_interval);

	thecore_memcpy(m_aiPremiumTimes, t->aiPremiumTimes, sizeof(t->aiPremiumTimes));

	m_dwLogOffInterval = t->logoff_interval;
	m_dwLastAttackTime = t->last_play;

	sys_log(0, "PLAYER_LOAD: %s PREMIUM %d %d, LOGGOFF_INTERVAL %u PTR: %p", t->name, m_aiPremiumTimes[0], m_aiPremiumTimes[1], t->logoff_interval, this);

	if (GetGMLevel() != GM_PLAYER)
	{
		LogManager::instance().CharLog(this, GetGMLevel(), "GM_LOGIN", "");
		sys_log(0, "GM_LOGIN(gmlevel=%d, name=%s(%d), pos=(%d, %d)", GetGMLevel(), GetName(), GetPlayerID(), GetX(), GetY());
	}

#ifdef ENABLE_MOUNT_SYSTEM
	if (m_mountSystem)
	{
		m_mountSystem->Destroy();
		delete m_mountSystem;
	}

	m_mountSystem = M2_NEW CMountSystem(this);
#endif

#ifdef ENABLE_PET_SYSTEM
	if (m_petSystem)
	{
		m_petSystem->Destroy();
		delete m_petSystem;
	}

	m_petSystem = M2_NEW CPetSystem(this);
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
	if (m_pkBiologManager)
	{
		sys_err("Biolog manager already exist for owner %u %s", GetPlayerID(), GetName());
		delete m_pkBiologManager;
	}

	m_pkBiologManager = M2_NEW CBiologSystem(this);

	SetBiologMissions(t->m_BiologActualMission);
	SetBiologCollectedItems(t->m_BiologCollectedItems);
	SetBiologCooldownReminder(t->m_BiologCooldownReminder);
	SetBiologCooldown(t->m_BiologCooldown);
#endif

#ifdef ENABLE_RENEWAL_TEAM_AFFECT
	if (GetGMLevel() == GM_IMPLEMENTOR)
	{
		m_afAffectFlag.Set(AFF_TEAM_SA);
		m_bPKMode = PK_MODE_PROTECT;
	}
	else if (GetGMLevel() == GM_HIGH_WIZARD)
	{
		m_afAffectFlag.Set(AFF_TEAM_GA);
		m_bPKMode = PK_MODE_PROTECT;
	}
	else if (GetGMLevel() == GM_GOD)
	{
		m_afAffectFlag.Set(AFF_TEAM_GM);
		m_bPKMode = PK_MODE_PROTECT;
	}
	else if (GetGMLevel() == GM_LOW_WIZARD)
	{
		m_afAffectFlag.Set(AFF_TEAM_TGM);
		m_bPKMode = PK_MODE_PROTECT;
	}
#endif

#ifdef ENABLE_RENEWAL_PREMIUM_SYSTEM
	LoadPremium();
#endif
}

EVENTFUNC(kill_ore_load_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "kill_ore_load_even> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL)
	{
		return 0;
	}

	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::SetProto(const CMob * pkMob)
{
	if (m_pkMobInst)
		M2_DELETE(m_pkMobInst);

	m_pkMobData = pkMob;
	m_pkMobInst = M2_NEW CMobInstance;

	m_bPKMode = PK_MODE_FREE;

	const TMobTable * t = &m_pkMobData->m_table;

	m_bCharType = t->bType;

	SetLevel(t->bLevel);
	SetEmpire(t->bEmpire);

	SetExp(t->dwExp);
	SetRealPoint(POINT_ST, t->bStr);
	SetRealPoint(POINT_DX, t->bDex);
	SetRealPoint(POINT_HT, t->bCon);
	SetRealPoint(POINT_IQ, t->bInt);

	ComputePoints();

	SetHP(GetMaxHP());
	SetSP(GetMaxSP());

	m_pointsInstant.dwAIFlag = t->dwAIFlag;
	SetImmuneFlag(t->dwImmuneFlag);

	AssignTriggers(t);

	ApplyMobAttribute(t);

	if (IsWarp() || IsGoto())
	{
		StartWarpNPCEvent();
	}

	CHARACTER_MANAGER::instance().RegisterRaceNumMap(this);

	if (GetRaceNum() == xmas::MOB_SANTA_VNUM)
	{
		SetPoint(POINT_ATT_GRADE_BONUS, 10);
		SetPoint(POINT_DEF_GRADE_BONUS, 6);

		m_dwPlayStartTime = get_dword_time() + 30 * 1000;
		if (test_server)
			m_dwPlayStartTime = get_dword_time() + 30 * 1000;
	}

	if (warmap::IsWarFlag(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlag, &CHARACTER::EndStateEmpty);
	}

	if (warmap::IsWarFlagBase(GetRaceNum()))
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateFlagBase, &CHARACTER::EndStateEmpty);
	}

	if (m_bCharType == CHAR_TYPE_HORSE ||
			GetRaceNum() == 20101 ||
			GetRaceNum() == 20102 ||
			GetRaceNum() == 20103 ||
			GetRaceNum() == 20104 ||
			GetRaceNum() == 20105 ||
			GetRaceNum() == 20106 ||
			GetRaceNum() == 20107 ||
			GetRaceNum() == 20108 ||
			GetRaceNum() == 20109
#ifdef ENABLE_RIDING_EXTENDED
			|| GetRaceNum() == 20149
			|| GetRaceNum() == 20150
			|| GetRaceNum() == 20151
#endif
	  )
	{
		m_stateIdle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
		m_stateMove.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateMove, &CHARACTER::EndStateEmpty);
		m_stateBattle.Set(this, &CHARACTER::BeginStateEmpty, &CHARACTER::StateHorse, &CHARACTER::EndStateEmpty);
	}

	// MINING
	if (mining::IsVeinOfOre (GetRaceNum()))
	{
		char_event_info* info = AllocEventInfo<char_event_info>();
		info->ch = this;
		m_pkMiningEvent = event_create(kill_ore_load_event, info, PASSES_PER_SEC(number(7 * 60, 15 * 60)));
	}
	// END_OF_MINING
}

const TMobTable & CHARACTER::GetMobTable() const
{
	return m_pkMobData->m_table;
}

bool CHARACTER::IsRaceFlag(DWORD dwBit) const
{
	return m_pkMobData ? IS_SET(m_pkMobData->m_table.dwRaceFlag, dwBit) : 0;
}

DWORD CHARACTER::GetMobDamageMin() const
{
	return m_pkMobData->m_table.dwDamageRange[0];
}

DWORD CHARACTER::GetMobDamageMax() const
{
	return m_pkMobData->m_table.dwDamageRange[1];
}

float CHARACTER::GetMobDamageMultiply() const
{
	float fDamMultiply = GetMobTable().fDamMultiply;

	if (IsBerserk())
		fDamMultiply = fDamMultiply * 2.0f;

	return fDamMultiply;
}

DWORD CHARACTER::GetMobDropItemVnum() const
{
	return m_pkMobData->m_table.dwDropItemVnum;
}

bool CHARACTER::IsSummonMonster() const
{
	return GetSummonVnum() != 0;
}

DWORD CHARACTER::GetSummonVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwSummonVnum : 0;
}

DWORD CHARACTER::GetPolymorphItemVnum() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwPolymorphItemVnum : 0;
}

DWORD CHARACTER::GetMonsterDrainSPPoint() const
{
	return m_pkMobData ? m_pkMobData->m_table.dwDrainSP : 0;
}

BYTE CHARACTER::GetMobRank() const
{
	if (!m_pkMobData)
		return MOB_RANK_KNIGHT;

	return m_pkMobData->m_table.bRank;
}

BYTE CHARACTER::GetMobSize() const
{
	if (!m_pkMobData)
		return MOBSIZE_MEDIUM;

	return m_pkMobData->m_table.bSize;
}

WORD CHARACTER::GetMobAttackRange() const
{
	switch (GetMobBattleType())
	{
		case BATTLE_TYPE_RANGE:
		case BATTLE_TYPE_MAGIC:
			return m_pkMobData->m_table.wAttackRange + GetPoint(POINT_BOW_DISTANCE);
		default:
			return m_pkMobData->m_table.wAttackRange;
	}
}

BYTE CHARACTER::GetMobBattleType() const
{
	if (!m_pkMobData)
		return BATTLE_TYPE_MELEE;

	return (m_pkMobData->m_table.bBattleType);
}

void CHARACTER::ComputeBattlePoints()
{
	if (IsPolymorphed())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
		int iAtt = 0;
		int iDef = 0;

		if (pMob)
		{
			iAtt = GetLevel() * 2 + GetPolymorphPoint(POINT_ST) * 2;
			// lev + con
			iDef = GetLevel() + GetPolymorphPoint(POINT_HT) + pMob->m_table.wDef;
		}

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
	else if (IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| IsBotCharacter()
#endif
	)
	{
		SetPoint(POINT_ATT_GRADE, 0);
		SetPoint(POINT_DEF_GRADE, 0);
		SetPoint(POINT_CLIENT_DEF_GRADE, 0);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));

		int iAtk = GetLevel() * 2;
		int iStatAtk = 0;

		switch (GetJob())
		{
			case JOB_WARRIOR:
			case JOB_SURA:
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;

			case JOB_ASSASSIN:
				iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_DX)) / 3;
				break;

			case JOB_SHAMAN:
				iStatAtk = (4 * GetPoint(POINT_ST) + 2 * GetPoint(POINT_IQ)) / 3;
				break;

			default:
				sys_err("invalid job %d", GetJob());
				iStatAtk = (2 * GetPoint(POINT_ST));
				break;
		}

		if (GetMountVnum() && iStatAtk < 2 * GetPoint(POINT_ST))
			iStatAtk = (2 * GetPoint(POINT_ST));

		iAtk += iStatAtk;

		if (GetMountVnum())
		{
			if (GetJob() == JOB_SURA && GetSkillGroup() == 1)
			{
				iAtk += (iAtk * GetHorseLevel()) / 60;
			}
			else
			{
				iAtk += (iAtk * GetHorseLevel()) / 30;
			}
		}

		iAtk += GetPoint(POINT_ATT_GRADE_BONUS);

		PointChange(POINT_ATT_GRADE, iAtk);

		int iShowDef = GetLevel() + GetPoint(POINT_HT);
		int iDef = GetLevel() + (int) (GetPoint(POINT_HT) / 1.25);
		int iArmor = 0;

		LPITEM pkItem;

		for (int i = 0; i < WEAR_MAX_NUM; ++i)
		{
			if ((pkItem = GetWear(i)) && pkItem->GetType() == ITEM_ARMOR)
			{
				if (pkItem->GetSubType() == ARMOR_BODY || pkItem->GetSubType() == ARMOR_HEAD || pkItem->GetSubType() == ARMOR_FOOTS || pkItem->GetSubType() == ARMOR_SHIELD)
				{
					iArmor += pkItem->GetValue(1);
					iArmor += (2 * pkItem->GetValue(5));
				}
			}
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			else if (pkItem && pkItem->GetType() == ITEM_COSTUME && pkItem->GetSubType() == COSTUME_AURA)
			{
				const long c_lLevelSocket = pkItem->GetSocket(ITEM_SOCKET_AURA_CURRENT_LEVEL);
				const long c_lDrainSocket = pkItem->GetSocket(ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM);
				const long c_lBoostSocket = pkItem->GetSocket(ITEM_SOCKET_AURA_BOOST);

				BYTE bCurLevel = (c_lLevelSocket / 100000) - 1000;
				BYTE bBoostIndex = c_lBoostSocket / 100000000;

				TItemTable* pBoosterProto = ITEM_MANAGER::instance().GetTable(ITEM_AURA_BOOST_ITEM_VNUM_BASE + bBoostIndex);
				float fAuraDrainPer = (1.0f * bCurLevel / 10.0f) / 100.0f;
				if (pBoosterProto)
					fAuraDrainPer += 1.0f * pBoosterProto->alValues[ITEM_AURA_BOOST_PERCENT_VALUE] / 100.0f;

				TItemTable* pDrainedItem = NULL;
				if (c_lDrainSocket != 0)
					pDrainedItem = ITEM_MANAGER::instance().GetTable(c_lDrainSocket);
				if (pDrainedItem != NULL && pDrainedItem->bType == ITEM_ARMOR && pDrainedItem->bSubType == ARMOR_SHIELD)
				{
					float fValue = (pDrainedItem->alValues[1] + (2 * pDrainedItem->alValues[5])) * fAuraDrainPer;
					iArmor += static_cast<int>((fValue < 1.0f) ? ceilf(fValue) : truncf(fValue));;
				}
			}
#endif
		}

		if( true == IsHorseRiding() )
		{
			if (iArmor < GetHorseArmor())
				iArmor = GetHorseArmor();

			const char* pHorseName = CHorseNameManager::instance().GetHorseName(GetPlayerID());

			if (pHorseName != NULL && strlen(pHorseName))
			{
				iArmor += 20;
			}
		}

		iArmor += GetPoint(POINT_DEF_GRADE_BONUS);
		iArmor += GetPoint(POINT_PARTY_DEFENDER_BONUS);

		// INTERNATIONAL_VERSION
		PointChange(POINT_DEF_GRADE, iDef + iArmor);
		PointChange(POINT_CLIENT_DEF_GRADE, (iShowDef + iArmor) - GetPoint(POINT_DEF_GRADE));
		// END_OF_INTERNATIONAL_VERSION

		PointChange(POINT_MAGIC_ATT_GRADE, GetLevel() * 2 + GetPoint(POINT_IQ) * 2 + GetPoint(POINT_MAGIC_ATT_GRADE_BONUS));
		PointChange(POINT_MAGIC_DEF_GRADE, GetLevel() + (GetPoint(POINT_IQ) * 3 + GetPoint(POINT_HT)) / 3 + iArmor / 2 + GetPoint(POINT_MAGIC_DEF_GRADE_BONUS));
	}
	else
	{
		// 2lev + str * 2
		int iAtt = GetLevel() * 2 + GetPoint(POINT_ST) * 2;
		// lev + con
		int iDef = GetLevel() + GetPoint(POINT_HT) + GetMobTable().wDef;

		SetPoint(POINT_ATT_GRADE, iAtt);
		SetPoint(POINT_DEF_GRADE, iDef);
		SetPoint(POINT_MAGIC_ATT_GRADE, GetPoint(POINT_ATT_GRADE));
		SetPoint(POINT_MAGIC_DEF_GRADE, GetPoint(POINT_DEF_GRADE));
	}
}

void CHARACTER::ComputePoints()
{
	long lStat = GetPoint(POINT_STAT);
	long lStatResetCount = GetPoint(POINT_STAT_RESET_COUNT);
	long lSkillActive = GetPoint(POINT_SKILL);
	long lSkillSub = GetPoint(POINT_SUB_SKILL);
	long lSkillHorse = GetPoint(POINT_HORSE_SKILL);
	long lLevelStep = GetPoint(POINT_LEVEL_STEP);

	long lAttackerBonus = GetPoint(POINT_PARTY_ATTACKER_BONUS);
	long lTankerBonus = GetPoint(POINT_PARTY_TANKER_BONUS);
	long lBufferBonus = GetPoint(POINT_PARTY_BUFFER_BONUS);
	long lSkillMasterBonus = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
	long lHasteBonus = GetPoint(POINT_PARTY_HASTE_BONUS);
	long lDefenderBonus = GetPoint(POINT_PARTY_DEFENDER_BONUS);

	long lHPRecovery = GetPoint(POINT_HP_RECOVERY);
	long lSPRecovery = GetPoint(POINT_SP_RECOVERY);

#ifdef ENABLE_COINS_INVENTORY
	long long lcoins = GetPoint(POINT_COINS);
#endif

#ifdef ENABLE_TITLE_SYSTEM
	std::vector<int> vTitles;
	if (IsPC())
	{
		for (int w = TITLE_0; w < TITLES_MAX_NUM; ++w)
		{
			BYTE bType = POINT_TITLE + w;
			vTitles.push_back(GetPoint(bType));
		}

		vTitles.push_back(0);
	}
#endif

#ifdef ENABLE_RENEWAL_BONUS_BOARD
	long long llMonsterKilled = GetMonsterKilled();
	long long llStoneKilled = GetStoneKilled();
	long long llBossKilled = GetBossKilled();
	long long llBluePlayerKilled = GetBluePlayerKilled();
	long long llYellowPlayerKilled = GetYellowPlayerKilled();
	long long llRedPlayerKilled = GetRedPlayerKilled();
	long long llAllPlayerKilled = GetAllPlayerKilled();
	long long llDuelWon = GetDuelWon();
	long long llDuelLost = GetDuelLose();
#endif
#ifdef ENABLE_COMPUTE_POINT_FIX
	m_pointsInstant.computed = false;
#endif
	memset(m_pointsInstant.points, 0, sizeof(m_pointsInstant.points));
	BuffOnAttr_ClearAll();
	m_SkillDamageBonus.clear();

	SetPoint(POINT_STAT, lStat);
	SetPoint(POINT_SKILL, lSkillActive);
	SetPoint(POINT_SUB_SKILL, lSkillSub);
	SetPoint(POINT_HORSE_SKILL, lSkillHorse);
	SetPoint(POINT_LEVEL_STEP, lLevelStep);
	SetPoint(POINT_STAT_RESET_COUNT, lStatResetCount);

	SetPoint(POINT_ST, GetRealPoint(POINT_ST));
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetPart(PART_MAIN, GetOriginalPart(PART_MAIN));
	SetPart(PART_WEAPON, GetOriginalPart(PART_WEAPON));
	SetPart(PART_HEAD, GetOriginalPart(PART_HEAD));
	SetPart(PART_HAIR, GetOriginalPart(PART_HAIR));
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	SetPart(PART_ACCE, GetOriginalPart(PART_ACCE));
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	SetPart(PART_AURA, GetOriginalPart(PART_AURA));
#endif

	SetPoint(POINT_PARTY_ATTACKER_BONUS, lAttackerBonus);
	SetPoint(POINT_PARTY_TANKER_BONUS, lTankerBonus);
	SetPoint(POINT_PARTY_BUFFER_BONUS, lBufferBonus);
	SetPoint(POINT_PARTY_SKILL_MASTER_BONUS, lSkillMasterBonus);
	SetPoint(POINT_PARTY_HASTE_BONUS, lHasteBonus);
	SetPoint(POINT_PARTY_DEFENDER_BONUS, lDefenderBonus);

	SetPoint(POINT_HP_RECOVERY, lHPRecovery);
	SetPoint(POINT_SP_RECOVERY, lSPRecovery);

#ifdef ENABLE_COINS_INVENTORY
	SetPoint(POINT_COINS, lcoins);
#endif

#ifdef ENABLE_TITLE_SYSTEM
	if (IsPC())
	{
		for (int w = TITLE_0; w < TITLES_MAX_NUM; ++w)
		{
			BYTE bType = POINT_TITLE + w;
			SetPoint(bType, vTitles[w]);
		}

		vTitles.clear();
	}
#endif

#ifdef ENABLE_RENEWAL_BONUS_BOARD
	SetPoint(POINT_MONSTER_KILLED, llMonsterKilled);
	SetPoint(POINT_STONE_KILLED, llStoneKilled);
	SetPoint(POINT_BOSS_KILLED, llBossKilled);
	SetPoint(POINT_BLUE_PLAYER_KILLED, llBluePlayerKilled);
	SetPoint(POINT_YELLOW_PLAYER_KILLED, llYellowPlayerKilled);
	SetPoint(POINT_RED_PLAYER_KILLED, llRedPlayerKilled);
	SetPoint(POINT_ALL_PLAYER_KILLED, llAllPlayerKilled);
	SetPoint(POINT_KILL_DUELWON, llDuelWon);
	SetPoint(POINT_KILL_DUELLOST, llDuelLost);
#endif

	int iMaxHP, iMaxSP;
	int iMaxStamina;

	if (IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| IsBotCharacter()
#endif
	)
	{
		iMaxHP = JobInitialPoints[GetJob()].max_hp + m_points.iRandomHP + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].hp_per_ht;
		iMaxSP = JobInitialPoints[GetJob()].max_sp + m_points.iRandomSP + GetPoint(POINT_IQ) * JobInitialPoints[GetJob()].sp_per_iq;
		iMaxStamina = JobInitialPoints[GetJob()].max_stamina + GetPoint(POINT_HT) * JobInitialPoints[GetJob()].stamina_per_con;

		{
			CSkillProto* pkSk = CSkillManager::instance().Get(SKILL_ADD_HP);

			if (NULL != pkSk)
			{
				pkSk->SetPointVar("k", 1.0f * GetSkillPower(SKILL_ADD_HP) / 100.0f);

#ifdef ENABLE_BOT_PLAYER
				// Bot oyuncular için skill bonuslarýný ekleme (sabit 30000 HP)
				if (!IsBotCharacter())
#endif
				{
					iMaxHP += static_cast<int>(pkSk->kPointPoly.Eval());
				}
			}
		}

#ifdef ENABLE_BOT_PLAYER
		// Bot oyuncularýn HP'si sabit 30000 (skill bonuslarýndan sonra tekrar ayarla)
		if (IsBotCharacter())
		{
			iMaxHP = 30000;
		}
#endif

#ifndef ENABLE_WITHOUT_WINDSHOES
		SetPoint(POINT_MOV_SPEED, 140);
#else
		SetPoint(POINT_MOV_SPEED, 100);
#endif
		SetPoint(POINT_ATT_SPEED, 100);
		PointChange(POINT_ATT_SPEED, GetPoint(POINT_PARTY_HASTE_BONUS));
		SetPoint(POINT_CASTING_SPEED, 100);
	}
	else
	{
		iMaxHP = m_pkMobData->m_table.dwMaxHP;
		iMaxSP = 0;
		iMaxStamina = 0;

		SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		SetPoint(POINT_MOV_SPEED, m_pkMobData->m_table.sMovingSpeed);
		SetPoint(POINT_CASTING_SPEED, m_pkMobData->m_table.sAttackSpeed);
	}

	if (IsPC())
	{
#ifdef ENABLE_MOUNT_SYSTEM
		if (GetMountVnum() && !GetWear(WEAR_MOUNT))
#else
		if (GetMountVnum())
#endif
		{
			if (GetHorseST() > GetPoint(POINT_ST))
				PointChange(POINT_ST, GetHorseST() - GetPoint(POINT_ST));

			if (GetHorseDX() > GetPoint(POINT_DX))
				PointChange(POINT_DX, GetHorseDX() - GetPoint(POINT_DX));

			if (GetHorseHT() > GetPoint(POINT_HT))
				PointChange(POINT_HT, GetHorseHT() - GetPoint(POINT_HT));

			if (GetHorseIQ() > GetPoint(POINT_IQ))
				PointChange(POINT_IQ, GetHorseIQ() - GetPoint(POINT_IQ));

/*	#Ýleriye dönük Yohara Sistemi eklersen bu kod bloðunu aktif et.
	#If you add the forward-looking Yohara System, activate this code block.If you add the forward-looking Yohara System, activate this code block.
#ifdef ENABLE_RIDING_EXTENDED
			if (GetHorseSungMaST() > GetPoint(POINT_SUNGMA_STR))
				PointChange(POINT_SUNGMA_STR, GetHorseSungMaST() - GetPoint(POINT_SUNGMA_STR));

			if (GetHorseSungMaDX() > GetPoint(POINT_SUNGMA_HP))
				PointChange(POINT_SUNGMA_HP, GetHorseSungMaDX() - GetPoint(POINT_SUNGMA_HP));

			if (GetHorseSungMaHT() > GetPoint(POINT_SUNGMA_MOVE))
				PointChange(POINT_SUNGMA_MOVE, GetHorseSungMaHT() - GetPoint(POINT_SUNGMA_MOVE));

			if (GetHorseSungMaIQ() > GetPoint(POINT_SUNGMA_IMMUNE))
				PointChange(POINT_SUNGMA_IMMUNE, GetHorseSungMaIQ() - GetPoint(POINT_SUNGMA_IMMUNE));
#endif
*/
		}

	}

	ComputeBattlePoints();

#ifdef ENABLE_BOT_PLAYER
	// Bot oyuncularýn HP'si sabit 30000 (tüm bonuslardan sonra kesin olarak ayarla)
	if (IsBotCharacter())
	{
		iMaxHP = 30000;
	}
#endif

	if (iMaxHP != GetMaxHP())
	{
		SetRealPoint(POINT_MAX_HP, iMaxHP);
	}

	PointChange(POINT_MAX_HP, 0);

#ifdef ENABLE_BOT_PLAYER
	// Bot oyuncular için MaxHP'nin asla 0 olmamasýný garanti et
	if (IsBotCharacter() && GetMaxHP() <= 0)
	{
		SetRealPoint(POINT_MAX_HP, 30000);
		PointChange(POINT_MAX_HP, 0);
		SetHP(30000);
	}
#endif

	if (iMaxSP != GetMaxSP())
	{
		SetRealPoint(POINT_MAX_SP, iMaxSP);
	}

	PointChange(POINT_MAX_SP, 0);

	SetMaxStamina(iMaxStamina);
	// @fixme118 part1
	int iCurHP = this->GetHP();
	int iCurSP = this->GetSP();

	m_pointsInstant.dwImmuneFlag = 0;

	for (int i = 0 ; i < WEAR_MAX_NUM; i++)
	{
		LPITEM pItem = GetWear(i);
		if (pItem)
		{
			pItem->ModifyPoints(true);
			SET_BIT(m_pointsInstant.dwImmuneFlag, GetWear(i)->GetImmuneFlag());
		}
	}

	// Dragon Soul Stone System
	// In ComputePoints, initialize all property values of the character,
	// Because all property values related to items, buffs, etc. are recalculated,
	// The Dragon Soul Stone system must also re-apply the attribute values of all Dragon Soul Stones in ActiveDeck.
	if (DragonSoul_IsDeckActivated())
	{
		for (int i = WEAR_MAX_NUM + DS_SLOT_MAX * DragonSoul_GetActiveDeck();
			i < WEAR_MAX_NUM + DS_SLOT_MAX * (DragonSoul_GetActiveDeck() + 1); i++)
		{
			LPITEM pItem = GetWear(i);
			if (pItem)
			{
				if (DSManager::instance().IsTimeLeftDragonSoul(pItem))
					pItem->ModifyPoints(true);
			}
		}
	}

	if (GetHP() > GetMaxHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetSP() > GetMaxSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());

	ComputeSkillPoints();

	RefreshAffect();
	// @fixme118 part2 (after petsystem stuff)
	if (IsPC())
	{
		if (this->GetHP() != iCurHP)
			this->PointChange(POINT_HP, iCurHP-this->GetHP());
		if (this->GetSP() != iCurSP)
			this->PointChange(POINT_SP, iCurSP-this->GetSP());
	}

	CPetSystem * pPetSystem = GetPetSystem();
	if (NULL != pPetSystem)
	{
		pPetSystem->RefreshBuff();
	}

#ifdef ENABLE_COMPUTE_POINT_FIX
	m_pointsInstant.computed = true;
#endif

	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); it++)
	{
		it->second->GiveAllAttributes();
	}

#ifdef ENABLE_BOT_PLAYER
	if (IsBotCharacter())
	{
		// Bot oyuncular için HP'yi MaxHP'ye eþitle (30000)
		if (GetHP() != GetMaxHP())
		{
			SetHP(GetMaxHP());
		}
		
		// Bot oyuncular için %100 savunma deðerleri
		SetPoint(POINT_DEF_GRADE, 100);       // Çok yüksek temel savunma
		SetPoint(POINT_RESIST_NORMAL_DAMAGE, 50);    // %100 normal hasar azaltma
		SetPoint(POINT_NORMAL_HIT_DEFEND_BONUS, 10); // %99 normal vuruþ azaltma (maksimum)
		SetPoint(POINT_SKILL_DEFEND_BONUS, 10);      // %99 skill hasarý azaltma (maksimum)
		
		// Karakter sýnýflarýna karþý %50 savunma
		SetPoint(POINT_RESIST_WARRIOR, 50);    // Savaþçýya karþý %100 savunma
		SetPoint(POINT_RESIST_ASSASSIN, 50);   // Ninja'ya karþý %100 savunma
		SetPoint(POINT_RESIST_SURA, 50);       // Sura'ya karþý %100 savunma
		SetPoint(POINT_RESIST_SHAMAN, 50);     // Þaman'a karþý %100 savunma
#ifdef ENABLE_WOLFMAN_CHARACTER
		SetPoint(POINT_RESIST_WOLFMAN, 50);     // Kurt Adam'a karþý %100 savunma
#endif
	}
	else
#endif

	UpdatePacket();
}

// The unit of m_dwPlayStartTime is milisecond. Record in minutes in database
// Because of this, when calculating the play time / divided by 60000, the remainder is left over.
// When , you have to put it as dwTimeRemain here so that it can be calculated properly.
void CHARACTER::ResetPlayTime(DWORD dwTimeRemain)
{
	m_dwPlayStartTime = get_dword_time() - dwTimeRemain;
}

const int aiRecoveryPercents[10] = { 1, 5, 5, 5, 5, 5, 5, 5, 5, 5 };

EVENTFUNC(recovery_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "recovery_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL) { // <Factor>
		return 0;
	}

	if (!ch->IsPC()
#ifdef ENABLE_BOT_PLAYER
		&& !ch->IsBotCharacter()
#endif
	)
	{
		if (ch->IsAffectFlag(AFF_POISON))
			return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
		if (2493 == ch->GetMobTable().dwVnum)	//Aqua Dragon
		{
			int regenPct = BlueDragon_GetRangeFactor("hp_regen", ch->GetHPPct());
			regenPct += ch->GetMobTable().bRegenPercent;

			for (int i=1 ; i <= 4 ; ++i)
			{
				if (REGEN_PECT_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap( ch->GetMapIndex(), dwDragonStoneID );

					regenPct += (val*cnt);

					break;
				}
			}

			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * regenPct) / 100));
		}
		else if (!ch->IsDoor())
		{
			ch->MonsterLog("HP_REGEN +%d", MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
			ch->PointChange(POINT_HP, MAX(1, (ch->GetMaxHP() * ch->GetMobTable().bRegenPercent) / 100));
		}

		if (ch->GetHP() >= ch->GetMaxHP())
		{
			ch->m_pkRecoveryEvent = NULL;
			return 0;
		}

		if (2493 == ch->GetMobTable().dwVnum)
		{
			for (int i=1 ; i <= 4 ; ++i)
			{
				if (REGEN_TIME_BONUS == BlueDragon_GetIndexFactor("DragonStone", i, "effect_type"))
				{
					DWORD dwDragonStoneID = BlueDragon_GetIndexFactor("DragonStone", i, "vnum");
					size_t val = BlueDragon_GetIndexFactor("DragonStone", i, "val");
					size_t cnt = SECTREE_MANAGER::instance().GetMonsterCountInMap( ch->GetMapIndex(), dwDragonStoneID );

					return PASSES_PER_SEC(MAX(1, (ch->GetMobTable().bRegenCycle - (val*cnt))));
				}
			}
		}

		return PASSES_PER_SEC(MAX(1, ch->GetMobTable().bRegenCycle));
	}
	else
	{
		ch->CheckTarget();
		ch->UpdateKillerMode();

		if (ch->IsAffectFlag(AFF_POISON) == true)
		{
			return 3;
		}

		int iSec = (get_dword_time() - ch->GetLastMoveTime()) / 3000;

		ch->DistributeSP(ch);

		if (ch->GetMaxHP() <= ch->GetHP())
			return PASSES_PER_SEC(3);

		int iPercent = 0;
		int iAmount = 0;

		{
			iPercent = aiRecoveryPercents[MIN(9, iSec)];
			iAmount = 15 + (ch->GetMaxHP() * iPercent) / 100;
		}

		iAmount += (iAmount * ch->GetPoint(POINT_HP_REGEN)) / 100;

		sys_log(1, "RECOVERY_EVENT: %s %d HP_REGEN %d HP +%d", ch->GetName(), iPercent, ch->GetPoint(POINT_HP_REGEN), iAmount);

		ch->PointChange(POINT_HP, iAmount, false);
		return PASSES_PER_SEC(3);
	}
}

void CHARACTER::StartRecoveryEvent()
{
	if (m_pkRecoveryEvent)
		return;

	if (IsDead() || IsStun())
		return;

	if (IsNPC() && GetHP() >= GetMaxHP())
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	int iSec = 0;
	if (IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| IsBotCharacter()
#endif
	)
		iSec = 3; 
	else
		iSec = (MAX(1, GetMobTable().bRegenCycle));

	m_pkRecoveryEvent = event_create(recovery_event, info, PASSES_PER_SEC(iSec));
}

void CHARACTER::Standup()
{
	struct packet_position pack_position;

	if (!IsPosition(POS_SITTING))
		return;

	SetPosition(POS_STANDING);

	sys_log(1, "STANDUP: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_GENERAL;

	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::Sitdown(int is_ground)
{
	struct packet_position pack_position;

	if (IsPosition(POS_SITTING))
		return;

	SetPosition(POS_SITTING);
	sys_log(1, "SITDOWN: %s", GetName());

	pack_position.header	= HEADER_GC_CHARACTER_POSITION;
	pack_position.vid		= GetVID();
	pack_position.position	= POSITION_SITTING_GROUND;
	PacketAround(&pack_position, sizeof(pack_position));
}

void CHARACTER::SetRotation(float fRot)
{
	m_pointsInstant.fRot = fRot;
}

void CHARACTER::SetRotationToXY(long x, long y)
{
	SetRotation(GetDegreeFromPositionXY(GetX(), GetY(), x, y));
}

bool CHARACTER::CannotMoveByAffect() const
{
	return (IsAffectFlag(AFF_STUN));
}

bool CHARACTER::CanMove() const
{
	if (CannotMoveByAffect())
		return false;

	if (GetMyShop())
		return false;

	return true;
}

// DevFix 29 - Necessary arrangement for scenarios where sync is not possible. - [MT2Dev Note] - 01/04/2024
// Moves to the specified x, y position regardless. - [Ymir Dev Note]
bool CHARACTER::Sync(long x, long y)
{
	LPSECTREE current_tree = GetSectree();  // For a better performance, call it only once.. - [MT2Dev Note]
	if (!current_tree)
	{
		sys_err("<CHARACTER::Sync> Sectree is NULL!");
		return false;
	}

	if (IsPC() && IsDead())   // DevFix 27 - Dead players not needed sync.. - [MT2Dev Note] - 01/04/2024
	{
		return false;
	}

	LPSECTREE new_tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), x, y);
	if (!new_tree)
	{
		if (GetDesc())
		{
			sys_err("No Tree: (x: %ld)  (y: %ld)  (Name: %s)  (Map: %d)", x, y, GetName(), GetMapIndex());  // Added map index. - [MT2Dev Note] - 23/10/2024
			x = GetX();
			y = GetY();
			new_tree = current_tree;  // If there is no new tree, just use the old one. - [MT2Dev Note]
			if (!new_tree)
			{
				sys_err("[CRITICAL!!] - No Tree: (x: %ld)  (y: %ld)  (Name: %s)  (Map: %d)", x, y, GetName(), GetMapIndex());  // Added map index. - [MT2Dev Note] - 23/10/2024
			GetDesc()->SetPhase(PHASE_CLOSE);
				return false;  // Set the new_tree, call that a "fix" and forget one f**in line that cause return it false, hi there it's MT2Dev -_-
			}
		}
		else
		{
			if (IsMonster() || IsStone())  // Dead is only possible when victim is mob or stone. - [MT2Dev Note] - 10/10/2024
			{
				sys_err("[MOB or STONE]No Tree: (x: %ld)  (y: %ld)  (Map: %d)", x, y, GetMapIndex());
				RewardlessDead(); // In this case, we don't want any reward so this is new function for it. - [MT2Dev Note] - 10/10/2024
				return false;
			}
			else
			{
				sys_err("[POTENTIAL HACK!]No Tree: (x: %ld)  (y: %ld)  (Map: %d)", x, y, GetMapIndex());
		return false;
			}
		}
	}

	SetRotationToXY(x, y);
	SetXYZ(x, y, 0);

	// Under that, you will see some kind a unnecesary nullptr controls, this ones for ensure everything is correct.
	// This ones will not chance anything about performance in new compilers, so that ones don't hurt anyone i guess. - [MT2Dev Note] - 23/10/2024
	if (GetDungeon())
	{
		if (new_tree)
		{
			// Dungeon event attribute change. - [Ymir Dev Note]
		int iLastEventAttr = m_iEventAttr;
		m_iEventAttr = new_tree->GetEventAttribute(x, y);
		if (m_iEventAttr != iLastEventAttr)
		{
			if (GetParty())
			{
				quest::CQuestManager::instance().AttrOut(GetParty()->GetLeaderPID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetParty()->GetLeaderPID(), this, m_iEventAttr);
			}
			else
			{
				quest::CQuestManager::instance().AttrOut(GetPlayerID(), this, iLastEventAttr);
				quest::CQuestManager::instance().AttrIn(GetPlayerID(), this, m_iEventAttr);
			}
		}
		}
		else
		{
			sys_err("[POTENTIAL HACK!] Dungeon - There is no new tree!");
			return false;
		}
	}

	if (current_tree != new_tree)
	{
		if (!IsNPC())
		{
			if (current_tree && new_tree)
		{
			SECTREEID id = new_tree->GetID();
				SECTREEID old_id = current_tree->GetID();
				const float fDist = DISTANCE_SQRT(id.coord.x - old_id.coord.x, id.coord.y - old_id.coord.y);
				sys_log(0, "SECTREE DIFFER: %s %dx%d was %dx%d dist %.1fm", GetName(), id.coord.x, id.coord.y, old_id.coord.x, old_id.coord.y, fDist);  // DevFix 30
		}
			else
			{
				sys_err("[POTENTIAL HACK!] There is no current tree or new tree!");
				return false;
			}
		}

		if (new_tree)
		{
		new_tree->InsertEntity(this);
		}
		else
		{
			sys_err("[POTENTIAL HACK!] InsertEntity NOT possible because there is no new tree!");
			return false;
		}
	}

	return true;
}

#ifdef ENABLE_STOP_FUNCTION_FIX
void CHARACTER::Stop()
{
	if (IsPC())
	{
		// PC karakterler için sadece pozisyon resetleme yapýlýr, state deðiþikliði yapýlmaz
		m_posDest.x = m_posStart.x = GetX();
		m_posDest.y = m_posStart.y = GetY();
		return;
	}

	if (!IsStone() && GetVictim() && !IsCoward()) {
		if (!IsState(m_stateBattle))
			MonsterLog("[BATTLE] %s started attacking nearby", GetVictim()->GetName());

		GotoState(m_stateBattle);
	}
	else {
		if (!IsState(m_stateIdle))
			MonsterLog("[IDLE] No target goi");

		GotoState(m_stateIdle);
	}

	m_dwStateDuration = 1;

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
}
#else
void CHARACTER::Stop()
{
	if (!IsState(m_stateIdle))
		MonsterLog("[IDLE] Á¤Áö");

	GotoState(m_stateIdle);

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();
}
#endif

bool CHARACTER::Goto(long x, long y)
{
	if (GetX() == x && GetY() == y)
		return false;

	if (m_posDest.x == x && m_posDest.y == y)
	{
		if (!IsState(m_stateMove))
		{
			m_dwStateDuration = 4;
			GotoState(m_stateMove);
		}
		return false;
	}

	m_posDest.x = x;
	m_posDest.y = y;

	CalculateMoveDuration();

	m_dwStateDuration = 4;

	if (!IsState(m_stateMove))
	{
		MonsterLog("[MOVE] %s", GetVictim() ? "´ë»óÃßÀû" : "±×³ÉÀÌµ¿");

		if (GetVictim())
		{
			MonsterChat(MONSTER_CHAT_ATTACK);
		}
	}

	GotoState(m_stateMove);

	return true;
}


DWORD CHARACTER::GetMotionMode() const
{
	DWORD dwMode = MOTION_MODE_GENERAL;

	if (IsPolymorphed())
		return dwMode;

	LPITEM pkItem;

	if ((pkItem = GetWear(WEAR_WEAPON)))
	{
		switch (pkItem->GetProto()->bSubType)
		{
			case WEAPON_SWORD:
				dwMode = MOTION_MODE_ONEHAND_SWORD;
				break;

			case WEAPON_TWO_HANDED:
				dwMode = MOTION_MODE_TWOHAND_SWORD;
				break;

			case WEAPON_DAGGER:
				dwMode = MOTION_MODE_DUALHAND_SWORD;
				break;

			case WEAPON_BOW:
				dwMode = MOTION_MODE_BOW;
				break;

			case WEAPON_BELL:
				dwMode = MOTION_MODE_BELL;
				break;

			case WEAPON_FAN:
				dwMode = MOTION_MODE_FAN;
				break;
		}
	}
	return dwMode;
}

float CHARACTER::GetMoveMotionSpeed() const
{
	DWORD dwMode = GetMotionMode();

	const CMotion * pkMotion = NULL;

	if (!GetMountVnum())
		pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(dwMode, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	else
	{
		pkMotion = CMotionManager::instance().GetMotion(GetMountVnum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));

		if (!pkMotion)
			pkMotion = CMotionManager::instance().GetMotion(GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_HORSE, (IsWalking() && IsPC()) ? MOTION_WALK : MOTION_RUN));
	}

	if (pkMotion)
		return -pkMotion->GetAccumVector().y / pkMotion->GetDuration();
	else
	{
		sys_err("cannot find motion (name %s race %d mode %d)", GetName(), GetRaceNum(), dwMode);
		return 300.0f;
	}
}

float CHARACTER::GetMoveSpeed() const
{
	return GetMoveMotionSpeed() * 10000 / CalculateDuration(GetLimitPoint(POINT_MOV_SPEED), 10000);
}

void CHARACTER::CalculateMoveDuration()
{
	m_posStart.x = GetX();
	m_posStart.y = GetY();

	float fDist = DISTANCE_SQRT(m_posStart.x - m_posDest.x, m_posStart.y - m_posDest.y);

	float motionSpeed = GetMoveMotionSpeed();

	m_dwMoveDuration = CalculateDuration(GetLimitPoint(POINT_MOV_SPEED),
			(int) ((fDist / motionSpeed) * 1000.0f));

	if (IsNPC())
		sys_log(1, "%s: GOTO: distance %f, spd %u, duration %u, motion speed %f pos %d %d -> %d %d",
				GetName(), fDist, GetLimitPoint(POINT_MOV_SPEED), m_dwMoveDuration, motionSpeed,
				m_posStart.x, m_posStart.y, m_posDest.x, m_posDest.y);

	m_dwMoveStartTime = get_dword_time();
}

bool CHARACTER::Move(long x, long y)
{
	if (GetX() == x && GetY() == y)
		return true;

	if (test_server)
		if (m_bDetailLog)
			sys_log(0, "%s position %u %u", GetName(), x, y);

	OnMove();
	return Sync(x, y);
}

void CHARACTER::SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime, int iRot)
{
	TPacketGCMove pack;

	if (bFunc == FUNC_WAIT)
	{
		x = m_posDest.x;
		y = m_posDest.y;
		dwDuration = m_dwMoveDuration;
	}

	EncodeMovePacket(pack, GetVID(), bFunc, bArg, x, y, dwDuration, dwTime, iRot == -1 ? (int) GetRotation() / 5 : iRot);
	PacketView(&pack, sizeof(TPacketGCMove), this);
}

int CHARACTER::GetRealPoint(BYTE type) const
{
	return m_points.points[type];
}

void CHARACTER::SetRealPoint(BYTE type, int val)
{
	m_points.points[type] = val;
}

int CHARACTER::GetPolymorphPoint(BYTE type) const
{
	if (IsPolymorphed() && !IsPolyMaintainStat())
	{
		DWORD dwMobVnum = GetPolymorphVnum();
		const CMob * pMob = CMobManager::instance().Get(dwMobVnum);
		int iPower = GetPolymorphPower();

		if (pMob)
		{
			switch (type)
			{
				case POINT_ST:
					if (GetJob() == JOB_SHAMAN || GetJob() == JOB_SURA && GetSkillGroup() == 2)
						return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_IQ);
					return pMob->m_table.bStr * iPower / 100 + GetPoint(POINT_ST);

				case POINT_HT:
					return pMob->m_table.bCon * iPower / 100 + GetPoint(POINT_HT);

				case POINT_IQ:
					return pMob->m_table.bInt * iPower / 100 + GetPoint(POINT_IQ);

				case POINT_DX:
					return pMob->m_table.bDex * iPower / 100 + GetPoint(POINT_DX);
			}
		}
	}

	return GetPoint(type);
}

#ifdef ENABLE_GOLD_LIMIT
long long CHARACTER::GetPoint(BYTE type) const
#else
int CHARACTER::GetPoint(BYTE type) const
#endif
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

#ifdef ENABLE_GOLD_LIMIT
	long long val = m_pointsInstant.points[type];
	long long max_val = LLONG_MAX;
#else
	int val = m_pointsInstant.points[type];
	int max_val = INT_MAX;
#endif

	switch (type)
	{
		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
			max_val = 50;
			break;

#ifdef ENABLE_GOLD_LIMIT
		case POINT_GOLD:
			max_val = GOLD_MAX;
			break;
#endif
	}

	if (val > max_val)
#ifdef ENABLE_HP_SP_ABSORB_ORB_FIX
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), type, val, max_val);
#else
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), val, max_val);
#endif

	return (val);
}

int CHARACTER::GetLimitPoint(BYTE type) const
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return 0;
	}

	int val = m_pointsInstant.points[type];
	int max_val = INT_MAX;
	int limit = INT_MAX;
	int min_limit = -INT_MAX;

	switch (type)
	{
		case POINT_ATT_SPEED:
			min_limit = 0;

			if (IsPC()
#ifdef ENABLE_BOT_PLAYER
				|| IsBotCharacter()
#endif
				)
				limit = 170;
			else
				limit = 250;
			break;

		case POINT_MOV_SPEED:
			min_limit = 0;

			if (IsPC()
#ifdef ENABLE_BOT_PLAYER
				|| IsBotCharacter()
#endif
				)
				limit = 200;
			else
				limit = 250;
			break;

		case POINT_STEAL_HP:
		case POINT_STEAL_SP:
			limit = 50;
			max_val = 50;
			break;

		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
			limit = 20;
			max_val = 50;
			break;
	}

	if (val > max_val)
#ifdef ENABLE_HP_SP_ABSORB_ORB_FIX
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), type, val, max_val);
#else
		sys_err("POINT_ERROR: %s type %d val %d (max: %d)", GetName(), val, max_val);
#endif
	if (val > limit)
		val = limit;

	if (val < min_limit)
		val = min_limit;

	return (val);
}

#ifdef ENABLE_GOLD_LIMIT
void CHARACTER::SetPoint(BYTE type, long long val)
#else
void CHARACTER::SetPoint(BYTE type, int val)
#endif
{
	if (type >= POINT_MAX_NUM)
	{
		sys_err("Point type overflow (type %u)", type);
		return;
	}

	m_pointsInstant.points[type] = val;

	if (type == POINT_MOV_SPEED && get_dword_time() < m_dwMoveStartTime + m_dwMoveDuration)
	{
		CalculateMoveDuration();
	}
}

#ifdef ENABLE_GOLD_LIMIT
long long CHARACTER::GetAllowedGold() const
#else
INT CHARACTER::GetAllowedGold() const
#endif
{
	if (GetLevel() <= 10)
		return 100000;
	else if (GetLevel() <= 20)
		return 500000;
	else
		return 50000000;
}

void CHARACTER::CheckMaximumPoints()
{
	if (GetMaxHP() < GetHP())
		PointChange(POINT_HP, GetMaxHP() - GetHP());

	if (GetMaxSP() < GetSP())
		PointChange(POINT_SP, GetMaxSP() - GetSP());
}

#ifdef ENABLE_GOLD_LIMIT
void CHARACTER::PointChange(BYTE type, long long amount, bool bAmount, bool bBroadcast)
#else
void CHARACTER::PointChange(BYTE type, int amount, bool bAmount, bool bBroadcast)
#endif
{
#ifdef ENABLE_GOLD_LIMIT
	long long val = 0;
#else
	int val = 0;
#endif

	switch (type)
	{
		case POINT_NONE:
			return;

		case POINT_LEVEL:
			if ((GetLevel() + amount) > gPlayerMaxLevel)
				return;

			SetLevel(GetLevel() + amount);
			val = GetLevel();

			sys_log(0, "LEVELUP: %s %d NEXT EXP %d", GetName(), GetLevel(), GetNextExp());

			PointChange(POINT_NEXT_EXP,	GetNextExp(), false);

#ifdef ENABLE_ANNONUNCEMENT_LEVELUP
		switch (val)
		{
			case 120:
				char szNoticeLevelUp[QUERY_MAX_LEN];
				snprintf(szNoticeLevelUp, sizeof(szNoticeLevelUp), "<Bilgi> [CH%d]: [%s] isimli oyuncumuz [%u] seviye olmuþtur. Tebrikler!", g_bChannel, GetName(), GetLevel());
				BroadcastNotice(szNoticeLevelUp);
				break;

			default:
				break;
		}
#endif

#ifdef ENABLE_RENEWAL_SKILL_SELECT
			RenewalSkillSelect();
#endif

			if (amount)
			{
				quest::CQuestManager::instance().LevelUp(GetPlayerID());

#ifdef ENABLE_HUNTING_SYSTEM
				CheckHunting();
#endif

				LogManager::instance().LevelLog(this, val, GetRealPoint(POINT_PLAYTIME) + (get_dword_time() - m_dwPlayStartTime) / 60000);

				if (GetGuild())
				{
					GetGuild()->LevelChange(GetPlayerID(), GetLevel());
				}

				if (GetParty())
				{
					GetParty()->RequestSetMemberLevel(GetPlayerID(), GetLevel());
				}
			}
			break;

		case POINT_NEXT_EXP:
			val = GetNextExp();
			bAmount = false;
			break;

		case POINT_EXP:
			{
				DWORD exp = GetExp();
				DWORD next_exp = GetNextExp();

#ifdef ENABLE_POINT_EXP_FIX
				if ((amount < 0) && (exp <= std::abs(amount)))
#else
				if (amount < 0 && exp < -amount)
#endif
				{
					sys_log(1, "%s AMOUNT < 0 %d, CUR EXP: %d", GetName(), -amount, exp);
					amount = -exp;

					SetExp(exp + amount);
					val = GetExp();
				}
				else
				{
					if (gPlayerMaxLevel <= GetLevel())
						return;

					if (test_server)
						LocaleChatPacket(CHAT_TYPE_INFO, 22, "%d", amount);

					DWORD iExpBalance = 0;

					if (exp + amount >= next_exp)
					{
						iExpBalance = (exp + amount) - next_exp;
						amount = next_exp - exp;

						SetExp(0);
						exp = next_exp;
					}
					else
					{
						SetExp(exp + amount);
						exp = GetExp();
					}

					DWORD q = DWORD(next_exp / 4.0f);
					int iLevStep = GetRealPoint(POINT_LEVEL_STEP);

					if (iLevStep >= 4)
					{
						sys_err("%s LEVEL_STEP bigger than 4! (%d)", GetName(), iLevStep);
						iLevStep = 4;
					}

					if (exp >= next_exp && iLevStep < 4)
					{
						for (int i = 0; i < 4 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q * 3 && iLevStep < 3)
					{
						for (int i = 0; i < 3 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q * 2 && iLevStep < 2)
					{
						for (int i = 0; i < 2 - iLevStep; ++i)
							PointChange(POINT_LEVEL_STEP, 1, false, true);
					}
					else if (exp >= q && iLevStep < 1)
						PointChange(POINT_LEVEL_STEP, 1);

					if (iExpBalance)
					{
						PointChange(POINT_EXP, iExpBalance);
					}

					val = GetExp();
				}
			}
			break;

		case POINT_LEVEL_STEP:
			if (amount > 0)
			{
				val = GetPoint(POINT_LEVEL_STEP) + amount;

				switch (val)
				{
					case 1:
					case 2:
					case 3:
						if (GetLevel() < 111) PointChange(POINT_STAT, 1);
						break;

					case 4:
						{
							int iHP = number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end);
							int iSP = number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end);

							m_points.iRandomHP += iHP;
							m_points.iRandomSP += iSP;

							if (GetSkillGroup())
							{
								if (GetLevel() >= 5)
									PointChange(POINT_SKILL, 1);

								if (GetLevel() >= 9)
									PointChange(POINT_SUB_SKILL, 1);
							}

							PointChange(POINT_MAX_HP, iHP);
							PointChange(POINT_MAX_SP, iSP);
							PointChange(POINT_LEVEL, 1, false, true);

							val = 0;
						}
						break;
				}

#ifdef ENABLE_POTIONS_LEVELUP
				if (GetLevel() <= 10)
					AutoGiveItem(27001, 2);
				else if (GetLevel() <= 30)
					AutoGiveItem(27002, 2);
				else
				{
					AutoGiveItem(27002, 2);
				}
#endif

				PointChange(POINT_HP, GetMaxHP() - GetHP());
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				PointChange(POINT_STAMINA, GetMaxStamina() - GetStamina());

				SetPoint(POINT_LEVEL_STEP, val);
				SetRealPoint(POINT_LEVEL_STEP, val);

				Save();
			}
			else
				val = GetPoint(POINT_LEVEL_STEP);

			break;

		case POINT_HP:
			{
				if (IsDead() || IsStun())
					return;

				int prev_hp = GetHP();

				amount = MIN(GetMaxHP() - GetHP(), amount);
				SetHP(GetHP() + amount);
				val = GetHP();

				BroadcastTargetPacket();

				if (GetParty() && IsPC() && val != prev_hp)
					GetParty()->SendPartyInfoOneToAll(this);
			}
			break;

		case POINT_SP:
			{
				if (IsDead() || IsStun())
					return;

				amount = MIN(GetMaxSP() - GetSP(), amount);
				SetSP(GetSP() + amount);
				val = GetSP();
			}
			break;

		case POINT_STAMINA:
			{
				if (IsDead() || IsStun())
					return;

				int prev_val = GetStamina();
				amount = MIN(GetMaxStamina() - GetStamina(), amount);
				SetStamina(GetStamina() + amount);
				val = GetStamina();

				if (val == 0)
				{
					SetNowWalking(true);
				}
				else if (prev_val == 0)
				{
					ResetWalking();
				}

				if (amount < 0 && val != 0)
					return;
			}
			break;

		case POINT_MAX_HP:
			{
				SetPoint(type, GetPoint(type) + amount);

				int hp = GetRealPoint(POINT_MAX_HP);
				int add_hp = MIN(3500, hp * GetPoint(POINT_MAX_HP_PCT) / 100);
				add_hp += GetPoint(POINT_MAX_HP);
				add_hp += GetPoint(POINT_PARTY_TANKER_BONUS);

				SetMaxHP(hp + add_hp);

				val = GetMaxHP();
			}
			break;

		case POINT_MAX_SP:
			{
				SetPoint(type, GetPoint(type) + amount);

				int sp = GetRealPoint(POINT_MAX_SP);
				int add_sp = MIN(800, sp * GetPoint(POINT_MAX_SP_PCT) / 100);
				add_sp += GetPoint(POINT_MAX_SP);
				add_sp += GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);

				SetMaxSP(sp + add_sp);

				val = GetMaxSP();
			}
			break;

		case POINT_MAX_HP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_MAX_HP, 0);
			break;

		case POINT_MAX_SP_PCT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_MAX_SP, 0);
			break;

		case POINT_MAX_STAMINA:
			SetMaxStamina(GetMaxStamina() + amount);
			val = GetMaxStamina();
			break;

		case POINT_GOLD:
			{
#ifdef ENABLE_GOLD_LIMIT
				const long long nTotalMoney = static_cast<long long>(GetGold()) + static_cast<long long>(amount);

				if (GOLD_MAX <= nTotalMoney)
				{
					sys_err("[OVERFLOW_GOLD] OriGold %lld AddedGold %lld id %u Name %s ", GetGold(), amount, GetPlayerID(), GetName());
					LogManager::instance().CharLog(this, GetGold() + amount, "OVERFLOW_GOLD", "");
					return;
				}
#else
				const int64_t nTotalMoney = static_cast<int64_t>(GetGold()) + static_cast<int64_t>(amount);

				if (GOLD_MAX <= nTotalMoney)
				{
					sys_err("[OVERFLOW_GOLD] OriGold %d AddedGold %d id %u Name %s ", GetGold(), amount, GetPlayerID(), GetName());
					LogManager::instance().CharLog(this, GetGold() + amount, "OVERFLOW_GOLD", "");
					return;
				}
#endif
				SetGold(GetGold() + amount);
				val = GetGold();
			}
			break;

#ifdef ENABLE_COINS_INVENTORY
		case POINT_COINS:
			{
				SetCoins(GetCoins() + amount);
				val = GetCoins();
			}
			break;
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
		case POINT_BATTLE_PASS_PREMIUM_ID:
			{
				SetExtBattlePassPremiumID(amount);
				val = GetExtBattlePassPremiumID();
			}
			break;
#endif

#ifdef ENABLE_RENEWAL_BONUS_BOARD
		case POINT_MONSTER_KILLED:
		{
			const long long llTotalValue = static_cast<long long>(GetMonsterKilled()) + static_cast<long long>(amount);

			if (MAX_REGIST_NUMBER <= llTotalValue)
				return;

			SetMonsterKilled(GetMonsterKilled() + amount);
			val = GetMonsterKilled();
		}
		break;

		case POINT_STONE_KILLED:
		{
			const long long llTotalValue = static_cast<long long>(GetStoneKilled()) + static_cast<long long>(amount);

			if (MAX_REGIST_NUMBER <= llTotalValue)
				return;

			SetStoneKilled(GetStoneKilled() + amount);
			val = GetStoneKilled();
		}
		break;

		case POINT_BOSS_KILLED:
		{
			const long long llTotalValue = static_cast<long long>(GetBossKilled()) + static_cast<long long>(amount);

			if (MAX_REGIST_NUMBER <= llTotalValue)
				return;

			SetBossKilled(GetBossKilled() + amount);
			val = GetBossKilled();
		}
		break;

		case POINT_BLUE_PLAYER_KILLED:
		{
			const long long llTotalValue = static_cast<long long>(GetBluePlayerKilled()) + static_cast<long long>(amount);

			if (MAX_REGIST_NUMBER <= llTotalValue)
				return;

			SetBluePlayerKilled(GetBluePlayerKilled() + amount);
			val = GetBluePlayerKilled();
		}
		break;

		case POINT_YELLOW_PLAYER_KILLED:
		{
			const long long llTotalValue = static_cast<long long>(GetYellowPlayerKilled()) + static_cast<long long>(amount);

			if (MAX_REGIST_NUMBER <= llTotalValue)
				return;

			SetYellowPlayerKilled(GetYellowPlayerKilled() + amount);
			val = GetYellowPlayerKilled();
		}
		break;

		case POINT_RED_PLAYER_KILLED:
		{
			const long long llTotalValue = static_cast<long long>(GetRedPlayerKilled()) + static_cast<long long>(amount);

			if (MAX_REGIST_NUMBER <= llTotalValue)
				return;

			SetRedPlayerKilled(GetRedPlayerKilled() + amount);
			val = GetRedPlayerKilled();
		}
		break;

		case POINT_ALL_PLAYER_KILLED:
		{
			const long long llTotalValue = static_cast<long long>(GetAllPlayerKilled()) + static_cast<long long>(amount);

			if (MAX_REGIST_NUMBER <= llTotalValue)
				return;

			SetAllPlayerKilled(GetAllPlayerKilled() + amount);
			val = GetAllPlayerKilled();
		}
		break;

		case POINT_KILL_DUELWON:
		{
			const long long llTotalValue = static_cast<long long>(GetDuelWon()) + static_cast<long long>(amount);

			if (MAX_REGIST_NUMBER <= llTotalValue)
				return;

			SetDuelWon(GetDuelWon() + amount);
			val = GetDuelWon();
		}
		break;

		case POINT_KILL_DUELLOST:
		{
			const long long llTotalValue = static_cast<long long>(GetDuelLose()) + static_cast<long long>(amount);

			if (MAX_REGIST_NUMBER <= llTotalValue)
				return;

			SetDuelLose(GetDuelLose() + amount);
			val = GetDuelLose();
		}
		break;
#endif

		case POINT_SKILL:
		case POINT_STAT:
		case POINT_SUB_SKILL:
		case POINT_STAT_RESET_COUNT:
		case POINT_HORSE_SKILL:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			SetRealPoint(type, val);
			break;

		case POINT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);

			PointChange(POINT_CLIENT_DEF_GRADE, amount);
			break;

		case POINT_CLIENT_DEF_GRADE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_ST:
		case POINT_HT:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HP_REGEN:
		case POINT_SP_REGEN:
		case POINT_ATT_SPEED:
		case POINT_ATT_GRADE:
		case POINT_MOV_SPEED:
		case POINT_CASTING_SPEED:
		case POINT_MAGIC_ATT_GRADE:
		case POINT_MAGIC_DEF_GRADE:
		case POINT_BOW_DISTANCE:
		case POINT_HP_RECOVERY:
		case POINT_SP_RECOVERY:

		case POINT_ATTBONUS_HUMAN:
		case POINT_ATTBONUS_ANIMAL:
		case POINT_ATTBONUS_ORC:
		case POINT_ATTBONUS_MILGYO:
		case POINT_ATTBONUS_UNDEAD:
		case POINT_ATTBONUS_DEVIL:

		case POINT_ATTBONUS_MONSTER:
		case POINT_ATTBONUS_SURA:
		case POINT_ATTBONUS_ASSASSIN:
		case POINT_ATTBONUS_WARRIOR:
		case POINT_ATTBONUS_SHAMAN:

		case POINT_POISON_PCT:
		case POINT_STUN_PCT:
		case POINT_SLOW_PCT:

		case POINT_BLOCK:
		case POINT_DODGE:

		case POINT_CRITICAL_PCT:
		case POINT_RESIST_CRITICAL:
		case POINT_PENETRATE_PCT:
		case POINT_RESIST_PENETRATE:
		case POINT_CURSE_PCT:

		case POINT_STEAL_HP:
		case POINT_STEAL_SP:

		case POINT_MANA_BURN_PCT:
		case POINT_DAMAGE_SP_RECOVER:
		case POINT_RESIST_NORMAL_DAMAGE:
		case POINT_RESIST_SWORD:
		case POINT_RESIST_TWOHAND:
		case POINT_RESIST_DAGGER:
		case POINT_RESIST_BELL:
		case POINT_RESIST_FAN:
		case POINT_RESIST_BOW:
		case POINT_RESIST_FIRE:
		case POINT_RESIST_ELEC:
		case POINT_RESIST_MAGIC:
		case POINT_RESIST_WIND:
		case POINT_RESIST_ICE:
		case POINT_RESIST_EARTH:
		case POINT_RESIST_DARK:
		case POINT_REFLECT_MELEE:
		case POINT_REFLECT_CURSE:
		case POINT_POISON_REDUCE:
		case POINT_KILL_SP_RECOVER:
		case POINT_KILL_HP_RECOVERY:
		case POINT_HIT_HP_RECOVERY:
		case POINT_HIT_SP_RECOVERY:
		case POINT_MANASHIELD:
		case POINT_ATT_BONUS:
		case POINT_DEF_BONUS:
		case POINT_SKILL_DAMAGE_BONUS:
		case POINT_NORMAL_HIT_DAMAGE_BONUS:
			// DEPEND_BONUS_ATTRIBUTES
		case POINT_SKILL_DEFEND_BONUS:
		case POINT_NORMAL_HIT_DEFEND_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
			// END_OF_DEPEND_BONUS_ATTRIBUTES

		case POINT_PARTY_ATTACKER_BONUS:
		case POINT_PARTY_TANKER_BONUS:
		case POINT_PARTY_BUFFER_BONUS:
		case POINT_PARTY_SKILL_MASTER_BONUS:
		case POINT_PARTY_HASTE_BONUS:
		case POINT_PARTY_DEFENDER_BONUS:

		case POINT_RESIST_WARRIOR:
		case POINT_RESIST_ASSASSIN:
		case POINT_RESIST_SURA:
		case POINT_RESIST_SHAMAN:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_MALL_ATTBONUS:
		case POINT_MALL_DEFBONUS:
		case POINT_MALL_EXPBONUS:
		case POINT_MALL_ITEMBONUS:
		case POINT_MALL_GOLDBONUS:
		case POINT_MELEE_MAGIC_ATT_BONUS_PER:
			if (GetPoint(type) + amount > 100)
			{
				sys_err("MALL_BONUS exceeded over 100!! point type: %d name: %s amount %d", type, GetName(), amount);
				amount = 100 - GetPoint(type);
			}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_RAMADAN_CANDY_BONUS_EXP:
			SetPoint(type, amount);
			val = GetPoint(type);
			break;

		case POINT_EXP_DOUBLE_BONUS:
		case POINT_GOLD_DOUBLE_BONUS:
		case POINT_ITEM_DROP_BONUS:
		case POINT_POTION_BONUS:
			if (GetPoint(type) + amount > 100)
			{
				sys_err("BONUS exceeded over 100!! point type: %d name: %s amount %d", type, GetName(), amount);
				amount = 100 - GetPoint(type);
			}

			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;

		case POINT_IMMUNE_STUN:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_STUN);
			}
			break;

		case POINT_IMMUNE_SLOW:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_SLOW);
			}
			break;

		case POINT_IMMUNE_FALL:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			if (val)
			{
				SET_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			}
			else
			{
				REMOVE_BIT(m_pointsInstant.dwImmuneFlag, IMMUNE_FALL);
			}
			break;

		case POINT_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_ATT_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_ATT_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_MAGIC_DEF_GRADE_BONUS:
			SetPoint(type, GetPoint(type) + amount);
			PointChange(POINT_MAGIC_DEF_GRADE, amount);
			val = GetPoint(type);
			break;

		case POINT_VOICE:
		case POINT_EMPIRE_POINT:
			val = GetRealPoint(type);
			break;

		case POINT_POLYMORPH:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			SetPolymorph(val);
			break;

		case POINT_MOUNT:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			MountVnum(val);
			break;

		case POINT_ENERGY:
		case POINT_COSTUME_ATTR_BONUS:
			{
				int old_val = GetPoint(type);
				SetPoint(type, old_val + amount);
				val = GetPoint(type);
				BuffOnAttr_ValueChange(type, old_val, val);
			}
			break;

#ifdef ENABLE_PENDANT_SYSTEM
		case POINT_ATTBONUS_ELEC:
		case POINT_ATTBONUS_FIRE:
		case POINT_ATTBONUS_ICE:
		case POINT_ATTBONUS_WIND:
		case POINT_ATTBONUS_EARTH:
		case POINT_ATTBONUS_DARK:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case POINT_ACCEDRAIN_RATE:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
#endif

#ifdef ENABLE_ATTBONUS_METIN
		case POINT_ATTBONUS_METIN:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
#endif

#ifdef ENABLE_ATTBONUS_BOSS
		case POINT_ATTBONUS_BOSS:
			SetPoint(type, GetPoint(type) + amount);
			val = GetPoint(type);
			break;
#endif

		default:
			sys_err("CHARACTER::PointChange: %s: unknown point change type %d", GetName(), type);
			return;
	}

	switch (type)
	{
		case POINT_LEVEL:
		case POINT_ST:
		case POINT_DX:
		case POINT_IQ:
		case POINT_HT:
			ComputeBattlePoints();
			break;
		case POINT_MAX_HP:
		case POINT_MAX_SP:
		case POINT_MAX_STAMINA:
			break;
	}

	if (type == POINT_HP && amount == 0)
		return;

	if (GetDesc())
	{
		struct packet_point_change pack;

		pack.header = HEADER_GC_CHARACTER_POINT_CHANGE;
		pack.dwVID = m_vid;
		pack.type = type;
		pack.value = val;

		if (bAmount)
			pack.amount = amount;
		else
			pack.amount = 0;

		if (!bBroadcast)
			GetDesc()->Packet(&pack, sizeof(struct packet_point_change));
		else
			PacketAround(&pack, sizeof(pack));
	}
}

void CHARACTER::ApplyPoint(BYTE bApplyType, int iVal)
{
	switch (bApplyType)
	{
		case APPLY_NONE:			// 0
			break;;

		case APPLY_CON:
			PointChange(POINT_HT, iVal);
			PointChange(POINT_MAX_HP, (iVal * JobInitialPoints[GetJob()].hp_per_ht));
			PointChange(POINT_MAX_STAMINA, (iVal * JobInitialPoints[GetJob()].stamina_per_con));
			break;

		case APPLY_INT:
			PointChange(POINT_IQ, iVal);
			PointChange(POINT_MAX_SP, (iVal * JobInitialPoints[GetJob()].sp_per_iq));
			break;

		case APPLY_SKILL:
			// SKILL_DAMAGE_BONUS
			{
				BYTE bSkillVnum = (BYTE) (((DWORD)iVal) >> 24);
				int iAdd = iVal & 0x00800000;
				int iChange = iVal & 0x007fffff;

				sys_log(1, "APPLY_SKILL skill %d add? %d change %d", bSkillVnum, iAdd ? 1 : 0, iChange);

				if (0 == iAdd)
					iChange = -iChange;

				std::unordered_map<BYTE, int>::iterator iter = m_SkillDamageBonus.find(bSkillVnum);

				if (iter == m_SkillDamageBonus.end())
					m_SkillDamageBonus.insert(std::make_pair(bSkillVnum, iChange));
				else
					iter->second += iChange;
			}
			// END_OF_SKILL_DAMAGE_BONUS
	
		case APPLY_MAX_HP:
		case APPLY_MAX_HP_PCT:
			{
				int i = GetMaxHP(); if(i == 0) break;
				PointChange(aApplyInfo[bApplyType].bPointType, iVal);
				float fRatio = (float)GetMaxHP() / (float)i;
				PointChange(POINT_HP, GetHP() * fRatio - GetHP());
			}
			break;

		case APPLY_MAX_SP:
		case APPLY_MAX_SP_PCT:
			{
				int i = GetMaxSP(); if(i == 0) break;
				PointChange(aApplyInfo[bApplyType].bPointType, iVal);
				float fRatio = (float)GetMaxSP() / (float)i;
				PointChange(POINT_SP, GetSP() * fRatio - GetSP());
			}
			break;

		case APPLY_STR:
		case APPLY_DEX:
		case APPLY_ATT_SPEED:
		case APPLY_MOV_SPEED:
		case APPLY_CAST_SPEED:
		case APPLY_HP_REGEN:
		case APPLY_SP_REGEN:
		case APPLY_POISON_PCT:
		case APPLY_STUN_PCT:
		case APPLY_SLOW_PCT:
		case APPLY_CRITICAL_PCT:
		case APPLY_PENETRATE_PCT:
		case APPLY_ATTBONUS_HUMAN:
		case APPLY_ATTBONUS_ANIMAL:
		case APPLY_ATTBONUS_ORC:
		case APPLY_ATTBONUS_MILGYO:
		case APPLY_ATTBONUS_UNDEAD:
		case APPLY_ATTBONUS_DEVIL:
		case APPLY_ATTBONUS_WARRIOR:	// 59
		case APPLY_ATTBONUS_ASSASSIN:	// 60
		case APPLY_ATTBONUS_SURA:	// 61
		case APPLY_ATTBONUS_SHAMAN:	// 62
		case APPLY_ATTBONUS_MONSTER:	// 63
		case APPLY_STEAL_HP:
		case APPLY_STEAL_SP:
		case APPLY_MANA_BURN_PCT:
		case APPLY_DAMAGE_SP_RECOVER:
		case APPLY_BLOCK:
		case APPLY_DODGE:
		case APPLY_RESIST_SWORD:
		case APPLY_RESIST_TWOHAND:
		case APPLY_RESIST_DAGGER:
		case APPLY_RESIST_BELL:
		case APPLY_RESIST_FAN:
		case APPLY_RESIST_BOW:
		case APPLY_RESIST_FIRE:
		case APPLY_RESIST_ELEC:
		case APPLY_RESIST_MAGIC:
		case APPLY_RESIST_WIND:
		case APPLY_RESIST_ICE:
		case APPLY_RESIST_EARTH:
		case APPLY_RESIST_DARK:
		case APPLY_REFLECT_MELEE:
		case APPLY_REFLECT_CURSE:
		case APPLY_ANTI_CRITICAL_PCT:
		case APPLY_ANTI_PENETRATE_PCT:
		case APPLY_POISON_REDUCE:
		case APPLY_KILL_SP_RECOVER:
		case APPLY_EXP_DOUBLE_BONUS:
		case APPLY_GOLD_DOUBLE_BONUS:
		case APPLY_ITEM_DROP_BONUS:
		case APPLY_POTION_BONUS:
		case APPLY_KILL_HP_RECOVER:
		case APPLY_IMMUNE_STUN:
		case APPLY_IMMUNE_SLOW:
		case APPLY_IMMUNE_FALL:
		case APPLY_BOW_DISTANCE:
		case APPLY_ATT_GRADE_BONUS:
		case APPLY_DEF_GRADE_BONUS:
		case APPLY_MAGIC_ATT_GRADE:
		case APPLY_MAGIC_DEF_GRADE:
		case APPLY_CURSE_PCT:
		case APPLY_MAX_STAMINA:
		case APPLY_MALL_ATTBONUS:
		case APPLY_MALL_DEFBONUS:
		case APPLY_MALL_EXPBONUS:
		case APPLY_MALL_ITEMBONUS:
		case APPLY_MALL_GOLDBONUS:
		case APPLY_SKILL_DAMAGE_BONUS:
		case APPLY_NORMAL_HIT_DAMAGE_BONUS:
			// DEPEND_BONUS_ATTRIBUTES
		case APPLY_SKILL_DEFEND_BONUS:
		case APPLY_NORMAL_HIT_DEFEND_BONUS:
			// END_OF_DEPEND_BONUS_ATTRIBUTES

		case APPLY_RESIST_WARRIOR:
		case APPLY_RESIST_ASSASSIN:
		case APPLY_RESIST_SURA:
		case APPLY_RESIST_SHAMAN:
		case APPLY_ENERGY:
		case APPLY_DEF_GRADE:
		case APPLY_COSTUME_ATTR_BONUS:
		case APPLY_MAGIC_ATTBONUS_PER:
		case APPLY_MELEE_MAGIC_ATTBONUS_PER:
#ifdef ENABLE_PENDANT_SYSTEM
		case APPLY_ATTBONUS_ELEC:
		case APPLY_ATTBONUS_FIRE:
		case APPLY_ATTBONUS_ICE:
		case APPLY_ATTBONUS_WIND:
		case APPLY_ATTBONUS_EARTH:
		case APPLY_ATTBONUS_DARK:
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case APPLY_ACCEDRAIN_RATE:
#endif
#ifdef ENABLE_ATTBONUS_METIN
		case APPLY_ATTBONUS_METIN:
#endif
#ifdef ENABLE_ATTBONUS_BOSS
		case APPLY_ATTBONUS_BOSS:
#endif
			PointChange(aApplyInfo[bApplyType].bPointType, iVal);
			break;

		default:
			sys_err("Unknown apply type %d name %s", bApplyType, GetName());
			break;
	}
}

void CHARACTER::MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet)
{
	packet->header	= HEADER_GC_MOTION;
	packet->vid		= m_vid;
	packet->motion	= motion;

	if (victim)
		packet->victim_vid = victim->GetVID();
	else
		packet->victim_vid = 0;
}

void CHARACTER::Motion(BYTE motion, LPCHARACTER victim)
{
	struct packet_motion pack_motion;
	MotionPacketEncode(motion, victim, &pack_motion);
	PacketAround(&pack_motion, sizeof(struct packet_motion));
}

EVENTFUNC(save_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if (info == NULL)
	{
		sys_err( "save_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;

	if (ch == NULL)
	{
		return 0;
	}

	sys_log(1, "SAVE_EVENT: %s", ch->GetName());
	ch->Save();
	ch->FlushDelayedSaveItem();
	return (save_event_second_cycle);
}

void CHARACTER::StartSaveEvent()
{
	if (m_pkSaveEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;
	m_pkSaveEvent = event_create(save_event, info, save_event_second_cycle);
}

void CHARACTER::MonsterLog(const char* format, ...)
{
	if (!test_server)
		return;

	if (IsPC())
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	int len = snprintf(chatbuf, sizeof(chatbuf), "%u)", (DWORD)GetVID());

	if (len < 0 || len >= (int) sizeof(chatbuf))
		len = sizeof(chatbuf) - 1;

	va_list args;

	va_start(args, format);

	int len2 = vsnprintf(chatbuf + len, sizeof(chatbuf) - len, format, args);

	if (len2 < 0 || len2 >= (int) sizeof(chatbuf) - len)
		len += (sizeof(chatbuf) - len) - 1;
	else
		len += len2;

	++len;

	va_end(args);

	TPacketGCChat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size		= sizeof(TPacketGCChat) + len;
	pack_chat.type      = CHAT_TYPE_TALKING;
	pack_chat.id        = (DWORD)GetVID();
	pack_chat.bEmpire	= 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(TPacketGCChat));
	buf.write(chatbuf, len);

	CHARACTER_MANAGER::instance().PacketMonsterLog(this, buf.read_peek(), buf.size());
}

void CHARACTER::ChatPacket(BYTE type, const char* format, ...)
{
	LPDESC d = GetDesc();

	if (!d || !format)
		return;

	char chatbuf[CHAT_MAX_LEN + 1];
	va_list args;

	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	struct packet_chat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size      = sizeof(struct packet_chat) + len;
	pack_chat.type      = type;
	pack_chat.id        = 0;
	pack_chat.bEmpire   = d->GetEmpire();

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());
}

#ifdef ENABLE_CLIENT_LOCALE_STRING
void CHARACTER::LocaleChatPacket(BYTE type, DWORD index, const char* format, ...)
{
	if (type != CHAT_TYPE_INFO && type != CHAT_TYPE_NOTICE && type != CHAT_TYPE_BIG_NOTICE
#ifdef ENABLE_DICE_SYSTEM
		&& type != CHAT_TYPE_DICE_INFO
#endif
#ifdef ENABLE_RENEWAL_OX_EVENT
		&& type != CHAT_TYPE_CONTROL_NOTICE
#endif
	)
		return;

	LPDESC d = GetDesc();

	if (!d)
		return;

	char chatbuf[256];
	va_list args;

	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	TPacketGCLocaleChat packet;
	packet.header = HEADER_GC_LOCALE_CHAT;
	packet.size = sizeof(packet) + len;
	packet.type = type;
	packet.id = index;

	TEMP_BUFFER buf;
	buf.write(&packet, sizeof(packet));

	if (len > 0)
		buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());
}

void CHARACTER::LocaleWhisperPacket(BYTE type, DWORD index, const char* namefrom, const char* format, ...)
{
	LPDESC d = GetDesc();

	if (!d)
		return;

	char chatbuf[256];
	va_list args;

	va_start(args, format);
	int len = vsnprintf(chatbuf, sizeof(chatbuf), format, args);
	va_end(args);

	TPacketGCLocaleWhisper packet;
	packet.header = HEADER_GC_LOCALE_WHISPER;
	packet.size = sizeof(packet) + len;
	packet.type = type;
	packet.id = index;
	strlcpy(packet.namefrom, namefrom, sizeof(packet.namefrom));

	TEMP_BUFFER buf;
	buf.write(&packet, sizeof(packet));

	if (len > 0)
		buf.write(chatbuf, len);

	d->Packet(buf.read_peek(), buf.size());
}
#endif

// MINING
void CHARACTER::mining_take()
{
	m_pkMiningEvent = NULL;
}

void CHARACTER::mining_cancel()
{
	if (m_pkMiningEvent)
	{
		sys_log(0, "XXX MINING CANCEL");
		event_cancel(&m_pkMiningEvent);
		LocaleChatPacket(CHAT_TYPE_INFO, 23, "");
	}
}

void CHARACTER::mining(LPCHARACTER chLoad)
{
	if (m_pkMiningEvent)
	{
		mining_cancel();
		return;
	}

	if (!chLoad)
		return;

	// @fixme128
	if (GetMapIndex() != chLoad->GetMapIndex() || DISTANCE_APPROX(GetX() - chLoad->GetX(), GetY() - chLoad->GetY()) > 1000)
		return;

	if (mining::GetRawOreFromLoad(chLoad->GetRaceNum()) == 0)
		return;

	LPITEM pick = GetWear(WEAR_WEAPON);

	if (!pick || pick->GetType() != ITEM_PICK)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 24, "");
		return;
	}

#ifdef ENABLE_MINING_DISTANCE_FIX
	if (DISTANCE_APPROX(GetX() - chLoad->GetX(), GetY() - chLoad->GetY()) > 2500) //T#77 A special mining bot can mine from everywhere with a faked OnClick packet.
	{
		LogManager::instance().HackLog("MINING_LOCATION", this);
		return;
	}
#endif

	int count = number(5, 15);

	TPacketGCDigMotion p;
	p.header = HEADER_GC_DIG_MOTION;
	p.vid = GetVID();
	p.target_vid = chLoad->GetVID();
	p.count = count;

	PacketAround(&p, sizeof(p));

	m_pkMiningEvent = mining::CreateMiningEvent(this, chLoad, count);
}
// END_OF_MINING

void CHARACTER::fishing()
{
	if (m_pkFishingEvent)
	{
		fishing_take();
		return;
	}

	{
		LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

		int	x = GetX();
		int y = GetY();

		LPSECTREE tree = pkSectreeMap->Find(x, y);
		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 25, "");
			return;
		}
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (GetActiveGrowthPet())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 737, "");
		return;
	}

	if (IsPetHatchWindowOpen())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 739, "");
		return;
	}

	if (IsPetChangeNameWindowOpen())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 738, "");
		return;
	}
#endif

	LPITEM rod = GetWear(WEAR_WEAPON);

	if (!rod || rod->GetType() != ITEM_ROD)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 26, "");
		return;
	}

	if (0 == rod->GetSocket(2))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 27, "");
		return;
	}

	float fx, fy;
	GetDeltaByDegree(GetRotation(), 400.0f, &fx, &fy);

	m_pkFishingEvent = fishing::CreateFishingEvent(this);
}

void CHARACTER::fishing_take()
{
	LPITEM rod = GetWear(WEAR_WEAPON);
	if (rod && rod->GetType() == ITEM_ROD)
	{
		using fishing::fishing_event_info;
		if (m_pkFishingEvent)
		{
			struct fishing_event_info* info = dynamic_cast<struct fishing_event_info*>(m_pkFishingEvent->info);

			if (info)
				fishing::Take(info, this);
		}
	}
	else
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 28, "");
	}

	event_cancel(&m_pkFishingEvent);
}

bool CHARACTER::StartStateMachine(int iNextPulse)
{
	if (CHARACTER_MANAGER::instance().AddToStateList(this))
	{
		m_dwNextStatePulse = thecore_heart->pulse + iNextPulse;
		return true;
	}

	return false;
}

void CHARACTER::StopStateMachine()
{
	CHARACTER_MANAGER::instance().RemoveFromStateList(this);
}

void CHARACTER::UpdateStateMachine(DWORD dwPulse)
{
	if (dwPulse < m_dwNextStatePulse)
		return;

	if (IsDead())
		return;

	Update();
	m_dwNextStatePulse = dwPulse + m_dwStateDuration;
}

void CHARACTER::SetNextStatePulse(int iNextPulse)
{
	CHARACTER_MANAGER::instance().AddToStateList(this);
	m_dwNextStatePulse = iNextPulse;

	if (iNextPulse < 10)
		MonsterLog("´ÙÀ½»óÅÂ·Î¾î¼­°¡ÀÚ");
}

void CHARACTER::UpdateCharacter(DWORD dwPulse)
{
	CFSM::Update();

	/*
	// This method has in client too open which one your want both same code.
	if (m_bAutoHuntStatus)
	{
		const static std::map<BYTE, std::pair<DWORD, int>> m_vecItemData = {
			{
				0,
				{
					70038,//itemIdx
					5//itemcooldown
				}
			},
			{
				1,
				{
					72049,//itemIdx
					60 * 5//itemcooldown
				}
			},
			{
				2,
				{
					71016,//itemIdx
					60 * 5//itemcooldown
				}
			}
		};

		const int now = time(0);
		for (itertype(m_vecAutoHuntItems) it = m_vecAutoHuntItems.begin(); it != m_vecAutoHuntItems.end(); ++it)
		{
			if (it->second > now)
				continue;
			const auto itData = m_vecItemData.find(it->first);
			if (itData != m_vecItemData.end())
			{
				LPITEM item = FindSpecifyItem(itData->second.first);
				if (item)
				{
					UseItemEx(item, NPOS);
					it->second = now + itData->second.second;
				}
			}
		}
	}
	*/
}

void CHARACTER::SetShop(LPSHOP pkShop)
{
	if ((m_pkShop = pkShop))
		SET_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP);
	else
	{
		REMOVE_BIT(m_pointsInstant.instant_flag, INSTANT_FLAG_SHOP); 
		SetShopOwner(NULL);
	}
}

void CHARACTER::SetExchange(CExchange * pkExchange)
{
	m_pkExchange = pkExchange;
}

void CHARACTER::SetPart(BYTE bPartPos, WORD wVal)
{
	assert(bPartPos < PART_MAX_NUM);
	m_pointsInstant.parts[bPartPos] = wVal;
}

WORD CHARACTER::GetPart(BYTE bPartPos) const
{
	assert(bPartPos < PART_MAX_NUM);

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	if (bPartPos == PART_MAIN && GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden())
	{
		if (const LPITEM pArmor = GetWear(WEAR_BODY))
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			return pArmor->GetTransmutationVnum() != 0 ? pArmor->GetTransmutationVnum() : pArmor->GetVnum();
#else
			return pArmor->GetVnum();
#endif
		else
			return 0;
	}
	else if (bPartPos == PART_HAIR && GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden())
		return 0;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	else if (bPartPos == PART_ACCE && GetWear(WEAR_COSTUME_ACCE) && IsAcceCostumeHidden())
		return 0;
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	else if (bPartPos == PART_WEAPON && GetWear(WEAR_COSTUME_WEAPON) && IsWeaponCostumeHidden())
	{
		if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			return pWeapon->GetTransmutationVnum() != 0 ? pWeapon->GetTransmutationVnum() : pWeapon->GetVnum();
#else
			return pWeapon->GetVnum();
#endif
		else
			return 0;
	}
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	else if (bPartPos == PART_AURA && GetWear(WEAR_COSTUME_AURA) && IsAuraCostumeHidden())
		return 0;
#endif
#endif

	return m_pointsInstant.parts[bPartPos];
}

WORD CHARACTER::GetOriginalPart(BYTE bPartPos) const
{
	switch (bPartPos)
	{
		case PART_MAIN:
		{
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
			if (GetWear(WEAR_COSTUME_BODY) && IsBodyCostumeHidden())
				if (const LPITEM pArmor = GetWear(WEAR_BODY))
					return pArmor->GetVnum();
#endif
			if (!IsPC())
				return GetPart(PART_MAIN);
			else
				return m_pointsInstant.bBasePart;
		}

		case PART_HAIR:
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
			if (GetWear(WEAR_COSTUME_HAIR) && IsHairCostumeHidden())
				return 0;
#endif
			return GetPart(PART_HAIR);

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		case PART_ACCE:
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
			if (GetWear(WEAR_COSTUME_ACCE) && IsAcceCostumeHidden())
				return 0;
#endif
			return GetPart(PART_ACCE);
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		case PART_WEAPON:
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
			if (GetWear(WEAR_COSTUME_WEAPON) && IsWeaponCostumeHidden())
				if (const LPITEM pWeapon = GetWear(WEAR_WEAPON))
					return pWeapon->GetVnum();
#endif
			return GetWear(WEAR_COSTUME_WEAPON) ? GetPart(PART_WEAPON) : 0;
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
		case PART_AURA:
#ifdef ENABLE_HIDE_COSTUME_SYSTEM
			if (GetWear(WEAR_COSTUME_AURA) && IsAuraCostumeHidden())
				return 0;
#endif
			return GetWear(WEAR_COSTUME_AURA) ? GetPart(PART_AURA) : 0;
#endif

		default:
			return 0;
	}
}

BYTE CHARACTER::GetCharType() const
{
	return m_bCharType;
}

bool CHARACTER::SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList)
{
	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
		return false;
	// END_OF_TRENT_MONSTER

	if (ch == this)
	{
		sys_err("SetSyncOwner owner == this (%p)", this);
		return false;
	}

	if (!ch)
	{
		if (bRemoveFromList && m_pkChrSyncOwner)
		{
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
		}

		if (m_pkChrSyncOwner)
			sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());

		m_pkChrSyncOwner = NULL;
	}
	else
	{
		if (!IsSyncOwner(ch))
			return false;

		if (DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY()) > 250)
		{
			sys_log(1, "SetSyncOwner distance over than 250 %s %s", GetName(), ch->GetName());

			if (m_pkChrSyncOwner == ch)
				return true;

			return false;
		}

		if (m_pkChrSyncOwner != ch)
		{
			if (m_pkChrSyncOwner)
			{
				sys_log(1, "SyncRelease %s %p from %s", GetName(), this, m_pkChrSyncOwner->GetName());
				m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.remove(this);
			}

			m_pkChrSyncOwner = ch;
			m_pkChrSyncOwner->m_kLst_pkChrSyncOwned.push_back(this);

			static const timeval zero_tv = {0, 0};
			SetLastSyncTime(zero_tv);

			sys_log(1, "SetSyncOwner set %s %p to %s", GetName(), this, ch->GetName());
		}

		m_fSyncTime = get_float_time();
	}

	TPacketGCOwnership pack;

	pack.bHeader	= HEADER_GC_OWNERSHIP;
	pack.dwOwnerVID	= ch ? ch->GetVID() : 0;
	pack.dwVictimVID	= GetVID();

	PacketAround(&pack, sizeof(TPacketGCOwnership));
	return true;
}

struct FuncClearSync
{
	void operator () (LPCHARACTER ch)
	{
		assert(ch != NULL);
		ch->SetSyncOwner(NULL, false);
	}
};

void CHARACTER::ClearSync()
{
	SetSyncOwner(NULL);

	std::for_each(m_kLst_pkChrSyncOwned.begin(), m_kLst_pkChrSyncOwned.end(), FuncClearSync());
	m_kLst_pkChrSyncOwned.clear();
}

bool CHARACTER::IsSyncOwner(LPCHARACTER ch) const
{
	if (m_pkChrSyncOwner == ch)
		return true;

	if (get_float_time() - m_fSyncTime >= 3.0f)
		return true;

	return false;
}

void CHARACTER::SetParty(LPPARTY pkParty)
{
	if (pkParty == m_pkParty)
		return;

	if (pkParty && m_pkParty)
		sys_err("%s is trying to reassigning party (current %p, new party %p)", GetName(), get_pointer(m_pkParty), get_pointer(pkParty));

	sys_log(1, "PARTY set to %p", get_pointer(pkParty));

#ifdef ENABLE_DUNGEON_CORE_CRASH_FIX
	if (m_pkDungeon && IsPC() && !pkParty)
		SetDungeon(NULL);
#else
	if (m_pkDungeon && IsPC())
	SetDungeon(NULL);
#endif

	m_pkParty = pkParty;

	if (IsPC())
	{
		if (m_pkParty)
			SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);
		else
			REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_PARTY);

		UpdatePacket();
	}
}

EVENTINFO(TPartyJoinEventInfo)
{
	DWORD	dwGuestPID;
	DWORD	dwLeaderPID;

	TPartyJoinEventInfo()
	: dwGuestPID( 0 )
	, dwLeaderPID( 0 )
	{
	}
} ;

EVENTFUNC(party_request_event)
{
	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo*> (event->info);

	if (info == NULL)
	{
		sys_err( "party_request_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->dwGuestPID);

	if (ch)
	{
		sys_log(0, "PartyRequestEvent %s", ch->GetName());
		ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
		ch->SetPartyRequestEvent(NULL);
	}

	return 0;
}

bool CHARACTER::RequestToParty(LPCHARACTER leader)
{
	if (leader->GetParty())
		leader = leader->GetParty()->GetLeaderCharacter();

	if (!leader)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 29, "");
		return false;
	}

	if (m_pkPartyRequestEvent)
		return false;

	if (!IsPC() || !leader->IsPC())
		return false;

	if (leader->IsBlockMode(BLOCK_PARTY_REQUEST))
		return false;

	PartyJoinErrCode errcode = IsPartyJoinableCondition(leader, this);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			LocaleChatPacket(CHAT_TYPE_INFO, 30, "");
			return false;

		case PERR_DIFFEMPIRE:
			LocaleChatPacket(CHAT_TYPE_INFO, 31, "");
			return false;

		case PERR_DUNGEON:
			LocaleChatPacket(CHAT_TYPE_INFO, 32, "");
			return false;

		case PERR_OBSERVER:
			LocaleChatPacket(CHAT_TYPE_INFO, 33, "");
			return false;

		case PERR_LVBOUNDARY:
			LocaleChatPacket(CHAT_TYPE_INFO, 34, "");
			return false;

		case PERR_LOWLEVEL:
			LocaleChatPacket(CHAT_TYPE_INFO, 35, "");
			return false;

		case PERR_HILEVEL:
			LocaleChatPacket(CHAT_TYPE_INFO, 36, "");
			return false;

		case PERR_ALREADYJOIN:
			return false;

		case PERR_PARTYISFULL:
			LocaleChatPacket(CHAT_TYPE_INFO, 37, "");
			return false;

		default:
			sys_err("Do not process party join error(%d)", errcode); 
			return false;
	}

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = GetPlayerID();
	info->dwLeaderPID = leader->GetPlayerID();

	SetPartyRequestEvent(event_create(party_request_event, info, PASSES_PER_SEC(10)));

	leader->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequest %u", (DWORD) GetVID());
	LocaleChatPacket(CHAT_TYPE_INFO, 38, "%s", leader->GetName());
	return true;
}

void CHARACTER::DenyToParty(LPCHARACTER member)
{
	sys_log(1, "DenyToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::DenyToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

void CHARACTER::AcceptToParty(LPCHARACTER member)
{
	sys_log(1, "AcceptToParty %s member %s %p", GetName(), member->GetName(), get_pointer(member->m_pkPartyRequestEvent));

	if (!member->m_pkPartyRequestEvent)
		return;

	TPartyJoinEventInfo * info = dynamic_cast<TPartyJoinEventInfo *>(member->m_pkPartyRequestEvent->info);

	if (!info)
	{
		sys_err( "CHARACTER::AcceptToParty> <Factor> Null pointer" );
		return;
	}

	if (info->dwGuestPID != member->GetPlayerID())
		return;

	if (info->dwLeaderPID != GetPlayerID())
		return;

	event_cancel(&member->m_pkPartyRequestEvent);

	if (!GetParty())
		member->LocaleChatPacket(CHAT_TYPE_INFO, 39, "");
	else 
	{
		if (GetPlayerID() != GetParty()->GetLeaderPID())
			return;

		PartyJoinErrCode errcode = IsPartyJoinableCondition(this, member);
		switch (errcode) 
		{
			case PERR_NONE:
				member->PartyJoin(this);
				return;

			case PERR_SERVER:
				member->LocaleChatPacket(CHAT_TYPE_INFO, 30, "");
				break;

			case PERR_DUNGEON:
				member->LocaleChatPacket(CHAT_TYPE_INFO, 32, "");
				break;

			case PERR_OBSERVER:
				member->LocaleChatPacket(CHAT_TYPE_INFO, 33, "");
				break;

			case PERR_LVBOUNDARY:
				member->LocaleChatPacket(CHAT_TYPE_INFO, 34, "");
				break;

			case PERR_LOWLEVEL:
				member->LocaleChatPacket(CHAT_TYPE_INFO, 35, "");
				break;

			case PERR_HILEVEL:
				member->LocaleChatPacket(CHAT_TYPE_INFO, 36, "");
				break;

			case PERR_ALREADYJOIN:
				break;

			case PERR_PARTYISFULL:
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 37, "");
				member->LocaleChatPacket(CHAT_TYPE_INFO, 40, "");
				break;
			}

			default:
				sys_err("Do not process party join error(%d)", errcode);
		}
	}

	member->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

/**
* Party invitation event callback function.
* When an event is triggered, it is treated as an invitation rejection.
*/
EVENTFUNC(party_invite_event)
{
	TPartyJoinEventInfo * pInfo = dynamic_cast<TPartyJoinEventInfo *>(  event->info );

	if ( pInfo == NULL )
	{
		sys_err( "party_invite_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER pchInviter = CHARACTER_MANAGER::instance().FindByPID(pInfo->dwLeaderPID);

	if (pchInviter)
	{
		sys_log(1, "PartyInviteEvent %s", pchInviter->GetName());
		pchInviter->PartyInviteDeny(pInfo->dwGuestPID);
	}

	return 0;
}

void CHARACTER::PartyInvite(LPCHARACTER pchInvitee)
{
	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 41, "");
		return;
	}
	else if (pchInvitee->IsBlockMode(BLOCK_PARTY_INVITE))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 42, "%s", pchInvitee->GetName());
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			LocaleChatPacket(CHAT_TYPE_INFO, 30, "");
			return;

		case PERR_DIFFEMPIRE:
			LocaleChatPacket(CHAT_TYPE_INFO, 31, "");
			return;

		case PERR_DUNGEON:
			LocaleChatPacket(CHAT_TYPE_INFO, 32, "");
			return;

		case PERR_OBSERVER:
			LocaleChatPacket(CHAT_TYPE_INFO, 33, "");
			return;

		case PERR_LVBOUNDARY:
			LocaleChatPacket(CHAT_TYPE_INFO, 34, "");
			return;

		case PERR_LOWLEVEL:
			LocaleChatPacket(CHAT_TYPE_INFO, 35, "");
			return;

		case PERR_HILEVEL:
			LocaleChatPacket(CHAT_TYPE_INFO, 36, "");
			return;

		case PERR_ALREADYJOIN:
			LocaleChatPacket(CHAT_TYPE_INFO, 43, "%s", pchInvitee->GetName());
			return;

		case PERR_PARTYISFULL:
			LocaleChatPacket(CHAT_TYPE_INFO, 37, "");
			return;

		default:
			sys_err("Do not process party join error(%d)", errcode);
			return;
	}

	if (m_PartyInviteEventMap.end() != m_PartyInviteEventMap.find(pchInvitee->GetPlayerID()))
		return;

	TPartyJoinEventInfo* info = AllocEventInfo<TPartyJoinEventInfo>();

	info->dwGuestPID = pchInvitee->GetPlayerID();
	info->dwLeaderPID = GetPlayerID();

	m_PartyInviteEventMap.insert(EventMap::value_type(pchInvitee->GetPlayerID(), event_create(party_invite_event, info, PASSES_PER_SEC(10))));

	TPacketGCPartyInvite p;
	p.header = HEADER_GC_PARTY_INVITE;
	p.leader_vid = GetVID();
	pchInvitee->GetDesc()->Packet(&p, sizeof(p));
}

void CHARACTER::PartyInviteAccept(LPCHARACTER pchInvitee)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(pchInvitee->GetPlayerID());

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteAccept from not invited character(%s)", pchInvitee->GetName());
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	if (GetParty() && GetParty()->GetLeaderPID() != GetPlayerID())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 41, "");
		return;
	}

	PartyJoinErrCode errcode = IsPartyJoinableMutableCondition(this, pchInvitee);

	switch (errcode)
	{
		case PERR_NONE:
			break;

		case PERR_SERVER:
			pchInvitee->LocaleChatPacket(CHAT_TYPE_INFO, 30, "");
			return;

		case PERR_DUNGEON:
			pchInvitee->LocaleChatPacket(CHAT_TYPE_INFO, 32, "");
			return;

		case PERR_OBSERVER:
			pchInvitee->LocaleChatPacket(CHAT_TYPE_INFO, 33, "");
			return;

		case PERR_LVBOUNDARY:
			pchInvitee->LocaleChatPacket(CHAT_TYPE_INFO, 34, "");
			return;

		case PERR_LOWLEVEL:
			pchInvitee->LocaleChatPacket(CHAT_TYPE_INFO, 35, "");
			return;

		case PERR_HILEVEL:
			pchInvitee->LocaleChatPacket(CHAT_TYPE_INFO, 36, "");
			return;

		case PERR_ALREADYJOIN:
			pchInvitee->LocaleChatPacket(CHAT_TYPE_INFO, 44, "");
			return;

		case PERR_PARTYISFULL:
			LocaleChatPacket(CHAT_TYPE_INFO, 37, "");
			pchInvitee->LocaleChatPacket(CHAT_TYPE_INFO, 40, "");
			return;

		default:
			sys_err("ignore party join error(%d)", errcode);
			return;
	}

	if (GetParty())
		pchInvitee->PartyJoin(this);
	else
	{
		LPPARTY pParty = CPartyManager::instance().CreateParty(this);

		pParty->Join(pchInvitee->GetPlayerID());
		pParty->Link(pchInvitee);
		pParty->SendPartyInfoAllToOne(this);
	}
}

void CHARACTER::PartyInviteDeny(DWORD dwPID)
{
	EventMap::iterator itFind = m_PartyInviteEventMap.find(dwPID);

	if (itFind == m_PartyInviteEventMap.end())
	{
		sys_log(1, "PartyInviteDeny to not exist event(inviter PID: %d, invitee PID: %d)", GetPlayerID(), dwPID);
		return;
	}

	event_cancel(&itFind->second);
	m_PartyInviteEventMap.erase(itFind);

	LPCHARACTER pchInvitee = CHARACTER_MANAGER::instance().FindByPID(dwPID);
	if (pchInvitee)
		LocaleChatPacket(CHAT_TYPE_INFO, 42, "%s", pchInvitee->GetName());
}

void CHARACTER::PartyJoin(LPCHARACTER pLeader)
{
	pLeader->LocaleChatPacket(CHAT_TYPE_INFO, 45, "%s", GetName());
	LocaleChatPacket(CHAT_TYPE_INFO, 45, "%s", pLeader->GetName());

	pLeader->GetParty()->Join(GetPlayerID());
	pLeader->GetParty()->Link(this);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (pchLeader->GetEmpire() != pchGuest->GetEmpire())
		return PERR_DIFFEMPIRE;

	return IsPartyJoinableMutableCondition(pchLeader, pchGuest);
}

static bool __party_can_join_by_level(LPCHARACTER leader, LPCHARACTER quest)
{
	return (abs (leader->GetLevel() - quest->GetLevel()) <= 30);
}

CHARACTER::PartyJoinErrCode CHARACTER::IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest)
{
	if (!CPartyManager::instance().IsEnablePCParty())
		return PERR_SERVER;
	else if (pchLeader->GetDungeon())
		return PERR_DUNGEON;
	else if (pchGuest->IsObserverMode())
		return PERR_OBSERVER;
	else if (false == __party_can_join_by_level(pchLeader, pchGuest))
		return PERR_LVBOUNDARY;
	else if (pchGuest->GetParty())
		return PERR_ALREADYJOIN;
	else if (pchLeader->GetParty())
	{
		if (pchLeader->GetParty()->GetMemberCount() == PARTY_MAX_MEMBER)
			return PERR_PARTYISFULL;
	}

	return PERR_NONE;
}

void CHARACTER::SetDungeon(LPDUNGEON pkDungeon)
{
	if (pkDungeon && m_pkDungeon)
		sys_err("%s is trying to reassigning dungeon (current %p, new party %p)", GetName(), get_pointer(m_pkDungeon), get_pointer(pkDungeon));

	if (m_pkDungeon == pkDungeon) {
		return;
	}

	if (m_pkDungeon)
	{
		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->DecPartyMember(GetParty(), this);
			else
				m_pkDungeon->DecMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->DecMonster();
		}
	}

	m_pkDungeon = pkDungeon;

	if (pkDungeon)
	{
		sys_log(0, "%s DUNGEON set to %p, PARTY is %p", GetName(), get_pointer(pkDungeon), get_pointer(m_pkParty));

		if (IsPC())
		{
			if (GetParty())
				m_pkDungeon->IncPartyMember(GetParty(), this);
			else
				m_pkDungeon->IncMember(this);
		}
		else if (IsMonster() || IsStone())
		{
			m_pkDungeon->IncMonster();
		}
	}
}

void CHARACTER::SetWarMap(CWarMap * pWarMap)
{
	if (m_pWarMap)
		m_pWarMap->DecMember(this);

	m_pWarMap = pWarMap;

	if (m_pWarMap)
		m_pWarMap->IncMember(this);
}

void CHARACTER::SetWeddingMap(marriage::WeddingMap* pMap)
{
	if (m_pWeddingMap)
		m_pWeddingMap->DecMember(this);

	m_pWeddingMap = pMap;

	if (m_pWeddingMap)
		m_pWeddingMap->IncMember(this);
}

void CHARACTER::SetRegen(LPREGEN pkRegen)
{
	m_pkRegen = pkRegen;
	if (pkRegen != NULL) {
		regen_id_ = pkRegen->id;
	}
	m_fRegenAngle = GetRotation();
	m_posRegen = GetXYZ();
}

bool CHARACTER::OnIdle()
{
	return false;
}

void CHARACTER::OnMove(bool bIsAttack)
{
	m_dwLastMoveTime = get_dword_time();

	if (bIsAttack)
	{
		m_dwLastAttackTime = m_dwLastMoveTime;

		if (IsAffectFlag(AFF_REVIVE_INVISIBLE))
			RemoveAffect(AFFECT_REVIVE_INVISIBLE);

		if (IsAffectFlag(AFF_EUNHYUNG))
		{
			RemoveAffect(SKILL_EUNHYUNG);
			SetAffectedEunhyung();
		}
		else
		{
			ClearAffectedEunhyung();
		}
	}

	// MINING
	mining_cancel();
	// END_OF_MINING
}

void CHARACTER::OnClick(LPCHARACTER pkChrCauser)
{
	if (!pkChrCauser)
	{
		sys_err("OnClick %s by NULL", GetName());
		return;
	}

	DWORD vid = GetVID();
	sys_log(0, "OnClick %s[vnum %d ServerUniqueID %d, pid %d] by %s", GetName(), GetRaceNum(), vid, GetPlayerID(), pkChrCauser->GetName());

	{
		if (pkChrCauser->GetMyShop() && pkChrCauser != this) 
		{
			sys_err("OnClick Fail (%s->%s) - pc has shop", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	{
		if (pkChrCauser->GetExchange())
		{
			sys_err("OnClick Fail (%s->%s) - pc is exchanging", pkChrCauser->GetName(), GetName());
			return;
		}
	}

	if (IsPC())
	{
		if (!CTargetManager::instance().GetTargetInfo(pkChrCauser->GetPlayerID(), TARGET_TYPE_VID, GetVID()))
		{
			if (GetMyShop())
			{
				if (pkChrCauser->IsDead() == true)
					return;

#ifdef ENABLE_GROWTH_PET_SYSTEM
				if (pkChrCauser->GetPetWindowType() == PET_WINDOW_ATTR_CHANGE || pkChrCauser->GetPetWindowType() == PET_WINDOW_PRIMIUM_FEEDSTUFF)
				{
					pkChrCauser->LocaleChatPacket(CHAT_TYPE_INFO, 738, "");
					return;
				}
#endif

				if (pkChrCauser == this)
				{
					if ((GetExchange() || IsOpenSafebox() || GetShopOwner()) || IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
						|| IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
						|| GetTransmutation()
#endif
						)
					{
						pkChrCauser->LocaleChatPacket(CHAT_TYPE_INFO, 46, "");
						return;
					}
				}
				else
				{
					if ((pkChrCauser->GetExchange() || pkChrCauser->IsOpenSafebox() || pkChrCauser->GetMyShop() || pkChrCauser->GetShopOwner()) || pkChrCauser->IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
						|| pkChrCauser->IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
						|| pkChrCauser->GetTransmutation()
#endif
						)
					{
						pkChrCauser->LocaleChatPacket(CHAT_TYPE_INFO, 46, "");
						return;
					}

					if ((GetExchange() || IsOpenSafebox() || IsCubeOpen())
#ifdef ENABLE_AURA_COSTUME_SYSTEM
						|| IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
						|| GetTransmutation()
#endif
						)
					{
						pkChrCauser->LocaleChatPacket(CHAT_TYPE_INFO, 47, "");
						return;
					}
				}

				if (pkChrCauser->GetShop())
				{
					pkChrCauser->GetShop()->RemoveGuest(pkChrCauser);
					pkChrCauser->SetShop(NULL);
				}

				GetMyShop()->AddGuest(pkChrCauser, GetVID(), false);
				pkChrCauser->SetShopOwner(this);
				return;
			}

			if (test_server)
				sys_err("%s.OnClickFailure(%s) - target is PC", pkChrCauser->GetName(), GetName());

			return;
		}
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (pkChrCauser->GetPetWindowType() == PET_WINDOW_ATTR_CHANGE || pkChrCauser->GetPetWindowType() == PET_WINDOW_PRIMIUM_FEEDSTUFF)
	{
		pkChrCauser->LocaleChatPacket(CHAT_TYPE_INFO, 738, "");
		return;
	}
#endif

	pkChrCauser->SetQuestNPCID(GetVID());

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	if (IsOfflineShopNPC())
	{
		if (pkChrCauser->CanOpenOfflineShop())
			return;

		GetOfflineShop()->AddGuest(pkChrCauser, this);
		return;
	}
#endif

	if (quest::CQuestManager::instance().Click(pkChrCauser->GetPlayerID(), this))
	{
		return;
	}

	if (!IsPC())
	{
		if (!m_triggerOnClick.pFunc)
		{
			return;
		}

		m_triggerOnClick.pFunc(this, pkChrCauser);
	}

}

BYTE CHARACTER::GetGMLevel() const
{
	if (test_server)
		return GM_IMPLEMENTOR;
	return m_pointsInstant.gm_level;
}

void CHARACTER::SetGMLevel()
{
	if (GetDesc())
	{
	    m_pointsInstant.gm_level =  gm_get_level(GetName(), GetDesc()->GetHostName(), GetDesc()->GetAccountTable().login);
	}
	else
	{
	    m_pointsInstant.gm_level = GM_PLAYER;
	}
}

BOOL CHARACTER::IsGM() const
{
	if (m_pointsInstant.gm_level != GM_PLAYER)
		return true;
	if (test_server)
		return true;
	return false;
}

void CHARACTER::SetStone(LPCHARACTER pkChrStone)
{
	m_pkChrStone = pkChrStone;

	if (m_pkChrStone)
	{
		if (pkChrStone->m_set_pkChrSpawnedBy.find(this) == pkChrStone->m_set_pkChrSpawnedBy.end())
			pkChrStone->m_set_pkChrSpawnedBy.insert(this);
	}
}

struct FuncDeadSpawnedByStone
{
	void operator () (LPCHARACTER ch)
	{
		ch->Dead(NULL);
		ch->SetStone(NULL);
	}
};

void CHARACTER::ClearStone()
{
	if (!m_set_pkChrSpawnedBy.empty())
	{
		FuncDeadSpawnedByStone f;
		std::for_each(m_set_pkChrSpawnedBy.begin(), m_set_pkChrSpawnedBy.end(), f);
		m_set_pkChrSpawnedBy.clear();
	}

	if (!m_pkChrStone)
		return;

	m_pkChrStone->m_set_pkChrSpawnedBy.erase(this);
	m_pkChrStone = NULL;
}

void CHARACTER::ClearTarget()
{
#ifdef __AUTO_HUNT__
	// AUTO_HUNT aktif oyuncular için rezervasyonu kaldýr
	if (m_pkChrTarget && IsPC() && m_bAutoHuntStatus && (m_pkChrTarget->IsStone() || m_pkChrTarget->IsNPC()))
	{
		// Eðer bu oyuncu tarafýndan rezerve edilmiþse kaldýr
		if (CHARACTER_MANAGER::instance().IsTargetReserved(m_pkChrTarget->GetVID(), GetPlayerID()))
		{
			CHARACTER_MANAGER::instance().ReleaseTarget(m_pkChrTarget->GetVID());
		}
	}
#endif

	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);
		m_pkChrTarget = NULL;
	}

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;
	p.dwVID = 0;
	p.bHPPercent = 0;
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
	p.iMinHP = 0;
	p.iMaxHP = 0;
#endif

	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();

	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *(it++);
		pkChr->m_pkChrTarget = NULL;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}

		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}

	m_set_pkChrTargetedBy.clear();
}

void CHARACTER::SetTarget(LPCHARACTER pkChrTarget)
{
	if (m_pkChrTarget == pkChrTarget)
		return;

#ifdef __AUTO_HUNT__
	// Eski hedefin rezervasyonunu kaldýr - SADECE AUTO_HUNT aktif olan oyuncular için
	if (m_pkChrTarget && IsPC() && m_bAutoHuntStatus && (m_pkChrTarget->IsStone() || m_pkChrTarget->IsNPC()))
	{
		// Eðer bu oyuncu tarafýndan rezerve edilmiþse kaldýr
		if (CHARACTER_MANAGER::instance().IsTargetReserved(m_pkChrTarget->GetVID(), GetPlayerID()))
		{
			CHARACTER_MANAGER::instance().ReleaseTarget(m_pkChrTarget->GetVID());
		}
	}
#endif

	if (m_pkChrTarget)
		m_pkChrTarget->m_set_pkChrTargetedBy.erase(this);

	m_pkChrTarget = pkChrTarget;

#ifdef __AUTO_HUNT__
	// Yeni hedefi rezerve et - SADECE AUTO_HUNT aktif olan oyuncular için
	if (m_pkChrTarget && IsPC() && m_bAutoHuntStatus && (m_pkChrTarget->IsStone() || m_pkChrTarget->IsNPC()))
	{
		CHARACTER_MANAGER::instance().ReserveTarget(m_pkChrTarget->GetVID(), GetPlayerID());
	}
#endif

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;

	if (m_pkChrTarget)
	{
		m_pkChrTarget->m_set_pkChrTargetedBy.insert(this);

		p.dwVID	= m_pkChrTarget->GetVID();
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
		if ((m_pkChrTarget->GetMaxHP() <= 0))
		{
			p.bHPPercent = 0;
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
		}
		else if (m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed())
		{
			p.bHPPercent = MINMAX(0, m_pkChrTarget->GetHPPct(), 100);
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
			p.iMinHP = m_pkChrTarget->GetHP();
			p.iMaxHP = m_pkChrTarget->GetMaxHP();
#endif
#else
		if ((m_pkChrTarget->IsPC() && !m_pkChrTarget->IsPolymorphed()) || (m_pkChrTarget->GetMaxHP() <= 0))
		{
			p.bHPPercent = 0;
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
#endif
		}
		else
		{
			if (m_pkChrTarget->GetRaceNum() == 20101 ||
					m_pkChrTarget->GetRaceNum() == 20102 ||
					m_pkChrTarget->GetRaceNum() == 20103 ||
					m_pkChrTarget->GetRaceNum() == 20104 ||
					m_pkChrTarget->GetRaceNum() == 20105 ||
					m_pkChrTarget->GetRaceNum() == 20106 ||
					m_pkChrTarget->GetRaceNum() == 20107 ||
					m_pkChrTarget->GetRaceNum() == 20108 ||
					m_pkChrTarget->GetRaceNum() == 20109
#ifdef ENABLE_RIDING_EXTENDED
					|| m_pkChrTarget->GetRaceNum() == 20149
					|| m_pkChrTarget->GetRaceNum() == 20150
					|| m_pkChrTarget->GetRaceNum() == 20151
#endif
			  )
			{
				LPCHARACTER owner = m_pkChrTarget->GetVictim();

				if (owner)
				{
					int iHorseHealth = owner->GetHorseHealth();
					int iHorseMaxHealth = owner->GetHorseMaxHealth();

					if (iHorseMaxHealth)
					{
						p.bHPPercent = MINMAX(0,  iHorseHealth * 100 / iHorseMaxHealth, 100);
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
						p.iMinHP = 100;
						p.iMaxHP = 100;
#endif
					}
					else
					{
						p.bHPPercent = 100;
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
						p.iMinHP = 100;
						p.iMaxHP = 100;
#endif
					}
				}
				else
				{
					p.bHPPercent = 100;
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
					p.iMinHP = 100;
					p.iMaxHP = 100;
#endif
				}
			}
			else
			{
				if (m_pkChrTarget->GetMaxHP() <= 0)
				{
					p.bHPPercent = 0;
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
					p.iMinHP = 0;
					p.iMaxHP = 0;
#endif
				}
				else
				{
					p.bHPPercent = MINMAX(0, m_pkChrTarget->GetHPPct(), 100);
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
					p.iMinHP = m_pkChrTarget->GetHP();
					p.iMaxHP = m_pkChrTarget->GetMaxHP();
#endif
				}
			}
		}
	}
	else
	{
		p.dwVID = 0;
		p.bHPPercent = 0;
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
		p.iMinHP = 0;
		p.iMaxHP = 0;
#endif
	}
	
	GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
}

void CHARACTER::BroadcastTargetPacket()
{
	if (m_set_pkChrTargetedBy.empty())
		return;

	TPacketGCTarget p;

	p.header = HEADER_GC_TARGET;
	p.dwVID = GetVID();
	if (GetMaxHP() <= 0)
	{
		p.bHPPercent = 0;
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
		p.iMinHP = 0;
		p.iMaxHP = 0;
#endif
	}
	else
	{
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
		p.bHPPercent = MINMAX(0, GetHPPct(), 100);
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
		p.iMinHP = GetHP();
		p.iMaxHP = GetMaxHP();
#endif
#else
		if (IsPC())
		{
			p.bHPPercent = 0;
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
			p.iMinHP = 0;
			p.iMaxHP = 0;
#endif
		}
		else
		{
			p.bHPPercent = MINMAX(0, (GetHP() * 100) / GetMaxHP(), 100);
#ifdef ENABLE_VIEW_TARGET_PLAYER_HP
			p.iMinHP = GetHP();
			p.iMaxHP = GetMaxHP();
#endif
		}
#endif
	}
	
	CHARACTER_SET::iterator it = m_set_pkChrTargetedBy.begin();

	while (it != m_set_pkChrTargetedBy.end())
	{
		LPCHARACTER pkChr = *it++;

		if (!pkChr->GetDesc())
		{
			sys_err("%s %p does not have desc", pkChr->GetName(), get_pointer(pkChr));
			abort();
		}

		pkChr->GetDesc()->Packet(&p, sizeof(TPacketGCTarget));
	}
}

void CHARACTER::CheckTarget()
{
	if (!m_pkChrTarget)
		return;

	if (DISTANCE_APPROX(GetX() - m_pkChrTarget->GetX(), GetY() - m_pkChrTarget->GetY()) >= 4800)
		SetTarget(NULL);
}

void CHARACTER::SetWarpLocation(long lMapIndex, long x, long y)
{
	m_posWarp.x = x * 100;
	m_posWarp.y = y * 100;
	m_lWarpMapIndex = lMapIndex;
}

void CHARACTER::SaveExitLocation()
{
	m_posExit = GetXYZ();
	m_lExitMapIndex = GetMapIndex();
}

void CHARACTER::ExitToSavedLocation()
{
	sys_log (0, "ExitToSavedLocation");
	WarpSet(m_posWarp.x, m_posWarp.y, m_lWarpMapIndex);

	m_posExit.x = m_posExit.y = m_posExit.z = 0;
	m_lExitMapIndex = 0;
}

bool CHARACTER::WarpSet(long x, long y, long lPrivateMapIndex)
{
	if (!IsPC())
		return false;

	long lAddr;
	long lMapIndex;
	WORD wPort;

	if (!CMapLocation::instance().Get(x, y, lMapIndex, lAddr, wPort))
	{
		sys_err("cannot find map location index %d x %d y %d name %s", lMapIndex, x, y, GetName());
		return false;
	}

	if (lPrivateMapIndex >= 10000)
	{
		if (lPrivateMapIndex / 10000 != lMapIndex)
		{
			sys_err("Invalid map index %d, must be child of %d", lPrivateMapIndex, lMapIndex);
			return false;
		}

		lMapIndex = lPrivateMapIndex;
	}

	Stop();
	Save();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();

		EncodeRemovePacket(this);
	}

	m_lWarpMapIndex = lMapIndex;
	m_posWarp.x = x;
	m_posWarp.y = y;

	sys_log(0, "WarpSet %s %d %d current map %d target map %d", GetName(), x, y, GetMapIndex(), lMapIndex);

	TPacketGCWarp p;

	p.bHeader	= HEADER_GC_WARP;
	p.lX	= x;
	p.lY	= y;
	p.lAddr	= lAddr;

#ifdef ENABLE_PROXY_IP
	if (!g_stProxyIP.empty())
		p.lAddr = inet_addr(g_stProxyIP.c_str());
#endif

	p.wPort	= wPort;

#ifdef ENABLE_RENEWAL_SWITCHBOT
	DWORD dwPID = GetPlayerID();
	CSwitchbotManager::Instance().SetIsWarping(dwPID, true);

	if (CSwitchbotManager::Instance().CanFindSwitchbot(dwPID))
		if (p.wPort != mother_port)
			CSwitchbotManager::Instance().P2PSendSwitchbot(dwPID, p.wPort);
#endif

	GetDesc()->Packet(&p, sizeof(TPacketGCWarp));

	char buf[256];
	snprintf(buf, sizeof(buf), "%s MapIdx %ld DestMapIdx%ld DestX%ld DestY%ld Empire%d", GetName(), GetMapIndex(), lPrivateMapIndex, x, y, GetEmpire());
	LogManager::instance().CharLog(this, 0, "WARP", buf);

	return true;
}

void CHARACTER::WarpEnd()
{
	if (test_server)
		sys_log(0, "WarpEnd %s", GetName());

	if (m_posWarp.x == 0 && m_posWarp.y == 0)
		return;

	int index = m_lWarpMapIndex;

	if (index > 10000)
		index /= 10000;

	if (!map_allow_find(index))
	{
		sys_err("location %d %d not allowed to login this server", m_posWarp.x, m_posWarp.y);
#ifdef ENABLE_GOHOME_IFNOMAP
		GoHome();
#else
		GetDesc()->SetPhase(PHASE_CLOSE);
#endif
		return;
	}

	sys_log(0, "WarpEnd %s %d %u %u", GetName(), m_lWarpMapIndex, m_posWarp.x, m_posWarp.y);

	Show(m_lWarpMapIndex, m_posWarp.x, m_posWarp.y, 0);
	Stop();

	m_lWarpMapIndex = 0;
	m_posWarp.x = m_posWarp.y = m_posWarp.z = 0;

	{
		// P2P Login
		TPacketGGLogin p;
		p.bHeader = HEADER_GG_LOGIN;
		strlcpy(p.szName, GetName(), sizeof(p.szName));
		p.dwPID = GetPlayerID();
		p.bEmpire = GetEmpire();
		p.lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(GetX(), GetY());
		p.bChannel = g_bChannel;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		p.bLanguage = GetLanguage();
#endif

		P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGLogin));
	}
}

bool CHARACTER::Return()
{
	if (!IsNPC())
		return false;

	int x, y;
	SetVictim(NULL);

	x = m_pkMobInst->m_posLastAttacked.x;
	y = m_pkMobInst->m_posLastAttacked.y;

	SetRotationToXY(x, y);

	if (!Goto(x, y))
		return false;

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);

	if (test_server)
		sys_log(0, "%s %p Let's give up and go back! %d %d", GetName(), this, x, y);

	if (GetParty())
		GetParty()->SendMessage(this, PM_RETURN, x, y);

	return true;
}

bool CHARACTER::Follow(LPCHARACTER pkChr, float fMinDistance)
{
	if (IsPC()
#ifdef ENABLE_BOT_PLAYER
		|| IsBotCharacter()
#endif
	)
	{
		sys_err("CHARACTER::Follow : PC cannot use this method", GetName());
		return false;
	}

	// TRENT_MONSTER
	if (IS_SET(m_pointsInstant.dwAIFlag, AIFLAG_NOMOVE))
	{
		if (pkChr->IsPC()) // When the person you are following is a PC
		{
			// If i'm in a party. I must obey party leader's AI.
			if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
			{
				if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000) // 15 seconds have passed since the last attack
				{
					// If it is more than 50 meters from the last hit, give up and go back.
					if (m_pkMobData->m_table.wAttackRange < DISTANCE_APPROX(pkChr->GetX() - GetX(), pkChr->GetY() - GetY()))
						if (Return())
							return true;
				}
			}
		}
		return false;
	}
	// END_OF_TRENT_MONSTER

	long x = pkChr->GetX();
	long y = pkChr->GetY();

	if (pkChr->IsPC()) // When the person you are following is a PC
	{
		// If i'm in a party. I must obey party leader's AI.
		if (!GetParty() || !GetParty()->GetLeader() || GetParty()->GetLeader() == this)
		{
			if (get_dword_time() - m_pkMobInst->m_dwLastAttackedTime >= 15000) // 15 seconds have passed since the last attack
			{
				// If it is more than 50 meters from the last hit, give up and go back.
				if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
					if (Return())
						return true;
			}
		}
	}

#ifdef ENABLE_GUARDIAN_MOB_AGGRO_FIX
	if (IsGuardNPC())
	{
		if (5000 < DISTANCE_APPROX(m_pkMobInst->m_posLastAttacked.x - GetX(), m_pkMobInst->m_posLastAttacked.y - GetY()))
			if (Return())
				return true;
	}
#endif

	if (pkChr->IsState(pkChr->m_stateMove) && 
		GetMobBattleType() != BATTLE_TYPE_RANGE && 
		GetMobBattleType() != BATTLE_TYPE_MAGIC &&
		false == IsPet()
#ifdef ENABLE_GROWTH_PET_SYSTEM
		&& false == IsGrowthPet()
#endif
		)
	{
		// If the target is moving, predictive movement
		// After predicting the time to meet from the speed difference and distance between me and the other party
		// Assume that the opponent moves in a straight line up to that time and moves there.
		float rot = pkChr->GetRotation();
		float rot_delta = GetDegreeDelta(rot, GetDegreeFromPositionXY(GetX(), GetY(), pkChr->GetX(), pkChr->GetY()));

		float yourSpeed = pkChr->GetMoveSpeed();
		float mySpeed = GetMoveSpeed();

		float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());
		float fFollowSpeed = mySpeed - yourSpeed * cos(rot_delta * M_PI / 180);

		if (fFollowSpeed >= 0.1f)
		{
			float fMeetTime = fDist / fFollowSpeed;
			float fYourMoveEstimateX, fYourMoveEstimateY;

			if( fMeetTime * yourSpeed <= 100000.0f )
			{
				GetDeltaByDegree(pkChr->GetRotation(), fMeetTime * yourSpeed, &fYourMoveEstimateX, &fYourMoveEstimateY);

				x += (long) fYourMoveEstimateX;
				y += (long) fYourMoveEstimateY;

				float fDistNew = sqrt(((double)x - GetX())*(x-GetX())+((double)y - GetY())*(y-GetY()));
				if (fDist < fDistNew)
				{
					x = (long)(GetX() + (x - GetX()) * fDist / fDistNew);
					y = (long)(GetY() + (y - GetY()) * fDist / fDistNew);
				}
			}
		}
	}

	// You have to look at where you want to go.
	SetRotationToXY(x, y);

	float fDist = DISTANCE_SQRT(x - GetX(), y - GetY());

	if (fDist <= fMinDistance)
		return false;

	float fx, fy;

	if (IsChangeAttackPosition(pkChr) && GetMobRank() < MOB_RANK_BOSS)
	{
		// Move to a random place around the opponent
		SetChangeAttackPositionTime();

		int retry = 16;
		int dx, dy;
		int rot = (int) GetDegreeFromPositionXY(x, y, GetX(), GetY());

		while (--retry)
		{
			if (fDist < 500.0f)
				GetDeltaByDegree((rot + number(-90, 90) + number(-90, 90)) % 360, fMinDistance, &fx, &fy);
			else
				GetDeltaByDegree(number(0, 359), fMinDistance, &fx, &fy);

			dx = x + (int) fx;
			dy = y + (int) fy;

			LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), dx, dy);

			if (NULL == tree)
				break;

			if (0 == (tree->GetAttribute(dx, dy) & (ATTR_BLOCK | ATTR_OBJECT)))
				break;
		}

		if (!Goto(dx, dy))
			return false;
	}
	else
	{
		// Follow a straight line
		float fDistToGo = fDist - fMinDistance;
		GetDeltaByDegree(GetRotation(), fDistToGo, &fx, &fy);

		if (!Goto(GetX() + (int) fx, GetY() + (int) fy))
			return false;
	}

	SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	//MonsterLog("Follow; %s", pkChr->GetName());
	return true;
}

float CHARACTER::GetDistanceFromSafeboxOpen() const
{
	return DISTANCE_APPROX(GetX() - m_posSafeboxOpen.x, GetY() - m_posSafeboxOpen.y);
}

void CHARACTER::SetSafeboxOpenPosition()
{
	m_posSafeboxOpen = GetXYZ();
}

CSafebox * CHARACTER::GetSafebox() const
{
	return m_pkSafebox;
}

void CHARACTER::ReqSafeboxLoad(const char* pszPassword)
{
#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (GetPetWindowType() == PET_WINDOW_ATTR_CHANGE || GetPetWindowType() == PET_WINDOW_PRIMIUM_FEEDSTUFF)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 738, "");
		return;
	}
#endif

	if (!*pszPassword || strlen(pszPassword) > SAFEBOX_PASSWORD_MAX_LEN)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 48, "");
		return;
	}
	else if (m_pkSafebox)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 49, "");
		return;
	}

	int iPulse = thecore_pulse();

	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(10))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 50, "");
		return;
	}
	else if (m_bOpeningSafebox)
	{
		sys_log(0, "Overlapped safebox load request from %s", GetName());
		return;
	}

	SetSafeboxLoadTime();
	m_bOpeningSafebox = true;

	TSafeboxLoadPacket p;
	p.dwID = GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, pszPassword, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_LOAD, GetDesc()->GetHandle(), &p, sizeof(p));
}

#ifdef ENABLE_SPECIAL_INVENTORY
EVENTFUNC(open_safebox_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("open_safebox_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL)
	{
		return 0;
	}

	int iPulse = thecore_pulse();
	if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(10))
		return PASSES_PER_SEC(1);

	if ((ch->GetExchange() || ch->GetShopOwner()) || ch->IsCubeOpen() || ch->GetMall())
	{
		return 0;
	}

	ch->m_pkOpenSafeboxEvent = NULL;
	ch->SafeboxLoad(true);
	return 0;
}

EVENTFUNC(open_mall_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("open_mall_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL)
	{
		return 0;
	}

	int iPulse = thecore_pulse();
	if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(10))
		return PASSES_PER_SEC(1);

	ch->m_pkOpenMallEvent = NULL;
	ch->MallOpen(true);
	return 0;
}

void CHARACTER::SafeboxLoad(bool forceOpen)
{
	if (GetMapIndex() >= 10000 || g_bChannel == 99)
	{
		ChatPacket(CHAT_TYPE_COMMAND, "CloseLoadSafeboxEvent");
		LocaleChatPacket(CHAT_TYPE_INFO, 51, "");
		return;
	}

	if ((GetExchange() || GetShopOwner()) || IsCubeOpen() || GetMall())
	{
		ChatPacket(CHAT_TYPE_COMMAND, "CloseLoadSafeboxEvent");
		LocaleChatPacket(CHAT_TYPE_INFO, 52, "");
		return;
	}

	int iPulse = thecore_pulse();

	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(10) && !forceOpen)
	{
		if (m_pkOpenSafeboxEvent)
			return;

		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = this;
		m_pkOpenSafeboxEvent = event_create(open_safebox_event, info, PASSES_PER_SEC(1));
		return;
	}
	else if (m_bOpeningSafebox)
	{
		ChatPacket(CHAT_TYPE_COMMAND, "CloseLoadSafeboxEvent");
		sys_log(0, "Overlapped safebox load request from %s", GetName());
		return;
	}

	SetSafeboxLoadTime();
	m_bOpeningSafebox = true;

	TSafeboxLoadPacket p;
	p.dwID = GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, GetDesc()->GetAccountTable().login, sizeof(p.szLogin));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_LOAD, GetDesc()->GetHandle(), &p, sizeof(p));
}

void CHARACTER::MallOpen(bool forceOpen)
{
	if (GetMapIndex() >= 10000 || g_bChannel == 99)
	{
		ChatPacket(CHAT_TYPE_COMMAND, "CloseLoadMallEvent");
		LocaleChatPacket(CHAT_TYPE_INFO, 53, "");
		return;
	}

	if (GetMall())
	{
		ChatPacket(CHAT_TYPE_COMMAND, "CloseLoadMallEvent");
		LocaleChatPacket(CHAT_TYPE_INFO, 52, "");
		return;
	}

	int iPulse = thecore_pulse();

	if (iPulse - GetMallLoadTime() < passes_per_sec * 10 && !forceOpen)
	{
		if (m_pkOpenMallEvent)
			return;

		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = this;
		m_pkOpenMallEvent = event_create(open_mall_event, info, PASSES_PER_SEC(1));
		return;
	}

	SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, GetDesc()->GetAccountTable().login, sizeof(p.szLogin));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, GetDesc()->GetHandle(), &p, sizeof(p));
}
#endif

void CHARACTER::LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(true);
	//END_PREVENT_TRADE_WINDOW

	if (m_pkSafebox)
		bLoaded = true;

	if (!m_pkSafebox)
		m_pkSafebox = M2_NEW CSafebox(this, iSize, dwGold);
	else
		m_pkSafebox->ChangeSize(iSize);

	m_iSafeboxSize = iSize;

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = iSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkSafebox->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			item->SetTransmutationVnum(pItems->dwTransmutationVnum);
#endif

			// Fix
			if (!m_pkSafebox->Add(pItems->pos, item))
			{
				M2_DESTROY_ITEM(item);
			}
			else
			{
				item->OnAfterCreatedItem();
				item->SetSkipSave(false);
			}
		}
	}
}

void CHARACTER::ChangeSafeboxSize(BYTE bSize)
{
	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_SAFEBOX_SIZE;
	p.bSize = bSize;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (m_pkSafebox)
		m_pkSafebox->ChangeSize(bSize);

	m_iSafeboxSize = bSize;
}

void CHARACTER::CloseSafebox()
{
#ifdef ENABLE_SPECIAL_INVENTORY
	event_cancel(&m_pkOpenSafeboxEvent);
#endif

	if (!m_pkSafebox)
		return;

	//PREVENT_TRADE_WINDOW
	SetOpenSafebox(false);
	//END_PREVENT_TRADE_WINDOW

	m_pkSafebox->Save();

	M2_DELETE(m_pkSafebox);
	m_pkSafebox = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseSafebox");

	SetSafeboxLoadTime();
	m_bOpeningSafebox = false;

	Save();
}

CSafebox * CHARACTER::GetMall() const
{
	return m_pkMall;
}

void CHARACTER::LoadMall(int iItemCount, TPlayerItem * pItems)
{
	bool bLoaded = false;

	//if (m_pkMall)//i think here...
	//	bLoaded = true;

	if (!m_pkMall)
		m_pkMall = M2_NEW CSafebox(this, 3 * SAFEBOX_PAGE_SIZE, 0);
	else
		m_pkMall->ChangeSize(3 * SAFEBOX_PAGE_SIZE);

	m_pkMall->SetWindowMode(MALL);

	TPacketCGSafeboxSize p;

	p.bHeader = HEADER_GC_MALL_OPEN;
	p.bSize = 3 * SAFEBOX_PAGE_SIZE;

	GetDesc()->Packet(&p, sizeof(TPacketCGSafeboxSize));

	if (!bLoaded)
	{
		for (int i = 0; i < iItemCount; ++i, ++pItems)
		{
			if (!m_pkMall->IsValidPosition(pItems->pos))
				continue;

			LPITEM item = ITEM_MANAGER::instance().CreateItem(pItems->vnum, pItems->count, pItems->id);

			if (!item)
			{
				sys_err("cannot create item vnum %d id %u (name: %s)", pItems->vnum, pItems->id, GetName());
				continue;
			}

			item->SetSkipSave(true);
			item->SetSockets(pItems->alSockets);
			item->SetAttributes(pItems->aAttr);
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			item->SetTransmutationVnum(pItems->dwTransmutationVnum);
#endif

			if (!m_pkMall->Add(pItems->pos, item))
				M2_DESTROY_ITEM(item);
			else
				item->SetSkipSave(false);
		}
	}
}

void CHARACTER::CloseMall()
{
#ifdef ENABLE_SPECIAL_INVENTORY
	event_cancel(&m_pkOpenMallEvent);
#endif

	if (!m_pkMall)
		return;

	m_pkMall->Save();

	M2_DELETE(m_pkMall);
	m_pkMall = NULL;

	ChatPacket(CHAT_TYPE_COMMAND, "CloseMall");
}

bool CHARACTER::BuildUpdatePartyPacket(TPacketGCPartyUpdate & out)
{
	if (!GetParty())
		return false;

	memset(&out, 0, sizeof(out));

	out.header		= HEADER_GC_PARTY_UPDATE;
	out.pid		= GetPlayerID();
	if (GetMaxHP() <= 0) // @fixme136
		out.percent_hp	= 0;
	else
		out.percent_hp	= MINMAX(0, GetHP() * 100 / GetMaxHP(), 100);
	out.role		= GetParty()->GetRole(GetPlayerID());

	sys_log(1, "PARTY %s role is %d", GetName(), out.role);

	LPCHARACTER l = GetParty()->GetLeaderCharacter();

	if (l && DISTANCE_APPROX(GetX() - l->GetX(), GetY() - l->GetY()) < PARTY_DEFAULT_RANGE)
	{
		out.affects[0] = GetParty()->GetPartyBonusExpPercent();
		out.affects[1] = GetPoint(POINT_PARTY_ATTACKER_BONUS);
		out.affects[2] = GetPoint(POINT_PARTY_TANKER_BONUS);
		out.affects[3] = GetPoint(POINT_PARTY_BUFFER_BONUS);
		out.affects[4] = GetPoint(POINT_PARTY_SKILL_MASTER_BONUS);
		out.affects[5] = GetPoint(POINT_PARTY_HASTE_BONUS);
		out.affects[6] = GetPoint(POINT_PARTY_DEFENDER_BONUS);
	}

	return true;
}

int CHARACTER::GetLeadershipSkillLevel() const
{
	return GetSkillLevel(SKILL_LEADERSHIP);
}

void CHARACTER::QuerySafeboxSize()
{
	if (m_iSafeboxSize == -1)
	{
		DBManager::instance().ReturnQuery(QID_SAFEBOX_SIZE,
				GetPlayerID(),
				NULL,
				"SELECT size FROM safebox%s WHERE account_id = %u",
				get_table_postfix(),
				GetDesc()->GetAccountTable().id);
	}
}

void CHARACTER::SetSafeboxSize(int iSize)
{
	sys_log(1, "SetSafeboxSize: %s %d", GetName(), iSize);
	m_iSafeboxSize = iSize;
	DBManager::instance().Query("UPDATE safebox%s SET size = %d WHERE account_id = %u", get_table_postfix(), iSize / SAFEBOX_PAGE_SIZE, GetDesc()->GetAccountTable().id);
}

int CHARACTER::GetSafeboxSize() const
{
	return m_iSafeboxSize;
}

void CHARACTER::SetNowWalking(bool bWalkFlag)
{
	//if (m_bNowWalking != bWalkFlag || IsNPC())
	if (m_bNowWalking != bWalkFlag)
	{
		if (bWalkFlag)
		{
			m_bNowWalking = true;
			m_dwWalkStartTime = get_dword_time();
		}
		else
		{
			m_bNowWalking = false;
		}

		//if (m_bNowWalking)
		{
			TPacketGCWalkMode p;
			p.vid = GetVID();
			p.header = HEADER_GC_WALK_MODE;
			p.mode = m_bNowWalking ? WALKMODE_WALK : WALKMODE_RUN;

			PacketView(&p, sizeof(p));
		}

		if (IsNPC())
		{
			if (m_bNowWalking)
				MonsterLog("°È´Â´Ù");
			else
				MonsterLog("¶Ú´Ù");
		}
	}
}

void CHARACTER::StartStaminaConsume()
{
	if (m_bStaminaConsume)
		return;

	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = true;

	if (IsStaminaHalfConsume())
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec / 2, GetStamina());
	else
		ChatPacket(CHAT_TYPE_COMMAND, "StartStaminaConsume %d %d", STAMINA_PER_STEP * passes_per_sec, GetStamina());
}

void CHARACTER::StopStaminaConsume()
{
	if (!m_bStaminaConsume)
		return;
	PointChange(POINT_STAMINA, 0);
	m_bStaminaConsume = false;
	ChatPacket(CHAT_TYPE_COMMAND, "StopStaminaConsume %d", GetStamina());
}

bool CHARACTER::IsStaminaConsume() const
{
	return m_bStaminaConsume;
}

bool CHARACTER::IsStaminaHalfConsume() const
{
	return IsEquipUniqueItem(UNIQUE_ITEM_HALF_STAMINA);
}

void CHARACTER::ResetStopTime()
{
	m_dwStopTime = get_dword_time();
}

DWORD CHARACTER::GetStopTime() const
{
	return m_dwStopTime;
}

void CHARACTER::ResetPoint(int iLv)
{
	BYTE bJob = GetJob();

	// Fix
	PointChange(POINT_LEVEL, iLv - GetLevel(), false, true);

	SetRealPoint(POINT_ST, JobInitialPoints[bJob].st);
	SetPoint(POINT_ST, GetRealPoint(POINT_ST));

	SetRealPoint(POINT_HT, JobInitialPoints[bJob].ht);
	SetPoint(POINT_HT, GetRealPoint(POINT_HT));

	SetRealPoint(POINT_DX, JobInitialPoints[bJob].dx);
	SetPoint(POINT_DX, GetRealPoint(POINT_DX));

	SetRealPoint(POINT_IQ, JobInitialPoints[bJob].iq);
	SetPoint(POINT_IQ, GetRealPoint(POINT_IQ));

	SetRandomHP((iLv - 1) * number(JobInitialPoints[GetJob()].hp_per_lv_begin, JobInitialPoints[GetJob()].hp_per_lv_end));
	SetRandomSP((iLv - 1) * number(JobInitialPoints[GetJob()].sp_per_lv_begin, JobInitialPoints[GetJob()].sp_per_lv_end));

	PointChange(POINT_STAT, ((MINMAX(1, iLv, 90) - 1) * 3) + GetPoint(POINT_LEVEL_STEP) - GetPoint(POINT_STAT));

	ComputePoints();

	PointChange(POINT_HP, GetMaxHP() - GetHP());
	PointChange(POINT_SP, GetMaxSP() - GetSP());

	PointsPacket();

	LogManager::instance().CharLog(this, 0, "RESET_POINT", "");
}

bool CHARACTER::IsChangeAttackPosition(LPCHARACTER target) const
{
	if (!IsNPC())
		return true;

	DWORD dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_NEAR;

	if (DISTANCE_APPROX(GetX() - target->GetX(), GetY() - target->GetY()) >
		AI_CHANGE_ATTACK_POISITION_DISTANCE + GetMobAttackRange())
		dwChangeTime = AI_CHANGE_ATTACK_POISITION_TIME_FAR;

	return get_dword_time() - m_dwLastChangeAttackPositionTime > dwChangeTime;
}

void CHARACTER::GiveRandomSkillBook()
{
	LPITEM item = AutoGiveItem(50300);

	if (NULL != item)
	{
		BYTE bJob = 0;

		// 50% of getting random books or getting one of the same player's race
		if (!number(0, 1))
			bJob = GetJob() + 1;

		DWORD dwSkillVnum = 0;

		do
		{
			dwSkillVnum = number(1, 111);
			const CSkillProto* pkSk = CSkillManager::instance().Get(dwSkillVnum);

			if (NULL == pkSk)
				continue;

			if (bJob && bJob != pkSk->dwType)
				continue;

			break;
		} while (true);

		item->SetSocket(0, dwSkillVnum);
	}
}

struct RemoveInvisibleVictim
{
	RemoveInvisibleVictim(LPCHARACTER pkOldVictim)
	{
		m_pkOldVictim = pkOldVictim;
	}
	void operator () (LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER pkChr = (LPCHARACTER) ent;
			if (pkChr && pkChr->IsMonster())
			{
				LPCHARACTER pkVictim = pkChr->GetVictim();
				if (pkVictim && pkVictim == m_pkOldVictim)
				{
					LPCHARACTER new_victim = pkChr->GetNewNearestVictim(pkChr, m_pkOldVictim);
					pkChr->SetVictim(new_victim);
				}
			}
		}
	}
	LPCHARACTER m_pkOldVictim;
};

void CHARACTER::ReviveInvisible(int iDur)
{
	if (GetSectree())
	{
		RemoveInvisibleVictim f(this);
		GetSectree()->ForEachAround(f);
	}
	AddAffect(AFFECT_REVIVE_INVISIBLE, POINT_NONE, 0, AFF_REVIVE_INVISIBLE, iDur, 0, true);
}

void CHARACTER::ToggleMonsterLog()
{
	m_bMonsterLog = !m_bMonsterLog;

	if (m_bMonsterLog)
	{
		CHARACTER_MANAGER::instance().RegisterForMonsterLog(this);
	}
	else
	{
		CHARACTER_MANAGER::instance().UnregisterForMonsterLog(this);
	}
}

void CHARACTER::SetGuild(CGuild* pGuild)
{
	if (m_pGuild != pGuild)
	{
		m_pGuild = pGuild;
		UpdatePacket();
	}
}

void CHARACTER::SendGreetMessage()
{
	__typeof(DBManager::instance().GetGreetMessage()) v = DBManager::instance().GetGreetMessage();

	for (itertype(v) it = v.begin(); it != v.end(); ++it)
	{
		ChatPacket(CHAT_TYPE_NOTICE, it->c_str());
	}
}

void CHARACTER::BeginStateEmpty()
{
	MonsterLog("!");
}

void CHARACTER::EffectPacket(int enumEffectType)
{
	TPacketGCSpecialEffect p;

	p.header = HEADER_GC_SEPCIAL_EFFECT;
	p.type = enumEffectType;
	p.vid = GetVID();

	PacketAround(&p, sizeof(TPacketGCSpecialEffect));
}

void CHARACTER::SpecificEffectPacket(const char filename[MAX_EFFECT_FILE_NAME])
{
	TPacketGCSpecificEffect p;

	p.header = HEADER_GC_SPECIFIC_EFFECT;
	p.vid = GetVID();
	memcpy (p.effect_file, filename, MAX_EFFECT_FILE_NAME);

	PacketAround(&p, sizeof(TPacketGCSpecificEffect));
}

void CHARACTER::MonsterChat(BYTE bMonsterChatType)
{
	if (IsPC())
		return;

	char sbuf[256+1];

	if (IsMonster())
	{
		if (number(0, 60))
			return;

		snprintf(sbuf, sizeof(sbuf),
				"(locale.monster_chat[%i] and locale.monster_chat[%i][%d] or '')",
				GetRaceNum(), GetRaceNum(), bMonsterChatType*3 + number(1, 3));
	}
	else
	{
		if (bMonsterChatType != MONSTER_CHAT_WAIT)
			return;

		if (IsGuardNPC())
		{
			if (number(0, 6))
				return;
		}
		else
		{
			if (number(0, 30))
				return;
		}

		snprintf(sbuf, sizeof(sbuf), "(locale.monster_chat[%i] and locale.monster_chat[%i][number(1, table.getn(locale.monster_chat[%i]))] or '')", GetRaceNum(), GetRaceNum(), GetRaceNum());
	}

	std::string text = quest::ScriptToString(sbuf);

	if (text.empty())
		return;

	struct packet_chat pack_chat;

	pack_chat.header    = HEADER_GC_CHAT;
	pack_chat.size	= sizeof(struct packet_chat) + text.size() + 1;
	pack_chat.type      = CHAT_TYPE_TALKING;
	pack_chat.id        = GetVID();
	pack_chat.bEmpire	= 0;

	TEMP_BUFFER buf;
	buf.write(&pack_chat, sizeof(struct packet_chat));
	buf.write(text.c_str(), text.size() + 1);

	PacketAround(buf.read_peek(), buf.size());
}

void CHARACTER::SetQuestNPCID(DWORD vid)
{
	m_dwQuestNPCVID = vid;
}

LPCHARACTER CHARACTER::GetQuestNPC() const
{
	return CHARACTER_MANAGER::instance().Find(m_dwQuestNPCVID);
}

void CHARACTER::SetQuestItemPtr(LPITEM item)
{
	m_dwQuestItemVID = (item) ? item->GetVID() : 0;
}

void CHARACTER::ClearQuestItemPtr()
{
	m_dwQuestItemVID = 0;
}

LPITEM CHARACTER::GetQuestItemPtr() const
{
	if (!m_dwQuestItemVID)
		return nullptr;

	return ITEM_MANAGER::Instance().FindByVID(m_dwQuestItemVID);
}

LPDUNGEON CHARACTER::GetDungeonForce() const
{
	if (m_lWarpMapIndex > 10000)
		return CDungeonManager::instance().FindByMapIndex(m_lWarpMapIndex);

	return m_pkDungeon;
}

void CHARACTER::SetBlockMode(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;

	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);

	SetQuestFlag("game_option.block_exchange", bFlag & BLOCK_EXCHANGE ? 1 : 0);
	SetQuestFlag("game_option.block_party_invite", bFlag & BLOCK_PARTY_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_guild_invite", bFlag & BLOCK_GUILD_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_whisper", bFlag & BLOCK_WHISPER ? 1 : 0);
	SetQuestFlag("game_option.block_messenger_invite", bFlag & BLOCK_MESSENGER_INVITE ? 1 : 0);
	SetQuestFlag("game_option.block_party_request", bFlag & BLOCK_PARTY_REQUEST ? 1 : 0);
}

void CHARACTER::SetBlockModeForce(BYTE bFlag)
{
	m_pointsInstant.bBlockMode = bFlag;
	ChatPacket(CHAT_TYPE_COMMAND, "setblockmode %d", m_pointsInstant.bBlockMode);
}

bool CHARACTER::IsGuardNPC() const
{
	return IsNPC() && (GetRaceNum() == 11000 || GetRaceNum() == 11002 || GetRaceNum() == 11004);
}

int CHARACTER::GetPolymorphPower() const
{
	if (test_server)
	{
		int value = quest::CQuestManager::instance().GetEventFlag("poly");
		if (value)
			return value;
	}
	return aiPolymorphPowerByLevel[MINMAX(0, GetSkillLevel(SKILL_POLYMORPH), 40)];
}

void CHARACTER::SetPolymorph(DWORD dwRaceNum, bool bMaintainStat)
{
	if (dwRaceNum < JOB_MAX_NUM)
	{
		dwRaceNum = 0;
		bMaintainStat = false;
	}

	if (m_dwPolymorphRace == dwRaceNum)
		return;

	m_bPolyMaintainStat = bMaintainStat;
	m_dwPolymorphRace = dwRaceNum;

	sys_log(0, "POLYMORPH: %s race %u ", GetName(), dwRaceNum);

	if (dwRaceNum != 0)
		StopRiding();

	SET_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);
	m_afAffectFlag.Set(AFF_SPAWN);

	ViewReencode();

	REMOVE_BIT(m_bAddChrState, ADD_CHARACTER_STATE_SPAWN);

	if (!bMaintainStat)
	{
		PointChange(POINT_ST, 0);
		PointChange(POINT_DX, 0);
		PointChange(POINT_IQ, 0);
		PointChange(POINT_HT, 0);
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputeBattlePoints();
}

#ifdef ENABLE_QUEST_PC_GETFLAG_CRASH_FIX
int CHARACTER::GetQuestFlag(const std::string& flag) const
{
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(GetPlayerID());

	if(!pPC)
	{
		sys_err("Nullpointer in CHARACTER::GetQuestFlag %lu", GetPlayerID());
		return 0;
	}

	return pPC->GetFlag(flag);
}
#else
int CHARACTER::GetQuestFlag(const std::string& flag) const
{
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(GetPlayerID());
	return pPC->GetFlag(flag);
}
#endif

void CHARACTER::SetQuestFlag(const std::string& flag, int value)
{
	quest::CQuestManager& q = quest::CQuestManager::instance();
	quest::PC* pPC = q.GetPC(GetPlayerID());
	if(!pPC) {
        return;
    } 

	pPC->SetFlag(flag, value);
}

void CHARACTER::SendEquipment(LPCHARACTER ch)
{
	TPacketViewEquip p;
	p.header = HEADER_GC_VIEW_EQUIP;
	p.vid    = GetVID();
	for (int i = 0; i<WEAR_MAX_NUM; i++)
	{
		LPITEM item = GetWear(i);
		if (item)
		{
			p.equips[i].vnum = item->GetVnum();
			p.equips[i].count = item->GetCount();

			thecore_memcpy(p.equips[i].alSockets, item->GetSockets(), sizeof(p.equips[i].alSockets));
			thecore_memcpy(p.equips[i].aAttr, item->GetAttributes(), sizeof(p.equips[i].aAttr));
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			p.equips[i].dwTransmutationVnum = item->GetTransmutationVnum();
#endif
		}
		else
		{
			p.equips[i].vnum = 0;
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			p.equips[i].dwTransmutationVnum = 0;
#endif
		}
	}
	ch->GetDesc()->Packet(&p, sizeof(p));
}

bool CHARACTER::CanSummon(int iLeaderShip)
{
	return (iLeaderShip >= 20 || iLeaderShip >= 12 && m_dwLastDeadTime + 180 > get_dword_time());
}


void CHARACTER::MountVnum(DWORD vnum)
{
	if (m_dwMountVnum == vnum)
		return;
	if ((m_dwMountVnum != 0)&&(vnum!=0)) //@fixme108 set recursively to 0 for eventuality
		MountVnum(0);

	m_dwMountVnum = vnum;
	m_dwMountTime = get_dword_time();

	if (m_bIsObserver)
		return;

	m_posDest.x = m_posStart.x = GetX();
	m_posDest.y = m_posStart.y = GetY();

	EncodeInsertPacket(this);

	ENTITY_MAP::iterator it = m_map_view.begin();

	while (it != m_map_view.end())
	{
		LPENTITY entity = (it++)->first;
		EncodeInsertPacket(entity);
	}

	SetValidComboInterval(0);
	SetComboSequence(0);

	ComputePoints();
}

namespace {
	class FuncCheckWarp
	{
		public:
			FuncCheckWarp(LPCHARACTER pkWarp)
			{
				m_lTargetY = 0;
				m_lTargetX = 0;

				m_lX = pkWarp->GetX();
				m_lY = pkWarp->GetY();

				m_bInvalid = false;
				m_bEmpire = pkWarp->GetEmpire();

				char szTmp[64];

				if (3 != sscanf(pkWarp->GetName(), " %s %ld %ld ", szTmp, &m_lTargetX, &m_lTargetY))
				{
					if (number(1, 100) < 5)
						sys_err("Warp NPC name wrong : vnum(%d) name(%s)", pkWarp->GetRaceNum(), pkWarp->GetName());

					m_bInvalid = true;

					return;
				}

				m_lTargetX *= 100;
				m_lTargetY *= 100;

				m_bUseWarp = true;

				if (pkWarp->IsGoto())
				{
					LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pkWarp->GetMapIndex());
					m_lTargetX += pkSectreeMap->m_setting.iBaseX;
					m_lTargetY += pkSectreeMap->m_setting.iBaseY;
					m_bUseWarp = false;
				}
			}

			bool Valid()
			{
				return !m_bInvalid;
			}

			void operator () (LPENTITY ent)
			{
				if (!Valid())
					return;

				if (!ent->IsType(ENTITY_CHARACTER))
					return;

				LPCHARACTER pkChr = (LPCHARACTER) ent;

				if (!pkChr->IsPC())
					return;

				int iDist = DISTANCE_APPROX(pkChr->GetX() - m_lX, pkChr->GetY() - m_lY);

				if (iDist > 300)
					return;

				if (m_bEmpire && pkChr->GetEmpire() && m_bEmpire != pkChr->GetEmpire())
					return;

				if (pkChr->IsHack())
					return;

				if (!pkChr->CanHandleItem(false, true))
					return;

				if (m_bUseWarp)
					pkChr->WarpSet(m_lTargetX, m_lTargetY);
				else
				{
					pkChr->Show(pkChr->GetMapIndex(), m_lTargetX, m_lTargetY);
					pkChr->Stop();
				}
			}

			bool m_bInvalid;
			bool m_bUseWarp;

			long m_lX;
			long m_lY;
			long m_lTargetX;
			long m_lTargetY;

			BYTE m_bEmpire;
	};
}

EVENTFUNC(warp_npc_event)
{
	char_event_info* info = dynamic_cast<char_event_info*> (event->info);
	if (info == NULL)
	{
		sys_err( "warp_npc_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL)
	{
		return 0;
	}

	if (!ch->GetSectree())
	{
		ch->m_pkWarpNPCEvent = NULL;
		return 0;
	}

	FuncCheckWarp f(ch);
	if (f.Valid())
		ch->GetSectree()->ForEachAround(f);

	return passes_per_sec / 2;
}

void CHARACTER::StartWarpNPCEvent()
{
	if (m_pkWarpNPCEvent)
		return;

	if (!IsWarp() && !IsGoto())
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkWarpNPCEvent = event_create(warp_npc_event, info, passes_per_sec / 2);
}

void CHARACTER::SyncPacket()
{
	TEMP_BUFFER buf;

	TPacketCGSyncPositionElement elem;

	elem.dwVID = GetVID();
	elem.lX = GetX();
	elem.lY = GetY();

	TPacketGCSyncPosition pack;

	pack.bHeader = HEADER_GC_SYNC_POSITION;
	pack.wSize = sizeof(TPacketGCSyncPosition) + sizeof(elem);

	buf.write(&pack, sizeof(pack));
	buf.write(&elem, sizeof(elem));

	PacketAround(buf.read_peek(), buf.size());
}

LPCHARACTER CHARACTER::GetMarryPartner() const
{
	return m_pkChrMarried;
}

void CHARACTER::SetMarryPartner(LPCHARACTER ch)
{
	m_pkChrMarried = ch;
}

int CHARACTER::GetMarriageBonus(DWORD dwItemVnum, bool bSum)
{
	if (IsNPC())
		return 0;

	marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());

	if (!pMarriage)
		return 0;

	return pMarriage->GetBonus(dwItemVnum, bSum, this);
}

void CHARACTER::ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID)
{
	if (!IsPC())
		return;

	TPacketGCQuestConfirm p;

	p.header = HEADER_GC_QUEST_CONFIRM;
	p.requestPID = dwRequestPID;
	p.timeout = iTimeout;
	strlcpy(p.msg, szMsg, sizeof(p.msg));

	GetDesc()->Packet(&p, sizeof(p));
}

int CHARACTER::GetPremiumRemainSeconds(BYTE bType) const
{
	if (bType >= PREMIUM_MAX_NUM)
		return 0;

	return m_aiPremiumTimes[bType] - get_global_time();
}

bool CHARACTER::WarpToPID(DWORD dwPID)
{
	LPCHARACTER victim;
	if ((victim = (CHARACTER_MANAGER::instance().FindByPID(dwPID))))
	{
		int mapIdx = victim->GetMapIndex();
		if (IS_SUMMONABLE_ZONE(mapIdx))
		{
			if (CAN_ENTER_ZONE(this, mapIdx))
			{
				WarpSet(victim->GetX(), victim->GetY());
			}
			else
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 54, "");
				return false;
			}
		}
		else
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 54, "");
			return false;
		}
	}
	else
	{
		CCI * pcci = P2P_MANAGER::instance().FindByPID(dwPID);

		if (!pcci)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 55, "");
			return false;
		}

		if (pcci->bChannel != g_bChannel)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 56, "%d#%d", pcci->bChannel, g_bChannel);
			return false;
		}
		else if (false == IS_SUMMONABLE_ZONE(pcci->lMapIndex))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 54, "");
			return false;
		}
		else
		{
			if (!CAN_ENTER_ZONE(this, pcci->lMapIndex))
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 54, "");
				return false;
			}

			TPacketGGFindPosition p;
			p.header = HEADER_GG_FIND_POSITION;
			p.dwFromPID = GetPlayerID();
			p.dwTargetPID = dwPID;
			pcci->pkDesc->Packet(&p, sizeof(TPacketGGFindPosition));

			if (test_server)
				ChatPacket(CHAT_TYPE_PARTY, "sent find position packet for teleport");
		}
	}
	return true;
}

// ADD_REFINE_BUILDING
CGuild* CHARACTER::GetRefineGuild() const
{
	LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);

	return (chRefineNPC ? chRefineNPC->GetGuild() : NULL);
}

bool CHARACTER::IsRefineThroughGuild() const
{
	return GetRefineGuild() != NULL;
}

int CHARACTER::ComputeRefineFee(int iCost, int iMultiply) const
{
	CGuild* pGuild = GetRefineGuild();
	if (pGuild)
	{
		if (pGuild == GetGuild())
			return iCost * iMultiply * 9 / 10;

		LPCHARACTER chRefineNPC = CHARACTER_MANAGER::instance().Find(m_dwRefineNPCVID);
		if (chRefineNPC && chRefineNPC->GetEmpire() != GetEmpire())
			return iCost * iMultiply * 3;

		return iCost * iMultiply;
	}
	else
		return iCost;
}

void CHARACTER::PayRefineFee(int iTotalMoney)
{
	int iFee = iTotalMoney / 10;
	CGuild* pGuild = GetRefineGuild();

	int iRemain = iTotalMoney;

	if (pGuild)
	{
		if (pGuild != GetGuild())
		{
			pGuild->RequestDepositMoney(this, iFee);
			iRemain -= iFee;
		}
	}

	PointChange(POINT_GOLD, -iRemain);
}
// END_OF_ADD_REFINE_BUILDING

bool CHARACTER::IsHack(bool bSendMsg, bool bCheckShopOwner, int limittime)
{
	const int iPulse = thecore_pulse();

	if (test_server)
		bSendMsg = true;

	if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			LocaleChatPacket(CHAT_TYPE_INFO, 57, "%d", limittime);

		if (test_server)
			ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(limittime));
		return true;
	}

	if (bCheckShopOwner)
	{
		if (GetExchange()
			 || GetMyShop()
			 || GetShopOwner()
			 || IsOpenSafebox()
			 || IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			 || IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			 || GetTransmutation()
#endif
			)
		{
			if (bSendMsg)
				LocaleChatPacket(CHAT_TYPE_INFO, 58, "");

			return true;
		}
	}
	else
	{
		if (GetExchange()
			 || GetMyShop()
			 || IsOpenSafebox()
			 || IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			 || IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			 || GetTransmutation()
#endif
			)
		{
			if (bSendMsg)
				LocaleChatPacket(CHAT_TYPE_INFO, 58, "");

			return true;
		}
	}

	if (iPulse - GetExchangeTime()  < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			LocaleChatPacket(CHAT_TYPE_INFO, 59, "%d", limittime);
		return true;
	}

	if (iPulse - GetMyShopTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			LocaleChatPacket(CHAT_TYPE_INFO, 59, "%d", limittime);
		return true;
	}

	if (iPulse - GetRefineTime() < PASSES_PER_SEC(limittime))
	{
		if (bSendMsg)
			LocaleChatPacket(CHAT_TYPE_INFO, 60, "%d", limittime);
		return true; 
	}

	return false;
}

void CHARACTER::Say(const std::string & s)
{
	struct ::packet_script packet_script;

	packet_script.header = HEADER_GC_SCRIPT;
	packet_script.skin = 1;
	packet_script.src_size = s.size();
	packet_script.size = packet_script.src_size + sizeof(struct packet_script);

	TEMP_BUFFER buf;

	buf.write(&packet_script, sizeof(struct packet_script));
	buf.write(&s[0], s.size());

	if (IsPC())
	{
		GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}

void CHARACTER::UpdateDepositPulse()
{
	m_deposit_pulse = thecore_pulse() + PASSES_PER_SEC(60*5);
}

bool CHARACTER::CanDeposit() const
{
	return (m_deposit_pulse == 0 || (m_deposit_pulse < thecore_pulse()));
}

ESex GET_SEX(LPCHARACTER ch)
{
	switch (ch->GetRaceNum())
	{
		case MAIN_RACE_WARRIOR_M:
		case MAIN_RACE_SURA_M:
		case MAIN_RACE_ASSASSIN_M:
		case MAIN_RACE_SHAMAN_M:
			return SEX_MALE;

		case MAIN_RACE_ASSASSIN_W:
		case MAIN_RACE_SHAMAN_W:
		case MAIN_RACE_WARRIOR_W:
		case MAIN_RACE_SURA_W:
			return SEX_FEMALE;
	}

	/* default sex = male */
	return SEX_MALE;
}

int CHARACTER::GetHPPct() const
{
	return ((int64_t)GetHP() * 100) / (int64_t)GetMaxHP();
}

bool CHARACTER::IsBerserk() const
{
	if (m_pkMobInst != NULL)
		return m_pkMobInst->m_IsBerserk;
	else
		return false;
}

void CHARACTER::SetBerserk(bool mode)
{
	if (m_pkMobInst != NULL)
		m_pkMobInst->m_IsBerserk = mode;
}

bool CHARACTER::IsGodSpeed() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsGodSpeed;
	}
	else
	{
		return false;
	}
}

void CHARACTER::SetGodSpeed(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsGodSpeed = mode;

		if (mode == true)
		{
			SetPoint(POINT_ATT_SPEED, 250);
		}
		else
		{
			SetPoint(POINT_ATT_SPEED, m_pkMobData->m_table.sAttackSpeed);
		}
	}
}

bool CHARACTER::IsDeathBlow() const
{
	if (number(1, 100) <= m_pkMobData->m_table.bDeathBlowPoint)
	{
		return true;
	}
	else
	{
		return false;
	}
}

struct FFindReviver
{
	FFindReviver()
	{
		pChar = NULL;
		HasReviver = false;
	}

	void operator() (LPCHARACTER ch)
	{
		if (ch->IsMonster() != true)
		{
			return;
		}

		if (ch->IsReviver() == true && pChar != ch && ch->IsDead() != true)
		{
			if (number(1, 100) <= ch->GetMobTable().bRevivePoint)
			{
				HasReviver = true;
				pChar = ch;
			}
		}
	}

	LPCHARACTER pChar;
	bool HasReviver;
};

bool CHARACTER::HasReviverInParty() const
{
	LPPARTY party = GetParty();

	if (party != NULL)
	{
		if (party->GetMemberCount() == 1) return false;

		FFindReviver f;
		party->ForEachMemberPtr(f);
		return f.HasReviver;
	}

	return false;
}

bool CHARACTER::IsRevive() const
{
	if (m_pkMobInst != NULL)
	{
		return m_pkMobInst->m_IsRevive;
	}

	return false;
}

void CHARACTER::SetRevive(bool mode)
{
	if (m_pkMobInst != NULL)
	{
		m_pkMobInst->m_IsRevive = mode;
	}
}

#define IS_SPEED_HACK_PLAYER(ch) (ch->m_speed_hack_count > SPEEDHACK_LIMIT_COUNT)

EVENTFUNC(check_speedhack_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_speedhack_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (NULL == ch || ch->IsNPC())
		return 0;

	if (IS_SPEED_HACK_PLAYER(ch))
	{
		LogManager::instance().SpeedHackLog(ch->GetPlayerID(), ch->GetX(), ch->GetY(), ch->m_speed_hack_count);
	}

	ch->m_speed_hack_count = 0;

	ch->ResetComboHackCount();
	return PASSES_PER_SEC(60);
}

void CHARACTER::StartCheckSpeedHackEvent()
{
	if (m_pkCheckSpeedHackEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkCheckSpeedHackEvent = event_create(check_speedhack_event, info, PASSES_PER_SEC(60));
}

void CHARACTER::GoHome()
{
	WarpSet(EMPIRE_START_X(GetEmpire()), EMPIRE_START_Y(GetEmpire()));
}

void CHARACTER::SendGuildName(CGuild* pGuild)
{
	if (NULL == pGuild) return;

	DESC	*desc = GetDesc();

	if (NULL == desc) return;
	if (m_known_guild.find(pGuild->GetID()) != m_known_guild.end()) return;

	m_known_guild.insert(pGuild->GetID());

	TPacketGCGuildName	pack;
	memset(&pack, 0x00, sizeof(pack));

	pack.header		= HEADER_GC_GUILD;
	pack.subheader	= GUILD_SUBHEADER_GC_GUILD_NAME;
	pack.size		= sizeof(TPacketGCGuildName);
	pack.guildID	= pGuild->GetID();
	memcpy(pack.guildName, pGuild->GetName(), GUILD_NAME_MAX_LEN);

	desc->Packet(&pack, sizeof(pack));
}

void CHARACTER::SendGuildName(DWORD dwGuildID)
{
	SendGuildName(CGuildManager::instance().FindGuild(dwGuildID));
}

EVENTFUNC(destroy_when_idle_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "destroy_when_idle_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = info->ch;
	if (ch == NULL)
	{
		return 0;
	}

	if (ch->GetVictim())
	{
		return PASSES_PER_SEC(300);
	}

	sys_log(1, "DESTROY_WHEN_IDLE: %s", ch->GetName());

	ch->m_pkDestroyWhenIdleEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

void CHARACTER::StartDestroyWhenIdleEvent()
{
	if (m_pkDestroyWhenIdleEvent)
		return;

	char_event_info* info = AllocEventInfo<char_event_info>();

	info->ch = this;

	m_pkDestroyWhenIdleEvent = event_create(destroy_when_idle_event, info, PASSES_PER_SEC(300));
}

void CHARACTER::SetComboSequence(BYTE seq)
{
	m_bComboSequence = seq;
}

BYTE CHARACTER::GetComboSequence() const
{
	return m_bComboSequence;
}

void CHARACTER::SetLastComboTime(DWORD time)
{
	m_dwLastComboTime = time;
}

DWORD CHARACTER::GetLastComboTime() const
{
	return m_dwLastComboTime;
}

void CHARACTER::SetValidComboInterval(int interval)
{
	m_iValidComboInterval = interval;
}

int CHARACTER::GetValidComboInterval() const
{
	return m_iValidComboInterval;
}

BYTE CHARACTER::GetComboIndex() const
{
	return m_bComboIndex;
}

void CHARACTER::IncreaseComboHackCount(int k)
{
	m_iComboHackCount += k;

	if (m_iComboHackCount >= 10)
	{
		if (GetDesc())
			if (GetDesc()->DelayedDisconnect(number(2, 7)))
			{
				sys_log(0, "COMBO_HACK_DISCONNECT: %s count: %d", GetName(), m_iComboHackCount);
				LogManager::instance().HackLog("Combo", this);
			}
	}
}

void CHARACTER::ResetComboHackCount()
{
	m_iComboHackCount = 0;
}

void CHARACTER::SkipComboAttackByTime(int interval)
{
	m_dwSkipComboAttackByTime = get_dword_time() + interval;
}

DWORD CHARACTER::GetSkipComboAttackByTime() const
{
	return m_dwSkipComboAttackByTime;
}

void CHARACTER::ResetChatCounter()
{
	m_bChatCounter = 0;
}

BYTE CHARACTER::IncreaseChatCounter()
{
	return ++m_bChatCounter;
}

BYTE CHARACTER::GetChatCounter() const
{
	return m_bChatCounter;
}

#ifdef ENABLE_MOUNT_SYSTEM
BYTE CHARACTER::GetMountCounter() const
{
	return m_bMountCounter;
}

void CHARACTER::ResetMountCounter()
{
	m_bMountCounter = 0;
}

BYTE CHARACTER::IncreaseMountCounter()
{
	return ++m_bMountCounter;
}
#endif

bool CHARACTER::IsRiding() const
{
	return IsHorseRiding() || GetMountVnum();
}

bool CHARACTER::CanWarp() const
{
	const int iPulse = thecore_pulse();
	const int limit_time = PASSES_PER_SEC(g_nPortalLimitTime);

	if ((iPulse - GetSafeboxLoadTime()) < limit_time)
		return false;

	if ((iPulse - GetExchangeTime()) < limit_time)
		return false;

	if ((iPulse - GetMyShopTime()) < limit_time)
		return false;

	if ((iPulse - GetRefineTime()) < limit_time)
		return false;

	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		|| IsAuraRefineWindowOpen()
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		|| GetTransmutation()
#endif
		)
		return false;

	return true;
}

DWORD CHARACTER::GetNextExp() const
{
	if (PLAYER_EXP_TABLE_MAX < GetLevel())
		return 2500000000;
	else
		return exp_table[GetLevel()];
}

int	CHARACTER::GetSkillPowerByLevel(int level, bool bMob) const
{
	return CTableBySkill::instance().GetSkillPowerByLevelFromType(GetJob(), GetSkillGroup(), MINMAX(0, level, SKILL_MAX_LEVEL), bMob); 
}

void CHARACTER::SetLastPMPulse(void)
{
	m_iLastPMPulse = thecore_pulse() + 25;
}

#ifdef ENABLE_CHANGE_CHANNEL
void CHARACTER::ChangeChannel(DWORD channelId)
{
	long lAddr;
	long lMapIndex;
	WORD wPort;
	long x = this->GetX();
	long y = this->GetY();

	if (!CMapLocation::instance().Get(x, y, lMapIndex, lAddr, wPort))
	{
		sys_err("cannot find map location index %d x %d y %d name %s", lMapIndex, x, y, GetName());
		return;
	}

	if(lMapIndex >= 10000)
	{
		this->LocaleChatPacket(CHAT_TYPE_INFO, 61, "");
		return;
	}

	Stop();
	Save();

	if(GetSectree())
	{
		GetSectree()->RemoveEntity(this);
		ViewCleanup();
		EncodeRemovePacket(this);
	}

	TPacketGCWarp p;

	p.bHeader = HEADER_GC_WARP;
	p.lX = x;
	p.lY = y;
	p.lAddr	= lAddr;
	p.wPort	= (wPort - 100*(g_bChannel-1) + 100*(channelId-1));

#ifdef ENABLE_RENEWAL_SWITCHBOT
	DWORD dwPID = GetPlayerID();
	CSwitchbotManager::Instance().SetIsWarping(dwPID, true);

	if (CSwitchbotManager::Instance().CanFindSwitchbot(dwPID))
		if (p.wPort != mother_port)
			CSwitchbotManager::Instance().P2PSendSwitchbot(dwPID, p.wPort);
#endif

	GetDesc()->Packet(&p, sizeof(TPacketGCWarp));
}
#endif

#ifdef ENABLE_RENEWAL_DEAD_PACKET
DWORD CHARACTER::CalculateDeadTime(BYTE type)
{
	if (!m_pkDeadEvent)
		return 0;

	int iTimeToDead = (event_time(m_pkDeadEvent) / passes_per_sec);
	if (type == REVIVE_TYPE_AUTO_TOWN)
		return iTimeToDead-7;

	if (!test_server && type == REVIVE_TYPE_HERE && (!GetWarMap() || GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (IsHack(false))
			return iTimeToDead - (180 - g_nPortalLimitTime);

		if (iTimeToDead > 170)
			return iTimeToDead - 170;
	}

	if (IsHack(false) && ((!GetWarMap() || GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG)))
		return iTimeToDead - (180 - g_nPortalLimitTime);

	if (iTimeToDead > 173) 
		return iTimeToDead - 173;

	return 0;
}
#endif

#ifdef ENABLE_MOUNT_SYSTEM
void CHARACTER::MountSummon(LPITEM mountItem)
{
	if (IsPolymorphed() == true)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 62, "");
		return; 
	}

	if (GetMapIndex() == 113)
		return;

	if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
		return;

	CMountSystem* mountSystem = GetMountSystem();
#ifdef ENABLE_MOUNT_PET_SKIN
	LPITEM mountSkinItem = GetWear(WEAR_COSTUME_MOUNT);
#endif
	DWORD mobVnum = 0;

	if (!mountSystem || !mountItem)
		return;

#ifdef ENABLE_MOUNT_PET_SKIN
	if (mountSkinItem)
		mobVnum = mountSkinItem->GetValue(1);
	else
		mobVnum = mountItem->GetValue(1);
#else
	if(mountItem->GetValue(1) != 0)
		mobVnum = mountItem->GetValue(1);
#endif

	if (IsHorseRiding())
		StopRiding();

	if (GetHorse())
		HorseSummon(false);

	mountSystem->Summon(mobVnum, mountItem, false);
}

void CHARACTER::MountUnsummon(LPITEM mountItem)
{
	CMountSystem* mountSystem = GetMountSystem();
#ifdef ENABLE_MOUNT_PET_SKIN
	LPITEM mountSkinItem = GetWear(WEAR_COSTUME_MOUNT);
#endif
	DWORD mobVnum = 0;

	if (!mountSystem || !mountItem)
		return;

#ifdef ENABLE_MOUNT_PET_SKIN
	if (mountSkinItem)
		mobVnum = mountSkinItem->GetValue(1);
	else
		mobVnum = mountItem->GetValue(1);
#else
	if(mountItem->GetValue(1) != 0)
		mobVnum = mountItem->GetValue(1);
#endif

	if (GetMountVnum() == mobVnum)
		mountSystem->Unmount(mobVnum);

	mountSystem->Unsummon(mobVnum);
}

void CHARACTER::CheckMount()
{
	CMountSystem* mountSystem = GetMountSystem();
	LPITEM mountItem = GetWear(WEAR_MOUNT);
#ifdef ENABLE_MOUNT_PET_SKIN
	LPITEM mountSkinItem = GetWear(WEAR_COSTUME_MOUNT);
#endif
	DWORD mobVnum = 0;

	if (!mountSystem || !mountItem)
		return;

#ifdef ENABLE_MOUNT_PET_SKIN
	if (mountSkinItem)
		mobVnum = mountSkinItem->GetValue(1);
	else
		mobVnum = mountItem->GetValue(1);
#else
	if(mountItem->GetValue(1) != 0)
		mobVnum = mountItem->GetValue(1);
#endif

	if(mountSystem->CountSummoned() == 0)
	{
		mountSystem->Summon(mobVnum, mountItem, false);
	}
}

bool CHARACTER::IsRidingMount()
{
	return (GetWear(WEAR_MOUNT) || FindAffect(AFFECT_MOUNT));
}
#endif

#ifdef ENABLE_PET_SYSTEM
void CHARACTER::PetSummon(LPITEM petItem)
{
	CPetSystem* petSystem = GetPetSystem();
#ifdef ENABLE_MOUNT_PET_SKIN
	LPITEM petSkinItem = GetWear(WEAR_COSTUME_PET);
#endif
	DWORD mobVnum = 0;

	if (GetMapIndex() == 113)
		return;

	if (!petSystem || !petItem)
		return;

#ifdef ENABLE_MOUNT_PET_SKIN
	if (petSkinItem)
		mobVnum = petSkinItem->GetValue(1);
	else
		mobVnum = petItem->GetValue(1);
#else
	if(petItem->GetValue(1) != 0)
		mobVnum = petItem->GetValue(1);
#endif

	petSystem->Summon(mobVnum, petItem, false);
}

void CHARACTER::PetUnsummon(LPITEM petItem)
{
	CPetSystem* petSystem = GetPetSystem();
#ifdef ENABLE_MOUNT_PET_SKIN
	LPITEM petSkinItem = GetWear(WEAR_COSTUME_PET);
#endif
	DWORD mobVnum = 0;

	if (!petSystem || !petItem)
		return;

#ifdef ENABLE_MOUNT_PET_SKIN
	if (petSkinItem)
		mobVnum = petSkinItem->GetValue(1);
	else
		mobVnum = petItem->GetValue(1);
#else
	if(petItem->GetValue(1) != 0)
		mobVnum = petItem->GetValue(1);
#endif

	petSystem->Unsummon(mobVnum);
}

void CHARACTER::CheckPet()
{
	CPetSystem* petSystem = GetPetSystem();
	LPITEM petItem = GetWear(WEAR_PET);
#ifdef ENABLE_MOUNT_PET_SKIN
	LPITEM petSkinItem = GetWear(WEAR_COSTUME_PET);
#endif
	DWORD mobVnum = 0;

	if (GetMapIndex() == 113)
		return;

	if (!petSystem || !petItem)
		return;

#ifdef ENABLE_MOUNT_PET_SKIN
	if (petSkinItem)
		mobVnum = petSkinItem->GetValue(1);
	else
		mobVnum = petItem->GetValue(1);
#else
	if(petItem->GetValue(1) != 0)
		mobVnum = petItem->GetValue(1);
#endif

	if(petSystem->CountSummoned() == 0)
	{
		petSystem->Summon(mobVnum, petItem, false);
	}
	else
	{
		ReloadPetBonus(petItem);
	}
}

void CHARACTER::ReloadPetBonus(LPITEM pSummonItem)
{
	RemoveAffect(AFFECT_PET_SUMMON);

	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (pSummonItem->GetProto()->aApplies[i].bType == APPLY_NONE)
			continue;

		AddAffect(AFFECT_PET_SUMMON, aApplyInfo[pSummonItem->GetProto()->aApplies[i].bType].bPointType, pSummonItem->GetProto()->aApplies[i].lValue, AFF_NONE, 60*60*24*365, 0, false);
	}
}

bool CHARACTER::IsPetSummon()
{
	return (GetWear(WEAR_PET));
}
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
BYTE CHARACTER::GetLanguage() const
{
#ifdef ENABLE_BOT_PLAYER
	// Bot karakterlerin dil bilgisini CBotCharacterManager'dan al
	if (IsBotCharacter())
	{
		BYTE botLanguage = CBotCharacterManager::instance().GetBotLanguage(GetName());
		if (botLanguage != 0)
			return botLanguage;
		// Eðer bot dil bilgisi bulunamazsa default language döndür
	}
#endif

	if (IsPC() && (!GetDesc() || !GetDesc()->GetCharacter()))
		return LOCALE_DEFAULT;

	if (GetDesc())
		return GetDesc()->GetLanguage();

	return LOCALE_DEFAULT;
}

bool CHARACTER::ChangeLanguage(BYTE bLanguage)
{
	if (!IsPC())
		return false;

	if (!CanWarp())
		return false;

	TPacketChangeLanguage packet;
	packet.bHeader = HEADER_GC_REQUEST_CHANGE_LANGUAGE;
	packet.bLanguage = bLanguage;
	GetDesc()->Packet(&packet, sizeof(packet));

	GetDesc()->SetLanguage(bLanguage);
	UpdatePacket();

	char buf[256];
	snprintf(buf, sizeof(buf), "%s Language %d", GetName(), bLanguage);
	LogManager::instance().CharLog(this, 0, "CHANGE_LANGUAGE", buf);

	return true;
}

BYTE CHARACTER::GetCountryFlagFromGMList(BYTE bFlag, const char* TargetName, bool bMessenger)
{
	if (!IsPC())
		return false;

	if (!bFlag)
		return false;

	char szTargetNameCut[CHARACTER_NAME_MAX_LEN + 1];
	strlcpy(szTargetNameCut, TargetName, sizeof(szTargetNameCut));

	char userNameMessengerEscaped[CHARACTER_NAME_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(userNameMessengerEscaped, sizeof(userNameMessengerEscaped), szTargetNameCut, strlen(szTargetNameCut));

	const char* GetNameForQuery;
	const char* GetTableName;
	bool bTarget;

	if (bMessenger == true)
	{
		GetNameForQuery = userNameMessengerEscaped;
		bTarget = true;
	}
	else
	{
		GetNameForQuery = GetName();
		bTarget = false;
	}

	GetTableName = "gmlist";

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT mLanguage+0, mSecondLanguage+0 FROM common.%s INNER JOIN account.account ON account.account.id = common.%s.mIDAccount WHERE common.%s.`mName` = '%s'", GetTableName, GetTableName, GetTableName, GetNameForQuery));
	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
	BYTE bLanguage, bLanguage2nd;
	BYTE flag;

	if (pMsg->Get()->uiNumRows)
	{
		str_to_number(bLanguage, row[0]);
		str_to_number(bLanguage2nd, row[1]);
	}

	if (bFlag == 1)
		flag = bLanguage;
	else if (bFlag == 2)
		flag = bLanguage2nd;
	else
		return false;

	return flag;
}
#endif

#ifdef ENABLE_TELEPORT_TO_A_FRIEND
bool CHARACTER::SummonCharacter(LPCHARACTER victim)
{
	if (!IsAllowedMapIndex(GetMapIndex()))
	{
		victim->LocaleChatPacket(CHAT_TYPE_INFO, 63, "");
		return false;
	}
	
	if ((victim = (CHARACTER_MANAGER::instance().FindByPID(victim->GetPlayerID()))))
	{
		int mapIdx = victim->GetMapIndex();
		if (IS_SUMMONABLE_ZONE(mapIdx))
		{
			if (CAN_ENTER_ZONE(this, mapIdx))
			{
				victim->WarpSet(GetX(), GetY());
			}
			else
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 54, "");
				return false;
			}
		}
		else
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 54, "");
			return false;
		}
	}
	else
	{
		CCI * pcci = P2P_MANAGER::instance().FindByPID(victim->GetPlayerID());

		if (!pcci)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 55, "");
			return false;
		}

		if (!IS_SUMMONABLE_ZONE(pcci->lMapIndex))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 54, "");
			return false;
		}
		else if (pcci->bChannel != g_bChannel)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 56, "%d#%d", pcci->bChannel, g_bChannel);
			return false;
		}
		else
		{
			if (!CAN_ENTER_ZONE(this, pcci->lMapIndex))
			{
				LocaleChatPacket(CHAT_TYPE_INFO, 54, "");
				return false;
			}

			TPacketGGFindPosition p;
			p.header = HEADER_GG_FIND_POSITION;
			p.dwFromPID = GetPlayerID();
			p.dwTargetPID = victim->GetPlayerID();
			pcci->pkDesc->Packet(&p, sizeof(TPacketGGFindPosition));
		}
	}
	return true;
}

bool CHARACTER::IsAllowedMapIndex(int mapindex)
{
	switch(mapindex)
	{
		case 1:		// metin2_map_a1
		case 3:		// metin2_map_a3
		case 21:	// metin2_map_b1
		case 23:	// metin2_map_b3
		case 41:	// metin2_map_c1
		case 43:	// metin2_map_c3
		case 45:	// metin2_map_monkey_dungeon_13
		case 61:	// map_n_snowm_01
		case 62:	// metin2_map_n_flame_01
		case 63:	// metin2_map_n_desert_01
		case 65:	// metin2_map_milgyo
		case 68:	// metin2_map_trent02
		case 71:	// metin2_map_spiderdungeon_02
		case 72:	// metin2_map_skipia_dungeon_01
		case 73:	// metin2_map_skipia_dungeon_02
			return true;
	}
	return false;
}
#endif

#ifdef ENABLE_MOUNT_PET_SKIN
bool CHARACTER::CanChangeCostumeMountSkin()
{
	return (thecore_pulse() - GetLastChangeCostumeMountSkinTime() > PASSES_PER_SEC(5));
}

bool CHARACTER::CanChangeCostumePetSkin()
{
	return (thecore_pulse() - GetLastChangeCostumePetSkinTime() > PASSES_PER_SEC(5));
}

void CHARACTER::EquipCostumeMountSkin()
{
	if (!IsRidingMount()) return;

	CMountSystem* mountSystem = GetMountSystem();
	LPITEM mountItem = GetWear(WEAR_MOUNT);
	if (!mountSystem || !mountItem) return;

	DWORD mobVnum = mountItem->GetValue(1);

	if (GetMountVnum() == mobVnum)
		mountSystem->Unmount(mobVnum);

	mountSystem->Unsummon(mobVnum);
	CheckMount();
	UpdateChangeCostumeMountSkinTime();
}

void CHARACTER::RemoveCostumeMountSkin(LPITEM mountSkin)
{
	if (!IsRidingMount()) return;

	CMountSystem* mountSystem = GetMountSystem();
	if (!mountSystem || !mountSkin) return;

	DWORD mobVnum = mountSkin->GetValue(1);

	if (GetMountVnum() == mobVnum)
		mountSystem->Unmount(mobVnum);

	mountSystem->Unsummon(mobVnum);
	CheckMount();
	UpdateChangeCostumeMountSkinTime();
}

void CHARACTER::EquipCostumePetSkin()
{
	if (!IsPetSummon()) return;

	CPetSystem* petSystem = GetPetSystem();
	LPITEM petItem = GetWear(WEAR_PET);
	if (!petSystem || !petItem) return;

	petSystem->Unsummon(petItem->GetValue(1));
	CheckPet();
	UpdateChangeCostumePetSkinTime();
}

void CHARACTER::RemoveCostumePetSkin(LPITEM petSkin)
{
	if (!IsPetSummon()) return;

	CPetSystem* petSystem = GetPetSystem();
	if (!petSystem || !petSkin) return;

	petSystem->Unsummon(petSkin->GetValue(1));
	CheckPet();
	UpdateChangeCostumePetSkinTime();
}
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
void CHARACTER::SetBodyCostumeHidden(bool hidden, bool pass)
{
	m_bHideBodyCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetBodyCostumeHidden %d", m_bHideBodyCostume ? 1 : 0);

	if (!pass)
	{
		SetQuestFlag("costume_option.hide_body", m_bHideBodyCostume ? 1 : 0);
	}
}

void CHARACTER::SetHairCostumeHidden(bool hidden, bool pass)
{
	m_bHideHairCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetHairCostumeHidden %d", m_bHideHairCostume ? 1 : 0);

	if (!pass)
	{
		SetQuestFlag("costume_option.hide_hair", m_bHideHairCostume ? 1 : 0);
	}
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void CHARACTER::SetAcceCostumeHidden(bool hidden, bool pass)
{
	m_bHideAcceCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetAcceCostumeHidden %d", m_bHideAcceCostume ? 1 : 0);

	if (!pass)
	{
		SetQuestFlag("costume_option.hide_acce", m_bHideAcceCostume ? 1 : 0);
	}
}
#endif

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
void CHARACTER::SetWeaponCostumeHidden(bool hidden, bool pass)
{
	m_bHideWeaponCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetWeaponCostumeHidden %d", m_bHideWeaponCostume ? 1 : 0);

	if (!pass)
	{
		SetQuestFlag("costume_option.hide_weapon", m_bHideWeaponCostume ? 1 : 0);
	}
}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
void CHARACTER::SetAuraCostumeHidden(bool hidden, bool pass)
{
	m_bHideAuraCostume = hidden;
	ChatPacket(CHAT_TYPE_COMMAND, "SetAuraCostumeHidden %d", m_bHideAuraCostume ? 1 : 0);

	if (!pass)
	{
		SetQuestFlag("costume_option.hide_aura", m_bHideAuraCostume ? 1 : 0);
	}
}
#endif
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
std::vector<LPITEM> CHARACTER::GetAcceMaterials()
{
	return std::vector<LPITEM>{ITEM_MANAGER::instance().Find(m_pointsInstant.pAcceMaterials[0].id), ITEM_MANAGER::instance().Find(m_pointsInstant.pAcceMaterials[1].id)};
}

const TItemPosEx* CHARACTER::GetAcceMaterialsInfo()
{
	return m_pointsInstant.pAcceMaterials;
}

void CHARACTER::SetAcceMaterial(int pos, LPITEM ptr)
{
	if (pos < 0 || pos >= ACCE_WINDOW_MAX_MATERIALS)
		return;
	if (!ptr)
		m_pointsInstant.pAcceMaterials[pos] = {};
	else
	{
		m_pointsInstant.pAcceMaterials[pos].id = ptr->GetID();
		m_pointsInstant.pAcceMaterials[pos].pos.cell = ptr->GetCell();
		m_pointsInstant.pAcceMaterials[pos].pos.window_type = ptr->GetWindow();
	}
}

void CHARACTER::OpenAcce(bool bCombination)
{
	if (IsAcceOpened(bCombination))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 52, "");
		return;
	}

	if (bCombination)
	{
		if (m_bAcceAbsorption)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 64, "");
			return;
		}

		m_bAcceCombination = true;
	}
	else
	{
		if (m_bAcceCombination)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 65, "");
			return;
		}

		m_bAcceAbsorption = true;
	}

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_OPEN;
	sPacket.bWindow = bCombination;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));

	ClearAcceMaterials();
}

void CHARACTER::CloseAcce()
{
	if ((!m_bAcceCombination) && (!m_bAcceAbsorption))
		return;

	bool bWindow = m_bAcceCombination;

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_CLOSE;
	sPacket.bWindow = bWindow;
	sPacket.dwPrice = 0;
	sPacket.bPos = 0;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));

	if (bWindow)
		m_bAcceCombination = false;
	else
		m_bAcceAbsorption = false;

	ClearAcceMaterials();
}

void CHARACTER::ClearAcceMaterials()
{
	auto pkItemMaterial = GetAcceMaterials();
	for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			continue;

		pkItemMaterial[i]->Lock(false);
		pkItemMaterial[i] = NULL;
		SetAcceMaterial(i, nullptr);
	}
}

bool CHARACTER::AcceIsSameGrade(long lGrade)
{
	auto pkItemMaterial = GetAcceMaterials();
	if (!pkItemMaterial[0])
		return false;
	return pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD) == lGrade;
}

#ifdef ENABLE_GOLD_LIMIT
long long CHARACTER::GetAcceCombinePrice(long lGrade)
#else
DWORD CHARACTER::GetAcceCombinePrice(long lGrade)
#endif
{
#ifdef ENABLE_GOLD_LIMIT
	long long dwPrice = 0;
#else
	DWORD dwPrice = 0;
#endif

	switch (lGrade)
	{
	case 2:
	{
		dwPrice = ACCE_GRADE_2_PRICE;
	}
	break;
	case 3:
	{
		dwPrice = ACCE_GRADE_3_PRICE;
	}
	break;
	case 4:
	{
		dwPrice = ACCE_GRADE_4_PRICE;
	}
	break;
	default:
	{
		dwPrice = ACCE_GRADE_1_PRICE;
	}
	break;
	}

	return dwPrice;
}

BYTE CHARACTER::CheckEmptyMaterialSlot()
{
	auto pkItemMaterial = GetAcceMaterials();
	for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
	{
		if (!pkItemMaterial[i])
			return i;
	}

	return 255;
}

void CHARACTER::GetAcceCombineResult(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs)
{
	auto pkItemMaterial = GetAcceMaterials();

	if (m_bAcceCombination)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			long lVal = pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD);
			if (lVal == 4)
			{
				dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
				dwMinAbs = pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET);
				DWORD dwMaxAbsCalc = (dwMinAbs + ACCE_GRADE_4_ABS_RANGE > ACCE_GRADE_4_ABS_MAX ? ACCE_GRADE_4_ABS_MAX : (dwMinAbs + ACCE_GRADE_4_ABS_RANGE));
				dwMaxAbs = dwMaxAbsCalc;
			}
			else
			{
				DWORD dwMaskVnum = pkItemMaterial[0]->GetOriginalVnum();
				TItemTable * pTable = ITEM_MANAGER::instance().GetTable(dwMaskVnum + 1);
				if (pTable)
					dwMaskVnum += 1;

				dwItemVnum = dwMaskVnum;
				switch (lVal)
				{
				case 2:
				{
					dwMinAbs = ACCE_GRADE_3_ABS;
					dwMaxAbs = ACCE_GRADE_3_ABS;
				}
				break;
				case 3:
				{
					dwMinAbs = ACCE_GRADE_4_ABS_MIN;
					dwMaxAbs = ACCE_GRADE_4_ABS_MAX_COMB;
				}
				break;
				default:
				{
					dwMinAbs = ACCE_GRADE_2_ABS;
					dwMaxAbs = ACCE_GRADE_2_ABS;
				}
				break;
				}
			}
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
	else
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			dwItemVnum = pkItemMaterial[0]->GetOriginalVnum();
			dwMinAbs = pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET);
			dwMaxAbs = dwMinAbs;
		}
		else
		{
			dwItemVnum = 0;
			dwMinAbs = 0;
			dwMaxAbs = 0;
		}
	}
}

void CHARACTER::AddAcceMaterial(TItemPos tPos, BYTE bPos)
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
	{
		if (bPos == 255)
		{
			bPos = CheckEmptyMaterialSlot();
			if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
				return;
		}
		else
			return;
	}

	LPITEM pkItem = GetItem(tPos);
	if (!pkItem)
		return;
	else if ((pkItem->GetCell() >= INVENTORY_MAX_NUM) || (pkItem->IsEquipped()) || (tPos.IsBeltInventoryPosition()) || (pkItem->IsDragonSoul()))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (m_bAcceCombination))
		return;
	else if ((pkItem->GetType() != ITEM_COSTUME) && (bPos == 0) && (m_bAcceAbsorption))
		return;
	else if (pkItem->isLocked())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 66, "");
		return;
	}
	else if ((m_bAcceCombination) && (bPos == 1) && (!AcceIsSameGrade(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD))))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 67, "");
		return;
	}
	else if ((m_bAcceCombination) && (pkItem->GetSocket(ACCE_ABSORPTION_SOCKET) >= ACCE_GRADE_4_ABS_MAX))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 68, "");
		return;
	}
	else if ((bPos == 1) && (m_bAcceAbsorption))
	{
		if ((pkItem->GetType() != ITEM_WEAPON) && (pkItem->GetType() != ITEM_ARMOR))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 69, "");
			return;
		}
		else if ((pkItem->GetType() == ITEM_ARMOR) && (pkItem->GetSubType() != ARMOR_BODY))
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 69, "");
			return;
		}
	}
	else if ((pkItem->GetSubType() != COSTUME_ACCE) && (m_bAcceCombination))
		return;
	else if ((pkItem->GetSubType() != COSTUME_ACCE) && (bPos == 0) && (m_bAcceAbsorption))
		return;
	else if ((pkItem->GetSocket(ACCE_ABSORBED_SOCKET) > 0) && (bPos == 0) && (m_bAcceAbsorption))
		return;

	auto pkItemMaterial = GetAcceMaterials();
	if ((bPos == 1) && (!pkItemMaterial[0]))
		return;

	if (pkItemMaterial[bPos])
		return;

	SetAcceMaterial(bPos, pkItem);
	pkItemMaterial[bPos] = pkItem;
	pkItemMaterial[bPos]->Lock(true);

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAcceCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_ADDED;
	sPacket.bWindow = m_bAcceCombination;
	sPacket.dwPrice = GetAcceCombinePrice(pkItem->GetValue(ACCE_GRADE_VALUE_FIELD));
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = dwItemVnum;
	sPacket.dwMinAbs = dwMinAbs;
	sPacket.dwMaxAbs = dwMaxAbs;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
}

void CHARACTER::RemoveAcceMaterial(BYTE bPos)
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
		return;

	auto pkItemMaterial = GetAcceMaterials();

#ifdef ENABLE_GOLD_LIMIT
	long long dwPrice = 0;
#else
	DWORD dwPrice = 0;
#endif

	if (bPos == 1)
	{
		if (pkItemMaterial[bPos])
		{
			pkItemMaterial[bPos]->Lock(false);
			pkItemMaterial[bPos] = NULL;
			SetAcceMaterial(bPos, nullptr);
		}

		if (pkItemMaterial[0])
			dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));
	}
	else
		ClearAcceMaterials();

	TItemPos tPos;
	tPos.window_type = INVENTORY;
	tPos.cell = 0;

	TPacketAcce sPacket;
	sPacket.header = HEADER_GC_ACCE;
	sPacket.subheader = ACCE_SUBHEADER_GC_REMOVED;
	sPacket.bWindow = m_bAcceCombination;
	sPacket.dwPrice = dwPrice;
	sPacket.bPos = bPos;
	sPacket.tPos = tPos;
	sPacket.dwItemVnum = 0;
	sPacket.dwMinAbs = 0;
	sPacket.dwMaxAbs = 0;
	GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
}

BYTE CHARACTER::CanRefineAcceMaterials()
{
	BYTE bReturn = 0;
	if (!GetDesc())
		return bReturn;

	if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
		return bReturn;

	auto materialInfo = GetAcceMaterialsInfo();
	auto pkItemMaterial = GetAcceMaterials();
	if (!pkItemMaterial[0] || !pkItemMaterial[1])
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial null");
		return bReturn;
	}
	else if (pkItemMaterial[0]->GetOwner()!=this || pkItemMaterial[1]->GetOwner() != this)
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial different ownership");
		return bReturn;
	}
	else if (pkItemMaterial[0]->IsEquipped() || pkItemMaterial[1]->IsEquipped())
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial equipped");
		return bReturn;
	}
	else if (pkItemMaterial[0]->GetWindow() != INVENTORY || pkItemMaterial[1]->GetWindow() != INVENTORY)
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial not in INVENTORY");
		return bReturn;
	}
	else if (!materialInfo[0].id || !materialInfo[1].id)
	{
		sys_err("CanRefineAcceMaterials: materialInfo id 0");
		return bReturn;
	}
	else if (materialInfo[0].pos.cell != pkItemMaterial[0]->GetCell() || materialInfo[1].pos.cell != pkItemMaterial[1]->GetCell())
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial wrong cell");
		return bReturn;
	}
	else if (materialInfo[0].pos.window_type != pkItemMaterial[0]->GetWindow() || materialInfo[1].pos.window_type != pkItemMaterial[1]->GetWindow())
	{
		sys_err("CanRefineAcceMaterials: pkItemMaterial wrong window_type");
		return bReturn;
	}

	if (m_bAcceCombination)
	{
		if (!AcceIsSameGrade(pkItemMaterial[1]->GetValue(ACCE_GRADE_VALUE_FIELD)))
		{
			sys_err("CanRefineAcceMaterials: pkItemMaterial different acce grade");
			return bReturn;
		}

		for (int i = 0; i < ACCE_WINDOW_MAX_MATERIALS; ++i)
		{
			if (pkItemMaterial[i])
			{
				if ((pkItemMaterial[i]->GetType() == ITEM_COSTUME) && (pkItemMaterial[i]->GetSubType() == COSTUME_ACCE))
					bReturn = 1;
				else
				{
					bReturn = 0;
					break;
				}
			}
			else
			{
				bReturn = 0;
				break;
			}
		}
	}
	else if (m_bAcceAbsorption)
	{
		if ((pkItemMaterial[0]) && (pkItemMaterial[1]))
		{
			if ((pkItemMaterial[0]->GetType() == ITEM_COSTUME) && (pkItemMaterial[0]->GetSubType() == COSTUME_ACCE))
				bReturn = 2;
			else
				bReturn = 0;

			if ((pkItemMaterial[1]->GetType() == ITEM_WEAPON) || ((pkItemMaterial[1]->GetType() == ITEM_ARMOR) && (pkItemMaterial[1]->GetSubType() == ARMOR_BODY)))
				bReturn = 2;
			else
				bReturn = 0;

			if (pkItemMaterial[0]->GetSocket(ACCE_ABSORBED_SOCKET) > 0)
				bReturn = 0;
		}
		else
			bReturn = 0;
	}

	return bReturn;
}

void CHARACTER::RefineAcceMaterials()
{
	BYTE bCan = CanRefineAcceMaterials();
	if (bCan == 0)
		return;

	auto pkItemMaterial = GetAcceMaterials();

	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	GetAcceCombineResult(dwItemVnum, dwMinAbs, dwMaxAbs);

#ifdef ENABLE_GOLD_LIMIT
	long long dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));
#else
	DWORD dwPrice = GetAcceCombinePrice(pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD));
#endif

	if (bCan == 1)
	{
		int iSuccessChance = 0;
		long lVal = pkItemMaterial[0]->GetValue(ACCE_GRADE_VALUE_FIELD);
		switch (lVal)
		{
		case 2:
		{
			iSuccessChance = ACCE_COMBINE_GRADE_2;
		}
		break;
		case 3:
		{
			iSuccessChance = ACCE_COMBINE_GRADE_3;
		}
		break;
		case 4:
		{
			iSuccessChance = ACCE_COMBINE_GRADE_4;
		}
		break;
		default:
		{
			iSuccessChance = ACCE_COMBINE_GRADE_1;
		}
		break;
		}

		if (GetGold() < dwPrice)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 70, "");
			return;
		}

		int iChance = number(1, 100);
		bool bSucces = (iChance <= iSuccessChance ? true : false);
		if (bSucces)
		{
			LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(dwItemVnum, 1, 0, false);
			if (!pkItem)
			{
				sys_err("%d can't be created.", dwItemVnum);
				return;
			}

			ITEM_MANAGER::CopyAllAttrTo(pkItemMaterial[0], pkItem);
			LogManager::instance().ItemLog(this, pkItem, "COMBINE SUCCESS", pkItem->GetName());
			DWORD dwAbs = (dwMinAbs == dwMaxAbs ? dwMinAbs : number(dwMinAbs + 1, dwMaxAbs));
			pkItem->SetSocket(ACCE_ABSORPTION_SOCKET, dwAbs);
			pkItem->SetSocket(ACCE_ABSORBED_SOCKET, pkItemMaterial[0]->GetSocket(ACCE_ABSORBED_SOCKET));

			PointChange(POINT_GOLD, -dwPrice);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);

			WORD wCell = pkItemMaterial[0]->GetCell();
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[0], "COMBINE (REFINE SUCCESS)");
			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE SUCCESS)");

			pkItem->AddToCharacter(this, TItemPos(INVENTORY, wCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
			pkItem->AttrLog();

			if (lVal == 4)
				LocaleChatPacket(CHAT_TYPE_INFO, 71, "%d", dwAbs);
			else
				LocaleChatPacket(CHAT_TYPE_INFO, 72, "");

			EffectPacket(SE_EFFECT_ACCE_SUCCEDED);
			LogManager::instance().AcceLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, pkItem->GetID(), 1, dwAbs, 1);

			ClearAcceMaterials();
		}
		else
		{
			PointChange(POINT_GOLD, -dwPrice);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, pkItemMaterial[0]->GetVnum(), -dwPrice);

			ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "COMBINE (REFINE FAIL)");

			if (lVal == 4)
				LocaleChatPacket(CHAT_TYPE_INFO, 71, "%d", pkItemMaterial[0]->GetSocket(ACCE_ABSORPTION_SOCKET));
			else
				LocaleChatPacket(CHAT_TYPE_INFO, 73, "");

			LogManager::instance().AcceLog(GetPlayerID(), GetX(), GetY(), dwItemVnum, 0, 0, 0, 0);

			pkItemMaterial[1] = NULL;
		}

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAcce sPacket;
		sPacket.header = HEADER_GC_ACCE;
		sPacket.subheader = ACCE_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bAcceCombination;
		sPacket.dwPrice = dwPrice;
		sPacket.bPos = 0;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		if (bSucces)
			sPacket.dwMaxAbs = 100;
		else
			sPacket.dwMaxAbs = 0;

		GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
	}
	else
	{
		pkItemMaterial[1]->CopyAttributeTo(pkItemMaterial[0]);
		LogManager::instance().ItemLog(this, pkItemMaterial[0], "ABSORB (REFINE SUCCESS)", pkItemMaterial[0]->GetName());
		pkItemMaterial[0]->SetSocket(ACCE_ABSORBED_SOCKET, pkItemMaterial[1]->GetOriginalVnum());
		ITEM_MANAGER::instance().RemoveItem(pkItemMaterial[1], "ABSORBED (REFINE SUCCESS)");

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemMaterial[0]);
		pkItemMaterial[0]->AttrLog();

		LocaleChatPacket(CHAT_TYPE_INFO, 72, "");
		EffectPacket(SE_EFFECT_ACCE_SUCCEDED);

		ClearAcceMaterials();

		TItemPos tPos;
		tPos.window_type = INVENTORY;
		tPos.cell = 0;

		TPacketAcce sPacket;
		sPacket.header = HEADER_GC_ACCE;
		sPacket.subheader = ACCE_SUBHEADER_CG_REFINED;
		sPacket.bWindow = m_bAcceCombination;
		sPacket.dwPrice = dwPrice;
		sPacket.bPos = 255;
		sPacket.tPos = tPos;
		sPacket.dwItemVnum = 0;
		sPacket.dwMinAbs = 0;
		sPacket.dwMaxAbs = 1;
		GetDesc()->Packet(&sPacket, sizeof(TPacketAcce));
	}
}

bool CHARACTER::CleanAcceAttr(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 74, "");
		return false;
	}
	else if ((!pkItem) || (!pkTarget))
	{
		return false;
	}
	else if ((pkTarget->GetType() != ITEM_COSTUME) && (pkTarget->GetSubType() != COSTUME_ACCE))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 75, "");
		return false;
	}

	if (pkTarget->GetSocket(ACCE_ABSORBED_SOCKET) <= 0)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 76, "");
		return false;
	}

	pkTarget->SetSocket(ACCE_ABSORBED_SOCKET, 0);
	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		pkTarget->SetForceAttribute(i, 0, 0);

	LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT (CLEAN ATTR)", pkTarget->GetName());
	return true;
}
#endif

#ifdef ENABLE_TITLE_SYSTEM
void CHARACTER::EquipTitle(LPITEM item)
{
	DWORD iTitle = item->GetValue(1);
	SetPoint(POINT_TITLE, iTitle);
	ComputePoints();
	PointsPacket();
	UpdatePacket();
}

void CHARACTER::UnequipTitle(LPITEM item)
{
	SetPoint(POINT_TITLE, 0);
	ComputePoints();
	PointsPacket();
	UpdatePacket();
}
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
void CHARACTER::SetLastReciveExtBattlePassInfoTime(DWORD time)
{
	m_dwLastReciveExtBattlePassInfoTime = time;
}

void CHARACTER::SetLastReciveExtBattlePassOpenRanking(DWORD time)
{
	m_dwLastExtBattlePassOpenRankingTime = time;
}

void CHARACTER::LoadExtBattlePass(DWORD dwCount, TPlayerExtBattlePassMission* data)
{
	m_bIsLoadedExtBattlePass = false;

	for (int i = 0; i < dwCount; ++i, ++data)
	{
		TPlayerExtBattlePassMission* newMission = new TPlayerExtBattlePassMission;
		newMission->dwPlayerId = data->dwPlayerId;
		newMission->dwBattlePassType = data->dwBattlePassType;
		newMission->dwMissionIndex = data->dwMissionIndex;
		newMission->dwMissionType = data->dwMissionType;
		newMission->dwBattlePassId = data->dwBattlePassId;
		newMission->dwExtraInfo = data->dwExtraInfo;
		newMission->bCompleted = data->bCompleted;
		newMission->bIsUpdated = data->bIsUpdated;

		m_listExtBattlePass.push_back(newMission);
	}

	m_bIsLoadedExtBattlePass = true;
}

DWORD CHARACTER::GetExtBattlePassMissionProgress(DWORD dwBattlePassType, BYTE bMissionIndex, BYTE bMissionType)
{
	DWORD BattlePassID;
	if (dwBattlePassType == 1)
		BattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
	else if (dwBattlePassType == 2)
		BattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
	else
	{
		sys_err("Unknown BattlePassType (%d)", dwBattlePassType);
		return 0;
	}

	ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
	while (it != m_listExtBattlePass.end())
	{
		TPlayerExtBattlePassMission* pkMission = *it++;
		if (pkMission->dwBattlePassType == dwBattlePassType && pkMission->dwMissionIndex == bMissionIndex && pkMission->dwMissionType == bMissionType && pkMission->dwBattlePassId == BattlePassID)
			return pkMission->dwExtraInfo;
	}
	return 0;
}

bool CHARACTER::IsExtBattlePassCompletedMission(DWORD dwBattlePassType, BYTE bMissionIndex, BYTE bMissionType)
{
	DWORD BattlePassID;
	if (dwBattlePassType == 1)
		BattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
	else if (dwBattlePassType == 2)
		BattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
	else
	{
		sys_err("Unknown BattlePassType (%d)", dwBattlePassType);
		return false;
	}

	ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
	while (it != m_listExtBattlePass.end())
	{
		TPlayerExtBattlePassMission* pkMission = *it++;
		if (pkMission->dwBattlePassType == dwBattlePassType && pkMission->dwMissionIndex == bMissionIndex && pkMission->dwMissionType == bMissionType && pkMission->dwBattlePassId == BattlePassID)
			return (pkMission->bCompleted ? true : false);
	}
	return false;
}

bool CHARACTER::IsExtBattlePassRegistered(BYTE bBattlePassType, DWORD dwBattlePassID)
{
	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT * FROM player.battlepass_playerindex WHERE player_id = %d and battlepass_type = %d and battlepass_id = %d", GetPlayerID(), bBattlePassType, dwBattlePassID));
	if (pMsg->Get()->uiNumRows != 0)
		return true;

	return false;
}

void CHARACTER::UpdateExtBattlePassMissionProgress(DWORD dwMissionType, DWORD dwUpdateValue, DWORD dwCondition, bool isOverride)
{
	if (!GetDesc())
		return;

	if (!m_bIsLoadedExtBattlePass)
		return;

	DWORD dwSafeCondition = dwCondition;
	for (BYTE bBattlePassType = 1; bBattlePassType <= 2 ; ++bBattlePassType)
	{
		bool foundMission = false;
		DWORD dwSaveProgress = 0;
		dwCondition = dwSafeCondition;

		BYTE bBattlePassID;
		BYTE bMissionIndex = CBattlePassManager::instance().GetMissionIndex(bBattlePassType, dwMissionType, dwCondition);

		if (bBattlePassType == 1)
			bBattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();

		if (bBattlePassType == 2)
		{
			bBattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
			if (bBattlePassID != GetExtBattlePassPremiumID())
				continue;
		}

		DWORD dwFirstInfo, dwSecondInfo;
		if (CBattlePassManager::instance().BattlePassMissionGetInfo(bBattlePassType, bMissionIndex, bBattlePassID, dwMissionType, &dwFirstInfo, &dwSecondInfo))
		{
			if (dwFirstInfo == 0)
				dwCondition = 0;

			if (dwMissionType == 3 and dwFirstInfo <= dwCondition or dwMissionType == 6 and dwFirstInfo <= dwCondition)
				dwCondition = dwFirstInfo;

			if (dwFirstInfo == dwCondition && GetExtBattlePassMissionProgress(bBattlePassType, bMissionIndex, dwMissionType) < dwSecondInfo)
			{
				ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
				while (it != m_listExtBattlePass.end())
				{
					TPlayerExtBattlePassMission* pkMission = *it++;

					if (pkMission->dwBattlePassType == bBattlePassType && pkMission->dwMissionIndex == bMissionIndex && pkMission->dwMissionType == dwMissionType && pkMission->dwBattlePassId == bBattlePassID)
					{
						pkMission->bIsUpdated = 1;

						if (isOverride)
							pkMission->dwExtraInfo = dwUpdateValue;
						else
							pkMission->dwExtraInfo += dwUpdateValue;

						if (pkMission->dwExtraInfo >= dwSecondInfo)
						{
							pkMission->dwExtraInfo = dwSecondInfo;
							pkMission->bCompleted = 1;

							std::string stMissionName = CBattlePassManager::instance().GetMissionNameByType(pkMission->dwMissionType);
							std::string stBattlePassName = CBattlePassManager::instance().GetNormalBattlePassNameByID(pkMission->dwBattlePassId);

							CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, dwMissionType, bBattlePassID);
							if (bBattlePassType == 1)
							{
								EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
								LocaleChatPacket(CHAT_TYPE_NOTICE, 343, "");
							}
							if (bBattlePassType == 2)
							{
								EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
								LocaleChatPacket(CHAT_TYPE_NOTICE, 343, "");
							}

							TPacketGCExtBattlePassMissionUpdate packet;
							packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
							packet.bBattlePassType = bBattlePassType;
							packet.bMissionIndex = bMissionIndex;
							packet.dwNewProgress = pkMission->dwExtraInfo;
							GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
						}

						dwSaveProgress = pkMission->dwExtraInfo;
						foundMission = true;

						if (pkMission->bCompleted != 1)
						{
							TPacketGCExtBattlePassMissionUpdate packet;
							packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
							packet.bBattlePassType = bBattlePassType;
							packet.bMissionIndex = bMissionIndex;
							packet.dwNewProgress = dwSaveProgress;
							GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
						}
						break;
					}
				}

				if (!foundMission)
				{
					if (!IsExtBattlePassRegistered(bBattlePassType, bBattlePassID))
						DBManager::instance().DirectQuery("INSERT INTO player.battlepass_playerindex SET player_id=%d, player_name='%s', battlepass_type=%d, battlepass_id=%d, start_time=NOW()", GetPlayerID(), GetName(), bBattlePassType, bBattlePassID);

					TPlayerExtBattlePassMission* newMission = new TPlayerExtBattlePassMission;
					newMission->dwPlayerId = GetPlayerID();
					newMission->dwBattlePassType = bBattlePassType;
					newMission->dwMissionType = dwMissionType;
					newMission->dwBattlePassId = bBattlePassID;

					if (dwUpdateValue >= dwSecondInfo)
					{
						newMission->dwMissionIndex = CBattlePassManager::instance().GetMissionIndex(bBattlePassType, dwMissionType, dwCondition);
						newMission->dwExtraInfo = dwSecondInfo;
						newMission->bCompleted = 1;

						CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, dwMissionType, bBattlePassID);
						if (bBattlePassType == 1)
						{
							EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
							LocaleChatPacket(CHAT_TYPE_NOTICE, 343, "");
						}
						if (bBattlePassType == 2)
						{
							EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
							LocaleChatPacket(CHAT_TYPE_NOTICE, 343, "");
						}

						dwSaveProgress = dwSecondInfo;
					}
					else
					{
						newMission->dwMissionIndex = CBattlePassManager::instance().GetMissionIndex(bBattlePassType, dwMissionType, dwCondition);
						newMission->dwExtraInfo = dwUpdateValue;
						newMission->bCompleted = 0;

						dwSaveProgress = dwUpdateValue;
					}

					newMission->bIsUpdated = 1;

					m_listExtBattlePass.push_back(newMission);

					TPacketGCExtBattlePassMissionUpdate packet;
					packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
					packet.bBattlePassType = bBattlePassType;
					packet.bMissionIndex = bMissionIndex;
					packet.dwNewProgress = dwSaveProgress;
					GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
				}
			}
		}
	}
}

void CHARACTER::SetExtBattlePassMissionProgress(BYTE bBattlePassType, DWORD dwMissionIndex, DWORD dwMissionType, DWORD dwUpdateValue)
{
	if (!GetDesc())
		return;

	if (!m_bIsLoadedExtBattlePass)
		return;

	bool foundMission = false;
	DWORD dwSaveProgress = 0;

	BYTE bBattlePassID;
	if (bBattlePassType == 1)
		bBattlePassID = CBattlePassManager::instance().GetNormalBattlePassID();
	else if (bBattlePassType == 2)
		bBattlePassID = CBattlePassManager::instance().GetPremiumBattlePassID();
	else
	{
		sys_err("Unknown BattlePassType (%d)", bBattlePassType);
		return;
	}

	DWORD dwFirstInfo, dwSecondInfo;
	if (CBattlePassManager::instance().BattlePassMissionGetInfo(bBattlePassType, dwMissionIndex, bBattlePassID, dwMissionType, &dwFirstInfo, &dwSecondInfo))
	{
		ListExtBattlePassMap::iterator it = m_listExtBattlePass.begin();
		while (it != m_listExtBattlePass.end())
		{
			TPlayerExtBattlePassMission* pkMission = *it++;

			if (pkMission->dwBattlePassType == bBattlePassType && pkMission->dwMissionIndex == dwMissionIndex && pkMission->dwMissionType == dwMissionType && pkMission->dwBattlePassId == bBattlePassID)
			{
				pkMission->bIsUpdated = 1;
				pkMission->bCompleted = 0;

				pkMission->dwExtraInfo = dwUpdateValue;

				if (pkMission->dwExtraInfo >= dwSecondInfo)
				{
					pkMission->dwExtraInfo = dwSecondInfo;
					pkMission->bCompleted = 1;

					std::string stMissionName = CBattlePassManager::instance().GetMissionNameByType(pkMission->dwMissionType);
					std::string stBattlePassName = CBattlePassManager::instance().GetNormalBattlePassNameByID(pkMission->dwBattlePassId);

					CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, dwMissionType, bBattlePassID);
					if (bBattlePassType == 1)
					{
						EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
						LocaleChatPacket(CHAT_TYPE_NOTICE, 343, "");
					}
					if (bBattlePassType == 2)
					{
						EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
						LocaleChatPacket(CHAT_TYPE_NOTICE, 343, "");
					}

					TPacketGCExtBattlePassMissionUpdate packet;
					packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
					packet.bBattlePassType = bBattlePassType;
					packet.bMissionIndex = dwMissionIndex;
					packet.dwNewProgress = pkMission->dwExtraInfo;
					GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
				}

				dwSaveProgress = pkMission->dwExtraInfo;
				foundMission = true;

				if (pkMission->bCompleted != 1)
				{
					TPacketGCExtBattlePassMissionUpdate packet;
					packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
					packet.bBattlePassType = bBattlePassType;
					packet.bMissionIndex = dwMissionIndex;
					packet.dwNewProgress = dwSaveProgress;
					GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
				}
				break;
			}
		}

		if (!foundMission)
		{
			if (!IsExtBattlePassRegistered(bBattlePassType, bBattlePassID))
				DBManager::instance().DirectQuery("INSERT INTO player.battlepass_playerindex SET player_id=%d, player_name='%s', battlepass_type=%d, battlepass_id=%d, start_time=NOW()", GetPlayerID(), GetName(), bBattlePassType, bBattlePassID);

			TPlayerExtBattlePassMission* newMission = new TPlayerExtBattlePassMission;
			newMission->dwPlayerId = GetPlayerID();
			newMission->dwBattlePassType = bBattlePassType;
			newMission->dwMissionType = dwMissionType;
			newMission->dwBattlePassId = bBattlePassID;

			if (dwUpdateValue >= dwSecondInfo)
			{
				newMission->dwMissionIndex = dwMissionIndex;
				newMission->dwExtraInfo = dwSecondInfo;
				newMission->bCompleted = 1;

				CBattlePassManager::instance().BattlePassRewardMission(this, bBattlePassType, dwMissionType, bBattlePassID);
				if (bBattlePassType == 1)
				{
					EffectPacket(SE_EFFECT_BP_NORMAL_MISSION_COMPLETED);
					LocaleChatPacket(CHAT_TYPE_NOTICE, 343, "");
				}
				if (bBattlePassType == 2)
				{
					EffectPacket(SE_EFFECT_BP_PREMIUM_MISSION_COMPLETED);
					LocaleChatPacket(CHAT_TYPE_NOTICE, 343, "");
				}

				dwSaveProgress = dwSecondInfo;
			}
			else
			{
				newMission->dwMissionIndex = dwMissionIndex;
				newMission->dwExtraInfo = dwUpdateValue;
				newMission->bCompleted = 0;

				dwSaveProgress = dwUpdateValue;
			}

			newMission->bIsUpdated = 1;

			m_listExtBattlePass.push_back(newMission);

			TPacketGCExtBattlePassMissionUpdate packet;
			packet.bHeader = HEADER_GC_EXT_BATTLE_PASS_MISSION_UPDATE;
			packet.bBattlePassType = bBattlePassType;
			packet.bMissionIndex = dwMissionIndex;
			packet.dwNewProgress = dwSaveProgress;
			GetDesc()->Packet(&packet, sizeof(TPacketGCExtBattlePassMissionUpdate));
		}
	}
}
#endif

#ifdef ENABLE_RENEWAL_SPECIAL_CHAT
void CHARACTER::SendPickupItemPacket(int item_vnum)
{
	if (!GetDesc())
		return;

	TPacketGCPickupItemSC PickupItemGC;
	PickupItemGC.bHeader = HEADER_GC_PICKUP_ITEM_SC;
	PickupItemGC.item_vnum = item_vnum;

	GetDesc()->Packet(&PickupItemGC, sizeof(TPacketGCPickupItemSC));
}
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
void CHARACTER::SetSkillColor(DWORD* dwSkillColor)
{
	memcpy(m_dwSkillColor, dwSkillColor, sizeof(m_dwSkillColor));
	UpdatePacket();
}
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
void CHARACTER::SetProtectTime(const std::string& flagname, int value)
{
	itertype(m_protection_Time) it = m_protection_Time.find(flagname);
	if (it != m_protection_Time.end())
		it->second = value;
	else
		m_protection_Time.insert(make_pair(flagname, value));
}

int CHARACTER::GetProtectTime(const std::string& flagname) const
{
	itertype(m_protection_Time) it = m_protection_Time.find(flagname);
	if (it != m_protection_Time.end())
		return it->second;
	return 0;
}

bool CHARACTER::CheckWindows(bool bChat)
{
	if (GetExchange() || GetShop() || GetSafebox() || GetOfflineShop() || GetOfflineShopPanel())
	{
		if(bChat)
			LocaleChatPacket(CHAT_TYPE_INFO, 74, "");
		return true;
	}
	return false;
}

bool CHARACTER::CanAddItemShop()
{
	time_t now = get_global_time();
	if (GetProtectTime("offlineshop.additem") > get_global_time())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.additem") - now);
		return true;
	}
	SetProtectTime("offlineshop.additem", now + 1);

	if (m_pkTimedEvent)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		event_cancel(&m_pkTimedEvent);
		sys_log(0, "COfflineShopManager::AddItem: adding item with timed event xD %s:%d", GetName(), GetPlayerID());
		return true;
	}
	else if (!GetOfflineShop())
	{
		sys_log(0, "COfflineShopManager::AddItem: adding item without panel <?> %s:%d", GetName(), GetPlayerID());
		return true;
	}
	return false;
}

bool CHARACTER::CanDestroyShop()
{
	time_t now = get_global_time();
	if (GetProtectTime("offlineshop.destroy") > get_global_time())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.destroy") - now);
		return true;
	}

	SetProtectTime("offlineshop.destroy", now + 3);
	if (!GetOfflineShop())
		return true;

	if (m_pkTimedEvent)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		event_cancel(&m_pkTimedEvent);
		return true;
	}

	return false;
}

bool CHARACTER::CanRemoveItemShop()
{
	time_t now = get_global_time();
	if (GetProtectTime("offlineshop.remove") > get_global_time())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.remove") - now);
		return true;
	}

	SetProtectTime("offlineshop.remove", now + 1);

	if (!GetOfflineShop())
		return true;

	if (m_pkTimedEvent)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		event_cancel(&m_pkTimedEvent);
		return true;
	}

	return false;
}

bool CHARACTER::CanCreateShop()
{
	if (!GetOfflineShopPanel())
		return true;

	SetOfflineShopPanel(false);
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.create") > time)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.create") - time);
		return true;
	}
	SetProtectTime("offlineshop.create", time + 10);

	if (m_pkTimedEvent)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		event_cancel(&m_pkTimedEvent);
		return true;
	}

	if (GetGold() < 2000000)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 79, "%d", 2000000);
		return true;
	}
	else if (GetDungeon())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		return true;
	}
	else if (COfflineShopManager::Instance().HasOfflineShop(this) == true)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 80, "");
		return true;
	}

	switch(GetMapIndex())
	{
		case 231:
			return false;

		default:
			LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
			return true;
	}
	return true;
}

bool CHARACTER::CanRemoveLogShop()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.log") > time)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.log") - time);
		return true;
	}
	SetProtectTime("offlineshop.log", time + 30);

	return false;
}

bool CHARACTER::CanWithdrawMoney()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.money") > time)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.money") - time);
		return true;
	}
	SetProtectTime("offlineshop.money", time + 5);

	return false;
}

bool CHARACTER::CanOpenShopPanel()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.panel") > time)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.panel") - time);
		return true;
	}
	SetProtectTime("offlineshop.panel", time + 5);

	if (CheckWindows(true))
		return true;

	return false;
}

bool CHARACTER::CanOpenOfflineShop()
{
	if (m_pkTimedEvent)
		return true;

	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.offlineshop") > time)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.offlineshop") - time);
		return true;
	}
	SetProtectTime("offlineshop.offlineshop", time + 1);

	if (CheckWindows(true))
		return true;

	return false;
}

bool CHARACTER::CanChangeTitle()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.title") > time)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.title") - time);
		return true;
	}
	SetProtectTime("offlineshop.title", time + 10);

	if (m_pkTimedEvent)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		event_cancel(&m_pkTimedEvent);
		return true;
	}

	if (!FindAffect(AFFECT_DECORATION))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 81, "");
		return true;
	}

	return false;
}

bool CHARACTER::CanBuyItemOfflineShop()
{
	if (m_pkTimedEvent)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		event_cancel(&m_pkTimedEvent);
		return true;
	}

	return false;
}
bool CHARACTER::CanChangeDecoration()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.decoration") > time)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.decoration") - time);
		return true;
	}
	SetProtectTime("offlineshop.decoration", time + 10);

	if (m_pkTimedEvent)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		event_cancel(&m_pkTimedEvent);
		return true;
	}

	if (!FindAffect(AFFECT_DECORATION))
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 81, "");
		return true;
	}

	return false;
}

bool CHARACTER::CanGetBackItems()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.back") > time)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.back") - time);
		return true;
	}
	SetProtectTime("offlineshop.back", time + 5);

	if (m_pkTimedEvent)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		event_cancel(&m_pkTimedEvent);
		return true;
	}

	return false;
}

bool CHARACTER::CanAddTimeShop()
{
	time_t time = get_global_time();
	if (GetProtectTime("offlineshop.time") > time)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.time") - time);
		return true;
	}
	SetProtectTime("offlineshop.time", time + 10);

	if (m_pkTimedEvent)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 78, "");
		event_cancel(&m_pkTimedEvent);
		return true;
	}

	if (GetGold() < 20000000)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 79, "%d", 2000000);
		return true;
	}

	return false;
}
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
bool CHARACTER::CanSearch()
{
	time_t now = get_global_time();
	if (GetProtectTime("offlineshop.search") > get_global_time())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 77, "%d", GetProtectTime("offlineshop.search") - now);
		return true;
	}
	SetProtectTime("offlineshop.search", now + 1);

	return false;
}

bool CHARACTER::IsLookingSearchItem(DWORD itemID)
{
	return (std::find(m_vecSearchLooking.begin(), m_vecSearchLooking.end(), itemID) != m_vecSearchLooking.end());
}

void CHARACTER::SetLookingSearch(const std::vector<const OFFLINE_SHOP_ITEM*>& dataVector)
{
	m_vecSearchLooking.clear();
	for (WORD j = 0; j < dataVector.size(); ++j)
		m_vecSearchLooking.emplace_back(dataVector[j]->id);
}
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
bool CHARACTER::CheckItemCanGet(const LPITEM item)
{
	const BYTE itemType = item->GetType();
	const BYTE itemSubType = item->GetSubType();

	if(IS_SET(dwPickUPMode, AUTOMATIC_PICK_UP_YANG) && item->GetVnum() == 1)
		return false;
	else if(IS_SET(dwPickUPMode, AUTOMATIC_PICK_UP_WEAPON) && (itemType == ITEM_WEAPON && itemSubType != WEAPON_ARROW))
		return false;
	else if(IS_SET(dwPickUPMode, AUTOMATIC_PICK_UP_ARMOR) && (itemType == ITEM_ARMOR && itemSubType == ARMOR_BODY))
		return false;
	else if(IS_SET(dwPickUPMode, AUTOMATIC_PICK_UP_SHIELD) && (itemType == ITEM_ARMOR && itemSubType == ARMOR_SHIELD))
		return false;
	else if(IS_SET(dwPickUPMode, AUTOMATIC_PICK_UP_HELMETS) && (itemType == ITEM_ARMOR && itemSubType == ARMOR_HEAD))
		return false;
	else if(IS_SET(dwPickUPMode, AUTOMATIC_PICK_UP_BRACELETS) && (itemType == ITEM_ARMOR && itemSubType == ARMOR_WRIST))
		return false;
	else if(IS_SET(dwPickUPMode, AUTOMATIC_PICK_UP_NECKLACE) && (itemType == ITEM_ARMOR && itemSubType == ARMOR_NECK))
		return false;
	else if(IS_SET(dwPickUPMode, AUTOMATIC_PICK_UP_EARRINGS) && (itemType == ITEM_ARMOR && itemSubType == ARMOR_EAR))
		return false;
	else if(IS_SET(dwPickUPMode, AUTOMATIC_PICK_UP_SHOES) && (itemType == ITEM_ARMOR && itemSubType == ARMOR_FOOTS))
		return false;
	else if(IS_SET(dwPickUPMode, AUTOMATIC_PICK_UP_OTHERS) && itemType == ITEM_NONE)
		return false;
	else if(IS_SET(dwPickUPMode, AUTOMATIC_PICK_UP_CHESTS) && itemType == ITEM_GIFTBOX)
		return false;
	return true;
}

void CHARACTER::ChangePickUPMode(DWORD dwMode)
{
	dwPickUPMode = dwMode;
	ChatPacket(CHAT_TYPE_COMMAND, "PickUPMode %u", dwPickUPMode);
}
#endif

#ifdef ENABLE_OFFLINE_MESSAGE
void CHARACTER::SendOfflineMessage(const char* To, const char* Message)
{
	if (!GetDesc())
		return;

	if (strlen(To) < 1)
		return;

	TPacketGDSendOfflineMessage p;
	strlcpy(p.szFrom, GetName(), sizeof(p.szFrom));
	strlcpy(p.szTo, To, sizeof(p.szTo));
	strlcpy(p.szMessage, Message, sizeof(p.szMessage));
	db_clientdesc->DBPacket(HEADER_GD_SEND_OFFLINE_MESSAGE, GetDesc()->GetHandle(), &p, sizeof(p));

	SetLastOfflinePMTime();
}

void CHARACTER::ReadOfflineMessages()
{
	if (!GetDesc())
		return;

	TPacketGDReadOfflineMessage p;
	strlcpy(p.szName, GetName(), sizeof(p.szName));
	db_clientdesc->DBPacket(HEADER_GD_REQUEST_OFFLINE_MESSAGES, GetDesc()->GetHandle(), &p, sizeof(p));
}
#endif

#ifdef ENABLE_RENEWAL_SKILL_SELECT
void CHARACTER::RenewalSkillSelect()
{
	if (GetSkillGroup() != 0 || GetLevel() < 5)
		return;

	const char* job = "";

	switch(GetJob())
	{
		case JOB_WARRIOR:
			job = "JOB_WARRIOR";
			break;

		case JOB_ASSASSIN:
			job = "JOB_ASSASSIN";
			break;

		case JOB_SURA:
			job = "JOB_SURA";
			break;

		case JOB_SHAMAN:
			job = "JOB_SHAMAN";
			break;

		default:
			sys_err("[RenewalSkillSelect]: Invalid job %d", GetJob());
			break;
	}

	ChatPacket(CHAT_TYPE_COMMAND, "RenewalSkillSelectWindow %s", job);
}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
bool CHARACTER::SetGrowthPet(LPGROWTH_PET pkPet)
{
	auto it = m_growthPetMap.find(pkPet->GetPetID());

	if (it != m_growthPetMap.end())
		m_growthPetMap.erase(it);

	if (pkPet->GetOwner() != this)
		pkPet->SetOwner(this);

	m_growthPetMap.emplace(pkPet->GetPetID(), pkPet);

	TPacketGCPetSet packet;
	packet.header = HEADER_GC_PET_SET;
	packet.dwID = pkPet->GetPetID();
	packet.dwSummonItemVnum = pkPet->GetSummonItemVnum();

	strlcpy(packet.szName, pkPet->GetPetName().c_str(), sizeof(packet.szName));
	thecore_memcpy(packet.aSkill, pkPet->GetPetSkill(), sizeof(packet.aSkill));

	for (int i = 0; i < POINT_UPBRINGING_MAX_NUM; i++)
		packet.dwPoints[i] = pkPet->GetPetPoint(i);

	if(GetDesc())
		GetDesc()->Packet(&packet, sizeof(packet));

	return true;
}

bool CHARACTER::DeleteGrowthPet(DWORD dwID)
{
	auto it = m_growthPetMap.find(dwID);

	if (it == m_growthPetMap.end())
		return false;

	m_growthPetMap.erase(it);

	TPacketGCPetDelete packet;
	packet.header = HEADER_GC_PET_DEL;
	packet.dwID = dwID;

	if (GetDesc())
		GetDesc()->Packet(&packet, sizeof(packet));

	return true;
}

LPGROWTH_PET CHARACTER::GetGrowthPet(DWORD dwID)
{
	auto it = m_growthPetMap.find(dwID);

	if (it != m_growthPetMap.end())
		return it->second;

	return nullptr;
}

void CHARACTER::ClearGrowthPet()
{
	if (m_activeGrowthPet)
		m_activeGrowthPet->Unsummon();

	if (m_growthPetMap.size())
	{
		auto it = m_growthPetMap.begin();

		while (it != m_growthPetMap.end())
		{
			LPGROWTH_PET pPet = it->second;

			pPet->Save();

			++it;

			CGrowthPetManager::Instance().DeleteGrowthPet(pPet->GetPetID());
		}

		m_growthPetMap.clear();
	}
}
#endif

#ifdef ENABLE_SHIP_DEFENCE_DUNGEON
bool CHARACTER::IsHydraMob() const
{
	switch(GetRaceNum())
	{
		case 6802:
		case 6803:
		case 6804:
		case 6805:
		case 6806:
		case 6807:
		case 6808:
		case 6809:
		case 6812:
			return true;
	}
	return false;
}

bool CHARACTER::IsHydraMobLP(LPCHARACTER ch) const
{
	switch(ch->GetRaceNum())
	{
		case 6802:
		case 6803:
		case 6804:
		case 6805:
		case 6806:
		case 6807:
		case 6808:
		case 6809:
		case 6812:
			return true;
	}
	return false;
}

bool CHARACTER::IsHydraNPC() const
{
	switch(GetRaceNum())
	{
		case 6801:
			return true;
	}
	return false;
}

bool CHARACTER::IsHydra() const
{
	switch(GetRaceNum())
	{
		case 6812:
			return true;
	}
	return false;
}
#endif

#ifdef ENABLE_RENEWAL_TELEPORT_SYSTEM
int teleportMap[][5] = {
	{0,	0, 0, 0, 0},												// unused 							>>>>>	index: 0

	{469300,	964200,		0,		0,				0},				// Map1-Red							>>>>>	index: 1
	{353100,	882900,		0,		0,				0},				// Map2-Red							>>>>>	index: 2
	{55700,		157900,		0,		0,				0},				// Map1-Yellow						>>>>>	index: 3
	{145700,	240000,		0,		0,				0},				// Map2-Yellow						>>>>>	index: 4
	{969600,	278400,		0,		0,				0},				// Map1-Blue						>>>>>	index: 5
	{863900,	245700,		0,		0,				0},				// Map2-Blue						>>>>>	index: 6

	{332600,	746800,		0,		0,				0},				// Valley of Seungryong				>>>>>	index: 7
	{295600,	548100,		0,		0,				0},				// Yongbi Desert					>>>>>	index: 8
	{434700,	214200,		0,		0,				0},				// Mount Sohan						>>>>>	index: 9
	{601900,	684200,		0,		0,				0},				// Fireland							>>>>>	index: 10
	{288700,	5700,		0,		0,				0},				// Ghost Forest						>>>>>	index: 11
	{1119900,	70800,		0,		0,				0},				// Red Forest						>>>>>	index: 12

	{60000,		497000,		0,		0,				0},				// V1								>>>>>	index: 13
	{703800,	462500,		40,		0,				71095},			// V2								>>>>>	index: 14
	{97800,		571100,		40,		0,				71095},			// SD3				 				>>>>>	index: 15
	{90000,		1207800,	75,		0,				30190},			// V3								>>>>>	index: 16
	{241300,	1275400,	75,		0,				30190},			// V4								>>>>>	index: 17

	{828200,	763400,		0,		0,				0},				// Land of Giants					>>>>>	index: 18

	{1104700,	1786500,	90,		5000000,		0},				// Cape Dragon Fire					>>>>>	index: 19
	{1177600,	1664000,	90,		5000000,		0},				// Gautama Cliff					>>>>>	index: 20
	{1089600,	1650400,	90,		5000000,		0},				// Nephrite Bay						>>>>>	index: 21
	{1126400,	1510400,	90,		5000000,		0},				// Thunder Mountains				>>>>>	index: 22

	{9193300,	9242600,	0,		0,				0},				// Trade							>>>>>	index: 23
};

void CHARACTER::StartWarpOn(BYTE map_index)
{
	if (teleportMap[map_index][2] != 0)
	{
		if (GetLevel() < teleportMap[map_index][2])
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 735, "");
			return;
		}
	}

	if (teleportMap[map_index][3] != 0)
	{
		if (GetGold() < teleportMap[map_index][3])
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 79, "%d", teleportMap[map_index][3]);
			return;
		}
	}

	if (teleportMap[map_index][4] != 0)
	{
		if (CountSpecifyItem(teleportMap[map_index][4]) < 1)
		{
			LocaleChatPacket(CHAT_TYPE_INFO, 736, "");
			return;
		}
	}

	RemoveSpecifyItem(teleportMap[map_index][4], 1);
	SetGold(GetGold() - teleportMap[map_index][3]);
	WarpSet(teleportMap[map_index][0], teleportMap[map_index][1]);
}
#endif

#ifdef ENABLE_DUNGEON_TRACKING_SYSTEM
void CHARACTER::GetDungeonCooldownTest(WORD bossIndex, int value, bool isCooldown)
{
	const std::map<WORD, std::string> m_vecQuestCooldowns = {
		{1093, isCooldown ? "deviltower_zone.cooldown" : "deviltower_zone.time"},
		{2598, isCooldown ? "devilcatacomb_zone.cooldown" : "devilcatacomb_zone.time"},
		{2493, isCooldown ? "dragonlair.cooldown" : "dragonlair.time"},
		{6091, isCooldown ? "flame_dungeon.cooldown" : "flame_dungeon.time"},
		{9018, isCooldown ? "shipdefense.cooldown" : "shipdefense.time"},
		{6191, isCooldown ? "snow_dungeon.cooldown" : "snow_dungeon.time"},
		{20442, isCooldown ? "meley_dungeon.cooldown" : "meley_dungeon.time"},
		{2092, isCooldown ? "spider_dungeon.cooldown" : "spider_dungeon.time"},
	};

	const auto it = m_vecQuestCooldowns.find(bossIndex);
	if (it != m_vecQuestCooldowns.end())
	{
		SetQuestFlag(it->second.c_str(), time(0) + value);
		GetDungeonCooldown(bossIndex);
	}
}

void CHARACTER::GetDungeonCooldown(WORD bossIndex)
{
	const std::map<WORD, std::pair<std::string, std::string>> m_vecQuestCooldowns = {
		{1093, {"deviltower_zone.cooldown", "deviltower_zone.time"}},
		{2598, {"devilcatacomb_zone.cooldown", "devilcatacomb_zone.time"}},
		{2493, {"dragonlair.cooldown", "dragonlair.time"}},
		{6091, {"flame_dungeon.cooldown", "flame_dungeon.time"}},
		{9018, {"shipdefense.cooldown", "shipdefense.time"}},
		{6191, {"snow_dungeon.cooldown", "snow_dungeon.time"}},
		{20442, {"meley_dungeon.cooldown", "meley_dungeon.time"}},
		{2092, {"spider_dungeon.cooldown", "spider_dungeon.time"}},
	};

	std::string cmdText("");
	if (bossIndex == WORD_MAX)
	{
#if __cplusplus < 199711L
		for (const auto& [bossVnum, cooldown] : m_vecQuestCooldowns)
		{
			const int leftTime = GetQuestFlag(cooldown.c_str()) - time(0);
#else
		for (auto it = m_vecQuestCooldowns.begin(); it != m_vecQuestCooldowns.end(); ++it)
		{
			const WORD bossVnum = it->first;
			int leftTime = GetQuestFlag(it->second.first.c_str()) - time(0);
#endif
			if (leftTime > 0)
			{
				cmdText += std::to_string(bossVnum);
				cmdText += "|";
				cmdText += std::to_string(leftTime);
				cmdText += "|";
				cmdText += "0";
				cmdText += "#";

				if (cmdText.length() + 50 > CHAT_MAX_LEN)
				{
					ChatPacket(CHAT_TYPE_COMMAND, "TrackDungeonInfo %s", cmdText.c_str());
					cmdText.clear();
				}
				continue;
			}

			leftTime = GetQuestFlag(it->second.second.c_str()) - time(0);
			if (leftTime > 0)
			{
				cmdText += std::to_string(bossVnum);
				cmdText += "|";
				cmdText += std::to_string(leftTime);
				cmdText += "|";
				cmdText += "1";
				cmdText += "#";

				if (cmdText.length() + 50 > CHAT_MAX_LEN)
				{
					ChatPacket(CHAT_TYPE_COMMAND, "TrackDungeonInfo %s", cmdText.c_str());
					cmdText.clear();
				}
				continue;
			}
		}
	}
	else
	{
		const auto it = m_vecQuestCooldowns.find(bossIndex);
		if (it != m_vecQuestCooldowns.end())
		{
			int leftTime = GetQuestFlag(it->second.first.c_str()) - time(0);
			if (leftTime > 0)
			{
				cmdText += std::to_string(it->first);
				cmdText += "|";
				cmdText += std::to_string(leftTime);
				cmdText += "|";
				cmdText += "0";
				cmdText += "#";
			}

			leftTime = GetQuestFlag(it->second.second.c_str()) - time(0);
			if (leftTime > 0)
			{
				cmdText += std::to_string(it->first);
				cmdText += "|";
				cmdText += std::to_string(leftTime);
				cmdText += "|";
				cmdText += "1";
				cmdText += "#";
			}
		}
	}

	if (cmdText.length())
		ChatPacket(CHAT_TYPE_COMMAND, "TrackDungeonInfo %s", cmdText.c_str());
}
#endif

#ifdef ENABLE_GUILD_TOKEN_AUTH
bool CHARACTER::IsGuildMaster() const
{
	const auto guild = GetGuild();
	return (guild && GetPlayerID() == guild->GetMasterPID());
}

uint64_t CHARACTER::GetGuildToken() const
{
	const auto guild = GetGuild();
	if (guild && IsGuildMaster())
		return guild->GetToken();
	return 0;
}

void CHARACTER::SendGuildToken()
{
	CGuildManager::instance().SendGuildToken(this, GetGuildToken());
}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
void CHARACTER::SetTransmutation(CTransmutation* c)
{
	if (m_pkTransmutation != nullptr)
		delete m_pkTransmutation;

	m_pkTransmutation = c;
}

CTransmutation* CHARACTER::GetTransmutation() const
{
	return m_pkTransmutation;
}
#endif

#ifdef ENABLE_BOT_PLAYER
BYTE CHARACTER::GetBotPlayerComboIndex() const
{
	if (!GetWear(WEAR_WEAPON))
		return 0;

	if (GetComboSequence() > 0 && GetComboSequence() <= 3)
	{
		if (get_dword_time() - GetLastComboTime() <= GetValidComboInterval())
			return GetComboSequence();
	}
	return 0;
}
#endif

#ifdef __AUTO_HUNT__
void CHARACTER::SetAutoHuntStatus(bool bStatus, bool bMobFarm, bool bMetinFarm)
{
	m_bAutoHuntStatus = bStatus;
	RemoveAffect(AFFECT_AUTO_HUNT_AFFECT);
	if (bStatus)
		AddAffect(AFFECT_AUTO_HUNT_AFFECT, POINT_NONE, 0, AFF_AUTO_HUNT, INFINITE_AFFECT_DURATION, 0, false);
	ChatPacket(CHAT_TYPE_COMMAND, "AutoHuntStatus %d %d %d", bStatus, bMobFarm, bMetinFarm);
}

void CHARACTER::GetAutoHuntCommand(const char* szArgument)
{
	m_vecAutoHuntItems.clear();

	std::vector<std::string> vecArgs;
	split_argument(szArgument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "end")
	{
		SetAutoHuntStatus(false);
#ifdef __AUTO_HUNT__
		// AUTO_HUNT sonlandýrýldýðýnda tüm rezervasyonlarý kaldýr
		if (IsPC() && GetPlayerID())
		{
			CHARACTER_MANAGER::instance().ReleaseAllTargetsByPlayer(GetPlayerID());
		}
#endif
	}
	else if (vecArgs[1] == "start")
	{
		if (vecArgs.size() < 7) { return; }
		if (!IsAutoHuntAffectHas())
		{
			ChatPacket(CHAT_TYPE_INFO, "You don't have auto hunt affect.");
			return;
		}
		if (vecArgs[2] != "empty")
		{
			std::vector<std::string> vecArgsEx;
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
			split_argument_special(vecArgs[2].c_str(), vecArgsEx, "?");
#else
			// Manuel string split (ENABLE_OFFLINESHOP_SEARCH_SYSTEM tanýmlý deðilse)
			std::string str = vecArgs[2];
			std::string delimiter = "?";
			size_t pos = 0;
			std::string token;
			while ((pos = str.find(delimiter)) != std::string::npos)
			{
				token = str.substr(0, pos);
				if (!token.empty())
					vecArgsEx.push_back(token);
				str.erase(0, pos + delimiter.length());
			}
			if (!str.empty())
				vecArgsEx.push_back(str);
#endif
			for (BYTE j = 0; j < vecArgsEx.size(); ++j)
			{
				if (vecArgsEx[j] != "1")
					continue;
				m_vecAutoHuntItems.emplace_back(j, 0);
			}
		}
		SetAutoHuntStatus(true, vecArgs[5] == "1" ? true : false, vecArgs[6] == "1" ? true : false);
	}
	ChatPacket(CHAT_TYPE_COMMAND, "auto_hunt %s", szArgument);
}
bool CHARACTER::IsAutoHuntAffectHas()
{
	return FindAffect(AFFECT_AUTO_HUNT) != NULL ? true : false;
}
#endif

#ifdef ENABLE_AUTO_SELL_SYSTEM
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void CHARACTER::LoadAutoSellItemsFromJson()
{
	if (m_isAutoSellItemsLoaded)
		return;

	char file_name[256];
	int result = snprintf(file_name, sizeof(file_name), "%s/auto_sell/%s.json", 
		LocaleService_GetBasePath().c_str(), 
		GetName());

	if (result >= sizeof(file_name))
		return;

	std::ifstream file(file_name);
	if (file.is_open())
	{
		try {
			json jsonData;
			file >> jsonData;
			
			m_vecAutoSellItems.clear();
			if (jsonData.contains("items") && jsonData["items"].is_array())
			{
				for (const auto& item : jsonData["items"])
				{
					m_vecAutoSellItems.push_back(item.get<DWORD>());
				}
			}
			
			m_bAutoSellStatus = jsonData.value("status", false);
			m_isAutoSellItemsLoaded = true;
		}
		catch (const std::exception& e) {
			sys_err("Error loading autosell data for %s: %s", GetName(), e.what());
			m_vecAutoSellItems.clear();
			m_bAutoSellStatus = false;
		}
	}
	else
	{
		std::ofstream createFile(file_name);
		if (createFile.is_open())
		{
			try {
				json defaultJson;
				defaultJson["items"] = json::array();
				defaultJson["status"] = false;
				createFile << defaultJson.dump(4);
			}
			catch (const std::exception& e) {
				sys_err("Error creating autosell file for %s: %s", GetName(), e.what());
			}
		}

		m_vecAutoSellItems.clear();
		m_bAutoSellStatus = false;
		m_isAutoSellItemsLoaded = true;
	}
}

void CHARACTER::SaveAutoSellItemsToJson()
{
	char file_name[256];
	snprintf(file_name, sizeof(file_name), "%s/auto_sell/%s.json", 
		LocaleService_GetBasePath().c_str(), 
		GetName());

	try {
		json jsonData;

		if (m_vecAutoSellItems.empty() && !m_bAutoSellStatus)
		{
			remove(file_name);
			return;
		}

		jsonData["items"] = m_vecAutoSellItems;
		jsonData["status"] = m_bAutoSellStatus;

		std::ofstream file(file_name);
		if (file.is_open())
		{
			file << jsonData.dump(4);
		}
	}
	catch (const std::exception& e) {
		sys_err("Error saving autosell data for %s: %s", GetName(), e.what());
	}
}

void CHARACTER::SendItemProcessInfo()
{
	if (!GetDesc())
		return;

	if (!m_isAutoSellItemsLoaded)
		LoadAutoSellItemsFromJson();

	// ChatPacket(CHAT_TYPE_COMMAND, "clearAutoSellInfo");
	
	for (const auto& vnum : m_vecAutoSellItems)
	{
		ChatPacket(CHAT_TYPE_COMMAND, "addAutoSellInfo %u", vnum);
	}
	
	ChatPacket(CHAT_TYPE_COMMAND, "autosell_status %d", m_bAutoSellStatus);
}

void CHARACTER::AddAutoSellItem(DWORD dwVnum)
{
	if (m_vecAutoSellItems.size() >= MAX_AUTO_SELL_ITEMS)
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 790, "");
		//ChatPacket(CHAT_TYPE_INFO, LC_TEXT("oto_sat_max_sayi"));
		return;
	}

	if (std::find(m_vecAutoSellItems.begin(), m_vecAutoSellItems.end(), dwVnum) != m_vecAutoSellItems.end())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 791, "");
		//ChatPacket(CHAT_TYPE_INFO, LC_TEXT("oto_sat_zaten_listede_mevcut"));
		return;
	}

	m_vecAutoSellItems.push_back(dwVnum);
	SaveAutoSellItemsToJson();
	LocaleChatPacket(CHAT_TYPE_INFO, 792, "");
	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("oto_sat_zaten_listeye_eklendi"));
}

void CHARACTER::RemoveAutoSellItem(DWORD dwVnum)
{
	auto it = std::find(m_vecAutoSellItems.begin(), m_vecAutoSellItems.end(), dwVnum);
	if (it != m_vecAutoSellItems.end())
	{
		m_vecAutoSellItems.erase(std::remove(m_vecAutoSellItems.begin(), m_vecAutoSellItems.end(), dwVnum), m_vecAutoSellItems.end());
		SaveAutoSellItemsToJson();
		LocaleChatPacket(CHAT_TYPE_INFO, 793, "");
		//ChatPacket(CHAT_TYPE_INFO, LC_TEXT("oto_sat_listeden_kaldirildi"));
	}
}

void CHARACTER::RemoveAllAutoSellItem()
{
	if (m_vecAutoSellItems.empty())
	{
		LocaleChatPacket(CHAT_TYPE_INFO, 794, "");
		//ChatPacket(CHAT_TYPE_INFO, LC_TEXT("oto_sat_liste_zaten_bos"));
		return;
	}

	m_vecAutoSellItems.clear();
	SaveAutoSellItemsToJson();

	m_isAutoSellItemsLoaded = true;

	LocaleChatPacket(CHAT_TYPE_INFO, 795, "");
	//ChatPacket(CHAT_TYPE_INFO, LC_TEXT("oto_sat_tum_oge_kaldirildi"));
}


bool CHARACTER::IsAutoSellItem(DWORD dwVnum) const
{
	return std::find(m_vecAutoSellItems.begin(), m_vecAutoSellItems.end(), dwVnum) != m_vecAutoSellItems.end();
}

void CHARACTER::SetAutoSellStatus(bool bStatus)
{
	m_bAutoSellStatus = bStatus;
	ChatPacket(CHAT_TYPE_INFO, "Otomatik Sat: %s", bStatus ? "Aktif" : "Pasif");
}
#endif
