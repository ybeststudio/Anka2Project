#include "StdAfx.h"
#include "NetStream.h"
#include "../eterBase/tea.h"

void CNetworkStream::SetSecurityMode(bool isSecurityMode, const char *c_szTeaKey)
{
	m_isSecurityMode = isSecurityMode;
	memcpy(m_szEncryptKey, c_szTeaKey, TEA_KEY_LENGTH);
	memcpy(m_szDecryptKey, c_szTeaKey, TEA_KEY_LENGTH);
}

void CNetworkStream::SetSecurityMode(bool isSecurityMode, const char *c_szTeaEncryptKey, const char *c_szTeaDecryptKey)
{
	m_isSecurityMode = isSecurityMode;
	memcpy(m_szEncryptKey, c_szTeaEncryptKey, TEA_KEY_LENGTH);
	memcpy(m_szDecryptKey, c_szTeaDecryptKey, TEA_KEY_LENGTH);
}

bool CNetworkStream::IsSecurityMode()
{
	return m_isSecurityMode;
}

void CNetworkStream::SetRecvBufferSize(int recvBufSize)
{
	if (m_recvBuf)
	{
		if (m_recvBufSize>recvBufSize)
			return;

		delete [] m_recvBuf;

		if (m_recvTEABuf)
			delete [] m_recvTEABuf;
	}
	m_recvBufSize = recvBufSize;
	m_recvBuf = new char[m_recvBufSize];	
	m_recvTEABufSize = ((m_recvBufSize>>3)+1)<<3;
	m_recvTEABuf = new char[m_recvTEABufSize];
}

void CNetworkStream::SetSendBufferSize(int sendBufSize)
{
	if (m_sendBuf)
	{
		if (m_sendBufSize > sendBufSize)
			return;

		delete [] m_sendBuf;

		if (m_sendTEABuf)
			delete [] m_sendTEABuf;
	}

	m_sendBufSize = sendBufSize;
	m_sendBuf = new char[m_sendBufSize];
	m_sendTEABufSize = ((m_sendBufSize>>3)+1)<<3;
	m_sendTEABuf = new char[m_sendTEABufSize];
}

bool CNetworkStream::__RecvInternalBuffer()
{
	if (m_recvBufOutputPos>0)
	{
		int recvBufDataSize = m_recvBufInputPos - m_recvBufOutputPos;
		if (recvBufDataSize>0)
		{
			memmove(m_recvBuf, m_recvBuf + m_recvBufOutputPos, recvBufDataSize);
		}
		
		m_recvBufInputPos -= m_recvBufOutputPos;
		m_recvBufOutputPos = 0;
	}

	if (IsSecurityMode())
	{		
		int restSize = min(m_recvTEABufSize - m_recvTEABufInputPos, m_recvBufSize - m_recvBufInputPos);
 
		if (restSize > 0)
		{
			int recvSize = recv(m_sock, m_recvTEABuf + m_recvTEABufInputPos, restSize, 0);	

			if (recvSize < 0)
			{
				int error = WSAGetLastError();

				if (error != WSAEWOULDBLOCK)
				{
					return false;
				}
			}
			else if (recvSize == 0)
			{
				return false;
			}

			m_recvTEABufInputPos += recvSize;

			int decodeSize = m_recvTEABufInputPos;

			if (decodeSize >= 8)
			{
				decodeSize >>= 3;
				decodeSize <<= 3;

				int decodeDstSize = tea_decrypt((DWORD *) (m_recvBuf + m_recvBufInputPos),
												(DWORD *) m_recvTEABuf,
												(const DWORD *) m_szDecryptKey,
												decodeSize);

				m_recvBufInputPos += decodeDstSize;

				if (m_recvTEABufInputPos>decodeSize)
					memmove(m_recvTEABuf, m_recvTEABuf+decodeSize, m_recvTEABufInputPos-decodeSize);

				m_recvTEABufInputPos -= decodeSize;
			}
		}
	}
	else
	{
		int restSize = m_recvBufSize - m_recvBufInputPos;
		if (restSize>0)
		{		
			int recvSize = recv(m_sock, m_recvBuf + m_recvBufInputPos, m_recvBufSize - m_recvBufInputPos, 0);	

			if (recvSize < 0)
			{
				int error = WSAGetLastError();

				if (error != WSAEWOULDBLOCK)
				{
					return false;
				}
			}
			else if (recvSize == 0)
			{
				return false;
			}

			m_recvBufInputPos += recvSize;
		}
	}

	return true;
}


bool CNetworkStream::__SendInternalBuffer()
{
	if (IsSecurityMode())
	{
		int encodeSize=__GetSendBufferSize();
		if (encodeSize<=0)
			return true;

		m_sendTEABufInputPos += tea_encrypt((DWORD *) (m_sendTEABuf + m_sendTEABufInputPos),
												 (DWORD *) (m_sendBuf + m_sendBufOutputPos),
												 (const DWORD *) m_szEncryptKey,
												 encodeSize);
		m_sendBufOutputPos += encodeSize;

		if (m_sendTEABufInputPos>0)
		{	
			int sendSize = send(m_sock, m_sendTEABuf, m_sendTEABufInputPos, 0);	
			if (sendSize < 0)
				return false;

			if (m_sendTEABufInputPos>sendSize)
				memmove(m_sendTEABuf, m_sendTEABuf+sendSize, m_sendTEABufInputPos-sendSize);

			m_sendTEABufInputPos-=sendSize;
		}

		__PopSendBuffer();
	}
	else
	{
		int dataSize=__GetSendBufferSize();
		if (dataSize<=0)
			return true;

		int sendSize = send(m_sock, m_sendBuf+m_sendBufOutputPos, dataSize, 0);	
		if (sendSize < 0)
			return false;

		m_sendBufOutputPos+=sendSize;

		__PopSendBuffer();
	}

	return true;
}

void CNetworkStream::__PopSendBuffer()
{
	if (m_sendBufOutputPos<=0)
		return;
		
	int sendBufDataSize = m_sendBufInputPos - m_sendBufOutputPos;

	if (sendBufDataSize>0)
	{
		memmove(m_sendBuf, m_sendBuf+m_sendBufOutputPos, sendBufDataSize);
	}

	m_sendBufInputPos = sendBufDataSize;
	m_sendBufOutputPos = 0;	
}

#pragma warning(push)
#pragma warning(disable:4127)
void CNetworkStream::Process()
{
	if (m_sock == INVALID_SOCKET)
		return;

	fd_set fdsRecv;
	fd_set fdsSend;

	FD_ZERO(&fdsRecv);
	FD_ZERO(&fdsSend);

	FD_SET(m_sock, &fdsRecv);
	FD_SET(m_sock, &fdsSend);

	TIMEVAL delay;

	delay.tv_sec = 0;
	delay.tv_usec = 0;
	
	if (select(0, &fdsRecv, &fdsSend, nullptr, &delay) == SOCKET_ERROR)
		return;

	if (!m_isOnline)
	{
		if (FD_ISSET(m_sock, &fdsSend))
		{
			m_isOnline = true;
			OnConnectSuccess();
		}
		else if (time(nullptr) > m_connectLimitTime)
		{
			Clear();
			OnConnectFailure();
		}

		return;
	}

	if (FD_ISSET(m_sock, &fdsSend) && (m_sendBufInputPos > m_sendBufOutputPos))
	{
		if (!__SendInternalBuffer())
		{
			int error = WSAGetLastError();

			if (error != WSAEWOULDBLOCK)
			{
				OnRemoteDisconnect();
				Clear();
				return;
			}
		}
	}

	if (FD_ISSET(m_sock, &fdsRecv))
	{
		if (!__RecvInternalBuffer())
		{
			OnRemoteDisconnect();
			Clear();
			return;
		}
	}

	if (!OnProcess())
	{
		OnRemoteDisconnect();
		Clear();
	}
}
#pragma warning(pop)

void CNetworkStream::Disconnect()
{
	if (m_sock == INVALID_SOCKET)
		return;

	//OnDisconnect();

	Clear();
}

void CNetworkStream::Clear()
{
	if (m_sock == INVALID_SOCKET)
	{
		return;
	}

	closesocket(m_sock);
	m_sock = INVALID_SOCKET;

	memset(m_szEncryptKey, 0, sizeof(m_szEncryptKey));
	memset(m_szDecryptKey, 0, sizeof(m_szDecryptKey));

	m_isSecurityMode = false;

	m_isOnline = false;
	m_connectLimitTime = 0;

	m_recvTEABufInputPos = 0;
	m_sendTEABufInputPos = 0;

	m_recvBufInputPos = 0;
	m_recvBufOutputPos = 0;

	m_sendBufInputPos = 0;
	m_sendBufOutputPos = 0;
}

bool CNetworkStream::Connect (const CNetworkAddress& c_rkNetAddr, int limitSec)
{
	Clear();

	m_addr = c_rkNetAddr;

	m_sock = socket (AF_INET, SOCK_STREAM, 0);

	if (m_sock == INVALID_SOCKET)
	{
		Clear();
		OnConnectFailure();
		return false;
	}

	DWORD arg = 1;
	ioctlsocket (m_sock, FIONBIO, &arg);	// Non-blocking mode

	if (connect(m_sock, reinterpret_cast<PSOCKADDR>(&m_addr), m_addr.GetSize()) == SOCKET_ERROR)
	{
		int error = WSAGetLastError();

		if (error != WSAEWOULDBLOCK)
		{
			Sleep(1000);
			Clear();
			m_sock = socket(AF_INET, SOCK_STREAM, 0);
			if (m_sock == INVALID_SOCKET)
			{
				Clear();
				OnConnectFailure();
				return false;

			}

			ioctlsocket(m_sock, FIONBIO, &arg);
			if (connect(m_sock, reinterpret_cast<PSOCKADDR>(&m_addr), m_addr.GetSize()) == SOCKET_ERROR)
			{
				Tracen("error != WSAEWOULDBLOCK");
				Clear();
				OnConnectFailure();
				return false;
			}
		}
	}

	m_connectLimitTime = time (nullptr) + limitSec;
	return true;
}

bool CNetworkStream::Connect (DWORD dwAddr, int port, int limitSec)
{
	char szAddr[256];
	{
		BYTE ip[4];
		ip[0]=dwAddr&0xff;
		dwAddr>>=8;
		ip[1]=dwAddr&0xff;
		dwAddr>>=8;
		ip[2]=dwAddr&0xff;
		dwAddr>>=8;
		ip[3]=dwAddr&0xff;
		dwAddr>>=8;

		snprintf(szAddr, sizeof(szAddr), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	}

	return Connect (szAddr, port, limitSec);
}

bool CNetworkStream::Connect (const char* c_szAddr, int port, int /*limitSec*/)
{
	CNetworkAddress kNetAddr;
	kNetAddr.Set (c_szAddr, port);

	return Connect (kNetAddr);
}

void CNetworkStream::ClearRecvBuffer()
{
	m_recvBufOutputPos = m_recvBufInputPos = 0;
}

int CNetworkStream::GetRecvBufferSize()
{
	return m_recvBufInputPos - m_recvBufOutputPos;
}

bool CNetworkStream::Peek (int size)
{
	if (GetRecvBufferSize() < size)
	{
		return false;
	}

	return true;
}

bool CNetworkStream::Peek (int size, char* pDestBuf)
{
	if (GetRecvBufferSize() < size)
	{
		return false;
	}

	memcpy (pDestBuf, m_recvBuf + m_recvBufOutputPos, size);
	return true;
}

bool CNetworkStream::Recv (int size)
{
	if (!Peek (size))
	{
		return false;
	}

	m_recvBufOutputPos += size;
	return true;
}

bool CNetworkStream::Recv (int size, char* pDestBuf)
{
	if (!Peek (size, pDestBuf))
	{
		return false;
	}

	m_recvBufOutputPos += size;
	return true;
}

int CNetworkStream::__GetSendBufferSize()
{
	return m_sendBufInputPos-m_sendBufOutputPos;
}

bool CNetworkStream::Send (int size, const char* pSrcBuf)
{
	int sendBufRestSize = m_sendBufSize - m_sendBufInputPos;
	if ((size + 1) > sendBufRestSize)
	{
		return false;
	}

	memcpy (m_sendBuf + m_sendBufInputPos, pSrcBuf, size);
	m_sendBufInputPos += size;

	return true;
}

bool CNetworkStream::Peek (int len, void* pDestBuf)
{
	return Peek (len, (char*)pDestBuf);
}

bool CNetworkStream::Recv (int len, void* pDestBuf)
{
	return Recv (len, (char*)pDestBuf);
}

bool CNetworkStream::SendFlush (int len, const void* pSrcBuf)
{
	if (!Send (len, pSrcBuf))
	{
		return false;
	}

	return __SendInternalBuffer();
}

bool CNetworkStream::Send (int len, const void* pSrcBuf)
{
	return Send (len, (const char*)pSrcBuf);
}

bool CNetworkStream::IsOnline()
{
	return m_isOnline;
}

void CNetworkStream::SetPacketSequenceMode(bool isOn)
{
}

bool CNetworkStream::SendSequence()
{
	return true;
}

bool CNetworkStream::OnProcess()
{
	return true;
}

void CNetworkStream::OnRemoteDisconnect()
{
}

void CNetworkStream::OnDisconnect()
{
}

void CNetworkStream::OnConnectSuccess()
{
	Tracen ("Succeed connecting.");
}

void CNetworkStream::OnConnectFailure()
{
	Tracen ("Failed to connect.");
}

CNetworkStream::CNetworkStream()
{
	m_sock = INVALID_SOCKET;

	m_isSecurityMode = false;
	m_isOnline = false;
	m_connectLimitTime = 0;

	m_recvTEABuf = nullptr;
	m_recvTEABufSize = 0;
	m_recvTEABufInputPos = 0;

	m_recvBuf = nullptr;
	m_recvBufSize = 0;
	m_recvBufOutputPos = 0;
	m_recvBufInputPos = 0;

	m_sendTEABuf = nullptr;
	m_sendTEABuf = 0;
	m_sendTEABufInputPos = 0;

	m_sendBuf = nullptr;
	m_sendBufSize = 0;
	m_sendBufOutputPos = 0;
	m_sendBufInputPos = 0;
}

CNetworkStream::~CNetworkStream()
{
	Clear();

	if (m_sendTEABuf)
	{
		delete [] m_sendTEABuf;
		m_sendTEABuf=NULL;
	}

	if (m_recvTEABuf)
	{
		delete [] m_recvTEABuf;
		m_recvTEABuf=NULL;
	}

	if (m_recvBuf)
	{
		delete [] m_recvBuf;
		m_recvBuf=NULL;
	}

	if (m_sendBuf)
	{
		delete [] m_sendBuf;
		m_sendBuf=NULL;
	}
}
