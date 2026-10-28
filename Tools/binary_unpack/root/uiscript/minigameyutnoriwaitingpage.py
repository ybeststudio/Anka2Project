import uiScriptLocale
import app

PUBLIC_PATH = "d:/ymir work/ui/public/"

WINDOW_WIDTH	= 352
WINDOW_HEIGHT	= 384

BOARD_WIDTH		= 322
BOARD_HEIGHT	= 281

ROOT_PATH		= "d:/ymir work/ui/event/"

window = {
	"name"		: "MiniGameYutnoriWaitingPage",
	"style"		: ("movable", "float"),
	
	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,
	
	"children" :
	(
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			
			"x"			: 0,
			"y"			: 0,
			
			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,
			
			"title"		: uiScriptLocale.MINI_GAME_YUTNORI_TITLE,
			
			"children" :
			(		
				{
					"name"		: "desc_board",
					"type"		: "bar",
					
					"x"			: 15,
					"y"			: 40,

					"width"		: BOARD_WIDTH,
					"height"	: BOARD_HEIGHT,
					
					"children" :
					(
						{
							"name"		: "right_line",
							"type"		: "line",

							"x"			: BOARD_WIDTH-1,
							"y"			: 0,

							"width"		: 0,
							"height"	: BOARD_HEIGHT,

							"color"		: 0xffAAA6A1,
						},
						
						{
							"name"		: "bottom_line",
							"type"		: "line",

							"x"			: 0,
							"y"			: BOARD_HEIGHT-1,

							"width"		: BOARD_WIDTH,
							"height"	: 0,

							"color"		: 0xffAAA6A1,
						},
						{
							"name"		: "left_line",
							"type"		: "line",

							"x"			: 0,
							"y"			: 0,

							"width"		: 0,
							"height"	: BOARD_HEIGHT,

							"color"		: 0xff2A2521,
						},
						{
							"name"		: "top_line",
							"type"		: "line",

							"x"			: 0,
							"y"			: 0,

							"width"		: BOARD_WIDTH,
							"height"	: 0,

							"color"		: 0xff2A2521,
						},
						
					),
				
				},

				## yut_flag_count_window
				{
					"name"				: "yut_flag_count_window",
					"type"				: "window",
					
					"style"				: ("attach", "ltr",),

					"x"					: 15,
					"y"					: WINDOW_HEIGHT - 60,
					
					"width"				: BOARD_WIDTH / 2,
					"height"			: 32,
								
					"horizontal_align"	: "left",
								
					"children" :
					(
						## yut_piece_count
						{
							"name"		: "yut_piece_count_bg",
							"type"		: "image",
							"x"			: 0,
							"y"			: 0,
							"image"		: ROOT_PATH + "slot.sub",
						},
						{
							"name"		: "yut_piece_count_slot",
							"type"		: "slot",

							"x"			: 7,
							"y"			: 7,
							"width"		: 32,
							"height"	: 32,
							
							"slot"		: ({"index":0, "x":0, "y":0, "width":32, "height":32,},),
						},
						{
							"name"		: "yut_piece_count_text_bg",
							"type"		: "image",

							"x"			: 50,
							"y"			: 13,
							
							"image"		: ROOT_PATH + "count_bg2.sub",

							"children" :
							(
								{
									"name"					: "yut_piece_count_text",
									"type"					: "text",

									"x"						: 0,
									"y"						: 3,

									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",

									"text"					: "",
								},
							),
						},
									
						## yut_board_count
						{
							"name"		: "yut_board_count_bg",
							"type"		: "image",
							"x"			: 120,
							"y"			: 0,
							"image"		: ROOT_PATH + "slot.sub",
						},
						{
							"name"		: "yut_board_count_slot",
							"type"		: "slot",

							"x"			: 127,
							"y"			: 7,
							"width"		: 32,
							"height"	: 32,
							
							"slot"		: ({"index":0, "x":0, "y":0, "width":32, "height":32,},),
						},
						{
							"name"		: "yut_board_count_text_bg",
							"type"		: "image",

							"x"			: 170,
							"y"			: 13,
							
							"image"		: ROOT_PATH + "count_bg2.sub",

							"children" :
							(
								{
									"name"					: "yut_board_count_text",
									"type"					: "text",

									"x"						: 0,
									"y"						: 3,

									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",

									"text"					: "",
								},
							),
						},
					),
				},

				{
					"name"			: "prev_button",
					"type"			: "button",

					"x"				: WINDOW_WIDTH - 60,
					"y"				: WINDOW_HEIGHT - 25,

					"default_image" : PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
				},
				
				{
					"name"			: "next_button",
					"type"			: "button",

					"x"				: WINDOW_WIDTH - 35,
					"y"				: WINDOW_HEIGHT - 25,

					"default_image" : PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
				},
				
				{
					"name"				: "game_start_button",
					"type"				: "button",
					
					"x"					: 120,
					"y"					: WINDOW_HEIGHT - 50,
					
					"text"				: uiScriptLocale.MINI_GAME_YUTNORI_START_TEXT,

					"horizontal_align"	: "center",					
					
					"default_image"		: "d:/ymir work/ui/public/large_button_01.sub",
					"over_image"		: "d:/ymir work/ui/public/large_button_02.sub",
					"down_image"		: "d:/ymir work/ui/public/large_button_03.sub",
				},				
			),
		},
	),	
}