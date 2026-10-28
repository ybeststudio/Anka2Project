#include "StdAfx.h"
#include "../eterLib/StateManager.h"

#include "ActorInstance.h"

bool CActorInstance::ms_isDirLine=false;

bool CActorInstance::IsDirLine()
{
	return ms_isDirLine;
}

void CActorInstance::ShowDirectionLine(bool isVisible)
{
	ms_isDirLine=isVisible;
}

void CActorInstance::SetMaterialColor(DWORD dwColor)
{
	if (m_pkHorse)
		m_pkHorse->SetMaterialColor(dwColor);

	m_dwMtrlColor&=0xff000000;
	m_dwMtrlColor|=(dwColor&0x00ffffff);
}

void CActorInstance::SetMaterialAlpha(DWORD dwAlpha)
{
	m_dwMtrlAlpha=dwAlpha;
}

void CActorInstance::OnRender()
{

	if (GetAlphaValue() < 1.0f)
	{
		for (auto it = m_AttachingEffectList.begin(); it != m_AttachingEffectList.end(); ++it)
		{
			CEffectManager::Instance().SelectEffectInstance(it->dwEffectIndex);
			CEffectManager::Instance().RenderEffect();
		}
	}

#ifdef ENABLE_DIRECTX9_UPDATE
	D3DMATERIAL9 kMtrl;
#else
	D3DMATERIAL8 kMtrl;
#endif
	STATEMANAGER.GetMaterial(&kMtrl);

	kMtrl.Diffuse=D3DXCOLOR(m_dwMtrlColor);
	STATEMANAGER.SetMaterial(&kMtrl);

	STATEMANAGER.SaveRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	switch(m_iRenderMode)
	{
		case RENDER_MODE_NORMAL:
			BeginDiffuseRender();
				RenderWithOneTexture();
			EndDiffuseRender();
			BeginOpacityRender();
				BlendRenderWithOneTexture();
			EndOpacityRender();
			break;
		case RENDER_MODE_BLEND:
			if (m_fAlphaValue == 1.0f)
			{
				BeginDiffuseRender();
					RenderWithOneTexture();
				EndDiffuseRender();
				BeginOpacityRender();
					BlendRenderWithOneTexture();
				EndOpacityRender();
			}
			else if (m_fAlphaValue > 0.0f)
			{
				BeginBlendRender();
					RenderWithOneTexture();
					BlendRenderWithOneTexture();
				EndBlendRender();
			}
			break;
		case RENDER_MODE_ADD:
			BeginAddRender();
				RenderWithOneTexture();
				BlendRenderWithOneTexture();
			EndAddRender();
			break;
		case RENDER_MODE_MODULATE:
			BeginModulateRender();
				RenderWithOneTexture();
				BlendRenderWithOneTexture();
			EndModulateRender();
			break;
	}

	STATEMANAGER.RestoreRenderState(D3DRS_CULLMODE);

	kMtrl.Diffuse=D3DXCOLOR(0xffffffff);
	STATEMANAGER.SetMaterial(&kMtrl);

	if (ms_isDirLine)
	{
		D3DXVECTOR3 kD3DVt3Cur(m_x, m_y, m_z);

		D3DXVECTOR3 kD3DVt3LookDir(0.0f, -1.0f, 0.0f);
		D3DXMATRIX kD3DMatLook;
		D3DXMatrixRotationZ(&kD3DMatLook, D3DXToRadian(GetRotation()));
		D3DXVec3TransformCoord(&kD3DVt3LookDir, &kD3DVt3LookDir, &kD3DMatLook);
		D3DXVec3Scale(&kD3DVt3LookDir, &kD3DVt3LookDir, 200.0f);
		D3DXVec3Add(&kD3DVt3LookDir, &kD3DVt3LookDir, &kD3DVt3Cur);

		D3DXVECTOR3 kD3DVt3AdvDir(0.0f, -1.0f, 0.0f);
		D3DXMATRIX kD3DMatAdv;
		D3DXMatrixRotationZ(&kD3DMatAdv, D3DXToRadian(GetAdvancingRotation()));
		D3DXVec3TransformCoord(&kD3DVt3AdvDir, &kD3DVt3AdvDir, &kD3DMatAdv);
		D3DXVec3Scale(&kD3DVt3AdvDir, &kD3DVt3AdvDir, 200.0f);
		D3DXVec3Add(&kD3DVt3AdvDir, &kD3DVt3AdvDir, &kD3DVt3Cur);

		static CScreen s_kScreen;

		STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
		STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		STATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);
		STATEMANAGER.SaveRenderState(D3DRS_ZENABLE, FALSE);
		STATEMANAGER.SetRenderState(D3DRS_LIGHTING, FALSE);

		s_kScreen.SetDiffuseColor(1.0f, 1.0f, 0.0f);
		s_kScreen.RenderLine3d(kD3DVt3Cur.x, kD3DVt3Cur.y, kD3DVt3Cur.z, kD3DVt3AdvDir.x, kD3DVt3AdvDir.y, kD3DVt3AdvDir.z);

		s_kScreen.SetDiffuseColor(0.0f, 1.0f, 1.0f);
		s_kScreen.RenderLine3d(kD3DVt3Cur.x, kD3DVt3Cur.y, kD3DVt3Cur.z, kD3DVt3LookDir.x, kD3DVt3LookDir.y, kD3DVt3LookDir.z);

		STATEMANAGER.SetRenderState(D3DRS_LIGHTING, TRUE);
		STATEMANAGER.RestoreRenderState(D3DRS_ZENABLE);

		STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
		STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);
		STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAOP);
#ifdef ENABLE_DIRECTX9_UPDATE
		STATEMANAGER.RestoreFVF();
#else
		STATEMANAGER.RestoreVertexShader();
#endif
	}
}

void CActorInstance::BeginDiffuseRender()
{
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CActorInstance::EndDiffuseRender()
{
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
}

void CActorInstance::BeginOpacityRender()
{
	STATEMANAGER.SaveRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAREF, 0);
	STATEMANAGER.SaveRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);
}

void CActorInstance::EndOpacityRender()
{
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHATESTENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAREF);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHAFUNC);
}

void CActorInstance::BeginBlendRender()
{
	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlphaValue));
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
}

void CActorInstance::EndBlendRender()
{
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);
}

void CActorInstance::BeginAddRender()
{
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, m_AddColor);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1,	D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG2,	D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP,	D3DTOP_ADD);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CActorInstance::EndAddRender()
{
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
}

void CActorInstance::RestoreRenderMode()
{
	// NOTE : This is temporary code. I wanna convert this code to that restore the mode to
	//        model's default setting which had has as like specular or normal. - [levites]
	m_iRenderMode = RENDER_MODE_NORMAL;
	if (m_kBlendAlpha.m_isBlending)
	{
		m_kBlendAlpha.m_iOldRenderMode = m_iRenderMode;
	}
}


void CActorInstance::SetAddRenderMode()
{
	m_iRenderMode = RENDER_MODE_ADD;
	if (m_kBlendAlpha.m_isBlending)
	{
		m_kBlendAlpha.m_iOldRenderMode = m_iRenderMode;
	}
}

void CActorInstance::SetRenderMode(int iRenderMode)
{
	m_iRenderMode = iRenderMode;
	if (m_kBlendAlpha.m_isBlending)
	{
		m_kBlendAlpha.m_iOldRenderMode = iRenderMode;
	}
}

void CActorInstance::SetAddColor(const D3DXCOLOR & c_rColor)
{
	m_AddColor = c_rColor;
	m_AddColor.a = 1.0f;
}

void CActorInstance::BeginModulateRender()
{
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, m_AddColor);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2,	D3DTA_DIFFUSE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP,	D3DTOP_MODULATE);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1,	D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG2,	D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP,	D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

void CActorInstance::EndModulateRender()
{
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
}

void CActorInstance::SetModulateRenderMode()
{
	m_iRenderMode = RENDER_MODE_MODULATE;
	if (m_kBlendAlpha.m_isBlending)
	{
		m_kBlendAlpha.m_iOldRenderMode = m_iRenderMode;
	}
}

void CActorInstance::RenderCollisionData()
{
	static CScreen screen;
	D3DXVECTOR3 object_center;
	float sphere_radius;
	GetBoundingSphere(object_center, sphere_radius);

	if (false)
	{
		STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_XRGB(0, isShow() ? 255 : 0, isShow() ? 0 : 255));
		screen.RenderSphere(nullptr, object_center.x, object_center.y, object_center.z, sphere_radius, D3DFILL_WIREFRAME);
	}

	for (const TCollisionPointInstance& point : m_DefendingPointInstanceList)
	{
		for (const CDynamicSphereInstance& collision : point.SphereInstanceVector)
		{
			STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_XRGB(255, 0, 0));
			screen.RenderSphere(nullptr, collision.v3Position.x, collision.v3Position.y, collision.v3Position.z, collision.fRadius, D3DFILL_WIREFRAME);
		}
	}

	for (const TCollisionPointInstance& point : m_BodyPointInstanceList)
	{
		for (const CDynamicSphereInstance& collision : point.SphereInstanceVector)
		{
			STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_XRGB(0, 255, 0));
			screen.RenderSphere(nullptr, collision.v3Position.x, collision.v3Position.y, collision.v3Position.z, collision.fRadius, D3DFILL_WIREFRAME);
		}
	}

	for (const CDynamicSphereInstance& collision : m_kSplashArea.SphereInstanceVector)
	{
		STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_XRGB(0, 0, 255));
		screen.RenderSphere(nullptr, collision.v3Position.x, collision.v3Position.y, collision.v3Position.z, collision.fRadius, D3DFILL_WIREFRAME);
	}
}

void CActorInstance::RenderToShadowMap()
{
	if (RENDER_MODE_BLEND == m_iRenderMode)
	if (GetAlphaValue() < 0.5f)
		return;

	CGraphicThingInstance::RenderToShadowMap();

	if (m_pkHorse)
		m_pkHorse->RenderToShadowMap();
}
