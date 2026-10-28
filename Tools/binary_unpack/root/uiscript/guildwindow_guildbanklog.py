import uiScriptLocale

BUTTON_ROOT = "d:/ymir work/ui/game/guild/guildbuttons/bankinfopage/"
ROOT_DIR = "d:/ymir work/ui/game/guild/guildbanklogpage/"
PLUS_WITDH = 130
window = {
	"name" : "GuildBanklogWindow",
	"style": ("movable","float"),

	"x" : 280,
	"y" : 20,

	"width"  : 305+PLUS_WITDH,
	"height" : 356,

	"children":
	(
		{
			"name" : "Board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 305+PLUS_WITDH,
			"height": 356,
			
			"children":
			(
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),
					"x" : 8,
					"y" : 7,
				
					"width" : 285+PLUS_WITDH,
					"color" : "yellow",
						"children" :
						(
							{ "name":"TitleName", "type":"text", "x": (305+PLUS_WITDH)/2, "y":3, "text":uiScriptLocale.GUILD_BANKINFO_TITLE, "text_horizontal_align":"center" },
						),
				},	
				## ScrollBar
				{
					"name" : "ScrollBar",
					"type" : "scrollbar",

					"x" : 280+PLUS_WITDH,
					"y" : 50,
					"scrollbar_type" : "normal",
					"size" : 260,
				},
				## GuildBankLogTItle
				{
					"name" : "GuildBankLogTItle", "type" : "image", "x" : 13, "y" : 39, "image" : ROOT_DIR+"GuildBankLogTItle.sub",
				},					
				## IndexName
				{
					"name" : "IndexName", "type" : "image", "x" : 49, "y" : 40, "image" : ROOT_DIR+"IndexName.sub",
				},
				## IndexItemGuildMoney
				{
					"name" : "IndexItemGuildMoney", "type" : "image", "x" : 188, "y" : 40, "image" : ROOT_DIR+"IndexItemGuildMoney.sub",
				},
				## IndexUseing
				{
					"name" : "IndexUseing", "type" : "image", "x" : 344, "y" : 40, "image" : ROOT_DIR+"IndexUseing.sub",
				},

				### TextLine
				#{
					#"name" : "IndexName", "type" : "text", "x" : 48, "y" : 40, "text" : uiScriptLocale.GUILD_BANKINFO_NAME,
				#},
				#{
					#"name" : "IndexGrade", "type" : "text", "x" : 100+PLUS_WITDH/2, "y" : 40, "text" : uiScriptLocale.GUILD_BANKINFO_TYPE,
				#},
				#{
					#"name" : "IndexJob", "type" : "text", "x" : 285+PLUS_WITDH/2, "y" : 40, "text" : uiScriptLocale.GUILD_BANKINFO_USETYPE,
				#},
				## Button
				{
					"name" : "ExitButton",
					"type" : "button",
					"x" : 200+PLUS_WITDH,
					"y" : 325,
					"tooltip_text" : uiScriptLocale.GUILD_BANKINFO_CLOSE,
					"default_image" : BUTTON_ROOT+"ExitButton00.sub",
					"over_image" : BUTTON_ROOT+"ExitButton01.sub",
					"down_image" : BUTTON_ROOT+"ExitButton02.sub",
				},
			),
		},
	),
}
