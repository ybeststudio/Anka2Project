import uiScriptLocale

IMG_PATH = "d:/ymir work/ui/public/huntsystem/"

GOLD_COLOR = 0xFFFEE3AE

WINDOW_WIDTH = 500
WINDOW_HEIGHT = 265

window = {
	"name" : "HuntingWindow", "style" : ("movable", "float",),
	"x" : ( SCREEN_WIDTH / 2 ) - ( WINDOW_WIDTH / 2 ), "y" : ( SCREEN_HEIGHT / 2 ) -( WINDOW_HEIGHT / 2 ),
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
	"children" :
	(
		## BOARD
		{
			"name" : "board", "type" : "board_with_titlebar",  "x" : 0, "y" : 0, "width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"title" : uiScriptLocale.HUNTING_SYSTEM_TITLE,
			"children" :
			(
				{
					"name" : "thinboard",  "type" : "border_a", "x" : 15, "y" : 35, "width" : WINDOW_WIDTH - 30, "height" : WINDOW_HEIGHT - 35 - 15 - 25,
					"children" :
					(
						{
							"name" : "container_1", "type" : "image", "x" : 148, "y" : 5,  "image" : IMG_PATH + "container_1.sub",
							"children" :
							(
								{
									"name" : "title_text", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "fontsize":"LARGE", "horizontal_align" : "center", "text_horizontal_align" : "center",
									"text" : uiScriptLocale.HUNTING_SYSTEM_TITLE_INFO,
								},
								{ "name" : "VerticalLine", "type" : "line", "x" : 211, "y" : 24, "width" : 0, "height" : 152, "color" : 0xff4d4d4d, },
								{ "name" : "HorizontalLine", "type" : "line", "x" : 7, "y" : 144, "width" : 195, "height" : 0, "color" : 0xff4d4d4d, },
								{
									"name" : "mobname_info_slot", "type" : "image", "x" : 7, "y" : 27 + 22*0,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "mobname_info", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_MOB, }, ),
								},
								{
									"name" : "mobname_text_slot", "type" : "image", "x" : 85, "y" : 27 + 22*0,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "mobname_text", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "Ober-Ork", }, ),
								},
								{
									"name" : "amount_info_slot", "type" : "image", "x" : 7, "y" : 27 + 22*1,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "amount_info", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_AMOUNT, }, ),
								},
								{
									"name" : "amount_text_slot", "type" : "image", "x" : 85, "y" : 27 + 22*1,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "amount_text", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "0 von 150", }, ),
								},
								{
									"name" : "dest_info_slot", "type" : "image", "x" : 7, "y" : 27 + 22*2,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "dest_info", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_DESTINATION, }, ),
								},
								{
									"name" : "dest_text_slot", "type" : "image", "x" : 85, "y" : 27 + 22*2,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "dest_text", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "Tal von Seungyong", }, ),
								},
								{
									"name" : "money_info_slot", "type" : "image", "x" : 7, "y" : 27 + 22*3,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "money_info", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_MONEY, }, ),
								},
								{ 
									"name" : "money_text_slot", "type" : "image", "x" : 85, "y" : 27 + 22*3,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "money_text", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "500.000 - 2.000.000", }, ),
								},
								{
									"name" : "exp_info_slot", "type" : "image", "x" : 7, "y" : 27 + 22*4,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "exp_info", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_EXP, }, ),
								},
								{
									"name" : "exp_text_slot", "type" : "image", "x" : 85, "y" : 27 + 22*4,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "exp_text", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "10% - 25%", }, ),
								},
								{ "name" : "progress_text", "type" : "text", "x" : 105, "y" : 147, "color" : GOLD_COLOR, "outline" : 1, "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_PROGRESS, },
								{
									"name" : "gauge_background", "type" : "image", "x" : 6, "y" : 164, "image" : IMG_PATH + "gauge_empty.sub",
									"children": ( { "name" : "gauge", "type" : "expanded_image", "x" : 8, "y" : 2, "image" : IMG_PATH + "gauge_blue.sub", }, ),
								},
								{ "name" : "face_icon", "type" : "image", "x" : 224, "y" : 27, },
								{ "name" : "face_icon_overlay", "style" : ("not_pick",), "type" : "image", "x" : 221, "y" : 24,  "image" : IMG_PATH + "face_overlay.sub", },
								{
									"name" : "item_icon_area", "type" : "window", "x" : 224, "y" : 69, "width" : 32, "height" : 96,
									"children" :
									(
										{ "name" : "empty_slot_0", "style" : ("not_pick",), "type" : "image", "x" : 0, "y" : 0, "image" : "d:/ymir work/ui/pet/skill_button/skill_enable_button.sub", },
										{ "name" : "empty_slot_1", "style" : ("not_pick",), "type" : "image", "x" : 0, "y" : 32, "image" : "d:/ymir work/ui/pet/skill_button/skill_enable_button.sub", },
										{ "name" : "empty_slot_2", "style" : ("not_pick",), "type" : "image", "x" : 0, "y" : 64, "image" : "d:/ymir work/ui/pet/skill_button/skill_enable_button.sub", },
										{ "name" : "item_icon", "type" : "image", "x" : 0, "y" : 0 },
									),
								},
							),
						},
						{
							"name" : "container_2", "type" : "image", "x" : 417, "y" : 5,  "image" : IMG_PATH + "container_2.sub",
							"children" :
							(
								{ "name" : "scrollbar_area", "type" : "window", "x" : 39, "y" : 25, "width" : 4, "height" : 150, },
								{ "name" : "item_area", "type" : "window", "x" : 3, "y" : 23, "width" : 32, "height" : 154, },
							),
						},
						{
							"name" : "info_layer", "type" : "bar", "x" : 3, "y" : 3,  "width" : 464, "height" : 184,
							"children" :
							(
								{ "name" : "info_title", "type" : "text", "x" : 0, "y" : 60, "outline" : 1, "fontsize":"XLARGE", "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_INFO },
								{ "name" : "info_text_over_level", "type" : "text", "x" : 0, "y" : 100, "outline" : 1, "fontsize":"LARGE", "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_INFO_OVER_LEVEL },
								{ "name" : "info_text_complete_all", "type" : "text", "x" : 0, "y" : 100, "outline" : 1, "fontsize":"LARGE", "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_INFO_COMPLETE_ALL },
							),
						},
					),
				},
				{
					"name" : "mini_ui_button", "type" : "button", "x" : 15, "y" : 15 + 20, "vertical_align" : "bottom", "text" : uiScriptLocale.HUNTING_SYSTEM_BUTTON_MINI_UI_ON, "text_outline" : 1, "text_y" : -1,
					"default_image" : IMG_PATH + "button_small_normal.sub", "over_image" : IMG_PATH + "button_small_hover.sub", "down_image" : IMG_PATH + "button_small_down.sub", "disable_image" : IMG_PATH + "button_small_down.sub",
				},
				{
					"name" : "close_button", "type" : "button", "x" : 0, "y" : 15 + 20, "horizontal_align" : "center", "vertical_align" : "bottom", "text" : uiScriptLocale.CLOSE, "text_outline" : 1, "text_y" : -1,
					"default_image" : IMG_PATH + "button_small_normal.sub", "over_image" : IMG_PATH + "button_small_hover.sub", "down_image" : IMG_PATH + "button_small_down.sub", "disable_image" : IMG_PATH + "button_small_down.sub",
				},
				{
					"name" : "reward_button", "type" : "button", "x" : 15 + 120, "y" : 15 + 20, "vertical_align" : "bottom", "horizontal_align" : "right", "text" : uiScriptLocale.HUNTING_SYSTEM_BUTTON_REWARD_COLLECT, "text_outline" : 1, "text_y" : -1,
					"default_image" : IMG_PATH + "button_small_normal.sub", "over_image" : IMG_PATH + "button_small_hover.sub", "down_image" : IMG_PATH + "button_small_down.sub", "disable_image" : IMG_PATH + "button_small_down.sub",
				},
			),
		},
	),
}