#include "StdAfx.h"

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
#include "PythonAcce.h"
#include "PythonNetworkStream.h"

CPythonAcce::CPythonAcce()
{
	Clear();
}

CPythonAcce::~CPythonAcce()
{
	Clear();
}

void CPythonAcce::Clear()
{
	dwPrice = 0;

	ZeroMemory(&m_vAcceResult, sizeof(m_vAcceResult));

	m_vAcceMaterials.clear();
	m_vAcceMaterials.resize(ACCE_WINDOW_MAX_MATERIALS);

	ZeroMemory(m_vAcceMaterials.data() , m_vAcceMaterials.size() * sizeof(m_vAcceMaterials[0]));
}

#ifdef ENABLE_GOLD_LIMIT
void CPythonAcce::AddMaterial(long long dwRefPrice, BYTE bPos, TItemPos tPos)
#else
void CPythonAcce::AddMaterial(DWORD dwRefPrice, BYTE bPos, TItemPos tPos)
#endif
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
		return;

	if (bPos == 0)
		dwPrice = dwRefPrice;

	TAcceMaterial& tMaterial=m_vAcceMaterials[bPos];
	tMaterial.bHere = 1;
	tMaterial.wCell = tPos.cell;
}

void CPythonAcce::AddResult(DWORD dwItemVnum, DWORD dwMinAbs, DWORD dwMaxAbs)
{
	TAcceResult& tResult = m_vAcceResult;
	tResult.dwItemVnum = dwItemVnum;
	tResult.dwMinAbs = dwMinAbs;
	tResult.dwMaxAbs = dwMaxAbs;
}

#ifdef ENABLE_GOLD_LIMIT
void CPythonAcce::RemoveMaterial(long long dwRefPrice, BYTE bPos)
#else
void CPythonAcce::RemoveMaterial(DWORD dwRefPrice, BYTE bPos)
#endif
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
		return;

	if (bPos == 1)
		dwPrice = dwRefPrice;

	memset(&m_vAcceMaterials[bPos], 0, sizeof(TAcceMaterial));
}

bool CPythonAcce::GetAttachedItem(BYTE bPos, BYTE & bHere, WORD & wCell)
{
	if (bPos >= ACCE_WINDOW_MAX_MATERIALS)
		return false;

	bHere = m_vAcceMaterials[bPos].bHere;
	wCell = m_vAcceMaterials[bPos].wCell;
	return true;
}

void CPythonAcce::GetResultItem(DWORD & dwItemVnum, DWORD & dwMinAbs, DWORD & dwMaxAbs)
{
	dwItemVnum = m_vAcceResult.dwItemVnum;
	dwMinAbs = m_vAcceResult.dwMinAbs;
	dwMaxAbs = m_vAcceResult.dwMaxAbs;
}

PyObject *SendAcceCloseRequest(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream & rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendAcceClosePacket();
	return Py_BuildNone();
}

PyObject *AcceClear(PyObject *poSelf, PyObject *poArgs)
{
	CPythonAcce::Instance().Clear();
	return Py_BuildNone();
}

PyObject *SendAcceAdd(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bPos=0;
	TItemPos tPos;
	if (!PyTuple_GetInteger(poArgs, 0, &tPos.window_type))
		return Py_BuildException();
	else if (!PyTuple_GetInteger(poArgs, 1, &tPos.cell))
		return Py_BuildException();
	else if (!PyTuple_GetInteger(poArgs, 2, &bPos))
		return Py_BuildException();

	CPythonNetworkStream & rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendAcceAddPacket(tPos, bPos);
	return Py_BuildNone();
}

PyObject *SendAcceRemove(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetInteger(poArgs, 0, &bPos))
		return Py_BuildException();

	CPythonNetworkStream & rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendAcceRemovePacket(bPos);
	return Py_BuildNone();
}

PyObject *GetAccePrice(PyObject *poSelf, PyObject *poArgs)
{
#ifdef ENABLE_GOLD_LIMIT
	return PyLong_FromLongLong(CPythonAcce::Instance().GetPrice());
#else
	return Py_BuildValue("i", CPythonAcce::Instance().GetPrice());
#endif
}

PyObject *GetAcceAttachedItem(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetInteger(poArgs, 0, &bPos))
		return Py_BuildException();

	BYTE bHere;
	WORD wCell;
	bool bGet = CPythonAcce::Instance().GetAttachedItem(bPos, bHere, wCell);
	if (!bGet)
	{
		bHere = 0;
		wCell = 0;
	}

	return Py_BuildValue("ii", bHere, wCell);
}

PyObject *GetAcceResultItem(PyObject *poSelf, PyObject *poArgs)
{
	DWORD dwItemVnum, dwMinAbs, dwMaxAbs;
	CPythonAcce::Instance().GetResultItem(dwItemVnum, dwMinAbs, dwMaxAbs);
	return Py_BuildValue("iii", dwItemVnum, dwMinAbs, dwMaxAbs);
}

PyObject *SendAcceRefineRequest(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bHere;
	WORD wCell;
	bool bGet = CPythonAcce::Instance().GetAttachedItem(1, bHere, wCell);
	if (bGet)
	{
		if (bHere)
		{
			CPythonNetworkStream & rkNetStream = CPythonNetworkStream::Instance();
			rkNetStream.SendAcceRefinePacket();
		}
	}

	return Py_BuildNone();
}

void initAcce()
{
	static PyMethodDef functions[] = {
		{"Clear",					AcceClear,					METH_VARARGS},
		{"SendCloseRequest",		SendAcceCloseRequest,		METH_VARARGS},
		{"Add",						SendAcceAdd,				METH_VARARGS},
		{"Remove",					SendAcceRemove,				METH_VARARGS},
		{"GetPrice",				GetAccePrice,				METH_VARARGS},
		{"GetAttachedItem",			GetAcceAttachedItem,		METH_VARARGS},
		{"GetResultItem",			GetAcceResultItem,			METH_VARARGS},
		{"SendRefineRequest",		SendAcceRefineRequest,		METH_VARARGS},

		{nullptr, nullptr, NULL},
	};

	PyObject *pModule = Py_InitModule("acce", functions);
	PyModule_AddIntConstant(pModule, "ABSORPTION_SOCKET",		ACCE_ABSORPTION_SOCKET);
	PyModule_AddIntConstant(pModule, "ABSORBED_SOCKET",			ACCE_ABSORBED_SOCKET);
	PyModule_AddIntConstant(pModule, "CLEAN_ATTR_VALUE0",		ACCE_CLEAN_ATTR_VALUE0);
	PyModule_AddIntConstant(pModule, "WINDOW_MAX_MATERIALS",	ACCE_WINDOW_MAX_MATERIALS);
	PyModule_AddIntConstant(pModule, "CLEAN_ATTR_VALUE_FIELD",	0);
	PyModule_AddIntConstant(pModule, "LIMIT_RANGE",				1000);
	PyModule_AddIntConstant(pModule, "ACCE_REVERSAL_VNUM_1", ACCE_REVERSAL_VNUM_1);
	PyModule_AddIntConstant(pModule, "ACCE_REVERSAL_VNUM_2", ACCE_REVERSAL_VNUM_2);
}
#endif
