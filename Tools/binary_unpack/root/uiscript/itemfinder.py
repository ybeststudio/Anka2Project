import uiScriptLocale
import localeInfo
LOCALE_PATH = "d:/ymir work/ui/itemfinder/"
window = {
	"name" : "ItemFinder",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : 464,
	"height" : 380,

	"children" :
	(
		## Board and buttons
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 464,
			"height" : 380,

			"children" :
			(
				## TitleBar
				{"name" : "TitleBar","type" : "titlebar","style" : ("attach",),"x" : 8,"y" : 8,"width" : 464 - 15,"color" : "gray",
					"children" :
					(
						{ 
							"name":"TitleName", "type":"text", "x" : (464 - 15) / 2, "y":4, "text":"Drop Arama Sistemi", "text_horizontal_align":"center" 
						},
					),
				},
				{
					"name" : "Quest_BoardA",
					"type" : "border_a",
					
					"x" : 10,
					"y" : 35,

        			"width" : 433,
        			"height" : 49,
				},
				{
					"name" : "search_button",
					"type" : "button",

					"x" : -30,
					"y" : 48,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/itemfinder/search_button_default.tga",
					"over_image" : "d:/ymir work/ui/itemfinder/search_button_over.tga",
					"down_image" : "d:/ymir work/ui/itemfinder/search_button_down.tga",
				},
				{
					"name" : "clear_button",
					"type" : "button",

					"x" : 60,
					"y" : 48,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/itemfinder/clear_button_default.tga",
					"over_image" : "d:/ymir work/ui/itemfinder/clear_button_over.tga",
					"down_image" : "d:/ymir work/ui/itemfinder/clear_button_down.tga",
				},
				{
					"name" : "bg_findere",
					"type" : "image",
					"x" : 10,
					"y" : 83,
					"image" : LOCALE_PATH+"board_finder.tga",
				},
				{
					"name" : "BG2",
					"type" : "image",
					"x" : 235,
					"y" : 90,
					"image" : LOCALE_PATH+"preview_board.tga",
				},
				{
					"name" : "ScrollBar","type" : "scrollbar","x" : 220,"y" : 90,"size" : 268,
				},
				{ 
					"name" : "TextPreviewTtiel", "type" : "text", "x" : 325, "y" : 99, "text" : "Önizleme", 
				},
				{
					"name" : "ItemNameImg",
					"type" : "image",
					"x" : 19,
					"y" : 275-238,
					"image" : LOCALE_PATH+"private_leftNameImg.sub",
					"children" :
					(
						{ "name" : "ItemNameText", "type" : "text", "text_horizontal_align":"center", "x" : 60, "y" : 5, "text" : "Eþya Adý:", #"color":0xFFFEE3AE 
						},
					),
				},
				
				{
					"name" : "ItemNameSlot",
					"type" : "image",
					"x" : 19,
					"y" : 295-238,
					"image" : LOCALE_PATH+"private_leftSlotImg.sub",
					"children" :
					(
						{
							"name" : "ItemNameValue",
							"type" : "editline",
							"x" : 3,
							"y" : 3,
							"width" : 136,
							"height" : 15,
							"input_limit" : 20,
							"text" : "",
						},
					),
				},
	
			),
		},
	),
}