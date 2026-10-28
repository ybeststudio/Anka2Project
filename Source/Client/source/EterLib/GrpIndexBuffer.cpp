#include "StdAfx.h"
#include "../eterBase/Stl.h"
#include "GrpIndexBuffer.h"
#include "StateManager.h"

#ifdef ENABLE_DIRECTX9_UPDATE
LPDIRECT3DINDEXBUFFER9 CGraphicIndexBuffer::GetD3DIndexBuffer() const
#else
LPDIRECT3DINDEXBUFFER8 CGraphicIndexBuffer::GetD3DIndexBuffer() const
#endif
{
	return m_lpd3dIdxBuf;
}

void CGraphicIndexBuffer::SetIndices(int startIndex) const
{
	if (ms_lpd3dDevice && m_lpd3dIdxBuf)
		STATEMANAGER.SetIndices(m_lpd3dIdxBuf, startIndex);
}


bool CGraphicIndexBuffer::Lock(void** pretIndices) const
{
	if (!m_lpd3dIdxBuf)
		return false;

#ifdef ENABLE_DIRECTX9_UPDATE
	if (FAILED(m_lpd3dIdxBuf->Lock(0, 0, pretIndices, 0)))
#else
	if (FAILED(m_lpd3dIdxBuf->Lock(0, 0, (BYTE**)pretIndices, 0)))
#endif
    {
        return false;
    }

	return true;
}

void CGraphicIndexBuffer::Unlock() const
{
	if (m_lpd3dIdxBuf)
		m_lpd3dIdxBuf->Unlock();
}

bool CGraphicIndexBuffer::Lock(void** pretIndices)
{
	if (!m_lpd3dIdxBuf)
		return false;

#ifdef ENABLE_DIRECTX9_UPDATE
	if (FAILED(m_lpd3dIdxBuf->Lock(0, 0, pretIndices, 0)))
#else
	if (FAILED(m_lpd3dIdxBuf->Lock(0, 0, (BYTE**)pretIndices, 0)))
#endif
    {
        return false;
    }

	return true;
}

void CGraphicIndexBuffer::Unlock()
{
	if (m_lpd3dIdxBuf)
		m_lpd3dIdxBuf->Unlock();
}

bool CGraphicIndexBuffer::Copy(int bufSize, const void* srcIndices)
{
	if (!m_lpd3dIdxBuf)
		return false;

	BYTE* dstIndices;
#ifdef ENABLE_DIRECTX9_UPDATE
	if (FAILED(m_lpd3dIdxBuf->Lock(0, 0, (void**)&dstIndices, 0)))
#else
	if (FAILED(m_lpd3dIdxBuf->Lock(0, 0, &dstIndices, 0)))
#endif
    {
        return false;
    }

	memcpy(dstIndices, srcIndices, bufSize);

	m_lpd3dIdxBuf->Unlock();

	return true;
}

bool CGraphicIndexBuffer::Create(int faceCount, TFace* faces)
{
	int idxCount = faceCount * 3;
	m_iidxCount = idxCount;
	if (!Create(idxCount, D3DFMT_INDEX16))
		return false;

	WORD* dstIndices;
#ifdef ENABLE_DIRECTX9_UPDATE
	if (FAILED(m_lpd3dIdxBuf->Lock(0, 0, (void**)&dstIndices, 0)))
#else
	if (FAILED(m_lpd3dIdxBuf->Lock(0, 0, (BYTE**)dstIndices, 0)))
#endif
    {
        return false;
    }

	for (int i = 0; i<faceCount; ++i, dstIndices+=3)
	{
		TFace * curFace=faces+i;
		dstIndices[0]=curFace->indices[0];
		dstIndices[1]=curFace->indices[1];
		dstIndices[2]=curFace->indices[2];
	}

	m_lpd3dIdxBuf->Unlock();
	return true;
}

bool CGraphicIndexBuffer::CreateDeviceObjects()
{
	if (!ms_lpd3dDevice)
		return false;

#ifdef DirectX9Ex
    HRESULT hr = ms_lpd3dDevice->CreateIndexBuffer(m_dwBufferSize, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, m_d3dFmt, D3DPOOL_DEFAULT, &m_lpd3dIdxBuf, nullptr);
#elif defined(ENABLE_DIRECTX9_UPDATE)
    HRESULT hr = ms_lpd3dDevice->CreateIndexBuffer(m_dwBufferSize, D3DUSAGE_WRITEONLY, m_d3dFmt, D3DPOOL_MANAGED, &m_lpd3dIdxBuf, nullptr);
#else
    HRESULT hr = ms_lpd3dDevice->CreateIndexBuffer(m_dwBufferSize, D3DUSAGE_WRITEONLY, m_d3dFmt, D3DPOOL_MANAGED, &m_lpd3dIdxBuf);
#endif
    if (FAILED(hr))
    {
        TraceError("CGraphicIndexBuffer::CreateDeviceObjects FAILED [size=%u, fmt=%u, hr=0x%08X]", m_dwBufferSize, m_d3dFmt, hr);
        return false;
    }

	return true;
}

void CGraphicIndexBuffer::DestroyDeviceObjects()
{
	safe_release(m_lpd3dIdxBuf);
}

bool CGraphicIndexBuffer::Create(int idxCount, D3DFORMAT d3dFmt)
{
	Destroy();

	m_iidxCount = idxCount;
	m_dwBufferSize = sizeof(WORD) * idxCount;
	m_d3dFmt = d3dFmt;

	return CreateDeviceObjects();
}

void CGraphicIndexBuffer::Destroy()
{
	DestroyDeviceObjects();
}

void CGraphicIndexBuffer::Initialize()
{
	m_lpd3dIdxBuf=NULL;
}

CGraphicIndexBuffer::CGraphicIndexBuffer()
{
	Initialize();
}

CGraphicIndexBuffer::~CGraphicIndexBuffer()
{
	Destroy();
}
