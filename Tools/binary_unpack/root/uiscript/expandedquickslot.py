# if app.ENABLE_QUICKSLOT_IMPROVE
import uiScriptLocale
import localeInfo

ROOT_PATH					= "d:/ymir work/ui/game/taskbar/"

WINDOW_WIDTH				= 318
WINDOW_HEIGHT				= 125

SLOT_COLS					= 4
SLOT_SIZE					= 32

BACKGROUND_MIDDLE_SCALE_Y	= 93.0 / 32.0

window = {
	
	"name"		: "expanded_quickslot_window",
	"style"		: ("float",),

	"x"			: SCREEN_WIDTH / 2 - 113,
	"y"			: SCREEN_HEIGHT - 159,

	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,
	
	"children"	:
	(
		# background images
		{
			"name"		: "background",
			"type"		: "window",

			"style"		: ("attach",),

			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"children"	:
			(
				{
					"name"		: "bg_top",
					"type"		: "image",
				
					"x"			: 0,
					"y"			: 0,
				
					"image"		: ROOT_PATH + "expanded_quickslot_bg_top.sub",
				},
				
				{
					"name"		: "bg_middle",
					"type"		: "expanded_image",
				
					"x"			: 0,
					"y"			: 16,

					"x_scale"	: 1.0, 
					"y_scale"	: BACKGROUND_MIDDLE_SCALE_Y,
				
					"image"		: ROOT_PATH + "expanded_quickslot_bg_middle.sub",
				},
				
				{
					"name"		: "bg_bottom",
					"type"		: "image",
				
					"x"			: 0,
					"y"			: 109,
				
					"image"		: ROOT_PATH + "expanded_quickslot_bg_bottom.sub",
				},

				{
					"name"		: "bg_line1",
					"type"		: "image",

					"x"			: 8,
					"y"			: 43,

					"image"		: ROOT_PATH + "expanded_quickslot_list_line.sub",
				},

				{
					"name"		: "bg_line2",
					"type"		: "image",

					"x"			: 8,
					"y"			: 82,

					"image"		: ROOT_PATH + "expanded_quickslot_list_line.sub",
				},

				{
					"name"		: "bg_line3",
					"type"		: "image",

					"x"			: 8,
					"y"			: 121,

					"image"		: ROOT_PATH + "expanded_quickslot_list_line.sub",
				},

				{
					"name"		: "bg_number_box1",
					"type"		: "image",

					"x"			: 300,
					"y"			: 19,

					"image"		: ROOT_PATH + "expanded_quickslot_number_box.sub",
				},

				{
					"name"		: "bg_number_box2",
					"type"		: "image",

					"x"			: 300,
					"y"			: 58,

					"image"		: ROOT_PATH + "expanded_quickslot_number_box.sub",
				},

				{
					"name"		: "bg_number_box3",
					"type"		: "image",

					"x"			: 300,
					"y"			: 97,

					"image"		: ROOT_PATH + "expanded_quickslot_number_box.sub",
				},
			),
		},

		# 확장 퀵슬롯 (4번)
		{
			"name"			: "quickslot_3",
			"type"			: "window",

			"style"			: ("attach",),

			"x"				: 9,
			"y"				: 8,

			"width"			: WINDOW_WIDTH,
			"height"		: SLOT_SIZE,

			"children"		:
			(
				# 교체 버튼
				{
					"name"			: "switch_button_3",
					"type"			: "button",

					"x"				: 0,
					"y"				: 0,
						
					"default_image" : ROOT_PATH + "expanded_quickslot_switch_button_default.sub",
					"over_image"	: ROOT_PATH + "expanded_quickslot_switch_button_over.sub",
					"down_image"	: ROOT_PATH + "expanded_quickslot_switch_button_down.sub",
				},

				# 왼쪽 슬롯
				{
					"name"			: "expanded_quickslot_line_left_3",
					"type"			: "grid_table",
				
					"x"				: 27 - 9,
					"y"				: 0,
				
					"start_index"	: 0,
				
					"x_count"		: SLOT_COLS,
					"y_count"		: 1,
					
					"x_step"		: SLOT_SIZE,
					"y_step"		: SLOT_SIZE,
				
					"image"			: ROOT_PATH + "expanded_quickslot_slot.sub",
				},
				
				# 오른쪽 슬롯
				{
					"name"			: "expanded_quickslot_line_right_3",
					"type"			: "grid_table",
				
					"x"				: 169 - 9,
					"y"				: 0,
				
					"start_index"	: 4,
				
					"x_count"		: SLOT_COLS,
					"y_count"		: 1,
					
					"x_step"		: SLOT_SIZE,
					"y_step"		: SLOT_SIZE,
				
					"image"			: ROOT_PATH + "expanded_quickslot_slot.sub",
				},

				# 퀵슬롯 번호
				{
					"name"			: "expanded_quickslot_line_number_3",
					"type"			: "image",

					"x"				: 300 + 2 - 9,
					"y"				: 19 + 1 - 8,
					
					"image"			: ROOT_PATH + "4.sub",
				},
			),
		},

		# 확장 퀵슬롯 (3번)
		{
			"name"			: "quickslot_2",
			"type"			: "window",

			"style"			: ("attach",),

			"x"				: 9,
			"y"				: 47,

			"width"			: WINDOW_WIDTH,
			"height"		: SLOT_SIZE,

			"children"		:
			(
				# 교체 버튼
				{
					"name"			: "switch_button_2",
					"type"			: "button",

					"x"				: 0,
					"y"				: 0,
					
						
					"default_image" : ROOT_PATH + "expanded_quickslot_switch_button_default.sub",
					"over_image"	: ROOT_PATH + "expanded_quickslot_switch_button_over.sub",
					"down_image"	: ROOT_PATH + "expanded_quickslot_switch_button_down.sub",
				},

				# 왼쪽 슬롯
				{
					"name"			: "expanded_quickslot_line_left_2",
					"type"			: "grid_table",
				
					"x"				: 27 - 9,
					"y"				: 0,
				
					"start_index"	: 0,
				
					"x_count"		: SLOT_COLS,
					"y_count"		: 1,
					
					"x_step"		: SLOT_SIZE,
					"y_step"		: SLOT_SIZE,
				
					"image"			: ROOT_PATH + "expanded_quickslot_slot.sub",
				},
				
				# 오른쪽 슬롯
				{
					"name"			: "expanded_quickslot_line_right_2",
					"type"			: "grid_table",
				
					"x"				: 169 - 9,
					"y"				: 0,
				
					"start_index"	: 4,
				
					"x_count"		: SLOT_COLS,
					"y_count"		: 1,
					
					"x_step"		: SLOT_SIZE,
					"y_step"		: SLOT_SIZE,
				
					"image"			: ROOT_PATH + "expanded_quickslot_slot.sub",
				},

				# 퀵슬롯 번호
				{
					"name"			: "expanded_quickslot_line_number_2",
					"type"			: "image",

					"x"				: 300 + 2 - 9,
					"y"				: 58 + 1 - 47,
					
					"image"			: ROOT_PATH + "3.sub",
				},
			),
		},

		# 확장 퀵슬롯 (2번)
		{
			"name"			: "quickslot_1",
			"type"			: "window",

			"style"			: ("attach",),

			"x"				: 9,
			"y"				: 86,

			"width"			: WINDOW_WIDTH,
			"height"		: SLOT_SIZE,

			"children"		:
			(
				# 교체 버튼
				{
					"name"			: "switch_button_1",
					"type"			: "button",

					"x"				: 0,
					"y"				: 0,
					
						
					"default_image" : ROOT_PATH + "expanded_quickslot_switch_button_default.sub",
					"over_image"	: ROOT_PATH + "expanded_quickslot_switch_button_over.sub",
					"down_image"	: ROOT_PATH + "expanded_quickslot_switch_button_down.sub",
				},

				# 왼쪽 슬롯
				{
					"name"			: "expanded_quickslot_line_left_1",
					"type"			: "grid_table",
				
					"x"				: 27 - 9,
					"y"				: 0,
				
					"start_index"	: 0,
				
					"x_count"		: SLOT_COLS,
					"y_count"		: 1,
					
					"x_step"		: SLOT_SIZE,
					"y_step"		: SLOT_SIZE,
				
					"image"			: ROOT_PATH + "expanded_quickslot_slot.sub",
				},
				
				# 오른쪽 슬롯
				{
					"name"			: "expanded_quickslot_line_right_1",
					"type"			: "grid_table",
				
					"x"				: 169 - 9,
					"y"				: 0,
				
					"start_index"	: 4,
				
					"x_count"		: SLOT_COLS,
					"y_count"		: 1,
					
					"x_step"		: SLOT_SIZE,
					"y_step"		: SLOT_SIZE,
				
					"image"			: ROOT_PATH + "expanded_quickslot_slot.sub",
				},

				# 퀵슬롯 번호
				{
					"name"			: "expanded_quickslot_line_number_1",
					"type"			: "image",

					"x"				: 300 + 2 - 9,
					"y"				: 97 + 1 - 86,
					
					"image"			: ROOT_PATH + "2.sub",
				},
			),
		},

		# 확장 퀵슬롯 (1번, default : Hide)
		{
			"name"			: "quickslot_0",
			"type"			: "window",

			"style"			: ("attach",),

			"x"				: 0,
			"y"				: 0,

			"width"			: WINDOW_WIDTH,
			"height"		: SLOT_SIZE,

			"children"		:
			(
				# 교체 버튼
				{
					"name"			: "switch_button_0",
					"type"			: "button",

					"x"				: 0,
					"y"				: 0,
					
						
					"default_image" : ROOT_PATH + "expanded_quickslot_switch_button_default.sub",
					"over_image"	: ROOT_PATH + "expanded_quickslot_switch_button_over.sub",
					"down_image"	: ROOT_PATH + "expanded_quickslot_switch_button_down.sub",
				},

				# 왼쪽 슬롯
				{
					"name"			: "expanded_quickslot_line_left_0",
					"type"			: "grid_table",
				
					"x"				: 27 - 9,
					"y"				: 0,
				
					"start_index"	: 0,
				
					"x_count"		: SLOT_COLS,
					"y_count"		: 1,
					
					"x_step"		: SLOT_SIZE,
					"y_step"		: SLOT_SIZE,
				
					"image"			: ROOT_PATH + "expanded_quickslot_slot.sub",
				},
				
				# 오른쪽 슬롯
				{
					"name"			: "expanded_quickslot_line_right_0",
					"type"			: "grid_table",
				
					"x"				: 169 - 9,
					"y"				: 0,
				
					"start_index"	: 4,
				
					"x_count"		: SLOT_COLS,
					"y_count"		: 1,
					
					"x_step"		: SLOT_SIZE,
					"y_step"		: SLOT_SIZE,
				
					"image"			: ROOT_PATH + "expanded_quickslot_slot.sub",
				},

				# 퀵슬롯 번호
				{
					"name"			: "expanded_quickslot_line_number_0",
					"type"			: "image",

					"x"				: 300 + 2 - 9,
					"y"				: 97 + 1 - 86,
					
					"image"			: ROOT_PATH + "1.sub",
				},
			),
		},
	),
}