#ifndef __INC_METIN_II_GAME_SECTREE_MANAGER_H__
#define __INC_METIN_II_GAME_SECTREE_MANAGER_H__

#include "sectree.h"

typedef struct SMapRegion
{
	int			index;
	int			sx, sy, ex, ey;
	PIXEL_POSITION	posSpawn;

	bool		bEmpireSpawnDifferent;
	PIXEL_POSITION	posEmpire[3];

	std::string		strMapName;
} TMapRegion;

#ifdef ENABLE_CONQUEROR_LEVEL
typedef struct SSungmaAffectMap{
	long lMapIndex;
	int iAffecType;
	int iValue;
}TSungmaAffectMap;
#endif

struct TAreaInfo
{
	int sx, sy, ex, ey, dir;
	TAreaInfo(int sx, int sy, int ex, int ey, int dir)
		: sx(sx), sy(sy), ex(ex), ey(ey), dir(dir)
		{}
};

struct npc_info
{
	BYTE bType;
#ifdef ENABLE_ULTIMATE_REGEN
	DWORD mobVnum;
#else
	const char* name;
#endif
	long x, y;
#ifdef ENABLE_ULTIMATE_REGEN
	int regenTime;
#endif

#ifdef ENABLE_ULTIMATE_REGEN
	npc_info(BYTE bType, DWORD vnum, long x, long y, int iTime) : bType(bType), mobVnum(vnum), x(x), y(y), regenTime(iTime) {}
#else
	npc_info(BYTE bType, const char* name, long x, long y) : bType(bType), name(name), x(x), y(y) {}
#endif
};

typedef std::map<std::string, TAreaInfo> TAreaMap;

typedef struct SSetting
{
	int			iIndex;
	int			iCellScale;
	int			iBaseX;
	int			iBaseY;
	int			iWidth;
	int			iHeight;

	PIXEL_POSITION	posSpawn;
} TMapSetting;

class SECTREE_MAP
{
	public:
		typedef std::map<DWORD, LPSECTREE> MapType;

		SECTREE_MAP();
		SECTREE_MAP(SECTREE_MAP & r);
		virtual ~SECTREE_MAP();

		bool Add(DWORD key, LPSECTREE sectree)
		{
			return map_.insert(MapType::value_type(key, sectree)).second;
		}

		LPSECTREE	Find(DWORD dwPackage);
		LPSECTREE	Find(DWORD x, DWORD y);
		void		Build();
#ifdef ENABLE_DRAGON_LAIR
		void SetFlag(std::string flagname, int value)
		{
			itertype(m_entity_Flag) it =  m_entity_Flag.find(flagname);
			if (it != m_entity_Flag.end())
			{
				it->second = value;
			}
			else
			{
				m_entity_Flag.insert(make_pair(flagname, value));
			}
		}
		int GetFlag(std::string flagname)
		{
			itertype(m_entity_Flag) it =  m_entity_Flag.find(flagname);
			if (it != m_entity_Flag.end())
			{
				return it->second;
			}
			return 0;
		}
		std::map<std::string, int>  m_entity_Flag;
#endif
		TMapSetting	m_setting;

		template< typename Func >
		void for_each( Func & rfunc )
		{
			// <Factor> Using snapshot copy to avoid side-effects
			FCollectEntity collector;
			std::map<DWORD, LPSECTREE>::iterator it = map_.begin();
			for ( ; it != map_.end(); ++it)
			{
				LPSECTREE sectree = it->second;
				sectree->for_each_entity(collector);
			}
			collector.ForEach(rfunc);
		}

		void DumpAllToSysErr()
		{
			SECTREE_MAP::MapType::iterator i;
			for (i = map_.begin(); i != map_.end(); ++i)
			{
				sys_err("SECTREE %x(%u, %u)", i->first, i->first & 0xffff, i->first >> 16);
			}
		}

	private:
		MapType map_;
};

enum EAttrRegionMode
{
	ATTR_REGION_MODE_SET,
	ATTR_REGION_MODE_REMOVE,
	ATTR_REGION_MODE_CHECK,
};

class SECTREE_MANAGER : public singleton<SECTREE_MANAGER>
{
	public:
		SECTREE_MANAGER();
		virtual ~SECTREE_MANAGER();

		LPSECTREE_MAP GetMap(long lMapIndex);
		LPSECTREE Get(DWORD dwIndex, DWORD package);
		LPSECTREE Get(DWORD dwIndex, DWORD x, DWORD y);

		template<typename Func>
		void for_each(int iMapIndex, Func & rfunc)
		{
			LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(iMapIndex);
			if (pSecMap)
			{
				pSecMap->for_each(rfunc);
			}
		}
#ifdef ENABLE_CONQUEROR_LEVEL
		void		LoadSungmaAttr(const char * c_pszFileName, long lMapIndex);
		int		GetSungmaValueAffectByRegion(long lMapIndex, int iAffecType);
#endif

		int LoadSettingFile(long lIndex, const char * c_pszSettingFileName, TMapSetting & r_setting);
		bool LoadMapRegion(const char * c_pszFileName, TMapSetting & r_Setting, const char * c_pszMapName);
		int Build(const char * c_pszListFileName, const char* c_pszBasePath);
		LPSECTREE_MAP BuildSectreeFromSetting(TMapSetting & r_setting);
		bool LoadAttribute(LPSECTREE_MAP pkMapSectree, const char * c_pszFileName, TMapSetting & r_setting);
		void LoadDungeon(int iIndex, const char * c_pszFileName);
		bool GetValidLocation(long lMapIndex, long x, long y, long & r_lValidMapIndex, PIXEL_POSITION & r_pos, BYTE empire = 0);
		bool GetSpawnPosition(long x, long y, PIXEL_POSITION & r_pos);
		bool GetSpawnPositionByMapIndex(long lMapIndex, PIXEL_POSITION & r_pos);
		bool GetRecallPositionByEmpire(int iMapIndex, BYTE bEmpire, PIXEL_POSITION & r_pos);

		const TMapRegion *GetMapRegion(long lMapIndex);
		int GetMapIndex(long x, long y);
		const TMapRegion *FindRegionByPartialName(const char* szMapName);

		bool GetMapBasePosition(long x, long y, PIXEL_POSITION & r_pos);
		bool GetMapBasePositionByMapIndex(long lMapIndex, PIXEL_POSITION & r_pos);
		bool GetMovablePosition(long lMapIndex, long x, long y, PIXEL_POSITION & pos);
		bool IsMovablePosition(long lMapIndex, long x, long y);
		bool GetCenterPositionOfMap(long lMapIndex, PIXEL_POSITION & r_pos);
		bool GetRandomLocation(long lMapIndex, PIXEL_POSITION & r_pos, DWORD dwCurrentX = 0, DWORD dwCurrentY = 0, int iMaxDistance = 0);

		long CreatePrivateMap(long lMapIndex);
		void DestroyPrivateMap(long lMapIndex);

		TAreaMap& GetDungeonArea(long lMapIndex);
		void SendNPCPosition(LPCHARACTER ch);

#ifdef ENABLE_ULTIMATE_REGEN
		void InsertNPCPosition(long lMapIndex, BYTE bType, DWORD mobVnum, long x, long y, int regenTime);
#else
		void InsertNPCPosition(long lMapIndex, BYTE bType, const char* szName, long x, long y);
#endif

		BYTE GetEmpireFromMapIndex(long lMapIndex);

		void PurgeMonstersInMap(long lMapIndex);
		void PurgeStonesInMap(long lMapIndex);
		void PurgeNPCsInMap(long lMapIndex);
		size_t GetMonsterCountInMap(long lMapIndex);
#ifdef __SYSTEM_SEARCH_ITEM_MOB__
		size_t GetMonsterCountSpawned(DWORD dwVnum);
#endif
		size_t GetMonsterCountInMap(long lMpaIndex, DWORD dwVnum);

		bool ForAttrRegion(long lMapIndex, long lStartX, long lStartY, long lEndX, long lEndY, long lRotate, DWORD dwAttr, EAttrRegionMode mode);

#ifdef ENABLE_NPC_LOCATION_TRACE
		bool GetNpcLocationByVnum(long lMapIndex, DWORD npcVnum, std::vector<std::pair<long, long>>& positions);
#endif
#ifdef ENABLE_OCHAO_TEMPLE_SYSTEM
		void GetRestartCityPos(int iMapIndex, int iEmpire, int &iTargetX, int &iTargetY, int &iTargetZ);
		void AddRestartCityPos(int iMapIndex = 0, int iEmpire = 0, int iX = 0, int iY = 0, int iZ = 0);
#endif
		bool SaveAttributeToImage(int lMapIndex, const char * c_pszFileName, LPSECTREE_MAP pMapSrc = NULL);

		/// Executes specific processing for Sectree's Attribute for the area.
		/**
		* @param [in] lMapIndex Map index to apply
		* @param [in] lStartX The leftmost coordinate of the rectangular area.
		* @param [in] lStartY The top coordinate of the rectangle area.
		* @param [in] lEndX Rightmost coordinate of the rectangular area
		* @param [in] lEndY The coordinates of the bottom of the rectangle
		* @param [in] lRotate The angle to rotate about the region.
		* @param [in] dwAttr Attribute to apply
		* @param [in] Type to be processed for mode Attribute
		*/
	private:
		bool ForAttrRegionRightAngle( long lMapIndex, long lCX, long lCY, long lCW, long lCH, long lRotate, DWORD dwAttr, EAttrRegionMode mode );
		bool ForAttrRegionFreeAngle( long lMapIndex, long lCX, long lCY, long lCW, long lCH, long lRotate, DWORD dwAttr, EAttrRegionMode mode );
		bool ForAttrRegionCell( long lMapIndex, long lCX, long lCY, DWORD dwAttr, EAttrRegionMode mode );

		static WORD current_sectree_version;
		std::map<DWORD, LPSECTREE_MAP> m_map_pkSectree;
		std::map<int, TAreaMap> m_map_pkArea;
		std::vector<TMapRegion> m_vec_mapRegion;
#ifdef ENABLE_CONQUEROR_LEVEL
		std::vector<TSungmaAffectMap>		m_vec_mapSungmaAffect;
#endif
		std::map<DWORD, std::vector<npc_info> > m_mapNPCPosition;

	// <Factor> Circular private map indexing
		typedef std::unordered_map<long, int> PrivateIndexMapType;
		PrivateIndexMapType next_private_index_map_;
};
#endif
