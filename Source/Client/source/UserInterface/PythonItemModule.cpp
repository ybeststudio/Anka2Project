#include "StdAfx.h"
#include "PythonItem.h"

#include "../gamelib/ItemManager.h"
#include "../gamelib/ItemData.h"
#include "InstanceBase.h"
#include "AbstractApplication.h"
#include "Locale_inc.h"
#include "PythonPlayer.h"
#include "PythonSafeBox.h"
#include "PythonExchange.h"

extern int TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;

PyObject *itemSetUseSoundFileName(PyObject *poSelf, PyObject *poArgs)
{
	int iUseSound;
	if (!PyTuple_GetInteger(poArgs, 0, &iUseSound))
		return Py_BadArgument();

	char *szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.SetUseSoundFileName(iUseSound, szFileName);
	return Py_BuildNone();
}

PyObject *itemSetDropSoundFileName(PyObject *poSelf, PyObject *poArgs)
{
	int iDropSound;
	if (!PyTuple_GetInteger(poArgs, 0, &iDropSound))
		return Py_BadArgument();

	char *szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
		return Py_BadArgument();

	CPythonItem& rkItem=CPythonItem::Instance();
	rkItem.SetDropSoundFileName(iDropSound, szFileName);
	return Py_BuildNone();
}

PyObject *itemSelectItem(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	if (!CItemManager::Instance().SelectItemData(iIndex))
	{
		CItemManager::Instance().SelectItemData(60001);
	}

	return Py_BuildNone();
}




PyObject *itemGetItemName(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("s", pItemData->GetName());
}

PyObject *itemGetItemDescription(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("s", pItemData->GetDescription());
}

PyObject *itemGetItemSummary(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("s", pItemData->GetSummary());
}

PyObject *itemGetIconImage(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

//	if (CItemData::ITEM_TYPE_SKILLBOOK == pItemData->GetType())
//	{
//		char szItemName[64+1];
//		_snprintf(szItemName, "d:/ymir work/ui/items/etc/book_%02d.sub", );
//		CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(szItemName);
//	}

	return Py_BuildPointer(pItemData->GetIconImage());
}

PyObject *itemGetIconImageFileName(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	CGraphicSubImage * pImage = pItemData->GetIconImage();
	if (!pImage)
		return Py_BuildValue("s", "Noname");

	return Py_BuildValue("s", pImage->GetFileName());
}

PyObject *itemGetItemSize(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("(ii)", 1, pItemData->GetSize());
}

PyObject *itemGetItemType(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetType());
}

PyObject *itemGetItemSubType(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetSubType());
}

PyObject *itemGetItemMaskType(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetItemMaskType());
}

PyObject *itemGetItemMaskSubType(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetItemMaskSubType());
}

PyObject *itemGetAttr67MaterialVnum(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->GetAttr67MaterialVnum());
}

PyObject *itemGetRefineLevel(PyObject *poSelf, PyObject *poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BadArgument();

	CItemData * pItemData = nullptr;
	if (!CItemManager::Instance().GetItemDataPointer(dwVnum, &pItemData) || !pItemData)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pItemData->GetRefineLevel());
}

PyObject *itemIsCostumeMountItem(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bType;
	if (!PyTuple_GetByte(poArgs, 0, &bType))
		return Py_BadArgument();
	BYTE bSubType;
	if (!PyTuple_GetByte(poArgs, 1, &bSubType))
		return Py_BadArgument();

	return Py_BuildValue("b", CItemData::IsCostumeMountItem(bType, bSubType));
}

#ifdef ENABLE_WHITE_DRAGON
PyObject *itemIsWhiteDragonBlessItem(PyObject *poSelf, PyObject *poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildValue("b", false);

	return Py_BuildValue("b", CItemData::IsWhiteDragonBlessItem(dwVnum));
}
#endif

PyObject *itemIsIronDragonBlessItem(PyObject *poSelf, PyObject *poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildValue("b", false);

	return Py_BuildValue("b", CItemData::IsIronDragonBlessItem(dwVnum));
}

PyObject *itemGetDSBasicApplyCount(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bDSType;
	if (!PyTuple_GetByte(poArgs, 0, &bDSType))
		return Py_BadArgument();
	BYTE bDSGrade;
	if (!PyTuple_GetByte(poArgs, 1, &bDSGrade))
		return Py_BadArgument();

	return Py_BuildValue("i", 0);
}

PyObject *itemGetDSBasicApplyValue(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bDSType;
	if (!PyTuple_GetByte(poArgs, 0, &bDSType))
		return Py_BadArgument();
	BYTE bDSGrade;
	if (!PyTuple_GetByte(poArgs, 1, &bDSGrade))
		return Py_BadArgument();

	return Py_BuildValue("i", 0);
}

PyObject *itemGetDSAdditionalApplyValue(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bDSType;
	if (!PyTuple_GetByte(poArgs, 0, &bDSType))
		return Py_BadArgument();
	BYTE bDSGrade;
	if (!PyTuple_GetByte(poArgs, 1, &bDSGrade))
		return Py_BadArgument();

	return Py_BuildValue("i", 0);
}

PyObject *itemGetDSSetWeight(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bDSType;
	if (!PyTuple_GetByte(poArgs, 0, &bDSType))
		return Py_BadArgument();
	BYTE bDSGrade;
	if (!PyTuple_GetByte(poArgs, 1, &bDSGrade))
		return Py_BadArgument();

	return Py_BuildValue("f", 0.0f);
}

PyObject *itemGetIBuyItemPrice(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	if (!pItemData)
		return Py_BuildException("no selected item data");

#ifdef ENABLE_GOLD_LIMIT
	return PyLong_FromLongLong(pItemData->GetIBuyItemPrice());
#else
	return Py_BuildValue("i", pItemData->GetIBuyItemPrice());
#endif
}

PyObject *itemGetISellItemPrice(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	if (!pItemData)
		return Py_BuildException("no selected item data");

#ifdef ENABLE_GOLD_LIMIT
	return PyLong_FromLongLong(pItemData->GetISellItemPrice());
#else
	return Py_BuildValue("i", pItemData->GetISellItemPrice());
#endif
}


PyObject *itemIsAntiFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsAntiFlag(iFlag));
}

PyObject *itemIsFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsFlag(iFlag));
}

PyObject *itemIsWearableFlag(PyObject *poSelf, PyObject *poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsWearableFlag(iFlag));
}

PyObject *itemIs1GoldItem(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", pItemData->IsFlag(CItemData::ITEM_FLAG_COUNT_PER_1GOLD));
}

PyObject *itemGetLimit(PyObject *poSelf, PyObject *poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	CItemData::TItemLimit ItemLimit;
	if (!pItemData->GetLimit(iValueIndex, &ItemLimit))
		return Py_BuildException();

	return Py_BuildValue("ii", ItemLimit.bType, ItemLimit.lValue);
}

PyObject *itemGetLimitType(PyObject *poSelf, PyObject *poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	CItemData::TItemLimit ItemLimit;
	if (!pItemData->GetLimit(iValueIndex, &ItemLimit))
		return Py_BuildException();

	return Py_BuildValue("i", ItemLimit.bType);
}

PyObject *itemGetLimitValue(PyObject *poSelf, PyObject *poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	CItemData::TItemLimit ItemLimit;
	if (!pItemData->GetLimit(iValueIndex, &ItemLimit))
		return Py_BuildException();

	return Py_BuildValue("i", ItemLimit.lValue);
}

PyObject *itemGetAffect(PyObject *poSelf, PyObject *poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	CItemData::TItemApply ItemApply;
	if (!pItemData->GetApply(iValueIndex, &ItemApply))
		return Py_BuildException();

	if ((CItemData::APPLY_ATT_SPEED == ItemApply.bType) && (CItemData::ITEM_TYPE_WEAPON == pItemData->GetType()) && (CItemData::WEAPON_TWO_HANDED == pItemData->GetSubType()))
	{
		ItemApply.lValue -= TWOHANDED_WEWAPON_ATT_SPEED_DECREASE_VALUE;
	}

	return Py_BuildValue("ii", ItemApply.bType, ItemApply.lValue);
}

PyObject *itemGetValue(PyObject *poSelf, PyObject *poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	return Py_BuildValue("i", pItemData->GetValue(iValueIndex));
}

PyObject *itemGetSocket(PyObject *poSelf, PyObject *poArgs)
{
	int iValueIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iValueIndex))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	return Py_BuildValue("i", pItemData->GetSocket(iValueIndex));
}

PyObject *itemGetIconInstance(PyObject *poSelf, PyObject *poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	CGraphicSubImage * pImage = pItemData->GetIconImage();
	if (!pImage)
		return Py_BuildException("Cannot get icon image by %d", pItemData->GetIndex());

	CGraphicImageInstance * pImageInstance = CGraphicImageInstance::New();
	pImageInstance->SetImagePointer(pImage);

	return Py_BuildPointer(pImageInstance);
}

PyObject *itemDeleteIconInstance(PyObject *poSelf, PyObject *poArgs)
{
	CGraphicImageInstance* pImageInstance = nullptr;
	if (!PyTuple_GetPointer(poArgs, 0, &pImageInstance))
		return Py_BadArgument();

	CGraphicImageInstance::Delete(pImageInstance);

	return Py_BuildNone();
}

PyObject *itemIsEquipmentVID(PyObject *poSelf, PyObject *poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Not yet select item data");

	return Py_BuildValue("i", pItemData->IsEquipment());
}

PyObject *itemGetUseType(PyObject *poSelf, PyObject *poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("s", pItemData->GetUseTypeString());	
}

PyObject *itemIsRefineScroll(PyObject *poSelf, PyObject *poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (pItemData->GetType() != CItemData::ITEM_TYPE_USE)
		return Py_BuildValue("i", FALSE);

	switch (pItemData->GetSubType())
	{
		case CItemData::USE_TUNING:
			return Py_BuildValue("i", TRUE);
			break;
	}

	return Py_BuildValue("i", FALSE);
}

PyObject *itemIsDetachScroll(PyObject *poSelf, PyObject *poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	int iType = pItemData->GetType();
	int iSubType = pItemData->GetSubType();
	if (iType == CItemData::ITEM_TYPE_USE)
	if (iSubType == CItemData::USE_DETACHMENT)
	{
		return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject *itemCanAddToQuickSlotItem(PyObject *poSelf, PyObject *poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (CItemData::ITEM_TYPE_USE == pItemData->GetType() || CItemData::ITEM_TYPE_QUEST == pItemData->GetType())
	{
		return Py_BuildValue("i", TRUE);
	}

#ifdef ENABLE_GROWTH_PET_SYSTEM
	if (CItemData::ITEM_TYPE_GROWTH_PET == pItemData->GetType() && CItemData::PET_UPBRINGING == pItemData->GetSubType())
	{
		return Py_BuildValue("i", TRUE);
	}
#endif

	return Py_BuildValue("i", FALSE);
}

PyObject *itemIsKey(PyObject *poSelf, PyObject *poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (CItemData::ITEM_TYPE_TREASURE_KEY == pItemData->GetType())
	{
		return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject *itemIsMetin(PyObject *poSelf, PyObject *poArgs)
{
	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemIndex))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemIndex);
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (CItemData::ITEM_TYPE_METIN == pItemData->GetType())
	{
		return Py_BuildValue("i", TRUE);
	}

	return Py_BuildValue("i", FALSE);
}

PyObject *itemRender(PyObject *poSelf, PyObject *poArgs)
{
	CPythonItem::Instance().Render();
	return Py_BuildNone();
}

PyObject *itemUpdate(PyObject *poSelf, PyObject *poArgs)
{
	IAbstractApplication& rkApp=IAbstractApplication::GetSingleton();

	POINT ptMouse;
	rkApp.GetMousePosition(&ptMouse);

	CPythonItem::Instance().Update(ptMouse);
	return Py_BuildNone();
}

PyObject *itemCreateItem(PyObject *poSelf, PyObject *poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BadArgument();
	int iVirtualNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iVirtualNumber))
		return Py_BadArgument();

	float x;
	if (!PyTuple_GetFloat(poArgs, 2, &x))
		return Py_BadArgument();
	float y;
	if (!PyTuple_GetFloat(poArgs, 3, &y))
		return Py_BadArgument();
	float z;
	if (!PyTuple_GetFloat(poArgs, 4, &z))
		return Py_BadArgument();

	bool bDrop = true;
	PyTuple_GetBoolean(poArgs, 5, &bDrop);

	CPythonItem::Instance().CreateItem(iVirtualID, iVirtualNumber, x, y, z, bDrop);

	return Py_BuildNone();
}

PyObject *itemDeleteItem(PyObject *poSelf, PyObject *poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BadArgument();

	CPythonItem::Instance().DeleteItem(iVirtualID);
	return Py_BuildNone();
}

PyObject *itemPick(PyObject *poSelf, PyObject *poArgs)
{
	DWORD dwItemID;
	if (CPythonItem::Instance().GetPickedItemID(&dwItemID))
		return Py_BuildValue("i", dwItemID);
	else
		return Py_BuildValue("i", -1);
}

PyObject *itemLoadItemTable(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BadArgument();

	CItemManager::Instance().LoadItemTable(szFileName);
	return Py_BuildNone();
}

PyObject *itemIsWeddingItem(PyObject *poSelf, PyObject *poArgs)
{
	DWORD dwItemIndex;
	switch (PyTuple_Size(poArgs))
	{
	case 0:
	{
		CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
		if (!pItemData)
			return Py_BuildException("No item selected!");
		dwItemIndex = pItemData->GetIndex();
		break;
	}
	case 1:
	{
		if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwItemIndex))
			return Py_BuildException();
		break;
	}
	default:
		return Py_BuildException();
	}

	switch (dwItemIndex)
	{
		case CItemData::WEDDING_TUXEDO1:
		case CItemData::WEDDING_TUXEDO2:
		case CItemData::WEDDING_TUXEDO3:
		case CItemData::WEDDING_TUXEDO4:
		case CItemData::WEDDING_BRIDE_DRESS1:
		case CItemData::WEDDING_BRIDE_DRESS2:
		case CItemData::WEDDING_BRIDE_DRESS3:
		case CItemData::WEDDING_BRIDE_DRESS4:
		case CItemData::WEDDING_BOUQUET1:
		case CItemData::WEDDING_BOUQUET2:
			return Py_BuildValue("i", TRUE);
			break;
	}

	return Py_BuildValue("i", FALSE);
}

#if defined(__BL__DETAILS_UI__)
#include "Packet.h"

static BYTE __GetApplyPointType(WORD bApplyType)
{
	switch (bApplyType)
	{
		case CItemData::APPLY_MAX_HP:	return EPointTypes::POINT_MAX_HP;
		case CItemData::APPLY_MAX_SP:	return EPointTypes::POINT_MAX_SP;
		case CItemData::APPLY_CON:	return EPointTypes::POINT_HT;
		case CItemData::APPLY_INT:	return EPointTypes::POINT_IQ;
		case CItemData::APPLY_STR:	return EPointTypes::POINT_ST;
		case CItemData::APPLY_DEX:	return EPointTypes::POINT_DX;
		case CItemData::APPLY_ATT_SPEED:	return EPointTypes::POINT_ATT_SPEED;
		case CItemData::APPLY_MOV_SPEED:	return EPointTypes::POINT_MOV_SPEED;
		case CItemData::APPLY_CAST_SPEED:	return EPointTypes::POINT_CASTING_SPEED;
		case CItemData::APPLY_HP_REGEN:	return EPointTypes::POINT_HP_REGEN;
		case CItemData::APPLY_SP_REGEN:	return EPointTypes::POINT_SP_REGEN;
		case CItemData::APPLY_POISON_PCT:	return EPointTypes::POINT_POISON_PCT;
		case CItemData::APPLY_STUN_PCT:	return EPointTypes::POINT_STUN_PCT;
		case CItemData::APPLY_SLOW_PCT:	return EPointTypes::POINT_SLOW_PCT;
		case CItemData::APPLY_CRITICAL_PCT:	return EPointTypes::POINT_CRITICAL_PCT;
		case CItemData::APPLY_PENETRATE_PCT:	return EPointTypes::POINT_PENETRATE_PCT;
		case CItemData::APPLY_ATTBONUS_HUMAN:	return EPointTypes::POINT_ATTBONUS_HUMAN;
		case CItemData::APPLY_ATTBONUS_ANIMAL:	return EPointTypes::POINT_ATTBONUS_ANIMAL;
		case CItemData::APPLY_ATTBONUS_ORC:	return EPointTypes::POINT_ATTBONUS_ORC;
		case CItemData::APPLY_ATTBONUS_MILGYO:	return EPointTypes::POINT_ATTBONUS_MILGYO;
		case CItemData::APPLY_ATTBONUS_UNDEAD:	return EPointTypes::POINT_ATTBONUS_UNDEAD;
		case CItemData::APPLY_ATTBONUS_DEVIL:	return EPointTypes::POINT_ATTBONUS_DEVIL;
		case CItemData::APPLY_STEAL_HP:	return EPointTypes::POINT_STEAL_HP;
		case CItemData::APPLY_STEAL_SP:	return EPointTypes::POINT_STEAL_SP;
		case CItemData::APPLY_MANA_BURN_PCT:	return EPointTypes::POINT_MANA_BURN_PCT;
		case CItemData::APPLY_DAMAGE_SP_RECOVER:	return EPointTypes::POINT_DAMAGE_SP_RECOVER;
		case CItemData::APPLY_BLOCK:	return EPointTypes::POINT_BLOCK;
		case CItemData::APPLY_DODGE:	return EPointTypes::POINT_DODGE;
		case CItemData::APPLY_RESIST_SWORD:	return EPointTypes::POINT_RESIST_SWORD;
		case CItemData::APPLY_RESIST_TWOHAND:	return EPointTypes::POINT_RESIST_TWOHAND;
		case CItemData::APPLY_RESIST_DAGGER:	return EPointTypes::POINT_RESIST_DAGGER;
		case CItemData::APPLY_RESIST_BELL:	return EPointTypes::POINT_RESIST_BELL;
		case CItemData::APPLY_RESIST_FAN:	return EPointTypes::POINT_RESIST_FAN;
		case CItemData::APPLY_RESIST_BOW:	return EPointTypes::POINT_RESIST_BOW;
		case CItemData::APPLY_RESIST_FIRE:	return EPointTypes::POINT_RESIST_FIRE;
		case CItemData::APPLY_RESIST_ELEC:	return EPointTypes::POINT_RESIST_ELEC;
		case CItemData::APPLY_RESIST_MAGIC:	return EPointTypes::POINT_RESIST_MAGIC;
		case CItemData::APPLY_RESIST_WIND:	return EPointTypes::POINT_RESIST_WIND;
		case CItemData::APPLY_REFLECT_MELEE:	return EPointTypes::POINT_REFLECT_MELEE;
		case CItemData::APPLY_REFLECT_CURSE:	return EPointTypes::POINT_REFLECT_CURSE;
		case CItemData::APPLY_POISON_REDUCE:	return EPointTypes::POINT_POISON_REDUCE;
		case CItemData::APPLY_KILL_SP_RECOVER:	return EPointTypes::POINT_KILL_SP_RECOVER;
		case CItemData::APPLY_EXP_DOUBLE_BONUS:	return EPointTypes::POINT_EXP_DOUBLE_BONUS;
		case CItemData::APPLY_GOLD_DOUBLE_BONUS:	return EPointTypes::POINT_GOLD_DOUBLE_BONUS;
		case CItemData::APPLY_ITEM_DROP_BONUS:	return EPointTypes::POINT_ITEM_DROP_BONUS;
		case CItemData::APPLY_POTION_BONUS:	return EPointTypes::POINT_POTION_BONUS;
		case CItemData::APPLY_KILL_HP_RECOVER:	return EPointTypes::POINT_KILL_HP_RECOVER;
		case CItemData::APPLY_IMMUNE_STUN:	return EPointTypes::POINT_IMMUNE_STUN;
		case CItemData::APPLY_IMMUNE_SLOW:	return EPointTypes::POINT_IMMUNE_SLOW;
		case CItemData::APPLY_IMMUNE_FALL:	return EPointTypes::POINT_IMMUNE_FALL;
		case CItemData::APPLY_BOW_DISTANCE:	return EPointTypes::POINT_BOW_DISTANCE;
		case CItemData::APPLY_ATT_GRADE_BONUS:	return EPointTypes::POINT_ATT_GRADE_BONUS;
		case CItemData::APPLY_DEF_GRADE_BONUS:	return EPointTypes::POINT_DEF_GRADE_BONUS;
		case CItemData::APPLY_MAGIC_ATT_GRADE:	return EPointTypes::POINT_MAGIC_ATT_GRADE_BONUS;
		case CItemData::APPLY_MAGIC_DEF_GRADE:	return EPointTypes::POINT_MAGIC_DEF_GRADE_BONUS;
		case CItemData::APPLY_CURSE_PCT:	return EPointTypes::POINT_CURSE_PCT;
		case CItemData::APPLY_MAX_STAMINA:	return EPointTypes::POINT_MAX_STAMINA;
		case CItemData::APPLY_ATTBONUS_WARRIOR:	return EPointTypes::POINT_ATTBONUS_WARRIOR;
		case CItemData::APPLY_ATTBONUS_ASSASSIN:	return EPointTypes::POINT_ATTBONUS_ASSASSIN;
		case CItemData::APPLY_ATTBONUS_SURA:	return EPointTypes::POINT_ATTBONUS_SURA;
		case CItemData::APPLY_ATTBONUS_SHAMAN:	return EPointTypes::POINT_ATTBONUS_SHAMAN;
		case CItemData::APPLY_ATTBONUS_MONSTER:	return EPointTypes::POINT_ATTBONUS_MONSTER;
		case CItemData::APPLY_MALL_ATTBONUS:	return EPointTypes::POINT_ATT_BONUS;
		case CItemData::APPLY_MALL_DEFBONUS:	return EPointTypes::POINT_MALL_DEFBONUS;
		case CItemData::APPLY_MALL_EXPBONUS:	return EPointTypes::POINT_MALL_EXPBONUS;
		case CItemData::APPLY_MALL_ITEMBONUS:	return EPointTypes::POINT_MALL_ITEMBONUS;
		case CItemData::APPLY_MALL_GOLDBONUS:	return EPointTypes::POINT_MALL_GOLDBONUS;
		case CItemData::APPLY_MAX_HP_PCT:	return EPointTypes::POINT_MAX_HP_PCT;
		case CItemData::APPLY_MAX_SP_PCT:	return EPointTypes::POINT_MAX_SP_PCT;
		case CItemData::APPLY_SKILL_DAMAGE_BONUS:	return EPointTypes::POINT_SKILL_DAMAGE_BONUS;
		case CItemData::APPLY_NORMAL_HIT_DAMAGE_BONUS:	return EPointTypes::POINT_NORMAL_HIT_DAMAGE_BONUS;
		case CItemData::APPLY_SKILL_DEFEND_BONUS:	return EPointTypes::POINT_SKILL_DEFEND_BONUS;
		case CItemData::APPLY_NORMAL_HIT_DEFEND_BONUS:	return EPointTypes::POINT_NORMAL_HIT_DEFEND_BONUS;
		case CItemData::APPLY_PC_BANG_EXP_BONUS:	return EPointTypes::POINT_PC_BANG_EXP_BONUS;
		case CItemData::APPLY_PC_BANG_DROP_BONUS:	return EPointTypes::POINT_PC_BANG_DROP_BONUS;
		case CItemData::APPLY_RESIST_WARRIOR:	return EPointTypes::POINT_RESIST_WARRIOR;
		case CItemData::APPLY_RESIST_ASSASSIN:	return EPointTypes::POINT_RESIST_ASSASSIN;
		case CItemData::APPLY_RESIST_SURA:	return EPointTypes::POINT_RESIST_SURA;
		case CItemData::APPLY_RESIST_SHAMAN:	return EPointTypes::POINT_RESIST_SHAMAN;
		case CItemData::APPLY_ENERGY:	return EPointTypes::POINT_ENERGY;
		case CItemData::APPLY_DEF_GRADE:	return EPointTypes::POINT_DEF_GRADE;
		case CItemData::APPLY_COSTUME_ATTR_BONUS:	return EPointTypes::POINT_COSTUME_ATTR_BONUS;
		case CItemData::APPLY_MAGIC_ATTBONUS_PER:	return EPointTypes::POINT_MAGIC_ATT_BONUS_PER;
		case CItemData::APPLY_MELEE_MAGIC_ATTBONUS_PER:	return EPointTypes::POINT_MELEE_MAGIC_ATT_BONUS_PER;
		case CItemData::APPLY_RESIST_ICE:	return EPointTypes::POINT_RESIST_ICE;
		case CItemData::APPLY_RESIST_EARTH:	return EPointTypes::POINT_RESIST_EARTH;
		case CItemData::APPLY_RESIST_DARK:	return EPointTypes::POINT_RESIST_DARK;
		case CItemData::APPLY_ANTI_CRITICAL_PCT:	return EPointTypes::POINT_RESIST_CRITICAL;
		case CItemData::APPLY_ANTI_PENETRATE_PCT:	return EPointTypes::POINT_RESIST_PENETRATE;
		case CItemData::APPLY_ACCEDRAIN_RATE:	return EPointTypes::POINT_ACCEDRAIN_RATE;
		case CItemData::APPLY_RESIST_MAGIC_REDUCTION:	return EPointTypes::POINT_RESIST_MAGIC_REDUCTION;
		case CItemData::APPLY_ENCHANT_ELECT:	return EPointTypes::POINT_ENCHANT_ELECT;
		case CItemData::APPLY_ENCHANT_FIRE:	return EPointTypes::POINT_ENCHANT_FIRE;
		case CItemData::APPLY_ENCHANT_ICE:	return EPointTypes::POINT_ENCHANT_ICE;
		case CItemData::APPLY_ENCHANT_WIND:	return EPointTypes::POINT_ENCHANT_WIND;
		case CItemData::APPLY_ENCHANT_EARTH:	return EPointTypes::POINT_ENCHANT_EARTH;
		case CItemData::APPLY_ENCHANT_DARK:	return EPointTypes::POINT_ENCHANT_DARK;
		case CItemData::APPLY_ATTBONUS_CZ:	return EPointTypes::POINT_ATTBONUS_CZ;
		case CItemData::APPLY_ATTBONUS_INSECT:	return EPointTypes::POINT_ATTBONUS_INSECT;
		case CItemData::APPLY_ATTBONUS_DESERT:	return EPointTypes::POINT_ATTBONUS_DESERT;
		case CItemData::APPLY_ATTBONUS_SWORD:	return EPointTypes::POINT_ATTBONUS_SWORD;
		case CItemData::APPLY_ATTBONUS_TWOHAND:	return EPointTypes::POINT_ATTBONUS_TWOHAND;
		case CItemData::APPLY_ATTBONUS_DAGGER:	return EPointTypes::POINT_ATTBONUS_DAGGER;
		case CItemData::APPLY_ATTBONUS_BELL:	return EPointTypes::POINT_ATTBONUS_BELL;
		case CItemData::APPLY_ATTBONUS_FAN:	return EPointTypes::POINT_ATTBONUS_FAN;
		case CItemData::APPLY_ATTBONUS_BOW:	return EPointTypes::POINT_ATTBONUS_BOW;
		case CItemData::APPLY_RESIST_HUMAN:	return EPointTypes::POINT_RESIST_HUMAN;
		case CItemData::APPLY_RESIST_MOUNT_FALL:	return EPointTypes::POINT_RESIST_MOUNT_FALL;
		case CItemData::APPLY_ATTBONUS_STONE:	return EPointTypes::POINT_ATTBONUS_STONE;
		case CItemData::APPLY_SUNGMA_STR:	return EPointTypes::POINT_SUNGMA_STR;
		case CItemData::APPLY_SUNGMA_HP:	return EPointTypes::POINT_SUNGMA_HP;
		case CItemData::APPLY_SUNGMA_MOVE:	return EPointTypes::POINT_SUNGMA_MOVE;
		case CItemData::APPLY_SUNGMA_IMMUNE:	return EPointTypes::POINT_SUNGMA_IMMUNE;
		case CItemData::APPLY_ATTBONUS_BOSS:	return EPointTypes::POINT_ATTBONUS_BOSS;
		case CItemData::APPLY_ATTBONUS_ELEMENTS:	return EPointTypes::POINT_ATTBONUS_ELEMENTS;
		case CItemData::APPLY_ENCHANT_ELEMENTS:	return EPointTypes::POINT_ENCHANT_ELEMENTS;
		case CItemData::APPLY_ATTBONUS_CHARACTERS:	return EPointTypes::POINT_ATTBONUS_CHARACTERS;
		case CItemData::APPLY_ENCHANT_CHARACTERS:	return EPointTypes::POINT_ENCHANT_CHARACTERS;
		case CItemData::APPLY_RESIST_MONSTER:	return EPointTypes::POINT_RESIST_MONSTER;
		case CItemData::APPLY_ATTBONUS_MEDI_PVM:	return EPointTypes::POINT_ATTBONUS_MEDI_PVM;
		case CItemData::APPLY_ATTBONUS_PVM_STR:	return EPointTypes::POINT_ATTBONUS_PVM_STR;
		case CItemData::APPLY_ATTBONUS_PVM_BERSERKER:	return EPointTypes::POINT_ATTBONUS_PVM_BERSERKER;
		default:	return EPointTypes::POINT_NONE;
	}
}

PyObject* itemGetApplyPoint(PyObject* poSelf, PyObject* poArgs)
{
	int iApplyType;
	if (!PyTuple_GetInteger(poArgs, 0, &iApplyType))
		return Py_BuildException();

	const BYTE bApplyPointType{ __GetApplyPointType((WORD) iApplyType) };
	// Bazı server/derlemelerde yeni apply type'lar gelebilir.
	// Burada syserr spam yerine güvenli şekilde POINT_NONE döndür.
	if (bApplyPointType == POINT_MAX_NUM)
		return Py_BuildValue("i", POINT_NONE);

	return Py_BuildValue("i", bApplyPointType);
}
#endif

PyObject *itemGetItemNameByVnum(PyObject *poSelf, PyObject *poArgs)
{
	DWORD dwVirtualNumber;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVirtualNumber))
		return Py_BadArgument();

	CItemData *pItemData;
	if (!CItemManager::Instance().GetItemDataPointer(dwVirtualNumber, &pItemData))
		return Py_BuildException();

	return Py_BuildValue("s", pItemData->GetName());
}

#ifdef ENABLE_WIKI_SYSTEM
PyObject *itemGetRefineSet(PyObject *poSelf, PyObject *poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");
	return Py_BuildValue("i", pItemData->GetRefinedSet());
}

PyObject *itemGetRefinedVnum(PyObject *poSelf, PyObject *poArgs)
{
	CItemData* pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");
	return Py_BuildValue("i", pItemData->GetRefinedVnum());
}

PyObject *itemSelectItemWiki(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();
	if (!CItemManager::Instance().SelectItemData(iIndex))
		return Py_BuildValue("i", 0);
	return Py_BuildValue("i", 1);
}
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
#include <cctype>
PyObject *itemGetItemsByName(PyObject *poSelf, PyObject *poArgs)
{
	char *szItemName;
	if (!PyTuple_GetString(poArgs, 0, &szItemName))
		return Py_BadArgument();

	std::map<DWORD, CItemData*> m_vec_ItemRange = CItemManager::Instance().GetItems();
	int n = m_vec_ItemRange.size();
	std::string searchedFileName(szItemName);
	PyObject *poList = PyList_New(0);

	std::map<DWORD, CItemData*>::iterator i;
	for (i = m_vec_ItemRange.begin(); i != m_vec_ItemRange.end(); ++i)
	{
		CItemData* p = i->second;
		const CItemData::TItemTable* pTable = p->GetTable();
		if (pTable)
		{
			std::string tempFileName(pTable->szLocaleName);
			std::transform(tempFileName.begin(), tempFileName.end(), tempFileName.begin(),[](unsigned char c) { return std::tolower(c); });
			if (tempFileName.compare(0, searchedFileName.length(), searchedFileName) == 0)
			{
				PyObject *poItemVnum = PyInt_FromLong(pTable->dwVnum);
				PyList_Append(poList, poItemVnum);
			}
		}
	}
	return poList;
}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
PyObject *itemIsPossibleChangeLookLeft(PyObject *poSelf, PyObject *poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData *pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("b", CPythonItem::Instance().CanAddChangeLookItem(pItemData, nullptr));
}

PyObject *itemIsPossibleChangeLookRight(PyObject *poSelf, PyObject *poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	int iOtherItemVID;
	if (!PyTuple_GetInteger(poArgs, 1, &iOtherItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData *pItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	CItemManager::Instance().SelectItemData(iOtherItemVID);
	CItemData *pOtherItemData = CItemManager::Instance().GetSelectedItemDataPointer();

	return Py_BuildValue("b", CPythonItem::Instance().CanAddChangeLookItem(pItemData, pOtherItemData));
}

PyObject *itemIsChangeLookFreePassYangItem(PyObject *poSelf, PyObject *poArgs)
{
	DWORD dwVirtualNumber;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVirtualNumber))
		return Py_BadArgument();

	return Py_BuildValue("b", CPythonItem::Instance().CanAddChangeLookFreeItem(dwVirtualNumber));
}

PyObject *itemIsChangeLookClearScroll(PyObject *poSelf, PyObject *poArgs)
{
	int iItemVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemVID))
		return Py_BadArgument();

	CItemManager::Instance().SelectItemData(iItemVID);
	CItemData *pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("b", CPythonItem::Instance().IsChangeLookClearScrollItem(pItemData->GetIndex()));
}
#endif

PyObject * itemGetVnum(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("i", pItemData->GetIndex());
}

PyObject * itemGetItemVnum(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bWindowType;
	if (!PyTuple_GetByte(poArgs, 0, &bWindowType))
		return Py_BuildValue("i", 0);

	WORD wSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wSlotIndex))
		return Py_BuildValue("i", 0);

	switch (bWindowType)
	{
		case 1: case 2: case 3: case 4: case 5: case 6: case 7:
		case 8: case 9: case 10: case 11: case 12: case 13: case 14:
		case 15: case 16: case 17: case 21: case 28:
		{
			const TItemData * pItem = CPythonPlayer::Instance().GetItemData(TItemPos(bWindowType, wSlotIndex));
			if (!pItem)
				return Py_BuildException("Cell(%u, %u) item is null", bWindowType, wSlotIndex);
			return Py_BuildValue("i", pItem->vnum);
		}
		case 23: // SAFEBOX
		{
			TItemData * pItem = nullptr;
			if (!CPythonSafeBox::Instance().GetItemDataPtr(wSlotIndex, &pItem) || !pItem)
				return Py_BuildException("Cell(%u, %u) item is null", bWindowType, wSlotIndex);
			return Py_BuildValue("i", pItem->vnum);
		}
		case 24: // MALL
		{
			TItemData * pItem = nullptr;
			if (!CPythonSafeBox::Instance().GetMallItemDataPtr(wSlotIndex, &pItem) || !pItem)
				return Py_BuildException("Cell(%u, %u) item is null", bWindowType, wSlotIndex);
			return Py_BuildValue("i", pItem->vnum);
		}
		case 25: // EXCHANGE SELF
		{
			return Py_BuildValue("i", CPythonExchange::Instance().GetItemVnumFromSelf((BYTE)wSlotIndex));
		}
		case 26: // EXCHANGE TARGET
		{
			return Py_BuildValue("i", CPythonExchange::Instance().GetItemVnumFromTarget((BYTE)wSlotIndex));
		}
		default:
			return Py_BuildValue("i", 0);
	}
}

PyObject * itemGetSocketCount(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pItemData->GetSocketCount());
}

PyObject * itemGetTypeByVnum(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildValue("i", 42);

	CItemData * pItemData = nullptr;
	if (!CItemManager::Instance().GetItemDataPointer(dwVnum, &pItemData) || !pItemData)
		return Py_BuildValue("i", 42);

	return Py_BuildValue("i", pItemData->GetType());
}

PyObject * itemGetUnlimitedSealDate(PyObject * poSelf, PyObject * poArgs)
{
	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

	return Py_BuildValue("i", 0);
}

#ifdef ENABLE_REMOTE_CALL
PyObject * itemIsRemoteCallItem(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BadArgument();

	if (dwVnum >= 72368 && dwVnum <= 72371)
		return Py_BuildValue("b", true);

	return Py_BuildValue("b", false);
}
#endif

PyObject * itemIsAutoPotion(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildValue("b", false);

	if ((dwVnum >= 72723 && dwVnum <= 72726) ||
		(dwVnum >= 76005 && dwVnum <= 76006) ||
		dwVnum == 79012 || dwVnum == 76023 ||
		(dwVnum >= 39037 && dwVnum <= 39039) ||
		(dwVnum >= 72727 && dwVnum <= 72730) ||
		(dwVnum >= 75988 && dwVnum <= 75989) ||
		dwVnum == 79013 || dwVnum == 76024 ||
		(dwVnum >= 39040 && dwVnum <= 39042))
	{
		return Py_BuildValue("b", true);
	}

	return Py_BuildValue("b", false);
}

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
PyObject * itemIsAcceScroll(PyObject * poSelf, PyObject * poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BadArgument();

	if (iVnum <= 0)
		return Py_BuildValue("i", 0);

	if (!CItemManager::Instance().SelectItemData(iVnum))
		return Py_BuildException("Can't find select item data");

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (pItemData->GetType() != CItemData::ITEM_TYPE_USE)
		return Py_BuildValue("i", 0);

	if (pItemData->GetSubType() != CItemData::USE_SPECIAL)
		return Py_BuildValue("i", 0);

	DWORD dwIndex = pItemData->GetIndex();
	if (dwIndex == 90000 || dwIndex == 39046)
		return Py_BuildValue("i", 1);

	return Py_BuildValue("i", 0);
}
#endif

PyObject * itemIsSealScroll(PyObject * poSelf, PyObject * poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BadArgument();

	if (iVnum <= 0)
		return Py_BuildValue("i", 0);

	if (!CItemManager::Instance().SelectItemData(iVnum))
		return Py_BuildException("Can't find select item data");

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	if (pItemData->GetType() != CItemData::ITEM_TYPE_USE)
		return Py_BuildValue("i", 0);

	if (pItemData->GetSubType() != CItemData::USE_SPECIAL)
		return Py_BuildValue("i", 0);

	DWORD dwIndex = pItemData->GetIndex();
	if (dwIndex == 50262 || dwIndex == 50263 || dwIndex == 50264 || dwIndex == 72344 || dwIndex == 72343)
		return Py_BuildValue("i", 1);

	return Py_BuildValue("i", 0);
}

#ifdef ENABLE_UN_SEAL_SCROLL_PLUS
PyObject * itemIsUnSealScrollPlus(PyObject * poSelf, PyObject * poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BadArgument();

	if (iVnum <= 0)
		return Py_BuildValue("b", false);

	if (!CItemManager::Instance().SelectItemData(iVnum))
		return Py_BuildException("Can't find select item data");

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("b", pItemData->GetIndex() == 50262 || pItemData->GetIndex() == 72344);
}
#endif

PyObject * itemIsSetClearScroll(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BadArgument();

	if (dwVnum == 0)
		return Py_BuildValue("b", false);

	CItemData * pItemData = nullptr;
	if (!CItemManager::Instance().GetItemDataPointer(dwVnum, &pItemData) || !pItemData)
		return Py_BuildException("Can't find select item data");

	return Py_BuildValue("b", pItemData->GetIndex() == 39115);
}

PyObject * itemIsDragonSoulRelevantItem(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BadArgument();

	if (dwVnum == 0)
		return Py_BuildValue("b", false);

	CItemData * pItemData = nullptr;
	if (!CItemManager::Instance().GetItemDataPointer(dwVnum, &pItemData) || !pItemData)
		return Py_BuildValue("b", false);

	BYTE byType = pItemData->GetType();
	if (byType == CItemData::ITEM_TYPE_USE)
	{
		BYTE bySubType = pItemData->GetSubType();
		if (bySubType == CItemData::USE_SPECIAL)
		{
			DWORD dwIndex = pItemData->GetIndex();
			if (dwIndex == 50262 || dwIndex == 50263 || dwIndex == 50264)
				return Py_BuildValue("b", true);
		}
		else if (bySubType == CItemData::USE_TIME_CHARGE_PER || bySubType == CItemData::USE_TIME_CHARGE_FIX || bySubType == 33)
		{
			return Py_BuildValue("b", true);
		}
	}
	else if (byType == CItemData::ITEM_TYPE_EXTRACT)
	{
		BYTE bySubType = pItemData->GetSubType();
		if (bySubType == 0 || bySubType == 1)
			return Py_BuildValue("b", true);
	}

	return Py_BuildValue("b", false);
}

PyObject * itemIsItemUsedForDragonSoul(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BadArgument();

	BYTE bWindowType;
	if (!PyTuple_GetByte(poArgs, 1, &bWindowType))
		return Py_BadArgument();

	WORD wSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &wSlotIndex))
		return Py_BadArgument();

	if (dwVnum == 0)
		return Py_BuildValue("b", false);

	CItemData * pItemData = nullptr;
	if (!CItemManager::Instance().GetItemDataPointer(dwVnum, &pItemData) || !pItemData)
		return Py_BuildValue("b", false);

	const TItemData * pTargetItem = CPythonPlayer::Instance().GetItemData(TItemPos(bWindowType, wSlotIndex));
	if (!pTargetItem)
		return Py_BuildValue("b", false);

	CItemData * pTargetItemData = nullptr;
	if (!CItemManager::Instance().GetItemDataPointer(pTargetItem->vnum, &pTargetItemData) || !pTargetItemData)
		return Py_BuildValue("b", false);

	if (pTargetItemData->GetType() != CItemData::ITEM_TYPE_DS)
		return Py_BuildValue("b", false);

	BYTE byType = pItemData->GetType();
	if (byType == CItemData::ITEM_TYPE_USE)
	{
		BYTE bySubType = pItemData->GetSubType();
		if (bySubType == CItemData::USE_SPECIAL)
		{
			DWORD dwIndex = pItemData->GetIndex();
			if (dwIndex == 50262 || dwIndex == 50263 || dwIndex == 50264)
				return Py_BuildValue("b", true);
		}
		else if (bySubType == CItemData::USE_TIME_CHARGE_PER || bySubType == CItemData::USE_TIME_CHARGE_FIX || bySubType == 33)
		{
			return Py_BuildValue("b", true);
		}
	}
	else if (byType == CItemData::ITEM_TYPE_EXTRACT)
	{
		BYTE bySubType = pItemData->GetSubType();
		if (bySubType == 0 || bySubType == 1)
			return Py_BuildValue("b", true);
	}

	return Py_BuildValue("b", false);
}

PyObject * itemSetPreName(PyObject * poSelf, PyObject * poArgs)
{
	int iVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iVnum))
		return Py_BadArgument();

	char * szPreName;
	if (!PyTuple_GetString(poArgs, 1, &szPreName))
		return Py_BadArgument();

	return Py_BuildNone();
}

PyObject * itemGetDragonSoulItemGradeIndex(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildValue("i", 0);

	if (dwVnum == 0)
		return Py_BuildValue("i", 6);

	return Py_BuildValue("i", (dwVnum / 1000) % 10);
}

PyObject * itemGetTimePrintOnToolTip(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildNone();

	if (dwVnum == 0)
		return Py_BuildValue("i", 0);

	CItemData * pItemData = nullptr;
	if (!CItemManager::Instance().GetItemDataPointer(dwVnum, &pItemData) || !pItemData)
		return Py_BuildNone();

	int timeValues[6] = {0};
	for (int i = 0; i < CItemData::ITEM_LIMIT_MAX_NUM; ++i)
	{
		CItemData::TItemLimit limit;
		if (pItemData->GetLimit(i, &limit))
		{
			if (limit.bType == CItemData::LIMIT_REAL_TIME)
			{
				timeValues[0] = limit.lValue > 0 ? (int)(timeValues[0] + limit.lValue) : (int)(timeValues[0] + 604800);
			}
			else if (limit.bType == CItemData::LIMIT_TIMER_BASED_ON_WEAR)
			{
				timeValues[0] = limit.lValue > 0 ? limit.lValue : 36000;
			}
		}
	}

	return Py_BuildValue("(iiiiii)", timeValues[0], timeValues[1], timeValues[2], timeValues[3], timeValues[4], timeValues[5]);
}

PyObject * itemGetItemNameBySetValue(PyObject * poSelf, PyObject * poArgs)
{
	int iSetValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iSetValue))
		return Py_BadArgument();

	CItemData * pItemData = CItemManager::Instance().GetSelectedItemDataPointer();
	if (!pItemData)
		return Py_BuildException("no selected item data");

#ifdef ENABLE_SET_ITEM
	const char* szItemName = CPythonPlayer::Instance().GetItemNameBySetValue(iSetValue, pItemData->GetName());
	return Py_BuildValue("s", szItemName);
#else
	return Py_BuildValue("s", pItemData->GetName());
#endif
}

PyObject * itemCheckAffect(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwType;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwType))
		return Py_BadArgument();

	WORD wApplyOn;
	if (!PyTuple_GetInteger(poArgs, 1, &wApplyOn))
		return Py_BadArgument();

	return Py_BuildValue("b", false);
}

PyObject * itemGetAffectDuration(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BadArgument();

	return Py_BuildValue("i", 0);
}

#ifdef ENABLE_ATTENDANCE_EVENT
PyObject * itemGetAttendanceRewardList(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * poList = PyList_New(0);
	return poList;
}
#endif

PyObject * itemGetPointApply(PyObject * poSelf, PyObject * poArgs)
{
	WORD wApplyType;
	if (!PyTuple_GetInteger(poArgs, 0, &wApplyType))
		return Py_BadArgument();

	return Py_BuildValue("i", (int)wApplyType);
}

PyObject * itemHasAffectShouldBeRemovedBeforeEnterMistsIsland(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("b", false);
}

PyObject * itemGetItemListContainsSubstringInName(PyObject * poSelf, PyObject * poArgs)
{
	char * szFilter;
	if (!PyTuple_GetString(poArgs, 0, &szFilter))
		return Py_BadArgument();

	BYTE bType = 0;
	PyTuple_GetByte(poArgs, 1, &bType);
	BYTE bSubType = 0;
	PyTuple_GetByte(poArgs, 2, &bSubType);

	PyObject * poList = PyList_New(0);
	return poList;
}

PyObject * itemGetCommonRewardData(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * poList = PyList_New(0);
	return poList;
}

void initItem()
{
	static PyMethodDef s_methods[] =
	{
		{ "SetUseSoundFileName",			itemSetUseSoundFileName,				METH_VARARGS },
		{ "SetDropSoundFileName",			itemSetDropSoundFileName,				METH_VARARGS },
		{ "SelectItem",						itemSelectItem,							METH_VARARGS },

		{ "GetItemName",					itemGetItemName,						METH_VARARGS },
		{ "GetItemDescription",				itemGetItemDescription,					METH_VARARGS },
		{ "GetItemSummary",					itemGetItemSummary,						METH_VARARGS },
		{ "GetIconImage",					itemGetIconImage,						METH_VARARGS },
		{ "GetIconImageFileName",			itemGetIconImageFileName,				METH_VARARGS },
		{ "GetItemSize",					itemGetItemSize,						METH_VARARGS },
		{ "GetItemType",					itemGetItemType,						METH_VARARGS },
		{ "GetItemSubType",					itemGetItemSubType,						METH_VARARGS },
		{ "GetItemMaskType",				itemGetItemMaskType,					METH_VARARGS },
		{ "GetItemMaskSubType",				itemGetItemMaskSubType,					METH_VARARGS },
		{ "GetAttr67MaterialVnum",			itemGetAttr67MaterialVnum,				METH_VARARGS },
		{ "GetRefineLevel",					itemGetRefineLevel,						METH_VARARGS },
		{ "IsCostumeMountItem",				itemIsCostumeMountItem,					METH_VARARGS },
#ifdef ENABLE_WHITE_DRAGON
		{ "IsWhiteDragonBlessItem",			itemIsWhiteDragonBlessItem,				METH_VARARGS },
#endif
		{ "IsIronDragonBlessItem",			itemIsIronDragonBlessItem,				METH_VARARGS },
		{ "GetDSBasicApplyCount",			itemGetDSBasicApplyCount,				METH_VARARGS },
		{ "GetDSBasicApplyValue",			itemGetDSBasicApplyValue,				METH_VARARGS },
		{ "GetDSAdditionalApplyValue",		itemGetDSAdditionalApplyValue,			METH_VARARGS },
		{ "GetDSSetWeight",					itemGetDSSetWeight,						METH_VARARGS },
		{ "GetIBuyItemPrice",				itemGetIBuyItemPrice,					METH_VARARGS },
		{ "GetISellItemPrice",				itemGetISellItemPrice,					METH_VARARGS },
		{ "IsAntiFlag",						itemIsAntiFlag,							METH_VARARGS },
		{ "IsFlag",							itemIsFlag,								METH_VARARGS },
		{ "IsWearableFlag",					itemIsWearableFlag,						METH_VARARGS },
		{ "Is1GoldItem",					itemIs1GoldItem,						METH_VARARGS },
		{ "GetLimit",						itemGetLimit,							METH_VARARGS },
		{ "GetLimitType",					itemGetLimitType,						METH_VARARGS },
		{ "GetLimitValue",					itemGetLimitValue,						METH_VARARGS },
		{ "GetAffect",						itemGetAffect,							METH_VARARGS },
		{ "GetValue",						itemGetValue,							METH_VARARGS },
		{ "GetSocket",						itemGetSocket,							METH_VARARGS },
		{ "GetIconInstance",				itemGetIconInstance,					METH_VARARGS },
		{ "GetUseType",						itemGetUseType,							METH_VARARGS },
		{ "DeleteIconInstance",				itemDeleteIconInstance,					METH_VARARGS },
		{ "IsEquipmentVID",					itemIsEquipmentVID,						METH_VARARGS },
		{ "IsRefineScroll",					itemIsRefineScroll,						METH_VARARGS },
		{ "IsDetachScroll",					itemIsDetachScroll,						METH_VARARGS },
		{ "IsKey",							itemIsKey,								METH_VARARGS },
		{ "IsMetin",						itemIsMetin,							METH_VARARGS },
		{ "CanAddToQuickSlotItem",			itemCanAddToQuickSlotItem,				METH_VARARGS },

		{ "Update",							itemUpdate,								METH_VARARGS },
		{ "Render",							itemRender,								METH_VARARGS },
		{ "CreateItem",						itemCreateItem,							METH_VARARGS },
		{ "DeleteItem",						itemDeleteItem,							METH_VARARGS },
		{ "Pick",							itemPick,								METH_VARARGS },

		{ "LoadItemTable",					itemLoadItemTable,						METH_VARARGS },

		{ "IsWeddingItem",					itemIsWeddingItem,						METH_VARARGS },
		{ "GetItemNameByVnum",				itemGetItemNameByVnum,					METH_VARARGS },

#ifdef ENABLE_WIKI_SYSTEM
		{ "SelectItemWiki",					itemSelectItemWiki,						METH_VARARGS },
		{ "GetRefineSet",					itemGetRefineSet,						METH_VARARGS },
		{ "GetRefinedVnum",					itemGetRefinedVnum,						METH_VARARGS },
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
		{ "GetItemsByName",					itemGetItemsByName,						METH_VARARGS },
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		{ "IsPossibleChangeLookLeft",		itemIsPossibleChangeLookLeft,			METH_VARARGS },
		{ "IsPossibleChangeLookRight",		itemIsPossibleChangeLookRight,			METH_VARARGS },
		{ "IsChangeLookFreePassYangItem",	itemIsChangeLookFreePassYangItem,		METH_VARARGS },
		{ "IsChangeLookClearScroll",		itemIsChangeLookClearScroll,			METH_VARARGS },
#endif
#if defined(__BL__DETAILS_UI__)
		{ "GetApplyPoint",					itemGetApplyPoint,						METH_VARARGS },
#endif
		{ "GetVnum",						itemGetVnum,							METH_VARARGS },
		{ "GetItemVnum",					itemGetItemVnum,						METH_VARARGS },
		{ "GetSocketCount",					itemGetSocketCount,						METH_VARARGS },
		{ "GetTypeByVnum",					itemGetTypeByVnum,						METH_VARARGS },
		{ "GetUnlimitedSealDate",			itemGetUnlimitedSealDate,				METH_VARARGS },
#ifdef ENABLE_REMOTE_CALL
		{ "IsRemoteCallItem",				itemIsRemoteCallItem,					METH_VARARGS },
#endif
		{ "IsAutoPotion",					itemIsAutoPotion,						METH_VARARGS },
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		{ "IsAcceScroll",					itemIsAcceScroll,						METH_VARARGS },
#endif
		{ "IsSealScroll",					itemIsSealScroll,						METH_VARARGS },
#ifdef ENABLE_UN_SEAL_SCROLL_PLUS
		{ "IsUnSealScrollPlus",				itemIsUnSealScrollPlus,					METH_VARARGS },
#endif
		{ "IsSetClearScroll",				itemIsSetClearScroll,					METH_VARARGS },
		{ "IsDragonSoulRelevantItem",		itemIsDragonSoulRelevantItem,			METH_VARARGS },
		{ "IsItemUsedForDragonSoul",		itemIsItemUsedForDragonSoul,			METH_VARARGS },
		{ "SetPreName",						itemSetPreName,							METH_VARARGS },
		{ "GetDragonSoulItemGradeIndex",	itemGetDragonSoulItemGradeIndex,		METH_VARARGS },
		{ "GetTimePrintOnToolTip",			itemGetTimePrintOnToolTip,				METH_VARARGS },
		{ "GetItemNameBySetValue",			itemGetItemNameBySetValue,				METH_VARARGS },
		{ "CheckAffect",					itemCheckAffect,						METH_VARARGS },
		{ "GetAffectDuration",				itemGetAffectDuration,					METH_VARARGS },
#ifdef ENABLE_ATTENDANCE_EVENT
		{ "GetAttendanceRewardList",		itemGetAttendanceRewardList,			METH_VARARGS },
#endif
		{ "GetPointApply",					itemGetPointApply,						METH_VARARGS },
		{ "HasAffectShouldBeRemovedBeforeEnterMistsIsland", itemHasAffectShouldBeRemovedBeforeEnterMistsIsland, METH_VARARGS },
		{ "GetItemListContainsSubstringInName", itemGetItemListContainsSubstringInName, METH_VARARGS },
		{ "GetCommonRewardData",			itemGetCommonRewardData,				METH_VARARGS },
		{ "IsWedddingItem",					itemIsWeddingItem,						METH_VARARGS },

		{ nullptr, nullptr },
	};

	PyObject *poModule = Py_InitModule("item", s_methods);

	PyModule_AddIntConstant(poModule, "USESOUND_ACCESSORY", CPythonItem::USESOUND_ACCESSORY);
	PyModule_AddIntConstant(poModule, "USESOUND_ARMOR", CPythonItem::USESOUND_ARMOR);
	PyModule_AddIntConstant(poModule, "USESOUND_BOW", CPythonItem::USESOUND_BOW);
	PyModule_AddIntConstant(poModule, "USESOUND_DEFAULT", CPythonItem::USESOUND_DEFAULT);
	PyModule_AddIntConstant(poModule, "USESOUND_WEAPON", CPythonItem::USESOUND_WEAPON);
	PyModule_AddIntConstant(poModule, "USESOUND_POTION", CPythonItem::USESOUND_POTION);
	PyModule_AddIntConstant(poModule, "USESOUND_PORTAL", CPythonItem::USESOUND_PORTAL);

	PyModule_AddIntConstant(poModule, "DROPSOUND_ACCESSORY", CPythonItem::DROPSOUND_ACCESSORY);
	PyModule_AddIntConstant(poModule, "DROPSOUND_ARMOR", CPythonItem::DROPSOUND_ARMOR);
	PyModule_AddIntConstant(poModule, "DROPSOUND_BOW", CPythonItem::DROPSOUND_BOW);
	PyModule_AddIntConstant(poModule, "DROPSOUND_DEFAULT", CPythonItem::DROPSOUND_DEFAULT);
	PyModule_AddIntConstant(poModule, "DROPSOUND_WEAPON", CPythonItem::DROPSOUND_WEAPON);

	PyModule_AddIntConstant(poModule, "EQUIPMENT_COUNT", c_Equipment_Count);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_HEAD", c_Equipment_Head);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_BODY", c_Equipment_Body);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_WEAPON", c_Equipment_Weapon);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_WRIST", c_Equipment_Wrist);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_SHOES", c_Equipment_Shoes);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_NECK", c_Equipment_Neck);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_EAR", c_Equipment_Ear);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_UNIQUE1", c_Equipment_Unique1);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_UNIQUE2", c_Equipment_Unique2);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_ARROW", c_Equipment_Arrow);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_SHIELD", c_Equipment_Shield);

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	PyModule_AddIntConstant(poModule, "EQUIPMENT_RING1", c_Equipment_Ring1);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_RING2", c_Equipment_Ring2);
	PyModule_AddIntConstant(poModule, "EQUIPMENT_BELT", c_Equipment_Belt);
#ifdef ENABLE_MOUNT_SYSTEM
	PyModule_AddIntConstant(poModule, "EQUIPMENT_MOUNT", c_Equipment_Mount);
#endif
#ifdef ENABLE_PET_SYSTEM
	PyModule_AddIntConstant(poModule, "EQUIPMENT_PET", c_Equipment_Pet);
#endif
#ifdef ENABLE_PENDANT_SYSTEM
	PyModule_AddIntConstant(poModule, "EQUIPMENT_PENDANT", c_Equipment_Pendant);
#endif
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
	PyModule_AddIntConstant(poModule, "EQUIPMENT_PASSIVE",		c_Equipment_Passive);
#endif
	// MASK_ITEM_TYPE
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_NONE", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_MOUNT_PET", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_EQUIPMENT_WEAPON", 2);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_EQUIPMENT_ARMOR", 3);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_EQUIPMENT_JEWELRY", 4);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_TUNING", 5);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_POTION", 6);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_FISHING_PICK", 7);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_DRAGON_STONE", 8);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_COSTUMES", 9);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_SKILL", 10);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_UNIQUE", 11);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_ETC", 12);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_TYPE_MAX", 13);

	// MASK_ITEM_SUBTYPE
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_MOUNT_PET_MOUNT", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_MOUNT_PET_CHARGED_PET", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_MOUNT_PET_FREE_PET", 2);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_MOUNT_PET_EGG", 3);

	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_WEAPON_WEAPON_SWORD", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_WEAPON_WEAPON_DAGGER", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_WEAPON_WEAPON_BOW", 2);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_WEAPON_WEAPON_TWO_HANDED", 3);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_WEAPON_WEAPON_BELL", 4);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_WEAPON_WEAPON_CLAW", 5);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_WEAPON_WEAPON_FAN", 6);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_WEAPON_WEAPON_MOUNT_SPEAR", 7);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_WEAPON_WEAPON_ARROW", 8);

	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ARMOR_ARMOR_BODY", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ARMOR_ARMOR_HEAD", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ARMOR_ARMOR_SHIELD", 2);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ARMOR_ARMOR_GLOVE", 3);

	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_JEWELRY_ARMOR_WRIST", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_JEWELRY_ARMOR_FOOTS", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_JEWELRY_ARMOR_NECK", 2);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_JEWELRY_ARMOR_EAR", 3);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_JEWELRY_ITEM_BELT", 4);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_JEWELRY_ARMOR_PENDANT", 5);

	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_TUNING_RESOURCE", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_TUNING_STONE", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_TUNING_ETC", 2);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_TUNING_ALCHEMY", 3);

	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_POTION_ABILITY", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_POTION_HAIRDYE", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_POTION_ETC", 2);

	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_FISHING_PICK_FISHING_POLE", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_FISHING_PICK_EQUIPMENT_PICK", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_FISHING_PICK_FOOD", 2);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_FISHING_PICK_STONE", 3);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_FISHING_PICK_ETC", 4);

	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_DRAGON_STONE_DRAGON_DIAMOND", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_DRAGON_STONE_DRAGON_RUBY", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_DRAGON_STONE_DRAGON_JADE", 2);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_DRAGON_STONE_DRAGON_SAPPHIRE", 3);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_DRAGON_STONE_DRAGON_GARNET", 4);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_DRAGON_STONE_DRAGON_ONYX", 5);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_DRAGON_STONE_DRAGON_DEMON", 6);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_DRAGON_STONE_ETC", 7);

	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_COSTUMES_COSTUME_WEAPON", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_COSTUMES_COSTUME_BODY", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_COSTUMES_COSTUME_HAIR", 2);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_COSTUMES_SASH", 3);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_COSTUMES_ETC", 4);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_COSTUMES_AURA", 5);

	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_SKILL_PAHAE", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_SKILL_SKILL_BOOK", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_SKILL_BOOK_OF_OBLIVION", 2);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_SKILL_ETC", 3);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_SKILL_SKILL_BOOK_HORSE", 4);

	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_UNIQUE_ABILITY", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_UNIQUE_ETC", 1);

	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ETC_GIFTBOX", 0);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ETC_MATRIMORY", 1);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ETC_EVENT", 2);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ETC_SEAL", 3);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ETC_PARTI", 4);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ETC_POLYMORPH", 5);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ETC_RECIPE", 6);
	PyModule_AddIntConstant(poModule, "MASK_ITEM_SUBTYPE_ETC_ETC", 7);

	PyModule_AddIntConstant(poModule, "ITEM_TYPE_NONE", CItemData::ITEM_TYPE_NONE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_WEAPON", CItemData::ITEM_TYPE_WEAPON);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ARMOR", CItemData::ITEM_TYPE_ARMOR);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_USE", CItemData::ITEM_TYPE_USE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_AUTOUSE", CItemData::ITEM_TYPE_AUTOUSE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_MATERIAL", CItemData::ITEM_TYPE_MATERIAL);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SPECIAL", CItemData::ITEM_TYPE_SPECIAL);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TOOL", CItemData::ITEM_TYPE_TOOL);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_LOTTERY", CItemData::ITEM_TYPE_LOTTERY);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ELK", CItemData::ITEM_TYPE_ELK);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_METIN", CItemData::ITEM_TYPE_METIN);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_CONTAINER", CItemData::ITEM_TYPE_CONTAINER);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_FISH", CItemData::ITEM_TYPE_FISH);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ROD", CItemData::ITEM_TYPE_ROD);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RESOURCE", CItemData::ITEM_TYPE_RESOURCE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_CAMPFIRE", CItemData::ITEM_TYPE_CAMPFIRE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_UNIQUE", CItemData::ITEM_TYPE_UNIQUE);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SKILLBOOK", CItemData::ITEM_TYPE_SKILLBOOK);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_QUEST", CItemData::ITEM_TYPE_QUEST);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_POLYMORPH", CItemData::ITEM_TYPE_POLYMORPH);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TREASURE_BOX", CItemData::ITEM_TYPE_TREASURE_BOX);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_TREASURE_KEY", CItemData::ITEM_TYPE_TREASURE_KEY);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_PICK", CItemData::ITEM_TYPE_PICK);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_BLEND", CItemData::ITEM_TYPE_BLEND);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_DS", CItemData::ITEM_TYPE_DS);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_SPECIAL_DS", CItemData::ITEM_TYPE_SPECIAL_DS);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_GIFTBOX", CItemData::ITEM_TYPE_GIFTBOX);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_RING", CItemData::ITEM_TYPE_RING);
#ifdef ENABLE_MOUNT_SYSTEM
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_MOUNT", CItemData::ITEM_TYPE_MOUNT);
#endif
#ifdef ENABLE_PET_SYSTEM
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_PET", CItemData::ITEM_TYPE_PET);
#endif
#ifdef ENABLE_NEW_ITEM_TYPE_GACHA
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_GACHA", CItemData::ITEM_TYPE_GACHA);
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_PASSIVE",		CItemData::ITEM_TYPE_PASSIVE);
#endif
#ifdef ENABLE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_COSTUME", CItemData::ITEM_TYPE_COSTUME);
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_BODY", CItemData::COSTUME_BODY);
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_HAIR", CItemData::COSTUME_HAIR);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_ACCE", CItemData::COSTUME_ACCE);
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_WEAPON", CItemData::COSTUME_WEAPON);
#endif
#ifdef ENABLE_MOUNT_PET_SKIN
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_MOUNT", CItemData::COSTUME_MOUNT);
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_PET", CItemData::COSTUME_PET);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_TYPE_AURA", CItemData::COSTUME_AURA);
#endif

	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_START", c_Costume_Slot_Start);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_COUNT", c_Costume_Slot_Count);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_BODY", c_Costume_Slot_Body);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_HAIR", c_Costume_Slot_Hair);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_ACCE", c_Costume_Slot_Acce);
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_WEAPON", c_Costume_Slot_Weapon);
#endif
#ifdef ENABLE_MOUNT_PET_SKIN
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_MOUNT", c_Costume_Slot_Mount);
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_PET", c_Costume_Slot_Pet);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_AURA", c_Costume_Slot_Aura);
#endif
	PyModule_AddIntConstant(poModule, "COSTUME_SLOT_END", c_Costume_Slot_End);
#endif

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	PyModule_AddIntConstant(poModule, "BELT_INVENTORY_SLOT_START", c_Belt_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "BELT_INVENTORY_SLOT_COUNT", c_Belt_Inventory_Slot_Count);
	PyModule_AddIntConstant(poModule, "BELT_INVENTORY_SLOT_END", c_Belt_Inventory_Slot_End);
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	PyModule_AddIntConstant(poModule, "SKILL_BOOK_INVENTORY_SLOT_START", c_Skill_Book_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "UPGRADE_ITEMS_INVENTORY_SLOT_START", c_Upgrade_Items_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "STONE_INVENTORY_SLOT_START", c_Stone_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "GIFT_BOX_INVENTORY_SLOT_START", c_GiftBox_Inventory_Slot_Start);
	PyModule_AddIntConstant(poModule, "CHANGERS_INVENTORY_SLOT_START", c_Changers_Inventory_Slot_Start);
#endif

	PyModule_AddIntConstant(poModule, "WEAPON_SWORD", CItemData::WEAPON_SWORD);
	PyModule_AddIntConstant(poModule, "WEAPON_DAGGER", CItemData::WEAPON_DAGGER);
	PyModule_AddIntConstant(poModule, "WEAPON_BOW", CItemData::WEAPON_BOW);
	PyModule_AddIntConstant(poModule, "WEAPON_TWO_HANDED", CItemData::WEAPON_TWO_HANDED);
	PyModule_AddIntConstant(poModule, "WEAPON_BELL", CItemData::WEAPON_BELL);
	PyModule_AddIntConstant(poModule, "WEAPON_FAN", CItemData::WEAPON_FAN);
	PyModule_AddIntConstant(poModule, "WEAPON_ARROW", CItemData::WEAPON_ARROW);
	PyModule_AddIntConstant(poModule, "WEAPON_NUM_TYPES", CItemData::WEAPON_NUM_TYPES);

	PyModule_AddIntConstant(poModule, "QUEST_NONE", CItemData::QUEST_NONE);
	PyModule_AddIntConstant(poModule, "QUEST_PET_PAY", CItemData::QUEST_PET_PAY);
	PyModule_AddIntConstant(poModule, "QUEST_WARP", CItemData::QUEST_WARP);
	PyModule_AddIntConstant(poModule, "QUEST_GEM_BAG", CItemData::QUEST_GEM_BAG);
#ifdef ENABLE_GEM_BAG_ANTI_SELL_TOOLTIP
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_GEM_BAG", 70608);
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_GEM_BAG_REWARD", 70611);
#endif

	PyModule_AddIntConstant(poModule, "USE_POTION", CItemData::USE_POTION);
	PyModule_AddIntConstant(poModule, "USE_TALISMAN", CItemData::USE_TALISMAN);
	PyModule_AddIntConstant(poModule, "USE_TUNING", CItemData::USE_TUNING);
	PyModule_AddIntConstant(poModule, "USE_MOVE", CItemData::USE_MOVE);
	PyModule_AddIntConstant(poModule, "USE_TREASURE_BOX", CItemData::USE_TREASURE_BOX);
	PyModule_AddIntConstant(poModule, "USE_MONEYBAG", CItemData::USE_MONEYBAG);
	PyModule_AddIntConstant(poModule, "USE_BAIT", CItemData::USE_BAIT);
	PyModule_AddIntConstant(poModule, "USE_ABILITY_UP", CItemData::USE_ABILITY_UP);
	PyModule_AddIntConstant(poModule, "USE_AFFECT", CItemData::USE_AFFECT);
	PyModule_AddIntConstant(poModule, "USE_CREATE_STONE", CItemData::USE_CREATE_STONE);
	PyModule_AddIntConstant(poModule, "USE_SPECIAL", CItemData::USE_SPECIAL);
	PyModule_AddIntConstant(poModule, "USE_POTION_NODELAY", CItemData::USE_POTION_NODELAY);
	PyModule_AddIntConstant(poModule, "USE_CLEAR", CItemData::USE_CLEAR);
	PyModule_AddIntConstant(poModule, "USE_INVISIBILITY", CItemData::USE_INVISIBILITY);
	PyModule_AddIntConstant(poModule, "USE_DETACHMENT", CItemData::USE_DETACHMENT);
	PyModule_AddIntConstant(poModule, "USE_TIME_CHARGE_PER", CItemData::USE_TIME_CHARGE_PER);
	PyModule_AddIntConstant(poModule, "USE_TIME_CHARGE_FIX", CItemData::USE_TIME_CHARGE_FIX);
#ifdef ENABLE_TITLE_SYSTEM
	PyModule_AddIntConstant(poModule, "USE_TITLE", CItemData::USE_TITLE);
#endif
#ifdef ENABLE_RENEWAL_AFFECT
	PyModule_AddIntConstant(poModule, "USE_AFFECT_PLUS", CItemData::USE_AFFECT_PLUS);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "USE_PUT_INTO_AURA_SOCKET", CItemData::USE_PUT_INTO_AURA_SOCKET);
#endif

#ifdef ENABLE_PASSIVE_SYSTEM
	PyModule_AddIntConstant(poModule, "PASSIVE_JOB", CItemData::PASSIVE_JOB);
#endif

	PyModule_AddIntConstant(poModule, "MATERIAL_DS_REFINE_NORMAL", CItemData::MATERIAL_DS_REFINE_NORMAL);
	PyModule_AddIntConstant(poModule, "MATERIAL_DS_REFINE_BLESSED", CItemData::MATERIAL_DS_REFINE_BLESSED);
	PyModule_AddIntConstant(poModule, "MATERIAL_DS_REFINE_HOLLY", CItemData::MATERIAL_DS_REFINE_HOLLY);
#ifdef ENABLE_PASSIVE_SYSTEM
	PyModule_AddIntConstant(poModule, "MATERIAL_PASSIVE_WEAPON", CItemData::MATERIAL_PASSIVE_WEAPON);
	PyModule_AddIntConstant(poModule, "MATERIAL_PASSIVE_ARMOR", CItemData::MATERIAL_PASSIVE_ARMOR);
	PyModule_AddIntConstant(poModule, "MATERIAL_PASSIVE_ACCE", CItemData::MATERIAL_PASSIVE_ACCE);
	PyModule_AddIntConstant(poModule, "MATERIAL_PASSIVE_ELEMENT", CItemData::MATERIAL_PASSIVE_ELEMENT);
#endif

	PyModule_AddIntConstant(poModule, "METIN_NORMAL", CItemData::METIN_NORMAL);
	PyModule_AddIntConstant(poModule, "METIN_GOLD", CItemData::METIN_GOLD);

	PyModule_AddIntConstant(poModule, "LIMIT_NONE", CItemData::LIMIT_NONE);
	PyModule_AddIntConstant(poModule, "LIMIT_LEVEL", CItemData::LIMIT_LEVEL);
	PyModule_AddIntConstant(poModule, "LIMIT_STR", CItemData::LIMIT_STR);
	PyModule_AddIntConstant(poModule, "LIMIT_DEX", CItemData::LIMIT_DEX);
	PyModule_AddIntConstant(poModule, "LIMIT_INT", CItemData::LIMIT_INT);
	PyModule_AddIntConstant(poModule, "LIMIT_CON", CItemData::LIMIT_CON);
	PyModule_AddIntConstant(poModule, "LIMIT_REAL_TIME", CItemData::LIMIT_REAL_TIME);
	PyModule_AddIntConstant(poModule, "LIMIT_REAL_TIME_START_FIRST_USE", CItemData::LIMIT_REAL_TIME_START_FIRST_USE);
	PyModule_AddIntConstant(poModule, "LIMIT_TIMER_BASED_ON_WEAR", CItemData::LIMIT_TIMER_BASED_ON_WEAR);
	PyModule_AddIntConstant(poModule, "LIMIT_TYPE_MAX_NUM", CItemData::LIMIT_MAX_NUM);
	PyModule_AddIntConstant(poModule, "LIMIT_MAX_NUM", CItemData::ITEM_LIMIT_MAX_NUM);

	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_FEMALE", CItemData::ITEM_ANTIFLAG_FEMALE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_MALE", CItemData::ITEM_ANTIFLAG_MALE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_WARRIOR", CItemData::ITEM_ANTIFLAG_WARRIOR);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_ASSASSIN", CItemData::ITEM_ANTIFLAG_ASSASSIN);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SURA", CItemData::ITEM_ANTIFLAG_SURA);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SHAMAN", CItemData::ITEM_ANTIFLAG_SHAMAN);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_GET", CItemData::ITEM_ANTIFLAG_GET);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_DROP", CItemData::ITEM_ANTIFLAG_DROP);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SELL", CItemData::ITEM_ANTIFLAG_SELL);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_EMPIRE_A", CItemData::ITEM_ANTIFLAG_EMPIRE_A);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_EMPIRE_B", CItemData::ITEM_ANTIFLAG_EMPIRE_B);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_EMPIRE_R", CItemData::ITEM_ANTIFLAG_EMPIRE_R);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SAVE", CItemData::ITEM_ANTIFLAG_SAVE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_GIVE", CItemData::ITEM_ANTIFLAG_GIVE);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_PKDROP", CItemData::ITEM_ANTIFLAG_PKDROP);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_STACK", CItemData::ITEM_ANTIFLAG_STACK);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_MYSHOP", CItemData::ITEM_ANTIFLAG_MYSHOP);
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_SAFEBOX", CItemData::ITEM_ANTIFLAG_SAFEBOX);
#ifdef ENABLE_DESTROY_DIALOG
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_DESTROY", CItemData::ITEM_ANTIFLAG_DESTROY);
#endif
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_RT_REMOVE", CItemData::ITEM_ANTIFLAG_RT_REMOVE);
#ifdef ENABLE_ITEMSHOP
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_STACKABLE", CItemData::ITEM_FLAG_STACKABLE);
#endif

	PyModule_AddIntConstant(poModule, "ITEM_FLAG_RARE", CItemData::ITEM_FLAG_RARE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_UNIQUE", CItemData::ITEM_FLAG_UNIQUE);
	PyModule_AddIntConstant(poModule, "ITEM_FLAG_CONFIRM_WHEN_USE", CItemData::ITEM_FLAG_CONFIRM_WHEN_USE);

	PyModule_AddIntConstant(poModule, "ANTIFLAG_FEMALE", CItemData::ITEM_ANTIFLAG_FEMALE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_MALE", CItemData::ITEM_ANTIFLAG_MALE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_WARRIOR", CItemData::ITEM_ANTIFLAG_WARRIOR);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_ASSASSIN", CItemData::ITEM_ANTIFLAG_ASSASSIN);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SURA", CItemData::ITEM_ANTIFLAG_SURA);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SHAMAN", CItemData::ITEM_ANTIFLAG_SHAMAN);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_GET", CItemData::ITEM_ANTIFLAG_GET);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_DROP", CItemData::ITEM_ANTIFLAG_DROP);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SELL", CItemData::ITEM_ANTIFLAG_SELL);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_EMPIRE_A", CItemData::ITEM_ANTIFLAG_EMPIRE_A);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_EMPIRE_B", CItemData::ITEM_ANTIFLAG_EMPIRE_B);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_EMPIRE_R", CItemData::ITEM_ANTIFLAG_EMPIRE_R);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SAVE", CItemData::ITEM_ANTIFLAG_SAVE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_GIVE", CItemData::ITEM_ANTIFLAG_GIVE);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_PKDROP", CItemData::ITEM_ANTIFLAG_PKDROP);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_STACK", CItemData::ITEM_ANTIFLAG_STACK);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_MYSHOP", CItemData::ITEM_ANTIFLAG_MYSHOP);
	PyModule_AddIntConstant(poModule, "ANTIFLAG_SAFEBOX", CItemData::ITEM_ANTIFLAG_SAFEBOX);
#ifdef ENABLE_DESTROY_DIALOG
	PyModule_AddIntConstant(poModule, "ANTIFLAG_DESTROY", CItemData::ITEM_ANTIFLAG_DESTROY);
#endif
	PyModule_AddIntConstant(poModule, "ANTIFLAG_RT_REMOVE", CItemData::ITEM_ANTIFLAG_RT_REMOVE);

	PyModule_AddIntConstant(poModule, "WEARABLE_BODY", CItemData::WEARABLE_BODY);
	PyModule_AddIntConstant(poModule, "WEARABLE_HEAD", CItemData::WEARABLE_HEAD);
	PyModule_AddIntConstant(poModule, "WEARABLE_FOOTS", CItemData::WEARABLE_FOOTS);
	PyModule_AddIntConstant(poModule, "WEARABLE_WRIST", CItemData::WEARABLE_WRIST);
	PyModule_AddIntConstant(poModule, "WEARABLE_WEAPON", CItemData::WEARABLE_WEAPON);
	PyModule_AddIntConstant(poModule, "WEARABLE_NECK", CItemData::WEARABLE_NECK);
	PyModule_AddIntConstant(poModule, "WEARABLE_EAR", CItemData::WEARABLE_EAR);
	PyModule_AddIntConstant(poModule, "WEARABLE_UNIQUE", CItemData::WEARABLE_UNIQUE);
	PyModule_AddIntConstant(poModule, "WEARABLE_SHIELD", CItemData::WEARABLE_SHIELD);
	PyModule_AddIntConstant(poModule, "WEARABLE_ARROW", CItemData::WEARABLE_ARROW);
	PyModule_AddIntConstant(poModule, "WEARABLE_BELT", CItemData::WEARABLE_BELT);
#ifdef ENABLE_PENDANT_SYSTEM
	PyModule_AddIntConstant(poModule, "WEARABLE_PENDANT", CItemData::WEARABLE_PENDANT);
#endif

	PyModule_AddIntConstant(poModule, "ARMOR_BODY", CItemData::ARMOR_BODY);
	PyModule_AddIntConstant(poModule, "ARMOR_HEAD", CItemData::ARMOR_HEAD);
	PyModule_AddIntConstant(poModule, "ARMOR_SHIELD", CItemData::ARMOR_SHIELD);
	PyModule_AddIntConstant(poModule, "ARMOR_WRIST", CItemData::ARMOR_WRIST);
	PyModule_AddIntConstant(poModule, "ARMOR_FOOTS", CItemData::ARMOR_FOOTS);
	PyModule_AddIntConstant(poModule, "ARMOR_NECK", CItemData::ARMOR_NECK);
	PyModule_AddIntConstant(poModule, "ARMOR_EAR", CItemData::ARMOR_EAR);
	PyModule_AddIntConstant(poModule, "ARMOR_BELT", CItemData::ARMOR_BELT);
#ifdef ENABLE_PENDANT_SYSTEM
	PyModule_AddIntConstant(poModule, "ARMOR_PENDANT", CItemData::ARMOR_PENDANT);
#endif

	PyModule_AddIntConstant(poModule, "ITEM_APPLY_MAX_NUM", CItemData::ITEM_APPLY_MAX_NUM);
	PyModule_AddIntConstant(poModule, "ITEM_SOCKET_MAX_NUM", CItemData::ITEM_SOCKET_MAX_NUM);

	PyModule_AddIntConstant(poModule, "APPLY_NONE", CItemData::APPLY_NONE);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_HP", CItemData::APPLY_MAX_HP);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_SP", CItemData::APPLY_MAX_SP);
	PyModule_AddIntConstant(poModule, "APPLY_CON", CItemData::APPLY_CON);
	PyModule_AddIntConstant(poModule, "APPLY_INT", CItemData::APPLY_INT);
	PyModule_AddIntConstant(poModule, "APPLY_STR", CItemData::APPLY_STR);
	PyModule_AddIntConstant(poModule, "APPLY_DEX", CItemData::APPLY_DEX);
	PyModule_AddIntConstant(poModule, "APPLY_ATT_SPEED", CItemData::APPLY_ATT_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_MOV_SPEED", CItemData::APPLY_MOV_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_CAST_SPEED", CItemData::APPLY_CAST_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_HP_REGEN", CItemData::APPLY_HP_REGEN);
	PyModule_AddIntConstant(poModule, "APPLY_SP_REGEN", CItemData::APPLY_SP_REGEN);
	PyModule_AddIntConstant(poModule, "APPLY_POISON_PCT", CItemData::APPLY_POISON_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_STUN_PCT", CItemData::APPLY_STUN_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_SLOW_PCT", CItemData::APPLY_SLOW_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_CRITICAL_PCT", CItemData::APPLY_CRITICAL_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_PENETRATE_PCT", CItemData::APPLY_PENETRATE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_HUMAN", CItemData::APPLY_ATTBONUS_HUMAN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_ANIMAL", CItemData::APPLY_ATTBONUS_ANIMAL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_ORC", CItemData::APPLY_ATTBONUS_ORC);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_MILGYO", CItemData::APPLY_ATTBONUS_MILGYO);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_UNDEAD", CItemData::APPLY_ATTBONUS_UNDEAD);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_DEVIL", CItemData::APPLY_ATTBONUS_DEVIL);
	PyModule_AddIntConstant(poModule, "APPLY_STEAL_HP", CItemData::APPLY_STEAL_HP);
	PyModule_AddIntConstant(poModule, "APPLY_STEAL_SP", CItemData::APPLY_STEAL_SP);
	PyModule_AddIntConstant(poModule, "APPLY_MANA_BURN_PCT", CItemData::APPLY_MANA_BURN_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_DAMAGE_SP_RECOVER", CItemData::APPLY_DAMAGE_SP_RECOVER);
	PyModule_AddIntConstant(poModule, "APPLY_BLOCK", CItemData::APPLY_BLOCK);
	PyModule_AddIntConstant(poModule, "APPLY_DODGE", CItemData::APPLY_DODGE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_SWORD", CItemData::APPLY_RESIST_SWORD);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_TWOHAND", CItemData::APPLY_RESIST_TWOHAND);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_DAGGER", CItemData::APPLY_RESIST_DAGGER);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_BELL", CItemData::APPLY_RESIST_BELL);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_FAN", CItemData::APPLY_RESIST_FAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_BOW", CItemData::APPLY_RESIST_BOW);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_FIRE", CItemData::APPLY_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_ELEC", CItemData::APPLY_RESIST_ELEC);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_MAGIC", CItemData::APPLY_RESIST_MAGIC);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_WIND", CItemData::APPLY_RESIST_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_REFLECT_MELEE", CItemData::APPLY_REFLECT_MELEE);
	PyModule_AddIntConstant(poModule, "APPLY_REFLECT_CURSE", CItemData::APPLY_REFLECT_CURSE);
	PyModule_AddIntConstant(poModule, "APPLY_POISON_REDUCE", CItemData::APPLY_POISON_REDUCE);
	PyModule_AddIntConstant(poModule, "APPLY_KILL_SP_RECOVER", CItemData::APPLY_KILL_SP_RECOVER);
	PyModule_AddIntConstant(poModule, "APPLY_EXP_DOUBLE_BONUS", CItemData::APPLY_EXP_DOUBLE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_GOLD_DOUBLE_BONUS", CItemData::APPLY_GOLD_DOUBLE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_ITEM_DROP_BONUS", CItemData::APPLY_ITEM_DROP_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_POTION_BONUS", CItemData::APPLY_POTION_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_KILL_HP_RECOVER", CItemData::APPLY_KILL_HP_RECOVER);
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_STUN", CItemData::APPLY_IMMUNE_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_SLOW", CItemData::APPLY_IMMUNE_SLOW);
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_FALL", CItemData::APPLY_IMMUNE_FALL);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL", CItemData::APPLY_SKILL);
	PyModule_AddIntConstant(poModule, "APPLY_BOW_DISTANCE", CItemData::APPLY_BOW_DISTANCE);
	PyModule_AddIntConstant(poModule, "APPLY_ATT_GRADE_BONUS", CItemData::APPLY_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_DEF_GRADE_BONUS", CItemData::APPLY_DEF_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MAGIC_ATT_GRADE", CItemData::APPLY_MAGIC_ATT_GRADE);
	PyModule_AddIntConstant(poModule, "APPLY_MAGIC_DEF_GRADE", CItemData::APPLY_MAGIC_DEF_GRADE);
	PyModule_AddIntConstant(poModule, "APPLY_CURSE_PCT", CItemData::APPLY_CURSE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_STAMINA", CItemData::APPLY_MAX_STAMINA);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_WARRIOR", CItemData::APPLY_ATTBONUS_WARRIOR);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_ASSASSIN", CItemData::APPLY_ATTBONUS_ASSASSIN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_SURA", CItemData::APPLY_ATTBONUS_SURA);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_SHAMAN", CItemData::APPLY_ATTBONUS_SHAMAN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_MONSTER", CItemData::APPLY_ATTBONUS_MONSTER);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_ATTBONUS", CItemData::APPLY_MALL_ATTBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_DEFBONUS", CItemData::APPLY_MALL_DEFBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_EXPBONUS", CItemData::APPLY_MALL_EXPBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_ITEMBONUS", CItemData::APPLY_MALL_ITEMBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MALL_GOLDBONUS", CItemData::APPLY_MALL_GOLDBONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_HP_PCT", CItemData::APPLY_MAX_HP_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_MAX_SP_PCT", CItemData::APPLY_MAX_SP_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_BONUS", CItemData::APPLY_SKILL_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_HIT_DAMAGE_BONUS", CItemData::APPLY_NORMAL_HIT_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DEFEND_BONUS", CItemData::APPLY_SKILL_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_HIT_DEFEND_BONUS", CItemData::APPLY_NORMAL_HIT_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_EXTRACT_HP_PCT", CItemData::APPLY_EXTRACT_HP_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_PC_BANG_EXP_BONUS", CItemData::APPLY_PC_BANG_EXP_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_PC_BANG_DROP_BONUS", CItemData::APPLY_PC_BANG_DROP_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_WARRIOR", CItemData::APPLY_RESIST_WARRIOR);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_ASSASSIN", CItemData::APPLY_RESIST_ASSASSIN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_SURA", CItemData::APPLY_RESIST_SURA);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_SHAMAN", CItemData::APPLY_RESIST_SHAMAN);
	PyModule_AddIntConstant(poModule, "APPLY_ENERGY", CItemData::APPLY_ENERGY);
	PyModule_AddIntConstant(poModule, "APPLY_DEF_GRADE", CItemData::APPLY_DEF_GRADE);
	PyModule_AddIntConstant(poModule, "APPLY_COSTUME_ATTR_BONUS", CItemData::APPLY_COSTUME_ATTR_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MAGIC_ATTBONUS_PER", CItemData::APPLY_MAGIC_ATTBONUS_PER);
	PyModule_AddIntConstant(poModule, "APPLY_MELEE_MAGIC_ATTBONUS_PER", CItemData::APPLY_MELEE_MAGIC_ATTBONUS_PER);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_ICE", CItemData::APPLY_RESIST_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_EARTH", CItemData::APPLY_RESIST_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_DARK", CItemData::APPLY_RESIST_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_ANTI_CRITICAL_PCT", CItemData::APPLY_ANTI_CRITICAL_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_ANTI_PENETRATE_PCT", CItemData::APPLY_ANTI_PENETRATE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_BLEEDING_REDUCE", CItemData::APPLY_BLEEDING_REDUCE);
	PyModule_AddIntConstant(poModule, "APPLY_BLEEDING_PCT", CItemData::APPLY_BLEEDING_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_WOLFMAN", CItemData::APPLY_ATTBONUS_WOLFMAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_WOLFMAN", CItemData::APPLY_RESIST_WOLFMAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_CLAW", CItemData::APPLY_RESIST_CLAW);
	PyModule_AddIntConstant(poModule, "APPLY_ACCEDRAIN_RATE", CItemData::APPLY_ACCEDRAIN_RATE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_MAGIC_REDUCTION", CItemData::APPLY_RESIST_MAGIC_REDUCTION);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_ELECT", CItemData::APPLY_ENCHANT_ELECT);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_FIRE", CItemData::APPLY_ENCHANT_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_ICE", CItemData::APPLY_ENCHANT_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_WIND", CItemData::APPLY_ENCHANT_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_EARTH", CItemData::APPLY_ENCHANT_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_DARK", CItemData::APPLY_ENCHANT_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_CZ", CItemData::APPLY_ATTBONUS_CZ);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_INSECT", CItemData::APPLY_ATTBONUS_INSECT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_DESERT", CItemData::APPLY_ATTBONUS_DESERT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_SWORD", CItemData::APPLY_ATTBONUS_SWORD);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_TWOHAND", CItemData::APPLY_ATTBONUS_TWOHAND);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_DAGGER", CItemData::APPLY_ATTBONUS_DAGGER);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_BELL", CItemData::APPLY_ATTBONUS_BELL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_FAN", CItemData::APPLY_ATTBONUS_FAN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_BOW", CItemData::APPLY_ATTBONUS_BOW);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_CLAW", CItemData::APPLY_ATTBONUS_CLAW);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_HUMAN", CItemData::APPLY_RESIST_HUMAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_MOUNT_FALL", CItemData::APPLY_RESIST_MOUNT_FALL);
	PyModule_AddIntConstant(poModule, "APPLY_MOUNT", CItemData::APPLY_MOUNT);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_SAMYEON", CItemData::APPLY_SKILL_DAMAGE_SAMYEON);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_TANHWAN", CItemData::APPLY_SKILL_DAMAGE_TANHWAN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_PALBANG", CItemData::APPLY_SKILL_DAMAGE_PALBANG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GIGONGCHAM", CItemData::APPLY_SKILL_DAMAGE_GIGONGCHAM);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GYOKSAN", CItemData::APPLY_SKILL_DAMAGE_GYOKSAN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GEOMPUNG", CItemData::APPLY_SKILL_DAMAGE_GEOMPUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_AMSEOP", CItemData::APPLY_SKILL_DAMAGE_AMSEOP);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GUNGSIN", CItemData::APPLY_SKILL_DAMAGE_GUNGSIN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_CHARYUN", CItemData::APPLY_SKILL_DAMAGE_CHARYUN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_SANGONG", CItemData::APPLY_SKILL_DAMAGE_SANGONG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_YEONSA", CItemData::APPLY_SKILL_DAMAGE_YEONSA);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_KWANKYEOK", CItemData::APPLY_SKILL_DAMAGE_KWANKYEOK);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GIGUNG", CItemData::APPLY_SKILL_DAMAGE_GIGUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_HWAJO", CItemData::APPLY_SKILL_DAMAGE_HWAJO);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_SWAERYUNG", CItemData::APPLY_SKILL_DAMAGE_SWAERYUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_YONGKWON", CItemData::APPLY_SKILL_DAMAGE_YONGKWON);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_PABEOB", CItemData::APPLY_SKILL_DAMAGE_PABEOB);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_MARYUNG", CItemData::APPLY_SKILL_DAMAGE_MARYUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_HWAYEOMPOK", CItemData::APPLY_SKILL_DAMAGE_HWAYEOMPOK);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_MAHWAN", CItemData::APPLY_SKILL_DAMAGE_MAHWAN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_BIPABU", CItemData::APPLY_SKILL_DAMAGE_BIPABU);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_YONGBI", CItemData::APPLY_SKILL_DAMAGE_YONGBI);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_PAERYONG", CItemData::APPLY_SKILL_DAMAGE_PAERYONG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_NOEJEON", CItemData::APPLY_SKILL_DAMAGE_NOEJEON);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_BYEURAK", CItemData::APPLY_SKILL_DAMAGE_BYEURAK);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_CHAIN", CItemData::APPLY_SKILL_DAMAGE_CHAIN);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_CHAYEOL", CItemData::APPLY_SKILL_DAMAGE_CHAYEOL);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_SALPOONG", CItemData::APPLY_SKILL_DAMAGE_SALPOONG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_GONGDAB", CItemData::APPLY_SKILL_DAMAGE_GONGDAB);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_PASWAE", CItemData::APPLY_SKILL_DAMAGE_PASWAE);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE", CItemData::APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE", CItemData::APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE", CItemData::APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE", CItemData::APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_FIRE", CItemData::APPLY_HIT_BUFF_ENCHANT_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_ICE", CItemData::APPLY_HIT_BUFF_ENCHANT_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_ELEC", CItemData::APPLY_HIT_BUFF_ENCHANT_ELEC);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_WIND", CItemData::APPLY_HIT_BUFF_ENCHANT_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_DARK", CItemData::APPLY_HIT_BUFF_ENCHANT_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_ENCHANT_EARTH", CItemData::APPLY_HIT_BUFF_ENCHANT_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_FIRE", CItemData::APPLY_HIT_BUFF_RESIST_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_ICE", CItemData::APPLY_HIT_BUFF_RESIST_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_ELEC", CItemData::APPLY_HIT_BUFF_RESIST_ELEC);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_WIND", CItemData::APPLY_HIT_BUFF_RESIST_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_DARK", CItemData::APPLY_HIT_BUFF_RESIST_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_RESIST_EARTH", CItemData::APPLY_HIT_BUFF_RESIST_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED", CItemData::APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED", CItemData::APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT", CItemData::APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_JEOKRANG_DEF_BONUS", CItemData::APPLY_USE_SKILL_JEOKRANG_DEF_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GWIGEOM_DEF_BONUS", CItemData::APPLY_USE_SKILL_GWIGEOM_DEF_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_MANASHILED_CASTING_SPEED", CItemData::APPLY_USE_SKILL_MANASHILED_CASTING_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_HOSIN_DEF_BONUS", CItemData::APPLY_USE_SKILL_HOSIN_DEF_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS", CItemData::APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED", CItemData::APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS", CItemData::APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DURATION_INCREASE_EUNHYUNG", CItemData::APPLY_SKILL_DURATION_INCREASE_EUNHYUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DURATION_INCREASE_GYEONGGONG", CItemData::APPLY_SKILL_DURATION_INCREASE_GYEONGGONG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG", CItemData::APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG);
	PyModule_AddIntConstant(poModule, "APPLY_SKILL_DURATION_INCREASE_JEOKRANG", CItemData::APPLY_SKILL_DURATION_INCREASE_JEOKRANG);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PALBANG_HP_ABSORB", CItemData::APPLY_USE_SKILL_PALBANG_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_AMSEOP_HP_ABSORB", CItemData::APPLY_USE_SKILL_AMSEOP_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_YEONSA_HP_ABSORB", CItemData::APPLY_USE_SKILL_YEONSA_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_YONGBI_HP_ABSORB", CItemData::APPLY_USE_SKILL_YONGBI_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHAIN_HP_ABSORB", CItemData::APPLY_USE_SKILL_CHAIN_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PASWAE_SP_ABSORB", CItemData::APPLY_USE_SKILL_PASWAE_SP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GIGONGCHAM_STUN", CItemData::APPLY_USE_SKILL_GIGONGCHAM_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHARYUN_STUN", CItemData::APPLY_USE_SKILL_CHARYUN_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PABEOB_STUN", CItemData::APPLY_USE_SKILL_PABEOB_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_MAHWAN_STUN", CItemData::APPLY_USE_SKILL_MAHWAN_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GONGDAB_STUN", CItemData::APPLY_USE_SKILL_GONGDAB_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SAMYEON_STUN", CItemData::APPLY_USE_SKILL_SAMYEON_STUN);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GYOKSAN_KNOCKBACK", CItemData::APPLY_USE_SKILL_GYOKSAN_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SEOMJEON_KNOCKBACK", CItemData::APPLY_USE_SKILL_SEOMJEON_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK", CItemData::APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK", CItemData::APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GONGDAB_KNOCKBACK", CItemData::APPLY_USE_SKILL_GONGDAB_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK", CItemData::APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER", CItemData::APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_STONE", CItemData::APPLY_ATTBONUS_STONE);
	PyModule_AddIntConstant(poModule, "APPLY_DAMAGE_HP_RECOVERY", CItemData::APPLY_DAMAGE_HP_RECOVERY);
	PyModule_AddIntConstant(poModule, "APPLY_DAMAGE_SP_RECOVERY", CItemData::APPLY_DAMAGE_SP_RECOVERY);
	PyModule_AddIntConstant(poModule, "APPLY_ALIGNMENT_DAMAGE_BONUS", CItemData::APPLY_ALIGNMENT_DAMAGE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_NORMAL_DAMAGE_GUARD", CItemData::APPLY_NORMAL_DAMAGE_GUARD);
	PyModule_AddIntConstant(poModule, "APPLY_MORE_THEN_HP90_DAMAGE_REDUCE", CItemData::APPLY_MORE_THEN_HP90_DAMAGE_REDUCE);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_TUSOK_HP_ABSORB", CItemData::APPLY_USE_SKILL_TUSOK_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PAERYONG_HP_ABSORB", CItemData::APPLY_USE_SKILL_PAERYONG_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_BYEURAK_HP_ABSORB", CItemData::APPLY_USE_SKILL_BYEURAK_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_FIRST_ATTRIBUTE_BONUS", CItemData::APPLY_FIRST_ATTRIBUTE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_SECOND_ATTRIBUTE_BONUS", CItemData::APPLY_SECOND_ATTRIBUTE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_THIRD_ATTRIBUTE_BONUS", CItemData::APPLY_THIRD_ATTRIBUTE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_FOURTH_ATTRIBUTE_BONUS", CItemData::APPLY_FOURTH_ATTRIBUTE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_FIFTH_ATTRIBUTE_BONUS", CItemData::APPLY_FIFTH_ATTRIBUTE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER", CItemData::APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHAYEOL_HP_ABSORB", CItemData::APPLY_USE_SKILL_CHAYEOL_HP_ABSORB);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_STR", CItemData::APPLY_SUNGMA_STR);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_HP", CItemData::APPLY_SUNGMA_HP);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_MOVE", CItemData::APPLY_SUNGMA_MOVE);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_IMMUNE", CItemData::APPLY_SUNGMA_IMMUNE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_PCT", CItemData::APPLY_HIT_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_RANDOM", CItemData::APPLY_RANDOM);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_HUMAN", CItemData::APPLY_ATTBONUS_PER_HUMAN);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_ANIMAL", CItemData::APPLY_ATTBONUS_PER_ANIMAL);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_ORC", CItemData::APPLY_ATTBONUS_PER_ORC);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_MILGYO", CItemData::APPLY_ATTBONUS_PER_MILGYO);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_UNDEAD", CItemData::APPLY_ATTBONUS_PER_UNDEAD);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_DEVIL", CItemData::APPLY_ATTBONUS_PER_DEVIL);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_ELECT", CItemData::APPLY_ENCHANT_PER_ELECT);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_FIRE", CItemData::APPLY_ENCHANT_PER_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_ICE", CItemData::APPLY_ENCHANT_PER_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_WIND", CItemData::APPLY_ENCHANT_PER_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_EARTH", CItemData::APPLY_ENCHANT_PER_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_PER_DARK", CItemData::APPLY_ENCHANT_PER_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_CZ", CItemData::APPLY_ATTBONUS_PER_CZ);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_INSECT", CItemData::APPLY_ATTBONUS_PER_INSECT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_DESERT", CItemData::APPLY_ATTBONUS_PER_DESERT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_STONE", CItemData::APPLY_ATTBONUS_PER_STONE);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PER_MONSTER", CItemData::APPLY_ATTBONUS_PER_MONSTER);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_HUMAN", CItemData::APPLY_RESIST_PER_HUMAN);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_ICE", CItemData::APPLY_RESIST_PER_ICE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_DARK", CItemData::APPLY_RESIST_PER_DARK);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_EARTH", CItemData::APPLY_RESIST_PER_EARTH);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_FIRE", CItemData::APPLY_RESIST_PER_FIRE);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_ELEC", CItemData::APPLY_RESIST_PER_ELEC);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_MAGIC", CItemData::APPLY_RESIST_PER_MAGIC);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_PER_WIND", CItemData::APPLY_RESIST_PER_WIND);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_SUNGMA_STR", CItemData::APPLY_HIT_BUFF_SUNGMA_STR);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_SUNGMA_MOVE", CItemData::APPLY_HIT_BUFF_SUNGMA_MOVE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_SUNGMA_HP", CItemData::APPLY_HIT_BUFF_SUNGMA_HP);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_BUFF_SUNGMA_IMMUNE", CItemData::APPLY_HIT_BUFF_SUNGMA_IMMUNE);
	PyModule_AddIntConstant(poModule, "APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER", CItemData::APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER);
	PyModule_AddIntConstant(poModule, "APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER", CItemData::APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_AUTO_HP_RECOVERY", CItemData::APPLY_HIT_AUTO_HP_RECOVERY);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_AUTO_SP_RECOVERY", CItemData::APPLY_HIT_AUTO_SP_RECOVERY);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_COOLTIME_DECREASE_ALL", CItemData::APPLY_USE_SKILL_COOLTIME_DECREASE_ALL);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_STONE_ATTBONUS_STONE", CItemData::APPLY_HIT_STONE_ATTBONUS_STONE);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_STONE_DEF_GRADE_BONUS", CItemData::APPLY_HIT_STONE_DEF_GRADE_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_KILL_BOSS_ITEM_BONUS", CItemData::APPLY_KILL_BOSS_ITEM_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_MOB_HIT_MOB_AGGRESSIVE", CItemData::APPLY_MOB_HIT_MOB_AGGRESSIVE);
	PyModule_AddIntConstant(poModule, "APPLY_NO_DEATH_AND_HP_RECOVERY30", CItemData::APPLY_NO_DEATH_AND_HP_RECOVERY30);
	PyModule_AddIntConstant(poModule, "APPLY_AUTO_PICKUP", CItemData::APPLY_AUTO_PICKUP);
	PyModule_AddIntConstant(poModule, "APPLY_MOUNT_NO_KNOCKBACK", CItemData::APPLY_MOUNT_NO_KNOCKBACK);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_PER_STR", CItemData::APPLY_SUNGMA_PER_STR);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_PER_HP", CItemData::APPLY_SUNGMA_PER_HP);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_PER_MOVE", CItemData::APPLY_SUNGMA_PER_MOVE);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_PER_IMMUNE", CItemData::APPLY_SUNGMA_PER_IMMUNE);
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_POISON100", CItemData::APPLY_IMMUNE_POISON100);
	PyModule_AddIntConstant(poModule, "APPLY_IMMUNE_BLEEDING100", CItemData::APPLY_IMMUNE_BLEEDING100);
	PyModule_AddIntConstant(poModule, "APPLY_MONSTER_DEFEND_BONUS", CItemData::APPLY_MONSTER_DEFEND_BONUS);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_FINISH_NEXT_COOLTIME_DECREASE_PCT", CItemData::APPLY_USE_SKILL_FINISH_NEXT_COOLTIME_DECREASE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_ILGWANGPYO_NEXT_COOLTIME_DECREASE_PCT", CItemData::APPLY_USE_SKILL_ILGWANGPYO_NEXT_COOLTIME_DECREASE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_PUNGLOEPO_NEXT_COOLTIME_DECREASE_PCT", CItemData::APPLY_USE_SKILL_PUNGLOEPO_NEXT_COOLTIME_DECREASE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_GEOMAGGWI_NEXT_COOLTIME_DECREASE_PCT", CItemData::APPLY_USE_SKILL_GEOMAGGWI_NEXT_COOLTIME_DECREASE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_MABEOBAGGWI_NEXT_COOLTIME_DECREASE_PCT", CItemData::APPLY_USE_SKILL_MABEOBAGGWI_NEXT_COOLTIME_DECREASE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_METEO_NEXT_COOLTIME_DECREASE_PCT", CItemData::APPLY_USE_SKILL_METEO_NEXT_COOLTIME_DECREASE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_CHEONUN_NEXT_COOLTIME_DECREASE_PCT", CItemData::APPLY_USE_SKILL_CHEONUN_NEXT_COOLTIME_DECREASE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_USE_SKILL_ILIPUNGU_NEXT_COOLTIME_DECREASE_PCT", CItemData::APPLY_USE_SKILL_ILIPUNGU_NEXT_COOLTIME_DECREASE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_GLOVE_BASE_EFFECT_INCREASE", CItemData::APPLY_GLOVE_BASE_EFFECT_INCREASE);
	PyModule_AddIntConstant(poModule, "APPLY_NO_FALLEN", CItemData::APPLY_NO_FALLEN);
	PyModule_AddIntConstant(poModule, "APPLY_HIT_AREA_ELEC_ATTACK", CItemData::APPLY_HIT_AREA_ELEC_ATTACK);
	PyModule_AddIntConstant(poModule, "APPLY_BLOCK_PENETRATE_PCT", CItemData::APPLY_BLOCK_PENETRATE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_MYSTERY_DUNGEON", CItemData::APPLY_ATTBONUS_MYSTERY_DUNGEON);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_DRAGON", CItemData::APPLY_ATTBONUS_DRAGON);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_MOON", CItemData::APPLY_ATTBONUS_MOON);
	PyModule_AddIntConstant(poModule, "APPLY_BONUS_BLOCK_PENETRATE_PCT", CItemData::APPLY_BONUS_BLOCK_PENETRATE_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_BONUS_HIT_PCT", CItemData::APPLY_BONUS_HIT_PCT);
	PyModule_AddIntConstant(poModule, "APPLY_REFUSE_EXP", CItemData::APPLY_REFUSE_EXP);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_BOSS", CItemData::APPLY_ATTBONUS_BOSS);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_ELEMENTS", CItemData::APPLY_ATTBONUS_ELEMENTS);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_ELEMENTS", CItemData::APPLY_ENCHANT_ELEMENTS);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_CHARACTERS", CItemData::APPLY_ATTBONUS_CHARACTERS);
	PyModule_AddIntConstant(poModule, "APPLY_ENCHANT_CHARACTERS", CItemData::APPLY_ENCHANT_CHARACTERS);
	PyModule_AddIntConstant(poModule, "APPLY_RESIST_MONSTER", CItemData::APPLY_RESIST_MONSTER);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_MEDI_PVM", CItemData::APPLY_ATTBONUS_MEDI_PVM);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PVM_STR", CItemData::APPLY_ATTBONUS_PVM_STR);
	PyModule_AddIntConstant(poModule, "APPLY_ATTBONUS_PVM_BERSERKER", CItemData::APPLY_ATTBONUS_PVM_BERSERKER);


#ifdef ENABLE_COLLECTIONS_SYSTEM
	PyModule_AddIntConstant(poModule, "USE_COLLECTION_SCROLL", CItemData::USE_COLLECTION_SCROLL);
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "RESOURCE_AURA", CItemData::RESOURCE_AURA);

	PyModule_AddIntConstant(poModule, "AURA_GRADE_NONE", CItemData::AURA_GRADE_NONE);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_ORDINARY", CItemData::AURA_GRADE_ORDINARY);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_SIMPLE", CItemData::AURA_GRADE_SIMPLE);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_NOBLE", CItemData::AURA_GRADE_NOBLE);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_SPARKLING", CItemData::AURA_GRADE_SPARKLING);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_MAGNIFICENT", CItemData::AURA_GRADE_MAGNIFICENT);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_RADIANT", CItemData::AURA_GRADE_RADIANT);
	PyModule_AddIntConstant(poModule, "AURA_GRADE_MAX_NUM", CItemData::AURA_GRADE_MAX_NUM);
	PyModule_AddIntConstant(poModule, "AURA_BOOST_ITEM_VNUM_BASE", CItemData::AURA_BOOST_ITEM_VNUM_BASE);

	PyModule_AddIntConstant(poModule, "ITEM_AURA_BOOST_ERASER", CItemData::ITEM_AURA_BOOST_ERASER);
	PyModule_AddIntConstant(poModule, "ITEM_AURA_BOOST_WEAK", CItemData::ITEM_AURA_BOOST_WEAK);
	PyModule_AddIntConstant(poModule, "ITEM_AURA_BOOST_NORMAL", CItemData::ITEM_AURA_BOOST_NORMAL);
	PyModule_AddIntConstant(poModule, "ITEM_AURA_BOOST_STRONG", CItemData::ITEM_AURA_BOOST_STRONG);
	PyModule_AddIntConstant(poModule, "ITEM_AURA_BOOST_ULTIMATE", CItemData::ITEM_AURA_BOOST_ULTIMATE);
	PyModule_AddIntConstant(poModule, "ITEM_AURA_BOOST_MAX", CItemData::ITEM_AURA_BOOST_MAX);
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	PyModule_AddIntConstant(poModule, "ITEM_ANTIFLAG_PETFEED", CItemData::ITEM_ANTIFLAG_PETFEED);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_GROWTH_PET", CItemData::ITEM_TYPE_GROWTH_PET);

	PyModule_AddIntConstant(poModule, "PET_EGG", CItemData::PET_EGG);
	PyModule_AddIntConstant(poModule, "PET_UPBRINGING", CItemData::PET_UPBRINGING);
	PyModule_AddIntConstant(poModule, "PET_BAG", CItemData::PET_BAG);
	PyModule_AddIntConstant(poModule, "PET_FEEDSTUFF", CItemData::PET_FEEDSTUFF);
	PyModule_AddIntConstant(poModule, "PET_SKILL", CItemData::PET_SKILL);
	PyModule_AddIntConstant(poModule, "PET_SKILL_DEL_BOOK", CItemData::PET_SKILL_DEL_BOOK);
	PyModule_AddIntConstant(poModule, "PET_NAME_CHANGE", CItemData::PET_NAME_CHANGE);
	PyModule_AddIntConstant(poModule, "PET_EXPFOOD", CItemData::PET_EXPFOOD);
	PyModule_AddIntConstant(poModule, "PET_SKILL_ALL_DEL_BOOK", CItemData::PET_SKILL_ALL_DEL_BOOK);
	PyModule_AddIntConstant(poModule, "PET_EXPFOOD_PER", CItemData::PET_EXPFOOD_PER);
	PyModule_AddIntConstant(poModule, "PET_ATTR_DETERMINE", CItemData::PET_ATTR_DETERMINE);
	PyModule_AddIntConstant(poModule, "PET_ATTR_CHANGE", CItemData::PET_ATTR_CHANGE);
	PyModule_AddIntConstant(poModule, "PET_PAY", CItemData::PET_PAY);
	PyModule_AddIntConstant(poModule, "PET_PRIMIUM_FEEDSTUFF", CItemData::PET_PRIMIUM_FEEDSTUFF);

	PyModule_AddIntConstant(poModule, "EGG_USE_SUCCESS", CItemData::EGG_USE_SUCCESS);
	PyModule_AddIntConstant(poModule, "EGG_USE_FAILED_BECAUSE_NAME", CItemData::EGG_USE_FAILED_BECAUSE_NAME);
	PyModule_AddIntConstant(poModule, "EGG_USE_FAILED_TIMEOVER", CItemData::EGG_USE_FAILED_TIMEOVER);

	PyModule_AddIntConstant(poModule, "PET_EGG_USE_TRUE", CItemData::PET_EGG_USE_TRUE);
	PyModule_AddIntConstant(poModule, "PET_EGG_USE_FAILED_BECAUSE_TRADING", CItemData::PET_EGG_USE_FAILED_BECAUSE_TRADING);
	PyModule_AddIntConstant(poModule, "PET_EGG_USE_FAILED_BECAUSE_SHOP_OPEN", CItemData::PET_EGG_USE_FAILED_BECAUSE_SHOP_OPEN);
	PyModule_AddIntConstant(poModule, "PET_EGG_USE_FAILED_BECAUSE_MALL_OPEN", CItemData::PET_EGG_USE_FAILED_BECAUSE_MALL_OPEN);
	PyModule_AddIntConstant(poModule, "PET_EGG_USE_FAILED_BECAUSE_SAFEBOX_OPEN", CItemData::PET_EGG_USE_FAILED_BECAUSE_SAFEBOX_OPEN);

	PyModule_AddIntConstant(poModule, "NAME_CHANGE_USE_SUCCESS", CItemData::NAME_CHANGE_USE_SUCCESS);
	PyModule_AddIntConstant(poModule, "NAME_CHANGE_USE_FAILED_BECAUSE_NAME", CItemData::NAME_CHANGE_USE_FAILED_BECAUSE_NAME);

	PyModule_AddIntConstant(poModule, "PET_ATTR_CHANGE_ITEMVNUM", CItemData::PET_ATTR_CHANGE_ITEMVNUM);
	PyModule_AddIntConstant(poModule, "PET_HATCHING_MONEY", CItemData::PET_HATCHING_MONEY);
	PyModule_AddIntConstant(poModule, "PET_NAME_MAX_SIZE", CItemData::PET_NAME_MAX_SIZE);
	PyModule_AddIntConstant(poModule, "PET_NAME_MIN_SIZE", CItemData::PET_NAME_MIN_SIZE);
	PyModule_AddIntConstant(poModule, "E_SEAL_DATE_DEFAULT_TIMESTAMP", CItemData::E_SEAL_DATE_DEFAULT_TIMESTAMP);
#endif

#ifdef ENABLE_GUILDRENEWAL_SYSTEM
	PyModule_AddIntConstant(poModule, "GUILD_SLOT_START_INDEX",			210);
#endif

	PyModule_AddIntConstant(poModule, "ITEM_TYPE_MEDIUM", 36);
#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
	PyModule_AddIntConstant(poModule, "MEDIUM_MOVE_COSTUME_ATTR", 0);
	PyModule_AddIntConstant(poModule, "MEDIUM_MOVE_COSTUME_ACCE_ATTR", 1);
#endif
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_ALCHEMY", 40);
	PyModule_AddIntConstant(poModule, "ITEM_TYPE_FORCE", 41);
	PyModule_AddIntConstant(poModule, "WEAPON_CLAW", 8);
	PyModule_AddIntConstant(poModule, "WEAPON_QUIVER", 9);
	PyModule_AddIntConstant(poModule, "WEARABLE_GLOVE", 4096);
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_LOOTING", 72756);
	PyModule_AddIntConstant(poModule, "REFINE_TYPE_FORCE_UPGRADE", 8);
		PyModule_AddIntConstant(poModule, "ITEM_MAX_COUNT", 200);

	PyModule_AddIntConstant(poModule, "DS_SLOT1", CItemData::DS_SLOT1);
	PyModule_AddIntConstant(poModule, "DS_SLOT2", CItemData::DS_SLOT2);
	PyModule_AddIntConstant(poModule, "DS_SLOT3", CItemData::DS_SLOT3);
	PyModule_AddIntConstant(poModule, "DS_SLOT4", CItemData::DS_SLOT4);
	PyModule_AddIntConstant(poModule, "DS_SLOT5", CItemData::DS_SLOT5);
	PyModule_AddIntConstant(poModule, "DS_SLOT6", CItemData::DS_SLOT6);
#ifdef ENABLE_DRAGON_SOUL_7_SLOT
	PyModule_AddIntConstant(poModule, "DS_SLOT7", CItemData::DS_SLOT7);
#endif
	PyModule_AddIntConstant(poModule, "DS_SLOT_NUM_TYPES", CItemData::DS_SLOT_NUM_TYPES);

#ifdef ENABLE_GEM_CONVERTER
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_GEM_CONVERTER", 50935);
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_GEM_STONE", 50926);
#endif
#ifdef ENABLE_AURA_CLEAR
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_AURA_CLEAR", 89999);
	PyModule_AddIntConstant(poModule, "ITEM_VNUM_AURA_CLEAR_SCROLL", 49980);
#endif
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_STR", 239);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_HP", 240);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_MOVE", 241);
	PyModule_AddIntConstant(poModule, "APPLY_SUNGMA_IMMUNE", 242);
}