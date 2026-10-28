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
	ITEM2_SLOT_POS_X		= MAIN_WINDOW_WIDTH - 253
	ITEM1_COUNT_POS_X		= MAIN_WINDOW_WIDTH - 130
	ITEM2_COUNT_POS_X		= MAIN_WINDOW_WIDTH - 311
	PREV_BUTTON_POS_X		= 31
	NEXT_BUTTON_POS_X		= 0

	ITEM_COUNT_TEXT_POS_X	= -1
	ITEM_COUNT_TEXT_POS_Y	= 0
else:
	ITEM1_SLOT_POS_X		= 30
	ITEM2_SLOT_POS_X		= 211
	ITEM1_COUNT_POS_X		= 80
	ITEM2_COUNT_POS_X		= 261
	PREV_BUTTON_POS_X		= 289
	NEXT_BUTTON_POS_X		= 320

	ITEM_COUNT_TEXT_POS_X	= 1
	ITEM_COUNT_TEXT_POS_Y	= 3

ITEM_SLOT_POS_Y				= 10
ITEM_COUNT_POS_Y			= 24
PREV_BUTTON_POS_Y			= 168
NEXT_BUTTON_POS_Y			= 168


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
			
			## 황금의땅
			"title"		: uiScriptLocale.GOLDEN_LAND_EVENT_TITLE,

			"children" :
			[
				## background
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
							"name" : "line_window_left_top",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,

							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},

						{
							"name" : "line_window_right_top",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : 0,

							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},

						{
							"name" : "line_window_left_bottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : MAIN_WINDOW_HEIGHT - 16,

							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},

						{
							"name" : "line_window_right_bottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : MAIN_WINDOW_HEIGHT - 16,

							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},

						{
							"name" : "line_window_top_center",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,

							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
						},

						{
							"name" : "line_window_left_center",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,

							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, MAIN_WINDOW_PATTERN_Y_COUNT),
						},

						{
							"name" : "line_window_right_center",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : MAIN_WINDOW_WIDTH - 16,
							"y" : 16,

							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, MAIN_WINDOW_PATTERN_Y_COUNT),
						},

						{
							"name" : "line_window_bottom_center",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : MAIN_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
						},

						{
							"name" : "line_window_center",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, MAIN_WINDOW_PATTERN_Y_COUNT),
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

						## 세로 경계라인
						{
							"name" : "line_window_vertical_boundary_top_line",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 174,
							"y" : 0,

							"image" : ROOT_PATH + "vertical_top_line.sub",
						},

						{
							"name" : "line_window_vertical_boundary_bottom_line",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 174,
							"y" : 53,

							"image" : ROOT_PATH + "vertical_bottom_line.sub",
						},

						{
							"name" : "line_window_horizontal_boundary_right_line",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 174,
							"y" : 2,

							"image" : ROOT_PATH + "vertical_middle_line.sub",
						},

					],
				},

				## slot
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
						## item1 slot
						{
							"name"		: "tree_fruit_slot_bg",
							"type"		: "image",
							
							"x"			: ITEM1_SLOT_POS_X,
							"y"			: ITEM_SLOT_POS_Y,
							
							"width"		: SLOT_IMAGE_WIDTH,
							"height"	: SLOT_IMAGE_HEIGHT,
							
							"image"		: ROOT_PATH + "slot.sub",

							"children" :
							[
								{
									"name"		: "tree_fruit_slot",
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

						## item1 count
						{
							"name"	: "tree_fruit_count_bg",
							"type"	: "image",

							"x"		: ITEM1_COUNT_POS_X,
							"y"		: ITEM_COUNT_POS_Y,
							
							"image"	: ROOT_PATH + "count_bg2.sub",

							"children" :
							[
								{
									"name"					: "tree_fruit_count_text",
									"type"					: "text",

									"x"						: ITEM_COUNT_TEXT_POS_X,
									"y"						: ITEM_COUNT_TEXT_POS_Y,

									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",

									"text"					: "",
								},
							],
						},

						## item2 slot
						{
							"name"		: "yellow_fruit_slot_bg",
							"type"		: "image",
							
							"x"			: ITEM2_SLOT_POS_X,
							"y"			: ITEM_SLOT_POS_Y,
							
							"width"		: SLOT_IMAGE_WIDTH,
							"height"	: SLOT_IMAGE_HEIGHT,
							
							"image"		: ROOT_PATH + "slot.sub",

							"children" :
							[
								{
									"name"		: "yellow_fruit_slot",
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

						## item2 count
						{
							"name"	: "yellow_fruit_count_bg",
							"type"	: "image",

							"x"		: ITEM2_COUNT_POS_X,
							"y"		: ITEM_COUNT_POS_Y,
							
							"image"	: ROOT_PATH + "count_bg2.sub",

							"children" :
							[
								{
									"name"					: "yellow_fruit_count_text",
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

				## desc bard
				{
					"name"		: "desc_board",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 10,
					"y"			: 106,

					"width"		: DESC_WINDOW_WIDTH,
					"height"	: DESC_WINDOW_HEIGHT,

					"children" :
					[
						{
							"name"			: "prev_button",
							"type"			: "button",

							"x"				: PREV_BUTTON_POS_X,
							"y"				: PREV_BUTTON_POS_Y,

							"default_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
							"over_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_02.sub",
							"down_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
						},
						
						{
							"name"			: "next_button",
							"type"			: "button",

							"x"				: NEXT_BUTTON_POS_X,
							"y"				: NEXT_BUTTON_POS_Y,

							"default_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
							"over_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_02.sub",
							"down_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
						},

					],
				},
			],
		},
	],
}