#include "VolumetricFog.h"
#include "D3D12Device.h"
#include "BindlessDescriptorHeap.h"
#include "ShaderCompiler.h"

namespace RenderCore
{
	CVolumetricFog::~CVolumetricFog()
	{
		Shutdown();
	}

	bool CVolumetricFog::Initialize()
	{
		if (m_initialized)
			return true;

		if (!CreateTexturesAndDescriptors())
			return false;

		if (!CreatePipelines())
			return false;

		m_initialized = true;
		return true;
	}

	void CVolumetricFog::Shutdown()
	{
		auto& heap = CBindlessDescriptorHeap::Instance();

		if (m_injectionSRVIndex != 0) heap.FreeDescriptor(m_injectionSRVIndex);
		if (m_injectionUAVIndex != 0) heap.FreeDescriptor(m_injectionUAVIndex);
		if (m_accumulatedSRVIndex != 0) heap.FreeDescriptor(m_accumulatedSRVIndex);
		if (m_accumulatedUAVIndex != 0) heap.FreeDescriptor(m_accumulatedUAVIndex);

		m_injectionTexture.Reset();
		m_injectionAlloc.Reset();
		m_accumulatedTexture.Reset();
		m_accumulatedAlloc.Reset();

		m_compositePSO.Reset();
		m_volumeIntegrationPSO.Reset();
		m_lightInjectionPSO.Reset();

		m_initialized = false;
	}

	bool CVolumetricFog::CreateTexturesAndDescriptors()
	{
		auto pAllocator = CD3D12Device::Instance().GetAllocator();
		auto& heap = CBindlessDescriptorHeap::Instance();

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		// 3D Texture Desc: 160 x 90 x 64, R16G16B16A16_FLOAT
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		desc.Width = FROXEL_GRID_X;
		desc.Height = FROXEL_GRID_Y;
		desc.DepthOrArraySize = FROXEL_GRID_Z;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		// 1. Light Injection 3D Texture
		ThrowIfFailed(pAllocator->CreateResource(
			&allocDesc,
			&desc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			&m_injectionAlloc,
			IID_PPV_ARGS(&m_injectionTexture)),
			"Volumetric Fog Injection Texture oluşturulamadı");

		m_injectionSRVIndex = heap.AllocateDescriptor();
		m_injectionUAVIndex = heap.AllocateDescriptor();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
		srvDesc.Texture3D.MipLevels = 1;
		heap.CreateShaderResourceView(m_injectionTexture.Get(), &srvDesc, m_injectionSRVIndex);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = desc.Format;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
		uavDesc.Texture3D.MipSlice = 0;
		uavDesc.Texture3D.FirstWSlice = 0;
		uavDesc.Texture3D.WSize = FROXEL_GRID_Z;
		heap.CreateUnorderedAccessView(m_injectionTexture.Get(), &uavDesc, m_injectionUAVIndex);

		// 2. Volume Integration / Accumulated 3D Texture
		ThrowIfFailed(pAllocator->CreateResource(
			&allocDesc,
			&desc,
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
			nullptr,
			&m_accumulatedAlloc,
			IID_PPV_ARGS(&m_accumulatedTexture)),
			"Volumetric Fog Accumulated Texture oluşturulamadı");

		m_accumulatedSRVIndex = heap.AllocateDescriptor();
		m_accumulatedUAVIndex = heap.AllocateDescriptor();

		heap.CreateShaderResourceView(m_accumulatedTexture.Get(), &srvDesc, m_accumulatedSRVIndex);
		heap.CreateUnorderedAccessView(m_accumulatedTexture.Get(), &uavDesc, m_accumulatedUAVIndex);

		m_constants.injectionUAVIndex = m_injectionUAVIndex;
		m_constants.accumulatedUAVIndex = m_accumulatedUAVIndex;
		m_constants.accumulatedSRVIndex = m_accumulatedSRVIndex;

		return true;
	}

	bool CVolumetricFog::CreatePipelines()
	{
		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto pRootSig = CBindlessDescriptorHeap::Instance().GetGlobalRootSignature();
		auto& compiler = CShaderCompiler::Instance();

		std::wstring shaderPath = L"source/RenderCore/Shaders/VolumetricFog.hlsl";

		// 1. Light Injection Compute PSO
		ComPtr<IDxcBlob> injectCSBlob;
		if (compiler.CompileShaderFromFile(shaderPath, L"InjectLightCS", L"cs_6_6", injectCSBlob))
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.CS = { injectCSBlob->GetBufferPointer(), injectCSBlob->GetBufferSize() };
			ThrowIfFailed(pDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_lightInjectionPSO)),
				"Volumetric Fog Light Injection PSO oluşturulamadı");
		}

		// 2. Volume Integration Compute PSO
		ComPtr<IDxcBlob> accumCSBlob;
		if (compiler.CompileShaderFromFile(shaderPath, L"AccumulateCS", L"cs_6_6", accumCSBlob))
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.CS = { accumCSBlob->GetBufferPointer(), accumCSBlob->GetBufferSize() };
			ThrowIfFailed(pDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_volumeIntegrationPSO)),
				"Volumetric Fog Accumulation PSO oluşturulamadı");
		}

		// 3. Composite Graphics PSO (Blends into Scene Color Buffer with Reverse-Z Depth)
		ComPtr<IDxcBlob> vsBlob;
		ComPtr<IDxcBlob> psBlob;
		if (compiler.CompileShaderFromFile(shaderPath, L"FogFullscreenVS", L"vs_6_6", vsBlob) &&
			compiler.CompileShaderFromFile(shaderPath, L"FogCompositePS", L"ps_6_6", psBlob))
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
			psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };

			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

			// Blend: Final = Scene * Transmittance + InScattering
			// SrcBlend = ONE, DestBlend = SRC_ALPHA
			D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			psoDesc.BlendState = blendDesc;

			// Reverse-Z Depth: Read-only, test always passes (full screen composite)
			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = SCENE_COLOR_FORMAT;
			psoDesc.SampleDesc.Count = 1;

			ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_compositePSO)),
				"Volumetric Fog Composite PSO oluşturulamadı");
		}

		return true;
	}

	void CVolumetricFog::Compute(ID3D12GraphicsCommandList7* pCmdList, const XMMATRIX& invViewProj, const XMFLOAT3& cameraPos, uint32_t depthSRVIndex)
	{
		if (!m_lightInjectionPSO || !m_volumeIntegrationPSO)
			return;

		m_constants.depthSRVIndex = depthSRVIndex;

		auto& heap = CBindlessDescriptorHeap::Instance();
		heap.SetDescriptorHeaps(pCmdList);
		pCmdList->SetComputeRootSignature(heap.GetGlobalRootSignature());

		// 1. Dispatch Light Injection: 160 x 90 x 64 -> (20, 12, 64) thread groups of (8, 8, 1)
		pCmdList->SetPipelineState(m_lightInjectionPSO.Get());
		pCmdList->Dispatch(FROXEL_GRID_X / 8, (FROXEL_GRID_Y + 7) / 8, FROXEL_GRID_Z);

		// UAV Barrier between Injection and Accumulation
		D3D12_BARRIER_SYNC syncBefore = D3D12_BARRIER_SYNC_COMPUTE_SHADING;
		D3D12_BARRIER_SYNC syncAfter = D3D12_BARRIER_SYNC_COMPUTE_SHADING;
		D3D12_BARRIER_ACCESS accessBefore = D3D12_BARRIER_ACCESS_UNORDERED_ACCESS;
		D3D12_BARRIER_ACCESS accessAfter = D3D12_BARRIER_ACCESS_SHADER_RESOURCE;

		D3D12_TEXTURE_BARRIER textureBarrier = CD3DX12_TEXTURE_BARRIER(
			syncBefore, syncAfter,
			accessBefore, accessAfter,
			D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS, D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
			m_injectionTexture.Get(),
			CD3DX12_BARRIER_SUBRESOURCE_RANGE(0, 1, 0, 1, 0, 1),
			D3D12_TEXTURE_BARRIER_FLAG_NONE);

		D3D12_BARRIER_GROUP group = CD3DX12_BARRIER_GROUP(1, &textureBarrier);
		pCmdList->Barrier(1, &group);

		// 2. Dispatch Volume Integration: 160 x 90 -> (20, 12, 1) thread groups of (8, 8, 1)
		pCmdList->SetPipelineState(m_volumeIntegrationPSO.Get());
		pCmdList->Dispatch(FROXEL_GRID_X / 8, (FROXEL_GRID_Y + 7) / 8, 1);

		// Barrier: accumulated texture to SHADER_RESOURCE for pixel shader composite
		textureBarrier = CD3DX12_TEXTURE_BARRIER(
			D3D12_BARRIER_SYNC_COMPUTE_SHADING, D3D12_BARRIER_SYNC_PIXEL_SHADING,
			D3D12_BARRIER_ACCESS_UNORDERED_ACCESS, D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
			D3D12_BARRIER_LAYOUT_UNORDERED_ACCESS, D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
			m_accumulatedTexture.Get(),
			CD3DX12_BARRIER_SUBRESOURCE_RANGE(0, 1, 0, 1, 0, 1),
			D3D12_TEXTURE_BARRIER_FLAG_NONE);

		group = CD3DX12_BARRIER_GROUP(1, &textureBarrier);
		pCmdList->Barrier(1, &group);
	}

	void CVolumetricFog::Composite(ID3D12GraphicsCommandList7* pCmdList)
	{
		if (!m_compositePSO)
			return;

		auto& heap = CBindlessDescriptorHeap::Instance();
		pCmdList->SetPipelineState(m_compositePSO.Get());
		pCmdList->SetGraphicsRootSignature(heap.GetGlobalRootSignature());
		heap.SetDescriptorHeaps(pCmdList);

		pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCmdList->DrawInstanced(3, 1, 0, 0);
	}
}
