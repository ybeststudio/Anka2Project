import uiScriptLocale

SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
XLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_04.sub"

BUTTON_ROOT = "d:/ymir work/ui/game/guild/guildbuttons/declareselectpage/"
window = {
	"name" : "InputDialog",

	"x" : 0,
	"y" : 0,

	"style" : ("float",),

	"width" : 160,
	"height" : 120,

	"children" :
	(
		{
			"name" : "Mainboard",
			"type" : "board",
			
			"x" : 0,
			"y" : 0,
			
			"width" :160,
			"height" : 120,
			
			"children" :
			(
				## TitleBar
				{
					"name" : "Board",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 7,
					"y" : 7,

					"width" : 144,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":75, "y":4, "text":uiScriptLocale.GUILD_WAR_BATTLE_TYPE, "text_horizontal_align":"center" },
					),
				},
				{
					"name" : "King_Button",
					"type" : "button",
					"x" : 18,
					"y" : 35,
					#"text" : uiScriptLocale.GUILD_WAR_TYPE_NORMAL,
					"default_image" : BUTTON_ROOT+"King_Button00.sub",
					"over_image" : BUTTON_ROOT+"King_Button01.sub",
					"down_image" : BUTTON_ROOT+"King_Button02.sub",
				},
				{
					"name" : "Protect_Button",
					"type" : "button",
					"x" : 18,
					"y" : 35 + 25,
					#"text" : uiScriptLocale.GUILD_WAR_TYPE_FLAG,
					"default_image" : BUTTON_ROOT+"Protect_Button00.sub",
					"over_image" : BUTTON_ROOT+"Protect_Button01.sub",
					"down_image" : BUTTON_ROOT+"Protect_Button02.sub",
				},							
				{
					"name" : "Time_Button",
					"type" : "button",
					"x" : 18,
					"y" : 35 + 25 + 25,
					#"text" : uiScriptLocale.GUILD_WAR_TYPE_TIME,
					"default_image" : BUTTON_ROOT+"Time_Button00.sub",
					"over_image" : BUTTON_ROOT+"Time_Button01.sub",
					"down_image" : BUTTON_ROOT+"Time_Button02.sub",
				},
				{
					"name" : "Die_Button",
					"type" : "button",
					"x" : 18 + 65,
					"y" : 35,
					#"text" : uiScriptLocale.GUILD_WAR_TYPE_DIE,
					"default_image" : BUTTON_ROOT+"Die_Button00.sub",
					"over_image" : BUTTON_ROOT+"Die_Button01.sub",
					"down_image" : BUTTON_ROOT+"Die_Button02.sub",
				},
				{
					"name" : "Tiger_Button",
					"type" : "button",
					"x" : 18 + 65,
					"y" : 35 + 25,
					#"text" : uiScriptLocale.GUILD_WAR_TYPE_TIGER,
					"default_image" : BUTTON_ROOT+"Tiger_Button00.sub",
					"over_image" : BUTTON_ROOT+"Tiger_Button01.sub",
					"down_image" : BUTTON_ROOT+"Tiger_Button02.sub",
				},
				{
					"name" : "Defense_Button",
					"type" : "button",
					"x" : 18 + 65,
					"y" : 35 + 25 + 25,
					#"text" : uiScriptLocale.GUILD_WAR_TYPE_DEFENSE,
					"default_image" : BUTTON_ROOT+"Defense_Button00.sub",
					"over_image" : BUTTON_ROOT+"Defense_Button01.sub",
					"down_image" : BUTTON_ROOT+"Defense_Button02.sub",
				},
			),
		},
	),
}