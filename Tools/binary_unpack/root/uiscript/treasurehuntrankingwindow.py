import uiScriptLocale
import localeInfo

PUBLIC_PATH						= "d:/ymir work/ui/public/"
PATTERN_PATH					= "d:/ymir work/ui/pattern/"
ROOT_PATH						= "d:/ymir work/ui/game/treasure_hunt/event/ranking/"
WINDOW_WIDTH					= 279
WINDOW_HEIGHT					= 354
OUTLINE_WIDTH					= 258
OUTLINE_HEIGHT					= 310

window = {
	"name"		: "treasure_hunt_event_reward_list_window",
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
			
			# 보물찾기 랭킹
			"title"		: uiScriptLocale.TREASURE_HUNT_EVENT_RANKING_TITLE,

			"children" :
			(
				# 테두리
				{
					"name"		: "outline_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					"x"			: 0,
					"y"			: 0,
					"width"		: WINDOW_WIDTH,
					"height"	: WINDOW_HEIGHT,
					"style"		: ("not_pick", ),

					"children"	:
					(
						{
							"name"		: "outline_bg",
							"type"		: "outline_window",
							"x"			: 10,
							"y"			: 32,
							"width"		: OUTLINE_WIDTH,
							"height"	: OUTLINE_HEIGHT,
						},
					),
				},
				# 랭킹 타이틀 이미지
				{
					"name" : "ranking_menu_bg",
					"type" : "image",

					"x" : 13,
					"y" : 34,
							
					"image"	: ROOT_PATH + "menu_bg.sub",
				},
				# 순위
				{
					"name"		: "rank_text_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 36,
					"y"			: 39,

					"width"		: 22,
					"height"	: 11,

					"children" :
					(
						{
							"name" : "rank",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align"		: "center",
							"text_horizontal_align" : "center",

							"vertical_align"		: "center",
							"text_vertical_align"	: "center",

							# 순위
							"text" : uiScriptLocale.TREASURE_HUNT_EVENT_RANKING_RANK,
						},
					),
				},
				# 이름
				{
					"name"		: "name_text_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 122,
					"y"			: 39,

					"width"		: 22,
					"height"	: 11,

					"children" :
					(
						{
							"name" : "name",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align"		: "center",
							"text_horizontal_align" : "center",

							"vertical_align"		: "center",
							"text_vertical_align"	: "center",

							# 이름
							"text" : uiScriptLocale.TREASURE_HUNT_EVENT_RANKING_NAME,
						},
					),
				},
				# 누적횟수
				{
					"name"		: "count_text_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 215,
					"y"			: 39,

					"width"		: 22,
					"height"	: 11,

					"children" :
					(
						{
							"name" : "count",
							"type" : "text",

							"x" : 0,
							"y" : 0,

							"horizontal_align"		: "center",
							"text_horizontal_align" : "center",

							"vertical_align"		: "center",
							"text_vertical_align"	: "center",

							# 누적횟수
							"text" : uiScriptLocale.TREASURE_HUNT_EVENT_RANKING_COUNT,
						},
					),
				},

				# 상위권 랭킹
				{
					"name"		: "high_ranking_list",
					"type"		: "listboxex",
					"x"			: 15,
					"y"			: 55,
					"width"		: 203,
					"height"	: 280,
				},

				# etc 
				{
					"name" : "dot",
					"type" : "image",

					"x" : 139,
					"y" : 303,
							
					"image"	: ROOT_PATH + "dot.sub",	
				},

				# 내 랭킹
				{
					"name"		: "cur_player_rank",
					"type"		: "listboxex",
					"x"			: 15,
					"y"			: 316,
					"width"		: 203,
					"height"	: 28,
				},

			),
		},
	),
}