import uiScriptLocale

window = {
	"name" : "Halloween",

	"x" : SCREEN_WIDTH - 175 - 436,
	"y" : SCREEN_HEIGHT - 37 - 375,

	"style" : ("movable", "float",),

	"width" : 436,
	"height" : 375,

	"children" :
	(
		{
			"name" : "board",
			"type" : "image",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,
			
			"image" : "d:/ymir work/halloween/board.tga",

			"children" :
			(
				{
					"name" : "Exit",
					"type" : "button",

					"x" : 398,
					"y" : 15,

					"text" : "",

					"default_image" : "d:/ymir work/halloween/slot.tga",
					"over_image" : "d:/ymir work/halloween/slot_over_ov.tga",
					"down_image" : "d:/ymir work/halloween/slot_over.tga",
				},
				{
					"name" : "MiniGameBoard",
					"type" : "image",

					"x" : 14,
					"y" : 45,

					"image" : "d:/ymir work/halloween/minigame_board.tga",
				},
				{
					"name" : "grid_rewards","type" : "grid_table","x" : 72,"y" : 92,"start_index" : 0,"x_count" : 2,"y_count" : 2,"x_step" : 32,"y_step" : 32,
				},
				# Take Reward
				{
					"name" : "MiniGamePoints",
					"type" : "image",

					"x" : 334,
					"y" : 38,

					"image" : "d:/ymir work/halloween/points_image.tga",
				},
				{
					"name" : "MiniGamePointsText",
					"type" : "text",

					"x" : 367,
					"y" : 44,

					"text" : "Puan ",
				},
				{
					"name" : "MiniGamePointsText",
					"type" : "text",

					"x" : 375,
					"y" : 68,

					"text" : "200",
				},
				{
					"name" : "MiniGameLevel",
					"type" : "image",

					"x" : 337,
					"y" : 140,

					"image" : "d:/ymir work/halloween/level_image.tga",
				},
				{
					"name" : "MiniGameRewardImage",
					"type" : "image",

					"x" : 341,
					"y" : 240,

					"image" : "d:/ymir work/halloween/takereward_image.tga",
				},
				{
					"name" : "MiniGameTakeReward",
					"type" : "button",

					"x" : 333,
					"y" : 330,
					
					"tooltip_text" : "Ödülü Al ",

					"default_image" : "d:/ymir work/halloween/takereward_button.tga",
					"over_image" : "d:/ymir work/halloween/takereward_apasat.tga",
					"down_image" : "d:/ymir work/halloween/takereward_over.tga",
				},
				{
					"name" : "MiniGameIncreaseNivel",
					"type" : "button",

					"x" : 337,
					"y" : 210,

					"default_image" : "d:/ymir work/halloween/increase.tga",
					"over_image" : "d:/ymir work/halloween/increase_over.tga",
					"down_image" : "d:/ymir work/halloween/increase_apasat.tga",
				},
				{
					"name" : "MiniGameReqTextInfo",
					"type" : "text",

					"x" : 348+7,
					"y" : 141,

					"text" : "Gerekli Nesne ",
				},
				{
					"name" : "MiniGameReqText",
					"type" : "text",

					"x" : 342,
					"y" : 156,

					"text" : "ITEMS",
				},
			),
		},
	),
}