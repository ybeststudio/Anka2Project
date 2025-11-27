#include "StdAfx.h"
#include "InstanceBase.h"
#include "resource.h"
#include "PythonTextTail.h"
#include "PythonCharacterManager.h"
#include "PythonGuild.h"
#include "Locale.h"
#include "MarkManager.h"

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	#include "PythonApplication.h"
#endif

#if defined(ENABLE_SHOW_MOB_INFO) || defined(ENABLE_GRAPHIC_ON_OFF)
	#include "PythonSystem.h"
#endif

const D3DXCOLOR c_TextTail_Player_Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR c_TextTail_Monster_Color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
const D3DXCOLOR c_TextTail_Item_Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR c_TextTail_Chat_Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
const D3DXCOLOR c_TextTail_Info_Color = D3DXCOLOR(1.0f, 0.785f, 0.785f, 1.0f);
const D3DXCOLOR c_TextTail_Guild_Name_Color = 0xFFEFD3FF;
const float c_TextTail_Name_Position = -10.0f;
const float c_fxMarkPosition = 1.5f;
#ifdef ENABLE_TITLE_SYSTEM
const float c_fyGuildNamePosition = 25.0f;
const float c_fyMarkPosition = 25.0f + 11.0f;
const float c_fysTitlePosition = 12.0f;
const float c_fyGuildNamePositionOld = 15.0f;
const float c_fyMarkPositionOld = 15.0f + 11.0f;
#else
const float c_fyGuildNamePosition = 15.0f;
const float c_fyMarkPosition = 15.0f + 11.0f;
#endif
BOOL bPKTitleEnable = TRUE;

long gs_TextTail_LivingTime = 5000;

long TextTail_GetLivingTime()
{
	assert(gs_TextTail_LivingTime>1000);
	return gs_TextTail_LivingTime;
}

void TextTail_SetLivingTime(long livingTime)
{
	gs_TextTail_LivingTime = livingTime;
}

CGraphicText* ms_pFont = nullptr;

void CPythonTextTail::GetInfo(std::string* pstInfo)
{
	char szInfo[256];
	sprintf(szInfo, "TextTail: ChatTail %d, ChrTail (Map %d, List %d), ItemTail (Map %d, List %d), Pool %d", 
		m_ChatTailMap.size(), 
		m_CharacterTextTailMap.size(), m_CharacterTextTailList.size(), 
		m_ItemTextTailMap.size(), m_ItemTextTailList.size(), 
		m_TextTailPool.GetCapacity());

	pstInfo->append(szInfo);
}

void CPythonTextTail::UpdateAllTextTail()
{
	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
	if (pInstance)
	{
		TPixelPosition pixelPos;
		pInstance->NEW_GetPixelPosition(&pixelPos);

		TTextTailMap::iterator itorMap;

		for (itorMap = m_CharacterTextTailMap.begin(); itorMap != m_CharacterTextTailMap.end(); ++itorMap)
		{
			UpdateDistance(pixelPos, itorMap->second);
		}

		for (itorMap = m_ItemTextTailMap.begin(); itorMap != m_ItemTextTailMap.end(); ++itorMap)
		{
			UpdateDistance(pixelPos, itorMap->second);
		}

		for (TChatTailMap::iterator itorChat=m_ChatTailMap.begin(); itorChat!=m_ChatTailMap.end(); ++itorChat)
		{
			UpdateDistance(pixelPos, itorChat->second);

			if (itorChat->second->bNameFlag)
			{
				DWORD dwVID = itorChat->first;
				ShowCharacterTextTail(dwVID);
			}
		}
	}
}

void CPythonTextTail::UpdateShowingTextTail()
{
	TTextTailList::iterator itor;

	for (itor = m_ItemTextTailList.begin(); itor != m_ItemTextTailList.end(); ++itor)
	{
		UpdateTextTail(*itor);
	}

	for (TChatTailMap::iterator itorChat=m_ChatTailMap.begin(); itorChat!=m_ChatTailMap.end(); ++itorChat)
	{
		UpdateTextTail(itorChat->second);
	}

	for (itor = m_CharacterTextTailList.begin(); itor != m_CharacterTextTailList.end(); ++itor)
	{
		TTextTail* pTextTail =*itor;
		UpdateTextTail(pTextTail);

		TChatTailMap::iterator itor = m_ChatTailMap.find(pTextTail->dwVirtualID);
		if (m_ChatTailMap.end() != itor)
		{
			TTextTail* pChatTail = itor->second;
			if (pChatTail->bNameFlag)
			{
				pTextTail->y = pChatTail->y - 17.0f;
			}
		}
	}
}

void CPythonTextTail::UpdateTextTail(TTextTail* pTextTail)
{
	if (!pTextTail->pOwner)
		return;

	CPythonGraphic & rpyGraphic = CPythonGraphic::Instance();
	rpyGraphic.Identity();

	const D3DXVECTOR3 & c_rv3Position = pTextTail->pOwner->GetPosition();
	rpyGraphic.ProjectPosition(c_rv3Position.x,
							   c_rv3Position.y,
							   c_rv3Position.z + pTextTail->fHeight,
							   &pTextTail->x,
							   &pTextTail->y,
							   &pTextTail->z);

	pTextTail->x = floorf(pTextTail->x);
	pTextTail->y = floorf(pTextTail->y);

	if (pTextTail->fDistanceFromPlayer < 1300.0f)
	{
		pTextTail->z = 0.0f;
	}
	else
	{
		pTextTail->z = pTextTail->z* CPythonGraphic::Instance().GetOrthoDepth()* -1.0f;
		pTextTail->z += 10.0f;
	}
}

void CPythonTextTail::ArrangeTextTail()
{
	TTextTailList::iterator itor;
	TTextTailList::iterator itorCompare;

	DWORD dwTime = CTimer::Instance().GetCurrentMillisecond();

	for (itor = m_ItemTextTailList.begin(); itor != m_ItemTextTailList.end(); ++itor)
	{
		TTextTail* pInsertTextTail =*itor;

		int yTemp = 5;
		int LimitCount = 0;

		for (itorCompare = m_ItemTextTailList.begin(); itorCompare != m_ItemTextTailList.end();)
		{
			TTextTail* pCompareTextTail =*itorCompare;

			if (*itorCompare ==*itor)
			{
				++itorCompare;
				continue;
			}

			if (LimitCount >= 20)
				break;

			if (isIn(pInsertTextTail, pCompareTextTail))
			{
				pInsertTextTail->y = (pCompareTextTail->y + pCompareTextTail->yEnd + yTemp);

				itorCompare = m_ItemTextTailList.begin();
				++LimitCount;
				continue;
			}

			++itorCompare;
		}


		if (pInsertTextTail->pOwnerTextInstance)
		{
			pInsertTextTail->pOwnerTextInstance->SetPosition(pInsertTextTail->x, pInsertTextTail->y, pInsertTextTail->z);
			pInsertTextTail->pOwnerTextInstance->Update();

			pInsertTextTail->pTextInstance->SetColor(pInsertTextTail->Color.r, pInsertTextTail->Color.g, pInsertTextTail->Color.b);
			pInsertTextTail->pTextInstance->SetPosition(pInsertTextTail->x, pInsertTextTail->y + 15.0f, pInsertTextTail->z);
			pInsertTextTail->pTextInstance->Update();

		}
		else
		{
			pInsertTextTail->pTextInstance->SetColor(pInsertTextTail->Color.r, pInsertTextTail->Color.g, pInsertTextTail->Color.b);
			pInsertTextTail->pTextInstance->SetPosition(pInsertTextTail->x, pInsertTextTail->y, pInsertTextTail->z);
			pInsertTextTail->pTextInstance->Update();

		}
	}

	for (itor = m_CharacterTextTailList.begin(); itor != m_CharacterTextTailList.end(); ++itor)
	{
		TTextTail* pTextTail =*itor;

		float fxAdd = 0.0f;

#ifdef ENABLE_TITLE_SYSTEM
		CGraphicTextInstance* pTitleSystem = pTextTail->pTitleSystemTextInstance;
		if (pTitleSystem)
		{
			int iWidth, iHeight;
			pTitleSystem->GetTextSize(&iWidth, &iHeight);
			pTitleSystem->SetPosition(pTextTail->x, pTextTail->y - c_fysTitlePosition, pTextTail->z);
			pTitleSystem->Update();
		}
#endif

		CGraphicMarkInstance* pMarkInstance = pTextTail->pMarkInstance;
		CGraphicTextInstance* pGuildNameInstance = pTextTail->pGuildNameTextInstance;
		if (pMarkInstance && pGuildNameInstance)
		{
			int iWidth, iHeight;
			int iImageHalfSize = pMarkInstance->GetWidth()/2 + c_fxMarkPosition;
			pGuildNameInstance->GetTextSize(&iWidth, &iHeight);

#ifdef ENABLE_TITLE_SYSTEM
			if (!pTitleSystem)
			{
				pMarkInstance->SetPosition(pTextTail->x - iWidth/2 - iImageHalfSize, pTextTail->y - c_fyMarkPositionOld);
				pGuildNameInstance->SetPosition(pTextTail->x + iImageHalfSize, pTextTail->y - c_fyGuildNamePositionOld, pTextTail->z);
			}
			else
			{
				pMarkInstance->SetPosition(pTextTail->x - iWidth/2 - iImageHalfSize, pTextTail->y - c_fyMarkPosition);
				pGuildNameInstance->SetPosition(pTextTail->x + iImageHalfSize, pTextTail->y - c_fyGuildNamePosition, pTextTail->z);
			}
#else
			pMarkInstance->SetPosition(pTextTail->x - iWidth/2 - iImageHalfSize, pTextTail->y - c_fyMarkPosition);
			pGuildNameInstance->SetPosition(pTextTail->x + iImageHalfSize, pTextTail->y - c_fyGuildNamePosition, pTextTail->z);
#endif
			pGuildNameInstance->Update();
		}

		int iNameWidth, iNameHeight;
		pTextTail->pTextInstance->GetTextSize(&iNameWidth, &iNameHeight);

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		CGraphicImageInstance* pLanguageInstance = pTextTail->pLanguageInstance;
		CGraphicImageInstance* pLanguageInstance2 = pTextTail->pLanguageInstance2;
#endif

		CGraphicTextInstance* pTitle = pTextTail->pTitleTextInstance;
		if (pTitle)
		{
			int iTitleWidth, iTitleHeight;
			pTitle->GetTextSize(&iTitleWidth, &iTitleHeight);

			fxAdd = 2.0f;
			pTitle->SetPosition(pTextTail->x - (iNameWidth / 2) - fxAdd, pTextTail->y, pTextTail->z);
			pTitle->Update();

			CGraphicTextInstance* pLevel = pTextTail->pLevelTextInstance;
			if (pLevel)
			{
				int iLevelWidth, iLevelHeight;
				pLevel->GetTextSize(&iLevelWidth, &iLevelHeight);
				pLevel->SetPosition(pTextTail->x - (iNameWidth / 2) - fxAdd - 4.0f - iTitleWidth, pTextTail->y, pTextTail->z);
				pLevel->Update();

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
				if (pLanguageInstance)
				{
					int iLevelWidth, iLevelHeight;
					pLevel->GetTextSize(&iLevelWidth, &iLevelHeight);
					pLanguageInstance->SetPosition(pTextTail->x - (iNameWidth / 2) - iTitleWidth - iLevelWidth - pLanguageInstance->GetWidth() - 12.0f, pTextTail->y - 10.0f);
				}

				if (pLanguageInstance2)
				{
					int iLevelWidth, iLevelHeight;
					pLevel->GetTextSize(&iLevelWidth, &iLevelHeight);
					pLanguageInstance2->SetPosition(pTextTail->x - (iNameWidth / 2) - iTitleWidth - iLevelWidth - pLanguageInstance2->GetWidth() - 32.0f, pTextTail->y - 10.0f);
				}
#endif
			}
		}
		else
		{
			fxAdd = 2.0f;
			CGraphicTextInstance* pLevel = pTextTail->pLevelTextInstance;
			if (pLevel)
			{
				int iLevelWidth, iLevelHeight;
				pLevel->GetTextSize(&iLevelWidth, &iLevelHeight);
				pLevel->SetPosition(pTextTail->x - (iNameWidth / 2) - fxAdd, pTextTail->y, pTextTail->z);
				pLevel->Update();

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
				if (pLanguageInstance)
				{
					pLanguageInstance->SetPosition(pTextTail->x - (iNameWidth / 2) - iLevelWidth - pLanguageInstance->GetWidth() - 8.0f, pTextTail->y - 10.0f);
				}

				if (pLanguageInstance2)
				{
					pLanguageInstance2->SetPosition(pTextTail->x - (iNameWidth / 2) - iLevelWidth - pLanguageInstance2->GetWidth() - 30.0f, pTextTail->y - 10.0f);
				}
#endif
			}
		}

		pTextTail->pTextInstance->SetColor(pTextTail->Color.r, pTextTail->Color.g, pTextTail->Color.b);
		pTextTail->pTextInstance->SetPosition(pTextTail->x + fxAdd, pTextTail->y, pTextTail->z);
		pTextTail->pTextInstance->Update();

#ifdef ENABLE_SHOW_MOB_INFO
		CGraphicTextInstance* pAIFlag = pTextTail->pAIFlagTextInstance;
		if (pAIFlag)
		{
			pAIFlag->SetColor(pTextTail->Color.r, pTextTail->Color.g, pTextTail->Color.b);
			pAIFlag->SetPosition(pTextTail->x + fxAdd + (iNameWidth / 2) + 1.0f, pTextTail->y, pTextTail->z);
			pAIFlag->Update();
		}
#endif
	}

	for (TChatTailMap::iterator itorChat=m_ChatTailMap.begin(); itorChat!=m_ChatTailMap.end();)
	{
		TTextTail* pTextTail = itorChat->second;

		if (pTextTail->LivingTime < dwTime)
		{
			DeleteTextTail(pTextTail);
			itorChat = m_ChatTailMap.erase(itorChat);
			continue;
		}
		else
			++itorChat;

		pTextTail->pTextInstance->SetColor(pTextTail->Color);
		pTextTail->pTextInstance->SetPosition(pTextTail->x, pTextTail->y, pTextTail->z);
		pTextTail->pTextInstance->Update();
	}
}

void CPythonTextTail::Render()
{
	TTextTailList::iterator itor;

	for (itor = m_CharacterTextTailList.begin(); itor != m_CharacterTextTailList.end(); ++itor)
	{
		TTextTail* pTextTail =*itor;
		pTextTail->pTextInstance->Render();
		if (pTextTail->pMarkInstance && pTextTail->pGuildNameTextInstance)
		{
			pTextTail->pMarkInstance->Render();
			pTextTail->pGuildNameTextInstance->Render();
		}
		if (pTextTail->pTitleTextInstance)
		{
			pTextTail->pTitleTextInstance->Render();
		}
#ifdef ENABLE_SHOW_MOB_INFO
		if (pTextTail->pLevelTextInstance && (pTextTail->bIsPC == TRUE || CPythonSystem::Instance().IsShowMobLevel()))
#else
		if (pTextTail->pLevelTextInstance)
#endif
		{
			pTextTail->pLevelTextInstance->Render();
		}
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		if (pTextTail->pLanguageInstance)
		{
			pTextTail->pLanguageInstance->Render();
		}

		if (pTextTail->pLanguageInstance2)
		{
			pTextTail->pLanguageInstance2->Render();
		}
#endif
#ifdef ENABLE_TITLE_SYSTEM
		if (pTextTail->pTitleSystemTextInstance)
		{
			pTextTail->pTitleSystemTextInstance->Render();
		}
#endif
#ifdef ENABLE_SHOW_MOB_INFO
		if (pTextTail->pAIFlagTextInstance && CPythonSystem::Instance().IsShowMobAIFlag())
		{
			pTextTail->pAIFlagTextInstance->Render();
		}
#endif
	}

	for (itor = m_ItemTextTailList.begin(); itor != m_ItemTextTailList.end(); ++itor)
	{
		TTextTail* pTextTail =*itor;

		RenderTextTailBox(pTextTail);
		pTextTail->pTextInstance->Render();
		if (pTextTail->pOwnerTextInstance)
			pTextTail->pOwnerTextInstance->Render();
	}

	for (TChatTailMap::iterator itorChat = m_ChatTailMap.begin(); itorChat!=m_ChatTailMap.end(); ++itorChat)
	{
		TTextTail* pTextTail = itorChat->second;
		if (pTextTail->pOwner->isShow())
			RenderTextTailName(pTextTail);
	}
}

void CPythonTextTail::RenderTextTailBox(TTextTail* pTextTail)
{
	CPythonGraphic::Instance().SetDiffuseColor(0.0f, 0.0f, 0.0f, 1.0f);
	CPythonGraphic::Instance().RenderBox2d(pTextTail->x + pTextTail->xStart, pTextTail->y + pTextTail->yStart, pTextTail->x + pTextTail->xEnd, pTextTail->y + pTextTail->yEnd, pTextTail->z);

	CPythonGraphic::Instance().SetDiffuseColor(0.0f, 0.0f, 0.0f, 0.3f);
	CPythonGraphic::Instance().RenderBar2d(pTextTail->x + pTextTail->xStart, pTextTail->y + pTextTail->yStart, pTextTail->x + pTextTail->xEnd, pTextTail->y + pTextTail->yEnd, pTextTail->z);
}

void CPythonTextTail::RenderTextTailName(TTextTail* pTextTail)
{
	pTextTail->pTextInstance->Render();
}

void CPythonTextTail::HideAllTextTail()
{
	m_CharacterTextTailList.clear();
	m_ItemTextTailList.clear();
}

void CPythonTextTail::UpdateDistance(const TPixelPosition & c_rCenterPosition, TTextTail* pTextTail)
{
	const D3DXVECTOR3 & c_rv3Position = pTextTail->pOwner->GetPosition();
	D3DXVECTOR2 v2Distance(c_rv3Position.x - c_rCenterPosition.x, -c_rv3Position.y - c_rCenterPosition.y);
	pTextTail->fDistanceFromPlayer = D3DXVec2Length(&v2Distance);
}

void CPythonTextTail::ShowAllTextTail()
{
	TTextTailMap::iterator itor;
	for (itor = m_CharacterTextTailMap.begin(); itor != m_CharacterTextTailMap.end(); ++itor)
	{
		TTextTail* pTextTail = itor->second;
		if (pTextTail->fDistanceFromPlayer < 3500.0f)
			ShowCharacterTextTail(itor->first);
	}

	for (itor = m_ItemTextTailMap.begin(); itor != m_ItemTextTailMap.end(); ++itor)
	{
		TTextTail* pTextTail = itor->second;
		if (pTextTail->fDistanceFromPlayer < 3500.0f)
			ShowItemTextTail(itor->first);
	}
}

void CPythonTextTail::ShowCharacterTextTail(DWORD VirtualID)
{
	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(VirtualID);

	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;

	if (m_CharacterTextTailList.end() != std::find(m_CharacterTextTailList.begin(), m_CharacterTextTailList.end(), pTextTail))
	{
		return;
	}

	if (!pTextTail->pOwner->isShow())
		return;
	
	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(pTextTail->dwVirtualID);

#ifdef ENABLE_GRAPHIC_ON_OFF
	if (CPythonSystem::instance().IsNpcNameStatus() == 1)
		if (pInstance->IsNPC())
			return;
#endif

	if (!pInstance)
		return;

	if (pInstance->IsGuildWall())
		return;

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	if (pInstance->IsShop())
		return;
#endif

#ifdef ENABLE_RENEWAL_TEXT_SHADOW
	static auto& system = CPythonSystem::Instance();
	const auto isShowOutline = system.IsShowOutline();

	if (pTextTail->pTextInstance)
	{
		if (isShowOutline)
			pTextTail->pTextInstance->SetOutline(true);
		else
			pTextTail->pTextInstance->SetOutline(false);
	}

	if (pTextTail->pLevelTextInstance)
	{
		if (isShowOutline)
			pTextTail->pLevelTextInstance->SetOutline(true);
		else
			pTextTail->pLevelTextInstance->SetOutline(false);
	}

	if (pTextTail->pTitleTextInstance)
	{
		if (isShowOutline)
			pTextTail->pTitleTextInstance->SetOutline(true);
		else
			pTextTail->pTitleTextInstance->SetOutline(false);
	}

	if (pTextTail->pGuildNameTextInstance)
	{
		if (isShowOutline)
			pTextTail->pGuildNameTextInstance->SetOutline(true);
		else
			pTextTail->pGuildNameTextInstance->SetOutline(false);
	}

	if (pTextTail->pTitleSystemTextInstance)
	{
		if (isShowOutline)
			pTextTail->pTitleSystemTextInstance->SetOutline(true);
		else
			pTextTail->pTitleSystemTextInstance->SetOutline(false);
	}
#endif

	if (pInstance->CanPickInstance())
		m_CharacterTextTailList.push_back(pTextTail);
}

void CPythonTextTail::ShowItemTextTail(DWORD VirtualID)
{
	TTextTailMap::iterator itor = m_ItemTextTailMap.find(VirtualID);

	if (m_ItemTextTailMap.end() == itor)
		return;

#ifdef ENABLE_GRAPHIC_ON_OFF
	if (CPythonSystem::instance().GetDropItemLevel() >= 2)
		return;
#endif

	TTextTail* pTextTail = itor->second;

	if (m_ItemTextTailList.end() != std::find(m_ItemTextTailList.begin(), m_ItemTextTailList.end(), pTextTail))
	{
		return;
	}

#ifdef ENABLE_RENEWAL_TEXT_SHADOW
	if (pTextTail->pTextInstance)
	{
		static auto& system = CPythonSystem::Instance();

		if (system.IsShowOutline())
			pTextTail->pTextInstance->SetOutline(true);
		else
			pTextTail->pTextInstance->SetOutline(false);
	}
#endif

	m_ItemTextTailList.push_back(pTextTail);
}

bool CPythonTextTail::isIn(CPythonTextTail::TTextTail* pSource, CPythonTextTail::TTextTail* pTarget)
{
	float x1Source = pSource->x + pSource->xStart;
	float y1Source = pSource->y + pSource->yStart;
	float x2Source = pSource->x + pSource->xEnd;
	float y2Source = pSource->y + pSource->yEnd;
	float x1Target = pTarget->x + pTarget->xStart;
	float y1Target = pTarget->y + pTarget->yStart;
	float x2Target = pTarget->x + pTarget->xEnd;
	float y2Target = pTarget->y + pTarget->yEnd;

	if (x1Source <= x2Target && x2Source >= x1Target && y1Source <= y2Target && y2Source >= y1Target)
	{
		return true;
	}

	return false;
}

#ifdef ENABLE_GUILD_LEADER_TEXTAIL
void CPythonTextTail::RegisterCharacterTextTail(DWORD dwGuildID, BYTE dwGuildLeader, DWORD dwVirtualID, const D3DXCOLOR & c_rColor, float fAddHeight)
#else
void CPythonTextTail::RegisterCharacterTextTail(DWORD dwGuildID, DWORD dwVirtualID, const D3DXCOLOR & c_rColor, float fAddHeight)
#endif
{
	CInstanceBase* pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwVirtualID);

	if (!pCharacterInstance)
		return;

#ifdef ENABLE_GM_MOB_VNUM_DISPLAY
	CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();

	uint32_t raceNum{ pCharacterInstance->GetRace() };
	const char* originalName = pCharacterInstance->GetNameString();
	const char* displayName = originalName;

	char chrName[CHARACTER_NAME_MAX_LEN + 1 + 15];

	if (pInstance)
	{
		if (pInstance->IsGameMaster() && pInstance != pCharacterInstance && !pCharacterInstance->IsPC())
		{
			snprintf(chrName, sizeof(chrName), "%s - (%u)", originalName, raceNum);
			displayName = chrName;
		}
	}
#endif

#ifdef ENABLE_GM_MOB_VNUM_DISPLAY
	TTextTail* pTextTail = RegisterTextTail(dwVirtualID, displayName, pCharacterInstance->GetGraphicThingInstancePtr(), pCharacterInstance->GetGraphicThingInstanceRef().GetHeight() + fAddHeight, c_rColor);
#else
	TTextTail* pTextTail = RegisterTextTail(dwVirtualID, pCharacterInstance->GetNameString(), pCharacterInstance->GetGraphicThingInstancePtr(), pCharacterInstance->GetGraphicThingInstanceRef().GetHeight() + fAddHeight, c_rColor);
#endif

	CGraphicTextInstance* pTextInstance = pTextTail->pTextInstance;
	pTextInstance->SetOutline(true);
	pTextInstance->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);

	pTextTail->pMarkInstance = nullptr;
	pTextTail->pGuildNameTextInstance = nullptr;
	pTextTail->pTitleTextInstance = nullptr;
	pTextTail->pLevelTextInstance = nullptr;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	pTextTail->pLanguageInstance = nullptr;
	pTextTail->pLanguageInstance2 = nullptr;
#endif
#ifdef ENABLE_TITLE_SYSTEM
	pTextTail->pTitleSystemTextInstance = nullptr;
#endif
#ifdef ENABLE_SHOW_MOB_INFO
	pTextTail->pAIFlagTextInstance = nullptr;
#endif

	if (0 != dwGuildID)
	{
		pTextTail->pMarkInstance = CGraphicMarkInstance::New();

		DWORD dwMarkID = CGuildMarkManager::Instance().GetMarkID(dwGuildID);

		if (dwMarkID != CGuildMarkManager::INVALID_MARK_ID)
		{
			std::string markImagePath;

			if (CGuildMarkManager::Instance().GetMarkImageFilename(dwMarkID / CGuildMarkImage::MARK_TOTAL_COUNT, markImagePath))
			{
				pTextTail->pMarkInstance->SetImageFileName(markImagePath.c_str());
				pTextTail->pMarkInstance->Load();
				pTextTail->pMarkInstance->SetIndex(dwMarkID % CGuildMarkImage::MARK_TOTAL_COUNT);
			}
		}

		std::string strGuildName;
		if (!CPythonGuild::Instance().GetGuildName(dwGuildID, &strGuildName))
			strGuildName = "Noname";

#ifdef ENABLE_GUILD_LEADER_TEXTAIL
		if (dwGuildLeader == 3)
			 // Koyu turuncu (Dark Orange: #FF8C00) [Lider]
			strGuildName.insert(0, " |cFFFF8C00[L]|r ");
		else if (dwGuildLeader == 2)
			// Çam yeþili (Pine Green: #01796F) [General]
			strGuildName.insert(0, " |cFF01796F[G]|r ");
#endif


		CGraphicTextInstance* & prGuildNameInstance = pTextTail->pGuildNameTextInstance;
		prGuildNameInstance = CGraphicTextInstance::New();
		prGuildNameInstance->SetTextPointer(ms_pFont);
		prGuildNameInstance->SetOutline(true);
		prGuildNameInstance->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
		prGuildNameInstance->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
		prGuildNameInstance->SetValue(strGuildName.c_str());
		prGuildNameInstance->SetColor(c_TextTail_Guild_Name_Color.r, c_TextTail_Guild_Name_Color.g, c_TextTail_Guild_Name_Color.b);
		prGuildNameInstance->Update();
	}

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	CGraphicImageInstance* & prLanguage = pTextTail->pLanguageInstance;
	CGraphicImageInstance* & prLanguage2 = pTextTail->pLanguageInstance2;

	if (!prLanguage)
	{
		BYTE bLanguage = pCharacterInstance->GetLanguage();
		if(pCharacterInstance->IsPC() && bLanguage)
		{
			std::string langName = "en";

			switch (bLanguage)
			{
				case CPythonApplication::LOCALE_EN: langName = "en"; break;
				case CPythonApplication::LOCALE_RO: langName = "ro"; break;
				case CPythonApplication::LOCALE_PT: langName = "pt"; break;
				case CPythonApplication::LOCALE_ES: langName = "es"; break;
				case CPythonApplication::LOCALE_FR: langName = "fr"; break;
				case CPythonApplication::LOCALE_DE: langName = "de"; break;
				case CPythonApplication::LOCALE_PL: langName = "pl"; break;
				case CPythonApplication::LOCALE_IT: langName = "it"; break;
				case CPythonApplication::LOCALE_CZ: langName = "cz"; break;
				case CPythonApplication::LOCALE_HU: langName = "hu"; break;
				case CPythonApplication::LOCALE_TR: langName = "tr"; break;
				default: langName = "en"; break;
			}

			char szFileName[256];
			sprintf(szFileName, "d:/ymir work/ui/game/emoji/flag/%s.tga", langName.c_str());

			if (CResourceManager::Instance().IsFileExist(szFileName))
			{
				CGraphicImage* pLanguageImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(szFileName);
				if (pLanguageImage)
				{
					prLanguage = CGraphicImageInstance::New();
					prLanguage->SetImagePointer(pLanguageImage);
				}
			}
		}
	}

	if (!prLanguage2)
	{
		BYTE bLanguage2 = pCharacterInstance->GetLanguage2();
		if(pCharacterInstance->IsPC() && bLanguage2)
		{
			std::string langName2 = "en";

			switch (bLanguage2)
			{
				case CPythonApplication::LOCALE_EN: langName2 = "en"; break;
				case CPythonApplication::LOCALE_RO: langName2 = "ro"; break;
				case CPythonApplication::LOCALE_PT: langName2 = "pt"; break;
				case CPythonApplication::LOCALE_ES: langName2 = "es"; break;
				case CPythonApplication::LOCALE_FR: langName2 = "fr"; break;
				case CPythonApplication::LOCALE_DE: langName2 = "de"; break;
				case CPythonApplication::LOCALE_PL: langName2 = "pl"; break;
				case CPythonApplication::LOCALE_IT: langName2 = "it"; break;
				case CPythonApplication::LOCALE_CZ: langName2 = "cz"; break;
				case CPythonApplication::LOCALE_HU: langName2 = "hu"; break;
				case CPythonApplication::LOCALE_TR: langName2 = "tr"; break;
				default: langName2 = "en"; break;
			}

			char szFileName[256];
			sprintf(szFileName, "d:/ymir work/ui/game/emoji/flag/%s.tga", langName2.c_str());

			if (CResourceManager::Instance().IsFileExist(szFileName))
			{
				CGraphicImage* pLanguageImage2 = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(szFileName);
				if (pLanguageImage2)
				{
					prLanguage2 = CGraphicImageInstance::New();
					prLanguage2->SetImagePointer(pLanguageImage2);
				}
			}
		}
	}
#endif

#ifdef ENABLE_SHOW_MOB_INFO
	if (IS_SET(pCharacterInstance->GetAIFlag(), CInstanceBase::AIFLAG_AGGRESSIVE))
	{
		CGraphicTextInstance* & prAIFlagInstance = pTextTail->pAIFlagTextInstance;
		prAIFlagInstance = CGraphicTextInstance::New();
		prAIFlagInstance->SetTextPointer(ms_pFont);
		prAIFlagInstance->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);
		prAIFlagInstance->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
		prAIFlagInstance->SetValue("*");
		prAIFlagInstance->SetOutline(true);
		prAIFlagInstance->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
		prAIFlagInstance->Update();

	}
	pTextTail->bIsPC = pCharacterInstance->IsPC() != FALSE;
#endif

	m_CharacterTextTailMap.insert(TTextTailMap::value_type(dwVirtualID, pTextTail));
}

void CPythonTextTail::RegisterItemTextTail(DWORD VirtualID, const char *c_szText, CGraphicObjectInstance* pOwner)
{
#ifdef __DEBUG
	char szName[256];
	spritnf(szName, "%s[%d]", c_szText, VirtualID);

	TTextTail* pTextTail = RegisterTextTail(VirtualID, c_szText, pOwner, c_TextTail_Name_Position, c_TextTail_Item_Color);
	m_ItemTextTailMap.insert(TTextTailMap::value_type(VirtualID, pTextTail));
#else
	TTextTail* pTextTail = RegisterTextTail(VirtualID, c_szText, pOwner, c_TextTail_Name_Position, c_TextTail_Item_Color);
	m_ItemTextTailMap.insert(TTextTailMap::value_type(VirtualID, pTextTail));
#endif
}

void CPythonTextTail::RegisterChatTail(DWORD VirtualID, const char *c_szChat)
{
	CInstanceBase* pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(VirtualID);

	if (!pCharacterInstance)
		return;

	TChatTailMap::iterator itor = m_ChatTailMap.find(VirtualID);

	if (m_ChatTailMap.end() != itor)
	{
		TTextTail* pTextTail = itor->second;

#ifdef ENABLE_EMOTICONS_SYSTEM
		DeleteTextTail (pTextTail);
		m_ChatTailMap.erase (itor);
#else
		pTextTail->pTextInstance->SetValue(c_szChat);
		pTextTail->pTextInstance->Update();
		pTextTail->Color = c_TextTail_Chat_Color;
		pTextTail->pTextInstance->SetColor(c_TextTail_Chat_Color);
		pTextTail->LivingTime = CTimer::Instance().GetCurrentMillisecond() + TextTail_GetLivingTime();
		pTextTail->bNameFlag = TRUE;
		return;
#endif
	}

	TTextTail* pTextTail = RegisterTextTail(VirtualID,
											 c_szChat,
											 pCharacterInstance->GetGraphicThingInstancePtr(),
											 pCharacterInstance->GetGraphicThingInstanceRef().GetHeight() + pCharacterInstance->GetBaseHeight() + 10.0f,
											 c_TextTail_Chat_Color
#ifdef ENABLE_EMOTICONS_SYSTEM
											 , "Tahoma:12.fnt"
#endif
											 );

	pTextTail->LivingTime = CTimer::Instance().GetCurrentMillisecond() + TextTail_GetLivingTime();

	pTextTail->bNameFlag = TRUE;
	pTextTail->pTextInstance->SetOutline(true);
	pTextTail->pTextInstance->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	m_ChatTailMap.insert(TTextTailMap::value_type(VirtualID, pTextTail));
}

void CPythonTextTail::RegisterInfoTail(DWORD VirtualID, const char *c_szChat)
{
	CInstanceBase* pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(VirtualID);

	if (!pCharacterInstance)
		return;

	TChatTailMap::iterator itor = m_ChatTailMap.find(VirtualID);

	if (m_ChatTailMap.end() != itor)
	{
		TTextTail* pTextTail = itor->second;

#ifdef ENABLE_EMOTICONS_SYSTEM
		DeleteTextTail (pTextTail);
		m_ChatTailMap.erase (itor);
#else
		pTextTail->pTextInstance->SetValue(c_szChat);
		pTextTail->pTextInstance->Update();
		pTextTail->Color = c_TextTail_Info_Color;
		pTextTail->pTextInstance->SetColor(c_TextTail_Info_Color);
		pTextTail->LivingTime = CTimer::Instance().GetCurrentMillisecond() + TextTail_GetLivingTime();
		pTextTail->bNameFlag = FALSE;
		return;
#endif
	}

	TTextTail* pTextTail = RegisterTextTail(VirtualID,
											 c_szChat,
											 pCharacterInstance->GetGraphicThingInstancePtr(),
											 pCharacterInstance->GetGraphicThingInstanceRef().GetHeight() + 10.0f,
											 c_TextTail_Info_Color
#ifdef ENABLE_EMOTICONS_SYSTEM
											 , "Tahoma:12.fnt"
#endif
											 );

	pTextTail->LivingTime = CTimer::Instance().GetCurrentMillisecond() + TextTail_GetLivingTime();

	pTextTail->bNameFlag = FALSE;
	pTextTail->pTextInstance->SetOutline(true);
	pTextTail->pTextInstance->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	m_ChatTailMap.insert(TTextTailMap::value_type(VirtualID, pTextTail));
}

bool CPythonTextTail::GetTextTailPosition(DWORD dwVID, float *px, float *py, float *pz)
{
	TTextTailMap::iterator itorCharacter = m_CharacterTextTailMap.find(dwVID);

	if (m_CharacterTextTailMap.end() == itorCharacter)
	{
		return false;
	}

	TTextTail* pTextTail = itorCharacter->second;
	*px=pTextTail->x;
	*py=pTextTail->y;
	*pz=pTextTail->z;

	return true;
}

bool CPythonTextTail::IsChatTextTail(DWORD dwVID)
{
	TChatTailMap::iterator itorChat = m_ChatTailMap.find(dwVID);

	if (m_ChatTailMap.end() == itorChat)
		return false;

	return true;
}

void CPythonTextTail::SetCharacterTextTailColor(DWORD VirtualID, const D3DXCOLOR & c_rColor)
{
	TTextTailMap::iterator itorCharacter = m_CharacterTextTailMap.find(VirtualID);

	if (m_CharacterTextTailMap.end() == itorCharacter)
		return;

	TTextTail* pTextTail = itorCharacter->second;
	pTextTail->pTextInstance->SetColor(c_rColor);
	pTextTail->Color = c_rColor;
}

void CPythonTextTail::SetItemTextTailOwner(DWORD dwVID, const char *c_szName)
{
	TTextTailMap::iterator itor = m_ItemTextTailMap.find(dwVID);
	if (m_ItemTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;

	if (strlen(c_szName) > 0)
	{
		if (!pTextTail->pOwnerTextInstance)
		{
			pTextTail->pOwnerTextInstance = CGraphicTextInstance::New();
		}

		std::string strName = c_szName;
		pTextTail->pOwnerTextInstance->SetTextPointer(ms_pFont);
		pTextTail->pOwnerTextInstance->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
		pTextTail->pOwnerTextInstance->SetValue(strName.c_str());
		pTextTail->pOwnerTextInstance->SetColor(1.0f, 1.0f, 0.0f);
		pTextTail->pOwnerTextInstance->Update();

		int xOwnerSize, yOwnerSize;
		pTextTail->pOwnerTextInstance->GetTextSize(&xOwnerSize, &yOwnerSize);
		pTextTail->yStart	= -2.0f;
		pTextTail->yEnd		+= float(yOwnerSize + 4);
		pTextTail->xStart	= fMIN(pTextTail->xStart, float(-xOwnerSize / 2 - 1));
		pTextTail->xEnd		= fMAX(pTextTail->xEnd, float(xOwnerSize / 2 + 1));
	}
	else
	{
		if (pTextTail->pOwnerTextInstance)
		{
			CGraphicTextInstance::Delete(pTextTail->pOwnerTextInstance);
			pTextTail->pOwnerTextInstance = nullptr;
		}

		int xSize, ySize;
		pTextTail->pTextInstance->GetTextSize(&xSize, &ySize);
		pTextTail->xStart	= (float) (-xSize / 2 - 2);
		pTextTail->yStart	= -2.0f;
		pTextTail->xEnd		= (float) (xSize / 2 + 2);
		pTextTail->yEnd		= (float) ySize;
	}
}

void CPythonTextTail::DeleteCharacterTextTail(DWORD VirtualID)
{
	TTextTailMap::iterator itorCharacter = m_CharacterTextTailMap.find(VirtualID);
	TTextTailMap::iterator itorChat = m_ChatTailMap.find(VirtualID);

	if (m_CharacterTextTailMap.end() != itorCharacter)
	{
		DeleteTextTail(itorCharacter->second);
		m_CharacterTextTailMap.erase(itorCharacter);
	}
	else
	{
		Tracenf("CPythonTextTail::DeleteCharacterTextTail - Find VID[%d] Error", VirtualID);
	}

	if (m_ChatTailMap.end() != itorChat)
	{
		DeleteTextTail(itorChat->second);
		m_ChatTailMap.erase(itorChat);
	}
}

void CPythonTextTail::DeleteItemTextTail(DWORD VirtualID)
{
	TTextTailMap::iterator itor = m_ItemTextTailMap.find(VirtualID);

	if (m_ItemTextTailMap.end() == itor)
	{
		Tracef(" CPythonTextTail::DeleteItemTextTail - None Item Text Tail\n");
		return;
	}

	DeleteTextTail(itor->second);
	m_ItemTextTailMap.erase(itor);
}

CPythonTextTail::TTextTail* CPythonTextTail::RegisterTextTail(DWORD dwVirtualID, const char *c_szText, CGraphicObjectInstance* pOwner, float fHeight, const D3DXCOLOR & c_rColor
#ifdef ENABLE_EMOTICONS_SYSTEM
	, std::string szFont
#endif
)
{
	TTextTail* pTextTail = m_TextTailPool.Alloc();

	pTextTail->dwVirtualID = dwVirtualID;
	pTextTail->pOwner = pOwner;
	pTextTail->pTextInstance = CGraphicTextInstance::New();
	pTextTail->pOwnerTextInstance = nullptr;
	pTextTail->fHeight = fHeight;

#ifdef ENABLE_EMOTICONS_SYSTEM
	if (szFont.c_str() != "")
	{
		CResourceManager& rkResMgr = CResourceManager::Instance();
		CResource* pkRes = rkResMgr.GetTypeResourcePointer(szFont.c_str());
		CGraphicText* pkResFont = static_cast<CGraphicText*>(pkRes);
		pTextTail->pTextInstance->SetTextPointer(pkResFont);
	}
	else
	{
		pTextTail->pTextInstance->SetTextPointer(ms_pFont);
	}
#else
	pTextTail->pTextInstance->SetTextPointer(ms_pFont);
#endif

	pTextTail->pTextInstance->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
	pTextTail->pTextInstance->SetValue(c_szText);
	pTextTail->pTextInstance->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	pTextTail->pTextInstance->Update();

	int xSize, ySize;
	pTextTail->pTextInstance->GetTextSize(&xSize, &ySize);
	pTextTail->xStart = (float) (-xSize / 2 - 2);
	pTextTail->yStart = -2.0f;
	pTextTail->xEnd = (float) (xSize / 2 + 2);
	pTextTail->yEnd = (float) ySize;
	pTextTail->Color = c_rColor;
	pTextTail->fDistanceFromPlayer = 0.0f;
	pTextTail->x = -100.0f;
	pTextTail->y = -100.0f;
	pTextTail->z = 0.0f;
	pTextTail->pMarkInstance = nullptr;
	pTextTail->pGuildNameTextInstance = nullptr;
	pTextTail->pTitleTextInstance = nullptr;
	pTextTail->pLevelTextInstance = nullptr;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	pTextTail->pLanguageInstance = nullptr;
	pTextTail->pLanguageInstance2 = nullptr;
#endif
#ifdef ENABLE_TITLE_SYSTEM
	pTextTail->pTitleSystemTextInstance = nullptr;
#endif
#ifdef ENABLE_SHOW_MOB_INFO
	pTextTail->pAIFlagTextInstance = nullptr;
#endif
	return pTextTail;
}

void CPythonTextTail::DeleteTextTail(TTextTail* pTextTail)
{
	if (pTextTail->pTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pTextInstance);
		pTextTail->pTextInstance = nullptr;
	}
	if (pTextTail->pOwnerTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pOwnerTextInstance);
		pTextTail->pOwnerTextInstance = nullptr;
	}
	if (pTextTail->pMarkInstance)
	{
		CGraphicMarkInstance::Delete(pTextTail->pMarkInstance);
		pTextTail->pMarkInstance = nullptr;
	}
	if (pTextTail->pGuildNameTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pGuildNameTextInstance);
		pTextTail->pGuildNameTextInstance = nullptr;
	}
	if (pTextTail->pTitleTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pTitleTextInstance);
		pTextTail->pTitleTextInstance = nullptr;
	}
	if (pTextTail->pLevelTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pLevelTextInstance);
		pTextTail->pLevelTextInstance = nullptr;
	}
#ifdef ENABLE_TITLE_SYSTEM
	if (pTextTail->pTitleSystemTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pTitleSystemTextInstance);
		pTextTail->pTitleSystemTextInstance = nullptr;
	}
#endif
#ifdef ENABLE_SHOW_MOB_INFO
	if (pTextTail->pAIFlagTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pAIFlagTextInstance);
		pTextTail->pAIFlagTextInstance = nullptr;
	}
#endif

	m_TextTailPool.Free(pTextTail);
}

int CPythonTextTail::Pick(int ixMouse, int iyMouse)
{
	for (TTextTailMap::iterator itor = m_ItemTextTailMap.begin(); itor != m_ItemTextTailMap.end(); ++itor)
	{
		TTextTail* pTextTail = itor->second;

		if (ixMouse >= pTextTail->x + pTextTail->xStart && ixMouse <= pTextTail->x + pTextTail->xEnd &&
			iyMouse >= pTextTail->y + pTextTail->yStart && iyMouse <= pTextTail->y + pTextTail->yEnd)
		{
			SelectItemName(itor->first);
			return (itor->first);
		}
	}

	return -1;
}

void CPythonTextTail::SelectItemName(DWORD dwVirtualID)
{
	TTextTailMap::iterator itor = m_ItemTextTailMap.find(dwVirtualID);

	if (m_ItemTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;
	pTextTail->pTextInstance->SetColor(0.1f, 0.9f, 0.1f);
}

#ifdef ENABLE_TITLE_SYSTEM
void CPythonTextTail::AttachTitleSystem(DWORD dwVID, const char *c_szName, const D3DXCOLOR & c_rColor)
{
	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;
	CGraphicTextInstance* & prTitleSystem = pTextTail->pTitleSystemTextInstance;
	if (!prTitleSystem)
	{
		prTitleSystem = CGraphicTextInstance::New();
		prTitleSystem->SetTextPointer(ms_pFont);
		prTitleSystem->SetOutline(true);
		prTitleSystem->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
		prTitleSystem->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	}

	prTitleSystem->SetValue(c_szName);
	prTitleSystem->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	prTitleSystem->Update();
}

void CPythonTextTail::DetachTitleSystem(DWORD dwVID)
{
	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;

	if (pTextTail->pTitleSystemTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pTitleSystemTextInstance);
		pTextTail->pTitleSystemTextInstance = nullptr;
	}
}
#endif

void CPythonTextTail::AttachTitle(DWORD dwVID, const char *c_szName, const D3DXCOLOR & c_rColor)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;

	CGraphicTextInstance* & prTitle = pTextTail->pTitleTextInstance;
	if (!prTitle)
	{
		prTitle = CGraphicTextInstance::New();
		prTitle->SetTextPointer(ms_pFont);
		prTitle->SetOutline(true);
		prTitle->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_RIGHT);
		prTitle->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	}

	prTitle->SetValue(c_szName);
	prTitle->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	prTitle->Update();
}

void CPythonTextTail::DetachTitle(DWORD dwVID)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;

	if (pTextTail->pTitleTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pTitleTextInstance);
		pTextTail->pTitleTextInstance = nullptr;
	}
}

void CPythonTextTail::EnablePKTitle(BOOL bFlag)
{
	bPKTitleEnable = bFlag;
}

void CPythonTextTail::AttachLevel(DWORD dwVID, const char *c_szText, const D3DXCOLOR & c_rColor)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;

	CGraphicTextInstance* & prLevel = pTextTail->pLevelTextInstance;
	if (!prLevel)
	{
		prLevel = CGraphicTextInstance::New();
		prLevel->SetTextPointer(ms_pFont);
		prLevel->SetOutline(true);

		prLevel->SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_RIGHT);
		prLevel->SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	}

	prLevel->SetValue(c_szText);
	prLevel->SetColor(c_rColor.r, c_rColor.g, c_rColor.b);
	prLevel->Update();
}

void CPythonTextTail::DetachLevel(DWORD dwVID)
{
	if (!bPKTitleEnable)
		return;

	TTextTailMap::iterator itor = m_CharacterTextTailMap.find(dwVID);
	if (m_CharacterTextTailMap.end() == itor)
		return;

	TTextTail* pTextTail = itor->second;

	if (pTextTail->pLevelTextInstance)
	{
		CGraphicTextInstance::Delete(pTextTail->pLevelTextInstance);
		pTextTail->pLevelTextInstance = nullptr;
	}
}


void CPythonTextTail::Initialize()
{
	CGraphicText* pkDefaultFont = static_cast<CGraphicText*>(DefaultFont_GetResource());
	if (!pkDefaultFont)
	{
		TraceError("CPythonTextTail::Initialize - CANNOT_FIND_DEFAULT_FONT");
		return;
	}

	ms_pFont = pkDefaultFont;
}

void CPythonTextTail::Destroy()
{
	m_TextTailPool.Clear();
}

void CPythonTextTail::Clear()
{
	m_CharacterTextTailMap.clear();
	m_CharacterTextTailList.clear();
	m_ItemTextTailMap.clear();
	m_ItemTextTailList.clear();
	m_ChatTailMap.clear();

	m_TextTailPool.Clear();
}

CPythonTextTail::CPythonTextTail()
{
	Clear();
}

CPythonTextTail::~CPythonTextTail()
{
	Destroy();
}
