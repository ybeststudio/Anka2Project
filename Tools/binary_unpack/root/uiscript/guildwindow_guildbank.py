import uiScriptLocale

BUTTON_ROOT = "d:/ymir work/ui/game/guild/guildbuttons/baseinfopage/"
PUBLIC_ROOT = "d:/ymir work/ui/public/"
window = {
	"name" : "GuildBankWindow",

	"x" : 100,
	"y" : 20,

	"style" : ("movable", "float",),

	"width" : 176,
	"height" : 225,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 176,
			"height" : 225,

			"children" :
			(
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8,
					"y" : 7,

					"width" : 161,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":77, "y":3, "text": uiScriptLocale.GUILD_BASEINFO_BANK, "text_horizontal_align":"center" },
					),
				},
				### Button
				#{
					#"name" : "BankIndex",
					#"type" : "button",

					#"x" : 10,
					#"y" : 52,

					#"text" : uiScriptLocale.GUILD_BANK_NUMBER,
					#"horizontal_align" : "left",
					#"vertical_align" : "bottom",

					#"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					#"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					#"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				#},			
				{
					"name" : "BankInfoButton",
					"type" : "button",

					"x" : 10,
					"y" : 30,

					#"text" : uiScriptLocale.GUILD_BANK_INFO,
					"horizontal_align" : "left",
					"vertical_align" : "bottom",
					"default_image" : BUTTON_ROOT+"GuildBankGoldInfoButton00.sub",
					"over_image" : BUTTON_ROOT+"GuildBankGoldInfoButton01.sub",
					"down_image" : BUTTON_ROOT+"GuildBankGoldInfoButton02.sub",
				},				
				{
					"name" : "ExitButton",
					"type" : "button",

					"x" : 48,
					"y" : 30,

					#"text" : uiScriptLocale.GUILD_BANK_CLOSE,
					"horizontal_align" : "center",
					"vertical_align" : "bottom",
					"default_image" : PUBLIC_ROOT+"CancleButton00.sub",
					"over_image" : PUBLIC_ROOT+"CancleButton01.sub",
					"down_image" : PUBLIC_ROOT+"CancleButton02.sub",
				},					
			),
		},
	),
}
