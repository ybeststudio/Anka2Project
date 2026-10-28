#include "StdAfx.h"
#include "Timer.h"

static LARGE_INTEGER gs_qpcFrequency = { 0 };
static LARGE_INTEGER gs_qpcLastTime = { 0 };
static double gs_fCurrentTime = 0.0;
static double gs_fElapsedTime = 0.0166666;
static DWORD gs_dwBaseTime = 0;
static DWORD gs_dwServerTime = 0;
static DWORD gs_dwClientTime = 0;
static DWORD gs_dwFrameTime = 0;

#pragma comment(lib, "winmm.lib")

BOOL ELTimer_Init()
{
	gs_dwBaseTime = timeGetTime();
	if (gs_qpcFrequency.QuadPart == 0)
	{
		QueryPerformanceFrequency(&gs_qpcFrequency);
		QueryPerformanceCounter(&gs_qpcLastTime);
	}
	return 1;
}

DWORD ELTimer_GetMSec()
{
	return timeGetTime() - gs_dwBaseTime;
}

VOID	ELTimer_SetServerMSec(DWORD dwServerTime)
{
	NANOBEGIN
	if (0 != dwServerTime)
	{
		gs_dwServerTime = dwServerTime;
		gs_dwClientTime = CTimer::instance().GetCurrentMillisecond();
	}
	NANOEND
}

DWORD	ELTimer_GetServerMSec()
{
	return CTimer::instance().GetCurrentMillisecond() - gs_dwClientTime + gs_dwServerTime;
}

DWORD	ELTimer_GetFrameMSec()
{
	return gs_dwFrameTime;
}

DWORD	ELTimer_GetServerFrameMSec()
{
	return ELTimer_GetFrameMSec() - gs_dwClientTime + gs_dwServerTime;
}

VOID	ELTimer_SetFrameMSec()
{
	gs_dwFrameTime = ELTimer_GetMSec();
}

CTimer::CTimer()
{
	ELTimer_Init();

	NANOBEGIN
	if (this)
	{
		m_dwCurrentTime = 0;
		m_bUseRealTime = true;
		m_index = 0;

		m_dwElapsedTime = 16;
		m_fCurrentTime = 0.0f;
#ifdef ENABLE_MORE_FPS
		m_firstArg = 16;
		m_SecondArg = 1;
#endif
	}
	NANOEND
}

CTimer::~CTimer()
{
}

void CTimer::SetBaseTime()
{
	m_dwCurrentTime = 0;
	gs_fCurrentTime = 0.0;
	if (gs_qpcFrequency.QuadPart != 0)
		QueryPerformanceCounter(&gs_qpcLastTime);
}

void CTimer::Advance()
{
	if (gs_qpcFrequency.QuadPart == 0)
	{
		QueryPerformanceFrequency(&gs_qpcFrequency);
		QueryPerformanceCounter(&gs_qpcLastTime);
	}

	LARGE_INTEGER qpcCurrent;
	QueryPerformanceCounter(&qpcCurrent);

	if (gs_qpcLastTime.QuadPart == 0)
		gs_qpcLastTime = qpcCurrent;

	double deltaSeconds = static_cast<double>(qpcCurrent.QuadPart - gs_qpcLastTime.QuadPart) / static_cast<double>(gs_qpcFrequency.QuadPart);
	gs_qpcLastTime = qpcCurrent;

	// Clamp spikes (e.g. map loading, debugger pause, alt-tab) to max 0.1s (100ms)
	if (deltaSeconds > 0.1)
		deltaSeconds = 0.1;
	else if (deltaSeconds < 0.0)
		deltaSeconds = 0.001;

	gs_fElapsedTime = deltaSeconds;
	gs_fCurrentTime += deltaSeconds;

	m_dwElapsedTime = static_cast<DWORD>(deltaSeconds * 1000.0);
	m_dwCurrentTime = static_cast<DWORD>(gs_fCurrentTime * 1000.0);
	m_fCurrentTime = static_cast<float>(gs_fCurrentTime);
}

void CTimer::Adjust(int iTimeGap)
{
	gs_fCurrentTime += static_cast<double>(iTimeGap) / 1000.0;
	m_dwCurrentTime += iTimeGap;
	m_fCurrentTime = static_cast<float>(gs_fCurrentTime);
}

float CTimer::GetCurrentSecond()
{
	return static_cast<float>(gs_fCurrentTime);
}

DWORD CTimer::GetCurrentMillisecond()
{
	return static_cast<DWORD>(gs_fCurrentTime * 1000.0);
}

float CTimer::GetElapsedSecond()
{
	return static_cast<float>(gs_fElapsedTime);
}

DWORD CTimer::GetElapsedMilliecond()
{
	return static_cast<DWORD>(gs_fElapsedTime * 1000.0);
}

void CTimer::UseCustomTime()
{
	m_bUseRealTime = true;
}
