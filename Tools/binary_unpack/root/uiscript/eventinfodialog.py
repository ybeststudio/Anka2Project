import uiScriptLocale

ROOT = "d:/ymir work/ui/public/"
ROOT_PATH = "d:/ymir work/ui/event/"

window = {
	"name" : "MiniGameDialog",
	"style" : ("float",),
	
	"x" : SCREEN_WIDTH - 136 - 225-13-100,
	"y" : 117-85,
	
	"width" : 325,
	"height" : 65,
	
	"children" :
	(

		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 210,
			"height" : 45,
			
			"children" :
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 7,
					"y" : 10,

					"width" : 310,

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":3, "y":0, "text": uiScriptLocale.EVENT_TITLE_BOARD, "all_align":"center" },
					),
				},
				{
					"name" : "title",
					"type" : "image",
					"x" : 8,
					"y" : 35,
					"image" : "d:/ymir work/ui/event/button/event_button_title.tga",
				},
				{
					"name" : "text",
					"type" : "text",

					"x" : 0,
					"y" : 65+2,

					"text" : "",
				},
			),
		},		
	),	
}
