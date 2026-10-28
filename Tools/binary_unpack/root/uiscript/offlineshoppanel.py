import localeInfo

ROOT = "d:/ymir work/ui/game/offlineshop/"

BOARD_WIDTH = 562
BOARD_HEIGHT = 400

window = {
	"name" : "OfflineShopBuilder", "style" : ("movable", "float",),
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
					"image" : ROOT + "window/window.tga",

					"children" :
					(
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
						{
							"name" : "RenderTarget", "type" : "render_target",
							"x" : 8, "y" : 50,
							"width" : 196, "height" : 8*32,
							"index" : 1,

							"children" :
							(
								{
									"name" : "TitleBackButton", "type" : "button",
									"x" : 5, "y" : 30,

									"default_image" : ROOT + "prev_norm.tga",
									"over_image" : ROOT + "prev_hover.tga",
									"down_image" : ROOT + "prev_down.tga",
								},
								{
									"name" : "TitleNextButton", "type" : "button",
									"x" : 175, "y" : 32,

									"default_image" : ROOT + "next_norm.tga",
									"over_image" : ROOT + "next_hover.tga",
									"down_image" : ROOT + "next_down.tga",
								},
								{
									"name" : "ShopBackButton", "type" : "button",
									"x" : 10, "y" : 150,

									"default_image" : ROOT + "prev_norm.tga",
									"over_image" : ROOT + "prev_hover.tga",
									"down_image" : ROOT + "prev_down.tga",
								},
								{
									"name" : "ShopNextButton", "type" : "button",
									"x" : 165, "y" : 150,

									"default_image" : ROOT + "next_norm.tga",
									"over_image" : ROOT + "next_hover.tga",
									"down_image" : ROOT + "next_down.tga",
								},
								{
									"name" : "RefreshRender", "type" : "button",
									"x" : 155, "y": 210,

									"default_image" : ROOT + "refresh_button_norm.tga",
									"over_image" : ROOT + "refresh_button_hover.tga",
									"down_image" : ROOT + "refresh_button_down.tga",
								},
							),
						},
						{
							"name" : "TimeInput", "type" : "image",
							"x" : 310, "y" : 320,
							"image" : ROOT + "input.tga",

							"children" :
							(
								{
									"name" : "RefillButton", "type" : "button",
									"x" : 133, "y" : 0.5,

									"default_image" : ROOT + "refill_button_norm.tga",
									"over_image" : ROOT + "refill_button_hover.tga",
									"down_image" : ROOT + "refill_button_down.tga",
								},
								{
									"name" : "Time", "type" : "text",
									"x" : -10, "y" : 6,
									"horizontal_align" : "center",
									"text_horizontal_align" : "center",
									"text" : "0",
									"fontname" : "Tahoma:14",
								},
							),
						},
						{
							"name" : "ViewsInput", "type" : "image",
							"x" : 65, "y" : 320,
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
				{
					"name" : "LogsWindow", "type" : "image",
					"x" : 10, "y" : 33,
					"image" : ROOT + "window/window_logs.tga",

					"children" :
					(
						{
							"name" : "ListBox", "type" : "listboxex",
							"x" : 5, "y" : 48,
							"width" : 184*2-15, "height" : 38*40,
							"viewcount" : 10,
						},
						{
							"name" : "ScrollBar", "type" : "scrollbar3",
							"x" : 525, "y" : 45,
							"size" : 307,
						},
					),
				},
				{
					"name" : "NameInput", "type" : "image",
					"x" : 20, "y" : 42,
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
						{
							"name" : "RefreshTitle", "type" : "button",
							"x" : 187, "y": 6.5,

							"default_image" : ROOT + "refresh_button_norm.tga",
							"over_image" : ROOT + "refresh_button_hover.tga",
							"down_image" : ROOT + "refresh_button_down.tga",
						},
					),
				},
				{
					"name" : "MoneyInput", "type" : "button",
					"x" : 240, "y" : 42,

					"default_image" : ROOT + "input_money_norm.tga",
					"over_image" : ROOT + "input_money_hover.tga",
					"down_image" : ROOT + "input_money_down.tga",

					"children" :
					(
						{
							"name":"Money_Icon", "type":"image",
							"x" : 3, "y" : 3,
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
					"x" : 455, "y": 42,
					"width" : 61, "height" : 21,

					"default_image" : ROOT + "close_button_norm.tga",
					"over_image" : ROOT + "close_button_hover.tga",
					"down_image" : ROOT + "close_button_down.tga",

					"tooltip_text" : localeInfo.OFFLINESHOP_BUTTON_CLOSE,
				},
				{
					"name" : "SecondButton", "type" : "button",
					"x" : 500, "y": 42,
					"width" : 61, "height" : 21,

					"default_image" : ROOT + "history_button_norm.tga",
					"over_image" : ROOT + "history_button_hover.tga",
					"down_image" : ROOT + "history_button_down.tga",

					"tooltip_text" : localeInfo.OFFLINESHOP_BUTTON_HISTORY,
				},
			),
		},
	),
}