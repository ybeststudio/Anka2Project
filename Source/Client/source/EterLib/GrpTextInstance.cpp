#include "StdAfx.h"
#include "GrpTextInstance.h"
#include "StateManager.h"
#include "IME.h"
#include "TextTag.h"
#include "../EterLocale/StringCodec.h"
#include "../EterBase/Utils.h"
#include "../EterLocale/Arabic.h"

#if defined(ENABLE_TEXTLINE_EMOJI) || defined (ENABLE_EMOTICONS_SYSTEM)
	#include "ResourceManager.h"
#endif

extern DWORD GetDefaultCodePage();

const float c_fFontFeather = 0.5f;

CDynamicPool<CGraphicTextInstance> CGraphicTextInstance::ms_kPool;

static int gs_mx = 0;
static int gs_my = 0;

static std::wstring gs_hyperlinkText;

void CGraphicTextInstance::Hyperlink_UpdateMousePos(int x, int y)
{
	gs_mx = x;
	gs_my = y;
	gs_hyperlinkText = L"";
}

int CGraphicTextInstance::Hyperlink_GetText(char* buf, int len)
{
	if (gs_hyperlinkText.empty())
		return 0;

	int codePage = GetDefaultCodePage();

	return Ymir_WideCharToMultiByte(codePage, 0, gs_hyperlinkText.c_str(), gs_hyperlinkText.length(), buf, len, nullptr, nullptr);
}

int CGraphicTextInstance::__DrawCharacter(CGraphicFontTexture* pFontTexture, WORD codePage, wchar_t text, DWORD dwColor)
{
	CGraphicFontTexture::TCharacterInfomation* pInsCharInfo = pFontTexture->GetCharacterInfomation(codePage, text);

	if (pInsCharInfo)
	{
		m_dwColorInfoVector.push_back(dwColor);
		m_pCharInfoVector.push_back(pInsCharInfo);

		m_textWidth += pInsCharInfo->advance;
		m_textHeight = max(pInsCharInfo->height, m_textHeight);
		return pInsCharInfo->advance;
	}

	return 0;
}

void CGraphicTextInstance::__GetTextPos(DWORD index, float* x, float* y)
{
	index = min(index, m_pCharInfoVector.size());

	float sx = 0;
	float sy = 0;
	float fFontMaxHeight = 0;

#ifdef INSIDE_RENDER
	for (DWORD i = m_startPos; i < index; ++i)
#else
	for (DWORD i = 0; i < index; ++i)
#endif
	{
		if (sx + float(m_pCharInfoVector[i]->width) > m_fLimitWidth)
		{
			sx = 0;
			sy += fFontMaxHeight;
		}

#ifdef ENABLE_MULTI_TEXTLINE
		if (m_isEnterToken && std::find(m_vLineCutPos.begin(), m_vLineCutPos.end(), i) != m_vLineCutPos.end())
		{
			sx = 0;
			sy += i ? m_iLineHeight : 0;
		}
#endif

		sx += float(m_pCharInfoVector[i]->advance);
		fFontMaxHeight = max(float(m_pCharInfoVector[i]->height), fFontMaxHeight);
	}

	*x = sx;
	*y = sy;
}

bool isNumberic(const char chr)
{
	if (chr >= '0' && chr <= '9')
		return true;
	return false;
}

bool IsValidToken(const char* iter)
{
	return	iter[0] == '@' &&
		isNumberic(iter[1]) &&
		isNumberic(iter[2]) &&
		isNumberic(iter[3]) &&
		isNumberic(iter[4]);
}

const char* FindToken(const char* begin, const char* end)
{
	while (begin < end)
	{
		begin = std::find(begin, end, '@');

		if (end - begin > 5 && IsValidToken(begin))
		{
			return begin;
		}
		else
		{
			++begin;
		}
	}

	return end;
}

int ReadToken(const char* token)
{
	int nRet = (token[1] - '0') * 1000 + (token[2] - '0') * 100 + (token[3] - '0') * 10 + (token[4] - '0');
	if (nRet == 9999)
		return CP_UTF8;
	return nRet;
}

void CGraphicTextInstance::Update()
{
	if (m_isUpdate)
		return;

	if (m_roText.IsNull())
	{
		Tracef("CGraphicTextInstance::Update - Font has not been set\n");
		return;
	}

	if (m_roText->IsEmpty())
		return;

	CGraphicFontTexture* pFontTexture = m_roText->GetFontTexturePointer();
	if (!pFontTexture)
		return;

	UINT defCodePage = GetDefaultCodePage();

#ifdef ENABLE_MULTILANGUAGE_CHATTING
	UINT dataCodePage = m_dwCodePage ? m_dwCodePage : defCodePage;
#else
	UINT dataCodePage = defCodePage;
#endif

	CGraphicFontTexture::TCharacterInfomation* pSpaceInfo = pFontTexture->GetCharacterInfomation(dataCodePage, ' ');

	int spaceHeight = pSpaceInfo ? pSpaceInfo->height : 12;

	m_pCharInfoVector.clear();
	m_dwColorInfoVector.clear();
	m_hyperlinkVector.clear();

#ifdef ENABLE_TEXTLINE_EMOJI
	if (m_emojiVector.size() != 0)
	{
		for (std::vector<SEmoji>::iterator itor = m_emojiVector.begin(); itor != m_emojiVector.end(); ++itor)
		{
			SEmoji & rEmo = *itor;
			if (rEmo.pInstance)
			{
				CGraphicImageInstance::Delete(rEmo.pInstance);
				rEmo.pInstance = nullptr;
			}
		}
	}
	m_emojiVector.clear();
#endif

#ifdef ENABLE_EMOTICONS_SYSTEM
	if (m_emoticonVector.size() != 0)
	{
		for (std::vector<SEmoticon>::iterator itor = m_emoticonVector.begin(); itor != m_emoticonVector.end(); ++itor)
		{
			SEmoticon& rTmo = *itor;
			if (rTmo.pInstance)
			{
				CGraphicExpandedImageInstance::Delete(rTmo.pInstance);
				rTmo.pInstance = nullptr;
			}
		}
	}
	m_emoticonVector.clear();
#endif

	m_textWidth = 0;
	m_textHeight = spaceHeight;

	/* wstring begin */

	const char* begin = m_stText.c_str();
	const char* end = begin + m_stText.length();

	int wTextMax = (end - begin) * 2;
	wchar_t* wText = (wchar_t*)_alloca(sizeof(wchar_t) * wTextMax);

	DWORD dwColor = m_dwTextColor;

	/* wstring end */
	while (begin < end)
	{
		const char* token = FindToken(begin, end);

		int wTextLen = Ymir_MultiByteToWideChar(dataCodePage, 0, begin, token - begin, wText, wTextMax);

		if (m_isSecret)
		{
			for (int i = 0; i < wTextLen; ++i)
				__DrawCharacter(pFontTexture, dataCodePage, '*', dwColor);
		}
		else
		{
			if (defCodePage == CP_ARABIC) // ARABIC
			{

				wchar_t* wArabicText = (wchar_t*)_alloca(sizeof(wchar_t) * wTextLen);
				int wArabicTextLen = Arabic_MakeShape(wText, wTextLen, wArabicText, wTextLen);

				bool isEnglish = true;
				int nEnglishBase = wArabicTextLen - 1;

				int x = 0;
				int len;
				int hyperlinkStep = 0;
				SHyperlink kHyperlink;
				std::wstring hyperlinkBuffer;
				int no_hyperlink = 0;

				if (Arabic_IsInSymbol(wArabicText[wArabicTextLen - 1]))
				{
					isEnglish = false;
				}

				int i = 0;
				for (i = wArabicTextLen - 1 ; i >= 0; --i)
				{
					wchar_t wArabicChar = wArabicText[i];

					if (isEnglish)
					{
						if (Arabic_IsInSymbol(wArabicChar) && (
								(i == 0) ||
								(i > 0 && 
									!(Arabic_HasPresentation(wArabicText, i - 1) || Arabic_IsInPresentation(wArabicText[i + 1]))  && //앞글자, �?글자가 아랍어 아님.
									wArabicText[i+1] != '|'
								) ||
								wArabicText[i] == '|'
							))
						{
							int temptest = 1;
						}
						else if (Arabic_IsInPresentation(wArabicChar) || Arabic_IsInSymbol(wArabicChar))
						{
							for (int e = i + 1; e <= nEnglishBase;) {
								int ret = GetTextTag(&wArabicText[e], wArabicTextLen - e, len, hyperlinkBuffer);

								if (ret == TEXT_TAG_PLAIN || ret == TEXT_TAG_TAG)
								{
									if (hyperlinkStep == 1)
										hyperlinkBuffer.append(1, wArabicText[e]);
									else
									{
										int charWidth = __DrawCharacter(pFontTexture, dataCodePage, wArabicText[e], dwColor);
										kHyperlink.ex += charWidth;

										for (int j = 1; j <= no_hyperlink; j++)
										{
											if(m_hyperlinkVector.size() < j)
												break;

											SHyperlink & tempLink = m_hyperlinkVector[m_hyperlinkVector.size() - j];
											tempLink.ex += charWidth;
											tempLink.sx += charWidth;
										}
									}
								}
								else
								{
									if (ret == TEXT_TAG_COLOR)
										dwColor = htoi(hyperlinkBuffer.c_str(), 8);
									else if (ret == TEXT_TAG_RESTORE_COLOR)
										dwColor = m_dwTextColor;
									else if (ret == TEXT_TAG_HYPERLINK_START)
									{
										hyperlinkStep = 1;
										hyperlinkBuffer = L"";
									}
									else if (ret == TEXT_TAG_HYPERLINK_END)
									{
										if (hyperlinkStep == 1)
										{
											++hyperlinkStep;
											kHyperlink.ex = kHyperlink.sx = 0;
										}
										else
										{
											kHyperlink.text = hyperlinkBuffer;
											m_hyperlinkVector.push_back(kHyperlink);
											no_hyperlink++;


											hyperlinkStep = 0;
											hyperlinkBuffer = L"";
										}
									}
								}
								e += len;
							}

							int charWidth = __DrawCharacter(pFontTexture, dataCodePage, Arabic_ConvSymbol(wArabicText[i]), dwColor);
							kHyperlink.ex += charWidth;

							for (int j = 1; j <= no_hyperlink; j++)
							{
								if (m_hyperlinkVector.size() < j)
									break;

								SHyperlink & tempLink = m_hyperlinkVector[m_hyperlinkVector.size() - j];
								tempLink.ex += charWidth;
								tempLink.sx += charWidth;
							}

							isEnglish = false;
						}
					}
					else
					{
						if (Arabic_IsInPresentation(wArabicChar) || Arabic_IsInSymbol(wArabicChar))
						{
							int charWidth = __DrawCharacter(pFontTexture, dataCodePage, Arabic_ConvSymbol(wArabicText[i]), dwColor);
							kHyperlink.ex += charWidth;
							x += charWidth;

							for (int j = 1; j <= no_hyperlink; j++)
							{
								if (m_hyperlinkVector.size() < j)
									break;

								SHyperlink & tempLink = m_hyperlinkVector[m_hyperlinkVector.size() - j];
								tempLink.ex += charWidth;
								tempLink.sx += charWidth;
							}
						}
						else
						{
							nEnglishBase = i;
							isEnglish = true;
						}
					}
				}

				if (isEnglish)
				{
					for (int e = i + 1; e <= nEnglishBase;) {
						int ret = GetTextTag(&wArabicText[e], wArabicTextLen - e, len, hyperlinkBuffer);

						if (ret == TEXT_TAG_PLAIN || ret == TEXT_TAG_TAG)
						{
							if (hyperlinkStep == 1)
								hyperlinkBuffer.append(1, wArabicText[e]);
							else
							{
								int charWidth = __DrawCharacter(pFontTexture, dataCodePage, wArabicText[e], dwColor);
								kHyperlink.ex += charWidth;

								for (int j = 1; j <= no_hyperlink; j++)
								{
									if (m_hyperlinkVector.size() < j)
										break;

									SHyperlink & tempLink = m_hyperlinkVector[m_hyperlinkVector.size() - j];
									tempLink.ex += charWidth;
									tempLink.sx += charWidth;
								}
							}
						}
						else
						{
							if (ret == TEXT_TAG_COLOR)
								dwColor = htoi(hyperlinkBuffer.c_str(), 8);
							else if (ret == TEXT_TAG_RESTORE_COLOR)
								dwColor = m_dwTextColor;
							else if (ret == TEXT_TAG_HYPERLINK_START)
							{
								hyperlinkStep = 1;
								hyperlinkBuffer = L"";
							}
							else if (ret == TEXT_TAG_HYPERLINK_END)
							{
								if (hyperlinkStep == 1)
								{
									++hyperlinkStep;
									kHyperlink.ex = kHyperlink.sx = 0;
								}
								else
								{
									kHyperlink.text = hyperlinkBuffer;
									m_hyperlinkVector.push_back(kHyperlink);
									no_hyperlink++;

									hyperlinkStep = 0;
									hyperlinkBuffer = L"";
								}
							}
						}
						e += len;
					}

				}
			}
			else
			{
				int x = 0;
				int len;
				int hyperlinkStep = 0;
				SHyperlink kHyperlink;
				std::wstring hyperlinkBuffer;

#ifdef ENABLE_TEXTLINE_EMOJI
				SEmoji kEmoji;
				int emojiStep = 0;
				std::wstring emojiBuffer;
#endif

#ifdef ENABLE_EMOTICONS_SYSTEM
				SEmoticon kEmoticon;
				int emoticonStep = 0;
				std::wstring emoticonBuffer;
#endif

				for (int i = 0; i < wTextLen; )
				{
					int ret = GetTextTag(&wText[i], wTextLen - i, len, hyperlinkBuffer);

					if (ret == TEXT_TAG_PLAIN || ret == TEXT_TAG_TAG)
					{
						if (hyperlinkStep == 1)
							hyperlinkBuffer.append(1, wText[i]);
#ifdef ENABLE_TEXTLINE_EMOJI
						else if (emojiStep == 1)
							emojiBuffer.append(1, wText[i]);
#endif
#ifdef ENABLE_EMOTICONS_SYSTEM
						else if (emoticonStep == 1)
							emoticonBuffer.append(1, wText[i]);
#endif
						else
						{
							int charWidth = __DrawCharacter(pFontTexture, dataCodePage, wText[i], dwColor);
							kHyperlink.ex += charWidth;
							x += charWidth;
						}
					}
#ifdef ENABLE_TEXTLINE_EMOJI
					else if (ret == TEXT_TAG_EMOJI_START)
					{
						emojiStep = 1;
						emojiBuffer = L"";
					}
					else if (ret == TEXT_TAG_EMOJI_END)
					{
						kEmoji.x = x;

						char retBuf[1024];
						int retLen = Ymir_WideCharToMultiByte(GetDefaultCodePage(), 0, emojiBuffer.c_str(), emojiBuffer.length(), retBuf, sizeof(retBuf) - 1, nullptr, nullptr);
						retBuf[retLen] = '\0';

						char szPath[255];
						_snprintf(szPath, sizeof(szPath), "d:/ymir work/ui/game/emoji/%s.tga", retBuf);
						if (CResourceManager::Instance().IsFileExist(szPath))
						{
							CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(szPath);
							kEmoji.pInstance = CGraphicImageInstance::New();
							kEmoji.pInstance->SetImagePointer(pImage);
							m_emojiVector.push_back(kEmoji);
							memset(&kEmoji, 0, sizeof(SEmoji));
							for (int i = 0; i < pImage->GetWidth() / (pSpaceInfo->width-1); ++i)
								x += __DrawCharacter(pFontTexture, dataCodePage, ' ', dwColor);
							if (pImage->GetWidth() % (pSpaceInfo->width - 1) > 1)
								x += __DrawCharacter(pFontTexture, dataCodePage, ' ', dwColor);
						}
						emojiStep = 0;
						emojiBuffer = L"";
					}
#endif

#ifdef ENABLE_EMOTICONS_SYSTEM
					else if (ret == TEXT_TAG_EMOTICON_START)
					{
						emoticonStep = 1;
						emoticonBuffer = L"";
					}
					else if (ret == TEXT_TAG_EMOTICON_END)
					{
						char retBuf[1024];
						int retLen = Ymir_WideCharToMultiByte(GetDefaultCodePage(), 0, emoticonBuffer.c_str(), emoticonBuffer.length(), retBuf, sizeof(retBuf) - 1, nullptr, nullptr);
						retBuf[retLen] = '\0';

						char szPath[1024];
						_snprintf(szPath, sizeof(szPath), "d:/ymir work/ui/game/emoji/emoticons/%s.png", retBuf);

						if (CResourceManager::Instance().IsFileExist(szPath))
						{
							kEmoticon.t = x;
							CGraphicImage* pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(szPath);
							kEmoticon.pInstance = CGraphicExpandedImageInstance::New();
							kEmoticon.pInstance->SetImagePointer(pImage);
							kEmoticon.pInstance->SetScale(float(0.26), float(0.26));
							x += (__DrawCharacter(pFontTexture, dataCodePage, ' ', dwColor))*2; // Temp fix
							m_emoticonVector.emplace_back(kEmoticon);
							memset(&kEmoticon, 0, sizeof(SEmoticon));
						}
						emoticonStep = 0;
						emoticonBuffer = L"";
					}
#endif
					else
					{
						if (ret == TEXT_TAG_COLOR)
							dwColor = htoi(hyperlinkBuffer.c_str(), 8);
						else if (ret == TEXT_TAG_RESTORE_COLOR)
							dwColor = m_dwTextColor;
						else if (ret == TEXT_TAG_HYPERLINK_START)
						{
							hyperlinkStep = 1;
							hyperlinkBuffer = L"";
						}
						else if (ret == TEXT_TAG_HYPERLINK_END)
						{
							if (hyperlinkStep == 1)
							{
								++hyperlinkStep;
								kHyperlink.ex = kHyperlink.sx = x;
							}
							else
							{
								kHyperlink.text = hyperlinkBuffer;
								m_hyperlinkVector.push_back(kHyperlink);

								hyperlinkStep = 0;
								hyperlinkBuffer = L"";
							}
						}
					}
					i += len;
				}
			}
		}

		if (token < end)
		{
			int newCodePage = ReadToken(token);
			dataCodePage = newCodePage;
			begin = token + 5;
		}
		else
		{
			begin = token;
		}
	}

	pFontTexture->UpdateTexture();

	m_isUpdate = true;
}


static bool ms_bBatchMode = false;
#ifdef ENABLE_FIX_MOBS_LAG
static std::map<CGraphicImageTexture*, std::vector<SPDTVertexRaw>> ms_kBatchVerticesMap;
#endif
// Per-texture glyph quads collected while batching (4 vertices per quad, strip order TL BL TR BR).
static std::vector<std::pair<LPDIRECT3DBASETEXTURE9, std::vector<SVertex>>> ms_kBatchQuads;

// Every per-glyph draw in Render() goes through here: draws immediately as before, or, between
// BeginBatch/EndBatch, appends to the frame batch keyed by the texture Render() just bound.
static void __TextEmitQuad(const SVertex* pkQuad)
{
	if (!ms_bBatchMode)
	{
		if (CGraphicBase::SetPDTStream((SPDTVertex*)pkQuad, 4))
			STATEMANAGER.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		return;
	}

	LPDIRECT3DBASETEXTURE9 pkTexture = nullptr;
	STATEMANAGER.GetTexture(0, &pkTexture);
	std::vector<SVertex>* pkList = nullptr;
	for (auto& rkEntry : ms_kBatchQuads)
		if (rkEntry.first == pkTexture) { pkList = &rkEntry.second; break; }
	if (!pkList)
	{
		ms_kBatchQuads.emplace_back(pkTexture, std::vector<SVertex>());
		pkList = &ms_kBatchQuads.back().second;
		pkList->reserve(4096);
	}
	pkList->insert(pkList->end(), pkQuad, pkQuad + 4);
}

void CGraphicTextInstance::BeginBatch()
{
	ms_bBatchMode = true;
}

void CGraphicTextInstance::EndBatch()
{
	ms_bBatchMode = false;

	size_t uQuads = 0;
	for (const auto& rkEntry : ms_kBatchQuads)
		uQuads += rkEntry.second.size() / 4;
	if (!uQuads)
		return;

	// Same states Render() uses for its glyph pass.
	STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	DWORD dwFogEnable = STATEMANAGER.GetRenderState(D3DRS_FOGENABLE);
	DWORD dwLighting = STATEMANAGER.GetRenderState(D3DRS_LIGHTING);
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);
#ifdef ENABLE_DIRECTX9_UPDATE
	STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
#else
	STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
#endif
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	// Independent triangles per quad, so glyphs of different texts are never joined.
	static std::vector<SVertex> s_kList;
	const size_t MAX_UP_VERTICES = 60000;	// multiple of 6
	for (auto& rkEntry : ms_kBatchQuads)
	{
		std::vector<SVertex>& rkQuads = rkEntry.second;
		if (rkQuads.empty())
			continue;

		s_kList.clear();
		s_kList.reserve(rkQuads.size() / 4 * 6);
		for (size_t i = 0; i + 3 < rkQuads.size(); i += 4)
		{
			s_kList.push_back(rkQuads[i + 0]);
			s_kList.push_back(rkQuads[i + 1]);
			s_kList.push_back(rkQuads[i + 2]);
			s_kList.push_back(rkQuads[i + 2]);
			s_kList.push_back(rkQuads[i + 1]);
			s_kList.push_back(rkQuads[i + 3]);
		}
		rkQuads.clear();

		STATEMANAGER.SetTexture(0, rkEntry.first);
		for (size_t i = 0; i < s_kList.size(); i += MAX_UP_VERTICES)
		{
			const size_t n = (std::min)(MAX_UP_VERTICES, s_kList.size() - i);
			STATEMANAGER.DrawPrimitiveUP(D3DPT_TRIANGLELIST, UINT(n / 3), &s_kList[i], sizeof(SVertex));
		}
	}

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, dwFogEnable);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, dwLighting);
	STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);
}

void CGraphicTextInstance::Render(RECT * pClipRect)
{
	if (!m_isUpdate)
		return;

	CGraphicText* pkText=m_roText.GetPointer();
	if (!pkText)
		return;

	CGraphicFontTexture* pFontTexture = pkText->GetFontTexturePointer();
	if (!pFontTexture)
		return;


#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CGraphicTextInstance::Render **");
#endif

#ifdef INSIDE_RENDER
	float textureWidth, textureHeight;
	pFontTexture->GetTextureSize(textureWidth, textureHeight);
#endif

	float fStanX = m_v3Position.x;
	float fStanY = m_v3Position.y + 1.0f;

	UINT defCodePage = GetDefaultCodePage();

	if (defCodePage == CP_ARABIC)
	{
		switch (m_hAlign)
		{
			case HORIZONTAL_ALIGN_LEFT:
				fStanX -= m_textWidth;
				break;

			case HORIZONTAL_ALIGN_CENTER:
				fStanX -= float(m_textWidth / 2);
				break;
		}
	}
	else
	{
		switch (m_hAlign)
		{
			case HORIZONTAL_ALIGN_RIGHT:
				fStanX -= m_textWidth;
				break;

			case HORIZONTAL_ALIGN_CENTER:
				fStanX -= float(m_textWidth / 2);
				break;
		}
	}

	switch (m_vAlign)
	{
		case VERTICAL_ALIGN_BOTTOM:
			fStanY -= m_textHeight;
			break;

		case VERTICAL_ALIGN_CENTER:
			fStanY -= float(m_textHeight) / 2.0f;
			break;
	}


	STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	DWORD dwFogEnable = STATEMANAGER.GetRenderState(D3DRS_FOGENABLE);
	DWORD dwLighting = STATEMANAGER.GetRenderState(D3DRS_LIGHTING);
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

#ifdef ENABLE_DIRECTX9_UPDATE
    STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
#else
    STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
#endif

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

#ifdef ENABLE_FIX_MOBS_LAG
	std::map<CGraphicImageTexture*, std::vector<SPDTVertexRaw>> kLocalVerticesMap;
	auto& verticesMap = ms_bBatchMode ? ms_kBatchVerticesMap : kLocalVerticesMap;
#endif

	{
		const float fFontHalfWeight = 1.0f;

		float fCurX;
		float fCurY;
#ifdef INSIDE_RENDER
		float fCurLocalX;
#endif

		float fFontSx;
		float fFontSy;
		float fFontEx;
		float fFontEy;
		float fFontWidth;
		float fFontHeight;
		float fFontMaxHeight;
		float fFontAdvance;

#ifdef ENABLE_FIX_MOBS_LAG
		SPDTVertexRaw akVertex[4];
		akVertex[0].pz = m_v3Position.z;
		akVertex[1].pz = m_v3Position.z;
		akVertex[2].pz = m_v3Position.z;
		akVertex[3].pz = m_v3Position.z;
#else
		SVertex akVertex[4];
		akVertex[0].z = m_v3Position.z;
		akVertex[1].z = m_v3Position.z;
		akVertex[2].z = m_v3Position.z;
		akVertex[3].z = m_v3Position.z;
#endif

		CGraphicFontTexture::TCharacterInfomation* pCurCharInfo;

#ifdef INSIDE_RENDER
		m_startPos = max(0, m_startPos);
		m_endPos = min(m_endPos, WORD(m_pCharInfoVector.size()));
		if (!m_isFixedRenderPos && (m_startPos >= m_endPos || m_isMultiLine || !m_isCursor || !m_isOutline))
		{
			m_startPos = 0;
			m_endPos = WORD(m_pCharInfoVector.size());
		}
#endif

		if (m_isOutline)
		{
#ifdef INSIDE_RENDER
			if (m_isCursor && !m_isMultiLine && !m_isFixedRenderPos)
			{
				int curPos = min(CIME::GetCurPos(), m_pCharInfoVector.size());
				if (curPos < m_startPos)
					m_startPos = max(curPos, 0);
				else if (curPos > m_endPos)
				{
					m_endPos = curPos;
					m_startPos = min(m_endPos, WORD(m_pCharInfoVector.size()) - 1);
					fCurX = 0;
					for (; m_startPos >= 0; --m_startPos)
					{
						if (fCurX + float(m_pCharInfoVector[m_startPos]->width) > m_fLimitWidth)
						{
							++m_startPos;
							break;
						}
						fCurX += float(m_pCharInfoVector[m_startPos]->advance);

						if (m_startPos == 0)
							break;
					}
				}
			}
#endif

			fCurX = fStanX;
			fCurY = fStanY;
			fFontMaxHeight = 0.0f;

#ifdef INSIDE_RENDER
			for (WORD i = m_startPos; i < m_endPos; ++i)
			{
				pCurCharInfo = m_pCharInfoVector[i];

				fFontWidth = float(pCurCharInfo->width);
				fFontHeight = float(pCurCharInfo->height);
				fFontAdvance = float(pCurCharInfo->advance);

				float fXRenderLeft = 0.0f;
				float fXRenderRight = 0.0f;

				if (m_bUseRenderingRect)
				{
					if (fCurX - fStanX < m_RenderingRect.left)
					{
						if (fCurX - fStanX + fFontWidth <= m_RenderingRect.left)
						{
							fCurX += fFontAdvance;
							continue;
						}

						fXRenderLeft = -((float)(m_RenderingRect.left - (fCurX - fStanX)) / fFontWidth);
					}
					else if ((fCurX - fStanX) + fFontWidth > m_RenderingRect.right)
					{
						if ((fCurX - fStanX) >= m_RenderingRect.right)
						{
							fCurX += fFontAdvance;
							continue;
						}

						fXRenderRight = -((float)((fCurX - fStanX) + fFontWidth - m_RenderingRect.right) / fFontWidth);
					}
				}

				if ((fCurX + fFontWidth) - fStanX > m_fLimitWidth)
				{
					if (m_isMultiLine)
					{
						fCurX = fStanX;
						fCurY += fFontMaxHeight;
					}
					else
					{
						m_endPos = i;
						break;
					}
				}

#ifdef ENABLE_MULTI_TEXTLINE
				if (m_isEnterToken && std::find(m_vLineCutPos.begin(), m_vLineCutPos.end(), i) != m_vLineCutPos.end())
				{
					switch (m_hAlign)
					{
						case HORIZONTAL_ALIGN_LEFT:
							fCurX = fStanX;
							break;

						case HORIZONTAL_ALIGN_CENTER:
							fCurX = fStanX + float(m_textWidth - GetTextWidth(i)) / 2.0f;
							break;

						case HORIZONTAL_ALIGN_RIGHT:
							fCurX = fStanX + float(m_textWidth - GetTextWidth(i));
							break;
					}

					fCurY += m_iLineHeight;
				}
#endif

#ifndef ENABLE_CLIP_MASKING
				if (pClipRect)
				{
					if (fCurY <= pClipRect->top)
					{
						fCurX += fFontAdvance;
						continue;
					}
				}
#endif

				fFontSx = fCurX - 0.5f;
				fFontSy = fCurY - 0.5f;
				fFontEx = fFontSx + fFontWidth;
				fFontEy = fFontSy + fFontHeight;

#ifndef ENABLE_CLIP_MASKING
                pFontTexture->SelectTexture(pCurCharInfo->index);
                STATEMANAGER.SetTexture(0, pFontTexture->GetD3DTexture());

                akVertex[0].u = pCurCharInfo->left;
                akVertex[0].v = pCurCharInfo->top;
                akVertex[1].u = pCurCharInfo->left;
                akVertex[1].v = pCurCharInfo->bottom;
                akVertex[2].u = pCurCharInfo->right;
                akVertex[2].v = pCurCharInfo->top;
                akVertex[3].u = pCurCharInfo->right;
                akVertex[3].v = pCurCharInfo->bottom;
#else
                float su = pCurCharInfo->left;
                float sv = pCurCharInfo->top;
                float eu = pCurCharInfo->right;
                float ev = pCurCharInfo->bottom;

                if (pClipRect)
                {
                    const float width = pCurCharInfo->right - pCurCharInfo->left;
                    const float height = pCurCharInfo->bottom - pCurCharInfo->top;

                    if (fFontEx <= pClipRect->left)
                    {
                        fCurX += fFontAdvance;
                        continue;
                    }

                    if (fFontSx < pClipRect->left)
                    {
                        su += (pClipRect->left - fFontSx) / fFontWidth * width;
                        fFontSx = pClipRect->left;
                    }

                    if (fFontEy <= pClipRect->top)
                    {
                        fCurX += fFontAdvance;
                        continue;
                    }

                    if (fFontSy < pClipRect->top)
                    {
                        sv += (pClipRect->top - fFontSy) / fFontHeight * height;
                        fFontSy = pClipRect->top;
                    }

                    if (fFontSx >= pClipRect->right)
                    {
                        fCurX += fFontAdvance;
                        continue;
                    }

                    if (fFontEx > pClipRect->right)
                    {
                        eu -= (fFontEx - pClipRect->right) / fFontWidth * width;
                        fFontEx = pClipRect->right;
                    }

                    if (fFontSy >= pClipRect->bottom)
                    {
                        fCurX += fFontAdvance;
                        continue;
                    }

                    if (fFontEy > pClipRect->bottom)
                    {
                        ev -= (fFontEy - pClipRect->bottom) / fFontHeight * height;
                        fFontEy = pClipRect->bottom;
                    }
                }


#ifdef ENABLE_FIX_MOBS_LAG
				const auto tex = pFontTexture->GetTexture(pCurCharInfo->index);
				auto & batchVertices = verticesMap[tex];
#else
                pFontTexture->SelectTexture(pCurCharInfo->index);
                STATEMANAGER.SetTexture(0, pFontTexture->GetD3DTexture());
#endif

                akVertex[0].u = su;
                akVertex[0].v = sv;
                akVertex[1].u = su;
                akVertex[1].v = ev;
                akVertex[2].u = eu;
                akVertex[2].v = sv;
                akVertex[3].u = eu;
                akVertex[3].v = ev;
#endif

#ifdef ENABLE_FIX_MOBS_LAG
				akVertex[3].diffuse = akVertex[2].diffuse = akVertex[1].diffuse = akVertex[0].diffuse = m_dwOutLineColor;
#else
				akVertex[3].color = akVertex[2].color = akVertex[1].color = akVertex[0].color = m_dwOutLineColor;
#endif


				float feather = 0.0f; // m_fFontFeather

#ifdef ENABLE_FIX_MOBS_LAG
				akVertex[0].py = fFontSy - feather;
				akVertex[1].py = fFontEy + feather;
				akVertex[2].py = fFontSy - feather;
				akVertex[3].py = fFontEy + feather;
#else
				akVertex[0].y = fFontSy - feather;
				akVertex[1].y = fFontEy + feather;
				akVertex[2].y = fFontSy - feather;
				akVertex[3].y = fFontEy + feather;
#endif

				// 왼
#ifdef ENABLE_FIX_MOBS_LAG
				akVertex[0].px = fFontSx - fFontHalfWeight - feather;
				akVertex[1].px = fFontSx - fFontHalfWeight - feather;
				akVertex[2].px = fFontEx - fFontHalfWeight + feather;
				akVertex[3].px = fFontEx - fFontHalfWeight + feather;
#else
				akVertex[0].x = fFontSx - fFontHalfWeight - feather;
				akVertex[1].x = fFontSx - fFontHalfWeight - feather;
				akVertex[2].x = fFontEx - fFontHalfWeight + feather;
				akVertex[3].x = fFontEx - fFontHalfWeight + feather;
#endif

#ifdef ENABLE_FIX_MOBS_LAG
				batchVertices.insert(batchVertices.end(), std::begin(akVertex), std::end(akVertex));
#else
				__TextEmitQuad(akVertex);
#endif


				// 오른
#ifdef ENABLE_FIX_MOBS_LAG
				akVertex[0].px = fFontSx + fFontHalfWeight - feather;
				akVertex[1].px = fFontSx + fFontHalfWeight - feather;
				akVertex[2].px = fFontEx + fFontHalfWeight + feather;
				akVertex[3].px = fFontEx + fFontHalfWeight + feather;
				batchVertices.insert(batchVertices.end(), std::begin(akVertex), std::end(akVertex));

				akVertex[0].px = fFontSx - feather;
				akVertex[1].px = fFontSx - feather;
				akVertex[2].px = fFontEx + feather;
				akVertex[3].px = fFontEx + feather;

				akVertex[0].py = fFontSy - fFontHalfWeight - feather;
				akVertex[1].py = fFontEy - fFontHalfWeight + feather;
				akVertex[2].py = fFontSy - fFontHalfWeight - feather;
				akVertex[3].py = fFontEy - fFontHalfWeight + feather;
				batchVertices.insert(batchVertices.end(), std::begin(akVertex), std::end(akVertex));

				akVertex[0].py = fFontSy + fFontHalfWeight - feather;
				akVertex[1].py = fFontEy + fFontHalfWeight + feather;
				akVertex[2].py = fFontSy + fFontHalfWeight - feather;
				akVertex[3].py = fFontEy + fFontHalfWeight + feather;
				batchVertices.insert(batchVertices.end(), std::begin(akVertex), std::end(akVertex));
#else
				akVertex[0].x = fFontSx + fFontHalfWeight - feather;
				akVertex[1].x = fFontSx + fFontHalfWeight - feather;
				akVertex[2].x = fFontEx + fFontHalfWeight + feather;
				akVertex[3].x = fFontEx + fFontHalfWeight + feather;

				__TextEmitQuad(akVertex);

				akVertex[0].x = fFontSx - feather;
				akVertex[1].x = fFontSx - feather;
				akVertex[2].x = fFontEx + feather;
				akVertex[3].x = fFontEx + feather;

				akVertex[0].y = fFontSy - fFontHalfWeight - feather;
				akVertex[1].y = fFontEy - fFontHalfWeight + feather;
				akVertex[2].y = fFontSy - fFontHalfWeight - feather;
				akVertex[3].y = fFontEy - fFontHalfWeight + feather;

				__TextEmitQuad(akVertex);

				akVertex[0].y = fFontSy + fFontHalfWeight - feather;
				akVertex[1].y = fFontEy + fFontHalfWeight + feather;
				akVertex[2].y = fFontSy + fFontHalfWeight - feather;
				akVertex[3].y = fFontEy + fFontHalfWeight + feather;

				__TextEmitQuad(akVertex);
#endif

				fCurX += fFontAdvance;
			}
#else
			CGraphicFontTexture::TPCharacterInfomationVector::iterator i;
			for (int i = 0; i < m_pCharInfoVector.size(); ++i)
			{
				pCurCharInfo = m_pCharInfoVector[i];

				fFontWidth = float(pCurCharInfo->width);
				fFontHeight = float(pCurCharInfo->height);
				fFontAdvance = float(pCurCharInfo->advance);

				if ((fCurX + fFontWidth) - m_v3Position.x > m_fLimitWidth)
				{
					if (m_isMultiLine)
					{
						fCurX = fStanX;
						fCurY += fFontMaxHeight;
					}
					else
					{
						break;
					}
				}

#ifdef ENABLE_MULTI_TEXTLINE
				if (m_isEnterToken && std::find(m_vLineCutPos.begin(), m_vLineCutPos.end(), i) != m_vLineCutPos.end())
				{
					switch (m_hAlign)
					{
						case HORIZONTAL_ALIGN_LEFT:
							fCurX = fStanX;
							break;

						case HORIZONTAL_ALIGN_CENTER:
							fCurX = fStanX + float(m_textWidth - GetTextWidth(i)) / 2.0f;
							break;

						case HORIZONTAL_ALIGN_RIGHT:
							fCurX = fStanX + float(m_textWidth - GetTextWidth(i));
							break;
					}

					fCurY += m_iLineHeight;
				}
#endif

#ifndef ENABLE_CLIP_MASKING
                if (pClipRect)
                {
                    if (fCurY <= pClipRect->top)
                    {
                        fCurX += fFontAdvance;
                        continue;
                    }
                }
#endif

				fFontSx = fCurX - 0.5f;
				fFontSy = fCurY - 0.5f;
				fFontEx = fFontSx + fFontWidth;
				fFontEy = fFontSy + fFontHeight;

#ifndef ENABLE_CLIP_MASKING
                pFontTexture->SelectTexture(pCurCharInfo->index);
                STATEMANAGER.SetTexture(0, pFontTexture->GetD3DTexture());

                akVertex[0].u = pCurCharInfo->left;
                akVertex[0].v = pCurCharInfo->top;
                akVertex[1].u = pCurCharInfo->left;
                akVertex[1].v = pCurCharInfo->bottom;
                akVertex[2].u = pCurCharInfo->right;
                akVertex[2].v = pCurCharInfo->top;
                akVertex[3].u = pCurCharInfo->right;
                akVertex[3].v = pCurCharInfo->bottom;
#else
                float su = pCurCharInfo->left;
                float sv = pCurCharInfo->top;
                float eu = pCurCharInfo->right;
                float ev = pCurCharInfo->bottom;

                if (pClipRect)
                {
                    const float width = pCurCharInfo->right - pCurCharInfo->left;
                    const float height = pCurCharInfo->bottom - pCurCharInfo->top;

                    if (fFontEx <= pClipRect->left)
                    {
                        fCurX += fFontAdvance;
                        continue;
                    }

                    if (fFontSx < pClipRect->left)
                    {
                        su += (pClipRect->left - fFontSx) / fFontWidth * width;
                        fFontSx = pClipRect->left;
                    }

                    if (fFontEy <= pClipRect->top)
                    {
                        fCurX += fFontAdvance;
                        continue;
                    }

                    if (fFontSy < pClipRect->top)
                    {
                        sv += (pClipRect->top - fFontSy) / fFontHeight * height;
                        fFontSy = pClipRect->top;
                    }

                    if (fFontSx >= pClipRect->right)
                    {
                        fCurX += fFontAdvance;
                        continue;
                    }

                    if (fFontEx > pClipRect->right)
                    {
                        eu -= (fFontEx - pClipRect->right) / fFontWidth * width;
                        fFontEx = pClipRect->right;
                    }

                    if (fFontSy >= pClipRect->bottom)
                    {
                        fCurX += fFontAdvance;
                        continue;
                    }

                    if (fFontEy > pClipRect->bottom)
                    {
                        ev -= (fFontEy - pClipRect->bottom) / fFontHeight * height;
                        fFontEy = pClipRect->bottom;
                    }
                }

#ifdef ENABLE_FIX_MOBS_LAG
				const auto tex = pFontTexture->GetTexture(pCurCharInfo->index);
				auto & batchVertices = verticesMap[tex];
#else
				pFontTexture->SelectTexture(pCurCharInfo->index);
				STATEMANAGER.SetTexture(0, pFontTexture->GetD3DTexture());
#endif

				akVertex[0].u = su;
				akVertex[0].v = sv;
				akVertex[1].u = su;
				akVertex[1].v = ev;
				akVertex[2].u = eu;
				akVertex[2].v = sv;
				akVertex[3].u = eu;
				akVertex[3].v = ev;
#else
#ifdef ENABLE_FIX_MOBS_LAG
				const auto tex = pFontTexture->GetTexture(pCurCharInfo->index);
				auto & batchVertices = verticesMap[tex];
#else
				pFontTexture->SelectTexture(pCurCharInfo->index);
				STATEMANAGER.SetTexture(0, pFontTexture->GetD3DTexture());
#endif

				akVertex[0].u=pCurCharInfo->left;
				akVertex[0].v=pCurCharInfo->top;
				akVertex[1].u=pCurCharInfo->left;
				akVertex[1].v=pCurCharInfo->bottom;
				akVertex[2].u=pCurCharInfo->right;
				akVertex[2].v=pCurCharInfo->top;
				akVertex[3].u=pCurCharInfo->right;
				akVertex[3].v=pCurCharInfo->bottom;
#endif

#ifdef ENABLE_FIX_MOBS_LAG
				akVertex[3].diffuse = akVertex[2].diffuse = akVertex[1].diffuse = akVertex[0].diffuse = m_dwOutLineColor;
#else
				akVertex[3].color = akVertex[2].color = akVertex[1].color = akVertex[0].color = m_dwOutLineColor;
#endif
				float feather = 0.0f;

#ifdef ENABLE_FIX_MOBS_LAG
				akVertex[0].py = fFontSy - feather;
				akVertex[1].py = fFontEy + feather;
				akVertex[2].py = fFontSy - feather;
				akVertex[3].py = fFontEy + feather;
				akVertex[0].px = fFontSx - fFontHalfWeight - feather;
				akVertex[1].px = fFontSx - fFontHalfWeight - feather;
				akVertex[2].px = fFontEx - fFontHalfWeight + feather;
				akVertex[3].px = fFontEx - fFontHalfWeight + feather;
#else
				akVertex[0].y = fFontSy - feather;
				akVertex[1].y = fFontEy + feather;
				akVertex[2].y = fFontSy - feather;
				akVertex[3].y = fFontEy + feather;
				akVertex[0].x = fFontSx - fFontHalfWeight - feather;
				akVertex[1].x = fFontSx - fFontHalfWeight - feather;
				akVertex[2].x = fFontEx - fFontHalfWeight + feather;
				akVertex[3].x = fFontEx - fFontHalfWeight + feather;
#endif

#ifdef ENABLE_FIX_MOBS_LAG
				batchVertices.insert(batchVertices.end(), std::begin(akVertex), std::end(akVertex));

				akVertex[0].px = fFontSx + fFontHalfWeight - feather;
				akVertex[1].px = fFontSx + fFontHalfWeight - feather;
				akVertex[2].px = fFontEx + fFontHalfWeight + feather;
				akVertex[3].px = fFontEx + fFontHalfWeight + feather;
#else
				__TextEmitQuad(akVertex);

				akVertex[0].x = fFontSx + fFontHalfWeight - feather;
				akVertex[1].x = fFontSx + fFontHalfWeight - feather;
				akVertex[2].x = fFontEx + fFontHalfWeight + feather;
				akVertex[3].x = fFontEx + fFontHalfWeight + feather;
#endif

#ifdef ENABLE_FIX_MOBS_LAG
				batchVertices.insert(batchVertices.end(), std::begin(akVertex), std::end(akVertex));

				akVertex[0].px = fFontSx - feather;
				akVertex[1].px = fFontSx - feather;
				akVertex[2].px = fFontEx + feather;
				akVertex[3].px = fFontEx + feather;

				akVertex[0].py = fFontSy - fFontHalfWeight - feather;
				akVertex[1].py = fFontEy - fFontHalfWeight + feather;
				akVertex[2].py = fFontSy - fFontHalfWeight - feather;
				akVertex[3].py = fFontEy - fFontHalfWeight + feather;
#else
				__TextEmitQuad(akVertex);

				akVertex[0].x = fFontSx - feather;
				akVertex[1].x = fFontSx - feather;
				akVertex[2].x = fFontEx + feather;
				akVertex[3].x = fFontEx + feather;
				akVertex[0].y = fFontSy - fFontHalfWeight - feather;
				akVertex[1].y = fFontEy - fFontHalfWeight + feather;
				akVertex[2].y = fFontSy - fFontHalfWeight - feather;
				akVertex[3].y = fFontEy - fFontHalfWeight + feather;
#endif

#ifdef ENABLE_FIX_MOBS_LAG
				batchVertices.insert(batchVertices.end(), std::begin(akVertex), std::end(akVertex));

				akVertex[0].py = fFontSy + fFontHalfWeight - feather;
				akVertex[1].py = fFontEy + fFontHalfWeight + feather;
				akVertex[2].py = fFontSy + fFontHalfWeight - feather;
				akVertex[3].py = fFontEy + fFontHalfWeight + feather;
#else
				__TextEmitQuad(akVertex);

				akVertex[0].y = fFontSy + fFontHalfWeight - feather;
				akVertex[1].y = fFontEy + fFontHalfWeight + feather;
				akVertex[2].y = fFontSy + fFontHalfWeight - feather;
				akVertex[3].y = fFontEy + fFontHalfWeight + feather;
#endif

#ifdef ENABLE_FIX_MOBS_LAG
				batchVertices.insert(batchVertices.end(), std::begin(akVertex), std::end(akVertex));
#else
				__TextEmitQuad(akVertex);
#endif
				fCurX += fFontAdvance;
			}
#endif
		}

		fCurX = fStanX;
		fCurY = fStanY;
		fFontMaxHeight = 0.0f;

#ifdef INSIDE_RENDER
		float fCountX = 0.0f;
		float fCountY = 0.0f;
		float addXL, addYT, addXR, addYB;
		for (WORD i = m_startPos; i < m_endPos; ++i)
		{
			pCurCharInfo = m_pCharInfoVector[i];

			fCurLocalX = fCurX - fStanX;

			fFontWidth = float(pCurCharInfo->width);
			fFontHeight = float(pCurCharInfo->height);
			fFontMaxHeight = max(fFontHeight, pCurCharInfo->height);
			fFontAdvance = float(pCurCharInfo->advance);

			float fXRenderLeft = 0.0f;
			float fXRenderRight = 0.0f;
			float fYRenderTop = 0.0f;
			float fYRenderBottom = 0.0f;

			if (m_bUseRenderingRect)
			{
				if (fCurLocalX < m_RenderingRect.left && !m_isMultiLine)
				{
					if (fCurLocalX + fFontWidth <= m_RenderingRect.left)
					{
						fCurX += fFontAdvance;
						continue;
					}

					fXRenderLeft = ((float)(m_RenderingRect.left - fCurLocalX) / fFontWidth);
				}
				else if (fCurLocalX + fFontWidth > m_RenderingRect.right && !m_isMultiLine)
				{
					if (fCurLocalX >= m_RenderingRect.right)
					{
						fCurX += fFontAdvance;
						continue;
					}

					fXRenderRight = ((float)(fCurLocalX + fFontWidth - m_RenderingRect.right) / fFontWidth);
				}

				if (m_RenderingRect.top)
					fYRenderTop = m_RenderingRect.top / fFontHeight;
				if (m_RenderingRect.bottom < fFontHeight)
					fYRenderBottom = 1.0f - (m_RenderingRect.bottom / fFontHeight);
			}

			if (fCurLocalX + fFontWidth > m_fLimitWidth)
			{
				if (m_isMultiLine)
				{
					fCurX = fStanX;
					fCountX = 0.0f;
					fCurY += fFontMaxHeight;
					fCountY += fFontMaxHeight;
				}
				else
				{
					break;
				}
			}

#ifdef ENABLE_MULTI_TEXTLINE
			if (m_isEnterToken && std::find(m_vLineCutPos.begin(), m_vLineCutPos.end(), i) != m_vLineCutPos.end())
			{
				switch (m_hAlign)
				{
					case HORIZONTAL_ALIGN_LEFT:
						fCurX = fStanX;
						break;

					case HORIZONTAL_ALIGN_CENTER:
						fCurX = fStanX + float(m_textWidth - GetTextWidth(i)) / 2.0f;
						break;

					case HORIZONTAL_ALIGN_RIGHT:
						fCurX = fStanX + float(m_textWidth - GetTextWidth(i));
						break;
				}

				fCurY += m_iLineHeight;
			}
#endif

#ifndef ENABLE_CLIP_MASKING
			if (pClipRect)
			{
				if (fCurY <= pClipRect->top)
				{
					fCurX += fFontAdvance;
					continue;
				}
			}
#endif

			fFontSx = fCurX - 0.5f + fFontWidth * fXRenderLeft;
			fFontSy = fCurY - 0.5f + fFontHeight * fYRenderTop;
			fFontEx = fFontSx + fFontWidth * (1.0 - fXRenderRight - fXRenderLeft);
			fFontEy = fFontSy + fFontHeight * (1.0 - fYRenderBottom - fYRenderTop);

			addXR = addXL = addYT = addYB = 0.0f;

			if (!m_isMultiLine)
			{
				if (fCountX + fFontWidth < float(m_renderBox.left))
				{
					fCurX += fFontAdvance;
					fCountX += fFontAdvance;
					continue;
				}
				else if (fCountX < float(m_renderBox.left))
					addXL = float(m_renderBox.left) - fCountX;

				if (fCountY + fFontHeight < float(m_renderBox.top))
				{
					fCurX += fFontAdvance;
					fCountX += fFontAdvance;
					continue;
				}
				else if (fCountY < float(m_renderBox.top))
					addYT = float(m_renderBox.top) - fCountY;

				if (fCountX > m_textWidth - float(m_renderBox.right))
				{
					fCurX += fFontAdvance;
					fCountX += fFontAdvance;
					continue;
				}
				else if (fCountX + fFontWidth > m_textWidth - float(m_renderBox.right))
					addXR = fCountX + fFontWidth - m_textWidth + float(m_renderBox.right);

				if (fCountY > m_textHeight - float(m_renderBox.bottom))
				{
					fCurX += fFontAdvance;
					fCountX += fFontAdvance;
					continue;
				}
				else if (fCountY + fFontHeight > m_textHeight - float(m_renderBox.bottom))
					addYB = fCountY + fFontHeight - m_textHeight + float(m_renderBox.bottom);
			}

#ifdef ENABLE_FIX_MOBS_LAG
			const auto tex = pFontTexture->GetTexture(pCurCharInfo->index);
			auto& batchVertices = verticesMap[tex];
#else
            pFontTexture->SelectTexture(pCurCharInfo->index);
            STATEMANAGER.SetTexture(0, pFontTexture->GetD3DTexture());
#endif

            float fTextureRenderLeft = (pCurCharInfo->right - pCurCharInfo->left) * fXRenderLeft;
            float fTextureRenderTop = (pCurCharInfo->bottom - pCurCharInfo->top) * fYRenderTop;
            float fTextureRenderRight = (pCurCharInfo->right - pCurCharInfo->left) * fXRenderRight;
            float fTextureRenderBottom = (pCurCharInfo->bottom - pCurCharInfo->top) * fYRenderBottom;

#ifdef ENABLE_CLIP_MASKING
            if (pClipRect)
            {
                const float width = pCurCharInfo->right - pCurCharInfo->left;
                const float height = pCurCharInfo->bottom - pCurCharInfo->top;

                if (fFontEx <= pClipRect->left)
                {
                    fCurX += fFontAdvance;
                    continue;
                }

                if (fFontSx < pClipRect->left)
                {
                    fTextureRenderLeft += (pClipRect->left - fFontSx) / fFontWidth * width;
                    fFontSx = pClipRect->left;
                }

                if (fFontEy <= pClipRect->top)
                {
                    fCurX += fFontAdvance;
                    continue;
                }

                if (fFontSy < pClipRect->top)
                {
                    fTextureRenderTop += (pClipRect->top - fFontSy) / fFontHeight * height;
                    fFontSy = pClipRect->top;
                }

                if (fFontSx >= pClipRect->right)
                {
                    fCurX += fFontAdvance;
                    continue;
                }

                if (fFontEx > pClipRect->right)
                {
                    fTextureRenderRight -= (fFontEx - pClipRect->right) / fFontWidth * width;
                    fFontEx = pClipRect->right;
                }

                if (fFontSy >= pClipRect->bottom)
                {
                    fCurX += fFontAdvance;
                    continue;
                }

                if (fFontEy > pClipRect->bottom)
                {
                    fTextureRenderBottom -= (fFontEy - pClipRect->bottom) / fFontHeight * height;
                    fFontEy = pClipRect->bottom;
                }
            }
#endif

#ifdef ENABLE_FIX_MOBS_LAG
            akVertex[0].px = fFontSx + addXL;
            akVertex[0].py = fFontSy + addYT;
            akVertex[0].u = pCurCharInfo->left + fTextureRenderLeft + addXL / textureWidth;
            akVertex[0].v = pCurCharInfo->top + fTextureRenderTop + addYT / textureHeight;

            akVertex[1].px = fFontSx + addXL;
            akVertex[1].py = fFontEy - addYB;
            akVertex[1].u = pCurCharInfo->left + fTextureRenderLeft + addXL / textureWidth;
            akVertex[1].v = pCurCharInfo->bottom - fTextureRenderBottom - addYB / textureHeight;

            akVertex[2].px = fFontEx - addXR;
            akVertex[2].py = fFontSy + addYT;
            akVertex[2].u = pCurCharInfo->right - fTextureRenderRight - addXR / textureWidth;
            akVertex[2].v = pCurCharInfo->top + fTextureRenderTop + addYT / textureHeight;

            akVertex[3].px = fFontEx - addXR;
            akVertex[3].py = fFontEy - addYB;
            akVertex[3].u = pCurCharInfo->right - fTextureRenderRight - addXR / textureWidth;
            akVertex[3].v = pCurCharInfo->bottom - fTextureRenderBottom - addYB / textureHeight;

			akVertex[0].diffuse = akVertex[1].diffuse = akVertex[2].diffuse = akVertex[3].diffuse = m_dwColorInfoVector[i];

			batchVertices.insert(batchVertices.end(), std::begin(akVertex), std::end(akVertex));
#else
            akVertex[0].x = fFontSx + addXL;
            akVertex[0].y = fFontSy + addYT;
            akVertex[0].u = pCurCharInfo->left + fTextureRenderLeft + addXL / textureWidth;
            akVertex[0].v = pCurCharInfo->top + fTextureRenderTop + addYT / textureHeight;

            akVertex[1].x = fFontSx + addXL;
            akVertex[1].y = fFontEy - addYB;
            akVertex[1].u = pCurCharInfo->left + fTextureRenderLeft + addXL / textureWidth;
            akVertex[1].v = pCurCharInfo->bottom - fTextureRenderBottom - addYB / textureHeight;

            akVertex[2].x = fFontEx - addXR;
            akVertex[2].y = fFontSy + addYT;
            akVertex[2].u = pCurCharInfo->right - fTextureRenderRight - addXR / textureWidth;
            akVertex[2].v = pCurCharInfo->top + fTextureRenderTop + addYT / textureHeight;

            akVertex[3].x = fFontEx - addXR;
            akVertex[3].y = fFontEy - addYB;
            akVertex[3].u = pCurCharInfo->right - fTextureRenderRight - addXR / textureWidth;
            akVertex[3].v = pCurCharInfo->bottom - fTextureRenderBottom - addYB / textureHeight;

			akVertex[0].color = akVertex[1].color = akVertex[2].color = akVertex[3].color = m_dwColorInfoVector[i];

			__TextEmitQuad(akVertex);
#endif

			fCurX += fFontAdvance;
			fCountX += fFontAdvance;
		}

#ifdef ENABLE_FIX_MOBS_LAG
		if (!ms_bBatchMode)
		{
		for (auto& p : verticesMap)
			{
				STATEMANAGER.SetTexture(0, p.first->GetD3DTexture());
	
				for (auto f = p.second.begin(), l = p.second.end(); f != l; )
				{
					const auto batchCount = std::min<std::size_t>(LARGE_PDT_VERTEX_BUFFER_SIZE,
						l - f);
	
					if (CGraphicBase::SetPDTStream(&*f, batchCount))
						STATEMANAGER.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0,
							batchCount - 2);
					f += batchCount;
				}
			}
		}
#endif
#else
		for (int i = 0; i < m_pCharInfoVector.size(); ++i)
		{
			pCurCharInfo = m_pCharInfoVector[i];

			fFontWidth = float(pCurCharInfo->width);
			fFontHeight = float(pCurCharInfo->height);
			fFontMaxHeight = max(fFontHeight, pCurCharInfo->height);
			fFontAdvance = float(pCurCharInfo->advance);

			// NOTE : 폰트 출력에 Width 제한을 둡니다. - [levites]
			if ((fCurX + fFontWidth) - m_v3Position.x > m_fLimitWidth)
			{
				if (m_isMultiLine)
				{
					fCurX = fStanX;
					fCurY += fFontMaxHeight;
				}
				else
				{
					break;
				}
			}

#ifdef ENABLE_MULTI_TEXTLINE
			if (m_isEnterToken && std::find(m_vLineCutPos.begin(), m_vLineCutPos.end(), i) != m_vLineCutPos.end())
			{
				switch (m_hAlign)
				{
				case HORIZONTAL_ALIGN_LEFT:
					fCurX = fStanX;
					break;

				case HORIZONTAL_ALIGN_CENTER:
					fCurX = fStanX + float(m_textWidth - GetTextWidth(i)) / 2.0f;
					break;

				case HORIZONTAL_ALIGN_RIGHT:
					fCurX = fStanX + float(m_textWidth - GetTextWidth(i));
					break;
				}

				//fCurX=fStanX;
				fCurY += m_iLineHeight;
			}
#endif

#ifndef ENABLE_CLIP_MASKING
            if (pClipRect)
            {
                if (fCurY <= pClipRect->top)
                {
                    fCurX += fFontAdvance;
                    continue;
                }
            }
#endif

#ifdef ENABLE_CLIP_MASKING
            float su = pCurCharInfo->left;
            float sv = pCurCharInfo->top;
            float eu = pCurCharInfo->right;
            float ev = pCurCharInfo->bottom;

            if (pClipRect)
            {
                const float width = pCurCharInfo->right - pCurCharInfo->left;
                const float height = pCurCharInfo->bottom - pCurCharInfo->top;

                if (fFontEx <= pClipRect->left)
                {
                    fCurX += fFontAdvance;
                    continue;
                }

                if (fFontSx < pClipRect->left)
                {
                    su += (pClipRect->left - fFontSx) / fFontWidth * width;
                    fFontSx = pClipRect->left;
                }

                if (fFontEy <= pClipRect->top)
                {
                    fCurX += fFontAdvance;
                    continue;
                }

                if (fFontSy < pClipRect->top)
                {
                    sv += (pClipRect->top - fFontSy) / fFontHeight * height;
                    fFontSy = pClipRect->top;
                }

                if (fFontSx >= pClipRect->right)
                {
                    fCurX += fFontAdvance;
                    continue;
                }

                if (fFontEx > pClipRect->right)
                {
                    eu -= (fFontEx - pClipRect->right) / fFontWidth * width;
                    fFontEx = pClipRect->right;
                }

                if (fFontSy >= pClipRect->bottom)
                {
                    fCurX += fFontAdvance;
                    continue;
                }

                if (fFontEy > pClipRect->bottom)
                {
                    ev -= (fFontEy - pClipRect->bottom) / fFontHeight * height;
                    fFontEy = pClipRect->bottom;
                }
            }

#ifdef ENABLE_FIX_MOBS_LAG
			const auto tex = pFontTexture->GetTexture(pCurCharInfo->index);
			auto& batchVertices = verticesMap[tex];

			akVertex[0].px = fFontSx;
			akVertex[0].py = fFontSy;
			akVertex[0].u = su;
			akVertex[0].v = sv;

			akVertex[1].px = fFontSx;
			akVertex[1].py = fFontEy;
			akVertex[1].u = su;
			akVertex[1].v = ev;

			akVertex[2].px = fFontEx;
			akVertex[2].py = fFontSy;
			akVertex[2].u = eu;
			akVertex[2].v = sv;

			akVertex[3].px = fFontEx;
			akVertex[3].py = fFontEy;
			akVertex[3].u = eu;
			akVertex[3].v = ev;

			akVertex[0].diffuse = akVertex[1].diffuse = akVertex[2].diffuse = akVertex[3].diffuse = m_dwColorInfoVector[i];
#endif
#else
#ifdef ENABLE_FIX_MOBS_LAG
			const auto tex = pFontTexture->GetTexture(pCurCharInfo->index);
			auto & batchVertices = verticesMap[tex];

			akVertex[0].px = fFontSx;
			akVertex[0].py = fFontSy;
			akVertex[0].u = pCurCharInfo->left;
			akVertex[0].v = pCurCharInfo->top;

			akVertex[1].px = fFontSx;
			akVertex[1].py = fFontEy;
			akVertex[1].u = pCurCharInfo->left;
			akVertex[1].v = pCurCharInfo->bottom;

			akVertex[2].px = fFontEx;
			akVertex[2].py = fFontSy;
			akVertex[2].u = pCurCharInfo->right;
			akVertex[2].v = pCurCharInfo->top;

			akVertex[3].px = fFontEx;
			akVertex[3].py = fFontEy;
			akVertex[3].u = pCurCharInfo->right;
			akVertex[3].v = pCurCharInfo->bottom;

			akVertex[0].diffuse = akVertex[1].diffuse = akVertex[2].diffuse = akVertex[3].diffuse = m_dwColorInfoVector[i];
#else
			pFontTexture->SelectTexture(pCurCharInfo->index);
			STATEMANAGER.SetTexture(0, pFontTexture->GetD3DTexture());

			akVertex[0].x=fFontSx;
			akVertex[0].y=fFontSy;
			akVertex[0].u=pCurCharInfo->left;
			akVertex[0].v=pCurCharInfo->top;

			akVertex[1].x=fFontSx;
			akVertex[1].y=fFontEy;
			akVertex[1].u=pCurCharInfo->left;
			akVertex[1].v=pCurCharInfo->bottom;

			akVertex[2].x=fFontEx;
			akVertex[2].y=fFontSy;
			akVertex[2].u=pCurCharInfo->right;
			akVertex[2].v=pCurCharInfo->top;

			akVertex[3].x=fFontEx;
			akVertex[3].y=fFontEy;
			akVertex[3].u=pCurCharInfo->right;
			akVertex[3].v=pCurCharInfo->bottom;

			akVertex[0].color = akVertex[1].color = akVertex[2].color = akVertex[3].color = m_dwColorInfoVector[i];
#endif
#endif

#ifdef ENABLE_FIX_MOBS_LAG
			batchVertices.insert(batchVertices.end(), std::begin(akVertex), std::end(akVertex));
#else
			__TextEmitQuad(akVertex);
#endif

			fCurX += fFontAdvance;
		}
	}

#ifdef ENABLE_FIX_MOBS_LAG
	if (!ms_bBatchMode)
	{
	for (auto& p : verticesMap)
		{
			STATEMANAGER.SetTexture(0, p.first->GetD3DTexture());
	
			for (auto f = p.second.begin(), l = p.second.end(); f != l; )
			{
				const auto batchCount = std::min<std::size_t>(LARGE_PDT_VERTEX_BUFFER_SIZE,
					l - f);
	
				if (CGraphicBase::SetPDTStream(&*f, batchCount))
					STATEMANAGER.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0,
						batchCount - 2);
				f += batchCount;
			}
		}
	}
#endif
#endif
	}

	if (m_isCursor && !(CTimer::instance().GetCurrentSecond() >= m_isBlinking && (ELTimer_GetMSec() / 400) % 2))
	{
		// Draw Cursor
		float sx, sy, ex, ey;
		TDiffuse diffuse;

		int curpos = CIME::GetCurPos();
		int compend = curpos + CIME::GetCompLen();

		__GetTextPos(curpos, &sx, &sy);

		// If Composition
		if (curpos < compend)
		{
			diffuse = 0x7fffffff;
			__GetTextPos(compend, &ex, &sy);
		}
		else
		{
			diffuse = 0xffffffff;
			ex = sx + 2;
		}

		// FOR_ARABIC_ALIGN
		if (defCodePage == CP_ARABIC)
		{
			sx += m_v3Position.x - m_textWidth;
			ex += m_v3Position.x - m_textWidth;
			sy += m_v3Position.y;
			ey = sy + m_textHeight;
		}
		else
		{
			sx += m_v3Position.x;
			sy += m_v3Position.y;
			ex += m_v3Position.x;
			ey = sy + m_textHeight;
		}

		switch (m_hAlign)
		{
			case HORIZONTAL_ALIGN_RIGHT:
				sx -= m_textWidth;
				ex -= m_textWidth;
				break;

			case HORIZONTAL_ALIGN_CENTER:
				sx -= float(m_textWidth) / 2.0f;
				ex -= float(m_textWidth) / 2.0f;
				break;
		}

		switch (m_vAlign)
		{
			case VERTICAL_ALIGN_BOTTOM:
				sy -= m_textHeight;
				break;

			case VERTICAL_ALIGN_CENTER:
				sy -= float(m_textHeight) / 2.0f;
				break;
		}

		TPDTVertex vertices[4];
		vertices[0].diffuse = diffuse;
		vertices[1].diffuse = diffuse;
		vertices[2].diffuse = diffuse;
		vertices[3].diffuse = diffuse;
		vertices[0].position = TPosition(sx, sy, 0.0f);
		vertices[1].position = TPosition(ex, sy, 0.0f);
		vertices[2].position = TPosition(sx, ey, 0.0f);
		vertices[3].position = TPosition(ex, ey, 0.0f);

		STATEMANAGER.SetTexture(0, nullptr);


		// 2004.11.18.myevan.DrawIndexPrimitiveUP -> DynamicVertexBuffer
		CGraphicBase::SetDefaultIndexBuffer(CGraphicBase::DEFAULT_IB_FILL_RECT);
		if (CGraphicBase::SetPDTStream(vertices, 4))
			STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);

		int ulbegin = CIME::GetULBegin();
		int ulend = CIME::GetULEnd();

		if (ulbegin < ulend)
		{
			__GetTextPos(curpos + ulbegin, &sx, &sy);
			__GetTextPos(curpos + ulend, &ex, &sy);

			sx += m_v3Position.x;
			sy += m_v3Position.y + m_textHeight;
			ex += m_v3Position.x;
			ey = sy + 2;

			vertices[0].diffuse = 0xFFFF0000;
			vertices[1].diffuse = 0xFFFF0000;
			vertices[2].diffuse = 0xFFFF0000;
			vertices[3].diffuse = 0xFFFF0000;
			vertices[0].position = TPosition(sx, sy, 0.0f);
			vertices[1].position = TPosition(ex, sy, 0.0f);
			vertices[2].position = TPosition(sx, ey, 0.0f);
			vertices[3].position = TPosition(ex, ey, 0.0f);

			STATEMANAGER.DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 4, 2, c_FillRectIndices, D3DFMT_INDEX16, vertices, sizeof(TPDTVertex));
		}
	}


	STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);

	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, dwFogEnable);
	STATEMANAGER.SetRenderState(D3DRS_LIGHTING, dwLighting);

	if (m_hyperlinkVector.size() != 0)
	{
		int lx = gs_mx - m_v3Position.x;
		int ly = gs_my - m_v3Position.y;

		if (GetDefaultCodePage() == CP_ARABIC)
		{
			lx = -lx;
			ly = -ly + m_textHeight;
		}

		if (lx >= 0 && ly >= 0 && lx < m_textWidth && ly < m_textHeight)
		{
			std::vector<SHyperlink>::iterator it = m_hyperlinkVector.begin();

			while (it != m_hyperlinkVector.end())
			{
				SHyperlink & link = *it++;
				if (lx >= link.sx && lx < link.ex)
				{
					gs_hyperlinkText = link.text;
					break;
				}
			}
		}
	}
#ifdef ENABLE_TEXTLINE_EMOJI
	if (m_emojiVector.size() != 0)
	{
		for (std::vector<SEmoji>::iterator itor = m_emojiVector.begin(); itor != m_emojiVector.end(); ++itor)
		{
			SEmoji & rEmo = *itor;
			if (rEmo.pInstance)
			{
				rEmo.pInstance->SetPosition(fStanX + rEmo.x, (fStanY + 7.0) - (rEmo.pInstance->GetHeight() / 2));
#ifdef ENABLE_CLIP_MASKING
				rEmo.pInstance->Render(pClipRect);
#else
				rEmo.pInstance->Render();
#endif
			}
		}
	}
#endif

#ifdef ENABLE_EMOTICONS_SYSTEM
	if (m_emoticonVector.size() != 0)
	{
		for (std::vector<SEmoticon>::iterator itor = m_emoticonVector.begin(); itor != m_emoticonVector.end(); ++itor)
		{
			SEmoticon& rTmo = *itor;
			if (rTmo.pInstance)
			{
				rTmo.pInstance->SetPosition(fStanX + rTmo.t, fStanY - 2);
#ifdef ENABLE_CLIP_MASKING
				rTmo.pInstance->Render(pClipRect);
#else
				rTmo.pInstance->Render();
#endif
			}
		}
	}
#endif

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CGraphicTextInstance::CreateSystem(UINT uCapacity)
{
	ms_kPool.Create(uCapacity);
}

void CGraphicTextInstance::DestroySystem()
{
	ms_kPool.Destroy();
}

CGraphicTextInstance* CGraphicTextInstance::New()
{
	return ms_kPool.Alloc();
}

void CGraphicTextInstance::Delete(CGraphicTextInstance* pkInst)
{
	pkInst->Destroy();
	ms_kPool.Free(pkInst);
}

void CGraphicTextInstance::ShowCursor()
{
	m_isCursor = true;
}

void CGraphicTextInstance::HideCursor()
{
	m_isCursor = false;
}

#ifdef INSIDE_RENDER
bool CGraphicTextInstance::IsShowCursor()
{
	return m_isCursor;
}
#endif

void CGraphicTextInstance::ShowOutLine()
{
	m_isOutline = true;
}

void CGraphicTextInstance::HideOutLine()
{
	m_isOutline = false;
}

void CGraphicTextInstance::SetColor(DWORD color)
{
	if (m_dwTextColor != color)
	{
		for (int i = 0; i < m_pCharInfoVector.size(); ++i)
		{
			//TraceError("size %d oldcolor %d newcolor %d", m_dwColorInfoVector.size(), m_dwColorInfoVector[i], m_dwTextColor);
			if (m_dwColorInfoVector[i] == m_dwTextColor)
				m_dwColorInfoVector[i] = color;
		}
		m_dwTextColor = color;
	}
}

void CGraphicTextInstance::SetColor(float r, float g, float b, float a)
{
	SetColor(D3DXCOLOR(r, g, b, a));
}

void CGraphicTextInstance::SetOutLineColor(DWORD color)
{
	m_dwOutLineColor = color;
}

void CGraphicTextInstance::SetOutLineColor(float r, float g, float b, float a)
{
	m_dwOutLineColor = D3DXCOLOR(r, g, b, a);
}

void CGraphicTextInstance::SetSecret(bool Value)
{
	m_isSecret = Value;
}

void CGraphicTextInstance::SetOutline(bool Value)
{
	m_isOutline = Value;
}

void CGraphicTextInstance::SetFeather(bool Value)
{
	if (Value)
	{
		m_fFontFeather = c_fFontFeather;
	}
	else
	{
		m_fFontFeather = 0.0f;
	}
}

void CGraphicTextInstance::SetMultiLine(bool Value)
{
	m_isMultiLine = Value;
}

void CGraphicTextInstance::SetHorizonalAlign(int hAlign)
{
	m_hAlign = hAlign;
}

void CGraphicTextInstance::SetVerticalAlign(int vAlign)
{
	m_vAlign = vAlign;
}

void CGraphicTextInstance::SetMax(int iMax)
{
	m_iMax = iMax;
}

void CGraphicTextInstance::SetLimitWidth(float fWidth)
{
	m_fLimitWidth = fWidth;
}

void CGraphicTextInstance::SetValueString(const std::string& c_stValue)
{
	if (0 == m_stText.compare(c_stValue))
		return;

	m_stText = c_stValue;

#ifdef ENABLE_MULTI_TEXTLINE
	m_vLineCutPos.clear();
	std::size_t findN = m_stText.find("\n");
	while (findN != std::string::npos)
	{
		m_vLineCutPos.push_back(findN);
		m_stText.erase(findN, 1);
		findN = m_stText.find("\n");
	}

	// Let's hardcode the first line :)
	// So if the vector is not empty it mean it found \n in it
	// and if we append first pos in vector it will consider it 
	// was an \n in the beggining of the std::string
	if (m_vLineCutPos.size())
		m_vLineCutPos.push_back(0);
#endif

	m_isUpdate = false;
}

void CGraphicTextInstance::SetValue(const char *c_szText, size_t len)
{
	if (0 == m_stText.compare(c_szText))
		return;

	m_isBlinking = CTimer::instance().GetCurrentSecond() + 1.5f;

	m_stText = c_szText;

#ifdef ENABLE_MULTI_TEXTLINE
	m_vLineCutPos.clear();
	std::size_t findN = m_stText.find("\n");
	while (findN != std::string::npos)
	{
		m_vLineCutPos.push_back(findN);
		m_stText.erase(findN, 1);
		findN = m_stText.find("\n");
	}

	if(m_vLineCutPos.size())
		m_vLineCutPos.push_back(0);
#endif

	m_isUpdate = false;
}

#ifdef ENABLE_MULTILANGUAGE_CHATTING
void CGraphicTextInstance::SetColorRange(DWORD dwColor, int iStart, int iEnd)
{
	for (size_t i = 0; i < m_dwColorInfoVector.size(); ++i)
	{
	if (i >= (size_t)iStart && i < (size_t)iEnd)
		m_dwColorInfoVector[i] = dwColor;
	}
}

void CGraphicTextInstance::SetValue(const char *c_szText, const DWORD *c_pdwCodePage)
{
	if (c_pdwCodePage)
		m_dwCodePage = *c_pdwCodePage;
	else
		m_dwCodePage = 0;
	SetValue(c_szText, -1);
}
#endif

void CGraphicTextInstance::SetPosition(float fx, float fy, float fz)
{
	m_v3Position.x = fx;
	m_v3Position.y = fy;
	m_v3Position.z = fz;
}

void CGraphicTextInstance::SetTextPointer(CGraphicText* pText)
{
	m_roText = pText;
	m_isUpdate = false;
	Update();
}

const std::string & CGraphicTextInstance::GetValueStringReference()
{
	return m_stText;
}

WORD CGraphicTextInstance::GetTextLineCount()
{
	CGraphicFontTexture::TCharacterInfomation* pCurCharInfo;
	CGraphicFontTexture::TPCharacterInfomationVector::iterator itor;

	float fx = 0.0f;
	WORD wLineCount = 1;
	for (itor = m_pCharInfoVector.begin(); itor != m_pCharInfoVector.end(); ++itor)
	{
		pCurCharInfo = *itor;

		float fFontWidth = float(pCurCharInfo->width);
		float fFontAdvance = float(pCurCharInfo->advance);

		if (fx + fFontWidth > m_fLimitWidth)
		{
			fx = 0.0f;
			++wLineCount;
		}

		fx += fFontAdvance;
	}

#ifdef ENABLE_MULTI_TEXTLINE
	if (m_isEnterToken && m_vLineCutPos.size())
	{
		return m_vLineCutPos.size();
	}
#endif

	return wLineCount;
}

#ifdef ENABLE_PYTHON_EVENT_FUNCTIONS
WORD CGraphicTextInstance::GetLineHeight()
{
	return m_textHeight;
}
#endif

void CGraphicTextInstance::GetTextSize(int* pRetWidth, int* pRetHeight)
{
#ifdef ENABLE_MULTI_TEXTLINE
	if (m_isEnterToken && m_vLineCutPos.size())
	{
		GetMultiTextSize(pRetWidth, pRetHeight);
		return;
	}
#endif

	* pRetWidth = m_textWidth;
	*pRetHeight = m_textHeight;
}

#ifdef ENABLE_SUNG_MAHI_TOWER
void CGraphicTextInstance::GetCharacterWidth(short* sWidth)
{
	CGraphicFontTexture::TCharacterInfomation* pCurCharInfo;
	CGraphicFontTexture::TPCharacterInfomationVector::iterator itor;

	*sWidth = 0;
	for (itor = m_pCharInfoVector.begin(); itor != m_pCharInfoVector.end(); ++itor)
	{
		pCurCharInfo = *itor;
		
		*sWidth = pCurCharInfo->width;
		break;
	}
}
#endif

int CGraphicTextInstance::PixelPositionToCharacterPosition(int iPixelPosition)
{
	int icurPosition = 0;
	for (int i = 0; i < (int)m_pCharInfoVector.size(); ++i)
	{
		CGraphicFontTexture::TCharacterInfomation* pCurCharInfo = m_pCharInfoVector[i];
		icurPosition += pCurCharInfo->width;

		if (iPixelPosition < icurPosition)
			return i;
	}

	return -1;
}

int CGraphicTextInstance::GetHorizontalAlign()
{
	return m_hAlign;
}

#ifdef INSIDE_RENDER
void CGraphicTextInstance::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
{
	if (m_textWidth == 0 || m_textHeight == 0)
		return;

	m_bUseRenderingRect = true;

	float fWidth = float(m_textWidth);
	float fHeight = float(m_textHeight);

	m_RenderingRect.left = fWidth * fLeft;
	m_RenderingRect.top = fHeight * fTop;
	m_RenderingRect.right = fWidth * fRight;
	m_RenderingRect.bottom = fHeight * fBottom;
}

void CGraphicTextInstance::iSetRenderingRect(int iLeft, int iTop, int iRight, int iBottom)
{
	if (m_textWidth == 0 || m_textHeight == 0)
		return;

	m_bUseRenderingRect = true;

	m_RenderingRect.left = iLeft;
	m_RenderingRect.top = iTop;
	m_RenderingRect.right = iRight;
	m_RenderingRect.bottom = iBottom;
}

void CGraphicTextInstance::SetRenderBox(RECT& renderBox)
{
	memcpy(&m_renderBox, &renderBox, sizeof(m_renderBox));
}
#endif

void CGraphicTextInstance::__Initialize()
{
	m_roText = nullptr;

	m_hAlign = HORIZONTAL_ALIGN_LEFT;
	m_vAlign = VERTICAL_ALIGN_TOP;

#ifdef ENABLE_MULTI_TEXTLINE
	m_isEnterToken = true;
	m_iLineHeight = 12; // default line height
	m_vLineCutPos.clear();
#endif

	m_iMax = 0;
	m_fLimitWidth = 1600.0f;

	m_isCursor = false;
	m_isSecret = false;
	m_isMultiLine = false;

	m_isBlinking = CTimer::instance().GetCurrentSecond();

	m_isOutline = false;
	m_fFontFeather = c_fFontFeather;

	m_isUpdate = false;

	m_textWidth = 0;
	m_textHeight = 0;

	m_v3Position.x = m_v3Position.y = m_v3Position.z = 0.0f;

	m_dwOutLineColor = 0xff000000;
#ifdef ENABLE_MULTILANGUAGE_CHATTING
	m_dwCodePage = 0;
	m_dwLangID = 0;
#endif
#ifdef INSIDE_RENDER
	memset(&m_RenderingRect, 0, sizeof(RECT));
	m_bUseRenderingRect = false;

	memset(&m_renderBox, 0, sizeof(m_renderBox));
	m_startPos = m_endPos = 0;
	m_isFixedRenderPos = false;
#endif
}

#ifdef ENABLE_MULTI_TEXTLINE
void CGraphicTextInstance::DisableEnterToken()
{
	m_isEnterToken = false;
}

void CGraphicTextInstance::SetLineHeight(int iHeight)
{
	m_iLineHeight = iHeight;
}

void CGraphicTextInstance::GetMultiTextSize(int* pRetWidth, int* pRetHeight)
{
	int iMaxWidth = 0;
	int iMaxHeigh = 0;
	int iWidth = 0;

	for (int i = 0; i < (int)m_pCharInfoVector.size(); ++i)
	{
		CGraphicFontTexture::TCharacterInfomation* pCurCharInfo = m_pCharInfoVector[i];
		float fFontWidth = float(pCurCharInfo->width);
		float fFontAdvance = float(pCurCharInfo->advance);

		if (std::find(m_vLineCutPos.begin(), m_vLineCutPos.end(), i) != m_vLineCutPos.end())
		{
			if (iMaxWidth < iWidth)
				iMaxWidth = iWidth;

			iWidth = 0;
			iMaxHeigh += i ? m_iLineHeight : 0;
		}
		else if (i == (int)m_pCharInfoVector.size() - 1)
		{
			if (iMaxWidth < iWidth + fFontAdvance)
				iMaxWidth = iWidth + fFontAdvance;

			iWidth = 0;
			iMaxHeigh += i ? m_iLineHeight : 0;
		}

		iWidth += fFontAdvance;
	}

	m_textWidth = iMaxWidth;
	m_textHeight = iMaxHeigh;

	*pRetWidth = iMaxWidth;
	*pRetHeight = iMaxHeigh;
}

// This function return the width from iStart position until it find \n again or the end of the std::string
float CGraphicTextInstance::GetTextWidth(int iStart)
{
	float iMaxWidth = 0;
	float iWidth = 0;

	for (int i = iStart; i < (int)m_pCharInfoVector.size(); ++i)
	{
		CGraphicFontTexture::TCharacterInfomation* pCurCharInfo = m_pCharInfoVector[i];
		float fFontWidth = float(pCurCharInfo->width);
		float fFontAdvance = float(pCurCharInfo->advance);

		if (i != iStart && (std::find(m_vLineCutPos.begin(), m_vLineCutPos.end(), i) != m_vLineCutPos.end()))
			return iWidth;
		else if (i == (int)m_pCharInfoVector.size() - 1)
			return iWidth + fFontAdvance;

		iWidth += fFontAdvance;
	}

	return iWidth;
}
#endif

void CGraphicTextInstance::Destroy()
{
	m_stText = "";
	m_pCharInfoVector.clear();
	m_dwColorInfoVector.clear();
	m_hyperlinkVector.clear();

#ifdef ENABLE_TEXTLINE_EMOJI
	if (m_emojiVector.size() != 0)
	{
		for (std::vector<SEmoji>::iterator itor = m_emojiVector.begin(); itor != m_emojiVector.end(); ++itor)
		{
			SEmoji & rEmo = *itor;
			if (rEmo.pInstance)
			{
				CGraphicImageInstance::Delete(rEmo.pInstance);
				rEmo.pInstance = nullptr;
			}
		}
	}
	m_emojiVector.clear();
#endif

#ifdef ENABLE_EMOTICONS_SYSTEM
	if (m_emoticonVector.size() != 0)
	{
		for (std::vector<SEmoticon>::iterator itor = m_emoticonVector.begin(); itor != m_emoticonVector.end(); ++itor)
		{
			SEmoticon& rTmo = *itor;
			if (rTmo.pInstance)
			{
				CGraphicExpandedImageInstance::Delete(rTmo.pInstance);
				rTmo.pInstance = nullptr;
			}
		}
	}
	m_emoticonVector.clear();
#endif

	__Initialize();
}

CGraphicTextInstance::CGraphicTextInstance()
{
	__Initialize();
}

CGraphicTextInstance::~CGraphicTextInstance()
{
	Destroy();
}
