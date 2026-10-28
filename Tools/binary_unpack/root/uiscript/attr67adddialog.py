import uiScriptLocale

PATTERN_PATH = "d:/ymir work/ui/pattern/"
ROOT = "d:/ymir work/ui/game/attr6th7th/"

WINDOW_WIDTH			= 312
WINDOW_HEIGHT			= 224

REGIST_WINDOW_WIDTH		= 144
REGIST_WINDOW_HEIGHT	= 156
REGIST_WINDOW_PATTERN_X_COUNT = ((REGIST_WINDOW_WIDTH - 32) / 16)-1
REGIST_WINDOW_PATTERN_Y_COUNT = (REGIST_WINDOW_HEIGHT - 32) / 16

PROCESS_WINDOW_WIDTH	= 144
PROCESS_WINDOW_HEIGHT	= 156
PROCESS_WINDOW_PATTERN_X_COUNT = ((PROCESS_WINDOW_WIDTH - 32) / 16)-1
PROCESS_WINDOW_PATTERN_Y_COUNT = (PROCESS_WINDOW_HEIGHT - 32) / 16


window = {
	"name" : "Attr67AddWindow",
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

			"x" : 0,
			"y" : 0,

			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			
			"title" : uiScriptLocale.ATTR_67_ADD_TITLE,
		},
		
		## regist window
		{
			"name" : "regist_window",
			"type" : "window",
			
			"style" : ("attach", "ltr",),

			"x" : 10,
			"y" : 32,
			
			"width" : REGIST_WINDOW_WIDTH,
			"height" : REGIST_WINDOW_HEIGHT,
			
			"children" :
			(
				## LeftTop 1
				{
					"name" : "RegistWindowLeftTop",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_left_top.tga",
				},
				## RightTop 2
				{
					"name" : "RegistWindowRightTop",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : REGIST_WINDOW_WIDTH - 16,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_right_top.tga",
				},
				## LeftBottom 3
				{
					"name" : "RegistWindowLeftBottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : REGIST_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_left_bottom.tga",
				},
				## RightBottom 4
				{
					"name" : "RegistWindowRightBottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : REGIST_WINDOW_WIDTH - 16,
					"y" : REGIST_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_right_bottom.tga",
				},
				## topcenterImg 5
				{
					"name" : "RegistWindowTopCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_top.tga",
					"rect" : (0.0, 0.0, REGIST_WINDOW_PATTERN_X_COUNT, 0),
				},
				## leftcenterImg 6
				{
					"name" : "RegistWindowLeftCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_left.tga",
					"rect" : (0.0, 0.0, 0, REGIST_WINDOW_PATTERN_Y_COUNT),
				},
				## rightcenterImg 7
				{
					"name" : "RegistWindowRightCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : REGIST_WINDOW_WIDTH - 16,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_right.tga",
					"rect" : (0.0, 0.0, 0, REGIST_WINDOW_PATTERN_Y_COUNT),
				},
				## bottomcenterImg 8
				{
					"name" : "RegistWindowBottomCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : REGIST_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_bottom.tga",
					"rect" : (0.0, 0.0, REGIST_WINDOW_PATTERN_X_COUNT, 0),
				},
				## centerImg
				{
					"name" : "RegistWindowCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_center.tga",
					"rect" : (0.0, 0.0, REGIST_WINDOW_PATTERN_X_COUNT, REGIST_WINDOW_PATTERN_Y_COUNT),
				},
				
				## regist slot text
				{ 
					"name" : "regist_slot_text_window", "type" : "window", "x" : 13 - 10, "y" : 35 - 32, "width" : 138, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"regist_slot_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT + "memu_text.sub"},
						{"name":"regist_slot_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.ATTR_67_REGIST_SLOT_TITLE, "all_align" : "center", "color" : 0xFFD8CAC2, },
					),	
				},
				
				## regist slot img
				{
					"name" : "regist_slot_img",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 60-10,
					"y" : 68-32,
					
					"image" : ROOT + "regist_slot.sub",
					
					"children" :
					(
						## regist slot
						{
								"name" : "regist_slot",
								"type" : "slot",

								"x" : 0,
								"y" : 0,

								"width" : 44,
								"height" : 107,
								
								"slot" : ( {"index":0, "x":6, "y":6, "width":32, "height":96}, )
						},
					),
				},
			),
		},
		
		## process window
		{
			"name" : "process_window",
			"type" : "window",
			
			"style" : ("attach", "ltr",),

			"x" : 157,
			"y" : 32,
			
			"width" : PROCESS_WINDOW_WIDTH,
			"height" : PROCESS_WINDOW_HEIGHT,
			
			"children" :
			(
				## LeftTop 1
				{
					"name" : "ProcessWindowLeftTop",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_left_top.tga",
				},
				## RightTop 2
				{
					"name" : "ProcessWindowRightTop",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : PROCESS_WINDOW_WIDTH - 16,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_right_top.tga",
				},
				## LeftBottom 3
				{
					"name" : "ProcessWindowLeftBottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : PROCESS_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_left_bottom.tga",
				},
				## RightBottom 4
				{
					"name" : "ProcessWindowRightBottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : PROCESS_WINDOW_WIDTH - 16,
					"y" : PROCESS_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_right_bottom.tga",
				},
				## topcenterImg 5
				{
					"name" : "ProcessWindowTopCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 0,
					"image" : PATTERN_PATH + "border_A_top.tga",
					"rect" : (0.0, 0.0, PROCESS_WINDOW_PATTERN_X_COUNT, 0),
				},
				## leftcenterImg 6
				{
					"name" : "ProcessWindowLeftCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_left.tga",
					"rect" : (0.0, 0.0, 0, PROCESS_WINDOW_PATTERN_Y_COUNT),
				},
				## rightcenterImg 7
				{
					"name" : "ProcessWindowRightCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : PROCESS_WINDOW_WIDTH - 16,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_right.tga",
					"rect" : (0.0, 0.0, 0, PROCESS_WINDOW_PATTERN_Y_COUNT),
				},
				## bottomcenterImg 8
				{
					"name" : "ProcessWindowBottomCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : PROCESS_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_bottom.tga",
					"rect" : (0.0, 0.0, PROCESS_WINDOW_PATTERN_X_COUNT, 0),
				},
				## centerImg
				{
					"name" : "ProcessWindowCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_center.tga",
					"rect" : (0.0, 0.0, PROCESS_WINDOW_PATTERN_X_COUNT, PROCESS_WINDOW_PATTERN_Y_COUNT),
				},
				
				## material slot text
				{ 
					"name" : "material_slot_text_window", "type" : "window", "x" : 160 - 157, "y" : 35 - 32, "width" : 138, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"material_slot_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT + "memu_text.sub"},
						{"name":"material_slot_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.ATTR_67_MATERIAL_SLOT_TITLE, "all_align" : "center", "color" : 0xFFD8CAC2, },
					),	
				},
				## material slot bg
				{
					"name" : "material_slot_bg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 177 - 157,
					"y" : 59 - 32,
					
					"image" : ROOT + "material_slot.sub",
				},
				## material slot
				{
					"name" : "material_slot",
					"type" : "slot",
					
					"x" : 185 - 157,
					"y" : 67 - 32,
					
					"width" : 32,
					"height" : 32,
					
					"slot" : ( {"index":0, "x":0, "y":0, "width":32, "height":32}, )
				},
				## material slot count text window
				{ 
					"name" : "material_slot_count_text_window", "type" : "window", "x" : 233 - 157, "y" : 74 - 32, "width" : 26, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{"name":"material_slot_count_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT + "material_count_text.sub"},
						{"name":"material_slot_count_text", "type":"text", "x":0, "y":0, "text": "0", "all_align" : "center"},
					),	
				},
				## material slot arrow up button
				{
					"name" : "material_slot_arrow_up_button",
					"type" : "button",
					
					"x" : 259 - 157,
					"y" : 66 - 32,
					
					"default_image" : ROOT + "arrow_up_default.sub",
					"over_image" : ROOT + "arrow_up_over.sub",
					"down_image" : ROOT + "arrow_up_down.sub",
				},
				## material slot arrow down button
				{
					"name" : "material_slot_arrow_down_button",
					"type" : "button",
					
					"x" : 259 - 157,
					"y" : 83 - 32,
					
					"default_image" : ROOT + "arrow_down_default.sub",
					"over_image" : ROOT + "arrow_down_over.sub",
					"down_image" : ROOT + "arrow_down_down.sub",
				},
				
				## support slot text
				{ 
					"name" : "support_slot_text_window", "type" : "window", "x" : 160 - 157, "y" : 111 - 32, "width" : 138, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"support_slot_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT + "memu_text.sub"},
						{"name":"support_slot_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.ATTR_67_SUPPORT_SLOT_TITLE, "all_align" : "center", "color" : 0xFFD8CAC2, },
					),	
				},
				## success slot bg
				{
					"name" : "support_slot_bg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 177 - 157,
					"y" : 135 - 32,
					
					"image" : ROOT + "material_slot.sub",
				},
				## success slot
				{
					"name" : "support_slot",
					"type" : "slot",

					"x" : 185 - 157,
					"y" : 143 - 32,

					"width" : 32,
					"height" : 32,
					
					"slot" : ( {"index":0, "x":0, "y":0, "width":32, "height":32}, )
				},
				
				## support slot count window
				{ 
					"name" : "support_slot_count_text_window", "type" : "window", "x" : 233 - 157, "y" : 150 - 32, "width" : 26, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{"name":"support_slot_count_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT + "material_count_text.sub"},
						{"name":"support_slot_count_text", "type":"text", "x":0, "y":0, "text": "0", "all_align" : "center"},
					),	
				},
				
				## support slot arrow up button
				{
					"name" : "support_slot_arrow_up_button",
					"type" : "button",
					
					"x" : 259 - 157,
					"y" : 142 - 32,
					
					"default_image" : ROOT + "arrow_up_default.sub",
					"over_image" : ROOT + "arrow_up_over.sub",
					"down_image" : ROOT + "arrow_up_down.sub",
				},
				## support slot arrow down button
				{
					"name" : "support_slot_arrow_down_button",
					"type" : "button",
					
					"x" : 259 - 157,
					"y" : 159 - 32,
					
					"default_image" : ROOT + "arrow_down_default.sub",
					"over_image" : ROOT + "arrow_down_over.sub",
					"down_image" : ROOT + "arrow_down_down.sub",
				},
			),
		},
		
		## total success percent text window
		{
			"name" : "total_success_percent_text_window",
			"type" : "window",
			"style" : ("attach", "ltr",),

			"x" : 14,
			"y" : 192,

			"width" : 184,
			"height" : 18,
			
			"children" :
			(
				{
					"name" : "TotalSuccessWindowLeftImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 0,
					"image" : PATTERN_PATH + "border_c_left.tga",
				},
				{
					"name" : "TotalSuccessWindowCenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 21,
					"y" : 0,
					"image" : PATTERN_PATH + "border_c_middle.tga",
					"rect" : (0.0, 0.0, 6, 0),
				},
				{
					"name" : "TotalSuccessWindowRightImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 184-21,
					"y" : 0,
					"image" : PATTERN_PATH + "border_c_right.tga",
				},
				## Text
				{
					"name" : "TotalSuccessText",
					"type" : "text",
					"x" : 0,
					"y" : 2,
					"all_align" : "center",
					"text" : "",
				},
			),
		},
		
		## attr add button
		{
			"name" : "attr_add_button",
			"type" : "button",
			
			"x" : 209,
			"y" : 193,
			
			"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
			
			"text" : uiScriptLocale.ATTR_67_ADD,
		},
		
		## question
		{
			"name" : "question_button",
			"type" : "button",

			"x" : WINDOW_WIDTH - 30 - 16,
			"y" : 9,

			"default_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
			"over_image" : "d:/ymir work/ui/pattern/q_mark_02.tga",
			"down_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
		},
	),
}