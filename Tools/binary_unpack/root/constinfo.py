if __USE_DYNAMIC_MODULE__:
	import pyapi

app = __import__(pyapi.GetModuleName("app"))
chrmgr = __import__(pyapi.GetModuleName("chrmgr"))
player = __import__(pyapi.GetModuleName("player"))
net = __import__(pyapi.GetModuleName("net"))

import item

ALREADY_NOTIFY_LIST = []

# Taskbar buttons
ENABLE_EXPANDED_TASKBAR = 0

_game_instance = None
_interface_instance = None

def GetGameInstance():
	global _game_instance
	return _game_instance

def SetGameInstance(instance):
	global _game_instance

	if _game_instance:
		del _game_instance

	_game_instance = instance

def GetInterfaceInstance():
	global _interface_instance
	return _interface_instance

def SetInterfaceInstance(instance):
	global _interface_instance

	if _interface_instance:
		del _interface_instance

	_interface_instance = instance


if app.__AUTO_HUNT__:
	autoHuntAutoLoginDict = {
		"status" : 0,
		"leftTime" : 0,
		"id" : "",
		"pwd" : "",
		"addr" : "",
		"port" : 0,
		"account_addr" : "",
		"account_port" : 0,
		"slot" : -1,
		"newOptions" : None,
		"slotStatus" : {},
		"skillDict" : None,
		"is_mounting" : 0,  # Bineðe binme durumu (0 = binmiyor, 1 = biniyor)
	}
INPUT_IGNORE = 0
SELECT_CHARACTER_ROTATION = 1
IN_GAME_SHOP_ENABLE = 1
# option
CONSOLE_ENABLE = 0

PVPMODE_ENABLE = 1
PVPMODE_TEST_ENABLE = 0
PVPMODE_ACCELKEY_ENABLE = 1
PVPMODE_ACCELKEY_DELAY = 0.5
PVPMODE_PROTECTED_LEVEL = 15

FOG_LEVEL0 = 4800.0
FOG_LEVEL1 = 9600.0
FOG_LEVEL2 = 12800.0
FOG_LEVEL = FOG_LEVEL0
FOG_LEVEL_LIST = [FOG_LEVEL0, FOG_LEVEL1, FOG_LEVEL2]

CAMERA_MAX_DISTANCE_SHORT = 2500.0
CAMERA_MAX_DISTANCE_LONG = 3500.0
CAMERA_MAX_DISTANCE_LIST=[CAMERA_MAX_DISTANCE_SHORT, CAMERA_MAX_DISTANCE_LONG]
CAMERA_MAX_DISTANCE = CAMERA_MAX_DISTANCE_SHORT

CHRNAME_COLOR_INDEX = 0

ENVIRONMENT_NIGHT = "d:/ymir work/environment/moonlight04.msenv"

HIGH_PRICE = 500000
MIDDLE_PRICE = 50000
ERROR_METIN_STONE = 28960
EXPANDED_COMBO_ENABLE = 1
CONVERT_EMPIRE_LANGUAGE_ENABLE = 1
USE_ITEM_WEAPON_TABLE_ATTACK_BONUS = 0
ADD_DEF_BONUS_ENABLE = 0
LOGIN_COUNT_LIMIT_ENABLE = 0

USE_SKILL_EFFECT_UPGRADE_ENABLE = 1

VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = 1
GUILD_MONEY_PER_GSP = 100
GUILD_WAR_TYPE_SELECT_ENABLE = 1
TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE = 10

HAIR_COLOR_ENABLE = 1
ARMOR_SPECULAR_ENABLE = 1
WEAPON_SPECULAR_ENABLE = 1
KEEP_ACCOUNT_CONNETION_ENABLE = 1
MINIMAP_POSITIONINFO_ENABLE = 0

isItemQuestionDialog = 0

def GET_ITEM_QUESTION_DIALOG_STATUS():
	global isItemQuestionDialog
	return isItemQuestionDialog

def SET_ITEM_QUESTION_DIALOG_STATUS(flag):
	global isItemQuestionDialog
	isItemQuestionDialog = flag

########################

def SET_DEFAULT_FOG_LEVEL():
	global FOG_LEVEL
	app.SetMinFog(FOG_LEVEL)

def SET_FOG_LEVEL_INDEX(index):
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	try:
		FOG_LEVEL=FOG_LEVEL_LIST[index]
	except IndexError:
		FOG_LEVEL=FOG_LEVEL_LIST[0]
	app.SetMinFog(FOG_LEVEL)

def GET_FOG_LEVEL_INDEX():
	global FOG_LEVEL
	global FOG_LEVEL_LIST
	return FOG_LEVEL_LIST.index(FOG_LEVEL)

########################

def SET_DEFAULT_CAMERA_MAX_DISTANCE():
	global CAMERA_MAX_DISTANCE
	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def SET_CAMERA_MAX_DISTANCE_INDEX(index):
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	try:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[index]
	except:
		CAMERA_MAX_DISTANCE=CAMERA_MAX_DISTANCE_LIST[0]

	app.SetCameraMaxDistance(CAMERA_MAX_DISTANCE)

def GET_CAMERA_MAX_DISTANCE_INDEX():
	global CAMERA_MAX_DISTANCE
	global CAMERA_MAX_DISTANCE_LIST
	return CAMERA_MAX_DISTANCE_LIST.index(CAMERA_MAX_DISTANCE)

########################

def SET_DEFAULT_CHRNAME_COLOR():
	global CHRNAME_COLOR_INDEX
	chrmgr.SetEmpireNameMode(CHRNAME_COLOR_INDEX)

def SET_CHRNAME_COLOR_INDEX(index):
	global CHRNAME_COLOR_INDEX
	CHRNAME_COLOR_INDEX=index
	chrmgr.SetEmpireNameMode(index)

def GET_CHRNAME_COLOR_INDEX():
	global CHRNAME_COLOR_INDEX
	return CHRNAME_COLOR_INDEX

def SET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD(index):
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD = index

def GET_VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD():
	global VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD
	return VIEW_OTHER_EMPIRE_PLAYER_TARGET_BOARD

def SET_DEFAULT_CONVERT_EMPIRE_LANGUAGE_ENABLE():
	global CONVERT_EMPIRE_LANGUAGE_ENABLE
	net.SetEmpireLanguageMode(CONVERT_EMPIRE_LANGUAGE_ENABLE)

def SET_DEFAULT_USE_ITEM_WEAPON_TABLE_ATTACK_BONUS():
	global USE_ITEM_WEAPON_TABLE_ATTACK_BONUS
	player.SetWeaponAttackBonusFlag(USE_ITEM_WEAPON_TABLE_ATTACK_BONUS)

def SET_DEFAULT_USE_SKILL_EFFECT_ENABLE():
	global USE_SKILL_EFFECT_UPGRADE_ENABLE
	app.SetSkillEffectUpgradeEnable(USE_SKILL_EFFECT_UPGRADE_ENABLE)

def SET_TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE():
	global TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE
	app.SetTwoHandedWeaponAttSpeedDecreaseValue(TWO_HANDED_WEAPON_ATT_SPEED_DECREASE_VALUE)

import item

ACCESSORY_MATERIAL_LIST = [50623, 50624, 50625, 50626, 50627, 50628, 50629, 50630, 50631, 50632, 50633, 50634, 50635, 50636, 50637, 50638]

# Cevher socket sayýsý - cevher_bilgisi.txt'ye göre deðiþtirilebilir
ACCESSORY_SOCKET_MAX_SIZE = 3

JewelAccessoryInfos = [
		# jewel		wrist	neck	ear
		[ 50634,	14420,	16220,	17220 ],
		[ 50635,	14500,	16500,	17500 ],
		[ 50636,	14520,	16520,	17520 ],
		[ 50637,	14540,	16540,	17540 ],
		[ 50638,	14560,	16560,	17560 ],
	]

def GET_ACCESSORY_MATERIAL_VNUM(vnum, subType):
	ret = vnum
	item_base = (vnum / 10) * 10
	for info in JewelAccessoryInfos:
		if item.ARMOR_WRIST == subType:
			if info[1] == item_base:
				return info[0]
		elif item.ARMOR_NECK == subType:
			if info[2] == item_base:
				return info[0]
		elif item.ARMOR_EAR == subType:
			if info[3] == item_base:
				return info[0]

	if vnum >= 16210 and vnum <= 16219:
		return 50625

	if item.ARMOR_WRIST == subType:
		WRIST_ITEM_VNUM_BASE = 14000
		ret -= WRIST_ITEM_VNUM_BASE
	elif item.ARMOR_NECK == subType:
		NECK_ITEM_VNUM_BASE = 16000
		ret -= NECK_ITEM_VNUM_BASE
	elif item.ARMOR_EAR == subType:
		EAR_ITEM_VNUM_BASE = 17000
		ret -= EAR_ITEM_VNUM_BASE

	type = ret/20

	if type<0 or type>=len(ACCESSORY_MATERIAL_LIST):
		type = (ret-170) / 20
		if type<0 or type>=len(ACCESSORY_MATERIAL_LIST):
			return 0

	return ACCESSORY_MATERIAL_LIST[type]

##################################################################
## The newly added'belt' item type and the item to be plugged into the belt socket...
## Since the belt socket system is the same as the accessory, it has to be done in this way like the hard coding related to the accessory above.

def GET_BELT_MATERIAL_VNUM(vnum, subType = 0):
	# Currently, only one item (#18900) can be inserted on all belts.
	return 18900

##################################################################
## Automatic Potion (HP: #72723 ~ #72726, SP: #72727 ~ #72730)

# Is the vnum an automatic potion?
def IS_AUTO_POTION(itemVnum):
	return IS_AUTO_POTION_HP(itemVnum) or IS_AUTO_POTION_SP(itemVnum)

def IS_AUTO_POTION_HP(itemVnum):
	if 72723 <= itemVnum and 72726 >= itemVnum:
		return 1
	elif itemVnum >= 76021 and itemVnum <= 76022:		## New Gift Fire Dragon's Blessing
		return 1
	elif itemVnum == 79012:
		return 1

	return 0

# Is the vnum an SP automatic potion?
def IS_AUTO_POTION_SP(itemVnum):
	if 72727 <= itemVnum and 72730 >= itemVnum:
		return 1
	elif itemVnum >= 76004 and itemVnum <= 76005:		## New Gift Blue Dragon's Blessing
		return 1
	elif itemVnum == 79013:
		return 1

	return 0

if app.ENABLE_RENEWAL_AFFECT:
	def IS_AFFECT_PLUS(itemVnum):
		if itemVnum == 0:
			return 0

		item.SelectItem(itemVnum)
		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()

		if itemType == item.ITEM_TYPE_USE and itemSubType == item.USE_AFFECT_PLUS:
			return 1

		return 0

##################################################################
# No official BEGIN

if app.ENABLE_GROWTH_PET_SYSTEM:
	def IS_GROWTH_PET_ITEM(itemVnum):
		if itemVnum == 0:
			return 0

		item.SelectItem(itemVnum)
		itemType = item.GetItemType()
		itemSubType = item.GetItemSubType()

		if itemType == item.ITEM_TYPE_GROWTH_PET and itemSubType == item.PET_UPBRINGING:
			return 1

		return 0

if app.ENABLE_RENEWAL_SWITCHBOT:
	def CanSwitchItemBonus(itemIndex):
		if item.GetItemType() == item.ITEM_TYPE_COSTUME:
			return FALSE;

		if (itemIndex >= 11901 and itemIndex <= 11904 or itemIndex >= 11911 and itemIndex <= 11914):
			return FALSE;

		if (itemIndex == 50201 or itemIndex == 50202):
			return FALSE;

		return TRUE

	def CanPutItemInSwitchbot(itemIndex):
		item.SelectItem(itemIndex)

		if not CanSwitchItemBonus(itemIndex):
			return FALSE

		if not item.GetItemType() in [item.ITEM_TYPE_WEAPON, item.ITEM_TYPE_ARMOR, item.ITEM_TYPE_COSTUME]:
			return FALSE

		if item.GetItemType() == item.ITEM_TYPE_WEAPON and item.GetItemSubType() == item.WEAPON_ARROW:
			return FALSE

		if item.GetItemType() == item.ITEM_TYPE_ARMOR and\
			not item.GetItemSubType() in [item.ARMOR_BODY, item.ARMOR_HEAD, item.ARMOR_SHIELD, item.ARMOR_WRIST, item.ARMOR_FOOTS, item.ARMOR_NECK, item.ARMOR_EAR, item.ARMOR_PENDANT]:
			return FALSE

		if item.GetItemType() == item.ITEM_TYPE_COSTUME and\
			not item.GetItemSubType() in [item.COSTUME_TYPE_BODY, item.COSTUME_TYPE_HAIR, item.COSTUME_TYPE_WEAPON]:
			return FALSE

		return TRUE

if app.ENABLE_RENEWAL_SHOPEX:
	def NumberToStrRomanNumerals(number):
		ROMAN = [
			(1000, "M"),
			( 900, "CM"),
			( 500, "D"),
			( 400, "CD"),
			( 100, "C"),
			(  90, "XC"),
			(  50, "L"),
			(  40, "XL"),
			(  10, "X"),
			(   9, "IX"),
			(   5, "V"),
			(   4, "IV"),
			(   1, "I"),
		]

		result = ""
		for (arabic, roman) in ROMAN:
			(factor, number) = divmod(number, arabic)
			result += roman * factor
		return result

if app.ENABLE_AUTO_REFINE:
	IS_AUTO_REFINE = FALSE
	AUTO_REFINE_TYPE = 0
	AUTO_REFINE_DATA = {
		"ITEM" : [-1, -1],
		"NPC" : [0, -1, -1, 0]
	}

if app.ENABLE_HIDE_COSTUME_SYSTEM:
	HIDDEN_BODY_COSTUME = 0
	HIDDEN_HAIR_COSTUME = 0
	if app.ENABLE_ACCE_COSTUME_SYSTEM:
		HIDDEN_ACCE_COSTUME = 0
	if app.ENABLE_WEAPON_COSTUME_SYSTEM:
		HIDDEN_WEAPON_COSTUME = 0
	if app.ENABLE_AURA_COSTUME_SYSTEM:
		HIDDEN_AURA_COSTUME = 0

if app.ENABLE_RENEWAL_OFFLINESHOP:
	MAX_SHOP_TYPE = 13
	MAX_SHOP_TITLE = 6

	def IS_SET(value, flag):
		return (value & flag) == flag

	def SET_BIT(value, bit):
		return value | (bit)

	def REMOVE_BIT(value, bit):
		return value & ~(bit)

	def getFlagValue(value):
		return 1 << value

	def getInjectCheck(text):
		words = ["SELECT", "TRUNCATE", "INSERT", "REPLACE", "DELETE", "ALTER", "DROP",";", ":", "*", "'", '"', "="]
		characters = []

		for word in words:
			if text.find(word) != -1:
				return TRUE

		return FALSE

if app.ENABLE_INGAME_WIKI_SYSTEM:
	_main_wiki_instance = None
	_listbox_wiki_instance = None

	def SetMainParent(instance):
		global _main_wiki_instance

		if _main_wiki_instance:
			del _main_wiki_instance

		_main_wiki_instance = instance

	def GetMainParent():
		global _main_wiki_instance
		return _main_wiki_instance

	def SetListBox(instance):
		global _listbox_wiki_instance

		if _listbox_wiki_instance:
			del _listbox_wiki_instance

		_listbox_wiki_instance = instance

	def GetListBox():
		global _listbox_wiki_instance
		return _listbox_wiki_instance

if app.ENABLE_AUTOMATIC_PICK_UP_SYSTEM:
	PICKUPMODE = 0
	PREMIUMMODE = [FALSE, 0]

if app.ENABLE_INVENTORY_EXPANSION_SYSTEM:
	Inventory_Locked = {
		"Active" : TRUE,
	}

	for x in xrange(0, 6):
		Inventory_Locked.update({"Keys_Can_Unlock_%d" % x : 0})

if app.ENABLE_MULTI_LANGUAGE_SYSTEM:
	def GetLang(lang):
		langDict = { 1 : "en", 2 : "ro", 3 : "pt", 4 : "es", 5 : "fr", 6 : "de", 7 : "pl", 8 : "it", 9 : "cz", 10 : "hu", 11 : "tr" }

		for key, value in langDict.iteritems():
			if key == lang:
				return value
			elif value == lang:
				return key

		return "tr"

if app.ENABLE_HUNTING_SYSTEM:
	HUNTING_MAIN_UI_SHOW = 0
	HUNTING_MINI_UI_SHOW = 0
	HUNTING_BUTTON_FLASH = 0
	HUNTING_BUTTON_IS_FLASH = 0

# No official END
