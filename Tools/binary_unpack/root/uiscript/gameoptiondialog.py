if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import localeInfo
import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/option/"

window = {
	"name" : "GameOptionDialog", "style" : ["movable", "float",],
	"x" : 0, "y" : 0,
	"width" : 535, "height" : 380,

	"children" :
	(
		{
			"name" : "board", "type" : "board",
			"x" : 0, "y" : 0,
			"width" : 535, "height" : 380,

			"children" :
			(
				{
					"name" : "titlebar", "type" : "titlebar", "style" : ["attach",],

					"x" : 8, "y" : 8,
					"width" : 520, "color" : "gray",

					"children" :
					[
						{ "name":"titlename", "type":"text", "x":0, "y":3, "text" : uiScriptLocale.GAMEOPTION_TITLE, "horizontal_align":"center", "text_horizontal_align":"center" },
					],
				},
				{
					"name" : "bg", "type" : "image",
					"x" : 15, "y" : 60,
					"image" : ROOT_PATH + "bg_board.tga",
				},
				{
					"name" : "background_game",
					"x" : 15, "y" : 60,
					"width" : 500, "height" : 466,

					"children" :
					(
						{
							"name" : "character0", "type" : "radio_button",
							"x" : 10, "y" : 17,

							"text" : localeInfo.OPTION_GAME_CHARACTER,

							"default_image" : ROOT_PATH + "board_button_main.tga",
							"over_image" : ROOT_PATH + "board_button_over.tga",
							"down_image" : ROOT_PATH + "board_button_down.tga",
						},
						{
							"name" : "char_opt_board",
							"x" : 190, "y" : 17,
							"width" : 500, "height" : 466,

							"children" :
							(

								{
									"name" : "always_show_name", "type" : "text",
									"x" : 10, "y" : 2,
									"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME,
								},
								{
									"name" : "always_show_name_on_button", "type" : "radio_button",
									"x" : 105, "y" : 0,

									"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_ON,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "always_show_name_off_button", "type" : "radio_button",
									"x" : 170, "y" : 0,

									"text" : uiScriptLocale.OPTION_ALWAYS_SHOW_NAME_OFF,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "name_color", "type" : "text",
									"x" : 10, "y" : 25+2,

									"text" : uiScriptLocale.OPTION_NAME_COLOR,
								},
								{
									"name" : "name_color_normal", "type" : "radio_button",
									"x" : 105, "y" : 25,

									"text" : uiScriptLocale.OPTION_NAME_COLOR_NORMAL,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "name_color_empire", "type" : "radio_button",
									"x" : 170, "y" : 25,

									"text" : uiScriptLocale.OPTION_NAME_COLOR_EMPIRE,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "name_outline", "type" : "text",
									"x" : 10, "y" : 50+2,

									"text" : uiScriptLocale.OPTION_NAME_OUTLINE,
								},
								{
									"name" : "name_outline_on", "type" : "radio_button",
									"x" : 105, "y" : 50,

									"text" : uiScriptLocale.OPTION_NAME_OUTLINE_ON,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "name_outline_off", "type" : "radio_button",
									"x" : 170, "y" : 50,

									"text" : uiScriptLocale.OPTION_NAME_OUTLINE_OFF,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "pvp_mode", "type" : "text",
									"x" : 10, "y" : 75+2,

									"text" : uiScriptLocale.OPTION_PVPMODE,
								},
								{
									"name" : "pvp_peace", "type" : "radio_button",

									"x" : 105, "y" : 75,

									"text" : uiScriptLocale.OPTION_PVPMODE_PEACE,
									"tooltip_text" : uiScriptLocale.OPTION_PVPMODE_PEACE_TOOLTIP,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "pvp_revenge", "type" : "radio_button",
									"x" : 157, "y" : 75,

									"text" : uiScriptLocale.OPTION_PVPMODE_REVENGE,
									"tooltip_text" : uiScriptLocale.OPTION_PVPMODE_REVENGE_TOOLTIP,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "pvp_guild", "type" : "radio_button",
									"x" : 210, "y" : 75,

									"text" : uiScriptLocale.OPTION_PVPMODE_GUILD,
									"tooltip_text" : uiScriptLocale.OPTION_PVPMODE_GUILD_TOOLTIP,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "pvp_free", "type" : "radio_button",
									"x" : 262, "y" : 75,

									"text" : uiScriptLocale.OPTION_PVPMODE_FREE,
									"tooltip_text" : uiScriptLocale.OPTION_PVPMODE_FREE_TOOLTIP,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "block", "type" : "text",
									"x" : 10, "y" : 100+2,

									"text" : uiScriptLocale.OPTION_BLOCK,
								},
								{
									"name" : "block_exchange_button", "type" : "toggle_button",
									"x" : 105, "y" : 100,

									"text" : uiScriptLocale.OPTION_BLOCK_EXCHANGE,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "block_party_button", "type" : "toggle_button",
									"x" : 170, "y" : 100,

									"text" : uiScriptLocale.OPTION_BLOCK_PARTY,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "block_guild_button", "type" : "toggle_button",
									"x" : 235, "y" : 100,

									"text" : uiScriptLocale.OPTION_BLOCK_GUILD,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "block_whisper_button", "type" : "toggle_button",
									"x" : 105, "y" : 125,

									"text" : uiScriptLocale.OPTION_BLOCK_WHISPER,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "block_friend_button", "type" : "toggle_button",
									"x" : 170, "y" : 125,

									"text" : uiScriptLocale.OPTION_BLOCK_FRIEND,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "block_party_request_button", "type" : "toggle_button",
									"x" : 235, "y" : 125,

									"text" : uiScriptLocale.OPTION_BLOCK_PARTY_REQUEST,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "block_warp_request_button", "type" : "toggle_button",
									"x" : 105, "y" : 150,

									"text" : uiScriptLocale.OPTION_TELEPORT,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
							),
						},
						{
							"name" : "ui_opt_board",
							"x" : 190, "y" : 17,
							"width" : 500, "height" : 466,

							"children" :
							(
								{
									"name" : "target_board", "type" : "text",
									"x" : 10, "y" : 2,
									"text" : uiScriptLocale.OPTION_TARGET_BOARD,
								},
								{
									"name" : "target_board_no_view", "type" : "radio_button",
									"x" : 105, "y" : 0,

									"text" : uiScriptLocale.OPTION_TARGET_BOARD_NO_VIEW,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "target_board_view", "type" : "radio_button",
									"x" : 170, "y" : 0,

									"text" : uiScriptLocale.OPTION_TARGET_BOARD_VIEW,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "chat", "type" : "text",
									"x" : 10, "y" : 25+2,
									"text" : uiScriptLocale.OPTION_VIEW_CHAT,
								},
								{
									"name" : "view_chat_on_button", "type" : "radio_button",
									"x" : 105, "y" : 25,

									"text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "view_chat_off_button", "type" : "radio_button",
									"x" : 170, "y" : 25,

									"text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "effect_on_off", "type" : "text",
									"x" : 10, "y" : 50+2,
									"text" : uiScriptLocale.OPTION_EFFECT,
								},
								{
									"name" : "show_damage_on_button", "type" : "radio_button",
									"x" : 105, "y" : 50,

									"text" : uiScriptLocale.OPTION_VIEW_CHAT_ON,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "show_damage_off_button", "type" : "radio_button",
									"x" : 170, "y" : 50,

									"text" : uiScriptLocale.OPTION_VIEW_CHAT_OFF,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "salestext_on_off", "type" : "text",
									"x" : 10, "y" : 75+2,
									"text" : uiScriptLocale.OPTION_SALESTEXT,
								},
								{
									"name" : "salestext_on_button", "type" : "radio_button",
									"x" : 105, "y" : 75,

									"text" : uiScriptLocale.OPTION_SALESTEXT_VIEW_OFF,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "salestext_off_button", "type" : "radio_button",
									"x" : 170, "y" : 75,

									"text" : uiScriptLocale.OPTION_SALESTEXT_VIEW_ON,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "language_change_window", "type" : "window",
									"x" : -30, "y" : 90,
									"width" : 305, "height" : 22+110,

									"children" :
									(
										{
											"name" : "language_select_img", "type" : "image",
											"x" : 40, "y" : 20,
											"image" : "d:/ymir work/ui/quest_re/button_one.sub",
										},
										{
											"name" : "cur_language_text_window", "type" : "window",
											"x" : 40, "y" : 20,
											"width" : 210-16, "height" : 16,

											"children" :
											(
												{
													"name" : "cur_language_text", "type" : "text",
													"x" : 0, "y" : 0,
													"text" : "-", "all_align" : "center"
												},
											),
										},
										{
											"name" : "language_select_button", "type" : "button",
											"x" : 40 + 210 -16, "y" : 20,

											"default_image" : "d:/ymir work/ui/game/party_match/arrow_default.sub",
											"over_image" : "d:/ymir work/ui/game/party_match/arrow_over.sub",
											"down_image" : "d:/ymir work/ui/game/party_match/arrow_down.sub",
										},
										{
											"name" : "language_change_button", "type" : "button",
											"x" : 40 + 210 + 10, "y" : 20,

											"text" : uiScriptLocale.GRAPHICONOFF_EFFECT_APPLY,

											"default_image" : ROOT_PATH + "option_button_thin_norm.tga",
											"over_image" : ROOT_PATH + "option_button_thin_hover.tga",
											"down_image" : ROOT_PATH + "option_button_thin_down.tga",
											"disable_image" : ROOT_PATH + "option_button_thin_down.tga",
										},
										{
											"name" : "language_select_pivot_window", "type" : "window",
											"x" : 40, "y" : 16+20,
											"width" : 210, "height" : 0,
										},
										{
											"name" : "mouse_over_image", "type" : "expanded_image", "style" : ("not_pick",),
											"x" : 0 + 10, "y" : 0 + 32,
											"image" : "d:/ymir work/ui/quest_re/button_over.sub",
										},
									),
								},
							),
						},
						{
							"name" : "ambient_opt_board",
							"x" : 190, "y" : 17,
							"width" : 500, "height" : 466,

							"children" :
							(
								{
									"name" : "fog_mode", "type" : "text",
									"x" : 10, "y" : 2,
									"text" : uiScriptLocale.OPTION_FOG,
								},
								{
									"name" : "fog_on", "type" : "radio_button",
									"x" : 105, "y" : 0,

									"text" : uiScriptLocale.OPTION_FOG_ON,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "fog_off", "type" : "radio_button",
									"x" : 170, "y" : 0,

									"text" : uiScriptLocale.OPTION_FOG_OFF,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "night_mode", "type" : "text",
									"x" : 10, "y" : 25+2,
									"text" : uiScriptLocale.OPTION_NIGHT_MODE,
								},
								{
									"name" : "night_mode_on", "type" : "radio_button",
									"x" : 105, "y" : 25,

									"text" : uiScriptLocale.OPTION_NIGHT_MODE_ON,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "night_mode_off", "type" : "radio_button",
									"x" : 170, "y" : 25,

									"text" : uiScriptLocale.OPTION_NIGHT_MODE_OFF,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "snow_mode", "type" : "text",
									"x" : 10, "y" : 50+2,
									"text" : uiScriptLocale.OPTION_SNOW_MODE,
								},
								{
									"name" : "snow_mode_on", "type" : "radio_button",
									"x" : 105, "y" : 50,

									"text" : uiScriptLocale.OPTION_SNOW_MODE_ON,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "snow_mode_off", "type" : "radio_button",
									"x" : 170, "y" : 50,

									"text" : uiScriptLocale.OPTION_SNOW_MODE_OFF,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
							),
						},
						{
							"name" : "pickup_opt_board",
							"x" : 190, "y" : 17,
							"width" : 500, "height" : 466,

							"children" :
							(
								{
									"name" : "pickup_premimum_image", "type" : "image",
									"x" : -13, "y" : 0,
									"image" : "d:/ymir work/ui/game/option/info_bg.tga",

									"children" :
									(
										{
											"name" : "pickup_premium_status", "type" : "text",
											"x" : 60, "y" : 6, "text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_STATUS, "fontname" : "Verdana:17",
											"color" : 0xFFC1C1C1, "outline" : 1,
										},
										{
											"name" : "pickup_premium_status_text", "type" : "text",
											"x" : 112, "y" : 6, "text" : "", "fontname" : "Verdana:17",
											"color" : 0xFFC1C1C1, "outline" : 1,
										},
										{
											"name" : "pickup_premium_info", "type" : "text",
											"x" : 60, "y" : 28, "text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_INFO, "fontname" : "Verdana:13",
											"color" : 0xFFFFA900, "outline" : 1,
										},
										{
											"name" : "pickup_premium_info_benefit_1", "type" : "text",
											"x" : 60, "y" : 43, "text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_INFO_BENEFIT_1, "fontname" : "Verdana:12",
											"color" : 0xFFC1C1C1, "outline" : 1,
										},
										{
											"name" : "pickup_premium_info_benefit_2", "type" : "text",
											"x" : 60, "y" : 56, "text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_INFO_BENEFIT_2, "fontname" : "Verdana:12",
											"color" : 0xFFC1C1C1, "outline" : 1,
										},
									)
								},
								{
									"name" : "pickup_title", "type" : "image",
									"x" : -13, "y" : 90,
									"image" : ROOT_PATH + "category_bar.tga",

									"children" :
									(
										{
											"name" : "pickup_title_text", "type" : "text",
											"x" : 155, "y" : 5, "text_horizontal_align":"center",
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_TITLE,
										},
									)
								},
								{
									"name" : "pick_up_button_activate", "type" : "toggle_button",
									"x" : 5, "y" : 130,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "pick_up_activate", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_ENABLED,
										},
									)
								},
								{
									"name" : "pick_up_button_deactivate", "type" : "toggle_button",
									"x" : 100, "y" : 130,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "pick_up_deactivate", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_DISABLED,
										},
									)
								},
								{
									"name" : "pickup_filtre", "type" : "image",
									"x" : -13, "y" : 160,
									"image" : ROOT_PATH + "category_bar.tga",

									"children" :
									(
										{
											"name" : "pickup_filtre_text", "type" : "text",
											"x" : 155, "y" : 5, "text_horizontal_align":"center",
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER,
										},
									)
								},
								{
									"name" : "pick_up_weapons", "type" : "toggle_button",
									"x" : 5, "y" : 195,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "weapons", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER_WEAPONS,
										},
									)
								},
								{
									"name" : "pick_up_armors", "type" : "toggle_button",
									"x" : 80, "y" : 195,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "armors", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER_ARMOR,
										},
									)
								},
								{
									"name" : "pick_up_shield", "type" : "toggle_button",
									"x" : 165, "y" : 195,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "shield", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER_SHIELD,
										},
									)
								},
								{
									"name" : "pick_up_helmets", "type" : "toggle_button",
									"x" : 235, "y" : 195,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "helmets", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER_HELMET,
										},
									)
								},
								{
									"name" : "pick_up_bracelets", "type" : "toggle_button",
									"x" : 5, "y" : 225,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "bracelets", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER_BRACELET,
										},
									)
								},
								{
									"name" : "pick_up_necklace", "type" : "toggle_button",
									"x" : 80, "y" : 225,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "necklace", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER_NECKLACE,
										},
									)
								},
								{
									"name" : "pick_up_earrings", "type" : "toggle_button",
									"x" : 165, "y" : 225,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "earrings", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER_EARRINGS,
										},
									)
								},
								{
									"name" : "pick_up_shoes", "type" : "toggle_button",
									"x" : 235, "y" : 225,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "shoes", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER_SHOES,
										},
									)
								},
								{
									"name" : "pick_up_yang", "type" : "toggle_button",
									"x" : 5, "y" : 255,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "yang", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER_YANG,
										},
									)
								},
								{
									"name" : "pick_up_chests", "type" : "toggle_button",
									"x" : 80, "y" : 255,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "chests", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER_CHESTS,
										},
									)
								},
								{
									"name" : "pick_up_others", "type" : "toggle_button",
									"x" : 165, "y" : 255,

									"default_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"over_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_unselected.tga",
									"down_image" : "d:/ymir work/ui/game/biolog_manager/checkbox_new_selected.tga",

									"children" :
									(
										{
											"name" : "stones", "type" : "text",
											"x" : 20, "y" : 0,
											"text" : uiScriptLocale.AUTO_PICKUP_PREMIUM_FILTER_OTHERS,
										},
									)
								},
							),
						},
						{
							"name" : "character1", "type" : "radio_button",
							"x" : 10, "y" : 17+38,

							"text" : localeInfo.OPTION_GAME_INTERFACE,

							"default_image" : ROOT_PATH + "board_button_main.tga",
							"over_image" : ROOT_PATH + "board_button_over.tga",
							"down_image" : ROOT_PATH + "board_button_down.tga",
						},
						{
							"name" : "character2", "type" : "radio_button",
							"x" : 10, "y" : 17+38*2,

							"text" : localeInfo.OPTION_GAME_ENVIRONMENT,

							"default_image" : ROOT_PATH + "board_button_main.tga",
							"over_image" : ROOT_PATH + "board_button_over.tga",
							"down_image" : ROOT_PATH + "board_button_down.tga",
						},
						{
							"name" : "character3", "type" : "radio_button",
							"x" : 10, "y" : 17+38*3,

							"text" : localeInfo.OPTION_GAME_PICKUP,

							"default_image" : ROOT_PATH + "board_button_main.tga",
							"over_image" : ROOT_PATH + "board_button_over.tga",
							"down_image" : ROOT_PATH + "board_button_down.tga",
						},
					),
				},
				{
					"name" : "background_graphics", 
					"x" : 15, "y" : 60,
					"width" : 500, "height" : 466,

					"children" :
					(
						{
							"name" : "display_settings_option_board",
							"x" : 190, "y" : 17,
							"width" : 500, "height" : 466,

							"children" :
							(
								{
									"name" : "camera_mode", "type" : "text",
									"x" : 10, "y" : 2,
									"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE,
								},
								{
									"name" : "camera_short", "type" : "radio_button",
									"x" : 105, "y" : 0,

									"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE_SHORT,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "camera_long", "type" : "radio_button",
									"x" : 170, "y" : 0,

									"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE_LONG,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "enbMode_status", "type" : "text",
									"x" : 10, "y" : 25+2,
									"text" : uiScriptLocale.OPTION_ENB_MODE,
								},
								{
									"name" : "enbMode_on", "type" : "radio_button",
									"x" : 105, "y" : 25,

									"text" : uiScriptLocale.OPTION_ENB_MODE_ON,
									"tooltip_text" : uiScriptLocale.OPTION_ENB_MODE_ON_TOOLTIP,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "enbMode_off", "type" : "radio_button",
									"x" : 170, "y" : 25,

									"text" : uiScriptLocale.OPTION_ENB_MODE_OFF,
									"tooltip_text" : uiScriptLocale.OPTION_ENB_MODE_OFF_TOOLTIP,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "effect_level", "type" : "text",
									"x" : 10, "y" : 50+2,
									"text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL, 
								},
								{
									"name" : "effect_level1", "type" : "radio_button",
									"x" : 105, "y" : 50,

									"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL1,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL1_TOOLTIP,

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "effect_level2", "type" : "radio_button",
									"x" : 129, "y" : 50,

									"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL2,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL2_TOOLTIP,

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "effect_level3", "type" : "radio_button",
									"x" : 153, "y" : 50,

									"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL3,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL3_TOOLTIP,

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "effect_level4", "type" : "radio_button",
									"x" : 177, "y" : 50,

									"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL4,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL4_TOOLTIP,

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "effect_level5", "type" : "radio_button",
									"x" : 201, "y" : 50,

									"text" :  uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL5,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_EFFECT_LEVEL5_TOOLTIP,

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "effect_apply", "type" : "button",
									"x" : 225, "y" : 50,

									"text" : uiScriptLocale.GRAPHICONOFF_EFFECT_APPLY,

									"default_image" : ROOT_PATH + "option_button_thin_norm.tga",
									"over_image" : ROOT_PATH + "option_button_thin_hover.tga",
									"down_image" : ROOT_PATH + "option_button_thin_down.tga",
								},
								{
									"name" : "privateShop_level", "type" : "text",
									"x" : 10, "y" : 75+2,
									"text" : uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL, 
								},
								{
									"name" : "privateShop_level1", "type" : "radio_button",
									"x" : 105, "y" : 75,

									"text" :  uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL1,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL1_TOOLTIP, 

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "privateShop_level2", "type" : "radio_button",
									"x" : 129, "y" : 75,

									"text" :  uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL2,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL2_TOOLTIP,

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "privateShop_level3", "type" : "radio_button",
									"x" : 153, "y" : 75,

									"text" :  uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL3,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL3_TOOLTIP,

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "privateShop_level4", "type" : "radio_button",
									"x" : 177, "y" : 75,

									"text" :  uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL4,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL4_TOOLTIP,

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "privateShop_level5", "type" : "radio_button",
									"x" : 201, "y" : 75,

									"text" :  uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL5,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_LEVEL5_TOOLTIP,

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "privateShop_apply", "type" : "button",
									"x" : 225, "y" : 75,

									"text" : uiScriptLocale.GRAPHICONOFF_PRIVATE_SHOP_APPLY,

									"default_image" : ROOT_PATH + "option_button_thin_norm.tga",
									"over_image" : ROOT_PATH + "option_button_thin_hover.tga",
									"down_image" : ROOT_PATH + "option_button_thin_down.tga",
								},
								{
									"name" : "dropItem_level", "type" : "text",
									"x" : 10, "y" : 100+2,
									"text" : uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL, 
								},
								{
									"name" : "dropItem_level1", "type" : "radio_button",
									"x" : 105, "y" : 100,

									"text" :  uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL1,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL1_TOOLTIP, 

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "dropItem_level2", "type" : "radio_button",
									"x" : 129, "y" : 100,

									"text" :  uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL2,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL2_TOOLTIP, 

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "dropItem_level3", "type" : "radio_button",
									"x" : 153, "y" : 100,

									"text" :  uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL3,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL3_TOOLTIP, 

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "dropItem_level4", "type" : "radio_button",
									"x" : 177, "y" : 100,

									"text" :  uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL4,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL4_TOOLTIP, 

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "dropItem_level5", "type" : "radio_button",
									"x" : 201, "y" : 100,

									"text" :  uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL5,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_DROP_ITEM_LEVEL5_TOOLTIP, 

									"default_image" : ROOT_PATH + "graphic_button_norm.tga",
									"over_image" : ROOT_PATH + "graphic_button_hover.tga",
									"down_image" : ROOT_PATH + "graphic_button_down.tga",
								},
								{
									"name" : "dropItem_apply", "type" : "button",
									"x" : 225, "y" : 100,

									"text" : uiScriptLocale.GRAPHICONOFF_DROP_ITEM_APPLY,

									"default_image" : ROOT_PATH + "option_button_thin_norm.tga",
									"over_image" : ROOT_PATH + "option_button_thin_hover.tga",
									"down_image" : ROOT_PATH + "option_button_thin_down.tga",
								},
								{
									"name" : "pet_status", "type" : "text",
									"x" : 10, "y" : 125+2,
									"text" : uiScriptLocale.GRAPHICONOFF_PET_STATUS,
								},
								{
									"name" : "pet_on", "type" : "radio_button",
									"x" : 105, "y" : 125,

									"text" : uiScriptLocale.GRAPHICONOFF_PET_STATUS_ON,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_PET_STATUS_ON_TOOLTIP,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "pet_off", "type" : "radio_button",
									"x" : 170, "y" : 125,

									"text" : uiScriptLocale.GRAPHICONOFF_PET_STATUS_OFF,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_PET_STATUS_OFF_TOOLTIP,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "npcName_status", "type" : "text",
									"x" : 10, "y" : 150+2,
									"text" : uiScriptLocale.GRAPHICONOFF_NPC_NAME_STATUS,
								},
								{
									"name" : "npcName_on", "type" : "radio_button",
									"x" : 105, "y" : 150,

									"text" : uiScriptLocale.GRAPHICONOFF_NPC_NAME_STATUS_ON,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_NPC_NAME_STATUS_ON_TOOLTIP,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "npcName_off", "type" : "radio_button",
									"x" : 170, "y" : 150,

									"text" : uiScriptLocale.GRAPHICONOFF_NPC_NAME_STATUS_OFF,
									"tooltip_text" : uiScriptLocale.GRAPHICONOFF_NPC_NAME_STATUS_OFF_TOOLTIP,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "show_mob_info", "type" : "text",
									"x" : 10, "y" : 175+2,
									"text" : uiScriptLocale.OPTION_MOB_INFO,
								},
								{
									"name" : "show_mob_level_button", "type" : "toggle_button",
									"x" : 105, "y" : 175,
									"text" : uiScriptLocale.OPTION_MOB_INFO_LEVEL,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "show_mob_AI_flag_button", "type" : "toggle_button",
									"x" : 170, "y" : 175,

									"text" : uiScriptLocale.OPTION_MOB_INFO_AGGR,

									"default_image" : ROOT_PATH + "option_button_norm.tga",
									"over_image" : ROOT_PATH + "option_button_hover.tga",
									"down_image" : ROOT_PATH + "option_button_down.tga",
								},
								{
									"name" : "fov_option", "type" : "text",
									"x" : 10, "y" : 200+2,
									"text" : uiScriptLocale.OPTION_FOV,
								},
								{
									"name" : "fov_controller", "type" : "newsliderbar",
									"x" : 105, "y" : 208,
								},
								{
									"name" : "fov_reset_button", "type" : "button",
									"x" : 280, "y" : 201,

									"tooltip_text" : uiScriptLocale.OPTION_FOV_RESET,

									"default_image" : ROOT_PATH + "option_button_reset_norm.tga",
									"over_image" : ROOT_PATH + "option_button_reset_hover.tga",
									"down_image" : ROOT_PATH + "option_button_reset_down.tga",
								},
								{
									"name" : "fov_value", "type" : "image",
									"image" : ROOT_PATH + "value_box.tga",
									"x" : 258, "y" : 201,

									"children" :
									(
										{
											"name" : "fov_value_text", "type" : "text",
											"x" : 1, "y" : 0,
											"text" : "0",
										},
									)
								},
								{
									"name" : "stone_scale_option", "type" : "text",
									"x" : 10, "y" : 225+2,
									"text" : uiScriptLocale.OPTION_STONES,
								},
								{
									"name" : "stone_scale_controller", "type" : "newsliderbar",
									"x" : 105, "y" : 233,
								},
								{
									"name" : "stone_scale_reset_button", "type" : "button",
									"x" : 260, "y" : 224,

									"tooltip_text" : uiScriptLocale.OPTION_FOV_RESET,

									"default_image" : ROOT_PATH + "option_button_reset_norm.tga",
									"over_image" : ROOT_PATH + "option_button_reset_hover.tga",
									"down_image" : ROOT_PATH + "option_button_reset_down.tga",
								},
							),
						},
						{
							"name" : "display_settings_graphics", "type" : "radio_button",
							"x" : 10, "y" : 17,

							"text" : localeInfo.OPTION_GAME_GRAPHIC,

							"default_image" : ROOT_PATH + "board_button_main.tga",
							"over_image" : ROOT_PATH + "board_button_over.tga",
							"down_image" : ROOT_PATH + "board_button_down.tga",
						},
					),
				},
				{
					"name" : "background_sound",
					"x" : 15, "y" : 60,
					"width" : 500, "height" : 466,

					"children" : 
					(
						{
							"name" : "music_option_board",
							"x" : 190, "y" : 17,
							"width" : 500, "height" : 466,

							"children" :
							(
								{
									"name" : "sound_name", "type" : "text",
									"x" : 10, "y" : 0,
									"text" : uiScriptLocale.OPTION_SOUND,
								},
								{
									"name" : "sound_volume_controller", "type" : "newsliderbar",
									"x" : 105, "y" : 5,
								},
								{
									"name" : "music_name", "type" : "text",
									"x" : 10, "y" : 25,
									"text" : uiScriptLocale.OPTION_MUSIC,
								},
								{
									"name" : "music_volume_controller", "type" : "newsliderbar",
									"x" : 105, "y" : 30,
								},
								{
									"name" : "bgm_button", "type" : "button",
									"x" : 10, "y" : 55,

									"text" : uiScriptLocale.OPTION_MUSIC_CHANGE,

									"default_image" : ROOT_PATH + "option_button_thin_norm.tga",
									"over_image" : ROOT_PATH + "option_button_thin_hover.tga",
									"down_image" : ROOT_PATH + "option_button_thin_down.tga",
								},
								{
									"name" : "bgm_file", "type" : "text",
									"x" : 105, "y" : 57,
									"text" : uiScriptLocale.OPTION_MUSIC_DEFAULT_THEMA,
								},
							),
						},
						{
							"name" : "general_button_sound", "type" : "radio_button",
							"x" : 10, "y" : 17,

							"text" : localeInfo.OPTION_GAME_SOUND,

							"default_image" : ROOT_PATH + "board_button_main.tga",
							"over_image" : ROOT_PATH + "board_button_over.tga",
							"down_image" : ROOT_PATH + "board_button_down.tga",
						},
					),
				},
				{
					"name" : "category_game_option", "type" : "radio_button",
					"x" : 15, "y" : 34,

					"text" : localeInfo.OPTION_GAME_SETTINGS,

					"default_image" : ROOT_PATH + "button_main.tga",
					"over_image" : ROOT_PATH + "button_main.tga",
					"down_image" : ROOT_PATH + "button_secondary.tga",
				},
				{
					"name" : "category_graphics_option", "type" : "radio_button",
					"x" : 15 + 88, "y" : 34,

					"text" : localeInfo.OPTION_GRAPHIC_SETTINGS,

					"default_image" : ROOT_PATH + "button_main.tga",
					"over_image" : ROOT_PATH + "button_main.tga",
					"down_image" : ROOT_PATH + "button_secondary.tga",
				},
				{
					"name" : "category_sound_option", "type" : "radio_button",
					"x" : 15 + 88 + 88, "y" : 34,

					"text" : localeInfo.OPTION_SOUND_SETTINGS,

					"default_image" : ROOT_PATH + "button_main.tga",
					"over_image" : ROOT_PATH + "button_main.tga",
					"down_image" : ROOT_PATH + "button_secondary.tga",
				},
			),
		},
	),
}