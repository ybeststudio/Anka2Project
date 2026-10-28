import uiScriptLocale, localeInfo
import item
import app

import player
EQUIPMENT_START_INDEX = player.EQUIPMENT_SLOT_START

# Reference layout (Baerim InventoryWindowEx, 2026). Slot names of the reference mapped to Anka2:
# companion mount = EQUIPMENT_MOUNT, costume mount / pet = COSTUME_SLOT_MOUNT / COSTUME_SLOT_PET,
# new wear positions 33.. (Buffi, growth pet, sash skin, shining) come from the item module.
def _SLOT(name, wear):
	return getattr(item, name, EQUIPMENT_START_INDEX + wear)

# Anka2 items: mounts are COSTUME_MOUNT costumes, pets are ITEM_PET (WEAR_PET) -> the reference mount / pet slots show them
COMPANION_MOUNT = item.COSTUME_SLOT_MOUNT
COSTUME_MOUNT = item.EQUIPMENT_MOUNT
COSTUME_PET = item.COSTUME_SLOT_PET
COMPANION_BUFFI = _SLOT("COMPANION_BUFFI", 33)
COMPANION_BUFFI_ARMOR = _SLOT("COMPANION_BUFFI_ARMOR", 34)
COMPANION_BUFFI_WEAPON = _SLOT("COMPANION_BUFFI_WEAPON", 35)
COMPANION_BUFFI_HAIR = _SLOT("COMPANION_BUFFI_HAIR", 36)
COMPANION_BUFFI_ACCE = _SLOT("COMPANION_BUFFI_ACCE", 37)
COMPANION_GROWTH_PET = item.EQUIPMENT_PET
COSTUME_SLOT_ACCE_SKIN = _SLOT("COSTUME_SLOT_ACCE_SKIN", 39)
SHINING_SLOT_WEAPON = _SLOT("SHINING_SLOT_WEAPON", 40)
SHINING_SLOT_ARMOR = _SLOT("SHINING_SLOT_ARMOR", 41)

def _T(name, default):
	return getattr(localeInfo, name, default)

INVENTORY_PATH = "d:/ymir work/ui/game/inventory{}"
SPECIAL_INVENTORY_PATH = "d:/ymir work/ui/game/special_inventory{}"
COMMON_PATH = "d:/ymir work/ui/game/common{}"

window = {
	"name" : "InventoryWindow",

	"x" : SCREEN_WIDTH - 176,
	"y" : SCREEN_HEIGHT - 37 - 561,

	"style" : ("movable", "float",),

	"width" : 176,
	"height" : 561,

	"children" :
	(
		## Inventory, Equipment Slots
		{
			"name" : "board",
			"type" : "board",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 176,
			"height" : 561,

			"children" :
			(
				{
					"name" : "ManageInventory",
					"type" : "button",

					"x" : 8,
					"y" : 7,

					"default_image" : INVENTORY_PATH.format("/manage/btn-settings-1.png"),
					"over_image" : INVENTORY_PATH.format("/manage/btn-settings-2.png"),
					"down_image" : INVENTORY_PATH.format("/manage/btn-settings-3.png"),
					"disable_image" : INVENTORY_PATH.format("/manage/btn-settings-3.png"),
				},
				## Title
				{
					"name" : "TitleBar",
					"type" : "titlebar",
					"style" : ("attach",),

					"x" : 8+38,
					"y" : 7,

					"width" : 161-38,
					"color" : "yellow",

					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":77-20, "y":3, "text":uiScriptLocale.INVENTORY_TITLE, "text_horizontal_align":"center" },
					),
				},

				{
					"name" : "SortInventoryButton",
					"type" : "button",

					"x" : 10,
					"y" : 30,

					"tooltip_text": _T("INVENTORY_ITEM_SORT_TOOLTIP", "Envanteri s\xfdrala"),

					"default_image" : INVENTORY_PATH.format("/manage/btn-sort-1.png"),
					"over_image" : INVENTORY_PATH.format("/manage/btn-sort-2.png"),
					"down_image" : INVENTORY_PATH.format("/manage/btn-sort-3.png"),
				},
				{
					"name" : "StackInventoryButton",
					"type" : "button",

					"x" : 35,
					"y" : 30,

					"tooltip_text": _T("SPECIAL_INVENTORY_STACK_ITEMS", "Itemleri birle\xfetir"),

					"default_image" : INVENTORY_PATH.format("/manage/btn-stack-1.png"),
					"over_image" : INVENTORY_PATH.format("/manage/btn-stack-2.png"),
					"down_image" : INVENTORY_PATH.format("/manage/btn-stack-3.png"),
				},
                
				{
					"name" : "bg_input_name",
					"type" : "image",
					"x" : 60,
					"y" : 30,
					"image" : INVENTORY_PATH.format("/manage/input.png"),
					"children" :
					(
						{
							"name" : "editline_name",
							"type" : "editline",
							"x" : 5,
							"y" : 4,
							"width" : 210,
							"height" : 30,
							"input_limit" : 55,
						},
					),
                },

				## Equipment Slot
				{
					"name" : "Equipment_Base",
					"type" : "image",

					"x" : 10,
					"y" : 33,

					"image" : INVENTORY_PATH.format("/equipment_bg.png"),

					"children" :
					(

						{
							"name" : "EquipmentSlot",
							"type" : "slot",

							"x" : 3,
							"y" : 3,

							"width" : 150,
							"height" : 182,

							"slot" : (
										{"index":item.EQUIPMENT_BODY, "x":39, "y":37, "width":32, "height":64},
										{"index":item.EQUIPMENT_HEAD, "x":39, "y":2, "width":32, "height":32},
										{"index":item.EQUIPMENT_SHOES, "x":39, "y":145, "width":32, "height":32},
										{"index":item.EQUIPMENT_WRIST, "x":75, "y":67, "width":32, "height":32},
										{"index":item.EQUIPMENT_WEAPON, "x":3, "y":3, "width":32, "height":96},
										{"index":item.EQUIPMENT_NECK, "x":114, "y":67, "width":32, "height":32},
										{"index":item.EQUIPMENT_EAR, "x":114, "y":35, "width":32, "height":32},
										{"index":item.EQUIPMENT_UNIQUE1, "x":2, "y":144, "width":32, "height":32},
										{"index":item.EQUIPMENT_UNIQUE2, "x":74, "y":144, "width":32, "height":32},
										{"index":item.EQUIPMENT_ARROW, "x":114, "y":2, "width":32, "height":32},
										{"index":item.EQUIPMENT_SHIELD, "x":75, "y":35, "width":32, "height":32},
										{"index":item.EQUIPMENT_BELT, "x":39, "y":106, "width":32, "height":32},
										{"index":item.EQUIPMENT_RING1, "x":2, "y":106, "width":32, "height":32},
										# {"index":item.EQUIPMENT_RING2, "x":75, "y":106, "width":32, "height":32},
										{"index":item.COSTUME_SLOT_ACCE, "x":74, "y":2, "width":32, "height":32},
									),
						},
						{
							"name" : "AcceToolTipButton",
							"type" : "toggle_button",
		
							"x" : 59 + 42,
							"y" : 1,

							"default_image" : COMMON_PATH.format("/eye_normal_01.tga"),
							"over_image" : COMMON_PATH.format("/eye_normal_02.tga"),
							"down_image" : COMMON_PATH.format("/eye_closed_01.tga"),
						},
						{
							"name" : "MyShopButton",
							"type" : "button",

							"x" : 77,
							"y" : 108,

							"tooltip_text" : _T("IKASHOP_BUSINESS_BOARD_TITLE", "\xc7evrimd\xfd\xfe\xfd Pazar") + "^" + _T("IKASHOP_BUSINESS_BOARD_LINK_TOOLTIP", "\xc7evrimd\xfd\xfe\xfd Pazar"),

							"default_image" : "d:/ymir work/ui/game/inventory/btn_offlineshop_norm.png",
							"over_image" : "d:/ymir work/ui/game/inventory/btn_offlineshop_hover.png",
							"down_image" : "d:/ymir work/ui/game/inventory/btn_offlineshop_select.png",
						},
						{
							"name" : "CounterBg",
							"type" : "image",

							"x" : 77+17,
							"y" : 104,

							"image" : "ikashop/lite/business/counter.png",
							"children" :
							(
								{
									"name" : "CounterText",
									"type" : "text",

									"x" : 0,
									"y" : -2,

									"all_align" : "center",

									"text" : "",
								},
							),

						},
						## Dragon Soul Button
						{
							"name" : "DSSButton",
							"type" : "button",

							"x" : 114,
							"y" : 107,

							"tooltip_text" : uiScriptLocale.TASKBAR_DRAGON_SOUL,

							"default_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_01.tga",
							"over_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_02.tga",
							"down_image" : "d:/ymir work/ui/dragonsoul/dss_inventory_button_03.tga",
						},
						## MallButton
						{
							"name" : "MallButton",
							"type" : "button",

							"x" : 118,
							"y" : 148,

							"tooltip_text" : uiScriptLocale.MALL_TITLE,

							"default_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_01.tga",
							"over_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_02.tga",
							"down_image" : "d:/ymir work/ui/game/TaskBar/Mall_Button_03.tga",
						},
						## CostumeButton
						{
							"name" : "CostumeButton",
							"type" : "button",

							"x" : 78,
							"y" : 5,

							"tooltip_text" : uiScriptLocale.COSTUME_TITLE,

							"default_image" : "d:/ymir work/ui/game/taskbar/costume_Button_01.tga",
							"over_image" : "d:/ymir work/ui/game/taskbar/costume_Button_02.tga",
							"down_image" : "d:/ymir work/ui/game/taskbar/costume_Button_03.tga",
						},
						{
							"name" : "Equipment_Tab_01",
							"type" : "radio_button",

							"x" : 86,
							"y" : 161,

							"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
							"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
							"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",

							"children" :
							(
								{
									"name" : "Equipment_Tab_01_Print",
									"type" : "text",

									"x" : 0,
									"y" : 0,

									"all_align" : "center",

									"text" : "I",
								},
							),
						},
						{
							"name" : "Equipment_Tab_02",
							"type" : "radio_button",

							"x" : 86 + 32,
							"y" : 161,

							"default_image" : "d:/ymir work/ui/game/windows/tab_button_small_01.sub",
							"over_image" : "d:/ymir work/ui/game/windows/tab_button_small_02.sub",
							"down_image" : "d:/ymir work/ui/game/windows/tab_button_small_03.sub",

							"children" :
							(
								{
									"name" : "Equipment_Tab_02_Print",
									"type" : "text",

									"x" : 0,
									"y" : 0,

									"all_align" : "center",

									"text" : "II",
								},
							),
						},

					),
				},

				## Costume Slot
				{
					"name" : "Costume_Base",
					"type" : "image",

					"x" : 10,
					"y" : 33,

					"image" : "d:/ymir work/ui/game/inventory/costumes.png",

					"children" :
					(					
						{
							"name" : "CostumeSlot",
							"type" : "slot",

							"x" : 3,
							"y" : 3,

							"width" : 150,
							"height" : 182,

							"slot" : (
								{"index":item.COSTUME_SLOT_BODY, "x":58, "y":70, "width":32, "height":64},
								{"index":item.COSTUME_SLOT_HAIR, "x":58, "y":35, "width":32, "height":32},
								{"index":item.COSTUME_SLOT_WEAPON, "x":13, "y":43, "width":32, "height":96},	
								{"index":COSTUME_SLOT_ACCE_SKIN, "x":108, "y":52, "width":32, "height":32},	
								{"index":SHINING_SLOT_WEAPON, "x":21, "y":144, "width":32, "height":32},
								{"index":SHINING_SLOT_ARMOR, "x":95, "y":144, "width":32, "height":32},
							
							),
						},	
						{
							"name" : "Acce2ToolTipButton",
							"type" : "toggle_button",
		
							"x" : 110 + 26,
							"y" : 64,
		
							"default_image" : COMMON_PATH.format("/eye_normal_01.tga"),
							"over_image" : COMMON_PATH.format("/eye_normal_02.tga"),
							"down_image" : COMMON_PATH.format("/eye_closed_01.tga"),
						},
						{
							"name" : "BodyToolTipButton",
							"type" : "toggle_button",
		
							"x" : 59 + 32,
							"y" : 77,
		
							"default_image" : COMMON_PATH.format("/eye_normal_01.tga"),
							"over_image" : COMMON_PATH.format("/eye_normal_02.tga"),
							"down_image" : COMMON_PATH.format("/eye_closed_01.tga"),
						},
						{
							"name" : "HairToolTipButton",
							"type" : "toggle_button",
		
							"x" : 59 + 32,
							"y" : 37,
		
							"default_image" : COMMON_PATH.format("/eye_normal_01.tga"),
							"over_image" : COMMON_PATH.format("/eye_normal_02.tga"),
							"down_image" : COMMON_PATH.format("/eye_closed_01.tga"),
						},
						{
							"name" : "WeaponToolTipButton",
							"type" : "toggle_button",
		
							"x" : 8 + 32,
							"y" : 60,
		
							"default_image" : COMMON_PATH.format("/eye_normal_01.tga"),
							"over_image" : COMMON_PATH.format("/eye_normal_02.tga"),
							"down_image" : COMMON_PATH.format("/eye_closed_01.tga"),
						},	
						{
							"name" : "CostumeShopButton",
							"type" : "button",

							"x" : 5,
							"y" : 5,

							"tooltip_text" : _T("COSTUME_SHOP_TITLE", "Kost\xfcm Ma\xf0azas\xfd"),

							"default_image" : COMMON_PATH.format("/shop_btn.png"),
							"over_image" : COMMON_PATH.format("/shop_btn.png"),
							"down_image" : COMMON_PATH.format("/shop_btn.png"),
						},
						
					),
				},

				# Companion
				{
					"name" : "Companion_Base",
					"type" : "image",
	
					"x" : 10,
					"y" : 33,
	
					"image" : "d:/ymir work/ui/game/inventory/compagnon.png",
					"children" :
					(

						{
							"name" : "CompanionSlot",
							"type" : "slot",

							"x" : 3,
							"y" : 3,
							"width" : 150,
							"height" : 182,

							"slot" : (	
								{"index":COMPANION_MOUNT, "x":78, "y":133, "width":32, "height":32},
								{"index":COSTUME_MOUNT, "x":113, "y":133, "width":32, "height":32},
								{"index":COMPANION_GROWTH_PET, "x":5, "y":133, "width":32, "height":32},
								{"index":COSTUME_PET, "x":41, "y":133, "width":32, "height":32},
								{"index":COMPANION_BUFFI, "x":113, "y":87, "width":32, "height":32},

								{"index":COMPANION_BUFFI_ACCE, "x":112, "y":13, "width":32, "height":32},
								{"index":COMPANION_BUFFI_WEAPON, "x":10, "y":39, "width":32, "height":32},
								{"index":COMPANION_BUFFI_ARMOR, "x":63, "y":48, "width":32, "height":64},
								{"index":COMPANION_BUFFI_HAIR, "x":63, "y":7, "width":32, "height":32},
							),
						},
						{
							"name" : "info_buffi",
							"type" : "button",

							"x" : 107,
							"y" : 60,
                            
							"tooltip_text": _T("BUFFI_NO_BONUS_COSTUME", "Buffi kost\xfcmleri bonus vermez."),

							"default_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
							"over_image" : "d:/ymir work/ui/pattern/q_mark_02.tga",
							"down_image" : "d:/ymir work/ui/pattern/q_mark_01.tga",
						},
					),
				}, 

				# {
				# 	"name" : "bg_slot",
				# 	"type" : "image",
				# 	"x" : 8,
				# 	"y" : 33 + 188,
				# 	"image" : SPECIAL_INVENTORY_PATH.format("/background_inventory.tga"),
                # },

				{
					"name" : "Inventory_Tab_01",
					"type" : "radio_button",

					"x" : 10,
					"y" : 33 + 189,

					"default_image" : INVENTORY_PATH.format("/norm.png"),
					"over_image" : INVENTORY_PATH.format("/hover.png"),
					"down_image" : INVENTORY_PATH.format("/select.png"),
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_1,

					"children" :
					(
						{
							"name" : "Inventory_Tab_01_Print",
							"type" : "text",

							"x" : 0,
							"y" : -1,

							"outline": True,
							"color": 0xFFcabdab,
							"all_align" : "center",

							"text" : "I",
						},
					),
				},
				{
					"name" : "Inventory_Tab_02",
					"type" : "radio_button",

					#"x" : 10 + 78,
					"x" : 10 + 39,
					"y" : 33 + 189,

					"default_image" : INVENTORY_PATH.format("/norm.png"),
					"over_image" : INVENTORY_PATH.format("/hover.png"),
					"down_image" : INVENTORY_PATH.format("/select.png"),
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_2,

					"children" :
					(
						{
							"name" : "Inventory_Tab_02_Print",
							"type" : "text",

							"x" : 0,
							"y" : -1,

							"outline": True,
							"color": 0xFFcabdab,
							"all_align" : "center",

							"text" : "II",
						},
					),
				},

				{
					"name" : "Inventory_Tab_03",
					"type" : "radio_button",

					"x" : 10 + 39 + 39,
					"y" : 33 + 189,

					"default_image" : INVENTORY_PATH.format("/norm.png"),
					"over_image" : INVENTORY_PATH.format("/hover.png"),
					"down_image" : INVENTORY_PATH.format("/select.png"),
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_3,

					"children" :
					(
						{
							"name" : "Inventory_Tab_03_Print",
							"type" : "text",

							"x" : 0,
							"y" : -1,

							"outline": True,
							"color": 0xFFcabdab,
							"all_align" : "center",

							"text" : "III",
						},
					),
				},

				{
					"name" : "Inventory_Tab_04",
					"type" : "radio_button",

					"x" : 10 + 39 + 39 + 39,
					"y" : 33 + 189,

					"default_image" : INVENTORY_PATH.format("/norm.png"),
					"over_image" : INVENTORY_PATH.format("/hover.png"),
					"down_image" : INVENTORY_PATH.format("/select.png"),
					"tooltip_text" : uiScriptLocale.INVENTORY_PAGE_BUTTON_TOOLTIP_4,

					"children" :
					(
						{
							"name" : "Inventory_Tab_04_Print",
							"type" : "text",

							"x" : 0,
							"y" : -1,

							"outline": True,
							"color": 0xFFcabdab,
							"all_align" : "center",

							"text" : "IV",
						},
					),
				},

				## Item Slot
				{
					"name" : "ItemSlot",
					"type" : "grid_table",

					"x" : 8,
					"y" : 242,

					"start_index" : 0,
					"x_count" : 5,
					"y_count" : 9,
					"x_step" : 32,
					"y_step" : 32,

					"image" : "d:/ymir work/ui/public/Slot_Base.sub"
				},

				## Print
				{
					"name":"Money_Slot",
					"type":"button",

					"x":8,
					"y":28,

					"horizontal_align":"center",
					"vertical_align":"bottom",

					"default_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
					"over_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",
					"down_image" : "d:/ymir work/ui/public/parameter_slot_05.sub",

					"children" :
					(
						{
							"name":"Money_Icon",
							"type":"image",

							"x":-18,
							"y":2,

							"image":"d:/ymir work/ui/game/windows/money_icon.sub",
						},

						{
							"name" : "Money",
							"type" : "text",

							"x" : 3,
							"y" : 3,

							"horizontal_align" : "right",
							"text_horizontal_align" : "right",

							"text" : "123456789",
						},
					),
				},

			),
		},
	),
}
