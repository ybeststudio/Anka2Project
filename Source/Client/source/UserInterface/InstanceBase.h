#pragma once

#include "StdAfx.h"

#include "../gamelib/RaceData.h"
#include "../gamelib/ActorInstance.h"

#include "AffectFlagContainer.h"

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	#include "../eterlib/GrpObjectInstance.h"
#endif

class CInstanceBase
{
	public:
		struct SCreateData
		{
			BYTE m_bType;
			DWORD m_dwStateFlags;
			DWORD m_dwEmpireID;
			DWORD m_dwGuildID;
			DWORD m_dwLevel;
#ifdef ENABLE_CONQUEROR_LEVEL
			DWORD m_dwConquerorLevel;
#endif
#ifdef ENABLE_SHOW_MOB_INFO
			DWORD m_dwAIFlag;
#endif
			DWORD m_dwVID;
			DWORD m_dwRace;
			DWORD m_dwMovSpd;
			DWORD m_dwAtkSpd;
			LONG m_lPosX;
			LONG m_lPosY;
			FLOAT m_fRot;
			DWORD m_dwArmor;
			DWORD m_dwWeapon;
			DWORD m_dwHair;
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			DWORD m_dwAcce;
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			DWORD m_dwAura;
#endif
			DWORD m_dwMountVnum;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
			BYTE m_dwGuildLeader;
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			BYTE m_bLanguage;
			BYTE m_bLanguage2;
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
			DWORD m_dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
			short m_sAlignment;

			BYTE m_byPKMode;
			CAffectFlagContainer m_kAffectFlags;

			std::string m_stName;

#ifdef ENABLE_GROWTH_PET_SYSTEM
			BYTE m_bCharacterSize;
#endif

			bool m_isMain;
		};

	public:
		typedef DWORD TType;

#ifdef ENABLE_GROWTH_PET_SYSTEM
		enum EPetSkill
		{
			PET_SKILL_AFFECT_NONE,
			PET_SKILL_AFFECT_JIJOONG_WARRIOR,
			PET_SKILL_AFFECT_JIJOONG_SURA,
			PET_SKILL_AFFECT_JIJOONG_ASSASSIN,
			PET_SKILL_AFFECT_JIJOONG_SHAMAN,
			PET_SKILL_AFFECT_PACHEON,
			PET_SKILL_AFFECT_BANYA,
			PET_SKILL_AFFECT_CHOEHOENBIMU,
			PET_SKILL_AFFECT_HEAL,
			PET_SKILL_AFFECT_STEALHP,
			PET_SKILL_AFFECT_STEALMP,
			PET_SKILL_AFFECT_BLOCK,
			PET_SKILL_AFFECT_REFLECT_MELEE,
			PET_SKILL_AFFECT_GOLD_DROP,
			PET_SKILL_AFFECT_BOW_DISTANCE,
			PET_SKILL_AFFECT_INVINCIBILITY,
			PET_SKILL_AFFECT_REMOVAL,
			PET_SKILL_AFFECT_POTION,
			PET_SKILL_AFFECT_MOB_BONUS,
			PET_SKILL_HP_RECOVER,
			PET_SKILL_AFFECT_EXP,
			PET_SKILL_FEATHER,

			PET_SKILL_AFFECT_MAX,
		};
#endif

		enum EDirection
		{
			DIR_NORTH,
			DIR_NORTHEAST,
			DIR_EAST,
			DIR_SOUTHEAST,
			DIR_SOUTH,
			DIR_SOUTHWEST,
			DIR_WEST,
			DIR_NORTHWEST,
			DIR_MAX_NUM,
		};

		enum
		{
			FUNC_WAIT,
			FUNC_MOVE,
			FUNC_ATTACK,
			FUNC_COMBO,
			FUNC_MOB_SKILL,
			FUNC_EMOTION,
			FUNC_SKILL = 0x80,
		};

		enum eFlyTypes
		{
			FLY_NONE,
			FLY_EXP,
			FLY_HP_MEDIUM,
			FLY_HP_BIG,
			FLY_SP_SMALL,
			FLY_SP_MEDIUM,
			FLY_SP_BIG,
			FLY_FIREWORK1,
			FLY_FIREWORK2,
			FLY_FIREWORK3,
			FLY_FIREWORK4,
			FLY_FIREWORK5,
			FLY_FIREWORK6,
			FLY_FIREWORK_XMAS,
			FLY_CHAIN_LIGHTNING,
			FLY_HP_SMALL,
			FLY_SKILL_MUYEONG,
#ifdef ENABLE_CONQUEROR_LEVEL
			FLY_CONQUEROR_EXP,
#endif
		};

		enum
		{
			AFFECT_YMIR,						// 0
			AFFECT_INVISIBILITY,				// 1
			AFFECT_SPAWN,						// 2
			AFFECT_POISON,						// 3
			AFFECT_SLOW,						// 4
			AFFECT_STUN,						// 5
			AFFECT_DUNGEON_READY,				// 6
			AFFECT_SHOW_ALWAYS,					// 7
			AFFECT_BUILDING_CONSTRUCTION_SMALL,	// 8
			AFFECT_BUILDING_CONSTRUCTION_LARGE,	// 9
			AFFECT_BUILDING_UPGRADE,			// 10
			AFFECT_MOV_SPEED_POTION,			// 11
			AFFECT_ATT_SPEED_POTION,			// 12
			AFFECT_FISH_MIND,					// 13
			AFFECT_JEONGWI,						// 14
			AFFECT_GEOMGYEONG,					// 15
			AFFECT_CHEONGEUN,					// 16
			AFFECT_GYEONGGONG,					// 17
			AFFECT_EUNHYEONG,					// 18
			AFFECT_GWIGEOM,						// 19
			AFFECT_GONGPO,						// 20
			AFFECT_JUMAGAP,						// 21
			AFFECT_HOSIN,						// 22
			AFFECT_BOHO,						// 23
			AFFECT_KWAESOK,						// 24
			AFFECT_HEUKSIN,						// 25
			AFFECT_MUYEONG,						// 26
			AFFECT_REVIVE_INVISIBILITY,			// 27
			AFFECT_FIRE,						// 28
			AFFECT_GICHEON,						// 29
			AFFECT_JEUNGRYEOK,					// 30
			AFFECT_DASH,						// 31
			AFFECT_PABEOP,						// 32
			AFFECT_FALLEN_CHEONGEUN,			// 33
			AFFECT_POLYMORPH,					// 34
			AFFECT_WAR_FLAG1,					// 35
			AFFECT_WAR_FLAG2,					// 36
			AFFECT_WAR_FLAG3,					// 37
			AFFECT_CHINA_FIREWORK,				// 38
			AFFECT_PREMIUM_SILVER,				// 39
			AFFECT_PREMIUM_GOLD,				// 40
			AFFECT_RAMADAN_RING,				// 41
#ifdef ENABLE_RENEWAL_TEAM_AFFECT
			AFFECT_TEAM_SA,						// 42
			AFFECT_TEAM_GA,						// 43
			AFFECT_TEAM_GM,						// 44
			AFFECT_TEAM_TGM,					// 45
#endif
#ifdef __AUTO_HUNT__
			AFFECT_AUTO_HUNT,					// 47
#endif
#ifdef ENABLE_MELEY_LAIR_DUNGEON
			AFFECT_STATUE1,						// 48
			AFFECT_STATUE2,						// 49
			AFFECT_STATUE3,						// 50
			AFFECT_STATUE4,						// 51
#endif
#ifdef ENABLE_EQUIPMENT_HAND_EFFECT
			AFF_DBONE_BRONZE,					// 52
			AFF_DBONE_SILVER,					// 53
			AFF_DBONE_GOLD,						// 54
			AFF_DBONE_DIAMOND,					// 55
			AFF_DBONE_PLATIN,					// 56
			AFF_DBONE_EMERALD,					// 57
#endif
#ifdef ENABLE_NINETH_SKILL
			AFFECT_CHEONUN,						// 58
			AFFECT_CHUNWOON,					// 59
#endif

			AFFECT_NUM = 64,

			AFFECT_HWAYEOM = AFFECT_GEOMGYEONG,
		};

		enum
		{
			NEW_AFFECT_MOV_SPEED = 200,
			NEW_AFFECT_ATT_SPEED,
			NEW_AFFECT_ATT_GRADE,
			NEW_AFFECT_INVISIBILITY,
			NEW_AFFECT_STR,
			NEW_AFFECT_DEX,                 // 205
			NEW_AFFECT_CON,
			NEW_AFFECT_INT,
			NEW_AFFECT_FISH_MIND_PILL,
			NEW_AFFECT_POISON,
			NEW_AFFECT_STUN,                // 210
			NEW_AFFECT_SLOW,
			NEW_AFFECT_DUNGEON_READY,
			NEW_AFFECT_DUNGEON_UNIQUE,
			NEW_AFFECT_BUILDING,
			NEW_AFFECT_REVIVE_INVISIBLE,    // 215
			NEW_AFFECT_FIRE,
			NEW_AFFECT_CAST_SPEED,
			NEW_AFFECT_HP_RECOVER_CONTINUE,
			NEW_AFFECT_SP_RECOVER_CONTINUE,
			NEW_AFFECT_POLYMORPH,           // 220
			NEW_AFFECT_MOUNT,
			NEW_AFFECT_WAR_FLAG,            // 222
			NEW_AFFECT_BLOCK_CHAT,          // 223
			NEW_AFFECT_CHINA_FIREWORK,
			NEW_AFFECT_BOW_DISTANCE,        // 225

			NEW_AFFECT_RAMADAN_ABILITY = 300,
			NEW_AFFECT_RAMADAN_RING = 301,
			NEW_AFFECT_NOG_POCKET_ABILITY = 302,
			NEW_AFFECT_EXP_BONUS = 500,
			NEW_AFFECT_ITEM_BONUS = 501,
			NEW_AFFECT_SAFEBOX = 502,
			NEW_AFFECT_AUTOLOOT = 503,
			NEW_AFFECT_FISH_MIND = 504,
			NEW_AFFECT_MARRIAGE_FAST = 505,
			NEW_AFFECT_GOLD_BONUS = 506,
			NEW_AFFECT_MALL = 510,
			NEW_AFFECT_NO_DEATH_PENALTY = 511,
			NEW_AFFECT_SKILL_BOOK_BONUS = 512,
			NEW_AFFECT_SKILL_BOOK_NO_DELAY = 513,
			NEW_AFFECT_EXP_BONUS_EURO_FREE = 516,
			NEW_AFFECT_EXP_BONUS_EURO_FREE_UNDER_15 = 517,
			NEW_AFFECT_AUTO_HP_RECOVERY = 534,
			NEW_AFFECT_AUTO_SP_RECOVERY = 535,
			NEW_AFFECT_DRAGON_SOUL_QUALIFIED = 540,
			NEW_AFFECT_DRAGON_SOUL_DECK1 = 541,
			NEW_AFFECT_DRAGON_SOUL_DECK2 = 542,
#ifdef ENABLE_RENEWAL_AFFECT
			NEW_AFFECT_BLEND = 548,
			NEW_AFFECT_MALL_PLUS = 549,
			NEW_AFFECT_BLEND_PLUS = 550,
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
			NEW_AFFECT_EXP_BONUS_EVENT = 557,
			NEW_AFFECT_ATT_SPEED_SLOW = 558,
#endif

#ifdef ENABLE_EQUIPMENT_HAND_EFFECT
			AFFECT_DBONE_BRONZE = 590,
			AFFECT_DBONE_SILVER = 591,
#endif
#ifdef ENABLE_EQUIPMENT_HAND_EFFECT
		AFFECT_DBONE_GOLD = 602,
		AFFECT_DBONE_DIAMOND = 603,
		AFFECT_DBONE_PLATIN = 604,
		AFFECT_DBONE_EMERALD = 605,
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
			NEW_AFFECT_DECORATION = 703,
#endif
#ifdef ENABLE_MULTI_FARM_BLOCK
			NEW_AFFECT_MULTI_FARM = 704,
#endif
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
			NEW_AFFECT_AUTO_PICK_UP = 706,
#endif
#ifdef ENABLE_QUEEN_NETHIS
			NEW_AFFECT_SUNGMA_MOVE_DEBUFF = 815,
			NEW_AFFECT_SUNGMA_STR_DEBUFF = 816,
#endif
#ifdef __AUTO_HUNT__
			NEW_AFFECT_AUTO_HUNT = 830,
#endif
#ifdef ENABLE_SUNG_MAHI_TOWER
			NEW_AFFECT_SUNG_MAHI_BP,
			NEW_AFFECT_SUNG_MAHI_CURSE,
#endif
			NEW_AFFECT_QUEST_START_IDX = 1000,
		};

		enum
		{
			STONE_SMOKE1 = 0, // 99%
			STONE_SMOKE2 = 1, // 85%
			STONE_SMOKE3 = 2, // 80%
			STONE_SMOKE4 = 3, // 60%
			STONE_SMOKE5 = 4, // 45%
			STONE_SMOKE6 = 5, // 40%
			STONE_SMOKE7 = 6, // 20%
			STONE_SMOKE8 = 7, // 10%
			STONE_SMOKE_NUM = 4,
		};

		enum EBuildingAffect
		{
			BUILDING_CONSTRUCTION_SMALL = 0,
			BUILDING_CONSTRUCTION_LARGE = 1,
			BUILDING_UPGRADE = 2,
		};

		enum
		{
			WEAPON_DUALHAND,
			WEAPON_ONEHAND,
			WEAPON_TWOHAND,
			WEAPON_NUM,
		};

		enum
		{
			EMPIRE_NONE,
			EMPIRE_A,
			EMPIRE_B,
			EMPIRE_C,
			EMPIRE_NUM,
		};

		enum
		{
			NAMECOLOR_MOB,
			NAMECOLOR_NPC,
			NAMECOLOR_PC,
			NAMECOLOR_PC_END = NAMECOLOR_PC + EMPIRE_NUM,
			NAMECOLOR_NORMAL_MOB,
			NAMECOLOR_NORMAL_NPC,
			NAMECOLOR_NORMAL_PC,
			NAMECOLOR_NORMAL_PC_END = NAMECOLOR_NORMAL_PC + EMPIRE_NUM,
			NAMECOLOR_EMPIRE_MOB,
			NAMECOLOR_EMPIRE_NPC,
			NAMECOLOR_EMPIRE_PC,
			NAMECOLOR_EMPIRE_PC_END = NAMECOLOR_EMPIRE_PC + EMPIRE_NUM,
			NAMECOLOR_FUNC,
			NAMECOLOR_PK,
			NAMECOLOR_PVP,
			NAMECOLOR_PARTY,
			NAMECOLOR_WARP,
			NAMECOLOR_WAYPOINT,
#ifdef ENABLE_CONQUEROR_LEVEL
			NAMECOLOR_CONQUEROR,
#endif
#ifdef NAMECOLOR_BOSS_CLIENT
			NAMECOLOR_BOSS,
#endif
#ifdef ENABLE_METIN_STONES_MINIMAP
			NAMECOLOR_METIN,
#endif
#ifdef ENABLE_RENEWAL_OFFLINESHOP
			NAMECOLOR_SHOP,
#endif
			NAMECOLOR_EXTRA = NAMECOLOR_FUNC + 10,
			NAMECOLOR_NUM = NAMECOLOR_EXTRA + 10,
		};

		enum
		{
			ALIGNMENT_TYPE_WHITE,
			ALIGNMENT_TYPE_NORMAL,
			ALIGNMENT_TYPE_DARK,
		};

		enum
		{
			EMOTICON_EXCLAMATION = 1,
			EMOTICON_FISH = 11,
			EMOTICON_NUM = 128,

			TITLE_NUM = 9,
			TITLE_NONE = 4,
		};

		enum
		{
			EFFECT_REFINED_NONE,

/*---------------------------------------------------*/
			/*---------------SWORD---------------*/
			EFFECT_SWORD_REFINED7,
			EFFECT_SWORD_REFINED8,
			EFFECT_SWORD_REFINED9,
#ifdef ENABLE_LVL96_WEAPON_EFFECT
			EFFECT_SWORD_REFINED7TH,
#endif

			/*---------------BOW---------------*/
			EFFECT_BOW_REFINED7,
			EFFECT_BOW_REFINED8,
			EFFECT_BOW_REFINED9,
#ifdef ENABLE_LVL96_WEAPON_EFFECT
			EFFECT_BOW_REFINED7TH,
#endif

			/*---------------FAN---------------*/
			EFFECT_FANBELL_REFINED7,
			EFFECT_FANBELL_REFINED8,
			EFFECT_FANBELL_REFINED9,
#ifdef ENABLE_LVL96_WEAPON_EFFECT
			EFFECT_FANBELL_REFINED7TH,
#endif

			/*---------------DAGGER---------------*/
			//RIGHT
			EFFECT_SMALLSWORD_REFINED7,
			EFFECT_SMALLSWORD_REFINED8,
			EFFECT_SMALLSWORD_REFINED9,
#ifdef ENABLE_LVL96_WEAPON_EFFECT
			EFFECT_SMALLSWORD_REFINED7TH,
#endif
			//LEFT
			EFFECT_SMALLSWORD_REFINED7_LEFT,
			EFFECT_SMALLSWORD_REFINED8_LEFT,
			EFFECT_SMALLSWORD_REFINED9_LEFT,
#ifdef ENABLE_LVL96_WEAPON_EFFECT
			EFFECT_SMALLSWORD_REFINED7TH_LEFT,
#endif

			/*---------------CLAW---------------*/
#ifdef ENABLE_WOLFMAN_CHARACTER
			//RIGHT
			EFFECT_SWORD_REFINED7_W,
			EFFECT_SWORD_REFINED8_W,
			EFFECT_SWORD_REFINED9_W,
#ifdef ENABLE_LVL96_WEAPON_EFFECT
			EFFECT_SWORD_REFINED7TH_W,
#endif
			//LEFT
			EFFECT_SWORD_REFINED7_W_LEFT,
			EFFECT_SWORD_REFINED8_W_LEFT,
			EFFECT_SWORD_REFINED9_W_LEFT,
#ifdef ENABLE_LVL96_WEAPON_EFFECT
			EFFECT_SWORD_REFINED7TH_W_LEFT,
#endif
#endif

	/*---------------------------------------------------*/
			/*---------------ARMOR---------------*/
			EFFECT_BODYARMOR_REFINED7,
			EFFECT_BODYARMOR_REFINED8,
			EFFECT_BODYARMOR_REFINED9,
#ifdef ENABLE_LVL96_ARMOR_EFFECT
			EFFECT_BODYARMOR_SPECIAL_TH,
#endif
			EFFECT_BODYARMOR_SPECIAL,	// 4-2-1	Bubbles-Effekt
			EFFECT_BODYARMOR_SPECIAL2,	// 4-2-2	Blue-Shining-Effekt
			EFFECT_BODYARMOR_SPECIAL3,	// 5-1		Blitze-Effekt	ENABLE_LVL115_ARMOR_EFFECT

#ifdef USE_BODY_COSTUME_WITH_EFFECT
			EFFECT_COSTUMEBODY_YELLOW,
			EFFECT_COSTUMEBODY_ORANGE,
			EFFECT_COSTUMEBODY_BLUE,
			EFFECT_COSTUMEBODY_RED,
			EFFECT_COSTUMEBODY_GREEN,
			EFFECT_COSTUMEBODY_VIOLETT,
			EFFECT_COSTUMEBODY_WHITE,
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			EFFECT_ACCE,
#endif

#ifdef ENABLE_MDE_EFFECT
			EFFECT_SPECIAL_SWORD_30_1,
			EFFECT_SPECIAL_SWORD_90_1,
			EFFECT_SPECIAL_SWORD_75_1,
			EFFECT_SPECIAL_SWORD_75_2,
			EFFECT_SPECIAL_SWORD_90_2,
			EFFECT_SPECIAL_SWORD_90_3,
			EFFECT_SPECIAL_BLADE_30,
			EFFECT_SPECIAL_BLADE_75,
			EFFECT_SPECIAL_BOW_30,
			EFFECT_SPECIAL_BOW_75,
			EFFECT_SPECIAL_BOW_90,
			EFFECT_SPECIAL_BELL_30,
			EFFECT_SPECIAL_BELL_70,
			EFFECT_SPECIAL_BELL_90,
			EFFECT_SPECIAL_DAGGER_30,
			EFFECT_SPECIAL_DAGGER_30_LEFT,
			EFFECT_SPECIAL_DAGGER_75,
			EFFECT_SPECIAL_DAGGER_75_LEFT,
			EFFECT_SPECIAL_DAGGER_90,
			EFFECT_SPECIAL_DAGGER_90_LEFT,
			EFFECT_SPECIAL_FAN_30,
			EFFECT_SPECIAL_FAN_30_LEFT,
			EFFECT_SPECIAL_FAN_90,
			EFFECT_SPECIAL_FAN_90_LEFT,
			EFFECT_SPECIAL_BLADE_90,
#endif

			EFFECT_REFINED_NUM,
		};

		enum DamageFlag
		{
			DAMAGE_NORMAL	= (1 << 0),
			DAMAGE_POISON	= (1 << 1),
			DAMAGE_DODGE	= (1 << 2),
			DAMAGE_BLOCK	= (1 << 3),
			DAMAGE_PENETRATE= (1 << 4),
			DAMAGE_CRITICAL = (1 << 5),
		};

		enum
		{
			EFFECT_DUST,
			EFFECT_STUN,
			EFFECT_HIT,
			EFFECT_FLAME_ATTACK,
			EFFECT_FLAME_HIT,
			EFFECT_FLAME_ATTACH,
			EFFECT_ELECTRIC_ATTACK,
			EFFECT_ELECTRIC_HIT,
			EFFECT_ELECTRIC_ATTACH,
			EFFECT_SPAWN_APPEAR,
			EFFECT_SPAWN_DISAPPEAR,
			EFFECT_LEVELUP,
			EFFECT_SKILLUP,
			EFFECT_HPUP_RED,
			EFFECT_SPUP_BLUE,
			EFFECT_SPEEDUP_GREEN,
			EFFECT_DXUP_PURPLE,
			EFFECT_CRITICAL,
			EFFECT_PENETRATE,
			EFFECT_BLOCK,
			EFFECT_DODGE,
			EFFECT_FIRECRACKER,
			EFFECT_SPIN_TOP,
			EFFECT_WEAPON,
			EFFECT_WEAPON_END = EFFECT_WEAPON + WEAPON_NUM,
			EFFECT_AFFECT,
			EFFECT_AFFECT_GYEONGGONG = EFFECT_AFFECT + AFFECT_GYEONGGONG,
			EFFECT_AFFECT_KWAESOK = EFFECT_AFFECT + AFFECT_KWAESOK,
			EFFECT_AFFECT_END = EFFECT_AFFECT + AFFECT_NUM,
			EFFECT_EMOTICON,
			EFFECT_EMOTICON_END = EFFECT_EMOTICON + EMOTICON_NUM,
			EFFECT_SELECT,
			EFFECT_TARGET,
			EFFECT_EMPIRE,
			EFFECT_EMPIRE_END = EFFECT_EMPIRE + EMPIRE_NUM,
			EFFECT_HORSE_DUST,
			EFFECT_REFINED,
			EFFECT_REFINED_END = EFFECT_REFINED + EFFECT_REFINED_NUM,
			EFFECT_DAMAGE_TARGET,
			EFFECT_DAMAGE_NOT_TARGET,
			EFFECT_DAMAGE_SELFDAMAGE,
			EFFECT_DAMAGE_SELFDAMAGE2,
			EFFECT_DAMAGE_POISON,
			EFFECT_DAMAGE_MISS,
			EFFECT_DAMAGE_TARGETMISS,
			EFFECT_DAMAGE_CRITICAL,
			EFFECT_SUCCESS,
			EFFECT_FAIL,
			EFFECT_FR_SUCCESS,
			EFFECT_PERCENT_DAMAGE1,
			EFFECT_PERCENT_DAMAGE2,
			EFFECT_PERCENT_DAMAGE3,
			EFFECT_AUTO_HPUP,
			EFFECT_AUTO_SPUP,
			EFFECT_RAMADAN_RING_EQUIP,
			EFFECT_HALLOWEEN_CANDY_EQUIP,
			EFFECT_HAPPINESS_RING_EQUIP,
			EFFECT_LOVE_PENDANT_EQUIP,
			EFFECT_TEMP,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
			EFFECT_ACCE_SUCCEDED,
			EFFECT_ACCE_EQUIP,
#endif
#ifdef ENABLE_BOSS_EFFECT_OVER_HEAD
			EFFECT_BOSS,
#endif
#if defined(ENABLE_GROWTH_PET_SYSTEM) && defined(ENABLE_PVP_BALANCE)
			EFFECT_FEATHER_WALK,
			EFFECT_GYEONGGONG_BOOM,
#endif
#ifdef ENABLE_OCHAO_TEMPLE_SYSTEM
			EFFECT_HEALER,
#endif
#ifdef ENABLE_ZODIAC_MISSION
			EFFECT_SKILL_DAMAGE_ZONE,
			EFFECT_SKILL_DAMAGE_ZONE_BUYUK,
			EFFECT_SKILL_DAMAGE_ZONE_ORTA,
			EFFECT_SKILL_DAMAGE_ZONE_KUCUK,
			EFFECT_SKILL_SAFE_ZONE,
			EFFECT_SKILL_SAFE_ZONE_BUYUK,
			EFFECT_SKILL_SAFE_ZONE_ORTA,
			EFFECT_SKILL_SAFE_ZONE_KUCUK,
			EFFECT_METEOR,
			EFFECT_BEAD_RAIN,
			EFFECT_FALL_ROCK,
			EFFECT_ARROW_RAIN,
			EFFECT_HORSE_DROP,
			EFFECT_EGG_DROP,
			EFFECT_DEAPO_BOOM,
#endif
#ifdef ENABLE_QUEEN_NETHIS
			EFFECT_SNAKE_REGEN,
#endif
#ifdef ENABLE_PASSIVE_SYSTEM
			EFFECT_PASSIVE,
#endif

			EFFECT_NUM,
		};

		enum
		{
			DUEL_NONE,
			DUEL_CANNOTATTACK,
			DUEL_START,
		};

#ifdef ENABLE_SHOW_MOB_INFO
		enum EMobAIFlags
		{
			AIFLAG_AGGRESSIVE = (1 <<  0),
			AIFLAG_NOMOVE = (1 <<  1),
			AIFLAG_COWARD = (1 <<  2),
			AIFLAG_NOATTACKSHINSU = (1 <<  3),
			AIFLAG_NOATTACKJINNO = (1 <<  4),
			AIFLAG_NOATTACKCHUNJO = (1 <<  5),
			AIFLAG_ATTACKMOB = (1 <<  6),
			AIFLAG_BERSERK = (1 <<  7),
			AIFLAG_STONESKIN = (1 <<  8),
			AIFLAG_GODSPEED = (1 <<  9),
			AIFLAG_DEATHBLOW = (1 << 10),
			AIFLAG_REVIVE = (1 << 11),
		};
#endif

	public:
		static void DestroySystem();
		static void CreateSystem(UINT uCapacity);
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		static bool RegisterEffect(UINT eEftType, const char *c_szEftAttachBone, const char *c_szEftName, bool isCache, const char *name = nullptr);
#else
		static bool RegisterEffect(UINT eEftType, const char *c_szEftAttachBone, const char *c_szEftName, bool isCache);
#endif
		static void RegisterTitleName(int iIndex, const char *c_szTitleName);
		static void RegisterLeftSeatText(const char *c_szText);
		static const std::string& GetLeftSeatText();
		static bool RegisterNameColor(UINT uIndex, UINT r, UINT g, UINT b);
		static bool RegisterTitleColor(UINT uIndex, UINT r, UINT g, UINT b);

		static bool ChangeEffectTexture(UINT eEftType, const char *c_szSrcFileName, const char *c_szDstFileName);

		static void SetDustGap(float fDustGap);
		static void SetHorseDustGap(float fDustGap);

		static void SetEmpireNameMode(bool isEnable);
		static const D3DXCOLOR& GetIndexedNameColor(UINT eNameColor);

	public:
		void SetMainInstance();

		void OnSelected();
		void OnUnselected();
		void OnTargeted();
		void OnUntargeted();

	protected:
		bool __IsExistMainInstance();
		bool __IsMainInstance();
		bool __MainCanSeeHiddenThing();
		float __GetBowRange();

	protected:
		DWORD __AttachEffect(UINT eEftType
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			, float fScale = 1.0f
#endif
		);

		DWORD __AttachEffect(char filename[128]);
		void __DetachEffect(DWORD dwEID);

	public:
		void CreateSpecialEffect(DWORD iEffectIndex
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			, float fScale = 1.0f
#endif
		);

		void AttachSpecialEffect(DWORD effect
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			, float fScale = 1.0f
#endif
		);
#ifdef ENABLE_TITLE_SYSTEM
		void AttachSpecialEffectOverHead(DWORD effect);
		void ClearSpecialEffectOverHead();
#endif

	protected:
		static std::string ms_astAffectEffectAttachBone[EFFECT_NUM];
		static std::string ms_strLeftSeatText;
		static DWORD ms_adwCRCAffectEffect[EFFECT_NUM];
		static float ms_fDustGap;
		static float ms_fHorseDustGap;

	public:
		CInstanceBase();
		virtual ~CInstanceBase();

		bool LessRenderOrder(CInstanceBase* pkInst);

		void MountHorse(UINT eRace);
		void DismountHorse();

		void SCRIPT_SetAffect(UINT eAffect, bool isVisible);

		float CalculateDistanceSq3d(const TPixelPosition& c_rkPPosDst);

		// Instance Data
		bool IsFlyTargetObject();
		void ClearFlyTargetInstance();
		void SetFlyTargetInstance(CInstanceBase& rkInstDst);
		void AddFlyTargetInstance(CInstanceBase& rkInstDst);
		void AddFlyTargetPosition(const TPixelPosition& c_rkPPosDst);

		float GetFlyTargetDistance();

		void SetAlpha(float fAlpha);

		void DeleteBlendOut();

		void AttachTextTail();
		void DetachTextTail();
		void UpdateTextTailLevel(DWORD level);
#ifdef ENABLE_CONQUEROR_LEVEL
		void UpdateTextTailConquerorLevel(DWORD level);
#endif
		void RefreshTextTail();
		void RefreshTextTailTitle();

		bool Create(const SCreateData& c_rkCreateData);

		bool CreateDeviceObjects();
		void DestroyDeviceObjects();

		void Destroy();

		void Update();
		bool UpdateDeleting();

		void Transform();
		void Deform();
		void Render();
		void RenderTrace();
		void RenderToShadowMap();
		void RenderCollision();
		void RegisterBoundingSphere();

		// Temporary
		void GetBoundBox(D3DXVECTOR3 * vtMin, D3DXVECTOR3 * vtMax);

		void SetNameString(const char *c_szName, int len);
		bool SetRace(DWORD dwRaceIndex);
		void SetVirtualID(DWORD wVirtualNumber);
		void SetVirtualNumber(DWORD dwVirtualNumber);
		void SetInstanceType(int iInstanceType);
		void SetAlignment(short sAlignment);
#ifdef ENABLE_CONQUEROR_LEVEL
		void SetConquerorLevelText(int iLevel);
#endif
		void SetPKMode(BYTE byPKMode);
		void SetKiller(bool bFlag);
		void SetPartyMemberFlag(bool bFlag);
		void SetStateFlags(DWORD dwStateFlags);

#ifdef ENABLE_RENDER_TARGET
		DWORD GetArmor();
		DWORD GetHair();
		DWORD GetWeapon();
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		DWORD GetAcce();
#endif
#endif

		void SetArmor(DWORD dwArmor);
		void SetShape(DWORD eShape, float fSpecular = 0.0f);
		void SetHair(DWORD eHair);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		bool SetAcce(DWORD dwAcce);
		void ChangeAcce(DWORD dwAcce);
#endif
		bool SetWeapon(DWORD eWeapon);
		bool ChangeArmor(DWORD dwArmor);
		void ChangeWeapon(DWORD eWeapon);
		void ChangeHair(DWORD eHair);
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
		void ChangeGuild(DWORD dwGuildID, DWORD dwGuildLeader);
#else
		void ChangeGuild(DWORD dwGuildID);
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		void SetLanguage(BYTE bLang) { m_bLanguage = bLang; }
		BYTE GetLanguage() { return m_bLanguage; }

		void SetLanguage2(BYTE bLang) { m_bLanguage2 = bLang; }
		BYTE GetLanguage2() { return m_bLanguage2; }
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		void ChangeSkillColor(const DWORD * dwSkillColor);
#endif
		DWORD GetWeaponType();

		void SetComboType(UINT uComboType);
		void SetAttackSpeed(UINT uAtkSpd);
		void SetMoveSpeed(UINT uMovSpd);
		void SetRotationSpeed(float fRotSpd);

		const char *GetNameString();
		int GetInstanceType();
		DWORD GetPart(CRaceData::EParts part);
		DWORD GetShape();
		DWORD GetRace();
		DWORD GetLevel();
		DWORD GetVirtualID();
		DWORD GetVirtualNumber();
		DWORD GetEmpireID();
#ifdef ENABLE_SHOW_MOB_INFO
		DWORD GetAIFlag();
#endif
		DWORD GetGuildID();
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
		BYTE GetGuildLeader();
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		DWORD *GetSkillColor(const DWORD dwSkillIndex);
#endif
		int GetAlignment();
		UINT GetAlignmentGrade();
		int GetAlignmentType();
		BYTE GetPKMode();
		bool IsKiller();
		bool IsPartyMember();

		void ActDualEmotion(CInstanceBase & rkDstInst, WORD dwMotionNumber1, WORD dwMotionNumber2);
		void ActEmotion(DWORD dwMotionNumber);
		void LevelUp();
		void SkillUp();
		void UseSpinTop();
		void Revive();
		void Stun();
		void Die();
		void Hide();
		void Show();

#ifdef ENABLE_HIDE_COSTUME_SYSTEM
		bool CanInteract();
#endif
		bool CanAct();
		bool CanMove();
		bool CanAttack();
		bool CanUseSkill();
		bool CanFishing();
		bool IsConflictAlignmentInstance(CInstanceBase& rkInstVictim);
		bool IsAttackableInstance(CInstanceBase& rkInstVictim);
		bool IsTargetableInstance(CInstanceBase& rkInstVictim);
		bool IsPVPInstance(CInstanceBase& rkInstVictim);
		bool CanChangeTarget();
		bool CanPickInstance();
		bool CanViewTargetHP(CInstanceBase& rkInstVictim);
		bool __Background_IsGrassPixelPosition(const TPixelPosition& c_rkPPos);
		bool IsGrassPosition(float x, float y);
		void GetUnderwaterCameraPosition();

		// Movement
		BOOL IsGoing();
		bool NEW_Goto(const TPixelPosition& c_rkPPosDst, float fDstRot);
		void EndGoing();

		void SetRunMode();
		void SetWalkMode();

		bool IsAffect(UINT uAffect);
		BOOL IsInvisibility();
		BOOL IsParalysis();
		BOOL IsGameMaster();
		BOOL IsSameEmpire(CInstanceBase& rkInstDst);
		BOOL IsBowMode();
		BOOL IsHandMode();
		BOOL IsFishingMode();
		BOOL IsFishing();

		BOOL IsWearingDress();
		BOOL IsHoldingPickAxe();
#ifdef ENABLE_STANDING_MOUNT
		BOOL IsHoldingFishRod();
#endif
		BOOL IsMountingHorse();
#ifdef ENABLE_STANDING_MOUNT
		BOOL IsMountingHoverBoard();
#endif
#ifdef NAMECOLOR_BOSS_CLIENT
		BOOL IsBoss();
#endif
		BOOL IsNewMount();
		BOOL IsForceVisible();
		BOOL IsInSafe();

#ifdef ENABLE_EVENT_BANNER_FLAG
		BOOL IsBannerFlag();
#endif

		BOOL IsEnemy();
		BOOL IsStone();
		BOOL IsResource();
		BOOL IsNPC();
		BOOL IsPC();
        bool IsHiddenByOptions();
        void SetOptionQueueIndex(DWORD index) { m_optionQueueIndex = index; }
        DWORD m_optionQueueIndex = 0;
        bool IsInOptionViewFrustum() { return __IsInViewFrustum(); }
		BOOL IsPoly();
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		BOOL IsShop();
#endif
#ifdef ENABLE_DEFENSAWESHIP
		BOOL IsHydraNPC();
#endif
		BOOL IsWarp();
		BOOL IsGoto();
		BOOL IsObject();
		BOOL IsDoor();
		BOOL IsBuilding();
		BOOL IsWoodenDoor();
		BOOL IsStoneDoor();
		BOOL IsFlag();
		BOOL IsGuildWall();
		BOOL IsHorse();
		BOOL IsPet();
#ifdef ENABLE_GROWTH_PET_SYSTEM
		BOOL IsGrowthPet();
#endif
		BOOL IsMount();
		BOOL IsDead();
		BOOL IsStun();
		BOOL IsSleep();
		BOOL __IsSyncing();
		BOOL IsWaiting();
		BOOL IsWalking();
		BOOL IsPushing();
		BOOL IsAttacking();
		BOOL IsActingEmotion();
		BOOL IsAttacked();
		BOOL IsKnockDown();
		BOOL IsUsingSkill();
		BOOL IsUsingMovingSkill();
		BOOL CanCancelSkill();
		BOOL CanAttackHorseLevel();

		bool NEW_CanMoveToDestPixelPosition(const TPixelPosition& c_rkPPosDst);

		void NEW_SetAdvancingRotationFromPixelPosition(const TPixelPosition& c_rkPPosSrc, const TPixelPosition& c_rkPPosDst);
		void NEW_SetAdvancingRotationFromDirPixelPosition(const TPixelPosition& c_rkPPosDir);
		bool NEW_SetAdvancingRotationFromDestPixelPosition(const TPixelPosition& c_rkPPosDst);
		void SetAdvancingRotation(float fRotation);

		void EndWalking(float fBlendingTime = 0.15f);
		void EndWalkingWithoutBlending();

		// Battle
		void SetEventHandler(CActorInstance::IEventHandler* pkEventHandler);

		void PushUDPState(DWORD dwCmdTime, const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg);
		void PushTCPState(DWORD dwCmdTime, const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg);
		void PushTCPStateExpanded(DWORD dwCmdTime, const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg, UINT uTargetVID);

		void NEW_Stop();

		bool NEW_UseSkill(UINT uSkill, UINT uMot, UINT uMotLoopCount, bool isMovingSkill);
		void NEW_Attack();
		void NEW_Attack(float fDirRot);
		void NEW_AttackToDestPixelPositionDirection(const TPixelPosition& c_rkPPosDst);
		bool NEW_AttackToDestInstanceDirection(CInstanceBase& rkInstDst, IFlyEventHandler* pkFlyHandler);
		bool NEW_AttackToDestInstanceDirection(CInstanceBase& rkInstDst);

		bool NEW_MoveToDestPixelPositionDirection(const TPixelPosition& c_rkPPosDst);
		void NEW_MoveToDestInstanceDirection(CInstanceBase& rkInstDst);
		void NEW_MoveToDirection(float fDirRot);

		float NEW_GetDistanceFromDirPixelPosition(const TPixelPosition& c_rkPPosDir);
		float NEW_GetDistanceFromDestPixelPosition(const TPixelPosition& c_rkPPosDst);
		float NEW_GetDistanceFromDestInstance(CInstanceBase& rkInstDst);

		float NEW_GetRotation();
		float NEW_GetRotationFromDestPixelPosition(const TPixelPosition& c_rkPPosDst);
		float NEW_GetRotationFromDirPixelPosition(const TPixelPosition& c_rkPPosDir);
		float NEW_GetRotationFromDestInstance(CInstanceBase& rkInstDst);

		float NEW_GetAdvancingRotationFromDirPixelPosition(const TPixelPosition& c_rkPPosDir);
		float NEW_GetAdvancingRotationFromDestPixelPosition(const TPixelPosition& c_rkPPosDst);
		float NEW_GetAdvancingRotationFromPixelPosition(const TPixelPosition& c_rkPPosSrc, const TPixelPosition& c_rkPPosDst);

		BOOL NEW_IsClickableDistanceDestPixelPosition(const TPixelPosition& c_rkPPosDst);
		BOOL NEW_IsClickableDistanceDestInstance(CInstanceBase& rkInstDst);

		bool NEW_GetFrontInstance(CInstanceBase ** ppoutTargetInstance, float fDistance);
		void NEW_GetRandomPositionInFanRange(CInstanceBase& rkInstTarget, TPixelPosition* pkPPosDst);
		bool NEW_GetInstanceVectorInFanRange(float fSkillDistance, CInstanceBase& rkInstTarget, std::vector<CInstanceBase*>* pkVct_pkInst);
		bool NEW_GetInstanceVectorInCircleRange(float fSkillDistance, std::vector<CInstanceBase*>* pkVct_pkInst);

		void NEW_SetOwner(DWORD dwOwnerVID);
		void NEW_SyncPixelPosition(long & nPPosX, long & nPPosY);
		void NEW_SyncCurrentPixelPosition();

		void NEW_SetPixelPosition(const TPixelPosition& c_rkPPosDst);

		bool NEW_IsLastPixelPosition();
		const TPixelPosition& NEW_GetLastPixelPositionRef();

		// Battle
		BOOL isNormalAttacking();
		BOOL isComboAttacking();
		MOTION_KEY GetNormalAttackIndex();
		DWORD GetComboIndex();
		float GetAttackingElapsedTime();
		void InputNormalAttack(float fAtkDirRot);
		void InputComboAttack(float fAtkDirRot);

		void RunNormalAttack(float fAtkDirRot);
		void RunComboAttack(float fAtkDirRot, DWORD wMotionIndex);

		CInstanceBase* FindNearestVictim();
		BOOL CheckAdvancing();

		bool AvoidObject(const CGraphicObjectInstance& c_rkBGObj);
		bool IsBlockObject(const CGraphicObjectInstance& c_rkBGObj);
		void BlockMovement();

	public:
		BOOL CheckAttacking(CInstanceBase& rkInstVictim);
		void ProcessHitting(DWORD dwMotionKey, CInstanceBase * pVictimInstance);
		void ProcessHitting(DWORD dwMotionKey, BYTE byEventIndex, CInstanceBase * pVictimInstance);
		void GetBlendingPosition(TPixelPosition * pPixelPosition);
		void SetBlendingPosition(const TPixelPosition & c_rPixelPosition);

		// Fishing
		void StartFishing(float frot);
		void StopFishing();
		void ReactFishing();
		void CatchSuccess();
		void CatchFail();
		BOOL GetFishingRot(int * pirot);

		// Render Mode
		void RestoreRenderMode();
		void SetAddRenderMode();
		void SetModulateRenderMode();
		void SetRenderMode(int iRenderMode);
		void SetAddColor(const D3DXCOLOR & c_rColor);

		// Position
		void SCRIPT_SetPixelPosition(float fx, float fy);
		void NEW_GetPixelPosition(TPixelPosition * pPixelPosition);

		// Rotation
		void NEW_LookAtFlyTarget();
		void NEW_LookAtDestInstance(CInstanceBase& rkInstDst);
		void NEW_LookAtDestPixelPosition(const TPixelPosition& c_rkPPosDst);

		float GetRotation();
		float GetAdvancingRotation();
		void SetRotation(float fRotation);
		void BlendRotation(float fRotation, float fBlendTime = 0.1f);

		void SetDirection(int dir);
		void BlendDirection(int dir, float blendTime);
		float GetDegreeFromDirection(int dir);

		// Motion
		//	Motion Deque
		BOOL isLock();

		void SetMotionMode(int iMotionMode);
		int GetMotionMode(DWORD dwMotionIndex);

		// Motion
		//	Pushing Motion
		void ResetLocalTime();
		void SetLoopMotion(WORD wMotion, float fBlendTime=0.1f, float fSpeedRatio=1.0f);
		void PushOnceMotion(WORD wMotion, float fBlendTime=0.1f, float fSpeedRatio=1.0f);
		void PushLoopMotion(WORD wMotion, float fBlendTime=0.1f, float fSpeedRatio=1.0f);
		void SetEndStopMotion();

		// Intersect
		bool IntersectDefendingSphere();
		bool IntersectBoundingBox();

		// Part
		void Refresh(DWORD dwMotIndex, bool isLoop);

		float GetDistance(CInstanceBase * pkTargetInst);
		float GetDistance(const TPixelPosition & c_rPixelPosition);

		float GetBaseHeight();

		// ETC
		CActorInstance& GetGraphicThingInstanceRef();
		CActorInstance* GetGraphicThingInstancePtr();

		bool __Background_IsWaterPixelPosition(const TPixelPosition& c_rkPPos);
		bool __Background_GetWaterHeight(const TPixelPosition& c_rkPPos, float *pfHeight);

		int HasAffect(DWORD dwIndex);

		void __ClearAffectFlagContainer();
		void __ClearAffects();

		void __SetAffect(UINT eAffect, bool isVisible);
		
		void SetAffectFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer);

		void __SetNormalAffectFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer);
		void __SetStoneSmokeFlagContainer(const CAffectFlagContainer& c_rkAffectFlagContainer);

		void SetEmoticon(UINT eEmoticon);
		void SetFishEmoticon();
		bool IsPossibleEmoticon();

	protected:
		UINT __LessRenderOrder_GetLODLevel();
		void __Initialize();
		void __InitializeRotationSpeed();

		void __Create_SetName(const SCreateData& c_rkCreateData);
		void __Create_SetWarpName(const SCreateData& c_rkCreateData);

		CInstanceBase* __GetMainInstancePtr();
		CInstanceBase* __FindInstancePtr(DWORD dwVID);

		bool __FindRaceType(DWORD dwRace, BYTE* pbType);
		DWORD __GetRaceType();

		bool __IsShapeAnimalWear();
		BOOL __IsChangableWeapon(int iWeaponID);

		void __EnableSkipCollision();
		void __DisableSkipCollision();

		void __ClearMainInstance();

		void __Shaman_SetParalysis(bool isParalysis);
		void __Warrior_SetGeomgyeongAffect(bool isVisible);
		void __Assassin_SetEunhyeongAffect(bool isVisible);
#ifdef ENABLE_PVP_BALANCE
		void __Assassin_SetGyeongGongAffect(bool isVisible);
#endif
		void __SetReviveInvisibilityAffect(bool isVisible);

		BOOL __CanProcessNetworkStatePacket();

		bool __IsInDustRange();

		// Emotion
		void __ProcessFunctionEmotion(DWORD dwMotionNumber, DWORD dwTargetVID, const TPixelPosition & c_rkPosDst);
		void __EnableChangingTCPState();
		void __DisableChangingTCPState();
		BOOL __IsEnableTCPProcess(UINT eCurFunc);

		bool __CanRender();
		bool __IsInViewFrustum();

		// HORSE
		void __AttachHorseSaddle();
		void __DetachHorseSaddle();

		struct SHORSE
		{
			bool m_isMounting;
			CActorInstance* m_pkActor;

			SHORSE();
			~SHORSE();

			void Destroy();
			void Create(const TPixelPosition& c_rkPPos, UINT eRace, UINT eHitEffect);

			void SetAttackSpeed(UINT uAtkSpd);
			void SetMoveSpeed(UINT uMovSpd);
			void Deform();
			void Render();
			CActorInstance& GetActorRef();
			CActorInstance* GetActorPtr();

			bool IsMounting();
			bool CanAttack();
			bool CanUseSkill();

			UINT GetLevel();
			bool IsNewMount();
#ifdef ENABLE_STANDING_MOUNT
			bool IsHoverBoard();
#endif

			void __Initialize();
		} m_kHorse;


	protected:
		// Blend Mode
		void __SetBlendRenderingMode();
		void __SetAlphaValue(float fAlpha);
		float __GetAlphaValue();

		void __ComboProcess();
		void MovementProcess();
		void TodoProcess();
		void StateProcess();
		void AttackProcess();

		void StartWalking();
		float GetLocalTime();

		void RefreshState(DWORD dwMotIndex, bool isLoop);
		void RefreshActorInstance();

	protected:
		void OnSyncing();
		void OnWaiting();
		void OnMoving();

		void NEW_SetCurPixelPosition(const TPixelPosition& c_rkPPosDst);
		void NEW_SetSrcPixelPosition(const TPixelPosition& c_rkPPosDst);
		void NEW_SetDstPixelPosition(const TPixelPosition& c_rkPPosDst);
		void NEW_SetDstPixelPositionZ(FLOAT z);

		const TPixelPosition& NEW_GetCurPixelPositionRef();
		const TPixelPosition& NEW_GetSrcPixelPositionRef();

	public:
		const TPixelPosition& NEW_GetDstPixelPositionRef();

	protected:
		BOOL m_isTextTail;

		// Instance Data
		std::string m_stName;

		DWORD m_awPart[CRaceData::PART_MAX_NUM];
		DWORD m_dwLevel;
#ifdef ENABLE_CONQUEROR_LEVEL
		DWORD m_dwConquerorLevel;
#endif
#ifdef ENABLE_SHOW_MOB_INFO
		DWORD m_dwAIFlag;
#endif
		DWORD m_dwEmpireID;
		DWORD m_dwGuildID;
#ifdef ENABLE_GUILD_LEADER_TEXTAIL
		BYTE m_dwGuildLeader;
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
		BYTE m_bLanguage;
		BYTE m_bLanguage2;
#endif
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		DWORD *m_dwSkillColor[ESkillColorLength::MAX_SKILL_COUNT + MAX_BUFF_COUNT][ESkillColorLength::MAX_EFFECT_COUNT];
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
		BYTE m_bCharacterSize;
#endif

	protected:
		CAffectFlagContainer m_kAffectFlagContainer;
		DWORD m_adwCRCAffectEffect[AFFECT_NUM];

		UINT __GetRefinedEffect(CItemData* pItem);
		void __ClearWeaponRefineEffect();
		void __ClearArmorRefineEffect();
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		void	ClearAcceEffect();
#endif

	protected:
		void __AttachSelectEffect();
		void __DetachSelectEffect();

		void __AttachTargetEffect();
		void __DetachTargetEffect();

		void __AttachEmpireEffect(DWORD eEmpire);

	protected:
		struct SEffectContainer
		{
			typedef std::map<DWORD, DWORD> Dict;
			Dict m_kDct_dwEftID;
		} m_kEffectContainer;

		void __EffectContainer_Initialize();
		void __EffectContainer_Destroy();

#ifdef ENABLE_BOSS_EFFECT_OVER_HEAD
		void __AttachBossEffect();
#endif

		DWORD __EffectContainer_AttachEffect(DWORD eEffect);
		void __EffectContainer_DetachEffect(DWORD eEffect);

		SEffectContainer::Dict& __EffectContainer_GetDict();

	protected:
		struct SStoneSmoke 
		{
			DWORD m_dwEftID;
		} m_kStoneSmoke;

		void __StoneSmoke_Inialize();
		void __StoneSmoke_Destroy();
		void __StoneSmoke_Create(DWORD eSmoke);


	protected:
		BYTE m_eType;
		BYTE m_eRaceType;
		DWORD m_eShape;
		DWORD m_dwRace;
		DWORD m_dwVirtualNumber;
		short m_sAlignment;

		BYTE m_byPKMode;
		bool m_isKiller;
		bool m_isPartyMember;

		int m_iRotatingDirection;

		DWORD m_dwAdvActorVID;
		DWORD m_dwLastDmgActorVID;

		LONG m_nAverageNetworkGap;
		DWORD m_dwNextUpdateHeightTime;

		bool m_isGoing;

		TPixelPosition m_kPPosDust;

		DWORD m_dwLastComboIndex;

		DWORD m_swordRefineEffectRight;
		DWORD m_swordRefineEffectLeft;
		DWORD m_armorRefineEffect[2];
#ifdef ENABLE_TITLE_SYSTEM
		DWORD m_dwOverHeadSpecialEffect;
#endif

#ifdef ENABLE_MDE_EFFECT
		DWORD m_dwSpecialEffect;
		DWORD m_dwSpecialEffectLeft;
#endif

#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		DWORD m_dwAcceEffect;
#endif

		struct SMoveAfterFunc
		{
			UINT eFunc;
			UINT uArg;

			UINT uArgExpanded;
			TPixelPosition kPosDst;
		};

		SMoveAfterFunc m_kMovAfterFunc;

		float m_fDstRot;
		float m_fAtkPosTime;
		float m_fRotSpd;
		float m_fMaxRotSpd;

		BOOL m_bEnableTCPState;

		CActorInstance m_GraphicThingInstance;


	protected:
		struct SCommand
		{
			DWORD m_dwChkTime;
			DWORD m_dwCmdTime;
			float m_fDstRot;
			UINT m_eFunc;
			UINT m_uArg;
			UINT m_uTargetVID;
			TPixelPosition m_kPPosDst;
		};

		typedef std::list<SCommand> CommandQueue;

		DWORD m_dwBaseChkTime;
		DWORD m_dwBaseCmdTime;

		DWORD m_dwSkipTime;

		CommandQueue m_kQue_kCmdNew;

		BOOL m_bDamageEffectType;

		struct SEffectDamage
		{
			DWORD damage;
			BYTE flag;
			BOOL bSelf;
			BOOL bTarget;
			DWORD dwVictimVID;
			DWORD dwAttackerVID;
		};

#ifdef ENABLE_DAMAGE_EFFECT_ACCUMULATION_FIX
		typedef std::queue<SEffectDamage> CommandDamageQueue;
#else
		typedef std::list<SEffectDamage> CommandDamageQueue;
#endif

		CommandDamageQueue m_DamageQueue;

		void ProcessDamage();
		void ProcessRemoveOldDamage();

	public:
		void AddDamageEffect(DWORD damage, BYTE flag, BOOL bSelf, BOOL bTarget, DWORD dwVictimVID, DWORD dwAttackerVID);

	protected:
		struct SWarrior
		{
			DWORD m_dwGeomgyeongEffect;
		};

		SWarrior m_kWarrior;

		void __Warrior_Initialize();

#ifdef ENABLE_PVP_BALANCE
	protected:
		struct SAssassin
		{
			DWORD m_dwGyeongGongEffect;
		};

		SAssassin m_kAssassin;

		void __Assassin_Initialize();
#endif

	public:
		static void ClearPVPKeySystem();

		static void InsertPVPKey(DWORD dwSrcVID, DWORD dwDstVID);
		static void InsertPVPReadyKey(DWORD dwSrcVID, DWORD dwDstVID);
		static void RemovePVPKey(DWORD dwSrcVID, DWORD dwDstVID);

		static void InsertGVGKey(DWORD dwSrcGuildVID, DWORD dwDstGuildVID);
		static void RemoveGVGKey(DWORD dwSrcGuildVID, DWORD dwDstGuildVID);

		static void InsertDUELKey(DWORD dwSrcVID, DWORD dwDstVID);

		UINT GetNameColorIndex();

		const D3DXCOLOR& GetNameColor();
		const D3DXCOLOR& GetTitleColor();

	protected:
		static DWORD __GetPVPKey(DWORD dwSrcVID, DWORD dwDstVID);
		static bool __FindPVPKey(DWORD dwSrcVID, DWORD dwDstVID);
		static bool __FindPVPReadyKey(DWORD dwSrcVID, DWORD dwDstVID);
		static bool __FindGVGKey(DWORD dwSrcGuildID, DWORD dwDstGuildID);
		static bool __FindDUELKey(DWORD dwSrcGuildID, DWORD dwDstGuildID);

	protected:
		CActorInstance::IEventHandler* GetEventHandlerPtr();
		CActorInstance::IEventHandler& GetEventHandlerRef();

	protected:
		static float __GetBackgroundHeight(float x, float y);
		static DWORD __GetShadowMapColor(float x, float y);

	public:
		static void ResetPerformanceCounter();
		static void GetInfo(std::string* pstInfo);

	public:
		static CInstanceBase* New();
		static void Delete(CInstanceBase* pkInst);

		static CDynamicPool<CInstanceBase>	ms_kPool;

	protected:
		static DWORD ms_dwUpdateCounter;
		static DWORD ms_dwRenderCounter;
		static DWORD ms_dwDeformCounter;

	public:
		// Crowd budget: other players at or beyond this distance (the nearest N keep full
		// detail, see CPythonCharacterManager::Update) get reduced effects/LOD/skinning.
		static float ms_fCrowdDetailDistance;
		bool m_bBeyondCrowdBudget = false;
		DWORD GetDuelMode();
		void SetDuelMode(DWORD type);

		DWORD GetPartData(BYTE partIndex);

	protected:
		DWORD m_dwDuelMode;
		DWORD m_dwEmoticonTime;

#ifdef ENABLE_RENDER_TARGET
	protected:
		bool m_IsAlwaysRender;
		std::vector<DWORD> m_vecWikiEffects;

	public:
		bool IsAlwaysRender();
		void SetAlwaysRender(bool val);
		void AttachWikiAffect(DWORD effectIndex);
		void RemoveWikiAffect(DWORD effectIndex);
		void ClearWikiAffect();
#endif

	public:
		DWORD GetLevel() const { return m_dwLevel; }
		void SetLevel(DWORD dwLevel);

#ifdef ENABLE_GROWTH_PET_SYSTEM
		void SetPetLevel(DWORD dwLevel);
#endif

#ifdef ENABLE_STONE_SCALE_OPTION
		void RefreshStoneScale();
#endif

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	public:
		bool SetAura(DWORD eAura);
		void ChangeAura(DWORD eAura);

	protected:
		DWORD m_auraRefineEffect;
#endif
};

inline int RaceToJob(int race)
{
	const int JOB_NUM = 4;
	return race % JOB_NUM;
}

inline int RaceToSex(int race)
{
	switch (race)
	{
		case 0:
		case 2:
		case 5:
		case 7:
			return 1;
		case 1:
		case 3:
		case 4:
		case 6:
			return 0;
	}
	return 0;
}
