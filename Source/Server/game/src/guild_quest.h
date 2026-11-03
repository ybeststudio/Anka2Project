#pragma once
#include "../../common/service.h"

#ifdef ENABLE_GUILD_QUEST
enum EGuildQuestType
{
	GUILD_QUEST_METIN	= 1,
	GUILD_QUEST_DUNGEON	= 2,
	GUILD_QUEST_BOSS	= 3,
	GUILD_QUEST_FISH	= 4,	// fishing and mining share one quest
};

void GuildQuest_Command(LPCHARACTER ch, const char* argument);
void GuildQuest_Reward(LPCHARACTER ch);
void GuildQuest_OnLogin(LPCHARACTER ch);
void GuildQuest_AddProgress(LPCHARACTER ch, int type);
#endif
