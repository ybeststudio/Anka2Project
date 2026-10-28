# ENABLE_UPBRINGING_PET_CONQUEROR_LEVEL
import uiScriptLocale

# ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH:	# 미사용 경로 제거
#LOCALE_PATH				= uiScriptLocale.WINDOWS_PATH
ROOT_PATH				= "d:/ymir work/ui/pet/"

window = {
	"name" : "PetPotentialWindow",
	"style" : ("movable", "float",),
	
	"x" : SCREEN_WIDTH - 500 + 328 -148,
	"y" : SCREEN_HEIGHT - 595 + 240 - 166,

	"width" : 174,
	"height" : 166,

	"children" :
	(
		{
			"name" : "potential_board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : 174,
			"height" : 166,

			# potential
			"title"		: uiScriptLocale.PET_POTENTIAL_TITLE,

			"children" :
			(
				# 테두리
				{
					"name"		: "outline_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 10,
					"y"			: 32,

					"width"		: 152,
					"height"	: 122,

					"children" :
					(
						{
							"name"		: "desc_window_background",
							"type"		: "outline_window",
							"x"			: 0,
							"y"			: 0,
							"width"		: 152,
							"height"	: 122,
						},
					),
				},
				# 잠재력 펫 슬롯
				{
					"name" : "potential_pet_slot_bg_0",
					"type" : "image",

					"x" : 33,
					"y" : 41,
							
					"image"		: ROOT_PATH + "conqueror/potential_pet_slot_bg.sub",

					"children" :
					(
						{
							"name"			: "potential_pet_slot_0",
							"type"			: "grid_table",
							"x"				: 4,
							"y"				: 4,
							"start_index"	: 0,
							"x_count"		: 1,
							"y_count"		: 1,
							"x_step"		: 32,
							"y_step"		: 32,
						},
					),
				},
				{
					"name" : "potential_pet_slot_bg_1",
					"type" : "image",

					"x" : 99,
					"y" : 41,
							
					"image"		: ROOT_PATH + "conqueror/potential_pet_slot_bg.sub",

					"children" :
					(
						{
							"name"			: "potential_pet_slot_1",
							"type"			: "grid_table",
							"x"				: 4,
							"y"				: 4,
							"start_index"	: 0,
							"x_count"		: 1,
							"y_count"		: 1,
							"x_step"		: 32,
							"y_step"		: 32,
						},
					),
				},
				# 잠재력 펫 스킬 슬롯 0
				{
					"name" : "potential_skill_slot_bg_0",
					"type" : "expanded_image",

					"x" : 33,
					"y" : 105,
							
					"image"		: ROOT_PATH + "conqueror/potential_slot_bg.sub",

					"children" :
					(
						{
							"name"			: "potential_skill_slot_0",
							"type"			: "grid_table",
							"x"				: 4,
							"y"				: 4,
							"start_index"	: 0,
							"x_count"		: 1,
							"y_count"		: 1,
							"x_step"		: 32,
							"y_step"		: 32,
						},
					),
				},
				# 잠재력 펫 슬롯 1
				{
					"name" : "potential_skill_slot_bg_1",
					"type" : "image",

					"x" : 99,
					"y" : 105,
							
					"image"		: ROOT_PATH + "conqueror/potential_slot_bg.sub",

					"children" :
					(
						{
							"name"			: "potential_skill_slot_1",
							"type"			: "grid_table",
							"x"				: 4,
							"y"				: 4,
							"start_index"	: 0,
							"x_count"		: 1,
							"y_count"		: 1,
							"x_step"		: 32,
							"y_step"		: 32,
						},
					),
				},
				# 잠재력 슬롯 게이지 배경 이미지 0
				{
					"name" : "potential_gauge_bg_0",
					"type" : "image",
				
					"x" : 32 + 16,
					"y" : 81,
							
					"image" : ROOT_PATH + "conqueror/potential_slot_gauge_bg.sub",
				},

				# 잠재력 슬롯 게이지 0
				{
					"name" : "potential_gauge_0",
					"type" : "expanded_image",

					"x" : 32 + 16,
					"y" : 81,
					
					"image" : ROOT_PATH + "conqueror/potential_slot_gauge.sub",
				},

				# 잠재력 슬롯 게이지 배경 이미지 1
				{
					"name" : "potential_gauge_bg_1",
					"type" : "image",
				
					"x" : 98 + 16,
					"y" : 81,
							
					"image"		: ROOT_PATH + "conqueror/potential_slot_gauge_bg.sub",
				},

				# 잠재력 슬롯 게이지 1
				{
					"name" : "potential_gauge_1",
					"type" : "expanded_image",

					"x" : 98 + 16,
					"y" : 81,
					
					"image" : ROOT_PATH + "conqueror/potential_slot_gauge.sub",
				},
			),				
		},
	),
}
