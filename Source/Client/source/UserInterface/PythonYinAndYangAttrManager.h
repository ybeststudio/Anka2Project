#pragma once

#include "Packet.h"
#include <map>
#include <vector>
#include <string>

#ifdef ENABLE_YIN_AND_YANG_ATTR
class CPythonYinAndYangAttrManager : public CSingleton<CPythonYinAndYangAttrManager>
{
public:
	enum EConstants
	{
		TYPE_YIN = 0,
		TYPE_YANG = 1,

		KEYWORD_SUPERNATURAL = 0,
		KEYWORD_CAST = 1,
		KEYWORD_PROVOKE = 2,
		KEYWORD_BACK_ATTACK = 3,
		KEYWORD_TALISMAN = 4,
		KEYWORD_REVIVE = 5,
		KEYWORD_SMITE = 6,
		KEYWORD_OCCUPY = 7,
		KEYWORD_BERSERK = 8,
		KEYWORD_MAX = 9,

		INVENTORY_SLOT_INDEX_RECENTLY_OBTAINED = 0,
		INVENTORY_DEFAULT_OPEN_SLOT_COUNT = 5,
		INVENTORY_EXPANSION_COUNT_ITEM_MAX = 10,
		INVENTORY_EXPANSION_COUNT_QUEST_MAX = 5,
		INVENTORY_EXPANSION_ITEM_NEED_COUNT = 20,

		NEED_GOLD_TO_DELETE_ATTR = 1000000,
		RECENTLY_OBTAINED_ATTR_DELETE_TIME_SEC = 86400,

		CONNECTION_TYPE_NONE = 0,
		CONNECTION_TYPE_WEAPON = 1,
		CONNECTION_TYPE_GLOVE = 2,

		YIN_AND_YANG_ATTACHABLE_ITEM_LEVEL_MIN = 75,
		YIN_AND_YANG_ENABLE_REMAIN_TIME_SEC_MAX = 86400,
		YIN_AND_YANG_ENABLE_TOGGLE_COOLTIME_SEC = 10,
	};

	struct TYinAndYangAttrProto
	{
		DWORD dwVnum;
		std::string strName;
		BYTE byAttrType;
		BYTE byAttrKeyword;
		std::string strJob;
		DWORD dwCoefficient;
		DWORD dwCooltime;
		BYTE byActivateType;
		std::string strTargetMonster;
		float fLv[10];
		float fY;
		DWORD dwApplicablePart;
		std::vector<DWORD> vecObtainableMap;
	};

	CPythonYinAndYangAttrManager();
	virtual ~CPythonYinAndYangAttrManager();

	bool Initialize(const char* c_szLocalePath = NULL);
	bool LoadYinAndYangAttrProto(const char* c_szFileName = NULL);
	bool LoadYinAndYangAttrDesc(const char* c_szLocalePath = NULL);

	void SetInventoryUIHandler(PyObject* poHandler);
	void SetHarmonyUIHandler(PyObject* poHandler);
	void Clear();

	bool GetYinAndYangAttrDescText(DWORD dwVnum, BYTE byStep, std::vector<std::string>& rVecLines);
	bool GetYinAndYangAttrDescTextMinMax(DWORD dwVnum, std::vector<std::string>& rVecLines);

	const TYinAndYangAttrProto* GetProto(DWORD dwVnum) const;
	BYTE GetYinAndYangAttrType(DWORD dwVnum) const;
	BYTE GetYinAndYangAttrKeyword(DWORD dwVnum) const;
	bool IsYinAndYangAttrAttachablePart(DWORD dwVnum, DWORD dwWearFlag) const;
	int GetNeedGoldToAttachAttr(DWORD dwVnum, BYTE byStep, WORD wCell) const;
	bool IsAttrAttachableItemType(DWORD dwItemVnum) const;
	bool IsAttrAttachableToItem(DWORD dwVnum, DWORD dwItemVnum) const;
	bool IsYinAndYangAttrConnected(DWORD dwVnum1, DWORD dwVnum2) const;
	BYTE GetConnectionTypeByKeyword(BYTE byKeyword) const;

	const std::vector<DWORD>* GetVnumsByKeyword(BYTE byKeyword) const;
	const std::vector<DWORD>* GetVnumsByWearAndType(DWORD dwWearFlag, BYTE byAttrType) const;
	const std::vector<DWORD>* GetObtainableMap(DWORD dwVnum) const;

	DWORD GetExpansionCountByItem() const { return m_dwExpansionCountByItem; }
	DWORD GetExpansionCountByQuest() const { return m_dwExpansionCountByQuest; }

	void SetSystemEnable(bool bEnable);
	bool IsSystemEnabled() const { return m_bSystemEnable; }

	void RefreshShowConnectedItemsEquipment();

	// Client-to-server requests
	bool SendCloseInventory();
	bool SendRequestInventoryMoveItem(BYTE bFromPos, BYTE bToPos);
	bool SendRequestInventoryDeleteItem(BYTE bPos);
	bool SendRequestRegisterRecentlyGainedItem();
	bool SendRequestAttachAttrToItem(BYTE bFromPos, BYTE bToPos);
	bool SendRequestInventoryExpansion();
	bool SendRequestEnableYinAndYangAttr(bool bEnable);

	// Server-to-client packet receiver
	bool RecvYinAndYangAttrPacket();

private:
	std::string FormatDescString(const std::string& strFormat, float fVal, float fY) const;
	std::string FormatDescMinMaxString(const std::string& strFormat, float fMin, float fMax, float fY) const;

private:
	PyObject* m_poInventoryUIHandler;
	PyObject* m_poHarmonyUIHandler;

	std::map<DWORD, TYinAndYangAttrProto> m_map_Proto;
	std::map<DWORD, std::string> m_map_Desc;
	std::map<BYTE, std::vector<DWORD>> m_map_KeywordToVnums;
	std::map<DWORD, std::map<BYTE, std::vector<DWORD>>> m_map_WearAndTypeToVnums;
	std::map<BYTE, BYTE> m_map_KeywordToConnectionType;

	DWORD m_dwExpansionCountByItem;
	DWORD m_dwExpansionCountByQuest;
	bool m_bSystemEnable;
};

void inityinAndYangAttr();
#endif // ENABLE_YIN_AND_YANG_ATTR
