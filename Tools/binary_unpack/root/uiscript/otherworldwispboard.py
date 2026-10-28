import uiScriptLocale

PUBLIC_PATH					= "d:/ymir work/ui/public/"
PATTERN_PATH				= "d:/ymir work/ui/pattern/"
ROOT_PATH					= "d:/ymir work/ui/minigame/otherworld/"

WINDOW_WIDTH				= 336
WINDOW_HEIGHT				= 306

DESC_WINDOW_WIDTH			= 314
DESC_WINDOW_HEIGHT			= 262
DESC_WINDOW_PATTERN_X_COUNT	= (DESC_WINDOW_WIDTH - 32) / 16
DESC_WINDOW_PATTERN_Y_COUNT	= (DESC_WINDOW_HEIGHT - 32) / 16

window = {
	"name"	: "OtherWorldWispBoard",
	"style"	: ("movable", "float", ),
	
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
			## 도깨비불 보유현황
			"title"		: uiScriptLocale.OTHER_WORLD_WISP_BOARD_TITLE,
			
			"children" :
			[
				{
					"name"		: "desc_window",
					"type"		: "window",
					"style"		: ("ltr", "attach", ),
					
					"x"			: 10,
					"y"			: 32,
					
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

				## slotImg
				{
					"name" : "slot_bg",
					"type" : "image",
					
					"x" : 143,
					"y" : 44,
					
					"width"		: 50,
					"height"	: 50,
					
					"image"		: ROOT_PATH + "slot_img.sub",
				},

				## slotBar
				{
					"name" : "slot_bar",
					"type" : "image",
					
					"x" : 192,
					"y" : 59,
					
					"width"		: 33,
					"height"	: 18,
					
					"image"		: ROOT_PATH + "count_bg.sub",
					
					"children" :
					[
						{
							"name" : "item_count_text_0",
							"type" : "text",

							"x" : 1,
							"y" : 3,

							"horizontal_align"		: "center",
							"text_horizontal_align" : "center",

							"text" : "-",
						},
					],
				},

				## bottomArrow
				{
					"name" : "bottom_arrow",
					"type" : "image",
					
					"x" : 152,
					"y" : 102,
					
					"width"		: 33,
					"height"	: 15,
					
					"image"		: ROOT_PATH + "bottom_arrow.sub",
				},

				## menuImg
				{
					"name" : "menu_bg",
					"type" : "image",
					
					"x" : 13,
					"y" : 129,
					
					"width"		: 308,
					"height"	: 37,
					
					"image"		: ROOT_PATH + "menu_bg.sub",

					"children" :
					[
						{
							"name" : "item_name_text",
							"type" : "text",

							"x" : 56 - 13,
							"y" : 143 - 129,

							## 아이템명
							"text" : uiScriptLocale.OTHER_WORLD_ITEM_NAME_TEXT,
						},

						{
							"name" : "count_text",
							"type" : "text",

							"x" : 147 - 13,
							"y" : 143 - 129,

							## 개수
							"text" : uiScriptLocale.OTHER_WORLD_COUNT_TEXT,
						},

						{
							"name" : "slash_text",
							"type" : "text",

							"x" : 177 - 13,
							"y" : 143 - 129,

							## /
							"text" : uiScriptLocale.OTHER_WORLD_SLASH_TEXT,
						},

						{
							"name" : "need_text",
							"type" : "text",

							"x" : 190 - 13,
							"y" : 134 - 129,

							## 필요
							"text" : uiScriptLocale.OTHER_WORLD_NEED_TEXT,
						},

						{
							"name" : "count_text",
							"type" : "text",

							"x" : 190 - 13,
							"y" : 149 - 129,

							## 개수
							"text" : uiScriptLocale.OTHER_WORLD_COUNT_TEXT,
						},

						{
							"name" : "exchange_menu_text",
							"type" : "text",

							"x" : 258 - 13,
							"y" : 143 - 129,

							## 교환
							"text" : uiScriptLocale.OTHER_WORLD_EXCHANGE_MENU_TEXT,
						},
					],
				},

				## list1
				{
					"name" : "list_bg_1",
					"type" : "image",
					
					"x" : 15,
					"y" : 168,
					
					"width"		: 304,
					"height"	: 28,
					
					"image"		: ROOT_PATH + "list_bg.sub",

					"children" :
					[
						## itemName1
						{
							"name" : "item_name_window_1",
							"type" : "window",

							"x" : 7,
							"y" : 9,
							
							"width"		: 112,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_name_text_1",
									"type" : "text",

									"x" : 2,
									"y" : -1,
									
									"horizontal_align"		: "right",
									"text_horizontal_align" : "right",

									## 살생부
									"text" : uiScriptLocale.OTHER_WORLD_LIFE_AND_DEATH_LIST_TEXT,
								},
							],
						},

						## itemCount1
						{
							"name" : "item_count_window_1",
							"type" : "window",

							"x" : 127,
							"y" : 9,
							
							"width"		: 31,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_count_text_1",
									"type" : "text",

									"x" : 1,
									"y" : -1,
									
									"horizontal_align"		: "center",
									"text_horizontal_align" : "center",

									"text" : "-",
								},
							],
						},

						## itemNeedCount1
						{
							"name" : "item_need_count_window_1",
							"type" : "window",

							"x" : 171,
							"y" : 9,
							
							"width"		: 31,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_need_count_text_1",
									"type" : "text",

									"x" : 1,
									"y" : -1,
									
									"horizontal_align"		: "center",
									"text_horizontal_align" : "center",

									"text" : "-",
								},
							],
						},

						## 교환하기 버튼
						{
							"name"			: "exchange_button_1",
							"type"			: "button",

							"x"				: 211,
							"y"				: 4,

							"default_image"	: PUBLIC_PATH + "large_button_01.sub",
							"over_image"	: PUBLIC_PATH + "large_button_02.sub",
							"down_image"	: PUBLIC_PATH + "large_button_03.sub",

							## 교환하기
							"text"			: uiScriptLocale.OTHER_WORLD_EXCHANGE_TEXT,
						},
					],
				},

				## list2
				{
					"name" : "list_bg_2",
					"type" : "image",
					
					"x" : 15,
					"y" : 199,
					
					"width"		: 304,
					"height"	: 28,
					
					"image"		: ROOT_PATH + "list_bg.sub",

					"children" :
					[
						## itemName2
						{
							"name" : "item_name_window_2",
							"type" : "window",

							"x" : 7,
							"y" : 9,
							
							"width"		: 112,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_name_text_2",
									"type" : "text",

									"x" : 2,
									"y" : -1,
									
									"horizontal_align"		: "right",
									"text_horizontal_align" : "right",

									## 환생수
									"text" : uiScriptLocale.OTHER_WORLD_REINCARNATION_WATER_TEXT,
								},
							],
						},

						## itemCount2
						{
							"name" : "item_count_window_2",
							"type" : "window",

							"x" : 127,
							"y" : 9,
							
							"width"		: 31,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_count_text_2",
									"type" : "text",

									"x" : 1,
									"y" : -1,
									
									"horizontal_align"		: "center",
									"text_horizontal_align" : "center",

									"text" : "-",
								},
							],
						},

						## itemNeedCount2
						{
							"name" : "item_need_count_window_2",
							"type" : "window",

							"x" : 171,
							"y" : 9,
							
							"width"		: 31,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_need_count_text_2",
									"type" : "text",

									"x" : 1,
									"y" : -1,
									
									"horizontal_align"		: "center",
									"text_horizontal_align" : "center",

									"text" : "-",
								},
							],
						},

						## 교환하기 버튼
						{
							"name"			: "exchange_button_2",
							"type"			: "button",

							"x"				: 211,
							"y"				: 4,

							"default_image"	: PUBLIC_PATH + "large_button_01.sub",
							"over_image"	: PUBLIC_PATH + "large_button_02.sub",
							"down_image"	: PUBLIC_PATH + "large_button_03.sub",

							## 교환하기
							"text"			: uiScriptLocale.OTHER_WORLD_EXCHANGE_TEXT,
						},
					],
				},

				## list3
				{
					"name" : "list_bg_3",
					"type" : "image",
					
					"x" : 15,
					"y" : 230,
					
					"width"		: 304,
					"height"	: 28,
					
					"image"		: ROOT_PATH + "list_bg.sub",

					"children" :
					[
						## itemName3
						{
							"name" : "item_name_window_3",
							"type" : "window",

							"x" : 7,
							"y" : 9,
							
							"width"		: 112,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_name_text_3",
									"type" : "text",

									"x" : 2,
									"y" : -1,
									
									"horizontal_align"		: "right",
									"text_horizontal_align" : "right",

									## 사령
									"text" : uiScriptLocale.OTHER_WORLD_DEAD_SOUL_TEXT,
								},
							],
						},

						## itemCount3
						{
							"name" : "item_count_window_3",
							"type" : "window",

							"x" : 127,
							"y" : 9,
							
							"width"		: 31,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_count_text_3",
									"type" : "text",

									"x" : 1,
									"y" : -1,
									
									"horizontal_align"		: "center",
									"text_horizontal_align" : "center",

									"text" : "-",
								},
							],
						},

						## itemNeedCount3
						{
							"name" : "item_need_count_window_3",
							"type" : "window",

							"x" : 171,
							"y" : 9,
							
							"width"		: 31,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_need_count_text_3",
									"type" : "text",

									"x" : 1,
									"y" : -1,
									
									"horizontal_align"		: "center",
									"text_horizontal_align" : "center",

									"text" : "-",
								},
							],
						},

						## 교환하기 버튼
						{
							"name"			: "exchange_button_3",
							"type"			: "button",

							"x"				: 211,
							"y"				: 4,

							"default_image"	: PUBLIC_PATH + "large_button_01.sub",
							"over_image"	: PUBLIC_PATH + "large_button_02.sub",
							"down_image"	: PUBLIC_PATH + "large_button_03.sub",

							## 교환하기
							"text"			: uiScriptLocale.OTHER_WORLD_EXCHANGE_TEXT,
						},
					],
				},

				## list4
				{
					"name" : "list_bg_4",
					"type" : "image",
					
					"x" : 15,
					"y" : 261,
					
					"width"		: 304,
					"height"	: 28,
					
					"image"		: ROOT_PATH + "list_bg.sub",

					"children" :
					[
						## itemName4
						{
							"name" : "item_name_window_4",
							"type" : "window",

							"x" : 7,
							"y" : 9,
							
							"width"		: 112,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_name_text_4",
									"type" : "text",

									"x" : 2,
									"y" : -1,
									
									"horizontal_align"		: "right",
									"text_horizontal_align" : "right",

									## 지옥문 열쇠
									"text" : uiScriptLocale.OTHER_WORLD_HELL_GATE_KEY_TEXT,
								},
							],
						},

						## itemCount4
						{
							"name" : "item_count_window_4",
							"type" : "window",

							"x" : 127,
							"y" : 9,
							
							"width"		: 31,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_count_text_4",
									"type" : "text",

									"x" : 1,
									"y" : -1,
									
									"horizontal_align"		: "center",
									"text_horizontal_align" : "center",

									"text" : "-",
								},
							],
						},

						## itemNeedCount4
						{
							"name" : "item_need_count_window_4",
							"type" : "window",

							"x" : 171,
							"y" : 9,
							
							"width"		: 31,
							"height"	: 11,

							"children" :
							[
								{ 
									"name" : "item_need_count_text_4",
									"type" : "text",

									"x" : 1,
									"y" : -1,
									
									"horizontal_align"		: "center",
									"text_horizontal_align" : "center",

									"text" : "-",
								},
							],
						},

						## 교환하기 버튼
						{
							"name"			: "exchange_button_4",
							"type"			: "button",

							"x"				: 211,
							"y"				: 4,

							"default_image"	: PUBLIC_PATH + "large_button_01.sub",
							"over_image"	: PUBLIC_PATH + "large_button_02.sub",
							"down_image"	: PUBLIC_PATH + "large_button_03.sub",

							## 교환하기
							"text"			: uiScriptLocale.OTHER_WORLD_EXCHANGE_TEXT,
						},
					],
				},

				## dropDownBar
				{
					"name" : "drop_down_bar",
					"type" : "image",
					
					"x" : 198,
					"y" : 100,
					
					"width"		: 90,
					"height"	: 18,
					
					"image"		: ROOT_PATH + "drop_down_base_bg.sub",
					
					"children" :
					[
						{
							"name" : "drop_down_text",
							"type" : "text",

							"x" : -6,
							"y" : 3,

							"horizontal_align"		: "center",
							"text_horizontal_align" : "center",

							## X1
							"text" : uiScriptLocale.OTHER_WORLD_DROP_DOWN_X1,
						},
					],
				},

				## dropDownList
				{
					"name" : "drop_down_list",
					"type" : "image",
					
					"x" : 198,
					"y" : 117,
					
					"image"	: ROOT_PATH + "drop_down_list_bg.sub",

					"children" :
					[
						## X1Button
						{
							"name" : "x1_button",
							"type" : "button",
							
							"x" : 0,
							"y" : 0,
							
							"default_image"	: ROOT_PATH + "x1_button_default.sub",
							"over_image"	: ROOT_PATH + "drop_down_list_over.sub",
							"down_image"	: ROOT_PATH + "x1_button_default.sub",

							## X1
							"text" : uiScriptLocale.OTHER_WORLD_DROP_DOWN_X1,
						},

						## X5Button
						{
							"name" : "x5_button",
							"type" : "button",
							
							"x" : 0,
							"y" : 16,
							
							"default_image"	: ROOT_PATH + "x5_button_default.sub",
							"over_image"	: ROOT_PATH + "drop_down_list_over.sub",
							"down_image"	: ROOT_PATH + "x5_button_default.sub",

							## X5
							"text" : uiScriptLocale.OTHER_WORLD_DROP_DOWN_X5,
						},

						## X10Button
						{
							"name" : "x10_button",
							"type" : "button",
							
							"x" : 0,
							"y" : 32,
							
							"default_image"	: ROOT_PATH + "x10_button_default.sub",
							"over_image"	: ROOT_PATH + "drop_down_list_over.sub",
							"down_image"	: ROOT_PATH + "x10_button_default.sub",

							## X10
							"text" : uiScriptLocale.OTHER_WORLD_DROP_DOWN_X10,
						},
					],
				},
				
				## dropDownButton
				{
					"name" : "drop_down_arrow_button",
					"type" : "button",
					
					"x" : 271,
					"y" : 101,
					
					"default_image"	: ROOT_PATH + "drop_down_button_default.sub",
					"over_image"	: ROOT_PATH + "drop_down_button_down.sub",
					"down_image"	: ROOT_PATH + "drop_down_button_over.sub",
				},
			],
		},
	],
}