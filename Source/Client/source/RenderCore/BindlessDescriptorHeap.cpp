#include "BindlessDescriptorHeap.h"
#include "D3D12Device.h"

namespace RenderCore
{
	CBindlessDescriptorHeap& CBindlessDescriptorHeap::Instance()
	{
		static CBindlessDescriptorHeap s_instance;
		return s_instance;
	}

	CBindlessDescriptorHeap::~CBindlessDescriptorHeap()
	{
		Shutdown();
	}

	bool CBindlessDescriptorHeap::Initialize(ID3D12Device10* pDevice)
	{
		if (m_initialized)
			return true;

		// 1. Create Shader-Visible Resource Descriptor Heap (CBV, SRV, UAV)
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = MAX_BINDLESS_RESOURCES;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NodeMask = 0;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_resourceHeap)), "Bindless Resource Descriptor Heap oluşturulamadı");

		// 2. Create Shader-Visible Sampler Descriptor Heap
		D3D12_DESCRIPTOR_HEAP_DESC samplerHeapDesc = {};
		samplerHeapDesc.NumDescriptors = MAX_BINDLESS_SAMPLERS;
		samplerHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		samplerHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		samplerHeapDesc.NodeMask = 0;
		ThrowIfFailed(pDevice->CreateDescriptorHeap(&samplerHeapDesc, IID_PPV_ARGS(&m_samplerHeap)), "Bindless Sampler Descriptor Heap oluşturulamadı");

		m_descriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		m_samplerDescriptorSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		m_cpuStartHandle = m_resourceHeap->GetCPUDescriptorHandleForHeapStart();
		m_gpuStartHandle = m_resourceHeap->GetGPUDescriptorHandleForHeapStart();

		// Index 0 is reserved as Null / Dummy descriptor
		m_allocatedCount = 1;

		if (!CreateRootSignature(pDevice))
			return false;

		m_initialized = true;
		return true;
	}

	void CBindlessDescriptorHeap::Shutdown()
	{
		std::lock_guard<std::mutex> lock(m_allocMutex);
		while (!m_freeIndices.empty())
			m_freeIndices.pop();

		m_allocatedCount = 0;
		m_globalRootSignature.Reset();
		m_samplerHeap.Reset();
		m_resourceHeap.Reset();
		m_initialized = false;
	}

	uint32_t CBindlessDescriptorHeap::AllocateDescriptor()
	{
		std::lock_guard<std::mutex> lock(m_allocMutex);

		if (!m_freeIndices.empty())
		{
			uint32_t idx = m_freeIndices.front();
			m_freeIndices.pop();
			return idx;
		}

		if (m_allocatedCount >= MAX_BINDLESS_RESOURCES)
		{
			ThrowIfFailed(E_OUTOFMEMORY, "Bindless Descriptor Heap kapasitesi doldu (65536 aşıldı)");
		}

		return m_allocatedCount++;
	}

	void CBindlessDescriptorHeap::FreeDescriptor(uint32_t index)
	{
		if (index == 0 || index == INVALID_DESCRIPTOR_INDEX)
			return;

		std::lock_guard<std::mutex> lock(m_allocMutex);
		m_freeIndices.push(index);
	}

	void CBindlessDescriptorHeap::CreateShaderResourceView(ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc, uint32_t index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = GetCPUHandle(index);
		CD3D12Device::Instance().GetDevice()->CreateShaderResourceView(pResource, pDesc, handle);
	}

	void CBindlessDescriptorHeap::CreateUnorderedAccessView(ID3D12Resource* pResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc, uint32_t index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = GetCPUHandle(index);
		CD3D12Device::Instance().GetDevice()->CreateUnorderedAccessView(pResource, nullptr, pDesc, handle);
	}

	void CBindlessDescriptorHeap::CreateConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc, uint32_t index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = GetCPUHandle(index);
		CD3D12Device::Instance().GetDevice()->CreateConstantBufferView(pDesc, handle);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE CBindlessDescriptorHeap::GetCPUHandle(uint32_t index) const
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cpuStartHandle;
		handle.ptr += static_cast<SIZE_T>(index) * m_descriptorSize;
		return handle;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE CBindlessDescriptorHeap::GetGPUHandle(uint32_t index) const
	{
		D3D12_GPU_DESCRIPTOR_HANDLE handle = m_gpuStartHandle;
		handle.ptr += static_cast<UINT64>(index) * m_descriptorSize;
		return handle;
	}

	void CBindlessDescriptorHeap::SetDescriptorHeaps(ID3D12GraphicsCommandList* pCmdList)
	{
		ID3D12DescriptorHeap* ppHeaps[] = { m_resourceHeap.Get(), m_samplerHeap.Get() };
		pCmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	}

	bool CBindlessDescriptorHeap::CreateRootSignature(ID3D12Device10* pDevice)
	{
		// Common Global Root Signature for SM 6.6 Bindless Rendering & Compute
		// Root Parameters:
		// 0: RootConstants (16 DWORDS) at register(b0)
		// 1: Root CBV at register(b1) -> Camera & Frame Globals
		// 2: Root CBV at register(b2) -> Atmosphere & Sky
		// 3: Root CBV at register(b3) -> Deform & Object Data
		// 4: Root SRV at register(t0) -> TLAS (Raytracing Acceleration Structure)

		CD3DX12_ROOT_PARAMETER1 rootParameters[5];
		rootParameters[0].InitAsConstants(16, 0, 0, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[2].InitAsConstantBufferView(2, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[3].InitAsConstantBufferView(3, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);
		rootParameters[4].InitAsShaderResourceView(0, 0, D3D12_ROOT_DESCRIPTOR_FLAG_DATA_STATIC, D3D12_SHADER_VISIBILITY_ALL);

		// Static Samplers
		CD3DX12_STATIC_SAMPLER_DESC staticSamplers[5];
		// s0: Linear Wrap
		staticSamplers[0].Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);
		// s1: Linear Clamp
		staticSamplers[1].Init(1, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
		// s2: Point Clamp
		staticSamplers[2].Init(2, D3D12_FILTER_MIN_MAG_MIP_POINT, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP);
		// s3: Anisotropic Wrap
		staticSamplers[3].Init(3, D3D12_FILTER_ANISOTROPIC, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, 0.0f, 16);
		// s4: Reverse-Z Shadow Comparison Sampler (GREATER_EQUAL)
		staticSamplers[4].Init(4, D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP, D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			0.0f, 1, D3D12_COMPARISON_FUNC_GREATER_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSigDesc;
		D3D12_ROOT_SIGNATURE_FLAGS rootSigFlags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
			D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;

		rootSigDesc.Init_1_1(_countof(rootParameters), rootParameters, _countof(staticSamplers), staticSamplers, rootSigFlags);

		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &signatureBlob, &errorBlob);
		if (FAILED(hr))
		{
			if (errorBlob)
			{
				::MessageBoxA(nullptr, static_cast<const char*>(errorBlob->GetBufferPointer()), "Root Signature Serialize Hatası", MB_ICONERROR | MB_OK);
			}
			return false;
		}

		ThrowIfFailed(pDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_globalRootSignature)),
			"Global Bindless Root Signature oluşturulamadı");

		return true;
	}
}
