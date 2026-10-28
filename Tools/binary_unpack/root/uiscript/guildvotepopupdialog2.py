import uiScriptLocale

window = {
	"name" : "GuildPopupDialog2",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH/2 - 250,
	"y" : SCREEN_HEIGHT/2 - 40,

	"width" : 280,
	"height" : 135,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",

			"x" : 0,
			"y" : 0,

			"width" : 280,
			"height" : 135,

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
						"text" : uiScriptLocale.GAMEOPTION_TITLE, 
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
				##Message3
				{
					"name" : "message3",
					"type" : "text",

					"x" : 20,
					"y" : 75,

					"text" : "",

					"horizontal_align" : "left",
					"text_horizontal_align" : "left",
					"text_vertical_align" : "left",
				},				

				##LeftButton
				{
					"name" : "Lbutton",
					"type" : "button",

					"x" : -40,
					"y" : 101,

					"width" : 61,
					"height" : 50,

					"horizontal_align" : "center",
					"text" : uiScriptLocale.ACCEPT,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
				},
				
				##RightButton
				{
				    "name" : "Rbutton",
				    "type" : "button",
				    
				    "x" : 40,
				    "y" : 101,
				    
				    "width" : 61,
				    "height" : 50,
				    
					"horizontal_align" : "center",
					"text" : uiScriptLocale.OPPOSITES,

					"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",				    
				},
			),
		},
	),
}

