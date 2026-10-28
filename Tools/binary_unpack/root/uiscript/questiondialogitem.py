if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

window = {
	"name" : "QuestionDialog",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH/2 - 125,
	"y" : SCREEN_HEIGHT/2 - 52,

	"width" : 340,
	"height" : 105,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 340,
			"height" : 105,

			"children" :
			(
				{
					"name" : "message",
					"type" : "text",

					"x" : 0,
					"y" : 38,

					"horizontal_align" : "center",
					"text" : uiScriptLocale.MESSAGE,

					"text_horizontal_align" : "center",
					"text_vertical_align" : "center",
				},
				{
					"name" : "accept",
					"type" : "button",

					"x" : -75,
					"y" : 63,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
					"disable_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
				},
				{
					"name" : "cancel",
					"type" : "button",

					"x" : 75,
					"y" : 63,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/public/cancelbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/cancelbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/cancelbutton02.sub",
					"disable_image" : "d:/ymir work/ui/public/cancelbutton02.sub",
				},
				{
					"name" : "sell",
					"type" : "button",

					"x" : 30,
					"y" : 63,

					"width" : 61,
					"height" : 21,

					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/public/sellbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/sellbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/sellbutton02.sub",
					"disable_image" : "d:/ymir work/ui/public/sellbutton02.sub",
				},
			),
		},
	),
}

if app.ENABLE_DESTROY_DIALOG:
	window['children'][0]['children'] += ( \
		{
			"name" : "destroy",
			"type" : "button",

			"x" : 0,
			"y" : 63,

			"width" : 61,
			"height" : 21,

			"horizontal_align" : "center",

			"default_image" : "d:/ymir work/ui/public/deletebutton00.sub",
			"over_image" : "d:/ymir work/ui/public/deletebutton01.sub",
			"down_image" : "d:/ymir work/ui/public/deletebutton02.sub",
			"disable_image" : "d:/ymir work/ui/public/deletebutton02.sub",
		},
	)