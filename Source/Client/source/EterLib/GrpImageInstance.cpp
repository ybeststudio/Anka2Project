#include "StdAfx.h"
#include "GrpImageInstance.h"
#include "StateManager.h"

#include "../eterBase/CRC32.h"
//STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR);
//STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
//STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
//STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);

CDynamicPool<CGraphicImageInstance>		CGraphicImageInstance::ms_kPool;

void CGraphicImageInstance::CreateSystem(UINT uCapacity)
{
	ms_kPool.Create(uCapacity);
}

void CGraphicImageInstance::DestroySystem()
{
	ms_kPool.Destroy();
}

CGraphicImageInstance* CGraphicImageInstance::New()
{
	return ms_kPool.Alloc();
}

void CGraphicImageInstance::Delete(CGraphicImageInstance* pkImgInst)
{
	pkImgInst->Destroy();
	ms_kPool.Free(pkImgInst);
}

#ifdef ENABLE_CLIP_MASKING
void CGraphicImageInstance::Render(RECT* pClipRect)
#else
void CGraphicImageInstance::Render()
#endif
{
	if (IsEmpty())
		return;

	assert(!IsEmpty());

#ifdef ENABLE_CLIP_MASKING
	OnRender(pClipRect);
#else
	OnRender();
#endif
}

#ifdef ENABLE_CLIP_MASKING
void CGraphicImageInstance::OnRender(RECT* pClipRect)
#else
void CGraphicImageInstance::OnRender()
#endif
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CGraphicImageInstance::OnRender **");
#endif

#ifdef ENABLE_RENDER_TARGET
	CGraphicTexture* graphicTexture = m_roImage->GetTexturePointer();

	float fimgWidth = m_roImage->GetWidth() * m_v2Scale.x;
	float fimgHeight = m_roImage->GetHeight() * m_v2Scale.y;

	const RECT& c_rRect = m_roImage->GetRectReference();
	float texReverseWidth = 1.0f / float(graphicTexture->GetWidth());
	float texReverseHeight = 1.0f / float(graphicTexture->GetHeight());
	float su = c_rRect.left * texReverseWidth;
	float sv = c_rRect.top * texReverseHeight;
	float eu = (c_rRect.left + (c_rRect.right - c_rRect.left)) * texReverseWidth;
	float ev = (c_rRect.top + (c_rRect.bottom - c_rRect.top)) * texReverseHeight;
#else
	CGraphicImage * pImage = m_roImage.GetPointer();
	CGraphicTexture * pTexture = pImage->GetTexturePointer();

	float fimgWidth = pImage->GetWidth();
	float fimgHeight = pImage->GetHeight();

	const RECT& c_rRect = pImage->GetRectReference();
	float texReverseWidth = 1.0f / float(pTexture->GetWidth());
	float texReverseHeight = 1.0f / float(pTexture->GetHeight());
	float su = c_rRect.left * texReverseWidth;
	float sv = c_rRect.top * texReverseHeight;
	float eu = (c_rRect.left + (c_rRect.right-c_rRect.left)) * texReverseWidth;
	float ev = (c_rRect.top + (c_rRect.bottom-c_rRect.top)) * texReverseHeight;
#endif

#ifdef ENABLE_DIRECTX9_UPDATE
	TPDTVertex vertices[4];
#else
	TPDTVertex vertices[4];
#endif

#ifdef ENABLE_CLIP_MASKING
    if (pClipRect)
    {
        auto sx = m_v2Position.x - 0.5f;
        auto sy = m_v2Position.y - 0.5f;
        auto ex = m_v2Position.x + fimgWidth - 0.5f;
        auto ey = m_v2Position.y + fimgHeight - 0.5f;

        const auto width = ex - sx;
        const auto height = ey - sy;
        const auto uDiff = eu - su;
        const auto vDiff = ev - sv;

        if (ex < pClipRect->left)
        {
            return;
        }

        if (ey < pClipRect->top)
        {
            return;
        }

        if (sx > pClipRect->right)
        {
            return;
        }

        if (sy > pClipRect->bottom)
        {
            return;
        }

        if (sx < pClipRect->left)
        {
            su += (pClipRect->left - sx) / width * uDiff;
            sx = pClipRect->left;
        }

        if (sy < pClipRect->top)
        {
            sv += (pClipRect->top - sy) / height * vDiff;
            sy = pClipRect->top;
        }

        if (ex > pClipRect->right)
        {
            eu -= (ex - pClipRect->right) / width * uDiff;
            ex = pClipRect->right;
        }

        if (ey > pClipRect->bottom)
        {
            ev -= (ey - pClipRect->bottom) / height * vDiff;
            ey = pClipRect->bottom;
        }

        vertices[0].position.x = sx;
        vertices[0].position.y = sy;
        vertices[0].position.z = 0.0f;
        vertices[0].texCoord = TTextureCoordinate(su, sv);
        vertices[0].diffuse = m_DiffuseColor;

        vertices[1].position.x = ex;
        vertices[1].position.y = sy;
        vertices[1].position.z = 0.0f;
        vertices[1].texCoord = TTextureCoordinate(eu, sv);
        vertices[1].diffuse = m_DiffuseColor;

        vertices[2].position.x = sx;
        vertices[2].position.y = ey;
        vertices[2].position.z = 0.0f;
        vertices[2].texCoord = TTextureCoordinate(su, ev);
        vertices[2].diffuse = m_DiffuseColor;

        vertices[3].position.x = ex;
        vertices[3].position.y = ey;
        vertices[3].position.z = 0.0f;
        vertices[3].texCoord = TTextureCoordinate(eu, ev);
        vertices[3].diffuse = m_DiffuseColor;
    }
    else
    {
#endif
    vertices[0].position.x = m_v2Position.x - 0.5f;
    vertices[0].position.y = m_v2Position.y - 0.5f;
    vertices[0].position.z = 0.0f;
    vertices[0].texCoord = TTextureCoordinate(su, sv);
    vertices[0].diffuse = m_DiffuseColor;

    vertices[1].position.x = m_v2Position.x + fimgWidth - 0.5f;
    vertices[1].position.y = m_v2Position.y - 0.5f;
    vertices[1].position.z = 0.0f;
    vertices[1].texCoord = TTextureCoordinate(eu, sv);
    vertices[1].diffuse = m_DiffuseColor;

    vertices[2].position.x = m_v2Position.x - 0.5f;
    vertices[2].position.y = m_v2Position.y + fimgHeight - 0.5f;
    vertices[2].position.z = 0.0f;
    vertices[2].texCoord = TTextureCoordinate(su, ev);
    vertices[2].diffuse = m_DiffuseColor;

    vertices[3].position.x = m_v2Position.x + fimgWidth - 0.5f;
    vertices[3].position.y = m_v2Position.y + fimgHeight - 0.5f;
    vertices[3].position.z = 0.0f;
    vertices[3].texCoord = TTextureCoordinate(eu, ev);
    vertices[3].diffuse = m_DiffuseColor;
#ifdef ENABLE_CLIP_MASKING
    }
#endif

#ifdef ENABLE_OFFICAL_FEATURES
	if (m_bLeftRightReverse)
	{
		vertices[0].texCoord = TTextureCoordinate(eu, sv);
		vertices[1].texCoord = TTextureCoordinate(su, sv);
		vertices[2].texCoord = TTextureCoordinate(eu, ev);
		vertices[3].texCoord = TTextureCoordinate(su, ev);
	}
#endif

	if (CGraphicBase::SetPDTStream(vertices, 4))
	{
		CGraphicBase::SetDefaultIndexBuffer(CGraphicBase::DEFAULT_IB_FILL_RECT);

#ifdef ENABLE_RENDER_TARGET
		STATEMANAGER.SetTexture(0, graphicTexture->GetD3DTexture());
#else
		STATEMANAGER.SetTexture(0, pTexture->GetD3DTexture());
#endif
		STATEMANAGER.SetTexture(1, nullptr);
#ifdef ENABLE_DIRECTX9_UPDATE
		STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
#else
		STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
#endif

		STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);
	}

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

#ifdef ENABLE_OFFICAL_FEATURES
void CGraphicImageInstance::LeftRightReverse()
{
	m_bLeftRightReverse = true;
}
#endif

const CGraphicTexture & CGraphicImageInstance::GetTextureReference() const
{
	return m_roImage->GetTextureReference();
}

CGraphicTexture * CGraphicImageInstance::GetTexturePointer()
{
	CGraphicImage* pkImage = m_roImage.GetPointer();
	return pkImage ? pkImage->GetTexturePointer() : NULL;
}

CGraphicImage * CGraphicImageInstance::GetGraphicImagePointer()
{
	return m_roImage.GetPointer();
}

int CGraphicImageInstance::GetWidth()
{
	if (IsEmpty())
		return 0;

	return m_roImage->GetWidth() * m_v2Scale.x;
}

int CGraphicImageInstance::GetHeight()
{
	if (IsEmpty())
		return 0;

	return m_roImage->GetHeight() * m_v2Scale.y;
}

void CGraphicImageInstance::SetDiffuseColor(float fr, float fg, float fb, float fa)
{
	m_DiffuseColor.r = fr;
	m_DiffuseColor.g = fg;
	m_DiffuseColor.b = fb;
	m_DiffuseColor.a = fa;
}

#if defined(ENABLE_RENDER_TARGET) || defined(ENABLE_EMOTICONS_SYSTEM)
void CGraphicImageInstance::SetScale(float fx, float fy)
{
	m_v2Scale.x = fx;
	m_v2Scale.y = fy;
}
#endif

const D3DXVECTOR2& CGraphicImageInstance::GetScale() const
{
	return m_v2Scale;
}

void CGraphicImageInstance::SetPosition(float fx, float fy)
{
	m_v2Position.x = fx;
	m_v2Position.y = fy;
}

void CGraphicImageInstance::SetImagePointer(CGraphicImage * pImage)
{
	m_roImage.SetPointer(pImage);

	OnSetImagePointer();
}

void CGraphicImageInstance::ReloadImagePointer(CGraphicImage * pImage)
{
	if (m_roImage.IsNull())
	{
		SetImagePointer(pImage);
		return;
	}

	CGraphicImage * pkImage = m_roImage.GetPointer();

	if (pkImage)
		pkImage->Reload();
}

bool CGraphicImageInstance::IsEmpty() const
{
	if (!m_roImage.IsNull() && !m_roImage->IsEmpty())
		return false;

	return true;
}

bool CGraphicImageInstance::operator == (const CGraphicImageInstance & rhs) const
{
	return (m_roImage.GetPointer() == rhs.m_roImage.GetPointer());
}

DWORD CGraphicImageInstance::Type()
{
	static DWORD s_dwType = GetCRC32("CGraphicImageInstance", strlen("CGraphicImageInstance"));
	return (s_dwType);
}

BOOL CGraphicImageInstance::IsType(DWORD dwType)
{
	return OnIsType(dwType);
}

BOOL CGraphicImageInstance::OnIsType(DWORD dwType)
{
	if (CGraphicImageInstance::Type() == dwType)
		return TRUE;

	return FALSE;
}

void CGraphicImageInstance::OnSetImagePointer()
{
}

void CGraphicImageInstance::Initialize()
{
	m_DiffuseColor.r = m_DiffuseColor.g = m_DiffuseColor.b = m_DiffuseColor.a = 1.0f;
	m_v2Position.x = m_v2Position.y = 0.0f;
#if defined(ENABLE_RENDER_TARGET) || defined(ENABLE_EMOTICONS_SYSTEM)
	m_v2Scale.x = m_v2Scale.y = 1.0f;
#endif
#ifdef ENABLE_OFFICAL_FEATURES
	m_bLeftRightReverse = false;
#endif
}

void CGraphicImageInstance::Destroy()
{
	m_roImage.SetPointer(nullptr);
	Initialize();
}

CGraphicImageInstance::CGraphicImageInstance()
{
	Initialize();
}

CGraphicImageInstance::~CGraphicImageInstance()
{
	Destroy();
}

D3DXCOLOR CGraphicImageInstance::GetPixelColor(int x, int y)
{
	D3DXCOLOR dxClr = D3DXCOLOR(0, 0, 0, 0);
	CGraphicImage* pImage = m_roImage.GetPointer();
	if (!pImage)
		return dxClr;
	CGraphicTexture* pTexture = pImage->GetTexturePointer();
	if (!pTexture)
		return dxClr;

	LPDIRECT3DTEXTURE9 d3dTexture = pTexture->GetD3DTexture();
	if (!d3dTexture)
		return dxClr;

	IDirect3DSurface9* surface;
	D3DSURFACE_DESC desc;
	D3DLOCKED_RECT rect;
	RECT rc;

	rc.left = x;
	rc.right = x + 1;
	rc.top = y;
	rc.bottom = y + 1;

	if (FAILED(d3dTexture->GetSurfaceLevel(0, &surface)))
		return dxClr;
	if (FAILED(surface->GetDesc(&desc)))
		return dxClr;
	if (FAILED(surface->LockRect(&rect, &rc, D3DLOCK_READONLY | D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOSYSLOCK)))
		return dxClr;

	PBYTE dwTexel = (PBYTE)rect.pBits;

	switch (desc.Format)
	{
		case D3DFMT_A8R8G8B8:
			dxClr.a = dwTexel[3];
			dxClr.r = dwTexel[2];
			dxClr.g = dwTexel[1];
			dxClr.b = dwTexel[0];
			break;
	}
	surface->UnlockRect();

	return dxClr;
}

#ifdef ENABLE_NEW_DUNGEON_LIB
#ifdef ENABLE_CLIP_MASKING
void CGraphicImageInstance::RenderCoolTime(float fCoolTime, RECT* pClipRect)
#else
void CGraphicImageInstance::RenderCoolTime(float fCoolTime)
#endif
{
	if (IsEmpty())
		return;

	assert(!IsEmpty());

#ifdef ENABLE_CLIP_MASKING
    OnRenderCoolTime(fCoolTime, pClipRect);
#else
    OnRenderCoolTime(fCoolTime);
#endif
}

#ifdef ENABLE_CLIP_MASKING
void CGraphicImageInstance::OnRenderCoolTime(float fCoolTime, RECT* pClipRect)
#else
void CGraphicImageInstance::OnRenderCoolTime(float fCoolTime)
#endif
{
	//if (fCoolTime >= 1.0f)
	//	fCoolTime = 1.0f;

	CGraphicImage* pImage = m_roImage.GetPointer();
	CGraphicTexture* pTexture = pImage->GetTexturePointer();

	// <!!!> ACHTUNG <!!!>
	float fimgWidth = pImage->GetWidth() * m_v2Scale.x;
	float fimgHeight = pImage->GetHeight() * m_v2Scale.y;

	// float fimgWidth = pImage->GetWidth();
	// float fimgHeight = pImage->GetHeight();

	float fimgWidthHalf = fimgWidth * 0.5f;
	float fimgHeightHalf = fimgHeight * 0.5f;

	const RECT& c_rRect = pImage->GetRectReference();
	float texReverseWidth = 1.0f / float(pTexture->GetWidth());
	float texReverseHeight = 1.0f / float(pTexture->GetHeight());
	float su = c_rRect.left * texReverseWidth;
	float sv = c_rRect.top * texReverseHeight;
	float eu = c_rRect.right * texReverseWidth;
	float ev = c_rRect.bottom * texReverseHeight;
	float euh = eu * 0.5f;
	float evh = ev * 0.5f;
	float fxCenter = m_v2Position.x + fimgWidthHalf - 0.5f;
	float fyCenter = m_v2Position.y + fimgHeightHalf - 0.5f;

	if (fCoolTime < 1.0f)
	{
		if (fCoolTime < 0.0)
			fCoolTime = 0.0;

		const int c_iTriangleCountPerBox = 8;
		static D3DXVECTOR2 s_v2BoxPos[c_iTriangleCountPerBox] =
		{
			D3DXVECTOR2(-1.0f, -1.0f),
			D3DXVECTOR2(-1.0f, 0.0f),
			D3DXVECTOR2(-1.0f, +1.0f),
			D3DXVECTOR2(0.0f, +1.0f),
			D3DXVECTOR2(+1.0f, +1.0f),
			D3DXVECTOR2(+1.0f, 0.0f),
			D3DXVECTOR2(+1.0f, -1.0f),
			D3DXVECTOR2(0.0f, -1.0f),
		};

		D3DXVECTOR2 v2TexPos[c_iTriangleCountPerBox] =
		{
			D3DXVECTOR2(su, sv),
			D3DXVECTOR2(su, evh),
			D3DXVECTOR2(su, ev),
			D3DXVECTOR2(euh, ev),
			D3DXVECTOR2(eu, ev),
			D3DXVECTOR2(eu, evh),
			D3DXVECTOR2(eu, sv),
			D3DXVECTOR2(euh, sv),
		};

		int iTriCount = int(8.0f - 8.0f * fCoolTime);
		float fLastPercentage = (8.0f - 8.0f * fCoolTime) - iTriCount;

		std::vector<TPDTVertex> vertices;
		TPDTVertex vertex;
		vertex.position = TPosition(fxCenter, fyCenter, 0.0f);
		vertex.texCoord = TTextureCoordinate(euh, evh);
		vertex.diffuse = m_DiffuseColor;
		vertices.push_back(vertex);

		vertex.position = TPosition(fxCenter, fyCenter - fimgHeightHalf - 0.5f, 0.0f);
		vertex.texCoord = TTextureCoordinate(euh, sv);
		vertex.diffuse = m_DiffuseColor;
		vertices.push_back(vertex);

		if (iTriCount > 0)
		{
			for (int j = 0; j < iTriCount; ++j)
			{
				vertex.position = TPosition(fxCenter + (s_v2BoxPos[j].x * fimgWidthHalf) - 0.5f,
					fyCenter + (s_v2BoxPos[j].y * fimgHeightHalf) - 0.5f,
					0.0f);
				vertex.texCoord = TTextureCoordinate(v2TexPos[j & (c_iTriangleCountPerBox - 1)].x,
					v2TexPos[j & (c_iTriangleCountPerBox - 1)].y);
				vertex.diffuse = m_DiffuseColor;
				vertices.push_back(vertex);
			}
		}

		if (fLastPercentage > 0.0f)
		{
			D3DXVECTOR2* pv2Pos;
			D3DXVECTOR2* pv2LastPos;
			assert((iTriCount + 8) % 8 >= 0 && (iTriCount + 8) % 8 < 8);
			assert((iTriCount + 7) % 8 >= 0 && (iTriCount + 7) % 8 < 8);
			pv2LastPos = &s_v2BoxPos[(iTriCount + 8) % 8];
			pv2Pos = &s_v2BoxPos[(iTriCount + 7) % 8];
			float fxShit = (pv2LastPos->x - pv2Pos->x) * fLastPercentage + pv2Pos->x;
			float fyShit = (pv2LastPos->y - pv2Pos->y) * fLastPercentage + pv2Pos->y;
			vertex.position = TPosition(fimgWidthHalf * fxShit + fxCenter - 0.5f,
				fimgHeightHalf * fyShit + fyCenter - 0.5f,
				0.0f);
			vertex.texCoord = TTextureCoordinate(euh * fxShit + euh,
				evh * fyShit + evh);
			vertex.diffuse = m_DiffuseColor;
			vertices.push_back(vertex);
			++iTriCount;
		}

		if (vertices.empty())
			return;

		STATEMANAGER.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		if (CGraphicBase::SetPDTStream(&vertices[0], vertices.size()))
		{
			CGraphicBase::SetDefaultIndexBuffer(CGraphicBase::DEFAULT_IB_FILL_TRI);
			STATEMANAGER.SetTexture(0, pTexture->GetD3DTexture());
			STATEMANAGER.SetTexture(1, nullptr);

#ifdef ENABLE_DIRECTX9_UPDATE
            STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
#else
            STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
#endif

			STATEMANAGER.DrawPrimitive(D3DPT_TRIANGLEFAN, 0, iTriCount);
		}
		STATEMANAGER.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	}
}
#endif
