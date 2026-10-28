#include "stdafx.h"
#include "StringCodec.h"
#include "StringCodec_Vietnamese.h"

int Ymir_WideCharToMultiByte(
  UINT CodePage,
  DWORD dwFlags,
  LPCWSTR lpWideCharStr,
  int cchWideChar,
  LPSTR lpMultiByteStr,
  int cbMultiByte,
  LPCSTR lpDefaultChar,
  LPBOOL lpUsedDefaultChar
)
{
	if (CodePage == CP_1258)
	{
		return EL_String_Encode_Vietnamese(lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte);
	}
	else
	{
		return WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);
	}
}

int Ymir_MultiByteToWideChar(
  UINT CodePage,
  DWORD dwFlags,
  LPCSTR lpMultiByteStr,
  int cbMultiByte,
  LPWSTR lpWideCharStr,
  int cchWideChar
)
{
	if (CodePage == CP_1258)
	{
		return EL_String_Decode_Vietnamese(lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
	}
	else
	{
		if (cbMultiByte != 0 && lpMultiByteStr)
		{
			int srcLen = (cbMultiByte < 0) ? (int)strlen(lpMultiByteStr) : cbMultiByte;
			bool bHasNonAscii = false;
			for (int i = 0; i < srcLen; ++i)
			{
				if ((unsigned char)lpMultiByteStr[i] >= 0x80)
				{
					bHasNonAscii = true;
					break;
				}
			}

			if (bHasNonAscii)
			{
				int utf8Len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
				if (utf8Len > 0)
					return utf8Len;
			}
		}

		return MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
	}
}