#ifndef __PYTHON_CONFIG_HEADER__
#define __PYTHON_CONFIG_HEADER__

#include "StdAfx.h"

#ifdef ENABLE_CONFIG_MODULE
#include "../../extern/include/minini_12b/minIni.h"

class CPythonConfig : public CSingleton<CPythonConfig>
{
public:
	enum EClassTypes
	{
		CLASS_GENERAL,
		CLASS_CHAT,
		CLASS_OPTION,
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		CLASS_SKILL_COLOR,
#endif
	};

public:
	CPythonConfig();
	~CPythonConfig();
	void Initialize(const std::string& stFileName);

	std::string	GetClassNameByType(EClassTypes eType) const;

	void Write(EClassTypes eType, const std::string& stKey, const std::string& stValue);
	void Write(EClassTypes eType, const std::string& stKey, const char *szValue);
	void Write(EClassTypes eType, const std::string& stKey, int iValue);
	void Write(EClassTypes eType, const std::string& stKey, bool bValue);

	std::string GetString(EClassTypes eType, const std::string& stKey, const std::string& stDefaultValue = "") const;
	int GetInteger(EClassTypes eType, const std::string& stKey, int iDefaultValue = 0) const;
	bool GetBool(EClassTypes eType, const std::string& stKey, bool bDefaultValue = false) const;

	void RemoveSection(EClassTypes eType);

private:
	minIni* m_pkIniFile;
	std::string m_stFileName;
};
#endif

#endif
