if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
XLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_04.sub"

if not app.ENABLE_GUILD_VOTE_SYSTEM_OFF:
	window = {
		"name" : "GuildLandDealVoteDialog",
		"style" : ("movable", "float",),

		"x" : SCREEN_WIDTH/2 - 250,
		"y" : SCREEN_HEIGHT/2 - 40,

		"width" : 280,
		"height" : 140,

		"children" :
		(
			{
				"name" : "board",
				"type" : "board",

				"x" : 0,
				"y" : 0,

				"width" : 280,
				"height" : 140,

				"children" :
				(
					## Title
					{
						"name" : "titlebar",
						"type" : "titlebar",
						"style" : ("attach",),

						"x" : 8,
						"y" : 8,

						"width" : 263,
						"color" : "gray",

						"children" :
						(
							{ "name":"titlename", "type":"text", "x":0, "y":3, 
							"text" : uiScriptLocale.GUILD_LANDDEAL_VOTE, 
							"horizontal_align":"center", "text_horizontal_align":"center" },
						),
					},

					##Message1
					{
						"name" : "message1",
						"type" : "text",

						"x" : 20,
						"y" : 45,

						"text" : "",
						
						"horizontal_align" : "left",
						"text_horizontal_align" : "left",
						"text_vertical_align" : "left",
					},
					
					##Message2
					{
						"name" : "message2",
						"type" : "text",

						"x" : 20,
						"y" : 60,

						"text" : "",

						"horizontal_align" : "left",
						"text_horizontal_align" : "left",
						"text_vertical_align" : "left",
					},
					
					## 거래 길드
					{"name" : "DealGuildName", "type" : "text", "x" : 20, "y" : 83, "text" : uiScriptLocale.GUILD_LANDDEAL_VOTE_DEALGUILDNAME,	},
					
					## 거래 길드 이름 슬롯
					{
						"name" : "dealguildname_slot",
						"type" : "image",

						"x" : 100,
						"y" : 80,
						
						"image" : XLARGE_VALUE_FILE,

						"children" :
						(
							{
								"name" : "dealguildname_slot",
								"type" : "editline",

								"x" : 3,
								"y" : 0,

								"width" : 80,
								"height" : 18,

								"input_limit" : 18,
								"only_number" : 0,

								"text" : "",
							},
						),
					},
					
					##LeftButton
					{
						"name" : "Lbutton",
						"type" : "button",

						"x" : -40,
						"y" : 108,

						"width" : 61,
						"height" : 50,

						"horizontal_align" : "center",
						"text" : uiScriptLocale.OK,

						"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
						"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
						"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
					},
					
					##RightButton
					{
						"name" : "Rbutton",
						"type" : "button",
					    
						"x" : 40,
						"y" : 108,
					    
						"width" : 61,
						"height" : 50,
					    
						"horizontal_align" : "center",
						"text" : uiScriptLocale.CANCEL,

						"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
						"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
						"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",				    
					},
				),
			},
		),
	}

