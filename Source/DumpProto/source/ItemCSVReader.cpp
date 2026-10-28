#include <unordered_map>
#include <math.h>
#include "ItemCSVReader.h"

#define ENABLE_WEAPON_COSTUME_SYSTEM
#define ENABLE_BONUS_COSTUME_SYSTEM
#define ENABLE_ACCE_COSTUME_SYSTEM
#define ENABLE_AURA_COSTUME_SYSTEM
#define ENABLE_GROWTH_PET_SYSTEM
#define ENABLE_DESTROY_DIALOG
#define ENABLE_RENEWAL_AFFECT
#define ENABLE_MOUNT_PET_SKIN
#define ENABLE_PENDANT_SYSTEM
#define ENABLE_MOUNT_SYSTEM
#define ENABLE_PET_SYSTEM
#define ENABLE_MOVE_COSTUME_ACCE_ATTR
#define ENABLE_PITTY_REFINE
#define __ENABLE_COLLECTIONS_SYSTEM__
#define ENABLE_NEW_ITEM_TYPE_GACHA
#define ENABLE_CONQUEROR_LEVEL
#define ENABLE_MAGIC_REDUCTION_SYSTEM
//#define ENABLE_WOLFMAN_CHARACTER
#define ENABLE_NEW_BONUS_SYSTEM
#define ENABLE_AVG_PVM
#define ENABLE_PASSIVE_SYSTEM
#define ENABLE_TITLE_SYSTEM

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

string trim(const string& str){return trim_left(trim_right(str));}

static string* StringSplit(string strOrigin, string strTok)
{
	int index = 0;
	string* strResult = new string[40];
	size_t cutAt = 0;
	while ((cutAt = strOrigin.find_first_of(strTok)) != string::npos)
	{
		if (cutAt > 0 && index < 39)
			strResult[index++] = trim(strOrigin.substr(0, cutAt));

		strOrigin = strOrigin.substr(cutAt + 1);
	}

	if (!strOrigin.empty() && index < 40)
		strResult[index++] = trim(strOrigin);

	return strResult;
}

int get_Item_Type_Value(string inputString)
{
	string s = trim(inputString);
	if (s == "ITEM_TITLE" || s == "ITEM_FORCE" || s == "ITEM_MERCENARY")
		s = "ITEM_USE";
	else if (s == "ITEM_ALCHEMY")
		s = "ITEM_MATERIAL";
	else if (s == "ITEM_BELT")
		s = "ITEM_ARMOR";
	else if (s == "29")
		s = "ITEM_DS";

	string arType[] =
	{
		"ITEM_NONE",
		"ITEM_WEAPON",
		"ITEM_ARMOR",
		"ITEM_USE",
		"ITEM_AUTOUSE",
		"ITEM_MATERIAL",
		"ITEM_SPECIAL",
		"ITEM_TOOL",
		"ITEM_LOTTERY",
		"ITEM_ELK",
		"ITEM_METIN",
		"ITEM_CONTAINER",
		"ITEM_FISH",
		"ITEM_ROD",
		"ITEM_RESOURCE",
		"ITEM_CAMPFIRE",
		"ITEM_UNIQUE",
		"ITEM_SKILLBOOK",
		"ITEM_QUEST",
		"ITEM_POLYMORPH",
		"ITEM_TREASURE_BOX",
		"ITEM_TREASURE_KEY",
		"ITEM_SKILLFORGET",
		"ITEM_GIFTBOX",
		"ITEM_PICK",
		"ITEM_HAIR",
		"ITEM_TOTEM",
		"ITEM_BLEND",
		"ITEM_COSTUME",
		"ITEM_DS",
		"ITEM_SPECIAL_DS",
		"ITEM_EXTRACT",
		"ITEM_SECONDARY_COIN",
		"ITEM_RING"
#ifdef ENABLE_MOUNT_SYSTEM
		,"ITEM_MOUNT"
#endif
#ifdef ENABLE_PET_SYSTEM
		,"ITEM_PET"
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
		,"ITEM_GROWTH_PET"
#endif
#ifdef ENABLE_NEW_ITEM_TYPE_GACHA
		,"ITEM_GACHA"
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
		,"ITEM_PASSIVE"
#endif
#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
		,"ITEM_MEDIUM"
#endif
	};

	int retInt = -1;
	for (int j=0;j<sizeof(arType)/sizeof(arType[0]);j++) {
		string tempString = arType[j];
		if	(inputString.find(tempString)!=string::npos && tempString.find(inputString)!=string::npos) {
			retInt =  j;
			break;
		}
	}

	return retInt;

}

#include <map>
#include <string>
#include <cstdint>
#include <vector>

enum EItemTypes
{
	ITEM_NONE 			= 0,
	ITEM_WEAPON 		= 1,
	ITEM_ARMOR 			= 2,
	ITEM_USE 			= 3,
	ITEM_AUTOUSE 		= 4,
	ITEM_MATERIAL 		= 5,
	ITEM_SPECIAL 		= 6,
	ITEM_TOOL 			= 7,
	ITEM_LOTTERY 		= 8,
	ITEM_ELK 			= 9,
	ITEM_METIN 			= 10,
	ITEM_CONTAINER		= 11,
	ITEM_FISH 			= 12,
	ITEM_ROD 			= 13,
	ITEM_RESOURCE 		= 14,
	ITEM_CAMPFIRE 		= 15,
	ITEM_UNIQUE 		= 16,
	ITEM_SKILLBOOK 		= 17,
	ITEM_QUEST 			= 18,
	ITEM_POLYMORPH		= 19,
	ITEM_TREASURE_BOX	= 20,
	ITEM_TREASURE_KEY	= 21,
	ITEM_SKILLFORGET	= 22,
	ITEM_GIFTBOX		= 23,
	ITEM_PICK			= 24,
	ITEM_HAIR			= 25,
	ITEM_TOTEM			= 26,
	ITEM_BLEND			= 27,
	ITEM_COSTUME		= 28,
	ITEM_DS				= 29,
	ITEM_SPECIAL_DS 	= 30,
	ITEM_EXTRACT		= 31,
	ITEM_SECONDARY_COIN	= 32,
	ITEM_RING			= 33,
#ifdef ENABLE_MOUNT_SYSTEM
	ITEM_MOUNT			= 34,
#endif
#ifdef ENABLE_PET_SYSTEM
	ITEM_PET			= 35,
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	ITEM_GROWTH_PET		= 36,
#endif
#ifdef ENABLE_NEW_ITEM_TYPE_GACHA
	ITEM_GACHA			= 37,
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
	ITEM_PASSIVE		= 38,
#endif
#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
	ITEM_MEDIUM,
#endif

	ITEM_TYPE_MAX
};

int get_Item_SubType_Value(int type_value, string inputString)
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
		,"USE_COLLECTION_SCROLL"
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
		"USE_SPECIAL"
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
		"PET_PRIMIUM_FEEDSTUFF"
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
		return 0;
	}

	// Don't process if there are no subtypes for this type 
	if (subtypes.count(type_value) == 0)
		return 0;

	std::string trimmedInput = trim(inputString);

	// Allow no subtype for ITEM_QUEST
	if (type_value == ITEM_QUEST && (trimmedInput.compare("NONE") == 0 || trimmedInput.compare("0") == 0))
		return 0;

	for (size_t i = 0, size = subtypes[type_value].size(); i < size; ++i)
	{
		// Found the subtype that's specified
		if (trimmedInput.compare(subtypes[type_value][i]) == 0)
			return i;
	}


	return 0;
}

int get_Item_AntiFlag_Value(string inputString)
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

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for(int i =0;i<sizeof(arAntiFlag)/sizeof(arAntiFlag[0]);i++) {
		string tempString = arAntiFlag[i];
		for (int j=0; j<40 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

int get_Item_Flag_Value(string inputString) 
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

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for(int i =0;i<sizeof(arFlag)/sizeof(arFlag[0]);i++) {
		string tempString = arFlag[i];
		for (int j=0; j<40 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

int get_Item_WearFlag_Value(string inputString)
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

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for(int i = 0; i<sizeof(arWearrFlag)/sizeof(arWearrFlag[0]);i++)
	{
		string tempString = arWearrFlag[i];
		for (int j = 0; j<40 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0)
			{
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}

	delete []arInputString;
	return retValue;
}

int get_Item_Immune_Value(string inputString) 
{
	string arImmune[] =
	{
		"PARA",
		"CURSE",
		"STUN",
		"SLEEP",
		"SLOW",
		"POISON",
		"TERROR"
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for(int i =0;i<sizeof(arImmune)/sizeof(arImmune[0]);i++) {
		string tempString = arImmune[i];
		for (int j=0; j<40 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

int get_Item_LimitType_Value(string inputString)
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

	int retInt = -1;
	for (int j = 0; j < sizeof(arLimitType)/sizeof(arLimitType[0]); j++)
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

int get_Item_ApplyType_Value(const string &inputString)
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


int get_Mob_Rank_Value(string inputString) 
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

	int retInt = -1;
	for (int j=0;j<sizeof(arRank)/sizeof(arRank[0]);j++) {
		string tempString = arRank[j];
		string tempInputString = trim(inputString);
		if	(tempInputString.compare(tempString)==0)
		{
			retInt =  j;
			break;
		}
	}

	return retInt;
}

int get_Mob_Type_Value(string inputString)
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

	int retInt = -1;
	for (int j=0;j<sizeof(arType)/sizeof(arType[0]);j++) {
		string tempString = arType[j];
		if	(tempInputString.compare(tempString)==0)
		{
			retInt =  j;
			break;
		}
	}

	return retInt;
}

int get_Mob_BattleType_Value(string inputString) 
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

	int retInt = -1;
	for (int j=0;j<sizeof(arBattleType)/sizeof(arBattleType[0]);j++) {
		string tempString = arBattleType[j];
		string tempInputString = trim(inputString);
		if	(tempInputString.compare(tempString)==0)
		{
			retInt =  j;
			break;
		}
	}

	return retInt;
}

int get_Mob_Size_Value(string inputString)
{
	string arSize[] =
	{
		"SMALL",
		"MEDIUM",
		"BIG",
	};

	int retInt = 0;
	for (int j=0;j<sizeof(arSize)/sizeof(arSize[0]);j++) {
		string tempString = arSize[j];
		string tempInputString = trim(inputString);
		if	(tempInputString.compare(tempString)==0)
		{
			retInt =  j + 1;
			break;
		}
	}

	return retInt;
}

int get_Mob_AIFlag_Value(string inputString)
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

	int retValue = 0;
	string* arInputString = StringSplit(inputString, ",");
	for(int i =0;i<sizeof(arAIFlag)/sizeof(arAIFlag[0]);i++) {
		string tempString = arAIFlag[i];
		for (int j=0; j<40 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

int get_Mob_RaceFlag_Value(string inputString)
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

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");
	for(int i =0;i<sizeof(arRaceFlag)/sizeof(arRaceFlag[0]);i++) {
		string tempString = arRaceFlag[i];
		for (int j=0; j<40 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}

int get_Mob_ImmuneFlag_Value(string inputString)
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

	int retValue = 0;
	string* arInputString = StringSplit(inputString, ",");
	for(int i =0;i<sizeof(arImmuneFlag)/sizeof(arImmuneFlag[0]);i++) {
		string tempString = arImmuneFlag[i];
		for (int j=0; j<40 ; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString)==0) {
				retValue = retValue + pow((float)2,(float)i);
			}

			if(tempString2.compare("") == 0)
				break;
		}
	}
	delete []arInputString;

	return retValue;
}