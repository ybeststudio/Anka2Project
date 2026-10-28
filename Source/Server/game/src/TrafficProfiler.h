#ifndef _METIN_II_TRAFFICPROFILER_H_
#define _METIN_II_TRAFFICPROFILER_H_

class TrafficProfiler : public singleton< TrafficProfiler >
{
	public:
		enum IODirection
		{
			IODIR_INPUT = 0,
			IODIR_OUTPUT,
			IODIR_MAX
		};

	public:
		TrafficProfiler( void );
		~TrafficProfiler( void );

		bool Initialize( DWORD dwFlushCycle, const char* pszLogFileName );
		bool Report( IODirection dir, BYTE byHeader, DWORD dwSize )
		{
			ComputeTraffic( dir, byHeader, dwSize );
			if ( (DWORD)(time( NULL ) - m_tmProfileStartTime) >= m_dwFlushCycle )
				return Flush();
			return true;
		}

		bool Flush( void );

	private:
		void InitializeProfiling( void );
		void ComputeTraffic( IODirection dir, BYTE byHeader, DWORD dwSize )
		{
			TrafficInfo& rTrafficInfo = m_aTrafficVec[ dir ][ byHeader ];

			m_dwTotalTraffic += dwSize;
			m_dwTotalPacket += !rTrafficInfo.second;

			rTrafficInfo.first += dwSize;
			rTrafficInfo.second++;
		}

		typedef std::pair< DWORD, DWORD > TrafficInfo;
		typedef std::vector< TrafficInfo > TrafficVec;

		FILE* m_pfProfileLogFile;
		DWORD m_dwFlushCycle;
		time_t m_tmProfileStartTime;
		DWORD m_dwTotalTraffic;
		DWORD m_dwTotalPacket;
		TrafficVec m_aTrafficVec[ IODIR_MAX ];
};

#endif
