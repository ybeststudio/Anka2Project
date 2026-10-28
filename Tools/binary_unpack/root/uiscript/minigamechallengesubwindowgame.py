import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/minigame/challenge/"

BOARD_WIDTH		= 434
BOARD_HEIGHT	= 312

ROULETTE_WINDOW_WIDTH	= 284
ROULETTE_WINDOW_HEIGHT	= 284

window = {
	"name" : "MiniGameChallengeGamePage",
	"style" : ("attach",),
	
	"x" : 0,
	"y" : 0,
	
	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,
	
	"children" :
	(
		## vertical line
		{
			"name" : "vertical_line",
			"type" : "image",
			"x" : 309,	# 319-10,
			"y" : 0,	# 32-32,
			"image" : ROOT_PATH + "vertical_line.sub",
		},
						
		## roulette_window
		{
			"name" : "roulette_window",
			"type" : "window",
			"style" : ("attach",),
			"x" : 23-10,
			"y" : 45-32,
			"width" : ROULETTE_WINDOW_WIDTH,
			"height" : ROULETTE_WINDOW_HEIGHT,
		},
		
		## computer scissors,rock,paper animation img
		{
			"name" : "com_scissors_rock_paper_ani",
			"type" : "ani_image",
			
			"x" : 133-10,
			"y" : 101-32,
			
			"delay" : 10,

			"images" :
			(
				ROOT_PATH + "scissors_com_img.sub",
				ROOT_PATH + "rock_com_img.sub",
				ROOT_PATH + "paper_com_img.sub",
			),
		},
		## computer scissors img
		{ "name" : "scissors_com_img", "type" : "image", "x" : 133-10, "y" : 101-32, "image" : ROOT_PATH + "scissors_com_img.sub", },
		## computer rock img
		{ "name" : "rock_com_img", "type" : "image", "x" : 133-10, "y" : 101-32, "image" : ROOT_PATH + "rock_com_img.sub", },
		## computer paper img
		{ "name" : "paper_com_img", "type" : "image", "x" : 133-10, "y" : 101-32, "image" : ROOT_PATH + "paper_com_img.sub", },
		
		## pc scissors button
		{
			"name" : "pc_scissors_button",
			"type" : "radio_button",
			"x" : 78-10,
			"y" : 216-32,
			"default_image" : ROOT_PATH + "scissors_pc_img_default.sub",
			"over_image"	: ROOT_PATH + "scissors_pc_img_over.sub",
			"down_image"	: ROOT_PATH + "scissors_pc_img_down.sub",			
		},
		## pc rock button
		{
			"name" : "pc_rock_button",
			"type" : "radio_button",
			"x" : 138-10,
			"y" : 216-32,
			"default_image" : ROOT_PATH + "rock_pc_img_default.sub",
			"over_image"	: ROOT_PATH + "rock_pc_img_over.sub",
			"down_image"	: ROOT_PATH + "rock_pc_img_down.sub",			
		},
		## pc paper button
		{
			"name" : "pc_paper_button",
			"type" : "radio_button",
			"x" : 198-10,
			"y" : 216-32,
			"default_image" : ROOT_PATH + "paper_pc_img_default.sub",
			"over_image"	: ROOT_PATH + "paper_pc_img_over.sub",
			"down_image"	: ROOT_PATH + "paper_pc_img_down.sub",			
		},
		
		## turn sign text window
		{
			"name" : "turn_sign_text_window", "type" : "window", "x" : 77-10, "y" : 180-32, "width" : 178, "height" : 21, "style" : ("attach",),
			"children" :
			(
				{"name":"turn_sign_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "turn_sign_text_bg.sub", "style" : ("attach",), },
				{"name":"turn_sign_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_COM_VS_PC, "all_align" : "center", },
			),
		},
		
		## betting name window
		{
			"name" : "betting_name_window", "type" : "window", "x" : 322-10, "y" : 35-32, "width" : 119, "height" : 21, "style" : ("attach",),
			"children" :
			(
				{"name":"betting_name_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "name_bg.sub", "style" : ("attach",), },
				{"name":"betting_name_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_BETTING_TEXT, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		## betting double arrow left button
		{
			"name" : "betting_double_arrow_left",
			"type" : "button",
			"x" : 328-10,
			"y" : 77-32,
			"default_image" : ROOT_PATH + "betting_double_arrow_left_default.sub",
			"over_image"	: ROOT_PATH + "betting_double_arrow_left_over.sub",
			"down_image"	: ROOT_PATH + "betting_double_arrow_left_down.sub",			
		},
		## betting arrow left button
		{
			"name" : "betting_arrow_left",
			"type" : "button",
			"x" : 346-10,
			"y" : 77-32,
			"default_image" : ROOT_PATH + "betting_arrow_left_default.sub",
			"over_image"	: ROOT_PATH + "betting_arrow_left_over.sub",
			"down_image"	: ROOT_PATH + "betting_arrow_left_down.sub",			
		},
		
		## betting number window
		{
			"name" : "betting_count_window", "type" : "window", "x" : 364-10, "y" : 76-32, "width" : 35, "height" : 18, "style" : ("attach",),
			"children" :
			(
				{"name":"betting_count_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "betting_input_bg.sub"},
				{"name":"betting_count_text", "type":"text", "x":0, "y":0, "text": "0", "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		
		## betting arrow right button
		{
			"name" : "betting_arrow_right",
			"type" : "button",
			"x" : 401-10,
			"y" : 77-32,
			"default_image" : ROOT_PATH + "betting_arrow_right_default.sub",
			"over_image"	: ROOT_PATH + "betting_arrow_right_over.sub",
			"down_image"	: ROOT_PATH + "betting_arrow_right_down.sub",			
		},
		## betting double right button
		{
			"name" : "betting_double_arrow_right",
			"type" : "button",
			"x" : 419-10,
			"y" : 77-32,
			"default_image" : ROOT_PATH + "betting_double_arrow_right_default.sub",
			"over_image"	: ROOT_PATH + "betting_double_arrow_right_over.sub",
			"down_image"	: ROOT_PATH + "betting_double_arrow_right_down.sub",			
		},
		
		## challenge ticket bg
		{
			"name" : "challenge_ticket_bg",
			"type" : "image",
			"x" : 322-10,
			"y" : 113-32,
			"image" : ROOT_PATH + "name_value_bg.sub",
		},
		## challenge ticket count title text
		{
			"name" : "challenge_ticket_title_text_window", "type" : "window", "x" : 323-10, "y" : 117-32, "width" : 117, "height" : 12, "style" : ("attach",),
			"children" :
			(						
				{"name":"challenge_ticket_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_ITEM_NAME_TEXT, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		## challenge ticket count text
		{
			"name" : "challenge_ticket_count_text_window", "type" : "window", "x" : 323-10, "y" : 136-32, "width" : 117, "height" : 12, "style" : ("attach",),
			"children" :
			(						
				{"name":"challenge_ticket_count_text", "type":"text", "x":0, "y":0, "text": "200", "all_align" : "center", "color" : 0xFFD2C4AD, },
			),
		},
		
		## yang text bg
		{
			"name" : "yang_text_bg",
			"type" : "image",
			"x" : 322-10,
			"y" : 151-32,
			"image" : ROOT_PATH + "name_value_bg.sub",
		},
		## yang text title
		{
			"name" : "yang_text_title_window", "type" : "window", "x" : 323-10, "y" : 155-32, "width" : 117, "height" : 12, "style" : ("attach",),
			"children" :
			(						
				{"name":"yang_text_title", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_YANG_TEXT, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		## yang text
		{
			"name" : "yang_text_window", "type" : "window", "x" : 323-10, "y" : 174-32, "width" : 117, "height" : 12, "style" : ("attach",),
			"children" :
			(						
				{"name":"yang_text", "type":"text", "x":0, "y":0, "text": "200,000,000", "all_align" : "center", "color" : 0xFFD2C4AD, },
			),
		},
		
		## result bg
		{
			"name" : "result_bg",
			"type" : "image",
			"x" : 325-10,
			"y" : 211-32-11,
			"image" : ROOT_PATH + "result_text_bg.sub",
		},
		## result up text
		{
			"name" : "result_up_text_window", "type" : "window", "x" : 329-10, "y" : 220-32-11, "width" : 105, "height" : 14, "style" : ("attach",),
		},
		## result down text
		{
			"name" : "result_down_text_window", "type" : "window", "x" : 329-10, "y" : 238-32-11, "width" : 105, "height" : 14, "style" : ("attach",),
		},
		## result center text
		{
			"name" : "result_center_text_window", "type" : "window", "x" : 329-10, "y" : 229-32-11, "width" : 105, "height" : 14, "style" : ("attach",),
		},
		
		## challenge button
		{
			"name" : "challenge_button",
			"type" : "button",
			"x" : 326-10,
			"y" : 259-32,
			
			"text" : uiScriptLocale.MINI_GAME_CHALLENGE_BUTTON_TEXT,
			"text_color" : 0xFFD2C4AD,
			
			"default_image" : ROOT_PATH + "challenge_button_default.sub",
			"over_image"	: ROOT_PATH + "challenge_button_over.sub",
			"down_image"	: ROOT_PATH + "challenge_button_down.sub",
		},
		
		## slot edge effect
		{
			"name" : "roulette_effect_window", "type" : "window", "x" : 23-10, "y" : 45-32, "width" : 46, "height" : 46, "style" : ("not_pick",),
			"children" :
			(
				## roulette effect
				{
					"name" : "roulette_effect",
					"type" : "image",

					"x" : 0,
					"y" : 0,

					"image" : ROOT_PATH + "roulette_effect.sub",
				},
			),
		},
		
		## challenge dlg skip button
		{
			"name" : "challenge_dlg_skip_button",
			"type" : "toggle_button",
			"x" : 369-10,
			"y" : 315-32,			
			
			"default_image" : ROOT_PATH + "check_button_default.sub",
			"over_image"	: ROOT_PATH + "check_button_default.sub",
			"down_image"	: ROOT_PATH + "check_button_down.sub",
		},
	),	
}
