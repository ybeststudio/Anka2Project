import uiScriptLocale

window = {
	"name" : "PopupDialog",
	"style" : ("float",),

	"x" : SCREEN_WIDTH/2 - 250,
	"y" : SCREEN_HEIGHT/2 - 40,

	"width" : 225,
	"height" : 46,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 225,
			"height" : 46,

			"children" :
			(
				{
					"name" : "message",
					"type" : "text",

					"x" : 0,
					"y" : 14,

					"text" : uiScriptLocale.MESSAGE,
					
					"width" : 280 - 25 * 2,

					"horizontal_align" : "center",
					"text_horizontal_align" : "center",
				},
				{
					"name" : "accept",
					"type" : "button",

					"x" : 0,
					"y" : 32,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
				},
			),
		},
	),
}