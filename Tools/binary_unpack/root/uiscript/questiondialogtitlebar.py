import uiScriptLocale
import app

MAIN_WINDOW_X		= SCREEN_WIDTH / 2 - 125
MAIN_WINDOW_Y		= SCREEN_HEIGHT / 2 - 52
MAIN_WINDOW_WIDTH	= 280
MAIN_WINDOW_HEIGHT	= 105

MESSAGE_Y			= 45

BUTTON_X			= 40
BUTTON_Y			= 68
BUTTON_WIDTH		= 61
BUTTON_HEIGHT		= 21

if app.ENABLE_FLEXIBLE_WIDTH_BUTTON:
	window = {
		"name" : "QuestionDialogTitleBar",
		"style" : ("movable", "float",),
	
		"x" : MAIN_WINDOW_X,
		"y" : MAIN_WINDOW_Y,
	
		"width" : MAIN_WINDOW_WIDTH,
		"height" : MAIN_WINDOW_HEIGHT,
	
		"children" :
		(
			{
				"name" : "board",
				"type" : "board_with_titlebar",
			
				"x" : 0,
				"y" : 0,
			
				"width" : MAIN_WINDOW_WIDTH,
				"height" : MAIN_WINDOW_HEIGHT,
			
				"title" : "",
			
				"children" :
				(
					{
						"name" : "message",
						"type" : "text",
					
						"x" : 0,
						"y" : MESSAGE_Y,
					
						"text" : uiScriptLocale.MESSAGE,
					
						"horizontal_align" : "center",
						"text_horizontal_align" : "center",
						"text_vertical_align" : "center",
					},
				
					{
						"name" : "button1",
						"type" : "flexible_width_button",
					
						"x" : -BUTTON_X,
						"y" : BUTTON_Y,
					
						"width" : BUTTON_WIDTH,
						"height" : BUTTON_HEIGHT,
					
						"horizontal_align" : "center",
						"text" : uiScriptLocale.YES,
					
						"default_image"			: "d:/ymir work/ui/public/flexible_width_button/middle_button_default_center.sub",
						"default_left_image"	: "d:/ymir work/ui/public/flexible_width_button/middle_button_default_left.sub",
						"default_right_image"	: "d:/ymir work/ui/public/flexible_width_button/middle_button_default_right.sub",

						"over_image"			: "d:/ymir work/ui/public/flexible_width_button/middle_button_over_center.sub",
						"over_left_image"		: "d:/ymir work/ui/public/flexible_width_button/middle_button_over_left.sub",
						"over_right_image"		: "d:/ymir work/ui/public/flexible_width_button/middle_button_over_right.sub",

						"down_image"			: "d:/ymir work/ui/public/flexible_width_button/middle_button_down_center.sub",
						"down_left_image"		: "d:/ymir work/ui/public/flexible_width_button/middle_button_down_left.sub",
						"down_right_image"		: "d:/ymir work/ui/public/flexible_width_button/middle_button_down_right.sub",
					},
				
					{
						"name" : "button2",
						"type" : "flexible_width_button",
					
						"x" : BUTTON_X,
						"y" : BUTTON_Y,
					
						"width" : BUTTON_WIDTH,
						"height" : BUTTON_HEIGHT,
					
						"horizontal_align" : "center",
						"text" : uiScriptLocale.NO,
					
						"default_image"			: "d:/ymir work/ui/public/flexible_width_button/middle_button_default_center.sub",
						"default_left_image"	: "d:/ymir work/ui/public/flexible_width_button/middle_button_default_left.sub",
						"default_right_image"	: "d:/ymir work/ui/public/flexible_width_button/middle_button_default_right.sub",

						"over_image"			: "d:/ymir work/ui/public/flexible_width_button/middle_button_over_center.sub",
						"over_left_image"		: "d:/ymir work/ui/public/flexible_width_button/middle_button_over_left.sub",
						"over_right_image"		: "d:/ymir work/ui/public/flexible_width_button/middle_button_over_right.sub",

						"down_image"			: "d:/ymir work/ui/public/flexible_width_button/middle_button_down_center.sub",
						"down_left_image"		: "d:/ymir work/ui/public/flexible_width_button/middle_button_down_left.sub",
						"down_right_image"		: "d:/ymir work/ui/public/flexible_width_button/middle_button_down_right.sub",
					},
				),
			},
		),
	}
else:
	window = {
		"name" : "QuestionDialogTitleBar",
		"style" : ("movable", "float",),
	
		"x" : MAIN_WINDOW_X,
		"y" : MAIN_WINDOW_Y,
	
		"width" : MAIN_WINDOW_WIDTH,
		"height" : MAIN_WINDOW_HEIGHT,
	
		"children" :
		(
			{
				"name" : "board",
				"type" : "board_with_titlebar",
			
				"x" : 0,
				"y" : 0,
			
				"width" : MAIN_WINDOW_WIDTH,
				"height" : MAIN_WINDOW_HEIGHT,
			
				"title" : "",
			
				"children" :
				(
					{
						"name" : "message",
						"type" : "text",
					
						"x" : 0,
						"y" : MESSAGE_Y,
					
						"text" : uiScriptLocale.MESSAGE,
					
						"horizontal_align" : "center",
						"text_horizontal_align" : "center",
						"text_vertical_align" : "center",
					},
				
					{
						"name" : "button1",
						"type" : "button",
					
						"x" : -BUTTON_X,
						"y" : BUTTON_Y,
					
						"width" : BUTTON_WIDTH,
						"height" : BUTTON_HEIGHT,
					
						"horizontal_align" : "center",
						"text" : uiScriptLocale.YES,
					
						"default_image" : "d:/ymir work/ui/event/long_button_01.sub",
						"over_image"	: "d:/ymir work/ui/event/long_button_02.sub",
						"down_image"	: "d:/ymir work/ui/event/long_button_03.sub",
					},
				
					{
						"name" : "button2",
						"type" : "button",
					
						"x" : BUTTON_X,
						"y" : BUTTON_Y,
					
						"width" : BUTTON_WIDTH,
						"height" : BUTTON_HEIGHT,
					
						"horizontal_align" : "center",
						"text" : uiScriptLocale.NO,
					
						"default_image" : "d:/ymir work/ui/event/long_button_01.sub",
						"over_image"	: "d:/ymir work/ui/event/long_button_02.sub",
						"down_image"	: "d:/ymir work/ui/event/long_button_03.sub",
					},
				),
			},
		),
	}