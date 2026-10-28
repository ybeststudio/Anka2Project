import uiScriptLocale
import localeInfo

ROOT = "d:/ymir work/ui/minigame/bnw/"
PUBLIC_PATH = "d:/ymir work/ui/public/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"

WINDOW_WIDTH	= 376
WINDOW_HEIGHT	= 336

RIGHT_BOARD_WIDTH	= 100
RIGHT_BOARD_HEIGHT	= 292
RIGHT_BOARD_PATTERN_X_COUNT = (RIGHT_BOARD_WIDTH - 32) / 16
RIGHT_BOARD_PATTERN_Y_COUNT = (RIGHT_BOARD_HEIGHT - 32) / 16

BNW_BOARD_WIDTH		= 250
BNW_BOARD_HEIGHT	= 292

window = {
	"name" : "MiniGameBNWGamePage",
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
			
			"title" : uiScriptLocale.MINI_GAME_BLACK_AND_WHITE_GAME_TITLE,
			
			"children" :
			(
				
				## bg center
				{
					"name" : "bnw_bg2",
					"type" : "image",
					
					"x" : 10,
					"y" : 32+129,
					"image" : ROOT + "bnw_bg_center.sub",
				},
				
				## alarm text top window1
				{ 
					"name" : "alarm_text_top_window1", "type" : "window", "x" : 21, "y" : 161-34-10, "width" : 227, "height" : 17, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "alarm_text_top1", "type":"text", "x":0, "y":0, "text": "", "all_align" : "center", "color" : 0xFFE6E600, },
					),
				},
				## alarm text top window2
				{ 
					"name" : "alarm_text_top_window2", "type" : "window", "x" : 21, "y" : 161-17-10, "width" : 227, "height" : 17, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "alarm_text_top2", "type":"text", "x":0, "y":0, "text": "", "all_align" : "center", "color" : 0xFFE6E600, },
					),
				},
				
				## alarm text center window1
				{ 
					"name" : "alarm_text_center_window1", "type" : "window", "x" : 21, "y" : 161, "width" : 227, "height" : 17, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "alarm_text_center1", "type":"text", "x":0, "y":0, "text": "", "all_align" : "center", "color" : 0xFFE6E600, },
					),
				},
				## alarm text center window2
				{ 
					"name" : "alarm_text_center_window2", "type" : "window", "x" : 21, "y" : 161+17, "width" : 227, "height" : 17, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "alarm_text_center2", "type":"text", "x":0, "y":0, "text": "", "all_align" : "center", "color" : 0xFFE6E600, },
					),
				},
				
				## alarm text bottom window1
				{ 
					"name" : "alarm_text_bottom_window1", "type" : "window", "x" : 21, "y" : 161+34+10, "width" : 227, "height" : 17, "style" : ("attach", "not_pick"),
				},
				## alarm text bottom window2
				{ 
					"name" : "alarm_text_bottom_window2", "type" : "window", "x" : 21, "y" : 161+34+17+10, "width" : 227, "height" : 17, "style" : ("attach", "not_pick"),
				},
				
				## bg top
				{
					"name" : "bnw_bg1",
					"type" : "image",
					
					"x" : 10,
					"y" : 32,
					"image" : ROOT + "bnw_bg_top.sub",
				},
				
				## bg bottom
				{
					"name" : "bnw_bg3",
					"type" : "image",
					
					"x" : 10,
					"y" : 32+129+34,
					"image" : ROOT + "bnw_bg_bottom.sub",
				},
				
				## right board window
				{
					"name" : "right_board_window",
					"type" : "window",
					
					"style" : ("attach", "ltr",),

					"x" : 264,
					"y" : 32,
					
					"width" : RIGHT_BOARD_WIDTH,
					"height" : RIGHT_BOARD_HEIGHT,
					
					"children" :
					(
						## LeftTop 1
						{
							"name" : "left_top_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						## RightTop 2
						{
							"name" : "right_top_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : RIGHT_BOARD_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "left_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : RIGHT_BOARD_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "right_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : RIGHT_BOARD_WIDTH - 16,
							"y" : RIGHT_BOARD_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						## topcenterImg 5
						{
							"name" : "top_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, RIGHT_BOARD_PATTERN_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "left_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, RIGHT_BOARD_PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "right_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : RIGHT_BOARD_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, RIGHT_BOARD_PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "bottom_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : RIGHT_BOARD_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, RIGHT_BOARD_PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, RIGHT_BOARD_PATTERN_X_COUNT, RIGHT_BOARD_PATTERN_Y_COUNT),
						},
						
						## player2 name
						{ 
							"name" : "player2_name_window", "type" : "window", "x" : 3, "y" : 3, "width" : 94, "height" : 19, "style" : ("attach", "not_pick"),
							"children" :
							(
								{ "name" : "player2_name_bg", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT + "name_bg.sub",	},
								{ "name" : "player2_name_text", "type":"text", "x":0, "y":0, "text": "", "all_align" : "center", "color" : 0xFFD4D4D4, },
							),
						},
						## player2 score
						{ 
							"name" : "player2_score_window", "type" : "window", "x" : 3, "y" : 22, "width" : 94, "height" : 35, "style" : ("attach", "not_pick"),
							"children" :
							(
								{ "name" : "player2_score_bg", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT + "score_bg.sub", },
								{ "name" : "player2_score_text", "type":"text", "x":0, "y":0, "text": "0", "all_align" : "center", "color" : 0xFFD4D4D4, "fontname" : localeInfo.MINI_GAME_BNW_FONT, },
							),
						},
						
						## player2 cur card
						{ "name" : "player2_cur_card_bg", "type" : "image", "x" : 22, "y" : 69, "image" : ROOT + "cur_card_slot.sub", },
						{ 
							"name" : "player2_cur_card_window", "type" : "window", "x" : 30, "y" : 77, "width" : 41, "height" : 51, "style" : ("attach", "not_pick"),
							"children" :
							(
								{ "name" : "player2_cur_card", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT + "back_black.sub", },
								{ "name" : "player2_cur_card_over", "type" : "image", "x" : 0,  "y" : 0, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
							),
						},
						
						## player1 name
						{
							"name" : "player1_name_window", "type" : "window", "x" : 3, "y" : 147, "width" : 94, "height" : 19, "style" : ("attach", "not_pick"),
							"children" :
							(
								{ "name" : "player1_name_bg", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT + "name_bg.sub",	},
								{ "name" : "player1_name_text", "type":"text", "x":0, "y":0, "text": "", "all_align" : "center", "color" : 0xFFEEA900, },
							),
						},
						## player1 score
						{ 
							"name" : "player1_score_window", "type" : "window", "x" : 3, "y" : 166, "width" : 94, "height" : 35, "style" : ("attach", "not_pick"),
							"children" :
							(
								{ "name" : "player1_score_bg", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT + "score_bg.sub", },
								{ "name" : "player1_score_text", "type":"text", "x":0, "y":0, "text": "0", "all_align" : "center", "color" : 0xFFEEA900, "fontname" : localeInfo.MINI_GAME_BNW_FONT, },
							),
						},
						
						## player1 cur card
						{ "name" : "player1_cur_card_bg", "type" : "image", "x" : 22, "y" : 212, "image" : ROOT + "cur_card_slot.sub", },
						{ 
							"name" : "player1_cur_card_window", "type" : "window", "x" : 30, "y" : 220, "width" : 41, "height" : 51, "style" : ("attach", "not_pick"),
							"children" :
							(
								{ "name" : "player1_cur_card", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT + "back_white.sub", },
								{ "name" : "player1_cur_card_over", "type" : "image", "x" : 0,  "y" : 0, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
							),
						},
					),
				},
				
				## player1 card
				{ "name" : "player1_card0", "type" : "button", "x" : 30, "y" : 263, "default_image" : ROOT + "number0.sub", "over_image" : ROOT + "number0.sub", "down_image" : ROOT + "number0.sub",  },			
				{ "name" : "player1_card1", "type" : "button", "x" : 51, "y" : 211, "default_image" : ROOT + "number1.sub", "over_image" : ROOT + "number1.sub", "down_image" : ROOT + "number1.sub",  },
				{ "name" : "player1_card2", "type" : "button", "x" : 72, "y" : 263, "default_image" : ROOT + "number2.sub", "over_image" : ROOT + "number2.sub", "down_image" : ROOT + "number2.sub",  },
				{ "name" : "player1_card3", "type" : "button", "x" : 93, "y" : 211, "default_image" : ROOT + "number3.sub", "over_image" : ROOT + "number3.sub", "down_image" : ROOT + "number3.sub",  },
				{ "name" : "player1_card4", "type" : "button", "x" : 114, "y" : 263, "default_image" : ROOT + "number4.sub", "over_image" : ROOT + "number4.sub", "down_image" : ROOT + "number4.sub", },
				{ "name" : "player1_card5", "type" : "button", "x" : 135, "y" : 211, "default_image" : ROOT + "number5.sub", "over_image" : ROOT + "number5.sub", "down_image" : ROOT + "number5.sub", },
				{ "name" : "player1_card6", "type" : "button", "x" : 156, "y" : 263, "default_image" : ROOT + "number6.sub", "over_image" : ROOT + "number6.sub", "down_image" : ROOT + "number6.sub", },
				{ "name" : "player1_card7", "type" : "button", "x" : 177, "y" : 211, "default_image" : ROOT + "number7.sub", "over_image" : ROOT + "number7.sub", "down_image" : ROOT + "number7.sub", },
				{ "name" : "player1_card8", "type" : "button", "x" : 198, "y" : 263, "default_image" : ROOT + "number8.sub", "over_image" : ROOT + "number8.sub", "down_image" : ROOT + "number8.sub", },
				## player1 card over
				{ "name" : "player1_card0_over", "type" : "image", "x" : 30,  "y" : 263, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card1_over", "type" : "image", "x" : 51,  "y" : 211, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card2_over", "type" : "image", "x" : 72,  "y" : 263, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card3_over", "type" : "image", "x" : 93,  "y" : 211, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card4_over", "type" : "image", "x" : 114, "y" : 263, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card5_over", "type" : "image", "x" : 135, "y" : 211, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card6_over", "type" : "image", "x" : 156, "y" : 263, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card7_over", "type" : "image", "x" : 177, "y" : 211, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card8_over", "type" : "image", "x" : 198, "y" : 263, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				## player1 cross img
				{ "name" : "player1_card0_cross", "type" : "image", "x" : 30,  "y" : 263, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },			
				{ "name" : "player1_card1_cross", "type" : "image", "x" : 51,  "y" : 211, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card2_cross", "type" : "image", "x" : 72,  "y" : 263, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card3_cross", "type" : "image", "x" : 93,  "y" : 211, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card4_cross", "type" : "image", "x" : 114, "y" : 263, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card5_cross", "type" : "image", "x" : 135, "y" : 211, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card6_cross", "type" : "image", "x" : 156, "y" : 263, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card7_cross", "type" : "image", "x" : 177, "y" : 211, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player1_card8_cross", "type" : "image", "x" : 198, "y" : 263, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				
							
				## player2 card
				{ "name" : "player2_card0", "type" : "button", "x" : 30, "y" : 42, "default_image" : ROOT + "back_black.sub", "over_image" : ROOT + "back_black.sub", "down_image" : ROOT + "back_black.sub",  },
				{ "name" : "player2_card1", "type" : "button", "x" : 51, "y" : 94, "default_image" : ROOT + "back_white.sub", "over_image" : ROOT + "back_white.sub", "down_image" : ROOT + "back_white.sub",  },
				{ "name" : "player2_card2", "type" : "button", "x" : 72, "y" : 42, "default_image" : ROOT + "back_black.sub", "over_image" : ROOT + "back_black.sub", "down_image" : ROOT + "back_black.sub",  },
				{ "name" : "player2_card3", "type" : "button", "x" : 93, "y" : 94, "default_image" : ROOT + "back_white.sub", "over_image" : ROOT + "back_white.sub", "down_image" : ROOT + "back_white.sub",  },
				{ "name" : "player2_card4", "type" : "button", "x" : 114, "y" : 42, "default_image" : ROOT + "back_black.sub", "over_image" : ROOT + "back_black.sub", "down_image" : ROOT + "back_black.sub", },
				{ "name" : "player2_card5", "type" : "button", "x" : 135, "y" : 94, "default_image" : ROOT + "back_white.sub", "over_image" : ROOT + "back_white.sub", "down_image" : ROOT + "back_white.sub", },
				{ "name" : "player2_card6", "type" : "button", "x" : 156, "y" : 42, "default_image" : ROOT + "back_black.sub", "over_image" : ROOT + "back_black.sub", "down_image" : ROOT + "back_black.sub", },
				{ "name" : "player2_card7", "type" : "button", "x" : 177, "y" : 94, "default_image" : ROOT + "back_white.sub", "over_image" : ROOT + "back_white.sub", "down_image" : ROOT + "back_white.sub", },
				{ "name" : "player2_card8", "type" : "button", "x" : 198, "y" : 42, "default_image" : ROOT + "back_black.sub", "over_image" : ROOT + "back_black.sub", "down_image" : ROOT + "back_black.sub", },
				
				## player2 card over
				{ "name" : "player2_card0_over", "type" : "image", "x" : 30,  "y" : 42, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card1_over", "type" : "image", "x" : 51,  "y" : 94, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card2_over", "type" : "image", "x" : 72,  "y" : 42, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card3_over", "type" : "image", "x" : 93,  "y" : 94, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card4_over", "type" : "image", "x" : 114, "y" : 42, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card5_over", "type" : "image", "x" : 135, "y" : 94, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card6_over", "type" : "image", "x" : 156, "y" : 42, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card7_over", "type" : "image", "x" : 177, "y" : 94, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card8_over", "type" : "image", "x" : 198, "y" : 42, "image" : ROOT + "card_mouse_over.sub", "style" : ("not_pick",), },
				## player2 cross img
				{ "name" : "player2_card0_cross", "type" : "image", "x" : 30,  "y" : 42, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card1_cross", "type" : "image", "x" : 51,  "y" : 94, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card2_cross", "type" : "image", "x" : 72,  "y" : 42, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card3_cross", "type" : "image", "x" : 93,  "y" : 94, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card4_cross", "type" : "image", "x" : 114, "y" : 42, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card5_cross", "type" : "image", "x" : 135, "y" : 94, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card6_cross", "type" : "image", "x" : 156, "y" : 42, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card7_cross", "type" : "image", "x" : 177, "y" : 94, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },
				{ "name" : "player2_card8_cross", "type" : "image", "x" : 198, "y" : 42, "image" : ROOT + "cross_mark.sub", "style" : ("not_pick",), },

				## card move layer
				{
					"name" : "move_card_layer_window",
					"type" : "window",
					"style" : ("attach", "ltr", "not_pick", ),
					
					"x" : 10,
					"y" : 32,					
					
					"width" : BNW_BOARD_WIDTH + 4 + RIGHT_BOARD_WIDTH,
					"height" : RIGHT_BOARD_HEIGHT,
				},
			),
		},
	),	
}
