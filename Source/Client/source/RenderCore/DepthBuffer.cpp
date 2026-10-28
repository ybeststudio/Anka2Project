#include "DepthBuffer.h"
#include "D3D12Device.h"
#include "BindlessDescriptorHeap.h"

namespace RenderCore
{
	CDepthBuffer::~CDepthBuffer()
	{
		Shutdown();
	}

	bool CDepthBuffer::Initialize(uint32_t width, uint32_t height)
	{
		m_width = width;
		m_height = height;

		auto pDevice = CD3D12Device::Instance().GetDevice();

		// Create DSV Descriptor Heap (2 descriptors: 0 = writeable, 1 = read-only)
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 2;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		ThrowIfFailed(pDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)), "DSV Descriptor Heap oluşturulamadı");
		m_dsvDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		// Allocate bindless SRV descriptor
		m_srvIndex = CBindlessDescriptorHeap::Instance().AllocateDescriptor();

		return CreateResources();
	}

	void CDepthBuffer::Shutdown()
	{
		if (m_srvIndex != 0)
		{
			CBindlessDescriptorHeap::Instance().FreeDescriptor(m_srvIndex);
			m_srvIndex = 0;
		}

		m_depthResource.Reset();
		m_allocation.Reset();
		m_dsvHeap.Reset();
	}

	bool CDepthBuffer::CreateResources()
	{
		auto pDevice = CD3D12Device::Instance().GetDevice();
		auto pAllocator = CD3D12Device::Instance().GetAllocator();

		// R32_TYPELESS texture allowing both D32_FLOAT DSV and R32_FLOAT SRV
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = m_width;
		desc.Height = m_height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12MA::ALLOCATION_DESC allocDesc = {};
		allocDesc.HeapType = D3D12_HEAP_TYPE_DEFAULT;

		// Reverse-Z: Clear depth is 0.0f
		D3D12_CLEAR_VALUE clearVal = {};
		clearVal.Format = DXGI_FORMAT_D32_FLOAT;
		clearVal.DepthStencil.Depth = 0.0f;
		clearVal.DepthStencil.Stencil = 0;

		ThrowIfFailed(pAllocator->CreateResource(
			&allocDesc,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearVal,
			&m_allocation,
			IID_PPV_ARGS(&m_depthResource)),
			"Reverse-Z DepthBuffer kaynağı oluşturulamadı");

		// 1. Create Writeable DSV at index 0
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		pDevice->CreateDepthStencilView(m_depthResource.Get(), &dsvDesc, dsvHandle);

		// 2. Create Read-Only DSV at index 1 (for forward lighting pass with EQUAL depth)
		dsvHandle.Offset(1, m_dsvDescriptorSize);
		dsvDesc.Flags = D3D12_DSV_FLAG_READ_ONLY_DEPTH;
		pDevice->CreateDepthStencilView(m_depthResource.Get(), &dsvDesc, dsvHandle);

		// 3. Create Bindless SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		CBindlessDescriptorHeap::Instance().CreateShaderResourceView(m_depthResource.Get(), &srvDesc, m_srvIndex);

		return true;
	}

	void CDepthBuffer::Resize(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
			return;

		m_width = width;
		m_height = height;

		m_depthResource.Reset();
		m_allocation.Reset();

		CreateResources();
	}

	void CDepthBuffer::Clear(ID3D12GraphicsCommandList* pCmdList)
	{
		// Reverse-Z: Clear to 0.0f
		pCmdList->ClearDepthStencilView(GetDSV(), D3D12_CLEAR_FLAG_DEPTH, 0.0f, 0, 0, nullptr);
	}

	void CDepthBuffer::TransitionToDepthWrite(ID3D12GraphicsCommandList7* pCmdList)
	{
		D3D12_TEXTURE_BARRIER barrier = {};
		barrier.SyncBefore = D3D12_BARRIER_SYNC_ALL_SHADING;
		barrier.SyncAfter = D3D12_BARRIER_SYNC_DEPTH_STENCIL;
		barrier.AccessBefore = D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
		barrier.AccessAfter = D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE;
		barrier.LayoutBefore = D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_READ;
		barrier.LayoutAfter = D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE;
		barrier.pResource = m_depthResource.Get();
		barrier.Subresources.IndexOrFirstMipLevel = 0;
		barrier.Subresources.NumMipLevels = 1;
		barrier.Subresources.FirstArraySlice = 0;
		barrier.Subresources.NumArraySlices = 1;
		barrier.Subresources.FirstPlane = 0;
		barrier.Subresources.NumPlanes = 1;
		barrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

		D3D12_BARRIER_GROUP group = {};
		group.Type = D3D12_BARRIER_TYPE_TEXTURE;
		group.NumBarriers = 1;
		group.pTextureBarriers = &barrier;

		pCmdList->Barrier(1, &group);
	}

	void CDepthBuffer::TransitionToDepthRead(ID3D12GraphicsCommandList7* pCmdList)
	{
		D3D12_TEXTURE_BARRIER barrier = {};
		barrier.SyncBefore = D3D12_BARRIER_SYNC_DEPTH_STENCIL;
		barrier.SyncAfter = D3D12_BARRIER_SYNC_ALL_SHADING;
		barrier.AccessBefore = D3D12_BARRIER_ACCESS_DEPTH_STENCIL_WRITE;
		barrier.AccessAfter = D3D12_BARRIER_ACCESS_DEPTH_STENCIL_READ | D3D12_BARRIER_ACCESS_SHADER_RESOURCE;
		barrier.LayoutBefore = D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_WRITE;
		barrier.LayoutAfter = D3D12_BARRIER_LAYOUT_DEPTH_STENCIL_READ;
		barrier.pResource = m_depthResource.Get();
		barrier.Subresources.IndexOrFirstMipLevel = 0;
		barrier.Subresources.NumMipLevels = 1;
		barrier.Subresources.FirstArraySlice = 0;
		barrier.Subresources.NumArraySlices = 1;
		barrier.Subresources.FirstPlane = 0;
		barrier.Subresources.NumPlanes = 1;
		barrier.Flags = D3D12_TEXTURE_BARRIER_FLAG_NONE;

		D3D12_BARRIER_GROUP group = {};
		group.Type = D3D12_BARRIER_TYPE_TEXTURE;
		group.NumBarriers = 1;
		group.pTextureBarriers = &barrier;

		pCmdList->Barrier(1, &group);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CDepthBuffer::GetDSV() const
	{
		return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CDepthBuffer::GetReadOnlyDSV() const
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(1, m_dsvDescriptorSize);
		return handle;
	}

	XMMATRIX CDepthBuffer::CreateReverseZInfiniteProjection(float fovY, float aspect, float nearZ)
	{
		// Reverse-Z Infinite Projection Matrix
		// Maps z in [nearZ, infinity) -> [1.0, 0.0]
		float f = 1.0f / tanf(fovY * 0.5f);
		return XMMatrixSet(
			f / aspect, 0.0f, 0.0f,  0.0f,
			0.0f,       f,    0.0f,  0.0f,
			0.0f,       0.0f, 0.0f,  1.0f,
			0.0f,       0.0f, nearZ, 0.0f
		);
	}
}
