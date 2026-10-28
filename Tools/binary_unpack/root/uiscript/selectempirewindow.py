import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/intro/empire/"
COMMON_PATH = "d:/ymir work/ui/intro/common/"
FLAGS_PATH = ROOT_PATH + "flags/"
BUTTONS_PATH = ROOT_PATH + "buttons/"

window = {
	"name" : "SelectEmpireWindow",
	"x" : 0, "y" : 0,
	"width" : SCREEN_WIDTH, "height" : SCREEN_HEIGHT,

	"children" :
	(
		{
			"name" : "BackgroundImage", "type" : "expanded_image",
			"x" : 0, "y" : 0,
			"x_scale" : float(SCREEN_WIDTH) / 1920.0, "y_scale" : float(SCREEN_HEIGHT) / 1080.0,
			"image" : ROOT_PATH + "background.png",

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
			"name" : "ShinsooSelectFlag", "type" : "image",
			"x" : -512, "y" : 40,
			"horizontal_align" : "center", "vertical_align" : "center",
			"image" : FLAGS_PATH + "shinsoo_flag.png",

			"children" :
			(
				{
					"name" : "ShinsooSelectButton", "type" : "button",
					"x" : 150, "y" : 125,

					"default_image" : BUTTONS_PATH + "shinsoo_button_01.png",
					"over_image" : BUTTONS_PATH + "shinsoo_button_02.png",
					"down_image" : BUTTONS_PATH + "shinsoo_button_03.png",
				},
			),
		},
		{
			"name" : "ChunjoSelectFlag", "type" : "image",
			"x" : 0, "y" : 40,
			"horizontal_align" : "center", "vertical_align" : "center",
			"image" : FLAGS_PATH + "chunjo_flag.png",

			"children" :
			(
				{
					"name" : "ChunjoSelectButton", "type" : "button",
					"x" : 150, "y" : 125,

					"default_image" : BUTTONS_PATH + "chunjo_button_01.png",
					"over_image" : BUTTONS_PATH + "chunjo_button_02.png",
					"down_image" : BUTTONS_PATH + "chunjo_button_03.png",
				},
			),
		},
		{
			"name" : "JinnoSelectFlag", "type" : "image",
			"x" : 512, "y" : 40,
			"horizontal_align" : "center", "vertical_align" : "center",
			"image" : FLAGS_PATH + "jinno_flag.png",

			"children" :
			(
				{
					"name" : "JinnoSelectButton", "type" : "button",
					"x" : 150, "y" : 125,

					"default_image" : BUTTONS_PATH + "jinno_button_01.png",
					"over_image" : BUTTONS_PATH + "jinno_button_02.png",
					"down_image" : BUTTONS_PATH + "jinno_button_03.png",
				},
			),
		},
		{
			"name" : "ExitButton", "type" : "button",
			"x" : SCREEN_WIDTH - 90, "y" : 20,

			"default_image" : COMMON_PATH + "exit_button_01.png",
			"over_image" : COMMON_PATH + "exit_button_02.png",
			"down_image" : COMMON_PATH + "exit_button_03.png",
		},
	),
}