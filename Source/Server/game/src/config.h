#ifndef __INC_METIN_II_GAME_CONFIG_H__
#define __INC_METIN_II_GAME_CONFIG_H__

enum
{
	ADDRESS_MAX_LEN = 15
};

void config_init (const std::string& st_localeServiceName); // default "" is CONFIG

extern char sql_addr[256];

extern WORD mother_port;
extern WORD p2p_port;

extern char db_addr[ADDRESS_MAX_LEN + 1];
extern WORD db_port;

extern int passes_per_sec;
extern int save_event_second_cycle;
extern int ping_event_second_cycle;
extern int test_server;
extern bool	guild_mark_server;
extern BYTE guild_mark_min_level;
extern bool	distribution_test_server;

extern int g_iStatusPointGetLevelLimit;

extern bool	g_bNoMoreClient;
extern bool	g_bNoRegen;

// ========================================= //
extern bool	g_bGuildInviteLimit;
extern bool	g_bGuildInfiniteMembers;

extern bool	g_bTrafficProfileOn;
// ========================================= //

extern BYTE	g_bChannel;

extern bool	map_allow_find (int index);
extern void	map_allow_copy (int32_t* pl, int size);
extern bool	no_wander;

extern int	g_iUserLimit;
extern time_t	g_global_time;

const char* 	get_table_postfix();

extern std::string	g_stHostname;
extern std::string	g_stLocale;
extern std::string	g_stLocaleFilename;

extern char		g_szPublicIP[16];
extern char		g_szInternalIP[16];

#ifdef ENABLE_PROXY_IP
extern std::string g_stProxyIP;
#endif

extern int (*is_twobyte) (const char * str);
extern int (*check_name) (const char* str);

extern bool		g_bSkillDisable;

extern int		g_iFullUserCount;
extern int		g_iBusyUserCount;
extern void		LoadStateUserCount();

extern bool	g_bEmpireWhisper;

extern BYTE	g_bAuthServer;

#ifdef ENABLE_LEVEL_INT
extern int PK_PROTECT_LEVEL;
#else
extern BYTE	PK_PROTECT_LEVEL;
#endif

extern void	LoadValidCRCList();
extern bool	IsValidProcessCRC (DWORD dwCRC);
extern bool	IsValidFileCRC (DWORD dwCRC);

extern std::string	g_stAuthMasterIP;
extern WORD		g_wAuthMasterPort;

extern std::string	g_stClientVersion;

extern std::string	g_stQuestDir;
//extern std::string	g_stQuestObjectDir;
extern std::set<std::string> g_setQuestObjectDir;


extern std::vector<std::string>	g_stAdminPageIP;
extern std::string	g_stAdminPagePassword;

extern int	SPEEDHACK_LIMIT_COUNT;
extern int 	SPEEDHACK_LIMIT_BONUS;

extern int g_iSyncHackLimitCount;

extern int g_server_id;
extern std::string g_strWebMallURL;

extern int VIEW_RANGE;
extern int VIEW_BONUS_RANGE;

extern bool g_bCheckMultiHack;
extern bool g_protectNormalPlayer;
extern bool g_noticeBattleZone;

extern DWORD g_GoldDropTimeLimitValue;

extern int gPlayerMaxLevel;
#ifdef ENABLE_CONQUEROR_LEVEL
extern int gPlayerConquerorMaxLevel;
#endif
extern bool g_BlockCharCreation;

extern bool gHackCheckEnable;

// missing begin
extern void map_allow_log();
// missing end

#endif
