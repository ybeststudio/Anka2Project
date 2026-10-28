import uiScriptLocale

PUBLIC_PATH					= "d:/ymir work/ui/public/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"
ROOT_PATH					= "d:/ymir work/ui/minigame/otherworld/"

WINDOW_WIDTH				= 336
WINDOW_HEIGHT				= 306

DESC_WINDOW_WIDTH			= 310
DESC_WINDOW_HEIGHT			= 185
DESC_WINDOW_PATTERN_X_COUNT	= (DESC_WINDOW_WIDTH - 32) / 16
DESC_WINDOW_PATTERN_Y_COUNT	= (DESC_WINDOW_HEIGHT - 32) / 16

window = {
	"name"	: "OtherWorldEventWindow",
	"style" : ("movable", "float", ),
	
	"x" : SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
	"y" : SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
	
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
			## 저승 이벤트
			"title"		: uiScriptLocale.OTHER_WORLD_EVENT_TITLE,
			
			"children" :
			[
				{
					"name"		: "desc_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 12,
					"y"			: 34,
					
					"width"		: DESC_WINDOW_WIDTH,
					"height"	: DESC_WINDOW_HEIGHT,
					
					"children"	:
					[
						## LeftTop 1
						{
							"name" : "DescWindowLeftTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_left_top.tga",
						},
						
						## RightTop 2
						{
							"name" : "DescWindowRightTop",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : DESC_WINDOW_WIDTH - 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_right_top.tga",
						},
						
						## LeftBottom 3
						{
							"name" : "DescWindowLeftBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : DESC_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_left_bottom.tga",
						},
						
						## RightBottom 4
						{
							"name" : "ListWindowRightBottom",
							"type" : "image",
							"style" : ("ltr",),
							
							"x" : DESC_WINDOW_WIDTH - 16,
							"y" : DESC_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_right_bottom.tga",
						},
						
						## topcenterImg 5
						{
							"name" : "DescWindowTopCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 0,
							"image" : PATTERN_PATH + "border_A_top.tga",
							"rect" : (0.0, 0.0, DESC_WINDOW_PATTERN_X_COUNT, 0),
						},
						
						## leftcenterImg 6
						{
							"name" : "DescWindowLeftCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 0,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_left.tga",
							"rect" : (0.0, 0.0, 0, DESC_WINDOW_PATTERN_Y_COUNT),
						},
						
						## rightcenterImg 7
						{
							"name" : "DescWindowRightCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : DESC_WINDOW_WIDTH - 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_right.tga",
							"rect" : (0.0, 0.0, 0, DESC_WINDOW_PATTERN_Y_COUNT),
						},
						
						## bottomcenterImg 8
						{
							"name" : "DescWindowBottomCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : DESC_WINDOW_HEIGHT - 16,
							"image" : PATTERN_PATH + "border_A_bottom.tga",
							"rect" : (0.0, 0.0, DESC_WINDOW_PATTERN_X_COUNT, 0),
						},
						
						## centerImg
						{
							"name" : "DescWindowCenterImg",
							"type" : "expanded_image",
							"style" : ("ltr",),
							
							"x" : 16,
							"y" : 16,
							"image" : PATTERN_PATH + "border_A_center.tga",
							"rect" : (0.0, 0.0, DESC_WINDOW_PATTERN_X_COUNT, DESC_WINDOW_PATTERN_Y_COUNT),
						},
					],
				},
				
				{
					"name"		: "desc_board",
					"type"		: "bar",
					
					"x"			: 12 + 2,
					"y"			: 34 + 2,
					
					"width"		: DESC_WINDOW_WIDTH - 4,
					"height"	: DESC_WINDOW_HEIGHT -4,
				},
				
				## 전야제 남은 기간
				{
					"name" : "eve_event_time_label_text_window",
					"type" : "window",
					
					"x" : 12,
					"y" : 226,
					
					"width"		: 226,
					"height"	: 11,
					
					"children" :
					[
						{
							"name" : "eve_event_time_label_text",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							
							"horizontal_align"		: "right",
							"text_horizontal_align"	: "right",
							
							## 전야제 남은 기간
							"text" : uiScriptLocale.OTHER_WORLD_EVE_EVENT_TIME_TEXT,
						},
					],
				},
				
				{
					"name" : "eve_event_time_text_bg",
					"type" : "image",
					
					"x" : 243,
					"y" : 223,
					
					"width"		: 79,
					"height"	: 18,
					
					"image"		: ROOT_PATH + "event_time_text_bg.sub",
					
					"children" :
					[
						{
							"name" : "eve_event_time_text",
							"type" : "text",
							
							"x" : 1,
							"y" : 3,
							
							"horizontal_align"		: "center",
							"text_horizontal_align" : "center",
							
							"text" : "-",
						},
					],
				},
				
				## 저승 이벤트 남은 기간
				{
					"name" : "event_time_label_text_window",
					"type" : "window",
					
					"x" : 12,
					"y" : 248,
					
					"width"		: 226,
					"height"	: 11,
					
					"children" :
					[
						{
							"name" : "event_time_label_text",
							"type" : "text",
							
							"x" : 0,
							"y" : 0,
							
							"horizontal_align"		: "right",
							"text_horizontal_align"	: "right",
							
							## 저승 이벤트 남은 기간
							"text" : uiScriptLocale.OTHER_WORLD_EVENT_TIME_TEXT,
						},
					],
				},
				
				{
					"name" : "event_time_text_bg",
					"type" : "image",
					
					"x" : 243,
					"y" : 245,
					
					"width"		: 79,
					"height"	: 18,
					
					"image"		: ROOT_PATH + "event_time_text_bg.sub",
					
					"children" :
					[
						{
							"name" : "event_time_text",
							"type" : "text",
							
							"x" : 1,
							"y" : 3,
							
							"horizontal_align"		: "center",
							"text_horizontal_align" : "center",
							
							"text" : "-",
						},
					],
				},
				
				## 이전 페이지 버튼
				{
					"name"			: "prev_button",
					"type"			: "button",
					
					"x"				: 260,
					"y"				: 201,
					
					"default_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_intro_btn/prev_btn_01.sub",
				},
				
				## 다음 페이지 버튼
				{
					"name"			: "next_button",
					"type"			: "button",
					
					"x"				: 291,
					"y"				: 201,
					
					"default_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
					"over_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_02.sub",
					"down_image"	: PUBLIC_PATH + "public_intro_btn/next_btn_01.sub",
				},
				
				## 도깨비불 이벤트 버튼
				{
					"name"			: "wisp_button",
					"type"			: "button",
					
					"x"				: 127,
					"y"				: 272,
					
					"default_image"	: ROOT_PATH + "wisp_button_default.sub",
					"over_image"	: ROOT_PATH + "wisp_button_over.sub",
					"down_image"	: ROOT_PATH + "wisp_button_down.sub",
				},
			],
		},
	],
}