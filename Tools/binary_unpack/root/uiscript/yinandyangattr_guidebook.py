import uiScriptLocale

ROOT_PATH			= "d:/ymir work/ui/game/yin_and_yang_attr/"

WINDOW_WIDTH		= 590
WINDOW_HEIGHT		= 422

DESC_WINDOW_WIDTH	= 279
DESC_WINDOW_HEIGHT	= 222

window = {
	"name"		: "YinAndYangAttr_GuideBook",
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
			"title"		: uiScriptLocale.YIN_AND_YANG_ATTR_GUIDE_BOOK_TITLE,	# 음양속성 도감

			"children"	:
			[
				{
					"name"		: "bg_window",
					"type"		: "outline_window",
					"x"			: 10,
					"y"			: 32,
					"width"		: 568, #578,
					"height"	: 378, #410,

					"children"	:
					[
						# window["children"][0]["children"][0]["children"]
					],
				},
			],
		},
	),
}

# 메뉴 윈도우
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "menu_parent_window",
	"type"		: "window",
	"style"		: ("ltr",),					
	"x"			: 4,
	"y"			: 4,
	"width"		: 277,
	"height"	: 374,

	"children"	:
	(
		{
			"name"		: "menu_window",
			"type"		: "window",
			"style"		: ("ltr",),
			"x"			: 0,
			"y"			: 0,
			"width"		: 277,
			"height"	: 374,
		},

		{
			"name"				: "menu_scroll_bar",
			"type"				: "scrollbar",
			"x"					: 15,
			"y"					: 0,
			"size"				: 372,
			"horizontal_align"	: "right",
		},
	),
},)

# 세로 구분선
window["children"][0]["children"][0]["children"].append(
{
	"name"	: "vertical_line",
	"type"	: "image",
	"style"	: ("ltr",),		
	"x"		: 291 - 10,
	"y"		: 34 - 32,
	"image"	: ROOT_PATH + "guide_book/bg_vertical_line.sub",
},)

window["children"][0]["children"][0]["children"].append(
{
	"name"	: "attr_slot_title_bar",
	"type"	: "image",
	"style"	: ("ltr",),		
	"x"		: 293 - 10,
	"y"		: 32 - 32,
	"image"	: ROOT_PATH + "guide_book/title_bar_bg.sub",

	"children"	:
	[
		{
			"name"		: "attr_slot_title_text",
			"type"		: "text",
			"x"			: 0,
			"y"			: 0,
			"all_align" : "center",
			"text"		: uiScriptLocale.YIN_AND_YANG_ATTR_GUIDE_BOOK_ATTR_SLOTS_TITLE,	# 속성
		},
	],
},)

window["children"][0]["children"][0]["children"].append(
{
	"name"			: "attr_slot",
	"type"			: "grid_table",
	"x"				: 299 - 10,
	"y"				: 60 - 32,
	"start_index"	: 0,
	"x_count"		: 8,
	"y_count"		: 3,
	"x_step"		: 32,
	"y_step"		: 32,

	"image"			: "d:/ymir work/ui/public/Slot_Base.sub"
},)

window["children"][0]["children"][0]["children"].append(
{
	"name"	: "attr_slot_scroll_bar",
	"type"	: "scrollbar",
	"x"		: 560 - 10,
	"y"		: 56 - 32,
	"size"	: 104,
},)

window["children"][0]["children"][0]["children"].append(
{
	"name"	: "description_title_bar",
	"type"	: "image",
	"style"	: ("ltr",),
	"x"		: 293 - 10,
	"y"		: 161 - 32,
	"image"	: ROOT_PATH + "guide_book/title_bar_bg.sub",

	"children"	:
	[
		{
			"name"		: "attr_slot_title_text",
			"type"		: "text",
			"x"			: 0,
			"y"			: 0,
			"all_align" : "center",
			"text"		: uiScriptLocale.YIN_AND_YANG_ATTR_GUIDE_BOOK_DESCRIPTION_TITLE,	# 도움말
		},
	],
},)

window["children"][0]["children"][0]["children"].append(
{
	"name"		: "description_window",
	"type"		: "window",
	"style"		: ("ltr",),
	"x"			: 295 - 10,
	"y"			: 185 - 32,
	"width"		: DESC_WINDOW_WIDTH,
	"height"	: DESC_WINDOW_HEIGHT,
},)