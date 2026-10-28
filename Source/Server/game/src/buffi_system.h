#ifndef __INC_BUFFI_SYSTEM_H__
#define __INC_BUFFI_SYSTEM_H__

#include "../../common/service.h"

#ifdef ENABLE_BUFFI_SYSTEM
#include <unordered_map>

// Buffi (support shaman) — 1:1 port of the Baerim system.
// The Buffi is an NPC (vnum BUFFI_NPC_FEMALE/MALE, drawn as a shaman through npc_wear.txt) that follows
// its owner and keeps the five shaman buffs on him using the regular skill formulas.
// State lives in player.player_buffi (one row per owner, see sql/player_buffi.sql); older characters are
// migrated once from the "buffi.*" quest flags. The casts are shown with HEADER_GC_BUFFI_USE_SKILL and the
// costume parts (armor / weapon / hair) are sent through the regular character parts.

enum EBuffiConfig
{
	BUFFI_SEAL_VNUM = 57100,
	BUFFI_ORE_VNUM = 57101,			// +3 INT, 24h
	BUFFI_DURATION_VNUM = 57102,		// +3 days, revives
	BUFFI_ORE_PERM_VNUM = 57103,		// +3 INT, permanent
	BUFFI_DURATION_PERM_VNUM = 57104,	// permanent duration
	BUFFI_CHANGE_SEX_VNUM = 57105,
	BUFFI_SKILL_BOOK_VNUM = 57106,		// reference 98506: one more step on a Master skill
	BUFFI_MASTER_BOOK_VNUM = 57107,		// reference 98508: Master -> G1 at once
	BUFFI_MASTER_STONE_VNUM = 57108,	// reference 98509: Grand Master -> P at once
	BUFFI_SOUL_STONE_VNUM = 50513,		// one Grand Master step

	BUFFI_NPC_FEMALE = 34900,
	BUFFI_NPC_MALE = 34901,

	BUFFI_SKILL_COUNT = 5,
	BUFFI_RUNE_COUNT = 3,
	BUFFI_MAX_LEVEL = 90,
	BUFFI_MAX_EVOLUTION = 3,
	BUFFI_ORE_INT = 3,
	BUFFI_START_DURATION = 3 * 24 * 60 * 60,
	BUFFI_ORE_DURATION = 24 * 60 * 60,
	BUFFI_MAX_DURATION = 30 * 24 * 60 * 60,

	// default look (same as npc_wear.txt) until a costume is put on
	BUFFI_DEFAULT_ARMOR = 11899,
	BUFFI_DEFAULT_WEAPON = 7179,
};

enum EBuffiCostume
{
	BUFFI_COSTUME_BODY,
	BUFFI_COSTUME_HAIR,
	BUFFI_COSTUME_WEAPON,
	BUFFI_COSTUME_SASH,
	BUFFI_COSTUME_MAX,
};

class CBuffiActor
{
	public:
		CBuffiActor(LPCHARACTER owner);
		~CBuffiActor();

		bool Summon();
		void Unsummon(bool bRemoveBuffs);
		bool IsSummoned() const { return m_pkChar != NULL; }
		bool CanSummon() const { return !IsExpired(); }
		LPCHARACTER GetCharacter() const { return m_pkChar; }

		void Update();
		void OnOwnerGainExp(int iExp);

		bool RaiseSkill(BYTE idx);
		bool UseOre(LPITEM item);
		bool RemoveOre(BYTE idx);
#ifdef ENABLE_BUFFI_EVOLUTION
		bool Evolve();
#endif
#ifdef ENABLE_BUFFI_CHANGE_SEX
		bool ChangeSex(LPITEM item);
#endif
#if defined(ENABLE_SKILL_UPGRADE) && defined(ENABLE_BUFFI_SKILL_BOOK)
		// Skill Upgrader (skill_upgrade.cpp)
		static DWORD GetSkillVnum(int idx);
		int GetSkillLevel(BYTE idx) const { return m_aiSkillLevel[idx]; }
		int GetBookProgress(BYTE idx) const { return m_aiBookProgress[idx]; }
		void SetBookProgress(BYTE idx, int iProgress);
		void OnBookLevelUp(BYTE idx);
		bool UseMasterBook(BYTE idx);
#endif
		void SetSkillColor(BYTE idx, const DWORD* adwColor);
		bool UseDuration(LPITEM item);
		int UseAllDuration();

		void SendAll();
		void SendData();
		void SendSkills();
		void SendCooldowns();
		void OnCharacterDestroyed();
		void RefreshLook();
		int GetSex() const { return m_iSex; }
		void SetKeepBuffsOnUnequip(bool b) { m_bKeepBuffs = b; }
		bool IsKeepBuffsOnUnequip() const { return m_bKeepBuffs; }

	private:
		void Load();
		bool LoadFromQuestFlags();
		void Save();
		void ApplyLook();
		void SendUseSkill(DWORD dwVnum, int iLevel);
		void CancelUpdateEvent();
		void RequestRecast();
#ifdef ENABLE_BUFFI_SKILL_BOOK
		bool ReadSkillBook(BYTE idx);
#endif
		void GiveOwnerBuffColor(int idx);
		void ApplySkillColor(int iCastIdx);
		void SendSkillColors();
		int GetIntelligence() const;
		int GetLevelCap() const;
		DWORD GetNeedExp() const;
		bool IsExpired() const;
		int GetRemainDuration() const;
		int GetRuneValue(BYTE idx) const;
		void ExpireRunes();
		void ApplyStats();
		void Follow();
		bool TryCastBuffs();
		int GetSkillBonus(BYTE idx) const;

	private:
		LPCHARACTER m_pkOwner;
		LPCHARACTER m_pkChar;
		LPEVENT m_pkUpdateEvent;

		std::string m_strName;
		DWORD m_adwCostume[BUFFI_COSTUME_MAX];
		DWORD m_adwSkillColor[BUFFI_SKILL_COUNT][5];	// skill colour system: 5 layers per Buffi skill

		int m_iLevel;
		DWORD m_dwExp;
		int m_iEvolution;
		int m_iSex;
		int m_iDurationEnd;		// unix time, 0 = dead
		bool m_bDurationPerm;
		int m_aiRune[BUFFI_RUNE_COUNT];	// 0 empty, -1 permanent, >0 expire time
		int m_aiSkillLevel[BUFFI_SKILL_COUNT];
		int m_aiBookProgress[BUFFI_SKILL_COUNT];	// books read on the current Master level
		int m_iSkillPoints;

		DWORD m_adwNextCast[BUFFI_SKILL_COUNT];
		bool m_abRecast[BUFFI_SKILL_COUNT];	// INT changed: cast again even if the buff is active
		bool m_bKeepBuffs;		// automatic (PvP option) removal keeps the buffs, a manual one clears them
		DWORD m_dwNextCastTime;
		DWORD m_dwLastSendTime;
		bool m_bDirty;
};

class CBuffiManager : public singleton<CBuffiManager>
{
	public:
		CBuffiManager();
		virtual ~CBuffiManager();

		CBuffiActor* Get(LPCHARACTER owner, bool bCreate = false);
		bool IsSummoned(LPCHARACTER owner);
		bool IsBuffiCharacter(LPCHARACTER ch) const;

		// The seal and the Buffi costumes are worn in WEAR_BUFFI..WEAR_BUFFI_ACCE (Companion panel).
		static bool IsBuffiWearCell(int cell) { return cell >= WEAR_BUFFI && cell <= WEAR_BUFFI_ACCE; }
		static bool IsSealEquipped(LPCHARACTER owner);
		static bool HasSeal(LPCHARACTER owner);
		bool EquipToBuffi(LPCHARACTER owner, LPITEM item, int cell);
		void OnWearChanged(LPCHARACTER owner, int cell, bool bEquipped);

		void OnLogin(LPCHARACTER owner);
		void OnDestroy(LPCHARACTER owner);
		void OnOwnerGainExp(LPCHARACTER owner, int iExp);
		bool OnUseItem(LPCHARACTER owner, LPITEM item);

		bool Summon(LPCHARACTER owner);
		void Unsummon(LPCHARACTER owner);
		void OnOwnerAttack(LPCHARACTER owner, LPCHARACTER victim);

	private:
		std::unordered_map<DWORD, CBuffiActor*> m_map;	// owner VID
};
#endif
#endif
