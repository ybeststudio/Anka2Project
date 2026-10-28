#include "StdAfx.h"
#include "GuildMarkUploader.h"
#include "Packet.h"

CGuildMarkUploader::CGuildMarkUploader()
 : m_pbySymbolBuf(nullptr)
{
	SetRecvBufferSize(1024);
	SetSendBufferSize(1024);
	__Initialize();
}

CGuildMarkUploader::~CGuildMarkUploader()
{
	__OfflineState_Set();
}

void CGuildMarkUploader::Disconnect()
{
	__OfflineState_Set();
}

bool CGuildMarkUploader::IsCompleteUploading()
{
	return STATE_OFFLINE == m_eState;
}

bool CGuildMarkUploader::__Save(const char *c_szFileName)
{
	return true;
}

bool CGuildMarkUploader::__Load(const char *c_szFileName, UINT* peError)
{
	ILuint uImg;
	ilGenImages(1, &uImg);
	ilBindImage(uImg);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);

	if (!ilLoad(IL_TYPE_UNKNOWN, (const ILstring)c_szFileName))
	{
		*peError=ERROR_LOAD;
		return false;
	}

	if (ilGetInteger(IL_IMAGE_WIDTH)!=SGuildMark::WIDTH)
	{
		*peError=ERROR_WIDTH;
		return false;
	}

	if (ilGetInteger(IL_IMAGE_HEIGHT)!=SGuildMark::HEIGHT)
	{
		*peError=ERROR_HEIGHT;
		return false;
	}

	ilConvertImage(IL_BGRA, IL_BYTE);

	ilCopyPixels(0, 0, 0, SGuildMark::WIDTH, SGuildMark::HEIGHT, 1, IL_BGRA, IL_BYTE, (ILvoid*)m_kMark.m_apxBuf);

	ilDeleteImages(1, &uImg);
	return true;
}

bool CGuildMarkUploader::__LoadSymbol(const char *c_szFileName, UINT* peError)
{
	ILuint uImg;
	ilGenImages(1, &uImg);
	ilBindImage(uImg);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	if (!ilLoad(IL_TYPE_UNKNOWN, (const ILstring)c_szFileName))
	{
		*peError=ERROR_LOAD;
		return false;
	}
	if (ilGetInteger(IL_IMAGE_WIDTH) != 64)
	{
		*peError=ERROR_WIDTH;
		return false;
	}
	if (ilGetInteger(IL_IMAGE_HEIGHT) != 128)
	{
		*peError=ERROR_HEIGHT;
		return false;
	}
	ilDeleteImages(1, &uImg);
	ilShutDown();

	FILE * file = fopen(c_szFileName, "rb");
	if (!file)
	{
		*peError=ERROR_LOAD;
	}

	fseek(file, 0, SEEK_END);
	m_dwSymbolBufSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	m_pbySymbolBuf = new BYTE [m_dwSymbolBufSize];
	fread(m_pbySymbolBuf, m_dwSymbolBufSize, 1, file);

	fclose(file);

	m_dwSymbolCRC32 = GetFileCRC32(c_szFileName);
	return true;
}

bool CGuildMarkUploader::Connect(const CNetworkAddress& c_rkNetAddr, DWORD dwHandle, DWORD dwRandomKey, DWORD dwGuildID, const char* c_szFileName, UINT* peError)
{
	__OfflineState_Set();
	SetRecvBufferSize(1024);
	SetSendBufferSize(1024);

	if (!CNetworkStream::Connect(c_rkNetAddr))
	{
		*peError = ERROR_CONNECT;
		return false;
	}

	m_dwSendType = SEND_TYPE_MARK;
	m_dwHandle = dwHandle;
	m_dwRandomKey = dwRandomKey;
	m_dwGuildID = dwGuildID;

	if (!__Load(c_szFileName, peError))
		return false;

	return true;
}

bool CGuildMarkUploader::ConnectToSendSymbol(const CNetworkAddress& c_rkNetAddr, DWORD dwHandle, DWORD dwRandomKey, DWORD dwGuildID, const char* c_szFileName, UINT* peError)
{
	__OfflineState_Set();
	SetRecvBufferSize(1024);
	SetSendBufferSize(64*1024);

	if (!CNetworkStream::Connect(c_rkNetAddr))
	{
		*peError = ERROR_CONNECT;
		return false;
	}

	m_dwSendType = SEND_TYPE_SYMBOL;
	m_dwHandle = dwHandle;
	m_dwRandomKey = dwRandomKey;
	m_dwGuildID = dwGuildID;

	if (!__LoadSymbol(c_szFileName, peError))
		return false;

	return true;
}

void CGuildMarkUploader::Process()
{
	CNetworkStream::Process();

	if (!__StateProcess())
	{
		__OfflineState_Set();
		Disconnect();
	}
}

void CGuildMarkUploader::OnConnectFailure()
{
	__OfflineState_Set();
}

void CGuildMarkUploader::OnConnectSuccess()
{
	__LoginState_Set();
}

void CGuildMarkUploader::OnRemoteDisconnect()
{
	__OfflineState_Set();
}

void CGuildMarkUploader::OnDisconnect()
{
	__OfflineState_Set();
}

void CGuildMarkUploader::__Initialize()
{
	m_eState = STATE_OFFLINE;

	m_dwGuildID = 0;
	m_dwHandle = 0;
	m_dwRandomKey = 0;

	if (m_pbySymbolBuf)
	{
		delete m_pbySymbolBuf;
	}

	m_dwSymbolBufSize = 0;
	m_pbySymbolBuf = nullptr;
}

bool CGuildMarkUploader::__StateProcess()
{
	switch (m_eState)
	{
		case STATE_LOGIN:
			return __LoginState_Process();
			break;
	}

	return true;
}

void CGuildMarkUploader::__OfflineState_Set()
{
	__Initialize();
}

void CGuildMarkUploader::__CompleteState_Set()
{
	m_eState=STATE_COMPLETE;

	__OfflineState_Set();
}


void CGuildMarkUploader::__LoginState_Set()
{
	m_eState=STATE_LOGIN;
}

bool CGuildMarkUploader::__LoginState_Process()
{
	if (!__AnalyzePacket(HEADER_GC_PHASE, sizeof(TPacketGCPhase), &CGuildMarkUploader::__LoginState_RecvPhase))
		return false;

	if (!__AnalyzePacket(HEADER_GC_HANDSHAKE, sizeof(TPacketGCHandshake), &CGuildMarkUploader::__LoginState_RecvHandshake))
		return false;

	if (!__AnalyzePacket(HEADER_GC_PING, sizeof(TPacketGCPing), &CGuildMarkUploader::__LoginState_RecvPing))
		return false;

	return true;
}

bool CGuildMarkUploader::__SendMarkPacket()
{
	TPacketCGMarkUpload kPacketMarkUpload;
	kPacketMarkUpload.header = HEADER_CG_MARK_UPLOAD;
	kPacketMarkUpload.gid = m_dwGuildID;

	assert(sizeof(kPacketMarkUpload.image) == sizeof(m_kMark.m_apxBuf));
	memcpy(kPacketMarkUpload.image, m_kMark.m_apxBuf, sizeof(kPacketMarkUpload.image));

	if (!Send(sizeof(kPacketMarkUpload), &kPacketMarkUpload))
		return false;

	return true;
}
bool CGuildMarkUploader::__SendSymbolPacket()
{
	if (!m_pbySymbolBuf)
		return false;

	TPacketCGSymbolUpload kPacketSymbolUpload;
	kPacketSymbolUpload.header=HEADER_CG_GUILD_SYMBOL_UPLOAD;
	kPacketSymbolUpload.handle=m_dwGuildID;
	kPacketSymbolUpload.size=sizeof(TPacketCGSymbolUpload) + m_dwSymbolBufSize;

	if (!Send(sizeof(TPacketCGSymbolUpload), &kPacketSymbolUpload))
		return false;
	if (!Send(m_dwSymbolBufSize, m_pbySymbolBuf))
		return false;

#ifdef _DEBUG
	printf("__SendSymbolPacket : [GuildID:%d/PacketSize:%d/BufSize:%d/CRC:%d]\n", m_dwGuildID, kPacketSymbolUpload.size, m_dwSymbolBufSize, m_dwSymbolCRC32);
#endif

	CNetworkStream::__SendInternalBuffer();
	__CompleteState_Set();

	return true;
}

bool CGuildMarkUploader::__LoginState_RecvPhase()
{
	TPacketGCPhase kPacketPhase;
	if (!Recv(sizeof(kPacketPhase), &kPacketPhase))
		return false;

	if (kPacketPhase.phase==PHASE_LOGIN)
	{
		const char *key = LSS_SECURITY_KEY;
		SetSecurityMode(true, key);

		if (SEND_TYPE_MARK == m_dwSendType)
		{
			if (!__SendMarkPacket())
				return false;
		}
		else if (SEND_TYPE_SYMBOL == m_dwSendType)
		{
			if (!__SendSymbolPacket())
				return false;
		}
	}

	return true;
}

bool CGuildMarkUploader::__LoginState_RecvHandshake()
{
	TPacketGCHandshake kPacketHandshake;
	if (!Recv(sizeof(kPacketHandshake), &kPacketHandshake))
		return false;

	{
		TPacketCGMarkLogin kPacketMarkLogin;
		kPacketMarkLogin.header=HEADER_CG_MARK_LOGIN;
		kPacketMarkLogin.handle=m_dwHandle;
		kPacketMarkLogin.random_key=m_dwRandomKey;
		if (!Send(sizeof(kPacketMarkLogin), &kPacketMarkLogin))
			return false;
	}

	return true;
}

bool CGuildMarkUploader::__LoginState_RecvPing()
{
	TPacketGCPing kPacketPing;
	if (!Recv(sizeof(kPacketPing), &kPacketPing))
		return false;

	TPacketCGPong kPacketPong;
	kPacketPong.bHeader = HEADER_CG_PONG;

	if (!Send(sizeof(TPacketCGPong), &kPacketPong))
		return false;

	if (IsSecurityMode())
		return SendSequence();
	else
		return true;
}

bool CGuildMarkUploader::__AnalyzePacket(UINT uHeader, UINT uPacketSize, bool (CGuildMarkUploader::*pfnDispatchPacket)())
{
	BYTE bHeader;
	if (!Peek(sizeof(bHeader), &bHeader))
		return true;

	if (bHeader!=uHeader)
		return true;

	if (!Peek(uPacketSize))
		return true;

	return (this->*pfnDispatchPacket)();
}
