#pragma once

#include "../../common/service.h"

#ifdef ENABLE_MESSENGER_DETAILS
// Messenger member details (reference Baerim uiMessenger cards): level / race / channel / map of online members and
// the last seen time of offline ones, for the friend and guild groups. Sent as "MessengerInfo" server commands next to
// the regular messenger packets, which stay unchanged.
void MessengerDetails_OnLogin(LPCHARACTER ch);
void MessengerDetails_OnFriendLogin(const char* account, const char* companion);
#endif
