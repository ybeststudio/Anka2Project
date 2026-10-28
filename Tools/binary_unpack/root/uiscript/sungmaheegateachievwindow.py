import uiScriptLocale

PUBLIC_PATH		= "d:/ymir work/ui/public/"
PATTERN_PATH	= "d:/ymir work/ui/pattern/"
ROOT_PATH		= "d:/ymir work/ui/game/sungmahee_gate/achiev/"

WINDOW_WIDTH	= 336
WINDOW_HEIGHT	= 448

MAIN_WINDOW_WIDTH	= 314
MAIN_WINDOW_HEIGHT	= 404
MAIN_WINDOW_PATTERN_X_COUNT = (MAIN_WINDOW_WIDTH - 32) / 16
MAIN_WINDOW_PATTERN_Y_COUNT = (MAIN_WINDOW_HEIGHT - 32) / 16

window = {
	"name" : "SungmaheeGateAchievWindow",
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
			"title"		: uiScriptLocale.SUNGMAHEE_GATE_TITLE,
			
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
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						
						## LeftBottom 3
						{
							"name" : "bg_left_bottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						
						## RightBottom 4
						{
							"name" : "bg_right_bottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : MAIN_WINDOW_HEIGHT - 16,
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
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
						},
						
						## leftcenterImg 6
						{
							"name" : "bg_center_left",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, MAIN_WINDOW_PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "bg_center_right",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, MAIN_WINDOW_PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "bg_center_bottom",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "bg_center",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, MAIN_WINDOW_PATTERN_Y_COUNT),
						},
					],
				},
				
				#	증표 업적
				{
					"name" : "achiev_title_background",
					"type" : "image",

					"x" : 13,
					"y" : 35,
					
					"image"		: ROOT_PATH + "background_image_title_achiev.sub",
					
					"children" :
					[
						{
							"name" : "achiev_title_text",
							"type" : "text",

							"x" : 0,
							"y" : 3,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",

							"text" : uiScriptLocale.SUNGMAHEE_GATE_TITLE_ACHIEV,
						},
					],
				},
				
				#	증표 업적 달성 개수
				{
					"name" : "achiev_count_background",
					"type" : "image",

					"x" : 27,
					"y" : 63,
					
					"image"		: ROOT_PATH + "background_image_achiev_count.sub",
					
					"children" :
					[
						{
							"name" : "achiev_count_title_text_window",
							"type" : "window",

							"x" : 55 - 27,
							"y" : 70 - 63,

							"width" : 142,
							"height" : 11,

							"children" :
							[
								{
									"name" : "achiev_count_title_text",
									"type" : "text",

									"x" : 0,
									"y" : 0,

									"horizontal_align" : "right",
									"text_horizontal_align" : "right",
									"vertical_align" : "center",
									"text_vertical_align" : "center",

									"text" : uiScriptLocale.SUNGMAHEE_GATE_ACHIEV_COUNT,
									"color"	: 0xFFC1BEB9,
								},
							],
						},

						{
							"name" : "achiev_count_text_window",
							"type" : "window",

							"x" : 205 - 27,
							"y" : 67 - 63,

							"width" : 25,
							"height" : 16,

							"children" :
							[
								{
									"name" : "achiev_count_text",
									"type" : "text",

									"x" : 0,
									"y" : 0,

									"horizontal_align" : "center",
									"text_horizontal_align" : "center",
									"vertical_align" : "center",
									"text_vertical_align" : "center",

									"text" : "0",
									"color"	: 0xFFCEC6B5,
								},
							],
						},
					],
				},

				#	증표 업적 이미지 목록 (슬롯)
				{
					"name" : "achiev_slot", "type" : "slot", "x" : 39, "y" : 101, "width" : 256, "height" : 218,
					"slot" :
					(
						{"index":0, "x":39-39, "y":101-101, "width":64, "height":64},
						{"index":1, "x":135-39, "y":101-101, "width":64, "height":64},
						{"index":2, "x":231-39, "y":101-101, "width":64, "height":64},

						{"index":3, "x":39-39, "y":178-101, "width":64, "height":64},
						{"index":4, "x":135-39, "y":178-101, "width":64, "height":64},
						{"index":5, "x":231-39, "y":178-101, "width":64, "height":64},

						{"index":6, "x":39-39, "y":255-101, "width":64, "height":64},
						{"index":7, "x":135-39, "y":255-101, "width":64, "height":64},
						{"index":8, "x":231-39, "y":255-101, "width":64, "height":64},
					),
				},

				#	증표 보상
				{
					"name" : "reward_title_background",
					"type" : "image",

					"x" : 13,
					"y" : 332,
					
					"image"		: ROOT_PATH + "background_image_title_reward.sub",
					
					"children" :
					[
						{
							"name" : "reward_title_text",
							"type" : "text",

							"x" : 0,
							"y" : 3,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",

							"text" : uiScriptLocale.SUNGMAHEE_GATE_TITLE_REWARD,
						},
					],
				},

				# Reward 0
				{
					"name" : "reward_button_0", "type" : "toggle_button", "x" : 20, "y" : 360, "width" : 29, "height" : 29, "outline" : 1,
					"default_image"	: ROOT_PATH + "button_normal_default.sub", "over_image"	: ROOT_PATH + "button_normal_over.sub", "down_image"	: ROOT_PATH + "button_normal_down.sub",

					"children" :
					[
						{
							"name" : "reward_button_0_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"vertical_align" : "center",
							"text_vertical_align" : "center",

							"text" : "",
						},
					],
				},

				# Reward 1
				{
					"name" : "reward_button_1", "type" : "toggle_button", "x" : 53, "y" : 360, "width" : 29, "height" : 29, "outline" : 1,
					"default_image"	: ROOT_PATH + "button_normal_default.sub", "over_image"	: ROOT_PATH + "button_normal_over.sub", "down_image"	: ROOT_PATH + "button_normal_down.sub",

					"children" :
					[
						{
							"name" : "reward_button_1_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"vertical_align" : "center",
							"text_vertical_align" : "center",

							"text" : "",
						},
					],
				},

				# Reward 2
				{
					"name" : "reward_button_2", "type" : "toggle_button", "x" : 86, "y" : 360, "width" : 29, "height" : 29, "outline" : 1,
					"default_image"	: ROOT_PATH + "button_normal_default.sub", "over_image"	: ROOT_PATH + "button_normal_over.sub", "down_image"	: ROOT_PATH + "button_normal_down.sub",

					"children" :
					[
						{
							"name" : "reward_button_2_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"vertical_align" : "center",
							"text_vertical_align" : "center",

							"text" : "",
						},
					],
				},

				# Reward 3
				{
					"name" : "reward_button_3", "type" : "toggle_button", "x" : 119, "y" : 360, "width" : 29, "height" : 29, "outline" : 1,
					"default_image"	: ROOT_PATH + "button_normal_default.sub", "over_image"	: ROOT_PATH + "button_normal_over.sub", "down_image"	: ROOT_PATH + "button_normal_down.sub",

					"children" :
					[
						{
							"name" : "reward_button_3_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"vertical_align" : "center",
							"text_vertical_align" : "center",

							"text" : "",
						},
					],
				},

				# Reward 4
				{
					"name" : "reward_button_4", "type" : "toggle_button", "x" : 152, "y" : 360, "width" : 29, "height" : 29, "outline" : 1,
					"default_image"	: ROOT_PATH + "button_normal_default.sub", "over_image"	: ROOT_PATH + "button_normal_over.sub", "down_image"	: ROOT_PATH + "button_normal_down.sub",

					"children" :
					[
						{
							"name" : "reward_button_4_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"vertical_align" : "center",
							"text_vertical_align" : "center",

							"text" : "",
						},
					],
				},

				# Reward 5
				{
					"name" : "reward_button_5", "type" : "toggle_button", "x" : 185, "y" : 360, "width" : 29, "height" : 29, "outline" : 1,
					"default_image"	: ROOT_PATH + "button_normal_default.sub", "over_image"	: ROOT_PATH + "button_normal_over.sub", "down_image"	: ROOT_PATH + "button_normal_down.sub",

					"children" :
					[
						{
							"name" : "reward_button_5_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"vertical_align" : "center",
							"text_vertical_align" : "center",

							"text" : "",
						},
					],
				},

				# Reward 6
				{
					"name" : "reward_button_6", "type" : "toggle_button", "x" : 218, "y" : 360, "width" : 29, "height" : 29, "outline" : 1,
					"default_image"	: ROOT_PATH + "button_normal_default.sub", "over_image"	: ROOT_PATH + "button_normal_over.sub", "down_image"	: ROOT_PATH + "button_normal_down.sub",

					"children" :
					[
						{
							"name" : "reward_button_6_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"vertical_align" : "center",
							"text_vertical_align" : "center",

							"text" : "",
						},
					],
				},

				# Reward 7
				{
					"name" : "reward_button_7", "type" : "toggle_button", "x" : 251, "y" : 360, "width" : 29, "height" : 29, "outline" : 1,
					"default_image"	: ROOT_PATH + "button_normal_default.sub", "over_image"	: ROOT_PATH + "button_normal_over.sub", "down_image"	: ROOT_PATH + "button_normal_down.sub",

					"children" :
					[
						{
							"name" : "reward_button_7_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"vertical_align" : "center",
							"text_vertical_align" : "center",

							"text" : "",
						},
					],
				},

				# Reward 8
				{
					"name" : "reward_button_8", "type" : "toggle_button", "x" : 284, "y" : 360, "width" : 29, "height" : 29, "outline" : 1,
					"default_image"	: ROOT_PATH + "button_normal_default.sub", "over_image"	: ROOT_PATH + "button_normal_over.sub", "down_image"	: ROOT_PATH + "button_normal_down.sub",

					"children" :
					[
						{
							"name" : "reward_button_8_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",
							"vertical_align" : "center",
							"text_vertical_align" : "center",

							"text" : "",
						},
					],
				},

				# Reward List Button
				{
					"name" : "reward_list_button", "type" : "toggle_button", "x" : 76, "y" : 400,
					
					"default_image"	: ROOT_PATH + "button_reward_list_default.sub", 
					"over_image"	: ROOT_PATH + "button_reward_list_over.sub",
					"down_image"	: ROOT_PATH + "button_reward_list_down.sub",

					"tooltip_text"	: uiScriptLocale.SUNGMAHEE_GATE_ACHIEV_TOOLTIP_REWARD_LIST,
				},

				# Get Reward Button
				{
					"name" : "get_reward_button", "type" : "button", "x" : 170, "y" : 400,
					
					"default_image"	: ROOT_PATH + "button_get_reward_default.sub", 
					"over_image"	: ROOT_PATH + "button_get_reward_over.sub",
					"down_image"	: ROOT_PATH + "button_get_reward_down.sub",

					"tooltip_text"	: uiScriptLocale.SUNGMAHEE_GATE_ACHIEV_TOOLTIP_GET_REWARD,
				},
			],
		},
	],
}