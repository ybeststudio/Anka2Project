import uiScriptLocale
import localeInfo

PUBLIC_PATH					= "d:/ymir work/ui/public/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"
ROOT_PATH					= "d:/ymir work/ui/game/guild_battle/"

WINDOW_WIDTH				= 434
WINDOW_HEIGHT				= 294

WINDOW_PATTERN_X_COUNT		= (WINDOW_WIDTH - 32) / 16
WINDOW_PATTERN_Y_COUNT		= (WINDOW_HEIGHT - 32) / 16


window = {
	"name"		: "guild_contents_list_window",
	"style"		: ("float",),

	"x"			: 10,
	"y"			: 32,
	
	"width"		: WINDOW_WIDTH,
	"height"	: WINDOW_HEIGHT,

	"children" :
	[
		## background
		{
			"name"		: "line_window",
			"type"		: "window",
			"style"		: ("ltr", "attach", ),
			
			"x"			: 0,
			"y"			: 0,

			"width"		: WINDOW_WIDTH,
			"height"	: WINDOW_HEIGHT,

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
					
					"x" : WINDOW_WIDTH - 16,
					"y" : 0,

					"image" : PATTERN_PATH + "border_A_right_top.tga",
				},

				{
					"name" : "line_window_left_bottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : WINDOW_HEIGHT - 16,

					"image" : PATTERN_PATH + "border_A_left_bottom.tga",
				},

				{
					"name" : "line_window_right_bottom",
					"type" : "image",
					"style" : ("ltr",),
					
					"x" : WINDOW_WIDTH - 16,
					"y" : WINDOW_HEIGHT - 16,

					"image" : PATTERN_PATH + "border_A_right_bottom.tga",
				},

				{
					"name" : "line_window_top_center",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 0,

					"image" : PATTERN_PATH + "border_A_top.tga",
					"rect" : (0.0, 0.0, WINDOW_PATTERN_X_COUNT, 0),
				},

				{
					"name" : "line_window_left_center",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 0,
					"y" : 16,

					"image" : PATTERN_PATH + "border_A_left.tga",
					"rect" : (0.0, 0.0, 0, WINDOW_PATTERN_Y_COUNT),
				},

				{
					"name" : "line_window_right_center",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : WINDOW_WIDTH - 16,
					"y" : 16,

					"image" : PATTERN_PATH + "border_A_right.tga",
					"rect" : (0.0, 0.0, 0, WINDOW_PATTERN_Y_COUNT),
				},

				{
					"name" : "line_window_bottom_center",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : WINDOW_HEIGHT - 16,
					"image" : PATTERN_PATH + "border_A_bottom.tga",
					"rect" : (0.0, 0.0, WINDOW_PATTERN_X_COUNT, 0),
				},

				{
					"name" : "line_window_center",
					"type" : "expanded_image",
					"style" : ("ltr",),
					
					"x" : 16,
					"y" : 16,
					"image" : PATTERN_PATH + "border_A_center.tga",
					"rect" : (0.0, 0.0, WINDOW_PATTERN_X_COUNT, WINDOW_PATTERN_Y_COUNT),
				},
			],
		},

		# scroll bar
		{
			"name"				: "guild_contents_list_scroll_bar",
			"type"				: "scrollbar",

			"x"					: 17,
			"y"					: 3,
			"size"				: 289,
			"horizontal_align"	: "right",
		},
	],
}