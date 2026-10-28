import uiScriptLocale

TEXT_Y = 8
TEXT_GAP = 15

WIDTH_GAP = 6

THINBOARD_X = WIDTH_GAP*3 + 32
THINBOARD_Y = 50
THINBOARD_WIDTH = 192
THINBOARD_HEIGHT = (7*TEXT_GAP) + (2*TEXT_Y)

SIZE_X = 454
SIZE_Y = 205

window = {
	"name" : "SelectAttrWindow",
	"style" : ("movable", "float",),
	
	"x" : (SCREEN_WIDTH/2) - (SIZE_X/2),
	"y" : (SCREEN_HEIGHT/2) - (SIZE_Y/2),	

	"width" : SIZE_X,
	"height" : SIZE_Y,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : SIZE_X,
			"height" : SIZE_Y,

			"children" :
			(
				## SelectAttr Dialog Title
				{
					"name" : "SelectAttr_TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 6,
					"y" : 7,

					"width" : SIZE_X - 13,
					
					"children" :
					(
						{ "name" : "TitleName", "type" : "text", "x" : 0, "y" : 0, "text": uiScriptLocale.SELECT_ATTR_TITLE, "all_align":"center" },
					),
				},

				## Item Slot Area
				{
					"name" : "toolTipItem_Slot_Area",
					"type" : "window",
					"style" : ("attach",),
					
					"x" : WIDTH_GAP*2,
					"y" : THINBOARD_Y,
					
					"width" : 32, # 32 * 1
					"height" : 96, # 32 * 3
					"children" :
					(								
						## SkillBook Slot
						{
							"name" : "toolTipItemSlot",
							"type" : "grid_table",

							"x" : 0,
							"y" : 0,

							"start_index" : 0,
							"x_count" : 1,
							"y_count" : 3,
							"x_step" : 32,
							"y_step" : 32,

							"image" : "d:/ymir work/ui/public/Slot_Base.sub"
						},
					),						
					
				},	
	
				## ToolTip 
				{
					"name" : "left_black_board",
					"type" : "thinboard",
					"x" : THINBOARD_X,
					"y" : THINBOARD_Y,
					"width" : THINBOARD_WIDTH,
					"height" : THINBOARD_HEIGHT,
					
					"children" :
					(
						{	"name" : "lTitle", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : -17, "text" : uiScriptLocale.SELECT_ATTR_CURRENT, "text_horizontal_align" : "center", },
						## Left Text
						{	"name" : "lText0", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "lText1", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "lText2", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP * 2, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "lText3", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP * 3, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "lText4", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP * 4, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "lText5", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP * 5, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "lText6", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP * 6, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },

						## SelectButton0
						{
							"name" : "SelectBtn0",
							"type" : "radio_button",

							"x" : THINBOARD_WIDTH/2 - 30,
							"y" : THINBOARD_HEIGHT - 25,

							"text" : uiScriptLocale.SELECT_ATTR_SELECT,

							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
						},
					),
				},

				{
					"name" : "right_black_board",
					"type" : "thinboard",
					"x" : THINBOARD_X + THINBOARD_WIDTH + WIDTH_GAP,
					"y" : THINBOARD_Y,
					"width" : THINBOARD_WIDTH,
					"height" : THINBOARD_HEIGHT,
					
					"children" :
					(
						{	"name" : "rTitle", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : -17, "text" : uiScriptLocale.SELECT_ATTR_CHANGED, "text_horizontal_align" : "center", },
						## Left Text
						{	"name" : "rText0", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "rText1", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "rText2", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP * 2, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "rText3", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP * 3, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "rText4", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP * 4, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "rText5", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP * 5, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },
						{	"name" : "rText6", "type" : "text", "x" : THINBOARD_WIDTH/2, "y" : TEXT_Y + TEXT_GAP * 6, "text" : "Hello", "text_horizontal_align" : "center", "color" : 0xFF777777, },

						## SelectButton1
						{
							"name" : "SelectBtn1",
							"type" : "radio_button",

							"x" : THINBOARD_WIDTH/2 - 30,
							"y" : THINBOARD_HEIGHT - 25,

							"text" : uiScriptLocale.SELECT_ATTR_SELECT,

							"default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
							"over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
							"down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
						},				
					),
				},

				## Button
				{
					"name" : "accept_button",
					"type" : "button",

					"x" : SIZE_X - 100,
					"y" : SIZE_Y - 31,

					"text" : uiScriptLocale.OK,

					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
			),				
		},
	),
}
