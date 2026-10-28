import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/mercenary/"
PUBLIC_PATH = "d:/ymir work/ui/public/"
BOARD_WIDTH		= 612
BOARD_HEIGHT	= 454

window = {
	"name" : "MercenaryMissionWindow",
		
	"x" : 10,
	"y" : 32,

	"width" : BOARD_WIDTH,
	"height" : BOARD_HEIGHT,

	"children" :
	(		
		## main bg
		{
			"name" : "main_bg_window",
			"type" : "window",
					
			"style" : ("attach", "ltr",),

			"x" : 0,
			"y" : 0,
					
			"width" : BOARD_WIDTH,
			"height" : BOARD_HEIGHT,
					
			"children" :
			(
				## main pattern bg
				{ "name" : "left_top", "type" : "image", "style" : ("ltr",),					"x" : 0,	"y" : 0,	"image" : "d:/ymir work/ui/pattern/border_A_left_top.tga", },
				{ "name" : "right_top", "type" : "image", "style" : ("ltr",),					"x" : 596,	"y" : 0,	"image" : "d:/ymir work/ui/pattern/border_A_right_top.tga", },
				{ "name" : "left_bottom", "type" : "image", "style" : ("ltr",),					"x" : 0,	"y" : 438,	"image" : "d:/ymir work/ui/pattern/border_A_left_bottom.tga", },
				{ "name" : "right_bottom", "type" : "image", "style" : ("ltr",),				"x" : 596,	"y" : 438,	"image" : "d:/ymir work/ui/pattern/border_A_right_bottom.tga", },
				{ "name" : "top_center_img", "type" : "expanded_image", "style" : ("ltr",),		"x" : 16,	"y" : 0,	"image" : "d:/ymir work/ui/pattern/border_A_top.tga", "rect" : (0.0, 0.0, 36, 0), },
				{ "name" : "left_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 0,	"y" : 16,	"image" : "d:/ymir work/ui/pattern/border_A_left.tga", "rect" : (0.0, 0.0, 0, 26), },
				{ "name" : "right_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 596,	"y" : 16,	"image" : "d:/ymir work/ui/pattern/border_A_right.tga", "rect" : (0.0, 0.0, 0, 26), },
				{ "name" : "bottom_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 16,	"y" : 438,	"image" : "d:/ymir work/ui/pattern/border_A_bottom.tga", "rect" : (0.0, 0.0, 36, 0), },
				{ "name" : "center_img", "type" : "expanded_image", "style" : ("ltr",),			"x" : 16,	"y" : 16,	"image" : "d:/ymir work/ui/pattern/border_A_center.tga", "rect" : (0.0, 0.0, 36, 26), },
				{ "name" : "main_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 258,	"y" : 2,	"image" : "d:/ymir work/ui/pattern/border_A_right.tga", "rect" : (0.0, 0.0, 0, 27), },
			),
		},
		## mission regist window
		{
			"name" : "mission_regist_window", "type" : "window", "style" : ("attach", "ltr",), "x" : 3, "y" : 3, "width" : 268, "height" : 267,
			"children" :
			(
				## mission regist
				{
					"name" : "mission_regist_img", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT_PATH + "mercenary_menu_tab_1.sub",
					"children" :
					(				
						{ "name" : "mission_regist_txt",  "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.MERCENARY_MISSION_REGIST, "all_align" : "center" },
					),
				},
				## mission list 1
				{
					"name" : "mission_regist_img_0", "type" : "image", "x" : 3, "y" : 27, "image" : ROOT_PATH + "mercenary_mssion_list.sub",
					"children" :
					(
						{ "name" : "mission_location_0",	"type" : "text", "x" : 17, "y" : 4, "text" : "", },
						{ "name" : "mission_name_0",		"type" : "text", "x" : 17, "y" : 22, "text" : "", },
						{ "name" : "mission_time_0",		"type" : "text", "x" : 200, "y" : 13, "text" : "00:00", },
						{ "name" : "mission_state_0",		"type" : "image", "x" : 163, "y" : 6, "image" : ROOT_PATH + "mercenary_mission_icon_none.sub", },
					),
				},
				## mission list 2
				{
					"name" : "mission_regist_img_1", "type" : "image", "x" : 3, "y" : 70, "image" : ROOT_PATH + "mercenary_mssion_list.sub",
					"children" :
					(
						{ "name" : "mission_location_1",	"type" : "text", "x" : 17, "y" : 4, "text" : "", },
						{ "name" : "mission_name_1",		"type" : "text", "x" : 17, "y" : 22, "text" : "", },
						{ "name" : "mission_time_1",		"type" : "text", "x" : 200, "y" : 13, "text" : "00:00", },
						{ "name" : "mission_state_1",		"type" : "image", "x" : 163, "y" : 6, "image" : ROOT_PATH + "mercenary_mission_icon_none.sub", },
					),
				},
				## mission list 3
				{
					"name" : "mission_regist_img_2", "type" : "image", "x" : 3, "y" : 113, "image" : ROOT_PATH + "mercenary_mssion_list.sub",
					"children" :
					(
						{ "name" : "mission_location_2",	"type" : "text", "x" : 17, "y" : 4, "text" : "", },
						{ "name" : "mission_name_2",		"type" : "text", "x" : 17, "y" : 22, "text" : "", },
						{ "name" : "mission_time_2",		"type" : "text", "x" : 200, "y" : 13, "text" : "00:00", },
						{ "name" : "mission_state_2",		"type" : "image", "x" : 163, "y" : 6, "image" : ROOT_PATH + "mercenary_mission_icon_none.sub", },
					),
				},
				## mission list 4
				{
					"name" : "mission_regist_img_3", "type" : "image", "x" : 3, "y" : 156, "image" : ROOT_PATH + "mercenary_mssion_list.sub",
					"children" :
					(
						{ "name" : "mission_location_3",	"type" : "text", "x" : 17, "y" : 4, "text" : "", },
						{ "name" : "mission_name_3",		"type" : "text", "x" : 17, "y" : 22, "text" : "", },
						{ "name" : "mission_time_3",		"type" : "text", "x" : 200, "y" : 13, "text" : "00:00", },
						{ "name" : "mission_state_3",			"type" : "image", "x" : 163, "y" : 6, "image" : ROOT_PATH + "mercenary_mission_icon_none.sub", },
					),
				},
				## mission list scrollbar
				{ "name" : "mission_list_scrollbar", "type" : "scrollbar", "x" : 16, "y" : 24, "size" : 171, "horizontal_align" : "right", },
				## mission active
				{
					"name" : "mission_active_img", "type" : "image", "x" : 6, "y" : 209, "image" : ROOT_PATH + "mercenary_small_menu_tab_1.sub",
					"children" :
					(
						{ "name" : "mission_active_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MISSION_ACTIVE, "all_align" : "center" },
					),
				},
				## mission active value/maxvalue
				{
					"name" : "mission_regist_img", "type" : "image", "x" : 122, "y" : 207, "image" : ROOT_PATH + "mercenary_text_bg_1.sub",
					"children" :
					(
						## mission location
						{ "name" : "mission_active_txt",  "type" : "text", "x" : 1, "y" : 1, "text" : "0/0", "all_align" : "center" },
					),
				},
				## mission refresh button
				{
					"name" : "mission_refresh_button", "type" : "button", "x" : 220, "y" : 203,					
					"default_image"	: ROOT_PATH + "mercenary_mission_refresh_default.sub",
					"over_image"	: ROOT_PATH + "mercenary_mission_refresh_over.sub",
					"down_image"	: ROOT_PATH + "mercenary_mission_refresh_down.sub",
				},
			),
		},

		## mercenary list
		{
			"name" : "mercenary_list_window", "type" : "window", "style" : ("attach", "ltr",), "x" : 3, "y" : 235, "width" : 268, "height" : 267,
			"children" :
			(
				## mercenary list
				{
					"name" : "mercenary_list_img", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT_PATH + "mercenary_menu_tab_1.sub",
					"children" :
					(				
						{ "name" : "mercenary_list_txt",  "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.MERCENARY_MISSION_MERCENARY_LIST, "all_align" : "center" },
					),
				},
				## mercenary_list_mission_slot
				{
					"name" : "mercenary_list_mission_slot", "type" : "slot", "x" : 0, "y" : 0, "width" : 268, "height" : 200,
					"slot" : 
					(
						{"index":0,		"x":27,		"y":33,	"width":31, "height":31},
						{"index":1,		"x":82,		"y":33, "width":31, "height":31},
						{"index":2,		"x":137,	"y":33, "width":31, "height":31},
						{"index":3,		"x":192,	"y":33, "width":31, "height":31},
						
						{"index":4,		"x":27,		"y":71, "width":31, "height":31},
						{"index":5,		"x":82,		"y":71, "width":31, "height":31},
						{"index":6,		"x":137,	"y":71, "width":31, "height":31},
						{"index":7,		"x":192,	"y":71, "width":31, "height":31},
						## specificity_slots
						{"index":8,		"x":59,		"y":33, "width":16, "height":16},
						{"index":9,		"x":114,	"y":33, "width":16, "height":16},
						{"index":10,	"x":169,	"y":33, "width":16, "height":16},
						{"index":11,	"x":224,	"y":33, "width":16, "height":16},
						
						{"index":12,	"x":59,		"y":71, "width":16, "height":16},
						{"index":13,	"x":114,	"y":71, "width":16, "height":16},
						{"index":14,	"x":169,	"y":71, "width":16, "height":16},
						{"index":15,	"x":224,	"y":71, "width":16, "height":16},
						## state_slots
						{"index":16,	"x":59,		"y":49, "width":16, "height":16},
						{"index":17,	"x":114,	"y":49, "width":16, "height":16},
						{"index":18,	"x":169,	"y":49, "width":16, "height":16},
						{"index":19,	"x":224,	"y":49, "width":16, "height":16},
						
						{"index":20,	"x":59,		"y":87, "width":16, "height":16},
						{"index":21,	"x":114,	"y":87, "width":16, "height":16},
						{"index":22,	"x":169,	"y":87, "width":16, "height":16},
						{"index":23,	"x":224,	"y":87, "width":16, "height":16},
					),
				},
				## mercenary_list_left_arrow_btn
				{
					"name" : "mercenary_list_left_arrow", "type" : "button", "x" : 3, "y" : 62,					
					"default_image"	: ROOT_PATH + "mercenary_arrow_left_default.sub",
					"over_image"	: ROOT_PATH + "mercenary_arrow_left_over.sub",
					"down_image"	: ROOT_PATH + "mercenary_arrow_left_down.sub",
				},
				## mercenary_list_right_arrow_btn
				{
					"name" : "mercenary_list_right_arrow", "type" : "button", "x" : 247, "y" : 62,					
					"default_image"	: ROOT_PATH + "mercenary_arrow_right_default.sub",
					"over_image"	: ROOT_PATH + "mercenary_arrow_right_over.sub",
					"down_image"	: ROOT_PATH + "mercenary_arrow_right_down.sub",
				},
				## mercenary_possible_count_img
				{ 
					"name" : "mercenary_possible_count_img", "type" : "image", "x" : 3, "y" : 118, "image" : ROOT_PATH + "mercenary_limit_icon.sub", 
				},
				## mercenary_possible_count_edit
				{ 
					"name" : "mercenary_possible_count_edit_img", "type" : "image", "x" : 26, "y" : 118, "image" : ROOT_PATH + "mercenary_text_bg_2.sub", 
					"children" :
					(
						## mission location
						{ "name" : "mercenary_possible_count_txt",  "type" : "text", "x" : 1, "y" : 1, "text" : "0/6", "all_align" : "center" },
					),
				},				
				## mission_auto_join_btn
				{
					"name" : "mission_auto_join_btn", "type" : "button", "x" : 85, "y" : 118,
					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image" : PUBLIC_PATH + "large_button_02.sub",
					"down_image" : PUBLIC_PATH + "large_button_03.sub",
					"text" : uiScriptLocale.MERCENARY_MISSION_AUTO_JOIN,
				},
				## mission_join_btn
				{
					"name" : "mission_join_btn", "type" : "button", "x" : 175, "y" : 118,
					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image" : PUBLIC_PATH + "large_button_02.sub",
					"down_image" : PUBLIC_PATH + "large_button_03.sub",
					"text" : uiScriptLocale.MERCENARY_MISSION_JOIN,
				},
				## sub pattern bg
				{ "name" : "left_top", "type" : "image", "style" : ("ltr",),					"x" : 0,	"y" : 22+120, "image" : "d:/ymir work/ui/pattern/border_d_left_top.tga", },
				{ "name" : "right_top", "type" : "image", "style" : ("ltr",),					"x" : 262,	"y" : 22+120, "image" : "d:/ymir work/ui/pattern/border_d_right_top.tga", },
				{ "name" : "top_center_img", "type" : "expanded_image", "style" : ("ltr",),		"x" : 4,	"y" : 22+120, "image" : "d:/ymir work/ui/pattern/border_d_top.tga", "rect" : (0.0, 0.0, 64, 0), },
				{ "name" : "left_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 0,	"y" : 26+120, "image" : "d:/ymir work/ui/pattern/border_d_left.tga", "rect" : (0.0, 0.0, 0, 16), },
				{ "name" : "right_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 262,	"y" : 26+120, "image" : "d:/ymir work/ui/pattern/border_d_right.tga", "rect" : (0.0, 0.0, 0, 16), },
				{ "name" : "center_img", "type" : "expanded_image", "style" : ("ltr",),			"x" : 4,	"y" : 26+120, "image" : "d:/ymir work/ui/pattern/border_d_center.tga", "rect" : (0.0, 0.0, 64, 16), },
				{ "name" : "left_bottom", "type" : "image", "style" : ("ltr",),					"x" : 0,	"y" : 110+100, "image" : "d:/ymir work/ui/pattern/border_d_left_bottom.tga", },
				{ "name" : "right_bottom", "type" : "image", "style" : ("ltr",),				"x" : 262,	"y" : 110+100, "image" : "d:/ymir work/ui/pattern/border_d_right_bottom.tga", },
				{ "name" : "bottom_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 4,	"y" : 110+100, "image" : "d:/ymir work/ui/pattern/border_d_bottom.tga", "rect" : (0.0, 0.0, 64, 0), },

				## mercenary info text
				{ "name" : "mercenary_info_name_text",			"type" : "text", "x" : 0, "y" : 20, "all_align" : "center", "r":1.0, "g":1.0, "b":1.0, "a":1.0, },
				{ "name" : "mercenary_info_exp_text",			"type" : "text", "x" : 0, "y" : 36, "all_align" : "center" },
				{ "name" : "mercenary_info_skill_text",			"type" : "text", "x" : 0, "y" : 52, "all_align" : "center" },
				{ "name" : "mercenary_info_specificity_text",	"type" : "text", "x" : 0, "y" : 68, "all_align" : "center" },
			),
		},

		## mission info
		{
			"name" : "mercenary_info_window", "type" : "window", "x" : 275, "y" : 3, "width" : 335, "height" : 140,
			"children" :
			(
				{
					"name" : "mission_info_img", "type" : "image", "x" : 0, "y" : 0, "image" : ROOT_PATH + "mercenary_menu_tab_2.sub",
					"children" :
					(				
						{ "name" : "mission_info_txt",  "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.MERCENARY_MISSION_INFO, "all_align" : "center" },
					),
				},
				## mission_type
				{
					"name" : "mission_type_img", "type" : "image", "x" : 5, "y" : 25, "image" : ROOT_PATH + "mercenary_small_menu_tab_2.sub",
					"children" :
					(
						{ "name" : "mission_type_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MISSION_TYPE, "all_align" : "center" },
						## mission active value/maxvalue
						{
							"name" : "mission_type_edit_img", "type" : "image", "x" : 165, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_3.sub",
							"children" :
							(
								## mission location
								{ "name" : "mission_type_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "", "all_align" : "center" },
							),
						},
					),
				},
				## mission_limit_level
				{
					"name" : "mission_limit_level_img", "type" : "image", "x" : 5, "y" : 43, "image" : ROOT_PATH + "mercenary_small_menu_tab_2.sub",
					"children" :
					(
						{ "name" : "mission_limit_level_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MISSION_LIMIT_LEVEL, "all_align" : "center" },
						## mission active value/maxvalue
						{
							"name" : "mission_limit_level_edit_img", "type" : "image", "x" : 165, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_3.sub",
							"children" :
							(
								## mission location
								{ "name" : "mission_limit_level_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "", "all_align" : "center" },
							),
						},
					),
				},
				## mission_limit_active
				{
					"name" : "mission_limit_active_img", "type" : "image", "x" : 5, "y" : 61, "image" : ROOT_PATH + "mercenary_small_menu_tab_2.sub",
					"children" :
					(
						{ "name" : "mission_limit_active_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MISSION_LIMIT_ACTIVE, "all_align" : "center" },
						## mission active value/maxvalue
						{
							"name" : "mission_limit_active_edit_img", "type" : "image", "x" : 165, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_3.sub",
							"children" :
							(
								## mission location
								{ "name" : "mission_limit_active_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "", "all_align" : "center" },
							),
						},
					),
				},
				## mission_success_pct
				{
					"name" : "mission_success_pct_img", "type" : "image", "x" : 5, "y" : 79, "image" : ROOT_PATH + "mercenary_small_menu_tab_2.sub",
					"children" :
					(
						{ "name" : "mission_success_pct_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MISSION_SUCCESS_PCT, "all_align" : "center" },
						## mission active value/maxvalue
						{
							"name" : "mission_success_pct_ecit_img", "type" : "image", "x" : 165, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_3.sub",
							"children" :
							(
								## mission location
								{ "name" : "mission_success_pct_ecit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "", "all_align" : "center" },
							),
						},
					),
				},
				## mission_suggestion_text
				{
					"name" : "mission_suggestion_img", "type" : "image", "x" : 5, "y" : 97, "image" : ROOT_PATH + "mercenary_small_menu_tab_3.sub",
					"children" :
					(
						{ "name" : "mission_suggestion_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MISSION_SUGGESTION, "all_align" : "center" },
					),
				},
				## mission_suggestion_img
				{ "name" : "mission_suggestion_icon", "type" : "image", "x" : 168, "y" : 98, "image" : ROOT_PATH + "mercenary_mission_type_spy.sub", },
				## reward_list_btn
				{
					"name" : "reward_list_btn", "type" : "button", "x" : 204, "y" : 105,
					"default_image" : ROOT_PATH + "mercenary_button_default.sub",
					"over_image" : ROOT_PATH + "mercenary_button_over.sub",
					"down_image" : ROOT_PATH + "mercenary_button_down.sub",
					"text" : uiScriptLocale.MERCENARY_MISSION_REWARD_LIST,
				},
			),
		},

		## dispatch mercenary
		{
			"name" : "dispatch_mercenary_window", "type" : "window", "x" : 275, "y" : 135, "width" : 335, "height" : 90,
			"children" :
			(
				{
					"name" : "dispatch_mercenary_img", "type" : "image", "x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : 53, "image" : ROOT_PATH + "mercenary_menu_tab_2.sub",
					"children" :
					(				
						{ "name" : "dispatch_mercenary_txt",  "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.MERCENARY_MISSION_DISPATCH_MERCENARY, "all_align" : "center" },
					),
				},
				## mercenary_dispatch_slot
				{
					"name" : "mercenary_dispatch_slot", "type" : "slot", "x" : 0, "y" : 0, "width" : 335, "height" : 62,
					"slot" : 
					(
						{"index":0, "x":7, "y":27, "width":31, "height":31},
						{"index":1, "x":62, "y":27, "width":31, "height":31},
						{"index":2, "x":117, "y":27, "width":31, "height":31},
						{"index":3, "x":172, "y":27, "width":31, "height":31},
						{"index":4, "x":227, "y":27, "width":31, "height":31},
						{"index":5, "x":282, "y":27, "width":31, "height":31},
						## mercenary_specificity
						{"index":6, "x":39, "y":27, "width":16, "height":16},
						{"index":7, "x":94, "y":27, "width":16, "height":16},
						{"index":8, "x":149, "y":27, "width":16, "height":16},
						{"index":9, "x":204, "y":27, "width":16, "height":16},						
						{"index":10, "x":259, "y":27, "width":16, "height":16},
						{"index":11, "x":314,"y":27, "width":16, "height":16},
						## mercenary_state
						{"index":12, "x":39, "y":43, "width":16, "height":16},
						{"index":13, "x":94,  "y":43, "width":16, "height":16},
						{"index":14, "x":149, "y":43, "width":16, "height":16},
						{"index":15, "x":204, "y":43, "width":16, "height":16},						
						{"index":16, "x":259,"y":43, "width":16, "height":16},
						{"index":17, "x":314,"y":43, "width":16, "height":16},
					),
				},
				## all_cancle_btn
				{
					"name" : "all_cancle_btn", "type" : "button", "x" : 239, "y" : 65,
					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image" : PUBLIC_PATH + "large_button_02.sub",
					"down_image" : PUBLIC_PATH + "large_button_03.sub",
					"text" : uiScriptLocale.MERCENARY_MISSION_ALL_CANCLE,
				},
			),
		},

		## dispatch mercenary benefits
		{
			"name" : "dispatch_mercenary_benefits_window", "type" : "window", "x" : 275, "y" : 225, "width" : 335, "height" : 21,
			"children" :
			(
				{
					"name" : "dispatch_mercenary_benefits_img", "type" : "image", "x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : 53, "image" : ROOT_PATH + "mercenary_menu_tab_2.sub",
					"children" :
					(				
						{ "name" : "dispatch_mercenary_benefits_txt",  "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.MERCENARY_MISSION_DISPATCH_BENEFITS, "all_align" : "center" },
					),
				},
				## mission_success_pct
				{
					"name" : "mission_success_pct_img", "type" : "image", "x" : 5, "y" : 25 + 4, "image" : ROOT_PATH + "mercenary_small_menu_tab_6.sub",
					"children" :
					(
						{ "name" : "mission_success_pct_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MISSION_SUCCESS_PCT, "all_align" : "center" },
						## mission active value/maxvalue
						{
							"name" : "mission_success_pct_edit_img", "type" : "image", "x" : 190, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_6.sub",
							"children" :
							(
								## mission location
								{ "name" : "mission_success_pct_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "1%", "all_align" : "center" },
							),
						},
					),
				},
				## mission_decreas_runtime
				{
					"name" : "mission_decreas_runtime_img", "type" : "image", "x" : 5, "y" : 43 + 4, "image" : ROOT_PATH + "mercenary_small_menu_tab_6.sub",
					"children" :
					(
						{ "name" : "mission_decreas_runtime_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MISSION_DECREASE_RUNTIME, "all_align" : "center" },
						## mission active value/maxvalue
						{
							"name" : "mission_decreas_runtime_edit_img", "type" : "image", "x" : 190, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_6.sub",
							"children" :
							(
								## mission location
								{ "name" : "mission_decreas_runtime_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "", "all_align" : "center" },
							),
						},
					),
				},
				## mission_decreas_injure
				{
					"name" : "mission_decreas_injure_img", "type" : "image", "x" : 5, "y" : 61 + 4, "image" : ROOT_PATH + "mercenary_small_menu_tab_6.sub",
					"children" :
					(
						{ "name" : "mission_decreas_injure_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MISSION_DECREAS_INJURE, "all_align" : "center" },
						## mission active value/maxvalue
						{
							"name" : "mission_decreas_injure_edit_img", "type" : "image", "x" : 190, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_6.sub",
							"children" :
							(
								## mission location
								{ "name" : "mission_decreas_injure_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "", "all_align" : "center" },
							),
						},
					),
				},
				## mission_exp_bonus
				{
					"name" : "mission_exp_bonus_img", "type" : "image", "x" : 5, "y" : 79 + 4, "image" : ROOT_PATH + "mercenary_small_menu_tab_6.sub",
					"children" :
					(
						{ "name" : "mission_exp_bonus_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MISSION_EXP_BONUS, "all_align" : "center" },
						## mission active value/maxvalue
						{
							"name" : "mission_exp_bonus_edit_img", "type" : "image", "x" : 190, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_6.sub",
							"children" :
							(
								## mission location
								{ "name" : "mission_exp_bonus_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "", "all_align" : "center" },
							),
						},
					),
				},
				## mission_bonus_reward
				{
					"name" : "mission_bonus_reward_img", "type" : "image", "x" : 5, "y" : 97 + 4, "image" : ROOT_PATH + "mercenary_small_menu_tab_6.sub",
					"children" :
					(
						{ "name" : "mission_bonus_reward_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MISSION_BONUS_REWARD, "all_align" : "center" },
						## mission active value/maxvalue
						{
							"name" : "mission_bonus_reward_edit_img", "type" : "image", "x" : 190, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_6.sub",
							"children" :
							(
								## mission location
								{ "name" : "mission_bonus_reward_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "", "all_align" : "center" },
							),
						},
					),
				},
			),
		},

		## mercenary mission current info
		{
			"name" : "mercenary_mission_current_info_window", "type" : "window", "style" : ("attach", "ltr",), "x" : 275, "y" : 350, "width" : 335, "height" : 120,
			"children" :
			(
				{
					"name" : "mercenary_mission_current_info_img", "type" : "image", "x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : 53, "image" : ROOT_PATH + "mercenary_menu_tab_2.sub",
					"children" :
					(				
						{ "name" : "mercenary_mission_current_info_txt",  "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.MERCENARY_MISSION_CURRENT_INFO, "all_align" : "center" },
					),
				},
				## sub pattern bg
				{ "name" : "left_top", "type" : "image", "style" : ("ltr",),					"x" : 0,	"y" : 22, "image" : "d:/ymir work/ui/pattern/border_d_left_top.tga", },
				{ "name" : "right_top", "type" : "image", "style" : ("ltr",),					"x" : 328,	"y" : 22, "image" : "d:/ymir work/ui/pattern/border_d_right_top.tga", },
				{ "name" : "top_center_img", "type" : "expanded_image", "style" : ("ltr",),		"x" : 4,	"y" : 22, "image" : "d:/ymir work/ui/pattern/border_d_top.tga", "rect" : (0.0, 0.0, 80, 0), },
				{ "name" : "left_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 0,	"y" : 26, "image" : "d:/ymir work/ui/pattern/border_d_left.tga", "rect" : (0.0, 0.0, 0, 11), },
				{ "name" : "right_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 328,	"y" : 26, "image" : "d:/ymir work/ui/pattern/border_d_right.tga", "rect" : (0.0, 0.0, 0, 11), },
				{ "name" : "center_img", "type" : "expanded_image", "style" : ("ltr",),			"x" : 4,	"y" : 26, "image" : "d:/ymir work/ui/pattern/border_d_center.tga", "rect" : (0.0, 0.0, 80, 11), },
				{ "name" : "left_bottom", "type" : "image", "style" : ("ltr",),					"x" : 0,	"y" : 74, "image" : "d:/ymir work/ui/pattern/border_d_left_bottom.tga", },
				{ "name" : "right_bottom", "type" : "image", "style" : ("ltr",),				"x" : 328,	"y" : 74, "image" : "d:/ymir work/ui/pattern/border_d_right_bottom.tga", },
				{ "name" : "bottom_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 4,	"y" : 74, "image" : "d:/ymir work/ui/pattern/border_d_bottom.tga", "rect" : (0.0, 0.0, 80, 0), },
				## mercenary info text
				{ "name" : "current_state_text_1",  "type" : "text", "x" : 0, "y" : -18, "text" : "-", "all_align" : "center" },
				{ "name" : "current_state_text_2",  "type" : "text", "x" : 0, "y" : -2, "text" : "-", "all_align" : "center" },
				## start_dispatch_btn
				{
					"name" : "mission_control_btn", "type" : "button", "x" : 239, "y" : 80,
					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image" : PUBLIC_PATH + "large_button_02.sub",
					"down_image" : PUBLIC_PATH + "large_button_03.sub",
				},
			),
		},
	),
}