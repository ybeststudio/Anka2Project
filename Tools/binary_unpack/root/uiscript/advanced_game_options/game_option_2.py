import uiScriptLocale
import localeInfo

IMG_DIR = "d:/ymir work/ui/game/advanced_game_options/"
ROOT_PATH = "d:/ymir work/ui/public/"

TITLE_IMAGE_TEXT_X = 58
TITLE_IMAGE_TEXT_Y = 5

OPTION_START_X = 17
SLIDER_POSITION_X = 50

SLIDER_START_Y = 40
BUTTON_START_Y = 33
BUTTON_NEXT_Y = 20

RADIO_BUTTON_RANGE_X = 80
TOGGLE_BUTTON_RANGE_X = 65

RADIO_BUTTON_TEXT_X = 25
TOGGLE_BUTTON_TEXT_X = 45

SMALL_OPTION_HEIGHT = 65
NORMAL_OPTION_HEIGHT = 80
SLIDER_OPTION_HEIGHT = 65

PRIMARY_COLOR = 0xffd0c2b7
SECONDARY_COLOR = 0xff9f9892

window = {
	"name" : "GameOptionDialog",
	# Dont touch these lines!
	"style" : (),
	"x" : 222,
	"y" : 1,
	"width" : 371,
	"height" : 404,
	# Dont touch these lines!
	"children" :
	(
		  
		{
			"name" : "anti_aliasing_window",
			"type" : "window",
			"x" : 0,
			"y" : 70,
			"width" : 304,
			"height" : SMALL_OPTION_HEIGHT,
			"children" :
			(
				{
					"name" : "anti_aliasing_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR + "option_title.png",
					"children" :
					(
						{
							"name" : "anti_aliasing_title",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : 0xFFcabdab,
							"outline" : True,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_ANTI_ALIASING_TITLE,
						},
					),
				},
				{
					"name" : "anti_aliasing_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "anti_aliasing_toggle_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color" : 0xFFcebb98,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_ANTI_ALIASING_TITLE,
						},
						{
							"name" : "anti_aliasing_toggle_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"color" : SECONDARY_COLOR,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_ANTI_ALIASING_DESCRIPTION,
						},
					),
				},
			),
		},
		{
			"name" : "graphics_level_window",
			"type" : "window",
			"x" : 0,
			"y" : 70,
			"width" : 304,
			"height" : 87,
			"children" : (
				{
					"name" : "graphics_level_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children" : (
						{
							"name" : "graphics_level_window_title",
							"type" : "text",
							"x" : 17,
							"y" : 8,
							"text" : "Grafik Kalitesi",
							"color" : 0xFFcebb98,
							"outline" : True,
						},
					),
				},
				{
					"name" : "graphics_level_window_label",
					"type" : "text",
					"x" : 17,
					"y" : 33,
					"text" : "Grafik Kalitesi",
					"color" : 0xFFcebb98,
				},
				{
					"name" : "graphics_level_0",
					"type" : "radio_button",
					"x" : 17,
					"y" : 55,
					"text" : "D\xfc\xfe\xfck",
					"text_x" : 25,
					"default_image" : IMG_DIR+"radio_unselected.png",
					"over_image" : IMG_DIR+"radio_unselected.png",
					"down_image" : IMG_DIR+"radio_selected.png",
				},
				{
					"name" : "graphics_level_1",
					"type" : "radio_button",
					"x" : 117,
					"y" : 55,
					"text" : "Orta",
					"text_x" : 25,
					"default_image" : IMG_DIR+"radio_unselected.png",
					"over_image" : IMG_DIR+"radio_unselected.png",
					"down_image" : IMG_DIR+"radio_selected.png",
				},
				{
					"name" : "graphics_level_2",
					"type" : "radio_button",
					"x" : 217,
					"y" : 55,
					"text" : "Ultra",
					"text_x" : 25,
					"default_image" : IMG_DIR+"radio_unselected.png",
					"over_image" : IMG_DIR+"radio_unselected.png",
					"down_image" : IMG_DIR+"radio_selected.png",
				},
			),
		},
		{
			"name" : "shadow_options_window",
			"type" : "window",
			"x" : 0,
			"y" : 70,
			"width" : 304,
			"height" : 112,
			"children" : (
				{
					"name" : "shadow_options_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children" : (
						{
							"name" : "shadow_options_window_title",
							"type" : "text",
							"x" : 17,
							"y" : 8,
							"text" : localeInfo.GRAPHICS_SHADOW_TITLE,
							"color" : 0xFFcebb98,
							"outline" : True,
						},
					),
				},
				{
					"name" : "shadow_options_window_label",
					"type" : "text",
					"x" : 17,
					"y" : 33,
					"text" : localeInfo.GRAPHICS_SHADOW_TITLE,
					"color" : 0xFFcebb98,
				},
				{
					"name" : "shadow_quality_0",
					"type" : "radio_button",
					"x" : 17,
					"y" : 55,
					"text" : localeInfo.GRAPHICS_SHADOW_OFF,
					"text_x" : 25,
					"default_image" : IMG_DIR+"radio_unselected.png",
					"over_image" : IMG_DIR+"radio_unselected.png",
					"down_image" : IMG_DIR+"radio_selected.png",
				},
				{
					"name" : "shadow_quality_1",
					"type" : "radio_button",
					"x" : 162,
					"y" : 55,
					"text" : localeInfo.GRAPHICS_SHADOW_LOW,
					"text_x" : 25,
					"default_image" : IMG_DIR+"radio_unselected.png",
					"over_image" : IMG_DIR+"radio_unselected.png",
					"down_image" : IMG_DIR+"radio_selected.png",
				},
				{
					"name" : "shadow_quality_2",
					"type" : "radio_button",
					"x" : 17,
					"y" : 80,
					"text" : localeInfo.GRAPHICS_SHADOW_MEDIUM,
					"text_x" : 25,
					"default_image" : IMG_DIR+"radio_unselected.png",
					"over_image" : IMG_DIR+"radio_unselected.png",
					"down_image" : IMG_DIR+"radio_selected.png",
				},
				{
					"name" : "shadow_quality_3",
					"type" : "radio_button",
					"x" : 162,
					"y" : 80,
					"text" : localeInfo.GRAPHICS_SHADOW_HIGH,
					"text_x" : 25,
					"default_image" : IMG_DIR+"radio_unselected.png",
					"over_image" : IMG_DIR+"radio_unselected.png",
					"down_image" : IMG_DIR+"radio_selected.png",
				},
			),
		},		
		{
			"name" : "tree_shadow_window",
			"type" : "window",
			"x" : 0,
			"y" : 70,
			"width" : 304,
			"height" : 132,
			"children" : (
				{
					"name" : "tree_shadow_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children" : (
						{
							"name" : "tree_shadow_window_title",
							"type" : "text",
							"x" : 17,
							"y" : 8,
							"text" : localeInfo.GRAPHICS_SHADOW_CASTERS,
							"color" : 0xFFcebb98,
							"outline" : True,
						},
					),
				},
				{
					"name" : "tree_shadow_window_label",
					"type" : "text",
					"x" : 17,
					"y" : 33,
					"text" : localeInfo.GRAPHICS_SHADOW_CASTERS,
					"color" : 0xFFcebb98,
				},
				{
					"name" : "shadow_caster_player",
					"type" : "toggle_button",
					"x" : 17,
					"y" : 55,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
					"children" : (
						{
							"name" : "shadow_caster_player_text",
							"type" : "text",
							"x" : 25,
							"y" : 0,
							"text" : localeInfo.GRAPHICS_SHADOW_CHARACTERS,
						},
					),
				},
				{
					"name" : "shadow_caster_terrain",
					"type" : "toggle_button",
					"x" : 17,
					"y" : 79,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
					"children" : (
						{
							"name" : "shadow_caster_terrain_text",
							"type" : "text",
							"x" : 25,
							"y" : 0,
							"text" : localeInfo.GRAPHICS_SHADOW_SCENERY,
						},
					),
				},
				{
					"name" : "shadow_caster_trees",
					"type" : "toggle_button",
					"x" : 17,
					"y" : 103,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
					"children" : (
						{
							"name" : "shadow_caster_trees_text",
							"type" : "text",
							"x" : 25,
							"y" : 0,
							"text" : localeInfo.GRAPHICS_SHADOW_TREES,
						},
					),
				},
			),
		},
 		{
			"name" : "sun_rays_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "sun_rays_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "sun_rays_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SUN_RAYS_TITLE,
						},
					),
				},
				{
					"name" : "sun_rays_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "sun_rays_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_SUN_RAYS_TITLE,
						},
						{
							"name" : "sun_rays_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SUN_RAYS_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "wet_ground_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "wet_ground_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "wet_ground_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WET_GROUND_TITLE,
						},
					),
				},
				{
					"name" : "wet_ground_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "wet_ground_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_WET_GROUND_TITLE,
						},
						{
							"name" : "wet_ground_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WET_GROUND_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "rain_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "rain_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "rain_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_RAIN_TITLE,
						},
					),
				},
				{
					"name" : "rain_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "rain_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_RAIN_TITLE,
						},
						{
							"name" : "rain_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_RAIN_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "storm_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "storm_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "storm_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_STORM_TITLE,
						},
					),
				},
				{
					"name" : "storm_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "storm_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_STORM_TITLE,
						},
						{
							"name" : "storm_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_STORM_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "shooting_stars_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "shooting_stars_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "shooting_stars_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SHOOTING_STARS_TITLE,
						},
					),
				},
				{
					"name" : "shooting_stars_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "shooting_stars_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_SHOOTING_STARS_TITLE,
						},
						{
							"name" : "shooting_stars_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SHOOTING_STARS_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "water_ripple_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "water_ripple_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "water_ripple_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WATER_RIPPLE_TITLE,
						},
					),
				},
				{
					"name" : "water_ripple_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "water_ripple_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_WATER_RIPPLE_TITLE,
						},
						{
							"name" : "water_ripple_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WATER_RIPPLE_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "enhanced_water_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "enhanced_water_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "enhanced_water_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_ENHANCED_WATER_TITLE,
						},
					),
				},
				{
					"name" : "enhanced_water_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "enhanced_water_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_ENHANCED_WATER_TITLE,
						},
						{
							"name" : "enhanced_water_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_ENHANCED_WATER_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "underwater_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "underwater_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "underwater_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_UNDERWATER_TITLE,
						},
					),
				},
				{
					"name" : "underwater_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "underwater_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_UNDERWATER_TITLE,
						},
						{
							"name" : "underwater_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_UNDERWATER_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "specular_reflection_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "specular_reflection_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "specular_reflection_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SPECULAR_REFLECTION_TITLE,
						},
					),
				},
				{
					"name" : "specular_reflection_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "specular_reflection_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_SPECULAR_REFLECTION_TITLE,
						},
						{
							"name" : "specular_reflection_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SPECULAR_REFLECTION_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "water_reflection_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "water_reflection_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "water_reflection_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WATER_REFLECTION_TITLE,
						},
					),
				},
				{
					"name" : "water_reflection_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "water_reflection_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_WATER_REFLECTION_TITLE,
						},
						{
							"name" : "water_reflection_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WATER_REFLECTION_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "local_light_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "local_light_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "local_light_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_LOCAL_LIGHT_TITLE,
						},
					),
				},
				{
					"name" : "local_light_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "local_light_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_LOCAL_LIGHT_TITLE,
						},
						{
							"name" : "local_light_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_LOCAL_LIGHT_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "sharpness_windows",
			"type" : "window",
			"x" : 0,
			"y" : 200,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "sharpness_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "sharpness_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SHARPNESS_TITLE,
						},
					),
				},
				{
					"name" : "sharpness_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 55,
				},
				{
					"name" : "sharpness_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_SHARPNESS_TITLE,
				},
			),			
		},
		{
			"name" : "death_cam_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "death_cam_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "death_cam_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : "Death Cam",
						},
					),
				},
				{
					"name" : "death_cam_effect",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "death_cam_effect_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : "Death Cam",
						},
						{
							"name" : "death_cam_effect_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : "Activate / Deactivate the death camera.",
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "speedtree_fx_window",
			"type" : "window",
			"x" : 0,
			"y" : 330,
			"width":304,
			"height":185,
			"children":
			(
				{
					"name" : "speedtree_fx_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "speedtree_fx_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_TITLE,
						},
					),
				},
				{
					"name" : "speedtree_dynamic_light",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "speedtree_dynamic_light_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_LIGHTING,
						},
						{
							"name" : "speedtree_dynamic_light_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_TOGGLE_HINT,
							"color" : SECONDARY_COLOR,
						},
					),
				},
				{
					"name" : "speedtree_wind",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 73,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "speedtree_wind_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_WIND,
						},
						{
							"name" : "speedtree_wind_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_TOGGLE_HINT,
							"color" : SECONDARY_COLOR,
						},
					),
				},
				{
					"name" : "speedtree_enhanced_foliage",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 113,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "speedtree_enhanced_foliage_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_ENHANCED_FOLIAGE,
						},
						{
							"name" : "speedtree_enhanced_foliage_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_ENHANCED_FOLIAGE_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},
				{
					"name" : "speedtree_falling_leaves",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 153,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "speedtree_falling_leaves_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_FALLING_LEAVES,
						},
						{
							"name" : "speedtree_falling_leaves_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_FALLING_LEAVES_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},

			),
		},
		{
			"name" : "fov_windows",
			"type" : "window",
			"x" : 0,
			"y" : 395,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "fov_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "fov_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_FOV,
						},
					),
				},
				{
					"name" : "camera_fov_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 55,
				},
				{
					"name" : "camera_fov_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_FOV,
				},
			),
		},

        {
			"name" : "camera_window",
			"type" : "window",
			"x" : 0,
			"y" : 460,
			"width":304,
			"height":SMALL_OPTION_HEIGHT * 2,
			"children":
			(
				{
					"name" : "camera_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_camera",
							"type" : "text",
							"color": 0xFFcabdab,
							"outline": True,
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE,
						},
					),
				},
				{
					"name" : "camera_far_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "camera_far_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_CAMERA_FAR,
						},
						{
							"name" : "camera_far_toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_CAMERA_FAR_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	
				{
					"name" : "keyboard_camera_speed_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 77,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_KEYBOARD_CAMERA_SPEED,
				},
				{
					"name" : "keyboard_camera_speed_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 99,
				},

			),
		},
   
		{
			"name" : "skybox_window",
			"type" : "window",
			"x" : 0, "y" : 525, "width" : 304, "height" : 90,
			"children" : (
				{
					"name" : "skybox_window_img", "type" : "expanded_image",
					"x" : 0, "y" : 0, "image" : IMG_DIR + "option_title.png",
					"children" : ({
						"name" : "skybox_window_name", "type" : "text",
						"x" : TITLE_IMAGE_TEXT_X, "y" : TITLE_IMAGE_TEXT_Y,
						"text" : localeInfo.GRAPHICS_SKYBOX_TITLE,
					},),
				},
				{
					"name" : "skybox_label", "type" : "text",
					"x" : OPTION_START_X, "y" : 33,
					"text" : localeInfo.GRAPHICS_SKYBOX_TITLE,
				},
				{
					"name" : "skybox_combo_holder", "type" : "window",
					"x" : OPTION_START_X, "y" : 55, "width" : 260, "height" : 22,
				},
			),
		},
		{
			"name" : "environment_window",
			"type" : "window",
			"x" : 0,
			"y" : 525,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "environment_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "environment_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_TITLE,
						},
					),
				},
				{
					"name" : "night_mode_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "night_mode_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_SKY_NIGHT,
						},
						{
							"name" : "night_mode_toggle_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"color": 0xff9f9892,
							"text" : localeInfo.GRAPHICS_TOGGLE_HINT,
						},
					),
				},
			),
		},
		{
			"name" : "remove_fog_window",
			"type" : "window",
			"x" : 0,
			"y" : 590,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "remove_fog_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "remove_fog_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : "Remove Fog",
						},
					),
				},
				{
					"name" : "remove_fog_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "remove_fog_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : "Remove Fog",
						},
						{
							"name" : "remove_fog_toggle_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"color": 0xff9f9892,
							"text" : "Active / Desactive",
						},
					),
				},
			),
		},
		{
			"name" : "environment_darkness_window",
			"type" : "window",
			"x" : 0,
			"y" : 655,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "environment_darkness_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "environment_darkness_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_DARKNESS_TITLE,
						},
					),
				},
				{
					"name" : "environment_darkness_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 55,
				},
				{
					"name" : "environment_darkness_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_ENVIRONMENT_DARKNESS_TITLE,
				},
			),
		},
		{
			"name" : "environment_saturation_window",
			"type" : "window",
			"x" : -10000,
			"y" : 720,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "environment_saturation_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "environment_saturation_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_SATURATION_TITLE,
						},
					),
				},
				{
					"name" : "environment_saturation_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 55,
				},
				{
					"name" : "environment_saturation_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_ENVIRONMENT_SATURATION_TITLE,
				},
			),
		},
		{
			"name" : "environment_contrast_window",
			"type" : "window",
			"x" : -10000,
			"y" : 785,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "environment_contrast_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "environment_contrast_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_CONTRAST_TITLE,
						},
					),
				},
				{
					"name" : "environment_contrast_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 55,
				},
				{
					"name" : "environment_contrast_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_ENVIRONMENT_CONTRAST_TITLE,
				},
			),
		},
		{
			"name" : "environment_temperature_window",
			"type" : "window",
			"x" : -10000,
			"y" : 850,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "environment_temperature_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "environment_temperature_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_TEMPERATURE_TITLE,
						},
					),
				},
				{
					"name" : "environment_temperature_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 55,
				},
				{
					"name" : "environment_temperature_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_ENVIRONMENT_TEMPERATURE_TITLE,
				},
			),
		},
		{
			"name" : "environment_color_window",
			"type" : "window",
			"x" : 0,
			"y" : 720,
			"width" : 304,
			"height" : NORMAL_OPTION_HEIGHT,
			"children" :
			(
				{
					"name" : "environment_color_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR + "option_title.png",
					"children" :
					(
						{
							"name" : "environment_color_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : 0xFFcabdab,
							"outline" : True,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_COLOR_TITLE,
						},
					),
				},
				{
					"name" : "environment_color_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color" : 0xFFcebb98,
					"outline" : True,
					"text_horizontal_align" : "left",
					"text" : localeInfo.GAME_OPTION_ENVIRONMENT_COLOR_TITLE,
				},
				{
					"name" : "environment_color_button",
					"type" : "button",
					"x" : OPTION_START_X,
					"y" : 52,
					"text_color" : 0xFFcebb98,
					"text" : localeInfo.GAME_OPTION_ENVIRONMENT_COLOR_OPEN,
					"outline" : True,
					"default_image" : IMG_DIR + "btn_0.png",
					"over_image" : IMG_DIR + "btn_1.png",
					"down_image" : IMG_DIR + "btn_2.png",
				},
			),
		},
	),
}

import advanced_options_reference
advanced_options_reference.ApplyLayout(window, 2)
