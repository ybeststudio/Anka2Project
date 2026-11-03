import uiScriptLocale

window = {
	"name" : "QuestionDialog",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH/2 - 125,
	"y" : SCREEN_HEIGHT/2 - 52,

	"width" : 340,
	"height" : 105,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 340,
			"height" : 105,

			"children" :
			(
				{
					"name" : "message",
					"type" : "text",

					"x" : 0,
					"y" : 38,

					"horizontal_align" : "center",
					"text" : uiScriptLocale.MESSAGE,

					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "accept",
					"type" : "button",

					"x" : -40,
					"y" : 63,

					"width" : 57,
					"height" : 24,

					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/game/common/button_accept_norm.png",
					"over_image" : "d:/ymir work/ui/game/common/button_accept_hover.png",
					"down_image" : "d:/ymir work/ui/game/common/button_accept_down.png",
				},
				{
					"name" : "cancel",
					"type" : "button",

					"x" : 40,
					"y" : 63,

					"width" : 57,
					"height" : 24,

					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/game/common/button_cancel_norm.png",
					"over_image" : "d:/ymir work/ui/game/common/button_cancel_hover.png",
					"down_image" : "d:/ymir work/ui/game/common/button_cancel_down.png",
				},
			),
		},
	),
}