#include "StdAfx.h"

#ifdef ENABLE_BIOLOG_SYSTEM
#include "PythonBiologManager.h"
#include "PythonNetworkStream.h"

PyObject *biologManagerGetCurrentLevel(PyObject *poSelf, PyObject *poArgs)
{
	auto dwLevel = CPythonBiologManager::Instance().Biolog_GetCurrentLevel();
	return Py_BuildValue("i", dwLevel);
}

PyObject *biologManagerGetCurrentItem(PyObject *poSelf, PyObject *poArgs)
{
	auto iItem = CPythonBiologManager::Instance().Biolog_GetCurrentItem();
	return Py_BuildValue("i", iItem);
}

PyObject *biologManagerGetCurrentCount(PyObject *poSelf, PyObject *poArgs)
{
	auto iGiven = CPythonBiologManager::Instance().Biolog_GetCurrentGivenItems();
	auto iCount = CPythonBiologManager::Instance().Biolog_GetCurrentRequiredCount();
	return Py_BuildValue("ii", iGiven, iCount);
}

PyObject *biologManagerGetCurrentCooldown(PyObject *poSelf, PyObject *poArgs)
{
	auto iGCooldown = CPythonBiologManager::Instance().Biolog_GetCurrentGlobalCooldown();
	auto iCooldown = CPythonBiologManager::Instance().Biolog_GetCurrentCooldown();
	return Py_BuildValue("ii", iGCooldown, iCooldown);
}

PyObject *biologManagerGetCooldownReminder(PyObject *poSelf, PyObject *poArgs)
{
	auto bReminder = CPythonBiologManager::Instance().Biolog_GetCurrentCooldownReminder();
	return Py_BuildValue("b", bReminder);
}

PyObject *biologManagerGetCurrentReward(PyObject *poSelf, PyObject *poArgs)
{
	int index = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	auto iType = CPythonBiologManager::Instance().Biolog_GetCurrentRewardType(index);
	auto iValue = CPythonBiologManager::Instance().Biolog_GetCurrentRewardValue(index);
	auto iItem = CPythonBiologManager::Instance().Biolog_GetCurrentRewardItem();
	auto iItemCount = CPythonBiologManager::Instance().Biolog_GetCurrentRewardItemCount();

	return Py_BuildValue("iiii", iType, iValue, iItem, iItemCount);
}

PyObject *biologSendPacket(PyObject *poSelf, PyObject *poArgs)
{
	int bSubHeader = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &bSubHeader))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendBiologManagerAction(bSubHeader);

	return Py_BuildNone();
}

PyObject *biologSendPacketItem(PyObject *poSelf, PyObject *poArgs)
{
	bool bAdditionalChance;
	if (!PyTuple_GetBoolean(poArgs, 0, &bAdditionalChance))
		bAdditionalChance = false;

	bool bAdditionalTime;
	if (!PyTuple_GetBoolean(poArgs, 1, &bAdditionalTime))
		bAdditionalTime = false;

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendBiologManagerAction(CG_BIOLOG_MANAGER_SEND);
	rkNetStream.Send(sizeof(bAdditionalChance), &bAdditionalChance);
	rkNetStream.Send(sizeof(bAdditionalTime), &bAdditionalTime);

	return Py_BuildNone();
}

PyObject *biologSendPacketReminder(PyObject *poSelf, PyObject *poArgs)
{
	bool bReminder;
	if (!PyTuple_GetBoolean(poArgs, 0, &bReminder))
		bReminder = false;

	CPythonNetworkStream::Instance().SendBiologManagerAction(CG_BIOLOG_MANAGER_TIMER);
	CPythonNetworkStream::Instance().Send(sizeof(bReminder), &bReminder);

	return Py_BuildNone();
}

void initBiologManager()
{
	static PyMethodDef s_methods[] =
	{
		{ "SendPacket", biologSendPacket, METH_VARARGS },
		{ "SendPacketItem", biologSendPacketItem, METH_VARARGS },
		{ "SendPacketReminder", biologSendPacketReminder, METH_VARARGS },
		{ "GetInfoLevel", biologManagerGetCurrentLevel, METH_VARARGS },
		{ "GetInfoItem", biologManagerGetCurrentItem, METH_VARARGS },
		{ "GetInfoCount", biologManagerGetCurrentCount, METH_VARARGS },
		{ "GetInfoCooldown", biologManagerGetCurrentCooldown, METH_VARARGS },
		{ "GetInfoReminder", biologManagerGetCooldownReminder, METH_VARARGS },
		{ "GetInfoReward", biologManagerGetCurrentReward, METH_VARARGS },
		{ nullptr, nullptr },
	};

	PyObject *poModule = Py_InitModule("biologmgr", s_methods);

	PyModule_AddIntConstant(poModule, "BIOLOG_BONUS_LENGTH", MAX_BONUSES_LENGTH);
}
#endif