#include "StdAfx.h"
#include "PythonGuildRanking.h"
#include "AbstractPlayer.h"
#include <memory>
#include <string>
#ifdef ENABLE_GUILD_RANK_SYSTEM

void CPythonGuildRanking::RegisterRankingData(int id, const char *guild, const char *name, int level, int point, int win, int draw, int loss)
{
	vRankingContainer.emplace_back(std::make_shared<SRankingData>(id, guild, name, level, point, win, draw, loss));
}

void CPythonGuildRanking::ClearRankData()
{
	vRankingContainer.clear();
	vRankingContainer.reserve(GUILD_RANKING_MAX_NUM);
}

size_t CPythonGuildRanking::GetRankCount() const
{
	return vRankingContainer.size();
}

CPythonGuildRanking::SRankingData* CPythonGuildRanking::GetRankByLine(std::uint16_t dwArrayIndex) const
{
	if (dwArrayIndex >= GetRankCount())
		return nullptr;

	return vRankingContainer.at(dwArrayIndex).get();
}

std::uint16_t CPythonGuildRanking::GetRankMyLine() const
{
	auto it = std::find_if(vRankingContainer.begin(), vRankingContainer.end(), [](const std::shared_ptr<SRankingData>& r)
	{
		return (!r->id != IAbstractPlayer::GetSingleton().GetGuildID());
	});

	if (it != vRankingContainer.end())
		return std::distance(vRankingContainer.begin(), it) + 1;

	return 0;
}

CPythonGuildRanking::CPythonGuildRanking()
{
	vRankingContainer.reserve(GUILD_RANKING_MAX_NUM);
}

CPythonGuildRanking::~CPythonGuildRanking()
{
	vRankingContainer.clear();
}

/*Module*/

PyObject *GuildRankingGetRankCount(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonGuildRanking::Instance().GetRankCount());
}

PyObject *GuildRankingGetRankByLine(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	const auto Rank = CPythonGuildRanking::Instance().GetRankByLine(iIndex);
	if (!Rank)
		return Py_BuildException("Failed to find rank by index %d", iIndex);

	return Py_BuildValue("issiiiii", Rank->id, Rank->guild.c_str(), Rank->name.c_str(), Rank->level, Rank->point, Rank->win, Rank->draw, Rank->loss);
}

PyObject *GuildRankingGetRankMyLine(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonGuildRanking::Instance().GetRankMyLine());
}

PyObject *GuildRankingClear(PyObject *poSelf, PyObject *poArgs)
{
	CPythonGuildRanking::Instance().ClearRankData();
	return Py_BuildNone();
}

void initguildranking()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetGuildRankCount", GuildRankingGetRankCount, METH_VARARGS },
		{ "GetGuildRankByLine", GuildRankingGetRankByLine, METH_VARARGS },
		{ "GetGuildRankMyLine", GuildRankingGetRankMyLine, METH_VARARGS },
		{ "GuildRankClear", GuildRankingClear, METH_VARARGS },
		{ nullptr, nullptr }
	};

	PyObject *poModule = Py_InitModule("guildranking", s_methods);
	PyModule_AddIntConstant(poModule, "GUILD_RANK_PAGE_MAX_NUM", CPythonGuildRanking::GUILD_RANK_PAGE_MAX_NUM);
	PyModule_AddIntConstant(poModule, "GUILD_RANK_SHOW_COUNT", CPythonGuildRanking::GUILD_RANK_SHOW_COUNT);
}
#endif // ENABLE_GUILD_RANK_SYSTEM
