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
#define ENABLE_ATTBONUS_METIN
#define ENABLE_ATTBONUS_BOSS
#define ENABLE_MOUNT_SYSTEM
#define ENABLE_TITLE_SYSTEM
#define ENABLE_PET_SYSTEM

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
	int cutAt;
	int index = 0;
	string* strResult = new string[30];

	while ((cutAt = strOrigin.find_first_of(strTok)) != strOrigin.npos)
	{
		if (cutAt > 0)
		{
			strResult[index++] = strOrigin.substr(0, cutAt);
		}
		strOrigin = strOrigin.substr(cutAt+1);
	}

	if(strOrigin.length() > 0)
	{
		strResult[index++] = strOrigin.substr(0, cutAt);
	}

	for( int i=0;i<index;i++)
	{
		strResult[i] = trim(strResult[i]);
	}

	return strResult;
}

int get_Item_Type_Value(string inputString)
{
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
#ifdef ENABLE_TITLE_SYSTEM
		,"ITEM_TITLE"
#endif
	};

	int retInt = -1;
	for (int j = 0; j < sizeof(arType)/sizeof(arType[0]); j++)
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

int get_Item_SubType_Value(int type_value, string inputString) 
{
	string arSub1[] =
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

	string arSub2[] =
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

	string arSub3[] =
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
		"USE_PUT_INTO_BELT_SOCKET",
		"USE_PUT_INTO_RING_SOCKET"
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
	};

	string arSub4[] =
	{
		"AUTOUSE_POTION",
		"AUTOUSE_ABILITY_UP",
		"AUTOUSE_BOMB",
		"AUTOUSE_GOLD",
		"AUTOUSE_MONEYBAG",
		"AUTOUSE_TREASURE_BOX"
	};

	string arSub5[] =
	{
		"MATERIAL_LEATHER",
		"MATERIAL_BLOOD",
		"MATERIAL_ROOT",
		"MATERIAL_NEEDLE",
		"MATERIAL_JEWEL",
		"MATERIAL_DS_REFINE_NORMAL",
		"MATERIAL_DS_REFINE_BLESSED",
		"MATERIAL_DS_REFINE_HOLLY"
	};

	string arSub6[] =
	{
		"SPECIAL_MAP",
		"SPECIAL_KEY",
		"SPECIAL_DOC",
		"SPECIAL_SPIRIT"
	};

	string arSub7[] =
	{
		"TOOL_FISHING_ROD"
	};

	string arSub8[] =
	{
		"LOTTERY_TICKET",
		"LOTTERY_INSTANT"
	};

	string arSub10[] =
	{
		"METIN_NORMAL",
		"METIN_GOLD"
	};

	string arSub12[] =
	{
		"FISH_ALIVE",
		"FISH_DEAD"
	};

	string arSub14[] =
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

	string arSub16[] =
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

	string arSub28[] =
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

	string arSub29[] =
	{
		"DS_SLOT1",
		"DS_SLOT2",
		"DS_SLOT3",
		"DS_SLOT4",
		"DS_SLOT5",
		"DS_SLOT6"
	};

	string arSub31[] =
	{
		"EXTRACT_DRAGON_SOUL",
		"EXTRACT_DRAGON_HEART"
	};

#ifdef ENABLE_GROWTH_PET_SYSTEM
	string arSub36[] =
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

	string* arSubType[] =
	{
		0,	//0
		arSub1,		//1
		arSub2,	//2
		arSub3,	//3
		arSub4,	//4
		arSub5,	//5
		arSub6,	//6
		arSub7,	//7
		arSub8,	//8
		0,			//9
		arSub10,	//10
		0,			//11
		arSub12,	//12
		0,			//13
		arSub14,	//14
		0,			//15
		arSub16,	//16
		0,			//17
		0,			//18
		0,			//19
		0,			//20
		0,			//21
		0,			//22
		0,			//23
		0,			//24
		0,			//25
		0,			//26
		0,			//27
		arSub28,	//28
		arSub29,	//29
		arSub29,	//30
		arSub31,	//31
		0, // 32
		0, // 33
		0, // 34
#ifdef ENABLE_MOUNT_SYSTEM
		0, // 35
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
		arSub36,
#endif
#ifdef ENABLE_PET_SYSTEM
		0, // 37
#endif
#ifdef ENABLE_TITLE_SYSTEM
		0, // 38
#endif
	};

	int arNumberOfSubtype[39];
	arNumberOfSubtype[0] = 0;
	arNumberOfSubtype[1] = sizeof(arSub1)/sizeof(arSub1[0]);
	arNumberOfSubtype[2] = sizeof(arSub2)/sizeof(arSub2[0]);
	arNumberOfSubtype[3] = sizeof(arSub3)/sizeof(arSub3[0]);
	arNumberOfSubtype[4] = sizeof(arSub4)/sizeof(arSub4[0]);
	arNumberOfSubtype[5] = sizeof(arSub5)/sizeof(arSub5[0]);
	arNumberOfSubtype[6] = sizeof(arSub6)/sizeof(arSub6[0]);
	arNumberOfSubtype[7] = sizeof(arSub7)/sizeof(arSub7[0]);
	arNumberOfSubtype[8] = sizeof(arSub8)/sizeof(arSub8[0]);
	arNumberOfSubtype[9] = 0;
	arNumberOfSubtype[10] = sizeof(arSub10)/sizeof(arSub10[0]);
	arNumberOfSubtype[11] = 0;
	arNumberOfSubtype[12] = sizeof(arSub12)/sizeof(arSub12[0]);
	arNumberOfSubtype[13] = 0;
	arNumberOfSubtype[14] = sizeof(arSub14)/sizeof(arSub14[0]);
	arNumberOfSubtype[15] = 0;
	arNumberOfSubtype[16] = sizeof(arSub16)/sizeof(arSub16[0]);
	arNumberOfSubtype[17] = 0;
	arNumberOfSubtype[18] = 0;
	arNumberOfSubtype[19] = 0;
	arNumberOfSubtype[20] = 0;
	arNumberOfSubtype[21] = 0;
	arNumberOfSubtype[22] = 0;
	arNumberOfSubtype[23] = 0;
	arNumberOfSubtype[24] = 0;
	arNumberOfSubtype[25] = 0;
	arNumberOfSubtype[26] = 0;
	arNumberOfSubtype[27] = 0;
	arNumberOfSubtype[28] = sizeof(arSub28)/sizeof(arSub28[0]);
	arNumberOfSubtype[29] = sizeof(arSub29)/sizeof(arSub29[0]);
	arNumberOfSubtype[30] = sizeof(arSub29)/sizeof(arSub29[0]);
	arNumberOfSubtype[31] = sizeof(arSub31)/sizeof(arSub31[0]);
	arNumberOfSubtype[32] = 0;
	arNumberOfSubtype[33] = 0;
	arNumberOfSubtype[34] = 0;
#ifdef ENABLE_MOUNT_SYSTEM
	arNumberOfSubtype[35] = 0;
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	arNumberOfSubtype[36] = sizeof(arSub36) / sizeof(arSub36[0]);
#endif
#ifdef ENABLE_PET_SYSTEM
	arNumberOfSubtype[37] = 0;
#endif
#ifdef ENABLE_TITLE_SYSTEM
	arNumberOfSubtype[38] = 0;
#endif

	if (arSubType[type_value]==0)
	{
		return 0;
	}

	int retInt = -1;
	for (int j = 0; j < arNumberOfSubtype[type_value]; j++)
	{
		string tempString = arSubType[type_value][j];
		string tempInputString = trim(inputString);

		if (tempInputString.compare(tempString) == 0)
		{
			retInt = j;
			break;
		}
	}

	return retInt;
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

	for(int i = 0; i < sizeof(arAntiFlag)/sizeof(arAntiFlag[0]); i++)
	{
		string tempString = arAntiFlag[i];
		for (int j = 0; j < 30; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
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

	for(int i = 0; i < sizeof(arFlag)/sizeof(arFlag[0]); i++)
	{
		string tempString = arFlag[i];
		for (int j = 0; j < 30; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
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
#ifdef ENABLE_TITLE_SYSTEM
		,"WEAR_TITLE"
#endif
#ifdef ENABLE_PENDANT_SYSTEM
		,"WEAR_PENDANT"
#endif
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");

	for(int i = 0; i < sizeof(arWearrFlag)/sizeof(arWearrFlag[0]); i++)
	{
		string tempString = arWearrFlag[i];
		for (int j = 0; j < 30; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
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

	for(int i = 0; i < sizeof(arImmune)/sizeof(arImmune[0]); i++)
	{
		string tempString = arImmune[i];
		for (int j = 0; j < 30; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
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
	};

	int retInt = -1;
	for (int j = 0; j < sizeof(arLimitType)/sizeof(arLimitType[0]); j++)
	{
		string tempString = arLimitType[j];
		string tempInputString = trim(inputString);

		if (tempInputString.compare(tempString) == 0)
		{
			retInt = j;
			break;
		}
	}

	return retInt;
}

int get_Item_ApplyType_Value(string inputString)
{
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
#ifdef ENABLE_PENDANT_SYSTEM
		"APPLY_ATTBONUS_ELECT",
		"APPLY_ATTBONUS_FIRE",
		"APPLY_ATTBONUS_ICE",
		"APPLY_ATTBONUS_WIND",
		"APPLY_ATTBONUS_EARTH",
		"APPLY_ATTBONUS_DARK",
#endif
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		"APPLY_ACCEDRAIN_RATE",
#endif
#ifdef ENABLE_ATTBONUS_METIN
		"APPLY_ATTBONUS_METIN",
#endif
#ifdef ENABLE_ATTBONUS_BOSS
		"APPLY_ATTBONUS_BOSS",
#endif
	};

	int retInt = -1;
	for (int j = 0; j < sizeof(arApplyType)/sizeof(arApplyType[0]); j++)
	{
		string tempString = arApplyType[j];
		string tempInputString = trim(inputString);

		if (tempInputString.compare(tempString) == 0)
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

	for (int j = 0; j <sizeof(arRank)/sizeof(arRank[0]); j++)
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

	int retInt = -1;

	for (int j = 0; j < sizeof(arType)/sizeof(arType[0]); j++)
	{
		string tempString = arType[j];
		string tempInputString = trim(inputString);

		if (tempInputString.compare(tempString) == 0)
		{
			retInt = j;
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

	for (int j = 0; j < sizeof(arBattleType)/sizeof(arBattleType[0]); j++)
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

int get_Mob_Size_Value(string inputString)
{
	string arSize[] =
	{
		"SMALL",
		"MEDIUM",
		"BIG",
	};

	int retInt = 0;

	for (int j = 0; j < sizeof(arSize)/sizeof(arSize[0]); j++)
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

	for(int i = 0; i < sizeof(arAIFlag)/sizeof(arAIFlag[0]); i++)
	{
		string tempString = arAIFlag[i];
		for (int j = 0; j < 30; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
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

	for(int i = 0; i < sizeof(arRaceFlag)/sizeof(arRaceFlag[0]); i++)
	{
		string tempString = arRaceFlag[i];
		for (int j = 0; j < 30; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
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

	for(int i = 0; i < sizeof(arImmuneFlag)/sizeof(arImmuneFlag[0]); i++)
	{
		string tempString = arImmuneFlag[i];
		for (int j = 0; j < 30; j++)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0)
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
