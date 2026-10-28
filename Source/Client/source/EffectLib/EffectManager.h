#pragma once

#include "StdAfx.h"
#include "EffectInstance.h"

#include "../UserInterface/Locale_inc.h"

class CEffectManager : public CScreen, public CSingleton<CEffectManager>
{
	public:
		enum EEffectType
		{
			EFFECT_TYPE_NONE				= 0,
			EFFECT_TYPE_PARTICLE			= 1,
			EFFECT_TYPE_ANIMATION_TEXTURE	= 2,
			EFFECT_TYPE_MESH				= 3,
			EFFECT_TYPE_SIMPLE_LIGHT		= 4,

			EFFECT_TYPE_MAX_NUM				= 4,
		};

		typedef std::map<DWORD, CEffectData*> TEffectDataMap;
		typedef std::map<DWORD, CEffectInstance*> TEffectInstanceMap;

	public:
		CEffectManager();
		virtual ~CEffectManager();

		void Destroy();

		void UpdateSound();
		void Update();
		void Render();
        void SetOptionColorTint(DWORD index, const D3DXCOLOR& color);
        void SetOptionFiltered(DWORD id, bool filtered) { if (filtered) m_optionFiltered.insert(id); else m_optionFiltered.erase(id); }
        std::set<DWORD> m_optionFiltered;
#ifdef ENABLE_RENDER_TARGET
		void RenderOne(DWORD id);
#endif
		void GetInfo(std::string* pstInfo);
#ifdef ENABLE_WIKI_SYSTEM
		void RenderOneWiki(DWORD id);
#endif
		bool IsAliveEffect(DWORD dwInstanceIndex);
#ifdef __ENABLE_STEALTH_FIX__
		void ApplyAlwaysHidden();
		void ReleaseAlwaysHidden();
#endif
		// Register
		BOOL RegisterEffect(const char *c_szFileName, bool isExistDelete = false, bool isNeedCache = false
#ifdef ENABLE_SKILL_COLOR_SYSTEM
			, const char *name = NULL
#endif
		);
		BOOL RegisterEffect2(const char *c_szFileName, DWORD* pdwRetCRC, bool isNeedCache = false
#ifdef ENABLE_SKILL_COLOR_SYSTEM
			, const char *name = NULL
#endif
		);

		void DeleteAllInstances();

		// Usage
		int CreateEffect(DWORD dwID, const D3DXVECTOR3 & c_rv3Position, const D3DXVECTOR3 & c_rv3Rotation
#ifdef ENABLE_SKILL_COLOR_SYSTEM
			, DWORD *dwSkillColor = NULL
#endif
		);

		int CreateEffect(const char *c_szFileName, const D3DXVECTOR3 & c_rv3Position, const D3DXVECTOR3 & c_rv3Rotation);

		void CreateEffectInstance(DWORD dwInstanceIndex, DWORD dwID
#ifdef ENABLE_SKILL_COLOR_SYSTEM
			, DWORD *dwSkillColor = NULL
#endif
		);

#ifdef ENABLE_AURA_COSTUME_SYSTEM
		int CreateEffectWithScale(DWORD dwID, const D3DXVECTOR3& c_rv3Position, const D3DXVECTOR3& c_rv3Rotation, float fParticleScale = 1.0f
#ifdef ENABLE_SKILL_COLOR_SYSTEM
			, DWORD * dwSkillColor = NULL
#endif
		);
		void CreateEffectInstanceWithScale(DWORD dwInstanceIndex, DWORD dwID, float fParticleScale = 1.0f, const D3DXVECTOR3* c_pv3MeshScale = NULL
#ifdef ENABLE_SKILL_COLOR_SYSTEM
			, DWORD * dwSkillColor = NULL
#endif
		);
#endif

		BOOL SelectEffectInstance(DWORD dwInstanceIndex);
		bool DestroyEffectInstance(DWORD dwInstanceIndex);
		void DeactiveEffectInstance(DWORD dwInstanceIndex);

#ifdef ENABLE_GRAPHIC_ON_OFF
		void ActiveEffectInstance(DWORD dwInstanceIndex);
#endif

		void SetEffectTextures(DWORD dwID, std::vector<std::string> textures);
		void SetEffectInstancePosition(const D3DXVECTOR3 & c_rv3Position);
		void SetEffectInstanceRotation(const D3DXVECTOR3 & c_rv3Rotation);
		void SetEffectInstanceGlobalMatrix(const D3DXMATRIX & c_rmatGlobal);

		void ShowEffect();
		void HideEffect();

		void RenderEffect();

		// Temporary function
		DWORD GetRandomEffect();
		int GetEmptyIndex();
		bool GetEffectData(DWORD dwID, CEffectData ** ppEffect);
		bool GetEffectData(DWORD dwID, const CEffectData ** c_ppEffect);

		void CreateUnsafeEffectInstance(DWORD dwEffectDataID, CEffectInstance ** ppEffectInstance);
		bool DestroyUnsafeEffectInstance(CEffectInstance * pEffectInstance);

		int GetRenderingEffectCount();

	protected:
		void __Initialize();

		void __DestroyEffectInstanceMap();
		void __DestroyEffectCacheMap();
		void __DestroyEffectDataMap();

	protected:
		bool m_isDisableSortRendering;
		TEffectDataMap					m_kEftDataMap;
		TEffectInstanceMap				m_kEftInstMap;
		TEffectInstanceMap				m_kEftCacheMap;

		CEffectInstance *				m_pSelectedEffectInstance;
};
