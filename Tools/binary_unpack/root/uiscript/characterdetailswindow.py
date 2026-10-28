import uiScriptLocale

LABEL_WIDTH = 50
LABEL_HEIGHT = 22
MAINBOARD_WIDTH = 247
MAINBOARD_HEIGHT = 379
LABEL_START_X = 186+8+4
LABEL_GAP = LABEL_HEIGHT+7

IMG_DIR = "characterdetails/"

window = {
	"name" : "CharacterDetailsWindow",
	"style" : ("float",),
	"x" : 274,
	"y" : (SCREEN_HEIGHT - 398) / 2,
	"width" : MAINBOARD_WIDTH,
	"height" : MAINBOARD_HEIGHT,
	"children" :
	(
		{
			"name" : "MainBoard",
			"type" : "board",
			"x" : 0,
			"y" : 0,
			"width" : MAINBOARD_WIDTH,
			"height" : MAINBOARD_HEIGHT,
			"children" :
			(
				{
					"name" : "bonus_button",
					"type" : "radio_button",
					"x" : 19,
					"y" : 9,
					"text" : "Bonus Tablosu",
					"default_image" :IMG_DIR+"btn_0.png",
					"over_image" :IMG_DIR+"btn_0_hover.png",
					"down_image" :IMG_DIR+"btn_1.png",
				},
				{
					"name" : "stat_button",
					"type" : "radio_button",
					"x" : 126,
					"y" : 9,
					"text" : uiScriptLocale.DETAILS_ISTAISTIK,
					"default_image" :IMG_DIR+"btn_0.png",
					"over_image" :IMG_DIR+"btn_0_hover.png",
					"down_image" :IMG_DIR+"btn_1.png",
				},
				{
					"name" : "bonus_window",
					"type" : "image",
					"x" : 10,
					"y" : 10+25,
					"image" :IMG_DIR+"bonus_bg.png",
				},

				{
					"name" : "stat_window",
					"type" : "image",
					"x" : 10,
					"y" : 10+25,
					"image" :IMG_DIR+"log_bg.png",
					"children" :
					(
						{
							"name" : "search2",
							"type" : "button",
							"x" : 151,
							"y" : 13,
							"default_image" : "characterdetails/btn_search_1.png",
							"over_image" : "characterdetails/btn_search_2.png",
							"down_image" : "characterdetails/btn_search_3.png",
						},
						{
							"name" : "QueryString",
							"type" : "editline",
							"x" : 54,
							"y" : 16,
							"width" : 100,
							"height" : 10,
							"input_limit" : 10,
						},
					),
				},

				{
					"name" : "stones_kills_bg",
					"type" : "text",
					"x":LABEL_START_X,
					"y":58+LABEL_GAP*2,
					"width" : LABEL_WIDTH,
					"height" : LABEL_HEIGHT,
					
					"children" :
					( 
						{
						"name" : "stones_kills",
						"type" : "text",
						"x" : 0,
						"y" : 0,
						"text" : "0",
						#"fontname" : "Tahoma:13",
						"all_align" : "center",
						},
					),
				},
				{
					"name" : "bosses_kills_bg",
					"type" : "text",
					"x":LABEL_START_X,
					"y":58+LABEL_GAP*3,
					"width" : LABEL_WIDTH,
					"height" : LABEL_HEIGHT,
					
					"children" :
					( 
						{
						"name" : "bosses_kills",
						"type" : "text",
						"x" : 0,
						"y" : 0,
						"text" : "0",
						#"fontname" : "Tahoma:13",
						"all_align" : "center",
						},
					),
				},
				{
					"name" : "top_damages_bg",
					"type" : "text",
					"x":138+14,
					"y":58+LABEL_GAP*4,
					"width" : LABEL_WIDTH,
					"height" : LABEL_HEIGHT,
					
					"children" :
					( 
						{
						"name" : "top_damages",
						"type" : "text",
						"x" : 0,
						"y" : 0,
						"text" : "0",
						#"fontname" : "Tahoma:13",
						"all_align" : "center",
						},
					),
				},
			),
		},
	),
}
