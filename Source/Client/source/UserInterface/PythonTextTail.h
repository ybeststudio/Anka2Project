#pragma once

#include "StdAfx.h"

#include "../eterBase/Singleton.h"

class CPythonTextTail : public CSingleton<CPythonTextTail>
{
	public:
		typedef struct STextTail
		{
			CGraphicTextInstance* pTextInstance;
			CGraphicTextInstance* pOwnerTextInstance;

			CGraphicMarkInstance* pMarkInstance;
			CGraphicTextInstance* pGuildNameTextInstance;
            std::string optionGuildBase;
            DWORD optionGuildID = 0;
            BYTE optionGuildLeader = 0;

			CGraphicTextInstance* pTitleTextInstance;
			CGraphicTextInstance* pLevelTextInstance;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			CGraphicImageInstance* pLanguageInstance;
			CGraphicImageInstance* pLanguageInstance2;
#endif
#ifdef ENABLE_SHOW_MOB_INFO
			CGraphicTextInstance* pAIFlagTextInstance;
#endif

			CGraphicObjectInstance* pOwner;

			DWORD dwVirtualID;

			float x, y, z;
			float fDistanceFromPlayer;
			D3DXCOLOR Color;
			BOOL bNameFlag;

			float xStart, yStart;
			float xEnd, yEnd;

			DWORD LivingTime;

			float fHeight;

#ifdef ENABLE_SHOW_MOB_INFO
			BOOL bIsPC;
#endif

			STextTail() {}
			virtual ~STextTail() {}
		} TTextTail;

		typedef std::map<DWORD, TTextTail*> TTextTailMap;
		typedef std::list<TTextTail*> TTextTailList;
		typedef TTextTailMap TChatTailMap;

	public:
		CPythonTextTail(void);
		virtual ~CPythonTextTail(void);

		void GetInfo(std::string* pstInfo);

		void Initialize();
		void Destroy();
		void Clear();

		void UpdateAllTextTail();
		void UpdateShowingTextTail();
		void Render();

		void ArrangeTextTail();
        void RefreshGuildGrades();
		void HideAllTextTail();
		void ShowAllTextTail();
		void ShowCharacterTextTail(DWORD VirtualID);
		void ShowItemTextTail(DWORD VirtualID);

#ifdef ENABLE_GUILD_LEADER_TEXTAIL
		void RegisterCharacterTextTail(DWORD dwGuildID, BYTE m_dwGuildLeader, DWORD dwVirtualID, const D3DXCOLOR & c_rColor, float fAddHeight = 10.0f);
#else
		void RegisterCharacterTextTail(DWORD dwGuildID, DWORD dwVirtualID, const D3DXCOLOR & c_rColor, float fAddHeight = 10.0f);
#endif
		void RegisterItemTextTail(DWORD VirtualID, const char *c_szText, CGraphicObjectInstance* pOwner);
		void RegisterChatTail(DWORD VirtualID, const char *c_szChat);
		void RegisterInfoTail(DWORD VirtualID, const char *c_szChat);
		void SetCharacterTextTailColor(DWORD VirtualID, const D3DXCOLOR & c_rColor);
		void SetItemTextTailOwner(DWORD dwVID, const char *c_szName);
		void DeleteCharacterTextTail(DWORD VirtualID);
		void DeleteItemTextTail(DWORD VirtualID);

		int Pick(int ixMouse, int iyMouse);
		void SelectItemName(DWORD dwVirtualID);

		bool GetTextTailPosition(DWORD dwVID, float *px, float *py, float *pz);
		bool IsChatTextTail(DWORD dwVID);

		void EnablePKTitle(BOOL bFlag);

		void AttachTitle(DWORD dwVID, const char *c_szName, const D3DXCOLOR& c_rColor);
		void DetachTitle(DWORD dwVID);

		void AttachLevel(DWORD dwVID, const char *c_szText, const D3DXCOLOR& c_rColor);
		void DetachLevel(DWORD dwVID);
#ifdef ENABLE_CONQUEROR_LEVEL
		void AttachConquerorLevel(DWORD dwVID, const char* c_szText);
#endif

	protected:
		TTextTail* RegisterTextTail(DWORD dwVirtualID, const char *c_szText, CGraphicObjectInstance* pOwner, float fHeight, const D3DXCOLOR & c_rColor
#ifdef ENABLE_EMOTICONS_SYSTEM
			, std::string szFont = "Tahoma:12.fnt"
#endif
		);

		void DeleteTextTail(TTextTail* pTextTail);
		void UpdateTextTail(TTextTail* pTextTail);
		void RenderTextTailBox(TTextTail* pTextTail);
		void RenderTextTailName(TTextTail* pTextTail);
		void UpdateDistance(const TPixelPosition & c_rCenterPosition, TTextTail* pTextTail);

		bool isIn(TTextTail* pSource, TTextTail* pTarget);

	protected:
		TTextTailMap m_CharacterTextTailMap;
		TTextTailMap m_ItemTextTailMap;
		TChatTailMap m_ChatTailMap;

		TTextTailList m_CharacterTextTailList;
		TTextTailList m_ItemTextTailList;

	private:
		CDynamicPool<STextTail> m_TextTailPool;
};
