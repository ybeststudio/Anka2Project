import uiScriptLocale

BOARD_WIDTH = 190
BOARD_HEIGHT = 158+15+10
LEFT = 11

window = {
	"name" : "BiyologWindow",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float", "ltr"),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT+8,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach", "ltr"),

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT+5,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6,
					"y" : 6,

					"width" : BOARD_WIDTH-13,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":BOARD_WIDTH/2-7, "y":3, "text": "Bonus Seç", "text_horizontal_align":"center" },
					),
				},
				{
					"name" : "BlackBoard",
					"type" : "thinboard",
					"x" : 13, "y" : 32, "width" : BOARD_WIDTH-28, "height" : BOARD_HEIGHT-67,
				},
				{
					"name" : "AcceptButton",
					"type" : "button",
					"text" : uiScriptLocale.OK,
					"x" : LEFT+52,
					"y" : BOARD_HEIGHT-28,
					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
			),
		},
	),
}