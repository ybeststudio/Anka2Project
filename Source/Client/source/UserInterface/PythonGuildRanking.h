#pragma once

#ifdef ENABLE_GUILD_RANK_SYSTEM
class CPythonGuildRanking : public CSingleton<CPythonGuildRanking>
{
private:
	struct SRankingData
	{
		SRankingData(int c_id, const char *c_guild, const char *c_name, int c_level, int c_point, int c_win, int c_draw, int c_loss)
			: id(c_id), guild(c_guild), name(c_name), level(c_level), point(c_point), win(c_win), draw(c_draw), loss(c_loss)
		{}
		std::string guild, name;
		int id, level, point, win, draw, loss;
	};
	std::vector<std::shared_ptr<SRankingData>> vRankingContainer;

public:
	enum
	{
		GUILD_RANK_PAGE_MAX_NUM = 10,
		GUILD_RANK_SHOW_COUNT = 50,
		GUILD_RANKING_MAX_NUM = 100, // game(packet.h)
	};
	CPythonGuildRanking();
	virtual ~CPythonGuildRanking();

	void RegisterRankingData(int id, const char *guild, const char *name, int level, int point, int win, int draw, int loss);
	void ClearRankData();

	size_t GetRankCount() const;
	std::uint16_t GetRankMyLine() const;
	SRankingData* GetRankByLine(std::uint16_t dwArrayIndex) const;
};
#endif // ENABLE_GUILD_RANK_SYSTEM
