import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/minigame/challenge/"
PUBLIC_PATH = "d:/ymir work/ui/public/"

BOARD_WIDTH		= 434
BOARD_HEIGHT	= 312

window = {
	"name" : "MiniGameChallengeSubWindowDesc",
	"style" : ("attach",),
	
	"x" : 0,
	"y" : 0,
	
	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,
	
	"children" :
	(
		## bg pattern window
		{
			"name" : "desc_board_window",
			"type" : "window",
			
			"style" : ("attach", "ltr",),

			"x" : 0,
			"y" : 0,
			
			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,
		},
		
		{
			"name" : "prev_button",
			"type" : "button",

			"x" : 380-10,
			"y" : 323-32,

			"default_image" : PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
			"over_image" : PUBLIC_PATH + "public_intro_btn/prev_btn_02.sub",
			"down_image" : PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
		},
		
		{
			"name" : "next_button",
			"type" : "button",

			"x" : 410-10,
			"y" : 323-32,

			"default_image" : PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
			"over_image" : PUBLIC_PATH + "public_intro_btn/next_btn_02.sub",
			"down_image" : PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
		},
	),	
}
