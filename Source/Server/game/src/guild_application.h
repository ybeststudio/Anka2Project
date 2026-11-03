#pragma once

#include "../../common/service.h"

#ifdef ENABLE_GUILD_APPLICATION
// Guild application (reference Baerim uiGuildApplication): guild list, apply / join, leader approval,
// leader settings (status / language / min. level / description) and the sliding join notification.
// Client <-> server traffic: "/guild_app <sub> ..." commands and "GuildApp..." server commands.
void GuildApplication_Command(LPCHARACTER ch, const char* argument);
void GuildApplication_OnLogin(LPCHARACTER ch);
#endif
