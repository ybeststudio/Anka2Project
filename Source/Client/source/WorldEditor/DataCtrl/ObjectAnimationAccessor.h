#pragma once
#include "../../GameLib/RaceMotionData.h"

class CRaceMotionDataAccessor : public CRaceMotionData
{
	public:
		CRaceMotionDataAccessor();
		~CRaceMotionDataAccessor();

		void ClearAccessor();

		void SetMotionFileName (const char* c_szFileName);

		void SetLoopCount (int iLoopCount);

		// Duration
		float GetMotionDuration();
		void SetMotionDuration (float fDuration);

		// Motion Event
		DWORD MakeEvent (int iEventType);
		DWORD MakeEventScreenWaving();
		DWORD MakeEventScreenFlashing();
		DWORD MakeEventFly();
		DWORD MakeEventEffect();
		DWORD MakeEventAttacking();
		DWORD MakeEventSound();
		DWORD MakeEventCharacterShow();
		DWORD MakeEventCharacterHide();
		DWORD MakeEventWarp();
		DWORD MakeEventEffectToTarget();
		BOOL GetMotionEventDataPointer (DWORD dwIndex, TMotionEventData** ppData);

		void MovePosition (DWORD dwIndex, float fTime);
		void DeleteEvent (DWORD dwIndex);

		// Combo Data
		void SetComboInputStartTime (float fTime);
		void SetNextComboTime (float fTime);
		void SetComboInputEndTime (float fTime);

		// Attacking Data
		void SetComboMotionEnable (BOOL bFlag);
		void SetAttackingMotionEnable (BOOL bFlag);

		void SetMotionType (int iType);
		void SetAttackType (int iType);
		void SetHittingType (int iType);
		void SetStiffenTime (float fTime);
		void SetInvisibleTime (float fTime);
		void SetExternalForceTime (float fForce);

		// Hit Data
		void SetAttackStartTime (DWORD dwIndex, float fTime);
		void SetAttackEndTime (DWORD dwIndex, float fTime);

		void SetAttackingWeaponLength (DWORD dwIndex, float fWeaponLength);
		void SetAttackingBoneName (DWORD dwIndex, const std::string & strBoneName);

		void RefreshTimeHitPosition();

		DWORD GetHitDataCount() const;
		const NRaceData::THitData* GetHitDataPtr (DWORD dwIndex) const;

		void InsertHitData();
		void DeleteHitData (DWORD dwIndex);

		void SetHitLimitCount (int iCount);

		// Movement
		void SetMovementEnable (BOOL bFlag);
		void SetMovementDistance (const D3DXVECTOR3 & c_rv3MovementDistance);

		// Loop
		void SetLoopMotionEnable (BOOL bFlag);
		void SetLoopStartTime (float fTime);
		void SetLoopEndTime (float fTime);

		// Cancel
		void SetCancelEnable (BOOL bEnable);

	protected:
		void UpdateActorInstanceAccessorMotion();
};