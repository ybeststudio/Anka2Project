import uiScriptLocale

PUBLIC_PATH		= "d:/ymir work/ui/public/"

WINDOW_WIDTH	= 100
WINDOW_HEIGHT	= 60

window = {
	"name" : "BattleRoyaleKillInfo",
	"style" : ("float", ),
	
	"x" : 2,
	"y" : 64,
	
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
					"name"	: "kill_count_window",
					"type"	: "window",
					"x"		: 0,
					"y"		: 20,
					"width"	: WINDOW_WIDTH,
					"height" : 20,
					
					"children" :
					(
						{
							"name"					: "kill_count_text",
							"type"					: "text",
							"x"						: 0,
							"y"						: 0,
							
							"horizontal_align"		: "center", 
							"vertical_align"		: "top",
							"text_horizontal_align"	: "center",
							"text_vertical_align"	: "center",
							
							"text"					: "",
						},
					),
				
				},
				
				{
					"name"	: "alive_count_window",
					"type"	: "window",
					"x"		: 0,
					"y"		: 40,
					"width"	: WINDOW_WIDTH,
					"height" : 20,
				
					"children" :
					(
						{
							"name"					: "alive_count_text",
							"type"					: "text",
							"x"						: 0,
							"y"						: 0,
							
							"horizontal_align"		: "center", 
							"vertical_align"		: "top",
							"text_horizontal_align"	: "center",
							"text_vertical_align"	: "center",
							
							"text"					: "",
						},
					),
				},
			],
		},
	],
}