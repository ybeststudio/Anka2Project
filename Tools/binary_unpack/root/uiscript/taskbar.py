if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

ROOT = "d:/ymir work/ui/game/"
GAME_PATH = "d:/ymir work/ui/game/"
PATTERN_PATH = "d:/ymir work/ui/pattern/"

#Y_ADD_POSITION = -2
Y_ADD_POSITION = 0

window = {
	"name" : "TaskBar",

	"x" : 0,
	"y" : SCREEN_HEIGHT - 37,

	"width" : SCREEN_WIDTH,
	"height" : 37,

	"children" :
	(
		## Board
		{
			"name" : "Base_Board_01",
			"type" : "expanded_image",

			"x" : 263,
			"y" : 0,

			"rect" : (0.0, 0.0, float(SCREEN_WIDTH - 263 - 256) / 256.0, 0.0),

			"image" : PATTERN_PATH + "TaskBar_Base.tga"
		},

		## Gauge
		{
			"name" : "Gauge_Board",
			"type" : "image",

			"x" : 0,
			"y" : -10 + Y_ADD_POSITION,

			"image" : GAME_PATH + "taskbar/gauge.sub",

			"children" :
			(
				{
					"name" : "RampageGauge",
					"type" : "ani_image",

					"x" : 8,
					"y" : 4,
					"width"  : 40,
					"height" : 40,

					"delay" : 6,

					"images" :
					(
						GAME_PATH + "mall/00.sub",
						GAME_PATH + "mall/01.sub",
						GAME_PATH + "mall/02.sub",
						GAME_PATH + "mall/03.sub",
						GAME_PATH + "mall/04.sub",
						GAME_PATH + "mall/05.sub",
						GAME_PATH + "mall/06.sub",
						GAME_PATH + "mall/07.sub",
						GAME_PATH + "mall/08.sub",
						GAME_PATH + "mall/09.sub",
						GAME_PATH + "mall/11.sub",
						GAME_PATH + "mall/12.sub",
						GAME_PATH + "mall/13.sub",
						GAME_PATH + "mall/14.sub",
						GAME_PATH + "mall/15.sub",
						GAME_PATH + "mall/16.sub",
					)
				},
				{
					"name" : "RampageGauge2",
					"type" : "ani_image",

					"x" : 8,
					"y" : 4,
					"width"  : 40,
					"height" : 40,

					"delay" : 6,

					"images" :
					(
						GAME_PATH + "mall/00.sub",
						GAME_PATH + "mall/01.sub",
						GAME_PATH + "mall/02.sub",
						GAME_PATH + "mall/03.sub",
						GAME_PATH + "mall/04.sub",
						GAME_PATH + "mall/05.sub",
						GAME_PATH + "mall/06.sub",
						GAME_PATH + "mall/07.sub",
						GAME_PATH + "mall/08.sub",
						GAME_PATH + "mall/09.sub",
						GAME_PATH + "mall/11.sub",
						GAME_PATH + "mall/12.sub",
						GAME_PATH + "mall/13.sub",
						GAME_PATH + "mall/14.sub",
						GAME_PATH + "mall/15.sub",
						GAME_PATH + "mall/16.sub",
					)
				},
				{
					## Tooltip popup for
					"name" : "HPGauge_Board",
					"type" : "window",

					"x" : 59,
					"y" : 14,

					"width" : 95,
					"height" : 11,

					"children" :
					(
						{
							"name" : "HPRecoveryGaugeBar",
							"type" : "bar",

							"x" : 0,
							"y" : 0,
							"width" : 95,
							"height" : 13,
							"color" : 0x55ff0000,
						},
						{
							"name" : "HPGauge",
							"type" : "ani_image",

							"x" : 0,
							"y" : 0,

							"delay" : 6,

							"images" :
							(
								PATTERN_PATH + "HPGauge/01.tga",
								PATTERN_PATH + "HPGauge/02.tga",
								PATTERN_PATH + "HPGauge/03.tga",
								PATTERN_PATH + "HPGauge/04.tga",
								PATTERN_PATH + "HPGauge/05.tga",
								PATTERN_PATH + "HPGauge/06.tga",
								PATTERN_PATH + "HPGauge/07.tga",
							),
						},
					),
				},
				{
					## Tooltip popup for
					"name" : "SPGauge_Board",
					"type" : "window",

					"x" : 59,
					"y" : 24,

					"width" : 95,
					"height" : 11,

					"children" :
					(
						{
							"name" : "SPRecoveryGaugeBar",
							"type" : "bar",

							"x" : 0,
							"y" : 0,
							"width" : 95,
							"height" : 13,
							"color" : 0x550000ff,
						},
						{
							"name" : "SPGauge",
							"type" : "ani_image",

							"x" : 0,
							"y" : 0,

							"delay" : 6,

							"images" :
							(
								PATTERN_PATH + "SPGauge/01.tga",
								PATTERN_PATH + "SPGauge/02.tga",
								PATTERN_PATH + "SPGauge/03.tga",
								PATTERN_PATH + "SPGauge/04.tga",
								PATTERN_PATH + "SPGauge/05.tga",
								PATTERN_PATH + "SPGauge/06.tga",
								PATTERN_PATH + "SPGauge/07.tga",
							),
						},
					),
				},
				{
					## Tooltip popup for
					"name" : "STGauge_Board",
					"type" : "window",

					"x" : 59,
					"y" : 38,

					"width" : 95,
					"height" : 6,

					"children" :
					(
						{
							"name" : "STGauge",
							"type" : "ani_image",

							"x" : 0,
							"y" : 0,

							"delay" : 6,

							"images" :
							(
								PATTERN_PATH + "STGauge/01.tga",
								PATTERN_PATH + "STGauge/02.tga",
								PATTERN_PATH + "STGauge/03.tga",
								PATTERN_PATH + "STGauge/04.tga",
								PATTERN_PATH + "STGauge/05.tga",
								PATTERN_PATH + "STGauge/06.tga",
								PATTERN_PATH + "STGauge/07.tga",
							),
						},
					),
				},

			),
		},
		{
			"name" : "EXP_Gauge_Board",
			"type" : "image",

			"x" : 158,
			"y" : 0 + Y_ADD_POSITION,

			"image" : GAME_PATH + "taskbar/exp_gauge.sub",

			"children" :
			(
				{
					"name" : "EXPGauge_01",
					"type" : "expanded_image",

					"x" : 5,
					"y" : 9,

					"image" : GAME_PATH + "TaskBar/EXP_Gauge_Point.sub",
				},
				{
					"name" : "EXPGauge_02",
					"type" : "expanded_image",

					"x" : 30,
					"y" : 9,

					"image" : GAME_PATH + "TaskBar/EXP_Gauge_Point.sub",
				},
				{
					"name" : "EXPGauge_03",
					"type" : "expanded_image",

					"x" : 55,
					"y" : 9,

					"image" : GAME_PATH + "TaskBar/EXP_Gauge_Point.sub",
				},
				{
					"name" : "EXPGauge_04",
					"type" : "expanded_image",

					"x" : 80,
					"y" : 9,

					"image" : GAME_PATH + "TaskBar/EXP_Gauge_Point.sub",
				},
			),
		},

		## Mouse Button
		{
			"name" : "LeftMouseButton",
			"type" : "button",

			"x" : SCREEN_WIDTH/2 - 128,
			"y" : 3 + Y_ADD_POSITION,

			"default_image" : GAME_PATH + "TaskBar/Mouse_Button_Move_01.sub",
			"over_image" : GAME_PATH + "TaskBar/Mouse_Button_Move_02.sub",
			"down_image" : GAME_PATH + "TaskBar/Mouse_Button_Move_03.sub",
		},
		{
			"name" : "RightMouseButton",
			"type" : "button",

			"x" : SCREEN_WIDTH/2 + 128 + 66 + 11,
			"y" : 3 + Y_ADD_POSITION,

			"default_image" : GAME_PATH + "TaskBar/Mouse_Button_Move_01.sub",
			"over_image" : GAME_PATH + "TaskBar/Mouse_Button_Move_02.sub",
			"down_image" : GAME_PATH + "TaskBar/Mouse_Button_Move_03.sub",
		},

		## Button
		{
			"name" : "ExpandMoneyButton",
			"type" : "button",

			"x" : SCREEN_WIDTH - 178,
			"y" : 3 + Y_ADD_POSITION,
			"tooltip_text" : "			" + uiScriptLocale.TASKBAR_MONEY_EXPAND,

			"default_image" : GAME_PATH + "TaskBar/Ex_gemshop_button_01.tga",
			"over_image" : GAME_PATH + "TaskBar/Ex_gemshop_button_02.tga",
			"down_image" : GAME_PATH + "TaskBar/Ex_gemshop_button_03.tga",
		},
		{
			"name" : "CharacterButton",
			"type" : "button",

			"x" : SCREEN_WIDTH - 144,
			"y" : 3 + Y_ADD_POSITION,

			"tooltip_text" : uiScriptLocale.TASKBAR_CHARACTER,

			"default_image" : GAME_PATH + "TaskBar/Character_Button_01.sub",
			"over_image" : GAME_PATH + "TaskBar/Character_Button_02.sub",
			"down_image" : GAME_PATH + "TaskBar/Character_Button_03.sub",
		},
		{
			"name" : "InventoryButton",
			"type" : "button",

			"x" : SCREEN_WIDTH - 110,
			"y" : 3 + Y_ADD_POSITION,

			"tooltip_text" : uiScriptLocale.TASKBAR_INVENTORY,

			"default_image" : GAME_PATH + "TaskBar/Inventory_Button_01.sub",
			"over_image" : GAME_PATH + "TaskBar/Inventory_Button_02.sub",
			"down_image" : GAME_PATH + "TaskBar/Inventory_Button_03.sub",
		},
		{
			"name" : "MessengerButton",
			"type" : "button",

			"x" : SCREEN_WIDTH - 76,
			"y" : 3 + Y_ADD_POSITION,

			"tooltip_text" : uiScriptLocale.TASKBAR_MESSENGER,

			"default_image" : GAME_PATH + "TaskBar/Community_Button_01.sub",
			"over_image" : GAME_PATH + "TaskBar/Community_Button_02.sub",
			"down_image" : GAME_PATH + "TaskBar/Community_Button_03.sub",
		},
		{
			"name" : "SystemButton",
			"type" : "button",

			"x" : SCREEN_WIDTH - 42,
			"y" : 3 + Y_ADD_POSITION,

			"tooltip_text" : uiScriptLocale.TASKBAR_SYSTEM,

			"default_image" : GAME_PATH + "TaskBar/System_Button_01.sub",
			"over_image" : GAME_PATH + "TaskBar/System_Button_02.sub",
			"down_image" : GAME_PATH + "TaskBar/System_Button_03.sub",
		},

		## QuickBar
		{
			"name" : "quickslot_board",
			"type" : "window",

			"x" : SCREEN_WIDTH/2 - 128 + 32 + 10,
			"y" : 0 + Y_ADD_POSITION,

			"width" : 256 + 14 + 2 + 11,
			"height" : 37,

			"children" :
			(
				{
					# ExpandButton ChatButton
					# ExpandButton.
					"name" : "ExpandButton",
					"type" : "button",

					"x" : 128,
					"y" : 1,
					"tooltip_text" : uiScriptLocale.TASKBAR_EXPAND,

					"default_image" : GAME_PATH + "TaskBar/Chat_Button_01.sub",
					"over_image" : GAME_PATH + "TaskBar/Chat_Button_02.sub",
					"down_image" : GAME_PATH + "TaskBar/Chat_Button_03.sub",
				},
				{
					"name" : "quick_slot_1",
					"type" : "grid_table",

					"start_index" : 0,

					"x" : 0,
					"y" : 3,

					"x_count" : 4,
					"y_count" : 1,
					"x_step" : 32,
					"y_step" : 32,

					"image" : "d:/ymir work/ui/Public/Slot_Base.sub",
					"image_r" : 1.0,
					"image_g" : 1.0,
					"image_b" : 1.0,
					"image_a" : 1.0,

					"children" :
					(
						{ "name" : "slot_1", "type" : "image", "x" : 3, "y" : 3, "image" : GAME_PATH + "taskbar/1.sub", },
						{ "name" : "slot_2", "type" : "image", "x" : 35, "y" : 3, "image" : GAME_PATH + "taskbar/2.sub", },
						{ "name" : "slot_3", "type" : "image", "x" : 67, "y" : 3, "image" : GAME_PATH + "taskbar/3.sub", },
						{ "name" : "slot_4", "type" : "image", "x" : 99, "y" : 3, "image" : GAME_PATH + "taskbar/4.sub", },
					),
				},
				{
					"name" : "quick_slot_2",
					"type" : "grid_table",

					"start_index" : 4,

					"x" : 128 + 14,
					"y" : 3,

					"x_count" : 4,
					"y_count" : 1,
					"x_step" : 32,
					"y_step" : 32,

					"image" : "d:/ymir work/ui/Public/Slot_Base.sub",
					"image_r" : 1.0,
					"image_g" : 1.0,
					"image_b" : 1.0,
					"image_a" : 1.0,

					"children" :
					(
						{ "name" : "slot_5", "type" : "image", "x" : 3, "y" : 3, "image" : GAME_PATH + "taskbar/f1.sub", },
						{ "name" : "slot_6", "type" : "image", "x" : 35, "y" : 3, "image" : GAME_PATH + "taskbar/f2.sub", },
						{ "name" : "slot_7", "type" : "image", "x" : 67, "y" : 3, "image" : GAME_PATH + "taskbar/f3.sub", },
						{ "name" : "slot_8", "type" : "image", "x" : 99, "y" : 3, "image" : GAME_PATH + "taskbar/f4.sub", },
					),
				},
				{
					"name" : "QuickSlotBoard",
					"type" : "window",

					"x" : 128+14+128+2,
					"y" : 0,
					"width" : 11,
					"height" : 37,
					"children" :
					(
						{
							"name" : "QuickSlotNumberBox",
							"type" : "image",
							"x" : 1,
							"y" : 15,
							"image" : GAME_PATH + "taskbar/QuickSlot_Button_Board.sub",
						},
						{
							"name" : "QuickPageUpButton",
							"type" : "button",
							"tooltip_text" : uiScriptLocale.TASKBAR_PREV_QUICKSLOT,
							"x" : 1,
							"y" : 9,
							"default_image" : GAME_PATH + "TaskBar/QuickSlot_UpButton_01.sub",
							"over_image" : GAME_PATH + "TaskBar/QuickSlot_UpButton_02.sub",
							"down_image" : GAME_PATH + "TaskBar/QuickSlot_UpButton_03.sub",
						},

						{
							"name" : "QuickPageNumber",
							"type" : "image",
							"x" : 3, "y" : 15, "image" : GAME_PATH + "taskbar/1.sub",
						},
						{
							"name" : "QuickPageDownButton",
							"type" : "button",
							"tooltip_text" : uiScriptLocale.TASKBAR_NEXT_QUICKSLOT,

							"x" : 1,
							"y" : 24,

							"default_image" : GAME_PATH + "TaskBar/QuickSlot_DownButton_01.sub",
							"over_image" : GAME_PATH + "TaskBar/QuickSlot_DownButton_02.sub",
							"down_image" : GAME_PATH + "TaskBar/QuickSlot_DownButton_03.sub",
						},
					),
				},
			),
		},
	),
}

if app.ENABLE_ANTI_EXP:
	window["children"] = window["children"] + (
					{
						"name" : "AntiExpButton",
						"type" : "button",

						"x" : 265,
						"y" : 3 + Y_ADD_POSITION,

						# "tooltip_text" : uiScriptLocale.TASKBAR_ANTI_EXP,

						"default_image" : GAME_PATH + "TaskBar/antiexp_button_04.tga",
						"over_image" : GAME_PATH + "TaskBar/antiexp_button_05.tga",
						"down_image" : GAME_PATH + "TaskBar/antiexp_button_06.tga",
					},)

if app.ENABLE_CONQUEROR_LEVEL:
	window["children"][2]["children"] = window["children"][2]["children"] + (
	{
		"name" : "NewExpGauge_01",
		"type" : "expanded_image",

		"x" : 5,
		"y" : 9,

		"image" : ROOT + "TaskBar/new_exp_gauge_point.sub",
	},
	{
		"name" : "NewExpGauge_02",
		"type" : "expanded_image",

		"x" : 30,
		"y" : 9,

		"image" : ROOT + "TaskBar/new_exp_gauge_point.sub",
	},
	{
		"name" : "NewExpGauge_03",
		"type" : "expanded_image",

		"x" : 55,
		"y" : 9,

		"image" : ROOT + "TaskBar/new_exp_gauge_point.sub",
	},
	{
		"name" : "NewExpGauge_04",
		"type" : "expanded_image",

		"x" : 80,
		"y" : 9,

		"image" : ROOT + "TaskBar/new_exp_gauge_point.sub",
	},)
