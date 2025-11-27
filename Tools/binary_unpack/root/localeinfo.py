if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
net = __import__(pyapi.GetModuleName("net"))

import constInfo
import dbg

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



__IS_NEWCIBN	= "locale/country/newcibn" == app.GetLocalePath()
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

def LoadLocaleFile(srcFileName, localeDict):
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

LOCALE_FILE_NAME = "%s/locale_game.txt" % app.GetLocalePath()
LoadLocaleFile(LOCALE_FILE_NAME, locals())

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
# Title name of title system
if app.ENABLE_TITLE_SYSTEM:
	TITLE_SYSTEM_LIST = 					(TITLE_SYSTEM_NAME_0, TITLE_SYSTEM_NAME_1, TITLE_SYSTEM_NAME_2, TITLE_SYSTEM_NAME_3, TITLE_SYSTEM_NAME_4, TITLE_SYSTEM_NAME_5, TITLE_SYSTEM_NAME_6, TITLE_SYSTEM_NAME_7, TITLE_SYSTEM_NAME_8, TITLE_SYSTEM_NAME_9, TITLE_SYSTEM_NAME_10, TITLE_SYSTEM_NAME_11, TITLE_SYSTEM_NAME_12, TITLE_SYSTEM_NAME_13, TITLE_SYSTEM_NAME_14, TITLE_SYSTEM_NAME_15, TITLE_SYSTEM_NAME_16, TITLE_SYSTEM_NAME_17, TITLE_SYSTEM_NAME_18, TITLE_SYSTEM_NAME_19, TITLE_SYSTEM_NAME_20)

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

def RTSecondToDHMS(time):
	text = ""

	d = time // (24 * 3600)
	time = time % (24 * 3600)
	h = time // 3600
	time %= 3600
	m = time // 60
	time %= 60
	s = time

	if d:
		text += "%dd " % d
	if text or h:
		text += "%dh " % h
	if text or m:
		text += "%dm " % m
	if text or s:
		text += "%ds " % s

	return text[:-1]

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

	def SecondToH(time):
		hour = int((time / 60) / 60)

		text = ""
		hour = max( 0, hour )
		text += str(hour)

		return text

# Cevher bilgisi için locale tanýmý
POSSIBLE_ORE = "Kullanýlabilir Cevher:"