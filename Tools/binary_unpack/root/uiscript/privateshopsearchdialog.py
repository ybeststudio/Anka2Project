import localeInfo

ROOT = "d:/ymir work/ui/game/privatesearch/"

window = {
	"name" : "PrivateShopSearchDialog", "style" : ("movable", "float",),
	"x" : 0, "y" : 0,
	"width" : 841, "height" : 492,

	"children" :
	(
		{
			"name" : "board", "type" : "board_with_titlebar",
			"x" : 0, "y" : 0,
			"width" : 841, "height" : 492,
			"title" : localeInfo.OFFLINESHOP_SEARCH_TITLE,

			"children":
			(
				{
					"name" : "board_bg", "type" : "image",
					"x" : 13, "y" : 34,
					"image" : ROOT + "window.tga",

					"children" :
					(
						{
							"name" : "category_menu_class", "type" : "image",
							"x" : 6, "y" : 8,
							"image" : ROOT + "category_input.tga",

							"children" :
							(
								{"name" : "classText", "type" : "text", "x" : -15, "y" : 5, "text" : localeInfo.OFFLINESHOP_SEARCH_CLASS, "horizontal_align" : "center",},
							),
						},
						{
							"name" : "category_menu_objects", "type" : "image",
							"x" : 6, "y" : 75,
							"image" : ROOT + "category_input.tga",

							"children" :
							(
								{"name" : "objectsText","type" : "text","x" : -15, "y" : 5, "text" : localeInfo.OFFLINESHOP_SEARCH_OBJECTS, "horizontal_align" : "center",},
							),
						},
						{
							"name" : "category_menu_minmax", "type" : "image",
							"x" : 6, "y" : 170,
							"image" : ROOT + "category_input.tga",

							"children" :
							(
								{"name" : "min-maxText", "type" : "text","x" : -15, "y" : 5, "text" : localeInfo.OFFLINESHOP_SEARCH_FILTER, "horizontal_align" : "center",},
							),
						},
						{
							"name" : "category_menu_price", "type" : "image",
							"x" : 5, "y" : 272,
							"image" : ROOT + "category_input.tga",

							"children" :
							(
								{"name" : "leftpriceText", "type" : "text","x" : -15, "y" : 5, "text" : localeInfo.OFFLINESHOP_SEARCH_PRICE, "horizontal_align" : "center",},
							),
						},
						{
							"name" : "category_menu_input", "type" : "image",
							"x" : 5, "y" : 370,
							"image" : ROOT + "category_input.tga",

							"children" :
							(
								{"name" : "inputItemText", "type" : "text","x" : -25, "y" : 5, "text" : localeInfo.OFFLINESHOP_SEARCH_ITEM_NAME, "horizontal_align" : "center",},
							),
						},
						{"name" : "first~", "type" : "text", "x" : 97, "y" : 206, "text" : "~",},
						{"name" : "second~", "type" : "text", "x" : 97, "y" : 241, "text" : "~",},
						{
							"name" : "tab_menu", "type" : "image",
							"x" : 206, "y" : 8,
							"image" : ROOT + "tab_menu.tga",

							"children" :
							(
								{"name" : "itemNameText", "type" : "text", "x" : 73, "y" : 5, "text" : localeInfo.OFFLINESHOP_SEARCH_ITEM_NAME,},
								{"name" : "sellerText", "type" : "text", "x" : 230, "y" : 5, "text" : localeInfo.OFFLINESHOP_SEARCH_ITEM_SELLER,},
								{"name" : "countText", "type" : "text", "x" : 320, "y" : 5, "text" : localeInfo.OFFLINESHOP_SEARCH_ITEM_COUNT,},
								{"name" : "rightpriceText", "type" : "text", "x" : 450, "y" : 5, "text" : localeInfo.OFFLINESHOP_SEARCH_PRICE,},
							),
						},
						{
							"name" : "editLineWnd", "type" : "window", "style" : ("not_pick",),
							"x" : 4, "y" : 199,
							"width" : 196, "height" : 220,

							"children":
							(
								{
									"name" : "minLevelInput", "type" : "image",
									"x" : 1, "y" : 2,
									"image" : ROOT + "filter_input.tga",

									"children" :
									(
										{
											"name" : "minLevel", "type" : "editline",
											"x" : 90, "y" : 7,
											"width" : 194, "height" : 26,

											"input_limit" : 3, "only_number" : 1,
											"horizontal_align" : "center",
											"text":"", "info_msg": "1",
										},
									),
								},
								{
									"name" : "maxLevelInput", "type" : "image",
									"x" : 105, "y" : 2,
									"image" : ROOT + "filter_input.tga",

									"children" :
									(
										{
											"name" : "maxLevel", "type" : "editline",
											"x" : 90, "y" : 7,
											"width" : 194, "height" : 26,

											"input_limit" : 3, "only_number" : 1,
											"horizontal_align" : "center",
											"text":"", "info_msg": "120",
										},
									),
								},
								{
									"name" : "minRefineInput", "type" : "image",
									"x" : 1, "y" : 35,
									"image" : ROOT + "filter_input.tga",

									"children" :
									(
										{
											"name" : "minRefine", "type" : "editline",
											"x" : 90, "y" : 7,
											"width" : 194, "height" : 26,

											"input_limit" : 3, "only_number" : 1,
											"horizontal_align" : "center",
											"text":"", "info_msg": "+0",
										},
									),
								},
								{
									"name" : "maxRefineInput", "type" : "image",
									"x" : 105, "y" : 35,
									"image" : ROOT + "filter_input.tga",

									"children" :
									(
										{
											"name" : "maxRefine", "type" : "editline",
											"x" : 90, "y" : 7,
											"width" : 194, "height" : 26,

											"input_limit" : 3, "only_number" : 1,
											"horizontal_align" : "center",
											"text":"", "info_msg": "+9",
										},
									),
								},
								{
									"name" : "minYangInput", "type" : "image",
									"x" : 1, "y" : 105,
									"image" : ROOT + "category_input.tga",

									"children" :
									(
										{
											"name" : "minYang", "type" : "editline",
											"x" : 10, "y" : 7,
											"width" : 194, "height" : 26,

											"input_limit" : 16, "only_number" : 1,
											"horizontal_align" : "center",
											"text":"", "info_msg": "0 Yang",
										},
									),
								},
								{
									"name" : "maxYangInput", "type" : "image",
									"x" : 1, "y" : 135,
									"image" : ROOT + "category_input.tga",

									"children" :
									(
										{
											"name" : "maxYang", "type" : "editline",
											"x" : 10, "y" : 7,
											"width" : 194, "height" : 26,

											"input_limit" : 16, "only_number" : 1,
											"horizontal_align" : "center",
											"text":"", "info_msg": "999.999.999.999 Yang",
										},
									),
								},
								{
									"name" : "searchItemNameInput", "type" : "image",
									"x" : 1, "y" : 202,
									"image" : ROOT + "category_input.tga",

									"children" :
									(
										{
											"name" : "searchItemName", "type" : "editline",
											"x" : 10, "y" : 6,
											"width" : 194, "height" : 26,

											"input_limit" : 35,
											"horizontal_align" : "center",
											"text":"", "info_msg": localeInfo.OFFLINESHOP_SEARCH_ITEM_NAME,

											"children" :
											(
												{
													"name" : "searchClearBtn",
													"type" : "button",
													"x" : 194-35,
													"y" : 1,
													"default_image" : ROOT + "clear_button_1.tga",
													"over_image" : ROOT + "clear_button_2.tga",
													"down_image" : ROOT + "clear_button_1.tga",
												},
											),
										},
									),
								},
							),
						},
						{
							"name" : "ListBox", "type" : "listbox3", "style" : ("attach",),
							"x" : 207, "y" : 29,
							"width" : 600, "height" : 416,
							"viewcount" : 10,

							"children":
							(
								{"name" : "listboxText", "type" : "text", "x" : 0, "y" : 0, "text" : "", "all_align" : 1,},
							),
						},
						{"name" : "searchBtn", "type" : "button", "x" : 205, "y" : 412, "text" : localeInfo.OFFLINESHOP_SEARCH_BUTTON, "default_image" : ROOT + "category_input.tga", "over_image" : ROOT + "category_input.tga", "down_image" : ROOT + "category_input.tga",},
						{"name" : "buyBtn", "type" : "button", "x" : 622, "y" : 412, "text" : localeInfo.OFFLINESHOP_SEARCH_BUTTON_BUY, "default_image" : ROOT + "category_input.tga", "over_image" : ROOT + "category_input.tga", "down_image" : ROOT + "category_input.tga",},

						{"name" : "first_prev_button", "type" : "button", "x" : 400, "y" : 422, "default_image" : ROOT + "first_page_0.tga", "over_image" : ROOT + "first_page_1.tga", "down_image" : ROOT + "first_page_1.tga",},
						{"name" : "prev_button", "type" : "button", "x" : 416, "y" : 422, "default_image" : ROOT + "back_page_0.tga", "over_image" : ROOT + "back_page_1.tga", "down_image" : ROOT + "back_page_2.tga",},
						{"name" : "next_button", "type" : "button", "x" : 584, "y" : 422, "default_image" : ROOT + "next_page_0.tga", "over_image" : ROOT + "next_page_1.tga", "down_image" : ROOT + "next_page_1.tga",},
						{"name" : "last_next_button", "type" : "button", "x" : 596, "y" : 422, "default_image" : ROOT + "last_page_0.tga", "over_image" : ROOT + "last_page_1.tga", "down_image" : ROOT + "last_page_1.tga",},

						{"name" : "page1_button", "type" : "radio_button", "x" : 433 + (29 * 0), "y" : 412, "text" : "1", "default_image" : ROOT + "page_box_0.tga", "over_image" : ROOT + "page_box_1.tga", "down_image" : ROOT + "page_box_2.tga",},
						{"name" : "page2_button", "type" : "radio_button", "x" : 433 + (29 * 1), "y" : 412, "text" : "2", "default_image" : ROOT + "page_box_0.tga", "over_image" : ROOT + "page_box_1.tga", "down_image" : ROOT + "page_box_2.tga",},
						{"name" : "page3_button", "type" : "radio_button", "x" : 433 + (29 * 2), "y" : 412, "text" : "3", "default_image" : ROOT + "page_box_0.tga", "over_image" : ROOT + "page_box_1.tga", "down_image" : ROOT + "page_box_2.tga",},
						{"name" : "page4_button", "type" : "radio_button", "x" : 433 + (29 * 3), "y" : 412, "text" : "4", "default_image" : ROOT + "page_box_0.tga", "over_image" : ROOT + "page_box_1.tga", "down_image" : ROOT + "page_box_2.tga",},
						{"name" : "page5_button", "type" : "radio_button", "x" : 433 + (29 * 4), "y" : 412, "text" : "5", "default_image" : ROOT + "page_box_0.tga", "over_image" : ROOT + "page_box_1.tga", "down_image" : ROOT + "page_box_2.tga",},
					),
				},
			),
		},
	),
}