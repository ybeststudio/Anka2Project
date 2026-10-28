#pragma once
#include "SpeedTreeForest.h"
#include "SpeedTreeMaterial.h"

#ifdef FIX_MEMORY_SPEEDTREE
class CSpeedTreeForestDirectX8 : public CSpeedTreeForest, public CGraphicBase
#else
class CSpeedTreeForestDirectX8 : public CSpeedTreeForest, public CGraphicBase, public CSingleton<CSpeedTreeForestDirectX8>
#endif
{
public:
	static CSpeedTreeForestDirectX8& Instance()
	{
		return *(CSpeedTreeForestDirectX8*)CSpeedTreeForest::InstancePtr();
	}

	CSpeedTreeForestDirectX8();
#ifdef FIX_MEMORY_SPEEDTREE
	~CSpeedTreeForestDirectX8();
#else
	virtual ~CSpeedTreeForestDirectX8();
#endif

	void                        UploadWindMatrix(unsigned int uiLocation, const float* pMatrix) const;
	void						UpdateCompundMatrix(const D3DXVECTOR3& c_rEyeVec, const D3DXMATRIX& c_rmatView, const D3DXMATRIX& c_rmatProj);
	void						UpdateCompundMatrix2(const D3DXVECTOR3& c_rEyeVec, const D3DXMATRIX& c_rmatView, const D3DXMATRIX& c_rmatProj);

	void                        Render(unsigned long ulRenderBitVector = Forest_RenderAll);
	void						Render2(unsigned long ulRenderBitVector);
	void						Render3(unsigned long ulRenderBitVector);
#ifdef JTX_WATER_REFLECTION
	void						RenderReflection(unsigned long ulRenderBitVector = Forest_RenderAll);
#endif
#ifdef DirectX9Ex
	bool						SetRenderingDevice(IDirect3DDevice9Ex* pDevice);
#else
	bool						SetRenderingDevice(LPDIRECT3DDEVICE9 pDevice);
#endif

private:
	bool                        InitVertexShaders();

private:
#ifdef DirectX9Ex
	IDirect3DDevice9Ex* m_pDx;
#else
	LPDIRECT3DDEVICE9 m_pDx;

#endif

#ifdef DIRECTX9
	LPDIRECT3DVERTEXSHADER9 m_dwBranchVertexShader;			// branch/frond vertex shaders
	LPDIRECT3DVERTEXSHADER9 m_dwLeafVertexShader;			// leaf vertex shader
#else
	DWORD					m_dwBranchVertexShader;			// branch/frond vertex shaders	
	DWORD					m_dwLeafVertexShader;			// leaf vertex shader
#endif;
};

typedef CSpeedTreeForestDirectX8 CSpeedTreeForestDirectX9;
