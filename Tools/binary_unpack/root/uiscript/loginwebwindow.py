WEB_WIDTH	= 660
WEB_HEIGHT	= 500

window = {
	"name" : "LoginWebWindow",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width"  : WEB_WIDTH,
	"height" : WEB_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width"	 : WEB_WIDTH,
			"height" : WEB_HEIGHT,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 7,

					"width" : WEB_WIDTH - 10,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x": 50, "y" : 3, "text_horizontal_align":"center" },
					),
				},
			),
		},
	),
}
