import uiScriptLocale

ROOT_PATH				= "d:/ymir work/ui/game/mount_upgrade_system/"
PATTERN_PATH			= "d:/ymir work/ui/pattern/"

WINDOW_WIDTH			= 336
WINDOW_HEIGHT			= 142

PATTERN_WINDOW_WIDTH	= 314
PATTERN_WINDOW_HEIGHT	= 72

PATTERN_X_COUNT = (PATTERN_WINDOW_WIDTH - 32) / 16
PATTERN_Y_COUNT = (PATTERN_WINDOW_HEIGHT - 32) / 16

BACKGROUND_X			= 10
BACKGROUND_Y			= 32


window = {
	"name" : "MountUpgradeSystemWindow",
	"style" : ("movable", "float",),
	
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width"  : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	
	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach", "ltr",),
			
			"x" : 0,
			"y" : 0,
			
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			
			"title" : uiScriptLocale.MOUNT_UPGRADE_SYSTEM_TITLE,
			
			"children" :
			(
				## base pattern
				{
					"name" : "base_pattern",
					"type" : "window",
					"style" : ("attach", "ltr",),
					
					"x" : 10,
					"y" : 33,
					"width" : PATTERN_WINDOW_WIDTH,
					"height" :PATTERN_WINDOW_HEIGHT,
					
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

				## back ground
				{
					"name" : "mount_upgrade_system_bg",
					"type" : "window",
					"style" : ("attach", "ltr",),
					
					"x" : BACKGROUND_X,
					"y" : BACKGROUND_Y,
					
					"width" : 314,
					"height" : 72,
					
					"children" :
					(
						## cur level
						{
							"name" : "cur_level_window", "type" : "window", "x" : 8, "y" : 12, "width" : 56, "height" : 20, "style" : ("attach",),
							"children" :
							(
								{"name":"cur_level_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "level_cur_bg.sub"},
								{"name":"cur_level_text", "type":"text", "x":0, "y":0, "text": "31", "all_align" : "center"},
							),
						},
						
						## next level
						{
							"name" : "next_level_window", "type" : "window", "x" : 250, "y" : 12, "width" : 56, "height" : 20, "style" : ("attach",),
							"children" :
							(
								{"name":"next_level_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "level_next_bg.sub"},
								{"name":"next_level_text", "type":"text", "x":0, "y":0, "text": "31", "all_align" : "center"},
								{"name":"level_up_arrow", "type":"image", "x":4, "y":2, "image" : ROOT_PATH + "levelup_arrow.sub"},
							),
						},
						
						## exp
						{
							"name" : "exp_gauge_window", "type" : "window", "x" : 80 - BACKGROUND_X, "y" : 42 - BACKGROUND_Y, "width" : 168, "height" : 22, "style" : ("attach",),
							"children" :
							(
								{"name":"exp_gauge_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "exp_gage_bg.sub"},
								{"name":"exp_gauge_bar", "type":"expanded_image", "x":6, "y":2, "image" : ROOT_PATH + "exp_gage_bar.sub"},
								{"name":"exp_gauge_text", "type":"text", "x":0, "y":0, "text" : "2350/3750", "all_align" : "center"},
							),
						},
						
						## feed button
						{
							"name" : "feed_button",
							"type" : "button",
							"x" : 126 - BACKGROUND_X,
							"y" : 75 - BACKGROUND_Y,
							
							"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
			
							"text" : uiScriptLocale.MOUNT_UPGRADE_SYSTEM_GIVE_FEED,
						},

						## levelup button
						{
							"name" : "level_up_button",
							"type" : "button",
							"x" : 126 - BACKGROUND_X,
							"y" : 75 - BACKGROUND_Y,
							
							"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
			
							"text" : uiScriptLocale.MOUNT_UPGRADE_SYSTEM_LEVEL_UP,
						},
					),
				},
				
				## gem icon
				{
					"name":"gem_icon",
					"type":"image",
					
					"x":147,
					"y":114,
					
					"image" : ROOT_PATH + "gem_icon.sub"
				},
				
				## gem text window
				{
					"name" : "gem_text_window", "type" : "window", "x" : 166, "y" : 111, "width" : 48, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{"name":"gem_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "gem_text_bg.sub"},
						
						{"name":"gem_text", "type":"text", "x":0, "y":2, "text": "1234", "horizontal_align" : "right", "text_horizontal_align" : "right",},
						
					),	
				},
				
				##yang icon
				{
					"name":"yang_icon",
					"type":"image",
					
					"x":220,
					"y":114,
					
					"image" : ROOT_PATH + "yang_icon.sub"
				},
				
				## yang text window
				{
					"name" : "yang_window", "type" : "window", "x" : 237, "y" : 111, "width" : 84, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{"name":"yang_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "yang_text_bg.sub"},
						
						{"name":"yang_text", "type":"text", "x":0, "y":2, "text": "", "horizontal_align" : "right", "text_horizontal_align" : "right",},
						
					),	
				},
			),
			
		},
	
	),
	
}