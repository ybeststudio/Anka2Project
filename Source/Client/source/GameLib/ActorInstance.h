#pragma once

#include "StdAfx.h"
#include "FlyTarget.h"
#include "RaceData.h"
#include "RaceMotionData.h"
#include "PhysicsObject.h"
#include "ActorInstanceInterface.h"
#include "Interface.h"

#ifdef ENABLE_SKILL_COLOR_SYSTEM
	#include "../UserInterface/GameType.h"
#endif

class CItemData;
class CWeaponTrace;
class IFlyEventHandler;
class CSpeedTreeWrapper;
typedef std::shared_ptr<CSpeedTreeWrapper> SpeedTreeWrapperPtr;

class IMobProto : public CSingleton<IMobProto>
{
	public:
		IMobProto() {}
		virtual ~IMobProto() {}

		virtual bool FindRaceType(UINT eRace, UINT* puType);
};

class CActorInstance : public IActorInstance, public IFlyTargetableObject
{
	public:
		class IEventHandler
		{
			public:
				static IEventHandler* GetEmptyPtr();

			public:
				struct SState
				{
					TPixelPosition kPPosSelf;
					FLOAT fAdvRotSelf;
				};

			public:
				IEventHandler() {}
				virtual ~IEventHandler() {}

				virtual void OnSyncing(const SState& c_rkState) = 0;
				virtual void OnWaiting(const SState& c_rkState) = 0;
				virtual void OnMoving(const SState& c_rkState) = 0;
				virtual void OnMove(const SState& c_rkState) = 0;
				virtual void OnStop(const SState& c_rkState) = 0;
				virtual void OnWarp(const SState& c_rkState) = 0;
				virtual void OnSetAffect(UINT uAffect) = 0;
				virtual void OnResetAffect(UINT uAffect) = 0;
				virtual void OnClearAffects() = 0;

				virtual void OnAttack(const SState& c_rkState, WORD wMotionIndex) = 0;
				virtual void OnUseSkill(const SState& c_rkState, UINT uMotSkill, UINT uMotLoopCount) = 0;

				virtual void OnHit(UINT uSkill, CActorInstance& rkActorVictim, BOOL isSendPacket) = 0;

				virtual void OnChangeShape() = 0;
		};

	private:
		static IBackground& GetBackground();

	public:
		static bool IsDirLine();

	public:

		enum EType
		{
			TYPE_ENEMY,
			TYPE_NPC,
			TYPE_STONE,
			TYPE_WARP,
			TYPE_DOOR,
			TYPE_BUILDING,
			TYPE_PC,
			TYPE_POLY,
			TYPE_HORSE,
			TYPE_GOTO,
#ifdef ENABLE_PET_SYSTEM
			TYPE_PET,
#endif
#ifdef ENABLE_MOUNT_SYSTEM
			TYPE_MOUNT,
#endif
#ifdef ENABLE_GROWTH_PET_SYSTEM
			TYPE_GROWTH_PET,
#endif

			TYPE_OBJECT, // Only For Client
		};

		enum ERenderMode
		{
			RENDER_MODE_NORMAL,
			RENDER_MODE_BLEND,
			RENDER_MODE_ADD,
			RENDER_MODE_MODULATE,
		};

		/////////////////////////////////////////////////////////////////////////////////////
		// Motion Queueing System
		enum EMotionPushType
		{
			MOTION_TYPE_NONE,
			MOTION_TYPE_ONCE,
			MOTION_TYPE_LOOP,
		};

		typedef struct SReservingMotionNode
		{
			EMotionPushType	iMotionType;

			float			fStartTime;
			float			fBlendTime;
			float			fDuration;
			float			fSpeedRatio;

			DWORD			dwMotionKey;
		} TReservingMotionNode;

		struct SCurrentMotionNode
		{
			EMotionPushType	iMotionType;
			DWORD			dwMotionKey;

			DWORD			dwcurFrame;
			DWORD			dwFrameCount;

			float			fStartTime;
			float			fEndTime;
			float			fSpeedRatio;

			int				iLoopCount;
			UINT			uSkill;
		};

		typedef std::deque<TReservingMotionNode> TMotionDeque;
		/////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////
		// Motion Event
		typedef struct SMotionEventInstance
		{
			int iType;
			int iMotionEventIndex;
			float fStartingTime;

			const CRaceMotionData::TMotionEventData * c_pMotionData;
		} TMotionEventInstance;

		typedef std::list<TMotionEventInstance> TMotionEventInstanceList;
		typedef TMotionEventInstanceList::iterator TMotionEventInstanceListIterator;
		/////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////
		// For Collision Detection
		typedef struct SCollisionPointInstance
		{
			const NRaceData::TCollisionData * c_pCollisionData;
			BOOL isAttached;
			DWORD dwModelIndex;
			DWORD dwBoneIndex;
			CDynamicSphereInstanceVector SphereInstanceVector;
		} TCollisionPointInstance;
		typedef std::list<TCollisionPointInstance> TCollisionPointInstanceList;
		typedef TCollisionPointInstanceList::iterator TCollisionPointInstanceListIterator;

		typedef std::map<CActorInstance*, float> THittedInstanceMap;
		typedef std::map<const NRaceData::THitData *, THittedInstanceMap> THitDataMap;
		struct SSplashArea
		{
			BOOL isEnableHitProcess;
			UINT uSkill;
			MOTION_KEY MotionKey;
			float fDisappearingTime;
			const CRaceMotionData::TMotionAttackingEventData * c_pAttackingEvent;
			CDynamicSphereInstanceVector SphereInstanceVector;

			THittedInstanceMap HittedInstanceMap;
		};

		typedef struct SHittingData
		{
			BYTE byAttackingType;
			DWORD dwMotionKey;
			BYTE byEventIndex;
		} THittingData;
		/////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////
		// For Attaching
		enum EAttachEffect
		{
			EFFECT_LIFE_NORMAL,
			EFFECT_LIFE_INFINITE,
			EFFECT_LIFE_WITH_MOTION,
		};

		struct TAttachingEffect
		{
			DWORD dwEffectIndex;
			int iBoneIndex;
			DWORD dwModelIndex;
			D3DXMATRIX matTranslation;
			BOOL isAttaching;

			int iLifeType;
			DWORD dwEndTime;
		};
		/////////////////////////////////////////////////////////////////////////////////////

	public:
		static void ShowDirectionLine(bool isVisible);
		static void DestroySystem();

	public:
		CActorInstance();
		virtual ~CActorInstance();

		void INSTANCEBASE_Transform();
		void INSTANCEBASE_Deform();

		void Destroy();

		void Move();
		void Stop(float fBlendingTime=0.15f);

		void SetMainInstance();

		void SetParalysis(bool isParalysis);
		void SetFaint(bool isFaint);
		void SetSleep(bool isSleep);
		void SetResistFallen(bool isResistFallen);

		void SetAttackSpeed(float fAtkSpd);
		void SetMoveSpeed(float fMovSpd);

		void SetMaterialAlpha(DWORD dwAlpha);
		void SetMaterialColor(DWORD dwColor);

		void SetEventHandler(IEventHandler* pkEventHandler);

		bool SetRace(DWORD eRace);
		void SetHair(DWORD eHair
#ifdef ENABLE_HAIR_SPECULAR
			, float fSpecular = 0.0f
#endif
		);
#ifdef ENABLE_ACCE_COSTUME_SYSTEM
		void AttachAcce(CItemData * pItemData, float fSpecular = 0.0f);
#endif
		void SetVirtualID(DWORD dwVID);

		void SetShape(DWORD eShape, float fSpecular=0.0f);
		void ChangeMaterial(const char *c_szFileName);

#ifdef ENABLE_SKILL_COLOR_SYSTEM
		DWORD* GetSkillColorByMotionID(DWORD dwMotionID) { return m_dwSkillColor[dwMotionID]; };
		DWORD* GetSkillColorByEffectID(DWORD dwEffectID);
		void ChangeSkillColor(const DWORD *c_dwSkillColor);

	protected:
		DWORD m_dwSkillColor[CRaceMotionData::SKILL_NUM][ESkillColorLength::MAX_EFFECT_COUNT];
#endif

	public:
		void SetComboType(WORD wComboType);

		DWORD GetRace();
		DWORD GetVirtualID();

		UINT GetActorType() const;
		void SetActorType(UINT eType);

		bool CanAct();
		bool CanMove();
		bool CanAttack();
		bool CanUseSkill();

		bool IsPC();
		bool IsNPC();
		bool IsHorse();
		bool IsPet();
#ifdef ENABLE_GROWTH_PET_SYSTEM
		bool IsGrowthPet();
#endif
		bool IsMount();
		bool IsEnemy();
		bool IsStone();
		bool IsWarp();
		bool IsGoto();
		bool IsObject();
		bool IsDoor();
		bool IsPoly();
#ifdef ENABLE_RENEWAL_OFFLINESHOP
		bool IsShop();
#endif

		bool IsBuilding();

		bool IsHandMode();
		bool IsBowMode();
		bool IsTwoHandMode();

		void AttachWeapon(DWORD dwItemIndex,DWORD dwParentPartIndex = CRaceData::PART_MAIN, DWORD dwPartIndex = CRaceData::PART_WEAPON);
		void AttachWeapon(DWORD dwParentPartIndex, DWORD dwPartIndex, CItemData * pItemData);

		void RefreshActorInstance();
#ifdef ENABLE_AURA_COSTUME_SYSTEM
		void ChangePart(DWORD dwPartIndex, DWORD dwItemIndex);
#endif
		DWORD GetPartItemID(DWORD dwPartIndex);

		BOOL GetAttachingBoneName(DWORD dwPartIndex, const char ** c_szBoneName);
		void UpdateAttachingInstances();
		void  DettachEffect(DWORD dwEID);
		DWORD AttachEffectByName(DWORD dwParentPartIndex, const char *c_pszBoneName, const char *c_pszEffectFileName);
		DWORD AttachEffectByID(DWORD dwParentPartIndex, const char *c_pszBoneName, DWORD dwEffectID, const D3DXVECTOR3 *c_pv3Position = NULL
#ifdef ENABLE_SKILL_COLOR_SYSTEM
			, DWORD * dwSkillColor = NULL
#endif
#ifdef ENABLE_AURA_COSTUME_SYSTEM
			, float fParticleScale = 1.0f, const D3DXVECTOR3 *c_pv3MeshScale = NULL
#endif
		);

		DWORD AttachSmokeEffect(DWORD eSmoke);

		void SetMotionMode(int iMotionMode);
		int GetMotionMode();
		void SetLoopMotion(DWORD dwMotion, float fBlendTime = 0.1f, float fSpeedRatio=1.0f);
		bool InterceptOnceMotion(DWORD dwMotion, float fBlendTime = 0.1f, UINT uSkill=0, float fSpeedRatio=1.0f);
		bool InterceptLoopMotion(DWORD dwMotion, float fBlendTime = 0.1f);
		bool PushOnceMotion(DWORD dwMotion, float fBlendTime = 0.1f, float fSpeedRatio=1.0f);
		bool PushLoopMotion(DWORD dwMotion, float fBlendTime = 0.1f, float fSpeedRatio=1.0f);
		void SetMotionLoopCount(int iCount);

		bool IsPushing();

		BOOL isLock();
		BOOL IsUsingSkill();
		BOOL CanCheckAttacking();
		BOOL CanCancelSkill();

		bool CreateCollisionInstancePiece(DWORD dwAttachingModelIndex, const NRaceData::TAttachingData * c_pAttachingData, TCollisionPointInstance * pPointInstance);

		void UpdatePointInstance();
		void UpdatePointInstance(TCollisionPointInstance * pPointInstance);
		bool CheckCollisionDetection(const CDynamicSphereInstanceVector * c_pAttackingSphereVector, D3DXVECTOR3 * pv3Position);

		virtual bool TestCollisionWithDynamicSphere(const CDynamicSphereInstance & dsi);

		void UpdateAdvancingPointInstance();

		BOOL IsClickableDistanceDestInstance(CActorInstance & rkInstDst, float fDistance);

		bool AvoidObject(const CGraphicObjectInstance& c_rkBGObj);
		bool IsBlockObject(const CGraphicObjectInstance& c_rkBGObj);
		void BlockMovement();

	protected:
		BOOL __TestObjectCollision(const CGraphicObjectInstance * c_pObjectInstance);

	public:
		BOOL TestActorCollision(CActorInstance & rVictim );
		BOOL TestPhysicsBlendingCollision(CActorInstance & rVictim);

		BOOL IsInSafeZone(CActorInstance& ptr);

		BOOL AttackingProcess(CActorInstance & rVictim);

		void PreAttack();

		// Battle
		// Input
		void		InputNormalAttackCommand(float fDirRot);	// Process input - Only used by player's character
		bool		InputComboAttackCommand(float fDirRot);	// Process input - Only used by player's character

		// Command
		BOOL		isAttacking();
		BOOL		isNormalAttacking();
		BOOL		isComboAttacking();
		BOOL		IsSplashAttacking();
		BOOL		IsUsingMovingSkill();
		BOOL		IsActEmotion();
		DWORD		GetComboIndex();
		float		GetAttackingElapsedTime();
		void		SetBlendingPosition(const TPixelPosition & c_rPosition, float fBlendingTime = 1.0f);
		void		ResetBlendingPosition();
		void		GetBlendingPosition(TPixelPosition * pPosition);

		BOOL		NormalAttack(float fDirRot, float fBlendTime = 0.1f);
		BOOL		ComboAttack(DWORD wMotionIndex, float fDirRot, float fBlendTime = 0.1f);

		void		Revive();

		BOOL		IsSleep();
		BOOL		IsParalysis();
		BOOL		IsFaint();
		BOOL		IsResistFallen();
		BOOL		IsWaiting();
		BOOL		IsMoving();
		BOOL		IsDead();
		BOOL		IsStun();
		BOOL		IsAttacked();
		BOOL		IsDamage();
		BOOL		IsKnockDown();
		void		SetWalkMode();
		void		SetRunMode();
		void		Stun();
		void		Die();
		void		DieEnd();

		void		SetBattleHitEffect(DWORD dwID);
		void		SetBattleAttachEffect(DWORD dwID);

		MOTION_KEY	GetNormalAttackIndex();

		// Position
		const D3DXVECTOR3&	GetMovementVectorRef();
		const D3DXVECTOR3&	GetPositionVectorRef();
#if defined(ENABLE_WIKI_SYSTEM) || defined(INSIDE_RENDER)
		std::vector<CGrannyLODController*>& GetLODController() { return m_LODControllerVector; }
#endif

		void		SetCurPixelPosition(const TPixelPosition& c_rkPPosCur);
		void		NEW_SetAtkPixelPosition(const TPixelPosition& c_rkPPosAtk);
		void		NEW_SetSrcPixelPosition(const TPixelPosition& c_rkPPosSrc);
		void		NEW_SetDstPixelPosition(const TPixelPosition& c_rkPPosDst);
		void		NEW_SetDstPixelPositionZ(float z);

		const		TPixelPosition& NEW_GetAtkPixelPositionRef();
		const		TPixelPosition& NEW_GetCurPixelPositionRef();
		const		TPixelPosition& NEW_GetSrcPixelPositionRef();
		const		TPixelPosition& NEW_GetDstPixelPositionRef();

		const		TPixelPosition& NEW_GetLastPixelPositionRef();

		void		GetPixelPosition(TPixelPosition * pPixelPosition);
		void		SetPixelPosition(const TPixelPosition& c_rPixelPos);

		// Rotation Command
		void		LookAt(float fDirRot);
		void		LookAt(float fx, float fy);
		void		LookAt(CActorInstance * pInstance);
		void		LookWith(CActorInstance * pInstance);
		void		LookAtFromXY(float x, float y, CActorInstance * pDestInstance);


		void		SetReachScale(float fScale);
		void		SetOwner(DWORD dwOwnerVID);

		float		GetRotation();
		float		GetTargetRotation();

		float		GetAdvancingRotation();

		float		GetRotatingTime();
		void		SetRotation(float fRot);
		void		SetXYRotation(float fRotX, float fRotY);
		void		BlendRotation(float fRot, float fBlendTime);
		void		SetAdvancingRotation(float fRot);
		/////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////
		void		MotionEventProcess();
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		void		MotionEventProcess(DWORD dwcurTime, int iIndex, const CRaceMotionData::TMotionEventData * c_pData, DWORD * dwSkillColor = nullptr);
#else
		void		MotionEventProcess(DWORD dwcurTime, int iIndex, const CRaceMotionData::TMotionEventData * c_pData);
#endif
		void		SoundEventProcess(BOOL bCheckFrequency);
		/////////////////////////////////////////////////////////////////////////////////////

		////
		// Rendering Functions - Temporary Place
		BOOL		IsMovement();

		void		RestoreRenderMode();

		void		BeginDiffuseRender();
		void		EndDiffuseRender();
		void		BeginOpacityRender();
		void		EndOpacityRender();

		void		BeginBlendRender();
		void		EndBlendRender();
		void		SetBlendRenderMode();
		void		SetAlphaValue(float fAlpha);
		float		GetAlphaValue();
		void		BlendAlphaValue(float fDstAlpha, float fDuration);
		void		SetSpecularInfo(BOOL bEnable, int iPart, float fAlpha);
		void		SetSpecularInfoForce(BOOL bEnable, int iPart, float fAlpha);

		void		BeginAddRender();
		void		EndAddRender();
		void		SetAddRenderMode();
		void		SetAddColor(const D3DXCOLOR & c_rColor);

		void		BeginModulateRender();
		void		EndModulateRender();
		void		SetModulateRenderMode();

		void		SetRenderMode(int iRenderMode);

		void		RenderTrace();
		void		RenderCollisionData();
		void		RenderToShadowMap();


	protected:
		void		__AdjustCollisionMovement(const CGraphicObjectInstance * c_pGraphicObjectInstance);

	public:
		void		AdjustDynamicCollisionMovement(const CActorInstance * c_pActorInstance);

		// Weapon Trace
		void		SetWeaponTraceTexture(const char * szTextureName);
		void		UseTextureWeaponTrace();
		void		UseAlphaWeaponTrace();
#if defined(ENABLE_WIKI_SYSTEM) || defined(INSIDE_RENDER)
		void		RenderAllAttachingEffectWiki();
#endif
		// ETC
		void		UpdateAttribute();
		bool		IntersectDefendingSphere();
		float		GetHeight();
		void		ShowAllAttachingEffect();
		void		HideAllAttachingEffect();
        void SetAttachingEffectsFiltered(bool filtered);
		void		ClearAttachingEffect();

#ifdef ENABLE_GRAPHIC_ON_OFF
		void		SetActiveAllAttachingEffect();
		void		SetDeactiveAllAttachingEffect();
#endif

		// Fishing
		bool		CanFishing();
		BOOL		IsFishing();
		void		SetFishingPosition(D3DXVECTOR3 & rv3Position);

#ifdef ENABLE_RENDER_TARGET
		void		RenderAllAttachingEffect();
#endif

	// Flying Methods
		// As a Flying Target
	public:
		virtual D3DXVECTOR3 OnGetFlyTargetPosition();

		void OnShootDamage();

		// As a Shooter
		// NOTE : target and target position are exclusive
	public:
		void ClearFlyTarget();
		bool IsFlyTargetObject();
		void AddFlyTarget(const CFlyTarget & cr_FlyTarget);
		void SetFlyTarget(const CFlyTarget & cr_FlyTarget);
		void LookAtFlyTarget();

		float GetFlyTargetDistance();

		void ClearFlyEventHandler();
		void SetFlyEventHandler(IFlyEventHandler * pHandler);

		bool CanChangeTarget();

	protected:
		IFlyEventHandler * m_pFlyEventHandler;

	public:
		void MountHorse(CActorInstance * pkHorse);
		void HORSE_MotionProcess(BOOL isPC);
		void MotionProcess(BOOL isPC);
		void RotationProcess();
		void PhysicsProcess();
		void ComboProcess();
		void TransformProcess();
		void AccumulationMovement();
		void ShakeProcess();
		void TraceProcess();
		void __MotionEventProcess(BOOL isPC);
		void __AccumulationMovement(float fRot);
		BOOL __SplashAttackProcess(CActorInstance & rVictim);
		BOOL __NormalAttackProcess(CActorInstance & rVictim);
		bool __CanInputNormalAttackCommand();

	private:
		void __Shake(DWORD dwDuration);

	protected:
		CFlyTarget m_kFlyTarget;
		CFlyTarget m_kBackupFlyTarget;
		std::deque<CFlyTarget> m_kQue_kFlyTarget;

	protected:
		bool		__IsInSplashTime();

		void		OnUpdate();
		void		OnRender();

		BOOL		isValidAttacking();

		void		ReservingMotionProcess();
		void		CurrentMotionProcess();
		MOTION_KEY	GetRandomMotionKey(MOTION_KEY dwMotionKey);

		float GetLastMotionTime(float fBlendTime);
		float GetMotionDuration(DWORD dwMotionKey);

		bool InterceptMotion(EMotionPushType iMotionType, WORD wMotion, float fBlendTime = 0.1f, UINT uSkill=0, float fSpeedRatio=1.0f);
		void PushMotion(EMotionPushType iMotionType, DWORD dwMotionKey, float fBlendTime, float fSpeedRatio=1.0f);
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		void ProcessMotionEventEffectToTargetEvent(const CRaceMotionData::TMotionEventData * c_pData, DWORD * dwSkillColor = nullptr);
		void ProcessMotionEventEffectEvent(const CRaceMotionData::TMotionEventData * c_pData, DWORD * dwSkillColor = nullptr);
#else
		void ProcessMotionEventEffectToTargetEvent(const CRaceMotionData::TMotionEventData * c_pData);
		void ProcessMotionEventEffectEvent(const CRaceMotionData::TMotionEventData * c_pData);
#endif
		void ProcessMotionEventSpecialAttacking(int iMotionEventIndex, const CRaceMotionData::TMotionEventData * c_pData);
		void ProcessMotionEventSound(const CRaceMotionData::TMotionEventData * c_pData);
#ifdef ENABLE_SKILL_COLOR_SYSTEM
		void ProcessMotionEventFly(const CRaceMotionData::TMotionEventData * c_pData, DWORD * dwSkillColor = nullptr);
#else
		void ProcessMotionEventFly(const CRaceMotionData::TMotionEventData * c_pData);
#endif
		void ProcessMotionEventWarp(const CRaceMotionData::TMotionEventData * c_pData);

		void AddMovement(float fx, float fy, float fz);

		bool __IsLeftHandWeapon(DWORD type);
		bool __IsRightHandWeapon(DWORD type);
		bool __IsWeaponTrace(DWORD weaponType);

	protected:
		void __InitializeMovement();

	protected:
		void __Initialize();

		void __ClearAttachingEffect();

		float __GetOwnerTime();
		DWORD __GetOwnerVID();
		bool __CanPushDestActor(CActorInstance& rkActorDst);

	protected:
		void __RunNextCombo();
		void __ClearCombo();
		void __OnEndCombo();

		void __ProcessDataAttackSuccess(const NRaceData::TAttackData & c_rAttackData, CActorInstance & rVictim, const D3DXVECTOR3 & c_rv3Position, UINT uiSkill = 0, BOOL isSendPacket = TRUE);
		void __ProcessMotionEventAttackSuccess(DWORD dwMotionKey, BYTE byEventIndex, CActorInstance & rVictim);
		void __ProcessMotionAttackSuccess(DWORD dwMotionKey, CActorInstance & rVictim);

		float __GetInvisibleTimeAdjust(const UINT uiSkill, const NRaceData::TAttackData& c_rAttackData);

		void __HitStone(CActorInstance& rVictim);
		void __HitGood(CActorInstance& rVictim);
		void __HitGreate(CActorInstance& rVictim);

		void __PushDirect(CActorInstance & rVictim);
		void __PushCircle(CActorInstance & rVictim);
		bool __isInvisible();
		void __SetFallingDirection(float fx, float fy);

	protected:
		struct SSetMotionData
		{
			MOTION_KEY	dwMotKey;
			float		fSpeedRatio;
			float		fBlendTime;
			int			iLoopCount;
			UINT		uSkill;

			SSetMotionData()
			{
				iLoopCount=0;
				dwMotKey=0;
				fSpeedRatio=1.0f;
				fBlendTime=0.0f;
				uSkill=0;
			}
		};

	protected:
		float		__GetAttackSpeed();
		DWORD		__SetMotion(const SSetMotionData& c_rkSetMotData, DWORD dwRandMotKey=0);
		void		__ClearMotion();

		bool		__BindMotionData(DWORD dwMotionKey);
		void		__ClearHittedActorInstanceMap();

		UINT		__GetMotionType();

		bool		__IsNeedFlyTargetMotion();
		bool		__HasMotionFlyEvent();
		bool		__IsWaitMotion();
		bool		__IsMoveMotion();
		bool		__IsAttackMotion();
		bool		__IsComboAttackMotion();
		bool		__IsDamageMotion();
		bool		__IsKnockDownMotion();
		bool		__IsDieMotion();
		bool		__IsStandUpMotion();
		bool		__IsMountingHorse();

		bool		__CanAttack();
		bool		__CanNextComboAttack();

		bool		__IsComboAttacking();
		void		__CancelComboAttack();

		WORD		__GetCurrentMotionIndex();
		DWORD		__GetCurrentMotionKey();

		int			__GetLoopCount();
		WORD		__GetCurrentComboType();

		void		__ShowEvent();
		void		__HideEvent();
		BOOL		__IsHiding();
		BOOL		__IsMovingSkill(WORD wSkillNumber);

		float		__GetReachScale();

		void		__CreateAttributeInstance(CAttributeData * pData);

		bool		__IsFlyTargetPC();
		bool		__IsSameFlyTarget(CActorInstance * pInstance);
		D3DXVECTOR3	__GetFlyTargetPosition();

	protected:
		void		__DestroyWeaponTrace();
		void		__ShowWeaponTrace();
		void		__HideWeaponTrace();

	protected:
		// collision data
		void			OnUpdateCollisionData(const CStaticCollisionDataVector * pscdVector);
		void			OnUpdateHeighInstance(CAttributeInstance * pAttributeInstance);
		bool			OnGetObjectHeight(float fX, float fY, float * pfHeight);

	protected:
		/////////////////////////////////////////////////////////////////////////////////////
		// Motion Queueing System
		TMotionDeque					m_MotionDeque;
		SCurrentMotionNode				m_kCurMotNode;
		WORD							m_wcurMotionMode;
		/////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////
		// For Collision Detection
		TCollisionPointInstanceList		m_BodyPointInstanceList;
		TCollisionPointInstanceList		m_DefendingPointInstanceList;
		SSplashArea						m_kSplashArea;
		CAttributeInstance *			m_pAttributeInstance;
		/////////////////////////////////////////////////////////////////////////////////////

		/////////////////////////////////////////////////////////////////////////////////////
		// For Battle System
		std::vector<CWeaponTrace*>	m_WeaponTraceVector;
		CPhysicsObject				m_PhysicsObject;

		DWORD						m_dwcurComboIndex;

		DWORD						m_eActorType;

		DWORD						m_eRace;
		DWORD						m_eShape;
		DWORD						m_eHair;
		BOOL						m_isPreInput;
		BOOL						m_isNextPreInput;
		DWORD						m_dwcurComboBackMotionIndex;

		WORD						m_wcurComboType;

		float						m_fAtkDirRot;

		CRaceData*					m_pkCurRaceData;
		CRaceMotionData*			m_pkCurRaceMotionData;

		// Defender
		float						m_fInvisibleTime;
		BOOL						m_isHiding;

		BOOL						m_isResistFallen;
		BOOL						m_isSleep;
		BOOL						m_isFaint;
		BOOL						m_isParalysis;
		BOOL						m_isStun;
		BOOL						m_isRealDead;
		BOOL						m_isWalking;
		BOOL						m_isMain;

		// Effect
		DWORD						m_dwBattleHitEffectID;
		DWORD						m_dwBattleAttachEffectID;
		/////////////////////////////////////////////////////////////////////////////////////

		// Fishing
		D3DXVECTOR3					m_v3FishingPosition;
		int							m_iFishingEffectID;

		// Position
		float						m_x;
		float						m_y;
		float						m_z;
		D3DXVECTOR3					m_v3Pos;
		D3DXVECTOR3					m_v3Movement;
		BOOL						m_bNeedUpdateCollision;

		DWORD						m_dwShakeTime;

		float						m_fReachScale;
		float						m_fMovSpd;
		float						m_fAtkSpd;

		// Rotation
		float						m_fcurRotation;
		float						m_rotBegin;
		float						m_rotEnd;
		float						m_rotEndTime;
		float						m_rotBeginTime;
		float						m_rotBlendTime;
		float						m_fAdvancingRotation;
		float						m_rotX;
		float						m_rotY;

		float m_fOwnerBaseTime;

		// Rendering
		int							m_iRenderMode;
		D3DXCOLOR					m_AddColor;
		float						m_fAlphaValue;

		// Part
		DWORD						m_adwPartItemID[CRaceData::PART_MAX_NUM];

		// Attached Effect
		std::list<TAttachingEffect> m_AttachingEffectList;
		bool						m_bEffectInitialized;

		// material color
		DWORD						m_dwMtrlColor;
		DWORD						m_dwMtrlAlpha;

		TPixelPosition				m_kPPosCur;
		TPixelPosition				m_kPPosSrc;
		TPixelPosition				m_kPPosDst;
		TPixelPosition				m_kPPosAtk;

		TPixelPosition				m_kPPosLast;

		THitDataMap					m_HitDataMap;

		CActorInstance *			m_pkHorse;
		SpeedTreeWrapperPtr			m_pkTree;


	protected:
		DWORD m_dwSelfVID;
		DWORD m_dwOwnerVID;


	protected:
		void __InitializeStateData();
		void __InitializeMotionData();
		void __InitializeRotationData();
		void __InitializePositionData();

	public:
		IEventHandler* __GetEventHandlerPtr();
		IEventHandler& __GetEventHandlerRef();

		void	__OnSyncing();
		void	__OnWaiting();
		void	__OnMoving();
		void	__OnMove();
		void	__OnStop();
		void	__OnWarp();
		void	__OnClearAffects();
		void	__OnSetAffect(UINT uAffect);
		void	__OnResetAffect(UINT uAffect);
		void	__OnAttack(WORD wMotionIndex);
		void	__OnUseSkill(UINT uMotSkill, UINT uLoopCount, bool isMoving);

	protected:
		void	__OnHit(UINT uSkill, CActorInstance& rkInstVictm, BOOL isSendPacket);

	public:
		void EnableSkipCollision();
		void DisableSkipCollision();
		bool CanSkipCollision();

	protected:
		void __InitializeCollisionData();

		bool m_canSkipCollision;

	protected:
		struct SBlendAlpha
		{
			float m_fBaseTime;
			float m_fBaseAlpha;
			float m_fDuration;
			float m_fDstAlpha;

			DWORD m_iOldRenderMode;
			bool m_isBlending;
		} m_kBlendAlpha;

		void __BlendAlpha_Initialize();
		void __BlendAlpha_Apply(float fDstAlpha, float fDuration);
		void __BlendAlpha_Update();
		void __BlendAlpha_UpdateFadeIn();
		void __BlendAlpha_UpdateFadeOut();
		void __BlendAlpha_UpdateComplete();
		float __BlendAlpha_GetElapsedTime();

		void __Push(int x, int y);

	public:
		void TEMP_Push(int x, int y);
		bool __IsSyncing();

		void __CreateTree(const char * c_szFileName);
		void __DestroyTree();
		void __SetTreePosition(float fx, float fy, float fz);

	protected:
		IEventHandler* m_pkEventHandler;

	protected:
		static bool ms_isDirLine;
};
