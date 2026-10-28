#pragma once

#include "StdAfx.h"
#include "GrpImageInstance.h"

#ifdef INSIDE_RENDER
typedef struct tagExpandedRECT
{
	LONG	left_top;
	LONG	left_bottom;
	LONG	top_left;
	LONG	top_right;
	LONG	right_top;
	LONG	right_bottom;
	LONG	bottom_left;
	LONG	bottom_right;
} ExpandedRECT, * PEXPANDEDRECT;
#endif

class CGraphicExpandedImageInstance : public CGraphicImageInstance
{
	public:
		static DWORD Type();
		static void DeleteExpandedImageInstance(CGraphicExpandedImageInstance * pkInstance)
		{
			pkInstance->Destroy();
			ms_kPool.Free(pkInstance);
		}

		enum ERenderingMode
		{
			RENDERING_MODE_NORMAL,
			RENDERING_MODE_SCREEN,
			RENDERING_MODE_COLOR_DODGE,
			RENDERING_MODE_MODULATE,
		};

	public:
		CGraphicExpandedImageInstance();
		virtual ~CGraphicExpandedImageInstance();

		void Destroy();

		void SetDepth(float fDepth);
		void SetOrigin();
		void SetOrigin(float fx, float fy);
		void SetRotation(float fRotation);
		void SetScale(float fx, float fy);
		void SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
		void SetRenderingRectWithScale(float fLeft, float fTop, float fRight, float fBottom);
		void SetRenderingMode(int iMode);
#ifdef INSIDE_RENDER
		void SetRenderBox(RECT& renderBox);
		DWORD GetPixelColor(DWORD x, DWORD y);
		int GetRenderWidth();
		int GetRenderHeight();

		void SetExpandedRenderingRect(float fLeftTop, float fLeftBottom, float fTopLeft, float fTopRight, float fRightTop, float fRightBottom, float fBottomLeft, float fBottomRight);
		void iSetRenderingRect(int iLeft, int iTop, int iRight, int iBottom);
		void iSetExpandedRenderingRect(int iLeftTop, int iLeftBottom, int iTopLeft, int iTopRight, int iRightTop, int iRightBottom, int iBottomLeft, int iBottomRight);
		void SetTextureRenderingRect(float fLeft, float fTop, float fRight, float fBottom);
#endif
/* wtf?
#ifdef ENABLE_NEW_DUNGEON_LIB
        void RenderCoolTime(float fCoolTime);
#endif
*/
#ifdef ENABLE_FISH_GAME
		float GetRotation() { return m_fRotation; }
#endif

	protected:
		void Initialize();

#ifdef ENABLE_CLIP_MASKING
        void OnRender(RECT* pClipRect);
#else
        void OnRender();
#endif
/* wtf?
#ifdef ENABLE_NEW_DUNGEON_LIB
        void OnRenderCoolTime(float fCoolTime);
#endif
*/

		void OnSetImagePointer();

		BOOL OnIsType(DWORD dwType);

	protected:
		/*
		float m_fDepth;
		D3DXVECTOR2 m_v2Origin;
		D3DXVECTOR2 m_v2Scale;
		float m_fRotation;
		*/
		float m_fDepth;
		D3DXVECTOR2 m_v2Origin;
		float m_fRotation;

#ifdef INSIDE_RENDER
		ExpandedRECT m_RenderingRect;
		RECT m_TextureRenderingRect;
		RECT m_renderBox;
		DWORD* m_pColorMap;
#else
		RECT m_RenderingRect;
#endif
		int m_iRenderingMode;
#ifdef INSIDE_RENDER
	private:
		void SaveColorMap();
#endif

	public:
		static void CreateSystem(UINT uCapacity);
		static void DestroySystem();

		static CGraphicExpandedImageInstance* New();
		static void Delete(CGraphicExpandedImageInstance* pkImgInst);

		static CDynamicPool<CGraphicExpandedImageInstance>		ms_kPool;
};
