import localeInfo

ROOT = "d:/ymir work/ui/game/offlineshop/"

BOARD_WIDTH = 562
BOARD_HEIGHT = 360

window = {
	"name" : "OfflineShopDialog", "style" : ("float","movable",),
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
					"image" : ROOT + "window/window_dialog.tga",

					"children" :
					(
						{
							"name" : "RenderTarget", "type" : "render_target",
							"x" : 8, "y" : 9,
							"width" : 196, "height" : 8*32,
							"index" : 1,
						},
						{
							"name" : "ItemSlot", "type" : "grid_table",
							"x" : 215, "y" : 9,
							"start_index" : 0,
							"x_count" : 5, "y_count" : 8,
							"x_step" : 32, "y_step" : 32,
							"image" : "d:/ymir work/ui/public/Slot_Base.sub",
						},
						{
							"name" : "ItemSlot2", "type" : "grid_table",
							"x" : 215+(32*5), "y" : 9,
							"start_index" : 0,
							"x_count" : 5, "y_count" : 8,
							"x_step" : 32, "y_step" : 32,
							"image" : "d:/ymir work/ui/public/Slot_Base.sub",
						},
						{
							"name" : "ViewsInput", "type" : "image",
							"x" : 185, "y" : 278,
							"image" : ROOT + "input.tga",

							"children" :
							(
								{
									"name":"ViewsIcon", "type":"image",
									"x" : 0, "y" : 0.5,
									"image": ROOT + "views_icon.tga",
								},
								{
									"name" : "Views", "type" : "text",
									"x" : 10, "y" : 5,
									"horizontal_align" : "center",
									"text_horizontal_align" : "center",
									"text" : "0",
									"fontname" : "Tahoma:14",
								},
							),
						},
					),
				},
			),
		},
	),
}