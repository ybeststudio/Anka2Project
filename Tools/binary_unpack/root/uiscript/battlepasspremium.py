import uiScriptLocale
import localeInfo

WIDTH = 627
HEIGHT = 508

IMAGE = "premium_new_battlepas/"

window = {
	"name": "BattlepassWindow",
	"style": ("movable", "float",),

	"x": 0,
	"y": 0,
	
	"width": WIDTH,
	"height": HEIGHT,

	"children": (
		{
			"name": "board",
			"type": "window",

			"style": ("attach",),

			"x": 0,
			"y": 0,

			"width": WIDTH,
			"height": HEIGHT,

			"children": (
				{
					"name": "board_bg",
					"type": "image",
					"style": ("attach",),
					"x": 0,
					"y": 0,
					"width": WIDTH,
					"height": HEIGHT,
					"image": IMAGE + "outside_bp_bg.png",
				},
				{
					"name": "TitleBar",
					"type": "titlebar",

					"style": ("attach",),

					"x": 4,
					"y": 5,

					"width": WIDTH - 10,

					"children": (
						{
							"name": "title_text",
							"type": "text",

							"x": 0,
							"y": -3,

							"horizontal_align": "center",
							"vertical_align": "center",

							"text_horizontal_align": "center",
							"text_vertical_align": "center",

							"text": uiScriptLocale.BATTLEPASS_TITLE_PREMIUM,
						},
					),
				},

				{
					"name": "missions_list",
					"type": "listboxex",

					"x": 10,
					"y": 35-4,

					"width": 365,
					"height": HEIGHT - 35 - 10,

					"itemsize_x": 365,
					"itemsize_y": 51,

					"itemstep": 51,

					"viewcount": (HEIGHT - 35 - 10) / 51,
				},
				{
					"name": "ScrollBarOuter",
					"type": "image",

					"x": 10 + 359 +3,
					"y": 35,
					"image" : IMAGE + "missions_scrollbar_outer.png",
				},
				{
					"name": "ScrollBar",
					"type": "slimscrollbar",

					"x": 10 + 359 + 3 + 1,
					"y": 35 + 1,
					"size" : 458,
				},

				{
					"name": "battlepass_description_box",
					"type": "image",
					"style": ("attach",),

					"x": 10 + 229,
					"y": 35,

					"horizontal_align": "right",

					"image": IMAGE + "bp_desc_bg.png",

					"children": (
						{
							"name": "normal_view",
							"type": "window",
							"style": ("attach",),

							"width": 229,
							"height": 462,

							"x": 0,
							"y": 0,

							"children": (

								{
									"name": "description_bg_n",
									"type": "image",

									"x": 0,
									"y": 190,

									"horizontal_align": "center",

									"image": IMAGE + "battlepass_infos_bg.png",

									"children": (
										{
											"name": "Text3Info",
											"type": "text",

											"x": 0,
											"y": 0,

											"horizontal_align": "center",
											"vertical_align": "center",

											"text_horizontal_align": "center",
											"text_vertical_align": "center",

											"text" : uiScriptLocale.BATTLEPASS_SELECT_MISSION,
											"fontsize" : "LARGE",
										},
									),
								},
								
								
							),
						},

						{
							"name": "description_bg_f",
							"type": "image",

							"x": 0,
							"y": 295,

							"horizontal_align": "center",

							"image": IMAGE + "battlepass_infos_bg.png",

							"children": (
								{
									"name": "Text6Info",
									"type": "text",

									"x": 0,
									"y": -130,

									"horizontal_align": "center",
									"vertical_align": "center",

									"text_horizontal_align": "center",
									"text_vertical_align": "center",

									"text" : uiScriptLocale.BATTLEPASS_SELECT_MISSION,
									"fontsize" : "LARGE",
								},
							),
						},
						{
							"name": "quest_view",
							"type": "window",
							"style": ("attach",),

							"width": 229,
							"height": 462,

							"x": 0,
							"y": 0,

							"children": (
								{
									"name": "battlepass_name_n",
									"type": "button",

									"x": 0,
									"y": 4,

									"horizontal_align": "center",

									"default_image": IMAGE + "battlepass_info.png",
									"over_image": IMAGE + "battlepass_info_1.png",
									"down_image": IMAGE + "battlepass_info_2.png",
									"disable_image": IMAGE + "battlepass_info.png",

									"text": "",
									"children" : 
									(
										{

										"name": "Text1Info",
										"type": "text",

										"x": -90,
										"y": 10,
										"text" : uiScriptLocale.BATTLEPASS_SELECT_MISSION,
										"fontsize" : "LARGE",

										"horizontal_align": "center",
										},
									),
								},
								{
									"name": "status_bg_q",
									"type": "image",

									"x": 0,
									"y": 38 + 4 + 4,

									"horizontal_align": "center",

									"image": IMAGE + "battlepass_infos_bg.png",

									"children": (
										{
											"name": "status_text_q",
											"type": "text",

											"x": 0,
											"y": 0,

											"horizontal_align": "center",
											"vertical_align": "center",

											"text_horizontal_align": "center",
											"text_vertical_align": "center",

											"text" : uiScriptLocale.BATTLEPASS_STATUS,
										},
									),
								},
								
								{
									"name": "description_title_bg_q",
									"type": "image",

									"x": 0,
									"y": 38 + 4 + 4 + 20 + 2 + 21 + 2 + 21 + 20 + 10,

									"horizontal_align": "center",

									"image": IMAGE + "battlepass_infos_bg.png",

									"children": (
										{
											"name": "description_title_q",
											"type": "text",

											"x": 0,
											"y": 0,

											"horizontal_align": "center",
											"vertical_align": "center",

											"text_horizontal_align": "center",
											"text_vertical_align": "center",

											"text" : uiScriptLocale.BATTLEPASS_MISSION_INFO,
											"fontsize": "LARGE",
										},
									),
								},


							),
						},
						{
							"name": "mission_time_bg",
							"type": "image",

							"x": 0,
							"y": 38 + 4 + 4 + 20 + 4,

							"horizontal_align": "center",

							"image": IMAGE + "mission_state_underline.png",

							"children": (
								{
									"name": "Text2Info",
									"type": "text",

									"x": 0,
									"y": 0,

									"horizontal_align": "center",
									"vertical_align": "center",

									"text_horizontal_align": "center",
									"text_vertical_align": "center",

									"text" : uiScriptLocale.BATTLEPASS_SELECT_MISSION,
								},
							),
						},
						{
							"name": "mission_time_progress",
							"type": "image",

							"x": 0,
							"y": 38 + 4 + 4 + 20 + 4 + 21 + 2,

							"horizontal_align": "center",

							"image": IMAGE + "big_empty.png",

							"children": (
								{
									"name": "mission_progress",
									"type": "expanded_image",

									"x": 0,
									"y": 0,

									"image": IMAGE + "big_empty.png",
								},
							),
						},
						{
							"name": "mission_progress_bg",
							"type": "image",

							"x": 0,
							"y": 38 + 4 + 4 + 20 + 4 + 21 + 2 + 11 + 3,

							"horizontal_align": "center",

							"image": IMAGE + "mission_state_underline.png",

							"children": (
								{
									"name": "progress_title",
									"type": "text",

									"x": 0,
									"y": 0,

									"horizontal_align": "center",
									"vertical_align": "center",

									"text_horizontal_align": "center",
									"text_vertical_align": "center",

									"text" : uiScriptLocale.BATTLEPASS_MISSION_PROGRESS,
								},
							),
						},
						{
							"name": "mission_progress_bg_q",
							"type": "image",

							"x": 0,
							"y": 38 + 4 + 4 + 20 + 4 + 21 + 2 + 11 + 3 + 21 + 2,

							"horizontal_align": "center",

							"image": IMAGE + "big_empty.png",

							"children": (
								{
									"name": "mission_progress_q",
									"type": "expanded_image",

									"x": 0,
									"y": 0,

									"image": IMAGE + "big_empty.png",
								},
							),
						},
						{
							"name": "reward_icon_n",
							"type": "image",

							"x": 0,
							"y": 300,

							"horizontal_align": "center",

							"image": IMAGE + "battlepass_infos_bg.png",

							"children":
							(
								{
									"name" : "odul_sonu",
									"type" : "text",
									"x" : 0,
									"y" : 0,
									"horizontal_align": "center",
									"vertical_align": "center",
									"text_horizontal_align": "center",
									"text_vertical_align": "center",
								},
								# {
									# "name" : "odul_sonu_resim",
									# "type" : "image",
									# "x" : 80,
									# "y" : -53,
									# "image" : IMAGE + "treasure.png",
								# },
							

							),
						},

						{
							"name": "FinalReward",
							"type": "button",

							"x": 0,
							"y": 28 + 5,

							"horizontal_align": "center",
							"vertical_align": "bottom",

							"default_image": IMAGE + "upgrade_btn.png",
							"over_image": IMAGE + "upgrade_btn_1.png",
							"down_image": IMAGE + "upgrade_btn_2.png",
							"disable_image": IMAGE + "upgrade_btn.png",

							"text" : uiScriptLocale.BATTLEPASS_REWARD_GET,
						},
					),
				},
			),
		},
	),
}