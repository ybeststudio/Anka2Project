import uiScriptLocale
import app

GAMEOPTION_PATH			= "d:/ymir work/ui/gameoption/"
ROOT_PATH				= "d:/ymir work/ui/public/"
PATTERN_PATH			= "d:/ymir work/ui/pattern/"
PATTERN_WINDOW_WIDTH	= 294
PATTERN_WINDOW_HEIGHT	= 390
PATTERN_X_COUNT			= (PATTERN_WINDOW_WIDTH - 32) / 16
PATTERN_Y_COUNT			= (PATTERN_WINDOW_HEIGHT - 32) / 16

window = {
	"name"		: "GameOptionWindow_KeyChange",
	
	"x"			: 0,
	"y"			: 32,

	"width"		: PATTERN_WINDOW_WIDTH + 5,
	"height"	: PATTERN_WINDOW_HEIGHT - 5,

	"children"	:
	[
		## base pattern
		{
			"name" : "base_pattern",
			"type" : "window",
			"style" : ("attach", "ltr",),
					
			"x" : 10,
			"y" : 0,
			"width" : PATTERN_WINDOW_WIDTH,
			"height" :PATTERN_WINDOW_HEIGHT,
					
			"children" :
			[
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
			],
		},
		## 목록들 마스킹 윈도우
		{
			"name" : "list_window_masking",
			"type" : "window",
			"style" : ("attach", "ltr",),
					
			"x" : 0,
			"y" : 5,

			"width" : PATTERN_WINDOW_WIDTH,
			"height" :PATTERN_WINDOW_HEIGHT - 8,
		},
		## 시스템 옵션 목록 상위 윈도우
		{
			"name" : "list_window",
			"type" : "window",
			"style" : ("attach", "ltr",),
					
			"x" : 0,
			"y" : 0,

			"width" : PATTERN_WINDOW_WIDTH,
			"height" : 0,
			"children" :
			[
			],
		},

		## scroll bar
		{
			"name"	: "scroll_bar",
			"type"	: "scrollbar",

			"x"		: 286,
			"y"		: 4,
			"size"	: 380,
		},
	],
}

KEY_LOCALE_INFO_X		= -46
MAIN_TITLE_X			= 2
LINE_BEGIN				= 5
LINE_STEP				= 28
LINE_NUMBER				= 0
PARENT_CHILDREN_INDEX	= 2

## 기본 동작
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## base_action
				{
					"name" : "base_action",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "base_action_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align":"center",
							"text" : uiScriptLocale.KEYCHANGE_BASEACTION,
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## up1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## up1
				{
					"name" : "up1", "type" : "image",  "x" : -46, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "up1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_UP1,
						},
					],
				},
				## up1
				{
					"name" : "up1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "up1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "W",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## down1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## down1
				{
					"name" : "down1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "down1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_DOWN1,
						},
					],
				},
				## down1
				{
					"name" : "down1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "down1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "S",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## left1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## left1
				{
					"name" : "left1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "left1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_LEFT1,
						},
					],
				},
				## left1
				{
					"name" : "left1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "left1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "A",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## right1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## right1
				{
					"name" : "right1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "right1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_RIGHT1,
						},
					],
				},
				## right1
				{
					"name" : "right1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "right1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "D",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## up2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## up2
				{
					"name" : "up2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "up2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_UP2,
						},
					],
				},
				## up2
				{
					"name" : "up2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "up2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "UP",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## down2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## down2
				{
					"name" : "down2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "down2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_DOWN2,
						},
					],
				},
				## down2
				{
					"name" : "down2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "down2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "DOWN",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## left2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## left2
				{
					"name" : "left2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "left2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_LEFT2,
						},
					],
				},
				## left2
				{
					"name" : "left2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "left2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "LEFT",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## right2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## right2
				{
					"name" : "right2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "right2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_RIGHT2,
						},
					],
				},
				## right2
				{
					"name" : "right2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "right2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "RIGHT",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## screenshot ( 변경 안됨 )
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## screenshot
				{
					"name" : "screenshot", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "screenshot", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_SCREENSHOT,
						},
					],
				},
				## screenshot
				{
					"name" : "screenshot_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "screenshot_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "PRINTSCREEN",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## autorun
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## autorun
				{
					"name" : "autorun", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "autorun", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_AUTORUN,
						},
					],
				},
				## autorun
				{
					"name" : "autorun_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "autorun_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "NUMLOOK",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## attack
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## attack
				{
					"name" : "attack", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "attack", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_ATTACK,
						},
					],
				},
				## attack
				{
					"name" : "attack_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "attack_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "SPACE",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## next_target
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## next_target
				{
					"name" : "next_target", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "next_target", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_NEXT_TARGET,
						},
					],
				},
				## next_target
				{
					"name" : "next_target_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "next_target_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "TAB",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## root1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## root1
				{
					"name" : "root1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "root1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_ROOT1,
						},
					],
				},
				## root1
				{
					"name" : "root1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "root1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "`",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## root2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## root2
				{
					"name" : "root2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "root2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_ROOT2,
						},
					],
				},
				## root2
				{
					"name" : "root2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "root2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "Z",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## 마운트
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## mount
				{
					"name" : "mount",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "mount_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align":"center",
							"text" : uiScriptLocale.GAME_OPTION_KEYSETTING_MOUNT,
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## ride
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## ride
				{
					"name" : "ride", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "ride", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_RIDE,
						},
					],
				},
				## ride
				{
					"name" : "ride_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "ride_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + G",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## keychange_ride_horse
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## keychange_ride_horse
				{
					"name" : "keychange_ride_horse", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "keychange_ride_horse", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_RIDE_HORSE,
						},
					],
				},
				## keychange_ride_horse
				{
					"name" : "keychange_ride_horse_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "keychange_ride_horse_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + H",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## ride_bye
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## ride_bye
				{
					"name" : "ride_bye", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "ride_bye", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_RIDE_BYE,
						},
					],
				},
				## ride_bye
				{
					"name" : "ride_bye_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "ride_bye_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + B",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## ride_peed
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## ride_peed
				{
					"name" : "ride_peed", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "ride_peed", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_RIDE_PEED,
						},
					],
				},
				## ride_bye
				{
					"name" : "ride_peed_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "ride_peed_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + F",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

# 인터페이스
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## interface
				{
					"name" : "interface",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "interface_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align":"center",
							"text" : uiScriptLocale.KEYCHANGE_INTERFACE,
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## status_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## status_window
				{
					"name" : "status_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "status_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_STATUS_WINDOW,
						},
					],
				},
				## status_window
				{
					"name" : "status_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "status_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "C",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## skill_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## skill_window
				{
					"name" : "skill_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "skill_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_SKILL_WINDOW,
						},
					],
				},
				## skill_window
				{
					"name" : "skill_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "skill_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "V",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## quest_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## quest_window
				{
					"name" : "quest_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "quest_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_QUEST_WINDOW,
						},
					],
				},
				## quest_window
				{
					"name" : "quest_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "quest_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "N",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## inventory_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## inventory_window
				{
					"name" : "inventory_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "inventory_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_INVENTORY_WINDOW,
						},
					],
				},
				## inventory_window
				{
					"name" : "inventory_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "inventory_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "I",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## dds_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## dds_window
				{
					"name" : "dds_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "dds_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_DDS_WINDOW,
						},
					],
				},
				## dds_window
				{
					"name" : "dds_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "dds_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "O",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## minimap_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## minimap_window
				{
					"name" : "minimap_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "minimap_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_MINIMAP_WINDOW,
						},
					],
				},
				## minimap_window
				{
					"name" : "minimap_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "minimap_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "M",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## chat_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## chat_window
				{
					"name" : "chat_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "chat_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CHAT_WINDOW,
						},
					],
				},
				## chat_window
				{
					"name" : "chat_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "chat_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "L",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## quest_scroll_onoff
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## quest_scroll_onoff
				{
					"name" : "quest_scroll_onoff", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "quest_scroll_onoff", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_QUEST_SCROLL_ONOFF,
						},
					],
				},
				## quest_scroll_onoff
				{
					"name" : "quest_scroll_onoff_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "quest_scroll_onoff_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + Q",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## guild_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## guild_window
				{
					"name" : "guild_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "guild_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_GUILD_WINDOW,
						},
					],
				},
				## guild_window
				{
					"name" : "guild_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "guild_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "ALT + G",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## messenger_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## messenger_window
				{
					"name" : "messenger_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "messenger_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_MESSENGER_WINDOW,
						},
					],
				},
				## messenger_window
				{
					"name" : "messenger_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "messenger_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "ALT + M",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## help_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## help_window
				{
					"name" : "help_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "help_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_HELP_WINDOW,
						},
					],
				},
				## help_window
				{
					"name" : "help_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "help_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "H",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## action_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## action_window
				{
					"name" : "action_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "action_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_ACTION_WINDOW,
						},
					],
				},
				## action_window
				{
					"name" : "action_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "action_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "B",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## minimap_plus
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## minimap_plus
				{
					"name" : "minimap_plus", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "minimap_plus", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_MINIMAP_PLUS,
						},
					],
				},
				## minimap_plus
				{
					"name" : "minimap_plus_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "minimap_plus_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "+",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## minimap_miner
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## minimap_miner
				{
					"name" : "minimap_miner", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "minimap_miner", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_MINIMAP_MINER,
						},
					],
				},
				## minimap_miner
				{
					"name" : "minimap_miner_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "minimap_miner_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "-",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## use_prism ( 변경 안됨 )
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## use_prism
				{
					"name" : "use_prism", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "use_prism", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_USE_PRISM,
						},
					],
				},
				## use_prism
				{
					"name" : "use_prism_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "use_prism_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "ALT",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

if app.ENABLE_SLOT_CHAR_ENLARGEMENT:
	window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
			# 슬롯 문자 확대
			{
				"name" : "slot_char_enlarge", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
				"children" :
				[
					{
						"name" : "slot_char_enlarge_text", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
						"text" : uiScriptLocale.SLOT_CHAR_ENLARGE_KEYCHANGE,	# 슬롯 문자 확대
					},
				],
			},
			{
				"name" : "slot_char_enlarge_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
				"children" :
				[
					{
						"name" : "slot_char_enlarge_key_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
						"text" : "CTRL",
					},
				],
			},
		]
	LINE_NUMBER		+= 1
	window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## show_name_bar
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## show_name_bar
				{
					"name" : "show_name_bar", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "show_name_bar", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_SHOW_NAME_BAR,
						},
					],
				},
				## show_name_bar
				{
					"name" : "show_name_bar_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "show_name_bar_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "X",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## pet_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## pet_window
				{
					"name" : "pet_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "pet_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_PET_WINDOW,
						},
					],
				},
				## pet_window
				{
					"name" : "pet_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "pet_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "P",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## auto_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## auto_window
				{
					"name" : "auto_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "auto_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_AUTO_WINDOW,
						},
					],
				},
				## auto_window
				{
					"name" : "auto_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "auto_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "K",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## monster_card_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## monster_card_window
				{
					"name" : "monster_card_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "monster_card_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_MONSTER_CARD_WINDOW,
						},
					],
				},
				## monster_card_window
				{
					"name" : "monster_card_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "monster_card_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "J",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## party_match_window
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## party_match_window
				{
					"name" : "party_match_window", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "party_match_window", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_PARTY_MATCH_WINDOW,
						},
					],
				},
				## party_match_window
				{
					"name" : "party_match_window_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "party_match_window_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "SHIFT + J",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## system ( 변경 안됨 )
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## system
				{
					"name" : "system", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "system", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_ESC,
						},
					],
				},
				## party_match_window
				{
					"name" : "system_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "system_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "ESC",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

if app.ENABLE_DRAGON_GATE:
	## dragon_gate
	window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
					## dragon_gate
					{
						"name" : "dragon_gate", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
						"children" :
						[
							{
								"name" : "dragon_gate", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
								"text" : uiScriptLocale.KEYCHANGE_DRAGON_GATE,
							},
						],
					},
					## dragon_gate
					{
						"name" : "dragon_gate_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
						"children" :
						[
							{
								"name" : "dragon_gate_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
								"text" : "Y",
							},
						],
					},
		]
	LINE_NUMBER		+= 1
	window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## mercenary
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## mercenary
				{
					"name" : "mercenary", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "mercenary", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_MERCENARY,
						},
					],
				},
				## mercenary
				{
					"name" : "mercenary_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "mercenary_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "U",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## ingameevent
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## ingameevent
				{
					"name" : "ingameevent", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "ingameevent", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_INGAMEEVENT,
						},
					],
				},
				## ingameevent
				{
					"name" : "ingameevent_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "ingameevent_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + E",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## 카메라
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## camera
				{
					"name" : "camera",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "camera_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align":"center",
							"text" : uiScriptLocale.GAME_OPTION_KEYSETTING_CAMERA,
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_right1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_right1
				{
					"name" : "cam_right1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_right1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_RIGHT1,
						},
					],
				},
				## cam_right1
				{
					"name" : "cam_right1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_right1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "E",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_left1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_left1
				{
					"name" : "cam_left1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_left1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_LEFT1,
						},
					],
				},
				## cam_left1
				{
					"name" : "cam_left1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_left1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "Q",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_zoom_in1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_zoom_in1
				{
					"name" : "cam_zoom_in1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_zoom_in1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_ZOOM_IN1,
						},
					],
				},
				## cam_zoom_in1
				{
					"name" : "cam_zoom_in1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_zoom_in1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "R",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_zoom_out1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_zoom_out1
				{
					"name" : "cam_zoom_out1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_zoom_out1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_ZOOM_OUT1,
						},
					],
				},
				## cam_zoom_out1
				{
					"name" : "cam_zoom_out1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_zoom_out1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "F",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_down1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_down1
				{
					"name" : "cam_down1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_down1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_DOWN1,
						},
					],
				},
				## cam_down1
				{
					"name" : "cam_down1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_down1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "T",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_up1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_up1
				{
					"name" : "cam_up1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_up1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_UP1,
						},
					],
				},
				## cam_up1
				{
					"name" : "cam_up1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_up1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "G",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_right2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_right2
				{
					"name" : "cam_right2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_right2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_RIGHT2,
						},
					],
				},
				## cam_right2
				{
					"name" : "cam_right2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_right2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "NUMPAD6",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_left2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_left2
				{
					"name" : "cam_left2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_left2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_LEFT2,
						},
					],
				},
				## cam_left2
				{
					"name" : "cam_left2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_left2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "NUMPAD4",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_zoom_in2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_zoom_in2
				{
					"name" : "cam_zoom_in2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_zoom_in2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_ZOOM_IN2,
						},
					],
				},
				## cam_zoom_in2
				{
					"name" : "cam_zoom_in2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_zoom_in2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "PAGEUP",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_zoom_out2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_zoom_out2
				{
					"name" : "cam_zoom_out2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_zoom_out2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_ZOOM_OUT2,
						},
					],
				},
				## cam_zoom_out2
				{
					"name" : "cam_zoom_out2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_zoom_out2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "PAGEDOWN",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_down2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_down2
				{
					"name" : "cam_down2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_down2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_DOWN2,
						},
					],
				},
				## cam_down2
				{
					"name" : "cam_down2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_down2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "NUMPAD8",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## cam_up2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## cam_up2
				{
					"name" : "cam_up2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "cam_up2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_CAM_UP2,
						},
					],
				},
				## cam_up2
				{
					"name" : "cam_up2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "cam_up2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "NUMPAD2",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## 슬롯 단축키
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## quick_slot
				{
					"name" : "quick_slot",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "quick_slot_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align":"center",
							"text" : uiScriptLocale.KEYCHANGE_SLOT_KET,
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## slot1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## slot1
				{
					"name" : "slot1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "slot1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_SLOT1,
						},
					],
				},
				## slot1
				{
					"name" : "slot1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "slot1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "1",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## slot2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## slot2
				{
					"name" : "slot2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "slot1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_SLOT2,
						},
					],
				},
				## slot2
				{
					"name" : "slot2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "slot2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "2",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## slot3
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## slot3
				{
					"name" : "slot3", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "slot1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_SLOT3,
						},
					],
				},
				## slot3
				{
					"name" : "slot3_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "slot3_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "3",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## slot4
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## slot4
				{
					"name" : "slot4", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "slot4", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_SLOT4,
						},
					],
				},
				## slot4
				{
					"name" : "slot4_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "slot4_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "4",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## slot5
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## slot5
				{
					"name" : "slot5", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "slot4", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_SLOT5,
						},
					],
				},
				## slot5
				{
					"name" : "slot5_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "slot5_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "F1",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## slot6
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## slot6
				{
					"name" : "slot6", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "slot6", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_SLOT6,
						},
					],
				},
				## slot6
				{
					"name" : "slot6_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "slot6_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "F2",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## slot7
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## slot7
				{
					"name" : "slot7", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "slot7", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_SLOT7,
						},
					],
				},
				## slot7
				{
					"name" : "slot7_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "slot7_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "F3",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## slot8
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## slot8
				{
					"name" : "slot8", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "slot8", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_SLOT8,
						},
					],
				},
				## slot8
				{
					"name" : "slot8_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "slot8_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "F4",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## quickslot1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## quickslot1
				{
					"name" : "quickslot1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "quickslot1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_QUICKSLOT1,
						},
					],
				},
				## quickslot1
				{
					"name" : "quickslot1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "quickslot1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "SHIFT + 1",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## quickslot2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## quickslot2
				{
					"name" : "quickslot2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "quickslot2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_QUICKSLOT2,
						},
					],
				},
				## quickslot2
				{
					"name" : "quickslot2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "quickslot2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "SHIFT + 2",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## quickslot3
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## quickslot3
				{
					"name" : "quickslot3", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "quickslot3", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_QUICKSLOT3,
						},
					],
				},
				## quickslot3
				{
					"name" : "quickslot3_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "quickslot3_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "SHIFT + 3",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## quickslot4
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## quickslot4
				{
					"name" : "quickslot4", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "quickslot3", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_QUICKSLOT4,
						},
					],
				},
				## quickslot4
				{
					"name" : "quickslot4_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "quickslot4_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "SHIFT + 4",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## 감정표현
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## emotion
				{
					"name" : "emotion",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "emotion_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align":"center",
							"text" : uiScriptLocale.GAME_OPTION_KEYSETTING_EMOTION,
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## emotion1
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## emotion1
				{
					"name" : "emotion1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "emotion1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_EMOTION1,
						},
					],
				},
				## emotion1
				{
					"name" : "emotion1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "emotion1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + 1",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## emotion2
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## emotion2
				{
					"name" : "emotion2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "emotion2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_EMOTION2,
						},
					],
				},
				## emotion2
				{
					"name" : "emotion2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "emotion2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + 2",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## emotion3
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## emotion3
				{
					"name" : "emotion3", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "emotion3", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_EMOTION3,
						},
					],
				},
				## emotion3
				{
					"name" : "emotion3_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "emotion3_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + 3",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## emotion4
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## emotion4
				{
					"name" : "emotion4", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "emotion3", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_EMOTION4,
						},
					],
				},
				## emotion4
				{
					"name" : "emotion4_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "emotion4_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + 4",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## emotion5
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## emotion5
				{
					"name" : "emotion5", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "emotion5", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_EMOTION5,
						},
					],
				},
				## emotion5
				{
					"name" : "emotion5_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "emotion5_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + 5",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## emotion6
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## emotion6
				{
					"name" : "emotion6", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "emotion6", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_EMOTION6,
						},
					],
				},
				## emotion6
				{
					"name" : "emotion6_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "emotion6_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + 6",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## emotion7
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## emotion7
				{
					"name" : "emotion7", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "emotion7", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_EMOTION7,
						},
					],
				},
				## emotion7
				{
					"name" : "emotion7_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "emotion7_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + 7",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## emotion8
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## emotion8
				{
					"name" : "emotion8", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "emotion8", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_EMOTION8,
						},
					],
				},
				## emotion8
				{
					"name" : "emotion8_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "emotion8_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + 8",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## emotion9
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## emotion9
				{
					"name" : "emotion9", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "emotion9", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_EMOTION9,
						},
					],
				},
				## emotion9
				{
					"name" : "emotion9_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "emotion9_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + 9",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## 기타
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## etc
				{
					"name" : "etc",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "etc_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align":"center",
							"text" : uiScriptLocale.GAME_OPTION_KEYSETTING_ETC,
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## 용혼석[천]
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## dss1
				{
					"name" : "dss1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "dss1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_DSS1,
						},
					],
				},
				## dss1
				{
					"name" : "dss1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "dss1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + C",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## 용혼석[지]
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## dss2
				{
					"name" : "dss2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "dss2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_DSS2,
						},
					],
				},
				## dss2
				{
					"name" : "dss2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "dss2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + V",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## 유물[상]
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## passive_attr1
				{
					"name" : "passive_attr1", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "passive_attr1", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_PASSIVE_ATTR1,
						},
					],
				},
				## passive_attr1
				{
					"name" : "passive_attr1_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "passive_attr1_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + O",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## 유물[하]
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## passive_attr2
				{
					"name" : "passive_attr2", "type" : "image",  "x" : KEY_LOCALE_INFO_X, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : GAMEOPTION_PATH + "keychange_tab_02.sub",
					"children" :
					[
						{
							"name" : "passive_attr2", "type" : "text",  "x" : 0, "y" : 0, "all_align":"center", 
							"text" : uiScriptLocale.KEYCHANGE_PASSIVE_ATTR2,
						},
					],
				},
				## passive_attr1
				{
					"name" : "passive_attr2_img", "type" : "image", "x" : 85, "y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER, "horizontal_align" : "center", "image" : ROOT_PATH + "Parameter_Slot_03.sub",
					"children" :
					[
						{
							"name" : "passive_attr2_text", "type" : "text", "x" : 0, "y" : 0, "all_align":"center",
							"text" : "CONTROL + P",
						},
					],
				},
	]
LINE_NUMBER		+= 1
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER









