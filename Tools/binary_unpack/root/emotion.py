if __USE_DYNAMIC_MODULE__:
	import pyapi

chr = __import__(pyapi.GetModuleName("chr"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))

import localeInfo

EMOTION_CLAP = 1
EMOTION_CONGRATULATION = 2
EMOTION_FORGIVE = 3
EMOTION_ANGRY = 4
EMOTION_ATTRACTIVE = 5
EMOTION_SAD = 6
EMOTION_SHY = 7
EMOTION_CHEERUP = 8
EMOTION_BANTER = 9
EMOTION_JOY = 10
EMOTION_CHEERS_1 = 11
EMOTION_CHEERS_2 = 12
EMOTION_DANCE_1 = 13
EMOTION_DANCE_2 = 14
EMOTION_DANCE_3 = 15
EMOTION_DANCE_4 = 16
EMOTION_DANCE_5 = 17
EMOTION_DANCE_6 = 18
EMOTION_DANCE_7	= 19
EMOTION_SELFIE	= 20
EMOTION_DOZE	= 21
EMOTION_EXERCISE	= 22
EMOTION_PUSHUP	= 23

EMOTION_KISS = 51
EMOTION_FRENCH_KISS = 52
EMOTION_SLAP = 53

EMOTION_EMOTION_1	= 61
EMOTION_EMOTION_2	= 62
EMOTION_EMOTION_3	= 63
EMOTION_EMOTION_4	= 64
EMOTION_EMOTION_5	= 65
EMOTION_EMOTION_6	= 66
EMOTION_EMOTION_7	= 67
EMOTION_EMOTION_8	= 68
EMOTION_EMOTION_9	= 69
EMOTION_EMOTION_10	= 70
EMOTION_EMOTION_11	= 71
EMOTION_EMOTION_12	= 72
EMOTION_EMOTION_13	= 73
EMOTION_EMOTION_14	= 74
EMOTION_EMOTION_15	= 75
EMOTION_EMOTION_16	= 76
EMOTION_EMOTION_17	= 77
EMOTION_EMOTION_18	= 78
EMOTION_EMOTION_19	= 79
EMOTION_EMOTION_20	= 80
EMOTION_EMOTION_21	= 81

EMOTION_DICT = {
	EMOTION_CLAP :				{"name": localeInfo.EMOTION_CLAP,			"command":"/clap"},
	EMOTION_DANCE_1 :			{"name": localeInfo.EMOTION_DANCE_1,		"command":"/dance1"},
	EMOTION_DANCE_2 :			{"name": localeInfo.EMOTION_DANCE_2,		"command":"/dance2"},
	EMOTION_DANCE_3 :			{"name": localeInfo.EMOTION_DANCE_3,		"command":"/dance3"},
	EMOTION_DANCE_4 :			{"name": localeInfo.EMOTION_DANCE_4,		"command":"/dance4"},
	EMOTION_DANCE_5 :			{"name": localeInfo.EMOTION_DANCE_5,		"command":"/dance5"},
	EMOTION_DANCE_6 :			{"name": localeInfo.EMOTION_DANCE_6,		"command":"/dance6"},
	EMOTION_DANCE_7 :			{"name": localeInfo.EMOTION_DANCE_7,		"command":"/dance7"},
	EMOTION_SELFIE :			{"name": localeInfo.EMOTION_SELFIE,			"command":"/selfie"},
	EMOTION_DOZE :				{"name": localeInfo.EMOTION_DOZE,			"command":"/doze"},
	EMOTION_EXERCISE :			{"name": localeInfo.EMOTION_EXERCISE,		"command":"/exercise"},
	EMOTION_PUSHUP :			{"name": localeInfo.EMOTION_PUSHUP,			"command":"/pushup"},
	EMOTION_EMOTION_1 :			{"name": localeInfo.EMOTION_EMOTION_1,		"command":"/worship"},
	EMOTION_EMOTION_2 :			{"name": localeInfo.EMOTION_EMOTION_2,		"command":"/backflip"},
	EMOTION_EMOTION_3 :			{"name": localeInfo.EMOTION_EMOTION_3,		"command":"/breakdance"},
	EMOTION_EMOTION_4 :			{"name": localeInfo.EMOTION_EMOTION_4,		"command":"/dance8"},
	EMOTION_EMOTION_5 :			{"name": localeInfo.EMOTION_EMOTION_5,		"command":"/dab"},
	EMOTION_EMOTION_6 :			{"name": localeInfo.EMOTION_EMOTION_6,		"command":"/facepalm"},
	EMOTION_EMOTION_7 :			{"name": localeInfo.EMOTION_EMOTION_7,		"command":"/fuckoff"},
	EMOTION_EMOTION_8 :			{"name": localeInfo.EMOTION_EMOTION_8,		"command":"/wank"},
	EMOTION_EMOTION_9 :			{"name": localeInfo.EMOTION_EMOTION_9,		"command":"/shake_it"},
	EMOTION_EMOTION_10 :		{"name": localeInfo.EMOTION_EMOTION_10,		"command":"/throw_money"},
	EMOTION_EMOTION_11 :		{"name": localeInfo.EMOTION_EMOTION_11,		"command":"/dance9"},
	EMOTION_EMOTION_12 :		{"name": localeInfo.EMOTION_EMOTION_12,		"command":"/twerk"},

	EMOTION_EMOTION_13 :		{"name": "Limbo",							"command":"/samba"},
	EMOTION_EMOTION_14 :		{"name": "Shimmy",							"command":"/shimmy"},
	EMOTION_EMOTION_15 :		{"name": "Slow",							"command":"/slow2"},
	EMOTION_EMOTION_16 :		{"name": "Mates",							"command":"/bestmates"},
	EMOTION_EMOTION_17 :		{"name": "Street",							"command":"/flair"},
	EMOTION_EMOTION_18 :		{"name": "Parada",							"command":"/horon"},
	EMOTION_EMOTION_19 :		{"name": "Robot",							"command":"/robotdance"},
	EMOTION_EMOTION_20 :		{"name": "Snowball",						"command":"/snowball"},
	EMOTION_EMOTION_21 :		{"name": "Dead",							"command":"/surprised2"},

	EMOTION_CONGRATULATION :	{"name": localeInfo.EMOTION_CONGRATULATION,	"command":"/congratulation"},
	EMOTION_FORGIVE :			{"name": localeInfo.EMOTION_FORGIVE,		"command":"/forgive"},
	EMOTION_ANGRY :				{"name": localeInfo.EMOTION_ANGRY,			"command":"/angry"},
	EMOTION_ATTRACTIVE :		{"name": localeInfo.EMOTION_ATTRACTIVE,		"command":"/attractive"},
	EMOTION_SAD :				{"name": localeInfo.EMOTION_SAD,			"command":"/sad"},
	EMOTION_SHY :				{"name": localeInfo.EMOTION_SHY,			"command":"/shy"},
	EMOTION_CHEERUP :			{"name": localeInfo.EMOTION_CHEERUP,		"command":"/cheerup"},
	EMOTION_BANTER :			{"name": localeInfo.EMOTION_BANTER,			"command":"/banter"},
	EMOTION_JOY :				{"name": localeInfo.EMOTION_JOY,			"command":"/joy"},
	EMOTION_CHEERS_1 :			{"name": localeInfo.EMOTION_CHEERS_1,		"command":"/cheer1"},
	EMOTION_CHEERS_2 :			{"name": localeInfo.EMOTION_CHEERS_2,		"command":"/cheer2"},
	EMOTION_KISS :				{"name": localeInfo.EMOTION_CLAP_KISS,		"command":"/kiss"},
	EMOTION_FRENCH_KISS :		{"name": localeInfo.EMOTION_FRENCH_KISS,	"command":"/french_kiss"},
	EMOTION_SLAP :				{"name": localeInfo.EMOTION_SLAP,			"command":"/slap"},
}

ICON_DICT = {
	EMOTION_CLAP :				"d:/ymir work/ui/game/windows/emotion_clap.sub",
	EMOTION_CHEERS_1 :			"d:/ymir work/ui/game/windows/emotion_cheers_1.sub",
	EMOTION_CHEERS_2 :			"d:/ymir work/ui/game/windows/emotion_cheers_2.sub",

	EMOTION_DANCE_1 :			"icon/action/dance1.tga",
	EMOTION_DANCE_2 :			"icon/action/dance2.tga",

	EMOTION_CONGRATULATION :	"icon/action/congratulation.tga",
	EMOTION_FORGIVE :			"icon/action/forgive.tga",
	EMOTION_ANGRY :				"icon/action/angry.tga",
	EMOTION_ATTRACTIVE :		"icon/action/attractive.tga",
	EMOTION_SAD :				"icon/action/sad.tga",
	EMOTION_SHY :				"icon/action/shy.tga",
	EMOTION_CHEERUP :			"icon/action/cheerup.tga",
	EMOTION_BANTER :			"icon/action/banter.tga",
	EMOTION_JOY :				"icon/action/joy.tga",
	EMOTION_DANCE_1 :			"icon/action/dance1.tga",
	EMOTION_DANCE_2 :			"icon/action/dance2.tga",
	EMOTION_DANCE_3 :			"icon/action/dance3.tga",
	EMOTION_DANCE_4 :			"icon/action/dance4.tga",
	EMOTION_DANCE_5 :			"icon/action/dance5.tga",
	EMOTION_DANCE_6 :			"icon/action/dance6.tga",
	EMOTION_DANCE_7 :			"icon/action/dance7.tga",
	EMOTION_SELFIE :			"icon/action/selfie.tga",
	EMOTION_DOZE :				"icon/action/doze.tga",
	EMOTION_EXERCISE :			"icon/action/exercise.tga",
	EMOTION_PUSHUP :			"icon/action/pushup.tga",
	EMOTION_EMOTION_1 :			"icon/action/emotion_1.tga",
	EMOTION_EMOTION_2 :			"icon/action/emotion_2.tga",
	EMOTION_EMOTION_3 :			"icon/action/emotion_3.tga",
	EMOTION_EMOTION_4 :			"icon/action/emotion_4.tga",
	EMOTION_EMOTION_5 :			"icon/action/emotion_5.tga",
	EMOTION_EMOTION_6 :			"icon/action/emotion_6.tga",
	EMOTION_EMOTION_7 :			"icon/action/emotion_7.tga",
	EMOTION_EMOTION_8 :			"icon/action/emotion_8.tga",
	EMOTION_EMOTION_9 :			"icon/action/emotion_9.tga",
	EMOTION_EMOTION_10 :		"icon/action/emotion_10.tga",
	EMOTION_EMOTION_11 :		"icon/action/emotion_11.tga",
	EMOTION_EMOTION_12 :		"icon/action/emotion_12.tga",

	EMOTION_EMOTION_13 :		"icon/action/bani.tga",
	EMOTION_EMOTION_14 :		"icon/action/bani.tga",
	EMOTION_EMOTION_15 :		"icon/action/bani.tga",
	EMOTION_EMOTION_16 :		"icon/action/bestmates.tga",
	EMOTION_EMOTION_17 :		"icon/action/flair.tga",
	EMOTION_EMOTION_18 :		"icon/action/horon.tga",
	EMOTION_EMOTION_19 :		"icon/action/robotdance.tga",
	EMOTION_EMOTION_20 :		"icon/action/snowball.tga",
	EMOTION_EMOTION_21 :		"icon/action/surprised2.tga",

	EMOTION_KISS :				"d:/ymir work/ui/game/windows/emotion_kiss.sub",
	EMOTION_FRENCH_KISS :		"d:/ymir work/ui/game/windows/emotion_french_kiss.sub",
	EMOTION_SLAP :				"d:/ymir work/ui/game/windows/emotion_slap.sub",
}

ANI_DICT = {
	chr.MOTION_CLAP :						"clap.msa",
	chr.MOTION_CHEERS_1 :					"cheers_1.msa",
	chr.MOTION_CHEERS_2 :					"cheers_2.msa",
	chr.MOTION_DANCE_1 :					"dance_1.msa",
	chr.MOTION_DANCE_2 :					"dance_2.msa",
	chr.MOTION_DANCE_3 :					"dance_3.msa",
	chr.MOTION_DANCE_4 :					"dance_4.msa",
	chr.MOTION_DANCE_5 :					"dance_5.msa",
	chr.MOTION_DANCE_6 :					"dance_6.msa",
	chr.MOTION_DANCE_7 :					"dance_7.msa",
	chr.MOTION_SELFIE :						"selfie.msa",
	chr.MOTION_DOZE :						"doze.msa",
	chr.MOTION_EXERCISE :					"exercise.msa",
	chr.MOTION_PUSHUP :						"pushup.msa",
	chr.MOTION_EMOTION_1 :					"amin.msa",
	chr.MOTION_EMOTION_2 :					"backflip.msa",
	chr.MOTION_EMOTION_3 :					"break.msa",
	chr.MOTION_EMOTION_4 :					"cocalar.msa",
	chr.MOTION_EMOTION_5 :					"dab.msa",
	chr.MOTION_EMOTION_6 :					"facepalm.msa",
	chr.MOTION_EMOTION_7 :					"fuckyou.msa",
	chr.MOTION_EMOTION_8 :					"laba.msa",
	chr.MOTION_EMOTION_9 :					"oriental.msa",
	chr.MOTION_EMOTION_10 :					"ridack_animoney.msa",
	chr.MOTION_EMOTION_11 :					"pingu.msa",
	chr.MOTION_EMOTION_12 :					"twerk.msa",

	chr.MOTION_EMOTION_13 :					"samba.msa",
	chr.MOTION_EMOTION_14 :					"shimmy.msa",
	chr.MOTION_EMOTION_15 :					"slow.msa",
	chr.MOTION_EMOTION_16 :					"bestmates.msa",
	chr.MOTION_EMOTION_17 :					"flair.msa",
	chr.MOTION_EMOTION_18 :					"horon.msa",
	chr.MOTION_EMOTION_19 :					"robotdance.msa",
	chr.MOTION_EMOTION_20 :					"snowball.msa",
	chr.MOTION_EMOTION_21 :					"surprised2.msa",

	chr.MOTION_CONGRATULATION :				"congratulation.msa",
	chr.MOTION_FORGIVE :					"forgive.msa",
	chr.MOTION_ANGRY :						"angry.msa",
	chr.MOTION_ATTRACTIVE :					"attractive.msa",
	chr.MOTION_SAD :						"sad.msa",
	chr.MOTION_SHY :						"shy.msa",
	chr.MOTION_CHEERUP :					"cheerup.msa",
	chr.MOTION_BANTER :						"banter.msa",
	chr.MOTION_JOY :						"joy.msa",
	chr.MOTION_FRENCH_KISS_WITH_WARRIOR :	"french_kiss_with_warrior.msa",
	chr.MOTION_FRENCH_KISS_WITH_ASSASSIN :	"french_kiss_with_assassin.msa",
	chr.MOTION_FRENCH_KISS_WITH_SURA :		"french_kiss_with_sura.msa",
	chr.MOTION_FRENCH_KISS_WITH_SHAMAN :	"french_kiss_with_shaman.msa",
	chr.MOTION_KISS_WITH_WARRIOR :			"kiss_with_warrior.msa",
	chr.MOTION_KISS_WITH_ASSASSIN :			"kiss_with_assassin.msa",
	chr.MOTION_KISS_WITH_SURA :				"kiss_with_sura.msa",
	chr.MOTION_KISS_WITH_SHAMAN :			"kiss_with_shaman.msa",
	chr.MOTION_SLAP_HIT_WITH_WARRIOR :		"slap_hit.msa",
	chr.MOTION_SLAP_HIT_WITH_ASSASSIN :		"slap_hit.msa",
	chr.MOTION_SLAP_HIT_WITH_SURA :			"slap_hit.msa",
	chr.MOTION_SLAP_HIT_WITH_SHAMAN :		"slap_hit.msa",
	chr.MOTION_SLAP_HURT_WITH_WARRIOR :		"slap_hurt.msa",
	chr.MOTION_SLAP_HURT_WITH_ASSASSIN :	"slap_hurt.msa",
	chr.MOTION_SLAP_HURT_WITH_SURA :		"slap_hurt.msa",
	chr.MOTION_SLAP_HURT_WITH_SHAMAN :		"slap_hurt.msa",
}


def __RegisterSharedEmotionAnis(mode, path):
	chrmgr.SetPathName(path)
	chrmgr.RegisterMotionMode(mode)

	for key, val in ANI_DICT.items():
		chrmgr.RegisterMotionData(mode, key, val)

def RegisterEmotionAnis(path):
	actionPath = path + "action/"
	weddingPath = path + "wedding/"

	__RegisterSharedEmotionAnis(chr.MOTION_MODE_GENERAL, actionPath)
	__RegisterSharedEmotionAnis(chr.MOTION_MODE_WEDDING_DRESS, actionPath)

	chrmgr.SetPathName(weddingPath)
	chrmgr.RegisterMotionMode(chr.MOTION_MODE_WEDDING_DRESS)
	chrmgr.RegisterMotionData(chr.MOTION_MODE_WEDDING_DRESS, chr.MOTION_WAIT, "wait.msa")
	chrmgr.RegisterMotionData(chr.MOTION_MODE_WEDDING_DRESS, chr.MOTION_WALK, "walk.msa")
	chrmgr.RegisterMotionData(chr.MOTION_MODE_WEDDING_DRESS, chr.MOTION_RUN, "walk.msa")

def RegisterEmotionIcons():
	for key, val in ICON_DICT.items():
		player.RegisterEmotionIcon(key, val)
