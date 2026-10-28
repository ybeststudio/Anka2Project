import uiScriptLocale
import app

# ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH:	# 미사용 경로 제거
#LOCALE_PATH = uiScriptLocale.WINDOWS_PATH
ROOT = "d:/ymir work/ui/game/party_match/"

UI_WIDTH	= 264
UI_HEIGHT	= 200
UI_BUTTON_POS_Y = 158

SLOT_WIDTH	= 32
SLOT_HEIGHT	= 32
SLOT_GAP	= 12

BUTTON_WIDTH	= 88
BUTTON_HEIGHT	= 21

FONT_COLOR1		= 0xFFD4D4D4
FONT_COLOR2		= 0xFFDDCEC3

window = {
	"name" : "PartyMatchWindow",
	"style" : ("movable", "float",),
	
	"x" : SCREEN_WIDTH / 2 - UI_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - UI_HEIGHT / 2,

	"width" : UI_WIDTH,
	"height" : UI_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : UI_WIDTH,
			"height" : UI_HEIGHT,
			
			"title" : uiScriptLocale.KEYCHANGE_PARTY_MATCH_WINDOW,
			
			"children" :
			(
				## bg
				{
					"name" : "main_bg",
					"type" : "expanded_image",
					"style" : ("attach",),
					
					"x" : 14,
					"y" : 36,
					"image" : ROOT + "bg.sub",
				},
				
				## Text
				{ 
					"name" : "dungeon_name_text_window", "type" : "window", "x" : 21, "y" : 45, "width" : 83, "height" : 16, "style" : ("attach",),
					"children" :
					(
						{"name":"dungeon_name_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PARTY_MATCH_TEXT_DUNGEON_NAME, "color":FONT_COLOR1, "all_align" : "center"},
					),	
				},
				{ 
					"name" : "required_items_text_window", "type" : "window", "x" : 21, "y" : 79, "width" : 83, "height" : 16, "style" : ("attach",),
					"children" :
					(
						{"name":"required_items_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PARTY_MATCH_TEXT_REQUIRED_ITEMS, "color":FONT_COLOR1, "all_align" : "center"},
					),	
				},
				{ 
					"name" : "level_limit_text_window", "type" : "window", "x" : 21, "y" : 115, "width" : 83, "height" : 16, "style" : ("attach",),
					"children" :
					(
						{"name":"level_limit_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PARTY_MATCH_TEXT_LEVEL_LIMIT, "color":FONT_COLOR1, "all_align" : "center"},
					),	
				},
				{ 
					"name" : "entree_level_text_window", "type" : "window", "x" : 109, "y" : 114, "width" : 130, "height" : 18, "style" : ("attach",),
					"children" :
					(
						{"name":"entree_level_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PARTY_MATCH_TEXT_ENTREE_LEVEL % 0, "color":FONT_COLOR2, "all_align" : "center"},
					),	
				},
				
				## 필요아이템 슬롯
				{
					"name" : "required_items_slot_window",
					"type" : "window",
					"style" : ("attach",),

					"x" : 115,
					"y" : 72,
					"width" : SLOT_WIDTH*3 + SLOT_GAP*2,
					"height" : SLOT_HEIGHT,
					
					"children" :
					(
						{
							"name" : "required_items_slot",
							"type" : "slot",
							
							"x" : 0,
							"y" : 0,
							
							"width" : SLOT_WIDTH*3 + SLOT_GAP*2,
							"height" : SLOT_HEIGHT,
							
							"image" : "d:/ymir work/ui/public/Slot_Base.sub",
							
							"slot" : 
							(
								{"index":0, "x":0, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":1, "x":SLOT_WIDTH * 1 + SLOT_GAP * 1, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
								{"index":2, "x":SLOT_WIDTH * 2 + SLOT_GAP * 2, "y":0, "width":SLOT_WIDTH, "height":SLOT_HEIGHT},
							),
						},
					),
				},
				
				## 버튼
			    {
				    "name" : "MatchingButton",
				    "type" : "button",

				    "x" : 44,
				    "y" : UI_BUTTON_POS_Y,

				    "text" : uiScriptLocale.PARTY_MATCH_TEXT_REQUEST_MATCH,

				    "default_image" : "d:/ymir work/ui/public/large_button_01.sub",
				    "over_image" : "d:/ymir work/ui/public/large_button_02.sub",
				    "down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				    "disable_image" : "d:/ymir work/ui/public/large_button_03.sub",
			    },
			    {
				    "name" : "CloseButton",
				    "type" : "button",

				    "x" : 156,
				    "y" : UI_BUTTON_POS_Y,

				    "text" : uiScriptLocale.CLOSE,

				    "default_image" : "d:/ymir work/ui/public/middle_button_01.sub",
				    "over_image" : "d:/ymir work/ui/public/middle_button_02.sub",
				    "down_image" : "d:/ymir work/ui/public/middle_button_03.sub",
			    },
			    
			    ## 던전 선택 Text
				{
					"name" : "dungeon_select_text_window",
					"type" : "window",
					
					"x" : 109,
					"y" : 44,
					"width" : 130,
					"height" : 18,
					
					"children" :
					(
						{"name":"dungeon_select_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.PARTY_MATCH_TEXT_DUNGEON_SELECT, "color":FONT_COLOR2, "all_align" : "center"},
					),
				},
				## 던전 이름 버튼들
				{
					"name" : "dungeon_select_window",
					"type" : "window",

					"x" : 109,
					"y" : 61,
					"width" : 130,
					"height" : 0,
				},
				## 던전 선택 버튼
				{
					"name" : "dungeon_select_button",
					"type" : "button",

					"x" : 222,
					"y" : 45,

					"default_image" : ROOT + "arrow_default.sub",
					"over_image" : ROOT + "arrow_over.sub",
					"down_image" : ROOT + "arrow_down.sub",
				},
				
				## dungeon mouse over img
				{
					"name" : "mouse_over_image",
					"type" : "expanded_image",
					"style" : ("not_pick",),
					
					"x" : 0,
					"y" : 0,
					"image" : ROOT + "button_over.sub",
				},
			),
		},
	),
}