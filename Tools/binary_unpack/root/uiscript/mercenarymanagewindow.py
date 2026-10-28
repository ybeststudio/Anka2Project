import uiScriptLocale

ROOT_PATH		= "d:/ymir work/ui/game/mercenary/"
PUBLIC_PATH		= "d:/ymir work/ui/public/"
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
				{ "name" : "left_top", "type" : "image", "style" : ("ltr",),					"x" : 0,	"y" : 0, "image" : "d:/ymir work/ui/pattern/border_A_left_top.tga", },
				{ "name" : "right_top", "type" : "image", "style" : ("ltr",),					"x" : 596,	"y" : 0, "image" : "d:/ymir work/ui/pattern/border_A_right_top.tga", },
				{ "name" : "left_bottom", "type" : "image", "style" : ("ltr",),					"x" : 0,	"y" : 438, "image" : "d:/ymir work/ui/pattern/border_A_left_bottom.tga", },
				{ "name" : "right_bottom", "type" : "image", "style" : ("ltr",),				"x" : 596,	"y" : 438, "image" : "d:/ymir work/ui/pattern/border_A_right_bottom.tga", },
				{ "name" : "top_center_img", "type" : "expanded_image", "style" : ("ltr",),		"x" : 16,	"y" : 0, "image" : "d:/ymir work/ui/pattern/border_A_top.tga", "rect" : (0.0, 0.0, 36, 0), },
				{ "name" : "left_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 0,	"y" : 16, "image" : "d:/ymir work/ui/pattern/border_A_left.tga", "rect" : (0.0, 0.0, 0, 26), },
				{ "name" : "right_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 596,	"y" : 16, "image" : "d:/ymir work/ui/pattern/border_A_right.tga", "rect" : (0.0, 0.0, 0, 26), },
				{ "name" : "bottom_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 16,	"y" : 438, "image" : "d:/ymir work/ui/pattern/border_A_bottom.tga", "rect" : (0.0, 0.0, 36, 0), },
				{ "name" : "center_img", "type" : "expanded_image", "style" : ("ltr",),			"x" : 16,	"y" : 16, "image" : "d:/ymir work/ui/pattern/border_A_center.tga", "rect" : (0.0, 0.0, 36, 26), },
				{ "name" : "main_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 290,	"y" : 2, "image" : "d:/ymir work/ui/pattern/border_A_right.tga", "rect" : (0.0, 0.0, 0, 27), },
			),
		},
		## mercenary_list
		{
			"name" : "mercenary_list_window", "type" : "window", "style" : ("attach", "ltr",), "x" : 3, "y" : 3, "width" : 301, "height" : 600,
			"children" :
			(
				## mercenary_list
				{
					"name" : "mercenary_list_img", "type" : "image", "x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : 53, "image" : ROOT_PATH + "mercenary_menu_tab_3.sub",
					"children" :
					(				
						{ "name" : "mercenary_list_txt",  "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.MERCENARY_MISSION_MERCENARY_LIST, "all_align" : "center" },
					),
				},
				## sub pattern bg
				{ "name" : "left_top", "type" : "image", "style" : ("ltr",),					"x" : 1,	"y" : 22, "image" : "d:/ymir work/ui/pattern/border_d_left_top.tga", },
				{ "name" : "right_top", "type" : "image", "style" : ("ltr",),					"x" : 296,	"y" : 22, "image" : "d:/ymir work/ui/pattern/border_d_right_top.tga", },
				{ "name" : "left_bottom", "type" : "image", "style" : ("ltr",),					"x" : 1,	"y" : 390, "image" : "d:/ymir work/ui/pattern/border_d_left_bottom.tga", },
				{ "name" : "right_bottom", "type" : "image", "style" : ("ltr",),				"x" : 296,	"y" : 390, "image" : "d:/ymir work/ui/pattern/border_d_right_bottom.tga", },
				{ "name" : "top_center_img", "type" : "expanded_image", "style" : ("ltr",),		"x" : 5,	"y" : 22, "image" : "d:/ymir work/ui/pattern/border_d_top.tga", "rect" : (0.0, 0.0, 72, 0), },
				{ "name" : "left_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 1,	"y" : 26, "image" : "d:/ymir work/ui/pattern/border_d_left.tga", "rect" : (0.0, 0.0, 0, 90), },
				{ "name" : "right_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 296,	"y" : 26, "image" : "d:/ymir work/ui/pattern/border_d_right.tga", "rect" : (0.0, 0.0, 0, 90), },
				{ "name" : "bottom_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 5,	"y" : 390, "image" : "d:/ymir work/ui/pattern/border_d_bottom.tga", "rect" : (0.0, 0.0, 72, 0), },
				{ "name" : "center_img", "type" : "expanded_image", "style" : ("ltr",),			"x" : 5,	"y" : 26, "image" : "d:/ymir work/ui/pattern/border_d_center.tga", "rect" : (0.0, 0.0, 72, 90), },
				## mercenary_list_manage_slot
				{
					"name" : "mercenary_list_manage_slot", "type" : "slot", "x" : 3, "y" : 3, "width" : 301, "height" : 600,
					"slot" : 
					(
						{"index":0, "x":30, "y":33, "width":31, "height":31},
						{"index":1, "x":94, "y":33, "width":31, "height":31},
						{"index":2, "x":158, "y":33, "width":31, "height":31},
						{"index":3, "x":222, "y":33, "width":31, "height":31},
						
						{"index":4, "x":30, "y":96, "width":31, "height":31},
						{"index":5, "x":94, "y":96, "width":31, "height":31},
						{"index":6, "x":158, "y":96, "width":31, "height":31},
						{"index":7, "x":222, "y":96, "width":31, "height":31},
						
						{"index":8, "x":30, "y":159, "width":31, "height":31},
						{"index":9, "x":94, "y":159, "width":31, "height":31},
						{"index":10, "x":158, "y":159, "width":31, "height":31},
						{"index":11, "x":222, "y":159, "width":31, "height":31},
						
						{"index":12, "x":30, "y":222, "width":31, "height":31},
						{"index":13, "x":94, "y":222, "width":31, "height":31},
						{"index":14, "x":158, "y":222, "width":31, "height":31},
						{"index":15, "x":222, "y":222, "width":31, "height":31},
						
						{"index":16, "x":30, "y":285, "width":31, "height":31},
						{"index":17, "x":94, "y":285, "width":31, "height":31},
						{"index":18, "x":158, "y":285, "width":31, "height":31},
						{"index":19, "x":222, "y":285, "width":31, "height":31},
						
						{"index":20, "x":30, "y":346, "width":31, "height":31},
						{"index":21, "x":94, "y":346, "width":31, "height":31},
						{"index":22, "x":158, "y":346, "width":31, "height":31},
						{"index":23, "x":222, "y":346, "width":31, "height":31},
						## specificity_slots
						{"index":24, "x":62, "y":33, "width":16, "height":16},
						{"index":25, "x":126, "y":33, "width":16, "height":16},
						{"index":26, "x":190, "y":33, "width":16, "height":16},
						{"index":27, "x":254, "y":33, "width":16, "height":16},
						
						{"index":28, "x":62, "y":96, "width":16, "height":16},
						{"index":29, "x":126,"y":96, "width":16, "height":16},
						{"index":30, "x":190, "y":96, "width":16, "height":16},
						{"index":31, "x":254, "y":96, "width":16, "height":16},
						
						{"index":32, "x":62, "y":159, "width":16, "height":16},
						{"index":33, "x":126,"y":159, "width":16, "height":16},
						{"index":34, "x":190, "y":159, "width":16, "height":16},
						{"index":35, "x":254, "y":159, "width":16, "height":16},
						
						{"index":36, "x":62, "y":222, "width":16, "height":16},
						{"index":37, "x":126,"y":222, "width":16, "height":16},
						{"index":38, "x":190, "y":222, "width":16, "height":16},
						{"index":39, "x":254, "y":222, "width":16, "height":16},
						
						{"index":40, "x":62, "y":285, "width":16, "height":16},
						{"index":41, "x":126,"y":285, "width":16, "height":16},
						{"index":42, "x":190, "y":285, "width":16, "height":16},
						{"index":43, "x":254, "y":285, "width":16, "height":16},
						
						{"index":44, "x":62, "y":346, "width":16, "height":16},
						{"index":45, "x":126,"y":346, "width":16, "height":16},
						{"index":46, "x":190, "y":346, "width":16, "height":16},
						{"index":47, "x":254, "y":346, "width":16, "height":16},
						## state_slots
						{"index":48, "x":62, "y":49, "width":16, "height":16},
						{"index":49, "x":126, "y":49, "width":16, "height":16},
						{"index":50, "x":190, "y":49, "width":16, "height":16},
						{"index":51, "x":254, "y":49, "width":16, "height":16},
						
						{"index":52, "x":62, "y":112, "width":16, "height":16},
						{"index":53, "x":126,"y":112, "width":16, "height":16},
						{"index":54, "x":190, "y":112, "width":16, "height":16},
						{"index":55, "x":254, "y":112, "width":16, "height":16},
						
						{"index":56, "x":62, "y":175, "width":16, "height":16},
						{"index":57, "x":126,"y":175, "width":16, "height":16},
						{"index":58, "x":190, "y":175, "width":16, "height":16},
						{"index":59, "x":254, "y":175, "width":16, "height":16},
						
						{"index":60, "x":62, "y":238, "width":16, "height":16},
						{"index":61, "x":126,"y":238, "width":16, "height":16},
						{"index":62, "x":190, "y":238, "width":16, "height":16},
						{"index":63, "x":254, "y":238, "width":16, "height":16},
						
						{"index":64, "x":62, "y":301, "width":16, "height":16},
						{"index":65, "x":126,"y":301, "width":16, "height":16},
						{"index":66, "x":190, "y":301, "width":16, "height":16},
						{"index":67, "x":254, "y":301, "width":16, "height":16},
						
						{"index":68, "x":62, "y":362, "width":16, "height":16},
						{"index":69, "x":126,"y":362, "width":16, "height":16},
						{"index":70, "x":190, "y":362, "width":16, "height":16},
						{"index":71, "x":254, "y":362, "width":16, "height":16},
					),
				},
				## mercenary_list_line
				{ "name" : "mission_list_line_1", "type" : "image", "x" : 30, "y" : 80, "image" : ROOT_PATH + "mercenary_manage_line.sub",},
				{ "name" : "mission_list_line_2", "type" : "image", "x" : 30, "y" : 143, "image" : ROOT_PATH + "mercenary_manage_line.sub",},
				{ "name" : "mission_list_line_3", "type" : "image", "x" : 30, "y" : 206, "image" : ROOT_PATH + "mercenary_manage_line.sub",},
				{ "name" : "mission_list_line_4", "type" : "image", "x" : 30, "y" : 269, "image" : ROOT_PATH + "mercenary_manage_line.sub",},
				{ "name" : "mission_list_line_5", "type" : "image", "x" : 30, "y" : 332, "image" : ROOT_PATH + "mercenary_manage_line.sub",},
				## mercenary_list_left_arrow_btn
				{
					"name" : "mercenary_list_left_arrow_btn", "type" : "button", "x" : 10, "y" : 200,
					"default_image"	: ROOT_PATH + "mercenary_arrow_left_default.sub",
					"over_image"	: ROOT_PATH + "mercenary_arrow_left_over.sub",
					"down_image"	: ROOT_PATH + "mercenary_arrow_left_down.sub",
				},
				## mercenary_list_right_arrow_btn
				{
					"name" : "mercenary_list_right_arrow_btn", "type" : "button", "x" : 280, "y" : 200,					
					"default_image"	: ROOT_PATH + "mercenary_arrow_right_default.sub",
					"over_image"	: ROOT_PATH + "mercenary_arrow_right_over.sub",
					"down_image"	: ROOT_PATH + "mercenary_arrow_right_down.sub",
				},
				## mercenary_fire_btn
				{
					"name" : "mercenary_fire_btn", "type" : "button", "x" : 107, "y" : 397,
					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image" : PUBLIC_PATH + "large_button_02.sub",
					"down_image" : PUBLIC_PATH + "large_button_03.sub",
					"text" : uiScriptLocale.MERCENARY_MANAGE_FIRE,
				},				
				## mercenary_regist_drain_btn
				{
					"name" : "mercenary_regist_drain_btn", "type" : "button", "x" : 198, "y" : 397,
					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image" : PUBLIC_PATH + "large_button_02.sub",
					"down_image" : PUBLIC_PATH + "large_button_03.sub",
					"text" : uiScriptLocale.MERCENARY_MANAGE_REGIST_DRAIN,
				},

				## mercenary_immediately_cure_btn
				{
					"name" : "mercenary_immediately_cure_btn", "type" : "button", "x" : 107, "y" : 422,
					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image" : PUBLIC_PATH + "large_button_02.sub",
					"down_image" : PUBLIC_PATH + "large_button_03.sub",
					"text" : uiScriptLocale.MERCENARY_MANAGE_IMMEDIATELY_CURE,
				},				
				## mercenary_delay_cure_btn
				{
					"name" : "mercenary_delay_cure_btn", "type" : "button", "x" : 198, "y" : 422,
					"default_image" : PUBLIC_PATH + "large_button_01.sub",
					"over_image" : PUBLIC_PATH + "large_button_02.sub",
					"down_image" : PUBLIC_PATH + "large_button_03.sub",
					"text" : uiScriptLocale.MERCENARY_MANAGE_DELAY_CURE,
				},		
			),
		},

		## mercenary_info
		{
			"name" : "mercenary_info_window", "type" : "window", "x" : 307, "y" : 3, "width" : 301, "height" : 21,
			"children" :
			(
				## mercenary_info
				{
					"name" : "mercenary_info_img", "type" : "image", "x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : 53, "image" : ROOT_PATH + "mercenary_menu_tab_4.sub",
					"children" :
					(				
						{ "name" : "mercenary_info_txt",  "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.MERCENARY_MANAGE_INFO, "all_align" : "center" },
					),
				},
				## mercenary_info_name
				{
					"name" : "mercenary_info_name_img", "type" : "image", "x" : 5, "y" : 22, "image" : ROOT_PATH + "mercenary_small_menu_tab_4.sub",
					"children" :
					(
						{ "name" : "mercenary_info_name_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MANAGE_NAME, "all_align" : "center" },
						{
							"name" : "mercenary_info_name_edit_img", "type" : "image", "x" : 131, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_3.sub",
							"children" :
							(
								{ "name" : "mercenary_info_name_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "-", "all_align" : "center" },
							),
						},
					),
				},
				## mercenary_info_level
				{
					"name" : "mercenary_info_level_img", "type" : "image", "x" : 5, "y" : 40, "image" : ROOT_PATH + "mercenary_small_menu_tab_4.sub",
					"children" :
					(
						{ "name" : "mercenary_info_level_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MANAGE_LEVEL, "all_align" : "center" },
						{
							"name" : "mercenary_info_level_edit_img", "type" : "image", "x" : 131, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_3.sub",
							"children" :
							(
								{ "name" : "mercenary_info_level_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "0", "all_align" : "center" },
							),
						},
					),
				},
				## mercenary_info_exp
				{
					"name" : "mercenary_info_exp_img", "type" : "image", "x" : 5, "y" : 58, "image" : ROOT_PATH + "mercenary_small_menu_tab_4.sub",
					"children" :
					(
						{ "name" : "mercenary_info_exp_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MANAGE_EXP, "all_align" : "center" },
						{
							"name" : "mercenary_info_exp_edit_img", "type" : "image", "x" : 131, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_3.sub",
							"children" :
							(
								{ "name" : "mercenary_info_exp_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "0/100", "all_align" : "center" },
							),
						},
					),
				},
				## mercenary_info_skill
				{
					"name" : "mercenary_info_skill_img", "type" : "image", "x" : 5, "y" : 76, "image" : ROOT_PATH + "mercenary_small_menu_tab_5.sub",
					"children" :
					(
						{ "name" : "mercenary_info_skill_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MANAGE_SKILL, "all_align" : "center" },
						{
							"name" : "mercenary_info_skill_edit_img", "type" : "image", "x" : 131, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_4.sub",
							"children" :
							(
								{ "name" : "mercenary_info_skill_edit_txt_1",  "type" : "text", "color" : 0xFFFFCC00, "x" : 1, "y" : -15, "text" : "-", "all_align" : "center" },
								{ "name" : "mercenary_info_skill_edit_txt_2",  "type" : "text", "x" : 1, "y" : -1, "text" : "-", "all_align" : "center" },
								{ "name" : "mercenary_info_skill_edit_txt_3",  "type" : "text", "x" : 1, "y" : 15, "text" : "-", "all_align" : "center" },
							),
						},
					),
				},
				## mercenary_info_specificity_effect
				{
					"name" : "mercenary_info_specificity_effect_img", "type" : "image", "x" : 5, "y" : 129, "image" : ROOT_PATH + "mercenary_small_menu_tab_5.sub",
					"children" :
					(
						{ "name" : "mercenary_info_specificity_effect_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MANAGE_SPECIFICITY, "all_align" : "center" },
						{
							"name" : "mercenary_info_specificity_effect_edit_img", "type" : "image", "x" : 131, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_4.sub",
							"children" :
							(
								## mission location
								{ "name" : "mercenary_info_specificity_effect_edit_txt_1",  "type" : "text", "color" : 0xFFFFCC00, "x" : 0, "y" : -15, "text" : "-", "all_align" : "center" },
								{ "name" : "mercenary_info_specificity_effect_edit_txt_2",  "type" : "text", "x" : 0, "y" : -1, "text" : "-", "all_align" : "center" },
								{ "name" : "mercenary_info_specificity_effect_edit_txt_3",  "type" : "text", "x" : 0, "y" : 15, "text" : "-", "all_align" : "center" },
							),
						},
					),
				},
			),
		},

		## mercenary_levelup
		{
			"name" : "mercenary_levelup_window", "type" : "window", "style" : ("attach", "ltr",), "x" : 307, "y" : 186, "width" : 301, "height" : 277,
			"children" :
			(
				## mission regist
				{
					"name" : "mercenary_levelup_img", "type" : "image", "x" : 0, "y" : 0, "width" : BOARD_WIDTH, "height" : 53, "image" : ROOT_PATH + "mercenary_menu_tab_4.sub",
					"children" :
					(				
						{ "name" : "mercenary_levelup_txt",  "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.MERCENARY_MANAGE_LEVELUP, "all_align" : "center" },
					),
				},
				## sub pattern bg
				{ "name" : "left_top", "type" : "image", "style" : ("ltr",),					"x" : 1,	"y" : 22, "image" : "d:/ymir work/ui/pattern/border_d_left_top.tga", },
				{ "name" : "right_top", "type" : "image", "style" : ("ltr",),					"x" : 296,	"y" : 22, "image" : "d:/ymir work/ui/pattern/border_d_right_top.tga", },
				{ "name" : "left_bottom", "type" : "image", "style" : ("ltr",),					"x" : 1,	"y" : 260, "image" : "d:/ymir work/ui/pattern/border_d_left_bottom.tga", },
				{ "name" : "right_bottom", "type" : "image", "style" : ("ltr",),				"x" : 296,	"y" : 260, "image" : "d:/ymir work/ui/pattern/border_d_right_bottom.tga", },
				{ "name" : "top_center_img", "type" : "expanded_image", "style" : ("ltr",),		"x" : 5,	"y" : 22, "image" : "d:/ymir work/ui/pattern/border_d_top.tga", "rect" : (0.0, 0.0, 72, 0), },
				{ "name" : "left_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 1,	"y" : 26, "image" : "d:/ymir work/ui/pattern/border_d_left.tga", "rect" : (0.0, 0.0, 0, 59), },
				{ "name" : "right_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 296,	"y" : 26, "image" : "d:/ymir work/ui/pattern/border_d_right.tga", "rect" : (0.0, 0.0, 0, 59), },
				{ "name" : "bottom_center_img", "type" : "expanded_image", "style" : ("ltr",),	"x" : 5,	"y" : 260, "image" : "d:/ymir work/ui/pattern/border_d_bottom.tga", "rect" : (0.0, 0.0, 72, 0), },
				{ "name" : "center_img", "type" : "expanded_image", "style" : ("ltr",),			"x" : 5,	"y" : 26, "image" : "d:/ymir work/ui/pattern/border_d_center.tga", "rect" : (0.0, 0.0, 72, 59), },
				## mercenary_levelup_sub_window
				{
					"name" : "mercenary_levelup_sub_window", "type" : "window", "x" : 0, "y" : 0, "width" : 301, "height" : 267,
					"children" :
					(
						## text
						{ "name" : "drain_main_mercenary",  "type" : "text", "x" : 0, "y" : -100, "text" : uiScriptLocale.MERCENARY_MANAGE_DRAIN_MAIN, "all_align" : "center" },
						{ "name" : "drain_main_slot_img", "type" : "image", "x" : 0, "y" : 45, "image" : ROOT_PATH + "mercenary_empty_slot.sub", "horizontal_align" : "center" },
						## sub_mercenary_slots
						{
							"name" : "drain_main_icon_slot", "type" : "slot", "x" : 134, "y" : 52, "width" : 200, "height" : 200,
							"image" : ROOT_PATH + "mercenary_name_empty.sub",
							"slot" : 
							(
								{"index":0, "x":0, "y":0, "width":31, "height":31},
							),
						},
						## mercenary_next_level
						{
							"name" : "mercenary_next_level_img", "type" : "image", "x" : 20, "y" : 94, "image" : ROOT_PATH + "mercenary_small_menu_tab_4.sub",
							"children" :
							(
								{ "name" : "mercenary_next_level_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MANAGE_NEXT_LEVEL, "all_align" : "center" },
								{
									"name" : "mercenary_next_level_edit_img", "type" : "image", "x" : 131, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_5.sub",
									"children" :
									(
										{ "name" : "mercenary_next_level_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "0", "all_align" : "center" },
									),
								},
							),
						},						
						## mercenary_next_exp
						{
							"name" : "mercenary_next_exp_img", "type" : "image", "x" : 20, "y" : 114, "image" : ROOT_PATH + "mercenary_small_menu_tab_4.sub",
							"children" :
							(
								{ "name" : "mercenary_next_exp_txt",  "type" : "text", "x" : 0, "y" : -1, "text" : uiScriptLocale.MERCENARY_MANAGE_NEXT_EXP, "all_align" : "center" },
								{
									"name" : "mercenary_next_exp_edit_img", "type" : "image", "x" : 131, "y" : -1, "image" : ROOT_PATH + "mercenary_text_bg_5.sub",
									"children" :
									(
										{ "name" : "mercenary_next_exp_edit_txt",  "type" : "text", "x" : 1, "y" : -1, "text" : "0/1000", "all_align" : "center" },
									),
								},
							),
						},
						## drain_sub_mercenary_txt
						{ "name" : "drain_sub_mercenary_txt",  "type" : "text", "x" : 0, "y" : 10, "text" : uiScriptLocale.MERCENARY_MANAGE_DRAIN_SUB, "all_align" : "center" },
						## sub_mercenary_slots
						{
							"name" : "sub_mercenary_slot", "type" : "slot", "x" : 3, "y" : 155, "width" : 300, "height" : 200,
							"image" : ROOT_PATH + "mercenary_name_empty.sub",
							"slot" : 
							(
								{"index":0, "x":20, "y":0, "width":31, "height":31},
								{"index":1, "x":76, "y":0, "width":31, "height":31},
								{"index":2, "x":132, "y":0, "width":31, "height":31},
								{"index":3, "x":188, "y":0, "width":31, "height":31},
								{"index":4, "x":244, "y":0, "width":31, "height":31},
								
								{"index":5, "x":20, "y":38, "width":31, "height":31},
								{"index":6, "x":76, "y":38, "width":31, "height":31},
								{"index":7, "x":132, "y":38, "width":31, "height":31},
								{"index":8, "x":188, "y":38, "width":31, "height":31},
								{"index":9, "x":244, "y":38, "width":31, "height":31},
							),
						},
					),
				},				
				## drain_start_btn
				{
					"name" : "drain_start_btn", "type" : "button", "x" : 174, "y" : 235,
					"default_image" : ROOT_PATH + "mercenary_button_default.sub",
					"over_image" : ROOT_PATH + "mercenary_button_over.sub",
					"down_image" : ROOT_PATH + "mercenary_button_down.sub",
					"text" : uiScriptLocale.MERCENARY_MANAGE_DRAIN_START,
				},
			),
		},
	),
}