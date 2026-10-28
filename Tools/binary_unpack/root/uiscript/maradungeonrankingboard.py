import uiScriptLocale

ROOT_PATH			= "d:/ymir work/ui/game/mara_dungeon/ranking_board/"

WINDOW_WIDTH		= 526
WINDOW_HEIGHT		= 384

window = {
	"name"		: "MaraDungeonRankingWindow",
	"style"		: ("movable", "float", ),
	
	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,	

	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children"	:
	(
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			"style"		: ("attach", ),

			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"title"		: uiScriptLocale.MARA_DUNGEON_RANKING_TITLE,	# 마계던전 랭킹

			"children" :
			(				
				## Button
				{
					"name"					: "refresh_button",
					"type"					: "button",

					"x"						: 18,
					"y"						: 31,

					"default_image"			: ROOT_PATH + "refresh_button_default.sub",
					"over_image"			: ROOT_PATH + "refresh_button_over.sub",
					"down_image"			: ROOT_PATH + "refresh_button_down.sub",
				},

				# 랭킹 카테고리
				{
					"name"		: "select_ranking_category_window",
					"type"		: "window",

					"x"			: 269,
					"y"			: 34,

					"width"		: 225 + 16,
					"height"	: 16,
				},

				{
					"name"		: "bg_window",
					"type"		: "outline_window",
					"x"			: 10,
					"y"			: 57,
					"width"		: 504,
					"height"	: 280,

					"children" :
					(
						# 메뉴 바
						{
							"name"	: "menu_bar",
							"type"	: "image",
							"x"		: 12 - 10,
							"y"		: 57 - 57,
							"image"	: ROOT_PATH + "menu_bar.sub",

							"children" :
							(
								# 순위
								{
									"name"		: "rank_text_window",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 25 - 10,
									"y"			: 64 - 57,
									"width"		: 21,
									"height"	: 11,
			
									"children"	:
									(
										{
											"name"		: "rank_text",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: uiScriptLocale.MARA_DUNGEON_RANKING_TEXT_RANK,	# 순위
										},
									),
								},

								# 이름
								{
									"name"		: "name_text_window",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 92 - 10,
									"y"			: 64 - 57,
									"width"		: 52,
									"height"	: 11,
			
									"children"	:
									(
										{
											"name"		: "name_text",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: uiScriptLocale.MARA_DUNGEON_RANKING_TEXT_NAME,	# 이름
										},
									),
								},

								# 장비 아이콘
								{
									"name"	: "show_equip_icon",
									"type"	: "image",
									"x"		: 189 - 10,
									"y"		: 60 - 57,
									"image"	: ROOT_PATH + "equip_icon.sub",
								},

								# 성마평점
								{
									"name"		: "sungma_score_text_window",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 233 - 10,
									"y"			: 64 - 57,
									"width"		: 23,
									"height"	: 11,
			
									"children"	:
									(
										{
											"name"		: "sungma_score_text",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: "", # uiScriptLocale.MARA_DUNGEON_RANKING_TEXT_SUNGMA_SCORE,	# 성마평점
										},
									),
								},

								# 단계
								{
									"name"		: "stage_text_window",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 288 - 10,
									"y"			: 64 - 57,
									"width"		: 21,
									"height"	: 11,
			
									"children"	:
									(
										{
											"name"		: "stage_text",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: uiScriptLocale.MARA_DUNGEON_RANKING_TEXT_STAGE,	# 단계
										},
									),
								},

								# 클리어시간
								{
									"name"		: "clear_time_text_window",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 340 - 10,
									"y"			: 64 - 57,
									"width"		: 24,
									"height"	: 11,
			
									"children"	:
									(
										{
											"name"		: "clear_time_text",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: uiScriptLocale.MARA_DUNGEON_RANKING_TEXT_CLEAR_TIME,	# 시간
										},
									),
								},

								# 갱신 시간
								{
									"name"		: "update_time_text_window",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 418 - 10,
									"y"			: 64 - 57,
									"width"		: 53,
									"height"	: 11,
			
									"children"	:
									(
										{
											"name"		: "update_time_text",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: uiScriptLocale.MARA_DUNGEON_RANKING_TEXT_UPDATE_TIME,	# 갱신 시간
										},
									),
								},
							),
						},

						# 랭킹 목록
						{
							"name"		: "ranking_list",
							"type"		: "listboxex",
							"style"		: ("attach", ),
							"x"			: 15 - 10,
							"y"			: 84 - 57,
							"width"		: 494,
							"height"	: 246,
						},
					),
				},

				# 내 랭킹
				{
					"name"		: "bg_window_my_rank",
					"type"		: "outline_window",
					"x"			: 10,
					"y"			: 340,
					"width"		: 504,
					"height"	: 31,

					"children" :
					(
						# 메뉴 바
						{
							"name"	: "rank_node_my",
							"type"	: "image",
							"x"		: 15 - 10,
							"y"		: 346 - 340,
							"image"	: ROOT_PATH + "rank_node_my.sub",

							"children" :
							(
								# 순위
								{
									"name"		: "rank_text_window_my",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 7,
									"y"			: 2,
									"width"		: 30,
									"height"	: 16,
			
									"children"	:
									(
										{
											"name"		: "rank_text_my",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: "9999",
										},
									),
								},

								# 이름
								{
									"name"		: "name_text_window_my",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 46,
									"y"			: 2,
									"width"		: 113,
									"height"	: 16,
			
									"children"	:
									(
										{
											"name"		: "name_text_my",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: "my_name",
										},
									),
								},

								# 장비 보기 버튼
								{
									"name"	: "show_equip_button_my",
									"type"	: "button",
									"x"		: 176,
									"y"		: 2,
									
									"default_image"		: ROOT_PATH + "show_equip_button_default.sub",
									"over_image"		: ROOT_PATH + "show_equip_button_over.sub",
									"down_image"		: ROOT_PATH + "show_equip_button_down.sub",
								},

								# 성마평점
								{
									"name"		: "sungma_score_text_window_my",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 207,
									"y"			: 2,
									"width"		: 44,
									"height"	: 16,
			
									"children"	:
									(
										{
											"name"		: "sungma_score_text_my",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: "",
										},
									),
								},

								# 단계
								{
									"name"		: "stage_text_window_my",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 265,
									"y"			: 2,
									"width"		: 38,
									"height"	: 16,
			
									"children"	:
									(
										{
											"name"		: "stage_text_my",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: "99",	# 단계
										},
									),
								},

								# 클리어시간
								{
									"name"		: "clear_time_text_window_my",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 316,
									"y"			: 2,
									"width"		: 39,
									"height"	: 16,
			
									"children"	:
									(
										{
											"name"		: "clear_time_text_my",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: "99:99",
										},
									),
								},

								# 갱신시간
								{
									"name"		: "update_time_text_window_my",
									"type"		: "window",
									"style"		: ("attach", "not_pick", ),
									"x"			: 364,
									"y"			: 2,
									"width"		: 123,
									"height"	: 16,
			
									"children"	:
									(
										{
											"name"		: "update_time_text_my",
											"type"		: "text",
											"x"			: 0,
											"y"			: 0,
											"all_align" : "center",
											"text"		: "9999/99/99 99:99:99",
										},
									),
								},
							),
						},
					),
				},
			),				
		},
	),
}
