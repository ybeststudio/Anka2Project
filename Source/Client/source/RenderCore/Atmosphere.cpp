#include "Atmosphere.h"
#include "D3D12Device.h"
#include "BindlessDescriptorHeap.h"
#include "ShaderCompiler.h"
#include <algorithm>

namespace RenderCore
{
	CAtmosphere::~CAtmosphere()
	{
		Shutdown();
	}

	bool CAtmosphere::Initialize()
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

	void CAtmosphere::Shutdown()
	{
		auto& heap = CBindlessDescriptorHeap::Instance();

		if (m_transmittanceLUTIndex != 0) heap.FreeDescriptor(m_transmittanceLUTIndex);
		if (m_transmittanceUAVIndex != 0) heap.FreeDescriptor(m_transmittanceUAVIndex);
		if (m_multiScatterLUTIndex != 0) heap.FreeDescriptor(m_multiScatterLUTIndex);
		if (m_multiScatterUAVIndex != 0) heap.FreeDescriptor(m_multiScatterUAVIndex);
		if (m_skyViewLUTIndex != 0) heap.FreeDescriptor(m_skyViewLUTIndex);
		if (m_skyViewUAVIndex != 0) heap.FreeDescriptor(m_skyViewUAVIndex);

		m_transmittanceLUT.Reset();
		m_transmittanceAlloc.Reset();
		m_multiScatterLUT.Reset();
		m_multiScatterAlloc.Reset();
		m_skyViewLUT.Reset();
		m_skyViewAlloc.Reset();

		m_renderSkyPSO.Reset();
		m_skyViewPSO.Reset();
		m_multiScatterPSO.Reset();
		m_transmittancePSO.Reset();

		m_initialized = false;
	}

	bool CAtmosphere::CreateTexturesAndDescriptors()
	{
		auto pAllocator = CD3D12Device::Instance().GetAllocator();
		auto& heap = CBindlessDescriptorHeap::Instance();

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		// 1. Transmittance LUT (256x64, R16G16B16A16_FLOAT)
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Width = 256;
		desc.Height = 64;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		ThrowIfFailed(pAllocator->CreateResource(&allocDesc, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, &m_transmittanceAlloc, IID_PPV_ARGS(&m_transmittanceLUT)),
			"Transmittance LUT kaynağı oluşturulamadı");

		m_transmittanceLUTIndex = heap.AllocateDescriptor();
		m_transmittanceUAVIndex = heap.AllocateDescriptor();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		heap.CreateShaderResourceView(m_transmittanceLUT.Get(), &srvDesc, m_transmittanceLUTIndex);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = desc.Format;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		heap.CreateUnorderedAccessView(m_transmittanceLUT.Get(), &uavDesc, m_transmittanceUAVIndex);

		// 2. MultiScatter LUT (32x32, R16G16B16A16_FLOAT)
		desc.Width = 32;
		desc.Height = 32;
		ThrowIfFailed(pAllocator->CreateResource(&allocDesc, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, &m_multiScatterAlloc, IID_PPV_ARGS(&m_multiScatterLUT)),
			"MultiScatter LUT kaynağı oluşturulamadı");

		m_multiScatterLUTIndex = heap.AllocateDescriptor();
		m_multiScatterUAVIndex = heap.AllocateDescriptor();
		heap.CreateShaderResourceView(m_multiScatterLUT.Get(), &srvDesc, m_multiScatterLUTIndex);
		heap.CreateUnorderedAccessView(m_multiScatterLUT.Get(), &uavDesc, m_multiScatterUAVIndex);

		// 3. Sky-View LUT (192x108, R11G11B10_FLOAT)
		desc.Width = 192;
		desc.Height = 108;
		desc.Format = DXGI_FORMAT_R11G11B10_FLOAT;
		ThrowIfFailed(pAllocator->CreateResource(&allocDesc, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, &m_skyViewAlloc, IID_PPV_ARGS(&m_skyViewLUT)),
			"SkyView LUT kaynağı oluşturulamadı");

		m_skyViewLUTIndex = heap.AllocateDescriptor();
		m_skyViewUAVIndex = heap.AllocateDescriptor();
		srvDesc.Format = desc.Format;
		uavDesc.Format = desc.Format;
		heap.CreateShaderResourceView(m_skyViewLUT.Get(), &srvDesc, m_skyViewLUTIndex);
		heap.CreateUnorderedAccessView(m_skyViewLUT.Get(), &uavDesc, m_skyViewUAVIndex);

		m_params.padding0.x = static_cast<float>(m_transmittanceUAVIndex);
		m_params.padding0.y = static_cast<float>(m_skyViewUAVIndex);

		return true;
	}

	bool CAtmosphere::CreatePipelines()
	{
		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto& compiler = CShaderCompiler::Instance();
		auto pRootSig = CBindlessDescriptorHeap::Instance().GetGlobalRootSignature();

		std::wstring shaderPath = L"source/RenderCore/Shaders/Atmosphere.hlsl";

		// 1. Transmittance CS
		ComPtr<IDxcBlob> csBlob;
		if (compiler.CompileShaderFromFile(shaderPath, L"ComputeTransmittanceCS", L"cs_6_6", csBlob))
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.CS = { csBlob->GetBufferPointer(), csBlob->GetBufferSize() };
			ThrowIfFailed(pDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_transmittancePSO)), "Transmittance PSO oluşturulamadı");
		}

		// 2. Sky-View CS
		if (compiler.CompileShaderFromFile(shaderPath, L"ComputeSkyViewCS", L"cs_6_6", csBlob))
		{
			D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.CS = { csBlob->GetBufferPointer(), csBlob->GetBufferSize() };
			ThrowIfFailed(pDevice->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&m_skyViewPSO)), "SkyView PSO oluşturulamadı");
		}

		// 3. RenderSky Graphics PSO (Reverse-Z Infinite Sky Dome)
		ComPtr<IDxcBlob> vsBlob;
		ComPtr<IDxcBlob> psBlob;
		if (compiler.CompileShaderFromFile(shaderPath, L"FullscreenVS", L"vs_6_6", vsBlob) &&
			compiler.CompileShaderFromFile(shaderPath, L"RenderSkyPS", L"ps_6_6", psBlob))
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
			psoDesc.pRootSignature = pRootSig;
			psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
			psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };

			// Rasterizer: CULL_NONE
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

			// Blend: Opaque
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

			// Depth: EQUAL test at 0.0f (Reverse-Z far plane)
			psoDesc.DepthStencilState.DepthEnable = TRUE;
			psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
			psoDesc.DepthStencilState.StencilEnable = FALSE;

			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = SCENE_COLOR_FORMAT;
			psoDesc.DSVFormat = DEPTH_FORMAT;
			psoDesc.SampleDesc.Count = 1;

			ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_renderSkyPSO)), "RenderSky PSO oluşturulamadı");
		}

		return true;
	}

	void CAtmosphere::Update(float deltaTime, float timeOfDay)
	{
		m_params.timeOfDay = timeOfDay;

		// Calculate diurnal sun elevation and azimuth
		float sunHour = (timeOfDay / 24.0f) * XM_2PI - XM_PIDIV2;
		float sunElev = sinf(sunHour);
		float sunAzim = cosf(sunHour);

		XMVECTOR sunDir = XMVector3Normalize(XMVectorSet(sunAzim * 0.7071f, std::max(sunElev, -0.2f), sunAzim * 0.7071f, 0.0f));
		XMStoreFloat3(&m_params.sunDirection, sunDir);

		// Moon is opposite to the sun
		XMVECTOR moonDir = XMVectorScale(sunDir, -1.0f);
		XMStoreFloat3(&m_params.moonDirection, moonDir);

		m_params.sunIntensity = std::max(0.0f, sunElev * 25.0f);
		m_params.moonIntensity = std::max(0.05f, -sunElev * 0.5f);
	}

	void CAtmosphere::RenderLUTs(ID3D12GraphicsCommandList7* pCmdList)
	{
		if (!m_transmittancePSO || !m_skyViewPSO)
			return;

		auto& heap = CBindlessDescriptorHeap::Instance();
		heap.SetDescriptorHeaps(pCmdList);
		pCmdList->SetComputeRootSignature(heap.GetGlobalRootSignature());

		// 1. Dispatch Transmittance LUT (256x64 -> 32x8 threads)
		pCmdList->SetPipelineState(m_transmittancePSO.Get());
		pCmdList->Dispatch(32, 8, 1);

		// 2. Dispatch Sky-View LUT (192x108 -> 24x14 threads)
		pCmdList->SetPipelineState(m_skyViewPSO.Get());
		pCmdList->Dispatch(24, 14, 1);
	}

	void CAtmosphere::RenderSky(ID3D12GraphicsCommandList7* pCmdList, const XMMATRIX& invViewProj, const XMFLOAT3& cameraPos)
	{
		if (!m_renderSkyPSO)
			return;

		auto& heap = CBindlessDescriptorHeap::Instance();
		pCmdList->SetPipelineState(m_renderSkyPSO.Get());
		pCmdList->SetGraphicsRootSignature(heap.GetGlobalRootSignature());
		heap.SetDescriptorHeaps(pCmdList);

		pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCmdList->DrawInstanced(3, 1, 0, 0);
	}
}
