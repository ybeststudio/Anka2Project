import uiScriptLocale

SMALL_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_01.sub"
LARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_03.sub"
XLARGE_VALUE_FILE = "d:/ymir work/ui/public/Parameter_Slot_04.sub"

BUTTON_ROOT = "d:/ymir work/ui/game/guild/guildbuttons/declarepage/"
PUBLIC_ROOT = "d:/ymir work/ui/public/"
PLUS_WIDTH = 45
window = {
	"name" : "InputDialog",

	"x" : 0,
	"y" : 0,

	"style" : ("movable", "float",),

	"width" : 230+PLUS_WIDTH,
	"height" : 240,

	"children" :
	(
		{
			"name" : "Mainboard",
			"type" : "board",
			
			"x" : 0,
			"y" : 0,
			
			"width" : 230+PLUS_WIDTH,
			"height" : 240,
			
			"children" :
			(
				## 타이틀바
				{
					"name" : "Board",
					"type" : "titlebar",
					"style" : ("attach",),
			
					"x" : 7,
					"y" : 7,

					"width" : 216+PLUS_WIDTH,
	
					"children" :
					(
						{ "name":"TitleName", "type":"text", "x":76+PLUS_WIDTH/2, "y":4, "text": uiScriptLocale.GUILD_WAR_DECLARE },
					),
				},
				## 상대길드
				{
					"name" : "InputName", "type" : "text", "x" : 15, "y" : 40, "text" : uiScriptLocale.GUILD_WAR_ENEMY,
				},
				{
					"name" : "InputSlot", "type" : "slotbar", "x" : 55+PLUS_WIDTH, "y" : 37, "width" : 130, "height" : 18,
					"children" :
					(
						{
							"name" : "InputValue", "type" : "editline", "x" : 3, "y" : 3, "width" : 90, "height" : 18, "input_limit" : 12,
						},
					),
				},
				## 전투방식
				{
					"name" : "WarType", "type" : "text", "x" : 16, "y" : 73, "text" : uiScriptLocale.GUILD_WAR_BATTLE_TYPE,
				},
				{
					"name" : "WarTypeSlot", "type" : "image", "x" : 55+PLUS_WIDTH, "y" : 70, "width" : 130, "height" : 18, "image" : XLARGE_VALUE_FILE,
					"children" :
					(
						{"name" : "WarTypeName", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.GUILD_WAR_TYPE_NORMAL,"all_align":"center",},
					),
				},
				## 전투방식 선택 버튼
				{
					"name" : "WarTypeButton",
					"type" : "button",
					"x" : 175+PLUS_WIDTH,
					"y" : 70,
					#"text" : uiScriptLocale.LOGIN_SELECT_BUTTON,

					"default_image" : BUTTON_ROOT+"WarTypeButton00.sub",
					"over_image" : BUTTON_ROOT+"WarTypeButton01.sub",
					"down_image" : BUTTON_ROOT+"WarTypeButton02.sub",
				},
				## 선승제
				{
					"name" : "WarWinType", "x" : 16, "y" : 105, "width" : 230, "height" : 50,
					"children" :
					(
						{ "name" : "WarWinName", "type" : "text", "x" : 0, "y" : 0, "text" : uiScriptLocale.GUILD_WAR_WIN, },
						{
							"name" : "WarWinbutton1",
							"type" : "radio_button",
							"x" : 60+PLUS_WIDTH/2,
							"y" : 0,
							#"text" : uiScriptLocale.GUILD_WAR_WIN_ONE,

							"default_image" : BUTTON_ROOT+"WarWinbutton100.sub",
							"over_image" : BUTTON_ROOT+"WarWinbutton101.sub",
							"down_image" : BUTTON_ROOT+"WarWinbutton102.sub",
						},						
						{
							"name" : "WarWinbutton2",
							"type" : "radio_button",
							"x" : 60+55*1+PLUS_WIDTH/2,
							"y" : 0,
							#"text" : uiScriptLocale.GUILD_WAR_WIN_THREE,

							"default_image" : BUTTON_ROOT+"WarWinbutton200.sub",
							"over_image" : BUTTON_ROOT+"WarWinbutton201.sub",
							"down_image" : BUTTON_ROOT+"WarWinbutton202.sub",
						},						
						{
							"name" : "WarWinbutton3",
							"type" : "radio_button",
							"x" : 60+55*2+PLUS_WIDTH/2,
							"y" : 0,
							#"text" : uiScriptLocale.GUILD_WAR_WIN_FIVE,

							"default_image" : BUTTON_ROOT+"WarWinbutton300.sub",
							"over_image" : BUTTON_ROOT+"WarWinbutton301.sub",
							"down_image" : BUTTON_ROOT+"WarWinbutton302.sub",
						},						
					),
				},
				## 획득점수
				{
					"name" : "WarScore", "x" : 16, "y" : 140, "width" : 230, "height" : 50,
					"children" :
					(
						{ "name" : "WarScoreName", "type" : "text", "x" : 0, "y" : 2, "text" : uiScriptLocale.GUILD_WAR_GAINSCORE, },
						{
							"name" : "WarScorebutton1",
							"type" : "radio_button",
							"x" : 60+PLUS_WIDTH/2,
							"y" : 0,
							#"text" : uiScriptLocale.GUILD_WAR_GAINSCORE_THIRTY,

							"default_image" : BUTTON_ROOT+"WarScorebutton100.sub",
							"over_image" : BUTTON_ROOT+"WarScorebutton101.sub",
							"down_image" : BUTTON_ROOT+"WarScorebutton102.sub",
						},						
						{
							"name" : "WarScorebutton2",
							"type" : "radio_button",
							"x" : 60+55*1+PLUS_WIDTH/2,
							"y" : 0,
							#"text" : uiScriptLocale.GUILD_WAR_GAINSCORE_FIFTY,

							"default_image" : BUTTON_ROOT+"WarScorebutton200.sub",
							"over_image" : BUTTON_ROOT+"WarScorebutton201.sub",
							"down_image" : BUTTON_ROOT+"WarScorebutton202.sub",
						},						
						{
							"name" : "WarScorebutton3",
							"type" : "radio_button",
							"x" : 60+55*2+PLUS_WIDTH/2,
							"y" : 0,
							#"text" : uiScriptLocale.GUILD_WAR_GAINSCORE_HUNDRED,

							"default_image" : BUTTON_ROOT+"WarScorebutton300.sub",
							"over_image" : BUTTON_ROOT+"WarScorebutton301.sub",
							"down_image" : BUTTON_ROOT+"WarScorebutton302.sub",
						},						
					),
				},
				## 경기시간
				{
					"name" : "WarTime", "x" : 16, "y" : 175, "width" : 230, "height" : 50,
					"children" :
					(
						{ "name" : "WarTimeName", "type" : "text", "x" : 0, "y" : 2, "text" : uiScriptLocale.GUILD_WAR_GAMETIME, },
						{
							"name" : "WarTimebutton1",
							"type" : "radio_button",
							"x" : 60+PLUS_WIDTH/2,
							"y" : 0,
							#"text" : uiScriptLocale.GUILD_WAR_GAMETIME_TEN,
							"default_image" : BUTTON_ROOT+"WarTimebutton100.sub",
							"over_image" : BUTTON_ROOT+"WarTimebutton101.sub",
							"down_image" : BUTTON_ROOT+"WarTimebutton102.sub",
						},						
						{
							"name" : "WarTimebutton2",
							"type" : "radio_button",
							"x" : 60+55*1+PLUS_WIDTH/2,
							"y" : 0,
							#"text" : uiScriptLocale.GUILD_WAR_GAMETIME_THIRTY,
							"default_image" : BUTTON_ROOT+"WarTimebutton200.sub",
							"over_image" : BUTTON_ROOT+"WarTimebutton201.sub",
							"down_image" : BUTTON_ROOT+"WarTimebutton202.sub",
						},						
						{
							"name" : "WarTimebutton3",
							"type" : "radio_button",
							"x" : 60+55*2+PLUS_WIDTH/2,
							"y" : 0,
							#"text" : uiScriptLocale.GUILD_WAR_GAMETIME_SIXTY,
							"default_image" : BUTTON_ROOT+"WarTimebutton300.sub",
							"over_image" : BUTTON_ROOT+"WarTimebutton301.sub",
							"down_image" : BUTTON_ROOT+"WarTimebutton302.sub",
						},						
					),
				},
				## 확인 버튼
				{
					"name" : "AcceptButton",
					"type" : "button",

					"x" : - 61 - 5 + 30,
					"y" : 205+2,
					"horizontal_align" : "center",

					#"text" : uiScriptLocale.OK,
					
					"default_image" : PUBLIC_ROOT+"AcceptButton00.sub",
					"over_image" : PUBLIC_ROOT+"AcceptButton01.sub",
					"down_image" : PUBLIC_ROOT+"AcceptButton02.sub",
				},
				## 취소 버튼
				{
					"name" : "CancelButton",
					"type" : "button",

					"x" : 5 + 30,
					"y" : 205+2,
					"horizontal_align" : "center",

					#"text" : uiScriptLocale.CANCEL,
					"default_image" : PUBLIC_ROOT+"CancleButton00.sub",
					"over_image" : PUBLIC_ROOT+"CancleButton01.sub",
					"down_image" : PUBLIC_ROOT+"CancleButton02.sub",
				},
			),
		},
	),
}
