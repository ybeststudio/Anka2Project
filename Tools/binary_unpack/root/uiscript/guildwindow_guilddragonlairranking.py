import uiScriptLocale
import item
import app

if app.ENABLE_GUILD_DRAGONLAIR_ENTRY_RANK_IMPROVE:
	ROOT_PATH		= "d:/ymir work/ui/game/guild/guildRankingList/"
	PAGE_IMG_PATH	= "d:/ymir work/ui/public/public_page_button/"
	BACK_IMG_PATH	= "d:/ymir work/ui/public/public_board_back/"
	
	WINDOW_WIDTH	= 480
	WINDOW_HEIGHT	= 384

	window = {
		"name"		: "GuildDragonLairRankingWindow",
		"style"		: ("movable", "float",),
		"x"			: SCREEN_WIDTH / 2 - WINDOW_WIDTH / 2,
		"y"			: SCREEN_HEIGHT / 2 - WINDOW_HEIGHT / 2,
		"width"		: WINDOW_WIDTH,
		"height"	: WINDOW_HEIGHT,
	
		"children" :
		(
			{
				"name"		: "board",
				"type"		: "board_with_titlebar",
				"style"		: ("attach",),
				"x"			: 0,
				"y"			: 0,
				"width"		: WINDOW_WIDTH,
				"height"	: WINDOW_HEIGHT,
				"title"		: "",
	
				"children" :
				(
					{
						"name" : "guide_button",
						"type" : "button",
						
						"x" : WINDOW_WIDTH - 50,
						"y" : 8,
						
						"default_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
						"over_image"	: "d:/ymir work/ui/pattern/q_mark_02.tga",
						"down_image"	: "d:/ymir work/ui/pattern/q_mark_01.tga",
					},

					{
						"name"		: "ranking_list_outline",
						"type"		: "outline_window",
						"style"		: ("float", ),
						"x"			: 17,
						"y"			: 57,
						"width"		: 445,
						"height"	: 279,
					},
					
					{
						"name"		: "ranking_list_menu_img",
						"type"		: "image",
						"x"			: 20,
						"y"			: 60,
						"image"		: ROOT_PATH+"ranking_list_menu.sub",
					},

					{
						"name"						: "column_rank",
						"type"						: "text",
						"style"						: ("ltr",),
						"x"							: 20 + 25,
						"y"							: 64,
						"text_horizontal_align"		: "center",
						"text"						: uiScriptLocale.GUILD_RAGONLAIR_RANKING_BOARD_COLUMN_RANK,	# 순위
					},

					{
						"name"						: "column_guild_name",
						"type"						: "text",
						"style"						: ("ltr",),
						"x"							: 20 + 110,
						"y"							: 64,
						"text_horizontal_align"		: "center",
						"text"						: uiScriptLocale.GUILD_RAGONLAIR_RANKING_BOARD_COLUMN_GUILD_NAME,	# 길드명
					},

					{
						"name"						: "column_clear_time",
						"type"						: "text",
						"style"						: ("ltr",),
						"x"							: 20 + 205,
						"y"							: 64,
						"text_horizontal_align"		: "center",
						"text"						: uiScriptLocale.GUILD_RAGONLAIR_RANKING_BOARD_COLUMN_CLEAR_TIME,	# 시간
					},

					{
						"name"						: "column_member_count",
						"type"						: "text",
						"style"						: ("ltr",),
						"x"							: 20 + 263,
						"y"							: 64,
						"text_horizontal_align"		: "center",
						"text"						: uiScriptLocale.GUILD_RAGONLAIR_RANKING_BOARD_COLUMN_MEMBER_COUNT,	# 인원
					},

					{
						"name"						: "column_update_time",
						"type"						: "text",
						"style"						: ("ltr",),
						"x"							: 20 + 362,
						"y"							: 64,
						"text_horizontal_align"		: "center",
						"text"						: uiScriptLocale.GUILD_RAGONLAIR_RANKING_BOARD_COLUMN_UPDATE_TIME,	# 갱신시간
					},

					{
						"name"		: "ranking_list",
						"type"		: "listboxex",
						"x"			: 22,
						"y"			: 82,
						"width"		: 445,
						"height"	: 280,
					},

					{
						"name"		: "my_guild_ranking_outline",
						"type"		: "outline_window",
						"style"		: ("float", ),
						"x"			: 17,
						"y"			: 340,
						"width"		: 445,
						"height"	: 26,
					},

					{
						"name"		: "my_guild_ranking",
						"type"		: "listboxex",
						"x"			: 22,
						"y"			: 343,
						"width"		: 445,
						"height"	: 30,
					},
				),
			},
		),
	}
else:
	BOARD_WIDTH = 368
	BACK_IMG_PATH = "d:/ymir work/ui/public/public_board_back/"
	ROOT_PATH = "d:/ymir work/ui/game/guild/dragonlairranking/"
	
	BOARD_WIDTH = 353
	BOARD_HEIGHT = 350
	window = {
		"name" : "GuildDragonLairWindow",
	
		"x" : 0,
		"y" : 0,
	
		"style" : ("movable", "float",),
	
		"width" : BOARD_WIDTH,
		"height" : BOARD_HEIGHT,
	
		"children" :
		(
			{
				"name" : "board",
				"type" : "board",
				"style" : ("attach",),
	
				"x" : 0,
				"y" : 0,
	
				"width" : BOARD_WIDTH,
				"height" : BOARD_HEIGHT,
			
				"children" :
				(
					## Title
					{
						"name" : "TitleBar",
						"type" : "titlebar",
						"style" : ("attach",),
	
						"x" : 6,
						"y" : 6,
	
						"width" : BOARD_WIDTH-15,
						"color" : "yellow",
	
						"children" :
						(
							{ "name":"TitleName", "type":"text", "x":BOARD_WIDTH/2, "y":3, "text":uiScriptLocale.GUILD_DRAGONLAIR_RANKING, "text_horizontal_align":"center" },
						),
					},
					## 센터 백그라운드 시작
					## LeftTop
					{
						"name" : "LeftTop",
						"type" : "image",
						"x" : 17,
						"y" : 38-5,
						"image" : BACK_IMG_PATH+"boardback_mainboxlefttop.sub",
					},
					## RightTop
					{
						"name" : "RightTop",
						"type" : "image",
						"x" : 318,
						"y" : 38-5,
						"image" : BACK_IMG_PATH+"boardback_mainboxrighttop.sub",
					},
					## LeftBottom
					{
						"name" : "LeftBottom",
						"type" : "image",
						"x" : 17,
						"y" : 173+118-5,
						"image" : BACK_IMG_PATH+"boardback_mainboxleftbottom.sub",
					},
					## RightBottom
					{
						"name" : "RightBottom",
						"type" : "image",
						"x" : 318,
						"y" : 173+118-5,
						"image" : BACK_IMG_PATH+"boardback_mainboxrightbottom.sub",
					},
					## leftcenterImg
					{
						"name" : "leftcenterImg",
						"type" : "expanded_image",
						"x" : 17,
						"y" : 38+16-5,
						"image" : BACK_IMG_PATH+"boardback_leftcenterImg.tga",
						"rect" : (0.0, 0.0, 0, 13),
					},
					## rightcenterImg
					{
						"name" : "rightcenterImg",
						"type" : "expanded_image",
						"x" : 317,
						"y" : 38+16-5,
						"image" : BACK_IMG_PATH+"boardback_rightcenterImg.tga",
						"rect" : (0.0, 0.0, 0, 13),
					},
					## topcenterImg
					{
						"name" : "topcenterImg",
						"type" : "expanded_image",
						"x" : 17+15,
						"y" :  38-5,
						"image" : BACK_IMG_PATH+"boardback_topcenterImg.tga",
						"rect" : (0.0, 0.0, 16, 0),
					},
					## bottomcenterImg
					{
						"name" : "bottomcenterImg",
						"type" : "expanded_image",
						"x" : 17+15,
						"y" : 173+118-5,
						"image" : BACK_IMG_PATH+"boardback_bottomcenterImg.tga",
						"rect" : (0.0, 0.0, 16, 0),
					},
					## centerImg
					{
						"name" : "centerImg",
						"type" : "expanded_image",
						"x" : 17+15,
						"y" : 38+15-5,
						"image" : BACK_IMG_PATH+"boardback_centerImg.tga",
						"rect" : (0.0, 0.0, 16, 13),
					},
					## 센터 백그라운드 끝
					
					## GuildTiTleImg
					{
						"name" : "GuildTiTleImg",
						"type" : "image",
						"x" : 20,
						"y" : 41-5,
						"image" : ROOT_PATH+"ranking_list_menu.sub",
						"children" :
						(
							## Text
							{ "name" : "ResultNameRanking", "type" : "text", "x" : 10, "y" : 4,  "text" : uiScriptLocale.GUILD_DRAGONLAIR_RANKING_COUNT, },
							{ "name" : "ResultNameGuild", "type" : "text", "x" : 95, "y" : 4, "text" : uiScriptLocale.GUILD_DRAGONLAIR_RANKING_NAME, },
							{ "name" : "ResultMemberCount", "type" : "text", "x" : 180, "y" : 4, "text" : uiScriptLocale.GUILD_DRAGONLAIR_RANKING_MEMBER, },
							{ "name" : "ResultClearTime", "type" : "text", "x" : 240, "y" : 4, "text" : uiScriptLocale.GUILD_DRAGONLAIR_RANKING_TIME, },
						),
					},
					
					## 스크롤 바.
					##{
					##	"name" : "GuildDragonLairScrollBar",
					##	"type" : "scrollbar",
					##	"x" : 340,
					##	"y" : 38,
					##	"size" : 180,
					##},
					
					## 센터 백그라운드 시작 (자신길드)
					## LeftTop
					{
						"name" : "LeftTopSelf",
						"type" : "image",
						"x" : 17,
						"y" : 190+120-5,
						"image" : BACK_IMG_PATH+"boardback_mainboxlefttop.sub",
					},
					## RightTop
					{
						"name" : "RightTopSelf",
						"type" : "image",
						"x" : 318,
						"y" : 190+120-5,
						"image" : BACK_IMG_PATH+"boardback_mainboxrighttop.sub",
					},
					## LeftBottom
					{
						"name" : "LeftBottomSelf",
						"type" : "image",
						"x" : 17,
						"y" : 190+15+120-5,
						"image" : BACK_IMG_PATH+"boardback_mainboxleftbottom.sub",
					},
					## RightBottom
					{
						"name" : "RightBottomSelf",
						"type" : "image",
						"x" : 318,
						"y" : 190+15+120-5,
						"image" : BACK_IMG_PATH+"boardback_mainboxrightbottom.sub",
					},
					## topcenterImg
					{
						"name" : "topcenterImgSelf",
						"type" : "expanded_image",
						"x" : 17+15,
						"y" :  190+120-5,
						"image" : BACK_IMG_PATH+"boardback_topcenterImg.tga",
						"rect" : (0.0, 0.0, 16, 0),
					},
					## bottomcenterImg
					{
						"name" : "bottomcenterImgSelf",
						"type" : "expanded_image",
						"x" : 17+15,
						"y" : 190+15+120-5,
						"image" : BACK_IMG_PATH+"boardback_bottomcenterImg.tga",
						"rect" : (0.0, 0.0, 16, 0),
					},
				),
			},
		),
	}

