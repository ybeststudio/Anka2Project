import uiScriptLocale, localeInfo

IMG_DIR = "d:/ymir work/ui/game/advanced_game_options/"

TITLE_IMAGE_TEXT_X = 58
TITLE_IMAGE_TEXT_Y = 5

OPTION_START_X = 17
SLIDER_POSITION_X = 50

SLIDER_START_Y = 40
BUTTON_START_Y = 33
BUTTON_NEXT_Y = 20

RADIO_BUTTON_RANGE_X = 80
TOGGLE_BUTTON_RANGE_X = 65

RADIO_BUTTON_TEXT_X = 25
TOGGLE_BUTTON_TEXT_X = 45

SMALL_OPTION_HEIGHT = 65
NORMAL_OPTION_HEIGHT = 80
SLIDER_OPTION_HEIGHT = 65
FONT_OPTION_HEIGHT = 145
WINDOW_SCALE_OPTION_HEIGHT = 105

PRIMARY_COLOR = 0xffd0c2b7
SECONDARY_COLOR = 0xff9f9892

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
							"type" : "adjust_text",
                            "max_width"	: 285,
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
							"type" : "adjust_text",
							"max_width"	: 285,
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
							"type" : "adjust_text",
                            "max_width"	: 285,
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
							"type" : "adjust_text",
                            "max_width"	: 285,
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
							"type" : "adjust_text",
                            "max_width"	: 285,
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
							"type" : "adjust_text",
                            "max_width"	: 285,
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
			"height":FONT_OPTION_HEIGHT + 35,
			"children":
			(
				{
					"name" : "font_change_title_img",
					"type" : "expanded_image",
					"x" : 5,
					"y" : 0,
					"image" : IMG_DIR+"option_title.png",
					"children":
					(
						{
							"name" : "title_font_change",
							"type" : "text",
							"x" : TITLE_IMAGE_TEXT_X,
							"y" : TITLE_IMAGE_TEXT_Y,
							"color": 0xFFcabdab,
							"outline": True,
							"text_horizontal_align":"left",
							"text" : localeInfo.GAME_OPTION_FONT,
						},
					),
				},
				{
					"name" : "font_name_label",
					"type" : "text",
					"x" : OPTION_START_X,
					"y" : 33,
					"color": 0xFFcebb98,
					"outline": True,
					"text_horizontal_align":"left",
					"text" : "Font Name",
				},
				{
					"name" : "font_btn_0",
					"type" : "radio_button",
					"x" : OPTION_START_X,
					"y" : 55,
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
					"y" : 55,
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
					"y" : 85,
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
					"y" : 130,
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
					"y" : 113,
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
			"name" : "show_guild_join_notification_window",
			"type" : "window",
			"x" : 0,
			"y" : 2000,
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
							"type" : "adjust_text",
							"max_width" : 285,
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
	),
}

import advanced_options_reference
advanced_options_reference.ApplyLayout(window, 0)
