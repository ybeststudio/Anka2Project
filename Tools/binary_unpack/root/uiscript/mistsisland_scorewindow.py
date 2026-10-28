import uiScriptLocale

WINDOW_WIDTH	= 392
WINDOW_HEIGHT	= 144
MAIN_WIDTH		= 369
MAIN_HEIGHT		= 100
ROOT_PATH		= "d:/ymir work/ui/occupation/"
PATTERN_PATH	= "d:/ymir work/ui/pattern/"

window = {
	"name" : "MistsIsland_ScoreWindow", "style" : ("movable", "float", ),
	"x" : (SCREEN_WIDTH - WINDOW_WIDTH) / 2, "y" : (SCREEN_HEIGHT - WINDOW_HEIGHT) / 2 - 96,
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar",
			"x" : 0, "y" : 0,
			"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"title" : uiScriptLocale.MISTS_ISLAND_TITLE_SCORE,
		},
		{
			"name" : "help", "type" : "image",
			"x" : WINDOW_WIDTH - 48, "y" : 9,
			"image" : PATTERN_PATH + "q_mark_01.tga",
		},
		{
			"name" : "main_window", "type" : "window",
			"x" : 11, "y" : 32,
			"width" : MAIN_WIDTH, "height" : MAIN_HEIGHT,
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
						## LeftBottom
						{
							"name" : "left_bttom", "type" : "image", "style" : ("ltr",),
							"x" : 0, "y" : MAIN_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						## RightBottom
						{
							"name" : "right_bttom", "type" : "image", "style" : ("ltr",),
							"x" : MAIN_WIDTH - 16, "y" : MAIN_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						## Top
						{
							"name" : "top", "type" : "expanded_image", "style" : ("ltr",),
							"x" : 16, "y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, 20.0625, 0.0),
						},
						## Left
						{
							"name" : "left", "type" : "expanded_image", "style" : ("ltr",),
							"x" : 0, "y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0.0, 3.25),
						},
						## Right
						{
							"name" : "right", "type" : "expanded_image", "style" : ("ltr",),
							"x" : MAIN_WIDTH - 16, "y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0.0, 3.25),
						},
						## Bottom
						{
							"name" : "bottom", "type" : "expanded_image", "style" : ("ltr",),
							"x" : 16, "y" : MAIN_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, 20.0625, 0.0),
						},
					),
				},
				# 제국 이름
				{
					"name" : "empire_window", "type" : "window",
					"x" : 0, "y" : 0,
					"width" : 127, "height" : 100,
					"style" : ("ltr", ),
					"children" :
					(
						{
							"name" : "empire_title", "type" : "image",
							"x" : 3, "y" : 3,
							"image" : ROOT_PATH + "tab_empire.sub",
							"children" :
							(
								{
									"name" : "empire_text", "type" : "text",
									"x" : 0, "y" : 0,
									"text" : uiScriptLocale.MISTS_ISLAND_EMPIRE_NAME,
									"color" : 0xFFD4D4D4, "all_align" : "center",
								},
							)
						},
						{
							"name" : "empire_A", "type" : "image",
							"x" : 5, "y" : 25,
							"image" : ROOT_PATH + "occupy_tab_01.tga",
							"children" :
							(
								{
									"name" : "empire_A_flag", "type" : "image",
									"x" : 3, "y" : 2,
									"image" : ROOT_PATH + "flag_rect_01.sub",
								},
								{
									"name" : "empire_A_text", "type" : "text",
									"x" : 11, "y" : 0,
									"text" : uiScriptLocale.EMPIRE_A,
									"color" : 0xFFD7D1C3, "all_align" : "center",
								},
							)
						},
						{
							"name" : "empire_B", "type" : "image",
							"x" : 5, "y" : 50,
							"image" : ROOT_PATH + "occupy_tab_01.tga",
							"children" :
							(
								{
									"name" : "empire_B_flag", "type" : "image",
									"x" : 3, "y" : 2,
									"image" : ROOT_PATH + "flag_rect_02.sub",
								},
								{
									"name" : "empire_B_text", "type" : "text",
									"x" : 11, "y" : 0,
									"text" : uiScriptLocale.EMPIRE_B,
									"color" : 0xFFD7D1C3, "all_align" : "center",
								},
							)
						},
						{
							"name" : "empire_C", "type" : "image",
							"x" : 5, "y" : 75,
							"image" : ROOT_PATH + "occupy_tab_01.tga",
							"children" :
							(
								{
									"name" : "empire_C_flag", "type" : "image",
									"x" : 3, "y" : 2,
									"image" : ROOT_PATH + "flag_rect_03.sub",
								},
								{
									"name" : "empire_C_text", "type" : "text",
									"x" : 11, "y" : 0,
									"text" : uiScriptLocale.EMPIRE_C,
									"color" : 0xFFD7D1C3, "all_align" : "center",
								},
							)
						},
					),
				},
				{
					"name" : "score_board", "type" : "image",
					"x" : 127, "y" : 0,
					"image" : ROOT_PATH + "bg_score.sub",
				},
				# Empire Score
				{
					"name" : "score_window", "type" : "window",
					"x" : 267, "y" : 0,
					"width" : 102, "height" : 100,
					"children" :
					(
						{
							"name" : "score_title", "type" : "image",
							"x" : 0, "y" : 3,
							"image" : ROOT_PATH + "tab_kill_count.sub",
							"children" :
							(
								{
									"name" : "score_text", "type" : "text",
									"x" : 0, "y" : 0,
									"text" : uiScriptLocale.MISTS_ISLAND_EMPIRE_SCORE,
									"color" : 0xFFD4D4D4, "all_align" : "center",
								},
							)
						},
						{
							"name" : "score_bg_A", "type" : "image",
							"x" : 2, "y" : 25,
							"image" : ROOT_PATH + "occupy_tab_02.tga",
							"children" :
							(
								{
									"name" : "score_A", "type" : "text",
									"x" : 0, "y" : 0,
									"text" : "0",
									"color" : 0xFFD7D1C3, "all_align" : "center",
								},
							)
						},
						{
							"name" : "score_bg_B", "type" : "image",
							"x" : 2, "y" : 50,
							"image" : ROOT_PATH + "occupy_tab_02.tga",
							"children" :
							(
								{
									"name" : "score_B", "type" : "text",
									"x" : 0, "y" : 0,
									"text" : "0",
									"color" : 0xFFD7D1C3, "all_align" : "center",
								},
							)
						},
						{
							"name" : "score_bg_C", "type" : "image",
							"x" : 2, "y" : 75,
							"image" : ROOT_PATH + "occupy_tab_02.tga",
							"children" :
							(
								{
									"name" : "score_C", "type" : "text",
									"x" : 0, "y" : 0,
									"text" : "0",
									"color" : 0xFFD7D1C3, "all_align" : "center",
								},
							)
						},
					),
				},
			),
		},
	),
}
