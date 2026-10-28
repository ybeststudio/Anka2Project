#pragma once

#ifdef ENABLE_PYTHON_DYNAMIC_MODULE_NAME
enum {
	NET_MODULE,
	CHR_MODULE,
	CHRMGR_MODULE,
	APP_MODULE,
	PACK_MODULE,
	PLAYER_MODULE,
	MODULE_MAX
};

std::string GetModuleName(int iModuleCode);
#endif
