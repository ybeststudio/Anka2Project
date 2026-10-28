#pragma once

#pragma warning(disable:4067)
#pragma warning(disable:4172)
#pragma warning(disable:4267)
#pragma warning(disable:4305)
#pragma warning(disable:4834)
#pragma warning(disable:4702)
#pragma warning(disable:4100)
#pragma warning(disable:4201)
#pragma warning(disable:4511)
#pragma warning(disable:4663)
#pragma warning(disable:4018)
#pragma warning(disable:4245)
#pragma warning(disable:5033)
#pragma warning(disable:4838)
#pragma warning(disable:4995)
#pragma warning(disable:4474)
#pragma warning(disable:4267)
#pragma warning(disable:4267)
#pragma warning(disable:4005)
#pragma warning(disable:4101)
#pragma warning(disable:4715)

#if _MSC_VER >= 1400
//if don't use below, time_t is 64bit
	#ifndef _WIN64
		#define _USE_32BIT_TIME_T
	#endif
#endif

#include "../EterLib/StdAfx.h"
#include "../EterPythonLib/StdAfx.h"
#include "../GameLib/StdAfx.h"
#include "../ScriptLib/StdAfx.h"
#include "../MilesLib/StdAfx.h"
#include "../EffectLib/StdAfx.h"
#include "../PRTerrainLib/StdAfx.h"
#include "../SpeedTreeLib/StdAfx.h"

#ifndef __D3DRM_H__
	#define __D3DRM_H__
#endif

#include <array>
#include <dshow.h>
#include <qedit.h>

#include "Locale.h"
#include "../UserInterface/Locale_inc.h"

#include "GameType.h"
extern DWORD __DEFAULT_CODE_PAGE__;

#define APP_NAME	"Metin 2"

enum
{
	POINT_MAX_NUM = 255,
	CHARACTER_NAME_MAX_LEN = 48,
	PLAYER_NAME_MAX_LEN = 12,
};

void initudp();
#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
void initPythonApi();
#endif
void initapp();
void initime();
void initsystem();
void initchr();
void initchrmgr();
void initChat();
void initTextTail();
void initime();
void initItem();
void initNonPlayer();
void initnet();
void initPlayer();
void initSectionDisplayer();
void initServerStateChecker();
void initTrade();
void initMiniMap();
void initProfiler();
void initEvent();
void initeffect();
void initsnd();
void initeventmgr();
void initBackground();
void initwndMgr();
void initshop();
void initpack();
void initskill();
void initfly();
void initquest();
void initsafebox();
void initguild();
void initMessenger();
#ifdef ENABLE_GUILD_RANK_SYSTEM
void initguildranking();
#endif
#ifdef ENABLE_CONFIG_MODULE
void initcfg();
#endif
#ifdef ENABLE_RENEWAL_SWITCHBOT
void initSwitchbot();
#endif
#ifdef ENABLE_RENEWAL_CUBE
void intcuberenewal();
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
void initAcce();
#endif
#ifdef ENABLE_RENDER_TARGET
void initRenderTarget();
#endif
#ifdef ENABLE_WIKI_SYSTEM
void initWiki();
#endif
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
void initprivateShopSearch();
#endif
#ifdef ENABLE_RIDING_EXTENDED
void initmountupgrade();
#endif
#ifdef ENABLE_DUNGEON_INFO
void intdungeoninfo();
#endif
#ifdef ENABLE_EVENT_SYSTEM
void initGameEvents();
#endif
#ifdef __AUTO_HUNT__
extern float GetDistanceNew(const TPixelPosition& PixelPosition, const TPixelPosition& c_rPixelPosition);
extern void split_argument(const char* argument, std::vector<std::string>& vecArgs, const char* arg = " ");
#endif