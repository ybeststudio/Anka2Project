import uiScriptLocale

PUBLIC_PATH		= "d:/ymir work/ui/public/"
PATTERN_PATH	= "d:/ymir work/ui/pattern/"
ROOT_PATH		= "d:/ymir work/ui/minigame/flower_event/"

WINDOW_WIDTH	= 336
WINDOW_HEIGHT	= 306

MAIN_WINDOW_WIDTH	= 314
MAIN_WINDOW_HEIGHT	= 262
MAIN_WINDOW_PATTERN_X_COUNT = (MAIN_WINDOW_WIDTH - 32) / 16
MAIN_WINDOW_PATTERN_Y_COUNT = (MAIN_WINDOW_HEIGHT - 32) / 16

window = {
	"name" : "WorldBossWindow",
	"style" : ("movable", "float", ),
	
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	
	"children" :
	[
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			"x"			: 0,
			"y"			: 0,
			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,
			"title"		: uiScriptLocale.FLOWER_EVENT_TITLE,
			
			"children" :
			[
				{
					"name"		: "main_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 10,
					"y"			: 32,
					
					"width"		: MAIN_WINDOW_WIDTH,
					"height"	: MAIN_WINDOW_HEIGHT,
					
					"children"	:
					[
						## LeftTop 1
						{
							"name" : "DescWindowLeftTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						
						## RightTop 2
						{
							"name" : "DescWindowRightTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						
						## LeftBottom 3
						{
							"name" : "DescWindowLeftBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						
						## RightBottom 4
						{
							"name" : "ListWindowRightBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						
						## topcenterImg 5
						{
							"name" : "DescWindowTopCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
						},
						
						## leftcenterImg 6
						{
							"name" : "DescWindowLeftCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, MAIN_WINDOW_PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "DescWindowRightCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, MAIN_WINDOW_PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "DescWindowBottomCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "DescWindowCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, MAIN_WINDOW_PATTERN_Y_COUNT),
						},
					],
				},
				
				{
					"name" : "slot_image",
					"type" : "image",
					"style" : ("ltr", ),
					"x" : 143,
					"y" : 38,
					
					"image" : ROOT_PATH + "slot.sub",
				},
				
				{
					"name" : "main_shoot_count_bg",
					"type" : "image",

					"x" : 193,
					"y" : 57,
					
					"image"		: ROOT_PATH + "input_bg.sub",
					
					"children" :
					[
						{
							"name" : "main_shoot_count_text",
							"type" : "text",

							"x" : 3,
							"y" : 3,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "0",
						},
					],
				},
				
				{
					"name"			: "main_exchange_button",
					"type"			: "button",
					"x"				: 124,
					"y"				: 91,
					"text"			: uiScriptLocale.FLOWER_EVENT_EXCHANGE_BUTTON_TEXT,
					"default_image"	: PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
				},
				
				{
					"name" : "down_arrow_image",
					"type" : "image",
					"style" : ("ltr", ),
					"x" : 152,
					"y" : 117,
					
					"image" : ROOT_PATH + "down_arrow.sub",
				},
				
				{
					"name" : "list_bg_image_1",
					"type" : "image",

					"x" : 15,
					"y" : 137,
					
					"width" : 304,
					"height" : 28,
					
					"image"		: ROOT_PATH + "list_bg.sub",
					
					"children" :
					[
						{
							"name" : "shoot_name_text_window_1",
							"type" : "window",

							"x" : 7,
							"y" : 8,
							
							"width" : 136,
							"height" : 11,
							
							"children" :
							[
								{
									"name" : "shoot_name_text_1",
									"type" : "text",

									"x" : 2,
									"y" : 0,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : uiScriptLocale.FLOWER_EVENT_SHOOT_CHRYSANTHEMUM_TEXT,
								},
							],
						},
					
						{
							"name" : "shoot_count_text_window_1",
							"type" : "window",

							"x" : 151,
							"y" : 8,
							
							"width" : 51,
							"height" : 11,
							
							"children" :
							[
								{
									"name" : "shoot_count_text_1",
									"type" : "text",

									"x" : 2,
									"y" : 0,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : "0",
								},
							],
						},
					],
				},
				
				{
					"name" : "list_bg_image_2",
					"type" : "image",

					"x" : 15,
					"y" : 168,
					
					"width" : 304,
					"height" : 28,
					
					"image"		: ROOT_PATH + "list_bg.sub",
					
					"children" :
					[
						{
							"name" : "shoot_name_text_window_2",
							"type" : "window",

							"x" : 7,
							"y" : 8,
							
							"width" : 136,
							"height" : 11,
							
							"children" :
							[
								{
									"name" : "shoot_name_text_2",
									"type" : "text",

									"x" : 2,
									"y" : 0,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : uiScriptLocale.FLOWER_EVENT_SHOOT_MAY_BELL_TEXT,
								},
							],
						},
					
						{
							"name" : "shoot_count_text_window_2",
							"type" : "window",

							"x" : 151,
							"y" : 8,
							
							"width" : 51,
							"height" : 11,
							
							"children" :
							[
								{
									"name" : "shoot_count_text_2",
									"type" : "text",

									"x" : 2,
									"y" : 0,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : "0",
								},
							],
						},
						
					],
				},
				
				{
					"name" : "list_bg_image_3",
					"type" : "image",

					"x" : 15,
					"y" : 199,
					
					"width" : 304,
					"height" : 28,
					
					"image"		: ROOT_PATH + "list_bg.sub",
					
					"children" :
					[
						{
							"name" : "shoot_name_text_window_3",
							"type" : "window",

							"x" : 7,
							"y" : 8,
							
							"width" : 136,
							"height" : 11,
							
							"children" :
							[
								{
									"name" : "shoot_name_text_3",
									"type" : "text",

									"x" : 2,
									"y" : 0,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : uiScriptLocale.FLOWER_EVENT_SHOOT_DAFFODIL_TEXT,
								},
							],
						},
					
						{
							"name" : "shoot_count_text_window_3",
							"type" : "window",

							"x" : 151,
							"y" : 8,
							
							"width" : 51,
							"height" : 11,
							
							"children" :
							[
								{
									"name" : "shoot_count_text_3",
									"type" : "text",

									"x" : 2,
									"y" : 0,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : "0",
								},
							],
						},
						
					],
				},
				
				{
					"name" : "list_bg_image_4",
					"type" : "image",

					"x" : 15,
					"y" : 230,
					
					"width" : 304,
					"height" : 28,
					
					"image"		: ROOT_PATH + "list_bg.sub",
					
					"children" :
					[
						{
							"name" : "shoot_name_text_window_4",
							"type" : "window",

							"x" : 7,
							"y" : 8,
							
							"width" : 136,
							"height" : 11,
							
							"children" :
							[
								{
									"name" : "shoot_name_text_4",
									"type" : "text",

									"x" : 2,
									"y" : 0,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : uiScriptLocale.FLOWER_EVENT_SHOOT_LILY_TEXT,
								},
							],
						},
					
						{
							"name" : "shoot_count_text_window_4",
							"type" : "window",

							"x" : 151,
							"y" : 8,
							
							"width" : 51,
							"height" : 11,
							
							"children" :
							[
								{
									"name" : "shoot_count_text_4",
									"type" : "text",

									"x" : 2,
									"y" : 0,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : "0",
								},
							],
						},
					],
				},
				
				{
					"name" : "list_bg_image_5",
					"type" : "image",

					"x" : 15,
					"y" : 261,
					
					"width" : 304,
					"height" : 28,
					
					"image"		: ROOT_PATH + "list_bg.sub",
					
					"children" :
					[
						{
							"name" : "shoot_name_text_window_5",
							"type" : "window",

							"x" : 7,
							"y" : 8,
							
							"width" : 136,
							"height" : 11,
							
							"children" :
							[
								{
									"name" : "shoot_name_text_5",
									"type" : "text",

									"x" : 2,
									"y" : 0,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : uiScriptLocale.FLOWER_EVENT_SHOOT_SUNFLOWER_TEXT,
								},
							],
						},
					
						{
							"name" : "shoot_count_text_window_5",
							"type" : "window",

							"x" : 151,
							"y" : 8,
							
							"width" : 51,
							"height" : 11,
							
							"children" :
							[
								{
									"name" : "shoot_count_text_5",
									"type" : "text",

									"x" : 2,
									"y" : 0,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",

									"text" : "0",
								},
							],
						},
					],
				},
				
				
				{
					"name"			: "shoot_list_exchange_button_1",
					"type"			: "button",
					"x"				: 226,
					"y"				: 141,
					"text"			: uiScriptLocale.FLOWER_EVENT_EXCHANGE_BUTTON_TEXT,
					"default_image"	: PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
				},
				
				{
					"name"			: "shoot_list_exchange_button_2",
					"type"			: "button",
					"x"				: 226,
					"y"				: 172,
					"text"			: uiScriptLocale.FLOWER_EVENT_EXCHANGE_BUTTON_TEXT,
					"default_image"	: PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
				},
				
				{
					"name"			: "shoot_list_exchange_button_3",
					"type"			: "button",
					"x"				: 226,
					"y"				: 203,
					"text"			: uiScriptLocale.FLOWER_EVENT_EXCHANGE_BUTTON_TEXT,
					"default_image"	: PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
				},
				
				{
					"name"			: "shoot_list_exchange_button_4",
					"type"			: "button",
					"x"				: 226,
					"y"				: 234,
					"text"			: uiScriptLocale.FLOWER_EVENT_EXCHANGE_BUTTON_TEXT,
					"default_image"	: PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
				},
				
				{
					"name"			: "shoot_list_exchange_button_5",
					"type"			: "button",
					"x"				: 226,
					"y"				: 265,
					"text"			: uiScriptLocale.FLOWER_EVENT_EXCHANGE_BUTTON_TEXT,
					"default_image"	: PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
				},
			],
		},
	],
}