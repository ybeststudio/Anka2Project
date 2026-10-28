import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/npc_location_helper/"

WINDOW_WIDTH		= 378
WINDOW_HEIGHT		= 434

window = {
	"name" : "NPCLocationHelper_Atlas",
	"style" : ("movable", "float", ),

	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,

	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"title" : uiScriptLocale.NPC_HELPER_ATLAS_TITLE,

			"children" :
			(
				{
					"name"		: "bg_window",
					"type"		: "outline_window",
					"x"			: 10,
					"y"			: 32,
					"width"		: 358,
					"height"	: 390,

					"children"	:
					(
						# 하위 제목줄 1 : 맵이름 + 맵 확대/축소 등의 버튼
						{
							"name"	: "tab_menu_1_atlas",
							"type"	: "image",
							"x"		: 3,
							"y"		: 1,			
							"image"	: ROOT_PATH + "tab_menu_1_atlas.sub",

							"children"	:
							(
								{
									"name"		: "tab_menu_1_atlas_text",
									"type"		: "text",
									"x"			: 10,
									"y"			: 0,
									"text"		: "",

									"horizontal_align"		: "left",
									"vertical_align"		: "center",
									"text_vertical_align"	: "center",
								},

								{
									"name"			: "show_guild_land_button",
									"type"			: "button",
									"x"				: 245 - 13 - 14,
									"y"				: 38 - 33,
									
									"default_image" : ROOT_PATH + "button_show_guild_land_default.sub",
									"over_image"	: ROOT_PATH + "button_show_guild_land_over.sub",
									"down_image"	: ROOT_PATH + "button_show_guild_land_down.sub",
								},

								{
									"name"			: "drop_item_info_button",
									"type"			: "button",
									"x"				: 273 - 13  - 14,
									"y"				: 38 - 33,
									
									"default_image" : ROOT_PATH + "button_drop_item_info_default.sub",
									"over_image"	: ROOT_PATH + "button_drop_item_info_over.sub",
									"down_image"	: ROOT_PATH + "button_drop_item_info_down.sub",
								},

								{
									"name"			: "scale_up_button",
									"type"			: "button",
									"x"				: 298 - 13  - 14,
									"y"				: 37 - 33,
									
									"default_image" : ROOT_PATH + "button_scale_up_default.sub",
									"over_image"	: ROOT_PATH + "button_scale_up_over.sub",
									"down_image"	: ROOT_PATH + "button_scale_up_down.sub",
								},

								{
									"name"			: "scale_down_button",
									"type"			: "button",
									"x"				: 324 - 13  - 14,
									"y"				: 37 - 33,
									
									"default_image" : ROOT_PATH + "button_scale_down_default.sub",
									"over_image"	: ROOT_PATH + "button_scale_down_over.sub",
									"down_image"	: ROOT_PATH + "button_scale_down_down.sub",
								},

								{
									"name"			: "my_location_button",
									"type"			: "button",
									"x"				: 350 - 13  - 14,
									"y"				: 37 - 33,
									
									"default_image" : ROOT_PATH + "button_my_location_default.sub",
									"over_image"	: ROOT_PATH + "button_my_location_over.sub",
									"down_image"	: ROOT_PATH + "button_my_location_down.sub",
								},
							),
						},

						{
							"name"		: "atlas_window",
							"type"		: "window",
							"style"		: ( "float", "dragable", ),
							"x"			: 3,
							"y"			: 35,
							"width"		: 352,
							"height"	: 352,

							"children"	:
							(
								{
									"name"		: "atlas_text_1",
									"type"		: "text",
									"x"			: 0,
									"y"			: -10,
									"text"		: "",

									"all_align" : "center",
								},

								{
									"name"		: "atlas_text_2",
									"type"		: "text",
									"x"			: 0,
									"y"			: 10,
									"text"		: "",

									"all_align" : "center",
								},
							)
						},
					),
				},
			),
		},
	),
}
