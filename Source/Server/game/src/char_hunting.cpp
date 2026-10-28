#include "stdafx.h"

#ifdef ENABLE_HUNTING_SYSTEM

#include "../../common/VnumHelper.h"

#include "char.h"
#include "utils.h"
#include "crc32.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "item.h"
#include "item_manager.h"

void CHARACTER::CheckHunting(bool isLevelUp)
{
	if (GetQuestFlag("hunting_system.is_active") == 0)
	{
		SetQuestFlag("hunting_system.is_active", -1);
		SetQuestFlag("hunting_system.level", 1);
		SetQuestFlag("hunting_system.type", -1);
		SetQuestFlag("hunting_system.count", 0);
	}

	// SECURITY: DB'de kalmis bozuk type/level degerleri array OOB'a yol acmasin
	if (GetQuestFlag("hunting_system.is_active") == 1)
	{
		const int iType = GetQuestFlag("hunting_system.type");
		const int iLevel = GetQuestFlag("hunting_system.level");
		if (iType < 0 || iType > 1 || iLevel < 0 || iLevel > HUNTING_MISSION_COUNT)
		{
			sys_err("CheckHunting: invalid flags reset (pid %u type %d level %d)", GetPlayerID(), iType, iLevel);
			SetQuestFlag("hunting_system.is_active", -1);
			SetQuestFlag("hunting_system.type", -1);
			SetQuestFlag("hunting_system.count", 0);
			if (iLevel < 0 || iLevel > HUNTING_MISSION_COUNT)
				SetQuestFlag("hunting_system.level", 1);
		}
	}

	if (GetQuestFlag("hunting_system.is_active") == -1 and GetLevel() >= GetQuestFlag("hunting_system.level"))
		ChatPacket(CHAT_TYPE_COMMAND, "HuntingButtonFlash");

	if (GetQuestFlag("hunting_system.is_active") == 1 and GetQuestFlag("hunting_system.count") == THuntingMissions[GetQuestFlag("hunting_system.level")][GetQuestFlag("hunting_system.type")][1])
		ChatPacket(CHAT_TYPE_COMMAND, "HuntingButtonFlash");
}

void CHARACTER::OpenHuntingWindowMain()
{
	if (GetQuestFlag("hunting_system.is_active") == 1 and GetLevel() >= GetQuestFlag("hunting_system.level")) 
	{
		DWORD actLevel = GetQuestFlag("hunting_system.level");
		BYTE actType = GetQuestFlag("hunting_system.type");
		DWORD actCount = GetQuestFlag("hunting_system.count");

		SendRandomItemPacket(false);

		TPacketGCOpenWindowHuntingMain pack;
		pack.bHeader = HEADER_GC_HUNTING_OPEN_MAIN;
		pack.dLevel = GetQuestFlag("hunting_system.level");
		pack.dMonster = THuntingMissions[actLevel][actType][0];
		pack.dCurCount = actCount;
		pack.dDestCount = THuntingMissions[actLevel][actType][1];
		pack.dMoneyMin = GetMinMaxMoney(actLevel, false);
		pack.dMoneyMax = GetMinMaxMoney(actLevel, true);
		pack.dExpMin = GetMinMaxExp(actLevel, false);
		pack.dExpMax = GetMinMaxExp(actLevel, true);
		pack.dRaceItem = THuntingRewardItem[actLevel][actType][GetRaceTable()][0];
		pack.dRaceItemCount = THuntingRewardItem[actLevel][actType][GetRaceTable()][1];

		GetDesc()->Packet(&pack, sizeof(pack));
	}
	else
	{
		if (GetLevel() < GetQuestFlag("hunting_system.level")) 
		{
			TPacketGCOpenWindowHuntingMain pack;
			pack.bHeader = HEADER_GC_HUNTING_OPEN_MAIN;
			pack.dLevel = GetQuestFlag("hunting_system.level");
			pack.dMonster, pack.dCurCount, pack.dDestCount = 0;
			pack.dMoneyMin, pack.dMoneyMax = 0;
			pack.dExpMin, pack.dExpMax = 0;
			pack.dRaceItem, pack.dRaceItemCount = 0;
			GetDesc()->Packet(&pack, sizeof(pack));
		}
		else
		{
			OpenHuntingWindowSelect();
		}
	}
}

void CHARACTER::OpenHuntingWindowSelect()
{
	DWORD actLevel = GetQuestFlag("hunting_system.level");
	BYTE actType = GetQuestFlag("hunting_system.type");
	DWORD actCount = GetQuestFlag("hunting_system.count");

	SendRandomItemPacket(true);

	for (BYTE type = 0; type < 2; ++type)
	{
		TPacketGCOpenWindowHuntingSelect pack;
		pack.bHeader = HEADER_GC_HUNTING_OPEN_SELECT;
		pack.dLevel = GetQuestFlag("hunting_system.level");
		pack.bType = type;
		pack.dMonster = THuntingMissions[actLevel][type][0];
		pack.dCount = THuntingMissions[actLevel][type][1];
		pack.dMoneyMin = GetMinMaxMoney(actLevel, false);
		pack.dMoneyMax = GetMinMaxMoney(actLevel, true);
		pack.dExpMin = GetMinMaxExp(actLevel, false);
		pack.dExpMax = GetMinMaxExp(actLevel, true);
		pack.dRaceItem = THuntingRewardItem[actLevel][type][GetRaceTable()][0];
		pack.dRaceItemCount = THuntingRewardItem[actLevel][type][GetRaceTable()][1];
		GetDesc()->Packet(&pack, sizeof(pack));
	}
}

void CHARACTER::OpenHuntingWindowReward()
{
	if (GetQuestFlag("hunting_system.reward_cached") == 1)
	{
		TPacketGCOpenWindowReward pack;
		pack.bHeader = HEADER_GC_HUNTING_OPEN_REWARD;
		pack.dLevel = GetQuestFlag("hunting_system.level");
		pack.dReward = GetQuestFlag("hunting_system.reward_race");
		pack.dRewardCount = GetQuestFlag("hunting_system.reward_race_count");
		pack.dRandomReward = GetQuestFlag("hunting_system.reward_rand");
		pack.dRandomRewardCount = GetQuestFlag("hunting_system.reward_rand_count");
		pack.dMoney = GetQuestFlag("hunting_system.reward_money");
		pack.bExp = GetQuestFlag("hunting_system.reward_exp");

		GetDesc()->Packet(&pack, sizeof(pack));
	}
	else
		LocaleChatPacket(CHAT_TYPE_INFO, 199, "");
}

void CHARACTER::UpdateHuntingMission(DWORD dwMonsterVnum)
{
	if (!GetDesc())
		return;

	if (GetQuestFlag("hunting_system.is_active") == -1)
		return;

	int iLevel = GetQuestFlag("hunting_system.level");
	int iType = GetQuestFlag("hunting_system.type");

	if (GetLevel() >= GetQuestFlag("hunting_system.level") and THuntingMissions[iLevel][iType][0] == dwMonsterVnum and THuntingMissions[iLevel][iType][1] > GetQuestFlag("hunting_system.count"))
	{
		SetQuestFlag("hunting_system.count", GetQuestFlag("hunting_system.count")+1);

		TPacketGCUpdateHunting pack;
		pack.bHeader = HEADER_GC_HUNTING_UPDATE;
		pack.dCount = GetQuestFlag("hunting_system.count");

		GetDesc()->Packet(&pack, sizeof(pack));
	}
	if (THuntingMissions[iLevel][iType][1] <= GetQuestFlag("hunting_system.count") and GetQuestFlag("hunting_system.reward_cached") == 0)
	{
		SetCachedRewards();
		ChatPacket(CHAT_TYPE_COMMAND, "HuntingButtonFlash");
	}
}

void CHARACTER::ReciveHuntingRewards()
{
	if (GetQuestFlag("hunting_system.reward_race") != 0) { 
		int item_vnum = GetQuestFlag("hunting_system.reward_race");
		int item_count = GetQuestFlag("hunting_system.reward_race_count");

		if (item_count == 0)
			item_count = 1;

		LPITEM RaceItem = ITEM_MANAGER::instance().CreateItem(item_vnum, item_count, 0, false);

		int cell = GetEmptyInventory(RaceItem->GetSize());
		if (cell != -1)
		{
			if (RaceItem->IsDragonSoul())
				RaceItem->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, cell));
#ifdef ENABLE_SPECIAL_INVENTORY
			else if (RaceItem->IsSkillBook())
				RaceItem->AddToCharacter(this, TItemPos(INVENTORY, cell));
			else if (RaceItem->IsUpgradeItem())
				RaceItem->AddToCharacter(this, TItemPos(INVENTORY, cell));
			else if (RaceItem->IsStone())
				RaceItem->AddToCharacter(this, TItemPos(INVENTORY, cell));
			else if (RaceItem->IsGiftBox())
				RaceItem->AddToCharacter(this, TItemPos(INVENTORY, cell));
			else if (RaceItem->IsChanger())
				RaceItem->AddToCharacter(this, TItemPos(INVENTORY, cell));
#endif
			else
				RaceItem->AddToCharacter(this, TItemPos(INVENTORY, cell));

			if (RaceItem->GetType() == ITEM_USE && RaceItem->GetSubType() == USE_POTION)
			{
				TQuickslot * pSlot;
				if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
				{
					TQuickslot slot;
					slot.type = QUICKSLOT_TYPE_ITEM;
					slot.pos = cell;
					SetQuickslot(0, slot);
				}
			}
		}
		else
		{
			RaceItem->AddToGround(GetMapIndex(), GetXYZ());
			RaceItem->StartDestroyEvent();
			RaceItem->SetOwnership(this, 60);
		}
		SetQuestFlag("hunting_system.reward_race", 0);
		SetQuestFlag("hunting_system.reward_race_count", 0);
	}

	if (GetQuestFlag("hunting_system.reward_rand") != 0) { 
		int item_vnum = GetQuestFlag("hunting_system.reward_rand");
		int item_count = GetQuestFlag("hunting_system.reward_rand_count");

		if (item_count == 0)
			item_count = 1;

		LPITEM RandItem = ITEM_MANAGER::instance().CreateItem(item_vnum, item_count, 0, false);

		int cell = GetEmptyInventory(RandItem->GetSize());
		if (cell != -1)
		{
			if (RandItem->IsDragonSoul())
				RandItem->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, cell));
#ifdef ENABLE_SPECIAL_INVENTORY
			else if (RandItem->IsSkillBook())
				RandItem->AddToCharacter(this, TItemPos(INVENTORY, cell));
			else if (RandItem->IsUpgradeItem())
				RandItem->AddToCharacter(this, TItemPos(INVENTORY, cell));
			else if (RandItem->IsStone())
				RandItem->AddToCharacter(this, TItemPos(INVENTORY, cell));
			else if (RandItem->IsGiftBox())
				RandItem->AddToCharacter(this, TItemPos(INVENTORY, cell));
			else if (RandItem->IsChanger())
				RandItem->AddToCharacter(this, TItemPos(INVENTORY, cell));
#endif
			else
				RandItem->AddToCharacter(this, TItemPos(INVENTORY, cell));

			if (RandItem->GetType() == ITEM_USE && RandItem->GetSubType() == USE_POTION)
			{
				TQuickslot * pSlot;
				if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
				{
					TQuickslot slot;
					slot.type = QUICKSLOT_TYPE_ITEM;
					slot.pos = cell;
					SetQuickslot(0, slot);
				}
			}
		}
		else
		{
			RandItem->AddToGround(GetMapIndex(), GetXYZ());
			RandItem->StartDestroyEvent();
			RandItem->SetOwnership(this, 60);
		}

		SetQuestFlag("hunting_system.reward_rand", 0);
		SetQuestFlag("hunting_system.reward_rand_count", 0);
	}

	if (GetQuestFlag("hunting_system.reward_money") != 0)
	{
		PointChange(POINT_GOLD, GetQuestFlag("hunting_system.reward_money"), true);
		SetQuestFlag("hunting_system.reward_money", 0);
	}

	if (GetQuestFlag("hunting_system.reward_exp") != 0)
	{
		int level = GetQuestFlag("hunting_system.level");
		double proc = GetQuestFlag("hunting_system.reward_exp");

		DWORD exp = (DWORD)((exp_table[MINMAX(0, level, PLAYER_EXP_TABLE_MAX)] * proc) / 100);

		if (GetExp() + exp > GetNextExp())
			PointChange(POINT_EXP, GetNextExp() - 1 - GetExp());
		else
			PointChange(POINT_EXP, exp);

		SetQuestFlag("hunting_system.reward_exp", 0);
	}

	SetQuestFlag("hunting_system.reward_cached", 0);
	SetQuestFlag("hunting_system.is_active", -1);
	SetQuestFlag("hunting_system.type", -1);
	SetQuestFlag("hunting_system.count", 0);
	SetQuestFlag("hunting_system.level", GetQuestFlag("hunting_system.level")+1);
}

void CHARACTER::SetCachedRewards()
{
	SetQuestFlag("hunting_system.reward_cached", 1);

	DWORD actLevel = GetQuestFlag("hunting_system.level");
	BYTE actType = GetQuestFlag("hunting_system.type");

	SetQuestFlag("hunting_system.reward_race", THuntingRewardItem[actLevel][actType][GetRaceTable()][0]);
	SetQuestFlag("hunting_system.reward_race_count", THuntingRewardItem[actLevel][actType][GetRaceTable()][1]);
	SetQuestFlag("hunting_system.reward_money", GetRandomMoney(actLevel));
	SetQuestFlag("hunting_system.reward_exp", GetRandomExp(actLevel));
	SetRewardRandomItemFromTable();
}

int CHARACTER::GetRaceTable()
{
	switch (GetPlayerRace())
	{
		case 0:
		case 4:
			return 0;
		case 1:
		case 5:
			return 1;
		case 2:
		case 6:
			return 2;
		case 3:
		case 7:
			return 3;
	}
	return 0;
}

void CHARACTER::SetRewardRandomItemFromTable()
{
	DWORD MissionLevel = GetQuestFlag("hunting_system.level");

	DWORD itemVnum, itemCount;

	if (MissionLevel >= 1 and MissionLevel <= 20)
	{
		int tableSize = (sizeof THuntingRandomItem_01_20/sizeof THuntingRandomItem_01_20[0]);
		int randItem = number(0, tableSize-1);
		itemVnum = THuntingRandomItem_01_20[randItem][0];
		itemCount = THuntingRandomItem_01_20[randItem][1];
	}

	if (MissionLevel >= 21 and MissionLevel <= 40)
	{
		int tableSize = (sizeof THuntingRandomItem_21_40/sizeof THuntingRandomItem_21_40[0]);
		int randItem = number(0, tableSize-1);
		itemVnum = THuntingRandomItem_21_40[randItem][0];
		itemCount = THuntingRandomItem_21_40[randItem][1];
	}

	if (MissionLevel >= 41 and MissionLevel <= 60)
	{
		int tableSize = (sizeof THuntingRandomItem_41_60/sizeof THuntingRandomItem_41_60[0]);
		int randItem = number(0, tableSize-1);
		itemVnum = THuntingRandomItem_41_60[randItem][0];
		itemCount = THuntingRandomItem_41_60[randItem][1];
	}

	if (MissionLevel >= 61 and MissionLevel <= 80)
	{
		int tableSize = (sizeof THuntingRandomItem_61_80/sizeof THuntingRandomItem_61_80[0]);
		int randItem = number(0, tableSize-1);
		itemVnum = THuntingRandomItem_61_80[randItem][0];
		itemCount = THuntingRandomItem_61_80[randItem][1];
	}

	if (MissionLevel >= 81 and MissionLevel <= 90)
	{
		int tableSize = (sizeof THuntingRandomItem_81_90/sizeof THuntingRandomItem_81_90[0]);
		int randItem = number(0, tableSize-1);
		itemVnum = THuntingRandomItem_81_90[randItem][0];
		itemCount = THuntingRandomItem_81_90[randItem][1];
	}

	SetQuestFlag("hunting_system.reward_rand", itemVnum);
	SetQuestFlag("hunting_system.reward_rand_count", itemCount);
	return;
}

void CHARACTER::SendRandomItemPacket(bool IsSelectWindow)
{
	DWORD MissionLevel = GetQuestFlag("hunting_system.level");

	if (MissionLevel >= 1 and MissionLevel <= 20)
	{
		DWORD TableSize = (sizeof THuntingRandomItem_01_20/sizeof THuntingRandomItem_01_20[0]);
		for (int i = 0; i < TableSize; ++i)
		{
			TPacketGCReciveRandomItems pack;
			pack.bHeader = HEADER_GC_HUNTING_RECIVE_RAND_ITEMS;
			pack.bWindow = IsSelectWindow;
			pack.dItemVnum = THuntingRandomItem_01_20[i][0];
			pack.dItemCount = THuntingRandomItem_01_20[i][1];
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}

	if (MissionLevel >= 21 and MissionLevel <= 40)
	{
		DWORD TableSize = (sizeof THuntingRandomItem_21_40/sizeof THuntingRandomItem_21_40[0]);
		for (int i = 0; i < TableSize; ++i)
		{
			TPacketGCReciveRandomItems pack;
			pack.bHeader = HEADER_GC_HUNTING_RECIVE_RAND_ITEMS;
			pack.bWindow = IsSelectWindow;
			pack.dItemVnum = THuntingRandomItem_21_40[i][0];
			pack.dItemCount = THuntingRandomItem_21_40[i][1];
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}

	if (MissionLevel >= 41 and MissionLevel <= 60)
	{
		DWORD TableSize = (sizeof THuntingRandomItem_41_60/sizeof THuntingRandomItem_41_60[0]);
		for (int i = 0; i < TableSize; ++i)
		{
			TPacketGCReciveRandomItems pack;
			pack.bHeader = HEADER_GC_HUNTING_RECIVE_RAND_ITEMS;
			pack.bWindow = IsSelectWindow;
			pack.dItemVnum = THuntingRandomItem_41_60[i][0];
			pack.dItemCount = THuntingRandomItem_41_60[i][1];
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}

	if (MissionLevel >= 61 and MissionLevel <= 80)
	{
		DWORD TableSize = (sizeof THuntingRandomItem_61_80/sizeof THuntingRandomItem_61_80[0]);
		for (int i = 0; i < TableSize; ++i)
		{
			TPacketGCReciveRandomItems pack;
			pack.bHeader = HEADER_GC_HUNTING_RECIVE_RAND_ITEMS;
			pack.bWindow = IsSelectWindow;
			pack.dItemVnum = THuntingRandomItem_61_80[i][0];
			pack.dItemCount = THuntingRandomItem_61_80[i][1];
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}

	if (MissionLevel >= 81 and MissionLevel <= 90)
	{
		DWORD TableSize = (sizeof THuntingRandomItem_81_90/sizeof THuntingRandomItem_81_90[0]);
		for (int i = 0; i < TableSize; ++i)
		{
			TPacketGCReciveRandomItems pack;
			pack.bHeader = HEADER_GC_HUNTING_RECIVE_RAND_ITEMS;
			pack.bWindow = IsSelectWindow;
			pack.dItemVnum = THuntingRandomItem_81_90[i][0];
			pack.dItemCount = THuntingRandomItem_81_90[i][1];
			GetDesc()->Packet(&pack, sizeof(pack));
		}
	}
}

int CHARACTER::GetMinMaxMoney(DWORD missionLevel, bool AskMax)
{
	for(int i = 0; i < HUNTING_MONEY_TABLE_SIZE; ++i)
	{
		const SHuntingRewardMoney & info = THuntingRewardMoney[i];
		if (missionLevel >= info.dLevelFrom and missionLevel <= info.dLevelTo)
			if (!AskMax)
				return info.dRandMoney[0];
			else
				return info.dRandMoney[4];
	}
	return 0;
}

int CHARACTER::GetRandomMoney(DWORD missionLevel)
{
	for(int i = 0; i < HUNTING_MONEY_TABLE_SIZE; ++i)
	{
		const SHuntingRewardMoney & info = THuntingRewardMoney[i];
		if (missionLevel >= info.dLevelFrom and missionLevel <= info.dLevelTo)
		{
			int prob = number(1, 100);
			int money = 0;

			if (prob > 0) // 15%
				money = info.dRandMoney[0];

			if (prob > 15) // 25%
				money = info.dRandMoney[1];

			if (prob > 40) // 30%
				money = info.dRandMoney[2];

			if (prob > 70) // 20%
				money = info.dRandMoney[3];

			if (prob > 90) // 10%
				money = info.dRandMoney[4];

			LocaleChatPacket(CHAT_TYPE_INFO, 201, "%d", money);
			return money;
		}
	}
	return 0;
}

int CHARACTER::GetMinMaxExp(DWORD missionLevel, bool AskMax)
{
	for(int i = 0; i < HUNTING_EXP_TABLE_SIZE; ++i)
	{
		const SHuntingRewardEXP & info = THuntingRewardEXP[i];
		if (missionLevel >= info.dLevelFrom and missionLevel <= info.dLevelTo)
		{
			if (!AskMax)
				return info.dPerc[0];
			else
				return info.dPerc[1];
		}
	}
	return 0;
}

int CHARACTER::GetRandomExp(DWORD missionLevel)
{
	int percent = 0;
	for(int i = 0; i < HUNTING_EXP_TABLE_SIZE; ++i)
	{
		const SHuntingRewardEXP & info = THuntingRewardEXP[i];
		if (missionLevel >= info.dLevelFrom and missionLevel <= info.dLevelTo)
		{
			percent = number(info.dPerc[0], info.dPerc[1]);
			return percent;
		}
	}
	return 0;
}
#endif
