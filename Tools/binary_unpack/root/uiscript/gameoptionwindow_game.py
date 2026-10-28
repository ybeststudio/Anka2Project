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
	"name"		: "GameOptionWindow_Game",
	
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
			"name" : "scroll_bar",
			"type" : "scrollbar",

			"x" : 286,
			"y" : 4,
			"size" : 380,
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

if app.ENABLE_SLOT_CHAR_ENLARGEMENT:
	BUTTON_THREE_LEFT_X = -87

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [

				## PvP 모드
				{
					"name" : "pvp_mode_img",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "pvp_mode",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_PVPMODE,
						},
					],
				},
				{
					"name" : "pvp_peace",
					"type" : "radio_button",
					
					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_PVPMODE_PEACE,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				{
					"name" : "pvp_revenge",
					"type" : "radio_button",
					
					"x" : 70,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_PVPMODE_REVENGE,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				{
					"name" : "pvp_guild",
					"type" : "radio_button",
					
					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 2),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_PVPMODE_GUILD,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				{
					"name" : "pvp_free",
					"type" : "radio_button",
					
					"x" : 70,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 2),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_PVPMODE_FREE,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
	]
LINE_NUMBER += 3
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [

				## 이름색상
				{
					"name" : "name_color_img",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[						
						{
							"name" : "name_color",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_NAME_COLOR,
						},
					],
				},
				{
					"name" : "name_color_normal",
					"type" : "radio_button",
					
					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_NAME_COLOR_NORMAL,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				{
					"name" : "name_color_empire",
					"type" : "radio_button",
					
					"x" : BUTTON_TWO_RIGHT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_NAME_COLOR_EMPIRE,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
	]	
LINE_NUMBER += 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [

				## 이름보기
				{
					"name" : "always_show_name_img",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "always_show_name",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME,
						},
					],
				},
				{
					"name" : "always_show_name_on_button",
					"type" : "radio_button",
					
					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_ON,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				{
					"name" : "always_show_name_off_button",
					"type" : "radio_button",
					
					"x" : BUTTON_TWO_RIGHT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_OFF,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
	]
LINE_NUMBER += 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				## 몹정보
				{
					"name" : "show_mob_info_img",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "show_mob_info",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.OPTION_MOB_INFO,
						},
					],
				},
				{
					"name" : "show_mob_level_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_MOB_INFO_LEVEL,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				{
					"name" : "show_mob_AI_flag_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_RIGHT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_MOB_INFO_AGGR,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
	] 
LINE_NUMBER += 2
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER
		
if app.ENABLE_OPTIMIZATION:
	window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
					## 피격 모션
					{
						"name" : "other_char_attacked_img",
						"type" : "image", 

						"x" : MAIN_TITLE_X, 
						"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
						"horizontal_align" : "center",

						"image" : GAMEOPTION_PATH + "list_tab_01.sub",

						"children" :
						[
							{
								"name" : "other_char_attacked",
								"type" : "text",
							
								"x" : 0,
								"y" : 0,
								"all_align":"center",

								"text" : uiScriptLocale.OPTION_ATTACKED_MOTION,
							},
						],
					},
					{
						"name" : "other_char_attacked_all",
						"type" : "radio_button",
					
						"x" : BUTTON_TWO_LEFT_X,
						"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
						"horizontal_align" : "center",

						"text" : uiScriptLocale.OPTION_ATTACKED_MOTION_ALL,
					
						"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
						"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
						"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
					},
					{
						"name" : "other_char_attacked_self",
						"type" : "radio_button",
					
						"x" : BUTTON_TWO_RIGHT_X,
						"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
						"horizontal_align" : "center",

						"text" : uiScriptLocale.OPTION_ATTACKED_MOTION_SELF,
					
						"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
						"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
						"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
					},

		]
	LINE_NUMBER += 2
	window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
	
				## 보이기
				{
					"name" : "show_img",
					"type" : "image", 

					"x" : MAIN_TITLE_X, 
					"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
					"horizontal_align" : "center",

					"image" : GAMEOPTION_PATH + "list_tab_01.sub",

					"children" :
					[
						{
							"name" : "show_text",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							"all_align":"center",

							"text" : uiScriptLocale.GAME_OPTION_SHOW,
						},
					],
				},
				{
					"name" : "show_target_info_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_TARGET_BOARD,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				{
					"name" : "show_chatting_window_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_RIGHT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_VIEW_CHAT,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},	
				{
					"name" : "show_sales_text_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 2),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_SALESTEXT,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				{
					"name" : "show_damage_text_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_RIGHT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 2),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.OPTION_EFFECT,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},	
				{
					"name" : "show_npc_name_text_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 3),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.GRAPHICONOFF_NPC_NAME_STATUS,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},	
				{
					"name" : "show_monster_name_text_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_RIGHT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 3),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.GAME_OPTION_GAME_MONSTER_NAME,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},	
				{
					"name" : "show_structure_view_mode_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 4),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.STRUCTURE_VIEW_TYPE_STRUCTURE,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},
				{
					"name" : "show_pet_text_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_RIGHT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 4),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.GRAPHICONOFF_PET_STATUS,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},	
				{
					"name" : "show_hp_bar_text_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 5),
					"horizontal_align" : "center",

					"text" : uiScriptLocale.GAME_OPTION_GAME_HP_BAR,
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},	

	]	
	
if app.ENABLE_TITLE_SYSTEM:
	if app.ENABLE_QUICKSLOT_IMPROVE:
		SET_LINE_NUMBER = LINE_NUMBER + 5
		# On/Off 옵션 - 타이틀
		window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
					{
						"name" : "show_player_title_button",
						"type" : "toggle_button",
						
						"x" : BUTTON_TWO_RIGHT_X,
						"y" : LINE_BEGIN + LINE_STEP * SET_LINE_NUMBER,
						"horizontal_align" : "center",

						"text" : uiScriptLocale.GAME_OPTION_PLAYER_TITLE,					# 타이틀
						
						"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
						"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
						"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
					},				
			]
	else:
		# On/Off 옵션 - 타이틀
		window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
					{
						"name" : "show_player_title_button",
						"type" : "toggle_button",
						
						"x" : BUTTON_TWO_RIGHT_X,
						"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 5),
						"horizontal_align" : "center",

						"text" : uiScriptLocale.GAME_OPTION_PLAYER_TITLE,					# 타이틀
						
						"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
						"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
						"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
					},				
			]

if app.ENABLE_QUICKSLOT_IMPROVE:
	SET_LINE_NUMBER = LINE_NUMBER + 6
	# On/Off 옵션 - 퀵슬롯 단축키
	window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [
				{
					"name" : "show_quickslot_shortcut_button",
					"type" : "toggle_button",
					
					"x" : BUTTON_TWO_LEFT_X,
					"y" : LINE_BEGIN + LINE_STEP * SET_LINE_NUMBER,
					"horizontal_align" : "center",

					"text" : uiScriptLocale.GAME_OPTION_QUICKSLOT_SHORTCUT,				# 퀵슬롯 단축키
					
					"default_image" : GAMEOPTION_PATH + "button_128_default.sub",
					"over_image"	: GAMEOPTION_PATH + "button_128_over.sub",
					"down_image"	: GAMEOPTION_PATH + "button_128_down.sub",
				},				
		]

if app.ENABLE_QUICKSLOT_IMPROVE:
	LINE_NUMBER = SET_LINE_NUMBER + 1
else:
	LINE_NUMBER += 6
window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

if app.ENABLE_SLOT_CHAR_ENLARGEMENT:
	window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [		
					## 슬롯 문자 확대
					{
						"name"				: "slot_char_enlarge_title_image",
						"type"				: "image", 
					
						"x"					: MAIN_TITLE_X, 
						"y"					: LINE_BEGIN + LINE_STEP * LINE_NUMBER,
						"horizontal_align"	: "center",

						"image"				: GAMEOPTION_PATH + "list_tab_01.sub",

						"children" :
						[
							{
								"name"		: "slot_char_enlarge_title_text",
								"type"		: "text",

								"x"			: 0,
								"y"			: 0,
								"all_align"	:"center",

								"text"		: uiScriptLocale.SLOT_CHAR_ENLARGE_TITLE,	# 슬롯 문자 확대
							},
						],
					},
						
					## ON 버튼
					{
						"name"				: "slot_char_enlarge_on_button",
						"type"				: "radio_button",
						"horizontal_align"	: "center",

						"x"					: BUTTON_THREE_LEFT_X,
						"y"					: LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 3,

						"default_image"		: ROOT_PATH + "large_button_01.sub",
						"over_image"		: ROOT_PATH + "large_button_02.sub",
						"down_image"		: ROOT_PATH + "large_button_03.sub",
					
						"text"				: uiScriptLocale.SLOT_CHAR_ENLARGE_ON,	# ON
					},

					# OFF 버튼
					{
						"name"				: "slot_char_enlarge_off_button",
						"type"				: "radio_button",
						"horizontal_align"	: "center",
					
						"x"					: BUTTON_THREE_LEFT_X + 90,
						"y"					: LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 3,
					
						"default_image"		: ROOT_PATH + "large_button_01.sub",
						"over_image"		: ROOT_PATH + "large_button_02.sub",
						"down_image"		: ROOT_PATH + "large_button_03.sub",
					
						"text"				: uiScriptLocale.SLOT_CHAR_ENLARGE_OFF,	# OFF
					},
					
					# KEY 버튼
					{
						"name"				: "slot_char_enlarge_key_button",
						"type"				: "radio_button",
						"horizontal_align"	: "center",
					
						"x"					: BUTTON_THREE_LEFT_X + 180,
						"y"					: LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 3,
					
						"default_image"		: ROOT_PATH + "large_button_01.sub",
						"over_image"		: ROOT_PATH + "large_button_02.sub",
						"down_image"		: ROOT_PATH + "large_button_03.sub",
					
						"text"				: uiScriptLocale.SLOT_CHAR_ENLARGE_KEY,	# KEY
					},
		]
	LINE_NUMBER		+= 2
	window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER

if app.ENABLE_LEFT_SEAT:
	window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [		
					## 자리비움
					{
						"name" : "left_seat_time_bar_text_img",
						"type" : "image", 
					
						"x" : MAIN_TITLE_X, 
						"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
						"horizontal_align" : "center",

						"image" : GAMEOPTION_PATH + "list_tab_01.sub",

						"children" :
						[
							## 자리비움
							{
								"name" : "left_seat_time_bar_text",
								"type" : "text",

								"x" : 0,
								"y" : 0,
								"all_align":"center",

								"text" : uiScriptLocale.LEFT_SEAT,
							},
						],
					},
						
					## 자리비움 대기시간
					{ 
						"name" : "left_seat_time_list_button", 
						"type" : "button", 
						
						"x" : 0, 
						"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 3,
						"horizontal_align" : "center",

						"text" : uiScriptLocale.LEFT_SEAT_10_MIN, 
						"default_image"	: "d:/ymir work/ui/quest_re/button_one.sub", 
						"over_image"	: "d:/ymir work/ui/quest_re/button_one.sub", 
						"down_image"	: "d:/ymir work/ui/quest_re/button_one.sub", 
					},

					{ 
						"name" : "left_seat_list_time_arrow_button", 
						"type" : "button", 
						
						"x" : 57,
						"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 3,
						"horizontal_align" : "right",

						"default_image" : "d:/ymir work/ui/game/party_match/arrow_default.sub", 
						"over_image" : "d:/ymir work/ui/game/party_match/arrow_over.sub", 
						"down_image" : "d:/ymir work/ui/game/party_match/arrow_down.sub", 
					},
		]
	LINE_NUMBER		+= 2
	window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER
	
	window["children"][PARENT_CHILDREN_INDEX]["children"] = window["children"][PARENT_CHILDREN_INDEX]["children"] + [		
					## 자리비움 로그아웃
					{
						"name" : "left_seat_logout_bar_img",
						"type" : "image", 
					
						"x" : MAIN_TITLE_X, 
						"y" : LINE_BEGIN + LINE_STEP * LINE_NUMBER,
						"horizontal_align" : "center",

						"image" : GAMEOPTION_PATH + "list_tab_01.sub",

						"children" :
						[
							## 로그아웃
							{
								"name" : "left_seat_logout_bar_text",
								"type" : "text",

								"x" : 0,
								"y" : 0,
								"all_align":"center",

								"text" : uiScriptLocale.LEFT_SEAT_LOGOUT,
							},
						],
					},	
					
					## 자리비움 로그아웃 대기시간
					{ 
						"name" : "left_seat_logout_list_button", 
						"type" : "button",
						
						"x" : 0, 
						"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 3,
						"horizontal_align" : "center",

						"text" : uiScriptLocale.LEFT_SEAT_180_MIN, 
						"default_image"	: "d:/ymir work/ui/quest_re/button_one.sub", 
						"over_image"	: "d:/ymir work/ui/quest_re/button_one.sub", 
						"down_image"	: "d:/ymir work/ui/quest_re/button_one.sub", 
					},

					{ 
						"name" : "left_seat_logout_list_arrow_button", 
						"type" : "button", 
											
						"x" : 57,
						"y" : LINE_BEGIN + LINE_STEP * (LINE_NUMBER + 1) + 3,
						"horizontal_align" : "right",

						"default_image" : "d:/ymir work/ui/game/party_match/arrow_default.sub", 
						"over_image" : "d:/ymir work/ui/game/party_match/arrow_over.sub", 
						"down_image" : "d:/ymir work/ui/game/party_match/arrow_down.sub", 
					},
		]
	LINE_NUMBER		+= 2
	window["children"][PARENT_CHILDREN_INDEX]["height"] = LINE_BEGIN + LINE_STEP * LINE_NUMBER
