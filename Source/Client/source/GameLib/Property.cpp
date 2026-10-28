#include "StdAfx.h"
#include <string.h>
#include "../eterBase/TempFile.h"

#include "PropertyManager.h"
#include "Property.h"

CProperty::CProperty(const char *c_pszFileName) : mc_pFileName(nullptr), m_dwCRC(0)
{
	m_stFileName = c_pszFileName;
	StringPath(m_stFileName);

	mc_pFileName = strrchr(m_stFileName.c_str(), '/');

	if (!mc_pFileName)
		mc_pFileName = m_stFileName.c_str();
	else
		++mc_pFileName;
}

CProperty::~CProperty()
{
}

DWORD CProperty::GetCRC()
{
	return m_dwCRC;
}

const char *CProperty::GetFileName()
{
	return (m_stFileName.c_str());
}

bool CProperty::GetString(const char *c_pszKey, const char ** c_ppString)
{
	std::string stTempKey = c_pszKey;
	stl_lowers(stTempKey);
	CTokenVectorMap::iterator it = m_stTokenMap.find(stTempKey.c_str());

	if (m_stTokenMap.end() == it)
		return false;

	*c_ppString = it->second[0].c_str();
	return true;
}

DWORD CProperty::GetSize()
{
	return m_stTokenMap.size();
}

bool CProperty::GetVector(const char *c_pszKey, CTokenVector & rTokenVector)
{
	std::string stTempKey = c_pszKey;
	stl_lowers(stTempKey);
	CTokenVectorMap::iterator it = m_stTokenMap.find(stTempKey.c_str());

	if (m_stTokenMap.end() == it)
		return false;

	CTokenVector & rSourceTokenVector = it->second;
	CTokenVector::iterator itor = rSourceTokenVector.begin();
	for (; itor != rSourceTokenVector.end(); ++itor)
	{
		rTokenVector.push_back(*itor);
	}

	return true;
}

void CProperty::PutString(const char *c_pszKey, const char *c_pszString)
{
	std::string stTempKey = c_pszKey;
	stl_lowers(stTempKey);

	CTokenVectorMap::iterator itor = m_stTokenMap.find(stTempKey);

	if (itor != m_stTokenMap.end())
		m_stTokenMap.erase(itor);

	CTokenVector tokenVector;
	tokenVector.push_back(c_pszString);

	m_stTokenMap.insert(CTokenVectorMap::value_type(stTempKey, tokenVector));
}

void CProperty::PutVector(const char *c_pszKey, const CTokenVector & c_rTokenVector)
{
	std::string stTempKey = c_pszKey;
	stl_lowers(stTempKey);

	m_stTokenMap.insert(CTokenVectorMap::value_type(stTempKey, c_rTokenVector));
}

void GetTimeString(char *str, time_t ct)
{
    struct tm tm;
    tm = *localtime(&ct);

    _snprintf(str, 15, "%04d%02d%02d%02d%02d%02d",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec);
}

bool CProperty::Save(const char *c_pszFileName)
{

#ifdef ENABLE_FOXFS_ENCRYPT
	return false;
#endif

	CTempFile file;

	DWORD fourcc = MAKEFOURCC('Y', 'P', 'R', 'T');
	file.Write(&fourcc, sizeof(DWORD));
	file.Write("\r\n", 2);

	if (0 == m_stCRC.length())
	{
		char szCRC[MAX_PATH + 16 + 1];

		GetTimeString(szCRC, time(0));
		strcpy(szCRC + strlen(szCRC), c_pszFileName);

		m_dwCRC = CPropertyManager::Instance().GetUniqueCRC(szCRC);
		_snprintf(szCRC, sizeof(szCRC), "%u", m_dwCRC);

		m_stCRC.assign(szCRC);
	}

	file.Write(m_stCRC.c_str(), m_stCRC.length());
	file.Write("\r\n", 2);

	CTokenVectorMap::iterator itor = m_stTokenMap.begin();
	char buf[4096 + 1];

	while (itor != m_stTokenMap.end())
	{
		CTokenVector & tokenVector = itor->second;

		int len = _snprintf(buf, sizeof(buf), "%s\t", itor->first.c_str());
		file.Write(buf, len);

		for (DWORD i = 0; i < tokenVector.size(); ++i)
		{
			len = _snprintf(buf, sizeof(buf), "\t\"%s\"", tokenVector[i].c_str());
			file.Write(buf, len);
		}

		file.Write("\r\n", 2);
		++itor;
	}

	file.Close();
	return CPropertyManager::Instance().Put(c_pszFileName, file.GetFileName());
}

bool CProperty::ReadFromMemory(const void * c_pvData, int iLen, const char *c_pszFileName)
{
	const char *pcData = (const char *) c_pvData;

	if (*(DWORD *) pcData != MAKEFOURCC('Y', 'P', 'R', 'T'))
		return false;

	pcData += sizeof(DWORD);

	int skip = 0;
	if (*pcData == '\r' && *(pcData + 1) == '\n')
		skip = 2;
	else if (*pcData == '\n' || *pcData == '\r')
		skip = 1;
	else
	{
		TraceError("CProperty::ReadFromMemory: File format error after FourCC: %s\n", c_pszFileName);
		return false;
	}

	pcData += skip;

	CTokenVector stTokenVector;

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(iLen - (sizeof(DWORD) + skip), pcData);

	m_stCRC = textFileLoader.GetLineString(0);
	m_dwCRC = (DWORD)_strtoui64(m_stCRC.c_str(), nullptr, 10);

	for (DWORD i = 1; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &stTokenVector))
			continue;

		stl_lowers(stTokenVector[0]);
		std::string stKey = stTokenVector[0];

		stTokenVector.erase(stTokenVector.begin());
		PutVector(stKey.c_str(), stTokenVector);
	}

	return true;
}

void CProperty::Clear()
{
	m_stTokenMap.clear();
}

#ifdef ENABLE_FOXFS_ENCRYPT
bool CProperty::ReadFromXML(const char *c_pszCRC)
{
	m_dwCRC = (DWORD)_strtoui64(c_pszCRC, nullptr, 10);

	return true;
}
#endif
