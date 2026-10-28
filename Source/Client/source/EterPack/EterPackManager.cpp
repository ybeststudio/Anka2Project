#include "StdAfx.h"

#include <io.h>
#include <assert.h>

#include "EterPackManager.h"

#include "../eterBase/Debug.h"
#include "../eterBase/CRC32.h"

#define PATH_ABSOLUTE_YMIRWORK1	"d:/ymir work/"
#define PATH_ABSOLUTE_YMIRWORK2	"d:\\ymir work\\"

#ifdef ENABLE_FOXFS_ENCRYPT
namespace FoxFS
{
	enum
	{
		ERROR_OK = 0,
		ERROR_BASE_CODE = 0,
		ERROR_FILE_WAS_NOT_FOUND = ERROR_BASE_CODE + 1,
		ERROR_CORRUPTED_FILE = ERROR_BASE_CODE + 2,
		ERROR_MISSING_KEY = ERROR_BASE_CODE + 3,
		ERROR_MISSING_IV = ERROR_BASE_CODE + 4,
		ERROR_DECRYPTION_HAS_FAILED = ERROR_BASE_CODE + 5,
		ERROR_DECOMPRESSION_FAILED = ERROR_BASE_CODE + 6,
		ERROR_ARCHIVE_NOT_FOUND = ERROR_BASE_CODE + 7,
		ERROR_ARCHIVE_NOT_READABLE = ERROR_BASE_CODE + 8,
		ERROR_ARCHIVE_INVALID = ERROR_BASE_CODE + 9,
		ERROR_ARCHIVE_ACCESS_DENIED = ERROR_BASE_CODE + 10,
		ERROR_KEYSERVER_SOCKET = ERROR_BASE_CODE + 11,
		ERROR_KEYSERVER_CONNECTION = ERROR_BASE_CODE + 12,
		ERROR_KEYSERVER_RESPONSE = ERROR_BASE_CODE + 13,
		ERROR_KEYSERVER_TIMEOUT = ERROR_BASE_CODE + 14,
		ERROR_UNKNOWN = ERROR_BASE_CODE + 15
	};
}

const char *white_file_list[] = { "mark\10_0.tga" "mark\250_0.tga" };

bool isWhiteFile(const char *c_szFileName)
{
	for (int i = 0; i < ARRAYSIZE(white_file_list); i++)
	{
		if (stricmp(c_szFileName, white_file_list[i]) == 0)
		{
			return true;
		}
	}
	return false;
}

const char *white_file_list_ext[] = { "xml", "tga", "png", "bmp", "mp3", "wav", "jpg", "dds", "fxd", "psh", "vsc", "psc", "mse", "msa", "msf", "mde", "gr2", "msenv", "prb", "prd", "prt", "sub", "txt", "atr", "spt", "pre" };

bool isWhiteFileExt(const char *c_szFileName)
{
	for (int i = 0; i < ARRAYSIZE(white_file_list_ext); i++)
	{
		auto v = std::string(c_szFileName);
		std::string ext = CFileNameHelper::GetExtension(v);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		std::string whiteExt = std::string(white_file_list_ext[i]);

		if (ext == whiteExt)
			return true;
	}
	return false;
}
#endif

CEterPack* CEterPackManager::FindPack(const char *c_szPathName)
{
	std::string strFileName;

	if (0 == ConvertFileName(c_szPathName, strFileName))
	{
		return &m_RootPack;
	}
	else
	{
		for (TEterPackMap::iterator itor = m_DirPackMap.begin(); itor != m_DirPackMap.end(); ++itor)
		{
			const std::string & c_rstrName = itor->first;
			CEterPack * pEterPack = itor->second;

			if (CompareName(c_rstrName.c_str(), c_rstrName.length(), strFileName.c_str()))
			{
				return pEterPack;
			}
		}
	}

	return NULL;
}

void CEterPackManager::SetCacheMode()
{
	m_isCacheMode=true;
}

void CEterPackManager::SetRelativePathMode()
{
	m_bTryRelativePath = true;
}


// StringPath std::string ¹öÀü
int CEterPackManager::ConvertFileName(const char *c_szFileName, std::string & rstrFileName)
{
	rstrFileName = c_szFileName;
	stl_lowers(rstrFileName);

	int iCount = 0;

	for (DWORD i = 0; i < rstrFileName.length(); ++i)
	{
		if (rstrFileName[i] == '/')
			++iCount;
		else if (rstrFileName[i] == '\\')
		{
			rstrFileName[i] = '/';
			++iCount;
		}
	}

	return iCount;
}

bool CEterPackManager::CompareName(const char *c_szDirectoryName, DWORD /*dwLength*/, const char *c_szFileName)
{
	const char *c_pszSrc = c_szDirectoryName;
	const char *c_pszCmp = c_szFileName;

	while (*c_pszSrc)
	{
		if (*(c_pszSrc++) != *(c_pszCmp++))
			return false;

		if (!*c_pszCmp)
			return false;
	}

	return true;
}

void CEterPackManager::LoadStaticCache(const char *c_szFileName)
{
	if (!m_isCacheMode)
		return;

	std::string strFileName;
	if (0 == ConvertFileName(c_szFileName, strFileName))
	{
		return;
	}
	
	DWORD dwFileNameHash = GetCRC32(strFileName.c_str(), strFileName.length());

	std::unordered_map<DWORD, SCache>::iterator f = m_kMap_dwNameKey_kCache.find(dwFileNameHash);
	if (m_kMap_dwNameKey_kCache.end() != f)
		return;	
	
	CMappedFile kMapFile;
	const void* c_pvData;
	if (!Get(kMapFile, c_szFileName, &c_pvData))
		return;

	SCache kNewCache;
	kNewCache.m_dwBufSize = kMapFile.Size();
	kNewCache.m_abBufData = new BYTE[kNewCache.m_dwBufSize];
	memcpy(kNewCache.m_abBufData, c_pvData, kNewCache.m_dwBufSize);
	m_kMap_dwNameKey_kCache.insert(std::unordered_map<DWORD, SCache>::value_type(dwFileNameHash, kNewCache));
}

CEterPackManager::SCache* CEterPackManager::__FindCache(DWORD dwFileNameHash)
{	
	std::unordered_map<DWORD, SCache>::iterator f=m_kMap_dwNameKey_kCache.find(dwFileNameHash);
	if (m_kMap_dwNameKey_kCache.end()==f)
		return NULL;

	return &f->second;
}

void	CEterPackManager::__ClearCacheMap()
{
	std::unordered_map<DWORD, SCache>::iterator i;

	for (i = m_kMap_dwNameKey_kCache.begin(); i != m_kMap_dwNameKey_kCache.end(); ++i)
		delete [] i->second.m_abBufData;

	m_kMap_dwNameKey_kCache.clear();
}

struct TimeChecker
{
	TimeChecker(const char *name) : name(name)
	{
		baseTime = timeGetTime();
	}
	~TimeChecker()
	{
		printf("load %s (%d)\n", name, timeGetTime() - baseTime);
	}

	const char *name;
	DWORD baseTime;
};

bool CEterPackManager::Get(CMappedFile& rMappedFile, const char *c_szFileName, LPCVOID* pData)
{
#ifdef ENABLE_FOXFS_ENCRYPT
	if (m_iSearchMode == SEARCH_FILE)
	{
		if (GetFromFile(rMappedFile, c_szFileName, pData))
		{
			return true;
		}

		return GetFromPack(rMappedFile, c_szFileName, pData);
	}
	else
	{
		if (GetFromPack(rMappedFile, c_szFileName, pData))
		{
			return true;
		}

		if (isWhiteFile(c_szFileName) || isWhiteFileExt(c_szFileName))
		{
			if (_access(c_szFileName, 0) == 0)
			{
				return GetFromFile(rMappedFile, c_szFileName, pData);
			}
		}
	}
	return false;
#else
	if (m_iSearchMode == SEARCH_PACK_FIRST)
	{
		if (GetFromPack(rMappedFile, c_szFileName, pData))
			return true;

		if (c_szFileName[1] != ':' && GetFromFile(rMappedFile, c_szFileName, pData))
		{
			TraceError("%s", c_szFileName); // only for log. it's not an error.
			return true;
		}
	}

	if (m_iSearchMode == SEARCH_FILE_FIRST)
	{
		if (GetFromFile(rMappedFile, c_szFileName, pData))
			return true;

		return GetFromPack(rMappedFile, c_szFileName, pData);
	}

	return false;
#endif
}

struct FinderLock
{
	FinderLock(CRITICAL_SECTION& cs) : p_cs(&cs)
	{
		EnterCriticalSection(p_cs);
	}

	~FinderLock()
	{
		LeaveCriticalSection(p_cs);
	}

	CRITICAL_SECTION* p_cs;
};

#ifdef ENABLE_FOXFS_ENCRYPT
static bool FoxFS_ResolvePath(PFoxFS pFoxFS, const char* c_szFileName, std::string& strFoundPath)
{
	if (!pFoxFS || !c_szFileName || !*c_szFileName)
		return false;

	// 1. Try original path as-is
	if (FoxFS_ExistsA(pFoxFS, c_szFileName) == FoxFS::ERROR_OK)
	{
		strFoundPath = c_szFileName;
		return true;
	}

	// 2. Normalize backslashes to forward slashes and collapse multiple slashes
	std::string normalized = c_szFileName;
	for (size_t i = 0; i < normalized.length(); ++i)
	{
		if (normalized[i] == '\\')
			normalized[i] = '/';
	}
	size_t dSlash;
	while ((dSlash = normalized.find("//")) != std::string::npos)
	{
		normalized.replace(dSlash, 2, "/");
	}

	if (FoxFS_ExistsA(pFoxFS, normalized.c_str()) == FoxFS::ERROR_OK)
	{
		strFoundPath = normalized;
		return true;
	}

	// 3. Try stripping "d:/" prefix
	if (_strnicmp(normalized.c_str(), "d:/", 3) == 0)
	{
		std::string stripped = normalized.substr(3);
		if (FoxFS_ExistsA(pFoxFS, stripped.c_str()) == FoxFS::ERROR_OK)
		{
			strFoundPath = stripped;
			return true;
		}
	}
	// 4. Try prepending "d:/" prefix if not present
	else
	{
		std::string prepended = "d:/" + normalized;
		if (FoxFS_ExistsA(pFoxFS, prepended.c_str()) == FoxFS::ERROR_OK)
		{
			strFoundPath = prepended;
			return true;
		}
	}

	// 5. Special resolution for trees:
	// Maps/properties frequently refer to paths like:
	// "d:/ymir work/tree/Shye/Officials/b1_pagodatree_rt3.spt"
	// but tree pack archives store them flat at "d:/ymir work/tree/b1_pagodatree_rt3.spt"
	size_t treePos = normalized.find("tree/");
	if (treePos != std::string::npos)
	{
		size_t lastSlash = normalized.find_last_of('/');
		if (lastSlash != std::string::npos && lastSlash > treePos + 4)
		{
			std::string fileNameOnly = normalized.substr(lastSlash + 1);
			std::string directTree = "d:/ymir work/tree/" + fileNameOnly;
			if (FoxFS_ExistsA(pFoxFS, directTree.c_str()) == FoxFS::ERROR_OK)
		{
				strFoundPath = directTree;
				return true;
			}
			std::string directTreeNoD = "ymir work/tree/" + fileNameOnly;
			if (FoxFS_ExistsA(pFoxFS, directTreeNoD.c_str()) == FoxFS::ERROR_OK)
		{
				strFoundPath = directTreeNoD;
				return true;
			}
		}
	}

	// 6. Lowercase variant
	std::string lowerStr = normalized;
	for (size_t i = 0; i < lowerStr.length(); ++i)
	{
		lowerStr[i] = (char)tolower(lowerStr[i]);
	}
	if (lowerStr != normalized && FoxFS_ExistsA(pFoxFS, lowerStr.c_str()) == FoxFS::ERROR_OK)
	{
		strFoundPath = lowerStr;
		return true;
	}

	return false;
}
#endif

bool CEterPackManager::GetFromPack(CMappedFile & rMappedFile, const char *c_szFileName, LPCVOID * pData)
{
#ifdef ENABLE_FOXFS_ENCRYPT
	assert(c_szFileName);

	FinderLock lock(m_csFinder);

	if (m_pFoxFS)
	{
		std::string strLookupPath;
		if (FoxFS_ResolvePath(m_pFoxFS, c_szFileName, strLookupPath))
		{
			unsigned int dwSize = FoxFS_SizeA(m_pFoxFS, strLookupPath.c_str()), dwReadSize = 0;
			if (dwSize == 0)
			{
				TraceError("FoxFS - File size is 0: %s", strLookupPath.c_str());
				return false;
			}
			BYTE* pbData = new BYTE[dwSize + 1];
			int errorCode = 0;
			if ((errorCode = FoxFS_GetA(m_pFoxFS, strLookupPath.c_str(), pbData, dwSize, &dwReadSize)) == FoxFS::ERROR_OK)
			{
				pbData[dwReadSize] = 0;
				*pData = pbData;
				rMappedFile.Link(dwReadSize, pbData);
				return true;
			}
			else {
				TraceError("FoxFS - Could not get file %s Error Code %d", strLookupPath.c_str(), errorCode);
			}
			delete[] pbData;
		}
	}
	else
	{
		TraceError("FoxFS: Not initialized!");
	}

	return false;

#else
	FinderLock lock(m_csFinder);

	static std::string strFileName;
	
	if (0 == ConvertFileName(c_szFileName, strFileName))
	{
		return m_RootPack.Get(rMappedFile, strFileName.c_str(), pData);
	}
	else
	{
		DWORD dwFileNameHash = GetCRC32(strFileName.c_str(), strFileName.length());
		SCache* pkCache = __FindCache(dwFileNameHash);

		if (pkCache)
		{
			rMappedFile.Link(pkCache->m_dwBufSize, pkCache->m_abBufData);
			return true;
		}

		CEterFileDict::Item* pkFileItem = m_FileDict.GetItem(dwFileNameHash, strFileName.c_str());

		if (pkFileItem)
			if (pkFileItem->pkPack)
			{
				bool r = pkFileItem->pkPack->Get2(rMappedFile, strFileName.c_str(), pkFileItem->pkInfo, pData);	
				//pkFileItem->pkPack->ClearDataMemoryMap();
				return r;
			}
	}
#ifdef _DEBUG
	TraceError("CANNOT_FIND_PACK_FILE [%s]", strFileName.c_str());
#endif

	return false;
#endif
}

const time_t g_tCachingInterval = 10; // 10ÃÊ
void CEterPackManager::ArrangeMemoryMappedPack()
{
	//time_t curTime = time(nullptr);
	//CEterFileDict::TDict dict = m_FileDict.GetDict();
	//for (CEterFileDict::TDict::iterator it = dict.begin(); it != dict.end(); ++it)
	//{
	//	CEterFileDict::Item &rFileItem = it->second;
	//	CEterPack* pkPack = rFileItem.pkPack;
	//	if (pkPack)
	//	{
	//		if (curTime - pkPack->GetLastAccessTime() > g_tCachingInterval)
	//		{
	//			pkPack->ClearDataMemoryMap();
	//		}
	//	}
	//}
}

bool CEterPackManager::GetFromFile(CMappedFile & rMappedFile, const char *c_szFileName, LPCVOID * pData)
{
	return rMappedFile.Create(c_szFileName, pData, 0, 0) ? true : false;
}

bool CEterPackManager::isExistInPack(const char *c_szFileName)
{
#ifdef ENABLE_FOXFS_ENCRYPT
	assert(c_szFileName);

	if (m_pFoxFS)
	{
		std::string strResolved;
		return FoxFS_ResolvePath(m_pFoxFS, c_szFileName, strResolved);
	}
	else
	{
		TraceError("FoxFS: Not initialized!");
	}

	return false;
#else
	std::string strFileName;

	if (0 == ConvertFileName(c_szFileName, strFileName))
	{
		return m_RootPack.IsExist(strFileName.c_str());
	}
	else
	{
		DWORD dwFileNameHash = GetCRC32(strFileName.c_str(), strFileName.length());
		CEterFileDict::Item* pkFileItem = m_FileDict.GetItem(dwFileNameHash, strFileName.c_str());

		if (pkFileItem)
			if (pkFileItem->pkPack)
				return pkFileItem->pkPack->IsExist(strFileName.c_str());
	}

	return false;
#endif
}

bool CEterPackManager::isExist(const char *c_szFileName)
{
#ifdef ENABLE_FOXFS_ENCRYPT
	if (m_iSearchMode == SEARCH_PACK)
	{
		if (isWhiteFile(c_szFileName) || isWhiteFileExt(c_szFileName))
		{
			return isExistInPack(c_szFileName) || (_access(c_szFileName, 0) == 0);
		}
		return isExistInPack(c_szFileName);
	}

	if (_access(c_szFileName, 0) == 0)
		return true;
#endif

	return isExistInPack(c_szFileName);
}

void CEterPackManager::RegisterRootPack(const char *c_szName)
{
#ifdef ENABLE_FOXFS_ENCRYPT
	assert(c_szName);
	if (m_pFoxFS)
	{
		int errorCode = 0;
		if ((errorCode = FoxFS_LoadA(m_pFoxFS, c_szName)) != FoxFS::ERROR_OK)
		{
			TraceError("%s: Error Code %d", c_szName, errorCode);
		}
	}
	else
	{
		TraceError("FoxFS: Not initialized!");
	}
#else
	if (!m_RootPack.Create(m_FileDict, c_szName, ""))
	{
		TraceError("%s: Pack file does not exist", c_szName);
	}
#endif
}

const char *CEterPackManager::GetRootPackFileName()
{
	return m_RootPack.GetDBName();
}

bool CEterPackManager::RegisterPackWhenPackMaking(const char *c_szName, const char *c_szDirectory, CEterPack* pPack)
{
	m_PackMap.insert(TEterPackMap::value_type(c_szName, pPack));
	m_PackList.push_front(pPack);

	m_DirPackMap.insert(TEterPackMap::value_type(c_szDirectory, pPack));
	return true;
}


bool CEterPackManager::RegisterPack(const char *c_szName, const char *c_szDirectory, const BYTE* c_pbIV)
{
#ifdef ENABLE_FOXFS_ENCRYPT
	assert(c_szName);
	if (m_pFoxFS)
	{
		int errorCode = 0;
		if ((errorCode = FoxFS_LoadA(m_pFoxFS, c_szName)) != FoxFS::ERROR_OK)
		{
			TraceError("%s: Error Code %d", c_szName, errorCode);
		}
	}
	else
	{
		TraceError("FoxFS: Not initialized!");
	}

	return false;
#else
	CEterPack * pEterPack = nullptr;
	{
		TEterPackMap::iterator itor = m_PackMap.find(c_szName);

		if (m_PackMap.end() == itor)
		{
			bool bReadOnly = true;

			pEterPack = new CEterPack;
			if (pEterPack->Create(m_FileDict, c_szName, c_szDirectory, bReadOnly, c_pbIV))
			{
				m_PackMap.insert(TEterPackMap::value_type(c_szName, pEterPack));
			}
			else
			{
#ifdef _DEBUG
				Tracef("The eterpack doesn't exist [%s]\n", c_szName);
#endif
				delete pEterPack;
				pEterPack = nullptr;
				return false;
			}
		}
		else
		{
			pEterPack = itor->second;
		}
	}

	if (c_szDirectory && c_szDirectory[0] != '*')
	{
		TEterPackMap::iterator itor = m_DirPackMap.find(c_szDirectory);
		if (m_DirPackMap.end() == itor)
		{
			m_PackList.push_front(pEterPack);
			m_DirPackMap.insert(TEterPackMap::value_type(c_szDirectory, pEterPack));
		}
	}

	return true;
#endif
}

void CEterPackManager::SetSearchMode(bool bPackFirst)
{
	m_iSearchMode = bPackFirst ? SEARCH_PACK_FIRST : SEARCH_FILE_FIRST;
}

int CEterPackManager::GetSearchMode()
{
	return m_iSearchMode;
}

CEterPackManager::CEterPackManager() : m_bTryRelativePath(false), m_iSearchMode(SEARCH_FILE_FIRST), m_isCacheMode(false)
{
	InitializeCriticalSection(&m_csFinder);
#ifdef ENABLE_FOXFS_ENCRYPT
	m_pFoxFS = FoxFS_Create();
#endif
}

CEterPackManager::~CEterPackManager()
{
	__ClearCacheMap();

	TEterPackMap::iterator i = m_PackMap.begin();
	TEterPackMap::iterator e = m_PackMap.end();
	while (i != e)
	{
		delete i->second;
		i++;
	}

	DeleteCriticalSection(&m_csFinder);

#ifdef ENABLE_FOXFS_ENCRYPT
	if (m_pFoxFS)
	{
		FoxFS_Destroy(m_pFoxFS);
	}
#endif
}
