#pragma once

#ifdef ENABLE_MOKOKO_SYSTEM
// Baerim Mokoko seeds: NPC 9101 spawned on the teleport-window maps, collected once per character by clicking it.
// A collected seed is no longer sent to that character (per character visibility). Every 10 seeds is a milestone
// reward, chests at 100/200/300/400/450; rewards are claimed by hand in the client's reward window.
//   locale/<locale>/mokoko_seeds.txt   : id region mapIndex localX localY
//   locale/<locale>/mokoko_rewards.txt : id type(0 milestone, 1 chest) required vnum count
// Commands: /mokoko_info, /mokoko_rewards, /mokoko_claim <id>, /mokoko_reload (GM)
// Client:   MokokoData <found> <total> <pending> <region|found|total#...>
//           MokokoRewardsBegin <found> <total>, MokokoRewardEntry <id> <type> <required> <claimed> <vnum:count>,
//           MokokoRewardsEnd <pending>, MokokoRewardResult <code> <id>, MokokoFound <found> <total> <region> <regionFound> <regionTotal>
enum EMokokoRewardResult
{
	MOKOKO_REWARD_RESULT_OK,
	MOKOKO_REWARD_RESULT_INVALID,
	MOKOKO_REWARD_RESULT_LOCKED,
	MOKOKO_REWARD_RESULT_ALREADY_CLAIMED,
	MOKOKO_REWARD_RESULT_INVENTORY_FULL,
	MOKOKO_REWARD_RESULT_DATABASE_ERROR,
};

void Mokoko_SpawnMap(long lMapIndex, long lBaseX, long lBaseY);
bool Mokoko_IsHiddenFor(const CHARACTER* pkSeed, const CHARACTER* pkViewer);
bool Mokoko_OnClick(LPCHARACTER ch, LPCHARACTER pkSeed);
void Mokoko_OnLogin(LPCHARACTER ch);
void Mokoko_OnDestroy(LPCHARACTER ch);
void Mokoko_Command(LPCHARACTER ch, const char* szCommand, const char* argument);
#endif
