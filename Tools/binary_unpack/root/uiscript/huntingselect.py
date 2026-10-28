import uiScriptLocale

IMG_PATH = "d:/ymir work/ui/public/huntsystem/"

GOLD_COLOR = 0xFFFEE3AE

WINDOW_WIDTH = 500
WINDOW_HEIGHT = 265 * 2 - 90

window = {
	"name" : "HuntingWindow", "style" : ("movable", "float",),
	"x" : ( SCREEN_WIDTH / 2 ) - ( WINDOW_WIDTH / 2 ), "y" : ( SCREEN_HEIGHT / 2 ) -( WINDOW_HEIGHT / 2 ),
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar",  "x" : 0, "y" : 0, "width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"title" : uiScriptLocale.HUNTING_SYSTEM_TITLE_SELECT,
			"children" :
			(
				{
					"name" : "board_0",  "type" : "border_a", "x" : 15, "y" : 35, "width" : WINDOW_WIDTH - 30, "height" : 190,
					"children" :
					(
						{
							"name" : "container_1_0", "type" : "image", "x" : 148, "y" : 5,  "image" : IMG_PATH + "container_1.sub",
							"children" :
							(
								{
									"name" : "title_text", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "fontsize":"LARGE", "horizontal_align" : "center", "text_horizontal_align" : "center",
									"text" : uiScriptLocale.HUNTING_SYSTEM_TITLE_INFO,
								},
								{ "name" : "VerticalLine", "type" : "line", "x" : 211, "y" : 24, "width" : 0, "height" : 152, "color" : 0xff4d4d4d, },
								{ "name" : "HorizontalLine", "type" : "line", "x" : 7, "y" : 144, "width" : 195, "height" : 0, "color" : 0xff4d4d4d, },
								{
									"name" : "mobname_info_slot_0", "type" : "image", "x" : 7, "y" : 27 + 22*0,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "mobname_info_0", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_MOB, }, ),
								},
								{
									"name" : "mobname_text_slot_0", "type" : "image", "x" : 85, "y" : 27 + 22*0,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "mobname_text_0", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "Ober-Ork", }, ),
								},
								{
									"name" : "amount_info_slot_0", "type" : "image", "x" : 7, "y" : 27 + 22*1,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "amount_info_0", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_AMOUNT, }, ),
								},
								{
									"name" : "amount_text_slot_0", "type" : "image", "x" : 85, "y" : 27 + 22*1,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "amount_text_0", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "0 von 150", }, ),
								},
								{
									"name" : "dest_info_slot_0", "type" : "image", "x" : 7, "y" : 27 + 22*2,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "dest_info_0", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_DESTINATION, }, ),
								},
								{
									"name" : "dest_text_slot_0", "type" : "image", "x" : 85, "y" : 27 + 22*2,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "dest_text_0", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "Tal von Seungyong", }, ),
								},
								{
									"name" : "money_info_slot_0", "type" : "image", "x" : 7, "y" : 27 + 22*3,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "money_info_0", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_MONEY, }, ),
								},
								{
									"name" : "money_text_slot_0", "type" : "image", "x" : 85, "y" : 27 + 22*3,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "money_text_0", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "500.000 - 2.000.000", }, ),
								},
								{
									"name" : "exp_info_slot_0", "type" : "image", "x" : 7, "y" : 27 + 22*4,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "exp_info_0", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_EXP, }, ),
								},
								{
									"name" : "exp_text_slot_0", "type" : "image", "x" : 85, "y" : 27 + 22*4,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "exp_text_0", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "10% - 25%", }, ),
								},
								{ "name" : "face_icon_0", "type" : "image", "x" : 224, "y" : 27, },
								{ "name" : "face_icon_overlay_0", "style" : ("not_pick",), "type" : "image", "x" : 221, "y" : 24,  "image" : IMG_PATH + "face_overlay.sub", },
								{
									"name" : "item_icon_area_0", "type" : "window", "x" : 224, "y" : 69, "width" : 32, "height" : 96,
									"children" :
									(
										{ "name" : "empty_slot_0_0", "style" : ("not_pick",), "type" : "image", "x" : 0, "y" : 0,  "image" : "d:/ymir work/ui/pet/skill_button/skill_enable_button.sub", },
										{ "name" : "empty_slot_1_0", "style" : ("not_pick",), "type" : "image", "x" : 0, "y" : 32,  "image" : "d:/ymir work/ui/pet/skill_button/skill_enable_button.sub", },
										{ "name" : "empty_slot_2_0", "style" : ("not_pick",), "type" : "image", "x" : 0, "y" : 64,  "image" : "d:/ymir work/ui/pet/skill_button/skill_enable_button.sub", },
										{ "name" : "item_icon_0", "type" : "image", "x" : 0, "y" : 0 },
									),
								},
								{
									"name" : "select_button_0", "type" : "button", "x" : 7, "y" : 149, "text" : uiScriptLocale.HUNTING_SYSTEM_BUTTON_SELECT, "text_color" : GOLD_COLOR, "text_fontsize" : "LARGE", "text_outline" : 1, "text_y" : -1,
									"default_image" : IMG_PATH + "button_large_normal.sub", "over_image" : IMG_PATH + "button_large_hover.sub", "down_image" : IMG_PATH + "button_large_down.sub",
								},
							),
						},
						{
							"name" : "container_2_0", "type" : "image", "x" : 417, "y" : 5,  "image" : IMG_PATH + "container_2.sub",
							"children" :
							(
								{ "name" : "scrollbar_area_0", "type" : "window", "x" : 39, "y" : 25, "width" : 4, "height" : 150, },
								{ "name" : "item_area_0", "type" : "window", "x" : 3, "y" : 23, "width" : 32, "height" : 154, },
							),
						},
					),
				},
				{
					"name" : "board_1",  "type" : "border_a", "x" : 15, "y" : 35 + 190 + 10, "width" : WINDOW_WIDTH - 30, "height" : 190,
					"children" :
					(
						{
							"name" : "container_1_1", "type" : "image", "x" : 148, "y" : 5,  "image" : IMG_PATH + "container_1.sub",
							"children" :
							(
								{
									"name" : "title_text", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "fontsize":"LARGE", "horizontal_align" : "center", "text_horizontal_align" : "center",
									"text" : uiScriptLocale.HUNTING_SYSTEM_TITLE_INFO,
								},
								{ "name" : "VerticalLine", "type" : "line", "x" : 211, "y" : 24, "width" : 0, "height" : 152, "color" : 0xff4d4d4d, },
								{ "name" : "HorizontalLine", "type" : "line", "x" : 7, "y" : 144, "width" : 195, "height" : 0, "color" : 0xff4d4d4d, },
								{
									"name" : "mobname_info_slot_1", "type" : "image", "x" : 7, "y" : 27 + 22*0,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "mobname_info_1", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_MOB, }, ),
								},
								{
									"name" : "mobname_text_slot_1", "type" : "image", "x" : 85, "y" : 27 + 22*0,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "mobname_text_1", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "Ober-Ork", }, ),
								},
								{
									"name" : "amount_info_slot_1", "type" : "image", "x" : 7, "y" : 27 + 22*1,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "amount_info_1", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_AMOUNT, }, ),
								},
								{
									"name" : "amount_text_slot_1", "type" : "image", "x" : 85, "y" : 27 + 22*1,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "amount_text_1", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "0 von 150", }, ),
								},
								{
									"name" : "dest_info_slot_1", "type" : "image", "x" : 7, "y" : 27 + 22*2,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "dest_info_1", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_DESTINATION, }, ),
								},
								{
									"name" : "dest_text_slot_1", "type" : "image", "x" : 85, "y" : 27 + 22*2,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "dest_text_1", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "Tal von Seungyong", }, ),
								},
								{
									"name" : "money_info_slot_1", "type" : "image", "x" : 7, "y" : 27 + 22*3,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "money_info_1", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_MONEY, }, ),
								},
								{
									"name" : "money_text_slot_1", "type" : "image", "x" : 85, "y" : 27 + 22*3,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "money_text_1", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "500.000 - 2.000.000", }, ),
								},
								{
									"name" : "exp_info_slot_1", "type" : "image", "x" : 7, "y" : 27 + 22*4,  "image" : IMG_PATH + "slot_info.sub", 
									"children" : ( { "name" : "exp_info_1", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "outline" : 1, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : uiScriptLocale.HUNTING_SYSTEM_EXP, }, ),
								},
								{
									"name" : "exp_text_slot_1", "type" : "image", "x" : 85, "y" : 27 + 22*4,  "image" : IMG_PATH + "slot_text.sub", 
									"children" : ( { "name" : "exp_text_1", "type" : "text", "x" : 0, "y" : 2, "color" : GOLD_COLOR, "horizontal_align" : "center", "text_horizontal_align" : "center", "text" : "10% - 25%", }, ),
								},
								{ "name" : "face_icon_1", "type" : "image", "x" : 224, "y" : 27, },
								{ "name" : "face_icon_overlay_1", "style" : ("not_pick",), "type" : "image", "x" : 221, "y" : 24,  "image" : IMG_PATH + "face_overlay.sub", },
								{
									"name" : "item_icon_area_1", "type" : "window", "x" : 224, "y" : 69, "width" : 32, "height" : 96,
									"children" :
									(
										{ "name" : "empty_slot_0_1", "style" : ("not_pick",), "type" : "image", "x" : 0, "y" : 0,  "image" : "d:/ymir work/ui/pet/skill_button/skill_enable_button.sub", },
										{ "name" : "empty_slot_1_1", "style" : ("not_pick",), "type" : "image", "x" : 0, "y" : 32,  "image" : "d:/ymir work/ui/pet/skill_button/skill_enable_button.sub", },
										{ "name" : "empty_slot_2_1", "style" : ("not_pick",), "type" : "image", "x" : 0, "y" : 64,  "image" : "d:/ymir work/ui/pet/skill_button/skill_enable_button.sub", },
										{ "name" : "item_icon_1", "type" : "image", "x" : 0, "y" : 0 },
									),
								},
								{
									"name" : "select_button_1", "type" : "button", "x" : 7, "y" : 149, "text" : uiScriptLocale.HUNTING_SYSTEM_BUTTON_SELECT, "text_color" : GOLD_COLOR, "text_fontsize" : "LARGE", "text_outline" : 1, "text_y" : -1,
									"default_image" : IMG_PATH + "button_large_normal.sub", "over_image" : IMG_PATH + "button_large_hover.sub", "down_image" : IMG_PATH + "button_large_down.sub",
								},
							),
						},
						{
							"name" : "container_2_1", "type" : "image", "x" : 417, "y" : 5,  "image" : IMG_PATH + "container_2.sub", 
							"children" :
							(
								{ "name" : "scrollbar_area_1", "type" : "window", "x" : 39, "y" : 25, "width" : 4, "height" : 150, },
								{ "name" : "item_area_1", "type" : "window", "x" : 3, "y" : 23, "width" : 32, "height" : 154, },
							),
						},
					),
				},
			),
		},
	),
}