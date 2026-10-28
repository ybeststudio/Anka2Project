import uiScriptLocale

PUBLIC_PATH = "d:/ymir work/ui/public/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"

WINDOW_WIDTH	= 376
WINDOW_HEIGHT	= 336

BOARD_WIDTH		= 354
BOARD_HEIGHT	= 248


window = {
	"name" : "MiniGameBNWWaitingPage",
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
			
			"title" : uiScriptLocale.MINI_GAME_BLACK_AND_WHITE_WAITTING_TITLE,
			
			"children" :
			(		
				{
					"name" : "desc_board",
					"type" : "bar",
					
					"x" : 10,
					"y" : 32,

					"width" : BOARD_WIDTH,
					"height" : BOARD_HEIGHT,
					
					"children" :
					(
						{
							"name" : "right_line",
							"type" : "line",

							"x" : BOARD_WIDTH-1,
							"y" : 0,

							"width" : 0,
							"height" : BOARD_HEIGHT,

							"color" : 0xffAAA6A1,
						},
						
						{
							"name" : "bottom_line",
							"type" : "line",

							"x" : 0,
							"y" : BOARD_HEIGHT-1,

							"width" : BOARD_WIDTH,
							"height" : 0,

							"color" : 0xffAAA6A1,
						},
						{
							"name" : "left_line",
							"type" : "line",

							"x" : 0,
							"y" : 0,

							"width" : 0,
							"height" : BOARD_HEIGHT,

							"color" : 0xff2A2521,
						},
						{
							"name" : "top_line",
							"type" : "line",

							"x" : 0,
							"y" : 0,

							"width" : BOARD_WIDTH,
							"height" : 0,

							"color" : 0xff2A2521,
						},
					),
				
				},
				
				{
					"name" : "prev_button",
					"type" : "button",

					"x" : 303,
					"y" : 284,

					"default_image" : PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
					"over_image" : PUBLIC_PATH + "public_intro_btn/prev_btn_02.sub",
					"down_image" : PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
				},
				
				{
					"name" : "next_button",
					"type" : "button",

					"x" : 337,
					"y" : 284,

					"default_image" : PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
					"over_image" : PUBLIC_PATH + "public_intro_btn/next_btn_02.sub",
					"down_image" : PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
				},
				
				{
					"name" : "game_start_button",
					"type" : "button",
					
					"x" : 145,
					"y" : 295,
					
					"text" : uiScriptLocale.MINI_GAME_BLACK_AND_WHITE_START_BUTTON,					
					
					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				
				{
					"name" : "rank_button",
					"type" : "button",
					
					"x" : 15,
					"y" : 283,
					
					"text" : uiScriptLocale.MINI_GAME_BLACK_AND_WHITE_RANK_BUTTON,					
					
					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				
				{
					"name" : "reward_trade_button",
					"type" : "button",
					
					"x" : 15,
					"y" : 304,
					
					"text" : uiScriptLocale.MINI_GAME_BLACK_AND_WHITE_REWARD_TRADE,					
					
					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
			),
		},
	),	
}
