#include "Stdafx.h"
#include "PythonPlayerSettingsModule.h"
#include "PythonNetworkStream.h"
#include "PythonPlayer.h"
#include "PythonBackground.h"
#include "PythonNetworkStream.h"

#include "../EterLib/ResourceManager.h"
#include "../EterPack/EterPackManager.h"
#include "../GameLib/RaceManager.h"

#include "boost/algorithm/string.hpp"

const char* CPythonPlayerSettingsModule::GetLocaleString(const char* key)
{
	static auto localeInfo = PyImport_ImportModule("localeInfo");

	assert(localeInfo != nullptr);
	if (!localeInfo)
	{
		TraceError("Can't import localeInfo.");
		return "";
	}

	if (PyObject_HasAttrString(localeInfo, key) != 0)
		return PyString_AsString(PyObject_GetAttrString(localeInfo, key));

	TraceError("unknown locale string key %s", key);

	return "";
}

void trim(std::string& str)
{
	static const char* whitespace = " \t\r\n";

	str.erase(0, str.find_first_not_of(whitespace));
	str.erase(str.find_last_not_of(whitespace) + 1);
}

void CPythonPlayerSettingsModule::Load()
{
	const DWORD dwStartTime = ELTimer_GetMSec();
	__RegisterDungeonMapNames();
	__RegisterTitleNames();
	__RegisterNameColors();
	__RegisterTitleColors();
	__RegisterEmotionIcons();

	__LoadGameSound();
	__LoadGameEffects();
	__LoadGameInit();
	__LoadEmoticons();

	for (const auto &[raceIndex, fileName] : vecRaceData)
	{
		CRaceManager::Instance().CreateRace(raceIndex);
		CRaceManager::Instance().SelectRace(raceIndex);

		switch (raceIndex)
		{
			case NPlayerData::MAIN_RACE_WARRIOR_M:
				__LoadGameRace(CRaceManager::Instance().GetSelectedRaceDataPointer(), "msm/warrior_m.msm");
				__LoadGameWarriorEx(CRaceManager::Instance().GetSelectedRaceDataPointer(), fileName);
				break;

			case NPlayerData::MAIN_RACE_WARRIOR_W:
				__LoadGameRace(CRaceManager::Instance().GetSelectedRaceDataPointer(), "msm/warrior_w.msm");
				__LoadGameWarriorEx(CRaceManager::Instance().GetSelectedRaceDataPointer(), fileName);
				break;

			case NPlayerData::MAIN_RACE_ASSASSIN_M:
				__LoadGameRace(CRaceManager::Instance().GetSelectedRaceDataPointer(), "msm/assassin_m.msm");
				__LoadGameAssassinEx(CRaceManager::Instance().GetSelectedRaceDataPointer(), fileName);
				break;

			case NPlayerData::MAIN_RACE_ASSASSIN_W:
				__LoadGameRace(CRaceManager::Instance().GetSelectedRaceDataPointer(), "msm/assassin_w.msm");
				__LoadGameAssassinEx(CRaceManager::Instance().GetSelectedRaceDataPointer(), fileName);
				break;

			case NPlayerData::MAIN_RACE_SURA_M:
				__LoadGameRace(CRaceManager::Instance().GetSelectedRaceDataPointer(), "msm/sura_m.msm");
				__LoadGameSuraEx(CRaceManager::Instance().GetSelectedRaceDataPointer(), fileName);
				break;

			case NPlayerData::MAIN_RACE_SURA_W:
				__LoadGameRace(CRaceManager::Instance().GetSelectedRaceDataPointer(), "msm/sura_w.msm");
				__LoadGameSuraEx(CRaceManager::Instance().GetSelectedRaceDataPointer(), fileName);
				break;

			case NPlayerData::MAIN_RACE_SHAMAN_M:
				__LoadGameRace(CRaceManager::Instance().GetSelectedRaceDataPointer(), "msm/shaman_m.msm");
				__LoadGameShamanEx(CRaceManager::Instance().GetSelectedRaceDataPointer(), fileName);
				break;

			case NPlayerData::MAIN_RACE_SHAMAN_W:
				__LoadGameRace(CRaceManager::Instance().GetSelectedRaceDataPointer(), "msm/shaman_w.msm");
				__LoadGameShamanEx(CRaceManager::Instance().GetSelectedRaceDataPointer(), fileName);
				break;
		}
	}

	__LoadGameNPC();

	Tracenf("CPythonPlayerSettingsModule::Load spent %u ms", ELTimer_GetMSec() - dwStartTime);
}

void CPythonPlayerSettingsModule::__RegisterCacheMotionData(CRaceData *pRaceData, const uint16_t wMotionMode, const uint16_t wMotionIndex, const char *c_szFileName, const uint8_t byPercentage)
{
	const char *c_szFullFileName = CRaceManager::Instance().GetFullPathFileName(c_szFileName);
	pRaceData->RegisterMotionData(wMotionMode, wMotionIndex, c_szFullFileName, byPercentage);
}

void CPythonPlayerSettingsModule::__SetIntroMotions(CRaceData *pRaceData, const std::string &c_rstrFolderName)
{
	CRaceManager::Instance().SetPathName((c_rstrFolderName + "intro/").c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_INTRO_WAIT, "wait.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_INTRO_SELECTED, "selected.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_INTRO_NOT_SELECTED, "not_selected.msa");
}

void CPythonPlayerSettingsModule::__SetGeneralMotions(CRaceData *pRaceData, const uint16_t wMotionMode, const std::string &c_rstrFolderName)
{
	CRaceManager::Instance().SetPathName(c_rstrFolderName.c_str());
	pRaceData->RegisterMotionMode(wMotionMode);

	for (const auto &[motionIndex, fileName, percentage] : vecGeneralMotions)
		__RegisterCacheMotionData(pRaceData, wMotionMode, motionIndex, fileName, percentage);
}

#ifdef ENABLE_STANDING_MOUNT
void CPythonPlayerSettingsModule::__SetGeneralMotionsHorseStand(CRaceData *pRaceData, const uint16_t wMotionMode, const std::string &c_rstrFolderName)
{
	CRaceManager::Instance().SetPathName(c_rstrFolderName.c_str());
	pRaceData->RegisterMotionMode(wMotionMode);

	// Horse Stand uses wait animation for all movement
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WALK, "wait.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_RUN, "wait.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DEAD, "dead.msa");
}
#endif

void CPythonPlayerSettingsModule::__LoadComboRace(CRaceData* pRaceData, const uint16_t wMotionMode, const std::string& c_rstrFolderName)
{
	const BYTE RaceIndex = RaceToJob(pRaceData->GetRace());

	CRaceManager::Instance().SetPathName(c_rstrFolderName.c_str());

	pRaceData->RegisterMotionMode(wMotionMode);

#ifdef ENABLE_STANDING_MOUNT
	const bool isHorseStandMode = (wMotionMode == CRaceMotionData::MODE_HORSE_STAND_SWORD ||
	                                wMotionMode == CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD ||
	                                wMotionMode == CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD ||
	                                wMotionMode == CRaceMotionData::MODE_HORSE_STAND_BOW ||
	                                wMotionMode == CRaceMotionData::MODE_HORSE_STAND_FAN ||
	                                wMotionMode == CRaceMotionData::MODE_HORSE_STAND_BELL);
#endif

	if (RaceIndex == NRaceData::JOB_WARRIOR)
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa", 50);
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait_1.msa", 50);
	}
	else if (RaceIndex == NRaceData::JOB_ASSASSIN)
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa", 70);
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait_1.msa", 30);
	}
	else if (RaceIndex == NRaceData::JOB_SURA)
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage.msa");
	}
	else if (RaceIndex == NRaceData::JOB_SHAMAN)
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa");
	}

#ifdef ENABLE_STANDING_MOUNT
	// For horse stand modes, use wait animation for walk and run (character stands on surfboard)
	if (isHorseStandMode)
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WALK, "wait.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_RUN, "wait.msa");
	}
	else
	{
#endif
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WALK, "walk.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_RUN, "run.msa");
#ifdef ENABLE_STANDING_MOUNT
	}
#endif
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50);

	if (wMotionMode == CRaceMotionData::MODE_BOW
#ifdef ENABLE_STANDING_MOUNT
		|| wMotionMode == CRaceMotionData::MODE_HORSE_STAND_BOW
#endif
		)
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa");
	else
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_4, "combo_04.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_5, "combo_05.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_6, "combo_06.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_7, "combo_07.msa");
	}
	if (wMotionMode == CRaceMotionData::MODE_DUALHAND_SWORD
#ifdef ENABLE_STANDING_MOUNT
		|| wMotionMode == CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD
#endif
		)
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_8, "combo_08.msa");

	__LoadCombo(pRaceData, wMotionMode);
}


void CPythonPlayerSettingsModule::__LoadCombo(CRaceData *pRaceData, const uint16_t wMotionMode)
{
	if (wMotionMode == CRaceMotionData::MODE_BOW
#ifdef ENABLE_STANDING_MOUNT
		|| wMotionMode == CRaceMotionData::MODE_HORSE_STAND_BOW
#endif
		)
	{
		pRaceData->ReserveComboAttack(wMotionMode, COMBO_TYPE_1, 1);
		pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
		return;
	}

	pRaceData->ReserveComboAttack(wMotionMode, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_4);


	pRaceData->ReserveComboAttack(wMotionMode, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(wMotionMode, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_6);
	if (wMotionMode == CRaceMotionData::MODE_DUALHAND_SWORD
#ifdef ENABLE_STANDING_MOUNT
		|| wMotionMode == CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD
#endif
		)
		pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_6, CRaceMotionData::NAME_COMBO_ATTACK_8);
	else
		pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_6, CRaceMotionData::NAME_COMBO_ATTACK_4);
}

void CPythonPlayerSettingsModule::__LoadGameWarriorEx(CRaceData *pRaceData, const std::string &c_rstrFolderName)
{
	__SetIntroMotions(pRaceData, c_rstrFolderName);
	__SetGeneralMotions(pRaceData, CRaceMotionData::MODE_GENERAL, c_rstrFolderName + "general/");
#ifdef ENABLE_STANDING_MOUNT
	__SetGeneralMotionsHorseStand(pRaceData, CRaceMotionData::MODE_HORSE_STAND, c_rstrFolderName + "general/");
#endif
	pRaceData->SetMotionRandomWeight(CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_WAIT, 0, 70);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 50);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack_1.msa", 50);
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "skill/").c_str());

	for (int i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; ++i)
	{
		std::string END_STRING = "";
		if (i != 0)
			END_STRING = "_" + std::to_string(i + 1);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("samyeon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("palbang" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("jeongwi" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("geomgyeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("tanhwan" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("gihyeol" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("finish" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("gigongcham" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("gyeoksan" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("daejin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("cheongeun" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("geompung" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("noegeom" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("finish" + END_STRING + ".msa").c_str());

		pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
		pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	}

#ifdef ENABLE_STANDING_MOUNT
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_STAND, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_STAND, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	// Standing mount ONEHAND_SWORD için skill motion'larý
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_STAND_SWORD);
	CRaceManager::Instance().SetPathName((c_rstrFolderName + "skill/").c_str());
	for (int i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; ++i)
	{
		std::string END_STRING = "";
		if (i != 0)
			END_STRING = "_" + std::to_string(i + 1);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("samyeon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("palbang" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("jeongwi" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("geomgyeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("tanhwan" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("gihyeol" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("finish" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("gigongcham" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("gyeoksan" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("daejin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("cheongeun" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("geompung" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("noegeom" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("finish" + END_STRING + ".msa").c_str());
	}

	// Standing mount TWOHAND_SWORD için skill motion'larý
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD);
	for (int i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; ++i)
	{
		std::string END_STRING = "";
		if (i != 0)
			END_STRING = "_" + std::to_string(i + 1);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("samyeon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("palbang" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("jeongwi" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("geomgyeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("tanhwan" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("gihyeol" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("finish" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("gigongcham" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("gyeoksan" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("daejin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("cheongeun" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("geompung" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("noegeom" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("finish" + END_STRING + ".msa").c_str());
	}
#endif

	__LoadFishing(pRaceData, c_rstrFolderName);
	__LoadGuildSkill(pRaceData, c_rstrFolderName);

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait.msa", 90);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_1.msa", 9);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_2.msa", 1);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_RUN, "run.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE, "damage.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE_BACK, "damage.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DEAD, "dead.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_SKILL + 122, "skill_charge.msa");

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse_onehand_sword/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa");
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa");

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse_twohand_sword/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 90);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 9);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 1);
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_TWOHAND_SWORD, CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa");

	__LoadWedding(pRaceData, c_rstrFolderName);
	__LoadActions(pRaceData, c_rstrFolderName);
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_ONEHAND_SWORD, c_rstrFolderName + "onehand_sword/");
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_TWOHAND_SWORD, c_rstrFolderName + "twohand_sword/");

#ifdef ENABLE_STANDING_MOUNT
	// HORSE_STAND ONEHAND_SWORD BATTLE
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, c_rstrFolderName + "onehand_sword/");

	// HORSE_STAND TWOHAND_SWORD BATTLE
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_HORSE_STAND_TWOHAND_SWORD, c_rstrFolderName + "twohand_sword/");
#endif

	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right_hand");
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_ACCE, "Bip01 Spine2");
}

void CPythonPlayerSettingsModule::__LoadGameAssassinEx(CRaceData *pRaceData, const std::string &c_rstrFolderName)
{
	__SetIntroMotions(pRaceData, c_rstrFolderName);
	__SetGeneralMotions(pRaceData, CRaceMotionData::MODE_GENERAL, c_rstrFolderName + "general/");
#ifdef ENABLE_STANDING_MOUNT
	__SetGeneralMotionsHorseStand(pRaceData, CRaceMotionData::MODE_HORSE_STAND, c_rstrFolderName + "general/");
#endif
	pRaceData->SetMotionRandomWeight(CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_WAIT, 0, 70);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 50);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack_1.msa", 50);
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "skill/").c_str());

	for (int i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; ++i)
	{
		std::string END_STRING = "";
		if (i != 0)
			END_STRING = "_" + std::to_string(i + 1);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("amseup" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("gungsin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("charyun" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("eunhyeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("sangong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("seomjeon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("ilgangpyo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("yeonsa" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("gwangyeok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("hwajo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("gyeonggong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("dokgigung" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("seomgwang" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("pungraepo" + END_STRING + ".msa").c_str());
		pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
		pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	}

#ifdef ENABLE_STANDING_MOUNT
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_STAND, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_STAND, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	// Standing mount ONEHAND_SWORD için skill motion'larý
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_STAND_SWORD);
	CRaceManager::Instance().SetPathName((c_rstrFolderName + "skill/").c_str());
	for (int i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; ++i)
	{
		std::string END_STRING = "";
		if (i != 0)
			END_STRING = "_" + std::to_string(i + 1);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("amseup" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("gungsin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("charyun" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("eunhyeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("sangong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("seomjeon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("ilgangpyo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("yeonsa" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("gwangyeok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("hwajo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("gyeonggong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("dokgigung" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("seomgwang" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("pungraepo" + END_STRING + ".msa").c_str());
	}

	// Standing mount DUALHAND_SWORD için skill motion'larý
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD);
	for (int i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; ++i)
	{
		std::string END_STRING = "";
		if (i != 0)
			END_STRING = "_" + std::to_string(i + 1);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("amseup" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("gungsin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("charyun" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("eunhyeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("sangong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("seomjeon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("ilgangpyo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("yeonsa" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("gwangyeok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("hwajo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("gyeonggong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("dokgigung" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("seomgwang" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("pungraepo" + END_STRING + ".msa").c_str());
	}

	// Standing mount BOW için skill motion'larý
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_STAND_BOW);
	for (int i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; ++i)
	{
		std::string END_STRING = "";
		if (i != 0)
			END_STRING = "_" + std::to_string(i + 1);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("amseup" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("gungsin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("charyun" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("eunhyeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("sangong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("seomjeon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("ilgangpyo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("yeonsa" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("gwangyeok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("hwajo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("gyeonggong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("dokgigung" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("seomgwang" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("pungraepo" + END_STRING + ".msa").c_str());
	}
#endif

	__LoadFishing(pRaceData, c_rstrFolderName);
	__LoadGuildSkill(pRaceData, c_rstrFolderName);

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait.msa", 90);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_1.msa", 9);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_2.msa", 1);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_RUN, "run.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE, "damage.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE_BACK, "damage.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DEAD, "dead.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_SKILL + 122, "skill_charge.msa");

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse_onehand_sword/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa");
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa");

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse_dualhand_sword/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa");
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_DUALHAND_SWORD, CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa");

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse_bow/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_BOW);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_WAIT, "wait.msa", 90);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_WAIT, "wait_1.msa", 9);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_WAIT, "wait_2.msa", 1);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_RUN, "run.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_DAMAGE, "damage.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_DEAD, "dead.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa");
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_BOW, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_BOW, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	__LoadWedding(pRaceData, c_rstrFolderName);
	__LoadActions(pRaceData, c_rstrFolderName);
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_ONEHAND_SWORD, c_rstrFolderName + "onehand_sword/");
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_DUALHAND_SWORD, c_rstrFolderName + "dualhand_sword/");
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_BOW, c_rstrFolderName + "bow/");

#ifdef ENABLE_STANDING_MOUNT
	// HORSE_STAND ONEHAND_SWORD BATTLE
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, c_rstrFolderName + "onehand_sword/");

	// HORSE_STAND DUALHAND_SWORD BATTLE
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_HORSE_STAND_DUALHAND_SWORD, c_rstrFolderName + "dualhand_sword/");

	// HORSE_STAND BOW BATTLE
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BOW, c_rstrFolderName + "bow/");
#endif

	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON_LEFT, "equip_left");
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_ACCE, "Bip01 Spine2");
}

void CPythonPlayerSettingsModule::__LoadGameSuraEx(CRaceData *pRaceData, const std::string &c_rstrFolderName)
{
	__SetIntroMotions(pRaceData, c_rstrFolderName);
	__SetGeneralMotions(pRaceData, CRaceMotionData::MODE_GENERAL, c_rstrFolderName + "general/");
#ifdef ENABLE_STANDING_MOUNT
	__SetGeneralMotionsHorseStand(pRaceData, CRaceMotionData::MODE_HORSE_STAND, c_rstrFolderName + "general/");
#endif
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 50);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack_1.msa", 50);
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "skill/").c_str());

	for (int i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; ++i)
	{
		std::string END_STRING = "";
		if (i != 0)
			END_STRING = "_" + std::to_string(i + 1);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("swaeryeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("yonggwon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("gwigeom" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("gongpo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("jumagap" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("pabeop" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("akgi" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("maryeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("hwayeom" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("muyeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("heuksin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("tusok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("mahwan" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("akgi_dark" + END_STRING + ".msa").c_str());
		pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
		pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	}
#ifdef ENABLE_STANDING_MOUNT
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_STAND, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_STAND, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	// Standing mount ONEHAND_SWORD için skill motion'larý
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_STAND_SWORD);
	CRaceManager::Instance().SetPathName((c_rstrFolderName + "skill/").c_str());
	for (int i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; ++i)
	{
		std::string END_STRING = "";
		if (i != 0)
			END_STRING = "_" + std::to_string(i + 1);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("swaeryeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("yonggwon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("gwigeom" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("gongpo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("jumagap" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("pabeop" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("akgi" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("maryeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("hwayeom" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("muyeong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("heuksin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("tusok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("mahwan" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("akgi_dark" + END_STRING + ".msa").c_str());
	}
#endif
	__LoadGuildSkill(pRaceData, c_rstrFolderName);
	__LoadFishing(pRaceData, c_rstrFolderName);

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait.msa", 90);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_1.msa", 9);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_2.msa", 1);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_RUN, "run.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE, "damage.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE_BACK, "damage.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DEAD, "dead.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_SKILL + 122, "skill_charge.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa");

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse_onehand_sword/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa");
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa");

	__LoadWedding(pRaceData, c_rstrFolderName);

	__LoadActions(pRaceData, c_rstrFolderName);
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_ONEHAND_SWORD, c_rstrFolderName + "onehand_sword/");

#ifdef ENABLE_STANDING_MOUNT
	// HORSE_STAND ONEHAND_SWORD BATTLE
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_HORSE_STAND_SWORD, c_rstrFolderName + "onehand_sword/");
#endif

	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_ACCE, "Bip01 Spine2");
}

void CPythonPlayerSettingsModule::__LoadGameShamanEx(CRaceData *pRaceData, const std::string &c_rstrFolderName)
{
	__SetIntroMotions(pRaceData, c_rstrFolderName);
	__SetGeneralMotions(pRaceData, CRaceMotionData::MODE_GENERAL, c_rstrFolderName + "general/");
#ifdef ENABLE_STANDING_MOUNT
	__SetGeneralMotionsHorseStand(pRaceData, CRaceMotionData::MODE_HORSE_STAND, c_rstrFolderName + "general/");
#endif
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 50);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack_1.msa", 50);
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "skill/").c_str());

	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 1, "bipabu.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 2, "yongpa.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 3, "paeryong.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 4, "hosin_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 5, "boho_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 6, "gicheon_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 9, "meteo.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 16, "noejeon.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 17, "byeorak.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 18, "pokroe.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 19, "jeongeop_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 20, "kwaesok_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 21, "jeungryeok_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + 24, "chunwoon.msa");

	std::vector<int> skillList{ 1, 2, 3 };

	for (const auto& i : skillList)
	{
		std::string END_STRING = "_" + std::to_string(i + 1);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("bipabu" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("yongpa" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("paeryong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("hosin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("boho" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("gicheon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("meteo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("noejeon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("byeorak" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("pokroe" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("jeongeop" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("kwaesok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("jeungryeok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("chunwoon" + END_STRING + ".msa").c_str());
		pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
		pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	}

#ifdef ENABLE_STANDING_MOUNT
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_STAND, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_STAND, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	// Standing mount FAN için skill motion'larý
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_STAND_FAN);
	CRaceManager::Instance().SetPathName((c_rstrFolderName + "skill/").c_str());
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 1, "bipabu.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 2, "yongpa.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 3, "paeryong.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 4, "hosin_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 5, "boho_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 6, "gicheon_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 9, "meteo.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 16, "noejeon.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 17, "byeorak.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 18, "pokroe.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 19, "jeongeop_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 20, "kwaesok_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 21, "jeungryeok_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + 24, "chunwoon.msa");

	for (const auto& i : skillList)
	{
		std::string END_STRING = "_" + std::to_string(i + 1);
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("bipabu" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("yongpa" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("paeryong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("hosin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("boho" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("gicheon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("meteo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("noejeon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("byeorak" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("pokroe" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("jeongeop" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("kwaesok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("jeungryeok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("chunwoon" + END_STRING + ".msa").c_str());
	}

	// Standing mount BELL için skill motion'larý
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_STAND_BELL);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 1, "bipabu.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 2, "yongpa.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 3, "paeryong.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 4, "hosin_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 5, "boho_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 6, "gicheon_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 9, "meteo.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 16, "noejeon.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 17, "byeorak.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 18, "pokroe.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 19, "jeongeop_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 20, "kwaesok_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 21, "jeungryeok_target.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + 24, "chunwoon.msa");

	for (const auto& i : skillList)
	{
		std::string END_STRING = "_" + std::to_string(i + 1);
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 1, std::string("bipabu" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 2, std::string("yongpa" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 3, std::string("paeryong" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 4, std::string("hosin" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 5, std::string("boho" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 6, std::string("gicheon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 9, std::string("meteo" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 16, std::string("noejeon" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 17, std::string("byeorak" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 18, std::string("pokroe" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 19, std::string("jeongeop" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 20, std::string("kwaesok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 21, std::string("jeungryeok" + END_STRING + ".msa").c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, CRaceMotionData::NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + 24, std::string("chunwoon" + END_STRING + ".msa").c_str());
	}
#endif

	__LoadGuildSkill(pRaceData, c_rstrFolderName);
	__LoadFishing(pRaceData, c_rstrFolderName);

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait.msa", 90);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_1.msa", 9);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_2.msa", 1);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_RUN, "run.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE, "damage.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE_BACK, "damage.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DEAD, "dead.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_SKILL + 122, "skill_charge.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_SKILL + 123, "skill_splash.msa");

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse_fan/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_FAN);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_FAN, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_FAN, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_FAN, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa");
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_FAN, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_FAN, CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa");

	CRaceManager::Instance().SetPathName((c_rstrFolderName + "horse_bell/").c_str());

	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_BELL);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BELL, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa", 90);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BELL, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa", 9);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BELL, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa", 1);
	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_BELL, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BELL, CRaceMotionData::NAME_SKILL + 121, "skill_wildattack.msa");

	__LoadWedding(pRaceData, c_rstrFolderName);
	__LoadActions(pRaceData, c_rstrFolderName);
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_FAN, c_rstrFolderName + "fan/");
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_BELL, c_rstrFolderName + "Bell/");

#ifdef ENABLE_STANDING_MOUNT
	// HORSE_STAND FAN BATTLE
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_HORSE_STAND_FAN, c_rstrFolderName + "fan/");

	// HORSE_STAND BELL BATTLE
	__LoadComboRace(pRaceData, CRaceMotionData::MODE_HORSE_STAND_BELL, c_rstrFolderName + "Bell/");
#endif

	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON_LEFT, "equip_left");
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_ACCE, "Bip01 Spine2");
}

void CPythonPlayerSettingsModule::__LoadGameRace(CRaceData *pRaceData, const std::string &c_rstrFolderName)
{
	if (!pRaceData->LoadRaceData(c_rstrFolderName.c_str()))
	{
		TraceError("Failed to load race data : %s\n", c_rstrFolderName.c_str());
	}
}

void CPythonPlayerSettingsModule::__LoadFishing(CRaceData *pRaceData, const std::string &c_rstrFolderName)
{
	CRaceManager::Instance().SetPathName((c_rstrFolderName + "fishing/").c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_FISHING);

	for (const auto &[motionIndex, fileName] : vecFishing)
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, motionIndex, fileName);
}

void CPythonPlayerSettingsModule::__LoadWedding(CRaceData *pRaceData, const std::string &c_rstrFolderName)
{
	CRaceManager::Instance().SetPathName((c_rstrFolderName + "wedding/").c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_WEDDING_DRESS);

	for (const auto &[motionIndex, fileName] : vecWeddingList)
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_WEDDING_DRESS, motionIndex, fileName);
}

void CPythonPlayerSettingsModule::__LoadGuildSkill(CRaceData *pRaceData, const std::string &c_rstrFolderName)
{
	CRaceManager::Instance().SetPathName((c_rstrFolderName + "skill/").c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);

	for (const auto &[motionIndex, fileName] : vecGuildSkill)
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, motionIndex, fileName);
}

void CPythonPlayerSettingsModule::__LoadActions(CRaceData *pRaceData, const std::string &c_rstrFolderName)
{
	CRaceManager::Instance().SetPathName((c_rstrFolderName + "action/").c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);

	for (const auto &[motionIndex, fileName] : vecActionsList)
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, motionIndex, fileName);
}

void CPythonPlayerSettingsModule::__LoadGameSound()
{
	for (const auto &[useSoundIndex, fileName] : vecUseSoundFileName)
		CPythonItem::Instance().SetUseSoundFileName(useSoundIndex, fileName);

	for (const auto &[dropSoundIndex, fileName] : vecDropSoundFileName)
		CPythonItem::Instance().SetDropSoundFileName(dropSoundIndex, fileName);
}

void CPythonPlayerSettingsModule::__LoadGameEffects()
{
	for (const auto& [effectName, effectType, effectFolder, effectCache] : vecEffectList)
		CInstanceBase::RegisterEffect(effectName, effectType, effectFolder, effectCache);

	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_EXP, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_yellow_small2.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_HP_MEDIUM, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_small.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_HP_BIG, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_big.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_SP_SMALL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_warrior_small.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_SP_MEDIUM, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_small.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_SP_BIG, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_big.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK1, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_1.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK2, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_2.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK3, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_3.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK4, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_4.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK5, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_5.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK6, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_6.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_FIREWORK_XMAS, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_xmas.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_CHAIN_LIGHTNING, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/pc/shaman/effect/pokroe.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_HP_SMALL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_smallest.msf");
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_SKILL_MUYEONG, CFlyingManager::INDEX_FLY_TYPE_AUTO_FIRE, "d:/ymir work/pc/sura/effect/muyeong_fly.msf");
#ifdef ENABLE_CONQUEROR_LEVEL
	CFlyingManager::Instance().RegisterIndexedFlyData(CFlyingManager::FLY_SKILL_MUYEONG, CFlyingManager::INDEX_FLY_TYPE_AUTO_FIRE, "d:/ymir work/effect/etc/gathering/ga_piece_orange_small.msf");
#endif
}

void CPythonPlayerSettingsModule::__LoadEmoticons()
{
	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 0, "", "d:/ymir work/effect/etc/emoticon/sweat.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("sweat");

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 1, "", "d:/ymir work/effect/etc/emoticon/money.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("money");

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 2, "", "d:/ymir work/effect/etc/emoticon/happy.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("happy");

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 3, "", "d:/ymir work/effect/etc/emoticon/love_s.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("love_s");

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 4, "", "d:/ymir work/effect/etc/emoticon/love_l.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("love_l");

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 5, "", "d:/ymir work/effect/etc/emoticon/angry.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("angry");

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 6, "", "d:/ymir work/effect/etc/emoticon/aha.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("aha");

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 7, "", "d:/ymir work/effect/etc/emoticon/gloom.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("gloom");

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 8, "", "d:/ymir work/effect/etc/emoticon/sorry.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("sorry");

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 10, "", "d:/ymir work/effect/etc/emoticon/question.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("?");

	CInstanceBase::RegisterEffect(CInstanceBase::EFFECT_EMOTICON + 11, "", "d:/ymir work/effect/etc/emoticon/fish.mse", true);
	CPythonNetworkStream::Instance().RegisterEmoticonString("fish");
}

void CPythonPlayerSettingsModule::__LoadGameInit()
{
	CInstanceBase::SetDustGap(250);
	CInstanceBase::SetHorseDustGap(500);
	CPythonPlayer::Instance().RegisterEffect(CPythonPlayer::EFFECT_PICK, "d:/ymir work/effect/etc/click/click.mse", true);
}

void CPythonPlayerSettingsModule::__LoadGameNPC()
{
	CMappedFile file;
	const VOID *pvData;
	if (!CEterPackManager::Instance().Get(file, NPC_LIST_FILE_NAME, &pvData))
	{
		TraceError("CPythonPlayerSettingsModule::LoadGameNPC(fileName=%s) - Load Error", NPC_LIST_FILE_NAME);
		return;
	}

	CMemoryTextFileLoader fileLoader;
	fileLoader.Bind(file.Size(), pvData);

	CTokenVector kTokenVector;

	for (size_t i = 0; i < fileLoader.GetLineCount(); ++i)
	{
		if (!fileLoader.SplitLineByTab(i, &kTokenVector))
			continue;

		if (kTokenVector.size() < 2)
			continue;

		const uint32_t vnum = (uint32_t)atoi(kTokenVector[0].c_str());
		std::string name = kTokenVector[1];
		trim(name);

		if (vnum)
		{
			CRaceManager::Instance().RegisterRaceName(vnum, name.c_str());
		}
		else
		{
			if (kTokenVector.size() < 3)
				continue;
			std::string src = kTokenVector[2];
			trim(src);
			CRaceManager::Instance().RegisterRaceSrcName(name, src);
		}
	}
}

void CPythonPlayerSettingsModule::__RegisterDungeonMapNames()
{
	// metin2_map_spiderdungeon: baerim haritasi zemini terrain ile ciziyor (baerim.exe listesinde de yok);
	// listede kalirsa EnableTerrainOnlyForHeight terrain cizimini kapatir, zemin gokyuzu gibi gorunur.
	CPythonBackground::Instance().RegisterDungeonMapName("metin2_map_monkeydungeon");
	CPythonBackground::Instance().RegisterDungeonMapName("metin2_map_monkeydungeon_02");
	CPythonBackground::Instance().RegisterDungeonMapName("metin2_map_monkeydungeon_03");
	CPythonBackground::Instance().RegisterDungeonMapName("metin2_map_deviltower1");
}

void CPythonPlayerSettingsModule::__RegisterTitleNames()
{
	for (size_t index = 0; index < titleNameList.size(); ++index)
	{
		CInstanceBase::RegisterTitleName(index, GetLocaleString(titleNameList[index]));
	}
}

void CPythonPlayerSettingsModule::__RegisterNameColors()
{
	for (const auto &[index, rgbTuple] : characterNameColors)
	{
		const auto &[r, g, b] = rgbTuple;
		CInstanceBase::RegisterNameColor(index, r, g, b);
	}
}

void CPythonPlayerSettingsModule::__RegisterTitleColors()
{
	for (const auto &[index, rgbTuple] : characterTitleColors)
	{
		const auto &[r, g, b] = rgbTuple;
		CInstanceBase::RegisterTitleColor(index, r, g, b);
	}
}

void CPythonPlayerSettingsModule::__RegisterEmotionIcons()
{
	for (const auto& [index, imagePath] : emotionIcons)
	{
		CGraphicImage* pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(imagePath);
		m_kMap_iEmotionIndex_pkIconImage.insert(std::make_pair(index, pImage));
	}
}

void CPythonPlayerSettingsModule::RegisterSkills()
{
	CPythonNetworkStream& net = CPythonNetworkStream::Instance();
	RegisterSkills(net.GetMainActorRace(), net.GetMainActorSkillGroup(), net.GetMainActorEmpire());
}

void CPythonPlayerSettingsModule::RegisterSkills(uint32_t race, uint8_t skill_group, uint8_t empire)
{
	const uint32_t playerRace = race;
	const uint32_t playerSkillGroup = skill_group;
	const uint8_t playerJob = RaceToJob(playerRace);
	const uint32_t playerEmpire = empire;

	if (skillIndexes.find(playerJob) != skillIndexes.end() &&
		skillIndexes.at(playerJob).find(skill_group) != skillIndexes.at(playerJob).end())
	{
		std::vector<uint8_t> playerSkills = skillIndexes.at(playerJob).at(playerSkillGroup);

		for (size_t slot = 0; slot < playerSkills.size(); ++slot)
			CPythonPlayer::Instance().SetSkill(slot + 1, playerSkills[slot]);
	}

	const auto supportSkills = supportSkillIndexes;
	for (size_t slot = 0; slot < supportSkills.size(); ++slot)
		CPythonPlayer::Instance().SetSkill(slot + 100 + 1, supportSkills[slot]);

#ifdef DEFAULT_LANGUAGE_SKILLS
	if (playerEmpire)
	{
		for (size_t i = 0; i < 3; ++i)
		{
			if ((i+1) != playerEmpire)
			{
				CPythonPlayer::Instance().SetSkill(109+i, c_iSkillIndex_Language1+i);
			}
		}
	}
#endif

	auto passiveSkills = guildSkills.at("PASSIVE");
	for (size_t j = 0; j < passiveSkills.size(); ++j)
		CPythonPlayer::Instance().SetSkill(200 + j, passiveSkills[j]);

	auto activeSkills = guildSkills.at("ACTIVE");
	for (size_t k = 0; k < activeSkills.size(); ++k)
		CPythonPlayer::Instance().SetSkill(210 + k, activeSkills[k]);
}

int32_t CPythonPlayerSettingsModule::GetCharacterTitleColor(uint8_t index)
{
	const auto &[r, g, b] = characterTitleColors.at(index);
	return CPythonGraphic::Instance().GenerateColor(r, g, b, 1.0);
}

bool CPythonPlayerSettingsModule::RegisterGuildBuildingList(const char* filepath)
{
	CMappedFile File;
	LPCVOID pData;

	if (!CEterPackManager::Instance().Get(File, filepath, &pData))
		return false;

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);

	PyObject* pyGuildData = PyList_New(0);

	CTokenVector TokenVector;
	for (size_t i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLineByTab(i, &TokenVector))
			continue;

		if (TokenVector.size() < GUIILD_BUILDING_LIMIT_TOKEN_COUNT)
			continue;

		std::string& strVnum = TokenVector[GUIILD_BUILDING_TOKEN_VNUM];

		if (strVnum.find_first_not_of("0123456789") != std::string::npos)
			continue;

		const int32_t& id = atoi(strVnum.c_str());
		const std::string_view type = TokenVector[GUIILD_BUILDING_TOKEN_TYPE];
		const char* name = TokenVector[GUIILD_BUILDING_TOKEN_NAME].c_str();
		const char* localName = TokenVector[GUIILD_BUILDING_TOKEN_LOCAL_NAME].c_str();
		const int32_t& xRotLimit = atoi(TokenVector[GUIILD_BUILDING_TOKEN_X_ROT_LIMIT].c_str());
		const int32_t& yRotLimit = atoi(TokenVector[GUIILD_BUILDING_TOKEN_Y_ROT_LIMIT].c_str());
		const int32_t& zRotLimit = atoi(TokenVector[GUIILD_BUILDING_TOKEN_Z_ROT_LIMIT].c_str());
		const int32_t& price = atoi(TokenVector[GUIILD_BUILDING_TOKEN_PRICE].c_str());
		const std::string_view material = TokenVector[GUIILD_BUILDING_TOKEN_MATERIAL];
		const int32_t& enableType = atoi(TokenVector[GUIILD_BUILDING_TOKEN_ENABLE_FLAG].c_str());

		if (enableType == 0)
			continue;

		// Register material
		const std::string& folderName = m_buildingTypeToFolder[type];

		CRaceManager::Instance().RegisterRaceSrcName(name, folderName);
		CRaceManager::Instance().RegisterRaceName(id, name);

		// Create building material List
		std::vector<std::string> materialVector;
		boost::split(materialVector, material, boost::is_any_of("/"));

		PyObject* pyMaterialList = PyList_New(3);

		for (size_t j = 0; j < GUILD_MATERIAL_NUM; ++j)
		{
			PyList_SetItem(pyMaterialList, j, Py_BuildValue("s", "0"));
		}

		for (const std::string& itemPair : materialVector)
		{
			std::vector<std::string> itemVector;
			boost::split(itemVector, itemPair, boost::is_any_of(","));

			if (itemVector.size() != 2)
				continue;

			const int32_t& vnum = atoi(itemVector[0].c_str());
			const std::string& count = itemVector[1];

			const uint8_t index = getGuildMaterialIndex(vnum);

			if (index == -1)
			{
				TraceError("Found strange guild material: %d", vnum);
				continue;
			}

			PyList_SetItem(pyMaterialList, index, Py_BuildValue("s", count.c_str()));
		}

		// Create guild data dict and append to list
		PyObject* pyGuildDataItem = PyDict_New();
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "VNUM"), Py_BuildValue("i", id));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "TYPE"), Py_BuildValue("s", type.data()));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "NAME"), Py_BuildValue("s", name));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "LOCAL_NAME"), Py_BuildValue("s", localName));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "X_ROT_LIMIT"), Py_BuildValue("i", xRotLimit));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "Y_ROT_LIMIT"), Py_BuildValue("i", yRotLimit));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "Z_ROT_LIMIT"), Py_BuildValue("i", zRotLimit));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "PRICE"), Py_BuildValue("i", price));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "MATERIAL"), Py_BuildValue("O", pyMaterialList));
		PyDict_SetItem(pyGuildDataItem, Py_BuildValue("s", "SHOW"), Py_BuildValue("i", enableType != 2));
		PyList_Append(pyGuildData, pyGuildDataItem);
	}

	CPythonNetworkStream::Instance().SendPythonData(pyGuildData, "BINARY_SetGuildBuildingList");
	return true;
}
