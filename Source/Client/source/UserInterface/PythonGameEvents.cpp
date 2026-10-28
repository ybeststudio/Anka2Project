#include "StdAfx.h"

#include "PythonSkill.h"
#include "PythonNonPlayer.h"
#include "PythonPlayer.h"
#include "PythonCharacterManager.h"
#include "PythonGameEvents.h"

void CPythonGameEvents::SetActivateEvent(bool isActivate, BYTE bEventID)
{
	if (bEventID > EVENT_MAX_NUM)
		return;

	m_pkActivateEvents[bEventID] = isActivate;
}

void CPythonGameEvents::SetEventTime(BYTE bEventID, DWORD event_time)
{
	if (bEventID > EVENT_MAX_NUM)
		return;

	m_dwEventEndTime[bEventID] = event_time;
}

bool CPythonGameEvents::IsActivateEvent(BYTE bEventID)
{
	return m_pkActivateEvents[bEventID];
}

DWORD CPythonGameEvents::GetEventTime(BYTE bEventID)
{
	return m_dwEventEndTime[bEventID];
}

CPythonGameEvents::CPythonGameEvents()
{
	for (int i = 0; i < EVENT_MAX_NUM; ++i)
	{
		m_pkActivateEvents[i] = false;
		m_dwEventEndTime[i] = 0;
	}
}

CPythonGameEvents::~CPythonGameEvents()
{
}

// Global instance - singleton'ý initialize etmek için
// Constructor çaðrýldýðýnda singleton pattern'in constructor'ý ms_singleton'ý set eder
static CPythonGameEvents g_GameEventsInstance;

PyObject* gameEventsIsActivateEvent(PyObject* poSelf, PyObject* poArgs)
{
	int iEventIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iEventIndex))
		return Py_BuildNone();

	bool isActivate = CPythonGameEvents::instance().IsActivateEvent(iEventIndex);

	return Py_BuildValue("i", isActivate == true ? 1 : 0);
}

PyObject* gameEventsGetEventTime(PyObject* poSelf, PyObject* poArgs)
{
	int iEventIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iEventIndex))
		return Py_BuildNone();

	DWORD event_time = CPythonGameEvents::instance().GetEventTime(iEventIndex);

	return Py_BuildValue("i", event_time);
}

void initGameEvents()
{
	// Ensure singleton instance is initialized before use
	(void)&g_GameEventsInstance;
	
	static PyMethodDef s_methods[] =
	{
		{"IsActivateEvent",gameEventsIsActivateEvent,METH_VARARGS},
		{"GetEventTime",gameEventsGetEventTime,METH_VARARGS},
		{NULL, NULL, NULL},
	};

	PyObject* poModule = Py_InitModule("gameEvents", s_methods);

	/*******************************************************************************************/
	/*** Events ***/
	/*******************************************************************************************/
	PyModule_AddIntConstant(poModule, "EVENT_FOOTBALL", EVENT_FOOTBALL);
	PyModule_AddIntConstant(poModule, "EVENT_AYISIGI", EVENT_AYISIGI);
	PyModule_AddIntConstant(poModule, "EVENT_OKEYCARD", EVENT_OKEYCARD);
	PyModule_AddIntConstant(poModule, "EVENT_FISH", EVENT_FISH);
	PyModule_AddIntConstant(poModule, "EVENT_ATTENDANCE", EVENT_ATTENDANCE);
	PyModule_AddIntConstant(poModule, "EVENT_CATCHKING", EVENT_CATCHKING);
	PyModule_AddIntConstant(poModule, "EVENT_WORD", EVENT_WORD);
	PyModule_AddIntConstant(poModule, "EVENT_HALLOWEEN", EVENT_HALLOWEEN);
	PyModule_AddIntConstant(poModule, "EVENT_RITUELSOUL", EVENT_RITUELSOUL);
	PyModule_AddIntConstant(poModule, "EVENT_EASTER", EVENT_EASTER);
	PyModule_AddIntConstant(poModule, "EVENT_STONEKILL", EVENT_STONEKILL);
	PyModule_AddIntConstant(poModule, "EVENT_XMAS", EVENT_XMAS);
	PyModule_AddIntConstant(poModule, "EVENT_RAMADAN", EVENT_RAMADAN);
	PyModule_AddIntConstant(poModule, "EVENT_CHEQUEDESK", EVENT_CHEQUEDESK);
	PyModule_AddIntConstant(poModule, "EVENT_MAX_NUM", EVENT_MAX_NUM);
	/*******************************************************************************************/
	/*** END ***/
	/*******************************************************************************************/
}