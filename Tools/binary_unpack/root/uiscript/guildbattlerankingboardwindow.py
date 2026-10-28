import uiScriptLocale
import localeInfo

PUBLIC_PATH					= "d:/ymir work/ui/public/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"
ROOT_PATH					= "d:/ymir work/ui/game/guild_battle/"

WINDOW_WIDTH				= 362
WINDOW_HEIGHT				= 358
if localeInfo.IsARABIC():
	TEAM_MARK_POS_X			= 181
else:
	TEAM_MARK_POS_X			= 173

window = {
	"name"		: "guild_battle_ranking_board_window",
	"style"		: ("not_pick", ),

	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH/2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT/2,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	[
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			
			"x"			: 0,
			"y"			: 0,
			
			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,
			
			# 길드대전 랭킹
			"title"		: uiScriptLocale.GUILD_BATTLE_RANK_BOARD,

			"children" :
			[
				# 테두리
				{
					"name"		: "outline_window",
					"type"		: "window",
					"x"			: 10,
					"y"			: 32,
					"width"		: WINDOW_WIDTH,
					"height"	: WINDOW_HEIGHT,
					"style"		: ("not_pick", ),

					"children"	:
					[
						{
							"name"		: "main_outline",
							"type"		: "outline_window",
							"x"			: 0,
							"y"			: 0,
							"width"		: 340,
							"height"	: 316,
						},
					]
				},

				## 랭킹 메뉴 탭
				{
					"name" : "ranking_menu_bg",
					"type" : "expanded_image",
					"x" : 13,
					"y" : 32,
					"width" : 340,
					"height" : 316,
					"image" : ROOT_PATH + "ranking_board_menue_bar_bg.sub",
					"children" :
					[
						# 순위
						{ "type" : "window", "x" : 13, "y" : 13, "width" : 21, "height" : 12, "children": [ { "name" : "rank_column_rank", "type" : "text", "style" : ("ltr",), "x" : 0, "y" : 0, "text" : uiScriptLocale.GUILD_BATTLE_RANK_BOARD_RANK, "r":0.749, "g":0.718, "b":0.675, "a":1.0, "horizontal_align" : "center", "text_horizontal_align" : "center"},],},
						# 이름
						{ "type" : "window", "x" : 101, "y" : 13, "width" : 22, "height" : 12, "children": [ { "name" : "rank_column_name", "type" : "text", "style" : ("ltr",), "x" : 0, "y" : 0, "text" : uiScriptLocale.GUILD_BATTLE_RANK_BOARD_NAME, "r":0.749, "g":0.718, "b":0.675, "a":1.0, "horizontal_align" : "center", "text_horizontal_align" : "center"},],},
						# 팀
						{ "type" : "window", "x" : 183, "y" : 13, "width" : 9, "height" : 12, "children": [ { "name" : "rank_column_team", "type" : "text", "style" : ("ltr",), "x" : 0,	"y" : 0, "text" : uiScriptLocale.GUILD_BATTLE_RANK_BOARD_TEAM, "r":0.749, "g":0.718, "b":0.675, "a":1.0, "horizontal_align" : "center", "text_horizontal_align" : "center"},],},
						# PC KILL
						{ "type" : "window", "x" : 224, "y" : 7, "width" : 11, "height" : 10, "children": [ { "name" : "rank_column_empire", "type" : "text", "style" : ("ltr",), "x" : 0, "y" : 0,	"text" : uiScriptLocale.GUILD_BATTLE_RANK_BOARD_PC_KILL, "r":0.749, "g":0.718, "b":0.675, "a":1.0, "horizontal_align" : "center", "text_horizontal_align" : "center"},],},
						# MOB KILL
						{ "type" : "window", "x" : 262, "y" : 7, "width" : 17, "height" : 10, "children": [ { "name" : "rank_column_tier", "type" : "text", "style" : ("ltr",), "x" : 0, "y" : 0, "text" : uiScriptLocale.GUILD_BATTLE_RANK_BOARD_MOB_KILL, "r":0.749, "g":0.718, "b":0.675, "a":1.0, "horizontal_align" : "center", "text_horizontal_align" : "center"},],},
						# PC Dead
						{ "type" : "window", "x" : 305, "y" : 7, "width" : 11, "height" : 10, "children":[ { "name" : "rank_column_tier", "type" : "text", "style" : ("ltr",), "x" : 0, "y" : 0, "text" : uiScriptLocale.GUILD_BATTLE_RANK_BOARD_PC_DEAD, "r":0.749, "g":0.718, "b":0.675, "a":1.0, "horizontal_align" : "center", "text_horizontal_align" : "center"},],},
					],
				},

				# 상위권 랭킹
				{
					"name"		: "high_ranking_list",
					"type"		: "listboxex",
					"x"			: 15,
					"y"			: 70,
					"width"		: 353,
					"height"	: 280,
				},

				# 나의랭킹
				{
					"name" : "my_ranking_item",
					"type" : "expanded_image",
					"x" : 15,
					"y" : WINDOW_HEIGHT - 37,
					"width" : 368,
					"height" : 21,
					"image" : ROOT_PATH + "ranking_board_my_rank_bg.sub",
					
					"children" :
					(
						## Text
						{
							"name" : "my_rank_rank_window", "type" : "window", "x" : 6, "y" : 3, "width" : 38, "height" : 16,
							
							"children" :
							[
								{ "name" : "my_rank_rank_text", "type" : "text", "style" : ("ltr",), "x" : 2, "y" : 0, "text" : "", "r":0.812, "g":0.749, "b":0.651, "a":1.0, "horizontal_align" : "right", "text_horizontal_align" : "right"},
							],
						},
						
						{
							"name" : "my_rank_name_window", "type" : "window", "x" : 51, "y" : 3, "width" : 115, "height" : 16,
							
							"children" :
							[
								{ "name" : "my_rank_name_text", "type" : "text", "style" : ("ltr",), "x" : 2, "y" : 0, "text" : "", "r":0.812, "g":0.749, "b":0.651, "a":1.0, "horizontal_align" : "right", "text_horizontal_align" : "right"},
							],
						},
						
						{ "name" : "my_rank_team_img", "type" : "image", "style" : ("ltr",), "x" : TEAM_MARK_POS_X,	"y" : 3, "image" : "d:/ymir work/ui/public/battle/empire_empty.sub"},
						
						{
							"name" : "my_rank_pc_kill_window", "type" : "window", "x" : 213, "y" : 3, "width" : 25, "height" : 16,
						
							"children" :
							[
								{ "name" : "my_rank_pc_kill_text", "type" : "text", "style" : ("ltr",), "x" : 0,	"y" : 0, "text" : "", "r":0.812, "g":0.749, "b":0.651, "a":1.0, "horizontal_align" : "center", "text_horizontal_align" : "center"},
							],
						},

						{
							"name" : "my_rank_mob_kill_window", "type" : "window", "x" : 255, "y" : 3, "width" : 25, "height" : 16,
						
							"children" :
							[
								{ "name" : "my_rank_mob_kill_text", "type" : "text", "style" : ("ltr",), "x" : 0,	"y" : 0, "text" : "", "r":0.812, "g":0.749, "b":0.651, "a":1.0, "horizontal_align" : "center", "text_horizontal_align" : "center"},
							],
						},

						{
							"name" : "my_rank_dead_window", "type" : "window", "x" : 298, "y" : 3, "width" : 25, "height" : 16,
						
							"children" :
							[
								{ "name" : "my_rank_dead_text", "type" : "text", "style" : ("ltr",), "x" : 0,	"y" : 0, "text" : "", "r":0.812, "g":0.749, "b":0.651, "a":1.0, "horizontal_align" : "center", "text_horizontal_align" : "center"},
							],
						},

					),
				},
			],
		},
	],
}