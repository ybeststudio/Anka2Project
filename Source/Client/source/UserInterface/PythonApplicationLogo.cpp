#include "StdAfx.h"
#include "PythonApplication.h"

static bool bInitializedLogo = false;

int CPythonApplication::OnLogoOpen(char *szName)
{
	m_pLogoTex = nullptr;
	m_pLogoTex = new CGraphicImageTexture();
	m_pCaptureBuffer = nullptr;
	m_lBufferSize = 0;
	m_bLogoError = true;
	m_bLogoPlay = false;

	m_pGraphBuilder = nullptr;
	m_pFilterSG = nullptr;
	m_pSampleGrabber = nullptr;
	m_pMediaCtrl = nullptr;
	m_pMediaEvent = nullptr;
	m_pVideoWnd = nullptr;
	m_pBasicVideo = nullptr;

	m_nLeft = 0; m_nRight = 0; m_nTop = 0; m_nBottom = 0;


	// 처음에는 1/1 크기의 텍스쳐를 �?성해�?다.
	if(!m_pLogoTex->Create(1, 1, D3DFMT_A8R8G8B8)) { return 0; }

	// Set GraphBuilder / SampleGrabber
	if(FAILED(CoCreateInstance(CLSID_FilterGraph, nullptr, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (VOID**)(&m_pGraphBuilder)))) { return 0; }
	if(FAILED(CoCreateInstance(CLSID_SampleGrabber, nullptr, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (VOID**)&m_pFilterSG))) { return 0; }
	if(FAILED(m_pGraphBuilder->AddFilter(m_pFilterSG, L"SampleGrabber"))) { return 0; }

	// Create Media Type
	AM_MEDIA_TYPE mediaType;
	ZeroMemory(&mediaType, sizeof(mediaType));
	mediaType.majortype = MEDIATYPE_Video;
	mediaType.subtype = MEDIASUBTYPE_RGB32;
	if(FAILED(m_pFilterSG->QueryInterface(IID_ISampleGrabber, (VOID**) &m_pSampleGrabber))) { return 0; }
	if(FAILED(m_pSampleGrabber->SetMediaType( &mediaType))) { return 0; }

	// Render File
	WCHAR wFileName[ MAX_PATH ];
	MultiByteToWideChar(CP_ACP, 0, szName, -1, wFileName, MAX_PATH);
	if(FAILED(m_pGraphBuilder->RenderFile(wFileName, nullptr))) { return 0; }

	IBaseFilter* pSrc;
	m_pGraphBuilder->AddSourceFilter(wFileName, L"Source", &pSrc);

	// Media Control
	if(FAILED(m_pGraphBuilder->QueryInterface(IID_IMediaControl, (VOID**) &m_pMediaCtrl))) { return 0; }

	// Video Window
	if(FAILED(m_pGraphBuilder->QueryInterface(IID_IVideoWindow, (VOID**) &m_pVideoWnd))) { return 0; }
	if(FAILED(m_pVideoWnd->put_MessageDrain((OAHWND)this->m_hWnd))) { return 0; }

	// Basic Video
	if(FAILED(m_pGraphBuilder->QueryInterface(IID_IBasicVideo, (VOID**)&m_pBasicVideo))) { return 0; }

	// Media Event
	if(FAILED(m_pGraphBuilder->QueryInterface(IID_IMediaEventEx, (VOID**) &m_pMediaEvent))) { return 0; }

	// Window 안보이게
	m_pVideoWnd->SetWindowPosition( 3000, 3000, 0, 0 );
	m_pVideoWnd->put_Visible(0);
	m_pSampleGrabber->SetBufferSamples(TRUE);

	m_pVideoWnd->put_Owner((OAHWND)m_hWnd);
	m_pMediaEvent->SetNotifyWindow((OAHWND)m_hWnd, WM_APP + 1, 0);

	bInitializedLogo = true;
	
	return 1;
}

int CPythonApplication::OnLogoUpdate()
{
	//OSVERSIONINFO osvi;
	//ZeroMemory(&osvi, sizeof(osvi));
	//osvi.dwOSVersionInfoSize = sizeof(osvi);
	//GetVersionEx(&osvi);

	//// windows xp 이하인 버전은 logo skip.
	////	m_pSampleGrabber->GetCurrentBuffer(&m_lBufferSize,  (LONG*)m_pCaptureBuffer) fail 나기 때문.
	//if (osvi.dwMajorVersion <= 5)
	//{
	//	return 0;
	//}

	if(m_pGraphBuilder == NULL || m_pFilterSG == NULL || m_pSampleGrabber == NULL || m_pMediaCtrl == NULL || m_pMediaEvent == NULL || m_pVideoWnd == NULL || false == bInitializedLogo)
	{
		return 0;
	}

	BYTE* pBuffer = m_pCaptureBuffer; LONG lBufferSize = m_lBufferSize;

	if(!m_bLogoPlay) { m_pMediaCtrl->Run(); m_bLogoPlay = true; }

	if( lBufferSize == 0  ) {
		m_pSampleGrabber->GetCurrentBuffer(&m_lBufferSize, nullptr);

		SAFE_DELETE_ARRAY(m_pCaptureBuffer);
		m_pCaptureBuffer = new BYTE[ m_lBufferSize ];
		pBuffer = m_pCaptureBuffer;
		lBufferSize = m_lBufferSize;
	}

	if(FAILED(m_pSampleGrabber->GetCurrentBuffer(&m_lBufferSize,  (LONG*)m_pCaptureBuffer)))
	{
		m_bLogoError = true;

#ifdef ENABLE_DIRECTX9_UPDATE
		LPDIRECT3DTEXTURE9 tex = m_pLogoTex->GetD3DTexture();
#else
		LPDIRECT3DTEXTURE8 tex = m_pLogoTex->GetD3DTexture();
#endif
		D3DLOCKED_RECT rt;
		ZeroMemory(&rt, sizeof(rt));

		tex->LockRect(0, &rt, 0, 0);
		BYTE* destb = static_cast<unsigned char *>(rt.pBits);
		for(int a = 0; a < 4; a+= 4)
		{
			BYTE* dest = &destb[a];
			dest[0] = 0; dest[1] = 0; dest[2] = 0; dest[3] = 0xff;
		}
		tex->UnlockRect(0);

		return 1;
	}

	m_bLogoError = false;

	long lWidth, lHeight;
	m_pBasicVideo->GetVideoSize(&lWidth, &lHeight);
	
	if(lWidth >= lHeight)
	{
		m_nLeft = 0; m_nRight = this->GetWidth();
		m_nTop = (this->GetHeight() >> 1) - ((this->GetWidth() * lHeight / lWidth) >> 1);
		m_nBottom = (this->GetHeight() >> 1) + ((this->GetWidth() * lHeight / lWidth) >> 1);
	}
	else
	{
		m_nTop = 0; m_nBottom = this->GetHeight();
		m_nLeft = (this->GetWidth() >> 1) - ((this->GetHeight() * lWidth / lHeight) >> 1);
		m_nRight = (this->GetWidth() >> 1) - ((this->GetHeight() * lWidth / lHeight) >> 1);
	}

	if(m_pLogoTex->GetWidth() == 1)
	{
		m_pLogoTex->Destroy(); m_pLogoTex->Create(lWidth, lHeight, D3DFMT_A8R8G8B8);
		
	}

#ifdef ENABLE_DIRECTX9_UPDATE
	LPDIRECT3DTEXTURE9 tex = m_pLogoTex->GetD3DTexture();
#else
	LPDIRECT3DTEXTURE8 tex = m_pLogoTex->GetD3DTexture();
#endif
	D3DLOCKED_RECT rt;
	ZeroMemory(&rt, sizeof(rt));

	tex->LockRect(0, &rt, 0, 0);
	BYTE* destb = static_cast<unsigned char *>(rt.pBits);
	for(int a = 0; a < lBufferSize; a+= 4)
	{
		BYTE* src = &m_pCaptureBuffer[a]; BYTE* dest = &destb[a];
		dest[0] = src[0]; dest[1] = src[1]; dest[2] = src[2]; dest[3] = 0xff;
	}
	tex->UnlockRect(0);

	long evCode;
	LONG_PTR param1, param2;
	while(SUCCEEDED(m_pMediaEvent->GetEvent(&evCode, &param1, &param2, 0)))
	{
		switch(evCode)
		{
		case EC_COMPLETE:
			return 0;
		case EC_USERABORT:
			return 0;
		case EC_ERRORABORT:
			return 0;
		}

		m_pMediaEvent->FreeEventParams(evCode, param1, param2);
	}

	if(GetAsyncKeyState(VK_ESCAPE) & 0x8000) { return 0; }

	return 1;
}

void CPythonApplication::OnLogoRender()
{
	if(!m_pLogoTex->IsEmpty() && !m_bLogoError && true == bInitializedLogo)
	{
#ifdef ENABLE_DIRECTX9_UPDATE
		STATEMANAGER.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
#else
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
		STATEMANAGER.SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
#endif
		m_pLogoTex->SetTextureStage(0);
		CPythonGraphic::instance().RenderTextureBox(m_nLeft, m_nTop, m_nRight, m_nBottom, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	}
}

void CPythonApplication::OnLogoClose()
{
	if (false == bInitializedLogo)
		return;

	if(m_pCaptureBuffer != nullptr)
	{
		delete[] m_pCaptureBuffer;
		m_pCaptureBuffer = nullptr;
	}
	if(m_pLogoTex != nullptr)
	{
		m_pLogoTex->Destroy();
		delete m_pLogoTex;
		m_pLogoTex = nullptr;
	}

	if(m_pMediaEvent != nullptr)
	{
		m_pMediaEvent->SetNotifyWindow(NULL, 0, 0);
		m_pMediaEvent->Release();
		m_pMediaEvent = nullptr;
	}
	if(m_pBasicVideo != nullptr) m_pBasicVideo->Release(); m_pBasicVideo = nullptr;
	if(m_pVideoWnd != nullptr) m_pVideoWnd->Release(); m_pVideoWnd = nullptr;
	if(m_pMediaCtrl != nullptr) m_pMediaCtrl->Release(); m_pMediaCtrl = nullptr;
	if(m_pSampleGrabber != nullptr) m_pSampleGrabber->Release(); m_pSampleGrabber = nullptr;
	if(m_pFilterSG != nullptr) m_pFilterSG->Release(); m_pFilterSG = nullptr;
	if(m_pGraphBuilder != nullptr) m_pGraphBuilder->Release(); m_pGraphBuilder = nullptr;

#ifdef ENABLE_DIRECTX9_UPDATE
	STATEMANAGER.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	STATEMANAGER.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
#else
	STATEMANAGER.SetTextureStageState(0, D3DTSS_MINFILTER, D3DTEXF_POINT);
	STATEMANAGER.SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTEXF_POINT);
#endif
}