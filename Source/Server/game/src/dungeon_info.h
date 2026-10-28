#include <boost/unordered_map.hpp>
#include "../common/stl.h"
#include "../common/tables.h"

class CDungeonInfo
{
	public:
		CDungeonInfo ();
		virtual ~CDungeonInfo ();
};


class CDungeonInfoExtern : public singleton<CDungeonInfoExtern>
{

	enum
	{
		ID_TORRE_DESAFIO = 27,
		ID_TORRE_DESAFIO_REWORK = 28,
	};

	public:
		CDungeonInfoExtern ();
		virtual ~CDungeonInfoExtern ();

		void 	Open(LPCHARACTER ch);
		void 	SendDateDungeon(LPCHARACTER ch);
		void 	LoadDateDungeon();

		void 	Teleport(LPCHARACTER ch, int index_teleport);

		void 	SendDungeonInfoPackets(LPCHARACTER ch, BYTE subheader, int index = 0);
		bool 	GetIdDungeon(int id_map_dungeon);

		long 	GetIndexMapDungeon(int id_map_dungeon);
		int 	GetXMapDungeon(int id_map_dungeon);
		int 	GetYMapDungeon(int id_map_dungeon);
		int 	GetTimeRoomDungeon(int id_map_dungeon);		
		int 	GetMinLevel(int id_map_dungeon);
		int 	GetMaxLevel(int id_map_dungeon);
		int 	GetPartyMembers(int id_map_dungeon);
		int 	GetIdWithMapIndex(int index_map_dungeon);
		int 	GetTimeRespawnDungeon(int id_map_dungeon);
		int 	GetVnumItem(int id_map_dungeon);
		int 	GetCountItem(int id_map_dungeon);
		int 	GetIdBoss(int index_map_dungeon);

		bool 	CheckTimeDungeonRoom(LPCHARACTER ch, long index_map);
		void	SetTimeDungeonRoom(LPCHARACTER ch, long index_map);
		

		int 	GetTimeRespawnDungeonActual(LPCHARACTER ch, long index_map);
		int 	GetTimeRoomDungeonActual(LPCHARACTER ch, long index_map);

		void 	CheckBossKill(LPCHARACTER ch);
		void 	KillBossDungeon(LPCHARACTER ch);
		bool 	CheckBossKillMap(int index_map_dungeon);


		int 	GetFinishTotalDungeon(LPCHARACTER ch, int id_boss);
		int 	GetTimeFinishTotalDungeon(LPCHARACTER ch, long index_map);


		bool 	CheckTimeDungeonRespawn(LPCHARACTER ch);

		void 	StartCheckTimeRoom(LPCHARACTER ch);

		void 	TestTimeRespawn(LPCHARACTER ch);

		int 	GetDamageDoneDungeon(LPCHARACTER ch, int id_boss);
		int 	GetDamageReceivedDungeon(LPCHARACTER ch, int id_boss);

		void 	SetInfoRanking(LPCHARACTER ch, int map_index);
		void 	LoadDateDungeonRanking();
		void 	SetDateDungeonRanking(LPCHARACTER ch, int index);

		void 	StatusMision(LPCHARACTER ch,int id_boss,int status);
		int 	GetStatusMision(LPCHARACTER ch, int id_boss);
		void 	ResetMision(LPCHARACTER ch, int id_boss);
		int 	GetCountMision(LPCHARACTER ch, int id_boss);
		void 	SetCountMision(LPCHARACTER ch, int id_boss);
		bool 	GetVnumMobMision(int vnum_mob);
		int 	GetCountMobMision(int vnum_mob);
		int 	GetVnumItemWithMobM(int vnum_mob);
		void 	GetLlaveDungeon(LPCHARACTER ch,int vnum_mob);
		int 	GetLvMinWithMobM(int vnum_mob);
		void 	SetDateDungeonMision(LPCHARACTER ch);

		int 	GetIdBossWithMobM(int vnum_mob);
		int 	GetItemCheckDungeonActual(LPCHARACTER ch, long index_map);

		void 	SetTimeRespawn(LPCHARACTER ch);
		void 	StartGiveItemDungeon(LPCHARACTER ch, DWORD vnum, BYTE count);

	private:

		LPEVENT DungeonInfoUpdateTime;


		std::vector<SFDateInfo> date_dungeon;
		std::vector<SFDateRanking> date_dungeon_ranking;
		boost::unordered_map<DWORD, CDungeonInfo*> ListDungeonInfo;
		

};

