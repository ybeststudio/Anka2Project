import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/minigame/challenge/"

BOARD_WIDTH		= 434
BOARD_HEIGHT	= 312

window = {
	"name" : "MiniGameChallengeRewardPage",
	"style" : ("attach",),
	
	"x" : 0,
	"y" : 0,
	
	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,
	
	"children" :
	(
		## vertical line
		{
			"name" : "vertical_line",
			"type" : "image",
			"x" : 122,
			"y" : 0,
			"image" : ROOT_PATH + "vertical_line.sub",
		},
						
		## level title text window
		{
			"name" : "level_title_text_window", "type" : "window", "x" : 13-10, "y" : 35-32, "width" : 119, "height" : 21, "style" : ("attach",),
			"children" :
			(
				{"name":"level_title_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "name_bg.sub", "style" : ("attach",), },
				{"name":"level_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_REWARD_TAB_TITLE_LEVEL, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		
		## level text window
		{
			"name" : "level_text_window", "type" : "window", "x" : 20-10, "y" : 63-32, "width" : 105, "height" : 49, "style" : ("attach",),
			"children" :
			(
				{"name":"level_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "level_bg.sub"},
				{"name":"levelup_highlight_img", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "level_bg_highlight.sub"},
				{"name":"level_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_REWARD_TAB_LEVEL, "all_align" : "center", "color" : 0xFFD2C4AD, "fontsize":"LARGE", "outline" : 1, },
			),
		},
		
		## use exp window
		{
			"name" : "use_exp_gauge_window", "type" : "window", "x" : 20-10, "y" : 120-32, "width" : 105, "height" : 19, "style" : ("attach",),
			"children" :
			(
				{"name":"use_exp_gauge_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "use_exp_gauge_bg.sub", "style" : ("attach",),},				
				{"name":"use_exp_gauge", "type":"expanded_image", "x":2, "y":2, "image" : ROOT_PATH + "use_exp_gauge.sub", "style" : ("attach",),},
				{"name":"use_exp_gauge_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_REWARD_TAB_USE_EXP_GAUGE_TEXT % 0, "all_align" : "center", "color" : 0xFFFFFFFF,},
			),
		},
		
		## exp use button				
		{
			"name" : "exp_use_button",
			"type" : "button",
			"x" : 17-10,
			"y" : 152-32,
			
			"text" : uiScriptLocale.MINI_GAME_CHALLENGE_REWARD_TAB_USE_CHALLENGE_EXP_TEXT,
			"text_color" : 0xFFD2C4AD,
			
			"default_image" : ROOT_PATH + "challenge_button_default.sub",
			"over_image"	: ROOT_PATH + "challenge_button_over.sub",
			"down_image"	: ROOT_PATH + "challenge_button_down.sub",
			"disable_image" : ROOT_PATH + "challenge_button_down.sub",
		},
		
		## 도전 경험치 bg
		{
			"name" : "challenge_exp_title_bg",
			"type" : "image",
			"style" : ("attach",),
			"x" : 13-10,
			"y" : 210-32,
			"image" : ROOT_PATH + "name_value_bg.sub",
		},
		## 도전 경험치 title
		{
			"name" : "challenge_exp_title_text_window", "type" : "window", "x" : 14-10, "y" : 214-32, "width" : 117, "height" : 12, "style" : ("attach",),
			"children" :
			(						
				{"name":"challenge_exp_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_REWARD_TAB_CHALLENGE_EXP_TITLT_TEXT, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		## 도전 경험치 수치 표기 text
		{
			"name" : "challenge_exp_text_window", "type" : "window", "x" : 14-10, "y" : 233-32, "width" : 117, "height" : 12, "style" : ("attach",),
			"children" :
			(						
				{"name":"challenge_exp_text", "type":"text", "x":0, "y":0, "text": "0", "all_align" : "center", "color" : 0xFFD2C4AD, },
			),
		},
		
		## tab division text bg
		{
			"name" : "tab_division_text_bg",
			"type" : "image",
			"style" : ("attach",),
			"x" : 135-10,
			"y" : 35-32,
			"image" : ROOT_PATH + "reward_tab_text_bg.sub",
		},
		
		## 레벨 title text
		{
			"name" : "level_title_text_window", "type" : "window", "x" : 140-10, "y" : 39-32, "width" : 62, "height" : 13, "style" : ("attach",),
			"children" :
			(
				{"name":"level_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_REWARD_TAB_LEVEL_TEXT, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		## 일반 title text
		{
			"name" : "common_title_text_window", "type" : "window", "x" : 214-10, "y" : 39-32, "width" : 95, "height" : 13, "style" : ("attach",),
			"children" :
			(
				{"name":"common_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_REWARD_TAB_COMMON_TEXT, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		## 프리미엄 title text
		{
			"name" : "premium_title_text_window", "type" : "window", "x" : 324-10, "y" : 39-32, "width" : 95, "height" : 13, "style" : ("attach",),
			"children" :
			(
				{"name":"premium_title_text", "type":"text", "x":0, "y":0, "text": uiScriptLocale.MINI_GAME_CHALLENGE_REWARD_TAB_PREMIUM, "all_align" : "center", "color" : 0xFFD9D9D9, },
			),
		},
		
		## 보상 리스트 표시 영역
		{
			"name" : "reward_list_window",
			"type" : "window",
			
			"style" : ("attach", "not_pick",),

			"x" : 137-10,
			"y" : 58-32,
			
			"width" : 289,
			"height" : 281,
		},
		
		## 보상 리스트 scroll bar
		{
			"name" : "reward_list_scrollbar",
			"type" : "scrollbar",

			"x" : 427-10,
			"y" : 59-32,
			"size" : 280,
		},
		
		## clock img
		{
			"name" : "clock_img",
			"type" : "image",
			"x" : 16-10,
			"y" : 317-32,
			"image" : ROOT_PATH + "clock_img.sub",
		},
		
		## clock text window
		{
			"name" : "clock_text_window", "type" : "window", "x" : 36-10, "y" : 317-32, "width" : 92, "height" : 18, "style" : ("attach",),
			"children" :
			(
				{"name":"clock_text_bg", "type":"image", "x":0, "y":0, "image" : ROOT_PATH + "time_text_bg.sub"},
				{"name":"clock_text", "type":"text", "x":0, "y":0, "text": "", "all_align" : "center", },
			),
		},
		
		## cycle end time desc area window
		{
			"name" : "cycle_end_time_desc_area_window",
			"type" : "window",
			"x" : 16-10,
			"y" : 317-32,
			"width" : 112,
			"height" : 18,
		},
		
		## level up effect
		{
			"name" : "critical_effect",
			"type" : "ani_image",
			
			"x" : 17,
			"y" : 18,
			
			"x_scale" : 128.0 * 0.7 / 128.0,
			"y_scale" : 128.0 * 0.7 / 128.0,
			
			"delay" : 4,

			"images" :
			(
				"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff1.sub",
				"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff2.sub",
				"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff3.sub",
				"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff4.sub",
				"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff5.sub",
				"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff6.sub",
				"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff7.sub",
				"D:/Ymir Work/UI/minigame/rumi/card_completion_effect/card_completion_eff8.sub",
			),
		},
	),	
}
