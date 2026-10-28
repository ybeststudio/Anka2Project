// NonPlayerCharacterInfo.cpp: implementation of the NonPlayerCharacterInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "../worldeditor.h"
#include "NonPlayerCharacterInfo.h"

#ifdef _DEBUG
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
	#define new DEBUG_NEW
#endif

CNonPlayerCharacterInfo aNonPlayerCharacterInfo;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNonPlayerCharacterInfo::CNonPlayerCharacterInfo()
{
	Clear();
}

CNonPlayerCharacterInfo::~CNonPlayerCharacterInfo()
{
	Destroy();
}

bool CNonPlayerCharacterInfo::LoadNonPlayerData (const char* c_szFileName)
{
	static DWORD s_adwMobProtoKey[4] =
	{
		4813894,
		18955,
		552631,
		6822045
	};

	CMappedFile file;
	LPCVOID pvData;

	if (!CEterPackManager::Instance().Get (file, c_szFileName, &pvData))
	{
		return false;
	}

	DWORD dwFourCC, dwElements, dwDataSize;

	file.Read (&dwFourCC, sizeof (DWORD));

	if (dwFourCC != MAKEFOURCC ('M', 'M', 'P', 'T'))
	{
		TraceError ("CPythonMob::LoadMobTable: invalid Mob proto type %s", c_szFileName);
		return false;
	}

	file.Read (&dwElements, sizeof (DWORD));
	file.Read (&dwDataSize, sizeof (DWORD));

	BYTE * pbData = new BYTE[dwDataSize];
	file.Read (pbData, dwDataSize);
	/////

	CLZObject zObj;

	if (!CLZO::Instance().Decompress (zObj, pbData, s_adwMobProtoKey))
	{
		delete [] pbData;
		return false;
	}

	TMobTable * pTable = (TMobTable*) zObj.GetBuffer();
	for (DWORD i = 0; i < dwElements; ++i, ++pTable)
	{
		TMobTable * pNonPlayerData = new TMobTable;

		memcpy (pNonPlayerData, pTable, sizeof (TMobTable));
		//Tracef(" [%d] %s - %d, %d\n", pNonPlayerData->dwVnum, pNonPlayerData->szName, pNonPlayerData->bType, pNonPlayerData->on_click);
		m_NonPlayerDataMap.insert (TNonPlayerDataMap::value_type (pNonPlayerData->dwVnum, pNonPlayerData));
	}

	delete [] pbData;
	return true;
}

bool CNonPlayerCharacterInfo::LoadNPCGroupData (const char* c_szFileName)
{
	CTokenVectorMap stTokenVectorMap;

	LPCVOID pModelData;
	CMappedFile File;

	if (!CEterPackManager::Instance().Get (File, c_szFileName, &pModelData))
	{
		TraceError (" CNonPlayerCharacterInfo::LoadNPCGroupData Load File %s ERROR", c_szFileName);
		return false;
	}

	CMemoryTextFileLoader textFileLoader;
	CTokenVector stTokenVector;

	textFileLoader.Bind (File.Size(), pModelData);

	for (DWORD i = 0; i < textFileLoader.GetLineCount();)
	{
		DWORD dwLineIncrementCount = 1;

		if (!textFileLoader.SplitLine (i, &stTokenVector))
		{
			++i;
			continue;
		}

		stl_lowers (stTokenVector[0]);

		// Start or End
		if (0 == stTokenVector[0].compare ("group"))
		{

			TNPCGroup * pNPCGroup = new TNPCGroup;
			pNPCGroup->m_FollowerIDVector.clear();

			//			const std::string & c_rstrGroupID	= stTokenVector[1].c_str();
			//			DWORD dwGroupID = (DWORD) atoi(c_rstrGroupID.c_str());

			const std::string & c_rstrGroupName = stTokenVector[1].c_str();
			strncpy (pNPCGroup->m_szName, c_rstrGroupName.c_str(), sizeof (pNPCGroup->m_szName));
			DWORD dwGroupID = 0;

			bool bLeaderExist = false;
			bool bFollowerExist = false;

			CTokenVector stGroupTokenVector;
			for (DWORD j = i + 1; j < textFileLoader.GetLineCount(); ++j)
			{
				if (!textFileLoader.SplitLine (j, &stGroupTokenVector))
				{
					continue;
				}

				++dwLineIncrementCount;

				stl_lowers (stGroupTokenVector[0]);

				if (0 == stGroupTokenVector[0].compare ("}"))
				{
					break;
				}
				else if (0 == stGroupTokenVector[0].compare ("{"))
				{
					continue;
				}
				else if (0 == stGroupTokenVector[0].compare ("vnum"))
				{
					const std::string & c_rstrGroupID	= stGroupTokenVector[1].c_str();
					dwGroupID = (DWORD) atoi (c_rstrGroupID.c_str());
				}
				else if (0 == stGroupTokenVector[0].compare ("leader"))
				{
					const std::string & c_rstrMonsterGroupLeaderName = stGroupTokenVector[1].c_str();
					const std::string & c_rstrMonsterGroupLeaderVID	= stGroupTokenVector[2].c_str();
					DWORD dwMonsterGroupLeaderVID = (DWORD) atoi (c_rstrMonsterGroupLeaderVID.c_str());

					pNPCGroup->m_dwLeaderID = dwMonsterGroupLeaderVID;
					bLeaderExist = true;
				}
				else
				{
					const std::string & c_rstrMonsterGroupFollowerID	= stGroupTokenVector[0].c_str();
					const std::string & c_rstrMonsterGroupFollowerName	= stGroupTokenVector[1].c_str();
					const std::string & c_rstrMonsterGroupFollowerVID	= stGroupTokenVector[2].c_str();
					DWORD dwMonsterGroupFollowerVID = (DWORD) atoi (c_rstrMonsterGroupFollowerVID.c_str());
					pNPCGroup->m_FollowerIDVector.push_back (dwMonsterGroupFollowerVID);
					bFollowerExist = true;
				}
			}
			if (!bLeaderExist || !bFollowerExist || 0 == dwGroupID)
			{
				TraceError ("Leader or Follower not Exist in Group %d[%s]", dwGroupID, pNPCGroup->m_szName);
				delete pNPCGroup;
				pNPCGroup = NULL;
			}
			else
			{
				m_NPCGroupDataMap.insert (TNPCGroupDataMap::value_type (dwGroupID, pNPCGroup));
			}
		}
		i += dwLineIncrementCount;
	}

	return true;
}

const char* CNonPlayerCharacterInfo::GetNameByVID (DWORD dwVID)
{
	const TMobTable * p = GetTable (dwVID);

	if (!p)
	{
		return "No name";
	}

	return p->szName;
}

BYTE CNonPlayerCharacterInfo::GetRankByVID (DWORD dwVID)
{
	const TMobTable * p = GetTable (dwVID);

	if (!p)
	{
		return 0;
	}

	return p->bRank;
}

void CNonPlayerCharacterInfo::GetLevelRangeByVID (DWORD dwVID, BYTE * pbyLowLevelLimit, BYTE * pbyHighLevelLimit)
{
	const TMobTable * p = GetTable (dwVID);

	if (!p)
	{
		*pbyLowLevelLimit = 0;
		*pbyHighLevelLimit = 0;
		return;
	}
	*pbyLowLevelLimit = p->abLevelRange[0];
	*pbyHighLevelLimit = p->abLevelRange[1];
}

DWORD CNonPlayerCharacterInfo::GetVIDByName (std::string strName)
{
	m_NonPlayerDataMapIterator = m_NonPlayerDataMap.begin();
	while (m_NonPlayerDataMapIterator != m_NonPlayerDataMap.end())
	{
		const TMobTable * p = m_NonPlayerDataMapIterator->second;
		if (0 == strName.compare (p->szName))
		{
			return m_NonPlayerDataMapIterator->first;
		}
		++m_NonPlayerDataMapIterator;
	}
	TraceError ("CNonPlayerCharacterInfo::GetVIDByName Cannot find VID of [%s]. Returning 0.", strName.c_str());
	return 0;
}

const char* CNonPlayerCharacterInfo::GetNameByMapIndex (DWORD dwMapIndex)
{
	m_NonPlayerDataMapIterator = m_NonPlayerDataMap.begin();
	DWORD dwCount = 0;
	while (dwCount < dwMapIndex)
	{
		++m_NonPlayerDataMapIterator;
		++dwCount;
	}
	const TMobTable * p = m_NonPlayerDataMapIterator->second;

	if (!p)
	{
		return "No name";
	}

	return p->szName;
}

void CNonPlayerCharacterInfo::GetLevelRangeByMapIndex (DWORD dwMapIndex, BYTE * pbyLowLevelLimit, BYTE * pbyHighLevelLimit)
{
	m_NonPlayerDataMapIterator = m_NonPlayerDataMap.begin();
	DWORD dwCount = 0;
	while (dwCount < dwMapIndex)
	{
		++m_NonPlayerDataMapIterator;
		++dwCount;
	}
	const TMobTable * p = m_NonPlayerDataMapIterator->second;

	if (!p)
	{
		*pbyLowLevelLimit = 0;
		*pbyHighLevelLimit = 0;
		return;
	}
	*pbyLowLevelLimit = p->abLevelRange[0];
	*pbyHighLevelLimit = p->abLevelRange[1];
}

BYTE CNonPlayerCharacterInfo::GetRankByMapIndex (DWORD dwMapIndex)
{
	m_NonPlayerDataMapIterator = m_NonPlayerDataMap.begin();
	DWORD dwCount = 0;
	while (dwCount < dwMapIndex)
	{
		++m_NonPlayerDataMapIterator;
		++dwCount;
	}
	const TMobTable * p = m_NonPlayerDataMapIterator->second;

	if (!p)
	{
		return 0;
	}

	return p->bRank;
}

DWORD CNonPlayerCharacterInfo::GetVIDByMapIndex (DWORD dwMapIndex)
{
	m_NonPlayerDataMapIterator = m_NonPlayerDataMap.begin();
	DWORD dwCount = 0;
	while (dwCount < dwMapIndex)
	{
		++m_NonPlayerDataMapIterator;
		++dwCount;
	}
	const TMobTable * p = m_NonPlayerDataMapIterator->second;

	if (!p)
	{
		return 0;
	}

	return m_NonPlayerDataMapIterator->first;
}

BYTE CNonPlayerCharacterInfo::GetInstanceType (DWORD dwVnum)
{
	const TMobTable * p = GetTable (dwVnum);

	if (!p)
	{
		return INSTANCE_TYPE_PLAYER;
	}

	return p->bType;
}

const CNonPlayerCharacterInfo::TMobTable* CNonPlayerCharacterInfo::GetTable (DWORD dwVnum)
{
	TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.find (dwVnum);

	if (itor == m_NonPlayerDataMap.end())
	{
		return NULL;
	}

	return itor->second;
}

void CNonPlayerCharacterInfo::Clear()
{
}

void CNonPlayerCharacterInfo::Destroy()
{
	for (TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.begin(); itor != m_NonPlayerDataMap.end(); ++itor)
	{
		delete itor->second;
	}
	m_NonPlayerDataMap.clear();

	for (TNPCGroupDataMap::iterator aNPCGroupDataIterator = m_NPCGroupDataMap.begin(); aNPCGroupDataIterator != m_NPCGroupDataMap.end(); ++aNPCGroupDataIterator)
	{
		delete aNPCGroupDataIterator->second;
	}
	m_NPCGroupDataMap.clear();
}

DWORD CNonPlayerCharacterInfo::GetNonPlayerCount()
{
	return m_NonPlayerDataMap.size();
}


// MonsterGroup
const CNonPlayerCharacterInfo::TNPCGroup* CNonPlayerCharacterInfo::GetGroup (DWORD dwGroupID)
{
	TNPCGroupDataMap::iterator itor = m_NPCGroupDataMap.find (dwGroupID);

	if (itor == m_NPCGroupDataMap.end())
	{
		return NULL;
	}

	return itor->second;
}

DWORD CNonPlayerCharacterInfo::GetNPCGroupCount()
{
	return m_NPCGroupDataMap.size();
}

const char* CNonPlayerCharacterInfo::GetNPCGroupNameByMapIndex (DWORD dwMapIndex)
{
	m_NPCGroupDataMapIterator = m_NPCGroupDataMap.begin();
	DWORD dwCount = 0;
	while (dwCount < dwMapIndex)
	{
		++m_NPCGroupDataMapIterator;
		++dwCount;
	}
	const TNPCGroup * pNPCGroup = m_NPCGroupDataMapIterator->second;

	if (!pNPCGroup)
	{
		return "No name";
	}

	return pNPCGroup->m_szName;
}

DWORD CNonPlayerCharacterInfo::GetNPCGroupIDByMapIndex (DWORD dwMapIndex)
{
	m_NPCGroupDataMapIterator = m_NPCGroupDataMap.begin();
	DWORD dwCount = 0;
	while (dwCount < dwMapIndex)
	{
		++m_NPCGroupDataMapIterator;
		++dwCount;
	}
	const TNPCGroup * pNPCGroup = m_NPCGroupDataMapIterator->second;

	if (!pNPCGroup)
	{
		return 0;
	}

	return m_NPCGroupDataMapIterator->first;
}

DWORD CNonPlayerCharacterInfo::GetNPCGroupLeaderVIDByMapIndex (DWORD dwMapIndex)
{
	m_NPCGroupDataMapIterator = m_NPCGroupDataMap.begin();
	DWORD dwCount = 0;
	while (dwCount < dwMapIndex)
	{
		++m_NPCGroupDataMapIterator;
		++dwCount;
	}
	const TNPCGroup * pNPCGroup = m_NPCGroupDataMapIterator->second;

	if (!pNPCGroup)
	{
		return 0;
	}

	return pNPCGroup->m_dwLeaderID;
}

const char* CNonPlayerCharacterInfo::GetNPCGroupLeaderNameByMapIndex (DWORD dwMapIndex)
{
	DWORD dwLeaderVID = GetNPCGroupLeaderVIDByMapIndex (dwMapIndex);
	if (0 == dwLeaderVID)
	{
		return "No name";
	}
	else
	{
		return GetNameByVID (dwLeaderVID);
	}
}

std::vector<DWORD> CNonPlayerCharacterInfo::GetNPCGroupFollowerVIDsByMapIndex (DWORD dwMapIndex)
{
	std::vector<DWORD> aFollowerVIDVector;

	m_NPCGroupDataMapIterator = m_NPCGroupDataMap.begin();
	DWORD dwCount = 0;
	while (dwCount < dwMapIndex)
	{
		++m_NPCGroupDataMapIterator;
		++dwCount;
	}
	const TNPCGroup * pNPCGroup = m_NPCGroupDataMapIterator->second;

	if (!pNPCGroup)
	{
		return aFollowerVIDVector;
	}

	return pNPCGroup->m_FollowerIDVector;

}

DWORD CNonPlayerCharacterInfo::GetNPCGroupFollowerCountByMapIndex (DWORD dwMapIndex)
{
	auto rFollowerVIDVector = GetNPCGroupFollowerVIDsByMapIndex (dwMapIndex);
	return rFollowerVIDVector.size();
}


DWORD CNonPlayerCharacterInfo::GetGroupIDByGroupName (std::string strGroupName)
{
	m_NPCGroupDataMapIterator = m_NPCGroupDataMap.begin();
	while (m_NPCGroupDataMapIterator != m_NPCGroupDataMap.end())
	{
		const TNPCGroup * pNPCGroup = m_NPCGroupDataMapIterator->second;
		if (0 == strGroupName.compare (pNPCGroup->m_szName))
		{
			return m_NPCGroupDataMapIterator->first;
		}
		++m_NPCGroupDataMapIterator;
	}
	TraceError ("CNonPlayerCharacterInfo::GetGroupIDByGroupName Cannot find GroupID of [%s]. Returning 0.", strGroupName.c_str());
	return 0;
}

const char* CNonPlayerCharacterInfo::GetNPCGroupNameByGroupID (DWORD dwGroupID)
{
	const TNPCGroup * pNPCGroup = GetGroup (dwGroupID);

	if (!pNPCGroup)
	{
		return "No name";
	}

	return pNPCGroup->m_szName;
}

DWORD CNonPlayerCharacterInfo::GetNPCGroupLeaderVIDByGroupID (DWORD dwGroupID)
{
	const TNPCGroup * pNPCGroup = GetGroup (dwGroupID);

	if (!pNPCGroup)
	{
		return 0;
	}

	return pNPCGroup->m_dwLeaderID;
}

const char* CNonPlayerCharacterInfo::GetNPCGroupLeaderNameByGroupID (DWORD dwGroupID)
{
	const TNPCGroup * pNPCGroup = GetGroup (dwGroupID);

	if (!pNPCGroup)
	{
		return "No name";
	}

	return GetNameByVID (pNPCGroup->m_dwLeaderID);
}

std::vector<DWORD>	CNonPlayerCharacterInfo::GetNPCGroupFollowerVIDsByGroupID (DWORD dwGroupID)
{
	static std::vector<DWORD> aEmptyVector;
	const TNPCGroup * pNPCGroup = GetGroup (dwGroupID);

	if (!pNPCGroup)
	{
		return aEmptyVector;
	}

	return pNPCGroup->m_FollowerIDVector;
}

DWORD CNonPlayerCharacterInfo::GetNPCGroupFollowerCountByGroupID (DWORD dwGroupID)
{
	std::vector<DWORD> aGroupFollowerVector = GetNPCGroupFollowerVIDsByGroupID (dwGroupID);

	return aGroupFollowerVector.size();
}
