#pragma once
#include "../../common/service.h"

#ifdef ENABLE_GUILD_DONATION
void GuildDonation_Info(LPCHARACTER ch);
void GuildDonation_Donate(LPCHARACTER ch, const char* argument);
void GuildDonation_Whisper(LPCHARACTER ch, const char* argument);
void GuildDonation_AddGold(CGuild* guild, int gold);
bool GuildDonation_TakeGold(CGuild* guild, int gold);
bool GuildDonation_TakeSilver(LPCHARACTER ch, int silver);
void GuildDonation_AddSilver(LPCHARACTER ch, int silver);
#endif
