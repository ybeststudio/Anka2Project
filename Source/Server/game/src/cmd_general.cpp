#include "stdafx.h"
#ifdef __FreeBSD__
#include <md5.h>
#else
#include "../../library/libthecore/include/xmd5.h"
#endif
#include <errno.h>
#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "item.h"
#include "arena.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "skill.h"
#include "../../common/service.h"
#include "../../common/VnumHelper.h"
#include "log.h"
#include "locale.hpp"

#ifdef ENABLE_MOUNT_SYSTEM
	#include "MountSystem.h"
#endif

#ifdef ENABLE_PET_SYSTEM
	#include "PetSystem.h"
#endif

#ifdef ENABLE_MAINTENANCE_SYSTEM
	#include "maintenance.h"
#endif

#ifdef ENABLE_QUEEN_NETHIS
#	include "SnakeLair.h"
#endif

#ifdef ENABLE_REMOTE_SHOP_SYSTEM
#include "shop.h"
#include "shop_manager.h"
#endif

#ifdef __SYSTEM_SEARCH_ITEM_MOB__
	#include <string>
	#include <boost/algorithm/string.hpp>
#endif

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
	#include "CollectionsSystem.hpp"
#endif



extern int g_server_id;

extern int g_nPortalLimitTime;

#ifdef ENABLE_PASSIVE_SYSTEM
namespace
{
	enum EPassiveRelicSockets
	{
		PASSIVE_RELIC_SOCKET_REMAIN = ITEM_SOCKET_REMAIN_SEC,
		PASSIVE_RELIC_SOCKET_ACTIVE = 1,
		PASSIVE_RELIC_SOCKET_PROC = 2,
	};

	enum EPassiveRelicMaterialIndex
	{
		PASSIVE_RELIC_MATERIAL_WEAPON,
		PASSIVE_RELIC_MATERIAL_ELEMENT,
		PASSIVE_RELIC_MATERIAL_ARMOR,
		PASSIVE_RELIC_MATERIAL_ACCE,
		PASSIVE_RELIC_MATERIAL_MAX,
	};

	struct TPassiveRelicMaterialInfo
	{
		BYTE bSubType;
		DWORD dwVnum;
	};

	struct TPassiveRelicBonusInfo
{
	BYTE bBonusType;
	WORD bApply;
		BYTE bValueCount;
		short sValues[10];
	};

	enum EPassiveRelicBonusTypes
	{
		PASSIVE_RELIC_BONUS_DIRECT,
	};

	const DWORD PASSIVE_RELIC_VNUM_MIN = 30272;
	const DWORD PASSIVE_RELIC_VNUM_MAX = 30276;
	const char* PASSIVE_RELIC_COOLDOWN_FLAG = "passive_relic.cooldown";
	const char* PASSIVE_RELIC_DECK_SELECTED_FLAG = "passive_relic.deck.selected";
	const char* PASSIVE_RELIC_DECK_INIT_FLAG = "passive_relic.deck.init";
	const char* PASSIVE_RELIC_DECK_ATTR_TYPE_FLAG_FMT = "passive_relic.deck%d.attr.type%d";
	const char* PASSIVE_RELIC_DECK_ATTR_VALUE_FLAG_FMT = "passive_relic.deck%d.attr.value%d";
	const char* PASSIVE_RELIC_DECK_PROC_STONE_FLAG_FMT = "passive_relic.deck%d.proc.stone";
	const char* PASSIVE_RELIC_DECK_PROC_DISMOUNT_FLAG_FMT = "passive_relic.deck%d.proc.dismount";
	const BYTE PASSIVE_RELIC_MAX_BONUS_COUNT = 4;
	const int PASSIVE_RELIC_DIRECT_UNEQUIP_SUCCESS_CHANCE = 20;
	const int PASSIVE_RELIC_FALLBACK_DURATION = 60 * 60 * 24 * 7;

	enum EPassiveRelicDeck
	{
		PASSIVE_RELIC_DECK_EARTH = 0,
		PASSIVE_RELIC_DECK_SKY = 1,
		PASSIVE_RELIC_DECK_MAX = 2,
	};

	const TPassiveRelicMaterialInfo kPassiveRelicMaterials[PASSIVE_RELIC_MATERIAL_MAX] =
	{
		{ MATERIAL_PASSIVE_WEAPON, 30255 },
		{ MATERIAL_PASSIVE_ELEMENT, 30258 },
		{ MATERIAL_PASSIVE_ARMOR, 30256 },
		{ MATERIAL_PASSIVE_ACCE, 30257 },
	};

	// https://tr-wiki.metin2.gameforge.com/index.php/Kalynty_Sistemi
	// Bonus de?erleri resmi oyuna göre düzenledim de?i?tirmek isterseniz a?a?ydaki de?erleri de?i?tirin
	/*
	SungMa STR/RES/VIT/INT için sabit 15
	Metin savunma için 1,2,3,4,5,6,8,10
	Metin ta?yna kar?y güç için 1,2,3,4,5,6,7,8,10
	Patron drop ?ansy için 1,2,3,4,5,6,7,8,10
	Patronlara kar?y gücü için 1,2,3,4,5,6,7,8,10
	*/

	const TPassiveRelicBonusInfo kPassiveRelicBonusPool[] =
	{
		{ PASSIVE_RELIC_BONUS_DIRECT, APPLY_SUNGMA_STR, 1, { 15 } },
		{ PASSIVE_RELIC_BONUS_DIRECT, APPLY_SUNGMA_HP, 1, { 15 } },
		{ PASSIVE_RELIC_BONUS_DIRECT, APPLY_SUNGMA_MOVE, 1, { 15 } },
		{ PASSIVE_RELIC_BONUS_DIRECT, APPLY_SUNGMA_IMMUNE, 1, { 15 } },
		{ PASSIVE_RELIC_BONUS_DIRECT, APPLY_ATTBONUS_STONE, 9, { 1, 2, 3, 4, 5, 6, 7, 8, 10 } },
		{ PASSIVE_RELIC_BONUS_DIRECT, APPLY_ATTBONUS_BOSS, 9, { 1, 2, 3, 4, 5, 6, 7, 8, 10 } },
		{ PASSIVE_RELIC_BONUS_DIRECT, APPLY_ITEM_DROP_BONUS, 9, { 1, 2, 3, 4, 5, 6, 7, 8, 10 } },
	};

	LPITEM GetEquippedPassiveRelic(LPCHARACTER ch)
	{
		if (!ch)
			return NULL;

		return ch->GetWear(WEAR_PASSIVE);
	}

	bool IsPassiveRelicActive(LPITEM pkItem)
	{
		return pkItem && pkItem->GetSocket(PASSIVE_RELIC_SOCKET_ACTIVE) != 0;
	}
	int GetPassiveRelicDurationSeconds(LPITEM pkItem)
	{
		if (!pkItem)
			return 0;

		const int iDuration = pkItem->GetDuration();
		if (iDuration > 0)
			return iDuration;

		const int iRemain = pkItem->GetSocket(PASSIVE_RELIC_SOCKET_REMAIN);
		if (iRemain > 0)
			return iRemain;

		return PASSIVE_RELIC_FALLBACK_DURATION;
	}

	long GetPassiveRelicProcSocket(LPITEM pkItem)
	{
		return pkItem ? pkItem->GetSocket(PASSIVE_RELIC_SOCKET_PROC) : 0;
	}

	short GetPassiveRelicStoneDefProcValue(LPITEM pkItem)
	{
		return static_cast<short>(GetPassiveRelicProcSocket(pkItem) & 0xFFFF);
	}

	short GetPassiveRelicDismountMoveProcValue(LPITEM pkItem)
	{
		return static_cast<short>((GetPassiveRelicProcSocket(pkItem) >> 16) & 0xFFFF);
	}

	void SetPassiveRelicStoneDefProcValue(LPITEM pkItem, short sValue)
	{
		if (!pkItem)
			return;

		const long lCurrent = GetPassiveRelicProcSocket(pkItem);
		const long lUpdated = (lCurrent & 0xFFFF0000L) | (static_cast<long>(sValue) & 0xFFFFL);
		pkItem->SetSocket(PASSIVE_RELIC_SOCKET_PROC, lUpdated);
	}

	void SetPassiveRelicDismountMoveProcValue(LPITEM pkItem, short sValue)
	{
		if (!pkItem)
			return;

		const long lCurrent = GetPassiveRelicProcSocket(pkItem);
		const long lUpdated = (lCurrent & 0x0000FFFFL) | ((static_cast<long>(sValue) & 0xFFFFL) << 16);
		pkItem->SetSocket(PASSIVE_RELIC_SOCKET_PROC, lUpdated);
	}

	int GetPassiveRelicBonusCount(LPITEM pkItem)
	{
		if (!pkItem)
			return 0;

		return pkItem->GetAttributeCount();
	}

	void RefreshPassiveRelicState(LPCHARACTER ch, LPITEM pkItem);

	int ClampPassiveRelicDeckIndex(int iDeck)
	{
		if (iDeck < PASSIVE_RELIC_DECK_EARTH || iDeck >= PASSIVE_RELIC_DECK_MAX)
			return PASSIVE_RELIC_DECK_EARTH;
		return iDeck;
	}

	std::string BuildPassiveRelicDeckAttrTypeFlag(int iDeck, int iAttrIndex)
	{
		char szFlag[64];
		snprintf(szFlag, sizeof(szFlag), PASSIVE_RELIC_DECK_ATTR_TYPE_FLAG_FMT, iDeck, iAttrIndex);
		return szFlag;
	}

	std::string BuildPassiveRelicDeckAttrValueFlag(int iDeck, int iAttrIndex)
	{
		char szFlag[64];
		snprintf(szFlag, sizeof(szFlag), PASSIVE_RELIC_DECK_ATTR_VALUE_FLAG_FMT, iDeck, iAttrIndex);
		return szFlag;
	}

	std::string BuildPassiveRelicDeckProcStoneFlag(int iDeck)
	{
		char szFlag[64];
		snprintf(szFlag, sizeof(szFlag), PASSIVE_RELIC_DECK_PROC_STONE_FLAG_FMT, iDeck);
		return szFlag;
	}

	std::string BuildPassiveRelicDeckProcDismountFlag(int iDeck)
	{
		char szFlag[64];
		snprintf(szFlag, sizeof(szFlag), PASSIVE_RELIC_DECK_PROC_DISMOUNT_FLAG_FMT, iDeck);
		return szFlag;
	}

	int GetPassiveRelicSelectedDeck(LPCHARACTER ch)
	{
		if (!ch)
			return PASSIVE_RELIC_DECK_EARTH;

		return ClampPassiveRelicDeckIndex(ch->GetQuestFlag(PASSIVE_RELIC_DECK_SELECTED_FLAG));
	}

	void SetPassiveRelicSelectedDeck(LPCHARACTER ch, int iDeck)
	{
		if (!ch)
			return;

		ch->SetQuestFlag(PASSIVE_RELIC_DECK_SELECTED_FLAG, ClampPassiveRelicDeckIndex(iDeck));
	}

	void SavePassiveRelicDeckState(LPCHARACTER ch, LPITEM pkRelic, int iDeck)
	{
		if (!ch || !pkRelic)
			return;

		iDeck = ClampPassiveRelicDeckIndex(iDeck);

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			const TPlayerItemAttribute& rAttr = pkRelic->GetAttribute(i);
			ch->SetQuestFlag(BuildPassiveRelicDeckAttrTypeFlag(iDeck, i), rAttr.bType);
			ch->SetQuestFlag(BuildPassiveRelicDeckAttrValueFlag(iDeck, i), rAttr.sValue);
		}

		ch->SetQuestFlag(BuildPassiveRelicDeckProcStoneFlag(iDeck), GetPassiveRelicStoneDefProcValue(pkRelic));
		ch->SetQuestFlag(BuildPassiveRelicDeckProcDismountFlag(iDeck), GetPassiveRelicDismountMoveProcValue(pkRelic));
	}

	void ClearPassiveRelicBonuses(LPITEM pkRelic)
	{
		if (!pkRelic)
			return;

		while (pkRelic->GetAttributeCount() > 0)
		{
			if (!pkRelic->RemoveAttributeAt(pkRelic->GetAttributeCount() - 1))
				break;
		}

		SetPassiveRelicStoneDefProcValue(pkRelic, 0);
		SetPassiveRelicDismountMoveProcValue(pkRelic, 0);
	}

	void LoadPassiveRelicDeckState(LPCHARACTER ch, LPITEM pkRelic, int iDeck)
	{
		if (!ch || !pkRelic)
			return;

		iDeck = ClampPassiveRelicDeckIndex(iDeck);
		ClearPassiveRelicBonuses(pkRelic);

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			const int iType = ch->GetQuestFlag(BuildPassiveRelicDeckAttrTypeFlag(iDeck, i));
			const int iValue = ch->GetQuestFlag(BuildPassiveRelicDeckAttrValueFlag(iDeck, i));
			if (iType <= 0 || iValue == 0)
				continue;

			pkRelic->AddAttribute(static_cast<BYTE>(iType), static_cast<short>(iValue));
		}

		SetPassiveRelicStoneDefProcValue(pkRelic, static_cast<short>(ch->GetQuestFlag(BuildPassiveRelicDeckProcStoneFlag(iDeck))));
		SetPassiveRelicDismountMoveProcValue(pkRelic, static_cast<short>(ch->GetQuestFlag(BuildPassiveRelicDeckProcDismountFlag(iDeck))));
	}

	void ResetPassiveRelicDeckState(LPCHARACTER ch, int iDeck)
	{
		if (!ch)
			return;

		iDeck = ClampPassiveRelicDeckIndex(iDeck);
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			ch->SetQuestFlag(BuildPassiveRelicDeckAttrTypeFlag(iDeck, i), 0);
			ch->SetQuestFlag(BuildPassiveRelicDeckAttrValueFlag(iDeck, i), 0);
		}

		ch->SetQuestFlag(BuildPassiveRelicDeckProcStoneFlag(iDeck), 0);
		ch->SetQuestFlag(BuildPassiveRelicDeckProcDismountFlag(iDeck), 0);
	}

	void InitializePassiveRelicDeckState(LPCHARACTER ch, LPITEM pkRelic)
	{
		if (!ch || !pkRelic)
			return;

		if (ch->GetQuestFlag(PASSIVE_RELIC_DECK_INIT_FLAG) != 0)
			return;

		SavePassiveRelicDeckState(ch, pkRelic, PASSIVE_RELIC_DECK_EARTH);
		ResetPassiveRelicDeckState(ch, PASSIVE_RELIC_DECK_SKY);
		SetPassiveRelicSelectedDeck(ch, PASSIVE_RELIC_DECK_EARTH);
		ch->SetQuestFlag(PASSIVE_RELIC_DECK_INIT_FLAG, 1);
	}

	void SwitchPassiveRelicDeck(LPCHARACTER ch, LPITEM pkRelic, int iNextDeck)
	{
		if (!ch || !pkRelic)
			return;

		InitializePassiveRelicDeckState(ch, pkRelic);

		const int iCurrentDeck = GetPassiveRelicSelectedDeck(ch);
		iNextDeck = ClampPassiveRelicDeckIndex(iNextDeck);

		if (iCurrentDeck == iNextDeck)
			return;

		const bool bWasActive = pkRelic->IsEquipped() && IsPassiveRelicActive(pkRelic);
		if (bWasActive)
			pkRelic->ModifyPoints(false);

		SavePassiveRelicDeckState(ch, pkRelic, iCurrentDeck);
		LoadPassiveRelicDeckState(ch, pkRelic, iNextDeck);
		SetPassiveRelicSelectedDeck(ch, iNextDeck);

		if (bWasActive)
		{
			ch->RemoveAffect(AFFECT_PASSIVE_RELIC_STONE_DEF);
			ch->RemoveAffect(AFFECT_PASSIVE_RELIC_DISMOUNT_SPEED);
			pkRelic->ModifyPoints(true);
		}

		RefreshPassiveRelicState(ch, pkRelic);
	}

	bool CanUsePassiveRelicCommand(LPCHARACTER ch)
	{
		if (!ch || !ch->IsPC())
			return false;

		if (ch->IsDead() || ch->IsStun())
			return false;

		if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot use the relic right now.");
			return false;
		}

		if (ch->GetQuestFlag(PASSIVE_RELIC_COOLDOWN_FLAG) > get_global_time())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Please wait a moment and try again.");
			return false;
		}

		return true;
	}

	void TouchPassiveRelicCooldown(LPCHARACTER ch)
	{
		if (ch)
			ch->SetQuestFlag(PASSIVE_RELIC_COOLDOWN_FLAG, get_global_time() + 1);
	}

	void RefreshPassiveRelicState(LPCHARACTER ch, LPITEM pkItem)
	{
		if (!ch || !pkItem)
			return;

		if (pkItem->IsEquipped())
		{
			ch->ComputeBattlePoints();
			ch->UpdatePacket();
		}

		pkItem->UpdatePacket();
	}

	void SetPassiveRelicActive(LPCHARACTER ch, LPITEM pkItem, bool bActive)
	{
		if (!ch || !pkItem)
			return;

		const bool bWasActive = IsPassiveRelicActive(pkItem);
		if (bWasActive == bActive)
			return;

		if (pkItem->IsEquipped() && bWasActive)
			pkItem->ModifyPoints(false);

		pkItem->SetSocket(PASSIVE_RELIC_SOCKET_ACTIVE, bActive ? 1 : 0);

		if (pkItem->IsEquipped() && bActive)
		{
			pkItem->ModifyPoints(true);
			ch->EffectPacket(SE_PASSIVE_EFFECT);
			ch->SpecificEffectPacket("d:/ymir work/effect/etc/buff/buff_passive_01.mse");
		}
		else if (!bActive)
		{
			ch->RemoveAffect(AFFECT_PASSIVE_RELIC_STONE_DEF);
			ch->RemoveAffect(AFFECT_PASSIVE_RELIC_DISMOUNT_SPEED);
		}

		RefreshPassiveRelicState(ch, pkItem);
	}

	bool ParsePassiveRelicMaterialCells(const char* argument, int aiCells[PASSIVE_RELIC_MATERIAL_MAX])
	{
		if (!argument)
			return false;

		for (int i = 0; i < PASSIVE_RELIC_MATERIAL_MAX; ++i)
		{
			char arg[256];
			argument = one_argument(argument, arg, sizeof(arg));
			if (!*arg)
				return false;

			str_to_number(aiCells[i], arg);
		}

		return true;
	}

	LPITEM GetPassiveRelicMaterialItem(LPCHARACTER ch, int iCell, const TPassiveRelicMaterialInfo& rInfo)
	{
		if (!ch)
			return NULL;

		if (iCell < 0 || iCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
			return NULL;

		LPITEM pkItem = ch->GetInventoryItem(iCell);
		if (!pkItem)
			return NULL;

		if (pkItem->GetType() != ITEM_MATERIAL)
			return NULL;

		if (pkItem->GetSubType() != rInfo.bSubType)
			return NULL;

		if (pkItem->GetVnum() != rInfo.dwVnum)
			return NULL;

		if (pkItem->GetCount() <= 0)
			return NULL;

		return pkItem;
	}

	bool CollectPassiveRelicMaterialItems(LPCHARACTER ch, const int aiCells[PASSIVE_RELIC_MATERIAL_MAX], LPITEM apItems[PASSIVE_RELIC_MATERIAL_MAX])
	{
		for (int i = 0; i < PASSIVE_RELIC_MATERIAL_MAX; ++i)
		{
			for (int j = i + 1; j < PASSIVE_RELIC_MATERIAL_MAX; ++j)
			{
				if (aiCells[i] == aiCells[j])
					return false;
			}

			apItems[i] = GetPassiveRelicMaterialItem(ch, aiCells[i], kPassiveRelicMaterials[i]);
			if (!apItems[i])
				return false;
		}

		return true;
	}

	void ConsumePassiveRelicMaterialItems(LPITEM apItems[PASSIVE_RELIC_MATERIAL_MAX])
	{
		for (int i = 0; i < PASSIVE_RELIC_MATERIAL_MAX; ++i)
		{
			if (!apItems[i])
				continue;

			if (apItems[i]->GetCount() > 1)
				apItems[i]->SetCount(apItems[i]->GetCount() - 1);
			else
				apItems[i]->SetCount(0);
		}
	}

	bool AddRandomPassiveRelicBonus(LPITEM pkRelic)
	{
		if (!pkRelic)
			return false;

		std::vector<int> vecAvailableIndexes;
		for (int i = 0; i < static_cast<int>(sizeof(kPassiveRelicBonusPool) / sizeof(kPassiveRelicBonusPool[0])); ++i)
		{
			const TPassiveRelicBonusInfo& rBonusInfo = kPassiveRelicBonusPool[i];
			switch (rBonusInfo.bBonusType)
			{
				case PASSIVE_RELIC_BONUS_DIRECT:
					if (!pkRelic->HasAttr(rBonusInfo.bApply))
						vecAvailableIndexes.push_back(i);
					break;
			}
		}

		if (vecAvailableIndexes.empty())
			return false;

		const int iSelectedIndex = vecAvailableIndexes[number(0, vecAvailableIndexes.size() - 1)];
		const TPassiveRelicBonusInfo& rBonusInfo = kPassiveRelicBonusPool[iSelectedIndex];
		const short sValue = rBonusInfo.sValues[number(0, rBonusInfo.bValueCount - 1)];
		switch (rBonusInfo.bBonusType)
		{
			case PASSIVE_RELIC_BONUS_DIRECT:
				pkRelic->AddAttribute(rBonusInfo.bApply, sValue);
				return true;
		}

		return false;
	}
}

ACMD(do_passive_relic)
{
	char arg1[256];
	argument = one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	const bool bIsUnequipCommand = !str_cmp(arg1, "unequip");
	if (!bIsUnequipCommand && !CanUsePassiveRelicCommand(ch))
		return;

	LPITEM pkRelic = GetEquippedPassiveRelic(ch);
	if (!pkRelic)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Equip your passive relic first.");
		return;
	}

	InitializePassiveRelicDeckState(ch, pkRelic);

	if (!str_cmp(arg1, "earth") || !str_cmp(arg1, "sky"))
	{
		const int iNextDeck = !str_cmp(arg1, "sky") ? PASSIVE_RELIC_DECK_SKY : PASSIVE_RELIC_DECK_EARTH;
		SwitchPassiveRelicDeck(ch, pkRelic, iNextDeck);
		TouchPassiveRelicCooldown(ch);
		ch->ChatPacket(CHAT_TYPE_INFO, iNextDeck == PASSIVE_RELIC_DECK_SKY ? "Sky relic deck selected." : "Earth relic deck selected.");
		return;
	}

	if (!str_cmp(arg1, "extract"))
	{
		char argCell[256];
		one_argument(argument, argCell, sizeof(argCell));
		if (!*argCell)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Place a valid pincer item on the relic.");
			return;
		}

		int iExtractCell = -1;
		str_to_number(iExtractCell, argCell);
		if (iExtractCell < 0 || iExtractCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Invalid extraction item slot.");
			return;
		}

		LPITEM pkExtractItem = ch->GetInventoryItem(iExtractCell);
		if (!pkExtractItem || (pkExtractItem->GetVnum() != 100100 && pkExtractItem->GetVnum() != 100101))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You need Dragon Pincer or Dragon Pincer+.");
			return;
		}

		const int iSuccessChance = (pkExtractItem->GetVnum() == 100101) ? 100 : 30;
		const int iEmptyPos = ch->GetEmptyInventory(pkRelic->GetSize());
		if (iEmptyPos < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You need an empty inventory slot to extract the relic.");
			return;
		}

		pkExtractItem->SetCount(pkExtractItem->GetCount() - 1);

		if (IsPassiveRelicActive(pkRelic))
			SetPassiveRelicActive(ch, pkRelic, false);

		if (number(1, 100) > iSuccessChance)
		{
			SavePassiveRelicDeckState(ch, pkRelic, GetPassiveRelicSelectedDeck(ch));
			pkRelic->RemoveFromCharacter();
			pkRelic->SetCount(0);
			ch->ChatPacket(CHAT_TYPE_INFO, "Relic extraction failed. The relic was destroyed.");
			TouchPassiveRelicCooldown(ch);
			return;
		}

		SavePassiveRelicDeckState(ch, pkRelic, GetPassiveRelicSelectedDeck(ch));
		pkRelic->RemoveFromCharacter();
		pkRelic->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		ch->ChatPacket(CHAT_TYPE_INFO, "Relic extraction succeeded.");
		TouchPassiveRelicCooldown(ch);
		return;
	}

	if (!str_cmp(arg1, "unequip"))
	{
		if (IsPassiveRelicActive(pkRelic))
			SetPassiveRelicActive(ch, pkRelic, false);

		if (number(1, 100) > PASSIVE_RELIC_DIRECT_UNEQUIP_SUCCESS_CHANCE)
		{
			SavePassiveRelicDeckState(ch, pkRelic, GetPassiveRelicSelectedDeck(ch));
			pkRelic->RemoveFromCharacter();
			pkRelic->SetCount(0);
			ch->ChatPacket(CHAT_TYPE_INFO, "Direct relic extraction failed. The relic was destroyed.");
			return;
		}

		int iEmptyPos = ch->GetEmptyInventory(pkRelic->GetSize());
		if (iEmptyPos < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You need an empty inventory slot to unequip the relic.");
			return;
		}

		SavePassiveRelicDeckState(ch, pkRelic, GetPassiveRelicSelectedDeck(ch));
		pkRelic->RemoveFromCharacter();
		pkRelic->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
		ch->ChatPacket(CHAT_TYPE_INFO, "The relic has been removed from the slot.");
		return;
	}

	if (!str_cmp(arg1, "charge"))
	{
		int aiCells[PASSIVE_RELIC_MATERIAL_MAX];
		LPITEM apItems[PASSIVE_RELIC_MATERIAL_MAX] = {};
		if (!ParsePassiveRelicMaterialCells(argument, aiCells) || !CollectPassiveRelicMaterialItems(ch, aiCells, apItems))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Place the four spirit items into the relic slots first.");
			return;
		}

		const int iDuration = GetPassiveRelicDurationSeconds(pkRelic);
		if (iDuration <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "This relic cannot be charged.");
			return;
		}

		if (pkRelic->GetSocket(PASSIVE_RELIC_SOCKET_REMAIN) >= iDuration)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "The relic is already fully charged.");
			return;
		}

		TouchPassiveRelicCooldown(ch);

		if (pkRelic->IsEquipped() && -1 != pkRelic->GetProto()->cLimitTimerBasedOnWearIndex)
			pkRelic->StopTimerBasedOnWearExpireEvent();

		pkRelic->SetSocket(PASSIVE_RELIC_SOCKET_REMAIN, iDuration);

		if (pkRelic->IsEquipped() && -1 != pkRelic->GetProto()->cLimitTimerBasedOnWearIndex)
			pkRelic->StartTimerBasedOnWearExpireEvent();

		ConsumePassiveRelicMaterialItems(apItems);
		RefreshPassiveRelicState(ch, pkRelic);
		ch->ChatPacket(CHAT_TYPE_INFO, "The relic time has been restored.");
		return;
	}

	if (!str_cmp(arg1, "add"))
	{
		int aiCells[PASSIVE_RELIC_MATERIAL_MAX];
		LPITEM apItems[PASSIVE_RELIC_MATERIAL_MAX] = {};
		if (!ParsePassiveRelicMaterialCells(argument, aiCells) || !CollectPassiveRelicMaterialItems(ch, aiCells, apItems))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Place the four spirit items into the relic slots first.");
			return;
		}

		if (GetPassiveRelicBonusCount(pkRelic) >= PASSIVE_RELIC_MAX_BONUS_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "The relic cannot receive more bonuses.");
			return;
		}

		const bool bWasActive = pkRelic->IsEquipped() && IsPassiveRelicActive(pkRelic);
		if (bWasActive)
			pkRelic->ModifyPoints(false);

		const bool bAdded = AddRandomPassiveRelicBonus(pkRelic);

		if (bWasActive)
			pkRelic->ModifyPoints(true);

		if (!bAdded)
		{
			RefreshPassiveRelicState(ch, pkRelic);
			ch->ChatPacket(CHAT_TYPE_INFO, "The relic already has every available bonus.");
			return;
		}

		TouchPassiveRelicCooldown(ch);
		ConsumePassiveRelicMaterialItems(apItems);
		RefreshPassiveRelicState(ch, pkRelic);
		ch->ChatPacket(CHAT_TYPE_INFO, "A random relic bonus has been added.");
		return;
	}

	if (!str_cmp(arg1, "activate"))
	{
		if (GetPassiveRelicBonusCount(pkRelic) <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Add a relic bonus first.");
			return;
		}

		const int iDuration = GetPassiveRelicDurationSeconds(pkRelic);
		if (iDuration > 0 && pkRelic->GetSocket(PASSIVE_RELIC_SOCKET_REMAIN) <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Charge the relic before activation.");
			return;
		}

		TouchPassiveRelicCooldown(ch);

		const bool bNextState = !IsPassiveRelicActive(pkRelic);
		SetPassiveRelicActive(ch, pkRelic, bNextState);
		ch->ChatPacket(CHAT_TYPE_INFO, bNextState ? "The relic is now active." : "The relic is now inactive.");
		return;
	}
}
#endif

#ifdef ENABLE_TITLE_SYSTEM
namespace
{
	struct STitleSystemDef
	{
		int iTitleID;
		const char* c_szName;
		const char* c_szEffect;
	};

	const STitleSystemDef kTitleSystemDefs[] =
	{
		{ 1000, "Sansli", "d:/ymir work/effect/etc/title/title_05_medal.mse" },
		{ 1001, "Metin+", "d:/ymir work/effect/etc/title/title_06_banner_gold.mse" },
		{ 1002, "Destansi", "d:/ymir work/effect/etc/title/title_07_banner_red.mse" },
		{ 1003, "Efsanevi", "d:/ymir work/effect/etc/title/title_08_banner_blue.mse" },
		{ 1004, "Mistik", "d:/ymir work/effect/etc/title/title_02_dragon.mse" },
		{ 1005, "Gunessever", "d:/ymir work/effect/etc/title/title_01_shield.mse" },
		{ 2000, "Son kurtulan", "d:/ymir work/effect/etc/title/title_04_trophy.mse" },
		{ 2001, "Yenilmez", "d:/ymir work/effect/etc/title/title_03_fist.mse" },
		{ 2002, "Savas habercisi", "d:/ymir work/effect/etc/title/title_02_dragon.mse" },
		{ 2003, "Kasap", "d:/ymir work/effect/etc/title/title_01_shield.mse" },
		{ 3000, "Metin+", "d:/ymir work/effect/etc/title/title_06_banner_gold.mse" },
		{ 3001, "Destansi", "d:/ymir work/effect/etc/title/title_07_banner_red.mse" },
		{ 3002, "Efsanevi", "d:/ymir work/effect/etc/title/title_08_banner_blue.mse" },
		{ 3003, "Mistik", "d:/ymir work/effect/etc/title/title_02_dragon.mse" },
		{ 3004, "Sansli", "d:/ymir work/effect/etc/title/title_05_medal.mse" },
	};

	const char* TITLE_SYSTEM_CLEAR_EFFECT_TOKEN = "__TITLE_EFFECT_CLEAR__";
	const char* TITLE_SYSTEM_ACTIVE_FLAG = "title_system.active";
	const char* TITLE_SYSTEM_OWNED_FLAG_FMT = "title_system.owned.%d";
	const char* TITLE_SYSTEM_EXPIRE_FLAG_FMT = "title_system.expire.%d";

	void BuildTitleSystemOwnedFlag(int iTitleID, char* szFlag, size_t stSize)
	{
		snprintf(szFlag, stSize, TITLE_SYSTEM_OWNED_FLAG_FMT, iTitleID);
	}

	void BuildTitleSystemExpireFlag(int iTitleID, char* szFlag, size_t stSize)
	{
		snprintf(szFlag, stSize, TITLE_SYSTEM_EXPIRE_FLAG_FMT, iTitleID);
	}

	const STitleSystemDef* FindTitleSystemDef(int iTitleID)
	{
		for (size_t i = 0; i < sizeof(kTitleSystemDefs) / sizeof(kTitleSystemDefs[0]); ++i)
		{
			if (kTitleSystemDefs[i].iTitleID == iTitleID)
				return &kTitleSystemDefs[i];
		}

		return NULL;
	}

	int GetTitleSystemExpireTime(LPCHARACTER ch, int iTitleID)
	{
		if (!ch)
			return 0;

		char szFlag[64];
		BuildTitleSystemExpireFlag(iTitleID, szFlag, sizeof(szFlag));
		return ch->GetQuestFlag(szFlag);
	}

	bool IsTitleSystemOwned(LPCHARACTER ch, int iTitleID)
	{
		if (!ch)
			return false;

		char szFlag[64];
		BuildTitleSystemOwnedFlag(iTitleID, szFlag, sizeof(szFlag));
		return ch->GetQuestFlag(szFlag) > 0;
	}

	void ClearTitleSystemOwnership(LPCHARACTER ch, int iTitleID)
	{
		if (!ch)
			return;

		char szOwned[64];
		char szExpire[64];
		BuildTitleSystemOwnedFlag(iTitleID, szOwned, sizeof(szOwned));
		BuildTitleSystemExpireFlag(iTitleID, szExpire, sizeof(szExpire));
		ch->SetQuestFlag(szOwned, 0);
		ch->SetQuestFlag(szExpire, 0);

		if (ch->GetQuestFlag(TITLE_SYSTEM_ACTIVE_FLAG) == iTitleID)
			ch->SetQuestFlag(TITLE_SYSTEM_ACTIVE_FLAG, 0);
	}

	bool IsTitleSystemExpired(LPCHARACTER ch, int iTitleID)
	{
		const int iExpireAt = GetTitleSystemExpireTime(ch, iTitleID);
		if (iExpireAt <= 0)
			return false;

		if (iExpireAt > get_global_time())
			return false;

		ClearTitleSystemOwnership(ch, iTitleID);
		return true;
	}

	void SetTitleSystemActive(LPCHARACTER ch, int iTitleID)
	{
		if (!ch)
			return;

		ch->SetQuestFlag(TITLE_SYSTEM_ACTIVE_FLAG, iTitleID);
	}
}

ACMD(do_title)
{
	char arg1[256];
	argument = one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Title commands: /title list, /title equip <id>, /title clear, /title status");
		return;
	}

	if (!str_cmp(arg1, "list"))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TitleSyncReset");
		ch->ChatPacket(CHAT_TYPE_INFO, "Unlocked titles:");
		for (size_t i = 0; i < sizeof(kTitleSystemDefs) / sizeof(kTitleSystemDefs[0]); ++i)
		{
			const int iTitleID = kTitleSystemDefs[i].iTitleID;
			if (!IsTitleSystemOwned(ch, iTitleID))
				continue;

			if (IsTitleSystemExpired(ch, iTitleID))
				continue;

			const int iExpireAt = GetTitleSystemExpireTime(ch, iTitleID);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "TitleSyncAdd %d", iTitleID);
			if (iExpireAt > 0)
				ch->ChatPacket(CHAT_TYPE_INFO, "- %d: %s (expires in %d sec)", iTitleID, kTitleSystemDefs[i].c_szName, iExpireAt - get_global_time());
			else
				ch->ChatPacket(CHAT_TYPE_INFO, "- %d: %s", iTitleID, kTitleSystemDefs[i].c_szName);
		}
		return;
	}

	if (!str_cmp(arg1, "status"))
	{
		const int iActive = ch->GetQuestFlag(TITLE_SYSTEM_ACTIVE_FLAG);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TitleSyncActive %d", iActive);
		if (iActive <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "No active title.");
			return;
		}

		const STitleSystemDef* pkDef = FindTitleSystemDef(iActive);
		if (!pkDef)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Active title id: %d", iActive);
			return;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, "Active title: %d (%s)", iActive, pkDef->c_szName);
		return;
	}


	if (!str_cmp(arg1, "sync"))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TitleSyncReset");
		for (size_t i = 0; i < sizeof(kTitleSystemDefs) / sizeof(kTitleSystemDefs[0]); ++i)
		{
			const int iTitleID = kTitleSystemDefs[i].iTitleID;
			if (!IsTitleSystemOwned(ch, iTitleID))
				continue;

			if (IsTitleSystemExpired(ch, iTitleID))
				continue;

			ch->ChatPacket(CHAT_TYPE_COMMAND, "TitleSyncAdd %d", iTitleID);
		}
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TitleSyncActive %d", ch->GetQuestFlag(TITLE_SYSTEM_ACTIVE_FLAG));
		return;
	}

	if (!str_cmp(arg1, "clear"))
	{
		SetTitleSystemActive(ch, 0);
		ch->SpecificEffectPacket(TITLE_SYSTEM_CLEAR_EFFECT_TOKEN);
		// no restart for title clear (client sync command handles UI state)
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TitleSyncActive 0");
		ch->ChatPacket(CHAT_TYPE_INFO, "Title unequipped.");
		return;
	}

	if (!str_cmp(arg1, "equip"))
	{
		char arg2[256];
		one_argument(argument, arg2, sizeof(arg2));
		if (!*arg2)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Usage: /title equip <id>");
			return;
		}

		int iTitleID = 0;
		str_to_number(iTitleID, arg2);
		const STitleSystemDef* pkDef = FindTitleSystemDef(iTitleID);
		if (!pkDef)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Unknown title id.");
			return;
		}

		if (!IsTitleSystemOwned(ch, iTitleID) || IsTitleSystemExpired(ch, iTitleID))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You do not own this title or it has expired.");
			return;
		}
		const int iActiveTitle = ch->GetQuestFlag(TITLE_SYSTEM_ACTIVE_FLAG);
		if (iActiveTitle == iTitleID)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "This title is already active.");
			return;
		}
		if (iActiveTitle > 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Remove your current title first.");
			return;
		}
		ch->SpecificEffectPacket(TITLE_SYSTEM_CLEAR_EFFECT_TOKEN);
		SetTitleSystemActive(ch, iTitleID);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TitleSyncActive %d", iTitleID);
		if (pkDef->c_szEffect && *pkDef->c_szEffect) ch->SpecificEffectPacket(pkDef->c_szEffect);
		ch->ChatPacket(CHAT_TYPE_INFO, "%s was equipped.", pkDef->c_szName);
		return;
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "Unknown title command.");
}
#endif

#ifdef __AUTO_HUNT__
ACMD(do_auto_hunt)
{
	ch->GetAutoHuntCommand(argument);
}
#endif

#ifdef ENABLE_HORSE_SPAWN_EXPLOIT_FIX
ACMD(do_user_horse_ride)
{
	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHorseRiding() == false)
	{
		if (ch->GetMountVnum())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You're already riding. Get off first."));
			return;
		}

		// Performans optimizasyonu: GetQuestFlag yerine direkt member variable
		if (ch->GetHorseCheckerFlag() > 0 && get_global_time() < ch->GetHorseCheckerFlag())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "<Sistem> Lütfen %d saniye bekleyin.", (ch->GetHorseCheckerFlag() - get_global_time()) % 180);
			return;
		}

		if (ch->GetHorse() == NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
			return;
		}

		ch->SetHorseCheckerFlag(get_global_time() + 3);
		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}

ACMD(do_user_horse_back)
{
	if (ch->GetHorse() != NULL)
	{
		// Performans optimizasyonu: GetQuestFlag yerine direkt member variable
		if (ch->GetHorseCheckerFlag() > 0 && get_global_time() < ch->GetHorseCheckerFlag())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "<Sistem> Lütfen %d saniye bekleyin.", (ch->GetHorseCheckerFlag() - get_global_time()) % 180);
			return;
		}
		ch->SetHorseCheckerFlag(get_global_time() + 3);
		ch->HorseSummon(false);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have sent your horse away."));
	}
	else if (ch->IsHorseRiding() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to get off your Horse."));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
	}
}
#else
ACMD(do_user_horse_ride)
{
	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHorseRiding() == false)
	{
		if (ch->GetMountVnum())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You're already riding. Get off first."));
			return;
		}

		if (ch->GetHorse() == NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
			return;
		}

		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}

ACMD(do_user_horse_back)
{
	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have sent your horse away."));
	}
	else if (ch->IsHorseRiding() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to get off your Horse."));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
	}
}
#endif

ACMD(do_user_horse_feed)
{
	if (ch->GetMyShop())
		return;

	if (ch->GetHorse() == NULL)
	{
		if (ch->IsHorseRiding() == false)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
		else
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed your Horse whilst sitting on it."));
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE locale = ch->GetLanguage();
#endif

	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();
		const char* itemName = 
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			LC_LOCALE_ITEM(dwFood, locale);
#else
			ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName;
#endif
		if (itemName)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 455, "");
		}
	}
	else
	{
		const char* itemName = 
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			LC_LOCALE_ITEM(dwFood, locale);
#else
			ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName;
#endif
		if (itemName)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need %s."), itemName);
		}
	}
}

#define MAX_REASON_LEN 128

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int		subcmd;
	int         	left_second;
	char		szReason[MAX_REASON_LEN];

	TimedEventInfo()
	: ch()
	, subcmd( 0 )
	, left_second( 0 )
	{
		::memset( szReason, 0, MAX_REASON_LEN );
	}
};

#ifdef ENABLE_CHANGE_CHANNEL
EVENTINFO(ChangeChannelEventInfo)
{
	DynamicCharacterPtr ch;
	int channel_number;
	int left_second;

	ChangeChannelEventInfo() : ch() , channel_number( 0 ) , left_second( 0 ) {}
};
#endif

struct SendDisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetCharacter())
		{
#ifdef FLUSH_AT_SHUTDOWN
			d->GetCharacter()->SaveReal();
			DWORD pid = d->GetCharacter()->GetPlayerID();
			db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
			db_clientdesc->Packet(&pid, sizeof(DWORD));
#endif			
			if (d->GetCharacter()->GetGMLevel() == GM_PLAYER)
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
#ifdef ENABLE_ANALYZE_CLOSE_FIX
		if (!d)
			return;
#endif

		if (d->GetType() == DESC_TYPE_CONNECTOR)
			return;

		if (d->IsPhase(PHASE_P2P))
			return;

		if (d->GetCharacter())
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
		: seconds(0)
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>( event->info );

	if ( info == NULL )
	{
		sys_err( "shutdown_event> <Factor> Null pointer" );
		return 0;
	}

	int * pSec = & (info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (-- * pSec == -10)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
			return 0;

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
		SendLocaleNotice(CHAT_TYPE_NOTICE, 0, 0, 288, "%d", *pSec);
		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();

	SendLocaleNotice(CHAT_TYPE_NOTICE, 0, 0, 342, "%d", iSec);

	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

ACMD(do_shutdown)
{
 	if (!ch)
		return;

	if (!ch->IsGM())
		return;

	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));
	sys_err("Accept shutdown command from %s.", ch->GetName());
	Shutdown(10);
}

EVENTFUNC(timed_event)
{
	TimedEventInfo * info = dynamic_cast<TimedEventInfo *>( event->info );

	if ( info == NULL )
	{
		sys_err( "timed_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL)
	{
		return 0;
	}

	LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
			case SCMD_QUIT:
			case SCMD_PHASE_SELECT:
				{
					TPacketNeedLoginLogInfo acc_info;
					acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
					acc_info.bLanguage = ch->GetDesc()->GetAccountTable().bLanguage;
#endif

					db_clientdesc->DBPacket( HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info) );

					LogManager::instance().DetailLoginLog( false, ch );
				}
				break;
		}

		switch (info->subcmd)
		{
			case SCMD_LOGOUT:
				if (d)
					d->SetPhase(PHASE_CLOSE);
				break;

			case SCMD_QUIT:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
				if (d)
					d->DelayedDisconnect(3);
				break;

			case SCMD_PHASE_SELECT:
				{
					ch->Disconnect("timed_event - SCMD_PHASE_SELECT");

					if (d)
					{
						d->SetPhase(PHASE_SELECT);
					}
				}
				break;
		}

		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d seconds until Exit."), info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

#ifdef ENABLE_CHANGE_CHANNEL
EVENTFUNC(change_channel_event)
{
	ChangeChannelEventInfo * info = dynamic_cast<ChangeChannelEventInfo *>(event->info);

	if (info == NULL)
	{
		sys_err( "change_channel_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL)
	{
		return 0;
	}

	if (info->left_second <= 0)
	{
		ch->m_pkChangeChannelEvent = NULL;

		ch->ChangeChannel(info->channel_number);
	
		return 0;
	}
	else
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 289, "%d", info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}
#endif

ACMD(do_cmd)
{
	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been cancelled."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	switch (subcmd)
	{
		case SCMD_LOGOUT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Back to login window. Please wait."));
			break;

		case SCMD_QUIT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have been disconnected from the server. Please wait."));
			break;

		case SCMD_PHASE_SELECT:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are changing character. Please wait."));
			break;
	}

	int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
		return;

	switch (subcmd)
	{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
			{
				TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

				{
					if (ch->IsPosition(POS_FIGHTING))
						info->left_second = 10;
					else
						info->left_second = 3;
				}

				info->ch = ch;
				info->subcmd = subcmd;
				strlcpy(info->szReason, argument, sizeof(info->szReason));

				ch->m_pkTimedEvent	= event_create(timed_event, info, 1);
			}
			break;
	}
}

ACMD(do_mount)
{
}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	char* endptr;
	double rotation = strtod(arg1, &endptr);
	if (*endptr != '\0' || errno == ERANGE)
		return;

	ch->SetRotation(rotation);
	ch->fishing();
}

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{

#ifdef ENABLE_SUNG_MAHI_TOWER
	LPDUNGEON dungeonInstance = ch->GetDungeon();
	if (dungeonInstance && dungeonInstance->GetFlag("isSungMahiDungeon") > 0)
	{
		ch->WarpSet((9216 * 100), (6144*100));
		return;
	}
#endif

	if (false == ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->StartRecoveryEvent();
		return;
	}

	if (NULL == ch->m_pkDeadEvent)
		return;

	int iTimeToDead = (event_time(ch->m_pkDeadEvent) / passes_per_sec);

	if (subcmd != SCMD_RESTART_TOWN && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (!test_server)
		{
			if (ch->IsHack())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot restart yet. Please wait another %d seconds."), iTimeToDead - (180 - g_nPortalLimitTime));
				return;
			}

			if (iTimeToDead > 170)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot restart yet. Please wait another %d seconds."), iTimeToDead - 170);
				return;
			}
		}
	}

	if (subcmd == SCMD_RESTART_TOWN)
	{
		if (ch->IsHack())
		{
			if ((!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot restart yet. Please wait another %d seconds."), iTimeToDead - (180 - g_nPortalLimitTime));
				return;
			}
		}

		if (iTimeToDead > 173)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot restart in the city yet. Wait another %d seconds."), iTimeToDead - 170);
			return;
		}
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	ch->GetDesc()->SetPhase(PHASE_GAME);
	ch->SetPosition(POS_STANDING);
	ch->StartRecoveryEvent();

	if (ch->GetDungeon())
		ch->GetDungeon()->UseRevive(ch);

	if (ch->GetWarMap() && !ch->IsObserverMode())
	{
		CWarMap * pMap = ch->GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(ch) : 0;

		if (dwGuildOpponent)
		{
			switch (subcmd)
			{
				case SCMD_RESTART_TOWN:
					sys_log(0, "do_restart: restart town");
					PIXEL_POSITION pos;

					if (CWarMapManager::instance().GetStartPosition(ch->GetMapIndex(), ch->GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
						ch->Show(ch->GetMapIndex(), pos.x, pos.y);
					else
						ch->ExitToSavedLocation();

					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_SYSTEM
					ch->CheckMount();
#endif
#ifdef ENABLE_PET_SYSTEM
					ch->CheckPet();
#endif
					break;

				case SCMD_RESTART_HERE:
					sys_log(0, "do_restart: restart here");
					ch->RestartAtSamePos();
					//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
					ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
					ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
					ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_SYSTEM
					ch->CheckMount();
#endif
#ifdef ENABLE_PET_SYSTEM
					ch->CheckPet();
#endif
					break;
			}

			return;
		}
	}

#ifdef ENABLE_QUEEN_NETHIS
	if (SnakeLair::CSnk::instance().IsSnakeMap(ch->GetMapIndex()))
	{
		switch (subcmd)
		{
			case SCMD_RESTART_TOWN:
				sys_log(0, "do_restart: restart town");
				//PIXEL_POSITION pos;

				ch->RestartAtSamePos();

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(3);
				break;

			case SCMD_RESTART_HERE:
				sys_log(0, "do_restart: restart here");
				ch->RestartAtSamePos();
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(3);
				break;

			default:
				break;
		}

		return;
	}
#endif

	switch (subcmd)
	{
		case SCMD_RESTART_TOWN:
			sys_log(0, "do_restart: restart town");
			PIXEL_POSITION pos;

			if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
				ch->WarpSet(pos.x, pos.y);
			else
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
			ch->PointChange(POINT_HP, 50 - ch->GetHP());
			ch->DeathPenalty(1);
			break;

		case SCMD_RESTART_HERE:
			sys_log(0, "do_restart: restart here");
			ch->RestartAtSamePos();
			//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
			ch->PointChange(POINT_HP, 50 - ch->GetHP());
			ch->DeathPenalty(0);
			ch->ReviveInvisible(5);
#ifdef ENABLE_MOUNT_SYSTEM
			ch->CheckMount();
#endif
#ifdef ENABLE_PET_SYSTEM
			ch->CheckPet();
#endif
			break;
	}
}

#define MAX_STAT 90

ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus) // Fix
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	int vlaxc = 1;
	str_to_number(vlaxc, arg2);

	if (!*arg1 || vlaxc <= 0)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your status while you are transformed."));
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
		return;

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
		{
			return;
		}
		idx = POINT_ST;
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
		{
			return;
		}
		idx = POINT_DX;
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
		{
			return;
		}
		idx = POINT_HT;
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
		{
			return;
		}
		idx = POINT_IQ;
	}
	else
		return;

	if (idx == 0)
		return;

	if (vlaxc > ch->GetPoint(POINT_STAT_RESET_COUNT))
		vlaxc = ch->GetPoint(POINT_STAT_RESET_COUNT);

	// FIX: stat, job baslangic degerinin altina (negatife) dusurulemez
	{
		const int iInitial = (idx == POINT_ST) ? JobInitialPoints[ch->GetJob()].st :
			(idx == POINT_DX) ? JobInitialPoints[ch->GetJob()].dx :
			(idx == POINT_HT) ? JobInitialPoints[ch->GetJob()].ht : JobInitialPoints[ch->GetJob()].iq;
		if (vlaxc > ch->GetRealPoint(idx) - iInitial)
			vlaxc = ch->GetRealPoint(idx) - iInitial;
		if (vlaxc <= 0)
			return;
	}

	if (ch->GetRealPoint(idx) - vlaxc > MAX_STAT)
		vlaxc = MAX_STAT + ch->GetRealPoint(idx);

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) - vlaxc);
	ch->SetPoint(idx, ch->GetPoint(idx) - vlaxc);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
		ch->PointChange(POINT_MAX_HP, 0);
	else if (idx == POINT_HT)
		ch->PointChange(POINT_MAX_SP, 0);

	ch->PointChange(POINT_STAT, + vlaxc);
	ch->PointChange(POINT_STAT_RESET_COUNT, - vlaxc);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256];
#ifdef ENABLE_STATUS_UP_RENEWAL
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
#else
	one_argument(argument, arg1, sizeof(arg1));
#endif

	if (!*arg1)
		return;

#ifdef ENABLE_STATUS_UP_RENEWAL
	int iStatUp = 1;
	if (*arg2)
		iStatUp = atoi(arg2);
#endif

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your status while you are transformed."));
		return;
	}

#ifdef ENABLE_STATUS_UP_RENEWAL
	if (ch->GetPoint(POINT_STAT) < iStatUp)
		iStatUp = ch->GetPoint(POINT_STAT);
#else
	if (ch->GetPoint(POINT_STAT) <= 0)
		return;
#endif

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
		idx = POINT_ST;
	else if (!strcmp(arg1, "dx"))
		idx = POINT_DX;
	else if (!strcmp(arg1, "ht"))
		idx = POINT_HT;
	else if (!strcmp(arg1, "iq"))
		idx = POINT_IQ;
	else
		return;

	// ch->ChatPacket(CHAT_TYPE_INFO, "%s GRP(%d) idx(%u), MAX_STAT(%d), expr(%d)", __FUNCTION__, ch->GetRealPoint(idx), idx, MAX_STAT, ch->GetRealPoint(idx) >= MAX_STAT);
#ifdef ENABLE_STATUS_UP_RENEWAL
	if ((ch->GetRealPoint(idx) + iStatUp) > MAX_STAT)
		iStatUp = MAX_STAT - ch->GetRealPoint(idx);

	if (iStatUp < 1)
#else
	if (ch->GetRealPoint(idx) >= MAX_STAT)
#endif
		return;
#ifdef ENABLE_STATUS_UP_RENEWAL
	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + iStatUp);
	ch->SetPoint(idx, ch->GetPoint(idx) + iStatUp);
#else
	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
#endif
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}
#ifdef ENABLE_STATUS_UP_RENEWAL
	ch->PointChange(POINT_STAT, -iStatUp);
#else
	ch->PointChange(POINT_STAT, -1);
#endif
	ch->ComputePoints();
}

ACMD(do_pvp)
{
	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
		return;

	if (pkVictim->IsNPC())
		return;

#ifdef ENABLE_MESSENGER_BLOCK
	if (MessengerManager::Instance().IsBlocked(ch->GetName(), pkVictim->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Unblock %s to continue."), pkVictim->GetName());
		return;
	}
	else if (MessengerManager::Instance().IsBlocked(pkVictim->GetName(), ch->GetName()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s has blocked you."), pkVictim->GetName());
		return;
	}
#endif

	if (pkVictim->GetArena() != NULL)
	{
		pkVictim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This player is currently fighting."));
		return;
	}

	CPVPManager::instance().Insert(ch, pkVictim);
}

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] It does not belong to the guild."));
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You do not have the authority to change the level of the guild skills."));
	}
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch(vnum)
		{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:

			case SKILL_7_A_ANTI_TANHWAN:
			case SKILL_7_B_ANTI_AMSEOP:
			case SKILL_7_C_ANTI_SWAERYUNG:
			case SKILL_7_D_ANTI_YONGBI:

			case SKILL_8_A_ANTI_GIGONGCHAM:
			case SKILL_8_B_ANTI_YEONSA:
			case SKILL_8_C_ANTI_MAHWAN:
			case SKILL_8_D_ANTI_BYEURAK:

			case SKILL_ADD_HP:
			case SKILL_RESIST_PENETRATE:
				ch->SkillLevelUp(vnum);
				break;
		}
	}
}

ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have entered an incorrect password."));
		return;
	}

	if (!*arg2 || strlen(arg2)>6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have entered an incorrect password."));
		return;
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have entered an incorrect password."));
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] The Storeroom is already open."));
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have to wait 10 seconds before you can open the Storeroom again."));
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

#ifdef ENABLE_SPECIAL_INVENTORY
ACMD(do_new_safebox_open)
{
	if (!ch) return;
	if (ch->m_pkOpenSafeboxEvent) return;
	ch->SafeboxLoad();
}

ACMD(do_new_mall_open)
{
	if (!ch) return;
	if (ch->m_pkOpenMallEvent) return;
	ch->MallOpen();
}
#endif

ACMD(do_ungroup)
{
	if (!ch)
		return;

	if (!ch->GetParty())
		return;

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot leave a group while you are in a dungeon."));
		return;
	}

#ifdef ENABLE_QUEEN_NETHIS
	if (SnakeLair::CSnk::instance().IsSnakeMap(ch->GetMapIndex()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;676]");
		return;
	}
#endif

	LPPARTY pParty = ch->GetParty();
	if (!pParty)
		return;

	if (pParty->GetMemberCount() == 2)
	{
		// party disband
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You have left the group."));
		pParty->Quit(ch->GetPlayerID());
	}
}

ACMD(do_close_shop)
{
	if (ch->GetMyShop())
	{
		ch->CloseMyShop();
		return;
	}
}

ACMD(do_set_walk_mode)
{
	if (!ch)
		return;

	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	if (!ch)
		return;

	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

ACMD(do_war)
{
	if (!ch)
		return;

	//get my guild information
	CGuild* g = ch->GetGuild();

	if (!g)
		return;

	//Check if there is a war!
	if (g->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Your guild is already participating in another war."));
		return;
	}

	//Divide the parameter by two

	char arg1[256], arg2[256];
	int type = GUILD_WAR_TYPE_FIELD; //fixme102 base int modded uint
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

#ifdef ENABLE_WAR_CRASHER_PROTECTION_FIX
	if (*arg2)
	{
		str_to_number(type, arg2);

		if(type < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<error> can't declare war with type less than zero."));
			return;
		}

		if (type >= GUILD_WAR_TYPE_MAX_NUM || type == 0)
			type = GUILD_WAR_TYPE_FIELD;
	}
#else
	if (*arg2)
	{
		str_to_number(type, arg2);

		if (type >= GUILD_WAR_TYPE_MAX_NUM || type < 0)
			type = GUILD_WAR_TYPE_FIELD;
	}
#endif

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] No one is entitled to a guild war."));
		return;
	}

	CGuild * opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] No guild with this name exists."));
		return;
	}

	switch (g->GetGuildWarState(opp_g->GetID()))
	{
		case GUILD_WAR_NONE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This guild is already participating in a war."));
					return;
				}

				int iWarPrice = KOR_aGuildWarInfo[type].iWarPrice;

				if (g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Not enough Yang to participate in a guild war."));
					return;
				}

				if (opp_g->GetGuildMoney() < iWarPrice)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The guild does not have enough Yang to participate in a guild war."));
					return;
				}
			}
			break;

		case GUILD_WAR_SEND_DECLARE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have declared war on this guild."));
				return;
			}
			break;

		case GUILD_WAR_RECV_DECLARE:
			{
				if (opp_g->UnderAnyWar())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This guild is already participating in a war."));
					g->RequestRefuseWar(opp_g->GetID());
					return;
				}
			}
			break;

		case GUILD_WAR_RESERVE:
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This Guild is already scheduled for another war."));
				return;
			}
			break;

		case GUILD_WAR_END:
			return;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This guild is already participating in another war."));
			g->RequestRefuseWar(opp_g->GetID());
			return;
	}

	if (!g->CanStartWar(type))
	{
		if (g->GetLadderPoint() == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Guild level is too low to participate in a guild war."));
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] A minimum of %d players are needed to participate in a guild war."), GUILD_WAR_MIN_MEMBER_COUNT);
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}

	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
		if (opp_g->GetLadderPoint() == 0)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The guild does not have enough points to participate in a guild war."));
		else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The guild does not have enough members to participate in a guild war."));
		return;
	}

	do
	{
		if (g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The enemy's guild leader is offline."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{
		if (opp_g->GetMasterCharacter() != NULL)
			break;

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());

		if (pCCI != NULL)
			break;

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The enemy's guild leader is offline."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] No one is entitled to a guild war."));
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] No guild with this name exists."));
		return;
	}

	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_detaillog)
{
	ch->DetailLog();
}

ACMD(do_monsterlog)
{
	ch->ToggleMonsterLog();
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE mode = 0;
	str_to_number(mode, arg1);

	if (mode == PK_MODE_PROTECT)
		return;

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
		return;

	ch->SetPKMode(mode);
}

ACMD(do_messenger_auth)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	char answer = LOWER(*arg1);
	
	if (!MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, answer == 'y' ? false : true))
		return;

	if (answer != 'y')
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);

		if (tch)
			tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s declined the invitation."), ch->GetName());
#ifdef ENABLE_CROSS_CHANNEL_REQUESTS
		else
		{
			CCI* pkCCI = P2P_MANAGER::Instance().Find(arg2);
			if (pkCCI)
			{
				LPDESC pkDesc = pkCCI->pkDesc;
				pkDesc->SetRelay(arg2);
				pkDesc->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s declined the invitation."), ch->GetName());
				pkDesc->SetRelay("");
			}
		}
#endif
	}
}

ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_unmount)
{
#ifdef ENABLE_MOUNT_SYSTEM
	if(ch->GetWear(WEAR_MOUNT))
	{
		CMountSystem* mountSystem = ch->GetMountSystem();
		LPITEM mount = ch->GetWear(WEAR_MOUNT);
#ifdef ENABLE_MOUNT_PET_SKIN
		LPITEM mountSkinItem = ch->GetWear(WEAR_COSTUME_MOUNT);
#endif

		if (!mountSystem && !mount)
			return;

#ifdef ENABLE_MOUNT_PET_SKIN
		DWORD mobVnum = mountSkinItem ? mountSkinItem->GetValue(1) : mount->GetValue(1);
#else
		DWORD mobVnum = mount->GetValue(1);
#endif

		if (ch->GetMountVnum())
		{
			if(mountSystem->CountSummoned() == 0)
			{
				mountSystem->Unmount(mobVnum);
			}
		}
		return;
	}

	LPITEM item = ch->GetWear(WEAR_UNIQUE1);
	LPITEM item2 = ch->GetWear(WEAR_UNIQUE2);
	LPITEM item3 = ch->GetWear(WEAR_MOUNT);

	if (item && item->IsRideItem())
		ch->UnequipItem(item);

	if (item2 && item2->IsRideItem())
		ch->UnequipItem(item2);

	if (item3 && item3->IsRideItem())
		ch->UnequipItem(item3);
#endif

	if (true == ch->UnEquipSpecialRideUniqueItem())
	{
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		if (ch->IsHorseRiding())
		{
			ch->StopRiding();
		}
	}
	else
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 191, "");
	}
}

ACMD(do_observer_exit)
{
#ifdef ENABLE_DRAGON_LAIR
	if(ch->GetMapIndex() == 208)
	{
		if (ch->IsObserverMode())
		{
			ch->SetObserverMode(false);
			ch->WarpSet(180000,1220000);
			return;
		}
	}
#endif
	if (ch->IsObserverMode())
	{
		if (ch->GetWarMap())
			ch->SetWarMap(NULL);

		if (ch->GetArena() != NULL || ch->GetArenaObserverMode() == true)
		{
			ch->SetArenaObserverMode(false);

			if (ch->GetArena() != NULL)
				ch->GetArena()->RemoveObserver(ch->GetPlayerID());

			ch->SetArena(NULL);
			ch->WarpSet(ARENA_RETURN_POINT_X(ch->GetEmpire()), ARENA_RETURN_POINT_Y(ch->GetEmpire()));
		}
		else
		{
			ch->ExitToSavedLocation();
		}
		ch->SetObserverMode(false);
	}
}

ACMD(do_view_equip)
{
	if (ch->GetGMLevel() <= GM_PLAYER)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD vid = 0;
		str_to_number(vid, arg1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

		if (!tch)
			return;

		if (!tch->IsPC())
			return;

#ifdef ENABLE_MESSENGER_BLOCK
		if (MessengerManager::Instance().IsBlocked(ch->GetName(), tch->GetName()))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Unblock %s to continue."), tch->GetName());
			return;
		}
		else if (MessengerManager::Instance().IsBlocked(tch->GetName(), ch->GetName()))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s has blocked you."), tch->GetName());
			return;
		}
#endif

		tch->SendEquipment(ch);
	}
}

ACMD(do_party_request)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
		return;
	}

	if (ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot accept the invitation because you are already in the group."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
}

ACMD(do_party_request_accept)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->AcceptToParty(tch);
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		ch->DenyToParty(tch);
}

struct GotoInfo
{
	std::string st_name;

	BYTE empire;
	int mapIndex;
	DWORD x, y;

	GotoInfo()
	{
		st_name = "";
		empire = 0;
		mapIndex = 0;

		x = 0;
		y = 0;
	}

	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}

	void __copy__(const GotoInfo& c_src)
	{
		st_name = c_src.st_name;
		empire = c_src.empire;
		mapIndex = c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

extern void BroadcastNotice(const char * c_pszBuf);

static const char* FN_point_string(int apply_number)
{
	switch (apply_number)
	{
		case POINT_MAX_HP:							return "Hit Points +%d";
		case POINT_MAX_SP:							return "Spell Points +%d";
		case POINT_HT:								return "Endurance +%d";
		case POINT_IQ:								return "Intelligence +%d";
		case POINT_ST:								return "Strength +%d";
		case POINT_DX:								return "Dexterity +%d";
		case POINT_ATT_SPEED:						return "Attack Speed +%d";
		case POINT_MOV_SPEED:						return "Movement Speed %d";
		case POINT_CASTING_SPEED:					return "Cooldown Time -%d";
		case POINT_HP_REGEN:						return "Energy Recovery +%d";
		case POINT_SP_REGEN:						return "Spell Point Recovery +%d";
		case POINT_POISON_PCT:						return "Poison Attack %d";
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_BLEEDING_PCT:					return "??? %d";
#endif
		case POINT_STUN_PCT:						return "Star +%d";
		case POINT_SLOW_PCT:						return "Speed Reduction +%d";
		case POINT_CRITICAL_PCT:					return "Critical Attack with a chance of %d%%";
		case POINT_RESIST_CRITICAL:					return "Critical Resistance %d%%";
		case POINT_PENETRATE_PCT:					return "Chance of a Speared Attack of %d%%";
		case POINT_RESIST_PENETRATE:				return "Penetrate Resistance %d%%";
		case POINT_ATTBONUS_HUMAN:					return "Player's Attack Power against Monsters +%d%%";
		case POINT_ATTBONUS_ANIMAL:					return "Horse's Attack Power against Monsters +%d%%";
		case POINT_ATTBONUS_ORC:					return "Attack Boost against Wonggui + %d%%";
		case POINT_ATTBONUS_MILGYO:					return "Attack Boost against Milgyo + %d%%";
		case POINT_ATTBONUS_UNDEAD:					return "Attack boost against zombies + %d%%";
		case POINT_ATTBONUS_DEVIL:					return "Attack boost against devils + %d%%";
		case POINT_STEAL_HP:						return "Absorbing of Energy %d%% while attacking.";
		case POINT_STEAL_SP:						return "Absorption of Spell Points (SP) %d%% while attacking.";
		case POINT_MANA_BURN_PCT:					return "With a chance of %d%% Spell Points (SP) will be taken from the enemy.";
		case POINT_DAMAGE_SP_RECOVER:				return "Absorbing of Spell Points (SP) with a chance of %d%%";
		case POINT_BLOCK:							return "%d%% Chance of blocking a close-combat attack";
		case POINT_DODGE:							return "%d%% Chance of blocking a long range attack";
		case POINT_RESIST_SWORD:					return "One-Handed Sword defence %d%%";
		case POINT_RESIST_TWOHAND:					return "Two-Handed Sword Defence %d%%";
		case POINT_RESIST_DAGGER:					return "Two-Handed Sword Defence %d%%";
		case POINT_RESIST_BELL:						return "Bell Defence %d%%";
		case POINT_RESIST_FAN:						return "Fan Defence %d%%";
		case POINT_RESIST_BOW:						return "Distant Attack Resistance %d%%";
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_CLAW:						return "??? ?? %d%%";
#endif
		case POINT_RESIST_FIRE:						return "Fire Resistance %d%%";
		case POINT_RESIST_ELEC:						return "Lightning Resistance %d%%";
		case POINT_RESIST_MAGIC:					return "Magic Resistance %d%%";
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
		case POINT_RESIST_MAGIC_REDUCTION:			return "¸¶¹y ÀúÇ× %d%%";
#endif
		case POINT_RESIST_WIND:						return "Wind Resistance %d%%";
		case POINT_RESIST_ICE:						return "Ice Resistance %d%%";
		case POINT_RESIST_EARTH:					return "Earth Resistance %d%%";
		case POINT_RESIST_DARK:						return "Dark Resistance %d%%";
		case POINT_REFLECT_MELEE:					return "Reflect Direct Hit: %d%%";
		case POINT_REFLECT_CURSE:					return "Reflect Curse: %d%%";
		case POINT_POISON_REDUCE:					return "Poison Resistance %d%%";
#ifdef ENABLE_WOLFMAN_CHARACTER
	case POINT_BLEEDING_REDUCE:						return "? ?? %d%%";
#endif
		case POINT_KILL_SP_RECOVER:					return "Spell Points (SP) will be increased by %d%% if you win.";
		case POINT_EXP_DOUBLE_BONUS:				return "Experience increases by %d%% if you win against an opponent.";
		case POINT_GOLD_DOUBLE_BONUS:				return "Increase of Yang up to %d%% if you win.";
		case POINT_ITEM_DROP_BONUS:					return "Increase of captured Items up to %d%% if you win.";
		case POINT_POTION_BONUS:					return "Power increase of up to %d%% after taking the potion.";
		case POINT_KILL_HP_RECOVERY:				return "%d%% Chance of filling up Hit Points after a victory.";
		case POINT_ATT_GRADE_BONUS:					return "Attack Power + %d";
		case POINT_DEF_GRADE_BONUS:					return "Armour + %d";
		case POINT_MAGIC_ATT_GRADE:					return "Magical Attack + %d";
		case POINT_MAGIC_DEF_GRADE:					return "Magical Defence + %d";
		case POINT_MAX_STAMINA:						return "Maximum Endurance + %d";
		case POINT_ATTBONUS_WARRIOR:				return "Strong against Warriors + %d%%";
		case POINT_ATTBONUS_ASSASSIN:				return "Strong against Ninjas + %d%%";
		case POINT_ATTBONUS_SURA:					return "Strong against Sura + %d%%";
		case POINT_ATTBONUS_SHAMAN:					return "Strong against Shamans + %d%%";
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_ATTBONUS_WOLFMAN:				return "???? ?? +%d%%";
#endif
		case POINT_ATTBONUS_MONSTER:				return "Strength against monsters + %d%%";
		case POINT_MALL_ATTBONUS:					return "Attack + %d%%";
		case POINT_MALL_DEFBONUS:					return "Defence + %d%%";
		case POINT_MALL_EXPBONUS:					return "Experience %d%%";
		case POINT_MALL_ITEMBONUS:					return "Chance to find an Item %. 1f";
		case POINT_MALL_GOLDBONUS:					return "Chance to find Yang %. 1f";
		case POINT_MAX_HP_PCT:						return "Maximum Energy +%d%%";
		case POINT_MAX_SP_PCT:						return "Maximum Energy +%d%%";
		case POINT_SKILL_DAMAGE_BONUS:				return "Skill Damage %d%%";
		case POINT_NORMAL_HIT_DAMAGE_BONUS:			return "Hit Damage %d%%";
		case POINT_SKILL_DEFEND_BONUS:				return "Resistance against Skill Damage %d%%";
		case POINT_NORMAL_HIT_DEFEND_BONUS:			return "Resistance against Hits %d%%";
		case POINT_RESIST_WARRIOR:					return "%d%% Resistance against Warrior Attacks";
		case POINT_RESIST_ASSASSIN:					return "%d%% Resistance against Ninja Attacks";
		case POINT_RESIST_SURA:						return "%d%% Resistance against Sura Attacks";
		case POINT_RESIST_SHAMAN:					return "%d%% Resistance against Shaman Attacks";
#ifdef ENABLE_WOLFMAN_CHARACTER
		case POINT_RESIST_WOLFMAN:					return "????? %d%% ??";
#endif
#ifdef ENABLE_CONQUEROR_LEVEL
		case POINT_SUNGMA_STR:						return "POINT_SUNGMA_STR %d%%";
		case POINT_SUNGMA_HP:						return "POINT_SUNGMA_HP %d%%";
		case POINT_SUNGMA_MOVE:						return "POINT_SUNGMA_MOVE %d%%";
		case POINT_SUNGMA_IMMUNE:					return "POINT_SUNGMA_IMMUNE %d%%";
		case POINT_CONQUEROR_POINT:					return "POINT_CONQUEROR_POINT %d%%";
#endif

		default:									return "UNK_ID %d%%"; // @fixme180
	}
}

static bool FN_hair_affect_string(LPCHARACTER ch, char *buf, size_t bufsiz)
{
	if (NULL == ch || NULL == buf)
		return false;

	CAffect* aff = NULL;
	time_t expire = 0;
	struct tm ltm;
	int	year, mon, day;
	int	offset = 0;

	aff = ch->FindAffect(AFFECT_HAIR);

	if (NULL == aff)
		return false;

	expire = ch->GetQuestFlag("hair.limit_time");

	if (expire < get_global_time())
		return false;

	// set apply string
	offset = snprintf(buf, bufsiz, FN_point_string(aff->bApplyOn), aff->lApplyValue);

	if (offset < 0 || offset >= (int) bufsiz)
		offset = bufsiz - 1;

	localtime_r(&expire, &ltm);

	year = ltm.tm_year + 1900;
	mon = ltm.tm_mon + 1;
	day = ltm.tm_mday;

	snprintf(buf + offset, bufsiz - offset, "(Procedure: %d y- %d m - %d d)", year, mon, day);

	return true;
}

ACMD(do_hair)
{
	char buf[256];

	if (false == FN_hair_affect_string(ch, buf, sizeof(buf)))
		return;

	ch->ChatPacket(CHAT_TYPE_INFO, "%s", buf);
}

ACMD(do_inventory)
{
	int	index = 0;
	int	count = 1;

	char arg1[256];
	char arg2[256];

	LPITEM	item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: inventory <start_index> <count>");
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
		str_to_number(index, arg1); index = MIN(index, INVENTORY_MAX_NUM);
		str_to_number(count, arg2); count = MIN(count, INVENTORY_MAX_NUM);
	}

	for (int i = 0; i < count; ++i)
	{
		if (index >= INVENTORY_MAX_NUM)
			break;

		item = ch->GetInventoryItem(index);

		ch->ChatPacket(CHAT_TYPE_INFO, "inventory [%d] = %s", index, item ? item->GetLocaleName() : "<NONE>");
		++index;
	}
}

ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}

ACMD(do_cube)
{
#ifndef ENABLE_RENEWAL_CUBE
	if (!ch->CanDoCube())
		return;

	int cube_index = 0, inven_index = 0;
#endif

	const char *line;
	char arg1[256], arg2[256], arg3[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (0 == arg1[0])
	{
#ifndef ENABLE_RENEWAL_CUBE
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: cube open");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube close");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube add <inveltory_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube delete <cube_index>");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube list");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube cancel");
		ch->ChatPacket(CHAT_TYPE_INFO, "       cube make [all]");
#endif
		return;
	}

#ifndef ENABLE_RENEWAL_CUBE
	const std::string& strArg1 = std::string(arg1);

	if (strArg1 == "r_info")
	{
		if (0 == arg2[0])
			Cube_request_result_list(ch);
		else
		{
			if (isdigit(*arg2))
			{
				int listIndex = 0, requestCount = 1;
				str_to_number(listIndex, arg2);

				if (0 != arg3[0] && isdigit(*arg3))
					str_to_number(requestCount, arg3);

				Cube_request_material_info(ch, listIndex, requestCount);
			}
		}

		return;
	}
#endif

	switch (LOWER(arg1[0]))
	{
		case 'o':
			Cube_open(ch);
			break;

#ifndef ENABLE_RENEWAL_CUBE
		case 'c':
			Cube_close(ch);
			break;

		case 'l':
			Cube_show_list(ch);
			break;

		case 'a':
			{
				if (0 == arg2[0] || !isdigit(*arg2) || 0 == arg3[0] || !isdigit(*arg3))
					return;

				str_to_number(cube_index, arg2);
				str_to_number(inven_index, arg3);
				Cube_add_item (ch, cube_index, inven_index);
			}
			break;

		case 'd':
			{
				if (0 == arg2[0] || !isdigit(*arg2))
					return;

				str_to_number(cube_index, arg2);
				Cube_delete_item (ch, cube_index);
			}
			break;

		case 'm':
			Cube_make(ch);
			break;
#endif

		default:
			return;
	}
}

ACMD(do_in_game_mall)
{
#ifdef ENABLE_ITEMSHOP
	ch->ChatPacket(CHAT_TYPE_COMMAND, "open_ishop");
#else
	if (LC_IsEurope() == true)
	{
		char country_code[3];

		switch (LC_GetLocalType())
		{
			case LC_GERMANY:	country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_FRANCE:		country_code[0] = 'f'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_ITALY:		country_code[0] = 'i'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_SPAIN:		country_code[0] = 'e'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_UK:			country_code[0] = 'e'; country_code[1] = 'n'; country_code[2] = '\0'; break;
			case LC_TURKEY:		country_code[0] = 't'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_POLAND:		country_code[0] = 'p'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_PORTUGAL:	country_code[0] = 'p'; country_code[1] = 't'; country_code[2] = '\0'; break;
			case LC_GREEK:		country_code[0] = 'g'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_RUSSIA:		country_code[0] = 'r'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_DENMARK:	country_code[0] = 'd'; country_code[1] = 'k'; country_code[2] = '\0'; break;
			case LC_BULGARIA:	country_code[0] = 'b'; country_code[1] = 'g'; country_code[2] = '\0'; break;
			case LC_CROATIA:	country_code[0] = 'h'; country_code[1] = 'r'; country_code[2] = '\0'; break;
			case LC_MEXICO:		country_code[0] = 'm'; country_code[1] = 'x'; country_code[2] = '\0'; break;
			case LC_ARABIA:		country_code[0] = 'a'; country_code[1] = 'e'; country_code[2] = '\0'; break;
			case LC_CZECH:		country_code[0] = 'c'; country_code[1] = 'z'; country_code[2] = '\0'; break;
			case LC_ROMANIA:	country_code[0] = 'r'; country_code[1] = 'o'; country_code[2] = '\0'; break;
			case LC_HUNGARY:	country_code[0] = 'h'; country_code[1] = 'u'; country_code[2] = '\0'; break;
			case LC_NETHERLANDS: country_code[0] = 'n'; country_code[1] = 'l'; country_code[2] = '\0'; break;
			case LC_USA:		country_code[0] = 'u'; country_code[1] = 's'; country_code[2] = '\0'; break;
			case LC_CANADA:	country_code[0] = 'c'; country_code[1] = 'a'; country_code[2] = '\0'; break;
			default:
				if (test_server == true)
				{
					country_code[0] = 'd'; country_code[1] = 'e'; country_code[2] = '\0';
				}
				break;
		}

	char buf[512+1];
	char sas[33];
	MD5_CTX ctx;
	const char sas_key[] = "GF9001";

	char language[3];

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	if (ch->GetDesc() != NULL)
		strlcpy(language, get_locale(ch->GetLanguage()), sizeof(language));
	else
		strlcpy(language, "en", sizeof(language));
#else
	strlcpy(language, "en", sizeof(language));
#endif

	snprintf(buf, sizeof(buf), "%u%u%s", ch->GetPlayerID(), ch->GetAID(), sas_key);

	MD5Init(&ctx);
	MD5Update(&ctx, (const unsigned char *) buf, strlen(buf));
#ifdef __FreeBSD__
	MD5End(&ctx, sas);
#else
	static const char hex[] = "0123456789abcdef";
	unsigned char digest[16];
	MD5Final(digest, &ctx);
	int i;
	for (i = 0; i < 16; ++i) {
		sas[i+i] = hex[digest[i] >> 4];
		sas[i+i+1] = hex[digest[i] & 0x0f];
	}
	sas[i+i] = '\0';
#endif

	snprintf(buf, sizeof(buf), "mall https://www.%s/shop?pid=%u&lang=%s&sid=%d&sas=%s", g_strWebMallURL.c_str(), ch->GetPlayerID(), language, g_server_id, sas);

	ch->ChatPacket(CHAT_TYPE_COMMAND, "%s", buf);
#endif
}

ACMD(do_dice)
{
	char arg1[256], arg2[256];
	int start = 1, end = 100;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2)
	{
		char* endptr1, *endptr2;
		errno = 0;
		long val1 = strtol(arg1, &endptr1, 10);
		long val2 = strtol(arg2, &endptr2, 10);
		
		if (*endptr1 != '\0' || *endptr2 != '\0' || errno == ERANGE)
			return;
		
		if (val1 < 1 || val1 > 100000 || val2 < 1 || val2 > 100000)
			return;
		
		start = (int)val1;
		end = (int)val2;
	}
	else if (*arg1 && !*arg2)
	{
		char* endptr;
		errno = 0;
		long val = strtol(arg1, &endptr, 10);
		
		if (*endptr != '\0' || errno == ERANGE)
			return;
		
		if (val < 1 || val > 100000)
			return;
		
		start = 1;
		end = (int)val;
	}

	if (start > end)
		std::swap(start, end);

	int n = number(start, end);

#ifdef ENABLE_DICE_SYSTEM
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_DICE_INFO, 309, "%s#%d#%d#%d", ch->GetName(), n, start, end);
	else
		ch->ChatPacket(CHAT_TYPE_DICE_INFO, LC_TEXT("You rolled dice. You got %d (%d-%d)."), n, start, end);
#else
	if (ch->GetParty())
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_INFO, 309, "%s#%d#%d#%d", ch->GetName(), n, start, end);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You rolled dice. You got %d (%d-%d)."), n, start, end);
#endif
}

ACMD(do_click_safebox)
{
	if (ch->IsDead())
		return;

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 2090, "");
		return;
	}

	ch->SetSafeboxOpenPosition();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
}

ACMD(do_click_mall)
{
	if (ch->IsDead())
		return;

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 2090, "");
		return;
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

ACMD(do_ride)
{
    if (ch->IsDead() || ch->IsStun() || ch->IsPolymorphed())
	return;

    int CalcLastAttackSec = (get_dword_time() - ch->GetLastAttackedTime()) / 1000 + 0.5;

    if (CalcLastAttackSec < 10)
    {
        ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait a few seconds after being attacked!"));
        return;
    }

	if (ch->GetMapIndex() == 113)
		return;

#ifdef ENABLE_MOUNT_SYSTEM
	if (ch->IsPolymorphed() == true)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 882, "");
		return;
	}

	if(ch->GetWear(WEAR_MOUNT))
	{
		CMountSystem* mountSystem = ch->GetMountSystem();
		LPITEM mount = ch->GetWear(WEAR_MOUNT);
#ifdef ENABLE_MOUNT_PET_SKIN
		LPITEM mountSkinItem = ch->GetWear(WEAR_COSTUME_MOUNT);
#endif

		if (!mountSystem && !mount)
			return;

#ifdef ENABLE_MOUNT_PET_SKIN
		DWORD mobVnum = mountSkinItem ? mountSkinItem->GetValue(1) : mount->GetValue(1);
#else
		DWORD mobVnum = mount->GetValue(1);
#endif

		if (ch->GetMountVnum())
		{
			if(mountSystem->CountSummoned() == 0)
			{
				mountSystem->Unmount(mobVnum);
			}
		}
		else
		{
			if(mountSystem->CountSummoned() == 1)
			{
				mountSystem->Mount(mobVnum, mount);
			}
		}
		
		return;
	}
#endif

	if (ch->IsHorseRiding())
	{
		ch->StopRiding();
		return;
	}

	if (ch->GetHorse() != NULL)
	{
		ch->StartRiding();
		return;
	}

	for (BYTE i=0; i<INVENTORY_MAX_NUM; ++i)
	{
		LPITEM item = ch->GetInventoryItem(i);
		if (NULL == item)
			continue;

		if (item->GetType() == ITEM_MOUNT)
		{
			ch->UseItem(TItemPos (INVENTORY, i));
			return;
		}
	}

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
}

#ifdef ENABLE_CHANGE_CHANNEL
ACMD(do_change_channel)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD channel_number = 0;
	str_to_number(channel_number, arg1);

	if (!ch)
	{
		return;
	}

	if (channel_number == 99 || g_bChannel == 99)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 61, "");
		return;
	}

	if (channel_number == g_bChannel)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 312, "");
		return;
	}

	if (ch->IsDead() || !ch->CanWarp())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 313, "");
		return;
	}

	if (channel_number <= 0 || channel_number > 6)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 314, "");
		return;
	}

	if (channel_number != 0)
	{
		if (ch->m_pkChangeChannelEvent)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 161, "");
			event_cancel(&ch->m_pkChangeChannelEvent);
			return;
		}

		ChangeChannelEventInfo* info = AllocEventInfo<ChangeChannelEventInfo>();

		{
			if (ch->IsPosition(POS_FIGHTING))
				info->left_second = 10;
			else
				info->left_second = 3;
		}

		info->ch = ch;
		info->channel_number = channel_number;

		ch->m_pkChangeChannelEvent = event_create(change_channel_event, info, 1);
	}
}
#endif

#ifdef ENABLE_SORT_INVENTORY
bool sortByType(CItem* a, CItem* b)
{
	return (a->GetType() < b->GetType());
}

bool sortBySubType(CItem* a, CItem* b)
{
	return (a->GetSubType() < b->GetSubType());
}

bool sortByVnum(CItem* a, CItem* b)
{
	return (a->GetVnum() < b->GetVnum());
}

bool sortBySocket(CItem* a, CItem* b)
{
	return (a->GetSocket(0) < b->GetSocket(0));
}

ACMD(do_sort_items)
{
	if (ch->IsDead())
		return;

	if (ch->GetExchange() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsOpenSafebox() || ch->IsCubeOpen())
		return;

	int lastSortInventoryPulse = ch->GetSortInventoryPulse();
	int currentPulse = thecore_pulse();

	if (lastSortInventoryPulse > currentPulse)
	{
		int deltaInSeconds = ((lastSortInventoryPulse / PASSES_PER_SEC(1)) - (currentPulse / PASSES_PER_SEC(1)));
		int minutes = deltaInSeconds / 60;
		int seconds = (deltaInSeconds - (minutes * 60));
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can sort your inventory again in %d seconds."), seconds);
		return;
	}

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	int UNLOCK_SLOTS = 90+(5*ch->GetUnlockSlotsW());
	for (int i = 0; i < UNLOCK_SLOTS; ++i)
#else
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#endif
	{
		LPITEM item = ch->GetInventoryItem(i);

		if(!item)
			continue;

		if(item->isLocked())
			continue;

		if(item->GetCount() == ITEM_MAX_COUNT)
			continue;

		if (item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
		{
#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
			int UNLOCK_SLOTS = 90+(5*ch->GetUnlockSlotsW());
			for (int j = i; j < UNLOCK_SLOTS; ++j)
#else
			for (int j = i; j < INVENTORY_MAX_NUM; ++j)
#endif
			{
				LPITEM item2 = ch->GetInventoryItem(j);

				if(!item2)
					continue;

				if(item2->isLocked())
					continue;

				if (item2->GetVnum() == item->GetVnum())
				{
					bool bStopSockets = false;

					for (int k = 0; k < ITEM_SOCKET_MAX_NUM; ++k)
					{
						if (item2->GetSocket(k) != item->GetSocket(k))
						{
							bStopSockets = true;
							break;
						}
					}

					if(bStopSockets)
						continue;

#ifdef ENABLE_STACK_LIMIT
					WORD bAddCount = MIN(ITEM_MAX_COUNT - item->GetCount(), item2->GetCount());
#else
					BYTE bAddCount = MIN(ITEM_MAX_COUNT - item->GetCount(), item2->GetCount());
#endif

					item->SetCount(item->GetCount() + bAddCount);
					item2->SetCount(item2->GetCount() - bAddCount);
					continue;
				}
			}
		}
	}

	ch->SetNextSortInventoryPulse(thecore_pulse() + PASSES_PER_SEC(10));
}

// Reference inventory "Sort" button: order the main inventory by type / sub type / vnum and pack it from the
// first cell. (Anka2's /click_sort_items only merges stacks; it is the "Stack" button now.)
ACMD(do_sort_inventory)
{
	if (ch->IsDead() || !ch->CanHandleItem())
		return;

	if (ch->GetExchange() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsOpenSafebox() || ch->IsCubeOpen())
		return;

	static std::unordered_map<DWORD, int> s_mapNextSort;	// pid -> pulse; separate from the stack button
	int& rNextSort = s_mapNextSort[ch->GetPlayerID()];
	if (rNextSort > thecore_pulse())
	{
		const int iSec = (rNextSort - thecore_pulse()) / PASSES_PER_SEC(1) + 1;
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can sort your inventory again in %d seconds."), iSec);
		return;
	}

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	const int iMaxCell = MIN(INVENTORY_MAX_NUM, 90 + (5 * ch->GetUnlockSlotsW()));
#else
	const int iMaxCell = INVENTORY_MAX_NUM;
#endif

	std::vector<std::pair<LPITEM, int>> vecItems;	// item, old cell
	for (int i = 0; i < iMaxCell; ++i)
	{
		LPITEM item = ch->GetInventoryItem(i);
		if (!item)
			continue;
		if (item->isLocked() || item->IsExchanging())
			continue;	// stays in its cell, the others are placed around it
		vecItems.push_back(std::make_pair(item, i));
	}

	std::stable_sort(vecItems.begin(), vecItems.end(), [](const std::pair<LPITEM, int>& a, const std::pair<LPITEM, int>& b)
	{
		if (a.first->GetType() != b.first->GetType())
			return a.first->GetType() < b.first->GetType();
		if (a.first->GetSubType() != b.first->GetSubType())
			return a.first->GetSubType() < b.first->GetSubType();
		if (a.first->GetVnum() != b.first->GetVnum())
			return a.first->GetVnum() < b.first->GetVnum();
		return a.first->GetCount() > b.first->GetCount();
	});

	for (auto& it : vecItems)
		it.first->RemoveFromCharacter();

	std::map<int, int> mapNewCell;	// old cell -> new cell
	for (auto& it : vecItems)
	{
		LPITEM item = it.first;
		const int iCell = ch->GetEmptyInventory(item->GetSize());
		if (iCell < 0)
		{
			// cannot happen (the same items were there before); never lose an item
			ch->AutoGiveItem(item);
			continue;
		}
#ifdef ENABLE_PICKUP_ITEM_EFFECT
		item->AddToCharacter(ch, TItemPos(INVENTORY, iCell), false);
#else
		item->AddToCharacter(ch, TItemPos(INVENTORY, iCell));
#endif
		mapNewCell[it.second] = iCell;
	}

	// quickslots follow their items (cleared first so two slots never point at the same cell meanwhile)
	TQuickslot aSlot[QUICKSLOT_MAX_NUM];
	bool abMove[QUICKSLOT_MAX_NUM] = {};
	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
	{
		TQuickslot* pSlot = NULL;
		if (!ch->GetQuickslot(i, &pSlot) || !pSlot || pSlot->type != QUICKSLOT_TYPE_ITEM)
			continue;
		auto it = mapNewCell.find(pSlot->pos);
		if (it == mapNewCell.end() || it->second == pSlot->pos)
			continue;
		aSlot[i].type = QUICKSLOT_TYPE_ITEM;
		aSlot[i].pos = (BYTE) it->second;
		abMove[i] = true;
		ch->DelQuickslot(i);
	}
	for (int i = 0; i < QUICKSLOT_MAX_NUM; ++i)
		if (abMove[i])
			ch->SetQuickslot(i, aSlot[i]);

	rNextSort = thecore_pulse() + PASSES_PER_SEC(10);
}

ACMD (do_sort_special_storage)
{
	if (!ch)
		return;

	if (!ch->CanHandleItem())
		return;

	int lastSortSpecialStoragePulse = ch->GetSortSpecialStoragePulse();
	int currentPulse = thecore_pulse();

	if (lastSortSpecialStoragePulse > currentPulse)
	{
		int deltaInSeconds = ((lastSortSpecialStoragePulse / PASSES_PER_SEC(1)) - (currentPulse / PASSES_PER_SEC(1)));
		int minutes = deltaInSeconds / 60;
		int seconds = (deltaInSeconds - (minutes * 60));

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can sort your inventory again in %d seconds."), seconds);
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	std::vector<CItem*> collectItems;

	for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
	{
		if (!ch)
			continue;

		LPITEM item = ch->GetSkillBookInventoryItem(i);

		if (item)
		{
			collectItems.push_back(item);
		}
	}

	for (int i = UPGRADE_ITEMS_INVENTORY_SLOT_START; i < UPGRADE_ITEMS_INVENTORY_SLOT_END; ++i)
	{
		if (!ch)
			continue;

		LPITEM item = ch->GetUpgradeItemsInventoryItem(i);

		if (item)
		{
			collectItems.push_back(item);
		}
	}

	for (int i = STONE_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
	{
		if (!ch)
			continue;

		LPITEM item = ch->GetStoneInventoryItem(i);

		if (item)
		{
			collectItems.push_back(item);
		}
	}

	for (int i = GIFT_BOX_INVENTORY_SLOT_START; i < GIFT_BOX_INVENTORY_SLOT_END; ++i)
	{
		if (!ch)
			continue;

		LPITEM item = ch->GetGiftBoxInventoryItem(i);

		if (item)
		{
			collectItems.push_back(item);
		}
	}

	for (int i = CHANGERS_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
	{
		if (!ch)
			continue;

		LPITEM item = ch->GetChangersInventoryItem(i);

		if (item)
		{
			collectItems.push_back(item);
		}
	}

	for (std::vector<CItem*>::iterator it = collectItems.begin() ; it != collectItems.end(); ++it)
	{
		if (!ch)
			continue;

		LPITEM item = *it;
		item->RemoveFromCharacter();
	}

	std::sort(collectItems.begin(), collectItems.end(), sortByType);
	std::sort(collectItems.begin(), collectItems.end(), sortBySubType);
	std::sort(collectItems.begin(), collectItems.end(), sortByVnum);
	std::sort(collectItems.begin(), collectItems.end(), sortBySocket);

	for (std::vector<CItem*>::iterator iit = collectItems.begin(); iit < collectItems.end(); ++iit)
	{
		if (!ch)
			continue;

		LPITEM sortedItem = *iit;
		if (sortedItem)
		{
#ifdef ENABLE_STACK_LIMIT
			WORD dwCount = sortedItem->GetCount();
#else
			BYTE dwCount = sortedItem->GetCount();
#endif
			if (sortedItem->IsStackable() && !IS_SET(sortedItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
			{
				for (int i = SKILL_BOOK_INVENTORY_SLOT_START; i < SKILL_BOOK_INVENTORY_SLOT_END; ++i)
				{
					LPITEM item2 = ch->GetSkillBookInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == sortedItem->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						{
							if (item2->GetSocket(j) != sortedItem->GetSocket(j))
							{
								break;
							}
						}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

#ifdef ENABLE_STACK_LIMIT
						WORD dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
#else
						BYTE dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
#endif
						dwCount -= dwCount2;

						item2->SetCount(item2->GetCount() + dwCount2);

						if (dwCount == 0)
						{
							M2_DESTROY_ITEM(sortedItem);
							break;
						}
						else
						{
							sortedItem->SetCount(dwCount);
						}
					}
				}

				for (int i = UPGRADE_ITEMS_INVENTORY_SLOT_START; i < UPGRADE_ITEMS_INVENTORY_SLOT_END; ++i)
				{
					LPITEM item2 = ch->GetUpgradeItemsInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == sortedItem->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						{
							if (item2->GetSocket(j) != sortedItem->GetSocket(j))
							{
								break;
							}
						}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

#ifdef ENABLE_STACK_LIMIT
						WORD dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
#else
						BYTE dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
#endif
						dwCount -= dwCount2;

						item2->SetCount(item2->GetCount() + dwCount2);

						if (dwCount == 0)
						{
							M2_DESTROY_ITEM(sortedItem);
							break;
						}
						else
						{
							sortedItem->SetCount(dwCount);
						}
					}
				}

				for (int i = STONE_INVENTORY_SLOT_START; i < STONE_INVENTORY_SLOT_END; ++i)
				{
					LPITEM item2 = ch->GetStoneInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == sortedItem->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						{
							if (item2->GetSocket(j) != sortedItem->GetSocket(j))
							{
								break;
							}
						}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

#ifdef ENABLE_STACK_LIMIT
						WORD dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
#else
						BYTE dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
#endif
						dwCount -= dwCount2;

						item2->SetCount(item2->GetCount() + dwCount2);

						if (dwCount == 0)
						{
							M2_DESTROY_ITEM(sortedItem);
							break;
						}
						else
						{
							sortedItem->SetCount(dwCount);
						}
					}
				}

				for (int i = GIFT_BOX_INVENTORY_SLOT_START; i < GIFT_BOX_INVENTORY_SLOT_END; ++i)
				{
					LPITEM item2 = ch->GetGiftBoxInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == sortedItem->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						{
							if (item2->GetSocket(j) != sortedItem->GetSocket(j))
							{
								break;
							}
						}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

#ifdef ENABLE_STACK_LIMIT
						WORD dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
#else
						BYTE dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
#endif
						dwCount -= dwCount2;

						item2->SetCount(item2->GetCount() + dwCount2);

						if (dwCount == 0)
						{
							M2_DESTROY_ITEM(sortedItem);
							break;
						}
						else
						{
							sortedItem->SetCount(dwCount);
						}
					}
				}

				for (int i = CHANGERS_INVENTORY_SLOT_START; i < CHANGERS_INVENTORY_SLOT_END; ++i)
				{
					LPITEM item2 = ch->GetChangersInventoryItem(i);

					if (!item2)
					{
						continue;
					}

					if (item2->GetVnum() == sortedItem->GetVnum())
					{
						int j;

						for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
						{
							if (item2->GetSocket(j) != sortedItem->GetSocket(j))
							{
								break;
							}
						}

						if (j != ITEM_SOCKET_MAX_NUM)
						{
							continue;
						}

#ifdef ENABLE_STACK_LIMIT
						WORD dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
#else
						BYTE dwCount2 = MIN(ITEM_MAX_COUNT - item2->GetCount(), dwCount);
#endif
						dwCount -= dwCount2;

						item2->SetCount(item2->GetCount() + dwCount2);

						if (dwCount == 0)
						{
							M2_DESTROY_ITEM(sortedItem);
							break;
						}
						else
						{
							sortedItem->SetCount(dwCount);
						}
					}
				}
			}

			if (dwCount > 0)
			{
				int cell;

				if (sortedItem->IsSkillBook())
					cell = ch->GetEmptySkillBookInventory(sortedItem->GetSize());
				else if (sortedItem->IsUpgradeItem())
					cell = ch->GetEmptyUpgradeItemsInventory(sortedItem->GetSize());
				else if(sortedItem->IsStone())
					cell = ch->GetEmptyStoneInventory(sortedItem->GetSize());
				else if(sortedItem->IsGiftBox())
					cell = ch->GetEmptyGiftBoxInventory(sortedItem->GetSize());
				else if(sortedItem->IsChanger())
					cell = ch->GetEmptyChangersInventory(sortedItem->GetSize());

				sortedItem->AddToCharacter(ch, TItemPos(INVENTORY, cell));
			}
		}
	}

	ch->SetNextSortSpecialStoragePulse(thecore_pulse() + PASSES_PER_SEC(10));
}
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
ACMD(do_hide_costume)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
		return;

	bool hidden = true;
	BYTE bPartPos = 0;
	BYTE bHidden = 0;

	str_to_number(bPartPos, arg1);

	if (*arg2)
	{
		str_to_number(bHidden, arg2);

		if (bHidden == 0)
			hidden = false;
	}

	if (ch->IsDead())
		return;

	bool bAttacking = (get_dword_time() - ch->GetLastAttackTime()) < 1500;
	bool bMoving = (get_dword_time() - ch->GetLastMoveTime()) < 1500;
	bool bDelayedCMD = false;

	if (ch->IsStateMove() || bAttacking || bMoving)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to stand still to hide your costume."));
		return;
	}

	if (bDelayedCMD)
	{
		int iPulse = thecore_pulse();
		if (iPulse - ch->GetHideCostumePulse() < passes_per_sec * 3)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to wait 3 seconds before you can hide your costume again."));
			return;
		}
		ch->SetHideCostumePulse(iPulse);
	}

	if (bPartPos == 1)
		ch->SetBodyCostumeHidden(hidden);
	else if (bPartPos == 2)
		ch->SetHairCostumeHidden(hidden);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	else if (bPartPos == 3)
		ch->SetAcceCostumeHidden(hidden);
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	else if (bPartPos == 4)
		ch->SetWeaponCostumeHidden(hidden);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	else if (bPartPos == 5)
		ch->SetAuraCostumeHidden(hidden);
#endif
	else
		return;

	ch->UpdatePacket();
}
#endif

#ifdef ENABLE_RESTART_INSTANT
ACMD(do_restart_now)
{
	if (false == ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->StartRecoveryEvent();
		return;
	}

	if (ch->GetGold() < 500000)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 785, "");
		return;
	}

	if (NULL == ch->m_pkDeadEvent)
		return;

	int iTimeToDead = (event_time(ch->m_pkDeadEvent) / passes_per_sec);

	if (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG)
	{
		if (!test_server)
		{
			if (ch->IsHack())
			{
				if ((!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 320, "");
					return;
				}
			}
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->GetDesc()->SetPhase(PHASE_GAME);
		ch->SetPosition(POS_STANDING);
		ch->StartRecoveryEvent();
		ch->RestartAtSamePos();
		ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
		ch->DeathPenalty(0);
		ch->ReviveInvisible(5);
		ch->PointChange(POINT_GOLD, -500000, false);
	}
}
#endif

#ifdef ENABLE_ANTI_EXP
ACMD(do_anti_exp)
{
	time_t real_time = time(0);
	if (ch->GetProtectTime("anti.exp") > real_time)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 437, "%d", ch->GetProtectTime("anti.exp") - real_time);
		return;
	}

	ch->SetProtectTime("anti.exp", real_time + 3);
	ch->SetAntiExp(!ch->GetAntiExp());
	ch->ChatPacket(CHAT_TYPE_COMMAND, "SetAntiExp %d", ch->GetAntiExp() ? 1:0);
}
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
ACMD(do_multi_farm)
{
	if (!ch->GetDesc())
		return;

	if (ch->GetProtectTime("multi-farm") > get_global_time())
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 437, "%d", ch->GetProtectTime("multi-farm") - get_global_time());
		return;
	}

	ch->SetProtectTime("multi-farm", get_global_time() + 10);

	CHARACTER_MANAGER::instance().CheckMultiFarmAccount(ch->GetDesc()->GetHostName(), ch->GetPlayerID(), ch->GetName(), !ch->GetRewardStatus());
}
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
ACMD(do_setpickupmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD flag = 0;
		str_to_number(flag, arg1);
		ch->ChangePickUPMode(flag);
	}
}
#endif

#ifdef ENABLE_MAINTENANCE_SYSTEM
ACMD(do_maintenance_text)
{
	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_NOTICE, "<Syntax> The arguments available for this command are:");
		ch->ChatPacket(CHAT_TYPE_NOTICE, "<Syntax> /m_text disable");
		ch->ChatPacket(CHAT_TYPE_NOTICE, "<Syntax> /m_text enable <text>");
		return;
	}

	if (*arg1 && !strcmp(arg1, "disable"))
	{
		MaintenanceManager::instance().Send_Text(ch, "rmf");
	}

	else if (*arg1 && !strcmp(arg1, "enable"))
	{
		const char* sReason = one_argument(argument, arg2, sizeof(arg2));
		MaintenanceManager::instance().Send_Text(ch, sReason);
	}
}

ACMD(do_maintenance)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && !strcmp(arg1, "force_stop"))
	{
		MaintenanceManager::instance().Send_DisableSecurity(ch);
	}
	else
	{
		long time_maintenance = parse_time_str(arg1);
		long duration_maintenance = parse_time_str(arg2);

		MaintenanceManager::instance().Send_ActiveMaintenance(ch, time_maintenance, duration_maintenance);
	}
}
#endif

#ifdef ENABLE_RENEWAL_SKILL_SELECT
ACMD(do_renewal_skill_select)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->GetSkillGroup() != 0 || ch->GetLevel() < 5)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 88, "");
		return;
	}

	int iValue = 0;
	str_to_number(iValue, arg1);
	ch->SetSkillGroup(iValue);
	ch->ClearSkill();
}
#endif

#ifdef ENABLE_ITEMSHOP
ACMD(do_ishop)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);

	if (vecArgs.size() < 2)
	{
		return;
	}
	else if (vecArgs[1] == "data")
	{
		if (ch->GetProtectTime("itemshop.load") == 1)
		{
			return;
		}

		ch->SetProtectTime("itemshop.load", 1);

		if (vecArgs.size() < 3)
		{
			return;
		}

		int updateTime;
		str_to_number(updateTime, vecArgs[2].c_str());

		CHARACTER_MANAGER::Instance().LoadItemShopData(ch, CHARACTER_MANAGER::Instance().GetItemShopUpdateTime() != updateTime);
	}
	else if (vecArgs[1] == "log")
	{
		if (ch->GetProtectTime("itemshop.log") == 1)
		{
			return;
		}

		ch->SetProtectTime("itemshop.log", 1);

		CHARACTER_MANAGER::Instance().LoadItemShopLog(ch);
	}
	else if (vecArgs[1] == "buy")
	{
		if (vecArgs.size() < 4)
		{
			return;
		}

		int itemID;
		str_to_number(itemID, vecArgs[2].c_str());

		int itemCount;
		str_to_number(itemCount, vecArgs[3].c_str());

/* check already in: LoadItemShopBuy
		if (itemCount < 1 || itemCount > 20)
		{
			return;
		}
*/
		CHARACTER_MANAGER::Instance().LoadItemShopBuy(ch, itemID, itemCount);
	}
	else if (vecArgs[1] == "wheel")
	{
		if (vecArgs.size() < 3)
		{
			return;
		}
		else if (vecArgs[2] == "start")
		{
			if (vecArgs.size() < 4)
			{
				return;
			}

			BYTE ticketType;
			if (!str_to_number(ticketType, vecArgs[3].c_str()))
			{
				return;
			}

			if (ticketType > 1)
			{
				return;
			}
			else if (ch->GetProtectTime("WheelWorking") != 0)
			{
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 683, "");
				return;
			}

			if (ticketType == 0)
			{
				if (ch->CountSpecifyItem(80013) <= 0)
				{
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 684, "");
					return;
				}

				ch->RemoveSpecifyItem(80013, 1);
			}
			else if (ticketType == 1)
			{
				long long act_lldCoins;
#ifdef USE_ITEMSHOP_RENEWED
				long long act_lldJCoins;
				ch->GetAccountMoney(act_lldCoins, act_lldJCoins);
#else
				ch->GetAccountMoney(act_lldCoins);
#endif

				if (act_lldCoins < 10)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "997");
					return;
				}

#ifdef USE_ITEMSHOP_RENEWED
				ch->SetAccountMoney(10, 0, false);
				ch->GetAccountMoney(act_lldCoins, act_lldJCoins);
#else
				ch->SetAccountMoney(10, false);
				ch->GetAccountMoney(act_lldCoins);
#endif

				LPDESC d = ch->GetDesc();
				if (d)
				{
					LogManager::Instance().WheelOfFortuneLog(d->GetAccountTable().id, ch->GetPlayerID(), ch->GetMapIndex(), ch->GetX(), ch->GetY(), 10);

					if (ch->GetProtectTime("itemshop.log") == 1)
					{
						char szIP[16] = {};
						strlcpy(szIP, d->GetHostName(), sizeof(szIP));

						char szTime[21];
						time_t now = time(0);
						struct tm tstruct = *localtime(&now);
						strftime(szTime, sizeof(szTime), "%Y-%m-%d %X", &tstruct);

						ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopAppendLog %s %d %s %s 1 1 5", szTime, time(0), ch->GetName(), szIP);
					}
				}
			}

			// Important items
			std::vector<std::pair<long, long>> m_important_item = {
				{72320, 1},
				{72320, 2},
				{72319, 1},
				{72319, 2},
				{70063, 1},
				{70064, 1},
				{84014, 1},
			};

			// normal items
			std::map<std::pair<long, long>, int> m_normal_item = {
				{{71084, 5}, 30},
				 {{71084, 10}, 30},
				 {{71084, 10}, 30},
				 {{71084, 20}, 30},
				 {{71084, 20}, 30},
				 {{71084, 100}, 30},
				 {{70005, 1}, 30},
				 {{25041, 1}, 30},
				 {{25041, 2}, 10},
				 {{71035, 1}, 30},
				 {{70043, 1}, 30},
				 {{71155, 1}, 30},
				 {{85001, 1}, 30},
				 {{80003, 1}, 30},
			};

			std::vector<std::pair<long, long>> m_send_items;
			if (m_important_item.size())
			{
				int random = number(0,m_important_item.size()-1);
				m_send_items.emplace_back(m_important_item[random].first, m_important_item[random].second);
			}

			while (true)
			{
				for (auto it = m_normal_item.begin(); it != m_normal_item.end(); ++it)
				{
					int randomEx = number(0,4);
					if (randomEx == 4)
					{
						int random = number(0,100);
						if (it->second >= random)
						{
							auto itFind = std::find(m_send_items.begin(), m_send_items.end(), it->first);
							if (itFind == m_send_items.end())
							{
								m_send_items.emplace_back(it->first.first, it->first.second);
								if (m_send_items.size() >= 10)
								{
									break;
								}
							}
						}
					}
				}

				if (m_send_items.size() >= 10)
				{
					break;
				}
			}

			std::string cmd_wheel = "";

			if (!m_send_items.empty())
			{
				for (auto it = m_send_items.begin(); it != m_send_items.end(); ++it)
				{
					cmd_wheel += std::to_string(it->first);
					cmd_wheel += "|";
					cmd_wheel += std::to_string(it->second);
					cmd_wheel += "#";
				}
			}

			int luckyWheel = number(0, 9);
			{
				if (luckyWheel == 0)
				{
					if (number(0, 1) == 0)
					{
						luckyWheel = number(0, 9);
					}
				}
			}

			ch->SetProtectTime("WheelLuckyIndex", luckyWheel);
			ch->SetProtectTime("WheelLuckyItemVnum", m_send_items[luckyWheel].first);
			ch->SetProtectTime("WheelLuckyItemCount", m_send_items[luckyWheel].second);
			ch->SetProtectTime("WheelWorking", 1);

			ch->ChatPacket(CHAT_TYPE_COMMAND, "SetWheelItemData %s", cmd_wheel.c_str());
			ch->ChatPacket(CHAT_TYPE_COMMAND, "OnSetWhell %d", luckyWheel);
		}
		else if (vecArgs[2] == "done")
		{
			if (ch->GetProtectTime("WheelWorking") == 0)
			{
				return;
			}

			ch->AutoGiveItem(ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));

			ch->ChatPacket(CHAT_TYPE_COMMAND, "GetWheelGiftData %d %d", ch->GetProtectTime("WheelLuckyItemVnum"), ch->GetProtectTime("WheelLuckyItemCount"));

			ch->SetProtectTime("WheelLuckyIndex", 0);
			ch->SetProtectTime("WheelLuckyItemVnum", 0);
			ch->SetProtectTime("WheelLuckyItemCount", 0);
			ch->SetProtectTime("WheelWorking", 0);
		}
	}
	else if (vecArgs[1] == "currency")
	{
		ch->RefreshAccountMoney();
	}
}
#endif

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
ACMD(do_reload_inventory)
{
	if (!ch)
		return;

	for (int i = 0; i < UNLOCK_INVENTORY_MAX; ++i)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ManagerInventoryUnlock Hide|%d|%d", ch->GetUnlockSlotsW(i),i);
	}
}

ACMD(do_unlock_inventory)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	DWORD type_inventory;
	str_to_number(type_inventory, arg1);

	DWORD inv = ch->GetUnlockSlotsW(type_inventory);
	int cant;

	if (inv > 0)
	{
		cant = (inv+1)*2;
		if (ch->CountSpecifyItem(72320) >= cant)
		{
			ch->RemoveSpecifyItem(72320, cant);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "ManagerInventoryUnlock Hide|%d|%d", inv+1, type_inventory);
			ch->SetUnlockSlotsW(inv+1, type_inventory);
		}
		else
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 112, "%d", cant);
		}
	}
	else
	{
		cant = 2;
		if (ch->CountSpecifyItem(72320) >= cant)
		{
			ch->RemoveSpecifyItem(72320, cant);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "ManagerInventoryUnlock Hide|%d|%d", 1, type_inventory);
			ch->SetUnlockSlotsW(1, type_inventory);
		}
		else
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 112, "%d", cant);
		}
	}
}
#endif

#ifdef ENABLE_RENEWAL_TELEPORT_SYSTEM
ACMD(do_open_warping_window)
{
	if (!ch)
		return;

	ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_OpenWarpWindow");
}

ACMD(do_warp_on)
{
	if (!ch)
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	int map_index;
	str_to_number(map_index, arg1);

	ch->StartWarpOn(map_index);
}
#endif

#ifdef ENABLE_ZODIAC_MISSION
ACMD(do_cz_reward)
{
	if (ch->GetProtectTime("Zodiac12ZiReward") > get_global_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Biraz beklemelisin.");
		return;
	}
	ch->SetProtectTime("Zodiac12ZiReward",get_global_time()+1);

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Eksik islem uygulandi!");
		return;
	}

	BYTE type = 0;
	str_to_number(type, arg1);
	int FYellow = ch->GetQuestFlag("Quest_ZodiacTemple.YellowReward");
	int FGreen = ch->GetQuestFlag("Quest_ZodiacTemple.GreenReward");

	if (type == 1)
	{
		if (ch->GetQuestFlag("Quest_ZodiacTemple.YellowMark") == 1073741823)
		{
			ch->AutoGiveItem(33026, 1);
			ch->SetQuestFlag("Quest_ZodiacTemple.YellowMark",0);
			ch->SetQuestFlag("Quest_ZodiacTemple.YellowReward",FYellow+1);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Yeterince sar? sanduka puan?n yok!");
			return;
		}
	}
	else if (type == 2)
	{
		if (ch->GetQuestFlag("Quest_ZodiacTemple.GreenMark") == 1073741823)
		{
			ch->AutoGiveItem(33027, 1);
			ch->SetQuestFlag("Quest_ZodiacTemple.GreenReward",FGreen+1);
			ch->SetQuestFlag("Quest_ZodiacTemple.GreenMark",0);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Yeterince Ye?il sanduka puan?n yok!");
			return;
		}
	}
	else if (type == 3)
	{
		if(FYellow >= 1 && FGreen >= 1)
		{
			ch->AutoGiveItem(33028, 1);
			ch->SetQuestFlag("Quest_ZodiacTemple.YellowReward",FYellow-1);
			ch->SetQuestFlag("Quest_ZodiacTemple.GreenReward",FGreen-1);
		}
	}
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenUI12zi %d %d %d %d", ch->GetQuestFlag("Quest_ZodiacTemple.YellowMark"), ch->GetQuestFlag("Quest_ZodiacTemple.GreenMark"), ch->GetQuestFlag("Quest_ZodiacTemple.YellowReward"), ch->GetQuestFlag("Quest_ZodiacTemple.GreenReward"));
}

ACMD(do_cz_check_box)
{
	if (!ch)
		return;


	if (ch->GetProtectTime("Zodiac12ZiTable") > get_global_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Biraz beklemelisin.");
		return;
	}
	ch->SetProtectTime("Zodiac12ZiTable",get_global_time()+1);

	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Eksik islem uygulandi!");
		return;
	}

	BYTE type = 0, value = 0, zero = 0;
	str_to_number(type, arg1);
	str_to_number(value, arg2);

	DWORD column_item_list_yellow[] = { 33001, 33003, 33005, 33007, 33009, 33011 };
	DWORD column_item_list_green[] = { 33002, 33004, 33006, 33008, 33010, 33012 };

	DWORD row_item_list_yellow[] = { 33013, 33015, 33017, 33019, 33021 };
	DWORD row_item_list_green[] = { 33014, 33016, 33018, 33020, 33022 };

	{
		if (type == 0)
		{
			if (value == 0 || value == 6 || value == 12 || value == 18 || value == 24)
			{
				if (ch->CountSpecifyItem(column_item_list_yellow[0]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_yellow[0], 50);
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value == 1 || value == 7 || value == 13 || value == 19 || value == 25)
			{
				if (ch->CountSpecifyItem(column_item_list_yellow[1]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_yellow[1], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value == 2 || value == 8 || value == 14 || value == 20 || value == 26)
			{
				if (ch->CountSpecifyItem(column_item_list_yellow[2]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_yellow[2], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value == 3 || value == 9 || value == 15 || value == 21 || value == 27)
			{
				if (ch->CountSpecifyItem(column_item_list_yellow[3]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_yellow[3], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value == 4 || value == 10 || value == 16 || value == 22 || value == 28)
			{
				if (ch->CountSpecifyItem(column_item_list_yellow[4]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_yellow[4], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value == 5 || value == 11 || value == 17 || value == 23 || value == 29)
			{
				if (ch->CountSpecifyItem(column_item_list_yellow[5]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_yellow[5], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}

			/////////////////////////////////////////////////////////////////////////////////
			if (value >= zero && value <= 5)
			{
				if (ch->CountSpecifyItem(row_item_list_yellow[0]) >= 50)
					ch->RemoveSpecifyItem(row_item_list_yellow[0], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value >= 6 && value <= 11)
			{
				if (ch->CountSpecifyItem(row_item_list_yellow[1]) >= 50)
					ch->RemoveSpecifyItem(row_item_list_yellow[1], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value >= 12 && value <= 17)
			{
				if (ch->CountSpecifyItem(row_item_list_yellow[2]) >= 50)
					ch->RemoveSpecifyItem(row_item_list_yellow[2], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value >= 18 && value <= 23)
			{
				if (ch->CountSpecifyItem(row_item_list_yellow[3]) >= 50)
					ch->RemoveSpecifyItem(row_item_list_yellow[3], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value >= 24 && value <= 29)
			{
				if (ch->CountSpecifyItem(row_item_list_yellow[4]) >= 50)
					ch->RemoveSpecifyItem(row_item_list_yellow[4], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
		}
		else///////////////////////////////////////////////////////////////////////////////////
		{
			if (value == 0 || value == 6 || value == 12 || value == 18 || value == 24)
			{
				if (ch->CountSpecifyItem(column_item_list_green[0]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_green[0], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value == 1 || value == 7 || value == 13 || value == 19 || value == 25)
			{
				if (ch->CountSpecifyItem(column_item_list_green[1]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_green[1], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value == 2 || value == 8 || value == 14 || value == 20 || value == 26)
			{
				if (ch->CountSpecifyItem(column_item_list_green[2]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_green[2], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value == 3 || value == 9 || value == 15 || value == 21 || value == 27)
			{
				if (ch->CountSpecifyItem(column_item_list_green[3]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_green[3], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value == 4 || value == 10 || value == 16 || value == 22 || value == 28)
			{
				if (ch->CountSpecifyItem(column_item_list_green[4]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_green[4], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value == 5 || value == 11 || value == 17 || value == 23 || value == 29)
			{
				if (ch->CountSpecifyItem(column_item_list_green[5]) >= 50)
					ch->RemoveSpecifyItem(column_item_list_green[5], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}

			/////////////////////////////////////////////////////////////////////////////////
			if (value >= zero && value <= 5)
			{
				if (ch->CountSpecifyItem(row_item_list_green[0]) >= 50)
					ch->RemoveSpecifyItem(row_item_list_green[0], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value >= 6 && value <= 11)
			{
				if (ch->CountSpecifyItem(row_item_list_green[1]) >= 50)
					ch->RemoveSpecifyItem(row_item_list_green[1], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value >= 12 && value <= 17)
			{
				if (ch->CountSpecifyItem(row_item_list_green[2]) >= 50)
					ch->RemoveSpecifyItem(row_item_list_green[2], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value >= 18 && value <= 23)
			{
				if (ch->CountSpecifyItem(row_item_list_green[3]) >= 50)
					ch->RemoveSpecifyItem(row_item_list_green[3], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
			else if (value >= 24 && value <= 29)
			{
				if (ch->CountSpecifyItem(row_item_list_green[4]) >= 50)
					ch->RemoveSpecifyItem(row_item_list_green[4], 50);
				else
				{
					 ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterince zodyak tilsimin yok!"));
					return;
				}
			}
		}
	}

	int size = 1;
	for (BYTE b = 0; b < value; ++b)
		size *= 2;

	if (type == 0)
		ch->SetQuestFlag("Quest_ZodiacTemple.YellowMark",ch->GetQuestFlag("Quest_ZodiacTemple.YellowMark")+size);
	else
		ch->SetQuestFlag("Quest_ZodiacTemple.GreenMark",ch->GetQuestFlag("Quest_ZodiacTemple.GreenMark")+size);

	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenUI12zi %d %d %d %d", ch->GetQuestFlag("Quest_ZodiacTemple.YellowMark"), ch->GetQuestFlag("Quest_ZodiacTemple.GreenMark"), ch->GetQuestFlag("Quest_ZodiacTemple.YellowReward"), ch->GetQuestFlag("Quest_ZodiacTemple.GreenReward"));
}
#endif

#ifdef ENABLE_TRACK_WINDOW
#include "new_mob_timer.h"
ACMD(do_track_window)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);

	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "load")
	{
		if (ch->GetProtectTime("track_dungeon")==1)
			return;

		ch->GetDungeonCooldown(WORD_MAX);

		for (BYTE i = 2; i < vecArgs.size(); ++i)
		{
			WORD globalBossID;
			if (!str_to_number(globalBossID, vecArgs[i].c_str()))
				continue;
			CNewMobTimer::Instance().GetTrackData(ch, globalBossID);
		}

		ch->SetProtectTime("track_dungeon", 1);
	}
	else if (vecArgs[1] == "reenter")
	{
		if (!ch->IsGM())
			return;

		if (vecArgs.size() < 4) { return; }

		WORD testVnum;
		if (!str_to_number(testVnum, vecArgs[2].c_str()))
			return;

		int testTime;
		if (!str_to_number(testTime, vecArgs[3].c_str()))
			return;

		ch->GetDungeonCooldownTest(testVnum, testTime, false);
	}
	else if (vecArgs[1] == "cooldown")
	{
		if (!ch->IsGM())
			return;

		if (vecArgs.size() < 4) { return; }

		WORD testVnum;
		if (!str_to_number(testVnum, vecArgs[2].c_str()))
			return;

		int testTime;
		if (!str_to_number(testTime, vecArgs[3].c_str()))
			return;

		ch->GetDungeonCooldownTest(testVnum, testTime, true);
	}
	else if (vecArgs[1] == "teleport")
	{
		if (vecArgs.size() < 3) { return; }

		WORD mobIndex;
		if (!str_to_number(mobIndex, vecArgs[2].c_str()))
			return;

		//PORTAL WARP I PUT ONLY FOR FLAME B
		const std::map<WORD, std::pair<std::pair<long, long>,std::pair<WORD, std::pair<BYTE,BYTE>>>> m_TeleportData = {
			//{mobindex - {{X, Y}, {PORT, {MINLVL,MAXLVL},}}},
			{9844, { {3840, 14323}, {0, {40, 60}} }},
			{9836, { {3327, 14848}, {0, {65, 85}} }},
			{9838, { {3840, 14861}, {0, {80, 100}} }},
			{9840, { {4350, 14850}, {0, {95, 115}} }},
			{9842, { {3327, 15384}, {0, {105, 120}} }},
			// {4140, { {9341, 4134}, {0, {105, 120}} }},
			{1093, { {5905, 1105}, {0, {40, 120}} }},
			{2092, { {689, 6111}, {0, {70, 120}} }},
			{2493, { {1800, 12199}, {0, {75, 120}} }},
			{2598, { {5918, 993}, {0, {80, 120}} }},
			{6091, { {5984, 7073}, {0, {90, 120}} }},
			{6191, { {4319, 1647}, {0, {90, 120}} }},
			{6192, { {8277, 14187}, {0, {105, 120}} }},
			{9018, { {11082, 17824}, {0, {105, 120}} }},
			{20442, { {7358, 6237}, {0, {110, 120}} }},
		};

		const auto it = m_TeleportData.find(mobIndex);
		if (it != m_TeleportData.end())
		{
			if (ch->GetLevel() < it->second.second.second.first || ch->GetLevel() > it->second.second.second.second)
			{
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 735, "");
				return;
			}

			ch->WarpSet(it->second.first.first * 100, it->second.first.second * 100, it->second.second.first);
		}
	}
}
#endif

#ifdef __DUNGEON_INFO__
ACMD(do_dungeon_info)
{
	std::vector<std::string> vecArgs;
	split_argument(argument, vecArgs);
	if (vecArgs.size() < 2) { return; }
	else if (vecArgs[1] == "rank")
	{
		if (vecArgs.size() < 4) { return; }
		DWORD mobIdx, rankIdx;
		if (!str_to_number(mobIdx, vecArgs[2].c_str()) || !str_to_number(rankIdx, vecArgs[3].c_str()))
			return;
		CHARACTER_MANAGER::Instance().SendDungeonRank(ch, mobIdx, rankIdx);
	}
	else if (vecArgs[1] == "test_cooldown" && ch->IsGM())
	{
		ch->SetQuestFlag("jotun.cooldown", time(0) + 90);
		ch->SetQuestFlag("deviltower.cooldown", time(0) + 30);
		ch->SetQuestFlag("hydra.cooldown", time(0) + 90);
		ch->SetQuestFlag("MeleyDungeon.cooldown", time(0) + 90);
		ch->SetQuestFlag("spider.cooldown", time(0) + 60);
		ch->SetQuestFlag("devil.cooldown", time(0) + 30);
		ch->SetQuestFlag("nemere.cooldown", time(0) + 60);
		ch->SetQuestFlag("azrael.cooldown", time(0) + 60);
		ch->SetQuestFlag("dragonlair.cooldown", time(0) + 60);
		ch->SetQuestFlag("nightmare.cooldown", time(0) + 60);
		ch->SetQuestFlag("razador.cooldown", time(0) + 60);
		ch->SetQuestFlag("duratus.cooldown", time(0) + 90);
		ch->SetQuestFlag("alastoreasy.cooldown", time(0) + 90);
		ch->SetQuestFlag("alastorhard.cooldown", time(0) + 90);
		ch->SetQuestFlag("serpent.cooldown", time(0) + 90);
		ch->SetQuestFlag("zodiactemple.cooldown", time(0) + 90);
		ch->SetQuestFlag("rxdragonlair.cooldown", time(0) + 240);

		ch->SendDungeonCooldown(0);
	}
	else if (vecArgs[1] == "update" && ch->IsGM())
		ch->SendDungeonCooldown(0);
}
#endif

#ifdef ENABLE_AFFECT_BUFF_REMOVE
ACMD(do_remove_buff)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (!ch)
		return;

	int affect = 0;
	str_to_number(affect, arg1);

	// SECURITY: client sadece skill buff index'i gonderir (uiaffectshower, 66 haric).
	// AFFECT_* (>=200: chat ban, debuff, PvP cezalari) silinemez.
	if (affect <= 0 || affect >= AFFECT_MOV_SPEED || affect == 66 || !CSkillManager::instance().Get(affect))
		return;

	CAffect* pAffect = ch->FindAffect(affect);

	if (pAffect)
		ch->RemoveAffect(affect);
}
#endif

#ifdef ENABLE_WORD_GAME_EVENT
ACMD(do_word_game)
{
	if (!ch)
		return;

	if (!ch->IsPC())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHack())
		return;

	if(ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		|| ch->GetShopOwner()
#endif
		)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("kelime_event_kpt"));
		return;
	}

	if(ch->CountSpecifyItem(30216) == 0 || ch->CountSpecifyItem(30213) == 0 || ch->CountSpecifyItem(30219) == 0 || ch->CountSpecifyItem(30214) == 0 || ch->CountSpecifyItem(30217) == 0 || ch->CountSpecifyItem(30210) == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("kelime_event_yk"));
		return;
	}
	ch->RemoveSpecifyItem(30216, 1);
	ch->RemoveSpecifyItem(30213, 1);
	ch->RemoveSpecifyItem(30219, 1);
	ch->RemoveSpecifyItem(30214, 1);
	ch->RemoveSpecifyItem(30217, 1);
	ch->RemoveSpecifyItem(30210, 1);
	ch->AutoGiveItem(50128, 1);
}
#endif

#ifdef ENABLE_SOCCER_BALL_EVENT
ACMD(do_top_ver)
{
	if (!ch)
		return;

	if (!ch->IsPC())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHack())
		return;

	if(ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		|| ch->GetShopOwner()
#endif
		)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("kelime_event_kpt"));
		return;
	}

	if(ch->CountSpecifyItem(50096) < 20)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("kelime_event_yk"));
		return;
	}
	ch->RemoveSpecifyItem(50096, 20);
	ch->AutoGiveItem(50265, 1);
}
#endif

#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
int iConfigReqcmd[18][3] = 
{
	{	0,	0,	0},
	{	8701,	1, 10000},
	{	8701,	1, 10000},
	{	8701,	1, 10000},
	{	8701,	2, 50000},
	{	8701,	2, 50000},
	{	8701,	2, 50000},
	{	8701,	2, 50000},
	{	8701,	2, 50000},
	{	8701,	3, 100000},
	{	8701,	3, 100000},
	{	8701,	3, 100000},
	{	8701,	3, 100000},
	{	8701,	4, 200000},
	{	8701,	4, 200000},
	{	8701,	4, 200000},
	{	8701,	4, 200000},
	{	8701,	4, 200000}
};

int iConfigRewardHalouncmd[3][2] = 
{
	{	8706,	1},
	{	8706,	1},
	{	8706,	1}
};

ACMD(take_reward_halloween)
{
	if (ch->GetHalounLv() == 0){	return;}
	if (ch->GetRewardHaloun() == 0){	return;}

	if (ch->IsObserverMode() || ch->IsDead() || ch->GetDungeon() || ch->GetExchange() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->GetShop() || ch->GetMyShop()
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		|| ch->GetShopOwner()
#endif
		)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("yapamazsin_hallowen"));
		return;
	}

	if (ch->GetHalounLv() == 4){	ch->AutoGiveItem(iConfigRewardHalouncmd[0][0], iConfigRewardHalouncmd[0][1]);}
	if (ch->GetHalounLv() == 9){	ch->AutoGiveItem(iConfigRewardHalouncmd[1][0], iConfigRewardHalouncmd[1][1]);}
	if (ch->GetHalounLv() == 13){	ch->AutoGiveItem(iConfigRewardHalouncmd[2][0], iConfigRewardHalouncmd[2][1]);}

	ch->PointChange(POINT_RHALOUN, -1);
}

ACMD(do_increase_halloween)
{
	if (ch->IsObserverMode() || ch->IsDead() || ch->GetDungeon() || ch->GetExchange() || ch->IsOpenSafebox() || ch->IsCubeOpen() || ch->GetShop() || ch->GetMyShop()
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		|| ch->GetShopOwner()
#endif
		)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("yapamazsin_hallowen"));
		return;
	}
	
	if (ch->GetHalounPoints() == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("etkinlik_kapali"));
		return;
	}

	if (ch->GetHalounLv() > 17){	return;}
	
	if (ch->GetRewardHaloun() > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("sonraki_icin_sandik_al"));
		return;
	}

	ch->PointChange(POINT_HALOUNLV, 1);

	if (ch->CountSpecifyItem(iConfigReqcmd[ch->GetHalounLv()][0]) >= iConfigReqcmd[ch->GetHalounLv()][1])
	{
		ch->RemoveSpecifyItem(iConfigReqcmd[ch->GetHalounLv()][0], iConfigReqcmd[ch->GetHalounLv()][1]);

		int iRandom = number(1, 15);
		ch->PointChange(POINT_HALOUN, iRandom);
	}	
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("yang_item_yok_hallowen"));

		ch->PointChange(POINT_HALOUNLV, -1);
		return;
	}

	ch->IncreaseNivel();
}
#endif

#ifdef ENABLE_SPIRIT_STONE_READING
LPEVENT ruhtimer = NULL;

EVENTINFO(TMainEventInfo)
{
	LPCHARACTER	kim;
	long skillindexx;
	
	TMainEventInfo() 
	: kim( NULL )
	, skillindexx( 0 )
	{
	}

} ;

EVENTFUNC(ruh_event)
{
	TMainEventInfo * info = dynamic_cast<TMainEventInfo *>(  event->info );

	if ( info == NULL )
	{
		sys_err( "ruh_event> <Factor> Null pointer" );
		return 0;
	}
	
	LPCHARACTER	ch = info->kim;
	long skillindex = info->skillindexx;
	
	if (NULL == ch || skillindex == 0)
		return 0;

	if (!ch)
		return 0;

	if (!ch->GetDesc())
		return 0;
 
	if (!ch->IsPC())
		return 0;
 
	if (ch->IsDead() || ch->IsStun())
		return 0;

	if (ch->IsHack())
		return 0;

	if(ch->CountSpecifyItem(50513) < 1 )
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhtasiyok"));
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 784, "");
		return 0;
	}

	int skilllevel = ch->GetSkillLevel(skillindex);

	if (skilllevel >= 40)
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhskillson"));
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 785, "");
		return 0;
	}

	int gerekenderece = (1000+500*(skilllevel-30));
	int derecem = (ch->GetRealAlignment()/10);
	int sonuc = (-19000+gerekenderece);
	if (derecem < 0)
	{
		gerekenderece = gerekenderece*2;
		sonuc = (-19000-gerekenderece);
	}

	if (derecem > sonuc)
	{
		int gerekliknk = gerekenderece;
		int kactane = gerekliknk/500;
		if (kactane < 0)
		{
			kactane = 0 - kactane;
		}

		if (derecem < 0)
		{
			if (ch->CountSpecifyItem(70102) < kactane)
			{
				//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhzenbitti %d"),kactane);
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 780, "%d", kactane);
				return 0;
			}

			int delta = MIN(-(ch->GetAlignment()), 500);
			ch->UpdateAlignment(delta*kactane);
			ch->RemoveSpecifyItem(70102,kactane);
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhzenbastim"));
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 781, "");
		}
	}

	// Performans optimizasyonu: GetQuestFlag yerine direkt member variable
	if(ch->GetRuhSureFlag() > get_global_time())
	{
		if (ch->CountSpecifyItem(71001) < 1 )
		{
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhsuredolmadi"));
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 782, "");
			return 0;
		}
		else
		{
			ch->RemoveSpecifyItem(71001,1);
		}
			
	}

	if (ch->CountSpecifyItem(71094) >= 1)
	{
		ch->AddAffect(512, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
		ch->RemoveSpecifyItem(71094,1);
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhmunzevikullandim"));
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 783, "");
	}

	if (gerekenderece < 0)
	{
		ch->UpdateAlignment(gerekenderece*10);
	}
	else
	{
		ch->UpdateAlignment(-gerekenderece*10);
	}

		ch->LearnGrandMasterSkill(skillindex);
	ch->RemoveSpecifyItem(50513,1);
	DWORD new_ruh_sure = get_global_time()+60*60*24;
	ch->SetRuhSureFlag(new_ruh_sure);  // Performans optimizasyonu: GetQuestFlag yerine direkt member variable

	return 1;
}

ACMD(do_ruhoku)
{
	int gelen;
	long skillindex;
	char arg1[256], arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	str_to_number(gelen, arg1);
	str_to_number(skillindex, arg2);

	if (!ch)
		return;

	if (!ch->IsPC())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;
 
	if (ch->IsHack())
		return;

	if(ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		|| ch->GetShopOwner()
#endif
		)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?u anda özel depoyu kullanamazsyn. "));
		return;
	}

	if(ch->CountSpecifyItem(50513) < 1 )
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhtasiyok"));
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 784, "");
		return;
	}

	LPITEM slot1 = ch->GetWear(WEAR_UNIQUE1);
	LPITEM slot2 = ch->GetWear(WEAR_UNIQUE2);

	if (NULL != slot1)
	{
		if (slot1->GetVnum() == 70048)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("pelerin_cikar"));
			return;
		}
	}
	if (NULL != slot2)
	{
		if (slot2->GetVnum() == 70048)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("pelerin_cikar"));
			return;
		}
	}

	int skillgrup = ch->GetSkillGroup();
	if (skillgrup == 0)
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhokuyamazsin"));
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 787, "");
		return;
	}

	if (gelen == 1) ///tek
	{
		int skilllevel = ch->GetSkillLevel(skillindex);
		int gerekenderece = (1000+500*(skilllevel-30));
		int derecem = (ch->GetRealAlignment()/10);
		int sonuc = (-19000+gerekenderece);

		// Performans optimizasyonu: GetQuestFlag yerine direkt member variable
		if(ch->GetRuhYeniSureFlag() > get_global_time())
		{
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruh1sn"));
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 786, "");
			return;
		}
		if (derecem < 0)
		{
			gerekenderece = gerekenderece*2;
			sonuc = (-19000-gerekenderece);
		}

		if (derecem > sonuc)
		{
			int gerekliknk = gerekenderece;
			int kactane = gerekliknk/500;
			if (kactane < 0)
			{
				kactane = 0 - kactane;
			}

			if (derecem < 0)
			{
				if (ch->CountSpecifyItem(70102) < kactane)
				{
					//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhzenbitti %d"),kactane);
					ch->LocaleChatPacket(CHAT_TYPE_INFO, 780, "%d", kactane);
					return;
				}

				int delta = MIN(-(ch->GetAlignment()), 500);
				ch->UpdateAlignment(delta*kactane);
				ch->RemoveSpecifyItem(70102,kactane);
				//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhzenbastim"));
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 781, "");
			}
		}

		// Performans optimizasyonu: GetQuestFlag yerine direkt member variable
		if(ch->GetRuhSureFlag() > get_global_time())
		{
			if (ch->CountSpecifyItem(71001) < 1 )
			{
				//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhsuredolmadi"));
				ch->LocaleChatPacket(CHAT_TYPE_INFO, 782, "");
				return;
			}
			else
			{
				ch->RemoveSpecifyItem(71001,1);
			}
		}

		if (ch->CountSpecifyItem(71094) >= 1)
		{
			ch->AddAffect(512, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
			ch->RemoveSpecifyItem(71094,1);
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhmunzevikullandim"));
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 783, "");
		}

		if (gerekenderece < 0)
		{
			ch->UpdateAlignment(gerekenderece*10);
		}
		else
		{
			ch->UpdateAlignment(-gerekenderece*10);
		}

		ch->LearnGrandMasterSkill(skillindex);
		ch->RemoveSpecifyItem(50513,1);
		DWORD new_ruh_sure = get_global_time()+60*60*24;
		DWORD new_ruh_yenisure = get_global_time()+1;
		ch->SetRuhSureFlag(new_ruh_sure);  // Performans optimizasyonu: GetQuestFlag yerine direkt member variable
		ch->SetRuhYeniSureFlag(new_ruh_yenisure);  // Performans optimizasyonu: GetQuestFlag yerine direkt member variable
		

	}
	else if(gelen == 0) ///hepsi
	{
		if (ruhtimer)
		{
			event_cancel(&ruhtimer);
		}

		TMainEventInfo* info = AllocEventInfo<TMainEventInfo>();

		info->kim = ch;
		info->skillindexx = skillindex;
		ruhtimer = event_create(ruh_event, info, PASSES_PER_SEC(1));
	}

	return;
}
#endif

#ifdef ENABLE_SKILL_BOOK_READING
LPEVENT bktimer = NULL;

EVENTINFO(TMainEventInfo2)
{
	LPCHARACTER	kim;
	long skillindexx;
	
	TMainEventInfo2() 
	: kim( NULL )
	, skillindexx( 0 )
	{
	}

} ;

EVENTFUNC(bk_event)
{
	TMainEventInfo2 * info = dynamic_cast<TMainEventInfo2 *>(  event->info );

	if ( info == NULL )
	{
		sys_err( "ruh_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->kim;
	long skillindex = info->skillindexx;

	if (NULL == ch || skillindex == 0)
		return 0;

	if (!ch)
		return 0;

	if (ch)
	{
		if(ch->CountSpecifyItem(50300) < 1 )
		{
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkyok"));
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 778, "");
			return 0;
		}

		int skilllevel = ch->GetSkillLevel(skillindex);
		if (skilllevel >= 30)
		{
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkskillson"));
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 774, "");
			return 0;
		}

		int dwVnum = ch->BKBul(skillindex);
		if (dwVnum == 999)
		{
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkskillyok"));
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 776, "");
			return 0;
		}

		LPITEM item = ch->GetInventoryItem(dwVnum);

		if (item->GetVnum() != 50300 || item->GetSocket(0) != skillindex)
		{
			return 0;
		}

		if (ch->CountSpecifyItem(71001) > 0)
		{
			if (!ch->IsAffectFlag(513))
			{
				ch->AddAffect(513, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
				ch->RemoveSpecifyItem(71001,1);
			}
		}

		if (ch->CountSpecifyItem(71001) < 1)
		{
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkkoturuhyok"));
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 779, "");
			return 0;
		}

		if (ch->CountSpecifyItem(71094) >= 1)
		{
			if (!ch->IsAffectFlag(512))
			{
				ch->AddAffect(512, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
				ch->RemoveSpecifyItem(71094,1);
			}
		}

		if (item->GetVnum() == 50300)
		{
			if (true == ch->LearnSkillByBook(skillindex))
			{
				if (item->GetVnum() == 50300 && item->GetSocket(0) == skillindex)
				{
					item->SetCount(item->GetCount() - 1);
					int iReadDelay;
					iReadDelay= number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					ch->SetSkillNextReadTime(skillindex, get_global_time() + iReadDelay);
					DWORD new_bk_yenisure = get_global_time() + 1;
					ch->SetBkYeniSureFlag(new_bk_yenisure);  // Performans optimizasyonu: GetQuestFlag yerine direkt member variable
				}
			}
			else
			{
				if (item->GetVnum() == 50300 && item->GetSocket(0) == skillindex)
				{
					item->SetCount(item->GetCount() - 1);
					int iReadDelay;
					iReadDelay= number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					ch->SetSkillNextReadTime(skillindex, get_global_time() + iReadDelay);
					DWORD new_bk_yenisure = get_global_time() + 1;
					ch->SetBkYeniSureFlag(new_bk_yenisure);  // Performans optimizasyonu: GetQuestFlag yerine direkt member variable
				}
			}
		}
		return 1;
	}
	return 0;
}

ACMD(do_bkoku)
{
	int gelen;
	long skillindex;
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	str_to_number(gelen, arg1);
	str_to_number(skillindex, arg2);

	if (!ch)
		return;

	if (!ch->IsPC())
		return;

	if (ch->IsDead() || ch->IsStun())
		return;

	if (ch->IsHack())
		return;

	if(ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		|| ch->GetShopOwner()
#endif
		)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?u anda özel depoyu kullanamazsyn. "));
		return;
	}

	int skillgrup = ch->GetSkillGroup();
	if (skillgrup == 0)
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkokuyamazsin"));
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 775, "");
		return;
	}

	if(ch->CountSpecifyItem(50300) < 1)
	{
		//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkyok"));
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 778, "");
		return;
	}

	if (gelen == 1) ///tek
	{
		int skilllevel = ch->GetSkillLevel(skillindex);

		if (skilllevel >= 30)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ruhskillson"));
			return;
		}

		// Performans optimizasyonu: GetQuestFlag yerine direkt member variable
		if (ch->GetBkYeniSureFlag() > get_global_time())
		{
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bk1sn"));
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 777, "");
			return;
		}

		int dwVnum = ch->BKBul(skillindex);
		if (dwVnum == 999)
		{
			//ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bkskillyok"));
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 776, "");
			return;
		}

		LPITEM item = ch->GetInventoryItem(dwVnum);

		if (item->GetVnum() != 50300 || item->GetSocket(0) != skillindex)
		{
			return;
		}

		if (ch->CountSpecifyItem(71001) > 0)
		{
			if (!ch->IsAffectFlag(513))
			{
				ch->AddAffect(513, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
				ch->RemoveSpecifyItem(71001,1);
			}
		}

		if (ch->CountSpecifyItem(71094) >= 1)
		{
			if (!ch->IsAffectFlag(512))
			{
				ch->AddAffect(512, aApplyInfo[0].bPointType, 0, 0, 536870911, 0, false);
				ch->RemoveSpecifyItem(71094,1);
			}
		}

		if (item->GetVnum() == 50300)
		{
			if (ch->FindAffect(AFFECT_SKILL_NO_BOOK_DELAY) == false && ch->CountSpecifyItem(71001) < 1)
			{
				ch->SkillLearnWaitMoreTimeMessage(ch->GetSkillNextReadTime(skillindex) - get_global_time());
				return;
			}

			if (true == ch->LearnSkillByBook(skillindex))
			{
				if (item->GetVnum() == 50300 && item->GetSocket(0) == skillindex)
				{
					item->SetCount(item->GetCount() - 1);
					int iReadDelay;
					iReadDelay= number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					ch->SetSkillNextReadTime(skillindex, get_global_time() + iReadDelay);
				}
			}
			else
			{
				if (item->GetVnum() == 50300 && item->GetSocket(0) == skillindex)
				{
					item->SetCount(item->GetCount() - 1);
					int iReadDelay;
					iReadDelay= number(SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
					ch->SetSkillNextReadTime(skillindex, get_global_time() + iReadDelay);
				}
			}
			DWORD new_bk_yenisure = get_global_time() + 1;
			ch->SetBkYeniSureFlag(new_bk_yenisure);  // Performans optimizasyonu: GetQuestFlag yerine direkt member variable
			ch->SetBkYeniSureFlag(new_bk_yenisure);  // Performans optimizasyonu: GetQuestFlag yerine direkt member variable
		}
	}
	else if(gelen == 0) ///hepsi
	{
		if (bktimer)
		{
			event_cancel(&bktimer);
		}

		TMainEventInfo2* info = AllocEventInfo<TMainEventInfo2>();
		info->kim = ch;
		info->skillindexx = skillindex;
		bktimer = event_create(bk_event, info, PASSES_PER_SEC(1));
	}

	return;
}
#endif

#ifdef ENABLE_AUTO_SELL_SYSTEM
ACMD(do_autosell_info)
{
	ch->SendItemProcessInfo();
}

ACMD(do_autosell_status)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	int status = 0;
	str_to_number(status, arg1);
	
	ch->SetAutoSellStatus(status == 1);
	ch->SendItemProcessInfo();
}

ACMD(do_autosell_add)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD item_vnum = 0;
	str_to_number(item_vnum, arg1);
	
	ch->AddAutoSellItem(item_vnum);
	ch->SendItemProcessInfo();
}

ACMD(do_autosell_remove)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD item_vnum = 0;
	str_to_number(item_vnum, arg1);
	
	ch->RemoveAutoSellItem(item_vnum);
	ch->SendItemProcessInfo();
}

ACMD(do_autosell_remove_all)
{
	ch->RemoveAllAutoSellItem();
	ch->SendItemProcessInfo();
	// ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("oto_sat_tum_liste_temizlendi"));
}

ACMD(do_auto_sell)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Kullan?m: /autosell <0|1>");
		return;
	}

	if (!strcmp(arg1, "0"))
		ch->SetAutoSellStatus(true);
	else if (!strcmp(arg1, "1"))
		ch->SetAutoSellStatus(false);
	else
		ch->ChatPacket(CHAT_TYPE_INFO, "Kullan?m: /autosell <0|1>");
}
#endif

#ifdef ENABLE_PLAYERS_ONLINE
ACMD(do_players_online){
	if (!ch)
		return;
	
	int iTotal;
	int * paiEmpireUserCount;
	int iLocal;

	DESC_MANAGER::instance().GetUserCount(iTotal, &paiEmpireUserCount, iLocal);
	
	ch->ChatPacket(CHAT_TYPE_COMMAND, "SetPlayersOnline %d", iTotal);
}
#endif

#ifdef ENABLE_NPC_LOCATION_TRACE
ACMD(do_find_npc)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "usage: find_npc <vnum>");
		return;
	}

	DWORD npcVnum = atoi(arg1);
	std::vector<std::pair<long, long>> npcPositions;

	if (SECTREE_MANAGER::instance().GetNpcLocationByVnum(ch->GetMapIndex(), npcVnum, npcPositions))
	{
		int positionIndex = 1;

		ch->ChatPacket(CHAT_TYPE_INFO, "Haritada NPC bulundu (%d)", npcVnum);
		for (const auto& position : npcPositions)
		{
			//DEBUG INFO
			ch->ChatPacket(CHAT_TYPE_INFO, "[%d] - Konum: %d, %d", positionIndex, position.first, position.second);
			positionIndex++;
		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu haritada NPC(%d) bulunamad?.", npcVnum);
	}
}

ACMD(do_find_near_npc)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "usage: find_near_npc <vnum>");
		return;
	}

	DWORD npcVnum = atoi(arg1);
	std::vector<std::pair<long, long>> npcPositions;

	if (SECTREE_MANAGER::instance().GetNpcLocationByVnum(ch->GetMapIndex(), npcVnum, npcPositions))
	{
		if (npcPositions.empty())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Bu haritada NPC(%d) bulunamad?.", npcVnum);
			return;
		}

		std::sort(npcPositions.begin(), npcPositions.end(),
			[ch](const std::pair<long, long>& a, const std::pair<long, long>& b)
			{
				return DISTANCE_APPROX(a.first - ch->GetX(), a.second - ch->GetY()) < DISTANCE_APPROX(b.first - ch->GetX(), b.second - ch->GetY());
			});

		//DEBUG INFO
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu haritada NPC(%d) bulundu.", npcVnum);
		ch->ChatPacket(CHAT_TYPE_INFO, "Bize en yak?n NPC konumu: X=%ld, Y=%ld, Distance=%d", npcPositions[0].first, npcPositions[0].second, DISTANCE_APPROX(npcPositions[0].first - ch->GetX(), npcPositions[0].second - ch->GetY()));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu haritada NPC(%d) bulunamad?.", npcVnum);
	}
}
#endif

#ifdef ENABLE_BIOLOGIST_SYSTEM
LPEVENT biyologtimer = NULL;

EVENTINFO(TMainEventInfo5)
{
	LPCHARACTER	kim;
	int deger;
	int itemim1;
	int itemim2;
	TMainEventInfo5() 
	: kim( NULL )
	, deger( 0 )
	, itemim1( 0 )
	, itemim2( 0 )
	{
	}
} ;

EVENTFUNC(biyolog_event)
{
	TMainEventInfo5 * info = dynamic_cast<TMainEventInfo5 *>(  event->info );
	if ( info == NULL )
	{
		sys_err( "biyolog_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->kim;
	int deger = info->deger;
	int itemim1 = info->itemim1;
	int itemim2 = info->itemim2;

	if (NULL == ch || deger == 0 || itemim1 == 0 || itemim2 == 0)
		return 0;

	if (!ch)
		return 0;

	if (!ch->GetDesc())
		return 0;

	int sans =  BiyologSistemi[ch->GetQuestFlag("bio.durum")][3];

	if (ch)
	{
		LPITEM item = ch->GetItem(TItemPos(INVENTORY, itemim1));
		if (item != NULL)
		{
			if (item->GetVnum() == 70022)
			{
				if(ch->GetQuestFlag("bio.durum") > 10)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Biyolog görevlerini tamamlamy?syn!"));
					return 0;
				}

				if (ch->CountSpecifyItem(70022) < 1)
				{
					return 0;
				}

				if(int(ch->GetQuestFlag("bio.sure")) == 1)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Zaten süre e?yasy kullanmy?syn!"));
				}
				else if(ch->GetQuestFlag("bio.ruhtasi") == 2)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Bunu ruh ta?y görevinde yapamazsyn."));
				}
				else
				{
					item->SetCount(item->GetCount() - 1);
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Süre i?ledi."));
					ch->SetQuestFlag("bio.sure",1);
					ch->SetQuestFlag("bio.kalan",get_global_time()+0);
					ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
				}
			}
		}

		LPITEM item2 = ch->GetItem(TItemPos(INVENTORY, itemim2));
		if (item2 != NULL)
		{
			if(ch->GetQuestFlag("bio.durum") > 10)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Biyolog görevlerini tamamlamy?syn!"));
				return 0;
			}

			int SANS_ITEMLER[3] = 
			{
				71035,
				76020,
				39023,
			};

			for (int it = 0; it <= 3; it++)
			{
				if (item2->GetVnum() == SANS_ITEMLER[it])
				{
					if (ch->CountSpecifyItem(SANS_ITEMLER[it]) < 1)
					{
						return 0;
					}

					if(int(ch->GetQuestFlag("bio.sans")) == 1)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Zaten ?ans e?yasyny kullanmy?syn."));
					}
					else if(ch->GetQuestFlag("bio.ruhtasi") == 2)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Bunu ruh ta?y görevinde yapamazsyn."));
					}
					else
					{
						item2->SetCount(item2->GetCount() - 1);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> ?ans i?ledi."));
						ch->SetQuestFlag("bio.sans", 1);
					}
				}
			}
		}

		if(ch->GetQuestFlag("bio.kalan") > get_global_time())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev süresi dolmady."));
			return 0;
		}

		if(ch->GetQuestFlag("bio.durum") == 1 || ch->GetQuestFlag("bio.durum") == 2 || ch->GetQuestFlag("bio.durum") == 3 || ch->GetQuestFlag("bio.durum") == 4 || ch->GetQuestFlag("bio.durum") == 5 || ch->GetQuestFlag("bio.durum") == 6 || ch->GetQuestFlag("bio.durum") == 7 || ch->GetQuestFlag("bio.durum") == 8 || ch->GetQuestFlag("bio.durum") == 9 || ch->GetQuestFlag("bio.durum") == 10)
		{
			if(ch->GetQuestFlag("bio.durum") > 10)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Biyolog görevlerini tamamlamy?syn!"));
				return 0;
			}

			if (ch->CountSpecifyItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][0]) < 1)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Gereken miktarda biyolog nesnesine sahip de?ilsin."));
				return 0;
			}
			else
			{
				int prob = number(1,100);
				if(ch->GetQuestFlag("bio.sans") == 1)
				{
					sans = sans +100;
				}
				if(ch->GetQuestFlag("bio.sure") == 1)
				{
					ch->SetQuestFlag("bio.sure",0);
				}

				if(sans >= prob)
				{
					if (ch->GetQuestFlag("bio.verilen") >= BiyologSistemi[ch->GetQuestFlag("bio.durum")][1])
					{
						return 0;
					}

					ch->SetQuestFlag("bio.verilen",ch->GetQuestFlag("bio.verilen")+1);

					if(ch->GetQuestFlag("bio.sans") == 1)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görevi tamamlamak için ara?tyrmacynyn özütü kullanyldy."));
						ch->SetQuestFlag("bio.sans",0);
					}

					if(ch->GetQuestFlag("bio.verilen") == BiyologSistemi[ch->GetQuestFlag("bio.durum")][1])
					{
						if (ch->GetQuestFlag("bio.durum") == 9)
						{
							ch->SetQuestFlag("bio.ruhtasi",3);
							ch->SetQuestFlag("bio.odulvakti",1);
						}
						else
						{
							TItemTable* pTable = ITEM_MANAGER::instance().GetTable(BiyologSistemi[ch->GetQuestFlag("bio.durum")][4]);
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> %s Ruh ta?yny bulmalysyn."), pTable->szLocaleName);
							ch->SetQuestFlag("bio.ruhtasi",2);
						}
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> %d adet e?ya kaldy."), (BiyologSistemi[ch->GetQuestFlag("bio.durum")][1]-ch->GetQuestFlag("bio.verilen")));
						ch->SetQuestFlag("bio.kalan",get_global_time()+(BiyologSistemi[ch->GetQuestFlag("bio.durum")][2]*60));
					}
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev ba?arysyz."));
					ch->SetQuestFlag("bio.kalan",get_global_time()+(BiyologSistemi[ch->GetQuestFlag("bio.durum")][2]*60));
				}
				ch->RemoveSpecifyItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][0],1);
			}
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
		return 1;
	}

	return 0;
}

ACMD(do_biyolog)
{
	if (quest::CQuestManager::instance().GetEventFlag("biyolog_disable") == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Sistem suan icin devre disi!");
		return;
	}

	char arg1[256], arg2[256], arg3[256];
	three_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2), arg3, sizeof(arg3));

	if (!*arg1 && !*arg2 && !*arg3)
		return;

	if (!ch->IsPC())
		return;

#ifdef ENABLE_OFFLINE_SHOP_SYSTEM
	if (ch->GetExchange() || ch->GetViewingShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
#else
	if (ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen())
#endif
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Pencere açykken bu i?lemi yapamazsyn."));
		return;
	}

	int sans =  BiyologSistemi[ch->GetQuestFlag("bio.durum")][3];
	int toplam =  BiyologSistemi[ch->GetQuestFlag("bio.durum")][1];
	int level =  ch->GetLevel();

	int affectvnum =  BiyologSistemi[ch->GetQuestFlag("bio.durum")][6];
	int affectvalue =  BiyologSistemi[ch->GetQuestFlag("bio.durum")][7];
	int affectvnum2 =  BiyologSistemi[ch->GetQuestFlag("bio.durum")][8];
	int affectvalue2 =  BiyologSistemi[ch->GetQuestFlag("bio.durum")][9];
	int affectvnum3 =  BiyologSistemi[ch->GetQuestFlag("bio.durum")][10];
	int affectvalue3 =  BiyologSistemi[ch->GetQuestFlag("bio.durum")][11];
	int affectvnum4 =  BiyologSistemi[ch->GetQuestFlag("bio.durum")][12];
	int affectvalue4 =  BiyologSistemi[ch->GetQuestFlag("bio.durum")][13];
	int unlimited = 60*60*60*365;

	if(level < 30)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> 30. seviyeden dü?ük iken görevleri tamamlayamazsyn."));
		return;
	}

	if(ch->GetQuestFlag("bio.durum") > 10)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Biyolog görevlerini tamamlamy?syn!"));
		return;
	}

	DWORD dwVnum = 0;
	DWORD dwVnum2 = 0;
	str_to_number(dwVnum, arg2);
	str_to_number(dwVnum2, arg3);

	const std::string& strArg1 = std::string(arg1);
	if(strArg1 == "request")
	{
		LPITEM item = ch->GetItem(TItemPos(INVENTORY, dwVnum));
		if (item != NULL)
		{
			if (item->GetVnum() == 70022)
			{
				if(ch->GetQuestFlag("bio.durum") > 10)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Biyolog görevlerini tamamlamy?syn!"));
					return;
				}

				if (ch->CountSpecifyItem(70022) < 1)
				{
					return;
				}

				if(int(ch->GetQuestFlag("bio.sure")) == 1)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Zaten süre e?yasy kullanmy?syn!"));
				}
				else if(ch->GetQuestFlag("bio.ruhtasi") == 2)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Bunu ruh ta?y görevinde yapamazsyn."));
				}
				else
				{
					item->SetCount(item->GetCount() - 1);
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Süre i?ledi."));
					ch->SetQuestFlag("bio.sure",1);
					ch->SetQuestFlag("bio.kalan",get_global_time()+0);
					ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
				}
			}
		}

		LPITEM item2 = ch->GetItem(TItemPos(INVENTORY, dwVnum2));
		if (item2 != NULL)
		{
			if(ch->GetQuestFlag("bio.durum") > 10)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Biyolog görevlerini tamamlamy?syn!"));
				return;
			}

			int SANS_ITEMLER[3] = 
			{
				71035,
				76020,
				39023,
			};

			for (int it = 0; it <= 3; it++)
			{
				if (item2->GetVnum() == SANS_ITEMLER[it])
				{
					if (ch->CountSpecifyItem(SANS_ITEMLER[it]) < 1)
					{
						return;
					}

					if(int(ch->GetQuestFlag("bio.sans")) == 1)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Zaten ?ans e?yasyny kullanmy?syn."));
					}
					else if(ch->GetQuestFlag("bio.ruhtasi") == 2)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Bunu ruh ta?y görevinde yapamazsyn."));
					}
					else
					{
						item2->SetCount(item2->GetCount() - 1);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> ?ans i?ledi."));
						ch->SetQuestFlag("bio.sans", 1);
					}
				}
			}
		}

		if(ch->GetQuestFlag("bio.kalan") > get_global_time())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev süresi dolmady."));
			return;
		}

		if(ch->GetQuestFlag("bio.durum") == 1 || ch->GetQuestFlag("bio.durum") == 2 || ch->GetQuestFlag("bio.durum") == 3 || ch->GetQuestFlag("bio.durum") == 4 || ch->GetQuestFlag("bio.durum") == 5 || ch->GetQuestFlag("bio.durum") == 6 || ch->GetQuestFlag("bio.durum") == 7 || ch->GetQuestFlag("bio.durum") == 8 || ch->GetQuestFlag("bio.durum") == 9 || ch->GetQuestFlag("bio.durum") == 10)
		{
			if(ch->GetQuestFlag("bio.durum") > 10)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Biyolog görevlerini tamamlamy?syn!"));
				return;
			}

			if (ch->CountSpecifyItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][0]) < 1)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Gereken miktarda biyolog nesnesine sahip de?ilsin."));
				return;
			}
			else
			{
				int prob = number(1,100);
				if(ch->GetQuestFlag("bio.sans") == 1)
				{
					sans = sans +100;
				}
				if(ch->GetQuestFlag("bio.sure") == 1)
				{
					ch->SetQuestFlag("bio.sure",0);
				}

				if(sans >= prob)
				{
					if (ch->GetQuestFlag("bio.verilen") >= BiyologSistemi[ch->GetQuestFlag("bio.durum")][1])
					{
						return;
					}

					ch->SetQuestFlag("bio.verilen",ch->GetQuestFlag("bio.verilen")+1);
					if(ch->GetQuestFlag("bio.sans") == 1)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görevi tamamlamak için ara?tyrmacynyn özütü kullanyldy."));
						ch->SetQuestFlag("bio.sans",0);
					}

					if(ch->GetQuestFlag("bio.verilen") == toplam)
					{
						if (ch->GetQuestFlag("bio.durum") == 9)
						{
							ch->SetQuestFlag("bio.ruhtasi",3);
							ch->SetQuestFlag("bio.odulvakti",1);
						}
						else
						{
							TItemTable* pTable = ITEM_MANAGER::instance().GetTable(BiyologSistemi[ch->GetQuestFlag("bio.durum")][4]);
							ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> %s Ruh ta?yny bulmalysyn."), pTable->szLocaleName);
							ch->SetQuestFlag("bio.ruhtasi",2);
						}
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("biogecti %d"), (toplam-ch->GetQuestFlag("bio.verilen")));
						ch->SetQuestFlag("bio.kalan",get_global_time()+(BiyologSistemi[ch->GetQuestFlag("bio.durum")][2]*60));
					}			
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev ba?arysyz."));
					ch->SetQuestFlag("bio.kalan",get_global_time()+(BiyologSistemi[ch->GetQuestFlag("bio.durum")][2]*60));
				}
				ch->RemoveSpecifyItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][0],1);
			}
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
		return;
	}

	if(strArg1 == "stone")
	{
		if(ch->GetQuestFlag("bio.durum") > 10)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Biyolog görevlerini tamamlamy?syn!"));
			return;
		}

		if(ch->GetQuestFlag("bio.durum") == 1 || ch->GetQuestFlag("bio.durum") == 2 || ch->GetQuestFlag("bio.durum") == 3 || ch->GetQuestFlag("bio.durum") == 4 || ch->GetQuestFlag("bio.durum") == 5 || ch->GetQuestFlag("bio.durum") == 6 || ch->GetQuestFlag("bio.durum") == 7 || ch->GetQuestFlag("bio.durum") == 8 || ch->GetQuestFlag("bio.durum") == 9 || ch->GetQuestFlag("bio.durum") == 10)
		{
			if (ch->GetQuestFlag("bio.verilen") >= BiyologSistemi[ch->GetQuestFlag("bio.durum")][1] && ch->GetQuestFlag("bio.ruhtasi") == 2)
			{
				if (ch->CountSpecifyItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][4]) < 1)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("bioruhtasiyok"));
					return;
				}
				else
				{
					ch->RemoveSpecifyItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][4],1);

					if(ch->GetQuestFlag("bio.durum") == 9 || ch->GetQuestFlag("bio.durum") == 10)
					{
						ch->SetQuestFlag("bio.ruhtasi",3);
						ch->SetQuestFlag("bio.odulvakti",1);
					}
					else
					{
						ch->SetQuestFlag("bio.ruhtasi",3);
					}
				}
			}
		}


		ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
		return;
	}

	if(strArg1 == "complate")
	{
		if(ch->GetQuestFlag("bio.durum") > 10)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Biyolog görevlerini tamamlamy?syn!"));
			return;
		}


		if(ch->GetQuestFlag("bio.durum") == 1)
		{
			if (ch->GetQuestFlag("bio.verilen") >= (int)BiyologSistemi[ch->GetQuestFlag("bio.durum")][1] && ch->GetQuestFlag("bio.ruhtasi") == 3)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
				ch->AutoGiveItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][5], 1);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Yeni görev eklendi."));

				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum].bPointType, affectvalue, 0, 60*60*24*365*60, 0, false);

				ch->SetQuestFlag("bio.durum",ch->GetQuestFlag("bio.durum")+1);
				ch->SetQuestFlag("bio.verilen",0);
				ch->SetQuestFlag("bio.kalan",get_global_time()+0);
				ch->SetQuestFlag("bio.ruhtasi",1);
				ch->SetQuestFlag("bio.30",1);
				ch->SetQuestFlag("bio.bildiri",ch->GetQuestFlag("bio.bildiri")+1);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
				return;
			}
		}

		if(ch->GetQuestFlag("bio.durum") == 2)
		{
			if (ch->GetQuestFlag("bio.verilen") >= (int)BiyologSistemi[ch->GetQuestFlag("bio.durum")][1] && ch->GetQuestFlag("bio.ruhtasi") == 3)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
				ch->AutoGiveItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][5], 1);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Yeni görev eklendi."));

				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum].bPointType, affectvalue, 0, 60*60*24*365*60, 0, false);

				ch->SetQuestFlag("bio.durum",ch->GetQuestFlag("bio.durum")+1);
				ch->SetQuestFlag("bio.verilen",0);
				ch->SetQuestFlag("bio.kalan",get_global_time()+0);
				ch->SetQuestFlag("bio.ruhtasi",1);
				ch->SetQuestFlag("bio.40",1);
				ch->SetQuestFlag("bio.bildiri",ch->GetQuestFlag("bio.bildiri")+1);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
				return;
			}
		}

		if(ch->GetQuestFlag("bio.durum") == 3)
		{
			if (ch->GetQuestFlag("bio.verilen") >= (int)BiyologSistemi[ch->GetQuestFlag("bio.durum")][1] && ch->GetQuestFlag("bio.ruhtasi") == 3)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
				ch->AutoGiveItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][5], 1);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Yeni görev eklendi."));

				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum].bPointType, affectvalue, 0, 60*60*24*365*60, 0, false);

				ch->SetQuestFlag("bio.durum",ch->GetQuestFlag("bio.durum")+1);
				ch->SetQuestFlag("bio.verilen",0);
				ch->SetQuestFlag("bio.kalan",get_global_time()+0);
				ch->SetQuestFlag("bio.ruhtasi",1);
				ch->SetQuestFlag("bio.50",1);
				ch->SetQuestFlag("bio.bildiri",ch->GetQuestFlag("bio.bildiri")+1);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
				return;
			}
		}

		if(ch->GetQuestFlag("bio.durum") == 4)
		{
			if (ch->GetQuestFlag("bio.verilen") >= (int)BiyologSistemi[ch->GetQuestFlag("bio.durum")][1] && ch->GetQuestFlag("bio.ruhtasi") == 3)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
				ch->AutoGiveItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][5], 1);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Yeni görev eklendi."));

				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum].bPointType, affectvalue, 0, 60*60*24*365*60, 0, false);

				ch->SetQuestFlag("bio.durum",ch->GetQuestFlag("bio.durum")+1);
				ch->SetQuestFlag("bio.verilen",0);
				ch->SetQuestFlag("bio.kalan",get_global_time()+0);
				ch->SetQuestFlag("bio.ruhtasi",1);
				ch->SetQuestFlag("bio.60",1);
				ch->SetQuestFlag("bio.bildiri",ch->GetQuestFlag("bio.bildiri")+1);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
				return;
			}
		}

		if(ch->GetQuestFlag("bio.durum") == 5)
		{
			if (ch->GetQuestFlag("bio.verilen") >= (int)BiyologSistemi[ch->GetQuestFlag("bio.durum")][1] && ch->GetQuestFlag("bio.ruhtasi") == 3)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
				ch->AutoGiveItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][5], 1);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Yeni görev eklendi."));

				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum].bPointType, affectvalue, 0, 60*60*24*365*60, 0, false);
				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum2].bPointType, affectvalue2, 0, 60*60*24*365*60, 0, false);

				ch->SetQuestFlag("bio.durum",ch->GetQuestFlag("bio.durum")+1);
				ch->SetQuestFlag("bio.verilen",0);
				ch->SetQuestFlag("bio.kalan",get_global_time()+0);
				ch->SetQuestFlag("bio.ruhtasi",1);
				ch->SetQuestFlag("bio.70",1);
				ch->SetQuestFlag("bio.bildiri",ch->GetQuestFlag("bio.bildiri")+1);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
				return;
			}
		}

		if(ch->GetQuestFlag("bio.durum") == 6)
		{
			if (ch->GetQuestFlag("bio.verilen") >= (int)BiyologSistemi[ch->GetQuestFlag("bio.durum")][1] && ch->GetQuestFlag("bio.ruhtasi") == 3)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
				ch->AutoGiveItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][5], 1);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Yeni görev eklendi."));

				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum].bPointType, affectvalue, 0, 60*60*24*365*60, 0, false);
				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum2].bPointType, affectvalue2, 0, 60*60*24*365*60, 0, false);

				ch->SetQuestFlag("bio.durum",ch->GetQuestFlag("bio.durum")+1);
				ch->SetQuestFlag("bio.verilen",0);
				ch->SetQuestFlag("bio.kalan",get_global_time()+0);
				ch->SetQuestFlag("bio.ruhtasi",1);
				ch->SetQuestFlag("bio.80",1);
				ch->SetQuestFlag("bio.bildiri",ch->GetQuestFlag("bio.bildiri")+1);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
				return;
			}
		}

		if(ch->GetQuestFlag("bio.durum") == 7)
		{
			if (ch->GetQuestFlag("bio.verilen") >= (int)BiyologSistemi[ch->GetQuestFlag("bio.durum")][1] && ch->GetQuestFlag("bio.ruhtasi") == 3)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
				ch->AutoGiveItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][5], 1);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Yeni görev eklendi."));

				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum].bPointType, affectvalue, 0, 60*60*24*365*60, 0, false);
				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum2].bPointType, affectvalue2, 0, 60*60*24*365*60, 0, false);
				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum3].bPointType, affectvalue3, 0, 60*60*24*365*60, 0, false);
				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum4].bPointType, affectvalue4, 0, 60*60*24*365*60, 0, false);

				ch->SetQuestFlag("bio.durum",ch->GetQuestFlag("bio.durum")+1);
				ch->SetQuestFlag("bio.verilen",0);
				ch->SetQuestFlag("bio.kalan",get_global_time()+0);
				ch->SetQuestFlag("bio.ruhtasi",1);
				ch->SetQuestFlag("bio.85",1);
				ch->SetQuestFlag("bio.bildiri",ch->GetQuestFlag("bio.bildiri")+1);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
				return;
			}
		}

		if(ch->GetQuestFlag("bio.durum") == 8)
		{
			if (ch->GetQuestFlag("bio.verilen") >= (int)BiyologSistemi[ch->GetQuestFlag("bio.durum")][1] && ch->GetQuestFlag("bio.ruhtasi") == 3)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
				ch->AutoGiveItem(BiyologSistemi[ch->GetQuestFlag("bio.durum")][5], 1);
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Yeni görev eklendi."));

				ch->AddAffect(AFFECT_BIO, aApplyInfo[affectvnum].bPointType, affectvalue, 0, 60*60*24*365*60, 0, false);

				ch->SetQuestFlag("bio.durum",ch->GetQuestFlag("bio.durum")+1);
				ch->SetQuestFlag("bio.verilen",0);
				ch->SetQuestFlag("bio.kalan",get_global_time()+0);
				ch->SetQuestFlag("bio.ruhtasi",1);
				ch->SetQuestFlag("bio.90",1);
				ch->SetQuestFlag("bio.bildiri",ch->GetQuestFlag("bio.bildiri")+1);
				ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
				return;
			}
		}

		if(ch->GetQuestFlag("bio.durum") == 9)
		{
			if (ch->GetQuestFlag("bio.odulvakti") == 0)
			{
				return;
			}

			if (ch->GetQuestFlag("bio.odulvakti") == 1 and level >= 92)
            {
				if (ch->GetQuestFlag("bio.verilen") >= BiyologSistemi[ch->GetQuestFlag("bio.durum")][1] && ch->GetQuestFlag("bio.ruhtasi") == 3)
				{
					if(dwVnum == 1)
					{
						ch->AddAffect(AFFECT_BIO_92, aApplyInfo[affectvnum].bPointType, affectvalue, 0, unlimited, 0, false);
						ch->SetQuestFlag("bio.durum",10);
						ch->SetQuestFlag("bio.92",1);
						ch->SetQuestFlag("bio.verilen",0);
						ch->SetQuestFlag("bio.kalan",get_global_time()+0);
						ch->SetQuestFlag("bio.ruhtasi",1);
						ch->SetQuestFlag("bio.odulvakti",0);
						ch->SetQuestFlag("bio.bildiri",ch->GetQuestFlag("bio.bildiri")+1);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Yeni görev eklendi."));
						ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
						return;
					}

					if(dwVnum == 2)
					{
						ch->AddAffect(AFFECT_BIO_92, aApplyInfo[affectvnum2].bPointType, affectvalue2, 0, unlimited, 0, false);
						ch->SetQuestFlag("bio.durum",10);
						ch->SetQuestFlag("bio.92",1);
						ch->SetQuestFlag("bio.verilen",0);
						ch->SetQuestFlag("bio.kalan",get_global_time()+0);
						ch->SetQuestFlag("bio.ruhtasi",1);
						ch->SetQuestFlag("bio.odulvakti",0);
						ch->SetQuestFlag("bio.bildiri",ch->GetQuestFlag("bio.bildiri")+1);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Yeni görev eklendi."));
						ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
						return;
					}

					if(dwVnum == 3)
					{
						ch->AddAffect(AFFECT_BIO_92, aApplyInfo[affectvnum3].bPointType, affectvalue3, 0, unlimited, 0, false);
						ch->SetQuestFlag("bio.durum",10);
						ch->SetQuestFlag("bio.92",1);
						ch->SetQuestFlag("bio.verilen",0);
						ch->SetQuestFlag("bio.kalan",get_global_time()+0);
						ch->SetQuestFlag("bio.ruhtasi",1);
						ch->SetQuestFlag("bio.odulvakti",0);
						ch->SetQuestFlag("bio.bildiri",ch->GetQuestFlag("bio.bildiri")+1);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
						ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
						return;
					}
				}
            }
		}

		if(ch->GetQuestFlag("bio.durum") == 10)
		{
			if (ch->GetQuestFlag("bio.odulvakti") == 0)
			{
				return;
			}

			if (ch->GetQuestFlag("bio.odulvakti") == 1 and level >= 94)
            {
				if (ch->GetQuestFlag("bio.verilen") >= BiyologSistemi[ch->GetQuestFlag("bio.durum")][1] && ch->GetQuestFlag("bio.ruhtasi") == 3)
				{
					if(dwVnum == 1)
					{
						ch->AddAffect(AFFECT_BIO_94, aApplyInfo[affectvnum].bPointType, affectvalue, 0, unlimited, 0, false);
						ch->SetQuestFlag("bio.durum",11);
						ch->SetQuestFlag("bio.94",1);
						ch->SetQuestFlag("bio.verilen",0);
						ch->SetQuestFlag("bio.kalan",get_global_time()+0);
						ch->SetQuestFlag("bio.ruhtasi",1);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
						ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
						return;
					}

					if(dwVnum == 2)
					{
						ch->AddAffect(AFFECT_BIO_94, aApplyInfo[affectvnum2].bPointType, affectvalue2, 0, unlimited, 0, false);
						ch->SetQuestFlag("bio.durum",11);
						ch->SetQuestFlag("bio.94",1);
						ch->SetQuestFlag("bio.verilen",0);
						ch->SetQuestFlag("bio.kalan",get_global_time()+0);
						ch->SetQuestFlag("bio.ruhtasi",1);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
						ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
						return;
					}

					if(dwVnum == 3)
					{
						ch->AddAffect(AFFECT_BIO_94, aApplyInfo[affectvnum3].bPointType, affectvalue3, 0, unlimited, 0, false);
						ch->SetQuestFlag("bio.durum",11);
						ch->SetQuestFlag("bio.94",1);
						ch->SetQuestFlag("bio.verilen",0);
						ch->SetQuestFlag("bio.kalan",get_global_time()+0);
						ch->SetQuestFlag("bio.ruhtasi",1);
						ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Biyolog Görevleri> Görev tamamlandy!"));
						ch->ChatPacket(CHAT_TYPE_COMMAND, "biyolog_update %d %d %d %d %d", ch->GetQuestFlag("bio.durum"), ch->GetQuestFlag("bio.ruhtasi"), ch->GetQuestFlag("bio.verilen"), BiyologSistemi[ch->GetQuestFlag("bio.durum")][1], ch->GetQuestFlag("bio.kalan") - get_global_time());
						return;
					}
				}
            }
		}
	}

	if(strArg1 == "all")
	{
		if (quest::CQuestManager::instance().GetEventFlag("biyolog_hizli") == 1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Sistem suan icin devre disi!");
			return;
		}

		if (biyologtimer)
		{
			event_cancel(&biyologtimer);
		}

		TMainEventInfo5* info = AllocEventInfo<TMainEventInfo5>();

		info->kim = ch;
		info->deger = toplam;
		info->itemim1 = dwVnum;
		info->itemim2 = dwVnum2;
		biyologtimer = event_create(biyolog_event, info, PASSES_PER_SEC(1));
	}

	return;
}
#endif

#ifdef __LEADERSHIP__BONUS__
#include "party.h"
ACMD(do_leadership_bonus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	BYTE bRole = 0;
	str_to_number(bRole, arg1);

	if (!ch->GetDesc() || !ch->CanWarp() || !ch->IsLoadedAffect())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu komutu kullanmadan önce 10 saniye beklemelisin.");
		return;
	}
	
	if (ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Partideyken bunu yapamazs?n.");
		return;
	}
	
	if (ch->GetLeadershipSkillLevel() == 0)
		return;
	
	if (ch->GetQuestFlag("wait_leadership") > get_global_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Bu komutu yeniden kullanabilmek için k?sa bir süre beklemen gerekiyor.");
		return;
	}
	
	ch->SetQuestFlag("wait_leadership", get_global_time() + 2);

	int iBonus = 0;
	float k = (float) ch->GetSkillPowerByLevel( MIN(SKILL_MAX_LEVEL, ch->GetLeadershipSkillLevel() ) )/ 100.0f;
	
	int iPointActived = 0;
	
	CAffect* aff = ch->FindAffect(AFF_LEADERSHIP);
	if (aff)
	{
		iPointActived = aff->bApplyOn;
	}

	ch->PointChange(POINT_PARTY_ATTACKER_BONUS, -ch->GetPoint(POINT_PARTY_ATTACKER_BONUS));
	ch->PointChange(POINT_PARTY_TANKER_BONUS, -ch->GetPoint(POINT_PARTY_TANKER_BONUS));
	ch->PointChange(POINT_PARTY_BUFFER_BONUS, -ch->GetPoint(POINT_PARTY_BUFFER_BONUS));
	ch->PointChange(POINT_PARTY_SKILL_MASTER_BONUS, -ch->GetPoint(POINT_PARTY_SKILL_MASTER_BONUS));
	ch->PointChange(POINT_PARTY_DEFENDER_BONUS, -ch->GetPoint(POINT_PARTY_DEFENDER_BONUS));
	ch->PointChange(POINT_PARTY_HASTE_BONUS, -ch->GetPoint(POINT_PARTY_HASTE_BONUS));
	ch->ComputeBattlePoints();

	ch->SetPoint(POINT_PARTY_ATTACKER_BONUS, 0);
	ch->SetPoint(POINT_PARTY_TANKER_BONUS, 0);
	ch->SetPoint(POINT_PARTY_BUFFER_BONUS, 0);
	ch->SetPoint(POINT_PARTY_SKILL_MASTER_BONUS, 0);
	ch->SetPoint(POINT_PARTY_DEFENDER_BONUS, 0);
	ch->SetPoint(POINT_PARTY_HASTE_BONUS, 0);

	ch->RemoveAffect(AFF_LEADERSHIP);
	ch->ComputePoints();
	
	switch(bRole)
	{
		case 1:
			{
				if (iPointActived == POINT_PARTY_ATTACKER_BONUS)
					break;
				
				iBonus = (int)(10 + 60 * k);
				ch->AddAffect(AFF_LEADERSHIP, POINT_PARTY_ATTACKER_BONUS, iBonus, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);
			}
			break;

		case 2:
			{
				if (iPointActived == POINT_PARTY_TANKER_BONUS)
					break;
				
				iBonus = (int)(50 + 1450 * k);

				ch->AddAffect(AFF_LEADERSHIP, POINT_PARTY_TANKER_BONUS, iBonus, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);
			}
			break;
		
		case 3:
			{
				if (iPointActived == POINT_PARTY_BUFFER_BONUS)
					break;
				
				iBonus = (int)(5 + 45 * k);

				ch->AddAffect(AFF_LEADERSHIP, POINT_PARTY_BUFFER_BONUS, iBonus, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);
			}
			break;
			
		case 4:
			{
				if (iPointActived == POINT_PARTY_SKILL_MASTER_BONUS)
					break;
				
				iBonus = (int)(25 + 600 * k);

				ch->AddAffect(AFF_LEADERSHIP, POINT_PARTY_SKILL_MASTER_BONUS, iBonus, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);
			}
			break;
			
		case 5:
			{
				if (iPointActived == POINT_PARTY_HASTE_BONUS)
					break;
				
				iBonus = (int)(1 + 5 * k);

				ch->AddAffect(AFF_LEADERSHIP, POINT_PARTY_HASTE_BONUS, iBonus, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);
			}
			break;
			
		case 6:
			{
				if (iPointActived == POINT_PARTY_DEFENDER_BONUS)
					break;
				
				iBonus = (int)(5 + 30 * k);

				ch->AddAffect(AFF_LEADERSHIP, POINT_PARTY_DEFENDER_BONUS, iBonus, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false);
			}
			break;
			
		default:
		{
			break;
		}
	}
	
	ch->ComputePoints();
	ch->PointsPacket();
}
#endif

#ifdef ENABLE_REMOTE_SHOP_SYSTEM
ACMD(do_open_range_npc)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (ch->IsDead())
		return;

	if (ch->IsDead() || ch->GetExchange() || ch->GetMyShop() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PLEASE_BEFORE_CLOSE_WINDOW_AND_USE_THIS_FUNCTION"));
		return;
	}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	if (ch->IsAcceOpened(true) || ch->IsAcceOpened(false))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PLEASE_BEFORE_CLOSE_WINDOW_AND_USE_THIS_FUNCTION"));
		return;
	}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	if (ch->IsAuraRefineWindowOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PLEASE_BEFORE_CLOSE_WINDOW_AND_USE_THIS_FUNCTION"));
		return;
	}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	if (ch->GetTransmutation() != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PLEASE_BEFORE_CLOSE_WINDOW_AND_USE_THIS_FUNCTION"));
		return;
	}
#endif

#ifdef ENABLE_ITEM_COMBINATION_SYSTEM
	if (ch->IsCombOpen() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("PLEASE_BEFORE_CLOSE_WINDOW_AND_USE_THIS_FUNCTION"));
		return;
	}
#endif

	LPSHOP shop = CShopManager::instance().Get(vnum);
	if (!shop) return;

	ch->SetShopOwner(ch);
	shop->AddGuest(ch, 0, false);
}
#endif

#ifdef __SYSTEM_SEARCH_ITEM_MOB__
ACMD(search_drop)
{
	int iWaitMadafaka = ch->GetQuestFlag("search.sijaja");
	if (iWaitMadafaka)
	{
		if (get_global_time() < iWaitMadafaka + 5) 
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Arama butonuna basmadan önce birkaç saniye bekleyin..");
			return;
		}
	}
	char arg1[4096];
	one_argument(argument, arg1, sizeof(arg1));
	
	if (!*arg1)
		return;

	std::string nume_item(arg1);
	boost::algorithm::replace_all(nume_item, "_", " ");
	
	ITEM_MANAGER::instance().FindItemMonster(ch, nume_item);
	ch->SetQuestFlag("search.sijaja", get_global_time());
}
#endif

#ifdef ENABLE_COLLECT_WINDOW
ACMD(do_choose_quest)
{
	const char *line;
	char arg1[256], arg2[256];
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!ch || !*arg1 && !*arg2)
		return;

	// Cooldown kontrolü (1 saniye)
	time_t cooldown_flag = ch->GetQuestFlag("collector.choose_quest_cooldown");
	time_t current_time = get_global_time();
	
	if (cooldown_flag > current_time)
	{
		time_t remaining = cooldown_flag - current_time;
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining time: %d seconds"), remaining);
		return;
	}

	// Cooldown'u ayarla (1 saniye)
	ch->SetQuestFlag("collector.choose_quest_cooldown", current_time + 1);

	BYTE id = 0;
	BYTE id1 = 0;
	str_to_number(id, arg1);
	str_to_number(id1, arg2);

	ch->SetQuestFlag("collector.state", id);
	// ch->ChatPacket(CHAT_TYPE_INFO, "%d %d %d", ch->GetQuestFlag("collector.state"), id, id1);

	quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), id1, 0);
}
ACMD(do_open_collect_window)
{
	// Cooldown kontrolü (0.7 saniye = 1 saniye rounded)
	time_t cooldown_flag = ch->GetQuestFlag("collector.window_cooldown");
	time_t current_time = get_global_time();
	
	if (cooldown_flag > current_time)
	{
		time_t remaining = cooldown_flag - current_time;
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Remaining time: %d seconds"), remaining);
		return;
	}

	// Cooldown'u ayarla (1 saniye)
	ch->SetQuestFlag("collector.window_cooldown", current_time + 1);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenCollectWindow");
	quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), 13, 2);
}
#endif

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
ACMD(do_add_collect_item)
{
	if (!ch || !ch->IsPC())
	{
		return;
	}

	char arg1[256];
	char arg2[256];
	char arg3[256];

	const char* line;
	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
	{
		return;
	}

	if (!ch->CanWarp())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Lütfen biraz bekle!");
		return;
	}

	BYTE collectID = std::atoi(arg1);
	BYTE itemID = std::atoi(arg2);
	BYTE isAll = std::atoi(arg3);
	CSystemCollections::instance().AddItem(ch, collectID, itemID, (isAll == 1) ? true : false);
}
#endif

#ifdef ENABLE_MINIGAME_RUMI_EVENT
ACMD(do_cards)
{
	const char *line;

	char arg1[256], arg2[256];

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	switch (LOWER(arg1[0]))
	{
		case 'o':	// open
			if (isdigit(*arg2))
			{
				DWORD safemode;
				str_to_number(safemode, arg2);
				ch->Cards_open(safemode);
			}
			break;
		case 'p':	// open
			ch->Cards_pullout();
			break;
		case 'e':	// open
			ch->CardsEnd();
			break;
		case 'd':	// open
			if (isdigit(*arg2))
			{
				DWORD destroy_index;
				str_to_number(destroy_index, arg2);
				ch->CardsDestroy(destroy_index);
			}
			break;
		case 'a':	// open
			if (isdigit(*arg2))
			{
				DWORD accpet_index;
				str_to_number(accpet_index, arg2);
				ch->CardsAccept(accpet_index);
			}
			break;
		case 'r':	// open
			if (isdigit(*arg2))
			{
				DWORD restore_index;
				str_to_number(restore_index, arg2);
				ch->CardsRestore(restore_index);
			}
			break;
		default:
			return;
	}
}
#endif

#ifdef ENABLE_GAYA_SHOP_SYSTEM
ACMD(do_gem)
{
	char arg1[255];
	char arg2[255];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (0 == arg1[0])
		return;

	const std::string& strArg1 = std::string(arg1);

	if (strArg1 == "craft") {

		if (0 == arg2[0])
			return;

		int slot = atoi(arg2);
		ch->CraftGemItems(slot);

	}
	else if (strArg1 == "market") {

		if (0 == arg2[0])
			return;

		int slot = atoi(arg2);
		ch->MarketGemItems(slot);

	}
	else if (strArg1 == "refresh")
	{
		ch->RefreshGemItems();
	}
}
#endif

#ifdef ENABLE_GAYA_TICKET_SYSTEM
ACMD(do_use_gem_ticket)
{
	if(ch->IsDead() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsOpenSafebox() || ch->IsCubeOpen()
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		|| ch->GetShopOwner()
#endif
		)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?u anda özel depoyu kullanamazsyn. "));
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	
	if (!*arg1 || !*arg2)
		return;
	
	int itemPos = atoi(arg1);
	int inputGem = atoi(arg2);
	
	if (itemPos < 0 || itemPos >= INVENTORY_MAX_NUM)
		return;
	
	if (inputGem < 0)
		return;
	
	if (inputGem > ch->GetGem())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GEM_COUPON_ENOUGH_GEM"));
		return;
	}
	
	if (inputGem > 999999)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GEM_COUPON_MAX_GEM"));
		return;
	}
	
	LPITEM item = ch->GetInventoryItem(itemPos);
	
	if (!item)
		return;
	
	if (item->GetSocket(0) != 0)
		return;
	
	if (item->GetVnum() != 50000)
		return;
	
	ch->PointChange(POINT_GEM, -inputGem, false);
	item->SetSocket(0, inputGem);
}
#endif

#ifdef ENABLE_BATTLE_PASS
ACMD(open_battlepass)
{
	if (!ch)
		return;

	if (quest::CQuestManager::instance().GetEventFlag("battlepas_open") == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Sava? bileti etkinli?i ?uanda aktif de?il."));
		return;
	}

	if (ch->v_counts.empty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aktif savas biletin yok."));
		return;
	}

	if (ch->missions_bp.empty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aktif gorevin yok."));
		return;
	}

	if (ch->IsObserverMode())
		return;

	if (ch->IsWarping())
		return;

	if (ch->IsDead() || ch->IsStun() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsCubeOpen() || ch->GetSafebox() || ch->IsOpenSafebox() || ch->IsHack()
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		|| ch->IsAcceOpened(true) || ch->IsAcceOpened(false)
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
		|| ch->GetOfflineShopGuest() || ch->GetSafebox() || ch->GetAuctionGuest()
#endif
		)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?u anda özel depoyu kullanamazsyn. "));
		return;
	}

#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);
	int month = vKey.tm_mon;
	if (month != ch->iMonthBattlePass)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu ay sava? bileti etkinli?i bulunamady."));
		return;
	}
#endif

	for (int i = 0; i < ch->missions_bp.size(); ++i)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "missions_bp %d %d %d %d %d", i, ch->missions_bp[i].type, ch->missions_bp[i].vnum, ch->missions_bp[i].count, ch->missions_bp[i].ep);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "info_missions_bp %d %d %d %s", i, ch->v_counts[i].count, ch->v_counts[i].status, ch->rewards_bp[i].name);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "rewards_missions_bp %d %d %d %d %d %d %d", i, ch->rewards_bp[i].vnum1, ch->rewards_bp[i].vnum2, ch->rewards_bp[i].vnum3, ch->rewards_bp[i].count1, ch->rewards_bp[i].count2, ch->rewards_bp[i].count3);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "rewards_missions_bonus_bp %d %d %d %d %d %d %d", i, ch->rewards_bonus_bp[i].vnum1, ch->rewards_bonus_bp[i].vnum2, ch->rewards_bonus_bp[i].vnum3, ch->rewards_bonus_bp[i].count1, ch->rewards_bonus_bp[i].count2, ch->rewards_bonus_bp[i].count3);
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "size_missions_bp %d ", ch->missions_bp.size());
	ch->ChatPacket(CHAT_TYPE_COMMAND, "final_reward %d %d %d %d %d %d", ch->final_rewards[0].f_vnum1, ch->final_rewards[0].f_vnum2, ch->final_rewards[0].f_vnum3, ch->final_rewards[0].f_count1, ch->final_rewards[0].f_count2, ch->final_rewards[0].f_count3);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "show_battlepass");
}

ACMD(final_reward_battlepass)
{
	if (!ch)
		return;

	if (quest::CQuestManager::instance().GetEventFlag("battlepas_reward") == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Sava? bileti etkinli?i ?uanda aktif de?il."));
		return;
	}

	if (ch->v_counts.empty())
		return;

	if (ch->missions_bp.empty())
		return;

	if (ch->v_counts[0].status == 2)
		return;

#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);
	int month = vKey.tm_mon;
	if (month != ch->iMonthBattlePass)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu ay sava? bileti etkinli?i bulunamady."));
		return;
	}
#endif

	for (int i = 0; i < ch->missions_bp.size(); ++i)
	{
		if (ch->missions_bp[i].count != ch->v_counts[i].count)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterli miktarda görev bulunamady."));
			return;
		}
	}

	ch->FinalRewardBattlePass();
}

ACMD(battlepass_bitirici)
{
	if (!ch)
		return;

	if (quest::CQuestManager::instance().GetEventFlag("battlepass_bitirici") == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Sava? bileti etkinli?i ?uanda aktif de?il."));
		return;
	}

	if (ch->v_counts.empty())
		return;

	if (ch->missions_bp.empty())
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	int indexid = 0;
	str_to_number(indexid, arg1);

#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);
	int month = vKey.tm_mon;
	if (month != ch->iMonthBattlePass)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu ay sava? bileti etkinli?i bulunamady."));
		return;
	}
#endif

	ch->SendDoneBattlePass(indexid);
}

//premium
ACMD(open_battlepass_premium)
{
	if (!ch)
		return;

	if (quest::CQuestManager::instance().GetEventFlag("battlepas_open_premium") == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Sava? bileti etkinli?i ?uanda aktif de?il."));
		return;
	}

	if (ch->v_counts_premium.empty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aktif savas biletin yok."));
		return;
	}

	if (ch->missions_bp_premium.empty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Aktif gorevin yok."));
		return;
	}

	if (ch->IsObserverMode())
		return;

	if (ch->IsWarping())
		return;

	if (ch->IsDead() || ch->IsStun() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsCubeOpen() || ch->GetSafebox() || ch->IsOpenSafebox() || ch->IsHack()
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		|| ch->IsAcceOpened(true) || ch->IsAcceOpened(false)
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
		|| ch->GetOfflineShopGuest() || ch->GetSafebox() || ch->GetAuctionGuest()
#endif
		)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?u anda özel depoyu kullanamazsyn. "));
		return;
	}

#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);
	int month = vKey.tm_mon;
	if (month != ch->iMonthBattlePassPremium)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu ay sava? bileti etkinli?i bulunamady."));
		return;
	}
#endif

	for (int i = 0; i < ch->missions_bp_premium.size(); ++i)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "missions_bp_premium %d %d %d %d %d", i, ch->missions_bp_premium[i].type, ch->missions_bp_premium[i].vnum, ch->missions_bp_premium[i].count, ch->missions_bp_premium[i].ep);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "info_missions_bp_premium %d %d %d %s", i, ch->v_counts_premium[i].count, ch->v_counts_premium[i].status, ch->rewards_bp_premium[i].name);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "rewards_missions_bp_premium %d %d %d %d %d %d %d", i, ch->rewards_bp_premium[i].vnum1, ch->rewards_bp_premium[i].vnum2, ch->rewards_bp_premium[i].vnum3, ch->rewards_bp_premium[i].count1, ch->rewards_bp_premium[i].count2, ch->rewards_bp_premium[i].count3);
		ch->ChatPacket(CHAT_TYPE_COMMAND, "rewards_missions_bonus_bp_premium %d %d %d %d %d %d %d", i, ch->rewards_bonus_bp_premium[i].vnum1, ch->rewards_bonus_bp_premium[i].vnum2, ch->rewards_bonus_bp_premium[i].vnum3, ch->rewards_bonus_bp_premium[i].count1, ch->rewards_bonus_bp_premium[i].count2, ch->rewards_bonus_bp_premium[i].count3);
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "size_missions_bp_premium %d ", ch->missions_bp_premium.size());
	ch->ChatPacket(CHAT_TYPE_COMMAND, "final_reward_premium %d %d %d %d %d %d", ch->final_rewards_premium[0].f_vnum1, ch->final_rewards_premium[0].f_vnum2, ch->final_rewards_premium[0].f_vnum3, ch->final_rewards_premium[0].f_count1, ch->final_rewards_premium[0].f_count2, ch->final_rewards_premium[0].f_count3);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "show_battlepass_premium");
}

ACMD(final_reward_battlepass_premium)
{
	if (!ch)
		return;

	if (quest::CQuestManager::instance().GetEventFlag("battlepas_reward_premium") == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Sava? bileti etkinli?i ?uanda aktif de?il."));
		return;
	}

	if (ch->v_counts_premium.empty())
		return;

	if (ch->missions_bp_premium.empty())
		return;

	if (ch->v_counts_premium[0].status == 2)
		return;

#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);
	int month = vKey.tm_mon;
	if (month != ch->iMonthBattlePassPremium)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu ay sava? bileti etkinli?i bulunamady."));
		return;
	}
#endif

	for (int i = 0; i < ch->missions_bp_premium.size(); ++i)
	{
		if (ch->missions_bp_premium[i].count != ch->v_counts_premium[i].count)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Yeterli miktarda görev bulunamady."));
			return;
		}
	}

	ch->FinalRewardBattlePassPremium();
}

ACMD(battlepass_bitirici_premium)
{
	if (!ch)
		return;

	if (quest::CQuestManager::instance().GetEventFlag("battlepass_bitirici_premium") == 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Sava? bileti etkinli?i ?uanda aktif de?il."));
		return;
	}

	if (ch->v_counts_premium.empty())
		return;

	if (ch->missions_bp_premium.empty())
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	int indexid = 0;
	str_to_number(indexid, arg1);

#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);
	int month = vKey.tm_mon;
	if (month != ch->iMonthBattlePassPremium)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Bu ay sava? bileti etkinli?i bulunamady."));
		return;
	}
#endif

	ch->SendDoneBattlePassPremium(indexid);
}

#endif

#ifdef ENABLE_RANKING
ACMD(do_ranking_subcategory)
{
	if (!ch)
		return;

	if (ch->IsObserverMode())
		return;

	if (ch->IsDead() || ch->IsStun() || ch->GetExchange() || ch->GetMyShop() || ch->GetShopOwner() || ch->IsCubeOpen() || ch->GetSafebox()
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		|| ch->IsAcceOpened(true) || ch->IsAcceOpened(false)
#endif
#ifdef __ENABLE_NEW_OFFLINESHOP__
		|| ch->GetOfflineShopGuest() || ch->GetSafebox() || ch->GetAuctionGuest()
#endif
		)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("?u anda özel depoyu kullanamazsyn."));
		return;
	}

	if (ch->GetProtectTime("RANK_OPEN_TIME") > get_global_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Kategoriler arasy gezerken 1 saniye beklemelisin."));
		return;
	}

	ch->SetProtectTime("RANK_OPEN_TIME", get_global_time() + 1);

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
		return;

	ch->RankingSubcategory(atoi(arg1));
}
#endif

#ifdef ENABLE_CONQUEROR_LEVEL
ACMD(do_conqueror_stat)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "[LS;521]");
		return;
	}

	if (ch->GetPoint(POINT_CONQUEROR_POINT) <= 0)
		return;

	BYTE idx = 0;

	if (!strcmp(arg1, "smh_str"))
		idx = POINT_SUNGMA_STR;
	else if (!strcmp(arg1, "smh_hp"))
		idx = POINT_SUNGMA_HP;
	else if (!strcmp(arg1, "smh_move"))
		idx = POINT_SUNGMA_MOVE;
	else if (!strcmp(arg1, "smh_immune"))
		idx = POINT_SUNGMA_IMMUNE;
	else
	{
		return;
	}

	if (ch->GetRealPoint(idx) >= MAX_STAT)
		return;

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + 1);
	ch->SetPoint(idx, ch->GetPoint(idx) + 1);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_SUNGMA_HP)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}

	ch->PointChange(POINT_CONQUEROR_POINT, -1);
	ch->ComputePoints();
}
#endif

#ifdef ENABLE_AUTO_CAPE_SYSTEM
// Existing cape items and UseItem enforce normal item use and consumption rules.
namespace {
std::map<DWORD, LPEVENT> optionCapeEvents;
EVENTINFO(OptionCapeInfo) { DWORD pid; DWORD vid; };

void StopOptionCape(LPCHARACTER ch)
{
    const auto it = optionCapeEvents.find(ch->GetPlayerID());
    if (it != optionCapeEvents.end()) {
        event_cancel(&it->second);
        optionCapeEvents.erase(it);
    }
}

LPITEM FindOptionCape(LPCHARACTER ch)
{
    const DWORD capes[] = { UNIQUE_ITEM_CAPE_OF_COURAGE, 70057, REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE };
    for (DWORD vnum : capes) {
        LPITEM item = ch->FindSpecifyItem(vnum);
        if (item && item->GetWindow() == INVENTORY && item->GetCount()) return item;
    }
    return nullptr;
}

EVENTFUNC(option_cape_tick)
{
    OptionCapeInfo* info = dynamic_cast<OptionCapeInfo*>(event->info);
    if (!info) return 0;
    LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->pid);
    if (!ch || static_cast<DWORD>(ch->GetVID()) != info->vid || !ch->GetDesc()) {
        optionCapeEvents.erase(info->pid);
        return 0;
    }
    if (!ch->IsDead() && !ch->IsStun() && !ch->IsWarping() && ch->CanHandleItem()) {
        LPITEM cape = FindOptionCape(ch);
        if (!cape) {
            ch->SetQuestFlag("advanced_options.cape_active", 0);
            ch->ChatPacket(CHAT_TYPE_COMMAND, "OptionAutoCape 0 2");
            optionCapeEvents.erase(info->pid);
            return 0;
        }
        ch->UseItem(TItemPos(INVENTORY, cape->GetCell()));
    }
    return PASSES_PER_SEC(5);
}

void StartOptionCape(LPCHARACTER ch)
{
    StopOptionCape(ch);
    if (!FindOptionCape(ch)) {
        ch->SetQuestFlag("advanced_options.cape_active", 0);
        ch->ChatPacket(CHAT_TYPE_COMMAND, "OptionAutoCape 0 2");
        return;
    }
    ch->SetQuestFlag("advanced_options.cape_active", 1);
    OptionCapeInfo* info = AllocEventInfo<OptionCapeInfo>();
    info->pid = ch->GetPlayerID(); info->vid = ch->GetVID();
    optionCapeEvents[info->pid] = event_create(option_cape_tick, info, PASSES_PER_SEC(1));
    ch->ChatPacket(CHAT_TYPE_COMMAND, "OptionAutoCape 1 1");
}
}

ACMD(do_auto_cape)
{
    if (!ch || !ch->IsPC() || !ch->GetDesc()) return;
    char action[32], value[32];
    two_arguments(argument, action, sizeof(action), value, sizeof(value));
    if (!strcmp(action, "start")) StartOptionCape(ch);
    else if (!strcmp(action, "stop")) {
        StopOptionCape(ch);
        ch->SetQuestFlag("advanced_options.cape_active", 0);
        ch->ChatPacket(CHAT_TYPE_COMMAND, "OptionAutoCape 0 0");
    }
    else if (!strcmp(action, "resume")) {
        const bool enabled = !strcmp(value, "1");
        ch->SetQuestFlag("advanced_options.cape_after_warp", enabled ? 1 : 0);
    }
    else if (!strcmp(action, "sync")) {
        if (!strcmp(value, "1") && ch->GetQuestFlag("advanced_options.cape_active"))
            StartOptionCape(ch);
        else {
            StopOptionCape(ch);
            ch->SetQuestFlag("advanced_options.cape_active", 0);
            ch->ChatPacket(CHAT_TYPE_COMMAND, "OptionAutoCape 0 3");
        }
    }
    else ch->ChatPacket(CHAT_TYPE_INFO, "Otomatik pelerin: /auto_cape start veya /auto_cape stop (5 saniye).");
}
#endif

#ifdef ENABLE_CONFIRM_LEVELUP
ACMD(do_confirm_levelup)
{
    if (!ch || !ch->IsPC() || !ch->GetDesc()) return;
    ch->ConfirmLevelUp();
}
#endif
