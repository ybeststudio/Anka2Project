#pragma once

#include "StdAfx.h"
#include "GrpDetector.h"
#include "Ray.h"
#include <vector>

#ifdef ENABLE_FIX_MOBS_LAG
static const std::size_t SMALL_PDT_VERTEX_BUFFER_SIZE = 16;
static const std::size_t LARGE_PDT_VERTEX_BUFFER_SIZE = 200;
#endif

void PixelPositionToD3DXVECTOR3(const D3DXVECTOR3& c_rkPPosSrc, D3DXVECTOR3* pv3Dst);
void D3DXVECTOR3ToPixelPosition(const D3DXVECTOR3& c_rv3Src, D3DXVECTOR3* pv3Dst);

class CGraphicTexture;

typedef WORD TIndex;

typedef struct SFace
{
	TIndex indices[3];
} TFace;

typedef D3DXVECTOR3 TPosition;

typedef D3DXVECTOR3 TNormal;

typedef D3DXVECTOR2 TTextureCoordinate;

typedef DWORD TDiffuse;
typedef DWORD TAmbient;
typedef DWORD TSpecular;

typedef union UDepth
{
	float	f;
	long	l;
	DWORD	dw;
} TDepth;

typedef struct SVertex
{
	float x, y, z;
	DWORD color;
	float u, v;
} TVertex;

struct STVertex
{
	float x, y, z, rhw;
};

struct SPVertex
{
	float x, y, z;
};

typedef struct SPDVertex
{
	float x, y, z;
	DWORD color;
} TPDVertex;

struct SPDTVertexRaw
{
	float px, py, pz;
	DWORD diffuse;
	float u, v;
};

typedef struct SPTVertex
{
	TPosition position;
	TTextureCoordinate texCoord;
} TPTVertex;

typedef struct SPDTVertex
{
	TPosition	position;
	TDiffuse	diffuse;
	TTextureCoordinate texCoord;
#ifdef ENABLE_FIX_MOBS_LAG
	static const DWORD kFVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
#endif
} TPDTVertex;

typedef struct SPNTVertex
{
	TPosition			position;
	TNormal				normal;
	TTextureCoordinate	texCoord;
} TPNTVertex;

typedef struct SPNT2Vertex
{
	TPosition	position;
	TNormal		normal;
	TTextureCoordinate texCoord;
	TTextureCoordinate texCoord2;
} TPNT2Vertex;

typedef struct SPDT2Vertex
{
	TPosition	position;
	DWORD		diffuse;
	TTextureCoordinate texCoord;
	TTextureCoordinate texCoord2;
} TPDT2Vertex;

typedef struct SNameInfo
{
	DWORD	name;
	TDepth	depth;
} TNameInfo;

typedef struct SBoundBox
{
	float sx, sy, sz;
	float ex, ey, ez;
	int meshIndex;
	int boneIndex;
} TBoundBox;

const WORD c_FillRectIndices[6] = { 0, 2, 1, 2, 3, 1 };

class CGraphicBase
{
	public:
		static DWORD GetAvailableTextureMemory();
		static const D3DXMATRIX& GetViewMatrix();
		static const D3DXMATRIX & GetIdentityMatrix();

		enum
		{
			DEFAULT_IB_LINE,
			DEFAULT_IB_LINE_TRI,
			DEFAULT_IB_LINE_RECT,
			DEFAULT_IB_LINE_CUBE,
			DEFAULT_IB_FILL_TRI,
			DEFAULT_IB_FILL_RECT,
			DEFAULT_IB_FILL_CUBE,
			DEFAULT_IB_NUM,
		};

	public:
		CGraphicBase();
		virtual	~CGraphicBase();

		void		SetSimpleCamera(float x, float y, float z, float pitch, float roll);
		void		SetEyeCamera(float xEye, float yEye, float zEye, float xCenter, float yCenter, float zCenter, float xUp, float yUp, float zUp);
		void		SetAroundCamera(float distance, float pitch, float roll, float lookAtZ = 0.0f);
		void		SetPositionCamera(float fx, float fy, float fz, float fDistance, float fPitch, float fRotation);
		void		MoveCamera(float fdeltax, float fdeltay, float fdeltaz);

		void		GetTargetPosition(float * px, float * py, float * pz);
		void		GetCameraPosition(float * px, float * py, float * pz);
		void		SetOrtho2D(float hres, float vres, float zres);
		void		SetOrtho3D(float hres, float vres, float zmin, float zmax);
		void		SetPerspective(float fov, float aspect, float nearz, float farz);
#ifdef ENABLE_RENDER_TARGET
		float		GetFOV() const;

		float		GetAspect() const;
		float		GetNear() const;
		float		GetFar() const;
#else
		float		GetFOV();
#endif

#if defined(ANK_SYSTEM_GRAPHICS)
		int			GetShadowIntensity();
#endif
		void		GetClipPlane(float * fNearY, float * fFarY)
		{
			*fNearY = ms_fNearY;
			*fFarY = ms_fFarY;
		}

		////////////////////////////////////////////////////////////////////////
		void		PushMatrix();

		void		MultMatrix( const D3DXMATRIX* pMat );
		void		MultMatrixLocal( const D3DXMATRIX* pMat );

		void		Translate(float x, float y, float z);
		void		Rotate(float degree, float x, float y, float z);
		void		RotateLocal(float degree, float x, float y, float z);
		void		RotateYawPitchRollLocal(float fYaw, float fPitch, float fRoll);
		void		Scale(float x, float y, float z);
		void		PopMatrix();
		void		LoadMatrix(const D3DXMATRIX & c_rSrcMatrix);
		void		GetMatrix(D3DXMATRIX * pRetMatrix) const;
		const		D3DXMATRIX * GetMatrixPointer() const;

		// Special Routine
		void		GetSphereMatrix(D3DXMATRIX * pMatrix, float fValue = 0.1f);

		////////////////////////////////////////////////////////////////////////
		void		InitScreenEffect();
		void		SetScreenEffectWaving(float fDuringTime, int iPower);
		void		SetScreenEffectFlashing(float fDuringTime, const D3DXCOLOR & c_rColor);

		////////////////////////////////////////////////////////////////////////
		DWORD		GetColor(float r, float g, float b, float a = 1.0f);

		DWORD		GetFaceCount();
		void		ResetFaceCount();
		HRESULT		GetLastResult();

		void		UpdateProjMatrix();
		void		UpdateViewMatrix();

		void		SetViewport(DWORD dwX, DWORD dwY, DWORD dwWidth, DWORD dwHeight, float fMinZ, float fMaxZ);
		static void		GetBackBufferSize(UINT* puWidth, UINT* puHeight);
		static bool		IsTLVertexClipping();
		static bool		IsFastTNL();
		static bool		IsLowTextureMemory();
		static bool		IsHighTextureMemory();

		static void SetDefaultIndexBuffer(UINT eDefIB);
		static bool SetPDTStream(SPDTVertexRaw* pVertices, UINT uVtxCount);
		static bool SetPDTStream(SPDTVertex* pVertices, UINT uVtxCount);
#ifdef ENABLE_FIX_MOBS_LAG
#ifdef ENABLE_DIRECTX9_UPDATE
		static IDirect3DVertexBuffer9* GetSmallPdtVertexBuffer();
		static IDirect3DVertexBuffer9* GetLargePdtVertexBuffer();
#else
		static IDirect3DVertexBuffer8* GetSmallPdtVertexBuffer();
		static IDirect3DVertexBuffer8* GetLargePdtVertexBuffer();
#endif
#endif

#if defined(ANK_SYSTEM_GRAPHICS)
	public:
		static int						ms_fShadowIntensity;
	protected:
#endif

	protected:
		static D3DXMATRIX				ms_matIdentity;

		static D3DXMATRIX				ms_matView;
		static D3DXMATRIX				ms_matProj;
		static D3DXMATRIX				ms_matInverseView;
		static D3DXMATRIX				ms_matInverseViewYAxis;

		static D3DXMATRIX				ms_matWorld;
		static D3DXMATRIX				ms_matWorldView;

	protected:
		void		UpdatePipeLineMatrix();

	protected:
		static LPD3DXMESH				ms_lpSphereMesh;
		static LPD3DXMESH				ms_lpCylinderMesh;
		static LPD3DXMESH				ms_lpBoxMesh;

	protected:
		static HRESULT					ms_hLastResult;

		static int						ms_iWidth;
		static int						ms_iHeight;

		static UINT						ms_iD3DAdapterInfo;
		static UINT						ms_iD3DDevInfo;
		static UINT						ms_iD3DModeInfo;
		static D3D_CDisplayModeAutoDetector				ms_kD3DDetector;

		static HWND						ms_hWnd;
		static HDC						ms_hDC;

#ifdef DirectX9Ex
		static IDirect3D9Ex*			ms_lpd3d;
		static IDirect3DDevice9Ex*		ms_lpd3dDevice;
#elif defined(ENABLE_DIRECTX9_UPDATE)
		static LPDIRECT3D9				ms_lpd3d;
		static LPDIRECT3DDEVICE9		ms_lpd3dDevice;
#else
        static LPDIRECT3D8				ms_lpd3d;
        static LPDIRECT3DDEVICE8		ms_lpd3dDevice;
#endif

		static ID3DXMatrixStack*		ms_lpd3dMatStack;

#ifdef ENABLE_DIRECTX9_UPDATE
		static D3DVIEWPORT9				ms_Viewport;
#else
		static D3DVIEWPORT8				ms_Viewport;
#endif

		static DWORD					ms_faceCount;

#ifdef ENABLE_DIRECTX9_UPDATE
        static D3DCAPS9					ms_d3dCaps;
#else
        static D3DCAPS8					ms_d3dCaps;
#endif

		static D3DPRESENT_PARAMETERS	ms_d3dPresentParameter;

		static DWORD					ms_dwD3DBehavior;

#ifdef ENABLE_DIRECTX9_UPDATE
		static LPDIRECT3DVERTEXDECLARATION9 ms_ptVS;
		static LPDIRECT3DVERTEXDECLARATION9 ms_pntVS;
		static LPDIRECT3DVERTEXDECLARATION9 ms_pnt2VS;
#else
		static DWORD ms_ptVS;
		static DWORD ms_pntVS;
		static DWORD ms_pnt2VS;
#endif

		static D3DXMATRIX				ms_matScreen0;
		static D3DXMATRIX				ms_matScreen1;
		static D3DXMATRIX				ms_matScreen2;

		static D3DXVECTOR3				ms_vtPickRayOrig;
		static D3DXVECTOR3				ms_vtPickRayDir;

		static float					ms_fFieldOfView;
		static float					ms_fAspect;
		static float					ms_fNearY;
		static float					ms_fFarY;

		// Screen Effect - Waving, Flashing and so on..
		static DWORD					ms_dwWavingEndTime;
		static int						ms_iWavingPower;
		static DWORD					ms_dwFlashingEndTime;
		static D3DXCOLOR				ms_FlashingColor;
 		static CRay						ms_Ray;

		static bool						ms_bSupportDXT;
		static bool						ms_isLowTextureMemory;
		static bool						ms_isHighTextureMemory;

		enum
		{
			PDT_VERTEX_NUM = 16,
			PDT_VERTEXBUFFER_NUM = 100,
		};

#ifdef ENABLE_FIX_MOBS_LAG
#ifdef ENABLE_DIRECTX9_UPDATE
		static IDirect3DVertexBuffer9* m_smallPdtVertexBuffer;
		static IDirect3DVertexBuffer9* m_largePdtVertexBuffer;
		static LPDIRECT3DINDEXBUFFER9 ms_alpd3dDefIB[DEFAULT_IB_NUM];
#else
		static IDirect3DVertexBuffer8* m_smallPdtVertexBuffer;
		static IDirect3DVertexBuffer8* m_largePdtVertexBuffer;
		static LPDIRECT3DINDEXBUFFER8 ms_alpd3dDefIB[DEFAULT_IB_NUM];
#endif
#else
#ifdef ENABLE_DIRECTX9_UPDATE
		static LPDIRECT3DVERTEXBUFFER9 ms_alpd3dPDTVB[PDT_VERTEXBUFFER_NUM];
		static LPDIRECT3DINDEXBUFFER9 ms_alpd3dDefIB[DEFAULT_IB_NUM];
#else
		static LPDIRECT3DVERTEXBUFFER8 ms_alpd3dPDTVB[PDT_VERTEXBUFFER_NUM];
		static LPDIRECT3DINDEXBUFFER8 ms_alpd3dDefIB[DEFAULT_IB_NUM];
#endif
#endif
};

#ifndef ms_lpd3dExDeviceEx
#define ms_lpd3dExDeviceEx ms_lpd3dDevice
#define ms_lpd3dExMatStack ms_lpd3dMatStack
#endif
