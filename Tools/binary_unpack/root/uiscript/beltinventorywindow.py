import localeInfo

import item

EQUIPMENT_START_INDEX = 90
BUTTON_ROOT = "d:/ymir work/ui/public/"

NEW_GUILD_PATH = "d:/ymir work/ui/game/new_guild/4/up_skill/"
DUNGEON_PATH = "d:/ymir work/ui/game/teleport_window/dungeon{}"

window = {
	"name" : "BeltInventoryWindow",

	## 600 - (width + ????? ?? ??? 24 px)
	"x" : SCREEN_WIDTH - 176 - 148,
	"y" : SCREEN_HEIGHT - 37 - 565 + 209 + 32,
#	"x" : -148,
#	"y" : 241,
	"width" : 248,
	"height" : 269,

	"style" : ("float", "not_pick",),

	"type" : "window",
	# "image" : "d:/ymir work/ui/game/belt_inventory/bg.tga",
	

	"children" :
	(
		## Expand Buttons
		{
			"name" : "ExpandBtn",
			"type" : "button",

			"x" : 2,
			"y" : 15,

			"default_image" : "d:/ymir work/ui/game/belt_inventory/btn_expand_normal.tga",
			"over_image" : "d:/ymir work/ui/game/belt_inventory/btn_expand_over.tga",
			"down_image" : "d:/ymir work/ui/game/belt_inventory/btn_expand_down.tga",
			"disable_image" : "d:/ymir work/ui/game/belt_inventory/btn_expand_disabled.tga",
		},
		
		## Belt Inventory Layer (include minimize button)
		{
			"name" : "BeltInventoryLayer",
#			"type" : "board",
#			"style" : ("attach", "float"),

			"x" : 5,
			"y" : 0,

			"width" : 248,
			"height" : 269,

			"children" :
			(
				## Minimize Button
				{
					"name" : "MinimizeBtn",
					"type" : "button",

					"x" : 2,
					"y" : 15+60,

					"width" : 10,

					"default_image" : "d:/ymir work/ui/game/belt_inventory/btn_minimize_normal.tga",
					"over_image" : "d:/ymir work/ui/game/belt_inventory/btn_minimize_over.tga",
					"down_image" : "d:/ymir work/ui/game/belt_inventory/btn_minimize_down.tga",
					"disable_image" : "d:/ymir work/ui/game/belt_inventory/btn_minimize_disabled.tga",
				},

				## Real Belt Inventory Board
				{
					"name" : "BeltInventoryBoard",
					"type" : "board",
					"style" : ("attach", "float"),

					"x" : 10,
					"y" : 0,

					"width" : 238,
					"height" : 269,

					"children" :
					(
						## Belt Inventory Slots
						{
							"name" : "BeltInventorySlot",
							"type" : "grid_table",

							"x" : 5,
							"y" : 5,

							"start_index" : item.BELT_INVENTORY_SLOT_START,
							"x_count" : 7,
							"y_count" : 7,
							"x_step" : 32,
							"y_step" : 32,

							"image" : "d:/ymir work/ui/public/Slot_Base.sub"
						},
						##activateall
						{
							"name" : "UseBeltItemsButton",
							"type" : "button",
							"x" : 15,
							"y" : 234,
                            "outline": True,
							"color": 0xFFcabdab,
							"text": localeInfo.ACTIVATE,
							"tooltip_text": localeInfo.BELT_ACTIVATE_TOOLTIP2,
							"default_image" : "d:/ymir work/ui/game/common/button_middle_norm.png",
							"over_image" : "d:/ymir work/ui/game/common/button_middle_hover.png",
							"down_image" : "d:/ymir work/ui/game/common/button_middle_down.png",
						},
						##activateall
						{
							"name" : "ShopBeltButton",
							"type" : "button",
							"x" : 102,
							"y" : 230,
                            "outline": True,
							"color": 0xFFcabdab,
							"tooltip_text": localeInfo.IKASHOP_SEARCH_SHOP_OPEN_SHOP_BUTTON_TEXT,
							"default_image" : DUNGEON_PATH.format("/shop_btn.png"),
							"over_image" : DUNGEON_PATH.format("/shop_btn.png"),
							"down_image" : DUNGEON_PATH.format("/shop_btn.png"),
						},
						##activateall
						{
							"name" : "DeactiveBeltItemsButton",
							"type" : "button",
							"x" : 143,
							"y" : 234,
                            "outline": True,
							"color": 0xFFcabdab,
							"text": localeInfo.DESACTIVATE,
							"tooltip_text": localeInfo.BELT_DESACTIVATE_TOOLTIP2,
							"default_image" : "d:/ymir work/ui/game/common/button_middle_norm.png",
							"over_image" : "d:/ymir work/ui/game/common/button_middle_hover.png",
							"down_image" : "d:/ymir work/ui/game/common/button_middle_down.png",
						},
					),
				},
			)
		},

	),
}
