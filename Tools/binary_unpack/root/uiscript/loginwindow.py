if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

COMMON_PATH = "d:/ymir work/ui/intro/common/"

window = {
	"name" : "LoginWindow", "sytle" : ("movable",), "x" : 0, "y" : 0,
	"width" : SCREEN_WIDTH, "height" : SCREEN_HEIGHT,

	"children" :
	[
		{
			"name" : "Background", "type" : "expanded_image", "x" : 0, "y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1920.0, "y_scale" : float(SCREEN_HEIGHT) / 1080.0,
			"image" : "d:/ymir work/ui/intro/login/background.png",

			"children" :
			(
				{
					"name" : "FooterCopyright", "type" : "text", "x" : 150, "y" : SCREEN_HEIGHT - SCREEN_HEIGHT/25,
					"text" : "© 2025 · BestStudio Production — Anka2 Project", "horizontal_align" : "center",
					"text_vertical_align" : "center", "text_horizontal_align" : "center",
					"fontsize" : "LARGE", "font" : "Tahoma",
				},
				{
					"name" : "FooterCopyright2", "type" : "text", "x" : 150, "y" : SCREEN_HEIGHT - SCREEN_HEIGHT/40,
					"text" : "Optimized & Deployed for TURKMMO Network", "horizontal_align" : "center",
					"text_vertical_align" : "center", "text_horizontal_align" : "center",
					"fontsize" : "LARGE", "font" : "Tahoma",
				},
			),
		},
		{
			"name" : "ConnectBoard", "type" : "image", "x" : -2, "y" : -120,
			"vertical_align" : "center", "horizontal_align" : "center",
			"image" : "d:/ymir work/ui/intro/login/connect/server_select.png",

			"children" :
			(
				{
					"name" : "ConnectName", "type" : "text",
					"x" : 0, "y" : 20, "text" : " ", "horizontal_align" : "center",
					"text_vertical_align" : "center", "text_horizontal_align" : "center",
				},
				{
					"name" : "ServerButton", "type" : "button", "x" : 170, "y" : 19,
					"default_image" : "d:/ymir work/ui/intro/login/arrow/arrow_down_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/arrow/arrow_down_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/arrow/arrow_down_down.png",
				},
			),
		},
		{
			"name" : "LoginBoard", "type" : "window",
			"x" : 0, "y" : 0, "width" : 352, "height" : 190,
			"vertical_align" : "center", "horizontal_align" : "center",

			"children" :
			(
				{
					"name" : "ID_EditLine_Input", "type" : "image", "x" : 0, "y" : -50,
					"horizontal_align" : "center", "vertical_align" : "center",
					"image" : "d:/ymir work/ui/intro/login/connect/input.png",

					"children" :
					(
						{
							"name" : "ID_EditLine", "type" : "editline",
							"x" : 12, "y" : 15, "width" : 200, "height" : 35,
							"input_limit": 19, "enable_codepage" : 0,
							"horizontal_align" : "center",
							"r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0,
							# "info_msg" : uiScriptLocale.LOGIN_ID,
						},
						{
							"name" : "ID_EditLine_Text", "type" : "text",
							"x" : 0, "y" : -10, "text" : uiScriptLocale.LOGIN_ID, "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
							"fontsize" : "LARGE", "font" : "Tahoma",
						},
					),
				},
				{
					"name" : "PWD_EditLine_Input", "type" : "image", "x" : 0, "y" : 20,
					"horizontal_align" : "center", "vertical_align" : "center",
					"image" : "d:/ymir work/ui/intro/login/connect/input.png",

					"children" :
					(
						{
							"name" : "PWD_EditLine", "type" : "editline",
							"x" : 12, "y" : 15, "width" : 200, "height" : 35,
							"input_limit": 16, "secret_flag": 1, "enable_codepage" : 0,
							"horizontal_align" : "center",
							"r" : 1.0, "g" : 1.0, "b" : 1.0, "a" : 1.0,
							# "info_msg" : uiScriptLocale.LOGIN_PASSWORD,
						},
						{
							"name" : "PWD_EditLine_Text", "type" : "text",
							"x" : 0, "y" : -10, "text" : uiScriptLocale.LOGIN_PASSWORD, "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
							"fontsize" : "LARGE", "font" : "Tahoma",
						},
					),
				},
				{
					"name" : "LoginButton", "type" : "button", "x" : 0, "y" : 75,
					"horizontal_align" : "center", "vertical_align" : "center",

					"default_image" : "d:/ymir work/ui/intro/login/connect/button_login_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/connect/button_login_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/connect/button_login_down.png",
				},
			),
		},
		{
			"name" : "ChannelBoard", "type" : "expanded_image", "x" : -245, "y" : 0,
			"horizontal_align" : "center", "vertical_align" : "center",
			"image": "d:/ymir work/ui/intro/login/channel/channel_board.png",

			"children" :
			(
				{
					"name" : "Channel1_Button", "type" : "radio_button", "x" : 0, "y" : -45,
					"horizontal_align" : "center", "vertical_align" : "center",

					"default_image" : "d:/ymir work/ui/intro/login/input_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/input_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/input_down.png",

					"children" :
					(
						{
							"name" : "Channel1_Text", "type" : "text", "x" : -45, "y" : 12,
							"text" : uiScriptLocale.LOGIN_CHANNEL_1, "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
						{
							"name" : "Channel1_Status", "type" : "image", "x" : 5, "y" : -2,
							"image" : "d:/ymir work/ui/intro/login/channel/offline.png",
						},
						{
							"name" : "Channel1_Status_Text", "type" : "text", "x" : 20, "y" : 12,
							"text" : "Offline", "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
					),
				},
				{
					"name" : "Channel2_Button", "type" : "radio_button", "x" : 0, "y" : -10,
					"horizontal_align" : "center", "vertical_align" : "center",

					"default_image" : "d:/ymir work/ui/intro/login/input_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/input_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/input_down.png",

					"children" :
					(
						{
							"name" : "Channel2_Text", "type" : "text", "x" : -45, "y" : 12,
							"text" : uiScriptLocale.LOGIN_CHANNEL_2, "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
						{
							"name" : "Channel2_Status", "type" : "image", "x" : 5, "y" : -2,
							"image" : "d:/ymir work/ui/intro/login/channel/offline.png",
						},
						{
							"name" : "Channel2_Status_Text", "type" : "text", "x" : 20, "y" : 12,
							"text" : "Offline", "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
					),
				},
				{
					"name" : "Channel3_Button", "type" : "radio_button", "x" : 0, "y" : 25,
					"horizontal_align" : "center", "vertical_align" : "center",

					"default_image" : "d:/ymir work/ui/intro/login/input_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/input_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/input_down.png",

					"children" :
					(
						{
							"name" : "Channel3_Text", "type" : "text", "x" : -45, "y" : 12,
							"text" : uiScriptLocale.LOGIN_CHANNEL_3, "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
						{
							"name" : "Channel3_Status", "type" : "image", "x" : 5, "y" : -2,
							"image" : "d:/ymir work/ui/intro/login/channel/offline.png",
						},
						{
							"name" : "Channel3_Status_Text", "type" : "text", "x" : 20, "y" : 12,
							"text" : "Offline", "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
					),
				},
				{
					"name" : "Channel4_Button", "type" : "radio_button", "x" : 0, "y" : 60,
					"horizontal_align" : "center", "vertical_align" : "center",

					"default_image" : "d:/ymir work/ui/intro/login/input_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/input_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/input_down.png",

					"children" :
					(
						{
							"name" : "Channel4_Text", "type" : "text", "x" : -45, "y" : 12,
							"text" : uiScriptLocale.LOGIN_CHANNEL_4, "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
						{
							"name" : "Channel4_Status", "type" : "image", "x" : 5, "y" : -2,
							"image" : "d:/ymir work/ui/intro/login/channel/offline.png",
						},
						{
							"name" : "Channel4_Status_Text", "type" : "text", "x" : 20, "y" : 12,
							"text" : "Offline", "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
					),
				},
			),
		},
		{
			"name" : "AccountBoard", "type" : "image", "x" : 245, "y" : 0,
			"horizontal_align" : "center", "vertical_align" : "center",
			"image" : "d:/ymir work/ui/intro/login/account/account_board.png",

			"children" :
			(
				{
					"name" : "Load_0_Button", "type" : "button", "x" : 0, "y" : -45,
					"horizontal_align" : "center", "vertical_align" : "center",

					"default_image" : "d:/ymir work/ui/intro/login/input_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/input_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/input_down.png",

					"children" :
					(
						{
							"name" : "Account_0_Text", "type" : "text", "x" : 0, "y" : 12,
							"text" : "", "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
						{
							"name" : "Account_0_Flag", "type" : "image", "x" : 10, "y" : 0,
							"image" : "d:/ymir work/ui/intro/login/account/flag.png",

							"children":
							(
								{
									"name" : "Account_0_Flag_Text", "type" : "text", "x" : 0, "y" : 6,
									"text" : "F1", "horizontal_align" : "center", "fontname" : "Arial:12",
									"text_vertical_align" : "center", "text_horizontal_align" : "center",
								},
							),
						},
					),
				},
				{
					"name" : "Delete_0_Button", "type" : "button", "x" : 200, "y" : 39,
					"default_image" : "d:/ymir work/ui/intro/login/account/button_delete_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/account/button_delete_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/account/button_delete_down.png",
				},
				{
					"name" : "Save_0_Button", "type" : "button", "x" : 200, "y" : 39,
					"default_image" : "d:/ymir work/ui/intro/login/account/button_save_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/account/button_save_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/account/button_save_down.png",
				},
				{
					"name" : "Load_1_Button", "type" : "button", "x" : 0, "y" : -10,
					"horizontal_align" : "center", "vertical_align" : "center",

					"default_image" : "d:/ymir work/ui/intro/login/input_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/input_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/input_down.png",

					"children" :
					(
						{
							"name" : "Account_1_Text", "type" : "text", "x" : 0, "y" : 12,
							"text" : "", "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
						{
							"name" : "Account_1_Flag", "type" : "image", "x" : 10, "y" : 0,
							"image" : "d:/ymir work/ui/intro/login/account/flag.png",

							"children":
							(
								{
									"name" : "Account_1_Flag_Text", "type" : "text", "x" : 0, "y" : 6,
									"text" : "F2", "horizontal_align" : "center", "fontname" : "Arial:12",
									"text_vertical_align" : "center", "text_horizontal_align" : "center",
								},
							),
						},
					),
				},
				{
					"name" : "Delete_1_Button", "type" : "button", "x" : 200, "y" : 74,
					"default_image" : "d:/ymir work/ui/intro/login/account/button_delete_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/account/button_delete_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/account/button_delete_down.png",
				},
				{
					"name" : "Save_1_Button", "type" : "button", "x" : 200, "y" : 74,
					"default_image" : "d:/ymir work/ui/intro/login/account/button_save_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/account/button_save_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/account/button_save_down.png",
				},
				{
					"name" : "Load_2_Button", "type" : "button", "x" : 0, "y" : 25,
					"horizontal_align" : "center", "vertical_align" : "center",

					"default_image" : "d:/ymir work/ui/intro/login/input_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/input_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/input_down.png",

					"children" :
					(
						{
							"name" : "Account_2_Text", "type" : "text", "x" : 0, "y" : 12,
							"text" : "", "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
						{
							"name" : "Account_2_Flag", "type" : "image", "x" : 10, "y" : 0,
							"image" : "d:/ymir work/ui/intro/login/account/flag.png",

							"children":
							(
								{
									"name" : "Account_2_Flag_Text", "type" : "text", "x" : 0, "y" : 6,
									"text" : "F3", "horizontal_align" : "center", "fontname" : "Arial:12",
									"text_vertical_align" : "center", "text_horizontal_align" : "center",
								},
							),
						},
					),
				},
				{
					"name" : "Delete_2_Button", "type" : "button", "x" : 200, "y" : 109,
					"default_image" : "d:/ymir work/ui/intro/login/account/button_delete_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/account/button_delete_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/account/button_delete_down.png",
				},
				{
					"name" : "Save_2_Button", "type" : "button", "x" : 200, "y" : 109,
					"default_image" : "d:/ymir work/ui/intro/login/account/button_save_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/account/button_save_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/account/button_save_down.png",
				},
				{
					"name" : "Load_3_Button", "type" : "button", "x" : 0, "y" : 60,
					"horizontal_align" : "center", "vertical_align" : "center",

					"default_image" : "d:/ymir work/ui/intro/login/input_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/input_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/input_down.png",

					"children" :
					(
						{
							"name" : "Account_3_Text", "type" : "text", "x" : 0, "y" : 12,
							"text" : "", "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
						},
						{
							"name" : "Account_3_Flag", "type" : "image", "x" : 10, "y" : 0,
							"image" : "d:/ymir work/ui/intro/login/account/flag.png",

							"children":
							(
								{
									"name" : "Account_3_Flag_Text", "type" : "text", "x" : 0, "y" : 6,
									"text" : "F4", "horizontal_align" : "center", "fontname" : "Arial:12",
									"text_vertical_align" : "center", "text_horizontal_align" : "center",
								},
							),
						},
					),
				},
				{
					"name" : "Delete_3_Button", "type" : "button", "x" : 200, "y" : 144,
					"default_image" : "d:/ymir work/ui/intro/login/account/button_delete_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/account/button_delete_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/account/button_delete_down.png",
				},
				{
					"name" : "Save_3_Button", "type" : "button", "x" : 200, "y" : 144,
					"default_image" : "d:/ymir work/ui/intro/login/account/button_save_norm.png",
					"over_image" : "d:/ymir work/ui/intro/login/account/button_save_over.png",
					"down_image" : "d:/ymir work/ui/intro/login/account/button_save_down.png",
				},
			),
		},
		{
			"name" : "ExitButton", "type" : "button",
			"x" : SCREEN_WIDTH - 90, "y" : SCREEN_HEIGHT - SCREEN_HEIGHT+20,
			"default_image" : COMMON_PATH + "exit_button_01.png",
			"over_image" : COMMON_PATH + "exit_button_02.png",
			"down_image" : COMMON_PATH + "exit_button_03.png",
		},
	],
}

if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
	window['children'] += [ \
		{
			"name" : "LanguageBoard", "type" : "window", "x" : -50, "y" : 110,
			"horizontal_align" : "center", "vertical_align" : "center",
			"width" : 370, "height" : 20,

			"children" :
			(
				# 0 - CZ
				{
					"name" : "Language_0_Button", "type" : "button", "x" : 40, "y" : 5,
					"default_image" : "d:/ymir work/ui/game/flags/flag_cz_norm.tga",
					"over_image"    : "d:/ymir work/ui/game/flags/flag_cz_over.tga",
					"down_image"    : "d:/ymir work/ui/game/flags/flag_cz_down.tga",
					"tooltip_text"  : uiScriptLocale.LANGUAGE_CZ,
				},
				# 1 - DE
				{
					"name" : "Language_1_Button", "type" : "button", "x" : 70, "y" : 5,
					"default_image" : "d:/ymir work/ui/game/flags/flag_de_norm.tga",
					"over_image"    : "d:/ymir work/ui/game/flags/flag_de_over.tga",
					"down_image"    : "d:/ymir work/ui/game/flags/flag_de_down.tga",
					"tooltip_text"  : uiScriptLocale.LANGUAGE_DE,
				},
				# 2 - EN
				{
					"name" : "Language_2_Button", "type" : "button", "x" : 105, "y" : 5,
					"default_image" : "d:/ymir work/ui/game/flags/flag_en_norm.tga",
					"over_image"    : "d:/ymir work/ui/game/flags/flag_en_over.tga",
					"down_image"    : "d:/ymir work/ui/game/flags/flag_en_down.tga",
					"tooltip_text"  : uiScriptLocale.LANGUAGE_EN,
				},
				# 3 - ES
				{
					"name" : "Language_3_Button", "type" : "button", "x" : 140, "y" : 5,
					"default_image" : "d:/ymir work/ui/game/flags/flag_es_norm.tga",
					"over_image"    : "d:/ymir work/ui/game/flags/flag_es_over.tga",
					"down_image"    : "d:/ymir work/ui/game/flags/flag_es_down.tga",
					"tooltip_text"  : uiScriptLocale.LANGUAGE_ES,
				},
				# 4 - FR
				{
					"name" : "Language_4_Button", "type" : "button", "x" : 140, "y" : 5,
					"default_image" : "d:/ymir work/ui/game/flags/flag_fr_norm.tga",
					"over_image"    : "d:/ymir work/ui/game/flags/flag_fr_over.tga",
					"down_image"    : "d:/ymir work/ui/game/flags/flag_fr_down.tga",
					"tooltip_text"  : uiScriptLocale.LANGUAGE_FR,
				},

				# 5 - HU
				{
					"name" : "Language_5_Button", "type" : "button", "x" : 175, "y" : 5,
					"default_image" : "d:/ymir work/ui/game/flags/flag_hu_norm.tga",
					"over_image"    : "d:/ymir work/ui/game/flags/flag_hu_over.tga",
					"down_image"    : "d:/ymir work/ui/game/flags/flag_hu_down.tga",
					"tooltip_text"  : uiScriptLocale.LANGUAGE_HU,
				},
				# 6 - IT
				{
					"name" : "Language_6_Button", "type" : "button", "x" : 210, "y" : 5,
					"default_image" : "d:/ymir work/ui/game/flags/flag_it_norm.tga",
					"over_image"    : "d:/ymir work/ui/game/flags/flag_it_over.tga",
					"down_image"    : "d:/ymir work/ui/game/flags/flag_it_down.tga",
					"tooltip_text"  : uiScriptLocale.LANGUAGE_IT,
				},
				# 7 - PL
				{
					"name" : "Language_7_Button", "type" : "button", "x" : 245, "y" : 5,
					"default_image" : "d:/ymir work/ui/game/flags/flag_pl_norm.tga",
					"over_image"    : "d:/ymir work/ui/game/flags/flag_pl_over.tga",
					"down_image"    : "d:/ymir work/ui/game/flags/flag_pl_down.tga",
					"tooltip_text"  : uiScriptLocale.LANGUAGE_PL,
				},
				# 8 - PT
				{
					"name" : "Language_8_Button", "type" : "button", "x" : 280, "y" : 5,
					"default_image" : "d:/ymir work/ui/game/flags/flag_pt_norm.tga",
					"over_image"    : "d:/ymir work/ui/game/flags/flag_pt_over.tga",
					"down_image"    : "d:/ymir work/ui/game/flags/flag_pt_down.tga",
					"tooltip_text"  : uiScriptLocale.LANGUAGE_PT,
				},
				# 9 - RO
				{
					"name" : "Language_9_Button", "type" : "button", "x" : 315, "y" : 5,
					"default_image" : "d:/ymir work/ui/game/flags/flag_ro_norm.tga",
					"over_image"    : "d:/ymir work/ui/game/flags/flag_ro_over.tga",
					"down_image"    : "d:/ymir work/ui/game/flags/flag_ro_down.tga",
					"tooltip_text"  : uiScriptLocale.LANGUAGE_RO,
				},

				# 10 - TR
				{
					"name" : "Language_10_Button", "type" : "button", "x" : 350, "y" : 5,
					"default_image" : "d:/ymir work/ui/game/flags/flag_tr_norm.tga",
					"over_image"    : "d:/ymir work/ui/game/flags/flag_tr_over.tga",
					"down_image"    : "d:/ymir work/ui/game/flags/flag_tr_down.tga",
					"tooltip_text"  : uiScriptLocale.LANGUAGE_TR,
				},
			),
		},
	]