#include "PostProcessing.h"
#include "D3D12Device.h"
#include "BindlessDescriptorHeap.h"
#include "ShaderCompiler.h"

namespace RenderCore
{
	CPostProcessing::~CPostProcessing()
	{
		Shutdown();
	}

	bool CPostProcessing::Initialize(uint32_t width, uint32_t height)
	{
		if (m_initialized)
			return true;

		m_width = width;
		m_height = height;

		// Create RTV heap for Scene Color Target
		auto pDevice = CD3D12Device::Instance().GetDevice();
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = 1;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)),
			"PostProcessing RTV Heap oluşturulamadı");

		m_sceneColorRTV = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

		if (!CreateSceneColorTarget(width, height))
			return false;

		if (!CreatePipelines())
			return false;

		m_initialized = true;
		return true;
	}

	void CPostProcessing::Shutdown()
	{
		auto& heap = CBindlessDescriptorHeap::Instance();

		if (m_sceneColorSRVIndex != 0)
		{
			heap.FreeDescriptor(m_sceneColorSRVIndex);
			m_sceneColorSRVIndex = 0;
		}

		m_tonemapHDR10PSO.Reset();
		m_tonemapSDRPSO.Reset();
		m_sceneColorTexture.Reset();
		m_sceneColorAlloc.Reset();
		m_rtvHeap.Reset();

		m_initialized = false;
	}

	void CPostProcessing::Resize(uint32_t width, uint32_t height)
	{
		if (m_width == width && m_height == height)
			return;

		m_width = width;
		m_height = height;

		if (m_sceneColorSRVIndex != 0)
		{
			CBindlessDescriptorHeap::Instance().FreeDescriptor(m_sceneColorSRVIndex);
			m_sceneColorSRVIndex = 0;
		}
		m_sceneColorTexture.Reset();
		m_sceneColorAlloc.Reset();

		CreateSceneColorTarget(width, height);
	}

	bool CPostProcessing::CreateSceneColorTarget(uint32_t width, uint32_t height)
	{
		auto pAllocator = CD3D12Device::Instance().GetAllocator();
		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto& heap = CBindlessDescriptorHeap::Instance();

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = width;
		desc.Height = height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = SCENE_COLOR_FORMAT; // R11G11B10_FLOAT
		desc.SampleDesc.Count = 1;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE clearValue = {};
		clearValue.Format = SCENE_COLOR_FORMAT;
		clearValue.Color[0] = 0.0f;
		clearValue.Color[1] = 0.0f;
		clearValue.Color[2] = 0.0f;
		clearValue.Color[3] = 1.0f;

		ThrowIfFailed(pAllocator->CreateResource(
			&allocDesc,
			&desc,
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			&clearValue,
			&m_sceneColorAlloc,
			IID_PPV_ARGS(&m_sceneColorTexture)),
			"Scene Color Texture oluşturulamadı");

		// Create RTV
		pDevice->CreateRenderTargetView(m_sceneColorTexture.Get(), nullptr, m_sceneColorRTV);

		// Create SRV in Bindless Heap
		m_sceneColorSRVIndex = heap.AllocateDescriptor();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		heap.CreateShaderResourceView(m_sceneColorTexture.Get(), &srvDesc, m_sceneColorSRVIndex);

		m_constants.sceneColorSRVIndex = m_sceneColorSRVIndex;

		return true;
	}

	bool CPostProcessing::CreatePipelines()
	{
		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto pRootSig = CBindlessDescriptorHeap::Instance().GetGlobalRootSignature();
		auto& compiler = CShaderCompiler::Instance();

		std::wstring shaderPath = L"source/RenderCore/Shaders/PostProcessing.hlsl";

		ComPtr<IDxcBlob> vsBlob;
		ComPtr<IDxcBlob> psSDRBlob;
		ComPtr<IDxcBlob> psHDRBlob;

		if (!compiler.CompileShaderFromFile(shaderPath, L"FullscreenVS", L"vs_6_6", vsBlob))
			return false;

		// 1. SDR Tonemap PSO (R8G8B8A8_UNORM)
		if (compiler.CompileShaderFromFile(shaderPath, L"TonemapSDR_PS", L"ps_6_6", psSDRBlob))
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
			psoDesc.PS = { psSDRBlob->GetBufferPointer(), psSDRBlob->GetBufferSize() };

			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = BACKBUFFER_FORMAT_SDR;
			psoDesc.SampleDesc.Count = 1;

			ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_tonemapSDRPSO)),
				"PostProcessing SDR PSO oluşturulamadı");
		}

		// 2. HDR10 Tonemap PSO (R16G16B16A16_FLOAT)
		if (compiler.CompileShaderFromFile(shaderPath, L"TonemapHDR10_PS", L"ps_6_6", psHDRBlob))
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
			psoDesc.PS = { psHDRBlob->GetBufferPointer(), psHDRBlob->GetBufferSize() };

			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

			psoDesc.DepthStencilState.DepthEnable = FALSE;
			psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = BACKBUFFER_FORMAT_HDR;
			psoDesc.SampleDesc.Count = 1;

			ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_tonemapHDR10PSO)),
				"PostProcessing HDR10 PSO oluşturulamadı");
		}

		return true;
	}

	void CPostProcessing::Render(ID3D12GraphicsCommandList7* pCmdList,
		D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTV,
		DXGI_FORMAT backBufferFormat,
		bool isHDR10)
	{
		ID3D12PipelineState* pso = (isHDR10 && m_tonemapHDR10PSO) ? m_tonemapHDR10PSO.Get() : m_tonemapSDRPSO.Get();
		if (!pso)
			return;

		m_constants.isHDR10 = isHDR10 ? 1 : 0;
		m_constants.sceneColorSRVIndex = m_sceneColorSRVIndex;

		// Enhanced Barrier: Transition Scene Color Texture from RENDER_TARGET to SHADER_RESOURCE
		D3D12_TEXTURE_BARRIER barrier = CD3DX12_TEXTURE_BARRIER(
			D3D12_BARRIER_SYNC_RENDER_TARGET, D3D12_BARRIER_SYNC_PIXEL_SHADING,
			D3D12_BARRIER_ACCESS_RENDER_TARGET, D3D12_BARRIER_ACCESS_SHADER_RESOURCE,
			D3D12_BARRIER_LAYOUT_RENDER_TARGET, D3D12_BARRIER_LAYOUT_SHADER_RESOURCE,
			m_sceneColorTexture.Get(),
			CD3DX12_BARRIER_SUBRESOURCE_RANGE(0, 1, 0, 1, 0, 1),
			D3D12_TEXTURE_BARRIER_FLAG_NONE);

		D3D12_BARRIER_GROUP group = CD3DX12_BARRIER_GROUP(1, &barrier);
		pCmdList->Barrier(1, &group);

		// Render to Swapchain BackBuffer
		auto& heap = CBindlessDescriptorHeap::Instance();
		pCmdList->SetPipelineState(pso);
		pCmdList->SetGraphicsRootSignature(heap.GetGlobalRootSignature());
		heap.SetDescriptorHeaps(pCmdList);

		// Set constants at root constant (register b0)
		pCmdList->SetGraphicsRoot32BitConstants(0, sizeof(m_constants) / 4, &m_constants, 0);

		pCmdList->OMSetRenderTargets(1, &backBufferRTV, FALSE, nullptr);

		D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, 1.0f };
		D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
		pCmdList->RSSetViewports(1, &viewport);
		pCmdList->RSSetScissorRects(1, &scissorRect);

		pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCmdList->DrawInstanced(3, 1, 0, 0);

		// Enhanced Barrier: Transition Scene Color Texture back to RENDER_TARGET for next frame
		barrier = CD3DX12_TEXTURE_BARRIER(
			D3D12_BARRIER_SYNC_PIXEL_SHADING, D3D12_BARRIER_SYNC_RENDER_TARGET,
			D3D12_BARRIER_ACCESS_SHADER_RESOURCE, D3D12_BARRIER_ACCESS_RENDER_TARGET,
			D3D12_BARRIER_LAYOUT_SHADER_RESOURCE, D3D12_BARRIER_LAYOUT_RENDER_TARGET,
			m_sceneColorTexture.Get(),
			CD3DX12_BARRIER_SUBRESOURCE_RANGE(0, 1, 0, 1, 0, 1),
			D3D12_TEXTURE_BARRIER_FLAG_NONE);

		group = CD3DX12_BARRIER_GROUP(1, &barrier);
		pCmdList->Barrier(1, &group);
	}
}
