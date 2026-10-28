import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/mercenary/"
BOARD_WIDTH		= 632
BOARD_HEIGHT	= 520

window = {
	"name" : "MercenaryWindow",
	"style" : ("movable", "float",),
	
	"x" : SCREEN_WIDTH / 2 - BOARD_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - BOARD_HEIGHT / 2,

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	[
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,
				
			"title" : uiScriptLocale.MERCENARY_TITLE,

			"children" :
			[
				## Tab Area
				{
					"name" : "TabControl",
					"type" : "window",

					"x" : 0,
					"y" : 483,

					"width" : BOARD_WIDTH,
					"height" : 53,

					"children" :
					[
						## Tab
						## mercenary mission
						{
							"name" : "Tab_Img_01",
							"type" : "image",

							"x" : 0,
							"y" : 0,

							"width" : BOARD_WIDTH,
							"height" : 53,

							"image" : ROOT_PATH + "mercenary_mission_tab.sub",
						},
						## mercenary manage
						{
							"name" : "Tab_Img_02",
							"type" : "image",

							"x" : 0,
							"y" : 0,

							"width" : BOARD_WIDTH,
							"height" : 53,

							"image" : ROOT_PATH + "mercenary_manage_tab.sub",
						},
						## RadioButton
						{
							"name" : "Tab_Btn_01",
							"type" : "radio_button",

							"x" : 22,
							"y" : 13,

							"width" : 77,
							"height" : 30,
						},
						{
							"name" : "Tab_Btn_02",
							"type" : "radio_button",

							"x" : 133,
							"y" : 13,

							"width" : 77,
							"height" : 30,
						},
					],
				},
			],
		},
	],
}