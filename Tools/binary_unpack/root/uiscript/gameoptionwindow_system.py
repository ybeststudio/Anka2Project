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
	"name"		: "GameOptionWindow_System",
	
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

MAIN_TITLE_X			= 2
LINE_BEGIN				= 5
LINE_STEP				= 28
LINE_NUMBER				= 0
PARENT_CHILDREN_INDEX	= 2

BUTTON_TWO_LEFT_X		= -65
BUTTON_TWO_RIGHT_X		= 70

## 배경음
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				{
					"name" : "music",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "music_name",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"all_align":"center",
							"text" : uiScriptLocale.OPTION_MUSIC,
						},
					],
				},
				
				{
					"name" : "music_volume_controller",
					"type" : "sliderbar",
							
					"horizontal_align" : "center",

					"x" : 5,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 5,
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## 효과음
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [	
				{
					"name" : "sound",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "sound_name",
							"type" : "text",

							"x" : 0,
							"y" : 0,
							
							"all_align":"center",
							"text" : uiScriptLocale.OPTION_SOUND,
						},
					],
				},
				
				{
					"name" : "sound_volume_controller",
					"type" : "sliderbar",
							
					"horizontal_align" : "center",
					
					"x" : 5,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 5,
				},	
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

## 배경음 바꾸기
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [	
				{
					"name" : "bgm",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "sound_name",
							"type" : "text",

							"x" : 0,
							"y" : 0,
							
							"all_align":"center",
							"text" : uiScriptLocale.OPTION_MUSIC + " " + uiScriptLocale.OPTION_MUSIC_CHANGE,
						},
					],
				},
				{
					"name" : "bgm_button",
					"type" : "button",

					"x" : -87,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" : uiScriptLocale.OPTION_MUSIC_CHANGE,
					"horizontal_align" : "center",

					"default_image" : GAMEOPTION_PATH + "button_84_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_84_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_84_down.sub",
				},
				{
					"name" : "bgm_file_back_image",
					"type" : "image",

					"x" : 45,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 2,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "bgm_name_bg.sub",
					
					"children" :
					[				
						{
							"name" : "bgm_file",
							"type" : "text",

							"x" : 0,
							"y" : 0,
							"all_align":"center",
					
							"text" : uiScriptLocale.OPTION_MUSIC_DEFAULT_THEMA,
						},	
					],
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

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
							"name" : "camera_mode",
							"type" : "text",

							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE,
						},
					],
				},
				{
					"name" : "camera_short",
					"type" : "radio_button",

					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE_SHORT,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				
				{
					"name" : "camera_long",
					"type" : "radio_button",

					"x" : BUTTON_TWO_RIGHT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE_LONG,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [		
				## view_distance
				{
					"name" : "view_distance",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						## 가시거리
						{
							"name" : "view_distance_name",
							"type" : "text",

							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_VIEW_DISTANCE,
						},
					],
				},
			
				{
					"name" : "view_distance_level1",
					"type" : "radio_button",
					
					"x" : 20 + (37 * 0),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  "1",
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "view_distance_level2",
					"type" : "radio_button",
					
					"x" : 20 + (37 * 1),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  "2",
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "view_distance_level3",
					"type" : "radio_button",
					
					"x" : 20 + (37 * 2),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  "3",
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "view_distance_level4",
					"type" : "radio_button",
					
					"x" : 20 + (37 * 3),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  "4",
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "view_distance_level5",
					"type" : "radio_button",
					
					"x" : 20 + (37 * 4),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  "5",
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "view_distance_level6",
					"type" : "radio_button",
					
					"x" : 20 + (37 * 5),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  "6",
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "view_distance_level7",
					"type" : "radio_button",
					
					"x" : 20 + (37 * 6),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  "7",
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [		
				## fog
				{
					"name" : "fog",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" :"fog_mode",
							"type" : "text",

							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_FOG_MODE,
						},
					],
				},
				
				{
					"name" : "fog_mode_on",
					"type" : "radio_button",

					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_FOG_MODE_ON,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				
				{
					"name" : "fog_mode_off",
					"type" : "radio_button",

					"x" : BUTTON_TWO_RIGHT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_FOG_MODE_OFF,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [		
				## night
				{
					"name" : "night",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "night_mode",
							"type" : "text",

							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_NIGHT_MODE,
						},
					],
				},
				
				{
					"name" : "night_mode_on",
					"type" : "radio_button",

					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_NIGHT_MODE_ON,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				
				{
					"name" : "night_mode_off",
					"type" : "radio_button",

					"x" : BUTTON_TWO_RIGHT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_NIGHT_MODE_OFF,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [		
				## snow
				{
					"name" : "snow",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						## 눈 내리기 on/off
						{
							"name" : "snow_mode",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_SNOW_MODE,
						},
					],
				},
				
				{
					"name" : "snow_mode_on",
					"type" : "radio_button",

					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_SNOW_MODE_ON,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				
				{
					"name" : "snow_mode_off",
					"type" : "radio_button",

					"x" : BUTTON_TWO_RIGHT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_SNOW_MODE_OFF,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [		
				## shadow
				{
					"name" : "shadow",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						## 그림자 렌더 대상
						{
							"name" : "shadow_target",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_SHADOW_TARGET, 
						},
					],
				},
			
				{
					"name" : "shadow_target_none",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 0),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.OPTION_SHADOW_TARGET_NONE,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "shadow_target_ground",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 1),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.OPTION_SHADOW_TARGET_GROUND,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "shadow_target_ground_and_solo",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 2),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.OPTION_SHADOW_TARGET_GROUND_AND_SOLO,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
				
				{
					"name" : "shadow_target_all",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 3),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.OPTION_SHADOW_TARGET_ALL,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [	
				## shadow_quality_window
				{
					"name" : "shadow_quality_window",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "shadow_quality",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_SHADOW_QUALITY, 
						},
					],
				},			
				{
					"name" : "shadow_quality_bad",
					"type" : "radio_button",
					
					"x" : 20 + (88 * 0),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.OPTION_SHADOW_QUALITY_BAD,

					"default_image" : GAMEOPTION_PATH + "button_84_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_84_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_84_down.sub",
				},
			
				{
					"name" : "shadow_quality_average",
					"type" : "radio_button",
					
					"x" : 20 + (88 * 1),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.OPTION_SHADOW_QUALITY_AVERAGE,

					"default_image" : GAMEOPTION_PATH + "button_84_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_84_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_84_down.sub",
				},
			
				{
					"name" : "shadow_quality_good",
					"type" : "radio_button",
					
					"x" : 20 + (88 * 2),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.OPTION_SHADOW_QUALITY_GOOD,

					"default_image" : GAMEOPTION_PATH + "button_84_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_84_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_84_down.sub",
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## effect
				{
					"name" : "effect",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "effect_level",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL, 
						},
					],
				},
			
				{
					"name" : "effect_level1",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 0),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL1,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "effect_level2",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 1),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL2,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "effect_level3",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 2),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL3,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "effect_level4",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 3),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL4,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "effect_level5",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 4),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL5,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## privateShop
				{
					"name" : "privateShop",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "privateShop_level",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL, 
						},
					],
				},
			
				{
					"name" : "privateShop_level1",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 0),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL1,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "privateShop_level2",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 1),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL2,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "privateShop_level3",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 2),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL3,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "privateShop_level4",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 3),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL4,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "privateShop_level5",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 4),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL5,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [	
				## dropItem
				{
					"name" : "dropItem",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						## 그래픽 ON/OFF: Drop Item
						{
							"name" : "dropItem_level",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL, 
						},
					],
				},
			
				{
					"name" : "dropItem_level1",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 0),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL1,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "dropItem_level2",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 1),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL2,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "dropItem_level3",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 2),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL3,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "dropItem_level4",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 3),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL4,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
			
				{
					"name" : "dropItem_level5",
					"type" : "radio_button",
					
					"x" : 20 + (36 * 4),
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),

					"text" :  uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL5,
					
					"default_image" : GAMEOPTION_PATH + "button_36_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_36_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_36_down.sub",
				},
				{
					"name" : "dropItem_yang_text",
					"type" : "text",
							
					"x" : 20 + (36 * 5) + 20,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 5,

					"text" : "yang",
				},
				{
					"name" : "dropItem_yang_checkbox_uncheck_img",
					"type" : "image", 
					
					"x" : 20 + (36 * 6) + 20,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 5,

					"image" : "d:/ymir work/ui/game/looting/uncheck_box.sub",
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

TEXT_TEMPORARY_X = -10
window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## language_select
				{
					"name" : "language_select",
					"type" : "image", 
					
					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "language_select_text",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_LANGUAGE_SELECT,
						},
					],
				},
				{
					"name" : "language_change_window",
					"type" : "window",
		
					"x" : 30,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"width" : 305,
					"height" : (LINE_STEP * 2),
		
					"children" :
					[
						{
							"name" : "language_select_img",
							"type" : "image",
							"x" : TEXT_TEMPORARY_X,
							"y" : 0,
							"image" : "d:/ymir work/ui/quest_re/button_one.sub",
						},
						{
							"name" : "cur_language_text_window",
							"type" : "window",
				
							"x" : TEXT_TEMPORARY_X,
							"y" : 0,
							"width" : 210-16,
							"height" : 16,
				
							"children" :
							(
								{"name":"cur_language_text", "type":"text", "x":0, "y":0, "text": "-", "all_align" : "center"},
							),
						},
						{
							"name" : "language_select_button",
							"type" : "button",

							"x" : TEXT_TEMPORARY_X + 210 -16,
							"y" : 0,

							"default_image" : "d:/ymir work/ui/game/party_match/arrow_default.sub",
							"over_image" : "d:/ymir work/ui/game/party_match/arrow_over.sub",
							"down_image" : "d:/ymir work/ui/game/party_match/arrow_down.sub",
						},
						{
							"name" : "language_change_button",
							"type" : "button",

							"x" : TEXT_TEMPORARY_X + 210 + 10,
							"y" : 0,

							"text" : uiScriptLocale.OPTION_LANGUAGE_CHANGE,
				
							"default_image" : ROOT_PATH + "small_Button_01.sub",
							"over_image" : ROOT_PATH + "small_Button_02.sub",
							"down_image" : ROOT_PATH + "small_Button_03.sub",
							"disable_image" : ROOT_PATH + "small_Button_03.sub",
						},
						{
							"name" : "language_select_pivot_window",
							"type" : "window",

							"x" : TEXT_TEMPORARY_X,
							"y" : 0,
							"width" : 210,
							"height" : 0,
						},
					],
				},
	]
LINE_NUMBER		+= 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER
