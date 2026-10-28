if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale
import localeInfo

LOCALE_PATH = uiScriptLocale.WINDOWS_PATH
ROOT_PATH = "d:/ymir work/ui/game/windows/"
ROOT = "d:/ymir work/ui/game/"
PET_UI_ROOT = "d:/ymir work/ui/pet/"
SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_00.sub"
MIDDLE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
XLARGE_BUTTON_FILE = "d:/ymir work/ui/public/xlarge_button_03.sub"
BASE_SLOT_FILE = "d:/ymir work/ui/public/Slot_Base.sub"

PET_UI_BG_IMAGE = "d:/ymir work/ui/pet/res/Pet_UI_bg.tga"
if app.ENABLE_UPBRINGING_PET_CONQUEROR_LEVEL:
	PET_UI_ITEM_SLOT_Y = 49 - 9
else:
	PET_UI_ITEM_SLOT_Y = 49
PET_UI_BG_HEIGHT	= 456	# 408 + 48 = 456

PET_UI_BG_WIDTH		= 352


PET_PAGE_UI_BG_WIDTH	= 352
PET_PAGE_UI_BG_HEIGHT	= 408

PET_PRIMIUM_FEEFSTUFF_UI_BG_IMAGE = "d:/ymir work/ui/pet/res/Pet_UI_bg3.tga"

## Evol Name, Pet Name, Pet Abilities
LONG_LABEL_WIDTH	= 266
LONG_LABEL_HEIGHT	= 19

## Level, exp, age, life
SHORT_LABLE_WIDTH	= 90
SHORT_LABLE_HEIGHT	= 20

## Defence, Magic Defence, HP
MIDDLE_LABLE_WIDTH	= 168
MIDDLE_LABLE_HEIGHT	= 20

## EXP Gague interval
EXP_GAGUE_INTERVAL	= 2
EXP_IMG_WIDTH		= 16
EXP_IMG_HEIGHT		= 16

## TEXT COLOR
GOLD_COLOR	= 0xFFFEE3AE
WHITE_COLOR = 0xFFFFFFFF

if app.ENABLE_UPBRINGING_PET_CONQUEROR_LEVEL:
	window = {
		"name" : "PetInformationWindow",
		"style" : ("movable", "float",),
		
		"x" : SCREEN_WIDTH - 176 -200 -146 -145,	## inven x ???? ? ???.
		"y" : SCREEN_HEIGHT - 37 - 565,				## inven y ??? ??.
	
		"width" : PET_UI_BG_WIDTH,
		"height" : PET_UI_BG_HEIGHT,
	
		"children" :
		[
			{
				"name" : "board",
				"type" : "window",
				"style" : ("attach",),
	
				"x" : 0,
				"y" : 0,
	
				"width" : PET_UI_BG_WIDTH,
				"height" : PET_UI_BG_HEIGHT,
				
				"children" :
				[
					## Page Area
					{
						"name" : "PetInfo_Page",
						"type" : "window",
						"style" : ("attach",),
						
						"x" : 0,
						"y" : 0,
						
						"width" : PET_PAGE_UI_BG_WIDTH,
						"height" : PET_PAGE_UI_BG_HEIGHT,
						
						"children":
						[
							## Pet Info Page UI BG
							{ "name" : "PetInfoUIBG", "type" : "expanded_image", "style" : ("attach",), "x" : 0, "y" : 0, "image" : PET_UI_BG_IMAGE },
						
							## UpBringing Pet Item Slot
							{
								"name" : "UpBringing_Pet_Slot",
								"type" : "slot",
								"x" : 25,
								"y" : PET_UI_ITEM_SLOT_Y,
								"width" : 32,
								"height" : 32,
						
								"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":32},),
							},
					
							## Normal Evol Name + Special Evol Name
							{ 
								"name" : "EvolNameWindow", "type" : "window", "x" : 65, "y" : 50 - 3, "width" : LONG_LABEL_WIDTH, "height" : LONG_LABEL_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"EvolName", "type":"text", "x":0, "y":0, "text": "Lv. 3 (Special Evolution)", "r":1.0, "g":0.0, "b":0.0, "a":1.0, "all_align" : "center"},
								),	
							},
					
							## Pet Name
							{ 
								"name" : "NameWindow", "type" : "window", "x" : 65, "y" : 75 - 3, "width" : LONG_LABEL_WIDTH, "height" : LONG_LABEL_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"PetName", "type":"text", "x":0, "y":0, "text": "Super Baby White Dragon", "r":0.83, "g":0.77, "b":0.73, "a":1.0, "all_align" : "center"},
								),	
							},
	
							## normal info button
							{
								"name" : "NormalInfoButton",
								"type" : "button",
							
								"x" : 14,
								"y" : 109,
							
								"default_image" : PET_UI_ROOT + "conqueror/normal_bt_default.sub",
								"over_image" : PET_UI_ROOT + "conqueror/normal_bt_over.sub",
								"down_image" : PET_UI_ROOT + "conqueror/normal_bt_down.sub",
							},
	
							## conqueror info button
							{
								"name" : "ConquerorInfoButton",
								"type" : "button",
	
								"x" : 122,
								"y" : 109,
	
								"default_image" : PET_UI_ROOT + "conqueror/conqueror_bt_default.sub",
								"over_image" : PET_UI_ROOT + "conqueror/conqueror_bt_over.sub",
								"down_image" : PET_UI_ROOT + "conqueror/conqueror_bt_down.sub",
							},
	
							## potential button
							{
								"name" : "PotentialButton",
								"type" : "button",
	
								"x" : 230,
								"y" : 109,
	
								"default_image" : PET_UI_ROOT + "conqueror/potential_bt_default.sub",
								"over_image" : PET_UI_ROOT + "conqueror/potential_bt_over.sub",
								"down_image" : PET_UI_ROOT + "conqueror/potential_bt_down.sub",
							},
					
							## Level Title
							{ 
								"name" : "LevelWindow", "type" : "window", "x" : 28, "y" : 124 + 11, "width" : SHORT_LABLE_WIDTH, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"LevelTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_LEVEL, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
							## Level Value
							{ 
								"name" : "LevelValueWindow", "type" : "window", "x" : 28, "y" : 124+SHORT_LABLE_HEIGHT+3 + 11, "width" : SHORT_LABLE_WIDTH, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"LevelValue", "type":"text", "x":0, "y":0, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),	
							},
							## EXP Title
							{ 
								"name" : "ExpWindow", "type" : "window", "x" : 131, "y" : 124 + 11, "width" : SHORT_LABLE_WIDTH, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"ExpTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_EXP, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
							## EXP Gauge
							{
								"name" : "UpBringing_Pet_EXP_Gauge_Board",
								"type" : "window",
								"style": ("ltr",),
		
								"x" : 131,
								"y" : 149 + 11,
					
								"width"		: EXP_IMG_WIDTH * 5 + EXP_GAGUE_INTERVAL * 4,
								"height"	: EXP_IMG_HEIGHT,
	
								"children" :
								(
									{
										"name" : "UpBringing_Pet_EXPGauge_01",
										"type" : "expanded_image",
	
										"x" : 0,
										"y" : 0,
	
										"image" : PET_UI_ROOT + "exp_gauge/exp_on.sub",
									},
									{
										"name" : "UpBringing_Pet_EXPGauge_02",
										"type" : "expanded_image",
	
										"x" : EXP_IMG_WIDTH + EXP_GAGUE_INTERVAL,
										"y" : 0,
	
										"image" : PET_UI_ROOT + "exp_gauge/exp_on.sub",
									},
									{
										"name" : "UpBringing_Pet_EXPGauge_03",
										"type" : "expanded_image",
	
										"x" : EXP_IMG_WIDTH * 2 + EXP_GAGUE_INTERVAL * 2,
										"y" : 0,
	
										"image" : PET_UI_ROOT + "exp_gauge/exp_on.sub",
									},
									{
										"name" : "UpBringing_Pet_EXPGauge_04",
										"type" : "expanded_image",
	
										"x" : EXP_IMG_WIDTH * 3 + EXP_GAGUE_INTERVAL * 3,
										"y" : 0,
	
										"image" : PET_UI_ROOT + "exp_gauge/exp_on.sub",
									},
									{
										"name" : "UpBringing_Pet_EXPGauge_05",
										"type" : "expanded_image",
	
										"x" : EXP_IMG_WIDTH * 4 + EXP_GAGUE_INTERVAL * 4,
										"y" : 0,
	
										"image" : PET_UI_ROOT + "exp_gauge/item_exp_on.sub",
									},
								),
							}, #End of EXP
					
							## AGE Title
							{ 
								"name" : "AgeWindow", "type" : "window", "x" : 232, "y" : 124 + 11, "width" : SHORT_LABLE_WIDTH, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"AgeTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_AGE, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
							## AGE Value
							{ 
								"name" : "AgeValueWindow", "type" : "window", "x" : 232, "y" : 124+SHORT_LABLE_HEIGHT+3 + 11, "width" : SHORT_LABLE_WIDTH, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"AgeValue", "type":"text", "x":0, "y":0, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),	
							},
					
							## LIFE Title
							{ 
								"name" : "LifeWindow", "type" : "window", "x" : 28, "y" : 169 + 11, "width" : 168, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"LifeTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_LIFE, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
					
							## LIFE Value
							{ 
								"name" : "LifeValueWindow", "type" : "window", "x" : 28, "y" : 192 + 11, "width" : 190, "height" : 20, "style" : ("attach",),
								"children" :
								(
									{"name":"LifeTextValue", "type":"text", "x":0, "y":2, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),	
							},
					
							## LIFE Time Gauge Bar
							{
								"name" : "LifeGaugeWindow", "type" : "window", "x" : 36, "y":216+ 11, "width" : 179, "height" : 12, "style" : ("attach",),
								"children" :
								(
									{
										"name" : "LifeGauge",
										"type" : "ani_image",
	
										"x" : 0,
										"y" : 0,
	
										"delay" : 6,
	
										"images" :
										(
											"D:/Ymir Work/UI/Pattern/HPGauge/01.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/02.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/03.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/04.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/05.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/06.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/07.tga",
										),
									},
								),
							},
					
							## UpBringing Pet Feed Button(Life Time Increase)
							{
								"name" : "FeedLifeTimeButton",
								"type" : "toggle_button",
	
								"x" : 207,
								"y" : 173 + 11,
	
								"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
								"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
								"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
						
								"text" : uiScriptLocale.PET_INFORMATION_LIFETIME_BUTTON,
								"text_color" : GOLD_COLOR,
							},
					
							## UpBringing Pet Feed Button(evolution)
							{
								"name" : "FeedEvolButton",
								"type" : "toggle_button",
	
								"x" : 207,
								"y" : 194 + 11,
	
								"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
								"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
								"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
							
								"text" : uiScriptLocale.PET_INFORMATION_EVOL_BUTTON,
								"text_color" : GOLD_COLOR,
							},
					
							## UpBringing Pet Feed Button(EXP)
							{
								"name" : "FeedExpButton",
								"type" : "toggle_button",
	
								"x" : 207,
								"y" : 215 + 11,
	
								"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
								"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
								"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
						
								"text" : uiScriptLocale.PET_INFORMATION_EXP_BUTTON,
								"text_color" : GOLD_COLOR,
							},
					
							## Pet Abilities
							{ 
								"name" : "AbilitiesWindow", "type" : "window", "x" : 43, "y" : 259 + 2, "width" : LONG_LABEL_WIDTH, "height" : LONG_LABEL_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"AbilitiesName", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_ABILITIES, "r":1.0, "g":0.0, "b":0.0, "a":1.0, "all_align" : "center"},
								),	
							},
					
							## HP Title
							{ 
								"name" : "HpWindow", "type" : "window", "x" : 20, "y" : 282 + 2, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"HpTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_HP, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
							## HP Value
							{ 
								"name" : "HpValueWindow", "type" : "window", "x" : 20 + MIDDLE_LABLE_WIDTH, "y" : 282 + 2, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"HpValue", "type":"text", "x":0, "y":0, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),	
							},
					
							## Defence Title
							{ 
								"name" : "DefWindow", "type" : "window", "x" : 20, "y" : 304 + 2, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"DefTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_DEFENCE, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
							## Defence Value
							{ 
								"name" : "DefValueWindow", "type" : "window", "x" : 20 + MIDDLE_LABLE_WIDTH, "y" : 304 + 2, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"DefValue", "type":"text", "x":0, "y":0, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),	
							},
					
							## SP Title
							{ 
								"name" : "SpWindow", "type" : "window", "x" : 20, "y" : 326 + 2, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"SpTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_SP, "color":GOLD_COLOR, "all_align" : "center"},
								),
							},
							## SP Value
							{ 
								"name" : "SpValueWindow", "type" : "window", "x" : 20 + MIDDLE_LABLE_WIDTH, "y" : 326 + 2, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"SpValue", "type":"text", "x":0, "y":0, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),
							},
						
							## Pet Skill Menu Icon
							{
								"name" : "PetSkillMenuIcon",
								"type" : "image",
								
								"x" : 20,
								"y" : 362,
								
								"width" : 40,			
								"height" : 40,
								
								"image" : PET_UI_ROOT + "conqueror/skill_menu_icon_bg.sub",
							},
					
							## Pet Skill Slot 0
							{
								"name"			: "PetSkillSlot0",
								"type"			: "grid_table",
								"x"				: 65,
								"y"				: 365,
								"start_index"	: 0,
								"x_count"		: 1,
								"y_count"		: 1,
								"x_step"		: 32,
								"y_step"		: 32,
							},
					
							## Pet Skill Slot 1
							{
								"name"			: "PetSkillSlot1",
								"type"			: "grid_table",
								"x"				: 101,
								"y"				: 365,
								"start_index"	: 0,
								"x_count"		: 1,
								"y_count"		: 1,
								"x_step"		: 32,
								"y_step"		: 32,
							},
					
							## Pet Skill Slot 2
							{
								"name"			: "PetSkillSlot2",
								"type"			: "grid_table",
								"x"				: 137,
								"y"				: 365,
								"start_index"	: 0,
								"x_count"		: 1,
								"y_count"		: 1,
								"x_step"		: 32,
								"y_step"		: 32,
							},
							
							## Potential Menu Icon
							{
								"name" : "PetPotentialMenuIcon",
								"type" : "image",
								
								"x" : 180,
								"y" : 362,
								
								"width" : 40,			
								"height" : 40,
								
								"image" : PET_UI_ROOT + "conqueror/potential_menu_icon_bg.sub",
							},

							# Pet Potential Slot 0
							{
								"name" : "potential_skill_slot_bg_0",
								"type" : "image",

								"x" : 225,
								"y" : 365,
										
								"image"		: BASE_SLOT_FILE,

								"children" :
								(
									{
										"name"			: "potential_pet_slot_0",
										"type"			: "grid_table",
										"x"				: 0,
										"y"				: 0,
										"start_index"	: 0,
										"x_count"		: 1,
										"y_count"		: 1,
										"x_step"		: 32,
										"y_step"		: 32,
									},
								),
							},
	
							# Pet Potential Slot 1
							{
								"name" : "potential_skill_slot_bg_1",
								"type" : "image",

								"x" : 261,
								"y" : 365,
										
								"image"		: BASE_SLOT_FILE,

								"children" :
								(
									{
										"name"			: "potential_pet_slot_1",
										"type"			: "grid_table",
										"x"				: 0,
										"y"				: 0,
										"start_index"	: 0,
										"x_count"		: 1,
										"y_count"		: 1,
										"x_step"		: 32,
										"y_step"		: 32,
									},
								),
							},
	
						], ## End of board window children			
					},
				],
			},
		],
	}
else:
	window = {
		"name" : "PetInformationWindow",
		"style" : ("movable", "float",),
		
		"x" : SCREEN_WIDTH - 176 -200 -146 -145,	## inven x ???? ? ???.
		"y" : SCREEN_HEIGHT - 37 - 565,				## inven y ??? ??.
	
		"width" : PET_UI_BG_WIDTH,
		"height" : PET_UI_BG_HEIGHT,
	
		"children" :
		[
			{
				"name" : "board",
				"type" : "window",
				"style" : ("attach",),
	
				"x" : 0,
				"y" : 0,
	
				"width" : PET_UI_BG_WIDTH,
				"height" : PET_UI_BG_HEIGHT,
				
				"children" :
				[
					## Page Area
					{
						"name" : "PetInfo_Page",
						"type" : "window",
						"style" : ("attach",),
						
						"x" : 0,
						"y" : 0,
						
						"width" : PET_PAGE_UI_BG_WIDTH,
						"height" : PET_PAGE_UI_BG_HEIGHT,
						
						"children":
						[
							## Pet Info Page UI BG
							{ "name" : "PetInfoUIBG", "type" : "expanded_image", "style" : ("attach",), "x" : 0, "y" : 0, "image" : PET_UI_BG_IMAGE },
						
							## UpBringing Pet Item Slot
							{
								"name" : "UpBringing_Pet_Slot",
								"type" : "slot",
								"x" : 25,
								"y" : PET_UI_ITEM_SLOT_Y,
								"width" : 32,
								"height" : 32,
						
								"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":32},),
							},
					
							## Normal Evol Name + Special Evol Name
							{ 
								"name" : "EvolNameWindow", "type" : "window", "x" : 65, "y" : 50, "width" : LONG_LABEL_WIDTH, "height" : LONG_LABEL_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"EvolName", "type":"text", "x":0, "y":0, "text": "Lv. 3 (Special Evolution)", "r":1.0, "g":0.0, "b":0.0, "a":1.0, "all_align" : "center"},
								),	
							},
					
							## Pet Name
							{ 
								"name" : "NameWindow", "type" : "window", "x" : 65, "y" : 75, "width" : LONG_LABEL_WIDTH, "height" : LONG_LABEL_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"PetName", "type":"text", "x":0, "y":0, "text": "Super Baby White Dragon", "r":0.83, "g":0.77, "b":0.73, "a":1.0, "all_align" : "center"},
								),	
							},
					
							## Level Title
							{ 
								"name" : "LevelWindow", "type" : "window", "x" : 28, "y" : 124, "width" : SHORT_LABLE_WIDTH, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"LevelTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_LEVEL, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
							## Level Value
							{ 
								"name" : "LevelValueWindow", "type" : "window", "x" : 28, "y" : 124+SHORT_LABLE_HEIGHT+3, "width" : SHORT_LABLE_WIDTH, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"LevelValue", "type":"text", "x":0, "y":0, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),	
							},
							## EXP Title
							{ 
								"name" : "ExpWindow", "type" : "window", "x" : 131, "y" : 124, "width" : SHORT_LABLE_WIDTH, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"ExpTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_EXP, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
							## EXP Gauge
							{
								"name" : "UpBringing_Pet_EXP_Gauge_Board",
								"type" : "window",
								"style": ("ltr",),
		
								"x" : 131,
								"y" : 149,
					
								"width"		: EXP_IMG_WIDTH * 5 + EXP_GAGUE_INTERVAL * 4,
								"height"	: EXP_IMG_HEIGHT,
	
								"children" :
								(
									{
										"name" : "UpBringing_Pet_EXPGauge_01",
										"type" : "expanded_image",
	
										"x" : 0,
										"y" : 0,
	
										"image" : PET_UI_ROOT + "exp_gauge/exp_on.sub",
									},
									{
										"name" : "UpBringing_Pet_EXPGauge_02",
										"type" : "expanded_image",
	
										"x" : EXP_IMG_WIDTH + EXP_GAGUE_INTERVAL,
										"y" : 0,
	
										"image" : PET_UI_ROOT + "exp_gauge/exp_on.sub",
									},
									{
										"name" : "UpBringing_Pet_EXPGauge_03",
										"type" : "expanded_image",
	
										"x" : EXP_IMG_WIDTH * 2 + EXP_GAGUE_INTERVAL * 2,
										"y" : 0,
	
										"image" : PET_UI_ROOT + "exp_gauge/exp_on.sub",
									},
									{
										"name" : "UpBringing_Pet_EXPGauge_04",
										"type" : "expanded_image",
	
										"x" : EXP_IMG_WIDTH * 3 + EXP_GAGUE_INTERVAL * 3,
										"y" : 0,
	
										"image" : PET_UI_ROOT + "exp_gauge/exp_on.sub",
									},
									{
										"name" : "UpBringing_Pet_EXPGauge_05",
										"type" : "expanded_image",
	
										"x" : EXP_IMG_WIDTH * 4 + EXP_GAGUE_INTERVAL * 4,
										"y" : 0,
	
										"image" : PET_UI_ROOT + "exp_gauge/item_exp_on.sub",
									},
								),
							}, #End of EXP
					
							## AGE Title
							{ 
								"name" : "AgeWindow", "type" : "window", "x" : 232, "y" : 124, "width" : SHORT_LABLE_WIDTH, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"AgeTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_AGE, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
							## AGE Value
							{ 
								"name" : "AgeValueWindow", "type" : "window", "x" : 232, "y" : 124+SHORT_LABLE_HEIGHT+3, "width" : SHORT_LABLE_WIDTH, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"AgeValue", "type":"text", "x":0, "y":0, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),	
							},
					
							## LIFE Title
							{ 
								"name" : "LifeWindow", "type" : "window", "x" : 28, "y" : 169, "width" : 168, "height" : SHORT_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"LifeTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_LIFE, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
					
							## LIFE Value
							{ 
								"name" : "LifeValueWindow", "type" : "window", "x" : 28, "y" : 192, "width" : 190, "height" : 20, "style" : ("attach",),
								"children" :
								(
									{"name":"LifeTextValue", "type":"text", "x":0, "y":2, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),	
							},
					
							## LIFE Time Gauge Bar
							{
								"name" : "LifeGaugeWindow", "type" : "window", "x" : 36, "y":216, "width" : 179, "height" : 12, "style" : ("attach",),
								"children" :
								(
									{
										"name" : "LifeGauge",
										"type" : "ani_image",
	
										"x" : 0,
										"y" : 0,
	
										"delay" : 6,
	
										"images" :
										(
											"D:/Ymir Work/UI/Pattern/HPGauge/01.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/02.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/03.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/04.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/05.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/06.tga",
											"D:/Ymir Work/UI/Pattern/HPGauge/07.tga",
										),
									},
								),
							},
					
							## UpBringing Pet Feed Button(Life Time Increase)
							{
								"name" : "FeedLifeTimeButton",
								"type" : "toggle_button",
	
								"x" : 207,
								"y" : 173,
	
								"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
								"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
								"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
						
								"text" : uiScriptLocale.PET_INFORMATION_LIFETIME_BUTTON,
								"text_color" : GOLD_COLOR,
							},
					
							## UpBringing Pet Feed Button(evolution)
							{
								"name" : "FeedEvolButton",
								"type" : "toggle_button",
	
								"x" : 207,
								"y" : 194,
	
								"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
								"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
								"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
							
								"text" : uiScriptLocale.PET_INFORMATION_EVOL_BUTTON,
								"text_color" : GOLD_COLOR,
							},
					
							## UpBringing Pet Feed Button(EXP)
							{
								"name" : "FeedExpButton",
								"type" : "toggle_button",
	
								"x" : 207,
								"y" : 215,
	
								"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
								"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
								"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
						
								"text" : uiScriptLocale.PET_INFORMATION_EXP_BUTTON,
								"text_color" : GOLD_COLOR,
							},
					
							## Pet Abilities
							{ 
								"name" : "AbilitiesWindow", "type" : "window", "x" : 43, "y" : 259, "width" : LONG_LABEL_WIDTH, "height" : LONG_LABEL_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"AbilitiesName", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_ABILITIES, "r":1.0, "g":0.0, "b":0.0, "a":1.0, "all_align" : "center"},
								),	
							},
					
							## HP Title
							{ 
								"name" : "HpWindow", "type" : "window", "x" : 20, "y" : 282, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"HpTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_HP, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
							## HP Value
							{ 
								"name" : "HpValueWindow", "type" : "window", "x" : 20 + MIDDLE_LABLE_WIDTH, "y" : 282, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"HpValue", "type":"text", "x":0, "y":0, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),	
							},
					
							## Defence Title
							{ 
								"name" : "DefWindow", "type" : "window", "x" : 20, "y" : 304, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"DefTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_DEFENCE, "color":GOLD_COLOR, "all_align" : "center"},
								),	
							},
							## Defence Value
							{ 
								"name" : "DefValueWindow", "type" : "window", "x" : 20 + MIDDLE_LABLE_WIDTH, "y" : 304, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"DefValue", "type":"text", "x":0, "y":0, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),	
							},
					
							## SP Title
							{ 
								"name" : "SpWindow", "type" : "window", "x" : 20, "y" : 326, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"SpTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_SP, "color":GOLD_COLOR, "all_align" : "center"},
								),
							},
							## SP Value
							{ 
								"name" : "SpValueWindow", "type" : "window", "x" : 20 + MIDDLE_LABLE_WIDTH, "y" : 326, "width" : MIDDLE_LABLE_WIDTH, "height" : MIDDLE_LABLE_HEIGHT, "style" : ("attach",),
								"children" :
								(
									{"name":"SpValue", "type":"text", "x":0, "y":0, "text": "", "color":WHITE_COLOR, "all_align" : "center"},
								),
							},
						
							## Pet Skill Title
							{ 
								"name" : "PetSkillWindow", "type" : "window", "x" : 23, "y" : 371, "width" : 110, "height" : 20, "style" : ("attach",),
								"children" :
								(
									{"name":"PetSkillTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_INFORMATION_SKILL, "r":1.0, "g":0.0, "b":0.0, "a":1.0, "all_align" : "center"},
								),
							},
					
							## Pet Skill Slot 0
							{
								"name" : "PetSkillSlot0",
								"type" : "slot",
						
								"x" : 156,
								"y" : 365,
								"width" : 32,
								"height" : 32,
								"image" : BASE_SLOT_FILE,
						
								"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":32},),
							},
					
							## Pet Skill Slot 1
							{
								"name" : "PetSkillSlot1",
								"type" : "slot",
						
								"x" : 220,
								"y" : 365,
								"width" : 32,
								"height" : 32,
								"image" : BASE_SLOT_FILE,
						
								"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":32},),
							},
					
							## Pet Skill Slot 2
							{
								"name" : "PetSkillSlot2",
								"type" : "slot",
						
								"x" : 284,
								"y" : 365,
								"width" : 32,
								"height" : 32,
								"image" : BASE_SLOT_FILE,
							
								"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":32},),
							},
					
				
						], ## End of board window children			
					},
				],
			},
		],
	}

PET_UI_TAB_X = 13
PET_UI_TAB_Y = 8
PET_UI_TAB_WIDTH = 77
PET_UI_TAB_HEIGHT = 30
#Tab ??	
window["children"] = window["children"] + [
	{
		"name" : "TabControl",
		"type" : "window",
		
		"x" : 0,
		"y" : PET_UI_BG_HEIGHT - 40 - 8,	#??? ?? ?? ?? ?
		
		"width" : PET_UI_BG_WIDTH,
		"height" : 48,				#??? ??
		
		"children" :
		[
			## Tab
			{
				"name" : "Tab_01",
				"type" : "image",
				
				"x" : 0,
				"y" : 0,
				
				"width" : PET_UI_BG_WIDTH,			
				"height" : 48,
				
				"image" : PET_UI_ROOT + "res/tab_1.tga",	
			},
			{
				"name" : "Tab_02",
				"type" : "image",
				
				"x" : 0,
				"y" : 0,
				
				"width" : PET_UI_BG_WIDTH,
				"height" : 48,
				
				"image" : PET_UI_ROOT + "res/tab_2.tga",
			},
			{
				"name" : "Tab_03",
				"type" : "image",
				
				"x" : 0,
				"y" : 0,
				
				"width" : PET_UI_BG_WIDTH,
				"height" : 48,
				
				"image" : PET_UI_ROOT + "res/tab_3.tga",
			},
			## Button
			{
				"name" : "Tab_Button_01",
				"type" : "button",
				
				"x" : PET_UI_TAB_X,
				"y" : PET_UI_TAB_Y,
				
				"width" : PET_UI_TAB_WIDTH,
				"height" : PET_UI_TAB_HEIGHT,
			},
			{
				"name" : "Tab_Button_02",
				"type" : "button",
				
				"x" : PET_UI_TAB_X + 94,
				"y" : PET_UI_TAB_Y,
				
				"width" : PET_UI_TAB_WIDTH,
				"height" : PET_UI_TAB_HEIGHT,
			},
			{
				"name" : "Tab_Button_03",
				"type" : "button",
				
				"x" : PET_UI_TAB_X + 94*2,
				"y" : PET_UI_TAB_Y,
				
				"width" : PET_UI_TAB_WIDTH,
				"height" : PET_UI_TAB_HEIGHT,
			},
		 ],	
	},]
	
#? ?? ?? ??
if app.ENABLE_UPBRINGING_PET_CONQUEROR_LEVEL:
	window["children"] = window["children"] + [
	## Click DETERMINE Button(DETERMINE)
		{
			"name" : "DetermineButton",
			"type" : "button",
	
			"x" : 21,
			"y" : 89 - 10,
			"width" : 38,
			"height" : 26,
	
			"default_image" : PET_UI_ROOT + "determine_button/determine_button_default.sub",
			"over_image" : PET_UI_ROOT + "determine_button/determine_button_over.sub",
			"down_image" : PET_UI_ROOT + "determine_button/determine_button_down.sub",
		},]
else:
	window["children"] = window["children"] + [
	## Click DETERMINE Button(DETERMINE)
		{
			"name" : "DetermineButton",
			"type" : "button",
	
			"x" : 21,
			"y" : 89,
			"width" : 38,
			"height" : 26,
	
			"default_image" : PET_UI_ROOT + "determine_button/determine_button_default.sub",
			"over_image" : PET_UI_ROOT + "determine_button/determine_button_over.sub",
			"down_image" : PET_UI_ROOT + "determine_button/determine_button_down.sub",
		},]

#? ?? ?? ???
window["children"] = window["children"] + [						
	{
		"name" : "PetAttrChange_Page",
		"type" : "window",
		"style" : ("attach",),
		
		"x" : 0,
		"y" : 0,
		
		"width" : PET_PAGE_UI_BG_WIDTH,
		"height" : PET_PAGE_UI_BG_HEIGHT,
		
		"children" :
		[
			## Pet Attr Change UI BG
			{ "name" : "PetAttrChangeUIBG", "type" : "expanded_image", "style" : ("attach",), "x" : 0, "y" : 0, "image" : "d:/ymir work/ui/pet/res/Pet_UI_bg2.tga" },
			
			## Pet Attr Change Title
			{ 
				"name" : "Pet_Attr_Change_Title", "type" : "window", "x" : 142, "y" : 41, "width" : 70, "height" : 11, "style" : ("attach",),
				"children" :
				(
					{"name":"PetAttrChangeTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_ATTR_CHANGE_TITLE, "r":0.85, "g":0.85, "b":0.85, "a":1.0, "all_align" : "center"},
				),	
			},
			
			## UpBringing Pet Item Slot
			{
				"name" : "Change_Pet_Slot",
				"type" : "slot",
				"x" : 160,
				"y" : 67,
				"width" : 32,
				"height" : 173,
				
				"slot" : (
					{"index":0, "x":0, "y":0, "width":32, "height":32},	# ???? ? ???
					{"index":1, "x":0, "y":69, "width":32, "height":32}, # ???? ???
					{"index":2, "x":0, "y":140, "width":32, "height":32}, # ??
				),
			},
			
			## Change Pet Text
			{ 
				"name" : "Change_Pet_Text", "type" : "window", "x" : 151, "y" : 112, "width" : 53, "height" : 11, "style" : ("attach",),
				"children" :
				(
					{"name":"ChangePetText", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_CHANGE_TEXT, "color":WHITE_COLOR, "all_align" : "center"},
				),	
			},
			
			## Attr Change Item Text
			{ 
				"name" : "Attr_Change_Item_Text", "type" : "window", "x" : 133, "y" : 181, "width" : 90, "height" : 11, "style" : ("attach",),
				"children" :
				(
					{"name":"AttrChangeItemText", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_ATTR_CHANGE_ITEM_TEXT, "color":WHITE_COLOR, "all_align" : "center"},
				),	
			},
			
			## Attr Change Result Text
			{ 
				"name" : "Attr_Change_Result_Text", "type" : "window", "x" : 166, "y" : 254, "width" : 23, "height" : 11, "style" : ("attach",),
				"children" :
				(
					{"name":"AttrChangeResultText", "type":"text", "x":0, "y":0, "text": uiScriptLocale.ACCE_RESULT, "color":WHITE_COLOR, "all_align" : "center"},
				),	
			},
			
			## Pet Determine Info Title
			{ 
				"name" : "Pet_Determine_Info_Title", "type" : "window", "x" : 142, "y" : 292, "width" : 70, "height" : 11, "style" : ("attach",),
				"children" :
				(
					{"name":"PetDetermineInfoTitle", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_ATTR_DETERMINE_TITLE, "color":GOLD_COLOR, "all_align" : "center"},
				),	
			},
			
			## Pet Determine Info Text
			{ 
				"name" : "Pet_Determine_Info_Text", "type" : "window", "x" : 70, "y" : 333, "width" : 200, "height" : 11, "style" : ("attach",),
				"children" :
				(
					{"name":"PetDetermineInfoText", "type":"text", "x":0, "y":0, "text": " ", "color":WHITE_COLOR, "all_align" : "center"},
				),	
			},
			
			## Pet Change Button
			{
				"name" : "Pet_Change_Button",
				"type" : "button",

				"x" : 25,
				"y" : 379,

				"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
				"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
				"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
		
				"text" : uiScriptLocale.PET_ATTR_CONFIRMATION,
				"text_color" : GOLD_COLOR,
			},
			
			## Pet OK Button
			{
				"name" : "Pet_OK_Button",
				"type" : "button",

				"x" : 205,
				"y" : 379,

				"default_image" : PET_UI_ROOT + "feed_button/feed_button_default.sub",
				"over_image" : PET_UI_ROOT + "feed_button/feed_button_over.sub",
				"down_image" : PET_UI_ROOT + "feed_button/feed_button_down.sub",
		
				"text" : uiScriptLocale.PET_ATTR_OK,
				"text_color" : GOLD_COLOR,
			},
		],	
	},]

# ???? ? ?? ???
window["children"] = window["children"] + [						
	{
		"name" : "PetPrimiumFeefstuff_Page",
		"type" : "window",
		"style" : ("attach",),
		
		"x" : 0,
		"y" : 0,
		
		"width" : PET_PAGE_UI_BG_WIDTH,
		"height" : PET_PAGE_UI_BG_HEIGHT,
		
		"children" :
		[
			## Pet Primium Feefstuff Page UI BG
			{ "name" : "PetPrimiumFeefstuffUIBG", "type" : "expanded_image", "style" : ("attach",), "x" : 0, "y" : 0, "image" : PET_PRIMIUM_FEEFSTUFF_UI_BG_IMAGE },
			
			## ? ??
			{ 
				"name" : "PetReviveTitleTextWindow", "type" : "window", "x" : 17, "y" : 36, "width" : 317, "height" : 21, "style" : ("attach",),
				"children" :
				(
					{"name":"PetReviveTitleText", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_REVIVE_TITLE, "all_align" : "center"},
				),	
			},
			
			## ??? ?
			{ 
				"name" : "PetReviveTextWindow", "type" : "window", "x" : 35, "y" : 59, "width" : 135, "height" : 21, "style" : ("attach",),
				"children" :
				(
					{"name":"PetReviveText", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_REVIVE_TARGET, "r":0.8, "g":0.76, "b":0.7, "a":1.0, "all_align" : "center"},
				),	
			},
			## ??? ? ?? "? ??" Text
			{ 
				"name" : "PetReviveAgeTextWindow", "type" : "window", "x" : 53, "y" : 132, "width" : 100, "height" : 18, "style" : ("attach",),
				"children" :
				(
					{"name":"PetReviveAgeText", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_AGE_TEXT, "all_align" : "center"},
				),	
			},
			## ??? ? ?? ? ?? ?? "1000?"
			{ 
				"name" : "PetReviveAgeNumberTextWindow", "type" : "window", "x" : 53, "y" : 132+16, "width" : 100, "height" : 18, "style" : ("attach",),
				"children" :
				(
					{"name":"PetReviveAgeNumberText", "type":"text", "x":0, "y":0, "text": "", "all_align" : "center"},
				),	
			},
			## ??? ? ??? "??" Text
			{ 
				"name" : "PetReviveResultTextWindow", "type" : "window", "x" : 181, "y" : 59, "width" : 135, "height" : 21, "style" : ("attach",),
				"children" :
				(
					{"name":"PetReviveResultText", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_REVIVE_REUSLT_TEXT, "r":0.8, "g":0.76, "b":0.7, "a":1.0, "all_align" : "center"},
				),	
			},
			## ?? Text ?? "? ??" Text
			{ 
				"name" : "PetReviveResultAgeTextWindow", "type" : "window", "x" : 199, "y" : 132, "width" : 100, "height" : 18, "style" : ("attach",),
				"children" :
				(
					{"name":"PetReviveResultAgeText", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_AGE_TEXT, "all_align" : "center"},
				),	
			},
			## ?? Text ?? ? ?? ?? "800?"
			{ 
				"name" : "PetReviveResultAgeNumerTextWindow", "type" : "window", "x" : 199, "y" : 132+16, "width" : 100, "height" : 18, "style" : ("attach",),
				"children" :
				(
					{"name":"PetReviveResultAgeNumerText", "type":"text", "x":0, "y":0, "text": "", "all_align" : "center"},
				),	
			},
			## ?????
			{ 
				"name" : "PetReviveMaterialTextWindow", "type" : "window", "x" : 35, "y" : 175, "width" : 135, "height" : 21, "style" : ("attach",),
				"children" :
				(
					{"name":"PetReviveMaterialText", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PET_REVIVE_MATERIAL_TITLE, "r":0.8, "g":0.76, "b":0.7, "a":1.0, "all_align" : "center"},
				),	
			},
			## ?? ??? ??? ?? ??
			{ 
				"name" : "PetReviveMaterialCountTextWindow", "type" : "window", "x" : 219, "y" : 177, "width" : 57, "height" : 17, "style" : ("attach",),
				"children" :
				(
					{"name":"PetReviveMaterialCountText", "type":"text", "x":0, "y":0, "text": localeInfo.PET_REVIVE_UI_MATERIAL_COUNT % (0,0), "all_align" : "center"},
				),	
			},
			## ??? ? Slot
			{
				"name" : "PetReviveSlot",
				"type" : "slot",
		
				"x" : 87,
				"y" : 91,
				"width" : 32,
				"height" : 32,
				"image" : BASE_SLOT_FILE,
		
				"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":32},),
			},
			## ?? Slot
			{
				"name" : "PetReviveResultSlot",
				"type" : "slot",
		
				"x" : 233,
				"y" : 91,
				"width" : 32,
				"height" : 32,
				"image" : BASE_SLOT_FILE,
		
				"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":32},),
			},
			## ?? ??? Slot
			{
				"name" : "PetReviveMaterialSlot",
				"type" : "slot",
		
				"x" : 56,
				"y" : 206,
				"width" : 32*5 + 20*4,
				"height" : 32*2,
				"image" : BASE_SLOT_FILE,
		
				"slot" : (
							## ?? 5?
							{"index":0, "x":0,			"y":0,		"width":32, "height":32},
							{"index":1, "x":32+20,		"y":0,		"width":32, "height":32},
							{"index":2, "x":32*2+20*2,	"y":0,		"width":32, "height":32},
							{"index":3, "x":32*3+20*3,	"y":0,		"width":32, "height":32},
							{"index":4, "x":32*4+20*4,	"y":0,		"width":32, "height":32},
							## ?? 5?
							{"index":5, "x":0,			"y":32,		"width":32, "height":32},
							{"index":6, "x":32+20,		"y":32,		"width":32, "height":32},
							{"index":7, "x":32*2+20*2,	"y":32,		"width":32, "height":32},
							{"index":8, "x":32*3+20*3,	"y":32,		"width":32, "height":32},
							{"index":9, "x":32*4+20*4,	"y":32,		"width":32, "height":32},
						),
			},
			
			{
				"name" : "desc_window",
				"type" : "window",
		
				"x" : 17,
				"y" : 282,
				"width" : 317,
				"height" : 93,							
			},
			
			## ? ?? ?? ? prev button
			{
				"name" : "PetReviveDescPrevButton", "type" : "button", "x" : 277, "y" : 359,
				"default_image"	: "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
				"over_image"	: "d:/ymir work/ui/public/public_intro_btn/prev_btn_02.sub",
				"down_image"	: "d:/ymir work/ui/public/public_intro_btn/prev_btn_01.sub",
			},
			
			## ? ?? ?? ? next button
			{
				"name" : "PetReviveDescNextButton", "type" : "button", "x" : 308, "y" : 359,
				"default_image"	: "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
				"over_image"	: "d:/ymir work/ui/public/public_intro_btn/next_btn_02.sub",
				"down_image"	: "d:/ymir work/ui/public/public_intro_btn/next_btn_01.sub",
			},
			
			### ?? ??
			{
				"name" : "PetReviveButton",
				"type" : "button",
				
				"x" : 56,
				"y" : 383,
				
				"text" : uiScriptLocale.PET_REVIVE_BUTTON,
				
				"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
				"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
				"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
			},
			### ?? ??
			{
				"name" : "PetReviveCancelButton",
				"type" : "button",
				
				"x" : 205,
				"y" : 383,
				
				"text" : uiScriptLocale.PET_REVIVE_CANCEL_BUTTON,
				
				"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
				"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
				"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
			},
		],	
	},]


# ? ?? ? ??? ??? ??
# ?? ???? ??
window["children"] = window["children"] + [
		## Pet Window Title
		{ 
			"name" : "PetWindowTitle", "type" : "window", "x" : 10, "y" : 10, "width" : PET_UI_BG_WIDTH-10-15, "height" : 15, "style" : ("attach",),
			"children" :
			(
				{"name":"TitleName", "type":"text", "x":0, "y":0, "text":uiScriptLocale.PET_INFORMATION_TITLE, "all_align" : "center"},
			),	
		},
									
		## Close Button
		{ 
			"name" : "PetWindowCloseButton", 
			"type" : "button", 
			"x" : PET_UI_BG_WIDTH -10-15, 
			"y" : 10, 
			"tooltip_text" : uiScriptLocale.CLOSE, 
			"default_image" : "d:/ymir work/ui/public/close_button_01.sub",	
			"over_image" : "d:/ymir work/ui/public/close_button_02.sub",
			"down_image" : "d:/ymir work/ui/public/close_button_03.sub",
		},
		]
