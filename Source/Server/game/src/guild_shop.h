#pragma once
#include "../../common/service.h"

#ifdef ENABLE_GUILD_SHOP
enum EGuildLogType
{
	GUILD_LOG_DONATION	= 1,
	GUILD_LOG_SKILL		= 2,
};

void GuildShop_Command(LPCHARACTER ch, const char* argument);
void GuildLog_Add(CGuild* guild, int type, const char* name, int a, int b, int c);
#endif
