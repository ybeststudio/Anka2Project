#pragma once

#include "StdAfx.h"
#include "AbstractPlayer.h"
#include "Packet.h"
#include "PythonSkill.h"

#ifdef ENABLE_GROWTH_PET_SYSTEM
	#include <unordered_map>
#endif

namespace NPlayerData
{
	enum ERace
	{
		MAIN_RACE_WARRIOR_M,
		MAIN_RACE_ASSASSIN_W,
		MAIN_RACE_SURA_M,
		MAIN_RACE_SHAMAN_W,
		MAIN_RACE_WARRIOR_W,
		MAIN_RACE_ASSASSIN_M,
		MAIN_RACE_SURA_W,
		MAIN_RACE_SHAMAN_M,
		MAIN_RACE_MAX_NUM,
	};

	enum EEmotions
	{
		EMOTION_CLAP = 1,
		EMOTION_CONGRATULATION,
		EMOTION_FORGIVE,
		EMOTION_ANGRY,
		EMOTION_ATTRACTIVE,
		EMOTION_SAD,
		EMOTION_SHY,
		EMOTION_CHEERUP,
		EMOTION_BANTER,
		EMOTION_JOY,
		EMOTION_CHEERS_1,
		EMOTION_CHEERS_2,
		EMOTION_DANCE_1,
		EMOTION_DANCE_2,
		EMOTION_DANCE_3,
		EMOTION_DANCE_4,
		EMOTION_DANCE_5,
		EMOTION_DANCE_6,
		EMOTION_DANCE_7,
		EMOTION_SELFIE,
		EMOTION_DOZE,
		EMOTION_EXERCISE,
		EMOTION_PUSHUP,
		EMOTION_KISS = 51,
		EMOTION_FRENCH_KISS,
		EMOTION_SLAP,
		EMOTION_EMOTION_1 = 61,
		EMOTION_EMOTION_2,
		EMOTION_EMOTION_3,
		EMOTION_EMOTION_4,
		EMOTION_EMOTION_5,
		EMOTION_EMOTION_6,
		EMOTION_EMOTION_7,
		EMOTION_EMOTION_8,
		EMOTION_EMOTION_9,
		EMOTION_EMOTION_10,
		EMOTION_EMOTION_11,
		EMOTION_EMOTION_12,
		EMOTION_EMOTION_13,
		EMOTION_EMOTION_14,
		EMOTION_EMOTION_15,
		EMOTION_EMOTION_16,
		EMOTION_EMOTION_17,
		EMOTION_EMOTION_18,
		EMOTION_EMOTION_19,
		EMOTION_EMOTION_20,
		EMOTION_EMOTION_21,
	};
};

class CInstanceBase;

extern std::map<int, CGraphicImage*> m_kMap_iEmotionIndex_pkIconImage;

extern const DWORD c_iSkillIndex_Tongsol;
extern const DWORD c_iSkillIndex_Combo;
extern const DWORD c_iSkillIndex_Fishing;
extern const DWORD c_iSkillIndex_Mining;
extern const DWORD c_iSkillIndex_Making;
extern const DWORD c_iSkillIndex_Language1;
extern const DWORD c_iSkillIndex_Language2;
extern const DWORD c_iSkillIndex_Language3;
extern const DWORD c_iSkillIndex_Polymorph;
extern const DWORD c_iSkillIndex_Riding;
extern const DWORD c_iSkillIndex_Summon;

class CPythonPlayer : public CSingleton<CPythonPlayer>, public IAbstractPlayer
{
	public:
		enum
		{
			CATEGORY_NONE		= 0,
			CATEGORY_ACTIVE		= 1,
			CATEGORY_PASSIVE	= 2,
			CATEGORY_MAX_NUM	= 3,

			STATUS_INDEX_ST = 1,
			STATUS_INDEX_DX = 2,
			STATUS_INDEX_IQ = 3,
			STATUS_INDEX_HT = 4,
		};

		enum
		{
			MBT_LEFT,
			MBT_RIGHT,
			MBT_MIDDLE,
			MBT_NUM,
		};

		enum
		{
			MBF_SMART,
			MBF_MOVE,
			MBF_CAMERA,
			MBF_ATTACK,
			MBF_SKILL,
			MBF_AUTO,
		};

		enum
		{
			MBS_CLICK,
			MBS_PRESS,
		};

		enum EMode
		{
			MODE_NONE,
			MODE_CLICK_POSITION,
			MODE_CLICK_ITEM,
			MODE_CLICK_ACTOR,
			MODE_USE_SKILL,
		};

		enum EEffect
		{
			EFFECT_PICK,
			EFFECT_NUM,
		};

		enum EMetinSocketType
		{
			METIN_SOCKET_TYPE_NONE,
			METIN_SOCKET_TYPE_SILVER,
			METIN_SOCKET_TYPE_GOLD,
		};

		typedef struct SSkillInstance
		{
			DWORD dwIndex;
			int iType;
			int iGrade;
			int iLevel;
			float fcurEfficientPercentage;
			float fnextEfficientPercentage;
			BOOL isCoolTime;

			float fCoolTime;
			float fLastUsedTime;
			BOOL bActive;
		} TSkillInstance;

		enum EKeyBoard_UD
		{
			KEYBOARD_UD_NONE,
			KEYBOARD_UD_UP,
			KEYBOARD_UD_DOWN,
		};

		enum EKeyBoard_LR
		{
			KEYBOARD_LR_NONE,
			KEYBOARD_LR_LEFT,
			KEYBOARD_LR_RIGHT,
		};

		enum
		{
			DIR_UP,
			DIR_DOWN,
			DIR_LEFT,
			DIR_RIGHT,
		};

		typedef struct SPlayerStatus
		{
			TItemData			aItem[c_Inventory_Count];
			TItemData			aDSItem[c_DragonSoul_Inventory_Count];
#ifdef ENABLE_RENEWAL_SWITCHBOT
			TItemData			aSwitchbotItem[SWITCHBOT_SLOT_COUNT];
#endif
			TQuickSlot			aQuickSlot[QUICKSLOT_MAX_NUM];
			TSkillInstance		aSkill[SKILL_MAX_NUM];
#ifdef ENABLE_GOLD_LIMIT
			long long			m_alPoint[POINT_MAX_NUM];
#else
			long				m_alPoint[POINT_MAX_NUM];
#endif
			long				lQuickPageIndex;

#ifdef ENABLE_GOLD_LIMIT
			void SetPoint(UINT ePoint, long long lPoint);
			long long GetPoint(UINT ePoint);
#else
			void SetPoint(UINT ePoint, long lPoint);
			long GetPoint(UINT ePoint);
#endif
		} TPlayerStatus;

		typedef struct SPartyMemberInfo
		{
			SPartyMemberInfo(DWORD _dwPID, const char *c_szName) : dwPID(_dwPID), strName(c_szName), dwVID(0) {}

			DWORD dwVID;
			DWORD dwPID;
			std::string strName;
			BYTE byState;
			BYTE byHPPercentage;
			short sAffects[PARTY_AFFECT_SLOT_MAX_NUM];
		} TPartyMemberInfo;

		enum EPartyRole
		{
			PARTY_ROLE_NORMAL,
			PARTY_ROLE_LEADER,
			PARTY_ROLE_ATTACKER,
			PARTY_ROLE_TANKER,
			PARTY_ROLE_BUFFER,
			PARTY_ROLE_SKILL_MASTER,
			PARTY_ROLE_BERSERKER,
			PARTY_ROLE_DEFENDER,
			PARTY_ROLE_MAX_NUM,
		};

		enum
		{
			SKILL_NORMAL,
			SKILL_MASTER,
			SKILL_GRAND_MASTER,
			SKILL_PERFECT_MASTER,
		};

		struct SAutoPotionInfo
		{
			SAutoPotionInfo() : bActivated(false), totalAmount(0), currentAmount(0) {}

			bool bActivated;
			long currentAmount;
			long totalAmount;
			long inventorySlotIndex;
		};

		enum EAutoPotionType
		{
			AUTO_POTION_TYPE_HP = 0,
			AUTO_POTION_TYPE_SP = 1,
			AUTO_POTION_TYPE_NUM
		};

	public:
		CPythonPlayer(void);
		virtual ~CPythonPlayer(void);

		void	PickCloseMoney();
		void	PickCloseItem();
#ifdef ENABLE_INSTANT_PICKUP
		void	PickCloseItemVector();
#endif
#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
		void	PickCloseItemAutoVector();
#endif

		void	SetGameWindow(PyObject *ppyObject);

		void	SetObserverMode(bool isEnable);
		bool	IsObserverMode();

		void	SetQuickCameraMode(bool isEnable);

		void	SetAttackKeyState(bool isPress);

		void	NEW_GetMainActorPosition(TPixelPosition* pkPPosActor);

		bool	RegisterEffect(DWORD dwEID, const char *c_szEftFileName, bool isCache);

		bool	NEW_SetMouseState(int eMBType, int eMBState);
		bool	NEW_SetMouseFunc(int eMBType, int eMBFunc);
		int		NEW_GetMouseFunc(int eMBT);
		void	NEW_SetMouseMiddleButtonState(int eMBState);

		void	NEW_SetAutoCameraRotationSpeed(float fRotSpd);
		void	NEW_ResetCameraRotation();

		void	NEW_SetSingleDirKeyState(int eDirKey, bool isPress);
		void	NEW_SetSingleDIKKeyState(int eDIKKey, bool isPress);
		void	NEW_SetMultiDirKeyState(bool isLeft, bool isRight, bool isUp, bool isDown);

		void	NEW_Attack();
		void	NEW_Fishing();
		bool	NEW_CancelFishing();

		void	NEW_LookAtFocusActor();
		bool	NEW_IsAttackableDistanceFocusActor();


		bool	NEW_MoveToDestPixelPositionDirection(const TPixelPosition& c_rkPPosDst);
		bool	NEW_MoveToMousePickedDirection();
		bool	NEW_MoveToMouseScreenDirection();
		bool	NEW_MoveToDirection(float fDirRot);
		void	NEW_Stop();


		// Reserved
		bool	NEW_IsEmptyReservedDelayTime(float fElapsedtime);


		// Dungeon
		void	SetDungeonDestinationPosition(int ix, int iy);
		void	AlarmHaveToGo();


		CInstanceBase* NEW_FindActorPtr(DWORD dwVID);
		CInstanceBase* NEW_GetMainActorPtr();

		// flying target set
		void	Clear();
		void	ClearSkillDict();
		void	ClearSkillToggle(DWORD dwSlotIndex);
		void	NEW_ClearSkillData(bool bAll = false);

		void	Update();


		// Play Time
		DWORD	GetPlayTime();
		void	SetPlayTime(DWORD dwPlayTime);


		// System
		void	SetMainCharacterIndex(int iIndex);

		DWORD	GetMainCharacterIndex();
		bool	IsMainCharacterIndex(DWORD dwIndex);
		DWORD	GetGuildID();
		void	NotifyDeletingCharacterInstance(DWORD dwVID);
		void	NotifyCharacterDead(DWORD dwVID);
		void	NotifyCharacterUpdate(DWORD dwVID);
		void	NotifyDeadMainCharacter();
		void	NotifyChangePKMode();
		void	ResetSkillCoolTimes();

		// Player Status
		const char *	GetName();
		void	SetName(const char *name);

		void	SetRace(DWORD dwRace);
		DWORD	GetRace();

		void	SetWeaponPower(DWORD dwMinPower, DWORD dwMaxPower, DWORD dwMinMagicPower, DWORD dwMaxMagicPower, DWORD dwAddPower);

#ifdef ENABLE_GOLD_LIMIT
		void	SetStatus(DWORD dwType, long long lValue);
		long long	GetStatus(DWORD dwType);
#else
		void	SetStatus(DWORD dwType, long lValue);
		int		GetStatus(DWORD dwType);
#endif

		int		GetLeftSeatTimeIndex() const { return m_iLeftSeatTimeIndex; }
		void	SetLeftSeatTimeIndex(int iIndex) { m_iLeftSeatTimeIndex = iIndex; }
		int		GetLeftSeatLogoutTimeIndex() const { return m_iLeftSeatLogoutTimeIndex; }
		void	SetLeftSeatLogoutTimeIndex(int iIndex) { m_iLeftSeatLogoutTimeIndex = iIndex; }
		bool	CheckLeftSeatState(DWORD dwVID);
		bool	CanEscapeState();
#ifdef ENABLE_SUNGMA_SCORE
		int		GetSungmaScore();
		DWORD	GetItemSungmaScore(BYTE bWindowType, WORD wSlotIndex);
#endif
#ifdef ENABLE_YIN_AND_YANG_ATTR
		bool	GetItemYinAndYangAttrInfo(BYTE bWindowType, WORD wSlotIndex, DWORD* pdwAttrVal, BYTE* pbyAttrType);
#endif


		// Item
		void	MoveItemData(TItemPos SrcCell, TItemPos DstCell);
		void	SetItemData(TItemPos Cell, const TItemData & c_rkItemInst);
		const TItemData * GetItemData(TItemPos Cell) const;
#ifdef ENABLE_STACK_LIMIT
		void	SetItemCount(TItemPos Cell, WORD byCount);
#else
		void	SetItemCount(TItemPos Cell, BYTE byCount);
#endif

		void	SetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex, DWORD dwMetinNumber);
		void	SetItemAttribute(TItemPos Cell, DWORD dwAttrIndex, BYTE byType, short sValue);
		DWORD	GetItemIndex(TItemPos Cell);
		int		GetItemSlotIndex(DWORD dwVnum);
		DWORD	GetItemFlags(TItemPos Cell);
		DWORD	GetItemAntiFlags(TItemPos Cell);
		BYTE	GetItemTypeBySlot(TItemPos Cell);
		BYTE	GetItemSubTypeBySlot(TItemPos Cell);
		DWORD	GetItemCount(TItemPos Cell);
		DWORD	GetItemCountByVnum(DWORD dwVnum);
		DWORD	GetItemMetinSocket(TItemPos Cell, DWORD dwMetinSocketIndex);
		void	GetItemAttribute(TItemPos Cell, DWORD dwAttrSlotIndex, BYTE * pbyType, short * psValue);
		void	SendClickItemPacket(DWORD dwIID);

#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
	public:
		enum
		{
			COMB_WND_SLOT_MEDIUM = 0,
			COMB_WND_SLOT_BASE = 1,
			COMB_WND_SLOT_MATERIAL = 2,
			COMB_WND_SLOT_RESULT = 3,
			COMB_WND_SLOT_MAX = 4
		};

		void	SetItemCombinationWindowActivedItemSlot(BYTE bSlot, int iInvenSlot);
		int		GetInvenSlotAttachedToConbWindowSlot(BYTE bSlot);
		BYTE	GetConbWindowSlotByAttachedInvenSlot(int iInvenSlot);
		bool	CanAttachToCombMediumSlot(BYTE bSlot, TItemPos ItemPos);
		bool	CanAttachToCombItemSlot(BYTE bSlot, TItemPos ItemPos);
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
		void	SetHitCountInfo(DWORD dwVid, DWORD dwCount);
		DWORD 	GetHitCountInfo(DWORD dwVid);
		
		void 	SetRewardItem(BYTE bDay, DWORD dwVnum, DWORD dwCount);
		std::vector<TRewardItem>& GetRewardVec() { return m_rewardItems; }
#endif

#ifdef ENABLE_AUTOMATIC_PICK_UP_SYSTEM
		void	SendPickUPItemPacket(std::vector<DWORD> itemVidList);
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		void	ClearChangeLook(const bool bClearEffect = false);

		void	SetItemTransmutationVnum(TItemPos& Cell, const DWORD blVnum);
		DWORD	GetItemChangeLookVnum(TItemPos& Cell) const;
#ifdef ENABLE_SET_ITEM
		void	SetItemSetValue(const TItemPos& Cell, const DWORD dwSetValue);
		DWORD	GetItemSetValue(const TItemPos& Cell) const;
		bool	CheckItemSetValue(DWORD dwSetValue) const;
		DWORD	ChangeFromSetValueToAffect(DWORD dwSetValue) const;
		DWORD	ChangeFromAffectToSetValue(DWORD dwAffect) const;
		bool	IsCubeSetAddCategory(BYTE bCategory) const;
		const char*	GetItemNameBySetValue(int iSetValue, const char* c_szItemName);
#endif

		void	SetChangeLookWindowOpen(const bool blOpen);
		bool	GetChangeLookWindowOpen() const;
		void	SetChangeLookWindowType(const bool blType);
		bool	GetChangeLookWindowType() const;

		void	SetChangeLookItemData(TPacketGCTransmutationItemSet& data);
		TPacketGCTransmutationItemSet*	GetChangeLookItemData(const BYTE bSlotType);
		void	DelChangeLookItemData(const BYTE bSlotType, const bool bClearEffect = false);
		
		void	SetChangeLookFreeItemData(TPacketGCTransmutationItemSet& data);
		TPacketGCTransmutationItemSet*	GetChangeLookFreeItemData();
		void	DelChangeLookFreeItemData(const bool bClearEffect = false);
#endif

		void	RequestAddLocalQuickSlot(DWORD dwLocalSlotIndex, DWORD dwWndType, DWORD dwWndItemPos);
		void	RequestAddToEmptyLocalQuickSlot(DWORD dwWndType, DWORD dwWndItemPos);
		void	RequestMoveGlobalQuickSlotToLocalQuickSlot(DWORD dwGlobalSrcSlotIndex, DWORD dwLocalDstSlotIndex);
		void	RequestDeleteGlobalQuickSlot(DWORD dwGlobalSlotIndex);
		void	RequestUseLocalQuickSlot(DWORD dwLocalSlotIndex);
		DWORD	LocalQuickSlotIndexToGlobalQuickSlotIndex(DWORD dwLocalSlotIndex);

		void	GetGlobalQuickSlotData(DWORD dwGlobalSlotIndex, DWORD* pdwWndType, DWORD* pdwWndItemPos);
		void	GetLocalQuickSlotData(DWORD dwSlotPos, DWORD* pdwWndType, DWORD* pdwWndItemPos);
		void	RemoveQuickSlotByValue(int iType, int iPosition);

		char	IsItem(TItemPos SlotIndex);

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
		bool	IsBeltInventorySlot(TItemPos Cell);
#endif
#ifdef ENABLE_SPECIAL_INVENTORY
		bool	IsSkillBookInventorySlot(TItemPos Cell);
		bool	IsUpgradeItemsInventorySlot(TItemPos Cell);
		bool	IsStoneInventorySlot(TItemPos Cell);
		bool	IsGiftBoxInventorySlot(TItemPos Cell);
		bool	IsChangersInventorySlot(TItemPos Cell);
#endif
		bool	IsInventorySlot(TItemPos SlotIndex);
		bool	IsEquipmentSlot(TItemPos SlotIndex);

		bool	IsEquipItemInSlot(TItemPos iSlotIndex);


		// Quickslot
		int		GetQuickPage();
		void	SetQuickPage(int nPageIndex);
		void	AddQuickSlot(int QuickslotIndex, char IconType, char IconPosition);
		void	DeleteQuickSlot(int QuickslotIndex);
		void	MoveQuickSlot(int Source, int Target);


		// Skill
		void	SetSkill(DWORD dwSlotIndex, DWORD dwSkillIndex);
		bool	GetSkillSlotIndex(DWORD dwSkillIndex, DWORD* pdwSlotIndex);
		int		GetSkillIndex(DWORD dwSlotIndex);
		int		GetSkillGrade(DWORD dwSlotIndex);
		int		GetSkillLevel(DWORD dwSlotIndex);
		float	GetSkillCurrentEfficientPercentage(DWORD dwSlotIndex);
		float	GetSkillNextEfficientPercentage(DWORD dwSlotIndex);
		void	SetSkillLevel(DWORD dwSlotIndex, DWORD dwSkillLevel);
		void	SetSkillLevel_(DWORD dwSkillIndex, DWORD dwSkillGrade, DWORD dwSkillLevel);
		BOOL	IsToggleSkill(DWORD dwSlotIndex);
		void	ClickSkillSlot(DWORD dwSlotIndex);
		void	ChangeCurrentSkillNumberOnly(DWORD dwSlotIndex);
		bool	FindSkillSlotIndexBySkillIndex(DWORD dwSkillIndex, DWORD * pdwSkillSlotIndex);

		void	SetSkillCoolTime(DWORD dwSkillIndex);
		void	EndSkillCoolTime(DWORD dwSkillIndex);

		float	GetSkillCoolTime(DWORD dwSlotIndex);
		float	GetSkillElapsedCoolTime(DWORD dwSlotIndex);
		BOOL	IsSkillActive(DWORD dwSlotIndex);
		BOOL	IsSkillCoolTime(DWORD dwSlotIndex);
		void	UseGuildSkill(DWORD dwSkillSlotIndex);
		bool	AffectIndexToSkillSlotIndex(UINT uAffect, DWORD* pdwSkillSlotIndex);
		bool	AffectIndexToSkillIndex(DWORD dwAffectIndex, DWORD * pdwSkillIndex);

		void	SetAffect(UINT uAffect);
		void	ResetAffect(UINT uAffect);
		void	ClearAffects();


		// Target
#ifdef __AUTO_HUNT__
		void	SetTarget(DWORD dwVID, BOOL bForceChange = TRUE, bool isAutoAttack = false);
#else
		void	SetTarget(DWORD dwVID, BOOL bForceChange = TRUE);
#endif
		void	OpenCharacterMenu(DWORD dwVictimActorID);
		DWORD	GetTargetVID();
        void SetOptionQueueAttackTarget(DWORD vid) {
            if (!vid) { __ClearAutoAttackTargetActorID(); __ClearReservedAction(); NEW_Stop(); return; }
            SetTarget(vid); __SetAutoAttackTargetActorID(vid);
        }


		// Party
		void	ExitParty();
		void	AppendPartyMember(DWORD dwPID, const char *c_szName);
		void	LinkPartyMember(DWORD dwPID, DWORD dwVID);
		void	UnlinkPartyMember(DWORD dwPID);
		void	UpdatePartyMemberInfo(DWORD dwPID, BYTE byState, BYTE byHPPercentage);
		void	UpdatePartyMemberAffect(DWORD dwPID, BYTE byAffectSlotIndex, short sAffectNumber);
		void	RemovePartyMember(DWORD dwPID);
		bool	IsPartyMemberByVID(DWORD dwVID);
		bool	IsPartyMemberByName(const char *c_szName);
		bool	GetPartyMemberPtr(DWORD dwPID, TPartyMemberInfo ** ppPartyMemberInfo);
		bool	PartyMemberPIDToVID(DWORD dwPID, DWORD * pdwVID);
		bool	PartyMemberVIDToPID(DWORD dwVID, DWORD * pdwPID);
		bool	IsSamePartyMember(DWORD dwVID1, DWORD dwVID2);


		// Fight
		void	RememberChallengeInstance(DWORD dwVID);
		void	RememberRevengeInstance(DWORD dwVID);
		void	RememberCantFightInstance(DWORD dwVID);
		void	ForgetInstance(DWORD dwVID);
		bool	IsChallengeInstance(DWORD dwVID);
		bool	IsRevengeInstance(DWORD dwVID);
		bool	IsCantFightInstance(DWORD dwVID);


		// Private Shop
		void	OpenPrivateShop();
		void	ClosePrivateShop();
		bool	IsOpenPrivateShop();

		// Horse SKill
		DWORD	GetHorseSkillSlotIndex();

		// Stamina
		void	StartStaminaConsume(DWORD dwConsumePerSec, DWORD dwCurrentStamina);
		void	StopStaminaConsume(DWORD dwCurrentStamina);


		// PK Mode
		DWORD	GetPKMode();


		// Combo
		void	SetComboSkillFlag(BOOL bFlag);


		// System
		void	SetMovableGroundDistance(float fDistance);


		// Emotion
		void	ActEmotion(DWORD dwEmotionID);
		void	StartEmotionProcess();
		void	EndEmotionProcess();

		// Function Only For Console System
		BOOL	__ToggleCoolTime();
		BOOL	__ToggleLevelLimit();

		__inline const	SAutoPotionInfo& GetAutoPotionInfo(int type) const	{ return m_kAutoPotionInfo[type]; }
		__inline		SAutoPotionInfo& GetAutoPotionInfo(int type)		{ return m_kAutoPotionInfo[type]; }
		__inline void					 SetAutoPotionInfo(int type, const SAutoPotionInfo& info)	{ m_kAutoPotionInfo[type] = info; }

#ifdef ENABLE_ULTIMATE_REGEN
		bool	LoadNewRegen();
		bool	CheckBossSafeRange();
#endif

		bool	IsFishing();
		BYTE	GetSex();
		DWORD	CountEmptyInventory();
		BYTE	GetChannel() const { return m_byChannel; }
		void	SetChannel(BYTE byChannel) { m_byChannel = byChannel; }
		bool	GetBattleRoyaleEnable() const { return m_bBattleRoyaleEnable; }
		void	SetBattleRoyaleEnable(bool bEnable) { m_bBattleRoyaleEnable = bEnable; }
		bool	GetWorldBossEnable() const { return m_bWorldBossEnable; }
		void	SetWorldBossEnable(bool bEnable) { m_bWorldBossEnable = bEnable; }
		BYTE	GetCurrentEquipmentPageIndex() const { return m_bCurrentEquipmentPageIndex; }
		void	SetCurrentEquipmentPageIndex(BYTE bIndex) { m_bCurrentEquipmentPageIndex = bIndex; }

	protected:
		TQuickSlot &	__RefLocalQuickSlot(int SlotIndex);
		TQuickSlot &	__RefGlobalQuickSlot(int SlotIndex);


		DWORD	__GetLevelAtk();
		DWORD	__GetStatAtk();
		DWORD	__GetWeaponAtk(DWORD dwWeaponPower);
		DWORD	__GetTotalAtk(DWORD dwWeaponPower, DWORD dwRefineBonus);
		DWORD	__GetRaceStat();
		DWORD	__GetHitRate();
		DWORD	__GetEvadeRate();

		void	__UpdateBattleStatus();

		void	__DeactivateSkillSlot(DWORD dwSlotIndex);
		void	__ActivateSkillSlot(DWORD dwSlotIndex);

		void	__OnPressSmart(CInstanceBase& rkInstMain, bool isAuto);
		void	__OnClickSmart(CInstanceBase& rkInstMain, bool isAuto);

		void	__OnPressItem(CInstanceBase& rkInstMain, DWORD dwPickedItemID);
		void	__OnPressActor(CInstanceBase& rkInstMain, DWORD dwPickedActorID, bool isAuto);
		void	__OnPressGround(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround);
		void	__OnPressScreen(CInstanceBase& rkInstMain);

		void	__OnClickActor(CInstanceBase& rkInstMain, DWORD dwPickedActorID, bool isAuto);
		void	__OnClickItem(CInstanceBase& rkInstMain, DWORD dwPickedItemID);
		void	__OnClickGround(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround);

		bool	__IsMovableGroundDistance(CInstanceBase& rkInstMain, const TPixelPosition& c_rkPPosPickedGround);

		bool	__GetPickedActorPtr(CInstanceBase** pkInstPicked);

		bool	__GetPickedActorID(DWORD* pdwActorID);
		bool	__GetPickedItemID(DWORD* pdwItemID);
		bool	__GetPickedGroundPos(TPixelPosition* pkPPosPicked);

		void	__ClearReservedAction();
		void	__ReserveClickItem(DWORD dwItemID);
		void	__ReserveClickActor(DWORD dwActorID);
		void	__ReserveClickGround(const TPixelPosition& c_rkPPosPickedGround);
		void	__ReserveUseSkill(DWORD dwActorID, DWORD dwSkillSlotIndex, DWORD dwRange);

		void	__ReserveProcess_ClickActor();

		void	__ShowPickedEffect(const TPixelPosition& c_rkPPosPickedGround);
		void	__SendClickActorPacket(CInstanceBase& rkInstVictim);

		void	__SetAutoAttackTargetActorID(DWORD dwActorID);

		void	NEW_ShowEffect(int dwEID, TPixelPosition kPPosDst);

		void	NEW_SetMouseSmartState(int eMBS, bool isAuto);
		void	NEW_SetMouseMoveState(int eMBS);
		void	NEW_SetMouseCameraState(int eMBS);
		void	NEW_GetMouseDirRotation(float fScrX, float fScrY, float *pfDirRot);
		void	NEW_GetMultiKeyDirRotation(bool isLeft, bool isRight, bool isUp, bool isDown, float *pfDirRot);

		float	GetDegreeFromDirection(int iUD, int iLR);
		float	GetDegreeFromPosition(int ix, int iy, int iHalfWidth, int iHalfHeight);

		bool	CheckCategory(int iCategory);
		bool	CheckAbilitySlot(int iSlotIndex);

		void	RefreshKeyWalkingDirection();
		void	NEW_RefreshMouseWalkingDirection();


		// Instances
		void	RefreshInstances();

		bool	__CanShot(CInstanceBase& rkInstMain, CInstanceBase& rkInstTarget);
		bool	__CanUseSkill();

		bool	__CanMove();

		bool	__CanAttack();
		bool	__CanChangeTarget();

		bool	__CheckSkillUsable(DWORD dwSlotIndex);
		void	__UseCurrentSkill();
		void	__UseChargeSkill(DWORD dwSkillSlotIndex);
		bool	__UseSkill(DWORD dwSlotIndex);
		bool	__CheckSpecialSkill(DWORD dwSkillIndex);

		bool	__CheckRestSkillCoolTime(DWORD dwSkillSlotIndex);
		bool	__CheckShortLife(TSkillInstance & rkSkillInst, CPythonSkill::TSkillData& rkSkillData);
		bool	__CheckShortMana(TSkillInstance & rkSkillInst, CPythonSkill::TSkillData& rkSkillData);
		bool	__CheckShortArrow(TSkillInstance & rkSkillInst, CPythonSkill::TSkillData& rkSkillData);
		bool	__CheckDashAffect(CInstanceBase& rkInstMain);

		void	__SendUseSkill(DWORD dwSkillSlotIndex, DWORD dwTargetVID);
		void	__RunCoolTime(DWORD dwSkillSlotIndex);

		BYTE	__GetSkillType(DWORD dwSkillSlotIndex);

		bool	__IsReservedUseSkill(DWORD dwSkillSlotIndex);
		bool	__IsMeleeSkill(CPythonSkill::TSkillData& rkSkillData);
		bool	__IsChargeSkill(CPythonSkill::TSkillData& rkSkillData);
		DWORD	__GetSkillTargetRange(CPythonSkill::TSkillData& rkSkillData);
		bool	__SearchNearTarget();
		bool	__IsUsingChargeSkill();

		bool	__ProcessEnemySkillTargetRange(CInstanceBase& rkInstMain, CInstanceBase& rkInstTarget, CPythonSkill::TSkillData& rkSkillData, DWORD dwSkillSlotIndex);

		bool __HasEnoughArrow();
		bool __HasItem(DWORD dwItemID);
		DWORD __GetPickableDistance();

		CInstanceBase *__GetTargetActorPtr();
		void __ClearTarget();
		DWORD __GetTargetVID();
		void __SetTargetVID(DWORD dwVID);
		bool __IsSameTargetVID(DWORD dwVID);
		bool __IsTarget();
		bool __ChangeTargetToPickedInstance();

		CInstanceBase *__GetSkillTargetInstancePtr(CPythonSkill::TSkillData& rkSkillData);
		CInstanceBase *__GetAliveTargetInstancePtr();
		CInstanceBase *__GetDeadTargetInstancePtr();

		BOOL __IsRightButtonSkillMode();

		void __Update_AutoAttack();
		void __Update_NotifyGuildAreaEvent();

		BOOL __IsProcessingEmotion();

	public:
		void __ClearAutoAttackTargetActorID();

	protected:
		PyObject *m_ppyGameWindow;

		std::map<DWORD, DWORD> m_skillSlotDict;
		std::string m_stName;
		DWORD m_dwMainCharacterIndex;
		DWORD m_dwRace;
		DWORD m_dwWeaponMinPower;
		DWORD m_dwWeaponMaxPower;
		DWORD m_dwWeaponMinMagicPower;
		DWORD m_dwWeaponMaxMagicPower;
		DWORD m_dwWeaponAddPower;

		DWORD m_dwSendingTargetVID;
		float m_fTargetUpdateTime;

		DWORD m_dwAutoAttackTargetVID;

		EMode m_eReservedMode;
		float m_fReservedDelayTime;

		float m_fMovDirRot;

		bool m_isUp;
		bool m_isDown;
		bool m_isLeft;
		bool m_isRight;
		bool m_isAtkKey;
		bool m_isDirKey;
		bool m_isCmrRot;
		bool m_isSmtMov;
		bool m_isDirMov;

		float m_fCmrRotSpd;

		TPlayerStatus m_playerStatus;

		UINT m_iComboOld;
		DWORD m_dwVIDReserved;
		DWORD m_dwIIDReserved;

		DWORD m_dwcurSkillSlotIndex;
		DWORD m_dwSkillSlotIndexReserved;
		DWORD m_dwSkillRangeReserved;

		TPixelPosition m_kPPosInstPrev;
		TPixelPosition m_kPPosReserved;

		BOOL m_bisProcessingEmotion;

		BOOL m_isDestPosition;
		int m_ixDestPos;
		int m_iyDestPos;
		int m_iLastAlarmTime;

		std::map<DWORD, TPartyMemberInfo> m_PartyMemberMap;

		std::set<DWORD> m_ChallengeInstanceSet;
		std::set<DWORD> m_RevengeInstanceSet;
		std::set<DWORD> m_CantFightInstanceSet;

		bool m_isOpenPrivateShop;
#ifdef ENABLE_MOVE_COSTUME_ACCE_ATTR
		int m_ItemCombinationWindowActivedItemSlot[4];
#endif
		bool m_isObserverMode;

		BOOL m_isConsumingStamina;
		float m_fCurrentStamina;
		float m_fConsumeStaminaPerSec;

		DWORD m_inGuildAreaID;

		BOOL m_sysIsCoolTime;
		BOOL m_sysIsLevelLimit;

	protected:
		TPixelPosition m_MovingCursorPosition;
		float m_fMovingCursorSettingTime;
		DWORD m_adwEffect[EFFECT_NUM];

		DWORD m_dwVIDPicked;
		DWORD m_dwIIDPicked;
		int m_aeMBFButton[MBT_NUM];

		DWORD m_dwTargetVID;
		DWORD m_dwTargetEndTime;
		DWORD m_dwPlayTime;

		int m_iLeftSeatTimeIndex;
		int m_iLeftSeatLogoutTimeIndex;

#ifdef ENABLE_ULTIMATE_REGEN
		std::vector<TNewRegen> m_eventData;
#endif

#ifdef ENABLE_CHANGE_LOOK_SYSTEM
		bool bIsChangeLookWindowOpen;
		bool bChangeLookWindowType;
		TPacketGCTransmutationItemSet m_ChangeLookSlot[static_cast<BYTE>(ETRANSMUTATIONSLOTTYPE::TRANSMUTATION_SLOT_MAX)];
		TPacketGCTransmutationItemSet m_ChangeLookFreeYangItemSlot;
#endif

		SAutoPotionInfo m_kAutoPotionInfo[AUTO_POTION_TYPE_NUM];

	protected:
		float MOVABLE_GROUND_DISTANCE;

	private:
		std::map<DWORD, DWORD> m_kMap_dwAffectIndexToSkillIndex;

#ifdef ENABLE_AURA_COSTUME_SYSTEM
	public:
		enum EItemAuraSockets
		{
			ITEM_SOCKET_AURA_DRAIN_ITEM_VNUM,
			ITEM_SOCKET_AURA_CURRENT_LEVEL,
			ITEM_SOCKET_AURA_BOOST,
		};

		enum EItemAuraMaterialValues
		{
			ITEM_VALUE_AURA_MATERIAL_EXP,
		};

		enum EItemAuraBoostValues
		{
			ITEM_VALUE_AURA_BOOST_PERCENT,
			ITEM_VALUE_AURA_BOOST_TIME,
			ITEM_VALUE_AURA_BOOST_UNLIMITED,
		};

	private:
		typedef struct SAuraRefineInfo
		{
			BYTE bAuraRefineInfoLevel;
			BYTE bAuraRefineInfoExpPercent;
		} TAuraRefineInfo;

		std::vector<TItemData> m_AuraItemInstanceVector;

	protected:
		bool m_bAuraWindowOpen;
		BYTE m_bOpenedAuraWindowType;
		TItemPos m_AuraRefineActivatedCell[AURA_SLOT_MAX];
		TAuraRefineInfo m_bAuraRefineInfo[AURA_REFINE_INFO_SLOT_MAX];

		void __ClearAuraRefineWindow();

	public:
		void SetAuraRefineWindowOpen(BYTE wndType);
		BYTE GetAuraRefineWindowType() const { return m_bOpenedAuraWindowType; };

		bool IsAuraRefineWindowOpen() const { return m_bAuraWindowOpen; }
		bool IsAuraRefineWindowEmpty();

		void SetAuraRefineInfo(BYTE bAuraRefineInfoSlot, BYTE bAuraRefineInfoLevel, BYTE bAuraRefineInfoExpPercent);
		BYTE GetAuraRefineInfoLevel(BYTE bAuraRefineInfoSlot);
		BYTE GetAuraRefineInfoExpPct(BYTE bAuraRefineInfoSlot);

		void SetAuraItemData(BYTE bSlotIndex, const TItemData& rItemInstance);
		void DelAuraItemData(BYTE bSlotIndex);

		BYTE FineMoveAuraItemSlot();
		BYTE GetAuraCurrentItemSlotCount();

		BOOL GetAuraItemDataPtr(BYTE bSlotIndex, TItemData** ppInstance);

		void SetActivatedAuraSlot(BYTE bSlotIndex, TItemPos ItemCell);
		BYTE FindActivatedAuraSlot(TItemPos ItemCell);
		TItemPos FindUsingAuraSlot(BYTE bSlotIndex);
#endif

#ifdef __AUTO_HUNT__
	public:
		void	AutoHuntStop();
		void	ClearAutoHunt();
		void	AutoHuntLoop();
		void	AutoHuntSettings(const char* szArgument);
		void	SetAutoHuntDieCheck(const int iSec);
		void	SetAutoHuntSleep(const int iSec);
		bool	GetAutoHuntStatus() { return m_bAutoHuntStatus; }
		const TPixelPosition* GetAutoHuntStartPosition() { return &m_bAutoHuntStartPoint; }

	protected:
		std::vector<std::pair<BYTE, int>> m_vecAutoHuntItems;
		std::vector<BYTE> m_vecAutoSkills;
		TPixelPosition	m_bAutoHuntStartPoint;
		bool m_bAutoHuntStatus, m_bAutoHuntMob, m_bAutoHuntStone;
		int	m_iLastAutoHuntDieCheck, m_iAutoHuntSleepTime;
#endif

#ifdef ENABLE_GROWTH_PET_SYSTEM
	public:
		typedef struct SPetSkill
		{
			BYTE bSkill;
			BYTE bLevel;
			DWORD dwCooltime;
			BYTE dwFormula1[PET_GROWTH_SKILL_LEVEL_MAX];
			WORD dwFormula2[PET_GROWTH_SKILL_LEVEL_MAX];
			BYTE dwFormula3[PET_GROWTH_SKILL_LEVEL_MAX];

			SPetSkill()
			{
				bSkill = 0;
				bLevel = 0;
				memset(dwFormula1, 0, sizeof(dwFormula1));
				memset(dwFormula2, 0, sizeof(dwFormula2));
				memset(dwFormula3, 0, sizeof(dwFormula3));
			}
		} TPetSkill;

		typedef struct SPetSkillData
		{
			BYTE bSkillCount;
			TPetSkill aSkillInfo[PET_SKILL_COUNT_MAX];

			SPetSkillData()
			{
				bSkillCount = 0;
			}
		} TPetSkillData;

		typedef struct SPetData
		{
			DWORD dwID;
			DWORD dwSummonItemVnum;
			std::string	strName;

			DWORD dwPoints[POINT_UPBRINGING_MAX_NUM];
			TPetSkillData skillData;

			bool bIsExchange;
			bool bIsShop;

			SPetData()
			{
				dwSummonItemVnum = 0;
				strName = "";

				memset(&dwPoints, 0, sizeof(dwPoints));

				bIsExchange = false;
			}

			void SetSummonItemVnum(DWORD dwVnum) { dwSummonItemVnum = dwVnum; }

			DWORD GetSummonItemVnum() { return dwSummonItemVnum; }
			std::string	GetName() { return strName; }
			time_t GetBirthday() { return dwPoints[POINT_UPBRINGING_BIRTHDAY]; }
			DWORD GetLevel() { return dwPoints[POINT_UPBRINGING_PET_LEVEL]; }
			DWORD GetEXP() { return dwPoints[POINT_UPBRINGING_PET_EXP]; }
			DWORD GetItemEXP() { return dwPoints[POINT_UPBRINGING_PET_ITEM_EXP]; }
			DWORD GetNextEXP() { return dwPoints[POINT_UPBRINGING_PET_NEXT_EXP]; }
			DWORD GetEvolution() { return dwPoints[POINT_UPBRINGING_PET_EVOL_LEVEL]; }
			DWORD GetHP() { return dwPoints[POINT_UPBRINGING_PET_HP]; }
			DWORD GetSP() { return dwPoints[POINT_UPBRINGING_PET_SP]; }
			DWORD GetDef() { return dwPoints[POINT_UPBRINGING_PET_DEF_GRADE]; }
			DWORD GetSkillCount() { return skillData.bSkillCount; }
			DWORD GetDuration() { return dwPoints[POINT_UPBRINGING_DURATION]; }
			DWORD GetMaxDuration() { return dwPoints[POINT_UPBRINGING_MAX_DURATION]; }
		} TPetData;

		typedef std::unordered_map<DWORD, TPetData> TPetDataMap;

		void SetOpenPetHatchingWindow(bool isOpen) { m_isOpenPetHatchingWindow = isOpen; };
		void SetOpenPetNameChangeWindow(bool isOpen) { m_isOpenPetNameChangeWindow = isOpen; };
		void SetOpenPetFeedWindow(bool isOpen) { m_isOpenPetFeedWindow = isOpen; };
		void SetOpenSafeBox(bool isOpen) { m_isOpenSafebox = isOpen; };
		void SetOpenMall(bool isOpen) { m_isOpenMall = isOpen; };

		bool IsOpenPetHatchingWindow() { return m_isOpenPetHatchingWindow; };
		bool IsOpenPetNameChangeWindow() { return m_isOpenPetNameChangeWindow; };
		bool IsOpenPetFeedWindow() { return m_isOpenPetFeedWindow; };
		bool IsOpenSafeBox() { return m_isOpenSafebox; };
		bool IsOpenMall() { return m_isOpenMall; };

		void SetPetData(TPetData* pPetData);
		bool GetPetInfo(DWORD dwID, TPetData** ppTPetData);
		void DeletePetData(DWORD dwID);
		void ClearPetExchangeData();

		void SetPetSkillData(DWORD dwID, TPetSkillUpdatePacket* pPetSkillData);
		bool GetPetSkill(DWORD dwID, TPetSkillData** ppPetSkillData);
		void SetPetSkillCooltime(DWORD dwID, BYTE bSlot, DWORD dwCooltime);

		void SetPetStatus(DWORD dwID, BYTE byPoint, DWORD dwValue);
		long GetPetStatus(DWORD dwID, BYTE byPoint);

		void SetActivePet(DWORD dwID);
		DWORD GetActivePet();

		DWORD CanUseGrowthPetQuickSlot(DWORD dwLocalSlotIndex);

	protected:
		bool m_isOpenPetHatchingWindow;
		bool m_isOpenPetNameChangeWindow;
		bool m_isOpenPetFeedWindow;
		bool m_isOpenSafebox;
		bool m_isOpenMall;

		TPetDataMap m_PetDataMap;
		DWORD m_dwActivePetID;
#endif

#ifdef ENABLE_ATTENDANCE_EVENT
		std::vector<THitCountInfo> m_hitCount;
		std::vector<TRewardItem> m_rewardItems;
#endif

	protected:
		BYTE	m_byChannel;
		bool	m_bBattleRoyaleEnable;
		bool	m_bWorldBossEnable;
		BYTE	m_bCurrentEquipmentPageIndex;
};

extern const int c_iFastestSendingCount;
extern const int c_iSlowestSendingCount;
extern const float c_fFastestSendingDelay;
extern const float c_fSlowestSendingDelay;
extern const float c_fRotatingStepTime;

extern const float c_fComboDistance;
extern const float c_fPickupDistance;
extern const float c_fClickDistance;
