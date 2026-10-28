import localeInfo

PUBLIC_PATH					= "d:/ymir work/ui/public/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"
ROOT_PATH					= "d:/ymir work/ui/event/"
SPORTS_MATCH_EVENT_PATH		= "d:/ymir work/ui/minigame/sports_match/"

WINDOW_WIDTH				= 296
WINDOW_HEIGHT				= 334

MAIN_WINDOW_WIDTH			= 274
MAIN_WINDOW_HEIGHT			= 262

MAIN_WINDOW_PATTERN_X_COUNT	= (MAIN_WINDOW_WIDTH - 32) / 16
MAIN_WINDOW_PATTERN_Y_COUNT	= (MAIN_WINDOW_HEIGHT - 32) / 16

SLOT_IMAGE_WIDTH			= 48
SLOT_IMAGE_HEIGHT			= 48
SLOT_WIDTH					= 33
SLOT_HEIGHT					= 33

DESC_WINDOW_WIDTH			= 345
DESC_WINDOW_HEIGHT			= 190

MIDDLE_TITLE_WIDTH			= 274
MIDDLE_TITLE_HEIGHT			= 19

EXCHANGE_WINDOW_HEIGHT		= 56

START_POS_X					= 10
START_POS_Y					= 32
if localeInfo.IsARABIC():
	TICKET_PIECE_IMG_POS_X		= MIDDLE_TITLE_WIDTH - SLOT_WIDTH - 15
	TICKET_PIECE_COUNT_POS_X	= MIDDLE_TITLE_WIDTH - SLOT_WIDTH - 48
	TICKET_IMG_POS_X			= MIDDLE_TITLE_WIDTH - SLOT_WIDTH - 95
	TICKET_COUNT_POS_X			= MIDDLE_TITLE_WIDTH - SLOT_WIDTH - 127
	TICKET_USE_BUTTON_POS_X		= 15
	EXCHANGE_REWARD_SLOT_POS_X	= MIDDLE_TITLE_WIDTH - SLOT_WIDTH - 21

else:
	TICKET_PIECE_IMG_POS_X		= 15
	TICKET_PIECE_COUNT_POS_X	= 47
	TICKET_IMG_POS_X			= 95
	TICKET_COUNT_POS_X			= 127
	TICKET_USE_BUTTON_POS_X		= 174
	EXCHANGE_REWARD_SLOT_POS_X	= 10


window = {
	"name"		: "SportsMatchEventWindow",
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
			
			## 뉴월드컵 이벤트
			"title"		: localeInfo.SPORTS_MATCH_WORLDCUP_EVENT_UI_TITLE,

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
							
							"x" : MAIN_WINDOW_WIDTH - 8,
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
							"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT+0.1, 0),
						},
					],
				},
				# ----------------------------------------------------------------------
				# 보유 갯수 타이틀
				{
					"name"		: "item_flag_title_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: START_POS_X,
					"y"			: START_POS_Y,

					"width"		: MIDDLE_TITLE_WIDTH,
					"height"	: MIDDLE_TITLE_HEIGHT,

					"children" :	
					[
						# 보유 갯수
						{
							"name"		: "item_flag_info_title_img",
							"type"		: "image",
							
							"x"			: 2,
							"y"			: 0,
							
							"width"		: MIDDLE_TITLE_WIDTH,
							"height"	: MIDDLE_TITLE_HEIGHT,
							
							"image"		: SPORTS_MATCH_EVENT_PATH + "itemflag_info_title_img.sub",

							"children"  :
							[
								{
									"name"		: "item_flag_info_title_text",
									"type"		: "text",

									"x"			: 0,
									"y"			: 0,
	
									"text"					: localeInfo.SPORTS_MATCH_UI_ITEM_FLAG_COUNT,
									
									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",
									"vertical_align"		: "center",
									"text_vertical_align"	: "center",
									
									
								},
							],
						},

					],
				},

				# 보유 갯수 영역
				{
					"name"		: "item_flag_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: START_POS_X,
					"y"			: START_POS_Y + MIDDLE_TITLE_HEIGHT+2,

					"width"		: MIDDLE_TITLE_WIDTH,
					"height"	: 48,

					"children" :
					[
						# 티켓 조각 배경 > 축구공 조각 이미지
						{
							"name"					: "item_ticket_piece_bg",
							"type"					: "image",
							"x"						: TICKET_PIECE_IMG_POS_X,
							"y"						: 0,
							
							"width"					: SLOT_WIDTH,
							"height"				: SLOT_HEIGHT,
							"vertical_align"		: "center",
							"image"					: SPORTS_MATCH_EVENT_PATH + "itemflag_bg.sub",

							"children"  :
							[
								{
									"name"				: "ticket_piece_icon",
									"type"				: "image",

									"x"					: 0,
									"y"					: 0,
							
									"image"				: SPORTS_MATCH_EVENT_PATH + "itemflag_piece.sub",
									
									"horizontal_align"	: "center",
									"vertical_align"	: "center",
									
								},
							],
						},

						# 티켓 조각 갯수 배경 > 축구공 조각 갯수 text
						{
							"name"					: "item_ticket_piece_count_bg",
							"type"					: "image",
							"x"						: TICKET_PIECE_COUNT_POS_X,
							"y"						: 0,
							
							"width"					: SLOT_WIDTH,
							"height"				: MIDDLE_TITLE_HEIGHT,
							"vertical_align"		: "center",
							"image"					: SPORTS_MATCH_EVENT_PATH + "itemflag_count_bg.sub",

							"children"  :
							[
								{
									"name"				: "ticket_piece_count_text",
									"type"				: "text",

									"x"					: 0,
									"y"					: 0,
							
									"text"				: "0",
									
									
									"horizontal_align"		: "center",
									"vertical_align"		: "center",
									"text_horizontal_align"	: "center",
									"text_vertical_align"	: "center",
									
								},
							],
						},

						# 티켓 배경 > 축구공 이미지
						{
							"name"				: "item_ticket_bg",
							"type"				: "image",
							"x"					: TICKET_IMG_POS_X,
							"y"					: 0,
							
							"width"				: SLOT_WIDTH,
							"height"			: SLOT_HEIGHT,
							
							"image"				: SPORTS_MATCH_EVENT_PATH + "itemflag_bg.sub",
							"vertical_align"	: "center",

							"children"  :
							[
								{
									"name"				: "ticket_icon",
									"type"				: "image",

									"x"					: 0,
									"y"					: 0,
							
									"image"				: SPORTS_MATCH_EVENT_PATH + "itemflag_result.sub",
									
									"horizontal_align"	: "center",
									"vertical_align"	: "center",
									
								},
							],
						},

						# 티켓 갯수 배경 > 티켓 갯수 text
						{
							"name"					: "item_ticket_count_bg",
							"type"					: "image",
							"x"						: TICKET_COUNT_POS_X,
							"y"						: 0,
							
							"width"					: 33,
							"height"				: 18,
							"vertical_align"		: "center",
							"image"					: SPORTS_MATCH_EVENT_PATH + "itemflag_count_bg.sub",

							"children"  :
							[
								{
									"name"				: "ticket_count_text",
									"type"				: "text",

									"x"					: 0,
									"y"					: 0,
							
									"text"				: "0",
									
									
									"horizontal_align"		: "center",
									"vertical_align"		: "center",
									"text_horizontal_align"	: "center",
									"text_vertical_align"	: "center",
									
								},
							],
						},

						# 사용하기 버튼
						{
							"name"					: "ticket_use_btn",
							"type"					: "button",
							"x"						: TICKET_USE_BUTTON_POS_X,
							"y"						: 0,
							"vertical_align"		: "center",
							"text"					: localeInfo.SPORTS_MATCH_UI_ITEM_USE_BUTTON,		# 사용하기
							"default_image"			: PUBLIC_PATH + "large_button_01.sub",
							"over_image"			: PUBLIC_PATH + "large_button_02.sub",
							"down_image"			: PUBLIC_PATH + "large_button_03.sub",
						},

					],
				},
				# ----------------------------------------------------------------------
				# 교환 목록 타이틀
				{
					"name"		: "item_flag_title_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: START_POS_X,
					"y"			: START_POS_Y + MIDDLE_TITLE_HEIGHT + 48,

					"width"		: MIDDLE_TITLE_WIDTH,
					"height"	: MIDDLE_TITLE_HEIGHT,

					"children" :	
					[
						{
							"name"		: "item_flag_info_title_img",
							"type"		: "image",
							
							"x"			: 2,
							"y"			: 0,
							
							"width"		: MIDDLE_TITLE_WIDTH,
							"height"	: MIDDLE_TITLE_HEIGHT,
							
							"image"		: SPORTS_MATCH_EVENT_PATH + "itemflag_info_title_img.sub",

							"children"  :
							[
								{
									"name"					: "item_flag_info_title_text",
									"type"					: "text",

									"x"						: 0,
									"y"						: 0,
	
									"text"					: localeInfo.SPORTS_MATCH_UI_ITEM_FLAG_EXCHANGE_LIST,
									
									"horizontal_align"		: "center",
									"text_horizontal_align"	: "center",
									"vertical_align"		: "center",
									"text_vertical_align"	: "center",
								},
							],
						},

					],
				},
				
				# 교환창 영역
				{
					"name"		: "item_exchange_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: START_POS_X,
					"y"			: START_POS_Y + MIDDLE_TITLE_HEIGHT + 50 + MIDDLE_TITLE_HEIGHT,

					"width"		: MIDDLE_TITLE_WIDTH,
					"height"	: 171,

					"children" :
					[
						# 0번째 교환 영역
						{
							"name"					: "item_exchange_bg_0",
							"type"					: "image",
							
							"x"						: 0,
							"y"						: 5,

							"width"					: MIDDLE_TITLE_WIDTH,
							"height"				: EXCHANGE_WINDOW_HEIGHT,

							"horizontal_align"		: "center",
							"image"					: SPORTS_MATCH_EVENT_PATH + "exchange_bg.sub",

							"children" :
							[
								# 보상 아이템 슬롯 
								{
									"name"				: "exchange_reward_slot_0",
									"type"				: "slot",

									"x"					: EXCHANGE_REWARD_SLOT_POS_X,
									"y"					: 10,
									
									"width"				: SLOT_WIDTH,
									"height"			: SLOT_HEIGHT,
									"slot" : 
									(
										{"index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
									),
									
								},

								# 티켓 슬롯 
								{
									"name"				: "exchange_ticket_icon_0",
									"type"				: "image",

									"x"					: 89,
									"y"					: 10,
							
									"image"				: SPORTS_MATCH_EVENT_PATH + "itemflag_result.sub",
								},

								# 티켓 갯수 text
								# 티켓 갯수 배경 > 티켓 갯수 text
								{
									"name"					: "exchange_ticket_count_window_0",
									"type"					: "window",
									"x"						: 121,
									"y"						: 0,
									
									"width"					: SLOT_WIDTH,
									"height"				: MIDDLE_TITLE_HEIGHT,
									"vertical_align"		: "center",
									"image"					: SPORTS_MATCH_EVENT_PATH + "itemflag_count_bg.sub",

									"children"  :
									[
										{
											"name"				: "exchange_ticket_count_0",
											"type"				: "text",

											"x"					: 0,
											"y"					: 0,
									
											"text"				: "0",
											
											
											"horizontal_align"		: "center",
											"vertical_align"		: "center",
											"text_horizontal_align"	: "center",
											"text_vertical_align"	: "center",
											
										},
									],
								},

								# 교환하기 버튼
								{
									"name"					: "exchange_button_0",
									"type"					: "button",
									"x"						: 168,
									"y"						: 0,
									"vertical_align"		: "center",
									"text"					: localeInfo.SPORTS_MATCH_UI_ITEM_EXCHANGE_BUTTON,
									"default_image"			: PUBLIC_PATH + "large_button_01.sub",
									"over_image"			: PUBLIC_PATH + "large_button_02.sub",
									"down_image"			: PUBLIC_PATH + "large_button_03.sub",
									"disable_image"			: PUBLIC_PATH + "large_button_03.sub",
								},
							],
						},

						# 1번째 교환 영역
						{
							"name"					: "item_exchange_bg_1",
							"type"					: "image",
							
							"x"						: 0,
							"y"						: 1*EXCHANGE_WINDOW_HEIGHT + 3,

							"width"					: MIDDLE_TITLE_WIDTH,
							"height"				: EXCHANGE_WINDOW_HEIGHT,

							"horizontal_align"		: "center",
							"image"					: SPORTS_MATCH_EVENT_PATH + "exchange_bg.sub",

							"children" :
							[
								# 보상 아이템 슬롯 
								{
									"name"				: "exchange_reward_slot_1",
									"type"				: "slot",

									"x"					: EXCHANGE_REWARD_SLOT_POS_X,
									"y"					: 10,
									
									"width"				: SLOT_WIDTH,
									"height"			: SLOT_HEIGHT,
									"slot" : 
									(
										{"index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
									),
									
								},

								# 축구공 슬롯 
								{
									"name"				: "exchange_ticket_icon_1",
									"type"				: "image",

									"x"					: 89,
									"y"					: 10,
							
									"image"				: SPORTS_MATCH_EVENT_PATH + "itemflag_result.sub",
									
								},

								# 축구공 갯수 text
								{
									"name"					: "exchange_ticket_count_window_1",
									"type"					: "window",
									"x"						: 121,
									"y"						: 0,
									
									"width"					: SLOT_WIDTH,
									"height"				: MIDDLE_TITLE_HEIGHT,
									"vertical_align"		: "center",
									"image"					: SPORTS_MATCH_EVENT_PATH + "itemflag_count_bg.sub",

									"children"  :
									[
										{
											"name"				: "exchange_ticket_count_1",
											"type"				: "text",

											"x"					: 0,
											"y"					: 0,
									
											"text"				: "0",
											
											
											"horizontal_align"		: "center",
											"vertical_align"		: "center",
											"text_horizontal_align"	: "center",
											"text_vertical_align"	: "center",
											
										},
									],
								},

								# 교환하기 버튼
								{
									"name"					: "exchange_button_1",
									"type"					: "button",
									"x"						: 168,
									"y"						: 0,
									"vertical_align"		: "center",
									"text"					: localeInfo.SPORTS_MATCH_UI_ITEM_EXCHANGE_BUTTON,
									"default_image"			: PUBLIC_PATH + "large_button_01.sub",
									"over_image"			: PUBLIC_PATH + "large_button_02.sub",
									"down_image"			: PUBLIC_PATH + "large_button_03.sub",
									"disable_image"			: PUBLIC_PATH + "large_button_03.sub",
								},
							],
						},

						# 2번째 교환 영역
						{
							"name"					: "item_exchange_bg_2",
							"type"					: "image",
							
							"x"						: 0,
							"y"						: 2*EXCHANGE_WINDOW_HEIGHT + 3,

							"width"					: MIDDLE_TITLE_WIDTH,
							"height"				: EXCHANGE_WINDOW_HEIGHT,

							"horizontal_align"		: "center",
							"image"					: SPORTS_MATCH_EVENT_PATH + "exchange_bg.sub",

							"children" :
							[
								# 보상 아이템 슬롯 
								{
									"name"				: "exchange_reward_slot_2",
									"type"				: "slot",

									"x"					: EXCHANGE_REWARD_SLOT_POS_X,
									"y"					: 10,
									
									"width"				: SLOT_WIDTH,
									"height"			: SLOT_HEIGHT,
									"slot" : 
									(
										{"index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
									),
									
								},

								# 축구공 슬롯 
								{
									"name"				: "exchange_ticket_icon_2",
									"type"				: "image",

									"x"					: 89,
									"y"					: 10,
							
									"image"				: SPORTS_MATCH_EVENT_PATH + "itemflag_result.sub",
									
								},

								# 축구공 갯수 text
								{
									"name"					: "exchange_ticket_count_window_2",
									"type"					: "window",
									"x"						: 121,
									"y"						: 0,
									
									"width"					: SLOT_WIDTH,
									"height"				: MIDDLE_TITLE_HEIGHT,
									"vertical_align"		: "center",
									"image"					: SPORTS_MATCH_EVENT_PATH + "itemflag_count_bg.sub",

									"children"  :
									[
										{
											"name"				: "exchange_ticket_count_2",
											"type"				: "text",

											"x"					: 0,
											"y"					: 0,
									
											"text"				: "0",
											
											
											"horizontal_align"		: "center",
											"vertical_align"		: "center",
											"text_horizontal_align"	: "center",
											"text_vertical_align"	: "center",
											
										},
									],
								},

								# 교환하기 버튼
								{
									"name"					: "exchange_button_2",
									"type"					: "button",
									"x"						: 168,
									"y"						: 0,
									"vertical_align"		: "center",
									"text"					: localeInfo.SPORTS_MATCH_UI_ITEM_EXCHANGE_BUTTON,
									"default_image"			: PUBLIC_PATH + "large_button_01.sub",
									"over_image"			: PUBLIC_PATH + "large_button_02.sub",
									"down_image"			: PUBLIC_PATH + "large_button_03.sub",
									"disable_image"			: PUBLIC_PATH + "large_button_03.sub",
								},
							],
						},

					],
				},

				# 승부예측 버튼
				{
					"name"					: "match_predict_btn",
					"type"					: "button",
					"x"						: 184,
					"y"						: WINDOW_HEIGHT - 35,
					"text"					: localeInfo.SPORTS_MATCH_UI_MATCH_PREDICT_BUTTON,				# 승부예측
					"default_image"			: PUBLIC_PATH + "large_button_01.sub",
					"over_image"			: PUBLIC_PATH + "large_button_02.sub",
					"down_image"			: PUBLIC_PATH + "large_button_03.sub",
				},
			],
		},
	],
}