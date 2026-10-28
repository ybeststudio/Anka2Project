import uiScriptLocale

PATTERN_PATH			= "d:/ymir work/ui/pattern/"
ROOT_PATH				= "d:/ymir work/ui/game/sungmahee_tower/"

WINDOW_WIDTH			= 348
WINDOW_HEIGHT			= 232

BOARD_WIDTH				= 348
BOARD_HEIGHT			= 232

PATTERN_WINDOW_WIDTH	= 326
PATTERN_WINDOW_HEIGHT	= 162

PATTERN_X_COUNT = (PATTERN_WINDOW_WIDTH - 32) / 16
PATTERN_Y_COUNT = (PATTERN_WINDOW_HEIGHT - 32) / 16


window = {
	"name" : "SungmaheeTowerEnter",
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
			"style" : ("attach", "ltr",),
			
			"x" : 0,
			"y" : 0,
			
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			
			"title" : uiScriptLocale.SUNGMAHEE_TOWER_ENTER_TITLE,
			
			"children" :
			(
				## base pattern
				{
					"name" : "base_pattern",
					"type" : "window",
					"style" : ("attach", "ltr",),
					
					"x" : 10,
					"y" : 33,
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
				
				## floor button area
				{
					"name" : "floor_list_window",
					"type" : "window",
					
					"style" : ("attach", "not_pick",),

					"x" : 15,
					"y" : 38,
					
					"width" : 126,
					"height" : 28*5 + 3*4,
				},
				
				## floor scroll bar
				{
					"name" : "floor_scrollbar",
					"type" : "scrollbar",

					"x" : 144,
					"y" : 37,
					"size" : 156,
				},
				
				## division line
				{
					"name" : "division_line_img",
					"type" : "image",
					
					"x" : 158,
					"y" : 35,
					
					"image" : ROOT_PATH + "division_line.sub",
				},
				
				## reward info title text window				
				{ 
					"name" : "reward_info_title_text_window", "type" : "window", "x" : 161, "y" : 36, "width" : 172, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"reward_info_title_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "menu_bg.sub"},
						{"name":"reward_info_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.SUNGMAHEE_TOWER_REWARD_INFO_TITLE, "all_align" : "center"},
					),	
				},
				
				## reward slot
				{
					"name" : "reward_slot",
					"type" : "slot",
			
					"x" : 167,
					"y" : 67,
			
					"width" : 160,
					"height" : 32,
					
					"image" : "d:/ymir work/ui/public/Slot_Base.sub",
			
					"slot" : (
						{"index":0, "x":32*0, "y":0, "width":32, "height":32},
						{"index":1, "x":32*1, "y":0, "width":32, "height":32},
						{"index":2, "x":32*2, "y":0, "width":32, "height":32},
						{"index":3, "x":32*3, "y":0, "width":32, "height":32},
						{"index":4, "x":32*4, "y":0, "width":32, "height":32},
					),
				},
				
				
				## my rank title text window				
				{ 
					"name" : "my_rank_title_text_window", "type" : "window", "x" : 161, "y" : 108, "width" : 172, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"my_rank_title_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "menu_bg.sub"},
						{"name":"my_rank_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.SUNGMAHEE_TOWER_MY_RANK_TITLE, "all_align" : "center"},
					),	
				},
				
				## my rank bg
				{
					"name" : "my_rank_bg",
					"type" : "image",
					
					"x" : 168,
					"y" : 136,
					
					"image" : ROOT_PATH + "my_rank_bg.sub",
				},
				## my rank text window				
				{ 
					"name" : "my_rank_text_window", "type" : "window", "x" : 172, "y" : 140, "width" : 150, "height" : 18, "style" : ("attach",),
					"children" :
					(						
						{"name":"my_rank_text", "type":"text", "x":0, "y":0, "text": "-", "all_align" : "center"},
					),	
				},
				## my rank time text window				
				{ 
					"name" : "my_rank_time_text_window", "type" : "window", "x" : 172, "y" : 164, "width" : 150, "height" : 18, "style" : ("attach",),
					"children" :
					(						
						{"name":"my_rank_time_text", "type":"text", "x":0, "y":0, "text": "-", "all_align" : "center"},
					),	
				},
			
				## Enter Button
				{
					"name" : "enter_button",
					"type" : "button",

					"x" : 248,
					"y" : 199,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.SUNGMAHEE_TOWER_ENTER_BUTTON,
				},
				
				## element text window
				{ 
					"name" : "element_text_window", "type" : "window", "x" : 11, "y" : 200, "width" : 105, "height" : 18,
					"children" :
					(
						{"name":"element_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "element_text_bg.sub", "style" : ("not_pick",), },
						{"name":"element_text", "type":"text", "x":0, "y":0, "text": "", "all_align" : "center", "style" : ("not_pick",), },
					),
				},
			),
		},
	),	
}
