import uiScriptLocale

ROOT = "d:/ymir work/ui/minigame/roulette/"

WINDOW_WIDTH	= 312
WINDOW_HEIGHT	= 330

ITEM_SLOT_AREA_WIDTH	= 252
ITEM_SLOT_AREA_HEIGHT	= 252

window = {
	"name" : "roulette_window",
	"style" : ("movable",),
	
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	
	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),
			
			"x" : 0,
			"y" : 0,
			
			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,
			
			"title" : uiScriptLocale.ROULETTE_TITLE,
			
			"children" :
			(
				{
					"name" : "main_bg",
					"type" : "image",
					"style" : ("attach",),

					"x" : 13,
					"y" : 35,

					"image" : ROOT + "main_bg.sub",
				},
				
				# Item Slot
				{
					"name" : "item_slot",
					"type" : "slot",
					"style" : ("attach",),
			
					"x" : 29,
					"y" : 50,
			
					"width" : ITEM_SLOT_AREA_WIDTH,
					"height" : ITEM_SLOT_AREA_HEIGHT,
			
					# Clockwise
					"slot" : (
						{"index":0, "x":44*0,	"y":0, "width":32, "height":32},
						{"index":1, "x":44*1,	"y":0, "width":32, "height":32},
						{"index":2, "x":44*2,	"y":0, "width":32, "height":32},
						{"index":3, "x":44*3,	"y":0, "width":32, "height":32},
						{"index":4, "x":44*4,	"y":0, "width":32, "height":32},
						{"index":5, "x":44*5,	"y":0, "width":32, "height":32},
						
						{"index":6, "x":44*5,	"y":44*1, "width":32, "height":32},
						{"index":7, "x":44*5,	"y":44*2, "width":32, "height":32},
						{"index":8, "x":44*5,	"y":44*3, "width":32, "height":32},
						{"index":9, "x":44*5,	"y":44*4, "width":32, "height":32},
						
						{"index":10, "x":44*5,	"y":44*5, "width":32, "height":32},
						{"index":11, "x":44*4,	"y":44*5, "width":32, "height":32},
						{"index":12, "x":44*3,	"y":44*5, "width":32, "height":32},
						{"index":13, "x":44*2,	"y":44*5, "width":32, "height":32},
						{"index":14, "x":44*1,	"y":44*5, "width":32, "height":32},
						{"index":15, "x":44*0,	"y":44*5, "width":32, "height":32},
						
						{"index":16, "x":44*0,	"y":44*4, "width":32, "height":32},
						{"index":17, "x":44*0,	"y":44*3, "width":32, "height":32},
						{"index":18, "x":44*0,	"y":44*2, "width":32, "height":32},
						{"index":19, "x":44*0,	"y":44*1, "width":32, "height":32},
					),
				},
				
				# spin button
				{
					"name" : "spin_button",
					"type" : "button",

					"x" : 108,
					"y" : 129,

					"default_image" : ROOT + "spin_button_default.sub",
					"over_image" : ROOT + "spin_button_over.sub",
					"down_image" : ROOT + "spin_button_down.sub",
				},
				## support slot text
				{ 
					"name" : "spin_button_text_window1", "type" : "window", "x" : 120, "y" : 151, "width" : 70, "height" : 16, "style" : ("attach", "not_pick"),
					"children" :
					(
						{"name":"spin_button_text1", "type":"text", "x":0, "y":0, "text": uiScriptLocale.ROULETTE_SPIN_TEXT1, "outline" : 1, "all_align" : "center", "color" : 0xFFD8CAC2, },
					),	
				},
				{ 
					"name" : "spin_button_text_window2", "type" : "window", "x" : 111, "y" : 168, "width" : 87, "height" : 16, "style" : ("attach", "not_pick"),
					"children" :
					(
						{"name":"spin_button_text2", "type":"text", "x":0, "y":0, "text": uiScriptLocale.ROULETTE_SPIN_TEXT2, "outline" : 1, "all_align" : "center", "color" : 0xFFD8CAC2, },
					),	
				},
				{ 
					"name" : "spin_button_text_window3", "type" : "window", "x" : 120, "y" : 185, "width" : 70, "height" : 16, "style" : ("attach", "not_pick"),
					"children" :
					(
						{"name":"spin_button_text3", "type":"text", "x":0, "y":0, "text": uiScriptLocale.ROULETTE_SPIN_TEXT3, "outline" : 1, "all_align" : "center", "color" : 0xFFD8CAC2, },
					),	
				},
				
				{
					"name" : "slot_edge_effect",
					"type" : "image",
					"style" : ("not_pick",),

					"x" : 25,
					"y" : 46,

					"image" : ROOT + "slot_edge.sub",
				},
			),
		},
	),	
}
