import uiScriptLocale

DRAGON_GATE_PATH			= "d:/ymir work/ui/game/dragon_gate/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"

WINDOW_WIDTH				= 222
WINDOW_HEIGHT				= 434

MAIN_WINDOW_WIDTH			= 200
MAIN_WINDOW_HEIGHT			= 391

PATTERN_X_COUNT				= 10
PATTERN_Y_COUNT				= 22

TAB_BUTTON_X				= 15
TAB_BUTTON_Y				= 37
TAB_BUTTON_INTERVER_Y		= 32

window = {
	"name"	: "DragonGateSideWindow",
	"style" : ("float",),
	
	"x" : 0,
	"y" : 0,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT, 
	
	"children" :
	(
		## BOARD
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			
			"x" : 0,
			"y" : 0,
			
			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,
			
			"title" : uiScriptLocale.DRAGON_GATE_SIDE_NAME,
			
			"children" :
			(
				## main bg
				{
					"name" : "main_bg_window",
					"type" : "window",
					
					"style" : ("attach", "ltr",),

					"x" : 10,
					"y" : 32,
					
					"width" : MAIN_WINDOW_WIDTH,
					"height" : MAIN_WINDOW_HEIGHT,
					
					"children" :
					(
						## LeftTop 1
						{
							"name" : "left_top",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						## RightTop 2
						{
							"name" : "right_top",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "left_bottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "right_bottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						## topcenterImg 5
						{
							"name" : "top_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "left_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "right_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "bottom_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, PATTERN_Y_COUNT),
						},
					),
				},
				
				## tab button 0
				{
					"name" : "tab_button_0",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y,
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## tab button 1
				{
					"name" : "tab_button_1",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y + TAB_BUTTON_INTERVER_Y,
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## tab button 2
				{
					"name" : "tab_button_2",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y + (TAB_BUTTON_INTERVER_Y * 2),
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## tab button 3
				{
					"name" : "tab_button_3",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y + (TAB_BUTTON_INTERVER_Y * 3),
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## tab button 4
				{
					"name" : "tab_button_4",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y + (TAB_BUTTON_INTERVER_Y * 4),
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## tab button 5
				{
					"name" : "tab_button_5",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y + (TAB_BUTTON_INTERVER_Y * 5),
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## tab button 6
				{
					"name" : "tab_button_6",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y + (TAB_BUTTON_INTERVER_Y * 6),
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## tab button 7
				{
					"name" : "tab_button_7",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y + (TAB_BUTTON_INTERVER_Y * 7),
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## tab button 8
				{
					"name" : "tab_button_8",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y + (TAB_BUTTON_INTERVER_Y * 8),
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## tab button 9
				{
					"name" : "tab_button_9",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y + (TAB_BUTTON_INTERVER_Y * 9),
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## tab button 10
				{
					"name" : "tab_button_10",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y + (TAB_BUTTON_INTERVER_Y * 10),
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## tab button 11
				{
					"name" : "tab_button_11",
					"type" : "radio_button",
					
					"x" : TAB_BUTTON_X,
					"y" : TAB_BUTTON_Y + (TAB_BUTTON_INTERVER_Y * 11),
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_list_button_down.sub",
					
					"text" : "",
				},
				
				## scroll bar
				{
					"name" : "scroll_bar",
					"type" : "scrollbar",
					
					"x" : 30,
					"y" : 36,
					
					"size" : 383,
					
					"horizontal_align" : "right",
				},
			),
		},
	),
}