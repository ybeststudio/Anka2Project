#include "stdafx.h"
#include "empire_text_convert.h"

namespace 
{
	struct STextConvertTable 
	{
		char acUpper[26];
		char acLower[26];
		BYTE aacHan[5000][2];
		BYTE aacJaum[50][2];
		BYTE aacMoum[50][2];
	} g_aTextConvTable[3];
}

bool LoadEmpireTextConvertTable(DWORD dwEmpireID, const char* c_szFileName)
{
	if (dwEmpireID < 1 || dwEmpireID > 3)
		return false;

	FILE * fp = fopen(c_szFileName, "rb");

	if (!fp)
		return false;

	DWORD dwEngCount = 26;
	DWORD dwHanCount = (0xC8 - 0xB0+1) * (0xFE - 0xA1+1);
	DWORD dwHanSize = dwHanCount * 2;

	STextConvertTable& rkTextConvTable=g_aTextConvTable[dwEmpireID-1];

	fread(rkTextConvTable.acUpper, 1, dwEngCount, fp);
	fread(rkTextConvTable.acLower, 1, dwEngCount, fp);
	fread(rkTextConvTable.aacHan, 1, dwHanSize, fp);

	fread(rkTextConvTable.aacJaum, 1, 60, fp);
	fread(rkTextConvTable.aacMoum, 1, 42, fp);

	fclose(fp);

	return true;
}

void ConvertEmpireText(DWORD dwEmpireID, char* szText, size_t len, int iPct)
{
#ifndef DISABLE_CONVERT_EMPIRE_TEXT
	return;
#else
	if (dwEmpireID < 1 || dwEmpireID > 3 || len == 0)
		return;

	const STextConvertTable& rkTextConvTable = g_aTextConvTable[dwEmpireID - 1];

	for (BYTE* pbText = reinterpret_cast<BYTE*>(szText) ; len > 0 && *pbText != '\0' ; --len, ++pbText)
	{
		if (number(1,100) > iPct)
		{
			if (*pbText & 0x80)
			{
				static char s_cChinaTable[][3] = {"¡ò","££","£¤","¡ù","¡?" };
				int n = number(0, 4);
				pbText[0] = s_cChinaTable[n][0];
				pbText[1] = s_cChinaTable[n][1];

				++pbText;
				--len;
			}
			else
			{
				if (*pbText >= 'a' && *pbText <= 'z')
				{
					*pbText = rkTextConvTable.acLower[*pbText - 'a'];
				}
				else if (*pbText >= 'A' && *pbText <= 'Z')
				{
					*pbText = rkTextConvTable.acUpper[*pbText - 'A'];
				}
			}
		}
		else
		{
			if (*pbText & 0x80)
			{
				++pbText;
				--len;
			}
		}
	}
#endif
}

