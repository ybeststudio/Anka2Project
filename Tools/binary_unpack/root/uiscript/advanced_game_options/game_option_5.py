import uiScriptLocale
import localeInfo

IMG_DIR = "d:/ymir work/ui/game/advanced_game_options/"
ROOT_PATH = "d:/ymir work/ui/public/"

TITLE_IMAGE_TEXT_X = 58
TITLE_IMAGE_TEXT_Y = 5

OPTION_START_X = 17
RADIO_BUTTON_RANGE_X = 80
RADIO_BUTTON_TEXT_X = 25
TOGGLE_BUTTON_TEXT_X = 45
SECONDARY_COLOR = 0xff9f9892

SMALL_OPTION_HEIGHT = 65
SLIDER_OPTION_HEIGHT = 92
FPS_OPTION_HEIGHT = 152
SLIDER_START_Y = 50

window = {
	"name" : "GameOptionDialog",
	"style" : (),
	"x" : 222,
	"y" : 1,
	"width" : 371,
	"height" : 404,
	"children" :
	(
		{
			"name" : "crowd_performance_window",
			"type" : "window",
			"x" : 0,
			"y" : 5,
			"width" : 304,
			"height" : 94,
			"children" :
			(
				{
					"name" : "crowd_performance_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR + "option_title.png",
					"children" :
					(
						{
							"name" : "crowd_performance_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : 0xFFcabdab,
							"outline" : True,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_CROWD_PERFORMANCE_TITLE,
						},
					),
				},
				{
					"name" : "high_performance_crowd_mode",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 35,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "high_performance_crowd_mode_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align" : "left",
							"color" : 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_CROWD_PERFORMANCE_MODE,
						},
					),
				},
				{
					"name" : "high_performance_crowd_mode_description",
					"type" : "adjust_text",
					"max_width" : 240,
					"x" : OPTION_START_X + TOGGLE_BUTTON_TEXT_X,
					"y" : 54,
					"text_horizontal_align" : "left",
					"color" : SECONDARY_COLOR,
					"text" : localeInfo.GAME_OPTION_CROWD_PERFORMANCE_DESC,
				},
			),
		},
		{
			"name" : "fps_limit_window",
			"type" : "window",
			"x" : 0,
			"y" : 5,
			"width":304,
			"height":FPS_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "fps_limit_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "fps_limit_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_FPS_LIMIT_TITLE,
						},
					),
				},
				{
					"name" : "fps_limit_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 34,
					"color" : 0xFFcebb98,
					"text" : localeInfo.GAME_OPTION_FPS_LIMIT_LABEL,
				},
				{
					"name" : "fps_limit_60",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 55,
					"text" : localeInfo.GAME_OPTION_FPS_VALUE_60,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
					"text_color": 0xff9f9892
				},
				{
					"name" : "fps_limit_120",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 55,
					"text" : localeInfo.GAME_OPTION_FPS_VALUE_120,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
					"text_color": 0xff9f9892
				},
				{
					"name" : "fps_limit_144",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 55,
					"text" : localeInfo.GAME_OPTION_FPS_VALUE_144,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
					"text_color": 0xff9f9892
				},
				{
					"name" : "fps_limit_200",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*3,
					"y" : 55,
					"text" : localeInfo.GAME_OPTION_FPS_VALUE_200,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
					"text_color": 0xff9f9892
				},
				{
					"name" : "hide_fps_minimap",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 109,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "hide_fps_minimap_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align" : "left",
							"color" : 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_HIDE_FPS_MINIMAP,
						},
					),
				},
				{
					"name" : "hide_fps_minimap_description",
					"type" : "adjust_text",
					"max_width" : 240,
					"x" : OPTION_START_X + TOGGLE_BUTTON_TEXT_X,
					"y" : 126,
					"text_horizontal_align" : "left",
					"color" : SECONDARY_COLOR,
					"text" : localeInfo.GAME_OPTION_HIDE_FPS_MINIMAP_DESC,
				},
			),
		},
	),
}

import advanced_options_reference
advanced_options_reference.ApplyLayout(window, 5)
