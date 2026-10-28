if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale, localeInfo

BOARD_WIDTH = 150+45*3 + 100
BOARD_HEIGHT = 200+32*2+40
SLOT_POS_X = 10+3
SLOT_POS_Y = 15
SLOT_BAR_POS_X = 76
SLOT_BAR_POS_Y = 105
PUBLIC_PATCH = "d:/ymir work/ui/public/biyolog/"

# Arka plan: statik resim (render target uibiyolog.py icinde kod ile eklenir)
BLACK_BOARD = {
	"name" : "BlackBoard",
	"type" : "image",
	"x" : 0, "y" : 0, "image" : "bestwork/biyolog.png",
}

window = {
	"name" : "BiyologWindow",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"children" :
			(
				BLACK_BOARD,
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6,
					"y" : 6,

					"width" : BOARD_WIDTH-13,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":BOARD_WIDTH/2, "y":3, "text": "Biyolog Sistemi", "text_horizontal_align":"center" },
					),
				},
				## Item Slots 01

				{
					"name" : "itemSlotImage01", "type" : "image",
					"x" : 15+SLOT_POS_X, "y" : 190,
					"image" : PUBLIC_PATCH+"slot_base_01.tga",
					"children" :
					(
						{
							"name" : "Image01", "type" : "image",
							"x" : 44, "y" : 15,
							"image" : PUBLIC_PATCH+"right_arrow.tga",
						},
					),
				},
				{
					"name" : "itemSlotImage02", "type" : "image",
					"x" : 15 + 59+SLOT_POS_X, "y" : 190,
					"image" : PUBLIC_PATCH+"slot_base_01.tga",
					"children" :
					(
						{
							"name" : "Image02", "type" : "image",
							"x" : 44, "y" : 15,
							"image" : PUBLIC_PATCH+"right_arrow.tga",
						},
					),
				},
				{
					"name" : "itemSlotImage03", "type" : "image",
					"x" : 15 + (59*2)+SLOT_POS_X, "y" : 190,
					"image" : PUBLIC_PATCH+"slot_base_01.tga",
				},
				{
					"name" : "itemSlot01", "type" : "grid_table", "x" : 20+SLOT_POS_X, "y" : 195, "start_index" : 0,
					"x_count" : 3, "y_count" : 1, "x_step" : 32, "y_step" : 32, "x_blank" : 4+13+10, "y_blank" : 4, "horizontal_align" : "center",
				},

				## Slot Bar
				{
					"name" : "slotBar01", "type" : "window", "x" : SLOT_BAR_POS_X - 38, "y" : SLOT_BAR_POS_Y+(16+5)*0 + 55,
					"width" : 55 , "height": 16,
					"children" :
					(
						{ "name" : "GivenCount", "type" : "text", "text_horizontal_align":"center", "x" : 55/2, "y" : 3, "text" : "5", },
					),
				},
				{ "name" : "text011", "type" : "text", "fontname" : "Tahoma:30", "x" : SLOT_BAR_POS_X+61 - 38, "y" : SLOT_BAR_POS_Y-9 + 55, "text" : "-", },
				{
					"name" : "slotBar01", "type" : "window", "x" : SLOT_BAR_POS_X+75 - 38, "y" : SLOT_BAR_POS_Y+(16+5)*0 + 55,
					"width" : 55, "height": 16,
					"children" :
					(
						{ "name" : "NeedCount", "type" : "text", "text_horizontal_align":"center", "x" : 55/2, "y" : 3, "text" : "10", },
					),
				},

				## Bonuses Tab
				{
					"name" : "tab01",
					"type" : "window",
					"x" : 100,
					"y" : 35,
					"window" : 100,
					"height" : 100,
					"children" :
					(
						{ "name" : "text", "type" : "text", "text_horizontal_align":"center", "x" : 60, "y" : 5/2, "text" : "", "color":0xFFFEE3AE },
					),
				},

				## Item Slots 02
				{
					"name" : "itemSlotImage04", "type" : "image",
					"x" : BOARD_WIDTH-72, "y" : BOARD_HEIGHT-134+46,
					# "image" : PUBLIC_PATCH+"new_base.tga",
				},
				{
					"name" : "itemSlotImage05", "type" : "image",
					"x" : 25, "y" : BOARD_HEIGHT-86,
					 # "image" : PUBLIC_PATCH+"new_base.tga",
				},
				{
					"name" : "itemSlot02", "type" : "grid_table", "x" : BOARD_WIDTH-72, "y" : BOARD_HEIGHT-134, "start_index" : 0,
					"x_count" : 1, "y_count" : 2, "x_step" : 32, "y_step" : 32, "x_blank" : 4, "y_blank" : 4+10, "horizontal_align" : "center",
				},
				## Buttons
				{
					"name" : "GiveButton", "type" : "button",
					"x" : 52, "y" : BOARD_HEIGHT-93, "text" : uiScriptLocale.BIYOLOG_BUTTON_01,
					"default_image" : "d:/ymir work/ui/Public/xlarge_button_01.sub",
					"over_image" 	: "d:/ymir work/ui/Public/xlarge_button_02.sub",
					"down_image" 	: "d:/ymir work/ui/Public/xlarge_button_03.sub",
				},
				{ "name" : "yazi1", "type" : "text", "fontname" : "Tahoma:12", "x" : 20, "y" : 310, "text" : localeInfo.BIYOLOG_TEXT_01, },
				{ "name" : "yazi2", "type" : "text", "fontname" : "Tahoma:12", "x" : 55, "y" : 323, "text" : localeInfo.BIYOLOG_TEXT_02, },
				{
					"name" : "slotBar02", "type" : "window", "x" : 52, "y" : BOARD_HEIGHT-33 + 91,
					"width" : 130, "height": 16,
					"children" :
					(
						{ "name" : "LeftTime", "type" : "text", "text_horizontal_align":"center", "x" : 110/2, "y" : 1, "text" : uiScriptLocale.BIYOLOG_STATE, },
					),
				},
				{
					"name" : "GiftButton", "type" : "button",
					"x" : 52, "y" : BOARD_HEIGHT-33, "text" : uiScriptLocale.BIYOLOG_BUTTON_03,
					"default_image" : "d:/ymir work/ui/Public/xlarge_button_01.sub",
					"over_image" 	: "d:/ymir work/ui/Public/xlarge_button_02.sub",
					"down_image" 	: "d:/ymir work/ui/Public/xlarge_button_03.sub",
				},
			),
		},
	),
}