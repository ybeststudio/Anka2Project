#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "Packet.h"
#include "PythonApplication.h"
#include "NetworkActorManager.h"

#include "AbstractPlayer.h"

#include "../eterPack/EterPackManager.h"

void CPythonNetworkStream::EnableChatInsultFilter(bool isEnable)
{
	m_isEnableChatInsultFilter=isEnable;
}

void CPythonNetworkStream::__FilterInsult(char *szLine, UINT uLineLen)
{
	m_kInsultChecker.FilterInsult(szLine, uLineLen);
}

bool CPythonNetworkStream::IsChatInsultIn(const char *c_szMsg)
{
	if (m_isEnableChatInsultFilter)
		return false;

	return IsInsultIn(c_szMsg);
}

bool CPythonNetworkStream::IsInsultIn(const char *c_szMsg)
{
	return m_kInsultChecker.IsInsultIn(c_szMsg, strlen(c_szMsg));
}

bool CPythonNetworkStream::LoadInsultList(const char *c_szInsultListFileName)
{
	CMappedFile file;
	const VOID* pvData;
	if (!CEterPackManager::Instance().Get(file, c_szInsultListFileName, &pvData))
		return false;

	CMemoryTextFileLoader kMemTextFileLoader;
	kMemTextFileLoader.Bind(file.Size(), pvData);

	m_kInsultChecker.Clear();
	for (DWORD dwLineIndex=0; dwLineIndex<kMemTextFileLoader.GetLineCount(); ++dwLineIndex)
	{
		const std::string& c_rstLine=kMemTextFileLoader.GetLineString(dwLineIndex);
		m_kInsultChecker.AppendInsult(c_rstLine);
	}
	return true;
}

#ifdef ENABLE_CLIENT_LOCALE_STRING
bool CPythonNetworkStream::LoadLocaleQuizVnum(const char *c_szLocaleQuizFileName)
{
	CMappedFile file;
	const VOID* pvData;

	if (!CEterPackManager::Instance().Get(file, c_szLocaleQuizFileName, &pvData))
	{
		Tracef("CPythonNetworkStream::LoadLocaleQuizVnum(c_szLocaleQuizFileName=%s) - Load Error", c_szLocaleQuizFileName);
		return false;
	}

	CMemoryTextFileLoader kMemTextFileLoader;
	kMemTextFileLoader.Bind(file.Size(), pvData);

	m_Quiz.clear();

	CTokenVector TokenVector;
	for (DWORD i = 0; i < kMemTextFileLoader.GetLineCount(); ++i)
	{
		if (!kMemTextFileLoader.SplitLineByTab(i, &TokenVector))
			continue;

		const char *c_szComment = "#";
		if (TokenVector[0].compare(0, 1, c_szComment) == 0 || TokenVector.size() != 2)
			continue;

		m_Quiz.emplace(atoi(TokenVector[0].c_str()), TokenVector[1]);
	}

	return true;
}

const char *CPythonNetworkStream::GetLocaleQuizVnum(DWORD id)
{
	const auto it = m_Quiz.find(id);

	static char s_szChat[1024 + 1] = "";
	if (it != m_Quiz.end())
	{
		snprintf(s_szChat, sizeof(s_szChat), "%s", it->second.c_str());
	}

	return s_szChat;
}

bool CPythonNetworkStream::LoadLocaleChatVnum(const char *c_szLocaleChatFileName)
{
	CMappedFile file;
	const VOID* pvData;

	if (!CEterPackManager::Instance().Get(file, c_szLocaleChatFileName, &pvData))
	{
		Tracef("CPythonNetworkStream::LoadLocaleChatVnum(c_szLocaleChatFileName=%s) - Load Error", c_szLocaleChatFileName);
		return false;
	}

	CMemoryTextFileLoader kMemTextFileLoader;
	kMemTextFileLoader.Bind(file.Size(), pvData);

	m_MonsterChat.clear();

	CTokenVector TokenVector;
	for (DWORD i = 0; i < kMemTextFileLoader.GetLineCount(); ++i)
	{
		if (!kMemTextFileLoader.SplitLineByTab(i, &TokenVector))
			continue;

		const char *c_szComment = "#";
		if (TokenVector[0].compare(0, 1, c_szComment) == 0 || TokenVector.size() != 2)
			continue;

		m_MonsterChat.insert(std::make_pair(atoi(TokenVector[0].c_str()), TokenVector[1]));
	}

	return true;
}

const char *CPythonNetworkStream::GetLocaleChatVnum(DWORD id)
{
	const auto it = m_MonsterChat.find(id);

	static char s_szChat[1024 + 1] = "";
	if (it != m_MonsterChat.end())
	{
		snprintf(s_szChat, sizeof(s_szChat), "%s", it->second.c_str());
	}

	return s_szChat;
}

bool CPythonNetworkStream::LoadLocaleStringVnum(const char *c_szLocaleStringFileName)
{
	CMappedFile file;
	const VOID* pvData;

	if (!CEterPackManager::Instance().Get(file, c_szLocaleStringFileName, &pvData))
	{
		Tracef("CPythonNetworkStream::LoadLocaleStringVnum(c_szLocaleStringFileName=%s) - Load Error", c_szLocaleStringFileName);
		return false;
	}

	CMemoryTextFileLoader kMemTextFileLoader;
	kMemTextFileLoader.Bind(file.Size(), pvData);

	m_String.clear();

	CTokenVector TokenVector;
	for (DWORD i = 0; i < kMemTextFileLoader.GetLineCount(); ++i)
	{
		if (!kMemTextFileLoader.SplitLineByTab(i, &TokenVector))
			continue;

		while (TokenVector.size() < 2)
		{
			TokenVector.push_back("");
		}

		DWORD id = atoi(TokenVector[0].c_str());
		const std::string& text = TokenVector[1].c_str();

		if (text.empty())
			continue;

		m_String.insert(std::pair<DWORD, std::string> (id, text));
	}

	return true;
}

std::string CPythonNetworkStream::GetLocaleStringVnum(DWORD id)
{
	auto it = m_String.find(id);
	if (it != m_String.end())
	{
		return it->second;
	}

	return "";
}
#endif

bool CPythonNetworkStream::LoadConvertTable(DWORD dwEmpireID, const char *c_szFileName)
{
	if (dwEmpireID<1 || dwEmpireID>=4)
		return false;

	CMappedFile file;
	const VOID* pvData;
	if (!CEterPackManager::Instance().Get(file, c_szFileName, &pvData))
		return false;

	DWORD dwEngCount=26;
	DWORD dwHanCount=(0xc8-0xb0+1)*(0xfe-0xa1+1);
	DWORD dwHanSize=dwHanCount*2;
	DWORD dwFileSize=dwEngCount*2+dwHanSize;

	if (file.Size()<dwFileSize)
		return false;

	char *pcData=(char *)pvData;

	STextConvertTable& rkTextConvTable=m_aTextConvTable[dwEmpireID-1];
	memcpy(rkTextConvTable.acUpper, pcData, dwEngCount);pcData+=dwEngCount;
	memcpy(rkTextConvTable.acLower, pcData, dwEngCount);pcData+=dwEngCount;
	memcpy(rkTextConvTable.aacHan, pcData, dwHanSize);

	return true;
}

// Loading ---------------------------------------------------------------------------
void CPythonNetworkStream::LoadingPhase()
{
	TPacketHeader header;

	if (!CheckPacket (&header))
	{
		return;
	}

	switch (header)
	{
		case HEADER_GC_PHASE:
			if (RecvPhasePacket())
			{
				return;
			}
			break;

		case HEADER_GC_MAIN_CHARACTER:
			if (RecvMainCharacter())
			{
				return;
			}
			break;

		// SUPPORT_BGM
		case HEADER_GC_MAIN_CHARACTER2_EMPIRE:
			if (RecvMainCharacter2_EMPIRE())
			{
				return;
			}
			break;

		case HEADER_GC_MAIN_CHARACTER3_BGM:
			if (RecvMainCharacter3_BGM())
			{
				return;
			}
			break;

		case HEADER_GC_MAIN_CHARACTER4_BGM_VOL:
			if (RecvMainCharacter4_BGM_VOL())
			{
				return;
			}
			break;

		// END_OF_SUPPORT_BGM

		case HEADER_GC_CHARACTER_UPDATE:
			if (RecvCharacterUpdatePacket())
			{
				return;
			}
			break;

		case HEADER_GC_PLAYER_POINTS:
			if (__RecvPlayerPoints())
			{
				return;
			}
			break;

		case HEADER_GC_PLAYER_POINT_CHANGE:
			if (RecvPointChange())
			{
				return;
			}
			break;

		case HEADER_GC_ITEM_SET:
			if (RecvItemSetPacket())
			{
				return;
			}
			break;

		case HEADER_GC_PING:
			if (RecvPingPacket())
			{
				return;
			}
			break;

		case HEADER_GC_QUICKSLOT_ADD:
			if (RecvQuickSlotAddPacket())
			{
				return;
			}
			break;

#ifdef ENABLE_GROWTH_PET_SYSTEM
		case HEADER_GC_PET_SET:
			if (RecvPetSet())
				return;
			break;
#endif

		default:
			GamePhase();
			return;
			break;
	}

	RecvErrorPacket (header);
}

void CPythonNetworkStream::SetLoadingPhase()
{
	if ("Loading"!=m_strPhase)
	{
		m_phaseLeaveFunc.Run();
	}

	Tracen ("");
	Tracen ("## Network - Loading Phase ##");
	Tracen ("");

	m_strPhase = "Loading";

	m_dwChangingPhaseTime = ELTimer_GetMSec();
	m_phaseProcessFunc.Set (this, &CPythonNetworkStream::LoadingPhase);
	m_phaseLeaveFunc.Set (this, &CPythonNetworkStream::__LeaveLoadingPhase);

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.Clear();

	CFlyingManager::Instance().DeleteAllInstances();
	CEffectManager::Instance().DeleteAllInstances();

	__DirectEnterMode_Initialize();
}

bool CPythonNetworkStream::RecvMainCharacter()
{
	TPacketGCMainCharacter mainChrPacket;
	if (!Recv(sizeof(mainChrPacket), &mainChrPacket))
		return false;

	m_dwMainActorVID = mainChrPacket.dwVID;
	m_dwMainActorRace = mainChrPacket.wRaceNum;
	m_dwMainActorEmpire = mainChrPacket.byEmpire;
	m_dwMainActorSkillGroup = mainChrPacket.bySkillGroup;

	m_rokNetActorMgr->SetMainActorVID(m_dwMainActorVID);

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.SetName(mainChrPacket.szName);
	rkPlayer.SetMainCharacterIndex(GetMainActorVID());

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.StartGame();

	Warp(mainChrPacket.lX, mainChrPacket.lY);
	return true;
}

bool CPythonNetworkStream::RecvMainCharacter2_EMPIRE()
{
	TPacketGCMainCharacter2_EMPIRE mainChrPacket;
	if (!Recv(sizeof(mainChrPacket), &mainChrPacket))
		return false;

	m_dwMainActorVID = mainChrPacket.dwVID;
	m_dwMainActorRace = mainChrPacket.wRaceNum;
	m_dwMainActorEmpire = mainChrPacket.byEmpire;
	m_dwMainActorSkillGroup = mainChrPacket.bySkillGroup;

	m_rokNetActorMgr->SetMainActorVID(m_dwMainActorVID);

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.SetName(mainChrPacket.szName);
	rkPlayer.SetMainCharacterIndex(GetMainActorVID());

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.StartGame();

	Warp(mainChrPacket.lX, mainChrPacket.lY);
	return true;
}

bool CPythonNetworkStream::RecvMainCharacter3_BGM()
{
	TPacketGCMainCharacter3_BGM mainChrPacket;
	if (!Recv(sizeof(mainChrPacket), &mainChrPacket))
		return false;

	m_dwMainActorVID = mainChrPacket.dwVID;
	m_dwMainActorRace = mainChrPacket.wRaceNum;
	m_dwMainActorEmpire = mainChrPacket.byEmpire;
	m_dwMainActorSkillGroup = mainChrPacket.bySkillGroup;

	m_rokNetActorMgr->SetMainActorVID(m_dwMainActorVID);

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.SetName(mainChrPacket.szUserName);
	rkPlayer.SetMainCharacterIndex(GetMainActorVID());

	__SetFieldMusicFileName(mainChrPacket.szBGMName);

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.StartGame();

	Warp(mainChrPacket.lX, mainChrPacket.lY);
	return true;
}

bool CPythonNetworkStream::RecvMainCharacter4_BGM_VOL()
{
	TPacketGCMainCharacter4_BGM_VOL mainChrPacket;
	if (!Recv(sizeof(mainChrPacket), &mainChrPacket))
		return false;

	m_dwMainActorVID = mainChrPacket.dwVID;
	m_dwMainActorRace = mainChrPacket.wRaceNum;
	m_dwMainActorEmpire = mainChrPacket.byEmpire;
	m_dwMainActorSkillGroup = mainChrPacket.bySkillGroup;

	m_rokNetActorMgr->SetMainActorVID(m_dwMainActorVID);

	CPythonPlayer& rkPlayer=CPythonPlayer::Instance();
	rkPlayer.SetName(mainChrPacket.szUserName);
	rkPlayer.SetMainCharacterIndex(GetMainActorVID());

	__SetFieldMusicFileInfo(mainChrPacket.szBGMName, mainChrPacket.fBGMVol);

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.StartGame();

	Warp(mainChrPacket.lX, mainChrPacket.lY);
	return true;
}

static std::string gs_fieldMusic_fileName;
static float gs_fieldMusic_volume = 1.0f / 5.0f * 0.1f;

void CPythonNetworkStream::__SetFieldMusicFileName(const char *musicName)
{
	gs_fieldMusic_fileName = musicName;
}

void CPythonNetworkStream::__SetFieldMusicFileInfo(const char *musicName, float vol)
{
	gs_fieldMusic_fileName = musicName;
	gs_fieldMusic_volume = vol;
}

const char *CPythonNetworkStream::GetFieldMusicFileName()
{
	return gs_fieldMusic_fileName.c_str();	
}

float CPythonNetworkStream::GetFieldMusicVolume()
{
	return gs_fieldMusic_volume;
}
// END_OF_SUPPORT_BGM


bool CPythonNetworkStream::__RecvPlayerPoints()
{
	TPacketGCPoints PointsPacket;

	if (!Recv(sizeof(TPacketGCPoints), &PointsPacket))
		return false;

	for (DWORD i = 0; i < POINT_MAX_NUM; ++i)
	{
		CPythonPlayer::Instance().SetStatus(i, PointsPacket.points[i]);
		if (i == POINT_LEVEL)
			m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byLevel = PointsPacket.points[i];
		else if (i == POINT_ST)
			m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byST = PointsPacket.points[i];
		else if (i == POINT_HT)
			m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byHT = PointsPacket.points[i];
		else if (i == POINT_DX)
			m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byDX = PointsPacket.points[i];
		else if (i == POINT_IQ)
			m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].byIQ = PointsPacket.points[i];

	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshStatus", Py_BuildValue("()"));
	return true;
}

void CPythonNetworkStream::StartGame()
{
	m_isStartGame=TRUE;
}

bool CPythonNetworkStream::SendEnterGame()
{
	TPacketCGEnterFrontGame EnterFrontGamePacket;

	EnterFrontGamePacket.header = HEADER_CG_ENTERGAME;

	if (!Send(sizeof(EnterFrontGamePacket), &EnterFrontGamePacket))
	{
		Tracen("Send EnterFrontGamePacket");
		return false;
	}

	if (!SendSequence())
		return false;

	m_akSimplePlayerInfo[m_dwSelectedCharacterIndex].dwLastPlay = static_cast<DWORD>(time(0));

	__SendInternalBuffer();
	return true;
}

#ifdef ENABLE_CLIENT_PERFORMANCE
void CPythonNetworkStream::SendPythonData(PyObject *obj, const char *funcname)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_LOGIN], funcname, Py_BuildValue("(O)", obj));
}
#endif