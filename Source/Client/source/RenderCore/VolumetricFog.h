#pragma once

#include "D3D12Common.h"

namespace RenderCore
{
	struct VolumetricFogConstants
	{
		XMFLOAT3 fogColor = XMFLOAT3(0.7f, 0.8f, 0.9f);
		float fogDensity = 0.02f;

		float fogHeightFalloff = 0.005f;
		float fogStartHeight = 0.0f;
		float nearPlane = 0.5f;
		float farPlane = 150.0f;

		XMFLOAT3 sunLightColor = XMFLOAT3(1.0f, 0.95f, 0.85f);
		float sunLightIntensity = 15.0f;

		XMFLOAT3 sunDirection = XMFLOAT3(0.0f, 0.7071f, 0.7071f);
		float anisotropy = 0.6f; // Henyey-Greenstein g

		uint32_t injectionUAVIndex = 0;
		uint32_t accumulatedUAVIndex = 0;
		uint32_t accumulatedSRVIndex = 0;
		uint32_t depthSRVIndex = 0;
	};

	class CVolumetricFog
	{
	public:
		CVolumetricFog() = default;
		~CVolumetricFog();

		bool Initialize();
		void Shutdown();

		void Compute(ID3D12GraphicsCommandList7* pCmdList, const XMMATRIX& invViewProj, const XMFLOAT3& cameraPos, uint32_t depthSRVIndex);
		void Composite(ID3D12GraphicsCommandList7* pCmdList);

		uint32_t GetAccumulatedSRVIndex() const { return m_accumulatedSRVIndex; }
		const VolumetricFogConstants& GetConstants() const { return m_constants; }

	private:
		bool CreateTexturesAndDescriptors();
		bool CreatePipelines();

	private:
		bool m_initialized = false;
		VolumetricFogConstants m_constants;

		// 3D Froxel Textures (160 x 90 x 64)
		ComPtr<D3D12MA::Allocation> m_injectionAlloc;
		ComPtr<ID3D12Resource> m_injectionTexture;
		uint32_t m_injectionSRVIndex = 0;
		uint32_t m_injectionUAVIndex = 0;

		ComPtr<D3D12MA::Allocation> m_accumulatedAlloc;
		ComPtr<ID3D12Resource> m_accumulatedTexture;
		uint32_t m_accumulatedSRVIndex = 0;
		uint32_t m_accumulatedUAVIndex = 0;

		// Compute PSOs
		ComPtr<ID3D12PipelineState> m_lightInjectionPSO;
		ComPtr<ID3D12PipelineState> m_volumeIntegrationPSO;
		ComPtr<ID3D12PipelineState> m_compositePSO;
	};
}
