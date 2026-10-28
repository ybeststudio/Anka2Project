#if app.ENABLE_MAP_ATTR_BANSHOP:
import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/"

window = {
	"name"		: "PrivateShopMapWindow",
	"style"		: ("float", "ltr", ),
	
	"x"			: SCREEN_WIDTH - 136,
	"y"			: 137 + 22,
	
	"width"		: 136,
	"height"	: 137,
	
	"children" :
	(
		{
			"name"		: "exit_window",
			"type"		: "window",
			"style"		: ("attach", "not_pick", ),

			"x"			: 25,
			"y"			: 0,

			"width"		: 88,
			"height"	: 21,
			
			"children"	:
			(
				{
					"name"				: "exit_button",
					"type"				: "button",

					"x"					: 0,
					"y"					: 0,

					# 나가기
					"text"				: uiScriptLocale.PRIVATE_SHOP_MAP_EXIT_BUTTON,
					"horizontal_align"	: "center",

					"default_image"		: ROOT_PATH + "large_button_01.sub",
					"over_image"		: ROOT_PATH + "large_button_02.sub",
					"down_image"		: ROOT_PATH + "large_button_03.sub",
				},
			),
		},
	),	
}
