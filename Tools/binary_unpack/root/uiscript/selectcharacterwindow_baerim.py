import localeInfo
import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/"
FACE_PATH = "d:/ymir work/ui/intro/public_intro/face/"
LOCALE_SELECT_PATH = "d:/ymir work/ui/intro/select/"
INTRO_SELECT_PATH = "d:/ymir work/ui/intro_select/"

BOARD_X = SCREEN_WIDTH * 25 / 800
BOARD_Y = SCREEN_HEIGHT * 156 / 600

X_GAP = 11
Y_GAP = 12
SELECT_BTN_X = X_GAP + 4
SELECT_BTN_Y = 10
SELECT_BTN_GAP = 44
FACE_X = 7
FACE_Y = 4

LEFT_WIDTH = 175
LEFT_BOARD_WIDTH = LEFT_WIDTH + (X_GAP * 2) + 1
RIGHT_WIDTH = 180
RIGHT_BOARD_WIDTH = RIGHT_WIDTH + (X_GAP * 2) + 1

NAME_X = 40
NAME_Y = 82
NAME_SCALE_X = 0.27
NAME_SCALE_Y = 0.27
FLAG_SCALE_X = 0.45
FLAG_SCALE_Y = 0.45

STAT_X = X_GAP + 3
STAT_Y = 286
STAT_GAP = 18
STAT_BAR_X = 40
STAT_BAR_WIDTH = 105
STAT_TEXT_WIDTH = 21
STAT_TEXT_HEIGHT = 13

START_Y = SCREEN_HEIGHT - SCREEN_HEIGHT * 70 / 600

def create_button(name, x, y, default_image, over_image, down_image, h_align=None, v_align=None):
	button = {
		"name": name,
		"type": "button",
		"x": x,
		"y": y,
		"default_image": ROOT_PATH + "public_intro_btn/" + default_image,
		"over_image": ROOT_PATH + "public_intro_btn/" + over_image,
		"down_image": ROOT_PATH + "public_intro_btn/" + down_image,
	}

	if h_align in ("left", "center", "right"):
		button["horizontal_align"] = h_align
		button["text_horizontal_align"] = h_align

	if v_align in ("top", "center", "bottom"):
		button["vertical_align"] = v_align
		button["text_vertical_align"] = v_align

	return button

def create_empty_slot(slot_id, y_position):
	return {
		"name": "empty_slot_%d" % slot_id,
		"type": "image",
		"x": SELECT_BTN_X,
		"y": y_position,
		"image": ROOT_PATH + "public_intro_btn/emptyslot_btn.sub",
	}

def create_character_slot(slot_id, y_position):
	return {
		"name": "slot_%d" % slot_id,
		"type": "radio_button",
		"x": SELECT_BTN_X,
		"y": y_position,
		"default_image": ROOT_PATH + "public_intro_btn/150x35_selectbtn_01.sub",
		"over_image": ROOT_PATH + "public_intro_btn/150x35_selectbtn_02.sub",
		"down_image": ROOT_PATH + "public_intro_btn/150x35_selectbtn_03.sub",
		"children": [
			{
				"name": "race_%d" % slot_id,
				"type": "image",
				"style": ("not_pick",),
				"x": -FACE_X,
				"y": -FACE_Y,
				"image": FACE_PATH + "face_warrior_m_01.sub",
			},
			{
				"name": "text_playername_new%d" % slot_id,
				"type": "text",
				"x": 138,
				"y": 4,
				"text": "",
				"text_horizontal_align": "right",
				"color": 0xffc58665,
			},
			{
				"name": "text_playertime_new%d" % slot_id,
				"type": "text",
				"x": -100,
				"y": -100,
				"text": "",
			},
			{
				"name": "text_playerguild_new%d" % slot_id,
				"type": "text",
				"x": -100,
				"y": -100,
				"text": "",
			},
			{
				"name": "text_levelvalue_new%d" % slot_id,
				"type": "text",
				"x": 138,
				"y": 20,
				"text": "",
				"text_horizontal_align": "right",
				"color": 0xfffad388,
			},
		],
	}

def create_stat(name, label, y_position, gauge_name, value_name, color):
	return {
		"name": name,
		"type": "text",
		"x": STAT_X,
		"y": y_position,
		"text": label,
		"children": [
			{
				"name": gauge_name,
				"type": "gauge",
				"x": STAT_BAR_X,
				"y": 4,
				"width": STAT_BAR_WIDTH,
				"color": color,
			},
			{
				"name": name + "_slot",
				"type": "slotbar",
				"x": STAT_BAR_WIDTH + STAT_BAR_X + 7,
				"y": -1,
				"width": STAT_TEXT_WIDTH,
				"height": STAT_TEXT_HEIGHT,
				"children": [
					{
						"name": value_name,
						"type": "text",
						"x": 0,
						"y": 1,
						"all_align": "center",
						"text": "0",
					},
				],
			},
		],
	}

def create_class_name(name, image):
	return {
		"name": name,
		"type": "expanded_image",
		"x": BOARD_X + NAME_X,
		"y": BOARD_Y - NAME_Y,
		"x_scale": NAME_SCALE_X,
		"y_scale": NAME_SCALE_Y,
		"image": INTRO_SELECT_PATH + image,
	}

def create_window():
	return {
		"name": "SelectCharacterWindow",
		"x": 0,
		"y": 0,
		"width": SCREEN_WIDTH,
		"height": SCREEN_HEIGHT,
		"children": [
			{
				"name": "Background",
				"type": "expanded_image",
				"x": 0,
				"y": 0,
				"x_scale": float(SCREEN_WIDTH) / 1024.0,
				"y_scale": float(SCREEN_HEIGHT) / 768.0,
				"image": LOCALE_SELECT_PATH + "empire/background/empire_jinno.sub",
			},
			{
				"name": "character_render_window",
				"type": "window",
				"x": 270,
				"y": 0,
				"width": SCREEN_WIDTH - 270,
				"height": SCREEN_HEIGHT,
			},

			create_class_name("name_warrior", "war.png"),
			create_class_name("name_assassin", "ninja.png"),
			create_class_name("name_sura", "sura.png"),
			create_class_name("name_shaman", "shaman.png"),

			{
				"name": "content_left",
				"type": "thinboard_gold",
				"x": BOARD_X,
				"y": BOARD_Y,
				"width": LEFT_BOARD_WIDTH,
				"height": 305,
				"children": [
					{
						"name": "EmpireNameSlot",
						"type": "thinboard_circle",
						"x": X_GAP,
						"y": Y_GAP,
						"width": LEFT_WIDTH,
						"height": 42,
						"children": [
							{
								"name": "kingdom_image",
								"type": "expanded_image",
								"x": 4,
								"y": 3,
								"x_scale": FLAG_SCALE_X,
								"y_scale": FLAG_SCALE_Y,
								"image": LOCALE_SELECT_PATH + "empire/empireflag_c.sub",
							},
							{
								"name": "kingdom",
								"type": "text",
								"x": 29,
								"y": 0,
								"text": "",
								"fontsize": "LARGE",
								"all_align": "center",
							},
						],
					},
					{
						"name": "SelectJobSlot",
						"type": "thinboard_circle",
						"x": X_GAP,
						"y": 61,
						"width": LEFT_WIDTH,
						"height": 231,
						"children": [
							create_empty_slot(0, SELECT_BTN_Y),
							create_empty_slot(1, SELECT_BTN_Y + SELECT_BTN_GAP),
							create_empty_slot(2, SELECT_BTN_Y + SELECT_BTN_GAP * 2),
							create_empty_slot(3, SELECT_BTN_Y + SELECT_BTN_GAP * 3),
							create_character_slot(0, SELECT_BTN_Y),
							create_character_slot(1, SELECT_BTN_Y + SELECT_BTN_GAP),
							create_character_slot(2, SELECT_BTN_Y + SELECT_BTN_GAP * 2),
							create_character_slot(3, SELECT_BTN_Y + SELECT_BTN_GAP * 3),
						],
					},
				],
			},
			{
				"name": "select_lower",
				"type": "thinboard_gold",
				"x": BOARD_X,
				"y": BOARD_Y + 308,
				"width": LEFT_BOARD_WIDTH,
				"height": (Y_GAP * 2) + 27,
				"children": [
					create_button("btn_create", X_GAP + 4, Y_GAP, "plus_btn_01.sub", "plus_btn_02.sub", "plus_btn_03.sub"),
					create_button("btn_delete", LEFT_WIDTH + X_GAP - 58, Y_GAP, "minus_btn_01.sub", "minus_btn_02.sub", "minus_btn_03.sub"),
				],
			},

			# country flag picker under the lower board (Baerim __CreateCountryFlagControls); the box inside is built in code
			{
				"name": "country_flag_board",
				"type": "thinboard_gold",
				"x": BOARD_X,
				"y": BOARD_Y + 308 + (Y_GAP * 2) + 27 + 4,
				"width": LEFT_BOARD_WIDTH,
				"height": 36,
			},

			create_button("btn_login", SCREEN_WIDTH / 2 - 107, START_Y, "start_btn_01.sub", "start_btn_02.sub", "start_btn_03.sub"),
			create_button("btn_exit", SCREEN_WIDTH / 2 + 32, START_Y, "cancel_btn_01.sub", "cancel_btn_02.sub", "cancel_btn_03.sub"),

			{
				"name": "content_right",
				"type": "thinboard_gold",
				"x": SCREEN_WIDTH - BOARD_X - RIGHT_BOARD_WIDTH,
				"y": BOARD_Y,
				"width": RIGHT_BOARD_WIDTH,
				"height": 364,
				"children": [
					{
						"name": "raceName_Btn",
						"type": "image",
						"x": 10,
						"y": -17,
						"image": ROOT_PATH + "public_intro_btn/racename_btn.sub",
						"children": [
							{
								"name": "DiscFace",
								"type": "image",
								"x": 4,
								"y": -23,
								"image": "icon/face/warrior_m.png",
							},
							{
								"name": "raceName_Text",
								"type": "text",
								"x": 100,
								"y": 5,
								"r": 0.7843,
								"g": 0.7843,
								"b": 0.7843,
								"text": localeInfo.JOB_WARRIOR,
								"text_horizontal_align": "center",
							},
						],
					},
					{
						"name": "text_board",
						"type": "thinboard_circle",
						"x": X_GAP,
						"y": Y_GAP,
						"width": RIGHT_WIDTH,
						"height": 270,
						"children": [
							create_button("prev_button", 122, 250, "prev_btn_01.sub", "prev_btn_02.sub", "prev_btn_01.sub"),
							create_button("next_button", 152, 250, "next_btn_01.sub", "next_btn_02.sub", "next_btn_01.sub"),
						],
					},
					create_stat("hp", uiScriptLocale.CREATE_HP, STAT_Y, "gauge_hp", "text_value_hp", "red"),
					create_stat("int", uiScriptLocale.CREATE_SP, STAT_Y + STAT_GAP, "gauge_int", "text_value_int", "pink"),
					create_stat("str", uiScriptLocale.CREATE_ATT_GRADE, STAT_Y + STAT_GAP * 2, "gauge_str", "text_value_str", "purple"),
					create_stat("dex", uiScriptLocale.CREATE_DEX_GRADE, STAT_Y + STAT_GAP * 3, "gauge_dex", "text_value_dex", "blue"),
				],
			},

			create_button("arrow_0", BOARD_X + 58, BOARD_Y + 372, "prev_btn_01.sub", "prev_btn_02.sub", "prev_btn_01.sub"),
			create_button("arrow_1", BOARD_X + 128, BOARD_Y + 372, "next_btn_01.sub", "next_btn_02.sub", "next_btn_01.sub"),
		],
	}

window = create_window()
