#include "StdAfx.h"

#ifdef ENABLE_CONFIG_MODULE
#include "PythonConfig.h"

CPythonConfig::CPythonConfig()
{
	m_pkIniFile = nullptr;
}
CPythonConfig::~CPythonConfig()
{
	if (m_pkIniFile)
	{
		delete m_pkIniFile;
		m_pkIniFile = nullptr;
	}
}

void CPythonConfig::Initialize(const std::string& szFileName)
{
	if (m_pkIniFile)
	{
		if (m_stFileName == szFileName)
			return;

		delete m_pkIniFile;
	}

	m_pkIniFile = new minIni(szFileName);
	m_stFileName = szFileName;
}

std::string CPythonConfig::GetClassNameByType(EClassTypes eType) const
{
	switch (eType)
	{
		case CLASS_GENERAL:
			return "GENERAL";
			break;

		case CLASS_CHAT:
			return "CHAT";
			break;

		case CLASS_OPTION:
			return "OPTION";
			break;

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		case CLASS_SKILL_COLOR:
			return "SKILL_COLOR";
			break;
#endif
	}

	TraceError("unkown class type %u", eType);
	return "UNKOWN";
}

void CPythonConfig::Write(EClassTypes eType, const std::string& stKey, const std::string& stValue)
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to write key %s with value %s", stKey.c_str(), stValue.c_str());
		return;
	}

	m_pkIniFile->put(GetClassNameByType(eType), stKey, stValue);
}
void CPythonConfig::Write(EClassTypes eType, const std::string& stKey, const char *szValue)
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to write key %s with value %s", stKey.c_str(), szValue);
		return;
	}

	m_pkIniFile->put(GetClassNameByType(eType), stKey, szValue);
}
void CPythonConfig::Write(EClassTypes eType, const std::string& stKey, int iValue)
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to write key %s with value %d", stKey.c_str(), iValue);
		return;
	}

	m_pkIniFile->put(GetClassNameByType(eType), stKey, iValue);
}
void CPythonConfig::Write(EClassTypes eType, const std::string& stKey, bool bValue)
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to write key %s with value %d", stKey.c_str(), bValue);
		return;
	}

	m_pkIniFile->put(GetClassNameByType(eType), stKey, bValue);
}

std::string CPythonConfig::GetString(EClassTypes eType, const std::string& stKey, const std::string& stDefaultValue) const
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to read key %s", stKey.c_str());
		return stDefaultValue;
	}

	return m_pkIniFile->gets(GetClassNameByType(eType), stKey, stDefaultValue);
}
int CPythonConfig::GetInteger(EClassTypes eType, const std::string& stKey, int iDefaultValue) const
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to read key %s", stKey.c_str());
		return iDefaultValue;
	}

	return m_pkIniFile->geti(GetClassNameByType(eType), stKey, iDefaultValue);
}
bool CPythonConfig::GetBool(EClassTypes eType, const std::string& stKey, bool bDefaultValue) const
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to read key %s", stKey.c_str());
		return bDefaultValue;
	}

	return m_pkIniFile->getbool(GetClassNameByType(eType), stKey, bDefaultValue);
}

void CPythonConfig::RemoveSection(EClassTypes eType)
{
	if (!m_pkIniFile)
	{
		TraceError("not initialized when trying to remove type %u", eType);
		return;
	}

	m_pkIniFile->del(GetClassNameByType(eType));
}
#endif
