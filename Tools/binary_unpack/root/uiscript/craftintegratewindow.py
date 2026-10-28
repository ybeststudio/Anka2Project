# ENABLE_CRAFT_INTEGRATE
import uiScriptLocale
import localeInfo

ROOT_PATH					= "d:/ymir work/ui/game/craft/"

BLANK_SPACE					= 10
TITLE_BAR_SPACE				= 32

MAIN_BOARD_WIDTH			= 602
MAIN_BOARD_HEIGHT			= 482

WINDOW_WIDTH				= MAIN_BOARD_WIDTH + BLANK_SPACE * 2
WINDOW_HEIGHT				= MAIN_BOARD_HEIGHT + BLANK_SPACE + TITLE_BAR_SPACE

RIGHT_BOARD_X				= 325 - BLANK_SPACE # 메인보드 기준이기 때문에 시작점 역계산
MATERIAL_SLOT_MAX			= 6					# CRAFT_MATERIAL_MAX_COUNT

TRY_COUNT_Y					= 230

window = {
	"name"		: "CraftIntegrateWindow",
	"style"		: ("movable", "float", ),
	
	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	[
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			"style"		: ("attach","ltr"),
			
			"x"			: 0,
			"y"			: 0,
			
			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"title"		: "",
			
			"children"	:
			[
				# 메인 보드
				{
					"name"		: "main_board",
					"type"		: "outline_window",
					"x"			: BLANK_SPACE,
					"y"			: TITLE_BAR_SPACE,
					"width"		: MAIN_BOARD_WIDTH,
					"height"	: MAIN_BOARD_HEIGHT,

					"children"	:
					[
						# window["children"][0]["children"][0]["children"]
					],
				},
			],
		},
	],
}

# 배경 이미지
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "backgroung_image",
	"type"		: "image",
	"x"			: 351 - BLANK_SPACE,
	"y"			: 53 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "background_image.sub",	
},)

# 보조재료 슬롯
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "sub_item_slot_image",
	"type"		: "image",
	"x"			: 444 - BLANK_SPACE,
	"y"			: 42 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "sub_material_slot.sub",
	"children"	:
	[
		{
			"name"			: "sub_item_slot",
			"type"			: "grid_table",
			"x"				: 6,
			"y"				: 6,
			"start_index"	: 0,
			"x_count"		: 1,
			"y_count"		: 1,
			"x_step"		: 32,
			"y_step"		: 32,
		},
	],
},)

# 메인재료 슬롯
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "main_material_slot_image",
	"type"		: "image",
	"x"			: 337 - BLANK_SPACE,
	"y"			: 100 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "default_3_slot.sub",	
	"children"	:
	[
		{
			"name"			: "main_material_slot",
			"type"			: "grid_table",
			"x"				: 5,
			"y"				: 5,
			"start_index"	: 0,
			"x_count"		: 1,
			"y_count"		: 3,
			"x_step"		: 32,
			"y_step"		: 32,
		},
	],
},)

# 결과 슬롯
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "result_slot_image",
	"type"		: "image",
	"x"			: 553 - BLANK_SPACE,
	"y"			: 98 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "golden_3_slot.sub",
	"children"	:
	[
		{
			"name"			: "result_slot",
			"type"			: "grid_table",
			"x"				: 7,
			"y"				: 7,
			"start_index"	: 0,
			"x_count"		: 1,
			"y_count"		: 3,
			"x_step"		: 32,
			"y_step"		: 32,
		},
	],
},)

# 게이지
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "gauge_background",
	"type"		: "image",
	"x"			: 386 - BLANK_SPACE,
	"y"			: 127 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "gauge_background.sub",	
	"children"	: [
		# 게이지
		{
			"name"		: "gauge_bar",
			"type"		: "expanded_image",
			"x"			: 0,
			"y"			: 0,
			"image"		: ROOT_PATH + "gauge_bar.sub",	
		},

		# 성공 확률 텍스트
		{
			"name"		: "success_pct_text",
			"type"		: "text",
			"x"			: 0,
			"y"			: 0,
			"text"		: localeInfo.CRAFT_INTEGRATE_SUCCESS_PCT % 0,	# 성공 확률 : %d%%
			"all_align"	: "center",
		},
	],
},)

# 성공 횟수 텍스트
window["children"][0]["children"][0]["children"].append(
{
	"name"					: "succes_count_title_text",
	"type"					: "text",
	"x"						: 445 - BLANK_SPACE,
	"y"						: 168 - TITLE_BAR_SPACE,
	"text_horizontal_align"	: "center",
	"text"					: uiScriptLocale.CRAFT_INTEGRATE_SUCCESS_COUNT, # 성공 횟수
},)

# 실패 횟수 텍스트
window["children"][0]["children"][0]["children"].append(
{
	"name"					: "fail_count_title_text",
	"type"					: "text",
	"x"						: 445 - BLANK_SPACE,
	"y"						: 189 - TITLE_BAR_SPACE,
	"text_horizontal_align"	: "center",
	"text"					: uiScriptLocale.CRAFT_INTEGRATE_FAIL_COUNT, # 실패 횟수
},)

# 성공 횟수 출력폼 이미지
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "success_count_background_image",
	"type"		: "image",
	"x"			: 508 - BLANK_SPACE,
	"y"			: 165 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "count_form_background_image.sub",	
	"children"	:
	[
		{
			"name"		: "success_count_text",
			"type"		: "text",
			"x"			: 0,
			"y"			: 0,
			"text"		: "0",
			"all_align"	: "center",
		},
	],
},)

# 실패 횟수 출력폼 이미지
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "fail_count_background_image",
	"type"		: "image",
	"x"			: 508 - BLANK_SPACE,
	"y"			: 186 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "count_form_background_image.sub",	
	"children"	:
	[
		{
			"name"		: "fail_count_text",
			"type"		: "text",
			"x"			: 0,
			"y"			: 0,
			"text"		: "0",
			"all_align"	: "center",
		},
	],
},)

# 시도횟수 이미지
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "try_count_title",
	"type"		: "image",
	"x"			: 340 - BLANK_SPACE,
	"y"			: TRY_COUNT_Y - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "try_count_title.sub",
	"children"	:
	[
		{
			"name"		: "try_count_title_text",
			"type"		: "text",
			"x"			: 0,
			"y"			: 0,
			"text"		: uiScriptLocale.CRAFT_INTEGRATE_TRY_COUNT, # 시도 횟수
			"all_align"	: "center",
		},
	],
},)

# 시도횟수 카운트 폼이미지
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "try_count_background_image",
	"type"		: "image",
	"x"			: 508 - BLANK_SPACE,
	"y"			: TRY_COUNT_Y + 2 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "count_form_background_image.sub",
	"children"	:
	[
		{
			"name"			: "try_count_editline",
			"type"			: "editline",

			"x"				: 4,
			"y"				: 3,

			"width"			: 35,
			"height"		: 18,

			"input_limit"	: 3,
			"only_number"	: 1,
		},
	],
},)

# 시도횟수 최소 버튼
window["children"][0]["children"][0]["children"].append(
{
	"name"				: "try_min_button",
	"type"				: "button",
	"x"					: 548 - BLANK_SPACE,
	"y"					: TRY_COUNT_Y - TITLE_BAR_SPACE,
	"default_image"		: ROOT_PATH + "min_button_default.sub",
	"over_image"		: ROOT_PATH + "min_button_over.sub",
	"down_image"		: ROOT_PATH + "min_button_down.sub",
},)

# 시도횟수 최대 버튼
window["children"][0]["children"][0]["children"].append(
{
	"name"				: "try_max_button",
	"type"				: "button",
	"x"					: 548 + 24 + 1 - BLANK_SPACE,
	"y"					: TRY_COUNT_Y - TITLE_BAR_SPACE,
	"default_image"		: ROOT_PATH + "max_button_default.sub",
	"over_image"		: ROOT_PATH + "max_button_over.sub",
	"down_image"		: ROOT_PATH + "max_button_down.sub",
},)

# 필요재료 타이틀
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "need_material_title",
	"type"		: "image",
	"x"			: 325 - BLANK_SPACE,
	"y"			: 279 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "material_title_bar.sub",
	"children"	:
	[
		{
			"name"		: "need_material_text",
			"type"		: "text",
			"x"			: 0,
			"y"			: 0,
			"text"		: uiScriptLocale.CRAFT_INTEGRATE_NEED_MATERIAL_TITLE_BAR, # 필요 재료
			"all_align"	: "center",
		},
	],
},)

# 잼 아이콘
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "gem_icon",
	"type"		: "image",
	"x"			: 354 - BLANK_SPACE,
	"y"			: 461 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "gem_icon.sub",	
},)

# 잼 금액
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "gem_price_image",
	"type"		: "image",
	"x"			: 371 - BLANK_SPACE,
	"y"			: 457 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "price_background.sub",
	"children"	:
	[
		{
			"name"					: "gem_price_text",
			"type"					: "text",
			"x"						: 5,
			"y"						: 3,
			"text"					: "0",
			"horizontal_align"		: "right",
			"text_horizontal_align" : "right",
		},
	],
},)

# 양 아이콘
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "yang_icon",
	"type"		: "image",
	"x"			: 473 - BLANK_SPACE,
	"y"			: 460 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "yang_icon.sub",	
},)

# 양 금액
window["children"][0]["children"][0]["children"].append(
{
	"name"		: "yang_price_image",
	"type"		: "image",
	"x"			: 490 - BLANK_SPACE,
	"y"			: 457 - TITLE_BAR_SPACE,
	"image"		: ROOT_PATH + "price_background.sub",
	"children"	:
	[
		{
			"name"					: "yang_price_text",
			"type"					: "text",
			"x"						: 5,
			"y"						: 3,
			"text"					: "0",
			"horizontal_align"		: "right",
			"text_horizontal_align" : "right",
		},
	],
},)

# 실행 버튼
window["children"][0]["children"][0]["children"].append(
{
	"name"			: "run_button",
	"type"			: "button",

	"x"				: 379 - BLANK_SPACE,
	"y"				: 483 - TITLE_BAR_SPACE,

	"text"			: uiScriptLocale.CRAFT_INTEGRATE_BUTTON_RUN,		# 실행

	"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
	"over_image"	: "d:/ymir work/ui/public/large_button_02.sub",
	"down_image"	: "d:/ymir work/ui/public/large_button_03.sub",
},)

# 취소 버튼
window["children"][0]["children"][0]["children"].append(
{
	"name"			: "cancel_button",
	"type"			: "button",

	"x"				: 467 - BLANK_SPACE,
	"y"				: 483 - TITLE_BAR_SPACE,

	"text"			: uiScriptLocale.CRAFT_INTEGRATE_BUTTON_CANCEL,		# 취소

	"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
	"over_image"	: "d:/ymir work/ui/public/large_button_02.sub",
	"down_image"	: "d:/ymir work/ui/public/large_button_03.sub",
},)

# 재료 슬롯 및 카운팅 박스
for i in range(MATERIAL_SLOT_MAX):
	image_name	= "need_material_slot_image_%d" % i
	slot_name	= "need_material_slot_%d" % i
	window["children"][0]["children"][0]["children"].append(
	{
		"name"		: image_name,
		"type"		: "image",
		"x"			: 332 - BLANK_SPACE + 46 * i,
		"y"			: 307 - TITLE_BAR_SPACE,
		"image"		: ROOT_PATH + "default_3_slot.sub",
		"children"	:
		[
			{
				"name"			: slot_name,
				"type"			: "grid_table",
				"x"				: 5,
				"y"				: 5,
				"start_index"	: 0,
				"x_count"		: 1,
				"y_count"		: 3,
				"x_step"		: 32,
				"y_step"		: 32,
			},
		],
	},)

	count_box_image_name	= "count_box_image_%d" % i
	have_count_name			= "have_count_text_%d" % i
	need_count_name			= "need_count_text_%d" % i
	window["children"][0]["children"][0]["children"].append(
	{
		"name"		: count_box_image_name,
		"type"		: "image",
		"x"			: 333 - BLANK_SPACE + 46 * i,
		"y"			: 413 - TITLE_BAR_SPACE,
		"image"		: ROOT_PATH + "price_double_box.sub",
		"children"	:
		[
			{
				"name"		: have_count_name,
				"type"		: "text",
				"x"			: 0,
				"y"			: -35/4 + 1,
				"text"		: "0",
				"all_align"	: "center",
			},
			{
				"name"		: need_count_name,
				"type"		: "text",
				"x"			: 0,
				"y"			: 35/4 + 1,
				"text"		: "0",
				"all_align"	: "center",
			},
		],
	},)