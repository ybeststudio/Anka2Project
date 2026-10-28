#app.ENABLE_USER_REPORT_SYSTEM
import uiScriptLocale

ROOT_PATH			= "d:/ymir work/ui/game/user_report/"

WINDOW_WIDTH		= 280
WINDOW_HEIGHT		= 194

SUBWINDOW_WIDTH		= 258
SUBWINDOW_HEIGHT	= 61

window = {
	"name"		: "ReportWindow",
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

			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

			"title"		: uiScriptLocale.USER_REPORT_SYSTEM_UI_WINDOW_NAME,	# 신고

			"children"	:
			[
				# 대상 확인
				{
					"name"		: "target_board",
					"type"		: "outline_window",
					"x"			: 11,
					"y"			: 32,
					"width"		: SUBWINDOW_WIDTH,
					"height"	: SUBWINDOW_HEIGHT,

					"children"	:
					[
						# 상단 바 이미지
						{
							"name"		: "target_bar",
							"type"		: "image",
							
							"x"			: 2,
							"y"			: 0,
							"image"		: ROOT_PATH + "title_list_bar.sub",
							
							"children"	:
							[
								# 내부 텍스트
								{ 
									"name"		: "target_text",
									"type"		: "text", 
									"x"			: 0, 
									"y"			: 0, 
									"text"		: uiScriptLocale.USER_REPORT_SYSTEM_UI_TARGET_CHECK,	# 대상 확인
									"all_align"	: "center",
								},
							],
						},

						# 캐릭터명 타이틀
						{
							"name"		: "name_title",
							"type"		: "image",
							
							"x"			: 10,
							"y"			: 30,
							"image"		: ROOT_PATH + "name_title.sub",

							"children"	:
							[
								# 내부 텍스트
								{ 
									"name"		: "name_text",
									"type"		: "text", 
									"x"			: 0, 
									"y"			: 0, 
									"text"		: uiScriptLocale.USER_REPORT_SYSTEM_UI_CHARACTER_NAME,	# 캐릭터 명
									"all_align"	: "center",
								},
							],
						},

						# 캐릭터명 슬롯
						{
							"name"		: "target_name_slot",
							"type"		: "image",
							"x"			: 123,
							"y"			: 30,
							"image"		: ROOT_PATH + "name_slot.sub",
							"children"	:
							[
								{
									"name"			: "name_slot",
									"type"			: "text",
									"x"				: 4,
									"y"				: 3,
								},
							],
						},
					],
				},

				# 신고 사유 선택
				{
					"name"		: "report_board",
					"type"		: "outline_window",
					"x"			: 11,
					"y"			: 30 + SUBWINDOW_HEIGHT,
					"width"		: SUBWINDOW_WIDTH,
					"height"	: SUBWINDOW_HEIGHT,
				
					"children"	:
					[
						# 상단 바 이미지
						{
							"name"		: "report_bar",
							"type"		: "image",
							
							"x"			: 2,
							"y"			: 0,
							"image"		: ROOT_PATH + "title_list_bar.sub",
							
							"children"	:
							[
								# 내부 텍스트
								{ 
									"name"			: "target_text",
									"type"			: "text", 
									"x"				: 0, 
									"y"				: 0, 
									"text"			: uiScriptLocale.USER_REPORT_SYSTEM_UI_SELECT_REASON,	# 신고 사유 선택
									"all_align"		: "center",
								},
							],
						},

						# 신고사유 버튼(원/냥 불법판매)
						{
							"name"					: "button_illegal_seller",
							"type"					: "toggle_button",
					
							"x"						: 20,
							"y"						: 28,

							"default_image"			: ROOT_PATH + "illegal_seller_button_default.sub",
							"over_image"			: ROOT_PATH + "illegal_seller_button_over.sub",
							"down_image"			: ROOT_PATH + "illegal_seller_button_down.sub",
						},

						# 신고사유 버튼(봇 사용)
						{
							"name"					: "button_bot_using",
							"type"					: "toggle_button",
					
							"x"						: 51,
							"y"						: 28,

							"default_image"			: ROOT_PATH + "bot_user_button_default.sub",
							"over_image"			: ROOT_PATH + "bot_user_button_over.sub",
							"down_image"			: ROOT_PATH + "bot_user_button_down.sub",
						},

						# 신고사유 버튼(부정 행위)
						{
							"name"					: "button_cheating",
							"type"					: "toggle_button",
					
							"x"						: 82,
							"y"						: 28,

							"default_image"			: ROOT_PATH + "cheater_button_default.sub",
							"over_image"			: ROOT_PATH + "cheater_button_over.sub",
							"down_image"			: ROOT_PATH + "cheater_button_down.sub",
						},
					],
				},

				# 보내기 버튼
				{
					"name"			: "send_button",
					"type"			: "button",

					"x"				: 52,
					"y"				: 159,

					"text"			: uiScriptLocale.USER_REPORT_SYSTEM_UI_SEND_BUTTON,		# 보내기

					"default_image" : ROOT_PATH + "button_default.sub",
					"over_image"	: ROOT_PATH + "button_over.sub",
					"down_image"	: ROOT_PATH + "button_down.sub",
					"disable_image" : ROOT_PATH + "button_default.sub",
				},

				# 닫기 버튼
				{
					"name"			: "close_button",
					"type"			: "button",

					"x"				: 140,
					"y"				: 159,

					"text"			: uiScriptLocale.USER_REPORT_SYSTEM_UI_CLOSE_BUTTON,		# 닫기

					"default_image" : ROOT_PATH + "button_default.sub",
					"over_image"	: ROOT_PATH + "button_over.sub",
					"down_image"	: ROOT_PATH + "button_down.sub",
					"disable_image" : ROOT_PATH + "button_default.sub",
				},
			],
		},
	],
}
