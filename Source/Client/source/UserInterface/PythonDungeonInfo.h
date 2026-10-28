#pragma once
#include "Packet.h"

#ifdef ENABLE_DUNGEON_INFO
class CPythonDungeonInfo : public CSingleton<CPythonDungeonInfo>
{

	public:
		enum 
		{
			PAGE_VIEWS 	= 13,
		};

		typedef std::vector<TFDateInfoPacket> TDungeonInfo;
		typedef std::vector<TFDateInfoRanking> TDungeonRankingInfo;
		typedef std::vector<TFDateMision> TDungeonMisionInfo;

	public:
		CPythonDungeonInfo();
		virtual ~CPythonDungeonInfo();


		void 	Open();
		void 	Clear();
		void 	ClearMision();
		void 	DateInfoList(const TFDateInfoPacket & infodungeon);
		int 	GetSizeList();

		int 	GetPageView(){return PAGE_VIEWS;}

		void 	SetPageCount(int index){page_count = index;}
		int 	GetPageCount(){return page_count;}


		const TDungeonInfo & GetDateInfo();

		void 	ClearRanking();
		void 	DateInfoRankingList(const TFDateInfoRanking & infodungeon_ranking);
		int 	GetSizeRankingList();
		const TDungeonRankingInfo & GetDateInfoRanking();

		
		const TDungeonMisionInfo & GetDateInfoMision();

		void 	DateInfoMisionList(const TFDateMision & infodungeonm);


	private:
		int 	page_count;
		TDungeonMisionInfo date_dungeon_mision;
		TDungeonInfo date_dungeon;
		TDungeonRankingInfo date_dungeon_ranking;
};
#endif // ENABLE_DUNGEON_INFO
