# import uiScriptLocale
PATH = "d:/ymir work/ui/weekly_rank/"
PATH2 = "d:/ymir work/ui/new_weekly_rank/"
PATH3 = "d:/ymir work/ui/biolog_gui/"
REAL_PATH = "d:/ymir work/ui/collect_window/"

#board
BOARD_WIDTH = 694
BOARD_HEIGHT = 260
ALL_HEIGHT = 155

#category window
CAT_WIDTH = 164
CAT_POS_X = 12
CAT_POS_Y = 24
CATEGORY_BUTTON_POSX = 0
CATEGORY_BUTTON_POSY = 27
POS_ADD = 24
CATEGORY_BUTTON_NAME = [
	"Biyolog",
	"Koleksiyoncu",
	"Canavar",
	"Boss",
	"Metin",
]
CATEGORY_TEXT_FONTNAME = "Tahoma:14"

#quest window
MAIN_WIDTH = 295
MAIN_POS_X = CAT_WIDTH+15
MAIN_POS_Y = CAT_POS_Y
MAIN_HEIGHT = 126
#sub quest window
SUB_WIDTH = MAIN_WIDTH
SUB_HEIGHT = 55
SUB_Y = 99
#buttons
SUB_BUTTON_X = 42
SUB_BUTTON_Y = 71

#render window
RENDER_WIDTH = 204
RENDER_HEIGHT = BOARD_HEIGHT-37
RENDER_X = 477
RENDER_Y = CAT_POS_Y

#bonus window
BONUS_Y = 152
BONUS_HEIGHT = 95

window = {
	"name" : "CollectWindow",
    "style" : ("movable", "float",),
	# "x" : 0,
	# "y" : 0,
	"x" : SCREEN_WIDTH/4,
	"y" : SCREEN_HEIGHT/7,
	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,
	"children" :
	(
		{
			"name" : "category_board",
			"type" : "board",
			"width" : BOARD_WIDTH, "height" : BOARD_HEIGHT,
			"x" : 0, "y" : 0,
			"children":
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 0,"y" : 0,
					"width" : BOARD_WIDTH-2,
					"color" : "yellow",
					"children" :
					(
						{ 
							"name": "TitleName",
							"type":"text",
							"x":0,"y":-5,
							"all_align":"center",
							"text": "Görev Paneli",
							"outline":"1",
						},
					),
				},
				{
					"name" : "category_window",
					"type" : "border_a",
					"width" : CAT_WIDTH, "height" : ALL_HEIGHT,
					"x" : CAT_POS_X, "y" : CAT_POS_Y,
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
									"name":"header_text",
									"type":"text",
									"x":0,"y":0,
									"all_align" : "center",
									"text": "Mevcut Görevler",
									"outline":"1",
								},  
							),
						},
						{
							"name" : "CategoryButton_0",
							"type" : "radio_button",
							"x": 0, "y":CATEGORY_BUTTON_POSY,
							"horizontal_align":"center",
							"default_image" : PATH+"button_new1.tga",
							"over_image" : PATH+"button_new3.tga",
							"down_image" : PATH+"button_new2.tga",
							"children":
							(
								{
									"name":"categorytext0","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[0],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
								},  
							),
						},  
						{
							"name" : "CategoryButton_1",
							"type" : "radio_button",
							"x": 0, "y":CATEGORY_BUTTON_POSY+POS_ADD,
							"horizontal_align":"center",
							"default_image" : PATH+"button_new1.tga",
							"over_image" : PATH+"button_new3.tga",
							"down_image" : PATH+"button_new2.tga",
							"children":
							(
								{
									"name":"categorytext1","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[1],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
								},  
							),
						},  
						{
							"name" : "CategoryButton_2",
							"type" : "radio_button",
							"x": 0, "y":CATEGORY_BUTTON_POSY+POS_ADD*2,
							"horizontal_align":"center",
							"default_image" : PATH+"button_new1.tga",
							"over_image" : PATH+"button_new3.tga",
							"down_image" : PATH+"button_new2.tga",
							"children":
							(
								{
									"name":"categorytext2","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[2],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
								},  
							),
						},  
						{
							"name" : "CategoryButton_3",
							"type" : "radio_button",
							"x": 0, "y":CATEGORY_BUTTON_POSY+POS_ADD*3,
							"horizontal_align":"center",
							"default_image" : PATH+"button_new1.tga",
							"over_image" : PATH+"button_new3.tga",
							"down_image" : PATH+"button_new2.tga",
							"children":
							(
								{
									"name":"categorytext3","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[3],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
								},  
							),
						},  
						{
							"name" : "CategoryButton_4",
							"type" : "radio_button",
							"x": 0, "y":CATEGORY_BUTTON_POSY+POS_ADD*4,
							"horizontal_align":"center",
							"default_image" : PATH+"button_new1.tga",
							"over_image" : PATH+"button_new3.tga",
							"down_image" : PATH+"button_new2.tga",
							"children":
							(
								{
									"name":"categorytext4","type":"text","x":0,"y":0,"all_align" : "center","text":CATEGORY_BUTTON_NAME[4],"fontname":CATEGORY_TEXT_FONTNAME,"outline":"1",
								},  
							),
						},  
					),
				},
				# {
                #     "name":"separator",
                #     "type":"image",
                #     "x":27,"y":190,
                #     "image":"d:/ymir work/ui/bar.png",
                # },
				{
					"name" : "page_window",
					"type" : "border_a",
					"width" : CAT_WIDTH, "height" : 65,
					"x" : CAT_POS_X, "y" : 182,
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
									"name":"header_text",
									"type":"text",
									"x":0,"y":0,
									"all_align" : "center",
									"text": "Görev Seçimi",
									"outline":"1",
								},  
							),
						},
						{
							"name":"PageSlotbar", "type":"image",
							"x": 0, "y": 31,
							"horizontal_align":"center",
							"image":"d:/ymir work/ui/location_window/slotbar-84x23.png",

							"tooltip_text_new": "Mevcut Görev",
							"tooltip_text_color" : 0xfff1e6c0,
							"tooltip_x" : 0, "tooltip_y" : -35,
							"children": 
							(
								{
									"name":"PrevPageBtn", "type":"button",
									"x": 10, "y": 5,
									"default_image":"d:/ymir work/ui/location_window/btn-left-0.png",
									"over_image":"d:/ymir work/ui/location_window/btn-left-1.png",
									"down_image":"d:/ymir work/ui/location_window/btn-left-2.png",
								},
								{
									"name":"PageText","type":"text",
									"x":0,"y":0,
									"all_align":"center",
									"text":"0/0",
									"fontname":"Tahoma:14b",
									"outline":"1",
								},
								{
									"name":"NextPageBtn", "type":"button",
									"x": 60, "y": 5,
									"default_image":"d:/ymir work/ui/location_window/btn-right-0.png",
									"over_image":"d:/ymir work/ui/location_window/btn-right-1.png",
									"down_image":"d:/ymir work/ui/location_window/btn-right-2.png",
								},  
							),
						},
					),
				},
				{
					"name" : "main_window",
					"type" : "border_a",
					"width" : MAIN_WIDTH, "height" : MAIN_HEIGHT,
					"x" : MAIN_POS_X, "y" : MAIN_POS_Y,
					"children":
					(
						{
							"name":"Header",
							"type":"image",
							"x":2,"y":2,
							"image":REAL_PATH+"header_biggest.png",
							"children":
							(
								{
									"name":"header_text_main",
									"type":"text",
									"x":0,"y":0,
									"all_align" : "center",
									"text":"-",
									"outline":"1",
								},  
							),
						},
						{
							"name":"base",
							"type":"image",
							"x":0,"y":28,
							"horizontal_align":"center",
							"image":REAL_PATH+"base.png",
							"children":
							(
								{
									"name" : "item_slot",
									"type" : "slot",
									"image" : "d:/ymir work/ui/Public/Slot_Base.sub",
									"x" : -4,
									"y" : -1,
									"width" : 32,
									"height" : 32,
									"slot" : ( {"index":0, "x":3, "y":2, "width":32, "height":32}, )
								},
								{
									"name" : "blocked_slot",
									"type" : "image",
									"x": 1, "y" : 3,
									"image" : "d:/ymir work/ui/slot_off.png",
								},

								{
									"name" : "time_left", 
									"type" : "text", 
									"x" : 77, 
									"y" : 0, 
									"all_align" : "center", 
									"text" : "00:00:00",
									"outline":1,
								},
								{
									"name" : "item_count", 
									"type" : "text", 
									"x" : -9, 
									"y" : 0, 
									"all_align":"center",
									"text" : "0/0",
								},
								{
									"name" : "chance_text", 
									"type" : "text", 
									"x" : -70, 
									"y" : 0, 
									"all_align" : "center", 
									"text" : "100%",
								},
							),
						},
						{
							"name" : "collect_button",
							"type" : "button",
							"x" : SUB_BUTTON_X,
							"y" : SUB_BUTTON_Y,
							#"horizontal_align" : "center",
							"default_image" : REAL_PATH+"collect0.png",
							"over_image" : REAL_PATH+"collect1.png",
							"down_image" : REAL_PATH+"collect2.png",

							"tooltip_text_new": "Eþyayý Teslim Et",
							"tooltip_text_color" : 0xfff1e6c0,
							"tooltip_x" : 0,
							"tooltip_y" : -35,
						},
						{
							"name" : "time_button1",
							"type" : "button",
							"x" : SUB_BUTTON_X+118,"y" : SUB_BUTTON_Y,
							# "horizontal_align" : "center",
							"default_image" : REAL_PATH+"reduce_time3.png",
							"over_image" : REAL_PATH+"reduce_time4.png",
							"down_image" : REAL_PATH+"reduce_time5.png",

							"tooltip_text_new": "Süreyi Azalt (%25)",
							"tooltip_text_color" : 0xfff1e6c0,
							"tooltip_x" : 0,
							"tooltip_y" : -35,
						},
						{
							"name" : "chance_button",
							"type" : "button",
							"x" : SUB_BUTTON_X+168,"y" : SUB_BUTTON_Y,
							# "horizontal_align" : "center",
							"default_image" : REAL_PATH+"increase_chance0.png",
							"over_image" : REAL_PATH+"increase_chance1.png",
							"down_image" : REAL_PATH+"increase_chance2.png",

							"tooltip_text_new": "Þansý Artýr (%25)",
							"tooltip_text_color" : 0xfff1e6c0,
							"tooltip_x" : 0,
							"tooltip_y" : -35,
						},
						# {
						#     "name":"separator",
						#     "type":"image",
						#     "x":0,"y":90,
						# 	"horizontal_align":"center",
						#     "image":"d:/ymir work/ui/bar.png",
						# },
						{
							"name":"progressbar_img",
							"type":"image",
							"x":0,"y":97,
							"horizontal_align":"center",
							"image":REAL_PATH+"progress_bar.png",
							"children":
							(
								{
									"name":"progressbar",
									"type":"expanded_image",
									"x":0,"y":1,
									"horizontal_align":"center",
									"vertical_align":"center",
									"image":REAL_PATH+"progress_bar2.png",
								},
							),
						},
						{
							"name":"progress_window",
							"type":"window",
							"x":0,"y":100,
							"width":245,"height":17,
							"horizontal_align":"center",
						},
					),
				},
				{
					"name" : "render_window",
					"type" : "border_a",
					"width" : RENDER_WIDTH, "height" : RENDER_HEIGHT,
					"x" : RENDER_X, "y" : RENDER_Y,
					"children":
					(
						{
							"name":"Header_Big",
							"type":"image",
							"x":2,"y":2,
							"image":"d:/ymir work/ui/separator_big.tga",
							"children":
							(
								{
									"name":"mobname_text",
									"type":"text",
									"x":0,"y":0,
									"all_align" : "center",
									"text": "Veri Yok",
									"outline":"1",
								},  
							),
						},
						# {
						# 	"name" : "render_button",
						# 	"type" : "toggle_button",
						# 	"x" : RENDER_WIDTH-22,
						# 	"y" : 5,
						# 	# "vertical_align":"center",
						# 	# "horizontal_align":"center",
						# 	"default_image" : "d:/ymir work/ui/new_weekly_rank/select_title0.png",
						# 	"over_image" : "d:/ymir work/ui/new_weekly_rank/select_title1.png",
						# 	"down_image" : "d:/ymir work/ui/new_weekly_rank/select_title2.png",

						# 	"tooltip_text_new" : "Podgl¹d modelu",
						# 	"tooltip_text_color" : 0xfff1e6c0,
						# 	"tooltip_x" : 0,
						# 	"tooltip_y" : -35,
						# },
						{
							"name" : "RenderTarget",
							"type" : "render_target",
							"x" : 2, "y" : 21,
							"width" : RENDER_WIDTH-4,
							"height" : RENDER_HEIGHT-23,
							"index" : 10,
						},
						{
							"name":"separator",
							"type":"image",
							"x":0,"y":170,
							"horizontal_align":"center",
							"image":"d:/ymir work/ui/bar.png",
							"children":
							(
								{
									"name":"header_icon",
									"type":"image",
									"x":1,"y": -2,
									"horizontal_align":"center",
									"image":"d:/ymir work/ui/new_weekly_rank/bonus_icon2.png",
								},
								{
									"name":"location_text",
									"type":"text",
									"x":0,"y":25,
									"all_align" : "center",
									"text": "|cFFf1e6c0Konum: |r-",
									"outline":"1",
								}, 
							),
						},
					),
				},
				{
					"name":"bonus_window",
					"type":"border_a",
					"x":MAIN_POS_X,"y":BONUS_Y,
					"width":SUB_WIDTH,"height":BONUS_HEIGHT,
					"children":
					(
						{
							"name":"Header",
							"type":"image",
							"x":2,"y":2,
							"image":REAL_PATH+"header_biggest.png",
							"children":
							(
								{
									"name":"bonusy_text",
									"type":"text",
									"x":0,"y":0,
									"all_align" : "center",
									"text": "Tamamlama Bonuslarý",
									"outline":"1",
								},  
							),
						},
						{
							"name":"bonusbox",
							"type":"image",
							"x":0,"y":29,
							"horizontal_align":"center",
							"image":REAL_PATH+"bonusbox.png",
							"children":
							(
								{
									"name":"bonustext_0",
									"type":"text",
									"x":0,"y":-19,
									"all_align" : "center",
									"text":"-",
									"outline":"1",
								}, 
								{
									"name":"bonustext_1",
									"type":"text",
									"x":0,"y":-2,
									"all_align" : "center",
									"text":"-",
									"outline":"1",
								}, 
								{
									"name":"bonustext_2",
									"type":"text",
									"x":0,"y":16,
									"all_align" : "center",
									"text":"-",
									"outline":"1",
								}, 
							),
						},
					),
				},
				{"name": "lowlevel_bg", "type" : "window", "x" : MAIN_POS_X, "y" : MAIN_POS_Y, "width": MAIN_WIDTH, "height" : RENDER_HEIGHT, "children": (
						{
							"name" : "blocked_bg", "type" : "image",
							"x": 0, "y": 0,
							"width" : MAIN_WIDTH, "height" : RENDER_HEIGHT,
							"image" : REAL_PATH+"blocked_bg.png"
						},
						{
							"name" : "lowlevel_text",
							"type" : "text",
							"x": 0, "y" : 0,
							"text": "Gereken Seviye: 0",
							"fontname":"Tahoma:20",
							"outline":1,
							"all_align":"center",
						},
					),
				}
			),
		},
	),
}
