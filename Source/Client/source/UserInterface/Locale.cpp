#include "StdAfx.h"
#include "Locale.h"
#include "PythonApplication.h"
#include "resource.h"

#include "../eterBase/CRC32.h"
#include "../eterpack/EterPackManager.h"
#include "../eterLocale/Japanese.h"
#include <windowsx.h>

const char* LSS_YMIR		= "YMIR";
const char* LSS_JAPAN		= "JAPAN";
const char* LSS_ENGLISH		= "ENGLISH";
const char* LSS_HONGKONG	= "HONGKONG";
const char* LSS_TAIWAN		= "TAIWAN";
const char* LSS_NEWCIBN		= "NEWCIBN";
const char* LSS_EUROPE		= "EUROPE";
const char* LSS_GLOBAL		= "GLOBAL";

static bool IS_CHEONMA = false;

char	MULTI_LOCALE_SERVICE[256]	= "YMIR";
char MULTI_LOCALE_PATH[256] = "locale/ymir";
char MULTI_LOCALE_NAME[256] = "ymir";

#ifdef ENABLE_UTF8_ENCODING
// ENABLE_UTF8_ENCODING aktifken bile Windows-1254 (Turkish) kullan
int LOCALE_CODEPAGE = 1254; // CP_1254 (Turkish Windows-1254)
#else
int LOCALE_CODEPAGE = 65001;
#endif
void LoadConfig(const char *fileName)
{
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	bool bMultiLocale = false;
	const char *szMultiLocale = "locale.cfg";

	if (_access(szMultiLocale, 0) == 0)
	{
		bMultiLocale = true;
		fileName = szMultiLocale;
	}
#endif

	FILE* fp = fopen(fileName, "rt");

	if (fp)
	{
		char line[256];
		char name[256];
		int code;
		int id;

		if (fgets(line, sizeof(line)-1, fp))
		{
			line[sizeof(line)-1] = '\0';

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			if (bMultiLocale)
			{
				sscanf(line, "%d %s", &code, name);
			}
			else
			{
				sscanf(line, "%d %d %s", &id, &code, name);
			}
#else
			sscanf(line, "%d %d %s", &id, &code, name);
#endif

			LOCALE_CODEPAGE = code;
#ifdef ENABLE_UTF8_ENCODING
			// ENABLE_UTF8_ENCODING aktifken UTF-8 (65001) yerine Windows-1254 (1254) kullan
			if (LOCALE_CODEPAGE == 65001)
				LOCALE_CODEPAGE = 1254; // CP_1254 (Turkish Windows-1254)
#endif
			strncpy_s(MULTI_LOCALE_NAME, sizeof(MULTI_LOCALE_NAME), name, _TRUNCATE);
			snprintf(MULTI_LOCALE_PATH, sizeof(MULTI_LOCALE_PATH), "locale/%s", MULTI_LOCALE_NAME);
		}
		fclose(fp);
	}
}

unsigned GetGuildLastExp(int level)
{
	static const int GUILD_LEVEL_MAX = 20;
	static DWORD GUILD_EXP_LIST[GUILD_LEVEL_MAX + 1] =
	{
		0,
		6000UL,
		18000UL,
		36000UL,
		64000UL,
		94000UL,
		130000UL,
		172000UL,
		220000UL,
		274000UL,
		334000UL,
		400000UL,
		600000UL,
		840000UL,
		1120000UL,
		1440000UL,
		1800000UL,
		2600000UL,
		3200000UL,
		4000000UL,
		16800000UL,
	};

	if (level < 0 && level >= GUILD_LEVEL_MAX)
	{
		return 0;
	}

	return GUILD_EXP_LIST[level];
}

int GetSkillPower(unsigned level)
{
	static const unsigned SKILL_POWER_NUM = 50;
	if (level >= SKILL_POWER_NUM)
	{
		return 0;
	}

	static unsigned SKILL_POWERS[SKILL_POWER_NUM] =
	{
		0,
		5,  6,  8, 10, 12,
		14, 16, 18, 20, 22,
		24, 26, 28, 30, 32,
		34, 36, 38, 40, 50,
		52, 54, 56, 58, 60,
		63, 66, 69, 72, 82,
		85, 88, 91, 94, 98,
		102, 106, 110, 115, 125,
		125,
	};

	return SKILL_POWERS[level];
}

unsigned int GetCodePage()
{
#ifdef ENABLE_UTF8_ENCODING
	// ENABLE_UTF8_ENCODING aktifken bile Windows-1254 (Turkish) kullan
	// Eðer locale.cfg dosyasýndan farklý bir deðer okunmuþsa onu kullan, yoksa CP_1254 kullan
	if (LOCALE_CODEPAGE == 65001)
		return 1254; // CP_1254 (Turkish Windows-1254)
	return LOCALE_CODEPAGE;
#else
	return LOCALE_CODEPAGE;
#endif
}

int StringCompareCI(LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength)
{
	return strnicmp (szStringLeft, szStringRight, sizeLength);
}

const char *LocaleService_GetLocaleName()
{
	return MULTI_LOCALE_NAME;
}

const char *LocaleService_GetLocalePath()
{
	return MULTI_LOCALE_PATH;
}

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
BYTE LocaleService_GetLocaleID()
{
	const char *c_szLocale = LocaleService_GetLocale();
	if (strcmp(c_szLocale, "en") == 0)
		return CPythonApplication::LOCALE_EN;
	else if (strcmp(c_szLocale, "ro") == 0)
		return CPythonApplication::LOCALE_RO;
	else if (strcmp(c_szLocale, "pt") == 0)
		return CPythonApplication::LOCALE_PT;
	else if (strcmp(c_szLocale, "es") == 0)
		return CPythonApplication::LOCALE_ES;
	else if (strcmp(c_szLocale, "fr") == 0)
		return CPythonApplication::LOCALE_FR;
	else if (strcmp(c_szLocale, "de") == 0)
		return CPythonApplication::LOCALE_DE;
	else if (strcmp(c_szLocale, "pl") == 0)
		return CPythonApplication::LOCALE_PL;
	else if (strcmp(c_szLocale, "it") == 0)
		return CPythonApplication::LOCALE_IT;
	else if (strcmp(c_szLocale, "cz") == 0)
		return CPythonApplication::LOCALE_CZ;
	else if (strcmp(c_szLocale, "hu") == 0)
		return CPythonApplication::LOCALE_HU;
	else if (strcmp(c_szLocale, "tr") == 0)
		return CPythonApplication::LOCALE_TR;
	else
		return CPythonApplication::LOCALE_DEFAULT;
}

const char *LocaleService_GetLocale() { return MULTI_LOCALE_NAME; }
bool LocaleService_SaveLocale(int iCodePage, const char *szLocale)
{
	LOCALE_CODEPAGE = iCodePage;
	strncpy_s(MULTI_LOCALE_NAME, sizeof(MULTI_LOCALE_NAME), szLocale, _TRUNCATE);
	snprintf(MULTI_LOCALE_PATH, sizeof(MULTI_LOCALE_PATH), "locale/%s", MULTI_LOCALE_NAME);

	SetDefaultCodePage(iCodePage);

	FILE* File;

	if (NULL == (File = fopen("locale.cfg", "wt")))
		return false;

	fprintf(File, "%d %s", iCodePage, szLocale);
	fclose(File);
	return true;
}
#endif

const char* LocaleService_GetName()
{
	return MULTI_LOCALE_SERVICE;
}
