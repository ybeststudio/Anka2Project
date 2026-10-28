if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

if app.ENABLE_GUILDRENEWAL_SYSTEM:
	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		BUTTON_ROOT = "d:/ymir work/ui/game/guild/guildbuttons/memberpage/"
		ROOT_DIR = "d:/ymir work/ui/game/guild/guildmemberpage/"
		PLUS_WITDH = 80
		window = {
			"name" : "GuildWindow_MemberPage",

 			"x" : 8,
			"y" : 30,

			"width" : 360 + PLUS_WITDH,
			"height" : 298,

			"children" :
			(
				## GuildGradeTItle
				{
					"name" : "GuildGradeTItle", "type" : "image", "x" : 10, "y" : 1, "image" : ROOT_DIR+"GuildMemberTItle.sub",
				},				
				## IndexName
				{
					"name" : "IndexName", "type" : "image", "x" : 45, "y" : 4, "image" : ROOT_DIR+"IndexName.sub",
				},
				## IndexGrade
				{
					"name" : "IndexGrade", "type" : "image", "x" : 141, "y" : 4, "image" : ROOT_DIR+"IndexGrade.sub",
				},
				## IndexJob
				{
					"name" : "IndexJob", "type" : "image", "x" : 229, "y" : 4, "image" : ROOT_DIR+"IndexJob.sub",
				},
				## IndexLevel
				{
					"name" : "IndexLevel", "type" : "image", "x" : 294, "y" : 4, "image" : ROOT_DIR+"IndexLevel.sub",
				},
				## IndexOffer
				{
					"name" : "IndexOffer", "type" : "image", "x" : 333, "y" : 4, "image" : ROOT_DIR+"IndexOffer.sub",
				},
				## IndexGeneral
				{
					"name" : "IndexGeneral", "type" : "image", "x" : 382, "y" : 4, "image" : ROOT_DIR+"IndexGeneral.sub",
				},
				## ScrollBar
				{
					"name" : "ScrollBar",
					"type" : "scrollbar",

					"x" : 341+PLUS_WITDH,
					"y" : 20,
					"scrollbar_type" : "normal",
					"size" : 240,
				},				
				##MemberOut Button
				{
					"name" : "MemberOutButton",
					"type" : "button",
					"x" : 10,
					"y" : 270,
					"default_image" : BUTTON_ROOT+"MemberOutButton00.sub",
					"over_image" : BUTTON_ROOT+"MemberOutButton01.sub",
					"down_image" : BUTTON_ROOT+"MemberOutButton02.sub",
				},
				##Master Change Button
				{
					"name" : "MasterChangeButton",
					"type" : "button",
					"x" : 250 + PLUS_WITDH,
					"y" : 270,
					"default_image" : BUTTON_ROOT+"MasterChangeButton00.sub",
					"over_image" : BUTTON_ROOT+"MasterChangeButton01.sub",
					"down_image" : BUTTON_ROOT+"MasterChangeButton02.sub",	
				},
			),
		}
	else:
		window = {
			"name" : "GuildWindow_MemberPage",

 			"x" : 8,
			"y" : 30,

			"width" : 360,
			"height" : 298,

			"children" :
			(
				## ScrollBar
				{
					"name" : "ScrollBar",
					"type" : "scrollbar",

					"x" : 341,
					"y" : 20,
					"scrollbar_type" : "normal",
					"size" : 240,
				},

				## Grade
				{
					"name" : "IndexName", "type" : "text", "x" : 43, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_NAME,
				},
				{
					"name" : "IndexGrade", "type" : "text", "x" : 119, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_RANK,
				},
				{
					"name" : "IndexJob", "type" : "text", "x" : 177, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_JOB,
				},
				{
					"name" : "IndexLevel", "type" : "text", "x" : 217, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_LEVEL,
				},
				{
					"name" : "IndexOffer", "type" : "text", "x" : 251, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_SPECIFIC_GRAVITY,
				},
				{
					"name" : "IndexGeneral", "type" : "text", "x" : 304, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_KNIGHT,
				},
				
				##MemberOut Button
				{
					"name" : "MemberOutButton",
					"type" : "button",
					"x" : 10,
					"y" : 264,
					
					#"tooltip_text" : uiScriptLocale.GUILD_MEMBER_OUT,
					
					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				
				##Vote Check Button
				{
					"name" : "VoteCheckButton",
					"type" : "button",
					"x" : 120,
					"y" : 264,
					
					#"tooltip_text" : uiScriptLocale.GUILD_MEMBER_VOTE_CHECK,
					
					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
				
				
				##Master Change Button
				{
					"name" : "MasterChangeButton",
					"type" : "button",
					"x" : 250,
					"y" : 264,
					
					#"tooltip_text" : uiScriptLocale.GUILDWINDOW_BUTTON_CHANGEMASTER,
					
					"default_image" : "d:/ymir work/ui/public/large_button_01.sub",
					"over_image" : "d:/ymir work/ui/public/large_button_02.sub",
					"down_image" : "d:/ymir work/ui/public/large_button_03.sub",
				},
			),
		}
else:

	window = {
		"name" : "GuildWindow_MemberPage",

		"x" : 8,
		"y" : 30,

		"width" : 360,
		"height" : 298,

		"children" :
		(
			## ScrollBar
			{
				"name" : "ScrollBar",
				"type" : "scrollbar",

				"x" : 341,
				"y" : 20,
				"scrollbar_type" : "normal",
				"size" : 270,
			},

			## Grade
			{
				"name" : "IndexName", "type" : "text", "x" : 43, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_NAME,
			},
			{
				"name" : "IndexGrade", "type" : "text", "x" : 119, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_RANK,
			},
			{
				"name" : "IndexJob", "type" : "text", "x" : 177, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_JOB,
			},
			{
				"name" : "IndexLevel", "type" : "text", "x" : 217, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_LEVEL,
			},
			{
				"name" : "IndexOffer", "type" : "text", "x" : 251, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_SPECIFIC_GRAVITY,
			},
			{
				"name" : "IndexGeneral", "type" : "text", "x" : 304, "y" : 8, "text" : uiScriptLocale.GUILD_MEMBER_KNIGHT,
			},

		),
	}
