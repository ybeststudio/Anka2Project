import uiScriptLocale

ROOT_PATH = "d:/ymir work/ui/game/guild/guildRankingList/"
PAGE_IMG_PATH = "d:/ymir work/ui/public/public_page_button/"
BACK_IMG_PATH = "d:/ymir work/ui/public/public_board_back/"
BUTTON_ROOT = "d:/ymir work/ui/game/guild/guildbuttons/guildlistpage/"

window = {
	"name" : "GuildWindow_GuildListWindow",
	"style" : ("movable", "float",),

	"x" : 0,
	"y" : 0,

	"width" : 480,
	"height" : 384,

	"children" :
	(
		{
			"name" : "Board",
			"type" : "board_with_titlebar",

			"x" : 0,
			"y" : 0,

			"width" : 480,
			"height" : 378,

			"title" : "",

			"children" :
			(
				## ItemNameEditLine
				{
					"name" : "GuildNameSlot",
					"type" : "image",
					"x" : 256,
					"y" : 34,
					"image" : ROOT_PATH+"ranking_list_guildnameslot.sub",
					"children" :
					(
						{
							"name" : "GuildNameValue",
							"type" : "editline",
							"x" : 2,
							"y" : 3,
							"width" : 115,
							"height" : 18,
							"input_limit" : 12,
							"text" : "",
						},
					),
				},
				## Button
				{
					"name" : "SearchGuildButton",
					"type" : "button",
					"x" : 376,
					"y" : 34,
					"default_image" : BUTTON_ROOT+"SearchGuildButton00.sub",
					"over_image" : BUTTON_ROOT+"SearchGuildButton01.sub",
					"down_image" : BUTTON_ROOT+"SearchGuildButton02.sub",
				},
				## 센터 백그라운드 시작
				## LeftTop
				{
					"name" : "LeftTop",
					"type" : "image",
					"x" : 17,
					"y" : 57,
					"image" : BACK_IMG_PATH+"boardback_mainboxlefttop.sub",
				},
				## RightTop
				{
					"name" : "RightTop",
					"type" : "image",
					"x" : 445,
					"y" : 57,
					"image" : BACK_IMG_PATH+"boardback_mainboxrighttop.sub",
				},
				## LeftBottom
				{
					"name" : "LeftBottom",
					"type" : "image",
					"x" : 17,
					"y" : 290,
					"image" : BACK_IMG_PATH+"boardback_mainboxleftbottom.sub",
				},
				## RightBottom
				{
					"name" : "RightBottom",
					"type" : "image",
					"x" : 445,
					"y" : 290,
					"image" : BACK_IMG_PATH+"boardback_mainboxrightbottom.sub",
				},
				## leftcenterImg
				{
					"name" : "leftcenterImg",
					"type" : "expanded_image",
					"x" : 17,
					"y" : 57+15,
					"image" : BACK_IMG_PATH+"boardback_leftcenterImg.tga",
					"rect" : (0.0, 0.0, 0, 12),
				},
				## rightcenterImg
				{
					"name" : "rightcenterImg",
					"type" : "expanded_image",
					"x" : 444,
					"y" : 57+15,
					"image" : BACK_IMG_PATH+"boardback_rightcenterImg.tga",
					"rect" : (0.0, 0.0, 0, 12),
				},
				## topcenterImg
				{
					"name" : "topcenterImg",
					"type" : "expanded_image",
					"x" : 17+15,
					"y" :  57,
					"image" : BACK_IMG_PATH+"boardback_topcenterImg.tga",
					"rect" : (0.0, 0.0, 24, 0),
				},
				## bottomcenterImg
				{
					"name" : "bottomcenterImg",
					"type" : "expanded_image",
					"x" : 17+15,
					"y" : 290,
					"image" : BACK_IMG_PATH+"boardback_bottomcenterImg.tga",
					"rect" : (0.0, 0.0, 24, 0),
				},
				## centerImg
				{
					"name" : "centerImg",
					"type" : "expanded_image",
					"x" : 17+15,
					"y" : 57+15,
					"image" : BACK_IMG_PATH+"boardback_centerImg.tga",
					"rect" : (0.0, 0.0, 24, 12),
				},
				## 센터 백그라운드 끝
				## 페이지 버튼 시작
				{
					"name" : "first_prev_button", "type" : "button",
					"horizontal_align" : "center",
					"x" : -140, "y" : 286,

					"default_image" : PAGE_IMG_PATH + "page_first_prev_btn_01.sub",
					"over_image" 	: PAGE_IMG_PATH + "page_first_prev_btn_02.sub",
					"down_image" 	: PAGE_IMG_PATH + "page_first_prev_btn_01.sub",
				},
				{
					"name" : "prev_button", "type" : "button",
					"horizontal_align" : "center",
					"x" : -120, "y" : 286,

					"default_image" : PAGE_IMG_PATH + "page_prev_btn_01.sub",
					"over_image" 	: PAGE_IMG_PATH + "page_prev_btn_02.sub",
					"down_image" 	: PAGE_IMG_PATH + "page_prev_btn_01.sub",
				},
				{
					"name" : "page1_button", "type" : "button",
					"horizontal_align" : "center",
					"x" : -80, "y" : 285,

					"text" : "1",

					"default_image" : PAGE_IMG_PATH + "page_pagenumber_00.sub",
					"over_image" 	: PAGE_IMG_PATH + "page_pagenumber_01.sub",
					"down_image" 	: PAGE_IMG_PATH + "page_pagenumber_02.sub",
				},
				{
					"name" : "page2_button", "type" : "button",
					"horizontal_align" : "center",
					"x" : -40, "y" : 285,

					"text" : "2",

					"default_image" : PAGE_IMG_PATH + "page_pagenumber_00.sub",
					"over_image" 	: PAGE_IMG_PATH + "page_pagenumber_01.sub",
					"down_image" 	: PAGE_IMG_PATH + "page_pagenumber_02.sub",
				},
				{
					"name" : "page3_button", "type" : "button",
					"horizontal_align" : "center",
					"x" : 0, "y" : 285,
					
					"text" : "3",

					"default_image" : PAGE_IMG_PATH + "page_pagenumber_00.sub",
					"over_image" 	: PAGE_IMG_PATH + "page_pagenumber_01.sub",
					"down_image" 	: PAGE_IMG_PATH + "page_pagenumber_02.sub",
				},
				{
					"name" : "page4_button", "type" : "button",
					"horizontal_align" : "center",
					"x" : 40, "y" : 285,

					"text" : "4",

					"default_image" : PAGE_IMG_PATH + "page_pagenumber_00.sub",
					"over_image" 	: PAGE_IMG_PATH + "page_pagenumber_01.sub",
					"down_image" 	: PAGE_IMG_PATH + "page_pagenumber_02.sub",
				},
				{
					"name" : "page5_button", "type" : "button",
					"horizontal_align" : "center",
					"x" : 80, "y" : 285,

					"text" : "5",

					"default_image" : PAGE_IMG_PATH + "page_pagenumber_00.sub",
					"over_image" 	: PAGE_IMG_PATH + "page_pagenumber_01.sub",
					"down_image" 	: PAGE_IMG_PATH + "page_pagenumber_02.sub",
				},
				{
					"name" : "next_button", "type" : "button",
					"horizontal_align" : "center",
					"x" : 120, "y" : 286,

					"default_image" : PAGE_IMG_PATH + "page_next_btn_01.sub",
					"over_image" 	: PAGE_IMG_PATH + "page_next_btn_02.sub",
					"down_image" 	: PAGE_IMG_PATH + "page_next_btn_01.sub",
				},
				{
					"name" : "last_next_button", "type" : "button",
					"horizontal_align" : "center",
					"x" : 140, "y" : 286,

					"default_image" : PAGE_IMG_PATH + "page_last_next_btn_01.sub",
					"over_image" 	: PAGE_IMG_PATH + "page_last_next_btn_02.sub",
					"down_image" 	: PAGE_IMG_PATH + "page_last_next_btn_01.sub",
				},				
				## 페이지 버튼 끝
				## tab_menu_01
				{
					"name" : "ItemTypeImg",
					"type" : "expanded_image",
					"x" : 20,
					"y" : 60,
					"image" : ROOT_PATH+"ranking_list_menu.sub",
					"children" :
					(
						## Text
						{ "name" : "ResultNameRanking", "type" : "text", "x" : 10, "y" : 4,  "text" : "", },
						{ "name" : "ResultNameGuildOrPlayer", "type" : "text", "x" : 95, "y" : 4, "text" : "", },
						{ "name" : "ResultNameLevel", "type" : "text", "x" : 185, "y" : 4, "text" : "", },
						{ "name" : "ResultNameLadderOrJob", "type" : "text", "x" : 250, "y" : 4, "text" : "", },
						{ "name" : "ResultNameMemberOrSKill", "type" : "text", "x" : 325, "y" : 4, "text" : "", },
						{ "name" : "ResultNamePromote", "type" : "text", "x" : 380, "y" : 4, "text" : "", },
					),
				},
				## Button
				{
					"name" : "ApplicantGuildButton",
					"type" : "button",
					"x" : 281,
					"y" : 310,
					"default_image" : BUTTON_ROOT+"ApplicantGuildButton00.sub",
					"over_image" : BUTTON_ROOT+"ApplicantGuildButton01.sub",
					"down_image" : BUTTON_ROOT+"ApplicantGuildButton02.sub",
				},				
				## Button
				{
					"name" : "ShowPromoteGuildButton",
					"type" : "button",
					"x" : 374,
					"y" : 310,
					"default_image" : BUTTON_ROOT+"ShowPromoteGuildButton00.sub",
					"over_image" : BUTTON_ROOT+"ShowPromoteGuildButton01.sub",
					"down_image" : BUTTON_ROOT+"ShowPromoteGuildButton02.sub",
				},
				## Tab Area
				{
					"name" : "TabControl",
					"type" : "window",

					"x" : 0,
					"y" : 328,

					"width" : 480,
					"height" : 50,

					"children" :
					(
						## Tab
						{
							"name" : "Tab_01",
							"type" : "image",

							"x" : 0,
							"y" : 0,

							"width" : 480,
							"height" : 50,

							"image" : ROOT_PATH+"ranking_list_tab_1.sub",
						},
						{
							"name" : "Tab_02",
							"type" : "image",

							"x" : 0,
							"y" : 0,

							"width" : 480,
							"height" : 50,

							"image" : ROOT_PATH+"ranking_list_tab_2.sub",
						},
						{
							"name" : "Tab_03",
							"type" : "image",

							"x" : 0,
							"y" : 0,

							"width" : 480,
							"height" : 50,

							"image" : ROOT_PATH+"ranking_list_tab_3.sub",
						},
						{
							"name" : "Tab_04",
							"type" : "image",

							"x" : 0,
							"y" : 0,

							"width" : 480,
							"height" : 50,

							"image" : ROOT_PATH+"ranking_list_tab_4.sub",
						},
						{
							"name" : "Tab_05",
							"type" : "image",

							"x" : 0,
							"y" : 0,

							"width" : 480,
							"height" : 50,

							"image" : ROOT_PATH+"ranking_list_tab_5.sub",
						},
						## RadioButton
						{
							"name" : "Tab_Button_01",
							"type" : "radio_button",

							"x" : 0,
							"y" : 5,
							"tooltip_x" : 0,
							"tooltip_y" : 50,

							"width" : 96,
							"height" : 50,
						},
						{
							"name" : "Tab_Button_02",
							"type" : "radio_button",

							"x" : 96,
							"y" : 5,
							"tooltip_x" : 0,
							"tooltip_y" : 50,

							"width" : 96,
							"height" : 50,
						},
						{
							"name" : "Tab_Button_03",
							"type" : "radio_button",

							"x" : 96*2,
							"y" : 5,
							"tooltip_x" : 0,
							"tooltip_y" : 50,

							"width" : 96,
							"height" : 50,
						},
						{
							"name" : "Tab_Button_04",
							"type" : "radio_button",

							"x" : 96*3,
							"y" : 5,
							"tooltip_x" : 0,
							"tooltip_y" : 50,

							"width" : 96,
							"height" : 50,
						},
						{
							"name" : "Tab_Button_05",
							"type" : "radio_button",

							"x" : 96*4,
							"y" : 5,
							"tooltip_x" : 0,
							"tooltip_y" : 50,

							"width" : 96,
							"height" : 50,
						},						
					),
				},
			),
		},
	),
}
