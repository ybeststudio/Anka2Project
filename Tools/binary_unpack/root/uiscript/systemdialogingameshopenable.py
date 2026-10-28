# ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH:	# UIScript 경로 변경
import uiScriptLocale
import app

ROOT = "d:/ymir work/ui/public/"
VERSION_ADD_Y = 10

if app.ENABLE_GAME_OPTION_RENEWAL:
	BASE_WIDTH	= 200
	BASE_HEIGHT	= 318

	window = {
		"name" : "SystemDialog",
		"style" : ("float",),
	
		"x" : (SCREEN_WIDTH  - BASE_WIDTH) /2,
		"y" : (SCREEN_HEIGHT - BASE_HEIGHT) /2,
	
		"width" : BASE_WIDTH,
		"height" : BASE_HEIGHT + VERSION_ADD_Y,
	
		"children" :
		[
			{
				"name" : "board",
				"type" : "thinboard",
	
				"x" : 0,
				"y" : 0,
	
				"width" : BASE_WIDTH,
				"height" : BASE_HEIGHT + VERSION_ADD_Y,
	
				"children" :
				[
				],
			},
		],
	}
	
	LINE_START	= 20
	LINE_STEP	= 30
	LINE_NUMBER	= 0

	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "help_button",
						"type" : "button",
	
						"x" : 0,
						"y" : LINE_START + LINE_STEP * LINE_NUMBER,
						"horizontal_align" : "center",

						# 도움말
						"text" : uiScriptLocale.SYSTEM_HELP,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
		]
	LINE_NUMBER	+= 1

	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "version",
						"type" : "text",
	
						"x" : 0,
						"y" : LINE_START + LINE_STEP * LINE_NUMBER + 5,

						"horizontal_align" : "center",
						"text_horizontal_align" : "center",
	
						## Ver.
						"text" : app.VERSION,
					},
		]
	LINE_NUMBER	+= 1

	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "mall_button",
						"type" : "button",
	
						"x" : 0,
						"y" : LINE_START + LINE_STEP * LINE_NUMBER,
						"horizontal_align" : "center",

						# 아이템 몰
						"text" : uiScriptLocale.SYSTEM_MALL,
						"text_color" : 0xffF8BF24,
	
						"default_image" : ROOT + "XLarge_Button_02.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_02.sub",
					},
		]
	LINE_NUMBER	+= 1

	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "game_option_button",
						"type" : "button",
	
						"x" : 0,
						"y" : LINE_START + LINE_STEP * LINE_NUMBER,
						"horizontal_align" : "center",

						# 옵션
						"text" : uiScriptLocale.GAMEOPTION_TITLE,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
		]
	LINE_NUMBER	+= 1

	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "escape_button",
						"type" : "button",
		
						"x" : 0,
						"y" : LINE_START + LINE_STEP * LINE_NUMBER,
						"horizontal_align" : "center",

						# 조작 불가 탈출하기
						"text" : uiScriptLocale.SYSTEM_CANNOT_CONTROL_AND_ESCAPTE,
		
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
		]
	LINE_NUMBER	+= 1

	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "change_button",
						"type" : "button",
		
						"x" : 0,
						"y" : LINE_START + LINE_STEP * LINE_NUMBER,
						"horizontal_align" : "center",

						# 캐릭터 전환하기
						"text" : uiScriptLocale.SYSTEM_CHANGE,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
		]
	LINE_NUMBER	+= 1

	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "logout_button",
						"type" : "button",
		
						"x" : 0,
						"y" : LINE_START + LINE_STEP * LINE_NUMBER,
						"horizontal_align" : "center",

						# 로그 아웃
						"text" : uiScriptLocale.SYSTEM_LOGOUT,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
		]
	LINE_NUMBER	+= 2

	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "exit_button",
						"type" : "button",
		
						"x" : 0,
						"y" : LINE_START + LINE_STEP * LINE_NUMBER,
						"horizontal_align" : "center",

						# 윈도우로 나가기
						"text" : uiScriptLocale.SYSTEM_EXIT,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
		]
	LINE_NUMBER	+= 1

	window["children"][0]["children"] = window["children"][0]["children"] + [
					{
						"name" : "cancel_button",
						"type" : "button",
		
						"x" : 0,
						"y" : LINE_START + LINE_STEP * LINE_NUMBER,
						"horizontal_align" : "center",
						
						# 취소
						"text" : uiScriptLocale.CANCEL,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
		]
	LINE_NUMBER	+= 1
else:
	window = {
		"name" : "SystemDialog",
		"style" : ("float",),
	
		"x" : (SCREEN_WIDTH  - 200) /2,
		"y" : (SCREEN_HEIGHT - 318) /2,
	
		"width" : 200,
		"height" : 318 + VERSION_ADD_Y,
	
		"children" :
		(
			{
				"name" : "board",
				"type" : "thinboard",
	
				"x" : 0,
				"y" : 0,
	
				"width" : 200,
				"height" : 318 + VERSION_ADD_Y,
	
				"children" :
				(
					{
						"name" : "help_button",
						"type" : "button",
	
						"x" : 10,
						"y" : 17,
						# 도움말
						"text" : uiScriptLocale.SYSTEM_HELP,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},	
					{
						"name" : "version",
						"type" : "text",
	
						"x" : 0,
						"y" : 47,
	
						"text" : app.VERSION,
						"horizontal_align" : "center",
						"text_horizontal_align" : "center",
					},
					{
						"name" : "mall_button",
						"type" : "button",
	
						"x" : 10,
						"y" : 57 + VERSION_ADD_Y,
						# 아이템 몰
						"text" : uiScriptLocale.SYSTEM_MALL,
						"text_color" : 0xffF8BF24,
	
						"default_image" : ROOT + "XLarge_Button_02.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_02.sub",
					},
	
					{
						"name" : "system_option_button",
						"type" : "button",
	
						"x" : 10,
						"y" : 87 + VERSION_ADD_Y,
						# 시스템 옵션
						"text" : uiScriptLocale.SYSTEMOPTION_TITLE,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
					{
						"name" : "game_option_button",
						"type" : "button",
	
						"x" : 10,
						"y" : 117 + VERSION_ADD_Y,
						# 게임옵션
						"text" : uiScriptLocale.GAMEOPTION_TITLE,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
					{
						"name" : "escape_button",
						"type" : "button",
		
						"x" : 10,
						"y" : 147 + VERSION_ADD_Y,
						# 조작 불가 탈출하기
						"text" : uiScriptLocale.SYSTEM_CANNOT_CONTROL_AND_ESCAPTE,
		
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
					{
						"name" : "change_button",
						"type" : "button",
	
						"x" : 10,
						"y" : 177 + VERSION_ADD_Y,
						# 캐릭터 전환하기
						"text" : uiScriptLocale.SYSTEM_CHANGE,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
					{
						"name" : "logout_button",
						"type" : "button",
	
						"x" : 10,
						"y" : 207 + VERSION_ADD_Y,
						# 로그 아웃
						"text" : uiScriptLocale.SYSTEM_LOGOUT,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
					#{
					#	"name" : "movechannel_button",
					#	"type" : "button",
	
					#	"x" : 10,
					#	"y" : 177 + VERSION_ADD_Y,
	
					#	"text" : uiScriptLocale.SYSTEM_MOVE_CHANNEL,
	
					#	"default_image" : ROOT + "XLarge_Button_01.sub",
					#	"over_image" : ROOT + "XLarge_Button_02.sub",
					#	"down_image" : ROOT + "XLarge_Button_03.sub",
					#},
					{
						"name" : "exit_button",
						"type" : "button",
	
						"x" : 10,
						"y" : 247 + VERSION_ADD_Y,
						# 윈도우로 나가기
						"text" : uiScriptLocale.SYSTEM_EXIT,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
					{
						"name" : "cancel_button",
						"type" : "button",
	
						"x" : 10,
						"y" : 277 + VERSION_ADD_Y,
						# 취소
						"text" : uiScriptLocale.CANCEL,
	
						"default_image" : ROOT + "XLarge_Button_01.sub",
						"over_image" : ROOT + "XLarge_Button_02.sub",
						"down_image" : ROOT + "XLarge_Button_03.sub",
					},
				),
			},
		),
	}