# -*- coding: utf-8 -*-
if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))
pack = __import__(pyapi.GetModuleName("pack"))

import constInfo
import dbg

def ShopSecondsToDHM(time):
	if time < 60:
		if IsARABIC():
			return "%.2f%s" % (time, IKASHOP_SECOND)
		else:
			return "0" + IKASHOP_MINUTE

	second = int(time % 60)
	minute = int((time / 60) % 60)
	hour = int((time / 60) / 60) % 24
	day = int(int((time / 60) / 60) / 24)

	text = ""

	if day > 0:
		text += str(day) + IKASHOP_DAY
		text += " "

	if hour > 0:
		text += str(hour) + IKASHOP_HOUR
		text += " "

	if minute > 0:
		text += str(minute) + IKASHOP_MINUTE

	return text

def NumberToWithItemString(n,c) :
	if n <= 0 :
		return "0 %s" % (c)
	return "%s x | %s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]), c)

def NumberToString(n):
	if n < 0:
		return "-" + NumberToString(-n)

	ret = ""
	while n >= 1000:
		r = n % 1000
		ret = (".%03d"%r) + ret
		n //= 1000

	return str(n) + ret

def DottedNumber(n) :
	return "%s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ])) 

def SecondToDHMSNew(time):
	if time < 60:
		return "0" + MINUTE

	second = int(time % 60)
	minute = int((time / 60) % 60)
	hour = int((time / 60) / 60) % 24
	day = int(int((time / 60) / 60) / 24)
	text = ""

	if day > 0:
		text += str(day) + DAY
		text += " "

	if hour > 0:
		text += str(hour) + HOUR
		text += " "

	if minute > 0:
		text += str(minute) + MINUTE

	if second > 0:
		text += str(second) + "s"

	return text

def SecondToMS(time):
	if time < 60:
		return "%d%s" % (time, SECOND)

	second = int(time % 60)
	minute = int((time / 60) % 60)

	text = ""

	if minute > 0:
		text += str(minute) + MINUTE
		if minute > 0:
			text += " "

	if second > 0:
		text += str(second) + SECOND

	return text

APP_TITLE 							= 'METIN2'

BLEND_POTION_NO_TIME 				= ''
BLEND_POTION_NO_INFO				= ''

LOGIN_FAILURE_WRONG_SOCIALID 		= 'LOGIN_FAILURE_WRONG_SOCIALID'
LOGIN_FAILURE_SHUTDOWN_TIME 		= 'LOGIN_FAILURE_SHUTDOWN_TIME'

GUILD_MEMBER_COUNT_INFINITY 		= 'INFINITY'
GUILD_MARK_MIN_LEVEL 				= '3'
GUILD_BUILDING_LIST_TXT 			= app.GetLocalePath() + '%s/GuildBuildingList.txt'

if app.ENABLE_RENEWAL_TEAM_AFFECT:
	FN_SA_MARK						= "d:/ymir work/ui/game/staff_effect/sa/sa.mse"
	FN_GA_MARK						= "d:/ymir work/ui/game/staff_effect/ga/ga.mse"
	FN_TGM_MARK						= "d:/ymir work/ui/game/staff_effect/tgm/tgm.mse"

FN_GM_MARK							= "d:/ymir work/ui/game/staff_effect/gm/gm.mse"

MAP_TREE2 							= 'MAP_TREE2'

ERROR_MARK_UPLOAD_NEED_RECONNECT	= 'UploadMark: Reconnect to game'
ERROR_MARK_CHECK_NEED_RECONNECT		= 'CheckMark: Reconnect to game'

VIRTUAL_KEY_ALPHABET_LOWERS			= r"[1234567890]/qwertyuiop\=asdfghjkl;`'zxcvbnm.,"
VIRTUAL_KEY_ALPHABET_UPPERS			= r"{1234567890}?QWERTYUIOP|+ASDFGHJKL:~'ZXCVBNM<>"
VIRTUAL_KEY_SYMBOLS    				= "!@#$%^&*()_+|{}:'<>?~"
VIRTUAL_KEY_NUMBERS					= "1234567890-=\[];',./`"
VIRTUAL_KEY_SYMBOLS_BR				= "!@#$%^&*()_+|{}:'<>?~áàãâéèêíìóòôõúùç"



__IS_ENGLISH = "ENGLISH" == app.GetLocaleServiceName()
__IS_HONGKONG = "HONGKONG" == app.GetLocaleServiceName()
__IS_NEWCIBN	= "locale/country/newcibn" == app.GetLocalePath()
__IS_EUROPE = "EUROPE" == app.GetLocaleServiceName()
__IS_CANADA		= "locale/country/ca" == app.GetLocalePath()
__IS_BRAZIL		= "locale/country/br" == app.GetLocalePath()
__IS_SINGAPORE	= "locale/country/sg" == app.GetLocalePath()
__IS_VIETNAM	= "locale/country/vn" == app.GetLocalePath()
__IS_ARABIC		= "locale/country/ae" == app.GetLocalePath()
__IS_CIBN10		= "locale/country/cibn10" == app.GetLocalePath()
__IS_WE_KOREA	= "locale/country/we_korea" == app.GetLocalePath()
__IS_TAIWAN		= "locale/country/taiwan" == app.GetLocalePath()
__IS_JAPAN		= "locale/country/japan" == app.GetLocalePath()
LOGIN_FAILURE_WRONG_SOCIALID = "ASDF"
LOGIN_FAILURE_SHUTDOWN_TIME = "ASDF"

if __IS_CANADA:
	__IS_EUROPE = True

def IsYMIR():
	return "locale/country/ymir" == app.GetLocalePath()

def IsJAPAN():
	return "locale/country/japan" == app.GetLocalePath()

def IsENGLISH():
	global __IS_ENGLISH
	return __IS_ENGLISH

def IsHONGKONG():
	global __IS_HONGKONG
	return __IS_HONGKONG

def IsTAIWAN():
	return "locale/country/taiwan" == app.GetLocalePath()

def IsNEWCIBN():
	return "locale/country/newcibn" == app.GetLocalePath()

def IsCIBN10():
	global __IS_CIBN10
	return __IS_CIBN10

def IsEUROPE():
	global __IS_EUROPE
	return __IS_EUROPE

def IsCANADA():
	global __IS_CANADA
	return __IS_CANADA

def IsBRAZIL():
	global __IS_BRAZIL
	return __IS_BRAZIL

def IsVIETNAM():
	global __IS_VIETNAM
	return __IS_VIETNAM

def IsSINGAPORE():
	global __IS_SINGAPORE
	return __IS_SINGAPORE

def IsARABIC():
	global __IS_ARABIC
	return __IS_ARABIC

def IsWE_KOREA():
	return "locale/country/we_korea" == app.GetLocalePath()

# SUPPORT_NEW_KOREA_SERVER
def LoadLocaleData():
	app.LoadLocaleData(app.GetLocalePath())

def IsCHEONMA():
	return IsYMIR()		# ÀÌÁ¦ YMIR ·ÎÄÉÀÏÀº ¹«Á¶°Ç Ãµ¸¶¼­¹öÀÓ. Ãµ¸¶¼­¹ö°¡ ¹®À» ´Ý±â Àü±îÁö º¯ÇÒ ÀÏ ¾øÀ½.

# END_OF_SUPPORT_NEW_KOREA_SERVER

def mapping(**kwargs): return kwargs

def SNA(text):
	def f(x):
		return text
	return f

def SA(text):
	def f(x):
		return text % x
	return f

## PET_SKILL_INFO FNCS BEGIN
def SAA(text):
	def f(x1, x2):
		return text % (x1, x2)
	return f

def SAF(text):
	def f(x1, x2, x3, x4):
		return text % (x1, x2, x3, x4)
	return f

def SAN(text):
	def f(x1, x2):
		return text % x1
	return f

# if app.ENABLE_GROWTH_PET_SYSTEM:
def SAAAA(text):
	def f(x1, x2, x3, x4):
		return text % (x1, x2, x3, x4)
	return f
## PET_SKILL_INFO FNCS END

def LoadLocaleFile(srcFileName, localeDict, optional=False):
	def SNA(text):
		def f(x):
			return text
		return f

	def SA(text):
		def f(x):
			return text % x
		return f

	def SAN(text):
		def f(x):
			return text % x
		return f

	def SAA(text):
		def f(x):
			return text % x
		return f

	funcDict = {"SA": SA, "SNA": SNA, "SAA": SAA, "SAN": SAN}
	lineIndex = 1

	try:
		lines = pack_open(srcFileName, "r").readlines()
	except IOError:
		if optional:
			return False
		dbg.LogBox("LoadLocaleError(%(srcFileName)s)" % locals())
		app.Abort()

	for line in lines:
		if line.count("\t") == 0:
			continue

		try:
			tokens = line[:-1].split("\t")
			if app.ENABLE_MULTI_TEXTLINE:
				for k in range(1, len(tokens)):
					tokens[k] = tokens[k].replace("\\n", "\n")
			if len(tokens) == 2:
				localeDict[tokens[0]] = tokens[1]
			elif len(tokens) >= 3:
				type = tokens[2].strip()
				if type:
					if type in funcDict:
						localeDict[tokens[0]] = funcDict[type](tokens[1])
					else:
						localeDict[tokens[0]] = tokens[1]
				else:
					localeDict[tokens[0]] = tokens[1]
			else:
				raise RuntimeError, "Unknown TokenSize"

			lineIndex += 1
		except:
			dbg.LogBox("%s: line(%d): %s" % (srcFileName, lineIndex, line), "Error")
			raise
	return True

LOCALE_FILE_NAME = "%s/locale_game.txt" % app.GetLocalePath()
LoadLocaleFile(LOCALE_FILE_NAME, locals())

# Additional UI strings live in the locale pack. Languages without their own
# extension use the Turkish pack values until translated.
NEW_LOCALE_GAME_FILE_NAME = "%s/new_locale_game.txt" % app.GetLocalePath()
if not LoadLocaleFile(NEW_LOCALE_GAME_FILE_NAME, locals(), optional=True):
	LoadLocaleFile("locale/tr/new_locale_game.txt", locals())

# Baglanti / giris metinleri (locale dosyasinda yoksa varsayilan; hata kullaniciya gosterilsin)
try:
	LOGIN_CONNETING
except NameError:
	LOGIN_CONNETING = "Lutfen bekle. Sunucuya baglaniyorsun."
try:
	LOGIN_PROCESSING
except NameError:
	LOGIN_PROCESSING = "Lutfen bekle. Sunucuya baglaniyorsun."
try:
	LOGIN_CONNECT_SUCCESS
except NameError:
	LOGIN_CONNECT_SUCCESS = "Baglanti basarili."
try:
	LOGIN_CLIENT_LOADING
except NameError:
	LOGIN_CLIENT_LOADING = "Yukleniyor..."
LOGIN_CONNECT_TIMEOUT = "Baglanti zaman asimina ugradi. Sunucuya ulasilamiyor veya yanit vermiyor. Adres ve portu kontrol edin."
LOGIN_CONNECT_ERROR = "Sunucuya baglanilamadi. Ag baglantisini veya sunucu adresini kontrol edin."
try:
	TOOLTIP_NEXT_SKILL_LEVEL_3
except NameError:
	TOOLTIP_NEXT_SKILL_LEVEL_3 = "Sonraki Seviye: %d"

if app.ENABLE_CANNOT_MINING_IF_POLY:
	try:
		MINING_FAIL_ON_MOUNT
	except NameError:
		MINING_FAIL_ON_MOUNT = "At veya binek uzerindeyken madencilik yapamazsin."
	try:
		MINING_FAIL_WITHOUT_PICK
	except NameError:
		MINING_FAIL_WITHOUT_PICK = "Elinde kazma olmadan madencilik yapamazsin."
	try:
		MINING_FAIL_WHEN_POLY
	except NameError:
		MINING_FAIL_WHEN_POLY = "Donusmus haldeyken madencilik yapamazsin."

try:
	CANNOT_FISHING_RIDE_MOUNT
except NameError:
	CANNOT_FISHING_RIDE_MOUNT = "Binek hayvanin uzerindeyken balikcilik yapamazsin."

if app.ENABLE_MSG_FISHING_WHILE_POLY:
	try:
		CANNOT_FISHING_WHILE_POLY
	except NameError:
		CANNOT_FISHING_WHILE_POLY = "Donusmus halde balik tutamazsin."

if app.ENABLE_MSG_DRAGON_SOUL_WHILE_ATTACK:
	try:
		CANNOT_MOVE_DRAGON_SOUL_WHILE_ATTACK
	except NameError:
		CANNOT_MOVE_DRAGON_SOUL_WHILE_ATTACK = "Hareket halindeyken bunu yapamazsin."

if app.ENABLE_MESSAGE_CANNOT_UNEQUIP_IN_BATTLE:
	try:
		CANNOT_UNEQUIP_IN_BATTLE
	except NameError:
		CANNOT_UNEQUIP_IN_BATTLE = "Esyayi cikarmak icin hareketsiz olmalisin."

if app.ENABLE_CLIENT_VERSION_RENEWAL:
	try:
		CHAT_MESSAGE_CLIENT_VERSION_NOT_MATCH
	except NameError:
		CHAT_MESSAGE_CLIENT_VERSION_NOT_MATCH = "Istemci surumun eslesmedigi icin oturumun kapatildi. Lutfen surumunu guncelle ve tekrar oturum ac."

# Option pvp messages
OPTION_PVPMODE_MESSAGE_DICT = {
	0: PVP_MODE_NORMAL,
	1: PVP_MODE_REVENGE,
	2: PVP_MODE_KILL,
	3: PVP_MODE_PROTECT,
	4: PVP_MODE_GUILD,
}

# Whisper messages
WHISPER_ERROR = {
	1: CANNOT_WHISPER_NOT_LOGON,
	2: CANNOT_WHISPER_DEST_REFUSE,
	3: CANNOT_WHISPER_SELF_REFUSE,
}

# Exception of graphic device.
error = dict(
	CREATE_WINDOW = GAME_INIT_ERROR_MAIN_WINDOW,
	CREATE_CURSOR = GAME_INIT_ERROR_CURSOR,
	CREATE_NETWORK = GAME_INIT_ERROR_NETWORK,
	CREATE_ITEM_PROTO = GAME_INIT_ERROR_ITEM_PROTO,
	CREATE_MOB_PROTO = GAME_INIT_ERROR_MOB_PROTO,
	CREATE_NO_DIRECTX = GAME_INIT_ERROR_DIRECTX,
	CREATE_DEVICE = GAME_INIT_ERROR_GRAPHICS_NOT_EXIST,
	CREATE_NO_APPROPRIATE_DEVICE = GAME_INIT_ERROR_GRAPHICS_BAD_PERFORMANCE,
	CREATE_FORMAT = GAME_INIT_ERROR_GRAPHICS_NOT_SUPPORT_32BIT,
	NO_ERROR = str()
)

# Guild war description
GUILDWAR_NORMAL_DESCLIST = 					(GUILD_WAR_USE_NORMAL_MAP, GUILD_WAR_LIMIT_30MIN, GUILD_WAR_WIN_CHECK_SCORE)
# Guild war warp description
GUILDWAR_WARP_DESCLIST = 					(GUILD_WAR_USE_BATTLE_MAP, GUILD_WAR_WIN_WIPE_OUT_GUILD, GUILD_WAR_REWARD_POTION)
# Guild war flag description
GUILDWAR_CTF_DESCLIST = 					(GUILD_WAR_USE_BATTLE_MAP, GUILD_WAR_WIN_TAKE_AWAY_FLAG1, GUILD_WAR_WIN_TAKE_AWAY_FLAG2, GUILD_WAR_REWARD_POTION)

# Mode of pvp options
MODE_NAME_LIST = 							(PVP_OPTION_NORMAL, PVP_OPTION_REVENGE, PVP_OPTION_KILL, PVP_OPTION_PROTECT)
# Title name of alignment
TITLE_NAME_LIST =							(PVP_LEVEL0, PVP_LEVEL1, PVP_LEVEL2, PVP_LEVEL3, PVP_LEVEL4, PVP_LEVEL5, PVP_LEVEL6, PVP_LEVEL7, PVP_LEVEL8)

# Notify messages
NOTIFY_MESSAGE = {
	'CANNOT_EQUIP_SHOP': 					CANNOT_EQUIP_IN_SHOP,
	'CANNOT_EQUIP_EXCHANGE': 				CANNOT_EQUIP_IN_EXCHANGE,
}

# Attack messages
ATTACK_ERROR_TAIL_DICT = {
	'IN_SAFE': 								CANNOT_ATTACK_SELF_IN_SAFE,
	'DEST_IN_SAFE': 						CANNOT_ATTACK_DEST_IN_SAFE,
}

# Shot messages
SHOT_ERROR_TAIL_DICT = {
	'EMPTY_ARROW': 							CANNOT_SHOOT_EMPTY_ARROW,
	'IN_SAFE':								CANNOT_SHOOT_SELF_IN_SAFE,
	'DEST_IN_SAFE':							CANNOT_SHOOT_DEST_IN_SAFE,
}

# Use-skill messages
USE_SKILL_ERROR_TAIL_DICT = {	
	'IN_SAFE':								CANNOT_SKILL_SELF_IN_SAFE,
	'NEED_TARGET': 							CANNOT_SKILL_NEED_TARGET,
	'NEED_EMPTY_BOTTLE': 					CANNOT_SKILL_NEED_EMPTY_BOTTLE,
	'NEED_POISON_BOTTLE': 					CANNOT_SKILL_NEED_POISON_BOTTLE,
	'REMOVE_FISHING_ROD': 					CANNOT_SKILL_REMOVE_FISHING_ROD,
	'NOT_YET_LEARN': 						CANNOT_SKILL_NOT_YET_LEARN,
	'NOT_MATCHABLE_WEAPON':					CANNOT_SKILL_NOT_MATCHABLE_WEAPON,
	'WAIT_COOLTIME':						CANNOT_SKILL_WAIT_COOLTIME,
	'NOT_ENOUGH_HP':						CANNOT_SKILL_NOT_ENOUGH_HP,
	'NOT_ENOUGH_SP':						CANNOT_SKILL_NOT_ENOUGH_SP,
	'CANNOT_USE_SELF':						CANNOT_SKILL_USE_SELF,
	'ONLY_FOR_ALLIANCE': 					CANNOT_SKILL_ONLY_FOR_ALLIANCE,
	'CANNOT_ATTACK_ENEMY_IN_SAFE_AREA':		CANNOT_SKILL_DEST_IN_SAFE,
	'CANNOT_APPROACH':						CANNOT_SKILL_APPROACH,
	'CANNOT_ATTACK':						CANNOT_SKILL_ATTACK,
	'ONLY_FOR_CORPSE':						CANNOT_SKILL_ONLY_FOR_CORPSE,
	'EQUIP_FISHING_ROD':					CANNOT_SKILL_EQUIP_FISHING_ROD, 
	'NOT_HORSE_SKILL':						CANNOT_SKILL_NOT_HORSE_SKILL,
	'HAVE_TO_RIDE':							CANNOT_SKILL_HAVE_TO_RIDE,
}

if app.ENABLE_STANDING_MOUNT:
	try:
		CANNOT_SKILL_NOT_STANDING_HORSE_SKILL
	except NameError:
		CANNOT_SKILL_NOT_STANDING_HORSE_SKILL = "Bu beceriyi bu binek uzerinde kullanamazsin."
	USE_SKILL_ERROR_TAIL_DICT.update({
		"NOT_HORSE_STANDING_SKILL" : CANNOT_SKILL_NOT_STANDING_HORSE_SKILL,
	})

# HORSE_LEVEL4 varsayýlan deðeri (locale dosyasýndan yüklenmemiþse)
try:
	HORSE_LEVEL4
except NameError:
	HORSE_LEVEL4 = 'HORSE_LEVEL4'

if app.ENABLE_RIDING_EXTENDED:
	LEVEL_LIST = ["", HORSE_LEVEL1, HORSE_LEVEL2, HORSE_LEVEL3, HORSE_LEVEL4]
else:
	LEVEL_LIST = ["", HORSE_LEVEL1, HORSE_LEVEL2, HORSE_LEVEL3]

HEALTH_LIST=[
	HORSE_HEALTH0,
	HORSE_HEALTH1,
	HORSE_HEALTH2,
	HORSE_HEALTH3,
]


# Skill messages
USE_SKILL_ERROR_CHAT_DICT = {
	'NEED_EMPTY_BOTTLE': 					SKILL_NEED_EMPTY_BOTTLE,
	'NEED_POISON_BOTTLE': 					SKILL_NEED_POISON_BOTTLE,
	'ONLY_FOR_GUILD_WAR': 					SKILL_ONLY_FOR_GUILD_WAR,
}

# Shop/private-shop messages
SHOP_ERROR_DICT = {
	'NOT_ENOUGH_MONEY': 					SHOP_NOT_ENOUGH_MONEY,
	'SOLDOUT': 								SHOP_SOLDOUT,
	'INVENTORY_FULL': 						SHOP_INVENTORY_FULL,
	'INVALID_POS': 							SHOP_INVALID_POS,
	'NOT_ENOUGH_COINS': 					SHOP_NOT_ENOUGH_COINS,
}

if app.ENABLE_RENEWAL_SHOPEX:
	SHOP_ERROR_DICT["NOT_ENOUGH_EXP"] =		SHOP_NOT_ENOUGH_EXP
	SHOP_ERROR_DICT["NOT_ENOUGH_ITEM"] =	SHOP_NOT_ENOUGH_ITEM
	SHOP_ERROR_DICT["ALREADY_HAVE_ONE_ITEM"] =	SHOP_ALREADY_HAVE_ONE_ITEM

# Character status description
STAT_MINUS_DESCRIPTION = {
	'HTH-': 								STAT_MINUS_CON,
	'INT-': 								STAT_MINUS_INT,
	'STR-': 								STAT_MINUS_STR,
	'DEX-': 								STAT_MINUS_DEX,
}

# Map names
MINIMAP_ZONE_NAME_DICT = {
	'metin2_map_a1': 						MAP_A1,
	'map_a2': 								MAP_A2,
	'metin2_map_a3': 						MAP_A3,
	'metin2_map_b1': 						MAP_B1,
	'map_b2': 								MAP_B2,
	'metin2_map_b3': 						MAP_B3,
	'metin2_map_c1': 						MAP_C1,
	'map_c2': 								MAP_C2,
	'metin2_map_c3': 						MAP_C3,
	'map_n_snowm_01': 						MAP_SNOW,
	'metin2_map_n_flame_01': 				MAP_FLAME,
	'metin2_map_n_desert_01': 				MAP_DESERT,
	'metin2_map_milgyo': 					MAP_TEMPLE,
	'metin2_map_spiderdungeon': 			MAP_SPIDER,
	'metin2_map_deviltower1': 				MAP_SKELTOWER,
	'metin2_map_guild_01': 					MAP_AG,
	'metin2_map_guild_02': 					MAP_BG,
	'metin2_map_guild_03': 					MAP_CG,
	'metin2_map_trent': 					MAP_TREE,
	'metin2_map_trent02': 					MAP_TREE2,
	'metin2_map_WL_01': 					MAP_WL,
	'metin2_map_nusluck01': 				MAP_NUSLUCK,
    'Metin2_map_CapeDragonHead': 			MAP_CAPE,
    'metin2_map_Mt_Thunder': 				MAP_THUNDER,
    'metin2_map_dawnmistwood': 				MAP_DAWN,
    'metin2_map_BayBlackSand': 				MAP_BAY,
}

# Path of quest icon file
def GetLetterImageName():
	return "icon/scroll_close.tga"

def GetLetterOpenImageName():
	return "icon/scroll_open.tga"

def GetLetterCloseImageName():
	return "icon/scroll_close.tga"

# Sell item question
def DO_YOU_SELL_ITEM(sellItemName, sellItemCount, sellItemPrice):
	return DO_YOU_SELL_ITEM2 % (sellItemName, sellItemCount, NumberToMoneyString(sellItemPrice)) if (sellItemCount > 1) else DO_YOU_SELL_ITEM1 % (sellItemName, NumberToMoneyString(sellItemPrice))

# Buy item question
def DO_YOU_BUY_ITEM(buyItemName, buyItemCount, buyItemPrice):
	return DO_YOU_BUY_ITEM2 % (buyItemName, buyItemCount, buyItemPrice) if (buyItemCount > 1) else DO_YOU_BUY_ITEM1 % (buyItemName, buyItemPrice)
		
# Notify when you can't attach a specific item.
def REFINE_FAILURE_CAN_NOT_ATTACH(attachedItemName):
	return REFINE_FAILURE_CAN_NOT_ATTACH0 % (attachedItemName)

def REFINE_FAILURE_NO_SOCKET(attachedItemName):
	return REFINE_FAILURE_NO_SOCKET0 % (attachedItemName)

# Drop item question
def REFINE_FAILURE_NO_GOLD_SOCKET(attachedItemName):
	return REFINE_FAILURE_NO_GOLD_SOCKET0 % (attachedItemName)
	
# Drop item question
def HOW_MANY_ITEM_DO_YOU_DROP(dropItemName, dropItemCount):
	return HOW_MANY_ITEM_DO_YOU_DROP2 % (dropItemName, dropItemCount) if (dropItemCount > 1) else HOW_MANY_ITEM_DO_YOU_DROP1 % (dropItemName)

# Fishing notify when looks like the fish is hooked.
def FISHING_NOTIFY(isFish, fishName):
	return FISHING_NOTIFY1 % (fishName) if isFish else FISHING_NOTIFY2 % (fishName)

# Fishing notify when you capture a fish.
def FISHING_SUCCESS(isFish, fishName):
	return FISHING_SUCCESS1 % (fishName) if isFish else FISHING_SUCCESS2 % (fishName)
	
# Convert a integer amount into a string and add . as separator for money.
def NumberToMoneyString(n):
	return '0 {:s}'.format(MONETARY_UNIT0) if (n <= 0) else '{:s} {:s}'.format('.'.join([(i - 3) < 0 and str(n)[:i] or str(n)[i - 3 : i] for i in range(len(str(n)) % 3, len(str(n)) + 1, 3) if i]), MONETARY_UNIT0)

# Convert a integer amount into a string and add . as separator for secondary coin.
def NumberToSecondaryCoinString(n):
	return '0 {:s}'.format(MONETARY_UNIT_JUN) if (n <= 0) else '{:s} {:s}'.format('.'.join([(i - 3) < 0 and str(n)[:i] or str(n)[i - 3: i] for i in range(len(str(n)) % 3, len(str(n)) + 1, 3) if i]), MONETARY_UNIT_JUN)

def NumberToMoneyStringNEW(n) :
	if n <= 0 :
		return "0"
	return "%s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))

def MoneyFormat(n):
	return "%s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ])) 

# Convert a integer amount into a string and add . as separator with decimal.
def NumberToDecimalString(n):
	return str('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))

# Convert a integer amount into a string and add . as separator for experience.
if app.ENABLE_RENEWAL_SHOPEX:
	def NumberToShopEXP(n):
		return '0 {:s}'.format(MONETARY_UNIT_EXP) if (n <= 0) else '{:s} {:s}'.format('.'.join([(i - 3) < 0 and str(n)[:i] or str(n)[i - 3: i] for i in range(len(str(n)) % 3, len(str(n)) + 1, 3) if i]), MONETARY_UNIT_EXP)

# Return the title of alignment by points.
def GetAlignmentTitleName(alignment):
	if alignment >= 12000:
		return TITLE_NAME_LIST[0]
	elif alignment >= 8000:
		return TITLE_NAME_LIST[1]
	elif alignment >= 4000:
		return TITLE_NAME_LIST[2]
	elif alignment >= 1000:
		return TITLE_NAME_LIST[3]
	elif alignment >= 0:
		return TITLE_NAME_LIST[4]
	elif alignment > -4000:
		return TITLE_NAME_LIST[5]
	elif alignment > -8000:
		return TITLE_NAME_LIST[6]
	elif alignment > -12000:
		return TITLE_NAME_LIST[7]

	return TITLE_NAME_LIST[8]

def NumberWithPoint(n) :
	if n <= 0 :
		return "0"

	return "%s" % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))

def NumberToMoneyString2(n) :
	if long(n) <= 0 :
		return "0"

	return "%s " % ('.'.join([ i-3<0 and str(n)[:i] or str(n)[i-3:i] for i in range(len(str(n))%3, len(str(n))+1, 3) if i ]))

if app.ENABLE_SUNG_MAHI_TOWER:
	import constInfo
	def LoadSungMahiInfo():
		if len(constInfo.sungMahiRewardInfo) > 0 or len(constInfo.sungMahiElementInfo) > 0:
			return
		
		loadFiles = ["sungmahee_tower_reward", "sungmahee_tower_element"]
		fileIndex = 1
		
		for file in loadFiles:
			lineIndex = 1
			
			# Try different possible paths
			possiblePaths = [
				"{}/common/sungmahee_tower/standard/{}.txt".format(app.GetLocalePath(), file),
				"common/sungmahee_tower/standard/{}.txt".format(file),
				"locale/common/sungmahee_tower/standard/{}.txt".format(file),
				"{}/sungmahee_tower/standard/{}.txt".format(app.GetLocalePath(), file),
			]
			
			lines = None
			filePath = None
			for path in possiblePaths:
				try:
					if pack.Exist(path):
						filePath = path
						lines = pack_open(path, "r").readlines()
						break
				except:
					pass
			
			if lines is None:
				dbg.TraceError("LoadSungMahiInfo read error file({}). Tried paths: {}".format(file, ", ".join(possiblePaths)))
				# Dosya bulunamazsa boþ liste ile devam et, abort etme
				if fileIndex == 1:
					constInfo.sungMahiRewardInfo = []
				else:
					constInfo.sungMahiElementInfo = []
				fileIndex += 1
				continue
			
			for line in lines:
				try:
					if not line:
						lineIndex += 1
						continue
					
					#skip first line
					if lineIndex == 1:
						lineIndex += 1
						continue
					
					tokens = line[:-1].split("\t")
					if fileIndex == 1:
						tempListInfo = []
						tempTokenInfo = []
						
						#set level info
						tempTokenInfo.append(tokens[0])
						for tokenInfo in xrange(len(tokens)):
							if (tokenInfo < 1):
								continue
							
							if len(tempListInfo) < 2:
								tempListInfo.append(tokens[tokenInfo])
								
							if len(tempListInfo) > 1:
								tempTokenInfo.append(tempListInfo)
								tempListInfo= []
						
						constInfo.sungMahiRewardInfo.append(tempTokenInfo)
					else:
						constInfo.sungMahiElementInfo.append(tokens)
					
					lineIndex += 1
				except:
					raise
					
			fileIndex += 1

	LoadSungMahiInfo()

	def SecondToM(time):
		if time < 60:
			return "1" + MINUTE

		minute = int((time / 60) % 60)
		text = ""

		if minute > 0:
			text += str(minute) + MINUTE

		return text

	def SecondToMS(time):
		if time < 60:
			return "%d%s" % (time, SECOND)

		second = int(time % 60)
		minute = int((time / 60) % 60)

		text = ""

		if minute > 0:
			text += str(minute) + MINUTE
			if minute > 0:
				text += " "

		if second > 0:
			text += str(second) + SECOND

		return text

def GetMiniMapZoneNameByIdx(idx):
	if idx in MINIMAP_ZONE_NAME_DICT_BY_IDX and idx != 0:
		return MINIMAP_ZONE_NAME_DICT_BY_IDX[idx]
	return MAP_NONE

MINIMAP_ZONE_NAME_DICT_BY_IDX = {
	0 : "",
	1 : MAP_A1,
	3 : MAP_A3,
	4 : MAP_GUILD_01,
	5 : MAP_MONKEY_DUNGEON_11,
	6 : GUILD_VILLAGE_01,
	21 : MAP_B1,
	23 : MAP_B3,
	24 : MAP_GUILD_02,
	25 : MAP_MONKEY_DUNGEON_12,
	26 : GUILD_VILLAGE_02,
	41 : MAP_C1,
	43 : MAP_C3,
	44 : MAP_GUILD_03,
	45 : MAP_MONKEY_DUNGEON_13,
	46 : GUILD_VILLAGE_03,
	61 : MAP_SNOW,
	62 : MAP_N_FLAME_01,
	63 : MAP_DESERT,
	64 : MAP_A2,
	65 : MAP_TEMPLE,
	66 : MAP_DEVILTOWER1,
	67 : MAP_TRENT,
	68 : MAP_TRENT02,
	69 : MAP_WL,
	70 : MAP_NUSLUCK,
	71 : MAP_SPIDERDUNGEON_02,
	72 : MAP_SKIPIA_DUNGEON_01,
	73 : MAP_SKIPIA_DUNGEON_02,
	81 : MAP_WEDDING_01,
	103 : MAP_T1,
	104 : MAP_SPIDERDUNGEON,
	105 : MAP_T2,
	107 : MAP_MONKEY_DUNGEON,
	108 : MAP_MONKEY_DUNGEON2,
	109 : MAP_MONKEY_DUNGEON3,
	110 : MAP_T3,
	111 : MAP_T4,
	112 : MAP_DUEL,
	113 : MAP_OXEVENT,
	181 : MAP_EMPIREWAR01,
	182 : MAP_EMPIREWAR02,
	183 : MAP_EMPIREWAR03,
	208 : MAP_SKIPIA_DUNGEON_BOSS,
	216 : MAP_DEVILCATACOMB,
	217 : MAP_SPIDERDUNGEON_03,
	301 : MAP_CAPE,
	302 : MAP_DAWN,
	303 : MAP_BAY,
	304 : MAP_THUNDER,
	351 : MAP_N_FLAME_DUNGEON_01,
	352 : MAP_N_SNOW_DUNGEON_01,
	355 : DEFENSAWE_HYDRA,
}

if app.ENABLE_GROWTH_PET_SYSTEM:
	def SecondToDay(time):
		if time < 60:
			return "1" + DAY

		second = int(time % 60)
		minute = int((time / 60) % 60)
		hour = int((time / 60) / 60) % 24
		day = int(int((time / 60) / 60) / 24)

		if day < 1:
			day = 1

		if day > 9999:
			day = 9999

		text = str(day) + DAY
		return text

if app.ENABLE_GROWTH_PET_SYSTEM:
	def SecondToDayNumber(time):
		if time < 60:
			return 1

		second = int(time % 60)
		minute = int((time / 60) % 60)
		hour = int((time / 60) / 60) % 24
		day = int(int((time / 60) / 60) / 24)

		if day < 1:
			day = 1

		if day > 9999:
			day = 9999

		return day

if app.ENABLE_GROWTH_PET_SYSTEM:
	def SecondToH(time):
		hour = int((time / 60) / 60)

		text = ""
		hour = max( 0, hour )
		text += str(hour)

		return text

# Convert seconds to Days-Hours-Minutes
def SecondToDHM(time):
	if time < 60:
		return '0' + MINUTE

	minute = int((time / 60) % 60)
	hour = int((time / 60) / 60) % 24
	day = int(int((time / 60) / 60) / 24)

	text = ''
	if day > 0:
		text += str(day) + DAY
		text += ' '

	if hour > 0:
		text += str(hour) + HOUR
		text += ' '

	if minute > 0:
		text += str(minute) + MINUTE
	return text

# Convert seconds to Hours-Minutes
def SecondToHM(time):
	if time < 60:
		return '0' + MINUTE

	minute = int((time / 60) % 60)
	hour = int((time / 60) / 60)

	text = ''
	if hour > 0:
		text += str(hour) + HOUR
		if hour > 0:
			text += ' '

	if minute > 0:
		text += str(minute) + MINUTE
	return text

def SecondToMS(time):
	if time < 60:
		return "%d%s" % (time, SECOND)

	second = int(time % 60)
	minute = int((time / 60) % 60)

	text = ""

	if minute > 0:
		text += str(minute) + MINUTE
		if minute > 0:
			text += " "

	if second > 0:
		text += str(second) + SECOND

	return text

def SecondToHMS(time):
	second = int(time % 60)
	minute = int((time / 60) % 60)
	hour = int((time / 60) / 60)

	text = ""

	if hour > 0:
		text+=str(hour) + HOUR
		if hour > 0:
			text += " "

	if minute > 0:
		text+=str(minute) + MINUTE

	if second > 0:
		text += str(second) + SECOND

	return text

import localeInfo

def RTSecondToDHMS(time):
	text = ""
	g = time // (24 * 3600)
	time %= (24 * 3600)

	s = time // 3600
	time %= 3600

	d = time // 60
	time %= 60

	sn = time

	if g:
		text += "%d%s " % (g, localeInfo.DAY)

	if text or s:
		text += "%d%s " % (s, localeInfo.HOUR)

	if text or d:
		text += "%d%s " % (d, localeInfo.MINUTE)

	if text or sn:
		text += "%d%s " % (sn, localeInfo.SECOND)

	return text[:-1]

if app.ENABLE_EVENT_SYSTEM:
	def FormatTime(time):
		second = int(time % 60)
		minute = int((time / 60) % 60)
		hour = int((time / 60) / 60) % 24
		return "%02d Saat %02d Dakika %02d Saniye" % (hour, minute, second)

# Cevher bilgisi iin locale tanm
POSSIBLE_ORE = "Cevher Türü:"
if app.ENABLE_AE_GUILD_MARK_NAME_ALIGNMENT:
	def GetTextWidth(text):
		return app.GetTextWidth(text)

	def GetTextLineByWidth(text, width):
		return app.GetTextLineByWidth(text, width)

if "CATEGORY_DRAGON_STONE_DEMON" not in locals():
	CATEGORY_DRAGON_STONE_DEMON = "Ametist"

if "ITEM_TOOLTIP_DESC_GEM_BAG_DEFAULT" not in locals():
	ITEM_TOOLTIP_DESC_GEM_BAG_DEFAULT = "Kullandiktan sonra 9.999 Gaya'ya kadar doldurabilirsin. | 30 gun sonra kaybolurlar.\nGaya kesesinin ticareti yapilabilir."
if "ITEM_TOOLTIP_DESC_GEM_BAG_HAVE_GEM" not in locals():
	ITEM_TOOLTIP_DESC_GEM_BAG_HAVE_GEM = "Gaya kesesi (%d Gaya, ticareti yapilabilir). \nKeseyi acarsan, %d Gaya alirsin."
if "ITEM_TOOLTIP_DESC_GEM_BAG_REWARD" not in locals():
	ITEM_TOOLTIP_DESC_GEM_BAG_REWARD = "Kazanan takimi alkisladigin icin odul olarak bir Gaya Spor Cantasi alirsin.\nTicareti yapilamaz."
if "ITEM_TOOLTIP_DESC_GEM_BAG_REWARD_HAVE_GEM" not in locals():
	ITEM_TOOLTIP_DESC_GEM_BAG_REWARD_HAVE_GEM = "Gaya spor cantasi (%d Gaya, ticareti yok). \nSpor cantasini acarsan, %d Gaya alirsin."
if "REFINE_FAILURE_GEM_SELECT_WINDOW_OPENED" not in locals():
	REFINE_FAILURE_GEM_SELECT_WINDOW_OPENED = "Bu pencere acikken zihin taslarini kullanamazsin."
if "SELECT_METINSTONE_NOT_ENOUGH_INVENTORY_SPACE" not in locals():
	SELECT_METINSTONE_NOT_ENOUGH_INVENTORY_SPACE = "Envanterde yeterli bosluk yok."

if "CANT_USE_MESSAGE_WHEN_EQUIP" not in locals():
	CANT_USE_MESSAGE_WHEN_EQUIP = "Giyiliyken bu nesneyi kullanamazsin."
if "AURA_CLEAR_QUESTION_WHEN_USE" not in locals():
	AURA_CLEAR_QUESTION_WHEN_USE = "Aura giysisindeki aktarilan tum degerleri sifirlamak istiyor musun? Bu islem geri alinamaz."
if "AURA_CLEAR_CANNOT_USE_SEALED_ITEM" not in locals():
	AURA_CLEAR_CANNOT_USE_SEALED_ITEM = "Ruh bagli nesnede aktarilan degerleri degistiremezsin."


# --- ADVANCED GAME OPTIONS LOCALE DEFAULTS ---
_adv_opt_defs = {
	"GAME_OPTION_ANTI_ALIASING_TITLE": "Kenar Yumusatma (Anti-Aliasing)",
	"GAME_OPTION_ANTI_ALIASING_DESCRIPTION": "Kenar yumusatma efektini acar veya kapatir.",
	"GRAPHICS_SHADOW_TITLE": "Golge Kalitesi",
	"GRAPHICS_SHADOW_OFF": "Kapali",
	"GRAPHICS_SHADOW_LOW": "Dusuk",
	"GRAPHICS_SHADOW_MEDIUM": "Orta",
	"GRAPHICS_SHADOW_HIGH": "Yuksek",
	"GRAPHICS_SHADOW_CASTERS": "Golge Veren Nesneler",
	"GRAPHICS_SHADOW_CASTERS_DESC": "Oyuncu, arazi ve agac golgelerini ayarlar.",
	"GRAPHICS_SHADOW_CASTERS_PLAYERS": "Oyuncular",
	"GRAPHICS_SHADOW_CASTERS_TERRAIN": "Arazi",
	"GRAPHICS_SHADOW_CASTERS_TREES": "Agaclar",
	"GAME_OPTION_SUN_RAYS": "Gunes Isinlari",
	"GAME_OPTION_SUN_RAYS_DESC": "Gunes isigi huzmeleri efektini ayarlar.",
	"GAME_OPTION_WET_GROUND": "Islak Zemin",
	"GAME_OPTION_WET_GROUND_DESC": "Yagmurlu havalarda zemin islaklik efektini ayarlar.",
	"GAME_OPTION_RAIN": "Yagmur",
	"GAME_OPTION_RAIN_DESC": "Yagmur yagis efektini acar veya kapatir.",
	"GAME_OPTION_STORM": "Firtina",
	"GAME_OPTION_STORM_DESC": "Firtina efektini acar veya kapatir.",
	"GAME_OPTION_SHOOTING_STARS": "Kayan Yildizlar",
	"GAME_OPTION_SHOOTING_STARS_DESC": "Gece modunda kayan yildiz efektini ayarlar.",
	"GAME_OPTION_WATER_RIPPLE": "Su Dalgaciklari",
	"GAME_OPTION_WATER_RIPPLE_DESC": "Su yuzeyindeki dalgalanma efektini ayarlar.",
	"GAME_OPTION_ENHANCED_WATER": "Gelismis Su",
	"GAME_OPTION_ENHANCED_WATER_DESC": "Gercekci su efektlerini acar veya kapatir.",
	"GAME_OPTION_UNDERWATER": "Su Alti Efekti",
	"GAME_OPTION_UNDERWATER_DESC": "Su altina girildiginde bulaniklik efektini ayarlar.",
	"GAME_OPTION_SPECULAR_REFLECTIONS": "Yansima Parlamasi",
	"GAME_OPTION_SPECULAR_REFLECTIONS_DESC": "Parlak yuzeylerdeki isik yansimalarini ayarlar.",
	"GAME_OPTION_WATER_REFLECTIONS": "Su Yansimalari",
	"GAME_OPTION_WATER_REFLECTIONS_DESC": "Su yuzeyine cevre yansimalarini uygular.",
	"GAME_OPTION_LOCAL_LIGHT": "Yerel Isiklandirma",
	"GAME_OPTION_LOCAL_LIGHT_DESC": "Yakindaki isik kaynaklarinin aydinlatmasini ayarlar.",
	"GAME_OPTION_SHARPNESS": "Netlik (Sharpness)",
	"GAME_OPTION_SHARPNESS_DESC": "Oyun goruntusunun keskinligini ayarlar.",
	"GAME_OPTION_DEATH_CAM": "Olum Kamerasi",
	"GAME_OPTION_DEATH_CAM_DESC": "Karakter oldugunde kamera efektini ayarlar.",
	"GAME_OPTION_SPEEDTREE_FX": "Agac Efektleri",
	"GAME_OPTION_SPEEDTREE_FX_DESC": "Agaclarin ruzgar ve yaprak dokme efektlerini ayarlar.",
	"GAME_OPTION_SPEEDTREE_DYNAMIC_LIGHT": "Dinamik Isik",
	"GAME_OPTION_SPEEDTREE_WIND": "Ruzgar Efekti",
	"GAME_OPTION_SPEEDTREE_ENHANCED_FOLIAGE": "Gelismis Yapraklar",
	"GAME_OPTION_SPEEDTREE_FALLING_LEAVES": "Dusen Yapraklar",
	"GAME_OPTION_FOV": "Kamera Acisi (FOV)",
	"GAME_OPTION_FOV_DESC": "Kameranin gorus acisini genisletir veya daraltir.",
	"GAME_OPTION_CAMERA_FAR": "Kamera Mesafesi",
	"GAME_OPTION_CAMERA_FAR_DESC": "Kameranin uzaklasabilecegi maksimum mesafeyi ayarlar.",
	"GAME_OPTION_KEYBOARD_CAMERA_SPEED": "Klavye Kamera Hizi",
	"GAME_OPTION_KEYBOARD_CAMERA_SPEED_DESC": "Klavyeden kamera dondurme hizini ayarlar.",
	"GAME_OPTION_SKYBOX": "Gokyuzu Temasi",
	"GAME_OPTION_SKYBOX_DESC": "Gokyuzu temasini ve hava durumunu degistirir.",
	"GAME_OPTION_NIGHT_MODE": "Gece Modu",
	"GAME_OPTION_NIGHT_MODE_DESC": "Oyunu gece moduna gecirir.",
	"GAME_OPTION_REMOVE_FOG": "Sisi Kaldir",
	"GAME_OPTION_REMOVE_FOG_DESC": "Haritadaki gorus engelleyen sisi kaldirir.",
	"GAME_OPTION_ENVIRONMENT_DARKNESS_TITLE": "Cevre Karanligi",
	"GAME_OPTION_ENVIRONMENT_SATURATION_TITLE": "Cevre Doygunlugu",
	"GAME_OPTION_ENVIRONMENT_CONTRAST_TITLE": "Cevre Kontrasti",
	"GAME_OPTION_ENVIRONMENT_TEMPERATURE_TITLE": "Renk Sicakligi",
	"GAME_OPTION_ENVIRONMENT_COLOR_TITLE": "Cevre Renk Ayari",
	"GAME_OPTION_ENVIRONMENT_COLOR_OPEN": "Renk Ayarla",
	"GAME_OPTION_ENVIRONMENT_SONG": "Cevre Muzigi",
	"GAME_OPTION_VOLUME_ENVIRONMENT": "Cevre Sesi",
	"GAME_OPTION_CROWD_PERFORMANCE_TITLE": "Kalabalik Modu",
	"GAME_OPTION_CROWD_PERFORMANCE_MODE": "Yuksek Performans Modu",
	"GAME_OPTION_CROWD_PERFORMANCE_DESC": "Kalabalik pazar veya savaslarda performansi artirir.",
	"GAME_OPTION_FPS_LIMIT_TITLE": "FPS Siniri",
	"GAME_OPTION_FPS_LIMIT_LABEL": "Maksimum Kare Hizi",
	"GAME_OPTION_FPS_VALUE_60": "60 FPS",
	"GAME_OPTION_FPS_VALUE_120": "120 FPS",
	"GAME_OPTION_FPS_VALUE_144": "144 FPS",
	"GAME_OPTION_FPS_VALUE_200": "200 FPS",
	"GAME_OPTION_HIDE_FPS_MINIMAP": "Minimap FPS Gostergesini Gizle",
	"GAME_OPTION_HIDE_FPS_MINIMAP_DESC": "Harita altindaki FPS gostergesini kapatir.",
	"GAME_OPTION_BUFFI_SETTINGS": "Saman / Buffi Ayarlari",
	"GAME_OPTION_AUTO_UNEQUIP_BUFFI_PVP": "PvP'de Buffi Cikar",
	"GAME_OPTION_AUTO_UNEQUIP_BUFFI_PVP_DESC": "PvP basladiginda buffi esyalarini otomatik cikarir.",
	"GAME_OPTION_AUTO_EQUIP_BUFFI_PVM": "PvM'de Buffi Tak",
	"GAME_OPTION_AUTO_EQUIP_BUFFI_PVM_DESC": "Yaratik keserken buffi esyalarini otomatik takar.",
	"IKASHOP_PRO_SHOP_RANGE_VIEW_HEADER": "Pazar Gorus Mesafesi",
	"GAME_OPTION_ENTITY_NAME": "Varlik Isimleri",
	"GAME_OPTION_ENTITY_NAME_DESC": "Oyuncu, mob ve NPC isimlerinin gorunumunu ayarlar.",
	"GAME_OPTION_NAME_FAR": "Uzak Isimler",
	"GAME_OPTION_NAMES_MONSTERS_FOV": "Yaratik Isimleri FOV",
	"GAME_OPTION_SHOW_COUNTRY_FLAG": "Ulke Bayraklarini Goster",
	"GAME_OPTION_DMG": "Hasar Gostergeleri",
	"GAME_OPTION_DMG_DESC": "Vurulan hasar yazilarinin boyutunu ve araligini ayarlar.",
	"GAME_OPTION_DMG_SIZE": "Hasar Yazi Boyutu",
	"GAME_OPTION_DAMAGE_SPACING": "Hasar Yazi Boslugu",
	"GAME_OPTION_DAMAGE_ON": "Hasarlari Goster",
	"GAME_OPTION_DMG_MONSTER": "Yaratik Hasarlarini Goster",
	"GAME_OPTION_DMG_MONSTER_DESC": "Yaratiklara vurulan hasar rakamlarini gosterir.",
	"GAME_OPTION_COLORED_DAMAGE": "Renkli Hasar",
	"GAME_OPTION_COLORED_DAMAGE_DESC": "Kritik ve delici hasarlari farkli renklerde gosterir.",
	"GAME_OPTION_DAMAGE_COLORS_OPEN": "Hasar Renkleri",
	"GAME_OPTION_STONE_SCALE": "Metin Tasi Boyutu",
	"GAME_OPTION_STONE_SCALE_DESC": "Metin taslarinin fiziksel boyutunu ayarlar.",
	"GAME_OPTION_HIDE_COMPANION": "Yoldaslari Gizle",
	"GAME_OPTION_HIDE_COMPANION_DESC": "Binek, pet ve buffi modellerini gizleyerek kasma yapmasini onler.",
	"GAME_OPTION_HIDE_MOUNT": "Binekleri Gizle",
	"GAME_OPTION_HIDE_PET": "Petleri Gizle",
	"GAME_OPTION_HIDE_BUFFI": "Buffi Gizle",
	"GAME_OPTION_QUEUE_METIN": "Sirali Metin Efekti",
	"GAME_OPTION_QUEUE_METIN_COLOR": "Metin Efekt Rengi",
	"GAME_OPTION_QUEUE_METIN_FLASH": "Metin Vurus Parlamasi",
	"GAME_OPTION_MINING_FLASH": "Madencilik Parlamasi",
	"GAME_OPTION_MINING_FLASH_DESC": "Madencilik bittiginde ekranin parlamasini saglar.",
	"GAME_OPTION_HIDE_EFFECTS": "Efektleri Gizle",
	"GAME_OPTION_HIDE_EFFECTS_DESC": "Hasar disindaki gereksiz efektleri kapatir.",
	"GAME_OPTION_HIDE_EFFECTS_EXCEPT_DAMAGE": "Sadece Hasari Goster",
	"GAME_OPTION_HIDE_EFFECTS_FROM_OTHERS": "Diger Oyuncularin Efektlerini Gizle",
	"GAME_OPTION_HIDE_SHAMAN_BUFF": "Saman Kutsamalarini Gizle",
	"GAME_OPTION_HIDE_STONE_EFFECT": "Tas Efektlerini Gizle",
	"GAME_OPTION_HIDE_MONSTER_EFFECT": "Yaratik Efektlerini Gizle",
	"GAME_OPTION_HIDE_EXP_EFFECT": "Exp Efektini Gizle",
	"GAME_OPTION_SHOW_GUILD_GRADE": "Lonca Rutbelerini Goster",
	"GAME_OPTION_SHOW_GUILD_JOIN_NOTIFICATION": "Loncaya Katilim Bildirimi",
	"GAME_OPTION_NOTIFICATION_HISTORY": "Bildirim Gecmisi Butonu",
	"GAME_OPTION_AUTO_CAPE_AFTER_WARP": "Isinlanma Sonrasi Oto Pelerin",
	"GAME_OPTION_AUTO_CAPE_AFTER_WARP_DESC": "Isinlandiktan sonra otomatik cesaret pelerini kullanir.",
	"GAME_OPTION_DISABLE_LEVEL_UP_CONFIRMATION": "Level Atlama Onayini Kapat",
	"GAME_OPTION_DISABLE_LEVEL_UP_CONFIRMATION_DESC": "Seviye atlandiginda cikan onay penceresini kapatir.",
	"GAME_OPTION_HIDE_METIN_SPAWNS": "Metin Dogma Efektini Gizle",
	"GAME_OPTION_AUTO_HIDE_COMPANION_MAP1": "1. Koyde Yoldaslari Oto Gizle",
	"GAME_OPTION_AUTO_HIDE_COMPANION_MAP1_DESC": "1. Koy pazarlarinda pet ve binekleri otomatik gizler.",
	"GAME_OPTION_DROP_INTERFACE": "Dusen Esya Arayuzu",
	"GAME_OPTION_DROP_INTERFACE_DESC": "Dusen esyalari ekranda gosteren arayuzu ayarlar.",
	"GAME_OPTION_GUIDELINE_BTN": "Rehber Butonu",
	"GAME_OPTION_GUIDELINE_BTN_DESC": "Yardim ve rehber butonunu ekranda gosterir.",
	"GAME_OPTION_SYSTEM_MENU": "Sistem Menusu Butonu",
	"GAME_OPTION_SYSTEM_MENU_DESC": "Mini harita yanindaki sistem menusunu gosterir.",
	"GAME_OPTION_EVENT_ICON": "Etkinlik Simgesi",
	"GAME_OPTION_EVENT_ICON_DESC": "Aktif etkinlik simgelerini ekranda gosterir.",
	"GAME_OPTION_WINDOW_SCALE": "Arayuz Boyutlandirma (Scale)",
	"GAME_OPTION_WINDOW_SCALE_DESC": "Kullanici arayuzunu ekran cozunurlugune gore olceklendirir.",
	"GAME_OPTION_FONT": "Yazi Tipi (Font)",
	"GAME_OPTION_BIG_FONT_SIZE": "Buyuk Yazi Tipi",
	"GAME_OPTION_BIG_SLOT_COUNT": "Buyuk Slot Sayisi",
	"GAME_OPTION_SAVE_STATUS": "Pencere Konumlarini Kaydet",
	"GAME_OPTION_SAVE_STATUS_DESC": "Pencereleri kapattiginiz yerleri hatirlar.",
	"GAME_OPTION_SMALL_AFFECT_ICON": "Kucuk Efekt Simgeleri",
	"GAME_OPTION_SMALL_AFFECT_ICON_DESC": "Sol ustteki sebnem ve etki simgelerini kucultur.",
	"GAME_OPTION_GUILD_QUEST": "Lonca Gorev Penceresi",
	"GAME_OPTION_WHISPER_SENDER_LETTER": "Fisilti Gonderen Mektubu",
	"GAME_OPTION_GUILD_CHAT_TO_WHISPER": "Lonca Sohbetini Fisiltiya Yansit",
	"GAME_OPTION_SMALL_WHISPER_WINDOW": "Kucuk Fisilti Penceresi",
	"GAME_OPTION_ENVIRONMENT_TITLE": "Cevre Ayarlari",
	"GAME_OPTION_EVENT_STONE_APPEARANCE": "Etkinlik Metin Gorunumu",
	"GAME_OPTION_EVENT_STONE_APPEARANCE_DESC": "Etkinlik metinlerinin ozel gorsellerini ayarlar.",
	"GAME_OPTION_AUTO_HIDE_COMPANION_MAP1_TITLE": "1. Koy Yoldas Gizleme",
	"GAME_OPTION_EVENT_ICON_SUBTITLE": "Etkinlik Simgeleri",
	"GAME_OPTION_EVENT_STONE_ORIGINAL": "Orijinal Model",
	"GAME_OPTION_GUILD_CHAT_WHISPER": "Lonca Sohbeti",
	"GAME_OPTION_GUILD_CHAT_WHISPER_DESC": "Lonca mesajlarini fisiltida gosterir.",
	"GAME_OPTION_GUILD_GRADE": "Lonca Rutbesi",
	"GAME_OPTION_GUILD_GRADE_DESC": "Lonca uyelerinin rutbelerini gosterir.",
	"GAME_OPTION_GUILD_JOIN_NOTIFICATION": "Loncaya Katilim Bildirimi",
	"GAME_OPTION_GUILD_JOIN_NOTIFICATION_DESC": "Loncaya biri katildiginda bildirim verir.",
	"GAME_OPTION_GUILD_QUEST_WINDOW": "Lonca Gorev Penceresi",
	"GAME_OPTION_GUILD_QUEST_WINDOW_DESC": "Lonca gorev penceresini gosterir.",
	"GAME_OPTION_HIDE_COMPANION_BUFFI": "Buffi Gizle",
	"GAME_OPTION_HIDE_COMPANION_MOUNT": "Binek Gizle",
	"GAME_OPTION_HIDE_COMPANION_PET": "Pet Gizle",
	"GAME_OPTION_HIDE_EFFECTS_ALL": "Tum Efektleri Gizle",
	"GAME_OPTION_HIDE_EFFECTS_EXP": "Exp Efekti",
	"GAME_OPTION_HIDE_EFFECTS_MONSTER": "Yaratik Efekti",
	"GAME_OPTION_HIDE_EFFECTS_OTHERS": "Diger Oyuncular",
	"GAME_OPTION_HIDE_EFFECTS_SHAMAN_BUFF": "Saman Kutsamasi",
	"GAME_OPTION_HIDE_EFFECTS_STONE": "Tas Efekti",
	"GAME_OPTION_HIDE_METIN_SPAWNS_DESC": "Metin tasi dogma efektini gizler.",
	"GAME_OPTION_HIDE_METIN_SPAWNS_TITLE": "Metin Dogma Efekti",
	"GAME_OPTION_MINING_FLASH_OFF": "Kapali",
	"GAME_OPTION_MINING_FLASH_ON": "Acik",
	"GAME_OPTION_MONSTER_NAME": "Yaratik Isimleri",
	"GAME_OPTION_MONSTER_NAME_DESC": "Yaratik isimlerinin gorunumunu ayarlar.",
	"GAME_OPTION_QUEUE_METIN_COLOR_OPEN": "Renk Sec",
	"GAME_OPTION_QUEUE_METIN_EFFECT": "Sirali Metin",
	"GAME_OPTION_QUEUE_METIN_EFFECT_OPTION": "Efekt Secenegi",
	"GAME_OPTION_QUEUE_METIN_FLASH_OFF": "Kapali",
	"GAME_OPTION_QUEUE_METIN_FLASH_ON": "Acik",
	"GAME_OPTION_SAVE_STATUS_WINDOW": "Pencere Konumu",
	"GAME_OPTION_SAVE_STATUS_WINDOW_DESC": "Pencerelerin konumunu kaydeder.",
	"GAME_OPTION_SHARPNESS_TITLE": "Netlik",
	"GAME_OPTION_SIZE_DMG": "Hasar Boyutu",
	"GAME_OPTION_SIZE_STONE": "Metin Boyutu",
	"GAME_OPTION_SPEEDTREE_ENHANCED_FOLIAGE_DESC": "Agac yapraklarinin detay seviyesi.",
	"GAME_OPTION_SPEEDTREE_FALLING_LEAVES_DESC": "Agaclardan dokulen yaprak efekti.",
	"GAME_OPTION_SPEEDTREE_LIGHTING": "Isiklandirma",
	"GAME_OPTION_SPEEDTREE_TITLE": "Agac Motoru (SpeedTree)",
	"GAME_OPTION_SYSTEM_MENU_SUBTITLE": "Sistem Menusu",
	"GAME_OPTION_WHISPER_SENDER_LETTERS": "Fisilti Mektubu",
	"GAME_OPTION_WHISPER_SENDER_LETTERS_DESC": "Fisilti geldiginde mektup simgesi gosterir.",
	"GAME_OPTION_WHISPER_TITLE": "Fisilti Ayarlari",
	"GRAPHICS_ENHANCED_WATER_DESCRIPTION": "Gelismis su efektlerini ayarlar.",
	"GRAPHICS_ENHANCED_WATER_TITLE": "Gelismis Su",
	"GRAPHICS_LOCAL_LIGHT_DESCRIPTION": "Yerel isiklandirma efektlerini ayarlar.",
	"GRAPHICS_LOCAL_LIGHT_TITLE": "Yerel Isik",
	"GRAPHICS_RAIN_DESCRIPTION": "Yagmur yagis efektini ayarlar.",
	"GRAPHICS_RAIN_TITLE": "Yagmur",
	"GRAPHICS_SHADOW_CHARACTERS": "Karakterler",
	"GRAPHICS_SHADOW_SCENERY": "Manzara",
	"GRAPHICS_SHADOW_TREES": "Agaclar",
	"GRAPHICS_SHOOTING_STARS_DESCRIPTION": "Kayan yildiz efektini ayarlar.",
	"GRAPHICS_SHOOTING_STARS_TITLE": "Kayan Yildizlar",
	"GRAPHICS_SKYBOX_TITLE": "Gokyuzu",
	"GRAPHICS_SKY_NIGHT": "Gece",
	"GRAPHICS_SPECULAR_REFLECTION_DESCRIPTION": "Parlak nesnelerdeki yansimalari ayarlar.",
	"GRAPHICS_SPECULAR_REFLECTION_TITLE": "Isik Yansimasi",
	"GRAPHICS_STORM_DESCRIPTION": "Firtina efektini ayarlar.",
	"GRAPHICS_STORM_TITLE": "Firtina",
	"GRAPHICS_SUN_RAYS_DESCRIPTION": "Gunes isigi huzmelerini ayarlar.",
	"GRAPHICS_SUN_RAYS_TITLE": "Gunes Isinlari",
	"GRAPHICS_TOGGLE_HINT": "Ac / Kapat",
	"GRAPHICS_UNDERWATER_DESCRIPTION": "Su alti gorus efektini ayarlar.",
	"GRAPHICS_UNDERWATER_TITLE": "Su Alti Efekti",
	"GRAPHICS_WATER_REFLECTION_DESCRIPTION": "Su yuzeyindeki cevre yansimalarini ayarlar.",
	"GRAPHICS_WATER_REFLECTION_TITLE": "Su Yansimasi",
	"GRAPHICS_WATER_RIPPLE_DESCRIPTION": "Su dalgalanmasi efektini ayarlar.",
	"GRAPHICS_WATER_RIPPLE_TITLE": "Su Dalgaciklari",
	"GRAPHICS_WET_GROUND_DESCRIPTION": "Islak zemin efektini ayarlar.",
	"GRAPHICS_WET_GROUND_TITLE": "Islak Zemin"
}
for _k, _v in _adv_opt_defs.items():
	if _k not in locals():
		locals()[_k] = _v

# ---- Wiki varsayilanlari (wikicontent/uiwiki/wikipages): locale_game.txt'de yoksa EN, TR client'ta TR ----
_WIKI_DEFAULTS = (
	("WIKI_TITLE", "Wikipedia", "Wikipedi"),
	("WIKI_HOME", "Home", "Home"),
	("WIKI_DUNGEON_TITLE", "Dungeons", "Zindanlar"),
	("DUNGEON_NAMES_MONKEY", "Monkey Dungeon", "Maymun Zindan\xfd"),
	("DUNGEON_NAMES_SERPENT", "Serpent Temple", "Y\xfdlan Tap\xfdna\xf0\xfd"),
	("DUNGEON_NAMES_SPIDER", "Spider Dungeon", "\xd6r\xfcmcek Zindan\xfd"),
	("DUNGEON_NAMES_DEVILTOWER", "Devil's Tower", "\xdeeytan Kulesi"),
	("DUNGEON_NAMES_CATACOMBS", "Devil's Catacomb", "\xdeeytan Yeralt\xfd Mezar\xfd"),
	("DUNGEON_NAMES_SETAOU", "Setaou Ground", "Setaou Arazisi"),
	("DUNGEON_NAMES_EARTH", "Earth Dungeon", "Toprak Zindan\xfd"),
	("DUNGEON_NAMES_RAZADOR", "Razador Shrine", "Razador Tap\xfdna\xf0\xfd"),
	("DUNGEON_NAMES_NEMERE", "Nemere's Watchtower", "Nemere'nin G\xf6zetleme Kulesi"),
	("DUNGEON_NAMES_OCHAO", "Ochao Temple", "Ochao Tap\xfdna\xf0\xfd"),
	("MAP_JOTUN_THRYM", "Jotun Thrym", "Jotun Thrym"),
	("DUNGEON_NAMES_MELEY", "Meley's Lair", "Meley'in Yuvas\xfd"),
	("WIKI_ENIGMA", "Enigma", "Enigma"),
	("WIKI_SYSTEM_CHAR_SYSTEMS", "Character Systems", "Karakter Sistemleri"),
	("WIKI_SYSTEM_GENERAL_SYSTEMS", "General Systems", "Genel Sistemler"),
	("WIKI_SYSTEM_DRAGON_ALCHEMY", "Dragon Soul Alchemy", "Ejderha Ta\xfe\xfd Simyas\xfd"),
	("WIKI_SYSTEM_COSTUMES", "Costumes", "Kost\xfcmler"),
	("WIKI_SYSTEM_MOUNTS", "Mounts", "Binekler"),
	("WIKI_SYSTEM_PETS", "Pets", "Evcil Hayvanlar"),
	("WIKI_SYSTEM_BIOLOGIST_QUEST", "Biologist Quest", "Biyolog G\xf6revi"),
	("PASSIVE_SKILL_TITLE", "Passive Skills", "Pasif Beceriler"),
	("SASH_SKIN_TITLE", "Sash Skins", "Ku\xfeak G\xf6r\xfcn\xfcmleri"),
	("WIKI_BUFFI_SYSTEM", "Buffi System", "Buffi Sistemi"),
	("WIKI_67_BONUS_SYSTEM", "6th & 7th Bonus", "6. ve 7. Bonus"),
	("WIKI_AURA_PVP", "Aura (PvP)", "Aura (PvP)"),
	("WIKI_PET_ORB", "Pet Orb", "Pet K\xfcresi"),
	("WIKI_BOUNS_INFO", "Bonus Info", "Bonus Bilgisi"),
	("WIKI_ENERGY_CRYSTAL", "Energy Crystal", "Enerji Kristali"),
	("WIKI_SYSTEM_GUILDS", "Guilds", "Loncalar"),
	("CASUAL_SYSTEM_TILE", "Casual Systems", "G\xfcnl\xfck Sistemler"),
	("Exploration", "Exploration", "Ke\xfeif"),
	("Shrines", "Shrines", "Tap\xfdnaklar"),
	("Fishing", "Fishing", "Bal\xfdk Tutma"),
	("Stone Exchange", "Stone Exchange", "Ta\xfe De\xf0i\xfeimi"),
	("WIKI_SYSTEM_MOKOKO", "Mokoko", "Mokoko"),
	("WIKI_MINING", "Mining", "Madencilik"),
	("WIKI_CHAMPION_ARENA", "Champion Arena", "\xdeampiyon Arenas\xfd"),
	("WIKI_DAILY_EVENTS", "Daily Events", "G\xfcnl\xfck Etkinlikler"),
	("RANK_BY_GUILD_DUNGEON", "Guild Dungeon Ranking", "Lonca Zindan\xfd S\xfdralamas\xfd"),
	("WIKI_ZONE_CONTROL", "Zone Control", "B\xf6lge Kontrol\xfc"),
	("Torgny Trial", "Torgny Trial", "Torgny S\xfdnav\xfd"),
	("Karuak Ceremony", "Karuak Ceremony", "Karuak T\xf6reni"),
	("WIKI_LEGENDARY", "Legendary", "Efsanevi"),
	("WIKI_OCTAVIO_FESTIVAL", "Octavio Festival", "Octavio Festivali"),
	("WIKI_BEGINNER_EVENT", "Beginner Event", "Yeni Ba\xfelayanlar Etkinli\xf0i"),
	("Guide Start", "Starter Guide", "Ba\xfelang\xfd\xe7 Rehberi"),
	("WIKI_GUIDE_55_75", "Level 55-75 Guide", "55-75 Seviye Rehberi"),
	("WIKI_GUIDETRY", "Beginner Tips", "Ba\xfelang\xfd\xe7 \xddpu\xe7lar\xfd"),
	("WIKI_GUIDEFAQ", "FAQ", "S\xfdk Sorulan Sorular"),
	("WIKI_EVENTS", "Current Events", "Mevcut Etkinlikler"),
	("WIKI_GUIDES", "Guides", "Rehberler"),
	("WIKI_CRAFT_INCREASE", "Increase", "Art\xfd\xfe"),
	("WIKI_CRAFT_MATERIALS", "Materials", "Malzemeler"),
	("WIKI_CRAFT_RESULT", "Result", "Sonu\xe7"),
	("WIKI_MOUNT_ENERGY", "Energy", "Enerji"),
	("WIKI_MOUNT_RUNES", "Runes", "R\xfcnler"),
	("WIKI_NO_DATA", "-", "-"),
	("WIKI_PET_EVOLUTION", "Evolution", "Evrim"),
	("WIKI_SHOP_PRICE", "Price", "Fiyat"),
	("WIKI_STONES_ALSO_AVAILABLE", "Also available:", "Ayr\xfdca bulunur:"),
)

def _ApplyWikiDefaults():
	_g = globals()
	try:
		_lang = app.GetLocaleName()
	except:
		_lang = ""
	_isTr = (_lang or "").lower() in ("tr", "turkey", "turkish")
	for _row in _WIKI_DEFAULTS:
		if not _g.has_key(_row[0]):
			if _isTr:
				_g[_row[0]] = _row[2]
			else:
				_g[_row[0]] = _row[1]

_ApplyWikiDefaults()
