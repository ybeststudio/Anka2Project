#include "RenderEngine.h"
#include "ShaderCompiler.h"
#include "RaytracingManager.h"
#include "ClusteredLighting.h"
#include "ComputeDeform.h"
#include "GpuScene.h"
#include "GltfPipeline.h"

namespace RenderCore
{
	CRenderEngine& CRenderEngine::Instance()
	{
		static CRenderEngine instance;
		return instance;
	}

	CRenderEngine::~CRenderEngine()
	{
		Shutdown();
	}

	bool CRenderEngine::Initialize(HWND hWnd, uint32_t width, uint32_t height)
	{
		if (m_initialized)
			return true;

		m_hWnd = hWnd;
		m_width = width;
		m_height = height;

		// 1. Initialize Device, Queues, Allocator
		if (!CD3D12Device::Instance().Initialize(hWnd, width, height))
		{
			return false;
		}

		auto pDevice = CD3D12Device::Instance().GetDevice();

		// 2. Initialize Bindless Descriptor Heap & Root Signature
		if (!CBindlessDescriptorHeap::Instance().Initialize(pDevice))
		{
			return false;
		}

		// 3. Create Swapchain
		m_swapchain = std::make_unique<CSwapchain>();
		if (!m_swapchain->Initialize(hWnd, width, height, false))
		{
			return false;
		}

		// 4. Create Reverse-Z Floating Point Depth Buffer
		m_depthBuffer = std::make_unique<CDepthBuffer>();
		if (!m_depthBuffer->Initialize(width, height))
		{
			return false;
		}

		// 5. Create Command Allocators & Command List
		for (uint32_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			ThrowIfFailed(pDevice->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(&m_commandAllocators[i])),
				"Command Allocator oluşturulamadı");
		}

		ThrowIfFailed(pDevice->CreateCommandList1(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			D3D12_COMMAND_LIST_FLAG_NONE,
			IID_PPV_ARGS(&m_commandList)),
			"Direct Command List oluşturulamadı");

		// 6. Create Frame Constant Buffer (aligned to 256 bytes)
		if (!CreateFrameConstantBuffer())
		{
			return false;
		}

		// Initialize Shader Compiler & Pipeline Library
		CShaderCompiler::Instance().Initialize();
		CShaderCompiler::Instance().InitializePipelineLibrary(L"shadercache/pso_cache.bin");

		// 7. Subsystem initializations
		m_atmosphere = std::make_unique<CAtmosphere>();
		m_atmosphere->Initialize();

		m_volumetricFog = std::make_unique<CVolumetricFog>();
		m_volumetricFog->Initialize();

		m_postProcessing = std::make_unique<CPostProcessing>();
		m_postProcessing->Initialize(width, height);

		CRaytracingManager::Instance().Initialize();
		CClusteredLighting::Instance().Initialize();
		CComputeDeform::Instance().Initialize();
		CGpuScene::Instance().Initialize(width, height);

		// Set default camera
		SetCamera(XMFLOAT3(0.0f, 20.0f, -50.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMConvertToRadians(60.0f), static_cast<float>(width) / static_cast<float>(height), 0.5f);

		m_initialized = true;
		return true;
	}

	void CRenderEngine::Shutdown()
	{
		if (!m_initialized)
			return;

		CD3D12Device::Instance().FlushGPU();

		CGpuScene::Instance().Shutdown();
		CComputeDeform::Instance().Shutdown();
		CClusteredLighting::Instance().Shutdown();
		CRaytracingManager::Instance().Shutdown();

		if (m_pFrameConstantData && m_frameConstantBuffer)
		{
			m_frameConstantBuffer->Unmap(0, nullptr);
			m_pFrameConstantData = nullptr;
		}
		m_frameConstantBuffer.Reset();
		m_frameConstantAlloc.Reset();

		m_commandList.Reset();
		for (uint32_t i = 0; i < FRAME_BUFFER_COUNT; ++i)
		{
			m_commandAllocators[i].Reset();
		}

		m_postProcessing.reset();
		m_volumetricFog.reset();
		m_atmosphere.reset();
		m_depthBuffer.reset();
		m_swapchain.reset();

		CBindlessDescriptorHeap::Instance().Shutdown();
		CD3D12Device::Instance().Shutdown();

		m_initialized = false;
	}

	void CRenderEngine::Resize(uint32_t width, uint32_t height)
	{
		if (!m_initialized || (m_width == width && m_height == height))
			return;

		CD3D12Device::Instance().FlushGPU();

		m_width = width;
		m_height = height;

		m_swapchain->Resize(width, height);
		m_depthBuffer->Resize(width, height);
		m_postProcessing->Resize(width, height);
		CGpuScene::Instance().Resize(width, height);

		m_frameConstants.renderResolution = XMFLOAT2(static_cast<float>(width), static_cast<float>(height));
		m_frameConstants.invRenderResolution = XMFLOAT2(1.0f / static_cast<float>(width), 1.0f / static_cast<float>(height));
	}

	bool CRenderEngine::CreateFrameConstantBuffer()
	{
		auto pAllocator = CD3D12Device::Instance().GetAllocator();

		D3D12MA::ALLOCATION_DESC uploadAllocDesc = {};
		uploadAllocDesc.HeapType = D3D12_HEAP_TYPE_UPLOAD;

		D3D12_RESOURCE_DESC cbDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(FrameConstants));

		ThrowIfFailed(pAllocator->CreateResource(
			&uploadAllocDesc,
			&cbDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			&m_frameConstantAlloc,
			IID_PPV_ARGS(&m_frameConstantBuffer)),
			"Frame Constant Buffer oluşturulamadı");

		ThrowIfFailed(m_frameConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_pFrameConstantData)),
			"Frame Constant Buffer Map başarısız");

		return true;
	}

	void CRenderEngine::UpdateFrameConstantBuffer()
	{
		if (m_pFrameConstantData)
		{
			memcpy(m_pFrameConstantData, &m_frameConstants, sizeof(FrameConstants));
		}
	}

	void CRenderEngine::SetCamera(const XMFLOAT3& position, const XMFLOAT3& target, const XMFLOAT3& up, float fovY, float aspectRatio, float nearPlane)
	{
		m_frameConstants.cameraPosition = position;
		m_frameConstants.nearPlane = nearPlane;
		m_frameConstants.farPlane = 5000.0f; // Infinite projection far reference

		XMVECTOR pos = XMLoadFloat3(&position);
		XMVECTOR tgt = XMLoadFloat3(&target);
		XMVECTOR u = XMLoadFloat3(&up);

		XMVECTOR forward = XMVector3Normalize(tgt - pos);
		XMStoreFloat3(&m_frameConstants.cameraDirection, forward);

		XMMATRIX view = XMMatrixLookAtLH(pos, tgt, u);
		// Reverse-Z Infinite Perspective Projection Matrix
		XMMATRIX proj = CDepthBuffer::CreateReverseZInfiniteProjection(fovY, aspectRatio, nearPlane);
		XMMATRIX viewProj = XMMatrixMultiply(view, proj);

		XMVECTOR det;
		XMMATRIX invView = XMMatrixInverse(&det, view);
		XMMATRIX invProj = XMMatrixInverse(&det, proj);
		XMMATRIX invViewProj = XMMatrixInverse(&det, viewProj);

		m_frameConstants.viewMatrix = view;
		m_frameConstants.projMatrix = proj;
		m_frameConstants.viewProjMatrix = viewProj;
		m_frameConstants.invViewMatrix = invView;
		m_frameConstants.invProjMatrix = invProj;
		m_frameConstants.invViewProjMatrix = invViewProj;

		m_frameConstants.renderResolution = XMFLOAT2(static_cast<float>(m_width), static_cast<float>(m_height));
		m_frameConstants.invRenderResolution = XMFLOAT2(1.0f / static_cast<float>(m_width), 1.0f / static_cast<float>(m_height));
	}

	void CRenderEngine::BeginFrame(float deltaTime, float timeOfDay)
	{
		if (!m_initialized)
			return;

		m_frameIndex++;
		m_totalTime += deltaTime;

		m_frameConstants.deltaTime = deltaTime;
		m_frameConstants.time = m_totalTime;
		m_frameConstants.frameIndex = m_frameIndex;
		m_frameConstants.sceneColorSRV = m_postProcessing->GetSceneColorSRVIndex();
		m_frameConstants.sceneDepthSRV = m_depthBuffer->GetSRVIndex();
		m_frameConstants.fogAccumulatedSRV = m_volumetricFog->GetAccumulatedSRVIndex();
		m_frameConstants.tlasIndex = CRaytracingManager::Instance().GetTLASDescriptorIndex();

		UpdateFrameConstantBuffer();

		// Update diurnal sky cycle
		m_atmosphere->Update(deltaTime, timeOfDay);

		// Reset frame allocator and prepare command list
		uint32_t slot = m_frameIndex % FRAME_BUFFER_COUNT;
		m_commandAllocators[slot]->Reset();
		m_commandList->Reset(m_commandAllocators[slot].Get(), nullptr);

		m_swapchain->BeginFrame(m_commandList.Get());
	}

	void CRenderEngine::RenderFrame()
	{
		if (!m_initialized)
			return;

		auto pCmdList = m_commandList.Get();
		auto& heap = CBindlessDescriptorHeap::Instance();

		// Set Global Heaps and Root Signature
		heap.SetDescriptorHeaps(pCmdList);
		pCmdList->SetGraphicsRootSignature(heap.GetGlobalRootSignature());
		pCmdList->SetComputeRootSignature(heap.GetGlobalRootSignature());

		// Bind Frame Constants at register(b1)
		D3D12_GPU_VIRTUAL_ADDRESS frameCBVAddr = m_frameConstantBuffer->GetGPUVirtualAddress();
		pCmdList->SetGraphicsRootConstantBufferView(1, frameCBVAddr);
		pCmdList->SetComputeRootConstantBufferView(1, frameCBVAddr);

		// 1. Atmosphere LUT computation passes
		m_atmosphere->RenderLUTs(pCmdList);

		// 2. Clustered lighting update & light culling compute passes
		CClusteredLighting::Instance().UpdateAndCull(pCmdList, m_frameConstants.viewMatrix, m_frameConstants.projMatrix);

		// 3. Clear Reverse-Z Depth Buffer (0.0f = far) and Scene HDR Color
		m_depthBuffer->Clear(pCmdList);

		D3D12_CPU_DESCRIPTOR_HANDLE rtv = m_postProcessing->GetSceneColorRTV();
		float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		pCmdList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

		// Set Scene Render Target & Depth Target
		D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_depthBuffer->GetDSV();
		pCmdList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, 1.0f };
		D3D12_RECT scissor = { 0, 0, static_cast<LONG>(m_width), static_cast<LONG>(m_height) };
		pCmdList->RSSetViewports(1, &viewport);
		pCmdList->RSSetScissorRects(1, &scissor);

		// 4. Render Sky Dome (Reverse-Z EQUAL depth at 0.0f)
		m_atmosphere->RenderSky(pCmdList, m_frameConstants.invViewProjMatrix, m_frameConstants.cameraPosition);

		// 5. Compute Froxel Volumetric Fog (160 x 90 x 64)
		m_volumetricFog->Compute(pCmdList, m_frameConstants.invViewProjMatrix, m_frameConstants.cameraPosition, m_depthBuffer->GetSRVIndex());

		// 6. Composite Volumetric Fog into Scene HDR Target
		m_volumetricFog->Composite(pCmdList);

		// 7. Post-Processing & ACES Tonemapping to Swapchain BackBuffer
		D3D12_CPU_DESCRIPTOR_HANDLE backBufferRTV = m_swapchain->GetCurrentRTV();
		DXGI_FORMAT backBufferFormat = m_swapchain->GetFormat();
		bool isHDR10 = m_swapchain->IsHDREnabled();

		m_postProcessing->Render(pCmdList, backBufferRTV, backBufferFormat, isHDR10);
	}

	void CRenderEngine::EndFrame()
	{
		if (!m_initialized)
			return;

		m_swapchain->EndFrame(m_commandList.Get());

		CD3D12Device::Instance().ExecuteCommandList(m_commandList.Get());

		m_swapchain->Present(true);
	}
}
