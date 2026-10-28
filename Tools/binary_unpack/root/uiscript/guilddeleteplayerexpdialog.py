#if app.ENABLE_GUILD_LAND_RENEWAL:
# 플레이어 경험치 삭제
import uiScriptLocale

WINDOW_WIDTH			= 252
WINDOW_HEIGHT			= 140

PUBLIC_PATH				= "d:/ymir work/ui/public/"
ROOT_PATH				= "d:/ymir work/ui/game/guild/delete_player_exp/"

PATTERN_WINDOW_WIDTH	= 230
PATTERN_WINDOW_HEIGHT	= 68

window = {
	"name" : "GuildDeletePlayerExpDialog",
	"style" : ("movable", "float",),

	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,

	"width" : WINDOW_WIDTH,
	"height" : WINDOW_HEIGHT,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("attach",),

			"x" : 0,
			"y" : 0,

			"width" : WINDOW_WIDTH,
			"height" : WINDOW_HEIGHT,

			"title" : uiScriptLocale.GUILD_DELETE_PLAYER_EXP_DIALOG_TITLE,	# 삭제 할 경험치
			
			"children" :
			(
				## bg pattern window
				{
					"name" : "bg_pattern_window",
					"type" : "outline_window",

					"x" : 10,
					"y" : 32,
					
					"width" : PATTERN_WINDOW_WIDTH,
					"height" : PATTERN_WINDOW_HEIGHT,

					"children" :
					(
						{
							"name" : "delete_exp_window",
							"type" : "window",
							"x" : 0,
							"y" : 10,
							"width" : PATTERN_WINDOW_WIDTH,
							"height" : 22,

							"children" :
							(
								{
									"name"	: "exp_icon",
									"type"	: "image",
									"x"		: 6,
									"y"		: 2,
									"image"	: ROOT_PATH + "exp_icon.sub",
								},

								{
									"name"		: "delete_exp",
									"type"		: "slotbar",
									"x"			: 26,
									"y"			: 0,
									"width"		: 90,
									"height"	: 18,

									"children" :
									(
										{ "name" : "delete_exp_edit_line", "type" : "editline", "x" : 2, "y" : 3, "width" : 90, "height" : 18, "text": "", "input_limit" : 11, "only_number" : 1, },
									),
								},

								{
									"name"	: "slash_img",
									"type"	: "image",
									"x"		: 120,
									"y"		: 3,
									"image"	: ROOT_PATH + "slash_img.sub",
								},

								{
									"name"		: "current_exp",
									"type"		: "slotbar",
									"x"			: 130,
									"y"			: 0,
									"width"		: 90,
									"height"	: 18,

									"children" :
									(
										{ "name" : "current_exp_text", "type" : "text", "x" : 2, "y" : 3, "text": "", },
									),
								},
							),
						},

						# need yang window
						{
							"name" : "need_yang_window",
							"type" : "window",
							"x" : 0,
							"y" : 36,	# 68
							"width" : PATTERN_WINDOW_WIDTH,
							"height" : 22,

							"children" :
							(
								{
									"name"	: "yang_icon",
									"type"	: "image",
									"x"		: 8,
									"y"		: 5,
									"image"	: ROOT_PATH + "yang_icon.sub",
								},

								{
									"name"		: "need_yang",
									"type"		: "slotbar",
									"x"			: 26,
									"y"			: 2,
									"width"		: 90,
									"height"	: 18,

									"children" :
									(
										{ "name" : "need_yang_text", "type" : "text", "x" : 2, "y" : 3, "text": "", "style"	: ("not_pick", ), },
									),
								},

								{
									"name"			: "max_button",
									"type"			: "button",
									"x"				: 131,
									"y"				: 0,
									"default_image"	: ROOT_PATH + "button_max_default.sub",
									"over_image"	: ROOT_PATH + "button_max_over.sub",
									"down_image"	: ROOT_PATH + "button_max_down.sub",
									"text"			: uiScriptLocale.GUILD_DELETE_PLAYER_EXP_DIALOG_MAX_BUTTON_TEXT	# MAX
								},
							),
						},
					),
				},

				{
					"name"			: "confirm_button",
					"type"			: "button",
					"x"				: 42,
					"y"				: 106,
					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
					"text"			: uiScriptLocale.GUILD_DELETE_PLAYER_EXP_DIALOG_CONFIRM_BUTTON_TEXT,	# 확인
				},

				{
					"name"			: "cancel_button",
					"type"			: "button",
					"x"				: 130,
					"y"				: 106,
					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image"	: PUBLIC_PATH + "large_button_02.sub",
					"down_image"	: PUBLIC_PATH + "large_button_03.sub",
					"text"			: uiScriptLocale.GUILD_DELETE_PLAYER_EXP_DIALOG_CANCEL_BUTTON_TEXT,		# 취소
				},
			),
		},
	),
}