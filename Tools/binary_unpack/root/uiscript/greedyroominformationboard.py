#if app.ENABLE_GREEDY_ROOM
import uiScriptLocale
import localeInfo

PUBLIC_PATH						= "d:/ymir work/ui/public/"
ROOT_PATH						= "d:/ymir work/ui/game/greedy_room/"

WINDOW_WIDTH					= 145
WINDOW_HEIGHT					= 112

window = {
	"name"		: "greedy_room_information_board_window",
	"style"		: ("float", "not_pick",),

	"x"			: SCREEN_WIDTH - WINDOW_WIDTH,
	"y"			: 155,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	(
		{
			"name" : "information_board_bg",
			"type" : "image",

			"x" : 0,
			"y" : 0,

			"image" : ROOT_PATH + "information_board_bg.sub",
		},

		{
			"name" : "information_board_bg_outline",
			"type" : "image",

			"x" : 0,
			"y" : 0,

			"image" : ROOT_PATH + "information_board_bg_outline.sub",
		},

		# 암흑의 저주 게이지 UI
		{
			"name"		: "curse_of_darkness_gague_window",
			"type"		: "window",
			
			"x"			: 0,
			"y"			: 28,
			
			"width"		: WINDOW_WIDTH,
			"height"	: 32,

			"children" :
			(
				# 빈 게이지
				{
					"name" : "empty_gauge_0",
					"type" : "image",

					"x" : 8,
					"y" : 2,

					"image" : ROOT_PATH + "information_board_gauge_circle_empty.sub",
				},
				{
					"name" : "empty_gauge_1",
					"type" : "image",

					"x" : 34,
					"y" : 2,

					"image" : ROOT_PATH + "information_board_gauge_circle_empty.sub",
				},
				{
					"name" : "empty_gauge_2",
					"type" : "image",

					"x" : 60,
					"y" : 2,

					"image" : ROOT_PATH + "information_board_gauge_circle_empty.sub",
				},
				{
					"name" : "empty_gauge_3",
					"type" : "image",

					"x" : 86,
					"y" : 2,

					"image" : ROOT_PATH + "information_board_gauge_circle_empty.sub",
				},
				{
					"name" : "empty_gauge_4",
					"type" : "image",

					"x" : 112,
					"y" : 2,

					"image" : ROOT_PATH + "information_board_gauge_circle_empty.sub",
				},

				# 게이지 : 파랑
				{
					"name" : "gauge_blue_0",
					"type" : "expanded_image",

					"x" : 6,
					"y" : 0,

					"image" : ROOT_PATH + "information_board_gauge_circle_blue.sub",
				},
				{
					"name" : "gauge_blue_1",
					"type" : "expanded_image",

					"x" : 32,
					"y" : 0,

					"image" : ROOT_PATH + "information_board_gauge_circle_blue.sub",
				},
				{
					"name" : "gauge_blue_2",
					"type" : "expanded_image",

					"x" : 58,
					"y" : 0,

					"image" : ROOT_PATH + "information_board_gauge_circle_blue.sub",
				},
				{
					"name" : "gauge_blue_3",
					"type" : "expanded_image",

					"x" : 84,
					"y" : 0,

					"image" : ROOT_PATH + "information_board_gauge_circle_blue.sub",
				},
				{
					"name" : "gauge_blue_4",
					"type" : "expanded_image",

					"x" : 110,
					"y" : 0,

					"image" : ROOT_PATH + "information_board_gauge_circle_blue.sub",
				},

				# 게이지 : 빨강
				{
					"name" : "gauge_red_0",
					"type" : "expanded_image",

					"x" : 6,
					"y" : 0,

					"image" : ROOT_PATH + "information_board_gauge_circle_red.sub",
				},
				{
					"name" : "gauge_red_1",
					"type" : "expanded_image",

					"x" : 32,
					"y" : 0,

					"image" : ROOT_PATH + "information_board_gauge_circle_red.sub",
				},
				{
					"name" : "gauge_red_2",
					"type" : "expanded_image",

					"x" : 58,
					"y" : 0,

					"image" : ROOT_PATH + "information_board_gauge_circle_red.sub",
				},
				{
					"name" : "gauge_red_3",
					"type" : "expanded_image",

					"x" : 84,
					"y" : 0,

					"image" : ROOT_PATH + "information_board_gauge_circle_red.sub",
				},
				{
					"name" : "gauge_red_4",
					"type" : "expanded_image",

					"x" : 110,
					"y" : 0,

					"image" : ROOT_PATH + "information_board_gauge_circle_red.sub",
				},

				# 반짝거리는 게이지
				{
					"name" : "shine_gauge_0",
					"type" : "ani_image",
					
					"x" : 6,
					"y" : 0,
					
					"delay" : 10,

					"images" :
					(
						ROOT_PATH + "information_board_gauge_shine_01.sub",
						ROOT_PATH + "information_board_gauge_shine_02.sub",
						ROOT_PATH + "information_board_gauge_shine_03.sub",
						ROOT_PATH + "information_board_gauge_shine_04.sub",
						ROOT_PATH + "information_board_gauge_shine_03.sub",
						ROOT_PATH + "information_board_gauge_shine_02.sub",
						ROOT_PATH + "information_board_gauge_shine_01.sub",
					),
				},
				{
					"name" : "shine_gauge_1",
					"type" : "ani_image",
					
					"x" : 32,
					"y" : 0,
					
					"delay" : 10,

					"images" :
					(
						ROOT_PATH + "information_board_gauge_shine_01.sub",
						ROOT_PATH + "information_board_gauge_shine_02.sub",
						ROOT_PATH + "information_board_gauge_shine_03.sub",
						ROOT_PATH + "information_board_gauge_shine_04.sub",
						ROOT_PATH + "information_board_gauge_shine_03.sub",
						ROOT_PATH + "information_board_gauge_shine_02.sub",
						ROOT_PATH + "information_board_gauge_shine_01.sub",
					),
				},
				{
					"name" : "shine_gauge_2",
					"type" : "ani_image",
					
					"x" : 58,
					"y" : 0,
					
					"delay" : 10,

					"images" :
					(
						ROOT_PATH + "information_board_gauge_shine_01.sub",
						ROOT_PATH + "information_board_gauge_shine_02.sub",
						ROOT_PATH + "information_board_gauge_shine_03.sub",
						ROOT_PATH + "information_board_gauge_shine_04.sub",
						ROOT_PATH + "information_board_gauge_shine_03.sub",
						ROOT_PATH + "information_board_gauge_shine_02.sub",
						ROOT_PATH + "information_board_gauge_shine_01.sub",
					),
				},
				{
					"name" : "shine_gauge_3",
					"type" : "ani_image",
					
					"x" : 84,
					"y" : 0,
					
					"delay" : 10,

					"images" :
					(
						ROOT_PATH + "information_board_gauge_shine_01.sub",
						ROOT_PATH + "information_board_gauge_shine_02.sub",
						ROOT_PATH + "information_board_gauge_shine_03.sub",
						ROOT_PATH + "information_board_gauge_shine_04.sub",
						ROOT_PATH + "information_board_gauge_shine_03.sub",
						ROOT_PATH + "information_board_gauge_shine_02.sub",
						ROOT_PATH + "information_board_gauge_shine_01.sub",
					),
				},
				{
					"name" : "shine_gauge_4",
					"type" : "ani_image",
					
					"x" : 110,
					"y" : 0,
					
					"delay" : 10,

					"images" :
					(
						ROOT_PATH + "information_board_gauge_shine_01.sub",
						ROOT_PATH + "information_board_gauge_shine_02.sub",
						ROOT_PATH + "information_board_gauge_shine_03.sub",
						ROOT_PATH + "information_board_gauge_shine_04.sub",
						ROOT_PATH + "information_board_gauge_shine_03.sub",
						ROOT_PATH + "information_board_gauge_shine_02.sub",
						ROOT_PATH + "information_board_gauge_shine_01.sub",
					),
				},
			),
		},

		# 보스 이미지
		{
			"name"		: "boss_img_window",
			"type"		: "window",
			
			"x"			: 0,
			"y"			: 112,
			
			"width"		: 32,
			"height"	: 32,

			"children" :
			(
				#보스 빈 이미지
				{
					"name" : "empty_boss_img",
					"type" : "image",

					"x" : 0,
					"y" : 2,
							
					"image"	: PUBLIC_PATH + "Slot_Base.sub",	
				},

				# 보스 이미지
				{
					"name" : "boss_img",
					"type" : "image",

					"x" : 0,
					"y" : 2,
							
					"image"	: ROOT_PATH + "iblis_portrait.sub",	
				},
			),
		},

		# 보스 hp
		{
			"name"		: "boss_thin_board",
			"type"		: "thinboard",
			
			"x"			: 32,
			"y"			: 112,
			
			"width"		: 110,
			"height"	: 36,

			"children" :
			(
				{
					"name" : "boss_name",
					"type" : "text",

					"x" : 0,
					"y" : -6,

					"horizontal_align"		: "center",
					"text_horizontal_align" : "center",

					"vertical_align"		: "center",
					"text_vertical_align"	: "center",
				},

				{
					"name"					: "boss_hp_gague",
					"type"					: "gauge",

					"x"						: 10,
					"y"						: 21,

					"width"					: 90,
					"color"					: "red",
				},
			),
		},

		## exit button
		{
			"name" : "exit_button",
			"type" : "button",
			
			"x" : 58,
			"y" : 58,
			
			"default_image"	: ROOT_PATH + "information_board_exit_button_default.sub",
			"over_image"	: ROOT_PATH + "information_board_exit_button_over.sub",
			"down_image"	: ROOT_PATH + "information_board_exit_button_down.sub",
		},
	),
}