import localeInfo
MAINBOARD_WIDTH = 352
MAINBOARD_HEIGHT = 458
MAINBOARD_X = (SCREEN_WIDTH/2) - (MAINBOARD_WIDTH/2)
MAINBOARD_Y = (SCREEN_HEIGHT/2) - (MAINBOARD_HEIGHT/2)

BOARD_PATH = "d:/ymir work/ui/minigame/attendance/number_board/"
DEFAULT_PATH = "d:/ymir work/ui/minigame/attendance/"

NUMBER_BUTTON_WIDTH = 72
NUMBER_BUTTON_HEIGHT = 60

NUMBER_BUTTON_GAP_X = 6
NUMBER_BUTTON_GAP_Y = 5

SLOT_WIDTH	= 32
SLOT_HEIGHT	= 32
SLOT_GAP	= 20

window = {
	"name" : "SpecialGachaWindow",
	"style" : ("movable", "float",),
	
	"x" : MAINBOARD_X,
	"y" : MAINBOARD_Y,	

	"width" : MAINBOARD_WIDTH,
	"height" : MAINBOARD_HEIGHT,
	
	"children" :
	(
		## MainBoard
		{
			"name" : "MainBoard",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : MAINBOARD_WIDTH,
			"height" : MAINBOARD_HEIGHT,
			
			"children" :
			(
				## Title Bar
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6, "y" : 7, "width" : MAINBOARD_WIDTH - 13,
					
					"children" :
					(
						{ "name" : "TitleName", "type" : "text", "x" : 0, "y" : 0, "text": localeInfo.SPECIAL_GACHA_TITLE, "all_align":"center" },
					),
				},
				
				{
					"name" : "number_board_window",
					"type" : "window",
					
					"x" : 13,
					"y" : 42,
					
					"width" : 325,
					"height" : 171,
					
					"children" :
					(
						## LeftTop 1
						{
							"name" : "LeftTop",
							"type" : "image",
							"x" : 0,
							"y" : 0,
							"image" : BOARD_PATH + "lefttop.sub",
						},
						## RightTop 2
						{
							"name" : "RightTop",
							"type" : "image",
							"x" : 309,
							"y" : 0,
							"image" : BOARD_PATH + "righttop.sub",
						},
						## LeftBottom 3
						{
							"name" : "LeftBottom",
							"type" : "image",
							"x" : 0,
							"y" : 155,
							"image" : BOARD_PATH + "leftbottom.sub",
						},
						## RightBottom 4
						{
							"name" : "RightBottom",
							"type" : "image",
							"x" : 309,
							"y" : 155,
							"image" : BOARD_PATH + "rightbottom.sub",
						},
						## topcenterImg 5
						{
							"name" : "TopCenterImg",
							"type" : "expanded_image",
							"x" : 16,
							"y" : 0,
							"image" : BOARD_PATH + "topcenterImg.tga",
							"rect" : (0.0, 0.0, 17, 0),
						},
						## leftcenterImg 6
						{
							"name" : "LeftCenterImg",
							"type" : "expanded_image",
							"x" : 0,
							"y" : 16,
							"image" : BOARD_PATH + "leftcenterImg.tga",
							"rect" : (0.0, 0.0, 0, 8),
						},
						## rightcenterImg 7
						{
							"name" : "RightCenterImg",
							"type" : "expanded_image",
							"x" : 308,
							"y" : 16,
							"image" : BOARD_PATH + "rightcenterImg.tga",
							"rect" : (0.0, 0.0, 0, 8),
						},
						## bottomcenterImg 8
						{
							"name" : "BottomCenterImg",
							"type" : "expanded_image",
							"x" : 16,
							"y" : 155,
							"image" : BOARD_PATH + "bottomcenterImg.tga",
							"rect" : (0.0, 0.0, 17, 0),
						},
						## centerImg
						{
							"name" : "CenterImg",
							"type" : "expanded_image",
							"x" : 16,
							"y" : 16,
							"image" : BOARD_PATH + "centerImg.tga",
							"rect" : (0.0, 0.0, 17, 8),
						},
						
						# number button 1
						{
							"name" : "disable_number_button1",
							"type" : "button",

							"x" : 10,
							"y" : 18,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_1.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_1.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_1.sub",
						},
						# number button 2
						{
							"name" : "disable_number_button2",
							"type" : "button",

							"x" : 10 + NUMBER_BUTTON_WIDTH + NUMBER_BUTTON_GAP_X,
							"y" : 18,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_2.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_2.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_2.sub",
						},
						# number button 3
						{
							"name" : "disable_number_button3",
							"type" : "button",

							"x" : 10 + NUMBER_BUTTON_WIDTH * 2 + NUMBER_BUTTON_GAP_X * 2,
							"y" : 18,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_3.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_3.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_3.sub",
						},
						# number button 4
						{
							"name" : "disable_number_button4",
							"type" : "button",

							"x" : 10 + NUMBER_BUTTON_WIDTH * 3 + NUMBER_BUTTON_GAP_X * 3,
							"y" : 18,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_4.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_4.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_4.sub",
						},
						# number button 5
						{
							"name" : "disable_number_button5",
							"type" : "button",

							"x" : 10,
							"y" : 18 + NUMBER_BUTTON_HEIGHT + NUMBER_BUTTON_GAP_Y,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_5.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_5.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_5.sub",
						},
						# number button 6
						{
							"name" : "disable_number_button6",
							"type" : "button",

							"x" : 10 + NUMBER_BUTTON_WIDTH + NUMBER_BUTTON_GAP_X,
							"y" : 18 + NUMBER_BUTTON_HEIGHT + NUMBER_BUTTON_GAP_Y,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_6.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_6.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_6.sub",
						},
						# number button 7
						{
							"name" : "disable_number_button7",
							"type" : "button",

							"x" : 10 + NUMBER_BUTTON_WIDTH * 2 + NUMBER_BUTTON_GAP_X * 2,
							"y" : 18 + NUMBER_BUTTON_HEIGHT + NUMBER_BUTTON_GAP_Y,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_7.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_7.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_7.sub",
						},
						# number button 8
						{
							"name" : "disable_number_button8",
							"type" : "button",

							"x" : 10 + NUMBER_BUTTON_WIDTH * 3 + NUMBER_BUTTON_GAP_X * 3,
							"y" : 18 + NUMBER_BUTTON_HEIGHT + NUMBER_BUTTON_GAP_Y,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_8.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_8.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_8.sub",
						},
						# number button 9
						{
							"name" : "disable_number_button9",
							"type" : "button",

							"x" : 10,
							"y" : 18,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_9.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_9.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_9.sub",
						},
						# number button 10
						{
							"name" : "disable_number_button10",
							"type" : "button",

							"x" : 10 + NUMBER_BUTTON_WIDTH + NUMBER_BUTTON_GAP_X,
							"y" : 18,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_10.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_10.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_10.sub",
						},
						# number button 11
						{
							"name" : "disable_number_button11",
							"type" : "button",

							"x" : 10 + NUMBER_BUTTON_WIDTH * 2 + NUMBER_BUTTON_GAP_X * 2,
							"y" : 18,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_11.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_11.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_11.sub",
						},
						# number button 12
						{
							"name" : "disable_number_button12",
							"type" : "button",

							"x" : 10 + NUMBER_BUTTON_WIDTH * 3 + NUMBER_BUTTON_GAP_X * 3,
							"y" : 18,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_12.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_12.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_12.sub",
						},
						# number button 13
						{
							"name" : "disable_number_button13",
							"type" : "button",

							"x" : 10,
							"y" : 18 + NUMBER_BUTTON_HEIGHT + NUMBER_BUTTON_GAP_Y,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_13.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_13.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_13.sub",
						},
						# number button 14
						{
							"name" : "disable_number_button14",
							"type" : "button",

							"x" : 10 + NUMBER_BUTTON_WIDTH + NUMBER_BUTTON_GAP_X,
							"y" : 18 + NUMBER_BUTTON_HEIGHT + NUMBER_BUTTON_GAP_Y,

							"default_image" : "d:/ymir work/ui/minigame/attendance/award_14.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/award_14.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/award_14.sub",
						},
						{
							"name" : "day_prev_button",
							"type" : "button",

							"x" : 160 - 13,
							"y" : 192 - 42,

							"default_image" : "d:/ymir work/ui/minigame/attendance/prev_btn.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/prev_btn.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/prev_btn.sub",
						},
						{
							"name" : "day_next_button",
							"type" : "button",

							"x" : 184 - 13,
							"y" : 192 - 42,

							"default_image" : "d:/ymir work/ui/minigame/attendance/next_btn.sub",
							"over_image" : "d:/ymir work/ui/minigame/attendance/next_btn.sub",
							"down_image" : "d:/ymir work/ui/minigame/attendance/next_btn.sub",
						},
						{
							"name" : "number_effect",
							"type" : "ani_image",
							
							"x" : 10,
							"y" : 18,
							
							"delay" : 6,

							"images" :
							(
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect2.sub",
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect3.sub",
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect4.sub",
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect5.sub",
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect4.sub",
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect3.sub",
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect2.sub",
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect1.sub",
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect1.sub",
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect1.sub",
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect1.sub",
								"d:/ymir work/ui/minigame/attendance/number_effect/attendance_button_effect1.sub",
							),
						},
					),
				},
				
				
				{
					"name" : "award_board_window",
					"type" : "window",
					
					"x" : 13,
					"y" : 42 + 171 + 10,
					
					"width" : 325,
					"height" : 171,
					
					"children" :
					(
						## LeftTop 1
						{
							"name" : "LeftTop",
							"type" : "image",
							"x" : 0,
							"y" : 0,
							"image" : BOARD_PATH + "lefttop.sub",
						},
						## RightTop 2
						{
							"name" : "RightTop",
							"type" : "image",
							"x" : 309,
							"y" : 0,
							"image" : BOARD_PATH + "righttop.sub",
						},
						## LeftBottom 3
						{
							"name" : "LeftBottom",
							"type" : "image",
							"x" : 0,
							"y" : 155,
							"image" : BOARD_PATH + "leftbottom.sub",
						},
						## RightBottom 4
						{
							"name" : "RightBottom",
							"type" : "image",
							"x" : 309,
							"y" : 155,
							"image" : BOARD_PATH + "rightbottom.sub",
						},
						## topcenterImg 5
						{
							"name" : "TopCenterImg",
							"type" : "expanded_image",
							"x" : 16,
							"y" : 0,
							"image" : BOARD_PATH + "topcenterImg.tga",
							"rect" : (0.0, 0.0, 17, 0),
						},
						## leftcenterImg 6
						{
							"name" : "LeftCenterImg",
							"type" : "expanded_image",
							"x" : 0,
							"y" : 16,
							"image" : BOARD_PATH + "leftcenterImg.tga",
							"rect" : (0.0, 0.0, 0, 8),
						},
						## rightcenterImg 7
						{
							"name" : "RightCenterImg",
							"type" : "expanded_image",
							"x" : 308,
							"y" : 16,
							"image" : BOARD_PATH + "rightcenterImg.tga",
							"rect" : (0.0, 0.0, 0, 8),
						},
						## bottomcenterImg 8
						{
							"name" : "BottomCenterImg",
							"type" : "expanded_image",
							"x" : 16,
							"y" : 155,
							"image" : BOARD_PATH + "bottomcenterImg.tga",
							"rect" : (0.0, 0.0, 17, 0),
						},
						## centerImg
						{
							"name" : "CenterImg",
							"type" : "expanded_image",
							"x" : 16,
							"y" : 16,
							"image" : BOARD_PATH + "centerImg.tga",
							"rect" : (0.0, 0.0, 17, 8),
						},
						
						## award background
						{
							"name" : "awardBackGround",
							"type" : "image",
							"x" : 0, "y" : 0,
							"image" : DEFAULT_PATH + "award_bg.sub",
						},
						
						{
							"name" : "mainAwardText",
							"type" : "text",
							"x" : 161, "y" : 9, "text" : localeInfo.SPECIAL_GACHA_MAIN, "text_horizontal_align":"center",
						},
						
						{
							"name" : "main_slot",
							"type" : "slot",
							
							"x" : 15,
							"y" : 34,
							
							"width" : SLOT_WIDTH*6 + SLOT_GAP*5,
							"height" : SLOT_HEIGHT,
							
							"image" : "d:/ymir work/ui/public/Slot_Base.sub",
							
							"slot" : 
							(
								{"index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":1, "x":SLOT_WIDTH * 1 + SLOT_GAP * 1, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":2, "x":SLOT_WIDTH * 2 + SLOT_GAP * 2, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":3, "x":SLOT_WIDTH * 3 + SLOT_GAP * 3, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":4, "x":SLOT_WIDTH * 4 + SLOT_GAP * 4, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":5, "x":SLOT_WIDTH * 5 + SLOT_GAP * 5, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
							),
						},
						
						{
							"name" : "subAwardText",
							"type" : "text",
							"x" : 161, "y" : 82, "text" : localeInfo.SPECIAL_GACHA_SUB, "text_horizontal_align":"center",
						},
						
						{
							"name" : "sub_slot",
							"type" : "slot",
							
							"x" : 15,
							"y" : 107,
							
							"width" : SLOT_WIDTH*6 + SLOT_GAP*5,
							"height" : SLOT_HEIGHT,
							
							"image" : "d:/ymir work/ui/public/Slot_Base.sub",
							
							"slot" : 
							(
								{"index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":1, "x":SLOT_WIDTH * 1 + SLOT_GAP * 1, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":2, "x":SLOT_WIDTH * 2 + SLOT_GAP * 2, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":3, "x":SLOT_WIDTH * 3 + SLOT_GAP * 3, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":4, "x":SLOT_WIDTH * 4 + SLOT_GAP * 4, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":5, "x":SLOT_WIDTH * 5 + SLOT_GAP * 5, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
							),
						},
						
						{
							"name" : "award_prev_button",
							"type" : "button",

							"x" : 267,
							"y" : 153,

							"default_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
							"over_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_02.sub",
							"down_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
						},
						{
							"name" : "award_next_button",
							"type" : "button",

							"x" : 298,
							"y" : 153,

							"default_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
							"over_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_02.sub",
							"down_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
						},
					),
				},
				
				{
					"name" : "time1iamge",
					"type" : "image",
					"x" : 135, "y" : MAINBOARD_HEIGHT - 33,
					"image" : DEFAULT_PATH + "time_slot.sub",
					"children" :
					(
						{
							"name" : "timetitle1",
							"type" : "text",
							"x" : -119, "y" : 3, 
							"text" : localeInfo.SPECIAL_GACHA_TIME1,
						},
				
						{
							"name" : "time1",
							"type" : "text",
							"x" : 3, "y" : 3, 
							"text" : "00:00:00",
						},		
					),
				},
				
				{
					"name" : "time2iamge",
					"type" : "image",
					"x" : 135, "y" : MAINBOARD_HEIGHT - 56,
					"image" : DEFAULT_PATH + "time_slot.sub",
					"children" :
					(
						{
							"name" : "timetitle2",
							"type" : "text",
							"x" : -119, "y" : 3, 
							"text" : localeInfo.SPECIAL_GACHA_TIME2,
						},
				
						{
							"name" : "time2",
							"type" : "text",
							"x" : 3, "y" : 3, 
							"text" : "00:00:00",
						},		
					),
				},
				
				## Accpet
				{
					"name" : "AcceptButton",
					"type" : "button",

					"x" : MAINBOARD_WIDTH - 80,
					"y" : MAINBOARD_HEIGHT - 43,

					"text" : localeInfo.SPECIAL_GACHA_ACCEPT,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
			),
		}, ## MainBoard End
	),
}