import uiScriptLocale

window = {
	"name" : "PopupDialog",
	"style" : ("float",),

	"x" : SCREEN_WIDTH/2 - 250,
	"y" : SCREEN_HEIGHT/2 - 40,

	"width" : 225,
	"height" : 105,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 225,
			"height" : 105,

			"children" :
			(
				{
					"name" : "message",
					"type" : "text",

					"x" : 0,
					"y" : 28,

					"text" : uiScriptLocale.MESSAGE,
					
					"width" : 280 - 25 * 2,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
				},
				{
					"name" : "accept",
					"type" : "button",

					"x" : 0,
					"y" : 63,

					"width" : 57,
					"height" : 24,

					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/game/common/button_accept_norm.png",
					"over_image" : "d:/ymir work/ui/game/common/button_accept_hover.png",
					"down_image" : "d:/ymir work/ui/game/common/button_accept_down.png",
				},
			),
		},
	),
}