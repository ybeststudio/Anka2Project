import uiScriptLocale

ROOT = "d:/ymir work/ui/public/"
ICON_SLOT_FILE = "d:/ymir work/ui/public/Slot_Base.sub"

window = {
	"name" : "FutbolTopuSystem",
	"style" : ("movable", "float",),
	"x" : SCREEN_WIDTH - 500-120,
	"y" : SCREEN_HEIGHT - 595 + 150,
	"width" : 136,
	"height" : 172,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"x" : 0,
			"y" : 0,
			"width" : 136,
			"height" : 172,
			"children" :
			(
				{
					"name" : "Top_TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,

					"width" : 226-108,
					"color" : "gray",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":3,  "text" : "Top Dönüþtür",  "horizontal_align":"center", "text_horizontal_align":"center" },
					),
				},
				{
					"name" : "Word_Area",
					"type" : "window",
					"style" : ("attach",),
					
					"x" : 0,
					"y" : 26,
					
					"width" : 284,
					"height" : 144,
					"children" :
					(								
						{
							"name": "ArkaPlan",
							"type": "thinboard",
							"x": 10,
							"y": 31-24,
							"width": 187-72,
							"height": 140-13,
						},
						{
							"name" : "TopItemSlot",
							"type" : "grid_table",

							"x" : 17+42-7,
							"y" : 15,

							"start_index" : 0,
							"x_count" : 1,
							"y_count" : 1,
							"x_step" : 34,
							"y_step" : 34,
							"image" : "d:/ymir work/ui/public/Slot_Base.sub"
						},
						{
							"name" : "Skill_ETC_Slot",
							"type" : "grid_table",
							"x" : 17+42-7,
							"y" : 36+35,
							"start_index" : 101,
							"x_count" : 1,
							"y_count" : 1,
							"x_step" : 32,
							"y_step" : 32,
							"x_blank" : 5,
							"y_blank" : 4,
							"image" : ICON_SLOT_FILE,
						},
						{
							"name" : "asagiimage",
							"type" : "image",
							"x" : 17+42-3,
							"y" : 36+15,
							"image" : "d:/ymir work/asagibak.tga",
						},
						{
							"name" : "image",
							"type" : "image",
							"x" : 17+42-7,
							"y" : 36+35,
							"image" : "icon/item/50265.tga",
						},
						{
							"name" : "TopButton",
							"type" : "button",
							"x" : 30-7,
							"y" : 55+53,
							"text" : "Dönüþtür",
							"default_image" : ROOT + "Large_Button_01.sub",
							"over_image" : ROOT + "Large_Button_02.sub",
							"down_image" : ROOT + "Large_Button_03.sub",
							"disable_image" : ROOT + "Large_Button_03.sub",
						},
					),						
				},
			),				
		},
	),
}
