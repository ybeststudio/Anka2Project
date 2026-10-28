#if app.ENABLE_GUILD_LAND_RENEWAL:
# 길드부지 건물 경험치 투자 UI
import uiScriptLocale

WINDOW_WIDTH	= 286
WINDOW_HEIGHT	= 230

ROOT_PATH = "d:/ymir work/ui/game/guild/guilddonate/"
PATTERN_WINDOW_WIDTH	= 264
PATTERN_WINDOW_HEIGHT	= 165

window = {
	"name" : "GuildBuildingDonateDialog",
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

			"title" : uiScriptLocale.GUILD_BUILDING_DONATE_WINDOW_TITLE % "",	# 건물 경험치 투자(%s)
			
			"children" :
			(
				## bg pattern window
				{
					"name" : "bg_pattern_window",
					"type" : "outline_window",

					"x" : 10,
					"y" : 32,
					
					"width" : PATTERN_WINDOW_WIDTH,
					"height" : PATTERN_WINDOW_HEIGHT,
				},
				
				## 일반 donate
				{
					"name" : "normal_donate_window", "type" : "window", "x" : 19, "y" : 39, "width" : 120, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"donate_tab_img", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "donate_menu_tab.sub"},
						# 일반
						{"name":"normal_donate_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.GUILD_BUILDING_DONATE_NORMAL_TITLE, "all_align" : "center", "color" : 0xFFE4CCB9, },
					),
				},
				
				## 일반 donate bg
				{"name":"normal_donate_contents_img", "type":"image", "x":18, "y":65, "image" : ROOT_PATH + "donate_menu_contents.sub"},
		
				## 일반 donate guild exp icon
				{"name":"normal_donate_guild_exp_icon", "type":"image", "x":23, "y":80, "image" : ROOT_PATH + "guild_exp_icon.sub"},
				## 일반 donate get exp
				{
					"name" : "normal_donate_guild_exp_window", "type" : "window", "x" : 38, "y" : 80, "width" : 101, "height" : 17, "style" : ("attach",),
					"children" :
					(
						{"name":"normal_donate_guild_exp", "type":"text", "x": 0, "y":0, "text": "2,500", "all_align" : "center", },
					),
				},
				## 일반 donate guild exp over area
				{ "name" : "normal_donate_guild_exp_over_window", "type" : "window", "x" : 19, "y" : 80, "width" : 120, "height" : 17, "style" : ("attach",), },
				
				## 일반 donate get item slot
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
				
				## 플러스 donate
				{
					"name" : "plus_donate_window", "type" : "window", "x" : 145, "y" : 39, "width" : 120, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"donate_tab_img", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "donate_menu_tab.sub"},
						# 플러스
						{"name":"plus_donate_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.GUILD_BUILDING_DONATE_PLUS_TITLE, "all_align" : "center", "color" : 0xFFE4CCB9, },
					),
				},
				
				## 플러스 donate bg
				{"name":"plus_donate_contents_img", "type":"image", "x":144, "y":65, "image" : ROOT_PATH + "donate_menu_contents.sub"},
				
				## 플러스 donate guild exp icon
				{"name":"plus_donate_guild_exp_icon", "type":"image", "x":149, "y":80, "image" : ROOT_PATH + "guild_exp_icon.sub"},
				## 플러스 donate get exp
				{
					"name" : "plus_donate_guild_exp_window", "type" : "window", "x" : 164, "y" : 80, "width" : 101, "height" : 17, "style" : ("attach",),
					"children" :
					(
						{"name":"plus_donate_guild_exp", "type":"text", "x": 0, "y":0, "text": "5,000", "all_align" : "center", },
					),
				},
				## 플러스 donate guild exp over area
				{ "name" : "plus_donate_guild_exp_over_window", "type" : "window", "x" : 145, "y" : 80, "width" : 120, "height" : 17, "style" : ("attach",), },
				
				## 플러스 donate get item slot
				{
					"name" : "plus_donate_get_item_slot",
					"type" : "grid_table",
					"x" : 157,
					"y" : 120,
					"start_index" : 0,
					"x_count" : 3,
					"y_count" : 1,
					"x_step" : 32,
					"y_step" : 32,
				},
		
				## donate count
				{
					"name" : "donate_count_window", "type" : "window", "x" : 235 - 126, "y" : 201, "width" : 159, "height" : 17, "style" : ("attach",),
					"children" :
					(
						{"name":"donate_count_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.GUILD_DONATE_COUNT % (3,3), "all_align" : "center", "color" : 0xFFC1BEB9, },
					),
				},
				
				## 일반 donate button
				{
					"name" : "normal_donate_button",
					"type" : "button",

					"x" : 35,
					"y" : 168,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.GUILD_BUILDING_DONATE_NORMAL_BUTTON_TEXT,	# 일반권
				},
				## 플러스 donate button
				{
					"name" : "plus_donate_button",
					"type" : "button",

					"x" : 161,
					"y" : 168,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",

					"text" : uiScriptLocale.GUILD_BUILDING_DONATE_PLUS_BUTTON_TEXT,	# 플러스권
				},
			),
		},
	),
}