import uiScriptLocale

window = {
	"name" : "SpecialInventoryWindow",

	"x" : SCREEN_WIDTH - 176 - 287 - 10,
	"y" : SCREEN_HEIGHT - 37 - 595,

	"style" : ("movable", "float", "not_pick",),

	"width" : 176+18+65,
	"height" : 375,

	"children" :
	(
		{
			"name" : "board2",
			"type" : "thinboard",
			"style" : ("attach",),

			"x" : 170,
			"y" : 40,

			"width" : 86,
			"height" : 305,

			"children" :
			(
				{
					"name" : "SkillBookButton",
					"type" : "radio_button",

					"x" : 38,
					"y" : 15,

					"default_image" : "d:/ymir work/ui/game/special_inventory/book_down.tga",
					"over_image" : "d:/ymir work/ui/game/special_inventory/book_hover.tga",
					"down_image" : "d:/ymir work/ui/game/special_inventory/book_normal.tga",
				},
				{
					"name" : "UpgradeItemsButton",
					"type" : "radio_button",

					"x" : 38,
					"y" : 55,

					"default_image" : "d:/ymir work/ui/game/special_inventory/upp_down.tga",
					"over_image" : "d:/ymir work/ui/game/special_inventory/upp_hover.tga",
					"down_image" : "d:/ymir work/ui/game/special_inventory/upp_normal.tga",
				},
				{
					"name" : "StoneButton",
					"type" : "radio_button",

					"x" : 38,
					"y" : 95,

					"default_image" : "d:/ymir work/ui/game/special_inventory/stone_down.tga",
					"over_image" : "d:/ymir work/ui/game/special_inventory/stone_hover.tga",
					"down_image" : "d:/ymir work/ui/game/special_inventory/stone_normal.tga",
				},
				{
					"name" : "ChangersButton",
					"type" : "radio_button",

					"x" : 38,
					"y" : 135,

					"default_image" : "d:/ymir work/ui/game/special_inventory/switcher_down.tga",
					"over_image" : "d:/ymir work/ui/game/special_inventory/switcher_hover.tga",
					"down_image" : "d:/ymir work/ui/game/special_inventory/switcher_normal.tga",
				},

				{
					"name" : "GiftBoxButton",
					"type" : "radio_button",

					"x" : 38,
					"y" : 175,

					"default_image" : "d:/ymir work/ui/game/special_inventory/box_down.tga",
					"over_image" : "d:/ymir work/ui/game/special_inventory/box_hover.tga",
					"down_image" : "d:/ymir work/ui/game/special_inventory/box_normal.tga",
				},
				{
					"name" : "SafeboxButton",
					"type" : "radio_button",

					"x" : 38,
					"y" : 215,

					"default_image" : "d:/ymir work/ui/game/special_inventory/safebox_down.tga",
					"over_image" : "d:/ymir work/ui/game/special_inventory/safebox_hover.tga",
					"down_image" : "d:/ymir work/ui/game/special_inventory/safebox_normal.tga",
				},
				{
					"name" : "MallButton",
					"type" : "radio_button",

					"x" : 38,
					"y" : 255,

					"default_image" : "d:/ymir work/ui/game/special_inventory/skin_down.tga",
					"over_image" : "d:/ymir work/ui/game/special_inventory/skin_hover.tga",
					"down_image" : "d:/ymir work/ui/game/special_inventory/skin_normal.tga",
				},
			),
		},
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 176+18+7,
			"height" : 375,

			"children" :
			(
				{
					"name" : "effecthinboard",
					"type" : "thinboard",

					"x" : 12,
					"y" : 33,

					"width" : 191-14,
					"height" : 330,
				},
				{
					"name" : "SortInventoryButton",
					"type" : "button",

					"x":1,
					"y":7.5,

					"tooltip_text" : uiScriptLocale.SORT_INVENTORY,
					"horizontal_align":"left",

					"default_image" : "d:/ymir work/ui/game/inventory/refresh_normal.png",
					"over_image" : "d:/ymir work/ui/game/inventory/refresh_over.png",
					"down_image" : "d:/ymir work/ui/game/inventory/refresh_down.png",
					"disable_image" : "d:/ymir work/ui/game/inventory/refresh_down.png",
				},

				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 30,
					"y" : 7,

					"width" : 164,

					"color" : "yellow",

					"children" :
					(
						{
							"name" : "TitleName",
							"type" : "text",

							"x" : -5,
							"y" : 3,

							"text" : uiScriptLocale.SPECIAL_INVENTORY_TITLE,
							"horizontal_align" : "center",
							"text_horizontal_align" : "center"
						},
					),
				},

				## Item Slot
				{
					"name" : "ItemSlot",
					"type" : "grid_table",

					"x" : 20,
					"y" : 43,

					"start_index" : 0,
					"x_count" : 5,
					"y_count" : 9,
					"x_step" : 32,
					"y_step" : 32,

					"image" : "d:/ymir work/ui/public/Slot_Base.sub"
				},

				{
					"name" : "ScrollBar",
					"type" : "scrollbar2",

					"x" : 19.5,
					"y" : 31,
					"size" : 320,
					"horizontal_align" : "right",
				},

				{
					"name" : "ScrollButton",
					"type" : "expanded_image",

					"x" : 95,
					"y" : 39,

					"width" : 32,
					"height" : 128,

					"vertical_align" : "bottom",
					"image" : "d:/ymir work/ui/game/special_inventory/scroll_down.png",
				},

				#Loading stuff
				{
					"name" : "loading_image",
					"type" : "ani_image",
					"style" : ("ltr",),

					"widht" : 16,
					"height" : 16,

					"x" : -25,
					"y" : 0,

					"horizontal_align" : "center",
					"vertical_align" : "center",

					"delay" : 2,

					"images" :
					[
						"d:/ymir work/ui/game/special_inventory/loading_gif/0.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/1.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/2.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/3.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/4.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/5.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/6.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/7.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/8.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/9.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/10.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/11.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/12.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/13.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/14.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/15.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/16.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/17.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/18.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/19.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/20.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/21.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/22.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/23.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/24.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/25.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/26.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/27.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/28.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/29.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/30.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/31.tga",
						"d:/ymir work/ui/game/special_inventory/loading_gif/32.tga",
					],

					"children" :
					[
						{
							"name" : "loading_text",
							"type" : "text",

							"x" : 25,
							"y" : -16,

							"text_horizontal_align" : "center",

							"text" : uiScriptLocale.SPECIAL_INVENTORY_PAGE_PLEASE_WAIT,
						},
					],
				},
			),
		},
	),
}