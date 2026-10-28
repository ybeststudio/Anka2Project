#include "ClusteredLighting.h"
#include "D3D12Device.h"
#include "BindlessDescriptorHeap.h"
#include "ShaderCompiler.h"

namespace RenderCore
{
	CClusteredLighting& CClusteredLighting::Instance()
	{
		static CClusteredLighting instance;
		return instance;
	}

	CClusteredLighting::~CClusteredLighting()
	{
		Shutdown();
	}

	bool CClusteredLighting::Initialize()
	{
		if (m_initialized)
			return true;

		if (!CreateBuffers())
			return false;

		if (!CreatePipelines())
			return false;

		m_initialized = true;
		return true;
	}

	void CClusteredLighting::Shutdown()
	{
		auto& heap = CBindlessDescriptorHeap::Instance();

		if (m_pLightUploadData && m_lightUploadBuffer)
		{
			m_lightUploadBuffer->Unmap(0, nullptr);
			m_pLightUploadData = nullptr;
		}

		if (m_lightBufferSRV != 0) heap.FreeDescriptor(m_lightBufferSRV);
		if (m_clusterAABBSRV != 0) heap.FreeDescriptor(m_clusterAABBSRV);
		if (m_clusterAABBUAV != 0) heap.FreeDescriptor(m_clusterAABBUAV);
		if (m_clusterMaskSRV != 0) heap.FreeDescriptor(m_clusterMaskSRV);
		if (m_clusterMaskUAV != 0) heap.FreeDescriptor(m_clusterMaskUAV);

		m_clusterMaskBuffer.Reset();
		m_clusterMaskAlloc.Reset();
		m_clusterAABBBuffer.Reset();
		m_clusterAABBAlloc.Reset();
		m_lightUploadBuffer.Reset();
		m_lightUploadAlloc.Reset();

		m_cullLightsPSO.Reset();
		m_buildAABBsPSO.Reset();

		m_lights.clear();
		m_initialized = false;
	}

	bool CClusteredLighting::CreateBuffers()
	{
		auto pAllocator = CD3D12Device::Instance().GetAllocator();
		auto& heap = CBindlessDescriptorHeap::Instance();

		// 1. Light Upload Buffer (128 lights)
		D3D12MA::ALLOCATION_DESC uploadAllocDesc = {};
		uploadAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC lightBufDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(LocalPointLight) * MAX_LOCAL_LIGHTS);

		ThrowIfFailed(pAllocator->CreateResource(
			&uploadAllocDesc,
			&lightBufDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			&m_lightUploadAlloc,
			IID_PPV_ARGS(&m_lightUploadBuffer)),
			"Clustered Light Upload Buffer oluşturulamadı");

		ThrowIfFailed(m_lightUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pLightUploadData)),
			"Clustered Light Upload Buffer Map başarısız");

		m_lightBufferSRV = heap.AllocateDescriptor();
		D3D12_SHADER_RESOURCE_VIEW_DESC lightSRVDesc = {};
		lightSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		lightSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		lightSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		lightSRVDesc.Buffer.FirstElement = 0;
		lightSRVDesc.Buffer.NumElements = MAX_LOCAL_LIGHTS;
		lightSRVDesc.Buffer.StructureByteStride = sizeof(LocalPointLight);
		heap.CreateShaderResourceView(m_lightUploadBuffer.Get(), &lightSRVDesc, m_lightBufferSRV);

		// 2. Cluster AABB Buffer (TOTAL_CLUSTERS = 3456 elements)
		D3D12MA::ALLOCATION_DESC defaultAllocDesc = {};
		defaultAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC aabbBufDesc = CD3DX12_RESOURCE_DESC::Buffer(
			sizeof(ClusterAABB) * TOTAL_CLUSTERS,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		ThrowIfFailed(pAllocator->CreateResource(
			&defaultAllocDesc,
			&aabbBufDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			&m_clusterAABBAlloc,
			IID_PPV_ARGS(&m_clusterAABBBuffer)),
			"Cluster AABB Buffer oluşturulamadı");

		m_clusterAABBSRV = heap.AllocateDescriptor();
		m_clusterAABBUAV = heap.AllocateDescriptor();

		D3D12_SHADER_RESOURCE_VIEW_DESC aabbSRVDesc = {};
		aabbSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		aabbSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		aabbSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		aabbSRVDesc.Buffer.NumElements = TOTAL_CLUSTERS;
		aabbSRVDesc.Buffer.StructureByteStride = sizeof(ClusterAABB);
		heap.CreateShaderResourceView(m_clusterAABBBuffer.Get(), &aabbSRVDesc, m_clusterAABBSRV);

		D3D12_UNORDERED_ACCESS_VIEW_DESC aabbUAVDesc = {};
		aabbUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		aabbUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		aabbUAVDesc.Buffer.NumElements = TOTAL_CLUSTERS;
		aabbUAVDesc.Buffer.StructureByteStride = sizeof(ClusterAABB);
		heap.CreateUnorderedAccessView(m_clusterAABBBuffer.Get(), &aabbUAVDesc, m_clusterAABBUAV);

		// 3. Cluster Light Mask Buffer (TOTAL_CLUSTERS * 16 bytes = uint4 per cluster)
		D3D12_RESOURCE_DESC maskBufDesc = CD3DX12_RESOURCE_DESC::Buffer(
			sizeof(uint32_t) * 4 * TOTAL_CLUSTERS,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		ThrowIfFailed(pAllocator->CreateResource(
			&defaultAllocDesc,
			&maskBufDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			&m_clusterMaskAlloc,
			IID_PPV_ARGS(&m_clusterMaskBuffer)),
			"Cluster Mask Buffer oluşturulamadı");

		m_clusterMaskSRV = heap.AllocateDescriptor();
		m_clusterMaskUAV = heap.AllocateDescriptor();

		D3D12_SHADER_RESOURCE_VIEW_DESC maskSRVDesc = {};
		maskSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		maskSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		maskSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		maskSRVDesc.Buffer.NumElements = TOTAL_CLUSTERS;
		maskSRVDesc.Buffer.StructureByteStride = sizeof(uint32_t) * 4;
		heap.CreateShaderResourceView(m_clusterMaskBuffer.Get(), &maskSRVDesc, m_clusterMaskSRV);

		D3D12_UNORDERED_ACCESS_VIEW_DESC maskUAVDesc = {};
		maskUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		maskUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		maskUAVDesc.Buffer.NumElements = TOTAL_CLUSTERS;
		maskUAVDesc.Buffer.StructureByteStride = sizeof(uint32_t) * 4;
		heap.CreateUnorderedAccessView(m_clusterMaskBuffer.Get(), &maskUAVDesc, m_clusterMaskUAV);

		m_constants.lightBufferSRV = m_lightBufferSRV;
		m_constants.clusterAABBBufferSRV = m_clusterAABBSRV;
		m_constants.clusterAABBBufferUAV = m_clusterAABBUAV;
		m_constants.clusterMaskBufferSRV = m_clusterMaskSRV;
		m_constants.clusterMaskBufferUAV = m_clusterMaskUAV;

		return true;
	}

	bool CClusteredLighting::CreatePipelines()
	{
		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto pRootSig = CBindlessDescriptorHeap::Instance().GetGlobalRootSignature();
		auto& compiler = CShaderCompiler::Instance();

		std::wstring shaderPath = L"source/RenderCore/Shaders/ClusteredLighting.hlsl";

		// 1. Build Cluster AABBs Compute PSO
		ComPtr<IDxcBlob> aabbCSBlob;
		if (compiler.CompileShaderFromFile(shaderPath, L"BuildClusterAABBs_CS", L"cs_6_6", aabbCSBlob))
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.CS = { aabbCSBlob->GetBufferPointer(), aabbCSBlob->GetBufferSize() };
			ThrowIfFailed(pDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_buildAABBsPSO)),
				"Build Cluster AABBs PSO oluşturulamadı");
		}

		// 2. Light Culling Compute PSO
		ComPtr<IDxcBlob> cullCSBlob;
		if (compiler.CompileShaderFromFile(shaderPath, L"CullLights_CS", L"cs_6_6", cullCSBlob))
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.CS = { cullCSBlob->GetBufferPointer(), cullCSBlob->GetBufferSize() };
			ThrowIfFailed(pDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_cullLightsPSO)),
				"Cluster Light Culling PSO oluşturulamadı");
		}

		return true;
	}

	void CClusteredLighting::ResetLights()
	{
		m_lights.clear();
	}

	void CClusteredLighting::AddPointLight(const XMFLOAT3& position, float radius, const XMFLOAT3& color, float intensity)
	{
		if (m_lights.size() < MAX_LOCAL_LIGHTS)
		{
			LocalPointLight light = {};
			light.position = position;
			light.radius = radius;
			light.color = color;
			light.intensity = intensity;
			m_lights.push_back(light);
		}
	}

	void CClusteredLighting::UpdateAndCull(ID3D12GraphicsCommandList7* pCmdList, const XMMATRIX& viewMatrix, const XMMATRIX& projMatrix)
	{
		if (!m_buildAABBsPSO || !m_cullLightsPSO)
			return;

		m_constants.activeLightCount = static_cast<uint32_t>(m_lights.size());

		// Upload light data
		if (!m_lights.empty())
		{
			memcpy(m_pLightUploadData, m_lights.data(), sizeof(LocalPointLight) * m_lights.size());
		}

		auto& heap = CBindlessDescriptorHeap::Instance();
		heap.SetDescriptorHeaps(pCmdList);
		pCmdList->SetComputeRootSignature(heap.GetGlobalRootSignature());

		// 1. Build AABBs (dispatch 1 x 1 x 24 thread groups of 16x9x1)
		pCmdList->SetPipelineState(m_buildAABBsPSO.Get());
		pCmdList->Dispatch(1, 1, CLUSTER_GRID_Z);

		// Enhanced Barrier: AABB Buffer from UAV to SHADER_RESOURCE
		D3D12_BUFFER_BARRIER aabbBarrier = CD3DX12_BUFFER_BARRIER(
			D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_COMPUTE_SHADING,
			D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
			m_clusterAABBBuffer.Get());

		D3D12_BARRIER_GROUP group = CD3DX12_BARRIER_GROUP(1, &aabbBarrier);
		pCmdList->Barrier(1, &group);

		// 2. Cull Lights (dispatch 1 x 1 x 24 thread groups of 16x9x1)
		pCmdList->SetPipelineState(m_cullLightsPSO.Get());
		pCmdList->Dispatch(1, 1, CLUSTER_GRID_Z);

		// Enhanced Barrier: Mask Buffer from UAV to PIXEL_SHADING SHADER_RESOURCE
		D3D12_BUFFER_BARRIER maskBarrier = CD3DX12_BUFFER_BARRIER(
			D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_PIXEL_SHADING,
			D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
			m_clusterMaskBuffer.Get());

		group = CD3DX12_BARRIER_GROUP(1, &maskBarrier);
		pCmdList->Barrier(1, &group);
	}
}
