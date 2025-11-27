#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "AccountConnector.h"
#include "PythonGuild.h"
#include "AbstractPlayer.h"

#ifdef ENABLE_EVENT_MANAGER
	#include "PythonGameEventManager.h"
#endif

#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
	#include "PythonDynamicModuleNames.h"
#endif

#ifdef ENABLE_CLIENT_PERFORMANCE
	#include "PythonPlayerSettingsModule.h"
#endif

static std::string gs_stServerInfo;
extern BOOL gs_bEmpireLanuageEnable;
std::list<std::string> g_kList_strCommand;

PyObject *netGetBettingGuildWarValue(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.EXPORT_GetBettingGuildWarValue(szName));
}

PyObject *netSetServerInfo(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	gs_stServerInfo=szFileName;
	return Py_BuildNone();
}

PyObject *netGetServerInfo(PyObject *poSelf, PyObject *poArgs)
{
	return Py_BuildValue("s", gs_stServerInfo.c_str());
}

PyObject *netPreserveServerCommand(PyObject *poSelf, PyObject *poArgs)
{
	char *szLine;
	if (!PyTuple_GetString(poArgs, 0, &szLine))
		return Py_BuildException();

	g_kList_strCommand.push_back(szLine);

	return Py_BuildNone();
}

PyObject *netGetPreservedServerCommand(PyObject *poSelf, PyObject *poArgs)
{
	if (g_kList_strCommand.empty())
		return Py_BuildValue("s", "");

	std::string strCommand = g_kList_strCommand.front();
	g_kList_strCommand.pop_front();

	return Py_BuildValue("s", strCommand.c_str());
}

PyObject *netStartGame(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.StartGame();

	return Py_BuildNone();
}

PyObject *netWarp(PyObject *poSelf, PyObject *poArgs)
{
	int nX;
	if (!PyTuple_GetInteger(poArgs, 0, &nX))
		return Py_BuildException();

	int nY;
	if (!PyTuple_GetInteger(poArgs, 1, &nY))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.Warp(nX, nY);

	return Py_BuildNone();
}

PyObject *netLoadInsultList(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.LoadInsultList(szFileName));
}

PyObject *netUploadMark(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.UploadMark(szFileName));
}

PyObject *netUploadSymbol(PyObject *poSelf, PyObject *poArgs)
{
	char *szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.UploadSymbol(szFileName));
}

PyObject *netGetGuildID(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetGuildID());
}

PyObject *netGetEmpireID(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetEmpireID());
}

PyObject *netGetMainActorVID(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorVID());
}

PyObject *netGetMainActorRace(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorRace());
}

PyObject *netGetMainActorEmpire(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorEmpire());
}

PyObject *netGetMainActorSkillGroup(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorSkillGroup());
}

PyObject *netIsSelectedEmpire(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsSelectedEmpire());
}


PyObject *netGetAccountCharacterSlotDataInteger(PyObject *poSelf, PyObject *poArgs)
{
	int nIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &nIndex))
		return Py_BuildException();

	int nType;
	if (!PyTuple_GetInteger(poArgs, 1, &nType))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	UINT uResult=rkNetStream.GetAccountCharacterSlotDatau(nIndex, nType);
	return Py_BuildValue("i", uResult);
}

PyObject *netGetAccountCharacterSlotDataString(PyObject *poSelf, PyObject *poArgs)
{
	int nIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &nIndex))
		return Py_BuildException();

	int nType;
	if (!PyTuple_GetInteger(poArgs, 1, &nType))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("s", rkNetStream.GetAccountCharacterSlotDataz(nIndex, nType));
}

// SUPPORT_BGM
PyObject *netGetFieldMusicFileName(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("s", rkNetStream.GetFieldMusicFileName());
}

PyObject *netGetFieldMusicVolume(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("f", rkNetStream.GetFieldMusicVolume());
}
// END_OF_SUPPORT_BGM

PyObject *netSetPhaseWindow(PyObject *poSelf, PyObject *poArgs)
{
	int ePhaseWnd;
	if (!PyTuple_GetInteger(poArgs, 0, &ePhaseWnd))
		return Py_BuildException();

	PyObject *poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 1, &poPhaseWnd))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetPhaseWindow(ePhaseWnd, poPhaseWnd);
	return Py_BuildNone();
}

PyObject *netClearPhaseWindow(PyObject *poSelf, PyObject *poArgs)
{
	int ePhaseWnd;
	if (!PyTuple_GetInteger(poArgs, 0, &ePhaseWnd))
		return Py_BuildException();

	PyObject *poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 1, &poPhaseWnd))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ClearPhaseWindow(ePhaseWnd, poPhaseWnd);
	return Py_BuildNone();
}

PyObject *netSetServerCommandParserWindow(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 0, &poPhaseWnd))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetServerCommandParserWindow(poPhaseWnd);
	return Py_BuildNone();
}

PyObject *netSetAccountConnectorHandler(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 0, &poPhaseWnd))
		return Py_BuildException();

	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rkAccountConnector.SetHandler(poPhaseWnd);
	return Py_BuildNone();
}

PyObject *netSetHandler(PyObject *poSelf, PyObject *poArgs)
{
	PyObject *poHandler;

	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetHandler(poHandler);
	return Py_BuildNone();
}

PyObject *netSetTCPRecvBufferSize(PyObject *poSelf, PyObject *poArgs)
{
	int bufSize;
	if (!PyTuple_GetInteger(poArgs, 0, &bufSize))
		return Py_BuildException();

#ifdef ENABLE_LARGE_DYNAMIC_PACKET
	bufSize = MAX(bufSize, (300 * 1024));
#endif

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetRecvBufferSize(bufSize);
	return Py_BuildNone();
}

PyObject *netSetTCPSendBufferSize(PyObject *poSelf, PyObject *poArgs)
{
	int bufSize;
	if (!PyTuple_GetInteger(poArgs, 0, &bufSize))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetSendBufferSize(bufSize);
	return Py_BuildNone();
}

PyObject *netSetUDPRecvBufferSize(PyObject *poSelf, PyObject *poArgs)
{
	int bufSize;
	if (!PyTuple_GetInteger(poArgs, 0, &bufSize))
		return Py_BuildException();

	//CPythonNetworkDatagram::Instance().SetRecvBufferSize(bufSize);
	return Py_BuildNone();
}

PyObject *netSetMarkServer(PyObject *poSelf, PyObject *poArgs)
{
	char *szAddr;
	if (!PyTuple_GetString(poArgs, 0, &szAddr))
		return Py_BuildException();

	int port;
	if (!PyTuple_GetInteger(poArgs, 1, &port))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetMarkServer(szAddr, port);
	return Py_BuildNone();
}

PyObject *netConnectTCP(PyObject *poSelf, PyObject *poArgs)
{
	char *szAddr;
	if (!PyTuple_GetString(poArgs, 0, &szAddr))
		return Py_BuildException();

	int port;
	if (!PyTuple_GetInteger(poArgs, 1, &port))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ConnectLoginServer(szAddr, port);
	return Py_BuildNone();
}

PyObject *netConnectUDP(PyObject *poSelf, PyObject *poArgs)
{
	char *c_szIP;
	if (!PyTuple_GetString(poArgs, 0, &c_szIP))
		return Py_BuildException();
	int iPort;
	if (!PyTuple_GetInteger(poArgs, 1, &iPort))
		return Py_BuildException();

	//CPythonNetworkDatagram::Instance().SetConnection(c_szIP, iPort);
	return Py_BuildNone();
}

PyObject *netConnectToAccountServer(PyObject *poSelf, PyObject *poArgs)
{
	char *addr;
	if (!PyTuple_GetString(poArgs, 0, &addr))
		return Py_BuildException();

	int port;
	if (!PyTuple_GetInteger(poArgs, 1, &port))
		return Py_BuildException();

	char *account_addr;
	if (!PyTuple_GetString(poArgs, 2, &account_addr))
		return Py_BuildException();

	int account_port;
	if (!PyTuple_GetInteger(poArgs, 3, &account_port))
		return Py_BuildException();

	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rkAccountConnector.Connect(addr, port, account_addr, account_port);
	return Py_BuildNone();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

PyObject *netSetLoginInfo(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	char *szPwd;
	if (!PyTuple_GetString(poArgs, 1, &szPwd))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rkNetStream.SetLoginInfo(szName, szPwd);
	rkAccountConnector.SetLoginInfo(szName, szPwd);
	return Py_BuildNone();
}

PyObject *netSetOfflinePhase(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetOffLinePhase();
	return Py_BuildNone();
}

PyObject *netSendSelectEmpirePacket(PyObject *poSelf, PyObject *poArgs)
{
	int iEmpireIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iEmpireIndex))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendSelectEmpirePacket(iEmpireIndex);
	return Py_BuildNone();
}

PyObject *netSendLoginPacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	char *szPwd;
	if (!PyTuple_GetString(poArgs, 1, &szPwd))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendLoginPacket(szName, szPwd);
	return Py_BuildNone();
}

PyObject *netDirectEnter(PyObject *poSelf, PyObject *poArgs)
{
	int nChrSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &nChrSlot))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ConnectGameServer(nChrSlot);
	return Py_BuildNone();
}

PyObject *netSendSelectCharacterPacket(PyObject *poSelf, PyObject *poArgs)
{
	int Index;
	if (!PyTuple_GetInteger(poArgs, 0, &Index))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendSelectCharacterPacket((BYTE) Index);
	return Py_BuildNone();
}

PyObject *netSendChangeNamePacket(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();
	char *szName;
	if (!PyTuple_GetString(poArgs, 1, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendChangeNamePacket((BYTE)iIndex, szName);
	return Py_BuildNone();
}

PyObject *netEnableChatInsultFilter(PyObject *poSelf, PyObject *poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.EnableChatInsultFilter(nEnable ? true : false);
	return Py_BuildNone();
}

PyObject *netIsChatInsultIn(PyObject *poSelf, PyObject *poArgs)
{
	char *szMsg;
	if (!PyTuple_GetString(poArgs, 0, &szMsg))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsChatInsultIn(szMsg));
}


PyObject *netIsInsultIn(PyObject *poSelf, PyObject *poArgs)
{
	char *szMsg;
	if (!PyTuple_GetString(poArgs, 0, &szMsg))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsInsultIn(szMsg));
}

PyObject *netSendWhisperPacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	char *szLine;

	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	if (!PyTuple_GetString(poArgs, 1, &szLine))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendWhisperPacket(szName, szLine);
	return Py_BuildNone();
}

PyObject *netSendCharacterPositionPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iPosition;
	if (!PyTuple_GetInteger(poArgs, 0, &iPosition))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendCharacterPositionPacket((BYTE) iPosition);
	return Py_BuildNone();
}

PyObject *netSendChatPacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szLine;
	if (!PyTuple_GetString(poArgs, 0, &szLine))
		return Py_BuildException();
	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
	{
		iType = CHAT_TYPE_TALKING;
	}

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendChatPacket(szLine, iType);
	return Py_BuildNone();
}

PyObject *netSendEmoticon(PyObject *poSelf, PyObject *poArgs)
{
	int eEmoticon;
	if (!PyTuple_GetInteger(poArgs, 0, &eEmoticon))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendEmoticon(eEmoticon);
	return Py_BuildNone();
}

PyObject *netSendCreateCharacterPacket(PyObject *poSelf, PyObject *poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	char *name;
	if (!PyTuple_GetString(poArgs, 1, &name))
		return Py_BuildException();

	int job;
	if (!PyTuple_GetInteger(poArgs, 2, &job))
		return Py_BuildException();

	int shape;
	if (!PyTuple_GetInteger(poArgs, 3, &shape))
		return Py_BuildException();

	int stat1;
	if (!PyTuple_GetInteger(poArgs, 4, &stat1))
		return Py_BuildException();
	int stat2;
	if (!PyTuple_GetInteger(poArgs, 5, &stat2))
		return Py_BuildException();
	int stat3;
	if (!PyTuple_GetInteger(poArgs, 6, &stat3))
		return Py_BuildException();
	int stat4;
	if (!PyTuple_GetInteger(poArgs, 7, &stat4))
		return Py_BuildException();

	if (index<0 && index>3)
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendCreateCharacterPacket((BYTE) index, name, (BYTE) job, (BYTE) shape, stat1, stat2, stat3, stat4);
	return Py_BuildNone();
}

PyObject *netSendDestroyCharacterPacket(PyObject *poSelf, PyObject *poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	char *szPrivateCode;
	if (!PyTuple_GetString(poArgs, 1, &szPrivateCode))
		return Py_BuildException();

	if (index<0 && index>3)
		return Py_BuildException();
	
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendDestroyCharacterPacket((BYTE) index, szPrivateCode);
	return Py_BuildNone();
}

PyObject *netSendEnterGamePacket(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendEnterGame();
	return Py_BuildNone();
}

PyObject *netOnClickPacket(PyObject *poSelf, PyObject *poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendOnClickPacket(index);

	return Py_BuildNone();
}

PyObject *netSendItemUsePacket(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();
		break;
	case 2:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemUsePacket(Cell);
	return Py_BuildNone();
}

PyObject *netSendItemUseToItemPacket(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos SourceCell;
	TItemPos TargetCell;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &SourceCell.cell))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &TargetCell.cell))
			return Py_BuildException();
		break;
	case 4:
		if (!PyTuple_GetByte(poArgs, 0, &SourceCell.window_type))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 1, &SourceCell.cell))
			return Py_BuildException();

		if (!PyTuple_GetByte(poArgs, 2, &TargetCell.window_type))
			return Py_BuildException();

		if (!PyTuple_GetInteger(poArgs, 3, &TargetCell.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemUseToItemPacket(SourceCell, TargetCell);
	return Py_BuildNone();
}

PyObject *netSendItemDropPacket(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();
		break;
	case 2:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}


	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacket(Cell, 0);
	return Py_BuildNone();
}

PyObject *netSendItemDropPacketNew(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos Cell;
	int count;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &count))
			return Py_BuildException();

		break;
	case 3:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &count))
			return Py_BuildException();
		
		break;
	default:
		return Py_BuildException();
	}
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacketNew(Cell, 0, count);
	return Py_BuildNone();
}

PyObject *netSendElkDropPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iElk;
	if (!PyTuple_GetInteger(poArgs, 0, &iElk))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacket(TItemPos(RESERVED_WINDOW, 0), (DWORD) iElk);
	return Py_BuildNone();
}

PyObject *netSendGoldDropPacketNew(PyObject *poSelf, PyObject *poArgs)
{
	int iElk;
	if (!PyTuple_GetInteger(poArgs, 0, &iElk))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacketNew(TItemPos (RESERVED_WINDOW, 0), (DWORD) iElk, 0);
	return Py_BuildNone();
}

PyObject *netSendItemMovePacket(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos Cell;
	TItemPos ChangeCell;
	int num;

	switch (PyTuple_Size(poArgs))
	{
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &ChangeCell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &num))
			return Py_BuildException();
		break;
	case 5:
		{
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();
			if (!PyTuple_GetByte(poArgs, 2, &ChangeCell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 3, &ChangeCell.cell))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 4, &num))
				return Py_BuildException();
		}
		break;
	default:
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
#ifdef ENABLE_STACK_LIMIT
	rkNetStream.SendItemMovePacket(Cell, ChangeCell, (WORD) num);
#else
	rkNetStream.SendItemMovePacket(Cell, ChangeCell, (BYTE) num);
#endif
	return Py_BuildNone();
}

PyObject *netSendItemPickUpPacket(PyObject *poSelf, PyObject *poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendItemPickUpPacket(vid);
	return Py_BuildNone();
}

PyObject *netSendGiveItemPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iTargetVID;
	TItemPos Cell;
	int iItemCount;
	switch (PyTuple_Size(poArgs))
	{
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iTargetVID))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &iItemCount))
			return Py_BuildException();
		break;
	case 4:
		if (!PyTuple_GetInteger(poArgs, 0, &iTargetVID))
			return Py_BuildException();
		if (!PyTuple_GetByte(poArgs, 1, &Cell.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &Cell.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 3, &iItemCount))
			return Py_BuildException();
		break;
	default:
		break;
	}

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendGiveItemPacket(iTargetVID, Cell, iItemCount);
	return Py_BuildNone();
}

PyObject *netSendShopEndPacket(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendShopEndPacket();
	return Py_BuildNone();
}

PyObject *netSendShopBuyPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iCount;
	if (!PyTuple_GetInteger(poArgs, 0, &iCount))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendShopBuyPacket(iCount);
	return Py_BuildNone();
}

PyObject *netSendShopSellPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotNumber))
		return Py_BuildException();
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendShopSellPacket(iSlotNumber);
	return Py_BuildNone();
}

PyObject *netSendShopSellPacketNew(PyObject *poSelf, PyObject *poArgs)
{
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotNumber))
		return Py_BuildException();

	int iCount;
	if (!PyTuple_GetInteger(poArgs, 1, &iCount))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendShopSellPacketNew(iSlotNumber, iCount);

	return Py_BuildNone();
}

PyObject *netSendExchangeStartPacket(PyObject *poSelf, PyObject *poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeStartPacket(vid);
	return Py_BuildNone();
}

PyObject *netSendExchangeElkAddPacket(PyObject *poSelf, PyObject *poArgs)
{
#ifdef ENABLE_GOLD_LIMIT
	long long iElk;
	if (!PyTuple_GetLongLong(poArgs, 0, &iElk))
#else
	int iElk;
	if (!PyTuple_GetInteger(poArgs, 0, &iElk))
#endif
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeElkAddPacket(iElk);
	return Py_BuildNone();
}

PyObject *netSendExchangeItemAddPacket(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bWindowType))
		return Py_BuildException();
	WORD wSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wSlotIndex))
		return Py_BuildException();
	int iDisplaySlotIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iDisplaySlotIndex))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeItemAddPacket(TItemPos(bWindowType, wSlotIndex), iDisplaySlotIndex);
	return Py_BuildNone();
}

PyObject *netSendExchangeItemDelPacket(PyObject *poSelf, PyObject *poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeItemDelPacket((BYTE) pos);
	return Py_BuildNone();
}

PyObject *netSendExchangeAcceptPacket(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeAcceptPacket();
	return Py_BuildNone();
}

PyObject *netSendExchangeExitPacket(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeExitPacket();
	return Py_BuildNone();
}

PyObject *netExitApplication(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ExitApplication();
	return Py_BuildNone();
}

PyObject *netExitGame(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ExitGame();
	return Py_BuildNone();
}

PyObject *netLogOutGame(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.LogOutGame();
	return Py_BuildNone();
}

PyObject *netDisconnect(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.SetOffLinePhase();
	rkNetStream.Disconnect();

	return Py_BuildNone();
}

PyObject *netIsConnect(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsOnline());
}

PyObject *netToggleGameDebugInfo(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.ToggleGameDebugInfo();
	return Py_BuildNone();
}

PyObject *netRegisterEmoticonString(PyObject *poSelf, PyObject *poArgs)
{
	char *pcEmoticonString;
	if (!PyTuple_GetString(poArgs, 0, &pcEmoticonString))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream=CPythonNetworkStream::Instance();
	rkNetStream.RegisterEmoticonString(pcEmoticonString);
	return Py_BuildNone();
}

#ifdef ENABLE_MESSENGER_BLOCK
PyObject *netSendMessengerBlockAddByVIDPacket(PyObject *poSelf, PyObject *poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendMessengerBlockAddByVIDPacket(vid);

	return Py_BuildNone();
}

PyObject *netSendMessengerBlockAddByNamePacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendMessengerBlockAddByNamePacket(szName);

	return Py_BuildNone();
}

PyObject *netSendMessengerBlockRemovePacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szKey;
	if (!PyTuple_GetString(poArgs, 0, &szKey))
		return Py_BuildException();

	char *szName;
	if (!PyTuple_GetString(poArgs, 1, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendMessengerBlockRemovePacket(szKey, szName);

	return Py_BuildNone();
}

PyObject *netSendMessengerBlockRemoveByVIDPacket(PyObject *poSelf, PyObject *poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendMessengerBlockRemoveByVIDPacket(vid);

	return Py_BuildNone();
}
#endif

PyObject *netSendMessengerAddByVIDPacket(PyObject *poSelf, PyObject *poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendMessengerAddByVIDPacket(vid);
	
	return Py_BuildNone();
}

PyObject *netSendMessengerAddByNamePacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendMessengerAddByNamePacket(szName);
	
	return Py_BuildNone();
}

PyObject *netSendMessengerRemovePacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szKey;
	if (!PyTuple_GetString(poArgs, 0, &szKey))
		return Py_BuildException();
	char *szName;
	if (!PyTuple_GetString(poArgs, 1, &szName))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendMessengerRemovePacket(szKey, szName);
	
	return Py_BuildNone();
}

PyObject *netSendPartyInvitePacket(PyObject *poSelf, PyObject *poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartyInvitePacket(vid);

	return Py_BuildNone();
}

PyObject *netSendPartyInviteAnswerPacket(PyObject *poSelf, PyObject *poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();
	int answer;
	if (!PyTuple_GetInteger(poArgs, 1, &answer))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartyInviteAnswerPacket(vid, answer);

	return Py_BuildNone();
}

PyObject *netSendPartyExitPacket(PyObject *poSelf, PyObject *poArgs)
{
	IAbstractPlayer& rPlayer=IAbstractPlayer::GetSingleton();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();

	DWORD dwVID = rPlayer.GetMainCharacterIndex();
	DWORD dwPID;
	if (rPlayer.PartyMemberVIDToPID(dwVID, &dwPID))
		rns.SendPartyRemovePacket(dwPID);

	return Py_BuildNone();
}

PyObject *netSendPartyRemovePacket(PyObject *poSelf, PyObject *poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartyRemovePacket(vid);

	return Py_BuildNone();
}

PyObject *netSendPartySetStatePacket(PyObject *poSelf, PyObject *poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();
	int iState;
	if (!PyTuple_GetInteger(poArgs, 1, &iState))
		return Py_BuildException();
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 2, &iFlag))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartySetStatePacket(iVID, iState, iFlag);

	return Py_BuildNone();
}

PyObject *netSendPartyUseSkillPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iSkillIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillIndex))
		return Py_BuildException();
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 1, &iVID))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartyUseSkillPacket(iSkillIndex, iVID);

	return Py_BuildNone();
}

PyObject *netSendPartyParameterPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iMode;
	if (!PyTuple_GetInteger(poArgs, 0, &iMode))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendPartyParameterPacket(iMode);

	return Py_BuildNone();
}

PyObject *netSendSafeboxSaveMoneyPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iMoney;
	if (!PyTuple_GetInteger(poArgs, 0, &iMoney))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSafeBoxMoneyPacket(SAFEBOX_MONEY_STATE_SAVE, iMoney);

	return Py_BuildNone();
}

PyObject *netSendSafeboxWithdrawMoneyPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iMoney;
	if (!PyTuple_GetInteger(poArgs, 0, &iMoney))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSafeBoxMoneyPacket(SAFEBOX_MONEY_STATE_WITHDRAW, iMoney);

	return Py_BuildNone();
}

PyObject *netSendSafeboxCheckinPacket(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos InventoryPos;
	int iSafeBoxPos;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		InventoryPos.window_type = INVENTORY;
		if (!PyTuple_GetInteger(poArgs, 0, &InventoryPos.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &iSafeBoxPos))
			return Py_BuildException();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &InventoryPos.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.cell))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &iSafeBoxPos))
			return Py_BuildException();
		break;

	}

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSafeBoxCheckinPacket(InventoryPos, iSafeBoxPos);

	return Py_BuildNone();
}

PyObject *netSendSafeboxCheckoutPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iSafeBoxPos;
	TItemPos InventoryPos;
	
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iSafeBoxPos))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.cell))
			return Py_BuildException();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iSafeBoxPos))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &InventoryPos.cell))
			return Py_BuildException();
		break;
	default:
		break;
	}

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSafeBoxCheckoutPacket(iSafeBoxPos, InventoryPos);

	return Py_BuildNone();
}

PyObject *netSendSafeboxItemMovePacket(PyObject *poSelf, PyObject *poArgs)
{
	int iSourcePos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSourcePos))
		return Py_BuildException();
	int iTargetPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iTargetPos))
		return Py_BuildException();
	int iCount;
	if (!PyTuple_GetInteger(poArgs, 2, &iCount))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSafeBoxItemMovePacket(iSourcePos, iTargetPos, iCount);

	return Py_BuildNone();
}

PyObject *netSendMallCheckoutPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iMallPos;
	TItemPos InventoryPos;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iMallPos))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.cell))
			return Py_BuildException();
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iMallPos))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.window_type))
			return Py_BuildException();
		if (!PyTuple_GetInteger(poArgs, 2, &InventoryPos.cell))
			return Py_BuildException();
		break;
	default:
		return Py_BuildException();
	}
	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendMallCheckoutPacket(iMallPos, InventoryPos);

	return Py_BuildNone();
}

PyObject *netSendQuestInputStringPacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szString;
	if (!PyTuple_GetString(poArgs, 0, &szString))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendQuestInputStringPacket(szString);

	return Py_BuildNone();
}

PyObject *netSendQuestConfirmPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iAnswer;
	if (!PyTuple_GetInteger(poArgs, 0, &iAnswer))
		return Py_BuildException();
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 1, &iPID))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendQuestConfirmPacket(iAnswer, iPID);

	return Py_BuildNone();
}

PyObject *netSendGuildAddMemberPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildAddMemberPacket(iVID);

	return Py_BuildNone();
}

PyObject *netSendGuildRemoveMemberPacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szKey;
	if (!PyTuple_GetString(poArgs, 0, &szKey))
		return Py_BuildException();

	CPythonGuild::TGuildMemberData * pGuildMemberData;
	if (!CPythonGuild::Instance().GetMemberDataPtrByName(szKey, &pGuildMemberData))
	{
		TraceError("netSendGuildRemoveMemberPacket(szKey=%s) - Can't Find Guild Member\n", szKey);
		return Py_BuildNone();
	}

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildRemoveMemberPacket(pGuildMemberData->dwPID);

	return Py_BuildNone();
}

PyObject *netSendGuildChangeGradeNamePacket(PyObject *poSelf, PyObject *poArgs)
{
	int iGradeNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iGradeNumber))
		return Py_BuildException();
	char *szGradeName;
	if (!PyTuple_GetString(poArgs, 1, &szGradeName))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildChangeGradeNamePacket(iGradeNumber, szGradeName);

	return Py_BuildNone();
}

PyObject *netSendGuildChangeGradeAuthorityPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iGradeNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iGradeNumber))
		return Py_BuildException();
	int iAuthority;
	if (!PyTuple_GetInteger(poArgs, 1, &iAuthority))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildChangeGradeAuthorityPacket(iGradeNumber, iAuthority);

	return Py_BuildNone();
}

PyObject *netSendGuildOfferPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iExperience;
	if (!PyTuple_GetInteger(poArgs, 0, &iExperience))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildOfferPacket(iExperience);

	return Py_BuildNone();
}

PyObject *netSnedGuildPostCommentPacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szComment;
	if (!PyTuple_GetString(poArgs, 0, &szComment))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildPostCommentPacket(szComment);

	return Py_BuildNone();
}

PyObject *netSnedGuildDeleteCommentPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildDeleteCommentPacket(iIndex);

	return Py_BuildNone();
}

PyObject *netSendGuildRefreshCommentsPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iHightestIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iHightestIndex))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildRefreshCommentsPacket(iHightestIndex);

	return Py_BuildNone();
}

PyObject *netSendGuildChangeMemberGradePacket(PyObject *poSelf, PyObject *poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();
	int iGradeNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iGradeNumber))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildChangeMemberGradePacket(iPID, iGradeNumber);

	return Py_BuildNone();
}

PyObject *netSendGuildUseSkillPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iSkillID;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillID))
		return Py_BuildException();
	int iTargetVID;
	if (!PyTuple_GetInteger(poArgs, 1, &iTargetVID))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildUseSkillPacket(iSkillID, iTargetVID);

	return Py_BuildNone();
}

PyObject *netSendGuildChangeMemberGeneralPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
		return Py_BuildException();
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildChangeMemberGeneralPacket(iPID, iFlag);

	return Py_BuildNone();
}

PyObject *netSendGuildInviteAnswerPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iGuildID;
	if (!PyTuple_GetInteger(poArgs, 0, &iGuildID))
		return Py_BuildException();
	int iAnswer;
	if (!PyTuple_GetInteger(poArgs, 1, &iAnswer))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildInviteAnswerPacket(iGuildID, iAnswer);

	return Py_BuildNone();
}

PyObject *netSendGuildChargeGSPPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iGSP;
	if (!PyTuple_GetInteger(poArgs, 0, &iGSP))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildChargeGSPPacket(iGSP);

	return Py_BuildNone();
}

PyObject *netSendGuildDepositMoneyPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iGSP;
	if (!PyTuple_GetInteger(poArgs, 0, &iGSP))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildDepositMoneyPacket(iGSP);

	return Py_BuildNone();
}

PyObject *netSendGuildWithdrawMoneyPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iGSP;
	if (!PyTuple_GetInteger(poArgs, 0, &iGSP))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendGuildWithdrawMoneyPacket(iGSP);

	return Py_BuildNone();
}

PyObject *netSendRequestRefineInfoPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();

//	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
//	rns.SendRequestRefineInfoPacket(iSlotIndex);
	assert(!"netSendRequestRefineInfoPacket - 더이상 사용하지 않는 함수 입니다");

	return Py_BuildNone();
}

PyObject *netSendRefinePacket(PyObject *poSelf, PyObject *poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BuildException();
	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendRefinePacket(iSlotIndex, iType);

	return Py_BuildNone();
}

PyObject *netSendSelectItemPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iItemPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemPos))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendSelectItemPacket(iItemPos);

	return Py_BuildNone();
}

PyObject *netSetPacketSequenceMode(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rns.SetPacketSequenceMode(true);
	rkAccountConnector.SetPacketSequenceMode(true);

	return Py_BuildNone();
}

PyObject *netSetEmpireLanguageMode(PyObject *poSelf, PyObject *poArgs)
{
	int iMode;
	if (!PyTuple_GetInteger(poArgs, 0, &iMode))
		return Py_BuildException();

	gs_bEmpireLanuageEnable = iMode;

	return Py_BuildNone();
}

PyObject *netSetSkillGroupFake(PyObject *poSelf, PyObject *poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.__TEST_SetSkillGroupFake(iIndex);

	return Py_BuildNone();
}

PyObject *netRegisterErrorLog(PyObject *poSelf, PyObject *poArgs)
{
	char *szLog;
	if (!PyTuple_GetString(poArgs, 0, &szLog))
		return Py_BuildException();

	return Py_BuildNone();
}

#ifdef ENABLE_STYLE_ATTRIBUTE_SYSTEM
PyObject* netSendItemNewAttributePacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos source_pos;
	TItemPos target_pos;
	BYTE bValues[4 + 1];

	if (!PyTuple_GetInteger(poArgs, 0, &source_pos.cell))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 1, &target_pos.cell))
		return Py_BuildException();
	if (!PyTuple_GetByte(poArgs, 2, &bValues[0]))
		return Py_BuildException();
	if (!PyTuple_GetByte(poArgs, 3, &bValues[1]))
		return Py_BuildException();
	if (!PyTuple_GetByte(poArgs, 4, &bValues[2]))
		return Py_BuildException();
	if (!PyTuple_GetByte(poArgs, 5, &bValues[3]))
		return Py_BuildException();
	if (!PyTuple_GetByte(poArgs, 6, &bValues[4]))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemNewAttributePacket(source_pos, target_pos, bValues);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_DESTROY_DIALOG
PyObject *netSendItemDestroyPacket(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos Cell;
	int count;

	switch (PyTuple_Size(poArgs))
	{
		case 2:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &count))
				return Py_BuildException();
			break;

		case 3:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 2, &count))
				return Py_BuildException();
			break;

		default:
			return Py_BuildException();
	}
	
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemDestroyPacket(Cell, 0, count);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_QUICK_SELL_ITEM
PyObject *netSendItemSellPacket(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos Cell;

	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BuildException();
			break;
		case 2:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BuildException();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BuildException();
			break;
		default:
			return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemSellPacket(Cell, 0);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
PyObject *netSendChangeLanguagePacket(PyObject *poSelf, PyObject *poArgs)
{
	int iLanguage;
	if (!PyTuple_GetInteger(poArgs, 0, &iLanguage))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendChangeLanguagePacket(iLanguage);
	return Py_BuildNone();
}

PyObject *netGetLanguage(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetLanguage());
}

PyObject *netSetLanguage(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bLanguage;
	if (!PyTuple_GetInteger(poArgs, 0, &bLanguage))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SetLanguage(bLanguage);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
PyObject *netSendWhisperDetails(PyObject *poSelf, PyObject *poArgs)
{
	char *name;
	if (!PyTuple_GetString(poArgs, 0, &name))
		return Py_BuildException();

	CPythonNetworkStream& net = CPythonNetworkStream::Instance();
	net.SendWhisperDetails(name);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_TELEPORT_TO_A_FRIEND
PyObject *netSendMessengerRequestWarpByNamePacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendMessengerRequestWarpByNamePacket(szName);

	return Py_BuildNone();
}

PyObject *netSendMessengerSummonByNamePacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	rns.SendMessengerSummonByNamePacket(szName);

	return Py_BuildNone();
}
#endif


#ifdef ENABLE_MOB_DROP_INFO
PyObject *netTargetInfoLoad(PyObject *poSelf, PyObject *poArgs)
{
	DWORD dwVID;

	if (!PyArg_ParseTuple(poArgs, "i", &dwVID))
	{
		return Py_BuildException();
	}
	if (dwVID < 0)
	{
		return Py_BuildNone();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendTargetInfoLoadPacket(dwVID);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_VIEW_CHEST_DROP
PyObject *netSendChestDropInfo(PyObject *poSelf, PyObject *poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
		return Py_BadArgument();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendChestDropInfo(iSlotIndex);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_EVENT_MANAGER
PyObject *netSendRequestEventQuest(PyObject *poSelf, PyObject *poArgs)
{
	char *szString;
	if (!PyTuple_GetString(poArgs, 0, &szString))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendRequestEventQuest(szString);

	return Py_BuildNone();
}

PyObject *netSendRequestEventData(PyObject *poSelf, PyObject *poArgs)
{
	int iMonth;
	if (!PyTuple_GetInteger(poArgs, 0, &iMonth))
		return Py_BuildException();

	if ((iMonth >= 0) && (iMonth <= InGameEventManager::MONTH_MAX_NUM))
	{
		CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
		rns.SendRequestEventData(iMonth);
	}

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
PyObject *netSendExtBattlePassAction(PyObject *poSelf, PyObject *poArgs)
{
	int iAction = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &iAction))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendExtBattlePassAction(iAction);
	return Py_BuildNone();
}

PyObject *netSendExtBattlePassPremiumItem(PyObject *poSelf, PyObject *poArgs)
{
	int slotindex = 0;
	if (!PyTuple_GetInteger(poArgs, 0, &slotindex))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendExtBattlePassPremiumItem(slotindex);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
PyObject *netSendOfflineShopEndPacket(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendOfflineShopEndPacket();
	return Py_BuildNone();
}

PyObject *netSendOfflineShopBuyPacket(PyObject *poSelf, PyObject *poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();
	int pos;
	if (!PyTuple_GetInteger(poArgs, 1, &pos))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendOfflineShopBuyPacket((DWORD)vid, pos);
	return Py_BuildNone();
}

PyObject *netSendAddOfflineShopItem(PyObject *poSelf, PyObject *poArgs)
{
	int bDisplayPos;
	int bPos;
	long long iPrice;
	BYTE bWindowType;

	if (!PyTuple_GetInteger(poArgs, 0, &bDisplayPos))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 1, &bPos))
		return Py_BuildException();
	if (!PyTuple_GetLongLong(poArgs, 2, &iPrice))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 3, &bWindowType))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendAddOfflineShopItem(TItemPos(bWindowType, bDisplayPos), bPos, iPrice);
	return Py_BuildNone();
}

PyObject *netSendRemoveOfflineShopItem(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetInteger(poArgs, 0, &bPos))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendRemoveOfflineShopItem(bPos);
	return Py_BuildNone();
}

PyObject *netSendDestroyOfflineShop(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendDestroyOfflineShop();
	return Py_BuildNone();
}

PyObject *netSendTakeOfflineMoney(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendTakeOfflineMoney();
	return Py_BuildNone();
}

PyObject *netSendOfflineShopChangeTitle(PyObject *poSelf, PyObject *poArgs)
{
	char *title;
	if (!PyTuple_GetString(poArgs, 0, &title))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendOfflineShopChangeTitle(title);
	return Py_BuildNone();
}

PyObject *netSendOfflineShopCheck(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendOfflineShopCheck();
	return Py_BuildNone();
}

PyObject *netSendOfflineShopButton(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendOfflineShopButton();
	return Py_BuildNone();
}

PyObject *netSendOfflineShopRemoveLogs(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendOfflineShopRemoveLogs();
	return Py_BuildNone();
}

PyObject *netSendOpenShopSlot(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bPos;
	if (!PyTuple_GetInteger(poArgs, 0, &bPos))
		return Py_BuildException();
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendOpenShopSlot(bPos);
	return Py_BuildNone();
}

PyObject *netSendGetBackItems(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendGetBackItems();
	return Py_BuildNone();
}

PyObject *netSendAddTime(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendAddTime();
	return Py_BuildNone();
}

PyObject *netSendOpenWithVID(PyObject *poSelf, PyObject *poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
		return Py_BuildException();
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::instance();
	rkNetStream.SendOfflineOpenWithVID(vid);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
PyObject *netSendPrivateShopSearchInfo(PyObject *poSelf, PyObject *poArgs)
{
	TPacketCGShopSearch p;
	p.header = HEADER_CG_SHOP_SEARCH;
	if (!PyTuple_GetInteger(poArgs, 0, &p.ItemCat))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 1, &p.SubCat))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 2, &p.avg[0]))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 3, &p.avg[1]))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 4, &p.skill[0]))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 5, &p.skill[1]))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 6, &p.abs[0]))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 7, &p.abs[1]))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 8, &p.level[0]))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 9, &p.level[1]))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 10, &p.refine[0]))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 11, &p.refine[1]))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 12, &p.enchant))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 13, &p.alchemy))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 14, &p.sex))
		return Py_BuildException();
	if (!PyTuple_GetInteger(poArgs, 15, &p.character))
		return Py_BuildException();
	long long minYang = 0, maxYang = 0;
	if (!PyTuple_GetLongLong(poArgs, 16, &minYang))
		return Py_BuildException();
	if (!PyTuple_GetLongLong(poArgs, 17, &maxYang))
		return Py_BuildException();
	char *searchInputConst;
	if (!PyTuple_GetString(poArgs, 18, &searchInputConst))
		return Py_BuildException();
	strcpy(p.searchInput, searchInputConst);
	p.price[0] = minYang;
	p.price[1] = maxYang;
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendPrivateShopSearchInfo(&p);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_RENEWAL_OX_EVENT
PyObject *netSendQuestInputStringLongPacket(PyObject *poSelf, PyObject *poArgs)
{
	char *szString;
	if (!PyTuple_GetString(poArgs, 0, &szString))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendQuestInputStringLongPacket(szString);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_CLIENT_PERFORMANCE
PyObject* netLoadResourcesInCache(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayerSettingsModule::Instance().startLoadThread();
	return Py_BuildNone();
}

PyObject* netRegisterSkills(PyObject* poSelf, PyObject* poArgs)
{
	int32_t race;
	if (!PyTuple_GetInteger(poArgs, 0, &race))
		return Py_BuildException();

	int32_t skill_group;
	if (!PyTuple_GetInteger(poArgs, 1, &skill_group))
		return Py_BuildException();

	int32_t empire;
	if (!PyTuple_GetInteger(poArgs, 2, &empire))
		return Py_BuildException();

	CPythonPlayerSettingsModule::Instance().RegisterSkills(race, skill_group, empire);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
PyObject *netSendAuraRefineCheckIn(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos InventoryPos;
	TItemPos AuraPos;
	BYTE bAuraRefineWindowType;

	switch (PyTuple_Size(poArgs))
	{
		case 3:
			if (!PyTuple_GetInteger(poArgs, 0, &InventoryPos.cell))
				return Py_BuildException();

			AuraPos.window_type = AURA_REFINE;

			if (!PyTuple_GetInteger(poArgs, 1, &AuraPos.cell))
				return Py_BuildException();

			if (!PyTuple_GetByte(poArgs, 2, &bAuraRefineWindowType))
				return Py_BuildException();
			break;

		case 5:
			if (!PyTuple_GetByte(poArgs, 0, &InventoryPos.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.cell))
				return Py_BuildException();

			if (!PyTuple_GetByte(poArgs, 2, &AuraPos.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 3, &AuraPos.cell))
				return Py_BuildException();

			if (!PyTuple_GetByte(poArgs, 4, &bAuraRefineWindowType))
				return Py_BuildException();
			break;

		default:
			return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendAuraRefineCheckIn(InventoryPos, AuraPos, bAuraRefineWindowType);

	return Py_BuildNone();
}

PyObject *netSendAuraRefineCheckOut(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos AuraPos;
	BYTE byAuraRefineWindowType;

	switch (PyTuple_Size(poArgs))
	{
		case 2:
			AuraPos.window_type = AURA_REFINE;

			if (!PyTuple_GetInteger(poArgs, 0, &AuraPos.cell))
				return Py_BuildException();

			if (!PyTuple_GetByte(poArgs, 1, &byAuraRefineWindowType))
				return Py_BuildException();
			break;

		case 3:
			if (!PyTuple_GetByte(poArgs, 0, &AuraPos.window_type))
				return Py_BuildException();

			if (!PyTuple_GetInteger(poArgs, 1, &AuraPos.cell))
				return Py_BuildException();

			if (!PyTuple_GetByte(poArgs, 2, &byAuraRefineWindowType))
				return Py_BuildException();
			break;

		default:
			return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendAuraRefineCheckOut(AuraPos, byAuraRefineWindowType);

	return Py_BuildNone();
}

PyObject *netSendAuraRefineAccept(PyObject *poSelf, PyObject *poArgs)
{
	BYTE byAuraRefineWindowType;

	if (!PyTuple_GetByte(poArgs, 0, &byAuraRefineWindowType))
		return Py_BuildException();

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendAuraRefineAccept(byAuraRefineWindowType);

	return Py_BuildNone();
}

PyObject *netSendAuraRefineCancel(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendAuraRefineCancel();

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
PyObject *netCheckUsePetItem(PyObject *poSelf, PyObject *poArgs)
{
	int petEggUse = CItemData::PET_EGG_USE_TRUE;

	return Py_BuildValue("i", petEggUse);
}
PyObject *netSendPetHatchingPacket(PyObject *poSelf, PyObject *poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	int iWindow;
	if (!PyTuple_GetInteger(poArgs, 1, &iWindow))
		return Py_BuildException();

	int iPos;
	if (!PyTuple_GetInteger(poArgs, 2, &iPos))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetHatchingPacket(szName, TItemPos(iWindow, iPos));

	return Py_BuildNone();
}
PyObject *netSendPetHatchingWindowPacket(PyObject *poSelf, PyObject *poArgs)
{
	bool bState;
	if (!PyTuple_GetBoolean(poArgs, 0, &bState))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetHatchingWindowPacket(bState);

	return Py_BuildNone();
}
PyObject *netSendPetFeedPacket(PyObject *poSelf, PyObject *poArgs)
{
	int iFeedIndex;
	PyObject *pyPosList;
	PyObject *pyCountList;
	std::vector<std::pair<WORD, WORD>> itemVec;

	if (!PyArg_ParseTuple(poArgs, "iOO", &iFeedIndex, &pyPosList, &pyCountList))
		return Py_BuildException();

	for (int i = 0; i < PyList_Size(pyPosList); ++i)
	{
		PyObject *pyItemPos = PyList_GetItem(pyPosList, i);
		WORD itemPos = static_cast<WORD>(PyInt_AsLong(pyItemPos));

		PyObject *pyItemCount = PyList_GetItem(pyCountList, i);
		WORD itemCount = static_cast<WORD>(PyInt_AsLong(pyItemCount));

		itemVec.push_back(std::make_pair(itemPos, itemCount));
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetFeedPacket(iFeedIndex, itemVec);

	return Py_BuildNone();
}
PyObject *netSendPetLearnSkill(PyObject *poSelf, PyObject *poArgs)
{
	int iSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlot))
		return Py_BuildException();

	TItemPos Cell;
	if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetLearnSkill(iSlot, Cell);

	return Py_BuildNone();
}
PyObject *netSendPetSkillUpgradeRequest(PyObject *poSelf, PyObject *poArgs)
{
	int iSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlot))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetSkillUpgradeRequest(iSlot);

	return Py_BuildNone();
}
PyObject *netSendPetDeleteSkill(PyObject *poSelf, PyObject *poArgs)
{
	int iSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlot))
		return Py_BuildException();

	TItemPos Cell;
	if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetDeleteSkill(iSlot, Cell);

	return Py_BuildNone();
}
PyObject *netSendPetDeleteAllSkill(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos Cell;
	if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetDeleteAllSkill(Cell);

	return Py_BuildNone();
}
PyObject *netSendPetNameChangePacket(PyObject *poSelf, PyObject *poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
		return Py_BuildException();

	TItemPos Cell;
	if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		return Py_BuildException();

	TItemPos Cell2;
	if (!PyTuple_GetInteger(poArgs, 2, &Cell2.cell))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetNameChangePacket(szName, Cell, Cell2);

	return Py_BuildNone();
}
PyObject *netSendPetNameChangeWindowPacket(PyObject *poSelf, PyObject *poArgs)
{
	bool bState;
	if (!PyTuple_GetBoolean(poArgs, 0, &bState))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetHatchingWindowPacket(bState);

	return Py_BuildNone();
}

PyObject *netSendPetWindowType(PyObject *poSelf, PyObject *poArgs)
{
	int bPetWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bPetWindowType))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetWindowType(bPetWindowType);

	return Py_BuildNone();
}

PyObject *netSendPetAttrDetermine(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos Cell;
	if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetAttrDetermine(Cell);

	return Py_BuildNone();
}

PyObject *netSendPetAttrChange(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos Cell;
	if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
		return Py_BuildException();

	TItemPos Cell2;
	if (!PyTuple_GetInteger(poArgs, 1, &Cell2.cell))
		return Py_BuildException();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetAttrChange(Cell, Cell2);

	return Py_BuildNone();
}

PyObject *netSendPetRevive(PyObject *poSelf, PyObject *poArgs)
{
	int iUpBringingPos;
	PyObject *pyPosList;
	PyObject *pyCountList;
	std::vector<std::pair<WORD, WORD>> itemVec;

	if (!PyArg_ParseTuple(poArgs, "iOO", &iUpBringingPos, &pyPosList, &pyCountList))
		return Py_BuildException();

	for (int i = 0; i < PyList_Size(pyPosList); ++i)
	{
		PyObject *pyItemPos = PyList_GetItem(pyPosList, i);
		WORD itemPos = static_cast<WORD>(PyInt_AsLong(pyItemPos));

		PyObject *pyItemCount = PyList_GetItem(pyCountList, i);
		WORD itemCount = static_cast<WORD>(PyInt_AsLong(pyItemCount));

		itemVec.push_back(std::make_pair(itemPos, itemCount));
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPetRevive(TItemPos(INVENTORY, iUpBringingPos), itemVec);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
PyObject *netSendChangeLookCheckIn(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos Cell;
	if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		return Py_BuildException();

	BYTE bChangeLookSlotType;
	if (!PyTuple_GetByte(poArgs, 2, &bChangeLookSlotType))
		return Py_BuildException();

	if (bChangeLookSlotType >= static_cast<BYTE>(ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_MAX))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendChangeLookCheckInPacket(Cell, bChangeLookSlotType);

	return Py_BuildNone();
}

PyObject *netSendChangeLookCheckOut(PyObject *poSelf, PyObject *poArgs)
{
	BYTE bChangeLookSlotType;
	if (!PyTuple_GetByte(poArgs, 0, &bChangeLookSlotType))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendChangeLookCheckOutPacket(bChangeLookSlotType);

	return Py_BuildNone();
}

PyObject *netSendChangeLookCheckInFreeYangItem(PyObject *poSelf, PyObject *poArgs)
{
	TItemPos Cell;
	if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendChangeLookCheckInFreeYangItemPacket(Cell);

	return Py_BuildNone();
}

PyObject *netSendChangeLookCheckOutFreeYangItem(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream::Instance().SendChangeLookCheckOutFreeYangItemPacket();

	return Py_BuildNone();
}

PyObject *netSendChangeLookAccept(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream::Instance().SendChangeLookAcceptPacket();

	return Py_BuildNone();
}

PyObject *netSendChangeLookCancel(PyObject *poSelf, PyObject *poArgs)
{
	CPythonNetworkStream::Instance().SendChangeLookCancelPacket();

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_HUNTING_SYSTEM
PyObject *netSendHuntingAction(PyObject *poSelf, PyObject *poArgs)
{
	int bAction;
	if (!PyTuple_GetInteger(poArgs, 0, &bAction))
		return Py_BuildException();

	int dValue;
	if (!PyTuple_GetInteger(poArgs, 1, &dValue))
		return Py_BuildException();

	CPythonNetworkStream::Instance().SendHuntingAction(bAction, dValue);
	return Py_BuildNone();
}
#endif

void initnet()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetBettingGuildWarValue", netGetBettingGuildWarValue, METH_VARARGS },
		{ "EnableChatInsultFilter", netEnableChatInsultFilter, METH_VARARGS },
		{ "SetServerInfo", netSetServerInfo, METH_VARARGS },
		{ "GetServerInfo", netGetServerInfo, METH_VARARGS },
		{ "PreserveServerCommand", netPreserveServerCommand, METH_VARARGS },
		{ "GetPreservedServerCommand", netGetPreservedServerCommand, METH_VARARGS },

		{ "StartGame", netStartGame, METH_VARARGS },
		{ "Warp", netWarp, METH_VARARGS },
		{ "SetMarkServer", netSetMarkServer, METH_VARARGS },
		{ "IsChatInsultIn", netIsChatInsultIn, METH_VARARGS },
		{ "IsInsultIn", netIsInsultIn, METH_VARARGS },
		{ "LoadInsultList", netLoadInsultList, METH_VARARGS },
		{ "UploadMark", netUploadMark, METH_VARARGS },
		{ "UploadSymbol", netUploadSymbol, METH_VARARGS },
		{ "GetGuildID", netGetGuildID, METH_VARARGS },
		{ "GetEmpireID", netGetEmpireID, METH_VARARGS },
		{ "GetMainActorVID", netGetMainActorVID, METH_VARARGS },
		{ "GetMainActorRace", netGetMainActorRace, METH_VARARGS },
		{ "GetMainActorEmpire", netGetMainActorEmpire, METH_VARARGS },
		{ "GetMainActorSkillGroup", netGetMainActorSkillGroup, METH_VARARGS },
		{ "GetAccountCharacterSlotDataInteger", netGetAccountCharacterSlotDataInteger, METH_VARARGS },
		{ "GetAccountCharacterSlotDataString", netGetAccountCharacterSlotDataString, METH_VARARGS },

		{ "GetFieldMusicFileName", netGetFieldMusicFileName, METH_VARARGS },
		{ "GetFieldMusicVolume", netGetFieldMusicVolume, METH_VARARGS },

		{ "ToggleGameDebugInfo", netToggleGameDebugInfo, METH_VARARGS },
		{ "SetLoginInfo", netSetLoginInfo, METH_VARARGS },
		{ "SetPhaseWindow", netSetPhaseWindow, METH_VARARGS },
		{ "ClearPhaseWindow", netClearPhaseWindow, METH_VARARGS },
		{ "SetServerCommandParserWindow", netSetServerCommandParserWindow, METH_VARARGS },
		{ "SetAccountConnectorHandler", netSetAccountConnectorHandler, METH_VARARGS },
		{ "SetHandler", netSetHandler, METH_VARARGS },
		{ "SetTCPRecvBufferSize", netSetTCPRecvBufferSize, METH_VARARGS },
		{ "SetTCPSendBufferSize", netSetTCPSendBufferSize, METH_VARARGS },
		{ "SetUDPRecvBufferSize", netSetUDPRecvBufferSize, METH_VARARGS },
		{ "DirectEnter", netDirectEnter, METH_VARARGS },

		{ "LogOutGame", netLogOutGame, METH_VARARGS },
		{ "ExitGame", netExitGame, METH_VARARGS },
		{ "ExitApplication", netExitApplication, METH_VARARGS },
		{ "ConnectTCP", netConnectTCP, METH_VARARGS },
		{ "ConnectUDP", netConnectUDP, METH_VARARGS },
		{ "ConnectToAccountServer", netConnectToAccountServer, METH_VARARGS },

		{ "SendLoginPacket", netSendLoginPacket, METH_VARARGS },
		{ "SendSelectEmpirePacket", netSendSelectEmpirePacket, METH_VARARGS },
		{ "SendSelectCharacterPacket", netSendSelectCharacterPacket, METH_VARARGS },
		{ "SendChangeNamePacket", netSendChangeNamePacket, METH_VARARGS },
		{ "SendCreateCharacterPacket", netSendCreateCharacterPacket, METH_VARARGS },
		{ "SendDestroyCharacterPacket", netSendDestroyCharacterPacket, METH_VARARGS },
		{ "SendEnterGamePacket", netSendEnterGamePacket, METH_VARARGS },

		{ "SendItemUsePacket", netSendItemUsePacket, METH_VARARGS },
		{ "SendItemUseToItemPacket", netSendItemUseToItemPacket, METH_VARARGS },
		{ "SendItemDropPacket", netSendItemDropPacket, METH_VARARGS },
		{ "SendItemDropPacketNew", netSendItemDropPacketNew, METH_VARARGS },
		{ "SendElkDropPacket", netSendElkDropPacket, METH_VARARGS },
		{ "SendGoldDropPacketNew", netSendGoldDropPacketNew, METH_VARARGS },
		{ "SendItemMovePacket", netSendItemMovePacket, METH_VARARGS },
		{ "SendItemPickUpPacket", netSendItemPickUpPacket, METH_VARARGS },
		{ "SendGiveItemPacket", netSendGiveItemPacket, METH_VARARGS },

		{ "SetOfflinePhase", netSetOfflinePhase, METH_VARARGS },
		{ "Disconnect", netDisconnect, METH_VARARGS },
		{ "IsConnect", netIsConnect, METH_VARARGS },

		{ "SendChatPacket", netSendChatPacket, METH_VARARGS },
		{ "SendEmoticon", netSendEmoticon, METH_VARARGS },
		{ "SendWhisperPacket", netSendWhisperPacket, METH_VARARGS },

		{ "SendCharacterPositionPacket", netSendCharacterPositionPacket, METH_VARARGS },

		{ "SendShopEndPacket", netSendShopEndPacket, METH_VARARGS },
		{ "SendShopBuyPacket", netSendShopBuyPacket, METH_VARARGS },
		{ "SendShopSellPacket", netSendShopSellPacket, METH_VARARGS },
		{ "SendShopSellPacketNew", netSendShopSellPacketNew, METH_VARARGS },

		{ "SendExchangeStartPacket", netSendExchangeStartPacket, METH_VARARGS },
		{ "SendExchangeItemAddPacket", netSendExchangeItemAddPacket, METH_VARARGS },
		{ "SendExchangeItemDelPacket", netSendExchangeItemDelPacket, METH_VARARGS },
		{ "SendExchangeElkAddPacket", netSendExchangeElkAddPacket, METH_VARARGS },
		{ "SendExchangeAcceptPacket", netSendExchangeAcceptPacket, METH_VARARGS },
		{ "SendExchangeExitPacket", netSendExchangeExitPacket, METH_VARARGS },

		{ "SendOnClickPacket", netOnClickPacket, METH_VARARGS },

		// Emoticon String
		{ "RegisterEmoticonString", netRegisterEmoticonString, METH_VARARGS },

		// Messenger
		{ "SendMessengerAddByVIDPacket", netSendMessengerAddByVIDPacket, METH_VARARGS },
		{ "SendMessengerAddByNamePacket", netSendMessengerAddByNamePacket, METH_VARARGS },
		{ "SendMessengerRemovePacket", netSendMessengerRemovePacket, METH_VARARGS },

		//Block
#ifdef ENABLE_MESSENGER_BLOCK
		{ "SendMessengerBlockAddByVIDPacket", netSendMessengerBlockAddByVIDPacket, METH_VARARGS },
		{ "SendMessengerBlockRemovePacket", netSendMessengerBlockRemovePacket, METH_VARARGS },
		{ "SendMessengerBlockAddByNamePacket", netSendMessengerBlockAddByNamePacket, METH_VARARGS },
		{ "SendMessengerBlockRemoveByVIDPacket", netSendMessengerBlockRemoveByVIDPacket, METH_VARARGS },
#endif

		// Party
		{ "SendPartyInvitePacket", netSendPartyInvitePacket, METH_VARARGS },
		{ "SendPartyInviteAnswerPacket", netSendPartyInviteAnswerPacket, METH_VARARGS },
		{ "SendPartyExitPacket", netSendPartyExitPacket, METH_VARARGS },
		{ "SendPartyRemovePacket", netSendPartyRemovePacket, METH_VARARGS },
		{ "SendPartySetStatePacket", netSendPartySetStatePacket, METH_VARARGS },
		{ "SendPartyUseSkillPacket", netSendPartyUseSkillPacket, METH_VARARGS },
		{ "SendPartyParameterPacket", netSendPartyParameterPacket, METH_VARARGS },

		// Safebox
		{ "SendSafeboxSaveMoneyPacket", netSendSafeboxSaveMoneyPacket, METH_VARARGS },
		{ "SendSafeboxWithdrawMoneyPacket", netSendSafeboxWithdrawMoneyPacket, METH_VARARGS },
		{ "SendSafeboxCheckinPacket", netSendSafeboxCheckinPacket, METH_VARARGS },
		{ "SendSafeboxCheckoutPacket", netSendSafeboxCheckoutPacket, METH_VARARGS },
		{ "SendSafeboxItemMovePacket", netSendSafeboxItemMovePacket, METH_VARARGS },

		// Mall
		{ "SendMallCheckoutPacket", netSendMallCheckoutPacket, METH_VARARGS },

		// Guild
		{ "SendQuestInputStringPacket", netSendQuestInputStringPacket, METH_VARARGS },
		{ "SendQuestConfirmPacket", netSendQuestConfirmPacket, METH_VARARGS },
		{ "SendGuildAddMemberPacket", netSendGuildAddMemberPacket, METH_VARARGS },
		{ "SendGuildRemoveMemberPacket", netSendGuildRemoveMemberPacket, METH_VARARGS },
		{ "SendGuildChangeGradeNamePacket", netSendGuildChangeGradeNamePacket, METH_VARARGS },
		{ "SendGuildChangeGradeAuthorityPacket", netSendGuildChangeGradeAuthorityPacket, METH_VARARGS },
		{ "SendGuildOfferPacket", netSendGuildOfferPacket, METH_VARARGS },
		{ "SendGuildPostCommentPacket", netSnedGuildPostCommentPacket, METH_VARARGS },
		{ "SendGuildDeleteCommentPacket", netSnedGuildDeleteCommentPacket, METH_VARARGS },
		{ "SendGuildRefreshCommentsPacket", netSendGuildRefreshCommentsPacket, METH_VARARGS },
		{ "SendGuildChangeMemberGradePacket", netSendGuildChangeMemberGradePacket, METH_VARARGS },
		{ "SendGuildUseSkillPacket", netSendGuildUseSkillPacket, METH_VARARGS },
		{ "SendGuildChangeMemberGeneralPacket", netSendGuildChangeMemberGeneralPacket, METH_VARARGS },
		{ "SendGuildInviteAnswerPacket", netSendGuildInviteAnswerPacket, METH_VARARGS },
		{ "SendGuildChargeGSPPacket", netSendGuildChargeGSPPacket, METH_VARARGS },
		{ "SendGuildDepositMoneyPacket", netSendGuildDepositMoneyPacket, METH_VARARGS },
		{ "SendGuildWithdrawMoneyPacket", netSendGuildWithdrawMoneyPacket, METH_VARARGS },

		// Refine
		{ "SendRequestRefineInfoPacket", netSendRequestRefineInfoPacket, METH_VARARGS },
		{ "SendRefinePacket", netSendRefinePacket, METH_VARARGS },
		{ "SendSelectItemPacket", netSendSelectItemPacket, METH_VARARGS },

		// SYSTEM
		{ "SetPacketSequenceMode", netSetPacketSequenceMode, METH_VARARGS },
		{ "SetEmpireLanguageMode", netSetEmpireLanguageMode, METH_VARARGS },

		// For Test
		{ "SetSkillGroupFake", netSetSkillGroupFake, METH_VARARGS },

		// Log
		{ "RegisterErrorLog", netRegisterErrorLog, METH_VARARGS },

#ifdef ENABLE_STYLE_ATTRIBUTE_SYSTEM
		{"SendItemNewAttributePacket", netSendItemNewAttributePacket, METH_VARARGS },
#endif

#ifdef ENABLE_DESTROY_DIALOG
		{ "SendItemDestroyPacket", netSendItemDestroyPacket, METH_VARARGS },
#endif

#ifdef ENABLE_QUICK_SELL_ITEM
		{ "SendItemSellPacket", netSendItemSellPacket, METH_VARARGS },
#endif

#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		{ "SendChangeLanguagePacket", netSendChangeLanguagePacket, METH_VARARGS },
		{ "SetLanguage", netSetLanguage, METH_VARARGS },
		{ "GetLanguage", netGetLanguage, METH_VARARGS },
#endif

#ifdef ENABLE_EXTENDED_WHISPER_DETAILS
		{ "SendWhisperDetails", netSendWhisperDetails, METH_VARARGS },
#endif

#ifdef ENABLE_TELEPORT_TO_A_FRIEND
		{ "SendMessengerRequestWarpByNamePacket", netSendMessengerRequestWarpByNamePacket, METH_VARARGS },
		{ "SendMessengerSummonByNamePacket", netSendMessengerSummonByNamePacket, METH_VARARGS },
#endif

#ifdef ENABLE_MOB_DROP_INFO
		{ "SendTargetInfoLoad", netTargetInfoLoad, METH_VARARGS },
#endif

#ifdef ENABLE_VIEW_CHEST_DROP
		{ "SendChestDropInfo", netSendChestDropInfo, METH_VARARGS },
#endif

#ifdef ENABLE_EVENT_MANAGER
		{ "SendRequestEventQuest", netSendRequestEventQuest, METH_VARARGS },
		{ "SendRequestEventData", netSendRequestEventData, METH_VARARGS },
#endif

#ifdef ENABLE_RENEWAL_BATTLE_PASS
		{ "SendExtBattlePassAction", netSendExtBattlePassAction, METH_VARARGS },
		{ "SendExtBattlePassPremiumItem", netSendExtBattlePassPremiumItem, METH_VARARGS },
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
		{ "SendOfflineShopEndPacket", netSendOfflineShopEndPacket, METH_VARARGS },
		{ "SendOfflineShopBuyPacket", netSendOfflineShopBuyPacket, METH_VARARGS },
		{ "SendAddOfflineShopItem", netSendAddOfflineShopItem, METH_VARARGS },
		{ "SendRemoveOfflineShopItem", netSendRemoveOfflineShopItem, METH_VARARGS },
		{ "SendDestroyOfflineShop", netSendDestroyOfflineShop, METH_VARARGS },
		{ "SendTakeOfflineMoney", netSendTakeOfflineMoney, METH_VARARGS },
		{ "SendOfflineShopChangeTitle", netSendOfflineShopChangeTitle, METH_VARARGS },
		{ "SendOfflineShopCheck", netSendOfflineShopCheck, METH_VARARGS },
		{ "SendOfflineShopButton", netSendOfflineShopButton, METH_VARARGS },
		{ "SendOfflineShopRemoveLogs", netSendOfflineShopRemoveLogs, METH_VARARGS },
		{ "SendOpenShopSlot", netSendOpenShopSlot, METH_VARARGS },
		{ "SendGetBackItems", netSendGetBackItems, METH_VARARGS },
		{ "SendAddTime", netSendAddTime, METH_VARARGS },
		{ "SendOpenWithVID", netSendOpenWithVID, METH_VARARGS },
#endif

#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
		{ "SendPrivateShopSearchInfo", netSendPrivateShopSearchInfo, METH_VARARGS },
#endif

#ifdef ENABLE_RENEWAL_OX_EVENT
		{ "SendQuestInputStringLongPacket", netSendQuestInputStringLongPacket, METH_VARARGS },
#endif

#ifdef ENABLE_CLIENT_PERFORMANCE
		{ "LoadResourcesInCache", netLoadResourcesInCache, METH_VARARGS },
		{ "RegisterSkills", netRegisterSkills, METH_VARARGS },
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
		{ "SendAuraRefineCheckIn", netSendAuraRefineCheckIn, METH_VARARGS },
		{ "SendAuraRefineCheckOut", netSendAuraRefineCheckOut, METH_VARARGS },
		{ "SendAuraRefineAccept", netSendAuraRefineAccept, METH_VARARGS },
		{ "SendAuraRefineCancel", netSendAuraRefineCancel, METH_VARARGS },
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
		{ "CheckUsePetItem", netCheckUsePetItem, METH_VARARGS },
		{ "SendPetFeedPacket", netSendPetFeedPacket, METH_VARARGS },
		{ "SendPetHatchingPacket", netSendPetHatchingPacket, METH_VARARGS },
		{ "SendPetHatchingWindowPacket", netSendPetHatchingWindowPacket, METH_VARARGS },
		{ "SendPetNameChangePacket", netSendPetNameChangePacket, METH_VARARGS },
		{ "SendPetNameChangeWindowPacket", netSendPetNameChangeWindowPacket, METH_VARARGS },
		{ "SendPetWindowType", netSendPetWindowType, METH_VARARGS },

		{ "SendPetLearnSkill", netSendPetLearnSkill, METH_VARARGS },
		{ "SendPetSkillUpgradeRequest", netSendPetSkillUpgradeRequest, METH_VARARGS },
		{ "SendPetDeleteSkill", netSendPetDeleteSkill, METH_VARARGS },
		{ "SendPetDeleteAllSkill", netSendPetDeleteAllSkill, METH_VARARGS },

		{ "SendPetAttrDetermine", netSendPetAttrDetermine, METH_VARARGS },
		{ "SendPetAttrChange", netSendPetAttrChange, METH_VARARGS },
		{ "SendPetRevive", netSendPetRevive, METH_VARARGS },
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		{ "SendChangeLookCheckIn", netSendChangeLookCheckIn, METH_VARARGS },
		{ "SendChangeLookCheckOut", netSendChangeLookCheckOut, METH_VARARGS },
		{ "SendChangeLookCheckInFreeYangItem", netSendChangeLookCheckInFreeYangItem, METH_VARARGS },
		{ "SendChangeLookCheckOutFreeYangItem", netSendChangeLookCheckOutFreeYangItem, METH_VARARGS },
		{ "SendChangeLookCheckOut", netSendChangeLookCheckOut, METH_VARARGS },
		{ "SendChangeLookAccept", netSendChangeLookAccept, METH_VARARGS },
		{ "SendChangeLookCanCle", netSendChangeLookCancel, METH_VARARGS },
#endif

#ifdef ENABLE_HUNTING_SYSTEM
		{ "SendHuntingAction", netSendHuntingAction, METH_VARARGS },
#endif

		{ nullptr, nullptr },
	};

#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
	PyObject *poModule = Py_InitModule(GetModuleName(NET_MODULE).c_str(), s_methods);
#else
	PyObject *poModule = Py_InitModule("net", s_methods);
#endif

	PyModule_AddIntConstant(poModule, "ERROR_NONE", CPythonNetworkStream::ERROR_NONE);
	PyModule_AddIntConstant(poModule, "ERROR_CONNECT_MARK_SERVER", CPythonNetworkStream::ERROR_CONNECT_MARK_SERVER);
	PyModule_AddIntConstant(poModule, "ERROR_LOAD_MARK", CPythonNetworkStream::ERROR_LOAD_MARK);
	PyModule_AddIntConstant(poModule, "ERROR_MARK_WIDTH", CPythonNetworkStream::ERROR_MARK_WIDTH);
	PyModule_AddIntConstant(poModule, "ERROR_MARK_HEIGHT", CPythonNetworkStream::ERROR_MARK_HEIGHT);

	PyModule_AddIntConstant(poModule, "ERROR_MARK_UPLOAD_NEED_RECONNECT", CPythonNetworkStream::ERROR_MARK_UPLOAD_NEED_RECONNECT);	
	PyModule_AddIntConstant(poModule, "ERROR_MARK_CHECK_NEED_RECONNECT", CPythonNetworkStream::ERROR_MARK_CHECK_NEED_RECONNECT);	

	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_LOGIN", CPythonNetworkStream::PHASE_WINDOW_LOGIN);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_SELECT", CPythonNetworkStream::PHASE_WINDOW_SELECT);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_CREATE", CPythonNetworkStream::PHASE_WINDOW_CREATE);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_LOAD", CPythonNetworkStream::PHASE_WINDOW_LOAD);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_GAME", CPythonNetworkStream::PHASE_WINDOW_GAME);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_EMPIRE", CPythonNetworkStream::PHASE_WINDOW_EMPIRE);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_LOGO", CPythonNetworkStream::PHASE_WINDOW_LOGO);

	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_ID", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_ID);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_NAME", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_NAME);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_RACE", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_RACE);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_LEVEL", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_LEVEL);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_STR", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_STR);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_DEX", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_DEX);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_INT", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_INT);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_HTH", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_HTH);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_PLAYTIME", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_PLAYTIME);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_FORM", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_FORM);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_ADDR", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_ADDR);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_PORT", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_PORT);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_GUILD_ID", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_GUILD_ID);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_GUILD_NAME", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_GUILD_NAME);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_HAIR", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_HAIR);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_ACCE", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_ACCE);
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_AURA", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_AURA);
#endif
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_LAST_PLAYTIME", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_LAST_PLAYTIME);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_MAPINDEX", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_MAPINDEX);

	PyModule_AddIntConstant(poModule, "SERVER_COMMAND_LOG_OUT",	CPythonNetworkStream::SERVER_COMMAND_LOG_OUT);
	PyModule_AddIntConstant(poModule, "SERVER_COMMAND_RETURN_TO_SELECT_CHARACTER", CPythonNetworkStream::SERVER_COMMAND_RETURN_TO_SELECT_CHARACTER);
	PyModule_AddIntConstant(poModule, "SERVER_COMMAND_QUIT", CPythonNetworkStream::SERVER_COMMAND_QUIT);

	PyModule_AddIntConstant(poModule, "EMPIRE_A", 1);
	PyModule_AddIntConstant(poModule, "EMPIRE_B", 2);
	PyModule_AddIntConstant(poModule, "EMPIRE_C", 3);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL", DS_SUB_HEADER_REFINE_FAIL);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE", DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL", DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY", DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL", DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL", DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_SUCCEED", DS_SUB_HEADER_REFINE_SUCCEED);

#ifdef ENABLE_BIOLOG_SYSTEM
	PyModule_AddIntConstant(poModule, "BIOLOG_MANAGER_OPEN", CG_BIOLOG_MANAGER_OPEN);
	PyModule_AddIntConstant(poModule, "BIOLOG_MANAGER_SEND", CG_BIOLOG_MANAGER_SEND);
	PyModule_AddIntConstant(poModule, "BIOLOG_MANAGER_TIMER", CG_BIOLOG_MANAGER_TIMER);
#endif
}
