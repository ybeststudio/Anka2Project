import uiScriptLocale
import localeInfo

BOARD_PATH = "d:/ymir work/ui/minigame/attendance/number_board/"

IN_GAME_UI_WIDTH = 337
CALENDAR_UI_WIDTH = IN_GAME_UI_WIDTH - 14

window = {
	"name" : "InGameEventWindow",
	"type" : "window",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH - 240 - IN_GAME_UI_WIDTH,
	"y" : 15,

	"width" : IN_GAME_UI_WIDTH,
	"height" : 100,

	"children" :
	[
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : IN_GAME_UI_WIDTH,
			"height" : 100,

			"children" :
			[
				{
					"name" : "Event_Title_Bar", "type" : "titlebar", "x" : 0, "y" : 5, "width" : IN_GAME_UI_WIDTH - 5, "height" : 21, "style" : ("attach",),
					"children" :
					(
						{"name":"TitleName", "type":"text", "x":0, "y":0, "text":uiScriptLocale.EVENT_ALARM_TITLE, "all_align" : "center"},
					),
				},

				{
					"name" : "calendar_board_window",
					"type" : "window",
					"style" : ("attach",),

					"x" : 7,
					"y" : 25,

					"width" : CALENDAR_UI_WIDTH,
					"height" : 65,

					"children" :
					[
						{
							"name" : "LeftTop",
							"type" : "image",
							"x" : 0,
							"y" : 0,
							"image" : BOARD_PATH + "lefttop.sub",
						},

						{
							"name" : "RightTop",
							"type" : "image",
							"x" : CALENDAR_UI_WIDTH - 17,
							"y" : 0,
							"image" : BOARD_PATH + "righttop.sub",
						},

						{
							"name" : "LeftBottom",
							"type" : "image",
							"x" : 0,
							"y" : 65 - 16,
							"image" : BOARD_PATH + "leftbottom.sub",
						},

						{
							"name" : "RightBottom",
							"type" : "image",
							"x" : CALENDAR_UI_WIDTH - 17,
							"y" : 65 - 16,
							"image" : BOARD_PATH + "rightbottom.sub",
						},

						{
							"name" : "TopCenterImg",
							"type" : "expanded_image",
							"x" : 16,
							"y" : 0,
							"image" : BOARD_PATH + "topcenterImg.tga",
							"rect" : (0.0, 0.0, 16.9, 0),
						},

						{
							"name" : "LeftCenterImg",
							"type" : "expanded_image",
							"x" : 0,
							"y" : 16,
							"image" : BOARD_PATH + "leftcenterImg.tga",
							"rect" : (0.0, 0.0, 0, 1),
						},

						{
							"name" : "RightCenterImg",
							"type" : "expanded_image",
							"x" : CALENDAR_UI_WIDTH - 18,
							"y" : 16,
							"image" : BOARD_PATH + "rightcenterImg.tga",
							"rect" : (0.0, 0.0, 0, 1),
						},

						{
							"name" : "BottomCenterImg",
							"type" : "expanded_image",
							"x" : 16,
							"y" : 65 - 16,
							"image" : BOARD_PATH + "bottomcenterImg.tga",
							"rect" : (0.0, 0.0, 16.9, 0),
						},

						{
							"name" : "CenterImg",
							"type" : "expanded_image",
							"x" : 16,
							"y" : 16,
							"image" : BOARD_PATH + "centerImg.tga",
							"rect" : (0.0, 0.0, 16, 1),
						},

						{ "name" : "calendar_01", "type" : "expanded_image", "style" : ("attach",), "x_scale" : 0.6, "y_scale" : 0.6, "x" : 3, "y" : 5, "image" : "d:/ymir work/ui/minigame/attendance/attendance_disable_number1.sub" },
						{ "name" : "calendar_02", "type" : "expanded_image", "style" : ("attach",), "x_scale" : 0.6, "y_scale" : 0.6, "x" : 48, "y" : 5, "image" : "d:/ymir work/ui/minigame/attendance/attendance_disable_number2.sub" },
						{ "name" : "calendar_03", "type" : "expanded_image", "style" : ("attach",), "x_scale" : 0.6, "y_scale" : 0.6, "x" : 93, "y" : 5, "image" : "d:/ymir work/ui/minigame/attendance/attendance_disable_number3.sub" },
						{ "name" : "calendar_04", "type" : "expanded_image", "style" : ("attach",), "x_scale" : 0.6, "y_scale" : 0.6, "x" : 138, "y" : 5, "image" : "d:/ymir work/ui/minigame/attendance/attendance_disable_number4.sub" },
						{ "name" : "calendar_05", "type" : "expanded_image", "style" : ("attach",), "x_scale" : 0.6, "y_scale" : 0.6, "x" : 183, "y" : 5, "image" : "d:/ymir work/ui/minigame/attendance/attendance_disable_number5.sub" },
						{ "name" : "calendar_06", "type" : "expanded_image", "style" : ("attach",), "x_scale" : 0.6, "y_scale" : 0.6, "x" : 228, "y" : 5, "image" : "d:/ymir work/ui/minigame/attendance/attendance_disable_number6.sub" },
						{ "name" : "calendar_07", "type" : "expanded_image", "style" : ("attach",), "x_scale" : 0.6, "y_scale" : 0.6, "x" : 273, "y" : 5, "image" : "d:/ymir work/ui/minigame/attendance/attendance_disable_number7.sub" },

						{
							"name" : "calendar_effect",
							"type" : "ani_image",

							"x" : 3,
							"y" : 5,

							"x_scale" : 0.65,
							"y_scale" : 0.65,

							"delay" : 5,

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
						
						{
							"name" : "prev_button",
							"type" : "button",

							"x" : CALENDAR_UI_WIDTH / 2 - 10 - 20,
							"y" : 46,

							"tooltip_text" : localeInfo.UI_PREVPAGE,

							"default_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
							"over_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
							"down_image" : "d:/ymir work/ui/public/public_intro_btn/prev_btn_02.sub",
						},
						{
							"name" : "next_button",
							"type" : "button",

							"x" : CALENDAR_UI_WIDTH / 2 + 10,
							"y" : 46,

							"tooltip_text" : localeInfo.UI_NEXTPAGE,

							"default_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
							"over_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
							"down_image" : "d:/ymir work/ui/public/public_intro_btn/next_btn_02.sub",
						},
					],
				},

				{
					"name" : "EventRewardBarWindow",
					"type" : "window", 
					"style" : ("attach",),

					"x" : 10,
					"y" : 95,

					"width" : 309,
					"height" : 21,

					"children" :
					[
						{ "name" : "bg_menu_tab", "type" : "expanded_image", "style" : ("attach",), "x" : 0, "y" : 0, "image" : "d:/ymir work/ui/event/bg_menu_tab.sub" },

						{
							"name" : "Event_Name_Bar", "type" : "window", "x" : 0, "y" : 0, "width" : 197-10, "height" : 21, "style" : ("attach",),
							"children" :
							[
								{"name":"TitleName", "type":"text", "x":0, "y":0, "text":uiScriptLocale.EVENT_NAME_TITLE, "all_align" : "center",},
							],
						},

						{
							"name" : "Event_Reward_Bar", "type" : "window", "x" : 197, "y" : 0, "width" : 309 - 197 - 10, "height" : 21, "style" : ("attach",),
							"children" :
							[
								{"name":"TitleName", "type":"text", "x":0, "y":0, "text":uiScriptLocale.EVENT_REWARD_TITLE, "all_align" : "center",},
							],
						},
					],
				},
			],
		},
	],
}