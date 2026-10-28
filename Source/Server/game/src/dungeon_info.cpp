#include "stdafx.h"
#include "dungeon_info.h"
#include "entity.h"
#include "sectree_manager.h"
#include "char.h"
#include "locale_service.h"
#include "regen.h"
#include "log.h"
#include "db.h"
#include "utils.h"
#include "guild.h"
#include "packet.h"
#include "desc_client.h"
#include "start_position.h"
#include "item.h"
#include "item_manager.h"
#include "mob_manager.h"
#ifdef BATTLEPASS_WORLDARD
#include "battlepass.h"
#endif
#ifdef ENABLE_DUNGEON_INFO

extern int passes_per_sec;


CDungeonInfo::CDungeonInfo()
{
}

CDungeonInfo::~CDungeonInfo()
{

}

CDungeonInfoExtern::CDungeonInfoExtern()
{

}

CDungeonInfoExtern::~CDungeonInfoExtern()
{
	
}

void CDungeonInfoExtern::LoadDateDungeon()
{
	char szQuery[1024];
	snprintf(szQuery, sizeof(szQuery),
	"SELECT id, name, img, lv_min, lv_max, difficulty, party_max, respawn, time_room, entrance_dungeon, resistance_dungeon, force_dungeon, vnum_item, count_item, index_map, x_dungeon_join, y_dungeon_join, id_boss, x_map_npc, y_map_npc, vnum_mob_mision, count_mob_mision  FROM player.date_dungeon");
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows > 0){

		date_dungeon.clear();

		for (int i = 0; i < mysql_num_rows(pMsg->Get()->pSQLResult); ++i)
		{
			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

			int col = 0;
			SFDateInfo InfoDate;
			
			str_to_number(InfoDate.id, row[col++]);

			strlcpy(InfoDate.dateinfo.name, row[col] ? row[col] : "", sizeof(InfoDate.dateinfo.name)); col++;
			strlcpy(InfoDate.dateinfo.img, row[col] ? row[col] : "", sizeof(InfoDate.dateinfo.img)); col++;

			str_to_number(InfoDate.dateinfo.lv_min, row[col++]);
			str_to_number(InfoDate.dateinfo.lv_max, row[col++]);

			str_to_number(InfoDate.dateinfo.difficulty, row[col++]);

			str_to_number(InfoDate.dateinfo.party_max, row[col++]);
			str_to_number(InfoDate.dateinfo.respawn, row[col++]);
			str_to_number(InfoDate.dateinfo.time_room, row[col++]);

			strlcpy(InfoDate.dateinfo.entrance, row[col] ? row[col] : "", sizeof(InfoDate.dateinfo.entrance)); col++;
			strlcpy(InfoDate.dateinfo.resistance, row[col] ? row[col] : "", sizeof(InfoDate.dateinfo.resistance)); col++;
			strlcpy(InfoDate.dateinfo.force, row[col] ? row[col] : "", sizeof(InfoDate.dateinfo.force)); col++;

			str_to_number(InfoDate.dateinfo.vnum_item, row[col++]);
			str_to_number(InfoDate.dateinfo.count_item, row[col++]);

			str_to_number(InfoDate.index_map, row[col++]);
			str_to_number(InfoDate.x_dungeon_join, row[col++]);
			str_to_number(InfoDate.y_dungeon_join, row[col++]);

			str_to_number(InfoDate.dateinfo.id_boss, row[col++]);
			str_to_number(InfoDate.x_map_npc, row[col++]);
			str_to_number(InfoDate.y_map_npc, row[col++]);

			str_to_number(InfoDate.dateinfom.vnum_mob_mision, row[col++]);
			str_to_number(InfoDate.dateinfom.count_mob_mision, row[col++]);

			date_dungeon.push_back(InfoDate);
		}
	}
}


void CDungeonInfoExtern::Open(LPCHARACTER ch)
{
	SendDateDungeon(ch);
	SendDungeonInfoPackets(ch,DUNGEON_INFO_SUB_HEADER_OPEN_SEND);
}


void CDungeonInfoExtern::Teleport(LPCHARACTER ch,int index_teleport)
{
	if (index_teleport < 0){
		return;
	}

	if (date_dungeon.size() < 0){
		return;
	}

	if (ch->GetLevel() < GetMinLevel(index_teleport)){
		return;
	}

	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == index_teleport){
				ch->WarpSet(date_dungeon[i].x_map_npc, date_dungeon[i].y_map_npc);
			}
		}
	}
	return;
}

void CDungeonInfoExtern::SendDateDungeon(LPCHARACTER ch)
{
	SendDungeonInfoPackets(ch,DUNGEON_INFO_SUB_HEADER_DATES_CLEAR);

	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			SendDungeonInfoPackets(ch,DUNGEON_INFO_SUB_HEADER_DATES_SEND,i);
			SendDungeonInfoPackets(ch,DUNGEON_INFO_SUB_HEADER_DATES_MISION_SEND,i);
		}
	}

}

void CDungeonInfoExtern::SetDateDungeonRanking(LPCHARACTER ch, int index)
{
	SendDungeonInfoPackets(ch,DUNGEON_INFO_SUB_HEADER_DATES_RANKING_CLEAR);
	if (date_dungeon_ranking.size() > 0){
		for (int i = 0; i<date_dungeon_ranking.size(); ++i){
			if (date_dungeon_ranking[i].vnum_mob_r == index){
				SendDungeonInfoPackets(ch,DUNGEON_INFO_SUB_HEADER_DATES_RANKING_SEND,i);
			}
		}
	}

	SendDungeonInfoPackets(ch,DUNGEON_INFO_SUB_HEADER_DATES_RANKING_LOAD);

}


bool CDungeonInfoExtern::GetIdDungeon(int id_map_dungeon)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == id_map_dungeon)
			{
				return true;	
			}
		}
	}
	return false;
}

long CDungeonInfoExtern::GetIndexMapDungeon(int id_map_dungeon)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == id_map_dungeon)
			{
				return date_dungeon[i].index_map;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetXMapDungeon(int id_map_dungeon)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == id_map_dungeon)
			{
				return date_dungeon[i].x_dungeon_join;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetYMapDungeon(int id_map_dungeon)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == id_map_dungeon)
			{
				return date_dungeon[i].y_dungeon_join;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetMinLevel(int id_map_dungeon)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == id_map_dungeon)
			{
				return date_dungeon[i].dateinfo.lv_min;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetMaxLevel(int id_map_dungeon)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == id_map_dungeon)
			{
				return date_dungeon[i].dateinfo.lv_max;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetPartyMembers(int id_map_dungeon)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == id_map_dungeon)
			{
				return date_dungeon[i].dateinfo.party_max;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetTimeRoomDungeon(int id_map_dungeon)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == id_map_dungeon)
			{
				return date_dungeon[i].dateinfo.time_room;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetTimeRespawnDungeon(int id_map_dungeon)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == id_map_dungeon)
			{
				return date_dungeon[i].dateinfo.respawn;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetVnumItem(int id_map_dungeon)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == id_map_dungeon)
			{
				return date_dungeon[i].dateinfo.vnum_item;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetCountItem(int id_map_dungeon)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].id == id_map_dungeon)
			{
				return date_dungeon[i].dateinfo.count_item;	
			}
		}
	}
	return 0;
}



int CDungeonInfoExtern::GetIdWithMapIndex(int index_map_dungeon)
{

	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].index_map == index_map_dungeon)
			{
				return date_dungeon[i].id;	
			}
		}
	}
	return 0;
}


int CDungeonInfoExtern::GetIdBoss(int index_map_dungeon)
{
	index_map_dungeon = index_map_dungeon/10000;
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].index_map == index_map_dungeon)
			{
				return date_dungeon[i].dateinfo.id_boss;	
			}
		}
	}
	return 0;
}


bool CDungeonInfoExtern::CheckBossKillMap(int index_map_dungeon)
{

	index_map_dungeon = index_map_dungeon/10000;
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].index_map == index_map_dungeon)
			{
				return true;	
			}
		}
	}
	return false;
}
void CDungeonInfoExtern::CheckBossKill(LPCHARACTER ch)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			char dungeon_boss_room[1024];
			snprintf(dungeon_boss_room, sizeof(dungeon_boss_room), "dungeon_info.dungeon_boss_room_%d", date_dungeon[i].index_map);

			if(ch->GetQuestFlag(dungeon_boss_room) == 1){
				ch->SetQuestFlag(dungeon_boss_room,0);
			}
			
			if(ch->GetQuestFlag(dungeon_boss_room) == 2){

				char dungeon_time_room[1024];
				char dungeon_time_respawn[1024];

				snprintf(dungeon_time_room, sizeof(dungeon_time_room), "dungeon_info.dungeon_time_room_%d", date_dungeon[i].index_map);
				ch->SetQuestFlag(dungeon_time_room, get_global_time());

				snprintf(dungeon_time_respawn, sizeof(dungeon_time_respawn), "dungeon_info.dungeon_time_respawn_%d", date_dungeon[i].index_map);
				ch->SetQuestFlag(dungeon_time_respawn,get_global_time() + GetTimeRespawnDungeon(GetIdWithMapIndex(date_dungeon[i].index_map)));

				ch->SetQuestFlag(dungeon_boss_room,0);

			}
		}
	}

}

struct FKillBossDungeon
{
	void operator() (LPENTITY ent)
	{
		if (NULL != ent)
		{
			LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);

			if (NULL != pChar)
			{
				if (true == pChar->IsPC())
				{
					int map_index = pChar->GetMapIndex()/10000;
					char dungeon_boss_room[1024];
					snprintf(dungeon_boss_room, sizeof(dungeon_boss_room), "dungeon_info.dungeon_boss_room_%d", map_index);
					pChar->SetQuestFlag(dungeon_boss_room, 2);

					char dungeon_total_finish[1024];
					snprintf(dungeon_total_finish, sizeof(dungeon_total_finish), "dungeon_info.dungeon_finish_total_%d", CDungeonInfoExtern::instance().GetIdBoss(pChar->GetMapIndex()));
					pChar->SetQuestFlag(dungeon_total_finish,pChar->GetQuestFlag(dungeon_total_finish)+1);

					char dungeon_time_room[1024];
					snprintf(dungeon_time_room, sizeof(dungeon_time_room), "dungeon_info.dungeon_time_room_%d", map_index);

					int time_room_total = CDungeonInfoExtern::instance().GetTimeRoomDungeon(CDungeonInfoExtern::instance().GetIdWithMapIndex(map_index));
					int time_room_restante = pChar->GetQuestFlag(dungeon_time_room) - get_global_time();

					char dungeon_time_finish[1024];
					snprintf(dungeon_time_finish, sizeof(dungeon_time_finish), "dungeon_info.dungeon_time_finish_w_%d", map_index);

					if (pChar->GetQuestFlag(dungeon_time_finish) > 0){
						if ((time_room_total-time_room_restante) < pChar->GetQuestFlag(dungeon_time_finish))
						{
							pChar->SetQuestFlag(dungeon_time_finish,time_room_total-time_room_restante);
						}
					}
					else{
						pChar->SetQuestFlag(dungeon_time_finish,time_room_total-time_room_restante);
					}

					char dungeon_damage_done[1024];
					snprintf(dungeon_damage_done, sizeof(dungeon_damage_done), "dungeon_info.dungeon_damage_done_%d", CDungeonInfoExtern::instance().GetIdBoss(pChar->GetMapIndex()));

					if (pChar->GetDamageDoneDungeonInfo() > pChar->GetQuestFlag(dungeon_damage_done))
					{
						pChar->SetQuestFlag(dungeon_damage_done,pChar->GetDamageDoneDungeonInfo());
					}


					char dungeon_damage_received[1024];
					snprintf(dungeon_damage_received, sizeof(dungeon_damage_received), "dungeon_info.dungeon_damage_received_%d", CDungeonInfoExtern::instance().GetIdBoss(pChar->GetMapIndex()));

					if (pChar->GetDamageReceivedDungeonInfo() > pChar->GetQuestFlag(dungeon_damage_received))
					{
						pChar->SetQuestFlag(dungeon_damage_received,pChar->GetDamageReceivedDungeonInfo());
					}

#ifdef BATTLEPASS_WORLDARD
					CBattlePass::instance().UpdateMisionBattlePassExtra(pChar,DUNGEON_COMPLETE_SPECIFIC_BP,CDungeonInfoExtern::instance().GetIdWithMapIndex(map_index),1);
#endif
					CDungeonInfoExtern::instance().SetInfoRanking(pChar,pChar->GetMapIndex());
					pChar->SetDamageDoneDungeonInfo(0);
					pChar->SetDamageReceivedDungeonInfo(0);
				}
			}
		}
	}
};


void CDungeonInfoExtern::SetInfoRanking(LPCHARACTER ch, int map_index)
{

	char szQuery[1024];
	char InsertSql[1024];


	int vnum_boss = GetIdBoss(map_index);
	int time_room = GetTimeFinishTotalDungeon(ch,map_index/10000);
	int max_danger = GetDamageDoneDungeon(ch,vnum_boss);
	int kills = GetFinishTotalDungeon(ch,vnum_boss);


	snprintf(szQuery, sizeof(szQuery),"SELECT kills,danger,time FROM player.ranking_dungeon WHERE id_player=%d and vnum_mob = %d",ch->GetPlayerID(),vnum_boss);
	std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
	if (pMsg->Get()->uiNumRows > 0)
	{ 

		int time_room_get;
		int max_danger_get;
		int kills_get;

		MYSQL_ROW  row = mysql_fetch_row(pMsg->Get()->pSQLResult);


		str_to_number(kills_get,row[0]);
		str_to_number(max_danger_get,row[1]);
		str_to_number(time_room_get,row[2]);

		if(kills > kills_get){
			DBManager::instance().Query("UPDATE player.ranking_dungeon SET kills = %d WHERE id_player=%d and vnum_mob = %d", kills , ch->GetPlayerID(), vnum_boss);
		}

		if(max_danger > max_danger_get){
			DBManager::instance().Query("UPDATE player.ranking_dungeon SET danger = %d WHERE id_player=%d and vnum_mob = %d", max_danger , ch->GetPlayerID(), vnum_boss);
		}

		if(time_room < time_room_get){
			DBManager::instance().Query("UPDATE player.ranking_dungeon SET time = %d WHERE id_player=%d and vnum_mob = %d", time_room , ch->GetPlayerID(), vnum_boss);
		}

	}else{
		snprintf(InsertSql, sizeof(InsertSql), "INSERT INTO player.ranking_dungeon (id_player,vnum_mob,name,kills,danger,time) VALUES(%d,%d,'%s',%d,%d,%d)", ch->GetPlayerID() , vnum_boss ,ch->GetName(), kills, max_danger , time_room);
		std::unique_ptr<SQLMsg> pmsg1(DBManager::instance().DirectQuery(InsertSql));
	}

}

void CDungeonInfoExtern::LoadDateDungeonRanking()
{
	date_dungeon_ranking.clear();
	const char* types[] = {"time","kills","danger"};
	const char* type2[] = {"ASC","DESC","DESC"};
	
	char szQuery[1024];
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			for (int itype = 0; itype < 3; ++itype)
			{
				snprintf(szQuery, sizeof(szQuery),"SELECT vnum_mob, name, %s FROM player.ranking_dungeon WHERE vnum_mob = %d ORDER BY %s %s LIMIT 9",types[itype],date_dungeon[i].dateinfo.id_boss,types[itype],type2[itype]);
				std::unique_ptr<SQLMsg> pMsg(DBManager::Instance().DirectQuery(szQuery));
				if (pMsg->Get()->uiNumRows > 0){
					for (int i = 0; i < mysql_num_rows(pMsg->Get()->pSQLResult); ++i)
					{
						MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

						int col = 0;
						SFDateRanking InfoDateRanking;
							
						str_to_number(InfoDateRanking.vnum_mob_r, row[col++]);
						strlcpy(InfoDateRanking.name_r, row[col] ? row[col] : "", sizeof(InfoDateRanking.name_r)); col++;
						str_to_number(InfoDateRanking.extra_r, row[col++]);
						InfoDateRanking.type = itype;

						date_dungeon_ranking.push_back(InfoDateRanking);
					}
				}
			}
		}
	}
}


void CDungeonInfoExtern::KillBossDungeon(LPCHARACTER ch)
{
	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());
	if (NULL != pMap){
		FKillBossDungeon f;
		pMap->for_each( f );
	}

}

bool CDungeonInfoExtern::CheckTimeDungeonRoom(LPCHARACTER ch, long index_map)
{
	index_map = index_map/10000;

	char dungeon_time_room[1024];
	snprintf(dungeon_time_room, sizeof(dungeon_time_room), "dungeon_info.dungeon_time_room_%ld", index_map);

	if(ch->GetQuestFlag(dungeon_time_room) - get_global_time() <= 0)
	{
		return true;
	}
	return false;
}



void CDungeonInfoExtern::SetTimeDungeonRoom(LPCHARACTER ch, long index_map)
{
	index_map = index_map/10000;

	char dungeon_time_room[1024];
	snprintf(dungeon_time_room, sizeof(dungeon_time_room), "dungeon_info.dungeon_time_room_%ld", index_map);

	ch->SetQuestFlag(dungeon_time_room, get_global_time() +  GetTimeRoomDungeon(GetIdWithMapIndex(index_map)));


	char dungeon_time_respawn[1024];
	snprintf(dungeon_time_respawn, sizeof(dungeon_time_respawn), "dungeon_info.dungeon_time_respawn_%ld", index_map);

	ch->SetQuestFlag(dungeon_time_respawn,get_global_time() +  GetTimeRoomDungeon(GetIdWithMapIndex(index_map)) + GetTimeRespawnDungeon(GetIdWithMapIndex(index_map)));


	char fix_item_check[1024];
	snprintf(fix_item_check, sizeof(fix_item_check), "dungeon_info.fix_item_check_%ld", index_map);
	ch->SetQuestFlag(fix_item_check,1);

	ch->SetQuestFlag("torre_destino.round_desafio",1);
}

void CDungeonInfoExtern::SetTimeRespawn(LPCHARACTER ch)
{
	char dungeon_time_room[1024];
	char dungeon_time_respawn[1024];

	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if(date_dungeon[i].id == ID_TORRE_DESAFIO || date_dungeon[i].id == ID_TORRE_DESAFIO_REWORK ){
				if(GetTimeRoomDungeonActual(ch,date_dungeon[i].index_map) > 0)
				{
					snprintf(dungeon_time_room, sizeof(dungeon_time_room), "dungeon_info.dungeon_time_room_%d", date_dungeon[i].index_map);
					ch->SetQuestFlag(dungeon_time_room, get_global_time());

					snprintf(dungeon_time_respawn, sizeof(dungeon_time_respawn), "dungeon_info.dungeon_time_respawn_%d", date_dungeon[i].index_map);
					ch->SetQuestFlag(dungeon_time_respawn,get_global_time() + GetTimeRespawnDungeon(GetIdWithMapIndex(date_dungeon[i].index_map)));	
				}
			}
		}
	}	
}

void CDungeonInfoExtern::StatusMision(LPCHARACTER ch,int id_boss,int status){

	char dungeon_status_mision1[1024];
	snprintf(dungeon_status_mision1, sizeof(dungeon_status_mision1), "dungeon_info.dungeon_s_m_%d", id_boss);

	ch->SetQuestFlag(dungeon_status_mision1,status);
}

int CDungeonInfoExtern::GetStatusMision(LPCHARACTER ch, int id_boss)
{
	char dungeon_status_mision[1024];
	snprintf(dungeon_status_mision, sizeof(dungeon_status_mision), "dungeon_info.dungeon_s_m_%d", id_boss);

	int status_mision = ch->GetQuestFlag(dungeon_status_mision);
	return status_mision;
}


int CDungeonInfoExtern::GetCountMision(LPCHARACTER ch, int id_boss)
{
	char dungeon_count_mision1[1024];
	snprintf(dungeon_count_mision1, sizeof(dungeon_count_mision1), "dungeon_info.dungeon_count_mision_%d", id_boss);

	int get_count_mision = ch->GetQuestFlag(dungeon_count_mision1);
	return get_count_mision;
}

void CDungeonInfoExtern::SetCountMision(LPCHARACTER ch, int id_boss)
{
	char dungeon_count_mision[1024];
	snprintf(dungeon_count_mision, sizeof(dungeon_count_mision), "dungeon_info.dungeon_count_mision_%d", id_boss);
	ch->SetQuestFlag(dungeon_count_mision,GetCountMision(ch,id_boss)+1);

	if(GetCountMision(ch,id_boss) >= GetCountMobMision(id_boss))
	{	

		const CMob * pkMob = CMobManager::instance().Get(GetIdBossWithMobM(id_boss));

		if (pkMob){
			ch->ChatPacket(CHAT_TYPE_NOTICE, "Has terminado la mision de llave para %s, ya puedes recogerla en el panel",pkMob->m_table.szLocaleName);
		}
	}

	SetDateDungeonMision(ch);


}



void CDungeonInfoExtern::ResetMision(LPCHARACTER ch, int id_boss)
{
	char dungeon_count_mision[1024];
	snprintf(dungeon_count_mision, sizeof(dungeon_count_mision), "dungeon_info.dungeon_count_mision_%d", id_boss);
	ch->SetQuestFlag(dungeon_count_mision,0);
	StatusMision(ch,id_boss,0);
}

bool CDungeonInfoExtern::GetVnumMobMision(int vnum_mob)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].dateinfom.vnum_mob_mision == vnum_mob)
			{
				return true;	
			}
		}
	}
	return false;
}

int CDungeonInfoExtern::GetCountMobMision(int vnum_mob)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].dateinfom.vnum_mob_mision == vnum_mob)
			{
				return date_dungeon[i].dateinfom.count_mob_mision;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetVnumItemWithMobM(int vnum_mob)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].dateinfom.vnum_mob_mision == vnum_mob)
			{
				return date_dungeon[i].dateinfo.vnum_item;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetLvMinWithMobM(int vnum_mob)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].dateinfom.vnum_mob_mision == vnum_mob)
			{
				return date_dungeon[i].dateinfo.lv_min;	
			}
		}
	}
	return 0;
}

int CDungeonInfoExtern::GetIdBossWithMobM(int vnum_mob)
{
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			if (date_dungeon[i].dateinfom.vnum_mob_mision == vnum_mob)
			{
				return date_dungeon[i].dateinfo.id_boss;	
			}
		}
	}
	return 0;
}


void CDungeonInfoExtern::GetLlaveDungeon(LPCHARACTER ch,int vnum_mob)
{

	if(vnum_mob <= 0){
		return;
	}

	if(GetStatusMision(ch,vnum_mob) <= 0){
		if(ch->GetLevel() < GetLvMinWithMobM(vnum_mob)){
			
			return;
		}

		StatusMision(ch,vnum_mob,1);
		SetDateDungeonMision(ch);
		return;
	}

	if(!GetVnumMobMision(vnum_mob)){
		return;
	}

	if(GetCountMision(ch,vnum_mob) >= GetCountMobMision(vnum_mob)){
		if(GetVnumItemWithMobM(vnum_mob) != 0){
			LPITEM pItem = ITEM_MANAGER::instance().CreateItem(GetVnumItemWithMobM(vnum_mob));
			if (!pItem)
			{
				sys_err("[GetLlaveDungeon] Item vnum: %u cannot be created -> playerID %u.", GetVnumItemWithMobM(vnum_mob), ch->GetPlayerID());
				return;
			}

			int iEmptyPos;
			if (pItem->IsDragonSoul())
			{
				iEmptyPos = ch->GetEmptyDragonSoulInventory(pItem);
			}

#ifdef ENABLE_SPECIAL_STORAGE
			else if (pItem->IsUpgradeItem())
			{
				iEmptyPos = ch->GetEmptyUpgradeInventory(pItem);
			}
			else if (pItem->IsBook())
			{
				iEmptyPos = ch->GetEmptyBookInventory(pItem);
			}
			else if (pItem->IsStone())
			{
				iEmptyPos = ch->GetEmptyStoneInventory(pItem);
			}
			else if (pItem->IsChange())
			{
				iEmptyPos = ch->GetEmptyChangeInventory(pItem);
			}
			else if (pItem->IsCostume())
			{
				iEmptyPos = ch->GetEmptyCostumeInventory(pItem);
			}
			else if (pItem->IsChest())
			{
				iEmptyPos = ch->GetEmptyChestInventory(pItem);
			}
#endif
			else{
				iEmptyPos = ch->GetEmptyInventory(pItem->GetSize());
			}

			if (iEmptyPos < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Envanterinde yeterli alan yok.");
					return;
			}

			if (pItem->IsDragonSoul())
			{
				pItem->AddToCharacter(ch, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
			}

#ifdef ENABLE_SPECIAL_STORAGE
			else if (pItem->IsUpgradeItem())
			{
				pItem->AddToCharacter(ch, TItemPos(UPGRADE_INVENTORY, iEmptyPos));
			}
			else if (pItem->IsBook())
			{
				pItem->AddToCharacter(ch, TItemPos(BOOK_INVENTORY, iEmptyPos));
			}
			else if (pItem->IsStone())
			{
				pItem->AddToCharacter(ch, TItemPos(STONE_INVENTORY, iEmptyPos));
			}
			else if (pItem->IsChange())
			{
				pItem->AddToCharacter(ch, TItemPos(CHANGE_INVENTORY, iEmptyPos));
			}
			else if (pItem->IsCostume())
			{
				pItem->AddToCharacter(ch, TItemPos(COSTUME_INVENTORY, iEmptyPos));
			}
			else if (pItem->IsChest())
			{
				pItem->AddToCharacter(ch, TItemPos(CHEST_INVENTORY, iEmptyPos));
			}
#endif
			else{
				pItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
			}

			ResetMision(ch,vnum_mob);
			SetDateDungeonMision(ch);
		}
		
	}
}

void CDungeonInfoExtern::SetDateDungeonMision(LPCHARACTER ch)
{
	SendDungeonInfoPackets(ch,DUNGEON_INFO_SUB_HEADER_DATES_MISION_CLEAR);
	if (date_dungeon.size() > 0){
		for (int i = 0; i<date_dungeon.size(); ++i){
			SendDungeonInfoPackets(ch,DUNGEON_INFO_SUB_HEADER_DATES_MISION_SEND,i);
		}
	}

	SendDungeonInfoPackets(ch,DUNGEON_INFO_SUB_HEADER_DATES_MISION_LOAD);

}

void CDungeonInfoExtern::TestTimeRespawn(LPCHARACTER ch)
{

	int time_respawn = ch->GetQuestFlag("dungeon_info.dungeon_time_respawn") - get_global_time();
}


int CDungeonInfoExtern::GetTimeRespawnDungeonActual(LPCHARACTER ch, long index_map)
{

	char dungeon_time_respawn[1024];
	snprintf(dungeon_time_respawn, sizeof(dungeon_time_respawn), "dungeon_info.dungeon_time_respawn_%ld", index_map);

	int time_respawn = ch->GetQuestFlag(dungeon_time_respawn) - get_global_time();

	if (time_respawn >= 0)
		return time_respawn;


	return 0;
}


int CDungeonInfoExtern::GetTimeRoomDungeonActual(LPCHARACTER ch, long index_map)
{

	char dungeon_time_room[1024];
	snprintf(dungeon_time_room, sizeof(dungeon_time_room), "dungeon_info.dungeon_time_room_%ld", index_map);

	int time_room = ch->GetQuestFlag(dungeon_time_room) - get_global_time();

	if (time_room >= 0)
		return time_room;

	return 0;
}

int CDungeonInfoExtern::GetItemCheckDungeonActual(LPCHARACTER ch, long index_map)
{

	char fix_item_check[1024];
	snprintf(fix_item_check, sizeof(fix_item_check), "dungeon_info.fix_item_check_%ld", index_map);
	int fix_item = ch->GetQuestFlag(fix_item_check);
	return fix_item;
}

int CDungeonInfoExtern::GetFinishTotalDungeon(LPCHARACTER ch, int id_boss)
{
	char dungeon_total_finish[1024];
	snprintf(dungeon_total_finish, sizeof(dungeon_total_finish), "dungeon_info.dungeon_finish_total_%d", id_boss);
	int total_finish = ch->GetQuestFlag(dungeon_total_finish);
	return total_finish;
}


int CDungeonInfoExtern::GetTimeFinishTotalDungeon(LPCHARACTER ch, long index_map)
{

	char dungeon_time_finish[1024];
	snprintf(dungeon_time_finish, sizeof(dungeon_time_finish), "dungeon_info.dungeon_time_finish_w_%ld", index_map);
	int time_finish = ch->GetQuestFlag(dungeon_time_finish);
	return time_finish;
}

int CDungeonInfoExtern::GetDamageDoneDungeon(LPCHARACTER ch, int id_boss)
{

	char dungeon_damage_done[1024];
	snprintf(dungeon_damage_done, sizeof(dungeon_damage_done), "dungeon_info.dungeon_damage_done_%d", id_boss);
	int damage_done = ch->GetQuestFlag(dungeon_damage_done);
	return damage_done;
}


int CDungeonInfoExtern::GetDamageReceivedDungeon(LPCHARACTER ch, int id_boss)
{

	char dungeon_damage_received[1024];
	snprintf(dungeon_damage_received, sizeof(dungeon_damage_received), "dungeon_info.dungeon_damage_received_%d", id_boss);
	int damage_received = ch->GetQuestFlag(dungeon_damage_received);
	return damage_received;
}


struct FGiveItemDungeon
{
	DWORD InstanceVnumItem;
	BYTE InstanceCountItem;

	FGiveItemDungeon(DWORD vnum,BYTE count)
		: InstanceVnumItem(vnum), InstanceCountItem(count)
	{}

	void operator() (LPENTITY ent)
	{
		if (NULL != ent)
		{
			LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);

			if (NULL != pChar)
			{
				if (true == pChar->IsPC())
				{

					pChar->AutoGiveItem(InstanceVnumItem, InstanceCountItem);
				}
			}
		}
	}
};

EVENTINFO(give_item_event_info)
{
	long InstanceMapIndex;
	DWORD InstanceVnumItem;
	BYTE InstanceCountItem;

	give_item_event_info()
	: InstanceMapIndex( 0 )
	, InstanceVnumItem(0)
	, InstanceCountItem(0)
	{
	}
};

EVENTFUNC(check_give_item_dungeon)
{

	give_item_event_info* info = dynamic_cast<give_item_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_give_item_dungeon> <Factor> Null pointer" );
		return 0;
	}

	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap( info->InstanceMapIndex );
	if (NULL != pMap)
	{
		FGiveItemDungeon f(info->InstanceVnumItem,info->InstanceCountItem);
		pMap->for_each( f );
		
	}

	return 0;
}


void CDungeonInfoExtern::StartGiveItemDungeon(LPCHARACTER ch, DWORD vnum, BYTE count)
{
	give_item_event_info* info = AllocEventInfo<give_item_event_info>();
	info->InstanceMapIndex = ch->GetMapIndex();
	info->InstanceVnumItem = vnum;
	info->InstanceCountItem = count;
	event_create(check_give_item_dungeon, info, PASSES_PER_SEC(2));
}




struct FWarpDestroyMapRoom
{
	void operator() (LPENTITY ent)
	{
		if (NULL != ent)
		{
			LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);

			if (NULL != pChar)
			{
				if (true == pChar->IsPC())
				{

					pChar->ExitToSavedLocation();
				}
			}
		}
	}
};

EVENTINFO(time_room_event_info)
{

	long InstanceMapIndex;

	time_room_event_info()
	: InstanceMapIndex( 0 )
	{
	}
};

EVENTFUNC(check_time_room_dungeon)
{

	time_room_event_info* info = dynamic_cast<time_room_event_info*>( event->info );
	if ( info == NULL )
	{
		sys_err( "check_time_room_dungeon> <Factor> Null pointer" );
		return 0;
	}

	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap( info->InstanceMapIndex );
	if (NULL != pMap)
	{
		FWarpDestroyMapRoom f;
		pMap->for_each( f );
		
	}

	return 0;
}


void CDungeonInfoExtern::StartCheckTimeRoom(LPCHARACTER ch)
{
	int map_index = ch->GetMapIndex()/10000;

	char dungeon_time_room[1024];
	snprintf(dungeon_time_room, sizeof(dungeon_time_room), "dungeon_info.dungeon_time_room_%d", map_index);

	int time_room = ch->GetQuestFlag(dungeon_time_room) - get_global_time();

	char dungeon_boss_room[1024];
	snprintf(dungeon_boss_room, sizeof(dungeon_boss_room), "dungeon_info.dungeon_boss_room_%d", map_index);
	ch->SetQuestFlag(dungeon_boss_room, 1);

	ch->SetDamageDoneDungeonInfo(0);
	ch->SetDamageReceivedDungeonInfo(0);

	time_room_event_info* info = AllocEventInfo<time_room_event_info>();
	info->InstanceMapIndex = ch->GetMapIndex();
	event_create(check_time_room_dungeon, info, PASSES_PER_SEC(time_room));
}



void CDungeonInfoExtern::SendDungeonInfoPackets(LPCHARACTER ch, BYTE subheader, int index)
{

	TPacketGCDungeonInfoReceive pack;
	pack.bSubType = subheader;

	if(subheader == DUNGEON_INFO_SUB_HEADER_DATES_SEND)
	{
		pack.dateinfo.id_dungeon 	=	date_dungeon[index].id;
		memcpy (pack.dateinfo.name, 	date_dungeon[index].dateinfo.name, 		sizeof(pack.dateinfo.name));
		memcpy (pack.dateinfo.img, 		date_dungeon[index].dateinfo.img, 		sizeof(pack.dateinfo.img));
		pack.dateinfo.lv_min 		= 	date_dungeon[index].dateinfo.lv_min;
		pack.dateinfo.lv_max 		= 	date_dungeon[index].dateinfo.lv_max;
		pack.dateinfo.difficulty 	= 	date_dungeon[index].dateinfo.difficulty;
		pack.dateinfo.party_max 	= 	date_dungeon[index].dateinfo.party_max;
		pack.dateinfo.respawn 		= 	date_dungeon[index].dateinfo.respawn;
		pack.dateinfo.time_room 	= 	date_dungeon[index].dateinfo.time_room;
		pack.dateinfo.time_respawn_dungeon 	= 	GetTimeRespawnDungeonActual(ch,date_dungeon[index].index_map);
		pack.dateinfo.time_room_dungeon 	= 	GetTimeRoomDungeonActual(ch,date_dungeon[index].index_map);
		memcpy (pack.dateinfo.entrance, 	date_dungeon[index].dateinfo.entrance, 		sizeof(pack.dateinfo.entrance));
		memcpy (pack.dateinfo.resistance, 	date_dungeon[index].dateinfo.resistance, 	sizeof(pack.dateinfo.resistance));
		memcpy (pack.dateinfo.force, 		date_dungeon[index].dateinfo.force, 		sizeof(pack.dateinfo.force));
		pack.dateinfo.vnum_item 		= 	date_dungeon[index].dateinfo.vnum_item;
		pack.dateinfo.count_item 		= 	date_dungeon[index].dateinfo.count_item;
		pack.dateinfo.count_finish 		=  	GetFinishTotalDungeon(ch,date_dungeon[index].dateinfo.id_boss);
		pack.dateinfo.time_finish 		=  	GetTimeFinishTotalDungeon(ch,date_dungeon[index].index_map);
		pack.dateinfo.damage_done 		= 	GetDamageDoneDungeon(ch,date_dungeon[index].dateinfo.id_boss);
		pack.dateinfo.damage_received 	= 	GetDamageReceivedDungeon(ch,date_dungeon[index].dateinfo.id_boss);
		pack.dateinfo.id_boss 			= 	date_dungeon[index].dateinfo.id_boss;

	}

	if(subheader == DUNGEON_INFO_SUB_HEADER_DATES_MISION_SEND){
		pack.dateinfom.vnum_mob_mision		=	date_dungeon[index].dateinfom.vnum_mob_mision;
		pack.dateinfom.count_mob_mision 	= 	date_dungeon[index].dateinfom.count_mob_mision;
		pack.dateinfom.count_mob_a_mision 	= 	GetCountMision(ch,date_dungeon[index].dateinfom.vnum_mob_mision);
		pack.dateinfom.status_mision		= 	GetStatusMision(ch,date_dungeon[index].dateinfom.vnum_mob_mision);
	}

	if (subheader == DUNGEON_INFO_SUB_HEADER_DATES_RANKING_SEND)
	{

		pack.dateinfo_ranking.vnum_mob_r 	=	date_dungeon_ranking[index].vnum_mob_r;
		pack.dateinfo_ranking.type 			=	date_dungeon_ranking[index].type;
		memcpy (pack.dateinfo_ranking.name_r, 	date_dungeon_ranking[index].name_r, 		sizeof(pack.dateinfo_ranking.name_r));
		pack.dateinfo_ranking.extra_r 		=	date_dungeon_ranking[index].extra_r;

	}


	LPDESC d = ch->GetDesc();

	if (NULL == d)
	{
		sys_err ("User CHARACTER::SendDungeonInfoPackets (%s)'s DESC is NULL POINT.", ch->GetName());
		return ;
	}

	d->Packet(&pack, sizeof(pack));

}
#endif
