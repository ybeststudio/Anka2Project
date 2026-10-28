import uiScriptLocale

WINDOW_WIDTH	= 242
WINDOW_HEIGHT	= 192
MAIN_WIDTH		= 219
MAIN_HEIGHT		= 149
ROOT_PATH		= "d:/ymir work/ui/occupation/"
PATTERN_PATH	= "d:/ymir work/ui/pattern/"

window = {
	"name" : "MistsIsland_RankWindow", "style" : ("movable", "float",),
	"x" : (SCREEN_WIDTH - WINDOW_WIDTH) / 2 - 74, "y" : (SCREEN_HEIGHT - WINDOW_HEIGHT) / 2 + 72,
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar",
			"x" : 0, "y" : 0,
			"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"title" : uiScriptLocale.MISTS_ISLAND_TITLE_RANK,
		},
		{
			"name" : "main_window", "type" : "window",
			"x" : 11, "y" : 32,
			"width" : MAIN_WIDTH, "height" : MAIN_HEIGHT,
			"style" : ("ltr", ),
			"children" :
			(
				## Border
				{
					"name" : "border_window", "type" : "window",
					"x" : 0, "y" : 0,
					"width" : MAIN_WIDTH, "height" : MAIN_HEIGHT,
					"style" : ("ltr", ),
					"children" :
					(
						## LeftTop
						{
							"name" : "left_top", "type" : "image", "style" : ("ltr",),
							"x" : 0, "y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						## RightTop
						{
							"name" : "right_top", "type" : "image", "style" : ("ltr",),
							"x" : MAIN_WIDTH - 16, "y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						### LeftBottom
						#{
						#	"name" : "left_bttom", "type" : "image", "style" : ("ltr",),
						#	"x" : 0, "y" : MAIN_HEIGHT - 16,
						#	"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						#},
						### RightBottom
						#{
						#	"name" : "right_bttom", "type" : "image", "style" : ("ltr",),
						#	"x" : MAIN_WIDTH - 16, "y" : MAIN_HEIGHT - 16,
						#	"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						#},
						## Top
						{
							"name" : "top", "type" : "expanded_image", "style" : ("ltr",),
							"x" : 16, "y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, 10.6875, 0.0),
						},
						## Left
						{
							"name" : "left", "type" : "expanded_image", "style" : ("ltr",),
							"x" : 0, "y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0.0, 6.3125),
						},
						## Right
						{
							"name" : "right", "type" : "expanded_image", "style" : ("ltr",),
							"x" : MAIN_WIDTH - 16, "y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0.0, 6.3125),
						},
						### Bottom
						#{
						#	"name" : "bottom", "type" : "expanded_image", "style" : ("ltr",),
						#	"x" : 16, "y" : MAIN_HEIGHT - 16,
						#	"image" : PATTERN_PATH + "border_A_bottom.tga",
						#	"rect" : (0.0, 0.0, 10.6875, 0.0),
						#},
						## Mid
						{
							"name" : "mid", "type" : "expanded_image", "style" : ("ltr",),
							"x" : 2, "y" : 48,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, 12.4375, 0.0),
						},
					),
				},
				# Rank
				{
					"name" : "rank_window", "type" : "window",
					"x" : 0, "y" : 0,
					"width" : 219, "height" : 50,
					"style" : ("ltr", ),
					"children" :
					(
						{
							"name" : "rank_title", "type" : "image",
							"x" : 3, "y" : 3,
							"image" : ROOT_PATH + "tab_rank.sub",
							"style" : ("ltr", ),
							"children" :
							(
								{
									"name" : "score_title_window", "type" : "window",
									"x" : 10, "y" : 2,
									"width" : 72, "height" : 17,
									"children" :
									(
										{
											"name" : "score_title", "type" : "text",
											"x" : 0, "y" : 0,
											"text" : uiScriptLocale.MISTS_ISLAND_SCORE,
											"color" : 0xFFD4D4D4, "all_align" : "center",
										},
									),
								},
								{
									"name" : "rank_title_window", "type" : "window",
									"x" : 101, "y" : 2,
									"width" : 26, "height" : 17,
									"children" :
									(
										{
											"name" : "rank_title", "type" : "text",
											"x" : 0, "y" : 0,
											"text" : uiScriptLocale.MISTS_ISLAND_RANK,
											"color" : 0xFFD4D4D4, "all_align" : "center",
										},
									),
								},
								{
									"name" : "end_time_title_window", "type" : "window",
									"x" : 150, "y" : 2,
									"width" : 52, "height" : 17,
									"children" :
									(
										{
											"name" : "end_time_title", "type" : "text",
											"x" : 0, "y" : 0,
											"text" : uiScriptLocale.MISTS_ISLAND_END_TIME,
											"color" : 0xFFD4D4D4, "all_align" : "center",
										},
									),
								},
							),
						},
						{
							"name" : "rank_info", "type" : "image",
							"x" : 5, "y" : 24,
							"image" : ROOT_PATH + "occupy_tab_03.tga",
							"style" : ("ltr", ),
							"children" :
							(
								{
									"name" : "score_window", "type" : "window",
									"x" : 8, "y" : 2,
									"width" : 72, "height" : 17,
									"style" : ("ltr", ),
									"children" :
									(
										{
											"name" : "score", "type" : "text",
											"x" : 0, "y" : 0,
											"text" : "0",
											"color" : 0xFFD9CDB5, "all_align" : "center",
										},
									),
								},
								{
									"name" : "rank_window", "type" : "window",
									"x" : 99, "y" : 2,
									"width" : 26, "height" : 17,
									"style" : ("ltr", ),
									"children" :
									(
										{
											"name" : "rank", "type" : "text",
											"x" : 0, "y" : 0,
											"text" : "-",
											"color" : 0xFFD9CDB5, "all_align" : "center",
										},
									),
								},
								{
									"name" : "end_time_window", "type" : "window",
									"x" : 148, "y" : 2,
									"width" : 52, "height" : 17,
									"style" : ("ltr", ),
									"children" :
									(
										{
											"name" : "end_time", "type" : "text",
											"x" : 0, "y" : 0,
											"text" : "-",
											"color" : 0xFFD9CDB5, "all_align" : "center",
										},
									),
								},
							),
						},
					),
				},
				# Info
				{
					"name" : "info_image", "type" : "image",
					"x" : 0, "y" : 50,
					"image" : ROOT_PATH + "bg_rank.sub",
					"children" :
					(
						{
							"name" : "info_window", "type" : "window",
							"x" : 7, "y" : 6,
							"width" : 188, "height" : 84,
							"children" :
							(
								{
									"name" : "info_text", "type" : "text",
									"x" : 0, "y" : 0,
									"text" : uiScriptLocale.MISTS_ISLAND_INFO_TEXT,
									"color" : 0xFFD4D4D4,
								},
							),
						},
						{
							"name" : "info_scroll", "type" : "scrollbar",
							"x" : 202, "y" : 3,
							"size" : 91,
						},
					),
				},
			),
		},
	),
}
