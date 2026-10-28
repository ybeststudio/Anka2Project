import uiScriptLocale

window = {
	"name" : "ChestDropWindow", "style" : ("movable", "float",),
	"x" : 0, "y" : 0,
	"width" : 208, "height" : 455,

	"children" :
	(
		{
			"name" : "Board", "type" : "board", "style" : ("attach",),
			"x" : 0, "y" : 0,
			"width" : 208, "height" : 455,

			"children" :
			(
				{
					"name" : "TitleBar", "type" : "titlebar", "style" : ("attach",),
					"x" : 6, "y" : 7, "width" : 195,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x": 100, "y":3, "text": uiScriptLocale.CHEST_VIEW_DROP_TITLE, "text_horizontal_align":"center" },
					),
				},
				{
					"name" : "BoardPattern", "type" : "window", "style" : ("attach", "ltr",),
					"x" : 10, "y" : 32,
					"width" : 186, "height" :364,

					"children" :
					(
						{
							"name" : "pattern_left_top_img", "type" : "image", "style" : ("ltr",),
							"x" : 0, "y" : 0,
							"image" : "d:/ymir work/ui/pattern/border_a/border_A_left_top.tga",
						},
						{
							"name" : "pattern_right_top_img", "type" : "image", "style" : ("ltr",),
							"x" : 186 - 16, "y" : 0,
							"image" : "d:/ymir work/ui/pattern/border_a/border_A_right_top.tga",
						},
						{
							"name" : "pattern_left_bottom_img", "type" : "image", "style" : ("ltr",),
							"x" : 0, "y" : 364 - 16,
							"image" : "d:/ymir work/ui/pattern/border_a/border_A_left_bottom.tga",
						},
						{
							"name" : "pattern_right_bottom_img", "type" : "image", "style" : ("ltr",),
							"x" : 186 - 16, "y" : 364 - 16,
							"image" : "d:/ymir work/ui/pattern/border_a/border_A_right_bottom.tga",
						},
						{
							"name" : "pattern_top_cetner_img", "type" : "expanded_image", "style" : ("ltr",),
							"x" : 16, "y" : 0,
							"image" : "d:/ymir work/ui/pattern/border_a/border_A_top.tga",
							"rect" : (0.0, 0.0, (186 - 32) / 16, 0),
						},
						{
							"name" : "pattern_left_center_img", "type" : "expanded_image", "style" : ("ltr",),
							"x" : 0, "y" : 16,
							"image" : "d:/ymir work/ui/pattern/border_a/border_A_left.tga",
							"rect" : (0.0, 0.0, 0, (364 - 32) / 16),
						},
						{
							"name" : "pattern_right_center_img", "type" : "expanded_image", "style" : ("ltr",),
							"x" : 186 - 16, "y" : 16,
							"image" : "d:/ymir work/ui/pattern/border_a/border_A_right.tga",
							"rect" : (0.0, 0.0, 0, (364 - 32) / 16),
						},
						{
							"name" : "pattern_bottom_center_img", "type" : "expanded_image", "style" : ("ltr",),
							"x" : 16, "y" : 364 - 16,
							"image" : "d:/ymir work/ui/pattern/border_a/border_A_bottom.tga",
							"rect" : (0.0, 0.0, (186 - 32) / 16, 0),
						},
						{
							"name" : "pattern_center_img", "type" : "expanded_image", "style" : ("ltr",),
							"x" : 16, "y" : 16,
							"image" : "d:/ymir work/ui/pattern/border_a/border_A_center.tga",
							"rect" : (0.0, 0.0, (186 - 32) / 16, (364 - 32) / 16),
						},
					),
				},
				{
					"name" : "ItemSlot", "type" : "expanded_image", "style" : ("attach",),
					"x" : 81, "y" : 37,
					"image" : "d:/ymir work/ui/game/windows/slot_2.tga",

					"children" :
					(
						{
							"name" : "OpenItemSlot", "type" : "slot",
							"x" : 4, "y" : 4,
							"width" : 32, "height" : 32,

							"slot" :
							(
								{"index":0, "x":0, "y":0, "width":32, "height":32,},
							),
						},
					),
				},
				{
					"name" : "prev_button", "type" : "button",
					"x" : 54, "y" : 374,

					"default_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
					"over_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_02.sub",
					"down_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
				},
				{
					"name" : "CurrentPageBack", "type" : "thinboard_circle",
					"x" : 78, "y" : 371,
					"width" : 30, "height" : 20,

					"children" :
					(
						{
							"name" : "CurrentPage", "type" : "text",
							"x" : 0, "y" : 0,

							"vertical_align" : "center", "horizontal_align" : "center",
							"text_vertical_align" : "center", "text_horizontal_align" : "center",
							"text" : "1",
						},
					),
				},
				{
					"name" : "next_button", "type" : "button",
					"x" : 112, "y" : 374,

					"default_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
					"over_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_02.sub",
					"down_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
				},
				{
					"name" : "OpenCountController", "type" : "sliderbar",
					"x" : 16, "y" : 400,
				},
				{
					"name" : "OpenChestButton", "type" : "button",
					"x" : 43, "y" : 420,

					"default_image" : "d:/ymir work/ui/public/flat_button_default.sub",
					"over_image" : "d:/ymir work/ui/public/flat_button_over.sub",
					"down_image" : "d:/ymir work/ui/public/flat_button_down.sub",
				},
			),
		},
	),
}
