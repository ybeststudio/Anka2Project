if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

window = {
	"name" : "ChangeLookWindow",

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
						{ "name":"TitleName", "type":"text", "x":95, "y":3, "text":"", "text_horizontal_align":"center" },
					),
				},

				## Slot
				{
					"name" : "ChangeLook_SlotImg",
					"type" : "image",

					"x" : 9,
					"y" : 35,

					"image" : "d:/ymir work/ui/changelook/ingame_convert.tga",

					"children" :
					(
						{
							"name" : "ChangeLookSlot",
							"type" : "slot",

							"x" : 3,
							"y" : 3,

							"width" : 190,
							"height" : 200,

							"slot" : (
								{"index":0, "x":26, "y":20, "width":31, "height":96},
								{"index":1, "x":125, "y":20, "width":31, "height":96},
							),
						},
						{
							"name" : "ChangeLookSlot_PassYangItem",
							"type" : "slot",

							"x" : 77.5,
							"y" : 128,

							"width" : 31,
							"height" : 31,

							"slot" : (
								{"index":0, "x":1, "y":1, "width":31, "height":31},
							),
						},
					),
				},
				## ºñ¿ë
				{
					"name" : "Cost",
					"type" : "text",
					"text" : "",
					"text_horizontal_align" : "center",
					"x" : 105,
					"y" : 215,
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
				## ChangeLookToolTip Button
				{
					"name" : "ChangeLookToolTIpButton",
					"type" : "button",

					"x" : 155,
					"y" : 8,

					"default_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
					"over_image" : "d:/ymir work/ui/pattern/q_mark_02.tga",
					"down_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
				},
			),
		},
	),
}