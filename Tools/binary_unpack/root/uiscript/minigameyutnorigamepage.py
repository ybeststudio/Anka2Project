import uiScriptLocale

ROOT = "d:/ymir work/ui/minigame/yutnori/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"

WINDOW_WIDTH	= 456
WINDOW_HEIGHT	= 428

BOARD_WINDOW_WIDTH	= 436
BOARD_WINDOW_HEIGHT = 66

PATTERN_X_COUNT = 25
PATTERN_Y_COUNT = 2

window = {
	"name" : "MiniGameYutnoriGamePage",
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
			
			"title" : uiScriptLocale.MINI_GAME_YUTNORI_TITLE,
			
			"children" :
			(				
				## Game Board Img
				{
					"name" : "BoardImg",
					"type" : "expanded_image",
					"style" : ("attach",),
					
					"x" : 10,
					"y" : 32,

					"image" : ROOT + "board.sub",
				},
				## End Img
				{
					"name" : "end_img",
					"type" : "image",
					"style" : ("attach",),
					
					"x" : 270-2,
					"y" : 292-2,

					"image" : ROOT + "end_img.sub",
				},
				# yut throw button
				{
					"name" : "yut_throw_button",
					"type" : "button",

					"x" : 328,
					"y" : 180,

					"default_image" : ROOT + "yut_throw_button_default.sub",
					"over_image" : ROOT + "yut_throw_button_over.sub",
					"down_image" : ROOT + "yut_throw_button_down.sub",
					
					"text" : uiScriptLocale.MINI_GAME_YUTNORI_THROW,
				},
				## yut result img
				{
					"name" : "yut_result_img",
					"type" : "expanded_image",
					"style" : ("attach",),
					
					"x" : 328,
					"y" : 205,

					"image" : ROOT + "yut_result_board.sub",
				},
				## yut result model view
				{
					"name" : "model_view",
					"type" : "render_target",
					"style" : ("not_pick","float",),

					"x" : 10,
					"y" : 32,
					
					"width" : 312,
					"height" : 312,

					"index" : 2,
				},
				
				## score img
				{
					"name" : "score_img",
					"type" : "expanded_image",
					"style" : ("attach",),
					
					"x" : 326,
					"y" : 35,

					"image" : ROOT + "text_bg.sub",
				},
				## score title text
				{ 
					"name" : "score_title_window", "type" : "window", "x" : 326, "y" : 35, "width" : 120, "height" : 20, "style" : ("attach",),
					"children" :
					(
						{"name":"score_title", "type":"text", "x":0, "y":0, "text":uiScriptLocale.MINI_GAME_YUTNORI_SCORE, "all_align" : "center"},
					),	
				},
				## score text
				{ 
					"name" : "score_text_window", "type" : "window", "x" : 326, "y" : 56, "width" : 120, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{"name":"score_text", "type":"text", "x":0, "y":0, "text":"250", "all_align" : "center", "color" : 0xffFFFF99, },
					),	
				},
				
				## remain count img
				{
					"name" : "remain_img",
					"type" : "expanded_image",
					"style" : ("attach",),
					
					"x" : 326,
					"y" : 83,

					"image" : ROOT + "text_bg.sub",
				},
				## remain count title text
				{ 
					"name" : "remain_count_title_window", "type" : "window", "x" : 326, "y" : 83, "width" : 120, "height" : 20, "style" : ("attach",),
					"children" :
					(
						{"name":"remain_count_title", "type":"text", "x":0, "y":0, "text":uiScriptLocale.MINI_GAME_YUTNORI_REMAIN_COUNT, "all_align" : "center"},
					),	
				},
				## remain count text
				{ 
					"name" : "remain_count_text_window", "type" : "window", "x" : 326, "y" : 104, "width" : 120, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{"name":"remain_count_text", "type":"text", "x":0, "y":0, "text":"20", "all_align" : "center", "color" : 0xffEEA900, },
					),	
				},
				
				## probability img
				{
					"name" : "probability_img",
					"type" : "expanded_image",
					"style" : ("attach",),
					
					"x" : 326,
					"y" : 130,

					"image" : ROOT + "text_bg.sub",
				},
				## probability title text
				{ 
					"name" : "probability_title_window", "type" : "window", "x" : 326, "y" : 130, "width" : 120, "height" : 20, "style" : ("attach",),
					"children" :
					(
						{"name":"probability_title", "type":"text", "x":0, "y":0, "text":uiScriptLocale.MINI_GAME_YUTNORI_PROBABILITY, "all_align" : "center"},
					),	
				},
				## probability text
				{ 
					"name" : "probability_text_window", "type" : "window", "x" : 326, "y" : 151, "width" : 120, "height" : 18,
					"children" :
					(
						{"name":"probability_text", "type":"text", "x":0, "y":0, "text":"도", "all_align" : "center", "color" : 0xffEEA900, },
					),	
				},
				
				## probability select window
				{
					"name" : "prob_select_window",
					"type" : "window",

					"x" : 328,
					"y" : 168,
					"width" : 115,
					"height" : 0,
				},
				
				## probability select mouse over img
				{
					"name" : "mouse_over_image",
					"type" : "expanded_image",
					"style" : ("not_pick",),
					"x" : 0,
					"y" : 0,
					"image" : ROOT + "list_over.sub",
				},
				
				# probability select button
				{
					"name" : "prob_select_button",
					"type" : "button",

					"x" : 426,
					"y" : 151,

					"default_image" : ROOT + "arrow_button_default.sub",
					"over_image" : ROOT + "arrow_button_over.sub",
					"down_image" : ROOT + "arrow_button_down.sub",
				},
				
				## main bg
				{
					"name" : "board_window",
					"type" : "window",
					
					"style" : ("attach", "ltr",),

					"x" : 10,
					"y" : 349,
					
					"width" : BOARD_WINDOW_WIDTH,
					"height" : BOARD_WINDOW_HEIGHT,
					
					"children" :
					(
						## LeftTop 1
						{
							"name" : "LeftTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						## RightTop 2
						{
							"name" : "RightTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : BOARD_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "LeftBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : BOARD_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "RightBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : BOARD_WINDOW_WIDTH - 16,
							"y" : BOARD_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						## topcenterImg 5
						{
							"name" : "TopCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "LeftCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "RightCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : BOARD_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "BottomCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : BOARD_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "CenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, PATTERN_Y_COUNT),
						},
					),
				},
				
				## notice img
				{
					"name" : "notice_img",
					"type" : "expanded_image",
					"style" : ("attach",),
					
					"x" : 13,
					"y" : 394,
					
					"x_scale" : 1.945,
					"y_scale" : 1.0,

					"image" : ROOT + "notice.sub",
				},
				## notice
				{ 
					"name" : "notice_window", "type" : "window", "x" : 21, "y" : 394, "width" : 420, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{"name":"notice_text", "type":"text", "x":0, "y":0, "text": "", "text_horizontal_align":"left", "text_vertical_align":"center", "vertical_align":"center",},
					),	
				},
				
				## player, com, reward window
				{ 
					"name" : "etc_window", "type" : "window", "x" : 19, "y" : 355, "width" : 420, "height" : 36, "style" : ("attach",),
					"children" :
					(
						## player img bg
						{
							"name" : "player_img_bg",
							"type" : "expanded_image",
							"style" : ("attach",),
							
							"x" : 0,
							"y" : 0,

							"image" : ROOT + "player_img_bg.sub",
						},
						## player img bg2
						{
							"name" : "player_img_bg2",
							"type" : "expanded_image",
							"style" : ("attach",),
							
							"x" : 39,
							"y" : 0,

							"image" : ROOT + "player_img_bg.sub",
						},
						## player text
						{ 
							"name" : "player_text_window", "type" : "window", "x" : 81, "y" : 8, "width" : 75, "height" : 21, "style" : ("attach",),
							"children" :
							(
								{"name":"player_text_cover_img", "type":"expanded_image", "style" : ("attach",), "x":0, "y":0, "image" : ROOT + "naming_cover.sub", },
								{"name":"player_text_cover_over_img", "type":"expanded_image", "style" : ("attach",), "x":0, "y":0, "image" : ROOT + "naming_cover_over.sub", },
								{"name":"player_text", "type":"text", "x":0, "y":0, "text":uiScriptLocale.MINI_GAME_YUTNORI_PLAYER_TEXT, "all_align" : "center", "color" : 0xffEEA900,},
							),	
						},
						
						## enemy img bg
						{
							"name" : "enemy_img_bg",
							"type" : "expanded_image",
							"style" : ("attach",),
							
							"x" : 162,
							"y" : 0,

							"image" : ROOT + "enemy_img_bg.sub",
						},
						## enemy move pos img
						{
							"name" : "enemy_img_bg2",
							"type" : "expanded_image",
							"style" : ("attach",),
							
							"x" : 201,
							"y" : 0,

							"image" : ROOT + "enemy_img_bg.sub",
						},
						## enemy text
						{ 
							"name" : "enemy_text_window", "type" : "window", "x" : 244, "y" : 8, "width" : 75, "height" : 21, "style" : ("attach",),
							"children" :
							(
								{"name":"enemy_text_cover_img", "type":"expanded_image", "style" : ("attach",), "x":0, "y":0, "image" : ROOT + "naming_cover.sub", },
								{"name":"enemy_text_cover_over_img", "type":"expanded_image", "style" : ("attach",), "x":0, "y":0, "image" : ROOT + "naming_cover_over.sub", },
								{"name":"enemy_text", "type":"text", "x":0, "y":0, "text":uiScriptLocale.MINI_GAME_YUTNORI_COM_TEXT, "all_align" : "center"},
							),	
						},
						
						# reward button
						{
							"name" : "reward_button",
							"type" : "button",

							"x" : 326,
							"y" : 8,

							"default_image" : "d:/ymir work/ui/public/Large_Button_01.sub",
							"over_image" : "d:/ymir work/ui/public/Large_Button_02.sub",
							"down_image" : "d:/ymir work/ui/public/Large_Button_03.sub",
							"disable_image" : "d:/ymir work/ui/public/Large_Button_03.sub",
							
							"text" : uiScriptLocale.MINI_GAME_YUTNORI_REWARD_TEXT,
						},
					),	
				},
				
				## goal effect
				{
					"name" : "goal_effect1",
					"type" : "ani_image",
					
					"x" : 57,
					"y" : 100,
					
					"delay" : 6,

					"images" :
					(
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff1.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff2.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff3.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff4.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff5.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff6.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff7.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff8.sub",
					),
				},
				
				{
					"name" : "goal_effect2",
					"type" : "ani_image",
					
					"x" : 100,
					"y" : 100,
					
					"delay" : 6,

					"images" :
					(
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff1.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff2.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff3.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff4.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff5.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff6.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff7.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff8.sub",
					),
				},
				
				{
					"name" : "goal_effect3",
					"type" : "ani_image",
					
					"x" : 143,
					"y" : 100,
					
					"delay" : 6,

					"images" :
					(
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff1.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff2.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff3.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff4.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff5.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff6.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff7.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff8.sub",
					),
				},
				
				{
					"name" : "goal_text_effect",
					"type" : "ani_image",
					
					"x" : 91,
					"y" : 133,
					
					"delay" : 0,

					"images" :
					(
						
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_text_effect1.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_text_effect5.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_text_effect6.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_text_effect6.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_text_effect7.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_text_effect8.sub",
						"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_text_effect9.sub",
					),
				},
			),
		},
	),	
}
