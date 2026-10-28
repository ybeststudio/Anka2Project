#pragma once

#include "StdAfx.h"

#ifdef ENABLE_NPC_WEAR_ITEM
enum
{
	NPC_WEAR_RACE,
	NPC_WEAR_WEAPON,
	NPC_WEAR_ARMOR,
	NPC_WEAR_HAIR,
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
	NPC_WEAR_ACCE,
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
	NPC_WEAR_AURA,
#endif
	NPC_WEAR_MOUNT,
	NPC_WEAR_MAX,
};
#endif

class CPythonNonPlayer : public CSingleton<CPythonNonPlayer>
{
	public:
		enum  EClickEvent
		{
			ON_CLICK_EVENT_NONE		= 0,
			ON_CLICK_EVENT_BATTLE	= 1,
			ON_CLICK_EVENT_SHOP		= 2,
			ON_CLICK_EVENT_TALK		= 3,
			ON_CLICK_EVENT_VEHICLE	= 4,

			ON_CLICK_EVENT_MAX_NUM,
		};

		enum EMobEnchants
		{
			MOB_ENCHANT_CURSE,
			MOB_ENCHANT_SLOW,
			MOB_ENCHANT_POISON,
			MOB_ENCHANT_STUN,
			MOB_ENCHANT_CRITICAL,
			MOB_ENCHANT_PENETRATE,
			MOB_ENCHANTS_MAX_NUM
		};

		enum EMobResists
		{
			MOB_RESIST_SWORD,
			MOB_RESIST_TWOHAND,
			MOB_RESIST_DAGGER,
			MOB_RESIST_BELL,
			MOB_RESIST_FAN,
			MOB_RESIST_BOW,
			MOB_RESIST_FIRE,
			MOB_RESIST_ELECT,
			MOB_RESIST_MAGIC,
			MOB_RESIST_WIND,
			MOB_RESIST_POISON,
			MOB_RESISTS_MAX_NUM
		};

		enum EMobRank
		{
			MOB_RANK_PAWN,
			MOB_RANK_S_PAWN,
			MOB_RANK_KNIGHT,
			MOB_RANK_S_KNIGHT,
			MOB_RANK_BOSS,
			MOB_RANK_KING,
		};

		#define MOB_ATTRIBUTE_MAX_NUM 12
		#define MOB_SKILL_MAX_NUM 5

#pragma pack(push)
#pragma pack(1)
		typedef struct SMobSkillLevel
		{
			DWORD       dwVnum;
			BYTE        bLevel;
		} TMobSkillLevel;

		typedef struct SMobTable
		{
			DWORD dwVnum;
			char szName[CHARACTER_NAME_MAX_LEN + 1]; 
			char szLocaleName[CHARACTER_NAME_MAX_LEN + 1];

			BYTE bType;
			BYTE bRank;
			BYTE bBattleType;
			BYTE bLevel;
			BYTE bSize;

			DWORD dwGoldMin;
			DWORD dwGoldMax;
			DWORD dwExp;
			DWORD dwMaxHP;
			BYTE bRegenCycle;
			BYTE bRegenPercent;
			WORD wDef;

			DWORD dwAIFlag;
			DWORD dwRaceFlag;
			DWORD dwImmuneFlag;

			BYTE bStr, bDex, bCon, bInt;
			DWORD dwDamageRange[2];

			short sAttackSpeed;
			short sMovingSpeed;
			BYTE bAggresiveHPPct;
			WORD wAggressiveSight;
			WORD wAttackRange;

			char cEnchants[MOB_ENCHANTS_MAX_NUM];
			char cResists[MOB_RESISTS_MAX_NUM];

			DWORD dwResurrectionVnum;
			DWORD dwDropItemVnum;

			BYTE bMountCapacity;
			BYTE bOnClickType;

			BYTE bEmpire;
			char szFolder[64 + 1];
			float fDamMultiply;
			DWORD dwSummonVnum;
			DWORD dwDrainSP;
			DWORD dwMonsterColor;
			DWORD dwPolymorphItemVnum;

			TMobSkillLevel Skills[MOB_SKILL_MAX_NUM];

			BYTE bBerserkPoint;
			BYTE bStoneSkinPoint;
			BYTE bGodSpeedPoint;
			BYTE bDeathBlowPoint;
			BYTE bRevivePoint;

			float fHitRange;
		} TMobTable;
#pragma pack(pop)

#ifdef ENABLE_MOB_SCALE
		typedef struct SMobScaleTable
		{
			DWORD dwMobNum;
			float fx;
			float fy;
			float fz;
			float ra;
			float rb;
		} TMobScaleTable;

		enum EMobScale
		{
			MOBSCALETABLE_MOB_NUM,
			MOBSCALETABLE_X,
			MOBSCALETABLE_Y,
			MOBSCALETABLE_Z,
			MOBSCALETABLE_RANDOM_A,
			MOBSCALETABLE_RANDOM_B,
			MOBSCALETABLE_MAX_NUM,
		};

		typedef std::map<DWORD, TMobScaleTable*> TNonMobScaleDataMap;
#endif

		typedef std::list<TMobTable *> TMobTableList;
		typedef std::map<DWORD, TMobTable*> TNonPlayerDataMap;

	public:
		CPythonNonPlayer(void);
		virtual ~CPythonNonPlayer(void);

		void Clear();
		void Destroy();

		bool LoadNonPlayerData(const char *c_szFileName);

#ifdef ENABLE_MOB_SCALE
		bool LoadMobScale(const char *c_szFileName);
		const TMobScaleTable* GetScaleTable(DWORD dwVnum);
		bool GetScale(DWORD dwVnum, float& fx, float& fy, float& fz);
#endif

		const TMobTable * GetTable(DWORD dwVnum);
		bool GetName(DWORD dwVnum, const char ** c_pszName);
		bool GetInstanceType(DWORD dwVnum, BYTE* pbType);
		BYTE GetEventType(DWORD dwVnum);
		BYTE GetEventTypeByVID(DWORD dwVID);
		DWORD GetMonsterColor(DWORD dwVnum);
		const char * GetMonsterName(DWORD dwVnum);

		float GetMonsterHitRange(DWORD dwVnum);

		// event_stone_index: swaps currently visible metin stone actors between two of
		// Anka's own existing metin models (mode 1/2), or restores their original model (mode 0).
		void SetEventStoneAppearance(int iMode);
		void ClearEventStoneAppearance();

#ifdef ENABLE_MONSTER_TARGET_ELEMENT
		bool MonsterHasRaceFlag(DWORD dwVnum, const char *szSearchString);
#endif

#ifdef ENABLE_BOSS_EFFECT_OVER_HEAD
		DWORD GetMonsterType(DWORD dwVnum);
		DWORD GetMonsterRank(DWORD dwVnum);
#endif

#ifdef ENABLE_WIKI_SYSTEM
		DWORD GetMonsterPrice1(DWORD dwVnum);
		DWORD GetMonsterPrice2(DWORD dwVnum);
		TNonPlayerDataMap GetMonsterData() {return m_NonPlayerDataMap;}
		char GetMonsterResist(DWORD dwVnum, BYTE byResist);
		DWORD GetMonsterMaxHP(DWORD dwVnum);
		DWORD GetMonsterRaceFlag(DWORD dwVnum);
		DWORD GetMonsterLevel(DWORD dwVnum);
		DWORD GetMonsterDamage1(DWORD dwVnum);
		DWORD GetMonsterDamage2(DWORD dwVnum);
		DWORD GetMonsterExp(DWORD dwVnum);
		float GetMonsterDamageMultiply(DWORD dwVnum);
		DWORD GetMonsterST(DWORD dwVnum);
		DWORD GetMonsterDX(DWORD dwVnum);
		bool IsMonsterStone(DWORD dwVnum);
#endif

		void GetMatchableMobList(int iLevel, int iInterval, TMobTableList * pMobTableList);

	protected:
		TNonPlayerDataMap m_NonPlayerDataMap;
		std::map<DWORD, DWORD> m_EventStoneOriginalRaceMap;
#ifdef ENABLE_MOB_SCALE
		TNonMobScaleDataMap m_NonMobScaleDataMap;
#endif

#ifdef ENABLE_NPC_WEAR_ITEM
	public:
		bool LoadNPCWear(const char* szNPCWearFile);
		int GetNPCWear(const int raceIndex, const BYTE wearIndex);

	protected:
		std::map<std::string, std::map<DWORD, std::vector<int>>> m_mapnpcWear;
#endif
};