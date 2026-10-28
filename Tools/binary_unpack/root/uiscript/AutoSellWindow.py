import uiScriptLocale

LOCALE_PATH = uiScriptLocale.WINDOWS_PATH

WINDOW_WIDTH = 330
WINDOW_HEIGHT = 350

x_center = (SCREEN_WIDTH - WINDOW_WIDTH) // 2
y_center = (SCREEN_HEIGHT - WINDOW_HEIGHT) // 2

window = {
	"name": "AutoSellWindow",
	"x": x_center,
	"y": y_center,
	"width": WINDOW_WIDTH,
	"height": WINDOW_HEIGHT,
	"style": ("movable", "float"),
	"children": (
		{
			"name": "Board",
			"type": "board_with_titlebar",
			"x": 0,
			"y": 0,
			"width": WINDOW_WIDTH,
			"height": WINDOW_HEIGHT - 30,
			"title": "Otomatik Sat",
			"children": (
				{
					"name": "OtoSat",
					"type": "button",
					"x": 10,
					"y": WINDOW_HEIGHT - 59,
					"width": 120,
					"height": 25,
					"text": "Pasif",
					"default_image": "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image": "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image": "d:/ymir work/ui/public/middle_button_03.sub",
				},
				{
					"name": "RemoveItem",
					"type": "button",
					"x": 228,
					"y": WINDOW_HEIGHT - 59,
					"width": 140,
					"height": 25,
					"text": "Seçili itemi Kaldýr",
					"default_image": "d:/ymir work/ui/public/large_button_01.sub",
					"over_image": "d:/ymir work/ui/public/large_button_02.sub",
					"down_image": "d:/ymir work/ui/public/large_button_03.sub",
				},
				{
					"name": "RemoveAllItems",
					"type": "button",
					"x": 142,
					"y": WINDOW_HEIGHT - 59,
					"width": 90,
					"height": 25,
					"text": "Tümünü Temizle",
					"default_image": "d:/ymir work/ui/public/large_button_01.sub",
					"over_image": "d:/ymir work/ui/public/large_button_02.sub",
					"down_image": "d:/ymir work/ui/public/large_button_03.sub",
				},
				{
					"name": "thinboard_0",
					"type": "thinboard",
					"x": 7,
					"y": 33,
					"width": 316,
					"height": 254,
				},
			),
		},
	),
}