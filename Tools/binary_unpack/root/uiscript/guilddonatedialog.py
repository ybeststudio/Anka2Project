import uiScriptLocale

WINDOW_WIDTH	= 412
WINDOW_HEIGHT	= 230

ROOT_PATH = "d:/ymir work/ui/game/guild/guilddonate/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"
PATTERN_WINDOW_WIDTH	= 390
PATTERN_WINDOW_HEIGHT	= 165
PATTERN_X_COUNT = (PATTERN_WINDOW_WIDTH - 32) / 16
PATTERN_Y_COUNT = (PATTERN_WINDOW_HEIGHT - 32) / 16

window = {
	"name" : "GuildDonateDialog",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH/2 - WINDOW_WIDTH/2,
	"y" : SCREEN_HEIGHT/2 - WINDOW_HEIGHT/2,

	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,

			"title" : uiScriptLocale.GUILD_DONATE_TITLE,
			
			"children" :
			(
				## bg pattern window
				{
					"name" : "bg_pattern_window",
					"type" : "window",
					
					"style" : ("attach", "ltr",),

					"x" : 10,
					"y" : 32,
					
					"width" : PATTERN_WINDOW_WIDTH,
					"height" : PATTERN_WINDOW_HEIGHT,
					
					"children" :
					(
						## LeftTop 1
						{
							"name" : "RegistWindowLeftTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						## RightTop 2
						{
							"name" : "RegistWindowRightTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "RegistWindowLeftBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "RegistWindowRightBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						## topcenterImg 5
						{
							"name" : "RegistWindowTopCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "RegistWindowLeftCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "RegistWindowRightCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : PATTERN_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "RegistWindowBottomCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : PATTERN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "RegistWindowCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, PATTERN_Y_COUNT),
						},
					),
				},
				
				## noraml donate
				{
					"name" : "normal_donate_window", "type" : "window", "x" : 19, "y" : 39, "width" : 120, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"donate_tab_img", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "donate_menu_tab.sub"},
						{"name":"normal_donate_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.GUILD_DONATE_TAB_NORMAL, "all_align" : "center", "color" : 0xFFE4CCB9, },
					),
				},
				
				## noraml donate bg
				{"name":"normal_donate_contents_img", "type":"image", "x":18, "y":65, "image" : ROOT_PATH + "donate_menu_contents.sub"},
				## noraml donate medal of honor icon
				{"name":"normal_donate_medal_of_honor_icon", "type":"image", "x":22, "y":72, "image" : ROOT_PATH + "medal_of_honor_icon.sub"},
				## noraml donate medal of honor
				{
					"name" : "normal_donate_medal_of_honor_window", "type" : "window", "x" : 38, "y" : 71, "width" : 101, "height" : 17, "style" : ("attach",),
					"children" :
					(
						{"name":"normal_donate_medal_of_honor", "type":"text", "x": 0, "y":0, "text": uiScriptLocale.GUILD_DONATE_GET_MEDAL_OF_HONOR % 0, "all_align" : "center", },
					),
				},
				## noraml donate medal of honor over area
				{ "name" : "normal_donate_medal_of_honor_over_window", "type" : "window", "x" : 19, "y" : 71, "width" : 120, "height" : 17, "style" : ("attach",), },
				
				## noraml donate guild exp icon
				{"name":"normal_donate_guild_exp_icon", "type":"image", "x":21, "y":90, "image" : ROOT_PATH + "guild_exp_icon.sub"},
				## noraml donate get exp
				{
					"name" : "normal_donate_guild_exp_window", "type" : "window", "x" : 38, "y" : 90, "width" : 101, "height" : 17, "style" : ("attach",),
					"children" :
					(
						{"name":"normal_donate_guild_exp", "type":"text", "x": 0, "y":0, "text": "0", "all_align" : "center", },
					),
				},
				## noraml donate guild exp over area
				{ "name" : "normal_donate_guild_exp_over_window", "type" : "window", "x" : 19, "y" : 90, "width" : 120, "height" : 17, "style" : ("attach",), },
				
				## noraml donate get item slot
				{
					"name" : "normal_donate_get_item_slot",
					"type" : "grid_table",
					"x" : 31,
					"y" : 120,
					"start_index" : 0,
					"x_count" : 3,
					"y_count" : 1,
					"x_step" : 32,
					"y_step" : 32,
				},
				
				## middle donate
				{
					"name" : "middle_donate_window", "type" : "window", "x" : 145, "y" : 39, "width" : 120, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"donate_tab_img", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "donate_menu_tab.sub"},
						{"name":"middle_donate_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.GUILD_DONATE_TAB_MIDDLE, "all_align" : "center", "color" : 0xFFE4CCB9, },
					),
				},
				
				## middle donate bg
				{"name":"middle_donate_contents_img", "type":"image", "x":144, "y":65, "image" : ROOT_PATH + "donate_menu_contents.sub"},
				## middle donate medal of honor icon
				{"name":"middle_donate_medal_of_honor_icon", "type":"image", "x":148, "y":72, "image" : ROOT_PATH + "medal_of_honor_icon.sub"},
				## middle donate medal of honor
				{
					"name" : "middle_donate_medal_of_honor_window", "type" : "window", "x" : 164, "y" : 71, "width" : 101, "height" : 17, "style" : ("attach",),
					"children" :
					(
						{"name":"middle_donate_medal_of_honor", "type":"text", "x": 0, "y":0, "text": uiScriptLocale.GUILD_DONATE_GET_MEDAL_OF_HONOR % 0, "all_align" : "center", },
					),
				},
				## middle donate medal of honor over area
				{ "name" : "middle_donate_medal_of_honor_over_window", "type" : "window", "x" : 145, "y" : 71, "width" : 120, "height" : 17, "style" : ("attach",), },
				
				## middle donate guild exp icon
				{"name":"middle_donate_guild_exp_icon", "type":"image", "x":147, "y":90, "image" : ROOT_PATH + "guild_exp_icon.sub"},
				## middle donate get exp
				{
					"name" : "middle_donate_guild_exp_window", "type" : "window", "x" : 164, "y" : 90, "width" : 101, "height" : 17, "style" : ("attach",),
					"children" :
					(
						{"name":"middle_donate_guild_exp", "type":"text", "x": 0, "y":0, "text": "0", "all_align" : "center", },
					),
				},
				## middle donate guild exp over area
				{ "name" : "middle_donate_guild_exp_over_window", "type" : "window", "x" : 145, "y" : 90, "width" : 120, "height" : 17, "style" : ("attach",), },
				
				## middle donate get item slot
				{
					"name" : "middle_donate_get_item_slot",
					"type" : "grid_table",
					"x" : 157,
					"y" : 120,
					"start_index" : 0,
					"x_count" : 3,
					"y_count" : 1,
					"x_step" : 32,
					"y_step" : 32,
				},
				
				## high donate
				{
					"name" : "high_donate_window", "type" : "window", "x" : 271, "y" : 39, "width" : 120, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"donate_tab_img", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "donate_menu_tab.sub"},
						{"name":"high_donate_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.GUILD_DONATE_TAB_HIGH, "all_align" : "center", "color" : 0xFFE4CCB9, },
					),
				},
				
				## high donate bg
				{"name":"high_donate_contents_img", "type":"image", "x":270, "y":65, "image" : ROOT_PATH + "donate_menu_contents.sub"},
				## high donate medal of honor icon
				{"name":"high_donate_medal_of_honor_icon", "type":"image", "x":274, "y":72, "image" : ROOT_PATH + "medal_of_honor_icon.sub"},
				## high donate medal of honor
				{
					"name" : "high_donate_medal_of_honor_window", "type" : "window", "x" : 290, "y" : 71, "width" : 101, "height" : 17, "style" : ("attach",),
					"children" :
					(
						{"name":"high_donate_medal_of_honor", "type":"text", "x": 0, "y":0, "text": uiScriptLocale.GUILD_DONATE_GET_MEDAL_OF_HONOR % 0, "all_align" : "center", },
					),
				},
				## high donate medal of honor over area
				{ "name" : "high_donate_medal_of_honor_over_window", "type" : "window", "x" : 271, "y" : 71, "width" : 120, "height" : 17, "style" : ("attach",), },
				
				## high donate guild exp icon
				{"name":"high_donate_guild_exp_icon", "type":"image", "x":273, "y":90, "image" : ROOT_PATH + "guild_exp_icon.sub"},
				## high donate get exp
				{
					"name" : "high_donate_guild_exp_window", "type" : "window", "x" : 290, "y" : 90, "width" : 101, "height" : 17, "style" : ("attach",),
					"children" :
					(
						{"name":"high_donate_guild_exp", "type":"text", "x": 0, "y":0, "text": "0", "all_align" : "center", },
					),
				},
				## high donate guild exp over area
				{ "name" : "high_donate_guild_exp_over_window", "type" : "window", "x" : 271, "y" : 90, "width" : 120, "height" : 17, "style" : ("attach",), },
				
				## high donate get item slot
				{
					"name" : "high_donate_get_item_slot",
					"type" : "grid_table",
					"x" : 283,
					"y" : 120,
					"start_index" : 0,
					"x_count" : 3,
					"y_count" : 1,
					"x_step" : 32,
					"y_step" : 32,
				},
				
				## current medal of honor
				{ "name" : "cur_medal_of_honor_img", "type" : "image", "x" : 16, "y" : 202, "image" : ROOT_PATH + "medal_of_honor_icon.sub", },
				## current medal of honor count
				{
					"name" : "cur_medal_of_honor_window", "type" : "window", "x" : 35, "y" : 200, "width" : 86, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{"name":"cur_medal_of_honor_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "medal_of_honor_bg.sub" },
						{"name":"cur_medal_of_honor_text", "type":"text", "x":0, "y":0, "text": "0", "all_align" : "center", "color" : 0xFFCEC6B5,},
					),
				},
				## current medal of honor over area
				{ "name" : "cur_medal_of_honor_over_window", "type" : "window", "x" : 16, "y" : 200, "width" : 105, "height" : 18, },
		
				## donate count
				{
					"name" : "donate_count_window", "type" : "window", "x" : 235, "y" : 201, "width" : 159, "height" : 17, "style" : ("attach",),
					"children" :
					(
						{"name":"donate_count_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.GUILD_DONATE_COUNT % (3,3), "all_align" : "center", "color" : 0xFFC1BEB9, },
					),
				},
				
				## noraml donate button
				{
					"name" : "NoramlDonateButton",
					"type" : "button",

					"x" : 35,
					"y" : 168,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.GUILD_DONATE_NORMAL_MATERIAL,
				},
				## middle donate button
				{
					"name" : "MiddleDonateButton",
					"type" : "button",

					"x" : 161,
					"y" : 168,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.GUILD_DONATE_MIDDLE_MATERIAL,
				},
				## high donate button
				{
					"name" : "HighDonateButton",
					"type" : "button",

					"x" : 287,
					"y" : 168,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.GUILD_DONATE_HIGH_MATERIAL,
				},
			),
		},
	),
}

