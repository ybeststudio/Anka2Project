#include "../GameLib/RaceData.h"
#include "../GameLib/RaceManager.h"
#include "../GameLib/ActorInstance.h"
#include <cstdint>

class AlphaSpecialVnums
{
public:
    static void ApplyAlphaOnVnums(DWORD eRace, float& fSpecular) 
    {
        CRaceData* pRaceData;
        if (!CRaceManager::Instance().GetRaceDataPointer(eRace, &pRaceData))
            return;
        CActorInstance actor_erace;
        if (actor_erace.IsNPC())
        {
            fSpecular = 1.0f;
        };
        if (actor_erace.IsStone())
        {
            fSpecular = 0.2f;
        }
        static const DWORD specificRaces[] = 
        {
            191, 192, 491, 193, 492, 493, 591, 494, 691, 791, 2091, 2191, 2291, 3690, 3691, 3490, 3491, 1192,
            1901, 2206, 2207, 1191, 1091, 1092, 1304, 1305, 1306, 2092, 1093, 2306, 2307, 2591, 2592, 2593, 2594, 2596,
            2598, 3910, 3890, 3891, 3290, 3291, 3390, 3391, 3190, 3191, 2493, 6091, 6191, 6151, 3590, 3790, 3791,
            3591, 3595, 3596, 2491, 2492, 20015, 9005, 501, 502, 503, 504, 531, 532, 533, 534, 551, 552, 553,
            554, 591, 595, 451, 452, 454, 455, 456, 20118
        };
        const size_t numSpecificRaces = sizeof(specificRaces) / sizeof(specificRaces[0]);
        for (size_t i = 0; i < numSpecificRaces; ++i) 
        {
            if (eRace == specificRaces[i]) 
            {
                fSpecular = 0.9f;
                break;
            }
        }
    }
};