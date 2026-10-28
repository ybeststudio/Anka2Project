#if app.ENABLE_GUILD_LAND_RENEWAL:
# 길드부지 조경물 UI
import uiScriptLocale

WINDOW_WIDTH	= 242
WINDOW_HEIGHT	= 166

ROOT_PATH = "d:/ymir work/ui/game/guild/guilddonate/"
PATTERN_WINDOW_WIDTH	= 190
PATTERN_WINDOW_HEIGHT	= 110

window = {
	"name" : "GuildBuildingShowLandscapeDialog",
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

			"title" : uiScriptLocale.GUILD_BUILDING_LANDSCAPE_WINDOW_TITLE,	# 조경물
			
			"children" :
			(
				## bg pattern window
				{
					"name" : "bg_pattern_window",
					"type" : "slotbar",

					"x" : 17,
					"y" : 37,
					
					"width" : PATTERN_WINDOW_WIDTH,
					"height" : PATTERN_WINDOW_HEIGHT,

					"children" :
					(
						{
							"name"		: "landscape_list",
							"type"		: "listboxex",
							"style"		: ("attach", ),
							"x"			: 2,
							"y"			: 4,
							"width"		: 186,
							"height"	: 102,
						},
					),
				},

				{
					"name" : "landscape_list_scroll_bar",
					"type" : "scrollbar",
					"x" : 210,
					"y" : 37,
					"size" : 110,
				},
			),
		},
	),
}