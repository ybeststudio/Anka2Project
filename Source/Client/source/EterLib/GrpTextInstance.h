#ifndef __INC_ETERLIB_GRPTEXTINSTANCE_H__
#define __INC_ETERLIB_GRPTEXTINSTANCE_H__

#include <memory>

#include "StdAfx.h"
#include "Pool.h"
#include "GrpText.h"

#include "../EterBase/Timer.h"

#ifdef ENABLE_TEXTLINE_EMOJI
	#include "GrpImageInstance.h"
#endif

#ifdef ENABLE_EMOTICONS_SYSTEM
	#include "GrpExpandedImageInstance.h"
#endif

class CGraphicTextInstance
{
	public:
		typedef CDynamicPool<CGraphicTextInstance> TPool;

	public:
		enum EHorizontalAlign
		{
			HORIZONTAL_ALIGN_LEFT		= 0x01,
			HORIZONTAL_ALIGN_CENTER		= 0x02,
			HORIZONTAL_ALIGN_RIGHT		= 0x03,
		};
		enum EVerticalAlign
		{
			VERTICAL_ALIGN_TOP		= 0x10,
			VERTICAL_ALIGN_CENTER	= 0x20,
			VERTICAL_ALIGN_BOTTOM	= 0x30
		};

	public:
		static void Hyperlink_UpdateMousePos(int x, int y);
		static int  Hyperlink_GetText(char* buf, int len);

	public:
		CGraphicTextInstance();
		virtual ~CGraphicTextInstance();

		void Destroy();

		void Update();
		void Render(RECT * pClipRect = nullptr);

		// Frame-level batching for many small texts (name tags): between BeginBatch/EndBatch,
		// Render() only collects glyph quads; EndBatch draws them with one call per font page
		// instead of one or more draw calls (and heap allocations) per text.
		static void BeginBatch();
		static void EndBatch();

		void ShowCursor();
		void HideCursor();
#ifdef INSIDE_RENDER
		bool IsShowCursor();
#endif

#ifdef INSIDE_RENDER
		std::string	GetText() { return m_stText; }
#endif
		void ShowOutLine();
		void HideOutLine();

		void SetColor(DWORD color);
		void SetColor(float r, float g, float b, float a = 1.0f);

		void SetOutLineColor(DWORD color);
		void SetOutLineColor(float r, float g, float b, float a = 1.0f);
#ifdef ENABLE_SUNG_MAHI_TOWER
		void GetCharacterWidth(short* sWidth);
#endif
		void SetHorizonalAlign(int hAlign);
		void SetVerticalAlign(int vAlign);
		void SetMax(int iMax);
		void SetTextPointer(CGraphicText* pText);
		void SetValueString(const std::string& c_stValue);
		void SetValue(const char* c_szValue, size_t len = -1);
#ifdef ENABLE_MULTILANGUAGE_CHATTING
		void SetColorRange(DWORD dwColor, int iStart, int iEnd);
		void SetValue(const char* c_szValue, const DWORD* c_pdwCodePage);
		void SetLangID(DWORD dwLangID) { m_dwLangID = dwLangID; }
		DWORD GetLangID() const { return m_dwLangID; }
#endif
		void SetPosition(float fx, float fy, float fz = 0.0f);
		void SetSecret(bool Value);
		void SetOutline(bool Value);
		void SetFeather(bool Value);
		void SetMultiLine(bool Value);
		void SetLimitWidth(float fWidth);

#ifdef INSIDE_RENDER
		void SetFixedRenderPos(WORD startPos, WORD endPos) { m_startPos = startPos; m_endPos = endPos; m_isFixedRenderPos = true; }
		void GetRenderPositions(WORD& startPos, WORD& endPos) { startPos = m_startPos; endPos = m_endPos; }

		//bool IsShowCursor(){return m_isCursor;}

		DWORD GetColor() const { return m_dwTextColor; }

		float GetPositionX() const { return m_v3Position.x; }
		float GetPositionY() const { return m_v3Position.y; }

		void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
		void iSetRenderingRect(int iLeft, int iTop, int iRight, int iBottom);
		void SetRenderBox(RECT& renderBox);
#endif
#ifdef ENABLE_MULTI_TEXTLINE
		void GetMultiTextSize(int* pRetWidth, int* pRetHeight);
		void DisableEnterToken();
		void SetLineHeight(int iHeight);
		float GetTextWidth(int iStart);
#endif

		void GetTextSize(int* pRetWidth, int* pRetHeight);
		const std::string& GetValueStringReference();

		WORD GetTextLineCount();
#ifdef ENABLE_PYTHON_EVENT_FUNCTIONS
		WORD GetLineHeight();
#endif

		int PixelPositionToCharacterPosition(int iPixelPosition);
		int GetHorizontalAlign();

	protected:
		void __Initialize();
		int  __DrawCharacter(CGraphicFontTexture * pFontTexture, WORD codePage, wchar_t text, DWORD dwColor);
		void __GetTextPos(DWORD index, float* x, float* y);
		int __GetTextTag(const wchar_t * src, int maxLen, int & tagLen, std::wstring & extraInfo);

	protected:
		struct SHyperlink
		{
			short sx;
			short ex;
			std::wstring text;

			SHyperlink() : sx(0), ex(0) { }
		};

#ifdef ENABLE_TEXTLINE_EMOJI
		struct SEmoji
		{
			short x;
			CGraphicImageInstance *pInstance;

			SEmoji() : x(0)
			{
				pInstance = nullptr;
			}
		};
#endif

#ifdef ENABLE_EMOTICONS_SYSTEM
		struct SEmoticon
		{
			short t;
			CGraphicExpandedImageInstance *pInstance;

			SEmoticon() : t(0)
			{
				pInstance = nullptr;
			}
		};
#endif

	protected:
		DWORD m_dwTextColor;
		DWORD m_dwOutLineColor;
#ifdef ENABLE_MULTILANGUAGE_CHATTING
		DWORD m_dwCodePage;
		DWORD m_dwLangID;
#endif

		WORD m_textWidth;
		WORD m_textHeight;

		BYTE m_hAlign;
		BYTE m_vAlign;
#ifdef INSIDE_RENDER
		WORD m_startPos, m_endPos;
		bool m_isFixedRenderPos;
		RECT m_renderBox;
#endif

		WORD m_iMax;
		float m_fLimitWidth;

		bool m_isCursor;
		float m_isBlinking;
		bool m_isSecret;
		bool m_isMultiLine;

#ifdef ENABLE_MULTI_TEXTLINE
		bool m_isEnterToken;
		int m_iLineHeight;
		std::vector<std::size_t> m_vLineCutPos;
#endif

		bool m_isOutline;
		float m_fFontFeather;

		std::string m_stText;
		D3DXVECTOR3 m_v3Position;

	private:
		bool m_isUpdate;
		bool m_isUpdateFontTexture;

		CGraphicText::TRef m_roText;
		CGraphicFontTexture::TPCharacterInfomationVector m_pCharInfoVector;
		std::vector<DWORD> m_dwColorInfoVector;
		std::vector<SHyperlink> m_hyperlinkVector;
#ifdef ENABLE_TEXTLINE_EMOJI
		std::vector<SEmoji> m_emojiVector;
#endif
#ifdef ENABLE_EMOTICONS_SYSTEM
		std::vector<SEmoticon> m_emoticonVector;
#endif
#ifdef INSIDE_RENDER
		bool m_bUseRenderingRect;
		RECT m_RenderingRect;
#endif

	public:
		static void CreateSystem(UINT uCapacity);
		static void DestroySystem();

		static CGraphicTextInstance* New();
		static void Delete(CGraphicTextInstance* pkInst);

		static CDynamicPool<CGraphicTextInstance>		ms_kPool;
};

extern const char *FindToken(const char *begin, const char *end);
extern int ReadToken(const char *token);

#endif