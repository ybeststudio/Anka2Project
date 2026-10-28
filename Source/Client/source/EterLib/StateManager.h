#ifndef __CSTATEMANAGER_H
#define __CSTATEMANAGER_H

#ifdef ENABLE_DIRECTX9_UPDATE
#include <d3d9.h>
#include <d3dx9.h>
#else
#include <d3d8.h>
#include <d3dx8.h>
#endif

#include <vector>

#include "../EterBase/Singleton.h"

#define CHECK_D3DAPI(a) \
{ \
    HRESULT hr = (a); \
 \
    if (hr != S_OK) \
        assert(!#a); \
}

static const DWORD STATEMANAGER_MAX_RENDERSTATES = 256;
static const DWORD STATEMANAGER_MAX_TEXTURESTATES = 128;
static const DWORD STATEMANAGER_MAX_STAGES = 8;
// ps_3_0 exposes 16 samplers; texture bindings beyond the 8 fixed-function stages (the terrain
// shader binds s8) used to overflow m_Textures and overwrite the cached pixel shader pointer.
static const DWORD STATEMANAGER_MAX_SAMPLERS = 16;
static const DWORD STATEMANAGER_MAX_VCONSTANTS = 256;
static const DWORD STATEMANAGER_MAX_PCONSTANTS = 224;
static const DWORD STATEMANAGER_MAX_TRANSFORMSTATES = 300; // World1 lives way up there...
static const DWORD STATEMANAGER_MAX_STREAMS = 16;

class CStreamData
{
    public:
#ifdef ENABLE_DIRECTX9_UPDATE
        CStreamData(LPDIRECT3DVERTEXBUFFER9 pStreamData = NULL, UINT Stride = 0) : m_lpStreamData(pStreamData), m_Stride(Stride)
#else
        CStreamData(LPDIRECT3DVERTEXBUFFER8 pStreamData = NULL, UINT Stride = 0) : m_lpStreamData(pStreamData), m_Stride(Stride)
#endif
        {
        }

        bool operator == (const CStreamData& rhs) const
        {
            return ((m_lpStreamData == rhs.m_lpStreamData) && (m_Stride == rhs.m_Stride));
        }

#ifdef ENABLE_DIRECTX9_UPDATE
        LPDIRECT3DVERTEXBUFFER9 m_lpStreamData;
#else
        LPDIRECT3DVERTEXBUFFER8 m_lpStreamData;
#endif
        UINT m_Stride;
};

class CIndexData
{
    public:
#ifdef ENABLE_DIRECTX9_UPDATE
        CIndexData(LPDIRECT3DINDEXBUFFER9 pIndexData = NULL, UINT BaseVertexIndex = 0)
#else
        CIndexData(LPDIRECT3DINDEXBUFFER8 pIndexData = NULL, UINT BaseVertexIndex = 0)
#endif
            : m_lpIndexData(pIndexData),
            m_BaseVertexIndex(BaseVertexIndex)
        {
        }

        bool operator == (const CIndexData& rhs) const
        {
            return ((m_lpIndexData == rhs.m_lpIndexData) && (m_BaseVertexIndex == rhs.m_BaseVertexIndex));
        }

#ifdef ENABLE_DIRECTX9_UPDATE
        LPDIRECT3DINDEXBUFFER9 m_lpIndexData;
#else
        LPDIRECT3DINDEXBUFFER8 m_lpIndexData;
#endif
        UINT m_BaseVertexIndex;
};

// State types managed by the class
typedef enum eStateType
{
    STATE_MATERIAL = 0,
    STATE_RENDER,
    STATE_TEXTURE,
    STATE_TEXTURESTAGE,
    STATE_VSHADER,
    STATE_PSHADER,
    STATE_TRANSFORM,
    STATE_VCONSTANT,
    STATE_PCONSTANT,
    STATE_STREAM,
    STATE_INDEX
} eStateType;

class CStateID
{
public:
    CStateID(eStateType Type, DWORD dwValue0 = 0, DWORD dwValue1 = 0)
        : m_Type(Type),
        m_dwValue0(dwValue0),
        m_dwValue1(dwValue1)
    {
    }

    CStateID(eStateType Type, DWORD dwStage, D3DTEXTURESTAGESTATETYPE StageType)
        : m_Type(Type),
        m_dwStage(dwStage),
        m_TextureStageStateType(StageType)
    {
    }

    CStateID(eStateType Type, D3DRENDERSTATETYPE RenderType)
        : m_Type(Type),
        m_RenderStateType(RenderType)
    {
    }

    eStateType m_Type;

    union
    {
        DWORD m_dwValue0;
        DWORD m_dwStage;
        D3DRENDERSTATETYPE m_RenderStateType;
        D3DTRANSFORMSTATETYPE m_TransformStateType;
    };

    union
    {
        DWORD m_dwValue1;
        D3DTEXTURESTAGESTATETYPE m_TextureStageStateType;
    };

};

typedef std::vector<CStateID> TStateID;

class CStateManagerState
{
public:
    CStateManagerState()
    {
    }

    void ResetState()
    {
        DWORD i, y;
        for (i = 0; i < STATEMANAGER_MAX_RENDERSTATES; i++)
        {
            m_RenderStates[i] = 0x7FFFFFFF;
        }

        for (i = 0; i < STATEMANAGER_MAX_STAGES; i++)
        {
            for (y = 0; y < STATEMANAGER_MAX_TEXTURESTATES; y++)
            {
                m_TextureStates[i][y] = 0x7FFFFFFF;
            }
        }

        for (i = 0; i < STATEMANAGER_MAX_STREAMS; i++)
        {
            m_StreamData[i] = CStreamData();
        }

        m_IndexData = CIndexData();

        for (i = 0; i < STATEMANAGER_MAX_SAMPLERS; i++)
        {
            m_Textures[i] = NULL;
        }

        for (i = 0; i < STATEMANAGER_MAX_TRANSFORMSTATES; i++)
        {
            D3DXMatrixIdentity(&m_Matrices[i]);
        }

        for (i = 0; i < STATEMANAGER_MAX_VCONSTANTS; i++)
        {
            m_VertexShaderConstants[i] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
        }

        for (i = 0; i < STATEMANAGER_MAX_PCONSTANTS; i++)
        {
            m_PixelShaderConstants[i] = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
        }

        m_dwPixelShader = 0;
#ifdef ENABLE_DIRECTX9_UPDATE
        m_dwVertexShader = 0;
        m_dwVertexDeclaration = 0;
        m_dwFVF = D3DFVF_XYZ;
        m_bVertexProcessing = false;
#else
        m_dwVertexShader = D3DFVF_XYZ;
#endif
        ZeroMemory(&m_Matrices, sizeof(D3DXMATRIX) * STATEMANAGER_MAX_TRANSFORMSTATES);
    }

    // Renderstates
    DWORD m_RenderStates[STATEMANAGER_MAX_RENDERSTATES];

    // Texture stage states
    DWORD m_TextureStates[STATEMANAGER_MAX_STAGES][STATEMANAGER_MAX_TEXTURESTATES];
#ifdef ENABLE_DIRECTX9_UPDATE
    DWORD m_SamplerStates[STATEMANAGER_MAX_STAGES][STATEMANAGER_MAX_TEXTURESTATES];
#endif

    // Vertex shader constants
    D3DXVECTOR4 m_VertexShaderConstants[STATEMANAGER_MAX_VCONSTANTS];

    // Pixel shader constants
    D3DXVECTOR4 m_PixelShaderConstants[STATEMANAGER_MAX_PCONSTANTS];

    // Textures
#ifdef ENABLE_DIRECTX9_UPDATE
    LPDIRECT3DBASETEXTURE9 m_Textures[STATEMANAGER_MAX_SAMPLERS];
#else
    LPDIRECT3DBASETEXTURE8 m_Textures[STATEMANAGER_MAX_SAMPLERS];
#endif

    // Shaders
#ifdef ENABLE_DIRECTX9_UPDATE
    LPDIRECT3DPIXELSHADER9  m_dwPixelShader;
    LPDIRECT3DVERTEXSHADER9 m_dwVertexShader;
    LPDIRECT3DVERTEXDECLARATION9 m_dwVertexDeclaration;
    DWORD m_dwFVF;
#else
    DWORD m_dwPixelShader;
    DWORD m_dwVertexShader;
#endif

    D3DXMATRIX m_Matrices[STATEMANAGER_MAX_TRANSFORMSTATES];

#ifdef ENABLE_DIRECTX9_UPDATE
    D3DMATERIAL9 m_D3DMaterial;
#else
    D3DMATERIAL8 m_D3DMaterial;
#endif

    CStreamData m_StreamData[STATEMANAGER_MAX_STREAMS];
    CIndexData m_IndexData;

#ifdef ENABLE_DIRECTX9_UPDATE
    bool m_bVertexProcessing;
#endif
};

class CStateManager : public CSingleton<CStateManager>
{
public:
#ifdef DirectX9Ex
    CStateManager(IDirect3DDevice9Ex* lpDevice);
#elif defined(ENABLE_DIRECTX9_UPDATE)
    CStateManager(LPDIRECT3DDEVICE9 lpDevice);
#else
    CStateManager(LPDIRECT3DDEVICE8 lpDevice);
#endif
    virtual ~CStateManager();

//#ifdef ENABLE_DIRECTX9_UPDATE
    void UpdateAnisotropy();
//#enndif

    void SetDefaultState();
    void Restore();

    bool BeginScene();
    void EndScene();

    // Material
    void SaveMaterial();
#ifdef ENABLE_DIRECTX9_UPDATE
    void SaveMaterial(const D3DMATERIAL9* pMaterial);
#else
    void SaveMaterial(const D3DMATERIAL8* pMaterial);
#endif
    void RestoreMaterial();
#ifdef ENABLE_DIRECTX9_UPDATE
    void SetMaterial(const D3DMATERIAL9* pMaterial);
    void GetMaterial(D3DMATERIAL9* pMaterial);
    void SetLight(DWORD index, CONST D3DLIGHT9* pLight);
    void GetLight(DWORD index, D3DLIGHT9* pLight);
#else
    void SetMaterial(const D3DMATERIAL8* pMaterial);
    void GetMaterial(D3DMATERIAL8* pMaterial);
    void SetLight(DWORD index, CONST D3DLIGHT8* pLight);
    void GetLight(DWORD index, D3DLIGHT8* pLight);
#endif

    // Renderstates
    void SaveRenderState(D3DRENDERSTATETYPE Type, DWORD dwValue);
    void RestoreRenderState(D3DRENDERSTATETYPE Type);
    void SetRenderState(D3DRENDERSTATETYPE Type, DWORD Value);
    void GetRenderState(D3DRENDERSTATETYPE Type, DWORD* pdwValue);

    // Textures
#ifdef ENABLE_DIRECTX9_UPDATE
    void SaveTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE9 pTexture);
#else
    void SaveTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE8 pTexture);
#endif
    void RestoreTexture(DWORD dwStage);
#ifdef ENABLE_DIRECTX9_UPDATE
    void SetTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE9 pTexture);
    void GetTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE9* ppTexture);
#else
    void SetTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE8 pTexture);
    void GetTexture(DWORD dwStage, LPDIRECT3DBASETEXTURE8* ppTexture);
#endif

    // Texture stage states
    void SaveTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue);
    void RestoreTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type);
    void SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue);
    void GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pdwValue);
    void SetBestFiltering(DWORD dwStage); // if possible set anisotropy filtering, or use trilinear

#ifdef ENABLE_DIRECTX9_UPDATE
    void SaveSamplerState(DWORD dwStage, D3DSAMPLERSTATETYPE Type, DWORD dwValue);
    void RestoreSamplerState(DWORD dwStage, D3DSAMPLERSTATETYPE Type);
    void SetSamplerState(DWORD dwStage, D3DSAMPLERSTATETYPE Type, DWORD dwValue);
    void GetSamplerState(DWORD dwStage, D3DSAMPLERSTATETYPE Type, DWORD* pdwValue);
#endif

    // Vertex Shader
#ifdef ENABLE_DIRECTX9_UPDATE
    void SaveVertexShader(LPDIRECT3DVERTEXSHADER9 dwShader);
#else
    void SaveVertexShader(DWORD dwShader);
#endif
    void RestoreVertexShader();
#ifdef ENABLE_DIRECTX9_UPDATE
    void SetVertexShader(LPDIRECT3DVERTEXSHADER9 dwShader);
    void GetVertexShader(LPDIRECT3DVERTEXSHADER9* pdwShader);
#else
    void SetVertexShader(DWORD dwShader);
    void GetVertexShader(DWORD* pdwShader);
#endif

#ifdef ENABLE_DIRECTX9_UPDATE
    // Vertex Declaration
    void SaveVertexDeclaration(LPDIRECT3DVERTEXDECLARATION9 dwShader);
    void RestoreVertexDeclaration();
    void SetVertexDeclaration(LPDIRECT3DVERTEXDECLARATION9 dwShader);
    void GetVertexDeclaration(LPDIRECT3DVERTEXDECLARATION9* pdwShader);

    // FVF
    void SaveFVF(DWORD dwShader);
    void RestoreFVF();
    void SetFVF(DWORD dwShader);
    void GetFVF(DWORD * pdwShader);
#endif

    // Pixel Shader
#ifdef ENABLE_DIRECTX9_UPDATE
    void SavePixelShader(LPDIRECT3DPIXELSHADER9 dwShader);
#else
    void SavePixelShader(DWORD dwShader);
#endif
    void RestorePixelShader();
#ifdef ENABLE_DIRECTX9_UPDATE
    void SetPixelShader(LPDIRECT3DPIXELSHADER9 dwShader);
    void GetPixelShader(LPDIRECT3DPIXELSHADER9 * pdwShader);
    // Device'a her zaman yazar ve cache'i senkron tutar. Device'a dogrudan SetPixelShader cagirmak yerine bunu kullan:
    // aksi halde cache ile device ayrisir, sonraki SetPixelShader(nullptr) atlanir ve objeler yanlis shader ile cizilir.
    HRESULT ForcePixelShader(LPDIRECT3DPIXELSHADER9 dwShader);
#else
    void SetPixelShader(DWORD dwShader);
    void GetPixelShader(DWORD* pdwShader);
#endif

    // *** These states are cached, but not protected from multiple sends of the same value.
    // Transform
    void SaveTransform(D3DTRANSFORMSTATETYPE Transform, const D3DMATRIX* pMatrix);
    void RestoreTransform(D3DTRANSFORMSTATETYPE Transform);

#ifdef ENABLE_DIRECTX9_UPDATE
    // VertexProcessing
    void SaveVertexProcessing(bool IsON);
    void RestoreVertexProcessing();
#endif

    // Don't cache-check the transform. To much to do
    void SetTransform(D3DTRANSFORMSTATETYPE Type, const D3DMATRIX* pMatrix);
    void GetTransform(D3DTRANSFORMSTATETYPE Type, D3DMATRIX* pMatrix);

    // SetVertexShaderConstant
    void SaveVertexShaderConstant(DWORD dwRegister, CONST void* pConstantData, DWORD dwConstantCount);
    void RestoreVertexShaderConstant(DWORD dwRegister, DWORD dwConstantCount);
    void SetVertexShaderConstant(DWORD dwRegister, CONST void* pConstantData, DWORD dwConstantCount);

    // SetPixelShaderConstant
    void SavePixelShaderConstant(DWORD dwRegister, CONST void* pConstantData, DWORD dwConstantCount);
    void RestorePixelShaderConstant(DWORD dwRegister, DWORD dwConstantCount);
    void SetPixelShaderConstant(DWORD dwRegister, CONST void* pConstantData, DWORD dwConstantCount);

#ifdef ENABLE_DIRECTX9_UPDATE
    void SaveStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER9 pStreamData, UINT Stride);
#else
    void SaveStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER8 pStreamData, UINT Stride);
#endif
    void RestoreStreamSource(UINT StreamNumber);
#ifdef ENABLE_DIRECTX9_UPDATE
    void SetStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER9 pStreamData, UINT Stride);
#else
    void SetStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER8 pStreamData, UINT Stride);
#endif

#ifdef ENABLE_DIRECTX9_UPDATE
    void SaveIndices(LPDIRECT3DINDEXBUFFER9 pIndexData, UINT BaseVertexIndex);
#else
    void SaveIndices(LPDIRECT3DINDEXBUFFER8 pIndexData, UINT BaseVertexIndex);
#endif
    void RestoreIndices();
#ifdef DirectX9Ex
    inline IDirect3DDevice9Ex* GetDevice() { return m_lpD3DDev; }
#elif defined(ENABLE_DIRECTX9_UPDATE)
    inline LPDIRECT3DDEVICE9 GetDevice() { return m_lpD3DDev; }
#endif

    void PushRenderTarget();
    void PopRenderTarget();
    void ReleaseRenderTargets();

    void EnableClipPlane(int planeId, float A, float B, float C, float D);
    void DisableClipPlane(int planeId);
#ifdef ENABLE_DIRECTX9_UPDATE
    void SetIndices(LPDIRECT3DINDEXBUFFER9 pIndexData, UINT BaseVertexIndex);
#else
    void SetIndices(LPDIRECT3DINDEXBUFFER8 pIndexData, UINT BaseVertexIndex);
#endif

    HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount);
    HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, const void* pVertexStreamZeroData, UINT VertexStreamZeroStride);
#ifdef ENABLE_DIRECTX9_UPDATE
    HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount, INT baseVertexIndex = 0);
    HRESULT DrawIndexedPrimitiveReversed(D3DPRIMITIVETYPE PrimitiveType, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount);
#else
    HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount);
#endif
    HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride);

    // Codes For Debug
    DWORD GetRenderState(D3DRENDERSTATETYPE Type);

private:
#ifdef DirectX9Ex
    void SetDevice(IDirect3DDevice9Ex* lpDevice);
#elif defined(ENABLE_DIRECTX9_UPDATE)
    void SetDevice(LPDIRECT3DDEVICE9 lpDevice);
#else
    void SetDevice(LPDIRECT3DDEVICE8 lpDevice);
#endif

private:
    CStateManagerState m_CurrentState;
    CStateManagerState m_CopyState;
    TStateID m_DirtyStates;
    bool m_bForce;
    bool m_bScene;
    DWORD m_dwBestMinFilter;
    DWORD m_dwBestMagFilter;
    std::vector<LPDIRECT3DSURFACE9> m_renderTargetStack;
    std::vector<D3DVIEWPORT9> m_viewportStack;
    DWORD m_userClipPlanes = 0;

#ifdef DirectX9Ex
    IDirect3DDevice9Ex* m_lpD3DDev;
#elif defined(ENABLE_DIRECTX9_UPDATE)
    LPDIRECT3DDEVICE9 m_lpD3DDev;
#else
    LPDIRECT3DDEVICE8 m_lpD3DDev;
#endif

#ifdef _DEBUG
    // Saving Flag
    bool m_bRenderStateSavingFlag[STATEMANAGER_MAX_RENDERSTATES];
    bool m_bTextureStageStateSavingFlag[STATEMANAGER_MAX_STAGES][STATEMANAGER_MAX_TEXTURESTATES];
//    bool m_bTransformSavingFlag[STATEMANAGER_MAX_TRANSFORMSTATES];
#ifdef ENABLE_DIRECTX9_UPDATE
    bool m_bSamplerStateSavingFlag[STATEMANAGER_MAX_STAGES][STATEMANAGER_MAX_TEXTURESTATES];
#endif
#endif

};

#define STATEMANAGER (CStateManager::Instance())
#endif __CSTATEMANAGER_H
