
#include <stdint.h>
#include "../../common/stl.h"

class CMapLocation : public singleton<CMapLocation>
{
	public:
		typedef struct SLocation
		{
			int32_t     addr;
			WORD        port;
		} TLocation;

		bool    Get(long x, long y, long & lMapIndex, int32_t & lAddr, WORD & wPort);
		bool	Get(int iIndex, int32_t & lAddr, WORD & wPort);
		void    Insert(long lIndex, const char * c_pszHost, WORD wPort);

	protected:
		std::map<long, TLocation> m_map_address;
};

