#include "D3D12Device.h"
#include <dxgidebug.h>

// Agility SDK Exports for Windows Runtime Loader
extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 619; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = ".\\D3D12\\"; }

namespace RenderCore
{
	CD3D12Device& CD3D12Device::Instance()
	{
		static CD3D12Device s_instance;
		return s_instance;
	}

	CD3D12Device::~CD3D12Device()
	{
		Shutdown();
	}

	bool CD3D12Device::Initialize(HWND hWnd, uint32_t width, uint32_t height, bool enableDebugLayer)
	{
		if (m_initialized)
			return true;

		if (!CreateFactoryAndAdapter(enableDebugLayer))
			return false;

		if (!QueryCapabilitiesAndCreateDevice())
			return false;

		if (!CreateCommandQueues())
			return false;

		if (!CreateMemoryAllocator())
			return false;

		m_initialized = true;
		return true;
	}

	void CD3D12Device::Shutdown()
	{
		if (!m_initialized)
			return;

		FlushGPU();

		if (m_allocator)
		{
			m_allocator.Reset();
		}

		if (m_fenceEvent)
		{
			CloseHandle(m_fenceEvent);
			m_fenceEvent = nullptr;
		}

		m_directFence.Reset();
		m_computeFence.Reset();
		m_copyFence.Reset();

		m_directQueue.Reset();
		m_computeQueue.Reset();
		m_copyQueue.Reset();

		m_device.Reset();
		m_adapter.Reset();
		m_factory.Reset();

		m_initialized = false;
	}

	bool CD3D12Device::CreateFactoryAndAdapter(bool enableDebugLayer)
	{
		UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
		enableDebugLayer = true;
#endif

		if (enableDebugLayer)
		{
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
				dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

				ComPtr<ID3D12Debug1> debugController1;
				if (SUCCEEDED(debugController.As(&debugController1)))
				{
					debugController1->SetEnableGPUBasedValidation(TRUE);
					debugController1->SetEnableSynchronizedCommandQueueValidation(TRUE);
				}
			}
		}

		HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&m_factory));
		if (FAILED(hr))
		{
			::MessageBoxA(nullptr, "DXGI 1.6 Fabrikası oluşturulamadı. DirectX 12 Agility SDK çalışma ortamınızı kontrol edin.", "RenderCore Hata", MB_ICONERROR | MB_OK);
			return false;
		}

		// Enumerate High Performance discrete GPU
		ComPtr<IDXGIAdapter1> adapter1;
		for (UINT i = 0; SUCCEEDED(m_factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter1))); ++i)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter1->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			// Check D3D12 Support
			if (SUCCEEDED(D3D12CreateDevice(adapter1.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
			{
				adapter1.As(&m_adapter);
				m_adapterName = desc.Description;
				break;
			}
		}

		if (!m_adapter)
		{
			::MessageBoxA(nullptr, "DirectX 12 uyumlu fiziksel grafik işlemcisi (GPU) bulunamadı.", "RenderCore Başlatma Hatası", MB_ICONERROR | MB_OK);
			return false;
		}

		return true;
	}

	bool CD3D12Device::QueryCapabilitiesAndCreateDevice()
	{
		// Create preliminary device to query feature levels and hardware capabilities
		ComPtr<ID3D12Device> tempDevice;
		HRESULT hr = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&tempDevice));
		if (FAILED(hr))
		{
			::MessageBoxA(nullptr, "D3D12Device oluşturulamadı.", "RenderCore Hata", MB_ICONERROR | MB_OK);
			return false;
		}

		// Check Feature Level 12_2
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_12_2,
			D3D_FEATURE_LEVEL_12_1
		};

		D3D12_FEATURE_DATA_FEATURE_LEVELS featureLevelsData = {};
		featureLevelsData.NumFeatureLevels = _countof(featureLevels);
		featureLevelsData.pFeatureLevelsRequested = featureLevels;

		tempDevice->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &featureLevelsData, sizeof(featureLevelsData));
		m_caps.maxFeatureLevel = featureLevelsData.MaxSupportedFeatureLevel;

		if (m_caps.maxFeatureLevel < D3D_FEATURE_LEVEL_12_0)
		{
			std::wstring errorMsg = std::format(
				L"Grafik kartınız: {}\n"
				L"Gereksinim: DirectX 12 Uyumlu Donanım (Minimum Feature Level 12_0)\n"
				L"Mevcut Seviye: Desteklenmiyor\n\n"
				L"Oyun motoru modern D3D12 hattında çalıştığından DirectX 12 donanımı zorunludur.\n"
				L"Lütfen GPU sürücülerinizi güncelleyin.",
				m_adapterName
			);
			::MessageBoxW(nullptr, errorMsg.c_str(), L"DirectX 12 Gereksinimi", MB_ICONERROR | MB_OK);
			return false;
		}

		OutputDebugStringW(std::format(L"[RenderCore] GPU: {}, Feature Level: {}\n",
			m_adapterName,
			(m_caps.maxFeatureLevel >= D3D_FEATURE_LEVEL_12_2) ? L"12_2 (Ultimate)" :
			(m_caps.maxFeatureLevel >= D3D_FEATURE_LEVEL_12_1) ? L"12_1" : L"12_0").c_str());

		// Query Shader Model 6.6
		D3D12_FEATURE_DATA_SHADER_MODEL shaderModel = { D3D_SHADER_MODEL_6_6 };
		if (SUCCEEDED(tempDevice->CheckFeatureSupport(D3D12_FEATURE_SHADER_MODEL, &shaderModel, sizeof(shaderModel))))
		{
			m_caps.highestShaderModel = shaderModel.HighestShaderModel;
		}

		if (m_caps.highestShaderModel < D3D_SHADER_MODEL_6_6)
		{
			::MessageBoxA(nullptr, "Shader Model 6.6 bindless kaynak desteği GPU sürücünüz tarafından desteklenmiyor.", "RenderCore Hata", MB_ICONERROR | MB_OK);
			return false;
		}

		// Query Resource Binding Tier
		D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
		if (SUCCEEDED(tempDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options))))
		{
			m_caps.resourceBindingTier = options.ResourceBindingTier;
		}

		// Query Raytracing Tier (DXR 1.1 inline ray queries)
		D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5 = {};
		if (SUCCEEDED(tempDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5))))
		{
			m_caps.raytracingTier = options5.RaytracingTier;
		}

		// Query Mesh Shaders
		D3D12_FEATURE_DATA_D3D12_OPTIONS7 options7 = {};
		if (SUCCEEDED(tempDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS7, &options7, sizeof(options7))))
		{
			m_caps.meshShaderTier = options7.MeshShaderTier;
		}

		// Query Enhanced Barriers
		D3D12_FEATURE_DATA_D3D12_OPTIONS12 options12 = {};
		if (SUCCEEDED(tempDevice->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS12, &options12, sizeof(options12))))
		{
			m_caps.enhancedBarriersSupported = (options12.EnhancedBarriersSupported != FALSE);
		}

		// Create actual device with maximum feature level
		hr = D3D12CreateDevice(m_adapter.Get(), m_caps.maxFeatureLevel, IID_PPV_ARGS(&m_device));
		if (FAILED(hr))
		{
			hr = D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device));
			if (FAILED(hr))
			{
				::MessageBoxA(nullptr, "Nihai D3D12Device oluşturulamadı.", "RenderCore Hata", MB_ICONERROR | MB_OK);
				return false;
			}
		}

		return true;
	}

	bool CD3D12Device::CreateCommandQueues()
	{
		// 1. Direct Command Queue
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.NodeMask = 0;
		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_directQueue)), "Direct Command Queue oluşturulamadı");

		// 2. Compute Command Queue
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_computeQueue)), "Compute Command Queue oluşturulamadı");

		// 3. Copy Command Queue
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_copyQueue)), "Copy Command Queue oluşturulamadı");

		// Fences
		ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_directFence)), "Direct Fence oluşturulamadı");
		ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_computeFence)), "Compute Fence oluşturulamadı");
		ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_copyFence)), "Copy Fence oluşturulamadı");

		m_fenceEvent = CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS);
		if (!m_fenceEvent)
			return false;

		return true;
	}

	bool CD3D12Device::CreateMemoryAllocator()
	{
		D3D12MA::ALLOCATOR_DESC desc = {};
		desc.Flags = D3D12MA::ALLOCATOR_FLAG_DEFAULT_POOLS_NOT_ZEROED;
		desc.pDevice = m_device.Get();
		desc.pAdapter = m_adapter.Get();

		ThrowIfFailed(D3D12MA::CreateAllocator(&desc, &m_allocator), "D3D12 Memory Allocator (D3D12MA) oluşturulamadı");
		return true;
	}

	uint64_t CD3D12Device::ExecuteCommandList(ID3D12GraphicsCommandList* pCmdList, D3D12_COMMAND_LIST_TYPE type)
	{
		HRESULT hr = pCmdList->Close();
		if (FAILED(hr))
		{
			HRESULT removeReason = m_device ? m_device->GetDeviceRemovedReason() : S_OK;
			if (FAILED(removeReason))
			{
				char szErr[256];
				snprintf(szErr, sizeof(szErr), "D3D12 Device Removed: 0x%08X", removeReason);
				ThrowIfFailed(removeReason, szErr);
			}

			if (hr != E_FAIL)
			{
				ThrowIfFailed(hr, "Command List kapatılamadı");
			}
		}

		ID3D12CommandList* const ppCmdLists[] = { pCmdList };
		ID3D12CommandQueue* pQueue = nullptr;

		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			pQueue = m_directQueue.Get();
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			pQueue = m_computeQueue.Get();
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			pQueue = m_copyQueue.Get();
			break;
		default:
			pQueue = m_directQueue.Get();
			break;
		}

		pQueue->ExecuteCommandLists(1, ppCmdLists);
		return SignalFence(type);
	}

	uint64_t CD3D12Device::SignalFence(D3D12_COMMAND_LIST_TYPE type)
	{
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			++m_directFenceValue;
			m_directQueue->Signal(m_directFence.Get(), m_directFenceValue);
			return m_directFenceValue;

		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			++m_computeFenceValue;
			m_computeQueue->Signal(m_computeFence.Get(), m_computeFenceValue);
			return m_computeFenceValue;

		case D3D12_COMMAND_LIST_TYPE_COPY:
			++m_copyFenceValue;
			m_copyQueue->Signal(m_copyFence.Get(), m_copyFenceValue);
			return m_copyFenceValue;

		default:
			return 0;
		}
	}

	void CD3D12Device::WaitForFence(D3D12_COMMAND_LIST_TYPE type, uint64_t fenceValue)
	{
		ID3D12Fence* pFence = nullptr;

		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
			pFence = m_directFence.Get();
			break;
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
			pFence = m_computeFence.Get();
			break;
		case D3D12_COMMAND_LIST_TYPE_COPY:
			pFence = m_copyFence.Get();
			break;
		default:
			return;
		}

		if (pFence->GetCompletedValue() < fenceValue)
		{
			pFence->SetEventOnCompletion(fenceValue, m_fenceEvent);
			WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
		}
	}

	void CD3D12Device::FlushGPU()
	{
		uint64_t directVal = SignalFence(D3D12_COMMAND_LIST_TYPE_DIRECT);
		WaitForFence(D3D12_COMMAND_LIST_TYPE_DIRECT, directVal);

		uint64_t computeVal = SignalFence(D3D12_COMMAND_LIST_TYPE_COMPUTE);
		WaitForFence(D3D12_COMMAND_LIST_TYPE_COMPUTE, computeVal);

		uint64_t copyVal = SignalFence(D3D12_COMMAND_LIST_TYPE_COPY);
		WaitForFence(D3D12_COMMAND_LIST_TYPE_COPY, copyVal);
	}
}
