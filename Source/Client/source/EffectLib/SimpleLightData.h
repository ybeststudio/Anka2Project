#pragma once

#ifdef ENABLE_DIRECTX9_UPDATE
#include <d3dx9.h>
#else
#include <d3dx8.h>
#endif

#include "../eterLib/TextFileLoader.h"

#include "Type.h"
#include "EffectElementBase.h"

class CLightData : public CEffectElementBase
{
	friend class CLightInstance;
	public:
		CLightData();
		virtual ~CLightData();

		void GetRange(float fTime, float& rRange);
		float GetDuration();
		BOOL isLoop()
		{
			return m_bLoopFlag;
		}
		int GetLoopCount()
		{
			return m_iLoopCount;
		}

#ifdef ENABLE_DIRECTX9_UPDATE
		void InitializeLight(D3DLIGHT9& light);
#else
		void InitializeLight(D3DLIGHT8& light);
#endif

	protected:
		void OnClear();
		bool OnIsData();

		BOOL OnLoadScript(CTextFileLoader & rTextFileLoader);

	protected:
		float m_fMaxRange;
		float m_fDuration;
		TTimeEventTableFloat m_TimeEventTableRange;

		D3DXCOLOR m_cAmbient;
		D3DXCOLOR m_cDiffuse;

		BOOL m_bLoopFlag;
		int m_iLoopCount;

		float m_fAttenuation0;
		float m_fAttenuation1;
		float m_fAttenuation2;

	public:
		static void DestroySystem();

		static CLightData* New();
		static void Delete(CLightData* pkData);

		static CDynamicPool<CLightData>		ms_kPool;
};
