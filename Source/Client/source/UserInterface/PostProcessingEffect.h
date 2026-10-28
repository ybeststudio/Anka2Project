#pragma once
#include "StdAfx.h"
#ifndef DISABLE_POST_PROCESSING
#include <string>
#include <xmllite.h>
#include <shlwapi.h>

class CPostProcessingEffect
{
public:
    CPostProcessingEffect();
    CPostProcessingEffect(const CPostProcessingEffect& c_kOld);
    CPostProcessingEffect(CPostProcessingEffect&& r_kOld);
    ~CPostProcessingEffect();

    CPostProcessingEffect& operator=(const CPostProcessingEffect& c_kOld);
    CPostProcessingEffect& operator=(CPostProcessingEffect&& r_kOld);

    bool operator==(const char* c_pszName) const;
    bool operator==(const CPostProcessingEffect& c_kOld) const;

    void SetName(const char* c_pszName);
    const char* GetName() const;

    bool CreateShader(CMappedFile& rkDescriptor);
    void SetShader(IDirect3DPixelShader9* lpShader, LPD3DXCONSTANTTABLE lpConstants);
    IDirect3DPixelShader9* GetShader();
    LPD3DXCONSTANTTABLE GetConstants();

    bool Apply();

private:
    std::string m_strName;
    std::string m_shaderSource;
    std::string m_entryPoint;
    IDirect3DPixelShader9* m_pPixelShader;
    IDirect3DVertexShader9* m_pVertexShader;
    LPD3DXCONSTANTTABLE m_pConstants;
};

#endif
