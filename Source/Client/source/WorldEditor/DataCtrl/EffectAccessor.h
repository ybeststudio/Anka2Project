#pragma once

#include "../../EffectLib/EffectElementBase.h"
#include "../../EffectLib/ParticleSystemData.h"
#include "../../EffectLib/EffectMesh.h"
#include "../../EffectLib/SimpleLightData.h"
#include "../../EffectLib/EffectData.h"
#include "../../EffectLib/EffectInstance.h"

// Data
class CEffectElementBaseAccessor : public CEffectElementBase
{
	public:
		CEffectElementBaseAccessor() {}
		virtual ~CEffectElementBaseAccessor() {}

		void SaveScript (int iBaseTab, FILE * File);

		void SetStartTime (float fTime);

		DWORD GetPositionCount();
		void DeletePosition (DWORD dwIndex);
		void InsertPosition (float fTime);
		bool GetTimePosition (DWORD dwIndex, float* pTime);
		void SetTimePosition (DWORD dwIndex, float fTime);
		bool GetValuePosition (DWORD dwIndex, TEffectPosition** ppEffectPosition);
		void SetValuePosition (DWORD dwIndex, const D3DXVECTOR3 & c_rVector);
		void SetValueControlPoint (DWORD dwIndex, const D3DXVECTOR3 & c_rVector);
};
class CParticleAccessor : public CParticleSystemData
{
	public:
		CParticleAccessor();
		virtual ~CParticleAccessor();

		void ClearAccessor();
		void SetDefaultData();
		void SaveScript (int iBaseTab, FILE * File, const char* c_szGlobalPath);

		void InsertTexture (const char* c_szFileName);
		void ClearOneTexture (DWORD dwIndex);
		void ClearAllTexture();

		// Emitter Property
		DWORD& GetMaxEmissionCountReference();

		float& GetCycleLengthReference();
		BOOL& GetCycleLoopFlagReference();
		int& GetLoopCountReference();

		BYTE& GetEmitterShapeReference();
		BYTE& GetEmitterAdvancedTypeReference();
		BOOL& GetEmitFromEdgeFlagReference();
		D3DXVECTOR3& GetEmittingSizeReference();
		float& GetEmittingRadiusReference();

		D3DXVECTOR3& GetEmittingDirectionReference();

		TTimeEventTableFloat* GetEmitterTimeEventTableEmittingSize();
		TTimeEventTableFloat* GetEmitterTimeEventTableEmittingAngularVelocity();
		TTimeEventTableFloat* GetEmitterTimeEventTableEmittingDirectionX();
		TTimeEventTableFloat* GetEmitterTimeEventTableEmittingDirectionY();
		TTimeEventTableFloat* GetEmitterTimeEventTableEmittingDirectionZ();
		TTimeEventTableFloat* GetEmitterTimeEventTableEmittingVelocity();
		TTimeEventTableFloat* GetEmitterTimeEventTableEmissionCount();
		TTimeEventTableFloat* GetEmitterTimeEventTableLifeTime();
		TTimeEventTableFloat* GetEmitterTimeEventTableSizeX();
		TTimeEventTableFloat* GetEmitterTimeEventTableSizeY();

		// Particle Property
		BYTE& GetSrcBlendTypeReference();
		BYTE& GetDestBlendTypeReference();
		BYTE& GetColorOperationTypeReference();
		BYTE& GetBillboardTypeReference();

		BYTE& GetTexAniTypeReference();
		BOOL& GetTexAniRandomStartFrameFlagReference();
		float& GetTexAniDelayReference();

		BOOL& GetStretchFlagReference();
		BOOL& GetAttachFlagReference();

		BYTE& GetRotationTypeReference();
		float& GetRotationSpeedReference();
		WORD& GetRotationRandomStartBeginReference();
		WORD& GetRotationRandomStartEndReference();

		TTimeEventTableFloat* GetParticleTimeEventGravity();
		TTimeEventTableFloat* GetParticleTimeEventAirResistance();

		TTimeEventTableFloat* GetParticleTimeEventScaleX();
		TTimeEventTableFloat* GetParticleTimeEventScaleY();
		TTimeEventTableFloat* GetParticleTimeEventColorRed();
		TTimeEventTableFloat* GetParticleTimeEventColorGreen();
		TTimeEventTableFloat* GetParticleTimeEventColorBlue();
		TTimeEventTableFloat* GetParticleTimeEventAlpha();
		TTimeEventTableFloat* GetParticleTimeEventRotation();
		TTimeEventTableFloat* GetParticleTimeEventStretch();

		DWORD GetTextureCount();
		BOOL GetImagePointer (DWORD dwIndex, CGraphicImage** ppImage);
		BOOL GetTextureName (DWORD dwIndex, const char** pszTextureName);
};
class CMeshAccessor : public CEffectMeshScript
{
	public:
		CMeshAccessor();
		virtual ~CMeshAccessor();

		void ClearAccessor();
		void SaveScript (int iBaseTab, FILE * File, const char* c_szGlobalPath);

		void SetMeshAnimationFlag (bool bFlag);
		void SetMeshAnimationLoopCount (int iNewCount);
		void SetMeshAnimationFrameDelay (float fDelay);
		void SetMeshFileName (const char* c_szFileName);

		void LoadMeshInstance();
		DWORD GetMeshElementCount();
		BOOL GetMeshElementDataPointer (DWORD dwIndex, CEffectMesh::TEffectMeshData** ppMeshElementData);

	protected:
		CEffectMesh* m_pEffectMesh;
};

class CLightAccessor : public CLightData
{
	public:
		CLightAccessor();
		virtual ~CLightAccessor();

		void ClearAccessor();
		void SaveScript (int iBaseTab, FILE * File);

		BOOL& GetLoopFlagReference();
		int& GetLoopCountReference()
		{
			return m_iLoopCount;
		}

		float& GetMaxRangeReference();
		float& GetDurationReference();

		float& GetAttenuation0Reference();
		float& GetAttenuation1Reference();
		float& GetAttenuation2Reference();

		TTimeEventTableFloat* GetEmitterTimeEventTableRange();

		D3DXCOLOR& GetAmbientColorReference();
		D3DXCOLOR& GetDiffuseColorReference();
		void SetAmbientColor (float fr, float fg, float fb);
		void SetDiffuseColor (float fr, float fg, float fb);
};

class CEffectAccessor : public CEffectData
{
	public:
		enum
		{
			EFFECT_ELEMENT_TYPE_PARTICLE,
			EFFECT_ELEMENT_TYPE_MESH,
			EFFECT_ELEMENT_TYPE_LIGHT,
		};

		typedef struct SEffectElement
		{
			BOOL bVisible;

			int iType;
			std::string strName;

			CEffectElementBaseAccessor* pBase;

			CParticleAccessor* pParticle;
			CMeshAccessor* pMesh;
			CLightAccessor* pLight;
			NSound::TSoundData* pSound;
		} TEffectElement;

		typedef std::vector<TEffectElement> TEffectElementVector;

	public:
		CEffectAccessor() : m_fLifeTime (5.0f) {}
		virtual ~CEffectAccessor();

		void					Clear();

		CParticleSystemData* 	AllocParticle();
		CEffectMeshScript* 		AllocMesh();
		CLightData* 			AllocLight();

		void					SetVisible (DWORD dwIndex, BOOL bVisible);
		BOOL					GetVisible (DWORD dwIndex);

		float					GetLifeTime();
		void					SetLifeTime (float fLifeTime);
		void					SetBoundingSphereRadius (float fRadius);
		void					SetBoundingSpherePosition (const D3DXVECTOR3 & c_rv3Pos);
		DWORD					GetElementCount();
		BOOL					GetElement (DWORD dwIndex, TEffectElement** ppElement);
		BOOL					GetElementParticle (DWORD dwIndex, CParticleAccessor** ppParticleAccessor);
		void					DeleteElement (DWORD dwIndex);
		void					SwapElement (DWORD dwIndex1, DWORD dwIndex2);

	protected:
		float					m_fLifeTime;
		TEffectElementVector	m_ElementVector;
};

class CEffectInstanceAccessor : public CEffectInstance
{
	public:
		CEffectInstanceAccessor() {}
		virtual ~CEffectInstanceAccessor() {}

		void SetEffectDataAccessorPointer (CEffectAccessor * pAccessor);
};