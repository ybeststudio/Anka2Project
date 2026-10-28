import uiScriptLocale

WINDOW_X = 742
WINDOW_Y = 360

PATH = "d:/ymir work/ui/weekly_rank/"
PATH2 = "d:/ymir work/ui/new_weekly_rank/"
PATH3 = "d:/ymir work/ui/biolog_gui/"
REAL_PATH = "d:/ymir work/ui/collect_window/"
PATH4 = "d:/ymir work/ui/zlomiarz_window/"

#category window
CAT_WIDTH = 164
CAT_HEIGHT = 235

#itembox
ITEMBOX_X = CAT_WIDTH+15
ITEMBOX_WIDTH = 549
ITEMBOX_HEIGHT = CAT_HEIGHT

#bonus
BONUS_WIDTH = WINDOW_X-27
BONUS_HEIGHT = 80
BONUS_Y = 267

window = {
	"name" : "CollectionsWindow",
	"x" : (SCREEN_WIDTH-WINDOW_X)/2,
	"y" : (SCREEN_HEIGHT-WINDOW_Y)/2,
	"style" : ("movable", "float",),
	"width" : WINDOW_X,
	"height" : WINDOW_Y,
	"children" :
	(
		{
			"name" : "Board",
			"type" : "board",
			"style" : ("attach",),
			"x" : 0, "y" : 0,
			"width" : WINDOW_X,
			"height" : WINDOW_Y,
			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 8,
					"y" : 7,
					"width" : WINDOW_X-15,
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":0, "text": uiScriptLocale.COLLECTION_WINDOW_TITLE, "text_horizontal_align":"center", "horizontal_align" : "center", "outline" : 1 },
					),
				},	
				{
					"name" : "ItemsBox",
					"type" : "border_a",
					"x" : ITEMBOX_X, "y" : 30,
                    # "horizontal_align":"center",
					"width" : ITEMBOX_WIDTH,
					"height" : ITEMBOX_HEIGHT,
                    "children":
                    (
                        {
							"name":"Header",
							"type":"image",
							"x":2,"y":2,
							"image":PATH4+"header_big.png",
							"children":
							(
								{
									"name":"category_text",
									"type":"text",
									"x":0,"y":0,
									"all_align" : "center",
									"text":"Savaþçý",
									"outline":"1",
								},  
							),
						},
					),
				},
                {
					"name" : "category_window",
					"type" : "border_a",
					"width" : CAT_WIDTH, "height" : CAT_HEIGHT,
                    # "horizontal_align" : "center",
					"x" : 13, "y" : 30,
					"children":
					(
                        {
							"name":"Header",
							"type":"image",
							"x":2,"y":2,
							"image":PATH2+"header_small.tga",
							"children":
							(
								{
									"name":"mission_text",
									"type":"text",
									"x":0,"y":0,
									"all_align" : "center",
									"text":"Mevcut Görevler",
									"outline":"1",
								},
							),
						},
						{
							"name" : "CategoryBox",
							"type" : "listboxex",
							"x" : 8,
							"y" : 31,
							"width" : 140,
							"height" : 82,
						},
					),
				},
                {
					"name":"bonus_window",
					"type":"border_a",
					"x":13,"y":BONUS_Y,
                    # "horizontal_align":"center",
					"width":BONUS_WIDTH,"height":BONUS_HEIGHT,
					"children":
					(
						{
							"name":"Header",
							"type":"image",
							"x":2,"y":2,
							"image":PATH4+"header_big2.png",
							"children":
							(
								{
									"name":"bonusy_text",
									"type":"text",
									"x":0,"y":0,
									"all_align" : "center",
									"text":"Tamamlama Bonuslarý",
									"outline":"1",
								},  
							),
						},
						{
							"name":"bonusbox",
							"type":"image",
							"x":17,"y":32,
							# "horizontal_align":"center",
							"image":PATH4+"bonusbox.png",
							"children":
							(
								{
									"name":"bonustext_0",
									"type":"text",
									"x":0,"y":-10,
									"all_align" : "center",
									"text":"-",
									"outline":"1",
								}, 
                                {
									"name":"bonustext_3",
									"type":"text",
									"x":0,"y":8,
									"all_align" : "center",
									"text":"-",
									"outline":"1",
								}, 
							),
						},
						{
							"name":"bonusbox",
							"type":"image",
							"x":244,"y":32,
							# "horizontal_align":"center",
							"image":PATH4+"bonusbox.png",
							"children":
							(
								{
									"name":"bonustext_1",
									"type":"text",
									"x":0,"y":-10,
									"all_align" : "center",
									"text":"-",
									"outline":"1",
								}, 
                                {
									"name":"bonustext_4",
									"type":"text",
									"x":0,"y":8,
									"all_align" : "center",
									"text":"-",
									"outline":"1",
								}, 
							),
						},
						{
							"name":"bonusbox",
							"type":"image",
							"x":471,"y":32,
							# "horizontal_align":"center",
							"image":PATH4+"bonusbox.png",
							"children":
							(
								{
									"name":"bonustext_2",
									"type":"text",
									"x":0,"y":-10,
									"all_align" : "center",
									"text":"-",
									"outline":"1",
								}, 
                                {
									"name":"bonustext_5",
									"type":"text",
									"x":0,"y":8,
									"all_align" : "center",
									"text":"-",
									"outline":"1",
								}, 
							),
						},
					),
				},
			),
		},
	),
}