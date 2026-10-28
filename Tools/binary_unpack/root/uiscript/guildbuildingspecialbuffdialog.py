#if app.ENABLE_GUILD_LAND_RENEWAL:
# 특수시설 버프 UI
import uiScriptLocale

WINDOW_WIDTH	= 396
WINDOW_HEIGHT	= 286

ROOT_PATH = "d:/ymir work/ui/game/guild/building_special/"

PATTERN_WINDOW_WIDTH	= 374
PATTERN_WINDOW_HEIGHT	= 242

window = {
	"name" : "GuildBuildingSpecialBuffDialog",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,

	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,

			"title" : uiScriptLocale.GUILD_BUILDING_SPECIAL_BUFF_UI_TITLE,	# 특수시설 버프
			
			"children" :
			(
				## bg pattern window
				{
					"name" : "bg_pattern_window",
					"type" : "outline_window",

					"x" : 10,
					"y" : 32,
					
					"width" : PATTERN_WINDOW_WIDTH,
					"height" : PATTERN_WINDOW_HEIGHT,

					"children" :
					(
						{
							"name" : "level_window",
							"type" : "window",
							"x" : 0,
							"y" : 0,
							"width" : PATTERN_WINDOW_WIDTH,
							"height" : 37,

							"children" :
							(
								{
									"name"	: "level_title_bg",
									"type"	: "image",
									"x"		: 7,
									"y"		: 7,
									"image"	: ROOT_PATH + "sub_menu_bar_bg_img.sub",

									"children" :
									(
										# 특수 시설
										{ "name" : "level_title_text", "type" : "text", "x" : 0, "y" : 0, "text": uiScriptLocale.GUILD_BUILDING_SPECIAL_BUFF_TITLE, "all_align" : "center", },
									),
								},

								{
									"name"		: "level_text_bg",
									"type"		: "slotbar",
									"x"			: 145,
									"y"			: 9,
									"width"		: 126,
									"height"	: 18,

									"children" :
									(
										{ "name" : "level_text", "type" : "text", "x" : 0, "y" : 0, "text": "", "all_align" : "center", },
									),
								},
							),
						},

						{
							"name" : "reroll_window",
							"type" : "window",
							"x" : 0,
							"y" : 37,
							"width" : PATTERN_WINDOW_WIDTH,
							"height" : 37,

							"children" :
							(
								{
									"name"	: "reroll_title_bg",
									"type"	: "image",
									"x"		: 7,
									"y"		: 7,
									"image"	: ROOT_PATH + "sub_menu_bar_bg_img.sub",

									"children" :
									(
										# 리롤 비용
										{ "name" : "reroll_title_text", "type" : "text", "x" : 0, "y" : 0, "text": uiScriptLocale.GUILD_BUILDING_SPECIAL_BUFF_REROLL_TITLE, "all_align" : "center", },
									),
								},

								{
									"name"		: "reroll_text_bg",
									"type"		: "slotbar",
									"x"			: 145,
									"y"			: 9,
									"width"		: 126,
									"height"	: 18,

									"children" :
									(
										{ "name" : "reroll_yang_icon", "type" : "image", "x" : 4, "y" : 2, "image"	: ROOT_PATH + "yang_icon_img.sub", },
										{ "name" : "reroll_text", "type" : "text", "x" : 4, "y" : 0, "text": "999.000.000.000", "horizontal_align" : "right", "text_horizontal_align" : "right", "vertical_align" : "center", "text_vertical_align" : "center", },
									),
								},

								{
									"name"			: "reroll_button",
									"type"			: "button",
									"x"				: 344,
									"y"				: 4,
									"default_image"	: ROOT_PATH + "reroll_button_default.sub",
									"over_image"	: ROOT_PATH + "reroll_button_over.sub",
									"down_image"	: ROOT_PATH + "reroll_button_down.sub",
								},
							),
						},

						{
							"name" : "material_window",
							"type" : "window",
							"x" : 0,
							"y" : 74,
							"width" : PATTERN_WINDOW_WIDTH,
							"height" : 37,

							"children" :
							(
								{
									"name"	: "material_title_bg",
									"type"	: "image",
									"x"		: 7,
									"y"		: 7,
									"image"	: ROOT_PATH + "sub_menu_bar_bg_img.sub",

									"children" :
									(
										# 확정 재료
										{ "name" : "material_title_text", "type" : "text", "x" : 0, "y" : 0, "text": uiScriptLocale.GUILD_BUILDING_SPECIAL_BUFF_MATERIAL_TITLE, "all_align" : "center", },
									),
								},

								{
									"name"	: "material_slot_bg",
									"type"	: "image",
									"x"		: 145,
									"y"		: 0,
									"image"	: ROOT_PATH + "slot_bg_img.sub",

									"children"	:
									(
										{
											"name"		: "material_slot",
											"type"		: "slot",
											"x"			: 0,
											"y"			: 0,
											"width"		: 32,
											"height"	: 32,

											"slot" : ( { "index":0, "x":0, "y":0, "width":32, "height":32, }, ),
										},
									),
								},

								{
									"name"			: "buff_button",
									"type"			: "button",
									"x"				: 344,
									"y"				: 4,
									"default_image"	: ROOT_PATH + "summon_button_default.sub",
									"over_image"	: ROOT_PATH + "summon_button_over.sub",
									"down_image"	: ROOT_PATH + "summon_button_down.sub",
								},
							),
						},

						{
							"name"	: "menu_bar_bg",
							"type"	: "image",
							"x"		: 2,
							"y"		: 111,
							"image"	: ROOT_PATH + "menu_bar_bg_img.sub",

							"children" :
							(
								{
									"name" : "buff_menu_buff_text_window",
									"type" : "window",
									"x" : 93,
									"y" : 0,
									"width" : 163,
									"height" : 24,

									"children" :
									(
										# 버프명
										{ "name" : "buff_menu_buff_text", "type" : "text", "x" : 0, "y" : 0, "text": uiScriptLocale.GUILD_BUILDING_SPECIAL_BUFF_NAME_TITLE, "all_align" : "center", },
									),
								},

								{
									"name" : "buff_menu_time_text_window",
									"type" : "window",
									"x" : 299,
									"y" : 0,
									"width" : 31,
									"height" : 24,

									"children" :
									(
										# 시간
										{ "name" : "buff_menu_time_text", "type" : "text", "x" : 0, "y" : 0, "text": uiScriptLocale.GUILD_BUILDING_SPECIAL_BUFF_TIME_TITLE, "all_align" : "center", },
									),
								},
							),
						},

						# 버프 1
						{
							"name"	: "buff_node_bg_1",
							"type"	: "image",
							"x"		: 6,
							"y"		: 138,
							"image"	: ROOT_PATH + "buff_info_bg_img_activated.sub",

							"children" :
							(
								{
									"name" : "buff_node_level_text_window_1",
									"type" : "window",
									"x" : 9,
									"y" : 10,
									"width" : 63,
									"height" : 11,

									"children" :
									(
										# 슬롯 1
										{ "name" : "buff_node_level_text_1", "type" : "text", "x" : 0, "y" : 0, "text": uiScriptLocale.GUILD_BUILDING_SPECIAL_BUFF_LEVEL_1, "all_align" : "center", },
									),
								},

								{
									"name" : "buff_node_name_text_window_1",
									"type" : "window",
									"x" : 89,
									"y" : 10,
									"width" : 163,
									"height" : 11,

									"children" :
									(
										{ "name" : "buff_node_name_text_1", "type" : "text", "x" : 0, "y" : 0, "text": "temp", "all_align" : "center", },
									),
								},

								{
									"name" : "buff_node_time_text_window_1",
									"type" : "window",
									"x" : 291,
									"y" : 10,
									"width" : 37,
									"height" : 11,

									"children" :
									(
										{ "name" : "buff_node_time_text_1", "type" : "text", "x" : 0, "y" : 0, "text": "8h", "all_align" : "center", },
									),
								},
							),
						},

						# 버프 2
						{
							"name"	: "buff_node_bg_2",
							"type"	: "image",
							"x"		: 6,
							"y"		: 172,
							"image"	: ROOT_PATH + "buff_info_bg_img_inactivated.sub",

							"children" :
							(
								{
									"name" : "buff_node_level_text_window_2",
									"type" : "window",
									"x" : 9,
									"y" : 10,
									"width" : 63,
									"height" : 11,

									"children" :
									(
										# 슬롯 2
										{ "name" : "buff_node_level_text_2", "type" : "text", "x" : 0, "y" : 0, "text": uiScriptLocale.GUILD_BUILDING_SPECIAL_BUFF_LEVEL_2, "all_align" : "center", },
									),
								},

								{
									"name" : "buff_node_name_text_window_2",
									"type" : "window",
									"x" : 89,
									"y" : 10,
									"width" : 163,
									"height" : 11,

									"children" :
									(
										{ "name" : "buff_node_name_text_2", "type" : "text", "x" : 0, "y" : 0, "text": "temp", "all_align" : "center", },
									),
								},

								{
									"name" : "buff_node_time_text_window_2",
									"type" : "window",
									"x" : 291,
									"y" : 10,
									"width" : 37,
									"height" : 11,

									"children" :
									(
										{ "name" : "buff_node_time_text_2", "type" : "text", "x" : 0, "y" : 0, "text": "8h", "all_align" : "center", },
									),
								},
							),
						},

						# 버프 3
						{
							"name"	: "buff_node_bg_3",
							"type"	: "image",
							"x"		: 6,
							"y"		: 206,
							"image"	: ROOT_PATH + "buff_info_bg_img_inactivated.sub",

							"children" :
							(
								{
									"name" : "buff_node_level_text_window_3",
									"type" : "window",
									"x" : 9,
									"y" : 10,
									"width" : 63,
									"height" : 11,

									"children" :
									(
										# 슬롯 3
										{ "name" : "buff_node_level_text_3", "type" : "text", "x" : 0, "y" : 0, "text": uiScriptLocale.GUILD_BUILDING_SPECIAL_BUFF_LEVEL_3, "all_align" : "center", },
									),
								},

								{
									"name" : "buff_node_name_text_window_3",
									"type" : "window",
									"x" : 89,
									"y" : 10,
									"width" : 163,
									"height" : 11,

									"children" :
									(
										{ "name" : "buff_node_name_text_3", "type" : "text", "x" : 0, "y" : 0, "text": "temp", "all_align" : "center", },
									),
								},

								{
									"name" : "buff_node_time_text_window_3",
									"type" : "window",
									"x" : 291,
									"y" : 10,
									"width" : 37,
									"height" : 11,

									"children" :
									(
										{ "name" : "buff_node_time_text_3", "type" : "text", "x" : 0, "y" : 0, "text": "8h", "all_align" : "center", },
									),
								},
							),
						},
					),
				},
			),
		},
	),
}