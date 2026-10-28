import uiScriptLocale

# Atlas Size : 126 * 126
WINDOW_WIDTH	= 126 + 24
WINDOW_HEIGHT	= 126 + 66
ROOT_PATH		= "d:/ymir work/ui/occupation/"
PUBLIC_PATH		= "d:/ymir work/ui/public/"

window = {
	"name" : "MistsIsland_AtlasWindow", "style" : ("movable", "float",),
	"x" : (SCREEN_WIDTH - WINDOW_WIDTH) / 2 + 121, "y" : (SCREEN_HEIGHT - WINDOW_HEIGHT) / 2 + 72,
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar",
			"x" : 0, "y" : 0,
			"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"title" : uiScriptLocale.ZONE_MAP,
		},
		{
			"name" : "score_button", "type" : "button",
			"x" : 12, "y" : 162,
			"default_image"	: ROOT_PATH + "btn_score_01.sub",
			"over_image"	: ROOT_PATH + "btn_score_02.sub",
			"down_image"	: ROOT_PATH + "btn_score_03.sub",
		},
		{
			"name" : "rank_button", "type" : "button",
			"x" : 31, "y" : 162,
			"default_image"	: ROOT_PATH + "btn_rank_01.sub",
			"over_image"	: ROOT_PATH + "btn_rank_02.sub",
			"down_image"	: ROOT_PATH + "btn_rank_03.sub",
		},
		{
			"name" : "exit_button", "type" : "button",
			"x" : 53, "y" : 161,
			"default_image"	: PUBLIC_PATH + "large_button_01.sub",
			"over_image"	: PUBLIC_PATH + "large_button_02.sub",
			"down_image"	: PUBLIC_PATH + "large_button_03.sub",
			"text" : uiScriptLocale.MISTS_ISLAND_EXIT,
		},
	),
}
