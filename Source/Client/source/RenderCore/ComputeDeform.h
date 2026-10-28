#pragma once

#include "D3D12Common.h"

namespace RenderCore
{
	struct SkinVertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT4 tangent;
		XMFLOAT2 uv;
		uint32_t boneIndices[4];
		float boneWeights[4];
	};

	struct DeformedVertex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT4 tangent;
		XMFLOAT2 uv;
	};

	struct SkinningDispatchParams
	{
		uint32_t vertexCount = 0;
		uint32_t inputVertexSRV = 0;
		uint32_t boneMatrixSRV = 0;
		uint32_t outputVertexUAV = 0;
	};

	struct VegetationDispatchParams
	{
		uint32_t vertexCount = 0;
		uint32_t inputVertexSRV = 0;
		uint32_t outputVertexUAV = 0;
		float time = 0.0f;

		XMFLOAT3 windDirection = XMFLOAT3(1.0f, 0.0f, 0.0f);
		float windSpeed = 1.5f;

		float windStrength = 0.3f;
		float branchAmplitude = 0.2f;
		float leafFrequency = 4.0f;
		uint32_t pad = 0;
	};

	class CComputeDeform
	{
	public:
		static CComputeDeform& Instance();

		bool Initialize();
		void Shutdown();

		// Character / Monster Compute Skinning
		void DispatchSkinning(
			ID3D12GraphicsCommandList7* pCmdList,
			uint32_t vertexCount,
			uint32_t inputVertexSRV,
			uint32_t boneMatrixSRV,
			uint32_t outputVertexUAV,
			ID3D12Resource* pOutputResource);

		// Sousa Vegetation Wind Sway Compute
		void DispatchVegetationSway(
			ID3D12GraphicsCommandList7* pCmdList,
			uint32_t vertexCount,
			uint32_t inputVertexSRV,
			uint32_t outputVertexUAV,
			ID3D12Resource* pOutputResource,
			float time,
			const XMFLOAT3& windDir,
			float windSpeed,
			float windStrength);

	private:
		CComputeDeform() = default;
		~CComputeDeform();
		CComputeDeform(const CComputeDeform&) = delete;
		CComputeDeform& operator=(const CComputeDeform&) = delete;

		bool CreatePipelines();

	private:
		bool m_initialized = false;
		ComPtr<ID3D12PipelineState> m_skinningPSO;
		ComPtr<ID3D12PipelineState> m_vegetationSwayPSO;
	};
}
