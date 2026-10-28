import uiScriptLocale
import app

PUBLIC_PATH							= "d:/ymir work/ui/public/"
PATTERN_PATH						= "d:/ymir work/ui/pattern/"
ROOT_PATH							= "d:/ymir work/ui/game/golden_land/find_golden_fruit/"

WINDOW_WIDTH						= 372 -1
WINDOW_HEIGHT						= 268 -1

LEFT_MAIN_WINDOW_WIDTH				= 60
LEFT_MAIN_WINDOW_HEIGHT				= 224
LEFT_MAIN_WINDOW_PATTERN_X_COUNT	= (LEFT_MAIN_WINDOW_WIDTH - 32) / 16
LEFT_MAIN_WINDOW_PATTERN_Y_COUNT	= (LEFT_MAIN_WINDOW_HEIGHT - 32) / 16 - 1

RIGHT_MAIN_WINDOW_WIDTH				= 288
RIGHT_MAIN_WINDOW_HEIGHT			= 224
RIGHT_MAIN_WINDOW_PATTERN_X_COUNT	= (RIGHT_MAIN_WINDOW_WIDTH - 32) / 16 - 1
RIGHT_MAIN_WINDOW_PATTERN_Y_COUNT	= (RIGHT_MAIN_WINDOW_HEIGHT - 32) / 16 - 1

if app.ENABLE_GOLDEN_LAND_VER2:
	window = {
		"name"		: "FindGoldenFruitWindow",
		"style"		: ("float", "movable", "ltr", ),
	
		"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
		"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
		"width"		: WINDOW_WIDTH,
		"height"	: WINDOW_HEIGHT,
	
		"children" :
		(
			{
				"name"		: "board",
				"type"		: "board_with_titlebar",
				"style"		: ("ltr", ),

				"x"			: 0,
				"y"			: 0,
			
				"width"		: WINDOW_WIDTH,
				"height"	: WINDOW_HEIGHT,
			
				## 황금의땅
				"title"		: uiScriptLocale.GOLDEN_LAND_FIND_GOLDEN_FRUIT_TITLE,

				"children" :
				(
					## background
					{
						"name"		: "left_main_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: 10,
						"y"			: 32,

						"width"		: LEFT_MAIN_WINDOW_WIDTH,
						"height"	: LEFT_MAIN_WINDOW_HEIGHT,

						"children" :
						(
							## LeftTop 1
							{
								"name" : "bg_left_top",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_left_top.tga",
							},
						
							## RightTop 2
							{
								"name" : "bg_right_top",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : LEFT_MAIN_WINDOW_WIDTH - 16,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_right_top.tga",
							},
						
							## LeftBottom 3
							{
								"name" : "bg_left_bottom",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : LEFT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_left_bottom.tga",
							},
						
							## RightBottom 4
							{
								"name" : "bg_right_bottom",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : LEFT_MAIN_WINDOW_WIDTH - 16,
								"y" : LEFT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_right_bottom.tga",
							},
						
							## topcenterImg 5
							{
								"name" : "bg_center_top",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_top.tga",
								"rect" : (0.0, 0.0, LEFT_MAIN_WINDOW_PATTERN_X_COUNT, 0),
							},
						
							## leftcenterImg 6
							{
								"name" : "bg_center_left",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_left.tga",
								"rect" : (0.0, 0.0, 0, LEFT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
							## rightcenterImg 7
							{
								"name" : "bg_center_right",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : LEFT_MAIN_WINDOW_WIDTH - 16,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_right.tga",
								"rect" : (0.0, 0.0, 0, LEFT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
							## bottomcenterImg 8
							{
								"name" : "bg_center_bottom",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : LEFT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_bottom.tga",
								"rect" : (0.0, 0.0, LEFT_MAIN_WINDOW_PATTERN_X_COUNT, 0),
							},
							## centerImg
							{
								"name" : "bg_center",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_center.tga",
								"rect" : (0.0, 0.0, LEFT_MAIN_WINDOW_PATTERN_X_COUNT, LEFT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
						),
					},

					## background
					{
						"name"		: "right_main_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: 72,
						"y"			: 32,

						"width"		: RIGHT_MAIN_WINDOW_WIDTH,
						"height"	: RIGHT_MAIN_WINDOW_HEIGHT,

						"children" :
						(
							## LeftTop 1
							{
								"name" : "bg_left_top",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_left_top.tga",
							},
						
							## RightTop 2
							{
								"name" : "bg_right_top",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : RIGHT_MAIN_WINDOW_WIDTH - 16,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_right_top.tga",
							},
						
							## LeftBottom 3
							{
								"name" : "bg_left_bottom",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : RIGHT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_left_bottom.tga",
							},
						
							## RightBottom 4
							{
								"name" : "bg_right_bottom",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : RIGHT_MAIN_WINDOW_WIDTH - 16,
								"y" : RIGHT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_right_bottom.tga",
							},
						
							## topcenterImg 5
							{
								"name" : "bg_center_top",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_top.tga",
								"rect" : (0.0, 0.0, RIGHT_MAIN_WINDOW_PATTERN_X_COUNT, 0),
							},
						
							## leftcenterImg 6
							{
								"name" : "bg_center_left",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_left.tga",
								"rect" : (0.0, 0.0, 0, RIGHT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
							## rightcenterImg 7
							{
								"name" : "bg_center_right",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : RIGHT_MAIN_WINDOW_WIDTH - 16,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_right.tga",
								"rect" : (0.0, 0.0, 0, RIGHT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
							## bottomcenterImg 8
							{
								"name" : "bg_center_bottom",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : RIGHT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_bottom.tga",
								"rect" : (0.0, 0.0, RIGHT_MAIN_WINDOW_PATTERN_X_COUNT, 0),
							},
							## centerImg
							{
								"name" : "bg_center",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_center.tga",
								"rect" : (0.0, 0.0, RIGHT_MAIN_WINDOW_PATTERN_X_COUNT, RIGHT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
						),
					},
				),
			},

			{
				"name" : "reward_text_bg",
				"type" : "image",
				"style"	: ("ltr", ),

				"x" : 13,
				"y" : 35,

				"width" : 54,
				"height" : 41,
					
				"image"		: ROOT_PATH + "reward_text_bg.sub",	

				"children":
				(
					{
						"name" : "reward_text",
						"type" : "text",

						"x" : 0,
						"y" : 0,

						"horizontal_align" : "center",
						"text_horizontal_align" : "center",

						"vertical_align" : "center",
						"text_vertical_align" : "center",

						"text" : uiScriptLocale.GOLDEN_LAND_FIND_GOLDEN_FRUIT_REWARD,
					},
				),
			},

			{
				"name" : "reward_slot_bg",
				"type" : "image",

				"x" : 15,
				"y" : 89,

				"width" : 50,
				"height" : 115,
					
				"image"		: ROOT_PATH + "reward_slot_bg.sub",	
			},

			{
				"name"			: "display_other_reward_button",
				"type"			: "button",

				"x"				: 30,
				"y"				: 215,

				"default_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
				"over_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_02.sub",
				"down_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
			},

			{
				"name" : "find_card_text_bg",
				"type" : "image",

				"x" : 75,
				"y" : 35,

				"width" : 282,
				"height" : 21,
					
				"image"		: ROOT_PATH + "find_card_text_bg.sub",

				"children":
				(
					{
						"name" : "find_card_text",
						"type" : "text",

						"x" : 0,
						"y" : 0,

						"horizontal_align" : "center",
						"text_horizontal_align" : "center",

						"vertical_align" : "center",
						"text_vertical_align" : "center",

						"text" : uiScriptLocale.GOLDEN_LAND_FIND_GOLDEN_FRUIT_FIND_CARD,
					},
				),
			},

			{
				"name" : "card_window_bg",
				"type" : "image",

				"x" : 79,
				"y" : 61,
					
				"image"	: ROOT_PATH + "card_window_bg.sub",	
			},

			#카드 슬롯
			{
				"name" : "card_slot", "type" : "slot", "x" : 84, "y" : 66, "width" : 264, "height" : 132,
				"slot" :
				(
					# line 1
					{"index":0, "x":84-84, "y":66-66, "width":33, "height":33},
					{"index":1, "x":117-84, "y":66-66, "width":33, "height":33},
					{"index":2, "x":150-84, "y":66-66, "width":33, "height":33},
					{"index":3, "x":183-84, "y":66-66, "width":33, "height":33},
					{"index":4, "x":216-84, "y":66-66, "width":33, "height":33},
					{"index":5, "x":249-84, "y":66-66, "width":33, "height":33},
					{"index":6, "x":282-84, "y":66-66, "width":33, "height":33},
					{"index":7, "x":315-84, "y":66-66, "width":33, "height":33},
				
					# line 2
					{"index":8, "x":84-84, "y":99-66, "width":33, "height":33},
					{"index":9, "x":117-84, "y":99-66, "width":33, "height":33},
					{"index":10, "x":150-84, "y":99-66, "width":33, "height":33},
					{"index":11, "x":183-84, "y":99-66, "width":33, "height":33},
					{"index":12, "x":216-84, "y":99-66, "width":33, "height":33},
					{"index":13, "x":249-84, "y":99-66, "width":33, "height":33},
					{"index":14, "x":282-84, "y":99-66, "width":33, "height":33},
					{"index":15, "x":315-84, "y":99-66, "width":33, "height":33},

					# line 3
					{"index":16, "x":84-84, "y":132-66, "width":33, "height":33},
					{"index":17, "x":117-84, "y":132-66, "width":33, "height":33},
					{"index":18, "x":150-84, "y":132-66, "width":33, "height":33},
					{"index":19, "x":183-84, "y":132-66, "width":33, "height":33},
					{"index":20, "x":216-84, "y":132-66, "width":33, "height":33},
					{"index":21, "x":249-84, "y":132-66, "width":33, "height":33},
					{"index":22, "x":282-84, "y":132-66, "width":33, "height":33},
					{"index":23, "x":315-84, "y":132-66, "width":33, "height":33},

					# line 4
					{"index":24, "x":84-84, "y":165-66, "width":33, "height":33},
					{"index":25, "x":117-84, "y":165-66, "width":33, "height":33},
					{"index":26, "x":150-84, "y":165-66, "width":33, "height":33},
					{"index":27, "x":183-84, "y":165-66, "width":33, "height":33},
					{"index":28, "x":216-84, "y":165-66, "width":33, "height":33},
					{"index":29, "x":249-84, "y":165-66, "width":33, "height":33},
					{"index":30, "x":282-84, "y":165-66, "width":33, "height":33},
					{"index":31, "x":315-84, "y":165-66, "width":33, "height":33},
				),	
			},

			# 필요 노란열매 개수
			{
				"name" : "need_yellow_fruit_window",
				"type" : "window",

				"x" : 79,
				"y" : 213,

				"width"		: 273,
				"height"	: 11,

				"children" :
				(
					{
						"name" : "increase_probability_text_window",
						"type" : "window",

						"x" : 0,
						"y" : 0,
					
						"width"		: 36,
						"height"	: 11,
					
						"children" :
						(
							{
								"name" : "increase_probability_text",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"horizontal_align" : "left",
								"text_horizontal_align" : "left",

								"vertical_align" : "center",
								"text_vertical_align" : "center",

								# 확률업
								"text" : uiScriptLocale.GOLDEN_LAND_FIND_GOLDEN_FRUIT_INCREASE_PROBABILITY,
							},
						),
					},

					{
						"name" : "yellow_fruit_need_count_fruit_img",
						"type" : "image",

						"x" : 294 - 79,
						"y" : -3,
					
						"image"	 : "d:/ymir work/ui/game/golden_land/yellow_fruit_small.sub",
					},

					{
						"name" : "yellow_fruit_need_count_bg",
						"type" : "image",

						"x" : 314 - 79,
						"y" : -4,

						"width" : 37,
						"height" : 18,
					
						"image"		: ROOT_PATH + "yellow_fruit_count_bg.sub",

						"children":
						(
							{
								"name" : "yellow_fruit_need_count_text",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"horizontal_align" : "center",
								"text_horizontal_align" : "center",

								"vertical_align" : "center",
								"text_vertical_align" : "center",

								"text" : "9999",
							},
						),
					},
				),
			},

			# 보유 노란열매 개수 윈도우
			{
				"name" : "yellow_fruit_window",
				"type" : "window",

				"x" : 79,
				"y" : 234,

				"width"		: 273,
				"height"	: 11,

				"children" :
				(
					{
						"name" : "increase_probability_arrow_img",
						"type" : "image",

						"x" : 0,
						"y" : -1,
					
						"image"	 : ROOT_PATH + "increase_probability_arrow.sub",
					},

					{
						"name" : "increase_probability_count_bg",
						"type" : "image",

						"x" : 0 + 13,
						"y" : -4,

						"width" : 29,
						"height" : 18,
					
						"image"		: ROOT_PATH + "increase_probability_count_bg.sub",

						"children":
						(
							{
								"name" : "increase_probability_count_text",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"horizontal_align" : "center",
								"text_horizontal_align" : "center",

								"vertical_align" : "center",
								"text_vertical_align" : "center",

								"text" : "999",
							},
						),
					},

					{
						"name" : "increase_probability_help_button",
						"type" : "button",

						"x" : 0 + 13 + 30,
						"y" : -7,
					
						"default_image"	: ROOT_PATH + "increase_probability_help_button_default.sub", 
						"over_image"	: ROOT_PATH + "increase_probability_help_button_over.sub",
						"down_image"	: ROOT_PATH + "increase_probability_help_button_down.sub",
					},

					{
						"name" : "yellow_fruit_count_fruit_img",
						"type" : "image",

						"x" : 294 - 79,
						"y" : -3,
					
						"image"		: "d:/ymir work/ui/game/golden_land/yellow_fruit_small.sub",
					},

					{
						"name" : "yellow_fruit_count_bg",
						"type" : "image",

						"x" : 314 - 79,
						"y" : -4,
					
						"width" : 37,
						"height" : 18,

						"image"		: ROOT_PATH + "yellow_fruit_count_bg.sub",

						"children":
						(
							{
								"name" : "yellow_fruit_count_text",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"horizontal_align" : "center",
								"text_horizontal_align" : "center",

								"vertical_align" : "center",
								"text_vertical_align" : "center",

								"text" : "1234",
							},
						),
					},
				),
			}, 

			# Success Effect Animation 1
			{
				"name" : "success_firework_effect_1",
				"type" : "ani_image",
					
				"x" : 97,
				"y" : 60,
					
				"delay" : 6,

				"images" :
				(
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff1.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff2.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff3.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff4.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff6.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff7.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff8.sub",
				),
			},
		
			# Success Effect Animation 2
			{
				"name" : "success_firework_effect_2",
				"type" : "ani_image",
					
				"x" : 140,
				"y" : 60,
					
				"delay" : 6,

				"images" :
				(
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff1.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff2.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff3.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff4.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff6.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff7.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff8.sub",
				),
			},
		
			# Success Effect Animation 3
			{
				"name" : "success_firework_effect_3",
				"type" : "ani_image",
					
				"x" : 183,
				"y" : 60,
					
				"delay" : 6,

				"images" :
				(
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff1.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff2.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff3.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff4.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff6.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff7.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff8.sub",
				),
			},
		
			# Success Text Animation
			{
				"name" : "success_text_effect",
				"type" : "ani_image",
					
				"x" : 131,
				"y" : 93,
					
				"delay" : 6,

				"images" :
				(
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect1.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect6.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect6.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect7.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect8.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect9.sub",
				),
			},
		),
	}
else:
	window = {
		"name"		: "FindGoldenFruitWindow",
		"style"		: ("float", "movable", "ltr", ),
	
		"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
		"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
		"width"		: WINDOW_WIDTH,
		"height"	: WINDOW_HEIGHT,
	
		"children" :
		(
			{
				"name"		: "board",
				"type"		: "board_with_titlebar",
				"style"		: ("ltr", ),

				"x"			: 0,
				"y"			: 0,
			
				"width"		: WINDOW_WIDTH,
				"height"	: WINDOW_HEIGHT,
			
				## 황금의땅
				"title"		: uiScriptLocale.GOLDEN_LAND_FIND_GOLDEN_FRUIT_TITLE,

				"children" :
				(
					## background
					{
						"name"		: "left_main_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: 10,
						"y"			: 32,

						"width"		: LEFT_MAIN_WINDOW_WIDTH,
						"height"	: LEFT_MAIN_WINDOW_HEIGHT,

						"children" :
						(
							## LeftTop 1
							{
								"name" : "bg_left_top",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_left_top.tga",
							},
						
							## RightTop 2
							{
								"name" : "bg_right_top",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : LEFT_MAIN_WINDOW_WIDTH - 16,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_right_top.tga",
							},
						
							## LeftBottom 3
							{
								"name" : "bg_left_bottom",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : LEFT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_left_bottom.tga",
							},
						
							## RightBottom 4
							{
								"name" : "bg_right_bottom",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : LEFT_MAIN_WINDOW_WIDTH - 16,
								"y" : LEFT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_right_bottom.tga",
							},
						
							## topcenterImg 5
							{
								"name" : "bg_center_top",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_top.tga",
								"rect" : (0.0, 0.0, LEFT_MAIN_WINDOW_PATTERN_X_COUNT, 0),
							},
						
							## leftcenterImg 6
							{
								"name" : "bg_center_left",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_left.tga",
								"rect" : (0.0, 0.0, 0, LEFT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
							## rightcenterImg 7
							{
								"name" : "bg_center_right",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : LEFT_MAIN_WINDOW_WIDTH - 16,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_right.tga",
								"rect" : (0.0, 0.0, 0, LEFT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
							## bottomcenterImg 8
							{
								"name" : "bg_center_bottom",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : LEFT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_bottom.tga",
								"rect" : (0.0, 0.0, LEFT_MAIN_WINDOW_PATTERN_X_COUNT, 0),
							},
							## centerImg
							{
								"name" : "bg_center",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_center.tga",
								"rect" : (0.0, 0.0, LEFT_MAIN_WINDOW_PATTERN_X_COUNT, LEFT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
						),
					},

					## background
					{
						"name"		: "right_main_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: 72,
						"y"			: 32,

						"width"		: RIGHT_MAIN_WINDOW_WIDTH,
						"height"	: RIGHT_MAIN_WINDOW_HEIGHT,

						"children" :
						(
							## LeftTop 1
							{
								"name" : "bg_left_top",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_left_top.tga",
							},
						
							## RightTop 2
							{
								"name" : "bg_right_top",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : RIGHT_MAIN_WINDOW_WIDTH - 16,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_right_top.tga",
							},
						
							## LeftBottom 3
							{
								"name" : "bg_left_bottom",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : RIGHT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_left_bottom.tga",
							},
						
							## RightBottom 4
							{
								"name" : "bg_right_bottom",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : RIGHT_MAIN_WINDOW_WIDTH - 16,
								"y" : RIGHT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_right_bottom.tga",
							},
						
							## topcenterImg 5
							{
								"name" : "bg_center_top",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : 0,
								"image" : PATTERN_PATH + "border_A_top.tga",
								"rect" : (0.0, 0.0, RIGHT_MAIN_WINDOW_PATTERN_X_COUNT, 0),
							},
						
							## leftcenterImg 6
							{
								"name" : "bg_center_left",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_left.tga",
								"rect" : (0.0, 0.0, 0, RIGHT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
							## rightcenterImg 7
							{
								"name" : "bg_center_right",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : RIGHT_MAIN_WINDOW_WIDTH - 16,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_right.tga",
								"rect" : (0.0, 0.0, 0, RIGHT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
							## bottomcenterImg 8
							{
								"name" : "bg_center_bottom",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : RIGHT_MAIN_WINDOW_HEIGHT - 16,
								"image" : PATTERN_PATH + "border_A_bottom.tga",
								"rect" : (0.0, 0.0, RIGHT_MAIN_WINDOW_PATTERN_X_COUNT, 0),
							},
							## centerImg
							{
								"name" : "bg_center",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 16,
								"y" : 16,
								"image" : PATTERN_PATH + "border_A_center.tga",
								"rect" : (0.0, 0.0, RIGHT_MAIN_WINDOW_PATTERN_X_COUNT, RIGHT_MAIN_WINDOW_PATTERN_Y_COUNT),
							},
						),
					},
				),
			},

			{
				"name" : "reward_text_bg",
				"type" : "image",
				"style"	: ("ltr", ),

				"x" : 13,
				"y" : 35,

				"width" : 54,
				"height" : 41,
					
				"image"		: ROOT_PATH + "reward_text_bg.sub",	

				"children":
				(
					{
						"name" : "reward_text",
						"type" : "text",

						"x" : 0,
						"y" : 0,

						"horizontal_align" : "center",
						"text_horizontal_align" : "center",

						"vertical_align" : "center",
						"text_vertical_align" : "center",

						"text" : uiScriptLocale.GOLDEN_LAND_FIND_GOLDEN_FRUIT_REWARD,
					},
				),
			},

			{
				"name" : "reward_slot_bg",
				"type" : "image",

				"x" : 15,
				"y" : 89,

				"width" : 50,
				"height" : 115,
					
				"image"		: ROOT_PATH + "reward_slot_bg.sub",	
			},

			{
				"name" : "find_card_text_bg",
				"type" : "image",

				"x" : 75,
				"y" : 35,

				"width" : 282,
				"height" : 21,
					
				"image"		: ROOT_PATH + "find_card_text_bg.sub",

				"children":
				(
					{
						"name" : "find_card_text",
						"type" : "text",

						"x" : 0,
						"y" : 0,

						"horizontal_align" : "center",
						"text_horizontal_align" : "center",

						"vertical_align" : "center",
						"text_vertical_align" : "center",

						"text" : uiScriptLocale.GOLDEN_LAND_FIND_GOLDEN_FRUIT_FIND_CARD,
					},

					# 초기화 버튼
					{
						"name" : "reset_button", "type" : "button", "x" : 340 - 75, "y" : 38 - 35,
					
						"default_image"	: ROOT_PATH + "button_reset_default.sub", 
						"over_image"	: ROOT_PATH + "button_reset_over.sub",
						"down_image"	: ROOT_PATH + "button_reset_down.sub",

						"tooltip_text"	: uiScriptLocale.GOLDEN_LAND_FIND_GOLDEN_FRUIT_TOOLTIP_RESET,
					},
				),
			},

			{
				"name" : "card_window_bg",
				"type" : "image",

				"x" : 79,
				"y" : 61,
					
				"image"	: ROOT_PATH + "card_window_bg.sub",	
			},

			#카드 슬롯
			{
				"name" : "card_slot", "type" : "slot", "x" : 84, "y" : 66, "width" : 264, "height" : 132,
				"slot" :
				(
					# line 1
					{"index":0, "x":84-84, "y":66-66, "width":33, "height":33},
					{"index":1, "x":117-84, "y":66-66, "width":33, "height":33},
					{"index":2, "x":150-84, "y":66-66, "width":33, "height":33},
					{"index":3, "x":183-84, "y":66-66, "width":33, "height":33},
					{"index":4, "x":216-84, "y":66-66, "width":33, "height":33},
					{"index":5, "x":249-84, "y":66-66, "width":33, "height":33},
					{"index":6, "x":282-84, "y":66-66, "width":33, "height":33},
					{"index":7, "x":315-84, "y":66-66, "width":33, "height":33},
				
					# line 2
					{"index":8, "x":84-84, "y":99-66, "width":33, "height":33},
					{"index":9, "x":117-84, "y":99-66, "width":33, "height":33},
					{"index":10, "x":150-84, "y":99-66, "width":33, "height":33},
					{"index":11, "x":183-84, "y":99-66, "width":33, "height":33},
					{"index":12, "x":216-84, "y":99-66, "width":33, "height":33},
					{"index":13, "x":249-84, "y":99-66, "width":33, "height":33},
					{"index":14, "x":282-84, "y":99-66, "width":33, "height":33},
					{"index":15, "x":315-84, "y":99-66, "width":33, "height":33},

					# line 3
					{"index":16, "x":84-84, "y":132-66, "width":33, "height":33},
					{"index":17, "x":117-84, "y":132-66, "width":33, "height":33},
					{"index":18, "x":150-84, "y":132-66, "width":33, "height":33},
					{"index":19, "x":183-84, "y":132-66, "width":33, "height":33},
					{"index":20, "x":216-84, "y":132-66, "width":33, "height":33},
					{"index":21, "x":249-84, "y":132-66, "width":33, "height":33},
					{"index":22, "x":282-84, "y":132-66, "width":33, "height":33},
					{"index":23, "x":315-84, "y":132-66, "width":33, "height":33},

					# line 4
					{"index":24, "x":84-84, "y":165-66, "width":33, "height":33},
					{"index":25, "x":117-84, "y":165-66, "width":33, "height":33},
					{"index":26, "x":150-84, "y":165-66, "width":33, "height":33},
					{"index":27, "x":183-84, "y":165-66, "width":33, "height":33},
					{"index":28, "x":216-84, "y":165-66, "width":33, "height":33},
					{"index":29, "x":249-84, "y":165-66, "width":33, "height":33},
					{"index":30, "x":282-84, "y":165-66, "width":33, "height":33},
					{"index":31, "x":315-84, "y":165-66, "width":33, "height":33},
				),	
			},

			# 필요 노란열매 개수 윈도우 
			{
				"name" : "need_yellow_fruit_window",
				"type" : "window",

				"x" : 79,
				"y" : 213,

				"width"		: 273,
				"height"	: 11,

				"children" :
				(
					# 필요 노란열매 개수
					{
						"name" : "need_yellow_fruit_text_window",
						"type" : "window",

						"x" : 213 - 79,
						"y" : 0,
					
						"width"		: 76,
						"height"	: 11,
					
						"children" :
						(
							{
								"name" : "need_yellow_fruit_text",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"horizontal_align" : "right",
								"text_horizontal_align" : "right",

								"vertical_align" : "center",
								"text_vertical_align" : "center",

								"text" : uiScriptLocale.GOLDEN_LAND_FIND_GOLDEN_FRUIT_NEED_YELLOW_FRUIT_COUNT,
							},
						),
					},

					{
						"name" : "yellow_fruit_need_count_fruit_img",
						"type" : "image",

						"x" : 294 - 79,
						"y" : -3,
					
						"image"	 : "d:/ymir work/ui/game/golden_land/yellow_fruit_small.sub",
					},

					{
						"name" : "yellow_fruit_need_count_bg",
						"type" : "image",

						"x" : 314 - 79,
						"y" : -4,

						"width" : 37,
						"height" : 18,
					
						"image"		: ROOT_PATH + "yellow_fruit_count_bg.sub",

						"children":
						(
							{
								"name" : "yellow_fruit_need_count_text",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"horizontal_align" : "center",
								"text_horizontal_align" : "center",

								"vertical_align" : "center",
								"text_vertical_align" : "center",

								"text" : "9999",
							},
						),
					},
				),
			},

			# 보유 노란열매 개수 윈도우
			{
				"name" : "yellow_fruit_window",
				"type" : "window",

				"x" : 79,
				"y" : 234,

				"width"		: 273,
				"height"	: 11,

				"children" :
				(
					# 보유 노란열매 개수
					{
						"name" : "yellow_fruit_text_window",
						"type" : "window",

						"x" : 213 - 79,
						"y" : 0,
					
						"width"		: 76,
						"height"	: 11,
					
						"children" :
						(
							{
								"name" : "yellow_fruit_text",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"horizontal_align" : "right",
								"text_horizontal_align" : "right",

								"vertical_align" : "center",
								"text_vertical_align" : "center",

								"text" : uiScriptLocale.GOLDEN_LAND_FIND_GOLDEN_FRUIT_YELLOW_FRUIT_COUNT,
							},
						),
					},


					{
						"name" : "yellow_fruit_count_fruit_img",
						"type" : "image",

						"x" : 294 - 79,
						"y" : -3,
					
						"image"		: "d:/ymir work/ui/game/golden_land/yellow_fruit_small.sub",
					},

					{
						"name" : "yellow_fruit_count_bg",
						"type" : "image",

						"x" : 314 - 79,
						"y" : -4,
					
						"width" : 37,
						"height" : 18,

						"image"		: ROOT_PATH + "yellow_fruit_count_bg.sub",

						"children":
						(
							{
								"name" : "yellow_fruit_count_text",
								"type" : "text",

								"x" : 0,
								"y" : 0,

								"horizontal_align" : "center",
								"text_horizontal_align" : "center",

								"vertical_align" : "center",
								"text_vertical_align" : "center",

								"text" : "1234",
							},
						),
					},
				),
			}, 

			# Success Effect Animation 1
			{
				"name" : "success_firework_effect_1",
				"type" : "ani_image",
					
				"x" : 97,
				"y" : 60,
					
				"delay" : 6,

				"images" :
				(
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff1.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff2.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff3.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff4.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff6.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff7.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff8.sub",
				),
			},
		
			# Success Effect Animation 2
			{
				"name" : "success_firework_effect_2",
				"type" : "ani_image",
					
				"x" : 140,
				"y" : 60,
					
				"delay" : 6,

				"images" :
				(
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff1.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff2.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff3.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff4.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff6.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff7.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff8.sub",
				),
			},
		
			# Success Effect Animation 3
			{
				"name" : "success_firework_effect_3",
				"type" : "ani_image",
					
				"x" : 183,
				"y" : 60,
					
				"delay" : 6,

				"images" :
				(
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff1.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff2.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff3.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff4.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff6.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff7.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_eff8.sub",
				),
			},
		
			# Success Text Animation
			{
				"name" : "success_text_effect",
				"type" : "ani_image",
					
				"x" : 131,
				"y" : 93,
					
				"delay" : 6,

				"images" :
				(
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect1.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect6.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect6.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect7.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect8.sub",
					"d:/ymir work/ui/minigame/rumi/card_completion_effect/card_completion_text_effect9.sub",
				),
			},
		),
	}
