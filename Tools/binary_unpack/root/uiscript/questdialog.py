import app

ROOT = "d:/ymir work/ui/public/"

if app.ENABLE_QUEST_WIDTH_EXPANSION:
	QUEST_BOARD_WIDTH_ADJUSTMENT_VALUE = 2

window = {
	"name" : "QuestDialog",
	"style" : ("float",),#"movable", 

	"x" : 0,
	"y" : 0,

	"width" : 800,
	"height" : 450,

	"children" :
	(
		{
			"name" : "board",
			"type" : "thinboard",
			"style" : ("attach", "ignore_size",),

			"x" : 0,
			"y" : 0,

			"horizontal_align" : "center",
			"vertical_align" : "center",

			"width" : 350,
			"height" : 300,
		},
	),
}

if app.ENABLE_QUEST_WIDTH_EXPANSION:
	window["children"][0]["width"] = window["children"][0]["width"] * QUEST_BOARD_WIDTH_ADJUSTMENT_VALUE
