import uiScriptLocale

PUBLIC_PATH = "d:/ymir work/ui/public/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"

WINDOW_WIDTH	= 350
WINDOW_HEIGHT	= 400

BOARD_WIDTH		= WINDOW_WIDTH
BOARD_HEIGHT	= WINDOW_HEIGHT

LIST_WINDOW_WIDTH	= BOARD_WIDTH - 21
LIST_WINDOW_HEIGHT	= BOARD_HEIGHT - 70

LIST_WINDOW_PATTERN_X_COUNT = (LIST_WINDOW_WIDTH - 32) / 16
LIST_WINDOW_PATTERN_Y_COUNT = (LIST_WINDOW_HEIGHT - 32) / 16

window = {
	"name"		: "PrivateShopSearchItemVnumWindow",
	"style"		: ("movable", "float",),

	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,

	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	[
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			"x"			: 0,
			"y"			: 0,
			"width"		: BOARD_WIDTH,
			"height"	: BOARD_HEIGHT,
			
			"title" : uiScriptLocale.PRIVATE_SHOP_SEARCH_BY_VNUM_BOARD_TITLE,
		
			"children" :
			[	
				{
					"name"		: "item_list_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 10,
					"y"			: 32,
					
					"width"		: LIST_WINDOW_WIDTH,
					"height"	: LIST_WINDOW_HEIGHT,
					
					"children"	:
					[
						## LeftTop 1
						{
							"name" : "ListWindowLeftTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						
						## RightTop 2
						{
							"name" : "ListWindowRightTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : LIST_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						
						## LeftBottom 3
						{
							"name" : "ListWindowLeftBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : LIST_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						
						## RightBottom 4
						{
							"name" : "ListWindowRightBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : LIST_WINDOW_WIDTH - 16,
							"y" : LIST_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						
						## topcenterImg 5
						{
							"name" : "ListWindowTopCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, LIST_WINDOW_PATTERN_X_COUNT, 0),
						},
						
						## leftcenterImg 6
						{
							"name" : "ListWindowLeftCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, LIST_WINDOW_PATTERN_Y_COUNT),
						},
						## rightcenterImg 7
						{
							"name" : "ListWindowRightCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : LIST_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, LIST_WINDOW_PATTERN_Y_COUNT),
						},
						## bottomcenterImg 8
						{
							"name" : "ListWindowBottomCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : LIST_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, LIST_WINDOW_PATTERN_X_COUNT, 0),
						},
						## centerImg
						{
							"name" : "ListWindowCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, LIST_WINDOW_PATTERN_X_COUNT, LIST_WINDOW_PATTERN_Y_COUNT),
						},
					],
				},
				
				{
					"name"		: "vnum_list",
					"type"		: "listboxex",
					"x"			: 5 + 10,
					"y"			: 5 + 32,
					"width"		: 291,
					"height"	: LIST_WINDOW_HEIGHT - 10,
					},
						
				{
					"name" : "scroll_bar",
					"type" : "scrollbar",

					"x" : 28,
					"y" : 36,
					"size" : LIST_WINDOW_HEIGHT - 7,
					"horizontal_align" : "right",
				},
				
				{
					"name" : "button_cancel",
					"type" : "button",

					"x" : 152,
					"y" : 367,

					"text" : uiScriptLocale.PRIVATE_SHOP_SEARCH_BY_VNUM_CANCEL_BUTTON_TEXT,

					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
				},
				
				{
					"name" : "button_search",
					"type" : "button",

					"x" : 252,
					"y" : 367,

					"text" : uiScriptLocale.PRIVATE_SHOP_SEARCH_BY_VNUM_SEARCH_BUTTON_TEXT,

					"default_image"	: PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
				},
			],
		},
	],
}
	