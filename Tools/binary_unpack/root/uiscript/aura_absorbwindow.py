if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale
import item

window = {
	"name" : "Aura_AbsorbWindow",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : 205,
	"height" : 270,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 205,
			"height" : 270,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6,
					"y" : 6,

					"width" : 190,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":95, "y":3, "text_horizontal_align":"center" },
					),
				},

				## Slot
				{
					"name" : "Aura_Absorb",
					"type" : "image",

					"x" : 9,
					"y" : 35,

					"image" : "d:/ymir work/ui/acce/acce_absorb.tga",

					"children" :
					(
						{
							"name" : "AuraSlot",
							"type" : "slot",

							"x" : 3,
							"y" : 3,

							"width" : 190,
							"height" : 200,

							"slot" : (
								{"index":0, "x":26, "y":41, "width":31, "height":31},
								{"index":1, "x":125, "y":8, "width":31, "height":96},
								{"index":2, "x":75, "y":126, "width":31, "height":31},
							),
						},
					),
				},
				## Button
				{
					"name" : "AcceptButton",
					"type" : "button",

					"x" : 40,
					"y" : 235,

					"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
					"disable_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
				},
				{
					"name" : "CancelButton",
					"type" : "button",

					"x" : 114,
					"y" : 235,

					"default_image" : "d:/ymir work/ui/public/cancelbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/cancelbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/cancelbutton02.sub",
					"disable_image" : "d:/ymir work/ui/public/cancelbutton02.sub",
				},
			),
		},
	),
}
