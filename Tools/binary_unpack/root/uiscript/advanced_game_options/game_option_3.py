import uiScriptLocale, localeInfo

IMG_DIR = "d:/ymir work/ui/game/advanced_game_options/"

TITLE_IMAGE_TEXT_X = 58
TITLE_IMAGE_TEXT_Y = 5

OPTION_START_X = 17
SLIDER_POSITION_X = 30

SLIDER_START_Y = 50
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
	"y" :1,
	"width" : 371,
	"height" : 404,
	# Dont touch these lines!
	"children" :
	(
		{
			"name" : "bgm_window",
			"type" : "window",
			"x" : 0,
			"y" : 5,
			"width":304,
			"height":SLIDER_OPTION_HEIGHT+15,
			"children":
			(
				{
					"name" : "bgm_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_bgm",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : uiScriptLocale.MUSICLIST_TITLE,
						},
					),
				},
				{
					"name" : "bgm_file",
					"type" : "text",
					"text_horizontal_align":"left",
					"x" : OPTION_START_X,
					"y" : BUTTON_START_Y+5,
					"text":uiScriptLocale.OPTION_MUSIC_DEFAULT_THEMA,
					"color" : PRIMARY_COLOR,
				},
				{
					"name" : "bgm_button",
					"type" : "toggle_button",
					"x" : 155,
					"y" : BUTTON_START_Y,
					"text" : uiScriptLocale.OPTION_MUSIC_CHANGE,
					"default_image" : IMG_DIR + "btn_0.png",
					"over_image" : IMG_DIR + "btn_1.png",
					"down_image" : IMG_DIR + "btn_2.png",
				},
			),
		},
		{
			"name" : "volume_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+SLIDER_OPTION_HEIGHT+15,
			"width":304,
			"height":SLIDER_OPTION_HEIGHT*3,
			"children":
			(
				{
					"name" : "volume_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_sound",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : getattr(localeInfo, "SOUND_VOLUME_TITLE", "Ses d\xfczeyi"),
						},
					),
				},
				{
					"name" : "sound_text",
					"type" : "text",
					"text_horizontal_align":"left",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y-20,
					"text":getattr(localeInfo, "SOUND_EFFECTS_MASTER", "Genel efekt ses d\xfczeyi"),
					"color" : PRIMARY_COLOR,
				},		
				{
					"name" : "sound_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y,
				},
				{
					"name" : "music_text",
					"type" : "text",
					"text_horizontal_align":"left",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y*2-20,
					"text":uiScriptLocale.OPTION_MUSIC,
					"color" : PRIMARY_COLOR,
				},		
				{
					"name" : "music_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y*2,
				},
				{
					"name" : "environment_volume_text",
					"type" : "text",
					"text_horizontal_align" : "left",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y*3-20,
					"text" : localeInfo.GAME_OPTION_ENVIRONMENT_SONG,
					"color" : PRIMARY_COLOR,
				},
				{
					"name" : "environment_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y*3,
				},
			),
		},
		{
			"name" : "sound_categories_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+SLIDER_OPTION_HEIGHT*4+15,
			"width" : 304,
			"height" : 380,
			"children" :
			(
				{
					"name" : "sound_categories_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children" :
					(
						{
							"name" : "sound_categories_title",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : 0xFFcabdab,
							"outline" : True,
							"text" : getattr(localeInfo, "SOUND_CATEGORY_TITLE", "Ses efekti ayr\xfdnt\xfdlar\xfd"),
						},
					),
				},
				{
					"name" : "sound_hits_text",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 30,
					"color" : PRIMARY_COLOR,
					"text" : getattr(localeInfo, "SOUND_CATEGORY_HITS", "Vuru\xfe sesleri"),
				},
				{
					"name" : "sound_hits_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 50,
				},
				{
					"name" : "sound_critical_text",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 80,
					"color" : PRIMARY_COLOR,
					"text" : getattr(localeInfo, "SOUND_CATEGORY_CRITICAL", "Kritik vuru\xfelar"),
				},
				{
					"name" : "sound_critical_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 100,
				},
				{
					"name" : "sound_skills_text",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 130,
					"color" : PRIMARY_COLOR,
					"text" : getattr(localeInfo, "SOUND_CATEGORY_SKILLS", "Beceriler"),
				},
				{
					"name" : "sound_skills_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 150,
				},
				{
					"name" : "sound_movement_text",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 180,
					"color" : PRIMARY_COLOR,
					"text" : getattr(localeInfo, "SOUND_CATEGORY_MOVEMENT", "Ad\xfdmlar ve binekler"),
				},
				{
					"name" : "sound_movement_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 200,
				},
				{
					"name" : "sound_interface_text",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 230,
					"color" : PRIMARY_COLOR,
					"text" : getattr(localeInfo, "SOUND_CATEGORY_INTERFACE", "Aray\xfcz"),
				},
				{
					"name" : "sound_interface_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 250,
				},
				{
					"name" : "sound_voices_text",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 280,
					"color" : PRIMARY_COLOR,
					"text" : getattr(localeInfo, "SOUND_CATEGORY_VOICES", "Karakter sesleri"),
				},
				{
					"name" : "sound_voices_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 300,
				},
				{
					"name" : "sound_monsters_text",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 330,
					"color" : PRIMARY_COLOR,
					"text" : getattr(localeInfo, "SOUND_CATEGORY_MONSTERS", "Canavar sesleri"),
				},
				{
					"name" : "sound_monsters_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 350,
				},
			),
		},

	),
}

import advanced_options_reference
advanced_options_reference.ApplyLayout(window, 3)
