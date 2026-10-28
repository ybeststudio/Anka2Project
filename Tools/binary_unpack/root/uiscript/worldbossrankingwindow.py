import uiScriptLocale

PUBLIC_PATH		= "d:/ymir work/ui/public/"
PATTERN_PATH	= "d:/ymir work/ui/pattern/"
ROOT_PATH		= "d:/ymir work/ui/minigame/world_boss/"
PAGE_BUTTON_PATH = "d:/ymir work/ui/privatesearch/"

WINDOW_WIDTH	= 400
WINDOW_HEIGHT	= 374

MAIN_WINDOW_WIDTH	= 378
MAIN_WINDOW_HEIGHT	= 330
MAIN_WINDOW_PATTERN_X_COUNT = (MAIN_WINDOW_WIDTH - 32) / 16
MAIN_WINDOW_PATTERN_Y_COUNT = (MAIN_WINDOW_HEIGHT - 32) / 16

window = {
	"name" : "WorldBossRankingWindow",
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
			"title"		: uiScriptLocale.WORLD_BOSS_RANK,
			
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
							"name" : "DescWindowLeftTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						
						## RightTop 2
						{
							"name" : "DescWindowRightTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						
						## LeftBottom 3
						{
							"name" : "DescWindowLeftBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						
						## RightBottom 4
						{
							"name" : "ListWindowRightBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						
						## topcenterImg 5
						{
							"name" : "DescWindowTopCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
						},
						
						## leftcenterImg 6
						{
							"name" : "DescWindowLeftCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, MAIN_WINDOW_PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "DescWindowRightCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, MAIN_WINDOW_PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "DescWindowBottomCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "DescWindowCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, MAIN_WINDOW_PATTERN_Y_COUNT),
						},
					],
				},
				
				## 컬럼 이름
				{
					"name" : "ranking_menu_img",
					"type" : "expanded_image",
					"x" : 13,
					"y" : 35,
					"width" : 372,
					"height" : 21,
					"image" : ROOT_PATH + "ranking_menu.sub",
					"children" :
					(
						## Text
						{
							"type" : "window", "x" : 18, "y" : 4, "width" : 21, "height" : 11,
							
							"children":
							[
								{ "name" : "rank_column_rank", "type" : "text", "style" : ("ltr",), "x" : 0, "y" : 0, "text" : uiScriptLocale.WORLD_BOSS_RANK_RANK, "r":0.749, "g":0.718, "b":0.675, "a":1.0, "text_horizontal_align" : "center"},
							],
						},
						
						{
							"type" : "window", "x" : 101, "y" : 4, "width" : 22, "height" : 11,
							
							"children":
							[
								{ "name" : "rank_column_name", "type" : "text", "style" : ("ltr",), "x" : 0, "y" : 0, "text" : uiScriptLocale.WORLD_BOSS_RANK_NAME, "r":0.749, "g":0.718, "b":0.675, "a":1.0, "text_horizontal_align" : "center"},
							],
						},
						
						{
							"type" : "window", "x" : 210, "y" : 4, "width" : 22, "height" : 11,
							
							"children":
							[
								{ "name" : "rank_column_guild_name", "type" : "text", "style" : ("ltr",), "x" : 0,	"y" : 0, "text" : uiScriptLocale.WORLD_BOSS_RANK_GUILD_NAME, "r":0.749, "g":0.718, "b":0.675, "a":1.0, "text_horizontal_align" : "center"},
							],
						},
						
						{
							"type" : "window", "x" : 281, "y" : 4, "width" : 23, "height" : 11,
							
							"children":
							[
								{ "name" : "rank_column_empire", "type" : "text", "style" : ("ltr",), "x" : 0, "y" : 0,	"text" : uiScriptLocale.WORLD_BOSS_RANK_EMPIRE, "r":0.749, "g":0.718, "b":0.675, "a":1.0, "text_horizontal_align" : "center"},
							],
						},
						
						{
							"type" : "window", "x" : 331, "y" : 4, "width" : 23, "height" : 11,
							
							"children":
							[
								{ "name" : "rank_column_tier", "type" : "text", "style" : ("ltr",), "x" : 0, "y" : 0, "text" : uiScriptLocale.WORLD_BOSS_RANK_TIER, "r":0.749, "g":0.718, "b":0.675, "a":1.0, "text_horizontal_align" : "center"},
							],
						}
					),
				},
				
				{
					"name"		: "rank_item_list",
					"type"		: "listboxex",
					"x"			: 15,
					"y"			: 58,
					"width"		: 353,
					"height"	: 265,
				},
				
				# 페이지 버튼
				{
					"name" : "first_prev_button", "type" : "button",
					"x" : 71 - 10, "y" : 302,

					"default_image" : PAGE_BUTTON_PATH + "private_first_prev_btn_01.sub",
					"over_image" 	: PAGE_BUTTON_PATH + "private_first_prev_btn_02.sub",
					"down_image" 	: PAGE_BUTTON_PATH + "private_first_prev_btn_01.sub",
				},
				{
					"name" : "prev_button", "type" : "button",
					"x" : 101 - 10, "y" : 302,

					"default_image" : PAGE_BUTTON_PATH + "private_prev_btn_01.sub",
					"over_image" 	: PAGE_BUTTON_PATH + "private_prev_btn_02.sub",
					"down_image" 	: PAGE_BUTTON_PATH + "private_prev_btn_01.sub",
				},
				{
					"name" : "page1_button", "type" : "button",
					"x" : 126 - 10, "y" : 300,

					"text" : "1",

					"default_image" : PAGE_BUTTON_PATH + "private_pagenumber_00.sub",
					"over_image" 	: PAGE_BUTTON_PATH + "private_pagenumber_01.sub",
					"down_image" 	: PAGE_BUTTON_PATH + "private_pagenumber_02.sub",
				},
				{
					"name" : "page2_button", "type" : "button",
					"x" : 161 - 10, "y" : 300,

					"text" : "2",

					"default_image" : PAGE_BUTTON_PATH + "private_pagenumber_00.sub",
					"over_image" 	: PAGE_BUTTON_PATH + "private_pagenumber_01.sub",
					"down_image" 	: PAGE_BUTTON_PATH + "private_pagenumber_02.sub",
				},
				{
					"name" : "page3_button", "type" : "button",
					"x" : 196 - 10, "y" : 300,
					
					"text" : "3",

					"default_image" : PAGE_BUTTON_PATH + "private_pagenumber_00.sub",
					"over_image" 	: PAGE_BUTTON_PATH + "private_pagenumber_01.sub",
					"down_image" 	: PAGE_BUTTON_PATH + "private_pagenumber_02.sub",
				},
				{
					"name" : "page4_button", "type" : "button",
					"x" : 231 - 10, "y" : 300,

					"text" : "4",

					"default_image" : PAGE_BUTTON_PATH + "private_pagenumber_00.sub",
					"over_image" 	: PAGE_BUTTON_PATH + "private_pagenumber_01.sub",
					"down_image" 	: PAGE_BUTTON_PATH + "private_pagenumber_02.sub",
				},
				{
					"name" : "page5_button", "type" : "button",
					"x" : 266 - 10, "y" : 300,

					"text" : "5",

					"default_image" : PAGE_BUTTON_PATH + "private_pagenumber_00.sub",
					"over_image" 	: PAGE_BUTTON_PATH + "private_pagenumber_01.sub",
					"down_image" 	: PAGE_BUTTON_PATH + "private_pagenumber_02.sub",
				},
				{
					"name" : "next_button", "type" : "button",
					"x" : 314 - 10, "y" : 302,

					"default_image" : PAGE_BUTTON_PATH + "private_next_btn_01.sub",
					"over_image" 	: PAGE_BUTTON_PATH + "private_next_btn_02.sub",
					"down_image" 	: PAGE_BUTTON_PATH + "private_next_btn_01.sub",
				},
				{
					"name" : "last_next_button", "type" : "button",
					"x" : 344 - 10, "y" : 302,

					"default_image" : PAGE_BUTTON_PATH + "private_last_next_btn_01.sub",
					"over_image" 	: PAGE_BUTTON_PATH + "private_last_next_btn_02.sub",
					"down_image" 	: PAGE_BUTTON_PATH + "private_last_next_btn_01.sub",
				},
				
				###...
				{
					"name" : "ranking_dotdotdot_img",
					"type" : "expanded_image",
					"x" : 198,
					"y" : 324,
					"image" : ROOT_PATH + "ranking_dotdotdot.sub",
				},
				
				### 나의랭킹
				{
					"name" : "my_ranking_item",
					"type" : "expanded_image",
					"x" : 15,
					"y" : 336,
					"width" : 368,
					"height" : 21,
					"image" : ROOT_PATH + "ranking_my_rank_item.sub",
					
					"children" :
					(
						## Text
						{
							"name" : "my_rank_rank_window", "type" : "window", "x" : 6, "y" : 4, "width" : 42, "height" : 11,
							
							"children" :
							[
								{ "name" : "my_rank_rank_text", "type" : "text", "style" : ("ltr",), "x" : 2, "y" : 0, "text" : "", "r":0.812, "g":0.749, "b":0.651, "a":1.0, "horizontal_align" : "right", "text_horizontal_align" : "right"},
							],
						},
						
						{
							"name" : "my_rank_name_window", "type" : "window", "x" : 55, "y" : 4, "width" : 113, "height" : 11,
							
							"children" :
							[
								{ "name" : "my_rank_name_text", "type" : "text", "style" : ("ltr",), "x" : 2, "y" : 0, "text" : "", "r":0.812, "g":0.749, "b":0.651, "a":1.0, "horizontal_align" : "right", "text_horizontal_align" : "right"},
							],
						},
						
						
						{
							"name" : "my_rank_guild_name_window", "type" : "window", "x" : 175, "y" : 4, "width" : 88, "height" : 11,
							
							"children" :
							[
								{ "name" : "my_rank_guild_name_text", "type" : "text", "style" : ("ltr",), "x" : 2, "y" : 0, "text" : "", "r":0.812, "g":0.749, "b":0.651, "a":1.0, "horizontal_align" : "right", "text_horizontal_align" : "right"},
							],
						},
						
						{ "name" : "my_rank_empire_img", "type" : "image", "style" : ("ltr",), "x" : 279,	"y" : 3, "image" : "d:/ymir work/ui/public/battle/empire_empty.sub"},
						
						{
							"name" : "my_rank_tier_window", "type" : "window", "x" : 318, "y" : 4, "width" : 39, "height" : 11,
						
							"children" :
							[
								{ "name" : "my_rank_tier_text", "type" : "text", "style" : ("ltr",), "x" : 0,	"y" : 0, "text" : "", "r":0.812, "g":0.749, "b":0.651, "a":1.0, "horizontal_align" : "center", "text_horizontal_align" : "center"},
							],
						},
					),
				},
			],
		},
	],
}