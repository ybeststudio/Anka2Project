#include "StdAfx.h"
#include "PythonYinAndYangAttrManager.h"
#include "PythonNetworkStream.h"
#include "PythonPlayer.h"
#include "../GameLib/ItemManager.h"
#include "../GameLib/ItemData.h"
#include "../EterPack/EterPackManager.h"
#include <cmath>
#ifdef ENABLE_YIN_AND_YANG_ATTR

static void StringReplace(std::string& str, const std::string& from, const std::string& to)
{
	size_t pos = 0;
	while ((pos = str.find(from, pos)) != std::string::npos)
	{
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
}

static void SplitLines(const std::string& str, std::vector<std::string>& rVecLines)
{
	rVecLines.clear();
	size_t start = 0;
	size_t end = str.find('\n');
	while (end != std::string::npos)
	{
		rVecLines.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find('\n', start);
	}
	rVecLines.push_back(str.substr(start));
}

CPythonYinAndYangAttrManager::CPythonYinAndYangAttrManager()
	: m_poInventoryUIHandler(NULL),
	  m_poHarmonyUIHandler(NULL),
	  m_dwExpansionCountByItem(0),
	  m_dwExpansionCountByQuest(0),
	  m_bSystemEnable(false)
{
}

CPythonYinAndYangAttrManager::~CPythonYinAndYangAttrManager()
{
	Clear();
}

void CPythonYinAndYangAttrManager::Clear()
{
	m_poInventoryUIHandler = NULL;
	m_poHarmonyUIHandler = NULL;
}

void CPythonYinAndYangAttrManager::SetInventoryUIHandler(PyObject* poHandler)
{
	m_poInventoryUIHandler = poHandler;
}

void CPythonYinAndYangAttrManager::SetHarmonyUIHandler(PyObject* poHandler)
{
	m_poHarmonyUIHandler = poHandler;
}

bool CPythonYinAndYangAttrManager::Initialize(const char* c_szLocalePath)
{
	if (!LoadYinAndYangAttrProto())
		return false;

	if (!LoadYinAndYangAttrDesc(c_szLocalePath))
		return false;

	return true;
}

bool CPythonYinAndYangAttrManager::LoadYinAndYangAttrProto(const char* c_szFileName)
{
	std::string strFileName;
	if (c_szFileName && *c_szFileName)
		strFileName = c_szFileName;
	else
		strFileName = "shared/common/yin_and_yang_attr_proto.txt";

	CMappedFile file;
	LPCVOID pvData = NULL;

	if (!CEterPackManager::Instance().Get(file, strFileName.c_str(), &pvData))
	{
		strFileName = LocaleService_GetLocalePath();
		strFileName += "/yin_and_yang_attr_proto.txt";
		if (!CEterPackManager::Instance().Get(file, strFileName.c_str(), &pvData))
		{
			TraceError("CPythonYinAndYangAttrManager::LoadYinAndYangAttrProto: Failed to load %s", strFileName.c_str());
			return false;
		}
	}

	CMemoryTextFileLoader loader;
	loader.Bind(file.Size(), pvData);

	m_map_Proto.clear();
	m_map_KeywordToVnums.clear();
	m_map_WearAndTypeToVnums.clear();
	m_map_KeywordToConnectionType.clear();

	for (DWORD i = 0; i < loader.GetLineCount(); ++i)
	{
		CTokenVector tokens;
		if (!loader.SplitLineByTab(i, &tokens))
			continue;

		if (tokens.empty() || tokens[0].empty() || tokens[0][0] == '#')
			continue;

		if (tokens.size() < 22)
		{
			TraceError("LoadYinAndYangAttrProto invalid column count line:%u (%u/22)", i, tokens.size());
			continue;
		}

		TYinAndYangAttrProto proto;
		proto.dwVnum = atoi(tokens[0].c_str());
		proto.strName = tokens[1];
		proto.byAttrType = (tokens[2] == "YANG" ? TYPE_YANG : TYPE_YIN);

		const std::string& strKeyword = tokens[3];
		if (strKeyword == "SUPERNATURAL") proto.byAttrKeyword = KEYWORD_SUPERNATURAL;
		else if (strKeyword == "CAST") proto.byAttrKeyword = KEYWORD_CAST;
		else if (strKeyword == "PROVOKE") proto.byAttrKeyword = KEYWORD_PROVOKE;
		else if (strKeyword == "BACK_ATTACK") proto.byAttrKeyword = KEYWORD_BACK_ATTACK;
		else if (strKeyword == "TALISMAN") proto.byAttrKeyword = KEYWORD_TALISMAN;
		else if (strKeyword == "REVIVE") proto.byAttrKeyword = KEYWORD_REVIVE;
		else if (strKeyword == "SMITE") proto.byAttrKeyword = KEYWORD_SMITE;
		else if (strKeyword == "OCCUPY") proto.byAttrKeyword = KEYWORD_OCCUPY;
		else if (strKeyword == "BERSERK") proto.byAttrKeyword = KEYWORD_BERSERK;
		else proto.byAttrKeyword = KEYWORD_MAX;

		proto.strJob = tokens[4];
		proto.dwCoefficient = atoi(tokens[5].c_str());
		proto.dwCooltime = atoi(tokens[6].c_str());

		const std::string& strActivate = tokens[7];
		if (strActivate == "BUFF_SELF") proto.byActivateType = 0;
		else if (strActivate == "BUFF_PARTY") proto.byActivateType = 1;
		else if (strActivate == "BUFF_PC") proto.byActivateType = 2;
		else if (strActivate == "APPLY_POINT") proto.byActivateType = 3;
		else if (strActivate == "ATTACK") proto.byActivateType = 4;
		else if (strActivate == "ACTIVATE_YANG") proto.byActivateType = 5;
		else proto.byActivateType = 0;

		proto.strTargetMonster = tokens[8];

		for (int lv = 0; lv < 10; ++lv)
			proto.fLv[lv] = (float)atof(tokens[9 + lv].c_str());

		proto.fY = (float)atof(tokens[19].c_str());

		proto.dwApplicablePart = 0;
		const std::string& strParts = tokens[20];
		if (strParts.find("WEAR_BODY") != std::string::npos) proto.dwApplicablePart |= (1 << 0);
		if (strParts.find("WEAR_HEAD") != std::string::npos) proto.dwApplicablePart |= (1 << 1);
		if (strParts.find("WEAR_FOOTS") != std::string::npos) proto.dwApplicablePart |= (1 << 2);
		if (strParts.find("WEAR_WEAPON") != std::string::npos) proto.dwApplicablePart |= (1 << 4);
		if (strParts.find("WEAR_GLOVE") != std::string::npos) proto.dwApplicablePart |= (1 << 12);

		std::string strMap = tokens[21];
		size_t mpos = 0;
		while ((mpos = strMap.find(',')) != std::string::npos)
		{
			std::string token = strMap.substr(0, mpos);
			proto.vecObtainableMap.push_back(atoi(token.c_str()));
			strMap.erase(0, mpos + 1);
		}
		if (!strMap.empty())
			proto.vecObtainableMap.push_back(atoi(strMap.c_str()));

		m_map_Proto[proto.dwVnum] = proto;
		m_map_KeywordToVnums[proto.byAttrKeyword].push_back(proto.dwVnum);
		m_map_WearAndTypeToVnums[proto.dwApplicablePart][proto.byAttrType].push_back(proto.dwVnum);

		BYTE byConnType = CONNECTION_TYPE_NONE;
		if (proto.dwApplicablePart & (1 << 12))
			byConnType = CONNECTION_TYPE_GLOVE;
		else if (proto.dwApplicablePart & (1 << 4))
			byConnType = CONNECTION_TYPE_WEAPON;

		m_map_KeywordToConnectionType[proto.byAttrKeyword] = byConnType;
	}

	return true;
}

bool CPythonYinAndYangAttrManager::LoadYinAndYangAttrDesc(const char* c_szLocalePath)
{
	std::string strFileName;
	if (c_szLocalePath && *c_szLocalePath)
		strFileName = c_szLocalePath;
	else
		strFileName = LocaleService_GetLocalePath();

	strFileName += "/yin_and_yang_attr_desc.txt";

	CMappedFile file;
	LPCVOID pvData = NULL;

	if (!CEterPackManager::Instance().Get(file, strFileName.c_str(), &pvData))
	{
		TraceError("CPythonYinAndYangAttrManager::LoadYinAndYangAttrDesc: Failed to load %s", strFileName.c_str());
		return false;
	}

	CMemoryTextFileLoader loader;
	loader.Bind(file.Size(), pvData);

	m_map_Desc.clear();

	for (DWORD i = 0; i < loader.GetLineCount(); ++i)
	{
		CTokenVector tokens;
		if (!loader.SplitLineByTab(i, &tokens))
			continue;

		if (tokens.size() < 2)
			continue;

		DWORD dwVnum = atoi(tokens[0].c_str());
		std::string strDesc = tokens[1];
		for (size_t k = 2; k < tokens.size(); ++k)
		{
			strDesc += "\t";
			strDesc += tokens[k];
		}

		m_map_Desc[dwVnum] = strDesc;
	}

	return true;
}

std::string CPythonYinAndYangAttrManager::FormatDescString(const std::string& strFormat, float fVal, float fY) const
{
	std::string str = strFormat;
	char buf[64];

	snprintf(buf, sizeof(buf), "%d", (int)fY);
	StringReplace(str, "%yd", buf);

	snprintf(buf, sizeof(buf), "%.1f", fY);
	StringReplace(str, "%yf", buf);

	snprintf(buf, sizeof(buf), "%d", (int)fVal);
	StringReplace(str, "%xd", buf);

	snprintf(buf, sizeof(buf), "%.1f", fVal);
	StringReplace(str, "%xf", buf);

	snprintf(buf, sizeof(buf), "%d", (int)fVal);
	StringReplace(str, "%d", buf);

	snprintf(buf, sizeof(buf), "%.1f", fVal);
	StringReplace(str, "%f", buf);

	StringReplace(str, "\\n", "\n");

	return str;
}

std::string CPythonYinAndYangAttrManager::FormatDescMinMaxString(const std::string& strFormat, float fMin, float fMax, float fY) const
{
	std::string str = strFormat;
	char buf[64];

	snprintf(buf, sizeof(buf), "%d", (int)fY);
	StringReplace(str, "%yd", buf);

	snprintf(buf, sizeof(buf), "%.1f", fY);
	StringReplace(str, "%yf", buf);

	snprintf(buf, sizeof(buf), "%d ~ %d", (int)fMin, (int)fMax);
	StringReplace(str, "%xd", buf);

	snprintf(buf, sizeof(buf), "%.1f ~ %.1f", fMin, fMax);
	StringReplace(str, "%xf", buf);

	snprintf(buf, sizeof(buf), "%d ~ %d", (int)fMin, (int)fMax);
	StringReplace(str, "%d", buf);

	snprintf(buf, sizeof(buf), "%.1f ~ %.1f", fMin, fMax);
	StringReplace(str, "%f", buf);

	StringReplace(str, "\\n", "\n");

	return str;
}

bool CPythonYinAndYangAttrManager::GetYinAndYangAttrDescText(DWORD dwVnum, BYTE byStep, std::vector<std::string>& rVecLines)
{
	rVecLines.clear();

	const TYinAndYangAttrProto* pProto = GetProto(dwVnum);
	if (!pProto)
		return false;

	std::map<DWORD, std::string>::const_iterator it = m_map_Desc.find(dwVnum);
	if (it == m_map_Desc.end())
		return false;

	if (byStep < 1 || byStep > 10)
		byStep = 1;

	float fVal = pProto->fLv[byStep - 1];
	std::string strFormatted = FormatDescString(it->second, fVal, pProto->fY);
	SplitLines(strFormatted, rVecLines);

	return true;
}

bool CPythonYinAndYangAttrManager::GetYinAndYangAttrDescTextMinMax(DWORD dwVnum, std::vector<std::string>& rVecLines)
{
	rVecLines.clear();

	const TYinAndYangAttrProto* pProto = GetProto(dwVnum);
	if (!pProto)
		return false;

	std::map<DWORD, std::string>::const_iterator it = m_map_Desc.find(dwVnum);
	if (it == m_map_Desc.end())
		return false;

	float fMin = pProto->fLv[0];
	float fMax = pProto->fLv[9];
	std::string strFormatted = FormatDescMinMaxString(it->second, fMin, fMax, pProto->fY);
	SplitLines(strFormatted, rVecLines);

	return true;
}

const CPythonYinAndYangAttrManager::TYinAndYangAttrProto* CPythonYinAndYangAttrManager::GetProto(DWORD dwVnum) const
{
	std::map<DWORD, TYinAndYangAttrProto>::const_iterator it = m_map_Proto.find(dwVnum);
	if (it != m_map_Proto.end())
		return &it->second;

	return NULL;
}

BYTE CPythonYinAndYangAttrManager::GetYinAndYangAttrType(DWORD dwVnum) const
{
	const TYinAndYangAttrProto* pProto = GetProto(dwVnum);
	if (pProto)
		return pProto->byAttrType;

	return TYPE_YIN;
}

BYTE CPythonYinAndYangAttrManager::GetYinAndYangAttrKeyword(DWORD dwVnum) const
{
	const TYinAndYangAttrProto* pProto = GetProto(dwVnum);
	if (pProto)
		return pProto->byAttrKeyword;

	return KEYWORD_MAX;
}

bool CPythonYinAndYangAttrManager::IsYinAndYangAttrAttachablePart(DWORD dwVnum, DWORD dwWearFlag) const
{
	const TYinAndYangAttrProto* pProto = GetProto(dwVnum);
	if (pProto)
		return (pProto->dwApplicablePart & dwWearFlag) != 0;

	return false;
}

int CPythonYinAndYangAttrManager::GetNeedGoldToAttachAttr(DWORD dwVnum, BYTE byStep, WORD wCell) const
{
	const TYinAndYangAttrProto* pProto = GetProto(dwVnum);
	if (!pProto)
		return 0;

	const TItemData* pItem = CPythonPlayer::Instance().GetItemData(TItemPos(INVENTORY, wCell));
	if (!pItem)
		return 0;

	CItemData* pItemData = NULL;
	if (!CItemManager::Instance().GetItemDataPointer(pItem->vnum, &pItemData))
		return 0;

	if (pProto->dwCoefficient == 0)
		return 0;

	double dLevel = (double)pItemData->GetRefine();
	double dExponent = (1.0 + (dLevel / 10.0) * (dLevel / 10.0)) * ((double)byStep / (double)pProto->dwCoefficient);
	double dResult = pow(1.2, dExponent) + 1000000.0;
	int nGold = (int)dResult;
	return nGold - (nGold % 1000);
}

bool CPythonYinAndYangAttrManager::IsAttrAttachableItemType(DWORD dwItemVnum) const
{
	CItemData* pItemData = NULL;
	if (!CItemManager::Instance().GetItemDataPointer(dwItemVnum, &pItemData))
		return false;

	CItemData::TItemLimit itemLimit;
	for (int i = 0; i < CItemData::ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (pItemData->GetLimit(i, &itemLimit) && itemLimit.bType == CItemData::LIMIT_LEVEL)
		{
			if (itemLimit.lValue < YIN_AND_YANG_ATTACHABLE_ITEM_LEVEL_MIN)
				return false;
		}
	}

	BYTE byType = pItemData->GetType();
	if (byType == CItemData::ITEM_TYPE_WEAPON || byType == CItemData::ITEM_TYPE_ARMOR)
		return true;

	return false;
}

bool CPythonYinAndYangAttrManager::IsAttrAttachableToItem(DWORD dwVnum, DWORD dwItemVnum) const
{
	const TYinAndYangAttrProto* pProto = GetProto(dwVnum);
	if (!pProto)
		return false;

	CItemData* pItemData = NULL;
	if (!CItemManager::Instance().GetItemDataPointer(dwItemVnum, &pItemData))
		return false;

	return (pItemData->GetWearFlags() & pProto->dwApplicablePart) != 0;
}

bool CPythonYinAndYangAttrManager::IsYinAndYangAttrConnected(DWORD dwVnum1, DWORD dwVnum2) const
{
	if (dwVnum1 == 0 || dwVnum2 == 0)
		return false;

	const TYinAndYangAttrProto* pProto1 = GetProto(dwVnum1);
	if (!pProto1)
		return false;

	const TYinAndYangAttrProto* pProto2 = GetProto(dwVnum2);
	if (!pProto2)
		return false;

	if (pProto1->byAttrType == pProto2->byAttrType)
		return false;

	if (pProto1->byAttrKeyword != pProto2->byAttrKeyword)
		return false;

	return true;
}

BYTE CPythonYinAndYangAttrManager::GetConnectionTypeByKeyword(BYTE byKeyword) const
{
	std::map<BYTE, BYTE>::const_iterator it = m_map_KeywordToConnectionType.find(byKeyword);
	if (it != m_map_KeywordToConnectionType.end())
		return it->second;

	return CONNECTION_TYPE_NONE;
}

const std::vector<DWORD>* CPythonYinAndYangAttrManager::GetVnumsByKeyword(BYTE byKeyword) const
{
	std::map<BYTE, std::vector<DWORD>>::const_iterator it = m_map_KeywordToVnums.find(byKeyword);
	if (it != m_map_KeywordToVnums.end())
		return &it->second;

	return NULL;
}

const std::vector<DWORD>* CPythonYinAndYangAttrManager::GetVnumsByWearAndType(DWORD dwWearFlag, BYTE byAttrType) const
{
	std::map<DWORD, std::map<BYTE, std::vector<DWORD>>>::const_iterator it = m_map_WearAndTypeToVnums.find(dwWearFlag);
	if (it != m_map_WearAndTypeToVnums.end())
	{
		std::map<BYTE, std::vector<DWORD>>::const_iterator it2 = it->second.find(byAttrType);
		if (it2 != it->second.end())
			return &it2->second;
	}

	return NULL;
}

const std::vector<DWORD>* CPythonYinAndYangAttrManager::GetObtainableMap(DWORD dwVnum) const
{
	const TYinAndYangAttrProto* pProto = GetProto(dwVnum);
	if (pProto)
		return &pProto->vecObtainableMap;

	return NULL;
}

void CPythonYinAndYangAttrManager::SetSystemEnable(bool bEnable)
{
	m_bSystemEnable = bEnable;

	if (m_poInventoryUIHandler && !bEnable)
	{
		PyCallClassMemberFunc(m_poInventoryUIHandler, "Hide", Py_BuildValue("()"));
	}

	if (m_poHarmonyUIHandler)
	{
		PyCallClassMemberFunc(m_poHarmonyUIHandler, "SetSystemEnable", Py_BuildValue("(b)", bEnable));
	}
}

void CPythonYinAndYangAttrManager::RefreshShowConnectedItemsEquipment()
{
	if (m_poHarmonyUIHandler)
		PyCallClassMemberFunc(m_poHarmonyUIHandler, "ShowConnectedItemsEquipment", Py_BuildValue("()"));
}

bool CPythonYinAndYangAttrManager::SendCloseInventory()
{
	if (!IsSystemEnabled())
		return false;

	TPacketCGYinAndYangAttr kPacket;
	kPacket.bHeader = HEADER_CG_YIN_YANG_ATTR;
	kPacket.wSize = sizeof(kPacket);
	kPacket.bSubHeader = SUBHEADER_CG_YIN_YANG_ATTR_CLOSE;

	return CPythonNetworkStream::Instance().Send(sizeof(kPacket), &kPacket);
}

bool CPythonYinAndYangAttrManager::SendRequestInventoryMoveItem(BYTE bFromPos, BYTE bToPos)
{
	if (!IsSystemEnabled())
		return false;

	TPacketCGYinAndYangAttrMove kPacket;
	kPacket.bHeader = HEADER_CG_YIN_YANG_ATTR;
	kPacket.wSize = sizeof(kPacket);
	kPacket.bSubHeader = SUBHEADER_CG_YIN_YANG_ATTR_MOVE;
	kPacket.bFromPos = bFromPos;
	kPacket.bToPos = bToPos;

	return CPythonNetworkStream::Instance().Send(sizeof(kPacket), &kPacket);
}

bool CPythonYinAndYangAttrManager::SendRequestInventoryDeleteItem(BYTE bPos)
{
	if (!IsSystemEnabled())
		return false;

	TPacketCGYinAndYangAttrDelete kPacket;
	kPacket.bHeader = HEADER_CG_YIN_YANG_ATTR;
	kPacket.wSize = sizeof(kPacket);
	kPacket.bSubHeader = SUBHEADER_CG_YIN_YANG_ATTR_DELETE;
	kPacket.bPos = bPos;

	return CPythonNetworkStream::Instance().Send(sizeof(kPacket), &kPacket);
}

bool CPythonYinAndYangAttrManager::SendRequestRegisterRecentlyGainedItem()
{
	if (!IsSystemEnabled())
		return false;

	TPacketCGYinAndYangAttr kPacket;
	kPacket.bHeader = HEADER_CG_YIN_YANG_ATTR;
	kPacket.wSize = sizeof(kPacket);
	kPacket.bSubHeader = SUBHEADER_CG_YIN_YANG_ATTR_REGISTER_RECENTLY_GAINED;

	return CPythonNetworkStream::Instance().Send(sizeof(kPacket), &kPacket);
}

bool CPythonYinAndYangAttrManager::SendRequestAttachAttrToItem(BYTE bFromPos, BYTE bToPos)
{
	if (!IsSystemEnabled())
		return false;

	TPacketCGYinAndYangAttrAttach kPacket;
	kPacket.bHeader = HEADER_CG_YIN_YANG_ATTR;
	kPacket.wSize = sizeof(kPacket);
	kPacket.bSubHeader = SUBHEADER_CG_YIN_YANG_ATTR_ATTACH;
	kPacket.bFromPos = bFromPos;
	kPacket.bToPos = bToPos;

	return CPythonNetworkStream::Instance().Send(sizeof(kPacket), &kPacket);
}

bool CPythonYinAndYangAttrManager::SendRequestInventoryExpansion()
{
	if (!IsSystemEnabled())
		return false;

	TPacketCGYinAndYangAttr kPacket;
	kPacket.bHeader = HEADER_CG_YIN_YANG_ATTR;
	kPacket.wSize = sizeof(kPacket);
	kPacket.bSubHeader = SUBHEADER_CG_YIN_YANG_ATTR_EXPANSION;

	return CPythonNetworkStream::Instance().Send(sizeof(kPacket), &kPacket);
}

bool CPythonYinAndYangAttrManager::SendRequestEnableYinAndYangAttr(bool bEnable)
{
	if (!IsSystemEnabled())
		return false;

	TPacketCGYinAndYangAttr kPacket;
	kPacket.bHeader = HEADER_CG_YIN_YANG_ATTR;
	kPacket.wSize = sizeof(kPacket);
	kPacket.bSubHeader = (bEnable ? SUBHEADER_CG_YIN_YANG_ATTR_ENABLE : SUBHEADER_CG_YIN_YANG_ATTR_DISABLE);

	return CPythonNetworkStream::Instance().Send(sizeof(kPacket), &kPacket);
}

bool CPythonYinAndYangAttrManager::RecvYinAndYangAttrPacket()
{
	TPacketGCYinAndYangAttr kHeader;
	if (!CPythonNetworkStream::Instance().Recv(sizeof(kHeader), &kHeader))
		return false;

	switch (kHeader.bSubHeader)
	{
	case SUBHEADER_GC_YIN_YANG_ATTR_OPEN:
		if (m_poInventoryUIHandler)
			PyCallClassMemberFunc(m_poInventoryUIHandler, "Open", Py_BuildValue("()"));
		break;

	case SUBHEADER_GC_YIN_YANG_ATTR_SET_DATA:
		{
			BYTE bSlotCount = 0;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(bSlotCount), &bSlotCount))
				return false;

			PyObject* poDict = PyDict_New();
			int iItemCount = (kHeader.wSize - sizeof(kHeader) - sizeof(bSlotCount)) / sizeof(TPacketGCYinAndYangAttrItemData);
			for (int i = 0; i < iItemCount; ++i)
			{
				TPacketGCYinAndYangAttrItemData kItem;
				if (!CPythonNetworkStream::Instance().Recv(sizeof(kItem), &kItem))
				{
					Py_DECREF(poDict);
					return false;
				}

				PyObject* poTuple = Py_BuildValue("(iii)", kItem.dwAttrVal, kItem.byAttrType, kItem.dwRemainTime);
				PyDict_SetItem(poDict, Py_BuildValue("i", kItem.bSlotPos), poTuple);
				Py_DECREF(poTuple);
			}

			if (m_poInventoryUIHandler)
				PyCallClassMemberFunc(m_poInventoryUIHandler, "SetData", Py_BuildValue("(iO)", bSlotCount, poDict));

			Py_DECREF(poDict);
		}
		break;

	case SUBHEADER_GC_YIN_YANG_ATTR_DELETE:
		{
			BYTE bSlotPos = 0;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(bSlotPos), &bSlotPos))
				return false;

			if (m_poInventoryUIHandler)
				PyCallClassMemberFunc(m_poInventoryUIHandler, "OnDeleteItem", Py_BuildValue("(i)", bSlotPos));
		}
		break;

	case SUBHEADER_GC_YIN_YANG_ATTR_UPDATE:
		{
			TPacketGCYinAndYangAttrUpdate kUpdate;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(kUpdate), &kUpdate))
				return false;

			if (m_poInventoryUIHandler)
				PyCallClassMemberFunc(m_poInventoryUIHandler, "OnUpdateItem", Py_BuildValue("(iiii)", kUpdate.bSlotPos, kUpdate.dwAttrVal, kUpdate.byAttrType, kUpdate.dwRemainTime));
		}
		break;

	case SUBHEADER_GC_YIN_YANG_ATTR_MOVE:
		{
			TPacketGCYinAndYangAttrMove kMove;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(kMove), &kMove))
				return false;

			if (m_poInventoryUIHandler)
				PyCallClassMemberFunc(m_poInventoryUIHandler, "OnMoveItem", Py_BuildValue("(ii)", kMove.bFromPos, kMove.bToPos));
		}
		break;

	case SUBHEADER_GC_YIN_YANG_ATTR_EXPANSION:
		{
			WORD wExpansionCount = 0;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(wExpansionCount), &wExpansionCount))
				return false;

			m_dwExpansionCountByItem = wExpansionCount;

			if (m_poInventoryUIHandler)
				PyCallClassMemberFunc(m_poInventoryUIHandler, "UpdateIventoryExpansionCount", Py_BuildValue("(i)", wExpansionCount));
		}
		break;

	case SUBHEADER_GC_YIN_YANG_ATTR_ATTR_ACTIVATED:
		{
			BYTE bAttrType = 0;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(bAttrType), &bAttrType))
				return false;

			if (m_poHarmonyUIHandler)
				PyCallClassMemberFunc(m_poHarmonyUIHandler, "OnAttrActivated", Py_BuildValue("(i)", bAttrType));
		}
		break;

	case SUBHEADER_GC_YIN_YANG_ATTR_ENABLE:
		{
			DWORD dwRemainTime = 0;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(dwRemainTime), &dwRemainTime))
				return false;

			if (m_poHarmonyUIHandler)
				PyCallClassMemberFunc(m_poHarmonyUIHandler, "SetEnableOrDisable", Py_BuildValue("(bi)", 1, dwRemainTime));
		}
		break;

	case SUBHEADER_GC_YIN_YANG_ATTR_DISABLE:
		{
			DWORD dwCoolTime = 0;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(dwCoolTime), &dwCoolTime))
				return false;

			if (m_poHarmonyUIHandler)
				PyCallClassMemberFunc(m_poHarmonyUIHandler, "SetEnableOrDisable", Py_BuildValue("(bi)", 0, dwCoolTime));
		}
		break;

	default:
		return false;
	}

	return true;
}

// -----------------------------------------------------------------------
// Python Module Wrapper
// -----------------------------------------------------------------------

PyObject* yinAndYangAttrSetInventoryUIHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler = NULL;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonYinAndYangAttrManager::Instance().SetInventoryUIHandler(poHandler);
	return Py_BuildNone();
}

PyObject* yinAndYangAttrSetHarmonyUIHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler = NULL;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonYinAndYangAttrManager::Instance().SetHarmonyUIHandler(poHandler);
	return Py_BuildNone();
}

PyObject* yinAndYangAttrClear(PyObject* poSelf, PyObject* poArgs)
{
	CPythonYinAndYangAttrManager::Instance().Clear();
	return Py_BuildNone();
}

PyObject* yinAndYangAttrGetYinAndYangAttrDescText(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	BYTE byStep = 0;
	if (!PyTuple_GetByte(poArgs, 1, &byStep))
		return Py_BuildException();

	std::vector<std::string> vecLines;
	if (!CPythonYinAndYangAttrManager::Instance().GetYinAndYangAttrDescText(dwVnum, byStep, vecLines))
		return Py_BuildNone();

	PyObject* poList = PyList_New(0);
	for (size_t i = 0; i < vecLines.size(); ++i)
	{
		PyList_Append(poList, Py_BuildValue("s", vecLines[i].c_str()));
	}

	return poList;
}

PyObject* yinAndYangAttrGetYinAndYangAttrDescTextMinMax(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	std::vector<std::string> vecLines;
	if (!CPythonYinAndYangAttrManager::Instance().GetYinAndYangAttrDescTextMinMax(dwVnum, vecLines))
		return Py_BuildNone();

	PyObject* poList = PyList_New(0);
	for (size_t i = 0; i < vecLines.size(); ++i)
	{
		PyList_Append(poList, Py_BuildValue("s", vecLines[i].c_str()));
	}

	return poList;
}

PyObject* yinAndYangAttrGetYinAndYangAttrType(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	const CPythonYinAndYangAttrManager::TYinAndYangAttrProto* pProto = CPythonYinAndYangAttrManager::Instance().GetProto(dwVnum);
	if (!pProto)
		return Py_BuildNone();

	return Py_BuildValue("i", pProto->byAttrType);
}

PyObject* yinAndYangAttrGetYinAndYangAttrKeyword(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	const CPythonYinAndYangAttrManager::TYinAndYangAttrProto* pProto = CPythonYinAndYangAttrManager::Instance().GetProto(dwVnum);
	if (!pProto)
		return Py_BuildNone();

	return Py_BuildValue("i", pProto->byAttrKeyword);
}

PyObject* yinAndYangAttrGetYinAndYangAttrApplicableWearFlag(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	const CPythonYinAndYangAttrManager::TYinAndYangAttrProto* pProto = CPythonYinAndYangAttrManager::Instance().GetProto(dwVnum);
	if (!pProto)
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", pProto->dwApplicablePart);
}

PyObject* yinAndYangAttrGetYinAndYangAttrFormattedText(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	BYTE byStep = 0;
	if (!PyTuple_GetByte(poArgs, 1, &byStep))
		return Py_BuildException();

	std::vector<std::string> vecLines;
	if (!CPythonYinAndYangAttrManager::Instance().GetYinAndYangAttrDescText(dwVnum, byStep, vecLines))
		return Py_BuildValue("s", "");

	std::string strResult;
	for (size_t i = 0; i < vecLines.size(); ++i)
	{
		if (i > 0)
			strResult += "\n";
		strResult += vecLines[i];
	}

	return Py_BuildValue("s", strResult.c_str());
}

PyObject* yinAndYangAttrIsYinAndYangAttrAttachablePart(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	DWORD dwWearFlag = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 1, &dwWearFlag))
		return Py_BuildException();

	bool bAttachable = CPythonYinAndYangAttrManager::Instance().IsYinAndYangAttrAttachablePart(dwVnum, dwWearFlag);
	return Py_BuildValue("b", bAttachable);
}

PyObject* yinAndYangAttrGetNeedGoldToAttachAttr(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	int iStep = 0;
	if (!PyTuple_GetInteger(poArgs, 1, &iStep))
		return Py_BuildException();

	int iCell = 0;
	if (!PyTuple_GetInteger(poArgs, 2, &iCell))
		return Py_BuildException();

	int nGold = CPythonYinAndYangAttrManager::Instance().GetNeedGoldToAttachAttr(dwVnum, (BYTE)iStep, (WORD)iCell);
	return Py_BuildValue("i", nGold);
}

PyObject* yinAndYangAttrIsAttrAttachableItemType(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwItemVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwItemVnum))
		return Py_BuildException();

	bool bResult = CPythonYinAndYangAttrManager::Instance().IsAttrAttachableItemType(dwItemVnum);
	return Py_BuildValue("b", bResult);
}

PyObject* yinAndYangAttrIsAttrAttachableToItem(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	DWORD dwItemVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 1, &dwItemVnum))
		return Py_BuildException();

	bool bResult = CPythonYinAndYangAttrManager::Instance().IsAttrAttachableToItem(dwVnum, dwItemVnum);
	return Py_BuildValue("b", bResult);
}

PyObject* yinAndYangAttrRefreshShowConnectedItemsEquipment(PyObject* poSelf, PyObject* poArgs)
{
	CPythonYinAndYangAttrManager::Instance().RefreshShowConnectedItemsEquipment();
	return Py_BuildNone();
}

PyObject* yinAndYangAttrGetExpansionCountByItem(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonYinAndYangAttrManager::Instance().GetExpansionCountByItem());
}

PyObject* yinAndYangAttrGetExpansionCountByQuest(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonYinAndYangAttrManager::Instance().GetExpansionCountByQuest());
}

PyObject* yinAndYangAttrIsYinAndYangAttrConnected(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum1 = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum1))
		return Py_BuildException();

	DWORD dwVnum2 = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 1, &dwVnum2))
		return Py_BuildException();

	bool bConnected = CPythonYinAndYangAttrManager::Instance().IsYinAndYangAttrConnected(dwVnum1, dwVnum2);
	return Py_BuildValue("b", bConnected);
}

PyObject* yinAndYangAttrGetYinAndYangAttrVnumsByKeyword(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byKeyword = 0;
	if (!PyTuple_GetByte(poArgs, 0, &byKeyword))
		return Py_BuildException();

	const std::vector<DWORD>* pVec = CPythonYinAndYangAttrManager::Instance().GetVnumsByKeyword(byKeyword);
	PyObject* poList = PyList_New(0);
	if (pVec)
	{
		for (size_t i = 0; i < pVec->size(); ++i)
		{
			PyList_Append(poList, Py_BuildValue("i", (*pVec)[i]));
		}
	}

	return poList;
}

PyObject* yinAndYangAttrGetYinAndYangAttrVnumsByWearAndType(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwWearFlag = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwWearFlag))
		return Py_BuildException();

	BYTE byAttrType = 0;
	if (!PyTuple_GetByte(poArgs, 1, &byAttrType))
		return Py_BuildException();

	const std::vector<DWORD>* pVec = CPythonYinAndYangAttrManager::Instance().GetVnumsByWearAndType(dwWearFlag, byAttrType);
	PyObject* poList = PyList_New(0);
	if (pVec)
	{
		for (size_t i = 0; i < pVec->size(); ++i)
		{
			PyList_Append(poList, Py_BuildValue("i", (*pVec)[i]));
		}
	}

	return poList;
}

PyObject* yinAndYangAttrGetConnectionTypeByKeyword(PyObject* poSelf, PyObject* poArgs)
{
	BYTE byKeyword = 0;
	if (!PyTuple_GetByte(poArgs, 0, &byKeyword))
		return Py_BuildException();

	BYTE byType = CPythonYinAndYangAttrManager::Instance().GetConnectionTypeByKeyword(byKeyword);
	return Py_BuildValue("i", byType);
}

PyObject* yinAndYangAttrGetYinAndYangAttrObtainableMap(PyObject* poSelf, PyObject* poArgs)
{
	DWORD dwVnum = 0;
	if (!PyTuple_GetUnsignedLong(poArgs, 0, &dwVnum))
		return Py_BuildException();

	const std::vector<DWORD>* pVec = CPythonYinAndYangAttrManager::Instance().GetObtainableMap(dwVnum);
	PyObject* poList = PyList_New(0);
	if (pVec)
	{
		for (size_t i = 0; i < pVec->size(); ++i)
		{
			PyList_Append(poList, Py_BuildValue("i", (*pVec)[i]));
		}
	}

	return poList;
}

PyObject* yinAndYangAttrSendCloseInventory(PyObject* poSelf, PyObject* poArgs)
{
	CPythonYinAndYangAttrManager::Instance().SendCloseInventory();
	return Py_BuildNone();
}

PyObject* yinAndYangAttrSendRequestInventoryMoveItem(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bFrom = 0;
	if (!PyTuple_GetByte(poArgs, 0, &bFrom))
		return Py_BuildException();

	BYTE bTo = 0;
	if (!PyTuple_GetByte(poArgs, 1, &bTo))
		return Py_BuildException();

	CPythonYinAndYangAttrManager::Instance().SendRequestInventoryMoveItem(bFrom, bTo);
	return Py_BuildNone();
}

PyObject* yinAndYangAttrSendRequestInventoryDeleteItem(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bPos = 0;
	if (!PyTuple_GetByte(poArgs, 0, &bPos))
		return Py_BuildException();

	CPythonYinAndYangAttrManager::Instance().SendRequestInventoryDeleteItem(bPos);
	return Py_BuildNone();
}

PyObject* yinAndYangAttrSendRequestRegisterRecentlyGainedItem(PyObject* poSelf, PyObject* poArgs)
{
	CPythonYinAndYangAttrManager::Instance().SendRequestRegisterRecentlyGainedItem();
	return Py_BuildNone();
}

PyObject* yinAndYangAttrSendRequestAttachAttrToItem(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bFrom = 0;
	if (!PyTuple_GetByte(poArgs, 0, &bFrom))
		return Py_BuildException();

	BYTE bTo = 0;
	if (!PyTuple_GetByte(poArgs, 1, &bTo))
		return Py_BuildException();

	CPythonYinAndYangAttrManager::Instance().SendRequestAttachAttrToItem(bFrom, bTo);
	return Py_BuildNone();
}

PyObject* yinAndYangAttrSendRequestInventoryExpansion(PyObject* poSelf, PyObject* poArgs)
{
	CPythonYinAndYangAttrManager::Instance().SendRequestInventoryExpansion();
	return Py_BuildNone();
}

PyObject* yinAndYangAttrSendRequestEnableYinAndYangAttr(PyObject* poSelf, PyObject* poArgs)
{
	bool bEnable = false;
	if (!PyTuple_GetBoolean(poArgs, 0, &bEnable))
		return Py_BuildException();

	CPythonYinAndYangAttrManager::Instance().SendRequestEnableYinAndYangAttr(bEnable);
	return Py_BuildNone();
}

PyObject* yinAndYangAttrSetSystemEnable(PyObject* poSelf, PyObject* poArgs)
{
	bool bEnable = false;
	if (!PyTuple_GetBoolean(poArgs, 0, &bEnable))
		return Py_BuildException();

	CPythonYinAndYangAttrManager::Instance().SetSystemEnable(bEnable);
	return Py_BuildNone();
}

PyObject* yinAndYangAttrIsSystemEnabled(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonYinAndYangAttrManager::Instance().IsSystemEnabled());
}

void inityinAndYangAttr()
{
	static PyMethodDef s_methods[] =
	{
		{ "SetInventoryUIHandler",					yinAndYangAttrSetInventoryUIHandler,					METH_VARARGS },
		{ "SetYinAndYangAttrInventoryUIHandler",	yinAndYangAttrSetInventoryUIHandler,					METH_VARARGS },
		{ "SetHarmonyUIHandler",					yinAndYangAttrSetHarmonyUIHandler,						METH_VARARGS },
		{ "SetYinAndYangAttrHarmonyUIHandler",		yinAndYangAttrSetHarmonyUIHandler,						METH_VARARGS },
		{ "Clear",									yinAndYangAttrClear,									METH_VARARGS },
		{ "GetYinAndYangAttrDescText",				yinAndYangAttrGetYinAndYangAttrDescText,				METH_VARARGS },
		{ "GetYinAndYangAttrDescTextMinMax",		yinAndYangAttrGetYinAndYangAttrDescTextMinMax,			METH_VARARGS },
		{ "GetYinAndYangAttrType",					yinAndYangAttrGetYinAndYangAttrType,					METH_VARARGS },
		{ "GetYinAndYangAttrKeyword",				yinAndYangAttrGetYinAndYangAttrKeyword,					METH_VARARGS },
		{ "GetYinAndYangAttrApplicableWearFlag",	yinAndYangAttrGetYinAndYangAttrApplicableWearFlag,		METH_VARARGS },
		{ "GetYinAndYangAttrFormattedText",			yinAndYangAttrGetYinAndYangAttrFormattedText,			METH_VARARGS },
		{ "IsYinAndYangAttrAttachablePart",			yinAndYangAttrIsYinAndYangAttrAttachablePart,			METH_VARARGS },
		{ "GetNeedGoldToAttachAttr",				yinAndYangAttrGetNeedGoldToAttachAttr,					METH_VARARGS },
		{ "IsAttrAttachableItemType",				yinAndYangAttrIsAttrAttachableItemType,					METH_VARARGS },
		{ "IsAttrAttachableToItem",					yinAndYangAttrIsAttrAttachableToItem,					METH_VARARGS },
		{ "RefreshShowConnectedItemsEquipment",		yinAndYangAttrRefreshShowConnectedItemsEquipment,		METH_VARARGS },
		{ "GetExpansionCountByItem",				yinAndYangAttrGetExpansionCountByItem,					METH_VARARGS },
		{ "GetExpansionCountByQuest",				yinAndYangAttrGetExpansionCountByQuest,					METH_VARARGS },
		{ "IsYinAndYangAttrConnected",				yinAndYangAttrIsYinAndYangAttrConnected,				METH_VARARGS },
		{ "GetYinAndYangAttrVnumsByKeyword",		yinAndYangAttrGetYinAndYangAttrVnumsByKeyword,			METH_VARARGS },
		{ "GetYinAndYangAttrVnumsByWearAndType",	yinAndYangAttrGetYinAndYangAttrVnumsByWearAndType,		METH_VARARGS },
		{ "GetConnectionTypeByKeyword",				yinAndYangAttrGetConnectionTypeByKeyword,				METH_VARARGS },
		{ "GetYinAndYangAttrObtainableMap",			yinAndYangAttrGetYinAndYangAttrObtainableMap,			METH_VARARGS },
		{ "SendCloseInventory",						yinAndYangAttrSendCloseInventory,						METH_VARARGS },
		{ "SendRequestInventoryMoveItem",			yinAndYangAttrSendRequestInventoryMoveItem,				METH_VARARGS },
		{ "SendRequestInventoryDeleteItem",			yinAndYangAttrSendRequestInventoryDeleteItem,			METH_VARARGS },
		{ "SendRequestRegisterRecentlyGainedItem",	yinAndYangAttrSendRequestRegisterRecentlyGainedItem,	METH_VARARGS },
		{ "SendRequestAttachAttrToItem",			yinAndYangAttrSendRequestAttachAttrToItem,				METH_VARARGS },
		{ "SendRequestInventoryExpansion",			yinAndYangAttrSendRequestInventoryExpansion,			METH_VARARGS },
		{ "SendRequestEnableYinAndYangAttr",		yinAndYangAttrSendRequestEnableYinAndYangAttr,			METH_VARARGS },
		{ "SendRequestSetEnableOrDisable",			yinAndYangAttrSendRequestEnableYinAndYangAttr,			METH_VARARGS },
		{ "SetSystemEnable",						yinAndYangAttrSetSystemEnable,							METH_VARARGS },
		{ "IsSystemEnabled",						yinAndYangAttrIsSystemEnabled,							METH_VARARGS },
		{ NULL,										NULL,													0 }
	};

	PyObject* poModule = Py_InitModule("yinAndYangAttr", s_methods);

	PyModule_AddIntConstant(poModule, "TYPE_YIN", CPythonYinAndYangAttrManager::TYPE_YIN);
	PyModule_AddIntConstant(poModule, "TYPE_YANG", CPythonYinAndYangAttrManager::TYPE_YANG);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTR_TYPE_YIN", CPythonYinAndYangAttrManager::TYPE_YIN);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTR_TYPE_YANG", CPythonYinAndYangAttrManager::TYPE_YANG);

	PyModule_AddIntConstant(poModule, "KEYWORD_SUPERNATURAL", CPythonYinAndYangAttrManager::KEYWORD_SUPERNATURAL);
	PyModule_AddIntConstant(poModule, "KEYWORD_CAST", CPythonYinAndYangAttrManager::KEYWORD_CAST);
	PyModule_AddIntConstant(poModule, "KEYWORD_PROVOKE", CPythonYinAndYangAttrManager::KEYWORD_PROVOKE);
	PyModule_AddIntConstant(poModule, "KEYWORD_BACK_ATTACK", CPythonYinAndYangAttrManager::KEYWORD_BACK_ATTACK);
	PyModule_AddIntConstant(poModule, "KEYWORD_TALISMAN", CPythonYinAndYangAttrManager::KEYWORD_TALISMAN);
	PyModule_AddIntConstant(poModule, "KEYWORD_REVIVE", CPythonYinAndYangAttrManager::KEYWORD_REVIVE);
	PyModule_AddIntConstant(poModule, "KEYWORD_SMITE", CPythonYinAndYangAttrManager::KEYWORD_SMITE);
	PyModule_AddIntConstant(poModule, "KEYWORD_OCCUPY", CPythonYinAndYangAttrManager::KEYWORD_OCCUPY);
	PyModule_AddIntConstant(poModule, "KEYWORD_BERSERK", CPythonYinAndYangAttrManager::KEYWORD_BERSERK);
	PyModule_AddIntConstant(poModule, "KEYWORD_MAX", CPythonYinAndYangAttrManager::KEYWORD_MAX);

	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTR_KEYWORD_SUPERNATURAL", CPythonYinAndYangAttrManager::KEYWORD_SUPERNATURAL);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTR_KEYWORD_CAST", CPythonYinAndYangAttrManager::KEYWORD_CAST);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTR_KEYWORD_PROVOKE", CPythonYinAndYangAttrManager::KEYWORD_PROVOKE);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTR_KEYWORD_BACK_ATTACK", CPythonYinAndYangAttrManager::KEYWORD_BACK_ATTACK);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTR_KEYWORD_TALISMAN", CPythonYinAndYangAttrManager::KEYWORD_TALISMAN);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTR_KEYWORD_REVIVE", CPythonYinAndYangAttrManager::KEYWORD_REVIVE);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTR_KEYWORD_SMITE", CPythonYinAndYangAttrManager::KEYWORD_SMITE);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTR_KEYWORD_OCCUPY", CPythonYinAndYangAttrManager::KEYWORD_OCCUPY);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTR_KEYWORD_BERSERK", CPythonYinAndYangAttrManager::KEYWORD_BERSERK);

	PyModule_AddIntConstant(poModule, "WEAR_FLAG_BODY", 1);
	PyModule_AddIntConstant(poModule, "WEAR_FLAG_HEAD", 2);
	PyModule_AddIntConstant(poModule, "WEAR_FLAG_FOOTS", 4);
	PyModule_AddIntConstant(poModule, "WEAR_FLAG_WEAPON", 16);
	PyModule_AddIntConstant(poModule, "WEAR_FLAG_GLOVE", 4096);

	PyModule_AddIntConstant(poModule, "INVENTORY_SLOT_INDEX_RECENTLY_OBTAINED", CPythonYinAndYangAttrManager::INVENTORY_SLOT_INDEX_RECENTLY_OBTAINED);
	PyModule_AddIntConstant(poModule, "INVENTORY_DEFAULT_OPEN_SLOT_COUNT", CPythonYinAndYangAttrManager::INVENTORY_DEFAULT_OPEN_SLOT_COUNT);
	PyModule_AddIntConstant(poModule, "INVENTORY_EXPANSION_COUNT_ITEM_MAX", CPythonYinAndYangAttrManager::INVENTORY_EXPANSION_COUNT_ITEM_MAX);
	PyModule_AddIntConstant(poModule, "INVENTORY_EXPANSION_COUNT_QUEST_MAX", CPythonYinAndYangAttrManager::INVENTORY_EXPANSION_COUNT_QUEST_MAX);
	PyModule_AddIntConstant(poModule, "INVENTORY_EXPANSION_ITEM_NEED_COUNT", CPythonYinAndYangAttrManager::INVENTORY_EXPANSION_ITEM_NEED_COUNT);

	PyModule_AddIntConstant(poModule, "NEED_GOLD_TO_DELETE_ATTR", CPythonYinAndYangAttrManager::NEED_GOLD_TO_DELETE_ATTR);
	PyModule_AddIntConstant(poModule, "RECENTLY_OBTAINED_ATTR_DELETE_TIME_SEC", CPythonYinAndYangAttrManager::RECENTLY_OBTAINED_ATTR_DELETE_TIME_SEC);

	PyModule_AddIntConstant(poModule, "CONNECTION_TYPE_NONE", CPythonYinAndYangAttrManager::CONNECTION_TYPE_NONE);
	PyModule_AddIntConstant(poModule, "CONNECTION_TYPE_WEAPON", CPythonYinAndYangAttrManager::CONNECTION_TYPE_WEAPON);
	PyModule_AddIntConstant(poModule, "CONNECTION_TYPE_GLOVE", CPythonYinAndYangAttrManager::CONNECTION_TYPE_GLOVE);

	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ATTACHABLE_ITEM_LEVEL_MIN", CPythonYinAndYangAttrManager::YIN_AND_YANG_ATTACHABLE_ITEM_LEVEL_MIN);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ENABLE_REMAIN_TIME_SEC_MAX", CPythonYinAndYangAttrManager::YIN_AND_YANG_ENABLE_REMAIN_TIME_SEC_MAX);
	PyModule_AddIntConstant(poModule, "YIN_AND_YANG_ENABLE_TOGGLE_COOLTIME_SEC", CPythonYinAndYangAttrManager::YIN_AND_YANG_ENABLE_TOGGLE_COOLTIME_SEC);
}
#endif // ENABLE_YIN_AND_YANG_ATTR
