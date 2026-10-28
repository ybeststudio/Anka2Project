#ifndef __INC_CRC32_H__
#define __INC_CRC32_H__

#include "StdAfx.h"
#include <windows.h>

DWORD GetCRC32(const char *buffer, size_t count);
#ifdef ENABLE_SKILL_COLOR_SYSTEM
DWORD GetCaseCRC32(const char *buf, size_t len, const char *name = nullptr);
#else
DWORD GetCaseCRC32(const char *buf, size_t len);
#endif
DWORD GetHFILECRC32(HANDLE hFile);
DWORD GetFileCRC32(const char *c_szFileName);
DWORD GetFileSize(const char *c_szFileName);

#endif
