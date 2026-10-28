#pragma once

#include "StdAfx.h"

bool		LocaleService_IsYMIR();
bool		LocaleService_IsJAPAN();
bool		LocaleService_IsENGLISH();
bool		LocaleService_IsHONGKONG();
bool		LocaleService_IsTAIWAN();
bool		LocaleService_IsNEWCIBN();
bool		LocaleService_IsEUROPE();
bool		LocaleService_IsWorldEdition();

unsigned GetCodePage();
const char* LocaleService_GetName();
int StringCompareCI (LPCSTR szStringLeft, LPCSTR szStringRight, size_t sizeLength);
void LoadConfig(const char *fileName);
unsigned GetGuildLastExp(int level);
int GetSkillPower(unsigned level);

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
BYTE LocaleService_GetLocaleID();
bool LocaleService_SaveLocale(int iCodePage, const char *szLocale);
const char *LocaleService_GetLocale();
#endif

const char *LocaleService_GetLocaleName();
const char *LocaleService_GetLocalePath();