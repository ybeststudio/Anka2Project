import uiScriptLocale

WINDOW_WIDTH	= 204
WINDOW_HEIGHT	= 230

BUTTON_START_X	= 13
BUTTON_START_Y	= 35
BUTTON_HEIGHT	= 21
BUTTON_GAP		= 2

window = {
	"name" : "RefineElementChangeDialog",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,

	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),
			
			"x" : 0,
			"y" : 0,
			
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			
			"title" : uiScriptLocale.REFINE_ELEMENT_CHANGE_TITLE,
			
			"children" :
			(
				{
					"name" : "FireButton",
					"type" : "radio_button",

					"x" : BUTTON_START_X,
					"y" : BUTTON_START_Y,

					"text" : uiScriptLocale.REFINE_ELEMENT_CHANGE_FIRE,

					"default_image" : "d:/ymir work/ui/event/long_button_01.sub",
					"over_image" : "d:/ymir work/ui/event/long_button_02.sub",
					"down_image" : "d:/ymir work/ui/event/long_button_03.sub",
					"disable_image" : "d:/ymir work/ui/event/long_button_disable.sub",
				},
				{
					"name" : "IceButton",
					"type" : "radio_button",

					"x" : BUTTON_START_X,
					"y" : BUTTON_START_Y + BUTTON_HEIGHT*1 + BUTTON_GAP*1,

					"text" : uiScriptLocale.REFINE_ELEMENT_CHANGE_ICE,

					"default_image" : "d:/ymir work/ui/event/long_button_01.sub",
					"over_image" : "d:/ymir work/ui/event/long_button_02.sub",
					"down_image" : "d:/ymir work/ui/event/long_button_03.sub",
					"disable_image" : "d:/ymir work/ui/event/long_button_disable.sub",
				},
				{
					"name" : "WindButton",
					"type" : "radio_button",

					"x" : BUTTON_START_X,
					"y" : BUTTON_START_Y + BUTTON_HEIGHT*2 + BUTTON_GAP*2,

					"text" : uiScriptLocale.REFINE_ELEMENT_CHANGE_WIND,

					"default_image" : "d:/ymir work/ui/event/long_button_01.sub",
					"over_image" : "d:/ymir work/ui/event/long_button_02.sub",
					"down_image" : "d:/ymir work/ui/event/long_button_03.sub",
					"disable_image" : "d:/ymir work/ui/event/long_button_disable.sub",
				},
				{
					"name" : "ElectButton",
					"type" : "radio_button",

					"x" : BUTTON_START_X,
					"y" : BUTTON_START_Y + BUTTON_HEIGHT*3 + BUTTON_GAP*3,

					"text" : uiScriptLocale.REFINE_ELEMENT_CHANGE_ELECT,

					"default_image" : "d:/ymir work/ui/event/long_button_01.sub",
					"over_image" : "d:/ymir work/ui/event/long_button_02.sub",
					"down_image" : "d:/ymir work/ui/event/long_button_03.sub",
					"disable_image" : "d:/ymir work/ui/event/long_button_disable.sub",
				},
				{
					"name" : "EarthButton",
					"type" : "radio_button",

					"x" : BUTTON_START_X,
					"y" : BUTTON_START_Y + BUTTON_HEIGHT*4 + BUTTON_GAP*4,

					"text" : uiScriptLocale.REFINE_ELEMENT_CHANGE_EARTH,

					"default_image" : "d:/ymir work/ui/event/long_button_01.sub",
					"over_image" : "d:/ymir work/ui/event/long_button_02.sub",
					"down_image" : "d:/ymir work/ui/event/long_button_03.sub",
					"disable_image" : "d:/ymir work/ui/event/long_button_disable.sub",
				},
				{
					"name" : "DarkButton",
					"type" : "radio_button",

					"x" : BUTTON_START_X,
					"y" : BUTTON_START_Y + BUTTON_HEIGHT*5 + BUTTON_GAP*5,

					"text" : uiScriptLocale.REFINE_ELEMENT_CHANGE_DARK,

					"default_image" : "d:/ymir work/ui/event/long_button_01.sub",
					"over_image" : "d:/ymir work/ui/event/long_button_02.sub",
					"down_image" : "d:/ymir work/ui/event/long_button_03.sub",
					"disable_image" : "d:/ymir work/ui/event/long_button_disable.sub",
					
				},
				{ 
					"name" : "CostTitleWindow", "type" : "window", "x" : 13, "y" : 176, "width" : 92, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{"name":"CostTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.REFINE_ELEMENT_CHANGE_COST_TITLE, "all_align" : "center"},
					),
				},
				{ 
					"name" : "CostWindow", "type" : "window", "x" : 112, "y" : 176, "width" : 75, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{"name":"CostBG", "type":"image", "x":0, "y":0, "image" : "d:/ymir work/ui/event/number_input_area.sub"},
						{"name":"Cost", "type":"text", "x":0, "y":0, "text": "", "all_align" : "center"},
					),
				},
				{
					"name" : "AcceptButton",
					"type" : "button",

					"x" : 13,
					"y" : 199,

					"text" : uiScriptLocale.OK,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				{
					"name" : "CancelButton",
					"type" : "button",

					"x" : 101,
					"y" : 199,

					"text" : uiScriptLocale.CANCEL,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
			),
		},
	),
}