// EffectParticleTexturePreview.cpp : implementation file
//

#include "StdAfx.h"
#include "../WorldEditor.h"
#include "effectparticletexturepreview.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEffectParticleTexturePreview

CEffectParticleTexturePreview::CEffectParticleTexturePreview()
{
	m_pImage = NULL;
}

CEffectParticleTexturePreview::~CEffectParticleTexturePreview()
{
}


BEGIN_MESSAGE_MAP (CEffectParticleTexturePreview, CWnd)
	//{{AFX_MSG_MAP(CEffectParticleTexturePreview)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CEffectParticleTexturePreview::OnPaint()
{
	//CPaintDC dc(this); // device context for painting

	if (IsWindowVisible() && m_pImage)
	{
		Begin();
		SetClearColor (1.0f, 1.0f, 1.0f);
		Clear();

		D3DXMATRIX matIdentity, matWorld;
		D3DXMatrixIdentity (&matIdentity);
		STATEMANAGER.SaveTransform (D3DTS_PROJECTION, &matIdentity);
		STATEMANAGER.SaveTransform (D3DTS_VIEW, &matIdentity);
		STATEMANAGER.SaveTransform (D3DTS_WORLD, &matIdentity);
		STATEMANAGER.SaveRenderState (D3DRS_ALPHABLENDENABLE, TRUE);
		STATEMANAGER.SaveRenderState (D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		STATEMANAGER.SaveRenderState (D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		CRect rect;
		GetClientRect (rect);
		SetOrtho2D ((float)rect.Width(), (float)rect.Height(), 1.0f);

		const RECT & c_rRect = m_ImageInstance.GetGraphicImagePointer()->GetRectReference();

		D3DXMatrixScaling (&matWorld,
						   static_cast<float> (rect.Width()) / static_cast<float> (c_rRect.right - c_rRect.left + 1),
						   static_cast<float> (rect.Height()) / static_cast<float> (c_rRect.bottom - c_rRect.top + 1), 1.0f);

		STATEMANAGER.SetTransform (D3DTS_WORLD, &matWorld);

		m_ImageInstance.SetDiffuseColor (1.0f, 1.0f, 1.0f, 1.0f);
		m_ImageInstance.Render();

		STATEMANAGER.RestoreRenderState (D3DRS_ALPHABLENDENABLE);
		STATEMANAGER.RestoreRenderState (D3DRS_SRCBLEND);
		STATEMANAGER.RestoreRenderState (D3DRS_DESTBLEND);
		STATEMANAGER.RestoreTransform (D3DTS_WORLD);
		STATEMANAGER.RestoreTransform (D3DTS_VIEW);
		STATEMANAGER.RestoreTransform (D3DTS_PROJECTION);

		End();
		Show (NULL, GetSafeHwnd());
	}
	else
	{
		CWnd::OnPaint();
	}

	ValidateRect (NULL);

	// Do not call CWnd::OnPaint() for painting messages
}

void CEffectParticleTexturePreview::UpdatePreview (CGraphicImage * pImage)
{
	m_pImage = pImage;
	m_ImageInstance.Destroy();
	m_ImageInstance.SetImagePointer (pImage);
	Invalidate();
}
