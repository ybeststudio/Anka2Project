#pragma once

#include "D3D12Common.h"

namespace RenderCore
{
	struct PostProcessConstants
	{
		float exposure = 1.0f;
		float paperWhiteNits = 200.0f;
		float maxLuminanceNits = 1000.0f;
		uint32_t isHDR10 = 0;

		uint32_t sceneColorSRVIndex = 0;
		float bloomIntensity = 0.05f;
		float bloomThreshold = 1.5f;
		uint32_t pad = 0;
	};

	class CPostProcessing
	{
	public:
		CPostProcessing() = default;
		~CPostProcessing();

		bool Initialize(uint32_t width, uint32_t height);
		void Shutdown();
		void Resize(uint32_t width, uint32_t height);

		void Render(ID3D12GraphicsCommandList7* pCmdList,
			D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTV,
			DXGI_FORMAT backBufferFormat,
			bool isHDR10);

		// Scene Color Buffer access
		ID3D12Resource* GetSceneColorResource() const { return m_sceneColorTexture.Get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE GetSceneColorRTV() const { return m_sceneColorRTV; }
		uint32_t GetSceneColorSRVIndex() const { return m_sceneColorSRVIndex; }

		PostProcessConstants& GetConstants() { return m_constants; }

	private:
		bool CreateSceneColorTarget(uint32_t width, uint32_t height);
		bool CreatePipelines();

	private:
		bool m_initialized = false;
		uint32_t m_width = 0;
		uint32_t m_height = 0;

		PostProcessConstants m_constants;

		// Scene HDR Render Target (R11G11B10_FLOAT)
		ComPtr<D3D12MA::Allocation> m_sceneColorAlloc;
		ComPtr<ID3D12Resource> m_sceneColorTexture;
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
		D3D12_CPU_DESCRIPTOR_HANDLE m_sceneColorRTV = {};
		uint32_t m_sceneColorSRVIndex = 0;

		// Tonemapping Graphics PSOs
		ComPtr<ID3D12PipelineState> m_tonemapSDRPSO;
		ComPtr<ID3D12PipelineState> m_tonemapHDR10PSO;
	};
}
