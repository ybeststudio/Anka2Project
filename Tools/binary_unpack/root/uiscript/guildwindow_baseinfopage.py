if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

RESOURCE_NAME_POS = 132
if app.ENABLE_GUILDRENEWAL_SYSTEM:

	ROOT_PATH = "d:/ymir work/ui/public/"
	SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
	LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
	XLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_04.sub"
	
	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		BUTTON_ROOT = "d:/ymir work/ui/game/guild/guildbuttons/baseinfopage/"
		XXLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_05.sub"
		PLUS_WITDH = 80
		window = {
			"name" : "GuildWindow_BaseInfoPage",

			"x" : 8,
			"y" : 30,

			"width" : 360 + PLUS_WITDH,
			"height" : 298,

			"children" :
			(
				## 기지정보
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					"x" : 0,
					"y" : 8,
					"width" : 337 + PLUS_WITDH,
					"horizontal_align" : "center",
					
					"children":
					(
						## 기지정보 Text
						{
							"name" : "BaseInfo", "type" : "text", "x" : 8, "y" : 3, "text" : uiScriptLocale.GUILD_BASEINFO_TITLE,
						},				
					),
				},
				## 기지위치
				{
					"name" : "BaseLocale", "type" : "text", "x" : 20, "y" : 30, "text" : uiScriptLocale.GUILD_BASEINFO_LOCALE,
					"children" :
					(
						{
							"name" : "GuildworldSlot", "type" : "image", "x" : 68 + PLUS_WITDH/2, "y" : -2, "image" : XLARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "Base_world", "type":"text", "text":"", "x":0, "y":0, "all_align":"center"},
							),
						},
						{
							"name" : "GuildtownSlot", "type" : "image", "x" : 190 + PLUS_WITDH/2, "y" : -2, "image" : XLARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "Base_town", "type":"text", "text":"", "x":0, "y":0, "all_align":"center"},
							),
						},
					),
				},
				## 설치건물
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					"x" : 0,
					"y" : 50,
					"width" : 337 + PLUS_WITDH,
					"horizontal_align" : "center",
					
					"children":
					(
						## 설치건물 Text
						{
							"name" : "BaseInfo", "type" : "text", "x" : 8, "y" : 3, "text" : uiScriptLocale.GUILD_BASEINFO_BUILDING,
						},				
					),
				},
				
				## 힘의제단
				{
					"name" : "Guild_Power", "type" : "text" , "x" : 20, "y" : 75, "text" : uiScriptLocale.GUILD_BASEINFO_POWER,
					"children" :
					(
						{
							"name" : "GuildPowerSlot", "type" : "image", "x" : 65 + PLUS_WITDH/3, "y" : 0, "image" : XXLARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "PowerLV", "type":"text", "text": "LV.", "x":8, "y":1,},
								{"name" : "PowerLevel", "type":"text", "text": "", "x":20, "y":0, "all_align":"right"},
							),
						},
					),
				},
				## 용광료
				{
					"name" : "Guild_smelter", "type" : "text" , "x" : 20, "y" : 100, "text" : uiScriptLocale.GUILD_BASEINFO_SMEITER,
					"children" :
					(
						{
							"name" : "GuildSmelterSlot", "type" : "image", "x" : 65 + PLUS_WITDH/3, "y" : 0, "image" : XXLARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "smeltertype", "type":"text", "text": "", "x":8, "y":1,},
							),
						},
					),
				},
				## 제련소
				{
					"name" : "Guild_Factory", "type" : "text" , "x" : 20, "y" : 125, "text" : uiScriptLocale.GUILD_BASEINFO_FACTORY,
					"children" :
					(
						{
							"name" : "GuildFactorySlot", "type" : "image", "x" : 65 + PLUS_WITDH/3, "y" : 0, "image" : XXLARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "factorytype", "type":"text", "text": "", "x":8, "y":1,},
							),
						},
					),
				},
				## 길드 창고
				{
					"name" : "Guild_Bank", "type" : "text" , "x" : 20, "y" : 150, "text" : uiScriptLocale.GUILD_BASEINFO_BANK,
					"children" :
					(
						{
							"name" : "GuildBankSlot", "type" : "image", "x" : 65 + PLUS_WITDH/3, "y" : 0, "image" : XXLARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "bankLV", "type":"text", "text": "LV.", "x":8, "y":1,},
								{"name" : "banklevel", "type":"text", "text": "", "x":20, "y":0, "all_align":"right"},
							),
						},
					),
				},
				## 조경물
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					"x" : 205+PLUS_WITDH/2,
					"y" : 75,
					"width" : 180,
					
					"children":
					(
						## 조경물 Text
						{
							"name" : "BaseInfo", "type" : "text", "x" : 8, "y" : 3, "text" : uiScriptLocale.GUILD_BASEINFO_LANDSCAPE,
						},
					),
				},
				## 조경물 리스트 박스
				{
					"name" : "Guild_landscape_List",
					"type" : "slotbar",
					"x" : 210 + PLUS_WITDH/2,
					"y" : 75+20,
					"width" : 130 + PLUS_WITDH/2,
					"height" : 90-20,
					
					"children" :
					(
						{
							"name" : "LandscapeList",
							"type" : "listbox",
							"x" : 0,
							"y" : 1,
							"width" : 130 + PLUS_WITDH/2,
							"height" : 85-15,
							"horizontal_align" : "left",
						},
						{
							"name" : "LandscapeScrollBar",
							"type" : "scrollbar",
							"x" : 15,
							"y" : 2,
							"size" : 88-20,
							"horizontal_align" : "right",
						},
					),
				},
			
				## 길드 자금 및 창고 현황
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					"x" : 0,
					"y" : 175,
					"width" : 337 + PLUS_WITDH,
					"horizontal_align" : "center",
					"children" :
					(
						## 길드 자금 및 창고 현황 타이틀바
						{
							"name" : "Guild_gold_information", "type" : "text", "x" : 8, "y" : 3, "text" : uiScriptLocale.GUILD_BASEINFO_MONEYBANKINFO,
						},
					),
				},	
				## 길드자금
				{
					"name" : "ResourceInformationName", "type" : "text", "x" : 20, "y" : 200, "text" : uiScriptLocale.GUILD_BASEINFO_MONEY,
					"children" :
					(
						{
							"name" : "GuildGoldSlot", "type" : "image", "x" : 88 + PLUS_WITDH/2, "y" : -2, "image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "guildgold", "type":"text", "text": "", "x":0, "y":1,"all_align":"center"},
							),
						},
					),
				},
				## 자금최종인출자
				{
					"name" : "OutGoldMember", "type" : "text", "x" : 20, "y" : 225, "text" : uiScriptLocale.GUILD_BASEINFO_MONEYLASTOUT,
					"children" :
					(
						{
							"name" : "GuildGoldSlot", "type" : "image", "x" : 88 + PLUS_WITDH/2, "y" : -2, "image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "outgoldmember", "type":"text", "text": "", "x":0, "y":-1,"all_align":"center"},
							),
						},
					),
				},
				## 창고최종사용
				{
					"name" : "UseBankMember", "type" : "text", "x" : 20, "y" : 250, "text" : uiScriptLocale.GUILD_BASEINFO_BANKLASTOUT,
					"children" :
					(
						{
							"name" : "GuildGoldSlot", "type" : "image", "x" : 88 + PLUS_WITDH/2, "y" : -2, "image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "banklistusechr", "type":"text", "text": "", "x":0, "y":-1,"all_align":"center"},
							),
						},
					),
				},
				## 길드부지
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					"x" : 205+PLUS_WITDH/2,
					"y" : 200,
					"width" : 180,
					"children":
					(
						## 길드부지 Text
						{
							"name" : "BaseInfo", "type" : "text", "x" : 8, "y" : 3, "text" : uiScriptLocale.GUILD_BASEINFO_GUILDLAND,
						},
					),
				},
				{
					"name" : "GuildBaseAuctionTaxInfo",
					"type" : "image",

					"x" : 250,
					"y" : 225,
					"image" : BUTTON_ROOT+"GuildBaseAuctionInfo.sub",
				},
				## 길드부지포기(버튼)
				{
					"name" : "GuildBaseAbandonButton",
					"type" : "button",

					"x" : 260 + PLUS_WITDH,
					"y" : 225,
					
					"tooltip_text" : uiScriptLocale.GUILD_BASEINFO_LANDABANDON,
					
					"default_image" : BUTTON_ROOT+"GuildBaseAbandonButton00.sub",
					"over_image" : BUTTON_ROOT+"GuildBaseAbandonButton01.sub",
					"down_image" : BUTTON_ROOT+"GuildBaseAbandonButton02.sub",
				},
				## 창고 및 자금 정보 확인 버튼
				{
					"name" : "GuildBankGoldInfoButton",
					"type" : "button",

					"x" : 160,
					"y" : 275,
					
					"tooltip_text" : uiScriptLocale.GUILD_BANKINFO_TITLE,
					
					"default_image" : BUTTON_ROOT+"GuildBankGoldInfoButton00.sub",
					"over_image" : BUTTON_ROOT+"GuildBankGoldInfoButton01.sub",
					"down_image" : BUTTON_ROOT+"GuildBankGoldInfoButton02.sub",
				},			
				## 자금인출(버튼)
				{
					"name" : "GuildGoldOutButton",
					"type" : "button",

					"x" : 250,
					"y" : 275,
					
					"tooltip_text" : uiScriptLocale.GUILD_BASEINFO_MONEYOUT,
					
					"default_image" : BUTTON_ROOT+"GuildGoldOutButton00.sub",
					"over_image" : BUTTON_ROOT+"GuildGoldOutButton01.sub",
					"down_image" : BUTTON_ROOT+"GuildGoldOutButton02.sub",
				},
				## 자금입금(버튼)
				{
					"name" : "GuildGoldinButton",
					"type" : "button",

					"x" : 260 + PLUS_WITDH,
					"y" : 275,
					
					"tooltip_text" : uiScriptLocale.GUILD_BASEINFO_MONEYIN,
					
					"default_image" : BUTTON_ROOT+"GuildGoldinButton00.sub",
					"over_image" : BUTTON_ROOT+"GuildGoldinButton01.sub",
					"down_image" : BUTTON_ROOT+"GuildGoldinButton02.sub",
				},
			),
		}
	else:
		window = {
			"name" : "GuildWindow_BaseInfoPage",

			"x" : 8,
			"y" : 30,

			"width" : 360,
			"height" : 298,

			"children" :
			(
				## 기지정보
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					"x" : 0,
					"y" : 8,
					"width" : 337,
					"horizontal_align" : "center",
					
					"children":
					(
						## 기지정보 Text
						{
							"name" : "BaseInfo", "type" : "text", "x" : 8, "y" : 3, "text" : uiScriptLocale.GUILD_BASEINFO_TITLE,
						},				
					),
				},
				## 기지위치
				{
					"name" : "BaseLocale", "type" : "text", "x" : 20, "y" : 30, "text" : uiScriptLocale.GUILD_BASEINFO_LOCALE,
					"children" :
					(
						{
							"name" : "GuildworldSlot", "type" : "image", "x" : 70, "y" : -2, "image" : SMALL_VALUE_FILE,
							"children" :
							(
								{"name" : "Base_world", "type":"text", "text":"", "x":0, "y":0, "all_align":"center"},
							),
						},
						{
							"name" : "GuildtownSlot", "type" : "image", "x" : 133, "y" : -2, "image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "Base_town", "type":"text", "text":"", "x":0, "y":0, "all_align":"center"},
							),
						},
					),
				},
				## 설치건물
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					"x" : 0,
					"y" : 50,
					"width" : 337,
					"horizontal_align" : "center",
					
					"children":
					(
						## 설치건물 Text
						{
							"name" : "BaseInfo", "type" : "text", "x" : 8, "y" : 3, "text" : uiScriptLocale.GUILD_BASEINFO_BUILDING,
						},				
					),
				},
				
				## 힘의제단
				{
					"name" : "Guild_Power", "type" : "text" , "x" : 20, "y" : 75, "text" : uiScriptLocale.GUILD_BASEINFO_POWER,
					"children" :
					(
						{
							"name" : "GuildPowerSlot", "type" : "image", "x" : 65, "y" : 0, "image" : XLARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "PowerLV", "type":"text", "text": "LV.", "x":8, "y":1,},
								{"name" : "PowerLevel", "type":"text", "text": "", "x":20, "y":0, "all_align":"right"},
							),
						},
					),
				},
				## 용광료
				{
					"name" : "Guild_smelter", "type" : "text" , "x" : 20, "y" : 100, "text" : uiScriptLocale.GUILD_BASEINFO_SMEITER,
					"children" :
					(
						{
							"name" : "GuildSmelterSlot", "type" : "image", "x" : 65, "y" : 0, "image" : XLARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "smeltertype", "type":"text", "text": "", "x":8, "y":1,},
							),
						},
					),
				},
				## 제련소
				{
					"name" : "Guild_Factory", "type" : "text" , "x" : 20, "y" : 125, "text" : uiScriptLocale.GUILD_BASEINFO_FACTORY,
					"children" :
					(
						{
							"name" : "GuildFactorySlot", "type" : "image", "x" : 65, "y" : 0, "image" : XLARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "factorytype", "type":"text", "text": "", "x":8, "y":1,},
							),
						},
					),
				},
				## 길드 창고
				{
					"name" : "Guild_Bank", "type" : "text" , "x" : 20, "y" : 150, "text" : uiScriptLocale.GUILD_BASEINFO_BANK,
					"children" :
					(
						{
							"name" : "GuildBankSlot", "type" : "image", "x" : 65, "y" : 0, "image" : XLARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "bankLV", "type":"text", "text": "LV.", "x":8, "y":1,},
								{"name" : "banklevel", "type":"text", "text": "", "x":20, "y":0, "all_align":"right"},
							),
						},
					),
				},
				## 조경물
				{
					"name" : "Guild_landscape", "type" : "text", "x" : 210, "y" : 75, "text" : uiScriptLocale.GUILD_BASEINFO_LANDSCAPE,
				},
				## 조경물 리스트 박스
				{
					"name" : "Guild_landscape_List",
					"type" : "slotbar",
					"x" : 250,
					"y" : 75,
					"width" : 100,
					"height" : 90,
					
					"children" :
					(
						{
							"name" : "LandscapeList",
							"type" : "listbox",
							"x" : 0,
							"y" : 1,
							"width" : 90,
							"height" : 90,
							"horizontal_align" : "left",
						},
						{
							"name" : "LandscapeScrollBar",
							"type" : "scrollbar",
							"x" : 15,
							"y" : 2,
							"size" : 88,
							"horizontal_align" : "right",
						},
					),
				},
			
				## 길드 자금 및 창고 현황
				{
					"name" : "HorizontalBar1",
					"type" : "horizontalbar",
					"x" : 0,
					"y" : 175,
					"width" : 337,
					"horizontal_align" : "center",
					"children" :
					(
						## 길드 자금 및 창고 현황 타이틀바
						{
							"name" : "Guild_gold_information", "type" : "text", "x" : 8, "y" : 3, "text" : uiScriptLocale.GUILD_BASEINFO_MONEYBANKINFO,
						},
					),
				},	
				## 길드자금
				{
					"name" : "ResourceInformationName", "type" : "text", "x" : 20, "y" : 200, "text" : uiScriptLocale.GUILD_BASEINFO_MONEY,
					"children" :
					(
						{
							"name" : "GuildGoldSlot", "type" : "image", "x" : 88, "y" : -2, "image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "guildgold", "type":"text", "text": "", "x":0, "y":1,"all_align":"center"},
							),
						},
					),
				},
				### 금일수익
				#{
					#"name" : "TodayincomeTex", "type" : "text", "x" : 205, "y" : 200, "text" : uiScriptLocale.GUILD_BASEINFO_TODAYINCOME,
					#"children" :
					#(
						#{
							#"name" : "GuildToDaySlot", "type" : "image", "x" : 50, "y" : -2, "image" : LARGE_VALUE_FILE,
							#"children" :
							#(
								#{"name" : "todaygold", "type":"text", "text": "0", "x":0, "y":1,"all_align":"center"},
							#),
						#},
					#),
				#},
				## 자금최종인출자
				{
					"name" : "OutGoldMember", "type" : "text", "x" : 20, "y" : 225, "text" : uiScriptLocale.GUILD_BASEINFO_MONEYLASTOUT,
					"children" :
					(
						{
							"name" : "GuildGoldSlot", "type" : "image", "x" : 88, "y" : -2, "image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "outgoldmember", "type":"text", "text": "", "x":0, "y":-1,"all_align":"center"},
							),
						},
					),
				},
				## 창고최종사용
				{
					"name" : "UseBankMember", "type" : "text", "x" : 20, "y" : 250, "text" : uiScriptLocale.GUILD_BASEINFO_BANKLASTOUT,
					"children" :
					(
						{
							"name" : "GuildGoldSlot", "type" : "image", "x" : 88, "y" : -2, "image" : LARGE_VALUE_FILE,
							"children" :
							(
								{"name" : "banklistusechr", "type":"text", "text": "", "x":0, "y":-1,"all_align":"center"},
							),
						},
					),
				},
				## 자금관리
				{
					"name" : "UseBankMember", "type" : "text", "x" : 205, "y" : 250, "text" : uiScriptLocale.GUILD_BASEINFO_GOLDMANAGE,
				},			
				## 자금인출(버튼)
				{
					"name" : "GuildGoldOutButton",
					"type" : "button",

					"x" : 260,
					"y" : 250,

					"text" : uiScriptLocale.GUILD_BASEINFO_MONEYOUT,
					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",
				},
				## 자금입금(버튼)
				{
					"name" : "GuildGoldinButton",
					"type" : "button",

					"x" : 260,
					"y" : 275,

					"text" : uiScriptLocale.GUILD_BASEINFO_MONEYIN,
					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",
				},
				## 길드부지
				{
					"name" : "UseBankMember", "type" : "text", "x" : 205, "y" : 200, "text" : uiScriptLocale.GUILD_BASEINFO_GUILDLAND,
				},
				## 길드부지투표(버튼)
				{
					"name" : "GuildBaseDealVoteButton",
					"type" : "button",

					"x" : 260,
					"y" : 200,

					"text" : uiScriptLocale.GUILD_BASEINFO_VOTE,
					"default_image" : ROOT_PATH + "Small_Button_01.sub",
					"over_image" : ROOT_PATH + "Small_Button_02.sub",
					"down_image" : ROOT_PATH + "Small_Button_03.sub",
				},
				## 길드부지거래(버튼)
				{
					"name" : "GuildBaseDealButton",
					"type" : "button",

					"x" : 305,
					"y" : 200,

					"text" : uiScriptLocale.GUILD_BASEINFO_DEAL,
					"default_image" : ROOT_PATH + "Small_Button_01.sub",
					"over_image" : ROOT_PATH + "Small_Button_02.sub",
					"down_image" : ROOT_PATH + "Small_Button_03.sub",
				},
				## 길드부지포기(버튼)
				{
					"name" : "GuildBaseAbandonButton",
					"type" : "button",

					"x" : 260,
					"y" : 225,

					"text" : uiScriptLocale.GUILD_BASEINFO_LANDABANDON,
					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",
				},
				## 창고 및 자금 정보 확인 버튼
				{
					"name" : "GuildBankGoldInfoButton",
					"type" : "button",

					"x" : 160,
					"y" : 275,

					"text" : uiScriptLocale.GUILD_BASEINFO_BANKMONEYINFO,
					"default_image" : ROOT_PATH + "large_button_01.sub",
					"over_image" : ROOT_PATH + "large_button_02.sub",
					"down_image" : ROOT_PATH + "large_button_03.sub",
				},
			),
		}
else:
	window = {
		"name" : "GuildWindow_BaseInfoPage",

		"x" : 8,
		"y" : 30,

		"width" : 360,
		"height" : 298,

		"children" :
		(

			## Name
			{
				"name" : "BaseName",
				"type" : "text",
				"x" : 90,
				"y" : 10,
				"text" : uiScriptLocale.GUILD_BASENAME,
			},
			{
				"name" : "BaseNameSlot",
				"type" : "slotbar",
				"x" : 180,
				"y" : 3,
				"width" : 105,
				"height" : 24,
				"text" : uiScriptLocale.GUILD_BASENAME,
				"children" :
				(
					{
						"name" : "BaseNameValue",
						"type" : "text",
						"x" : 0,
						"y" : 0,
						"text" : uiScriptLocale.GUILD_BASENAME,
						"fontsize" : "LARGE",
						"all_align" : "center",
					},
				),
			},

			## Resource Information
			{
				"name" : "HorizontalBar1",
				"type" : "horizontalbar",
				"x" : 0,
				"y" : 32,
				"width" : 337,
				"horizontal_align" : "center",
				"children" :
				(

					{
						"name" : "ResourceInformationName",
						"type" : "text",
						"x" : 0,
						"y" : 0,
						"text" : uiScriptLocale.GUILD_RESOURCE_INFO,
						"all_align" : "center",
					},

					{
						"name" : "ResourceSlot1", "type" : "slotbar", "x" : 5 + 31*0, "y" : 38, "width" : 27, "height" : 18,
						"children" :
						(
							{ "name" : "ResourceName1", "type" : "text", "x" : 0, "y" : -16, "text" : uiScriptLocale.GUILD_WATER_STONE, "horizontal_align" : "center", "text_horizontal_align" : "center", },
							{ "name" : "ResourceValue1", "type" : "text", "x" : 0, "y" : 0, "text" : "999", "all_align" : "center", },
						),
					},
					{
						"name" : "ResourceSlot2", "type" : "slotbar", "x" : 5 + 31*1, "y" : 38, "width" : 27, "height" : 18,
						"children" :
						(
							{ "name" : "ResourceName2", "type" : "text", "x" : 0, "y" : -16, "text" : uiScriptLocale.GUILD_METIN_STONE, "horizontal_align" : "center", "text_horizontal_align" : "center", },
							{ "name" : "ResourceValue2", "type" : "text", "x" : 0, "y" : 0, "text" : "999", "all_align" : "center", },
						),
					},
					{
						"name" : "ResourceSlot3", "type" : "slotbar", "x" : 5 + 31*2, "y" : 38, "width" : 27, "height" : 18,
						"children" :
						(
							{ "name" : "ResourceName3", "type" : "text", "x" : 0, "y" : -16, "text" : uiScriptLocale.GUILD_WATER, "horizontal_align" : "center", "text_horizontal_align" : "center", },
							{ "name" : "ResourceValue3", "type" : "text", "x" : 0, "y" : 0, "text" : "999", "all_align" : "center", },
						),
					},
					{
						"name" : "ResourceSlot4", "type" : "slotbar", "x" : 5 + 31*3, "y" : 38, "width" : 27, "height" : 18,
						"children" :
						(
							{ "name" : "ResourceName4", "type" : "text", "x" : 0, "y" : -16, "text" : uiScriptLocale.GUILD_CRYSTAL, "horizontal_align" : "center", "text_horizontal_align" : "center", },
							{ "name" : "ResourceValue4", "type" : "text", "x" : 0, "y" : 0, "text" : "999", "all_align" : "center", },
						),
					},
					{
						"name" : "ResourceSlot5", "type" : "slotbar", "x" : 5 + 31*4, "y" : 38, "width" : 27, "height" : 18,
						"children" :
						(
							{ "name" : "ResourceName5", "type" : "text", "x" : 0, "y" : -16, "text" : uiScriptLocale.GUILD_MINENAL, "horizontal_align" : "center", "text_horizontal_align" : "center", },
							{ "name" : "ResourceValue5", "type" : "text", "x" : 0, "y" : 0, "text" : "999", "all_align" : "center", },
						),
					},
					{
						"name" : "ResourceSlot6", "type" : "slotbar", "x" : 5 + 31*5, "y" : 38, "width" : 27, "height" : 18,
						"children" :
						(
							{ "name" : "ResourceName6", "type" : "text", "x" : 0, "y" : -16, "text" : uiScriptLocale.GUILD_GEM, "horizontal_align" : "center", "text_horizontal_align" : "center", },
							{ "name" : "ResourceValue6", "type" : "text", "x" : 0, "y" : 0, "text" : "999", "all_align" : "center", },
						),
					},

					{
						"name" : "ResourceBasket", "type" : "bar", "x" : 195, "y" : 21, "width" : 142, "height" : 39,
						"children" :
						(
							{
								"name" : "ResourceBasketDescription1", "type" : "text", "x" : 0, "y" : 5, "text" : uiScriptLocale.GUILD_DROP_RESOURCE1, "horizontal_align" : "center", "text_horizontal_align" : "center",
							},
							{
								"name" : "ResourceBasketDescription2", "type" : "text", "x" : 0, "y" : 21, "text" : uiScriptLocale.GUILD_DROP_RESOURCE2, "horizontal_align" : "center", "text_horizontal_align" : "center",
							},
						),
					},

				),
			},

			## Resource Information
			{
				"name" : "HorizontalBar1",
				"type" : "horizontalbar",
				"x" : 0,
				"y" : 95,
				"width" : 337,
				"horizontal_align" : "center",
				"children" :
				(

					{
						"name" : "ResourceInformationName",
						"type" : "text",
						"x" : 0,
						"y" : 0,
						"text" : uiScriptLocale.GUILD_BUILDING_INFO,
						"all_align" : "center",
					},

					## BuildingName
					{
						"name" : "BuildingName", "type" : "text", "x" : 16, "y" : 20, "text" : uiScriptLocale.GUILD_BUILDING_NAME,
					},
					## Grade
					{
						"name" : "Grade", "type" : "text", "x" : 89, "y" : 20, "text" : uiScriptLocale.GUILD_BUILDING_GRADE,
					},

					## Resources
					{ "name" : "ResourceName1", "type" : "text", "x" : RESOURCE_NAME_POS + 29*0, "y" : 20, "text" : uiScriptLocale.GUILD_WATER_STONE, "text_horizontal_align" : "center", },
					{ "name" : "ResourceName2", "type" : "text", "x" : RESOURCE_NAME_POS + 29*1, "y" : 20, "text" : uiScriptLocale.GUILD_METIN_STONE, "text_horizontal_align" : "center", },
					{ "name" : "ResourceName3", "type" : "text", "x" : RESOURCE_NAME_POS + 29*2, "y" : 20, "text" : uiScriptLocale.GUILD_WATER, "text_horizontal_align" : "center", },
					{ "name" : "ResourceName4", "type" : "text", "x" : RESOURCE_NAME_POS + 29*3, "y" : 20, "text" : uiScriptLocale.GUILD_CRYSTAL, "text_horizontal_align" : "center", },
					{ "name" : "ResourceName5", "type" : "text", "x" : RESOURCE_NAME_POS + 29*4, "y" : 20, "text" : uiScriptLocale.GUILD_MINENAL, "text_horizontal_align" : "center", },
					{ "name" : "ResourceName6", "type" : "text", "x" : RESOURCE_NAME_POS + 29*5, "y" : 20, "text" : uiScriptLocale.GUILD_GEM, "text_horizontal_align" : "center", },

					## Power
					{
						"name" : "Power", "type" : "text", "x" : 303, "y" : 20, "text" : uiScriptLocale.GUILD_BUILDING_OPERATE,
					},

				),
			},

			## Buttons
			{
				"name" : "RefreshButton",
				"type" : "button",
				"x" : 337,
				"y" : 5,
				"default_image" : "d:/ymir work/ui/game/guild/Refresh_Button_01.sub",
				"over_image" : "d:/ymir work/ui/game/guild/Refresh_Button_02.sub",
				"down_image" : "d:/ymir work/ui/game/guild/Refresh_Button_03.sub",
				"tooltip_text" : uiScriptLocale.GUILD_BUILDING_REFRESH,
			},

		),
	}
