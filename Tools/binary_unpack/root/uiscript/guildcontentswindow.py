import uiScriptLocale
import localeInfo

PUBLIC_PATH					= "d:/ymir work/ui/public/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"
ROOT_PATH					= "d:/ymir work/ui/game/guild_battle/"

WINDOW_WIDTH				= 420
WINDOW_HEIGHT				= 90

if localeInfo.IsARABIC():
	POS_X					= 10
	POS_Y					= 43
else:
	POS_X					= 20
	POS_Y					= 43

window = {
	"name"		: "guild_contents_window",
	"type"		: "window",

	"x"			: POS_X,
	"y"			: POS_Y,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	[
		## background image
		{ 
			"name"			: "guild_contents_list_button", 
			"type"			: "button",
	
			"x"				: 0,
			"y"				: 0,

			"text"			: "",
			"default_image" : ROOT_PATH + "list_guild_battle_default.sub",
			"over_image"	: ROOT_PATH + "list_guild_battle_over.sub",
			"down_image"	: ROOT_PATH + "list_guild_battle_down.sub",

			"children" :
			[
				## title name
				{
					"name"					: "title_text",
					"type"					: "text",

					"x"						: 0,
					"y"						: 9,

					"horizontal_align"		: "center",
					"text_horizontal_align"	: "center",

					"text"					: "",
				},

				## requirements text
				{
					"name"					: "requirements_text",
					"type"					: "text",

					"x"						: -165,
					"y"						: 35,

					"horizontal_align"		: "center",
					"text_horizontal_align"	: "left",

					"text"					: "",
				},

				## remain start time
				{
					"name"					: "remain_start_text",
					"type"					: "text",

					"x"						: -133,
					"y"						: 9,

					"horizontal_align"		: "center",
					"text_horizontal_align"	: "center",

					"text"					: "",
				},
			]
		},
	]
}