#include "stdafx.h"
#include "locale_service.h"
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
#include "../../common/length.h"
#include "../../common/VnumHelper.h"
#include "CsvReader.h"
#endif

typedef std::map< std::string, std::string > LocaleStringMapType;
typedef std::map<std::string, std::string> LocaleStringMapType;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
typedef std::map<DWORD, std::string> LocaleQuestTranslateMapType;
typedef std::map<DWORD, std::string> LocaleItemMapType;
typedef std::map<DWORD, std::string> LocaleMobMapType;
typedef std::map<DWORD, std::string> LocaleSkillMapType;
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
LocaleQuestTranslateMapType localeQuestTranslate[LOCALE_MAX_NUM];
LocaleStringMapType localeString[LOCALE_MAX_NUM];
LocaleItemMapType localeItem[LOCALE_MAX_NUM];
LocaleMobMapType localeMob[LOCALE_MAX_NUM];
LocaleSkillMapType localeSkill[LOCALE_MAX_NUM];
#else
LocaleStringMapType localeString;
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
const char* get_locale(BYTE locale)
{
	switch (locale)
	{
	case LOCALE_EUROPE:
		return "tr";
	case LOCALE_EN:
		return "en";
	case LOCALE_PT:
		return "pt";
	case LOCALE_ES:
		return "es";
	case LOCALE_FR:
		return "fr";
	case LOCALE_DE:
		return "de";
	case LOCALE_RO:
		return "ro";
	case LOCALE_PL:
		return "pl";
	case LOCALE_IT:
		return "it";
	case LOCALE_CZ:
		return "cz";
	case LOCALE_HU:
		return "hu";
	case LOCALE_TR:
		return "tr";
	default:
		return "en";
	}
}

void locale_add(const char** strings, BYTE locale)
{
	LocaleStringMapType::const_iterator iter = localeString[locale].find(strings[0]);

	if (iter == localeString[locale].end())
	{
		localeString[locale].insert(std::make_pair(strings[0], strings[1]));
	}
}

const char* locale_find(const char* string, BYTE locale)
{
	if (!*string || locale == LC_EUROPE)
		return string;

	if (locale > LOCALE_MAX_NUM)
		locale = LOCALE_EN;

	LocaleStringMapType::const_iterator iter = localeString[locale].find(string);

	if (iter == localeString[locale].end())
	{
		// sys_err("LOCALE_ERROR: \"%s\";", string);
		return string;
	}

	return iter->second.c_str();
}

const char* locale_quest_translate_find(DWORD vnum, BYTE locale)
{
	if (locale > LOCALE_MAX_NUM)
		locale = LOCALE_EN;

	LocaleQuestTranslateMapType::const_iterator iter = localeQuestTranslate[locale].find(vnum);

	if (iter == localeQuestTranslate[locale].end())
	{
		return "NoName";
	}

	return iter->second.c_str();
}

const char* locale_item_find(DWORD vnum, BYTE locale)
{
	if (locale > LOCALE_MAX_NUM)
		locale = LOCALE_EN;

	LocaleItemMapType::const_iterator iter = localeItem[locale].find(vnum);

	if (iter == localeItem[locale].end())
	{
		return "NoName";
	}

	return iter->second.c_str();
}

const char* locale_mob_find(DWORD vnum, BYTE locale)
{
	if (locale > LOCALE_MAX_NUM)
		locale = LOCALE_EN;

	LocaleMobMapType::const_iterator iter = localeMob[locale].find(vnum);

	if (iter == localeMob[locale].end())
		return "NoName";

	return iter->second.c_str();
}

const char* locale_skill_find(DWORD vnum, BYTE locale)
{
	if (locale > LOCALE_MAX_NUM)
		locale = LOCALE_EN;

	LocaleSkillMapType::const_iterator iter = localeSkill[locale].find(vnum);

	if (iter == localeSkill[locale].end())
		return "NoName";

	return iter->second.c_str();
}

void locale_quest_translate_init(const char* filename, BYTE locale)
{
	cCsvTable nameData;

	if (!nameData.Load(filename, '\t'))
	{
		fprintf(stderr, "%s couldn't be loaded or its format is incorrect.\n", filename);
		return;
	}

	nameData.Next();

	while (nameData.Next())
	{
		if (nameData.ColCount() < 2)
			continue;

		localeQuestTranslate[locale].insert(std::make_pair(atoi(nameData.AsStringByIndex(0)), nameData.AsStringByIndex(1)));
	}

	nameData.Destroy();
}

void locale_item_init(const char* filename, BYTE locale)
{
	cCsvTable nameData;

	if (!nameData.Load(filename, '\t'))
	{
		fprintf(stderr, "%s couldn't be loaded or its format is incorrect.\n", filename);
		return;
	}

	nameData.Next();

	DWORD dwVnum;
	while (nameData.Next())
	{
		if (nameData.ColCount() < 2)
			continue;

		dwVnum = atoi(nameData.AsStringByIndex(0));
		if (CItemVnumHelper::IsDragonSoul(dwVnum) == true)
		{
			for (DWORD VNUM = dwVnum; VNUM < (dwVnum + 100); ++VNUM)
			{
				localeItem[locale].insert(std::make_pair(VNUM, nameData.AsStringByIndex(1)));
			}
		}
		else
		{
			localeItem[locale].insert(std::make_pair(dwVnum, nameData.AsStringByIndex(1)));
		}
	}

	nameData.Destroy();
}

void locale_mob_init(const char* filename, BYTE locale)
{
	cCsvTable nameData;

	if (!nameData.Load(filename, '\t'))
	{
		fprintf(stderr, "%s couldn't be loaded or its format is incorrect.\n", filename);
		return;
	}

	nameData.Next();

	while (nameData.Next())
	{
		if (nameData.ColCount() < 2)
			continue;

		localeMob[locale].insert(std::make_pair(atoi(nameData.AsStringByIndex(0)), nameData.AsStringByIndex(1)));
	}

	nameData.Destroy();
}

void locale_skill_init(const char* filename, BYTE locale)
{
	cCsvTable nameData;

	if (!nameData.Load(filename, '\t'))
	{
		fprintf(stderr, "%s couldn't be loaded or its format is incorrect.\n", filename);
		return;
	}

	nameData.Next();

	while (nameData.Next())
	{
		if (nameData.ColCount() < 2)
			continue;

		localeSkill[locale].insert(std::make_pair(atoi(nameData.AsStringByIndex(0)), nameData.AsStringByIndex(1)));
	}

	nameData.Destroy();
}

#else
void locale_add(const char** strings)
{
	LocaleStringMapType::const_iterator iter = localeString.find(strings[0]);

	if (iter == localeString.end())
	{
		localeString.insert(std::make_pair(strings[0], strings[1]));
	}
}

const char* locale_find(const char* string)
{
	if (0 == g_iUseLocale || LC_IsKorea() || LC_IsWE_Korea())
		return (string);

	LocaleStringMapType::const_iterator iter = localeString.find(string);

	if (iter == localeString.end())
	{
		static char s_line[1024] = "@0949";
		strlcpy(s_line + 5, string, sizeof(s_line) - 5);

		sys_err("LOCALE_ERROR: \"%s\";", string);
		return s_line;
	}

	return iter->second.c_str();
}
#endif

const char *quote_find_end(const char *string)
{
	const char  *tmp = string;
	int         quote = 0;

	while (*tmp)
	{
		if (quote && *tmp == '\\' && *(tmp + 1))
		{
			switch (*(tmp + 1))
			{
				case '"':
					tmp += 2;
					continue;
			}
		}
		else if (*tmp == '"')
		{
			quote = !quote;
		}
		else if (!quote && *tmp == ';')
			return (tmp);

		tmp++;
	}

	return (NULL);
}

char *locale_convert(const char *src, int len)
{
	const char	*tmp;
	int		i, j;
	char	*buf, *dest;
	int		start = 0;
	char	last_char = 0;

	if (!len)
		return NULL;

	buf = M2_NEW char[len + 1];

	for (j = i = 0, tmp = src, dest = buf; i < len; i++, tmp++)
	{
		if (*tmp == '"')
		{
			if (last_char != '\\')
				start = !start;
			else
				goto ENCODE;
		}
		else if (*tmp == ';')
		{
			if (last_char != '\\' && !start)
				break;
			else
				goto ENCODE;
		}
		else if (start)
		{
ENCODE:
			if (*tmp == '\\' && *(tmp + 1) == 'n')
			{
				*(dest++) = '\n';
				tmp++;
				last_char = '\n';
			}
			else
			{
				*(dest++) = *tmp;
				last_char = *tmp;
			}

			j++;
		}
	}

	if (!j)
	{
		M2_DELETE_ARRAY(buf);
		return NULL;
	}

	*dest = '\0';
	return (buf);
}

#define NUM_LOCALES 2

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
void locale_init(const char* filename, BYTE locale)
#else
void locale_init(const char* filename)
#endif
{
	FILE        *fp = fopen(filename, "rb");
	char        *buf;

	if (!fp) return;

	fseek(fp, 0L, SEEK_END);
	int i = ftell(fp); 
	fseek(fp, 0L, SEEK_SET);

	i++;

	buf = M2_NEW char[i];

	memset(buf, 0, i);

	fread(buf, i - 1, sizeof(char), fp);

	fclose(fp);

	const char * tmp;
	const char * end;

	char *	strings[NUM_LOCALES];

	if (!buf)
	{
		sys_err("locale_read: no file %s", filename);
		exit(1);
	}

	tmp = buf;

	do
	{
		for (i = 0; i < NUM_LOCALES; i++)
			strings[i] = NULL;

		if (*tmp == '"')
		{
			for (i = 0; i < NUM_LOCALES; i++)
			{
				if (!(end = quote_find_end(tmp)))
					break;

				strings[i] = locale_convert(tmp, end - tmp);
				tmp = ++end;

				while (*tmp == '\n' || *tmp == '\r' || *tmp == ' ') tmp++;

				if (i + 1 == NUM_LOCALES)
					break;

				if (*tmp != '"')
				{
					sys_err("locale_init: invalid format filename %s", filename);
					break;
				}
			}

			if (strings[0] == NULL || strings[1] == NULL)
				break;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			locale_add((const char**)strings, locale);
#else
			locale_add((const char**)strings);
#endif

			for (i = 0; i < NUM_LOCALES; i++)
				if (strings[i])
					M2_DELETE_ARRAY(strings[i]);
		}
		else
		{
			tmp = strchr(tmp, '\n');

			if (tmp)
				tmp++;
		}
	}
	while (tmp && *tmp);

	M2_DELETE_ARRAY(buf);
}