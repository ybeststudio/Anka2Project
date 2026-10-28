#pragma once

#include "D3D12Common.h"

namespace RenderCore
{
	class CSwapchain
	{
	public:
		CSwapchain() = default;
		~CSwapchain();

		bool Initialize(HWND hWnd, uint32_t width, uint32_t height, bool enableHDR = false);
		void Shutdown();
		void Resize(uint32_t width, uint32_t height);

		void BeginFrame(ID3D12GraphicsCommandList7* pCmdList);
		void EndFrame(ID3D12GraphicsCommandList7* pCmdList);
		void Present(bool vsync = true);

		// Getters
		ID3D12Resource* GetCurrentBackBuffer() const { return m_backBuffers[m_currentBackBufferIndex].Get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const;
		uint32_t GetCurrentIndex() const { return m_currentBackBufferIndex; }
		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }
		DXGI_FORMAT GetFormat() const { return m_format; }
		bool IsHDREnabled() const { return m_isHDR; }

	private:
		bool CreateRTVHeap();
		bool CreateBackBuffers();
		void CheckHDRSupport();

	private:
		HWND m_hWnd = nullptr;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		bool m_isHDR = false;
		DXGI_FORMAT m_format = BACKBUFFER_FORMAT_SDR;

		ComPtr<IDXGISwapChain4> m_swapchain;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		uint32_t m_rtvDescriptorSize = 0;

		ComPtr<ID3D12Resource> m_backBuffers[FRAME_BUFFER_COUNT];
		uint32_t m_currentBackBufferIndex = 0;
	};
}
