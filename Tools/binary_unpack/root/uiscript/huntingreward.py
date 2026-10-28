import uiScriptLocale

IMG_PATH = "d:/ymir work/ui/public/huntsystem/"

GOLD_COLOR = 0xFFFEE3AE

WINDOW_WIDTH = 352
WINDOW_HEIGHT = 300

window = {
	"name" : "HuntingWindow", "style" : ("movable", "float",),
	"x" : ( SCREEN_WIDTH / 2 ) - ( WINDOW_WIDTH / 2 ), "y" : ( SCREEN_HEIGHT / 2 ) -( WINDOW_HEIGHT / 2 ),
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar",  "x" : 0, "y" : 0, "width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"title" : uiScriptLocale.HUNTING_SYSTEM_TITLE_REWARD,
			"children" :
			(
				{
					"name" : "board_border",  "type" : "border_a", "x" : 15, "y" : 35, "width" : 322, "height" : 252,
					"children" :
					(
						{
							"name" : "container_1", "type" : "image", "x" : 5, "y" : 5,  "image" : IMG_PATH + "reward_board_1.tga",
							"children" :
							(
								{
									"name" : "title_text", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "fontsize":"LARGE", "horizontal_align" : "center", "text_horizontal_align" : "center",
									"text" : uiScriptLocale.HUNTING_SYSTEM_TITLE_REWARD_RACE_ITEM,
								},
								{
									"name" : "item_icon_area_race", "type" : "window", "x" : 60, "y" : 26, "width" : 32, "height" : 96,
									"children" :
									(
										{ "name" : "item_icon", "type" : "image", "x" : 0, "y" : 0 },
										{ "name" : "empty_slot_0", "type" : "image", "x" : 0, "y" : 0, "image" : IMG_PATH + "key_slot.png", },
										{ "name" : "empty_slot_1", "type" : "image", "x" : 0, "y" : 32, "image" : IMG_PATH + "key_slot.png", },
										{ "name" : "empty_slot_2", "type" : "image", "x" : 0, "y" : 64, "image" : IMG_PATH + "key_slot.png", },
										{ "name" : "ani_slot_0", "type" : "window", "x" : 0, "y" : 0, "width" : 80, "height" : 80, },
										{ "name" : "ani_slot_1", "type" : "window", "x" : 0, "y" : 32, "width" : 80, "height" : 80, },
										{ "name" : "ani_slot_2", "type" : "window", "x" : 0, "y" : 64, "width" : 80, "height" : 80, },
									),
								},
							),
						},
						{
							"name" : "container_2", "type" : "image", "x" : 165, "y" : 5,  "image" : IMG_PATH + "reward_board_2.tga",
							"children" :
							(
								{
									"name" : "title_text", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "fontsize":"LARGE", "horizontal_align" : "center", "text_horizontal_align" : "center",
									"text" : uiScriptLocale.HUNTING_SYSTEM_TITLE_REWARD_RANDOM_ITEM,
								},
								{
									"name" : "item_icon_area_0", "type" : "window", "x" : 60, "y" : 41, "width" : 32, "height" : 96,
									"children" :
									(
										{ "name" : "item_icon_random", "type" : "image", "x" : 0, "y" : 0 },
										{ "name" : "empty_slot_3", "type" : "image", "x" : 0, "y" : 0,  "image" : IMG_PATH + "key_slot.png", },
										{ "name" : "ani_slot_3", "type" : "window", "x" : 0, "y" : 0, "width" : 80, "height" : 80, },
									),
								},
								{ "name" : "random_item_name", "type" : "text", "x" : 76, "y" : 92, "color" : GOLD_COLOR, "text_horizontal_align" : "center", "text" : "", },
							),
						},
						{
							"name" : "container_3", "type" : "image", "x" : 5, "y" : 139,  "image" : IMG_PATH + "reward_board_3.tga", 
							"children" :
							(
								{
									"name" : "title_text", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "fontsize":"LARGE", "horizontal_align" : "center", "text_horizontal_align" : "center",
									"text" : uiScriptLocale.HUNTING_SYSTEM_TITLE_REWARD_MONEY_EXP,
								},
								{ "name" : "money_info", "type" : "text", "x" : 83, "y" : 29, "color" : GOLD_COLOR, "outline" : 1, "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_MONEY, },
								{ "name" : "money_text", "type" : "text", "x" : 83, "y" : 46, "color" : GOLD_COLOR, "text_horizontal_align" : "center", "text" : "", },
								{ "name" : "exp_info", "type" : "text", "x" : 227, "y" : 29, "color" : GOLD_COLOR, "outline" : 1, "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_EXP, },
								{ "name" : "exp_text", "type" : "text", "x" : 227, "y" : 46, "color" : GOLD_COLOR, "text_horizontal_align" : "center", "text" : "", },
							),
						},
						{
							"name" : "ButtonRecive", "type" : "button", "x" : 0, "y" : 217, "horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_BUTTON_REWARD_OPEN, "text_color" : GOLD_COLOR, "text_fontsize" : "LARGE", "text_outline" : 1, "text_y" : -1,
							"default_image" : IMG_PATH + "button_large_normal.sub", "over_image" : IMG_PATH + "button_large_hover.sub", "down_image" : IMG_PATH + "button_large_down.sub",
						},
					),
				},
			),
		},
	),
}