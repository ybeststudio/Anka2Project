#pragma once

#include "GrpBase.h"

class CGraphicIndexBuffer : public CGraphicBase
{
	public:
		CGraphicIndexBuffer();
		virtual ~CGraphicIndexBuffer();

		void Destroy();
		bool Create(int idxCount, D3DFORMAT d3dFmt);
		bool Create(int faceCount, TFace* faces);

		bool CreateDeviceObjects();
		void DestroyDeviceObjects();

		bool Copy(int bufSize, const void* srcIndices);

		bool Lock(void** pretIndices) const;
		void Unlock() const;

		bool Lock(void** pretIndices);
		void Unlock();

		void SetIndices(int startIndex=0) const;

#ifdef ENABLE_DIRECTX9_UPDATE
		LPDIRECT3DINDEXBUFFER9 GetD3DIndexBuffer() const;
#else
		LPDIRECT3DINDEXBUFFER8 GetD3DIndexBuffer() const;
#endif

		int GetIndexCount() const {return m_iidxCount;}

	protected:
		void Initialize();

	protected:
#ifdef ENABLE_DIRECTX9_UPDATE
		LPDIRECT3DINDEXBUFFER9	m_lpd3dIdxBuf;
#else
		LPDIRECT3DINDEXBUFFER8	m_lpd3dIdxBuf;
#endif
		DWORD					m_dwBufferSize;
		D3DFORMAT				m_d3dFmt;
		int						m_iidxCount;
};
