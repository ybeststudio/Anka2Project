if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

window = {
	"name" : "InventoryWindow",

	"x" : SCREEN_WIDTH - 212,
	"y" : SCREEN_HEIGHT - 37 - 573,

	"style" : ("movable", "float", "window_without_alpha"),

	"width" : 209,
	"height" : 544,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 37,
			"y" : 0,

			"width" : 176,
			"height" : 544,

			"children" :
			(
				{
					"name":"SortInventoryButton",
					"type":"button",

					"x":1,
					"y":7.5,

					"tooltip_text" : uiScriptLocale.SORT_INVENTORY,
					"horizontal_align":"left",

					"default_image" : "d:/ymir work/ui/game/inventory/refresh_normal.png",
					"over_image" : "d:/ymir work/ui/game/inventory/refresh_over.png",
					"down_image" : "d:/ymir work/ui/game/inventory/refresh_down.png",
				},
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 30,
					"y" : 7,

					"width" : 139,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":65, "y":3, "text":uiScriptLocale.INVENTORY_TITLE, "text_horizontal_align":"center" },
					),
				},
				{
					"name" : "Equipment_Base",
					"type" : "image",

					"x" : 10,
					"y" : 33,

					"image" : "d:/ymir work/ui/game/inventory/1.jpg",
				},
				{
					"name" : "ItemSlot",
					"type" : "grid_table",

					"x" : 8,
					"y" : 246,

					"start_index" : 0,
					"x_count" : 5,
					"y_count" : 9,
					"x_step" : 32,
					"y_step" : 32,

					"image" : "d:/ymir work/ui/public/Slot_Base.sub"
				},
			),
		},
	),
}

pageName = ["I", "II", "III", "IV", "V", "VI"]
for i in xrange(4):
	window["children"][0]["children"] += ( \
		{
			"name" : "Inventory_Tab_0%d" % (i+1),
			"type" : "radio_button",

			"x" : 10+3+39*i,
			"y" : 33 + 191,

			"default_image" : "d:/ymir work/ui/public/xsmall_button_01.sub",
			"over_image" : "d:/ymir work/ui/public/xsmall_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/xsmall_button_03.sub",

			"children" :
			(
				{
					"name" : "Inventory_Tab_0%d_Print" % (i+1),
					"type" : "text",

					"x" : 0,
					"y" : 0,

					"all_align" : "center",
					"text" : pageName[i],
				},
			),
		},
	)

for i in xrange(3):
	window["children"] += ( \
		{
			"name" : "Equipment_Tab_0%d" % (i+1),
			"type" : "radio_button",

			"x" : 21,
			"y" : 33+23*i,

			"default_image" : "d:/ymir work/ui/game/inventory/eqtabclosed.tga",
			"over_image" : "d:/ymir work/ui/game/inventory/eqtabclosed.tga",
			"down_image" : "d:/ymir work/ui/game/inventory/eqtabopen.tga",

			"children" :
			(
				{
					"name" : "Equipment_Tab_0%d_Print" % (i+1),
					"type" : "text",

					"x" : 1,
					"y" : -1,

					"all_align" : "center",
					"text" : pageName[i],
				},
			),
		},
	)

if app.ENABLE_HIDE_COSTUME_SYSTEM:
	window['children'][0]['children'] += ( \
		{
			"name" : "CostumeBodyButton",
			"type" : "toggle_button",

			"x" : 99,
			"y" : 105,

			"default_image" : "d:/ymir work/ui/game/hide_costume/button_show_0.tga",
			"over_image" : "d:/ymir work/ui/game/hide_costume/button_show_1.tga",
			"down_image" : "d:/ymir work/ui/game/hide_costume/button_hide_0.tga",
		},
		{
			"name" : "CostumeHairButton",
			"type" : "toggle_button",

			"x" : 97,
			"y" : 60,

			"default_image" : "d:/ymir work/ui/game/hide_costume/button_show_0.tga",
			"over_image" : "d:/ymir work/ui/game/hide_costume/button_show_1.tga",
			"down_image" : "d:/ymir work/ui/game/hide_costume/button_hide_0.tga",
		},
		{
			"name" : "CostumeAcceButton",
			"type" : "toggle_button",

			"x" : 147,
			"y" : 40,

			"default_image" : "d:/ymir work/ui/game/hide_costume/button_show_0.tga",
			"over_image" : "d:/ymir work/ui/game/hide_costume/button_show_1.tga",
			"down_image" : "d:/ymir work/ui/game/hide_costume/button_hide_0.tga",
		},
		{
			"name" : "CostumeWeaponButton",
			"type" : "toggle_button",

			"x" : 47,
			"y" : 43,

			"default_image" : "d:/ymir work/ui/game/hide_costume/button_show_0.tga",
			"over_image" : "d:/ymir work/ui/game/hide_costume/button_show_1.tga",
			"down_image" : "d:/ymir work/ui/game/hide_costume/button_hide_0.tga",
		},
		{
			"name" : "CostumeAuraButton",
			"type" : "toggle_button",

			"x" : 147,
			"y" : 165,

			"default_image" : "d:/ymir work/ui/game/hide_costume/button_show_0.tga",
			"over_image" : "d:/ymir work/ui/game/hide_costume/button_show_1.tga",
			"down_image" : "d:/ymir work/ui/game/hide_costume/button_hide_0.tga",
		},
	)
