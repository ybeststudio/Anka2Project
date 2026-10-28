# ENABLE_FLEXIBLE_WIDTH_BUTTON
import uiScriptLocale

MAINBOARD_WIDTH = 402
MAINBOARD_HEIGHT = 320
MAINBOARD_X = (SCREEN_WIDTH/2) - (MAINBOARD_WIDTH/2)
MAINBOARD_Y = (SCREEN_HEIGHT/2) - (MAINBOARD_HEIGHT/2)

LEFTBOARD_WIDTH = 174
LEFTBOARD_HEIGHT = 269
LEFTBOARD_X = 13
LEFTBOARD_Y = 36

RIGHTBOARD_WIDTH = 196
RIGHTBOARD_HEIGHT = 269
RIGHTBOARD_X = 192
RIGHTBOARD_Y = 36

window = {
	"name" : "MyShopDecoWindow",
	"style" : ("movable", "float",),
	
	"x" : MAINBOARD_X,
	"y" : MAINBOARD_Y,	

	"width" : MAINBOARD_WIDTH,
	"height" : MAINBOARD_HEIGHT,
	
	"children" :
	(
		## MainBoard
		{
			"name" : "MyShopDecoBoard",
			"type" : "board",
			"style" : ("attach","ltr"),

			"x" : 0,
			"y" : 0,

			"width" : MAINBOARD_WIDTH,
			"height" : MAINBOARD_HEIGHT,
			
			"children" :
			(
				## Title Bar
				{
					"name" : "MyShopTitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6, "y" : 7, "width" : MAINBOARD_WIDTH - 13,
					
					"children" :
					(
						{ "name" : "TitleName", "type" : "text", "x" : 0, "y" : 0, "text": uiScriptLocale.MYSHOP_DECO_SELECT_MODEL, "all_align":"center" },
					),
				},
				
				# Left Board
				{
					"name" : "LeftBoard",
					"type" : "thinboard_circle",
					"x" : LEFTBOARD_X, "y" : LEFTBOARD_Y, "width" : LEFTBOARD_WIDTH, "height" : LEFTBOARD_HEIGHT,
					
					"children" : 
					(
						{
							"name" : "ScrollBar",
							"type" : "scrollbar",

							"x" : 17,
							"y" : 6,
							"size" : LEFTBOARD_HEIGHT - 12,
							"horizontal_align" : "right",
						},
					),
				}, ## Left Board End
				
				## Right Board
				{
					"name" : "RightBoard",
					"type" : "thinboard_circle",
					"x" : RIGHTBOARD_X, "y" : RIGHTBOARD_Y, "width" : RIGHTBOARD_WIDTH, "height" : RIGHTBOARD_HEIGHT,
					
					"children" : 
					(
						## Title Bar
						{
							"name" : "ModelView",
							"type" : "image",
							
							"x" : 195 - RIGHTBOARD_X, "y" : 39 - RIGHTBOARD_Y,
							"image" : "d:/ymir work/ui/game/myshop_deco/model_view_title.sub",
							
							"children" :
							(
								{ "name" : "ModelName", "type" : "text", "x" : 0, "y" : 0, "text" : "123", "all_align":"center" },
							),
						},
										
						{
							"name" : "RenderTarget",
							"type" : "render_target",
									
							"x" : 195 - RIGHTBOARD_X,
							"y" : 62 - RIGHTBOARD_Y,
									
							"width" : 190,
							"height" : 210, #306
									
							"index" : 1,
						},
						
						## Button Next					
						{
							"name" : "NextButton",
							"type" : "button",

							"x" : 293 - RIGHTBOARD_X,
							"y" : 278 - RIGHTBOARD_Y,

							"text" : uiScriptLocale.MYSHOP_DECO_NEXT,

							"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
						},

						## Button Cancel
						{
							"name" : "CancelButton",
							"type" : "button",

							"x" : 201 - RIGHTBOARD_X,
							"y" : 278 - RIGHTBOARD_Y,

							"text" : uiScriptLocale.MYSHOP_DECO_CANCEL,

							"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
						},
						
						## Button Previous				
						{
							"name" : "PrevButton",
							"type" : "button",

							"x" : 201 - RIGHTBOARD_X,
							"y" : 278 - RIGHTBOARD_Y,

							"text" : uiScriptLocale.MYSHOP_DECO_PREV,

							"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
						},

						## Button Complete
						{
							"name" : "CompleteButton",
							"type" : "button",

							"x" : 293 - RIGHTBOARD_X,
							"y" : 278 - RIGHTBOARD_Y,

							"text" : uiScriptLocale.MYSHOP_DECO_OK,

							"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
						},
					),
				}, ## Right Board End
			),
		}, ## MainBoard End
	),
}
