#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "PythonNonPlayer.h"
#include "AbstractApplication.h"
#include "AbstractPlayer.h"
#include "AbstractCharacterManager.h"
#include "AbstractChat.h"
#include "InstanceBase.h"

#define ishan(ch)		(((ch) & 0xE0) > 0x90)
#define ishanasc(ch)	(isascii(ch) || ishan(ch))
#define ishanalp(ch)	(isalpha(ch) || ishan(ch))
#define isnhdigit(ch)	(!ishan(ch) && isdigit(ch))
#define isnhspace(ch)	(!ishan(ch) && isspace(ch))

#define LOWER(c)		(((c) >= 'A' && (c) <= 'Z') ? ((c) + ('a' - 'A')) : (c))
#define UPPER(c)		(((c) >= 'a' && (c) <= 'z') ? ((c) + ('A' - 'a')) : (c))

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	#include "PythonShop.h"
#endif

#ifdef __AUTO_HUNT__
	#include "PythonPlayer.h"
#endif

void SkipSpaces(char **string)
{
	for (; **string != '\0' && isnhspace((unsigned char) **string); ++(*string));
}

char *OneArgument(char *argument, char *first_arg)
{
	char mark = FALSE;

	if (!argument)
	{
		*first_arg = '\0';
		return NULL;
	}

	SkipSpaces(&argument);

	while (*argument)
	{
		if (*argument == '\"')
		{
			mark = !mark;
			++argument;
			continue;
		}

		if (!mark && isnhspace((unsigned char) *argument))
			break;

		*(first_arg++) = LOWER(*argument);
		++argument;
	}

	*first_arg = '\0';

	SkipSpaces(&argument);
	return (argument);
}

void AppendMonsterList(const CPythonNonPlayer::TMobTableList & c_rMobTableList, const char * c_szHeader, int iType)
{
	DWORD dwMonsterCount = 0;
	std::string strMonsterList = c_szHeader;

	CPythonNonPlayer::TMobTableList::const_iterator itor = c_rMobTableList.begin();
	for (; itor!=c_rMobTableList.end(); ++itor)
	{
		const CPythonNonPlayer::TMobTable * c_pMobTable = *itor;
		if (iType == c_pMobTable->bRank)
		{
			if (dwMonsterCount != 0)
				strMonsterList += ", ";
			strMonsterList += c_pMobTable->szLocaleName;
			if (++dwMonsterCount > 5)
				break;
		}
	}
	if (dwMonsterCount > 0)
	{
		IAbstractChat& rkChat=IAbstractChat::GetSingleton();
		rkChat.AppendChat(CHAT_TYPE_INFO, strMonsterList.c_str());
	}
}

bool CPythonNetworkStream::ClientCommand(const char * c_szCommand)
{
	return false;
}

bool SplitToken(const char * c_szLine, CTokenVector * pstTokenVector, const char * c_szDelimeter = " ")
{
	pstTokenVector->reserve(10);
	pstTokenVector->clear();

	std::string stToken;
	std::string strLine = c_szLine;

	DWORD basePos = 0;

	do
	{
		int beginPos = strLine.find_first_not_of(c_szDelimeter, basePos);
		if (beginPos < 0)
			return false;

		int endPos;

		if (strLine[beginPos] == '"')
		{
			++beginPos;
			endPos = strLine.find_first_of("\"", beginPos);

			if (endPos < 0)
				return false;

			basePos = endPos + 1;
		}
		else
		{
			endPos = strLine.find_first_of(c_szDelimeter, beginPos);
			basePos = endPos;
		}

		pstTokenVector->push_back(strLine.substr(beginPos, endPos - beginPos));

		if (int(strLine.find_first_not_of(c_szDelimeter, basePos)) < 0)
			break;
	} while (basePos < strLine.length());

	return true;
}

void CPythonNetworkStream::ServerCommand(char * c_szCommand)
{
	if (strcmpi(c_szCommand, "ConsoleEnable") == 0)
		return;

	if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
	{
		bool isTrue;
		if (PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_ServerCommand_Run",
			Py_BuildValue("(s)", c_szCommand),
			&isTrue
		))
		{
			if (isTrue)
				return;
		}
	}
	else if (m_poSerCommandParserWnd)
	{
		bool isTrue;
		if (PyCallClassMemberFunc(m_poSerCommandParserWnd,
			"BINARY_ServerCommand_Run",
			Py_BuildValue("(s)", c_szCommand),
			&isTrue
		))
		{
			if (isTrue)
				return;
		}
	}

	CTokenVector TokenVector;
	if (!SplitToken(c_szCommand, &TokenVector))
		return;
	if (TokenVector.empty())
		return;

	const char * szCmd = TokenVector[0].c_str();

	if (!strcmpi(szCmd, "quit"))
	{
		PostQuitMessage(0);
	}
	else if (!strcmpi(szCmd, "BettingMoney"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}
	}
	else if (!strcmpi(szCmd, "gift"))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "Gift_Show", Py_BuildValue("()")); 	
	}

	// CUBE
#ifdef __AUTO_HUNT__
	else if (!strcmpi(szCmd, "auto_hunt"))
	{
		CPythonPlayer::Instance().AutoHuntSettings(c_szCommand);
	}
#endif

	else if (!strcmpi(szCmd, "cube"))
	{
		if (TokenVector.size() < 2)
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		if ("open" == TokenVector[1])
		{
			if (3 > TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}

			DWORD npcVNUM = (DWORD)atoi(TokenVector[2].c_str());
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Open", Py_BuildValue("(i)", npcVNUM));
		}
		else if ("close" == TokenVector[1])
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Close", Py_BuildValue("()"));
		}
		else if ("info" == TokenVector[1])
		{
			if (5 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}

			UINT gold = atoi(TokenVector[2].c_str());
			UINT itemVnum = atoi(TokenVector[3].c_str());
			UINT count = atoi(TokenVector[4].c_str());
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_UpdateInfo", Py_BuildValue("(iii)", gold, itemVnum, count));
		}
		else if ("success" == TokenVector[1])
		{
			if (4 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}

			UINT itemVnum = atoi(TokenVector[2].c_str());
			UINT count = atoi(TokenVector[3].c_str());
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Succeed", Py_BuildValue("(ii)", itemVnum, count));
		}
		else if ("fail" == TokenVector[1])
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Failed", Py_BuildValue("()"));
		}
		else if ("r_list" == TokenVector[1])
		{
			// result list (/cube r_list npcVNUM resultCount resultText)
			if (5 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %d", c_szCommand, 5);
				return;
			}

			DWORD npcVNUM = (DWORD)atoi(TokenVector[2].c_str());

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_ResultList", Py_BuildValue("(is)", npcVNUM, TokenVector[4].c_str()));
		}
		else if ("m_info" == TokenVector[1])
		{
			// material list (/cube m_info requestStartIndex resultCount MaterialText)

			if (5 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %d", c_szCommand, 5);
				return;
			}

			UINT requestStartIndex = (UINT)atoi(TokenVector[2].c_str());
			UINT resultCount = (UINT)atoi(TokenVector[3].c_str());

			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_MaterialInfo", Py_BuildValue("(iis)", requestStartIndex, resultCount, TokenVector[4].c_str()));
		}
	}
	// CUEBE_END
	else if (!strcmpi(szCmd, "ObserverCount"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		UINT uObserverCount= atoi(TokenVector[1].c_str());

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_BettingGuildWar_UpdateObserverCount",
			Py_BuildValue("(i)", uObserverCount)
		);
	}
	else if (!strcmpi(szCmd, "ObserverMode"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		UINT uMode= atoi(TokenVector[1].c_str());

		IAbstractPlayer & rkPlayer = IAbstractPlayer::GetSingleton();
		rkPlayer.SetObserverMode(uMode ? true : false);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_BettingGuildWar_SetObserverMode",
			Py_BuildValue("(i)", uMode)
		);
	}
#ifdef ENABLE_RENEWAL_OFFLINESHOP
	else if (!strcmpi(szCmd, "OfflineShopSetFlag"))
	{
		unsigned long long flag = atoi(TokenVector[1].c_str());
		CPythonShop::Instance().SetShopFlag(flag);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshOfflineShop", Py_BuildValue("()"));
	}
#endif
#ifdef ENABLE_ITEMSHOP
	else if (!strcmpi(szCmd, "open_ishop"))
	{
		if (m_apoPhaseWnd[PHASE_WINDOW_GAME])
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenItemShop", Py_BuildValue("()"));
		}
		else
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Game phase window missing for open_ishop", c_szCommand);
		}
	}
#endif
	else if (!strcmpi(szCmd, "ObserverTeamInfo"))
	{
	}
#ifdef ENABLE_MINI_GAME_OKEY
	else if (!strcmpi(szCmd, "cards"))
	{
		if (TokenVector.size() < 2)
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}
		if ("open" == TokenVector[1])
		{
			DWORD safemode = atoi(TokenVector[2].c_str());
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cards_Open", Py_BuildValue("(i)", safemode));
		}
		else if ("info" == TokenVector[1])
		{
			if (14 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}
			DWORD card_in_hand_1 = atoi(TokenVector[2].c_str());
			DWORD card_in_hand_1_v = atoi(TokenVector[3].c_str());
			DWORD card_in_hand_2 = atoi(TokenVector[4].c_str());
			DWORD card_in_hand_2_v = atoi(TokenVector[5].c_str());
			DWORD card_in_hand_3 = atoi(TokenVector[6].c_str());
			DWORD card_in_hand_3_v = atoi(TokenVector[7].c_str());
			DWORD card_in_hand_4 = atoi(TokenVector[8].c_str());
			DWORD card_in_hand_4_v = atoi(TokenVector[9].c_str());
			DWORD card_in_hand_5 = atoi(TokenVector[10].c_str());
			DWORD card_in_hand_5_v = atoi(TokenVector[11].c_str());
			DWORD cards_left = atoi(TokenVector[12].c_str());
			DWORD points = atoi(TokenVector[13].c_str());
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cards_UpdateInfo", Py_BuildValue("(iiiiiiiiiiii)", card_in_hand_1, card_in_hand_1_v, card_in_hand_2, card_in_hand_2_v,
																								card_in_hand_3, card_in_hand_3_v, card_in_hand_4, card_in_hand_4_v, card_in_hand_5, card_in_hand_5_v,
																								cards_left, points));
		}
		else if ("finfo" == TokenVector[1])
		{
			if (9 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}
			DWORD hand_1 = atoi(TokenVector[2].c_str());
			DWORD hand_1_v = atoi(TokenVector[3].c_str());
			DWORD hand_2 = atoi(TokenVector[4].c_str());
			DWORD hand_2_v = atoi(TokenVector[5].c_str());
			DWORD hand_3 = atoi(TokenVector[6].c_str());
			DWORD hand_3_v = atoi(TokenVector[7].c_str());
			DWORD points = atoi(TokenVector[8].c_str());
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cards_FieldUpdateInfo", Py_BuildValue("(iiiiiii)", hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points));
		}
		else if ("reward" == TokenVector[1])
		{
			if (9 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}
			DWORD hand_1 = atoi(TokenVector[2].c_str());
			DWORD hand_1_v = atoi(TokenVector[3].c_str());
			DWORD hand_2 = atoi(TokenVector[4].c_str());
			DWORD hand_2_v = atoi(TokenVector[5].c_str());
			DWORD hand_3 = atoi(TokenVector[6].c_str());
			DWORD hand_3_v = atoi(TokenVector[7].c_str());
			DWORD points = atoi(TokenVector[8].c_str());
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cards_PutReward", Py_BuildValue("(iiiiiii)", hand_1, hand_1_v, hand_2, hand_2_v, hand_3, hand_3_v, points));
		}
		else if ("icon" == TokenVector[1])
		{
			if (2 != TokenVector.size())
			{
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
				return;
			}
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cards_ShowIcon", Py_BuildValue("()"));
		}
	}
#endif
#ifdef ENABLE_RENEWAL_TELEPORT_SYSTEM
	else if (!strcmpi(szCmd, "BINARY_OpenWarpWindow"))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenWarpWindow", Py_BuildValue("()"));
	}
#endif
#ifdef ENABLE_ZODIAC_MISSION
	else if (!strcmpi(szCmd, "ZodiacDayorNight"))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ZodiacDayorNight", Py_BuildValue("()"));
	}
	else if (!strcmpi(szCmd, "ZodiacJumpButtonClose"))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ZodiacJumpButtonClose", Py_BuildValue("()"));
	}
	else if (!strcmpi(szCmd, "ZodiacJumpButtonShow"))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ZodiacJumpButtonShow", Py_BuildValue("()"));
	}
	else if (!strcmpi(szCmd, "ZodiacInfo"))
	{
		BYTE time = atoi(TokenVector[1].c_str());
		BYTE floor = atoi(TokenVector[2].c_str());
		BYTE nextfloor = atoi(TokenVector[3].c_str());
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "Refresh12ziTimer", Py_BuildValue("(iii)",time,floor,nextfloor));
	}
	else if (!strcmpi(szCmd, "OpenUI12zi"))
	{
		DWORD yellowmark = atoi(TokenVector[1].c_str());
		DWORD greenmark = atoi(TokenVector[2].c_str());
		DWORD yellowreward = atoi(TokenVector[3].c_str());
		DWORD greenreward = atoi(TokenVector[4].c_str());
		//DWORD goldreward = atoi(TokenVector[5].c_str());
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenUI12zi", Py_BuildValue("(iiii)", yellowmark, greenmark, yellowreward, greenreward));
	}
	else if (!strcmpi(szCmd, "OpenReviveDialog"))
	{
		int count = atoi(TokenVector[1].c_str());
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME] , "OpenReviveDialog", Py_BuildValue("(i)", count));
	}

	else if (!strcmpi(szCmd, "OpenReviveDialog_me"))
	{
		int count = atoi(TokenVector[1].c_str());
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenReviveDialog_me", Py_BuildValue("(i)", count));
	}
	else if (!strcmpi(szCmd, "SetBeadCount"))
	{
		int count = atoi(TokenVector[1].c_str());
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetBeadCount", Py_BuildValue("(i)", count));
	}
	else if (!strcmpi(szCmd, "NextBeadUpdateTime"))
	{
		int value = atoi(TokenVector[1].c_str());
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "NextBeadUpdateTime", Py_BuildValue("(i)", value));
	}
	else if (!strcmpi(szCmd, "GetMissionInformation"))
	{
		DWORD floor = atoi(TokenVector[1].c_str());
		DWORD time = atoi(TokenVector[2].c_str());
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "GetMissionInformation", Py_BuildValue("(ii)", floor,time));
	}
#endif
	else if (!strcmpi(szCmd, "StoneDetect"))
	{
		if (4 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		// vid distance(1-3) angle(0-360)
		DWORD dwVID = atoi(TokenVector[1].c_str());
		BYTE byDistance = atoi(TokenVector[2].c_str());
		float fAngle = atof(TokenVector[3].c_str());
		fAngle = fmod(540.0f - fAngle, 360.0f);
		Tracef("StoneDetect [VID:%d] [Distance:%d] [Angle:%d->%f]\n", dwVID, byDistance, atoi(TokenVector[3].c_str()), fAngle);

		IAbstractCharacterManager& rkChrMgr=IAbstractCharacterManager::GetSingleton();

		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(dwVID);
		if (!pInstance)
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Not Exist Instance", c_szCommand);
			return;
		}

		TPixelPosition PixelPosition;
		D3DXVECTOR3 v3Rotation(0.0f, 0.0f, fAngle);
		pInstance->NEW_GetPixelPosition(&PixelPosition);

		PixelPosition.y *= -1.0f;

		switch (byDistance)
		{
			case 0:
				CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/firecracker/find_out.mse");
				CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/firecracker/find_out.mse", PixelPosition, v3Rotation);
				break;
			case 1:
				CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_small.mse");
				CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_small.mse", PixelPosition, v3Rotation);
				break;
			case 2:
				CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_middle.mse");
				CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_middle.mse", PixelPosition, v3Rotation);
				break;
			case 3:
				CEffectManager::Instance().RegisterEffect("d:/ymir work/effect/etc/compass/appear_large.mse");
				CEffectManager::Instance().CreateEffect("d:/ymir work/effect/etc/compass/appear_large.mse", PixelPosition, v3Rotation);
				break;
			default:
				TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Distance", c_szCommand);
				break;
		}

#ifdef _DEBUG
		IAbstractChat& rkChat=IAbstractChat::GetSingleton();
		rkChat.AppendChat(CHAT_TYPE_INFO, c_szCommand);
#endif
	}
	else if (!strcmpi(szCmd, "StartStaminaConsume"))
	{
		if (3 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		DWORD dwConsumePerSec = atoi(TokenVector[1].c_str());
		DWORD dwCurrentStamina = atoi(TokenVector[2].c_str());

		IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();
		rPlayer.StartStaminaConsume(dwConsumePerSec, dwCurrentStamina);
	}

	else if (!strcmpi(szCmd, "StopStaminaConsume"))
	{
		if (2 != TokenVector.size())
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %d", c_szCommand, TokenVector.size());
			return;
		}

		DWORD dwCurrentStamina = atoi(TokenVector[1].c_str());

		IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();
		rPlayer.StopStaminaConsume(dwCurrentStamina);
	}
	else if (!strcmpi(szCmd, "messenger_auth"))
	{
		const std::string & c_rstrName = TokenVector[1].c_str();
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnMessengerAddFriendQuestion", Py_BuildValue("(s)", c_rstrName.c_str()));
	}
	else if (!strcmpi(szCmd, "combo"))
	{
		int iFlag = atoi(TokenVector[1].c_str());
		IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();
		rPlayer.SetComboSkillFlag(iFlag);
		m_bComboSkillFlag = iFlag ? true : false;
	}
	else if (!strcmpi(szCmd, "setblockmode"))
	{
		int iFlag = atoi(TokenVector[1].c_str());
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnBlockMode", Py_BuildValue("(i)", iFlag));
	}
	// Emotion Start
	else if (!strcmpi(szCmd, "french_kiss"))
	{
		int iVID1 = atoi(TokenVector[1].c_str());
		int iVID2 = atoi(TokenVector[2].c_str());

		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance1 = rkChrMgr.GetInstancePtr(iVID1);
		CInstanceBase * pInstance2 = rkChrMgr.GetInstancePtr(iVID2);
		if (pInstance1 && pInstance2)
			pInstance1->ActDualEmotion(*pInstance2, CRaceMotionData::NAME_FRENCH_KISS_START, CRaceMotionData::NAME_FRENCH_KISS_START);
	}
	else if (!strcmpi(szCmd, "kiss"))
	{
		int iVID1 = atoi(TokenVector[1].c_str());
		int iVID2 = atoi(TokenVector[2].c_str());

		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance1 = rkChrMgr.GetInstancePtr(iVID1);
		CInstanceBase * pInstance2 = rkChrMgr.GetInstancePtr(iVID2);
		if (pInstance1 && pInstance2)
			pInstance1->ActDualEmotion(*pInstance2, CRaceMotionData::NAME_KISS_START, CRaceMotionData::NAME_KISS_START);
	}
	else if (!strcmpi(szCmd, "slap"))
	{
		int iVID1 = atoi(TokenVector[1].c_str());
		int iVID2 = atoi(TokenVector[2].c_str());

		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance1 = rkChrMgr.GetInstancePtr(iVID1);
		CInstanceBase * pInstance2 = rkChrMgr.GetInstancePtr(iVID2);
		if (pInstance1 && pInstance2)
			pInstance1->ActDualEmotion(*pInstance2, CRaceMotionData::NAME_SLAP_HURT_START, CRaceMotionData::NAME_SLAP_HIT_START);
	}
	else if (!strcmpi(szCmd, "clap"))
	{
		int iVID = atoi(TokenVector[1].c_str());
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_CLAP);
	}
	else if (!strcmpi(szCmd, "cheer1"))
	{
		int iVID = atoi(TokenVector[1].c_str());
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_CHEERS_1);
	}
	else if (!strcmpi(szCmd, "cheer2"))
	{
		int iVID = atoi(TokenVector[1].c_str());
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_CHEERS_2);
	}
	else if (!strcmpi(szCmd, "dance1"))
	{
		int iVID = atoi(TokenVector[1].c_str());
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_DANCE_1);
	}
	else if (!strcmpi(szCmd, "dance2"))
	{
		int iVID = atoi(TokenVector[1].c_str());
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_DANCE_2);
	}
	else if (!strcmpi(szCmd, "dig_motion"))
	{
		int iVID = atoi(TokenVector[1].c_str());
		IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
		CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);
		if (pInstance)
			pInstance->ActEmotion(CRaceMotionData::NAME_DIG);
	}
	// Emotion End
#ifdef ENABLE_STANDING_MOUNT
	else if (!strcmpi(szCmd, "horse_state"))
	{
		if (TokenVector.size() < 4)
		{
			TraceError("CPythonNetworkStream::ServerCommand(c_szCommand=%s) - Strange Parameter Count : %s", c_szCommand);
			return;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
			"BINARY_ServerCommand_Run",
			Py_BuildValue("(s)", c_szCommand)
		);
	}
#endif
	else
	{
		static std::map<std::string, int> s_emotionDict;

		static bool s_isFirst = true;
		if (s_isFirst)
		{
			s_isFirst = false;

			s_emotionDict["dance3"] = CRaceMotionData::NAME_DANCE_3;
			s_emotionDict["dance4"] = CRaceMotionData::NAME_DANCE_4;
			s_emotionDict["dance5"] = CRaceMotionData::NAME_DANCE_5;
			s_emotionDict["dance6"] = CRaceMotionData::NAME_DANCE_6;
			s_emotionDict["congratulation"] = CRaceMotionData::NAME_CONGRATULATION;
			s_emotionDict["selfie"] = CRaceMotionData::NAME_SELFIE;
			s_emotionDict["dance7"] = CRaceMotionData::NAME_DANCE_7;
			s_emotionDict["doze"] = CRaceMotionData::NAME_DOZE;
			s_emotionDict["exercise"] = CRaceMotionData::NAME_EXERCISE;
			s_emotionDict["pushup"] = CRaceMotionData::NAME_PUSHUP;
			s_emotionDict["worship"] = CRaceMotionData::NAME_EMOTION_1;
			s_emotionDict["backflip"] = CRaceMotionData::NAME_EMOTION_2;
			s_emotionDict["breakdance"] = CRaceMotionData::NAME_EMOTION_3;
			s_emotionDict["dance8"] = CRaceMotionData::NAME_EMOTION_4;
			s_emotionDict["dab"] = CRaceMotionData::NAME_EMOTION_5;
			s_emotionDict["facepalm"] = CRaceMotionData::NAME_EMOTION_6;
			s_emotionDict["fuckoff"] = CRaceMotionData::NAME_EMOTION_7;
			s_emotionDict["wank"] = CRaceMotionData::NAME_EMOTION_8;
			s_emotionDict["shake_it"] = CRaceMotionData::NAME_EMOTION_9;
			s_emotionDict["throw_money"] = CRaceMotionData::NAME_EMOTION_10;
			s_emotionDict["dance9"] = CRaceMotionData::NAME_EMOTION_11;
			s_emotionDict["twerk"] = CRaceMotionData::NAME_EMOTION_12;
			
			s_emotionDict["samba"] = CRaceMotionData::NAME_EMOTION_13;
			s_emotionDict["shimmy"] = CRaceMotionData::NAME_EMOTION_14;
			s_emotionDict["slow2"] = CRaceMotionData::NAME_EMOTION_15;
			s_emotionDict["bestmates"] = CRaceMotionData::NAME_EMOTION_16;
			s_emotionDict["flair"] = CRaceMotionData::NAME_EMOTION_17;
			s_emotionDict["horon"] = CRaceMotionData::NAME_EMOTION_18;
			s_emotionDict["robotdance"] = CRaceMotionData::NAME_EMOTION_19;
			s_emotionDict["snowball"] = CRaceMotionData::NAME_EMOTION_20;
			s_emotionDict["surprised2"] = CRaceMotionData::NAME_EMOTION_21;
			s_emotionDict["forgive"] = CRaceMotionData::NAME_FORGIVE;
			s_emotionDict["angry"] = CRaceMotionData::NAME_ANGRY;
			s_emotionDict["attractive"] = CRaceMotionData::NAME_ATTRACTIVE;
			s_emotionDict["sad"] = CRaceMotionData::NAME_SAD;
			s_emotionDict["shy"] = CRaceMotionData::NAME_SHY;
			s_emotionDict["cheerup"] = CRaceMotionData::NAME_CHEERUP;
			s_emotionDict["banter"] = CRaceMotionData::NAME_BANTER;
			s_emotionDict["joy"] = CRaceMotionData::NAME_JOY;
		}

		std::map<std::string, int>::iterator f = s_emotionDict.find(szCmd);
		if (f == s_emotionDict.end())
		{
			TraceError("Unknown Server Command %s | %s", c_szCommand, szCmd);
		}
		else
		{
			int emotionIndex = f->second;

			int iVID = atoi(TokenVector[1].c_str());
			IAbstractCharacterManager & rkChrMgr = IAbstractCharacterManager::GetSingleton();
			CInstanceBase * pInstance = rkChrMgr.GetInstancePtr(iVID);

			if (pInstance)
				pInstance->ActEmotion(emotionIndex);
		}
	}
}
