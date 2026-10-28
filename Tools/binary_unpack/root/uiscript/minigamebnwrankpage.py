import uiScriptLocale

ROOT = "d:/ymir work/ui/minigame/bnw/"
PUBLIC_PATH = "d:/ymir work/ui/public/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"

WINDOW_WIDTH		= 294
WINDOW_HEIGHT		= 364

MENU_RANK_HEIGHT	= 21
MENU_RANK_GAP		= 3

BOARD_WIDTH			= 272
BOARD_HEIGHT		= 296
BOARD_PATTERN_X_COUNT = ((BOARD_WIDTH - 32) / 16)-1
BOARD_PATTERN_Y_COUNT = (BOARD_HEIGHT - 32) / 16



window = {
	"name" : "MiniGameBNWRankPage",
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
				## board window
				{
					"name" : "board_window",
					"type" : "window",
					
					"style" : ("attach", "ltr",),

					"x" : 10,
					"y" : 33,
					
					"width" : BOARD_WIDTH,
					"height" : BOARD_HEIGHT,
					
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
							
							"x" : BOARD_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "left_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : BOARD_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "right_bottom_img",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : BOARD_WIDTH - 16,
							"y" : BOARD_HEIGHT - 16,
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
							"rect" : (0.0, 0.0, BOARD_PATTERN_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "left_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, BOARD_PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "right_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : BOARD_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, BOARD_PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "bottom_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : BOARD_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, BOARD_PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, BOARD_PATTERN_X_COUNT, BOARD_PATTERN_Y_COUNT),
						},
					),
				},
				
				## menu bg
				{ "name" : "menu_bg", "type" : "image", "style" : ("ltr",), "x" : 13, "y" : 36, "image" : ROOT + "rank_memu_bg.sub", },
				
				{ 
					"name" : "menu_rank_window", "type" : "window", "x" : 15, "y" : 38, "width" : 60, "height" : 17, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "menu_rank_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.BNW_RANK_UI_RANK, "all_align" : "center", },
					),
				},
				{ 
					"name" : "menu_name_window", "type" : "window", "x" : 81, "y" : 38, "width" : 115, "height" : 17, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "menu_name_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.BNW_RANK_UI_NAME, "all_align" : "center", },
					),
				},
				{ 
					"name" : "menu_donate_count_window", "type" : "window", "x" : 201, "y" : 38, "width" : 72, "height" : 17, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "menu_donate_count_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.BNW_RANK_UI_COUNT, "all_align" : "center", },
					),
				},
				
				## rank 1~10 bg
				{ "name" : "rank_bg1",	"type" : "image", "style" : ("ltr",), "x" : 15, "y" : 60+MENU_RANK_HEIGHT*0 + MENU_RANK_GAP*0, "image" : ROOT + "rank_bg.sub", },
				{ "name" : "rank_bg2",	"type" : "image", "style" : ("ltr",), "x" : 15, "y" : 60+MENU_RANK_HEIGHT*1 + MENU_RANK_GAP*1, "image" : ROOT + "rank_bg.sub", },
				{ "name" : "rank_bg3",	"type" : "image", "style" : ("ltr",), "x" : 15, "y" : 60+MENU_RANK_HEIGHT*2 + MENU_RANK_GAP*2, "image" : ROOT + "rank_bg.sub", },
				{ "name" : "rank_bg4",	"type" : "image", "style" : ("ltr",), "x" : 15, "y" : 60+MENU_RANK_HEIGHT*3 + MENU_RANK_GAP*3, "image" : ROOT + "rank_bg.sub", },
				{ "name" : "rank_bg5",	"type" : "image", "style" : ("ltr",), "x" : 15, "y" : 60+MENU_RANK_HEIGHT*4 + MENU_RANK_GAP*4, "image" : ROOT + "rank_bg.sub", },
				{ "name" : "rank_bg6",	"type" : "image", "style" : ("ltr",), "x" : 15, "y" : 60+MENU_RANK_HEIGHT*5 + MENU_RANK_GAP*5, "image" : ROOT + "rank_bg.sub", },
				{ "name" : "rank_bg7",	"type" : "image", "style" : ("ltr",), "x" : 15, "y" : 60+MENU_RANK_HEIGHT*6 + MENU_RANK_GAP*6, "image" : ROOT + "rank_bg.sub", },
				{ "name" : "rank_bg8",	"type" : "image", "style" : ("ltr",), "x" : 15, "y" : 60+MENU_RANK_HEIGHT*7 + MENU_RANK_GAP*7, "image" : ROOT + "rank_bg.sub", },
				{ "name" : "rank_bg9",	"type" : "image", "style" : ("ltr",), "x" : 15, "y" : 60+MENU_RANK_HEIGHT*8 + MENU_RANK_GAP*8, "image" : ROOT + "rank_bg.sub", },
				{ "name" : "rank_bg10",	"type" : "image", "style" : ("ltr",), "x" : 15, "y" : 60+MENU_RANK_HEIGHT*9 + MENU_RANK_GAP*9, "image" : ROOT + "rank_bg.sub", },
				{ "name" : "rank_my",	"type" : "image", "style" : ("ltr",), "x" : 15, "y" : 302, "image" : ROOT + "rank_my.sub", },
				
				## rank1
				{ 
					"name" : "rank1_text_window", "type" : "window", "x" : 15+11, "y" : 60+1, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank1_text", "type":"text", "x":0, "y":0, "text": "1", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank1_name_window", "type" : "window", "x" : 15+66, "y" : 60+1, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank1_name", "type":"text", "x":0, "y":0, "text": "이름1", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank1_donate_count_window", "type" : "window", "x" : 15+199, "y" : 60+1, "width" : 43, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank1_donate_count_text", "type":"text", "x":0, "y":0, "text": "111", "all_align" : "center", },
					),
				},
				
				## rank2
				{ 
					"name" : "rank2_text_window", "type" : "window", "x" : 15+11, "y" : 60+MENU_RANK_HEIGHT*1 + MENU_RANK_GAP*1+1, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank2_text", "type":"text", "x":0, "y":0, "text": "2", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank2_name_window", "type" : "window", "x" : 15+66, "y" : 60+MENU_RANK_HEIGHT*1 + MENU_RANK_GAP*1+1, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank2_name", "type":"text", "x":0, "y":0, "text": "이름2", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank2_donate_count_window", "type" : "window", "x" : 15+199, "y" : 60+MENU_RANK_HEIGHT*1 + MENU_RANK_GAP*1+1, "width" : 43, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank2_donate_count_text", "type":"text", "x":0, "y":0, "text": "222", "all_align" : "center", },
					),
				},
				
				## rank3
				{ 
					"name" : "rank3_text_window", "type" : "window", "x" : 15+11, "y" : 60+MENU_RANK_HEIGHT*2 + MENU_RANK_GAP*2+1, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank3_text", "type":"text", "x":0, "y":0, "text": "3", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank3_name_window", "type" : "window", "x" : 15+66, "y" : 60+MENU_RANK_HEIGHT*2 + MENU_RANK_GAP*2+1, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank3_name", "type":"text", "x":0, "y":0, "text": "이름3", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank3_donate_count_window", "type" : "window", "x" : 15+199, "y" : 60+MENU_RANK_HEIGHT*2 + MENU_RANK_GAP*2+1, "width" : 43, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank3_donate_count_text", "type":"text", "x":0, "y":0, "text": "333", "all_align" : "center", },
					),
				},
				
				## rank4
				{ 
					"name" : "rank4_text_window", "type" : "window", "x" : 15+11, "y" : 60+MENU_RANK_HEIGHT*3 + MENU_RANK_GAP*3+1, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank4_text", "type":"text", "x":0, "y":0, "text": "4", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank4_name_window", "type" : "window", "x" : 15+66, "y" : 60+MENU_RANK_HEIGHT*3 + MENU_RANK_GAP*3+1, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank4_name", "type":"text", "x":0, "y":0, "text": "이름4", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank4_donate_count_window", "type" : "window", "x" : 15+199, "y" : 60+MENU_RANK_HEIGHT*3 + MENU_RANK_GAP*3+1, "width" : 43, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank4_donate_count_text", "type":"text", "x":0, "y":0, "text": "444", "all_align" : "center", },
					),
				},
				
				## rank5
				{ 
					"name" : "rank5_text_window", "type" : "window", "x" : 15+11, "y" : 60+MENU_RANK_HEIGHT*4 + MENU_RANK_GAP*4+1, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank5_text", "type":"text", "x":0, "y":0, "text": "5", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank5_name_window", "type" : "window", "x" : 15+66, "y" : 60+MENU_RANK_HEIGHT*4 + MENU_RANK_GAP*4+1, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank5_name", "type":"text", "x":0, "y":0, "text": "이름5", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank5_donate_count_window", "type" : "window", "x" : 15+199, "y" : 60+MENU_RANK_HEIGHT*4 + MENU_RANK_GAP*4+1, "width" : 43, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank5_donate_count_text", "type":"text", "x":0, "y":0, "text": "555", "all_align" : "center", },
					),
				},
				
				## rank6
				{ 
					"name" : "rank6_text_window", "type" : "window", "x" : 15+11, "y" : 60+MENU_RANK_HEIGHT*5 + MENU_RANK_GAP*5+1, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank6_text", "type":"text", "x":0, "y":0, "text": "6", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank6_name_window", "type" : "window", "x" : 15+66, "y" : 60+MENU_RANK_HEIGHT*5 + MENU_RANK_GAP*5+1, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank6_name", "type":"text", "x":0, "y":0, "text": "이름6", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank6_donate_count_window", "type" : "window", "x" : 15+199, "y" : 60+MENU_RANK_HEIGHT*5 + MENU_RANK_GAP*5+1, "width" : 43, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank6_donate_count_text", "type":"text", "x":0, "y":0, "text": "666", "all_align" : "center", },
					),
				},
				
				## rank7
				{ 
					"name" : "rank7_text_window", "type" : "window", "x" : 15+11, "y" : 60+MENU_RANK_HEIGHT*6 + MENU_RANK_GAP*6+1, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank7_text", "type":"text", "x":0, "y":0, "text": "7", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank7_name_window", "type" : "window", "x" : 15+66, "y" : 60+MENU_RANK_HEIGHT*6 + MENU_RANK_GAP*6+1, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank7_name", "type":"text", "x":0, "y":0, "text": "이름7", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank7_donate_count_window", "type" : "window", "x" : 15+199, "y" : 60+MENU_RANK_HEIGHT*6 + MENU_RANK_GAP*6+1, "width" : 43, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank7_donate_count_text", "type":"text", "x":0, "y":0, "text": "777", "all_align" : "center", },
					),
				},
				
				## rank8
				{ 
					"name" : "rank8_text_window", "type" : "window", "x" : 15+11, "y" : 60+MENU_RANK_HEIGHT*7 + MENU_RANK_GAP*7+1, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank8_text", "type":"text", "x":0, "y":0, "text": "8", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank8_name_window", "type" : "window", "x" : 15+66, "y" : 60+MENU_RANK_HEIGHT*7 + MENU_RANK_GAP*7+1, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank8_name", "type":"text", "x":0, "y":0, "text": "이름8", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank8_donate_count_window", "type" : "window", "x" : 15+199, "y" : 60+MENU_RANK_HEIGHT*7 + MENU_RANK_GAP*7+1, "width" : 43, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank8_donate_count_text", "type":"text", "x":0, "y":0, "text": "888", "all_align" : "center", },
					),
				},
				
				## rank9
				{ 
					"name" : "rank9_text_window", "type" : "window", "x" : 15+11, "y" : 60+MENU_RANK_HEIGHT*8 + MENU_RANK_GAP*8+1, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank9_text", "type":"text", "x":0, "y":0, "text": "9", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank9_name_window", "type" : "window", "x" : 15+66, "y" : 60+MENU_RANK_HEIGHT*8 + MENU_RANK_GAP*8+1, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank9_name", "type":"text", "x":0, "y":0, "text": "이름9", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank9_donate_count_window", "type" : "window", "x" : 15+199, "y" : 60+MENU_RANK_HEIGHT*8 + MENU_RANK_GAP*8+1, "width" : 43, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank9_donate_count_text", "type":"text", "x":0, "y":0, "text": "999", "all_align" : "center", },
					),
				},
				
				## rank10
				{ 
					"name" : "rank10_text_window", "type" : "window", "x" : 15+11, "y" : 60+MENU_RANK_HEIGHT*9 + MENU_RANK_GAP*9+1, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank10_text", "type":"text", "x":0, "y":0, "text": "10", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank10_name_window", "type" : "window", "x" : 15+66, "y" : 60+MENU_RANK_HEIGHT*9 + MENU_RANK_GAP*9+1, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank10_name", "type":"text", "x":0, "y":0, "text": "이름10", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank10_donate_count_window", "type" : "window", "x" : 15+199, "y" : 60+MENU_RANK_HEIGHT*9 + MENU_RANK_GAP*9+1, "width" : 43, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank10_donate_count_text", "type":"text", "x":0, "y":0, "text": "10", "all_align" : "center", },
					),
				},
				
				## rank my
				{ 
					"name" : "rank_my_text_window", "type" : "window", "x" : 15+11, "y" : 60+MENU_RANK_HEIGHT*10 + MENU_RANK_GAP*9+1+5, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank_my_text", "type":"text", "x":0, "y":0, "text": "100", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank_my_name_window", "type" : "window", "x" : 15+66, "y" : 60+MENU_RANK_HEIGHT*10 + MENU_RANK_GAP*9+1+5, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank_my_name", "type":"text", "x":0, "y":0, "text": "내이름", "all_align" : "center", },
					),
				},
				{ 
					"name" : "rank_my_donate_count_window", "type" : "window", "x" : 15+199, "y" : 60+MENU_RANK_HEIGHT*10 + MENU_RANK_GAP*9+1+5, "width" : 43, "height" : 18, "style" : ("attach", "not_pick"),
					"children" :
					(
						{ "name" : "rank_my_donate_count_text", "type":"text", "x":0, "y":0, "text": "0", "all_align" : "center", },
					),
				},
				
				## donate button
				{
					"name" : "donate_button",
					"type" : "button",
					
					"x" : 194,
					"y" : 332,
					
					"text" : uiScriptLocale.MINI_GAME_BLACK_AND_WHITE_DONATE,					
					
					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
			),
		},
	),	
}
