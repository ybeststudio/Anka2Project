if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

import uiScriptLocale

if app.ENABLE_GUILDRENEWAL_SYSTEM:
	if app.ENABLE_SECOND_GUILDRENEWAL_SYSTEM:
		ROOT_DIR = "d:/ymir work/ui/game/guild/guildgradepage/"
		PLUS_WITDH = 80
		window = {
			"name" : "GuildWindow_BoardPage",

			"x" : 8+PLUS_WITDH/2,
			"y" : 30,

			"width" : 360+PLUS_WITDH,
			"height" : 298,

			"children" :
			(
				## GuildGradeTItle
				{
					"name" : "GuildGradeTItle", "type" : "image", "x" : -30, "y" : 1, "image" : ROOT_DIR+"GuildGradeTItle.sub",
				},
				## GradeNumber
				{
					"name" : "GradeNumber", "type" : "image", "x" : -4, "y" : 4, "image" : ROOT_DIR+"GradeNumber.sub",
				},
				## GradeName
				{
					"name" : "GradeName", "type" : "image", "x" : 70, "y" : 4, "image" : ROOT_DIR+"GradeName.sub",
				},
				## InviteAuthority
				{
					"name" : "InviteAuthority", "type" : "image", "x" : 133, "y" : 4, "image" : ROOT_DIR+"InviteAuthority.sub",
				},
				## DriveOutAuthority
				{
					"name" : "DriveOutAuthority", "type" : "image", "x" : 174, "y" : 4, "image" : ROOT_DIR+"DriveOutAuthority.sub",
				},
				## NoticeAuthority
				{
					"name" : "NoticeAuthority", "type" : "image", "x" : 214, "y" : 4, "image" : ROOT_DIR+"NoticeAuthority.sub",
				},
				## SkillAuthority
				{
					"name" : "SkillAuthority", "type" : "image", "x" : 253, "y" : 4, "image" : ROOT_DIR+"SkillAuthority.sub",
				},
				## GuildWar
				{
					"name" : "GuildWar", "type" : "image", "x" : 299, "y" : 4, "image" : ROOT_DIR+"GuildWar.sub",
				},
				## Bank
				{
					"name" : "Bank", "type" : "image", "x" : 337, "y" : 4, "image" : ROOT_DIR+"Bank.sub",
				},
			),
		}
	else:
		window = {
			"name" : "GuildWindow_BoardPage",

			"x" : 8,
			"y" : 30,

			"width" : 360,
			"height" : 298,

			"children" :
			(

				## GradeNumber
				{
					"name" : "GradeNumber", "type" : "text", "x" : 21, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_NUM,
				},
				## GradeName
				{
					"name" : "GradeName", "type" : "text", "x" : 76, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_RANK,
				},
				## InviteAuthority
				{
					"name" : "InviteAuthority", "type" : "text", "x" : 126, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_PERMISSION_JOIN,
				},
				## DriveOutAuthority
				{
					"name" : "DriveOutAuthority", "type" : "text", "x" : 166, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_PERMISSION_DELETE,
				},
				## NoticeAuthority
				{
					"name" : "NoticeAuthority", "type" : "text", "x" : 206, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_PERMISSION_NOTICE,
				},
				## GeneralAuthority
				{
					"name" : "SkillAuthority", "type" : "text", "x" : 246, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_PERMISSION_SKILL,
				},
				## GuildWar
				{
					"name" : "GuildWar", "type" : "text", "x" : 286, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_PERMISSION_WAR,
				},
				## Bank
				{
					"name" : "Bank", "type" : "text", "x" : 326, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_PERMISSION_BANK,
				},

			),
		}
else:
	window = {
		"name" : "GuildWindow_BoardPage",

		"x" : 8,
		"y" : 30,

		"width" : 360,
		"height" : 298,

		"children" :
		(

			## GradeNumber
			{
				"name" : "GradeNumber", "type" : "text", "x" : 21, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_NUM,
			},
			## GradeName
			{
				"name" : "GradeName", "type" : "text", "x" : 76, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_RANK,
			},
			## InviteAuthority
			{
				"name" : "InviteAuthority", "type" : "text", "x" : 126, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_PERMISSION_JOIN,
			},
			## DriveOutAuthority
			{
				"name" : "DriveOutAuthority", "type" : "text", "x" : 183, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_PERMISSION_DELETE,
			},
			## NoticeAuthority
			{
				"name" : "NoticeAuthority", "type" : "text", "x" : 240, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_PERMISSION_NOTICE,
			},
			## GeneralAuthority
			{
				"name" : "SkillAuthority", "type" : "text", "x" : 297, "y" : 5, "text" : uiScriptLocale.GUILD_GRADE_PERMISSION_SKILL,
			},

		),
	}
