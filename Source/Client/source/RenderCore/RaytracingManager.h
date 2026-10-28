#pragma once

#include "D3D12Common.h"

namespace RenderCore
{
	struct RaytracingGeometryDesc
	{
		D3D12_GPU_VIRTUAL_ADDRESS vertexBufferAddress = 0;
		uint32_t vertexCount = 0;
		uint32_t vertexStride = sizeof(XMFLOAT3);
		DXGI_FORMAT vertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;

		D3D12_GPU_VIRTUAL_ADDRESS indexBufferAddress = 0;
		uint32_t indexCount = 0;
		DXGI_FORMAT indexFormat = DXGI_FORMAT_R32_UINT;

		D3D12_RAYTRACING_GEOMETRY_FLAGS flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;
	};

	struct BLASResource
	{
		ComPtr<D3D12MA::Allocation> allocation;
		ComPtr<ID3D12Resource> resource;
		ComPtr<D3D12MA::Allocation> scratchAlloc;
		ComPtr<ID3D12Resource> scratchBuffer;
		uint64_t resultSize = 0;
		uint64_t scratchSize = 0;
		bool isDynamic = false;
		bool isBuilt = false;
	};

	struct TLASInstance
	{
		XMMATRIX transform = XMMatrixIdentity();
		uint32_t instanceID = 0;
		uint32_t instanceMask = 0xFF;
		uint32_t instanceContributionToHitGroupIndex = 0;
		uint32_t flags = D3D12_RAYTRACING_INSTANCE_FLAG_NONE;
		BLASResource* pBLAS = nullptr;
	};

	class CRaytracingManager
	{
	public:
		static CRaytracingManager& Instance();

		bool Initialize();
		void Shutdown();

		bool IsRaytracingSupported() const { return m_isSupported; }

		// BLAS creation
		std::shared_ptr<BLASResource> BuildStaticBLAS(ID3D12GraphicsCommandList7* pCmdList, const std::vector<RaytracingGeometryDesc>& geometries);
		std::shared_ptr<BLASResource> BuildDynamicBLAS(ID3D12GraphicsCommandList7* pCmdList, const std::vector<RaytracingGeometryDesc>& geometries);
		void RefitDynamicBLAS(ID3D12GraphicsCommandList7* pCmdList, BLASResource* pBLAS, const std::vector<RaytracingGeometryDesc>& geometries);

		// TLAS management
		void ResetInstances();
		void AddInstance(const TLASInstance& instance);
		void BuildTLAS(ID3D12GraphicsCommandList7* pCmdList);

		uint32_t GetTLASDescriptorIndex() const { return m_tlasDescriptorIndex; }
		D3D12_GPU_VIRTUAL_ADDRESS GetTLASAddress() const { return m_tlasResource ? m_tlasResource->GetGPUVirtualAddress() : 0; }

	private:
		CRaytracingManager() = default;
		~CRaytracingManager();
		CRaytracingManager(const CRaytracingManager&) = delete;
		CRaytracingManager& operator=(const CRaytracingManager&) = delete;

	private:
		bool m_initialized = false;
		bool m_isSupported = false;

		// TLAS Resources
		static constexpr uint32_t MAX_TLAS_INSTANCES = 16384;
		ComPtr<D3D12MA::Allocation> m_tlasAlloc;
		ComPtr<ID3D12Resource> m_tlasResource;

		ComPtr<D3D12MA::Allocation> m_tlasScratchAlloc;
		ComPtr<ID3D12Resource> m_tlasScratchResource;

		ComPtr<D3D12MA::Allocation> m_instanceUploadAlloc;
		ComPtr<ID3D12Resource> m_instanceUploadBuffer;
		D3D12_RAYTRACING_INSTANCE_DESC* m_pInstanceUploadData = nullptr;

		std::vector<TLASInstance> m_instances;
		uint32_t m_tlasDescriptorIndex = 0;
	};
}
