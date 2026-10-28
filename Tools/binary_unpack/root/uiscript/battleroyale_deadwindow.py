import uiScriptLocale

PUBLIC_PATH		= "d:/ymir work/ui/public/"

WINDOW_WIDTH	= 300
WINDOW_HEIGHT	= 100

window = {
	"name" : "BattleRoyaleDeadWindow",
	"style" : ("float", ),
	
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,
	
	"children" :
	[
		{
			"name"		: "board",
			"type"		: "thinboard",
			"x"			: 0,
			"y"			: 0,
			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,
			
			"children" :
			[	
				{
					"name"					: "dead_by_text",
					"type"					: "text",
					"x"						: 0,
					"y"						: 20,
					
					"horizontal_align"		: "center",
					"text_horizontal_align"	: "center",
					"vertical_align"		: "top",
					"text_vertical_align"	: "center",
					
					"text"					: "Dead by 1254123",
				},
				
				{
					"name"					: "personal_rank_text",
					"type"					: "text",
					"x"						: 0,
					"y"						: 40,
					
					"horizontal_align"		: "center",
					"text_horizontal_align"	: "center",
					"vertical_align"		: "top",
					"text_vertical_align"	: "center",
					
					"text"					: "Your Rank is #1",
				},
				
				{
					"name"					: "return_village_button",
					"type"					: "button",
					"x"						: 0,
					"y"						: 60,
					"text"					: uiScriptLocale.BATTLE_ROYALE_RETURN_TO_TOWN,
					
					"horizontal_align"		: "center",
					
					"default_image"	: PUBLIC_PATH + "XLarge_Button_01.sub",
					"over_image"	: PUBLIC_PATH + "XLarge_Button_02.sub",
					"down_image"	: PUBLIC_PATH + "XLarge_Button_03.sub",
				},
			],
		},
	],
}