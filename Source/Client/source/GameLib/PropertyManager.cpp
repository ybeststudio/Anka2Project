#include "StdAfx.h"
#include "../eterPack/EterPackManager.h"

#include "PropertyManager.h"
#include "Property.h"

#ifdef ENABLE_FOXFS_ENCRYPT
	#include <boost/property_tree/ptree.hpp>
	#include <boost/property_tree/xml_parser.hpp>
	#include <boost/foreach.hpp>
#endif

CPropertyManager::CPropertyManager() : m_isFileMode(true)
{
}

CPropertyManager::~CPropertyManager()
{
	Clear();
}

#ifdef ENABLE_FOXFS_ENCRYPT
bool CPropertyManager::Initialize(const char *c_pszPackFileName)
{
	if (c_pszPackFileName)
	{
		if (CEterPackManager::Instance().isExist("property/reserve"))
		{
			LoadReservedCRC("property/reserve");
		}

		CMappedFile kListFile;
		PBYTE pbListData;
		if (CEterPackManager::Instance().Get(kListFile, "property/list", (LPCVOID*)&pbListData))
		{
			std::stringstream kSS;
			kSS << (char *)pbListData;

			std::string strLine;
			while (std::getline(kSS, strLine))
			{
				std::size_t posRight = strLine.find_last_not_of(" \t\r\n");
				strLine = (posRight == std::string::npos ? "" : strLine.substr(0, posRight + 1));
				std::size_t posLeft = strLine.find_first_not_of(" \t\r\n");
				strLine = (posLeft == std::string::npos ? "" : strLine.substr(posLeft, std::string::npos));

				if (!Register(strLine.c_str(), nullptr))
				{
					//TraceError("CPropertyManager::Initialize: Cannot register property '%s'!", strLine.c_str());
				}
			}
		}

		CMappedFile kPropertyXML;
		PBYTE pbPropertyXML;
		if (CEterPackManager::Instance().Get(kPropertyXML, "property.xml", (LPCVOID*)&pbPropertyXML))
		{
			char *pszXML = new char[kPropertyXML.Size() + 1];
			memcpy(pszXML, pbPropertyXML, kPropertyXML.Size());
			pszXML[kPropertyXML.Size()] = 0;
			std::stringstream kXML;
			kXML << pszXML;
			delete[] pszXML;

			try
			{
				boost::property_tree::ptree kPT;
				read_xml(kXML, kPT);

				for (auto& v : kPT.get_child("PropertyList"))
				{
					if (v.first == "Property")
					{
						CProperty* pProperty = new CProperty(v.second.get<std::string>("<xmlattr>.filename").c_str());
						if (!pProperty->ReadFromXML(v.second.get<std::string>("<xmlattr>.crc").c_str()))
						{
							TraceError("CPropertyManager::Initialize: Cannot register '%s'!", v.second.get<std::string>("<xmlattr>.filename").c_str());
							delete pProperty;
							continue;
						}

						DWORD dwCRC = pProperty->GetCRC();

						TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find(dwCRC);

						if (m_PropertyByCRCMap.end() != itor)
						{
							// Tracef("Property already registered, replace %s to %s\n",
							// 	itor->second->GetFileName(), v.second.get<std::string>("<xmlattr>.filename").c_str());

							delete itor->second;
							itor->second = pProperty;
						}
						else
							m_PropertyByCRCMap.insert(TPropertyCRCMap::value_type(dwCRC, pProperty));

						for (auto& s : v.second)
						{
							if (s.first == "<xmlattr>")
							{
								for (auto& d : s.second)
								{
									CTokenVector kVec;
									kVec.push_back(d.second.data());
									pProperty->PutVector(d.first.c_str(), kVec);
								}
							}
						}
					}
				}
			}
			catch (std::exception& e)
			{
				TraceError("CPropertyManager::Initialize: %s", e.what());
				return false;
			}
		}
	}
	else
	{
		m_isFileMode = true;
	}

	return true;
}

bool CPropertyManager::BuildPack()
{
	return false;
}
#else
bool CPropertyManager::Initialize(const char *c_pszPackFileName)
{
	if (c_pszPackFileName)
	{
		if (!m_pack.Create(m_fileDict, c_pszPackFileName, "", true))
		{
			LogBoxf("Cannot open property pack file (filename %s)", c_pszPackFileName);
			return false;
		}

		m_isFileMode = false;

		TDataPositionMap & indexMap = m_pack.GetIndexMap();

		TDataPositionMap::iterator itor = indexMap.begin();

		typedef std::map<DWORD, TEterPackIndex *> TDataPositionMap;

		int i = 0;

		while (indexMap.end() != itor)
		{
			TEterPackIndex * pIndex = itor->second;
			++itor;

			if (!stricmp("property/reserve", pIndex->filename))
			{
				LoadReservedCRC(pIndex->filename);
				continue;
			}

			if (!Register(pIndex->filename))
				continue;

			++i;
		}
	}
	else
	{
		m_isFileMode = true;
	}

	return true;
}

bool CPropertyManager::BuildPack()
{
	if (!m_pack.Create(m_fileDict, "property", ""))
		return false;

	WIN32_FIND_DATA fdata;
	HANDLE hFind = FindFirstFile("property\\*", &fdata);

	if (hFind == INVALID_HANDLE_VALUE)
		return false;

	do
	{
		if (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			continue;

		char szSourceFileName[256 + 1];
		_snprintf(szSourceFileName, sizeof(szSourceFileName), "property\\%s", fdata.cFileName);

		m_pack.Put(fdata.cFileName, szSourceFileName,COMPRESSED_TYPE_NONE,"");
	}
	while (FindNextFile(hFind, &fdata));

	FindClose(hFind);
	return true;
}
#endif

bool CPropertyManager::LoadReservedCRC(const char *c_pszFileName)
{
	CMappedFile file;
	LPCVOID c_pvData;

	if (!CEterPackManager::Instance().Get(file, c_pszFileName, &c_pvData))
		return false;

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(file.Size(), c_pvData);

	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		const char *pszLine = textFileLoader.GetLineString(i).c_str();

		if (!pszLine || !*pszLine)
			continue;

		ReserveCRC((DWORD)_strtoui64(pszLine, nullptr, 10));
	}

	return true;
}

void CPropertyManager::ReserveCRC(DWORD dwCRC)
{
	m_ReservedCRCSet.insert(dwCRC);
}

DWORD CPropertyManager::GetUniqueCRC(const char *c_szSeed)
{
	std::string stTmp = c_szSeed;

	while (1)
	{
		DWORD dwCRC = GetCRC32(stTmp.c_str(), stTmp.length());

		if (m_ReservedCRCSet.find(dwCRC) == m_ReservedCRCSet.end() &&
			m_PropertyByCRCMap.find(dwCRC) == m_PropertyByCRCMap.end())
			return dwCRC;

		char szAdd[2];
		_snprintf(szAdd, sizeof(szAdd), "%d", random() % 10);
		stTmp += szAdd;
	}
}

bool CPropertyManager::Register(const char *c_pszFileName, CProperty ** ppProperty)
{
	CMappedFile file;
	LPCVOID		c_pvData;

	if (!CEterPackManager::Instance().Get(file, c_pszFileName, &c_pvData))
		return false;

	CProperty * pProperty = new CProperty(c_pszFileName);

	if (!pProperty->ReadFromMemory(c_pvData, file.Size(), c_pszFileName))
	{
		delete pProperty;
		return false;
	}

	DWORD dwCRC = pProperty->GetCRC();

	TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find(dwCRC);

	if (m_PropertyByCRCMap.end() != itor)
	{
		// Tracef("Property already registered, replace %s to %s\n",
		// 		itor->second->GetFileName(),
		// 		c_pszFileName);

		delete itor->second;
		itor->second = pProperty;
	}
	else
		m_PropertyByCRCMap.insert(TPropertyCRCMap::value_type(dwCRC, pProperty));

	if (ppProperty)
		*ppProperty = pProperty;

	return true;
}

bool CPropertyManager::Get(const char *c_pszFileName, CProperty ** ppProperty)
{
	return Register(c_pszFileName, ppProperty);
}

bool CPropertyManager::Get(DWORD dwCRC, CProperty ** ppProperty)
{
	TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find(dwCRC);

	if (m_PropertyByCRCMap.end() == itor)
		return false;

	*ppProperty = itor->second;
	return true;
}

#ifdef ENABLE_FOXFS_ENCRYPT
bool CPropertyManager::Put(const char *c_pszFileName, const char *c_pszSourceFileName)
{
	if (!CopyFile(c_pszSourceFileName, c_pszFileName, FALSE))
		return false;

	if (!m_isFileMode)
	{
		if (!m_pack.Put(c_pszFileName, nullptr, COMPRESSED_TYPE_NONE,""))
		{
			assert(!"CPropertyManager::Put cannot write to pack file");
			return false;
		}
	}

	Register(c_pszFileName);
	return true;
}

bool CPropertyManager::Erase(DWORD dwCRC)
{
	TPropertyCRCMap::iterator itor = m_PropertyByCRCMap.find(dwCRC);

	if (m_PropertyByCRCMap.end() == itor)
		return false;

	CProperty * pProperty = itor->second;
	m_PropertyByCRCMap.erase(itor);

	DeleteFile(pProperty->GetFileName());
	ReserveCRC(pProperty->GetCRC());

	if (!m_isFileMode)
		m_pack.Delete(pProperty->GetFileName());

	FILE * fp = fopen("property/reserve", "a+");

	if (!fp)
		LogBox("Cannot open the CRC file 'property/reserve'.");
	else
	{
		char szCRC[64 + 1];
		_snprintf(szCRC, sizeof(szCRC), "%u\r\n", pProperty->GetCRC());

		fputs(szCRC, fp);
		fclose(fp);
	}

	delete pProperty;
	return true;
}

bool CPropertyManager::Erase(const char *c_pszFileName)
{
	CProperty * pProperty = nullptr;

	if (Get(c_pszFileName, &pProperty))
		return Erase(pProperty->GetCRC());

	return false;
}
#endif

void CPropertyManager::Clear()
{
	stl_wipe_second(m_PropertyByCRCMap);
}
