ROOT_PATH			= "d:/ymir work/ui/game/mara_dungeon/information_board/"

WINDOW_WIDTH		= 137
WINDOW_HEIGHT		= 160

window = {
	"name"		: "MaraDungeonInformationBoard",
	"style"		: ("float", "ltr", ),
	
	"x"			: SCREEN_WIDTH - WINDOW_WIDTH,
	"y"			: 162,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,
	
	"children" :
	(
		{
			"name"	: "info_board_background",
			"type"	: "image",
			"style"	: ("attach", "not_pick", ),
			"x"		: 0,
			"y"		: 0,
			"image"	: ROOT_PATH + "info_board_background.sub",
		},

		# Stage
		{
			"name"		: "stage_window",
			"type"		: "window",
			"style"		: ("attach", "not_pick", ),
			"x"			: 18,
			"y"			: 27,
			"width"		: 98,
			"height"	: 24,
			
			"children"	:
			(
				{
					"name"	: "stage_icon",
					"type"	: "image",
					"x"		: 0,
					"y"		: 0,
					"image"	: ROOT_PATH + "icon_stage.sub",
				},

				{
					"name"		: "stage_text",
					"type"		: "text",
					"x"			: 21,
					"y"			: 3,
					"width"		: 77,
					"height"	: 11,
					"horizontal_align" : "center",
					"text"		: "Stage99",
				},
			),
		},

		# Mission
		{
			"name"		: "mission_window",
			"type"		: "window",
			"style"		: ("attach", "not_pick", ),
			"x"			: 18,
			"y"			: 52,
			"width"		: 98,
			"height"	: 24,
			
			"children"	:
			(
				{
					"name"	: "mission_icon",
					"type"	: "image",
					"x"		: 0,
					"y"		: 0,
					"image"	: ROOT_PATH + "icon_mission.sub",
				},

				{
					"name"		: "mission_text",
					"type"		: "text",
					"x"			: 21,
					"y"			: 3,
					"width"		: 77,
					"height"	: 11,
					"horizontal_align" : "center",
					"text"		: "Mission99",
				},
			),
		},

		# Remain Life Count
		{
			"name"		: "remain_life_count_window",
			"type"		: "window",
			"style"		: ("attach", "not_pick", ),
			"x"			: 18,
			"y"			: 77,
			"width"		: 98,
			"height"	: 24,
			
			"children"	:
			(
				{
					"name"	: "remain_life_count_icon",
					"type"	: "image",
					"x"		: 0,
					"y"		: 0,
					"image"	: ROOT_PATH + "icon_remain_life_count.sub",
				},

				{
					"name"		: "remain_life_count_text",
					"type"		: "text",
					"x"			: 21,
					"y"			: 3,
					"width"		: 77,
					"height"	: 11,
					"horizontal_align" : "center",
					"text"		: "88",
				},
			),
		},

		# Remain Time
		{
			"name"		: "remain_time_window",
			"type"		: "window",
			"style"		: ("attach", "not_pick", ),
			"x"			: 18,
			"y"			: 102,
			"width"		: 98,
			"height"	: 24,
			
			"children"	:
			(
				{
					"name"	: "remain_time_icon",
					"type"	: "image",
					"x"		: 0,
					"y"		: 0,
					"image"	: ROOT_PATH + "icon_remain_time.sub",
				},

				{
					"name"		: "remain_time_text",
					"type"		: "text",
					"style"		: ("not_pick", ),
					"x"			: 21,
					"y"			: 3,
					"width"		: 77,
					"height"	: 11,
					"horizontal_align" : "center",
					"text"		: "99:99",
				},
			),
		},

		{
			"name" : "exit_button",
			"type" : "button",

			"x" : 55,
			"y" : 121,
			
			"default_image"	: ROOT_PATH + "exit_button_default.sub",
			"over_image"	: ROOT_PATH + "exit_button_over.sub",
			"down_image"	: ROOT_PATH + "exit_button_down.sub",
		},
	),	
}
