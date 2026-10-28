import localeInfo

ROOT = "d:/ymir work/ui/game/offlineshop/"

BOARD_WIDTH = 562
BOARD_HEIGHT = 357

window = {
	"name" : "OfflineShopBuilder", "style" : ("float","movable",),
	"x" : 0, "y" : 0,
	"width" : BOARD_WIDTH, "height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "Board", "type" : "board_with_titlebar", "style" : ("attach",),
			"x" : 0, "y" : 0,
			"width" : BOARD_WIDTH, "height" : BOARD_HEIGHT,

			"title" : localeInfo.OFFLINESHOP_BOARD_TITLE,

			"children" :
			(
				{
					"name" : "MainWindow", "type" : "image",
					"x" : 10, "y" : 33,
					"image" : ROOT + "window/window_build.tga",

					"children" :
					(
					
						{
							"name" : "NameInput", "type" : "image",
							"x" : 10, "y" : 9,
							"image" : ROOT + "input_name.tga",

							"children" :
							(
								{
									"name" : "NameLine", "type" : "editline",
									"x" : 5, "y" : 6,
									"width" : 280, "height" : 20,
									"input_limit" : 35,
									"text" : "NULL",
								},
							),
						},
						{
							"name" : "Money_Slot", "type" : "image",
							"x" : 230, "y" : 9,
							"image" : ROOT + "input_money_norm.tga",

							"children" :
							(
								{
									"name":"Money_Icon", "type":"image",
									"x":3, "y":3,
									"image":"d:/ymir work/ui/game/windows/money_icon.tga",
								},
								{
									"name" : "Money", "type" : "text",
									"x" : 3, "y" : 5,
									"horizontal_align" : "right",
									"text_horizontal_align" : "right",
									"text" : "0",
									"fontname" : "Tahoma:14",
								},
							),
						},
						{
							"name" : "FirstButton", "type" : "button",
							"x" : 445, "y": 9,
							"width" : 61, "height" : 21,

							"default_image" : ROOT + "open_button_norm.tga",
							"over_image" : ROOT + "open_button_hover.tga",
							"down_image" : ROOT + "open_button_down.tga",
						},
						{
							"name" : "SecondButton", "type" : "button",
							"x" : 490, "y": 9,
							"width" : 61, "height" : 21,

							"default_image" : ROOT + "close_button_norm.tga",
							"over_image" : ROOT + "close_button_hover.tga",
							"down_image" : ROOT + "close_button_down.tga",
						},
						{
							"name" : "RenderTarget", "type" : "render_target",
							"x" : 8, "y" : 50,
							"width" : 196, "height" : 8*32,
							"index" : 1,

							"children" :
							(
								{
									"name" : "title_back_btn", "type" : "button",
									"x" : 5, "y" : 30,

									"default_image" : ROOT + "prev_norm.tga",
									"over_image" : ROOT + "prev_hover.tga",
									"down_image" : ROOT + "prev_down.tga",
								},
								{
									"name" : "title_next_btn", "type" : "button",
									"x" : 175, "y" : 32,

									"default_image" : ROOT + "next_norm.tga",
									"over_image" : ROOT + "next_hover.tga",
									"down_image" : ROOT + "next_down.tga",
								},
								{
									"name" : "shop_back_btn", "type" : "button",
									"x" : 10, "y" : 150,

									"default_image" : ROOT + "prev_norm.tga",
									"over_image" : ROOT + "prev_hover.tga",
									"down_image" : ROOT + "prev_down.tga",
								},
								{
									"name" : "shop_next_btn", "type" : "button",
									"x" : 165, "y" : 150,

									"default_image" : ROOT + "next_norm.tga",
									"over_image" : ROOT + "next_hover.tga",
									"down_image" : ROOT + "next_down.tga",
								},
							),
						},
						{
							"name" : "ItemSlot", "type" : "grid_table",
							"x" : 215, "y" : 50,
							"start_index" : 0,
							"x_count" : 5, "y_count" : 8,
							"x_step" : 32, "y_step" : 32,
							"image" : "d:/ymir work/ui/public/Slot_Base.sub",
						},
						{
							"name" : "ItemSlot2", "type" : "grid_table",
							"x" : 215+(32*5), "y" : 50,
							"start_index" : 0,
							"x_count" : 5, "y_count" : 8,
							"x_step" : 32, "y_step" : 32,
							"image" : "d:/ymir work/ui/public/Slot_Base.sub",
						},
					),
				},
			),
		},
	),
}