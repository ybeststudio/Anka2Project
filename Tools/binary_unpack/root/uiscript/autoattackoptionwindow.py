import uiScriptLocale

BOARD_WIDTH						= 192
BOARD_HIGHT						= 190

PATTERN_PATH					= "d:/ymir work/ui/pattern/"
ACHIEV_LEFT_PATTERN_X_COUNT		= 8
ACHIEV_LEFT_PATTERN_Y_COUNT		= 6
ACHIEV_PATTERN_X_COUNT			= 7
ACHIEV_PATTERN_Y_COUNT			= 6

window = {
	"name" : "AutoAttackOptionWindow",
	
	"x" : 0,
	"y" : 0,
	
	"style" : ("not_move","float",),
	
	"width"		: BOARD_WIDTH,
	"height"	: BOARD_HIGHT,
	
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			
			"x" : 0,
			"y" : 0,
			
			"width"		: BOARD_WIDTH,
			"height"	: BOARD_HIGHT,
			
			"children" :
			(
				## 백그라운드
				{ "name" : "auto_option_leftTop", "type" : "image", "style" : ("ltr",), "x" : 10, "y" : 10, "image" : PATTERN_PATH + "border_A_left_top.tga", },
				{ "name" : "auto_option_rightTop", "type" : "image", "style" : ("ltr",), "x" : 165, "y" : 10, "image" : PATTERN_PATH + "border_A_right_top.tga", },
				{ "name" : "auto_option_leftBottom", "type" : "image", "style" : ("ltr",), "x" : 10, "y" : 138, "image" : PATTERN_PATH + "border_A_left_bottom.tga", },
				{ "name" : "auto_option_rightBottom", "type" : "image", "style" : ("ltr",), "x" : 165, "y" : 138, "image" : PATTERN_PATH + "border_A_right_bottom.tga", },
				{ "name" : "auto_option_topCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 26, "y" : 10, "image" : PATTERN_PATH + "border_A_top.tga", "rect" : (0.0, 0.0, ACHIEV_LEFT_PATTERN_X_COUNT, 0), },
				{ "name" : "auto_option_leftCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 10, "y" : 26, "image" : PATTERN_PATH + "border_A_left.tga", "rect" : (0.0, 0.0, 0, ACHIEV_LEFT_PATTERN_Y_COUNT), },
				{ "name" : "auto_option_rightCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 165, "y" : 26, "image" : PATTERN_PATH + "border_A_right.tga", "rect" : (0.0, 0.0, 0, ACHIEV_LEFT_PATTERN_Y_COUNT), },
				{ "name" : "auto_option_bottomCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 26, "y" : 138, "image" : PATTERN_PATH + "border_A_bottom.tga", "rect" : (0.0, 0.0, ACHIEV_LEFT_PATTERN_X_COUNT, 0), },
				{ "name" : "auto_option_centerImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 26, "y" : 26, "image" : PATTERN_PATH + "border_A_center.tga", "rect" : (0.0, 0.0, ACHIEV_PATTERN_X_COUNT, ACHIEV_PATTERN_Y_COUNT), },
				
				## 레벨 범위
				{
					"name"	: "level_range", "type" : "image", "x" : 16, "y" : 16 * 8 + 5,
					"image" : "d:/ymir work/ui/game/auto_hunting/auto_attack_level_range_back_img.sub",
					"children":
					(
						{ "name" : "level_range_text", "type" : "text", "x" : 16, "y" : 0, "horizontal_align" : "left", "text" : uiScriptLocale.AUTO_OPTION_RANGE, },
					),
				},

				## 레벨 - 표시
				{
					"name"	: "level_range_min_img", "type" : "image", "x" : 89, "y" : 16 * 8 + 12,
					"image" : "d:/ymir work/ui/game/auto_hunting/auto_attack_level_range_min_img.sub",
				},

				## 레벨 최소				
				{
					"name"	: "level_range_min", "type" : "image", "x" : 98, "y" : 16 * 8 + 4,
					"image" : "d:/ymir work/ui/game/auto_hunting/auto_attack_level_range_edit_img.sub",
					"children":							
					(
						{
							"name" : "auto_attack_level_min", "type" : "editline","x" : 3, "y" : 2,
							"input_limit" : 3, "only_number" : 1, "width" : 70, "height" : 16,
						},
					),
				},

				## 물결
				{ "name" : "level_range_sign", "type" : "text", "x" : 126, "y" : 16 * 8 + 5, "text" : "~", },
				

				## 레벨 + 표시
				{
					"name"	: "level_range_min_img", "type" : "image", "x" : 140, "y" : 16 * 8 + 10,
					"image" : "d:/ymir work/ui/game/auto_hunting/auto_attack_level_range_max_img.sub",
				},

				## 레벨 최대				
				{
					"name"	: "level_range_max", "type" : "image", "x" : 148, "y" : 16 * 8 + 4,
					"image" : "d:/ymir work/ui/game/auto_hunting/auto_attack_level_range_edit_img.sub",
					"children":							
					(
						{
							"name" : "auto_attack_level_max", "type" : "editline","x" : 3, "y" : 2,
							"input_limit" : 3, "only_number" : 1, "width" : 70, "height" : 16,
						},
					),
				},

				## __clear_button
				{
					"name" : "ClearButton",
					"type" : "button",
					
					"x" : 19,
					"y" : BOARD_HIGHT - 31,

					"text" : uiScriptLocale.AUTO_OPTION_CLEAR,
					
					"default_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_01.sub",
					"over_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_02.sub",
					"down_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_03.sub",
				},

				## __accept_button
				{
					"name" : "AcceptButton",
					"type" : "button",
					
					"x" : 97,
					"y" : BOARD_HIGHT - 31,

					"text" : uiScriptLocale.AUTO_OPTION_ACCEPT,
					
					"default_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_01.sub",
					"over_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_02.sub",
					"down_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_03.sub",
				},
			),
		},
	),
}
