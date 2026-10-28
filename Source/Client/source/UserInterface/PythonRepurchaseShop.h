#pragma once

#include "Packet.h"
#include <deque>

#ifdef ENABLE_SHOP_REPURCHASE
class CPythonRepurchaseShop : public CSingleton<CPythonRepurchaseShop>
{
public:
	enum
	{
		SLOT_MAX_COUNT = 12,
		MAX_SHORT_COOLDOWN_REPURCHASE_COUNT = 8,
		REPURCHASE_SHORT_COOLDOWN_MS = 1000,
		REPURCHASE_LONG_COOLDOWN_MS = 60000,

		REPURCHASE_RESULT_FAIL_INVALID_ITEM = 3,
		REPURCHASE_RESULT_FAIL_NOT_ENOUGH_INVENTORY = 4,
		REPURCHASE_RESULT_FAIL_COOLDOWN = 6,
	};

	struct TRepurchaseItemData
	{
		DWORD dwID;
		TItemData item;
		DWORD dwPrice;
		__int64 tExpireTime;
	};

public:
	CPythonRepurchaseShop();
	virtual ~CPythonRepurchaseShop();

	void Clear();
	void SetInterfaceHandler(PyObject* poHandler);
	void ProcessRepurchaseItemResult(BYTE bResult);

	bool SendShopOpen();
	bool SendShopClose();
	bool SendItemRepurchase(WORD wPos);

	DWORD GetItemPrice(WORD wPos);
	DWORD GetItemExpireTime(WORD wPos);
	void RemoveExpiredItems(time_t curTime);

	const TItemData* GetItemData(WORD wPos) const;
	bool ProcessPacket(BYTE bSubHeader);

private:
	std::deque<TRepurchaseItemData> m_RepurchaseItemDeque;
	PyObject* m_poInterfaceHandler;
};

void initrepurchaseShop();
#endif // ENABLE_SHOP_REPURCHASE
