import uiScriptLocale
BOARDH = 134
BOARDW = 55

window = {
	"name" : "StoneEventWindow",
	#"style" : ("float","movable"),
	"x" : 2,
	"y" : SCREEN_HEIGHT - 160,
	"width" : BOARDH,
	"height" : BOARDW,
	"children" :
	(
		{
			"name" : "Thinboard",
			"type" : "thinboard",
			"style" : ("attach",),
			"x" : 0,
			"y" : 0,
			"width" : BOARDH,
			"height" : BOARDW,
			"children" :
			(
				## Title Desing
				{ "name" : "StoneEventTitle", "type" : "text", "x" : 15, "y" : 15, "color" : 0xfff8d090, "text" : uiScriptLocale.METIN_EVENT_TITLE},
				{ "name" : "InformationBtn", "type" : "button", "x" : 50+50+13, "y" : 10, "default_image" : "d:/ymir work/ui/pattern/q_mark_01.tga", "over_image" : "d:/ymir work/ui/pattern/q_mark_02.tga", "down_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",},	
				{ "name" : "Point", "type" : "text", "x" : 15, "y" : 30 },
			),
		},
	),
}
