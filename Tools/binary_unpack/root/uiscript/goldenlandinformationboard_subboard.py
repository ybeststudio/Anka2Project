ROOT_PATH			= "d:/ymir work/ui/game/golden_land/information_board/"

WINDOW_WIDTH		= 76
WINDOW_HEIGHT		= 132

window = {
	"name"		: "GoldenLandInformationBoard_SubBoard",
	"style"		: ("float", "ltr", ),
	
	"x"			: 0,
	"y"			: 0,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,
	
	"children" :
	(
		# 남은 석상
		{
			"name"	: "count_list_bg_top",
			"type"	: "image",
			"x"		: 0,
			"y"		: 0,
			"image"	: ROOT_PATH + "count_list_bg_top.sub",

			"children" :
			(
				{
					"name"	: "count_icon_stones",
					"type"	: "image",
					"x"		: 17,
					"y"		: 8,
					"image"	: ROOT_PATH + "count_icon_stones.sub",
				},

				{	"name"		: "stone_count_text",
					"type"		: "text",
					"x"			: 21,
					"y"			: 14,
					"width"		: 31,
					"height"	: 10,
					"horizontal_align" : "center",
					"text"		: "",
				},
			),
		},

		# 남은 지뢰탑
		{
			"name"	: "count_list_bg_middle_1",
			"type"	: "image",
			"x"		: 0,
			"y"		: 34,
			"image"	: ROOT_PATH + "count_list_bg_middle.sub",

			"children" :
			(
				{
					"name"	: "count_icon_mine_tower",
					"type"	: "image",
					"x"		: 15,
					"y"		: 4,
					"image"	: ROOT_PATH + "count_icon_mine_tower.sub",
				},

				{	"name"		: "mine_tower_count_text",
					"type"		: "text",
					"x"			: 21,
					"y"			: 10,
					"width"		: 31,
					"height"	: 10,
					"horizontal_align" : "center",
					"text"		: "",
				},
			),
		},

		# 남은 노란열매
		{
			"name"	: "count_list_bg_middle_2",
			"type"	: "image",
			"x"		: 0,
			"y"		: 64,
			"image"	: ROOT_PATH + "count_list_bg_middle.sub",

			"children" :
			(
				{
					"name"	: "count_icon_yellow_fruit",
					"type"	: "image",
					"x"		: 17,
					"y"		: 4,
					"image"	: ROOT_PATH + "count_icon_yellow_fruit.sub",
				},

				{	"name"		: "yellow_fruit_count_text",
					"type"		: "text",
					"x"			: 21,
					"y"			: 10,
					"width"		: 31,
					"height"	: 10,
					"horizontal_align" : "center",
					"text"		: "",
				},
			),
		},

		# 남은 나무열매
		{
			"name"	: "count_list_bg_bottom",
			"type"	: "image",
			"x"		: 0,
			"y"		: 94,
			"image"	: ROOT_PATH + "count_list_bg_bottom.sub",

			"children" :
			(
				{
					"name"	: "count_icon_tree_fruit",
					"type"	: "image",
					"x"		: 17,
					"y"		: 4,
					"image"	: ROOT_PATH + "count_icon_tree_fruit.sub",
				},

				{	"name"		: "tree_fruit_count_text",
					"type"		: "text",
					"x"			: 21,
					"y"			: 10,
					"width"		: 31,
					"height"	: 10,
					"horizontal_align" : "center",
					"text"		: "",
				},
			),
		},
	),	
}
