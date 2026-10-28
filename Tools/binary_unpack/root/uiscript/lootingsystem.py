import uiScriptLocale

PATTERN_PATH			= "d:/ymir work/ui/pattern/"
ROOT_PATH				= "d:/ymir work/ui/game/looting/"

WINDOW_WIDTH			= 316
WINDOW_HEIGHT			= 494

PATTERN_WINDOW_WIDTH	= 294
PATTERN_WINDOW_HEIGHT	= 390

PATTERN_X_COUNT = (PATTERN_WINDOW_WIDTH - 32) / 16
PATTERN_Y_COUNT = (PATTERN_WINDOW_HEIGHT - 32) / 16


window = {
	"name" : "LootingSystem",
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
			
			"title" : uiScriptLocale.LOOTING_SYSTEM_TITLE,
			
			"children" :
			(
				## base pattern
				{
					"name" : "base_pattern",
					"type" : "window",
					"style" : ("attach", "ltr",),
					
					"x" : 10,
					"y" : 32,
					"width" : PATTERN_WINDOW_WIDTH,
					"height" :PATTERN_WINDOW_HEIGHT,
					
					"children" :
					(
						## LeftTop 1
						{
							"name" : "pattern_left_top_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						## RightTop 2
						{
							"name" : "pattern_right_top_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "pattern_left_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "pattern_right_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						## topcenterImg 5
						{
							"name" : "pattern_top_cetner_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "pattern_left_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "pattern_right_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "pattern_bottom_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "pattern_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, PATTERN_Y_COUNT),
						},
					),
				},
				
				## object in pattern window
				{
					"name" : "object_area_window",
					"type" : "window",
					"style" : ("attach",),
					
					"x" : 10+5,
					"y" : 32+5,
					"width"		: 270,
					"height"	: 380,
				},

				## scroll bar
				{
					"name" : "scroll_bar",
					"type" : "scrollbar",

					"x" : 286,
					"y" : 36,
					"size" : 380,
				},
				
				## Init Button
				{
					"name" : "init_button",
					"type" : "button",

					"x" : 11,
					"y" : 428,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.LOOTING_SYSTEM_INIT,
				},
				
				## Save Button
				{
					"name" : "save_button",
					"type" : "button",

					"x" : 217,
					"y" : 428,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.LOOTING_SYSTEM_SAVE,
				},

				## message
				{
					"name" : "message_window", "type" : "window", "x" : 18, "y" : 457, "width" : 279, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{ "name" : "message_bg",	"type" : "image", "style" : ("ltr", "attach", ), "x" : 0, "y" : 0, "image" : ROOT_PATH + "message_bg.sub", },
						{ "name" : "message_text",	"type" : "text", "x":0, "y":0, "text": "", "all_align" : "center" },
					),	
				},
			),
		},
	),	
}
