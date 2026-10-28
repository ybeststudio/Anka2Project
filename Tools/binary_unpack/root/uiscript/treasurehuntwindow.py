import uiScriptLocale
import localeInfo

PUBLIC_PATH						= "d:/ymir work/ui/public/"
PATTERN_PATH					= "d:/ymir work/ui/pattern/"
ROOT_PATH						= "d:/ymir work/ui/game/treasure_hunt/event/"

WINDOW_WIDTH					= 314
WINDOW_HEIGHT					= 272
MAIN_WINDOW_WIDTH				= 292
MAIN_WINDOW_HEIGHT				= 202
MAIN_WINDOW_PATTERN_X_COUNT		= (MAIN_WINDOW_WIDTH - 32) / 16
MAIN_WINDOW_PATTERN_Y_COUNT		= (MAIN_WINDOW_HEIGHT - 32) / 16
SLOT_WIDTH						= 32
SLOT_HEIGHT						= 32


window = {
	"name"		: "treasure_hunt_event_window",
	"style"		: ("movable", "float", ),

	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH/2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT/2,
	
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
			
			# 보물 찾기
			"title"		: uiScriptLocale.TREASURE_HUNT_EVENT_TITLE,

			"children" :
			(
				# 테두리
				{
					"name"		: "outline_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 10,
					"y"			: 32,

					"width"		: MAIN_WINDOW_WIDTH,
					"height"	: MAIN_WINDOW_HEIGHT,

					"children" :
					(
						{
							"name"		: "desc_window_background",
							"type"		: "outline_window",
							"x"			: 0,
							"y"			: 0,
							"width"		: MAIN_WINDOW_WIDTH,
							"height"	: MAIN_WINDOW_HEIGHT,
						},
					),
				},
				# 보상 목록
				{
					"name" : "reward_bg",
					"type" : "image",

					"x" : 13,
					"y" : 34,
							
					"image"	: ROOT_PATH + "main_title.sub",	

					"children":
					(
						{
							"name" : "reward_list_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",

							"vertical_align" : "center",
							"text_vertical_align" : "center",

							# 보상 목록
							"text" : uiScriptLocale.TREASURE_HUNT_EVENT_REWARD_LIST,
						},
					),
				},
				{
					"name" : "reward_slot_bg",
					"type" : "image",

					"x" : 20,
					"y" : 62,
							
					"image"	: ROOT_PATH + "reward_slot.sub",
					
					"children":
					(
						{
							"name" : "reward_slot", "type" : "slot", "x" : 21-20, "y" : 63-62, "width" : 160, "height" : 160,
							"slot" :
							(
								# line 1
								{"index":0, "x":SLOT_WIDTH * 0, "y":SLOT_HEIGHT * 0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":1, "x":SLOT_WIDTH * 1, "y":SLOT_HEIGHT * 0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":2, "x":SLOT_WIDTH * 2, "y":SLOT_HEIGHT * 0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":3, "x":SLOT_WIDTH * 3, "y":SLOT_HEIGHT * 0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":4, "x":SLOT_WIDTH * 4, "y":SLOT_HEIGHT * 0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},

								# line 2
								{"index":5, "x":SLOT_WIDTH * 0, "y":SLOT_HEIGHT * 1, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":6, "x":SLOT_WIDTH * 1, "y":SLOT_HEIGHT * 1, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":7, "x":SLOT_WIDTH * 2, "y":SLOT_HEIGHT * 1, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":8, "x":SLOT_WIDTH * 3, "y":SLOT_HEIGHT * 1, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":9, "x":SLOT_WIDTH * 4, "y":SLOT_HEIGHT * 1, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},

								# line 3
								{"index":10, "x":SLOT_WIDTH * 0, "y":SLOT_HEIGHT * 2, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":11, "x":SLOT_WIDTH * 1, "y":SLOT_HEIGHT * 2, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":12, "x":SLOT_WIDTH * 2, "y":SLOT_HEIGHT * 2, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":13, "x":SLOT_WIDTH * 3, "y":SLOT_HEIGHT * 2, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":14, "x":SLOT_WIDTH * 4, "y":SLOT_HEIGHT * 2, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},

								# line 4
								{"index":15, "x":SLOT_WIDTH * 0, "y":SLOT_HEIGHT * 3, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":16, "x":SLOT_WIDTH * 1, "y":SLOT_HEIGHT * 3, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":17, "x":SLOT_WIDTH * 2, "y":SLOT_HEIGHT * 3, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":18, "x":SLOT_WIDTH * 3, "y":SLOT_HEIGHT * 3, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":19, "x":SLOT_WIDTH * 4, "y":SLOT_HEIGHT * 3, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},

								# line 5
								{"index":20, "x":SLOT_WIDTH * 0, "y":SLOT_HEIGHT * 4, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":21, "x":SLOT_WIDTH * 1, "y":SLOT_HEIGHT * 4, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":22, "x":SLOT_WIDTH * 2, "y":SLOT_HEIGHT * 4, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":23, "x":SLOT_WIDTH * 3, "y":SLOT_HEIGHT * 4, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":24, "x":SLOT_WIDTH * 4, "y":SLOT_HEIGHT * 4, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
							),	
						},
					),
				},
				# 보유 금화
				{
					"name" : "gold_bg",
					"type" : "image",

					"x" : 192,
					"y" : 34,
							
					"image"	: ROOT_PATH + "gold_count_bg.sub",	

					"children":
					(
						{
							"name" : "gold_title_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align"		: "center",
							"text_horizontal_align"	: "center",

							"vertical_align"		: "center",
							"text_vertical_align"	: "center",

							# 보유 금화
							"text" : uiScriptLocale.TREASURE_HUNT_EVENT_GOLD,
						},
					),
				},
				{
					"name" : "gold_icon_bg",
					"type" : "image",

					"x" : 211,
					"y" : 66,
							
					"image"	: ROOT_PATH + "gold_icon.sub",	
				},
				{
					"name" : "gold_count_bg",
					"type" : "image",

					"x" : 234,
					"y" : 67,

					"width" : 39,
					"height" : 18,
					
					"image"		: ROOT_PATH + "gold_text_bg.sub",

					"children":
					(
						{
							"name" : "gold_count_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align"		: "center",
							"text_horizontal_align" : "center",

							"vertical_align"		: "center",
							"text_vertical_align"	: "center",
							"text" : "0",
						},
					),
				},
				# 보상 세팅 버튼
				{
					"name" : "reward_set_button", "type" : "button", "x" : 208, "y" : 97,
					
					"default_image"	: ROOT_PATH + "reward_set_bt_default.sub", 
					"over_image"	: ROOT_PATH + "reward_set_bt_over.sub",
					"down_image"	: ROOT_PATH + "reward_set_bt_down.sub",
				},

				# 보상 세팅 버튼 (비활성화 버튼)
				{
					"name" : "reward_set_button_off", "type" : "button", "x" : 208, "y" : 97,
					
					"default_image"	: ROOT_PATH + "reward_set_bt_down.sub", 
					"over_image"	: ROOT_PATH + "reward_set_bt_down.sub",
					"down_image"	: ROOT_PATH + "reward_set_bt_down.sub",
				},

				# 누적횟수
				{
					"name" : "accumulate_count_bg",
					"type" : "image",

					"x" : 192,
					"y" : 132,
							
					"image"	: ROOT_PATH + "accumulate_count_bg.sub",	

					"children":
					(
						{
							"name" : "accumulate_count_title_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",

							"vertical_align" : "center",
							"text_vertical_align" : "center",

							# 누적횟수
							"text" : uiScriptLocale.TREASURE_HUNT_EVENT_ACCUMULATE_COUNT,
						},
					),
				},
				# 물음표
				{
					"name"			: "accumulate_count_help_button",
					"type"			: "button",
					"x"				: 216,
					"y"				: 167,
					"default_image"	: PATTERN_PATH + "q_mark_01.tga",
					"over_image"	: PATTERN_PATH + "q_mark_02.tga",
					"down_image"	: PATTERN_PATH + "q_mark_01.tga",
				},		
				{
					"name" : "accumulate_count_text_bg",
					"type" : "image",

					"x" : 236,
					"y" : 166,

					"width" : 35,
					"height" : 18,
					
					"image"		: ROOT_PATH + "gold_text_bg.sub",

					"children":
					(
						{
							"name" : "accumulate_count_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",

							"vertical_align" : "center",
							"text_vertical_align" : "center",

							"text" : "0",
						},
					),
				},
				# 누적 보상 보기 버튼
				{
					"name" : "accumulate_reward_list_button", "type" : "button", "x" : 208, "y" : 197,
					
					"default_image"	: ROOT_PATH + "reward_list_bt_default.sub", 
					"over_image"	: ROOT_PATH + "reward_list_bt_over.sub",
					"down_image"	: ROOT_PATH + "reward_list_bt_down.sub",
				},
				# 세로 라인
				{
					"name" : "main_bg_line",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 189,
					"y" : 32,
					"image" : ROOT_PATH + "main_bg_line.sub",
				},
				# 보물찾기 버튼 (활성화)
				{
					"name" : "reward_button", "type" : "button", "x" : 12, "y" : 238,
					
					"default_image"	: ROOT_PATH + "reward_bt_default.sub", 
					"over_image"	: ROOT_PATH + "reward_bt_over.sub",
					"down_image"	: ROOT_PATH + "reward_bt_down.sub",
				},
				# 보물찾기 버튼(비활성화)
				{
					"name" : "reward_off_button", "type" : "button", "x" : 12, "y" : 238,
					
					"default_image"	: ROOT_PATH + "reward_bt_down.sub", 
					"over_image"	: ROOT_PATH + "reward_bt_down.sub",
					"down_image"	: ROOT_PATH + "reward_bt_down.sub",
				},
				# 열쇠
				{
					"name" : "key_img",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 93,
					"y" : 240,
					"image" : ROOT_PATH + "key_icon.sub",
				},
				# 열쇠 개수
				{
					"name" : "key_count_text_bg",
					"type" : "image",

					"x" : 113,
					"y" : 240,

					"width" : 35,
					"height" : 18,
					
					"image"		: ROOT_PATH + "key_count_text_bg.sub",

					"children":
					(
						{
							"name" : "key_count_text",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align" : "center",
							"text_horizontal_align" : "center",

							"vertical_align" : "center",
							"text_vertical_align" : "center",

							"text" : "0",
						},
					),
				},
				# 초기화 버튼 (활성화)
				{
					"name" : "reset_button", "type" : "button", "x" : 151, "y" : 239,
					
					"default_image"	: ROOT_PATH + "reset_bt_default.sub", 
					"over_image"	: ROOT_PATH + "reset_bt_over.sub",
					"down_image"	: ROOT_PATH + "reset_bt_down.sub",
				},

				# 초기화 버튼 (비활성화)
				{
					"name" : "reset_off_button", "type" : "button", "x" : 151, "y" : 239,
					
					"default_image"	: ROOT_PATH + "reset_bt_down.sub", 
					"over_image"	: ROOT_PATH + "reset_bt_down.sub",
					"down_image"	: ROOT_PATH + "reset_bt_down.sub",
				},

				# 누적 랭킹 버튼
				{
					"name" : "ranking_button", "type" : "button", "x" : 228, "y" : 239,
					
					"default_image"	: ROOT_PATH + "ranking_bt_default.sub", 
					"over_image"	: ROOT_PATH + "ranking_bt_over.sub",
					"down_image"	: ROOT_PATH + "ranking_bt_down.sub",
				},
			),
		},
	),
}