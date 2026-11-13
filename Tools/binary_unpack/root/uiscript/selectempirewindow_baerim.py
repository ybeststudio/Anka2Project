import localeInfo
import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/"
EMPIRE_PATH = "d:/ymir work/ui/intro/empire/"
SELECT_BUTTON_PATH = "d:/ymir work/ui/intro/select/"

FLAG_SCALE_X = 0.5
FLAG_SCALE_Y = 0.5

ATLAS_X = SCREEN_WIDTH / 2 - 340
ATLAS_Y = SCREEN_HEIGHT * 180 / 600
BOARD_X = SCREEN_WIDTH - 201 - SCREEN_WIDTH * 25 / 800
BOARD_Y = ATLAS_Y + 120
BOARD_HEIGHT = 72
BOARD_BUTTON_Y = BOARD_Y + BOARD_HEIGHT + 8

def create_button(name, x, y, default_image, over_image, down_image):
	return {
		"name": name,
		"type": "button",
		"x": x,
		"y": y,
		"default_image": default_image,
		"over_image": over_image,
		"down_image": down_image,
	}

window = {
	"name": "SelectEmpireWindow",
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
			"image": EMPIRE_PATH + "background/empire_select.jpg",
		},
		{
			"name": "desc_phase",
			"type": "image",
			"x": 0,
			"y": SCREEN_HEIGHT * 156 / 600 - 129,
			"image": ROOT_PATH + "public_intro_btn/descphase_btn.sub",
			"children": [
				{
					"name": "desc_phase_text",
					"type": "text",
					"x": 18,
					"y": 7,
					"r": 0.7843,
					"g": 0.7843,
					"b": 0.7843,
					"text": uiScriptLocale.EMPIRE_SELECT_TITLE,
					"fontsize": "LARGE",
				},
			],
		},
		{
			"name": "Atlas",
			"type": "image",
			"x": ATLAS_X,
			"y": ATLAS_Y,
			"image": EMPIRE_PATH + "atlas.sub",
			"children": [
				{
					"name": "EmpireArea_A",
					"type": "expanded_image",
					"x": 43,
					"y": 201,
					"image": EMPIRE_PATH + "empirearea_a.sub",
				},
				{
					"name": "EmpireArea_B",
					"type": "expanded_image",
					"x": 17,
					"y": 16,
					"image": EMPIRE_PATH + "empirearea_b.sub",
				},
				{
					"name": "EmpireArea_C",
					"type": "expanded_image",
					"x": 314,
					"y": 33,
					"image": EMPIRE_PATH + "empirearea_c.sub",
				},
				{
					"name": "EmpireAreaFlag_A",
					"type": "expanded_image",
					"x": 167,
					"y": 235,
					"image": EMPIRE_PATH + "empireareaflag_a.sub",
				},
				{
					"name": "EmpireAreaFlag_B",
					"type": "expanded_image",
					"x": 70,
					"y": 42,
					"image": EMPIRE_PATH + "empireareaflag_b.sub",
				},
				{
					"name": "EmpireAreaFlag_C",
					"type": "expanded_image",
					"x": 357,
					"y": 78,
					"image": EMPIRE_PATH + "empireareaflag_c.sub",
				},
			],
		},
		create_button("shinsoo_btn", ATLAS_X + 160, ATLAS_Y + 235, EMPIRE_PATH + "empireareaflag_a.sub", EMPIRE_PATH + "empireareaflag_a.sub", EMPIRE_PATH + "empireareaflag_a.sub"),
		create_button("chunjo_btn", ATLAS_X + 63, ATLAS_Y + 42, EMPIRE_PATH + "empireareaflag_b.sub", EMPIRE_PATH + "empireareaflag_b.sub", EMPIRE_PATH + "empireareaflag_b.sub"),
		create_button("jinno_btn", ATLAS_X + 350, ATLAS_Y + 78, EMPIRE_PATH + "empireareaflag_c.sub", EMPIRE_PATH + "empireareaflag_c.sub", EMPIRE_PATH + "empireareaflag_c.sub"),
		create_button("left_button", ATLAS_X + 120, ATLAS_Y + 340, SELECT_BUTTON_PATH + "left_button_01.sub", SELECT_BUTTON_PATH + "left_button_02.sub", SELECT_BUTTON_PATH + "left_button_03.sub"),
		create_button("right_button", ATLAS_X + 250, ATLAS_Y + 340, SELECT_BUTTON_PATH + "right_button_01.sub", SELECT_BUTTON_PATH + "right_button_02.sub", SELECT_BUTTON_PATH + "right_button_03.sub"),
		{
			"name": "empire_board",
			"type": "thinboard_gold",
			"x": BOARD_X,
			"y": BOARD_Y,
			"width": 205,
			"height": BOARD_HEIGHT,
			"children": [
				{
					"name": "EmpireNameSlot",
					"type": "thinboard_circle",
					"x": 13,
					"y": 14,
					"width": 180,
					"height": 40,
					"children": [
						{
							"name": "EmpireFlag_A",
							"type": "expanded_image",
							"x": 0,
							"y": 0,
							"x_scale": FLAG_SCALE_X,
							"y_scale": FLAG_SCALE_Y,
							"image": EMPIRE_PATH + "empireflag_a.sub",
						},
						{
							"name": "EmpireFlag_B",
							"type": "expanded_image",
							"x": 0,
							"y": 0,
							"x_scale": FLAG_SCALE_X,
							"y_scale": FLAG_SCALE_Y,
							"image": EMPIRE_PATH + "empireflag_b.sub",
						},
						{
							"name": "EmpireFlag_C",
							"type": "expanded_image",
							"x": 0,
							"y": 0,
							"x_scale": FLAG_SCALE_X,
							"y_scale": FLAG_SCALE_Y,
							"image": EMPIRE_PATH + "empireflag_c.sub",
						},
						{
							"name": "EmpireName",
							"type": "text",
							"x": 30,
							"y": 0,
							"text": "",
							"fontsize": "LARGE",
							"all_align": "center",
						},
					],
				},
			],
		},
		create_button("select_button", BOARD_X, BOARD_BUTTON_Y, ROOT_PATH + "public_intro_btn/select_btn_01.sub", ROOT_PATH + "public_intro_btn/select_btn_02.sub", ROOT_PATH + "public_intro_btn/select_btn_03.sub"),
		create_button("exit_button", BOARD_X + 130, BOARD_BUTTON_Y, ROOT_PATH + "public_intro_btn/cancel_btn_01.sub", ROOT_PATH + "public_intro_btn/cancel_btn_02.sub", ROOT_PATH + "public_intro_btn/cancel_btn_03.sub"),
	],
}
