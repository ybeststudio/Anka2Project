#pragma once

#include "D3D12Common.h"
#include "D3D12Device.h"
#include "BindlessDescriptorHeap.h"
#include "Swapchain.h"
#include "DepthBuffer.h"
#include "Atmosphere.h"
#include "VolumetricFog.h"
#include "PostProcessing.h"

namespace RenderCore
{
	class CRenderEngine
	{
	public:
		static CRenderEngine& Instance();

		bool Initialize(HWND hWnd, uint32_t width, uint32_t height);
		void Shutdown();
		void Resize(uint32_t width, uint32_t height);

		void BeginFrame(float deltaTime, float timeOfDay);
		void RenderFrame();
		void EndFrame();

		// Camera Setup (Reverse-Z Infinite Projection)
		void SetCamera(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& up, float fovY, float aspectRatio, float nearPlane);

		// Frame State Access
		bool IsInitialized() const { return m_initialized; }
		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }
		FrameConstants& GetFrameConstants() { return m_frameConstants; }

	private:
		CRenderEngine() = default;
		~CRenderEngine();
		CRenderEngine(const CRenderEngine&) = delete;
		CRenderEngine& operator=(const CRenderEngine&) = delete;

		bool CreateFrameConstantBuffer();
		void UpdateFrameConstantBuffer();

	private:
		bool m_initialized = false;
		HWND m_hWnd = nullptr;
		uint32_t m_width = 0;
		uint32_t m_height = 0;
		uint32_t m_frameIndex = 0;
		float m_totalTime = 0.0f;

		FrameConstants m_frameConstants;
		ComPtr<D3D12MA::Allocation> m_frameConstantAlloc;
		ComPtr<ID3D12Resource> m_frameConstantBuffer;
		FrameConstants* m_pFrameConstantData = nullptr;

		// Subsystems
		std::unique_ptr<CSwapchain> m_swapchain;
		std::unique_ptr<CDepthBuffer> m_depthBuffer;
		std::unique_ptr<CAtmosphere> m_atmosphere;
		std::unique_ptr<CVolumetricFog> m_volumetricFog;
		std::unique_ptr<CPostProcessing> m_postProcessing;

		// Command Recording
		ComPtr<ID3D12CommandAllocator> m_commandAllocators[FRAME_BUFFER_COUNT];
		ComPtr<ID3D12GraphicsCommandList7> m_commandList;
	};
}
