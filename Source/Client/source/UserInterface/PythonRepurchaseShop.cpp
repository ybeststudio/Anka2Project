#include "StdAfx.h"
#include "PythonRepurchaseShop.h"
#include "PythonNetworkStream.h"
#include "PythonApplication.h"
#ifdef ENABLE_SHOP_REPURCHASE

CPythonRepurchaseShop::CPythonRepurchaseShop()
	: m_poInterfaceHandler(NULL)
{
	m_RepurchaseItemDeque.clear();
}

CPythonRepurchaseShop::~CPythonRepurchaseShop()
{
	Clear();
}

void CPythonRepurchaseShop::Clear()
{
	m_RepurchaseItemDeque.clear();
}

void CPythonRepurchaseShop::SetInterfaceHandler(PyObject* poHandler)
{
	m_poInterfaceHandler = poHandler;
}

void CPythonRepurchaseShop::ProcessRepurchaseItemResult(BYTE bResult)
{
	if (m_poInterfaceHandler)
		PyCallClassMemberFunc(m_poInterfaceHandler, "ProcessRepurchaseItemResult", Py_BuildValue("(i)", bResult));
}

bool CPythonRepurchaseShop::SendShopOpen()
{
	if (!CPythonNetworkStream::Instance().IsGameOnline())
		return false;

	TPacketCGShopRepurchaseOpen packet;
	packet.header = HEADER_CG_SHOP;
	packet.size = sizeof(packet);
	packet.subheader = SHOP_SUBHEADER_CG_REPURCHASE_OPEN;

	return CPythonNetworkStream::Instance().Send(sizeof(packet), &packet);
}

bool CPythonRepurchaseShop::SendShopClose()
{
	if (!CPythonNetworkStream::Instance().IsGameOnline())
		return false;

	TPacketCGShopRepurchaseClose packet;
	packet.header = HEADER_CG_SHOP;
	packet.size = sizeof(packet);
	packet.subheader = SHOP_SUBHEADER_CG_REPURCHASE_CLOSE;

	return CPythonNetworkStream::Instance().Send(sizeof(packet), &packet);
}

bool CPythonRepurchaseShop::SendItemRepurchase(WORD wPos)
{
	if (!CPythonNetworkStream::Instance().IsGameOnline())
		return false;

	if (wPos >= SLOT_MAX_COUNT || wPos >= m_RepurchaseItemDeque.size())
		return false;

	const TRepurchaseItemData& rItem = m_RepurchaseItemDeque[wPos];
	if (rItem.item.vnum == 0)
		return false;

	TPacketCGShopRepurchase packet;
	packet.header = HEADER_CG_SHOP;
	packet.size = sizeof(packet);
	packet.subheader = SHOP_SUBHEADER_CG_REPURCHASE;
	packet.item_id = rItem.dwID;
	packet.pos = wPos;
	packet.count = (BYTE)rItem.item.count;

	return CPythonNetworkStream::Instance().Send(sizeof(packet), &packet);
}

DWORD CPythonRepurchaseShop::GetItemPrice(WORD wPos)
{
	if (wPos >= m_RepurchaseItemDeque.size())
		return 0;

	return m_RepurchaseItemDeque[wPos].dwPrice;
}

DWORD CPythonRepurchaseShop::GetItemExpireTime(WORD wPos)
{
	if (wPos >= m_RepurchaseItemDeque.size())
		return 0;

	return (DWORD)m_RepurchaseItemDeque[wPos].tExpireTime;
}

void CPythonRepurchaseShop::RemoveExpiredItems(time_t curTime)
{
	bool bRemoved = false;
	for (std::deque<TRepurchaseItemData>::iterator it = m_RepurchaseItemDeque.begin(); it != m_RepurchaseItemDeque.end(); )
	{
		if (it->tExpireTime > 0 && it->tExpireTime <= curTime)
		{
			it = m_RepurchaseItemDeque.erase(it);
			bRemoved = true;
		}
		else
		{
			++it;
		}
	}

	if (bRemoved && m_poInterfaceHandler)
	{
		PyCallClassMemberFunc(m_poInterfaceHandler, "Refresh", Py_BuildValue("()"));
	}
}

const TItemData* CPythonRepurchaseShop::GetItemData(WORD wPos) const
{
	if (wPos >= m_RepurchaseItemDeque.size())
		return NULL;

	return &m_RepurchaseItemDeque[wPos].item;
}

bool CPythonRepurchaseShop::ProcessPacket(BYTE bSubHeader)
{
	switch (bSubHeader)
	{
		case SHOP_SUBHEADER_GC_REPURCHASE_OPEN:
		{
			if (m_poInterfaceHandler)
			{
				PyCallClassMemberFunc(m_poInterfaceHandler, "Open", Py_BuildValue("()"));
				PyCallClassMemberFunc(m_poInterfaceHandler, "ResetOpenSendFlag", Py_BuildValue("()"));
			}
			return true;
		}

		case SHOP_SUBHEADER_GC_REPURCHASE_CLOSE:
		{
			if (m_poInterfaceHandler)
			{
				PyCallClassMemberFunc(m_poInterfaceHandler, "Close", Py_BuildValue("()"));
				PyCallClassMemberFunc(m_poInterfaceHandler, "ResetCloseSendFlag", Py_BuildValue("()"));
			}
			return true;
		}

		case SHOP_SUBHEADER_GC_REPURCHASE_ADD_ITEM:
		{
			TPacketGCItemSet2 itemSetPacket;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(TPacketGCItemSet2), &itemSetPacket))
				return false;

			TSubPacketGCShopRepurchaseItem extraInfo;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(TSubPacketGCShopRepurchaseItem), &extraInfo))
				return false;

			TRepurchaseItemData itemData;
			itemData.dwID = extraInfo.dwID;
			itemData.dwPrice = extraInfo.dwPrice;
			itemData.tExpireTime = extraInfo.tExpireTime;

			itemData.item.vnum = itemSetPacket.vnum;
			itemData.item.count = itemSetPacket.count;
			itemData.item.flags = itemSetPacket.flags;
			itemData.item.anti_flags = itemSetPacket.anti_flags;
			for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
				itemData.item.alSockets[i] = itemSetPacket.alSockets[i];
			for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
				itemData.item.aAttr[j] = itemSetPacket.aAttr[j];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
			itemData.item.dwTransmutationVnum = itemSetPacket.dwTransmutationVnum;
#endif

			m_RepurchaseItemDeque.push_back(itemData);
			if (m_RepurchaseItemDeque.size() > SLOT_MAX_COUNT)
				m_RepurchaseItemDeque.pop_front();

			if (m_poInterfaceHandler)
				PyCallClassMemberFunc(m_poInterfaceHandler, "Refresh", Py_BuildValue("()"));

			return true;
		}

		case SHOP_SUBHEADER_GC_REPURCHASE_RESULT:
		{
			TPacketGCShopRepurchaseResult resultPacket;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(TPacketGCShopRepurchaseResult), &resultPacket))
				return false;

			if (resultPacket.bResult == 1)
			{
				if (resultPacket.wPos < m_RepurchaseItemDeque.size())
				{
					m_RepurchaseItemDeque.erase(m_RepurchaseItemDeque.begin() + resultPacket.wPos);
					if (m_poInterfaceHandler)
						PyCallClassMemberFunc(m_poInterfaceHandler, "Refresh", Py_BuildValue("()"));
				}
			}

			if (m_poInterfaceHandler)
				PyCallClassMemberFunc(m_poInterfaceHandler, "ProcessRepurchaseItemResult", Py_BuildValue("(i)", resultPacket.bResult));

			return true;
		}

		case SHOP_SUBHEADER_GC_REPURCHASE_ITEM_LIST:
		{
			BYTE bCount = 0;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(BYTE), &bCount))
				return false;

			if (bCount > SLOT_MAX_COUNT)
				return false;

			m_RepurchaseItemDeque.clear();

			for (BYTE i = 0; i < bCount; ++i)
			{
				TPacketGCItemSet2 itemSetPacket;
				if (!CPythonNetworkStream::Instance().Recv(sizeof(TPacketGCItemSet2), &itemSetPacket))
					return false;

				TSubPacketGCShopRepurchaseItem extraInfo;
				if (!CPythonNetworkStream::Instance().Recv(sizeof(TSubPacketGCShopRepurchaseItem), &extraInfo))
					return false;

				TRepurchaseItemData itemData;
				itemData.dwID = extraInfo.dwID;
				itemData.dwPrice = extraInfo.dwPrice;
				itemData.tExpireTime = extraInfo.tExpireTime;

				itemData.item.vnum = itemSetPacket.vnum;
				itemData.item.count = itemSetPacket.count;
				itemData.item.flags = itemSetPacket.flags;
				itemData.item.anti_flags = itemSetPacket.anti_flags;
				for (int k = 0; k < ITEM_SOCKET_SLOT_MAX_NUM; ++k)
					itemData.item.alSockets[k] = itemSetPacket.alSockets[k];
				for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
					itemData.item.aAttr[j] = itemSetPacket.aAttr[j];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
				itemData.item.dwTransmutationVnum = itemSetPacket.dwTransmutationVnum;
#endif

				m_RepurchaseItemDeque.push_back(itemData);
			}

			if (m_poInterfaceHandler)
				PyCallClassMemberFunc(m_poInterfaceHandler, "Refresh", Py_BuildValue("()"));

			return true;
		}

		default:
			break;
	}

	return false;
}

PyObject* repurchaseShopSetInterfaceHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonRepurchaseShop::Instance().SetInterfaceHandler(poHandler);
	return Py_BuildNone();
}

PyObject* repurchaseShopGetItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonRepurchaseShop::Instance().GetItemPrice((WORD)iPos));
}

PyObject* repurchaseShopGetItemExpireTime(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonRepurchaseShop::Instance().GetItemExpireTime((WORD)iPos));
}

PyObject* repurchaseShopRemoveExpiredItems(PyObject* poSelf, PyObject* poArgs)
{
	CPythonRepurchaseShop::Instance().RemoveExpiredItems(CPythonApplication::Instance().GetServerTimeStamp());
	return Py_BuildNone();
}

PyObject* repurchaseShopClear(PyObject* poSelf, PyObject* poArgs)
{
	CPythonRepurchaseShop::Instance().Clear();
	return Py_BuildNone();
}

PyObject* repurchaseShopSendShopOpen(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonRepurchaseShop::Instance().SendShopOpen());
}

PyObject* repurchaseShopSendShopClose(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonRepurchaseShop::Instance().SendShopClose());
}

PyObject* repurchaseShopSendItemRepurchase(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	return Py_BuildValue("b", CPythonRepurchaseShop::Instance().SendItemRepurchase((WORD)iPos));
}

void initrepurchaseShop()
{
	static PyMethodDef s_methods[] =
	{
		{ "SetInterfaceHandler",	repurchaseShopSetInterfaceHandler,	METH_VARARGS },
		{ "GetItemPrice",			repurchaseShopGetItemPrice,			METH_VARARGS },
		{ "GetItemExpireTime",		repurchaseShopGetItemExpireTime,	METH_VARARGS },
		{ "RemoveExpiredItems",		repurchaseShopRemoveExpiredItems,	METH_VARARGS },
		{ "Clear",					repurchaseShopClear,				METH_VARARGS },
		{ "SendShopOpen",			repurchaseShopSendShopOpen,			METH_VARARGS },
		{ "SendShopClose",			repurchaseShopSendShopClose,		METH_VARARGS },
		{ "SendItemRepurchase",		repurchaseShopSendItemRepurchase,	METH_VARARGS },
		{ NULL,						NULL,								0 },
	};

	PyObject* poModule = Py_InitModule("repurchaseShop", s_methods);

	PyModule_AddIntConstant(poModule, "SLOT_MAX_COUNT", CPythonRepurchaseShop::SLOT_MAX_COUNT);
	PyModule_AddIntConstant(poModule, "MAX_SHORT_COOLDOWN_REPURCHASE_COUNT", CPythonRepurchaseShop::MAX_SHORT_COOLDOWN_REPURCHASE_COUNT);
	PyModule_AddIntConstant(poModule, "REPURCHASE_SHORT_COOLDOWN_MS", CPythonRepurchaseShop::REPURCHASE_SHORT_COOLDOWN_MS);
	PyModule_AddIntConstant(poModule, "REPURCHASE_LONG_COOLDOWN_MS", CPythonRepurchaseShop::REPURCHASE_LONG_COOLDOWN_MS);
	PyModule_AddIntConstant(poModule, "REPURCHASE_RESULT_FAIL_NOT_ENOUGH_INVENTORY", CPythonRepurchaseShop::REPURCHASE_RESULT_FAIL_NOT_ENOUGH_INVENTORY);
	PyModule_AddIntConstant(poModule, "REPURCHASE_RESULT_FAIL_INVALID_ITEM", CPythonRepurchaseShop::REPURCHASE_RESULT_FAIL_INVALID_ITEM);
	PyModule_AddIntConstant(poModule, "REPURCHASE_RESULT_FAIL_COOLDOWN", CPythonRepurchaseShop::REPURCHASE_RESULT_FAIL_COOLDOWN);
}
#endif // ENABLE_SHOP_REPURCHASE
