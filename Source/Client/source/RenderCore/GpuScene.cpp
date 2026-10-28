#include "GpuScene.h"
#include "D3D12Device.h"
#include "BindlessDescriptorHeap.h"
#include "ShaderCompiler.h"
#include <algorithm>

namespace RenderCore
{
	CGpuScene& CGpuScene::Instance()
	{
		static CGpuScene instance;
		return instance;
	}

	CGpuScene::~CGpuScene()
	{
		Shutdown();
	}

	bool CGpuScene::Initialize(uint32_t screenWidth, uint32_t screenHeight)
	{
		if (m_initialized)
			return true;

		m_screenWidth = screenWidth;
		m_screenHeight = screenHeight;

		if (!CreateBuffers())
			return false;

		if (!CreateHiZResources(screenWidth, screenHeight))
			return false;

		if (!CreatePipelines())
			return false;

		m_initialized = true;
		return true;
	}

	void CGpuScene::Shutdown()
	{
		auto& heap = CBindlessDescriptorHeap::Instance();

		if (m_pInstanceUploadData && m_instanceUploadBuffer)
		{
			m_instanceUploadBuffer->Unmap(0, nullptr);
			m_pInstanceUploadData = nullptr;
		}

		if (m_instanceBufferSRV != 0) heap.FreeDescriptor(m_instanceBufferSRV);
		if (m_indirectArgsSRV != 0) heap.FreeDescriptor(m_indirectArgsSRV);
		if (m_indirectArgsUAV != 0) heap.FreeDescriptor(m_indirectArgsUAV);
		if (m_drawCountUAV != 0) heap.FreeDescriptor(m_drawCountUAV);
		if (m_hizSRV != 0) heap.FreeDescriptor(m_hizSRV);

		for (uint32_t i = 0; i < HIZ_MIP_LEVELS; ++i)
		{
			if (m_hizMipUAVs[i] != 0)
			{
				heap.FreeDescriptor(m_hizMipUAVs[i]);
				m_hizMipUAVs[i] = 0;
			}
		}

		m_cullingPSO.Reset();
		m_hizGeneratePSO.Reset();
		m_hizTexture.Reset();
		m_hizTextureAlloc.Reset();
		m_drawCountBuffer.Reset();
		m_drawCountAlloc.Reset();
		m_indirectArgsBuffer.Reset();
		m_indirectArgsAlloc.Reset();
		m_instanceUploadBuffer.Reset();
		m_instanceUploadAlloc.Reset();
		m_commandSignature.Reset();

		m_instances.clear();
		m_initialized = false;
	}

	void CGpuScene::Resize(uint32_t screenWidth, uint32_t screenHeight)
	{
		if (m_screenWidth == screenWidth && m_screenHeight == screenHeight)
			return;

		m_screenWidth = screenWidth;
		m_screenHeight = screenHeight;

		auto& heap = CBindlessDescriptorHeap::Instance();
		if (m_hizSRV != 0)
		{
			heap.FreeDescriptor(m_hizSRV);
			m_hizSRV = 0;
		}

		for (uint32_t i = 0; i < HIZ_MIP_LEVELS; ++i)
		{
			if (m_hizMipUAVs[i] != 0)
			{
				heap.FreeDescriptor(m_hizMipUAVs[i]);
				m_hizMipUAVs[i] = 0;
			}
		}

		m_hizTexture.Reset();
		m_hizTextureAlloc.Reset();

		CreateHiZResources(screenWidth, screenHeight);
	}

	bool CGpuScene::CreateBuffers()
	{
		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto pAllocator = CD3D12Device::Instance().GetAllocator();
		auto& heap = CBindlessDescriptorHeap::Instance();

		// 1. Create Command Signature for Indirect Draw
		// Arguments: Root Constant (Object ID at b0) + Draw Indexed Arguments
		D3D12_INDIRECT_ARGUMENT_DESC args[2] = {};
		args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_CONSTANT;
		args[0].Constant.RootParameterIndex = 0;
		args[0].Constant.DestOffsetIn32BitValues = 0;
		args[0].Constant.Num32BitValuesToSet = 1;

		args[1].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

		D3D12_COMMAND_SIGNATURE_DESC cmdSigDesc = {};
		cmdSigDesc.ByteStride = sizeof(IndirectDrawIndexedCommand);
		cmdSigDesc.NumArgumentDescs = 2;
		cmdSigDesc.pArgumentDescs = args;
		cmdSigDesc.NodeMask = 0;

		ThrowIfFailed(pDevice->CreateCommandSignature(&cmdSigDesc, heap.GetGlobalRootSignature(), IID_PPV_ARGS(&m_commandSignature)),
			"Indirect Command Signature oluşturulamadı");

		// 2. GPU Instance Upload Buffer
		D3D12MA::ALLOCATION_DESC uploadAllocDesc = {};
		uploadAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC instBufDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(GpuInstanceData) * MAX_GPU_INSTANCES);

		ThrowIfFailed(pAllocator->CreateResource(
			&uploadAllocDesc,
			&instBufDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			&m_instanceUploadAlloc,
			IID_PPV_ARGS(&m_instanceUploadBuffer)),
			"Instance Upload Buffer oluşturulamadı");

		ThrowIfFailed(m_instanceUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pInstanceUploadData)),
			"Instance Upload Buffer Map başarısız");

		m_instanceBufferSRV = heap.AllocateDescriptor();
		D3D12_SHADER_RESOURCE_VIEW_DESC instSRVDesc = {};
		instSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		instSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		instSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		instSRVDesc.Buffer.NumElements = MAX_GPU_INSTANCES;
		instSRVDesc.Buffer.StructureByteStride = sizeof(GpuInstanceData);
		heap.CreateShaderResourceView(m_instanceUploadBuffer.Get(), &instSRVDesc, m_instanceBufferSRV);

		// 3. Indirect Arguments Buffer & Count Buffer
		D3D12MA::ALLOCATION_DESC defaultAllocDesc = {};
		defaultAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC argsBufDesc = CD3DX12_RESOURCE_DESC::Buffer(
			sizeof(IndirectDrawIndexedCommand) * MAX_GPU_INSTANCES,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		ThrowIfFailed(pAllocator->CreateResource(
			&defaultAllocDesc,
			&argsBufDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			&m_indirectArgsAlloc,
			IID_PPV_ARGS(&m_indirectArgsBuffer)),
			"Indirect Arguments Buffer oluşturulamadı");

		m_indirectArgsSRV = heap.AllocateDescriptor();
		m_indirectArgsUAV = heap.AllocateDescriptor();

		D3D12_SHADER_RESOURCE_VIEW_DESC argsSRVDesc = {};
		argsSRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		argsSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
		argsSRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		argsSRVDesc.Buffer.NumElements = MAX_GPU_INSTANCES;
		argsSRVDesc.Buffer.StructureByteStride = sizeof(IndirectDrawIndexedCommand);
		heap.CreateShaderResourceView(m_indirectArgsBuffer.Get(), &argsSRVDesc, m_indirectArgsSRV);

		D3D12_UNORDERED_ACCESS_VIEW_DESC argsUAVDesc = {};
		argsUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		argsUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		argsUAVDesc.Buffer.NumElements = MAX_GPU_INSTANCES;
		argsUAVDesc.Buffer.StructureByteStride = sizeof(IndirectDrawIndexedCommand);
		heap.CreateUnorderedAccessView(m_indirectArgsBuffer.Get(), &argsUAVDesc, m_indirectArgsUAV);

		// Draw count buffer (1 uint)
		D3D12_RESOURCE_DESC countBufDesc = CD3DX12_RESOURCE_DESC::Buffer(
			sizeof(uint32_t) * 4,
			D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		ThrowIfFailed(pAllocator->CreateResource(
			&defaultAllocDesc,
			&countBufDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			&m_drawCountAlloc,
			IID_PPV_ARGS(&m_drawCountBuffer)),
			"Draw Count Buffer oluşturulamadı");

		m_drawCountUAV = heap.AllocateDescriptor();
		D3D12_UNORDERED_ACCESS_VIEW_DESC countUAVDesc = {};
		countUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
		countUAVDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		countUAVDesc.Buffer.NumElements = 4;
		countUAVDesc.Buffer.StructureByteStride = sizeof(uint32_t);
		heap.CreateUnorderedAccessView(m_drawCountBuffer.Get(), &countUAVDesc, m_drawCountUAV);

		m_constants.instanceBufferSRV = m_instanceBufferSRV;
		m_constants.indirectArgsUAV = m_indirectArgsUAV;
		m_constants.drawCountUAV = m_drawCountUAV;

		return true;
	}

	bool CGpuScene::CreateHiZResources(uint32_t screenWidth, uint32_t screenHeight)
	{
		auto pAllocator = CD3D12Device::Instance().GetAllocator();
		auto& heap = CBindlessDescriptorHeap::Instance();

		// Hi-Z texture starts at half screen resolution, with 8 mip levels
		m_hizWidth = std::max(1u, screenWidth / 2);
		m_hizHeight = std::max(1u, screenHeight / 2);

		D3D12MA::ALLOCATION_DESC defaultAllocDesc = {};
		defaultAllocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC hizDesc = {};
		hizDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		hizDesc.Width = m_hizWidth;
		hizDesc.Height = m_hizHeight;
		hizDesc.DepthOrArraySize = 1;
		hizDesc.MipLevels = HIZ_MIP_LEVELS;
		hizDesc.Format = DXGI_FORMAT_R32_FLOAT;
		hizDesc.SampleDesc.Count = 1;
		hizDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		ThrowIfFailed(pAllocator->CreateResource(
			&defaultAllocDesc,
			&hizDesc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			&m_hizTextureAlloc,
			IID_PPV_ARGS(&m_hizTexture)),
			"Hi-Z Texture oluşturulamadı");

		m_hizSRV = heap.AllocateDescriptor();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = HIZ_MIP_LEVELS;
		heap.CreateShaderResourceView(m_hizTexture.Get(), &srvDesc, m_hizSRV);

		for (uint32_t mip = 0; mip < HIZ_MIP_LEVELS; ++mip)
		{
			m_hizMipUAVs[mip] = heap.AllocateDescriptor();
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Texture2D.MipSlice = mip;
			heap.CreateUnorderedAccessView(m_hizTexture.Get(), &uavDesc, m_hizMipUAVs[mip]);
		}

		m_constants.hizDepthSRV = m_hizSRV;
		m_constants.hizWidth = m_hizWidth;
		m_constants.hizHeight = m_hizHeight;
		m_constants.hizMipCount = HIZ_MIP_LEVELS;

		return true;
	}

	bool CGpuScene::CreatePipelines()
	{
		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto pRootSig = CBindlessDescriptorHeap::Instance().GetGlobalRootSignature();
		auto& compiler = CShaderCompiler::Instance();

		std::wstring shaderPath = L"source/RenderCore/Shaders/GpuScene.hlsl";

		// 1. Hi-Z Downsampling PSO
		ComPtr<IDxcBlob> hizCSBlob;
		if (compiler.CompileShaderFromFile(shaderPath, L"GenerateHiZMipCS", L"cs_6_6", hizCSBlob))
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.CS = { hizCSBlob->GetBufferPointer(), hizCSBlob->GetBufferSize() };
			ThrowIfFailed(pDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_hizGeneratePSO)),
				"Hi-Z Generate PSO oluşturulamadı");
		}

		// 2. GPU Culling PSO
		ComPtr<IDxcBlob> cullCSBlob;
		if (compiler.CompileShaderFromFile(shaderPath, L"GpuCullCS", L"cs_6_6", cullCSBlob))
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.CS = { cullCSBlob->GetBufferPointer(), cullCSBlob->GetBufferSize() };
			ThrowIfFailed(pDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_cullingPSO)),
				"GPU Culling PSO oluşturulamadı");
		}

		return true;
	}

	void CGpuScene::ResetInstances()
	{
		m_instances.clear();
	}

	void CGpuScene::AddInstance(const GpuInstanceData& instance)
	{
		if (m_instances.size() < MAX_GPU_INSTANCES)
		{
			m_instances.push_back(instance);
		}
	}

	void CGpuScene::GenerateHiZPyramid(ID3D12GraphicsCommandList7* pCmdList, uint32_t depthSRV)
	{
		if (!m_hizGeneratePSO)
			return;

		auto& heap = CBindlessDescriptorHeap::Instance();
		heap.SetDescriptorHeaps(pCmdList);
		pCmdList->SetComputeRootSignature(heap.GetGlobalRootSignature());
		pCmdList->SetPipelineState(m_hizGeneratePSO.Get());

		// Downsample each mip level iteratively
		uint32_t curWidth = m_hizWidth;
		uint32_t curHeight = m_hizHeight;
		uint32_t srcSRV = depthSRV;

		for (uint32_t mip = 0; mip < HIZ_MIP_LEVELS; ++mip)
		{
			GpuSceneConstants params = m_constants;
			params.sourceMipIndex = srcSRV;
			params.targetMipUAV = m_hizMipUAVs[mip];
			params.hizWidth = curWidth;
			params.hizHeight = curHeight;

			pCmdList->SetComputeRoot32BitConstants(0, sizeof(params) / 4, &params, 0);
			pCmdList->Dispatch((curWidth + 7) / 8, (curHeight + 7) / 8, 1);

			// Enhanced Barrier for mip level synchronization
			D3D12_TEXTURE_BARRIER barrier = CD3DX12_TEXTURE_BARRIER(
				D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_COMPUTE_SHADING,
				D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
				D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS, D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
				m_hizTexture.Get(),
				CD3DX12_BARRIER_SUBRESOURCE_RANGE(mip, 1, 0, 1, 0, 1),
				D3D12_TEXTURE_BARRIER_FLAG_NONE);

			D3D12_BARRIER_GROUP group = CD3DX12_BARRIER_GROUP(1, &barrier);
			pCmdList->Barrier(1, &group);

			srcSRV = m_hizSRV;
			curWidth = std::max(1u, curWidth / 2);
			curHeight = std::max(1u, curHeight / 2);
		}
	}

	void CGpuScene::CullAndBuildIndirectDraws(ID3D12GraphicsCommandList7* pCmdList, bool isPrepass)
	{
		if (!m_cullingPSO || m_instances.empty())
			return;

		m_constants.totalInstanceCount = static_cast<uint32_t>(m_instances.size());
		m_constants.isPrepass = isPrepass ? 1 : 0;

		// Upload instance data
		memcpy(m_pInstanceUploadData, m_instances.data(), sizeof(GpuInstanceData) * m_instances.size());

		auto& heap = CBindlessDescriptorHeap::Instance();
		heap.SetDescriptorHeaps(pCmdList);
		pCmdList->SetComputeRootSignature(heap.GetGlobalRootSignature());
		pCmdList->SetPipelineState(m_cullingPSO.Get());

		pCmdList->SetComputeRoot32BitConstants(0, sizeof(m_constants) / 4, &m_constants, 0);

		// Dispatch culling
		uint32_t threadGroups = (m_constants.totalInstanceCount + 63) / 64;
		pCmdList->Dispatch(threadGroups, 1, 1);

		// Barrier: Indirect args buffer ready for ExecuteIndirect
		D3D12_BUFFER_BARRIER barrier = CD3DX12_BUFFER_BARRIER(
			D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_EXECUTE_INDIRECT,
			D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_INDIRECT_ARGUMENT,
			m_indirectArgsBuffer.Get());

		D3D12_BARRIER_GROUP group = CD3DX12_BARRIER_GROUP(1, &barrier);
		pCmdList->Barrier(1, &group);
	}

	void CGpuScene::ExecuteIndirect(ID3D12GraphicsCommandList7* pCmdList)
	{
		if (!m_commandSignature || m_instances.empty())
			return;

		pCmdList->ExecuteIndirect(
			m_commandSignature.Get(),
			static_cast<UINT>(m_instances.size()),
			m_indirectArgsBuffer.Get(),
			0,
			m_drawCountBuffer.Get(),
			0);
	}
}
