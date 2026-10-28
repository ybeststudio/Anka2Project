#include "StdAfx.h"
#include "TextBar.h"
#include "../eterlib/Util.h"

#ifdef ENABLE_EMOTICONS_SYSTEM
	#include "ResourceManager.h"
#endif

void CTextBar::__SetFont(int fontSize, bool isBold)
{
	int iCodePage = GetDefaultCodePage();

	LOGFONT logFont;

	memset(&logFont, 0, sizeof(LOGFONT));

	logFont.lfHeight			= fontSize;
	logFont.lfEscapement		= 0;
	logFont.lfOrientation		= 0;

	if (isBold)
		logFont.lfWeight			= FW_BOLD;
	else
		logFont.lfWeight			= FW_NORMAL;

	logFont.lfItalic			= FALSE;
	logFont.lfUnderline			= FALSE;
	logFont.lfStrikeOut			= FALSE;
	logFont.lfCharSet			= GetCharsetFromCodePage(iCodePage);
	logFont.lfOutPrecision		= OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
	logFont.lfQuality			= ANTIALIASED_QUALITY;
	logFont.lfPitchAndFamily	= DEFAULT_PITCH;
	strcpy(logFont.lfFaceName, GetFontFaceFromCodePage(iCodePage));
	m_hFont = CreateFontIndirect(&logFont);


	HDC hdc = m_dib.GetDCHandle();
	m_hOldFont = (HFONT)SelectObject(hdc, m_hFont);

}

void CTextBar::SetTextColor(int r, int g, int b)
{
	HDC hDC = m_dib.GetDCHandle();
	::SetTextColor(hDC, RGB(r, g, b));
}

void CTextBar::GetTextExtent(const char *c_szText, SIZE* p_size)
{
	HDC hDC = m_dib.GetDCHandle();
	GetTextExtentPoint32(hDC, c_szText, strlen(c_szText), p_size);
}

#include <regex>
void CTextBar::TextOut(int ix, int iy, const char *c_szText)
{
	const auto x = std::regex_replace(c_szText, std::regex("\\|c[a-zA-Z0-9]+|\\|[r|R|H|h]"), "");
	c_szText = x.c_str();

	if (m_isBold)
	{
		SIZE size{ 0,0 };
		GetTextExtent(c_szText, &size);
		ix = (500 - size.cx) / 2;
	}

	m_dib.TextOut(ix, iy, c_szText);
#ifdef ENABLE_EMOTICONS_SYSTEM
	c_szText = Emojis(ix, iy, c_szText);
#endif
	Invalidate();
}

// #ifdef ENABLE_UTF8_ENCODING
// #include <regex>
// #include <codecvt>
// void CTextBar::TextOut(int ix, int iy, const char *c_szText)
// {
	// std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	// std::wstring wideText = converter.from_bytes(c_szText);

	// const auto x = std::regex_replace(converter.to_bytes(wideText), std::regex("\\|c[a-zA-Z0-9]+|\\|[r|R|H|h]"), "");
	// wideText = converter.from_bytes(x);

	// if (m_isBold)
	// {
		// SIZE size{ 0,0 };
		// GetTextExtent(c_szText, &size);
		// ix = (500 - size.cx) / 2;
	// }

	// m_dib.TextOut(ix, iy, wideText.c_str());
// #ifdef ENABLE_EMOTICONS_SYSTEM
	// c_szText = Emojis(ix, iy, wideText.c_str());
// #endif
	// Invalidate();
// }
// #endif

#ifdef ENABLE_EMOTICONS_SYSTEM
const char* CTextBar::Emojis(int ix, int iy, const char* c_szText)
{
	std::string str1e = c_szText;
	std::string find_s1e = "|T";
	std::size_t find_z1e = 0;
	for (int i = 0; i < str1e.length(); i++)
	{
		find_z1e = str1e.find(find_s1e, i);
		if (find_z1e != std::string::npos)
		{
			str1e = str1e.replace(str1e.find(find_s1e), find_s1e.length(), "");
			c_szText = str1e.c_str();
		}
	}

	std::string str2e = c_szText;
	std::string find_s2e = "|t";
	std::size_t find_z2e = 0;
	for (int i = 0; i < str2e.length(); i++)
	{
		find_z2e = str2e.find(find_s2e, i);
		if (find_z2e != std::string::npos)
		{
			str2e = str2e.replace(str2e.find(find_s2e), find_s2e.length(), "");
			c_szText = str2e.c_str();
		}
	}

	return c_szText;
}
#endif

void CTextBar::OnCreate()
{
	m_dib.SetBkMode(TRANSPARENT);

	__SetFont(m_fontSize, m_isBold);
}

CTextBar::CTextBar(int fontSize, bool isBold)
{
	m_hOldFont = nullptr;
	m_fontSize = fontSize;
	m_isBold = isBold;

}

CTextBar::~CTextBar()
{
	HDC hdc = m_dib.GetDCHandle();
	SelectObject(hdc, m_hOldFont);
}
