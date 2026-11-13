if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale


INTERFACE = "d:/ymir work/ui/login/login_interface/"

COLOR_TEXT = 0xffe0ebf6
COLOR_SLOT = 0xffc9a475

IS_ARABIC = app.GetLocalePath() == "locale/ae"

BASE_H       = 1080
CENTER_BIAS  = 0.44
COMPACT      = 0.40
LOGO_HIDE_H  = 720
LOGO_PUSH    = 125

LOGO_Y          = 51
USER_Y          = 248
PWD_Y           = 321
LINK_Y          = 463
LOGIN_BUTTON_Y  = 395
EXIT_BUTTON_X   = 23
EXIT_BUTTON_Y   = 10
DISCORD_BUTTON_X   = 350
DISCORD_BUTTON_Y   = 75
HOMEPAGE_BUTTON_X   = 350
HOMEPAGE_BUTTON_Y   = 125

BOARD_CHANNEL_Y = 538
BOARD_ACCOUNT_Y = 729

HEADER_Y = 188
HEADER_W = 500
HEADER_H = 17
HEADER_ITEM_GAP = 2

LINK_W = 100
LINK_H = 17
CREATE_ACCOUNT_X  = -80
FORGOT_PASSWORD_X =  80

PAGE_SIZE       = 6
CHANNEL_COUNT   = 6
LIST_ROW_HEIGHT = 30
LIST_ROW_START  = 6

INPUT_EDIT_X      = 60
INPUT_EDIT_Y      = 34
INPUT_EDIT_W      = 250
INPUT_EDIT_H      = 25
INPUT_LABEL_X     = 7
INPUT_LABEL_Y     = 3
INPUT_INPUT_LIMIT = 16
INPUT_LABEL_RTL_X = 245

SLOT_F_LABEL_X    = 6
SLOT_F_LABEL_Y    = 5
SLOT_TEXT_X       = 35
SLOT_TEXT_RTL_X   = 200
SLOT_TEXT_Y       = 5
SLOT_SAVE_X       = 30
SLOT_DELETE_X     = 31

CHANNEL_NAME_X  = 35
CHANNEL_NAME_RTL_X = 238
CHANNEL_NAME_Y  = 5
CHANNEL_ICON_X  = 4
CHANNEL_ICON_Y  = 6

FLAG_WIDTH        = 24
FLAG_SPACE_WIDTH  = 8
FLAG_GAP          = 1
FLAG_SPACE_OFFSET = 9
FLAG_ROW_TOP_Y    = 959
FLAG_ROW_BOTTOM_Y = 988
FLAG_ROW_TOP      = ("en", "cz", "it", "fr", "pt", "hu")
FLAG_ROW_BOTTOM   = ("ro", "de", "es", "pl", "tr")

IMG_BACKGROUND = "bg_login_interface.png"
IMG_BOARD      = "board_login.png"
IMG_LOGO       = "logo.png"
IMG_USER_LABEL = "user_label.png"
IMG_PWD_LABEL  = "password_label.png"
IMG_CHANNELS   = "boards_channels.png"
IMG_ACCOUNTS   = "board_accounts.png"
IMG_FLAG_SPACE = "flag_space.png"
IMG_ONLINE     = "online.png"
IMG_OFFLINE    = "offline.png"

BTN_LOGIN  = ("login_button01.png",
              "login_button02.png",
              "login_button03.png")
BTN_EXIT   = ("exit_game01.png",
              "exit_game02.png",
              "exit_game02.png")
BTN_SLOT   = ("account_slot_normal.png",
              "account_slot_over.png",
              "account_slot_down.png")
BTN_SAVE   = ("account_save_button_normal.png",
              "account_save_button_over.png",
              "account_save_button_down.png")
BTN_DELETE = ("account_delete_button_normal.png",
              "account_delete_button_over.png",
              "account_delete_button_down.png")
BTN_CHANNEL = ("channel_button_normal.png",
               "channel_button_over.png",
               "channel_button_down.png")
BTN_DISCORD = ("discord_normal.png",
               "discor_over.png",
               "discord_down.png")
BTN_HOMEPAGE = ("hp_normal.png",
               "hp_over.png",
               "hp_down.png")

FLAG_BTN_IMAGES = (
	"country/%s_01.tga",
	"country/%s_02.tga",
	"country/%s_02.tga",
)

HEADER_ITEMS = [
	("HomePage", uiScriptLocale.LOGIN_WINDOW_HOMEPAGE, 80),
	("Register", uiScriptLocale.LOGIN_WINDOW_REGISTER, 80),
	("Support",  uiScriptLocale.LOGIN_WINDOW_SUPPORT,  70),
	("Forum",    uiScriptLocale.LOGIN_WINDOW_FORUM,    60),
]


def HeightDiff():
	d = BASE_H - SCREEN_HEIGHT
	return d if d > 0 else 0

def AY(y):
	return int(y + HeightDiff() * CENTER_BIAS)

def CY(y):
	return int(AY(y) - HeightDiff() * COMPACT * (float(y) / float(BASE_H)))

def BY(y):
	return int(y - HeightDiff())

def FY(y):
	return y

def UY(y):
	shift = LOGO_PUSH if SCREEN_HEIGHT <= LOGO_HIDE_H else 0
	return int(CY(y) - shift)

def LoginY(y):
	h = SCREEN_HEIGHT
	if h <= 600: return UY(y) + 30
	if h <= 720: return UY(y) + 50
	return CY(y)


def _apply_align(node, h_align, v_align):
	if h_align:
		node["horizontal_align"]      = h_align
		node["text_horizontal_align"] = h_align
	if v_align:
		node["vertical_align"]      = v_align
		node["text_vertical_align"] = v_align


def img(name, x, y, image, h_align=None, v_align=None, children=None, **kw):
	node = {
		"name": name, "type": "image",
		"x": x, "y": y,
		"image": INTERFACE + image,
	}
	_apply_align(node, h_align, v_align)
	if children is not None:
		node["children"] = children
	node.update(kw)
	return node


def text(name, x, y, text_str, h_align=None, v_align=None, **kw):
	node = {
		"name": name, "type": "text",
		"x": x, "y": y, "text": text_str,
		"text_horizontal_align": "left",
	}
	_apply_align(node, h_align, v_align)
	node.update(kw)
	return node


def button(name, x, y, images, h_align=None, v_align=None, children=None, **kw):
	base, over, down = images
	node = {
		"name": name, "type": "button",
		"x": x, "y": y,
		"default_image": INTERFACE + base,
		"over_image":    INTERFACE + over,
		"down_image":    INTERFACE + down,
	}
	_apply_align(node, h_align, v_align)
	if children is not None:
		node["children"] = children
	node.update(kw)
	return node


def window(name, x, y, width, height, children=None, h_align=None, **kw):
	node = {
		"name": name, "type": "window",
		"x": x, "y": y, "width": width, "height": height,
	}
	if h_align:
		node["horizontal_align"] = h_align
	if children is not None:
		node["children"] = children
	node.update(kw)
	return node


def input_block(name, image_name, edit_name, y, label_text, is_password=False):
	label_x = INPUT_LABEL_RTL_X if IS_ARABIC else INPUT_LABEL_X
	label_align = "right" if IS_ARABIC else "left"
	return {
		"name": name, "type": "image",
		"x": 0, "y": UY(y),
		"horizontal_align": "center",
		"image": INTERFACE + image_name,
		"children": [
			{
				"name": edit_name, "type": "editline",
				"x": INPUT_EDIT_X, "y": INPUT_EDIT_Y,
				"width": INPUT_EDIT_W, "height": INPUT_EDIT_H,
				"input_limit": INPUT_INPUT_LIMIT,
				"secret_flag": 1 if is_password else 0,
			},
			{
				"name": edit_name + "name", "type": "text",
				"x": label_x, "y": INPUT_LABEL_Y,
				"text": label_text, "outline": 1,
				"text_horizontal_align": label_align,
			},
		],
	}


def account_slot(n):
	y    = (n - 1) * LIST_ROW_HEIGHT
	base = "f%d" % n
	account_text_x = SLOT_TEXT_RTL_X if IS_ARABIC else SLOT_TEXT_X
	account_text_align = "right" if IS_ARABIC else "left"
	return button(
		base + "_login_button",
		LIST_ROW_START, LIST_ROW_START + y,
		BTN_SLOT,
		children=[
			text(base + "_text",
			     SLOT_F_LABEL_X, SLOT_F_LABEL_Y, "F%d" % n,
			     color=COLOR_SLOT, outline=1, fontsize="LARGE"),
			text(base + "_account_text",
			     account_text_x, SLOT_TEXT_Y,
			     uiScriptLocale.LOGIN_WINDOW_FREE_SLOT,
			     color=COLOR_SLOT, outline=1, fontsize="LARGE",
			     text_horizontal_align=account_text_align),
			button(base + "_save_button",
			       SLOT_SAVE_X, 0, BTN_SAVE,
			       h_align="right", v_align="center"),
			button(base + "_delete_button",
			       SLOT_DELETE_X, 0, BTN_DELETE,
			       h_align="right", v_align="center"),
		],
	)


def channel_row(n):
	y = (n - 1) * LIST_ROW_HEIGHT
	base, over, down = BTN_CHANNEL
	channel_text_x = CHANNEL_NAME_RTL_X if IS_ARABIC else CHANNEL_NAME_X
	channel_text_align = "right" if IS_ARABIC else "left"
	return {
		"name": "channel_button_%d" % n,
		"type": "radio_button",
		"x": LIST_ROW_START, "y": LIST_ROW_START + y,
		"default_image": INTERFACE + base,
		"over_image":    INTERFACE + over,
		"down_image":    INTERFACE + down,
		"children": [
			text("ch%d_name" % n,
			     channel_text_x, CHANNEL_NAME_Y,
			     "%s %d" % (uiScriptLocale.LOGIN_WINDOW_CHANNEL, n),
			     color=COLOR_SLOT, outline=1, fontsize="LARGE",
			     text_horizontal_align=channel_text_align),
			{
				"name": "ch%d_online" % n, "type": "image",
				"style": ("not_pick",),
				"x": CHANNEL_ICON_X, "y": CHANNEL_ICON_Y,
				"image": INTERFACE + IMG_ONLINE,
			},
			{
				"name": "ch%d_offline" % n, "type": "image",
				"style": ("not_pick",),
				"x": CHANNEL_ICON_X, "y": CHANNEL_ICON_Y,
				"image": INTERFACE + IMG_OFFLINE,
			},
		],
	}


def language_button(name, x, y, code):
	base, over, down = FLAG_BTN_IMAGES
	return {
		"name": name, "type": "button",
		"x": x, "y": y,
		"horizontal_align": "center",
		"default_image": INTERFACE + (base % code),
		"over_image":    INTERFACE + (over % code),
		"down_image":    INTERFACE + (down % code),
	}


def language_space(name, x, y):
	return {
		"name": name, "type": "image",
		"x": x, "y": y,
		"horizontal_align": "center",
		"image": INTERFACE + IMG_FLAG_SPACE,
	}


def language_row(codes, y, prefix):
	n = len(codes)
	total_width = n * FLAG_WIDTH + (n - 1) * (FLAG_SPACE_WIDTH + FLAG_GAP * 2)
	cur_x = -total_width / 2

	out = []
	for i, code in enumerate(codes):
		out.append(language_button("%s_%s" % (prefix, code), int(cur_x), y, code))
		cur_x += FLAG_WIDTH
		if i < n - 1:
			cur_x += FLAG_GAP
			out.append(language_space(
				"%s_space_%d" % (prefix, i),
				int(cur_x - FLAG_SPACE_OFFSET), y + 2))
			cur_x += FLAG_SPACE_WIDTH + FLAG_GAP
	return out


def build_header():
	total = sum(w for _, _, w in HEADER_ITEMS) \
	      + HEADER_ITEM_GAP * (len(HEADER_ITEMS) - 1)
	x = (HEADER_W - total) / 2

	children = []
	for name, label, w in HEADER_ITEMS:
		children.append(window(
			name, x, 0, w, HEADER_H,
			children=[text(name + "_Name", w / 2, 0, label,
			               fontsize="LARGE",
			               text_horizontal_align="center")],
		))
		x += w + HEADER_ITEM_GAP

	return window("Header", 0, HEADER_Y, HEADER_W, HEADER_H,
	              h_align="center", children=children)


def build_link(name, x, label):
	return window(
		name, x, LINK_Y, LINK_W, LINK_H,
		h_align="center",
		children=[text(name + "_Name", LINK_W / 2, 0, label,
		               fontsize="LARGE", color=COLOR_SLOT,
		               text_horizontal_align="center")],
	)


def build_channel_board():
	return img(
		"Board_Channel", 0, UY(BOARD_CHANNEL_Y), IMG_CHANNELS,
		h_align="center",
		children=[channel_row(i) for i in range(1, CHANNEL_COUNT + 1)],
	)




def build_account_board():
	return img(
		"Board_Account", 0, UY(BOARD_ACCOUNT_Y), IMG_ACCOUNTS,
		h_align="center",
		children=[account_slot(i) for i in range(1, PAGE_SIZE + 1)],
	)


def build_login_children():
	children = []

	children.append(img("logo", 0, CY(LOGO_Y), IMG_LOGO, h_align="center"))

	children += [
		input_block("input_user",     IMG_USER_LABEL, "id",  USER_Y,
		            uiScriptLocale.LOGIN_WINDOW_USERNAME),
		input_block("input_password", IMG_PWD_LABEL,  "pwd", PWD_Y,
		            uiScriptLocale.LOGIN_WINDOW_PASSWORD, is_password=True),

		build_channel_board(),
		build_account_board(),

		build_link("CreateAccount",  CREATE_ACCOUNT_X,
		           uiScriptLocale.LOGIN_WINDOW_CREATE_ACCOUNT),
		build_link("ForgotPassword", FORGOT_PASSWORD_X,
		           uiScriptLocale.LOGIN_WINDOW_FORGOT_PASSWORD),

		button("login_button", 0, LoginY(LOGIN_BUTTON_Y), BTN_LOGIN,
		       h_align="center",
		       children=[text("login_button_name", 0, 0,
		                      uiScriptLocale.LOGIN_WINDOW_LOGIN,
		                      outline=True, fontsize="LARGE",
		                      h_align="center", v_align="center")]),

		button("exit_game", EXIT_BUTTON_X, CY(EXIT_BUTTON_Y), BTN_EXIT,
		       h_align="right"),


		build_header(),
	]

	children += language_row(FLAG_ROW_TOP,    FLAG_ROW_TOP_Y,    "flag_top")
	children += language_row(FLAG_ROW_BOTTOM, FLAG_ROW_BOTTOM_Y, "flag_bottom")


	return children


def create_window():
	bg_scale = max(float(SCREEN_WIDTH) / 1920.0, float(SCREEN_HEIGHT) / 1080.0, 1.0)
	return {
		"name": "LoginWindow",
		"x": 0, "y": 0,
		"width":  SCREEN_WIDTH,
		"height": SCREEN_HEIGHT,
		"force_ltr": True,
		"style": ("float",),
		"children": [
			{
				"name": "Background", "type": "expanded_image",
				"x": 0, "y": 0,
				"x_scale": bg_scale,
				"y_scale": bg_scale,
				"image": INTERFACE + IMG_BACKGROUND,
			},
			{
				"name": "board", "type": "expanded_image",
				"x": 0, "y": 0,
				"vertical_align": "center",
				"image": INTERFACE + IMG_BOARD,
				"children": build_login_children(),
			},
		button("Discord", DISCORD_BUTTON_X, CY(DISCORD_BUTTON_Y), BTN_DISCORD,),
		button("HomepageBtn", HOMEPAGE_BUTTON_X, CY(HOMEPAGE_BUTTON_Y), BTN_HOMEPAGE,),
		],
	}


window = create_window()
