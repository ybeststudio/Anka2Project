import uiScriptLocale
import localeInfo
import app

# 리소스 경로 폴더
PUBLIC_PATH										= "d:/ymir work/ui/public/"
PATTERN_PATH									= "d:/ymir work/ui/pattern/"
ROOT_PATH										= "d:/ymir work/ui/minigame/snowflake_stick/"

# 전체 UI 윈도우 크기
WINDOW_WIDTH									= 296
WINDOW_HEIGHT									= 290

# 설명 페이지의 설명 배경 크기
PAGE_DESCRIPTION_WINDOW_BACKGROUND_WIDTH		= 274
PAGE_DESCRIPTION_WINDOW_BACKGROUND_HEIGHT		= 220

# 보상교환 페이지의 배경 크기
PAGE_EXCHANGE_REWARD_WINDOW_BACKGROUND_WIDTH	= 274
PAGE_EXCHANGE_REWARD_WINDOW_BACKGROUND_HEIGHT	= 246

# 슬롯 크기
SLOT_WIDTH										= 32
SLOT_HEIGHT										= 32

window = {
	"name"		: "snowflake_stick_event_window",
	"style"		: ("movable", "float", ),

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
			
			# 로카 : 눈꽃 막대 이벤트
			"title"		: uiScriptLocale.SNOWFLAKE_STICK_EVENT_TITLE,

			"children" :
			(
				# (page1) 설명 페이지 윈도우
				{
					"name"		: "page_desc_window",
					"type"		: "window",
					"x"			: 0,
					"y"			: 0,
					"width"		: WINDOW_WIDTH,
					"height"	: WINDOW_HEIGHT,
					"style"		: ("not_pick", ),

					"children"	:
					(
						{
							"name"		: "desc_window_background",
							"type"		: "outline_window",
							"x"			: 10,
							"y"			: 32,
							"width"		: PAGE_DESCRIPTION_WINDOW_BACKGROUND_WIDTH,
							"height"	: PAGE_DESCRIPTION_WINDOW_BACKGROUND_HEIGHT,
						},

						{
							"name"			: "exchange_reward_button",
							"type"			: "button",
							"x"				: 103,
							"y"				: 257,
							# 로카 : 보상교환
							"text"			: uiScriptLocale.SNOWFLAKE_STICK_EVENT_EXCHANGE_REWARD_BUTTON_TEXT,
							"default_image" : PUBLIC_PATH + "large_button_01.sub",
							"over_image"	: PUBLIC_PATH + "large_button_02.sub",
							"down_image"	: PUBLIC_PATH + "large_button_03.sub",
						},
					),
				},

				# (page2) 보상 교환 페이지 윈도우
				{
					"name"		: "page_exchange_reward_window",
					"type"		: "window",
					"x"			: 0,
					"y"			: 0,
					"width"		: WINDOW_WIDTH,
					"height"	: WINDOW_HEIGHT,
					"style"		: ("not_pick", ),

					"children"	:
					(
						# 배경
						{
							"name"		: "exchange_reward_window_background",
							"type"		: "outline_window",
							"x"			: 10,
							"y"			: 32,
							"width"		: PAGE_EXCHANGE_REWARD_WINDOW_BACKGROUND_WIDTH,
							"height"	: PAGE_EXCHANGE_REWARD_WINDOW_BACKGROUND_HEIGHT,
						},

						# 하위 제목줄 1
						{
							"name"	: "exchange_sub_title_background_1",
							"type"	: "image",
							"x"		: 12,
							"y"		: 32,			
							"image"	: ROOT_PATH + "sub_title_bg_img.sub",

							"children"	:
							(
								{
									"name"		: "exchange_sub_title_1_text",
									"type"		: "text",
									"x"			: 0,
									"y"			: 0,
									"all_align"	: "center",
									# 로카 : 눈꽃 막대 교환
									"text"		: uiScriptLocale.SNOWFLAKE_STICK_EVENT_EXCHANGE_PAGE_SUBTITLE_1,
								},
							),
						},

						# 하위 제목줄 2
						{
							"name"	: "exchange_sub_title_background_2",
							"type"	: "image",
							"x"		: 12,
							"y"		: 139,			
							"image"	: ROOT_PATH + "sub_title_bg_img.sub",

							"children"	:
							(
								{
									"name"		: "exchange_sub_title_2_text",
									"type"		: "text",
									"x"			: 0,
									"y"			: 0,
									"all_align"	: "center",
									# 로카 : 이벤트 기간 내 3회 교환 가능!
									"text"		: uiScriptLocale.SNOWFLAKE_STICK_EVENT_EXCHANGE_PAGE_SUBTITLE_2,
								},
							),
						},

						# 눈꽃 막대 교환 윈도우
						{
							"name"		: "exchange_stick_window",
							"type"		: "window",
							"x"			: 12,
							"y"			: 54,
							"width"		: 270,
							"height"	: 77,

							"children"	:
							(
								# 2칸짜리 빈슬롯
								{
									"name"		: "2slot_background_img",
									"type"		: "image",
									"x"			: 20 - 12,
									"y"			: 59 - 54,
									"width"		: 42,
									"height"	: 74,
									"image"		: ROOT_PATH + "empty_2slot_img.sub",
								},

								# 눈덩이 조각 아이콘 이미지
								{
									"name"		: "snow_ball_img",
									"type"		: "image",
									"x"			: 25 - 12,
									"y"			: 64 - 54,
									"width"		: 32,
									"height"	: 32,
									"image"		: ROOT_PATH + "snow_ball_img.sub",
								},

								# 눈덩이 조각 개수
								{
									"name"		: "snow_ball_count_bg_img",
									"type"		: "image",
									"x"			: 61 - 12,
									"y"			: 70 - 54,
									"width"		: 33,
									"height"	: 18,
									"image"		: ROOT_PATH + "count_text_bg_img.sub",

									"children"	:
									(
										{
											"name"		: "snow_ball_count_text",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align"	: "center",
											"text"		: "",
										},
									),
								},

								# 나무가지 조각 아이콘 이미지
								{
									"name"		: "tree_branch_img",
									"type"		: "image",
									"x"			: 25 - 12,
									"y"			: 96 - 54,
									"width"		: 32,
									"height"	: 32,
									"image"		: ROOT_PATH + "tree_branch_img.sub",
								},

								# 나무가지 조각 개수
								{
									"name"		: "tree_branch_count_bg_img",
									"type"		: "image",
									"x"			: 61 - 12,
									"y"			: 102 - 54,
									"width"		: 33,
									"height"	: 18,
									"image"		: ROOT_PATH + "count_text_bg_img.sub",

									"children"	:
									(
										{
											"name"		: "tree_branch_count_text",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align"	: "center",
											"text"		: "",
										},
									),
								},

								# 오른쪽 방향 화살표 이미지
								{
									"name"		: "right_arrow_img",
									"type"		: "image",
									"x"			: 116 - 12,
									"y"			: 70 - 54,
									"width"		: 46,
									"height"	: 50,
									"image"		: ROOT_PATH + "right_arrow_img.sub",
								},

								# 눈꽃막대 슬롯
								{
									"name"		: "snowflake_stick_slot_bg_img",
									"type"		: "image",
									"x"			: 180 - 12,
									"y"			: 59 - 54,
									"width"		: 42,
									"height"	: 42,
									"image"		: ROOT_PATH + "empty_slot_img.sub",

									"children"	:
									(
										{
											"name"		: "snowflake_stick_slot",
											"type"		: "slot",
											"x"			: 5,
											"y"			: 5,
											"width"		: SLOT_WIDTH,
											"height"	: SLOT_HEIGHT,

											"slot" : ( { "index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT, }, ),
										},
									),
								},

								# 눈꽃막대 교환가능 개수
								{
									"name"		: "snowflake_stick_exchange_count_bg_img",
									"type"		: "image",
									"x"			: 221 - 12,
									"y"			: 70 - 54,
									"width"		: 33,
									"height"	: 18,
									"image"		: ROOT_PATH + "count_text_bg_img.sub",

									"children"	:
									(
										{
											"name"		: "snowflake_stick_exchange_count_text",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align"	: "center",
											"text"		: "",
										},
									),
								},

								# 눈꽃 막대 교환버튼
								{
									"name"			: "snowflake_stick_exchange_button",
									"type"			: "button",
									"x"				: 184 - 12,
									"y"				: 106 - 54,
									# 로카 : 교환하기
									"text"			: uiScriptLocale.SNOWFLAKE_STICK_EVENT_EXCHANGE_BUTTON_TEXT,
									"default_image" : PUBLIC_PATH + "large_button_01.sub",
									"over_image"	: PUBLIC_PATH + "large_button_02.sub",
									"down_image"	: PUBLIC_PATH + "large_button_03.sub",
								},

								# 물음표
								{
									"name"			: "help_button",
									"type"			: "button",
									"x"				: 259 - 12,
									"y"				: 71 - 54,
									"default_image"	: PATTERN_PATH + "q_mark_01.tga",
									"over_image"	: PATTERN_PATH + "q_mark_02.tga",
									"down_image"	: PATTERN_PATH + "q_mark_01.tga",
								},					

							),
						},

						# 펫 교환 윈도우
						{
							"name"		: "exchange_pet_bg_img",
							"type"		: "image",
							"x"			: 16,
							"y"			: 164,
							"width"		: 262,
							"height"	: 52,
							"image"		: ROOT_PATH + "exchange_mout_pet_bg_img.sub",

							"children"	:
							(
								# 펫 아이템 슬롯
								{
									"name"		: "reward_pet_slot",
									"type"		: "slot",
									"x"			: 117,
									"y"			: 10,
									"width"		: SLOT_WIDTH,
									"height"	: SLOT_HEIGHT,

									"slot" : ( { "index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT, }, ),
								},

								# 펫 교환시 필요한 눈꽃막대 아이템 슬롯
								{
									"name"		: "reward_pet_stick_slot",
									"type"		: "slot",
									"x"			: 10,
									"y"			: 10,
									"width"		: SLOT_WIDTH,
									"height"	: SLOT_HEIGHT,

									"slot" : ( { "index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT, }, ),
								},

								# 펫 교환 윈도우 오른쪽 화살표
								{
									"name"		: "reward_pet_right_arrow",
									"type"		: "window",
									"x"			: 70,
									"y"			: 11,
									"width"		: 25,
									"height"	: 30,
								},

								# 펫 교환 가능 횟수
								{
									"name"		: "reward_pet_exchange_count_window",
									"type"		: "window",
									"x"			: 194,
									"y"			: 5,
									"width"		: 33,
									"height"	: 18,

									"children"	:
									(
										{
											"name"		: "reward_pet_exchange_count_text",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align"	: "center",
											"text"		: "",
										},
									),
								},

								# 펫 교환버튼
								{
									"name"			: "reward_pet_exchange_button",
									"type"			: "button",
									"x"				: 168,
									"y"				: 27,
									# 로카 : 교환하기
									"text"			: uiScriptLocale.SNOWFLAKE_STICK_EVENT_EXCHANGE_BUTTON_TEXT,
									"default_image" : PUBLIC_PATH + "large_button_01.sub",
									"over_image"	: PUBLIC_PATH + "large_button_02.sub",
									"down_image"	: PUBLIC_PATH + "large_button_03.sub",
								},
							),
						},

						# 마운트 교환 윈도우
						{
							"name"		: "exchange_mount_bg_img",
							"type"		: "image",
							"x"			: 16,
							"y"			: 220,
							"width"		: 262,
							"height"	: 52,
							"image"		: ROOT_PATH + "exchange_mout_pet_bg_img.sub",

							"children"	:
							(
								# 마운트 아이템 슬롯
								{
									"name"		: "reward_mount_slot",
									"type"		: "slot",
									"x"			: 117,
									"y"			: 10,
									"width"		: SLOT_WIDTH,
									"height"	: SLOT_HEIGHT,

									"slot" : ( { "index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT, }, ),
								},

								# 마운트 교환시 필요한 눈꽃막대 아이템 슬롯
								{
									"name"		: "reward_mount_stick_slot",
									"type"		: "slot",
									"x"			: 10,
									"y"			: 10,
									"width"		: SLOT_WIDTH,
									"height"	: SLOT_HEIGHT,

									"slot" : ( { "index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT, }, ),
								},

								# 마운트 교환 윈도우 오른쪽 화살표
								{
									"name"		: "reward_mount_right_arrow",
									"type"		: "window",
									"x"			: 70,
									"y"			: 11,
									"width"		: 25,
									"height"	: 30,
								},

								# 마운트 교환 가능 횟수
								{
									"name"		: "reward_mount_exchange_count_window",
									"type"		: "window",
									"x"			: 194,
									"y"			: 5,
									"width"		: 33,
									"height"	: 18,

									"children"	:
									(
										{
											"name"		: "reward_mount_exchange_count_text",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align"	: "center",
											"text"		: "",
										},
									),
								},

								# 마운트 교환버튼
								{
									"name"			: "reward_mount_exchange_button",
									"type"			: "button",
									"x"				: 168,
									"y"				: 27,
									# 로카 : 교환하기
									"text"			: uiScriptLocale.SNOWFLAKE_STICK_EVENT_EXCHANGE_BUTTON_TEXT,
									"default_image" : PUBLIC_PATH + "large_button_01.sub",
									"over_image"	: PUBLIC_PATH + "large_button_02.sub",
									"down_image"	: PUBLIC_PATH + "large_button_03.sub",
								},
							),
						},
					),
				},
			),
		},
	),
}