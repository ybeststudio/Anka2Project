if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

ROOT = "d:/ymir work/ui/game/"

BOARD_ADD_X = 10
BOARD_ADD_X += 140
BOARD_X = SCREEN_WIDTH - (150 + BOARD_ADD_X)
BOARD_WIDTH = (150 + BOARD_ADD_X)
BOARD_HEIGHT = 40
window = {
	"name" : "ExpandedMoneyTaskbar",
	"x" : BOARD_X,
	"y" : SCREEN_HEIGHT - 65,
	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,
	"style" : ("float",),
	"children" :
	(
		{
			"name" : "ExpanedMoneyTaskBar_Board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,

			"children" :
			(
				{
					"name" : "Money_Icon",
					"type" : "image",

					"x":23 + BOARD_ADD_X,
					"y":11,

					"image" : "d:/ymir work/ui/game/windows/money_icon.sub",
				},
				{
					"name":"Money_Slot",
					"type":"slotbar",
					"x":40 + BOARD_ADD_X,
					"y":10,
					"width" : 64+35,
					"height" : 17,
					"children" :
					(
						{
							"name" : "Money",
							"type" : "text",
							"x" : 3,
							"y" : 3,
							"horizontal_align" : "right",
							"text_horizontal_align" : "right",
							"text" : "19,999,999,999",
						},
					),
				},
				{
					"name" : "Gem_Icon",
					"type" : "image",

					"x":BOARD_ADD_X - 47,
					"y":13,

					"image" : "d:/ymir work/gaya.tga",
				},
				{
					"name":"Gem_Slot",
					"type":"slotbar",
					"x": BOARD_ADD_X - 32,
					"y":10,
					"width" : 50,
					"height" : 17,
					"children" :
					(
						{
							"name" : "Gem",
							"type" : "text",
							"x" : 3,
							"y" : 3,
							"horizontal_align" : "right",
							"text_horizontal_align" : "right",
							"text" : "0",
						},
					),
				},
				{
					"name":"Coins_Icon",
					"type" : "image",

					"x":BOARD_ADD_X - 142,
					"y":13,

					"image":"d:/ymir work/ui/game/taskbar/coins.tga",
				},
				{
					"name":"Coins_Slot",
					"type":"slotbar",
					"x": BOARD_ADD_X - 125,
					"y":10,
					"width" : 70,
					"height" : 17,
					"children" :
					(
						{
							"name" : "Coins",
							"type" : "text",
							"x" : 3,
							"y" : 3,
							"horizontal_align" : "right",
							"text_horizontal_align" : "right",
							"text" : "9,999",
						},
					),
				},
			),
		},
	),
}