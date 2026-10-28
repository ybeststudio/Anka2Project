#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "locale_service.h"
#include "questmanager.h"

#ifdef ENABLE_BATTLE_PASS
void CHARACTER::DoMission(int index, long long count)
{
	if (quest::CQuestManager::instance().GetEventFlag("battle_do_mission") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1000"));
		return;
	}

	if (missions_bp.empty())
		return;

	if (index < 0 || index >= (int)missions_bp.size() || index >= (int)v_counts.size())
		return;

	if (v_counts[index].status > 0)
		return;

#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);
	int month = vKey.tm_mon;
	if (month != iMonthBattlePass)
		return;
#endif

	FILE 	*fileID;
	char file_name[256+1];

	snprintf(file_name, sizeof(file_name), "%s/battlepass_players/%s.txt", LocaleService_GetBasePath().c_str(),GetName());
	fileID = fopen(file_name, "w");
	
	if (NULL == fileID)
		return;
	
	v_counts[index].count = v_counts[index].count + count;
	
	if (v_counts[index].count > missions_bp[index].count)
		v_counts[index].count = missions_bp[index].count;
	
	if (missions_bp[index].count == v_counts[index].count)
	{
		v_counts[index].status = v_counts[index].status + 1;
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Tebrikler, Basariyla savas bileti gorevini tamamladiniz."));

		if (rewards_bonus_bp[index].vnum1 > 0 && rewards_bonus_bp[index].count1 > 0)
		{
			const DWORD pointType = aApplyInfo[rewards_bonus_bp[index].vnum1].bPointType;
			const CAffect * pkAff = FindAffect(AFFECT_BATTLE_BONUS1, pointType);

			if (pkAff != NULL)
			{
				DWORD pointVal = pkAff->lApplyValue;
				DWORD newVal = pointVal + rewards_bonus_bp[index].count1;
				RemoveAffect(const_cast<CAffect*>(pkAff));

				AddAffect(AFFECT_BATTLE_BONUS1, pointType, newVal, 0, INFINITE_AFFECT_DURATION, 0, false);

				//if (IsGM())
				//	ChatPacket(1, "<AFFECT_BATTLE_BONUS1> Old affect removed, new added. pointType:%d - oldVal: %u, addVal:%u, newVal: %u", pointType, pointVal, rewards_bonus_bp[index].count1, newVal);
			}
			else
			{
				AddAffect(AFFECT_BATTLE_BONUS1, aApplyInfo[rewards_bonus_bp[index].vnum1].bPointType, rewards_bonus_bp[index].count1, 0, INFINITE_AFFECT_DURATION, 0, false);
			}
		}

		if (rewards_bonus_bp[index].vnum2 > 0 && rewards_bonus_bp[index].count2 > 0)
		{
			const DWORD pointType = aApplyInfo[rewards_bonus_bp[index].vnum2].bPointType;
			const CAffect * pkAff = FindAffect(AFFECT_BATTLE_BONUS2, pointType);

			if (pkAff != NULL)
			{
				DWORD pointVal = pkAff->lApplyValue;
				DWORD newVal = pointVal + rewards_bonus_bp[index].count2;
				RemoveAffect(const_cast<CAffect*>(pkAff));

				AddAffect(AFFECT_BATTLE_BONUS2, pointType, newVal, 0, INFINITE_AFFECT_DURATION, 0, false);

				//if (IsGM())
				//	ChatPacket(1, "<AFFECT_BATTLE_BONUS2> Old affect removed, new added. pointType:%d - oldVal: %u, addVal:%u, newVal: %u", pointType, pointVal, rewards_bonus_bp[index].count2, newVal);
			}
			else
			{
				AddAffect(AFFECT_BATTLE_BONUS2, aApplyInfo[rewards_bonus_bp[index].vnum2].bPointType, rewards_bonus_bp[index].count2, 0, INFINITE_AFFECT_DURATION, 0, false);
			}
		}

		if (rewards_bonus_bp[index].vnum3 > 0 && rewards_bonus_bp[index].count3 > 0)
		{
			const DWORD pointType = aApplyInfo[rewards_bonus_bp[index].vnum3].bPointType;
			const CAffect * pkAff = FindAffect(AFFECT_BATTLE_BONUS3, pointType);

			if (pkAff != NULL)
			{
				DWORD pointVal = pkAff->lApplyValue;
				DWORD newVal = pointVal + rewards_bonus_bp[index].count3;
				RemoveAffect(const_cast<CAffect*>(pkAff));

				AddAffect(AFFECT_BATTLE_BONUS3, pointType, newVal, 0, INFINITE_AFFECT_DURATION, 0, false);

				//if (IsGM())
				//	ChatPacket(1, "<AFFECT_BATTLE_BONUS3> Old affect removed, new added. pointType:%d - oldVal: %u, addVal:%u, newVal: %u", pointType, pointVal, rewards_bonus_bp[index].count3, newVal);
			}
			else
			{
				AddAffect(AFFECT_BATTLE_BONUS3, aApplyInfo[rewards_bonus_bp[index].vnum3].bPointType, rewards_bonus_bp[index].count3, 0, INFINITE_AFFECT_DURATION, 0, false);
			}
		}

		AutoGiveItem(rewards_bp[index].vnum1,rewards_bp[index].count1);
		AutoGiveItem(rewards_bp[index].vnum2,rewards_bp[index].count2);
		AutoGiveItem(rewards_bp[index].vnum3,rewards_bp[index].count3);
	}

	for (int i = 0; i < v_counts.size(); ++i)
	{
		fprintf(fileID,"MISSION	%d	%d\n", v_counts[i].count, v_counts[i].status);
	}

	fclose(fileID);
	SendInfosBattlePass(index);
}

void CHARACTER::Load_BattlePass()
{
	FILE 	*fp;
	char	one_line[256];
	int		value1, value2;
	const char	*delim = " \t\r\n";
	char	*v, *token_string;
	char file_name[256+1];

	Struct_BattlePass save_counts = {0, 0};

	snprintf(file_name, sizeof(file_name), "%s/battlepass_players/%s.txt", LocaleService_GetBasePath().c_str(),GetName());
	fp = fopen(file_name, "r");

	if (!fp)
		return;

	while (fgets(one_line, 256, fp))
	{
		value1 = value2 = 0;

		if (one_line[0] == '#')
			continue;

		token_string = strtok(one_line, delim);
		if (NULL == token_string)
			continue;

		if ((v = strtok(NULL, delim)))
			str_to_number(value1, v);

		if ((v = strtok(NULL, delim)))
			str_to_number(value2, v);

		TOKEN("MISSION")
		{
			save_counts.count = value1;
			save_counts.status = value2;
			v_counts.push_back(save_counts);
		}
	}
	fclose(fp);
	
	ChatPacket(CHAT_TYPE_COMMAND, "battlepass_status %d", 1);
}

void CHARACTER::ExternBattlePass()
{
	FILE 	*fp;
	char	one_line[256];
	int		value1, value2, value3, value4, value5, value6;
	const char	*delimit = " \t\r\n";
	char	*v, *token_string;

	char file_name[256+1];
	char value7[4096];

	Infos_BattlePass bp_infos = {0, 0, 0, 0, 0, 0, 0};
	Infos_MissionsBP bp_missions = {0, 0, 0, 0};
	Infos_FinalBP bp_final_reward = {0, 0, 0, 0};
	Infos_Bonus_BattlePass bp_bonus_infos = {0, 0, 0, 0, 0, 0};
	
	snprintf(file_name, sizeof(file_name), "%s/battlepass_config.txt", LocaleService_GetBasePath().c_str());
	fp = fopen(file_name, "r");

	if (!fp)
		return;

	while (fgets(one_line, 256, fp))
	{
		value1 = value2 = value3 = value4 = value5 = value6 = 0;

		if (one_line[0] == '#')
			continue;
		
		token_string = strtok(one_line, delimit);
		if (NULL == token_string)
			continue;

		if ((v = strtok(NULL, delimit)))
			str_to_number(value1, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value2, v);

		if ((v = strtok(NULL, delimit)))
			str_to_number(value3, v);
	
		if ((v = strtok(NULL, delimit)))
			str_to_number(value4, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value5, v);
		
		if ((v = strtok(NULL, delimit)))
			str_to_number(value6, v);

		if ((v = strtok(NULL, delimit)))
			strlcpy(bp_infos.name, v, sizeof(bp_infos.name) - 1);

		TOKEN("REWARDS_AND_NAME")
		{
			bp_infos.vnum1 = value1;
			bp_infos.count1  = value2;
			bp_infos.vnum2 = value3;
			bp_infos.count2  = value4;
			bp_infos.vnum3 = value5;
			bp_infos.count3  = value6;
			rewards_bp.push_back(bp_infos);
		}
		else TOKEN("REWARDS_BONUS")
		{
			bp_bonus_infos.vnum1 = value1;
			bp_bonus_infos.count1  = value2;
			bp_bonus_infos.vnum2 = value3;
			bp_bonus_infos.count2  = value4;
			bp_bonus_infos.vnum3 = value5;
			bp_bonus_infos.count3  = value6;
			rewards_bonus_bp.push_back(bp_bonus_infos);
		}
		else TOKEN("DO_MISSION")
		{
			bp_missions.type = value1;
			bp_missions.vnum  = value2;
			bp_missions.count = value3;
			bp_missions.ep = value4;
			missions_bp.push_back(bp_missions);
		}
		else TOKEN("FINAL_REWARD")
		{
			bp_final_reward.f_vnum1 = value1;
			bp_final_reward.f_count1  = value2;
			bp_final_reward.f_vnum2 = value3;
			bp_final_reward.f_count2  = value4;
			bp_final_reward.f_vnum3 = value5;
			bp_final_reward.f_count3  = value6;
			final_rewards.push_back(bp_final_reward);
		}
#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
		else TOKEN("MONTH_SEASON")
		{
			iMonthBattlePass = value1;
		}
#endif

	}
	fclose(fp);
}

void CHARACTER::SendInfosBattlePass(int index)
{
	if (quest::CQuestManager::instance().GetEventFlag("battle_pass_info") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1000"));
		return;
	}

	if (v_counts.empty())
		return;
	
	if (missions_bp.empty())
		return;
	
	if (rewards_bp.empty())
		return;

	ChatPacket(CHAT_TYPE_COMMAND, "info_missions_bp %d %d %d %s", index, v_counts[index].count, v_counts[index].status, rewards_bp[index].name);
}

void CHARACTER::SendDoneBattlePass(int index)
{
	if (quest::CQuestManager::instance().GetEventFlag("battle_pass_done") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1000"));
		return;
	}

	// SECURITY: /battlepass_bitirici <idx> ile vector OOB (crash / OOB write)
	if (index < 0 || index >= (int)missions_bp.size() || index >= (int)v_counts.size())
		return;

	if (rewards_bp.empty() || v_counts.empty() || missions_bp.empty() || missions_bp[index].ep <= 0)
		return;

	if (v_counts[index].status == 1)
	{
		ChatPacket(1, "Bu gorevi zaten bitirdiniz.");
		return;
	}

	if (GetCoins() < missions_bp[index].ep)
	{
		ChatPacket(1, "Yeterli ep'iniz yok, gereken ep: %d", missions_bp[index].ep);
		return;
	}

	PointChange(POINT_COINS, -missions_bp[index].ep);

	DoMission(index, missions_bp[index].count);
}

void CHARACTER::FinalRewardBattlePass()
{
	if (quest::CQuestManager::instance().GetEventFlag("battle_pass_final") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1000"));
		return;
	}

	if (rewards_bp.empty())
		return;

	if (v_counts.empty())
		return;

	if (missions_bp.empty())
		return;

	if (final_rewards.empty())
		return;
	
	if (v_counts[0].status == 2)
		return;

	FILE 	*fileID;
	char file_name[256+1];

	snprintf(file_name, sizeof(file_name), "%s/battlepass_players/%s.txt", LocaleService_GetBasePath().c_str(),GetName());
	fileID = fopen(file_name, "w");
	
	if (NULL == fileID)
		return;

	v_counts[0].status = v_counts[0].status + 1;

	for (int i=0; i<v_counts.size(); ++i)
	{
		fprintf(fileID,"MISSION	%d	%d\n", v_counts[i].count, v_counts[i].status);
	}

	fclose(fileID);

	AutoGiveItem(final_rewards[0].f_vnum1,final_rewards[0].f_count1);
	AutoGiveItem(final_rewards[0].f_vnum2,final_rewards[0].f_count2);
	AutoGiveItem(final_rewards[0].f_vnum3,final_rewards[0].f_count3);
}
#endif

