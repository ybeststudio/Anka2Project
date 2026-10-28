import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/yin_and_yang_attr/"

WINDOW_WIDTH		= 198
WINDOW_HEIGHT		= 540

window = {
	"name"		: "YinAndYangAttr_Inventory",
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

			"title"		: uiScriptLocale.YIN_AND_YANG_ATTR_INVENTORY_TITLE,	# 음양속성

			"children"	:
			[
				{
					"name"		: "bg_window",
					"type"		: "outline_window",
					"x"			: 10,
					"y"			: 32,
					"width"		: 178,
					"height"	: 498,

					"children"	:
					[
						# window["children"][0]["children"][0]["children"]
					],
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

# 가로라인 1
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "horizontal_line_1",
	"type"	: "image",
	"style"	: ("ltr",),
					
	"x"		: 2,
	"y"		: 143,
	"image"	: ROOT_PATH + "inventory/horizontal_line.sub",
},)

# 가로라인 2
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "horizontal_line_2",
	"type"	: "image",
	"style"	: ("ltr",),
					
	"x"		: 2,
	"y"		: 363,
	"image"	: ROOT_PATH + "inventory/horizontal_line.sub",
},)

# 가로라인 3
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "horizontal_line_3",
	"type"	: "image",
	"style"	: ("ltr",),
					
	"x"		: 2,
	"y"		: 462,
	"image"	: ROOT_PATH + "inventory/horizontal_line.sub",
},)

# 타이틀바 1
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "title_bar_1",
	"type"		: "image",
	"style"		: ("ltr",),
					
	"x"			: 3,
	"y"			: 2,
	"image"		: ROOT_PATH + "inventory/title_bar_1_bg.sub",

	"children"	:
	(
		# 부여할 아이템
		{ "name":"title_bar_1_text", "type":"text", "x":0, "y":0, "text":uiScriptLocale.YIN_AND_YANG_ATTR_INVENTORY_SUB_TITLE_1, "all_align":"center" },	
	),
},)

# 타이틀바 2
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "title_bar_2",
	"type"		: "image",
	"style"		: ("ltr",),
					
	"x"			: 3,
	"y"			: 201,
	"image"		: ROOT_PATH + "inventory/title_bar_2_bg.sub",

	"children"	:
	(
		# 보유 속성
		{ "name":"title_bar_2_text", "type":"text", "x":0, "y":0, "text":uiScriptLocale.YIN_AND_YANG_ATTR_INVENTORY_SUB_TITLE_2, "all_align":"center" },
		# 음양속성 상점 버튼
		{ 
			"name"	:"yin_and_yang_shop_button",
			"type"	: "button", 
			"x":166 - 3 - 10, "y": 235 - 201 - 32,
			"default_image"	: ROOT_PATH + "inventory/button_yin_and_yang_shop_default.sub", 
			"over_image"	: ROOT_PATH + "inventory/button_yin_and_yang_shop_over.sub",
			"down_image"	: ROOT_PATH + "inventory/button_yin_and_yang_shop_down.sub",
		},
	),
},)

# 타이틀바 3
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "title_bar_3",
	"type"		: "image",
	"style"		: ("ltr",),
					
	"x"			: 3,
	"y"			: 396,
	"image"		: ROOT_PATH + "inventory/title_bar_2_bg.sub",

	"children"	:
	(
		# 최근 획득 속성
		{ "name":"title_bar_3_text", "type":"text", "x":0, "y":0, "text":uiScriptLocale.YIN_AND_YANG_ATTR_INVENTORY_SUB_TITLE_3, "all_align":"center" },
	),
},)

#음양속성 부여 UI 상단
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "attach_attr_window_1",
	"type"		: "window",
	"style"		: ("ltr",),
					
	"x"			: 1,
	"y"			: 23,
	"width"		: 174,
	"height"	: 120,

	"children"	:
	(
		# 음양속성 부여 : 아이템 슬롯 before
		{
			"name" : "attach_attr_item_slot_before_bg",
			"type" : "image",

			"x" : 22 - 10 - 1,
			"y" : 63 - 32 - 23,
					
			"image"		: ROOT_PATH + "inventory/attach_attr_item_slot_before.sub",

			"children" :
			(
				# 슬롯
				{
					"name"			: "attach_attr_item_slot_before",
					"type"			: "grid_table",
					"x"				: 4,
					"y"				: 4,
					"start_index"	: 0,
					"x_count"		: 1,
					"y_count"		: 3,
					"x_step"		: 32,
					"y_step"		: 32,
				},
			),
		},

		# 음양속성 부여 : 아이템 음양속성 슬롯
		{
			"name" : "attach_attr_item_slot_attr_bg",
			"type" : "image",

			"x" : 82 - 10 - 1,
			"y" : 99 - 32 - 23,
					
			"image"		: ROOT_PATH + "inventory/attach_attr_item_slot_attr.sub",

			"children" :
			(
				# 슬롯
				{
					"name"			: "attach_attr_item_slot_attr",
					"type"			: "grid_table",
					"x"				: 0,
					"y"				: 0,
					"start_index"	: 0,
					"x_count"		: 1,
					"y_count"		: 1,
					"x_step"		: 32,
					"y_step"		: 32,
				},
			),
		},

		# 음양속성 부여 : 아이템 슬롯 after
		{
			"name" : "attach_attr_item_slot_after_bg",
			"type" : "image",

			"x" : 134 - 10 - 1,
			"y" : 61 - 32 - 23,
					
			"image"		: ROOT_PATH + "inventory/attach_attr_item_slot_after.sub",

			"children" :
			(
				# 슬롯
				{
					"name"			: "attach_attr_item_slot_after",
					"type"			: "grid_table",
					"x"				: 6,
					"y"				: 6,
					"start_index"	: 0,
					"x_count"		: 1,
					"y_count"		: 3,
					"x_step"		: 32,
					"y_step"		: 32,
				},
			),
		},

		# 음양속성 부여 : gauge1
		{
			"name" : "attach_attr_item_gauge_1_bg",
			"type" : "image",

			"x" : 62 - 10 - 1,
			"y" : 77 - 32 - 23,
					
			"image"		: ROOT_PATH + "inventory/attach_attr_gauge_empty_1.sub",

			"children" :
			(
				# 실제 게이지
				{
					"name" : "attach_attr_item_gauge_1",
					"type" : "expanded_image",

					"x" : 0,
					"y" : 0,
					
					"image"	: ROOT_PATH + "inventory/attach_attr_gauge_empty_1.sub",
				},
			),
		},

		# 음양속성 부여 : gauge2
		{
			"name" : "attach_attr_item_gauge_2_bg",
			"type" : "image",

			"x" : 62 - 10 - 1,
			"y" : 131 - 32 - 23,
					
			"image"		: ROOT_PATH + "inventory/attach_attr_gauge_empty_2.sub",

			"children" :
			(
				# 실제 게이지
				{
					"name" : "attach_attr_item_gauge_2",
					"type" : "expanded_image",

					"x" : 0,
					"y" : 0,
					
					"image"	: ROOT_PATH + "inventory/attach_attr_gauge_empty_2.sub",
				},
			),
		},

		# 음양속성 부여 : gauge3
		{
			"name" : "attach_attr_item_gauge_3_bg",
			"type" : "expanded_image",

			"x" : 114 - 10 - 1,
			"y" : 110 - 32 - 23,
					
			"image"		: ROOT_PATH + "inventory/attach_attr_gauge_empty_3.sub",

			"children" :
			(
				# 실제 게이지
				{
					"name" : "attach_attr_item_gauge_3",
					"type" : "expanded_image",

					"x" : 0,
					"y" : 0,
					
					"image"	: ROOT_PATH + "inventory/attach_attr_gauge_empty_3.sub",
				},
			),
		},
	),
},)

#음양속성 부여 UI 하단
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "attach_attr_window_1",
	"type"		: "window",
	"x"			: 1,
	"y"			: 143,
	"width"		: 174,
	"height"	: 56,

	"children"	:
	(
		{
			"name"		: "need_gold_icon",
			"type"		: "image",
			"style"		: ("ltr",),
					
			"x"			: 36 - 10 - 1,
			"y"			: 186 - 32 - 143,
			"image"		: ROOT_PATH + "inventory/need_gold_icon.sub",	
		},

		{
			"name"		: "need_gold_text_bg",
			"type"		: "image",
			"style"		: ("ltr",),
					
			"x"			: 53 - 10 - 1,
			"y"			: 184 - 32 - 143,
			"image"		: ROOT_PATH + "inventory/need_gold_text_bg.sub",
			
			"children"	:
			(
				{ "name":"need_gold_text", "type":"text", "x":0, "y":0, "text":"", "all_align":"center" },	
			),
		},

		# 부여하기 버튼
		{ 
			"name"	: "attach_attr_button",
			"type"	: "button",
			"x"		: 35 - 10 - 1, "y": 207 - 32 - 143,

			"text"			: uiScriptLocale.YIN_AND_YANG_ATTR_INVENTORY_BUTTON_TEXT_1,	# 부여하기
			"default_image"	: ROOT_PATH + "inventory/button_1_default.sub", 
			"over_image"	: ROOT_PATH + "inventory/button_1_over.sub",
			"down_image"	: ROOT_PATH + "inventory/button_1_down.sub",
		},
	),
},)

# 보유 속성 슬롯 UI
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "attr_inventory_slot_grid_bg",
	"type"		: "image",
	"style"		: ("ltr",),
					
	"x"			: 19 - 10,
	"y"			: 261 - 32,
	"image"		: ROOT_PATH + "inventory/attr_inventory_slot_grid_bg.sub",

	"children" :
	(
		# 슬롯
		{
			"name"			: "attr_inventory_slot_grid",
			"type"			: "grid_table",
			"x"				: 0,
			"y"				: 0,
			"start_index"	: 0,
			"x_count"		: 5,
			"y_count"		: 4,
			"x_step"		: 32,
			"y_step"		: 32,
		},
	),
},)

# 버리기 버튼
window["children"][0]["children"][0]["children"].append(
{ 
	"name"	: "delete_attr_button",
	"type"	: "button",
	"x"		: 35 - 10, "y": 403 - 32,

	"text"			: uiScriptLocale.YIN_AND_YANG_ATTR_INVENTORY_BUTTON_TEXT_2,	# 버리기
	"default_image"	: ROOT_PATH + "inventory/button_1_default.sub", 
	"over_image"	: ROOT_PATH + "inventory/button_1_over.sub",
	"down_image"	: ROOT_PATH + "inventory/button_1_down.sub",
},)

#최근 획득 속성 UI
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "recently_earned_attr_window",
	"type"		: "window",
	"x"			: 1,
	"y"			: 418,
	"width"		: 174,
	"height"	: 44,

	"children"	:
	(
		# 최근 획득 음양속성 슬롯
		{
			"name" : "recently_obtained_attr_slot_bg",
			"type" : "image",

			"x" : 19 - 10 - 1,
			"y" : 456 - 32 - 418,
					
			"image"		: ROOT_PATH + "inventory/recently_obtained_attr_slot.sub",

			"children" :
			(
				# 슬롯
				{
					"name"			: "recently_obtained_attr_slot",
					"type"			: "grid_table",
					"x"				: 0,
					"y"				: 0,
					"start_index"	: 0,
					"x_count"		: 1,
					"y_count"		: 1,
					"x_step"		: 32,
					"y_step"		: 32,
				},
			),
		},

		{
			"name"		: "remain_time_icon",
			"type"		: "image",
			"style"		: ("ltr",),
					
			"x"			: 57 - 10 - 1,
			"y"			: 466 - 32 - 418,
			"image"		: ROOT_PATH + "inventory/remain_time_icon.sub",	
		},

		{
			"name"		: "remain_time_text_bg",
			"type"		: "image",
			"style"		: ("ltr",),
					
			"x"			: 75 - 10 - 1,
			"y"			: 464 - 32 - 418,
			"image"		: ROOT_PATH + "inventory/remain_time_text.sub",
			
			"children"	:
			(
				{ "name":"remain_time_text", "type":"text", "x":0, "y":0, "text":"23:55", "all_align":"center" },	
			),
		},
	),
},)

# 등록하기 버튼
window["children"][0]["children"][0]["children"].append(
{ 
	"name"	: "register_attr_button",
	"type"	: "button",
	"x"		: 35 - 10, "y": 501 - 32,

	"text"			: uiScriptLocale.YIN_AND_YANG_ATTR_INVENTORY_BUTTON_TEXT_3,	# 등록하기
	"default_image"	: ROOT_PATH + "inventory/button_1_default.sub", 
	"over_image"	: ROOT_PATH + "inventory/button_1_over.sub",
	"down_image"	: ROOT_PATH + "inventory/button_1_down.sub",
},)
