#pragma once

#ifdef ENABLE_TELEPORT_BAERIM
// Baerim teleport map window (uiTeleport, first tab of the dungeon window): /teleport_map <tpIdx>
// Points come from locale/<locale>/teleport_points.txt: tpIdx x y minLevel maxLevel yang. The client never sends a position.
enum ETeleportMapResult
{
	TELEPORT_MAP_RESULT_OK,
	TELEPORT_MAP_RESULT_INVALID,		// unknown point
	TELEPORT_MAP_RESULT_LEVEL,			// level out of range
	TELEPORT_MAP_RESULT_GOLD,			// not enough yang
	TELEPORT_MAP_RESULT_BUSY,			// dead / window open / warping / fighting
	TELEPORT_MAP_RESULT_NOT_AVAILABLE,	// the target map is not on this server
	TELEPORT_MAP_RESULT_COOLDOWN,		// clicked too fast
};

void TeleportMap_Command(LPCHARACTER ch, const char* argument);
void TeleportMap_Reload();
#endif
