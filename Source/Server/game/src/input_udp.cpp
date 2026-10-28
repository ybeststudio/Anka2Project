#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "input.h"
#include "desc.h"
#include "desc_manager.h"
#include "item_manager.h"
#include "char_manager.h"
#include "protocol.h"

extern socket_t udp_socket;

#define HEADER_CG_STATE_CHECKER	1

#pragma pack(1)

typedef unsigned long ServerStateChecker_Key;
typedef unsigned long ServerStateChecker_Index;
typedef unsigned char ServerStateChecker_State;

struct ServerStateChecker_RequestPacket
{
	BYTE header;
	ServerStateChecker_Key key;	
	ServerStateChecker_Index index;
};

struct ServerStateChecker_ResponsePacket
{
	BYTE header;
	ServerStateChecker_Key key;
	ServerStateChecker_Index index;
	ServerStateChecker_State state;
};

#pragma pack()

CPacketInfoUDP::CPacketInfoUDP()
{
	Set(1, sizeof(ServerStateChecker_RequestPacket), "ServerStateRequest");
}

CPacketInfoUDP::~CPacketInfoUDP()
{
	Log("udp_packet_info.txt");
}


CInputUDP::CInputUDP()
{
	memset( &m_SockAddr, 0, sizeof(m_SockAddr) );

	BindPacketInfo(&m_packetInfoUDP);
}

void CInputUDP::Handshake(LPDESC pDesc, const char * c_pData)
{
	TPacketCGHandshake * pInfo = (TPacketCGHandshake *) c_pData;

	if (pDesc->GetHandshake() == pInfo->dwHandshake)
	{
		sys_log(0, "UDP: Grant %s:%d", inet_ntoa(m_SockAddr.sin_addr), m_SockAddr.sin_port);
		pDesc->UDPGrant(m_SockAddr);
		return;
	}
	else
		sys_log(0, "UDP: Handshake differs %s", pDesc->GetHostName());
}

void CInputUDP::StateChecker(const char * c_pData) {}

int CInputUDP::Analyze(LPDESC pDesc, BYTE bHeader, const char * c_pData)
{
	switch (bHeader)
	{
		default:
			sys_err("unknown UDP header %u", bHeader);
			break;
	}

	return 0;
}

bool CInputUDP::Process(LPDESC pDesc, const void * c_pvOrig, int iBytes, int & r_iBytesProceed)
{
	return true;
}

