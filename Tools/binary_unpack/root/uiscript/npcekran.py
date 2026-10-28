import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/"
ROOT_YENI = "remoteshop_2022/"
TEMPORARY_X = +13
TEXT_TEMPORARY_X = -10
BUTTON_TEMPORARY_X = 5
PVP_X = -10

window = {
	"name" : "GameOptionDialog",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : 537,
	"height" : 467,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 537,
			"height" : 467,

			"children" :
			(
				## Title
				{
					"name" : "titlebar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,

					"width" : 537 - 15,
					"color" : "gray",

					"children" :
					(
						{ 
						"name":"titlename", "type":"text", "x":0, "y":3, 
						"horizontal_align":"center", "text_horizontal_align":"center",
						"text" : "Uzak Market",
						 },
					),
				},
				{
					"name" : "BlackBoard1",
					"type" : "thinboard_circle",
					"x" : 10, "y" : 36, "width" : 267, "height" : 422,
					"children":
					(
						{
							"name" : "ScrollBar",
							"type" : "scrollbar",

							"x" : 18,
							"y" : 0,
							"size" : 425,
							"horizontal_align" : "right",
						},
					),
				},
				{
					"name" : "BlackBoard2",
					"type" : "thinboard_circle",
					"x" : 280, "y" : 36, "width" :196 + 30 + 23, "height" : 422,
					"children":
					(
						{
							"name" : "ModelView",
							"type" : "image",
							
							"x" : 3, "y" : 2,
							"image" : ROOT_YENI + "butonlar/title.png",
							
							"children" :
							(
								{ "name" : "test", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align":"center" },
							),
						},
						{
							"name" : "OpenButton", "type" : "button",
							"x" : 35, "y" : 9+(30*6)+33 + 170, "text": "Aç",
							"text_height" : 6,


							"default_image" : ROOT_YENI + "butonlar/btn_norm.png",
							"over_image" : ROOT_YENI + "butonlar/btn_over.png",
							"down_image" : ROOT_YENI + "butonlar/btn_down.png",
						},
						{
							"name" : "cancel", "type" : "button",
							"x" : 128, "y" : 9+(30*6)+33 + 170, "text": "Kapat",
							"text_height" : 6,


							"default_image" : ROOT_YENI + "butonlar/btn_norm.png",
							"over_image" : ROOT_YENI + "butonlar/btn_over.png",
							"down_image" : ROOT_YENI + "butonlar/btn_down.png",
						},

					),
				},
			),
		},

	),
}
