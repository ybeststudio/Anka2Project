#pragma once
#include "StdAfx.h"

#ifdef ENABLE_DISCORD_RPC
#include "PythonCharacterManager.h"
#include "PythonBackground.h"
#include "PythonPlayer.h"
#include "PythonGuild.h"

namespace Discord
{
	inline constexpr auto DiscordClientID = "1022848045039230976";

	using DCDATA = std::pair<std::string, std::string>;

	inline DCDATA GetNameData()
	{
		auto WarpName = std::string(CPythonBackground::Instance().GetWarpMapName());

		static const std::unordered_map<std::string, std::string> m_MapName {
			{ "metin2_map_a1", "Yongan" },
			{ "metin2_map_b1", "Joan" },
			{ "metin2_map_c1", "Pyungmoo" },
		};

		std::string MapName = "Location: ";

		auto it = m_MapName.find(WarpName);
		if (it != m_MapName.end())
			MapName += it->second;
		else
			MapName += WarpName;

		auto CharacterName = "Name: " + std::string(CPythonPlayer::Instance().GetName());
		std::string GuildName;
		if (CPythonGuild::Instance().GetGuildName(CPythonPlayer::Instance().GetGuildID(), &GuildName))
			CharacterName += "-Guild: " + GuildName;

		return { MapName, CharacterName };
	}

	inline DCDATA GetRaceData()
	{
		auto pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
		if (!pInstance)
			return { "","" };

		auto RACENUM = pInstance->GetRace();

		auto RaceImage = "race_" + std::to_string(RACENUM);

		auto RaceName = "";
		switch (RACENUM)
		{
			case NRaceData::JOB_WARRIOR:
			case NRaceData::JOB_WARRIOR + 4:
				RaceName = "Warrior";
				break;
			case NRaceData::JOB_ASSASSIN:
			case NRaceData::JOB_ASSASSIN + 4:
				RaceName = "Assassin";
				break;
			case NRaceData::JOB_SURA:
			case NRaceData::JOB_SURA + 4:
				RaceName = "Sura";
				break;
			case NRaceData::JOB_SHAMAN:
			case NRaceData::JOB_SHAMAN + 4:
				RaceName = "Shaman";
				break;
		}
		return { RaceImage , RaceName };
	}

	inline DCDATA GetEmpireData()
	{
		auto pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
		if (!pInstance)
			return { "","" };

		auto EmpireID = pInstance->GetEmpireID();

		auto EmpireImage = "empire_" + std::to_string(EmpireID);

		std::string EmpireName = "";
		switch (EmpireID)
		{
		case 1:
			EmpireName = "Shinsoo";
			break;
		case 2:
			EmpireName = "Chunjo";
			break;
		case 3:
			EmpireName = "Jinno";
			break;
		}
		return { EmpireImage, EmpireName };
	}
}
#endif