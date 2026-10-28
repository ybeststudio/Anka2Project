#include "StdAfx.h"
#include "PythonChat.h"
#include "AbstractApplication.h"
#include "PythonCharacterManager.h"
#include "../eterbase/Timer.h"

#ifdef ENABLE_CONFIG_MODULE
	#include "PythonConfig.h"
#endif

#ifdef ENABLE_EMOTICONS_SYSTEM
	#include "PythonNetworkStream.h"
	#include "PythonPlayer.h"
	#include "../EterLocale/StringCodec.h"

static std::string strChatDefaultFontName = "";
#endif

int CPythonChat::TChatSet::ms_iChatModeSize = CHAT_TYPE_MAX_NUM;

const float c_fStartDisappearingTime = 5.0f;
const int c_iMaxLineCount = 5;

CDynamicPool<CPythonChat::SChatLine> CPythonChat::SChatLine::ms_kPool;

void CPythonChat::SetChatColor(UINT eType, UINT r, UINT g, UINT b)
{
	if (eType>=CHAT_TYPE_MAX_NUM)
		return;

	DWORD dwColor=(0xff000000)|(r<<16)|(g<<8)|(b);
	m_akD3DXClrChat[eType]=D3DXCOLOR(dwColor);
}

CPythonChat::SChatLine* CPythonChat::SChatLine::New()
{
	return ms_kPool.Alloc();
}

void CPythonChat::SChatLine::Delete(CPythonChat::SChatLine* pkChatLine)
{
	pkChatLine->Instance.Destroy();
	ms_kPool.Free(pkChatLine);
}

void CPythonChat::SChatLine::DestroySystem()
{
	ms_kPool.Destroy();
}

void CPythonChat::SChatLine::SetColor(DWORD dwID, DWORD dwColor)
{
	assert(dwID < CHAT_LINE_COLOR_ARRAY_MAX_NUM);

	if (dwID >= CHAT_LINE_COLOR_ARRAY_MAX_NUM)
		return;

	aColor[dwID] = dwColor;
}

void CPythonChat::SChatLine::SetColorAll(DWORD dwColor)
{
	for (int i = 0; i < CHAT_LINE_COLOR_ARRAY_MAX_NUM; ++i)
		aColor[i] = dwColor;
}

D3DXCOLOR & CPythonChat::SChatLine::GetColorRef(DWORD dwID)
{
	assert(dwID < CHAT_LINE_COLOR_ARRAY_MAX_NUM);

	if (dwID >= CHAT_LINE_COLOR_ARRAY_MAX_NUM)
	{
		static D3DXCOLOR color(1.0f, 0.0f, 0.0f, 1.0f);
		return color;
	}

	return aColor[dwID];
}

CPythonChat::SChatLine::SChatLine()
{
	for (int i = 0; i < CHAT_LINE_COLOR_ARRAY_MAX_NUM; ++i)
		aColor[i] = 0xff0000ff;
}
CPythonChat::SChatLine::~SChatLine()
{
	Instance.Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int CPythonChat::CreateChatSet(DWORD dwID)
{
	m_ChatSetMap.insert(std::make_pair(dwID, TChatSet()));
	return dwID;
}

void CPythonChat::UpdateViewMode(DWORD dwID)
{
	IAbstractApplication& rApp=IAbstractApplication::GetSingleton();

	float fcurTime = rApp.GetGlobalTime();

	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	TChatLineList * pLineList = &(dwID == CHAT_SET_LOG_WINDOW ? pChatSet->m_ShowingChatLogLineList : pChatSet->m_ShowingChatLineList);
	int iLineIndex = pLineList->size();
	int iHeight = -(int(pLineList->size()+1) * pChatSet->m_iStep);

	TChatLineList::iterator itor;
	for (itor = pLineList->begin(); itor != pLineList->end();)
	{
		TChatLine * pChatLine = (*itor);

		D3DXCOLOR & rColor = pChatLine->GetColorRef(dwID);

		float fElapsedTime = (fcurTime - pChatLine->fAppendedTime);
		if (fElapsedTime >= c_fStartDisappearingTime || iLineIndex >= c_iMaxLineCount)
		{
			rColor.a -= rColor.a / 10.0f;

			if (rColor.a <= 0.1f)
			{
				itor = pLineList->erase(itor);
			}
			else
			{
				++itor;
			}
		}
		else
		{
			++itor;
		}

		iHeight += pChatSet->m_iStep;
		--iLineIndex;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		int iWidth = pChatSet->m_ix;
		pChatLine->Instance.SetPosition(iWidth, pChatSet->m_iy + iHeight);
#else
		pChatLine->Instance.SetPosition(pChatSet->m_ix, pChatSet->m_iy + iHeight);
#endif

		pChatLine->Instance.SetColor(rColor);
		pChatLine->Instance.Update();
	}
}

void CPythonChat::UpdateEditMode(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	const int c_iAlphaLine = max(0, GetVisibleLineCount(dwID) - GetEditableLineCount(dwID) + 2);

	int iLineIndex = 0;
	float fAlpha = 0.0f;
	float fAlphaStep = 0.0f;

	if (c_iAlphaLine > 0)
		fAlphaStep = 1.0f / float(c_iAlphaLine);

	TChatLineList * pLineList = &(dwID == CHAT_SET_LOG_WINDOW ? pChatSet->m_ShowingChatLogLineList : pChatSet->m_ShowingChatLineList);
	int iHeight = -(int(pLineList->size()+1) * pChatSet->m_iStep);

	for (TChatLineList::iterator itor = pLineList->begin(); itor != pLineList->end(); ++itor)
	{
		TChatLine * pChatLine = (*itor);

		D3DXCOLOR & rColor = pChatLine->GetColorRef(dwID);

		if (iLineIndex < c_iAlphaLine)
		{
			rColor.a += (fAlpha - rColor.a) / 10.0f;
			fAlpha = fMIN(fAlpha+fAlphaStep, 1.0f);
		}
		else
		{
			rColor.a = fMIN(rColor.a+0.05f, 1.0f);
		}

		iHeight += pChatSet->m_iStep;

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		int iWidth = pChatSet->m_ix;
		pChatLine->Instance.SetPosition(iWidth, pChatSet->m_iy + iHeight);
#else
		pChatLine->Instance.SetPosition(pChatSet->m_ix, pChatSet->m_iy + iHeight);
#endif

		pChatLine->Instance.SetColor(rColor);
		pChatLine->Instance.Update();
	}
}

void CPythonChat::UpdateLogMode(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	TChatLineList * pLineList = &(dwID == CHAT_SET_LOG_WINDOW ? pChatSet->m_ShowingChatLogLineList : pChatSet->m_ShowingChatLineList);
	int iHeight = 0;

	for (TChatLineList::reverse_iterator itor = pLineList->rbegin(); itor != pLineList->rend(); ++itor)
	{
		TChatLine * pChatLine = (*itor);

		iHeight -= pChatSet->m_iStep;
		pChatLine->Instance.SetPosition(pChatSet->m_ix, pChatSet->m_iy + iHeight);
		pChatLine->Instance.SetColor(pChatLine->GetColorRef(dwID));
		pChatLine->Instance.Update();
	}
}

void CPythonChat::Update(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	switch (pChatSet->m_iBoardState)
	{
		case BOARD_STATE_VIEW:
			UpdateViewMode(dwID);
			break;
		case BOARD_STATE_EDIT:
			UpdateEditMode(dwID);
			break;
		case BOARD_STATE_LOG:
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		{
			UpdateLogMode(dwID);
			UpdateViewMode(dwID);
			UpdateEditMode(dwID);
		}
#else
			UpdateLogMode(dwID);
#endif
			break;
	}

	DWORD dwcurTime = CTimer::Instance().GetCurrentMillisecond();
	for (TWaitChatList::iterator itor = m_WaitChatList.begin(); itor != m_WaitChatList.end();)
	{
		TWaitChat & rWaitChat = *itor;

		if (rWaitChat.dwAppendingTime < dwcurTime)
		{
			AppendChat(rWaitChat.iType, rWaitChat.strChat.c_str());

			itor = m_WaitChatList.erase(itor);
		}
		else
		{
			++itor;
		}
	}
}

void CPythonChat::Render(DWORD dwID)
{
	TChatLineList * pLineList = GetChatLineListPtr(dwID);
	if (!pLineList)
		return;

	for (TChatLineList::iterator itor = pLineList->begin(); itor != pLineList->end(); ++itor)
	{
		CGraphicTextInstance & rInstance = (*itor)->Instance;
		rInstance.Render();
	}
}

void CPythonChat::SetBoardState(DWORD dwID, int iState)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iBoardState = iState;
	ArrangeShowingChat(dwID);
}

void CPythonChat::SetPosition(DWORD dwID, int ix, int iy)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_ix = ix;
	pChatSet->m_iy = iy;
}

void CPythonChat::SetHeight(DWORD dwID, int iHeight)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iHeight = iHeight;
}

void CPythonChat::SetStep(DWORD dwID, int iStep)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iStep = iStep;
}

#ifdef ENABLE_EMOTICONS_SYSTEM
#pragma warning(disable:4477) //line 371 - fix coming;
void CPythonChat::SetFontName (const char* c_szFontName)
{
	strChatDefaultFontName = c_szFontName;
	strChatDefaultFontName += ".fnt";
}

void CPythonChat::SaveEmojisChat (const char* c_szEmojisChat)
{
	FILE* fp;

	char bufEmojis[256];
	char bufCount[256];

	int iAuxCount = 0;
	std::string szAuxCodigo;

	int iCount = 0;
	std::string szCodigo;
	int iAuxCheck = 0;

	std::string str = c_szEmojisChat;
	std::string isEmojiFlag = "|T";
	std::size_t find1 = 0;
	std::size_t pos1 = 0;

	fp = fopen ("lib/emoji.cfg", "r");
	if (fp)
	{
		m_EmoticonsRecentChatVector.clear();
		m_EmoticonsRecentChatSet.clear();

		while (fgets (bufEmojis, 256, fp))
		{
			if (sscanf (bufEmojis, "%d\t%s\n", &iCount, &szCodigo) == EOF)
			{
				break;
			}
			else
			{
				TEmoticonsRecentChat EmoticonsRecentChat;
				EmoticonsRecentChat.count = iCount;
				EmoticonsRecentChat.codigo = szCodigo.c_str();
				EmoticonsRecentChat.check = 0;
				m_EmoticonsRecentChatVector.insert (TEmoticonsRecentChatVector::value_type (szCodigo.c_str(), EmoticonsRecentChat));
			}
		}
		fclose (fp);
	}

	for (int i = 0; i < str.length(); i++)
	{
		find1 = str.find (isEmojiFlag, i);
		if (find1 != std::string::npos)
		{
			pos1 = find1 + isEmojiFlag.length();
			i = find1 + isEmojiFlag.length();
			std::string szNameGeral = str.substr (pos1, str.length() - pos1);

			std::size_t pos2 = szNameGeral.find ("|t");
			std::string szEmojiName = szNameGeral.substr (0, pos2);

			auto itor = m_EmoticonsRecentChatVector.find (szEmojiName.c_str());
			if (m_EmoticonsRecentChatVector.end() != itor)
			{
				TEmoticonsRecentChat& rEmojis = itor->second;
				rEmojis.count += 1;
				rEmojis.check = 1;
			}
			else
			{
				TEmoticonsRecentChat EmoticonsRecentChat;
				EmoticonsRecentChat.count = 1;
				EmoticonsRecentChat.codigo = szEmojiName.c_str();
				EmoticonsRecentChat.check = 1;
				m_EmoticonsRecentChatVector.insert (TEmoticonsRecentChatVector::value_type (szEmojiName.c_str(), EmoticonsRecentChat));
			}
		}
	}

	for (auto itor = m_EmoticonsRecentChatVector.begin(); itor != m_EmoticonsRecentChatVector.end(); ++itor)
	{
		TEmoticonsRecentChat& rEmojis = itor->second;
		TEmoticonsRecentChat EmoticonsRecentChat;
		EmoticonsRecentChat.count = rEmojis.count;
		EmoticonsRecentChat.codigo = rEmojis.codigo.c_str();
		EmoticonsRecentChat.check = rEmojis.check;
		m_EmoticonsRecentChatSet.push_back (EmoticonsRecentChat);
	}

	int changed = 0;
	while (changed == 0 && m_EmoticonsRecentChatSet.size() > 0)
	{
		changed = 1;
		for (int i = 0; i < m_EmoticonsRecentChatSet.size() - 1; i++)
		{
			if (m_EmoticonsRecentChatSet[i].count < m_EmoticonsRecentChatSet[i + 1].count && m_EmoticonsRecentChatSet[i + 1].check >= 0)
			{
				changed = 0;

				iAuxCount = m_EmoticonsRecentChatSet[i + 1].count;
				m_EmoticonsRecentChatSet[i + 1].count = m_EmoticonsRecentChatSet[i].count;
				m_EmoticonsRecentChatSet[i].count = iAuxCount;

				szAuxCodigo = m_EmoticonsRecentChatSet[i + 1].codigo.c_str();
				m_EmoticonsRecentChatSet[i + 1].codigo = m_EmoticonsRecentChatSet[i].codigo.c_str();
				m_EmoticonsRecentChatSet[i].codigo = szAuxCodigo.c_str();

				iAuxCheck = m_EmoticonsRecentChatSet[i + 1].check;
				m_EmoticonsRecentChatSet[i + 1].check = iAuxCheck;
				m_EmoticonsRecentChatSet[i].check = iAuxCheck;
			}
			else if (m_EmoticonsRecentChatSet[i].count == m_EmoticonsRecentChatSet[i + 1].count && m_EmoticonsRecentChatSet[i + 1].check == 1)
			{
				changed = 0;

				iAuxCount = m_EmoticonsRecentChatSet[i + 1].count;
				m_EmoticonsRecentChatSet[i + 1].count = m_EmoticonsRecentChatSet[i].count;
				m_EmoticonsRecentChatSet[i].count = iAuxCount;

				szAuxCodigo = m_EmoticonsRecentChatSet[i + 1].codigo.c_str();
				m_EmoticonsRecentChatSet[i + 1].codigo = m_EmoticonsRecentChatSet[i].codigo.c_str();
				m_EmoticonsRecentChatSet[i].codigo = szAuxCodigo.c_str();

				iAuxCheck = m_EmoticonsRecentChatSet[i + 1].check;
				m_EmoticonsRecentChatSet[i + 1].check = 0;
				m_EmoticonsRecentChatSet[i].check = iAuxCheck;
			}
			else
			{
				m_EmoticonsRecentChatSet[i].check = 0;
				m_EmoticonsRecentChatSet[i + 1].check = 0;
			}
		}
	}

	if (m_EmoticonsRecentChatSet.size() != 0)
	{
		fp = fopen ("lib/emoji.cfg", "w");
		for (int i = 0; i < m_EmoticonsRecentChatSet.size(); i++)
		{
			_snprintf (bufCount, sizeof (bufCount), "%d", m_EmoticonsRecentChatSet[i].count);
			fprintf (fp, "%s\t%s\n", bufCount, m_EmoticonsRecentChatSet[i].codigo.c_str());
		}
		fclose (fp);
	}
}
#endif

void CPythonChat::ToggleChatMode(DWORD dwID, int iMode)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iMode[iMode] = 1 - pChatSet->m_iMode[iMode];
	ArrangeShowingChat(dwID);
}

void CPythonChat::EnableChatMode(DWORD dwID, int iMode)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iMode[iMode] = TRUE;
	ArrangeShowingChat(dwID);
}

void CPythonChat::DisableChatMode(DWORD dwID, int iMode)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_iMode[iMode] = FALSE;
	ArrangeShowingChat(dwID);
}

void CPythonChat::SetEndPos(DWORD dwID, float fPos)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	fPos = max(0.0f, fPos);
	fPos = min(1.0f, fPos);
	if (pChatSet->m_fEndPos != fPos)
	{
		pChatSet->m_fEndPos = fPos;
		ArrangeShowingChat(dwID);
	}
}

int CPythonChat::GetVisibleLineCount(DWORD dwID)
{
	TChatLineList * pLineList = GetChatLineListPtr(dwID);
	if (!pLineList)
		return 0;

	return pLineList->size();
}

int CPythonChat::GetEditableLineCount(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return 0;

	return pChatSet->m_iHeight / pChatSet->m_iStep + 1;
}

int CPythonChat::GetLineCount(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return 0;

	int iCount = 0;
	TChatLineDeque ChatLineDeque = (dwID == CHAT_SET_LOG_WINDOW ? m_ChatLogLineDeque : m_ChatLineDeque);
	for (DWORD i = 0; i < ChatLineDeque.size(); ++i)
	{
		if (!pChatSet->CheckMode(ChatLineDeque[i]->iType))
			continue;

		++iCount;
	}

	return iCount;
}

int CPythonChat::GetLineStep(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return 0;

	return pChatSet->m_iStep;
}

CPythonChat::TChatLineList * CPythonChat::GetChatLineListPtr(DWORD dwID)
{
	TChatSetMap::iterator itor = m_ChatSetMap.find(dwID);
	if (m_ChatSetMap.end() == itor)
		return NULL;

	TChatSet & rChatSet = itor->second;
	return &(dwID == CHAT_SET_LOG_WINDOW ? rChatSet.m_ShowingChatLogLineList : rChatSet.m_ShowingChatLineList);
}

CPythonChat::TChatSet * CPythonChat::GetChatSetPtr(DWORD dwID)
{
	TChatSetMap::iterator itor = m_ChatSetMap.find(dwID);
	if (m_ChatSetMap.end() == itor)
		return NULL;

	TChatSet & rChatSet = itor->second;
	return &rChatSet;
}

void CPythonChat::ArrangeShowingChat(DWORD dwID)
{
	TChatSet * pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_ShowingChatLineList.clear();

	TChatLineDeque TempChatLineDeque;
	for (TChatLineDeque::iterator itor = m_ChatLineDeque.begin(); itor != m_ChatLineDeque.end(); ++itor)
	{
		TChatLine * pChatLine = *itor;
		if (pChatSet->CheckMode(pChatLine->iType))
			TempChatLineDeque.push_back(pChatLine);
	}

	int icurLineCount = TempChatLineDeque.size();
	int iVisibleLineCount = min(icurLineCount, (pChatSet->m_iHeight + pChatSet->m_iStep) / pChatSet->m_iStep);
	int iEndLine = iVisibleLineCount + int(float(icurLineCount - iVisibleLineCount - 1) * pChatSet->m_fEndPos);

	int iHeight = 12;
	for (int i = min(icurLineCount-1, iEndLine); i >= 0; --i)
	{
		if (!pChatSet->CheckMode(TempChatLineDeque[i]->iType))
			continue;

		if (pChatSet->m_iHeight + pChatSet->m_iStep <= iHeight)
		{
			break;
		}

		pChatSet->m_ShowingChatLineList.push_front(TempChatLineDeque[i]);

		iHeight += pChatSet->m_iStep;
	}

	ArrangeShowingChatLog(dwID);
}

void CPythonChat::AppendChat(int iType, const char *c_szChat)
{
	CGraphicText* pkDefaultFont = static_cast<CGraphicText*>(DefaultFont_GetResource());
	if (!pkDefaultFont)
	{
		TraceError("CPythonChat::AppendChat - CANNOT_FIND_DEFAULT_FONT");
		return;
	}

#ifdef ENABLE_RENEWAL_OX_EVENT
	if (strstr(c_szChat, "#start") || strstr(c_szChat, "#send") || strstr(c_szChat, "#end"))
		return;
#endif

	IAbstractApplication& rApp = IAbstractApplication::GetSingleton();
	SChatLine * pChatLine = SChatLine::New();

	pChatLine->iType = iType;
	pChatLine->Instance.SetValue(c_szChat);

#ifdef ENABLE_EMOTICONS_SYSTEM
	if (!strChatDefaultFontName.empty())
	{
		CResourceManager& rkResMgr = CResourceManager::Instance();
		CResource* pkRes = rkResMgr.GetResourcePointer(strChatDefaultFontName.c_str());
		CGraphicText* pkResFont = static_cast<CGraphicText*>(pkRes);
		if (pkResFont)
			pChatLine->Instance.SetTextPointer(pkResFont);
		else
			pChatLine->Instance.SetTextPointer(pkDefaultFont);
	}
	else
	{
		pChatLine->Instance.SetTextPointer(pkDefaultFont);
	}
#else
	pChatLine->Instance.SetTextPointer(pkDefaultFont);
#endif

	pChatLine->fAppendedTime = rApp.GetGlobalTime();
	pChatLine->SetColorAll(GetChatColor(iType));

	m_ChatLineDeque.push_back(pChatLine);
	if (m_ChatLineDeque.size() > CHAT_LINE_MAX_NUM)
	{
		SChatLine * pChatLine = m_ChatLineDeque.front();
		SChatLine::Delete(pChatLine);
		m_ChatLineDeque.pop_front();
	}

	for (TChatSetMap::iterator itor = m_ChatSetMap.begin(); itor != m_ChatSetMap.end(); ++itor)
	{
		TChatSet * pChatSet = &(itor->second);

		if (BOARD_STATE_EDIT == pChatSet->m_iBoardState)
		{
			ArrangeShowingChat(itor->first);
		}
		else
		{
			pChatSet->m_ShowingChatLineList.push_back(pChatLine);
			if (pChatSet->m_ShowingChatLineList.size() > CHAT_LINE_MAX_NUM)
			{
				pChatSet->m_ShowingChatLineList.pop_front();
			}
		}
	}

	AppendChatLog(iType, c_szChat);
}

void CPythonChat::AppendChatWithDelay(int iType, const char *c_szChat, int iDelay)
{
	TWaitChat WaitChat;
	WaitChat.iType = iType;
	WaitChat.strChat = c_szChat;
	WaitChat.dwAppendingTime = CTimer::Instance().GetCurrentMillisecond() + iDelay;
	m_WaitChatList.push_back(WaitChat);
}

void CPythonChat::AddChatStack(const char *c_szChat)
{
	if (!m_WritedChatStack.empty() && strcmp(m_WritedChatStack.back().c_str(), c_szChat) == 0)
		return;

	std::string s; s+=c_szChat;
	m_WritedChatStack.push_back(s);

	if (m_WritedChatStack.size() > CHAT_STACK_MAX_NUM)
	{
		m_WritedChatStack.erase(m_WritedChatStack.begin());
	}
}

const char* CPythonChat::GetChatStack(BYTE index)
{
	if (index > m_WritedChatStack.size())
		return " ";

	return (m_WritedChatStack[m_WritedChatStack.size()-index].c_str()? m_WritedChatStack[m_WritedChatStack.size()-index].c_str(): " " );
}

BYTE CPythonChat::GetChatStackSize()
{
	return m_WritedChatStack.size();
}

void CPythonChat::AppendChatLog(int iType, const char *c_szChat)
{
	CGraphicText* pkDefaultFont = static_cast<CGraphicText*>(DefaultFont_GetResource());
	if (!pkDefaultFont)
	{
		TraceError("CPythonChat::AppendChatLog - CANNOT_FIND_DEFAULT_FONT");
		return;
	}

	IAbstractApplication& rApp = IAbstractApplication::GetSingleton();
	SChatLine* pChatLine = SChatLine::New();

	pChatLine->iType = iType;
	pChatLine->Instance.SetValue(c_szChat);

	pChatLine->Instance.SetTextPointer(pkDefaultFont);
	pChatLine->fAppendedTime = rApp.GetGlobalTime();
	pChatLine->SetColorAll(GetChatColor(iType));

	m_ChatLogLineDeque.emplace_back(pChatLine);
	if (m_ChatLogLineDeque.size() > CHAT_LINE_MAX_NUM)
	{
		SChatLine* pChatLine = m_ChatLogLineDeque.front();
		SChatLine::Delete(pChatLine);
		m_ChatLogLineDeque.pop_front();
	}

	for (TChatSetMap::iterator itor = m_ChatSetMap.begin(); itor != m_ChatSetMap.end(); ++itor)
	{
		TChatSet* pChatSet = &(itor->second);
		pChatSet->m_ShowingChatLogLineList.emplace_back(pChatLine);
		if (pChatSet->m_ShowingChatLogLineList.size() > CHAT_LINE_MAX_NUM)
			pChatSet->m_ShowingChatLogLineList.pop_front();
	}
}

void CPythonChat::ArrangeShowingChatLog(DWORD dwID)
{
	TChatSet* pChatSet = GetChatSetPtr(dwID);
	if (!pChatSet)
		return;

	pChatSet->m_ShowingChatLogLineList.clear();

	TChatLineDeque TempChatLineDeque;
	for (TChatLineDeque::iterator itor = m_ChatLogLineDeque.begin(); itor != m_ChatLogLineDeque.end(); ++itor)
	{
		TChatLine* pChatLine = *itor;
		if (pChatSet->CheckMode(pChatLine->iType))
			TempChatLineDeque.emplace_back(pChatLine);
	}

	int icurLineCount = TempChatLineDeque.size();
	int iVisibleLineCount = min(icurLineCount, (pChatSet->m_iHeight + pChatSet->m_iStep) / pChatSet->m_iStep);
	int iEndLine = iVisibleLineCount + int(float(icurLineCount - iVisibleLineCount - 1) * pChatSet->m_fEndPos);

	int iHeight = 12;
	for (int i = min(icurLineCount - 1, iEndLine); i >= 0; --i)
	{
		if (!pChatSet->CheckMode(TempChatLineDeque[i]->iType))
			continue;

		if (pChatSet->m_iHeight + pChatSet->m_iStep <= iHeight)
			break;

		pChatSet->m_ShowingChatLogLineList.emplace_front(TempChatLineDeque[i]);
		iHeight += pChatSet->m_iStep;
	}
}

DWORD CPythonChat::GetChatColor(int iType)
{
	if (iType<CHAT_TYPE_MAX_NUM)
	{
		return m_akD3DXClrChat[iType];
	}

	return D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
}

void CPythonChat::IgnoreCharacter(const char *c_szName)
{
	TIgnoreCharacterSet::iterator itor = m_IgnoreCharacterSet.find(c_szName);

	if (m_IgnoreCharacterSet.end() != itor)
	{
		m_IgnoreCharacterSet.erase(itor);
	}
	else
	{
		m_IgnoreCharacterSet.insert(c_szName);
	}
}

BOOL CPythonChat::IsIgnoreCharacter(const char *c_szName)
{
	TIgnoreCharacterSet::iterator itor = m_IgnoreCharacterSet.find(c_szName);

	if (m_IgnoreCharacterSet.end() == itor)
		return FALSE;

	return TRUE;
}

CWhisper * CPythonChat::CreateWhisper(const char *c_szName)
{
	CWhisper * pWhisper = CWhisper::New();
	m_WhisperMap.insert(TWhisperMap::value_type(c_szName, pWhisper));
	return pWhisper;
}

#ifdef ENABLE_MULTILANGUAGE_CHATTING
void CPythonChat::AppendWhisper(int iType, const char *c_szName, const char *c_szChat, DWORD dwCodePage)
#else
void CPythonChat::AppendWhisper(int iType, const char *c_szName, const char *c_szChat)
#endif
{
	TWhisperMap::iterator itor = m_WhisperMap.find(c_szName);

	CWhisper * pWhisper;
	if (itor == m_WhisperMap.end())
	{
		pWhisper = CreateWhisper(c_szName);
	}
	else
	{
		pWhisper = itor->second;
	}

#ifdef ENABLE_MULTILANGUAGE_CHATTING
	pWhisper->AppendChat(iType, c_szChat, dwCodePage);
#else
	pWhisper->AppendChat(iType, c_szChat);
#endif
}

void CPythonChat::ClearWhisper(const char *c_szName)
{
	TWhisperMap::iterator itor = m_WhisperMap.find(c_szName);

	if (itor != m_WhisperMap.end())
	{
		CWhisper * pWhisper = itor->second;
		CWhisper::Delete(pWhisper);
		
		m_WhisperMap.erase(itor);
	}
}

BOOL CPythonChat::GetWhisper(const char *c_szName, CWhisper ** ppWhisper)
{
	TWhisperMap::iterator itor = m_WhisperMap.find(c_szName);

	if (itor == m_WhisperMap.end())
		return FALSE;

	*ppWhisper = itor->second;

	return TRUE;
}

void CPythonChat::InitWhisper(PyObject *ppyObject)
{
	TWhisperMap::iterator itor = m_WhisperMap.begin();
	for (; itor != m_WhisperMap.end(); ++itor)
	{
		std::string strName = itor->first;
		PyCallClassMemberFunc(ppyObject, "MakeWhisperButton", Py_BuildValue("(s)", strName.c_str()));
	}
}

void CPythonChat::__DestroyWhisperMap()
{
	TWhisperMap::iterator itor = m_WhisperMap.begin();
	for (; itor != m_WhisperMap.end(); ++itor)
	{
		CWhisper::Delete(itor->second);
	}
	m_WhisperMap.clear();
}

void CPythonChat::Close()
{
	TChatSetMap::iterator itor = m_ChatSetMap.begin();
	for (; itor != m_ChatSetMap.end(); ++itor)
	{
		TChatSet & rChatSet = itor->second;
		TChatLineList * pLineList = &(rChatSet.m_ShowingChatLineList);
		for (TChatLineList::iterator itor = pLineList->begin(); itor != pLineList->end(); ++itor)
		{
			TChatLine * pChatLine = (*itor);
			pChatLine->fAppendedTime = 0.0f;
		}

		pLineList = &(rChatSet.m_ShowingChatLogLineList);
		for (TChatLineList::iterator itor = pLineList->begin(); itor != pLineList->end(); ++itor)
		{
			TChatLine* pChatLine = (*itor);
			pChatLine->fAppendedTime = 0.0f;
		}
	}
}

void CPythonChat::Destroy()
{
	__DestroyWhisperMap();

	m_ShowingChatLineList.clear();
	m_ChatSetMap.clear();
	m_ChatLineDeque.clear();

	m_ShowingChatLogLineList.clear();
	m_ChatLogLineDeque.clear();

	SChatLine::DestroySystem();
	CWhisper::DestroySystem();

	__Initialize();
}

void CPythonChat::__Initialize()
{
	m_akD3DXClrChat[CHAT_TYPE_TALKING] = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_INFO] = D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_NOTICE] = D3DXCOLOR(1.0f, 0.902f, 0.730f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_PARTY] = D3DXCOLOR(0.542f, 1.0f, 0.949f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_GUILD] = D3DXCOLOR(0.906f, 0.847f, 1.0f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_COMMAND] = D3DXCOLOR(0.658f, 1.0f, 0.835f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_SHOUT] = D3DXCOLOR(0.658f, 1.0f, 0.835f, 1.0f);
	m_akD3DXClrChat[CHAT_TYPE_WHISPER] = D3DXCOLOR(0xff4AE14A);
	m_akD3DXClrChat[CHAT_TYPE_BIG_NOTICE] = D3DXCOLOR(1.0f, 0.902f, 0.730f, 1.0f);
#ifdef ENABLE_DICE_SYSTEM
	m_akD3DXClrChat[CHAT_TYPE_DICE_INFO] = D3DXCOLOR(0xFFcc00cc);
#endif
#ifdef ENABLE_RENEWAL_OX_EVENT
	m_akD3DXClrChat[CHAT_TYPE_CONTROL_NOTICE] = D3DXCOLOR(1.0f, 0.902f, 0.730f, 1.0f);
#endif

#ifdef ENABLE_EMOTICONS_SYSTEM
	m_EmoticonsRecentChatVector.clear();
	m_EmoticonsRecentChatSet.clear();
#endif
}

CPythonChat::CPythonChat()
{
	__Initialize();
}

CPythonChat::~CPythonChat()
{
	assert(m_ChatLineDeque.empty());
	assert(m_ShowingChatLineList.empty());
	assert(m_ChatSetMap.empty());
	assert(m_WhisperMap.empty());
	assert(m_ChatLogLineDeque.empty());
	assert(m_ShowingChatLogLineList.empty());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CDynamicPool<CWhisper> CWhisper::ms_kPool;

CWhisper* CWhisper::New()
{
	return ms_kPool.Alloc();
}

void CWhisper::Delete(CWhisper* pkWhisper)
{
	pkWhisper->Destroy();
	ms_kPool.Free(pkWhisper);
}

void CWhisper::DestroySystem()
{
	ms_kPool.Destroy();

	SChatLine::DestroySystem();
}

void CWhisper::SetPosition(float fPosition)
{
	m_fcurPosition = fPosition;
	__ArrangeChat();
}

void CWhisper::SetBoxSize(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;

	for (TChatLineDeque::iterator itor = m_ChatLineDeque.begin(); itor != m_ChatLineDeque.end(); ++itor)
	{
		TChatLine * pChatLine = *itor;
		pChatLine->Instance.SetLimitWidth(fWidth);
	}
}

#ifdef ENABLE_MULTILANGUAGE_CHATTING
void CWhisper::AppendChat(int iType, const char *c_szChat, DWORD dwCodePage)
#else
void CWhisper::AppendChat(int iType, const char *c_szChat)
#endif
{
	CGraphicText* pkDefaultFont =  static_cast<CGraphicText*>(DefaultFont_GetResource());

	if (!pkDefaultFont)
	{
		TraceError("CWhisper::AppendChat - CANNOT_FIND_DEFAULT_FONT");
		return;
	}

	SChatLine * pChatLine = SChatLine::New();
#ifdef ENABLE_MULTILANGUAGE_CHATTING
	if (dwCodePage)
		pChatLine->Instance.SetValue(c_szChat, &dwCodePage);
	else
		pChatLine->Instance.SetValue(c_szChat);
#else
	pChatLine->Instance.SetValue(c_szChat);
#endif
	pChatLine->Instance.SetTextPointer(pkDefaultFont);
	pChatLine->Instance.SetLimitWidth(m_fWidth);
	pChatLine->Instance.SetMultiLine(TRUE);

	switch(iType)
	{
		case CPythonChat::WHISPER_TYPE_SYSTEM:
			pChatLine->Instance.SetColor(D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f));
			break;

		case CPythonChat::WHISPER_TYPE_GM:
			pChatLine->Instance.SetColor(D3DXCOLOR(1.0f, 0.632f, 0.0f, 1.0f));
			break;

#ifdef ENABLE_OFFLINE_MESSAGE
		case CPythonChat::WHISPER_TYPE_OFFLINE:
			pChatLine->Instance.SetColor(0xffb9f2ff);
			break;
#endif

		case CPythonChat::WHISPER_TYPE_CHAT:
		default:
			pChatLine->Instance.SetColor(0xffffffff);
			break;
	}

	m_ChatLineDeque.push_back(pChatLine);

	__ArrangeChat();
}

void CWhisper::__ArrangeChat()
{
	for (TChatLineDeque::iterator itor = m_ChatLineDeque.begin(); itor != m_ChatLineDeque.end(); ++itor)
	{
		TChatLine * pChatLine = *itor;
		pChatLine->Instance.Update();
	}
}

void CWhisper::Render(float fx, float fy)
{
	float fHeight = fy + m_fHeight;

	int iViewCount = int(m_fHeight / m_fLineStep) - 1;
	int iLineCount = int(m_ChatLineDeque.size());
	int iStartLine = -1;
	if (iLineCount > iViewCount)
	{
		iStartLine = int(float(iLineCount-iViewCount) * m_fcurPosition) + iViewCount - 1;
	}
	else if (!m_ChatLineDeque.empty())
	{
		iStartLine = iLineCount - 1;
	}

	RECT Rect = { static_cast<long>(fx), static_cast<long>(fy), static_cast<long>(fx+m_fWidth), static_cast<long>(fy+m_fHeight) };

#ifdef ENABLE_CLIP_MASKING
	if (GetDefaultCodePage() == CP_ARABIC)
	{
		Rect.left = fx - m_fWidth;
		Rect.right = fx;
	}
#endif

	for (int i = iStartLine; i >= 0; --i)
	{
		assert(i >= 0 && i < int(m_ChatLineDeque.size()));
		TChatLine * pChatLine = m_ChatLineDeque[i];

		WORD wLineCount = pChatLine->Instance.GetTextLineCount();
		fHeight -= wLineCount * m_fLineStep;

		pChatLine->Instance.SetPosition(fx, fHeight);
		pChatLine->Instance.Render(&Rect);

		if (fHeight < fy)
			break;
	}
}

void CWhisper::__Initialize()
{
#ifdef ENABLE_EMOTICONS_SYSTEM
	m_fLineStep = 20.0f;
#else
	m_fLineStep = 15.0f;
#endif
	m_fWidth = 300.0f;
	m_fHeight = 120.0f;
	m_fcurPosition = 1.0f;
}

void CWhisper::Destroy()
{
	std::for_each(m_ChatLineDeque.begin(), m_ChatLineDeque.end(), SChatLine::Delete);
	m_ChatLineDeque.clear();
	m_ShowingChatLineList.clear();
}

CWhisper::CWhisper()
{
	__Initialize();
}

CWhisper::~CWhisper()
{
	Destroy();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CDynamicPool<CWhisper::SChatLine> CWhisper::SChatLine::ms_kPool;

CWhisper::SChatLine* CWhisper::SChatLine::New()
{
	return ms_kPool.Alloc();
}

void CWhisper::SChatLine::Delete(CWhisper::SChatLine* pkChatLine)
{
	pkChatLine->Instance.Destroy();
	ms_kPool.Free(pkChatLine);
}

void CWhisper::SChatLine::DestroySystem()
{
	ms_kPool.Destroy();
}
