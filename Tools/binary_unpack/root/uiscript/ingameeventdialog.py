import uiScriptLocale

IN_GAME_UI_WIDTH	= 336

window = {
	"name" : "InGameEventWindow",
	"type" : "window",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH/2 - 30,
	"y" : 58 + 15,
	
	"width" : IN_GAME_UI_WIDTH,
	"height" : 52,

	"children" :
	[
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : IN_GAME_UI_WIDTH,
			"height" : 52,

			"children" :
			[
				{
					"name" : "Event_Title_Bar", "type" : "titlebar", "x" : 0, "y" : 5, "width" : IN_GAME_UI_WIDTH - 5, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"TitleName", "type":"text", "x":0, "y":0, "text":uiScriptLocale.EVENT_ALARM_TITLE, "all_align" : "center"},
					),
				},
				
				{ "name" : "bg_menu_tab", "type" : "expanded_image", "style" : ("attach",), "x" : 10, "y" : 35, "image" : "d:/ymir work/ui/event/bg_menu_tab.sub" },						
				
				{
					"name" : "Event_Name_Bar", "type" : "window", "x" : 10, "y" : 35, "width" : 197-10, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"TitleName", "type":"text", "x":0, "y":0, "text":uiScriptLocale.EVENT_NAME_TITLE, "all_align" : "center",},
					),
				},
				{
					"name" : "Event_Reward_Bar", "type" : "window", "x" : 197, "y" : 35, "width" : 309 - 197 - 10, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"TitleName", "type":"text", "x":0, "y":0, "text":uiScriptLocale.EVENT_REWARD_TITLE, "all_align" : "center",},
					),
				},
				{
					"name" : "Event_Scroll_Bar",
					"type" : "scrollbar",

					"x" : IN_GAME_UI_WIDTH - 308,
					"y" : 58,
					"size" : 230,
					"horizontal_align" : "right",
				},
				{
					"name" : "Event_Calendar_Button",
					"type" : "button",

					"x" : 285,
					"y" : 7,

					"default_image" : "d:/ymir work/ui/event/calendar_button_default.sub",
					"over_image" : "d:/ymir work/ui/event/calendar_button_over.sub",
					"down_image" : "d:/ymir work/ui/event/calendar_button_default.sub",
				},
			],
		},
	],
}