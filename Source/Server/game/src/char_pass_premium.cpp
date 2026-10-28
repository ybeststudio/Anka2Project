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
void CHARACTER::DoMissionPremium(int index, long long count)
{
	if (quest::CQuestManager::instance().GetEventFlag("battle_do_mission_premium") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1000"));
		return;
	}

	if (missions_bp_premium.empty())
		return;

	if (index < 0 || index >= (int)missions_bp_premium.size() || index >= (int)v_counts_premium.size())
		return;

	if (v_counts_premium[index].status > 0)
		return;

#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
	time_t cur_Time = time(NULL);
	struct tm vKey = *localtime(&cur_Time);
	int month = vKey.tm_mon;
	if (month != iMonthBattlePassPremium)
		return;
#endif

	FILE 	*fileID;
	char file_name[256+1];

	snprintf(file_name, sizeof(file_name), "%s/battlepass_premium_players/%s.txt", LocaleService_GetBasePath().c_str(),GetName());
	fileID = fopen(file_name, "w");
	
	if (NULL == fileID)
		return;
	
	v_counts_premium[index].count = v_counts_premium[index].count + count;
	
	if (v_counts_premium[index].count > missions_bp_premium[index].count)
		v_counts_premium[index].count = missions_bp_premium[index].count;
	
	if (missions_bp_premium[index].count == v_counts_premium[index].count)
	{
		v_counts_premium[index].status = v_counts_premium[index].status + 1;
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Tebrikler, Basariyla savas bileti gorevini tamamladiniz."));

		if (rewards_bonus_bp_premium[index].vnum1 > 0 && rewards_bonus_bp_premium[index].count1 > 0)
		{
			const DWORD pointType = aApplyInfo[rewards_bonus_bp_premium[index].vnum1].bPointType;
			const CAffect * pkAff = FindAffect(AFFECT_BATTLE_BONUS1, pointType);

			if (pkAff != NULL)
			{
				DWORD pointVal = pkAff->lApplyValue;
				DWORD newVal = pointVal + rewards_bonus_bp_premium[index].count1;
				RemoveAffect(const_cast<CAffect*>(pkAff));

				AddAffect(AFFECT_BATTLE_BONUS1, pointType, newVal, 0, INFINITE_AFFECT_DURATION, 0, false);
			}
			else
			{
				AddAffect(AFFECT_BATTLE_BONUS1, aApplyInfo[rewards_bonus_bp_premium[index].vnum1].bPointType, rewards_bonus_bp_premium[index].count1, 0, INFINITE_AFFECT_DURATION, 0, false);
			}
		}

		if (rewards_bonus_bp_premium[index].vnum2 > 0 && rewards_bonus_bp_premium[index].count2 > 0)
		{
			const DWORD pointType = aApplyInfo[rewards_bonus_bp_premium[index].vnum2].bPointType;
			const CAffect * pkAff = FindAffect(AFFECT_BATTLE_BONUS2, pointType);

			if (pkAff != NULL)
			{
				DWORD pointVal = pkAff->lApplyValue;
				DWORD newVal = pointVal + rewards_bonus_bp_premium[index].count2;
				RemoveAffect(const_cast<CAffect*>(pkAff));

				AddAffect(AFFECT_BATTLE_BONUS2, pointType, newVal, 0, INFINITE_AFFECT_DURATION, 0, false);
			}
			else
			{
				AddAffect(AFFECT_BATTLE_BONUS2, aApplyInfo[rewards_bonus_bp_premium[index].vnum2].bPointType, rewards_bonus_bp_premium[index].count2, 0, INFINITE_AFFECT_DURATION, 0, false);
			}
		}

		if (rewards_bonus_bp_premium[index].vnum3 > 0 && rewards_bonus_bp_premium[index].count3 > 0)
		{
			const DWORD pointType = aApplyInfo[rewards_bonus_bp_premium[index].vnum3].bPointType;
			const CAffect * pkAff = FindAffect(AFFECT_BATTLE_BONUS3, pointType);

			if (pkAff != NULL)
			{
				DWORD pointVal = pkAff->lApplyValue;
				DWORD newVal = pointVal + rewards_bonus_bp_premium[index].count3;
				RemoveAffect(const_cast<CAffect*>(pkAff));

				AddAffect(AFFECT_BATTLE_BONUS3, pointType, newVal, 0, INFINITE_AFFECT_DURATION, 0, false);
			}
			else
			{
				AddAffect(AFFECT_BATTLE_BONUS3, aApplyInfo[rewards_bonus_bp_premium[index].vnum3].bPointType, rewards_bonus_bp_premium[index].count3, 0, INFINITE_AFFECT_DURATION, 0, false);
			}
		}

		AutoGiveItem(rewards_bp_premium[index].vnum1,rewards_bp_premium[index].count1);
		AutoGiveItem(rewards_bp_premium[index].vnum2,rewards_bp_premium[index].count2);
		AutoGiveItem(rewards_bp_premium[index].vnum3,rewards_bp_premium[index].count3);
	}

	for (int i = 0; i < v_counts_premium.size(); ++i)
	{
		fprintf(fileID,"MISSION	%d	%d\n", v_counts_premium[i].count, v_counts_premium[i].status);
	}

	fclose(fileID);
	SendInfosBattlePassPremium(index);
}

void CHARACTER::Load_BattlePassPremium()
{
	FILE 	*fp;
	char	one_line[256];
	int		value1, value2;
	const char	*delim = " \t\r\n";
	char	*v, *token_string;
	char file_name[256+1];

	Struct_BattlePassPremium save_counts_premium = {0, 0};

	snprintf(file_name, sizeof(file_name), "%s/battlepass_premium_players/%s.txt", LocaleService_GetBasePath().c_str(),GetName());
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
			save_counts_premium.count = value1;
			save_counts_premium.status = value2;
			v_counts_premium.push_back(save_counts_premium);
		}
	}
	fclose(fp);
	
	ChatPacket(CHAT_TYPE_COMMAND, "battlepass_status_premium %d", 1);
}

void CHARACTER::ExternBattlePassPremium()
{
	FILE 	*fp;
	char	one_line[256];
	int		value1, value2, value3, value4, value5, value6;
	const char	*delimit = " \t\r\n";
	char	*v, *token_string;

	char file_name[256+1];
	char value7[4096];

	Infos_BattlePassPremium bp_infos_premium = {0, 0, 0, 0, 0, 0, 0};
	Infos_MissionsBPPremium bp_missions_premium = {0, 0, 0, 0};
	Infos_FinalBPPremium bp_final_reward_premium = {0, 0, 0, 0};
	Infos_Bonus_BattlePassPremium bp_bonus_infos_premium = {0, 0, 0, 0, 0, 0};

	snprintf(file_name, sizeof(file_name), "%s/battlepass_premium_config.txt", LocaleService_GetBasePath().c_str());
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
			strlcpy(bp_infos_premium.name, v, sizeof(bp_infos_premium.name) - 1);

		TOKEN("REWARDS_AND_NAME")
		{
			bp_infos_premium.vnum1 = value1;
			bp_infos_premium.count1  = value2;
			bp_infos_premium.vnum2 = value3;
			bp_infos_premium.count2  = value4;
			bp_infos_premium.vnum3 = value5;
			bp_infos_premium.count3  = value6;
			rewards_bp_premium.push_back(bp_infos_premium);
		}
		else TOKEN("REWARDS_BONUS")
		{
			bp_bonus_infos_premium.vnum1 = value1;
			bp_bonus_infos_premium.count1  = value2;
			bp_bonus_infos_premium.vnum2 = value3;
			bp_bonus_infos_premium.count2  = value4;
			bp_bonus_infos_premium.vnum3 = value5;
			bp_bonus_infos_premium.count3  = value6;
			rewards_bonus_bp_premium.push_back(bp_bonus_infos_premium);
		}
		else TOKEN("DO_MISSION")
		{
			bp_missions_premium.type = value1;
			bp_missions_premium.vnum  = value2;
			bp_missions_premium.count = value3;
			bp_missions_premium.ep = value4;
			missions_bp_premium.push_back(bp_missions_premium);
		}
		else TOKEN("FINAL_REWARD")
		{
			bp_final_reward_premium.f_vnum1 = value1;
			bp_final_reward_premium.f_count1  = value2;
			bp_final_reward_premium.f_vnum2 = value3;
			bp_final_reward_premium.f_count2  = value4;
			bp_final_reward_premium.f_vnum3 = value5;
			bp_final_reward_premium.f_count3  = value6;
			final_rewards_premium.push_back(bp_final_reward_premium);
		}
#ifdef ENABLE_BATTLE_PASS_MOUNTH_CLOSE
		else TOKEN("MONTH_SEASON")
		{
			iMonthBattlePassPremium = value1;
		}
#endif

	}
	fclose(fp);
}

void CHARACTER::SendInfosBattlePassPremium(int index)
{
	if (quest::CQuestManager::instance().GetEventFlag("battle_pass_info_premium") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1000"));
		return;
	}

	if (v_counts_premium.empty())
		return;

	if (missions_bp_premium.empty())
		return;

	if (rewards_bp_premium.empty())
		return;

	ChatPacket(CHAT_TYPE_COMMAND, "info_missions_bp_premium %d %d %d %s", index, v_counts_premium[index].count, v_counts_premium[index].status, rewards_bp_premium[index].name);
}

void CHARACTER::SendDoneBattlePassPremium(int index)
{
	if (quest::CQuestManager::instance().GetEventFlag("battle_pass_done_premium") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1000"));
		return;
	}

	// SECURITY: index bounds (vector OOB)
	if (index < 0 || index >= (int)missions_bp_premium.size() || index >= (int)v_counts_premium.size())
		return;

	if (rewards_bp_premium.empty() || v_counts_premium.empty() || missions_bp_premium.empty() || missions_bp_premium[index].ep <= 0)
		return;

	if (v_counts_premium[index].status == 1)
	{
		ChatPacket(1, "Bu gorevi zaten bitirdiniz.");
		return;
	}

	if (GetCoins() < missions_bp_premium[index].ep)
	{
		ChatPacket(1, "Yeterli ep'iniz yok, gereken ep: %d", missions_bp_premium[index].ep);
		return;
	}

	PointChange(POINT_COINS, -missions_bp_premium[index].ep);

	DoMissionPremium(index, missions_bp_premium[index].count);
}

void CHARACTER::FinalRewardBattlePassPremium()
{
	if (quest::CQuestManager::instance().GetEventFlag("battle_pass_final_premium") == 1)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("1000"));
		return;
	}

	if (rewards_bp_premium.empty())
		return;

	if (v_counts_premium.empty())
		return;

	if (missions_bp_premium.empty())
		return;

	if (final_rewards_premium.empty())
		return;
	
	if (v_counts_premium[0].status == 2)
		return;

	FILE 	*fileID;
	char file_name[256+1];

	snprintf(file_name, sizeof(file_name), "%s/battlepass_premium_players/%s.txt", LocaleService_GetBasePath().c_str(),GetName());
	fileID = fopen(file_name, "w");
	
	if (NULL == fileID)
		return;

	v_counts_premium[0].status = v_counts_premium[0].status + 1;

	for (int i=0; i<v_counts_premium.size(); ++i)
	{
		fprintf(fileID,"MISSION	%d	%d\n", v_counts_premium[i].count, v_counts_premium[i].status);
	}

	fclose(fileID);

	AutoGiveItem(final_rewards_premium[0].f_vnum1,final_rewards_premium[0].f_count1);
	AutoGiveItem(final_rewards_premium[0].f_vnum2,final_rewards_premium[0].f_count2);
	AutoGiveItem(final_rewards_premium[0].f_vnum3,final_rewards_premium[0].f_count3);
}
#endif

