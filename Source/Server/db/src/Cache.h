#ifndef __INC_DB_CACHE_H__
#define __INC_DB_CACHE_H__

#include "../../common/cache.h"
#include "../../common/service.h"

class CItemCache : public cache<TPlayerItem>
{
	public:
		CItemCache();
		virtual ~CItemCache();

		void Delete();
		virtual void OnFlush();
};

class CPlayerTableCache : public cache<TPlayerTable>
{
	public:
		CPlayerTableCache();
		virtual ~CPlayerTableCache();

		virtual void OnFlush();

		DWORD GetLastUpdateTime() { return m_lastUpdateTime; }
};

#ifdef ENABLE_SKILL_COLOR_SYSTEM
class CSKillColorCache : public cache<TSkillColor>
{
	public:
		CSKillColorCache();
		virtual ~CSKillColorCache();

		virtual void OnFlush();
};
#endif

class CItemPriceListTableCache : public cache< TItemPriceListTable >
{
	public:
		CItemPriceListTableCache(void);

		void UpdateList(const TItemPriceListTable* pUpdateList);
		virtual void OnFlush(void);

	private:
		static const int s_nMinFlushSec;
};

#ifdef ENABLE_GROWTH_PET_SYSTEM
class CGrowthPetCache : public cache<TGrowthPet>
{
	public:
		CGrowthPetCache();
		virtual ~CGrowthPetCache();
		void Delete();
		virtual void OnFlush();
};
#endif
#endif
