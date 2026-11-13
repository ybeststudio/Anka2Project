#include "stdafx.h"
#include "../../common/service.h"

#ifdef ENABLE_TELEPORT_BAERIM
#include "utils.h"
#include "config.h"
#include "char.h"
#include "locale_service.h"
#include "sectree_manager.h"
#include "map_location.h"
#include "teleport_window.h"

#include <fstream>
#include <sstream>
#include <map>

namespace
{
	struct STeleportPoint
	{
		long	x;
		long	y;
		int		minLevel;
		int		maxLevel;
		long long yang;
	};

	std::map<DWORD, STeleportPoint>	s_mapPoint;
	bool							s_bLoaded = false;

	void Load()
	{
		s_bLoaded = true;
		s_mapPoint.clear();

		char szPath[256];
		snprintf(szPath, sizeof(szPath), "%s/teleport_points.txt", LocaleService_GetBasePath().c_str());
		std::ifstream in(szPath);
		if (!in.is_open())
		{
			sys_err("teleport map: cannot open %s", szPath);
			return;
		}

		std::string line;
		int iLine = 0;
		while (std::getline(in, line))
		{
			++iLine;
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1);
			if (line.empty() || line[0] == '#')
				continue;

			std::istringstream ss(line);
			DWORD dwIdx = 0;
			STeleportPoint p{};
			if (!(ss >> dwIdx >> p.x >> p.y >> p.minLevel >> p.maxLevel >> p.yang) || p.yang < 0)
			{
				sys_err("teleport map: %s line %d is malformed", szPath, iLine);
				continue;
			}
			s_mapPoint[dwIdx] = p;
		}
		sys_log(0, "teleport map: %u points", (unsigned) s_mapPoint.size());
	}

	void SendResult(LPCHARACTER ch, int iResult, DWORD dwIdx)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TeleportMapResult %d %u", iResult, dwIdx);
	}

	int Teleport(LPCHARACTER ch, DWORD dwIdx)
	{
		if (!s_bLoaded)
			Load();

		std::map<DWORD, STeleportPoint>::const_iterator it = s_mapPoint.find(dwIdx);
		if (it == s_mapPoint.end())
			return TELEPORT_MAP_RESULT_INVALID;
		const STeleportPoint& p = it->second;

		// same rules as the old /warp_on: no warp with a window open, while dead or right after a fight
		if (ch->IsDead() || ch->IsWarping() || !ch->CanWarp() || !ch->CanHandleItem())
			return TELEPORT_MAP_RESULT_BUSY;

		if (ch->GetLevel() < p.minLevel || (p.maxLevel > 0 && ch->GetLevel() > p.maxLevel))
			return TELEPORT_MAP_RESULT_LEVEL;

		if (p.yang > 0 && ch->GetGold() < p.yang)
			return TELEPORT_MAP_RESULT_GOLD;

		// the target must be a map of this server (the 4 Baerim only maps are listed before they are installed)
		long lMapIndex = 0;
		int32_t lAddr = 0;
		WORD wPort = 0;
		if (!SECTREE_MANAGER::instance().GetMapIndex(p.x, p.y) || !CMapLocation::instance().Get(p.x, p.y, lMapIndex, lAddr, wPort))
			return TELEPORT_MAP_RESULT_NOT_AVAILABLE;

		if (p.yang > 0)
			ch->PointChange(POINT_GOLD, -p.yang);

		ch->WarpSet(p.x, p.y);
		return TELEPORT_MAP_RESULT_OK;
	}
}

void TeleportMap_Reload()
{
	Load();
}

void TeleportMap_Command(LPCHARACTER ch, const char* argument)
{
	if (!ch || !ch->GetDesc())
		return;

	char arg1[64], arg2[64];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!strcmp(arg1, "reload"))
	{
		if (ch->GetGMLevel() >= GM_IMPLEMENTOR)
		{
			Load();
			ch->ChatPacket(CHAT_TYPE_INFO, "teleport_points.txt: %u", (unsigned) s_mapPoint.size());
		}
		return;
	}

	DWORD dwIdx = 0;
	if (!*arg1 || !str_to_number(dwIdx, arg1))
		return;

	// one request a second: a double click must not start two warps
	if (ch->GetProtectTime("teleport_map") > get_global_time())
	{
		SendResult(ch, TELEPORT_MAP_RESULT_COOLDOWN, dwIdx);
		return;
	}
	ch->SetProtectTime("teleport_map", get_global_time() + 1);

	const int iResult = Teleport(ch, dwIdx);
	SendResult(ch, iResult, dwIdx);
}
#endif
