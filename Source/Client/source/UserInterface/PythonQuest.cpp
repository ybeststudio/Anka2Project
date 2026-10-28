#include "StdAfx.h"
#include "PythonQuest.h"

void CPythonQuest::RegisterQuestInstance(const SQuestInstance & c_rQuestInstance)
{
	DeleteQuestInstance(c_rQuestInstance.dwIndex);
	m_QuestInstanceContainer.push_back(c_rQuestInstance);

	SQuestInstance & rQuestInstance = *m_QuestInstanceContainer.rbegin();
	rQuestInstance.iStartTime = int(CTimer::Instance().GetCurrentSecond());
}

struct FQuestInstanceCompare
{
	DWORD dwSearchIndex;
	FQuestInstanceCompare(DWORD dwIndex) : dwSearchIndex(dwIndex) {}
	bool operator () (const CPythonQuest::SQuestInstance & rQuestInstance)
	{
		return dwSearchIndex == rQuestInstance.dwIndex;
	}
};

void CPythonQuest::DeleteQuestInstance(DWORD dwIndex)
{
	TQuestInstanceContainer::iterator itor = std::find_if(m_QuestInstanceContainer.begin(), m_QuestInstanceContainer.end(), FQuestInstanceCompare(dwIndex));
	if (itor == m_QuestInstanceContainer.end())
		return;

	m_QuestInstanceContainer.erase(itor);
}

bool CPythonQuest::IsQuest(DWORD dwIndex)
{
	TQuestInstanceContainer::iterator itor = std::find_if(m_QuestInstanceContainer.begin(), m_QuestInstanceContainer.end(), FQuestInstanceCompare(dwIndex));
	return itor != m_QuestInstanceContainer.end();
}

#ifdef ENABLE_RENEWAL_QUEST
void CPythonQuest::MakeQuest(DWORD dwIndex, DWORD c_index)
#else
void CPythonQuest::MakeQuest(DWORD dwIndex)
#endif
{
	DeleteQuestInstance(dwIndex);
	m_QuestInstanceContainer.push_back(SQuestInstance());

	SQuestInstance & rQuestInstance = *m_QuestInstanceContainer.rbegin();
	rQuestInstance.dwIndex = dwIndex;
#ifdef ENABLE_RENEWAL_QUEST
	rQuestInstance.c_index = c_index;
#endif
	rQuestInstance.iStartTime = int(CTimer::Instance().GetCurrentSecond());
}

void CPythonQuest::SetQuestTitle(DWORD dwIndex, const char *c_szTitle)
{
	SQuestInstance * pQuestInstance;
	if (!__GetQuestInstancePtr(dwIndex, &pQuestInstance))
		return;

	pQuestInstance->strTitle = c_szTitle;
}

void CPythonQuest::SetQuestClockName(DWORD dwIndex, const char *c_szClockName)
{
	SQuestInstance * pQuestInstance;
	if (!__GetQuestInstancePtr(dwIndex, &pQuestInstance))
		return;

	pQuestInstance->strClockName = c_szClockName;
}

void CPythonQuest::SetQuestCounterName(DWORD dwIndex, const char *c_szCounterName)
{
	SQuestInstance * pQuestInstance;
	if (!__GetQuestInstancePtr(dwIndex, &pQuestInstance))
		return;

	pQuestInstance->strCounterName = c_szCounterName;
}

void CPythonQuest::SetQuestClockValue(DWORD dwIndex, int iClockValue)
{
	SQuestInstance * pQuestInstance;
	if (!__GetQuestInstancePtr(dwIndex, &pQuestInstance))
		return;

	pQuestInstance->iClockValue = iClockValue;
	pQuestInstance->iStartTime = int(CTimer::Instance().GetCurrentSecond());
}

void CPythonQuest::SetQuestCounterValue(DWORD dwIndex, int iCounterValue)
{
	SQuestInstance * pQuestInstance;
	if (!__GetQuestInstancePtr(dwIndex, &pQuestInstance))
		return;

	pQuestInstance->iCounterValue = iCounterValue;
}

void CPythonQuest::SetQuestIconFileName(DWORD dwIndex, const char *c_szIconFileName)
{
	SQuestInstance * pQuestInstance;
	if (!__GetQuestInstancePtr(dwIndex, &pQuestInstance))
		return;

	pQuestInstance->strIconFileName = c_szIconFileName;
}

int CPythonQuest::GetQuestCount()
{
	return m_QuestInstanceContainer.size();
}

bool CPythonQuest::GetQuestInstancePtr(DWORD dwArrayIndex, SQuestInstance ** ppQuestInstance)
{
	if (dwArrayIndex >= m_QuestInstanceContainer.size())
		return false;

	*ppQuestInstance = &m_QuestInstanceContainer[dwArrayIndex];

	return true;
}

bool CPythonQuest::__GetQuestInstancePtr(DWORD dwQuestIndex, SQuestInstance ** ppQuestInstance)
{
	TQuestInstanceContainer::iterator itor = std::find_if(m_QuestInstanceContainer.begin(), m_QuestInstanceContainer.end(), FQuestInstanceCompare(dwQuestIndex));
	if (itor == m_QuestInstanceContainer.end())
		return false;

	*ppQuestInstance = &(*itor);

	return true;
}

void CPythonQuest::__Initialize()
{
/*
#ifdef _DEBUG
	for (int i = 0; i < 7; ++i)
	{
		SQuestInstance test;
		test.dwIndex = i;
		test.strIconFileName = "";
		test.strTitle = _getf("test%d", i);
		test.strClockName = "남은 시간";
		test.strCounterName = "남은 마리수";
		test.iClockValue = 1000;
		test.iCounterValue = 1000;
		test.iStartTime = 0;
		RegisterQuestInstance(test);
	}
#endif
*/
}

void CPythonQuest::Clear()
{
	m_QuestInstanceContainer.clear();
}

CPythonQuest::CPythonQuest()
{
	__Initialize();
}

CPythonQuest::~CPythonQuest()
{
	Clear();
}

PyObject *questGetQuestCount(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonQuest::Instance().GetQuestCount());
}

PyObject *questGetQuestData(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	CPythonQuest::SQuestInstance * pQuestInstance;
	if (!CPythonQuest::Instance().GetQuestInstancePtr(iIndex, &pQuestInstance))
		return Py_BuildException("Failed to find quest by index %d", iIndex);

	CGraphicImage * pImage = nullptr;
	if (!pQuestInstance->strIconFileName.empty())
	{
		std::string strIconFileName;
		strIconFileName = "d:/ymir work/ui/game/quest/questicon/";
		strIconFileName += pQuestInstance->strIconFileName.c_str();
		pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(strIconFileName.c_str());
	}
	else
	{
		{
			std::string strIconFileName = "icon/scroll_open.tga";
			pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(strIconFileName.c_str());
		}
	}

#ifdef ENABLE_RENEWAL_QUEST
	if (pQuestInstance->c_index == 99)
		pQuestInstance->c_index = 6;

	return Py_BuildValue("isiNsi",	pQuestInstance->dwIndex,
									pQuestInstance->strTitle.c_str(),
									pQuestInstance->c_index,
									Py_BuildPointer(pImage),
									pQuestInstance->strCounterName.c_str(),
									pQuestInstance->iCounterValue);
#else
	return Py_BuildValue("sNsi",	pQuestInstance->strTitle.c_str(),
									Py_BuildPointer(pImage),
									pQuestInstance->strCounterName.c_str(),
									pQuestInstance->iCounterValue);
#endif
}

PyObject *questGetQuestIndex(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	CPythonQuest::SQuestInstance * pQuestInstance;
	if (!CPythonQuest::Instance().GetQuestInstancePtr(iIndex, &pQuestInstance))
		return Py_BuildException("Failed to find quest by index %d", iIndex);

	return Py_BuildValue("i", pQuestInstance->dwIndex);
}

PyObject *questGetQuestLastTime(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	CPythonQuest::SQuestInstance * pQuestInstance;
	if (!CPythonQuest::Instance().GetQuestInstancePtr(iIndex, &pQuestInstance))
		return Py_BuildException("Failed to find quest by index %d", iIndex);

	int iLastTime = 0;

	if (pQuestInstance->iClockValue >= 0)
	{
		iLastTime = (pQuestInstance->iStartTime + pQuestInstance->iClockValue) - int(CTimer::Instance().GetCurrentSecond());
	}

	return Py_BuildValue("si", pQuestInstance->strClockName.c_str(), iLastTime);
}

PyObject *questClear(PyObject *poSelf, PyObject *poArgs)
{
	CPythonQuest::Instance().Clear();
	return Py_BuildNone();
}

#ifdef ENABLE_RENEWAL_QUEST
PyObject *questAdd(PyObject *poSelf, PyObject *poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BadArgument();

	int category;
	if (!PyTuple_GetInteger(poArgs, 1, &category))
		return Py_BadArgument();

	char *title;
	if (!PyTuple_GetString(poArgs, 2, &title))
		return Py_BadArgument();

	char *clockName;
	if (!PyTuple_GetString(poArgs, 3, &clockName))
		return Py_BadArgument();

	int clockValue;
	if (!PyTuple_GetInteger(poArgs, 4, &clockValue))
		return Py_BadArgument();

	char *counterName;
	if (!PyTuple_GetString(poArgs, 5, &counterName))
		return Py_BadArgument();

	int counterValue;
	if (!PyTuple_GetInteger(poArgs, 6, &counterValue))
		return Py_BadArgument();

	char *iconFilename;
	if (!PyTuple_GetString(poArgs, 7, &iconFilename))
		return Py_BadArgument();

	CPythonQuest& rkQuest = CPythonQuest::Instance();
	if (rkQuest.IsQuest(index)) 
	{
		TraceError("Quest with index %d already exists", index);
		return Py_BuildNone();
	}

	CPythonQuest::SQuestInstance QuestInstance;
	QuestInstance.dwIndex = index;
	QuestInstance.c_index = category;
	QuestInstance.strTitle = title;
	QuestInstance.strClockName = clockName;
	QuestInstance.iClockValue = clockValue;
	QuestInstance.strCounterName = counterName;
	QuestInstance.iCounterValue = counterValue;
	QuestInstance.strIconFileName = iconFilename;
	QuestInstance.iStartTime = int(CTimer::Instance().GetCurrentSecond());
	rkQuest.RegisterQuestInstance(QuestInstance);

	return Py_BuildNone();
}
#endif

void initquest()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetQuestCount",				questGetQuestCount,				METH_VARARGS },
		{ "GetQuestData",				questGetQuestData,				METH_VARARGS },
		{ "GetQuestIndex",				questGetQuestIndex,				METH_VARARGS },
		{ "GetQuestLastTime",			questGetQuestLastTime,			METH_VARARGS },
		{ "Clear",						questClear,						METH_VARARGS },
#ifdef ENABLE_RENEWAL_QUEST
		{ "Add",						questAdd,						METH_VARARGS },
#endif
		{ nullptr, nullptr },
	};

	PyObject *poModule = Py_InitModule("quest", s_methods);
	PyModule_AddIntConstant(poModule, "QUEST_MAX_NUM", 5);
#ifdef ENABLE_RENEWAL_QUEST
	PyModule_AddIntConstant(poModule, "QUEST_CATEGORY_MAX_NUM", 8);
#endif
}
