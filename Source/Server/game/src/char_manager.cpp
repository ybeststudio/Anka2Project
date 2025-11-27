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
#include "item_manager.h"

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
#ifdef ENABLE_RENEWAL_INGAME_ITEMSHOP
	m_IShopManager.clear();
#endif

	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.begin();
	while (it != m_map_pkChrByVID.end())
	{
		LPCHARACTER ch = it->second;
		M2_DESTROY_CHARACTER(ch);
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
	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.find(ch->GetVID());
	if (it == m_map_pkChrByVID.end())
	{
		sys_err("[CHARACTER_MANAGER::DestroyCharacter] <Factor> %d not found", (long)(ch->GetVID()));
		return; // prevent duplicated destrunction
	}

	if (ch->IsNPC() && !ch->IsPet()
#ifdef ENABLE_GROWTH_PET_SYSTEM
		&& !ch->IsGrowthPet()
        #ifdef ENABLE_BOT_PLAYER
		&& !ch->IsBotCharacter()
        #endif
#endif
		&& ch->GetRider() == NULL
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
	itertype(m_map_pkChrByVID) it = m_map_pkChrByVID.find(dwVID);

	if (m_map_pkChrByVID.end() == it)
		return NULL;

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

	LPCHARACTER found = it->second;
	if (found != NULL && strncasecmp(szName, found->GetName(), CHARACTER_NAME_MAX_LEN) != 0)
	{
		sys_err("[CHARACTER_MANAGER::FindPC] <Factor> %s != %s", name, found->GetName());
		return NULL;
	}
	return found;
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRandomPosition(DWORD dwVnum, long lMapIndex)
{
	{
		if (dwVnum == 5001 && !quest::CQuestManager::instance().GetEventFlag("japan_regen"))
		{
			sys_log(1, "WAEGU[5001] regen disabled.");
			return NULL;
		}
	}

	{
		if (dwVnum == 5002 && !quest::CQuestManager::instance().GetEventFlag("newyear_mob"))
		{
			sys_log(1, "HAETAE (new-year-mob) [5002] regen disabled.");
			return NULL;
		}
	}

	{
		if (dwVnum == 5004 && !quest::CQuestManager::instance().GetEventFlag("independence_day"))
		{
			sys_log(1, "INDEPENDECE DAY [5004] regen disabled.");
			return NULL;
		}
	}

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

	if (CMobVnumHelper::IsNPCType(pkMob->m_table.bType))
	{
		if (ch->GetEmpire() == 0)
			ch->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(lMapIndex));
	}

	ch->SetRotation(number(0, 360));

	if (!ch->Show(lMapIndex, x, y, 0, false))
	{
		M2_DESTROY_CHARACTER(ch);
		sys_err(0, "SpawnMobRandomPosition: cannot show monster");
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
#ifdef ENABLE_SHIP_DEFENCE_DUNGEON
		|| dwVnum == 6801
#endif
	)
		bSpawnMotion = true;

	int i = 16;

	while (i--)
	{
		int x = number(sx, ex);
		int y = number(sy, ey);

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
		sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
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
		sys_err("NOT_EXIST_GROUP_VNUM(%u) Map(%u) ", dwVnum, lMapIndex);
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
	}

	return chLeader;
}

struct FuncUpdateAndResetChatCounter
{
	void operator () (LPCHARACTER ch)
	{
		ch->ResetChatCounter();
		ch->CFSM::Update();
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

#define FIND_JOB_WARRIOR_0 (1 << 3)
#define FIND_JOB_WARRIOR_1 (1 << 4)
#define FIND_JOB_WARRIOR_2 (1 << 5)
#define FIND_JOB_WARRIOR (FIND_JOB_WARRIOR_0 | FIND_JOB_WARRIOR_1 | FIND_JOB_WARRIOR_2)
#define FIND_JOB_ASSASSIN_0 (1 << 6)
#define FIND_JOB_ASSASSIN_1 (1 << 7)
#define FIND_JOB_ASSASSIN_2 (1 << 8)
#define FIND_JOB_ASSASSIN (FIND_JOB_ASSASSIN_0 | FIND_JOB_ASSASSIN_1 | FIND_JOB_ASSASSIN_2)
#define FIND_JOB_SURA_0 (1 << 9)
#define FIND_JOB_SURA_1 (1 << 10)
#define FIND_JOB_SURA_2 (1 << 11)
#define FIND_JOB_SURA (FIND_JOB_SURA_0 | FIND_JOB_SURA_1 | FIND_JOB_SURA_2)
#define FIND_JOB_SHAMAN_0 (1 << 12)
#define FIND_JOB_SHAMAN_1 (1 << 13)
#define FIND_JOB_SHAMAN_2 (1 << 14)
#define FIND_JOB_SHAMAN (FIND_JOB_SHAMAN_0 | FIND_JOB_SHAMAN_1 | FIND_JOB_SHAMAN_2)

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
		for ( ; it != end; ++it) {
			M2_DESTROY_CHARACTER(*it);
		}

		m_set_pkChrPendingDestroy.clear();
	}
}

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
		if (farmPlayerCount >= 3)
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

#ifdef ENABLE_RENEWAL_INGAME_ITEMSHOP
#include "desc_client.h"
#include "desc_manager.h"

void CHARACTER_MANAGER::LoadItemShopLogReal(LPCHARACTER ch, const char* c_pData)
{
	if (!ch)
		return;

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

	if(logCount)
		buf.write(m_vec.data(), sizeof(TIShopLogData)* logCount);

	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CHARACTER_MANAGER::LoadItemShopLog(LPCHARACTER ch)
{
	BYTE subIndex = ITEMSHOP_LOG;
	DWORD accountID = ch->GetDesc()->GetAccountTable().id;

	db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP, ch->GetDesc()->GetHandle(), sizeof(BYTE)+sizeof(DWORD));
	db_clientdesc->Packet(&subIndex, sizeof(BYTE));
	db_clientdesc->Packet(&accountID, sizeof(DWORD));
}

void CHARACTER_MANAGER::LoadItemShopData(LPCHARACTER ch, bool isAll)
{
	TEMP_BUFFER buf;
	TPacketGCItemShop p;
	p.header = HEADER_GC_ITEMSHOP;

	long long dragonCoin = ch->GetCoins();

	if (isAll)
	{
		int calculateSize = 0;
		BYTE subIndex = ITEMSHOP_LOAD;
		calculateSize += sizeof(BYTE);

		calculateSize += sizeof(long long);
		calculateSize += sizeof(int);

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
							calculateSize += sizeof(TIShopData) * categorySubSize;
					}
				}
			}
		}

		p.size = sizeof(TPacketGCItemShop) + calculateSize;

		buf.write(&p, sizeof(TPacketGCItemShop));
		buf.write(&subIndex, sizeof(BYTE));
		buf.write(&dragonCoin, sizeof(long long));
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
							buf.write(itEx->second.data(), sizeof(TIShopData) * categorySubSize);
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
		buf.write(&dragonCoin, sizeof(long long));
		buf.write(&itemshopUpdateTime, sizeof(int));
		int categoryTotalSize = 9999;
		buf.write(&categoryTotalSize, sizeof(int));
	}
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CHARACTER_MANAGER::LoadItemShopBuyReal(LPCHARACTER ch, const char* c_pData)
{
	if (!ch)
		return;

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

	const bool isOpenLog = *(bool*)c_pData;
	c_pData += sizeof(bool);

	const DWORD itemVnum = *(DWORD*)c_pData;
	c_pData += sizeof(DWORD);

	const int itemCount = *(int*)c_pData;
	c_pData += sizeof(int);

	const long long itemPrice = *(long long*)c_pData;
	c_pData += sizeof(long long);

	// DB tarafýnda coin düþüþü yapýldý, þimdi güncel coin deðerini al ve POINT_COINS'i güncelle
	long long dragonCoin = ch->GetCoins();
	ch->SetPoint(POINT_COINS, dragonCoin);

#ifndef ENABLE_ITEMSHOP_TO_INVENTORY
	// Item Shop'tan alýnan item'larý mall deposuna ekle
	if (!ch->GetMall())
	{
		// Mall açýk deðilse oluþtur (LoadMall boþ item listesi ile çaðrýlarak mall oluþturulur)
		ch->LoadMall(0, NULL);
	}

	// Item'ý oluþtur
	LPITEM item = ITEM_MANAGER::instance().CreateItem(itemVnum, itemCount);
	if (item && ch->GetMall())
	{
		// Mall'da boþ slot bul ve ekle
		bool bAdded = false;
		for (DWORD pos = 0; pos < 3 * SAFEBOX_PAGE_SIZE; ++pos)
		{
			if (ch->GetMall()->IsValidPosition(pos) && ch->GetMall()->IsEmpty(pos, item->GetSize()))
			{
				if (ch->GetMall()->Add(pos, item))
				{
					item->SetSkipSave(false);
					bAdded = true;
					break;
				}
			}
		}
		
		// Eðer eklenemediyse item'ý yok et
		if (!bAdded)
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().RemoveItem(item, "ITEMSHOP_MALL_FULL");
		}
	}
	else if (!item)
	{
		sys_err("cannot create item vnum %d (name: %s)", itemVnum, ch->GetName());
	}
	else if (!ch->GetMall())
	{
		// Mall oluþturulamadýysa normal envantere ekle
		ch->AutoGiveItem(itemVnum, itemCount);
	}
#endif

	TEMP_BUFFER buf;
	TPacketGCItemShop p;
	p.header = HEADER_GC_ITEMSHOP;
	p.size = sizeof(TPacketGCItemShop) + sizeof(BYTE) + sizeof(long long) + sizeof(bool);

	if(isOpenLog)
		p.size += sizeof(TIShopLogData);

	BYTE subIndex = ITEMSHOP_DRAGONCOIN;

	buf.write(&p, sizeof(TPacketGCItemShop));
	buf.write(&subIndex, sizeof(BYTE));
	buf.write(&dragonCoin, sizeof(long long));
	buf.write(&isOpenLog, sizeof(bool));
	if (isOpenLog)
	{
		const TIShopLogData logData = *(TIShopLogData*)c_pData;
		c_pData += sizeof(TIShopLogData);

		buf.write(&logData, sizeof(TIShopLogData));
	}
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());

	if(itemCount > 1)
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 691, "%d#%d#%lld", itemVnum, itemCount, itemPrice);
	else
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 692, "%d#%lld", itemVnum, itemPrice);
}

void CHARACTER_MANAGER::LoadItemShopBuy(LPCHARACTER ch, int itemID, int itemCount)
{
	if (itemCount <= 0 || itemCount > 20)
		return;

	if (m_IShopManager.size())
	{
		for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
		{
			if (it->second.size())
			{
				for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
				{
					if (itEx->second.size())
					{
						for (auto itReal = itEx->second.begin(); itReal != itEx->second.end(); ++itReal)
						{
							const TIShopData& itemData = *itReal;
							if (itemData.id == itemID)
							{
								long long dragonCoin = ch->GetCoins();
								long long itemPrice = itemData.itemPrice * itemCount;
								if (itemData.discount > 0)
									itemPrice = long((float(itemData.itemPrice) / 100.0) * float(100 - itemData.discount));

								if (itemPrice > dragonCoin)
								{
									ch->LocaleChatPacket(CHAT_TYPE_INFO, 687, "");
									return;
								}

								DWORD accountID = ch->GetDesc()->GetAccountTable().id;
								BYTE subIndex = ITEMSHOP_BUY;
								char playerName[CHARACTER_NAME_MAX_LEN + 1];
								strlcpy(playerName, ch->GetName(), sizeof(playerName));

								char ipAdress[16];
								strlcpy(ipAdress, ch->GetDesc()->GetHostName(), sizeof(ipAdress));

								TEMP_BUFFER buf;
								buf.write(&subIndex, sizeof(BYTE));
								buf.write(&accountID, sizeof(DWORD));
								buf.write(&playerName, sizeof(playerName));
								buf.write(&ipAdress, sizeof(ipAdress));
								buf.write(&itemID, sizeof(int));
								buf.write(&itemCount, sizeof(int));
								bool isLogOpen = (ch->GetProtectTime("itemshop.log") == 1)?true:false;
								buf.write(&isLogOpen, sizeof(bool));

								db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP, ch->GetDesc()->GetHandle(), buf.size());
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

	if (m_IShopManager.size())
	{
		for (auto it = m_IShopManager.begin(); it != m_IShopManager.end(); ++it)
		{
			if (sendPacketProcess)
				break;
			if (it->second.size())
			{
				for (auto itEx = it->second.begin(); itEx != it->second.end(); ++itEx)
				{
					if (sendPacketProcess)
						break;
					if (itEx->second.size())
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
	LPCHARACTER ch = d->GetCharacter();

	if (!ch)
		return;

	if (ch->GetProtectTime("itemshop.load") == 1)
	{
		ch->LocaleChatPacket(CHAT_TYPE_INFO, 686, "");
		CHARACTER_MANAGER::Instance().LoadItemShopData(ch, true);
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

	for (DWORD j = 0; j < categoryTotalSize; ++j)
	{
		const BYTE categoryIndex = *(BYTE*)c_pData;
		c_pData += sizeof(BYTE);
		const BYTE categorySize = *(BYTE*)c_pData;
		c_pData += sizeof(BYTE);

		std::map<BYTE, std::vector<TIShopData>> m_map;
		m_map.clear();

		for (DWORD x = 0; x < categorySize; ++x)
		{
			const BYTE categorySubIndex = *(BYTE*)c_pData;
			c_pData += sizeof(BYTE);

			const BYTE categorySubSize = *(BYTE*)c_pData;
			c_pData += sizeof(BYTE);

			std::vector<TIShopData> m_vec;
			m_vec.clear();

			for (DWORD b = 0; b < categorySubSize; ++b)
			{
				const TIShopData itemData = *(TIShopData*)c_pData;

				m_vec.emplace_back(itemData);
				c_pData += sizeof(TIShopData);
			}

			if(m_vec.size())
				m_map.emplace(categorySubIndex, m_vec);
		}
		if(m_map.size())
			m_IShopManager.emplace(categoryIndex, m_map);
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
