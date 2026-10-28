#pragma once

#include "../milesLib/Type.h"
#include "RaceMotionDataEvent.h"

class CRaceMotionData
{
	public:
		enum EType
		{
			TYPE_NONE,
			TYPE_WAIT,
			TYPE_MOVE,
			TYPE_ATTACK,
			TYPE_COMBO,
			TYPE_DAMAGE,
			TYPE_KNOCKDOWN,
			TYPE_DIE,
			TYPE_SKILL,
			TYPE_STANDUP,
			TYPE_EVENT,
			TYPE_FISHING,
			TYPE_NUM,
		};

		enum
		{
			SKILL_NUM = 255,
		};

		enum EMode
		{
			MODE_RESERVED,
			MODE_GENERAL,

			MODE_ONEHAND_SWORD,
			MODE_TWOHAND_SWORD,
			MODE_DUALHAND_SWORD,
			MODE_BOW,
			MODE_BOW_SPECIAL,
			MODE_FAN,
			MODE_BELL,
			MODE_FISHING,

			MODE_HORSE,
			MODE_HORSE_ONEHAND_SWORD,
			MODE_HORSE_TWOHAND_SWORD,
			MODE_HORSE_DUALHAND_SWORD,
			MODE_HORSE_BOW,
			MODE_HORSE_FAN,
			MODE_HORSE_BELL,

#ifdef ENABLE_STANDING_MOUNT
			MODE_HORSE_STAND,
			MODE_HORSE_STAND_SWORD,
			MODE_HORSE_STAND_TWOHAND_SWORD,
			MODE_HORSE_STAND_DUALHAND_SWORD,
			MODE_HORSE_STAND_BOW,
			MODE_HORSE_STAND_BOW_SPECIAL,
			MODE_HORSE_STAND_FAN,
			MODE_HORSE_STAND_BELL,
#endif

			MODE_WEDDING_DRESS,

			MODE_MAX_NUM,
		};

		enum EName
		{
			NAME_NONE,
			NAME_WAIT,
			NAME_WALK,
			NAME_RUN,
			NAME_CHANGE_WEAPON,
			NAME_DAMAGE,
			NAME_DAMAGE_FLYING,
			NAME_STAND_UP,
			NAME_DAMAGE_BACK,
			NAME_DAMAGE_FLYING_BACK,
			NAME_STAND_UP_BACK,
			NAME_DEAD,
			NAME_DEAD_BACK,
			NAME_NORMAL_ATTACK,
			NAME_COMBO_ATTACK_1,
			NAME_COMBO_ATTACK_2,
			NAME_COMBO_ATTACK_3,
			NAME_COMBO_ATTACK_4,
			NAME_COMBO_ATTACK_5,
			NAME_COMBO_ATTACK_6,
			NAME_COMBO_ATTACK_7,
			NAME_COMBO_ATTACK_8,
			NAME_INTRO_WAIT,
			NAME_INTRO_SELECTED,
			NAME_INTRO_NOT_SELECTED,
			NAME_SPAWN,
			NAME_FISHING_THROW,
			NAME_FISHING_WAIT,
			NAME_FISHING_STOP,
			NAME_FISHING_REACT,
			NAME_FISHING_CATCH,
			NAME_FISHING_FAIL,
			NAME_STOP,
			NAME_SPECIAL_1,
			NAME_SPECIAL_2,
			NAME_SPECIAL_3,
			NAME_SPECIAL_4,
			NAME_SPECIAL_5,
			NAME_SPECIAL_6,
			NAME_SKILL = 50,
			NAME_SKILL_END = NAME_SKILL+SKILL_NUM,

			// CLAP
			NAME_CLAP,

			// CHEERS
			NAME_CHEERS_1,
			NAME_CHEERS_2,

			// KISS
			NAME_KISS_START,
			NAME_KISS_WITH_WARRIOR = NAME_KISS_START + 0,
			NAME_KISS_WITH_ASSASSIN = NAME_KISS_START + 1,
			NAME_KISS_WITH_SURA = NAME_KISS_START + 2,
			NAME_KISS_WITH_SHAMAN = NAME_KISS_START + 3,

			// FRENCH_KISS
			NAME_FRENCH_KISS_START,
			NAME_FRENCH_KISS_WITH_WARRIOR = NAME_FRENCH_KISS_START + 0,
			NAME_FRENCH_KISS_WITH_ASSASSIN = NAME_FRENCH_KISS_START + 1,
			NAME_FRENCH_KISS_WITH_SURA = NAME_FRENCH_KISS_START + 2,
			NAME_FRENCH_KISS_WITH_SHAMAN = NAME_FRENCH_KISS_START + 3,

			// SLAP
			NAME_SLAP_HIT_START,
			NAME_SLAP_HIT_WITH_WARRIOR = NAME_SLAP_HIT_START + 0,
			NAME_SLAP_HIT_WITH_ASSASSIN = NAME_SLAP_HIT_START + 1,
			NAME_SLAP_HIT_WITH_SURA = NAME_SLAP_HIT_START + 2,
			NAME_SLAP_HIT_WITH_SHAMAN = NAME_SLAP_HIT_START + 3,

			NAME_SLAP_HURT_START,
			NAME_SLAP_HURT_WITH_WARRIOR = NAME_SLAP_HURT_START + 0,
			NAME_SLAP_HURT_WITH_ASSASSIN = NAME_SLAP_HURT_START + 1,
			NAME_SLAP_HURT_WITH_SURA = NAME_SLAP_HURT_START + 2,
			NAME_SLAP_HURT_WITH_SHAMAN = NAME_SLAP_HURT_START + 3,

			NAME_DIG,

			NAME_DANCE_1,
			NAME_DANCE_2,
			NAME_DANCE_3,
			NAME_DANCE_4,
			NAME_DANCE_5,
			NAME_DANCE_6,

			NAME_DANCE_END = NAME_DANCE_1 + 16,

			NAME_CONGRATULATION,
			NAME_DANCE_7,
			NAME_SELFIE,
			NAME_DOZE,
			NAME_EXERCISE,
			NAME_PUSHUP,
			NAME_EMOTION_1,
			NAME_EMOTION_2,
			NAME_EMOTION_3,
			NAME_EMOTION_4,
			NAME_EMOTION_5,
			NAME_EMOTION_6,
			NAME_EMOTION_7,
			NAME_EMOTION_8,
			NAME_EMOTION_9,
			NAME_EMOTION_10,
			NAME_EMOTION_11,
			NAME_EMOTION_12,

			NAME_EMOTION_13,
			NAME_EMOTION_14,
			NAME_EMOTION_15,
			NAME_EMOTION_16,
			NAME_EMOTION_17,
			NAME_EMOTION_18,
			NAME_EMOTION_19,
			NAME_EMOTION_20,
			NAME_EMOTION_21,
			NAME_FORGIVE,
			NAME_ANGRY,
			NAME_ATTRACTIVE,
			NAME_SAD,
			NAME_SHY,
			NAME_CHEERUP,
			NAME_BANTER,
			NAME_JOY,

			NAME_MAX_NUM,
		};

		enum EMotionEventType
		{
			MOTION_EVENT_TYPE_NONE,

			MOTION_EVENT_TYPE_EFFECT,
			MOTION_EVENT_TYPE_SCREEN_WAVING,
			MOTION_EVENT_TYPE_SCREEN_FLASHING,
			MOTION_EVENT_TYPE_SPECIAL_ATTACKING,
			MOTION_EVENT_TYPE_SOUND,
			MOTION_EVENT_TYPE_FLY,
			MOTION_EVENT_TYPE_CHARACTER_SHOW,
			MOTION_EVENT_TYPE_CHARACTER_HIDE,
			MOTION_EVENT_TYPE_WARP,
			MOTION_EVENT_TYPE_EFFECT_TO_TARGET,

			MOTION_EVENT_TYPE_MAX_NUM,
		};

		typedef struct SComboInputData
		{
			float fInputStartTime;
			float fNextComboTime;
			float fInputEndTime;
		} TComboInputData;

		typedef struct NMotionEvent::SMotionEventData				TMotionEventData;
		typedef struct NMotionEvent::SMotionEventDataScreenWaving	TScreenWavingEventData;
		typedef struct NMotionEvent::SMotionEventDataScreenFlashing	TScreenFlashingEventData;
		typedef struct NMotionEvent::SMotionEventDataEffect			TMotionEffectEventData;
		typedef struct NMotionEvent::SMotionEventDataFly			TMotionFlyEventData;
		typedef struct NMotionEvent::SMotionEventDataAttack			TMotionAttackingEventData;
		typedef struct NMotionEvent::SMotionEventDataSound			TMotionSoundEventData;
		typedef struct NMotionEvent::SMotionEventDataCharacterShow	TMotionCharacterShowEventData;
		typedef struct NMotionEvent::SMotionEventDataCharacterHide	TMotionCharacterHideEventData;
		typedef struct NMotionEvent::SMotionEventDataWarp			TMotionWarpEventData;
		typedef struct NMotionEvent::SMotionEventDataEffectToTarget	TMotionEffectToTargetEventData;
		typedef std::vector<TMotionEventData*> TMotionEventDataVector;

	public:
		static CRaceMotionData* New();
		static void Delete(CRaceMotionData* pkData);

		static void CreateSystem(UINT uCapacity);
		static void DestroySystem();

	public:
		CRaceMotionData();
		virtual ~CRaceMotionData();

		void			Initialize();
		void			Destroy();

		void			SetName(UINT eName);

		UINT			GetType() const;
		bool			IsLock() const;

		int				GetLoopCount() const;

		const char *	GetMotionFileName() const;
		const char *	GetSoundScriptFileName() const;

		void			SetMotionDuration(float fDur);
		float			GetMotionDuration();

		bool			IsAccumulationMotion();
		void			SetAccumulationPosition(const TPixelPosition & c_rPos);
		const			TPixelPosition & GetAccumulationPosition() { return m_accumulationPosition; }

		BOOL			IsComboInputTimeData() const;

		float			GetComboInputStartTime() const;
		float			GetNextComboTime() const;
		float			GetComboInputEndTime() const;

		// Attacking
		BOOL			isAttackingMotion() const;
		const NRaceData::TMotionAttackData * GetMotionAttackDataPointer() const;
		const NRaceData::TMotionAttackData & GetMotionAttackDataReference() const;
		BOOL			HasSplashMotionEvent() const;

		// Skill
		BOOL			IsCancelEnableSkill() const;

		// Loop
		BOOL			IsLoopMotion() const;
		float			GetLoopStartTime() const;
		float			GetLoopEndTime() const;

		// Motion Event Data
		DWORD			GetMotionEventDataCount() const;
		BOOL			GetMotionEventDataPointer(BYTE byIndex, const CRaceMotionData::TMotionEventData ** c_ppData) const;
		BOOL			GetMotionAttackingEventDataPointer(BYTE byIndex, const CRaceMotionData::TMotionAttackingEventData ** c_ppMotionEventData) const;
		int				GetEventType(DWORD dwIndex) const;
		float			GetEventStartTime(DWORD dwIndex) const;

		// Sound Data
		const NSound::TSoundInstanceVector * GetSoundInstanceVectorPointer() const;

		// File
		bool			SaveMotionData(const char * c_szFileName);
		bool			LoadMotionData(const char * c_szFileName);
		bool			LoadSoundScriptData(const char * c_szFileName);

	protected:
		void			SetType(UINT eType);

	protected:
		UINT							m_eType;
		UINT							m_eName;
		BOOL							m_isLock;
		int								m_iLoopCount;

		std::string						m_strMotionFileName;
		std::string						m_strSoundScriptDataFileName;
		float							m_fMotionDuration;

		BOOL							m_isAccumulationMotion;
		TPixelPosition					m_accumulationPosition;

		BOOL							m_isComboMotion;
		TComboInputData					m_ComboInputData;

		BOOL							m_isLoopMotion;
		float							m_fLoopStartTime;
		float							m_fLoopEndTime;

		BOOL							m_isAttackingMotion;
		NRaceData::TMotionAttackData	m_MotionAttackData;

		BOOL							m_bCancelEnableSkill;

		TMotionEventDataVector			m_MotionEventDataVector;
		NSound::TSoundInstanceVector	m_SoundInstanceVector;

	private:
		BOOL							m_hasSplashEvent;

	protected:
		static CDynamicPool<CRaceMotionData> ms_kPool;
};