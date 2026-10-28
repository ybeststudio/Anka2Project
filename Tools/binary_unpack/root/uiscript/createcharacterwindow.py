import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/intro/create/"
COMMON_PATH = "d:/ymir work/ui/intro/common/"

LEFT_BOARD_X = SCREEN_WIDTH * (75) / 800
RIGHT_BOARD_X = SCREEN_WIDTH * (75) * (8) / 800

BOARD_Y = SCREEN_HEIGHT * (196) / 600

X_GAP = 11
Y_GAP = 12

FLAG_SCALE_X = 0.55
FLAG_SCALE_Y = 0.55

STAT_GAUGE_X = -30
STAT_GAUGE_Y = 53
STAT_GAUGE_BAR_X = X_GAP + 40
STAT_GAUGE_BAR_WIDTH = 105
STAT_GAUGE_GAP = 30

WINDOW_LEFT_WIDTH = 308

FACE_X = 8
FACE_Y = 6
SELECT_BTN_X = X_GAP + 4
SELECT_BTN_Y = 50
SELECT_BTN_GAP = 60

window = {
	"name": "CreateCharacterWindow",
	"x": 0, "y": 0,
	"width": SCREEN_WIDTH, "height": SCREEN_HEIGHT,

	"children" :
	(
		{
			"name" : "BackGround", "type" : "expanded_image",
			"x" : 0, "y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1920.0, "y_scale" : float(SCREEN_HEIGHT) / 1080.0,
			"image" : COMMON_PATH + "background.png",

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
			"name": "CharacterBoard", "type": "window",
			"x": LEFT_BOARD_X, "y": BOARD_Y,
			"width" : WINDOW_LEFT_WIDTH + (X_GAP * 2) + 1, "height" : 500,

			"children":
			(
				{
					"name" : "EmpireFlagWindow", "type" : "window",
					"x" : X_GAP + 75, "y" : Y_GAP - 65,
					"width" : WINDOW_LEFT_WIDTH, "height" : 42,

					"children" :
					(
						{
							"name" : "EmpireFlag", "type" : "expanded_image",
							"x" : 4, "y" : 3, "x_scale" : FLAG_SCALE_X, "y_scale" : FLAG_SCALE_Y,
							"image" : "D:/ymir work/ui/intro/empire/empireflag_a.sub"
						},
					),
				},
				{
					"name" : "CharacterSlotTitleBar", "type" : "image",
					"x" : 0, "y" : 0, "image" : COMMON_PATH + "titlebar_left.png",

					"children" :
					(
						{ "name" : "CharacterSlotTitleBarText", "type" : "text", "x" : 65, "y" : 11, "fontsize" : "LARGE", "text" : uiScriptLocale.SELECT_CHARACTER_TITLE },
					),
				},

				{
					"name": "Warrior", "type": "radio_button",
					"x" : SELECT_BTN_X, "y" : SELECT_BTN_Y,

					"default_image" : COMMON_PATH + "emptyslot_button_01.png",
					"over_image" : COMMON_PATH + "emptyslot_button_02.png",
					"down_image" : COMMON_PATH + "emptyslot_button_03.png",

					"children":
					(
						{
							"name": "CharacterFace_0", "type": "image",
							"x": FACE_X, "y": FACE_Y,
							"image": "D:/ymir work/ui/intro/public_intro/face/face_warrior_m_01.sub"
						},
						{ "name" : "Character_Text1", "type" : "text", "x" : 65, "y" : 13, "fontsize" : "LARGE", "text" : uiScriptLocale.SELECT_CHARACTER_WARRIOR },
						{ "name" : "Character_Text2", "type" : "text", "x" : 65, "y" : 28, "color" : 0xff6f6f6f, "text" : uiScriptLocale.SELECT_CHARACTER_WARRIOR_SKILL },
					),
				},
				{
					"name": "Assassin", "type": "radio_button",
					"x" : SELECT_BTN_X, "y" : SELECT_BTN_Y + SELECT_BTN_GAP,

					"default_image" : COMMON_PATH + "emptyslot_button_01.png",
					"over_image" : COMMON_PATH + "emptyslot_button_02.png",
					"down_image" : COMMON_PATH + "emptyslot_button_03.png",

					"children":
					(
						{
							"name": "CharacterFace_1", "type": "image",
							"x": FACE_X, "y": FACE_Y,
							"image": "D:/ymir work/ui/intro/public_intro/face/face_assassin_m_01.sub"
						},
						{ "name" : "Character_Text1", "type" : "text", "x" : 65, "y" : 13, "fontsize" : "LARGE", "text" : uiScriptLocale.SELECT_CHARACTER_ASSASSIN },
						{ "name" : "Character_Text2", "type" : "text", "x" : 65, "y" : 28, "color" : 0xff6f6f6f, "text" : uiScriptLocale.SELECT_CHARACTER_ASSASSIN_SKILL },
					),
				},
				{
					"name": "Sura", "type": "radio_button",
					"x" : SELECT_BTN_X, "y" :  SELECT_BTN_Y + SELECT_BTN_GAP*2,

					"default_image" : COMMON_PATH + "emptyslot_button_01.png",
					"over_image" : COMMON_PATH + "emptyslot_button_02.png",
					"down_image" : COMMON_PATH + "emptyslot_button_03.png",

					"children":
					(
						{
							"name": "CharacterFace_2", "type": "image",
							"x": FACE_X, "y": FACE_Y,
							"image": "D:/ymir work/ui/intro/public_intro/face/face_sura_m_01.sub"
						},
						{ "name" : "Character_Text1", "type" : "text", "x" : 65, "y" : 13, "fontsize" : "LARGE", "text" : uiScriptLocale.SELECT_CHARACTER_SURA },
						{ "name" : "Character_Text2", "type" : "text", "x" : 65, "y" : 28, "color" : 0xff6f6f6f, "text" : uiScriptLocale.SELECT_CHARACTER_SURA_SKILL },
					),
				},
				{
					"name": "Shaman", "type": "radio_button",
					"x" : SELECT_BTN_X, "y" : SELECT_BTN_Y + SELECT_BTN_GAP*3,

					"default_image" : COMMON_PATH + "emptyslot_button_01.png",
					"over_image" : COMMON_PATH + "emptyslot_button_02.png",
					"down_image" : COMMON_PATH + "emptyslot_button_03.png",

					"children":
					(
						{
							"name": "CharacterFace_3", "type": "image",
							"x": FACE_X, "y": FACE_Y,
							"image": "D:/ymir work/ui/intro/public_intro/face/face_shaman_m_01.sub"
						},
						{ "name" : "Character_Text1", "type" : "text", "x" : 65, "y" : 13, "fontsize" : "LARGE", "text" : uiScriptLocale.SELECT_CHARACTER_SHAMAN },
						{ "name" : "Character_Text2", "type" : "text", "x" : 65, "y" : 28, "color" : 0xff6f6f6f, "text" : uiScriptLocale.SELECT_CHARACTER_SHAMAN_SKILL },
					),
				},
			),
		},
		{
			"name" : "StatsBoard", "type" : "window",
			"x" : RIGHT_BOARD_X, "y" : BOARD_Y,
			"width" : WINDOW_LEFT_WIDTH + (X_GAP * 2) + 1, "height" : 500,

			"children" :
			(
				{
					"name" : "StatsSlotTitleBar", "type" : "image",
					"x" : 0, "y" : 0, "image" : COMMON_PATH + "window_right.png",

					"children" :
					(
						{ "name" : "StatsSlotTitleBarText", "type" : "text", "x" : 65, "y" : 11, "fontsize" : "LARGE", "text" : uiScriptLocale.SELECT_CHARACTER_STATUS_TITLE },

						{
							"name" : "hth_gauge", "type" : "gauge2",
							"x" : STAT_GAUGE_BAR_X, "y" : STAT_GAUGE_Y,
							"width" : STAT_GAUGE_BAR_WIDTH,

							"children" :
							(
								{
									"name" : "hth_text", "type" : "text",
									"x" : STAT_GAUGE_X, "y" : 2,
									"text" : uiScriptLocale.SELECT_HP,
								},
								{
									"name" : "hth_gauge_slot", "type" : "image",
									"x" : STAT_GAUGE_BAR_X*2 + 50, "y" : 0, "image" : COMMON_PATH + "input_small.png",

									"children" :
									(
										{
											"name" : "hth_value", "type" : "text",
											"x" : 0, "y" : -1,
											"all_align" : "center", "text" : "0",
										},
									),
								},
							),
						},
						{
							"name" : "int_gauge", "type" : "gauge2",
							"x" : STAT_GAUGE_BAR_X, "y" : STAT_GAUGE_Y + STAT_GAUGE_GAP,
							"width" : STAT_GAUGE_BAR_WIDTH,

							"children" :
							(
								{
									"name" : "int_text", "type" : "text",
									"x" : STAT_GAUGE_X, "y" : 2,
									"text" : uiScriptLocale.SELECT_SP,
								},
								{
									"name" : "int_gauge_slot", "type" : "image",
									"x" : STAT_GAUGE_BAR_X*2 + 50, "y" : 0, "image" : COMMON_PATH + "input_small.png",

									"children" :
									(
										{
											"name" : "int_value", "type" : "text",
											"x" : 0, "y" : -1,
											"all_align" : "center", "text" : "0",
										},
									),
								},
							),
						},
						{
							"name" : "str_gauge", "type" : "gauge2",
							"x" : STAT_GAUGE_BAR_X, "y" : STAT_GAUGE_Y + STAT_GAUGE_GAP*2,
							"width" : STAT_GAUGE_BAR_WIDTH,

							"children" :
							(
								{
									"name" : "str_text", "type" : "text",
									"x" : STAT_GAUGE_X, "y" : 2,
									"text" : uiScriptLocale.SELECT_ATT,
								},
								{
									"name" : "str_gauge_slot", "type" : "image",
									"x" : STAT_GAUGE_BAR_X*2 + 50, "y" : 0, "image" : COMMON_PATH + "input_small.png",

									"children" :
									(
										{
											"name" : "str_value", "type" : "text",
											"x" : 0, "y" : -1,
											"all_align" : "center", "text" : "0",
										},
									),
								},
							),
						},
						{
							"name" : "dex_gauge", "type" : "gauge2",
							"x" : STAT_GAUGE_BAR_X, "y" : STAT_GAUGE_Y + STAT_GAUGE_GAP*3,
							"width" : STAT_GAUGE_BAR_WIDTH,

							"children" :
							(
								{
									"name" : "dex_text", "type" : "text",
									"x" : STAT_GAUGE_X, "y" : 2,
									"text" : uiScriptLocale.SELECT_DEX,
								},
								{
									"name" : "dex_gauge_slot", "type" : "image",
									"x" : STAT_GAUGE_BAR_X*2 + 50, "y" : 0, "image" : COMMON_PATH + "input_small.png",

									"children" :
									(
										{
											"name" : "dex_value", "type" : "text",
											"x" : 0, "y" : -1,
											"all_align" : "center", "text" : "0",
										},
									),
								},
							),
						},
					),
				},
			),
		},
		{
			"name": "CustomizationBoard", "type": "window",
			"x": RIGHT_BOARD_X, "y": BOARD_Y + 195,
			"width" : WINDOW_LEFT_WIDTH + (X_GAP * 2) + 1, "height" : 500,

			"children" :
			(
				{
					"name" : "CustomizationSlotTitleBar", "type" : "image",
					"x" : 0, "y" : 0, "image" : COMMON_PATH + "window_right.png",

					"children":
					(
						{ "name" : "GenderSlotTitleBarText", "type" : "text", "x" : 65, "y" : 11, "fontsize" : "LARGE", "text" : uiScriptLocale.SELECT_CHARACTER_CUSTOMIZATION_TITLE },
						{ "name" : "SelectGenreText", "type" : "text", "x" : -77, "y" : -21, "all_align" : "center", "fontsize" : "LARGE", "text" : uiScriptLocale.SELECT_CHARACTER_SEX },
						{
							"name": "GenderButton1", "type": "radio_button",
							"x": 15, "y": 85,

							"default_image" : ROOT_PATH + "male_button_01.png",
							"over_image" : ROOT_PATH + "male_button_02.png",
							"down_image" : ROOT_PATH + "male_button_03.png",
						},
						{
							"name": "GenderButton2", "type": "radio_button",
							"x": 65, "y": 85,

							"default_image" : ROOT_PATH + "female_button_01.png",
							"over_image" : ROOT_PATH + "female_button_02.png",
							"down_image" : ROOT_PATH + "female_button_03.png",
						},
						{ "name" : "SelectSkinText", "type" : "text", "x" : 77, "y" : -21, "all_align" : "center", "fontsize" : "LARGE", "text" : uiScriptLocale.SELECT_CHARACTER_SKIN},
						{
							"name": "ShapeButton1", "type": "radio_button",
							"x": 160, "y": 85,

							"default_image" : ROOT_PATH + "skin_1_button_01.png",
							"over_image" : ROOT_PATH + "skin_1_button_02.png",
							"down_image" : ROOT_PATH + "skin_1_button_03.png",
						},
						{
							"name": "ShapeButton2", "type": "radio_button",
							"x": 210, "y": 85,

							"default_image" : ROOT_PATH + "skin_2_button_01.png",
							"over_image" : ROOT_PATH + "skin_2_button_02.png",
							"down_image" : ROOT_PATH + "skin_2_button_03.png",
						},
						{
							"name": "CharacterName", "type": "image",
							"x": 40, "y": 132,
							"image": ROOT_PATH + "input_large.png",

							"children":
							(
								{
									"name": "character_name_value", "type": "editline",
									"x": 5, "y": 7, "input_limit": 24,
									"width": 130, "height": 20,
								},
							),
						},
					),
				},
			),
		},
		{
			"name": "CreateButton", "type": "button",
			"x" : SCREEN_WIDTH/2 - 65, "y" : SCREEN_HEIGHT - SCREEN_HEIGHT*(85)/600.0,

			"default_image" : COMMON_PATH + "play_button_01.png",
			"over_image" : COMMON_PATH + "play_button_02.png",
			"down_image" : COMMON_PATH + "play_button_03.png",
		},
		{
			"name": "ExitButton", "type": "button",
			"x" : SCREEN_WIDTH - 90, "y" : SCREEN_HEIGHT - SCREEN_HEIGHT+20,

			"default_image" : COMMON_PATH + "exit_button_01.png",
			"over_image" : COMMON_PATH + "exit_button_02.png",
			"down_image" : COMMON_PATH + "exit_button_03.png",
		},
	),
}