#pragma once

#include "D3D12Common.h"

namespace RenderCore
{
	constexpr uint32_t CLUSTER_GRID_X = 16;
	constexpr uint32_t CLUSTER_GRID_Y = 9;
	constexpr uint32_t CLUSTER_GRID_Z = 24;
	constexpr uint32_t TOTAL_CLUSTERS = CLUSTER_GRID_X * CLUSTER_GRID_Y * CLUSTER_GRID_Z; // 3456 clusters
	constexpr uint32_t MAX_LOCAL_LIGHTS = 128; // Up to 128 dynamic local point/spot lights

	struct LocalPointLight
	{
		XMFLOAT3 position;
		float radius;

		XMFLOAT3 color;
		float intensity;
	};

	struct ClusterAABB
	{
		XMFLOAT4 minPoint; // xyz = min, w = unused
		XMFLOAT4 maxPoint; // xyz = max, w = unused
	};

	struct ClusteredLightingConstants
	{
		uint32_t clusterGridX = CLUSTER_GRID_X;
		uint32_t clusterGridY = CLUSTER_GRID_Y;
		uint32_t clusterGridZ = CLUSTER_GRID_Z;
		uint32_t activeLightCount = 0;

		float nearPlane = 0.5f;
		float farPlane = 150.0f;
		uint32_t lightBufferSRV = 0;
		uint32_t clusterMaskBufferUAV = 0;

		uint32_t clusterMaskBufferSRV = 0;
		uint32_t clusterAABBBufferSRV = 0;
		uint32_t clusterAABBBufferUAV = 0;
		uint32_t pad = 0;
	};

	class CClusteredLighting
	{
	public:
		static CClusteredLighting& Instance();

		bool Initialize();
		void Shutdown();

		void ResetLights();
		void AddPointLight(const XMFLOAT3& position, float radius, const XMFLOAT3& color, float intensity);

		void UpdateAndCull(ID3D12GraphicsCommandList7* pCmdList, const XMMATRIX& viewMatrix, const XMMATRIX& projMatrix);

		uint32_t GetClusterMaskBufferSRV() const { return m_clusterMaskSRV; }
		uint32_t GetLightBufferSRV() const { return m_lightBufferSRV; }
		const ClusteredLightingConstants& GetConstants() const { return m_constants; }

	private:
		CClusteredLighting() = default;
		~CClusteredLighting();
		CClusteredLighting(const CClusteredLighting&) = delete;
		CClusteredLighting& operator=(const CClusteredLighting&) = delete;

		bool CreateBuffers();
		bool CreatePipelines();

	private:
		bool m_initialized = false;
		ClusteredLightingConstants m_constants;

		// Lights buffer
		std::vector<LocalPointLight> m_lights;
		ComPtr<D3D12MA::Allocation> m_lightUploadAlloc;
		ComPtr<ID3D12Resource> m_lightUploadBuffer;
		LocalPointLight* m_pLightUploadData = nullptr;
		uint32_t m_lightBufferSRV = 0;

		// Cluster AABB Buffer (View-space AABBs)
		ComPtr<D3D12MA::Allocation> m_clusterAABBAlloc;
		ComPtr<ID3D12Resource> m_clusterAABBBuffer;
		uint32_t m_clusterAABBSRV = 0;
		uint32_t m_clusterAABBUAV = 0;

		// Cluster Light Bitmask Buffer (4x uint per cluster = 128 bits)
		ComPtr<D3D12MA::Allocation> m_clusterMaskAlloc;
		ComPtr<ID3D12Resource> m_clusterMaskBuffer;
		uint32_t m_clusterMaskSRV = 0;
		uint32_t m_clusterMaskUAV = 0;

		// Compute PSOs
		ComPtr<ID3D12PipelineState> m_buildAABBsPSO;
		ComPtr<ID3D12PipelineState> m_cullLightsPSO;
	};
}
