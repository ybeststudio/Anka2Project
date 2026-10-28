import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/minigame/challenge/"
PUBLIC_PATH = "d:/ymir work/ui/public/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"

WINDOW_WIDTH		= 456
WINDOW_HEIGHT		= 396

BOARD_WIDTH			= 434
BOARD_HEIGHT		= 312

PATTERN_WINDOW_WIDTH	= BOARD_WIDTH
PATTERN_WINDOW_HEIGHT	= BOARD_HEIGHT
PATTERN_X_COUNT = (PATTERN_WINDOW_WIDTH - 32) / 16
PATTERN_Y_COUNT = (PATTERN_WINDOW_HEIGHT - 32) / 16

TAB_WINDOW_HEIGHT	= 53

window = {
	"name" : "MiniGameChallenge",
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
			
			"title" : uiScriptLocale.MINI_GAME_CHALLENGE_TITLE,
			
			"children" :
			(
				{
					"name" : "board_area_window",
					"type" : "window",
					"style" : ("attach",),
					
					"x" : 10,
					"y" : 32,
					"width" : BOARD_WIDTH,
					"height" :BOARD_HEIGHT,
					
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
				
				## tab window
				{
					"name" : "tab_window",
					"type" : "window",
					
					"style" : ("attach", "ltr",),

					"x" : 0,
					"y" : 343,
					
					"width" : WINDOW_WIDTH,
					"height" : TAB_WINDOW_HEIGHT,
					
					"children" :
					(
						{
							"name" : "tab_desc_img",
							"type" : "image",
							"style" : ("attach", "not_pick",),
							"x" : 0,
							"y" : 0,
							"image" : ROOT_PATH + "tab/tab_desc.tga",
						},
						{
							"name" : "tab_play_img",
							"type" : "image",
							"style" : ("attach", "not_pick",),
							"x" : 0,
							"y" : 0,
							"image" : ROOT_PATH + "tab/tab_play.tga",
						},
						{
							"name" : "tab_reward_img",
							"type" : "image",
							"style" : ("attach", "not_pick",),
							"x" : 0,
							"y" : 0,
							"image" : ROOT_PATH + "tab/tab_reward.tga",
						},
						{
							"name" : "tab_rank_img",
							"type" : "image",
							"style" : ("attach", "not_pick",),
							"x" : 0,
							"y" : 0,
							"image" : ROOT_PATH + "tab/tab_rank.tga",
						},
						
						## click area window
						{
							"name" : "tab_desc_window",
							"type" : "window",
							"x" : 23,
							"y" : 14,
							"width" : 75,
							"height" :28,
						},
						{
							"name" : "tab_play_window",
							"type" : "window",
							"x" : 134,
							"y" : 14,
							"width" : 75,
							"height" :28,
						},
						{
							"name" : "tab_reward_window",
							"type" : "window",
							"x" : 246,
							"y" : 14,
							"width" : 75,
							"height" :28,
						},
						{
							"name" : "tab_rank_window",
							"type" : "window",
							"x" : 357,
							"y" : 14,
							"width" : 75,
							"height" :28,
						},
					),
				},
				
			),
		},
	),	
}
