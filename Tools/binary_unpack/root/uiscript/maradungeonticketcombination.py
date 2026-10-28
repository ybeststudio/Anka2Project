import uiScriptLocale

WINDOW_WIDTH		= 192
WINDOW_HEIGHT		= 154

window = {
	"name"		: "MaraDungeonTicketCombinationWindow",
	"style"		: ("movable", "float", ),
	
	"x"			: SCREEN_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2,	

	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children"	:
	(
		{
			"name"		: "board",
			"type"		: "board",
			"style"		: ("attach", ),

			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"children" :
			(
				{
					"name"		: "title_bar",
					"type"		: "titlebar",
					"style"		: ("attach", ),

					"x"			: 6,
					"y"			: 7,

					"width"		: WINDOW_WIDTH - 13,
					
					"children"	:
					(
						{
							"name"			: "question_button",
							"type"			: "button",

							"x"				: WINDOW_WIDTH - 30 - 16 - 6,
							"y"				: 2,

							"default_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
							"over_image"	: "d:/ymir work/ui/pattern/q_mark_02.tga",
							"down_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
						},

						# 입장권 조합
						{ "name" : "title_text", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.MARA_DUNGEON_TICKET_COMBINATION_TITLE, "all_align":"center" },
					),
				},

				{
					"name" : "need_yang_text",
					"type" : "text",

					"x"						: 0,
					"y"						: WINDOW_HEIGHT - 48,

					"horizontal_align"		: "center",
					"text_horizontal_align"	: "center",
					"text"					: "",
				},
				
				## Button
				{
					"name"					: "accept_button",
					"type"					: "button",

					"x"						: WINDOW_WIDTH / 2 - 65,
					"y"						: WINDOW_HEIGHT - 30,

					"text"					: uiScriptLocale.OK,

					"default_image"			: "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image"			: "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image"			: "d:/ymir work/ui/public/middle_button_03.sub",
				},

				{
					"name"					: "cancel_button",
					"type"					: "button",

					"x"						: WINDOW_WIDTH / 2 + 5,
					"y"						: WINDOW_HEIGHT - 30,

					"text"					: uiScriptLocale.CANCEL,

					"default_image"			: "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image"			: "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image"			: "d:/ymir work/ui/public/middle_button_03.sub",
				},
						
				{
					"name"		: "slot_window",
					"type"		: "window",
					"style"		: ("attach", ),
					
					"x"			: 16,
					"y"			: 34,
					
					"width"		: 160, # 32 * 5
					"height"	: 64, # 32 * 2
					"children"	:
					(								
						{
							"name"			: "slot_grid_table",
							"type"			: "grid_table",

							"x"				: 0,
							"y"				: 0,

							"start_index"	: 0,
							"x_count"		: 5,
							"y_count"		: 2,
							"x_step"		: 32,
							"y_step"		: 32,

							"image"			: "d:/ymir work/ui/public/Slot_Base.sub"
						},
					),						
				},
			),				
		},
	),
}
