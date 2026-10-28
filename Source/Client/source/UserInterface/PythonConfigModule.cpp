#include "StdAfx.h"

#ifdef ENABLE_CONFIG_MODULE
#include "PythonApplication.h"
#include "PythonConfig.h"

PyObject *cfgInit(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonConfig::Instance().Initialize(szFileName);

	return Py_BuildNone();
}

PyObject *cfgSet(PyObject *poSelf, PyObject *poArgs)
{
	unsigned char bType;
	if (!PyTuple_GetByte(poArgs, 0, &bType))
		return Py_BuildException();

	char *szKey;
	if (!PyTuple_GetString(poArgs, 1, &szKey))
		return Py_BuildException();

	char *szValue;
	int iValue;

	CPythonConfig& rkConfig = CPythonConfig::Instance();

	if (PyTuple_GetString(poArgs, 2, &szValue))
	{
		rkConfig.Write((CPythonConfig::EClassTypes) bType, szKey, szValue);
	}
	else if (PyTuple_GetInteger(poArgs, 2, &iValue))
	{
		rkConfig.Write((CPythonConfig::EClassTypes) bType, szKey, iValue);
	}
	else
		return Py_BuildException();

	return Py_BuildNone();
}

PyObject *cfgGet(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bType;
	if (!PyTuple_GetByte(poArgs, 0, &bType))
		return Py_BuildException();

	char* szKey;
	if (!PyTuple_GetString(poArgs, 1, &szKey))
		return Py_BuildException();

	char* szDefault;
	if (!PyTuple_GetString(poArgs, 2, &szDefault))
		return Py_BuildException();

	return Py_BuildValue("s", CPythonConfig::Instance().GetString((CPythonConfig::EClassTypes)bType, szKey, szDefault).c_str());
}

PyObject *cfgRemove(PyObject *poSelf, PyObject *poArgs)
{
	unsigned char bType;
	if (!PyTuple_GetByte(poArgs, 0, &bType))
		return Py_BuildException();

	CPythonConfig::Instance().RemoveSection((CPythonConfig::EClassTypes) bType);

	return Py_BuildNone();
}

void initcfg()
{
	static PyMethodDef s_methods[] =
	{
		{ "Init",		cfgInit,		METH_VARARGS },
		{ "Set",		cfgSet,			METH_VARARGS },
		{ "Get",		cfgGet,			METH_VARARGS },
		{ "Remove",		cfgRemove,		METH_VARARGS },

		{ nullptr, nullptr }
	};

	PyObject *poModule = Py_InitModule("cfg", s_methods);

	PyModule_AddIntConstant(poModule, "SAVE_GENERAL",			CPythonConfig::CLASS_GENERAL);
	PyModule_AddIntConstant(poModule, "SAVE_CHAT",				CPythonConfig::CLASS_CHAT);
	PyModule_AddIntConstant(poModule, "SAVE_OPTION",			CPythonConfig::CLASS_OPTION);
#ifdef ENABLE_SKILL_COLOR_SYSTEM
	PyModule_AddIntConstant(poModule, "SAVE_SKILL_COLOR",		CPythonConfig::CLASS_SKILL_COLOR);
#endif
}

#endif
