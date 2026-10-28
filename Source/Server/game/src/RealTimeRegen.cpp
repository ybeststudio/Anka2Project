#include "stdafx.h"

#ifdef ENABLE_REAL_TIME_REGEN
#include "RealTimeRegen.hpp"
#include "sectree_manager.h"
#include "locale_service.h"
#include "char_manager.h"
#include "desc_manager.h"
#include "sectree.h"
#include "utils.h"
#include "desc.h"

#include <sstream>
#include <fstream>
#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef ENABLE_RESP_SYSTEM
#include "resp_manager.h"
#include <chrono>
#include <numeric>
#endif

static const BYTE BASE_HOUR = 60;
static const BYTE BASE_DAY = 24;

CRealTimeRegen::CRealTimeRegen()
{
	m_dwLastTick = get_global_time();
	m_dwLastMobsRefresh = 0;
	m_bIsInitialized = false;
}

void CRealTimeRegen::Initialize()
{
	// Load regen file
	LoadRegen();

	// Initializing first-next respawn time
	auto time = boost::posix_time::second_clock::local_time();
	long lHour = time.time_of_day().hours();
	long lMinute = time.time_of_day().minutes();

	for (const auto & it : mapRespawnTimes)
	{
		const long lMapIndex = std::get<REGEN_MAP_INDEX>(it.second);
		
		// If there is no such map index assigned to the core, skip it
		SECTREE_MAP	* pSectree = SECTREE_MANAGER::instance().GetMap(lMapIndex);
		if (!pSectree)
			continue;
			
		WORD wNum = it.first;
		BYTE bType = std::get<REGEN_TYPE>(it.second);
		auto vecTimes = std::get<REGEN_TIMES>(it.second);
		
		if (bType == REAL_TIME_REGEN_STANDARD)
		{
			long lAddHour = lHour + (vecTimes.at(0) / BASE_HOUR);
			long lAddMin = vecTimes.at(0) % BASE_HOUR;
			
			// Find first-next minute
			while (lAddMin <= lMinute && lAddMin != 0)
			{
				lAddMin += vecTimes.at(0) % BASE_HOUR;
				
				if (lAddMin >= BASE_HOUR)
				{
					lAddMin -= BASE_HOUR;
					lAddHour++;
					break;
				}
			}
			
			if (lAddHour >= BASE_DAY)
				lAddHour -= BASE_DAY;
			
			mapNextRespawn.insert(std::make_pair(wNum, std::make_tuple(lAddHour, lAddMin)));

#ifdef ENABLE_RESP_SYSTEM
			const DWORD dwVnum = std::get<REGEN_VNUM>(it.second);
			const long lX = std::get<REGEN_X>(it.second);
			const long lY = std::get<REGEN_Y>(it.second);

			long x = 100 * lX + pSectree->m_setting.iBaseX;
			long y = 100 * lY + pSectree->m_setting.iBaseY;

			CRespManager::instance().RegisterMob(lMapIndex, wNum, dwVnum, x, y, vecTimes.at(0) * BASE_HOUR, 0);
#endif
		}
		else
		{
			long lRespHour = vecTimes.at(0);
			
			for (const auto & lFurtherHour : vecTimes)
			{
				if (lFurtherHour > lHour)
				{
					lRespHour = lFurtherHour;
					break;
				}
			}

			mapNextRespawn.insert(std::make_pair(wNum, std::make_tuple(lRespHour, 0)));

#ifdef ENABLE_RESP_SYSTEM
			const DWORD dwVnum = std::get<REGEN_VNUM>(it.second);
			const long lX = std::get<REGEN_X>(it.second);
			const long lY = std::get<REGEN_Y>(it.second);

			long x = 100 * lX + pSectree->m_setting.iBaseX;
			long y = 100 * lY + pSectree->m_setting.iBaseY;

			int totalSeconds = std::accumulate(vecTimes.begin(), vecTimes.end(), 0, [](int sum, int hour) {
				return sum + (hour * 3600);
				});

			int meanSeconds = std::round(static_cast<double>(totalSeconds) / vecTimes.size());

			CRespManager::instance().RegisterMob(lMapIndex, wNum, dwVnum, x, y, meanSeconds, get_global_time() + SecondsToSpawn(lRespHour, 0));
#endif
		}

		if (bType == REAL_TIME_REGEN_STANDARD)
		{
			const DWORD dwVnum = std::get<REGEN_VNUM>(it.second);
			const long lX = std::get<REGEN_X>(it.second);
			const long lY = std::get<REGEN_Y>(it.second);
			const long lRangeX = std::get<REGEN_RANGE_X>(it.second);
			const long lRangeY = std::get<REGEN_RANGE_Y>(it.second);

			SpawnMonster(wNum, dwVnum, lMapIndex, lX, lY, lRangeX, lRangeY);
		}
	}
	
	m_bIsInitialized = true;
}

void CRealTimeRegen::Process()
{
	if (!m_bIsInitialized)
		return;
	
	if (get_global_time() <= m_dwLastTick)
		return;
	
	auto time = boost::posix_time::second_clock::local_time();
	long lHour = time.time_of_day().hours();
	long lMinute = time.time_of_day().minutes();
	long lSeconds = time.time_of_day().seconds();

	for (auto & it : mapNextRespawn)
	{
		TRegen regen = mapRespawnTimes[it.first];
		
		BYTE bType = std::get<REGEN_TYPE>(regen);
		
		if (lHour == std::get<0>(it.second) && lMinute >= std::get<1>(it.second) && lSeconds == 0)
		{
			auto vecTimes = std::get<REGEN_TIMES>(regen);

			long lAddHour = 0, lAddMinute = 0;
			
			if (std::get<REGEN_TYPE>(regen) == REAL_TIME_REGEN_PRECISE)
			{
				lAddHour = vecTimes.at(0);
			
				for (const auto & lFurtherHour : vecTimes)
				{
					if (lFurtherHour > lHour)
					{
						lAddHour = lFurtherHour;
						break;
					}
				}
			}
			else
			{
				lAddHour = std::get<1>(it.second)+(vecTimes.at(0) % BASE_HOUR) >= BASE_HOUR ? lHour + (vecTimes.at(0) / BASE_HOUR) + 1 : lHour + (vecTimes.at(0) / BASE_HOUR);
				lAddMinute = std::get<1>(it.second)+(vecTimes.at(0) % BASE_HOUR) >= BASE_HOUR ? std::get<1>(it.second) + (vecTimes.at(0) % BASE_HOUR) - BASE_HOUR : std::get<1>(it.second) + (vecTimes.at(0) % BASE_HOUR);
			}

			if (lAddHour >= BASE_DAY)
				lAddHour -= BASE_DAY;

			it.second = std::make_tuple(lAddHour, lAddMinute);
			
			const DWORD dwVnum = std::get<REGEN_VNUM>(regen);
			const long lMapIndex = std::get<REGEN_MAP_INDEX>(regen);
			const long lX = std::get<REGEN_X>(regen);
			const long lY = std::get<REGEN_Y>(regen);
			const long lRangeX = std::get<REGEN_RANGE_X>(regen);
			const long lRangeY = std::get<REGEN_RANGE_Y>(regen);
			
			SpawnMonster(it.first, dwVnum, lMapIndex, lX, lY, lRangeX, lRangeY);
		}
	}
	
	// Refreshing all mobs if the option is enabled
	if (bRefreshAllMobs && bRefreshAllMobsHour == lHour && lMinute == 0 && time.time_of_day().seconds() == 0 && m_dwLastMobsRefresh != get_global_time())
	{
		CHARACTER_MANAGER::Instance().RefreshAllMonsters();
		m_dwLastMobsRefresh = get_global_time();

		SendRegenNotice("REAL_TIME_REGEN_NOTICE_RESPAWN_ALL_MOBS");
	}
	
	m_dwLastTick = get_global_time();
}

bool CRealTimeRegen::LoadRegen()
{
	std::string f_name(std::string(LocaleService_GetBasePath()) + "/" + file_name);
	
	std::ifstream config(f_name.c_str());
	if (config.is_open() && config.good())
	{
		int line_num = 0;
		// num	category	vnum	count
		for (std::string line; std::getline(config, line, '\n');)
		{
			size_t pos = line.find('\t');
			while (pos != std::string::npos)
			{
				line[pos] = ' ';
				pos = line.find('\t');
			}

			line_num++;

			if (line.find('#') != std::string::npos)
				continue;

			long type, vnum, map_index, x, y, range_x, range_y;
			std::vector<long> times;
			
			std::istringstream tokens(line);

			if (!(tokens >> vnum) || !(tokens >> type) || !(tokens >> map_index) || !(tokens >> x) || !(tokens >> y) || !(tokens >> range_x) || !(tokens >> range_y))
			{
				sys_err("Error during loading RealTimeRegen, line number %d. Skipping.", line_num);
				continue;
			}
			
			long time;
			while (tokens >> time)
			{
				if (type == REAL_TIME_REGEN_PRECISE && time > BASE_DAY)
				{
					sys_err("Time is greater than 24. Line: %d. Skipping.", line_num);
					continue;
				}
				
				if (time == BASE_DAY)
					time = 0;
				
				times.push_back(time);
			}
			
			// Sort all hours
			std::sort(times.begin(), times.end());
			
			// Insert regen into the map
			mapRespawnTimes.insert(std::make_pair(line_num, std::make_tuple(type, vnum, map_index, x, y, range_x, range_y, times)));
		}
	}
	else
		sys_err("Error. Cannot load RealTimeRegen file."); 
	
	return true;
}

void CRealTimeRegen::SpawnMonster(const WORD & wNum, const DWORD & dwVnum, const long & lMapIndex, const long & lX, const long & lY, const long & lRangeX, const long & lRangeY)
{
	// Do not spawn mob, if the refresh mode is disabled and monster still exists
	if (!bRefreshRegen && CHARACTER_MANAGER::Instance().FindRealTimeRegenMonster(wNum))
		return;
	
	const TMapRegion * region = SECTREE_MANAGER::instance().GetMapRegion(lMapIndex);
	if (!region)
		return;
	
	// If the refresh option is enabled, destroy old monster and respawn new one
	if (bRefreshRegen)
		CHARACTER_MANAGER::Instance().EraseRealTimeRegenCharacter(wNum);

	// Spawn the monster
	const long lMinSpawnX = MAX(region->sx, region->sx+(lX-lRangeX)*100);
	const long lMinSpawnY = MAX(region->sy, region->sy+(lY-lRangeY)*100);
	
	const long lMaxSpawnX = MIN(region->ex, region->sx+(lX+lRangeX)*100);
	const long lMaxSpawnY = MIN(region->ey, region->sy+(lY+lRangeY)*100);

	LPCHARACTER pMonster = CHARACTER_MANAGER::Instance().SpawnMobRange(dwVnum, lMapIndex, lMinSpawnX, lMinSpawnY, lMaxSpawnX, lMaxSpawnY);
	if (!pMonster)
	{
		sys_log(0, "SpawnMonster: Monster %d has not been spawned at %d %d, map index: %d.", dwVnum, lX, lY, lMapIndex);
		return;
	}

	pMonster->SetRealTimeRegenNum(wNum);
#ifdef ENABLE_RESP_SYSTEM
	CRespManager::instance().RegisterMob(pMonster);
#endif
}

void CRealTimeRegen::SendRegenNotice(const std::string & szNotice)
{
	const DESC_MANAGER::DESC_SET & c_client_set = DESC_MANAGER::instance().GetClientSet();

	std::for_each(c_client_set.begin(), c_client_set.end(), 
			[&szNotice](LPDESC d){ if (d->GetCharacter()) d->GetCharacter()->ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_TEXT(szNotice.c_str())); });
}

#ifdef ENABLE_RESP_SYSTEM
int CRealTimeRegen::GetNextSpawnTime(const WORD& wNum)
{
	auto time = GetTime();
	long lHour = time.tm_hour;
	long lMinute = time.tm_min;

	const auto& it = mapRespawnTimes.find(wNum);
	if (it == mapRespawnTimes.end())
	{
		sys_err("Failed to find resp data for num %d", wNum);
		return 0;
	}

	const TRegen regen = it->second;
	const TRegenTime regen_time = mapNextRespawn[wNum];

	long lAddHour = std::get<0>(regen_time);
	long lAddMinute = std::get<1>(regen_time);

	return SecondsToSpawn(lAddHour, lAddMinute);
}

int CRealTimeRegen::SecondsToSpawn(const long hour, const long minute)
{
	// Get the current system time
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

	// Get the current local time
	std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
	std::tm* localTime = std::localtime(&currentTime);

	// Set the desired time
	std::tm desiredTimeTm = *localTime;
	desiredTimeTm.tm_hour = hour;
	desiredTimeTm.tm_min = minute;
	desiredTimeTm.tm_sec = 0;

	// Save unix timestamp
	std::time_t desiredTime = std::mktime(&desiredTimeTm);

	// Add 24 hours when next spawn is next day
	auto time = GetTime();
	long lHour = time.tm_hour;
	if (lHour > hour)
		desiredTime += 60 * 60 * 24;

	// Convert desired time to system clock time_point
	std::chrono::system_clock::time_point desiredTimePoint = std::chrono::system_clock::from_time_t(desiredTime);

	sys_err("teraz %d, wtedy %d (aktualna godzina: %d, docelowa: %d)", currentTime, desiredTime, lHour, hour);

	// Calculate the time remaining until the desired event
	std::chrono::duration<double> remainingTime = desiredTimePoint - now;

	// Return the remaining time
	return std::chrono::duration_cast<std::chrono::seconds>(remainingTime).count();
}

const std::tm CRealTimeRegen::GetTime()
{
	const auto global_time = get_global_time();
	return *localtime(&global_time);
}
#endif

void CRealTimeRegen::Destroy()
{
	m_bIsInitialized = false;
	
	mapRespawnTimes.clear();
	mapNextRespawn.clear();
}
#endif
