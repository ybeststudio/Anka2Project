#include "../../common/service.h"

#ifdef ENABLE_WHITE_DRAGON
#include "../common/length.h"
#include "../common/item_length.h"
#include "../common/tables.h"
#include "guild.h"
#include "char_manager.h"
#include "sectree_manager.h"

namespace WhiteDragon
{
	enum eWhiteDragonConfig
	{
		WHITE_EGG_VNUM = 4030,
		BOSS_EASY_VNUM = 6791,
		BOSS_VNUM = 6790,
		TIME_LIMIT_DUNGEON = 1800,

		// Monster
		WHITEDRAGON_SOLDIER = 6784,	// Kuzey Rüzgarý Sýðýnaðý'nýn askeri
		WHITEDRAGON_SHELTER = 6785,	// Horts'un savaþçýsý
		WHITEDRAGON_WARRIOR = 6786,	// Kuzey Rüzgarý Kabilesi Savaþçýsý
		WHITEDRAGON_WARLOCK = 6787,	// Kuzey Rüzgarý Büyücüsü
		WHITEDRAGON_GENERAL = 6788,	// Kuzey Rüzgarý Karargahý Komutaný
	};

	extern int iSungmaWhitePoint[4][3];

#ifdef ENABLE_YOHARA_SYSTEM
	typedef struct SSungmaWhitePoint
	{
		bool bDungeonLevel;
		int iPointType;
		int iValue;
	} TSungmaWhitePoint;
#endif

	class CWhDrMap
	{
	public:
		CWhDrMap(long lMapIndex, bool bHard);
		~CWhDrMap();

		void Destroy();

#ifdef ENABLE_YOHARA_SYSTEM
		void LoadSungmaAttr();
		int GetSungmaValuePoint(int iPoint);
#endif

		void StartDungeon(LPCHARACTER pkChar);
		void OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller);
		void EndDungeonWarp();
		void Start();

		LPCHARACTER Spawn(uint32_t dwVnum, int iX, int iY, int iDir, bool bSpawnMotion = false);

		/* Get and Set */
		void SetDungeonStep(uint8_t bStep);
		uint8_t GetDungeonStep() { return dungeon_step; };

		void SetDungeonLevel(bool bLevel) { dungeon_level = bLevel; };
		bool GetDungeonLevel() { return dungeon_level; };

		void SetParty(LPPARTY pParty) { pPartyReg = pParty; };
		LPPARTY& GetParty() { return pPartyReg; };

		void SetMapSectree(LPSECTREE_MAP pkSectree) { pkSectreeMap = pkSectree; };
		LPSECTREE_MAP& GetMapSectree() { return pkSectreeMap; };

		void SetMapIndex(long lMapIndex) { map_index = lMapIndex; };
		long GetMapIndex() { return map_index; };

		void SetWhiteEggNPC(LPCHARACTER pkWhiteEgg) { pkWhiteEggNPC = pkWhiteEgg; };
		LPCHARACTER& GetWhiteEggNPC() { return pkWhiteEggNPC; };

		long map_index;
		LPSECTREE_MAP pkSectreeMap;
		LPPARTY pPartyReg;
		uint8_t dungeon_step;
		bool dungeon_level;

		LPCHARACTER pkWhiteEggNPC;

	protected:
		LPEVENT e_SpawnEvent;
		LPEVENT e_SpawnMonsterEvent;
		LPEVENT e_LimitEvent;

#ifdef ENABLE_YOHARA_SYSTEM
	private:
		std::vector<TSungmaWhitePoint> m_vec_mapSungmaWhitePoint;
#endif
	};

	class CWhDr : public singleton<CWhDr>
	{
	public:
		void Initialize();
		void Destroy();

		void Remove(long lMapIndex);
		bool Access(LPCHARACTER pChar, bool bHard);

#ifdef ENABLE_YOHARA_SYSTEM
		int GetSungmaWhiteDungeonValue(const CHARACTER* pkChar, int iAffect) const;
#endif

		bool IsWhiteMap(long lMapIndex);
		void OnKill(LPCHARACTER pkMonster, LPCHARACTER pKiller);
		void StartDungeon(LPCHARACTER pkChar);
		void Start(LPCHARACTER pkChar);

		//Set
		void SetSubXYZ(long lX, long lY, long lZ) { lSubMapPos.x = lX, lSubMapPos.y = lY, lSubMapPos.z = lZ; }
		void SetXYZ(long lX, long lY, long lZ) { lMapCenterPos.x = lX, lMapCenterPos.y = lY, lMapCenterPos.z = lZ; }

		//Get
		const PIXEL_POSITION& GetSubXYZ() const { return lSubMapPos; }
		const PIXEL_POSITION& GetXYZ() const { return lMapCenterPos; }

	private:
		std::map<long, CWhDrMap*> m_dwRegGroups;
		PIXEL_POSITION lMapCenterPos, lSubMapPos;
	};
};
#endif
