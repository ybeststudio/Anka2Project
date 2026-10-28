#pragma once

#include "D3D12Common.h"
#include <mutex>
#include <queue>

namespace RenderCore
{
	constexpr uint32_t INVALID_DESCRIPTOR_INDEX = 0xFFFFFFFF;

	class CBindlessDescriptorHeap
	{
	public:
		static CBindlessDescriptorHeap& Instance();

		bool Initialize(ID3D12Device10* pDevice);
		void Shutdown();

		// Descriptor Allocation & Deallocation
		uint32_t AllocateDescriptor();
		void FreeDescriptor(uint32_t index);

		// Creating Views into Bindless Heap
		void CreateShaderResourceView(ID3D12Resource* pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc, uint32_t index);
		void CreateUnorderedAccessView(ID3D12Resource* pResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc, uint32_t index);
		void CreateConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc, uint32_t index);

		// Handle Helpers
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(uint32_t index) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t index) const;

		// Bind Heaps to Command List
		void SetDescriptorHeaps(ID3D12GraphicsCommandList* pCmdList);

		// Global Root Signature
		ID3D12RootSignature* GetGlobalRootSignature() const { return m_globalRootSignature.Get(); }

		ID3D12DescriptorHeap* GetHeap() const { return m_resourceHeap.Get(); }
		ID3D12DescriptorHeap* GetSamplerHeap() const { return m_samplerHeap.Get(); }

	private:
		CBindlessDescriptorHeap() = default;
		~CBindlessDescriptorHeap();
		CBindlessDescriptorHeap(const CBindlessDescriptorHeap&) = delete;
		CBindlessDescriptorHeap& operator=(const CBindlessDescriptorHeap&) = delete;

		bool CreateRootSignature(ID3D12Device10* pDevice);

	private:
		bool m_initialized = false;
		ComPtr<ID3D12DescriptorHeap> m_resourceHeap;
		ComPtr<ID3D12DescriptorHeap> m_samplerHeap;
		ComPtr<ID3D12RootSignature> m_globalRootSignature;

		uint32_t m_descriptorSize = 0;
		uint32_t m_samplerDescriptorSize = 0;

		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStartHandle = {};
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStartHandle = {};

		std::mutex m_allocMutex;
		std::queue<uint32_t> m_freeIndices;
		uint32_t m_allocatedCount = 0;
	};
}
