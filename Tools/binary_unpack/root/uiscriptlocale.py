if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))

AUTOBAN_QUIZ_ANSWER = "ANSWER"
AUTOBAN_QUIZ_REFRESH = "REFRESH"
AUTOBAN_QUIZ_REST_TIME = "REST_TIME"

OPTION_SHADOW = "SHADOW"

CODEPAGE = str(app.GetDefaultCodePage())

def LoadLocaleFile(srcFileName, localeDict, optional=False):
	localeDict.setdefault("CUBE_INFO_TITLE", "Recipe")
	localeDict.setdefault("CUBE_REQUIRE_MATERIAL", "Requirements")
	localeDict.setdefault("CUBE_REQUIRE_MATERIAL_OR", "or")

	try:
		lines = pack_open(srcFileName, "r").readlines()
	except IOError:
		if optional:
			return False
		import dbg
		dbg.LogBox("LoadUIScriptLocaleError(%(srcFileName)s)" % locals())
		app.Abort()

	for line in lines:
		tokens = line[:-1].split("\t")
		if app.ENABLE_MULTI_TEXTLINE:
				for k in range(1, len(tokens)):
					tokens[k] = tokens[k].replace("\\n", "\n")
		if len(tokens) >= 2:
			localeDict[tokens[0]] = tokens[1]
		else:
			print len(tokens), lines.index(line), line
	return True

name = app.GetLocalePath()

if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
	LOCALE_UISCRIPT_PATH = "locale/common/ui/"
	LOGIN_PATH = "locale/common/ui/login/"
	GUILD_PATH = "locale/common/ui/guild/"
else:
	LOCALE_UISCRIPT_PATH = "%s/ui/" % (name)
	LOGIN_PATH = "%s/ui/login/" % (name)
	GUILD_PATH = "%s/ui/guild/" % (name)

EMPIRE_PATH = "%s/ui/empire/" % (name)
SELECT_PATH = "%s/ui/select/" % (name)
WINDOWS_PATH = "%s/ui/windows/" % (name)
MAPNAME_PATH = "%s/ui/mapname/" % (name)

JOBDESC_WARRIOR_PATH = "%s/jobdesc_warrior.txt" % (name)
JOBDESC_ASSASSIN_PATH = "%s/jobdesc_assassin.txt" % (name)
JOBDESC_SURA_PATH = "%s/jobdesc_sura.txt" % (name)
JOBDESC_SHAMAN_PATH = "%s/jobdesc_shaman.txt" % (name)

EMPIREDESC_A = "%s/empiredesc_a.txt" % (name)
EMPIREDESC_B = "%s/empiredesc_b.txt" % (name)
EMPIREDESC_C = "%s/empiredesc_c.txt" % (name)

if app.ENABLE_MINI_GAME_OKEY:
	MINIGAME_RUMI_DESC = "%s/mini_game_okey_desc.txt" % (name)

if app.ENABLE_MINI_GAME_CATCH_KING:
	MINIGAME_CATCH_KING_DESC = "%s/catchking_event_desc.txt" % (name)
	MINIGAME_CATCH_KING_SIMPLE_DESC = "%s/catchking_event_simple_desc.txt" % (name)

if app.ENABLE_FISH_EVENT_SYSTEM:
	FISH_JIGSAW_DESC = "%s/fish_event_desc.txt" % (name)

if app.ENABLE_GROWTH_PET_SYSTEM:
	PET_PRIMIUM_FEEDSTUFF_DESC = "%s/pet_primium_feedstuff_desc.txt" % (name)

LOCALE_INTERFACE_FILE_NAME = "%s/locale_interface.txt" % (name)

LoadLocaleFile(LOCALE_INTERFACE_FILE_NAME, locals())

NEW_LOCALE_INTERFACE_FILE_NAME = "%s/new_locale_interface.txt" % name
if not LoadLocaleFile(NEW_LOCALE_INTERFACE_FILE_NAME, locals(), optional=True):
	LoadLocaleFile("locale/tr/new_locale_interface.txt", locals())




LOCALE_NAME_DICT = {
	"ae": "Arabic",
	"cz": "Czech",
	"de": "German",
	"en": "English",
	"es": "Spanish",
	"fr": "French",
	"hu": "Hungarian",
	"it": "Italian",
	"pl": "Polish",
	"pt": "Portuguese",
	"ro": "Romanian",
	"tr": "Turkish",
}





