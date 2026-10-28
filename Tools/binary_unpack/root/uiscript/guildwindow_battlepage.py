import uiScriptLocale
import localeInfo
import app

PUBLIC_PATH					= "d:/ymir work/ui/public/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"
ROOT_PATH					= "d:/ymir work/ui/game/guild_battle/"

WINDOW_WIDTH				= 434
WINDOW_HEIGHT				= 294

MAIN_WINDOW_WIDTH			= 434
MAIN_WINDOW_HEIGHT			= 234

MAIN_WINDOW_PATTERN_X_COUNT	= (MAIN_WINDOW_WIDTH - 32) / 16
MAIN_WINDOW_PATTERN_Y_COUNT	= (MAIN_WINDOW_HEIGHT - 32) / 16

if localeInfo.IsARABIC():
	PREV_BUTTON_POS_X		= 31
	NEXT_BUTTON_POS_X		= 0
else:
	PREV_BUTTON_POS_X		= 375
	NEXT_BUTTON_POS_X		= 406

PREV_BUTTON_POS_Y			= 214
NEXT_BUTTON_POS_Y			= 214

window = {
	"name"		: "GuildWindow_Battle",
	
	"x"			: 10,
	"y"			: 32,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	[
		## background
		{
			"name"		: "guild_battle_desc_window",
			"type"		: "window",
			"style"		: ("ltr", "attach", ),
			
			"x"			: 0,
			"y"			: 0,

			"width"		: MAIN_WINDOW_WIDTH,
			"height"	: MAIN_WINDOW_HEIGHT,

			"children" :
			[
				{
					"name" : "line_window_left_top",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 0,

					"image" : PATTERN_PATH + "border_A_left_top.tga",
				},

				{
					"name" : "line_window_right_top",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : MAIN_WINDOW_WIDTH - 16,
					"y" : 0,

					"image" : PATTERN_PATH + "border_A_right_top.tga",
				},

				{
					"name" : "line_window_left_bottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : MAIN_WINDOW_HEIGHT - 16,

					"image" : PATTERN_PATH + "border_A_left_bottom.tga",
				},

				{
					"name" : "line_window_right_bottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : MAIN_WINDOW_WIDTH - 16,
					"y" : MAIN_WINDOW_HEIGHT - 16,

					"image" : PATTERN_PATH + "border_A_right_bottom.tga",
				},

				{
					"name" : "line_window_top_center",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 0,

					"image" : PATTERN_PATH + "border_A_top.tga",
					"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
				},

				{
					"name" : "line_window_left_center",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 16,

					"image" : PATTERN_PATH + "border_A_left.tga",
					"rect" : (0.0, 0.0, 0, MAIN_WINDOW_PATTERN_Y_COUNT),
				},

				{
					"name" : "line_window_right_center",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : MAIN_WINDOW_WIDTH - 16,
					"y" : 16,

					"image" : PATTERN_PATH + "border_A_right.tga",
					"rect" : (0.0, 0.0, 0, MAIN_WINDOW_PATTERN_Y_COUNT),
				},

				{
					"name" : "line_window_bottom_center",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : MAIN_WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_bottom.tga",
					"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, 0),
				},

				{
					"name" : "line_window_center",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_center.tga",
					"rect" : (0.0, 0.0, MAIN_WINDOW_PATTERN_X_COUNT, MAIN_WINDOW_PATTERN_Y_COUNT),
				},
			],
		},

		## guild battle desc
		{
			"name"		: "guild_battle_desc_button_window",
			"type"		: "window",
			"style"		: ("ltr", "attach", ),
			
			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: MAIN_WINDOW_HEIGHT,

			"children" :
			[
				{
					"name"			: "guild_battle_desc_prev_button",
					"type"			: "button",

					"x"				: PREV_BUTTON_POS_X,
					"y"				: PREV_BUTTON_POS_Y,

					"default_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
				},
				
				{
					"name"			: "guild_battle_desc_next_button",
					"type"			: "button",

					"x"				: NEXT_BUTTON_POS_X,
					"y"				: NEXT_BUTTON_POS_Y,

					"default_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
				},
			],
		},

		## guild battle
		{
			"name"		: "guild_battle_window",
			"type"		: "window",
			"style"		: ("attach",),
			
			"x"			: 0,
			"y"			: MAIN_WINDOW_HEIGHT,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT - MAIN_WINDOW_HEIGHT,

			"children" :
			[
				{
					"name"			: "enter_on_button",
					"type"			: "button",

					"x"				: 5,
					"y"				: 8,

					"default_image"	: ROOT_PATH + "enter_btn_default.sub",
					"over_image"	: ROOT_PATH + "enter_btn_over.sub",
					"down_image"	: ROOT_PATH + "enter_btn_down.sub",
				},

				{
					"name"			: "enter_off_button",
					"type"			: "button",

					"x"				: 5,
					"y"				: 8,

					"default_image"	: ROOT_PATH + "enter_off_btn_default.sub",
					"over_image"	: ROOT_PATH + "enter_off_btn_default.sub",
					"down_image"	: ROOT_PATH + "enter_off_btn_default.sub",
				},

				{
					"name"					: "enter_requirments_bg",
					"type"					: "image",

					"x"						: 117,
					"y"						: 10,

					"image"					: ROOT_PATH + "enter_requirements.sub"
				},

				{
					"name"					: "remain_start_time_text",
					"type"					: "text",

					"x"						: 10,
					"y"						: 16,

					"horizontal_align"		: "center",
					"text_horizontal_align"	: "center",

					"text"					: "remain start time : 00:00",
				},

				{
					"name"					: "is_enter_requiremnts_text",
					"type"					: "text",

					"x"						: 5,
					"y"						: 39,

					"horizontal_align"		: "center",
					"text_horizontal_align"	: "center",

					"text"					: "enter requiremnts",
				},

				{
					"name"					: "is_enter_requirements_check_box_on",
					"type"					: "image",

					"x"						: 270,
					"y"						: 36,

					"image"					: ROOT_PATH + "check_box_on_bg.sub"
				},

				{
					"name"					: "is_enter_requirements_check_box_off",
					"type"					: "image",

					"x"						: 270,
					"y"						: 36,

					"image"					: ROOT_PATH + "check_box_off_bg.sub"
				},

				{
					"name"					: "personal_reward_off",
					"type"					: "button",

					"x"						: 341,
					"y"						: 7,

					"default_image"			: ROOT_PATH + "reward_off_btn_default.sub",
					"over_image"			: ROOT_PATH + "reward_off_btn_default.sub",
					"down_image"			: ROOT_PATH + "reward_off_btn_default.sub",
				},

				{
					"name"					: "personal_reward_on",
					"type"					: "button",

					"x"						: 341,
					"y"						: 7,

					"default_image"			: ROOT_PATH + "reward_on_btn_default.sub",
					"over_image"			: ROOT_PATH + "reward_on_btn_over.sub",
					"down_image"			: ROOT_PATH + "reward_on_btn_down.sub",
				},

				{
					"name"					: "guild_reward_off",
					"type"					: "button",

					"x"						: 341,
					"y"						: 35,

					"default_image"			: ROOT_PATH + "reward_off_btn_default.sub",
					"over_image"			: ROOT_PATH + "reward_off_btn_default.sub",
					"down_image"			: ROOT_PATH + "reward_off_btn_default.sub",
				},

				{
					"name"					: "guild_reward_on",
					"type"					: "button",

					"x"						: 341,
					"y"						: 35,

					"default_image"			: ROOT_PATH + "reward_on_btn_default.sub",
					"over_image"			: ROOT_PATH + "reward_on_btn_over.sub",
					"down_image"			: ROOT_PATH + "reward_on_btn_down.sub",
				},
			],
		},
	],
}