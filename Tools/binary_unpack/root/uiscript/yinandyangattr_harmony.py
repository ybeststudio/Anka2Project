import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/yin_and_yang_attr/"

WINDOW_WIDTH		= 242
WINDOW_HEIGHT		= 288

window = {
	"name"		: "YinAndYangAttr_Harmony",
	"style"		: ("movable", "float", ),

	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,

	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	(
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",

			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"title"		: uiScriptLocale.YIN_AND_YANG_ATTR_HARMONY_TITLE,	# 연결의 조화

			"children"	:
			[
				{
					"name"		: "bg_window",
					"type"		: "outline_window",
					"x"			: 10,
					"y"			: 32,
					"width"		: 220,
					"height"	: 220,
					"style"		: ("ltr",),

					"children"	:
					[
						# window["children"][0]["children"][0]["children"]
					],
				},

				# 음양속성 남은 시간 텍스트
				{
					#"name" : "remain_time_bg_slot_bar", "type" : "slotbar", "x" : 48, "y" : 260, "width" : 81, "height" : 11,
					"name" : "remain_time_text_window", "type" : "window", "x" : 48, "y" : 260, "width" : 81, "height" : 11,
					"children" :
					(
						{"name" : "remain_time_text", "type":"text", "text": "", "x":0, "y":0, "all_align" : "center", },
					),
				},

				# 켜기 버튼
				{
					"name"			: "enable_toggle_button",
					"type"			: "toggle_button",
					"x"				: 170,
					"y"				: 255,
					"text"			: uiScriptLocale.YIN_AND_YANG_ATTR_HARMONY_ACTIVATE_BUTTON_TEXT,	# 활성화
					"default_image"	: "d:/ymir work/ui/public/middle_button_01.sub",
					"over_image"	: "d:/ymir work/ui/public/middle_button_02.sub",
					"down_image"	: "d:/ymir work/ui/public/middle_button_03.sub",
				},
			],
		},
	),
}

# ? 버튼 (도움말)
window["children"][0]["children"].append(
{
	"name"			: "question_button",
	"type"			: "button",

	"x"				: WINDOW_WIDTH - 30 - 16,
	"y"				: 9,

	"default_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
	"over_image"	: "d:/ymir work/ui/pattern/q_mark_02.tga",
	"down_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
},)

# 음양 무늬 배경 이미지
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "yin_and_yang_bg",
	"type"	: "image",
	"style"	: ("ltr",),
					
	"x"		: 0,
	"y"		: 0,
	"image"	: ROOT_PATH + "harmony/ver2/big_yin_and_yang_bg.sub",
},)


# (무기) : 아이콘
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "weapon_icon",
	"type"	: "image",
	"style"	: ("ltr",),
					
	"x"		: 21 - 10,
	"y"		: 67 - 32,
	"image"	: ROOT_PATH + "harmony/ver2/icon_yang_small.sub",
},)

# (무기) : 버튼
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "weapon_button",
	"type"	: "toggle_button",
	"style"	: ("ltr",),
					
	"x"		: 36 - 10,
	"y"		: 89 - 32,
	"default_image"	: ROOT_PATH + "harmony/weapon_default.sub", 
	"over_image"	: ROOT_PATH + "harmony/weapon_over.sub",
	"down_image"	: ROOT_PATH + "harmony/weapon_down.sub",
},)

# (무기) : 이펙트
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "weapon_activated_effect",
	"type"	: "ani_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 35 - 10,
	"y"		: 88 - 32,

	"images" :
	(
		ROOT_PATH + "harmony/blue_outline_eff_size1_1.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_2.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_3.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_4.sub",
	),
},)

# (장갑) : 아이콘
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "glove_icon",
	"type"	: "image",
	"style"	: ("ltr",),
					
	"x"		: 192 - 10,
	"y"		: 67 - 32,
	"image"	: ROOT_PATH + "harmony/ver2/icon_yang_small.sub",
},)

# (장갑) : 버튼
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "glove_button",
	"type"	: "toggle_button",
	"style"	: ("ltr",),
					
	"x"		: 168 - 10,
	"y"		: 89 - 32,
	"default_image"	: ROOT_PATH + "harmony/glove_default.sub", 
	"over_image"	: ROOT_PATH + "harmony/glove_over.sub",
	"down_image"	: ROOT_PATH + "harmony/glove_down.sub",
},)

# (장갑) : 이펙트
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "glove_activated_effect",
	"type"	: "ani_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 167 - 10,
	"y"		: 88 - 32,

	"images" :
	(
		ROOT_PATH + "harmony/blue_outline_eff_size1_1.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_2.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_3.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_4.sub",
	),
},)

# (머리) : 아이콘
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "head_icon",
	"type"	: "image",
	"style"	: ("ltr",),
					
	"x"		: 107 - 10,
	"y"		: 35 - 32,
	"image"	: ROOT_PATH + "harmony/ver2/icon_yang_small.sub",
},)

# (머리) : 버튼
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "head_button",
	"type"	: "toggle_button",
	"style"	: ("ltr",),
					
	"x"		: 102 - 10,
	"y"		: 57 - 32,
	"default_image"	: ROOT_PATH + "harmony/head_default.sub", 
	"over_image"	: ROOT_PATH + "harmony/head_over.sub",
	"down_image"	: ROOT_PATH + "harmony/head_down.sub",
},)

# (머리) : 이펙트
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "head_activated_effect",
	"type"	: "ani_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 101 - 10,
	"y"		: 56 - 32,

	"images" :
	(
		ROOT_PATH + "harmony/blue_outline_eff_size1_1.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_2.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_3.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_4.sub",
	),
},)

# (신발) : 아이콘
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "shoes_icon",
	"type"	: "image",
	"style"	: ("ltr",),
					
	"x"		: 107 - 10,
	"y"		: 180 - 32,
	"image"	: ROOT_PATH + "harmony/ver2/icon_yang_small.sub",
},)

# (신발) : 버튼
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "shoes_button",
	"type"	: "toggle_button",
	"style"	: ("ltr",),
					
	"x"		: 102 - 10,
	"y"		: 202 - 32,
	"default_image"	: ROOT_PATH + "harmony/foots_default.sub", 
	"over_image"	: ROOT_PATH + "harmony/foots_over.sub",
	"down_image"	: ROOT_PATH + "harmony/foots_down.sub",
},)

# (신발) : 이펙트
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "shoes_activated_effect",
	"type"	: "ani_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 101 - 10,
	"y"		: 201 - 32,

	"images" :
	(
		ROOT_PATH + "harmony/blue_outline_eff_size1_1.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_2.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_3.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_4.sub",
	),
},)

# (갑옷) : 아이콘
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "body_icon",
	"type"	: "image",
	"style"	: ("ltr",),
					
	"x"		: 107 - 10,
	"y"		: 115 - 32,
	"image"	: ROOT_PATH + "harmony/ver2/icon_yang_small.sub",
},)

# (갑옷) : 버튼
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "body_button",
	"type"	: "toggle_button",
	"style"	: ("ltr",),
					
	"x"		: 102 - 10,
	"y"		: 137 - 32,
	"default_image"	: ROOT_PATH + "harmony/body_default.sub", 
	"over_image"	: ROOT_PATH + "harmony/body_over.sub",
	"down_image"	: ROOT_PATH + "harmony/body_down.sub",
},)

# (갑옷) : 이펙트
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "body_activated_effect",
	"type"	: "ani_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 101 - 10,
	"y"		: 136 - 32,

	"images" :
	(
		ROOT_PATH + "harmony/blue_outline_eff_size1_1.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_2.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_3.sub",
		ROOT_PATH + "harmony/blue_outline_eff_size1_4.sub",
	),
},)

# 무기 -> 머리 연결선 (주황)
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_weapon_to_head",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 53 - 10,
	"y"		: 75 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_weapon_to_head.sub",
},)

# 무기 -> 장갑 연결선 (주황)
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_weapon_to_glove",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 74 - 10,
	"y"		: 106 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_weapon_to_glove.sub",
},)

# 무기 -> 갑옷 연결선 (주황)
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_weapon_to_body_1",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 53 - 10,
	"y"		: 127 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_weapon_to_body_1.sub",
},)

window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_weapon_to_body_2",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 55 - 10,
	"y"		: 154 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_weapon_to_body_2.sub",
},)

# 무기 -> 신발 연결선 (주황)
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_weapon_to_foots_1",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 53 - 10,
	"y"		: 127 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_weapon_to_foots_1.sub",
},)

window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_weapon_to_foots_2",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 55 - 10,
	"y"		: 219 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_weapon_to_foots_2.sub",
},)

# 장갑 -> 머리 연결선 (파랑)
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_glove_to_head",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 140 - 10,
	"y"		: 75 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_glove_to_head.sub",
},)

# 장갑 -> 무기 연결선 (파랑)
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_glove_to_weapon",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 74 - 10,
	"y"		: 106 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_glove_to_weapon.sub",
},)

# 장갑 -> 갑옷 연결선 (파랑)
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_glove_to_body_1",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 185 - 10,
	"y"		: 127 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_glove_to_body_1.sub",
},)

window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_glove_to_body_2",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 140 - 10,
	"y"		: 154 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_glove_to_body_2.sub",
},)

# 장갑 -> 신발 연결선 (파랑)
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_glove_to_foots_1",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 185 - 10,
	"y"		: 127 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_glove_to_foots_1.sub",
},)

window["children"][0]["children"][0]["children"].append(
{
	"name"	: "connect_line_glove_to_foots_2",
	"type"	: "expanded_image",
	"style"	: ("ltr", "not_pick", ),
					
	"x"		: 140 - 10,
	"y"		: 219 - 32,

	"image"	: ROOT_PATH + "harmony/ver2/connect_line_glove_to_foots_2.sub",
},)