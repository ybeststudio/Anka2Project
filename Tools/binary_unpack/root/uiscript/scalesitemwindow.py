#if app.ENABLE_SCALES_ITEM:

import uiScriptLocale
import localeInfo

ROOT_PATH							= "d:/ymir work/ui/game/scales_item/"

WINDOW_WIDTH						= 506
WINDOW_HEIGHT						= 536

TOOLTIP_WINDOW_WIDTH				= 240
TOOLTIP_WINDOW_HEIGHT				= 478

EQUIPMENT_AND_SELECTED_INTERVAL_X	= 238

window = {
	"name"		: "scales_item_window",
	"style"		: ("movable", "float"),

	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	(
		### 1. 메인 bg
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			"style"		: ("attach",),

			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"title"		: uiScriptLocale.SCALES_ITEM_TITLE,		# 천평칭

			"children"	:
			(
				### 2-1. 좌측 상태 표시 bg 
				{
					"name"		: "state_equipment_bg",
					"type"		: "image",

					"x"			: 12,
					"y"			: 31,

					"image"		: ROOT_PATH + "titlebar_scales.sub",

					### 3-1. 좌측 상태 표시 (장착중) text
					"children":
					(
						{
							"name"	: "state_equipment_text",
							"type"	: "text",

							"x"		: 0,
							"y"		: 0,

							"horizontal_align"		: "center",
							"text_horizontal_align" : "center",

							"vertical_align"		: "center",
							"text_vertical_align"	: "center",

							"text" : uiScriptLocale.SCALES_ITEM_EQUIPMENT,		# 장착중
						},
					)
				},

				### 4-1. 좌측 내부 테두리바 bg
				{
					"name"		: "inner_background_window_equipment",
					"type"		: "window",

					"x"			: 12,
					"y"			: 54,

					"width"		: TOOLTIP_WINDOW_WIDTH,
					"height"	: TOOLTIP_WINDOW_HEIGHT,
				},

				### 2-2. 우측 상태 표시 bg 
				{
					"name"		: "state_selected_bg",
					"type"		: "image",

					"x"			: 15 + EQUIPMENT_AND_SELECTED_INTERVAL_X,
					"y"			: 31,

					"image"		: ROOT_PATH + "titlebar_scales.sub",

					### 3-2. 우측 상태 표시 (선택) text
					"children":
					(
						{
							"name"	: "state_selected_text",
							"type"	: "text",

							"x"		: 0,
							"y"		: 0,

							"horizontal_align"		: "center",
							"text_horizontal_align" : "center",

							"vertical_align"		: "center",
							"text_vertical_align"	: "center",

							"text" : uiScriptLocale.SCALES_ITEM_SELECT,			# 선택
						},
					)
				},

				### 4-2. 우측 내부 테두리바 bg
				{
					"name"		: "inner_background_window_selected",
					"type"		: "window",

					"x"			: 16 + EQUIPMENT_AND_SELECTED_INTERVAL_X,
					"y"			: 54,

					"width"		: TOOLTIP_WINDOW_WIDTH,
					"height"	: TOOLTIP_WINDOW_HEIGHT,
				},
			),
		},
	),
}