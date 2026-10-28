import uiScriptLocale

window = {
	"name"		: "GameOptionWindow_Game_LeftSeatLogoutTime",
	
	"x"			: 0,
	"y"			: 0,

	"style"		: ("float",),
	
	"width"		: 210, 
	"height"	: 80,

	"children"	:
	[			
		{ 
			"name" : "left_seat_logout_30_min",	
			"type" : "button", 
			
			"x" : 0, 			
			"y" : 0,	
			
			"text" : uiScriptLocale.LEFT_SEAT_30_MIN, 
			
			"default_image"	: "d:/ymir work/ui/quest_re/button_top.sub", 
			"over_image"	: "d:/ymir work/ui/quest_re/button_top.sub", 
			"down_image"	: "d:/ymir work/ui/quest_re/button_top.sub", 
		},
		{ 
			"name" : "left_seat_logout_60_min",	
			"type" : "button", 
			
			"x" : 0, 
			"y" : 16,	
			
			"text" : uiScriptLocale.LEFT_SEAT_60_MIN, 
			
			"default_image" : "d:/ymir work/ui/quest_re/button_middle.sub",
			"over_image"	: "d:/ymir work/ui/quest_re/button_middle.sub",
			"down_image"	: "d:/ymir work/ui/quest_re/button_middle.sub", 
		},
		{ 
			"name" : "left_seat_logout_120_min",	
			"type" : "button", 
			
			"x" : 0, 
			"y" : 32,	
			
			"text" : uiScriptLocale.LEFT_SEAT_120_MIN, 
			
			"default_image" : "d:/ymir work/ui/quest_re/button_middle.sub",
			"over_image"	: "d:/ymir work/ui/quest_re/button_middle.sub",
			"down_image"	: "d:/ymir work/ui/quest_re/button_middle.sub", 
		},
		{ 
			"name" : "left_seat_logout_180_min",	
			"type" : "button", 
			
			"x" : 0, 
			"y" : 48,	
			
			"text" : uiScriptLocale.LEFT_SEAT_180_MIN, 
			
			"default_image" : "d:/ymir work/ui/quest_re/button_middle.sub",
			"over_image"	: "d:/ymir work/ui/quest_re/button_middle.sub",
			"down_image"	: "d:/ymir work/ui/quest_re/button_middle.sub", 
		},
		{ 
			"name" : "left_seat_logout_off",		
			"type" : "button", 
			
			"x" : 0, 
			"y" : 64,	
			
			"text" : uiScriptLocale.LEFT_SEAT_OFF, 
			
			"default_image" : "d:/ymir work/ui/quest_re/button_bottom.sub", 
			"over_image"	: "d:/ymir work/ui/quest_re/button_bottom.sub", 
			"down_image"	: "d:/ymir work/ui/quest_re/button_bottom.sub", 
		},
		{ 
			"name" : "left_seat_logout_list_mouse_over_image", 
			"type" : "expanded_image", 
			"style" : ("not_pick",), 

			"x" : 0, 
			"y" : 0,
			
			"image" : "d:/ymir work/ui/quest_re/button_over.sub", 
		},
	],
}