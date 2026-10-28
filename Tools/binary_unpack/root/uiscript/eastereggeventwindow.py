#if app.ENABLE_EASTER_EGG_RENEWAL:

import uiScriptLocale
import localeInfo

ROOT_PATH							= "d:/ymir work/ui/game/easter_egg_event/"
PUBLIC_PATH							= "d:/ymir work/ui/public/"

WINDOW_WIDTH						= 250
WINDOW_HEIGHT						= 355

window = {
	"name"		: "easter_egg_event_window",
	"style"		: ("movable", "float"),

	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	(
		### 메인
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			"style"		: ("attach",),

			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"title"		: uiScriptLocale.EASTER_EGG_TITLE,		# 부활절

			"children"	:
			(
				{
					"name"		: "main_bg",
					"type"		: "image",

					"x"			: 10,
					"y"			: 32,

					"image"		: ROOT_PATH + "main_bg.sub",

					"children"	:
					(
						## 총 보유 개수 text
						{
							"name"					: "egg_total_count_guide_text",
							"type"					: "text",

							"x"						: -35,
							"y"						: -135,

							"vertical_align"		: "center",
							"horizontal_align"		: "center",
							"text_vertical_align"	: "center",
							"text_horizontal_align"	: "center",

							"text"					: uiScriptLocale.EASTER_EGG_TOTAL_COUNT,			# 총 보유 개수
						},

						## 보유 개수 count 값의 bg
						{
							"name"					: "egg_total_count_bg",
							"type"					: "image",

							"x"						: 55,
							"y"						: -135,

							"vertical_align"		: "center",
							"horizontal_align"		: "center",

							"image"					: ROOT_PATH + "egg_total_count_bg.sub",
						},

						## 총 보유 개수 count 값의 text
						{
							"name"					: "egg_total_count_text",
							"type"					: "text",

							"x"						: 55,
							"y"						: -135,

							"vertical_align"		: "center",
							"horizontal_align"		: "center",
							"text_vertical_align"	: "center",
							"text_horizontal_align"	: "center",

							"text"					: "0"
						},

						## 나! 달걀을 찾았어 버튼
						{
							"name"					: "recv_button", 
							"type"					: "button", 

							"x"						: 0, 
							"y"						: 280, 

							"width"					: 300,
							"height"				: 100,

							"horizontal_align"		: "center",

							"default_image"			: ROOT_PATH + "recv_button_default.sub",
							"over_image"			: ROOT_PATH + "recv_button_over.sub",
							"down_image"			: ROOT_PATH + "recv_button_down.sub", 

							"text"					: uiScriptLocale.EASTER_EGG_RECEIVE_REWARD_BUTTON,	# 나! 달걀을 찾았어.
						}, 
					),
				},

			),
		},
	),
}