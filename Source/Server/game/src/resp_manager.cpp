#include "stdafx.h"

#ifdef ENABLE_RESP_SYSTEM
#include "char.h"
#include "regen.h"
#include "packet.h"
#include "buffer_manager.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "desc.h"
#include "wedding.h"
#include "OXEvent.h"

#ifdef ENABLE_REAL_TIME_REGEN
#include "RealTimeRegen.hpp"
#endif

#include "resp_manager.h"
#include "char_manager.h"
#include "utils.h"
CRespMapData::CRespMapData()
{
}

CRespMapData::~CRespMapData()
{
	m_setCharacter.clear();
	m_mapRespData.clear();
	m_mapMob.clear();
}

void CRespMapData::LoginToMap(const LPCHARACTER ch)
{
	if (m_setCharacter.find(ch) == m_setCharacter.end())
		m_setCharacter.insert(ch);
}

void CRespMapData::LogoutFromMap(const LPCHARACTER ch)
{
	if (m_setCharacter.find(ch) != m_setCharacter.end())
		m_setCharacter.erase(ch);
}

#ifdef ENABLE_REAL_TIME_REGEN
bool CRespMapData::RegisterMob(const size_t id, const uint32_t vnum, const bool is_stone, const uint32_t x, const uint32_t y, const uint32_t time, time_t nextRespTime)
#else
bool CRespMapData::RegisterMob(const size_t id, const uint32_t vnum, const bool is_stone, const uint32_t x, const uint32_t y, const uint32_t time)
#endif
{	
	if (m_mapMob.find(vnum) == m_mapMob.end())
		m_mapMob.emplace(vnum, is_stone);

	const auto& it = m_mapRespData.find(id);
	if (it == m_mapRespData.end())
	{
#ifdef ENABLE_REAL_TIME_REGEN
		std::shared_ptr<CRespData> respData = std::make_shared<CRespData>(vnum, x, y, time, nextRespTime);
#else
		std::shared_ptr<CRespData> respData = std::make_shared<CRespData>(vnum, x, y, time, 0);
#endif
		m_mapRespData.insert(std::make_pair(id, respData));

		return false;
	}

	const auto& respData = it->second;

	respData->x = x;
	respData->y = y;
	if (time)
		respData->time = time;
#ifdef ENABLE_REAL_TIME_REGEN
	if (nextRespTime)
		respData->nextRespTime = nextRespTime;
#endif

	return true;
}

#ifdef ENABLE_REAL_TIME_REGEN
time_t CRespMapData::KillMob(const uint32_t regen_id, const bool is_real_time_regen)
#else
time_t CRespMapData::KillMob(const uint32_t regen_id)
#endif
{
	const auto& it = m_mapRespData.find(regen_id);

	if (it == m_mapRespData.end())
	{
		sys_err("Didnt find mob with regen_id %d", regen_id);
		return 0;
	}

	const auto& respData = it->second;

#ifdef ENABLE_REAL_TIME_REGEN
	if (is_real_time_regen)
		respData->nextRespTime = time(nullptr) + CRealTimeRegen::instance().GetNextSpawnTime(regen_id);
	else
		respData->nextRespTime = time(nullptr) + respData->time;
#else
	respData->nextRespTime = respData->time + time(nullptr);
#endif

	return respData->nextRespTime;
}

bool CRespMapData::HasMob(uint32_t mobVnum)
{
	return std::any_of(m_mapRespData.begin(), m_mapRespData.end(),
		[mobVnum](const auto& it) 
		{ 
			return it.second->vnum == mobVnum; 
		}
	);
}

std::shared_ptr<CRespData> CRespMapData::GetRespData(size_t id)
{
	const auto& it = m_mapRespData.find(id);
	if (it != m_mapRespData.end())
		return it->second;

	return nullptr;
}

CRespManager::CRespManager()
{

}

CRespManager::~CRespManager()
{
	m_mapMapData.clear();
}

std::shared_ptr<CRespMapData> CRespManager::RegisterMap(const uint32_t mapIndex)
{
	if (m_mapMapData.find(mapIndex) != m_mapMapData.end())
		return nullptr;

	std::shared_ptr<CRespMapData> mapData = std::make_shared<CRespMapData>();
	m_mapMapData.emplace(mapIndex, mapData);

	sys_log(0, "RESP_MANAGER: map %d registered", mapIndex);

	return mapData;
}

std::shared_ptr<CRespMapData> CRespManager::GetMap(const uint32_t mapIndex)
{
	const auto& it = m_mapMapData.find(mapIndex);

	if (it == m_mapMapData.end())
		return nullptr;

	return it->second;
}

void CRespManager::LoginToMap(const LPCHARACTER ch)
{
	auto mapData = GetMap(ch->GetMapIndex());
	if (!mapData)
		mapData = RegisterMap(ch->GetMapIndex());

	if (!mapData)
		return;

	mapData->LoginToMap(ch);

	if (ch->GetDesc())
	{
		const auto& mapMob = mapData->GetMapMob();
		uint8_t mobCount = static_cast<uint8_t>(mapMob.size());

		const auto& respData = mapData->GetRespData();
		const auto& now = time(nullptr);

		TEMP_BUFFER buf;

		TPacketGCRespHeader header{};
		header.header = HEADER_GC_RESP;
		header.size = sizeof(TPacketGCRespHeader) + sizeof(TPacketGCMapData) + sizeof(uint32_t) * mobCount;
		header.subheader = RESP_GC_SUBHEADER_DATA_MOB;

		TPacketGCMapData pack{};
		pack.mobCount = mobCount;

		for (const auto& it : respData)
		{
			const auto& data = it.second;
			const auto& isStone = mapMob.at(data->vnum);

			if (isStone)
			{
				pack.maxMetinCount++;
				if (data->nextRespTime <= now)
					pack.currentMetinCount++;
			}
			else
			{
				pack.maxBossCount++;
				if (data->nextRespTime <= now)
					pack.currentBossCount++;
			}
		}

		buf.write(&header, sizeof(TPacketGCRespHeader));
		buf.write(&pack, sizeof(TPacketGCMapData));
		for (const auto& it : mapMob)
			buf.write(&it.first, sizeof(uint32_t));

		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
	}

	sys_log(0, "RESP_MANAGER: player %s login to map %d", ch->GetName(), ch->GetMapIndex());
}

void CRespManager::LogoutFromMap(const LPCHARACTER ch)
{
	const auto& mapData = GetMap(ch->GetMapIndex());
	if (!mapData)
		return;

	mapData->LogoutFromMap(ch);

	sys_log(0, "RESP_MANAGER: player %s logout from map %d", ch->GetName(), ch->GetMapIndex());
}

void CRespManager::RegisterMob(const LPCHARACTER ch)
{
	auto mapData = GetMap(ch->GetMapIndex());
	if (!mapData)
		mapData = RegisterMap(ch->GetMapIndex());

	if ((ch->IsMonster() && ch->GetMobRank() >= MOB_RANK_BOSS) || ch->IsStone())
	{
#ifdef ENABLE_REAL_TIME_REGEN
		size_t id = 0;
		uint32_t time = 0;
		PIXEL_POSITION position{};

		bool is_real_time_regen = ch->GetRealTimeRegenNum();

		if (is_real_time_regen)
		{
			position = ch->GetXYZ();
			id = ch->GetRealTimeRegenNum();
		}
		else
		{
			const auto& regen = ch->GetRegen();
			if (!regen)
				return;

			position = ch->GetRegenPos();
			id = regen->id;
			time = regen->time;
		}
#else
		const auto& regen = ch->GetRegen();
		if (!regen)
			return;

		const auto& position = ch->GetRegenPos();

		size_t id = regen->id;
		uint32_t time = regen->time;
#endif
		const auto& update = mapData->RegisterMob(id, ch->GetRaceNum(), ch->IsStone(), position.x, position.y, time);
		if (update)
		{
			TEMP_BUFFER buf;

			TPacketGCRespHeader header{};
			header.header = HEADER_GC_RESP;
			header.size = sizeof(TPacketGCRespHeader) + sizeof(TPacketGCRespRefresh);
			header.subheader = RESP_GC_SUBHEADER_REFRESH_RESP;

			TPacketGCRespRefresh pack{};
			pack.id = id;
			pack.vnum = ch->GetRaceNum();
			pack.nextRespTime = 0;
			pack.x = position.x;
			pack.y = position.y;

			buf.write(&header, sizeof(TPacketGCRespHeader));
			buf.write(&pack, sizeof(TPacketGCRespRefresh));

			for (const auto& it : mapData->GetCharacterSet())
			{
				if (!it || !it->GetDesc())
					continue;

				it->GetDesc()->BufferedPacket(buf.read_peek(), buf.size());
			}
		}

//		sys_log(0, "RESP_MANAGER: mob %d registered to map %d, regen id %d", ch->GetRaceNum(), ch->GetMapIndex(), id);
	}
}

#ifdef ENABLE_REAL_TIME_REGEN
void CRespManager::RegisterMob(const long mapIndex, const size_t id, const uint32_t vnum, const uint32_t x, const uint32_t y, const uint32_t time, const time_t nextRespTime)
{
	auto mapData = GetMap(mapIndex);
	if (!mapData)
		mapData = RegisterMap(mapIndex);

	const CMob* pkMob = CMobManager::instance().Get(vnum);
	if (!pkMob)
		return;

	sys_log(0, "RESP_MANAGER: RegisterMob map %d, id %d, vnum %d, time %d, next_resp %d (%d)", mapIndex, id, vnum, time, nextRespTime, get_global_time());

	const bool is_stone = pkMob->m_table.bType == CHAR_TYPE_STONE;

	if ((pkMob->m_table.bType == CHAR_TYPE_MONSTER && pkMob->m_table.bRank >= MOB_RANK_BOSS) || is_stone)
	{
		const auto& update = mapData->RegisterMob(id, vnum, is_stone, x, y, time, nextRespTime);
		if (update)
		{
			TEMP_BUFFER buf;

			TPacketGCRespHeader header{};
			header.header = HEADER_GC_RESP;
			header.size = sizeof(TPacketGCRespHeader) + sizeof(TPacketGCRespRefresh);
			header.subheader = RESP_GC_SUBHEADER_REFRESH_RESP;

			TPacketGCRespRefresh pack{};
			pack.id = id;
			pack.vnum = vnum;
			pack.nextRespTime = nextRespTime;
			pack.x = x;
			pack.y = y;

			buf.write(&header, sizeof(TPacketGCRespHeader));
			buf.write(&pack, sizeof(TPacketGCRespRefresh));

			for (const auto& it : mapData->GetCharacterSet())
			{
				if (!it || !it->GetDesc())
					continue;

				it->GetDesc()->Packet(buf.read_peek(), buf.size());
			}
		}

//		sys_log(0, "RESP_MANAGER: mob %d registered to map %d, real time regen id %d", vnum, mapIndex, id);
	}
}
#endif

void CRespManager::KillMob(const LPCHARACTER ch)
{
	const auto& mapData = GetMap(ch->GetMapIndex());
	if (!mapData)
		return;

#ifdef ENABLE_REAL_TIME_REGEN
	PIXEL_POSITION position{};
	size_t id{};
	bool is_real_time_regen = ch->GetRealTimeRegenNum();

	sys_log(0, "RESP_MANAGER: killed mob %s, is_real_time_regen %d", ch->GetName(), is_real_time_regen);

	if (is_real_time_regen)
	{
		position = ch->GetXYZ();
		id = ch->GetRealTimeRegenNum();
	}
	else
	{
		const auto& regen = ch->GetRegen();
		if (!regen)
			return;

		position = ch->GetRegenPos();
		id = regen->id;
	}

	const auto& respTime = mapData->KillMob(id, is_real_time_regen);

	if (!respTime)
		return;
#else
	const auto& regen = ch->GetRegen();
	if (!regen)
		return;

	const size_t id = regen->id;

	const auto& position = ch->GetRegenPos();

	const auto& respTime = mapData->KillMob(id);

	if (!respTime)
		return;
#endif
	const auto& characterSet = mapData->GetCharacterSet();

	sys_log(0, "RESP_MANAGER: mob %d killed on map %d (%d, %d), guests %d, mob id %d, resp time %d", ch->GetRaceNum(), ch->GetMapIndex(), position.x, position.y, characterSet.size(), id, respTime);

	TEMP_BUFFER buf;

	TPacketGCRespHeader header{};
	header.header = HEADER_GC_RESP;
	header.size = sizeof(TPacketGCRespHeader) + sizeof(TPacketGCRespRefresh);
	header.subheader = RESP_GC_SUBHEADER_REFRESH_RESP;

	TPacketGCRespRefresh pack{};
	pack.id = id;
	pack.vnum = ch->GetRaceNum();
	pack.nextRespTime = respTime;
	pack.x = position.x;
	pack.y = position.y;

	buf.write(&header, sizeof(TPacketGCRespHeader));
	buf.write(&pack, sizeof(TPacketGCRespRefresh));

	for (const auto& it : mapData->GetCharacterSet())
	{
		if (!it || !it->GetDesc())
			continue;

		it->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}

void CRespManager::FetchData(const LPCHARACTER ch, const uint8_t subheader, const uint32_t mobVnum)
{
	auto mapData = GetMap(ch->GetMapIndex());
	if (!mapData)
		mapData = RegisterMap(ch->GetMapIndex());

	if (!mapData)
		return;

	TEMP_BUFFER buf;
	buf.write(&mobVnum, sizeof(uint32_t));

	uint8_t outputSubheader = 0;
	uint16_t itemCount = 0;

	// If mob is not registered, send empty response
	if (!mapData->HasMob(mobVnum))
	{
		if (subheader == RESP_CG_SUBHEADER_FETCH_RESP)
		{
			outputSubheader = RESP_GC_SUBHEADER_DATA_RESP;
		}
		else if (subheader == RESP_CG_SUBHEADER_FETCH_DROP)
		{
			outputSubheader = RESP_GC_SUBHEADER_DATA_DROP;
			TPacketGCRespGold gold{};
			gold.minGold = 0;
			gold.maxGold = 0;
			buf.write(&gold, sizeof(TPacketGCRespGold));
		}

		TPacketGCRespHeader header{};
		header.header = HEADER_GC_RESP;
		header.size = sizeof(TPacketGCRespHeader) + buf.size() + sizeof(uint16_t);
		header.subheader = outputSubheader;

		ch->GetDesc()->BufferedPacket(&header, sizeof(TPacketGCRespHeader));
		ch->GetDesc()->BufferedPacket(&itemCount, sizeof(uint16_t));
		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
		return;
	}

	if (subheader == RESP_CG_SUBHEADER_FETCH_RESP)
	{
		outputSubheader = RESP_GC_SUBHEADER_DATA_RESP;
		for (const auto& it : mapData->GetRespData())
		{
			const auto& respData = it.second;

			if (respData->vnum != mobVnum)
				continue;

			itemCount++;

			TPacketGCRespData pack{};
			pack.id = it.first;
			pack.x = respData->x;
			pack.y = respData->y;
			pack.time = respData->time;
			pack.nextRespTime = respData->nextRespTime;

			sys_log(0, "Mob id %d, time %d, resp time %lld", it.first, respData->time, respData->nextRespTime);

			buf.write(&pack, sizeof(TPacketGCRespData));
		}
	}
	else if (subheader == RESP_CG_SUBHEADER_FETCH_DROP)
	{
		outputSubheader = RESP_GC_SUBHEADER_DATA_DROP;

		const CMob* pkMob = CMobManager::instance().Get(mobVnum);
		if (!pkMob)
			return;

		const auto& target = CHARACTER_MANAGER::instance().CreateCharacter(pkMob->m_table.szLocaleName);
		target->SetProto(pkMob);

		struct TDropInfoItem
		{
			DWORD dwVnum;
			BYTE byMinCount;
			BYTE byMaxCount;
		};

		std::vector<TDropInfoItem> vecDropInfo;
		std::vector<std::pair<int, int>> vecItemPair;
		const auto bHasDropInfo = ITEM_MANAGER::Instance().CreateDropItemVector(target, ch, vecItemPair);
		M2_DESTROY_CHARACTER(target);

		// Convert pair<int, int> to TDropInfoItem
		if (bHasDropInfo)
		{
			for (const auto& it : vecItemPair)
			{
				TDropInfoItem dropInfo{};
				dropInfo.dwVnum = static_cast<DWORD>(it.first);
				dropInfo.byMinCount = static_cast<BYTE>(it.second);
				dropInfo.byMaxCount = static_cast<BYTE>(it.second);
				vecDropInfo.push_back(dropInfo);
			}
		}

		TPacketGCRespGold gold{};
		gold.minGold = pkMob->m_table.dwGoldMin;
		gold.maxGold = pkMob->m_table.dwGoldMax;

		buf.write(&gold, sizeof(TPacketGCRespGold));

		if (bHasDropInfo)
		{
			for (const auto& it : vecDropInfo)
			{
				itemCount++;

				TPacketGCRespItem item{};
				item.vnum = it.dwVnum;
				item.minCount = it.byMinCount;
				item.maxCount = it.byMaxCount;

				buf.write(&item, sizeof(TPacketGCRespItem));
			}
		}
	}

	TPacketGCRespHeader header{};
	header.header = HEADER_GC_RESP;
	header.size = sizeof(TPacketGCRespHeader) + buf.size() + sizeof(uint16_t);
	header.subheader = outputSubheader;

	ch->GetDesc()->BufferedPacket(&header, sizeof(TPacketGCRespHeader));
	ch->GetDesc()->BufferedPacket(&itemCount, sizeof(uint16_t));
	ch->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CRespManager::Teleport(const LPCHARACTER ch, const size_t id)
{
	long mapIndex = ch->GetMapIndex();

	const auto& mapData = GetMap(mapIndex);
	if (!mapData)
	{
		sys_log(0, "RESP_TELEPORT: map data not found for map %ld", mapIndex);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Resp manager> Map data not found."));
		return;
	}

	const auto& respData = mapData->GetRespData(id);
	if (!respData)
	{
		sys_log(0, "RESP_TELEPORT: resp data not found for id %zu on map %ld", id, mapIndex);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Resp manager> Target location not found."));
		return;
	}

	if (ch->GetDungeon() || ch->GetWarMap() || mapIndex == OXEVENT_MAP_INDEX || mapIndex == marriage::WEDDING_MAP_INDEX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Resp manager> You cannot teleport on this map."));
		return;
	}

	time_t nextActionTime = ch->GetQuestFlag("__resp_manager.next_action");

	auto deltaTime = nextActionTime - get_global_time();
	if (deltaTime > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Resp manager> Wait %lu seconds."), deltaTime);
		return;
	}

#ifdef __RESET_PLAYER_DAMAGE__
	ch->ForgetMyAttacker(true);
#endif

	int teleportX = respData->x + number(-100, 100);
	int teleportY = respData->y + number(-100, 100);
	
	sys_log(0, "RESP_TELEPORT: player %s teleporting to map %ld, x %d, y %d (id %zu)", 
		ch->GetName(), mapIndex, teleportX, teleportY, id);

	ch->Show(ch->GetMapIndex(), teleportX, teleportY);
	ch->Stop();

	ch->SetQuestFlag("__resp_manager.next_action", get_global_time() + RESP_TELEPORT_COOLDOWN);
}
#endif
