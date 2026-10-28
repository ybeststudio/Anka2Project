# ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH : app 추가
import app
import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/public/battle/"
BACK_IMG_PATH = "d:/ymir work/ui/pattern/"

if app.ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH:	# ae ui랑 공용으로 쓰기 위해 수정, 수정된 내용은 주석 추가됨 (ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH 로 검색)
	window = {
		"name" : "BattleFieldWindow",
	
		
		"x" : (SCREEN_WIDTH -518) / 2,
		"y" : (SCREEN_HEIGHT - 400) / 2,
	
		"style" : ("movable","float",),
	
		"width" : 518,
		"height" : 400,
	
		"children" :
		(
			{
				"name" : "board",
				"type" : "board",
	
				"x" : 0,
				"y" : 0,
	
				"width" : 518,
				"height" : 400,
	
				"children" :
				(
					## Title
					{
						"name" : "titlebar",
						"type" : "titlebar",
						"style" : ("attach",),
	
						"x" : 8,
						"y" : 7,
	
						"width" : 503,
						"color" : "yellow",
	
						"children" :
						(
							{ "name":"title_name", "type":"text", "x":0, "y":-1, "text": uiScriptLocale.BATTLE_FIELD_INFO, "all_align":"center" },
						),
					},
	
					# ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH : 기존 desc 배경을 outline_window로 변경
					## Explanation bg
					{
						"name"		: "explanation",
						"type"		: "window",
						"style"		: ("ltr", "attach", ),
						
						"x"			: 14,
						"y"			: 36,
	
						"width"		: 169,
						"height"	: 326,
	
						"children" :
						(
							{
								"name"		: "explanation_background",
								"type"		: "outline_window",
								"x"			: 0,
								"y"			: 0,
								"width"		: 169,
								"height"	: 326,
							},
						),
					},
	
					# ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH : prev_button, next_button, explanation 안쪽에 있던 것 밖으로 분리
					## Explanation control
					{
						"name" : "prev_button",
						"type" : "button",
						"x" : 123,
						"y" : 341,
	
						"default_image" : ROOT_PATH +"arrow_left.sub",
						"over_image" 	: ROOT_PATH +"arrow_left_over.sub",
						"down_image" 	: ROOT_PATH +"arrow_left.sub",
					},
					{
						"name" : "next_button",
						"type" : "button",
						"x" : 154,
						"y" : 341,
	
						"default_image" : ROOT_PATH +"arrow_right.sub",
						"over_image" 	: ROOT_PATH +"arrow_right_over.sub",
						"down_image" 	: ROOT_PATH +"arrow_right.sub",
					},
					
					{
						"name" : "text_board",
						"type" : "window",
						"x" : 14,
						"y" : 36,
						"width" : 169-16,
						"height" : 326-16,
					},
	
	
	
					## RankingList bg
					{
						"name" : "ranking_list",
						"type" : "window",
						# ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH : "style" : ("attach","ltr",), 추가
						"style" : ("attach","ltr",),
	
						"x" : 191,
						"y" : 66,
	
						"width" : 312,
						"height" : 296,
						
						"children" :
						(
							## LeftTop
							{
								"name" : "left_top",
								"type" : "image",
								"x" : 0,
								"y" : 0,
								"image" : BACK_IMG_PATH+"border_A_left_top.tga",
							},
							## RightTop
							{
								"name" : "right_top",
								"type" : "image",
								"x" : 312-16,
								"y" : 0,
								"image" : BACK_IMG_PATH+"border_A_right_top.tga",
							},
							## LeftBottom
							{
								"name" : "left_bottom",
								"type" : "image",
								"x" : 0,
								"y" : 296-16,
								"image" : BACK_IMG_PATH+"border_A_left_bottom.tga",
							},
							## RightBottom
							{
								"name" : "right_bottom",
								"type" : "image",
								"x" : 312-16,
								"y" : 296-16,
								"image" : BACK_IMG_PATH+"border_A_right_bottom.tga",
							},
							## leftcenterImg
							{
								"name" : "left_center_img",
								"type" : "expanded_image",
								"x" : 0,
								"y" : 15,
								"image" : BACK_IMG_PATH+"border_A_left.tga",
								"rect" : (0.0, 0.0, 0, 16),
							},
							## rightcenterImg
							{
								"name" : "right_center_img",
								"type" : "expanded_image",
								"x" : 312-16,
								"y" : 15,
								"image" : BACK_IMG_PATH+"border_A_right.tga",
								"rect" : (0.0, 0.0, 0, 16),
							},
							## topcenterImg
							{
								"name" : "top_center_img",
								"type" : "expanded_image",
								"x" : 15,
								"y" :  0,
								"image" : BACK_IMG_PATH+"border_A_top.tga",
								"rect" : (0.0, 0.0, 17, 0),
							},
							## bottomcenterImg
							{
								"name" : "bottom_center_img",
								"type" : "expanded_image",
								"x" : 15,
								"y" : 296-16,
								"image" : BACK_IMG_PATH+"border_A_bottom.tga",
								"rect" : (0.0, 0.0, 17, 0),
							},
							## centerImg
							{
								"name" : "center_img",
								"type" : "expanded_image",
								"x" : 15,
								"y" : 15,
								"image" : BACK_IMG_PATH+"border_A_center.tga",
								"rect" : (0.0, 0.0, 17, 16),
							},
						),	
					},
	
					## Tab Area
					{
						"name" : "tab_control",
						"type" : "window",
	
						"x" : 191,
						"y" : 36,
	
						"width" : 312,
						"height" : 33,
	
						"children" :
						(
							## Tab
							{
								"name" : "tab_01",
								"type" : "image",
	
								"x" : 0,
								"y" : 0,
	
								"width" : 312,
								"height" : 33,
	
								"image" : ROOT_PATH+"tab_current_rank.sub",
							},
							{
								"name" : "tab_02",
								"type" : "image",
	
								"x" : 0,
								"y" : 0,
	
								"width" : 312,
								"height" : 33,
								
								"image" : ROOT_PATH+"tab_accum_rank.sub",
							},
							## RadioButton ##
							{
								"name" : "tab_button_01",
								"type" : "radio_button",
	
								"x" : 0,
								"y" : 0,
	
								"width" : 120,
								"height" : 33,
							},
							{
								"name" : "tab_button_02",
								"type" : "radio_button",
	
								"x" : 124,
								"y" : 0,
	
								"width" : 120,
								"height" : 33,
							},
						),
					},
	
					## List Column Titlebar
					{
						"name" : "list",
						"type" : "window",
	
						"x" : 194,
						"y" : 69,
	
						"width" : 306,
						"height" : 21,
	
						"children" :
						(
							{
								"name" : "sub_titlebar",
								"type" : "image",
	
								"x" : 0,
								"y" : 0,
	
								"image" : ROOT_PATH+"column_titlebar.sub",
	
								"children" :
								(
									{ "name":"column_rank", "type":"text", "x":227-194, "y":74-69, "text":uiScriptLocale.BATTLE_FILED_RANK, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									{ "name":"column_name", "type":"text", "x":320-194, "y":74-69, "text":uiScriptLocale.BATTLE_FILED_RANK_NAME, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									{ "name":"column_empire", "type":"text", "x":400-194, "y":74-69, "text":uiScriptLocale.BATTLE_FILED_RANK_EMPIRE, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									{ "name":"column_point", "type":"text", "x":455-194, "y":74-69, "text":uiScriptLocale.BATTLE_FILED_RANK_POINT, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
								),
							},
						),
					},
	
					## Battle info
					{
						"name" : "battle_info",
						"type" : "window",
	
						"x" : 0,
						"y" : 360,
	
						"width" : 518,
						"height" : 40,
	
						"children" :
						(
							{
								"name" : "point_icon",
								"type" : "image",
	
								"x" : 17,
								"y" : 10,
	
								"image" : ROOT_PATH+"icon_my_point.sub",
							},
							{ "name":"my_point", "type":"text", "x":41, "y":18, "text": "999", "text_vertical_align":"center" },
							{
								"name" : "notice_icon",
								"type" : "image",
	
								"x" : 133,
								"y" : 10,
	
								"image" : ROOT_PATH+"icon_notice.sub",
							},
							{ "name":"notice", "type":"text", "x":160, "y":18, "text": uiScriptLocale.BATTLE_FIELD_OPERATION_TIME_ALL, "text_vertical_align":"center" },
							{
								"name" : "enter_button",
								"type" : "button",
	
								"x" : 416,
								"y" : 8,
	
								"text" : uiScriptLocale.ENTER_BATTLE_FIELD,
								"horizontal_align" : "left",
								"vertical_align" : "top",
	
								"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
								"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
								"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
							},
						),
					},
				),
			},
		),
	}
else:
	window = {
		"name" : "BattleFieldWindow",
	
		
		"x" : (SCREEN_WIDTH -518) / 2,
		"y" : (SCREEN_HEIGHT - 400) / 2,
	
		"style" : ("movable","float",),
	
		"width" : 518,
		"height" : 400,
	
		"children" :
		(
			{
				"name" : "board",
				"type" : "board",
	
				"x" : 0,
				"y" : 0,
	
				"width" : 518,
				"height" : 400,
	
				"children" :
				(
					## Title
					{
						"name" : "titlebar",
						"type" : "titlebar",
						"style" : ("attach",),
	
						"x" : 8,
						"y" : 7,
	
						"width" : 503,
						"color" : "yellow",
	
						"children" :
						(
							{ "name":"title_name", "type":"text", "x":0, "y":-1, "text": uiScriptLocale.BATTLE_FIELD_INFO, "all_align":"center" },
						),
					},
					## Explanation bg
					{
						"name" : "explanation",
						"type" : "window",
	
						"x" : 14,
						"y" : 36,
	
						"width" : 169,
						"height" : 326,
	
						"children" :
						(
							## LeftTop
							{
								"name" : "left_top",
								"type" : "image",
								"x" : 0,
								"y" : 0,
								"image" : BACK_IMG_PATH+"border_A_left_top.tga",
							},
							## RightTop
							{
								"name" : "right_top",
								"type" : "image",
								"x" : 169-16,
								"y" : 0,
								"image" : BACK_IMG_PATH+"border_A_right_top.tga",
							},
							## LeftBottom
							{
								"name" : "left_bottom",
								"type" : "image",
								"x" : 0,
								"y" : 326-16,
								"image" : BACK_IMG_PATH+"border_A_left_bottom.tga",
							},
							## RightBottom
							{
								"name" : "right_bottom",
								"type" : "image",
								"x" : 169-16,
								"y" : 326-16,
								"image" : BACK_IMG_PATH+"border_A_right_bottom.tga",
							},
							## leftcenterImg
							{
								"name" : "left_center_img",
								"type" : "expanded_image",
								"x" : 0,
								"y" : 15,
								"image" : BACK_IMG_PATH+"border_A_left.tga",
								"rect" : (0.0, 0.0, 0, 18),
							},
							## rightcenterImg
							{
								"name" : "right_center_img",
								"type" : "expanded_image",
								"x" : 169-16,
								"y" : 15,
								"image" : BACK_IMG_PATH+"border_A_right.tga",
								"rect" : (0.0, 0.0, 0, 18),
							},
							## topcenterImg
							{
								"name" : "top_center_img",
								"type" : "expanded_image",
								"x" : 15,
								"y" :  0,
								"image" : BACK_IMG_PATH+"border_A_top.tga",
								"rect" : (0.0, 0.0, 8, 0),
							},
							## bottomcenterImg
							{
								"name" : "bottom_center_img",
								"type" : "expanded_image",
								"x" : 15,
								"y" : 326-16,
								"image" : BACK_IMG_PATH+"border_A_bottom.tga",
								"rect" : (0.0, 0.0, 8, 0),
							},
							## centerImg
							{
								"name" : "center_img",
								"type" : "expanded_image",
								"x" : 15,
								"y" : 15,
								"image" : BACK_IMG_PATH+"border_A_center.tga",
								"rect" : (0.0, 0.0, 8, 18),
							},
							
							## Explanation control
							{
								"name" : "prev_button",
								"type" : "button",
								"x" : 123-14,
								"y" : 341-36,
	
								"default_image" : ROOT_PATH +"arrow_left.sub",
								"over_image" 	: ROOT_PATH +"arrow_left_over.sub",
								"down_image" 	: ROOT_PATH +"arrow_left.sub",
							},
							{
								"name" : "next_button",
								"type" : "button",
								"x" : 154-14,
								"y" : 341-36,
	
								"default_image" : ROOT_PATH +"arrow_right.sub",
								"over_image" 	: ROOT_PATH +"arrow_right_over.sub",
								"down_image" 	: ROOT_PATH +"arrow_right.sub",
							},
						),
					},
					
					{
						"name" : "text_board",
						"type" : "window",
						"x" : 14,
						"y" : 36,
						"width" : 169-16,
						"height" : 326-16,
					},
	
	
	
					## RankingList bg
					{
						"name" : "ranking_list",
						"type" : "window",
	
						"x" : 191,
						"y" : 66,
	
						"width" : 312,
						"height" : 296,
						
						"children" :
						(
							## LeftTop
							{
								"name" : "left_top",
								"type" : "image",
								"x" : 0,
								"y" : 0,
								"image" : BACK_IMG_PATH+"border_A_left_top.tga",
							},
							## RightTop
							{
								"name" : "right_top",
								"type" : "image",
								"x" : 312-16,
								"y" : 0,
								"image" : BACK_IMG_PATH+"border_A_right_top.tga",
							},
							## LeftBottom
							{
								"name" : "left_bottom",
								"type" : "image",
								"x" : 0,
								"y" : 296-16,
								"image" : BACK_IMG_PATH+"border_A_left_bottom.tga",
							},
							## RightBottom
							{
								"name" : "right_bottom",
								"type" : "image",
								"x" : 312-16,
								"y" : 296-16,
								"image" : BACK_IMG_PATH+"border_A_right_bottom.tga",
							},
							## leftcenterImg
							{
								"name" : "left_center_img",
								"type" : "expanded_image",
								"x" : 0,
								"y" : 15,
								"image" : BACK_IMG_PATH+"border_A_left.tga",
								"rect" : (0.0, 0.0, 0, 16),
							},
							## rightcenterImg
							{
								"name" : "right_center_img",
								"type" : "expanded_image",
								"x" : 312-16,
								"y" : 15,
								"image" : BACK_IMG_PATH+"border_A_right.tga",
								"rect" : (0.0, 0.0, 0, 16),
							},
							## topcenterImg
							{
								"name" : "top_center_img",
								"type" : "expanded_image",
								"x" : 15,
								"y" :  0,
								"image" : BACK_IMG_PATH+"border_A_top.tga",
								"rect" : (0.0, 0.0, 17, 0),
							},
							## bottomcenterImg
							{
								"name" : "bottom_center_img",
								"type" : "expanded_image",
								"x" : 15,
								"y" : 296-16,
								"image" : BACK_IMG_PATH+"border_A_bottom.tga",
								"rect" : (0.0, 0.0, 17, 0),
							},
							## centerImg
							{
								"name" : "center_img",
								"type" : "expanded_image",
								"x" : 15,
								"y" : 15,
								"image" : BACK_IMG_PATH+"border_A_center.tga",
								"rect" : (0.0, 0.0, 17, 16),
							},
						),	
					},
	
					## Tab Area
					{
						"name" : "tab_control",
						"type" : "window",
	
						"x" : 191,
						"y" : 36,
	
						"width" : 312,
						"height" : 33,
	
						"children" :
						(
							## Tab
							{
								"name" : "tab_01",
								"type" : "image",
	
								"x" : 0,
								"y" : 0,
	
								"width" : 312,
								"height" : 33,
	
								"image" : ROOT_PATH+"tab_current_rank.sub",
							},
							{
								"name" : "tab_02",
								"type" : "image",
	
								"x" : 0,
								"y" : 0,
	
								"width" : 312,
								"height" : 33,
								
								"image" : ROOT_PATH+"tab_accum_rank.sub",
							},
							## RadioButton ##
							{
								"name" : "tab_button_01",
								"type" : "radio_button",
	
								"x" : 0,
								"y" : 0,
	
								"width" : 120,
								"height" : 33,
							},
							{
								"name" : "tab_button_02",
								"type" : "radio_button",
	
								"x" : 124,
								"y" : 0,
	
								"width" : 120,
								"height" : 33,
							},
						),
					},
	
					## List Column Titlebar
					{
						"name" : "list",
						"type" : "window",
	
						"x" : 194,
						"y" : 69,
	
						"width" : 306,
						"height" : 21,
	
						"children" :
						(
							{
								"name" : "sub_titlebar",
								"type" : "image",
	
								"x" : 0,
								"y" : 0,
	
								"image" : ROOT_PATH+"column_titlebar.sub",
	
								"children" :
								(
									{ "name":"column_rank", "type":"text", "x":227-194, "y":74-69, "text":uiScriptLocale.BATTLE_FILED_RANK, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									{ "name":"column_name", "type":"text", "x":320-194, "y":74-69, "text":uiScriptLocale.BATTLE_FILED_RANK_NAME, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									{ "name":"column_empire", "type":"text", "x":400-194, "y":74-69, "text":uiScriptLocale.BATTLE_FILED_RANK_EMPIRE, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
									{ "name":"column_point", "type":"text", "x":455-194, "y":74-69, "text":uiScriptLocale.BATTLE_FILED_RANK_POINT, "r":1.0, "g":1.0, "b":1.0, "a":1.0, "text_horizontal_align":"center" },
								),
							},
						),
					},
	
					## Battle info
					{
						"name" : "battle_info",
						"type" : "window",
	
						"x" : 0,
						"y" : 360,
	
						"width" : 518,
						"height" : 40,
	
						"children" :
						(
							{
								"name" : "point_icon",
								"type" : "image",
	
								"x" : 17,
								"y" : 10,
	
								"image" : ROOT_PATH+"icon_my_point.sub",
							},
							{ "name":"my_point", "type":"text", "x":41, "y":18, "text": "999", "text_vertical_align":"center" },
							{
								"name" : "notice_icon",
								"type" : "image",
	
								"x" : 133,
								"y" : 10,
	
								"image" : ROOT_PATH+"icon_notice.sub",
							},
							{ "name":"notice", "type":"text", "x":160, "y":18, "text": uiScriptLocale.BATTLE_FIELD_OPERATION_TIME_ALL, "text_vertical_align":"center" },
							{
								"name" : "enter_button",
								"type" : "button",
	
								"x" : 416,
								"y" : 8,
	
								"text" : uiScriptLocale.ENTER_BATTLE_FIELD,
								"horizontal_align" : "left",
								"vertical_align" : "top",
	
								"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
								"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
								"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
							},
						),
					},
				),
			},
		),
	}
