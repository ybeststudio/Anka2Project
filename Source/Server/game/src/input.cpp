#include "stdafx.h"

#include "../../common/service.h"

#include <sstream>
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "buffer_manager.h"
#include "config.h"
#include "profiler.h"
#include "p2p.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "login_sim.h"
#include "fishing.h"
#include "TrafficProfiler.h"
#include "priv_manager.h"

extern time_t get_global_time();

bool IsEmptyAdminPage()
{
	return g_stAdminPageIP.empty();
}

bool IsAdminPage(const char * ip)
{
	for (size_t n = 0; n < g_stAdminPageIP.size(); ++n)
	{
		if (g_stAdminPageIP[n] == ip)
			return 1; 
	}	
	return 0;
}

void ClearAdminPages()
{
	for (size_t n = 0; n < g_stAdminPageIP.size(); ++n)
		g_stAdminPageIP[n].clear();

	g_stAdminPageIP.clear();
}

CInputProcessor::CInputProcessor() : m_pPacketInfo(NULL), m_iBufferLeft(0)
{
	if (!m_pPacketInfo)
		BindPacketInfo(&m_packetInfoCG);
}

void CInputProcessor::BindPacketInfo(CPacketInfo * pPacketInfo)
{
	m_pPacketInfo = pPacketInfo;
}

bool CInputProcessor::Process(LPDESC lpDesc, const void * c_pvOrig, int iBytes, int & r_iBytesProceed)
{
	const char * c_pData = (const char *) c_pvOrig;

	BYTE bLastHeader = 0;
	int iLastPacketLen = 0;
	int iPacketLen;

#ifdef ENABLE_IMPROVED_HANDSHAKE_PROCESS
	if (lpDesc && DESC_MANAGER::instance().IsIntruder(lpDesc->GetHostName()))
	{
		lpDesc->SetPhase(PHASE_CLOSE);
		return true;
	}
#endif

	if (!m_pPacketInfo)
	{
		sys_err("No packet info has been binded to");
		return true;
	}

	for (m_iBufferLeft = iBytes; m_iBufferLeft > 0;)
	{
		BYTE bHeader = (BYTE) *(c_pData);
		const char * c_pszName;

		if (bHeader == 0)
			iPacketLen = 1;
		else if (!m_pPacketInfo->Get(bHeader, &iPacketLen, &c_pszName))
		{
			sys_err("UNKNOWN HEADER: %d, LAST HEADER: %d(%d), REMAIN BYTES: %d, fd: %d",
					bHeader, bLastHeader, iLastPacketLen, m_iBufferLeft, lpDesc->GetSocket());
			lpDesc->SetPhase(PHASE_CLOSE);
			return true;
		}

		if (m_iBufferLeft < iPacketLen)
			return true;

		if (bHeader)
		{
			if (test_server && bHeader != HEADER_CG_MOVE)
				sys_log(0, "Packet Analyze [Header %d][bufferLeft %d] ", bHeader, m_iBufferLeft);

			m_pPacketInfo->Start();

			int iExtraPacketSize = Analyze(lpDesc, bHeader, c_pData);

			if (iExtraPacketSize < 0)
			{
				sys_err("Failed to analyze header(%u) host(%s)", bHeader, inet_ntoa(lpDesc->GetAddr().sin_addr));
				lpDesc->SetPhase(PHASE_CLOSE);
				return true;
			}

			iPacketLen += iExtraPacketSize;
			lpDesc->Log("%s %d", c_pszName, iPacketLen);
			m_pPacketInfo->End();
		}

		if (g_bTrafficProfileOn)
			TrafficProfiler::instance().Report(TrafficProfiler::IODIR_INPUT, bHeader, iPacketLen);

		if (bHeader == HEADER_CG_PONG)
			sys_log(0, "PONG! %u", *(BYTE *) (c_pData + iPacketLen - sizeof(BYTE)));

		c_pData	+= iPacketLen;
		m_iBufferLeft -= iPacketLen;
		r_iBytesProceed += iPacketLen;

		iLastPacketLen = iPacketLen;
		bLastHeader	= bHeader;

		if (GetType() != lpDesc->GetInputProcessor()->GetType())
			return false;
	}

	return true;
}

void CInputProcessor::Pong(LPDESC d)
{
	d->SetPong(true);
}

void CInputProcessor::Handshake(LPDESC d, const char * c_pData)
{
	TPacketCGHandshake * p = (TPacketCGHandshake *) c_pData;

	if (d->GetHandshake() != p->dwHandshake)
	{
		sys_err("Invalid Handshake on %d", d->GetSocket());
		d->SetPhase(PHASE_CLOSE);
	}
	else
	{
		if (d->IsPhase(PHASE_HANDSHAKE))
		{
			if (d->HandshakeProcess(p->dwTime, p->lDelta, false))
			{
				if (g_bAuthServer)
					d->SetPhase(PHASE_AUTH);
				else
					d->SetPhase(PHASE_LOGIN);
			}
		}
		else
			d->HandshakeProcess(p->dwTime, p->lDelta, true);
	}
}

void LoginFailure(LPDESC d, const char * c_pszStatus)
{
	if (!d)
		return;

	TPacketGCLoginFailure failurePacket;

	failurePacket.header = HEADER_GC_LOGIN_FAILURE;
	strlcpy(failurePacket.szStatus, c_pszStatus, sizeof(failurePacket.szStatus));

	d->Packet(&failurePacket, sizeof(failurePacket));
}

CInputHandshake::CInputHandshake()
{
	CPacketInfoCG * pkPacketInfo = M2_NEW CPacketInfoCG;

	m_pMainPacketInfo = m_pPacketInfo;
	BindPacketInfo(pkPacketInfo);
}

CInputHandshake::~CInputHandshake()
{
	if( NULL != m_pPacketInfo )
	{
		M2_DELETE(m_pPacketInfo);
		m_pPacketInfo = NULL;
	}
}

std::map<DWORD, CLoginSim *> g_sim;
std::map<DWORD, CLoginSim *> g_simByPID;
std::vector<TPlayerTable> g_vec_save;

// BLOCK_CHAT
ACMD(do_block_chat);
// END_OF_BLOCK_CHAT

int CInputHandshake::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	if (bHeader == 10)
		return 0;

	if (bHeader == HEADER_CG_MARK_LOGIN)
	{
		if (!guild_mark_server)
		{
			sys_err("Guild Mark login requested but i'm not a mark server!");
			d->SetPhase(PHASE_CLOSE);
			return 0;
		}
		sys_log(0, "MARK_SERVER: Login");
		d->SetPhase(PHASE_LOGIN);
		return 0;
	}

	else if (bHeader == HEADER_CG_STATE_CHECKER)
	{
		if (d->isChannelStatusRequested())
			return 0;

		d->SetChannelStatusRequested(true);
		db_clientdesc->DBPacket(HEADER_GD_REQUEST_CHANNELSTATUS, d->GetHandle(), NULL, 0);
	}

	else if (bHeader == HEADER_CG_PONG)
		Pong(d);

	else if (bHeader == HEADER_CG_HANDSHAKE)
		Handshake(d, c_pData);

	else
	{
		#ifdef SYS_ERR_SOURCE_TRACKING
		sys_err("HANDSHAKE phase does not handle packet %d (fd %d) (source %s:%u)", bHeader, d->GetSocket(), d->GetHostName(), d->GetPort());
		#else
		sys_err("Handshake phase does not handle packet %d (fd %d)", bHeader, d->GetSocket());
		#endif
		return -1;
	}

	return 0;
}
