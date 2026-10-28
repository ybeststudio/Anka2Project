#include "StdAfx.h"
#include "../gamelib/GameEventManager.h"

#ifdef ENABLE_EVENT_MANAGER
	#include "PythonGameEventManager.h"
#endif

PyObject *eventMgrUpdate(PyObject *poSelf, PyObject *poArgs)
{
	float fx;
	if (!PyTuple_GetFloat(poArgs, 0, &fx))
		return Py_BuildException();
	float fy;
	if (!PyTuple_GetFloat(poArgs, 1, &fy))
		return Py_BuildException();
	float fz;
	if (!PyTuple_GetFloat(poArgs, 2, &fz))
		return Py_BuildException();

	CGameEventManager::Instance().SetCenterPosition(fx, fy, fz);
	CGameEventManager::Instance().Update();
	return Py_BuildNone();
}

#ifdef ENABLE_EVENT_MANAGER
PyObject *eventMgrGetDayEventCount(PyObject *poSelf, PyObject *poArgs)
{
	int iDay;
	if (!PyTuple_GetInteger(poArgs, 0, &iDay))
		return Py_BuildException();

	return Py_BuildValue("i", InGameEventManager::Instance().GetDayEventCount(iDay));
}

PyObject *eventMgrGetEventData(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iDay;
	if (!PyTuple_GetInteger(poArgs, 1, &iDay))
		return Py_BuildException();

	InGameEventManager::TEventTable table;
	memset(&table, 0, sizeof(table));

	InGameEventManager::Instance().GetEventData(iIndex, iDay, table);

	return Py_BuildValue("iiiiiiii", table.dwID, table.bType, table.startTime, table.endTime, table.dwVnum, table.iPercent, table.iDropType, table.bCompleted);
}

PyObject *eventMgrSetRequestEventData(PyObject *poSelf, PyObject *poArgs)
{
	bool bRequest;
	if (!PyTuple_GetBoolean(poArgs, 0, &bRequest))
		return Py_BadArgument();

	InGameEventManager::Instance().SetRequestEventData(bRequest);

	return Py_BuildNone();
}

PyObject *eventMgrGetRequestEventData(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("b", InGameEventManager::Instance().GetRequestEventData());
}

PyObject *eventMgrGetEventRewardData(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iPos))
		return Py_BuildException();

	InGameEventManager::TEventReward table;
	memset(&table, 0, sizeof(table));

	InGameEventManager::Instance().GetEventRewardData(iIndex, iPos, table);

	return Py_BuildValue("ii", table.dwVnum, table.dwCount);
}
#endif

void initeventmgr()
{
	static PyMethodDef s_methods[] = 
	{
		{ "Update",					eventMgrUpdate,					METH_VARARGS },

#ifdef ENABLE_EVENT_MANAGER
		{ "GetDayEventCount",		eventMgrGetDayEventCount,		METH_VARARGS },
		{ "GetEventData",			eventMgrGetEventData,			METH_VARARGS },
		{ "SetRequestEventData",	eventMgrSetRequestEventData,	METH_VARARGS },
		{ "GetRequestEventData",	eventMgrGetRequestEventData,	METH_VARARGS },
		{ "GetEventRewardData",		eventMgrGetEventRewardData,		METH_VARARGS },
#endif

		{ nullptr, nullptr },
	};

#ifdef ENABLE_EVENT_MANAGER
	PyObject *poModule = Py_InitModule("eventMgr", s_methods);

	PyModule_AddIntConstant(poModule, "EVENT_TYPE_NONE",			InGameEventManager::EVENT_TYPE_NONE);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_EXPERIENCE",		InGameEventManager::EVENT_TYPE_EXPERIENCE);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_ITEM_DROP",		InGameEventManager::EVENT_TYPE_ITEM_DROP);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_BOSS",			InGameEventManager::EVENT_TYPE_BOSS);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_METIN",			InGameEventManager::EVENT_TYPE_METIN);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_MINING",			InGameEventManager::EVENT_TYPE_MINING);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_GOLD_FROG",		InGameEventManager::EVENT_TYPE_GOLD_FROG);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_MOONLIGHT",		InGameEventManager::EVENT_TYPE_MOONLIGHT);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_HEXEGONAL_CHEST",	InGameEventManager::EVENT_TYPE_HEXEGONAL_CHEST);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_FISHING",			InGameEventManager::EVENT_TYPE_FISHING);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_HIDE_AND_SEEK",	InGameEventManager::EVENT_TYPE_HIDE_AND_SEEK);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_OX",				InGameEventManager::EVENT_TYPE_OX);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_TANAKA",			InGameEventManager::EVENT_TYPE_TANAKA);
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_YANG",			InGameEventManager::EVENT_TYPE_YANG);
#ifdef ENABLE_MINI_GAME_OKEY
	PyModule_AddIntConstant(poModule, "EVENT_TYPE_OKEY_CARDS",		InGameEventManager::EVENT_TYPE_OKEY_CARDS);
#endif
	PyModule_AddIntConstant(poModule, "EVENT_MAX_NUM",				InGameEventManager::EVENT_MAX_NUM);
	PyModule_AddIntConstant(poModule, "DAY_MAX_NUM",				InGameEventManager::DAY_MAX_NUM);
	PyModule_AddIntConstant(poModule, "REWARD_MAX_NUM",				InGameEventManager::REWARD_MAX_NUM);
#else
	Py_InitModule("eventMgr", s_methods);
#endif
}