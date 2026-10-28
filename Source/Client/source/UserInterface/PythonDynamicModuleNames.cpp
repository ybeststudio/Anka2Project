#include "StdAfx.h"

#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME

#include "PythonDynamicModuleNames.h"
#include "../EterPack/EterPackManager.h"

#pragma optimize("", off)

#if defined(_DEBUG)
std::string szModuleArrayOld[] = { "net", "chr", "chrmgr", "app", "pack", "player"};
#endif
std::string szModuleArray[MODULE_MAX] = {};

__forceinline const char *GetCallerReference_FileName()
{
	PyThreadState* pThreadState = PyThreadState_Get();
	PyFrameObject* pFrameObject = pThreadState->frame;

	char const* c_szCallerFileName = PyString_AsString(pFrameObject->f_code->co_filename);
	return c_szCallerFileName;
}

#if defined(_DEBUG)
const char *GetCallerReference_FunctionName()
{
	PyThreadState* pThreadState = PyThreadState_Get();
	PyFrameObject* pFrameObject = pThreadState->frame;

	char const* c_szCallerFunctionName = PyString_AsString(pFrameObject->f_code->co_name);
	return c_szCallerFunctionName;
}
#endif

inline std::string randomStrGen(int length)
{
	CHAR __alphabet[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0x0 }; // abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890
	static std::string charset = __alphabet;
	std::string result;
	result.resize(length);

	for (int i = 0; i < length; i++)
		result[i] = charset[rand() % charset.length()];

	return result;
}

inline bool IsUsedName(std::string szName)
{
	for (int i = 0; i < MODULE_MAX; ++i)
		if (szModuleArray[i].empty() == false)
			if (szModuleArray[i] == szName)
				return true;
	return false;
}

__forceinline std::string CreateModuleName()
{
	std::string sztmpName;
	do { sztmpName = randomStrGen(12); } while (IsUsedName(sztmpName));
	return sztmpName;
}

__forceinline void CreateModuleNames()
{
	for (int i = 0; i < MODULE_MAX; ++i)
	{
		if (szModuleArray[i].empty())
		{
			szModuleArray[i] = CreateModuleName();
#if defined(_DEBUG)
			TraceError("Dynamic module name created. %s -> %s", szModuleArrayOld[i].c_str(), szModuleArray[i].c_str());
#endif
		}
	}
}

std::string GetModuleName(int iModuleCode)
{
	return szModuleArray[iModuleCode];
}
#pragma optimize("", on )

PyObject *pythonApiGetModuleName(PyObject *poSelf, PyObject *poArgs)
{
	CHAR __net[] = { 'n', 'e', 't', 0x0 };
	CHAR __chr[] = { 'c', 'h', 'r', 0x0 };
	CHAR __chrmgr[] = { 'c', 'h', 'r', 'm', 'g', 'r', 0x0 };
	CHAR __app[] = { 'a', 'p', 'p', 0x0 };
	CHAR __pack[] = { 'p', 'a', 'c', 'k', 0x0 };
	CHAR __player[] = { 'p', 'l', 'a', 'y', 'e', 'r', 0x0 };
	CHAR __s[] = { 's', 0x0 };

	char *szOldName;
	if (!PyTuple_GetString(poArgs, 0, &szOldName))
		return Py_BuildException();

#ifndef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
	return Py_BuildValue(__s, szOldName);
#else
	CHAR __locale[] = { 'l', 'o', 'c', 'a', 'l', 'e', '/', 0x0 };
	CHAR __uiscript[] = { 'u', 'i', 's', 'c', 'r', 'i', 'p', 't', '/', 0x0 };
#if defined(__USE_CYTHON__)
	CHAR __rootlib[] = { '<', 's', 't', 'r', 'i', 'n', 'g', '>', 0x0 };
#endif
	const char *c_szCallerReference = GetCallerReference_FileName();
	if (!CEterPackManager::Instance().isExistInPack(c_szCallerReference) &&
		!strstr(c_szCallerReference, __locale) &&
		!strstr(c_szCallerReference, __uiscript)
#if defined(__USE_CYTHON__)
 && !strstr(c_szCallerReference, __rootlib)
#endif
		)
	{
#if defined(_DEBUG)
		TraceError("Error! Python module import request from unknown target! %s:%s", GetCallerReference_FileName(), GetCallerReference_FunctionName());
#endif
		return Py_BuildValue(__s, szOldName);
	}


	if (!strcmp(szOldName, __net))
		return Py_BuildValue(__s, szModuleArray[NET_MODULE].c_str());
	else if (!strcmp(szOldName, __chr))
		return Py_BuildValue(__s, szModuleArray[CHR_MODULE].c_str());
	else if (!strcmp(szOldName, __chrmgr))
		return Py_BuildValue(__s, szModuleArray[CHRMGR_MODULE].c_str());
	else if (!strcmp(szOldName, __app))
		return Py_BuildValue(__s, szModuleArray[APP_MODULE].c_str());
	else if (!strcmp(szOldName, __pack))
		return Py_BuildValue(__s, szModuleArray[PACK_MODULE].c_str());
	else if (!strcmp(szOldName, __player))
		return Py_BuildValue(__s, szModuleArray[PLAYER_MODULE].c_str());
#if defined(_DEBUG)
	else
		TraceError("Error! Unknown module name request as dynamic! (%s) - From-> %s:%s", szOldName, GetCallerReference_FileName(), GetCallerReference_FunctionName());
#endif

#endif

	return Py_BuildNone();
}

void initPythonApi()
{
	CHAR __GetModuleName[] = { 'G', 'e', 't', 'M', 'o', 'd', 'u', 'l', 'e', 'N', 'a', 'm', 'e', 0x0 };
	CHAR __pythonApi[] = { 'p', 'y', 'a', 'p', 'i', 0x0 };

	static PyMethodDef s_methods[] =
	{
		{ __GetModuleName, pythonApiGetModuleName, METH_VARARGS },
		{ nullptr, nullptr },
	};

	PyObject *poModule = Py_InitModule(__pythonApi, s_methods);

	CreateModuleNames();
}
#endif
