import uiScriptLocale

ROOT_PATH					= "d:/ymir work/ui/game/force_event/"

WINDOW_WIDTH				= 306
WINDOW_HEIGHT				= 176

MAIN_WINDOW_WIDTH			= 286
MAIN_WINDOW_HEIGHT			= 134

ITEM_SLOT_POS_Y				= 10
ITEM_COUNT_POS_Y			= 24

window = {
	"name"		: "ForceEventWindow",
	"style"		: ("movable", "float",),
	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	[
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			"x"			: 0,
			"y"			: 0,
			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,
			"title"		: uiScriptLocale.FORCE_EVENT_TITLE,			# 내공강화 이벤트

			"children" :
			[
				# background
				{
					"name"						: "main_outline",
					"type"						: "outline_window",
					"style"						: ("not_pick", ),
					"x"							: 10,
					"y"							: 32,
					"width"						: MAIN_WINDOW_WIDTH,
					"height"					: MAIN_WINDOW_HEIGHT,
				},

				{
					"name"						: "main_menu_img",
					"type"						: "image",
					"x"							: 12,
					"y"							: 35,
					"image"						: ROOT_PATH + "menu_bar_bg.sub",

				},

				{
					"name"						: "main_boundary_img",
					"type"						: "image",
					"x"							: 81,
					"y"							: 32,
					"image"						: ROOT_PATH + "slot_boundary.sub",

				},

				{
					"name"						: "main_force_menu_type_text",
					"type"						: "text",
					"x"							: 46,
					"y"							: 42,
					"text_horizontal_align"		: "center",
					"text"						: uiScriptLocale.FORCE_EVENT_FORCE_MENU_TYPE,	# Force
				},

				{
					"name"						: "main_force_menu_playtime_text",
					"type"						: "text",
					"x"							: 182,
					"y"							: 42,
					"text_horizontal_align"		: "center",
					"text"						: uiScriptLocale.FORCE_EVENT_FORCE_PLAY_TIME,	# 플레이타임
				},

				# normal attack force
				{
					"name"						: "normal_attack_force_slot_bg",
					"type"						: "image",
					"x"							: 31,
					"y"							: 71,
					"width"						: 34,
					"height"					: 34,
					"image"						: ROOT_PATH + "normal_attack_force_slot.sub",

				},

				{
					"name"						: "normal_attack_force_slot",
					"type"						: "slot",
					"x"							: 32,
					"y"							: 72,
					"width"						: 32,
					"height"					: 32,
					"slot"						: ({"index":0, "x":0, "y":0, "width":32, "height":32},),
				},

				{
					"name"						: "normal_attack_force_use_on_button",
					"type"						: "button",
					"x"							: 231,
					"y"							: 78,
					"default_image"				: ROOT_PATH + "normal_attack_force_use_bt_default.sub",
					"over_image"				: ROOT_PATH + "normal_attack_force_use_bt_over.sub",
					"down_image"				: ROOT_PATH + "normal_attack_force_use_bt_down.sub",
				},

				{
					"name"						: "normal_attack_force_use_off_button",
					"type"						: "button",
					"x"							: 231,
					"y"							: 78,
					"default_image"				: ROOT_PATH + "normal_attack_force_use_bt_down.sub",
					"over_image"				: ROOT_PATH + "normal_attack_force_use_bt_down.sub",
					"down_image"				: ROOT_PATH + "normal_attack_force_use_bt_down.sub",
				},

				{
					"name"						: "normal_attack_force_gauge_bar_bg",
					"type"						: "image",
					"x"							: 93,
					"y"							: 75,
					"width"						: 128,
					"height"					: 26,
					"image"						: ROOT_PATH + "gauge_bar_bg.sub",

				},

				{
					"name"						: "normal_attack_force_gauge",
					"type"						: "expanded_image",
					"x"							: 93,
					"y"							: 75,
					"width"						: 128,
					"height"					: 26,
					"image"						: ROOT_PATH + "gauge_bar.sub",
				},

				{
					"name"						: "normal_attack_force_remain_time",
					"type"						: "text",
					"x"							: 160,
					"y"							: 82,
					"text_horizontal_align"		: "center",
					"text"						: "",
				},

				# skill attack force
				{
					"name"						: "skill_attack_force_slot_bg",
					"type"						: "image",
					"x"							: 31,
					"y"							: 119,
					"width"						: 34,
					"height"					: 34,
					"image"						: ROOT_PATH + "skill_attack_force_slot.sub",

				},

				{
					"name"						: "skill_attack_force_slot",
					"type"						: "slot",
					"x"							: 32,
					"y"							: 120,
					"width"						: 32,
					"height"					: 32,
					"slot"						: ({"index":0, "x":0, "y":0, "width":32, "height":32},),
				},

				{
					"name"						: "skill_attack_force_use_on_button",
					"type"						: "button",
					"x"							: 231,
					"y"							: 126,
					"default_image"				: ROOT_PATH + "skill_attack_force_use_bt_default.sub",
					"over_image"				: ROOT_PATH + "skill_attack_force_use_bt_over.sub",
					"down_image"				: ROOT_PATH + "skill_attack_force_use_bt_down.sub",
				},

				{
					"name"						: "skill_attack_force_use_off_button",
					"type"						: "button",
					"x"							: 231,
					"y"							: 126,
					"default_image"				: ROOT_PATH + "skill_attack_force_use_bt_down.sub",
					"over_image"				: ROOT_PATH + "skill_attack_force_use_bt_down.sub",
					"down_image"				: ROOT_PATH + "skill_attack_force_use_bt_down.sub",
				},

				{
					"name"						: "skill_attack_force_gauge_bar_bg",
					"type"						: "image",
					"x"							: 93,
					"y"							: 123,
					"width"						: 128,
					"height"					: 26,
					"image"						: ROOT_PATH + "gauge_bar_bg.sub",
				},

				{
					"name"						: "skill_attack_force_gauge",
					"type"						: "expanded_image",
					"x"							: 93,
					"y"							: 123,
					"width"						: 128,
					"height"					: 26,
					"image"						: ROOT_PATH + "gauge_bar.sub",
				},

				{
					"name"						: "skill_attack_force_remain_time",
					"type"						: "text",
					"x"							: 160,
					"y"							: 130,
					"text_horizontal_align"		: "center",
					"text"						: "",
				},
			],
		},
	],
}