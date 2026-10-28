#include "StdAfx.h"
#include "PythonStatusEffectManager.h"
#include "PythonNetworkStream.h"
#include "PythonPlayer.h"
#include "PythonApplication.h"
#ifdef ENABLE_STATUS_EFFECT_SHOWER

CPythonStatusEffectManager::CPythonStatusEffectManager()
	: m_poHandler(NULL)
{
}

CPythonStatusEffectManager::~CPythonStatusEffectManager()
{
	m_poHandler = NULL;
}

void CPythonStatusEffectManager::SetStatusEffectShowerHandler(PyObject* poHandler)
{
	m_poHandler = poHandler;
}

void CPythonStatusEffectManager::ClearStatusEffects()
{
	if (!m_poHandler)
		return;

	PyCallClassMemberFunc(m_poHandler, "ClearStatusEffects", Py_BuildValue("()"));
	RefreshStatusEffects();
}

void CPythonStatusEffectManager::RefreshStatusEffects()
{
	if (!m_poHandler)
		return;

	PyCallClassMemberFunc(m_poHandler, "RefreshStatusEffects", Py_BuildValue("()"));
}

bool CPythonStatusEffectManager::IsUsingAffectFlagBit(DWORD dwAffect)
{
	if (dwAffect < 607 || dwAffect > 704)
		return true;

	switch (dwAffect)
	{
		case 607:
		case 617:
		case 618:
		case 619:
		case 620:
		case 621:
		case 622:
		case 628:
		case 629:
		case 663:
		case 684:
		case 703:
		case 704:
			return false;
		default:
			return true;
	}
}

void CPythonStatusEffectManager::RemoveAffect(DWORD dwAffect, bool is_buff)
{
	if (!m_poHandler)
		return;

	BYTE bStatusEffectType = IsUsingAffectFlagBit(dwAffect) ? STATUS_EFFECT_TYPE_AFFECT_FLAG : STATUS_EFFECT_TYPE_AFFECT_TYPE;
	PyCallClassMemberFunc(m_poHandler, "RemoveAffect", Py_BuildValue("(ib)", dwAffect, bStatusEffectType));
	RefreshStatusEffects();
}

bool CPythonStatusEffectManager::RecvStatusEffectPacket()
{
	TPacketGCStatusEffect kHeader;
	if (!CPythonNetworkStream::Instance().Recv(sizeof(kHeader), &kHeader))
		return false;

	switch (kHeader.bSubHeader)
	{
		case SUB_HEADER_GC_STATUS_EFFECT_INITIALIZE:
		{
			WORD wCount;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(wCount), &wCount))
				return false;

			if (m_poHandler)
			{
				PyCallClassMemberFunc(m_poHandler, "ClearStatusEffects", Py_BuildValue("()"));
			}

			for (WORD i = 0; i < wCount; ++i)
			{
				TPacketGCStatusEffectItem kItem;
				if (!CPythonNetworkStream::Instance().Recv(sizeof(kItem), &kItem))
					return false;

				if (m_poHandler)
				{
					BYTE bStatusEffectType = IsUsingAffectFlagBit(kItem.dwType) ? STATUS_EFFECT_TYPE_AFFECT_FLAG : STATUS_EFFECT_TYPE_AFFECT_TYPE;
					bool is_buff = false;
					PyCallClassMemberFunc(m_poHandler, "AddAffect", Py_BuildValue("(ibiiiii)",
						bStatusEffectType,
						is_buff,
						kItem.dwType,
						kItem.wPointType,
						kItem.lPointValue,
						kItem.lDuration,
						kItem.lFlag));
				}
			}

			if (m_poHandler)
			{
				PyCallClassMemberFunc(m_poHandler, "RefreshStatusEffects", Py_BuildValue("()"));
			}
			return true;
		}

		case SUB_HEADER_GC_STATUS_EFFECT_ELEMENT_ENCHANT:
		{
			TPacketGCStatusEffectElementEnchant kSubPacket;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(kSubPacket), &kSubPacket))
				return false;

			if (CPythonPlayer::Instance().GetMainCharacterIndex() == kSubPacket.dwVID)
			{
				if (m_poHandler)
				{
					PyObject* poDict = PyDict_New();
					for (int i = 0; i < 6; ++i)
					{
						PyDict_SetItem(poDict, PyInt_FromLong(i), PyInt_FromLong(kSubPacket.adwElement[i]));
					}
					PyCallClassMemberFunc(m_poHandler, "SetElementEnchant", Py_BuildValue("(O)", poDict));
					Py_DECREF(poDict);
					PyCallClassMemberFunc(m_poHandler, "RefreshStatusEffects", Py_BuildValue("()"));
				}
			}
			return true;
		}

		case SUB_HEADER_GC_STATUS_EFFECT_ADD:
		{
			TPacketGCStatusEffectItem kItem;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(kItem), &kItem))
				return false;

			if (m_poHandler)
			{
				BYTE bStatusEffectType = IsUsingAffectFlagBit(kItem.dwType) ? STATUS_EFFECT_TYPE_AFFECT_FLAG : STATUS_EFFECT_TYPE_AFFECT_TYPE;
				bool is_buff = false;
				PyCallClassMemberFunc(m_poHandler, "AddAffect", Py_BuildValue("(ibiiiii)",
					bStatusEffectType,
					is_buff,
					kItem.dwType,
					kItem.wPointType,
					kItem.lPointValue,
					kItem.lDuration,
					kItem.lFlag));
				PyCallClassMemberFunc(m_poHandler, "RefreshStatusEffects", Py_BuildValue("()"));
			}
			return true;
		}

		case SUB_HEADER_GC_STATUS_EFFECT_REMOVE:
		case SUB_HEADER_GC_STATUS_EFFECT_REMOVE_2:
		{
			DWORD dwType;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(dwType), &dwType))
				return false;

			if (m_poHandler)
			{
				BYTE bStatusEffectType = IsUsingAffectFlagBit(dwType) ? STATUS_EFFECT_TYPE_AFFECT_FLAG : STATUS_EFFECT_TYPE_AFFECT_TYPE;
				PyCallClassMemberFunc(m_poHandler, "RemoveAffect", Py_BuildValue("(ib)", dwType, bStatusEffectType));
				PyCallClassMemberFunc(m_poHandler, "RefreshStatusEffects", Py_BuildValue("()"));
			}
			return true;
		}

		case SUB_HEADER_GC_STATUS_EFFECT_PET_SKILL:
		{
			TPacketGCStatusEffectPetSkill kSubPacket;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(kSubPacket), &kSubPacket))
				return false;

			if (m_poHandler)
			{
				PyCallClassMemberFunc(m_poHandler, "ClearPetSkill", Py_BuildValue("()"));
				DWORD dwCurTime = CTimer::Instance().GetCurrentMillisecond() / 1000;
				for (int i = 0; i < 3; ++i)
				{
					if (kSubPacket.adwPetSkill[i] == 0)
						continue;

					if (kSubPacket.adwCoolTime[i] > dwCurTime)
					{
						PyCallClassMemberFunc(m_poHandler, "AddPetSkillOnCoolTime", Py_BuildValue("(ii)",
							kSubPacket.adwPetSkill[i], kSubPacket.adwCoolTime[i]));
					}
					else
					{
						PyCallClassMemberFunc(m_poHandler, "AddPetSkill", Py_BuildValue("(i)",
							kSubPacket.adwPetSkill[i]));
					}
				}
				PyCallClassMemberFunc(m_poHandler, "RefreshStatusEffects", Py_BuildValue("()"));
			}
			return true;
		}

		case SUB_HEADER_GC_STATUS_EFFECT_CLEAR_PET_SKILL:
		{
			if (m_poHandler)
			{
				PyCallClassMemberFunc(m_poHandler, "ClearPetSkill", Py_BuildValue("()"));
				PyCallClassMemberFunc(m_poHandler, "RefreshStatusEffects", Py_BuildValue("()"));
			}
			return true;
		}

		default:
			TraceError("CPythonStatusEffectManager::RecvStatusEffectPacket: Unknown SubHeader %d", kHeader.bSubHeader);
			return false;
	}

	return true;
}

PyObject* statusEffectManagerIsUsingAffectFlagBit(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwAffect;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwAffect))
		return Py_BadArgument();

	return Py_BuildValue("i", CPythonStatusEffectManager::Instance().IsUsingAffectFlagBit(dwAffect));
}

PyObject* statusEffectManagerSetStatusEffectShowerHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BadArgument();

	CPythonStatusEffectManager::Instance().SetStatusEffectShowerHandler(poHandler);
	return Py_BuildNone();
}

void initStatusEffectManager()
{
	static PyMethodDef s_methods[] =
	{
		{ "SetStatusEffectShowerHandler", statusEffectManagerSetStatusEffectShowerHandler, METH_VARARGS },
		{ "IsUsingAffectFlagBit", statusEffectManagerIsUsingAffectFlagBit, METH_VARARGS },
		{ NULL, NULL, 0 },
	};

	PyObject* poModule = Py_InitModule("statusEffectManager", s_methods);
	PyModule_AddIntConstant(poModule, "STATUS_EFFECT_TYPE_AFFECT_TYPE", CPythonStatusEffectManager::STATUS_EFFECT_TYPE_AFFECT_TYPE);
	PyModule_AddIntConstant(poModule, "STATUS_EFFECT_TYPE_AFFECT_FLAG", CPythonStatusEffectManager::STATUS_EFFECT_TYPE_AFFECT_FLAG);
}
#endif // ENABLE_STATUS_EFFECT_SHOWER
