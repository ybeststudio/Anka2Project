import uiScriptLocale

WEB_WIDTH = 100
WEB_HEIGHT = 125

window = {
	"name" : "DutchAuctionWindow",

	"x" : 68+32,
	"y" : SCREEN_HEIGHT-200, 
	##"y" : SCREEN_HEIGHT / 2 + 100,

	"style" : ("float",),

	"width"  : WEB_WIDTH  + 20,
	"height" : WEB_HEIGHT + 40,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width"	 : WEB_WIDTH  + 20,
			"height" : WEB_HEIGHT + 40,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 7,

					"width" : WEB_WIDTH + 5,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":0, "text":uiScriptLocale.DUTCHAUCTION_TITLE, "all_align":"center"},
					),
				},
				## MallOpenButton
				{
					"name" : "MallOpenButton",
					"type" : "button",
					"horizontal_align" : "center",


					"x" : 0,
					"y" : 135,

					"text" : uiScriptLocale.DUTCHAUCTION_BUTTON,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
			),
		},
	),
}
