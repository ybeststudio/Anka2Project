#pragma once
#include "SpeedTreeMaterial.h"
#include <SpeedTreeRT.h>
#ifdef DIRECTX9
#include <d3d9.h>
#include <d3d9types.h>
#include <d3dx9.h>
#else
#include <d3d8.h>
#include <d3d8types.h>
#include <d3dx8.h>
#endif
#include <vector>
#include "../eterLib/GrpObjectInstance.h"
#include "../eterLib/GrpImageInstance.h"
#ifdef FIX_MEMORY_SPEEDTREE
#include <memory>
#include <list>
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p) = NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p) = NULL; } }
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p) = NULL; } }
#endif

#ifdef FIX_MEMORY_SPEEDTREE
class CSpeedTreeWrapper : public CGraphicObjectInstance, public std::enable_shared_from_this<CSpeedTreeWrapper>
#else
class CSpeedTreeWrapper : public CGraphicObjectInstance
#endif
{
#ifdef FIX_MEMORY_SPEEDTREE
    typedef std::shared_ptr<CSpeedTreeWrapper> SpeedTreeWrapperPtr;
    typedef std::weak_ptr<CSpeedTreeWrapper> SpeedTreeWrapperWeakPtr;
#endif

public:
    enum
    {
        ID = TREE_OBJECT
    };
    int GetType() const { return ID; }

public:
    UINT GetCollisionObjectCount();
    void GetCollisionObject(UINT nIndex, CSpeedTreeRT::ECollisionObjectType& eType, float* pPosition, float* pDimensions);

protected:
    virtual void OnUpdateCollisionData(const CStaticCollisionDataVector* pscdVector);
    virtual void OnUpdateHeighInstance(CAttributeInstance* pAttributeInstance) {}
    virtual bool OnGetObjectHeight(float fX, float fY, float* pfHeight) { return false; }
public:
    virtual bool GetBoundingSphere(D3DXVECTOR3& v3Center, float& fRadius);
public:
    static bool					ms_bSelfShadowOn;
    static bool					ms_bDynamicLeafLighting;
public:
    virtual void				SetPosition(float x, float y, float z);
    virtual void				CalculateBBox();
    virtual void				OnRender();
    virtual void				OnRenderPCBlocker();
    virtual void				OnBlendRender() {}
    virtual void				OnRenderToShadowMap() {}
    virtual void				OnRenderShadow() {}

public:
    CSpeedTreeWrapper();
    virtual                             ~CSpeedTreeWrapper();

    const float* GetPosition();
#ifdef DIRECTX9
    static void					SetVertexShaders(LPDIRECT3DVERTEXSHADER9 dwBranchVertexShader, LPDIRECT3DVERTEXSHADER9 dwLeafVertexShader);
#else
    static void					SetVertexShaders(DWORD dwBranchVertexShader, DWORD dwLeafVertexShader);
#endif

#ifdef FIX_MEMORY_SPEEDTREE
    bool                        LoadTree(const char* pszSptFile, const BYTE* c_pbBlock = NULL, unsigned int uiBlockSize = 0, unsigned int nSeed = 1, float fSize = -1.0f, float fSizeVariance = -1.0f);
#else
    bool                        LoadTree(const char* pszSptFile, const BYTE* c_pbBlock = NULL, unsigned int uiBlockSize = 0, unsigned int nSeed = 1, float fSize = -1.0f, float fSizeVariance = -1.0f);
#endif
    const float* GetBoundingBox(void) const { return m_afBoundingBox; }
    void                        SetupBranchForTreeType(void);
    void                        SetupFrondForTreeType(void);
    void                        SetupLeafForTreeType(void);
    void                        EndLeafForTreeType(void);
    void                        RenderBranches(void) const;
    void                        RenderFronds(void) const;
    void                        RenderFrondsShadow(void) const;
    void                        RenderLeaves(void) const;
    void                        RenderLeaves2(void) const;
    void                        SetupBranchForTreeTypeShadow(void);
    void                        RenderBranchesShadow(void) const;
    void                        RenderBillboards(void) const;

    void                        UploadLeafTables(unsigned int uiLocation) const;

#ifdef FIX_MEMORY_SPEEDTREE
    std::list<SpeedTreeWrapperWeakPtr> GetInstances(unsigned int& nCount);
    SpeedTreeWrapperPtr InstanceOf(void) const { return m_pInstanceOf; }
    SpeedTreeWrapperPtr MakeInstance();
    void DeleteInstance(SpeedTreeWrapperPtr pInstance);
    CSpeedTreeRT* GetSpeedTree(void) const { return m_pSpeedTree; }
#else
CSpeedTreeWrapper** GetInstances(unsigned int& nCount);
CSpeedTreeWrapper* InstanceOf(void) const { return m_pInstanceOf; }
CSpeedTreeWrapper* MakeInstance();
void						DeleteInstance(CSpeedTreeWrapper* pInstance);
CSpeedTreeRT* GetSpeedTree(void) const { return m_pSpeedTree; }
#endif
    const CSpeedTreeMaterial& GetBranchMaterial(void) const { return m_cBranchMaterial; }
    const CSpeedTreeMaterial& GetFrondMaterial(void) const { return m_cFrondMaterial; }
    const CSpeedTreeMaterial& GetLeafMaterial(void) const { return m_cLeafMaterial; }
    float                       GetLeafLightingAdjustment(void) const { return m_pSpeedTree->GetLeafLightingAdjustment(); }
    void                        Advance(void);
    void                        CleanUpMemory(void);

private:
    void                        SetupBuffers(void);
    void                        SetupBranchBuffers(void);
    void                        SetupFrondBuffers(void);
    void                        SetupLeafBuffers(void);
    void                        PositionTree(void) const;
    void                        PositionTreeBranch(void) const;
    static  bool						LoadTexture(const char* pFilename, CGraphicImageInstance& rImage);
    void                        SetShaderConstants(const float* pMaterial) const;


private:
    CSpeedTreeRT* m_pSpeedTree;
    CSpeedTreeRT::STextures* m_pTextureInfo;
    bool                            m_bIsInstance;
#ifdef FIX_MEMORY_SPEEDTREE
    std::vector<SpeedTreeWrapperPtr> m_vInstances; // what is an instance of us
    SpeedTreeWrapperPtr				 m_pInstanceOf; // which tree is this an instance of
#else
    std::vector<CSpeedTreeWrapper*>	m_vInstances;					// what is an instance of us
    CSpeedTreeWrapper* m_pInstanceOf;					// which tree is this an instance of
#endif
    CSpeedTreeRT::SGeometry* m_pGeometryCache;
    LPDIRECT3DVERTEXBUFFER9         m_pBranchVertexBuffer;
    unsigned int                    m_unBranchVertexCount;
    LPDIRECT3DINDEXBUFFER9          m_pBranchIndexBuffer;
    unsigned short* m_pBranchIndexCounts;
    LPDIRECT3DVERTEXBUFFER9         m_pFrondVertexBuffer;
    unsigned int                    m_unFrondVertexCount;
    unsigned int					m_unNumFrondLods;
    LPDIRECT3DINDEXBUFFER9          m_pFrondIndexBuffers;
    unsigned short* m_pFrondIndexCounts;
    unsigned short                  m_usNumLeafLods;
    LPDIRECT3DVERTEXBUFFER9* m_pLeafVertexBuffer;
    bool* m_pLeavesUpdatedByCpu;
    float                           m_afPos[3];
    float                           m_afBoundingBox[6];
    CSpeedTreeMaterial              m_cBranchMaterial;
    CSpeedTreeMaterial              m_cLeafMaterial;
    CSpeedTreeMaterial              m_cFrondMaterial;
    CGraphicImageInstance           m_BranchImageInstance;
    CGraphicImageInstance           m_ShadowImageInstance;
    CGraphicImageInstance			m_CompositeImageInstance;
#ifdef FIX_MEMORY_SPEEDTREE
    static  unsigned int            m_unNumWrappersActive;
#endif

#ifdef DIRECTX9
	static LPDIRECT3DVERTEXSHADER9	ms_dwBranchVertexShader;
	static LPDIRECT3DVERTEXSHADER9	ms_dwLeafVertexShader;
#else
	static DWORD					ms_dwBranchVertexShader;
	static DWORD					ms_dwLeafVertexShader;
#endif
};

//martysama0134's 484266e236b8a7567df2ca6d4004b570
