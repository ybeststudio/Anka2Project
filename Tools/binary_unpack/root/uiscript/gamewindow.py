if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

GAME_BUTTON_PATH = "d:/ymir work/ui/game/game_button{}"

window = {
	"name" : "GameWindow",
	"style" : ("not_pick",),

	"x" : 0,
	"y" : 0,

	"width" : SCREEN_WIDTH,
	"height" : SCREEN_HEIGHT,

	"children" :
	[
		{ 
			"name":"QuestButton", 
			"type":"button", 
			"x" : SCREEN_WIDTH-50-32,
			"y" : SCREEN_HEIGHT-170,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" : 
			(
				{ 
					"name":"QuestButtonLabel", 
					"type":"text", 
					"x": 16, 
					"y": 40, 
					"text":uiScriptLocale.GAME_QUEST, 
					"r":1.0, "g":1.0, "b":1.0, "a":1.0, 
					"text_horizontal_align":"center" 
				},
			),
		},
		{
			"name":"StatusPlusButton",
			"type" : "button",
			"x" : 68,
			"y" : SCREEN_HEIGHT-100,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" :
			(
				{
					"name":"StatusPlusLabel",
					"type":"text",
					"x": 38,
					"y": 0,
					"outline": True,
					"text":uiScriptLocale.GAME_STAT_UP,
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_vertical_align":"center",
					"vertical_align": "center",
				},
			),
		},
		{
			"name":"SkillPlusButton",
			"type" : "button",
			"x" : 68,
			"y" : SCREEN_HEIGHT-140,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" :
			(
				{
					"name":"SkillPlusLabel",
					"type":"text",
					"x": 38,
					"y": 0,
					"outline": True,
					"text":uiScriptLocale.GAME_SKILL_UP,
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_vertical_align":"center",
					"vertical_align": "center",
				},
			),
		},
		{
			"name":"SkillBuffiPlusButton",
			"type" : "button",
			"x" : 68,
			"y" : SCREEN_HEIGHT-140,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",
			"children" :
			(
				{
					"name":"SkillBuffiPlusLabel",
					"type":"text",
					"x": 38,
					"y": 0,
					"outline": True,
					"text":"Buffi'yi Yükselt",
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_vertical_align":"center",
					"vertical_align": "center",
				},
			),
		},
		{
			"name":"GuidelineButton",
			"type" : "button",
			"x" : 68,
			"y" : SCREEN_HEIGHT-180,
			"default_image" : GAME_BUTTON_PATH.format("/guide_norm.png"),
			"over_image" : GAME_BUTTON_PATH.format("/guide_hover.png"),
			"down_image" : GAME_BUTTON_PATH.format("/guide_select.png"),

			"children" :
			(
				{
					"name":"GuidelinePLusLabel",
					"type":"text",
					"x": 38,
					"y": 0,
					"outline": True,
					"text":"Anka2 Rehberi",
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_vertical_align":"center",
					"vertical_align": "center",
				},
			),
		},
		{
			"name":"BeginnerQuestButton",
			"type":"button",
			"x":68,
			"y":SCREEN_HEIGHT-260,
			"default_image":GAME_BUTTON_PATH.format("/begginer_norm.png"),
			"over_image":GAME_BUTTON_PATH.format("/begginer_hover.png"),
			"down_image":GAME_BUTTON_PATH.format("/begginer_select.png"),
			"children":
			(
				{
					"name":"BeginnerQuestPlusLabel",
					"type":"text",
					"x":38,
					"y":0,
					"outline":True,
					"text":"Ba\xfelang\xfd\xe7 Etkinli\xf0i",
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_vertical_align":"center",
					"vertical_align":"center",
				},
			),
		},
		{
			"name":"NotificationHistoryButton",
			"type" : "button",
			"x" : 68,
			"y" : SCREEN_HEIGHT-220,
			"default_image" : GAME_BUTTON_PATH.format("/btn_notif_0.png"),
			"over_image" : GAME_BUTTON_PATH.format("/btn_notif_1.png"),
			"down_image" : GAME_BUTTON_PATH.format("/btn_notif_2.png"),
			"children" :
			(
				{
					"name":"NotificationHistoryLabel",
					"type":"text",
					"x": 38,
					"y": 0,
					"outline": True,
					"text":"Bildirimler",
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_vertical_align":"center",
					"vertical_align": "center",
				},
			),
		},
		{ 
			"name":"ExitObserver", 
			"type" : "button", 
			"x" : SCREEN_WIDTH-50-32,
			"y" : SCREEN_HEIGHT-170,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" : 
			(
				{ 
					"name":"ExitObserverButtonName", 
					"type":"text", 
					"x": 16, 
					"y": 40, 
					"text": uiScriptLocale.GAME_EXIT_OBSERVER, 
					"r":1.0, "g":1.0, "b":1.0, "a":1.0, 
					"text_horizontal_align":"center" 
				},	
			),
		},
		{ 
			"name":"BuildGuildBuilding",
			"type" : "button",
			"x" : SCREEN_WIDTH-50-32,
			"y" : SCREEN_HEIGHT-170,
			"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
			"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
			"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",

			"children" : 
			(
				{ 
					"name":"BuildGuildBuildingButtonName",
					"type":"text",
					"x": 16,
					"y": 40,
					"text": uiScriptLocale.GUILD_BUILDING_TITLE,
					"r":1.0, "g":1.0, "b":1.0, "a":1.0,
					"text_horizontal_align":"center"
				},	
			),
		},
	],
}

if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
	window["children"] = window["children"] + [
			{ 
				"name":"GuildWarButton", 
				"type" : "button", 
				"x" : SCREEN_WIDTH-50-32,
				"y" : SCREEN_HEIGHT-240,
				"default_image" : "d:/ymir work/ui/game/windows/btn_bigplus_up.sub",
				"over_image" : "d:/ymir work/ui/game/windows/btn_bigplus_over.sub",
				"down_image" : "d:/ymir work/ui/game/windows/btn_bigplus_down.sub",
	
				"children" : 
				(
					{ 
						"name":"GuildWarButtonName", 
						"type":"text", 
						"x": 16, 
						"y": 40, 
						"text": uiScriptLocale.GAME_GUILD_WAR_JOIN, 
						"r":1.0, "g":1.0, "b":1.0, "a":1.0, 
						"text_horizontal_align":"center" 
					},	
				),
			},]
