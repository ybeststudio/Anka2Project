#if app.ENABLE_MOONLIGHT_VALLEY:
import uiScriptLocale

PUBLIC_PATH					= "d:/ymir work/ui/public/"
ROOT_PATH					= "d:/ymir work/ui/game/moonlight_valley/"

WINDOW_WIDTH				= 200
WINDOW_HEIGHT				= 248

window = {
	"name"		: "MoonlightValleyBossSummonsWindow",
	"style"		: ("movable", "float",),
	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	[
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			"style"		: ("attach",),
			"x"			: 0,
			"y"			: 0,
			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,
			"title"		: uiScriptLocale.MOONLIGHT_VALLEY_BOSS_SUMMONS_TITLE,	# 벤시의 혼

			"children" :
			[
				# window outline
				{
					"name"				: "main_outline",
					"type"				: "outline_window",
					"style"				: ("float", ),
					"x"					: 10,
					"y"					: 32,
					"width"				: WINDOW_WIDTH - 20,
					"height"			: WINDOW_HEIGHT - 45,
				},

				# summons background image
				{
					"name"		: "summon_background_img",
					"type"		: "image",
					
					"x"			: 13,
					"y"			: 35,
					
					"width"		: 172,
					"height"	: 172,
					
					"image"		: ROOT_PATH + "summon_background.sub",
				},

				# slots of summmon
				{
					"name" : "summon_slots",
					"type" : "slot",

					"x" : 10,
					"y" : 32,

					"width"		: WINDOW_WIDTH - 20,
					"height"	: WINDOW_HEIGHT - 45,

					"slot" : (
						{"index":0, "x":37, "y":20, "width":32, "height":32},
						{"index":1, "x":109, "y":20, "width":32, "height":32},
						{"index":2, "x":129, "y":87, "width":32, "height":32},
						{"index":3, "x":73, "y":128, "width":32, "height":32},
						{"index":4, "x":17, "y":87, "width":32, "height":32},
					),
				},

				# button for boss summon
				{
					"name"			: "summons_button",
					"type"			: "button",
					"x"				: 40,
					"y"				: 209,

					# 소환
					"text"			: uiScriptLocale.MOONLIGHT_VALLEY_BOSS_SUMMONS_BUTTON,
					"default_image" : ROOT_PATH + "summon_button_default.sub",
					"over_image"	: ROOT_PATH + "summon_button_over.sub",
					"down_image"	: ROOT_PATH + "summon_button_down.sub",
				},

				# summons effect when summons button down
				{
					"name"			: "summons_effect_ani_img",
					"type"			: "ani_image",
					"x"				: 17,
					"y"				: 39,

					"delay" : 3,

					"images" :
					(
						ROOT_PATH + "summon_effect_01.sub",
						ROOT_PATH + "summon_effect_02.sub",
						ROOT_PATH + "summon_effect_03.sub",
						ROOT_PATH + "summon_effect_01.sub",
						ROOT_PATH + "summon_effect_02.sub",
						ROOT_PATH + "summon_effect_03.sub",
						ROOT_PATH + "summon_effect_01.sub",
						ROOT_PATH + "summon_effect_02.sub",
						ROOT_PATH + "summon_effect_03.sub",
						ROOT_PATH + "summon_effect_01.sub",
						ROOT_PATH + "summon_effect_02.sub",
						ROOT_PATH + "summon_effect_03.sub",
					),
				},
			],
		},
	],
}
