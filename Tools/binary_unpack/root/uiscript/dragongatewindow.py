import uiScriptLocale
import localeInfo

DRAGON_GATE_PATH			= "d:/ymir work/ui/game/dragon_gate/"
PUBLIC_PATH					= "d:/ymir work/ui/public/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"

WINDOW_WIDTH				= 544
WINDOW_HEIGHT				= 434

BACKGROUND_X				= 10
BACKGROUND_Y				= 32

MAIN_WINDOW_X				= 253
MAIN_WINDOW_Y				= 32

MAIN_WINDOW_WIDTH			= 276
MAIN_WINDOW_HEIGHT			= 362

PATTERN_X_COUNT				= 15
PATTERN_Y_COUNT				= 20

if localeInfo.IsARABIC():
	STEP_1_X				= 71 - (BACKGROUND_X * 2)
	STEP_2_X				= 124 - (BACKGROUND_X * 2)
	STEP_3_X				= 195 - (BACKGROUND_X * 2)
	STEP_4_X				= 135 - (BACKGROUND_X * 2)
	STEP_5_X				= 77 - (BACKGROUND_X * 2)
	STEP_6_X				= 64 - (BACKGROUND_X * 2)
	STEP_7_X				= 42 - (BACKGROUND_X * 2)
	STEP_8_X				= 57 - (BACKGROUND_X * 2)
	STEP_9_X				= 117 - (BACKGROUND_X * 2)
	STEP_10_X				= 122 - (BACKGROUND_X * 2)
	STEP_11_X				= 187 - (BACKGROUND_X * 2)
	STEP_12_X				= 214 - (BACKGROUND_X * 2)
	STEP_13_X				= 188 - (BACKGROUND_X * 2)
	STEP_14_X				= 167 - (BACKGROUND_X * 2)
	STEP_15_X				= 115 - (BACKGROUND_X * 2)
else:
	STEP_1_X				= 175 - BACKGROUND_X
	STEP_2_X				= 122 - BACKGROUND_X
	STEP_3_X				= 51 - BACKGROUND_X
	STEP_4_X				= 111 - BACKGROUND_X
	STEP_5_X				= 169 - BACKGROUND_X
	STEP_6_X				= 182 - BACKGROUND_X
	STEP_7_X				= 204 - BACKGROUND_X
	STEP_8_X				= 189 - BACKGROUND_X
	STEP_9_X				= 129 - BACKGROUND_X
	STEP_10_X				= 124 - BACKGROUND_X
	STEP_11_X				= 59 - BACKGROUND_X
	STEP_12_X				= 32 - BACKGROUND_X
	STEP_13_X				= 58 - BACKGROUND_X
	STEP_14_X				= 79 - BACKGROUND_X
	STEP_15_X				= 131 - BACKGROUND_X
	
STEP_1_Y					= 346 - BACKGROUND_Y
STEP_2_Y					= 308 - BACKGROUND_Y
STEP_3_Y					= 274 - BACKGROUND_Y
STEP_4_Y					= 241 - BACKGROUND_Y
STEP_5_Y					= 206 - BACKGROUND_Y
STEP_6_Y					= 265 - BACKGROUND_Y
STEP_7_Y					= 150 - BACKGROUND_Y
STEP_8_Y					= 84 - BACKGROUND_Y
STEP_9_Y					= 116 - BACKGROUND_Y
STEP_10_Y					= 57 - BACKGROUND_Y
STEP_11_Y					= 78 - BACKGROUND_Y
STEP_12_Y					= 196 - BACKGROUND_Y
STEP_13_Y					= 125 - BACKGROUND_Y
STEP_14_Y					= 172 - BACKGROUND_Y
STEP_15_Y					= 172 - BACKGROUND_Y

if localeInfo.IsARABIC():
	MY_POINT_TEXT_X			= 174
	
	ABILITY_NUMBER_X		= 240
	ABILITY_NUMBER_BAR_X	= 225
	
	ABILITY_BOARD_TEXT_X	= 207
	ABILITY_BOARD_LOCK_X	= 225
	
else:
	MY_POINT_TEXT_X			= 211
	
	ABILITY_NUMBER_X		= 26
	ABILITY_NUMBER_BAR_X	= 10
	
	ABILITY_BOARD_TEXT_X	= 70
	ABILITY_BOARD_LOCK_X	= 11
	
ABILITY_BOARD_X				= 6
ABILITY_BOARD_Y				= 5
ABILITY_BOARD_INTERVER		= 57
ABILITY_BOARD_TEXT_Y		= -155
ABILITY_BOARD_LOCK_Y		= 13

ABILITY_NUMBER_Y			= 14
ABILITY_NUMBER_BASE_Y		= 13
ABILITY_NUMBER_BAR_Y		= 11

REINFORCE_TEXT_X			= 121
REINFORCE_TEXT_Y			= 403

window = {
	"name"	: "DragonGateWindow",
	"style" : ("movable", "float",),
	
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT + 20, 
	
	"children" :
	(
		## BOARD
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			
			"x" : 0,
			"y" : 0,
			
			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,
			
			"title" : "",
			
			"children" :
			(
				{
					"name"	: "dragon_gate_bg",
					"type" : "image",
					
					"x" : BACKGROUND_X,
					"y" : BACKGROUND_Y,
					
					"image" : DRAGON_GATE_PATH + "dragon_gate_bg.sub",
					
					"children":
					(
						{
							"name" : "dragon_gate_fade",
							"type" : "expanded_image",
							
							"x" : 0,
							"y" : 0,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_bg_fade_out.sub",
							
							"x_scale" : 30.75,
							"y_scale" : 45.25,
						},
						
						## default
						{ "name" : "dragon_gate_default_1", "type" : "image", "x" : STEP_1_X, "y" : STEP_1_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_1_default.sub", },
						{ "name" : "dragon_gate_default_2", "type" : "image", "x" : STEP_2_X, "y" : STEP_2_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_2_default.sub", },
						{ "name" : "dragon_gate_default_3", "type" : "image", "x" : STEP_3_X, "y" : STEP_3_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_3_default.sub", },
						{ "name" : "dragon_gate_default_4", "type" : "image", "x" : STEP_4_X, "y" : STEP_4_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_4_default.sub", },
						{ "name" : "dragon_gate_default_5", "type" : "image", "x" : STEP_5_X, "y" : STEP_5_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_5_default.sub", },
						{ "name" : "dragon_gate_default_6", "type" : "image", "x" : STEP_6_X, "y" : STEP_6_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_6_default.sub", },
						{ "name" : "dragon_gate_default_7", "type" : "image", "x" : STEP_7_X, "y" : STEP_7_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_7_default.sub", },
						{ "name" : "dragon_gate_default_8", "type" : "image", "x" : STEP_8_X, "y" : STEP_8_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_8_default.sub", },
						{ "name" : "dragon_gate_default_9", "type" : "image", "x" : STEP_9_X, "y" : STEP_9_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_9_default.sub", },
						{ "name" : "dragon_gate_default_10", "type" : "image", "x" : STEP_10_X, "y" : STEP_10_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_10_default.sub", },
						{ "name" : "dragon_gate_default_11", "type" : "image", "x" : STEP_11_X, "y" : STEP_11_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_11_default.sub", },
						{ "name" : "dragon_gate_default_12", "type" : "image", "x" : STEP_12_X, "y" : STEP_12_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_12_default.sub", },
						{ "name" : "dragon_gate_default_13", "type" : "image", "x" : STEP_13_X, "y" : STEP_13_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_13_default.sub", },
						{ "name" : "dragon_gate_default_14", "type" : "image", "x" : STEP_14_X, "y" : STEP_14_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_14_default.sub", },
						{ "name" : "dragon_gate_default_15", "type" : "image", "x" : STEP_15_X, "y" : STEP_15_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_15_default.sub", },
						
						{ "name" : "dragon_gate_default_16", "type" : "image", "x" : STEP_1_X, "y" : STEP_1_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_16_default.sub", },
						{ "name" : "dragon_gate_default_17", "type" : "image", "x" : STEP_2_X, "y" : STEP_2_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_17_default.sub", },
						{ "name" : "dragon_gate_default_18", "type" : "image", "x" : STEP_3_X, "y" : STEP_3_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_18_default.sub", },
						{ "name" : "dragon_gate_default_19", "type" : "image", "x" : STEP_4_X, "y" : STEP_4_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_19_default.sub", },
						{ "name" : "dragon_gate_default_20", "type" : "image", "x" : STEP_5_X, "y" : STEP_5_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_20_default.sub", },
						{ "name" : "dragon_gate_default_21", "type" : "image", "x" : STEP_6_X, "y" : STEP_6_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_21_default.sub", },
						{ "name" : "dragon_gate_default_22", "type" : "image", "x" : STEP_7_X, "y" : STEP_7_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_22_default.sub", },
						{ "name" : "dragon_gate_default_23", "type" : "image", "x" : STEP_8_X, "y" : STEP_8_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_23_default.sub", },
						{ "name" : "dragon_gate_default_24", "type" : "image", "x" : STEP_9_X, "y" : STEP_9_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_24_default.sub", },
						{ "name" : "dragon_gate_default_25", "type" : "image", "x" : STEP_10_X, "y" : STEP_10_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_25_default.sub", },
						{ "name" : "dragon_gate_default_26", "type" : "image", "x" : STEP_11_X, "y" : STEP_11_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_26_default.sub", },
						{ "name" : "dragon_gate_default_27", "type" : "image", "x" : STEP_12_X, "y" : STEP_12_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_27_default.sub", },
						{ "name" : "dragon_gate_default_28", "type" : "image", "x" : STEP_13_X, "y" : STEP_13_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_28_default.sub", },
						{ "name" : "dragon_gate_default_29", "type" : "image", "x" : STEP_14_X, "y" : STEP_14_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_29_default.sub", },
						{ "name" : "dragon_gate_default_30", "type" : "image", "x" : STEP_15_X, "y" : STEP_15_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_30_default.sub", },
						
						## over
						{ "name" : "dragon_gate_over_1", "type" : "image", "x" : STEP_1_X, "y" : STEP_1_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_1_over.sub", },
						{ "name" : "dragon_gate_over_2", "type" : "image", "x" : STEP_2_X, "y" : STEP_2_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_2_over.sub", },
						{ "name" : "dragon_gate_over_3", "type" : "image", "x" : STEP_3_X, "y" : STEP_3_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_3_over.sub", },
						{ "name" : "dragon_gate_over_4", "type" : "image", "x" : STEP_4_X, "y" : STEP_4_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_4_over.sub", },
						{ "name" : "dragon_gate_over_5", "type" : "image", "x" : STEP_5_X, "y" : STEP_5_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_5_over.sub", },
						{ "name" : "dragon_gate_over_6", "type" : "image", "x" : STEP_6_X, "y" : STEP_6_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_6_over.sub", },
						{ "name" : "dragon_gate_over_7", "type" : "image", "x" : STEP_7_X, "y" : STEP_7_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_7_over.sub", },
						{ "name" : "dragon_gate_over_8", "type" : "image", "x" : STEP_8_X, "y" : STEP_8_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_8_over.sub", },
						{ "name" : "dragon_gate_over_9", "type" : "image", "x" : STEP_9_X, "y" : STEP_9_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_9_over.sub", },
						{ "name" : "dragon_gate_over_10", "type" : "image", "x" : STEP_10_X, "y" : STEP_10_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_10_over.sub", },
						{ "name" : "dragon_gate_over_11", "type" : "image", "x" : STEP_11_X, "y" : STEP_11_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_11_over.sub", },
						{ "name" : "dragon_gate_over_12", "type" : "image", "x" : STEP_12_X, "y" : STEP_12_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_12_over.sub", },
						{ "name" : "dragon_gate_over_13", "type" : "image", "x" : STEP_13_X, "y" : STEP_13_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_13_over.sub", },
						{ "name" : "dragon_gate_over_14", "type" : "image", "x" : STEP_14_X, "y" : STEP_14_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_14_over.sub", },
						{ "name" : "dragon_gate_over_15", "type" : "image", "x" : STEP_15_X, "y" : STEP_15_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_15_over.sub", },
						
						{ "name" : "dragon_gate_over_16", "type" : "image", "x" : STEP_1_X, "y" : STEP_1_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_16_over.sub", },
						{ "name" : "dragon_gate_over_17", "type" : "image", "x" : STEP_2_X, "y" : STEP_2_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_17_over.sub", },
						{ "name" : "dragon_gate_over_18", "type" : "image", "x" : STEP_3_X, "y" : STEP_3_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_18_over.sub", },
						{ "name" : "dragon_gate_over_19", "type" : "image", "x" : STEP_4_X, "y" : STEP_4_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_19_over.sub", },
						{ "name" : "dragon_gate_over_20", "type" : "image", "x" : STEP_5_X, "y" : STEP_5_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_20_over.sub", },
						{ "name" : "dragon_gate_over_21", "type" : "image", "x" : STEP_6_X, "y" : STEP_6_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_21_over.sub", },
						{ "name" : "dragon_gate_over_22", "type" : "image", "x" : STEP_7_X, "y" : STEP_7_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_22_over.sub", },
						{ "name" : "dragon_gate_over_23", "type" : "image", "x" : STEP_8_X, "y" : STEP_8_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_23_over.sub", },
						{ "name" : "dragon_gate_over_24", "type" : "image", "x" : STEP_9_X, "y" : STEP_9_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_24_over.sub", },
						{ "name" : "dragon_gate_over_25", "type" : "image", "x" : STEP_10_X, "y" : STEP_10_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_25_over.sub", },
						{ "name" : "dragon_gate_over_26", "type" : "image", "x" : STEP_11_X, "y" : STEP_11_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_26_over.sub", },
						{ "name" : "dragon_gate_over_27", "type" : "image", "x" : STEP_12_X, "y" : STEP_12_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_27_over.sub", },
						{ "name" : "dragon_gate_over_28", "type" : "image", "x" : STEP_13_X, "y" : STEP_13_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_28_over.sub", },
						{ "name" : "dragon_gate_over_29", "type" : "image", "x" : STEP_14_X, "y" : STEP_14_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_29_over.sub", },
						{ "name" : "dragon_gate_over_30", "type" : "image", "x" : STEP_15_X, "y" : STEP_15_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_step_30_over.sub", },
						
						{ "name" : "dragon_gate_number_small", "type" : "image", "x" : 0, "y" : 0, "image" : DRAGON_GATE_PATH + "dragon_gate_flash_1.sub", },
						{ "name" : "dragon_gate_number_big", "type" : "image", "x" : 0, "y" : 0, "image" : DRAGON_GATE_PATH + "dragon_gate_flash_2.sub", },
						
						## my point
						{ "name" : "dragon_gate_my_point_bar", "type" : "image", "x" : 152, "y" : 8, "image" : DRAGON_GATE_PATH + "dragon_gate_my_point_bar.sub", },
						{ "name" : "dragon_gate_my_point_value", "type" : "text", "x" : 80, "y" : -162, "text" : "0", "all_align" : "center", },
					),
				},
				
				## main bg
				{
					"name" : "main_bg_window",
					"type" : "window",
					
					"style" : ("attach", "ltr",),

					"x" : MAIN_WINDOW_X,
					"y" : MAIN_WINDOW_Y,
					
					"width" : MAIN_WINDOW_WIDTH,
					"height" : MAIN_WINDOW_HEIGHT,
					
					"children" :
					(
						## LeftTop 1
						{
							"name" : "left_top",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						## RightTop 2
						{
							"name" : "right_top",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 13,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						## LeftBottom 3
						{
							"name" : "left_bottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom 4
						{
							"name" : "right_bottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 13,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						## topcenterImg 5
						{
							"name" : "top_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## leftcenterImg 6
						{
							"name" : "left_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "right_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 13,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "bottom_center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "center_img",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, PATTERN_X_COUNT, PATTERN_Y_COUNT),
						},
						
						{ "name" : "dragon_gate_ability_board_line", "type" : "image", "x" : ABILITY_BOARD_X + 133, "y" : ABILITY_BOARD_Y + 56, "image" : DRAGON_GATE_PATH + "dragon_gate_board_line.sub", },
						
						## list
						{ "name" : "dragon_gate_ability_board_0", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_open.sub", },
						{ "name" : "dragon_gate_ability_board_1", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + ABILITY_BOARD_INTERVER, "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_open.sub", },
						{ "name" : "dragon_gate_ability_board_2", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 2), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_open.sub", },
						{ "name" : "dragon_gate_ability_board_3", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 3), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_open.sub", },
						{ "name" : "dragon_gate_ability_board_4", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 4), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_open.sub", },
						{ "name" : "dragon_gate_ability_board_5", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 5), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_open.sub", },
						
						
						{ "name" : "dragon_gate_ability_board_color_blue", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_board_blue.sub", },
						{ "name" : "dragon_gate_ability_board_color_red", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_board_red.sub", },
						
						## ability_info
						{ "name" : "dragon_gate_ability_info_0", "type" : "text", "x" : ABILITY_BOARD_TEXT_X, "y" : ABILITY_BOARD_TEXT_Y, "text" : localeInfo.DRAGON_GATE_REINFORCE_APPLY_DEFAULT_TOOLTIP, "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "vertical_align" : "center" },
						{ "name" : "dragon_gate_ability_info_1", "type" : "text", "x" : ABILITY_BOARD_TEXT_X, "y" : ABILITY_BOARD_TEXT_Y + 11 + ABILITY_BOARD_INTERVER, "text" : "", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "vertical_align" : "center" },
						{ "name" : "dragon_gate_ability_info_2", "type" : "text", "x" : ABILITY_BOARD_TEXT_X, "y" : ABILITY_BOARD_TEXT_Y + 11 + (ABILITY_BOARD_INTERVER * 2), "text" : "", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "vertical_align" : "center" },
						{ "name" : "dragon_gate_ability_info_3", "type" : "text", "x" : ABILITY_BOARD_TEXT_X, "y" : ABILITY_BOARD_TEXT_Y + 11 + (ABILITY_BOARD_INTERVER * 3), "text" : "", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "vertical_align" : "center" },
						{ "name" : "dragon_gate_ability_info_4", "type" : "text", "x" : ABILITY_BOARD_TEXT_X, "y" : ABILITY_BOARD_TEXT_Y + 11 + (ABILITY_BOARD_INTERVER * 4), "text" : "", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "vertical_align" : "center" },
						{ "name" : "dragon_gate_ability_info_5", "type" : "text", "x" : ABILITY_BOARD_TEXT_X, "y" : ABILITY_BOARD_TEXT_Y + 11 + (ABILITY_BOARD_INTERVER * 5), "text" : "", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "vertical_align" : "center" },
						
						## list_black
						{ "name" : "dragon_gate_ability_board_black_1", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + ABILITY_BOARD_INTERVER, "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_close.sub", },
						{ "name" : "dragon_gate_ability_board_black_2", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 2), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_close.sub", },
						{ "name" : "dragon_gate_ability_board_black_3", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 3), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_close.sub", },
						{ "name" : "dragon_gate_ability_board_black_4", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 4), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_close.sub", },
						{ "name" : "dragon_gate_ability_board_black_5", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 5), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_close.sub", },
						
						## list_no_benefit
						{ "name" : "dragon_gate_ability_board_no_benefit_1", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + ABILITY_BOARD_INTERVER, "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_no_benefit.sub", },
						{ "name" : "dragon_gate_ability_board_no_benefit_2", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 2), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_no_benefit.sub", },
						{ "name" : "dragon_gate_ability_board_no_benefit_3", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 3), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_no_benefit.sub", },
						{ "name" : "dragon_gate_ability_board_no_benefit_4", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 4), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_no_benefit.sub", },
						{ "name" : "dragon_gate_ability_board_no_benefit_5", "type" : "image", "x" : ABILITY_BOARD_X, "y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 5), "image" : DRAGON_GATE_PATH + "dragon_gate_ability_board_no_benefit.sub", },
						
						## ability_number_bar
						{ "name" : "dragon_gate_ability_number_bar_default_0", "type" : "image", "x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X, "y" : ABILITY_BOARD_Y + ABILITY_NUMBER_BAR_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_default.sub", },
						{ "name" : "dragon_gate_ability_number_bar_blue_0", "type" : "image", "x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X, "y" : ABILITY_BOARD_Y + ABILITY_NUMBER_BAR_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_blue.sub", },
						{ "name" : "dragon_gate_ability_number_bar_red_0", "type" : "image", "x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X, "y" : ABILITY_BOARD_Y + ABILITY_NUMBER_BAR_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_red.sub", },
						
						{
							"name" : "dragon_gate_ability_number_bar_default_1",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + ABILITY_BOARD_INTERVER + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_default.sub",
						},
						{
							"name" : "dragon_gate_ability_number_bar_blue_1",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + ABILITY_BOARD_INTERVER + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_blue.sub",
						},
						{
							"name" : "dragon_gate_ability_number_bar_red_1",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + ABILITY_BOARD_INTERVER + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_red.sub",
						},
						{
							"name" : "bonus_1",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X + 20,
							"y" : ABILITY_BOARD_Y + 10 + ABILITY_BOARD_INTERVER + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_bonus.sub",
						},
						
						{
							"name" : "dragon_gate_ability_number_bar_default_2",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 2) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_default.sub",
						},
						{
							"name" : "dragon_gate_ability_number_bar_blue_2",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 2) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_blue.sub",
						},
						{
							"name" : "dragon_gate_ability_number_bar_red_2",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 2) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_red.sub",
						},
						{
							"name" : "bonus_2",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X + 20,
							"y" : ABILITY_BOARD_Y + 10 + (ABILITY_BOARD_INTERVER * 2) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_bonus.sub",
						},
						
						{
							"name" : "dragon_gate_ability_number_bar_default_3",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 3) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_default.sub",
						},
						{
							"name" : "dragon_gate_ability_number_bar_blue_3",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 3) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_blue.sub",
						},
						{
							"name" : "dragon_gate_ability_number_bar_red_3",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 3) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_red.sub",
						},
						{
							"name" : "bonus_3",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X + 20,
							"y" : ABILITY_BOARD_Y + 10 + (ABILITY_BOARD_INTERVER * 3) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_bonus.sub",
						},
						
						{
							"name" : "dragon_gate_ability_number_bar_default_4",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 4) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_default.sub",
						},
						{
							"name" : "dragon_gate_ability_number_bar_blue_4",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 4) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_blue.sub",
						},
						{
							"name" : "dragon_gate_ability_number_bar_red_4",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 4) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_red.sub",
						},
						{
							"name" : "bonus_4",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X + 20,
							"y" : ABILITY_BOARD_Y + 10 + (ABILITY_BOARD_INTERVER * 4) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_bonus.sub",
						},
						
						{
							"name" : "dragon_gate_ability_number_bar_default_5",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 5) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_default.sub",
						},
						{
							"name" : "dragon_gate_ability_number_bar_blue_5",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 5) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_blue.sub",
						},
						{
							"name" : "dragon_gate_ability_number_bar_red_5",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X,
							"y" : ABILITY_BOARD_Y + 11 + (ABILITY_BOARD_INTERVER * 5) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_red.sub",
						},
						{
							"name" : "bonus_5",
							"type" : "image",
							
							"x" : ABILITY_BOARD_X + ABILITY_NUMBER_BAR_X + 20,
							"y" : ABILITY_BOARD_Y + 10 + (ABILITY_BOARD_INTERVER * 5) + ABILITY_NUMBER_BAR_Y,
							
							"image" : DRAGON_GATE_PATH + "dragon_gate_board_number_bar_bonus.sub",
						},
						
						## ability_number
						{ "name" : "dragon_gate_ability_number_0", "type" : "text", "x" : ABILITY_BOARD_X + ABILITY_NUMBER_X, "y" : ABILITY_NUMBER_BASE_Y + ABILITY_NUMBER_Y, "text" : "-", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "text_horizontal_align" : "center", },
						{ "name" : "dragon_gate_ability_number_1", "type" : "text", "x" : ABILITY_BOARD_X + ABILITY_NUMBER_X, "y" : ABILITY_NUMBER_BASE_Y + ABILITY_NUMBER_Y + 11 + ABILITY_BOARD_INTERVER, "text" : "", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "text_horizontal_align" : "center", },
						{ "name" : "dragon_gate_ability_number_2", "type" : "text", "x" : ABILITY_BOARD_X + ABILITY_NUMBER_X, "y" : ABILITY_NUMBER_BASE_Y + ABILITY_NUMBER_Y + 11 + (ABILITY_BOARD_INTERVER * 2), "text" : "", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "text_horizontal_align" : "center", },
						{ "name" : "dragon_gate_ability_number_3", "type" : "text", "x" : ABILITY_BOARD_X + ABILITY_NUMBER_X, "y" : ABILITY_NUMBER_BASE_Y + ABILITY_NUMBER_Y + 11 + (ABILITY_BOARD_INTERVER * 3), "text" : "", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "text_horizontal_align" : "center", },
						{ "name" : "dragon_gate_ability_number_4", "type" : "text", "x" : ABILITY_BOARD_X + ABILITY_NUMBER_X, "y" : ABILITY_NUMBER_BASE_Y + ABILITY_NUMBER_Y + 11 + (ABILITY_BOARD_INTERVER * 4), "text" : "", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "text_horizontal_align" : "center", },
						{ "name" : "dragon_gate_ability_number_5", "type" : "text", "x" : ABILITY_BOARD_X + ABILITY_NUMBER_X, "y" : ABILITY_NUMBER_BASE_Y + ABILITY_NUMBER_Y + 11 + (ABILITY_BOARD_INTERVER * 5), "text" : "", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "text_horizontal_align" : "center", },
						
						## lock
						{ "name" : "dragon_gate_ability_board_lock_1", "type" : "image", "x" : ABILITY_BOARD_X + ABILITY_BOARD_LOCK_X, "y" : ABILITY_BOARD_Y + 11 + ABILITY_BOARD_LOCK_Y + ABILITY_BOARD_INTERVER, "image" : DRAGON_GATE_PATH + "dragon_gate_lock.sub", },
						{ "name" : "dragon_gate_ability_board_lock_2", "type" : "image", "x" : ABILITY_BOARD_X + ABILITY_BOARD_LOCK_X, "y" : ABILITY_BOARD_Y + 11 + ABILITY_BOARD_LOCK_Y + (ABILITY_BOARD_INTERVER * 2), "image" : DRAGON_GATE_PATH + "dragon_gate_lock.sub", },
						{ "name" : "dragon_gate_ability_board_lock_3", "type" : "image", "x" : ABILITY_BOARD_X + ABILITY_BOARD_LOCK_X, "y" : ABILITY_BOARD_Y + 11 + ABILITY_BOARD_LOCK_Y + (ABILITY_BOARD_INTERVER * 3), "image" : DRAGON_GATE_PATH + "dragon_gate_lock.sub", },
						{ "name" : "dragon_gate_ability_board_lock_4", "type" : "image", "x" : ABILITY_BOARD_X + ABILITY_BOARD_LOCK_X, "y" : ABILITY_BOARD_Y + 11 + ABILITY_BOARD_LOCK_Y + (ABILITY_BOARD_INTERVER * 4), "image" : DRAGON_GATE_PATH + "dragon_gate_lock.sub", },
						{ "name" : "dragon_gate_ability_board_lock_5", "type" : "image", "x" : ABILITY_BOARD_X + ABILITY_BOARD_LOCK_X, "y" : ABILITY_BOARD_Y + 11 + ABILITY_BOARD_LOCK_Y + (ABILITY_BOARD_INTERVER * 5), "image" : DRAGON_GATE_PATH + "dragon_gate_lock.sub", },
					),
				},
				
				{ "name" : "dragon_gate_line", "type" : "image", "x" : MAIN_WINDOW_X, "y" : MAIN_WINDOW_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_line.sub", },
				
				## need point
				{ "name" : "dragon_gate_need_point", "type" : "image", "x" : 12, "y" : 404, "image" : DRAGON_GATE_PATH + "dragon_gate_need_point.sub", },
				{ "name" : "dragon_gate_need_point_bar", "type" : "image", "x" : 31, "y" : 403, "image" : DRAGON_GATE_PATH + "dragon_gate_need_point_bar.sub", },
				{ "name" : "dragon_gate_need_point_value", "type" : "text", "x" : 56, "y" : 406, "text" : "0", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "text_horizontal_align" : "center" },
				
				## reinforce button
				{
					"name" : "reinforce_button",
					"type" : "button",
					
					"x" : 20,
					"y" : 357,
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_reinforce_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_reinforce_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_reinforce_button_down.sub",
					
					"children" :
					(
						## reinforce count
						{ "name" : "dragon_gate_reinforce_count_bar", "type" : "image", "x" : 64 - 20, "y" : 362 - 357, "image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_count_bar.sub", },
						{ "name" : "dragon_gate_reinforce_count_value", "type" : "text", "x" : 88 - 20, "y" : 365 - 357, "text" : "0/10", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "text_horizontal_align" : "center" },
					),
				},
				
				## clear button
				{
					"name" : "clear_button",
					"type" : "button",
					
					"x" : 287,
					"y" : 399,
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_clear_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_clear_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_clear_button_down.sub",
				},
				
				## step upgrade button
				{
					"name" : "step_upgrade_button",
					"type" : "button",
					
					"x" : 464,
					"y" : 398,
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_step_upgrade_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_step_upgrade_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_step_upgrade_button_down.sub",
					"disable_image" : DRAGON_GATE_PATH + "dragon_gate_step_upgrade_button_disable.sub",
				},
				
				## page bar
				{ "name" : "dragon_gate_page_bar", "type" : "image", "x" : 405, "y" : 403, "image" : DRAGON_GATE_PATH + "dragon_gate_page_bar.sub", },
				{ "name" : "dragon_gate_page_value", "type" : "text", "x" : 419, "y" : 406, "text" : "1", "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "text_horizontal_align" : "center" },
				
				## prev button
				{
					"name" : "prev_button",
					"type" : "button",
					
					"x" : 387,
					"y" : 407,
					
					"default_image"	: PUBLIC_PATH + "public_page_button/page_prev_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_page_button/page_prev_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_page_button/page_prev_btn_01.sub",
				},
				
				## next button
				{
					"name" : "next_button",
					"type" : "button",
					
					"x" : 443,
					"y" : 407,
					
					"default_image"	: PUBLIC_PATH + "public_page_button/page_next_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_page_button/page_next_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_page_button/page_next_btn_01.sub",
				},
				
				## side button
				{
					"name" : "side_button",
					"type" : "button",
					
					"x" : -2,
					"y" : 143,
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_side_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_side_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_side_down.sub",
				},
				
				## side reverse button
				{
					"name" : "side_reverse_button",
					"type" : "button",
					
					"x" : -2,
					"y" : 143,
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_reverse_side_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_reverse_side_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_reverse_side_down.sub",
				},
				
				## tooltip button
				{
					"name" : "tooltip_button",
					"type" : "button",
					
					"x" : 495,
					"y" : 9,
					
					"default_image"	: PATTERN_PATH + "q_mark_01.tga",
					"over_image"	: PATTERN_PATH + "q_mark_02.tga",
					"down_image"	: PATTERN_PATH + "q_mark_01.tga",
				},
				
				## step clear button
				{
					"name" : "step_clear_button",
					"type" : "button",
					
					"x" : 323,
					"y" : 399,
					
					"default_image"	: DRAGON_GATE_PATH + "dragon_gate_step_clear_button_default.sub",
					"over_image"	: DRAGON_GATE_PATH + "dragon_gate_step_clear_button_over.sub",
					"down_image"	: DRAGON_GATE_PATH + "dragon_gate_step_clear_button_down.sub",
					
					"text" : "",
				},
			),
		},
		
		## reinforce bottom
		{ "name" : "reinforce_bottom_bar", "type" : "image", "x" : REINFORCE_TEXT_X, "y" : REINFORCE_TEXT_Y, "image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_menu.sub", },
		{ "name" : "reinforce_text", "type" : "text", "x" : REINFORCE_TEXT_X + 54, "y" : REINFORCE_TEXT_Y + 3, "text" : uiScriptLocale.DRAGON_GATE_REINFORCE_TEXT, "r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0, "text_horizontal_align" : "center" },
		
		## reinforce text button list
		{ "name" : "reinforce_bottom_menu_img", "type" : "image", "x" : REINFORCE_TEXT_X, "y" : REINFORCE_TEXT_Y + 17, "image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_bottom_menu_window.sub", },
		
		## base reinforce text button
		{
			"name" : "base_reinforce_text_button",
			"type" : "button",
			
			"x" : REINFORCE_TEXT_X + 2,
			"y" : REINFORCE_TEXT_Y + 19,
			
			"default_image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_bottom_menu_default.sub",
			"over_image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_bottom_menu_over.sub",
			"down_image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_bottom_menu_default.sub",
			
			"text" : uiScriptLocale.DRAGON_GATE_REINFORCE_TEXT,
		},

		## reinforce select button
		{
			"name" : "reinforce_select_button",
			"type" : "button",
			
			"x" : 228,
			"y" : 404,
			
			"default_image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_bottom_arrow_default.sub",
			"over_image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_bottom_arrow_over.sub",
			"down_image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_bottom_arrow_down.sub",
		},
		
		## auto reinforce text button
		{
			"name" : "auto_reinforce_text_button",
			"type" : "button",
			
			"x" : REINFORCE_TEXT_X + 2,
			"y" : REINFORCE_TEXT_Y + 34,
			
			"default_image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_bottom_menu_default.sub",
			"over_image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_bottom_menu_over.sub",
			"down_image" : DRAGON_GATE_PATH + "dragon_gate_reinforce_bottom_menu_default.sub",
			
			"text" : uiScriptLocale.DRAGON_GATE_AUTO_REINFORCE_TEXT,
		},
	),
}