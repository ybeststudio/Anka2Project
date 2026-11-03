import uiScriptLocale

window = {
	"name" : "MountUpGradeDialog",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH / 2 - 125,
	"y" : SCREEN_HEIGHT / 2 - 52,

	"width" : 385,
	"height" : 105,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 385,
			"height" : 105,

			"children" :
			(
				{
					"name" : "message1",
					"type" : "text",

					"x" : 0,
					"y" : 25,

					"text" : uiScriptLocale.MESSAGE,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "message2",
					"type" : "text",

					"x" : 0,
					"y" : 50,

					"text" : uiScriptLocale.MESSAGE,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "accept",
					"type" : "button",

					"x" : -40,
					"y" : 68,

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

					"x" : +40,
					"y" : 68,

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
