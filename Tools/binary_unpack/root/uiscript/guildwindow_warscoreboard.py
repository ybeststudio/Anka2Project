import uiScriptLocale

SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
XLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_04.sub"
GUILD_SCORE_ON = "d:/ymir work/ui/public/guild_score_button_on.sub"
GUILD_SCORE_OFF = "d:/ymir work/ui/public/guild_score_button_off.sub"
GUILD_SCORE_VS = "d:/ymir work/ui/public/guild_score_VS.sub"

window = {
	"name" : "InputDialog",

	"x" : 30,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : 475,
	"height" : 50,

	"children" :
	(
		{
			"name" : "Mainboard",
			"type" : "board",
			
			"x" : 0,
			"y" : 0,
			
			"width" :475,
			"height" : 50,
			
			"children" :
			(
				## GuildName    
				{ "name":"VS", "type":"image", "x":220, "y":16, "width" : 42, "height" :22, "image" : GUILD_SCORE_VS, },
				{ "name":"Guild1Score", "type":"text", "x":58, "y":40, "text":"0" },
				{ "name":"Guild2Score", "type":"text", "x":410, "y":40, "text":"0" },
				### Guild1NameMark
				{
					"name" : "Guild1NameMark", "x" : 140, "y" : 20, "width" : 90, "height" : 18,
					"children" :
					(
						{
							"name" : "Guild1Mark", "type" : "mark", "x" : -20, "y" : 0,
							"children" :
							(
								{ "name":"GuildName1", "type":"text", "x":20, "y":0, "text":"", },
							),
						},
					),
				},
				## Guild2NameMark
				{
					"name" : "Guild2NameMark", "x" : 280, "y" : 20, "width" : 90, "height" : 18,
					"children" :
					(
						{
							"name" : "Guild2Mark", "type" : "mark", "x" : -20, "y" : 0,
							"children" :
							(
								{ "name":"GuildName2", "type":"text", "x":20, "y":0, "text":"", },
							),
						},
					),
				},
				## SetButtonGuild1
				{
					"name" : "Guild1SetButton1",
					"type" : "radio_button",
					"x" : 10,
					"y" : 20,
					"default_image" : GUILD_SCORE_ON,
					"over_image" : GUILD_SCORE_ON,
					"down_image" : GUILD_SCORE_OFF,
				},
				{
					"name" : "Guild1SetButton2",
					"type" : "radio_button",
					"x" : 32,
					"y" : 20,
					"default_image" : GUILD_SCORE_ON,
					"over_image" : GUILD_SCORE_ON,
					"down_image" : GUILD_SCORE_OFF,
				},
				{
					"name" : "Guild1SetButton3",
					"type" : "radio_button",
					"x" : 54,
					"y" : 20,
					"default_image" : GUILD_SCORE_ON,
					"over_image" : GUILD_SCORE_ON,
					"down_image" : GUILD_SCORE_OFF,
				},
				{
					"name" : "Guild1SetButton4",
					"type" : "radio_button",
					"x" : 76,
					"y" : 20,
					"default_image" : GUILD_SCORE_ON,
					"over_image" : GUILD_SCORE_ON,
					"down_image" : GUILD_SCORE_OFF,
				},
				{
					"name" : "Guild1SetButton5",
					"type" : "radio_button",
					"x" : 98,
					"y" : 20,
					"default_image" : GUILD_SCORE_ON,
					"over_image" : GUILD_SCORE_ON,
					"down_image" : GUILD_SCORE_OFF,
				},
				## SetButtonGuild2
				{
					"name" : "Guild2SetButton1",
					"type" : "radio_button",
					"x" : 360,
					"y" : 20,
					"default_image" : GUILD_SCORE_ON,
					"over_image" : GUILD_SCORE_ON,
					"down_image" : GUILD_SCORE_OFF,
				},
				{
					"name" : "Guild2SetButton2",
					"type" : "radio_button",
					"x" : 382,
					"y" : 20,
					"default_image" : GUILD_SCORE_ON,
					"over_image" : GUILD_SCORE_ON,
					"down_image" : GUILD_SCORE_OFF,
				},
				{
					"name" : "Guild2SetButton3",
					"type" : "radio_button",
					"x" : 404,
					"y" : 20,
					"default_image" : GUILD_SCORE_ON,
					"over_image" : GUILD_SCORE_ON,
					"down_image" : GUILD_SCORE_OFF,
				},
				{
					"name" : "Guild2SetButton4",
					"type" : "radio_button",
					"x" : 426,
					"y" : 20,
					"default_image" : GUILD_SCORE_ON,
					"over_image" : GUILD_SCORE_ON,
					"down_image" : GUILD_SCORE_OFF,
				},
				{
					"name" : "Guild2SetButton5",
					"type" : "radio_button",
					"x" : 448,
					"y" : 20,
					"default_image" : GUILD_SCORE_ON,
					"over_image" : GUILD_SCORE_ON,
					"down_image" : GUILD_SCORE_OFF,
				},				
			),
		},
	),
}
