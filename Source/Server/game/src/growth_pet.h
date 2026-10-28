#pragma once
#include "../../common/length.h"
#include "../../common/tables.h"
#include "../../libpoly/Poly.h"
#include "packet.h"

#include <unordered_map>


#ifdef ENABLE_GROWTH_PET_SYSTEM
enum EPetButtonEvent
{
	FEED_LIFE_TIME_EVENT,
	FEED_EVOL_EVENT,
	FEED_EXP_EVENT,
	FEED_BUTTON_MAX,
};

enum EEvolution
{
	PET_EVOLUTION_1 = 1,
	PET_EVOLUTION_2 = 2,
	PET_EVOLUTION_3 = 3,
	PET_EVOLUTION_4 = 4,

	PET_EVOLUTION_MAX_NUM = 4
};

enum EPetBonusLevels
{
	EVEN_TYPE_HP_LEVEL = 5,
	EVEN_TYPE_SP_LEVEL = 4,
	EVEN_TYPE_DEF_GRADE_LEVEL = 7,

	ODD_TYPE_HP_LEVEL = 6,
	ODD_TYPE_SP_LEVEL = 6,
	ODD_TYPE_DEF_GRADE_LEVEL = 8,
};

enum EPetItem
{
	PET_ATTR_DETERMINE_ITEM = 55032,
	PET_ATTR_CHANGE_ITEM = 55033,
};

enum EPetRevive
{
	PET_REVIVE_TYPE_NORMAL,
	PET_REVIVE_TYPE_PREMIUM,
};

enum EPetFeed
{
	PET_FEED_TYPE_PCT,
	PET_FEED_TYPE_FULL,
};

enum EPetSkillType
{
	PET_SKILL_USE_TYPE_NONE,
	PET_SKILL_USE_TYPE_PASSIVE,
	PET_SKILL_USE_TYPE_AUTO,
	PET_SKILL_USE_TYPE_MAX,
};

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

enum EPetAutoSkillFlag
{
	PET_SKILL_FLAG_HEAL				= (1 << 0),
	PET_SKILL_FLAG_INVINCIBILITY	= (1 << 1),
	PET_SKILL_FLAG_REMOVAL			= (1 << 2),
	PET_SKILL_FLAG_FEATHER			= (1 << 3),
};

enum EPetWindowType
{
	PET_WINDOW_INFO,
	PET_WINDOW_ATTR_CHANGE,
	PET_WINDOW_PRIMIUM_FEEDSTUFF,
};

enum ELevelUpError
{
	LEVELUP_ERROR_NONE,
	LEVELUP_ERROR_MAX_LEVEL,
	LEVELUP_ERROR_OWNER_LEVEL,
	LEVELUP_ERROR_EVOLUTION
};

class CGrowthPet
{
	public:
		CGrowthPet(LPCHARACTER pOwner);
		~CGrowthPet();

		LPCHARACTER GetGrowthPet() const { return m_pGrowthPet; }
		LPCHARACTER GetOwner() const { return m_pOwner; }
		void SetOwner(LPCHARACTER owner) { m_pOwner = owner; }

		void SetState(BYTE bState) { m_bState = bState; };
		BYTE GetState() { return m_bState; }

		void SetSummonItem(LPITEM pItem) { m_pSummonItem = pItem; };
		LPITEM GetSummonItem() { return m_pSummonItem; }
		DWORD GetSummonItemVnum() { return m_dwVnum; }

		DWORD GetPetID() const { return m_dwID; }
		std::string GetPetName() { return m_strName; }
		BYTE GetPetType() { return m_bType; }
		TPetSkill* GetPetSkill() { return m_aSkill; }

		LPGROWTH_PET Summon(LPITEM pSummonItem, bool bIsResummon = false);
		void Unsummon();
		bool IsSummoned() const	{ return 0 != m_pGrowthPet; }

		bool Update();
		bool FollowCharacter(float fMinDistance = 50.f);
		bool UpdateFollowAI();

		void Save();
		void SetGrowthPetProto(TGrowthPet* pGrowthPetTable);
		void CreateGrowthPetProto(TGrowthPet* pGrowthPetTable);

		void SetPetPoint(BYTE bIdx, DWORD dwVal);
		void ChangePetPoint(BYTE bIdx, int iVal, bool bAmount = false, bool bPacket = true);
		DWORD GetPetPoint(BYTE bIdx) const;

		void PetSetExchangePacket(LPCHARACTER ch);
		void SendLevelUpEffect();
		void SendLevelStepEffect();
		void UpdateSkillPacket();

		void NameChange(const char* c_szName, LPITEM pChangeNameItem, LPITEM pUpBringingItem);

		void Feed(const TPacketCGPetFeed* feedPacket);

		bool CanReceiveEXP();
		BYTE CanLevelUp();
		static DWORD GetRequiredTotalExp(DWORD dwLevel);
		static DWORD GetRequiredMobExp(DWORD dwLevel);
		static DWORD GetRequiredItemExp(DWORD dwLevel);
		bool RewardEXP(BYTE bType, DWORD dwEXP);
		bool LevelUp();

		bool RewardFood(LPITEM pItem, BYTE bType);
		void PremiumFeed(const TPacketCGPetRevive* revivePacket, LPITEM pSummonItem);
		void Revive(LPITEM pSummonItem, BYTE bType);

		void SendEvolveMessage();
		bool Evolve(const TPacketCGPetFeed* feedPacket);
		void Evolve(BYTE dwEvolution);

		void IncreaseBattlePoints(DWORD dwLevel, bool bAttrChange = false);
		void RefreshAffect();
		void ClearAffect();

		void AttrDetermine(LPITEM pDetermineItem);
		void AttrChange(LPITEM pSummonItem, LPITEM pAttrChangeItem);

		void LearnSkill(BYTE bSlot, LPITEM pSkillBookItem);
		void SkillUpgrade(BYTE bSlot);
		void DeleteSkill(BYTE bSlot, LPITEM pSkillDeleteItem);
		void DeleteAllSkill(LPITEM pDeleteAllSkillItem);
		bool UseSkill(BYTE bSkillAffect);
		int GetSlotBySkill(BYTE bSkill);
		BYTE GetSkillLevelBySlot(BYTE bSlot);
		BYTE GetSkillBySlot(BYTE bSlot);
		DWORD GetSkillCooltimeBySlot(BYTE bSlot);
		void SetSkillCooltime(BYTE bSlot, DWORD dwCooltime);
		void RefreshAutoSkillFlag();
		BYTE GetAutoSkillFlag() { return m_bAutoSkillFlag; }

		void CancelFeatherWalk() { m_pFeatherSkillEvent = nullptr; }

	private:
		LPITEM m_pSummonItem;

		LPCHARACTER m_pGrowthPet;
		LPCHARACTER m_pOwner;

		DWORD m_dwID;
		DWORD m_dwVnum;
		BYTE m_bState;
		std::string m_strName;
		BYTE m_bSize;
		BYTE m_bType;
		BYTE m_bLevelStep;
		DWORD m_dwPoints[POINT_UPBRINGING_MAX_NUM];
		TPetSkill m_aSkill[PET_SKILL_COUNT_MAX];
		DWORD m_dwHPApply;
		DWORD m_dwSPApply;
		DWORD m_dwDefApply;
		DWORD m_dwAgeApply;
		BYTE m_bAutoSkillFlag;

		LPEVENT m_pUpdateEvent;
		LPEVENT m_pFeatherSkillEvent;

		bool m_bMobExpMsg;
		bool m_bItemExpMsg;
};

class CPetSkillProto
{
	public:
		DWORD dwPetVnum;
		DWORD dwSkillVnum;
		char szName[32 + 1];
		BYTE bType;
		DWORD dwCooldown;
		DWORD dwAffectFlag;

		DWORD dwPointOn;
		CPoly kPointPoly1;
		CPoly kPointPoly2;
		CPoly kPointPoly3;
		CPoly kPointPoly4;
		CPoly kPointPoly5;
		CPoly kPointPoly6;
		CPoly kPointPoly7;
		CPoly kPointPoly8;
		CPoly kActivatePctPoly;
		CPoly kDurationPoly;

		CPoly* GetPolyByType(BYTE bType);
		BYTE GetSkillType() { return bType; }
};

class CGrowthPetManager : public singleton<CGrowthPetManager>
{
	public:
		CGrowthPetManager();
		~CGrowthPetManager();

		LPGROWTH_PET CreateGrowthPet(LPCHARACTER ch, DWORD dwID);
		void DeleteGrowthPet(DWORD dwID, bool bDestroy = false);
		LPGROWTH_PET FindGrowthPet(DWORD dwID);
		void GenerateGrowthPetProto(LPCHARACTER ch, TGrowthPet* pGrowthPetTable, LPITEM pEgg, const char* c_szName);
		void EggHatch(LPCHARACTER pOwner, const char* c_szName, TItemPos Cell);

		bool InitializeSkill(TGrowthPetSkillTable* pTab, int iSize);
		CPetSkillProto* GetGrowthPetSkill(DWORD dwPetVnum, DWORD dwSkillVnum);

		typedef	std::unordered_map<DWORD, LPGROWTH_PET> TGrowthPetMap;

	private:
		typedef	std::unordered_map<DWORD, CPetSkillProto*> TGrowthPetSkillInfoMap;
		typedef	std::unordered_map<DWORD, TGrowthPetSkillInfoMap> TGrowthPetSkillMap;

		TGrowthPetMap m_growthPetGlobalMap;
		TGrowthPetSkillMap m_growthPetSkillMap;
};

#endif // ENABLE_GROWTH_PET_SYSTEM
