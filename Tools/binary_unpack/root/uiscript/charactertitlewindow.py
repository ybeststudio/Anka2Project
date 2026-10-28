# ENABLE_TITLE_SYSTEM
import uiScriptLocale

ROOT_PATH					= "d:/ymir work/ui/game/title/window/"

LEFTBOARD_WIDTH				= 224
LEFTBOARD_HEIGHT			= 260
LEFTBOARD_X					= 12
LEFTBOARD_Y					= 32

RIGHTBOARD_WIDTH			= 226
RIGHTBOARD_HEIGHT			= 346
RIGHTBOARD_X				= 240
RIGHTBOARD_Y				= 32

LEFT_BOTTOM_BOARD_HEIGHT	= RIGHTBOARD_HEIGHT - LEFTBOARD_HEIGHT
LEFT_BOTTOM_BOARD_Y			= LEFTBOARD_HEIGHT + LEFTBOARD_Y - 1
BUTTON_Y					= LEFT_BOTTOM_BOARD_HEIGHT - (21 + 8)	# 보드높이 - (버튼 높이 + 아래 여백)

MAINBOARD_WIDTH				= RIGHTBOARD_X + RIGHTBOARD_WIDTH + LEFTBOARD_X
MAINBOARD_HEIGHT			= RIGHTBOARD_HEIGHT + 46


window = {
	"name"		: "CharacterTitleWindow",
	"style"		: ("movable", "float",),
	
	"x"			: 24 + 253 - 3,						# 캐릭터창 x 위치 + width + 보정
	"y"			: (SCREEN_HEIGHT - 37 - 361) / 2,	# 캐릭터창 y 위치

	"width"		: MAINBOARD_WIDTH,
	"height"	: MAINBOARD_HEIGHT,
	
	"children"	:
	[
		# 전체 보드
		{
			"name"		: "main_board",
			"type"		: "board",
			"style"		: ("attach","ltr"),

			"x"			: 0,
			"y"			: 0,

			"width"		: MAINBOARD_WIDTH,
			"height"	: MAINBOARD_HEIGHT,
			
			"children"	:
			[
				# 메인 타이틀바
				{
					"name"	: "main_title_bar",
					"type"	: "titlebar",
					"style" : ("attach",),
					"x"		: 6,
					"y"		: 7,
					"width" : MAINBOARD_WIDTH - 13,
					
					"children" :
					[
						{ 
							"name"		: "title_name", 
							"type"		: "text",
							"x"			: 0,
							"y"			: 0,
							"text"		: uiScriptLocale.TITLE_SYSTEM_UI_WINDOW_NAME,	# 타이틀
							"all_align"	:"center" 
						},	
					],
				},
				
				# 왼쪽 리스트 보드
				{
					"name"		: "list_board",
					"type"		: "thinboard_circle",
					"x"			: LEFTBOARD_X,
					"y"			: LEFTBOARD_Y,
					"width"		: LEFTBOARD_WIDTH,
					"height"	: LEFTBOARD_HEIGHT,
					
					"children"	: 
					[
						# 목록바
						{
							"name"		: "list_bar",
							"type"		: "image",
							
							"x"			: 1,
							"y"			: 0,
							"image"		: ROOT_PATH + "title_list_bar.sub",
							
							"children"	:
							[
								# 내부 텍스트
								{ 
									"name"		: "list_text",
									"type"		: "text", 
									"x"			: 0, 
									"y"			: 0, 
									"text"		: uiScriptLocale.TITLE_SYSTEM_UI_TITLE_LIST,	# 타이틀 목록
									"all_align"	:"center"
								},
							],
						},

						# 스크롤 바
						{
							"name"				: "scroll_bar",
							"type"				: "scrollbar",
							"x"					: 17,
							"y"					: 24,
							"size"				: LEFTBOARD_HEIGHT - 25,
							"horizontal_align"	: "right",
						},
					],
				},

				# 왼쪽 현재 타이틀 보드
				{
					"name"		: "current_board",
					"type"		: "thinboard_circle",
					"x"			: LEFTBOARD_X,
					"y"			: LEFT_BOTTOM_BOARD_Y,
					"width"		: LEFTBOARD_WIDTH,
					"height"	: LEFT_BOTTOM_BOARD_HEIGHT,
					
					"children"	: 
					[
						# 현재 타이틀바
						{
							"name"		: "current_bar",
							"type"		: "image",
							
							"x"			: 1,
							"y"			: 0,
							"image"		: ROOT_PATH + "title_list_bar.sub",
							
							"children"	:
							[
								# 내부 텍스트
								{ 
									"name"		: "list_text",
									"type"		: "text", 
									"x"			: 0, 
									"y"			: 0, 
									"text"		: uiScriptLocale.TITLE_SYSTEM_UI_CURRENT_TITLE,	# 현재 타이틀
									"all_align"	:"center"
								},
							],
						},

						# 현재 타이틀
						{
							"name"		: "equipped_title",
							"type"		: "image",
							
							"x"			: 12,
							"y"			: 28,	# 23 + 5 (current_bar 이미지 높이 + 여백)
							"image"		: ROOT_PATH + "current_title.sub",
							
							"children"	:
							[
								# 내부 텍스트
								{ 
									"name"		: "equipped_name",
									"type"		: "text", 
									"x"			: 0, 
									"y"			: 0, 
									"text"		: "",
									"all_align"	:"center"
								},
							],
						},

						# 돌아가기 버튼
						{
							"name"			: "back_button",
							"type"			: "button",
							
							"x"				: 195,
							"y"				: 28,	# 23 + 5 (current_bar 이미지 높이 + 여백)

							"default_image" : ROOT_PATH + "back_button_default.sub",
							"over_image"	: ROOT_PATH + "back_button_over.sub",
							"down_image"	: ROOT_PATH + "back_button_down.sub",
						},

						## 적용 버튼
						{
							"name"			: "apply_button",
							"type"			: "button",

							"x"				: 12,
							"y"				: BUTTON_Y,

							"text"			: uiScriptLocale.TITLE_SYSTEM_UI_BUTTON_APPLY,		# 적용

							"default_image" : ROOT_PATH + "button_default.sub",
							"over_image"	: ROOT_PATH + "button_over.sub",
							"down_image"	: ROOT_PATH + "button_down.sub",
							"disable_image" : ROOT_PATH + "button_default.sub",
						},
						
						## 해제 버튼
						{
							"name"			: "clear_button",
							"type"			: "button",

							"x"				: 115,
							"y"				: BUTTON_Y,

							"text"			: uiScriptLocale.TITLE_SYSTEM_UI_BUTTON_CLEAR,	# 해제

							"default_image" : ROOT_PATH + "button_default.sub",
							"over_image"	: ROOT_PATH + "button_over.sub",
							"down_image"	: ROOT_PATH + "button_down.sub",
							"disable_image" : ROOT_PATH + "button_default.sub",
						},
					],
				},
							
				# 오른쪽 보드
				{
					"name"		: "right_board",
					"type"		: "thinboard_circle",
					"x"			: RIGHTBOARD_X,
					"y"			: RIGHTBOARD_Y,
					"width"		: RIGHTBOARD_WIDTH,
					"height"	: RIGHTBOARD_HEIGHT,
					
					"children"	: 
					[					
						# 렌더 타겟
						{
							"name"		: "model_render_target",
							"type"		: "render_target",
										
							"x"			: 0,
							"y"			: 0,
									
							"width"		: 224,
							"height"	: 346,
									
							"index"		: 4,	# TARGET_TITLE_UI
						},
					],
				},
			],
		},
	],
}