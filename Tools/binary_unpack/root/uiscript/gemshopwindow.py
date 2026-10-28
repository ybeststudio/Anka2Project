import uiScriptLocale

BUTTON_ROOT = "d:/ymir work/ui/public/"
ROOT_PATH = "d:/ymir work/ui/game/windows/"
GEM_PATH = "d:/ymir work/ui/gemshop/"
GEM_ICON = "d:/ymir work/ui/gemshop/gemshop_gemicon.sub"
BOARD_WIDTH = 168
BOARD_HEIGHT = 244

window = {
	"name" : "GemShopWindow",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6,
					"y" : 6,

					"width" : BOARD_WIDTH-15,
					"color" : "yellow",

					"children" :
					(
						{ "name" : "TitleName", "type" : "text", "x" : BOARD_WIDTH / 2 - 5, "y" : 3, "text" : uiScriptLocale.GEMS_ISIM, "text_horizontal_align" : "center" },
					),
				},

				{
					"name" : "gemshopbackimg",
					"type" : "image",
					"x" : 14,
					"y" : 31,
					"image" : GEM_PATH + "gemshop_backimg.sub",
				},

				{ "name" : "BuyRefreshTimeName", "type" : "text", "x" : BOARD_WIDTH / 2 - 10, "y" : 35, "text" : uiScriptLocale.KALAN_SURE, "text_horizontal_align" : "center" },
				{ "name" : "BuyRefreshTime", "type" : "text", "x" : (BOARD_WIDTH / 2) + 48, "y" : 35, "text" : "10:10", "text_horizontal_align" : "center" },

				{
					"name" : "RefreshButton",
					"type" : "button",

					"x" : 13,
					"y" : 30,

					"default_image" : GEM_PATH + "gemshop_refreshbutton_up.sub",
					"over_image" : GEM_PATH + "gemshop_refreshbutton_over.sub",
					"down_image" : GEM_PATH + "gemshop_refreshbutton_down.sub",
				},
			),
		},
	),
}