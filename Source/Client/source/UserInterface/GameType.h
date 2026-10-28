#pragma once

#include "StdAfx.h"

#include "../GameLib/ItemData.h"

struct SAffects
{
	enum
	{
		AFFECT_MAX_NUM = 32,
	};

	SAffects() : dwAffects(0) {}
	SAffects(const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}
	int operator = (const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}

	BOOL IsAffect(BYTE byIndex)
	{
		return dwAffects & (1 << byIndex);
	}

	void __SetAffects(const DWORD & c_rAffects)
	{
		dwAffects = c_rAffects;
	}

	DWORD dwAffects;
};

extern std::string g_strGuildSymbolPathName;

const DWORD c_Name_Max_Length = 64;
const DWORD c_FileName_Max_Length = 128;
const DWORD c_Short_Name_Max_Length = 32;

const DWORD c_Inventory_Page_Size = 5*9;
const DWORD c_Inventory_Page_Count = 4;

#ifdef ENABLE_SPECIAL_INVENTORY
const DWORD c_Special_Inventory_Page_Size = 5*9;
const DWORD c_Special_Inventory_Page_Count = 3;
const DWORD c_Special_ItemSlot_Count = c_Special_Inventory_Page_Size * c_Special_Inventory_Page_Count;
#endif

#ifdef ENABLE_SOUL_ROULETTE_SYSTEM
const int ROULETTE_ITEM_MAX = 20;
#endif

const DWORD c_ItemSlot_Count = c_Inventory_Page_Size * c_Inventory_Page_Count;
const DWORD c_Equipment_Count = 12;

const DWORD c_Equipment_Start = c_ItemSlot_Count;

const DWORD c_Equipment_Body 	= c_Equipment_Start + CItemData::WEAR_BODY;
const DWORD c_Equipment_Head 	= c_Equipment_Start + CItemData::WEAR_HEAD;
const DWORD c_Equipment_Shoes 	= c_Equipment_Start + CItemData::WEAR_FOOTS;
const DWORD c_Equipment_Wrist 	= c_Equipment_Start + CItemData::WEAR_WRIST;
const DWORD c_Equipment_Weapon 	= c_Equipment_Start + CItemData::WEAR_WEAPON;
const DWORD c_Equipment_Neck 	= c_Equipment_Start + CItemData::WEAR_NECK;
const DWORD c_Equipment_Ear 	= c_Equipment_Start + CItemData::WEAR_EAR;
const DWORD c_Equipment_Unique1 = c_Equipment_Start + CItemData::WEAR_UNIQUE1;
const DWORD c_Equipment_Unique2 = c_Equipment_Start + CItemData::WEAR_UNIQUE2;
const DWORD c_Equipment_Arrow 	= c_Equipment_Start + CItemData::WEAR_ARROW;
const DWORD c_Equipment_Shield 	= c_Equipment_Start + CItemData::WEAR_SHIELD;

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	const DWORD c_New_Equipment_Start = c_Equipment_Start + CItemData::WEAR_RING1;
	const DWORD c_New_Equipment_Count = CItemData::WEAR_MAX_NUM - CItemData::WEAR_RING1;

	const DWORD c_Equipment_Ring1 = c_Equipment_Start + CItemData::WEAR_RING1;
	const DWORD c_Equipment_Ring2 = c_Equipment_Start + CItemData::WEAR_RING2;
	const DWORD c_Equipment_Belt = c_Equipment_Start + CItemData::WEAR_BELT;
#ifdef ENABLE_MOUNT_SYSTEM
	const DWORD c_Equipment_Mount = c_Equipment_Start + CItemData::WEAR_MOUNT;
#endif
#ifdef ENABLE_PET_SYSTEM
	const DWORD c_Equipment_Pet = c_Equipment_Start + CItemData::WEAR_PET;
#endif
#ifdef ENABLE_PENDANT_SYSTEM
	const DWORD c_Equipment_Pendant = c_Equipment_Start + CItemData::WEAR_PENDANT;
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
	const DWORD c_Equipment_Passive = c_Equipment_Start + CItemData::WEAR_PASSIVE;
#endif
#endif

#ifdef ENABLE_RIDING_EXTENDED
const int HORSE_MAX_LEVEL = 40;
#endif

#ifdef ENABLE_SKILL_COLOR_SYSTEM
enum ESkillColorLength
{
#ifdef ENABLE_NINETH_SKILL
	MAX_SKILL_COUNT = 9,
	MAX_NORMAL_SKILL_COUNT = 6,
#else
	MAX_SKILL_COUNT = 6,
#endif
	MAX_EFFECT_COUNT = 5,
#if defined(ENABLE_WOLFMAN_CHARACTER) && defined(ENABLE_NINETH_SKILL)
	MAX_BUFF_COUNT = 7,
#elif defined(ENABLE_WOLFMAN_CHARACTER) && !defined(ENABLE_NINETH_SKILL)
	MAX_BUFF_COUNT = 6,
#else
	MAX_BUFF_COUNT = 5,
#endif
	BUFF_BEGIN = MAX_SKILL_COUNT,
	MAX_COLOR_SLOTS = MAX_SKILL_COUNT + MAX_BUFF_COUNT,
};
#endif

enum EDragonSoulDeckType
{
	DS_DECK_1,
	DS_DECK_2,
	DS_DECK_MAX_NUM = 2,
};

enum EDragonSoulGradeTypes
{
	DRAGON_SOUL_GRADE_NORMAL,
	DRAGON_SOUL_GRADE_BRILLIANT,
	DRAGON_SOUL_GRADE_RARE,
	DRAGON_SOUL_GRADE_ANCIENT,
	DRAGON_SOUL_GRADE_LEGENDARY,
	DRAGON_SOUL_GRADE_MAX,
};

enum EDragonSoulStepTypes
{
	DRAGON_SOUL_STEP_LOWEST,
	DRAGON_SOUL_STEP_LOW,
	DRAGON_SOUL_STEP_MID,
	DRAGON_SOUL_STEP_HIGH,
	DRAGON_SOUL_STEP_HIGHEST,
	DRAGON_SOUL_STEP_MAX,
};

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
enum EAutomaticPickUP
{
	AUTOMATIC_PICK_UP_ACTIVATE = (1 << 0),

	AUTOMATIC_PICK_UP_WEAPON = (1 << 1),
	AUTOMATIC_PICK_UP_ARMOR = (1 << 2),
	AUTOMATIC_PICK_UP_SHIELD = (1 << 3),
	AUTOMATIC_PICK_UP_HELMETS = (1 << 4),
	AUTOMATIC_PICK_UP_BRACELETS = (1 << 5),
	AUTOMATIC_PICK_UP_NECKLACE = (1 << 6),
	AUTOMATIC_PICK_UP_EARRINGS = (1 << 7),
	AUTOMATIC_PICK_UP_SHOES = (1 << 8),
	AUTOMATIC_PICK_UP_OTHERS = (1 << 9),
	AUTOMATIC_PICK_UP_YANG = (1 << 10),
	AUTOMATIC_PICK_UP_CHESTS = (1 << 11),
};
#endif

#ifdef ENABLE_COSTUME_SYSTEM
	const DWORD c_Costume_Slot_Start = c_Equipment_Start + CItemData::WEAR_COSTUME_BODY;
	const DWORD	c_Costume_Slot_Body = c_Costume_Slot_Start + CItemData::COSTUME_BODY;
	const DWORD	c_Costume_Slot_Hair = c_Costume_Slot_Start + CItemData::COSTUME_HAIR;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Acce = c_Costume_Slot_Start + CItemData::COSTUME_ACCE;
#endif
#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Weapon = c_Costume_Slot_Start + CItemData::COSTUME_WEAPON;
#endif
#ifdef ENABLE_MOUNT_PET_SKIN
	const DWORD c_Costume_Slot_Mount = c_Costume_Slot_Start + CItemData::COSTUME_MOUNT;
	const DWORD c_Costume_Slot_Pet = c_Costume_Slot_Start + CItemData::COSTUME_PET;
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	const DWORD c_Costume_Slot_Aura = c_Costume_Slot_Start + CItemData::COSTUME_AURA;
#endif

	const DWORD c_Costume_Slot_Count = CItemData::COSTUME_NUM_TYPES;
	const DWORD c_Costume_Slot_End = c_Costume_Slot_Start + c_Costume_Slot_Count;
#endif

const DWORD c_Wear_Max = 64;
const DWORD c_DragonSoul_Equip_Start = c_ItemSlot_Count + c_Wear_Max;
#ifdef ENABLE_DRAGON_SOUL_7_SLOT
const DWORD c_DragonSoul_Equip_Slot_Max = 7;
#else
const DWORD c_DragonSoul_Equip_Slot_Max = 6;
#endif
const DWORD c_DragonSoul_Equip_End = c_DragonSoul_Equip_Start + c_DragonSoul_Equip_Slot_Max * DS_DECK_MAX_NUM;

const DWORD c_DragonSoul_Equip_Reserved_Count = c_DragonSoul_Equip_Slot_Max * 3;

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	const DWORD c_Belt_Inventory_Slot_Start = c_DragonSoul_Equip_End + c_DragonSoul_Equip_Reserved_Count;
	const DWORD c_Belt_Inventory_Width = 7;	// reference belt inventory: 7x7 (was 5x5)
	const DWORD c_Belt_Inventory_Height= 7;
	const DWORD c_Belt_Inventory_Slot_Count = c_Belt_Inventory_Width * c_Belt_Inventory_Height;
	const DWORD c_Belt_Inventory_Slot_End = c_Belt_Inventory_Slot_Start + c_Belt_Inventory_Slot_Count;

#ifdef ENABLE_SPECIAL_INVENTORY
	const DWORD c_Skill_Book_Inventory_Slot_Start = c_Belt_Inventory_Slot_End;
	const DWORD c_Skill_Book_Inventory_Slot_Count = c_Special_ItemSlot_Count;
	const DWORD c_Skill_Book_Inventory_Slot_End = c_Skill_Book_Inventory_Slot_Start + c_Skill_Book_Inventory_Slot_Count;

	const DWORD c_Upgrade_Items_Inventory_Slot_Start = c_Skill_Book_Inventory_Slot_End;
	const DWORD c_Upgrade_Items_Inventory_Slot_Count = c_Special_ItemSlot_Count;
	const DWORD c_Upgrade_Items_Inventory_Slot_End = c_Upgrade_Items_Inventory_Slot_Start + c_Upgrade_Items_Inventory_Slot_Count;

	const DWORD c_Stone_Inventory_Slot_Start = c_Upgrade_Items_Inventory_Slot_End;
	const DWORD c_Stone_Inventory_Slot_Count = c_Special_ItemSlot_Count;
	const DWORD c_Stone_Inventory_Slot_End = c_Stone_Inventory_Slot_Start + c_Stone_Inventory_Slot_Count;

	const DWORD c_GiftBox_Inventory_Slot_Start = c_Stone_Inventory_Slot_End;
	const DWORD c_GiftBox_Inventory_Slot_Count = c_Special_ItemSlot_Count;
	const DWORD c_GiftBox_Inventory_Slot_End = c_GiftBox_Inventory_Slot_Start + c_GiftBox_Inventory_Slot_Count;

	const DWORD c_Changers_Inventory_Slot_Start = c_GiftBox_Inventory_Slot_End;
	const DWORD c_Changers_Inventory_Slot_Count = c_Special_ItemSlot_Count;
	const DWORD c_Changers_Inventory_Slot_End = c_Changers_Inventory_Slot_Start + c_Changers_Inventory_Slot_Count;
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	const DWORD c_Inventory_Count = c_Changers_Inventory_Slot_End;
#else
	const DWORD c_Inventory_Count = c_Belt_Inventory_Slot_End;
#endif
#else
	const DWORD c_Inventory_Count = c_DragonSoul_Equip_End;
#endif

const DWORD c_DragonSoul_Inventory_Start = 0;
const DWORD c_DragonSoul_Inventory_Box_Size = 32;
const DWORD c_DragonSoul_Inventory_Count = CItemData::DS_SLOT_NUM_TYPES * DRAGON_SOUL_GRADE_MAX * c_DragonSoul_Inventory_Box_Size;
const DWORD c_DragonSoul_Inventory_End = c_DragonSoul_Inventory_Start + c_DragonSoul_Inventory_Count;

#ifdef ENABLE_SLOT_MARKING_SYSTEM
enum ETopWindowTypes
{
	ON_TOP_WND_NONE,
	ON_TOP_WND_SHOP,
	ON_TOP_WND_EXCHANGE,
	ON_TOP_WND_SAFEBOX,
	ON_TOP_WND_PRIVATE_SHOP,
	ON_TOP_WND_ITEM_COMB,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	ON_TOP_WND_ACCE_COMBINE,
	ON_TOP_WND_ACCE_ABSORB,
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	ON_TOP_WND_PET_FEED,
	ON_TOP_WND_PET_ATTR_CHANGE,
	ON_TOP_WND_PET_PRIMIUM_FEEDSTUFF,
#endif
	ON_TOP_WND_MAX,
};
#endif

enum ESlotType
{
	SLOT_TYPE_NONE,
	SLOT_TYPE_INVENTORY,
	SLOT_TYPE_SKILL,
	SLOT_TYPE_EMOTION,
	SLOT_TYPE_SHOP,
	SLOT_TYPE_EXCHANGE_OWNER,
	SLOT_TYPE_EXCHANGE_TARGET,
	SLOT_TYPE_QUICK_SLOT,
	SLOT_TYPE_SAFEBOX,
	SLOT_TYPE_PRIVATE_SHOP,
	SLOT_TYPE_MALL,
	SLOT_TYPE_DRAGON_SOUL_INVENTORY,
	SLOT_TYPE_BELT_INVENTORY,
#ifdef ENABLE_RENEWAL_SWITCHBOT
	SLOT_TYPE_SWITCHBOT,
#endif
#ifdef ENABLE_SPECIAL_INVENTORY
	SLOT_TYPE_SKILL_BOOK_INVENTORY,
	SLOT_TYPE_UPGRADE_ITEMS_INVENTORY,
	SLOT_TYPE_STONE_INVENTORY,
	SLOT_TYPE_GIFT_BOX_INVENTORY,
	SLOT_TYPE_CHANGERS_INVENTORY,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	SLOT_TYPE_AURA,
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	SLOT_TYPE_CHANGE_LOOK,
#endif
#ifdef ENABLE_FISH_EVENT_SYSTEM
	SLOT_TYPE_FISH_EVENT,
#endif
	SLOT_TYPE_YIN_AND_YANG_INVENTORY,
#ifdef ENABLE_SHOP_REPURCHASE
	SLOT_TYPE_SHOP_REPURCHASE = 31,
#endif
	SLOT_TYPE_MAX,
};

#ifdef ENABLE_AURA_COSTUME_SYSTEM
const BYTE c_AuraMaxLevel = 250;

enum EAuraRefineInfoSlot
{
	AURA_REFINE_INFO_SLOT_CURRENT,
	AURA_REFINE_INFO_SLOT_NEXT,
	AURA_REFINE_INFO_SLOT_EVOLVED,
	AURA_REFINE_INFO_SLOT_MAX
};

enum EAuraWindowType
{
	AURA_WINDOW_TYPE_ABSORB,
	AURA_WINDOW_TYPE_GROWTH,
	AURA_WINDOW_TYPE_EVOLVE,
	AURA_WINDOW_TYPE_MAX,
};

enum EAuraSlotType
{
	AURA_SLOT_MAIN,
	AURA_SLOT_SUB,
	AURA_SLOT_RESULT,
	AURA_SLOT_MAX
};

enum EAuraRefineInfoType
{
	AURA_REFINE_INFO_STEP,
	AURA_REFINE_INFO_LEVEL_MIN,
	AURA_REFINE_INFO_LEVEL_MAX,
	AURA_REFINE_INFO_NEED_EXP,
	AURA_REFINE_INFO_MATERIAL_VNUM,
	AURA_REFINE_INFO_MATERIAL_COUNT,
	AURA_REFINE_INFO_NEED_GOLD,
	AURA_REFINE_INFO_EVOLVE_PCT,
	AURA_REFINE_INFO_MAX
};
#endif

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,
#ifdef ENABLE_RENEWAL_SWITCHBOT
	SWITCHBOT,
#endif
#ifdef ENABLE_SPECIAL_INVENTORY
	SKILL_BOOK_INVENTORY,
	UPGRADE_ITEMS_INVENTORY,
	STONE_INVENTORY,
	GIFT_BOX_INVENTORY,
	CHANGERS_INVENTORY,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	AURA_REFINE,
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
	PET_FEED,
#endif
	GROUND,
#ifdef ENABLE_SHOP_REPURCHASE
	SHOP_REPURCHASE = 28,
#endif

	WINDOW_TYPE_MAX,
};

#ifdef ENABLE_FISH_EVENT_SYSTEM
enum EFishEventInfo
{
	FISH_EVENT_SHAPE_NONE,
	FISH_EVENT_SHAPE_1,
	FISH_EVENT_SHAPE_2,
	FISH_EVENT_SHAPE_3,
	FISH_EVENT_SHAPE_4,
	FISH_EVENT_SHAPE_5,
	FISH_EVENT_SHAPE_6,
	FISH_EVENT_SHAPE_7,
	FISH_EVENT_SHAPE_MAX_NUM,
};
#endif

enum EDSInventoryMaxNum
{
	DS_INVENTORY_MAX_NUM = c_DragonSoul_Inventory_Count,
	DS_REFINE_WINDOW_MAX_NUM = 15,
};

#pragma pack (push, 1)
#define WORD_MAX 0xffff

#ifdef ENABLE_RENEWAL_SWITCHBOT
enum ESwitchbotValues
{
	SWITCHBOT_SLOT_COUNT = 5,
	SWITCHBOT_ALTERNATIVE_COUNT = 2,
	MAX_NORM_ATTR_NUM = 5,
};

enum EAttributeSet
{
	ATTRIBUTE_SET_WEAPON,
	ATTRIBUTE_SET_BODY,
	ATTRIBUTE_SET_WRIST,
	ATTRIBUTE_SET_FOOTS,
	ATTRIBUTE_SET_NECK,
	ATTRIBUTE_SET_HEAD,
	ATTRIBUTE_SET_SHIELD,
	ATTRIBUTE_SET_EAR,
#ifdef ENABLE_PENDANT_SYSTEM
	ATTRIBUTE_SET_PENDANT,
#endif
	ATTRIBUTE_SET_MAX_NUM,
};
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
enum EPet
{
	PET_FEED_SLOT_MAX = 9,
	PET_GROWTH_EVOL_MAX = 4,
	PET_GROWTH_SKILL_LEVEL_MAX = 20,
	PET_GROWTH_SKILL_OPEN_EVOL_LEVEL = 4,
	PET_REVIVE_MATERIAL_SLOT_MAX = 10,
	PET_SKILL_COUNT_MAX = 3,
	LIFE_TIME_FLASH_MIN_TIME = 3600,
	ITEM_SLOT_COUNT = 180,
	SPECIAL_EVOL_MIN_AGE = 2592000,
	PET_ATTR_DETERMINE_ITEM = 55032,
	PET_ATTR_CHANGE_ITEM = 55033,
};

enum EPetButtonEvent
{
	FEED_LIFE_TIME_EVENT,
	FEED_EVOL_EVENT,
	FEED_EXP_EVENT,
	FEED_BUTTON_MAX,
};

enum EPetWindowType
{
	PET_WINDOW_INFO,
	PET_WINDOW_ATTR_CHANGE,
	PET_WINDOW_PRIMIUM_FEEDSTUFF,
};

enum EPetAttrChange
{
	PET_WND_SLOT_ATTR_CHANGE,
	PET_WND_SLOT_ATTR_CHANGE_ITEM,
	PET_WND_SLOT_ATTR_CHANGE_RESULT,
	PET_WND_SLOT_ATTR_CHANGE_MAX,
};

enum EQuickSlotError
{
	QUICK_SLOT_POS_ERROR,
	QUICK_SLOT_ITEM_USE_SUCCESS,
	QUICK_SLOT_IS_NOT_ITEM,
	QUICK_SLOT_PET_ITEM_USE_SUCCESS,
	QUICK_SLOT_PET_ITEM_USE_FAILED,
	QUICK_SLOT_CAN_NOT_USE_PET_ITEM,
};
#endif

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
	SItemPos ()
	{
		window_type = INVENTORY;
		cell = WORD_MAX;
	}
	SItemPos (BYTE _window_type, WORD _cell)
	{
		window_type = _window_type;
		cell = _cell;
	}

	bool IsValidCell() const
	{
		switch (window_type)
		{
			case INVENTORY:
				return cell < c_Inventory_Count;
				break;

			case EQUIPMENT:
				return cell < c_DragonSoul_Equip_End;
				break;

			case DRAGON_SOUL_INVENTORY:
				return cell < (DS_INVENTORY_MAX_NUM);
				break;

#ifdef ENABLE_RENEWAL_SWITCHBOT
			case SWITCHBOT:
				return cell < SWITCHBOT_SLOT_COUNT;
				break;
#endif

#ifdef ENABLE_SHOP_REPURCHASE
			case SHOP_REPURCHASE:
				return cell < 12;
				break;
#endif
			default:
				return false;
		}
	}

	bool IsEquipCell() const
	{
		switch (window_type)
		{
			case INVENTORY:
			case EQUIPMENT:
				return (c_Equipment_Start + c_Wear_Max > cell) && (c_Equipment_Start <= cell);
				break;

			case BELT_INVENTORY:
			case DRAGON_SOUL_INVENTORY:
				return false;
				break;

			default:
				return false;
		}
	}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	bool IsBeltInventoryCell()
	{
		bool bResult = c_Belt_Inventory_Slot_Start <= cell && c_Belt_Inventory_Slot_End > cell;
		return bResult;
	}
#endif

#ifdef ENABLE_SPECIAL_INVENTORY
	bool IsSkillBookInventoryCell()
	{
		bool bResult = c_Skill_Book_Inventory_Slot_Start <= cell && c_Skill_Book_Inventory_Slot_End > cell;
		return bResult;
	}

	bool IsUpgradeItemsInventoryCell()
	{
		bool bResult = c_Upgrade_Items_Inventory_Slot_Start <= cell && c_Upgrade_Items_Inventory_Slot_End > cell;
		return bResult;
	}

	bool IsStoneInventoryCell()
	{
		bool bResult = c_Stone_Inventory_Slot_Start <= cell && c_Stone_Inventory_Slot_End > cell;
		return bResult;
	}

	bool IsGiftBoxInventoryCell()
	{
		bool bResult = c_GiftBox_Inventory_Slot_Start <= cell && c_GiftBox_Inventory_Slot_End > cell;
		return bResult;
	}

	bool IsChangersInventoryCell()
	{
		bool bResult = c_Changers_Inventory_Slot_Start <= cell && c_Changers_Inventory_Slot_End > cell;
		return bResult;
	}
#endif

	bool IsNPOS()
	{
		return (window_type == RESERVED_WINDOW && cell == WORD_MAX);
	}

	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}

	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;
const TItemPos NPOS(RESERVED_WINDOW, WORD_MAX);

#pragma pack(pop)

const DWORD c_QuickBar_Line_Count = 3;
const DWORD c_QuickBar_Slot_Count = 12;

const float c_Idle_WaitTime = 5.0f;

const int c_Monster_Race_Start_Number = 6;
const int c_Monster_Model_Start_Number = 20001;

const float c_fAttack_Delay_Time = 0.2f;
const float c_fHit_Delay_Time = 0.1f;
const float c_fCrash_Wave_Time = 0.2f;
const float c_fCrash_Wave_Distance = 3.0f;

const float c_fHeight_Step_Distance = 50.0f;

enum
{
	DISTANCE_TYPE_FOUR_WAY,
	DISTANCE_TYPE_EIGHT_WAY,
	DISTANCE_TYPE_ONE_WAY,
	DISTANCE_TYPE_MAX_NUM,
};

const float c_fMagic_Script_Version = 1.0f;
const float c_fSkill_Script_Version = 1.0f;
const float c_fMagicSoundInformation_Version = 1.0f;
const float c_fBattleCommand_Script_Version = 1.0f;
const float c_fEmotionCommand_Script_Version = 1.0f;
const float c_fActive_Script_Version = 1.0f;
const float c_fPassive_Script_Version = 1.0f;

// Used by PushMove
#ifdef ENABLE_FISH_EVENT_SYSTEM
const float c_fWalkDistance = 175.0f;
const float c_fRunDistance = 310.0f;
#endif

#define FILE_MAX_LEN 128

enum
{
	ITEM_SOCKET_SLOT_MAX_NUM = 3,

	ITEM_ATTRIBUTE_SLOT_NORM_NUM = 5,
	ITEM_ATTRIBUTE_SLOT_RARE_NUM = 2,

	ITEM_ATTRIBUTE_SLOT_NORM_START = 0,
	ITEM_ATTRIBUTE_SLOT_NORM_END = ITEM_ATTRIBUTE_SLOT_NORM_START + ITEM_ATTRIBUTE_SLOT_NORM_NUM,

	ITEM_ATTRIBUTE_SLOT_RARE_START = ITEM_ATTRIBUTE_SLOT_NORM_END,
	ITEM_ATTRIBUTE_SLOT_RARE_END = ITEM_ATTRIBUTE_SLOT_RARE_START + ITEM_ATTRIBUTE_SLOT_RARE_NUM,

	ITEM_ATTRIBUTE_SLOT_MAX_NUM = ITEM_ATTRIBUTE_SLOT_RARE_END,
};

#ifdef ENABLE_RENEWAL_SHOPEX
enum STableExTypes
{
	SHOPEX_GOLD = 1,
	SHOPEX_COINS,
	SHOPEX_ITEM,
	SHOPEX_EXP,
	SHOPEX_MAX,
};
#endif

#pragma pack(push)
#pragma pack(1)

typedef struct SQuickSlot
{
	BYTE Type;
	BYTE Position;
} TQuickSlot;

typedef struct TPlayerItemAttribute
{
	WORD bType;
	short sValue;
} TPlayerItemAttribute;

typedef struct packet_item
{
	DWORD vnum;
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
	DWORD flags;
	DWORD anti_flags;
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
#ifdef ENABLE_SUNGMA_SCORE
	DWORD dwSungmaScore;
#endif
#ifdef ENABLE_SET_ITEM
	DWORD dwSetValue;
#endif
#ifdef ENABLE_SEALBIND_SYSTEM
	long sealbind;
#endif
} TItemData;

typedef struct packet_shop_item
{
	DWORD vnum;
#ifdef ENABLE_GOLD_LIMIT
	long long price;
#else
	DWORD price;
#endif
#ifdef ENABLE_STACK_LIMIT
	WORD count;
#else
	BYTE count;
#endif
	BYTE display_pos;
	long alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef ENABLE_RENEWAL_SHOPEX
	BYTE price_type;
	DWORD price_vnum;
#endif
#ifdef ENABLE_CHANGE_LOOK_SYSTEM
	DWORD dwTransmutationVnum;
#endif
} TShopItemData;

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
enum class ETRANSMUTATIONTYPE : BYTE
{
	TRANSMUTATION_TYPE_MOUNT,
	TRANSMUTATION_TYPE_ITEM
};

enum class ETRANSMUTATIONSLOTTYPE : size_t
{
	TRANSMUTATION_SLOT_LEFT,
	TRANSMUTATION_SLOT_RIGHT,

	TRANSMUTATION_SLOT_MAX
};

enum class ETRANSMUTATIONSETTINGS : DWORD
{
	TRANSMUTATION_ITEM_PRICE = 50000000, // 50M
	TRANSMUTATION_MOUNT_PRICE = 30000000, // 30M
	TRANSMUTATION_TICKET_1 = 72326,
	TRANSMUTATION_TICKET_2 = 72341,
	TRANSMUTATION_CLEAR_SCROLL = 72325,
};
#endif

#pragma pack(pop)

inline float GetSqrtDistance(int ix1, int iy1, int ix2, int iy2) // By sqrt
{
	float dx, dy;

	dx = float(ix1 - ix2);
	dy = float(iy1 - iy2);

	return sqrtf(dx*dx + dy*dy);
}

// DEFAULT_FONT
void DefaultFont_Startup();
void DefaultFont_Cleanup();
void DefaultFont_SetName(const char *c_szFontName);
CResource *DefaultFont_GetResource();
#ifdef ENABLE_FISH_EVENT_SYSTEM
CResource *DefaultItalicFont_GetResource();
#endif
// END_OF_DEFAULT_FONT

void SetGuildSymbolPath(const char *c_szPathName);
const char *GetGuildSymbolFileName(DWORD dwGuildID);
BYTE SlotTypeToInvenType(BYTE bSlotType);
#ifdef ENABLE_AURA_COSTUME_SYSTEM
int *GetAuraRefineInfo(BYTE bLevel);
#endif
