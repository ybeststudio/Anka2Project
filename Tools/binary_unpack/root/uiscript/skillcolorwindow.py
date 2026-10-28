import uiScriptLocale

WINDOW_WIDTH = 260
WINDOW_HEIGHT = 278

COLOR_PICKER_WIDTH = 256
COLOR_PICKER_HEIGHT = 256

window = {
	"name" : "skill_colorWindow", "style" : ("movable", "float",),
	"x" : (SCREEN_WIDTH - WINDOW_WIDTH) / 2, "y" : (SCREEN_HEIGHT - WINDOW_HEIGHT) / 2,
	"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,

	"children" :
	(
		{
			"name" : "Board", "type" : "board_with_titlebar", "style" : ("attach",),
			"x" : 0, "y" : 0,
			"width" : WINDOW_WIDTH, "height" : WINDOW_HEIGHT,
			"title" : uiScriptLocale.SKILL_COLOR_TITLE,

			"children" :
			(
				{
					"name" : "MainFrame", "type" : "image",
					"x" : 0, "y" : 12,
					"image" : "d:/ymir work/ui/game/skill_color/frame.tga",
					"horizontal_align" : "center", "vertical_align" : "center",
				},
				{
					"name" : "ColorMapSlot", "type" : "image",
					"x" : 17, "y" : 38,
					"image" : "d:/ymir work/ui/game/skill_color/frame_color_picker.tga",

					"children" :
					(
						{
							"name" : "ColorMap", "type" : "image",
							"x" : 0, "y" : 0,
							"image" : "d:/ymir work/ui/game/skill_color/color_picker.tga",
							"horizontal_align" : "center", "vertical_align" : "center",
						},
						{
							"name" : "BGColorPickerButton", "type" : "button",
							"x" : 0, "y" : 0,
							"width" : COLOR_PICKER_WIDTH, "height" : COLOR_PICKER_HEIGHT,
						},
						{
							"name" : "BGColorPickerDotImage", "type" : "image",
							"x" : 0, "y" : 0,
							"width" : 15, "height" : 15,
							"image" : "d:/ymir work/ui/game/skill_color/color_picker_dot.tga",
						},
					),
				},
				{
					"name" : "LayerBtn0", "type" : "radio_button",
					"x" : 213, "y" : 40,

					"default_image" : "d:/ymir work/ui/game/skill_color/button_layer_norm.tga",
					"over_image" : "d:/ymir work/ui/game/skill_color/button_layer_over.tga",
					"down_image" : "d:/ymir work/ui/game/skill_color/button_layer_down.tga",

					"text" : "I",
				},
				{
					"name" : "LayerBtn1", "type" : "radio_button",
					"x" : 213, "y" : 40 + 30,

					"default_image" : "d:/ymir work/ui/game/skill_color/button_layer_norm.tga",
					"over_image" : "d:/ymir work/ui/game/skill_color/button_layer_over.tga",
					"down_image" : "d:/ymir work/ui/game/skill_color/button_layer_down.tga",

					"text" : "II",
				},
				{
					"name" : "LayerBtn2", "type" : "radio_button",
					"x" : 213, "y" : 40 + 30*2,

					"default_image" : "d:/ymir work/ui/game/skill_color/button_layer_norm.tga",
					"over_image" : "d:/ymir work/ui/game/skill_color/button_layer_over.tga",
					"down_image" : "d:/ymir work/ui/game/skill_color/button_layer_down.tga",

					"text" : "III",
				},
				{
					"name" : "LayerBtn3", "type" : "radio_button",
					"x" : 213, "y" : 40 + 30*3,

					"default_image" : "d:/ymir work/ui/game/skill_color/button_layer_norm.tga",
					"over_image" : "d:/ymir work/ui/game/skill_color/button_layer_over.tga",
					"down_image" : "d:/ymir work/ui/game/skill_color/button_layer_down.tga",

					"text" : "IV",
				},
				{
					"name" : "LayerBtn4", "type" : "radio_button",
					"x" : 213, "y" : 40 + 30*4,

					"default_image" : "d:/ymir work/ui/game/skill_color/button_layer_norm.tga",
					"over_image" : "d:/ymir work/ui/game/skill_color/button_layer_over.tga",
					"down_image" : "d:/ymir work/ui/game/skill_color/button_layer_down.tga",

					"text" : "V",
				},
				{
					"name" : "ColorPreviewSlot", "type" : "image",
					"x" : 213, "y" : 40 + 30*5,
					"image" : "d:/ymir work/ui/game/skill_color/frame_color.tga",

					"children" :
					(
						{
							"name" : "ColorPreview", "type" : "bar",
							"x" : 2, "y" : 2,
							"width" : 28, "height" : 25,
							"color" : 0xFFFFFFFF,
						},
					),
				},
				{
					"name" : "DefaultButton", "type" : "button",
					"x" : 97, "y" : 239,
					"text" : uiScriptLocale.SKILL_COLOR_BUTTON_DEFAUL,

					"default_image" : "d:/ymir work/ui/game/skill_color/button_option_norm.tga",
					"over_image" : "d:/ymir work/ui/game/skill_color/button_option_over.tga",
					"down_image" : "d:/ymir work/ui/game/skill_color/button_option_down.tga",
				},
				{
					"name" : "ConfirmButton", "type" : "button",
					"x" : 172, "y" : 239,
					"text" : uiScriptLocale.SKILL_COLOR_BUTTON_SAVE,

					"default_image" : "d:/ymir work/ui/game/skill_color/button_option_norm.tga",
					"over_image" : "d:/ymir work/ui/game/skill_color/button_option_over.tga",
					"down_image" : "d:/ymir work/ui/game/skill_color/button_option_down.tga",
				},
				{
					"name" : "ColorInputSlot", "type" : "image",
					"x" : 16, "y" : 244,
					"image" : "d:/ymir work/ui/game/skill_color/input.tga",

					"children" :
					(
						{
							"name" : "ColorInputValue", "type" : "editline",
							"x" : 4, "y" : 2,
							"width" : 70, "height" : 18,
							"text" : "", "input_limit" : 7,
						},
					),
				},
			)
		},
	),
}
