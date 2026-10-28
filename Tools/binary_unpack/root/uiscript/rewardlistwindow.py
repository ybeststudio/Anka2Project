import uiScriptLocale
import localeInfo
import app

WINDOW_WIDTH					= 192
WINDOW_HEIGHT					= 200

TITLE_BAR_X						= 7
TITLE_BAR_Y						= 7
TITLE_BAR_WIDTH					= 178

if localeInfo.IsARABIC():
	REWARD_LIST_POS_X			= 24
else:
	REWARD_LIST_POS_X			= 8
REWARD_LIST_POS_Y				= 31

SCROLL_BAR_POS_X				= 22
SCROLL_BAR_POS_Y				= 31
SCROLL_BAR_SIZE					= 160

REWARD_SLOT_WIDTH				= 32
REWARD_SLOT_HEIGHT				= 32
REWARD_LIST_COUNT_X				= 5
REWARD_LIST_COUNT_Y				= 5


window = {
	"name" : "RewardListWindow",
	"style" : ("float", "limit"),

	"x" : 0,
	"y" : 0,
	
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children" :
	[
		## background image
		{ 
			"name" : "event_reward_list_background",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,

			"children" :
			[
				## title
				{
					"name" : "title_bar",
					"type" : "titlebar",

					"x" : TITLE_BAR_X,
					"y" : TITLE_BAR_Y,

					"width" : TITLE_BAR_WIDTH,

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":87, "y":4, "text":uiScriptLocale.EVENT_REWARD_LIST_TITLE, "text_horizontal_align":"center" },
					),
				},
				
				# reward list
				{
					"name" : "reward_item_slot",
					"type" : "slot",
					
					"x" : REWARD_LIST_POS_X,
					"y" : REWARD_LIST_POS_Y,
					
					"width" : REWARD_SLOT_WIDTH * REWARD_LIST_COUNT_X,
					"height" : REWARD_SLOT_HEIGHT * REWARD_LIST_COUNT_Y,
					
					"image" : "d:/ymir work/ui/public/Slot_Base.sub",
					
					"slot" : 
					(
						{"index":0, "x":REWARD_SLOT_WIDTH * 0, "y":REWARD_SLOT_HEIGHT * 0, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":1, "x":REWARD_SLOT_WIDTH * 1, "y":REWARD_SLOT_HEIGHT * 0, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":2, "x":REWARD_SLOT_WIDTH * 2, "y":REWARD_SLOT_HEIGHT * 0, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":3, "x":REWARD_SLOT_WIDTH * 3, "y":REWARD_SLOT_HEIGHT * 0, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":4, "x":REWARD_SLOT_WIDTH * 4, "y":REWARD_SLOT_HEIGHT * 0, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},

						{"index":5, "x":REWARD_SLOT_WIDTH * 0, "y":REWARD_SLOT_HEIGHT * 1, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":6, "x":REWARD_SLOT_WIDTH * 1, "y":REWARD_SLOT_HEIGHT * 1, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":7, "x":REWARD_SLOT_WIDTH * 2, "y":REWARD_SLOT_HEIGHT * 1, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":8, "x":REWARD_SLOT_WIDTH * 3, "y":REWARD_SLOT_HEIGHT * 1, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":9, "x":REWARD_SLOT_WIDTH * 4, "y":REWARD_SLOT_HEIGHT * 1, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},

						{"index":10, "x":REWARD_SLOT_WIDTH * 0, "y":REWARD_SLOT_HEIGHT * 2, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":11, "x":REWARD_SLOT_WIDTH * 1, "y":REWARD_SLOT_HEIGHT * 2, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":12, "x":REWARD_SLOT_WIDTH * 2, "y":REWARD_SLOT_HEIGHT * 2, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":13, "x":REWARD_SLOT_WIDTH * 3, "y":REWARD_SLOT_HEIGHT * 2, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":14, "x":REWARD_SLOT_WIDTH * 4, "y":REWARD_SLOT_HEIGHT * 2, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},

						{"index":15, "x":REWARD_SLOT_WIDTH * 0, "y":REWARD_SLOT_HEIGHT * 3, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":16, "x":REWARD_SLOT_WIDTH * 1, "y":REWARD_SLOT_HEIGHT * 3, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":17, "x":REWARD_SLOT_WIDTH * 2, "y":REWARD_SLOT_HEIGHT * 3, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":18, "x":REWARD_SLOT_WIDTH * 3, "y":REWARD_SLOT_HEIGHT * 3, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":19, "x":REWARD_SLOT_WIDTH * 4, "y":REWARD_SLOT_HEIGHT * 3, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},

						{"index":20, "x":REWARD_SLOT_WIDTH * 0, "y":REWARD_SLOT_HEIGHT * 4, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":21, "x":REWARD_SLOT_WIDTH * 1, "y":REWARD_SLOT_HEIGHT * 4, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":22, "x":REWARD_SLOT_WIDTH * 2, "y":REWARD_SLOT_HEIGHT * 4, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":23, "x":REWARD_SLOT_WIDTH * 3, "y":REWARD_SLOT_HEIGHT * 4, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
						{"index":24, "x":REWARD_SLOT_WIDTH * 4, "y":REWARD_SLOT_HEIGHT * 4, "width":REWARD_SLOT_WIDTH, "height":REWARD_SLOT_HEIGHT},
					),
				},

				## scroll bar
				{
					"name" : "reward_list_scroll_bar",
					"type" : "small_thin_scrollbar",

					"x" : SCROLL_BAR_POS_X,
					"y" : SCROLL_BAR_POS_Y,

					"size" : SCROLL_BAR_SIZE,
					"horizontal_align" : "right",
				},
			]
		},
	],
}