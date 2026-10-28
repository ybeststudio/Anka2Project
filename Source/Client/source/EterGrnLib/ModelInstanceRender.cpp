#include "StdAfx.h"
#include "../eterlib/StateManager.h"
#include "ModelInstance.h"
#include "Model.h"
#if defined(ANK_SYSTEM_GRAPHICS)
#include "../SphereLib/frustum.h"
#endif


void CGrannyModelInstance::DeformNoSkin(const D3DXMATRIX * c_pWorldMatrix)
{
	if (IsEmpty())
		return;

	UpdateWorldPose();
	UpdateWorldMatrices(c_pWorldMatrix);
}

void CGrannyModelInstance::RenderWithOneTexture()
{
	if (IsEmpty())
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    STATEMANAGER.SetVertexShader(nullptr);
    STATEMANAGER.SetVertexDeclaration(ms_pntVS);

    LPDIRECT3DVERTEXBUFFER9 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
    LPDIRECT3DVERTEXBUFFER9 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();
#else
    STATEMANAGER.SetVertexShader(ms_pntVS);

    LPDIRECT3DVERTEXBUFFER8 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
    LPDIRECT3DVERTEXBUFFER8 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();
#endif

	if (lpd3dDeformPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dDeformPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithOneTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_DIFFUSE_PNT);
	}
	if (lpd3dRigidPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dRigidPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithOneTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_DIFFUSE_PNT);
	}
}

void CGrannyModelInstance::BlendRenderWithOneTexture()
{
	if (IsEmpty())
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    LPDIRECT3DVERTEXBUFFER9 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
    LPDIRECT3DVERTEXBUFFER9 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();

    STATEMANAGER.SetVertexShader(nullptr);
    STATEMANAGER.SetVertexDeclaration(ms_pntVS);
#else
    LPDIRECT3DVERTEXBUFFER8 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
    LPDIRECT3DVERTEXBUFFER8 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();

    STATEMANAGER.SetVertexShader(ms_pntVS);
#endif

	if (lpd3dDeformPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dDeformPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithOneTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_BLEND_PNT);
	}

	if (lpd3dRigidPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dRigidPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithOneTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_BLEND_PNT);
	}
}

// With Two Texture
void CGrannyModelInstance::RenderWithTwoTexture()
{
	if (IsEmpty())
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    STATEMANAGER.SetVertexShader(nullptr);
    STATEMANAGER.SetVertexDeclaration(ms_pntVS);

    LPDIRECT3DVERTEXBUFFER9 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
    LPDIRECT3DVERTEXBUFFER9 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();
#else
    STATEMANAGER.SetVertexShader(ms_pntVS);

    LPDIRECT3DVERTEXBUFFER8 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
    LPDIRECT3DVERTEXBUFFER8 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();
#endif

	if (lpd3dDeformPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dDeformPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithTwoTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_DIFFUSE_PNT);
	}
	if (lpd3dRigidPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dRigidPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithTwoTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_DIFFUSE_PNT);
	}
}

void CGrannyModelInstance::BlendRenderWithTwoTexture()
{
	if (IsEmpty())
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    LPDIRECT3DVERTEXBUFFER9 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
    LPDIRECT3DVERTEXBUFFER9 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();

    STATEMANAGER.SetVertexShader(nullptr);
    STATEMANAGER.SetVertexDeclaration(ms_pntVS);
#else
    LPDIRECT3DVERTEXBUFFER8 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
    LPDIRECT3DVERTEXBUFFER8 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();

    STATEMANAGER.SetVertexShader(ms_pntVS);
#endif

	if (lpd3dDeformPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dDeformPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithTwoTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_BLEND_PNT);
	}

	if (lpd3dRigidPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dRigidPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithTwoTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_BLEND_PNT);
	}
}

void CGrannyModelInstance::RenderWithoutTexture()
{
	if (IsEmpty())
		return;

#ifdef ENABLE_DIRECTX9_UPDATE
    STATEMANAGER.SetVertexDeclaration(ms_pntVS);
    // Depth pass: the shadow-map PS discards texels with alpha < 0.33. Sampling an unbound
    // sampler returns alpha 1, so the whole silhouette is written; the old shadowhelper.png,
    // sampled at the mesh UVs, discarded characters and they cast no shadow.
    STATEMANAGER.SetTexture(0, nullptr);
    STATEMANAGER.SetTexture(1, nullptr);

    LPDIRECT3DVERTEXBUFFER9 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
    LPDIRECT3DVERTEXBUFFER9 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();
#else
    STATEMANAGER.SetVertexShader(ms_pntVS);
    // Depth pass: the shadow-map PS discards texels with alpha < 0.33. Sampling an unbound
    // sampler returns alpha 1, so the whole silhouette is written; the old shadowhelper.png,
    // sampled at the mesh UVs, discarded characters and they cast no shadow.
    STATEMANAGER.SetTexture(0, nullptr);
    STATEMANAGER.SetTexture(1, nullptr);

    LPDIRECT3DVERTEXBUFFER8 lpd3dDeformPNTVtxBuf = __GetDeformableD3DVertexBufferPtr();
    LPDIRECT3DVERTEXBUFFER8 lpd3dRigidPNTVtxBuf = m_pModel->GetPNTD3DVertexBuffer();
#endif

	if (lpd3dDeformPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dDeformPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithoutTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_DIFFUSE_PNT);
		RenderMeshNodeListWithoutTexture(CGrannyMesh::TYPE_DEFORM, CGrannyMaterial::TYPE_BLEND_PNT);
	}

	if (lpd3dRigidPNTVtxBuf)
	{
		STATEMANAGER.SetStreamSource(0, lpd3dRigidPNTVtxBuf, sizeof(TPNTVertex));
		RenderMeshNodeListWithoutTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_DIFFUSE_PNT);
		RenderMeshNodeListWithoutTexture(CGrannyMesh::TYPE_RIGID, CGrannyMaterial::TYPE_BLEND_PNT);
	}
}

void CGrannyModelInstance::RenderMeshNodeListWithOneTexture(CGrannyMesh::EType eMeshType, CGrannyMaterial::EType eMtrlType)
{
	assert(m_pModel != nullptr);

#ifdef ENABLE_DIRECTX9_UPDATE
    LPDIRECT3DINDEXBUFFER9 lpd3dIdxBuf = m_pModel->GetD3DIndexBuffer();
#else
    LPDIRECT3DINDEXBUFFER8 lpd3dIdxBuf = m_pModel->GetD3DIndexBuffer();
#endif

	assert(lpd3dIdxBuf != nullptr);

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 50, 0), L"** CGrannyModelInstance::RenderMeshNodeListWithOneTexture **");
#endif

	const CGrannyModel::TMeshNode * pMeshNode = m_pModel->GetMeshNodeList(eMeshType, eMtrlType);

	while (pMeshNode)
	{
		const CGrannyMesh * pMesh = pMeshNode->pMesh;
		int vtxMeshBasePos = pMesh->GetVertexBasePosition();

		STATEMANAGER.SetIndices(lpd3dIdxBuf, vtxMeshBasePos);
		STATEMANAGER.SetTransform(D3DTS_WORLD, &m_meshMatrices[pMeshNode->iMesh]);

#ifdef ENABLE_DIRECTX9_UPDATE
		D3DXMATRIX matShader;
		D3DXMatrixTranspose(&matShader, &m_meshMatrices[pMeshNode->iMesh]);
		STATEMANAGER.SetVertexShaderConstant(20, &matShader, 4);
#endif

		const CGrannyMesh::TTriGroupNode* pTriGroupNode = pMesh->GetTriGroupNodeList(eMtrlType);
		int vtxCount = pMesh->GetVertexCount();
		while (pTriGroupNode)
		{
			ms_faceCount += pTriGroupNode->triCount;

			CGrannyMaterial& rkMtrl=m_kMtrlPal.GetMaterialRef(pTriGroupNode->mtrlIndex);
			rkMtrl.ApplyRenderState();

#ifdef ENABLE_DIRECTX9_UPDATE
            STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST
                                                , 0
                                                , vtxCount
                                                , pTriGroupNode->idxPos
                                                , pTriGroupNode->triCount
                                                , vtxMeshBasePos);
#else
            STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST
                                                , 0
                                                , vtxCount
                                                , pTriGroupNode->idxPos
                                                , pTriGroupNode->triCount);
#endif

			rkMtrl.RestoreRenderState();

			pTriGroupNode = pTriGroupNode->pNextTriGroupNode;
		}

		pMeshNode = pMeshNode->pNextMeshNode;
	}

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

// With Two Texture
void CGrannyModelInstance::RenderMeshNodeListWithTwoTexture(CGrannyMesh::EType eMeshType, CGrannyMaterial::EType eMtrlType)
{
	assert(m_pModel != nullptr);

#ifdef ENABLE_DIRECTX9_UPDATE
    LPDIRECT3DINDEXBUFFER9 lpd3dIdxBuf = m_pModel->GetD3DIndexBuffer();
#else
    LPDIRECT3DINDEXBUFFER8 lpd3dIdxBuf = m_pModel->GetD3DIndexBuffer();
#endif

	assert(lpd3dIdxBuf != nullptr);

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 50, 100, 0), L"** CGrannyModelInstance::RenderMeshNodeListWithTwoTexture **");
#endif

	const CGrannyModel::TMeshNode * pMeshNode = m_pModel->GetMeshNodeList(eMeshType, eMtrlType);

	while (pMeshNode)
	{
		const CGrannyMesh * pMesh = pMeshNode->pMesh;
		int vtxMeshBasePos = pMesh->GetVertexBasePosition();

		STATEMANAGER.SetIndices(lpd3dIdxBuf, vtxMeshBasePos);
		STATEMANAGER.SetTransform(D3DTS_WORLD, &m_meshMatrices[pMeshNode->iMesh]);

#ifdef ENABLE_DIRECTX9_UPDATE
		D3DXMATRIX matShader;
		D3DXMatrixTranspose(&matShader, &m_meshMatrices[pMeshNode->iMesh]);
		STATEMANAGER.SetVertexShaderConstant(20, &matShader, 4);
#endif

		const CGrannyMesh::TTriGroupNode* pTriGroupNode = pMesh->GetTriGroupNodeList(eMtrlType);
		int vtxCount = pMesh->GetVertexCount();
		while (pTriGroupNode)
		{
			ms_faceCount += pTriGroupNode->triCount;

			const CGrannyMaterial& rkMtrl=m_kMtrlPal.GetMaterialRef(pTriGroupNode->mtrlIndex);
			STATEMANAGER.SetTexture(0, rkMtrl.GetD3DTexture(0));
			STATEMANAGER.SetTexture(1, rkMtrl.GetD3DTexture(1));

#ifdef ENABLE_DIRECTX9_UPDATE
            STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST
                                                , 0
                                                , vtxCount
                                                , pTriGroupNode->idxPos
                                                , pTriGroupNode->triCount
                                                , vtxMeshBasePos);
#else
            STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST
                                                , 0
                                                , vtxCount
                                                , pTriGroupNode->idxPos
                                                , pTriGroupNode->triCount);
#endif

			pTriGroupNode = pTriGroupNode->pNextTriGroupNode;
		}
		/////

		pMeshNode = pMeshNode->pNextMeshNode;
	}

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}

// Without Texture
void CGrannyModelInstance::RenderMeshNodeListWithoutTexture(CGrannyMesh::EType eMeshType, CGrannyMaterial::EType eMtrlType)
{
	assert(m_pModel != nullptr);

#ifdef ENABLE_DIRECTX9_UPDATE
    LPDIRECT3DINDEXBUFFER9 lpd3dIdxBuf = m_pModel->GetD3DIndexBuffer();
#else
    LPDIRECT3DINDEXBUFFER8 lpd3dIdxBuf = m_pModel->GetD3DIndexBuffer();
#endif

	assert(lpd3dIdxBuf != nullptr);

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_BeginEvent(D3DCOLOR_ARGB(255, 140, 140, 0), L"** CGrannyModelInstance::RenderMeshNodeListWithoutTexture **");
#endif

	const CGrannyModel::TMeshNode * pMeshNode = m_pModel->GetMeshNodeList(eMeshType, eMtrlType);

	while (pMeshNode)
	{
		const CGrannyMesh * pMesh = pMeshNode->pMesh;
		int vtxMeshBasePos = pMesh->GetVertexBasePosition();

		STATEMANAGER.SetIndices(lpd3dIdxBuf, vtxMeshBasePos);
		STATEMANAGER.SetTransform(D3DTS_WORLD, &m_meshMatrices[pMeshNode->iMesh]);

#ifdef ENABLE_DIRECTX9_UPDATE
		D3DXMATRIX matShader;
		D3DXMatrixTranspose(&matShader, &m_meshMatrices[pMeshNode->iMesh]);
		STATEMANAGER.SetVertexShaderConstant(20, &matShader, 4);
#endif

		const CGrannyMesh::TTriGroupNode* pTriGroupNode = pMesh->GetTriGroupNodeList(eMtrlType);
		int vtxCount = pMesh->GetVertexCount();

		while (pTriGroupNode)
		{
			ms_faceCount += pTriGroupNode->triCount;

#ifdef ENABLE_DIRECTX9_UPDATE
            STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST
                                                , 0
                                                , vtxCount
                                                , pTriGroupNode->idxPos
                                                , pTriGroupNode->triCount
                                                , vtxMeshBasePos);
#else
            STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST
                                                , 0
                                                , vtxCount
                                                , pTriGroupNode->idxPos
                                                , pTriGroupNode->triCount);
#endif

			pTriGroupNode = pTriGroupNode->pNextTriGroupNode;
		}

		pMeshNode = pMeshNode->pNextMeshNode;
	}

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DPERF_EndEvent();
#endif
}
