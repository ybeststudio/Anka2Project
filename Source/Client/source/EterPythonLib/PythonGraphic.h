#pragma once

#include "../eterlib/GrpTextInstance.h"
#include "../eterlib/GrpMarkInstance.h"
#include "../eterlib/GrpImageInstance.h"
#include "../eterlib/GrpExpandedImageInstance.h"

#include "../eterGrnLib/ThingInstance.h"

class CPythonGraphic : public CScreen, public CSingleton<CPythonGraphic>
{
	public:
		CPythonGraphic();
		virtual ~CPythonGraphic();

		void Destroy();

		void PushState();
		void PopState();

#ifdef ENABLE_DIRECTX9_UPDATE
		LPDIRECT3D9 GetD3D();
#else
		LPDIRECT3D8 GetD3D();
#endif

		float GetOrthoDepth();
		void SetInterfaceRenderState();
		void SetGameRenderState();

		void SetCursorPosition(int x, int y);

		void SetOmniLight();

		void SetViewport(float fx, float fy, float fWidth, float fHeight);
		void RestoreViewport();

#ifdef ENABLE_UI_DEBUG_WINDOW
        void RenderTextLine(float x, float y, const char* c_szText, DWORD dwColor);
#endif
		long GenerateColor(float r, float g, float b, float a);
		void RenderDownButton(float sx, float sy, float ex, float ey);
		void RenderUpButton(float sx, float sy, float ex, float ey);

		void RenderImage(CGraphicImageInstance* pImageInstance, float x, float y);
		void RenderAlphaImage(CGraphicImageInstance* pImageInstance, float x, float y, float aLeft, float aRight);
		void RenderCoolTimeBox(float fxCenter, float fyCenter, float fRadius, float fTime, const D3DXCOLOR& color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f));
		void RenderCoolTimeBoxInverse(float fxCenter, float fyCenter, float fRadius, float fTime);

		bool SaveJPEG(const char *pszFileName, LPBYTE pbyBuffer, UINT uWidth, UINT uHeight);
		bool SaveScreenShot(const char *szFileName);

		DWORD GetAvailableMemory();
		void SetGamma(float fGammaFactor = 1.0f);

	protected:
		typedef struct SState
		{
			D3DXMATRIX matView;
			D3DXMATRIX matProj;
		} TState;

		DWORD		m_lightColor;
		DWORD		m_darkColor;

	protected:
		std::stack<TState>						m_stateStack;

		D3DXMATRIX								m_SaveWorldMatrix;

		CCullingManager							m_CullingManager;

#ifdef ENABLE_DIRECTX9_UPDATE
		D3DVIEWPORT9							m_backupViewport;
#else
		D3DVIEWPORT8							m_backupViewport;
#endif

		float									m_fOrthoDepth;
};