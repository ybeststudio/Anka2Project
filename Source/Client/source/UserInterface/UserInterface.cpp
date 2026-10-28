#include "StdAfx.h"
#include "PythonApplication.h"
#include "ProcessScanner.h"
#include "PythonExceptionSender.h"
#include "resource.h"
#include "Version.h"
#include "PythonStatusEffectManager.h"
#include "PythonYinAndYangAttrManager.h"
#ifdef ENABLE_SHOP_REPURCHASE
#include "PythonRepurchaseShop.h"
#endif

#ifdef _DEBUG
#include <crtdbg.h>
#endif
#pragma comment( lib, "gif.lib" )

#include "../eterPack/EterPackManager.h"
#include "../eterLib/Util.h"
#include "../CWebBrowser/CWebBrowser.h"
#include "../eterBase/CPostIt.h"

#ifdef ENABLE_CONFIG_MODULE
	#include "PythonConfig.h"
#endif

#ifdef ENABLE_CLIENT_PERFORMANCE
	#include "PythonPlayerSettingsModule.h"
#endif

#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
	#include "PythonDynamicModuleNames.h"
#endif

extern "C" {
extern int _fltused;
volatile int _AVOID_FLOATING_POINT_LIBRARY_BUG = _fltused;
};

extern "C" { FILE __iob_func[3] = {*stdin,*stdout,*stderr }; }

extern "C" {
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

#pragma comment(linker, "/NODEFAULTLIB:libci.lib")
#pragma comment(linker, "/NODEFAULTLIB:msvcrt.lib")

#pragma comment( lib, "version.lib" )
#pragma comment( lib, "python27.lib" )
#pragma comment( lib, "imagehlp.lib" )
#pragma comment( lib, "devil.lib" )
#pragma comment( lib, "granny2.lib" )
#pragma comment( lib, "mss32.lib" )
#pragma comment( lib, "winmm.lib" )
#pragma comment( lib, "imm32.lib" )
#pragma comment( lib, "oldnames.lib" )
#pragma comment( lib, "SpeedTreeRT.lib" )
#pragma comment( lib, "dinput8.lib" )
#pragma comment( lib, "dxguid.lib" )
#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "strmiids.lib" )
#pragma comment( lib, "ddraw.lib" )
#pragma comment( lib, "dmoguids.lib" )

#ifdef ENABLE_FOXFS_ENCRYPT
	#pragma comment( lib, "lz4.lib" )
	#pragma comment( lib, "xxhash.lib" )
	#ifndef _DEBUG
		#pragma comment(lib,"FoxFS.lib")
	#else
		#pragma comment(lib,"FoxFS_d.lib")
	#endif
	#pragma comment( lib, "Iphlpapi.lib" )
#endif

#include <stdlib.h>
#include <cryptopp/cryptoppLibLink.h>

extern bool SetDefaultCodePage(DWORD codePage);

static const char *sc_apszPythonLibraryFilenames[] =
{
	"UserDict.pyc",
	"__future__.pyc",
	"copy_reg.pyc",
	"linecache.pyc",
	"ntpath.pyc",
	"os.pyc",
	"site.pyc",
	"stat.pyc",
	"string.pyc",
	"traceback.pyc",
	"types.pyc",
	"\n",
};

bool CheckPythonLibraryFilenames()
{
	for (int i = 0;*sc_apszPythonLibraryFilenames[i] != '\n'; ++i)
	{
		std::string stFilename = "lib\\";
		stFilename += sc_apszPythonLibraryFilenames[i];

		if (_access(stFilename.c_str(), 0) != 0)
		{
			return false;
		}

		MoveFile(stFilename.c_str(), stFilename.c_str());
	}

	return true;
}

int Setup(LPSTR lpCmdLine);

#ifdef ENABLE_FOXFS_ENCRYPT
bool PackInitialize(const char *c_pszFolder)
{
	if (_access(c_pszFolder, 0) != 0)
		return true;

	std::string stFolder(c_pszFolder);
	stFolder += "/";

	CTextFileLoader::SetCacheMode();
	CEterPackManager::Instance().SetCacheMode();
	CEterPackManager::Instance().SetSearchMode(CEterPackManager::SEARCH_PACK);

	CSoundData::SetPackMode();

	// Baerim pack duzeni (baerim.exe kayit sirasi) + anka2 overlay (exe'nin zorunlu terrain normal map'leri).
	// FoxFS arsivleri ad sirasiyla gezer; baerim onceligi ("ilk kaydedilen kazanir") Pack Studio'da
	// settings.pack_priority ile build sirasinda tekillestirilerek uygulanir, burada sira sadece okunurluk icin.
	static const char* c_aszPacks[] =
	{
		"anka2",
		"special_patch_animation", "special_patch_common", "special_patch_ex", "special_patch_hair",
		"icon", "locale", "maps", "property", "season", "sound", "textureset",
		"yw_etc", "yw_etc2", "yw_monster", "yw_monster2", "yw_npc", "yw_npc_mount", "yw_npc_pet", "yw_npc2",
		"yw_pc", "yw_pc2", "yw_zone",
		"special_patch_maliml", "saryong_ui", "saryong_effect", "saryong_map", "saryong_map2",
		"saryong_costume", "saryong_costume2", "saryong_christmas", "saryong_chapter2",
		"yw_effect",
	};
	for (const char* c_szPack : c_aszPacks)
		CEterPackManager::Instance().RegisterPack((stFolder + c_szPack).c_str(), "*");

	CEterPackManager::Instance().RegisterRootPack((stFolder + std::string("root")).c_str());
	return true;
}
#else
#ifdef ENABLE_LOAD_INDEX_BINARY
bool PackInitialize(const char *c_pszFolder)
{
	std::vector<std::vector<std::string>> indexVec
	{
		{ "pack/", "bgm" },
		{ "d:/ymir work/pc/", "characters" },
		{ "d:/ymir work/pc2/", "characters" },
		{ "d:/ymir work/effect/", "effect" },
		{ "d:/ymir work/guild/", "guild" },
		{ "icon/", "icon" },
		{ "d:/ymir work/item/", "item" },
		{ "locale/ro/", "locale" },
		{ "d:/ymir work/monster/", "monster" },
		{ "d:/ymir work/monster2/", "monster" },
		{ "d:/ymir work/npc/", "npc" },
		{ "d:/ymir work/npc2/", "npc" },
		{ "d:/ymir work/environment/", "etc" },
		{ "d:/ymir work/environment/", "etc2" },
		{ "d:/ymir work/special/", "etc" },
		{ "d:/ymir work/special/", "etc2" },
		{ "d:/ymir work/ui/", "etc" },
		{ "d:/ymir work/ui/", "etc2" },
		{ "gm_guild_build/", "maps" },
		{ "maimetin2_map_empirewar_a01/", "maps" },
		{ "map_a2/", "maps" },
		{ "map_b_fielddungeon/", "maps" },
		{ "map_n_snowm_01/", "maps" },
		{ "map_n_snowm_02/", "maps" },
		{ "metin2_guild_village/", "maps" },
		{ "metin2_guild_village_01/", "maps" },
		{ "metin2_guild_village_02/", "maps" },
		{ "metin2_guild_village_03/", "maps" },
		{ "metin2_map/", "maps" },
		{ "metin2_map_a1/", "maps" },
		{ "metin2_map_a2_1/", "maps" },
		{ "metin2_map_a3/", "maps" },
		{ "metin2_map_b1/", "maps" },
		{ "metin2_map_b3/", "maps" },
		{ "metin2_map_bayblacksand/", "maps" },
		{ "metin2_map_c1/", "maps" },
		{ "metin2_map_c3/", "maps" },
		{ "metin2_map_capedragonhead/", "maps" },
		{ "metin2_map_dawnmistwood/", "maps" },
		{ "metin2_map_devilscatacomb/", "maps" },
		{ "metin2_map_deviltower1/", "maps" },
		{ "metin2_map_duel/", "maps" },
		{ "metin2_map_empirewar_a01/", "maps" },
		{ "metin2_map_empirewar01/", "maps" },
		{ "metin2_map_empirewar02/", "maps" },
		{ "metin2_map_empirewar03/", "maps" },
		{ "metin2_map_ew02/", "maps" },
		{ "metin2_map_guild_01/", "maps" },
		{ "metin2_map_guild_02/", "maps" },
		{ "metin2_map_guild_03/", "maps" },
		{ "metin2_map_guild_inside01/", "maps" },
		{ "metin2_map_milgyo/", "maps" },
		{ "metin2_map_milgyo_a/", "maps" },
		{ "metin2_map_monkeydungeon/", "maps" },
		{ "metin2_map_monkeydungeon_02/", "maps" },
		{ "metin2_map_monkeydungeon_03/", "maps" },
		{ "metin2_map_mt_thunder/", "maps" },
		{ "metin2_map_n_desert_01/", "maps" },
		{ "metin2_map_n_desert_02/", "maps" },
		{ "metin2_map_n_flame_01/", "maps" },
		{ "metin2_map_n_flame_02/", "maps" },
		{ "metin2_map_n_flame_dungeon_01/", "maps" },
		{ "metin2_map_n_snow_dungeon_01/", "maps" },
		{ "metin2_map_nusluck01/", "maps" },
		{ "metin2_map_oxevent/", "maps" },
		{ "metin2_map_shinsutest_01/", "maps" },
		{ "metin2_map_siege_01/", "maps" },
		{ "metin2_map_siege_02/", "maps" },
		{ "metin2_map_siege_03/", "maps" },
		{ "metin2_map_skipia_bossdungeon/", "maps" },
		{ "metin2_map_skipia_dungeon_01/", "maps" },
		{ "metin2_map_skipia_dungeon_02/", "maps" },
		{ "metin2_map_skipia_dungeon_boss/", "maps" },
		{ "metin2_map_spider_bossdungeon/", "maps" },
		{ "metin2_map_spiderdungeon/", "maps" },
		{ "metin2_map_spiderdungeon_02/", "maps" },
		{ "metin2_map_spiderdungeon_03/", "maps" },
		{ "metin2_map_sungzi/", "maps" },
		{ "metin2_map_sungzi_desert_01/", "maps" },
		{ "metin2_map_sungzi_desert_hill_01/", "maps" },
		{ "metin2_map_sungzi_desert_hill_02/", "maps" },
		{ "metin2_map_sungzi_desert_hill_03/", "maps" },
		{ "metin2_map_sungzi_flame_hill_01/", "maps" },
		{ "metin2_map_sungzi_flame_hill_02/", "maps" },
		{ "metin2_map_sungzi_flame_hill_03/", "maps" },
		{ "metin2_map_sungzi_milgyo/", "maps" },
		{ "metin2_map_sungzi_milgyo_pass_01/", "maps" },
		{ "metin2_map_sungzi_milgyo_pass_02/", "maps" },
		{ "metin2_map_sungzi_milgyo_pass_03/", "maps" },
		{ "metin2_map_sungzi_snow/", "maps" },
		{ "metin2_map_sungzi_snow_pass01/", "maps" },
		{ "metin2_map_sungzi_snow_pass02/", "maps" },
		{ "metin2_map_sungzi_snow_pass03/", "maps" },
		{ "metin2_map_t1/", "maps" },
		{ "metin2_map_t2/", "maps" },
		{ "metin2_map_t3/", "maps" },
		{ "metin2_map_t4/", "maps" },
		{ "metin2_map_trent/", "maps" },
		{ "metin2_map_trent_a/", "maps" },
		{ "metin2_map_trent02/", "maps" },
		{ "metin2_map_trent02_a/", "maps" },
		{ "metin2_map_wedding_01/", "maps" },
		{ "metin2_map_wl_01/", "maps" },
		{ "property/", "property" },
		{ "uiscript/", "root" },
		{ "sound/ambience/", "sound" },
		{ "sound/common/", "sound" },
		{ "sound/effect/", "sound" },
		{ "sound/monster/", "sound" },
		{ "sound/monster2/", "sound" },
		{ "sound/npc/", "sound" },
		{ "sound/npc2/", "sound" },
		{ "sound/pc/", "sound" },
		{ "sound/pc2/", "sound" },
		{ "sound/ui/", "sound" },
		{ "sound/grass/", "terrainmaps" },
		{ "d:/ymir work/terrainmaps/", "terrainmaps" },
		{ "textureset/", "textureset" },
		{ "d:/ymir work/tree/", "tree" },
		{ "d:/ymir work/zone/", "zone" },
	};

	if (_access(c_pszFolder, 0) != 0)
		return true;

	std::string stFolder(c_pszFolder);
	stFolder += "/";

	const bool bPackFirst = TRUE;

	CEterPackManager::Instance().SetCacheMode();
	CEterPackManager::Instance().SetSearchMode(bPackFirst);

	CSoundData::SetPackMode();

	std::string strPackName, strTexCachePackName;
	for (auto& elem : indexVec)
	{
		const std::string& c_rstFolder = elem[0];
		const std::string& c_rstName = elem[1];

		strPackName = stFolder + c_rstName;
		strTexCachePackName = strPackName + "_texcache";

		CEterPackManager::Instance().RegisterPack(strPackName.c_str(), c_rstFolder.c_str());
		CEterPackManager::Instance().RegisterPack(strTexCachePackName.c_str(), c_rstFolder.c_str());
	}

	CEterPackManager::Instance().RegisterRootPack((stFolder + std::string("root")).c_str());

	NANOEND

		return true;
}
#else
bool PackInitialize(const char * c_pszFolder)
{
	NANOBEGIN
	if (_access(c_pszFolder, 0) != 0)
		return true;

	std::string stFolder(c_pszFolder);
	stFolder += "/";

	std::string stFileName(stFolder);
	stFileName += "Index";

	CMappedFile file;
	LPCVOID pvData;

	if (!file.Create(stFileName.c_str(), &pvData, 0, 0))
	{
		LogBoxf("FATAL ERROR! File not exist: %s", stFileName.c_str());
		TraceError("FATAL ERROR! File not exist: %s", stFileName.c_str());
		return true;
	}

	CMemoryTextFileLoader TextLoader;
	TextLoader.Bind(file.Size(), pvData);

	bool bPackFirst = TRUE;

	const std::string & strPackType = TextLoader.GetLineString(0);

	if (strPackType.compare("FILE") && strPackType.compare("PACK"))
	{
		TraceError("Pack/Index has invalid syntax. First line must be 'PACK' or 'FILE'");
		return false;
	}

#ifdef NDEBUG // @warme601 _DISTRIBUTE -> NDEBUG
	Tracef("Note: PackFirst mode enabled. [pack]\n");
#else
	bPackFirst = FALSE;
	Tracef("Note: PackFirst mode not enabled. [file]\n");
#endif

	CTextFileLoader::SetCacheMode();
	CEterPackManager::Instance().SetCacheMode();
	CEterPackManager::Instance().SetSearchMode(bPackFirst);

	CSoundData::SetPackMode();

	std::string strPackName, strTexCachePackName;
	for (DWORD i = 1; i < TextLoader.GetLineCount() - 1; i += 2)
	{
		const std::string & c_rstFolder = TextLoader.GetLineString(i);
		const std::string & c_rstName = TextLoader.GetLineString(i + 1);

		strPackName = stFolder + c_rstName;
		strTexCachePackName = strPackName + "_texcache";

		CEterPackManager::Instance().RegisterPack(strPackName.c_str(), c_rstFolder.c_str());
		CEterPackManager::Instance().RegisterPack(strTexCachePackName.c_str(), c_rstFolder.c_str());
	}

	CEterPackManager::Instance().RegisterRootPack((stFolder + std::string("root")).c_str());
	NANOEND
	return true;
}
#endif
#endif

bool RunMainScript(CPythonLauncher& pyLauncher, const char *lpCmdLine)
{
#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
	initPythonApi();
#endif
	initpack();
	initdbg();
	initime();
	initgrp();
	initgrpImage();
	initgrpText();
	initwndMgr();
	initudp();
	initapp();
	initsystem();
	initchr();
	initchrmgr();
	initPlayer();
	initItem();
	initNonPlayer();
	initTrade();
	initChat();
	initTextTail();
	initnet();
	initMiniMap();
	initProfiler();
	initEvent();
	initeffect();
	initfly();
	initsnd();
	initeventmgr();
	initshop();
	initskill();
	initquest();
	initBackground();
	initMessenger();
	initsafebox();
	initguild();
	initServerStateChecker();
#ifdef ENABLE_STATUS_EFFECT_SHOWER
	initStatusEffectManager();
#endif
#ifdef ENABLE_YIN_AND_YANG_ATTR
	inityinAndYangAttr();
#endif
#ifdef ENABLE_SHOP_REPURCHASE
	initrepurchaseShop();
#endif
#ifdef ENABLE_GUILD_RANK_SYSTEM
	initguildranking();
#endif
#ifdef ENABLE_CONFIG_MODULE
	initcfg();
#endif
#ifdef ENABLE_RENEWAL_SWITCHBOT
	initSwitchbot();
#endif
#ifdef ENABLE_RENEWAL_CUBE
	intcuberenewal();
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	initAcce();
#endif
#ifdef ENABLE_RENDER_TARGET
	initRenderTarget();
#endif
#ifdef ENABLE_WIKI_SYSTEM
	initWiki();
#endif
#ifdef ENABLE_OFFLINESHOP_SEARCH_SYSTEM
	initprivateShopSearch();
#endif
#ifdef ENABLE_RIDING_EXTENDED
	initmountupgrade();
#endif
#ifdef ENABLE_DUNGEON_INFO
	intdungeoninfo();
#endif
#ifdef ENABLE_EVENT_SYSTEM
	initGameEvents();
#endif

	NANOBEGIN

	PyObject *builtins = PyImport_ImportModule("__builtin__");
#ifdef _DEBUG
	PyModule_AddIntConstant(builtins, "__DEBUG__", 1);
#else
	PyModule_AddIntConstant(builtins, "__DEBUG__", 0);
#endif
#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
	PyModule_AddIntConstant(builtins, "__USE_DYNAMIC_MODULE__", 1);
#else
	PyModule_AddIntConstant(builtins, "__USE_DYNAMIC_MODULE__", 0);
#endif

	{
		std::string stRegisterCmdLine;

		const char *loginMark = "-cs";
		const char *loginMark_NonEncode = "-ncs";
		const char *seperator = " ";

		std::string stCmdLine;
		const int CmdSize = 3;
		std::vector<std::string> stVec;
		SplitLine(lpCmdLine,seperator,&stVec);

		if (CmdSize == stVec.size() && stVec[0]==loginMark)
		{
			char buf[MAX_PATH];
			base64_decode(stVec[2].c_str(),buf);
			stVec[2] = buf;
			string_join(seperator,stVec,&stCmdLine);
		}
		else if (CmdSize <= stVec.size() && stVec[0]==loginMark_NonEncode)
		{
			stVec[0] = loginMark;
			string_join(" ",stVec,&stCmdLine);
		}
		else
			stCmdLine = lpCmdLine;

		stRegisterCmdLine ="__COMMAND_LINE__ = ";
		stRegisterCmdLine+='"';
		stRegisterCmdLine+=stCmdLine;
		stRegisterCmdLine+='"';

		const CHAR* c_szRegisterCmdLine=stRegisterCmdLine.c_str();
		if (!pyLauncher.RunLine(c_szRegisterCmdLine))
		{
			TraceError("RegisterCommandLine Error");
			return false;
		}
	}
	{
		std::vector<std::string> stVec;
		SplitLine(lpCmdLine," " ,&stVec);

		if (stVec.size() != 0 && "--pause-before-create-window" == stVec[0])
		{
			system("pause");
		}
		if (!pyLauncher.RunFile("system.py"))
		{
			TraceError("RunMain Error");
			return false;
		}
	}

	NANOEND
	return true;
}

bool Main(HINSTANCE hInstance, LPSTR lpCmdLine)
{
	DWORD dwRandSeed = (DWORD)time(nullptr) + GetCurrentProcessId();
	srandom(dwRandSeed);
	srand(random());
	SetLogLevel(1);

	ilInit();

	if (!Setup(lpCmdLine))
		return false;

#ifdef _DEBUG
	OpenConsoleWindow();
#endif
	OpenLogFile(true); // true == uses syserr.txt and log.txt

	static CLZO lzo;
	static CEterPackManager EterPackManager;
#ifdef ENABLE_CLIENT_PERFORMANCE
	static CPythonPlayerSettingsModule PlayerSettings;
#endif

#ifdef ENABLE_CONFIG_MODULE
	static CPythonConfig m_pyConfig;
	m_pyConfig.Initialize("config.cfg");
#endif

	if (!PackInitialize("pack"))
	{
		LogBox("Pack Initialization failed. Check log.txt file..");
		return false;
	}

	CPythonApplication* app = new CPythonApplication;

	app->Initialize(hInstance);

	bool ret=false;
	{
		CPythonLauncher pyLauncher;
		CPythonExceptionSender pyExceptionSender;
		SetExceptionSender(&pyExceptionSender);

		if (pyLauncher.Create())
		{
			ret=RunMainScript(pyLauncher, lpCmdLine);
		}

		app->Clear();

		timeEndPeriod(1);
		pyLauncher.Clear();
	}

	app->Destroy();
	delete app;

	return ret;
}

HANDLE CreateMetin2GameMutex()
{
	SECURITY_ATTRIBUTES sa;
	ZeroMemory(&sa, sizeof(SECURITY_ATTRIBUTES));
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = nullptr;
	sa.bInheritHandle = FALSE;

	return CreateMutex(&sa, FALSE, "Metin2GameMutex");
}

void DestroyMetin2GameMutex(HANDLE hMutex)
{
	if (hMutex)
	{
		ReleaseMutex(hMutex);
		hMutex = nullptr;
	}
}

void __ErrorPythonLibraryIsNotExist()
{
	LogBoxf("FATAL ERROR!! Python Library file not exist!");
}

bool __IsTimeStampOption(LPSTR lpCmdLine)
{
	const char *TIMESTAMP = "/timestamp";
	return (strncmp(lpCmdLine, TIMESTAMP, strlen(TIMESTAMP))==0);
}

void __PrintTimeStamp()
{
#ifdef _DEBUG
	LogBoxf("METIN2 BINARY DEBUG VERSION %s ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);
#else
	LogBoxf("METIN2 BINARY DISTRIBUTE VERSION %s ( MS C++ %d Compiled )", __TIMESTAMP__, _MSC_VER);
#endif
}

bool __IsLocaleOption(LPSTR lpCmdLine)
{
	return (strcmp(lpCmdLine, "--locale") == 0);
}

bool __IsLocaleVersion(LPSTR lpCmdLine)
{
	return (strcmp(lpCmdLine, "--perforce-revision") == 0);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LoadConfig ("locale.cfg");
	SetDefaultCodePage(GetCodePage());

	int nArgc = 0;
	PCHAR* szArgv = CommandLineToArgv( lpCmdLine, &nArgc );
	WebBrowser_Startup(hInstance);

	if (!CheckPythonLibraryFilenames())
	{
		__ErrorPythonLibraryIsNotExist();
		goto Clean;
	}

	Main(hInstance, lpCmdLine);
	WebBrowser_Cleanup();
	::CoUninitialize();

Clean:
	SAFE_FREE_GLOBAL(szArgv);
	return 0;
}

static void GrannyError(granny_log_message_type Type, granny_log_message_origin Origin, char const* File, granny_int32x Line, char const* Message, void* UserData)
{
	TraceError("GRANNY: %s", Message);
}

int Setup(LPSTR lpCmdLine)
{
	TIMECAPS tc;
	UINT wTimerRes;

	if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR)
		return 0;

	wTimerRes = MINMAX(tc.wPeriodMin, 1, tc.wPeriodMax);
	timeBeginPeriod(wTimerRes);

	granny_log_callback Callback;
	Callback.Function = nullptr;
	Callback.UserData = 0;
	GrannySetLogCallback(&Callback);
	return 1;
}
