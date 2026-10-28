#include "StdAfx.h"
#include "GrpVertexBufferStatic.h"

bool CStaticVertexBuffer::Create(int vtxCount, DWORD fvf, bool /*isManaged*/)
{
#ifdef DirectX9Ex
	return CGraphicVertexBuffer::Create(vtxCount, fvf, D3DUSAGE_WRITEONLY, D3DPOOL_DEFAULT);
#else
	return CGraphicVertexBuffer::Create(vtxCount, fvf, D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED);
#endif
}

CStaticVertexBuffer::CStaticVertexBuffer()
{
}

CStaticVertexBuffer::~CStaticVertexBuffer()
{
}
