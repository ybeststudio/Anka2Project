import uiScriptLocale

BOARD_W = (650-60-75+230-7-12-12-10-8)-300-150+85+190  # Yatay geniþletildi (+200)
BOARD_H = 282-55+9+45+90+50

LEFTBOARD_X = 13
LEFTBOARD_Y = 36

LEFTBOARD_WIDTH = (650-60-75+230-7-12-12-10-8)-300-150+85-15-5+185  # Yatay geniþletildi (+200)
LEFTBOARD_HEIGHT = 282-55+9+45+90

EK_Y = 4

window ={
	"name" : "SpeedButtonWindow",
	"style" : ("movable", "float"),

	"x"		: 0,
	"y"		: 0,

	"width"	: BOARD_W,
	"height" : BOARD_H,

	"children" : 
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : BOARD_W,
			"height" : BOARD_H,

			"title" : uiScriptLocale.SPEED_BUTTON_TITLE,

			"children" : 
			(
				{
					"name" : "dataManagerButtonThinBoard",
					"type" : "thinboard",

					"x" : LEFTBOARD_X - 4,
					"y" : LEFTBOARD_Y - 4,

					"width" : LEFTBOARD_WIDTH + 8,
					"height" : LEFTBOARD_HEIGHT + 8,
				},
				{
					"name" : "dataManagerButtonBoard",
					"type" : "thinboard_circle",

					"x" : LEFTBOARD_X,
					"y" : LEFTBOARD_Y,

					"width" : LEFTBOARD_WIDTH,
					"height" : LEFTBOARD_HEIGHT,
				},

				{
					"name" : "backGround",
					"type" : "expanded_image",
					"style" : ("not_pick",),
					
					"x" : 10,
					"y" : 29+5,
					"width" : LEFTBOARD_WIDTH - 10,
					"height" : LEFTBOARD_HEIGHT - 10,
					
					"image" : "menu_2341/bg.png",
					
					"children" : 
					(
						# 1. Sütun (sol)
						{"name" : "Button1","type" : "button","x" : 16,"y" : 40+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button1Text","type" : "text","x" : 16+46,"y" : 40+12+EK_Y, "text" : uiScriptLocale.BUTTON_1,},

						{"name" : "Button3","type" : "button","x" : 16,"y" : 85+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button3Text","type" : "text","x" : 16+46,"y" : 85+12+EK_Y, "text" : uiScriptLocale.BUTTON_3,},

						{"name" : "Button5","type" : "button","x" : 16,"y" : 85+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button5Text","type" : "text","x" : 16+46,"y" : 85+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_5,},

						{"name" : "Button7","type" : "button","x" : 16,"y" : 85+45+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button7Text","type" : "text","x" : 16+46,"y" : 85+45+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_7,},

						{"name" : "Button9","type" : "button","x" : 16,"y" : 85+45+45+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button9Text","type" : "text","x" : 16+46,"y" : 85+45+45+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_9,},

						{"name" : "Button11","type" : "button","x" : 16,"y" : 85+45+45+45+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button11Text","type" : "text","x" : 16+46,"y" : 85+45+45+45+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_11,},

						# 2. Sütun (orta)
						{"name" : "Button2","type" : "button","x" : 177,"y" : 40+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button2Text","type" : "text","x" : 177+42+66,"y" : 40+12+EK_Y, "text" : uiScriptLocale.BUTTON_2,"text_horizontal_align" : "right",},

						{"name" : "Button4","type" : "button","x" : 177,"y" : 85+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button4Text","type" : "text","x" : 177+42+66,"y" : 85+12+EK_Y, "text" : uiScriptLocale.BUTTON_4,"text_horizontal_align" : "right",},

						{"name" : "Button6","type" : "button","x" : 177,"y" : 85+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button6Text","type" : "text","x" : 177+42+66,"y" : 85+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_6,"text_horizontal_align" : "right",},

						{"name" : "Button8","type" : "button","x" : 177,"y" : 85+45+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button8Text","type" : "text","x" : 177+42+66,"y" : 85+45+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_8,"text_horizontal_align" : "right",},

						{"name" : "Button10","type" : "button","x" : 177,"y" : 85+45+45+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button10Text","type" : "text","x" : 177+42+66,"y" : 85+45+45+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_10,"text_horizontal_align" : "right",},

						{"name" : "Button12","type" : "button","x" : 177,"y" : 85+45+45+45+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button12Text","type" : "text","x" : 177+42+66,"y" : 85+45+45+45+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_12,"text_horizontal_align" : "right",},

						# 3. Sütun (sað) - Button13-19 yukarýdan baþlýyor
						{"name" : "Button13","type" : "button","x" : 338,"y" : 40+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button13Text","type" : "text","x" : 338+46,"y" : 40+12+EK_Y, "text" : uiScriptLocale.BUTTON_13,},

						{"name" : "Button14","type" : "button","x" : 338,"y" : 85+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button14Text","type" : "text","x" : 338+46,"y" : 85+12+EK_Y, "text" : uiScriptLocale.BUTTON_14,},

						{"name" : "Button15","type" : "button","x" : 338,"y" : 85+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button15Text","type" : "text","x" : 338+46,"y" : 85+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_15,},

						{"name" : "Button16","type" : "button","x" : 338,"y" : 85+45+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button16Text","type" : "text","x" : 338+46,"y" : 85+45+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_16,},

						{"name" : "Button17","type" : "button","x" : 338,"y" : 85+45+45+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button17Text","type" : "text","x" : 338+46,"y" : 85+45+45+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_17,},

						{"name" : "Button18","type" : "button","x" : 338,"y" : 85+45+45+45+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button18Text","type" : "text","x" : 338+46,"y" : 85+45+45+45+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_18,},

						{"name" : "Button19","type" : "button","x" : 338,"y" : 85+45+45+45+45+45+EK_Y,"default_image" : "menu_2341/best1.png","over_image" : "menu_2341/best2.png","down_image" : "menu_2341/best3.png",},				
						{"name" : "Button19Text","type" : "text","x" : 338+46,"y" : 85+45+45+45+45+45+12+EK_Y, "text" : uiScriptLocale.BUTTON_19,},

					),
				},
			),
		},
	),
}