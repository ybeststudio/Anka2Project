import uiScriptLocale
import localeInfo
import app

WIDTH	= 760
HEIGHT	= 570

if app.ENABLE_POPUP_NOTICE_IMPROVE:
	window = {
		"name" : "PopupNoticeWindow",

		"x" : SCREEN_WIDTH / 2 - WIDTH/2,

		"y" : SCREEN_HEIGHT / 2 - HEIGHT/2,

		"style" : ("movable", "float",),

		"width"  : WIDTH,
		"height" : HEIGHT,

		"children" :
		(
			{
				"name" : "board",
				"type" : "board",
				"style" : ("attach",),

				"x" : 0,
				"y" : 0,

				"width"	 : WIDTH,
				"height" : HEIGHT,

				"children" :
				(
					## Title
					{
						"name" : "TitleBar",
						"type" : "titlebar",
						"style" : ("attach",),

						"x" : 8,
						"y" : 7,

						"width" : WIDTH - 15,
						"color" : "yellow",

						"children" :
						(
							{ "name":"TitleName", "type":"text", "x":50, "y":3, "text":uiScriptLocale.POPUP_NOTICE_BUTTON_TEXT, "text_horizontal_align":"center" },
						),
					},
					## web
					{
						"name" : "webBox",
						"type" : "box",
						"style" : ("attach",),

						"x" : 7,
						"y" : 30,
						"color" : 0xFFFFFFFF,
						"width" : WIDTH - 14,
						"height" : HEIGHT - 40,
					
					},
				),
			},

		),
	}
else:
	window = {
		"name" : "PopupNoticeWindow",

		"x" : SCREEN_WIDTH / 2 - WIDTH/2,

		"y" : SCREEN_HEIGHT / 2 - HEIGHT/2,

		"style" : ("movable", "float",),

		"width"  : WIDTH,
		"height" : HEIGHT,

		"children" :
		(
			{
				"name" : "board",
				"type" : "board",
				"style" : ("attach",),

				"x" : 0,
				"y" : 0,

				"width"	 : WIDTH,
				"height" : HEIGHT,

				"children" :
				(
					## Title
					{
						"name" : "TitleBar",
						"type" : "titlebar",
						"style" : ("attach",),

						"x" : 8,
						"y" : 7,

						"width" : WIDTH - 15,
						"color" : "yellow",

						"children" :
						(
							{ "name":"TitleName", "type":"text", "x":50, "y":3, "text":uiScriptLocale.POPUP_NOTICE_BUTTON_TEXT, "text_horizontal_align":"center" },
						),
					},
					## web
					{
						"name" : "webBox",
						"type" : "box",
						"style" : ("attach",),

						"x" : 5,
						"y" : 30,
						"color" : 0xFFFFFFFF,
						"width" : WIDTH - 12,
						"height" : HEIGHT - 60,
					
					},

					# checkbox_bg
					{
						"name"	: "checkbox_bg",
						"type"	: "image",
						"image"	: "d:/ymir work/ui/public/popup_notice_checkbox_bg.sub",
						"x"		: 7,
						"y"		: HEIGHT - 28,
						"width"	: 24,
						"height": 18,
					},
					#checkbox image
					{
						"name" : "checkbox",
						"type" : "image",
						"style" : ("not_pick",),
						"image" : "d:/ymir work/ui/public/popup_notice_checkbox.sub",
						"x"		: 7,
						"y"		: HEIGHT - 28,
						"width" : 24,
						"height" : 18,

					},
					#checkbox_text
					{
						"name" : "checkbox_text",
						"type" : "text",

						"text":localeInfo.POPUP_NOTICE_CHECKBOX_TEXT,
						"x"		: 37,
						"y"		: HEIGHT - 24,
					},
				),
			},

		),
	}
