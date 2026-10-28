import uiScriptLocale
import app

ROOT_PATH				= "d:/ymir work/ui/gameoption/"
BASE_WITDH				= 316
BASE_HEIGHT				= 486

# 툴팁 버튼
TOOLTIP_BUTTON_X		= BASE_WITDH - 50
TOOLTIP_BUTTON_Y		= 8

window = {
	"name"		: "GameOptionWindow",
	"style"		: ("movable", "float",),
	
	"x"			: SCREEN_WIDTH / 2 - BASE_WITDH / 2,
	"y"			: SCREEN_HEIGHT / 2 - BASE_HEIGHT / 2,

	"width"		: BASE_WITDH,
	"height"	: BASE_HEIGHT,

	"children"	:
	(
		{
			"name"		: "Board",
			"type"		: "board_with_titlebar",

			"x"			: 0,
			"y"			: 0,

			"width"		: BASE_WITDH,
			"height"	: BASE_HEIGHT,

			"title"		: uiScriptLocale.GAME_OPTION_SYSTEM, ## 시스템

			"children" :
			(
				## __tooltip_button
				{
					"name" : "tooltip_button",
					"type" : "button",
					
					"x" : TOOLTIP_BUTTON_X,
					"y" : TOOLTIP_BUTTON_Y,
					
					"default_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
					"over_image"	: "d:/ymir work/ui/pattern/q_mark_02.tga",
					"down_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
				},
				
				## Init Button
				{
					"name"	: "init_button",
					"type"	: "button",

					"x"		: 11,
					"y"		: 427,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image"	: "d:/ymir work/ui/public/large_button_02.sub",
					"down_image"	: "d:/ymir work/ui/public/large_button_03.sub",
					
					"text" : uiScriptLocale.GAME_OPTION_INIT, ## 초기화
				},

				## keyChange_save_option_checkBox
				{
					"name" : "keychange_save_option_checkbox",
					"type" : "image", 
					
					"x"		: 105,
					"y"		: 430,

					"image" : "d:/ymir work/ui/game/looting/uncheck_box.sub",
				},

				## Tab Area
				{
					"name"		: "tab_control",
					"type"		: "window",

					"x"			: 0,
					"y"			: 446,

					"width"		: BASE_WITDH,
					"height"	: 30,

					"children" :
					(
						## Tab
						{
							"name"		: "tab_img_01",
							"type"		: "image",

							"x"			: 0,
							"y"			: 0,

							"width"		: 0,
							"height"	: 0,

							"image"		: ROOT_PATH + "tab_01.sub",
						},
						{
							"name"		: "tab_img_02",
							"type"		: "image",

							"x"			: 0,
							"y"			: 0,

							"width"		: 0,
							"height"	: 0,

							"image"		: ROOT_PATH + "tab_02.sub",
						},
						{
							"name"		: "tab_img_03",
							"type"		: "image",

							"x"			: 0,
							"y"			: 0,

							"width"		: 0,
							"height"	: 0,

							"image"		: ROOT_PATH + "tab_03.sub",
						},
						{
							"name"		: "tab_img_04",
							"type"		: "image",

							"x"			: 0,
							"y"			: 0,

							"width"		: 0,
							"height"	: 0,

							"image"		: ROOT_PATH + "tab_04.sub",
						},


						## RadioButton
						{
							"name"		: "tab_button_01",
							"type"		: "radio_button",

							"x"			: 14,
							"y"			: 10,

							"width"		: 58,
							"height"	: 30,
						},
						{
							"name"		: "tab_button_02",
							"type"		: "radio_button",

							"x"			: 91,
							"y"			: 10,

							"width"		: 58,
							"height"	: 30,
						},
						{
							"name"		: "tab_button_03",
							"type"		: "radio_button",

							"x"			: 168,
							"y"			: 10,

							"width"		: 57,
							"height"	: 30,
						},
						{
							"name"		: "tab_button_04",
							"type"		: "radio_button",

							"x"			: 244,
							"y"			: 10,

							"width"		: 57,
							"height"	: 30,
						},
					),
				},
			),
		},
	),
}
