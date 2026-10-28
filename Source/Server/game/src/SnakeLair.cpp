#include "stdafx.h"

#ifdef ENABLE_QUEEN_NETHIS
#include "SnakeLair.h"

#include "item.h"
#include "char.h"
#include "utils.h"
#include "party.h"
#include "regen.h"
#include "config.h"
#include "packet.h"
#include "motion.h"
#include "item_manager.h"
#include "guild_manager.h"
#include "guild.h"
#include "start_position.h"
#include "locale_service.h"
#include "char_manager.h"
#include <boost/lexical_cast.hpp>

namespace SnakeLair
{
#ifdef ENABLE_YOHARA_SYSTEM
	int iSungmaQueenPoint[5][8] =
	{
		{ POINT_SUNGMA_STR,		35, 35, 38, 41, 44, 47, 50 },
		{ POINT_SUNGMA_HP,		25, 25, 28, 31, 34, 37, 47 },
		{ POINT_SUNGMA_MOVE,	35, 35, 38, 41, 44, 47, 50 },
		{ POINT_SUNGMA_IMMUNE,	40, 40, 43, 46, 49, 52, 55 },
	};
#endif

	int stoneKillSort[4][4] =
	{
		{ SNAKEMETIN_FIRE, SNAKEMETIN_ICE, SNAKEMETIN_WIND, SNAKEMETIN_EARTH }, //Fire, Ice, Wind, Earth
		{ SNAKEMETIN_ICE, SNAKEMETIN_EARTH, SNAKEMETIN_FIRE, SNAKEMETIN_WIND }, //Ice, Earth, Fire, Wind
		{ SNAKEMETIN_EARTH, SNAKEMETIN_FIRE, SNAKEMETIN_WIND, SNAKEMETIN_ICE }, //Earth, Fire, Wind, Ice
		{ SNAKEMETIN_WIND, SNAKEMETIN_EARTH, SNAKEMETIN_ICE, SNAKEMETIN_FIRE } //Wind, Earth, Ice, Fire
	};

	int EarthMonsterFloor1[15][3] =
	{
		{ 6750, 163, 156 },
		{ 6750, 167, 159 },
		{ 6750, 160, 160 },
		{ 6750, 159, 162 },
		{ 6750, 167, 164 },
		{ 6750, 161, 167 },
		{ 6750, 163, 163 },
		{ 6750, 156, 156 },
		{ 6750, 153, 162 },
		{ 6750, 156, 169 },
		{ 6750, 171, 167 },
		{ 6750, 165, 164 },
		{ 6750, 165, 159 },
		{ 6750, 168, 161 },
		{ 6750, 171, 161 }
	};

	int FireMonsterFloor1[15][3] =
	{
		{ 6851, 164, 93 },
		{ 6851, 167, 91 },
		{ 6851, 163, 84 },
		{ 6851, 159, 89 },
		{ 6851, 155, 89 },
		{ 6851, 161, 95 },
		{ 6851, 159, 98 },
		{ 6851, 161, 98 },
		{ 6851, 166, 98 },
		{ 6851, 167, 98 },
		{ 6851, 169, 94 },
		{ 6851, 165, 93 },
		{ 6851, 168, 94 },
		{ 6851, 153, 93 },
		{ 6851, 159, 92 }
	};

	int IceMonsterFloor1[15][3] =
	{
		{ 6852, 96, 88 },
		{ 6852, 96, 93 },
		{ 6852, 89, 92 },
		{ 6852, 90, 88 },
		{ 6852, 88, 92 },
		{ 6852, 89, 97 },
		{ 6852, 93, 96 },
		{ 6852, 90, 95 },
		{ 6852, 95, 95 },
		{ 6852, 97, 98 },
		{ 6852, 93, 101 },
		{ 6852, 100, 93 },
		{ 6852, 97, 88 },
		{ 6852, 92, 91 },
		{ 6852, 94, 91 }
	};

	int WindMonsterFloor1[15][3] =
	{
		{ 6852, 96, 159 },
		{ 6852, 96, 156 },
		{ 6852, 89, 159 },
		{ 6852, 90, 164 },
		{ 6852, 88, 161 },
		{ 6852, 89, 161 },
		{ 6852, 93, 166 },
		{ 6852, 90, 161 },
		{ 6852, 95, 167 },
		{ 6852, 97, 164 },
		{ 6852, 93, 163 },
		{ 6852, 100, 162 },
		{ 6852, 97, 158 },
		{ 6852, 92, 159 },
		{ 6852, 94, 156 }
	};

	int iSerpentStoneCoord[6][2] =
	{
		{ 674, 93 },
		{ 674, 127 },
		{ 674, 162 },
		{ 605, 93 },
		{ 605, 127 },
		{ 605, 162 }
	};

	int iSortStoneCoord[6][4] =
	{
		{ 0, 1, 2, 3 },
		{ 1, 2, 3, 4 },
		{ 2, 3, 4, 5 },
		{ 3, 4, 5, 0 },
		{ 4, 5, 0, 1 },
		{ 5, 0, 1, 2 }
	};

	int iSerpentFinalBoss[28][3] =
	{
		{ 6850, 148, 618 },
		{ 6850, 133, 609 },
		{ 6851, 130, 617 },
		{ 6851, 110, 612 },
		{ 6852, 107, 619 },
		{ 6852, 97, 629 },
		{ 6853, 101, 638 },
		{ 6853, 94, 649 },
		{ 6854, 105, 656 },
		{ 6854, 102, 659 },
		{ 6850, 129, 664 },
		{ 6850, 145, 654 },
		{ 6851, 160, 654 },
		{ 6851, 155, 628 },
		{ 6852, 151, 638 },
		{ 6852, 165, 639 },
		{ 6853, 159, 631 },
		{ 6853, 169, 638 },
		{ 6854, 161, 624 },
		{ 6854, 171, 629 },
		{ 6850, 167, 649 },
		{ 6850, 104, 643 },
		{ 6851, 97, 646 },
		{ 6851, 92, 636 },
		{ 6852, 90, 623 },
		{ 6852, 97, 621 },
		{ 6853, 120, 609 }
	};

	//Estructura recolectora de miembros de grupo.
	struct FPartyCHCollector
	{
		std::vector <uint32_t> pPlayerID;
		FPartyCHCollector()
		{
		}
		void operator () (LPCHARACTER ch)
		{
			if (ch)
				pPlayerID.emplace_back(ch->GetPlayerID());
		}
	};

	struct FExitAndGoTo
	{
		FExitAndGoTo() {};
		void operator()(LPENTITY ent)
		{
			if (ent && ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pkChar = dynamic_cast<LPCHARACTER>(ent);
				if (pkChar && pkChar->IsPC())
				{
					const PIXEL_POSITION posSub = CSnk::Instance().GetSubXYZ();
					if (!posSub.x)
						pkChar->WarpSet(EMPIRE_START_X(pkChar->GetEmpire()), EMPIRE_START_Y(pkChar->GetEmpire()));
					else
						pkChar->WarpSet(posSub.x, posSub.y);
				}
			}
		}
	};

	struct FSkillQueenNethis
	{
		FSkillQueenNethis() {};
		void operator()(LPENTITY ent)
		{
			if (ent && ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pkChar = dynamic_cast<LPCHARACTER>(ent);
				if (pkChar && pkChar->IsPC())
				{
					if (!pkChar->IsDead())
					{
						/*
						PIXEL_POSITION mPos;
						mPos.x = pkChar->GetX();
						mPos.y = pkChar->GetY();
						*/
						pkChar->EffectPacket(SE_EFFECT_SNAKE_REGEN);
						pkChar->ComputeSnakeSkill(273, pkChar, 1);
					}
				}
			}
		}
	};

	struct FWarpMap
	{
		FWarpMap(int x, int y, long lMapIndex) : m_x(x), m_y(y), m_lMapIndex(lMapIndex) {};
		void operator()(LPENTITY ent)
		{
			if (ent && ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pkChar = dynamic_cast<LPCHARACTER>(ent);
				if (pkChar && pkChar->IsPC())
				{
					const PIXEL_POSITION pos = CSnk::Instance().GetXYZ();
					if (!pos.x)
						return;
					else
						pkChar->WarpSet(pos.x + m_x, pos.y + m_y, m_lMapIndex);
				}
			}
		}

		int m_x, m_y;
		long m_lMapIndex;
	};

	// DUNGEON_KILL_ALL_BUG_FIX
	struct FKillSectree
	{
		void operator () (LPENTITY ent)
		{
			if (ent && ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = dynamic_cast<LPCHARACTER>(ent);
				if (!ch)
					return;

				if (!ch->IsPC() && !ch->IsPet()
#ifdef ENABLE_GROWTH_PET_SYSTEM
					&& !ch->IsGrowthPet()
#endif
					)
				{
					ch->Dead();
				}
			}
		}
	};
	// END_OF_DUNGEON_KILL_ALL_BUG_FIX
	
	struct FPurgeSectree
	{
		void operator () (LPENTITY ent)
		{
			if (!ent)
				return;

			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = dynamic_cast<LPCHARACTER>(ent);
				if (!ch)
					return;

				if (!ch->IsPC() && !ch->IsDead() && !ch->IsPet()
#ifdef ENABLE_GROWTH_PET_SYSTEM
					&& !ch->IsGrowthPet()
#endif
					)
				{
					M2_DESTROY_CHARACTER(ch);
				}
			}
			else if (ent->IsType(ENTITY_ITEM))
			{
				LPITEM item = dynamic_cast<LPITEM>(ent);
				if (!item)
					return;
				M2_DESTROY_ITEM(item);
			}
			else
				sys_err("SnakeLair::unknown entity type %d is in snake", ent->GetType());
		}
	};

	EVENTINFO(r_snakespawn_info)
	{
		CSnkMap* pMap;
		uint8_t bStep;
		uint8_t bSubStep;
	};

	EVENTFUNC(r_snakespawn_event)
	{
		r_snakespawn_info* pEventInfo = dynamic_cast<r_snakespawn_info*>(event->info);
		if (!pEventInfo)
			return 0;

		CSnkMap* pMap = pEventInfo->pMap;
		if (!pMap)
			return 0;

		if (!pMap->GetMapSectree())
			return 0;

		char szNotice[512];
		// snprintf(szNotice, sizeof(szNotice), "Set: Step %d substep %d", pEventInfo->bStep, pEventInfo->bSubStep);
		// SendNoticeMap(szNotice, pMap->GetMapIndex(), true);

		if (pEventInfo->bStep == 2)
		{
			if (pEventInfo->bSubStep == 1)
			{
				if (pMap->GetElementSort() == 0)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(FireMonsterFloor1[b][0]), FireMonsterFloor1[b][1], FireMonsterFloor1[b][2], 0);
				}
				else if (pMap->GetElementSort() == 1)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(IceMonsterFloor1[b][0]), IceMonsterFloor1[b][1], IceMonsterFloor1[b][2], 0);
				}
				else if (pMap->GetElementSort() == 2)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(EarthMonsterFloor1[b][0]), EarthMonsterFloor1[b][1], EarthMonsterFloor1[b][2], 0);
				}
				else if (pMap->GetElementSort() == 3)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(WindMonsterFloor1[b][0]), WindMonsterFloor1[b][1], WindMonsterFloor1[b][2], 0);
				}

				pEventInfo->bSubStep = 2;
				return PASSES_PER_SEC(20);
			}
			else if (pEventInfo->bSubStep == 2)
			{
				if (pMap->GetElementSort() == 0)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(IceMonsterFloor1[b][0]), IceMonsterFloor1[b][1], IceMonsterFloor1[b][2], 0);
				}
				else if (pMap->GetElementSort() == 1)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(EarthMonsterFloor1[b][0]), EarthMonsterFloor1[b][1], EarthMonsterFloor1[b][2], 0);
				}
				else if (pMap->GetElementSort() == 2)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(FireMonsterFloor1[b][0]), FireMonsterFloor1[b][1], FireMonsterFloor1[b][2], 0);
				}
				else if (pMap->GetElementSort() == 3)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(EarthMonsterFloor1[b][0]), EarthMonsterFloor1[b][1], EarthMonsterFloor1[b][2], 0);
				}

				pEventInfo->bSubStep = 3;
				return PASSES_PER_SEC(20);

			}
			else if (pEventInfo->bSubStep == 3)
			{
				if (pMap->GetElementSort() == 0)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(WindMonsterFloor1[b][0]), WindMonsterFloor1[b][1], WindMonsterFloor1[b][2], 0);
				}
				else if (pMap->GetElementSort() == 1)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(FireMonsterFloor1[b][0]), FireMonsterFloor1[b][1], FireMonsterFloor1[b][2], 0);
				}
				if (pMap->GetElementSort() == 2)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(WindMonsterFloor1[b][0]), WindMonsterFloor1[b][1], WindMonsterFloor1[b][2], 0);
				}
				if (pMap->GetElementSort() == 3)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(IceMonsterFloor1[b][0]), IceMonsterFloor1[b][1], IceMonsterFloor1[b][2], 0);
				}

				pEventInfo->bSubStep = 4;
				return PASSES_PER_SEC(20);
			}
			if (pEventInfo->bSubStep == 4)
			{
				if (pMap->GetElementSort() == 0)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(EarthMonsterFloor1[b][0]), EarthMonsterFloor1[b][1], EarthMonsterFloor1[b][2], 0);
				}
				else if (pMap->GetElementSort() == 1)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(WindMonsterFloor1[b][0]), WindMonsterFloor1[b][1], WindMonsterFloor1[b][2], 0);
				}
				else if (pMap->GetElementSort() == 2)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(IceMonsterFloor1[b][0]), IceMonsterFloor1[b][1], IceMonsterFloor1[b][2], 0);
				}
				if (pMap->GetElementSort() == 3)
				{
					for (uint8_t b = 0; b < 15; b++)
						pMap->Spawn(static_cast<uint32_t>(FireMonsterFloor1[b][0]), FireMonsterFloor1[b][1], FireMonsterFloor1[b][2], 0);
				}

				pEventInfo->bSubStep = 5;
				return PASSES_PER_SEC(20);
			}
			if (pEventInfo->bSubStep == 5)
			{
				for (uint8_t b = 0; b < 5; b++)
					pMap->Spawn(static_cast<uint32_t>(EarthMonsterFloor1[b][0]), EarthMonsterFloor1[b][1], EarthMonsterFloor1[b][2], 0);
				for (uint8_t b = 0; b < 5; b++)
					pMap->Spawn(static_cast<uint32_t>(WindMonsterFloor1[b][0]), WindMonsterFloor1[b][1], WindMonsterFloor1[b][2], 0);
				for (uint8_t b = 0; b < 5; b++)
					pMap->Spawn(static_cast<uint32_t>(IceMonsterFloor1[b][0]), IceMonsterFloor1[b][1], IceMonsterFloor1[b][2], 0);
				for (uint8_t b = 0; b < 5; b++)
					pMap->Spawn(static_cast<uint32_t>(FireMonsterFloor1[b][0]), FireMonsterFloor1[b][1], FireMonsterFloor1[b][2], 0);

				return 0;
			}
		}

		if (pEventInfo->bStep == 3)
		{
			if (pEventInfo->bSubStep == 1)
			{
				FWarpMap f((254 * 100), (60 * 100), pMap->GetMapIndex());
				pMap->GetMapSectree()->for_each(f);

				SendNoticeMap("Destroy the Metin Stones in the correct order.", pMap->GetMapIndex(), true);
			}

			pMap->Spawn(8630, 418, 162, 8);
			pMap->Spawn(8631, 349, 162, 8);
			pMap->Spawn(8632, 349, 93, 8);
			pMap->Spawn(8633, 418, 93, 8);

			return 0;
		}
		if (pEventInfo->bStep == 4)
		{
			if (pEventInfo->bSubStep == 1)
			{
				FWarpMap f((510 * 100), (60 * 100), pMap->GetMapIndex());
				pMap->GetMapSectree()->for_each(f);

				pEventInfo->bSubStep = 2;

				SendNoticeMap("[LS;1741]", pMap->GetMapIndex(), true);

				return PASSES_PER_SEC(5);
			}
			if (pEventInfo->bSubStep == 2)
			{
				pMap->Spawn(SNAKEMETIN_TIME, iSerpentStoneCoord[iSortStoneCoord[0][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[0][pMap->GetSubElementSort()]][1], 0);
				pEventInfo->bSubStep = 3;
				return PASSES_PER_SEC(5);
			}

			if (pEventInfo->bSubStep == 3)
			{
				pMap->Spawn(SNAKEMETIN_TIME, iSerpentStoneCoord[iSortStoneCoord[1][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[1][pMap->GetSubElementSort()]][1], 0);
				pEventInfo->bSubStep = 4;
				return PASSES_PER_SEC(5);
			}
			if (pEventInfo->bSubStep == 4)
			{
				pMap->Spawn(SNAKEMETIN_TIME, iSerpentStoneCoord[iSortStoneCoord[2][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[2][pMap->GetSubElementSort()]][1], 0);
				pEventInfo->bSubStep = 5;
				return PASSES_PER_SEC(5);
			}
			if (pEventInfo->bSubStep == 5)
			{
				pMap->Spawn(SNAKEMETIN_TIME, iSerpentStoneCoord[iSortStoneCoord[3][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[3][pMap->GetSubElementSort()]][1], 0);
				pEventInfo->bSubStep = 6;
				return PASSES_PER_SEC(5);
			}
			if (pEventInfo->bSubStep == 6)
			{
				pMap->Spawn(SNAKEMETIN_TIME, iSerpentStoneCoord[iSortStoneCoord[4][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[4][pMap->GetSubElementSort()]][1], 0);
				pEventInfo->bSubStep = 7;
				return PASSES_PER_SEC(5);
			}
			if (pEventInfo->bSubStep == 7)
			{
				pMap->Spawn(SNAKEMETIN_TIME, iSerpentStoneCoord[iSortStoneCoord[5][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[5][pMap->GetSubElementSort()]][1], 0);
				//pEventInfo->bSubStep = 8;
				return 0;
			}
			if (pEventInfo->bSubStep == 8)
			{
				for (uint8_t b = 0; b < 15; b++)
				{
					pMap->Spawn(static_cast<uint32_t>(FireMonsterFloor1[b][0]), FireMonsterFloor1[b][1] + 511, FireMonsterFloor1[b][2], 0);
					pMap->Spawn(static_cast<uint32_t>(IceMonsterFloor1[b][0]), IceMonsterFloor1[b][1] + 511, IceMonsterFloor1[b][2], 0);
					pMap->Spawn(static_cast<uint32_t>(EarthMonsterFloor1[b][0]), EarthMonsterFloor1[b][1] + 511, EarthMonsterFloor1[b][2], 0);
					pMap->Spawn(static_cast<uint32_t>(WindMonsterFloor1[b][0]), WindMonsterFloor1[b][1] + 511, WindMonsterFloor1[b][2], 0);
				}

				for (uint8_t x = 0; x < 5; x++)
				{
					pMap->Spawn(static_cast<uint32_t>(EarthMonsterFloor1[x][0]), EarthMonsterFloor1[x][1] + 511, EarthMonsterFloor1[x][2], 0);
					pMap->Spawn(static_cast<uint32_t>(WindMonsterFloor1[x][0]), WindMonsterFloor1[x][1] + 511, WindMonsterFloor1[x][2], 0);
					pMap->Spawn(static_cast<uint32_t>(IceMonsterFloor1[x][0]), IceMonsterFloor1[x][1] + 511, IceMonsterFloor1[x][2], 0);
					pMap->Spawn(static_cast<uint32_t>(FireMonsterFloor1[x][0]), FireMonsterFloor1[x][1] + 511, FireMonsterFloor1[x][2], 0);
				}

				return 0;
			}
			if (pEventInfo->bSubStep == 9)
			{
				pMap->Spawn(SIREN_EARTH, 639, 147, 1);
				return 0;
			}
			if (pEventInfo->bSubStep == 10)
			{
				pMap->Spawn(SIREN_FIRE, 639, 147, 1);
				return 0;
			}
			if (pEventInfo->bSubStep == 11)
			{
				const auto spawnSirens = number(0, 4);
				if (spawnSirens == 0)
				{
					pMap->SetDungeonStep(5);
					return 0;
				}
				else
				{
					pMap->SetKillCountMonsters(spawnSirens);
					switch (spawnSirens)
					{
					case 1:
						pMap->Spawn(SIREN_ICE, 605, 92, 1);
						break;
					case 2:
						pMap->Spawn(SIREN_ICE, 675, 92, 1);
						pMap->Spawn(SIREN_ICE, 675, 92, 1);
						break;
					case 3:
						pMap->Spawn(SIREN_ICE, 675, 162, 1);
						pMap->Spawn(SIREN_ICE, 675, 162, 1);
						pMap->Spawn(SIREN_ICE, 675, 162, 1);
						break;
					case 4:
						pMap->Spawn(SIREN_ICE, 605, 162, 1);
						pMap->Spawn(SIREN_ICE, 605, 162, 1);
						pMap->Spawn(SIREN_ICE, 605, 162, 1);
						pMap->Spawn(SIREN_ICE, 605, 162, 1);
						break;
					default:
						break;
					}

					return 0;
				}
			}
			return 0;
		}
		if (pEventInfo->bStep == 5)
		{
			if (pEventInfo->bSubStep == 1)
			{
				FWarpMap f((507 * 100), (310 * 100), pMap->GetMapIndex());
				pMap->GetMapSectree()->for_each(f);

				pEventInfo->bSubStep = 2;

				SendNoticeMap("[LS;1742]", pMap->GetMapIndex(), true);
				pMap->SetSnakePilar1(pMap->Spawn(static_cast<uint32_t>(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[0][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[0][pMap->GetSubElementSort()]][1] + 255, 0));
				return PASSES_PER_SEC(1);
			}
			if (pEventInfo->bSubStep == 2)
			{
				pMap->SetSnakePilar2(pMap->Spawn(static_cast<uint32_t>(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[1][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[1][pMap->GetSubElementSort()]][1] + 255, 0));
				pEventInfo->bSubStep = 3;
				return PASSES_PER_SEC(1);
			}
			if (pEventInfo->bSubStep == 3)
			{
				pMap->SetSnakePilar3(pMap->Spawn(static_cast<uint32_t>(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[2][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[2][pMap->GetSubElementSort()]][1] + 255, 0));
				pEventInfo->bSubStep = 4;
				return PASSES_PER_SEC(1);
			}
			if (pEventInfo->bSubStep == 4)
			{
				pMap->SetSnakePilar4(pMap->Spawn(static_cast<uint32_t>(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[3][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[3][pMap->GetSubElementSort()]][1] + 255, 0));
				pEventInfo->bSubStep = 5;
				return PASSES_PER_SEC(1);
			}
			if (pEventInfo->bSubStep == 5)
			{
				pMap->SetSnakePilar5(pMap->Spawn(static_cast<uint32_t>(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[4][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[4][pMap->GetSubElementSort()]][1] + 255, 0));
				pEventInfo->bSubStep = 6;
				return PASSES_PER_SEC(1);
			}
			if (pEventInfo->bSubStep == 6)
			{
				pMap->SetSnakePilar6(pMap->Spawn(static_cast<uint32_t>(PILAR_STEP_4), iSerpentStoneCoord[iSortStoneCoord[5][pMap->GetSubElementSort()]][0], iSerpentStoneCoord[iSortStoneCoord[5][pMap->GetSubElementSort()]][1] + 255, 0));
				pEventInfo->bSubStep = 7;
			}
			if (pEventInfo->bSubStep == 7)
			{
				if (SECTREE_MANAGER::Instance().GetMonsterCountInMap(pMap->GetMapIndex()) < 4)
				{
					for (uint8_t bCount = 0; bCount < 2; bCount++)
					{
						pMap->Spawn(6750, 642, 435, 8);
						pMap->Spawn(6750, 629, 429, 8);
						pMap->Spawn(6751, 635, 420, 8);
						pMap->Spawn(6751, 638, 420, 8);
						pMap->Spawn(6751, 635, 435, 8);
						pMap->Spawn(6750, 629, 435, 8);
						pMap->Spawn(6751, 642, 425, 8);
						pMap->Spawn(6750, 635, 435, 8);
						pMap->Spawn(6751, 642, 320, 8);
						pMap->Spawn(6751, 629, 333, 8);
						pMap->Spawn(6750, 635, 342, 8);
						pMap->Spawn(6751, 638, 346, 8);
						pMap->Spawn(6750, 635, 325, 8);
						pMap->Spawn(6751, 629, 336, 8);
						pMap->Spawn(6750, 642, 326, 8);
						pMap->Spawn(6751, 635, 340, 8);
					}
				}
			}
		}
		if (pEventInfo->bStep == 6)
		{
			if (pEventInfo->bSubStep == 1)
			{
				FWarpMap f((253 * 100), (188 * 100), pMap->GetMapIndex());
				pMap->GetMapSectree()->for_each(f);

				pMap->SetSnakeBlackSmith1(pMap->Spawn(static_cast<uint32_t>(BLACKSMITH_5), 418, 348, 8));
				pMap->SetSnakeBlackSmith2(pMap->Spawn(static_cast<uint32_t>(BLACKSMITH_5), 418, 418, 8));
				pMap->SetSnakeBlackSmith3(pMap->Spawn(static_cast<uint32_t>(BLACKSMITH_5), 348, 418, 8));
				pMap->SetSnakeBlackSmith4(pMap->Spawn(static_cast<uint32_t>(BLACKSMITH_5), 348, 348, 8));

				SendNoticeMap("[LS;1741]", pMap->GetMapIndex(), true);
				pEventInfo->bSubStep = 2;
			}
			if (pEventInfo->bSubStep == 2)
			{
				if (SECTREE_MANAGER::Instance().GetMonsterCountInMap(pMap->GetMapIndex()) < 4)
				{
					pMap->Spawn(6750, 353, 388, 8);
					pMap->Spawn(6750, 351, 387, 8);
					pMap->Spawn(6750, 349, 381, 8);
					pMap->Spawn(6751, 354, 378, 8);
					pMap->Spawn(6750, 348, 378, 8);
					pMap->Spawn(6750, 347, 375, 8);
					pMap->Spawn(6750, 358, 388, 8);
					pMap->Spawn(6750, 354, 393, 8);
					pMap->Spawn(6751, 344, 387, 8);
					pMap->Spawn(6750, 338, 375, 8);

					pMap->Spawn(6750, 409, 379, 8);
					pMap->Spawn(6750, 418, 379, 8);
					pMap->Spawn(6751, 421, 384, 8);
					pMap->Spawn(6751, 413, 386, 8);
					pMap->Spawn(6750, 406, 388, 8);
					pMap->Spawn(6750, 406, 378, 8);
					pMap->Spawn(6750, 414, 390, 8);
					pMap->Spawn(6750, 423, 393, 8);
					pMap->Spawn(6751, 431, 387, 8);
					pMap->Spawn(6750, 425, 403, 8);
				}

				SendNoticeMap("You have killed all the monsters and can now proceed to the boss level.", pMap->GetMapIndex(), true);
			}
			if (pEventInfo->bSubStep == 3)
			{
				pMap->SetBlackSmithSerpent(pMap->Spawn(static_cast<uint32_t>(VNUM_BLACKSMITH_SERPENT), 383, 438, 8));
				return 0;
			}
		}

		if (pEventInfo->bStep == 7)
		{
			if (pEventInfo->bSubStep == 1)
			{
				FWarpMap f((-200), (560 * 100), pMap->GetMapIndex());
				pMap->GetMapSectree()->for_each(f);

				if (pMap->GetElementSort() == 0)
				{
					pMap->SetSnakeStatue1(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE1), 152, 664, 1));
					pMap->SetSnakeStatue2(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE2), 102, 664, 1));
					pMap->SetSnakeStatue3(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE3), 153, 614, 1));
					pMap->SetSnakeStatue4(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE4), 102, 614, 1));
				}
				else if (pMap->GetElementSort() == 1)
				{
					pMap->SetSnakeStatue1(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE2), 152, 664, 1));
					pMap->SetSnakeStatue2(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE4), 102, 664, 1));
					pMap->SetSnakeStatue3(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE1), 153, 614, 1));
					pMap->SetSnakeStatue4(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE3), 102, 614, 1));
				}
				else if (pMap->GetElementSort() == 2)
				{
					pMap->SetSnakeStatue1(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE4), 152, 664, 1));
					pMap->SetSnakeStatue2(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE1), 102, 664, 1));
					pMap->SetSnakeStatue3(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE3), 153, 614, 1));
					pMap->SetSnakeStatue4(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE2), 102, 614, 1));
				}
				else if (pMap->GetElementSort() == 3)
				{
					pMap->SetSnakeStatue1(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE3), 152, 664, 1));
					pMap->SetSnakeStatue2(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE4), 102, 664, 1));
					pMap->SetSnakeStatue3(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE2), 153, 614, 1));
					pMap->SetSnakeStatue4(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE1), 102, 614, 1));
				}
				else
				{
					pMap->SetSnakeStatue1(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE1), 152, 664, 1));
					pMap->SetSnakeStatue2(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE2), 102, 664, 1));
					pMap->SetSnakeStatue3(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE3), 153, 614, 1));
					pMap->SetSnakeStatue4(pMap->Spawn(static_cast<uint32_t>(SNAKE_STATUE4), 102, 614, 1));
				}

				pMap->SetRotationStatuesCount(0);
				pEventInfo->bSubStep = 2;
			}

			if (pEventInfo->bSubStep == 2)
			{
				if (SECTREE_MANAGER::Instance().GetMonsterCountInMap(pMap->GetMapIndex()) < 6)
				{
					for (uint8_t b = 0; b < 28; b++)
					{
						pMap->Spawn(iSerpentFinalBoss[b][0], iSerpentFinalBoss[b][1], iSerpentFinalBoss[b][2], 1);
					}
				}
			}

			if (pEventInfo->bSubStep == 3)
			{
				FKillSectree f;
				pMap->GetMapSectree()->for_each(f);

				pMap->Spawn(SERPENT_QEEN, 128, 640, 1);
				return 0;
			}

			if (pEventInfo->bSubStep == 4)
			{
				pMap->EndDungeonWarp();
				return 0;
			}
		}
		return PASSES_PER_SEC(5);
	}

	EVENTINFO(r_snakelimit_info)
	{
		CSnkMap* pMap;
	};

	EVENTFUNC(r_snakelimit_event)
	{
		r_snakelimit_info* pEventInfo = dynamic_cast<r_snakelimit_info*>(event->info);
		if (pEventInfo)
		{
			CSnkMap* pMap = pEventInfo->pMap;
			if (pMap)
			{
				pMap->EndDungeonWarp();
			}
		}

		return 0;
	}

	EVENTINFO(r_snakeskill_info)
	{
		CSnkMap* pMap;
	};

	EVENTFUNC(r_snakeskill_event)
	{
		r_snakeskill_info* pEventInfo = dynamic_cast<r_snakeskill_info*>(event->info);
		if (pEventInfo)
		{
			CSnkMap* pMap = pEventInfo->pMap;
			if (pMap)
			{
				FSkillQueenNethis f;
				pMap->GetMapSectree()->for_each(f);
			}
		}

		return PASSES_PER_SEC(25);
	}

	/* Object Part */
	CSnkMap::CSnkMap(long lMapIndex)
	{
		if (e_SpawnEvent != nullptr)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = nullptr;

		if (e_pEndEvent != nullptr)
			event_cancel(&e_pEndEvent);
		e_pEndEvent = nullptr;

		if (e_pSkillEvent != nullptr)
			event_cancel(&e_pSkillEvent);
		e_pSkillEvent = nullptr;

#ifdef ENABLE_YOHARA_SYSTEM
		m_vec_mapSungmaQueenPoint.clear();
		LoadSungmaAttr();
#endif
		SetDungeonStep(1);
		SetMapIndex(lMapIndex);
		SetPortalNPC(nullptr);

		SetSnakeStatue1(nullptr);
		SetSnakeStatue2(nullptr);
		SetSnakeStatue3(nullptr);
		SetSnakeStatue4(nullptr);

		SetMapSectree(SECTREE_MANAGER::Instance().GetMap(map_index));
		SetElementSort(0);
		SetSubElementSort(0);
		SetKillCountPilars(0);
		SetKillCountStones(0);
		pPilar1 = nullptr, pPilar2 = nullptr, pPilar3 = nullptr, pPilar4 = nullptr; //Pilar 4th room.
		pkBlackSmithSerpent = nullptr;

		SetBlockSnake1(false);
		SetBlockSnake2(false);
		SetBlockSnake3(false);
		SetBlockSnake4(false);

		Start();
	}

	CSnkMap::~CSnkMap()
	{
		if (e_SpawnEvent != nullptr)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = nullptr;

		if (e_pEndEvent != nullptr)
			event_cancel(&e_pEndEvent);
		e_pEndEvent = nullptr;

		if (e_pSkillEvent != nullptr)
			event_cancel(&e_pSkillEvent);
		e_pSkillEvent = nullptr;
	}

	void CSnkMap::Destroy()
	{
		if (e_SpawnEvent != nullptr)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = nullptr;

		if (e_pEndEvent != nullptr)
			event_cancel(&e_pEndEvent);
		e_pEndEvent = nullptr;

		if (e_pSkillEvent != nullptr)
			event_cancel(&e_pSkillEvent);
		e_pSkillEvent = nullptr;

#ifdef ENABLE_YOHARA_SYSTEM
		m_vec_mapSungmaQueenPoint.clear();
#endif
		SetDungeonStep(1);
		SetMapIndex(0);

		SetPortalNPC(nullptr);
		SetParty(nullptr);

		SetSnakeStatue1(nullptr);
		SetSnakeStatue2(nullptr);
		SetSnakeStatue3(nullptr);
		SetSnakeStatue4(nullptr);

		SetMapSectree(nullptr);
		SetElementSort(0);
		SetSubElementSort(0);
		SetKillCountPilars(0);
		SetKillCountStones(0);
		pPilar1 = nullptr, pPilar2 = nullptr, pPilar3 = nullptr, pPilar4 = nullptr; //Pilar 4th room.
		pkBlackSmithSerpent = nullptr;
	}

	void CSnkMap::StartDungeon(LPCHARACTER pkChar)
	{
		if (!pkChar)
			return;

		LPPARTY pParty = pkChar->GetParty();

		if (!pParty)
			return;

		SetParty(pParty);

		if (!GetParty())
			return;

		if (GetParty()->GetLeaderPID() != pkChar->GetPlayerID())
		{
			pkChar->ChatPacket(CHAT_TYPE_INFO, "[LS;230]");
			return;
		}

		char szNotice[512];
		snprintf(szNotice, sizeof(szNotice), "Bu artık %d dakika boyunca kullanılabilir.", uint32_t(TIME_LIMIT_DUNGEON) / 60);
		SendNoticeMap(szNotice, pkChar->GetMapIndex(), true);

		pkChar->SetQuestNPCID(0);

		if (GetPortalNPC())
			GetPortalNPC()->Dead();
		SetPortalNPC(nullptr);

		const int iElementSort = number(0, 3);
		SetElementSort(iElementSort);
		SetSubElementSort(iElementSort);
		SetDungeonStep(2);
	}

	void CSnkMap::SetDungeonStep(uint8_t bStep)
	{
		dungeon_step = bStep;

		if (e_SpawnEvent != nullptr)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = nullptr;

		if (bStep == 2)
		{
			SendNoticeMap("Kill all the monsters.", GetMapIndex(), true);

			SetKillCountMonsters(0);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));

			if (e_pSkillEvent != nullptr)
				event_cancel(&e_pSkillEvent);
			e_pSkillEvent = nullptr;

			r_snakeskill_info* pEventInfo_ = AllocEventInfo<r_snakeskill_info>();
			pEventInfo_->pMap = this;
			e_pSkillEvent = event_create(r_snakeskill_event, pEventInfo_, PASSES_PER_SEC(2));
		}
		if (bStep == 3)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), "You have killed all the monsters. You will be transported to level %d. %d seconds remaining until then.", 3, 5);
			SendNoticeMap(szNotice, GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));
		}
		if (bStep == 4)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), "You have killed all the monsters. You will be transported to level %d. %d seconds remaining until then.", 4, 5);
			SendNoticeMap(szNotice, GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));
		}
		if (bStep == 5)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), "You have killed all the monsters. You will be transported to level %d. %d seconds remaining until then.", 5, 5);
			SendNoticeMap(szNotice, GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));
		}
		if (bStep == 6)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), "You have killed all the monsters. You will be transported to level %d. %d seconds remaining until then.", 6, 5);
			SendNoticeMap(szNotice, GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));
		}
		if (bStep == 7)
		{
			FPurgeSectree f;
			GetMapSectree()->for_each(f);

			SetBlackSmithSerpent(nullptr);

			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), "You have killed all the monsters. You will be transported to level %d. %d seconds remaining until then.", 7, 5);
			SendNoticeMap(szNotice, GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));
		}
	}

	void CSnkMap::Start()
	{
		if (!GetMapSectree())
			EndDungeonWarp();
		else
		{
			SetPortalNPC(Spawn(static_cast<uint32_t>(PORTAL_VNUM), 126, 105, 8));

			if (e_pEndEvent != nullptr)
				event_cancel(&e_pEndEvent);
			e_pEndEvent = nullptr;

			r_snakelimit_info* pEventInfo = AllocEventInfo<r_snakelimit_info>();
			pEventInfo->pMap = this;
			e_pEndEvent = event_create(r_snakelimit_event, pEventInfo, PASSES_PER_SEC(3600));
		}
	}

	void CSnkMap::OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller)
	{
		if (!pkMonster)
			return;

		const uint8_t bStep = dungeon_step;

		if (!bStep)
			return;

		if (!GetMapSectree())
			return;

		if ((bStep == 2) && (pkMonster->GetRaceNum() == 6750 || (pkMonster->GetRaceNum() >= 6850 && pkMonster->GetRaceNum() <= 6854)))
		{
			SetKillCountMonsters(GetKillCountMonsters() + 1);
			if (GetKillCountMonsters() >= 60)
			{
				SetKillCountMonsters(0);
				FPurgeSectree f;
				GetMapSectree()->for_each(f);
				SetDungeonStep(3);
			}
		}
		if ((bStep == 3) && (pkMonster->GetRaceNum() == SNAKEMETIN_FIRE || pkMonster->GetRaceNum() == SNAKEMETIN_ICE || pkMonster->GetRaceNum() == SNAKEMETIN_WIND || pkMonster->GetRaceNum() == SNAKEMETIN_EARTH))
		{
			if (static_cast<uint32_t>(stoneKillSort[GetElementSort()][GetKillCountMonsters()]) == pkMonster->GetRaceNum())
			{
				SetKillCountMonsters(GetKillCountMonsters() + 1);
				if ((4 - GetKillCountMonsters()) != 0)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), "You have destroyed Metin %d. Now destroy Metin %d.", GetKillCountMonsters(), (GetKillCountMonsters() + 1)); // Need to make the maths
					SendNoticeMap(szNotice, GetMapIndex(), true);
				}
			}
			else
			{
				SendNoticeMap("The destroyed Metin isn't reacting. All the Metins have appeared again.", GetMapIndex(), true);

				FPurgeSectree f;
				GetMapSectree()->for_each(f);
				SetKillCountMonsters(0);

				if (e_SpawnEvent != nullptr)
					event_cancel(&e_SpawnEvent);
				e_SpawnEvent = nullptr;

				r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
				pEventInfo->pMap = this;
				pEventInfo->bStep = bStep;
				pEventInfo->bSubStep = 2;
				e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));
			}
			if (GetKillCountMonsters() >= 4)
			{
				SetKillCountMonsters(0);
				SetDungeonStep(4);
			}
		}

		if (bStep == 4)
		{
			if (pkMonster->GetRaceNum() == SNAKEMETIN_TIME)
			{
				SetKillCountStones(GetKillCountStones() + 1);
				if (GetKillCountStones() >= 6)
				{
					SetKillCountMonsters(0);
					SetKillCountStones(0);

					if (e_SpawnEvent != nullptr)
						event_cancel(&e_SpawnEvent);
					e_SpawnEvent = nullptr;

					r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
					pEventInfo->pMap = this;
					pEventInfo->bStep = bStep;
					pEventInfo->bSubStep = 8;
					e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));
				}

				return;
			}
			else if (pkMonster->GetRaceNum() == 6750 || (pkMonster->GetRaceNum() >= 6851 && pkMonster->GetRaceNum() <= 6854))
			{
				SetKillCountMonsters(GetKillCountMonsters() + 1);
				if (GetKillCountMonsters() >= 60)
				{
					SetKillCountMonsters(0);
					SetKillCountStones(0);

					if (e_SpawnEvent != nullptr)
						event_cancel(&e_SpawnEvent);
					e_SpawnEvent = nullptr;

					FPurgeSectree f;
					GetMapSectree()->for_each(f);

					r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
					pEventInfo->pMap = this;
					pEventInfo->bStep = bStep;
					pEventInfo->bSubStep = 9;
					e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));
				}

				return;
			}
			else if (pkMonster->GetRaceNum() == SIREN_EARTH)
			{
				if (e_SpawnEvent != nullptr)
					event_cancel(&e_SpawnEvent);
				e_SpawnEvent = nullptr;

				FPurgeSectree f;
				GetMapSectree()->for_each(f);

				r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
				pEventInfo->pMap = this;
				pEventInfo->bStep = bStep;
				pEventInfo->bSubStep = 10;
				e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));

				return;
			}
			else if (pkMonster->GetRaceNum() == SIREN_FIRE)
			{
				if (e_SpawnEvent != nullptr)
					event_cancel(&e_SpawnEvent);
				e_SpawnEvent = nullptr;

				FPurgeSectree f;
				GetMapSectree()->for_each(f);

				r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
				pEventInfo->pMap = this;
				pEventInfo->bStep = bStep;
				pEventInfo->bSubStep = 11;
				e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));

				return;
			}
			else if (pkMonster->GetRaceNum() == SIREN_ICE)
			{
				const auto remainSirens = GetKillCountMonsters();
				SetKillCountMonsters(GetKillCountMonsters() + 1);
				if (GetKillCountMonsters() >= remainSirens)
				{
					SetKillCountMonsters(0);
					SetKillCountStones(0);

					if (e_SpawnEvent != nullptr)
						event_cancel(&e_SpawnEvent);
					e_SpawnEvent = nullptr;

					SetElementSort(GetElementSort() + remainSirens);

					SetDungeonStep(5);
					return;
				}
			}
		}

		if (bStep == 5)
		{
			if (pkMonster->GetRaceNum() == 6750 || pkMonster->GetRaceNum() == 6751)
			{
				if (!pKiller)
					return;

				const int iChance = number(1, 100);
				if (iChance < 15)
				{
					LPITEM pkItem = ITEM_MANAGER::Instance().CreateItem(VNUM_KILL_PILAR);
					if (!pkItem)
						return;

					PIXEL_POSITION mPos{};
					mPos.x = pkMonster->GetX();
					mPos.y = pkMonster->GetY();

					pkItem->AddToGround(GetMapIndex(), mPos);
					pkItem->StartDestroyEvent();
					pkItem->SetOwnership(pKiller, 60);
				}
			}
		}

		if (bStep == 6)
		{
			if (pkMonster->GetRaceNum() == 6750 || pkMonster->GetRaceNum() == 6750 || pkMonster->GetRaceNum() == 6751)
			{
				if (!pKiller)
					return;

				const int iChance = number(1, 100);
				if (iChance < 15)
				{
					LPITEM pkItem = ITEM_MANAGER::Instance().CreateItem(VNUM_KILL_BLACKSMITH);
					if (!pkItem)
						return;

					PIXEL_POSITION mPos{};
					mPos.x = pkMonster->GetX();
					mPos.y = pkMonster->GetY();

					pkItem->AddToGround(GetMapIndex(), mPos);
					pkItem->StartDestroyEvent();
					pkItem->SetOwnership(pKiller, 60);
				}
			}
		}

		if (bStep == 7)
		{
			if (pkMonster->GetRaceNum() >= 6850 && pkMonster->GetRaceNum() <= 6854)
			{
				if (!pKiller)
					return;

				const int iChance = number(1, 100);
				if (iChance < 10)
				{
					const int iDropItem = number(VNUM_ITEM_SNAKE_FIRE, VNUM_ITEM_SNAKE_GROUND);
					LPITEM pkItem = ITEM_MANAGER::Instance().CreateItem(iDropItem);
					if (!pkItem)
						return;

					PIXEL_POSITION mPos{};
					mPos.x = pkMonster->GetX();
					mPos.y = pkMonster->GetY();

					pkItem->AddToGround(GetMapIndex(), mPos);
					pkItem->StartDestroyEvent();
					pkItem->SetOwnership(pKiller, 60);
				}

				return;
			}
			if (pkMonster->GetRaceNum() == SERPENT_QEEN)
			{
				char szNotice[512];
				snprintf(szNotice, sizeof(szNotice), "You have completed the Serpent Temple. You will be transported out of the dungeon in %d seconds.", 20);
				SendNoticeMap(szNotice, GetMapIndex(), true);

				if (e_SpawnEvent != nullptr)
					event_cancel(&e_SpawnEvent);
				e_SpawnEvent = nullptr;

				FKillSectree f;
				GetMapSectree()->for_each(f);

				if (e_pSkillEvent != nullptr)
					event_cancel(&e_pSkillEvent);
				e_pSkillEvent = nullptr;

				r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
				pEventInfo->pMap = this;
				pEventInfo->bStep = GetDungeonStep();
				pEventInfo->bSubStep = 4;
				e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(20));
				return;
			}
		}
	}

	void CSnkMap::EndDungeonWarp()
	{
		if (GetMapSectree())
		{
			FExitAndGoTo f;
			GetMapSectree()->for_each(f);
		}

		const long lMapIndex = GetMapIndex();
		SECTREE_MANAGER::Instance().DestroyPrivateMap(GetMapIndex());
		Destroy();
		CSnk::Instance().Remove(lMapIndex);
		M2_DELETE(this);
	}

	void CSnkMap::OnKillBlackSmith(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkBlackSmith)
	{
		if ((!pkItem) || (!pkChar) || (!pkBlackSmith))
			return;

		if (!GetMapSectree())
			return;

		if (GetSnakeBLackSmith1())
		{
			if (GetSnakeBLackSmith1()->GetVID() == pkBlackSmith->GetVID())
			{
				GetSnakeBLackSmith1()->Dead();
				SetSnakeBlackSmith1(nullptr);
			}
		}
		if (GetSnakeBLackSmith2())
		{
			if (GetSnakeBLackSmith2()->GetVID() == pkBlackSmith->GetVID())
			{
				GetSnakeBLackSmith2()->Dead();
				SetSnakeBlackSmith2(nullptr);
			}
		}
		if (GetSnakeBLackSmith3())
		{
			if (GetSnakeBLackSmith3()->GetVID() == pkBlackSmith->GetVID())
			{
				GetSnakeBLackSmith3()->Dead();
				SetSnakeBlackSmith3(nullptr);
			}
		}
		if (GetSnakeBLackSmith4())
		{
			if (GetSnakeBLackSmith4()->GetVID() == pkBlackSmith->GetVID())
			{
				GetSnakeBLackSmith4()->Dead();
				SetSnakeBlackSmith4(nullptr);
			}
		}

		ITEM_MANAGER::Instance().RemoveItem(pkItem);

		SetKillCountPilars(GetKillCountPilars() + 1);
		if ((4 - GetKillCountPilars()) != 0)
		{
			SendNoticeMap("The prison has been unlocked. The blacksmith smiles at you.", GetMapIndex(), true);
		}

		if (GetKillCountPilars() >= 4)
		{
			if (e_SpawnEvent != nullptr)
				event_cancel(&e_SpawnEvent);
			e_SpawnEvent = nullptr;

			FPurgeSectree f;
			GetMapSectree()->for_each(f);

			SendNoticeMap("The special Serpent Temple Smith appears!", GetMapIndex(), true);

			r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = GetDungeonStep();
			pEventInfo->bSubStep = 3;
			e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(1));
		}
	}

	void CSnkMap::OnKillPilar(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkPilar)
	{
		if ((!pkItem) || (!pkChar) || (!pkPilar))
			return;

		if (!GetMapSectree())
			return;

		ITEM_MANAGER::Instance().RemoveItem(pkItem);

		if (GetSnakePilar1())
		{
			if (GetSnakePilar1()->GetVID() == pkPilar->GetVID())
			{
				GetSnakePilar1()->Dead();
				SetKillCountPilars(GetKillCountPilars() + 1);
			}
		}
		if (GetSnakePilar2())
		{
			if (GetSnakePilar2()->GetVID() == pkPilar->GetVID())
			{
				if (GetKillCountPilars() < 1 || GetKillCountPilars() > 1)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), "You have to unlock the other pillar first.");
					SendNoticeMap(szNotice, GetMapIndex(), true);
					return;
				}
				GetSnakePilar2()->Dead();
				SetSnakePilar2(nullptr);
				SetKillCountPilars(GetKillCountPilars() + 1);
			}
		}
		if (GetSnakePilar3())
		{
			if (GetSnakePilar3()->GetVID() == pkPilar->GetVID())
			{
				if (GetKillCountPilars() < 2 || GetKillCountPilars() > 2)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), "You have to unlock the other pillar first.");
					SendNoticeMap(szNotice, GetMapIndex(), true);
					return;
				}

				GetSnakePilar3()->Dead();
				SetSnakePilar3(nullptr);
				SetKillCountPilars(GetKillCountPilars() + 1);
			}
		}
		if (GetSnakePilar4())
		{
			if (GetSnakePilar4()->GetVID() == pkPilar->GetVID())
			{
				if (GetKillCountPilars() < 3 || GetKillCountPilars() > 3)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), "You have to unlock the other pillar first.");
					SendNoticeMap(szNotice, GetMapIndex(), true);
					return;
				}

				GetSnakePilar4()->Dead();
				SetSnakePilar4(nullptr);
				SetKillCountPilars(GetKillCountPilars() + 1);
			}
		}
		if (GetSnakePilar5())
		{
			if (GetSnakePilar5()->GetVID() == pkPilar->GetVID())
			{
				if (GetKillCountPilars() < 4 || GetKillCountPilars() > 4)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), "You have to unlock the other pillar first.");
					SendNoticeMap(szNotice, GetMapIndex(), true);
					return;
				}

				GetSnakePilar5()->Dead();
				SetSnakePilar5(nullptr);
				SetKillCountPilars(GetKillCountPilars() + 1);
			}
		}
		if (GetSnakePilar6())
		{
			if (GetSnakePilar6()->GetVID() == pkPilar->GetVID())
			{
				if (GetKillCountPilars() < 5 || GetKillCountPilars() > 5)
				{
					char szNotice[512];
					snprintf(szNotice, sizeof(szNotice), "You have to unlock the other pillar first.");
					SendNoticeMap(szNotice, GetMapIndex(), true);
					return;
				}

				GetSnakePilar6()->Dead();
				SetSnakePilar6(nullptr);
				SetKillCountPilars(GetKillCountPilars() + 1);
			}
		}
		if ((6 - GetKillCountPilars()) != 0)
		{
			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), "Successfully opened!");
			SendNoticeMap(szNotice, GetMapIndex(), true);
		}
		if (GetKillCountPilars() >= 6)
		{
			FPurgeSectree f;
			GetMapSectree()->for_each(f);
			SetKillCountPilars(0);
			SetDungeonStep(6);
		}
	}

	void CSnkMap::OnStatueSetRotation(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkStatue)
	{
		if ((!pkItem) || (!pkChar) || (!pkStatue))
			return;

		if (GetDungeonStep() != 7)
			return;

		if (GetMapSectree())
		{
			PIXEL_POSITION pos = CSnk::Instance().GetXYZ();
			if (!pos.x)
			{
				pkChar->ChatPacket(CHAT_TYPE_INFO, "Hata! Koordinatlar oluşturulurken bir sorun oluştu, yetkililere bildiriniz.");
				return;
			}

			ITEM_MANAGER::Instance().RemoveItem(pkItem);

			if (GetSnakeStatue1())
			{
				if (GetSnakeStatue1()->GetVID() == pkStatue->GetVID())
				{
					if (GetBlockSnake1())
						return;

					if (GetSnakeStatue1()->GetRaceNum() == static_cast<int>(SNAKE_STATUE1))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_FIRE))
							return;
					}
					if (GetSnakeStatue1()->GetRaceNum() == static_cast<int>(SNAKE_STATUE2))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_ICE))
							return;
					}
					if (GetSnakeStatue1()->GetRaceNum() == static_cast<int>(SNAKE_STATUE3))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_WIND))
							return;
					}
					if (GetSnakeStatue1()->GetRaceNum() == static_cast<int>(SNAKE_STATUE4))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_GROUND))
							return;
					}

					pos.x = GetMapSectree()->m_setting.iBaseX + 127 * 100;
					pos.y = GetMapSectree()->m_setting.iBaseY + 640 * 100;
					pos.z = 0;
					SetRotationStatuesCount(GetRotationStatuesCount() + 1);
					GetSnakeStatue1()->SetRotationToXY(pos.x, pos.y);
					GetSnakeStatue1()->SendMovePacket(FUNC_WAIT, 0, GetSnakeStatue1()->GetX(), GetSnakeStatue1()->GetY(), 0, 0);
					SetBlockSnake1(true);
				}
			}

			if (GetSnakeStatue2())
			{
				if (GetSnakeStatue2()->GetVID() == pkStatue->GetVID())
				{
					if (GetBlockSnake2())
						return;

					if (GetSnakeStatue2()->GetRaceNum() == static_cast<int>(SNAKE_STATUE1))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_FIRE))
							return;
					}
					if (GetSnakeStatue2()->GetRaceNum() == static_cast<int>(SNAKE_STATUE2))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_ICE))
							return;
					}
					if (GetSnakeStatue2()->GetRaceNum() == static_cast<int>(SNAKE_STATUE3))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_WIND))
							return;
					}
					if (GetSnakeStatue2()->GetRaceNum() == static_cast<int>(SNAKE_STATUE4))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_GROUND))
							return;
					}

					pos.x = GetMapSectree()->m_setting.iBaseX + 127 * 100;
					pos.y = GetMapSectree()->m_setting.iBaseY + 640 * 100;
					pos.z = 0;
					SetRotationStatuesCount(GetRotationStatuesCount() + 1);
					GetSnakeStatue2()->SetRotationToXY(pos.x, pos.y);
					GetSnakeStatue2()->SendMovePacket(FUNC_WAIT, 0, GetSnakeStatue2()->GetX(), GetSnakeStatue2()->GetY(), 0, 0);
					SetBlockSnake2(true);
				}
			}

			if (GetSnakeStatue3())
			{
				if (GetSnakeStatue3()->GetVID() == pkStatue->GetVID())
				{
					if (GetBlockSnake3())
						return;

					if (GetSnakeStatue3()->GetRaceNum() == static_cast<int>(SNAKE_STATUE1))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_FIRE))
							return;
					}
					if (GetSnakeStatue3()->GetRaceNum() == static_cast<int>(SNAKE_STATUE2))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_ICE))
							return;
					}
					if (GetSnakeStatue3()->GetRaceNum() == static_cast<int>(SNAKE_STATUE3))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_WIND))
							return;
					}
					if (GetSnakeStatue3()->GetRaceNum() == static_cast<int>(SNAKE_STATUE4))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_GROUND))
							return;
					}

					pos.x = GetMapSectree()->m_setting.iBaseX + 127 * 100;
					pos.y = GetMapSectree()->m_setting.iBaseY + 640 * 100;
					pos.z = 0;
					SetRotationStatuesCount(GetRotationStatuesCount() + 1);
					GetSnakeStatue3()->SetRotationToXY(pos.x, pos.y);
					GetSnakeStatue3()->SendMovePacket(FUNC_WAIT, 0, GetSnakeStatue3()->GetX(), GetSnakeStatue3()->GetY(), 0, 0);
					SetBlockSnake3(true);
				}
			}

			if (GetSnakeStatue4())
			{
				if (GetSnakeStatue4()->GetVID() == pkStatue->GetVID())
				{
					if (GetBlockSnake4())
						return;

					if (GetSnakeStatue4()->GetRaceNum() == static_cast<int>(SNAKE_STATUE1))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_FIRE))
							return;
					}
					if (GetSnakeStatue4()->GetRaceNum() == static_cast<int>(SNAKE_STATUE2))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_ICE))
							return;
					}
					if (GetSnakeStatue4()->GetRaceNum() == static_cast<int>(SNAKE_STATUE3))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_WIND))
							return;
					}
					if (GetSnakeStatue4()->GetRaceNum() == static_cast<int>(SNAKE_STATUE4))
					{
						if (pkItem->GetVnum() != static_cast<int>(VNUM_ITEM_SNAKE_GROUND))
							return;
					}

					pos.x = GetMapSectree()->m_setting.iBaseX + 127 * 100;
					pos.y = GetMapSectree()->m_setting.iBaseY + 640 * 100;
					pos.z = 0;
					SetRotationStatuesCount(GetRotationStatuesCount() + 1);
					GetSnakeStatue4()->SetRotationToXY(pos.x, pos.y);
					GetSnakeStatue4()->SendMovePacket(FUNC_WAIT, 0, GetSnakeStatue4()->GetX(), GetSnakeStatue4()->GetY(), 0, 0);
					SetBlockSnake4(true);
				}
			}

			if (GetRotationStatuesCount() >= 4)
			{
				if (GetSnakeStatue1())
					GetSnakeStatue1()->SendMovePacket(FUNC_MOB_SKILL, 0, GetSnakeStatue1()->GetX(), GetSnakeStatue1()->GetY(), 0, 0);
				if (GetSnakeStatue2())
					GetSnakeStatue2()->SendMovePacket(FUNC_MOB_SKILL, 0, GetSnakeStatue2()->GetX(), GetSnakeStatue2()->GetY(), 0, 0);
				if (GetSnakeStatue3())
					GetSnakeStatue3()->SendMovePacket(FUNC_MOB_SKILL, 0, GetSnakeStatue3()->GetX(), GetSnakeStatue3()->GetY(), 0, 0);
				if (GetSnakeStatue4())
					GetSnakeStatue4()->SendMovePacket(FUNC_MOB_SKILL, 0, GetSnakeStatue4()->GetX(), GetSnakeStatue4()->GetY(), 0, 0);

				if (e_SpawnEvent != nullptr)
					event_cancel(&e_SpawnEvent);
				e_SpawnEvent = nullptr;

				r_snakespawn_info* pEventInfo = AllocEventInfo<r_snakespawn_info>();
				pEventInfo->pMap = this;
				pEventInfo->bStep = GetDungeonStep();
				pEventInfo->bSubStep = 3;
				e_SpawnEvent = event_create(r_snakespawn_event, pEventInfo, PASSES_PER_SEC(5));
			}
		}
	}

	LPCHARACTER CSnkMap::Spawn(uint32_t dwVnum, int iX, int iY, int iDir, bool bSpawnMotion)
	{
		if (dwVnum == 0)
			return nullptr;

		if (!GetMapSectree())
			return nullptr;

		LPCHARACTER pkMob = CHARACTER_MANAGER::Instance().SpawnMob(dwVnum, GetMapIndex(), GetMapSectree()->m_setting.iBaseX + iX * 100, GetMapSectree()->m_setting.iBaseY + iY * 100, 0, bSpawnMotion, iDir == 0 ? -1 : (iDir - 1) * 45);
		if (pkMob)
			sys_log(0, "<SnakeLair> Spawn: %s (map index: %d). x: %d y: %d", pkMob->GetName(), GetMapIndex(), (GetMapSectree()->m_setting.iBaseX + iX * 100), (GetMapSectree()->m_setting.iBaseY + iY * 100));

		return pkMob;
	}

#ifdef ENABLE_YOHARA_SYSTEM
	void CSnkMap::LoadSungmaAttr()
	{
		TSungmaQueenPoint aMap{};

		for (uint8_t by = 0; by < 5; by++)
		{
			for (uint8_t bx = 1; bx < 8; bx++)
			{
				aMap.bDungeonFloor = bx;
				aMap.iPointType = iSungmaQueenPoint[by][0];
				aMap.iValue = iSungmaQueenPoint[by][bx];
				m_vec_mapSungmaQueenPoint.emplace_back(aMap);
			}
		}
	}

	int CSnkMap::GetSungmaValuePoint(int iPoint)
	{
		if (!m_vec_mapSungmaQueenPoint.empty())
		{
			std::vector<TSungmaQueenPoint>::iterator it = m_vec_mapSungmaQueenPoint.begin();
			while (it != m_vec_mapSungmaQueenPoint.end())
			{
				const TSungmaQueenPoint& rRegion = *(it++);
				if (rRegion.bDungeonFloor == GetDungeonStep())
				{
					if (rRegion.iPointType == iPoint)
					{
						return rRegion.iValue;
					}
				}
			}
		}

		return 0;
	}
#endif

	/* Global Part */
	void CSnk::Initialize()
	{
		SetXYZ(0, 0, 0);
		SetSubXYZ(0, 0, 0);
		m_dwRegGroups.clear();
	}

	void CSnk::Destroy()
	{
		for (const auto& iter : m_dwRegGroups)
		{
			CSnkMap* pMap = iter.second;
			if (!pMap)
				continue;
			SECTREE_MANAGER::Instance().DestroyPrivateMap(pMap->GetMapIndex());
			pMap->Destroy();
			M2_DELETE(pMap);
		}

		SetXYZ(0, 0, 0);
		SetSubXYZ(0, 0, 0);
		m_dwRegGroups.clear();
	}

	void CSnk::OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller)
	{
		if ((!pkMonster) || (!pKiller))
			return;

		const LPPARTY& pParty = pKiller->GetParty();
		if (!pParty)
			return;

		const long lMapIndex = pKiller->GetMapIndex();
		if (lMapIndex < 1000)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (!pMap)
			return;

		pMap->OnKill(pkMonster, pKiller);
		return;
	}

	void CSnk::OnKillPilar(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkPilar)
	{
		if ((!pkItem) || (!pkChar) || (!pkPilar))
			return;

		const LPPARTY& pParty = pkChar->GetParty();
		if (!pParty)
			return;

		if (pkItem->GetOriginalVnum() != static_cast<uint32_t>(VNUM_KILL_PILAR))
			return;

		const long lMapIndex = pkChar->GetMapIndex();
		if (lMapIndex < 1000)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;

		if (!pMap)
			return;

		pMap->OnKillPilar(pkItem, pkChar, pkPilar);
		return;
	}

	void CSnk::OnKillBlackSmith(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkBlackSmith)
	{
		if ((!pkItem) || (!pkChar) || (!pkBlackSmith))
			return;

		const LPPARTY& pParty = pkChar->GetParty();
		if (!pParty)
			return;

		if (pkItem->GetOriginalVnum() != static_cast<uint32_t>(VNUM_KILL_BLACKSMITH))
			return;

		const long lMapIndex = pkChar->GetMapIndex();
		if (lMapIndex < 1000)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;

		if (!pMap)
			return;

		pMap->OnKillBlackSmith(pkItem, pkChar, pkBlackSmith);
		return;
	}

	void CSnk::OnStatueSetRotation(LPITEM pkItem, LPCHARACTER pkChar, LPCHARACTER pkStatue)
	{
		if ((!pkItem) || (!pkChar) || (!pkStatue))
			return;

		const LPPARTY& pParty = pkChar->GetParty();
		if (!pParty)
			return;

		if (pkItem->GetOriginalVnum() < static_cast<uint32_t>(SNAKE_STATUE1) && pkItem->GetOriginalVnum() > static_cast<uint32_t>(SNAKE_STATUE4))
			return;

		const long lMapIndex = pkChar->GetMapIndex();
		if (lMapIndex < 1000)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;

		if (!pMap)
			return;

		pMap->OnStatueSetRotation(pkItem, pkChar, pkStatue);
		return;
	}

	void CSnk::SetDungeonStep(LPCHARACTER pkChar, uint8_t step)
	{
		if (!pkChar)
			return;

		LPPARTY pParty = pkChar->GetParty();

		if (!pParty)
			return;

		if (pParty->GetLeaderPID() != pkChar->GetPlayerID())
		{
			pkChar->ChatPacket(CHAT_TYPE_INFO, "Parti lideri değilsin.");
			return;
		}

		const long lMapIndex = pkChar->GetMapIndex();
		if (lMapIndex < 1000)
			return;

		const auto iter = m_dwRegGroups.find(lMapIndex), iterEnd = m_dwRegGroups.end();
		if (iter == iterEnd)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (pMap)
		{
			if (pMap->GetParty()->GetLeaderPID() != pkChar->GetPlayerID())
			{
				pkChar->ChatPacket(CHAT_TYPE_INFO, "HATA! - Başka bir grup zaten kayıtlı.");
				return;
			}

			pMap->SetDungeonStep(step);
		}
	}

	bool CSnk::IsSnakeMap(long lMapIndex)
	{
		const long lMinIndex = static_cast<long>(MAP_SNAKE_TEMPLE_02) * 10000, lMaxIndex = static_cast<long>(MAP_SNAKE_TEMPLE_02) * 10000 + 10000;
		if (((lMapIndex >= lMinIndex) && (lMapIndex <= lMaxIndex)) || (lMapIndex == static_cast<long>(MAP_SNAKE_TEMPLE_02)))
			return true;

		return false;
	}

	void CSnk::Remove(long lMapIndex)
	{
		const auto iter = m_dwRegGroups.find(lMapIndex);
		if (iter != m_dwRegGroups.end())
		{
			m_dwRegGroups.erase(iter);
		}

		return;
	}

#ifdef ENABLE_YOHARA_SYSTEM
	int CSnk::GetSungmaQueenDungeonValue(const CHARACTER* pkChar, int iPoint) const
	{
		if (!pkChar)
			return 0;

		if (!pkChar->IsPC())
			return 0;

		const LPPARTY& pParty = pkChar->GetParty();
		if (!pParty)
			return 0;

		const long lMapIndex = pkChar->GetMapIndex();
		if (lMapIndex < 1000)
			return 0;

		const auto iter = m_dwRegGroups.find(lMapIndex), iterEnd = m_dwRegGroups.end();
		if (iter == iterEnd)
			return 0;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;

		if (pMap)
			return pMap->GetSungmaValuePoint(iPoint);

		return 0;
	}
#endif

	void CSnk::QueenDebuffAttack(LPCHARACTER pkAttacker, LPCHARACTER pkVictim)
	{
		if (pkAttacker == nullptr || pkVictim == nullptr)
			return;

		if (!pkAttacker->GetDesc())
			return;

		if (pkVictim->GetRaceNum() == SERPENT_QEEN)
		{
			if (pkAttacker->IsPC())
			{
				const int iChance = number(1, 100);
				if (iChance <= 40)
				{
					#ifdef ENABLE_YOHARA_SYSTEM
					if (!pkAttacker->FindAffect(AFFECT_SUNGMA_MOVE_DEBUFF))
						pkAttacker->AddAffect(AFFECT_SUNGMA_MOVE_DEBUFF, POINT_SUNGMA_MOVE, -20, 0, 8, 0, false);

					if (!pkAttacker->FindAffect(AFFECT_SUNGMA_STR_DEBUFF))
						pkAttacker->AddAffect(AFFECT_SUNGMA_STR_DEBUFF, POINT_SUNGMA_STR, -20, 0, 8, 0, false);
					#endif
				}
			}
		}
	}

	void CSnk::LeaveParty(long lMapIndex)
	{
		Remove(lMapIndex);
		return;
	}

	void CSnk::Leave(LPCHARACTER pChar)
	{
		if (!pChar)
			return;

		const PIXEL_POSITION posSub = GetSubXYZ();
		if (!posSub.x)
			pChar->WarpSet(EMPIRE_START_X(pChar->GetEmpire()), EMPIRE_START_Y(pChar->GetEmpire()));
		else
			pChar->WarpSet(posSub.x, posSub.y);

		return;
	}

	bool CSnk::Access(LPCHARACTER pChar)
	{
		if (pChar)
		{
			LPPARTY pParty = pChar->GetParty();

			if (!pParty)
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, "Herhangi bir gruba dahil değilsin.");
				return false;
			}

			const long lNormalMapIndex = static_cast<long>(MAP_SNAKE_TEMPLE_02);
			const PIXEL_POSITION pos = GetXYZ(), posSub = GetSubXYZ();
			if (!pos.x)
			{
				const LPSECTREE_MAP& pkSectreeMap = SECTREE_MANAGER::Instance().GetMap(static_cast<long>(MAP_SNAKE_TEMPLE_02));
				if (pkSectreeMap)
					SetXYZ(pkSectreeMap->m_setting.iBaseX + 130 * 100, pkSectreeMap->m_setting.iBaseY + 130 * 100, 0);
			}

			if (!posSub.x)
			{
				const LPSECTREE_MAP& pkSectreeMap = SECTREE_MANAGER::Instance().GetMap(static_cast<long>(MAP_SNAKE_TEMPLE_01));
				if (pkSectreeMap)
					SetSubXYZ(pkSectreeMap->m_setting.iBaseX + 428 * 100, pkSectreeMap->m_setting.iBaseY + 364 * 100, 0);
			}

			if (pParty->GetLeaderPID() != pChar->GetPlayerID())
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, "Yeni bir zindan yalnızca parti lideri tarafından oluşturulabilir.");
				return false;
			}

			long lMapIndex = SECTREE_MANAGER::Instance().CreatePrivateMap(lNormalMapIndex);
			if (!lMapIndex)
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, "Harita oluşturulurken bir hata oluştu.");
				return false;
			}

			CSnkMap* pMap = M2_NEW CSnkMap(lMapIndex);
			if (pMap)
			{
				m_dwRegGroups.insert(std::make_pair(lMapIndex, pMap));

				PIXEL_POSITION mPos;
				if (!SECTREE_MANAGER::Instance().GetRecallPositionByEmpire(static_cast<int>(MAP_SNAKE_TEMPLE_02), 0, mPos))
				{
					pChar->ChatPacket(CHAT_TYPE_INFO, "Sectree hatası: geri çağırma konumu alınamadı.");
					return true;
				}

				pMap->SetDungeonStep(1);

				FPartyCHCollector f;
				pChar->GetParty()->ForEachOnMapMember(f, pChar->GetMapIndex());
				std::vector <uint32_t>::iterator it;
				for (it = f.pPlayerID.begin(); it != f.pPlayerID.end(); it++)
				{
					LPCHARACTER pkChr = CHARACTER_MANAGER::Instance().FindByPID(*it);
					if (pkChr)
					{
						pkChr->SaveExitLocation();
						pkChr->WarpSet(mPos.x, mPos.y, pMap->GetMapIndex());
					}
				}
			}

			return true;
		}

		return false;
	}

	void CSnk::Start(LPCHARACTER pkChar)
	{
		if (!pkChar)
			return;

		const LPPARTY& pParty = pkChar->GetParty();
		if (!pParty)
		{
			pkChar->ChatPacket(CHAT_TYPE_INFO, "Grubun dağıldı. Burada ne yapıyorsun?");
			return;
		}

		const long lMapIndex = pkChar->GetMapIndex();
		if (lMapIndex < 1000)
			return;

		CSnkMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (pMap)
			pMap->StartDungeon(pkChar);
	}
};
#endif
