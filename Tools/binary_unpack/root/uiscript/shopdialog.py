if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

BUTTON_Y = 295 if app.ENABLE_SHOP_REPURCHASE else 325
WINDOW_H = 356 if app.ENABLE_SHOP_REPURCHASE else 330

window = {
	"name" : "ShopDialog",

	"x" : SCREEN_WIDTH - 400,
	"y" : 10,

	"style" : ("movable", "float", "window_without_alpha"),

	"width" : 184,
	"height" : WINDOW_H,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 184,
			"height" : WINDOW_H,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 8,

					"width" : 169,
					"color" : "gray",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":84, "y":4, "text":uiScriptLocale.SHOP_TITLE, "text_horizontal_align":"center" },
					),
				},

				## Item Slot
				{
					"name" : "ItemSlot",
					"type" : "grid_table",

					"x" : 12,
					"y" : 34,

					"start_index" : 0,
					"x_count" : 5,
					"y_count" : 8,
					"x_step" : 32,
					"y_step" : 32,

					"image" : "d:/ymir work/ui/public/Slot_Base.sub",
				},

				## Buy
				{
					"name" : "BuyButton",
					"type" : "toggle_button",

					"x" : 21,
					"y" : BUTTON_Y,

					"width" : 61,
					"height" : 21,

					"text" : uiScriptLocale.SHOP_BUY,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},

				## Sell
				{
					"name" : "SellButton",
					"type" : "toggle_button",

					"x" : 104,
					"y" : BUTTON_Y,

					"width" : 61,
					"height" : 21,

					"text" : uiScriptLocale.SHOP_SELL,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},

				## Close
				{
					"name" : "CloseButton",
					"type" : "button",

					"x" : 0,
					"y" : BUTTON_Y,

					"horizontal_align" : "center",

					"text" : uiScriptLocale.PRIVATE_SHOP_CLOSE_BUTTON,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},

				## MiddleTab1
				{
					"name" : "MiddleTab1",
					"type" : "radio_button",

					"x" : 21,
					"y" : BUTTON_Y,

					"width" : 61,
					"height" : 21,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},

				## MiddleTab2
				{
					"name" : "MiddleTab2",
					"type" : "radio_button",

					"x" : 104,
					"y" : BUTTON_Y,

					"width" : 61,
					"height" : 21,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},

				## SmallTab1
				{
					"name" : "SmallTab1",
					"type" : "radio_button",

					"x" : 21,
					"y" : BUTTON_Y,

					"width" : 43,
					"height" : 21,

					"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
				},

				## SmallTab2
				{
					"name" : "SmallTab2",
					"type" : "radio_button",

					"x" : 71,
					"y" : BUTTON_Y,

					"width" : 43,
					"height" : 21,

					"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
				},

				## SmallTab3
				{
					"name" : "SmallTab3",
					"type" : "radio_button",

					"x" : 120,
					"y" : BUTTON_Y,

					"width" : 43,
					"height" : 21,

					"default_image" : "d:/ymir work/ui/public/small_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/small_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/small_button_03.sub",
				},
			),
		},
	),
}

if app.ENABLE_SHOP_REPURCHASE:
	window["children"][0]["children"] = window["children"][0]["children"] + (
		{
			"name"					: "repurchase_button",
			"type"					: "button",

			"x"						: 48,
			"y"						: 322,

			"text"					: uiScriptLocale.SHOP_REPURCHASE_BUTTON,
			"text_horizontal_align"	: "center",

			"default_image"			: "d:/ymir work/ui/public/large_button_01.sub",
			"over_image"			: "d:/ymir work/ui/public/large_button_02.sub",
			"down_image"			: "d:/ymir work/ui/public/large_button_03.sub",
		},
	)