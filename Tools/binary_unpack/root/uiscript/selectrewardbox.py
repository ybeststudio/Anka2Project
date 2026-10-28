#if app.ENABLE_SELECT_REWARD_BOX:

import uiScriptLocale
import localeInfo

ROOT_PATH							= "d:/ymir work/ui/game/select_reward_box/" 
PUBLIC_PATH							= "d:/ymir work/ui/public/" 

WINDOW_WIDTH						= 262
WINDOW_HEIGHT						= 528

window = {
	"name"		: "select_reward_box",
	"style"		: ("movable", "float"),

	"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	(
		### 메인 
		{
			"name"		: "board",
			"type"		: "board_with_titlebar",
			"style"		: ("attach",),

			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"title"		: uiScriptLocale.SELECT_BOX_TITLE,		# 보상 선택 

			"children"	:
			(
				{
					"name"		: "bg",
					"type"		: "outline_window",

					"x"			: 10,
					"y"			: 32,

					"width"		: WINDOW_WIDTH - 20,
					"height"	: WINDOW_HEIGHT - 75,
				},

				### 보상 선택 
				{
					"name"			: "reward_select_bar",
					"type"			: "image",

					"x"				: 12,
					"y"				: 32,

					"image"			: ROOT_PATH + "menu_bar.sub",

					"children"		: 
					(
						## 보상 목록 텍스트 
						{
							"name"					: "reward_item_list_text",
							"type"					: "text",

							"x"						: 120,
							"y"						: 0,

							"vertical_align"		: "center",
							"horizontal_align"		: "left",
							"text_vertical_align"	: "center",
							"text_horizontal_align"	: "center",

							"text"					: uiScriptLocale.SELECT_BOX_LIST,			# 보상 목록 
						}, 

						## 받을 수 있는 목록 체크박스 버튼 
						{
							"name"					: "check_box_uncheck_image",
							"type"					: "image",

							"x"						: 220,
							"y"						: 5,

							"image"					: ROOT_PATH + "check_box_default.sub",
						},

						{
							"name"					: "check_box_check_image",
							"type"					: "image",

							"x"						: 220,
							"y"						: 5,

							"image"					: ROOT_PATH + "check_box_down.sub",
						},
					),
				}, 

				### 슬롯 
				{
					"name"					: "reward_slot", 
					"type"					: "grid_table", 

					"x"						: 18,
					"y"						: 61,

					"start_index"			: 0,

					"x_count"				: 7,										# 7x9 
					"y_count"				: 9,										# 7x9 

					"x_step"				: 32,
					"y_step"				: 32,

					"image"					: PUBLIC_PATH + "Slot_Base.sub"
				}, 

				### 화살표 버튼 (왼쪽) 
				{
					"name"					: "left_arrow_button",
					"type"					: "button",

					"x"						: 22,
					"y"						: 362,
					
					"default_image"			: ROOT_PATH + "arrow_button_left_default.sub",
					"over_image"			: ROOT_PATH + "arrow_button_left_over.sub",
					"down_image"			: ROOT_PATH + "arrow_button_left_down.sub",
				},

				### 화살표 버튼 (오른쪽) 
				{
					"name"					: "right_arrow_button",
					"type"					: "button",

					"x"						: 228,
					"y"						: 362,
					
					"default_image"			: ROOT_PATH + "arrow_button_right_default.sub",
					"over_image"			: ROOT_PATH + "arrow_button_right_over.sub",
					"down_image"			: ROOT_PATH + "arrow_button_right_down.sub",
				},

				### 선택중인 보상 
				{
					"name"					: "selected_item_menu_bar",
					"type"					: "image",

					"x"						: 12,
					"y"						: 385,

					"image"					: ROOT_PATH + "menu_bar.sub",

					"children"				: 
					(
						## 선택중인 보상 텍스트 
						{
							"name"					: "selected_item_menu_bar_text",
							"type"					: "text",

							"x"						: 120,
							"y"						: 0,

							"vertical_align"		: "center",
							"horizontal_align"		: "left",
							"text_vertical_align"	: "center",
							"text_horizontal_align"	: "center",

							"text"					: uiScriptLocale.SELECT_BOX_SELECTED_REWARD,			# 선택한 보상 
						}, 

						### 위치 되돌리기 버튼 
						{
							"name"					: "rewind_button", 
							"type"					: "button", 

							"x"						: 215, 
							"y"						: 2, 

							"default_image"			: ROOT_PATH + "rewind_button_default.sub",
							"over_image"			: ROOT_PATH + "rewind_button_over.sub",
							"down_image"			: ROOT_PATH + "rewind_button_down.sub", 

							"outline_tooltip_text"	: uiScriptLocale.SELECT_BOX_TOOLTIP_REWIND_BUTTON,		# 선택 중인 아이템의 위치로 이동할 수 있습니다.
						}, 

						## 보상 목록 텍스트 
						{
							"name"					: "selected_item_text",
							"type"					: "text",

							"x"						: 120,
							"y"						: 45,

							"vertical_align"		: "center",
							"horizontal_align"		: "left",
							"text_vertical_align"	: "center",
							"text_horizontal_align"	: "center",

							"text"					: uiScriptLocale.SELECT_BOX_SELECTED_ITEM_TEXT_1,			# 보상 목록 중 원하는 아이템을\n선택해주세요. 
						}, 
					),
				},

				### 하단 버튼들 
				{
					"name"					: "recv_button", 
					"type"					: "button", 

					"x"						: 10, 
					"y"						: 494, 

					"default_image"			: ROOT_PATH + "button_default.sub",
					"over_image"			: ROOT_PATH + "button_over.sub",
					"down_image"			: ROOT_PATH + "button_down.sub", 

					"text"					: uiScriptLocale.SELECT_BOX_RECEIVE,					# 받기 
				}, 
				{
					"name"					: "cancel_button", 
					"type"					: "button", 

					"x"						: 130, 
					"y"						: 494, 

					"default_image"			: ROOT_PATH + "button_default.sub",
					"over_image"			: ROOT_PATH + "button_over.sub",
					"down_image"			: ROOT_PATH + "button_down.sub", 

					"text"					: uiScriptLocale.SELECT_BOX_CANCEL,						# 취소  
				}, 
			),
		},
	),
}