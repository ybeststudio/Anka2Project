#include "StdAfx.h"
#include "MapOutdoor.h"
#include "TerrainPatch.h"
#include "AreaTerrain.h"
#include "TerrainQuadtree.h"

#include "../eterlib/Camera.h"
#include "../eterlib/StateManager.h"


#define MAX_RENDER_SPALT 150

CArea::TCRCWithNumberVector m_dwRenderedCRCWithNumberVector;

CMapOutdoor::TTerrainNumVector CMapOutdoor::FSortPatchDrawStructWithTerrainNum::m_TerrainNumVector;

void CMapOutdoor::RenderTerrain()
{
	if (!IsVisiblePart(PART_TERRAIN))
		return;

	if (!m_bSettingTerrainVisible)
		return;

	// Inserted by levites
	if (!m_pTerrainPatchProxyList)
		return;

	CCamera * pCamera = CCameraManager::Instance().GetCurrentCamera();
	if (!pCamera)
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CMapOutdoor::RenderTerrain **");
#endif

	auto vv = ms_matView * ms_matProj;
	BuildViewFrustum(vv);

	D3DXVECTOR3 v3Eye = pCamera->GetEye();
	m_fXforDistanceCaculation = -v3Eye.x;
	m_fYforDistanceCaculation = -v3Eye.y;

	//////////////////////////////////////////////////////////////////////////
	// Push
	m_PatchVector.clear();

	__RenderTerrain_RecurseRenderQuadTree(m_pRootNode);

	std::sort(m_PatchVector.begin(),m_PatchVector.end());

	// ±×¸®±â À§ÇÑ º¤ÅÍ ¼¼ÆÃ
	if (CTerrainPatch::SOFTWARE_TRANSFORM_PATCH_ENABLE)
		__RenderTerrain_RenderSoftwareTransformPatch();
	else
		__RenderTerrain_RenderHardwareTransformPatch();

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CMapOutdoor::__RenderTerrain_RecurseRenderQuadTree(CTerrainQuadtreeNode *Node, bool bCullCheckNeed)
{
	if (bCullCheckNeed)
	{
		switch (__RenderTerrain_RecurseRenderQuadTree_CheckBoundingCircle(Node->center, Node->radius))
		{
			case VIEW_ALL:
				// all child nodes need not cull check
				bCullCheckNeed = false;
				break;
			case VIEW_PART:
				break;
			case VIEW_NONE:
				// no need to render
				return;
		}
		// if no need cull check more
		// -> bCullCheckNeed = false;
	}

	if (Node->Size == 1)
	{
		D3DXVECTOR3 v3Center = Node->center;
		float fDistance = fMAX(fabs(v3Center.x + m_fXforDistanceCaculation), fabs(-v3Center.y + m_fYforDistanceCaculation));
		__RenderTerrain_AppendPatch(v3Center, fDistance, Node->PatchNum);
	}
	else
	{
		if (Node->NW_Node != nullptr)
			__RenderTerrain_RecurseRenderQuadTree(Node->NW_Node, bCullCheckNeed);
		if (Node->NE_Node != nullptr)
			__RenderTerrain_RecurseRenderQuadTree(Node->NE_Node, bCullCheckNeed);
		if (Node->SW_Node != nullptr)
			__RenderTerrain_RecurseRenderQuadTree(Node->SW_Node, bCullCheckNeed);
		if (Node->SE_Node != nullptr)
			__RenderTerrain_RecurseRenderQuadTree(Node->SE_Node, bCullCheckNeed);
	}
}

int	CMapOutdoor::__RenderTerrain_RecurseRenderQuadTree_CheckBoundingCircle(const D3DXVECTOR3 & c_v3Center, const float & c_fRadius)
{
	const int count = 6;

	D3DXVECTOR3 center = c_v3Center;
	center.y = -center.y;

	int i;

	float distance[count];
	for(i = 0; i < count; ++i)
	{
		distance[i] = D3DXPlaneDotCoord(&m_plane[i], &center);
		if (distance[i] <= -c_fRadius)
			return VIEW_NONE;
	}

	for(i = 0; i < count;++i)
	{
		if (distance[i] <= c_fRadius)
			return VIEW_PART;
	}

	return VIEW_ALL;
}

void CMapOutdoor::__RenderTerrain_AppendPatch(const D3DXVECTOR3& c_rv3Center, float fDistance, long lPatchNum)
{
	assert(NULL!=m_pTerrainPatchProxyList && "CMapOutdoor::__RenderTerrain_AppendPatch");
	if (!m_pTerrainPatchProxyList[lPatchNum].isUsed())
		return;

	m_pTerrainPatchProxyList[lPatchNum].SetCenterPosition(c_rv3Center);
	m_PatchVector.push_back(std::make_pair(fDistance, lPatchNum));
}

#ifdef ENABLE_DIRECTX9_UPDATE
void CMapOutdoor::ApplyLight(DWORD dwVersion, const D3DLIGHT9& c_rkLight)
#else
void CMapOutdoor::ApplyLight(DWORD dwVersion, const D3DLIGHT8& c_rkLight)
#endif
{
	m_kSTPD.m_dwLightVersion=dwVersion;
#ifdef ENABLE_DIRECTX9_UPDATE
	D3DLIGHT9 light = c_rkLight;
#else
	D3DLIGHT8 light = c_rkLight;
#endif
	if (light.Ambient.r < 0.40f) light.Ambient.r = 0.40f;
	if (light.Ambient.g < 0.40f) light.Ambient.g = 0.40f;
	if (light.Ambient.b < 0.40f) light.Ambient.b = 0.40f;
	STATEMANAGER.SetLight(0, &light);
}

void CMapOutdoor::InitializeVisibleParts()
{
	m_dwVisiblePartFlags=0xffffffff;
}

void CMapOutdoor::SetVisiblePart(int ePart, bool isVisible)
{
	DWORD dwMask=(1<<ePart);
	if (isVisible)
	{
		m_dwVisiblePartFlags|=dwMask;
	}
	else
	{
		DWORD dwReverseMask=~dwMask;
		m_dwVisiblePartFlags&=dwReverseMask;
	}
}

bool CMapOutdoor::IsVisiblePart(int ePart)
{
	DWORD dwMask=(1<<ePart);
	if (dwMask & m_dwVisiblePartFlags)
		return true;

	return false;
}

void CMapOutdoor::SetSplatLimit(int iSplatNum)
{
	m_iSplatLimit = iSplatNum;
}

std::vector<int> & CMapOutdoor::GetRenderedSplatNum(int * piPatch, int * piSplat, float * pfSplatRatio)
{
	*piPatch = m_iRenderedPatchNum;
	*piSplat = m_iRenderedSplatNum;
	*pfSplatRatio = m_iRenderedSplatNumSqSum/float(m_iRenderedPatchNum);

	return m_RenderedTextureNumVector;
}

CArea::TCRCWithNumberVector & CMapOutdoor::GetRenderedGraphicThingInstanceNum(DWORD * pdwGraphicThingInstanceNum, DWORD * pdwCRCNum)
{
	*pdwGraphicThingInstanceNum = m_dwRenderedGraphicThingInstanceNum;
	*pdwCRCNum = m_dwRenderedCRCNum;

	return m_dwRenderedCRCWithNumberVector;
}

void CMapOutdoor::RenderBeforeLensFlare()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CMapOutdoor::RenderBeforeLensFlare **");
#endif

	m_LensFlare.DrawBeforeFlare();

	if (!mc_pEnvironmentData)
	{
		TraceError("CMapOutdoor::RenderBeforeLensFlare mc_pEnvironmentData is NULL");

#ifdef ENABLE_DIRECTX9_UPDATE
        D3DPERF_EndEvent();
#endif

		return;
	}

	m_LensFlare.Compute(mc_pEnvironmentData->DirLights[ENV_DIRLIGHT_BACKGROUND].Direction);

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CMapOutdoor::RenderAfterLensFlare()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CMapOutdoor::RenderAfterLensFlare **");
#endif

	m_LensFlare.AdjustBrightness();
	m_LensFlare.DrawFlare();

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CMapOutdoor::RenderCollision()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CMapOutdoor::RenderCollision **");
#endif

	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CArea * pArea;
		if (GetAreaPointer(i, &pArea))
			pArea->RenderCollision();
	}

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CMapOutdoor::RenderScreenFiltering()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CMapOutdoor::RenderScreenFiltering **");
#endif

	m_ScreenFilter.Render();

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CMapOutdoor::RenderSky(bool isReflection)
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CMapOutdoor::RenderSky **");
#endif

	if (IsVisiblePart(PART_SKY))
		isReflection ? m_SkyBox.RenderWithReflection(true) : m_SkyBox.Render();

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

struct CMapOutdoor_FOpaqueThingInstanceRenderShadow
{
	inline void operator () (CGraphicThingInstance* pkThingInst)
	{
		pkThingInst->RenderToShadowMap();
	}
};

void CMapOutdoor::RenderAreaShadow()
{
	static std::vector<CGraphicThingInstance*> s_kVct_pkOpaqueThingInstShadow;
	s_kVct_pkOpaqueThingInstShadow.clear();

	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CArea * pArea;
		if (GetAreaPointer(i, &pArea))
		{
			pArea->CollectRenderingObject(s_kVct_pkOpaqueThingInstShadow);
		}
	}

	std::for_each(s_kVct_pkOpaqueThingInstShadow.begin(), s_kVct_pkOpaqueThingInstShadow.end(), CMapOutdoor_FOpaqueThingInstanceRenderShadow());
}

void CMapOutdoor::RenderCloud()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CMapOutdoor::RenderCloud **");
#endif

	if (IsVisiblePart(PART_CLOUD))
		m_SkyBox.RenderCloud();

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CMapOutdoor::RenderTree()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CMapOutdoor::RenderTree **");
#endif

	if (IsVisiblePart(PART_TREE))
		CSpeedTreeForestDirectX8::Instance().Render();

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CMapOutdoor::SetInverseViewAndDynamicShaodwMatrices()
{
	CCamera* pCamera = CCameraManager::Instance().GetCurrentCamera();

	if (!pCamera)
		return;

	m_matViewInverse = pCamera->GetInverseViewMatrix();

	D3DXVECTOR3 v3Target = pCamera->GetTarget();

	D3DXVECTOR3 v3LightEye(v3Target.x - 1.732f * 1250.0f,
		v3Target.y - 1250.0f,
		v3Target.z + 2.0f * 1.732f * 1250.0f);

	const auto vv = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXMatrixLookAtRH(&m_matLightView, &v3LightEye, &v3Target, &vv);
	m_matDynamicShadow = m_matViewInverse * m_matLightView * m_matDynamicShadowScale;
}

void CMapOutdoor::OnRender()
{
	SetInverseViewAndDynamicShaodwMatrices();

	SetBlendOperation();

	if (!m_bEnableTerrainOnlyForHeight)
		RenderTerrain();

	RenderArea();
	RenderTree();
	RenderBlendArea();
}

struct FAreaRenderShadow
{
	void operator () (CGraphicObjectInstance * pInstance)
	{
		pInstance->RenderShadow();
		pInstance->Hide();
	}
};

struct FPCBlockerHide
{
	void operator () (CGraphicObjectInstance * pInstance)
	{
		if (pInstance && pInstance->GetType() != TREE_OBJECT)
			pInstance->Hide();
	}
};

struct FRenderPCBlocker
{
	void operator () (CGraphicObjectInstance * pInstance)
	{
		pInstance->Show();
		CGraphicThingInstance* pThingInstance = dynamic_cast <CGraphicThingInstance*> (pInstance);
		if (pThingInstance != nullptr)
		{
			if (pThingInstance->HaveBlendThing())
			{
				STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);
				pThingInstance->BlendRender();
				return;
			}
		}
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

		pInstance->RenderPCBlocker();
	}
};

void CMapOutdoor::RenderEffect()
{
	if (!IsVisiblePart(PART_OBJECT))
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CMapOutdoor::RenderEffect **");
#endif

	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CArea * pArea;
		if (GetAreaPointer(i, &pArea))
		{
			pArea->RenderEffect();
		}
	}

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

struct CMapOutdoor_LessThingInstancePtrRenderOrder
{
	bool operator() (CGraphicThingInstance* pkLeft, CGraphicThingInstance* pkRight)
	{
		CCamera* pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();
		const D3DXVECTOR3& c_rv3CameraPos = pCurrentCamera->GetEye();
		const D3DXVECTOR3& c_v3LeftPos = pkLeft->GetPosition();
		const D3DXVECTOR3& c_v3RightPos = pkRight->GetPosition();
		const auto vv = D3DXVECTOR3(c_rv3CameraPos - c_v3RightPos);
		const auto vv2 = D3DXVECTOR3(c_rv3CameraPos - c_v3LeftPos);

		return D3DXVec3LengthSq(&vv2) < D3DXVec3LengthSq(&vv);
	}
};

struct CMapOutdoor_FOpaqueThingInstanceRender
{
	inline void operator () (CGraphicThingInstance * pkThingInst)
	{
		pkThingInst->Render();
	}
};
struct CMapOutdoor_FBlendThingInstanceRender
{
	inline void operator () (CGraphicThingInstance * pkThingInst)
	{
		pkThingInst->BlendRender();
	}
};

void CMapOutdoor::RenderArea(bool bRenderAmbience)
{
	if (!IsVisiblePart(PART_OBJECT))
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 255, 0, 0), L"** CMapOutdoor::RenderArea **");
#endif

#ifdef ENABLE_DIRECTX9_UPDATE
	// Zone/granny objeleri fixed-function ciziliyor; shadow depth PS gibi bagli kalmis bir pixel shader
	// objeleri siyah (yakin) / sis rengi (uzak) gosteriyordu.
	STATEMANAGER.ForcePixelShader(nullptr);
#endif

	m_dwRenderedCRCNum = 0;
	m_dwRenderedGraphicThingInstanceNum = 0;
	m_dwRenderedCRCWithNumberVector.clear();

	for (int j = 0; j < AROUND_AREA_NUM; ++j)
	{
		CArea * pArea;
		if (GetAreaPointer(j, &pArea))
		{
			pArea->RenderDungeon();
		}
	}

#ifndef WORLD_EDITOR
	// PCBlocker
	std::for_each(m_PCBlockerVector.begin(), m_PCBlockerVector.end(), FPCBlockerHide());

	// Shadow Receiver
	// NOTE: m_lpCharacterShadowMapTexture is D3DFMT_R32F (depth map used by modern cascade shadow shaders).
	// Modulating R32F in Fixed Function Pipeline causes green/blue channels to stay 1.0 while red is darkened,
	// turning all shadow receivers (such as stone bridges and railings) into bright turquoise/cyan stains!
	// Zone models and bridges are properly rendered in the opaque object pass without visual corruption.
#if 0
	if (m_bDrawShadow && m_bDrawChrShadow)
	{
		if (mc_pEnvironmentData != nullptr)
			STATEMANAGER.SetRenderState(D3DRS_FOGCOLOR, 0xFFFFFFFF);

		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
		STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

		// Transform
		STATEMANAGER.SaveTransform(D3DTS_TEXTURE1, &m_matDynamicShadow);
		STATEMANAGER.SetTexture(1, m_lpCharacterShadowMapTexture);

		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP,   D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE);

#ifdef ENABLE_DIRECTX9_UPDATE
        STATEMANAGER.SaveSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
        STATEMANAGER.SaveSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
        STATEMANAGER.SaveSamplerState(1, D3DSAMP_BORDERCOLOR, 0xFFFFFFFF);
#else
        STATEMANAGER.SaveTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_BORDER);
        STATEMANAGER.SaveTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_BORDER);
        STATEMANAGER.SaveTextureStageState(1, D3DTSS_BORDERCOLOR, 0xFFFFFFFF);
#endif

		std::for_each(m_ShadowReceiverVector.begin(), m_ShadowReceiverVector.end(), FAreaRenderShadow());

		STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXCOORDINDEX);
		STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS);

#ifdef ENABLE_DIRECTX9_UPDATE
        STATEMANAGER.RestoreSamplerState(1, D3DSAMP_ADDRESSU);
        STATEMANAGER.RestoreSamplerState(1, D3DSAMP_ADDRESSV);
        STATEMANAGER.RestoreSamplerState(1, D3DSAMP_BORDERCOLOR);
#else
        STATEMANAGER.RestoreTextureStageState(1, D3DTSS_ADDRESSU);
        STATEMANAGER.RestoreTextureStageState(1, D3DTSS_ADDRESSV);
        STATEMANAGER.RestoreTextureStageState(1, D3DTSS_BORDERCOLOR);
#endif

		STATEMANAGER.RestoreTransform(D3DTS_TEXTURE1);
		STATEMANAGER.SetTexture(1, nullptr);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		if (mc_pEnvironmentData != nullptr)
			STATEMANAGER.SetRenderState(D3DRS_FOGCOLOR, mc_pEnvironmentData->FogColor);
	}
#endif
#endif

	STATEMANAGER.SaveRenderState(D3DRS_ZWRITEENABLE, TRUE);

	bool m_isDisableSortRendering=false;

	if (m_isDisableSortRendering)
	{
		for (int i = 0; i < AROUND_AREA_NUM; ++i)
		{
			CArea * pArea;
			if (GetAreaPointer(i, &pArea))
			{
				pArea->Render();

				m_dwRenderedCRCNum += pArea->DEBUG_GetRenderedCRCNum();
				m_dwRenderedGraphicThingInstanceNum += pArea->DEBUG_GetRenderedGrapphicThingInstanceNum();

				CArea::TCRCWithNumberVector & rCRCWithNumberVector = pArea->DEBUG_GetRenderedCRCWithNumVector();

				CArea::TCRCWithNumberVector::iterator aIterator = rCRCWithNumberVector.begin();
				while (aIterator != rCRCWithNumberVector.end())
				{
					DWORD dwCRC = (*aIterator++).dwCRC;

					CArea::TCRCWithNumberVector::iterator aCRCWithNumberVectorIterator =
						std::find_if(m_dwRenderedCRCWithNumberVector.begin(), m_dwRenderedCRCWithNumberVector.end(), CArea::FFindIfCRC(dwCRC));

					if ( m_dwRenderedCRCWithNumberVector.end() == aCRCWithNumberVectorIterator)
					{
						CArea::TCRCWithNumber aCRCWithNumber;
						aCRCWithNumber.dwCRC = dwCRC;
						aCRCWithNumber.dwNumber = 1;
						m_dwRenderedCRCWithNumberVector.push_back(aCRCWithNumber);
					}
					else
					{
						CArea::TCRCWithNumber & rCRCWithNumber = *aCRCWithNumberVectorIterator;
						rCRCWithNumber.dwNumber += 1;
					}
				}
	#ifdef WORLD_EDITOR
				if (bRenderAmbience)
					pArea->RenderAmbience();
	#endif
			}
		}

		std::sort(m_dwRenderedCRCWithNumberVector.begin(), m_dwRenderedCRCWithNumberVector.end(), CArea::CRCNumComp());
	}
	else
	{
		static std::vector<CGraphicThingInstance*> s_kVct_pkOpaqueThingInstSort;
		s_kVct_pkOpaqueThingInstSort.clear();

		for (int i = 0; i < AROUND_AREA_NUM; ++i)
		{
			CArea * pArea;
			if (GetAreaPointer(i, &pArea))
			{
				pArea->CollectRenderingObject(s_kVct_pkOpaqueThingInstSort);
#ifdef WORLD_EDITOR
				if (bRenderAmbience)
					pArea->RenderAmbience();
#endif
			}

		}

		std::sort(s_kVct_pkOpaqueThingInstSort.begin(), s_kVct_pkOpaqueThingInstSort.end(), CMapOutdoor_LessThingInstancePtrRenderOrder());
		std::for_each(s_kVct_pkOpaqueThingInstSort.begin(), s_kVct_pkOpaqueThingInstSort.end(), CMapOutdoor_FOpaqueThingInstanceRender());
	}

	STATEMANAGER.RestoreRenderState(D3DRS_ZWRITEENABLE);

#ifndef WORLD_EDITOR
    if (m_bDrawShadow && m_bDrawChrShadow)
    {
        for (const auto &obj : m_ShadowReceiverVector)
        {
            obj->Show();
        }
    }
#endif

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CMapOutdoor::RenderBlendArea()
{
	if (!IsVisiblePart(PART_OBJECT))
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 255, 0, 0), L"** CMapOutdoor::RenderBlendArea **");
	STATEMANAGER.ForcePixelShader(nullptr);   // RenderArea ile ayni: blend objeler de fixed-function
#endif

	static std::vector<CGraphicThingInstance*> s_kVct_pkBlendThingInstSort;
	s_kVct_pkBlendThingInstSort.clear();

	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CArea * pArea;
		if (GetAreaPointer(i, &pArea))
		{
			pArea->CollectBlendRenderingObject(s_kVct_pkBlendThingInstSort);
		}
	}

	if (s_kVct_pkBlendThingInstSort.size() != 0)
	{
		std::sort(s_kVct_pkBlendThingInstSort.begin(), s_kVct_pkBlendThingInstSort.end(), CMapOutdoor_LessThingInstancePtrRenderOrder());

		STATEMANAGER.SaveRenderState(D3DRS_ZWRITEENABLE, TRUE);
		STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		STATEMANAGER.SetTexture(1, nullptr);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		std::for_each(s_kVct_pkBlendThingInstSort.begin(), s_kVct_pkBlendThingInstSort.end(), CMapOutdoor_FBlendThingInstanceRender());

		STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
		STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
		STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);
		STATEMANAGER.RestoreRenderState(D3DRS_ZWRITEENABLE);
		STATEMANAGER.SetTexture(1, nullptr);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}
void CMapOutdoor::RenderDungeon()
{
	for (int i = 0; i < AROUND_AREA_NUM; ++i)
	{
		CArea * pArea;
		if (!GetAreaPointer(i, &pArea))
			continue;
		pArea->RenderDungeon();
	}
}

void CMapOutdoor::RenderPCBlocker()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CMapOutdoor::RenderPCBlocker **");
#endif

#ifndef WORLD_EDITOR
	// PCBlocker
	if (m_PCBlockerVector.size() != 0)
	{
		STATEMANAGER.SetTexture(0, nullptr);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2,	D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,	D3DTOP_MODULATE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP,	D3DTOP_SELECTARG1);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP,	D3DTOP_DISABLE);

		STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
		STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

#ifdef ENABLE_DIRECTX9_UPDATE
        STATEMANAGER.SaveSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        STATEMANAGER.SaveSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
#else
        STATEMANAGER.SaveTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
        STATEMANAGER.SaveTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
#endif

		STATEMANAGER.SaveTransform(D3DTS_TEXTURE1, &m_matBuildingTransparent);
		CGraphicTexture* pTex = m_BuildingTransparentImageInstance.GetTexturePointer();
		if (pTex)
			STATEMANAGER.SetTexture(1, pTex->GetD3DTexture());
		else
			STATEMANAGER.SetTexture(1, nullptr);

		std::for_each(m_PCBlockerVector.begin(), m_PCBlockerVector.end(), FRenderPCBlocker());

		STATEMANAGER.SetTexture(1, nullptr);
		STATEMANAGER.RestoreTransform(D3DTS_TEXTURE1);

		STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXCOORDINDEX);
		STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

#ifdef ENABLE_DIRECTX9_UPDATE
        STATEMANAGER.RestoreSamplerState(1, D3DSAMP_ADDRESSU);
        STATEMANAGER.RestoreSamplerState(1, D3DSAMP_ADDRESSV);
#else
        STATEMANAGER.RestoreTextureStageState(1, D3DTSS_ADDRESSU);
        STATEMANAGER.RestoreTextureStageState(1, D3DTSS_ADDRESSV);
#endif

		STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	}
#endif

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CMapOutdoor::SelectIndexBuffer(BYTE byLODLevel, WORD * pwPrimitiveCount, D3DPRIMITIVETYPE * pePrimitiveType)
{
#ifdef WORLD_EDITOR
	*pwPrimitiveCount = m_wNumIndices - 2;
	*pePrimitiveType = D3DPT_TRIANGLESTRIP;
	STATEMANAGER.SetIndices(m_IndexBuffer.GetD3DIndexBuffer(), 0);
#else
	if (0 == byLODLevel)
	{
		*pwPrimitiveCount = m_wNumIndices[byLODLevel] - 2;
		*pePrimitiveType = D3DPT_TRIANGLESTRIP;
	}
	else
	{
		*pwPrimitiveCount =  m_wNumIndices[byLODLevel]/3;
		*pePrimitiveType = D3DPT_TRIANGLELIST;
	}
	STATEMANAGER.SetIndices(m_IndexBuffer[byLODLevel].GetD3DIndexBuffer(), 0);
#endif
}

void CMapOutdoor::SetPatchDrawVector()
{
	assert(NULL!=m_pTerrainPatchProxyList && "CMapOutdoor::__SetPatchDrawVector");

	m_PatchDrawStructVector.clear();

	std::vector<std::pair<float, long> >::iterator aDistancePatchVectorIterator;

	TPatchDrawStruct aPatchDrawStruct;

	aDistancePatchVectorIterator = m_PatchVector.begin();
	while(aDistancePatchVectorIterator != m_PatchVector.end())
	{
		std::pair<float, long> adistancePatchPair = *aDistancePatchVectorIterator;

		CTerrainPatchProxy * pTerrainPatchProxy = &m_pTerrainPatchProxyList[adistancePatchPair.second];

		if (!pTerrainPatchProxy->isUsed())
		{
			++aDistancePatchVectorIterator;
			continue;
		}

		long lPatchNum = pTerrainPatchProxy->GetPatchNum();
		if (lPatchNum < 0)
		{
			++aDistancePatchVectorIterator;
			continue;
		}

		BYTE byTerrainNum = pTerrainPatchProxy->GetTerrainNum();
		if (0xFF == byTerrainNum)
		{
			++aDistancePatchVectorIterator;
			continue;
		}

		CTerrain * pTerrain;
		if (!GetTerrainPointer(byTerrainNum, &pTerrain))
		{
			++aDistancePatchVectorIterator;
			continue;
		}

		aPatchDrawStruct.fDistance				= adistancePatchPair.first;
		aPatchDrawStruct.byTerrainNum			= byTerrainNum;
		aPatchDrawStruct.lPatchNum				= lPatchNum;
		aPatchDrawStruct.pTerrainPatchProxy		= pTerrainPatchProxy;

		m_PatchDrawStructVector.push_back(aPatchDrawStruct);

		++aDistancePatchVectorIterator;
	}

	std::stable_sort(m_PatchDrawStructVector.begin(), m_PatchDrawStructVector.end(), FSortPatchDrawStructWithTerrainNum());
}

float CMapOutdoor::__GetNoFogDistance()
{
	return (float)(CTerrainImpl::CELLSCALE * m_lViewRadius) * 0.5f;
}

float CMapOutdoor::__GetFogDistance()
{
	return (float)(CTerrainImpl::CELLSCALE * m_lViewRadius) * 0.75f;
}

struct FPatchNumMatch
{
	long m_lPatchNumToCheck;
	FPatchNumMatch(long lPatchNum)
	{
		m_lPatchNumToCheck = lPatchNum;
	}
	bool operator() (std::pair<long, BYTE> aPair)
	{
		return m_lPatchNumToCheck == aPair.first;
	}
};

void CMapOutdoor::NEW_DrawWireFrame(CTerrainPatchProxy * pTerrainPatchProxy, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
{
	DWORD dwFillMode = STATEMANAGER.GetRenderState(D3DRS_FILLMODE);
	STATEMANAGER.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	DWORD dwFogEnable = STATEMANAGER.GetRenderState(D3DRS_FOGENABLE);
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);

	STATEMANAGER.SetTexture(0, nullptr);
	STATEMANAGER.SetTexture(1, nullptr);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);

	STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);

	STATEMANAGER.SetRenderState(D3DRS_FILLMODE, dwFillMode);
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, dwFogEnable);

	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
}

void CMapOutdoor::DrawWireFrame(long patchnum, WORD wPrimitiveCount, D3DPRIMITIVETYPE ePrimitiveType)
{
	assert(NULL!=m_pTerrainPatchProxyList && "CMapOutdoor::DrawWireFrame");

	CTerrainPatchProxy * pTerrainPatchProxy= &m_pTerrainPatchProxyList[patchnum];

	if (!pTerrainPatchProxy->isUsed())
		return;

	long sPatchNum = pTerrainPatchProxy->GetPatchNum();
	if (sPatchNum < 0)
		return;
	BYTE ucTerrainNum = pTerrainPatchProxy->GetTerrainNum();
	if (0xFF == ucTerrainNum)
		return;

	DWORD dwFillMode = STATEMANAGER.GetRenderState(D3DRS_FILLMODE);
	STATEMANAGER.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	DWORD dwFogEnable = STATEMANAGER.GetRenderState(D3DRS_FOGENABLE);
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, FALSE);

	STATEMANAGER.SetTexture(0, nullptr);
	STATEMANAGER.SetTexture(1, nullptr);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);

	STATEMANAGER.DrawIndexedPrimitive(ePrimitiveType, 0, m_iPatchTerrainVertexCount, 0, wPrimitiveCount);

	STATEMANAGER.SetRenderState(D3DRS_FILLMODE, dwFillMode);
	STATEMANAGER.SetRenderState(D3DRS_FOGENABLE, dwFogEnable);

 	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
}

// Attr
void CMapOutdoor::RenderMarkedArea()
{
	if (!m_pTerrainPatchProxyList)
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CMapOutdoor::RenderMarkedArea **");
#endif

	m_matWorldForCommonUse._41 = 0.0f;
	m_matWorldForCommonUse._42 = 0.0f;
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorldForCommonUse);

	WORD wPrimitiveCount;
	D3DPRIMITIVETYPE eType;
	SelectIndexBuffer(0, &wPrimitiveCount, &eType);

	D3DXMATRIX matTexTransform, matTexTransformTemp;

	D3DXMatrixScaling(&matTexTransform, m_fTerrainTexCoordBase * 32.0f, -m_fTerrainTexCoordBase * 32.0f, 0.0f);
	D3DXMatrixMultiply(&matTexTransform, &m_matViewInverse, &matTexTransform);
	STATEMANAGER.SaveTransform(D3DTS_TEXTURE0, &matTexTransform);
	STATEMANAGER.SaveTransform(D3DTS_TEXTURE1, &matTexTransform);

	STATEMANAGER.SaveRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	STATEMANAGER.SaveRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	STATEMANAGER.SaveRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	static long lStartTime = timeGetTime();
	float fTime = float((timeGetTime() - lStartTime)%3000) / 3000.0f;
	float fAlpha = fabs(fTime - 0.5f) / 2.0f + 0.1f;
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, D3DXCOLOR(1.0f, 1.0f, 1.0f, fAlpha));
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

#ifdef ENABLE_DIRECTX9_UPDATE
    STATEMANAGER.SaveSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
    STATEMANAGER.SaveSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
    STATEMANAGER.SaveSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
    STATEMANAGER.SaveSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    STATEMANAGER.SaveSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
#else
    STATEMANAGER.SaveTextureStageState(1, D3DTSS_MINFILTER, D3DTEXF_POINT);
    STATEMANAGER.SaveTextureStageState(1, D3DTSS_MAGFILTER, D3DTEXF_POINT);
    STATEMANAGER.SaveTextureStageState(1, D3DTSS_MIPFILTER, D3DTEXF_POINT);
    STATEMANAGER.SaveTextureStageState(1, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP);
    STATEMANAGER.SaveTextureStageState(1, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP);
#endif

	STATEMANAGER.SetTexture(0, m_attrImageInstance.GetTexturePointer()->GetD3DTexture());

	RecurseRenderAttr(m_pRootNode);

	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS);

#ifdef ENABLE_DIRECTX9_UPDATE
    STATEMANAGER.RestoreSamplerState(1, D3DSAMP_MINFILTER);
    STATEMANAGER.RestoreSamplerState(1, D3DSAMP_MAGFILTER);
    STATEMANAGER.RestoreSamplerState(1, D3DSAMP_MIPFILTER);
    STATEMANAGER.RestoreSamplerState(1, D3DSAMP_ADDRESSU);
    STATEMANAGER.RestoreSamplerState(1, D3DSAMP_ADDRESSV);
#else
    STATEMANAGER.RestoreTextureStageState(1, D3DTSS_MINFILTER);
    STATEMANAGER.RestoreTextureStageState(1, D3DTSS_MAGFILTER);
    STATEMANAGER.RestoreTextureStageState(1, D3DTSS_MIPFILTER);
    STATEMANAGER.RestoreTextureStageState(1, D3DTSS_ADDRESSU);
    STATEMANAGER.RestoreTextureStageState(1, D3DTSS_ADDRESSV);
#endif

	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE0);
	STATEMANAGER.RestoreTransform(D3DTS_TEXTURE1);

	STATEMANAGER.RestoreRenderState(D3DRS_ALPHABLENDENABLE);
	STATEMANAGER.RestoreRenderState(D3DRS_SRCBLEND);
	STATEMANAGER.RestoreRenderState(D3DRS_DESTBLEND);

	STATEMANAGER.SetTexture(0, nullptr);
	STATEMANAGER.SetTexture(1, nullptr);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	STATEMANAGER.SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

void CMapOutdoor::RecurseRenderAttr(CTerrainQuadtreeNode *Node, bool bCullEnable)
{
	if (bCullEnable)
	{
		if (__RenderTerrain_RecurseRenderQuadTree_CheckBoundingCircle(Node->center, Node->radius)==VIEW_NONE)
			return;
	}

	{
		if (Node->Size == 1)
		{
			DrawPatchAttr(Node->PatchNum);
		}
		else
		{
			if (Node->NW_Node != nullptr)
				RecurseRenderAttr(Node->NW_Node, bCullEnable);
			if (Node->NE_Node != nullptr)
				RecurseRenderAttr(Node->NE_Node, bCullEnable);
			if (Node->SW_Node != nullptr)
				RecurseRenderAttr(Node->SW_Node, bCullEnable);
			if (Node->SE_Node != nullptr)
				RecurseRenderAttr(Node->SE_Node, bCullEnable);
		}
 	}
}

void CMapOutdoor::DrawPatchAttr(long patchnum)
{
	CTerrainPatchProxy * pTerrainPatchProxy = &m_pTerrainPatchProxyList[patchnum];
	if (!pTerrainPatchProxy->isUsed())
		return;

	long sPatchNum = pTerrainPatchProxy->GetPatchNum();
	if (sPatchNum < 0)
		return;

	BYTE ucTerrainNum = pTerrainPatchProxy->GetTerrainNum();
	if (0xFF == ucTerrainNum)
		return;

	// Deal with this material buffer
	CTerrain * pTerrain;
	if (!GetTerrainPointer(ucTerrainNum, &pTerrain))
		return;

	if (!pTerrain->IsMarked())
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(251, 50, 50, 0), L"** CMapOutdoor::DrawPatchAttr **");
#endif

	WORD wCoordX, wCoordY;
	pTerrain->GetCoordinate(&wCoordX, &wCoordY);

	m_matWorldForCommonUse._41 = -(float) (wCoordX * CTerrainImpl::XSIZE * CTerrainImpl::CELLSCALE);
	m_matWorldForCommonUse._42 = (float) (wCoordY * CTerrainImpl::YSIZE * CTerrainImpl::CELLSCALE);

	D3DXMATRIX matTexTransform, matTexTransformTemp;
	D3DXMatrixMultiply(&matTexTransform, &m_matViewInverse, &m_matWorldForCommonUse);
	D3DXMatrixMultiply(&matTexTransform, &matTexTransform, &m_matStaticShadow);
	STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &matTexTransform);

	TTerrainSplatPatch & rAttrSplatPatch = pTerrain->GetMarkedSplatPatch();
 	STATEMANAGER.SetTexture(1, rAttrSplatPatch.Splats[0].pd3dTexture);

#ifdef ENABLE_DIRECTX9_UPDATE
    STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
#else
    STATEMANAGER.SetVertexShader(D3DFVF_XYZ | D3DFVF_NORMAL);
#endif

	STATEMANAGER.SetStreamSource(0, pTerrainPatchProxy->HardwareTransformPatch_GetVertexBufferPtr()->GetD3DVertexBuffer(), m_iPatchTerrainVertexSize);

#ifdef WORLD_EDITOR
	STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, m_iPatchTerrainVertexCount, 0, m_wNumIndices - 2);
#else
	STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, m_iPatchTerrainVertexCount, 0, m_wNumIndices[0] - 2);
#endif

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}
