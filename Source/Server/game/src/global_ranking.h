/*********************************************************************
 * File Name : global_ranking.h
 * Author    : BestStudio
 * Version   : 1.0.0
 * Date      : 07-11-2026  
 */

#pragma once
#include "../../common/service.h"

#ifdef ENABLE_GLOBAL_RANKING
// Weekly global ranking (reference Baerim uiRanking: 3 pages, CATEGORY_BUTTONS data ids).
enum EGlobalRankCategory
{
	GLOBAL_RANK_QUEST_BOOK			= 1,
	GLOBAL_RANK_METIN_30			= 2,	// 2..5, 41: destroyed metins by metin level band
	GLOBAL_RANK_METIN_55			= 3,
	GLOBAL_RANK_METIN_75			= 4,
	GLOBAL_RANK_METIN_91			= 5,
	GLOBAL_RANK_DUNGEON				= 7,
	GLOBAL_RANK_MINING				= 8,
	GLOBAL_RANK_FISH				= 9,
	GLOBAL_RANK_METIN_DAMAGE_30		= 10,	// 10..13, 43: highest hit on a metin
	GLOBAL_RANK_METIN_DAMAGE_55		= 11,
	GLOBAL_RANK_METIN_DAMAGE_75		= 12,
	GLOBAL_RANK_METIN_DAMAGE_91		= 13,
	GLOBAL_RANK_REFINE_FAIL			= 14,
	GLOBAL_RANK_REFINE_SUCCESS		= 15,
	GLOBAL_RANK_SWITCH_ITEM			= 16,
	GLOBAL_RANK_SWITCH_COSTUME		= 17,
	GLOBAL_RANK_SWITCH_SASH			= 18,
	GLOBAL_RANK_SWITCH_MOUNT		= 20,
	GLOBAL_RANK_ETERNITY_PIT		= 21,
	GLOBAL_RANK_KARUAK_EASY			= 22,
	GLOBAL_RANK_KARUAK_NORMAL		= 23,
	GLOBAL_RANK_KARUAK_HARD			= 24,
	GLOBAL_RANK_METIN_RELIC			= 25,
	GLOBAL_RANK_PVP_KILLS			= 27,
	GLOBAL_RANK_METIN_DPS_30		= 28,	// 28..31, 42: best damage per second on one metin
	GLOBAL_RANK_METIN_DPS_55		= 29,
	GLOBAL_RANK_METIN_DPS_75		= 30,
	GLOBAL_RANK_METIN_DPS_91		= 31,
	GLOBAL_RANK_BOSS_1				= 32,	// 32..35, 40: killed bosses by stage
	GLOBAL_RANK_BOSS_2				= 33,
	GLOBAL_RANK_BOSS_3				= 34,
	GLOBAL_RANK_BOSS_4				= 35,
	GLOBAL_RANK_EXPLORATION			= 36,
	GLOBAL_RANK_BLESSING			= 38,
	GLOBAL_RANK_ENERGY_CATALYST		= 39,
	GLOBAL_RANK_BOSS_5				= 40,
	GLOBAL_RANK_METIN_106			= 41,
	GLOBAL_RANK_METIN_DPS_106		= 42,
	GLOBAL_RANK_METIN_DAMAGE_106	= 43,
	GLOBAL_RANK_KARUAK_EXTREME		= 44,

	GLOBAL_RANK_CATEGORY_MAX		= 45,
};

void GlobalRanking_Add(LPCHARACTER ch, int category, long long value = 1);
void GlobalRanking_Max(LPCHARACTER ch, int category, long long value);
void GlobalRanking_OnHit(LPCHARACTER attacker, LPCHARACTER victim, int damage);
void GlobalRanking_OnDead(LPCHARACTER victim, LPCHARACTER killer);
void GlobalRanking_Command(LPCHARACTER ch, const char* argument);
#endif
