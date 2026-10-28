import uiScriptLocale
import localeInfo
import app

PUBLIC_PATH					= "d:/ymir work/ui/public/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"
ROOT_PATH					= "d:/ymir work/ui/event/"
SPORTS_MATCH_EVENT_PATH		= "d:/ymir work/ui/minigame/sports_match/"

WINDOW_WIDTH				= 310
WINDOW_HEIGHT				= 454

MAIN_WINDOW_WIDTH			= 288
MAIN_WINDOW_HEIGHT			= 410

MAIN_WINDOW_PATTERN_X_COUNT	= (MAIN_WINDOW_WIDTH - 32) / 16
MAIN_WINDOW_PATTERN_Y_COUNT	= (MAIN_WINDOW_HEIGHT - 32) / 16

SLOT_IMAGE_WIDTH			= 48
SLOT_IMAGE_HEIGHT			= 48
SLOT_WIDTH					= 33
SLOT_HEIGHT					= 33

DESC_WINDOW_WIDTH			= 345
DESC_WINDOW_HEIGHT			= 190

EXCHANGE_WINDOW_HEIGHT		= 56

START_POS_X					= 10
START_POS_Y					= 32

DESCRIPTION_Y_HEIGHT		= 90

TEAM_LIST_WINDOW_HEIGHT		= 285
TEAM_BG_WIDTH				= 276
TEAM_BG_HEIGHT				= 31

SMALL_HEIGHT_GAP			= 4

PAGE_BUTTON_PREV_XPOS		= 20
PAGE_BUTTON_WIDTH			= 10
PAGE_BUTTON_NUMBER_WIDTH	= 32
PAGE_BUTTON_GAP				= 10
PAGE_BUTTON_NUMBER_GAP		= 3
NUMBER_BTN_START_POS_X		= 5
PAGE_WINDOW_WIDTH			= MAIN_WINDOW_WIDTH - 40

if localeInfo.IsARABIC():
	PREV_BUTTON_POS_X		= 40
	NEXT_BUTTON_POS_X		= 10
	TEAM_LIST_WINDOW_POS_X	= START_POS_X - 3
else:
	PREV_BUTTON_POS_X		= MAIN_WINDOW_WIDTH - 60
	NEXT_BUTTON_POS_X		= MAIN_WINDOW_WIDTH - 30
	TEAM_LIST_WINDOW_POS_X	= START_POS_X + 3


if app.ENABLE_FIX_SPORTS_MATCH_DESC:
	window = {
		"name"		: "SportsMatchPredictWindow",
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
			
				## 승부 예측 이벤트
				"title"		: localeInfo.SPORTS_MATCH_EVENT_PREDICT_UI_TITLE,

				"children" :
				[
					## background
					{
						"name"		: "line_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: START_POS_X,
						"y"			: START_POS_Y,

						"width"		: MAIN_WINDOW_WIDTH,
						"height"	: MAIN_WINDOW_HEIGHT,

						"children" :
						[
					
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
								"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT - 1, MAIN_WINDOW_PATTERN_Y_COUNT),
							},

							## 가로 경계 라인
							{
								"name" : "line_window_horizontal_boundary_left_line",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : DESCRIPTION_Y_HEIGHT,

								"image" : ROOT_PATH + "horizontal_line_left.sub",
							},

							{
								"name" : "line_window_horizontal_boundary_right_line",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : MAIN_WINDOW_WIDTH - 8,
								"y" : DESCRIPTION_Y_HEIGHT,

								"image" : ROOT_PATH + "horizontal_line_right.sub",
							},

							{
								"name" : "line_window_middle_boundary_center_line",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 8,
								"y" : DESCRIPTION_Y_HEIGHT,
							
								"image" : PATTERN_PATH + "border_A_top.tga",
								"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
							},

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
						],
					},

					## 설명 window
					{
						"name"		: "description_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: START_POS_X + 3,
						"y"			: START_POS_Y + 3,

						"width"		: MAIN_WINDOW_WIDTH,
						"height"	: DESCRIPTION_Y_HEIGHT,
					},

					## 팀 정보 리스트 window
					{
						"name"		: "team_list_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: TEAM_LIST_WINDOW_POS_X,
						"y"			: START_POS_Y + DESCRIPTION_Y_HEIGHT+2,

						"width"		: MAIN_WINDOW_WIDTH,
						"height"	: TEAM_LIST_WINDOW_HEIGHT,

						#"children" :
						#[
						#	{
						#		"name"		: "team_bg_0",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: TEAM_BG_HEIGHT*0 + SMALL_HEIGHT_GAP,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",

						#		# 팀 이미지
						#		# 팀 명칭
						#		# 응원 횟수
						#		# 버튼
							
						#	},

						#	{
						#		"name"		: "team_bg_1",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: TEAM_BG_HEIGHT*1 + SMALL_HEIGHT_GAP*2,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_2",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: TEAM_BG_HEIGHT*2 +  SMALL_HEIGHT_GAP*3,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_3",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: SMALL_HEIGHT_GAP*4 + TEAM_BG_HEIGHT*3,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_4",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: SMALL_HEIGHT_GAP*5 + TEAM_BG_HEIGHT*4,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_5",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: SMALL_HEIGHT_GAP*6 + TEAM_BG_HEIGHT*5,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_6",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: SMALL_HEIGHT_GAP*7 + TEAM_BG_HEIGHT*6,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_7",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: SMALL_HEIGHT_GAP*8 + TEAM_BG_HEIGHT*7,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#],
					},

					## 페이지 이동 버튼 영역 window
					{
						"name"		: "page_button_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: 0,
						"y"			: START_POS_Y + DESCRIPTION_Y_HEIGHT+2 + TEAM_LIST_WINDOW_HEIGHT,

						"horizontal_align"	: "center",

						"width"		: PAGE_WINDOW_WIDTH,
						"height"	: MAIN_WINDOW_HEIGHT - DESCRIPTION_Y_HEIGHT - TEAM_LIST_WINDOW_HEIGHT,

						"children" :
						[
							{
								"name" : "first_prev_page_button",
								"type" : "button",
								"x" : 0,
								"y" : 0,
								"vertical_align"	: "center",
								"default_image" : "d:/ymir work/ui/privatesearch/private_first_prev_btn_01.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_first_prev_btn_02.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_first_prev_btn_01.sub",
							},
							## 이전 버튼 10*10
							{
								"name" : "prev_page_button",
								"type" : "button",

								"x" : PAGE_BUTTON_WIDTH + PAGE_BUTTON_GAP ,
								"y" : 0,
								"vertical_align"	: "center",

								"default_image" : "d:/ymir work/ui/privatesearch/private_prev_btn_01.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_prev_btn_02.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_prev_btn_01.sub",
							},
							{
								"name" : "page_button0",
								"type" : "button",
								"x" : PAGE_WINDOW_WIDTH/2-PAGE_BUTTON_NUMBER_WIDTH/2 - PAGE_BUTTON_NUMBER_WIDTH*2 - PAGE_BUTTON_NUMBER_GAP*2,
								"y" : 0,
								"vertical_align"	: "center",

								"text" : "1",

								"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
							},
							{
								"name" : "page_button1",
								"type" : "button",
								"x" : PAGE_WINDOW_WIDTH/2-PAGE_BUTTON_NUMBER_WIDTH/2 - PAGE_BUTTON_NUMBER_WIDTH - PAGE_BUTTON_NUMBER_GAP,
							
								"y" : 0,
								"vertical_align"	: "center",
								"text" : "2",

								"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
							},
							{
								"name" : "page_button2",
								"type" : "button",
								"x" : PAGE_WINDOW_WIDTH/2-PAGE_BUTTON_NUMBER_WIDTH/2,
								"y" : 0,
								"vertical_align"	: "center",
								"text" : "3",

								"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
							},
							{
								"name" : "page_button3",
								"type" : "button",
								"x" : PAGE_WINDOW_WIDTH/2-PAGE_BUTTON_NUMBER_WIDTH/2 + PAGE_BUTTON_NUMBER_WIDTH + PAGE_BUTTON_NUMBER_GAP,
								"y" : 0,
								"vertical_align"	: "center",
								"text" : "4",

								"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
							},
							{
								"name" : "page_button4",
								"type" : "button",
								"x" : PAGE_WINDOW_WIDTH/2-PAGE_BUTTON_NUMBER_WIDTH/2 + PAGE_BUTTON_NUMBER_WIDTH*2 + PAGE_BUTTON_NUMBER_GAP*2,
								"y" : 0,
								"vertical_align"	: "center",
								"text" : "5",

								"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
							},
							## 다음 버튼 10*10
							{
								"name" : "next_page_button", 
								"type" : "button",

								"x" : PAGE_WINDOW_WIDTH - PAGE_BUTTON_WIDTH - PAGE_BUTTON_WIDTH - PAGE_BUTTON_GAP ,
								"y" : 0,
								"vertical_align"	: "center",
								"default_image" : "d:/ymir work/ui/privatesearch/private_next_btn_01.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_next_btn_02.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_next_btn_01.sub",
							},
							## 다음 * 10 버튼 13*10
							{
								"name" : "last_next_page_button", 
								"type" : "button",

								"x" : PAGE_WINDOW_WIDTH - PAGE_BUTTON_WIDTH,
								"y" : 0,
								"vertical_align"	: "center",

								"default_image" : "d:/ymir work/ui/privatesearch/private_last_next_btn_01.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_last_next_btn_02.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_last_next_btn_01.sub",
							},
						],
					},
				],
			},
		],
	}
else:
	window = {
		"name"		: "SportsMatchPredictWindow",
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
			
				## 승부 예측 이벤트
				"title"		: localeInfo.SPORTS_MATCH_EVENT_PREDICT_UI_TITLE,

				"children" :
				[
					## background
					{
						"name"		: "line_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: START_POS_X,
						"y"			: START_POS_Y,

						"width"		: MAIN_WINDOW_WIDTH,
						"height"	: MAIN_WINDOW_HEIGHT,

						"children" :
						[
					
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
								"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT - 1, MAIN_WINDOW_PATTERN_Y_COUNT),
							},

							## 가로 경계 라인
							{
								"name" : "line_window_horizontal_boundary_left_line",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : 0,
								"y" : DESCRIPTION_Y_HEIGHT,

								"image" : ROOT_PATH + "horizontal_line_left.sub",
							},

							{
								"name" : "line_window_horizontal_boundary_right_line",
								"type" : "image",
								"style" : ("ltr",),
							
								"x" : MAIN_WINDOW_WIDTH - 8,
								"y" : DESCRIPTION_Y_HEIGHT,

								"image" : ROOT_PATH + "horizontal_line_right.sub",
							},

							{
								"name" : "line_window_middle_boundary_center_line",
								"type" : "expanded_image",
								"style" : ("ltr",),
							
								"x" : 8,
								"y" : DESCRIPTION_Y_HEIGHT,
							
								"image" : PATTERN_PATH + "border_A_top.tga",
								"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
							},

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
						],
					},

					## 설명 window
					{
						"name"		: "description_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: START_POS_X + 3,
						"y"			: START_POS_Y + 3,

						"width"		: MAIN_WINDOW_WIDTH,
						"height"	: DESCRIPTION_Y_HEIGHT,

						"children" :
						[
							{
								"name"			: "prev_button",
								"type"			: "button",

								"x"				: PREV_BUTTON_POS_X,
								"y"				: DESCRIPTION_Y_HEIGHT - 20,

								"default_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
								"over_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_02.sub",
								"down_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
							},
						
							{
								"name"			: "next_button",
								"type"			: "button",

								"x"				: NEXT_BUTTON_POS_X,
								"y"				: DESCRIPTION_Y_HEIGHT - 20,

								"default_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
								"over_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_02.sub",
								"down_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
							},
						],
					},

					## 팀 정보 리스트 window
					{
						"name"		: "team_list_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: TEAM_LIST_WINDOW_POS_X,
						"y"			: START_POS_Y + DESCRIPTION_Y_HEIGHT+2,

						"width"		: MAIN_WINDOW_WIDTH,
						"height"	: TEAM_LIST_WINDOW_HEIGHT,

						#"children" :
						#[
						#	{
						#		"name"		: "team_bg_0",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: TEAM_BG_HEIGHT*0 + SMALL_HEIGHT_GAP,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",

						#		# 팀 이미지
						#		# 팀 명칭
						#		# 응원 횟수
						#		# 버튼
							
						#	},

						#	{
						#		"name"		: "team_bg_1",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: TEAM_BG_HEIGHT*1 + SMALL_HEIGHT_GAP*2,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_2",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: TEAM_BG_HEIGHT*2 +  SMALL_HEIGHT_GAP*3,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_3",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: SMALL_HEIGHT_GAP*4 + TEAM_BG_HEIGHT*3,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_4",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: SMALL_HEIGHT_GAP*5 + TEAM_BG_HEIGHT*4,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_5",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: SMALL_HEIGHT_GAP*6 + TEAM_BG_HEIGHT*5,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_6",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: SMALL_HEIGHT_GAP*7 + TEAM_BG_HEIGHT*6,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#	{
						#		"name"		: "team_bg_7",
						#		"type"		: "image",
							
						#		"x"			: 3,
						#		"y"			: SMALL_HEIGHT_GAP*8 + TEAM_BG_HEIGHT*7,
							
						#		"width"		: TEAM_BG_WIDTH,
						#		"height"	: TEAM_BG_HEIGHT,
							
						#		"image"		: SPORTS_MATCH_EVENT_PATH + "team_bg.sub",
						#	},

						#],
					},

					## 페이지 이동 버튼 영역 window
					{
						"name"		: "page_button_window",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
					
						"x"			: 0,
						"y"			: START_POS_Y + DESCRIPTION_Y_HEIGHT+2 + TEAM_LIST_WINDOW_HEIGHT,

						"horizontal_align"	: "center",

						"width"		: PAGE_WINDOW_WIDTH,
						"height"	: MAIN_WINDOW_HEIGHT - DESCRIPTION_Y_HEIGHT - TEAM_LIST_WINDOW_HEIGHT,

						"children" :
						[
							{
								"name" : "first_prev_page_button",
								"type" : "button",
								"x" : 0,
								"y" : 0,
								"vertical_align"	: "center",
								"default_image" : "d:/ymir work/ui/privatesearch/private_first_prev_btn_01.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_first_prev_btn_02.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_first_prev_btn_01.sub",
							},
							## 이전 버튼 10*10
							{
								"name" : "prev_page_button",
								"type" : "button",

								"x" : PAGE_BUTTON_WIDTH + PAGE_BUTTON_GAP ,
								"y" : 0,
								"vertical_align"	: "center",

								"default_image" : "d:/ymir work/ui/privatesearch/private_prev_btn_01.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_prev_btn_02.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_prev_btn_01.sub",
							},
							{
								"name" : "page_button0",
								"type" : "button",
								"x" : PAGE_WINDOW_WIDTH/2-PAGE_BUTTON_NUMBER_WIDTH/2 - PAGE_BUTTON_NUMBER_WIDTH*2 - PAGE_BUTTON_NUMBER_GAP*2,
								"y" : 0,
								"vertical_align"	: "center",

								"text" : "1",

								"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
							},
							{
								"name" : "page_button1",
								"type" : "button",
								"x" : PAGE_WINDOW_WIDTH/2-PAGE_BUTTON_NUMBER_WIDTH/2 - PAGE_BUTTON_NUMBER_WIDTH - PAGE_BUTTON_NUMBER_GAP,
							
								"y" : 0,
								"vertical_align"	: "center",
								"text" : "2",

								"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
							},
							{
								"name" : "page_button2",
								"type" : "button",
								"x" : PAGE_WINDOW_WIDTH/2-PAGE_BUTTON_NUMBER_WIDTH/2,
								"y" : 0,
								"vertical_align"	: "center",
								"text" : "3",

								"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
							},
							{
								"name" : "page_button3",
								"type" : "button",
								"x" : PAGE_WINDOW_WIDTH/2-PAGE_BUTTON_NUMBER_WIDTH/2 + PAGE_BUTTON_NUMBER_WIDTH + PAGE_BUTTON_NUMBER_GAP,
								"y" : 0,
								"vertical_align"	: "center",
								"text" : "4",

								"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
							},
							{
								"name" : "page_button4",
								"type" : "button",
								"x" : PAGE_WINDOW_WIDTH/2-PAGE_BUTTON_NUMBER_WIDTH/2 + PAGE_BUTTON_NUMBER_WIDTH*2 + PAGE_BUTTON_NUMBER_GAP*2,
								"y" : 0,
								"vertical_align"	: "center",
								"text" : "5",

								"default_image" : "d:/ymir work/ui/privatesearch/private_pagenumber_00.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_01.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_pagenumber_02.sub",
							},
							## 다음 버튼 10*10
							{
								"name" : "next_page_button", 
								"type" : "button",

								"x" : PAGE_WINDOW_WIDTH - PAGE_BUTTON_WIDTH - PAGE_BUTTON_WIDTH - PAGE_BUTTON_GAP ,
								"y" : 0,
								"vertical_align"	: "center",
								"default_image" : "d:/ymir work/ui/privatesearch/private_next_btn_01.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_next_btn_02.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_next_btn_01.sub",
							},
							## 다음 * 10 버튼 13*10
							{
								"name" : "last_next_page_button", 
								"type" : "button",

								"x" : PAGE_WINDOW_WIDTH - PAGE_BUTTON_WIDTH,
								"y" : 0,
								"vertical_align"	: "center",

								"default_image" : "d:/ymir work/ui/privatesearch/private_last_next_btn_01.sub",
								"over_image" 	: "d:/ymir work/ui/privatesearch/private_last_next_btn_02.sub",
								"down_image" 	: "d:/ymir work/ui/privatesearch/private_last_next_btn_01.sub",
							},
						],
					},
				],
			},
		],
	}