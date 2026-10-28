#include "StdAfx.h"
#include "GrpPixelShader.h"
#include "GrpD3DXBuffer.h"
#include "StateManager.h"

CPixelShader::CPixelShader()
{
    Initialize();
}

CPixelShader::~CPixelShader()
{
    Destroy();
}

void CPixelShader::Initialize()
{
    m_handle = 0;
}

void CPixelShader::Destroy()
{
#ifdef ENABLE_DIRECTX9_UPDATE
    if (m_handle)
    {
        m_handle = 0;
    }
#else
    if (m_handle)
    {
        if (ms_lpd3dDevice)
        {
            ms_lpd3dDevice->DeletePixelShader(m_handle);
        }

        m_handle = 0;
    }
#endif
}

bool CPixelShader::CreateFromDiskFile(const char* c_szFileName)
{
    Destroy();

#ifdef ENABLE_DIRECTX9_UPDATE
    return false;
#else
    LPD3DXBUFFER lpd3dxShaderBuffer;
    LPD3DXBUFFER lpd3dxErrorBuffer;
    if (FAILED(D3DXAssembleShaderFromFile(c_szFileName
                                            , 0
                                            , NULL
                                            , &lpd3dxShaderBuffer
                                            , &lpd3dxErrorBuffer)))
    {
        return false;
    }

    CDirect3DXBuffer shaderBuffer(lpd3dxShaderBuffer);
    CDirect3DXBuffer errorBuffer(lpd3dxErrorBuffer);

    if (FAILED(ms_lpd3dDevice->CreatePixelShader((DWORD*)shaderBuffer.GetPointer(), &m_handle)))
    {
        return false;
    }

    return true;
#endif
}

void CPixelShader::Set()
{
#ifndef ENABLE_DIRECTX9_UPDATE
    STATEMANAGER.SetPixelShader(m_handle);
#endif
}
