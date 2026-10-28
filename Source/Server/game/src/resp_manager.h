#pragma once

class CRespData
{
public:
	CRespData(uint32_t vnum, uint32_t x, uint32_t y, uint32_t time, time_t nextRespTime=0)
		: vnum(vnum), x(x), y(y), time(time), nextRespTime(nextRespTime)
	{}

	uint32_t vnum;
	uint32_t x;
	uint32_t y;
	uint32_t time;
	time_t nextRespTime;
};

class CRespMapData
{
public:
	using SetCharacter = std::unordered_set<LPCHARACTER>;
	using MapRespData = std::unordered_map<size_t, std::shared_ptr<CRespData>>;
	using MapMob = std::unordered_map<uint32_t, bool>;
	CRespMapData();
	~CRespMapData();

	void LoginToMap(const LPCHARACTER ch);
	void LogoutFromMap(const LPCHARACTER ch);

#ifdef ENABLE_REAL_TIME_REGEN
	bool RegisterMob(const size_t id, const uint32_t vnum, const bool is_stone, const uint32_t x, const uint32_t y, const uint32_t time, time_t nextRespTime=0);
	time_t KillMob(const uint32_t regen_id, const bool is_real_time_regen);
#else
	bool RegisterMob(const size_t id, const uint32_t vnum, const bool is_stone, const uint32_t x, const uint32_t y, const uint32_t time);
	time_t KillMob(const uint32_t regen_id);
#endif

	bool HasMob(uint32_t mobVnum);
	std::shared_ptr<CRespData>	GetRespData(size_t id);

	const SetCharacter& GetCharacterSet() { return m_setCharacter; }
	const MapRespData& GetRespData() { return m_mapRespData; }
	const MapMob& GetMapMob() { return m_mapMob; }
private:
	SetCharacter m_setCharacter;
	MapRespData m_mapRespData;
	MapMob m_mapMob;
};

class CRespManager: public singleton<CRespManager>
{
public:
	CRespManager();
	~CRespManager();

	std::shared_ptr<CRespMapData> RegisterMap(const uint32_t mapIndex);
	std::shared_ptr<CRespMapData> GetMap(const uint32_t mapIndex);

	void LoginToMap(const LPCHARACTER ch);
	void LogoutFromMap(const LPCHARACTER ch);

	void RegisterMob(const LPCHARACTER ch);
#ifdef ENABLE_REAL_TIME_REGEN
	void RegisterMob(const long mapIndex, const size_t id, const uint32_t vnum, const uint32_t x, const uint32_t y, const uint32_t time, const time_t nextRespTime);
#endif
	void KillMob(const LPCHARACTER ch);

	void FetchData(const LPCHARACTER ch, const uint8_t subheader, const uint32_t mobVnum);
	void Teleport(const LPCHARACTER ch, const size_t id);
private:
	std::unordered_map<uint32_t, std::shared_ptr<CRespMapData>> m_mapMapData;
};
