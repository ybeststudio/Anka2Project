import localeInfo

window = {
	"name" : "MoveChannelDialog",
	"style" : ("movable", "float", "ltr"),
	
	"x" : (SCREEN_WIDTH/2) - (190/2),
	"y" : (SCREEN_HEIGHT/2) - 100,	

	"width" : 0,
	"height" : 0,
	
	"children" :
	(
		## MainBoard
		{
			"name" : "MoveChannelBoard",
			"type" : "board",
			"style" : ("attach", "ltr"),

			"x" : 0,
			"y" : 0,

			"width" : 0,
			"height" : 0,
			
			"children" :
			(
				## Title Bar
				{
					"name" : "MoveChannelTitle",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6, "y" : 7, "width" : 190 - 13,
					
					"children" :
					(
						{ "name" : "TitleName", "type" : "text", "x" : 0, "y" : 0, "text": localeInfo.MOVE_CHANNEL_TITLE, "all_align":"center" },
					),
				},
				
				{
					"name" : "BlackBoard",
					"type" : "thinboard_circle",
					"x" : 13, "y" : 36, "width" : 0, "height" : 0,
				},
				{
					"name" : "AcceptButton",
					"type" : "button",
					
					"x" : 15,
					"y" : 30,

					"vertical_align" : "bottom",
					
					"width" : 61,
					"height" : 21,

					"default_image" : "d:/ymir work/ui/public/acceptbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/acceptbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
					"disable_image" : "d:/ymir work/ui/public/acceptbutton02.sub",
				},
				{
					"name" : "CancelButton",
					"type" : "button",

					"x" : 115,
					"y" : 30,

					"vertical_align" : "bottom",

					"width" : 61,
					"height" : 21,

					"default_image" : "d:/ymir work/ui/public/cancelbutton00.sub",
					"over_image" : "d:/ymir work/ui/public/cancelbutton01.sub",
					"down_image" : "d:/ymir work/ui/public/cancelbutton02.sub",
					"disable_image" : "d:/ymir work/ui/public/cancelbutton02.sub",
				},
			),
		}, ## MainBoard End
	),
}