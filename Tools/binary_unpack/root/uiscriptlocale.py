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

if "SHOP_REPURCHASE_BUTTON" not in locals():
	SHOP_REPURCHASE_BUTTON = "Geri al\xfdm"
if "SHOP_REPURCHASE_TITLE" not in locals():
	SHOP_REPURCHASE_TITLE = "Geri al\xfdm"

if "YIN_AND_YANG_ATTR_GUIDE_BOOK_TITLE" not in locals():
	YIN_AND_YANG_ATTR_GUIDE_BOOK_TITLE = "Yin-Yang Rehberi"
if "YIN_AND_YANG_ATTR_GUIDE_BOOK_ATTR_SLOTS_TITLE" not in locals():
	YIN_AND_YANG_ATTR_GUIDE_BOOK_ATTR_SLOTS_TITLE = "Efsun Yuvalar\xfd"
if "YIN_AND_YANG_ATTR_GUIDE_BOOK_DESCRIPTION_TITLE" not in locals():
	YIN_AND_YANG_ATTR_GUIDE_BOOK_DESCRIPTION_TITLE = "A\xe7\xfdklama"
if "YIN_AND_YANG_ATTR_HARMONY_TITLE" not in locals():
	YIN_AND_YANG_ATTR_HARMONY_TITLE = "Yin-Yang Uyumu"
if "YIN_AND_YANG_ATTR_HARMONY_ACTIVATE_BUTTON_TEXT" not in locals():
	YIN_AND_YANG_ATTR_HARMONY_ACTIVATE_BUTTON_TEXT = "Etkinle\xfetir"
if "YIN_AND_YANG_ATTR_INVENTORY_TITLE" not in locals():
	YIN_AND_YANG_ATTR_INVENTORY_TITLE = "Yin-Yang Envanteri"
if "YIN_AND_YANG_ATTR_INVENTORY_SUB_TITLE_1" not in locals():
	YIN_AND_YANG_ATTR_INVENTORY_SUB_TITLE_1 = "Yeni Efsun"
if "YIN_AND_YANG_ATTR_INVENTORY_SUB_TITLE_2" not in locals():
	YIN_AND_YANG_ATTR_INVENTORY_SUB_TITLE_2 = "Hedef E\xfeya"
if "YIN_AND_YANG_ATTR_INVENTORY_SUB_TITLE_3" not in locals():
	YIN_AND_YANG_ATTR_INVENTORY_SUB_TITLE_3 = "Efsunlar"
if "YIN_AND_YANG_ATTR_INVENTORY_BUTTON_TEXT_1" not in locals():
	YIN_AND_YANG_ATTR_INVENTORY_BUTTON_TEXT_1 = "Sil"
if "YIN_AND_YANG_ATTR_INVENTORY_BUTTON_TEXT_2" not in locals():
	YIN_AND_YANG_ATTR_INVENTORY_BUTTON_TEXT_2 = "Ekle"
if "YIN_AND_YANG_ATTR_INVENTORY_BUTTON_TEXT_3" not in locals():
	YIN_AND_YANG_ATTR_INVENTORY_BUTTON_TEXT_3 = "Geni\xfeet"

if "GUILD_LANDDEAL_VOTE" not in locals():
	GUILD_LANDDEAL_VOTE = "Arazi Anla\xfe\xfdlamas\xfd Oylamas\xfd"
if "GUILD_LANDDEAL_VOTE_DEALGUILDNAME" not in locals():
	GUILD_LANDDEAL_VOTE_DEALGUILDNAME = "Lonca Ad\xfd"
if "GUILD_LANDDEAL_VOTE_DEALGOLD" not in locals():
	GUILD_LANDDEAL_VOTE_DEALGOLD = "Fiyat"
if "GUILD_BASEINFO_GOLDMANAGE" not in locals():
	GUILD_BASEINFO_GOLDMANAGE = "Banka Y\xf6netimi"
if "GUILD_BASEINFO_VOTE" not in locals():
	GUILD_BASEINFO_VOTE = "Oyla"
if "GUILD_BASEINFO_DEAL" not in locals():
	GUILD_BASEINFO_DEAL = "Anla\xfe"
if "GUILD_BASEINFO_BANKMONEYINFO" not in locals():
	GUILD_BASEINFO_BANKMONEYINFO = "Banka Bilgisi"
if "DRAGONSOUL_TAP_TITLE_7" not in locals():
	DRAGONSOUL_TAP_TITLE_7 = "Ejderha Ametisti"

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

if "LANGUAGE_EN" not in locals():
	LANGUAGE_EN = "English"
if "LANGUAGE_TR" not in locals():
	LANGUAGE_TR = "Turkish"
if "LANGUAGE_DE" not in locals():
	LANGUAGE_DE = "Deutsch"
if "LANGUAGE_PL" not in locals():
	LANGUAGE_PL = "Polski"
if "LANGUAGE_RO" not in locals():
	LANGUAGE_RO = "Romana"
if "LANGUAGE_IT" not in locals():
	LANGUAGE_IT = "Italiano"
if "LANGUAGE_ES" not in locals():
	LANGUAGE_ES = "Espanol"
if "LANGUAGE_PT" not in locals():
	LANGUAGE_PT = "Portugues"
if "LANGUAGE_FR" not in locals():
	LANGUAGE_FR = "Francais"
if "LANGUAGE_CZ" not in locals():
	LANGUAGE_CZ = "Cestina"
if "LANGUAGE_HU" not in locals():
	LANGUAGE_HU = "Magyar"

if "OPTION_PVPMODE" not in locals():
	OPTION_PVPMODE = "PvP Modu"
if "OPTION_PVPMODE_PEACE" not in locals():
	OPTION_PVPMODE_PEACE = "Baris"
if "OPTION_PVPMODE_REVENGE" not in locals():
	OPTION_PVPMODE_REVENGE = "Intikam"
if "OPTION_PVPMODE_GUILD" not in locals():
	OPTION_PVPMODE_GUILD = "Lonca"
if "OPTION_PVPMODE_FREE" not in locals():
	OPTION_PVPMODE_FREE = "Serbest"

if "OPTION_BLOCK" not in locals():
	OPTION_BLOCK = "Engelle"
if "OPTION_BLOCK_EXCHANGE" not in locals():
	OPTION_BLOCK_EXCHANGE = "Ticaret"
if "OPTION_BLOCK_PARTY" not in locals():
	OPTION_BLOCK_PARTY = "Grup"
if "OPTION_BLOCK_GUILD" not in locals():
	OPTION_BLOCK_GUILD = "Lonca"
if "OPTION_BLOCK_WHISPER" not in locals():
	OPTION_BLOCK_WHISPER = "Fisilti"
if "OPTION_BLOCK_FRIEND" not in locals():
	OPTION_BLOCK_FRIEND = "Arkadas"
if "OPTION_BLOCK_PARTY_REQUEST" not in locals():
	OPTION_BLOCK_PARTY_REQUEST = "Grup Istegi"

if "OPTION_VIEW_CHAT" not in locals():
	OPTION_VIEW_CHAT = "Sohbeti Goster"
if "OPTION_CAMERA_DISTANCE" not in locals():
	OPTION_CAMERA_DISTANCE = "Kamera Mesafesi"

if "MUSICLIST_TITLE" not in locals():
	MUSICLIST_TITLE = "Muzik Listesi"
if "OPTION_MUSIC" not in locals():
	OPTION_MUSIC = "Muzik"
if "OPTION_MUSIC_CHANGE" not in locals():
	OPTION_MUSIC_CHANGE = "Degistir"
if "OPTION_MUSIC_DEFAULT_THEMA" not in locals():
	OPTION_MUSIC_DEFAULT_THEMA = "Varsayilan Tema"
if "OPTION_SOUND" not in locals():
	OPTION_SOUND = "Ses"
