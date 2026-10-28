#include "RaytracingManager.h"
#include "D3D12Device.h"
#include "BindlessDescriptorHeap.h"

namespace RenderCore
{
	CRaytracingManager& CRaytracingManager::Instance()
	{
		static CRaytracingManager instance;
		return instance;
	}

	CRaytracingManager::~CRaytracingManager()
	{
		Shutdown();
	}

	bool CRaytracingManager::Initialize()
	{
		if (m_initialized)
			return true;

		m_isSupported = CD3D12Device::Instance().IsRaytracingTier1_1Supported();
		if (!m_isSupported)
		{
			OutputDebugStringA("[RaytracingManager] DXR 1.1 not supported on this adapter.\n");
			return false;
		}

		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto pAllocator = CD3D12Device::Instance().GetAllocator();
		auto& heap = CBindlessDescriptorHeap::Instance();

		// 1. Allocate Instance Upload Buffer
		D3D12MA::ALLOCATION_DESC uploadAllocDesc = {};
		uploadAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(D3D12_RAYTRACING_INSTANCE_DESC) * MAX_TLAS_INSTANCES);

		ThrowIfFailed(pAllocator->CreateResource(
			&uploadAllocDesc,
			&uploadBufferDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			&m_instanceUploadAlloc,
			IID_PPV_ARGS(&m_instanceUploadBuffer)),
			"TLAS Instance Upload Buffer oluşturulamadı");

		ThrowIfFailed(m_instanceUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pInstanceUploadData)),
			"TLAS Instance Buffer Map başarısız");

		// 2. Query TLAS prebuild size
		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS tlasInputs = {};
		tlasInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		tlasInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
		tlasInputs.NumDescs = MAX_TLAS_INSTANCES;
		tlasInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
		pDevice->GetRaytracingAccelerationStructurePrebuildInfo(&tlasInputs, &prebuildInfo);

		// 3. Allocate TLAS Result & Scratch Buffers
		D3D12MA::ALLOCATION_DESC defaultAllocDesc = {};
		defaultAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC tlasBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(
			prebuildInfo.ResultDataMaxSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		ThrowIfFailed(pAllocator->CreateResource(
			&defaultAllocDesc,
			&tlasBufferDesc,
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			nullptr,
			&m_tlasAlloc,
			IID_PPV_ARGS(&m_tlasResource)),
			"TLAS Buffer oluşturulamadı");

		D3D12_RESOURCE_DESC scratchBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(
			prebuildInfo.ScratchDataSizeInBytes,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		ThrowIfFailed(pAllocator->CreateResource(
			&defaultAllocDesc,
			&scratchBufferDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			&m_tlasScratchAlloc,
			IID_PPV_ARGS(&m_tlasScratchResource)),
			"TLAS Scratch Buffer oluşturulamadı");

		// 4. Create TLAS SRV in Bindless Descriptor Heap
		m_tlasDescriptorIndex = heap.AllocateDescriptor();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_RAYTRACING_ACCELERATION_STRUCTURE;
		srvDesc.RaytracingAccelerationStructure.Location = m_tlasResource->GetGPUVirtualAddress();

		pDevice->CreateShaderResourceView(nullptr, &srvDesc, heap.GetCPUHandle(m_tlasDescriptorIndex));

		m_initialized = true;
		return true;
	}

	void CRaytracingManager::Shutdown()
	{
		if (m_pInstanceUploadData && m_instanceUploadBuffer)
		{
			m_instanceUploadBuffer->Unmap(0, nullptr);
			m_pInstanceUploadData = nullptr;
		}

		if (m_tlasDescriptorIndex != 0)
		{
			CBindlessDescriptorHeap::Instance().FreeDescriptor(m_tlasDescriptorIndex);
			m_tlasDescriptorIndex = 0;
		}

		m_instanceUploadBuffer.Reset();
		m_instanceUploadAlloc.Reset();
		m_tlasScratchResource.Reset();
		m_tlasScratchAlloc.Reset();
		m_tlasResource.Reset();
		m_tlasAlloc.Reset();

		m_instances.clear();
		m_initialized = false;
	}

	std::shared_ptr<BLASResource> CRaytracingManager::BuildStaticBLAS(
		ID3D12GraphicsCommandList7* pCmdList,
		const std::vector<RaytracingGeometryDesc>& geometries)
	{
		if (!m_isSupported || geometries.empty())
			return nullptr;

		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto pAllocator = CD3D12Device::Instance().GetAllocator();

		std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geomDescs(geometries.size());
		for (size_t i = 0; i < geometries.size(); ++i)
		{
			const auto& g = geometries[i];
			geomDescs[i].Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
			geomDescs[i].Flags = g.flags;
			geomDescs[i].Triangles.VertexBuffer.StartAddress = g.vertexBufferAddress;
			geomDescs[i].Triangles.VertexBuffer.StrideInBytes = g.vertexStride;
			geomDescs[i].Triangles.VertexCount = g.vertexCount;
			geomDescs[i].Triangles.VertexFormat = g.vertexFormat;
			geomDescs[i].Triangles.IndexBuffer = g.indexBufferAddress;
			geomDescs[i].Triangles.IndexCount = g.indexCount;
			geomDescs[i].Triangles.IndexFormat = g.indexFormat;
			geomDescs[i].Triangles.Transform3x4 = 0;
		}

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS blasInputs = {};
		blasInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		blasInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE |
						   D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_COMPACTION;
		blasInputs.NumDescs = static_cast<UINT>(geomDescs.size());
		blasInputs.pGeometryDescs = geomDescs.data();
		blasInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
		pDevice->GetRaytracingAccelerationStructurePrebuildInfo(&blasInputs, &prebuildInfo);

		auto blas = std::make_shared<BLASResource>();
		blas->resultSize = prebuildInfo.ResultDataMaxSizeInBytes;
		blas->scratchSize = prebuildInfo.ScratchDataSizeInBytes;
		blas->isDynamic = false;

		D3D12MA::ALLOCATION_DESC defaultAllocDesc = {};
		defaultAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC resultDesc = CD3DX12_RESOURCE_DESC::Buffer(
			blas->resultSize,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		ThrowIfFailed(pAllocator->CreateResource(
			&defaultAllocDesc,
			&resultDesc,
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			nullptr,
			&blas->allocation,
			IID_PPV_ARGS(&blas->resource)),
			"Static BLAS Resource oluşturulamadı");

		D3D12_RESOURCE_DESC scratchDesc = CD3DX12_RESOURCE_DESC::Buffer(
			blas->scratchSize,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		ThrowIfFailed(pAllocator->CreateResource(
			&defaultAllocDesc,
			&scratchDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			&blas->scratchAlloc,
			IID_PPV_ARGS(&blas->scratchBuffer)),
			"Static BLAS Scratch Buffer oluşturulamadı");

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
		buildDesc.Inputs = blasInputs;
		buildDesc.DestAccelerationStructureData = blas->resource->GetGPUVirtualAddress();
		buildDesc.ScratchAccelerationStructureData = blas->scratchBuffer->GetGPUVirtualAddress();

		pCmdList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);

		// Enhanced Barrier for Acceleration Structure Build
		D3D12_BUFFER_BARRIER barrier = CD3DX12_BUFFER_BARRIER(
			D3D12_BARRIER_SYNC_BUILD_RAYTRACING_ACCELERATION_STRUCTURE,
			D3D12_BARRIER_SYNC_ALL_SHADING,
			D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE,
			D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ,
			blas->resource.Get());

		D3D12_BARRIER_GROUP group = CD3DX12_BARRIER_GROUP(1, &barrier);
		pCmdList->Barrier(1, &group);

		blas->isBuilt = true;
		return blas;
	}

	std::shared_ptr<BLASResource> CRaytracingManager::BuildDynamicBLAS(
		ID3D12GraphicsCommandList7* pCmdList,
		const std::vector<RaytracingGeometryDesc>& geometries)
	{
		if (!m_isSupported || geometries.empty())
			return nullptr;

		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto pAllocator = CD3D12Device::Instance().GetAllocator();

		std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geomDescs(geometries.size());
		for (size_t i = 0; i < geometries.size(); ++i)
		{
			const auto& g = geometries[i];
			geomDescs[i].Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
			geomDescs[i].Flags = g.flags;
			geomDescs[i].Triangles.VertexBuffer.StartAddress = g.vertexBufferAddress;
			geomDescs[i].Triangles.VertexBuffer.StrideInBytes = g.vertexStride;
			geomDescs[i].Triangles.VertexCount = g.vertexCount;
			geomDescs[i].Triangles.VertexFormat = g.vertexFormat;
			geomDescs[i].Triangles.IndexBuffer = g.indexBufferAddress;
			geomDescs[i].Triangles.IndexCount = g.indexCount;
			geomDescs[i].Triangles.IndexFormat = g.indexFormat;
			geomDescs[i].Triangles.Transform3x4 = 0;
		}

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS blasInputs = {};
		blasInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		blasInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD |
						   D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;
		blasInputs.NumDescs = static_cast<UINT>(geomDescs.size());
		blasInputs.pGeometryDescs = geomDescs.data();
		blasInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

		D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfo = {};
		pDevice->GetRaytracingAccelerationStructurePrebuildInfo(&blasInputs, &prebuildInfo);

		auto blas = std::make_shared<BLASResource>();
		blas->resultSize = prebuildInfo.ResultDataMaxSizeInBytes;
		blas->scratchSize = prebuildInfo.ScratchDataSizeInBytes;
		blas->isDynamic = true;

		D3D12MA::ALLOCATION_DESC defaultAllocDesc = {};
		defaultAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC resultDesc = CD3DX12_RESOURCE_DESC::Buffer(
			blas->resultSize,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		ThrowIfFailed(pAllocator->CreateResource(
			&defaultAllocDesc,
			&resultDesc,
			D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE,
			nullptr,
			&blas->allocation,
			IID_PPV_ARGS(&blas->resource)),
			"Dynamic BLAS Resource oluşturulamadı");

		D3D12_RESOURCE_DESC scratchDesc = CD3DX12_RESOURCE_DESC::Buffer(
			blas->scratchSize,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		ThrowIfFailed(pAllocator->CreateResource(
			&defaultAllocDesc,
			&scratchDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			&blas->scratchAlloc,
			IID_PPV_ARGS(&blas->scratchBuffer)),
			"Dynamic BLAS Scratch Buffer oluşturulamadı");

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
		buildDesc.Inputs = blasInputs;
		buildDesc.DestAccelerationStructureData = blas->resource->GetGPUVirtualAddress();
		buildDesc.ScratchAccelerationStructureData = blas->scratchBuffer->GetGPUVirtualAddress();

		pCmdList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);

		D3D12_BUFFER_BARRIER barrier = CD3DX12_BUFFER_BARRIER(
			D3D12_BARRIER_SYNC_BUILD_RAYTRACING_ACCELERATION_STRUCTURE,
			D3D12_BARRIER_SYNC_ALL_SHADING,
			D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE,
			D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ,
			blas->resource.Get());

		D3D12_BARRIER_GROUP group = CD3DX12_BARRIER_GROUP(1, &barrier);
		pCmdList->Barrier(1, &group);

		blas->isBuilt = true;
		return blas;
	}

	void CRaytracingManager::RefitDynamicBLAS(
		ID3D12GraphicsCommandList7* pCmdList,
		BLASResource* pBLAS,
		const std::vector<RaytracingGeometryDesc>& geometries)
	{
		if (!m_isSupported || !pBLAS || geometries.empty())
			return;

		std::vector<D3D12_RAYTRACING_GEOMETRY_DESC> geomDescs(geometries.size());
		for (size_t i = 0; i < geometries.size(); ++i)
		{
			const auto& g = geometries[i];
			geomDescs[i].Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
			geomDescs[i].Flags = g.flags;
			geomDescs[i].Triangles.VertexBuffer.StartAddress = g.vertexBufferAddress;
			geomDescs[i].Triangles.VertexBuffer.StrideInBytes = g.vertexStride;
			geomDescs[i].Triangles.VertexCount = g.vertexCount;
			geomDescs[i].Triangles.VertexFormat = g.vertexFormat;
			geomDescs[i].Triangles.IndexBuffer = g.indexBufferAddress;
			geomDescs[i].Triangles.IndexCount = g.indexCount;
			geomDescs[i].Triangles.IndexFormat = g.indexFormat;
			geomDescs[i].Triangles.Transform3x4 = 0;
		}

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS blasInputs = {};
		blasInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
		blasInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_BUILD |
						   D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE |
						   D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PERFORM_UPDATE;
		blasInputs.NumDescs = static_cast<UINT>(geomDescs.size());
		blasInputs.pGeometryDescs = geomDescs.data();
		blasInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
		buildDesc.Inputs = blasInputs;
		buildDesc.SourceAccelerationStructureData = pBLAS->resource->GetGPUVirtualAddress();
		buildDesc.DestAccelerationStructureData = pBLAS->resource->GetGPUVirtualAddress();
		buildDesc.ScratchAccelerationStructureData = pBLAS->scratchBuffer->GetGPUVirtualAddress();

		pCmdList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);

		D3D12_BUFFER_BARRIER barrier = CD3DX12_BUFFER_BARRIER(
			D3D12_BARRIER_SYNC_BUILD_RAYTRACING_ACCELERATION_STRUCTURE,
			D3D12_BARRIER_SYNC_ALL_SHADING,
			D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE,
			D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ,
			pBLAS->resource.Get());

		D3D12_BARRIER_GROUP group = CD3DX12_BARRIER_GROUP(1, &barrier);
		pCmdList->Barrier(1, &group);
	}

	void CRaytracingManager::ResetInstances()
	{
		m_instances.clear();
	}

	void CRaytracingManager::AddInstance(const TLASInstance& instance)
	{
		if (m_instances.size() < MAX_TLAS_INSTANCES)
		{
			m_instances.push_back(instance);
		}
	}

	void CRaytracingManager::BuildTLAS(ID3D12GraphicsCommandList7* pCmdList)
	{
		if (!m_isSupported || m_instances.empty())
			return;

		const uint32_t instanceCount = static_cast<uint32_t>(m_instances.size());

		// Populate Instance Descriptors
		for (uint32_t i = 0; i < instanceCount; ++i)
		{
			const auto& src = m_instances[i];
			D3D12_RAYTRACING_INSTANCE_DESC& dst = m_pInstanceUploadData[i];

			// Transpose 4x4 matrix to 3x4 row-major
			XMFLOAT4X4 mat;
			XMStoreFloat4x4(&mat, src.transform);

			dst.Transform[0][0] = mat._11; dst.Transform[0][1] = mat._21; dst.Transform[0][2] = mat._31; dst.Transform[0][3] = mat._41;
			dst.Transform[1][0] = mat._12; dst.Transform[1][1] = mat._22; dst.Transform[1][2] = mat._32; dst.Transform[1][3] = mat._42;
			dst.Transform[2][0] = mat._13; dst.Transform[2][1] = mat._23; dst.Transform[2][2] = mat._33; dst.Transform[2][3] = mat._43;

			dst.InstanceID = src.instanceID;
			dst.InstanceMask = src.instanceMask;
			dst.InstanceContributionToHitGroupIndex = src.instanceContributionToHitGroupIndex;
			dst.Flags = src.flags;
			dst.AccelerationStructure = src.pBLAS ? src.pBLAS->resource->GetGPUVirtualAddress() : 0;
		}

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS tlasInputs = {};
		tlasInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;
		tlasInputs.Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;
		tlasInputs.NumDescs = instanceCount;
		tlasInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
		tlasInputs.InstanceDescs = m_instanceUploadBuffer->GetGPUVirtualAddress();

		D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDesc = {};
		buildDesc.Inputs = tlasInputs;
		buildDesc.DestAccelerationStructureData = m_tlasResource->GetGPUVirtualAddress();
		buildDesc.ScratchAccelerationStructureData = m_tlasScratchResource->GetGPUVirtualAddress();

		pCmdList->BuildRaytracingAccelerationStructure(&buildDesc, 0, nullptr);

		// Enhanced Barrier: Wait for TLAS build before any raytracing queries
		D3D12_BUFFER_BARRIER barrier = CD3DX12_BUFFER_BARRIER(
			D3D12_BARRIER_SYNC_BUILD_RAYTRACING_ACCELERATION_STRUCTURE,
			D3D12_BARRIER_SYNC_ALL_SHADING,
			D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_WRITE,
			D3D12_BARRIER_ACCESS_RAYTRACING_ACCELERATION_STRUCTURE_READ,
			m_tlasResource.Get());

		D3D12_BARRIER_GROUP group = CD3DX12_BARRIER_GROUP(1, &barrier);
		pCmdList->Barrier(1, &group);
	}
}
