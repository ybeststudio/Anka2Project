import uiScriptLocale

ROOT = "d:/ymir work/ui/public/"

window = {
	"name" : "WordWindowSystem",
	"style" : ("movable", "float",),
	"x" : SCREEN_WIDTH - 500-120,
	"y" : SCREEN_HEIGHT - 595 + 150,
	"width" : 240,
	"height" : 130,
	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"x" : 0,
			"y" : 0,
			"width" : 240,
			"height" : 130,
			"children" :
			(
				{
					"name" : "Word_TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,

					"width" : 226,
					"color" : "gray",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":3,  "text" : "Kelime Eventi",  "horizontal_align":"center", "text_horizontal_align":"center" },
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
							
							"image" : "d:/ymir work/tga/newlove.tga"
						},
						{
							"name" : "WordItemSlot",
							"type" : "grid_table",

							"x" : 17,
							"y" : 15,

							"start_index" : 0,
							"x_count" : 6,
							"y_count" : 1,
							"x_step" : 34,
							"y_step" : 34,
							"image" : "d:/ymir work/ui/public/Slot_Base.sub"
						},
						
						{
							"name" : "WordButton",
							"type" : "button",
							"x" : 30,
							"y" : 55,
							"text" : "Kabul",
							"default_image" : ROOT + "XLarge_Button_01.sub",
							"over_image" : ROOT + "XLarge_Button_02.sub",
							"down_image" : ROOT + "XLarge_Button_03.sub",
							"disable_image" : ROOT + "XLarge_Button_03.sub",
						},
					),						
				},
			),				
		},
	),
}
