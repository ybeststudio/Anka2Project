#pragma once

#include <windows.h>
#include "Singleton.h"

class CTimer : public CSingleton<CTimer>
{
	public:
		CTimer();
		virtual ~CTimer();

		void	Advance();
		void	Adjust(int iTimeGap);
		void	SetBaseTime();

		float	GetCurrentSecond();
		DWORD	GetCurrentMillisecond();

		float	GetElapsedSecond();
		DWORD	GetElapsedMilliecond();

		void	UseCustomTime();

	protected:
		bool	m_bUseRealTime;
		DWORD	m_dwBaseTime;
		DWORD	m_dwCurrentTime;
		float	m_fCurrentTime;
		DWORD	m_dwElapsedTime;
		int		m_index;

#ifdef ENABLE_MORE_FPS
public:
	void SetUpdateTime(uint32_t frequency, uint8_t first, uint8_t sec) { m_freq = frequency; m_firstArg = first; m_SecondArg = sec; }
	uint32_t GetFrequency() const { return m_freq; }

protected:
	uint8_t m_firstArg;
	uint8_t m_SecondArg;
	uint32_t m_freq;
#endif
};

BOOL	ELTimer_Init();

DWORD	ELTimer_GetMSec();

VOID	ELTimer_SetServerMSec(DWORD dwServerTime);
DWORD	ELTimer_GetServerMSec();

VOID	ELTimer_SetFrameMSec();
DWORD	ELTimer_GetFrameMSec();