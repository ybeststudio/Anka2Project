import uiScriptLocale
import localeInfo
import app

PUBLIC_PATH					= "d:/ymir work/ui/public/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"
ROOT_PATH					= "d:/ymir work/ui/event/"

WINDOW_WIDTH				= 372
WINDOW_HEIGHT				= 306

MAIN_WINDOW_WIDTH			= 350
MAIN_WINDOW_HEIGHT			= 262

MAIN_WINDOW_PATTERN_X_COUNT	= (MAIN_WINDOW_WIDTH - 32) / 16
MAIN_WINDOW_PATTERN_Y_COUNT	= (MAIN_WINDOW_HEIGHT - 32) / 16

SLOT_IMAGE_WIDTH			= 48
SLOT_IMAGE_HEIGHT			= 48
SLOT_WIDTH					= 32
SLOT_HEIGHT					= 32

DESC_WINDOW_WIDTH			= 345
DESC_WINDOW_HEIGHT			= 190

if localeInfo.IsARABIC():
	ITEM1_SLOT_POS_X		= MAIN_WINDOW_WIDTH - 72
	ITEM1_COUNT_POS_X		= MAIN_WINDOW_WIDTH - 130
	ITEM_COUNT_TEXT_POS_X	= -1
	ITEM_COUNT_TEXT_POS_Y	= 0
else:
	ITEM1_SLOT_POS_X		= 30
	ITEM1_COUNT_POS_X		= 80
	ITEM_COUNT_TEXT_POS_X	= 1
	ITEM_COUNT_TEXT_POS_Y	= 3

ITEM_SLOT_POS_Y				= 10
ITEM_COUNT_POS_Y			= 24

window = {
	"name"		: "EventQuestFlagWindow",
	"style"		: ("movable", "float",),

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
			
			# 월드컵 이벤트
			"title"		: uiScriptLocale.FOOTBALL_EVENT_TITLE,

			"children" :
			[
				# background
				{
					"name"		: "line_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 10,
					"y"			: 32,

					"width"		: MAIN_WINDOW_WIDTH,
					"height"	: MAIN_WINDOW_HEIGHT,

					"children" :
					[
						{
							"name"		: "main_outline",
							"type"		: "outline_window",
							"x"			: 0,
							"y"			: 0,
							"width"		: MAIN_WINDOW_WIDTH,
							"height"	: MAIN_WINDOW_HEIGHT,
						},

						## 가로 경계 라인
						{
							"name" : "line_window_horizontal_boundary_left_line",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 66,

							"image" : ROOT_PATH + "horizontal_line_left.sub",
						},

						{
							"name" : "line_window_horizontal_boundary_right_line",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 342,
							"y" : 66,

							"image" : ROOT_PATH + "horizontal_line_right.sub",
						},

						{
							"name" : "line_window_middle_boundary_center_line",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 8,
							"y" : 66,

							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT+1, 0),
						},
					],
				},

				# slot
				{
					"name"		: "slot_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 10,
					"y"			: 32,

					"width"		: MAIN_WINDOW_WIDTH,
					"height"	: MAIN_WINDOW_HEIGHT,

					"children" :
					[
						# football slot
						{
							"name"		: "football_slot_bg",
							"type"		: "image",
							
							"x"			: ITEM1_SLOT_POS_X,
							"y"			: ITEM_SLOT_POS_Y,
							
							"width"		: SLOT_IMAGE_WIDTH,
							"height"	: SLOT_IMAGE_HEIGHT,
							
							"image"		: ROOT_PATH + "slot.sub",

							"children" :
							[
								{
									"name"		: "football_slot",
									"type"		: "slot",
									
									"x"			: 8,
									"y"			: 8,
									
									"width"		: SLOT_IMAGE_WIDTH,
									"height"	: SLOT_IMAGE_HEIGHT,

									"slot" : 
									(
										{"index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
									),
								},
							],
						},

						# football count
						{
							"name"	: "football_count_bg",
							"type"	: "image",

							"x"		: ITEM1_COUNT_POS_X,
							"y"		: ITEM_COUNT_POS_Y,
							
							"image"	: ROOT_PATH + "count_bg2.sub",

							"children" :
							[
								{
									"name"					: "football_count_text",
									"type"					: "text",

									"x"						: ITEM_COUNT_TEXT_POS_X,
									"y"						: ITEM_COUNT_TEXT_POS_Y,

									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",

									"text"					: "",
								},
							],
						},
					],
				},

				# desc
				{
					"name"		: "desc_window",
					"type"		: "window",
					"x"			: 10,
					"y"			: 106,
					"width"		: DESC_WINDOW_WIDTH,
					"height"	: DESC_WINDOW_HEIGHT,
					"style"		: ("not_pick", ),
				},

				# use button
				{
					"name"			: "football_use_button",
					"type"			: "button",
					"x"				: MAIN_WINDOW_WIDTH - 122,
					"y"				: 56,

					# 사용하기
					"text"			: uiScriptLocale.FOOTBALL_USE_BUTTON,
					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
				},
			],
		},
	],
}