import uiScriptLocale
import app

PATTERN_PATH			= "d:/ymir work/ui/pattern/"
ROOT_PATH				= "d:/ymir work/ui/public/"
CHATTING_PATH			= "d:/ymir work/ui/chat/"

WINDOW_WIDTH			= 254
WINDOW_HEIGHT			= 520

PATTERN_WINDOW_WIDTH	= 232
PATTERN_WINDOW_HEIGHT	= 454


PATTERN_X_COUNT = (PATTERN_WINDOW_WIDTH - 32) / 16
PATTERN_Y_COUNT = (PATTERN_WINDOW_HEIGHT - 32) / 16

window = {
	"name" : "ChatSettingWindow",
	"style" : ("movable", "float",),
	
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	
	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),
			
			"x" : 0,
			"y" : 0,
			
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			
			"title" : uiScriptLocale.CHATTING_SETTING_TITLE,
			
			"children" :
			(
				## base pattern
				{
					"name" : "base_pattern",
					"type" : "window",
					"style" : ("attach", "ltr",),
					
					"x" : 10,
					"y" : 32,
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

				## 채팅 설정
				{
					"name" : "chatting_setting_menu_bg", "type" : "image", "x" : 15, "y" : 37,
					"image" : CHATTING_PATH + "chattingoption_menu_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_TITLE, "all_align":"center"	},
					),
				},
				## 목록 드롭다운
				{
					"name" : "chatting_setting_change_window", "type" : "window", "x" : 145, "y" : 64, "width" : 90, "height" : 18,			
					"children" :
					(
						{
							"name" : "language_select_img", "type" : "image", "x" : 0, "y" : 0,
							"image" : CHATTING_PATH + "chattingoption_dropdown_small_bg.sub",
						},
						{
							"name" : "cur_chatting_type_window", "type" : "window", "x" : 0, "y" : 0, "width" : 90-15, "height" : 18,
							"children" :
							(
								{"name":"cur_chatting_type_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.CHATTING_SETTING_DEFAULT_TITLE, "all_align" : "center"},
							),
						},
						{
							"name" : "chatting_type_select_button", "type" : "button", "x" : 218-145, "y" : 1,
							"default_image" : "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_01.sub",
							"over_image"	: "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_02.sub",
							"down_image"	: "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_03.sub",
						},
						{
							"name" : "chatting_type_select_pivot_window", "type" : "window", "x" : 0, "y" : 18, "width" : 90, "height" : 0,
						},
					),
				},
				## 목록
				{
					"name" : "chatting_setting_list_bg", "type" : "image", "x" : 18, "y" : 65,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_list", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_LIST, "all_align":"center"	},
					),
				},
				## 일반
				{
					"name" : "chatting_setting_talking_bg", "type" : "image", "x" : 18, "y" : 83,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_talking", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_TALKING, "all_align":"center"	},
					),
				},
				## 파티
				{
					"name" : "chatting_setting_party_bg", "type" : "image", "x" : 18, "y" : 101,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_party", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_PARTY, "all_align":"center"	},
					),
				},
				## 길드
				{
					"name" : "chatting_setting_guild_bg", "type" : "image", "x" : 18, "y" : 119,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_guild", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_GUILD, "all_align":"center"	},
					),
				},
				## 외침
				{
					"name" : "chatting_setting_shout_bg", "type" : "image", "x" : 18, "y" : 137,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_shout", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_SHOUT, "all_align":"center"	},
					),
				},
				## 시스템
				{
					"name" : "chatting_setting_info_bg", "type" : "image", "x" : 18, "y" : 155,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_info", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_SYSTEM, "all_align":"center"	},
					),
				},
				## 공지
				{
					"name" : "chatting_setting_notice_bg", "type" : "image", "x" : 18, "y" : 173,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_notice", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_NOTICE, "all_align":"center"	},
					),
				},				
				## 언어설정
				{
					"name" : "languate_setting_menu_bg", "type" : "image", "x" : 15, "y" : 194,
					"image" : "d:/ymir work/ui/chat/chattingoption_menu_bg.sub",
					"children" :
					(
						{ "name" : "language_setting_text", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align":"center"	},
					),
				},
				## 전체
				{
					"name" : "chatting_setting_notice_bg", "type" : "image", "x" : 18, "y" : 222,
					"image" : CHATTING_PATH + "chattingoption_sub_small_1_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_notice", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_LANGUAGE_ONOFF, "all_align":"center"	},
					),
				},
				## OnOff Button
				{
					"name" : "onoff_button", "type" : "toggle_button", "x" : 97, "y" : 220,
					"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
					"over_image"	: "d:/ymir work/ui/public/small_button_02.sub",
					"down_image"	: "d:/ymir work/ui/public/small_button_03.sub",
					"text" : "ON",
				},
				## 언어1
				{
					"name" : "chatting_setting_language1_bg", "type" : "image", "x" : 18, "y" : 242,
					"image" : CHATTING_PATH + "chattingoption_sub_small_2_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_language1", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_LANGUAGE_1, "all_align":"center"	},
					),
				},
				## 언어1 드롭다운
				{
					"name" : "language_1_change_window", "type" : "window", "x" : 98, "y" : 241, "width" : 137, "height" : 18,
					"children" :
					(
						{
							"name" : "language_select_img", "type" : "image", "x" : 0, "y" : 0,
							"image" : CHATTING_PATH + "chattingoption_dropdown_large_bg.sub",
						},
						{
							"name" : "cur_language_text_window", "type" : "window", "x" : 0, "y" : 0, "width" : 137 - 15, "height" : 18,							
							"children" :
							(
								{"name":"cur_language_1_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.LANGUAGE_NONE, "all_align" : "center"},
							),
						},
						{
							"name" : "language_select_button_1", "type" : "button", "x" : 218-98, "y" : 1,
							"default_image" : "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_01.sub",
							"over_image"	: "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_02.sub",
							"down_image"	: "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_03.sub",
						},
						{
							"name" : "language_1_select_pivot_window", "type" : "window", "x" : 0, "y" : 18, "width" : 137, "height" : 0,
						},
					),
				},
				## 언어2
				{
					"name" : "chatting_setting_language2_bg", "type" : "image", "x" : 18, "y" : 260,
					"image" : CHATTING_PATH + "chattingoption_sub_small_2_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_language2", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_LANGUAGE_2, "all_align":"center"	},
					),
				},
				## 언어2 드롭다운
				{
					"name" : "language_2_change_window", "type" : "window", "x" : 98, "y" : 260, "width" : 137, "height" : 18,
					"children" :
					(			
						{
							"name" : "language_select_img", "type" : "image", "x" : 0, "y" : 0,
							"image" : CHATTING_PATH + "chattingoption_dropdown_large_bg.sub",
						},
						{
							"name" : "cur_language_text_window", "type" : "window", "x" : 0, "y" : 0, "width" : 137-15, "height" : 18,							
							"children" :
							(
								{"name":"cur_language_2_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.LANGUAGE_NONE, "all_align" : "center"},
							),
						},
						{
							"name" : "language_select_button_2", "type" : "button", "x" : 218-98, "y" : 1,
							"default_image" : "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_01.sub",
							"over_image"	: "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_02.sub",
							"down_image"	: "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_03.sub",
						},
						{
							"name" : "language_2_select_pivot_window", "type" : "window", "x" : 0, "y" : 18, "width" : 137, "height" : 0,
						},
					),
				},
				## 언어3
				{
					"name" : "chatting_setting_language3_bg", "type" : "image", "x" : 18, "y" : 278,
					"image" : CHATTING_PATH + "chattingoption_sub_small_2_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_language3", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_LANGUAGE_3, "all_align":"center"	},
					),
				},
				## 언어3 드롭다운
				{
					"name" : "language_3_change_window", "type" : "window", "x" : 98, "y" : 278, "width" : 137, "height" : 18,					
					"children" :
					(
						{
							"name" : "language_select_img", "type" : "image", "x" : 0, "y" : 0,
							"image" : CHATTING_PATH + "chattingoption_dropdown_large_bg.sub",
						},
						{
							"name" : "cur_language_text_window", "type" : "window", "x" : 0, "y" : 0, "width" : 137-15, "height" : 18,
							"children" :
							(
								{"name":"cur_language_3_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.LANGUAGE_NONE, "all_align" : "center"},
							),
						},
						{
							"name" : "language_select_button_3", "type" : "button", "x" : 218-98, "y" : 1,
							"default_image" : "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_01.sub",
							"over_image"	: "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_02.sub",
							"down_image"	: "d:/ymir work/ui/chat/chattingoption_dropdown_arrow_03.sub",
						},
						{
							"name" : "language_3_select_pivot_window", "type" : "window", "x" : 0, "y" : 18, "width" : 137, "height" : 0,
						},
					),
				},
				## 주사위 설정
				{
					"name" : "dice_setting_menu_bg", "type" : "image", "x" : 15, "y" : 299,
					"image" : "d:/ymir work/ui/chat/chattingoption_menu_bg.sub",
					"children" :
					(
						{ "name" : "dice_setting", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_DICE_SETTING, "all_align":"center"	},
					),
				},
				## 주사위
				{
					"name" : "chatting_setting_dice_bg", "type" : "image", "x" : 18, "y" : 327,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_dice", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_DICE, "all_align":"center"	},
					),
				},
				## 시스템 세부설정
				{
					"name" : "system_setting_menu_bg", "type" : "image", "x" : 15, "y" : 348,
					"image" : "d:/ymir work/ui/chat/chattingoption_menu_bg.sub",
					"children" :
					(
						{ "name" : "system_setting", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_DETAIL, "all_align":"center"	},
					),
				},
				## 경험치
				{
					"name" : "chatting_setting_exp_bg", "type" : "image", "x" : 18, "y" : 376,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_exp", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_EXP, "all_align":"center"	},
					),
				},
				## 아이템
				{
					"name" : "chatting_setting_item_bg", "type" : "image", "x" : 18, "y" : 394,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_item", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_ITEM, "all_align":"center"	},
					),
				},
				## 재화
				{
					"name" : "chatting_setting_gold_bg", "type" : "image", "x" : 18, "y" : 412,
					"image" : CHATTING_PATH + "chattingoption_sub_large_bg.sub",
					"children" :
					(
						{ "name" : "chatting_setting_gold", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_GOLD, "all_align":"center"	},
					),
				},
				## 탭이름 세부설정
				{
					"name" : "tabname_setting_menu_bg", "type" : "image", "x" : 15, "y" : 433,
					"image" : "d:/ymir work/ui/chat/chattingoption_menu_bg.sub",
					"children" :
					(
						{ "name" : "tabname_setting", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.CHATTING_SETTING_TABNAME, "all_align":"center"	},
					),
				},
				## EditLine
				{
					"name" : "tab_name_slot",
					"type" : "slotbar",
					"x" : 17,
					"y" : 461,
					"width" : 123,
					"height" : 18,
					"children" :
					(
						{
							"name" : "tab_name_value",
							"type" : "editline",
							"x" : 2,
							"y" : 3,
							
							"width" : 160,
							
							"height" : 15,
							"input_limit" : 6,
							"check_width" : 1,
							"text" : "",
						},
					),
				},

				## tabname accpet Button
				{
					"name" : "tabname_accept_button",
					"type" : "button",

					"x" : 143,
					"y" : 460,

					"default_image" : CHATTING_PATH + "chattingoption_enter_btn_01.sub",
					"over_image"	: CHATTING_PATH + "chattingoption_enter_btn_02.sub",
					"down_image"	: CHATTING_PATH + "chattingoption_enter_btn_03.sub",
				},

				## reset_button
				{
					"name" : "reset_button",
					"type" : "button",

					"x" : 10,
					"y" : 489,

					"default_image"	: CHATTING_PATH + "chattingoption_bottom_button_01.sub",
					"over_image"	: CHATTING_PATH + "chattingoption_bottom_button_02.sub",
					"down_image"	: CHATTING_PATH + "chattingoption_bottom_button_03.sub",

					"text" : uiScriptLocale.CHATTING_SETTING_CLEAR,
				},
				## Save Button
				{
					"name" : "save_button",
					"type" : "button",

					"x" : 88,
					"y" : 489,

					"default_image"	: CHATTING_PATH + "chattingoption_bottom_button_01.sub",
					"over_image"	: CHATTING_PATH + "chattingoption_bottom_button_02.sub",
					"down_image"	: CHATTING_PATH + "chattingoption_bottom_button_03.sub",

					"text" : uiScriptLocale.CHATTING_SETTING_SAVE,
				},
				## Cancle Button
				{
					"name" : "cancle_button",
					"type" : "button",

					"x" : 166,
					"y" : 489,

					"default_image"	: CHATTING_PATH + "chattingoption_bottom_button_01.sub",
					"over_image"	: CHATTING_PATH + "chattingoption_bottom_button_02.sub",
					"down_image"	: CHATTING_PATH + "chattingoption_bottom_button_03.sub",

					"text" : uiScriptLocale.CHATTING_SETTING_CANCLE,
				},
			),
		},
	),
}