import uiScriptLocale
import app

ROOT_PATH = "d:/ymir work/ui/public/"
SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_02.sub"
MIDDLE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
window = {
	"name" : "BuildGuildBuildingChangeWindow",
	"style" : ("movable", "float",),

	"x" : 10,
	"y" : SCREEN_HEIGHT/2-100,

	"width" : 215,
	"height" : 330,

	"children" :
	(
		{
			"name" : "Board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : 215,
			"height" : 330,

			"title" : uiScriptLocale.GUILD_BUILDING_CHANGE_FIX,

			"children" :
			(
				{
					"name" : "temp_window",
					"type" : "window",
					"style" : ("not_pick",),

					"x" : 0,
					"y" : 0,
					"width" : 215,
					"height" : 330,

					"children" :
					(

						### START_TEMP
						{
							"name" : "InBuildingListTitle",
							"type" : "text",

							"x" : 215/2,
							"y" : 34,
							"text_horizontal_align" : "center",

							"text" : uiScriptLocale.GUILD_BUILDING_CHANGE_LIST,
						},

						{
							"name" : "BuildingListBar",
							"type" : "slotbar",

							"x" : 15,
							"y" : 50,

							"width" : 180,
							"height" : 172,

							"children" :
							(
								{
									"name" : "ChangeBuildingList",
									"type" : "listbox",

									"x" : 0,
									"y" : 1,

									"width" : 165,
									"height" : 170,
								},
								{
									"name" : "ChangeListScrollBar",
									"type" : "scrollbar",

									"x" : 15,
									"y" : 2,
									"size" : 172-2,
									"horizontal_align" : "right",
								},
							),
						},
						{
							"name" : "PositionButton",
							"type" : "button", 
							"x" : 40,
							"y" : 260, 
							"text" : uiScriptLocale.GUILD_BUILDING_CHANGE,
							"default_image" : ROOT_PATH + "Middle_Button_01.sub",
							"over_image" : ROOT_PATH + "Middle_Button_02.sub",
							"down_image" : ROOT_PATH + "Middle_Button_03.sub",
						},
						{
							"name" : "DeleteButton",
							"type" : "button", 
							"x" : 115, 
							"y" : 260, 
							"text" : uiScriptLocale.GUILD_BUILDING_CHANGE_DELETE,
							"default_image" : ROOT_PATH + "Middle_Button_01.sub",
							"over_image" : ROOT_PATH + "Middle_Button_02.sub",
							"down_image" : ROOT_PATH + "Middle_Button_03.sub",
						},
						{
							"name" : "BuildingRotationZTitle",
							"type" : "text", "x" : 15, "y" : 240, "text" : "Z",
							"text_horizontal_align" : "center",
						},
						{
							"name" : "BuildingRotationZ",
							"type" : "sliderbar",
							"x" : 23, "y" : 240,
						},
						{
							"name" : "AcceptButton",
							"type" : "button",

							"x" : 40,
							"y" : 290,

							"text" : uiScriptLocale.ACCEPT,

							"default_image" : ROOT_PATH + "Middle_Button_01.sub",
							"over_image" : ROOT_PATH + "Middle_Button_02.sub",
							"down_image" : ROOT_PATH + "Middle_Button_03.sub",
						},
						{
							"name" : "CancelButton",
							"type" : "button",

							"x" : 115,
							"y" : 290,

							"text" : uiScriptLocale.CANCEL,

							"default_image" : ROOT_PATH + "Middle_Button_01.sub",
							"over_image" : ROOT_PATH + "Middle_Button_02.sub",
							"down_image" : ROOT_PATH + "Middle_Button_03.sub",
						},
					),
				},
			),
		},
	),
}