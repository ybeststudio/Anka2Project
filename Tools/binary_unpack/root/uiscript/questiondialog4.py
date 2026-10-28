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
					"name" : "circle",
					"type" : "thinboard_circle",
					"x" : 12,
					"y" : 30,
					"width" : 340-16,
					"height" : 105-38,
				},
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
			),
		},
	),
}