#include "stdafx.h"
#include <math.h>
#include "ProtoReader.h"
#include "CsvReader.h"
#include <sstream>

#include "../../common/service.h"

using namespace std;

inline string trim_left(const string& str)
{
	string::size_type n = str.find_first_not_of(" \t\v\n\r");
	return n == string::npos ? str : str.substr(n, str.length());
}

inline string trim_right(const string& str)
{
	string::size_type n = str.find_last_not_of(" \t\v\n\r");
	return n == string::npos ? str : str.substr(0, n + 1);
}

string trim(const string& str)
{
	return trim_left(trim_right(str));
}

static string* StringSplit(string strOrigin, string strTok)
{
	int32_t index = 0;
	string* strResult = new string[30];
	size_t cutAt = 0;
	while ((cutAt = strOrigin.find_first_of(strTok)) != string::npos)
	{
		if (cutAt > 0 && index < 29)
			strResult[index++] = trim(strOrigin.substr(0, cutAt));

		strOrigin = strOrigin.substr(cutAt + 1);
	}

	if (!strOrigin.empty() && index < 30)
		strResult[index++] = trim(strOrigin);

	return strResult;
}

int32_t get_Item_Type_Value(const string &inputString)
{
	string s = trim(inputString);
	if (s == "ITEM_TITLE" || s == "ITEM_FORCE" || s == "ITEM_MEDIUM" || s == "ITEM_MERCENARY")
		return ITEM_USE;
	if (s == "ITEM_ALCHEMY")
		return ITEM_MATERIAL;
	if (s == "ITEM_BELT")
		return ITEM_ARMOR;
	if (s == "29")
		return ITEM_DS;

	string arType[] =
	{
		"ITEM_NONE",			//0
		"ITEM_WEAPON",			//1
		"ITEM_ARMOR",			//2
		"ITEM_USE",				//3
		"ITEM_AUTOUSE",			//4
		"ITEM_MATERIAL",		//5
		"ITEM_SPECIAL",			//6
		"ITEM_TOOL",			//7
		"ITEM_LOTTERY",			//8
		"ITEM_ELK",				//9
		"ITEM_METIN",			//10
		"ITEM_CONTAINER",		//11
		"ITEM_FISH",			//12
		"ITEM_ROD",				//13
		"ITEM_RESOURCE",		//14
		"ITEM_CAMPFIRE",		//15
		"ITEM_UNIQUE",			//16
		"ITEM_SKILLBOOK",		//17
		"ITEM_QUEST",			//18
		"ITEM_POLYMORPH",		//19
		"ITEM_TREASURE_BOX",	//20
		"ITEM_TREASURE_KEY",	//21
		"ITEM_SKILLFORGET",		//22
		"ITEM_GIFTBOX",			//23
		"ITEM_PICK",			//24
		"ITEM_HAIR",			//25
		"ITEM_TOTEM",			//26
		"ITEM_BLEND",			//27
		"ITEM_COSTUME",			//28
		"ITEM_DS",				//29
		"ITEM_SPECIAL_DS",		//30
		"ITEM_EXTRACT",			//31
		"ITEM_SECONDARY_COIN",	//32
		"ITEM_RING",			//33
#ifdef ENABLE_MOUNT_SYSTEM
		"ITEM_MOUNT",			//34
#endif
#ifdef ENABLE_PET_SYSTEM
		"ITEM_PET",				//35
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
		"ITEM_GROWTH_PET",		//36
#endif
#ifdef ENABLE_NEW_ITEM_TYPE_GACHA
		"ITEM_GACHA",			//37
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
		"ITEM_PASSIVE",			//38
#endif
#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
		"ITEM_MEDIUM"
#endif
	};

	int32_t retInt = -1;
	for (uint32_t j = 0; j < sizeof(arType)/sizeof(arType[0]); j++)
	{
		string tempString = arType[j];
		if (inputString.find(tempString)!=string::npos && tempString.find(inputString)!=string::npos)
		{
			retInt = j;
			break;
		}
	}

	return retInt;
}

int32_t get_Item_SubType_Value(uint32_t type_value, const string &inputString)
{
	static std::map<int32_t, std::vector<std::string>> subtypes;

	subtypes[ITEM_WEAPON] =
	{
		"WEAPON_SWORD",
		"WEAPON_DAGGER",
		"WEAPON_BOW",
		"WEAPON_TWO_HANDED",
		"WEAPON_BELL",
		"WEAPON_FAN",
		"WEAPON_ARROW",
		"WEAPON_MOUNT_SPEAR"
	};

	subtypes[ITEM_ARMOR] =
	{
		"ARMOR_BODY",
		"ARMOR_HEAD",
		"ARMOR_SHIELD",
		"ARMOR_WRIST",
		"ARMOR_FOOTS",
		"ARMOR_NECK",
		"ARMOR_EAR",
		"ARMOR_BELT",
#ifdef ENABLE_PENDANT_SYSTEM
		"ARMOR_PENDANT",
#endif
		"ARMOR_NUM_TYPES"
	};

	subtypes[ITEM_USE] =
	{
		"USE_POTION",
		"USE_TALISMAN",
		"USE_TUNING",
		"USE_MOVE",
		"USE_TREASURE_BOX",
		"USE_MONEYBAG",
		"USE_BAIT",
		"USE_ABILITY_UP",
		"USE_AFFECT",
		"USE_CREATE_STONE",
		"USE_SPECIAL",
		"USE_POTION_NODELAY",
		"USE_CLEAR",
		"USE_INVISIBILITY",
		"USE_DETACHMENT",
		"USE_BUCKET",
		"USE_POTION_CONTINUE",
		"USE_CLEAN_SOCKET",
		"USE_CHANGE_ATTRIBUTE",
		"USE_ADD_ATTRIBUTE",
		"USE_ADD_ACCESSORY_SOCKET",
		"USE_PUT_INTO_ACCESSORY_SOCKET",
		"USE_ADD_ATTRIBUTE2",
		"USE_RECIPE",
		"USE_CHANGE_ATTRIBUTE2",
		"USE_BIND",
		"USE_UNBIND",
		"USE_TIME_CHARGE_PER",
		"USE_TIME_CHARGE_FIX",
		"USE_PUT_INTO_BELT_SOCKET"
#ifdef ENABLE_TITLE_SYSTEM
		,"USE_PUT_INTO_RING_SOCKET"
		,"USE_TITLE"
#else
		,"USE_PUT_INTO_RING_SOCKET"
#endif
#ifdef ENABLE_RENEWAL_AFFECT
		,"USE_AFFECT_PLUS"
#endif
#ifdef ENABLE_BONUS_COSTUME_SYSTEM
		,"USE_CHANGE_COSTUME_ATTR"
		,"USE_RESET_COSTUME_ATTR"
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		,"USE_PUT_INTO_AURA_SOCKET"
#endif
#ifdef __ENABLE_COLLECTIONS_SYSTEM__
		,"USE_COLLECTION_SCROLL",
#endif
	};

#ifdef ENABLE_PASSIVE_SYSTEM
	subtypes[ITEM_PASSIVE] =
	{
		"PASSIVE_JOB"
	};
#endif

	subtypes[ITEM_AUTOUSE] =
	{
		"AUTOUSE_POTION",
		"AUTOUSE_ABILITY_UP",
		"AUTOUSE_BOMB",
		"AUTOUSE_GOLD",
		"AUTOUSE_MONEYBAG",
		"AUTOUSE_TREASURE_BOX"
	};

	subtypes[ITEM_MATERIAL] =
	{
		"MATERIAL_LEATHER",
		"MATERIAL_BLOOD",
		"MATERIAL_ROOT",
		"MATERIAL_NEEDLE",
		"MATERIAL_JEWEL",
		"MATERIAL_DS_REFINE_NORMAL",
		"MATERIAL_DS_REFINE_BLESSED",
		"MATERIAL_DS_REFINE_HOLLY"
#ifdef ENABLE_PASSIVE_SYSTEM
		,"MATERIAL_PASSIVE_WEAPON"
		,"MATERIAL_PASSIVE_ARMOR"
		,"MATERIAL_PASSIVE_ACCE"
		,"MATERIAL_PASSIVE_ELEMENT"
#endif
	};

	subtypes[ITEM_SPECIAL] =
	{
		"SPECIAL_MAP",
		"SPECIAL_KEY",
		"SPECIAL_DOC",
		"SPECIAL_SPIRIT"
	};

	subtypes[ITEM_TOOL] =
	{
		"TOOL_FISHING_ROD"
	};

	subtypes[ITEM_LOTTERY] =
	{
		"LOTTERY_TICKET",
		"LOTTERY_INSTANT"
	};

	subtypes[ITEM_METIN] =
	{
		"METIN_NORMAL",
		"METIN_GOLD"
	};

	subtypes[ITEM_FISH] =
	{
		"FISH_ALIVE",
		"FISH_DEAD"
	};

	subtypes[ITEM_RESOURCE] =
	{
		"RESOURCE_FISHBONE",
		"RESOURCE_WATERSTONEPIECE",
		"RESOURCE_WATERSTONE",
		"RESOURCE_BLOOD_PEARL",
		"RESOURCE_BLUE_PEARL",
		"RESOURCE_WHITE_PEARL",
		"RESOURCE_BUCKET",
		"RESOURCE_CRYSTAL",
		"RESOURCE_GEM",
		"RESOURCE_STONE",
		"RESOURCE_METIN",
		"RESOURCE_ORE"
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		,"RESOURCE_AURA"
#endif
	};

	subtypes[ITEM_UNIQUE] =
	{
		"UNIQUE_NONE",
		"UNIQUE_BOOK",
		"UNIQUE_SPECIAL_RIDE",
		"UNIQUE_3",
		"UNIQUE_4",
		"UNIQUE_5",
		"UNIQUE_6",
		"UNIQUE_7",
		"UNIQUE_8",
		"UNIQUE_9",
		"USE_SPECIAL",
	};

	subtypes[ITEM_COSTUME] =
	{
		"COSTUME_BODY",
		"COSTUME_HAIR"
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		,"COSTUME_ACCE"
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		,"COSTUME_WEAPON"
#endif
#ifdef ENABLE_MOUNT_PET_SKIN
		,"COSTUME_MOUNT"
		,"COSTUME_PET"
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		,"COSTUME_AURA"
#endif
	};

	subtypes[ITEM_DS] =
	{
		"DS_SLOT1",
		"DS_SLOT2",
		"DS_SLOT3",
		"DS_SLOT4",
		"DS_SLOT5",
		"DS_SLOT6"
	};

	subtypes[ITEM_EXTRACT] =
	{
		"EXTRACT_DRAGON_SOUL",
		"EXTRACT_DRAGON_HEART"
	};

#ifdef ENABLE_GROWTH_PET_SYSTEM
	subtypes[ITEM_GROWTH_PET] =
	{
		"PET_EGG",
		"PET_UPBRINGING",
		"PET_BAG",
		"PET_FEEDSTUFF",
		"PET_SKILL",
		"PET_SKILL_DEL_BOOK",
		"PET_NAME_CHANGE",
		"PET_EXPFOOD",
		"PET_SKILL_ALL_DEL_BOOK",
		"PET_EXPFOOD_PER",
		"PET_ATTR_DETERMINE",
		"PET_ATTR_CHANGE",
		"PET_PAY",
		"PET_PRIMIUM_FEEDSTUFF",
	};
#endif

	subtypes[ITEM_QUEST] =
	{
		"QUEST_NONE",
		"QUEST_PET_PAY",
		"QUEST_WARP",
		"QUEST_GEM_BAG"
	};

#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
	subtypes[ITEM_MEDIUM] =
	{
		"MEDIUM_MOVE_COSTUME_ATTR",
		"MEDIUM_MOVE_COSTUME_ACCE_ATTR"
	};
#endif

	if (type_value < 0 || type_value >= ITEM_TYPE_MAX)
	{
		sys_err("Out of range type! (type_value: %d, max valid type: %d)", type_value, ITEM_TYPE_MAX);
		return -1;
	}

	// Don't process if there are no subtypes for this type 
	if (subtypes.count(type_value) == 0)
		return 0;
	
	std::string trimmedInput = trim(inputString);

	if (!trimmedInput.empty() && (isdigit(trimmedInput[0]) || trimmedInput == "NONE"))
	{
		int val = atoi(trimmedInput.c_str());
		if (val >= 0 && (size_t)val < subtypes[type_value].size())
			return val;
		return 0;
	}

	for (size_t i = 0, size = subtypes[type_value].size(); i < size; ++i)
	{
		// Found the subtype that's specified
		if (trimmedInput.compare(subtypes[type_value][i]) == 0)
			return i;
	}

	return 0;
}

int32_t get_Item_AntiFlag_Value(const string &inputString)
{
	string arAntiFlag[] =
	{
		"ANTI_FEMALE",
		"ANTI_MALE",
		"ANTI_MUSA",
		"ANTI_ASSASSIN",
		"ANTI_SURA",
		"ANTI_MUDANG",
		"ANTI_GET",
		"ANTI_DROP",
		"ANTI_SELL",
		"ANTI_EMPIRE_A",
		"ANTI_EMPIRE_B",
		"ANTI_EMPIRE_C",
		"ANTI_SAVE",
		"ANTI_GIVE",
		"ANTI_PKDROP",
		"ANTI_STACK",
		"ANTI_MYSHOP",
		"ANTI_SAFEBOX"
#ifdef ENABLE_DESTROY_DIALOG
		,"ANTI_DESTROY"
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
		,"ANTI_PETFEED"
#endif
		,"ANTI_RT_REMOVE"
	};

	int32_t retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for(uint32_t i = 0; i < sizeof(arAntiFlag)/sizeof(arAntiFlag[0]); i++)
	{
		string tempString = arAntiFlag[i];
		for (uint32_t j = 0; j < 30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
				retValue = retValue + pow((float)2, (float)i);

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

int32_t get_Item_Flag_Value(const string &inputString)
{

	string arFlag[] = 
		{
			"ITEM_TUNABLE",
			"ITEM_SAVE",
			"ITEM_STACKABLE",
			"COUNT_PER_1GOLD",
			"ITEM_SLOW_QUERY",
			"ITEM_UNIQUE",
			"ITEM_MAKECOUNT",
			"ITEM_IRREMOVABLE",
			"CONFIRM_WHEN_USE",
			"QUEST_USE",
			"QUEST_USE_MULTIPLE",
			"QUEST_GIVE",
			"ITEM_QUEST",
			"LOG",
			"STACKABLE",
			"SLOW_QUERY",
			"REFINEABLE",
			"IRREMOVABLE",
			"ITEM_APPLICABLE"
		};

	int32_t retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for (uint32_t i = 0; i < sizeof(arFlag)/sizeof(arFlag[0]); i++)
	{
		string tempString = arFlag[i];
		for (uint32_t j = 0; j < 30; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
				retValue = retValue + pow((float)2, (float)i);

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

int32_t get_Item_WearFlag_Value(const string &inputString)
{
	string arWearrFlag[] =
	{
		"WEAR_BODY",
		"WEAR_HEAD",
		"WEAR_FOOTS",
		"WEAR_WRIST",
		"WEAR_WEAPON",
		"WEAR_NECK",
		"WEAR_EAR",
		"WEAR_SHIELD",
		"WEAR_UNIQUE",
		"WEAR_ARROW",
		"WEAR_HAIR",
		"WEAR_ABILITY",
		"WEAR_COSTUME_BODY",
		"WEAR_COSTUME_HAIR"
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		,"WEAR_COSTUME_ACCE"
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
		,"WEAR_COSTUME_WEAPON"
#endif
#ifdef ENABLE_MOUNT_PET_SKIN
		,"WEAR_COSTUME_MOUNT"
		,"WEAR_COSTUME_PET"
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		,"WEAR_COSTUME_AURA"
#endif
		,"WEAR_BELT"
#ifdef ENABLE_MOUNT_SYSTEM
		,"WEAR_MOUNT"
#endif
#ifdef ENABLE_PET_SYSTEM
		,"WEAR_PET"
#endif
#ifdef ENABLE_PENDANT_SYSTEM
		,"WEAR_PENDANT"
#endif
	};

	int32_t retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for(uint32_t i = 0; i < sizeof(arWearrFlag)/sizeof(arWearrFlag[0]); i++)
	{
		string tempString = arWearrFlag[i];
		for (uint32_t j = 0; j < 30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
				retValue = retValue + pow((float)2, (float)i);

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

int32_t get_Item_Immune_Value(const string &inputString)
{

	string arImmune[] =
	{
		"PARA",
		"CURSE",
		"STUN",
		"SLEEP",
		"SLOW",
		"POISON",
		"TERROR",
	};

	int32_t retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for (uint32_t i = 0; i <sizeof(arImmune)/sizeof(arImmune[0]); i++)
	{
		string tempString = arImmune[i];
		for (uint32_t j = 0; j < 30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
				retValue = retValue + pow((float)2,(float)i);

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

int32_t get_Item_LimitType_Value(const string &inputString)
{
	string arLimitType[] =
	{
		"LIMIT_NONE",
		"LEVEL",
		"STR",
		"DEX",
		"INT",
		"CON",
		"REAL_TIME",
		"REAL_TIME_FIRST_USE",
		"TIMER_BASED_ON_WEAR",
#ifdef ENABLE_PITTY_REFINE
		"PITTY_REFINE",
#endif
	};

	string tempInputString = trim(inputString);
	if (tempInputString == "NEWWORLD_LEVEL")
		tempInputString = "LEVEL";
	else if (tempInputString == "DURATION")
		tempInputString = "REAL_TIME";

	int32_t retInt = -1;
	for (uint32_t j = 0; j < sizeof(arLimitType)/sizeof(arLimitType[0]); j++)
	{
		string tempString = arLimitType[j];
		if (tempInputString.compare(tempString) == 0)
		{
			retInt = j;
			break;
		}
	}

	return retInt;
}

int32_t get_Item_ApplyType_Value(const string &inputString)
{
	string s = trim(inputString);
	if (s.empty())
		return 0;

	string raw = (s.compare(0, 6, "APPLY_") == 0) ? s.substr(6) : s;

	static const std::unordered_map<string, string> s_aliases = {
		{"ATT_BONUS_TO_MONSTER", "APPLY_ATTBONUS_MONSTER"},
		{"ATT_BONUS_TO_WARRIOR", "APPLY_ATTBONUS_WARRIOR"},
		{"ATT_BONUS_TO_ASSASSIN", "APPLY_ATTBONUS_ASSASSIN"},
		{"ATT_BONUS_TO_SURA", "APPLY_ATTBONUS_SURA"},
		{"ATT_BONUS_TO_SHAMAN", "APPLY_ATTBONUS_SHAMAN"},
		{"ATT_BONUS_TO_WOLFMAN", "APPLY_ATTBONUS_WOLFMAN"},
		{"ATTBONUS_ELEC", "APPLY_ENCHANT_ELECT"},
		{"ATTBONUS_ELECT", "APPLY_ENCHANT_ELECT"},
		{"ATTBONUS_FIRE", "APPLY_ENCHANT_FIRE"},
		{"ATTBONUS_ICE", "APPLY_ENCHANT_ICE"},
		{"ATTBONUS_WIND", "APPLY_ENCHANT_WIND"},
		{"ATTBONUS_EARTH", "APPLY_ENCHANT_EARTH"},
		{"ATTBONUS_DARK", "APPLY_ENCHANT_DARK"},
		{"ATTBONUS_METIN", "APPLY_ATTBONUS_STONE"},
		{"RESIST_CRITICAL", "APPLY_ANTI_CRITICAL_PCT"},
		{"RESIST_PENETRATE", "APPLY_ANTI_PENETRATE_PCT"},
		{"MONSTER_DEFEND_BONUS", "APPLY_RESIST_MONSTER"},
		{"ATT_BONUS", "APPLY_ATT_GRADE_BONUS"},
		{"DEF_BONUS", "APPLY_DEF_GRADE_BONUS"},
		{"MAGIC_ATT_GRADE_BONUS", "APPLY_MAGIC_ATT_GRADE"},
		{"MAGIC_DEF_GRADE_BONUS", "APPLY_MAGIC_DEF_GRADE"},
		{"MELEE_MAGIC_ATT_BONUS_PER", "APPLY_MELEE_MAGIC_ATTBONUS_PER"},
	};

	auto it = s_aliases.find(raw);
	if (it != s_aliases.end())
		s = it->second;
	else
	{
		it = s_aliases.find(s);
		if (it != s_aliases.end())
			s = it->second;
	}

	string withApply = (s.compare(0, 6, "APPLY_") == 0) ? s : ("APPLY_" + s);

	string arApplyType[] =
	{
		"APPLY_NONE",
		"APPLY_MAX_HP",
		"APPLY_MAX_SP",
		"APPLY_CON",
		"APPLY_INT",
		"APPLY_STR",
		"APPLY_DEX",
		"APPLY_ATT_SPEED",
		"APPLY_MOV_SPEED",
		"APPLY_CAST_SPEED",
		"APPLY_HP_REGEN",
		"APPLY_SP_REGEN",
		"APPLY_POISON_PCT",
		"APPLY_STUN_PCT",
		"APPLY_SLOW_PCT",
		"APPLY_CRITICAL_PCT",
		"APPLY_PENETRATE_PCT",
		"APPLY_ATTBONUS_HUMAN",
		"APPLY_ATTBONUS_ANIMAL",
		"APPLY_ATTBONUS_ORC",
		"APPLY_ATTBONUS_MILGYO",
		"APPLY_ATTBONUS_UNDEAD",
		"APPLY_ATTBONUS_DEVIL",
		"APPLY_STEAL_HP",
		"APPLY_STEAL_SP",
		"APPLY_MANA_BURN_PCT",
		"APPLY_DAMAGE_SP_RECOVER",
		"APPLY_BLOCK",
		"APPLY_DODGE",
		"APPLY_RESIST_SWORD",
		"APPLY_RESIST_TWOHAND",
		"APPLY_RESIST_DAGGER",
		"APPLY_RESIST_BELL",
		"APPLY_RESIST_FAN",
		"APPLY_RESIST_BOW",
		"APPLY_RESIST_FIRE",
		"APPLY_RESIST_ELEC",
		"APPLY_RESIST_MAGIC",
		"APPLY_RESIST_WIND",
		"APPLY_REFLECT_MELEE",
		"APPLY_REFLECT_CURSE",
		"APPLY_POISON_REDUCE",
		"APPLY_KILL_SP_RECOVER",
		"APPLY_EXP_DOUBLE_BONUS",
		"APPLY_GOLD_DOUBLE_BONUS",
		"APPLY_ITEM_DROP_BONUS",
		"APPLY_POTION_BONUS",
		"APPLY_KILL_HP_RECOVER",
		"APPLY_IMMUNE_STUN",
		"APPLY_IMMUNE_SLOW",
		"APPLY_IMMUNE_FALL",
		"APPLY_SKILL",
		"APPLY_BOW_DISTANCE",
		"APPLY_ATT_GRADE_BONUS",
		"APPLY_DEF_GRADE_BONUS",
		"APPLY_MAGIC_ATT_GRADE",
		"APPLY_MAGIC_DEF_GRADE",
		"APPLY_CURSE_PCT",
		"APPLY_MAX_STAMINA",
		"APPLY_ATTBONUS_WARRIOR",
		"APPLY_ATTBONUS_ASSASSIN",
		"APPLY_ATTBONUS_SURA",
		"APPLY_ATTBONUS_SHAMAN",
		"APPLY_ATTBONUS_MONSTER",
		"APPLY_MALL_ATTBONUS",
		"APPLY_MALL_DEFBONUS",
		"APPLY_MALL_EXPBONUS",
		"APPLY_MALL_ITEMBONUS",
		"APPLY_MALL_GOLDBONUS",
		"APPLY_MAX_HP_PCT",
		"APPLY_MAX_SP_PCT",
		"APPLY_SKILL_DAMAGE_BONUS",
		"APPLY_NORMAL_HIT_DAMAGE_BONUS",
		"APPLY_SKILL_DEFEND_BONUS",
		"APPLY_NORMAL_HIT_DEFEND_BONUS",
		"APPLY_EXTRACT_HP_PCT",
		"APPLY_PC_BANG_EXP_BONUS",
		"APPLY_PC_BANG_DROP_BONUS",
		"APPLY_RESIST_WARRIOR",
		"APPLY_RESIST_ASSASSIN",
		"APPLY_RESIST_SURA",
		"APPLY_RESIST_SHAMAN",
		"APPLY_ENERGY",
		"APPLY_DEF_GRADE",
		"APPLY_COSTUME_ATTR_BONUS",
		"APPLY_MAGIC_ATTBONUS_PER",
		"APPLY_MELEE_MAGIC_ATTBONUS_PER",
		"APPLY_RESIST_ICE",
		"APPLY_RESIST_EARTH",
		"APPLY_RESIST_DARK",
		"APPLY_ANTI_CRITICAL_PCT",
		"APPLY_ANTI_PENETRATE_PCT",
		"APPLY_BLEEDING_REDUCE",
		"APPLY_BLEEDING_PCT",
		"APPLY_ATTBONUS_WOLFMAN",
		"APPLY_RESIST_WOLFMAN",
		"APPLY_RESIST_CLAW",
		"APPLY_ACCEDRAIN_RATE",
		"APPLY_RESIST_MAGIC_REDUCTION",
		"APPLY_ENCHANT_ELECT",
		"APPLY_ENCHANT_FIRE",
		"APPLY_ENCHANT_ICE",
		"APPLY_ENCHANT_WIND",
		"APPLY_ENCHANT_EARTH",
		"APPLY_ENCHANT_DARK",
		"APPLY_ATTBONUS_CZ",
		"APPLY_ATTBONUS_INSECT",
		"APPLY_ATTBONUS_DESERT",
		"APPLY_ATTBONUS_SWORD",
		"APPLY_ATTBONUS_TWOHAND",
		"APPLY_ATTBONUS_DAGGER",
		"APPLY_ATTBONUS_BELL",
		"APPLY_ATTBONUS_FAN",
		"APPLY_ATTBONUS_BOW",
		"APPLY_ATTBONUS_CLAW",
		"APPLY_RESIST_HUMAN",
		"APPLY_RESIST_MOUNT_FALL",
		"UNK17",
		"APPLY_MOUNT",
		"APPLY_SKILL_DAMAGE_SAMYEON",
		"APPLY_SKILL_DAMAGE_TANHWAN",
		"APPLY_SKILL_DAMAGE_PALBANG",
		"APPLY_SKILL_DAMAGE_GIGONGCHAM",
		"APPLY_SKILL_DAMAGE_GYOKSAN",
		"APPLY_SKILL_DAMAGE_GEOMPUNG",
		"APPLY_SKILL_DAMAGE_AMSEOP",
		"APPLY_SKILL_DAMAGE_GUNGSIN",
		"APPLY_SKILL_DAMAGE_CHARYUN",
		"APPLY_SKILL_DAMAGE_SANGONG",
		"APPLY_SKILL_DAMAGE_YEONSA",
		"APPLY_SKILL_DAMAGE_KWANKYEOK",
		"APPLY_SKILL_DAMAGE_GIGUNG",
		"APPLY_SKILL_DAMAGE_HWAJO",
		"APPLY_SKILL_DAMAGE_SWAERYUNG",
		"APPLY_SKILL_DAMAGE_YONGKWON",
		"APPLY_SKILL_DAMAGE_PABEOB",
		"APPLY_SKILL_DAMAGE_MARYUNG",
		"APPLY_SKILL_DAMAGE_HWAYEOMPOK",
		"APPLY_SKILL_DAMAGE_MAHWAN",
		"APPLY_SKILL_DAMAGE_BIPABU",
		"APPLY_SKILL_DAMAGE_YONGBI",
		"APPLY_SKILL_DAMAGE_PAERYONG",
		"APPLY_SKILL_DAMAGE_NOEJEON",
		"APPLY_SKILL_DAMAGE_BYEURAK",
		"APPLY_SKILL_DAMAGE_CHAIN",
		"APPLY_SKILL_DAMAGE_CHAYEOL",
		"APPLY_SKILL_DAMAGE_SALPOONG",
		"APPLY_SKILL_DAMAGE_GONGDAB",
		"APPLY_SKILL_DAMAGE_PASWAE",
		"APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE",
		"APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE",
		"APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE",
		"APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE",
		"APPLY_HIT_BUFF_ENCHANT_FIRE",
		"APPLY_HIT_BUFF_ENCHANT_ICE",
		"APPLY_HIT_BUFF_ENCHANT_ELEC",
		"APPLY_HIT_BUFF_ENCHANT_WIND",
		"APPLY_HIT_BUFF_ENCHANT_DARK",
		"APPLY_HIT_BUFF_ENCHANT_EARTH",
		"APPLY_HIT_BUFF_RESIST_FIRE",
		"APPLY_HIT_BUFF_RESIST_ICE",
		"APPLY_HIT_BUFF_RESIST_ELEC",
		"APPLY_HIT_BUFF_RESIST_WIND",
		"APPLY_HIT_BUFF_RESIST_DARK",
		"APPLY_HIT_BUFF_RESIST_EARTH",
		"APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED",
		"APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED",
		"APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT",
		"APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_JEOKRANG_DEF_BONUS",
		"APPLY_USE_SKILL_GWIGEOM_DEF_BONUS",
		"APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_MANASHILED_CASTING_SPEED",
		"APPLY_USE_SKILL_HOSIN_DEF_BONUS",
		"APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS",
		"APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS",
		"APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED",
		"APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS",
		"APPLY_SKILL_DURATION_INCREASE_EUNHYUNG",
		"APPLY_SKILL_DURATION_INCREASE_GYEONGGONG",
		"APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG",
		"APPLY_SKILL_DURATION_INCREASE_JEOKRANG",
		"APPLY_USE_SKILL_PALBANG_HP_ABSORB",
		"APPLY_USE_SKILL_AMSEOP_HP_ABSORB",
		"APPLY_USE_SKILL_YEONSA_HP_ABSORB",
		"APPLY_USE_SKILL_YONGBI_HP_ABSORB",
		"APPLY_USE_SKILL_CHAIN_HP_ABSORB",
		"APPLY_USE_SKILL_PASWAE_SP_ABSORB",
		"APPLY_USE_SKILL_GIGONGCHAM_STUN",
		"APPLY_USE_SKILL_CHARYUN_STUN",
		"APPLY_USE_SKILL_PABEOB_STUN",
		"APPLY_USE_SKILL_MAHWAN_STUN",
		"APPLY_USE_SKILL_GONGDAB_STUN",
		"APPLY_USE_SKILL_SAMYEON_STUN",
		"APPLY_USE_SKILL_GYOKSAN_KNOCKBACK",
		"APPLY_USE_SKILL_SEOMJEON_KNOCKBACK",
		"APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK",
		"APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK",
		"APPLY_USE_SKILL_GONGDAB_KNOCKBACK",
		"APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK",
		"APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER",
		"APPLY_ATTBONUS_STONE",
		"APPLY_DAMAGE_HP_RECOVERY",
		"APPLY_DAMAGE_SP_RECOVERY",
		"APPLY_ALIGNMENT_DAMAGE_BONUS",
		"APPLY_NORMAL_DAMAGE_GUARD",
		"APPLY_MORE_THEN_HP90_DAMAGE_REDUCE",
		"APPLY_USE_SKILL_TUSOK_HP_ABSORB",
		"APPLY_USE_SKILL_PAERYONG_HP_ABSORB",
		"APPLY_USE_SKILL_BYEURAK_HP_ABSORB",
		"APPLY_FIRST_ATTRIBUTE_BONUS",
		"APPLY_SECOND_ATTRIBUTE_BONUS",
		"APPLY_THIRD_ATTRIBUTE_BONUS",
		"APPLY_FOURTH_ATTRIBUTE_BONUS",
		"APPLY_FIFTH_ATTRIBUTE_BONUS",
		"APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER",
		"APPLY_USE_SKILL_CHAYEOL_HP_ABSORB",
		"APPLY_SUNGMA_STR",
		"APPLY_SUNGMA_HP",
		"APPLY_SUNGMA_MOVE",
		"APPLY_SUNGMA_IMMUNE",
		"APPLY_HIT_PCT",
		"APPLY_RANDOM",
		"APPLY_ATTBONUS_PER_HUMAN",
		"APPLY_ATTBONUS_PER_ANIMAL",
		"APPLY_ATTBONUS_PER_ORC",
		"APPLY_ATTBONUS_PER_MILGYO",
		"APPLY_ATTBONUS_PER_UNDEAD",
		"APPLY_ATTBONUS_PER_DEVIL",
		"APPLY_ENCHANT_PER_ELECT",
		"APPLY_ENCHANT_PER_FIRE",
		"APPLY_ENCHANT_PER_ICE",
		"APPLY_ENCHANT_PER_WIND",
		"APPLY_ENCHANT_PER_EARTH",
		"APPLY_ENCHANT_PER_DARK",
		"APPLY_ATTBONUS_PER_CZ",
		"APPLY_ATTBONUS_PER_INSECT",
		"APPLY_ATTBONUS_PER_DESERT",
		"APPLY_ATTBONUS_PER_STONE",
		"APPLY_ATTBONUS_PER_MONSTER",
		"APPLY_RESIST_PER_HUMAN",
		"APPLY_RESIST_PER_ICE",
		"APPLY_RESIST_PER_DARK",
		"APPLY_RESIST_PER_EARTH",
		"APPLY_RESIST_PER_FIRE",
		"APPLY_RESIST_PER_ELEC",
		"APPLY_RESIST_PER_MAGIC",
		"APPLY_RESIST_PER_WIND",
		"APPLY_HIT_BUFF_SUNGMA_STR",
		"APPLY_HIT_BUFF_SUNGMA_MOVE",
		"APPLY_HIT_BUFF_SUNGMA_HP",
		"APPLY_HIT_BUFF_SUNGMA_IMMUNE",
		"APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER",
		"APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER",
		"APPLY_HIT_AUTO_HP_RECOVERY",
		"APPLY_HIT_AUTO_SP_RECOVERY",
		"APPLY_USE_SKILL_COOLTIME_DECREASE_ALL",
		"APPLY_HIT_STONE_ATTBONUS_STONE",
		"APPLY_HIT_STONE_DEF_GRADE_BONUS",
		"APPLY_KILL_BOSS_ITEM_BONUS",
		"APPLY_MOB_HIT_MOB_AGGRESSIVE",
		"APPLY_NO_DEATH_AND_HP_RECOVERY30",
		"APPLY_AUTO_PICKUP",
		"APPLY_MOUNT_NO_KNOCKBACK",
		"APPLY_SUNGMA_PER_STR",
		"APPLY_SUNGMA_PER_HP",
		"APPLY_SUNGMA_PER_MOVE",
		"APPLY_SUNGMA_PER_IMMUNE",
		"APPLY_IMMUNE_POISON100",
		"APPLY_IMMUNE_BLEEDING100",
		"APPLY_MONSTER_DEFEND_BONUS",
		"APPLY_RESERVED_293",
		"APPLY_RESERVED_294",
		"APPLY_RESERVED_295",
		"APPLY_RESERVED_296",
		"APPLY_RESERVED_297",
		"APPLY_RESERVED_298",
		"APPLY_RESERVED_299",
		"APPLY_USE_SKILL_FINISH_NEXT_COOLTIME_DECREASE_PCT",
		"APPLY_USE_SKILL_ILGWANGPYO_NEXT_COOLTIME_DECREASE_PCT",
		"APPLY_USE_SKILL_PUNGLOEPO_NEXT_COOLTIME_DECREASE_PCT",
		"APPLY_USE_SKILL_GEOMAGGWI_NEXT_COOLTIME_DECREASE_PCT",
		"APPLY_USE_SKILL_MABEOBAGGWI_NEXT_COOLTIME_DECREASE_PCT",
		"APPLY_USE_SKILL_METEO_NEXT_COOLTIME_DECREASE_PCT",
		"APPLY_USE_SKILL_CHEONUN_NEXT_COOLTIME_DECREASE_PCT",
		"APPLY_USE_SKILL_ILIPUNGU_NEXT_COOLTIME_DECREASE_PCT",
		"APPLY_GLOVE_BASE_EFFECT_INCREASE",
		"APPLY_NO_FALLEN",
		"APPLY_HIT_AREA_ELEC_ATTACK",
		"APPLY_BLOCK_PENETRATE_PCT",
		"APPLY_ATTBONUS_MYSTERY_DUNGEON",
		"APPLY_ATTBONUS_DRAGON",
		"APPLY_RESERVED_314",
		"APPLY_RESERVED_315",
		"APPLY_RESERVED_316",
		"APPLY_RESERVED_317",
		"APPLY_RESERVED_318",
		"APPLY_RESERVED_319",
		"APPLY_RESERVED_320",
		"APPLY_RESERVED_321",
		"APPLY_ATTBONUS_MOON",
		"APPLY_RESERVED_323",
		"APPLY_BONUS_BLOCK_PENETRATE_PCT",
		"APPLY_BONUS_HIT_PCT",
		"APPLY_REFUSE_EXP",
		"APPLY_RESERVED_327",
		"APPLY_RESERVED_328",
		"APPLY_RESERVED_329",
		"APPLY_ATTBONUS_BOSS",
		"APPLY_ATTBONUS_ELEMENTS",
		"APPLY_ENCHANT_ELEMENTS",
		"APPLY_ATTBONUS_CHARACTERS",
		"APPLY_ENCHANT_CHARACTERS",
		"APPLY_RESIST_MONSTER",
		"APPLY_ATTBONUS_MEDI_PVM",
		"APPLY_ATTBONUS_PVM_STR",
		"APPLY_ATTBONUS_PVM_BERSERKER",
	};

	int32_t retInt = -1;
	for (uint32_t j = 0; j < sizeof(arApplyType)/sizeof(arApplyType[0]); j++)
	{
		const string & tempString = arApplyType[j];
		if (s == tempString || withApply == tempString)
		{
			retInt = j;
			break;
		}
	}

	return retInt;
}


int32_t get_Mob_Rank_Value(const string &inputString)
{
	string arRank[] =
	{
		"PAWN",
		"S_PAWN",
		"KNIGHT",
		"S_KNIGHT",
		"BOSS",
		"KING",
	};

	int32_t retInt = -1;
	for (uint32_t j = 0; j < sizeof(arRank)/sizeof(arRank[0]); j++)
	{
		string tempString = arRank[j];
		string tempInputString = trim(inputString);

		if (tempInputString.compare(tempString) == 0)
		{
			retInt = j;
			break;
		}
	}

	return retInt;
}

int32_t get_Mob_Type_Value(const string &inputString)
{
	string arType[] =
	{
		"MONSTER",
		"NPC",
		"STONE",
		"WARP",
		"DOOR",
		"BUILDING",
		"PC",
		"POLYMORPH_PC",
		"HORSE",
		"GOTO"
#ifdef ENABLE_PET_SYSTEM
		,"PET"
#endif
#ifdef ENABLE_MOUNT_SYSTEM
		,"MOUNT"
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
		,"GROWTH_PET"
#endif
	};

	string tempInputString = trim(inputString);
	if (tempInputString == "PET_PAY")
		tempInputString = "PET";
	else if (tempInputString == "SHOP" || tempInputString == "OBJECT")
		tempInputString = "NPC";

	int32_t retInt = -1;
	for (uint32_t j = 0; j < sizeof(arType)/sizeof(arType[0]); j++)
	{
		string tempString = arType[j];
		if (tempInputString.compare(tempString) == 0)
		{
			retInt = j;
			break;
		}
	}

	return retInt;
}

int32_t get_Mob_BattleType_Value(const string &inputString)
{
	string arBattleType[] =
	{
		"MELEE",
		"RANGE",
		"MAGIC",
		"SPECIAL",
		"POWER",
		"TANKER",
		"SUPER_POWER",
		"SUPER_TANKER",
	};

	int32_t retInt = -1;
	for (uint32_t j = 0; j < sizeof(arBattleType)/sizeof(arBattleType[0]); j++)
	{
		string tempString = arBattleType[j];
		string tempInputString = trim(inputString);

		if (tempInputString.compare(tempString) == 0)
		{
			retInt = j;
			break;
		}
	}

	return retInt;
}

int32_t get_Mob_Size_Value(const string &inputString)
{
	string arSize[] =
	{
		"SMALL", //@fixme201 SAMLL to SMALL
		"MEDIUM",
		"BIG",
	};

	int32_t retInt = 0;
	for (uint32_t j = 0; j < sizeof(arSize)/sizeof(arSize[0]); j++)
	{
		string tempString = arSize[j];
		string tempInputString = trim(inputString);

		if (tempInputString.compare(tempString) == 0)
		{
			retInt = j + 1;
			break;
		}
	}

	return retInt;
}

int32_t get_Mob_AIFlag_Value(const string &inputString)
{
	string arAIFlag[] =
	{
		"AGGR",
		"NOMOVE",
		"COWARD",
		"NOATTSHINSU",
		"NOATTCHUNJO",
		"NOATTJINNO",
		"ATTMOB",
		"BERSERK",
		"STONESKIN",
		"GODSPEED",
		"DEATHBLOW",
		"REVIVE",
	};

	int32_t retValue = 0;
	string* arInputString = StringSplit(inputString, ",");
	for(uint32_t i = 0; i < sizeof(arAIFlag)/sizeof(arAIFlag[0]); i++)
	{
		string tempString = arAIFlag[i];
		for (uint32_t j = 0; j < 30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
				retValue = retValue + pow((float)2, (float)i);

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

int32_t get_Mob_RaceFlag_Value(const string &inputString)
{
	string arRaceFlag[] =
	{
		"ANIMAL",
		"UNDEAD",
		"DEVIL",
		"HUMAN",
		"ORC",
		"MILGYO",
		"INSECT",
		"FIRE",
		"ICE",
		"DESERT",
		"TREE",
		"ATT_ELEC",
		"ATT_FIRE",
		"ATT_ICE",
		"ATT_WIND",
		"ATT_EARTH",
		"ATT_DARK",
	};

	int32_t retValue = 0;
	string* arInputString = StringSplit(inputString, ",");
	for(uint32_t i = 0; i < sizeof(arRaceFlag)/sizeof(arRaceFlag[0]); i++)
	{
		string tempString = arRaceFlag[i];
		for (uint32_t j = 0; j < 30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
				retValue = retValue + pow((float)2, (float)i);

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

int32_t get_Mob_ImmuneFlag_Value(const string &inputString)
{
	string arImmuneFlag[] =
	{
		"STUN",
		"SLOW",
		"FALL",
		"CURSE",
		"POISON",
		"TERROR",
		"REFLECT",
	};

	int32_t retValue = 0;
	string* arInputString = StringSplit(inputString, ",");
	for(uint32_t i = 0; i < sizeof(arImmuneFlag)/sizeof(arImmuneFlag[0]); i++)
	{
		string tempString = arImmuneFlag[i];
		for (uint32_t j = 0; j < 30 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
				retValue = retValue + pow((float)2, (float)i);

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

string string_from_row(char *field, uint32_t length)
{
	auto buffer = new char[length+1];

	strncpy(buffer, field, length);
	buffer[length] = '\0';
	string res = buffer;

	delete []buffer;

	return res;
}
#ifndef __DUMP_PROTO__
bool Set_Proto_Mob_Table(TMobTable *mobTable, const cCsvTable &csvTable,std::map<int32_t, const char*> &nameMap)
{
	int32_t col = 0;
	str_to_number(mobTable->dwVnum, csvTable.AsStringByIndex(col++));
	strlcpy(mobTable->szName, csvTable.AsStringByIndex(col++), sizeof(mobTable->szName));

	auto it = nameMap.find(mobTable->dwVnum);
	if (it != nameMap.end())
	{
		const char * localeName = it->second;
		strlcpy(mobTable->szLocaleName, localeName, sizeof (mobTable->szLocaleName));
	}
	else
		strlcpy(mobTable->szLocaleName, mobTable->szName, sizeof (mobTable->szLocaleName));

	if (csvTable.ColCount() >= 85)
	{
		// Official GF 94-column format
		mobTable->bRank = get_Mob_Rank_Value(csvTable.AsStringByIndex(col++));
		mobTable->bType = get_Mob_Type_Value(csvTable.AsStringByIndex(col++));
		mobTable->bBattleType = get_Mob_BattleType_Value(csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bLevel, csvTable.AsStringByIndex(col++));

		col++; // 6: ScalePct
		str_to_number(mobTable->bSize, csvTable.AsStringByIndex(col++)); // 7: Size
		mobTable->dwAIFlag = get_Mob_AIFlag_Value(csvTable.AsStringByIndex(col++)); // 8: AIFlags
		str_to_number(mobTable->bMountCapacity, csvTable.AsStringByIndex(col++)); // 9: MountCapacity
		mobTable->dwRaceFlag = get_Mob_RaceFlag_Value(csvTable.AsStringByIndex(col++)); // 10: RaceFlags
		mobTable->dwImmuneFlag = get_Mob_ImmuneFlag_Value(csvTable.AsStringByIndex(col++)); // 11: ImmuneFlags (STUN, etc.)
		str_to_number(mobTable->bEmpire, csvTable.AsStringByIndex(col++)); // 12: Empire
		strlcpy(mobTable->szFolder, csvTable.AsStringByIndex(col++), sizeof(mobTable->szFolder)); // 13: Folder
		str_to_number(mobTable->bOnClickType, csvTable.AsStringByIndex(col++)); // 14: OnClick
		str_to_number(mobTable->bStr, csvTable.AsStringByIndex(col++)); // 15: St
		str_to_number(mobTable->bDex, csvTable.AsStringByIndex(col++)); // 16: Dx
		str_to_number(mobTable->bCon, csvTable.AsStringByIndex(col++)); // 17: Ht
		str_to_number(mobTable->bInt, csvTable.AsStringByIndex(col++)); // 18: Iq

		col += 4; // 19..22: SungMa stats (SungMaSt, SungMaDx, SungMaHt, SungMaIq)

		str_to_number(mobTable->dwDamageRange[0], csvTable.AsStringByIndex(col++)); // 23: MinDamage
		str_to_number(mobTable->dwDamageRange[1], csvTable.AsStringByIndex(col++)); // 24: MaxDamage
		str_to_number(mobTable->dwMaxHP, csvTable.AsStringByIndex(col++)); // 25: MaxHp
		str_to_number(mobTable->bRegenCycle, csvTable.AsStringByIndex(col++)); // 26: RegenCycle
		str_to_number(mobTable->bRegenPercent, csvTable.AsStringByIndex(col++)); // 27: RegenPercent
		str_to_number(mobTable->dwGoldMin, csvTable.AsStringByIndex(col++)); // 28: MinGold
		str_to_number(mobTable->dwGoldMax, csvTable.AsStringByIndex(col++)); // 29: MaxGold
		str_to_number(mobTable->dwExp, csvTable.AsStringByIndex(col++)); // 30: Exp

		col++; // 31: SungMaExp

		str_to_number(mobTable->wDef, csvTable.AsStringByIndex(col++)); // 32: Def
		str_to_number(mobTable->sAttackSpeed, csvTable.AsStringByIndex(col++)); // 33: AttackSpeed
		str_to_number(mobTable->sMovingSpeed, csvTable.AsStringByIndex(col++)); // 34: MoveSpeed
		str_to_number(mobTable->bAggresiveHPPct, csvTable.AsStringByIndex(col++)); // 35: AggressiveHpPct
		str_to_number(mobTable->wAggressiveSight, csvTable.AsStringByIndex(col++)); // 36: AggressiveSight
		str_to_number(mobTable->wAttackRange, csvTable.AsStringByIndex(col++)); // 37: AttackRange
		str_to_number(mobTable->dwDropItemVnum, csvTable.AsStringByIndex(col++)); // 38: DropItemGroup
		str_to_number(mobTable->dwResurrectionVnum, csvTable.AsStringByIndex(col++)); // 39: ResurrectionVnum

		// 40..45: 6 Enchants: Curse, Slow, Poison, Stun, Critical, Penetrate
		for (int32_t i = 0; i < MOB_ENCHANTS_MAX_NUM && i < 6; ++i)
			str_to_number(mobTable->cEnchants[i], csvTable.AsStringByIndex(col++));

		col++; // 46: ResistFist (skip)

		// 47..52: Sword, TwoHand, Dagger, Bell, Fan, Bow
		for (int32_t i = 0; i < 6; ++i)
			str_to_number(mobTable->cResists[i], csvTable.AsStringByIndex(col++));

		col++; // 53: ResistClaw (skip)

		// 54..58: Fire, Elect, Magic, Wind, Poison
		for (int32_t i = 6; i < MOB_RESISTS_MAX_NUM && i < 11; ++i)
			str_to_number(mobTable->cResists[i], csvTable.AsStringByIndex(col++));

		col += 10; // 59..68: ResistBleeding, AttElec..AttDark, ResistDark, ResistIce, ResistEarth

		str_to_number(mobTable->fDamMultiply, csvTable.AsStringByIndex(col++)); // 69: DamMultiply
		str_to_number(mobTable->dwSummonVnum, csvTable.AsStringByIndex(col++)); // 70: SummonVnum
		str_to_number(mobTable->dwDrainSP, csvTable.AsStringByIndex(col++)); // 71: DrainSp
		col++; // 72: MobColor
		str_to_number(mobTable->dwPolymorphItemVnum, csvTable.AsStringByIndex(col++)); // 73: PolymorphItem

		// 74..83: 5 Skills (Level, Vnum)
		for (int32_t i = 0; i < MOB_SKILL_MAX_NUM && i < 5; ++i)
		{
			str_to_number(mobTable->Skills[i].bLevel, csvTable.AsStringByIndex(col++));
			str_to_number(mobTable->Skills[i].dwVnum, csvTable.AsStringByIndex(col++));
		}

		// 84..88: SpBerserk..SpRevive
		str_to_number(mobTable->bBerserkPoint, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bStoneSkinPoint, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bGodSpeedPoint, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bDeathBlowPoint, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bRevivePoint, csvTable.AsStringByIndex(col++));

		col += 4; // 89..92: SpHealer..SpRHpRegen

		if (col < csvTable.ColCount())
			str_to_number(mobTable->fHitRange, csvTable.AsStringByIndex(col++));
		else
			mobTable->fHitRange = 0.0f;
	}
	else
	{
		// Legacy (71-72 columns)
		//RANK
		int32_t rankValue = get_Mob_Rank_Value(csvTable.AsStringByIndex(col++));
		mobTable->bRank = rankValue;

		//TYPE
		int32_t typeValue = get_Mob_Type_Value(csvTable.AsStringByIndex(col++));
		mobTable->bType = typeValue;

		//BATTLE_TYPE
		int32_t battleTypeValue = get_Mob_BattleType_Value(csvTable.AsStringByIndex(col++));
		mobTable->bBattleType = battleTypeValue;

		str_to_number(mobTable->bLevel, csvTable.AsStringByIndex(col++));

		//SIZE
		// int32_t sizeValue = get_Mob_Size_Value(csvTable.AsStringByIndex(col++));
		// mobTable->bSize = sizeValue;
		str_to_number(mobTable->bSize, csvTable.AsStringByIndex(col++));

		//AI_FLAG
		int32_t aiFlagValue = get_Mob_AIFlag_Value(csvTable.AsStringByIndex(col++));
		mobTable->dwAIFlag = aiFlagValue;

		// mount_capacity;
		// col++;
		str_to_number(mobTable->bMountCapacity, csvTable.AsStringByIndex(col++));

		//RACE_FLAG
		int32_t raceFlagValue = get_Mob_RaceFlag_Value(csvTable.AsStringByIndex(col++));
		mobTable->dwRaceFlag = raceFlagValue;

		//IMMUNE_FLAG
		int32_t immuneFlagValue = get_Mob_ImmuneFlag_Value(csvTable.AsStringByIndex(col++));
		mobTable->dwImmuneFlag = immuneFlagValue;

		str_to_number(mobTable->bEmpire, csvTable.AsStringByIndex(col++));

		strlcpy(mobTable->szFolder, csvTable.AsStringByIndex(col++), sizeof(mobTable->szFolder));

		str_to_number(mobTable->bOnClickType, csvTable.AsStringByIndex(col++));	

		str_to_number(mobTable->bStr, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bDex, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bCon, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bInt, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->dwDamageRange[0], csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->dwDamageRange[1], csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->dwMaxHP, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bRegenCycle, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bRegenPercent,	csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->dwGoldMin, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->dwGoldMax, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->dwExp,	csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->wDef, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->sAttackSpeed, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->sMovingSpeed, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bAggresiveHPPct, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->wAggressiveSight, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->wAttackRange, csvTable.AsStringByIndex(col++));

		str_to_number(mobTable->dwDropItemVnum, csvTable.AsStringByIndex(col++));	//32
		str_to_number(mobTable->dwResurrectionVnum, csvTable.AsStringByIndex(col++));
		for (int32_t i = 0; i < MOB_ENCHANTS_MAX_NUM; ++i)
			str_to_number(mobTable->cEnchants[i], csvTable.AsStringByIndex(col++));

		for (int32_t i = 0; i < MOB_RESISTS_MAX_NUM; ++i)
			str_to_number(mobTable->cResists[i], csvTable.AsStringByIndex(col++));

		str_to_number(mobTable->fDamMultiply, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->dwSummonVnum, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->dwDrainSP, csvTable.AsStringByIndex(col++));

		//Mob_Color
		++col;

		str_to_number(mobTable->dwPolymorphItemVnum, csvTable.AsStringByIndex(col++));

		str_to_number(mobTable->Skills[0].bLevel, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->Skills[0].dwVnum, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->Skills[1].bLevel, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->Skills[1].dwVnum, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->Skills[2].bLevel, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->Skills[2].dwVnum, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->Skills[3].bLevel, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->Skills[3].dwVnum, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->Skills[4].bLevel, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->Skills[4].dwVnum, csvTable.AsStringByIndex(col++));

		str_to_number(mobTable->bBerserkPoint, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bStoneSkinPoint, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bGodSpeedPoint, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bDeathBlowPoint, csvTable.AsStringByIndex(col++));
		str_to_number(mobTable->bRevivePoint, csvTable.AsStringByIndex(col++));

		if (col < csvTable.ColCount())
			str_to_number(mobTable->fHitRange, csvTable.AsStringByIndex(col++));
		else
			mobTable->fHitRange = 0.0f;
	}

	sys_log(0, "MOB #%-5d %-24s level: %-3u rank: %u empire: %d", mobTable->dwVnum, mobTable->szLocaleName, mobTable->bLevel, mobTable->bRank, mobTable->bEmpire);

	return true;
}

bool Set_Proto_Item_Table(TItemTable *itemTable, const cCsvTable &csvTable,std::map<int32_t, const char*> &nameMap)
{
	// Reading vnum and vnum range.
	{
		std::string s(csvTable.AsStringByIndex(0));
		uint32_t pos = s.find("~");
		if (std::string::npos == pos)
		{
			itemTable->dwVnum = atoi(s.c_str());
			itemTable->dwVnumRange = 0;
		}
		else
		{
			std::string s_start_vnum (s.substr(0, pos));
			std::string s_end_vnum (s.substr(pos +1 ));

			int32_t start_vnum = atoi(s_start_vnum.c_str());
			int32_t end_vnum = atoi(s_end_vnum.c_str());
			if (0 == start_vnum || (0 != end_vnum && end_vnum < start_vnum))
			{
				sys_err ("INVALID VNUM %s", s.c_str());
				return false;
			}

			itemTable->dwVnum = start_vnum;
			itemTable->dwVnumRange = end_vnum - start_vnum;
		}
	}

	strlcpy(itemTable->szName, csvTable.AsStringByIndex(1), sizeof(itemTable->szName));

	auto it = nameMap.find(itemTable->dwVnum);
	if (it != nameMap.end())
	{
		const char * localeName = it->second;
		strlcpy(itemTable->szLocaleName, localeName, sizeof (itemTable->szLocaleName));
	}
	else
		strlcpy(itemTable->szLocaleName, itemTable->szName, sizeof (itemTable->szLocaleName));

	itemTable->cLimitRealTimeFirstUseIndex = -1;
	itemTable->cLimitTimerBasedOnWearIndex = -1;

	if (csvTable.ColCount() >= 42)
	{
		// Official GF (42 cols)
		int32_t col = 2;
		itemTable->bType = get_Item_Type_Value(csvTable.AsStringByIndex(col++));
		itemTable->bSubType = get_Item_SubType_Value(itemTable->bType, csvTable.AsStringByIndex(col++));
		str_to_number(itemTable->bSize, csvTable.AsStringByIndex(col++));
		itemTable->dwAntiFlags = get_Item_AntiFlag_Value(csvTable.AsStringByIndex(col++));
		itemTable->dwFlags = get_Item_Flag_Value(csvTable.AsStringByIndex(col++));
		itemTable->dwWearFlags = get_Item_WearFlag_Value(csvTable.AsStringByIndex(col++));
		itemTable->dwImmuneFlag = get_Item_Immune_Value(csvTable.AsStringByIndex(col++));
		str_to_number(itemTable->dwGold, csvTable.AsStringByIndex(col++));
		str_to_number(itemTable->dwShopBuyPrice, csvTable.AsStringByIndex(col++));
		str_to_number(itemTable->dwRefinedVnum, csvTable.AsStringByIndex(col++));
		str_to_number(itemTable->wRefineSet, csvTable.AsStringByIndex(col++));
		col++; // 13: 67AttrMaterial
		str_to_number(itemTable->bAlterToMagicItemPct, csvTable.AsStringByIndex(col++)); // 14

		for (int32_t i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
		{
			itemTable->aLimits[i].bType = get_Item_LimitType_Value(csvTable.AsStringByIndex(col++));
			str_to_number(itemTable->aLimits[i].lValue, csvTable.AsStringByIndex(col++));

			if (LIMIT_REAL_TIME_START_FIRST_USE == itemTable->aLimits[i].bType)
				itemTable->cLimitRealTimeFirstUseIndex = (char)i;

			if (LIMIT_TIMER_BASED_ON_WEAR == itemTable->aLimits[i].bType)
				itemTable->cLimitTimerBasedOnWearIndex = (char)i;
		}

		for (int32_t i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
		{
			itemTable->aApplies[i].bType = get_Item_ApplyType_Value(csvTable.AsStringByIndex(col++));
			str_to_number(itemTable->aApplies[i].lValue, csvTable.AsStringByIndex(col++));
		}

		col += 2; // 25..26: ApplyType3, ApplyValue3 (skip GF 4th apply)

		for (int32_t i = 0; i < ITEM_VALUES_MAX_NUM; ++i)
			str_to_number(itemTable->alValues[i], csvTable.AsStringByIndex(col++));

		col += 6; // 33..38: Value6..Value11 (skip GF extra values)

		str_to_number(itemTable->bSpecular, csvTable.AsStringByIndex(col++)); // 39
		str_to_number(itemTable->bGainSocketPct, csvTable.AsStringByIndex(col++)); // 40
		str_to_number(itemTable->sAddonType, csvTable.AsStringByIndex(col++)); // 41
		str_to_number(itemTable->bWeight, "0");
	}
	else
	{
		// Legacy (33 cols)
		int32_t col = 0;
		int32_t dataArray[33];
		for (uint32_t i=0; i<sizeof(dataArray)/sizeof(dataArray[0]);i++)
		{
			int32_t validCheck = 0;
			if (i == 2)
			{
				dataArray[i] = get_Item_Type_Value(csvTable.AsStringByIndex(col));
				validCheck = dataArray[i];
			}
			else if (i == 3)
			{
				dataArray[i] = get_Item_SubType_Value(dataArray[i-1], csvTable.AsStringByIndex(col));
				validCheck = dataArray[i];
			}
			else if (i == 5)
			{
				dataArray[i] = get_Item_AntiFlag_Value(csvTable.AsStringByIndex(col));
				validCheck = dataArray[i];
			}
			else if (i == 6)
			{
				dataArray[i] = get_Item_Flag_Value(csvTable.AsStringByIndex(col));
				validCheck = dataArray[i];
			}
			else if (i == 7)
			{
				dataArray[i] = get_Item_WearFlag_Value(csvTable.AsStringByIndex(col));
				validCheck = dataArray[i];
			}
			else if (i == 8)
			{
				dataArray[i] = get_Item_Immune_Value(csvTable.AsStringByIndex(col));
				validCheck = dataArray[i];
			}
			else if (i == 14)
			{
				dataArray[i] = get_Item_LimitType_Value(csvTable.AsStringByIndex(col));
				validCheck = dataArray[i];
			}
			else if (i == 16)
			{
				dataArray[i] = get_Item_LimitType_Value(csvTable.AsStringByIndex(col));
				validCheck = dataArray[i];
			}
			else if (i == 18)
			{
				dataArray[i] = get_Item_ApplyType_Value(csvTable.AsStringByIndex(col));
				validCheck = dataArray[i];
			}
			else if (i == 20)
			{
				dataArray[i] = get_Item_ApplyType_Value(csvTable.AsStringByIndex(col));
				validCheck = dataArray[i];
			}
			else if (i == 22)
			{
				dataArray[i] = get_Item_ApplyType_Value(csvTable.AsStringByIndex(col));
				validCheck = dataArray[i];
			}
			else
				str_to_number(dataArray[i], csvTable.AsStringByIndex(col));

			if (validCheck == -1)
			{
				std::ostringstream dataStream;
				for (uint32_t j = 0; j < i; ++j)
					dataStream << dataArray[j] << ",";
				sys_err("ItemProto Reading Failed : Invalid value. (index: %d, col: %d, value: %s)", i, col, csvTable.AsStringByIndex(col));
				sys_err("\t%d ~ %d Values: %s", 0, i, dataStream.str().c_str());
				exit(0);
			}
			col = col + 1;
		}

		itemTable->bType = dataArray[2];
		itemTable->bSubType = dataArray[3];
		itemTable->bSize = dataArray[4];
		itemTable->dwAntiFlags = dataArray[5];
		itemTable->dwFlags = dataArray[6];
		itemTable->dwWearFlags = dataArray[7];
		itemTable->dwImmuneFlag = dataArray[8];
		itemTable->dwGold = dataArray[9];
		itemTable->dwShopBuyPrice = dataArray[10];
		itemTable->dwRefinedVnum = dataArray[11];
		itemTable->wRefineSet = dataArray[12];
		itemTable->bAlterToMagicItemPct = dataArray[13];

		for (int32_t i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
		{
			itemTable->aLimits[i].bType = dataArray[14+i*2];
			itemTable->aLimits[i].lValue = dataArray[15+i*2];

			if (LIMIT_REAL_TIME_START_FIRST_USE == itemTable->aLimits[i].bType)
				itemTable->cLimitRealTimeFirstUseIndex = (char)i;

			if (LIMIT_TIMER_BASED_ON_WEAR == itemTable->aLimits[i].bType)
				itemTable->cLimitTimerBasedOnWearIndex = (char)i;
		}

		for (int32_t i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
		{
			itemTable->aApplies[i].bType = dataArray[18+i*2];
			itemTable->aApplies[i].lValue = dataArray[19+i*2];
		}

		for (int32_t i = 0; i < ITEM_VALUES_MAX_NUM; ++i)
			itemTable->alValues[i] = dataArray[24+i];

		itemTable->bGainSocketPct = dataArray[31];
		itemTable->sAddonType = dataArray[32];
		str_to_number(itemTable->bWeight, "0");
	}

	return true;
}
#endif
