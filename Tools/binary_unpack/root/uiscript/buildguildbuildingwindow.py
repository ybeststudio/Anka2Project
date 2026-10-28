if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/"
SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_02.sub"
MIDDLE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"

TEMP_X = 110

PLUS_WIDTH = 60
if app.ENABLE_NEW_WORLD_GUILDRENEWAL:
	PLUS_HIGHT = 17
	window = {
		"name" : "BuildGuildBuildingWindow",
		"style" : ("movable", "float",),

		"x" : 10,
		"y" : SCREEN_HEIGHT - 240 - 50,

		"width" : 355+TEMP_X+PLUS_WIDTH,
		"height" : 240 + PLUS_HIGHT,

		"children" :
		(

			{
				"name" : "Board",
				"type" : "board_with_titlebar",

				"x" : 0,
				"y" : 0,

				"width" : 355+TEMP_X+PLUS_WIDTH,
				"height" : 240 + PLUS_HIGHT,

				"title" : uiScriptLocale.GUILD_BUILDING_TITLE,

				"children" :
				(

					{
						"name" : "BuildingCategoryTitle",
						"type" : "text",

						"x" : 15 + 40+PLUS_WIDTH/2,
						"y" : 33,
						"text_horizontal_align" : "center",

						"text" : uiScriptLocale.GUILD_BUILDING_CATEGORY_TITLE,
					},
					{
						"name" : "BuildingCategoryBar",
						"type" : "slotbar",

						"x" : 15,
						"y" : 50,

						"width" : 100+PLUS_WIDTH/2,
						"height" : 80,

						"children" :
						(
							{
								"name" : "CategoryList",
								"type" : "listbox",

								"x" : 0,
								"y" : 1,

								"width" : 100+PLUS_WIDTH/2,
								"height" : 80,
							},
						),
					},

					{
						"name" : "BuildingPriceTitle",
						"type" : "text", "x" : 30, "y" : 143, "text" : uiScriptLocale.GUILD_BUILDING_PRICE,
						"text_horizontal_align" : "center",
						"children" :
						(
							{
								"name":"BuildingPriceSlot",
								"type":"slotbar", "x":20+PLUS_WIDTH/3, "y":-4, "width":60+PLUS_WIDTH/4, "height":17,
								"children" :
								(
									{ "name" : "BuildingPriceValue", "type" : "text", "x" : 0, "y" : 1, "all_align" : "center", "text" : "", },
								),
							},
						),
					},

					{
						"name" : "BuildingMaterialStoneTitle",
						"type" : "text", "x" : 30, "y" : 163, "text" : uiScriptLocale.GUILD_BUILDING_STONE,
						"text_horizontal_align" : "center",
						"children" :
						(
							{
								"name":"BuildingMaterialStoneSlot",
								"type":"slotbar", "x":20+PLUS_WIDTH/3, "y":-4, "width":60+PLUS_WIDTH/4, "height":17,
								"children" :
								(
									{ "name" : "BuildingMaterialStoneValue", "type" : "text", "x" : 0, "y" : 1, "all_align" : "center", "text" : "", },
								),
							},
						),
					},

					{
						"name" : "BuildingMaterialLogTitle",
						"type" : "text", "x" : 30, "y" : 183, "text" : uiScriptLocale.GUILD_BUILDING_LOG,
						"text_horizontal_align" : "center",
						"children" :
						(
							{
								"name":"BuildingMaterialLogSlot",
								"type":"slotbar", "x":20+PLUS_WIDTH/3, "y":-4, "width":60+PLUS_WIDTH/4, "height":17,
								"children" :
								(
									{ "name" : "BuildingMaterialLogValue", "type" : "text", "x" : 0, "y" : 1, "all_align" : "center", "text" : "", },
								),
							},
						),
					},

					{
						"name" : "BuildingMaterialPlywoodTitle",
						"type" : "text", "x" : 30, "y" : 203, "text" : uiScriptLocale.GUILD_BUILDING_PLY,
						"text_horizontal_align" : "center",
						"children" :
						(
							{
								"name":"BuildingMaterialPlywoodSlot",
								"type":"slotbar", "x":20+PLUS_WIDTH/3, "y":-4, "width":60+PLUS_WIDTH/4, "height":17,
								"children" :
								(
									{ "name" : "BuildingMaterialPlywoodValue", "type" : "text", "x" : 0, "y" : 1, "all_align" : "center", "text" : "", },
								),
							},
						),
					},

					{
						"name" : "BuildingMaterialSmeltStoneTitle",
						"type" : "text", "x" : 30, "y" : 223, "text" : uiScriptLocale.GUILD_BUILDING_PLY,
						"text_horizontal_align" : "center",
						"children" :
						(
							{
								"name":"BuildingMaterialSmeltStoneSlot",
								"type":"slotbar", "x":20+PLUS_WIDTH/3, "y":-4, "width":60+PLUS_WIDTH/4, "height":17,
								"children" :
								(
									{ "name" : "BuildingMaterialSmeltStoneValue", "type" : "text", "x" : 0, "y" : 1, "all_align" : "center", "text" : "", },
								),
							},
						),
					},

					{
						"name" : "temp_window",
						"type" : "window",
						"style" : ("not_pick",),

						"x" : TEMP_X,
						"y" : 0,
						"width" : 355+PLUS_WIDTH,
						"height" : 240,

						"children" :
						(

							### START_TEMP
							{
								"name" : "BuildingListTitle",
								"type" : "text",

								"x" : 15 + 50+PLUS_WIDTH,
								"y" : 33,
								"text_horizontal_align" : "center",

								"text" : uiScriptLocale.GUILD_BUILDING_LIST_TITLE,
							},

							{
								"name" : "BuildingListBar",
								"type" : "slotbar",

								"x" : PLUS_WIDTH-15,
								"y" : 50,

								"width" : 120+PLUS_WIDTH/2,
								"height" : 172 + PLUS_HIGHT,

								"children" :
								(
									{
										"name" : "BuildingList",
										"type" : "listbox",

										"x" : 0,
										"y" : 1,

										"width" : 105+PLUS_WIDTH/2,
										"height" : 170 + PLUS_HIGHT,
									},
									{
										"name" : "ListScrollBar",
										"type" : "scrollbar",

										"x" : 15,
										"y" : 2,
										"size" : 172-2 + PLUS_HIGHT,
										"horizontal_align" : "right",
									},
								),
							},

							{
								"name" : "BuildingPositionTitle",
								"type" : "text",

								"x" : 250+PLUS_WIDTH,
								"y" : 33,
								"text_horizontal_align" : "center",

								"text" : uiScriptLocale.GUILD_BUILDING_POSITION,
							},
							{
								"name" : "PositionButton",
								"type" : "radio_button", "x" : 280+PLUS_WIDTH, "y" : 50, "text" : uiScriptLocale.GUILD_BUILDING_CHANGE,
								"default_image" : ROOT_PATH + "Big_Button_01.sub",
								"over_image" : ROOT_PATH + "Big_Button_02.sub",
								"down_image" : ROOT_PATH + "Big_Button_03.sub",
							},
							{
								"name" : "BuildingPositionXTitle",
								"type" : "text", "x" : 150+PLUS_WIDTH, "y" : 53, "text" : "X",
								"text_horizontal_align" : "center",
								"children" :
								(
									{
										"name":"BuildingPositionXSlot",
										"type":"image", "x":23, "y":-3, "image":MIDDLE_VALUE_FILE,
										"children" :
										(
											{ "name" : "BuildingPositionXValue", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "999", },
										),
									},
								),
							},
							{
								"name" : "BuildingPositionY",
								"type" : "text", "x" : 150+PLUS_WIDTH, "y" : 73, "text" : "Y",
								"text_horizontal_align" : "center",
								"children" :
								(
									{
										"name":"BuildingPositionYSlot",
										"type":"image", "x":23, "y":-3, "image":MIDDLE_VALUE_FILE,
										"children" :
										(
											{ "name" : "BuildingPositionYValue", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "999", },
										),
									},
								),
							},

							{
								"name" : "BuildingRotationTitle",
								"type" : "text",

								"x" : 250+PLUS_WIDTH,
								"y" : 95,
								"text_horizontal_align" : "center",

								"text" : uiScriptLocale.GUILD_BUILDING_DIRECTION,
							},
							{
								"name" : "BuildingRotationXTitle",
								"type" : "text", "x" : 150+PLUS_WIDTH, "y" : 115, "text" : "X",
								"text_horizontal_align" : "center",
							},
							{
								"name" : "BuildingRotationX",
								"type" : "sliderbar",
								"x" : 158+PLUS_WIDTH, "y" : 115,
							},
							{
								"name" : "BuildingRotationYTitle",
								"type" : "text", "x" : 150+PLUS_WIDTH, "y" : 135 + 5, "text" : "Y",
								"text_horizontal_align" : "center",
							},
							{
								"name" : "BuildingRotationY",
								"type" : "sliderbar",
								"x" : 158+PLUS_WIDTH, "y" : 135 + 5,
							},
							{
								"name" : "BuildingRotationZTitle",
								"type" : "text", "x" : 150+PLUS_WIDTH, "y" : 155 +  + 10, "text" : "Z",
								"text_horizontal_align" : "center",
							},
							{
								"name" : "BuildingRotationZ",
								"type" : "sliderbar",
								"x" : 158+PLUS_WIDTH, "y" : 155 + 10,
							},

							{
								"name" : "PreviewButton",
								"type" : "toggle_button",

								"x" : 180+PLUS_WIDTH,
								"y" : 60 - PLUS_HIGHT,
								"vertical_align" : "bottom",

								"text" : uiScriptLocale.GUILD_BUILDING_PREVIEW,

								"default_image" : ROOT_PATH + "Large_Button_01.sub",
								"over_image" : ROOT_PATH + "Large_Button_02.sub",
								"down_image" : ROOT_PATH + "Large_Button_03.sub",
							},

							{
								"name" : "AcceptButton",
								"type" : "button",

								"x" : 180+PLUS_WIDTH,
								"y" : 35 - PLUS_HIGHT,

								"text" : uiScriptLocale.ACCEPT,
								"vertical_align" : "bottom",

								"default_image" : ROOT_PATH + "Small_Button_01.sub",
								"over_image" : ROOT_PATH + "Small_Button_02.sub",
								"down_image" : ROOT_PATH + "Small_Button_03.sub",
							},
							{
								"name" : "CancelButton",
								"type" : "button",

								"x" : 225+PLUS_WIDTH,
								"y" : 35 - PLUS_HIGHT,

								"text" : uiScriptLocale.CANCEL,
								"vertical_align" : "bottom",

								"default_image" : ROOT_PATH + "Small_Button_01.sub",
								"over_image" : ROOT_PATH + "Small_Button_02.sub",
								"down_image" : ROOT_PATH + "Small_Button_03.sub",
							},
							{
								"name" : "ChangeButton",
								"type" : "button",

								"x" : 280+PLUS_WIDTH,
								"y" : 55 - PLUS_HIGHT,

								"text" : uiScriptLocale.GUILD_BUILDING_FIX,
								"vertical_align" : "bottom",

								"default_image" : ROOT_PATH + "Big_Button_01.sub",
								"over_image" : ROOT_PATH + "Big_Button_02.sub",
								"down_image" : ROOT_PATH + "Big_Button_03.sub",
							},
							### END_TEMP
						),
					},
				),
			},
		),
	}
else:
	window = {
		"name" : "BuildGuildBuildingWindow",
		"style" : ("movable", "float",),

		"x" : 10,
		"y" : SCREEN_HEIGHT - 240 - 50,

		"width" : 355+TEMP_X+PLUS_WIDTH,
		"height" : 240,

		"children" :
		(

			{
				"name" : "Board",
				"type" : "board_with_titlebar",

				"x" : 0,
				"y" : 0,

				"width" : 355+TEMP_X+PLUS_WIDTH,
				"height" : 240,

				"title" : uiScriptLocale.GUILD_BUILDING_TITLE,

				"children" :
				(

					{
						"name" : "BuildingCategoryTitle",
						"type" : "text",

						"x" : 15 + 40+PLUS_WIDTH/2,
						"y" : 33,
						"text_horizontal_align" : "center",

						"text" : uiScriptLocale.GUILD_BUILDING_CATEGORY_TITLE,
					},
					{
						"name" : "BuildingCategoryBar",
						"type" : "slotbar",

						"x" : 15,
						"y" : 50,

						"width" : 100+PLUS_WIDTH/2,
						"height" : 80,

						"children" :
						(
							{
								"name" : "CategoryList",
								"type" : "listbox",

								"x" : 0,
								"y" : 1,

								"width" : 100+PLUS_WIDTH/2,
								"height" : 80,
							},
						),
					},

					{
						"name" : "BuildingPriceTitle",
						"type" : "text", "x" : 30, "y" : 143, "text" : uiScriptLocale.GUILD_BUILDING_PRICE,
						"text_horizontal_align" : "center",
						"children" :
						(
							{
								"name":"BuildingPriceSlot",
								"type":"slotbar", "x":20+PLUS_WIDTH/3, "y":-4, "width":60+PLUS_WIDTH/4, "height":17,
								"children" :
								(
									{ "name" : "BuildingPriceValue", "type" : "text", "x" : 0, "y" : 1, "all_align" : "center", "text" : "2000000000", },
								),
							},
						),
					},

					{
						"name" : "BuildingMaterialStoneTitle",
						"type" : "text", "x" : 30, "y" : 163, "text" : uiScriptLocale.GUILD_BUILDING_STONE,
						"text_horizontal_align" : "center",
						"children" :
						(
							{
								"name":"BuildingMaterialStoneSlot",
								"type":"slotbar", "x":20+PLUS_WIDTH/3, "y":-4, "width":60+PLUS_WIDTH/4, "height":17,
								"children" :
								(
									{ "name" : "BuildingMaterialStoneValue", "type" : "text", "x" : 0, "y" : 1, "all_align" : "center", "text" : "50000000", },
								),
							},
						),
					},

					{
						"name" : "BuildingMaterialLogTitle",
						"type" : "text", "x" : 30, "y" : 183, "text" : uiScriptLocale.GUILD_BUILDING_LOG,
						"text_horizontal_align" : "center",
						"children" :
						(
							{
								"name":"BuildingMaterialLogSlot",
								"type":"slotbar", "x":20+PLUS_WIDTH/3, "y":-4, "width":60+PLUS_WIDTH/4, "height":17,
								"children" :
								(
									{ "name" : "BuildingMaterialLogValue", "type" : "text", "x" : 0, "y" : 1, "all_align" : "center", "text" : "50000000", },
								),
							},
						),
					},

					{
						"name" : "BuildingMaterialPlywoodTitle",
						"type" : "text", "x" : 30, "y" : 203, "text" : uiScriptLocale.GUILD_BUILDING_PLY,
						"text_horizontal_align" : "center",
						"children" :
						(
							{
								"name":"BuildingMaterialPlywoodSlot",
								"type":"slotbar", "x":20+PLUS_WIDTH/3, "y":-4, "width":60+PLUS_WIDTH/4, "height":17,
								"children" :
								(
									{ "name" : "BuildingMaterialPlywoodValue", "type" : "text", "x" : 0, "y" : 1, "all_align" : "center", "text" : "50000000", },
								),
							},
						),
					},

					{
						"name" : "temp_window",
						"type" : "window",
						"style" : ("not_pick",),

						"x" : TEMP_X,
						"y" : 0,
						"width" : 355+PLUS_WIDTH,
						"height" : 240,

						"children" :
						(

							### START_TEMP
							{
								"name" : "BuildingListTitle",
								"type" : "text",

								"x" : 15 + 50+PLUS_WIDTH,
								"y" : 33,
								"text_horizontal_align" : "center",

								"text" : uiScriptLocale.GUILD_BUILDING_LIST_TITLE,
							},

							{
								"name" : "BuildingListBar",
								"type" : "slotbar",

								"x" : PLUS_WIDTH-15,
								"y" : 50,

								"width" : 120+PLUS_WIDTH/2,
								"height" : 172,

								"children" :
								(
									{
										"name" : "BuildingList",
										"type" : "listbox",

										"x" : 0,
										"y" : 1,

										"width" : 105+PLUS_WIDTH/2,
										"height" : 170,
									},
									{
										"name" : "ListScrollBar",
										"type" : "scrollbar",

										"x" : 15,
										"y" : 2,
										"size" : 172-2,
										"horizontal_align" : "right",
									},
								),
							},

							{
								"name" : "BuildingPositionTitle",
								"type" : "text",

								"x" : 250+PLUS_WIDTH,
								"y" : 33,
								"text_horizontal_align" : "center",

								"text" : uiScriptLocale.GUILD_BUILDING_POSITION,
							},
							{
								"name" : "PositionButton",
								"type" : "radio_button", "x" : 280+PLUS_WIDTH, "y" : 50, "text" : uiScriptLocale.GUILD_BUILDING_CHANGE,
								"default_image" : ROOT_PATH + "Big_Button_01.sub",
								"over_image" : ROOT_PATH + "Big_Button_02.sub",
								"down_image" : ROOT_PATH + "Big_Button_03.sub",
							},
							{
								"name" : "BuildingPositionXTitle",
								"type" : "text", "x" : 150+PLUS_WIDTH, "y" : 53, "text" : "X",
								"text_horizontal_align" : "center",
								"children" :
								(
									{
										"name":"BuildingPositionXSlot",
										"type":"image", "x":23, "y":-3, "image":MIDDLE_VALUE_FILE,
										"children" :
										(
											{ "name" : "BuildingPositionXValue", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "999", },
										),
									},
								),
							},
							{
								"name" : "BuildingPositionY",
								"type" : "text", "x" : 150+PLUS_WIDTH, "y" : 73, "text" : "Y",
								"text_horizontal_align" : "center",
								"children" :
								(
									{
										"name":"BuildingPositionYSlot",
										"type":"image", "x":23, "y":-3, "image":MIDDLE_VALUE_FILE,
										"children" :
										(
											{ "name" : "BuildingPositionYValue", "type" : "text", "x" : 0, "y" : 0, "all_align" : "center", "text" : "999", },
										),
									},
								),
							},

							{
								"name" : "BuildingRotationTitle",
								"type" : "text",

								"x" : 250+PLUS_WIDTH,
								"y" : 95,
								"text_horizontal_align" : "center",

								"text" : uiScriptLocale.GUILD_BUILDING_DIRECTION,
							},
							{
								"name" : "BuildingRotationXTitle",
								"type" : "text", "x" : 150+PLUS_WIDTH, "y" : 115, "text" : "X",
								"text_horizontal_align" : "center",
							},
							{
								"name" : "BuildingRotationX",
								"type" : "sliderbar",
								"x" : 158+PLUS_WIDTH, "y" : 115,
							},
							{
								"name" : "BuildingRotationYTitle",
								"type" : "text", "x" : 150+PLUS_WIDTH, "y" : 135, "text" : "Y",
								"text_horizontal_align" : "center",
							},
							{
								"name" : "BuildingRotationY",
								"type" : "sliderbar",
								"x" : 158+PLUS_WIDTH, "y" : 135,
							},
							{
								"name" : "BuildingRotationZTitle",
								"type" : "text", "x" : 150+PLUS_WIDTH, "y" : 155, "text" : "Z",
								"text_horizontal_align" : "center",
							},
							{
								"name" : "BuildingRotationZ",
								"type" : "sliderbar",
								"x" : 158+PLUS_WIDTH, "y" : 155,
							},

							{
								"name" : "PreviewButton",
								"type" : "toggle_button",

								"x" : 180+PLUS_WIDTH,
								"y" : 60,
								"vertical_align" : "bottom",

								"text" : uiScriptLocale.GUILD_BUILDING_PREVIEW,

								"default_image" : ROOT_PATH + "Large_Button_01.sub",
								"over_image" : ROOT_PATH + "Large_Button_02.sub",
								"down_image" : ROOT_PATH + "Large_Button_03.sub",
							},

							{
								"name" : "AcceptButton",
								"type" : "button",

								"x" : 180+PLUS_WIDTH,
								"y" : 35,

								"text" : uiScriptLocale.ACCEPT,
								"vertical_align" : "bottom",

								"default_image" : ROOT_PATH + "Small_Button_01.sub",
								"over_image" : ROOT_PATH + "Small_Button_02.sub",
								"down_image" : ROOT_PATH + "Small_Button_03.sub",
							},
							{
								"name" : "CancelButton",
								"type" : "button",

								"x" : 225+PLUS_WIDTH,
								"y" : 35,

								"text" : uiScriptLocale.CANCEL,
								"vertical_align" : "bottom",

								"default_image" : ROOT_PATH + "Small_Button_01.sub",
								"over_image" : ROOT_PATH + "Small_Button_02.sub",
								"down_image" : ROOT_PATH + "Small_Button_03.sub",
							},
							{
								"name" : "ChangeButton",
								"type" : "button",

								"x" : 280+PLUS_WIDTH,
								"y" : 55,

								"text" : uiScriptLocale.GUILD_BUILDING_FIX,
								"vertical_align" : "bottom",

								"default_image" : ROOT_PATH + "Big_Button_01.sub",
								"over_image" : ROOT_PATH + "Big_Button_02.sub",
								"down_image" : ROOT_PATH + "Big_Button_03.sub",
							},
							### END_TEMP
						),
					},
				),
			},
		),
	}
