#pragma once

#include "DibBar.h"

#ifdef ENABLE_EMOTICONS_SYSTEM
	#include "GrpImageInstance.h"
#endif

class CTextBar : public CDibBar
{
	public:
		CTextBar(int fontSize, bool isBold);
		virtual ~CTextBar();

		void TextOut(int ix, int iy, const char *c_szText);
		void SetTextColor(int r, int g, int b);
		void GetTextExtent(const char *c_szText, SIZE* p_size);

#ifdef ENABLE_EMOTICONS_SYSTEM
		const char *Emojis(int ix, int iy, const char *c_szText);
#endif

	protected:
		void __SetFont(int fontSize, bool isBold);

		void OnCreate();

#ifdef ENABLE_EMOTICONS_SYSTEM
		struct SEmoticon
		{
			short t;
			CGraphicImageInstance *pInstance;
			SEmoticon() : t(0)
			{
				pInstance = nullptr;
			}
		};
#endif

	protected:
		HFONT m_hFont;
		HFONT m_hOldFont;

		int m_fontSize;
		bool m_isBold;

#ifdef ENABLE_EMOTICONS_SYSTEM
	private:
		std::vector<SEmoticon> m_emoticonVector;
#endif
};
