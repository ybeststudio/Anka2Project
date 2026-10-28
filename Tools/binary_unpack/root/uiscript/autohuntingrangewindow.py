import uiScriptLocale

BOARD_WIDTH						= 230
BOARD_HIGHT						= 110

PATTERN_PATH					= "d:/ymir work/ui/pattern/"
ACHIEV_LEFT_PATTERN_X_COUNT		= 10
ACHIEV_LEFT_PATTERN_Y_COUNT		= 1
ACHIEV_PATTERN_X_COUNT			= 10
ACHIEV_PATTERN_Y_COUNT			= 1

window = {
	"name" : "AutoHuntingRangeWindow",
	
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
			
			"title" : uiScriptLocale.AUTO_TITLE,
			
			"children" :
			(
				## 백그라운드
				{ "name" : "auto_option_leftTop", "type" : "image", "style" : ("ltr",), "x" : 10, "y" : 10, "image" : PATTERN_PATH + "border_A_left_top.tga", },
				{ "name" : "auto_option_rightTop", "type" : "image", "style" : ("ltr",), "x" : 200, "y" : 10, "image" : PATTERN_PATH + "border_A_right_top.tga", },
				{ "name" : "auto_option_leftBottom", "type" : "image", "style" : ("ltr",), "x" : 10, "y" : 55, "image" : PATTERN_PATH + "border_A_left_bottom.tga", },
				{ "name" : "auto_option_rightBottom", "type" : "image", "style" : ("ltr",), "x" : 200, "y" : 55, "image" : PATTERN_PATH + "border_A_right_bottom.tga", },
				{ "name" : "auto_option_topCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 26, "y" : 10, "image" : PATTERN_PATH + "border_A_top.tga", "rect" : (0.0, 0.0, ACHIEV_LEFT_PATTERN_X_COUNT, 0), },
				{ "name" : "auto_option_leftCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 10, "y" : 26, "image" : PATTERN_PATH + "border_A_left.tga", "rect" : (0.0, 0.0, 0, ACHIEV_LEFT_PATTERN_Y_COUNT), },
				{ "name" : "auto_option_rightCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 200, "y" : 26, "image" : PATTERN_PATH + "border_A_right.tga", "rect" : (0.0, 0.0, 0, ACHIEV_LEFT_PATTERN_Y_COUNT), },
				{ "name" : "auto_option_bottomCenterImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 26, "y" : 55, "image" : PATTERN_PATH + "border_A_bottom.tga", "rect" : (0.0, 0.0, ACHIEV_LEFT_PATTERN_X_COUNT, 0), },
				{ "name" : "auto_option_centerImg", "type" : "expanded_image", "style" : ("ltr",), "x" : 26, "y" : 26, "image" : PATTERN_PATH + "border_A_center.tga", "rect" : (0.0, 0.0, ACHIEV_PATTERN_X_COUNT, ACHIEV_PATTERN_Y_COUNT), },

				## 사냥 범위 슬라이더바
				{
					"name" : "hunting_range_sliderbar",
					"type" : "sliderbar",

					"x" : 0,
					"y" : BOARD_HIGHT - 85,

					"horizontal_align" : "center",
				},

				## 범위 표시 text
				{
					"name"	: "range_back_ui", "type" : "image", "x" : 0, "y" : BOARD_HIGHT - 65,
					"horizontal_align" : "center",
					"image" : "d:/ymir work/ui/game/windows/auto_system_edit_line.sub",
					"children":
					(
						{ "name" : "range_text", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", },
					), 
				},

				## __accept_button
				{
					"name" : "AcceptButton",
					"type" : "button",
					
					"x" : 0,
					"y" : BOARD_HIGHT - 31,

					"horizontal_align" : "center",

					"text" : uiScriptLocale.AUTO_RANGE_ACCEPT,
					
					"default_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_01.sub",
					"over_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_02.sub",
					"down_image"	: "d:/ymir work/ui/game/windows/auto_system_bottom_button_03.sub",
				},
			),
		},
	),
}
