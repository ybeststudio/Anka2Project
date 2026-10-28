#pragma once

#include "StdAfx.h"
#include "GrpImage.h"
#include "GrpIndexBuffer.h"
#include "GrpVertexBufferDynamic.h"
#include "Pool.h"

class CGraphicImageInstance
{
	public:
#ifdef ENABLE_OFFICAL_FEATURES
		void LeftRightReverse();
#endif
	protected:
#ifdef ENABLE_OFFICAL_FEATURES
		bool m_bLeftRightReverse;
#endif

	public:
		static DWORD Type();
		BOOL IsType(DWORD dwType);

	public:
		CGraphicImageInstance();
		virtual ~CGraphicImageInstance();

		void Destroy();

#ifdef ENABLE_CLIP_MASKING
		void Render(RECT* pClipRect = NULL);
#else
		void Render();
#endif

		void SetDiffuseColor(float fr, float fg, float fb, float fa);
#if defined(ENABLE_RENDER_TARGET) || defined(ENABLE_EMOTICONS_SYSTEM)
		void SetScale(float fx, float fy);
#endif
		void SetPosition(float fx, float fy);

		const D3DXVECTOR2& GetScale() const;

		void SetImagePointer(CGraphicImage* pImage);
		void ReloadImagePointer(CGraphicImage* pImage);
		bool IsEmpty() const;

		int GetWidth();
		int GetHeight();

#ifdef ENABLE_NEW_DUNGEON_LIB
#ifdef ENABLE_CLIP_MASKING
        void RenderCoolTime(float fCoolTime, RECT* pClipRect = nullptr);
#else
        void RenderCoolTime(float fCoolTime);
#endif
#endif
		D3DXCOLOR GetPixelColor(int x, int y);

		CGraphicTexture * GetTexturePointer();
		const CGraphicTexture &	GetTextureReference() const;
		CGraphicImage * GetGraphicImagePointer();

		bool operator == (const CGraphicImageInstance & rhs) const;

	protected:
		void Initialize();

#ifdef ENABLE_CLIP_MASKING
        virtual void OnRender(RECT* pClipRect);
#ifdef ENABLE_NEW_DUNGEON_LIB
        virtual void OnRenderCoolTime(float fCoolTime, RECT* pClipRect);
#endif
#else
        virtual void OnRender();
#ifdef ENABLE_NEW_DUNGEON_LIB
        virtual void OnRenderCoolTime(float fCoolTime);
#endif
#endif

		virtual void OnSetImagePointer();

		virtual BOOL OnIsType(DWORD dwType);

	protected:
		D3DXCOLOR m_DiffuseColor;
		D3DXVECTOR2 m_v2Position;
#if defined(ENABLE_RENDER_TARGET) || defined(ENABLE_EMOTICONS_SYSTEM)
		D3DXVECTOR2 m_v2Scale;
#endif

		CGraphicImage::TRef m_roImage;

	public:
		static void CreateSystem(UINT uCapacity);
		static void DestroySystem();

		static CGraphicImageInstance* New();
		static void Delete(CGraphicImageInstance* pkImgInst);

		static CDynamicPool<CGraphicImageInstance>		ms_kPool;
};
