#pragma once
#include "../../common/service.h"

#ifdef ENABLE_GUILD_SKILL_TREE
enum EGuildSkillTree
{
	GUILD_SKILL_TREE_STONE		= 0,	// active: reference Stone Destroyer
	GUILD_SKILL_TREE_WRATH		= 1,
	GUILD_SKILL_TREE_REFINE		= 2,
	GUILD_SKILL_TREE_DUNGEON	= 3,
	GUILD_SKILL_TREE_MINE		= 4,
	GUILD_SKILL_TREE_MEMBERS	= 5,	// passive: reference Banner of Brotherhood
	GUILD_SKILL_TREE_QUEST		= 6,
	GUILD_SKILL_TREE_SILVER		= 7,
	GUILD_SKILL_TREE_DONATION	= 8,
	GUILD_SKILL_TREE_ENERGY		= 9,
	GUILD_SKILL_TREE_COUNT		= 10,
};

void GuildSkill_Command(LPCHARACTER ch, const char* argument);
int GuildSkill_GetValue(CGuild* guild, int skill);	// 0 for an active skill while it is not activated
#endif
