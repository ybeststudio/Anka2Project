import uiScriptLocale

PATTERN_PATH		= "d:/ymir work/ui/pattern/"
ROOT_PATH			= "d:/ymir work/ui/game/otherworld/"

WINDOW_WIDTH		= 136
WINDOW_HEIGHT		= 135

window = {
	"name" : "OtherWorldInformationBoard",
	"style" : ("float","ltr", ),
	
	"x" : SCREEN_WIDTH - WINDOW_WIDTH,
	"y" : 2,
	
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	
	"children" :
	(
		{
			"name"	: "information_bg",
			"type"	: "image",
			"x"		: 0,
			"y"		: 0,
			"image"	: ROOT_PATH + "Information_board_bg.sub",
		},
		
		## alarm text window
		{
			"name" : "alarm_text_window", "type" : "window", "style" : ("attach",), "x" : 21, "y" : 59, "width" : 93, "height" : 11,
			"children" :
			(
				{ "name" : "alarm_text", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "", },
			),
		},
		
		## gauge
		{
			"name"	: "time_gauge",
			"style" : ("attach",),
			"type"	: "expanded_image",
			"x"		: 16,
			"y"		: 78,
			"image"	: ROOT_PATH + "gauge_bar.sub",
		},
		
		## time text window
		{
			"name" : "time_text_window", "type" : "window", "style" : ("attach",), "x" : 45, "y" : 80, "width" : 46, "height" : 11,
			"children" :
			(
				{ "name" : "time_text", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "", },
			),
		},

		## exit button
		{
			"name" : "exit_button",
			"type" : "button",
			
			"x" : 54,
			"y" : 99,
			
			## 나가기
			"tooltip_text" : uiScriptLocale.OTHER_WORLD_EXIT_BUTTON,
			"tooltip_x" : 0,
			"tooltip_y" : 32,
			
			"default_image"	: ROOT_PATH + "exit_button_default.sub",
			"over_image"	: ROOT_PATH + "exit_button_over.sub",
			"down_image"	: ROOT_PATH + "exit_button_down.sub",
		},
	),	
}
