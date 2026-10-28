import uiScriptLocale, localeInfo

IMG_DIR = "d:/ymir work/ui/game/advanced_game_options/"

TITLE_IMAGE_TEXT_X = 58
TITLE_IMAGE_TEXT_Y = 5

OPTION_START_X = 17
SLIDER_POSITION_X = 30

SLIDER_START_Y = 50
BUTTON_START_Y = 33
BUTTON_NEXT_Y = 20

RADIO_BUTTON_RANGE_X = 80
TOGGLE_BUTTON_RANGE_X = 65
RADIO_BUTTON_LARGE_RANGE_X = 90

RADIO_BUTTON_TEXT_X = 25
TOGGLE_BUTTON_TEXT_X = 45

SMALL_OPTION_HEIGHT = 65
NORMAL_OPTION_HEIGHT = 80
SLIDER_OPTION_HEIGHT = 65
FPS_OPTION_HEIGHT = 152
FONT_OPTION_HEIGHT = 145
WINDOW_SCALE_OPTION_HEIGHT = 0

PRIMARY_COLOR = 0xffd0c2b7
SECONDARY_COLOR = 0xff9f9892

if not hasattr(localeInfo, "GAME_OPTION_EVENT_STONE_APPEARANCE"):
	localeInfo.GAME_OPTION_EVENT_STONE_APPEARANCE = "Event Stone Appearance"
if not hasattr(localeInfo, "GAME_OPTION_EVENT_STONE_APPEARANCE_DESC"):
	localeInfo.GAME_OPTION_EVENT_STONE_APPEARANCE_DESC = "Toggle the visibility of event stones."

window = {
	"name" : "GameOptionDialog",
	# Dont touch these lines!
	"style" : (),
	"x" : 222,
	"y" :1,
	"width" : 371,
	"height" : 404,
	# Dont touch these lines!
	"children" :
	(
		{
			"name" : "keyboard_window",
			"type" : "window",
			"x" : 0,
			"y" : 5,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "keyboard_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_keyboard",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : "Keyboard Settings",
						},
					),
				},				
				{
					"name" : "keyboard_btn",
					"type" : "toggle_button",
                    "horizontal_align": "center",
					"x" : 25,
					"y" : 35,
					"text_color": 0xFFcebb98,
                    "text": "Open",
                    "outline": True,
					"default_image" : IMG_DIR + "btn_0.png",
					"over_image" : IMG_DIR + "btn_1.png",
					"down_image" : IMG_DIR + "btn_2.png",
				},
			),
		},		

		{
			"name" : "chat_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT+40,
			"children":
			(
				{
					"name" : "chat_mode_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_chat_mode",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : uiScriptLocale.OPTION_VIEW_CHAT,
						},
					),
				},				
				{
					"name" : "view_chat_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "chat_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : "Chat",
						},
						{
							"name" : "toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : "Disable / Enable chat box at the bottom center of the game window.",
							"color" : SECONDARY_COLOR,
						},			
					),
				},	
				{
					"name" : "hide_dungeon_announcement_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 72,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "hide_dungeon_announcement_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : "Hide Dungeon Announcement",
						},
						{
							"name" : "hide_dungeon_announcement_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : "Hide completed dungeon notices from all chat.",
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},		


		{
			"name" : "drop_ui_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75*2+40,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "drop_interface_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_drop_interface",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_DROP_INTERFACE,
						},
					),
				},				
				{
					"name" : "view_drop_ui_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "chat_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color": 0xFFcebb98,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_DROP_INTERFACE,
						},
						{
							"name" : "toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_DROP_INTERFACE_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	
			),
		},									

		{
			"name" : "guideline_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75*3+40,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "guideline_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_guideline",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUIDELINE_BTN,
						},
					),
				},				
				{
					"name" : "view_guideline_btn_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "chat_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color": 0xFFcebb98,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUIDELINE_BTN,
						},
						{
							"name" : "toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUIDELINE_BTN_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	
			),
		},									

		{
			"name" : "system_menu_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75*4+40,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "system_menu_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_system_menu",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SYSTEM_MENU,
						},
					),
				},				
				{
					"name" : "view_system_menu_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "chat_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color": 0xFFcebb98,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SYSTEM_MENU_SUBTITLE,
						},
						{
							"name" : "toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" :localeInfo.GAME_OPTION_SYSTEM_MENU_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	
			),
		},									

		{
			"name" : "event_icon_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75*5+40,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "event_icon_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_event_icon",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_EVENT_ICON,
						},
					),
				},				
				{
					"name" : "view_event_icon_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "chat_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color": 0xFFcebb98,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_EVENT_ICON_SUBTITLE,
						},
						{
							"name" : "toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" :localeInfo.GAME_OPTION_EVENT_ICON_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	
			),
		},									

		{
			"name" : "window_scale_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75*6+40,
			"width":304,
			"height":WINDOW_SCALE_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "window_scale_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_window_scale",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : getattr(localeInfo, "GAME_OPTION_WINDOW_SCALE", "Window scale"),
						},
					),
				},
				{
					"name" : "window_scale_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y,
				},
				{
					"name" : "window_scale_apply_button",
					"type" : "toggle_button",
					"horizontal_align": "center",
					"x" : 25,
					"y" : 68,
					"text_color": 0xFFcebb98,
					"text": "Apply",
					"outline": True,
					"default_image" : IMG_DIR + "btn_0.png",
					"over_image" : IMG_DIR + "btn_1.png",
					"down_image" : IMG_DIR + "btn_2.png",
				},
			),
		},

		{
			"name" : "font_change_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75*6 + WINDOW_SCALE_OPTION_HEIGHT+40,
			"width":304,
			"height":FONT_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "font_name_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 5,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : "Font Name",
				},
				{
					"name" : "font_btn_0",
					"type" : "radio_button",
					"x" : OPTION_START_X,
					"y" : 27,
					"text" : "Tahoma",
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "font_btn_1",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 27,
					"text" : "Arial",
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "big_font_size_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 58,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "big_font_size_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color": 0xFFcebb98,
							"text_horizontal_align":"left",
							"text" : "Big Font Size",
						},
						{
							"name" : "big_font_size_description",
							"type" : "adjust_text",
							"max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : "Enable / Disable the big font size",
							"color" : SECONDARY_COLOR,
						},
					),
				},
				{
					"name" : "big_slot_count_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 102,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "big_slot_count_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color": 0xFFcebb98,
							"text_horizontal_align":"left",
							"text" : "Big Slot Count",
						},
						{
							"name" : "big_slot_count_description",
							"type" : "adjust_text",
							"max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : "Enable / Disable the big slot count",
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
        
		{
			"name" : "save_status_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75*6 + WINDOW_SCALE_OPTION_HEIGHT + FONT_OPTION_HEIGHT+40,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "save_status_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_save_status",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SAVE_STATUS_WINDOW,
						},
					),
				},				
				{
					"name" : "save_status_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "save_status_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color": 0xFFcebb98,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SAVE_STATUS_WINDOW,
						},
						{
							"name" : "save_status_toggle2_button_description",
							"type" : "adjust_text",
                            "max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SAVE_STATUS_WINDOW_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	
			),
		},									

		{
			"name" : "small_affect_icon_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75*7 + WINDOW_SCALE_OPTION_HEIGHT + FONT_OPTION_HEIGHT+40,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "small_affect_icon_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_small_affect_icon",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SMALL_AFFECT_ICON,
						},
					),
				},
				{
					"name" : "small_affect_icon_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "small_affect_icon_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color": 0xFFcebb98,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SMALL_AFFECT_ICON,
						},
						{
							"name" : "small_affect_icon_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SMALL_AFFECT_ICON_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},

		{
			"name" : "guild_quest_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75*6 + WINDOW_SCALE_OPTION_HEIGHT + FONT_OPTION_HEIGHT + 80+40,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "guild_quest_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_guild_quest",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUILD_QUEST_WINDOW,
						},
					),
				},				
				{
					"name" : "guild_quest_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "guild_quest_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color": 0xFFcebb98,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUILD_QUEST_WINDOW,
						},
						{
							"name" : "guild_quest_toggle2_button_description",
							"type" : "adjust_text",
                            "max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUILD_QUEST_WINDOW_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	
			),
		},									
		{
			"name" : "whisper_sender_letter_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75*6 + WINDOW_SCALE_OPTION_HEIGHT + FONT_OPTION_HEIGHT + 80 + 80+40,
			"width":304,
			"height":168,
			"children":
			(
				{
					"name" : "whisper_sender_letter_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_whisper_sender_letter",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_WHISPER_TITLE,
						},
					),
				},
				{
					"name" : "view_whisper_sender_letter_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "whisper_sender_letter_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_WHISPER_SENDER_LETTERS,
						},
						{
							"name" : "whisper_sender_letter_button_description",
							"type" : "adjust_text",
							"max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_WHISPER_SENDER_LETTERS_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},
				{
					"name" : "guild_chat_to_whisper_window_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0 - 1,
					"y" : 73,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "guild_chat_to_whisper_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_GUILD_CHAT_WHISPER,
						},
						{
							"name" : "guild_chat_to_whisper_description",
							"type" : "adjust_text",
							"max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUILD_CHAT_WHISPER_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},
				{
					"name" : "small_whisper_window_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0 - 1,
					"y" : 116,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "small_whisper_window_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : getattr(localeInfo, "GAME_OPTION_SMALL_WHISPER_WINDOW", "Small Version Window"),
						},
						{
							"name" : "small_whisper_window_description",
							"type" : "adjust_text",
							"max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : getattr(localeInfo, "GAME_OPTION_SMALL_WHISPER_WINDOW_DESC", "Hide the conversation list in the Whisper Window."),
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},									

		{
			"name" : "notification_settings_window",
			"type" : "window",
			"x" : 0,
			"y" : 2000,
			"width" : 304,
			"height" : 85,
			"children" :
			(
				{
					"name" : "notification_settings_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR + "option_title.png",
					"children" :
					(
						{
							"name" : "title_notification_settings",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : 0xFFcabdab,
							"outline" : True,
							"text_horizontal_align" : "left",
							"text" : getattr(localeInfo, "GAME_OPTION_NOTIFICATION_SETTINGS", "Notification Settings"),
						},
					),
				},
				{
					"name" : "notification_settings_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color" : 0xFFcebb98,
					"outline" : True,
					"text_horizontal_align" : "left",
					"text" : getattr(localeInfo, "GAME_OPTION_HIDDEN_NOTIFICATIONS", "Hidden Notifications"),
				},
			),
		},

		{
			"name" : "notification_history_button_window",
			"type" : "window",
			"x" : 0,
			"y" : 5+75*6 + WINDOW_SCALE_OPTION_HEIGHT + FONT_OPTION_HEIGHT + 80 + 80 + 150+40,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "notification_history_button_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_notification_history_button",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : getattr(localeInfo, "GAME_OPTION_NOTIFICATION_HISTORY", "Notification History"),
						},
					),
				},
				{
					"name" : "view_notification_history_button_off_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "notification_history_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : getattr(localeInfo, "GAME_OPTION_NOTIFICATION_HISTORY", "Notification History"),
						},
						{
							"name" : "notification_history_button_description",
							"type" : "adjust_text",
							"max_width"	: 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : getattr(localeInfo, "GAME_OPTION_NOTIFICATION_HISTORY_DESC", "Show or hide the notification history button in the game button list."),
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},

		## PAGE_1
		{
			"name" : "pvp_mode_window",
			"type" : "window",
			"x" : 0,
			"y" : 500 + 5,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "pvp_mode",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_pvp",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : uiScriptLocale.OPTION_PVPMODE,
						},
					),
				},
				{
					"name" : "pvp_peace",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_PVPMODE_PEACE,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "pvp_revenge",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_PVPMODE_REVENGE,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "pvp_guild",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_PVPMODE_GUILD,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "pvp_free",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*3,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_PVPMODE_FREE,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
			),
		},
		{
			"name" : "buffi_settings_window",
			"type" : "window",
			"x" : 0,
			"y" : 500 + SMALL_OPTION_HEIGHT,
			"width" : 304,
			"height" : 105,
			"children" :
			(
				{
					"name" : "buffi_settings",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR + "option_title.png",
					"children" :
					(
						{
							"name" : "title_buffi_settings",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : PRIMARY_COLOR,
							"outline" : True,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_BUFFI_SETTINGS,
						},
					),
				},
				{
					"name" : "auto_unequip_buffi_pvp_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "auto_unequip_buffi_pvp_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color" : PRIMARY_COLOR,
							"text" : localeInfo.GAME_OPTION_AUTO_UNEQUIP_BUFFI_PVP,
						},
						{
							"name" : "auto_unequip_buffi_pvp_desc",
							"type" : "adjust_text",
							"max_width" : 245,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"color" : SECONDARY_COLOR,
							"text" : localeInfo.GAME_OPTION_AUTO_UNEQUIP_BUFFI_PVP_DESC,
						},
					),
				},
				{
					"name" : "auto_equip_buffi_pvm_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 73,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "auto_equip_buffi_pvm_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color" : PRIMARY_COLOR,
							"text" : localeInfo.GAME_OPTION_AUTO_EQUIP_BUFFI_PVM,
						},
						{
							"name" : "auto_equip_buffi_pvm_desc",
							"type" : "adjust_text",
							"max_width" : 245,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"color" : SECONDARY_COLOR,
							"text" : localeInfo.GAME_OPTION_AUTO_EQUIP_BUFFI_PVM_DESC,
						},
					),
				},
			),
		},

		{
			"name" : "block_mode_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "block_mode_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_block",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : uiScriptLocale.OPTION_BLOCK,
						},
					),
				},
				{
					"name" : "block_exchange_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_BLOCK_EXCHANGE,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "block_party_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_BLOCK_PARTY,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "block_guild_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_BLOCK_GUILD,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "block_whisper_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*3,
					"y" : 33,
					"text" : uiScriptLocale.OPTION_BLOCK_WHISPER,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "block_friend_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33+20,
					"text" : uiScriptLocale.OPTION_BLOCK_FRIEND,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "block_party_request_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33+20,
					"text" : uiScriptLocale.OPTION_BLOCK_PARTY_REQUEST,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
			),
		},

		{
			"name" : "names_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT+NORMAL_OPTION_HEIGHT,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT+25,
			"children":
			(
				{
					"name" : "names_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_names",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENTITY_NAME,
						},
					),
				},
				{
					"name" : "names_far_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "names_far_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_ENTITY_NAME,
						},
						{
							"name" : "names_far_toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENTITY_NAME_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	

				{
					"name" : "names_monsters_fov_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 72,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "names_monsters_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_MONSTER_NAME,
						},
						{
							"name" : "names_monsters_toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_MONSTER_NAME_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	
			),
		},

		{
			"name" : "damage_window",
			"type" : "window",
			"x" : 0,
			"y" : 255,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT*2+108,
			"children":
			(
				{
					"name" : "show_damage_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_show_damage",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_DMG,
						},
					),
				},
                
				{
					"name" : "subtitle_damage_1",
					"type" : "text",
					"x" : 17,
					"y" : 34,
					"color": 0xFFcebb98,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_SIZE_DMG,
				},

				{
					"name" : "size_damage",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y+3,
				},

				{
					"name" : "subtitle_damage_spacing",
					"type" : "text",
					"x" : 17,
					"y" : 68,
					"color": 0xFFcebb98,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_DAMAGE_SPACING,
				},

				{
					"name" : "damage_spacing",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y+37,
				},

				{
					"name" : "damage_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 112,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "damage_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_DMG,
						},
						{
							"name" : "damage_toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_DMG_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	

				{
					"name" : "damage_monster_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 151,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "damage_monster_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_DMG_MONSTER,
						},
						{
							"name" : "damage_monster_toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_DMG_MONSTER_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	

				{
					"name" : "colored_damage_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 190,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "colored_damage_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_COLORED_DAMAGE,
						},
						{
							"name" : "colored_damage_toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_COLORED_DAMAGE_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	

				{
					"name" : "colored_damage_color_button",
					"type" : "button",
					"x" : OPTION_START_X,
					"y" : 225,
					"text_color" : 0xFFcebb98,
					"text" : localeInfo.GAME_OPTION_DAMAGE_COLORS_OPEN,
					"outline" : True,
					"default_image" : IMG_DIR + "btn_0.png",
					"over_image" : IMG_DIR + "btn_1.png",
					"down_image" : IMG_DIR + "btn_2.png",
				},

			),
		},

		{
			"name" : "stone_scale_window",
			"type" : "window",
			"x" : 0,
			"y" : 528,
			"width":304,
			"height":NORMAL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "stone_scale_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_stone_scale",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SIZE_STONE,
						},
					),
				},
				{
					"name" : "stones_scale_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y,
				},

			),
		},
        
		{
			"name" : "hide_companion_window",
			"type" : "window",
			"x" : 0,
			"y" : 613,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "hide_companion",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_hide_companion",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_HIDE_COMPANION,
						},
					),
				},
				{
					"name" : "hide_mount",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_LARGE_RANGE_X*0,
					"y" : 33,
					"text" : localeInfo.GAME_OPTION_HIDE_COMPANION_MOUNT,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "hide_pet",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_LARGE_RANGE_X*1,
					"y" : 33,
					"text" : localeInfo.GAME_OPTION_HIDE_COMPANION_PET,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "hide_buffi",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_LARGE_RANGE_X*2,
					"y" : 33,
					"text" : localeInfo.GAME_OPTION_HIDE_COMPANION_BUFFI,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
			),
		},

		{
			"name" : "shop_range_window",
			"type" : "window",
			"x" : 0,
			"y" : 683,
			"width":304,
			"height":SLIDER_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "shop_range_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_shop_range",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": PRIMARY_COLOR,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.IKASHOP_PRO_SHOP_RANGE_VIEW_HEADER,
						},
					),
				},
				{
					"name" : "shop_range_slider",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y,
				},
			),
		},

		{
			"name" : "event_stone_window",
			"type" : "window",
			"x" : 0,
			"y" : 948,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "event_stone_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_event_stone",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_EVENT_STONE_APPEARANCE,
						},
					),
				},
				{
					"name" : "event_stone_original", "type" : "radio_button",
					"x" : OPTION_START_X, "y" : 33,
					"text" : localeInfo.GAME_OPTION_EVENT_STONE_ORIGINAL, "text_x" : RADIO_BUTTON_TEXT_X,
					"text_color" : 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png", "over_image" : IMG_DIR + "radio_unselected.png", "down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "event_stone_model_1", "type" : "radio_button",
					"x" : OPTION_START_X + RADIO_BUTTON_RANGE_X, "y" : 33,
					"text" : "Model 1", "text_x" : RADIO_BUTTON_TEXT_X, "text_color" : 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png", "over_image" : IMG_DIR + "radio_unselected.png", "down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "event_stone_model_2", "type" : "radio_button",
					"x" : OPTION_START_X + RADIO_BUTTON_RANGE_X * 2, "y" : 33,
					"text" : "Model 2", "text_x" : RADIO_BUTTON_TEXT_X, "text_color" : 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png", "over_image" : IMG_DIR + "radio_unselected.png", "down_image" : IMG_DIR + "radio_selected.png",
				},
			),
		},

		{
			"name" : "queue_metin_window",
			"type" : "window",
			"x" : 0,
			"y" : 753,
			"width":304,
			"height":145,
			"children":
			(
				{
					"name" : "queue_metin_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_queue_metin",
							"type" : "text",
							"color": 0xFFcabdab,
							"outline": True,
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_QUEUE_METIN_EFFECT,
						},
					),
				},
				{
					"name" : "effect_1",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 31,
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_EFFECT_OPTION.format(1),
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "effect_2",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 31,
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_EFFECT_OPTION.format(2),
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "effect_3",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 31,
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_EFFECT_OPTION.format(3),
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "queue_flash_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 113,
					"color": 0xFFcebb98,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_FLASH,
				},
				{
					"name" : "queue_flash_on",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 114,
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_FLASH_ON,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "queue_flash_off",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 114,
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_FLASH_OFF,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "queue_color_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 55,
					"color" : 0xFFcebb98,
					"outline" : True,
					"text_horizontal_align" : "left",
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_COLOR,
				},
				{
					"name" : "queue_color_button",
					"type" : "button",
					"x" : OPTION_START_X,
					"y" : 78,
					"text_color" : 0xFFcebb98,
					"text" : localeInfo.GAME_OPTION_QUEUE_METIN_COLOR_OPEN,
					"outline" : True,
					"default_image" : IMG_DIR + "btn_0.png",
					"over_image" : IMG_DIR + "btn_1.png",
					"down_image" : IMG_DIR + "btn_2.png",
				},
			),
		},

		{
			"name" : "mining_flash_window",
			"type" : "window",
			"x" : 0,
			"y" : 838,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "mining_flash_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_mining_flash",
							"type" : "text",
							"color": 0xFFcabdab,
							"outline": True,
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_MINING_FLASH,
						},
					),
				},
				{
					"name" : "mining_finish_flash_on",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 31,
					"text" : localeInfo.GAME_OPTION_MINING_FLASH_ON,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
				{
					"name" : "mining_finish_flash_off",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 31,
					"text" : localeInfo.GAME_OPTION_MINING_FLASH_OFF,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"text_color": 0xFFcebb98,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
				},
			),
		},

		{
			"name" : "show_guild_grade_window",
			"type" : "window",
			"x" : 0,
			"y" : 1013,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "show_guild_grade_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_show_guild_grade",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUILD_GRADE,
						},
					),
				},
				{
					"name" : "show_guild_grade_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "show_guild_grade_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_GUILD_GRADE,
						},
						{
							"name" : "show_guild_grade_desc",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUILD_GRADE_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},

		{
			"name" : "show_guild_join_notification_window",
			"type" : "window",
			"x" : 0,
			"y" : 1078,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "show_guild_join_notification_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_show_guild_join_notification",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUILD_JOIN_NOTIFICATION,
						},
					),
				},
				{
					"name" : "show_guild_join_notification_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "show_guild_join_notification_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_GUILD_JOIN_NOTIFICATION,
						},
						{
							"name" : "show_guild_join_notification_desc",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_GUILD_JOIN_NOTIFICATION_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},

		## PAGE_2
		  
		{
			"name" : "anti_aliasing_window",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT*3,
			"width" : 304,
			"height" : SMALL_OPTION_HEIGHT,
			"children" :
			(
				{
					"name" : "anti_aliasing_title_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR + "option_title.png",
					"children" :
					(
						{
							"name" : "anti_aliasing_title",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : 0xFFcabdab,
							"outline" : True,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_ANTI_ALIASING_TITLE,
						},
					),
				},
				{
					"name" : "anti_aliasing_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "anti_aliasing_toggle_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"color" : 0xFFcebb98,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_ANTI_ALIASING_TITLE,
						},
						{
							"name" : "anti_aliasing_toggle_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"color" : SECONDARY_COLOR,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_ANTI_ALIASING_DESCRIPTION,
						},
					),
				},
			),
		},
		{
			"name" : "fps_limit_window",
			"type" : "window",
			"x" : 0,
			"y" : 5,
			"width":304,
			"height":FPS_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "fps_limit_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "fps_limit_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_FPS_LIMIT_TITLE,
						},
					),
				},
				{
					"name" : "fps_limit_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 34,
					"color" : 0xFFcebb98,
					"text" : localeInfo.GAME_OPTION_FPS_LIMIT_LABEL,
				},
				{
					"name" : "fps_limit_60",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 55,
					"text" : localeInfo.GAME_OPTION_FPS_VALUE_60,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "fps_limit_120",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 55,
					"text" : localeInfo.GAME_OPTION_FPS_VALUE_120,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "fps_limit_144",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 55,
					"text" : localeInfo.GAME_OPTION_FPS_VALUE_144,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "fps_limit_200",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*3,
					"y" : 55,
					"text" : localeInfo.GAME_OPTION_FPS_VALUE_200,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
					"text_color": 0xff9f9892
				},
				{
					"name" : "hide_fps_minimap",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 109,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "hide_fps_minimap_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align" : "left",
							"color" : 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_HIDE_FPS_MINIMAP,
						},
					),
				},
				{
					"name" : "hide_fps_minimap_description",
					"type" : "adjust_text",
					"max_width" : 240,
					"x" : OPTION_START_X + TOGGLE_BUTTON_TEXT_X,
					"y" : 126,
					"text_horizontal_align" : "left",
					"color" : SECONDARY_COLOR,
					"text" : localeInfo.GAME_OPTION_HIDE_FPS_MINIMAP_DESC,
				},
			),
		},
		{
			"name" : "shadow_options_window",
			"type" : "window",
			"x" : 0,
			"y" : 70,
			"width" : 304,
			"height" : 112,
			"children" : (
				{
					"name" : "shadow_options_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children" : (
						{
							"name" : "shadow_options_window_title",
							"type" : "text",
							"x" : 17,
							"y" : 8,
							"text" : localeInfo.GRAPHICS_SHADOW_TITLE,
							"color" : 0xFFcebb98,
							"outline" : True,
						},
					),
				},
				{
					"name" : "shadow_options_window_label",
					"type" : "text",
					"x" : 17,
					"y" : 33,
					"text" : localeInfo.GRAPHICS_SHADOW_TITLE,
					"color" : 0xFFcebb98,
				},
				{
					"name" : "shadow_quality_0",
					"type" : "radio_button",
					"x" : 17,
					"y" : 55,
					"text" : localeInfo.GRAPHICS_SHADOW_OFF,
					"text_x" : 25,
					"default_image" : IMG_DIR+"radio_unselected.png",
					"over_image" : IMG_DIR+"radio_unselected.png",
					"down_image" : IMG_DIR+"radio_selected.png",
				},
				{
					"name" : "shadow_quality_1",
					"type" : "radio_button",
					"x" : 162,
					"y" : 55,
					"text" : localeInfo.GRAPHICS_SHADOW_LOW,
					"text_x" : 25,
					"default_image" : IMG_DIR+"radio_unselected.png",
					"over_image" : IMG_DIR+"radio_unselected.png",
					"down_image" : IMG_DIR+"radio_selected.png",
				},
				{
					"name" : "shadow_quality_2",
					"type" : "radio_button",
					"x" : 17,
					"y" : 80,
					"text" : localeInfo.GRAPHICS_SHADOW_MEDIUM,
					"text_x" : 25,
					"default_image" : IMG_DIR+"radio_unselected.png",
					"over_image" : IMG_DIR+"radio_unselected.png",
					"down_image" : IMG_DIR+"radio_selected.png",
				},
				{
					"name" : "shadow_quality_3",
					"type" : "radio_button",
					"x" : 162,
					"y" : 80,
					"text" : localeInfo.GRAPHICS_SHADOW_HIGH,
					"text_x" : 25,
					"default_image" : IMG_DIR+"radio_unselected.png",
					"over_image" : IMG_DIR+"radio_unselected.png",
					"down_image" : IMG_DIR+"radio_selected.png",
				},
			),
		},		
		{
			"name" : "tree_shadow_window",
			"type" : "window",
			"x" : 0,
			"y" : 70,
			"width" : 304,
			"height" : 132,
			"children" : (
				{
					"name" : "tree_shadow_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children" : (
						{
							"name" : "tree_shadow_window_title",
							"type" : "text",
							"x" : 17,
							"y" : 8,
							"text" : localeInfo.GRAPHICS_SHADOW_CASTERS,
							"color" : 0xFFcebb98,
							"outline" : True,
						},
					),
				},
				{
					"name" : "tree_shadow_window_label",
					"type" : "text",
					"x" : 17,
					"y" : 33,
					"text" : localeInfo.GRAPHICS_SHADOW_CASTERS,
					"color" : 0xFFcebb98,
				},
				{
					"name" : "shadow_caster_player",
					"type" : "toggle_button",
					"x" : 17,
					"y" : 55,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" : (
						{
							"name" : "shadow_caster_player_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text" : localeInfo.GRAPHICS_SHADOW_CHARACTERS,
						},
					),
				},
				{
					"name" : "shadow_caster_terrain",
					"type" : "toggle_button",
					"x" : 17,
					"y" : 79,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" : (
						{
							"name" : "shadow_caster_terrain_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text" : localeInfo.GRAPHICS_SHADOW_SCENERY,
						},
					),
				},
				{
					"name" : "shadow_caster_trees",
					"type" : "toggle_button",
					"x" : 17,
					"y" : 103,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" : (
						{
							"name" : "shadow_caster_trees_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text" : localeInfo.GRAPHICS_SHADOW_TREES,
						},
					),
				},
			),
		},
 		{
			"name" : "sun_rays_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "sun_rays_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "sun_rays_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SUN_RAYS_TITLE,
						},
					),
				},
				{
					"name" : "sun_rays_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "sun_rays_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_SUN_RAYS_TITLE,
						},
						{
							"name" : "sun_rays_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SUN_RAYS_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "wet_ground_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "wet_ground_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "wet_ground_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WET_GROUND_TITLE,
						},
					),
				},
				{
					"name" : "wet_ground_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "wet_ground_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_WET_GROUND_TITLE,
						},
						{
							"name" : "wet_ground_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WET_GROUND_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "rain_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "rain_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "rain_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_RAIN_TITLE,
						},
					),
				},
				{
					"name" : "rain_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "rain_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_RAIN_TITLE,
						},
						{
							"name" : "rain_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_RAIN_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "storm_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "storm_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "storm_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_STORM_TITLE,
						},
					),
				},
				{
					"name" : "storm_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "storm_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_STORM_TITLE,
						},
						{
							"name" : "storm_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_STORM_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "shooting_stars_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "shooting_stars_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "shooting_stars_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SHOOTING_STARS_TITLE,
						},
					),
				},
				{
					"name" : "shooting_stars_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "shooting_stars_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_SHOOTING_STARS_TITLE,
						},
						{
							"name" : "shooting_stars_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SHOOTING_STARS_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "water_ripple_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "water_ripple_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "water_ripple_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WATER_RIPPLE_TITLE,
						},
					),
				},
				{
					"name" : "water_ripple_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "water_ripple_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_WATER_RIPPLE_TITLE,
						},
						{
							"name" : "water_ripple_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WATER_RIPPLE_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "enhanced_water_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "enhanced_water_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "enhanced_water_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_ENHANCED_WATER_TITLE,
						},
					),
				},
				{
					"name" : "enhanced_water_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "enhanced_water_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_ENHANCED_WATER_TITLE,
						},
						{
							"name" : "enhanced_water_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_ENHANCED_WATER_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "underwater_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "underwater_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "underwater_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_UNDERWATER_TITLE,
						},
					),
				},
				{
					"name" : "underwater_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "underwater_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_UNDERWATER_TITLE,
						},
						{
							"name" : "underwater_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_UNDERWATER_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "specular_reflection_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "specular_reflection_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "specular_reflection_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SPECULAR_REFLECTION_TITLE,
						},
					),
				},
				{
					"name" : "specular_reflection_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "specular_reflection_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_SPECULAR_REFLECTION_TITLE,
						},
						{
							"name" : "specular_reflection_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_SPECULAR_REFLECTION_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "water_reflection_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "water_reflection_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "water_reflection_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WATER_REFLECTION_TITLE,
						},
					),
				},
				{
					"name" : "water_reflection_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "water_reflection_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_WATER_REFLECTION_TITLE,
						},
						{
							"name" : "water_reflection_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_WATER_REFLECTION_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "local_light_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":110,
			"children":
			(
				{
					"name" : "local_light_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "local_light_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_LOCAL_LIGHT_TITLE,
						},
					),
				},
				{
					"name" : "local_light_toggle",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "local_light_toggle_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_LOCAL_LIGHT_TITLE,
						},
						{
							"name" : "local_light_toggle_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_LOCAL_LIGHT_DESCRIPTION,
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},
		{
			"name" : "sharpness_windows",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT*5,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "sharpness_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "sharpness_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SHARPNESS_TITLE,
						},
					),
				},
				{
					"name" : "sharpness_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y,
				},
			),			
		},

		{
			"name" : "fov_windows",
			"type" : "window",
			"x" : 0,
			"y" : SMALL_OPTION_HEIGHT*6,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "fov_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "fov_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_FOV,
						},
					),
				},
				{
					"name" : "camera_fov_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y,
				},
			),
		},

        {
			"name" : "camera_window",
			"type" : "window",
			"x" : 0,
			"y" : 460,
			"width":304,
			"height":SMALL_OPTION_HEIGHT * 2,
			"children":
			(
				{
					"name" : "camera_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_camera",
							"type" : "text",
							"color": 0xFFcabdab,
							"outline": True,
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"text_horizontal_align":"left",
							"text" : uiScriptLocale.OPTION_CAMERA_DISTANCE,
						},
					),
				},
				{
					"name" : "camera_far_on_off_btn",
					"type" : "toggle_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,

					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "camera_far_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
                            "color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_CAMERA_FAR,
						},
						{
							"name" : "camera_far_toggle2_button_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_CAMERA_FAR_DESC,
							"color" : SECONDARY_COLOR,
						},			
					),
				},	
				{
					"name" : "keyboard_camera_speed_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 77,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : localeInfo.GAME_OPTION_KEYBOARD_CAMERA_SPEED,
				},
				{
					"name" : "keyboard_camera_speed_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : 99,
				},

			),
		},
   
		{
			"name" : "environment_window",
			"type" : "window",
			"x" : 0,
			"y" : 400,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "environment_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "environment_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_TITLE,
						},
					),
				},
				{
					"name" : "night_mode_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "night_mode_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GRAPHICS_SKY_NIGHT,
						},
						{
							"name" : "night_mode_toggle_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"color": 0xff9f9892,
							"text" : localeInfo.GRAPHICS_TOGGLE_HINT,
						},
					),
				},
			),
		},
		{
			"name" : "skybox_window",
			"type" : "window",
			"x" : 0, "y" : 525, "width" : 304, "height" : 90,
			"children" : (
				{
					"name" : "skybox_window_img", "type" : "expanded_image",
					"x" : 0, "y" : 0, "image" : IMG_DIR + "option_title.png",
					"children" : ({
						"name" : "skybox_window_name", "type" : "text",
						"x" : TITLE_IMAGE_TEXT_X, "y" : TITLE_IMAGE_TEXT_Y,
						"text" : localeInfo.GRAPHICS_SKYBOX_TITLE,
					},),
				},
				{
					"name" : "skybox_label", "type" : "text",
					"x" : OPTION_START_X, "y" : 33,
					"text" : localeInfo.GRAPHICS_SKYBOX_TITLE,
				},
				{
					"name" : "skybox_combo_holder", "type" : "window",
					"x" : OPTION_START_X, "y" : 55, "width" : 260, "height" : 22,
				},
			),
		},
		{
			"name" : "speedtree_fx_window",
			"type" : "window",
			"x" : 0,
			"y" : 330,
			"width":304,
			"height":185,
			"children":
			(
				{
					"name" : "speedtree_fx_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "speedtree_fx_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_TITLE,
						},
					),
				},
				{
					"name" : "speedtree_dynamic_light",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "speedtree_dynamic_light_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_LIGHTING,
						},
						{
							"name" : "speedtree_dynamic_light_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_TOGGLE_HINT,
							"color" : SECONDARY_COLOR,
						},
					),
				},
				{
					"name" : "speedtree_wind",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 73,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "speedtree_wind_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_WIND,
						},
						{
							"name" : "speedtree_wind_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GRAPHICS_TOGGLE_HINT,
							"color" : SECONDARY_COLOR,
						},
					),
				},
				{
					"name" : "speedtree_enhanced_foliage",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 113,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "speedtree_enhanced_foliage_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_ENHANCED_FOLIAGE,
						},
						{
							"name" : "speedtree_enhanced_foliage_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_ENHANCED_FOLIAGE_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},
				{
					"name" : "speedtree_falling_leaves",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 153,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "speedtree_falling_leaves_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_FALLING_LEAVES,
						},
						{
							"name" : "speedtree_falling_leaves_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 15,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_SPEEDTREE_FALLING_LEAVES_DESC,
							"color" : SECONDARY_COLOR,
						},
					),
				},

			),
		},
		{
			"name" : "death_cam_window",
			"type" : "window",
			"x" : 0,
			"y" : 265,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "death_cam_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "death_cam_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : "Death Cam",
						},
					),
				},
				{
					"name" : "death_cam_effect",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "death_cam_effect_text",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : -2,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : "Death Cam",
						},
						{
							"name" : "death_cam_effect_description",
							"type" : "adjust_text",
							"max_width" : 285,
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"text" : "Activate / Deactivate the death camera.",
							"color" : SECONDARY_COLOR,
						},
					),
				},
			),
		},

		{
			"name" : "remove_fog_window",
			"type" : "window",
			"x" : 0,
			"y" : 465,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "remove_fog_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "remove_fog_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : "Remove Fog",
						},
					),
				},
				{
					"name" : "remove_fog_button",
					"type" : "toggle_button",
					"x" : OPTION_START_X,
					"y" : 33,
					"default_image" : IMG_DIR + "toggle_unselected.png",
					"over_image" : IMG_DIR + "toggle_unselected.png",
					"down_image" : IMG_DIR + "toggle_selected.png",
					"children" :
					(
						{
							"name" : "remove_fog_button_title",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 0,
							"text_horizontal_align":"left",
							"color": 0xFFcebb98,
							"text" : "Remove Fog",
						},
						{
							"name" : "remove_fog_toggle_description",
							"type" : "text",
							"x" : TOGGLE_BUTTON_TEXT_X,
							"y" : 13,
							"text_horizontal_align":"left",
							"color": 0xff9f9892,
							"text" : "Active / Desactive",
						},
					),
				},
			),
		},

		{
			"name" : "environment_darkness_window",
			"type" : "window",
			"x" : 0,
			"y" : 530,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "environment_darkness_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "environment_darkness_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_DARKNESS_TITLE,
						},
					),
				},
				{
					"name" : "environment_darkness_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y,
				},
			),
		},

		{
			"name" : "environment_saturation_window",
			"type" : "window",
			"x" : -10000,
			"y" : 595,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "environment_saturation_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "environment_saturation_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_SATURATION_TITLE,
						},
					),
				},
				{
					"name" : "environment_saturation_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y,
				},
			),
		},

		{
			"name" : "environment_contrast_window",
			"type" : "window",
			"x" : -10000,
			"y" : 660,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "environment_contrast_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "environment_contrast_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_CONTRAST_TITLE,
						},
					),
				},
				{
					"name" : "environment_contrast_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y,
				},
			),
		},

		{
			"name" : "environment_temperature_window",
			"type" : "window",
			"x" : -10000,
			"y" : 725,
			"width":304,
			"height":SMALL_OPTION_HEIGHT,
			"children":
			(
				{
					"name" : "environment_temperature_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "environment_temperature_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_TEMPERATURE_TITLE,
						},
					),
				},
				{
					"name" : "environment_temperature_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y,
				},
			),
		},

		{
			"name" : "environment_color_window",
			"type" : "window",
			"x" : 0,
			"y" : 595,
			"width" : 304,
			"height" : NORMAL_OPTION_HEIGHT,
			"children" :
			(
				{
					"name" : "environment_color_window_img",
					"type" : "expanded_image",
					"x" : 0,
					"y" : 0,
					"image" : IMG_DIR + "option_title.png",
					"children" :
					(
						{
							"name" : "environment_color_window_name",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : 0xFFcabdab,
							"outline" : True,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_ENVIRONMENT_COLOR_TITLE,
						},
					),
				},
				{
					"name" : "environment_color_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color" : 0xFFcebb98,
					"outline" : True,
					"text_horizontal_align" : "left",
					"text" : localeInfo.GAME_OPTION_ENVIRONMENT_COLOR_TITLE,
				},
				{
					"name" : "environment_color_button",
					"type" : "button",
					"x" : OPTION_START_X,
					"y" : 52,
					"text_color" : 0xFFcebb98,
					"text" : localeInfo.GAME_OPTION_ENVIRONMENT_COLOR_OPEN,
					"outline" : True,
					"default_image" : IMG_DIR + "btn_0.png",
					"over_image" : IMG_DIR + "btn_1.png",
					"down_image" : IMG_DIR + "btn_2.png",
				},
			),
		},

		{
			"name" : "volume_window",
			"type" : "window",
			"x" : 0,
			"y" : 5,
			"width":304,
			"height":SLIDER_OPTION_HEIGHT*3,
			"children":
			(
				{
					"name" : "volume_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_sound",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : "Volume", #uiScriptLocale.OPTION_SOUND
						},
					),
				},
				{
					"name" : "sound_text",
					"type" : "text",
					"text_horizontal_align":"left",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y-20,
					"text":uiScriptLocale.OPTION_SOUND,
					"color" : PRIMARY_COLOR,
				},		
				{
					"name" : "sound_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y,
				},
				{
					"name" : "music_text",
					"type" : "text",
					"text_horizontal_align":"left",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y*2-20,
					"text":uiScriptLocale.OPTION_MUSIC,
					"color" : PRIMARY_COLOR,
				},		
				{
					"name" : "music_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y*2,
				},
				{
					"name" : "environment_volume_text",
					"type" : "text",
					"text_horizontal_align" : "left",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y*3-20,
					"text" : localeInfo.GAME_OPTION_ENVIRONMENT_SONG,
					"color" : PRIMARY_COLOR,
				},
				{
					"name" : "environment_volume_controller",
					"type" : "sliderbar_advancedgameoptions",
					"x" : OPTION_START_X,
					"y" : SLIDER_START_Y*3,
				},
			),
		},

		{
			"name" : "bgm_window",
			"type" : "window",
			"x" : 0,
			"y" : SLIDER_OPTION_HEIGHT*3,
			"width":304,
			"height":SLIDER_OPTION_HEIGHT + 15,
			"children":
			(
				{
					"name" : "bgm_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_bgm",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : uiScriptLocale.MUSICLIST_TITLE,
						},
					),
				},
				{
					"name" : "bgm_file",
					"type" : "text",
					"text_horizontal_align":"left",
					"x" : OPTION_START_X,
					"y" : BUTTON_START_Y+5,
					"text":uiScriptLocale.OPTION_MUSIC_DEFAULT_THEMA,
					"color" : PRIMARY_COLOR,
				},
				{
					"name" : "bgm_button",
					"type" : "toggle_button",
					"x" : 155,
					"y" : BUTTON_START_Y,
					"text" : uiScriptLocale.OPTION_MUSIC_CHANGE,
					"default_image" : IMG_DIR + "btn_0.png",
					"over_image" : IMG_DIR + "btn_1.png",
					"down_image" : IMG_DIR + "btn_2.png",
				},
			),
		},

		{
			"name" : "language_window",
			"type" : "window",
			"x" : 0,
			"y" : 5,
			"width":304,
			"height":SMALL_OPTION_HEIGHT*2,
			"children":
			(
				{
					"name" : "lang_mode",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_lang",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : "Client Language",
						},
					),
				},
				{
					"name" : "language_en",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33,
					"text" : uiScriptLocale.LANGUAGE_EN,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "language_pt",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33,
					"text" : uiScriptLocale.LANGUAGE_PT,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "language_es",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33,
					"text" : uiScriptLocale.LANGUAGE_ES,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "language_fr",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*3,
					"y" : 33,
					"text" : uiScriptLocale.LANGUAGE_FR,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
                
				{
					"name" : "language_de",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33+25,
					"text" : uiScriptLocale.LANGUAGE_DE,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "language_ro",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33+25,
					"text" : uiScriptLocale.LANGUAGE_RO,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "language_pl",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33+25,
					"text" : uiScriptLocale.LANGUAGE_PL,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "language_it",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*3,
					"y" : 33+25,
					"text" : uiScriptLocale.LANGUAGE_IT,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},

				{
					"name" : "language_cz",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*0,
					"y" : 33+25*2,
					"text" : uiScriptLocale.LANGUAGE_CZ,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "language_HU",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*1,
					"y" : 33+25*2,
					"text" : uiScriptLocale.LANGUAGE_HU,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "language_tr",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*2,
					"y" : 33+25*2,
					"text" : uiScriptLocale.LANGUAGE_TR,
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},
				{
					"name" : "language_ae",
					"type" : "radio_button",
					"x" : OPTION_START_X+RADIO_BUTTON_RANGE_X*3,
					"y" : 33+25*2,
					"text" : getattr(uiScriptLocale, "LOCALE_NAME_DICT", {}).get("ae", "Arabic"),
					"text_x" : RADIO_BUTTON_TEXT_X,
					"default_image" : IMG_DIR + "radio_unselected.png",
					"over_image" : IMG_DIR + "radio_unselected.png",
					"down_image" : IMG_DIR + "radio_selected.png",
                    "text_color": 0xff9f9892
				},

			),
		},

			{
				"name" : "hide_effects_except_damage_window",
			"type" : "window",
			"x" : 0,
			"y" : 0,
			"width" : 304,
			"height" : SMALL_OPTION_HEIGHT,
			"children" :
			(
				{
					"name" : "hide_effects_except_damage_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR + "option_title.png",
					"children" :
					(
						{
							"name" : "title_hide_effects_except_damage",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color" : 0xFFcabdab,
							"outline" : True,
							"text_horizontal_align" : "left",
							"text" : localeInfo.GAME_OPTION_HIDE_EFFECTS_EXCEPT_DAMAGE,
						},
					),
				},
				),
			},

			{
				"name" : "auto_cape_after_warp_window",
				"type" : "window",
				"x" : 0,
				"y" : 0,
				"width" : 304,
				"height" : NORMAL_OPTION_HEIGHT,
				"children" :
				(
					{
						"name" : "auto_cape_after_warp_title_img",
						"type" : "expanded_image",
						"x" : 5,
						"y" : 0,
						"image" : IMG_DIR + "option_title.png",
						"children" :
						(
							{
								"name" : "title_auto_cape_after_warp",
								"type" : "text",
								"x" : TITLE_IMAGE_TEXT_X,
								"y" : TITLE_IMAGE_TEXT_Y,
								"color" : PRIMARY_COLOR,
								"outline" : True,
								"text_horizontal_align" : "left",
								"text" : localeInfo.GAME_OPTION_AUTO_CAPE_AFTER_WARP,
							},
						),
					},
					{
						"name" : "auto_cape_after_warp_on_off_btn",
						"type" : "toggle_button",
						"x" : OPTION_START_X,
						"y" : 33,
						"default_image" : IMG_DIR + "toggle_unselected.png",
						"over_image" : IMG_DIR + "toggle_unselected.png",
						"down_image" : IMG_DIR + "toggle_selected.png",
						"children" :
						(
							{
								"name" : "auto_cape_after_warp_button_title",
								"type" : "text",
								"x" : TOGGLE_BUTTON_TEXT_X,
								"y" : -2,
								"text_horizontal_align" : "left",
								"color" : 0xFFcebb98,
								"text" : localeInfo.GAME_OPTION_AUTO_CAPE_AFTER_WARP,
							},
							{
								"name" : "auto_cape_after_warp_button_description",
								"type" : "adjust_text",
								"max_width" : 285,
								"x" : TOGGLE_BUTTON_TEXT_X,
								"y" : 13,
								"text_horizontal_align" : "left",
								"text" : localeInfo.GAME_OPTION_AUTO_CAPE_AFTER_WARP_DESC,
								"color" : SECONDARY_COLOR,
							},
						),
					},
				),
			},


	    ),
}
