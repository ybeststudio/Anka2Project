#include "Swapchain.h"
#include "D3D12Device.h"

namespace RenderCore
{
	CSwapchain::~CSwapchain()
	{
		Shutdown();
	}

	bool CSwapchain::Initialize(HWND hWnd, uint32_t width, uint32_t height, bool enableHDR)
	{
		m_hWnd = hWnd;
		m_width = width;
		m_height = height;

		if (enableHDR)
		{
			CheckHDRSupport();
		}
		else
		{
			m_format = BACKBUFFER_FORMAT_SDR;
			m_isHDR = false;
		}

		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto pFactory = CD3D12Device::Instance().GetFactory();
		auto pQueue = CD3D12Device::Instance().GetDirectQueue();

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = m_width;
		desc.Height = m_height;
		desc.Format = m_format;
		desc.Stereo = FALSE;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = FRAME_BUFFER_COUNT;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

		ComPtr<IDXGISwapChain1> swapchain1;
		ThrowIfFailed(pFactory->CreateSwapChainForHwnd(pQueue, m_hWnd, &desc, nullptr, nullptr, &swapchain1),
			"D3D12 Swapchain oluşturulamadı");

		pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
		ThrowIfFailed(swapchain1.As(&m_swapchain), "IDXGISwapChain4 arabirimi alınamadı");

		if (m_isHDR)
		{
			m_swapchain->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020);
		}

		m_currentBackBufferIndex = m_swapchain->GetCurrentBackBufferIndex();

		if (!CreateRTVHeap())
			return false;

		if (!CreateBackBuffers())
			return false;

		return true;
	}

	void CSwapchain::Shutdown()
	{
		for (uint32_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			m_backBuffers[i].Reset();
		}

		m_rtvHeap.Reset();
		m_swapchain.Reset();
	}

	void CSwapchain::CheckHDRSupport()
	{
		ComPtr<IDXGIOutput> output;
		if (SUCCEEDED(m_swapchain ? m_swapchain->GetContainingOutput(&output) : E_FAIL))
		{
			ComPtr<IDXGIOutput6> output6;
			if (SUCCEEDED(output.As(&output6)))
			{
				DXGI_OUTPUT_DESC1 desc1;
				if (SUCCEEDED(output6->GetDesc1(&desc1)))
				{
					if (desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)
					{
						m_format = BACKBUFFER_FORMAT_HDR;
						m_isHDR = true;
						return;
					}
				}
			}
		}

		m_format = BACKBUFFER_FORMAT_SDR;
		m_isHDR = false;
	}

	bool CSwapchain::CreateRTVHeap()
	{
		auto pDevice = CD3D12Device::Instance().GetDevice();

		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FRAME_BUFFER_COUNT;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		ThrowIfFailed(pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)), "RTV Descriptor Heap oluşturulamadı");
		m_rtvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		return true;
	}

	bool CSwapchain::CreateBackBuffers()
	{
		auto pDevice = CD3D12Device::Instance().GetDevice();
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		for (uint32_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			ThrowIfFailed(m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i])), "Swapchain BackBuffer alınamadı");
			pDevice->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}

		return true;
	}

	void CSwapchain::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		CD3D12Device::Instance().FlushGPU();

		m_width = width;
		m_height = height;

		for (uint32_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			m_backBuffers[i].Reset();
		}

		ThrowIfFailed(m_swapchain->ResizeBuffers(FRAME_BUFFER_COUNT, m_width, m_height, m_format, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING),
			"Swapchain ResizeBuffers başarısız oldu");

		m_currentBackBufferIndex = m_swapchain->GetCurrentBackBufferIndex();
		CreateBackBuffers();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CSwapchain::GetCurrentRTV() const
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(m_currentBackBufferIndex, m_rtvDescriptorSize);
		return handle;
	}

	void CSwapchain::BeginFrame(ID3D12GraphicsCommandList7* pCmdList)
	{
		m_currentBackBufferIndex = m_swapchain->GetCurrentBackBufferIndex();

		// Enhanced Barrier transition: Present -> RenderTarget
		D3D12_TEXTURE_BARRIER barrier = {};
		barrier.SyncBefore = D3D12_BARRIER_SYNC_NONE;
		barrier.SyncAfter = D3D12_BARRIER_SYNC_RENDER_TARGET;
		barrier.AccessBefore = D3D12_BARRIER_ACCESS_NO_ACCESS;
		barrier.AccessAfter = D3D12_BARRIER_ACCESS_RENDER_TARGET;
		barrier.LayoutBefore = D3D12_BARRIER_LAYOUT_PRESENT;
		barrier.LayoutAfter = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
		barrier.pResource = m_backBuffers[m_currentBackBufferIndex].Get();
		barrier.Subresources.IndexOrFirstMipLevel = 0;
		barrier.Subresources.NumMipLevels = 1;
		barrier.Subresources.FirstArraySlice = 0;
		barrier.Subresources.NumArraySlices = 1;
		barrier.Subresources.FirstPlane = 0;
		barrier.Subresources.NumPlanes = 1;
		barrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

		D3D12_BARRIER_GROUP group = {};
		group.Type = D3D12_BARRIER_TYPE_TEXTURE;
		group.NumBarriers = 1;
		group.pTextureBarriers = &barrier;

		pCmdList->Barrier(1, &group);
	}

	void CSwapchain::EndFrame(ID3D12GraphicsCommandList7* pCmdList)
	{
		// Enhanced Barrier transition: RenderTarget -> Present
		D3D12_TEXTURE_BARRIER barrier = {};
		barrier.SyncBefore = D3D12_BARRIER_SYNC_RENDER_TARGET;
		barrier.SyncAfter = D3D12_BARRIER_SYNC_NONE;
		barrier.AccessBefore = D3D12_BARRIER_ACCESS_RENDER_TARGET;
		barrier.AccessAfter = D3D12_BARRIER_ACCESS_NO_ACCESS;
		barrier.LayoutBefore = D3D12_BARRIER_LAYOUT_RENDER_TARGET;
		barrier.LayoutAfter = D3D12_BARRIER_LAYOUT_PRESENT;
		barrier.pResource = m_backBuffers[m_currentBackBufferIndex].Get();
		barrier.Subresources.IndexOrFirstMipLevel = 0;
		barrier.Subresources.NumMipLevels = 1;
		barrier.Subresources.FirstArraySlice = 0;
		barrier.Subresources.NumArraySlices = 1;
		barrier.Subresources.FirstPlane = 0;
		barrier.Subresources.NumPlanes = 1;
		barrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

		D3D12_BARRIER_GROUP group = {};
		group.Type = D3D12_BARRIER_TYPE_TEXTURE;
		group.NumBarriers = 1;
		group.pTextureBarriers = &barrier;

		pCmdList->Barrier(1, &group);
	}

	void CSwapchain::Present(bool vsync)
	{
		HRESULT hr = m_swapchain->Present(vsync ? 1 : 0, 0);
		if (FAILED(hr))
		{
			if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
			{
				HRESULT reason = CD3D12Device::Instance().GetDevice()->GetDeviceRemovedReason();
				ThrowIfFailed(reason, "GPU Aygıtı Çıkarıldı (Device Removed)!");
			}
		}
	}
}
