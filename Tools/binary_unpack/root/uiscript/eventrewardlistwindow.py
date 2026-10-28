IN_GAME_UI_WIDTH = 290
REWARD_UI_HEIGHT = 40

SLOT_WIDTH = 32
SLOT_HEIGHT = 32

window = {
	"name" : "EventRewardListWindow",
	"type" : "window",

	"x" : 0,
	"y" : 120,

	"width" : IN_GAME_UI_WIDTH,
	"height" : REWARD_UI_HEIGHT,

	"children" :
	[
		{
			"name" : "back_board_img",
			"type" : "expanded_image",
			"style" : ("attach",),
			"x" : 15,
			"y" : 0, 
			"image" : "d:/ymir work/ui/event/bg_list_tab.sub" 
		},
		{
			"name" : "EventButton",
			"type" : "button",
			"x" : 19,
			"y" : 3, 
			"text" : " ",
			"default_image" : "d:/ymir work/ui/event/long_button_01.sub",
			"over_image" : "d:/ymir work/ui/event/long_button_02.sub",
			"down_image" : "d:/ymir work/ui/event/long_button_03.sub",
		},
		{
			"name" : "reward_item_slot",
			"type" : "slot",

			"x" : 202,
			"y" : 7,

			"width" : SLOT_WIDTH*3,
			"height" : SLOT_HEIGHT,

			"image" : "d:/ymir work/ui/public/Slot_Base.sub",

			"slot" :
			(
				{"index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
				{"index":1, "x":SLOT_WIDTH * 1, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
				{"index":2, "x":SLOT_WIDTH * 2, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
			),
		},
		{
			"name" : "event_duration_window", 
			"type" : "window",

			"x" : 19,
			"y" : 25,

			"width" : 174,
			"height" : 16,

			"style" : ("attach",),

			"children" :
			(
				{"name":"EventDuration", "type":"text", "x":0, "y":0, "text": " ", "r" : 0.58, "g" : 0.56, "b" : 0.51, "a" : 1.0},
			),
		},
	],
}