#include "stdafx.h"

#ifdef ENABLE_WHITE_DRAGON
#include "WhiteDragon.h"

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
#include "questmanager.h"

namespace WhiteDragon
{
	/*
		Coords Ranges
		Middle: 130-130
		Top: 130, 80
		Left: 90, 130
		Right: 170, 130
		Bottom: 130, 160
	*/

	// WHITEDRAGON_SOLDIER - Soldat des Nordwindhorts [6784]
	int WhiteDragonSoldier[15][2] =
	{
		{ 110, 130 },
		{ 120, 140 },
		{ 150, 120 },
		{ 160, 140 },
		{ 100, 150 },
		{ 140, 160 },
		{ 170, 150 },
		{ 100, 110 },
		{ 160, 110 },
		{ 120, 90 },
		{ 150, 170 },
		{ 100, 160 },
		{ 170, 170 },
		{ 90, 130 },
		{ 130, 100 },
	};

	// WHITEDRAGON_SHELTER - Scharmützler d.Horts [6785]
	int WhiteDragonShelter[15][2] =
	{
		{ 160, 150 },
		{ 110, 160 },
		{ 120, 80 },
		{ 170, 110 },
		{ 100, 140 },
		{ 130, 170 },
		{ 150, 90 },
		{ 80, 130 },
		{ 160, 130 },
		{ 130, 70 },
		{ 70, 130 },
		{ 130, 180 },
		{ 180, 130 },
		{ 120, 120 },
		{ 140, 120 },
	};

	// WHITEDRAGON_WARRIOR - Krieger d.Nordwindhorts [6786]
	int WhiteDragonWarrior[15][2] =
	{
		{ 120, 140 },
		{ 140, 140 },
		{ 120, 100 },
		{ 140, 100 },
		{ 100, 120 },
		{ 100, 140 },
		{ 160, 120 },
		{ 160, 140 },
		{ 100, 160 },
		{ 100, 100 },
		{ 160, 100 },
		{ 80, 120 },
		{ 80, 140 },
		{ 160, 80 },
		{ 120, 160 },
	};

	// WHITEDRAGON_WARLOCK - Zauberer d.Nordwindh. [6787]
	int WhiteDragonWarlock[15][2] =
	{
		{ 140, 160 },
		{ 130, 80 },
		{ 130, 120 },
		{ 110, 110 },
		{ 150, 150 },
		{ 120, 110 },
		{ 140, 110 },
		{ 110, 90 },
		{ 150, 130 },
		{ 90, 110 },
		{ 170, 140 },
		{ 90, 150 },
		{ 170, 120 },
		{ 90, 90 },
		{ 170, 160 },
	};

	// WHITEDRAGON_GENERAL - General d.Nordwindhorts [6788]
	int WhiteDragonGeneral[15][2] =
	{
		{ 110, 150 },
		{ 150, 110 },
		{ 130, 90 },
		{ 130, 170 },
		{ 100, 90 },
		{ 160, 170 },
		{ 120, 70 },
		{ 140, 70 },
		{ 80, 100 },
		{ 180, 100 },
		{ 80, 160 },
		{ 180, 160 },
		{ 110, 80 },
		{ 150, 100 },
		{ 130, 150 },
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
			pPlayerID.push_back(ch->GetPlayerID());
		}
	};

	struct FExitAndGoTo
	{
		FExitAndGoTo() {};
		void operator()(LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER pkChar = (LPCHARACTER)ent;
				if (pkChar && pkChar->IsPC())
				{
					PIXEL_POSITION posSub = CWhDr::instance().GetSubXYZ();
					if (!posSub.x)
						pkChar->WarpSet(EMPIRE_START_X(pkChar->GetEmpire()), EMPIRE_START_Y(pkChar->GetEmpire()));
					else
						pkChar->WarpSet(posSub.x, posSub.y);
				}
			}
		}
	};

	// DUNGEON_KILL_ALL_BUG_FIX
	struct FPurgeSectree
	{
		void operator () (LPENTITY ent)
		{
			if (ent->IsType(ENTITY_CHARACTER))
			{
				LPCHARACTER ch = (LPCHARACTER)ent;
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
		}
	};
	// END_OF_DUNGEON_KILL_ALL_BUG_FIX

#ifdef ENABLE_YOHARA_SYSTEM
	int iSungmaWhitePoint[4][3] =
	{
		{ POINT_SUNGMA_STR, 60, 70 },
		{ POINT_SUNGMA_HP, 50, 60 },
		{ POINT_SUNGMA_MOVE, 60, 70 },
		{ POINT_SUNGMA_IMMUNE, 65, 75 },
	};
#endif

	// Boss Spawn
	EVENTINFO(r_whitespawn_info)
	{
		CWhDrMap* pMap;
		uint8_t bStep;
		uint8_t bSubStep;
	};

	EVENTFUNC(r_whitespawn_event)
	{
		r_whitespawn_info* pEventInfo = dynamic_cast<r_whitespawn_info*>(event->info);
		if (!pEventInfo)
			return 0;

		CWhDrMap* pMap = pEventInfo->pMap;
		if (!pMap)
			return 0;

		if (!pMap->GetMapSectree())
			return 0;

		if (pEventInfo->bStep == 2)
		{
			if (pMap->GetDungeonLevel())
				pMap->Spawn((uint32_t)BOSS_VNUM, 138, 94, 0);
			else
				pMap->Spawn((uint32_t)BOSS_EASY_VNUM, 138, 94, 0);
			return 0;
		}

		if (pEventInfo->bStep == 3)
		{
			pMap->EndDungeonWarp();
			return 0;
		}

		return 0;
	}

	// Monster Spawn
#ifdef ENABLE_WHITE_DRAGON_EX
	EVENTINFO(r_white_monster_spawn_info)
	{
		CWhDrMap* pMap;
		uint8_t bStep;
	};

	EVENTFUNC(r_whitespawn_monster_event)
	{
		r_white_monster_spawn_info* pEventInfo2 = dynamic_cast<r_white_monster_spawn_info*>(event->info);
		if (!pEventInfo2)
			return 0;

		CWhDrMap* pMap = pEventInfo2->pMap;
		if (!pMap)
			return 0;

		if (!pMap->GetMapSectree())
			return 0;

		if (pEventInfo2->bStep == 2)
		{
			//DISTANCE_APPROX(ch->GetX() - npc->GetX(), ch->GetY() - npc->GetY()) < dist * 100)
			int random_num = number(0, 15);
			pMap->Spawn(static_cast<uint32_t>(WHITEDRAGON_SOLDIER), WhiteDragonSoldier[random_num][0], WhiteDragonSoldier[random_num][1], 0);
			pMap->Spawn(static_cast<uint32_t>(WHITEDRAGON_SHELTER), WhiteDragonShelter[random_num][0], WhiteDragonShelter[random_num][1], 0);
			pMap->Spawn(static_cast<uint32_t>(WHITEDRAGON_WARRIOR), WhiteDragonWarrior[random_num][0], WhiteDragonWarrior[random_num][1], 0);
			pMap->Spawn(static_cast<uint32_t>(WHITEDRAGON_WARLOCK), WhiteDragonWarlock[random_num][0], WhiteDragonWarlock[random_num][1], 0);
			pMap->Spawn(static_cast<uint32_t>(WHITEDRAGON_GENERAL), WhiteDragonGeneral[random_num][0], WhiteDragonGeneral[random_num][1], 0);
		}

		if (pEventInfo2->bStep == 3)
			return 0;

		return PASSES_PER_SEC(20);
	}
#endif

	EVENTINFO(r_whitelimit_info)
	{
		CWhDrMap* pMap;
	};

	EVENTFUNC(r_whitelimit_event)
	{
		r_whitelimit_info* pEventInfo = dynamic_cast<r_whitelimit_info*>(event->info);
		if (pEventInfo)
		{
			CWhDrMap* pMap = pEventInfo->pMap;
			if (pMap)
			{
				pMap->EndDungeonWarp();
			}
		}

		return 0;
	}

	/*
	Object Part
	*/
	CWhDrMap::CWhDrMap(long lMapIndex, bool bHard)
	{
		SetDungeonStep(1);
		SetMapIndex(lMapIndex);

		SetMapSectree(SECTREE_MANAGER::instance().GetMap(map_index));

		SetDungeonLevel(bHard);
#ifdef ENABLE_YOHARA_SYSTEM
		m_vec_mapSungmaWhitePoint.clear();
		LoadSungmaAttr();
#endif

		Start();
	}

	CWhDrMap::~CWhDrMap()
	{
		if (e_SpawnEvent != nullptr)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = nullptr;

#ifdef ENABLE_WHITE_DRAGON_EX
		if (e_SpawnMonsterEvent != nullptr)
			event_cancel(&e_SpawnMonsterEvent);
		e_SpawnMonsterEvent = nullptr;
#endif

		if (e_LimitEvent != nullptr)
			event_cancel(&e_LimitEvent);
		e_LimitEvent = nullptr;
	}

	void CWhDrMap::Destroy()
	{
		if (e_SpawnEvent != nullptr)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = nullptr;

#ifdef ENABLE_WHITE_DRAGON_EX
		if (e_SpawnMonsterEvent != nullptr)
			event_cancel(&e_SpawnMonsterEvent);
		e_SpawnMonsterEvent = nullptr;
#endif

		if (e_LimitEvent != nullptr)
			event_cancel(&e_LimitEvent);
		e_LimitEvent = nullptr;

		SetDungeonStep(1);
		SetMapIndex(0);
		SetMapSectree(nullptr);
		SetParty(nullptr);

#ifdef ENABLE_YOHARA_SYSTEM
		m_vec_mapSungmaWhitePoint.clear();
#endif
	}

	void CWhDrMap::StartDungeon(LPCHARACTER pkChar)
	{
		char szNotice[512];
		snprintf(szNotice, sizeof(szNotice), "The dungeon will be available for %d minutes.", uint32_t(TIME_LIMIT_DUNGEON) / 60);
		SendNoticeMap(szNotice, pkChar->GetMapIndex(), true);

		pkChar->SetQuestNPCID(0);

		GetWhiteEggNPC()->Dead();
		SetWhiteEggNPC(nullptr);

		if (e_LimitEvent != nullptr)
			event_cancel(&e_LimitEvent);
		e_LimitEvent = nullptr;

		r_whitelimit_info* pEventInfo = AllocEventInfo<r_whitelimit_info>();
		pEventInfo->pMap = this;
		e_LimitEvent = event_create(r_whitelimit_event, pEventInfo, PASSES_PER_SEC(1800));

		SetDungeonStep(2);
	}

	void CWhDrMap::SetDungeonStep(uint8_t bStep)
	{
		dungeon_step = bStep;

		if (e_SpawnEvent != nullptr)
			event_cancel(&e_SpawnEvent);
		e_SpawnEvent = nullptr;

#ifdef ENABLE_WHITE_DRAGON_EX
		if (e_SpawnMonsterEvent != nullptr)
			event_cancel(&e_SpawnMonsterEvent);
		e_SpawnMonsterEvent = nullptr;
#endif

		if (bStep == 2)
		{
			// Boss Spawn
			r_whitespawn_info* pEventInfo = AllocEventInfo<r_whitespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = bStep;
			pEventInfo->bSubStep = 1;
			e_SpawnEvent = event_create(r_whitespawn_event, pEventInfo, PASSES_PER_SEC(2));

			// Monster Spawn
#ifdef ENABLE_WHITE_DRAGON_EX
			r_white_monster_spawn_info* pEventInfo2 = AllocEventInfo<r_white_monster_spawn_info>();
			pEventInfo2->pMap = this;
			pEventInfo->bStep = bStep;
			e_SpawnMonsterEvent = event_create(r_whitespawn_monster_event, pEventInfo2, PASSES_PER_SEC(5));
#endif
		}
	}

	void CWhDrMap::OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller)
	{
		uint8_t bStep = dungeon_step;

		if (!bStep)
			return;

		if (!GetMapSectree())
			return;

		if (((bStep == 2) && (pkMonster->GetMobTable().dwVnum == BOSS_VNUM || pkMonster->GetMobTable().dwVnum == BOSS_EASY_VNUM)))
		{
			FPurgeSectree f;
			GetMapSectree()->for_each(f);

			char szNotice[512];
			snprintf(szNotice, sizeof(szNotice), "Tüm katılımcılar 30 saniye içinde ışınlanacak.");
			SendNoticeMap(szNotice, GetMapIndex(), true);

			pKiller->ChatPacket(CHAT_TYPE_INFO, "Ödülünü girişten alabilirsin!");
			if (GetDungeonLevel() == true)
				pKiller->SetQuestFlag("white_dragon.loot_hard", 1);
			else
				pKiller->SetQuestFlag("white_dragon.loot_easy", 1);

			if (e_SpawnEvent != nullptr)
				event_cancel(&e_SpawnEvent);
			e_SpawnEvent = nullptr;

			r_whitespawn_info* pEventInfo = AllocEventInfo<r_whitespawn_info>();
			pEventInfo->pMap = this;
			pEventInfo->bStep = 3;
			e_SpawnEvent = event_create(r_whitespawn_event, pEventInfo, PASSES_PER_SEC(30));

#ifdef ENABLE_WHITE_DRAGON_EX
			if (e_SpawnMonsterEvent != nullptr)
				event_cancel(&e_SpawnMonsterEvent);
			e_SpawnMonsterEvent = nullptr;

			r_white_monster_spawn_info* pEventInfo2 = AllocEventInfo<r_white_monster_spawn_info>();
			pEventInfo2->pMap = this;
			pEventInfo->bStep = 3;
			e_SpawnMonsterEvent = event_create(r_whitespawn_monster_event, pEventInfo2, PASSES_PER_SEC(60));
#endif
		}
	}

	void CWhDrMap::EndDungeonWarp()
	{
		if (GetMapSectree())
		{
			FExitAndGoTo f;
			GetMapSectree()->for_each(f);
		}

		long lMapIndex = GetMapIndex();
		SECTREE_MANAGER::instance().DestroyPrivateMap(GetMapIndex());
		Destroy();
		CWhDr::instance().Remove(lMapIndex);
		M2_DELETE(this);
	}

	void CWhDrMap::Start()
	{
		if (!GetMapSectree())
			EndDungeonWarp();
		else
			SetWhiteEggNPC(Spawn((uint32_t)WHITE_EGG_VNUM, 133, 134, 0));
	}

	LPCHARACTER CWhDrMap::Spawn(uint32_t dwVnum, int iX, int iY, int iDir, bool bSpawnMotion)
	{
		if (dwVnum == 0)
			return nullptr;

		if (!GetMapSectree())
			return nullptr;

		LPCHARACTER pkMob = CHARACTER_MANAGER::instance().SpawnMob(dwVnum, GetMapIndex(), GetMapSectree()->m_setting.iBaseX + iX * 100, GetMapSectree()->m_setting.iBaseY + iY * 100, 0, bSpawnMotion, iDir == 0 ? -1 : (iDir - 1) * 45);
		if (pkMob)
			sys_log(0, "<SnakeLair> Spawn: %s (map index: %d). x: %d y: %d", pkMob->GetName(), GetMapIndex(), (GetMapSectree()->m_setting.iBaseX + iX * 100), (GetMapSectree()->m_setting.iBaseY + iY * 100));

		return pkMob;
	}

#ifdef ENABLE_YOHARA_SYSTEM
	void CWhDrMap::LoadSungmaAttr()
	{
		TSungmaWhitePoint aMap;

		for (uint8_t by = 0; by < 4; by++)
		{
			if (GetDungeonLevel())
			{
				aMap.bDungeonLevel = true;
				aMap.iPointType = iSungmaWhitePoint[by][0];
				aMap.iValue = iSungmaWhitePoint[by][2];
				m_vec_mapSungmaWhitePoint.push_back(aMap);
			}
			else
			{
				aMap.bDungeonLevel = false;
				aMap.iPointType = iSungmaWhitePoint[by][0];
				aMap.iValue = iSungmaWhitePoint[by][1];
				m_vec_mapSungmaWhitePoint.push_back(aMap);
			}
		}
	}

	int CWhDrMap::GetSungmaValuePoint(int iPoint)
	{
		if (!m_vec_mapSungmaWhitePoint.empty())
		{
			std::vector<TSungmaWhitePoint>::iterator it = m_vec_mapSungmaWhitePoint.begin();
			while (it != m_vec_mapSungmaWhitePoint.end())
			{
				const  TSungmaWhitePoint& rRegion = *(it++);
				if (rRegion.bDungeonLevel == GetDungeonLevel())
				{
					if (rRegion.iPointType == iPoint)
						return rRegion.iValue;
				}
			}
		}

		return 0;
	}
#endif

	/*
	Global Part
	*/
	void CWhDr::Initialize()
	{
		SetXYZ(0, 0, 0);
		SetSubXYZ(0, 0, 0);
		m_dwRegGroups.clear();
	}

	void CWhDr::Destroy()
	{
		itertype(m_dwRegGroups) iter = m_dwRegGroups.begin();
		for (; iter != m_dwRegGroups.end(); ++iter)
		{
			CWhDrMap* pMap = iter->second;
			SECTREE_MANAGER::instance().DestroyPrivateMap(pMap->GetMapIndex());
			pMap->Destroy();
			M2_DELETE(pMap);
		}
		SetXYZ(0, 0, 0);
		SetSubXYZ(0, 0, 0);
		m_dwRegGroups.clear();
	}

	void CWhDr::Remove(long lMapIndex)
	{
		itertype(m_dwRegGroups) iter = m_dwRegGroups.find(lMapIndex);

		if (iter != m_dwRegGroups.end())
		{
			m_dwRegGroups.erase(iter);
		}

		return;
	}

	bool CWhDr::Access(LPCHARACTER pChar, bool bHard)
	{
		if (!pChar)
			return false;

		const long lNormalMapIndex = (long)MAP_WHITEDRAGONCAVE_BOSS;
		PIXEL_POSITION pos = GetXYZ(), posSub = GetSubXYZ();

		if (!pos.x)
		{
			const LPSECTREE_MAP& pkSectreeMap = SECTREE_MANAGER::Instance().GetMap((long)(MAP_WHITEDRAGONCAVE_BOSS));
			if (pkSectreeMap)
				SetXYZ(pkSectreeMap->m_setting.iBaseX, pkSectreeMap->m_setting.iBaseY, 0);
		}

		if (!posSub.x)
		{
			const LPSECTREE_MAP& pkSectreeMap = SECTREE_MANAGER::Instance().GetMap((long)(MAP_WHITEDRAGONCAVE_01));
			if (pkSectreeMap)
				SetSubXYZ(pkSectreeMap->m_setting.iBaseX + 82 * 100, pkSectreeMap->m_setting.iBaseY + 639 * 100, 0);
		}

		long lMapIndex = SECTREE_MANAGER::instance().CreatePrivateMap(lNormalMapIndex);

		if (!lMapIndex)
		{
			pChar->ChatPacket(CHAT_TYPE_INFO, "Harita oluşturulurken bir hata oluştu.");
			return false;
		}

		CWhDrMap* pMap;

		if (bHard)
			pMap = M2_NEW CWhDrMap(lMapIndex, true);
		else
			pMap = M2_NEW CWhDrMap(lMapIndex, false);

		if (pMap)
		{
			m_dwRegGroups.insert(std::make_pair(lMapIndex, pMap));

			PIXEL_POSITION mPos;
			if (!SECTREE_MANAGER::instance().GetRecallPositionByEmpire((int)(MAP_WHITEDRAGONCAVE_BOSS), 0, mPos))
			{
				pChar->ChatPacket(CHAT_TYPE_INFO, "Sectree hatası: geri çağırma konumu alınamadı.");
				return true;
			}

			pMap->SetDungeonStep(1);

			LPPARTY pParty = pChar->GetParty();

			if (pParty)
			{
				pMap->SetParty(pParty);

				FPartyCHCollector f;
				pChar->GetParty()->ForEachOnMapMember(f, pChar->GetMapIndex());
				std::vector <uint32_t>::iterator it;
				for (it = f.pPlayerID.begin(); it != f.pPlayerID.end(); it++)
				{
					LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindByPID(*it);
					if (pkChr)
					{
						pkChr->SaveExitLocation();
						pkChr->WarpSet(mPos.x, mPos.y, pMap->GetMapIndex());
					}
				}
			}
			else
			{
				if (pChar)
				{
					pChar->SaveExitLocation();
					pChar->WarpSet(mPos.x, mPos.y, pMap->GetMapIndex());
				}
			}

			return true;
		}

		return false;
	}

	bool CWhDr::IsWhiteMap(long lMapIndex)
	{
		long lMinIndex = (long)(MAP_WHITEDRAGONCAVE_BOSS) * 10000, lMaxIndex = (long)(MAP_WHITEDRAGONCAVE_BOSS) * 10000 + 10000;
		if (((lMapIndex >= lMinIndex) && (lMapIndex <= lMaxIndex)) || (lMapIndex == (long)(MAP_WHITEDRAGONCAVE_BOSS)))
			return true;

		return false;
	}

#ifdef ENABLE_YOHARA_SYSTEM
	int CWhDr::GetSungmaWhiteDungeonValue(const CHARACTER* pkChar, int iPoint) const
	{
		if (!pkChar)
			return 0;

		if (!pkChar->IsPC())
			return 0;

		long lMapIndex = pkChar->GetMapIndex();

		if (lMapIndex < 1000)
			return 0;

		itertype(m_dwRegGroups) iter = m_dwRegGroups.find(lMapIndex), iterEnd = m_dwRegGroups.end();
		if (iter == iterEnd)
			return 0;

		CWhDrMap* pMap = m_dwRegGroups.find(lMapIndex)->second;

		if (pMap)
			return pMap->GetSungmaValuePoint(iPoint);

		return 0;
	}
#endif

	void CWhDr::OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller)
	{
		if ((!pkMonster) || (!pKiller))
			return;

		long lMapIndex = pKiller->GetMapIndex();

		if (lMapIndex < 1000)
			return;

		CWhDrMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (!pMap)
			return;

		pMap->OnKill(pkMonster, pKiller);

		return;
	}

	void CWhDr::StartDungeon(LPCHARACTER pkChar)
	{
		if (!pkChar)
			return;

		long lMapIndex = pkChar->GetMapIndex();

		if (lMapIndex < 1000)
			return;

		itertype(m_dwRegGroups) iter = m_dwRegGroups.find(lMapIndex), iterEnd = m_dwRegGroups.end();
		if (iter == iterEnd)
			return;

		CWhDrMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (pMap)
		{
			pMap->StartDungeon(pkChar);
		}
	}

	void CWhDr::Start(LPCHARACTER pkChar)
	{
		if (!pkChar)
			return;

		long lMapIndex = pkChar->GetMapIndex();

		if (lMapIndex < 1000)
			return;

		CWhDrMap* pMap = m_dwRegGroups.find(lMapIndex)->second;
		if (pMap)
			pMap->StartDungeon(pkChar);
	}
};
#endif
