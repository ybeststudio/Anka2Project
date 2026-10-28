import uiScriptLocale, localeInfo

IMG_DIR = "d:/ymir work/ui/game/advanced_game_options/"

TITLE_IMAGE_TEXT_X = 58
TITLE_IMAGE_TEXT_Y = 5

OPTION_START_X = 17
SLIDER_POSITION_X = 50

SLIDER_START_Y = 40
BUTTON_START_Y = 33
BUTTON_NEXT_Y = 20

RADIO_BUTTON_RANGE_X = 80
RADIO_BUTTON_LARGE_RANGE_X = 90
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
			"name" : "pvp_mode_window",
			"type" : "window",
			"x" : 0,
			"y" : 5,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "pvp_mode",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_pvp",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : uiScriptLocale.OPTION_PVPMODE,
						},
					),
				},
				{
					"name" : "pvp_peace",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_PVPMODE_PEACE,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "pvp_revenge",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_PVPMODE_REVENGE,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "pvp_guild",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_PVPMODE_GUILD,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "pvp_free",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*3,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_PVPMODE_FREE,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
			),
		},
		{
			"name" : "buffi_settings_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT,
			"width" : 304,
			"height" : 105,
			"children" :
			(
				{
					"name" : "buffi_settings",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR + "option_title.png",
					"children" :
					(
						{
							"name" : "title_buffi_settings",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : PRIMARY_COLOR,
							"outline" : True,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_BUFFI_SETTINGS,
						},
					),
				},
				{
					"name" : "auto_unequip_buffi_pvp_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "auto_unequip_buffi_pvp_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color" : PRIMARY_COLOR,
							"text" : localeInfo.GAME_OPTION_AUTO_UNEQUIP_BUFFI_PVP,
						},
						{
							"name" : "auto_unequip_buffi_pvp_desc",
							"type" : "adjust_text",
							"max_width" : 245,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"color" : SECONDARY_COLOR,
							"text" : localeInfo.GAME_OPTION_AUTO_UNEQUIP_BUFFI_PVP_DESC,
						},
					),
				},
				{
					"name" : "auto_equip_buffi_pvm_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 73,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "auto_equip_buffi_pvm_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color" : PRIMARY_COLOR,
							"text" : localeInfo.GAME_OPTION_AUTO_EQUIP_BUFFI_PVM,
						},
						{
							"name" : "auto_equip_buffi_pvm_desc",
							"type" : "adjust_text",
							"max_width" : 245,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"color" : SECONDARY_COLOR,
							"text" : localeInfo.GAME_OPTION_AUTO_EQUIP_BUFFI_PVM_DESC,
						},
					),
				},
			),
	},


		{
			"name" : "shop_range_window",
			"type" : "window",
			"x" : 0,
			"y" : 723,
			"width":304,
			"height":SLIDER_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "shop_range_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_shop_range",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": PRIMARY_COLOR,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.IKASHOP_PRO_SHOP_RANGE_VIEW_HEADER,
						},
					),
				},
				{
					"name" : "shop_range_slider",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 55,
				},
				{
					"name" : "shop_range_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.IKASHOP_PRO_SHOP_RANGE_VIEW_HEADER,
				},
			),
		},


		{
			"name" : "block_mode_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "block_mode_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_block",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : uiScriptLocale.OPTION_BLOCK,
						},
					),
				},
				{
					"name" : "block_exchange_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_BLOCK_EXCHANGE,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "block_party_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_BLOCK_PARTY,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "block_guild_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_BLOCK_GUILD,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "block_whisper_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*3,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_BLOCK_WHISPER,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "block_friend_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33+20,
					"text" : uiScriptLocale.OPTION_BLOCK_FRIEND,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "block_party_request_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33+20,
					"text" : uiScriptLocale.OPTION_BLOCK_PARTY_REQUEST,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
			),
		},

		{
			"name" : "names_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT+NORMAL_OPTION_HEIGHT,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT+65,
			"children":
			(
				{
					"name" : "names_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_names",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENTITY_NAME,
						},
					),
				},
				{
					"name" : "names_far_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "names_far_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_ENTITY_NAME,
						},
						{
							"name" : "names_far_toggle2_button_description",
							"type" : "adjust_text",
                            "max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENTITY_NAME_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},

				{
					"name" : "names_monsters_fov_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 72,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "names_monsters_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_MONSTER_NAME,
						},
						{
							"name" : "names_monsters_toggle2_button_description",
							"type" : "adjust_text",
                            "max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_MONSTER_NAME_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},

				{
					"name" : "show_country_flag_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 111,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "show_country_flag_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : "Country Flags",
						},
						{
							"name" : "show_country_flag_toggle_description",
							"type" : "adjust_text",
							"max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : "Show or hide country flags above player names.",
							"color" : SECONDARY_COLOR,
						},			
					),
				},	

			),
		},

		{
			"name" : "damage_window",
			"type" : "window",
			"x" : 0,
			"y" : 295,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT*2+108,
			"children":
			(
				{
					"name" : "show_damage_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_show_damage",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_DMG,
						},
					),
				},
                
				{
					"name" : "subtitle_damage_1",
					"type" : "text",
					"x" : 17,
					"y" : 34,
					"color": 0xFFcebb98,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_SIZE_DMG,
				},

				{
					"name" : "size_damage",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y+13,
				},

				{
					"name" : "subtitle_damage_spacing",
					"type" : "text",
					"x" : 17,
					"y" : 68,
					"color": 0xFFcebb98,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_DAMAGE_SPACING,
				},

				{
					"name" : "damage_spacing",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y+47,
				},

				{
					"name" : "damage_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 112,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "damage_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_DMG,
						},
						{
							"name" : "damage_toggle2_button_description",
							"type" : "adjust_text",
                            "max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_DMG_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	

				{
					"name" : "damage_monster_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 151,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "damage_monster_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_DMG_MONSTER,
						},
						{
							"name" : "damage_monster_toggle2_button_description",
							"type" : "adjust_text",
                            "max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_DMG_MONSTER_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	

				{
					"name" : "colored_damage_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 190,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "colored_damage_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_COLORED_DAMAGE,
						},
						{
							"name" : "colored_damage_toggle2_button_description",
							"type" : "adjust_text",
                            "max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_COLORED_DAMAGE_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	

				{
					"name" : "colored_damage_color_button",
					"type" : "button",
					"x" : OPTION_START_X,
					"y" : 225,
					"text_color" : 0xFFcebb98,
					"text" : localeInfo.GAME_OPTION_DAMAGE_COLORS_OPEN,
					"outline" : True,
					"default_image" : IMG_DIR + "btn_0.png",
					"over_image" : IMG_DIR + "btn_1.png",
					"down_image" : IMG_DIR + "btn_2.png",
				},

			),
		},

		{
			"name" : "stone_scale_window",
			"type" : "window",
			"x" : 0,
			"y" : 568,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "stone_scale_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_stone_scale",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SIZE_STONE,
						},
					),
				},
				{
					"name" : "stones_scale_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 55,
				},
				{
					"name" : "stone_scale_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_SIZE_STONE,
				},

			),
		},
        
		{
			"name" : "hide_companion_window",
			"type" : "window",
			"x" : 0,
			"y" : 653,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "hide_companion",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_hide_companion",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_HIDE_COMPANION,
						},
					),
				},
				{
					"name" : "hide_companion_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : "Hide Companion",
				},
				{
					"name" : "hide_companion_dropdown_anchor",
					"type" : "window",
					"x" : OPTION_START_X,
					"y" : 55,
					"width" : 1,
					"height" : 1,
				},
				{
					"name" : "hide_mount",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_LARGE_RANGE_X*0,
					"y" : 33,
					"text" : localeInfo.GAME_OPTION_HIDE_COMPANION_MOUNT,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "hide_pet",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_LARGE_RANGE_X*1,
					"y" : 33,
					"text" : localeInfo.GAME_OPTION_HIDE_COMPANION_PET,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "hide_buffi",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_LARGE_RANGE_X*2,
					"y" : 33,
					"text" : localeInfo.GAME_OPTION_HIDE_COMPANION_BUFFI,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
			),
		},

		{
			"name" : "queue_metin_window",
			"type" : "window",
			"x" : 0,
			"y" : 793,
			"width":304,
			"height":175,
			"children":
			(
				{
					"name" : "queue_metin_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_queue_metin",
							"type" : "text",
							"color": 0xFFcabdab,
							"outline": True,
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_QUEUE_METIN_EFFECT,
						},
					),
				},
				{
					"name" : "queue_effect_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_EFFECT,
				},
				{
					"name" : "queue_color_button",
					"type" : "button",
					"x" : OPTION_START_X,
					"y" : 103,
					"text_color" : 0xFFcebb98,
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_COLOR_OPEN,
					"outline" : True,
					"default_image" : IMG_DIR + "btn_0.png",
					"over_image" : IMG_DIR + "btn_1.png",
					"down_image" : IMG_DIR + "btn_2.png",
				},
				{
					"name" : "effect_1",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 55,
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_EFFECT_OPTION.format(1),
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "effect_2",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 55,
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_EFFECT_OPTION.format(2),
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "effect_3",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 55,
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_EFFECT_OPTION.format(3),
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "queue_color_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 80,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_COLOR,
				},
				{
					"name" : "queue_flash_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 138,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "queue_flash_toggle_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color": 0xFFcebb98,
							"text_horizontal_align":"left",
							"text" : "Queue-Metin End Flash",
						},
						{
							"name" : "queue_flash_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : "Activate Windows Flash when the queue metin finish.",
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},

		{
			"name" : "mining_flash_window",
			"type" : "window",
			"x" : 0,
			"y" : 968,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "mining_flash_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_mining_flash",
							"type" : "text",
							"color": 0xFFcabdab,
							"outline": True,
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_MINING_FLASH,
						},
					),
				},
				{
					"name" : "mining_finish_flash_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "mining_finish_flash_toggle_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color": 0xFFcebb98,
							"text_horizontal_align":"left",
							"text" : "Mining End Flash",
						},
						{
							"name" : "mining_finish_flash_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : "Activate Windows Flash when the mining finish.",
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},

		{
			"name" : "event_stone_window",
			"type" : "window",
			"x" : 0,
			"y" : 1038,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "event_stone_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_event_stone",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_EVENT_STONE_APPEARANCE,
						},
					),
				},
				{
					"name" : "event_stone_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_EVENT_STONE_APPEARANCE,
				},
				{
					"name" : "event_stone_original", "type" : "radio_button",
					"x" : OPTION_START_X, "y" : 55,
					"text" : localeInfo.GAME_OPTION_EVENT_STONE_ORIGINAL, "text_x" : RADIO_BUTTON_TEXT_X,
					"text_color" : 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png", "over_image" : IMG_DIR + "radio_unselected.png", "down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "event_stone_model_1", "type" : "radio_button",
					"x" : OPTION_START_X + RADIO_BUTTON_RANGE_X, "y" : 55,
					"text" : "Model 1", "text_x" : RADIO_BUTTON_TEXT_X, "text_color" : 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png", "over_image" : IMG_DIR + "radio_unselected.png", "down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "event_stone_model_2", "type" : "radio_button",
					"x" : OPTION_START_X + RADIO_BUTTON_RANGE_X * 2, "y" : 55,
					"text" : "Model 2", "text_x" : RADIO_BUTTON_TEXT_X, "text_color" : 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png", "over_image" : IMG_DIR + "radio_unselected.png", "down_image" : IMG_DIR + "radio_selected.png",
				},
			),
		},

		{
			"name" : "hide_effects_except_damage_window",
			"type" : "window",
			"x" : 0,
			"y" : 1103,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "hide_effects_except_damage_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_hide_effects_except_damage",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_HIDE_EFFECTS_EXCEPT_DAMAGE,
						},
					),
				},
				{
					"name" : "hide_effects_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_HIDE_EFFECTS_EXCEPT_DAMAGE,
				},
				{
					"name" : "hide_effects_dropdown_anchor",
					"type" : "window",
					"x" : OPTION_START_X,
					"y" : 55,
					"width" : 1,
					"height" : 1,
				},
				{
					"name" : "hide_effects_except_damage_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 31,
					"text" : localeInfo.GAME_OPTION_HIDE_EFFECTS_ALL,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "hide_effects_from_others_players_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+95,
					"y" : 31,
					"text" : localeInfo.GAME_OPTION_HIDE_EFFECTS_OTHERS,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "hide_shaman_buff_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 53,
					"text" : localeInfo.GAME_OPTION_HIDE_EFFECTS_SHAMAN_BUFF,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "hide_stone_effect_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 75,
					"text" : localeInfo.GAME_OPTION_HIDE_EFFECTS_STONE,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "hide_monster_effect_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 97,
					"text" : localeInfo.GAME_OPTION_HIDE_EFFECTS_MONSTER,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "hide_exp_effect_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 119,
					"text" : localeInfo.GAME_OPTION_HIDE_EFFECTS_EXP,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
			),
		},

		{
			"name" : "show_guild_grade_window",
			"type" : "window",
			"x" : 0,
			"y" : 1168,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "show_guild_grade_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_show_guild_grade",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUILD_GRADE,
						},
					),
				},
				{
					"name" : "show_guild_grade_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "show_guild_grade_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_GUILD_GRADE,
						},
						{
							"name" : "show_guild_grade_desc",
							"type" : "adjust_text",
							"max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUILD_GRADE_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},

		{
			"name" : "notification_settings_window",
			"type" : "window",
			"x" : 0,
			"y" : 1235,
			"width" : 304,
			"height" : SMALL_OPTION_HEIGHT,
			"children" :
			(
				{
					"name" : "notification_settings_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR + "option_title.png",
					"children" :
					(
						{
							"name" : "title_notification_settings",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : 0xFFcabdab,
							"outline" : True,
							"text_horizontal_align" : "left",
							"text" : getattr(localeInfo, "GAME_OPTION_NOTIFICATION_SETTINGS", "Notification Settings"),
						},
					),
				},
				{
					"name" : "notification_settings_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color" : 0xFFcebb98,
					"outline" : True,
					"text_horizontal_align" : "left",
					"text" : getattr(localeInfo, "GAME_OPTION_HIDDEN_NOTIFICATIONS", "Hidden Notifications"),
				},
			),
		},

			{
				"name" : "notification_history_button_window",
			"type" : "window",
			"x" : 0,
			"y" : 1300,
			"width" : 304,
			"height" : NORMAL_OPTION_HEIGHT,
			"children" :
			(
				{
					"name" : "notification_history_button_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR + "option_title.png",
					"children" :
					(
						{
							"name" : "title_notification_history_button",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : 0xFFcabdab,
							"outline" : True,
							"text_horizontal_align" : "left",
							"text" : getattr(localeInfo, "GAME_OPTION_NOTIFICATION_HISTORY", "Notification History"),
						},
					),
				},
				{
					"name" : "view_notification_history_button_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "notification_history_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align" : "left",
							"color" : 0xFFcebb98,
							"text" : getattr(localeInfo, "GAME_OPTION_NOTIFICATION_HISTORY", "Notification History"),
						},
						{
							"name" : "notification_history_button_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align" : "left",
							"text" : getattr(localeInfo, "GAME_OPTION_NOTIFICATION_HISTORY_DESC", "Show or hide the notification history button in the game button list."),
							"color" : SECONDARY_COLOR,
						},
					),
				},
				),
			},

			{
				"name" : "auto_cape_after_warp_window",
				"type" : "window",
				"x" : 0,
				"y" : 1385,
				"width" : 304,
				"height" : NORMAL_OPTION_HEIGHT,
				"children" :
				(
					{
						"name" : "auto_cape_after_warp_title_img",
						"type" : "expanded_image",
						"x" : 5,
						"y" : 0,
						"image" : IMG_DIR + "option_title.png",
						"children" :
						(
							{
								"name" : "title_auto_cape_after_warp",
								"type" : "text",
								"x" : TITLE_IMAGE_TEXT_X,
								"y" : TITLE_IMAGE_TEXT_Y,
								"color" : PRIMARY_COLOR,
								"outline" : True,
								"text_horizontal_align" : "left",
								"text" : localeInfo.GAME_OPTION_AUTO_CAPE_AFTER_WARP,
							},
						),
					},
					{
						"name" : "auto_cape_after_warp_on_off_btn",
						"type" : "toggle_button",
						"x" : OPTION_START_X,
						"y" : 33,
						"default_image" : IMG_DIR + "toggle_unselected.png",
						"over_image" : IMG_DIR + "toggle_unselected.png",
						"down_image" : IMG_DIR + "toggle_selected.png",
						"children" :
						(
							{
								"name" : "auto_cape_after_warp_button_title",
								"type" : "text",
								"x" : TOGGLE_BUTTON_TEXT_X,
								"y" : -2,
								"text_horizontal_align" : "left",
								"color" : 0xFFcebb98,
								"text" : localeInfo.GAME_OPTION_AUTO_CAPE_AFTER_WARP,
							},
							{
								"name" : "auto_cape_after_warp_button_description",
								"type" : "adjust_text",
								"max_width" : 285,
								"x" : TOGGLE_BUTTON_TEXT_X,
								"y" : 13,
								"text_horizontal_align" : "left",
								"text" : localeInfo.GAME_OPTION_AUTO_CAPE_AFTER_WARP_DESC,
								"color" : SECONDARY_COLOR,
							},
						),
					},
				),
			},

			{
				"name" : "disable_level_up_confirmation_window",
				"type" : "window",
				"x" : 0,
				"y" : 1465,
				"width" : 304,
				"height" : NORMAL_OPTION_HEIGHT,
				"children" :
				(
					{
						"name" : "disable_level_up_confirmation_title_img",
						"type" : "expanded_image",
						"x" : 5,
						"y" : 0,
						"image" : IMG_DIR + "option_title.png",
						"children" :
						(
							{
								"name" : "title_disable_level_up_confirmation",
								"type" : "text",
								"x" : TITLE_IMAGE_TEXT_X,
								"y" : TITLE_IMAGE_TEXT_Y,
								"color" : PRIMARY_COLOR,
								"outline" : True,
								"text_horizontal_align" : "left",
								"text" : localeInfo.GAME_OPTION_DISABLE_LEVEL_UP_CONFIRMATION,
							},
						),
					},
					{
						"name" : "disable_level_up_confirmation_on_off_btn",
						"type" : "toggle_button",
						"x" : OPTION_START_X,
						"y" : 33,
						"default_image" : IMG_DIR + "toggle_unselected.png",
						"over_image" : IMG_DIR + "toggle_unselected.png",
						"down_image" : IMG_DIR + "toggle_selected.png",
						"children" :
						(
							{
								"name" : "disable_level_up_confirmation_button_title",
								"type" : "text",
								"x" : TOGGLE_BUTTON_TEXT_X,
								"y" : -2,
								"color" : 0xFFcebb98,
								"text" : localeInfo.GAME_OPTION_DISABLE_LEVEL_UP_CONFIRMATION,
							},
							{
								"name" : "disable_level_up_confirmation_button_description",
								"type" : "adjust_text",
								"max_width" : 245,
								"x" : TOGGLE_BUTTON_TEXT_X,
								"y" : 13,
								"color" : SECONDARY_COLOR,
								"text" : localeInfo.GAME_OPTION_DISABLE_LEVEL_UP_CONFIRMATION_DESC,
							},
						),
					},
				),
			},

			{
				"name" : "hide_metin_spawns_window",
				"type" : "window",
				"x" : 0,
				"y" : 1465,
				"width" : 304,
				"height" : 94,
				"children" :
				(
					{
						"name" : "hide_metin_spawns_img",
						"type" : "expanded_image",
						"x" : 0,
						"y" : 0,
						"image" : IMG_DIR + "option_title.png",
						"children" :
						(
							{
								"name" : "hide_metin_spawns_name",
								"type" : "text",
								"x" : TITLE_IMAGE_TEXT_X,
								"y" : TITLE_IMAGE_TEXT_Y,
								"color" : 0xFFcabdab,
								"outline" : True,
								"text_horizontal_align" : "left",
								"text" : localeInfo.GAME_OPTION_HIDE_METIN_SPAWNS_TITLE,
							},
						),
					},
					{
						"name" : "hide_metin_spawns",
						"type" : "toggle_button",
						"x" : OPTION_START_X,
						"y" : 34,
						"default_image" : IMG_DIR + "toggle_unselected.png",
						"over_image" : IMG_DIR + "toggle_unselected.png",
						"down_image" : IMG_DIR + "toggle_selected.png",
						"children" :
						(
							{
								"name" : "hide_metin_spawns_text",
								"type" : "text",
								"x" : TOGGLE_BUTTON_TEXT_X,
								"y" : 0,
								"text_horizontal_align" : "left",
								"color" : 0xFFcebb98,
								"text" : localeInfo.GAME_OPTION_HIDE_METIN_SPAWNS,
							},
						),
					},
					{
						"name" : "hide_metin_spawns_description",
						"type" : "adjust_text",
						"max_width" : 240,
						"x" : OPTION_START_X + TOGGLE_BUTTON_TEXT_X,
						"y" : 54,
						"text_horizontal_align" : "left",
						"color" : SECONDARY_COLOR,
						"text" : localeInfo.GAME_OPTION_HIDE_METIN_SPAWNS_DESC,
					},
				),
			},

			{
				"name" : "auto_hide_companion_map1_window",
				"type" : "window",
				"x" : 0,
				"y" : 1465,
				"width" : 304,
				"height" : SMALL_OPTION_HEIGHT,
				"children" :
				(
					{
						"name" : "auto_hide_companion_map1_img",
						"type" : "expanded_image",
						"x" : 0,
						"y" : 0,
						"image" : IMG_DIR + "option_title.png",
						"children" :
						(
							{
								"name" : "auto_hide_companion_map1_name",
								"type" : "text",
								"x" : TITLE_IMAGE_TEXT_X,
								"y" : TITLE_IMAGE_TEXT_Y,
								"color" : 0xFFcabdab,
								"outline" : True,
								"text_horizontal_align" : "left",
								"text" : localeInfo.GAME_OPTION_AUTO_HIDE_COMPANION_MAP1_TITLE,
							},
						),
					},
					{
						"name" : "auto_hide_companion_map1",
						"type" : "toggle_button",
						"x" : OPTION_START_X,
						"y" : 34,
						"default_image" : IMG_DIR + "toggle_unselected.png",
						"over_image" : IMG_DIR + "toggle_unselected.png",
						"down_image" : IMG_DIR + "toggle_selected.png",
						"children" :
						(
							{
								"name" : "auto_hide_companion_map1_text",
								"type" : "text",
								"x" : TOGGLE_BUTTON_TEXT_X,
								"y" : 0,
								"text_horizontal_align" : "left",
								"color" : 0xFFcebb98,
								"text" : localeInfo.GAME_OPTION_AUTO_HIDE_COMPANION_MAP1,
							},
							{
								"name" : "auto_hide_companion_map1_description",
								"type" : "adjust_text",
								"max_width" : 265,
								"x" : TOGGLE_BUTTON_TEXT_X,
								"y" : 15,
								"text_horizontal_align" : "left",
								"color" : SECONDARY_COLOR,
								"text" : localeInfo.GAME_OPTION_AUTO_HIDE_COMPANION_MAP1_DESC,
							},
						),
					},
				),
			},

		),
	}

import advanced_options_reference
advanced_options_reference.ApplyLayout(window, 1)
