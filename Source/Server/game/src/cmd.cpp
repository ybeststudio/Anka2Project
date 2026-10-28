#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "locale_service.h"
#include "log.h"
#include "desc.h"

#include "../../common/service.h"

#ifdef ENABLE_AUTO_CAPE_SYSTEM
ACMD(do_auto_cape);
#endif
#ifdef ENABLE_CONFIRM_LEVELUP
ACMD(do_confirm_levelup);
#endif
ACMD(do_free_regen);
ACMD(do_user_horse_ride);
ACMD(do_user_horse_back);
ACMD(do_user_horse_feed);

ACMD(do_slow);
ACMD(do_stun);

ACMD(do_warp);
ACMD(do_goto);
ACMD(do_item);
ACMD(do_mob);
ACMD(do_mob_ld);
ACMD(do_mob_aggresive);
ACMD(do_mob_coward);
ACMD(do_mob_map);
ACMD(do_purge);
ACMD(do_weaken);
ACMD(do_item_purge);
ACMD(do_state);
ACMD(do_notice);
ACMD(do_map_notice);
ACMD(do_big_notice);
ACMD(do_who);
ACMD(do_user);
ACMD(do_disconnect);
ACMD(do_kill);
ACMD(do_emotion_allow);
ACMD(do_emotion);
ACMD(do_transfer);
ACMD(do_set);
ACMD(do_cmd);
ACMD(do_reset);
ACMD(do_greset);
ACMD(do_mount);
ACMD(do_fishing);
ACMD(do_refine_rod);
ACMD(do_max_pick);
ACMD(do_refine_pick);

ACMD(do_fishing_simul);
ACMD(do_console);
ACMD(do_restart);
ACMD(do_advance);
ACMD(do_stat);
#ifdef ENABLE_CONQUEROR_LEVEL
ACMD(do_clevel);
ACMD(do_conqueror_plus_amount);
ACMD(do_set_conqueror);
ACMD(do_state_sungma);
ACMD(do_conqueror_stat);
#endif
ACMD(do_respawn);
ACMD(do_skillup);
ACMD(do_guildskillup);
ACMD(do_pvp);
ACMD(do_point_reset);
ACMD(do_safebox_size);
ACMD(do_safebox_close);
ACMD(do_safebox_password);
ACMD(do_safebox_change_password);
ACMD(do_mall_password);
ACMD(do_mall_close);
ACMD(do_ungroup);
ACMD(do_makeguild);
ACMD(do_deleteguild);
ACMD(do_shutdown);
ACMD(do_group);
ACMD(do_group_random);
ACMD(do_invisibility);
ACMD(do_event_flag);
ACMD(do_get_event_flag);
ACMD(do_private);
ACMD(do_qf);
ACMD(do_clear_quest);
ACMD(do_book);
ACMD(do_reload);
ACMD(do_war);
ACMD(do_nowar);
ACMD(do_setskill);
ACMD(do_setskillother);
ACMD(do_level);
#ifdef ENABLE_CONQUEROR_LEVEL
ACMD(do_conqueror_stat);
#endif
ACMD(do_polymorph);
ACMD(do_polymorph_item);
ACMD(do_close_shop);
ACMD(do_set_walk_mode);
ACMD(do_set_run_mode);
ACMD(do_set_skill_group);
ACMD(do_set_skill_point);
ACMD(do_cooltime);
ACMD(do_detaillog);
ACMD(do_monsterlog);

ACMD(do_gwlist);
ACMD(do_stop_guild_war);
ACMD(do_cancel_guild_war);
ACMD(do_guild_state);

ACMD(do_pkmode);
ACMD(do_messenger_auth);

ACMD(do_getqf);
ACMD(do_setqf);
ACMD(do_delqf);
ACMD(do_set_state);

ACMD(do_forgetme);
ACMD(do_aggregate);
ACMD(do_attract_ranger);
ACMD(do_pull_monster);
ACMD(do_setblockmode);
ACMD(do_priv_empire);
ACMD(do_priv_guild);
ACMD(do_mount_test);
ACMD(do_unmount);
#ifdef ENABLE_BUFFI_SYSTEM
ACMD(do_buffi_summon);
ACMD(do_buffi_unsummon);
ACMD(do_buffi_info);
ACMD(do_buffi_set);
ACMD(do_buffi_option);
ACMD(do_support_skill_power);
#ifdef ENABLE_SKILL_UPGRADE
ACMD(do_upgrade_skill);
ACMD(do_upgrade_skill_info);
#endif
ACMD(do_support_add_ore);
ACMD(do_support_remove_ore);
ACMD(do_buffi_skill_color);
#ifdef ENABLE_BUFFI_EVOLUTION
ACMD(do_support_set_evolution);
#endif
ACMD(do_support_full_duration);
#endif
ACMD(do_observer);
ACMD(do_observer_exit);
ACMD(do_socket_item);
ACMD(do_xmas);
ACMD(do_stat_minus);
ACMD(do_stat_reset);
ACMD(do_view_equip);
ACMD(do_block_chat);
ACMD(do_vote_block_chat);
ACMD(do_block_chat_list);

ACMD(do_party_request);
ACMD(do_party_request_deny);
ACMD(do_party_request_accept);
ACMD(do_build);
ACMD(do_clear_land);

ACMD(do_horse_state);
ACMD(do_horse_level);
ACMD(do_horse_ride);
ACMD(do_horse_summon);
ACMD(do_horse_unsummon);
ACMD(do_horse_set_stat);

ACMD(do_save_attribute_to_image);
ACMD(do_affect_remove);

ACMD(do_change_attr);
ACMD(do_add_attr);
ACMD(do_add_socket);

ACMD(do_inputall)
{
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please enter the Order in full length."));
}

ACMD(do_show_arena_list);
ACMD(do_end_all_duel);
ACMD(do_end_duel);
ACMD(do_duel);

ACMD(do_stat_plus_amount);

ACMD(do_break_marriage);

ACMD(do_oxevent_show_quiz);
ACMD(do_oxevent_log);
ACMD(do_oxevent_get_attender);

ACMD(do_effect);
ACMD(do_hair);
ACMD(do_gift);
ACMD(do_inventory);
ACMD(do_cube);

ACMD(do_reset_subskill );
ACMD(do_flush);

ACMD(do_eclipse);
ACMD(do_weeklyevent);

ACMD(do_event_helper);

ACMD(do_in_game_mall);

ACMD(do_get_mob_count);

ACMD(do_dice);
ACMD(do_special_item);

ACMD(do_click_mall);
ACMD(do_click_safebox);

ACMD(do_ride);
ACMD(do_get_item_id_list);
ACMD(do_set_socket);

ACMD(do_set_stat);
ACMD (do_can_dead);

ACMD (do_full_set);
ACMD (do_item_full_set);
ACMD (do_attr_full_set);
ACMD (do_all_skill_master);
ACMD (do_use_item);
ACMD (do_dragon_soul);
ACMD (do_ds_list);
ACMD (do_clear_affect);

#ifdef ENABLE_CHANGE_CHANNEL
	ACMD(do_change_channel);
#endif

#ifdef ENABLE_SORT_INVENTORY
	ACMD(do_sort_items);
	ACMD(do_sort_inventory);
	ACMD(do_sort_special_storage);
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	ACMD(do_hide_costume);
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	ACMD(do_new_safebox_open);
	ACMD(do_new_mall_open);
#endif

#ifdef ENABLE_RESTART_INSTANT
	ACMD(do_restart_now);
#endif

#ifdef ENABLE_ANTI_EXP
	ACMD(do_anti_exp);
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
	ACMD(do_multi_farm);
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	ACMD(do_set_shop);
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	ACMD(do_setpickupmode);
#endif

#ifdef ENABLE_MAINTENANCE_SYSTEM
	ACMD(do_maintenance);
	ACMD(do_maintenance_text);
#endif

#ifdef ENABLE_RENEWAL_SKILL_SELECT
	ACMD(do_renewal_skill_select);
#endif

#ifdef ENABLE_ITEMSHOP
	ACMD(do_ishop);
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	ACMD (do_pet_set);
#endif

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	ACMD(do_unlock_inventory);
	ACMD(do_reload_inventory);
#endif

#ifdef ENABLE_RENEWAL_TELEPORT_SYSTEM
	ACMD(do_warp_on);
	ACMD(do_open_warping_window);
#endif

#ifdef ENABLE_TRACK_WINDOW
ACMD(do_track_window);
#endif

#ifdef ENABLE_ZODIAC_MISSION
ACMD(do_cz_reward);
ACMD(do_cz_check_box);
#endif

#ifdef ENABLE_BOT_PLAYER
ACMD(do_BotCharacter);
ACMD(do_BotSpawnA);
ACMD(do_BotSpawnB);
ACMD(do_BotSpawnC);
ACMD(do_BotCharacterDelete);
ACMD(do_BotCharacterDeleteName);
ACMD(do_BotCharacterTotal);
ACMD(do_BotReload);
#endif

#ifdef ENABLE_SPIRIT_STONE_READING
ACMD(do_ruhoku);
#endif
#ifdef ENABLE_SKILL_BOOK_READING
ACMD(do_bkoku);
#endif

#ifdef __AUTO_HUNT__
ACMD(do_auto_hunt);
#endif

#ifdef ENABLE_AUTO_SELL_SYSTEM
ACMD(do_autosell_status);
ACMD(do_autosell_add);
ACMD(do_autosell_remove);
ACMD(do_autosell_remove_all);
ACMD(do_autosell_info);
#endif

#ifdef ENABLE_PLAYERS_ONLINE
ACMD(do_players_online);
#endif

#ifdef ENABLE_NPC_LOCATION_TRACE
ACMD(do_find_npc);
ACMD(do_find_near_npc);
#endif

#ifdef ENABLE_BIOLOGIST_SYSTEM
ACMD (do_biyolog);
#endif

#ifdef ENABLE_DUNGEON_INFO
ACMD(do_reset_time_dungeon);
#endif

#ifdef __DUNGEON_INFO__
ACMD(do_dungeon_info);
#endif

#ifdef __LEADERSHIP__BONUS__
ACMD(do_leadership_bonus);
#endif

#ifdef ENABLE_AFFECT_BUFF_REMOVE
ACMD(do_remove_buff);
#endif

#ifdef ENABLE_PASSIVE_SYSTEM
ACMD(do_passive_relic);
#endif

#ifdef ENABLE_TITLE_SYSTEM
ACMD(do_title);
#endif

#ifdef ENABLE_REMOTE_SHOP_SYSTEM
ACMD(do_open_range_npc);
#endif

#ifdef __SYSTEM_SEARCH_ITEM_MOB__
ACMD(search_drop);
#endif

#ifdef ENABLE_COLLECT_WINDOW
ACMD(do_choose_quest);
ACMD(do_open_collect_window);
#endif

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
	ACMD(do_add_collect_item);
#endif

#ifdef ENABLE_EVENT_BANNER_FLAG
ACMD(do_banner);
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
ACMD(do_attendance);
ACMD(do_easter_event);
#endif

#ifdef ENABLE_MINI_GAME_CATCH_KING
ACMD(do_catch_king_event);
#endif

#ifdef ENABLE_MINIGAME_RUMI_EVENT
ACMD(do_cards);
#endif

#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
ACMD(do_increase_halloween);
ACMD(take_reward_halloween);
#endif

#ifdef ENABLE_SOCCER_BALL_EVENT
ACMD(do_top_ver);
#endif

#ifdef ENABLE_WORD_GAME_EVENT
ACMD(do_word_game);
#endif

#ifdef ENABLE_GAYA_SHOP_SYSTEM
ACMD(do_gem);
#endif

#ifdef ENABLE_GAYA_TICKET_SYSTEM
ACMD(do_use_gem_ticket);
#endif

#ifdef ENABLE_BATTLE_PASS
ACMD(final_reward_battlepass);
ACMD(open_battlepass);
ACMD(battlepass_bitirici);
ACMD(final_reward_battlepass_premium);
ACMD(open_battlepass_premium);
ACMD(battlepass_bitirici_premium);
#endif

#ifdef ENABLE_RANKING
ACMD(do_ranking_subcategory);
#endif

struct command_info cmd_info[] =
{
	{ "!RESERVED!",						NULL,							0,						POS_DEAD,			GM_IMPLEMENTOR	},
#ifdef ENABLE_AUTO_CAPE_SYSTEM
	{ "auto_cape", do_auto_cape, 0, POS_DEAD, GM_PLAYER },
#endif
#ifdef ENABLE_CONFIRM_LEVELUP
	{ "confirm_levelup", do_confirm_levelup, 0, POS_DEAD, GM_PLAYER },
#endif
	{ "free_regens",					do_free_regen,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "who",							do_who,							0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "war",							do_war,							0,						POS_DEAD,			GM_PLAYER		},
	{ "warp",							do_warp,						0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "user",							do_user,						0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "notice",							do_notice,						0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "notice_map",						do_map_notice,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "big_notice",						do_big_notice,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "nowar",							do_nowar,						0,						POS_DEAD,			GM_PLAYER		},
	{ "purge",							do_purge,						0,						POS_DEAD,			GM_WIZARD		},
	{ "weaken",							do_weaken,						0,						POS_DEAD,			GM_GOD			},
	{ "dc",								do_disconnect,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "transfer",						do_transfer,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "goto",							do_goto,						0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "level",							do_level,						0,						POS_DEAD,			GM_LOW_WIZARD	},
#ifdef ENABLE_CONQUEROR_LEVEL
	{ "conqueror_stat", 				do_conqueror_stat, 				0, 						POS_DEAD, 			GM_PLAYER 		},
#endif
	{ "eventflag",						do_event_flag,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "geteventflag",					do_get_event_flag,				0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "item",							do_item,						0,						POS_DEAD,			GM_GOD			},

	{ "mob",							do_mob,							0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "mob_ld",							do_mob_ld,						0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "ma",								do_mob_aggresive,				0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "mc",								do_mob_coward,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "mm",								do_mob_map,						0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "kill",							do_kill,						0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "ipurge",							do_item_purge,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "group",							do_group,						0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "grrandom",						do_group_random,				0,						POS_DEAD,			GM_HIGH_WIZARD	},

	{ "set",							do_set,							0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "reset",							do_reset,						0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "greset",							do_greset,						0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "advance",						do_advance,						0,						POS_DEAD,			GM_GOD			},
	{ "book",							do_book,						0,						POS_DEAD,			GM_IMPLEMENTOR	},

	{ "console",						do_console,						0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "shutdow",						do_inputall,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "shutdown",						do_shutdown,					0,						POS_DEAD,			GM_HIGH_WIZARD	},

	{ "stat",							do_stat,						0,						POS_DEAD,			GM_PLAYER		},
	{ "stat-",							do_stat_minus,					0,						POS_DEAD,			GM_PLAYER		},
	{ "stat_reset",						do_stat_reset,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "state",							do_state,						0,						POS_DEAD,			GM_LOW_WIZARD	},

#ifdef ENABLE_CONQUEROR_LEVEL
	{ "clevel",							do_clevel,						0,						POS_DEAD,			GM_LOW_WIZARD 	},

	{ "ccon+",							do_conqueror_plus_amount,		POINT_SUNGMA_HP,		POS_DEAD,			GM_LOW_WIZARD	},
	{ "cimu+",							do_conqueror_plus_amount,		POINT_SUNGMA_IMMUNE,	POS_DEAD,			GM_LOW_WIZARD	},
	{ "cstr+",							do_conqueror_plus_amount,		POINT_SUNGMA_STR,		POS_DEAD,			GM_LOW_WIZARD	},
	{ "cmov+",							do_conqueror_plus_amount,		POINT_SUNGMA_MOVE,		POS_DEAD,			GM_LOW_WIZARD	},

	{ "tccon",							do_set_conqueror,				POINT_SUNGMA_HP,		POS_DEAD,			GM_LOW_WIZARD	},
	{ "timu",							do_set_conqueror,				POINT_SUNGMA_IMMUNE,	POS_DEAD,			GM_LOW_WIZARD	},
	{ "tcstr",							do_set_conqueror,				POINT_SUNGMA_STR,		POS_DEAD,			GM_LOW_WIZARD	},
	{ "tcmov",							do_set_conqueror,				POINT_SUNGMA_MOVE,		POS_DEAD,			GM_LOW_WIZARD	},

	{ "sungma_state",					do_state_sungma,				 0, 					POS_DEAD, 			GM_LOW_WIZARD 	},
	{ "conqueror_stat",					do_conqueror_stat,				0,						POS_DEAD,			GM_PLAYER	},
#endif

	{ "stun",							do_stun,						0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "slow",							do_slow,						0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "respawn",						do_respawn,						0,						POS_DEAD,			GM_WIZARD		},

	{ "makeguild",						do_makeguild,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "deleteguild",					do_deleteguild,					0,						POS_DEAD,			GM_HIGH_WIZARD	},

	{ "mount",							do_mount,						0,						POS_MOUNTING,		GM_PLAYER		},
	{ "restart_here",					do_restart,						SCMD_RESTART_HERE,		POS_DEAD,			GM_PLAYER		},
	{ "restart_town",					do_restart,						SCMD_RESTART_TOWN,		POS_DEAD,			GM_PLAYER		},
	{ "phase_selec",					do_inputall,					0,						POS_DEAD,			GM_PLAYER		},
	{ "phase_select",					do_cmd,							SCMD_PHASE_SELECT,		POS_DEAD,			GM_PLAYER		},

	{ "qui",							do_inputall,					0,						POS_DEAD,			GM_PLAYER		},
	{ "quit",							do_cmd,							SCMD_QUIT,				POS_DEAD,			GM_PLAYER		},
	{ "logou",							do_inputall,					0,						POS_DEAD,			GM_PLAYER		},
	{ "logout",							do_cmd,							SCMD_LOGOUT,			POS_DEAD,			GM_PLAYER		},
	{ "skillup",						do_skillup,						0,						POS_DEAD,			GM_PLAYER		},
	{ "gskillup",						do_guildskillup,				0,						POS_DEAD,			GM_PLAYER		},
	{ "pvp",							do_pvp,							0,						POS_DEAD,			GM_PLAYER		},

	{ "mall_close",						do_mall_close,					0,						POS_DEAD,			GM_PLAYER		},
	{ "safebox_close",					do_safebox_close,				0,						POS_DEAD,			GM_PLAYER		},

#ifdef ENABLE_SPECIAL_INVENTORY
	{ "new_safebox_open",				do_new_safebox_open,			0,						POS_DEAD,			GM_PLAYER		},
	{ "new_mall_open",					do_new_mall_open,				0,						POS_DEAD,			GM_PLAYER		},
	{ "safebox",						do_safebox_size,				0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "safebox_passwor",				do_inputall,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "safebox_password",				do_safebox_password,			0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "safebox_change_passwor",			do_inputall,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "safebox_change_password",		do_safebox_change_password,		0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "mall_passwor",					do_inputall,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "mall_password",					do_mall_password,				0,						POS_DEAD,			GM_IMPLEMENTOR	},
#else
	{ "safebox",						do_safebox_size,				0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "safebox_passwor",				do_inputall,					0,						POS_DEAD,			GM_PLAYER		},
	{ "safebox_password",				do_safebox_password,			0,						POS_DEAD,			GM_PLAYER		},
	{ "safebox_change_passwor",			do_inputall,					0,						POS_DEAD,			GM_PLAYER		},
	{ "safebox_change_password",		do_safebox_change_password,		0,						POS_DEAD,			GM_PLAYER		},
	{ "mall_passwor",					do_inputall,					0,						POS_DEAD,			GM_PLAYER		},
	{ "mall_password",					do_mall_password,				0,						POS_DEAD,			GM_PLAYER		},
#endif

	{ "ungroup",						do_ungroup,						0,						POS_DEAD,			GM_PLAYER		},

	{ "refine_rod",						do_refine_rod,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "refine_pick",					do_refine_pick,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "max_pick",						do_max_pick,					0,						POS_DEAD,			GM_IMPLEMENTOR	},

	{ "fish_simul",						do_fishing_simul,				0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "invisible",						do_invisibility,				0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "qf",								do_qf,							0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "clear_quest",					do_clear_quest,					0,						POS_DEAD,			GM_HIGH_WIZARD	},

	{ "close_shop",						do_close_shop,					0,						POS_DEAD,			GM_PLAYER		},

	{ "set_walk_mode",					do_set_walk_mode,				0,						POS_DEAD,			GM_PLAYER		},
	{ "set_run_mode",					do_set_run_mode,				0,						POS_DEAD,			GM_PLAYER		},
	{ "setjob",							do_set_skill_group,				0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "setskill",						do_setskill,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "setskillother",					do_setskillother,				0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "setskillpoint",					do_set_skill_point,				0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "reload",							do_reload,						0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "cooltime",						do_cooltime,					0,						POS_DEAD,			GM_HIGH_WIZARD	},

	{ "gwlist",							do_gwlist,						0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "gwstop",							do_stop_guild_war,				0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "gwcancel",						do_cancel_guild_war,			0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "gstate",							do_guild_state,					0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "pkmode",							do_pkmode,						0,						POS_DEAD,			GM_PLAYER		},
	{ "messenger_auth",					do_messenger_auth,				0,						POS_DEAD,			GM_PLAYER		},

	{ "getqf",							do_getqf,						0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "setqf",							do_setqf,						0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "delqf",							do_delqf,						0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "set_state",						do_set_state,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "detaillog",						do_detaillog,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "monsterlog",						do_monsterlog,					0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "forgetme",						do_forgetme,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "aggregate",						do_aggregate,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "attract_ranger",					do_attract_ranger,				0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "pull_monster",					do_pull_monster,				0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "setblockmode",					do_setblockmode,				0,						POS_DEAD,			GM_PLAYER		},
	{ "polymorph",						do_polymorph,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "polyitem",						do_polymorph_item,				0,						POS_DEAD,			GM_HIGH_WIZARD },
	{ "priv_empire",					do_priv_empire,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "priv_guild",						do_priv_guild,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "mount_test",						do_mount_test,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "unmount",						do_unmount,						0,						POS_DEAD,			GM_PLAYER		},
#ifdef ENABLE_BUFFI_SYSTEM
	{ "buffi_summon",					do_buffi_summon,				0,						POS_DEAD,			GM_PLAYER		},
	{ "buffi_unsummon",					do_buffi_unsummon,				0,						POS_DEAD,			GM_PLAYER		},
	{ "buffi_info",						do_buffi_info,					0,						POS_DEAD,			GM_PLAYER		},
	{ "buffi_option",					do_buffi_option,				0,						POS_DEAD,			GM_PLAYER		},
	{ "buffi_set",						do_buffi_set,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "support_skill_power",			do_support_skill_power,			0,						POS_DEAD,			GM_PLAYER		},
#ifdef ENABLE_SKILL_UPGRADE
	{ "upgrade_skill",				do_upgrade_skill,				0,						POS_DEAD,			GM_PLAYER		},
	{ "upgrade_skill_info",			do_upgrade_skill_info,			0,						POS_DEAD,			GM_PLAYER		},
#endif
	{ "support_add_ore",				do_support_add_ore,				0,						POS_DEAD,			GM_PLAYER		},
	{ "support_remove_ore",			do_support_remove_ore,			0,						POS_DEAD,			GM_PLAYER		},
	{ "buffi_skill_color",			do_buffi_skill_color,			0,						POS_DEAD,			GM_PLAYER		},
#ifdef ENABLE_BUFFI_EVOLUTION
	{ "support_set_evolution",		do_support_set_evolution,		0,						POS_DEAD,			GM_PLAYER		},
#endif
	{ "support_full_duration",			do_support_full_duration,		0,						POS_DEAD,			GM_PLAYER		},
#endif
	{ "private",						do_private,						0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "party_request",					do_party_request,				0,						POS_DEAD,			GM_PLAYER		},
	{ "party_request_accept",			do_party_request_accept,		0,						POS_DEAD,			GM_PLAYER		},
	{ "party_request_deny",				do_party_request_deny,			0,						POS_DEAD,			GM_PLAYER		},
	{ "observer",						do_observer,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "observer_exit",					do_observer_exit,				0,						POS_DEAD,			GM_PLAYER		},
	{ "socketitem",						do_socket_item,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "saveati",						do_save_attribute_to_image,		0,						POS_DEAD,			GM_IMPLEMENTOR	},

	{ "xmas_boom",						do_xmas,						SCMD_XMAS_BOOM,			POS_DEAD,			GM_HIGH_WIZARD	},
	{ "xmas_snow",						do_xmas,						SCMD_XMAS_SNOW,			POS_DEAD,			GM_HIGH_WIZARD	},
	{ "xmas_santa",						do_xmas,						SCMD_XMAS_SANTA,		POS_DEAD,			GM_HIGH_WIZARD	},
	{ "view_equip",						do_view_equip,					0,						POS_DEAD,			GM_PLAYER		},
	{ "jy",								do_block_chat,					0,						POS_DEAD,			GM_HIGH_WIZARD	},

	{ "vote_block_chat",				do_vote_block_chat,				0,						POS_DEAD,			GM_PLAYER		},
	{ "block_chat",						do_block_chat,					0,						POS_DEAD,			GM_PLAYER		},
	{ "block_chat_list",				do_block_chat_list,				0,						POS_DEAD,			GM_PLAYER		},

	{ "build",							do_build,						0,						POS_DEAD,			GM_PLAYER		},
	{ "clear_land",						do_clear_land,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "affect_remove",					do_affect_remove,				0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "horse_state",					do_horse_state,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "horse_level",					do_horse_level,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "horse_ride",						do_horse_ride,					0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "horse_summon",					do_horse_summon,				0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "horse_unsummon",					do_horse_unsummon,				0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "horse_set_stat",					do_horse_set_stat,				0,						POS_DEAD,			GM_HIGH_WIZARD	},

	{ "emotion_allow",					do_emotion_allow,				0,						POS_FIGHTING,		GM_PLAYER		},
	{ "kiss",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "slap",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "french_kiss",					do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "clap",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "cheer1",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "cheer2",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},

	{ "dance1",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "dance2",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "dance3",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "dance4",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "dance5",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "dance6",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "dance7",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "dance8",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "dance9",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},

	{ "congratulation",					do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "forgive",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "angry",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "attractive",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "sad",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "shy",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "cheerup",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "banter",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "joy",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "selfie",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "doze",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "exercise",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "pushup",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "worship",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "backflip",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "breakdance",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "dab",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "facepalm",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "fuckoff",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "wank",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "shake_it",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "throw_money",					do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "twerk",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "samba",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "shimmy",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "slow2",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "bestmates",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "flair",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "horon",							do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "robotdance",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "snowball",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},
	{ "surprised2",						do_emotion,						0,						POS_FIGHTING,		GM_PLAYER		},

	{ "change_attr",					do_change_attr,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "add_attr",						do_add_attr,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "add_socket",						do_add_socket,					0,						POS_DEAD,			GM_IMPLEMENTOR	},

	{ "user_horse_ride",				do_user_horse_ride,				0,						POS_FISHING,		GM_PLAYER		},
	{ "user_horse_back",				do_user_horse_back,				0,						POS_FISHING,		GM_PLAYER		},
	{ "user_horse_feed",				do_user_horse_feed,				0,						POS_FISHING,		GM_PLAYER		},

	{ "show_arena_list",				do_show_arena_list,				0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "end_all_duel",					do_end_all_duel,				0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "end_duel",						do_end_duel,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "duel",							do_duel,						0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "con+",							do_stat_plus_amount,			POINT_HT,				POS_DEAD,			GM_LOW_WIZARD	},
	{ "int+",							do_stat_plus_amount,			POINT_IQ,				POS_DEAD,			GM_LOW_WIZARD	},
	{ "str+",							do_stat_plus_amount,			POINT_ST,				POS_DEAD,			GM_LOW_WIZARD	},
	{ "dex+",							do_stat_plus_amount,			POINT_DX,				POS_DEAD,			GM_LOW_WIZARD	},

	{ "break_marriage",					do_break_marriage,				0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "show_quiz",						do_oxevent_show_quiz,			0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "log_oxevent",					do_oxevent_log,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "get_oxevent_att",				do_oxevent_get_attender,		0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "effect",							do_effect,						0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "hair",							do_hair,						0,						POS_DEAD,			GM_PLAYER		},
	{ "inventory",						do_inventory,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "cube",							do_cube,						0,						POS_DEAD,			GM_PLAYER		},
	{ "reset_subskill",					do_reset_subskill,				0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "flush",							do_flush,						0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "gift",							do_gift,						0,						POS_DEAD,			GM_PLAYER		},
	
	{ "eclipse",						do_eclipse,						0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "weeklyevent",					do_weeklyevent,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "eventhelper",					do_event_helper,				0,						POS_DEAD,			GM_HIGH_WIZARD	},
	{ "in_game_mall",					do_in_game_mall,				0,						POS_DEAD,			GM_PLAYER		},
	{ "get_mob_count",					do_get_mob_count,				0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "dice",							do_dice,						0,						POS_DEAD,			GM_PLAYER		},
	{ "special_item",					do_special_item,				0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "click_mall",						do_click_mall,					0,						POS_DEAD,			GM_PLAYER		},
	{ "click_safebox",					do_click_safebox,				0,						POS_DEAD,			GM_PLAYER		},
	{ "ride",							do_ride,						0,						POS_DEAD,			GM_PLAYER		},

	{ "item_id_list",					do_get_item_id_list,			0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "set_socket",						do_set_socket,					0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "tcon",							do_set_stat,					POINT_HT,				POS_DEAD,			GM_LOW_WIZARD	},
	{ "tint",							do_set_stat,					POINT_IQ,				POS_DEAD,			GM_LOW_WIZARD	},
	{ "tstr",							do_set_stat,					POINT_ST,				POS_DEAD,			GM_LOW_WIZARD	},
	{ "tdex",							do_set_stat,					POINT_DX,				POS_DEAD,			GM_LOW_WIZARD	},

	{ "cannot_dead",					do_can_dead,					1,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "can_dead",						do_can_dead,					0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "full_set",						do_full_set,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "item_full_set",					do_item_full_set,				0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "attr_full_set",					do_attr_full_set,				0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "all_skill_master",				do_all_skill_master,			0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "use_item",						do_use_item,					0,						POS_DEAD,			GM_LOW_WIZARD	},
	{ "do_clear_affect",				do_clear_affect,				0,						POS_DEAD,			GM_LOW_WIZARD	},

	{ "dragon_soul",					do_dragon_soul,					0,						POS_DEAD,			GM_PLAYER		},
	{ "ds_list",						do_ds_list,						0,						POS_DEAD,			GM_PLAYER		},

#ifdef ENABLE_CHANGE_CHANNEL
	{ "change_channel",					do_change_channel,				0,						POS_DEAD,			GM_PLAYER		},
#endif

#ifdef ENABLE_SORT_INVENTORY
	{ "click_sort_items",				do_sort_items,					0,						POS_DEAD,			GM_PLAYER		},
	{ "sort_inventory",					do_sort_inventory,				0,						POS_DEAD,			GM_PLAYER		},
	{ "click_sort_special_storage",		do_sort_special_storage,		0,						POS_DEAD,			GM_PLAYER		},
#endif

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
	{ "hide_costume",					do_hide_costume,				0,						POS_DEAD,			GM_PLAYER		},
#endif

#ifdef ENABLE_RESTART_INSTANT
	{ "restart_now",					do_restart_now,					0,						POS_DEAD,			GM_PLAYER,		},
#endif

#ifdef ENABLE_ANTI_EXP
	{ "anti_exp",						do_anti_exp,					0,						POS_DEAD,			GM_PLAYER		},
#endif

#ifdef ENABLE_MULTI_FARM_BLOCK
	{ "multi_farm",						do_multi_farm,					0,						POS_DEAD,			GM_PLAYER		},
#endif

#ifdef ENABLE_RENEWAL_OFFLINESHOP
	{ "set_shop",						do_set_shop,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
	{ "setpickupmode",					do_setpickupmode,				0,						POS_DEAD,			GM_PLAYER		},
#endif

#ifdef ENABLE_MAINTENANCE_SYSTEM
	{ "maintenance",					do_maintenance,					0,						POS_DEAD,			GM_IMPLEMENTOR	},
	{ "m_text",							do_maintenance_text,			0,						POS_DEAD,			GM_IMPLEMENTOR	},
#endif

#ifdef ENABLE_RENEWAL_SKILL_SELECT
	{ "renewal_skill_select",			do_renewal_skill_select,		0,						POS_DEAD,			GM_PLAYER		},
#endif

#ifdef ENABLE_ITEMSHOP
	{ "ishop",							do_ishop,						0,						POS_DEAD,			GM_PLAYER		},
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	{ "pet_set",						do_pet_set,						0,						POS_DEAD,			GM_IMPLEMENTOR	},
#endif

#ifdef ENABLE_INVENTORY_EXPANSION_SYSTEM
	{ "unlock_inventory",				do_unlock_inventory,			0,						POS_DEAD,			GM_PLAYER		},
	{ "reload_inventory",				do_reload_inventory,			0,						POS_DEAD,			GM_PLAYER		},
#endif

#ifdef ENABLE_RENEWAL_TELEPORT_SYSTEM
	{ "warp_on",						do_warp_on,						0,						POS_DEAD,			GM_PLAYER		},
	{ "open_warping_window",			do_open_warping_window,			0,						POS_DEAD,			GM_PLAYER		},
#endif

#ifdef ENABLE_TRACK_WINDOW
	{ "track_window",					do_track_window,				0,						POS_DEAD,			GM_PLAYER		},
#endif

#ifdef ENABLE_ZODIAC_MISSION
	{ "cz_reward", 						do_cz_reward, 					0, 						POS_DEAD, 			GM_PLAYER		},
	{ "cz_check_box", 					do_cz_check_box, 				0, 						POS_DEAD, 			GM_PLAYER		},
#endif

#ifdef ENABLE_BOT_PLAYER
	{ "bot_spawn",					do_BotCharacter, 					0, 						POS_DEAD, 			GM_IMPLEMENTOR 		},
	{ "bot_spawn_a",				do_BotSpawnA, 						0, 						POS_DEAD, 			GM_IMPLEMENTOR 		},
	{ "bot_spawn_b",				do_BotSpawnB, 						0, 						POS_DEAD, 			GM_IMPLEMENTOR 		},
	{ "bot_spawn_c",				do_BotSpawnC, 						0, 						POS_DEAD, 			GM_IMPLEMENTOR 		},
	{ "bot_delete",					do_BotCharacterDelete, 				0, 						POS_DEAD, 			GM_IMPLEMENTOR 		},
	{ "bot_delete_name",			do_BotCharacterDeleteName,			0, 						POS_DEAD, 			GM_IMPLEMENTOR 		},
	{ "bot_total",					do_BotCharacterTotal, 				0, 						POS_DEAD, 			GM_IMPLEMENTOR 		},
	{ "bot_reload",					do_BotReload, 						0, 						POS_DEAD, 			GM_IMPLEMENTOR 		},
#endif

#ifdef ENABLE_SPIRIT_STONE_READING
	{ "ruhoku",						do_ruhoku,							0,						POS_DEAD,			GM_PLAYER			},
#endif
#ifdef ENABLE_SKILL_BOOK_READING
	{ "bkoku",						do_bkoku,							0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef __AUTO_HUNT__
	{ "auto_hunt",					do_auto_hunt,						0,						POS_DEAD,			GM_PLAYER 			},
#endif

#ifdef ENABLE_AUTO_SELL_SYSTEM
	{ "autosell_status",			do_autosell_status,					0,						POS_DEAD,			GM_PLAYER			},
	{ "autosell_add",				do_autosell_add,					0,						POS_DEAD,			GM_PLAYER			},
	{ "autosell_remove",			do_autosell_remove,					0,						POS_DEAD,			GM_PLAYER			},
	{ "autosell_remove_all",		do_autosell_remove_all,				0,						POS_DEAD,			GM_PLAYER 			},
	{ "autosell_info",				do_autosell_info,					0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef ENABLE_PLAYERS_ONLINE
	{ "players_online",				do_players_online,					0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef ENABLE_NPC_LOCATION_TRACE
	{ "find_npc",					do_find_npc,						0,						POS_DEAD,			GM_IMPLEMENTOR		},
	{ "find_near_npc",				do_find_near_npc,					0,						POS_DEAD,			GM_IMPLEMENTOR		},
#endif

#ifdef ENABLE_BIOLOGIST_SYSTEM
	{ "biyolo",						do_inputall,						0,						POS_DEAD,			GM_PLAYER			},
	{ "biyolog",					do_biyolog,							0,						POS_DEAD,			GM_PLAYER			},
#endif
#ifdef ENABLE_DUNGEON_INFO
	{"reset_time_dr", 				do_reset_time_dungeon, 				0, 						POS_DEAD, 			GM_IMPLEMENTOR 		},
#endif

#ifdef __DUNGEON_INFO__
	{ "dungeon_info",				do_dungeon_info,					0,						POS_DEAD,			GM_PLAYER 			},
#endif

#ifdef __LEADERSHIP__BONUS__
	{ "leadership_bonus",			do_leadership_bonus,				0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef ENABLE_AFFECT_BUFF_REMOVE
	{	"remove_buff",				do_remove_buff,						0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef ENABLE_PASSIVE_SYSTEM
	{ "passive_relic",				do_passive_relic,					0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef ENABLE_TITLE_SYSTEM
	{ "title",						do_title,							0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef ENABLE_REMOTE_SHOP_SYSTEM
	{"open_range_npc", 				do_open_range_npc, 					0, 						POS_DEAD, 			GM_PLAYER			},
#endif

#ifdef __SYSTEM_SEARCH_ITEM_MOB__
	{ "cauta_drop", 				search_drop,						0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef ENABLE_COLLECT_WINDOW
	{ "choose_quest",				do_choose_quest,					0,						POS_DEAD,			GM_PLAYER 			},
	{ "collect_window",				do_open_collect_window,				0,						POS_DEAD,			GM_PLAYER 			},
#endif

#ifdef __ENABLE_COLLECTIONS_SYSTEM__
	{ "do_add_collect_item", 		do_add_collect_item, 				0, 						POS_DEAD, 			GM_PLAYER 			},
#endif

#ifdef ENABLE_EVENT_BANNER_FLAG
	{ "banner", 					do_banner, 							0, 						POS_DEAD, 			GM_IMPLEMENTOR 		},
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
	{ "xmas_soul",					do_xmas,							SCMD_XMAS_SOUL,			POS_DEAD,			GM_HIGH_WIZARD		},
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
	{ "attendance",					do_attendance,						0,						POS_DEAD,			GM_IMPLEMENTOR		},
	{ "easter_event",				do_easter_event,					0,						POS_DEAD,			GM_IMPLEMENTOR		},
#endif

#ifdef ENABLE_MINI_GAME_CATCH_KING
	{ "catch_king_event",			do_catch_king_event,				0,						POS_DEAD,			GM_IMPLEMENTOR		},
#endif

#ifdef ENABLE_MINIGAME_RUMI_EVENT
	{ "cards",						do_cards,							0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef ENABLE_HALLOWEEN_EVENT_SYSTEM
	{ "increase_haloun", 			do_increase_halloween, 				0, 						POS_DEAD, 			GM_PLAYER 			},
	{ "take_reward_haloun", 		take_reward_halloween, 				0, 						POS_DEAD, 			GM_PLAYER 			},
#endif

#ifdef ENABLE_SOCCER_BALL_EVENT
	{ "topverirmisin",				do_top_ver,							0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef ENABLE_WORD_GAME_EVENT
	{ "wordgamestart",				do_word_game,						0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef ENABLE_GAYA_SHOP_SYSTEM
	{ "gem",						do_gem,								0,						POS_DEAD,			GM_PLAYER			},
#endif

#ifdef ENABLE_GAYA_TICKET_SYSTEM
	{ "use_gem_ticket", 			do_use_gem_ticket, 					0, 						POS_DEAD, 			GM_PLAYER			},
#endif

#ifdef ENABLE_BATTLE_PASS
	{ "open_battlepass",			open_battlepass, 0, POS_DEAD, GM_PLAYER },
	{ "final_reward_battlepass", final_reward_battlepass, 0, POS_DEAD, GM_PLAYER },
	{ "battlepass_bitirici", battlepass_bitirici, 0, POS_DEAD, GM_PLAYER },
	{ "open_battlepass_premium",			open_battlepass_premium, 0, POS_DEAD, GM_PLAYER },
	{ "final_reward_battlepass_premium", final_reward_battlepass_premium, 0, POS_DEAD, GM_PLAYER },
	{ "battlepass_bitirici_premium", battlepass_bitirici_premium, 0, POS_DEAD, GM_PLAYER },
#endif

#ifdef ENABLE_RANKING
	{"ranking_subcategory", do_ranking_subcategory, 0, POS_DEAD, GM_PLAYER },
#endif

	{ "\n",								NULL,							0,						POS_DEAD,			GM_IMPLEMENTOR		}
};

void interpreter_set_privilege(const char *cmd, int lvl)
{
	int i;

	for (i = 0; *cmd_info[i].command != '\n'; ++i)
	{
		if (!str_cmp(cmd, cmd_info[i].command))
		{
			cmd_info[i].gm_level = lvl;
			sys_log(0, "Setting command privilege: %s -> %d", cmd, lvl);
			break;
		}
	}
}

void double_dollar(const char *src, size_t src_len, char *dest, size_t dest_len)
{
	const char * tmp = src;
	size_t cur_len = 0;

	dest_len -= 1;

	while (src_len-- && *tmp)
	{
		if (*tmp == '$')
		{
			if (cur_len + 1 >= dest_len)
				break;

			*(dest++) = '$';
			*(dest++) = *(tmp++);
			cur_len += 2;
		}
		else
		{
			if (cur_len >= dest_len)
				break;

			*(dest++) = *(tmp++);
			cur_len += 1;
		}
	}

	*dest = '\0';
}

void interpret_command(LPCHARACTER ch, const char * argument, size_t len)
{
	if (NULL == ch)
	{
		sys_err ("NULL CHRACTER");
		return ;
	}

#ifdef ENABLE_EMOTICONS_SYSTEM
	char cmd[512 + 1];
	char new_line[1024 + 1];
#else
	char cmd[128 + 1];
	char new_line[256 + 1];
#endif

	const char * line;
	int icmd;

	if (len == 0 || !*argument)
		return;

	double_dollar(argument, len, new_line, sizeof(new_line));

	size_t cmdlen;
	line = first_cmd(new_line, cmd, sizeof(cmd), &cmdlen);

	for (icmd = 1; *cmd_info[icmd].command != '\n'; ++icmd)
	{
		if (cmd_info[icmd].command_pointer == do_cmd)
		{
			if (!strcmp(cmd_info[icmd].command, cmd))
				break;
		}
		else if (!strncmp(cmd_info[icmd].command, cmd, cmdlen))
			break;
	}

	if (ch->GetPosition() < cmd_info[icmd].minimum_position)
	{
		switch (ch->GetPosition())
		{
			case POS_MOUNTING:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do this whilst sitting on a Horse."));
				break;

			case POS_DEAD:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do that while you are lying on the ground."));
				break;

			case POS_SLEEPING:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("In my Dreams? What?"));
				break;

			case POS_RESTING:
			case POS_SITTING:
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Get up first."));
				break;
			default:
				sys_err("unknown position %d", ch->GetPosition());
				break;
		}

		return;
	}

	if (*cmd_info[icmd].command == '\n')
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This command does not exist."));
		return;
	}

	if (cmd_info[icmd].gm_level && cmd_info[icmd].gm_level > ch->GetGMLevel())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This command does not exist."));
		return;
	}

	if (strncmp("phase", cmd_info[icmd].command, 5) != 0)
		sys_log(0, "COMMAND: %s: %s", ch->GetName(), cmd_info[icmd].command);

	((*cmd_info[icmd].command_pointer) (ch, line, icmd, cmd_info[icmd].subcmd));

	if (ch->GetGMLevel() >= GM_LOW_WIZARD)
	{
		if (cmd_info[icmd].gm_level >= GM_LOW_WIZARD)
		{
			char buf[1024];
			snprintf( buf, sizeof(buf), "%s", argument );

			LogManager::instance().GMCommandLog(ch->GetPlayerID(), ch->GetName(), ch->GetDesc()->GetHostName(), g_bChannel, buf);
		}
	}
}
