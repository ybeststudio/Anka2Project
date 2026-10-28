import uiScriptLocale

# ENABLE_SHARED_PACK_CHANGE_LOCALE_PATH:	# 미사용 경로 제거
#LOCALE_PATH = uiScriptLocale.WINDOWS_PATH

ROOT_PATH = "d:/ymir work/ui/game/guild/guildRankingList/"
PAGE_IMG_PATH = "d:/ymir work/ui/public/public_page_button/"
BACK_IMG_PATH = "d:/ymir work/ui/public/public_board_back/"

window = {
	"name" : "RankingBoardWindow",
	"style" : ("movable", "float",),

	"x" : (SCREEN_WIDTH -480) / 2,
	"y" : (SCREEN_HEIGHT - 384) / 2,

	"width" : 480,
	"height" : 384,

	"children" :
	(
		{
			"name" : "board",
			"type" : "board_with_titlebar",
			"style" : ("ltr",),

			"x" : 0,
			"y" : 0,

			"width" : 480,
			"height" : 378,

			"title" : "랭킹",

			"children" :
			(							
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
					"style" : ("ltr",),
					"x" : 445,
					"y" : 57,
					"image" : BACK_IMG_PATH+"boardback_mainboxrighttop.sub",
				},
				## LeftBottom
				{
					"name" : "LeftBottom",
					"type" : "image",
					"style" : ("ltr",),
					"x" : 17,
					"y" : 320,
					"image" : BACK_IMG_PATH+"boardback_mainboxleftbottom.sub",
				},
				## RightBottom
				{
					"name" : "RightBottom",
					"type" : "image",
					"style" : ("ltr",),
					"x" : 445,
					"y" : 320,
					"image" : BACK_IMG_PATH+"boardback_mainboxrightbottom.sub",
				},
				## leftcenterImg
				{
					"name" : "leftcenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					"x" : 17,
					"y" : 57+15,
					"image" : BACK_IMG_PATH+"boardback_leftcenterImg.tga",
					"rect" : (0.0, 0.0, 0, 14),
				},
				## rightcenterImg
				{
					"name" : "rightcenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					"x" : 444,
					"y" : 57+15,
					"image" : BACK_IMG_PATH+"boardback_rightcenterImg.tga",
					"rect" : (0.0, 0.0, 0, 14),
				},
				## topcenterImg
				{
					"name" : "topcenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					"x" : 17+15,
					"y" :  57,
					"image" : BACK_IMG_PATH+"boardback_topcenterImg.tga",
					"rect" : (0.0, 0.0, 24, 0),
				},
				## bottomcenterImg
				{
					"name" : "bottomcenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					"x" : 17+15,
					"y" : 320,
					"image" : BACK_IMG_PATH+"boardback_bottomcenterImg.tga",
					"rect" : (0.0, 0.0, 24, 0),
				},
				## centerImg
				{
					"name" : "centerImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					"x" : 17+15,
					"y" : 57+15,
					"image" : BACK_IMG_PATH+"boardback_centerImg.tga",
					"rect" : (0.0, 0.0, 24, 14),
				},
				## 센터 백그라운드 끝
				
				## 컬럼 이름
				{
					"name" : "ItemTypeImg",
					"type" : "expanded_image",
					"x" : 20,
					"y" : 60,
					"image" : ROOT_PATH+"ranking_list_menu.sub",
					"children" :
					(
						## Text
						{ "name" : "ColumnNameRank", "type" : "text", "style" : ("ltr",), "x" : 23, "y" : 4,  "text" : "순위", "text_horizontal_align" : "center"},
						{ "name" : "ColumnNamePlayer", "type" : "text", "style" : ("ltr",), "x" : 115, "y" : 4, "text" : "이름", "text_horizontal_align" : "center"},
						{ "name" : "ColumnNameRecord", "type" : "text", "style" : ("ltr",), "x" : 205, "y" : 4, "text" : "점수", "text_horizontal_align" : "center"},
						{ "name" : "ColumnNameRecord2", "type" : "text", "style" : ("ltr",), "x" : 270, "y" : 4, "text" : "점수", "text_horizontal_align" : "center"},
						{ "name" : "ColumnNameUpdateTime", "type" : "text", "style" : ("ltr",), "x" : 357, "y" : 4, "text" : "시간", "text_horizontal_align" : "center"},
					),
				},
				## 내 기록 보여줄곳.
				## LeftTop
				{
					"name" : "LeftTop",
					"type" : "image",
					"style" : ("ltr",),
					"x" : 17,
					"y" : 340,
					"image" : BACK_IMG_PATH+"boardback_mainboxlefttop.sub",
				},
				## RightTop
				{
					"name" : "RightTop",
					"type" : "image",
					"style" : ("ltr",),
					"x" : 445,
					"y" : 340,
					"image" : BACK_IMG_PATH+"boardback_mainboxrighttop.sub",
				},
				## LeftBottom
				{
					"name" : "LeftBottom",
					"type" : "image",
					"style" : ("ltr",),
					"x" : 17,
					"y" : 350,
					"image" : BACK_IMG_PATH+"boardback_mainboxleftbottom.sub",
				},
				## RightBottom
				{
					"name" : "RightBottom",
					"type" : "image",
					"style" : ("ltr",),
					"x" : 445,
					"y" : 350,
					"image" : BACK_IMG_PATH+"boardback_mainboxrightbottom.sub",
				},
				## topcenterImg
				{
					"name" : "topcenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					"x" : 17+15,
					"y" :  340,
					"image" : BACK_IMG_PATH+"boardback_topcenterImg.tga",
					"rect" : (0.0, 0.0, 24, 0),
				},
				## bottomcenterImg
				{
					"name" : "bottomcenterImg",
					"type" : "expanded_image",
					"style" : ("ltr",),
					"x" : 17+15,
					"y" : 350,
					"image" : BACK_IMG_PATH+"boardback_bottomcenterImg.tga",
					"rect" : (0.0, 0.0, 24, 0),
				},
				## 센터 백그라운드 끝
			),
		},
	),
}
