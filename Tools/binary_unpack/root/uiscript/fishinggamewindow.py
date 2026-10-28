import localeInfo

ROOT_PATH = "d:/ymir work/ui/game/fishing/"
WINDOW_WIDTH = 278 + 14
WINDOW_HEIGHT = 226 + 37

window = {
	"name" : "FishingGameWindow",
	"style" : ("movable", "float", "limit", ),

	"x" : 0,
	"y" : 0,

	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children" :
	[
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("ltr", ),
			"x" : 0,
			"y" : 0,
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			"title" : localeInfo.FISHGAME_TITLE,
			"children" :
			(
				## BackGround Water
				{
					"name" : "fishing_background_water",
					"type" : "image",
					"style" : ("ltr", ),
					"x" : 7,
					"y" : 29,
					"image" : ROOT_PATH + "fishing_background_water.sub",
					"children" :
					(
						## BackGround Navigation Area
						{
							"name" : "fishing_water_navArea",
							"type" : "box",
							"style" : ("not_pick",),
							"x" : 15,
							"y" : 15,
							"width" : 278 - 30,
							"height" : 226 - 30,
							"color" : 0xFF35302D,
							"children":
							(
								{
									"name" : "Fish",
									"type" : "ani_image",
									"style" : ("not_pick",),
									"x" : (278 - 30)/2,
									"y" : 0,
									"images" :
									(
										ROOT_PATH + "fish/fishing_fish_1.sub",
										ROOT_PATH + "fish/fishing_fish_2.sub",
										ROOT_PATH + "fish/fishing_fish_3.sub",
										ROOT_PATH + "fish/fishing_fish_4.sub",
									)
								},
								{
									"name" : "waveEffect",
									"type" : "ani_image",
									"style" : ("not_pick",),
									"x" : (278 - 30)/2,
									"y" : 0,
									"images" :
									(
										ROOT_PATH + "wave/fishing_effect_wave_1.sub",
										ROOT_PATH + "wave/fishing_effect_wave_2.sub",
										ROOT_PATH + "wave/fishing_effect_wave_3.sub",
										ROOT_PATH + "wave/fishing_effect_wave_4.sub",
									)
								},
							),
						},
						
						## Goal Circle
						{
							"name" : "fishing_goal_circle",
							"type" : "image",
							"style" : ("not_pick",),
							"x" : 68,
							"y" : 42,
							"image" : ROOT_PATH + "fishing_goal_circle.sub",
						},
						
						# Debug Text
						{ 
							"name" : "debug_text_fish_pos",
							"type" : "text", 
							"style" : ("not_pick",),
							"x" : 15,
							"y" :  5 + 8, 
							"text" : "",
						},

						{
							"name" : "debug_text_circle_pos",
							"type" : "text", 
							"style" : ("not_pick",),
							"x" : 15,
							"y" :  5 + 8 + 10, 
							"text" : "",
						},

						{
							"name" : "debug_text_mouse_pos",
							"type" : "text", 
							"style" : ("not_pick",),
							"x" : 15,
							"y" :  5 + 8 + 20, 
							"text" : "",
						},
					),
				},

				{
					"name" : "fishing_goal_count",
					"type" : "expanded_image",
					"style" : ("not_pick",),
					"x" : 7 + 278 - 73,
					"y" : 29 + 5,
					"image" : ROOT_PATH + "fishing_goal_count.sub",

					"children" :
					(
						{
							"name" : "fishing_goal_count_text",
							"type" : "text",
							"style" : ("ltr", ),
							"x" : 45,
							"y" : 8, 
							"text" : "0/3",
							"text_horizontal_align" : "center" 	
						},
					),
				},

				## Timer Base
				{
					"name" : "fishing_timer_baseImg",
					"type" : "image",
					"style" : ("not_pick",),
					"x" : 7 + 28,
					"y" : 223,
					"image" : ROOT_PATH + "fishing_timer_base.sub",
				},

				## Timer Gauge type expand_image
				{
					"name" : "fishing_timer_gauge",
					"type" : "expanded_image",
					"style" : ("not_pick",),
					"x" : 7 + 28,
					"y" : 223,
					"image" : ROOT_PATH + "fishing_timer_gauge.sub",
				},
			),
		},
	],
}
