#pragma once

#ifndef _DX8_TO_DX9_COMPAT_H_
#define _DX8_TO_DX9_COMPAT_H_

#include <d3d9.h>

#define LPDIRECT3D8						LPDIRECT3D9
#define LPDIRECT3DDEVICE8				LPDIRECT3DDEVICE9
#define LPDIRECT3DTEXTURE8				LPDIRECT3DTEXTURE9
#define LPDIRECT3DSURFACE8				LPDIRECT3DSURFACE9
#define LPDIRECT3DVERTEXBUFFER8			LPDIRECT3DVERTEXBUFFER9
#define LPDIRECT3DINDEXBUFFER8			LPDIRECT3DINDEXBUFFER9
#define LPDIRECT3DCUBETEXTURE8			LPDIRECT3DCUBETEXTURE9
#define LPDIRECT3DVOLUMETEXTURE8		LPDIRECT3DVOLUMETEXTURE9
#define LPDIRECT3DSWAPCHAIN8			LPDIRECT3DSWAPCHAIN9
#define LPDIRECT3DQUERY8				LPDIRECT3DQUERY9
#define LPDIRECT3DSTATEBLOCK8			LPDIRECT3DSTATEBLOCK9
#define LPDIRECT3DVERTEXSHADER8			LPDIRECT3DVERTEXSHADER9
#define LPDIRECT3DPIXELSHADER8			LPDIRECT3DPIXELSHADER9
#define LPDIRECT3DVERTEXDECLARATION8	LPDIRECT3DVERTEXDECLARATION9
#define LPDIRECT3DBASETEXTURE8			LPDIRECT3DBASETEXTURE9
#define LPDIRECT3DRESOURCE8				LPDIRECT3DRESOURCE9

#define D3DCAPS8						D3DCAPS9
#define D3DPRESENT_PARAMETERS8			D3DPRESENT_PARAMETERS
#define D3DDISPLAYMODE8					D3DDISPLAYMODE
#define D3DLIGHT8						D3DLIGHT9
#define D3DMATERIAL8					D3DMATERIAL9
#define D3DVIEWPORT8					D3DVIEWPORT9
#define D3DGAMMARAMP8					D3DGAMMARAMP
#define D3DRASTER_STATUS8				D3DRASTER_STATUS
#define D3DSURFACE_DESC8				D3DSURFACE_DESC
#define D3DVOLUME_DESC8					D3DVOLUME_DESC
#define D3DLOCKED_RECT8					D3DLOCKED_RECT
#define D3DLOCKED_BOX8					D3DLOCKED_BOX
#define D3DVERTEXBUFFER_DESC8			D3DVERTEXBUFFER_DESC
#define D3DINDEXBUFFER_DESC8			D3DINDEXBUFFER_DESC

#define D3DDEVTYPE8						D3DDEVTYPE
#define D3DFORMAT8						D3DFORMAT
#define D3DRESOURCETYPE8				D3DRESOURCETYPE
#define D3DPOOL8						D3DPOOL
#define D3DMULTISAMPLE_TYPE8			D3DMULTISAMPLE_TYPE
#define D3DTEXTUREFILTERTYPE8			D3DTEXTUREFILTERTYPE
#define D3DPRIMITIVETYPE8				D3DPRIMITIVETYPE
#define D3DTRANSFORMSTATETYPE8			D3DTRANSFORMSTATETYPE
#define D3DRENDERSTATETYPE8				D3DRENDERSTATETYPE
#define D3DTEXTURESTAGESTATETYPE8		D3DTEXTURESTAGESTATETYPE

class IDirect3DDevice8Wrapper
{
private:
	LPDIRECT3DDEVICE9 m_pDevice;
	
public:
	IDirect3DDevice8Wrapper(LPDIRECT3DDEVICE9 pDevice) : m_pDevice(pDevice) {}
	
	operator LPDIRECT3DDEVICE9() const { return m_pDevice; }
	LPDIRECT3DDEVICE9 GetDevice() const { return m_pDevice; }
	
	inline HRESULT CreateTexture(
		UINT Width, UINT Height, UINT Levels, DWORD Usage,
		D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9* ppTexture)
	{
		return m_pDevice->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture, NULL);
	}
	
	inline HRESULT CreateVolumeTexture(
		UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage,
		D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DVOLUMETEXTURE9* ppVolumeTexture)
	{
		return m_pDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture, NULL);
	}
	
	inline HRESULT CreateCubeTexture(
		UINT EdgeLength, UINT Levels, DWORD Usage,
		D3DFORMAT Format, D3DPOOL Pool, LPDIRECT3DCUBETEXTURE9* ppCubeTexture)
	{
		return m_pDevice->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture, NULL);
	}
	
	inline HRESULT CreateVertexBuffer(
		UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool,
		LPDIRECT3DVERTEXBUFFER9* ppVertexBuffer)
	{
		return m_pDevice->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer, NULL);
	}
	
	inline HRESULT CreateIndexBuffer(
		UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool,
		LPDIRECT3DINDEXBUFFER9* ppIndexBuffer)
	{
		return m_pDevice->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer, NULL);
	}
	
	inline HRESULT CreateRenderTarget(
		UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
		BOOL Lockable, LPDIRECT3DSURFACE9* ppSurface)
	{
		return m_pDevice->CreateRenderTarget(Width, Height, Format, MultiSample, 0, Lockable, ppSurface, NULL);
	}
	
	inline HRESULT CreateDepthStencilSurface(
		UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample,
		LPDIRECT3DSURFACE9* ppSurface)
	{
		return m_pDevice->CreateDepthStencilSurface(Width, Height, Format, MultiSample, 0, TRUE, ppSurface, NULL);
	}
	
	inline HRESULT CreateImageSurface(UINT Width, UINT Height, D3DFORMAT Format, LPDIRECT3DSURFACE9* ppSurface)
	{
		return m_pDevice->CreateOffscreenPlainSurface(Width, Height, Format, D3DPOOL_SCRATCH, ppSurface, NULL);
	}
	
	inline HRESULT SetRenderTarget(LPDIRECT3DSURFACE9 pRenderTarget, LPDIRECT3DSURFACE9 pNewZStencil)
	{
		HRESULT hr = m_pDevice->SetRenderTarget(0, pRenderTarget);
		if (SUCCEEDED(hr) && pNewZStencil)
			hr = m_pDevice->SetDepthStencilSurface(pNewZStencil);
		return hr;
	}
	
	inline HRESULT GetRenderTarget(LPDIRECT3DSURFACE9* ppRenderTarget)
	{
		return m_pDevice->GetRenderTarget(0, ppRenderTarget);
	}
	
	inline HRESULT SetVertexShader(DWORD Handle)
	{
		if (Handle < 0x10000)
			return m_pDevice->SetFVF(Handle);
		else
			return m_pDevice->SetVertexShader((LPDIRECT3DVERTEXSHADER9)Handle);
	}
	
	inline HRESULT GetVertexShader(DWORD* pHandle)
	{
		return m_pDevice->GetFVF(pHandle);
	}
	
	inline HRESULT CopyRects(
		LPDIRECT3DSURFACE9 pSourceSurface, CONST RECT* pSourceRectsArray, UINT cRects,
		LPDIRECT3DSURFACE9 pDestinationSurface, CONST POINT* pDestPointsArray)
	{
		if (cRects == 0 || pSourceRectsArray == NULL)
		{
			return m_pDevice->StretchRect(pSourceSurface, NULL, pDestinationSurface, NULL, D3DTEXF_NONE);
		}
		
		for (UINT i = 0; i < cRects; i++)
		{
			RECT destRect = pSourceRectsArray[i];
			if (pDestPointsArray)
			{
				destRect.left = pDestPointsArray[i].x;
				destRect.top = pDestPointsArray[i].y;
				destRect.right = destRect.left + (pSourceRectsArray[i].right - pSourceRectsArray[i].left);
				destRect.bottom = destRect.top + (pSourceRectsArray[i].bottom - pSourceRectsArray[i].top);
			}
			
			HRESULT hr = m_pDevice->StretchRect(pSourceSurface, &pSourceRectsArray[i], pDestinationSurface, &destRect, D3DTEXF_NONE);
			if (FAILED(hr))
				return hr;
		}
		return D3D_OK;
	}
	
	inline HRESULT ApplyStateBlock(DWORD Token)
	{
		LPDIRECT3DSTATEBLOCK9 pStateBlock = (LPDIRECT3DSTATEBLOCK9)Token;
		return pStateBlock->Apply();
	}
	
	inline HRESULT CaptureStateBlock(DWORD Token)
	{
		LPDIRECT3DSTATEBLOCK9 pStateBlock = (LPDIRECT3DSTATEBLOCK9)Token;
		return pStateBlock->Capture();
	}
	
	inline HRESULT DeleteStateBlock(DWORD Token)
	{
		LPDIRECT3DSTATEBLOCK9 pStateBlock = (LPDIRECT3DSTATEBLOCK9)Token;
		pStateBlock->Release();
		return D3D_OK;
	}
	
	inline HRESULT TestCooperativeLevel() { return m_pDevice->TestCooperativeLevel(); }
	inline UINT GetAvailableTextureMem() { return m_pDevice->GetAvailableTextureMem(); }
	inline HRESULT GetDirect3D(LPDIRECT3D9* ppD3D9) { return m_pDevice->GetDirect3D(ppD3D9); }
	inline HRESULT GetDeviceCaps(D3DCAPS9* pCaps) { return m_pDevice->GetDeviceCaps(pCaps); }
	inline HRESULT GetDisplayMode(D3DDISPLAYMODE* pMode) { return m_pDevice->GetDisplayMode(0, pMode); }
	inline HRESULT GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS* pParameters) { return m_pDevice->GetCreationParameters(pParameters); }
	inline HRESULT SetCursorProperties(UINT XHotSpot, UINT YHotSpot, LPDIRECT3DSURFACE9 pCursorBitmap) { return m_pDevice->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap); }
	inline void SetCursorPosition(int X, int Y, DWORD Flags) { m_pDevice->SetCursorPosition(X, Y, Flags); }
	inline BOOL ShowCursor(BOOL bShow) { return m_pDevice->ShowCursor(bShow); }
	inline HRESULT Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) { return m_pDevice->Reset(pPresentationParameters); }
	inline HRESULT Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) { return m_pDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion); }
	inline HRESULT GetBackBuffer(UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, LPDIRECT3DSURFACE9* ppBackBuffer) { return m_pDevice->GetBackBuffer(0, iBackBuffer, Type, ppBackBuffer); }
	inline HRESULT GetRasterStatus(D3DRASTER_STATUS* pRasterStatus) { return m_pDevice->GetRasterStatus(0, pRasterStatus); }
	inline void SetGammaRamp(DWORD Flags, CONST D3DGAMMARAMP* pRamp) { m_pDevice->SetGammaRamp(0, Flags, pRamp); }
	inline void GetGammaRamp(D3DGAMMARAMP* pRamp) { m_pDevice->GetGammaRamp(0, pRamp); }
	inline HRESULT GetDepthStencilSurface(LPDIRECT3DSURFACE9* ppZStencilSurface) { return m_pDevice->GetDepthStencilSurface(ppZStencilSurface); }
	inline HRESULT BeginScene() { return m_pDevice->BeginScene(); }
	inline HRESULT EndScene() { return m_pDevice->EndScene(); }
	inline HRESULT Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil) { return m_pDevice->Clear(Count, pRects, Flags, Color, Z, Stencil); }
	inline HRESULT SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix) { return m_pDevice->SetTransform(State, pMatrix); }
	inline HRESULT GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) { return m_pDevice->GetTransform(State, pMatrix); }
	inline HRESULT MultiplyTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix) { return m_pDevice->MultiplyTransform(State, pMatrix); }
	inline HRESULT SetViewport(CONST D3DVIEWPORT9* pViewport) { return m_pDevice->SetViewport(pViewport); }
	inline HRESULT GetViewport(D3DVIEWPORT9* pViewport) { return m_pDevice->GetViewport(pViewport); }
	inline HRESULT SetMaterial(CONST D3DMATERIAL9* pMaterial) { return m_pDevice->SetMaterial(pMaterial); }
	inline HRESULT GetMaterial(D3DMATERIAL9* pMaterial) { return m_pDevice->GetMaterial(pMaterial); }
	inline HRESULT SetLight(DWORD Index, CONST D3DLIGHT9* pLight) { return m_pDevice->SetLight(Index, pLight); }
	inline HRESULT GetLight(DWORD Index, D3DLIGHT9* pLight) { return m_pDevice->GetLight(Index, pLight); }
	inline HRESULT LightEnable(DWORD Index, BOOL Enable) { return m_pDevice->LightEnable(Index, Enable); }
	inline HRESULT GetLightEnable(DWORD Index, BOOL* pEnable) { return m_pDevice->GetLightEnable(Index, pEnable); }
	inline HRESULT SetClipPlane(DWORD Index, CONST float* pPlane) { return m_pDevice->SetClipPlane(Index, pPlane); }
	inline HRESULT GetClipPlane(DWORD Index, float* pPlane) { return m_pDevice->GetClipPlane(Index, pPlane); }
	inline HRESULT SetRenderState(D3DRENDERSTATETYPE State, DWORD Value) { return m_pDevice->SetRenderState(State, Value); }
	inline HRESULT GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) { return m_pDevice->GetRenderState(State, pValue); }
	inline HRESULT BeginStateBlock() { return m_pDevice->BeginStateBlock(); }
	inline HRESULT EndStateBlock(DWORD* pToken) { return m_pDevice->EndStateBlock((LPDIRECT3DSTATEBLOCK9*)pToken); }
	inline HRESULT SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus) { return m_pDevice->SetClipStatus(pClipStatus); }
	inline HRESULT GetClipStatus(D3DCLIPSTATUS9* pClipStatus) { return m_pDevice->GetClipStatus(pClipStatus); }
	inline HRESULT GetTexture(DWORD Stage, LPDIRECT3DBASETEXTURE9* ppTexture) { return m_pDevice->GetTexture(Stage, ppTexture); }
	inline HRESULT SetTexture(DWORD Stage, LPDIRECT3DBASETEXTURE9 pTexture) { return m_pDevice->SetTexture(Stage, pTexture); }
	inline HRESULT GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) { return m_pDevice->GetTextureStageState(Stage, Type, pValue); }
	inline HRESULT SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) { return m_pDevice->SetTextureStageState(Stage, Type, Value); }
	inline HRESULT ValidateDevice(DWORD* pNumPasses) { return m_pDevice->ValidateDevice(pNumPasses); }
	inline HRESULT GetInfo(DWORD DevInfoID, void* pDevInfoStruct, DWORD DevInfoStructSize) { return S_OK; } // Deprecated in DX9
	inline HRESULT SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries) { return m_pDevice->SetPaletteEntries(PaletteNumber, pEntries); }
	inline HRESULT GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) { return m_pDevice->GetPaletteEntries(PaletteNumber, pEntries); }
	inline HRESULT SetCurrentTexturePalette(UINT PaletteNumber) { return m_pDevice->SetCurrentTexturePalette(PaletteNumber); }
	inline HRESULT GetCurrentTexturePalette(UINT* PaletteNumber) { return m_pDevice->GetCurrentTexturePalette(PaletteNumber); }
	inline HRESULT DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) { return m_pDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount); }
	inline HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount) { return m_pDevice->DrawIndexedPrimitive(PrimitiveType, 0, minIndex, NumVertices, startIndex, primCount); }
	inline HRESULT DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) { return m_pDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride); }
	inline HRESULT DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) { return m_pDevice->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertexIndices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride); }
	inline HRESULT ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, LPDIRECT3DVERTEXBUFFER9 pDestBuffer, DWORD Flags) { return m_pDevice->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, NULL, Flags); }
	inline HRESULT SetStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER9 pStreamData, UINT Stride) { return m_pDevice->SetStreamSource(StreamNumber, pStreamData, 0, Stride); }
	inline HRESULT GetStreamSource(UINT StreamNumber, LPDIRECT3DVERTEXBUFFER9* ppStreamData, UINT* pStride) { UINT offset; return m_pDevice->GetStreamSource(StreamNumber, ppStreamData, &offset, pStride); }
	inline HRESULT SetIndices(LPDIRECT3DINDEXBUFFER9 pIndexData, UINT BaseVertexIndex) { return m_pDevice->SetIndices(pIndexData); }
	inline HRESULT GetIndices(LPDIRECT3DINDEXBUFFER9* ppIndexData, UINT* pBaseVertexIndex) { *pBaseVertexIndex = 0; return m_pDevice->GetIndices(ppIndexData); }
	inline HRESULT SetFVF(DWORD FVF) { return m_pDevice->SetFVF(FVF); }
	inline HRESULT GetFVF(DWORD* pFVF) { return m_pDevice->GetFVF(pFVF); }
	inline HRESULT SetPixelShader(LPDIRECT3DPIXELSHADER9 pShader) { return m_pDevice->SetPixelShader(pShader); }
	inline HRESULT GetPixelShader(LPDIRECT3DPIXELSHADER9* ppShader) { return m_pDevice->GetPixelShader(ppShader); }
	inline HRESULT SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) { return m_pDevice->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
	inline HRESULT GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) { return m_pDevice->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); }
	inline HRESULT DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo) { return m_pDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo); }
	inline HRESULT DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo) { return m_pDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo); }
	inline HRESULT DeletePatch(UINT Handle) { return m_pDevice->DeletePatch(Handle); }
};

inline LPDIRECT3D9 Direct3DCreate8(UINT SDKVersion)
{
	return Direct3DCreate9(D3D_SDK_VERSION);
}

#define USE_DX8_TO_DX9_WRAPPER

#endif