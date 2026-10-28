import uiScriptLocale
import localeInfo

PUBLIC_PATH								= "d:/ymir work/ui/public/"
PATTERN_PATH							= "d:/ymir work/ui/pattern/"
ROOT_PATH								= "d:/ymir work/ui/game/guild_battle/"

WINDOW_WIDTH							= 256
WINDOW_HEIGHT							= 368

if localeInfo.IsARABIC():
	MOVE_BUTTON_POS_X					= 0
	RELOAD_BUTTON_POS_X					= 42
	AREA_BUTTON_0_POS_X					= 2
	AREA_SELECT_BUTTON_0_POS_X			= 0
	AREA_BUTTON_2_POS_X					= 136
	AREA_SELECT_BUTTON_2_POS_X			= 134
	AREA_BUTTON_3_POS_X					= 2
	AREA_SELECT_BUTTON_3_POS_X			= 0
	AREA_BUTTON_5_POS_X					= 136
	AREA_SELECT_BUTTON_5_POS_X			= 134
	AREA_BUTTON_6_POS_X					= 2
	AREA_SELECT_BUTTON_6_POS_X			= 0
	AREA_BUTTON_8_POS_X					= 136
	AREA_SELECT_BUTTON_8_POS_X			= 134
	AREA_LINE_BASE_POS_X				= 237
	PK_INFO_BASE_POS_X					= 257
	REMAIN_MOB_INFO_BASE_POS_X			= 257
	MOB_KILL_INFO_BASE_POS_X			= 257
	MOB_KILL_MARK_BLUE_POS_X			= 53
	MOB_KILL_MARK_YELLOW_POS_X			= 107
	MOB_KILL_MARK_GREEN_POS_X			= 161
	MOB_KILL_MARK_RED_POS_X				= 215
	MOB_KILL_COUNT_TEXT_BLUE_POS_X		= 24
	MOB_KILL_COUNT_TEXT_YELLOW_POS_X	= 78
	MOB_KILL_COUNT_TEXT_GREEN_POS_X		= 132
	MOB_KILL_COUNT_TEXT_RED_POS_X		= 186
else:
	MOVE_BUTTON_POS_X					= 83
	RELOAD_BUTTON_POS_X					= 175
	AREA_BUTTON_0_POS_X					= 2
	AREA_SELECT_BUTTON_0_POS_X			= 0
	AREA_BUTTON_2_POS_X					= 136
	AREA_SELECT_BUTTON_2_POS_X			= 134
	AREA_BUTTON_3_POS_X					= 2
	AREA_SELECT_BUTTON_3_POS_X			= 0
	AREA_BUTTON_5_POS_X					= 136
	AREA_SELECT_BUTTON_5_POS_X			= 134
	AREA_BUTTON_6_POS_X					= 2
	AREA_SELECT_BUTTON_6_POS_X			= 0
	AREA_BUTTON_8_POS_X					= 136
	AREA_SELECT_BUTTON_8_POS_X			= 134
	AREA_LINE_BASE_POS_X				= 0
	PK_INFO_BASE_POS_X					= 0
	REMAIN_MOB_INFO_BASE_POS_X			= 0
	MOB_KILL_INFO_BASE_POS_X			= 0
	MOB_KILL_MARK_BLUE_POS_X			= 23
	MOB_KILL_MARK_YELLOW_POS_X			= 78
	MOB_KILL_MARK_GREEN_POS_X			= 133
	MOB_KILL_MARK_RED_POS_X				= 188
	MOB_KILL_COUNT_TEXT_BLUE_POS_X		= 39
	MOB_KILL_COUNT_TEXT_YELLOW_POS_X	= 94
	MOB_KILL_COUNT_TEXT_GREEN_POS_X		= 149
	MOB_KILL_COUNT_TEXT_RED_POS_X		= 204

window = {
	"name"		: "guild_battle_status_board_window",
	"style"		: ("movable", "float"),

	"x"			: SCREEN_WIDTH - 300,
	"y"			: 78,
	
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
			
			## 현황판
			"title"		: uiScriptLocale.GUILD_BATTLE_STATUS_BOARD,

			"children" :
			[
				# line
				{
					"name"		: "line_window",
					"type"		: "window",
					"style"		: ("ltr",),
					
					"x"			: AREA_LINE_BASE_POS_X + 10,
					"y"			: 32,

					"width"		: 0,
					"height"	: 0,

					"children" :
					[
						# width
						{
							"name" : "line_width_1",
							"type" : "image",
							
							"x" : 18,
							"y" : 96,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						{
							"name" : "line_width_2",
							"type" : "image",
							
							"x" : 85,
							"y" : 96,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						{
							"name" : "line_width_3",
							"type" : "image",
							
							"x" : 152,
							"y" : 96,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						{
							"name" : "line_width_4",
							"type" : "image",
							
							"x" : 18,
							"y" : 163,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						{
							"name" : "line_width_5",
							"type" : "image",
							
							"x" : 85,
							"y" : 163,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						{
							"name" : "line_width_6",
							"type" : "image",
							
							"x" : 152,
							"y" : 163,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						{
							"name" : "line_width_7",
							"type" : "image",
							
							"x" : 18,
							"y" : 29,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						{
							"name" : "line_width_8",
							"type" : "image",
							
							"x" : 85,
							"y" : 29,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						{
							"name" : "line_width_9",
							"type" : "image",
							
							"x" : 152,
							"y" : 29,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						{
							"name" : "line_width_10",
							"type" : "image",
							
							"x" : 18,
							"y" : 230,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						{
							"name" : "line_width_11",
							"type" : "image",
							
							"x" : 85,
							"y" : 230,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						{
							"name" : "line_width_12",
							"type" : "image",
							
							"x" : 152,
							"y" : 230,

							"image" : ROOT_PATH + "status_board_line_witdh.sub",
						},

						# height
						{
							"name" : "line_height_1",
							"type" : "image",
							
							"x" : 82,
							"y" : 32,

							"image" : ROOT_PATH + "status_board_line_height.sub",
						},

						{
							"name" : "line_height_2",
							"type" : "image",
							
							"x" : 149,
							"y" : 32,

							"image" : ROOT_PATH + "status_board_line_height.sub",
						},

						{
							"name" : "line_height_3",
							"type" : "image",
							
							"x" : 15,
							"y" : 32,

							"image" : ROOT_PATH + "status_board_line_height.sub",
						},

						{
							"name" : "line_height_4",
							"type" : "image",
							
							"x" : 216,
							"y" : 32,

							"image" : ROOT_PATH + "status_board_line_height.sub",
						},
					],
				},

				# Flag
				{
					"name"		: "flag_window",
					"type"		: "window",
					"style"		: ("ltr",),
					
					"x"			: 0,
					"y"			: 25,

					"width"		: WINDOW_WIDTH,
					"height"	: WINDOW_HEIGHT - 25,

					"children" :
					[
						{
							"name" : "flag_blue",
							"type" : "button",
							
							"x" : 12,
							"y" : 36 - 25,

							"default_image"	: ROOT_PATH + "status_board_flag_blue.sub",
							"over_image"	: ROOT_PATH + "status_board_flag_blue.sub",
							"down_image"	: ROOT_PATH + "status_board_flag_blue.sub",
						},

						{
							"name" : "flag_yellow",
							"type" : "button",
							
							"x" : 207,
							"y" : 36 - 25,

							"default_image"	: ROOT_PATH + "status_board_flag_yellow.sub",
							"over_image"	: ROOT_PATH + "status_board_flag_yellow.sub",
							"down_image"	: ROOT_PATH + "status_board_flag_yellow.sub",
						},

						{
							"name" : "flag_green",
							"type" : "button",
							
							"x" : 12,
							"y" : 270 - 25,

							"default_image"	: ROOT_PATH + "status_board_flag_green.sub",
							"over_image"	: ROOT_PATH + "status_board_flag_green.sub",
							"down_image"	: ROOT_PATH + "status_board_flag_green.sub",
						},

						{
							"name" : "flag_red",
							"type" : "button",
							
							"x" : 207,
							"y" : 270 - 25,

							"default_image"	: ROOT_PATH + "status_board_flag_red.sub",
							"over_image"	: ROOT_PATH + "status_board_flag_red.sub",
							"down_image"	: ROOT_PATH + "status_board_flag_red.sub",
						},
					],
				},

				# PK text
				{
					"name"		: "pk_window",
					"type"		: "window",
					"style"		: ("ltr",),
					
					"x"			: PK_INFO_BASE_POS_X,
					"y"			: 0,

					"width"		: 0,
					"height"	: 0,

					"children" :
					[
						{
							"name" : "pk_bg",
							"type" : "image",
							
							"x" : 55,
							"y" : 32,

							"image" : ROOT_PATH + "status_board_pk_bg.sub",
						},

						{
							"name"					: "pk_text",
							"type"					: "text",

							"x"						: 128,
							"y"						: 37,

							"horizontal_align"		: "center",
							"text_horizontal_align"	: "center",

							"text"					: "",
						},
					],
				},

				# remain monster count
				{
					"name"		: "monster_count_window",
					"type"		: "window",
					"style"		: ("ltr",),
					
					"x"			: REMAIN_MOB_INFO_BASE_POS_X,
					"y"			: 0,

					"width"		: 0,
					"height"	: 0,

					"children" :
					[
						{
							"name" : "remain_monster_count_bg",
							"type" : "image",
							
							"x" : 14,
							"y" : 303,

							"image" : ROOT_PATH + "status_board_remain_mk_bg.sub",
						},

						{
							"name"					: "remain_monster_count_text",
							"type"					: "text",

							"x"						: 125,
							"y"						: 308,

							"horizontal_align"		: "center",
							"text_horizontal_align"	: "center",

							"text"					: "remain monsters : 61/61",
						},
					],
				},

				# guild monster kill count
				{
					"name"		: "guild_monster_kill_count_window",
					"type"		: "window",
					"style"		: ("ltr",),
					
					"x"			: MOB_KILL_INFO_BASE_POS_X,
					"y"			: 0,

					"width"		: 0,
					"height"	: 0,

					"children" :
					[
						# blue
						{
							"name" : "blue_mark",
							"type" : "image",
							
							"x" : MOB_KILL_MARK_BLUE_POS_X,
							"y" : 335,

							"image" : ROOT_PATH + "status_board_blue_team_mark.sub",
						},

						{
							"name" : "blue_mk_bg",
							"type" : "image",
							
							"x" : MOB_KILL_COUNT_TEXT_BLUE_POS_X,
							"y" : 332,

							"image" : ROOT_PATH + "status_board_mk_bg.sub",

							"children" :
							[
								{
									"name" : "blue_mk_text",
									"type" : "text",

									"x" : 0,
									"y" : 2,

									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",

									"text" : "999",
								},
							],
						},

						# yellow
						{
							"name" : "yellow_mark",
							"type" : "image",
							
							"x" : MOB_KILL_MARK_YELLOW_POS_X,
							"y" : 335,

							"image" : ROOT_PATH + "status_board_yellow_team_mark.sub",
						},

						{
							"name" : "yellow_mk_bg",
							"type" : "image",
							
							"x" : MOB_KILL_COUNT_TEXT_YELLOW_POS_X,
							"y" : 332,

							"image" : ROOT_PATH + "status_board_mk_bg.sub",

							"children" :
							[
								{
									"name" : "yellow_mk_text",
									"type" : "text",

									"x" : 0,
									"y" : 2,

									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",

									"text" : "999",
								},
							],
						},

						# green
						{
							"name" : "green_mark",
							"type" : "image",
							
							"x" : MOB_KILL_MARK_GREEN_POS_X,
							"y" : 335,

							"image" : ROOT_PATH + "status_board_green_team_mark.sub",
						},

						{
							"name" : "green_mk_bg",
							"type" : "image",
							
							"x" : MOB_KILL_COUNT_TEXT_GREEN_POS_X,
							"y" : 332,

							"image" : ROOT_PATH + "status_board_mk_bg.sub",

							"children" :
							[
								{
									"name" : "green_mk_text",
									"type" : "text",

									"x" : 0,
									"y" : 2,

									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",

									"text" : "999",
								},
							],
						},

						# red
						{
							"name" : "red_mark",
							"type" : "image",
							
							"x" : MOB_KILL_MARK_RED_POS_X,
							"y" : 335,

							"image" : ROOT_PATH + "status_board_red_team_mark.sub",
						},

						{
							"name" : "red_mk_bg",
							"type" : "image",
							
							"x" : MOB_KILL_COUNT_TEXT_RED_POS_X,
							"y" : 332,

							"image" : ROOT_PATH + "status_board_mk_bg.sub",

							"children" :
							[
								{
									"name" : "red_mk_text",
									"type" : "text",

									"x" : 0,
									"y" : 2,

									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",

									"text" : "999",
								},
							],
						},
					],
				},

				# area select button
				{
					"name"		: "area_select_window",
					"type"		: "window",
					"style"		: ("ltr",),
					
					"x"			: 26,
					"y"			: 62,

					"width"		: 204,
					"height"	: 204,

					"children" :
					[
						# button 0
						{
							"name" : "area_bg_1",
							"type" : "image",
							
							"x" : AREA_BUTTON_0_POS_X,
							"y" : 2,

							"image" : ROOT_PATH + "status_board_area_bg_dark.sub",
						},

						{
							"name" : "area_occupy_team_0",
							"type" : "image",
							
							"x" : AREA_BUTTON_0_POS_X,
							"y" : 2,

							"image" : ROOT_PATH + "status_board_area_bg_dark.sub",
						},

						{
							"name" : "area_default_select_image_0",
							"type" : "image",
							
							"x" : AREA_SELECT_BUTTON_0_POS_X,
							"y" : 0,

							"image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						{
							"name" : "select_button_0",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_0_POS_X,
							"y" : 0,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
							"down_image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						# button 2
						{
							"name" : "area_bg_2",
							"type" : "image",
							
							"x" : 69,
							"y" : 2,

							"image" : ROOT_PATH + "status_board_area_bg_silver.sub",
						},

						{
							"name" : "area_occupy_team_1",
							"type" : "image",
							
							"x" : 69,
							"y" : 2,

							"image" : ROOT_PATH + "status_board_area_bg_silver.sub",
						},

						{
							"name" : "area_default_select_image_1",
							"type" : "image",
							
							"x" : 67,
							"y" : 0,

							"image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						{
							"name" : "select_button_1",
							"type" : "button",

							"x" : 67,
							"y" : 0,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
							"down_image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						# button3
						{
							"name" : "area_bg_3",
							"type" : "image",
							
							"x" : AREA_BUTTON_2_POS_X,
							"y" : 2,

							"image" : ROOT_PATH + "status_board_area_bg_dark.sub",
						},

						{
							"name" : "area_occupy_team_2",
							"type" : "image",
							
							"x" : AREA_BUTTON_2_POS_X,
							"y" : 2,

							"image" : ROOT_PATH + "status_board_area_bg_dark.sub",
						},

						{
							"name" : "area_default_select_image_2",
							"type" : "image",
							
							"x" : AREA_SELECT_BUTTON_2_POS_X,
							"y" : 0,

							"image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						{
							"name" : "select_button_2",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_2_POS_X,
							"y" : 0,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
							"down_image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						# button 4
						{
							"name" : "area_bg_4",
							"type" : "image",
							
							"x" : AREA_BUTTON_3_POS_X,
							"y" : 69,

							"image" : ROOT_PATH + "status_board_area_bg_silver.sub",
						},

						{
							"name" : "area_occupy_team_3",
							"type" : "image",
							
							"x" : AREA_BUTTON_3_POS_X,
							"y" : 69,

							"image" : ROOT_PATH + "status_board_area_bg_silver.sub",
						},

						{
							"name" : "area_default_select_image_3",
							"type" : "image",
							
							"x" : AREA_SELECT_BUTTON_3_POS_X,
							"y" : 67,

							"image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						{
							"name" : "select_button_3",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_3_POS_X,
							"y" : 67,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
							"down_image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						# button 5
						{
							"name" : "area_bg_5",
							"type" : "image",
							
							"x" : 69,
							"y" : 69,

							"image" : ROOT_PATH + "status_board_area_bg_gold.sub",
						},

						{
							"name" : "area_occupy_team_4",
							"type" : "image",
							
							"x" : 69,
							"y" : 69,

							"image" : ROOT_PATH + "status_board_area_bg_gold.sub",
						},

						{
							"name" : "area_default_select_image_4",
							"type" : "image",
							
							"x" : 67,
							"y" : 67,

							"image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						{
							"name" : "select_button_4",
							"type" : "button",

							"x" : 67,
							"y" : 67,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
							"down_image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						# button 6
						{
							"name" : "area_bg_6",
							"type" : "image",
							
							"x" : AREA_BUTTON_5_POS_X,
							"y" : 69,

							"image" : ROOT_PATH + "status_board_area_bg_silver.sub",
						},

						{
							"name" : "area_occupy_team_5",
							"type" : "image",
							
							"x" : AREA_BUTTON_5_POS_X,
							"y" : 69,

							"image" : ROOT_PATH + "status_board_area_bg_silver.sub",
						},

						{
							"name" : "area_default_select_image_5",
							"type" : "image",
							
							"x" : AREA_SELECT_BUTTON_5_POS_X,
							"y" : 67,

							"image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						{
							"name" : "select_button_5",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_5_POS_X,
							"y" : 67,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
							"down_image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						# button 7
						{
							"name" : "area_bg_7",
							"type" : "image",
							
							"x" : AREA_BUTTON_6_POS_X,
							"y" : 136,

							"image" : ROOT_PATH + "status_board_area_bg_dark.sub",
						},

						{
							"name" : "area_occupy_team_6",
							"type" : "image",
							
							"x" : AREA_BUTTON_6_POS_X,
							"y" : 136,

							"image" : ROOT_PATH + "status_board_area_bg_dark.sub",
						},

						{
							"name" : "area_default_select_image_6",
							"type" : "image",
							
							"x" : AREA_SELECT_BUTTON_6_POS_X,
							"y" : 134,

							"image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						{
							"name" : "select_button_6",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_6_POS_X,
							"y" : 134,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
							"down_image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						# button 8
						{
							"name" : "area_bg_8",
							"type" : "image",
							
							"x" : 69,
							"y" : 136,

							"image" : ROOT_PATH + "status_board_area_bg_silver.sub",
						},

						{
							"name" : "area_occupy_team_7",
							"type" : "image",
							
							"x" : 69,
							"y" : 136,

							"image" : ROOT_PATH + "status_board_area_bg_silver.sub",
						},

						{
							"name" : "area_default_select_image_7",
							"type" : "image",
							
							"x" : 67,
							"y" : 134,

							"image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						{
							"name" : "select_button_7",
							"type" : "button",

							"x" : 67,
							"y" : 134,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
							"down_image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						# button 9
						{
							"name" : "area_bg_9",
							"type" : "image",
							
							"x" : AREA_BUTTON_8_POS_X,
							"y" : 136,

							"image" : ROOT_PATH + "status_board_area_bg_dark.sub",
						},

						{
							"name" : "area_occupy_team_8",
							"type" : "image",
							
							"x" : AREA_BUTTON_8_POS_X,
							"y" : 136,

							"image" : ROOT_PATH + "status_board_area_bg_dark.sub",
						},

						{
							"name" : "area_default_select_image_8",
							"type" : "image",
							
							"x" : AREA_SELECT_BUTTON_8_POS_X,
							"y" : 134,

							"image" : ROOT_PATH + "status_board_select_button_down.sub",
						},

						{
							"name" : "select_button_8",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_8_POS_X,
							"y" : 134,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
							"down_image" : ROOT_PATH + "status_board_select_button_down.sub",
						},
					],
				},

				# under attack 
				{
					"name"		: "under_attack_window",
					"type"		: "window",
					"style"		: ("ltr",),
					
					"x"			: 26,
					"y"			: 62,

					"width"		: 0,
					"height"	: 0,

					"children" :
					[
						# 1 area
						{
							"name" : "under_attack_img_0",
							"type" : "ani_image",

							"x" : 0,
							"y" : 0,

							"delay" : 10,

							"images" :
							(
								ROOT_PATH + "status_board_select_button_over.sub",
								ROOT_PATH + "status_board_blink_none.sub",
							)
						},

						# 2 area
						{
							"name" : "under_attack_img_1",
							"type" : "ani_image",

							"x" : 67,
							"y" : 0,

							"delay" : 10,

							"images" :
							(
								ROOT_PATH + "status_board_select_button_over.sub",
								ROOT_PATH + "status_board_blink_none.sub",
							)
						},

						# 3 area
						{
							"name" : "under_attack_img_2",
							"type" : "ani_image",

							"x" : 134,
							"y" : 0,

							"delay" : 10,

							"images" :
							(
								ROOT_PATH + "status_board_select_button_over.sub",
								ROOT_PATH + "status_board_blink_none.sub",
							)
						},

						# 4 area
						{
							"name" : "under_attack_img_3",
							"type" : "ani_image",

							"x" : 0,
							"y" : 67,

							"delay" : 10,

							"images" :
							(
								ROOT_PATH + "status_board_select_button_over.sub",
								ROOT_PATH + "status_board_blink_none.sub",
							)
						},

						# 5 area
						{
							"name" : "under_attack_img_4",
							"type" : "ani_image",

							"x" : 67,
							"y" : 67,

							"delay" : 10,

							"images" :
							(
								ROOT_PATH + "status_board_select_button_over.sub",
								ROOT_PATH + "status_board_blink_none.sub",
							)
						},

						# 6 area
						{
							"name" : "under_attack_img_5",
							"type" : "ani_image",

							"x" : 134,
							"y" : 67,

							"delay" : 10,

							"images" :
							(
								ROOT_PATH + "status_board_select_button_over.sub",
								ROOT_PATH + "status_board_blink_none.sub",
							)
						},

						# 7 area
						{
							"name" : "under_attack_img_6",
							"type" : "ani_image",

							"x" : 0,
							"y" : 134,

							"delay" : 10,

							"images" :
							(
								ROOT_PATH + "status_board_select_button_over.sub",
								ROOT_PATH + "status_board_blink_none.sub",
							)
						},

						# 8 area
						{
							"name" : "under_attack_img_7",
							"type" : "ani_image",

							"x" : 67,
							"y" : 134,

							"delay" : 10,

							"images" :
							(
								ROOT_PATH + "status_board_select_button_over.sub",
								ROOT_PATH + "status_board_blink_none.sub",
							)
						},

						# 9 area
						{
							"name" : "under_attack_img_8",
							"type" : "ani_image",

							"x" : 134,
							"y" : 134,

							"delay" : 10,

							"images" :
							(
								ROOT_PATH + "status_board_select_button_over.sub",
								ROOT_PATH + "status_board_blink_none.sub",
							)
						},
					],
				},

				# move button
				{
					"name" : "move_button",
					"type" : "button",

					"x" : MOVE_BUTTON_POS_X,
					"y" : 274,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "left",
					"text" : uiScriptLocale.GUILD_BATTLE_AREA_MOVE_BUTTON,

					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image" : PUBLIC_PATH + "large_button_02.sub",
					"down_image" : PUBLIC_PATH + "large_button_03.sub",
				},

				# reload button
				{
					"name" : "reload_button",
					"type" : "button",

					"x" : RELOAD_BUTTON_POS_X,
					"y" : 273,

					"width" : 22,
					"height" : 22,

					"horizontal_align" : "left",

					"default_image" : ROOT_PATH + "status_board_refresh_bt_default.sub",
					"over_image" : ROOT_PATH + "status_board_refresh_bt_over.sub",
					"down_image" : ROOT_PATH + "status_board_refresh_bt_down.sub",
				},
			],
		},
	],
}