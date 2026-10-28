#include "StdAfx.h"

#include "Locale_inc.h"

#ifdef ENABLE_RIDING_EXTENDED
	#include "PythonMountUpGrade.h"
	#include "PythonNetworkStream.h"

CPythonMountUpGrade::CPythonMountUpGrade() { Reset(); }
CPythonMountUpGrade::~CPythonMountUpGrade() { Reset(); }

void CPythonMountUpGrade::Reset()
{
	m_HorseLevel = CPythonMountUpGrade::EMountUpGradeReset::RESET;
	m_MountUpGradeFail = CPythonMountUpGrade::EMountUpGradeReset::RESET;
	m_MountUpGradeExistingExp = CPythonMountUpGrade::EMountUpGradeReset::RESET;
}

void CPythonMountUpGrade::SetWindow(PyObject * ppyObject)
{
	m_window = ppyObject;
}

void CPythonMountUpGrade::DestroyWindow()
{
	m_window = nullptr;
}

void CPythonMountUpGrade::Refresh()
{
	PyCallClassMemberFunc(m_window, "Refresh", Py_BuildValue("()"));
}

void CPythonMountUpGrade::Chat(const uint8_t type, const uint8_t value)
{
	if (type >= CPythonMountUpGrade::EMountUpGradeChatType::CHAT_TYPE_MAX)
	{
		TraceError("CPythonMountUpGrade::Chat invalid type: %d", type);
		return;
	}
	
	PyCallClassMemberFunc(m_window, "Chat", Py_BuildValue("(ii)", type, value));
}

void CPythonMountUpGrade::SetHorseLevel(const uint8_t level)
{
	m_HorseLevel = level;
}

uint8_t CPythonMountUpGrade::GetHorseLevel() const
{
	return m_HorseLevel;
}

void CPythonMountUpGrade::SetMountUpGradeFail(const uint8_t fail)
{
	m_MountUpGradeFail = fail;
}

uint8_t CPythonMountUpGrade::IsMountUpGradeFail() const
{
	return m_MountUpGradeFail;
}

void CPythonMountUpGrade::SetMountUpGradeExistingExp(const uint32_t exp)
{
	m_MountUpGradeExistingExp = exp;
}

uint32_t CPythonMountUpGrade::GetMountUpGradeExistingExp() const
{
	return m_MountUpGradeExistingExp;
}

uint32_t CPythonMountUpGrade::GetMountUpGradeNecessaryExp() const
{
	return mount_up_grade_exp_table[GetHorseLevel() + 1];
}

uint32_t CPythonMountUpGrade::GetMountUpGradePrice() const
{
	return mount_up_grade_price_table[GetHorseLevel() + 1];
}

/* Py Methods */
PyObject* SetWindow(PyObject* poSelf, PyObject* poArgs)
{
	PyObject * pyHandle;
	if (!PyTuple_GetObject(poArgs, 0, &pyHandle))
		return Py_BadArgument();

	CPythonMountUpGrade::Instance().SetWindow(pyHandle);
	return Py_BuildNone();
}

PyObject* DestroyWindow(PyObject* poSelf, PyObject* poArgs)
{
	CPythonMountUpGrade::Instance().DestroyWindow();
	return Py_BuildNone();
}

PyObject* GetHorseLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonMountUpGrade::Instance().GetHorseLevel());
}

PyObject* GetMountUpGradePrice(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonMountUpGrade::Instance().GetMountUpGradePrice());
}

PyObject* IsMountUpGradeFail(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonMountUpGrade::Instance().IsMountUpGradeFail());
}

PyObject* GetMountUpGradeExistingExp(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonMountUpGrade::Instance().GetMountUpGradeExistingExp());
}

PyObject* GetMountUpGradeNecessaryExp(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonMountUpGrade::Instance().GetMountUpGradeNecessaryExp());
}

PyObject* Reset(PyObject* poSelf, PyObject* poArgs)
{
	CPythonMountUpGrade::Instance().Reset();
	return Py_BuildNone();
}

PyObject* Send(PyObject* poSelf, PyObject* poArgs)
{
	uint8_t iSubHeader;
	if (!PyTuple_GetInteger(poArgs, 0, &iSubHeader))
		return Py_BuildException();

	CPythonNetworkStream::Instance().MountUpGrade(iSubHeader);
	return Py_BuildNone();
}

void initmountupgrade()
{
	static PyMethodDef s_methods[] =
	{
		{ "Send", Send, METH_VARARGS },
		{ "Reset", Reset, METH_VARARGS },
		{ "GetHorseLevel", GetHorseLevel, METH_VARARGS },
		{ "GetMountUpGradePrice", GetMountUpGradePrice, METH_VARARGS },
		{ "IsMountUpGradeFail", IsMountUpGradeFail, METH_VARARGS },
		{ "GetMountExistingExp", GetMountUpGradeExistingExp, METH_VARARGS },
		{ "GetMountNecessaryExp", GetMountUpGradeNecessaryExp, METH_VARARGS },
		{ "SetWindow", SetWindow, METH_VARARGS },
		{ "DestroyWindow", DestroyWindow, METH_VARARGS },

		{ NULL, NULL, NULL },
	};

	PyObject * poModule = Py_InitModule("mupgrd", s_methods);

	/* CG SubHeader */
	PyModule_AddIntConstant(poModule, "MOUNT_UP_GRADE_EXP", CPythonMountUpGrade::EMountUpGradeCGSubheaderType::SUBHEADER_CG_MOUNT_UP_GRADE_EXP);
	PyModule_AddIntConstant(poModule, "MOUNT_UP_GRADE_LEVEL_UP", CPythonMountUpGrade::EMountUpGradeCGSubheaderType::SUBHEADER_CG_MOUNT_UP_GRADE_LEVEL_UP);

	/* Chat Type */
	PyModule_AddIntConstant(poModule, "CHAT_TYPE_BANN_WHILE_MOUNTING", CPythonMountUpGrade::EMountUpGradeChatType::CHAT_TYPE_BANN_WHILE_MOUNTING);
	PyModule_AddIntConstant(poModule, "CHAT_TYPE_LEVEL_UP_YANG_OR_FEED_NOT_ENOUGH", CPythonMountUpGrade::EMountUpGradeChatType::CHAT_TYPE_LEVEL_UP_YANG_OR_FEED_NOT_ENOUGH);
	PyModule_AddIntConstant(poModule, "CHAT_TYPE_LEVEL_UP_GEM_NOT_ENOUGH", CPythonMountUpGrade::EMountUpGradeChatType::CHAT_TYPE_LEVEL_UP_GEM_NOT_ENOUGH);
	PyModule_AddIntConstant(poModule, "CHAT_TYPE_LEVEL_UP_PERCENT_SUCCESSFUL", CPythonMountUpGrade::EMountUpGradeChatType::CHAT_TYPE_LEVEL_UP_PERCENT_SUCCESSFUL);
	PyModule_AddIntConstant(poModule, "CHAT_TYPE_LEVEL_UP_PERCENT_FAIL", CPythonMountUpGrade::EMountUpGradeChatType::CHAT_TYPE_LEVEL_UP_PERCENT_FAIL);
	PyModule_AddIntConstant(poModule, "CHAT_TYPE_EXP_HORSE_FEED", CPythonMountUpGrade::EMountUpGradeChatType::CHAT_TYPE_EXP_HORSE_FEED);

	/* Chat Value */
	PyModule_AddIntConstant(poModule, "CHAT_TYPE_NO_VALUE", CPythonMountUpGrade::EMountUpGradeChatValue::CHAT_TYPE_NO_VALUE);

	/* Fail State */
	PyModule_AddIntConstant(poModule, "MOUNT_UP_GRADE_FAIL_OFF", CPythonMountUpGrade::EMountUpGradeFailType::MOUNT_UP_GRADE_FAIL_OFF);
	PyModule_AddIntConstant(poModule, "MOUNT_UP_GRADE_FAIL_ON", CPythonMountUpGrade::EMountUpGradeFailType::MOUNT_UP_GRADE_FAIL_ON);

	/* LevelUp -> Item ID - Horse Feed */
	PyModule_AddIntConstant(poModule, "HORSE_FEED_ITEM_ID", CPythonMountUpGrade::EMountUpGradeItem::HORSE_FEED_ITEM_ID);
	PyModule_AddIntConstant(poModule, "HORSE_FEED_LEVEL_COUNT", CPythonMountUpGrade::EMountUpGradeItem::HORSE_FEED_LEVEL_COUNT);

	/* Exp - Horse Feed */
	PyModule_AddIntConstant(poModule, "HORSE_FEED_EXP_COUNT", CPythonMountUpGrade::EMountUpGradeItem::HORSE_FEED_EXP_COUNT);

	/* Tooltip Price */
	PyModule_AddIntConstant(poModule, "TOOLTIP_YANG", CPythonMountUpGrade::EMountUpGradeTooltip::YANG);
	PyModule_AddIntConstant(poModule, "TOOLTIP_GEM", CPythonMountUpGrade::EMountUpGradeTooltip::GEM);

	/* Cost Gem */
	PyModule_AddIntConstant(poModule, "HORSE_LEVEL_DETERMINES_GEM_COST", CPythonMountUpGrade::EMountUpGradeRetryGemCost::HORSE_LEVEL_DETERMINES_GEM_COST);
	PyModule_AddIntConstant(poModule, "HORSE_LOWER_LEVEL_RETRY_GEM_COST", CPythonMountUpGrade::EMountUpGradeRetryGemCost::HORSE_LOWER_LEVEL_RETRY_GEM_COST);
	PyModule_AddIntConstant(poModule, "HORSE_UPPER_LEVEL_RETRY_GEM_COST", CPythonMountUpGrade::EMountUpGradeRetryGemCost::HORSE_UPPER_LEVEL_RETRY_GEM_COST);

	/* Reset */
	PyModule_AddIntConstant(poModule, "RESET", CPythonMountUpGrade::EMountUpGradeReset::RESET);
}
#endif
