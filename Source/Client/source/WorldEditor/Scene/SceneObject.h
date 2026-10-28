#pragma once

class ISceneObserver
{
	public:
		ISceneObserver() {}
		virtual ~ISceneObserver() {}

		virtual void SetLocalTime (float fDuration, float fLocalTime) = 0;
		virtual void Play (bool isLoop) = 0;
		virtual void Stop() = 0;
};

class CSceneObject : public CSceneBase
{
		friend class CSceneEffect;
	public:
		CSceneObject();
		~CSceneObject();

		void Initialize();
		void SetObjectData (CObjectData * pObjectData);

		float GetLocalTime();
		void SetLocalTime (float fTime);
		void SetLocalPercentTime (float fPercentage);
		void Refresh();
		void BuildAttachingData();
		void RefreshAttachingData();
		void FitCamera();
		void SetMotionSpeed (float fSpeed);

		bool isPlay();
		void Play (bool isLoop = false);
		void Stop();
		void RegisterObserver (ISceneObserver * pObserver);

		int GetLoopCount();
		void SetLoopCount (int iLoopCount);

		BOOL isModelData();
		BOOL isMotionData();
		float GetMotionDuration();

		DWORD GetBoneCount();
		BOOL GetBoneName (DWORD dwIndex, std::string * pstrBoneName);
		BOOL GetBoneIndex (const char* c_szBoneName, DWORD * pdwIndex);
		void SetCurrentHitDataIndex (DWORD dwIndex);

		void ShowMainCharacter (BOOL bFlag);
		void ShowCollisionData (BOOL bFlag);

		void SelectData (NRaceData::TAttachingData * pAttachingData);

		void BuildTimeHitPosition (NRaceData::TMotionAttackData * pMotionAttackData);

		void UpdateActorInstanceMotion();

		BOOL SaveAttrFile (const char* c_szFileName);

	protected:
		void ClearModelThing();
		void ClearMotionThing();
		BOOL SetModelThing (CGraphicThing * pThing);
		BOOL SetMotionThing (CGraphicThing * pThing);

		void RenderCollisionData();
		void RenderCollisionData (CActorInstanceAccessor * pInstance, const NRaceData::TAttachingData * c_pAttachingData);
		void __RenderHitData();
		void __RenderSplash();

	protected:
		void OnUpdate();
		void OnRender (BOOL bClear);
		void OnRenderUI (float fx, float fy);

		void OnKeyDown (int iChar);
		void OnKeyUp (int iChar);
		void OnMouseMove (long x, long y);
		void OnLButtonDown (UINT nFlags, CPoint point);
		void OnLButtonUp();
		void OnRButtonDown();
		void OnRButtonUp();
		BOOL OnMouseWheel (short zDelat);
		void OnMovePosition (float fx, float fy);

		//void RenderPivots();
		//void ProcessMotionEvent(float fElapsedTime);
		//void CreateEffectInstance(DWORD dwID, const D3DXVECTOR3 & c_rv3Position, const char * c_szBoneName);

		void AddAttachingEffect (const NRaceData::TAttachingData * c_pAttachingData);
		void AddAttachingObject (const NRaceData::TAttachingData * c_pAttachingData);

		void SendLocalTimeToObserver (float fDuration, float fLocalTime);
		void SendPlayToObserver();
		void SendStopToObserver();

	protected:
		typedef std::list<ISceneObserver*> TSceneObserverList;
		typedef std::list<ISceneObserver*>::iterator TSceneObserverIterator;

		TSceneObserverList m_SceneObserverList;

	protected:
		typedef struct SEffectManager
		{
			DWORD dwIndex;
			DWORD dwBoneIndex;
		} TEffectManager;

		typedef struct SAttachingEffect
		{
			DWORD dwIndex;
			DWORD dwBoneIndex;

			D3DXMATRIX mat;

			const NRaceData::TAttachingData* c_pAttachingData;
		} TAttachingEffect;

		typedef struct SAttachingObject
		{
			CModelInstanceAccessor* pModelInstanceAccessor;
			CItemData* pItemData;

			const NRaceData::TAttachingData* c_pAttachingData;
		} TAttachingObject;

	protected:
		void RenderEffectPosition();
		float m_fLocalTime;
		float m_fDuration;
		float m_fMotionStartTime;
		float m_fMotionSpeed;
		bool m_isPlay;
		bool m_isLoop;
		int m_icurLoopCount;

		D3DXCOLOR m_ClearColor;

		NRaceData::TAttachingData* m_pSelectedAttachingData;
		bool m_bMoveSelectedData;
		POINT m_poMoveSelectedData;
		bool m_bMoveZSelectedData;
		POINT m_poMoveZSelectedData;

		// Hit Data
		DWORD m_dwcurHitDataIndex;

		// Data
		CObjectData* m_pObjectData;

		// Instance
		CActorInstanceAccessor m_ActorInstanceAccessor;

		BOOL m_isShowingMainCharacter;
		BOOL m_isShowingCollisionData;

		// flying
		D3DXVECTOR3 m_v3Target;

		int m_iGrippedDirection;

		D3DXVECTOR3 m_vecGrippedPosition;
		D3DXVECTOR3 m_vecGrippedValue;
};