import uiScriptLocale

PUBLIC_PATH		= "d:/ymir work/ui/public/"
PATTERN_PATH	= "d:/ymir work/ui/pattern/"
ROOT_PATH		= "d:/ymir work/ui/minigame/world_boss/"

WINDOW_WIDTH	= 400
WINDOW_HEIGHT	= 374

BOARD_WIDTH		= 378
BOARD_HEIGHT	= 300

DESC_WINDOW_WIDTH	= 378
DESC_WINDOW_HEIGHT	= 300
DESC_WINDOW_PATTERN_X_COUNT = (DESC_WINDOW_WIDTH - 32) / 16
DESC_WINDOW_PATTERN_Y_COUNT = (DESC_WINDOW_HEIGHT - 32) / 16

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
			"title"		: uiScriptLocale.WORLD_BOSS_TITLE,
			
			"children" :
			[
				{
					"name"		: "desc_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 10,
					"y"			: 32,
					
					"width"		: DESC_WINDOW_WIDTH,
					"height"	: DESC_WINDOW_HEIGHT,
					
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
							
							"x" : DESC_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						
						## LeftBottom 3
						{
							"name" : "DescWindowLeftBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : DESC_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						
						## RightBottom 4
						{
							"name" : "ListWindowRightBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : DESC_WINDOW_WIDTH - 16,
							"y" : DESC_WINDOW_HEIGHT - 16,
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
							"rect" : (0.0, 0.0, DESC_WINDOW_PATTERN_X_COUNT, 0),
						},
						
						## leftcenterImg 6
						{
							"name" : "DescWindowLeftCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, DESC_WINDOW_PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "DescWindowRightCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : DESC_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, DESC_WINDOW_PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "DescWindowBottomCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : DESC_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, DESC_WINDOW_PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "DescWindowCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, DESC_WINDOW_PATTERN_X_COUNT, DESC_WINDOW_PATTERN_Y_COUNT),
						},
					],
				},
				
				# 진짜 애들
				{
					"name"			: "desc_board",
					"type"			: "bar",
					"x"				: 17,
					"y"				: 38,
					"width"			: 364,
					"height"		: 186,
				},
				
				{
					"name" : "current_state_text",
					"type" : "text",

					"x" : 30,
					"y" : 229,

					"horizontal_align" : "left",
					"text_horizontal_align" : "left",

					"text" : "",
				},
				
				{
					"name"			: "prev_button",
					"type"			: "button",
					"x"				: 329,
					"y"				: 229,
					"default_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
				},
				
				{
					"name"			: "next_button",
					"type"			: "button",
					"x"				: 360,
					"y"				: 229,
					"default_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
				},
				
				{
					"name"			: "dividing_line_image",
					"type"			: "image",
					"style"			: ("ltr", ),
					"x"				: 10,
					"y"				: 245,
					
					"image"			: ROOT_PATH + "dividing_line.sub",
				},
				
				# 보스유지시간
				{
					"name" : "run_time_label_text_window",
					"type" : "window",

					"x" : 10,
					"y" : 258,
					
					"width"		: 270,
					"height"	: 16,
					
					"children" :
					[
						{
							"name" : "run_time_label_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : uiScriptLocale.WORLD_BOSS_RUN_TIME_TEXT,
						},
					],
				},

				{
					"name" : "run_time_text_bg",
					"type" : "image",
					
					"x" : 291,
					"y" : 256,
					
					"width"		: 88,
					"height"	: 16,
					
					"image"		: ROOT_PATH + "value_text_bg.sub",
					
					"children" :
					[
						{
							"name" : "run_time_text",
							"type" : "text",

							"x" : 4,
							"y" : 2,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "",
						},
					],
				},
					
				# 휴식시간	
				{
					"name" : "break_time_label_text_window",
					"type" : "window",

					"x" : 10,
					"y" : 276,
					
					"width"		: 270,
					"height"	: 16,
					
					"children" :
					[
						{
							"name" : "break_time_label_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : uiScriptLocale.WORLD_BOSS_BREAK_TIME_TEXT,
						},
					],
				},

				{
					"name" : "break_time_text_bg",
					"type" : "image",

					"x" : 291,
					"y" : 274,
					
					"width"		: 88,
					"height"	: 16,
					
					"image"		: ROOT_PATH + "value_text_bg.sub",
					
					"children" :
					[
						{
							"name" : "break_time_text",
							"type" : "text",

							"x" : 4,
							"y" : 2,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "04:00",
						},
					],
				},		
				
				# 누적데미지
				{
					"name" : "total_damage_label_text_window",
					"type" : "window",

					"x" : 10,
					"y" : 294,
					
					"width"		: 270,
					"height"	: 16,
					
					"children" :
					[
						{
							"name" : "total_damage_label_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : uiScriptLocale.WORLD_BOSS_TOTAL_DAMAGE_TEXT,
						},
					],
				},

				{
					"name" : "total_damage_text_bg",
					"type" : "image",

					"x" : 291,
					"y" : 292,
					
					"width"		: 88,
					"height"	: 16,
					
					"image"		: ROOT_PATH + "value_text_bg.sub",
					
					"children" :
					[
						{
							"name" : "total_damage_text",
							"type" : "text",

							"x" : 4,
							"y" : 2,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "0000",
						},
					],
				},	
			
				# 보상기준데미지
				{
					"name" : "min_damage_label_text_window",
					"type" : "window",

					"x" : 10,
					"y" : 312,
					
					"width"		: 270,
					"height"	: 16,
					
					"children" :
					[
						{
							"name" : "min_damage_label_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : uiScriptLocale.WORLD_BOSS_MIN_DAMAGE_TEXT,
						},
					],
				},

				{
					"name" : "min_damage_text_bg",
					"type" : "image",

					"x" : 291,
					"y" : 310,
					
					"width"		: 88,
					"height"	: 16,
					
					"image"		: ROOT_PATH + "value_text_bg.sub",
					
					"children" :
					[
						{
							"name" : "min_damage_text",
							"type" : "text",

							"x" : 4,
							"y" : 2,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "100000",
						},
					],
				},	
				
				#버튼
				{
					"name"			: "ranking_button",
					"type"			: "button",
					"x"				: 23,
					"y"				: 339,
					"text"			: uiScriptLocale.WORLD_BOSS_RANKING_BUTTON,
					"default_image"	: PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
				},
				
				{
					"name"			: "reward_button",
					"type"			: "button",
					"x"				: 287,
					"y"				: 339,
					"text"			: uiScriptLocale.WORLD_BUSS_REWARD_BUTTON,
					"default_image"	: PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
				},
			],
		},
	],
}