if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

WINDOW_WIDTH	= 176
WINDOW_HEIGHT	= 184

window = {
	"name" : "PetNameChangeWindow",
	"style" : ("movable", "float",),
	
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,

	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,

			"children" :
			(
				# Pet Name Change Window Title
				{
					"name" : "PetNameChangeTitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 0,
					"y" : 0,

					"width" : WINDOW_WIDTH - 2,
				
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":0, "y":0, "text":uiScriptLocale.PET_NAME_CHANGE_WINDOW_TITLE, "all_align":"center" },
					),
				},
				
				## Pet Slot Img
				{ 
					"name" : "SlotBG", 
					"type" : "expanded_image", 
					"style" : ("attach",), 
					"x" : 68, 
					"y" : 34, 
					"image" : "d:/ymir work/ui/pet/Pet_Incu_slot_001.tga",
				},
				
				## Pet Slot
				{
					"name" : "PetItemSlot",
					"type" : "slot",

					"x" : 68 + 4,
					"y" : 34 + 4,
					"width" : 32,
					"height" : 32,

					"slot" : ({"index":0, "x":0, "y":0, "width":32, "height":32,},),
				},
				
				## Pet NameChange Money
				{
					"name" : "NameChangeMoneyWindow", "type" : "window", "x" : 13, "y" : 132, "width" : 150, "height" : 14, "style" : ("attach",),
					"children" :
					(
						{"name":"NameChangeMoney", "type":"text", "x":0, "y":0, "text": "Coin : 100000 ?", "r":1.0, "g":1.0, "b":1.0, "a":1.0, "all_align" : "center"},
					),
				},
				
				## Pet NameChange Button
				{
					"name" : "NameChangeButton",
					"type" : "button",

					"x" : 39,
					"y" : 151,

					"text" : uiScriptLocale.PET_NAME_CHANGE_BUTTON,

					"default_image" : "d:/ymir work/ui/pattern/LargeB_Button_01.tga",
					"over_image" : "d:/ymir work/ui/pattern/LargeB_Button_02.tga",
					"down_image" : "d:/ymir work/ui/pattern/LargeB_Button_03.tga",
				},
				
				## Pet Naming Area
				{
					"name" : "PetNamingBG", 
					"type" : "expanded_image",
					"style" : ("attach",),
					
					"x" : 12, 
					"y" : 78, 
					
					"image" : "d:/ymir work/ui/pet/Pet_Incu_001.tga",
					
					"children" :
					(
						## Pet Name Edit
						{
							"name" : "pet_name",
							"type" : "editline",

							"x" : 11,
							"y" : 28,

							"width" : 129,
							"height" : 16,

							"input_limit" : 20,
						},
					),
				},
						
				## Pet Naming Title
				{ 
					"name" : "PetNamingTitleWindow", "type" : "window", "x" : 22, "y" : 86, "width" : 130, "height" : 14, "style" : ("attach",),
					"children" :
					(
						{"name":"PetNamingTitle", "type":"text", "x":0, "y":0, "text":uiScriptLocale.PET_HATCHING_NIK, "all_align" : "center"},
					),
				},
			),				
		},
	),
}

#"name":"pet_name"
window["children"][0]["children"][5]["children"][0]["movable_text"]	= 1