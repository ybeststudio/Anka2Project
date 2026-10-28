#include "StdAfx.h"
#include "PythonExchange.h"

PyObject *exchangeInitTrading(PyObject *poSelf, PyObject *poArgs)
{
	CPythonExchange::Instance().End();
	return Py_BuildNone();
}

PyObject *exchangeisTrading(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().isTrading());
}

PyObject *exchangeGetElkFromSelf(PyObject *poSelf, PyObject *poArgs)
{
#ifdef ENABLE_GOLD_LIMIT
	return PyLong_FromLongLong(CPythonExchange::Instance().GetElkFromSelf());
#else
	return Py_BuildValue("i", CPythonExchange::Instance().GetElkFromSelf());
#endif
}

PyObject *exchangeGetElkFromTarget(PyObject *poSelf, PyObject *poArgs)
{
#ifdef ENABLE_GOLD_LIMIT
	return PyLong_FromLongLong(CPythonExchange::Instance().GetElkFromTarget());
#else
	return Py_BuildValue("i", CPythonExchange::Instance().GetElkFromTarget());
#endif
}

PyObject *exchangeGetAcceptFromSelf(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetAcceptFromSelf());
}

PyObject *exchangeGetAcceptFromTarget(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("i", CPythonExchange::Instance().GetAcceptFromTarget());
}

PyObject *exchangeGetItemVnumFromSelf(PyObject *poSelf, PyObject *poArgs)
{
	int pos;

	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemVnumFromSelf((char) pos));
}

PyObject *exchangeGetItemVnumFromTarget(PyObject *poTarget, PyObject *poArgs)
{
	int pos;

	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemVnumFromTarget((char) pos));
}

PyObject *exchangeGetItemCountFromSelf(PyObject *poSelf, PyObject *poArgs)
{
	int pos;

	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemCountFromSelf((char) pos));
}

PyObject *exchangeGetItemCountFromTarget(PyObject *poTarget, PyObject *poArgs)
{
	int pos;

	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonExchange::Instance().GetItemCountFromTarget((char) pos));
}

PyObject *exchangeGetNameFromSelf(PyObject *poTarget, PyObject *poArgs)
{
	return Py_BuildValue("s", CPythonExchange::Instance().GetNameFromSelf());
}

PyObject *exchangeGetNameFromTarget(PyObject *poTarget, PyObject *poArgs)
{
	return Py_BuildValue("s", CPythonExchange::Instance().GetNameFromTarget());
}

PyObject *exchangeGetRaceFromSelf(PyObject *poTarget, PyObject *poArgs)
{
	return Py_BuildValue("I", CPythonExchange::Instance().GetRaceFromSelf());
}

PyObject *exchangeGetRaceFromTarget(PyObject *poTarget, PyObject *poArgs)
{
	return Py_BuildValue("I", CPythonExchange::Instance().GetRaceFromTarget());
}

PyObject *exchangeGetLevelFromSelf(PyObject *poTarget, PyObject *poArgs)
{
	return Py_BuildValue("I", CPythonExchange::Instance().GetLevelFromSelf());
}

PyObject *exchangeGetLevelFromTarget(PyObject *poTarget, PyObject *poArgs)
{
	return Py_BuildValue("I", CPythonExchange::Instance().GetLevelFromTarget());
}

PyObject *exchangeGetItemMetinSocketFromTarget(PyObject *poTarget, PyObject *poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();
	int iMetinSocketPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketPos))
		return Py_BuildException();
	return Py_BuildValue("i", CPythonExchange::Instance().GetItemMetinSocketFromTarget(pos, iMetinSocketPos));
}

PyObject *exchangeGetItemMetinSocketFromSelf(PyObject *poTarget, PyObject *poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();
	int iMetinSocketPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketPos))
		return Py_BuildException();
	return Py_BuildValue("i", CPythonExchange::Instance().GetItemMetinSocketFromSelf(pos, iMetinSocketPos));
}

PyObject *exchangeGetItemAttributeFromTarget(PyObject *poTarget, PyObject *poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();
	int iAttrSlotPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotPos))
		return Py_BuildException();

	BYTE byType;
	short sValue;
	CPythonExchange::Instance().GetItemAttributeFromTarget(pos, iAttrSlotPos, &byType, &sValue);

	return Py_BuildValue("ii", byType, sValue);
}

PyObject *exchangeGetItemAttributeFromSelf(PyObject *poTarget, PyObject *poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();
	int iAttrSlotPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotPos))
		return Py_BuildException();

	BYTE byType;
	short sValue;
	CPythonExchange::Instance().GetItemAttributeFromSelf(pos, iAttrSlotPos, &byType, &sValue);

	return Py_BuildValue("ii", byType, sValue);
}

PyObject *exchangeGetElkMode(PyObject *poTarget, PyObject *poArgs)
{
	return Py_BuildValue("b", CPythonExchange::Instance().GetElkMode());
}

PyObject *exchangeSetElkMode(PyObject *poTarget, PyObject *poArgs)
{
	int elk_mode;

	if (!PyTuple_GetInteger(poArgs, 0, &elk_mode))
		return Py_BuildException();

	CPythonExchange::Instance().SetElkMode(elk_mode ? true : false);
	return Py_BuildNone();
}

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
PyObject *exchangeGetChangeLookVnumFromSelf(PyObject *poSelf, PyObject *poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	const DWORD dwTransmutationVnum = CPythonExchange::Instance().GetItemChangeLookVnumToSelf(pos);
	return Py_BuildValue("i", dwTransmutationVnum);
}

PyObject *exchangeGetChangeLookVnumFromTarget(PyObject *poSelf, PyObject *poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	const DWORD dwTransmutationVnum = CPythonExchange::Instance().GetItemChangeLookVnumToTarget(pos);
	return Py_BuildValue("i", dwTransmutationVnum);
}
#endif

void initTrade()
{
	static PyMethodDef s_methods[] = 
	{
		{"InitTrading", exchangeInitTrading, METH_VARARGS},
		{"isTrading", exchangeisTrading, METH_VARARGS},

		{"GetElkFromSelf", exchangeGetElkFromSelf, METH_VARARGS},
		{"GetElkFromTarget", exchangeGetElkFromTarget, METH_VARARGS},

		{"GetItemVnumFromSelf", exchangeGetItemVnumFromSelf, METH_VARARGS},
		{"GetItemVnumFromTarget", exchangeGetItemVnumFromTarget, METH_VARARGS},

		{"GetItemCountFromSelf", exchangeGetItemCountFromSelf, METH_VARARGS},
		{"GetItemCountFromTarget", exchangeGetItemCountFromTarget, METH_VARARGS},

		{"GetAcceptFromSelf", exchangeGetAcceptFromSelf, METH_VARARGS},
		{"GetAcceptFromTarget", exchangeGetAcceptFromTarget, METH_VARARGS},

		{"GetNameFromSelf", exchangeGetNameFromSelf, METH_VARARGS},
		{"GetNameFromTarget", exchangeGetNameFromTarget, METH_VARARGS},

		{"GetRaceFromSelf", exchangeGetRaceFromSelf, METH_VARARGS},
		{"GetRaceFromTarget", exchangeGetRaceFromTarget, METH_VARARGS},

		{"GetLevelFromSelf", exchangeGetLevelFromSelf, METH_VARARGS},
		{"GetLevelFromTarget", exchangeGetLevelFromTarget, METH_VARARGS},

		{"GetItemMetinSocketFromTarget", exchangeGetItemMetinSocketFromTarget, METH_VARARGS},
		{"GetItemMetinSocketFromSelf", exchangeGetItemMetinSocketFromSelf, METH_VARARGS},

		{"GetItemAttributeFromTarget", exchangeGetItemAttributeFromTarget, METH_VARARGS},
		{"GetItemAttributeFromSelf", exchangeGetItemAttributeFromSelf, METH_VARARGS},

		{"GetElkMode", exchangeGetElkMode, METH_VARARGS},
		{"SetElkMode", exchangeSetElkMode, METH_VARARGS},

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		{"GetChangeLookVnumFromSelf", exchangeGetChangeLookVnumFromSelf, METH_VARARGS},
		{"GetChangeLookVnumFromTarget", exchangeGetChangeLookVnumFromTarget, METH_VARARGS},
#endif

		{nullptr, NULL},
	};

	PyObject *poModule = Py_InitModule("exchange", s_methods);
	PyModule_AddIntConstant(poModule, "EXCHANGE_ITEM_MAX_NUM", CPythonExchange::EXCHANGE_ITEM_MAX_NUM);
}