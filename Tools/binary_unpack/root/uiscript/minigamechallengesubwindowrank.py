import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/minigame/challenge/"
PUBLIC_PATH = "d:/ymir work/ui/public/"

BOARD_WIDTH		= 434
BOARD_HEIGHT	= 312

MENU_RANK_HEIGHT	= 22
MENU_RANK_GAP		= 3

window = {
	"name" : "MiniGameChallengeRankPage",
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
			"x" : 273,
			"y" : 0,
			"image" : ROOT_PATH + "vertical_line.sub",
		},
		
		## 순위, 이름, 레벨 bg
		{
			"name" : "tab_text_bg",
			"type" : "image",
			"style" : ("attach", ),
			"x" : 13-10,
			"y" : 35-32,
			"image" : ROOT_PATH + "rank_tab_text_bg.sub",
		},
		## 순위 title text
		{
			"name" : "rank_title_text_window", "type" : "window", "x" : 21-10, "y" : 39-32, "width" : 51, "height" : 13, "style" : ("attach",),
			"children" :
			(
				{"name":"rank_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_RANK_TAB_TITLE_RANK, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		## 이름 title text
		{
			"name" : "name_title_text_window", "type" : "window", "x" : 98-10, "y" : 39-32, "width" : 93, "height" : 13, "style" : ("attach",),
			"children" :
			(
				{"name":"name_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_RANK_TAB_TITLE_NAME, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		## 레벨 title text
		{
			"name" : "level_title_text_window", "type" : "window", "x" : 219-10, "y" : 39-32, "width" : 51, "height" : 13, "style" : ("attach",),
			"children" :
			(
				{"name":"level_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_RANK_TAB_TITLE_LEVEL, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		
		## 랭크 보상 확인
		{
			"name" : "reward_text_window", "type" : "window", "x" : 286-10, "y" : 35-32, "width" : 155, "height" : 21, "style" : ("attach",),
			"children" :
			(
				{"name":"reward_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "rank_tab_reward_text_bg.sub", "style" : ("attach",) },
				{"name":"reward_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_RANK_TAB_REWARD_CONFIRM_TEXT, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
				
		## desc board
		{
			"name" : "desc_board_window", "type" : "window", "x" : 289-10, "y" : 58-32, "width" : 149, "height" : 281, "style" : ("attach",),
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
		
		## rank 1~10 bg
		{ "name" : "rank_bg1",		"type" : "image", "style" : ("attach","ltr",), "x" : 15-10, "y" : 58-32+MENU_RANK_HEIGHT*0 + MENU_RANK_GAP*0, "image" : ROOT_PATH + "rank_tab_rank_bg.sub", },
		{ "name" : "rank_bg2",		"type" : "image", "style" : ("attach","ltr",), "x" : 15-10, "y" : 58-32+MENU_RANK_HEIGHT*1 + MENU_RANK_GAP*1, "image" : ROOT_PATH + "rank_tab_rank_bg.sub", },
		{ "name" : "rank_bg3",		"type" : "image", "style" : ("attach","ltr",), "x" : 15-10, "y" : 58-32+MENU_RANK_HEIGHT*2 + MENU_RANK_GAP*2, "image" : ROOT_PATH + "rank_tab_rank_bg.sub", },
		{ "name" : "rank_bg4",		"type" : "image", "style" : ("attach","ltr",), "x" : 15-10, "y" : 58-32+MENU_RANK_HEIGHT*3 + MENU_RANK_GAP*3, "image" : ROOT_PATH + "rank_tab_rank_bg.sub", },
		{ "name" : "rank_bg5",		"type" : "image", "style" : ("attach","ltr",), "x" : 15-10, "y" : 58-32+MENU_RANK_HEIGHT*4 + MENU_RANK_GAP*4, "image" : ROOT_PATH + "rank_tab_rank_bg.sub", },
		{ "name" : "rank_bg6",		"type" : "image", "style" : ("attach","ltr",), "x" : 15-10, "y" : 58-32+MENU_RANK_HEIGHT*5 + MENU_RANK_GAP*5, "image" : ROOT_PATH + "rank_tab_rank_bg.sub", },
		{ "name" : "rank_bg7",		"type" : "image", "style" : ("attach","ltr",), "x" : 15-10, "y" : 58-32+MENU_RANK_HEIGHT*6 + MENU_RANK_GAP*6, "image" : ROOT_PATH + "rank_tab_rank_bg.sub", },
		{ "name" : "rank_bg8",		"type" : "image", "style" : ("attach","ltr",), "x" : 15-10, "y" : 58-32+MENU_RANK_HEIGHT*7 + MENU_RANK_GAP*7, "image" : ROOT_PATH + "rank_tab_rank_bg.sub", },
		{ "name" : "rank_bg9",		"type" : "image", "style" : ("attach","ltr",), "x" : 15-10, "y" : 58-32+MENU_RANK_HEIGHT*8 + MENU_RANK_GAP*8, "image" : ROOT_PATH + "rank_tab_rank_bg.sub", },
		{ "name" : "rank_bg10",		"type" : "image", "style" : ("attach","ltr",), "x" : 15-10, "y" : 58-32+MENU_RANK_HEIGHT*9 + MENU_RANK_GAP*9, "image" : ROOT_PATH + "rank_tab_rank_bg.sub", },
		{ "name" : "dotted_line",	"type" : "image", "style" : ("attach","ltr",), "x" : 149-10, "y" : 306-32, "image" : ROOT_PATH + "boundary_dotted_line.sub", },
		{ "name" : "rank_my",		"type" : "image", "style" : ("attach","ltr",), "x" : 15-10, "y" : 317-32, "image" : ROOT_PATH + "rank_tab_my_rank_bg.sub", },
		
		## rank1
		{ 
			"name" : "rank_text_window1", "type" : "window", "x" : 15+11-10, "y" : 58+2-32, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_text1", "type":"text", "x":0, "y":0, "text": "1", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_name_window1", "type" : "window", "x" : 15+71-10, "y" : 58+2-32, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_name1", "type":"text", "x":0, "y":0, "text": "이름1", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_level_window1", "type" : "window", "x" : 15+209-10, "y" : 60-32, "width" : 42, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_level1", "type":"text", "x":0, "y":0, "text": "111", "all_align" : "center", "color" : 0xFFCEC6B5, },						
			),
		},
		## rank2
		{ 
			"name" : "rank_text_window2", "type" : "window", "x" : 15+11-10, "y" : 60-32+MENU_RANK_HEIGHT*1 + MENU_RANK_GAP*1, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_text2", "type":"text", "x":0, "y":0, "text": "2", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_name_window2", "type" : "window", "x" : 15+71-10, "y" : 60-32+MENU_RANK_HEIGHT*1 + MENU_RANK_GAP*1, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_name2", "type":"text", "x":0, "y":0, "text": "이름2", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_level_window2", "type" : "window", "x" : 15+209-10, "y" : 60-32+MENU_RANK_HEIGHT*1 + MENU_RANK_GAP*1, "width" : 42, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_level2", "type":"text", "x":0, "y":0, "text": "222", "all_align" : "center", "color" : 0xFFCEC6B5, },						
			),
		},
		## rank3
		{ 
			"name" : "rank_text_window3", "type" : "window", "x" : 15+11-10, "y" : 60-32+MENU_RANK_HEIGHT*2 + MENU_RANK_GAP*2, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_text3", "type":"text", "x":0, "y":0, "text": "3", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_name_window3", "type" : "window", "x" : 15+71-10, "y" : 60-32+MENU_RANK_HEIGHT*2 + MENU_RANK_GAP*2, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_name3", "type":"text", "x":0, "y":0, "text": "이름3", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_level_window3", "type" : "window", "x" : 15+209-10, "y" : 60-32+MENU_RANK_HEIGHT*2 + MENU_RANK_GAP*2, "width" : 42, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_level3", "type":"text", "x":0, "y":0, "text": "333", "all_align" : "center", "color" : 0xFFCEC6B5, },						
			),
		},
		## rank4
		{ 
			"name" : "rank_text_window4", "type" : "window", "x" : 15+11-10, "y" : 60-32+MENU_RANK_HEIGHT*3 + MENU_RANK_GAP*3, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_text4", "type":"text", "x":0, "y":0, "text": "4", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_name_window4", "type" : "window", "x" : 15+71-10, "y" : 60-32+MENU_RANK_HEIGHT*3 + MENU_RANK_GAP*3, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_name4", "type":"text", "x":0, "y":0, "text": "이름4", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_level_window4", "type" : "window", "x" : 15+209-10, "y" : 60-32+MENU_RANK_HEIGHT*3 + MENU_RANK_GAP*3, "width" : 42, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_level4", "type":"text", "x":0, "y":0, "text": "444", "all_align" : "center", "color" : 0xFFCEC6B5, },						
			),
		},
		## rank5
		{ 
			"name" : "rank_text_window5", "type" : "window", "x" : 15+11-10, "y" : 60-32+MENU_RANK_HEIGHT*4 + MENU_RANK_GAP*4, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_text5", "type":"text", "x":0, "y":0, "text": "5", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_name_window5", "type" : "window", "x" : 15+71-10, "y" : 60-32+MENU_RANK_HEIGHT*4 + MENU_RANK_GAP*4, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_name5", "type":"text", "x":0, "y":0, "text": "이름5", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_level_window5", "type" : "window", "x" : 15+209-10, "y" : 60-32+MENU_RANK_HEIGHT*4 + MENU_RANK_GAP*4, "width" : 42, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_level5", "type":"text", "x":0, "y":0, "text": "555", "all_align" : "center", "color" : 0xFFCEC6B5, },						
			),
		},
		## rank6
		{ 
			"name" : "rank_text_window6", "type" : "window", "x" : 15+11-10, "y" : 60-32+MENU_RANK_HEIGHT*5 + MENU_RANK_GAP*5, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_text6", "type":"text", "x":0, "y":0, "text": "6", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_name_window6", "type" : "window", "x" : 15+71-10, "y" : 60-32+MENU_RANK_HEIGHT*5 + MENU_RANK_GAP*5, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_name6", "type":"text", "x":0, "y":0, "text": "이름6", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_level_window6", "type" : "window", "x" : 15+209-10, "y" : 60-32+MENU_RANK_HEIGHT*5 + MENU_RANK_GAP*5, "width" : 42, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_level6", "type":"text", "x":0, "y":0, "text": "666", "all_align" : "center", "color" : 0xFFCEC6B5, },						
			),
		},
		## rank7
		{ 
			"name" : "rank_text_window7", "type" : "window", "x" : 15+11-10, "y" : 60-32+MENU_RANK_HEIGHT*6 + MENU_RANK_GAP*6, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_text7", "type":"text", "x":0, "y":0, "text": "7", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_name_window7", "type" : "window", "x" : 15+71-10, "y" : 60-32+MENU_RANK_HEIGHT*6 + MENU_RANK_GAP*6, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_name7", "type":"text", "x":0, "y":0, "text": "이름7", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_level_window7", "type" : "window", "x" : 15+209-10, "y" : 60-32+MENU_RANK_HEIGHT*6 + MENU_RANK_GAP*6, "width" : 42, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_level7", "type":"text", "x":0, "y":0, "text": "777", "all_align" : "center", "color" : 0xFFCEC6B5, },						
			),
		},
		## rank8
		{ 
			"name" : "rank_text_window8", "type" : "window", "x" : 15+11-10, "y" : 60-32+MENU_RANK_HEIGHT*7 + MENU_RANK_GAP*7, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_text8", "type":"text", "x":0, "y":0, "text": "8", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_name_window8", "type" : "window", "x" : 15+71-10, "y" : 60-32+MENU_RANK_HEIGHT*7 + MENU_RANK_GAP*7, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_name8", "type":"text", "x":0, "y":0, "text": "이름8", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_level_window8", "type" : "window", "x" : 15+209-10, "y" : 60-32+MENU_RANK_HEIGHT*7 + MENU_RANK_GAP*7, "width" : 42, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_level8", "type":"text", "x":0, "y":0, "text": "888", "all_align" : "center", "color" : 0xFFCEC6B5, },						
			),
		},
		## rank9
		{ 
			"name" : "rank_text_window9", "type" : "window", "x" : 15+11-10, "y" : 60-32+MENU_RANK_HEIGHT*8 + MENU_RANK_GAP*8, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_text9", "type":"text", "x":0, "y":0, "text": "9", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_name_window9", "type" : "window", "x" : 15+71-10, "y" : 60-32+MENU_RANK_HEIGHT*8 + MENU_RANK_GAP*8, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_name9", "type":"text", "x":0, "y":0, "text": "이름9", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_level_window9", "type" : "window", "x" : 15+209-10, "y" : 60-32+MENU_RANK_HEIGHT*8 + MENU_RANK_GAP*8, "width" : 42, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_level9", "type":"text", "x":0, "y":0, "text": "999", "all_align" : "center", "color" : 0xFFCEC6B5, },						
			),
		},
		## rank10
		{ 
			"name" : "rank_text_window10", "type" : "window", "x" : 15+11-10, "y" : 60-32+MENU_RANK_HEIGHT*9 + MENU_RANK_GAP*9, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_text10", "type":"text", "x":0, "y":0, "text": "10", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_name_window10", "type" : "window", "x" : 15+71-10, "y" : 60-32+MENU_RANK_HEIGHT*9 + MENU_RANK_GAP*9, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_name10", "type":"text", "x":0, "y":0, "text": "이름10", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "rank_level_window10", "type" : "window", "x" : 15+209-10, "y" : 60-32+MENU_RANK_HEIGHT*9 + MENU_RANK_GAP*9, "width" : 42, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "rank_level10", "type":"text", "x":0, "y":0, "text": "1010", "all_align" : "center", "color" : 0xFFCEC6B5, },						
			),
		},
		## my rank
		{ 
			"name" : "my_rank_text_window", "type" : "window", "x" : 15+11-10, "y" : 60-32+MENU_RANK_HEIGHT*10 + MENU_RANK_GAP*9+12, "width" : 44, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "my_rank_text", "type":"text", "x":0, "y":0, "text": "1234", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "my_rank_name_window", "type" : "window", "x" : 15+71-10, "y" : 60-32+MENU_RANK_HEIGHT*10 + MENU_RANK_GAP*9+12, "width" : 115, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "my_rank_name", "type":"text", "x":0, "y":0, "text": "tja3", "all_align" : "center", "color" : 0xFFCEC6B5, },
			),
		},
		{ 
			"name" : "my_rank_level_window", "type" : "window", "x" : 15+209-10, "y" : 60-32+MENU_RANK_HEIGHT*10 + MENU_RANK_GAP*9+12, "width" : 42, "height" : 18, "style" : ("attach", "not_pick"),
			"children" :
			(
				{ "name" : "my_rank_level", "type":"text", "x":0, "y":0, "text": "1", "all_align" : "center", "color" : 0xFFCEC6B5, },						
			),
		},
	),	
}
