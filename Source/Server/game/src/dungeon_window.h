#pragma once
#include "../../common/service.h"

#ifdef ENABLE_DUNGEON_WINDOW_BAERIM
// Baerim dungeon window (uiTeleport dungeon page): per-dungeon statistics, daily limit and direct enter.
// Keys are the boss races of m_mapDungeonList (constants.cpp), the same keys the client window uses.
void DungeonWindow_OnEnterGame(LPCHARACTER ch);
void DungeonWindow_OnBossHit(LPCHARACTER attacker, LPCHARACTER boss, int damage);
void DungeonWindow_OnBossDead(LPCHARACTER boss);
void DungeonWindow_Command(LPCHARACTER ch, const std::vector<std::string>& args);
std::string DungeonWindow_ExtraInfo(LPCHARACTER ch, DWORD bossIdx);
#endif
