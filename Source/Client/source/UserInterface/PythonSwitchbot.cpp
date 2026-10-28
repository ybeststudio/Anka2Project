#include "StdAfx.h"

#ifdef ENABLE_RENEWAL_SWITCHBOT

#include "PythonSwitchbot.h"
#include "PythonNetworkStream.h"
#include "PythonPlayer.h"
#include "../GameLib/ItemManager.h"

CPythonSwitchbot::CPythonSwitchbot()
{
	Initialize();
}

CPythonSwitchbot::~CPythonSwitchbot()
{
	Initialize();
}

void CPythonSwitchbot::Initialize()
{
	m_SwitchbotTable = {};
	m_map_AttributesBySet.clear();
}

void CPythonSwitchbot::Update(const TSwitchbotTable& table)
{
	memcpy_s(m_SwitchbotTable.active, sizeof(m_SwitchbotTable.active), table.active, sizeof(table.active));
	memcpy_s(m_SwitchbotTable.finished, sizeof(m_SwitchbotTable.finished), table.finished, sizeof(table.finished));
	memcpy_s(m_SwitchbotTable.alternatives, sizeof(m_SwitchbotTable.alternatives), table.alternatives, sizeof(table.alternatives));
	memcpy_s(m_SwitchbotTable.items, sizeof(m_SwitchbotTable.items), table.items, sizeof(table.items));
	m_SwitchbotTable.player_id = table.player_id;
}

void CPythonSwitchbot::SetAttribute(BYTE slot, BYTE alternative, BYTE attrIdx, WORD attrType, int attrValue)
{
	if (slot >= SWITCHBOT_SLOT_COUNT)
	{
		return;
	}

	if (alternative >= SWITCHBOT_ALTERNATIVE_COUNT)
	{
		return;
	}

	m_SwitchbotTable.alternatives[slot][alternative].attributes[attrIdx].bType = attrType;
	m_SwitchbotTable.alternatives[slot][alternative].attributes[attrIdx].sValue = attrValue;
}

TPlayerItemAttribute CPythonSwitchbot::GetAttribute(BYTE slot, BYTE alternative, BYTE attrIdx)
{
	TPlayerItemAttribute attr = {};

	if (slot >= SWITCHBOT_SLOT_COUNT)
	{
		return attr;
	}

	if (alternative >= SWITCHBOT_ALTERNATIVE_COUNT)
	{
		return attr;
	}

	attr.bType = m_SwitchbotTable.alternatives[slot][alternative].attributes[attrIdx].bType;
	attr.sValue = m_SwitchbotTable.alternatives[slot][alternative].attributes[attrIdx].sValue;

	return attr;
}

void CPythonSwitchbot::GetAlternatives(BYTE slot, std::vector<TSwitchbotAttributeAlternativeTable>& vec_alternatives)
{
	if (slot >= SWITCHBOT_SLOT_COUNT)
	{
		return;
	}

	for (const auto& it : m_SwitchbotTable.alternatives[slot])
	{
		vec_alternatives.emplace_back(it);
	}
}

int CPythonSwitchbot::GetAttibuteSet(BYTE slot)
{
	if (slot >= SWITCHBOT_SLOT_COUNT)
	{
		return -1;
	}

	TItemPos pos(SWITCHBOT, slot);

	CItemData* pItemDataPtr = nullptr;
	BYTE item_type, item_subtype = 0;

	if (CItemManager::Instance().GetItemDataPointer(CPythonPlayer::Instance().GetItemIndex(pos), &pItemDataPtr))
	{
		item_type = pItemDataPtr->GetType();
		item_subtype = pItemDataPtr->GetSubType();
	}

	if (item_type == 0 && item_subtype == 0)
	{
		return -1;
	}

	if (item_type == CItemData::ITEM_TYPE_WEAPON)
	{
		if (item_subtype == CItemData::WEAPON_ARROW)
			return -1;
#ifdef ENABLE_QUIVER_SYSTEM
		else if (item_subtype == CItemData::WEAPON_QUIVER)
			return -1;
#endif

		return ATTRIBUTE_SET_WEAPON;
	}

	if (item_type == CItemData::ITEM_TYPE_ARMOR)
	{
		switch (item_subtype)
		{
		case CItemData::ARMOR_BODY:
			return ATTRIBUTE_SET_BODY;

		case CItemData::ARMOR_WRIST:
			return ATTRIBUTE_SET_WRIST;

		case CItemData::ARMOR_FOOTS:
			return ATTRIBUTE_SET_FOOTS;

		case CItemData::ARMOR_NECK:
			return ATTRIBUTE_SET_NECK;

		case CItemData::ARMOR_HEAD:
			return ATTRIBUTE_SET_HEAD;

		case CItemData::ARMOR_SHIELD:
			return ATTRIBUTE_SET_SHIELD;

		case CItemData::ARMOR_EAR:
			return ATTRIBUTE_SET_EAR;

#ifdef ENABLE_PENDANT_SYSTEM
		case CItemData::ARMOR_PENDANT:
			return ATTRIBUTE_SET_PENDANT;
#endif
		}
	}

#ifdef ENABLE_COSTUME_ATTR_SYSTEM
	if (item_type == CItemData::ITEM_TYPE_COSTUME)
	{
		switch (item_subtype)
		{
			case CItemData::COSTUME_BODY:
				return ATTRIBUTE_SET_COSTUME_BODY;

			case CItemData::COSTUME_HAIR:
				return ATTRIBUTE_SET_COSTUME_HAIR;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
			case CItemData::COSTUME_WEAPON:
				return ATTRIBUTE_SET_COSTUME_WEAPON;
#endif
		}
	}
#endif

	return -1;
}

bool CPythonSwitchbot::IsActive(BYTE slot)
{
	if (slot >= SWITCHBOT_SLOT_COUNT)
	{
		return false;
	}

	return m_SwitchbotTable.active[slot];
}

bool CPythonSwitchbot::IsFinished(BYTE slot)
{
	if (slot >= SWITCHBOT_SLOT_COUNT)
	{
		return false;
	}

	return m_SwitchbotTable.finished[slot];
}

void CPythonSwitchbot::ClearSlot(BYTE slot)
{
	if (slot >= SWITCHBOT_SLOT_COUNT)
	{
		return;
	}

	m_SwitchbotTable.active[slot] = false;
	m_SwitchbotTable.items[slot] = 0;
	memset(&m_SwitchbotTable.alternatives[slot], 0, sizeof(m_SwitchbotTable.alternatives[slot]));
}

void CPythonSwitchbot::ClearAttributeMap()
{
	m_map_AttributesBySet.clear();
}

void CPythonSwitchbot::AddAttributeToMap(const TSwitchbottAttributeTable& table)
{
	const auto& it = m_map_AttributesBySet.find(table.attribute_set);
	if (it == m_map_AttributesBySet.end())
	{
		std::map<BYTE, long> attribute_map;
		attribute_map.insert(std::make_pair(table.apply_num, table.max_value));

		m_map_AttributesBySet.insert(std::make_pair(table.attribute_set, attribute_map));
	}
	else
	{
		std::map<unsigned char, long>::iterator it2 = it->second.find(table.apply_num);
		if (it2 == it->second.end())
		{
			it->second.insert(std::make_pair(table.apply_num, table.max_value));
		}
		else
		{
			it2->second = table.max_value;
		}
	}
}

void CPythonSwitchbot::GetAttributesBySet(int iAttributeSet, std::vector<TPlayerItemAttribute>& vec_attributes)
{
	if (iAttributeSet == -1)
	{
		return;
	}

	const auto& it = m_map_AttributesBySet.find(iAttributeSet);
	if (it == m_map_AttributesBySet.end())
	{
		return;
	}

	for (const auto& it2 : it->second)
	{
		TPlayerItemAttribute attr;
		attr.bType = it2.first;
		attr.sValue = it2.second;

		vec_attributes.emplace_back(attr);
	}
}

long CPythonSwitchbot::GetAttributeMaxValue(int iAttributeSet, WORD attrType)
{
	if (iAttributeSet == -1)
	{
		return 0;
	}

	const auto& it = m_map_AttributesBySet.find(iAttributeSet);
	if (it == m_map_AttributesBySet.end())
	{
		return 0;
	}

	const auto& it2 = it->second.find(attrType);
	if (it2 == it->second.end())
	{
		return 0;
	}

	return it2->second;
}

PyObject *switchbotIsActive(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bSlot;
	if (!PyTuple_GetByte(poArgs, 0, &bSlot))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", CPythonSwitchbot::Instance().IsActive(bSlot));
}

PyObject *switchbotIsFinished(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bSlot;
	if (!PyTuple_GetByte(poArgs, 0, &bSlot))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", CPythonSwitchbot::Instance().IsFinished(bSlot));
}

PyObject *switchbotGetAttribute(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bSlot;
	if (!PyTuple_GetByte(poArgs, 0, &bSlot))
	{
		return Py_BuildException();
	}

	BYTE bAlternative;
	if (!PyTuple_GetByte(poArgs, 1, &bAlternative))
	{
		return Py_BuildException();
	}

	BYTE bAttrIndex;
	if (!PyTuple_GetByte(poArgs, 2, &bAttrIndex))
	{
		return Py_BuildException();
	}

	TPlayerItemAttribute attr = CPythonSwitchbot::Instance().GetAttribute(bSlot, bAlternative, bAttrIndex);
	return Py_BuildValue("ii", attr.bType, attr.sValue);
}

PyObject *switchbotSetAttribute(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bSlot;
	if (!PyTuple_GetByte(poArgs, 0, &bSlot))
	{
		return Py_BuildException();
	}

	BYTE bAlternative;
	if (!PyTuple_GetByte(poArgs, 1, &bAlternative))
	{
		return Py_BuildException();
	}

	BYTE bAttrIndex;
	if (!PyTuple_GetByte(poArgs, 2, &bAttrIndex))
	{
		return Py_BuildException();
	}

	int iType;
	if (!PyTuple_GetInteger(poArgs, 3, &iType))
	{
		return Py_BuildException();
	}

	int iValue;
	if (!PyTuple_GetInteger(poArgs, 4, &iValue))
	{
		return Py_BuildException();
	}

	CPythonSwitchbot::Instance().SetAttribute(bSlot, bAlternative, bAttrIndex, (WORD) iType, iValue);
	return Py_BuildNone();
}

PyObject *switchbotClearSlot(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bSlot;
	if (!PyTuple_GetByte(poArgs, 0, &bSlot))
	{
		return Py_BuildException();
	}

	CPythonSwitchbot::Instance().ClearSlot(bSlot);
	return Py_BuildNone();
}

PyObject *switchbotGetAttributesForSet(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bSlot;
	if (!PyTuple_GetByte(poArgs, 0, &bSlot))
	{
		return Py_BuildException();
	}

	PyObject *list = PyList_New(0);

	int iAttributeSet = CPythonSwitchbot::Instance().GetAttibuteSet(bSlot);
	if (iAttributeSet == -1)
	{
		return list;
	}

	std::vector<TPlayerItemAttribute> vec_attributes;
	CPythonSwitchbot::Instance().GetAttributesBySet(iAttributeSet, vec_attributes);

	struct
	{
		bool operator()(TPlayerItemAttribute a, TPlayerItemAttribute b) const
		{
			return a.bType < b.bType;
		}
	} SortAttributes;

	std::sort(vec_attributes.begin(), vec_attributes.end(), SortAttributes);

	for (TPlayerItemAttribute attr : vec_attributes)
	{
		PyList_Append(list, Py_BuildValue("(ii)", attr.bType, attr.sValue));
	}

	return list;
}

PyObject *switchbotGetAttributeMaxValue(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bSlot;
	if (!PyTuple_GetByte(poArgs, 0, &bSlot))
	{
		return Py_BuildException();
	}

	int iAttrType;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrType))
	{
		return Py_BuildException();
	}

	int iAttributeSet = CPythonSwitchbot::Instance().GetAttibuteSet(bSlot);
	if (iAttributeSet == -1)
	{
		return Py_BuildValue("i", 0);
	}
	
	long maxValue = CPythonSwitchbot::Instance().GetAttributeMaxValue(iAttributeSet, (WORD) iAttrType);
	return Py_BuildValue("i", maxValue);
}

PyObject *switchbotStart(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bSlot;
	if (!PyTuple_GetByte(poArgs, 0, &bSlot))
	{
		return Py_BuildException();
	}

	if (bSlot > SWITCHBOT_SLOT_COUNT)
	{
		return Py_BuildNone();
	}

	std::vector<CPythonSwitchbot::TSwitchbotAttributeAlternativeTable> vec_alternatives;
	CPythonSwitchbot::Instance().GetAlternatives(bSlot, vec_alternatives);

	CPythonNetworkStream::Instance().SendSwitchbotStartPacket(bSlot, vec_alternatives);
	return Py_BuildNone();
}

PyObject *switchbotStop(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bSlot;
	if (!PyTuple_GetByte(poArgs, 0, &bSlot))
	{
		return Py_BuildException();
	}

	if (bSlot > SWITCHBOT_SLOT_COUNT)
	{
		return Py_BuildNone();
	}

	CPythonNetworkStream::Instance().SendSwitchbotStopPacket(bSlot);
	return Py_BuildNone();
}

void initSwitchbot()
{
	static PyMethodDef s_methods[] =
	{
		{ "IsActive",				switchbotIsActive,				METH_VARARGS },
		{ "IsFinished",				switchbotIsFinished,			METH_VARARGS },	
		{ "GetAttribute",			switchbotGetAttribute,			METH_VARARGS },
		{ "SetAttribute",			switchbotSetAttribute,			METH_VARARGS },
		{ "GetAttributesForSet",	switchbotGetAttributesForSet,	METH_VARARGS },
		{ "GetAttributeMaxValue",	switchbotGetAttributeMaxValue,	METH_VARARGS },
		{ "ClearSlot",				switchbotClearSlot,				METH_VARARGS },
		{ "Start",					switchbotStart,					METH_VARARGS },
		{ "Stop",					switchbotStop,					METH_VARARGS },

		{ nullptr, nullptr },
	};

	PyObject *poModule = Py_InitModule("switchbot", s_methods);

	PyModule_AddIntConstant(poModule, "SLOT_COUNT", SWITCHBOT_SLOT_COUNT);
	PyModule_AddIntConstant(poModule, "ALTERNATIVE_COUNT", SWITCHBOT_ALTERNATIVE_COUNT);
}

#endif