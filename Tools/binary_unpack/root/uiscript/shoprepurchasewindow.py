import uiScriptLocale

WINDOW_WIDTH			= 156
WINDOW_HEIGHT			= 356

# Repurchase Shop UI
window = {
	"name" : "shop_repurchase_window",

	"x" : SCREEN_WIDTH - 400 - 156 - 10,
	"y" : 10,

	"style" : ("movable", "float",),

	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children" :
	[
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",

			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"title"		: uiScriptLocale.SHOP_REPURCHASE_TITLE,	# 재매입

			"children" :
			[
				{
					"name"			: "help_button",
					"type"			: "button",

					"x"				: WINDOW_WIDTH - 50,
					"y"				: 9,

					"default_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
					"over_image"	: "d:/ymir work/ui/pattern/q_mark_02.tga",
					"down_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
				},

				{
					"name"			: "repurchase_slots_bg_0",
					"type"			: "grid_table",

					"x"				: 14,
					"y"				: 37,

					"start_index"	: 0,
					"x_count"		: 4,
					"y_count"		: 3,
					"x_step"		: 32,
					"y_step"		: 32,
					"x_blank"		: 0,
					"y_blank"		: 0,

					"image" : "d:/ymir work/ui/public/slot_base.sub",
				},

				{
					"name"			: "repurchase_slots_bg_1",
					"type"			: "grid_table",

					"x"				: 14,
					"y"				: 141,

					"start_index"	: 12,
					"x_count"		: 4,
					"y_count"		: 3,
					"x_step"		: 32,
					"y_step"		: 32,
					"x_blank"		: 0,
					"y_blank"		: 0,

					"image" : "d:/ymir work/ui/public/slot_base.sub",
				},

				{
					"name"			: "repurchase_slots_bg_2",
					"type"			: "grid_table",

					"x"				: 14,
					"y"				: 245,

					"start_index"	: 24,
					"x_count"		: 4,
					"y_count"		: 3,
					"x_step"		: 32,
					"y_step"		: 32,
					"x_blank"		: 0,
					"y_blank"		: 0,

					"image" : "d:/ymir work/ui/public/slot_base.sub",
				},

				{
					"name" : "repurchase_slot_list",
					"type" : "slot",

					"x" : 8,
					"y" : 30,

					"width" : WINDOW_WIDTH - 16,
					"height" : WINDOW_HEIGHT - 32 - 10,
					
					"slot" : ( 
						{ "index":0,	"x":6 + (32 * 0), "y":6 + (105 * 0), "width":32, "height":96 }, 
						{ "index":1,	"x":6 + (32 * 1), "y":6 + (105 * 0), "width":32, "height":96 }, 
						{ "index":2,	"x":6 + (32 * 2), "y":6 + (105 * 0), "width":32, "height":96 }, 
						{ "index":3,	"x":6 + (32 * 3), "y":6 + (105 * 0), "width":32, "height":96 }, 

						{ "index":4,	"x":6 + (32 * 0), "y":6 + (105 * 1), "width":32, "height":96 }, 
						{ "index":5,	"x":6 + (32 * 1), "y":6 + (105 * 1), "width":32, "height":96 }, 
						{ "index":6,	"x":6 + (32 * 2), "y":6 + (105 * 1), "width":32, "height":96 }, 
						{ "index":7,	"x":6 + (32 * 3), "y":6 + (105 * 1), "width":32, "height":96 }, 

						{ "index":8,	"x":6 + (32 * 0), "y":6 + (105 * 2), "width":32, "height":96 }, 
						{ "index":9,	"x":6 + (32 * 1), "y":6 + (105 * 2), "width":32, "height":96 }, 
						{ "index":10,	"x":6 + (32 * 2), "y":6 + (105 * 2), "width":32, "height":96 }, 
						{ "index":11,	"x":6 + (32 * 3), "y":6 + (105 * 2), "width":32, "height":96 }, 
					)
				},
			],
		},
	],
}
