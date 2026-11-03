#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_APPLY_RANDOM
#include <algorithm>
#include <sstream>
#include "utils.h"
#include "constants.h"
#include "group_text_parse_tree.h"
#include "item_apply_random_table.h"

namespace
{
	const char* c_szListPrefix = "applylist";
	const char* c_aszPathName[APPLY_RANDOM_MAX_PATHS] = { "path1", "path2", "path3", "path4", "path5" };

	std::string ToLower(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}

	std::string ListName(int iGroup)
	{
		std::ostringstream os;
		os << c_szListPrefix << iGroup;
		return os.str();
	}
}

CApplyRandomTable::CApplyRandomTable() : m_pLoader(NULL), m_pValueNode(NULL)
{
}

CApplyRandomTable::~CApplyRandomTable()
{
	delete m_pLoader;
}

bool CApplyRandomTable::ReadApplyRandomTableFile(const char* c_pszFileName)
{
	delete m_pLoader;
	m_pLoader = new CGroupTextParseTreeLoader;
	m_vecGroupNames.clear();
	m_mapGroup.clear();

	if (!m_pLoader->Load(c_pszFileName))
		return false;
	if (!ReadApplyRandomMapper() || !ReadApplyRandomTypes())
		return false;

	m_pValueNode = m_pLoader->GetGroup("applyrandomvalues");
	if (!m_pValueNode)
	{
		sys_err("ApplyRandom: group ApplyRandomValues not found.");
		return false;
	}
	return true;
}

bool CApplyRandomTable::ReadApplyRandomMapper()
{
	CGroupNode* pNode = m_pLoader->GetGroup("applyrandommapper");
	if (!pNode || pNode->GetRowCount() == 0)
	{
		sys_err("ApplyRandom: group ApplyRandomMapper not found or empty.");
		return false;
	}

	for (int n = 0; n < pNode->GetRowCount(); ++n)
	{
		const CGroupNode::CGroupNodeRow* pRow = NULL;
		std::string strName;
		if (!pNode->GetRow(n, &pRow) || !pRow || !pRow->GetValue("group_name", strName))
		{
			sys_err("ApplyRandom: ApplyRandomMapper row %d has no Group_Name.", n);
			return false;
		}
		m_vecGroupNames.push_back(ToLower(strName));
	}
	return true;
}

bool CApplyRandomTable::ReadApplyRandomTypes()
{
	CGroupNode* pNode = m_pLoader->GetGroup("applyrandomtypes");
	if (!pNode)
	{
		sys_err("ApplyRandom: group ApplyRandomTypes not found.");
		return false;
	}

	for (size_t n = 0; n < m_vecGroupNames.size(); ++n)
	{
		CGroupNode* pChild = pNode->GetChildNode(m_vecGroupNames[n]);
		if (!pChild)
		{
			sys_err("ApplyRandom: ApplyRandomTypes has no group %s.", m_vecGroupNames[n].c_str());
			return false;
		}

		std::vector<SApplyRandom> vec;
		for (int j = 1; j <= pChild->GetRowCount(); ++j)
		{
			std::ostringstream os;
			os << j;
			const CGroupNode::CGroupNodeRow* pRow = NULL;
			if (!pChild->GetRow(os.str(), &pRow) || !pRow)
			{
				sys_err("ApplyRandom: %s has no row %d.", m_vecGroupNames[n].c_str(), j);
				return false;
			}

			std::string strType, strValueGroup;
			if (!pRow->GetValue("apply_type", strType) || !pRow->GetValue("apply_value_group_name", strValueGroup))
			{
				sys_err("ApplyRandom: %s row %d needs Apply_Type and Apply_Value_Group_Name.", m_vecGroupNames[n].c_str(), j);
				return false;
			}

			const int iType = FN_get_apply_type(strType.c_str());
			if (iType <= APPLY_NONE || iType >= MAX_APPLY_NUM)
			{
				sys_err("ApplyRandom: %s row %d has an invalid apply type %s.", m_vecGroupNames[n].c_str(), j, strType.c_str());
				return false;
			}

			SApplyRandom kApply;
			kApply.wApplyType = (WORD) iType;
			kApply.strValueGroup = ToLower(strValueGroup);
			vec.push_back(kApply);
		}
		m_mapGroup[m_vecGroupNames[n]] = vec;
	}
	return true;
}

bool CApplyRandomTable::GetPathValue(const std::string& strValueGroup, int iLevel, BYTE bPath, short& rsValue) const
{
	if (!m_pValueNode || bPath < APPLY_RANDOM_PATH1 || bPath > APPLY_RANDOM_MAX_PATHS)
		return false;

	iLevel = MINMAX(0, iLevel, (int) APPLY_RANDOM_MAX_REFINE_LEVEL);
	if (!m_pValueNode->GetGroupValue(strValueGroup, c_aszPathName[bPath - 1], iLevel, rsValue))
	{
		sys_err("ApplyRandom: no value for %s %s level %d.", strValueGroup.c_str(), c_aszPathName[bPath - 1], iLevel);
		return false;
	}
	return true;
}

bool CApplyRandomTable::GetApplyRandom(int iGroup, int iLevel, WORD& rwApplyType, short& rsApplyValue, BYTE& rbPath) const
{
	rwApplyType = APPLY_NONE;
	rsApplyValue = 0;
	rbPath = 0;

	std::map<std::string, std::vector<SApplyRandom> >::const_iterator it = m_mapGroup.find(ListName(iGroup));
	if (it == m_mapGroup.end() || it->second.empty())
		return false;

	const SApplyRandom& kApply = it->second[number(0, (int) it->second.size() - 1)];
	const BYTE bPathCount = GetPathCount(kApply.strValueGroup);
	if (bPathCount == 0)
		return false;
	const BYTE bPath = (BYTE) number(APPLY_RANDOM_PATH1, bPathCount);
	short sValue = 0;
	if (!GetPathValue(kApply.strValueGroup, iLevel, bPath, sValue))
		return false;

	rwApplyType = kApply.wApplyType;
	rsApplyValue = sValue;
	rbPath = bPath;
	return true;
}

BYTE CApplyRandomTable::GetPathCount(const std::string& strValueGroup) const
{
	CGroupNode* pGroup = m_pValueNode ? m_pValueNode->GetChildNode(strValueGroup) : NULL;
	if (!pGroup)
		return 0;

	BYTE bCount = 0;
	const CGroupNode::CGroupNodeRow* pRow = NULL;
	while (bCount < APPLY_RANDOM_MAX_PATHS && pGroup->GetRow(c_aszPathName[bCount], &pRow) && pRow)
		++bCount;
	return bCount;
}

short CApplyRandomTable::GetApplyRandomValue(int iGroup, int iLevel, BYTE bPath, WORD wApplyType) const
{
	std::map<std::string, std::vector<SApplyRandom> >::const_iterator it = m_mapGroup.find(ListName(iGroup));
	if (it == m_mapGroup.end())
		return 0;

	for (size_t i = 0; i < it->second.size(); ++i)
	{
		if (it->second[i].wApplyType != wApplyType)
			continue;
		short sValue = 0;
		return GetPathValue(it->second[i].strValueGroup, iLevel, bPath, sValue) ? sValue : 0;
	}
	return 0;
}
#endif
