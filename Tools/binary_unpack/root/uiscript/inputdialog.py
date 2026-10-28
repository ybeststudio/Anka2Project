import uiScriptLocale

window = {
	"name" : "InputDialog",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : 170,
	"height" : 90,

	"children" :
	(
		{
			"name" : "Board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : 170,
			"height" : 90,

			"title" : "",

			"children" :
			(

				## Input Slot
				{
					"name" : "InputSlot",
					"type" : "slotbar",

					"x" : 0,
					"y" : 34,
					"width" : 90,
					"height" : 18,
					"horizontal_align" : "center",

					#"type" : "image",
					#"image" : "d:/ymir work/ui/public/Parameter_Slot_03.sub",

					"children" :
					(
						{
							"name" : "InputValue",
							"type" : "editline",

							"x" : 3,
							"y" : 3,

							"width" : 90,
							"height" : 18,

							"input_limit" : 12,
						},
					),
				},

				## Button
				{
					"name" : "AcceptButton",
					"type" : "button",

					"x" : - 61 - 5 + 30,
					"y" : 58,
					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
					"disable_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
				},
				{
					"name" : "CancelButton",
					"type" : "button",

					"x" : 5 + 30,
					"y" : 58,
					"horizontal_align" : "center",

					"default_image" : "d:/ymir work/ui/public/cancelbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/cancelbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/cancelbutton02.sub",
					"disable_image" : "d:/ymir work/ui/public/cancelbutton02.sub",
				},
			),
		},
	),
}