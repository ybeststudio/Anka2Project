#include "StdAfx.h"
#include "PythonDungeonInfo.h"
#include "PythonNetworkStream.h"
#include "Packet.h"
#ifdef ENABLE_DUNGEON_INFO

CPythonDungeonInfo::CPythonDungeonInfo()
{
	
	Clear();
}

CPythonDungeonInfo::~CPythonDungeonInfo()
{
	Clear();
}
 

void CPythonDungeonInfo::Open()
{
	
}

void CPythonDungeonInfo::Clear()
{
	SetPageCount(1);
	date_dungeon.clear();
	date_dungeon_mision.clear();

}

void CPythonDungeonInfo::ClearMision()
{
	date_dungeon_mision.clear();
}

void CPythonDungeonInfo::DateInfoList(const TFDateInfoPacket & infodungeon)
{
	date_dungeon.push_back(infodungeon);
}

void CPythonDungeonInfo::DateInfoMisionList(const TFDateMision & infodungeonm)
{
	date_dungeon_mision.push_back(infodungeonm);
}

void CPythonDungeonInfo::DateInfoRankingList(const TFDateInfoRanking & infodungeon_ranking)
{
	date_dungeon_ranking.push_back(infodungeon_ranking);
}

void CPythonDungeonInfo::ClearRanking()
{
	date_dungeon_ranking.clear();
}


int CPythonDungeonInfo::GetSizeList()
{
	return date_dungeon.size(); 
}

int CPythonDungeonInfo::GetSizeRankingList()
{
	return date_dungeon_ranking.size();
}

const CPythonDungeonInfo::TDungeonInfo & CPythonDungeonInfo::GetDateInfo()
{
	return date_dungeon;
}

const CPythonDungeonInfo::TDungeonMisionInfo & CPythonDungeonInfo::GetDateInfoMision()
{
	return date_dungeon_mision;
}

const CPythonDungeonInfo::TDungeonRankingInfo & CPythonDungeonInfo::GetDateInfoRanking()
{
	return date_dungeon_ranking;
}

PyObject* OpenDungeonInfoSystem(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendDungeonInfoOpen();
	return Py_BuildNone();
}

PyObject* TeleportDungeonInfoSystem(PyObject* poSelf, PyObject* poArgs)
{

	int index_teleport;
	if (!PyTuple_GetInteger(poArgs, 0, &index_teleport))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendDungeonInfoTeleport(index_teleport);
	return Py_BuildNone();
}


PyObject* MisionDungeonInfoSystem(PyObject* poSelf, PyObject* poArgs)
{

	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendDungeonInfoMision(index);
	return Py_BuildNone();
}

PyObject* RankingDungeonInfoSystem(PyObject* poSelf, PyObject* poArgs)
{

	int index_dungeon;
	if (!PyTuple_GetInteger(poArgs, 0, &index_dungeon))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendDungeonInfoRanking(index_dungeon);
	return Py_BuildNone();
}


PyObject * GetDungeonInfoCount(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonDungeonInfo::Instance().GetSizeList());
}


PyObject * GetDungeonInfoCountRanking(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonDungeonInfo::Instance().GetSizeRankingList());
}


PyObject* SetPageCount(PyObject * poSelf, PyObject * poArgs)
{
	int page_count;
	if (!PyTuple_GetInteger(poArgs, 0, &page_count))
		return Py_BuildException();

	CPythonDungeonInfo::Instance().SetPageCount(page_count);

	return Py_BuildNone();
}

PyObject * GetPageCount(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonDungeonInfo::Instance().GetPageCount());
}

PyObject * GetPageView(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonDungeonInfo::Instance().GetPageView());
}

PyObject * GetDungeonInfoDate(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	const CPythonDungeonInfo::TDungeonInfo & DungeonInfoVector = CPythonDungeonInfo::Instance().GetDateInfo();
	if (DWORD(index) >= DungeonInfoVector.size())
		return Py_BuildValue("issiiiiiiisssiiiiiiii",0,"None","None",0,0,0,0,0,0,0,"None","None","None",0,0,0,0,0,0,0,0);

	const TFDateInfoPacket & CDungeonInfoVector = DungeonInfoVector[index];

	return Py_BuildValue("issiiiiiiisssiiiiiiii",
		CDungeonInfoVector.id_dungeon,
		CDungeonInfoVector.name,
		CDungeonInfoVector.img,
		CDungeonInfoVector.lv_min,
		CDungeonInfoVector.lv_max,
		CDungeonInfoVector.party_max,
		CDungeonInfoVector.respawn,
		CDungeonInfoVector.time_room,
		CDungeonInfoVector.time_respawn_dungeon,
		CDungeonInfoVector.time_room_dungeon,
		CDungeonInfoVector.entrance,
		CDungeonInfoVector.resistance,
		CDungeonInfoVector.force,
		CDungeonInfoVector.vnum_item,
		CDungeonInfoVector.count_item,
		CDungeonInfoVector.count_finish,
		CDungeonInfoVector.time_finish,
		CDungeonInfoVector.difficulty,
		CDungeonInfoVector.damage_done,
		CDungeonInfoVector.damage_received,
		CDungeonInfoVector.id_boss
		);
}

PyObject * GetDungeonInfoDateMision(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	const CPythonDungeonInfo::TDungeonMisionInfo & DungeonInfoVector = CPythonDungeonInfo::Instance().GetDateInfoMision();
	if (DWORD(index) >= DungeonInfoVector.size())
		return Py_BuildValue("iiii",0,0,0,0);

	const TFDateMision & CDungeonInfoVector = DungeonInfoVector[index];

	return Py_BuildValue("iiii",
		CDungeonInfoVector.vnum_mob_mision,
		CDungeonInfoVector.count_mob_mision,
		CDungeonInfoVector.count_mob_a_mision,
		CDungeonInfoVector.status_mision
		);
}

PyObject * GetDungeonInfoDateRanking(PyObject * poSelf, PyObject * poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	const CPythonDungeonInfo::TDungeonRankingInfo & DungeonInfoVector = CPythonDungeonInfo::Instance().GetDateInfoRanking();
	if (DWORD(index) >= DungeonInfoVector.size())
		return Py_BuildValue("iisi",0,0,"None",0);

	const TFDateInfoRanking & CDungeonInfoVector = DungeonInfoVector[index];

	return Py_BuildValue("iisi",
		CDungeonInfoVector.vnum_mob_r,
		CDungeonInfoVector.type,
		CDungeonInfoVector.name_r,
		CDungeonInfoVector.extra_r
	);
}


void intdungeoninfo()
{
	static PyMethodDef s_methods[] =
	{
		{ "CountDateRanking",			GetDungeonInfoCountRanking,				METH_VARARGS },

		{ "InfoDateRanking", 			GetDungeonInfoDateRanking,				METH_VARARGS },
		{ "InfoDateMision",				GetDungeonInfoDateMision,				METH_VARARGS },
		{ "CountDate",					GetDungeonInfoCount,					METH_VARARGS },
		{ "InfoDate",					GetDungeonInfoDate,						METH_VARARGS },
		{ "Open",						OpenDungeonInfoSystem,					METH_VARARGS },
		{ "Teleport", 					TeleportDungeonInfoSystem,				METH_VARARGS },
		{ "Ranking",					RankingDungeonInfoSystem,				METH_VARARGS },
		{ "Mision",						MisionDungeonInfoSystem,				METH_VARARGS },
		{ "SetPageCount",				SetPageCount,							METH_VARARGS },
		{ "GetPageCount",				GetPageCount,							METH_VARARGS },
		{ "GetPageView",				GetPageView,							METH_VARARGS },
		
		{ NULL,								NULL,								NULL },
	};

	PyObject * poModule = Py_InitModule("dungeon_info", s_methods);
}
#endif // ENABLE_DUNGEON_INFO
