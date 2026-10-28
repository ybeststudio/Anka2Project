import uiScriptLocale

ROOT_PATH			= "d:/ymir work/ui/game/mara_dungeon/mission_clear_board/"

window = {
	"name"		: "MaraDungeonInformationBoard",
	"style"		: ("float", "ltr", ),
	
	"x"			: 0,
	"y"			: 0,
	
	"width"		: SCREEN_WIDTH,
	"height"	: SCREEN_HEIGHT,
	
	"children" :
	(
		{
			"name"		: "model_view",
			"type"		: "render_target",
			"style"		: ("not_pick", "float", ),

			"x"			: 0,
			"y"			: 0,
					
			"width"		: SCREEN_WIDTH,
			"height"	: SCREEN_HEIGHT,

			"index"		: 3,
		},

		{
			"name"		: "reward_board",
			"type"		: "middleboard",
	
			"x"			: 0,
			"y"			: float(SCREEN_HEIGHT) * 500 / 600.0 - 230,
	
			"width"		: SCREEN_WIDTH,
			"height"	: 180,

			"children"	:
			(
				{
					"name"		: "mission_clear_window_1",
					"type"		: "window",
					"style"		: ("attach", ),
					"x"			: 0,
					"y"			: 0,
					"width"		: SCREEN_WIDTH,
					"height"	: 60,
			
					"children"	:
					(
						{
							"name"		: "mission_clear_text_1",
							"type"		: "text",
							"x"			: 0,
							"y"			: 0,
							"all_align" : "center",
							"text"		: "",
						},
					),
				},

				{
					"name"		: "mission_clear_window_2",
					"type"		: "window",
					"style"		: ("attach", ),
					"x"			: 0,
					"y"			: 60,
					"width"		: SCREEN_WIDTH,
					"height"	: 60,
			
					"children"	:
					(
						{
							"name"		: "mission_clear_text_2",
							"type"		: "text",
							"x"			: 0,
							"y"			: 0,
							"all_align" : "center",
							"text"		: "",
						},
					),
				},

				{
					"name"		: "mission_clear_window_3",
					"type"		: "window",
					"style"		: ("attach", ),
					"x"			: 0,
					"y"			: 120,
					"width"		: SCREEN_WIDTH,
					"height"	: 60,
			
					"children"	:
					(
						# 최근 획득 음양속성 슬롯
						{
							"name" : "reward_yin_and_yang_attr_slot_bg",
							"type" : "image",

							"x" : SCREEN_WIDTH / 2 - 140,
							"y" : 0,
					
							"image"		: "d:/ymir work/ui/game/yin_and_yang_attr/inventory/recently_obtained_attr_slot.sub",

							"vertical_align"	: "center",

							"children" :
							(
								# 슬롯
								{
									"name"			: "reward_yin_and_yang_attr_slot",
									"type"			: "grid_table",
									"x"				: 0,
									"y"				: 0,
									"start_index"	: 0,
									"x_count"		: 1,
									"y_count"		: 1,
									"x_step"		: 32,
									"y_step"		: 32,
								},
							),
						},

						{
							"name"		: "mission_clear_text_window_3",
							"type"		: "window",
							"style"		: ("attach", ),
							"x"			: SCREEN_WIDTH / 2 - 140 + 40,
							"y"			: 0,
							"width"		: 200,
							"height"	: 60,

							"children" :
							(
								{
									"name"		: "mission_clear_text_3",
									"type"		: "text",
									"x"			: 0,
									"y"			: 0,
									"all_align" : "center",

									"text"		: "",
								},
							),
						},

						{
							"name" : "close_button",
							"type" : "button",

							"x" : SCREEN_WIDTH - (88 + 10) ,
							"y" : 60 - (21 + 10),

							"text" : uiScriptLocale.CLOSE,

							"default_image"	: "d:/ymir work/ui/public/large_button_01.sub",
							"over_image"	: "d:/ymir work/ui/public/large_button_02.sub",
							"down_image"	: "d:/ymir work/ui/public/large_button_03.sub",
						},
					),
				},
			),
		},
	),	
}
