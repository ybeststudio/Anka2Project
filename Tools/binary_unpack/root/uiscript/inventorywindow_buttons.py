import app
import uiScriptLocale

INVENTORY_PATH = "d:/ymir work/ui/game/inventory/"
EQUIPMENT_BUTTON_MARGIN = 34

window = {
	"name" : "InventoryWindowButtons",

	"x" : 0,
	"y" : 0,

	"width" : 100,
	"height" : 215,

	"style" : ("float", "not_pick",),

	"children" :
	(
		{
			"name" : "Equipment_Tab_1",
			"type" : "radio_button",
			"x" : 13,
			"y" : 23,
			"default_image" : INVENTORY_PATH + "eq_norm.png",
			"over_image" : INVENTORY_PATH + "eq_hover.png",
			"down_image" : INVENTORY_PATH + "eq_select.png",
		},
		{
			"name" : "Equipment_Tab_2",
			"type" : "radio_button",
			"x" : 13,
			"y" : 23 + (EQUIPMENT_BUTTON_MARGIN * 1),
			"default_image" : INVENTORY_PATH + "cosm_norm.png",
			"over_image" : INVENTORY_PATH + "cosm_hover.png",
			"down_image" : INVENTORY_PATH + "cosm_select.png",
		},
		{
			"name" : "Equipment_Tab_3",
			"type" : "radio_button",
			"x" : 13,
			"y" : 23 + (EQUIPMENT_BUTTON_MARGIN * 2),
			"default_image" : INVENTORY_PATH + "pet_norm.png",
			"over_image" : INVENTORY_PATH + "pet_hover.png",
			"down_image" : INVENTORY_PATH + "pet_select.png",
		},
        
		{
			"name" : "Button_Auto_Cape",
			"type" : "button",
			"x" : 13,
			"y" : 23 + 110,
			"default_image" : INVENTORY_PATH + "cape_norm.png",
			"over_image" : INVENTORY_PATH + "cape_hover.png",
			"down_image" : INVENTORY_PATH + "cape_select.png",
		},

		{
			"name" : "Button_Quest_Book",
			"type" : "button",
			"x" : 13,
			"y" : 23 + 110+34,
			"default_image" : INVENTORY_PATH + "quest_norm.png",
			"over_image" : INVENTORY_PATH + "quest_hover.png",
			"down_image" : INVENTORY_PATH + "quest_select.png",
		},
	),
}

if True:	# the reference shows the wardrobe button (ENABLE_WARDROBE_SYSTEM)
	window["height"] = 249
	window["children"] += ({
		"name": "Button_Wardrobe",
		"type": "button",
		"x": 13,
		"y": 23 + 110 + 34 * 2,
		"default_image": INVENTORY_PATH + "wardrobe_norm.png",
		"over_image": INVENTORY_PATH + "wardrobe_hover.png",
		"down_image": INVENTORY_PATH + "wardrobe_select.png",
	},)
