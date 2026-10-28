import uiScriptLocale
import localeInfo
import app

PUBLIC_PATH					= "d:/ymir work/ui/public/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"
ROOT_PATH					= "d:/ymir work/ui/event/"

WINDOW_WIDTH				= 436
WINDOW_HEIGHT				= 303

EVENT_STATUS_WIDTH			= 48
EVENT_STATUS_HEIGHT			= 48

SLOT_WIDTH					= 32
SLOT_HEIGHT					= 32

MAIN_CENTER_LINE_PATTERN_Y_COUNT = (262 - 32) / 16

if localeInfo.IsARABIC():
	MAIN_BOUNDARY_CENTER_LINE_X = 191
else:
	MAIN_BOUNDARY_CENTER_LINE_X = 178

window = {
    "name"		: "valentine_day_event_window",
	"style"		: ("movable", "float", ),

	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	[
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			
			"x"			: 0,
			"y"			: 0,
			
			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,
			
			# 발렌타인데이 이벤트 현황
			"title"		: uiScriptLocale.VALENTINE_DAY_EVENT_TITLE,

			"children" :
			[
				# main bg
				{
					"name"		: "main_bg_window",
					"type"		: "window",
					"x"			: 10,
					"y"			: 32,
					"width"		: WINDOW_WIDTH,
					"height"	: WINDOW_HEIGHT,
					"style"		: ("not_pick", ),

					"children"	:
					[
						{
							"name"		: "main_outline",
							"type"		: "outline_window",
							"x"			: 0,
							"y"			: 0,
							"width"		: 415,
							"height"	: 262,
						},

						{
							"name" : "main_boundary_top_line",
							"type" : "image",
							"style" : ("ltr",),
							"x" : 191,
							"y" : 0,
							"image" : ROOT_PATH + "vertical_top_line.sub",
						},

						{
							"name" : "main_boundary_bottom_line",
							"type" : "image",
							"style" : ("ltr",),
							"x" : 191,
							"y" : 246,
							"image" : ROOT_PATH + "vertical_bottom_line.sub",
						},

						{
							"name" : "main_center_line",
							"type" : "expanded_image",
							"style" : ("ltr",),
							"x" : MAIN_BOUNDARY_CENTER_LINE_X,
							"y" : 10,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, MAIN_CENTER_LINE_PATTERN_Y_COUNT),
						},
					]
				},

				# 발렌타인데이 설명 페이지
				{
					"name"		: "desc_window",
					"type"		: "window",
					"x"			: 206,
					"y"			: 35,
					"width"		: 215,
					"height"	: 261,
					"style"		: ("not_pick", ),
				},

				# 발렌타인데이 이벤트 퀘스트 플래그 현황
				{
					"name"		: "status_window",
					"type"		: "window",
					"x"			: 10,
					"y"			: 32,
					"width"		: WINDOW_WIDTH,
					"height"	: WINDOW_HEIGHT,
					"style"		: ("not_pick", ),

					"children"	:
					[
						# 장미
						{
							"name"		: "rose_slot_bg",
							"type"		: "image",
							"x"			: 68,
							"y"			: 10,
							"image"		: ROOT_PATH + "slot.sub",

							"children"	:
							[
								{
									"name"		: "rose_slot",
									"type"		: "slot",
									"x"			: 8,
									"y"			: 8,
									"width"		: SLOT_WIDTH,
									"height"	: SLOT_HEIGHT,

									"slot" : ( { "index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT, }, ),
								},
							],
						},

						{
							"name"		: "rose_cur_count_menu_bg",
							"type"		: "image",
							"x"			: 10,
							"y"			: 63,
							"image"		: ROOT_PATH + "count_menu_bg.sub",

							"children"	:
							[
								{
									"name"					: "rose_cur_count_menu_text",
									"type"					: "text",
									"x"						: 0,
									"y"						: 1,
									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",

									# 현재 개수
									"text"					: uiScriptLocale.VALENTINE_DAY_EVENT_CURRENT_COUNT,
								},
							],
						},

						{
							"name"		: "rose_cur_count_bg",
							"type"		: "image",
							"x"			: 151,
							"y"			: 62,
							"image"		: ROOT_PATH + "count_bg.sub",

							"children"	:
							[
								{
									"name"					: "rose_cur_count_text",
									"type"					: "text",
									"x"						: 0,
									"y"						: 2,
									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",
									"text"					: "",
								},
							],
						},

						{
							"name"		: "rose_max_count_bg",
							"type"		: "image",
							"x"			: 10,
							"y"			: 81,
							"image"		: ROOT_PATH + "count_menu_bg.sub",

							"children"	:
							[
								{
									"name"					: "rose_max_count_bg_text",
									"type"					: "text",
									"x"						: 0,
									"y"						: 1,
									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",
									# 보유가능 개수
									"text"					: uiScriptLocale.VALENTINE_DAY_EVENT_MAX_COUNT,
								},
							],
						},

						{
							"name"		: "rose_max_count_bg",
							"type"		: "image",
							"x"			: 151,
							"y"			: 80,
							"image"		: ROOT_PATH + "count_bg.sub",

							"children"	:
							[
								{
									"name"					: "rose_max_count_text",
									"type"					: "text",

									"x"						: 0,
									"y"						: 2,

									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",
									"text"					: "",
								},
							],
						},

						{
							"name"			: "rose_use_button",
							"type"			: "button",
							"x"				: 9,
							"y"				: 105,

							# 사용하기
							"text"			: uiScriptLocale.VALENTINE_DAY_EVENT_USE_BUTTON,
							"default_image" : PUBLIC_PATH + "large_button_01.sub",
							"over_image"	: PUBLIC_PATH + "large_button_02.sub",
							"down_image"	: PUBLIC_PATH + "large_button_03.sub",
						},

						{
							"name"			: "rose_wrap_button",
							"type"			: "button",
							"x"				: 97,
							"y"				: 105,

							# 포장하기
							"text"			: uiScriptLocale.VALENTINE_DAY_EVENT_WRAP_BUTTON,
							"default_image" : PUBLIC_PATH + "large_button_01.sub",
							"over_image"	: PUBLIC_PATH + "large_button_02.sub",
							"down_image"	: PUBLIC_PATH + "large_button_03.sub",
						},

						# 초콜릿
						{
							"name"		: "chocolate_slot_bg",
							"type"		: "image",
							"x"			: 68,
							"y"			: 135,
							"image"		: ROOT_PATH + "slot.sub",

							"children"	:
							[
								{
									"name"		: "chocolate_slot",
									"type"		: "slot",
									"x"			: 8,
									"y"			: 8,
									"width"		: SLOT_WIDTH,
									"height"	: SLOT_HEIGHT,

									"slot" : ( { "index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT, }, ),
								},
							],
						},

						{
							"name"		: "chocolate_cur_count_menu_bg",
							"type"		: "image",
							"x"			: 10,
							"y"			: 188,
							"image"		: ROOT_PATH + "count_menu_bg.sub",

							"children"	:
							[
								{
									"name"					: "chocolate_cur_count_menu_text",
									"type"					: "text",
									"x"						: 0,
									"y"						: 1,
									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",

									# 현재 개수
									"text"					: uiScriptLocale.VALENTINE_DAY_EVENT_CURRENT_COUNT,
								},
							],
						},

						{
							"name"		: "chocolate_cur_count_bg",
							"type"		: "image",
							"x"			: 151,
							"y"			: 187,
							"image"		: ROOT_PATH + "count_bg.sub",

							"children"	:
							[
								{
									"name"					: "chocolate_cur_count_text",
									"type"					: "text",
									"x"						: 0,
									"y"						: 2,
									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",
									"text"					: "",
								},
							],
						},

						{
							"name"		: "chocolate_max_count_bg",
							"type"		: "image",
							"x"			: 10,
							"y"			: 206,
							"image"		: ROOT_PATH + "count_menu_bg.sub",

							"children"	:
							[
								{
									"name"					: "chocolate_max_count_bg_text",
									"type"					: "text",
									"x"						: 0,
									"y"						: 1,
									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",

									# 보유가능 개수
									"text"					: uiScriptLocale.VALENTINE_DAY_EVENT_MAX_COUNT,
								},
							],
						},

						{
							"name"		: "chocolate_max_count_bg",
							"type"		: "image",
							"x"			: 151,
							"y"			: 205,
							"image"		: ROOT_PATH + "count_bg.sub",

							"children"	:
							[
								{
									"name"					: "chocolate_max_count_text",
									"type"					: "text",
									"x"						: 0,
									"y"						: 2,
									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",
									"text"					: "",
								},
							],
						},

						{
							"name"			: "chocolate_use_button",
							"type"			: "button",
							"x"				: 9,
							"y"				: 230,

							# 사용하기
							"text"			: uiScriptLocale.VALENTINE_DAY_EVENT_USE_BUTTON,
							"default_image" : PUBLIC_PATH + "large_button_01.sub",
							"over_image"	: PUBLIC_PATH + "large_button_02.sub",
							"down_image"	: PUBLIC_PATH + "large_button_03.sub",
						},

						{
							"name"			: "chocolate_wrap_button",
							"type"			: "button",
							"x"				: 97,
							"y"				: 230,

							# 포장하기
							"text"			: uiScriptLocale.VALENTINE_DAY_EVENT_WRAP_BUTTON,
							"default_image" : PUBLIC_PATH + "large_button_01.sub",
							"over_image"	: PUBLIC_PATH + "large_button_02.sub",
							"down_image"	: PUBLIC_PATH + "large_button_03.sub",
						},
					]
				},

			]
		}
	]
}