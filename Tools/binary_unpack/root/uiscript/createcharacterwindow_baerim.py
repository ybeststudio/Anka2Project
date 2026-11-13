import localeInfo
import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/"
FACE_PATH = "d:/ymir work/ui/intro/public_intro/face/"
SELECT_PATH = "d:/ymir work/ui/intro/select/"
INTRO_SELECT_PATH = "d:/ymir work/ui/intro_select/"
LEGACY_SELECT_PATH = "d:/ymir work/ui/login/select_interface/"

BOARD_X = SCREEN_WIDTH * 25 / 800
BOARD_Y = SCREEN_HEIGHT * 156 / 600

X_GAP = 11
Y_GAP = 12
SELECT_BTN_X = X_GAP + 4
SELECT_BTN_Y = 10
SELECT_BTN_GAP = 44

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
		"default_image": default_image,
		"over_image": over_image,
		"down_image": down_image,
	}

	if h_align in ("left", "center", "right"):
		button["horizontal_align"] = h_align
		button["text_horizontal_align"] = h_align

	if v_align in ("top", "center", "bottom"):
		button["vertical_align"] = v_align
		button["text_vertical_align"] = v_align

	return button

def create_public_button(name, x, y, default_image, over_image, down_image):
	return create_button(
		name,
		x,
		y,
		ROOT_PATH + "public_intro_btn/" + default_image,
		ROOT_PATH + "public_intro_btn/" + over_image,
		ROOT_PATH + "public_intro_btn/" + down_image,
	)

def create_radio_button(name, x, y, default_image, over_image, down_image):
	return {
		"name": name,
		"type": "radio_button",
		"x": x,
		"y": y,
		"default_image": ROOT_PATH + "public_intro_btn/" + default_image,
		"over_image": ROOT_PATH + "public_intro_btn/" + over_image,
		"down_image": ROOT_PATH + "public_intro_btn/" + down_image,
	}

def create_slot(slot_id, y_position, face_image, label):
	return {
		"name": "slot_%d" % slot_id,
		"type": "radio_button",
		"x": SELECT_BTN_X,
		"y": y_position,
		"default_image": ROOT_PATH + "public_intro_btn/150x35_createbtn_01.sub",
		"over_image": ROOT_PATH + "public_intro_btn/150x35_createbtn_02.sub",
		"down_image": ROOT_PATH + "public_intro_btn/150x35_createbtn_03.sub",
		"children": [
			{
				"name": "race_%d" % slot_id,
				"type": "image",
				"style": ("not_pick",),
				"x": -6,
				"y": -4,
				"image": FACE_PATH + face_image,
			},
			{
				"name": "race_name_%d" % slot_id,
				"type": "image",
				"style": ("not_pick",),
				"x": -1000,
				"y": -1000,
				"image": LEGACY_SELECT_PATH + "name_char/0.png",
			},
			{
				"name": "race_label_%d" % slot_id,
				"type": "text",
				"style": ("not_pick",),
				"x": 138,
				"y": 10,
				"text": label,
				"fontname": "Tahoma:12",
				"outline": True,
				"text_horizontal_align": "right",
				"color": 0xffc58665,
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
		"name": "CreateCharacterWindow",
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
				"image": SELECT_PATH + "empire/background/empire_jinno.sub",
			},
			{
				"name": "character_render_window",
				"type": "window",
				"x": 270,
				"y": 0,
				"width": SCREEN_WIDTH - 540,
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
								"image": SELECT_PATH + "empire/empireflag_c.sub",
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
							create_slot(0, SELECT_BTN_Y, "face_warrior_m_01.sub", localeInfo.JOB_WARRIOR),
							create_slot(1, SELECT_BTN_Y + SELECT_BTN_GAP, "face_assassin_m_01.sub", localeInfo.JOB_ASSASSIN),
							create_slot(2, SELECT_BTN_Y + SELECT_BTN_GAP * 2, "face_sura_m_01.sub", localeInfo.JOB_SURA),
							create_slot(3, SELECT_BTN_Y + SELECT_BTN_GAP * 3, "face_shaman_m_01.sub", localeInfo.JOB_SHAMAN),
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
				"height": (Y_GAP * 2) + 48,
				"children": [
					{
						"name": "SelectSex",
						"type": "image",
						"x": X_GAP - 1,
						"y": Y_GAP,
						"image": ROOT_PATH + "public_intro_btn/88x47_btn.sub",
						"children": [
							create_radio_button("btn_gender_02", 5, 5, "sex_m_btn_01.sub", "sex_m_btn_02.sub", "sex_m_btn_03.sub"),
							create_radio_button("btn_gender_01", 46, 5, "sex_w_btn_01.sub", "sex_w_btn_02.sub", "sex_w_btn_03.sub"),
						],
					},
					{
						"name": "SelectShape",
						"type": "image",
						"x": LEFT_WIDTH + X_GAP - 87,
						"y": Y_GAP,
						"image": ROOT_PATH + "public_intro_btn/88x47_btn.sub",
						"children": [
							create_radio_button("btn_shape_01", 5, 5, "shape1_btn_01.sub", "shape1_btn_02.sub", "shape1_btn_03.sub"),
							create_radio_button("btn_shape_02", 46, 5, "shape2_btn_01.sub", "shape2_btn_02.sub", "shape2_btn_03.sub"),
						],
					},
				],
			},

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
							create_public_button("prev_button", 122, 250, "prev_btn_01.sub", "prev_btn_02.sub", "prev_btn_01.sub"),
							create_public_button("next_button", 152, 250, "next_btn_01.sub", "next_btn_02.sub", "next_btn_01.sub"),
						],
					},
					create_stat("hp", uiScriptLocale.CREATE_HP, STAT_Y, "gauge_hp", "text_value_hp", "red"),
					create_stat("int", uiScriptLocale.CREATE_SP, STAT_Y + STAT_GAP, "gauge_int", "text_value_int", "pink"),
					create_stat("str", uiScriptLocale.CREATE_ATT_GRADE, STAT_Y + STAT_GAP * 2, "gauge_str", "text_value_str", "purple"),
					create_stat("dex", uiScriptLocale.CREATE_DEX_GRADE, STAT_Y + STAT_GAP * 3, "gauge_dex", "text_value_dex", "blue"),
				],
			},

			{
				"name": "textAlpha",
				"type": "image",
				"x": SCREEN_WIDTH / 2 - 153,
				"y": START_Y - 31,
				"image": ROOT_PATH + "public_intro_btn/textalpha.sub",
				"children": [
					{
						"name": "character_name",
						"type": "text",
						"x": 58,
						"y": 6,
						"r": 0.7843,
						"g": 0.7843,
						"b": 0.7843,
						"text": uiScriptLocale.CREATE_NAME,
						"text_horizontal_align": "center",
					},
					{
						"name": "character_name_slot",
						"type": "image",
						"x": 97,
						"y": 4,
						"image": "d:/ymir work/ui/public/parameter_slot_04.sub",
					},
					{
						"name": "edit_name",
						"type": "editline",
						"x": 100,
						"y": 6,
						"input_limit": 12,
						"width": 90,
						"height": 20,
					},
				],
			},
			create_public_button("btn_create", SCREEN_WIDTH / 2 - 107, START_Y, "addcharacter_btn_01.sub", "addcharacter_btn_02.sub", "addcharacter_btn_03.sub"),
			create_public_button("btn_exit", SCREEN_WIDTH / 2 + 32, START_Y, "cancel_btn_01.sub", "cancel_btn_02.sub", "cancel_btn_03.sub"),
		],
	}

window = create_window()
