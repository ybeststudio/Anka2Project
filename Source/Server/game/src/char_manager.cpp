#include "stdafx.h"

#include "../../common/VnumHelper.h"
#include "../../common/service.h"
#include "../../common/tables.h"

#include "constants.h"
#include "utils.h"
#include "desc.h"
#include "char.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "party.h"
#include "regen.h"
#include "p2p.h"
#include "dungeon.h"
#include "db.h"
#include "config.h"
#include "xmas_event.h"
#include "questmanager.h"
#include "questlua.h"
#include "locale_service.h"
#include "safebox.h"
#include "item.h"
#ifdef ENABLE_ITEMSHOP
#include "desc_client.h"
#include "desc_manager.h"
#endif
#ifdef ENABLE_ITEMSHOP
#include "item_manager.h"
#endif
#ifdef ENABLE_EVENT_SYSTEM
	#include "auto_event_list.h"
#endif

#include <boost/bind.hpp>

CHARACTER_MANAGER::CHARACTER_MANAGER() :
	m_iVIDCount(0),
	m_pkChrSelectedStone(NULL),
	m_bUsePendingDestroy(false)
{
	RegisterRaceNum(xmas::MOB_XMAS_FIRWORK_SELLER_VNUM);
	RegisterRaceNum(xmas::MOB_SANTA_VNUM);
	RegisterRaceNum(xmas::MOB_XMAS_TREE_VNUM);

	m_iMobItemRate = 100;
	m_iMobDamageRate = 100;
	m_iMobGoldAmountRate = 100;
	m_iMobGoldDropRate = 100;
	m_iMobExpRate = 100;

	m_iMobItemRatePremium = 100;
	m_iMobGoldAmountRatePremium = 100;
	m_iMobGoldDropRatePremium = 100;
	m_iMobExpRatePremium = 100;

	m_iUserDamageRate = 100;
	m_iUserDamageRatePremium = 100;
#ifdef ENABLE_EVENT_BANNER_FLAG
	m_bIsLoadedBanners = false;
#endif
}

CHARACTER_MANAGER::~CHARACTER_MANAGER()
{
#ifdef __AUTO_HUNT__
	m_mapTargetReservation.clear();
#endif
	Destroy();
}

void CHARACTER_MANAGER::Destroy()
{
#ifdef ENABLE_ITEMSHOP
	m_IShopManager.clear();
#endif

	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.begin();
	while (it != m_map_pkChrByVID.end()) {
		LPCHARACTER ch = it->second;
		M2_DESTROY_CHARACTER(ch); // m_map_pkChrByVID is changed here
		it = m_map_pkChrByVID.begin();
	}
}

void CHARACTER_MANAGER::GracefulShutdown()
{
	NAME_MAP::iterator it = m_map_pkPCChr.begin();

	while (it != m_map_pkPCChr.end())
		(it++)->second->Disconnect("GracefulShutdown");
}

DWORD CHARACTER_MANAGER::AllocVID()
{
	++m_iVIDCount;
	return m_iVIDCount;
}

LPCHARACTER CHARACTER_MANAGER::CreateCharacter(const char * name, DWORD dwPID)
{
	DWORD dwVID = AllocVID();

	LPCHARACTER ch = M2_NEW CHARACTER;
	ch->Create(name, dwVID, dwPID ? true : false);

	m_map_pkChrByVID.insert(std::make_pair(dwVID, ch));

	if (dwPID)
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];
		str_lower(name, szName, sizeof(szName));

		m_map_pkPCChr.insert(NAME_MAP::value_type(szName, ch));
		m_map_pkChrByPID.insert(std::make_pair(dwPID, ch));
	}

	return (ch);
}

void CHARACTER_MANAGER::DestroyCharacter(LPCHARACTER ch)
{
	if (!ch)
		return;

	// <Factor> Check whether it has been already deleted or not.
	// Safely get VID - if pointer is invalid, GetVID() might crash
	DWORD dwVID = 0;
	try
	{
		dwVID = ch->GetVID();
	}
	catch (...)
	{
		sys_err("[CHARACTER_MANAGER::DestroyCharacter] Invalid pointer, cannot get VID");
		return;
	}

	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.find(dwVID);
	if (it == m_map_pkChrByVID.end())
	{
		sys_err("[CHARACTER_MANAGER::DestroyCharacter] <Factor> %d not found", (long)(ch->GetVID()));
		return; // prevent duplicated destrunction
	}

	// Monsters belonging to the dungeon should also be deleted from the dungeon.
	if (ch->IsNPC() && !ch->IsPet() && ch->GetRider() == NULL
#ifdef ENABLE_GROWTH_PET_SYSTEM
		&& !ch->IsGrowthPet()
#endif
#ifdef ENABLE_BOT_PLAYER
		&& !ch->IsBotCharacter()
#endif
		)
	{
		if (ch->GetDungeon())
		{
			ch->GetDungeon()->DeadCharacter(ch);
		}
	}

	if (m_bUsePendingDestroy)
	{
		m_set_pkChrPendingDestroy.insert(ch);
		return;
	}

	m_map_pkChrByVID.erase(it);

#ifdef __AUTO_HUNT__
	// AUTO_HUNT rezervasyon temizliði - karakter silinmeden önce
	// Eðer bu karakter bir hedef olarak rezerve edilmiþse (metin taþý veya mob), rezervasyonu temizle
	DWORD dwTargetVID = ch->GetVID();
	if (dwTargetVID != 0)
	{
		ReleaseTarget(dwTargetVID);
	}
	
	// Eðer bu karakter bir PC ise ve AUTO_HUNT aktifse, onun rezerve ettiði tüm hedefleri temizle
	if (ch->IsPC() && ch->GetPlayerID() != 0)
	{
		ReleaseAllTargetsByPlayer(ch->GetPlayerID());
	}
#endif

	if (true == ch->IsPC())
	{
		char szName[CHARACTER_NAME_MAX_LEN + 1];

		str_lower(ch->GetName(), szName, sizeof(szName));

		NAME_MAP::iterator it = m_map_pkPCChr.find(szName);

		if (m_map_pkPCChr.end() != it)
			m_map_pkPCChr.erase(it);
	}

	if (0 != ch->GetPlayerID())
	{
		itertype(m_map_pkChrByPID) it = m_map_pkChrByPID.find(ch->GetPlayerID());

		if (m_map_pkChrByPID.end() != it)
		{
			m_map_pkChrByPID.erase(it);
		}
	}

	UnregisterRaceNumMap(ch);
	RemoveFromStateList(ch);
	M2_DELETE(ch);
}

LPCHARACTER CHARACTER_MANAGER::Find(DWORD dwVID)
{
	// Güvenlik kontrolü: m_map_pkChrByVID map'i destroy edilmiþ olabilir
	// Bu durumda crash'i önlemek için null check yap
	if (m_map_pkChrByVID.empty())
		return NULL;

	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.find(dwVID);

	if (m_map_pkChrByVID.end() == it)
		return NULL;

	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && dwVID != (DWORD)found->GetVID())
	{
		sys_err("[CHARACTER_MANAGER::Find] <Factor> %u != %u", dwVID, (DWORD)found->GetVID());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::Find(const VID & vid)
{
	LPCHARACTER tch = Find((DWORD) vid);

	if (!tch || tch->GetVID() != vid)
		return NULL;

	return tch;
}

LPCHARACTER CHARACTER_MANAGER::FindByPID(DWORD dwPID)
{
	itertype(m_map_pkChrByPID) it = m_map_pkChrByPID.find(dwPID);

	if (m_map_pkChrByPID.end() == it)
		return NULL;

	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && dwPID != found->GetPlayerID())
	{
		sys_err("[CHARACTER_MANAGER::FindByPID] <Factor> %u != %u", dwPID, found->GetPlayerID());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::FindPC(const char * name)
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	str_lower(name, szName, sizeof(szName));
	NAME_MAP::iterator it = m_map_pkPCChr.find(szName);

	if (it == m_map_pkPCChr.end())
		return NULL;

	// <Factor> Added sanity check
	LPCHARACTER found = it->second;
	if (found != NULL && strncasecmp(szName, found->GetName(), CHARACTER_NAME_MAX_LEN) != 0)
	{
		sys_err("[CHARACTER_MANAGER::FindPC] <Factor> %s != %s", name, found->GetName());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRandomPosition(DWORD dwVnum, long lMapIndex, bool is_aggressive)
{
	// Allows you to decide why or not to spawn
	{
		if (dwVnum == 5001 && !quest::CQuestManager::instance().GetEventFlag("japan_regen"))
		{
			sys_log(1, "WAEGU[5001] regen disabled.");
			return NULL;
		}
	}

	// Allows you to decide whether or not to spawn a hatchet
	{
		if (dwVnum == 5002 && !quest::CQuestManager::instance().GetEventFlag("newyear_mob"))
		{
			sys_log(1, "HAETAE (new-year-mob) [5002] regen disabled.");
			return NULL;
		}
	}

	// Liberation Day event
	{
		if (dwVnum == 5004 && !quest::CQuestManager::instance().GetEventFlag("independence_day"))
		{
			sys_log(1, "INDEPENDECE DAY [5004] regen disabled.");
			return NULL;
		}
	}

#ifdef ENABLE_ATTENDANCE_EVENT
	if (dwVnum >= 6500 && dwVnum <= 6504 && !quest::CQuestManager::instance().GetEventFlag("enable_attendance_event"))
	{
		sys_log(1, "ATTENDANCE [6500 - 6504] regen disabled.");
		return NULL;
	}
	
	if (dwVnum >= 8041 && dwVnum <= 8050 && !quest::CQuestManager::instance().GetEventFlag("enable_easter_event"))
	{
		sys_log(1, "Easter Event [8041 - 8050] regen disabled.");
		return NULL;
	}
#endif

	const CMob * pkMob = CMobManager::instance().Get(dwVnum);

	if (!pkMob)
	{
		sys_err("no mob data for vnum %u", dwVnum);
		return NULL;
	}

	if (!map_allow_find(lMapIndex))
	{
		sys_err("not allowed map %u", lMapIndex);
		return NULL;
	}

	LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
	if (pkSectreeMap == NULL)
	{
		return NULL;
	}

	int i;
	long x, y;
	for (i=0; i<2000; i++)
	{
		x = number(1, (pkSectreeMap->m_setting.iWidth / 100) - 1) * 100 + pkSectreeMap->m_setting.iBaseX;
		y = number(1, (pkSectreeMap->m_setting.iHeight / 100) - 1) * 100 + pkSectreeMap->m_setting.iBaseY;
		LPSECTREE tree = pkSectreeMap->Find(x, y);

		if (!tree)
			continue;

		DWORD dwAttr = tree->GetAttribute(x, y);

		if (IS_SET(dwAttr, ATTR_BLOCK | ATTR_OBJECT))
			continue;

		if (IS_SET(dwAttr, ATTR_BANPK))
			continue;

		break;
	}

	if (i == 2000)
	{
		sys_err("cannot find valid location");
		return NULL;
	}

	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "SpawnMobRandomPosition: cannot create monster at non-exist sectree %d x %d (map %d)", x, y, lMapIndex);
		return NULL;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);

	if (!ch)
	{
		sys_log(0, "SpawnMobRandomPosition: cannot create new character");
		return NULL;
	}

	ch->SetProto(pkMob);

	// if mob is npc with no empire assigned, assign to empire of map
	if (CMobVnumHelper::IsNPCType(pkMob->m_table.bType))
	{
		if (ch->GetEmpire() == 0)
			ch->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex));
	}

	ch->SetRotation(number(0, 360));
	if (is_aggressive) //@fixme195
		ch->SetAggressive();

	if (!ch->Show(lMapIndex, x, y, 0, false))
	{
		M2_DESTROY_CHARACTER(ch);
		sys_err("SpawnMobRandomPosition: cannot show monster");
		return NULL;
	}

	char buf[512+1];
	long local_x = x - pkSectreeMap->m_setting.iBaseX;
	long local_y = y - pkSectreeMap->m_setting.iBaseY;
	snprintf(buf, sizeof(buf), "spawn %s[%d] random position at %ld %ld %ld %ld (time: %d)", ch->GetName(), dwVnum, x, y, local_x, local_y, get_global_time());

	if (test_server)
		SendNotice(buf);

	sys_log(0, buf);
	return (ch);
}

#ifdef ENABLE_SHOW_MOB_INFO
LPCHARACTER CHARACTER_MANAGER::SpawnMob(DWORD dwVnum, long lMapIndex, long x, long y, long z, bool bSpawnMotion, int iRot, bool bShow, bool bAggressive)
#else
LPCHARACTER CHARACTER_MANAGER::SpawnMob(DWORD dwVnum, long lMapIndex, long x, long y, long z, bool bSpawnMotion, int iRot, bool bShow)
#endif
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);
	if (!pkMob)
	{
		sys_err("SpawnMob: no mob data for vnum %u", dwVnum);
		return NULL;
	}

	if (!(CMobVnumHelper::IsNPCType(pkMob->m_table.bType) || pkMob->m_table.bType == CHAR_TYPE_WARP || pkMob->m_table.bType == CHAR_TYPE_GOTO) || mining::IsVeinOfOre (dwVnum))
	{
		LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

		if (!tree)
		{
			sys_log(0, "no sectree for spawn at %d %d mobvnum %d mapindex %d", x, y, dwVnum, lMapIndex);
			return NULL;
		}

		DWORD dwAttr = tree->GetAttribute(x, y);

		bool is_set = false;

		if ( mining::IsVeinOfOre (dwVnum) ) is_set = IS_SET(dwAttr, ATTR_BLOCK);
		else is_set = IS_SET(dwAttr, ATTR_BLOCK | ATTR_OBJECT);

		if ( is_set )
		{
			// SPAWN_BLOCK_LOG
			static bool s_isLog=quest::CQuestManager::instance().GetEventFlag("spawn_block_log");
			static DWORD s_nextTime=get_global_time()+10000;

			DWORD curTime=get_global_time();

			if (curTime>s_nextTime)
			{
				s_nextTime=curTime;
				s_isLog=quest::CQuestManager::instance().GetEventFlag("spawn_block_log");

			}

			if (s_isLog)
				sys_log(0, "SpawnMob: BLOCKED position for spawn %s %u at %d %d (attr %u)", pkMob->m_table.szName, dwVnum, x, y, dwAttr);
			// END_OF_SPAWN_BLOCK_LOG
			return NULL;
		}

		if (IS_SET(dwAttr, ATTR_BANPK))
		{
			sys_log(0, "SpawnMob: BAN_PK position for mob spawn %s %u at %d %d", pkMob->m_table.szName, dwVnum, x, y);
			return NULL;
		}
	}

	LPSECTREE sectree = SECTREE_MANAGER::instance().Get(lMapIndex, x, y);

	if (!sectree)
	{
		sys_log(0, "SpawnMob: cannot create monster at non-exist sectree %d x %d (map %d)", x, y, lMapIndex);
		return NULL;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);

	if (!ch)
	{
		sys_log(0, "SpawnMob: cannot create new character");
		return NULL;
	}

	if (iRot == -1)
		iRot = number(0, 360);

	ch->SetProto(pkMob);

	// if mob is npc with no empire assigned, assign to empire of map
	if (CMobVnumHelper::IsNPCType(pkMob->m_table.bType))
	{
		if (ch->GetEmpire() == 0)
			ch->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex));
	}

	ch->SetRotation(iRot);

#ifdef ENABLE_SHOW_MOB_INFO
	if (bShow && !ch->Show(lMapIndex, x, y, z, bSpawnMotion, bAggressive))
#else
	if (bShow && !ch->Show(lMapIndex, x, y, z, bSpawnMotion))
#endif
	{
		M2_DESTROY_CHARACTER(ch);
		sys_log(0, "SpawnMob: cannot show monster");
		return NULL;
	}

	return (ch);
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRange(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, bool bIsException, bool bSpawnMotion, bool bAggressive )
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);

	if (!pkMob)
		return NULL;

	if (pkMob->m_table.bType == CHAR_TYPE_STONE
#ifdef ENABLE_DEFENSAWE_SHIP
	|| dwVnum == 6512
#endif
	)
		bSpawnMotion = true;

	int i = 16;

	// Ensure sx <= ex and sy <= ey
	int min_x = (sx < ex) ? sx : ex;
	int max_x = (sx < ex) ? ex : sx;
	int min_y = (sy < ey) ? sy : ey;
	int max_y = (sy < ey) ? ey : sy;

	while (i--)
	{
		int x = number(min_x, max_x);
		int y = number(min_y, max_y);

#ifdef ENABLE_SHOW_MOB_INFO
		LPCHARACTER ch = SpawnMob(dwVnum, lMapIndex, x, y, 0, bSpawnMotion, -1, true, bAggressive);
		if (ch)
		{
			sys_log(1, "MOB_SPAWN: %s(%d):%d %dx%d", ch->GetName(), (DWORD)ch->GetVID(), bAggressive ? 1 : 0, ch->GetX(), ch->GetY());
			return (ch);
		}
#else
		LPCHARACTER ch = SpawnMob(dwVnum, lMapIndex, x, y, 0, bSpawnMotion);

		if (ch)
		{
			sys_log(1, "MOB_SPAWN: %s(%d) %dx%d", ch->GetName(), (DWORD) ch->GetVID(), ch->GetX(), ch->GetY());
			if ( bAggressive )
				ch->SetAggressive();
			return (ch);
		}
#endif
	}

	return NULL;
}

void CHARACTER_MANAGER::SelectStone(LPCHARACTER pkChr)
{
	m_pkChrSelectedStone = pkChr;
}

bool CHARACTER_MANAGER::SpawnMoveGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, int tx, int ty, LPREGEN pkRegen, bool bAggressive_)
{
	CMobGroup * pkGroup = CMobManager::Instance().GetGroup(dwVnum);

	if (!pkGroup)
	{
		//sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
		return false;
	}

	LPCHARACTER pkChrMaster = NULL;
	LPPARTY pkParty = NULL;

	const std::vector<DWORD> & c_rdwMembers = pkGroup->GetMemberVector();

	bool bSpawnedByStone = false;
	bool bAggressive = bAggressive_;

	if (m_pkChrSelectedStone)
	{
		bSpawnedByStone = true;
		if (m_pkChrSelectedStone->GetDungeon())
			bAggressive = true;
	}

	for (DWORD i = 0; i < c_rdwMembers.size(); ++i)
	{
		LPCHARACTER tch = SpawnMobRange(c_rdwMembers[i], lMapIndex, sx, sy, ex, ey, true, bSpawnedByStone);

		if (!tch)
		{
			if (i == 0)
				return false;

			continue;
		}

		sx = tch->GetX() - number(300, 500);
		sy = tch->GetY() - number(300, 500);
		ex = tch->GetX() + number(300, 500);
		ey = tch->GetY() + number(300, 500);

		if (m_pkChrSelectedStone)
			tch->SetStone(m_pkChrSelectedStone);
		else if (pkParty)
		{
			pkParty->Join(tch->GetVID());
			pkParty->Link(tch);
		}
		else if (!pkChrMaster)
		{
			pkChrMaster = tch;
			pkChrMaster->SetRegen(pkRegen);

			pkParty = CPartyManager::instance().CreateParty(pkChrMaster);
		}
		if (bAggressive)
			tch->SetAggressive();

		if (tch->Goto(tx, ty))
			tch->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	}

	return true;
}

bool CHARACTER_MANAGER::SpawnGroupGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen, bool bAggressive_, LPDUNGEON pDungeon)
{
	const DWORD dwGroupID = CMobManager::Instance().GetGroupFromGroupGroup(dwVnum);

	if( dwGroupID != 0 )
	{
		return SpawnGroup(dwGroupID, lMapIndex, sx, sy, ex, ey, pkRegen, bAggressive_, pDungeon);
	}
	else
	{
		sys_err( "NOT_EXIST_GROUP_GROUP_VNUM(%u) MAP(%ld)", dwVnum, lMapIndex );
		return false;
	}
}

LPCHARACTER CHARACTER_MANAGER::SpawnGroup(DWORD dwVnum, long lMapIndex, int sx, int sy, int ex, int ey, LPREGEN pkRegen, bool bAggressive_, LPDUNGEON pDungeon)
{
	CMobGroup * pkGroup = CMobManager::Instance().GetGroup(dwVnum);

	if (!pkGroup)
	{
		//sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
		return NULL;
	}

	LPCHARACTER pkChrMaster = NULL;
	LPPARTY pkParty = NULL;

	const std::vector<DWORD> & c_rdwMembers = pkGroup->GetMemberVector();

	bool bSpawnedByStone = false;
	bool bAggressive = bAggressive_;

	if (m_pkChrSelectedStone)
	{
		bSpawnedByStone = true;

		if (m_pkChrSelectedStone->GetDungeon())
			bAggressive = true;
	}

	LPCHARACTER chLeader = NULL;

	for (DWORD i = 0; i < c_rdwMembers.size(); ++i)
	{
		LPCHARACTER tch = SpawnMobRange(c_rdwMembers[i], lMapIndex, sx, sy, ex, ey, true, bSpawnedByStone);

		if (!tch)
		{
			if (i == 0)
				return NULL;

			continue;
		}

		if (i == 0)
			chLeader = tch;

		tch->SetDungeon(pDungeon);

		sx = tch->GetX() - number(300, 500);
		sy = tch->GetY() - number(300, 500);
		ex = tch->GetX() + number(300, 500);
		ey = tch->GetY() + number(300, 500);

		if (m_pkChrSelectedStone)
			tch->SetStone(m_pkChrSelectedStone);
		else if (pkParty)
		{
			pkParty->Join(tch->GetVID());
			pkParty->Link(tch);
		}
		else if (!pkChrMaster)
		{
			pkChrMaster = tch;
			pkChrMaster->SetRegen(pkRegen);

			pkParty = CPartyManager::instance().CreateParty(pkChrMaster);
		}

		if (bAggressive)
			tch->SetAggressive();

#ifdef ENABLE_SUNG_MAHI_TOWER
		if (tch && pDungeon)
		{
			BYTE bDungeonLevel = pDungeon->GetDungeonDifficulty();
			tch->SetDungeonMultipliers(bDungeonLevel);
		}
#endif
	}

	return chLeader;
}

struct FuncUpdateAndResetChatCounter
{
	void operator () (LPCHARACTER ch)
	{
		ch->ResetChatCounter();
		ch->CFSM::Update();
#ifdef ENABLE_EVENT_SYSTEM
		CGameEventsManager::instance().SendEventCharacter(ch);
#endif
	}
};

void CHARACTER_MANAGER::Update(int iPulse)
{
	using namespace std;

	BeginPendingDestroy();

	{
		if (!m_map_pkPCChr.empty())
		{
			CHARACTER_VECTOR v;
			v.reserve(m_map_pkPCChr.size());
			transform(m_map_pkPCChr.begin(), m_map_pkPCChr.end(), back_inserter(v), boost::bind(&NAME_MAP::value_type::second, _1));
			if (0 == (iPulse % PASSES_PER_SEC(5)))
			{
				FuncUpdateAndResetChatCounter f;
				for_each(v.begin(), v.end(), f);
			}
			else
			{
				for_each(v.begin(), v.end(), std::bind(&CHARACTER::UpdateCharacter, std::placeholders::_1, iPulse));
			}
		}
	}

	{
		if (!m_set_pkChrState.empty())
		{
			CHARACTER_VECTOR v;
			v.reserve(m_set_pkChrState.size());
			v.insert(v.end(), m_set_pkChrState.begin(), m_set_pkChrState.end());
			for_each(v.begin(), v.end(), std::bind(&CHARACTER::UpdateStateMachine, std::placeholders::_1, iPulse));
		}
	}

	{
		CharacterVectorInteractor i;

		if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(xmas::MOB_SANTA_VNUM, i))
		{
			for_each(i.begin(), i.end(), std::bind(&CHARACTER::UpdateStateMachine, std::placeholders::_1, iPulse));
		}
	}

	if (0 == (iPulse % PASSES_PER_SEC(3600)))
	{
		for (itertype(m_map_dwMobKillCount) it = m_map_dwMobKillCount.begin(); it != m_map_dwMobKillCount.end(); ++it)
			DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER_KILL, it->first, it->second);

		m_map_dwMobKillCount.clear();
	}

	if (test_server && 0 == (iPulse % PASSES_PER_SEC(60)))
		sys_log(0, "CHARACTER COUNT vid %zu pid %zu", m_map_pkChrByVID.size(), m_map_pkChrByPID.size());

	FlushPendingDestroy();
}

void CHARACTER_MANAGER::ProcessDelayedSave()
{
	CHARACTER_SET::iterator it = m_set_pkChrForDelayedSave.begin();

	while (it != m_set_pkChrForDelayedSave.end())
	{
		LPCHARACTER pkChr = *it++;
		pkChr->SaveReal();
	}

	m_set_pkChrForDelayedSave.clear();
}

bool CHARACTER_MANAGER::AddToStateList(LPCHARACTER ch)
{
	assert(ch != NULL);

	CHARACTER_SET::iterator it = m_set_pkChrState.find(ch);

	if (it == m_set_pkChrState.end())
	{
		m_set_pkChrState.insert(ch);
		return true;
	}

	return false;
}

void CHARACTER_MANAGER::RemoveFromStateList(LPCHARACTER ch)
{
	CHARACTER_SET::iterator it = m_set_pkChrState.find(ch);

	if (it != m_set_pkChrState.end())
	{
		m_set_pkChrState.erase(it);
	}
}

void CHARACTER_MANAGER::DelayedSave(LPCHARACTER ch)
{
	m_set_pkChrForDelayedSave.insert(ch);
}

bool CHARACTER_MANAGER::FlushDelayedSave(LPCHARACTER ch)
{
	CHARACTER_SET::iterator it = m_set_pkChrForDelayedSave.find(ch);

	if (it == m_set_pkChrForDelayedSave.end())
		return false;

	m_set_pkChrForDelayedSave.erase(it);
	ch->SaveReal();
	return true;
}

void CHARACTER_MANAGER::RegisterForMonsterLog(LPCHARACTER ch)
{
	m_set_pkChrMonsterLog.insert(ch);
}

void CHARACTER_MANAGER::UnregisterForMonsterLog(LPCHARACTER ch)
{
	m_set_pkChrMonsterLog.erase(ch);
}

void CHARACTER_MANAGER::PacketMonsterLog(LPCHARACTER ch, const void* buf, int size)
{
	itertype(m_set_pkChrMonsterLog) it;

	for (it = m_set_pkChrMonsterLog.begin(); it!=m_set_pkChrMonsterLog.end();++it)
	{
		LPCHARACTER c = *it;

		if (ch && DISTANCE_APPROX(c->GetX()-ch->GetX(), c->GetY()-ch->GetY())>6000)
			continue;

		LPDESC d = c->GetDesc();

		if (d)
			d->Packet(buf, size);
	}
}

void CHARACTER_MANAGER::KillLog(DWORD dwVnum)
{
	const DWORD SEND_LIMIT = 10000;

	itertype(m_map_dwMobKillCount) it = m_map_dwMobKillCount.find(dwVnum);

	if (it == m_map_dwMobKillCount.end())
		m_map_dwMobKillCount.insert(std::make_pair(dwVnum, 1));
	else
	{
		++it->second;

		if (it->second > SEND_LIMIT)
		{
			DBManager::instance().SendMoneyLog(MONEY_LOG_MONSTER_KILL, it->first, it->second);
			m_map_dwMobKillCount.erase(it);
		}
	}
}

void CHARACTER_MANAGER::RegisterRaceNum(DWORD dwVnum)
{
	m_set_dwRegisteredRaceNum.insert(dwVnum);
}

void CHARACTER_MANAGER::RegisterRaceNumMap(LPCHARACTER ch)
{
	DWORD dwVnum = ch->GetRaceNum();

	if (m_set_dwRegisteredRaceNum.find(dwVnum) != m_set_dwRegisteredRaceNum.end())
	{
		sys_log(0, "RegisterRaceNumMap %s %u", ch->GetName(), dwVnum);
		m_map_pkChrByRaceNum[dwVnum].insert(ch);
	}
}

void CHARACTER_MANAGER::UnregisterRaceNumMap(LPCHARACTER ch)
{
	DWORD dwVnum = ch->GetRaceNum();

	itertype(m_map_pkChrByRaceNum) it = m_map_pkChrByRaceNum.find(dwVnum);

	if (it != m_map_pkChrByRaceNum.end())
		it->second.erase(ch);
}

bool CHARACTER_MANAGER::GetCharactersByRaceNum(DWORD dwRaceNum, CharacterVectorInteractor & i)
{
	std::map<DWORD, CHARACTER_SET>::iterator it = m_map_pkChrByRaceNum.find(dwRaceNum);

	if (it == m_map_pkChrByRaceNum.end())
		return false;

	i = it->second;
	return true;
}

#define FIND_JOB_WARRIOR_0	(1 << 3)
#define FIND_JOB_WARRIOR_1	(1 << 4)
#define FIND_JOB_WARRIOR_2	(1 << 5)
#define FIND_JOB_WARRIOR	(FIND_JOB_WARRIOR_0 | FIND_JOB_WARRIOR_1 | FIND_JOB_WARRIOR_2)
#define FIND_JOB_ASSASSIN_0	(1 << 6)
#define FIND_JOB_ASSASSIN_1	(1 << 7)
#define FIND_JOB_ASSASSIN_2	(1 << 8)
#define FIND_JOB_ASSASSIN	(FIND_JOB_ASSASSIN_0 | FIND_JOB_ASSASSIN_1 | FIND_JOB_ASSASSIN_2)
#define FIND_JOB_SURA_0		(1 << 9)
#define FIND_JOB_SURA_1		(1 << 10)
#define FIND_JOB_SURA_2		(1 << 11)
#define FIND_JOB_SURA		(FIND_JOB_SURA_0 | FIND_JOB_SURA_1 | FIND_JOB_SURA_2)
#define FIND_JOB_SHAMAN_0	(1 << 12)
#define FIND_JOB_SHAMAN_1	(1 << 13)
#define FIND_JOB_SHAMAN_2	(1 << 14)
#define FIND_JOB_SHAMAN		(FIND_JOB_SHAMAN_0 | FIND_JOB_SHAMAN_1 | FIND_JOB_SHAMAN_2)
#ifdef ENABLE_WOLFMAN_CHARACTER
#define FIND_JOB_WOLFMAN_0	(1 << 15)
#define FIND_JOB_WOLFMAN_1	(1 << 16)
#define FIND_JOB_WOLFMAN_2	(1 << 17)
#define FIND_JOB_WOLFMAN		(FIND_JOB_WOLFMAN_0 | FIND_JOB_WOLFMAN_1 | FIND_JOB_WOLFMAN_2)
#endif

//
// (job+1)*3+(skill_group)
//
LPCHARACTER CHARACTER_MANAGER::FindSpecifyPC(unsigned int uiJobFlag, long lMapIndex, LPCHARACTER except, int iMinLevel, int iMaxLevel)
{
	LPCHARACTER chFind = NULL;
	itertype(m_map_pkChrByPID) it;
	int n = 0;

	for (it = m_map_pkChrByPID.begin(); it != m_map_pkChrByPID.end(); ++it)
	{
		LPCHARACTER ch = it->second;

		if (ch == except)
			continue;

		if (ch->GetLevel() < iMinLevel)
			continue;

		if (ch->GetLevel() > iMaxLevel)
			continue;

		if (ch->GetMapIndex() != lMapIndex)
			continue;

		if (uiJobFlag)
		{
			unsigned int uiChrJob = (1 << ((ch->GetJob() + 1) * 3 + ch->GetSkillGroup()));

			if (!IS_SET(uiJobFlag, uiChrJob))
				continue;
		}

		if (!chFind || number(1, ++n) == 1)
			chFind = ch;
	}

	return chFind;
}

int CHARACTER_MANAGER::GetMobItemRate(LPCHARACTER ch)
{
	if (ch && ch->GetPremiumRemainSeconds (PREMIUM_ITEM) > 0)
	{
		return m_iMobItemRatePremium;
	}

	return m_iMobItemRate;
}

int CHARACTER_MANAGER::GetMobDamageRate(LPCHARACTER ch)
{
	return m_iMobDamageRate; 
}

int CHARACTER_MANAGER::GetMobGoldAmountRate(LPCHARACTER ch)
{
	if (!ch)
	{
		return m_iMobGoldAmountRate;
	}

	if (ch && ch->GetPremiumRemainSeconds (PREMIUM_GOLD) > 0)
	{
		return m_iMobGoldAmountRatePremium;
	}

	return m_iMobGoldAmountRate;
}

int CHARACTER_MANAGER::GetMobGoldDropRate(LPCHARACTER ch)
{
	if (!ch)
	{
		return m_iMobGoldDropRate;
	}

	if (ch && ch->GetPremiumRemainSeconds (PREMIUM_GOLD) > 0)
	{
		return m_iMobGoldDropRatePremium;
	}

	return m_iMobGoldDropRate;
}

int CHARACTER_MANAGER::GetMobExpRate(LPCHARACTER ch)
{
	if (!ch)
	{
		return m_iMobExpRate;
	}

	if (ch && ch->GetPremiumRemainSeconds (PREMIUM_EXP) > 0)
	{
		return m_iMobExpRatePremium;
	}

	return m_iMobExpRate;
}

int	CHARACTER_MANAGER::GetUserDamageRate(LPCHARACTER ch)
{
	if (!ch)
		return m_iUserDamageRate;

	if (ch && ch->GetPremiumRemainSeconds(PREMIUM_EXP) > 0)
		return m_iUserDamageRatePremium;

	return m_iUserDamageRate;
}

void CHARACTER_MANAGER::SendScriptToMap(long lMapIndex, const std::string & s)
{
	LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);

	if (NULL == pSecMap)
		return;

	struct packet_script p;

	p.header = HEADER_GC_SCRIPT;
	p.skin = 1;
	p.src_size = s.size();

	quest::FSendPacket f;
	p.size = p.src_size + sizeof(struct packet_script);
	f.buf.write(&p, sizeof(struct packet_script));
	f.buf.write(&s[0], s.size());

	pSecMap->for_each(f);
}

bool CHARACTER_MANAGER::BeginPendingDestroy()
{
	if (m_bUsePendingDestroy)
		return false;

	m_bUsePendingDestroy = true;
	return true;
}

void CHARACTER_MANAGER::FlushPendingDestroy()
{
	using namespace std;

	m_bUsePendingDestroy = false;

	if (!m_set_pkChrPendingDestroy.empty())
	{
		sys_log(0, "FlushPendingDestroy size %d", m_set_pkChrPendingDestroy.size());

		CHARACTER_SET::iterator it = m_set_pkChrPendingDestroy.begin(),
			end = m_set_pkChrPendingDestroy.end();
		for (; it != end; ++it)
		{
			M2_DESTROY_CHARACTER(*it);
		}

		m_set_pkChrPendingDestroy.clear();
	}
}

#ifdef ENABLE_REAL_TIME_REGEN
void CHARACTER_MANAGER::EraseRealTimeRegenCharacter(WORD wNum)
{
	for (const auto & chr : m_map_pkChrByVID)
	{
		LPCHARACTER ch = chr.second;
		
		if (!ch)
			continue;
		
		if (ch->GetDungeon())
			continue;
		
		if (!ch->IsMonster() && !ch->IsStone())
			continue;
		
		if (ch->GetRealTimeRegenNum() == wNum)
		{
			M2_DESTROY_CHARACTER(ch);
			break;
		}
	}
}

LPCHARACTER CHARACTER_MANAGER::FindRealTimeRegenMonster(WORD wNum)
{
	for (const auto & chr : m_map_pkChrByVID)
	{
		LPCHARACTER ch = chr.second;
		
		if (!ch)
			continue;
		
		if (ch->GetDungeon())
			continue;
		
		if (!ch->IsMonster() && !ch->IsStone())
			continue;
		
		if (ch->GetRealTimeRegenNum() == wNum)
			return ch;
	}
	
	return nullptr;
}

void CHARACTER_MANAGER::RefreshAllMonsters()
{
	std::unordered_set<LPCHARACTER> monster_list;
	
	for (const auto & chr : m_map_pkChrByVID)
	{
		LPCHARACTER ch = chr.second;
		
		if (!ch)
			continue;
		
		if (ch->GetDungeon())
			continue;
		
		if (!ch->IsMonster() || ch->IsStone() || ch->IsBoss())
			continue;
		
		if (ch->GetRegen())
			event_reset_time(ch->GetRegen()->event, 2);
		
		monster_list.insert(ch);
	}
	
	for (const auto & ch : monster_list)
		M2_DESTROY_CHARACTER(ch);
}
#endif

void CHARACTER_MANAGER::DestroyCharacterInMap(long lMapIndex)
{
	std::vector<LPCHARACTER> tempVec;
	for (itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.begin(); it != m_map_pkChrByVID.end(); it++)
	{
		LPCHARACTER pkChr = it->second;
		if (pkChr && pkChr->GetMapIndex() == lMapIndex && pkChr->IsNPC() && !pkChr->IsPet() && pkChr->GetRider() == NULL)
		{
			tempVec.push_back(pkChr);
		}
	}
	for (std::vector<LPCHARACTER>::iterator it = tempVec.begin(); it != tempVec.end(); it++)
	{
		DestroyCharacter(*it);
	}
}

CharacterVectorInteractor::CharacterVectorInteractor(const CHARACTER_SET & r)
{
	using namespace std;

	reserve(r.size());
	insert(end(), r.begin(), r.end());

	if (CHARACTER_MANAGER::instance().BeginPendingDestroy())
		m_bMyBegin = true;
}

CharacterVectorInteractor::~CharacterVectorInteractor()
{
	if (m_bMyBegin)
		CHARACTER_MANAGER::instance().FlushPendingDestroy();
}

#ifdef ENABLE_MULTI_FARM_BLOCK
void CHARACTER_MANAGER::CheckMultiFarmAccounts(const char* szIP)
{
	auto it = m_mapmultiFarm.find(szIP);
	if (it != m_mapmultiFarm.end())
	{
		auto itVec = it->second.begin();
		while(itVec != it->second.end())
		{
			LPCHARACTER ch = FindByPID(itVec->playerID);
			CCI* chP2P = P2P_MANAGER::Instance().FindByPID(itVec->playerID);
			if(!ch && !chP2P)
				itVec = it->second.erase(itVec);
			else
				++itVec;
		}

		if(!it->second.size())
			m_mapmultiFarm.erase(szIP);
	}
}

void CHARACTER_MANAGER::RemoveMultiFarm(const char* szIP, const DWORD playerID, const bool isP2P)
{
	if (!isP2P)
	{
		TPacketGGMultiFarm p;
		p.header = HEADER_GG_MULTI_FARM;
		p.subHeader = MULTI_FARM_REMOVE;
		p.playerID = playerID;
		strlcpy(p.playerIP, szIP, sizeof(p.playerIP));
		P2P_MANAGER::Instance().Send(&p, sizeof(TPacketGGMultiFarm));
	}

	auto it = m_mapmultiFarm.find(szIP);
	if (it != m_mapmultiFarm.end())
	{
		for (auto itVec = it->second.begin(); itVec != it->second.end(); ++itVec)
		{
			if (itVec->playerID == playerID)
			{
				it->second.erase(itVec);
				break;
			}
		}

		if (!it->second.size())
			m_mapmultiFarm.erase(szIP);
	}
}

void CHARACTER_MANAGER::SetMultiFarm(const char* szIP, const DWORD playerID, const char* playerName, const bool bStatus, const BYTE affectType, const int affectTime)
{
	const auto it = m_mapmultiFarm.find(szIP);
	if (it != m_mapmultiFarm.end())
	{
		for (auto itVec = it->second.begin(); itVec != it->second.end(); ++itVec)
		{
			if (itVec->playerID == playerID)
			{
				itVec->farmStatus = bStatus;
				itVec->affectType = affectType;
				itVec->affectTime = affectTime;
				return;
			}
		}
		it->second.emplace_back(TMultiFarm(playerID, playerName, bStatus, affectType, affectTime));
	}
	else
	{
		std::vector<TMultiFarm> m_vecFarmList;
		m_vecFarmList.emplace_back(TMultiFarm(playerID, playerName, bStatus, affectType, affectTime));
		m_mapmultiFarm.emplace(szIP, m_vecFarmList);
	}
}

int CHARACTER_MANAGER::GetMultiFarmCount(const char* playerIP, std::map<DWORD, std::pair<std::string, bool>>& m_mapNames)
{
	int accCount = 0;
	bool affectTimeHas = false;
	BYTE affectType = 0;
	const auto it = m_mapmultiFarm.find(playerIP);
	if (it != m_mapmultiFarm.end())
	{
		for (auto itVec = it->second.begin(); itVec != it->second.end(); ++itVec)
		{
			if (itVec->farmStatus)
				accCount++;
			if (itVec->affectTime > get_global_time())
				affectTimeHas = true;
			if (itVec->affectType > affectType)
				affectType = itVec->affectType;
			m_mapNames.emplace(itVec->playerID, std::make_pair(itVec->playerName, itVec->farmStatus));
		}
	}

	if (affectTimeHas && affectType > 0)
		accCount -= affectType;
	if (accCount < 0)
		accCount = 0;

	return accCount;
}

void CHARACTER_MANAGER::CheckMultiFarmAccount(const char* szIP, const DWORD playerID, const char* playerName, const bool bStatus, BYTE affectType, int affectDuration, bool isP2P)
{
	CheckMultiFarmAccounts(szIP);

	LPCHARACTER ch = FindByPID(playerID);
	if (ch && bStatus)
	{
		affectDuration = ch->FindAffect(AFFECT_MULTI_FARM_PREMIUM) ? get_global_time() + ch->FindAffect(AFFECT_MULTI_FARM_PREMIUM)->lDuration : 0;
		affectType = ch->FindAffect(AFFECT_MULTI_FARM_PREMIUM) ? ch->FindAffect(AFFECT_MULTI_FARM_PREMIUM)->lApplyValue : 0;
	}

	std::map<DWORD, std::pair<std::string, bool>> m_mapNames;
	int farmPlayerCount = GetMultiFarmCount(szIP, m_mapNames);
	if (bStatus)
	{
		if (farmPlayerCount >= 2)
		{
			CheckMultiFarmAccount(szIP, playerID, playerName, false);
			return;
		}
	}

	if (!isP2P)
	{
		TPacketGGMultiFarm p;
		p.header = HEADER_GG_MULTI_FARM;
		p.subHeader = MULTI_FARM_SET;
		p.playerID = playerID;
		strlcpy(p.playerIP, szIP, sizeof(p.playerIP));
		strlcpy(p.playerName, playerName, sizeof(p.playerIP));
		p.farmStatus = bStatus;
		p.affectType = affectType;
		p.affectTime = affectDuration;
		P2P_MANAGER::Instance().Send(&p, sizeof(TPacketGGMultiFarm));
	}

	SetMultiFarm(szIP, playerID, playerName, bStatus, affectType, affectDuration);
	if(ch)
	{
		ch->SetRewardStatus(bStatus);
	}

	m_mapNames.clear();
	farmPlayerCount = GetMultiFarmCount(szIP, m_mapNames);

	for (auto it = m_mapNames.begin(); it != m_mapNames.end(); ++it)
	{
		LPCHARACTER newCh = FindByPID(it->first);
		if (newCh)
		{
			newCh->ChatPacket(CHAT_TYPE_COMMAND, "UpdateMultiFarmAffect %d %d", newCh->GetRewardStatus(), newCh == ch ? true : false);
			for (auto itEx = m_mapNames.begin(); itEx != m_mapNames.end(); ++itEx)
			{
				if (itEx->second.second)
					newCh->ChatPacket(CHAT_TYPE_COMMAND, "UpdateMultiFarmPlayer %s", itEx->second.first.c_str());
			}
		}
	}
}
#endif

#ifdef ENABLE_ITEMSHOP
void CHARACTER_MANAGER::LoadItemShopLogReal(LPCHARACTER ch, const char* c_pData)
{
	LPDESC d = ch ? ch->GetDesc() : nullptr;
	if (!d)
	{
		return;
	}

	BYTE subIndex = ITEMSHOP_LOG;

	const int logCount = *(int*)c_pData;
	c_pData += sizeof(int);

	std::vector<TIShopLogData> m_vec;

	if (logCount)
	{
		for (DWORD j = 0; j < logCount; ++j)
		{
			const TIShopLogData logData = *(TIShopLogData*)c_pData;
			m_vec.emplace_back(logData);

			c_pData += sizeof(TIShopLogData);
		}
	}

	TPacketGCItemShop p;
	p.header = HEADER_GC_ITEMSHOP;
	p.size = sizeof(TPacketGCItemShop) + sizeof(BYTE) + sizeof(int) + (sizeof(TIShopLogData) * logCount);

	TEMP_BUFFER buf;
	buf.write(&p, sizeof(TPacketGCItemShop));
	buf.write(&subIndex, sizeof(BYTE));
	buf.write(&logCount, sizeof(int));

	if (logCount > 0)
	{
		buf.write(m_vec.data(), sizeof(TIShopLogData)* logCount);
	}

	d->Packet(buf.read_peek(), buf.size());
}
void CHARACTER_MANAGER::LoadItemShopLog(LPCHARACTER ch)
{
	LPDESC d = ch ? ch->GetDesc() : nullptr;
	if (!d)
	{
		return;
	}

	BYTE subIndex = ITEMSHOP_LOG;
	DWORD accountID = d->GetAccountTable().id;

	db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE)+sizeof(DWORD));
	db_clientdesc->Packet(&subIndex, sizeof(BYTE));
	db_clientdesc->Packet(&accountID, sizeof(DWORD));
}

void CHARACTER_MANAGER::LoadItemShopData(LPCHARACTER ch, bool isAll)
{
	LPDESC d = ch ? ch->GetDesc() : nullptr;
	if (!d)
	{
		return;
	}

	TEMP_BUFFER buf;

	TPacketGCItemShop p;
	p.header = HEADER_GC_ITEMSHOP;

	long long act_lldCoins;
#ifdef USE_ITEMSHOP_RENEWED
	long long act_lldJCoins;
	ch->GetAccountMoney(act_lldCoins, act_lldJCoins);
#else
	ch->GetAccountMoney(act_lldCoins);
#endif

	if (isAll)
	{
		int calculateSize = 0;
		BYTE subIndex = ITEMSHOP_LOAD;
		calculateSize += sizeof(BYTE);

		calculateSize += sizeof(long long); // dragon coins
#ifdef USE_ITEMSHOP_RENEWED
		calculateSize += sizeof(long long); // dragon jetons
#endif
		calculateSize += sizeof(int); // updatetime

		int categoryTotalSize = m_IShopManager.size();
		calculateSize += sizeof(int);

		if (m_IShopManager.size())
		{
			for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
			{
				BYTE categoryIndex = it->first;
				calculateSize += sizeof(BYTE);

				BYTE categorySize = it->second.size();
				calculateSize += sizeof(BYTE);

				if (it->second.size())
				{
					for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
					{
						BYTE categorySubIndex = itEx->first;
						calculateSize += sizeof(BYTE);

						BYTE categorySubSize = itEx->second.size();
						calculateSize += sizeof(BYTE);

						if (categorySubSize)
						{
							calculateSize += sizeof(TIShopData) * categorySubSize;
						}
					}
				}
			}
		}

		p.size = sizeof(TPacketGCItemShop) + calculateSize;

		buf.write(&p, sizeof(TPacketGCItemShop));
		buf.write(&subIndex, sizeof(BYTE));
		buf.write(&act_lldCoins, sizeof(act_lldCoins));
#ifdef USE_ITEMSHOP_RENEWED
		buf.write(&act_lldJCoins, sizeof(act_lldJCoins));
#endif
		buf.write(&itemshopUpdateTime, sizeof(int));
		buf.write(&categoryTotalSize, sizeof(int));

		if (m_IShopManager.size())
		{
			for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
			{
				BYTE categoryIndex = it->first;
				buf.write(&categoryIndex, sizeof(BYTE));

				BYTE categorySize = it->second.size();
				buf.write(&categorySize, sizeof(BYTE));

				if (it->second.size())
				{
					for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
					{
						BYTE categorySubIndex = itEx->first;
						buf.write(&categorySubIndex, sizeof(BYTE));

						BYTE categorySubSize = itEx->second.size();
						buf.write(&categorySubSize, sizeof(BYTE));

						if (categorySubSize)
						{
							buf.write(itEx->second.data(), sizeof(TIShopData) * categorySubSize);
						}
					}
				}
			}
		}
	}
	else
	{
		p.size = sizeof(TPacketGCItemShop) + sizeof(BYTE)+sizeof(int)+sizeof(int);

		buf.write(&p, sizeof(TPacketGCItemShop));

		BYTE subIndex = ITEMSHOP_LOAD;
		buf.write(&subIndex, sizeof(BYTE));

		buf.write(&act_lldCoins, sizeof(act_lldCoins));
#ifdef USE_ITEMSHOP_RENEWED
		buf.write(&act_lldJCoins, sizeof(act_lldJCoins));
#endif
		buf.write(&itemshopUpdateTime, sizeof(int));

		int categoryTotalSize = 9999;
		buf.write(&categoryTotalSize, sizeof(int));
	}

	d->Packet(buf.read_peek(), buf.size());
}

void CHARACTER_MANAGER::LoadItemShopBuyReal(LPCHARACTER ch, const char* c_pData)
{
	LPDESC d = ch ? ch->GetDesc() : nullptr;
	if (!d)
	{
		return;
	}

	const BYTE returnType = *(BYTE*)c_pData;
	c_pData += sizeof(BYTE);

	if (returnType == 0)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 687, "");
		return;
	}
	else if (returnType == 1)
	{
		const int weekMaxCount = *(int*)c_pData;
		c_pData += sizeof(int);
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 689, "%d", weekMaxCount);
		return;
	}
	else if (returnType == 2)
	{
		const int monthMaxCount = *(int*)c_pData;
		c_pData += sizeof(int);
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 690, "%d", monthMaxCount);
		return;
	}
	else if (returnType == 4)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 688, "");
		return;
	}
	else if (returnType == 5)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 1500, "");
		return;
	}
#ifdef USE_ITEMSHOP_RENEWED
	else if (returnType == 6)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 1501, "");
		return;
	}
	else if (returnType == 7)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 1502, "");
		return;
	}
#endif

	const bool isOpenLog = *(bool*)c_pData;
	c_pData += sizeof(bool);

	const DWORD itemVnum = *(DWORD*)c_pData;
	c_pData += sizeof(DWORD);

	const int itemCount = *(int*)c_pData;
	c_pData += sizeof(int);

	const long long itemPrice = *(long long*)c_pData;
	c_pData += sizeof(long long);

#ifdef USE_ITEMSHOP_RENEWED
	const long long itemPriceJD = *(long long*)c_pData;
	c_pData += sizeof(long long);
#endif

	TEMP_BUFFER buf;
	TPacketGCItemShop p;
	p.header = HEADER_GC_ITEMSHOP;
	p.size = sizeof(TPacketGCItemShop) + sizeof(BYTE) + sizeof(long long) + sizeof(bool);

	if (isOpenLog)
	{
		p.size += sizeof(TIShopLogData);
	}

	long long act_lldCoins;
#ifdef USE_ITEMSHOP_RENEWED
	long long act_lldJCoins;
	ch->GetAccountMoney(act_lldCoins, act_lldJCoins, true);
#else
	ch->GetAccountMoney(act_lldCoins, true);
#endif

	ch->AutoGiveItem(itemVnum, itemCount, -1, false);

	BYTE subIndex = ITEMSHOP_DRAGONCOIN;

	buf.write(&p, sizeof(TPacketGCItemShop));
	buf.write(&subIndex, sizeof(BYTE));
	buf.write(&act_lldCoins, sizeof(act_lldCoins));
#ifdef USE_ITEMSHOP_RENEWED
	buf.write(&act_lldJCoins, sizeof(act_lldJCoins));
#endif
	buf.write(&isOpenLog, sizeof(bool));

	if (isOpenLog)
	{
		const TIShopLogData logData = *(TIShopLogData*)c_pData;
		c_pData += sizeof(TIShopLogData);

		buf.write(&logData, sizeof(TIShopLogData));
	}

	d->Packet(buf.read_peek(), buf.size());

#ifdef USE_ITEMSHOP_RENEWED
	if (itemPrice > 0 && itemPriceJD > 0)
	{
		if (itemCount > 1)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1508, "%d I%d %lld %lld", itemCount, itemVnum, itemPrice, itemPriceJD);
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1503, "");
		}
		else
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1509, "I%d %lld %lld", itemVnum, itemPrice, itemPriceJD);
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1503, "");
		}
	}
	else if (itemPrice > 0)
	{
		if (itemCount > 1)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1504, "%d I%d %lld", itemCount, itemVnum, itemPrice);
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1503, "");
		}
		else
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1505, "I%d %lld", itemVnum, itemPrice);
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1503, "");
		}
	}
	else if (itemPriceJD > 0)
	{
		if (itemCount > 1)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1506, "%d I%d %lld", itemCount, itemVnum, itemPriceJD);
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1503, "");
		}
		else
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1507, "I%d %lld", itemVnum, itemPriceJD);
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1503, "");
		}
	}
	else
	{
		if (itemCount > 1)
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1508, "%d I%d %lld %lld", itemCount, itemVnum, itemPrice, itemPriceJD);
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1503, "");
		}
		else
		{
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1509, "I%d %lld %lld", itemVnum, itemPrice, itemPriceJD);
			ch->LocaleChatPacket(CHAT_TYPE_INFO, 1503, "");
		}
	}
#else
	if (itemCount > 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "992 I%d %d %lld", itemVnum, itemCount, itemPrice);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "993 I%d %lld", itemVnum, itemPrice);
	}
#endif
}

void CHARACTER_MANAGER::LoadItemShopBuy(LPCHARACTER ch, int itemID, int itemCount)
{
	if (itemID < 0 || itemCount < 1 || itemCount > 20)
	{
		return;
	}

	LPDESC d = ch ? ch->GetDesc() : nullptr;
	if (!d)
	{
		return;
	}

	if (!m_IShopManager.empty())
	{
		for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
		{
			if (!it->second.empty())
			{
				for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
				{
					if (!itEx->second.empty())
					{
						for (auto itReal = itEx->second.begin(); itReal != itEx->second.end(); ++itReal)
						{
							const TIShopData& itemData = *itReal;
							if (itemData.id == itemID)
							{
								const TItemTable* pItemProto = ITEM_MANAGER::Instance().GetTable(itemData.itemVnum);
								if (!pItemProto)
								{
									sys_err("Item vnum: %u doesn't exists!", itemData.itemVnum);
									return;
								}

								if (itemCount > 1)
								{
									if (pItemProto->dwAntiFlags & ITEM_ANTIFLAG_STACK || !(pItemProto->dwFlags & ITEM_FLAG_STACKABLE))
									{
										sys_err("Item vnum: %u is not stackable!", itemData.itemVnum);
										return;
									}
								}

								long long act_lldCoins;
#ifdef USE_ITEMSHOP_RENEWED
								long long act_lldJCoins;
								ch->GetAccountMoney(act_lldCoins, act_lldJCoins);
#else
								ch->GetAccountMoney(act_lldCoins);
#endif

								long long itemPrice = itemData.itemPrice * itemCount;
#ifdef USE_ITEMSHOP_RENEWED
								long long itemPriceJD = itemData.itemPriceJD * itemCount;
#endif

								if (itemData.discount > 0)
								{
									itemPrice = static_cast<long long>((float(itemData.itemPrice) / 100.0) * float(100 - itemData.discount));
#ifdef USE_ITEMSHOP_RENEWED
									itemPriceJD = static_cast<long long>((float(itemData.itemPriceJD) / 100.0) * float(100 - itemData.discount));
#endif
								}

								if (itemPrice < 0)
								{
									return;
								}

								if (act_lldCoins < itemPrice)
								{
									ch->LocaleChatPacket(CHAT_TYPE_INFO, 687, "");
									return;
								}

#ifdef USE_ITEMSHOP_RENEWED
								if (itemPriceJD < 0)
								{
									return;
								}

								if (act_lldJCoins < itemPriceJD)
								{
									ch->LocaleChatPacket(CHAT_TYPE_INFO, 1502, "");
									return;
								}
#endif

								DWORD accountID = d->GetAccountTable().id;
								if (accountID == 0)
								{
									return;
								}

								BYTE subIndex = ITEMSHOP_BUY;

								char szName[CHARACTER_NAME_MAX_LEN + 1] = {};
								strlcpy(szName, ch->GetName(), sizeof(szName));

								char szIP[16];
								strlcpy(szIP, d->GetHostName(), sizeof(szIP));

								TEMP_BUFFER buf;
								buf.write(&subIndex, sizeof(BYTE));
								buf.write(&accountID, sizeof(DWORD));
								buf.write(&szName, sizeof(szName));
								buf.write(&szIP, sizeof(szIP));
								buf.write(&itemID, sizeof(int));
								buf.write(&itemCount, sizeof(int));
								bool isLogOpen = (ch->GetProtectTime("itemshop.log") == 1) ? true : false;
								buf.write(&isLogOpen, sizeof(bool));

								db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP, d->GetHandle(), buf.size());
								db_clientdesc->Packet(buf.read_peek(), buf.size());
								return;
							}
						}
					}
				}
			}
		}
	}
}

void CHARACTER_MANAGER::UpdateItemShopItem(const char* c_pData)
{
	const TIShopData& updateItem = *(TIShopData*)c_pData;
	c_pData += sizeof(TIShopData);

	bool sendPacketProcess = false;

	if (!m_IShopManager.empty())
	{
		for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
		{
			if (sendPacketProcess)
			{
				break;
			}

			if (!it->second.empty())
			{
				for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
				{
					if (sendPacketProcess)
					{
						break;
					}

					if (!itEx->second.empty())
					{
						for (auto itReal = itEx->second.begin(); itReal != itEx->second.end(); ++itReal)
						{
							TIShopData& itemData = *itReal;
							if (itemData.id == updateItem.id)
							{
								itemData.maxSellCount = updateItem.maxSellCount;
								sendPacketProcess = true;
								break;
							}
						}
					}
				}
			}
		}
	}

	if (sendPacketProcess)
	{
		TEMP_BUFFER buf;

		TPacketGCItemShop p;
		p.header = HEADER_GC_ITEMSHOP;
		p.size = sizeof(TPacketGCItemShop) + sizeof(BYTE) + sizeof(TIShopData);

		BYTE subIndex = ITEMSHOP_UPDATE_ITEM;
		buf.write(&p, sizeof(TPacketGCItemShop));
		buf.write(&subIndex, sizeof(BYTE));
		buf.write(&updateItem, sizeof(TIShopData));

		const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
		if (c_ref_set.size())
		{
			for (auto it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
			{
				auto desc = *it;
				if (desc)
				{
					LPCHARACTER ch = desc->GetCharacter();
					if (ch)
					{
						if (ch->GetProtectTime("itemshop.load") == 1)
						{
							desc->Packet(buf.read_peek(), buf.size());
						}
					}
				}
			}
		}
	}
}

void RefreshItemShop(LPDESC d)
{
	LPCHARACTER ch = d ? d->GetCharacter() : nullptr;
	if (!ch)
	{
		return;
	}

	if (ch->GetProtectTime("itemshop.load") == 1)
	{
		CHARACTER_MANAGER::Instance().LoadItemShopData(ch, true);

		ch->LocaleChatPacket(CHAT_TYPE_INFO, 990, "");
	}
}
void CHARACTER_MANAGER::LoadItemShopData(const char* c_pData)
{
	m_IShopManager.clear();

	const int updateTime = *(int*)c_pData;
	c_pData += sizeof(int);

	const bool isManuelUpdate = *(bool*)c_pData;
	c_pData += sizeof(bool);

	const int categoryTotalSize = *(int*)c_pData;
	c_pData += sizeof(int);

	itemshopUpdateTime = updateTime;

	for (auto j = 0; j < categoryTotalSize; ++j)
	{
		const BYTE categoryIndex = *(BYTE*)c_pData;
		c_pData += sizeof(BYTE);

		const BYTE categorySize = *(BYTE*)c_pData;
		c_pData += sizeof(BYTE);

		std::map<BYTE, std::vector<TIShopData>> m_map;
		m_map.clear();

		for (auto x = 0; x < categorySize; ++x)
		{
			const BYTE categorySubIndex = *(BYTE*)c_pData;
			c_pData += sizeof(BYTE);

			const BYTE categorySubSize = *(BYTE*)c_pData;
			c_pData += sizeof(BYTE);

			std::vector<TIShopData> m_vec;
			m_vec.clear();

			for (auto b = 0; b < categorySubSize; ++b)
			{
				const TIShopData itemData = *(TIShopData*)c_pData;

				m_vec.emplace_back(itemData);
				c_pData += sizeof(TIShopData);
			}

			if (!m_vec.empty())
			{
				m_map.emplace(categorySubIndex, m_vec);
			}
		}

		if (!m_map.empty())
		{
			m_IShopManager.emplace(categoryIndex, m_map);
		}
	}

	if (isManuelUpdate)
	{
		const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_ref_set.begin(), c_ref_set.end(), RefreshItemShop);
	}
}
#endif

#ifdef __AUTO_HUNT__
// Metin/slot rezervasyon sistemi - bir oyuncu bir metine/slota saldýrdýðýnda rezerve edilir
bool CHARACTER_MANAGER::ReserveTarget(DWORD dwTargetVID, DWORD dwPlayerPID)
{
	// Eðer hedef zaten baþka bir oyuncu tarafýndan rezerve edilmiþse false döner
	std::map<DWORD, DWORD>::iterator it = m_mapTargetReservation.find(dwTargetVID);
	if (it != m_mapTargetReservation.end())
	{
		// Eðer ayný oyuncu tekrar rezerve etmeye çalýþýyorsa true döner (zaten rezerve edilmiþ)
		if (it->second == dwPlayerPID)
			return true;
		// Baþka bir oyuncu tarafýndan rezerve edilmiþse false döner
		return false;
	}
	
	// Rezerve et
	m_mapTargetReservation[dwTargetVID] = dwPlayerPID;
	return true;
}

void CHARACTER_MANAGER::ReleaseTarget(DWORD dwTargetVID)
{
	m_mapTargetReservation.erase(dwTargetVID);
}

void CHARACTER_MANAGER::ReleaseAllTargetsByPlayer(DWORD dwPlayerPID)
{
	std::map<DWORD, DWORD>::iterator it = m_mapTargetReservation.begin();
	while (it != m_mapTargetReservation.end())
	{
		if (it->second == dwPlayerPID)
		{
			m_mapTargetReservation.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

bool CHARACTER_MANAGER::IsTargetReserved(DWORD dwTargetVID, DWORD dwPlayerPID)
{
	std::map<DWORD, DWORD>::iterator it = m_mapTargetReservation.find(dwTargetVID);
	if (it == m_mapTargetReservation.end())
		return false; // Rezerve edilmemiþ
	
	if (dwPlayerPID == 0)
		return true; // Herhangi biri tarafýndan rezerve edilmiþ
	
	// Belirli bir oyuncu tarafýndan rezerve edilmiþ mi kontrol et
	return (it->second == dwPlayerPID);
}

DWORD CHARACTER_MANAGER::GetReserverPID(DWORD dwTargetVID)
{
	std::map<DWORD, DWORD>::iterator it = m_mapTargetReservation.find(dwTargetVID);
	if (it == m_mapTargetReservation.end())
		return 0; // Rezerve edilmemiþ
	
	return it->second; // Rezerve eden oyuncunun PID'si
}
#endif

#ifdef __DUNGEON_INFO__
bool sortByTime(const TDungeonRank& a, const TDungeonRank& b){return a.value < b.value;}
bool sortByVal(const TDungeonRank& a, const TDungeonRank& b){return a.value > b.value;}
void CHARACTER_MANAGER::SendDungeonRank(LPCHARACTER ch, DWORD mobIdx, BYTE rankIdx)
{
	const auto it = m_mapDungeonList.find(mobIdx);
	if (it == m_mapDungeonList.end())
		return;

	bool reLoad = false;

	auto itMob = m_mapDungeonRank.find(mobIdx);
	if (itMob == m_mapDungeonRank.end())
	{
		std::map<BYTE, std::pair<std::vector<TDungeonRank>, int>> m_data;
		m_mapDungeonRank.emplace(mobIdx, m_data);
		itMob = m_mapDungeonRank.find(mobIdx);
		reLoad = true;
	}

	auto itRank = itMob->second.find(rankIdx);
	if (itRank == itMob->second.end())
	{
		reLoad = true;
		std::pair<std::vector<TDungeonRank>, int> m_vec;
		itMob->second.emplace(rankIdx, m_vec);
		itRank = itMob->second.find(rankIdx);
	}

	if (!reLoad && itRank->second.second < time(0))
		reLoad = true;


	if (reLoad)
	{
		itRank->second.first.clear();
		char szQuery[1024];
		if(rankIdx == 0)
			snprintf(szQuery, sizeof(szQuery), "SELECT dwPID, lValue FROM player.quest WHERE szName = 'dungeon' and szState = '%u_completed' and lValue > 0 ORDER BY lValue DESC LIMIT 10", mobIdx);
		else if(rankIdx == 1)
			snprintf(szQuery, sizeof(szQuery), "SELECT dwPID, lValue FROM player.quest WHERE szName = 'dungeon' and szState = '%u_fastest' and lValue > 0 ORDER BY lValue ASC LIMIT 10", mobIdx);
		else if (rankIdx == 2)
			snprintf(szQuery, sizeof(szQuery), "SELECT dwPID, lValue FROM player.quest WHERE szName = 'dungeon' and szState = '%u_damage' and lValue > 0 ORDER BY lValue DESC LIMIT 10", mobIdx);

		std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
		if (pMsg->Get()->pSQLResult)
		{
			MYSQL_ROW mRow;
			while (NULL != (mRow = mysql_fetch_row(pMsg->Get()->pSQLResult)))
			{
				TDungeonRank dungeonRank;
				memset(&dungeonRank, 0, sizeof(dungeonRank));
				DWORD playerID;
				str_to_number(playerID, mRow[0]);
				str_to_number(dungeonRank.value, mRow[1]);
				snprintf(szQuery, sizeof(szQuery), "SELECT name, level FROM player.player WHERE id = %d", playerID);
				std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery(szQuery));
				MYSQL_ROW  playerRow = mysql_fetch_row(pMsg2->Get()->pSQLResult);
				if (playerRow)
				{
					strlcpy(dungeonRank.name, playerRow[0], sizeof(dungeonRank.name));
					str_to_number(dungeonRank.level, playerRow[1]);
				}
				itRank->second.first.emplace_back(dungeonRank);
			}
			if (itRank->second.first.size())
			{
				if(rankIdx == 1)
					std::sort(itRank->second.first.begin(), itRank->second.first.end(), sortByTime);
				else
					std::sort(itRank->second.first.begin(), itRank->second.first.end(), sortByVal);
			}
		}
		itRank->second.second = time(0) + (60 * 5);
	}

	std::string cmd("");

	for (BYTE j = 0; j < itRank->second.first.size(); ++j)
	{
		const auto& rank = itRank->second.first[j];

		cmd += rank.name;
		cmd += "|";
		cmd += std::to_string(rank.value);
		cmd += "|";
		cmd += std::to_string(rank.level);
		cmd += "#";
	}
	if (cmd == "")
		cmd = "-";
	ch->ChatPacket(CHAT_TYPE_COMMAND, "dungeon_log_info %u %u %s", mobIdx, rankIdx, cmd.c_str());
}
#endif

#ifdef ENABLE_EVENT_BANNER_FLAG
#include "CsvReader.h"
bool CHARACTER_MANAGER::InitializeBanners()
{
	if (m_bIsLoadedBanners)
		return false;

	const char* c_szFileName = "data/banner/list.txt";

	cCsvTable nameData;
	if (!nameData.Load(c_szFileName, '\t'))
	{
		sys_log(0, "%s couldn't be loaded or its format is incorrect.", c_szFileName);
		return false;
	}
	else
	{
		nameData.Next();
		while (nameData.Next())
		{
			if (nameData.ColCount() < 2)
				continue;

			BannerMap.insert(std::make_pair(atoi(nameData.AsStringByIndex(0)), nameData.AsStringByIndex(1)));
		}
	}
	nameData.Destroy();

	m_bIsLoadedBanners = true;

	DWORD dwFlagVnum = quest::CQuestManager::instance().GetEventFlag("banner");
	if (dwFlagVnum > 0)
		SpawnBanners(dwFlagVnum);

	return true;
}

bool CHARACTER_MANAGER::SpawnBanners(int iEnable, const char* c_szBannerName)
{
	if (!m_bIsLoadedBanners)
		InitializeBanners();

	bool bDestroy = true;
	bool bSpawn = false;

	DWORD dwBannerVnum = 0;
	std::string strBannerName;

	if (!c_szBannerName)
	{
		BannerMapType::const_iterator it = BannerMap.find(iEnable);
		if (it == BannerMap.end())
			return false;

		dwBannerVnum = it->first;
		strBannerName = it->second;
	}
	else
	{
		for (BannerMapType::const_iterator it = BannerMap.begin(); BannerMap.end() != it; ++it)
		{
			if (!strcmp(it->second.c_str(), c_szBannerName))
			{
				dwBannerVnum = it->first;
				strBannerName = it->second;
				break;
			}
		}
	}

	if (dwBannerVnum == 0 || strBannerName.empty())
		return false;

	if (iEnable > 0)
		bSpawn = true;

	if (bDestroy)
	{
		quest::CQuestManager::instance().RequestSetEventFlag("banner", 0);

		CharacterVectorInteractor i;
		CHARACTER_MANAGER::GetCharactersByRaceNum(dwBannerVnum, i);

		for (CharacterVectorInteractor::iterator it = i.begin(); it != i.end(); it++)
		{
			M2_DESTROY_CHARACTER(*it);
		}
	}

	if (bSpawn)
	{
		quest::CQuestManager::instance().RequestSetEventFlag("banner", dwBannerVnum);

		if (map_allow_find(EBannerMapIndex::EMPIRE_A))
		{
			std::string strBannerFile = "data/banner/a/" + strBannerName + ".txt";

			if (LPSECTREE_MAP pkMap = SECTREE_MANAGER::instance().GetMap(EBannerMapIndex::EMPIRE_A))
			{
				regen_do(strBannerFile.c_str(), EBannerMapIndex::EMPIRE_A, pkMap->m_setting.iBaseX, pkMap->m_setting.iBaseY, NULL, false);
			}
		}

		if (map_allow_find(EBannerMapIndex::EMPIRE_B))
		{
			std::string strBannerFile = "data/banner/b/" + strBannerName + ".txt";

			if (LPSECTREE_MAP pkMap = SECTREE_MANAGER::instance().GetMap(EBannerMapIndex::EMPIRE_B))
			{
				regen_do(strBannerFile.c_str(), EBannerMapIndex::EMPIRE_B, pkMap->m_setting.iBaseX, pkMap->m_setting.iBaseY, NULL, false);
			}
		}

		if (map_allow_find(EBannerMapIndex::EMPIRE_C))
		{
			std::string strBannerFile = "data/banner/c/" + strBannerName + ".txt";

			if (LPSECTREE_MAP pkMap = SECTREE_MANAGER::instance().GetMap(EBannerMapIndex::EMPIRE_C))
			{
				regen_do(strBannerFile.c_str(), EBannerMapIndex::EMPIRE_C, pkMap->m_setting.iBaseX, pkMap->m_setting.iBaseY, NULL, false);
			}
		}
	}

	return true;
}
#endif
