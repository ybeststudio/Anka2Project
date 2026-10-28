import uiScriptLocale
import localeInfo

PUBLIC_PATH								= "d:/ymir work/ui/public/"
PATTERN_PATH							= "d:/ymir work/ui/pattern/"
ROOT_PATH								= "d:/ymir work/ui/game/guild_battle/"

WINDOW_WIDTH							= 256
WINDOW_HEIGHT							= 358

if localeInfo.IsARABIC():
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
	RANKING_BUTTON_BASE_POS_X			= 0
	EXIT_BUTTON_BASE_POS_X				= 0
else:
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
	RANKING_BUTTON_BASE_POS_X			= 0
	EXIT_BUTTON_BASE_POS_X				= 0


window = {
	"name"		: "guild_battle_status_board_window",
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
			
			# 길드대전 결과
			"title"		: uiScriptLocale.GUILD_BATTLE_RESULT_BOARD,

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
							"name" : "select_button_0",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_0_POS_X,
							"y" : 0,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
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
							"name" : "select_button_1",
							"type" : "button",

							"x" : 67,
							"y" : 0,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
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
							"name" : "select_button_2",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_2_POS_X,
							"y" : 0,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
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
							"name" : "select_button_3",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_3_POS_X,
							"y" : 67,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
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
							"name" : "select_button_4",
							"type" : "button",

							"x" : 67,
							"y" : 67,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
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
							"name" : "select_button_5",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_5_POS_X,
							"y" : 67,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
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
							"name" : "select_button_6",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_6_POS_X,
							"y" : 134,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
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
							"name" : "select_button_7",
							"type" : "button",

							"x" : 67,
							"y" : 134,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
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
							"name" : "select_button_8",
							"type" : "button",

							"x" : AREA_SELECT_BUTTON_8_POS_X,
							"y" : 134,

							"width" : 68,
							"height" : 68,

							"horizontal_align" : "left",

							"over_image" : ROOT_PATH + "status_board_select_button_over.sub",
						},
					],
				},

				# 남은 시간
				{
					"name"		: "remain_time_bg_window",
					"type"		: "window",
					"style"		: ("ltr",),
					
					"x"			: 53,
					"y"			: 269,

					"width"		: 149,
					"height"	: 45,

					"children" :
					[
						{
							"name" : "remain_time_bg",
							"type" : "image",
							"x" : 0,
							"y" : 0,
							"image" : ROOT_PATH + "result_board_remain_time.sub",
						},
					],
				},

				{
					"name"					: "remain_time_text",
					"type"					: "text",
					"x"						: 0,
					"y"						: 277,
					"horizontal_align"		: "center",
					"text_horizontal_align"	: "center",

					"text"					: "",
				},

				# ranking button
				{
					"name" : "ranking_button",
					"type" : "button",

					"x" : RANKING_BUTTON_BASE_POS_X - 54,
					"y" : 323,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",
					"text" : uiScriptLocale.GUILD_BATTLE_RESULT_BOARD_RANKING,		# 랭킹

					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image" : PUBLIC_PATH + "large_button_02.sub",
					"down_image" : PUBLIC_PATH + "large_button_03.sub",
				},

				# exit button
				{
					"name" : "exit_button",
					"type" : "button",

					"x" : EXIT_BUTTON_BASE_POS_X + 55,
					"y" : 323,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",
					"text" : uiScriptLocale.GUILD_BATTLE_RESULT_BOARD_EXIT,			# 나가기

					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image" : PUBLIC_PATH + "large_button_02.sub",
					"down_image" : PUBLIC_PATH + "large_button_03.sub",
				},
			],
		},
	],
}