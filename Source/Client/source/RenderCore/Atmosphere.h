#pragma once

#include "D3D12Common.h"

namespace RenderCore
{
	struct AtmosphereParameters
	{
		XMFLOAT3 rayleighScattering = XMFLOAT3(5.802e-6f, 13.558e-6f, 33.100e-6f);
		float rayleighScaleHeight = 8000.0f; // 8 km

		XMFLOAT3 mieScattering = XMFLOAT3(3.996e-6f, 3.996e-6f, 3.996e-6f);
		float mieScaleHeight = 1200.0f; // 1.2 km

		XMFLOAT3 mieExtinction = XMFLOAT3(4.440e-6f, 4.440e-6f, 4.440e-6f);
		float mieAssymetry = 0.8f; // Phase g

		XMFLOAT3 ozoneAbsorption = XMFLOAT3(0.650e-6f, 1.881e-6f, 0.085e-6f);
		float bottomRadius = 6360000.0f; // 6360 km

		float topRadius = 6460000.0f; // 6460 km
		float timeOfDay = 12.0f; // 0..24
		XMFLOAT2 padding0;

		XMFLOAT3 sunDirection = XMFLOAT3(0.0f, 0.7071f, 0.7071f);
		float sunIntensity = 20.0f;

		XMFLOAT3 moonDirection = XMFLOAT3(0.0f, -0.7071f, -0.7071f);
		float moonIntensity = 0.2f;

		XMFLOAT3 groundAlbedo = XMFLOAT3(0.1f, 0.1f, 0.1f);
		float multipleScatteringFactor = 1.0f;
	};

	class CAtmosphere
	{
	public:
		CAtmosphere() = default;
		~CAtmosphere();

		bool Initialize();
		void Shutdown();

		void Update(float deltaTime, float timeOfDay);
		void RenderLUTs(ID3D12GraphicsCommandList7* pCmdList);
		void RenderSky(ID3D12GraphicsCommandList7* pCmdList, const XMMATRIX& invViewProj, const XMFLOAT3& cameraPos);

		uint32_t GetTransmittanceLUTIndex() const { return m_transmittanceLUTIndex; }
		uint32_t GetMultiScatterLUTIndex() const { return m_multiScatterLUTIndex; }
		uint32_t GetSkyViewLUTIndex() const { return m_skyViewLUTIndex; }
		const AtmosphereParameters& GetParameters() const { return m_params; }

	private:
		bool CreateTexturesAndDescriptors();
		bool CreatePipelines();

	private:
		bool m_initialized = false;
		AtmosphereParameters m_params;

		// Textures
		ComPtr<D3D12MA::Allocation> m_transmittanceAlloc;
		ComPtr<ID3D12Resource> m_transmittanceLUT;
		uint32_t m_transmittanceLUTIndex = 0;
		uint32_t m_transmittanceUAVIndex = 0;

		ComPtr<D3D12MA::Allocation> m_multiScatterAlloc;
		ComPtr<ID3D12Resource> m_multiScatterLUT;
		uint32_t m_multiScatterLUTIndex = 0;
		uint32_t m_multiScatterUAVIndex = 0;

		ComPtr<D3D12MA::Allocation> m_skyViewAlloc;
		ComPtr<ID3D12Resource> m_skyViewLUT;
		uint32_t m_skyViewLUTIndex = 0;
		uint32_t m_skyViewUAVIndex = 0;

		// Compute PSOs
		ComPtr<ID3D12PipelineState> m_transmittancePSO;
		ComPtr<ID3D12PipelineState> m_multiScatterPSO;
		ComPtr<ID3D12PipelineState> m_skyViewPSO;
		ComPtr<ID3D12PipelineState> m_renderSkyPSO;
	};
}
