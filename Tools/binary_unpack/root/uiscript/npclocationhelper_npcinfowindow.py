import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/npc_location_helper/"

WINDOW_WIDTH		= 286
WINDOW_HEIGHT		= 434

window = {
	"name" : "NPCHelper_NPCInfo",
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

			"x" : 0,
			"y" : 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"title" : uiScriptLocale.NPC_HELPER_NPC_INFO_TITLE,

			"children" :
			(
				{
					"name"		: "bg_window",
					"type"		: "outline_window",
					"x"			: 10,
					"y"			: 32,
					"width"		: 266,
					"height"	: 390,

					"children"	:
					(
						# 하위 제목줄 : NPC목록
						{
							"name"	: "tab_menu_2_npc_list",
							"type"	: "image",
							"x"		: 3,
							"y"		: 109 - 32 - 14,
							"image"	: ROOT_PATH + "tab_menu_2_npc_info.sub",

							"children"	:
							(
								{
									"name"		: "tab_menu_2_npc_list",
									"type"		: "text",
									"x"			: 0,
									"y"			: 0,
									"all_align"	: "center",
									"text"		: uiScriptLocale.NPC_HELPER_NPC_INFO_MENU_NPC_LIST,
								},
							),
						},

						# NPC 목록
						{
							"name" : "npc_list_window",
							"type" : "window",

							"x"		: 0,
							"y"		: 100 - 14,
							"color"	: 0xFFFFFFFF,

							"width"		: 266,
							"height"	: 304,
						},

						# 맵 선택
						{
							"name" : "select_map_window",
							"type" : "window",

							"x"		: 21 - 10,
							"y"		: 84 - 32 - 17,

							"width"		: 225 + 16,
							"height"	: 16,
						},

						# 지역 선택
						{
							"name" : "select_category_window",
							"type" : "window",

							"x"		: 21 - 10,
							"y"		: 61 - 32 - 17,

							"width"		: 225 + 16,
							"height"	: 16,
						},
					),
				},
			),
		},
	),
}
