import uiScriptLocale

RUMI_ROOT = "d:/ymir work/ui/minigame/rumi/"

window = {
	"name" : "MiniGameWindow",

	"x" : SCREEN_WIDTH - 136 - 100-10,
	"y" : 15,

	"width" : 100,
	"height" : 58,

	"children" :
	(
		{
			"name" : "mini_game_window",
			"type" : "window",

			"x" : 0,
			"y" : 0,

			"width" : 100,
			"height" : 58,

			"children" :
			(
				{
					"name" : "minigame_rumi_button",
					"type" : "button",

					"x" : 0,
					"y" : 0,

					"default_image" : RUMI_ROOT + "rumi_button_max.sub",
					"over_image" : RUMI_ROOT + "rumi_button_max.sub",
					"down_image" : RUMI_ROOT + "rumi_button_max.sub",
				},

				{
					"name" : "minigame_rumi_button_effect",
					"type" : "ani_image",
					"style" : ("attach",),

					"x" : 0,
					"y" : 0,
					
					"delay" : 8,

					"images" :
					(
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect2.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect3.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect4.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect5.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect4.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect3.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect2.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect1.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect1.sub",
						RUMI_ROOT + "rumi_button_effect/rumi_button_effect1.sub",
					),
				},
				
			),
		},
	),
}
